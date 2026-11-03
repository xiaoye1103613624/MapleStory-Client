# 波3 · 占位渲染还原 —— 取证与对账单

日期：2026-11-03 · 工具：`_tools/summon_probe`（自研 nlnx 探针，镜像客户端 ImgSource
的 `open_merge` 装载路径，自动密钥探测；只读）· 产物：`_out/summon_probe_full.txt`

## 探针取证结论（推翻两个先前假设）

### 1. 召唤兽真对象化：动画在 Skill.img，不在 Mob.nx
`Skill/<ns>.img/skill/<id>/summon` 子节点即完整状态机（实测 3111002/3211002/2321003 等）：

| 状态 | 帧数 | 锚点 origin | delay |
| --- | --- | --- | --- |
| stand | 8 | (35,96) | 500 |
| hit | 4 | (35,96) | 150 |
| die | 1 | (35,96) | 500 |
| summoned | 12 | 首帧 7×7/3,69 | 90（入场闪光） |

- **修前根因**：`FieldMarker::try_load_anim` 读 `summon["0"]`（该节点不存在）⇒
  落到 `summoned["0"]`（7×7 占位小帧）⇒ 召唤兽"看不见/小碎块"。
- **修后**：`MapSummon` 自持 `stand/hit/die/summoned` 状态机；
  REMOVE_SPECIAL_MAPOBJECT `anim==4`（s9 PC:1194 animated）→ `play_die()` 播 die 后自清；
  SPAWN_SPECIAL_MAPOBJECT `animated` → `play_spawn()` 播 summoned 入场闪光；
  DAMAGE_SUMMON → `apply_damage` 播 hit。
- 原生客户端同样**不存在召唤兽本地攻击动画**（帧集没有 attack）——
  SummonAttack 包（PC:2323）里 dir/count 是服务端算好的伤害列表，
  客户端只需驱动受击方表现；本实现保持该语义。

### 2. 反应堆音效路径是 `{rid}/{state}/Hit`，不是 `{rid}/Hit`
实测 `Sound/Reactor.img`：

```
rid 1020000:
  state 0..3 → children: Hit(audio)      ← 每个状态一条
  state 4    → 空
```

- catalog 键名**不补零**（"2000" 而非 "0002000"）⇒ 用 `std::to_string(rid)` 对齐。
- IDA `sub_989ADD` 的 `"Sound/Reactor.img/%s/%s"`（串 3073）+ `"Hit"`（串 5543）
  与该结构吻合（两级 %s 后接 Hit 键）。
- **波2-B 初版按 `{rid}/Hit` 加载 = 层级错误** ⇒ 空节点静音（无副作用但无声）。
  已改为 `play_state_sound(state)`：命中/破坏时播**该状态**的 Hit。

### 3. 传送门动画：原生节点全集与三态语义（修正波2预留结论）
`MapHelper.img/portal/` 探针深探（第二轮，完整 dump）：

| 组 | 键 | 结构 |
| --- | --- | --- |
| editor | pc pci pcig pcj pcs pg pgi ph pi ps psh psi pv sp tp | 编辑器预览动画（客户端不读）|
| game | **pv** | `0..7` 八种样式（bitmap 帧）+ `default` ⇒ 显式门按 Map 节点 `pv` 值选样式 |
| game | **ph** | `default` → `portalStart`/`portalContinue`/`portalExit` 三态 |
| game | **psh** | `1..4` 四种 + `default`，每种同样三态 ⇒ 脚本隐藏门样式 |

结论（修正波2的预留表述）：原生游戏内**没有**「15 种 Portal::Type 各配一张动画」的设计——
类型差异在行为码（pt），动画只有 显式门(pv×8样式) / 隐藏门(ph 三态) / 脚本隐藏门(psh 三态×4) 三族。
所以波2「传送门全类型动画」预留项**关闭其字面目标**，但暴露出真实缺口：
修前客户端 `MapPortals::init` 只加载 `pv.default` 与 `ph.default.portalContinue`，
**portalStart/portalExit 未加载** ⇒ 隐藏门触碰时无出现/消失动画；
**pv 0..7 样式未接入** ⇒ 所有显式门共用一张图。两者在波4 补。

## 波3 已实现
- `MapSummon` 真状态机（stand/hit/die/summoned + flip + 名牌静音），
  `FieldMarker::apply_damage` 虚化，`MapFieldMarkers::summon_die/summon_spawn` 入口。
- `RemoveSummonHandler`：anim==4 → 死亡动画；否则瞬时移除（原生语义）。
- `SpawnSummonHandler`：animated → 入场闪光。
- `Reactor`：音效按状态播放（修波2-B 层级错误）。

## 波3 预留（证据链未闭合，不硬做）
- 风筝/商店盒/Evan龙 的原生贴图键路径：MapHelper 无命中，需原生 CField 函数级反编译；
- 迷雾区域渲染：同上；
- Boss 血条多目标队列 + 名牌（Mob/<id>/info/name）：可做，但当前召唤兽改动已占本波，
  移入波4 收尾切片。
