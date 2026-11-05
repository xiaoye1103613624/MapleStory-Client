# 迷雾区域渲染（Skill tile 矩形平铺）

> 状态：代码已闭环；真机 → pending-live-tests  
> 承接：`map-object-parity` §5「迷雾区域渲染」；`map-object-audit` G4  
> 日期：2026-11-05  
> 产物：`docs/features/mist-area-render/`

## 1. 选型

三项候选中本轮闭合**迷雾区域渲染**：协议框已对齐、贴图键经 IDA+IMG 钉死，平铺算法有函数级反编译，证据充分度高于 BossHP 多目标队列 / C8 页码图标。

## 2. 证据

| 源 | 结论 |
| --- | --- |
| IDA `sub_531325` → `sub_431A3E` → `sub_431A63` | SPAWN_MIST(273) / REMOVE_MIST(274) |
| IDA `sub_432776` | 毒雾/通用：椭圆带内随机 tile 变体平铺；x+=35+rand(20)，y+=H×0.8 |
| IDA `sub_432A1E` | 烟幕：单点 `tile/0` @ ((x1+x2)/2, y2) |
| IDA 串 `0xAFE1F4` | `Skill/MobSkill.img/%03d/level/%d/tile` |
| NX `Skill.nx` | 玩家 `211.img/skill/2111003/tile`、`422.img/skill/4221006/tile`；怪雾 `MobSkill.img/131/level/*/tile` |
| s9 `PacketCreator.spawnMist` | oid/type/owner/skill/level/delay/box4×int；type 0 mob / 1 poison / 2 smoke / 4 recovery |

**【已证实】** 雾区视觉是 **Skill tile 变体平铺**，不是 Map/Effect mist 键（旧预留口径过时）。  
**【差异】** 修前 `MapMist` 仅文本场标+ItemUp 占位，box/type/level 丢弃。  
**【推荐】** `MapMist` 真对象化 + `sub_432776`/`sub_432A1E` 布局。无待决项。

## 3. 实现

- `MapMist`：加载 tile 变体；按 type/skill 选散射或烟幕单点；覆盖 `draw`/`update`
- `SpawnMistHandler` / `MapFieldMarkers::spawn_mist`：消费 box + mist_type + level；lifetime=0（REMOVE_MIST）
- 怪雾 `mist_type==0` → `MobSkill.img`；否则 → 玩家 `Skill/<ns>.img/skill/<id>/tile`

## 4. 验证

- 构建：见 `builds/`
- 真机（本轮不跑）：毒雾/烟幕/怪雾进图，确认矩形内 tile 动画；REMOVE_MIST 后消失

## 5. 触碰

仅客户端 `MapMist` / `MapFieldMarkers` / `SpawnMistHandler` + 本文档 / parity 台账；未动服务端。
