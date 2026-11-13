# Angel 全量语义复刻 · 分波次计划与执行台账（2026-11）

> 性质：按用户已拍板决策执行的**总索引**。HeavenClient 与 Angel 非同树；「复刻」= 行为 / 布局 / 状态机 / 协议字段语义对齐，**禁止**逐文件粘贴反编译。  
> 对照源：`user-ida-pro-mcp` → `` `Angel.i64 ``（先 `server_health` 读 `idb_path`，勿把端口当成 BeiDou）。  
> IDA 盘点：[../ida-v83-083-v83-i64-inventory-2026-11/](../ida-v83-083-v83-i64-inventory-2026-11/README.md)

## 0. 已锁定决策

| ID | 抉择 |
|---|---|
| Q1=A | 按功能面分波次；用户已说「全部开始」，按依赖串行做完各波，不再逐波等待确认 |
| Q2 | R1 多分辨率 + R2 i18n + R3 IME/中文 + R4 s9 协议（meso long、level/stat 放宽） |
| Q3=P1 | opcode/字段宽度跟 s9 + 用户放宽；Angel 只校语义 |
| Q4=W1 | WZ/IMG 默认不动；仅证据证明缺节点时 orange-wz（128B key）按面最小补齐 |
| Q5=V1 | 每面：IDA 行为一致 + 联机不炸 + 白名单类型仍有效 |

## 1. 字段类型白名单（任何波次禁止改回）

详见 [notes/00-field-type-whitelist.md](notes/00-field-type-whitelist.md)。

- `MapleStat` 基值 `int32_t` — `Character/CharStats.h`、`Net/Login.h`
- Buff 数值 `int32_t` — `Character/Buff.h`、`ActiveBuffs.h`
- WATK/MAGIC 上限 `2147483647` — `Character/StatCaps.h`
- 子弹 watk `int32_t` — `Data/BulletData.h`
- EXP `int64_t`
- meso `int64_t` + `read/write_long`
- `Job::get_name` / `CharStats::get_jobname` 必须 `std::string` 按值
- Tooltip：`BodyLine` / `ColorBox` / 半透明 α

## 2. 波次索引与状态

| 波 | 面 | 状态 | notes | 已有校准 README |
|---:|---|---|---|---|
| 01 | login | **已改** `set_focus_world` + **60 频道翻页** | [notes/w01-login.md](notes/w01-login.md) | [login-flow…](../login-flow-angel-calibration-2026-11/README.md) |
| 02 | HUD/statusbar | 无新增差异（已闭环） | [notes/w02-hud.md](notes/w02-hud.md) | [statusbar-hud…](../statusbar-hud-angel-calibration-2026-11/README.md) |
| 03 | corewindow | 无新增差异 | [notes/w03-corewindow.md](notes/w03-corewindow.md) | [corewindow…](../corewindow-angel-calibration-2026-11/README.md) |
| 04 | inventory/item | 白名单 Tooltip 保留 | [notes/w04-inventory.md](notes/w04-inventory.md) | IDA r16 + 工作区 tip |
| 05 | skill | 无强制修复 | [notes/w05-skill.md](notes/w05-skill.md) | [skill…](../skill-angel-calibration-2026-11/README.md) |
| 06 | quest | 无新增差异 | [notes/w06-quest.md](notes/w06-quest.md) | [quest…](../quest-angel-calibration-2026-11/README.md) |
| 07 | npc | 无新增差异 | [notes/w07-npc.md](notes/w07-npc.md) | [npc…](../npc-angel-calibration-2026-11/README.md) |
| 08 | map | Mob 死亡兜底（工作区） | [notes/w08-map.md](notes/w08-map.md) | [map…](../map-angel-calibration-2026-11/README.md) |
| 09 | drop | 无新增差异 | [notes/w09-drop.md](notes/w09-drop.md) | [drop…](../drop-angel-calibration-2026-11/README.md) |
| 10 | social | UserList 工作区续作 | [notes/w10-social.md](notes/w10-social.md) | [social-economy…](../social-economy-angel-calibration-2026-11/README.md) |
| 11 | pet | 无新增差异 | [notes/w11-pet.md](notes/w11-pet.md) | [pet-summon…](../pet-summon-angel-calibration-2026-11/README.md) |
| 12 | cashshop | 无新增差异 | [notes/w12-cashshop.md](notes/w12-cashshop.md) | [cashshop…](../cashshop-angel-calibration-2026-11/README.md) |
| 13 | activity | 无新增差异 | [notes/w13-activity.md](notes/w13-activity.md) | [activity…](../activity-minigame-angel-calibration-2026-11/README.md) |
| 14 | medal | Medal 工作区落地 | [notes/w14-medal.md](notes/w14-medal.md) | IDA r18 |
| 15 | 其余缺口 | MonsterBook 续作；阻塞清单已复核 | [notes/w15-gap.md](notes/w15-gap.md) | [feature-gap…](../feature-gap-inventory-2026-11/README.md) |

依赖与范围总表：[notes/01-wave-dependencies.md](notes/01-wave-dependencies.md)  
保留项：[notes/00-preserve-r1-r4.md](notes/00-preserve-r1-r4.md)

## 3. 每波固定工序

```text
读已有校准/IDA 盘点 →（缺证据）IDA / orange-wz → 最小差异修复
→ notes/wXX-*.md + evidence 摘要 → V1 自检 → 下一波
```

已与 Angel 语义一致且不破白名单 / 多分辨率 / i18n / IME / s9 的面：**写明「本波无代码差异」**，不为复刻而改。

## 4. 验收门禁（V1）

1. 对照 Angel 具名行为（或已有校准 README 销案结论）无新增语义回退  
2. 不改坏 s9 opcode / 字段宽度 / 白名单类型  
3. 能构建则 `tools/build_smoke.bat`；联机项写明未验证  

## 5. 风险

- 旧 `feature-gap` 大量 ❌ 已被后续源码销案，实现前须对源码再核，避免「假缺口」大改  
- 场地伤害族 c2s、MTS、220 stub、每日签到、勋章图标等 → 已写清「为何不能做」，不拖死全任务  
- IMG 层：无节点证据时保持 W1，不盲改 WZ  

## 6. 执行摘要（2026-11-12）

- **本会话新增业务代码**：W01 `LAST_CONNECTED_WORLD` → `set_focus_world`（`UIWorldSelect` + `WaveAShellHandlers`）。
- **工作区既有续作入账**：Tooltip 白名单（W04）、Mob 死亡兜底（W08）、UIUserList（W10）、UIMedal（W14）、UIMonsterBook（W15）。
- **多面无新增差异**：W02/03/05/06/07/09/11/12/13（校准已闭环或缺口属双端/资源阻塞）。
- **构建**：`tools/build_smoke.bat` GATE OK；log 显示已链出 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（约 11:24，4993024 B）。联机实机未跑。
- **未 git commit**（用户未要求）。

## 7. 阻塞清扫摘要（同日续作）

| 项 | 结论 |
|---|---|
| 60 频道翻页 | **已做**：`CHANNEL_CAPACITY=60` + `BtChPagePrev/Next`（`UIWorldSelect`） |
| 场地伤害 c2s 族 | **不能做**：s9 有 handler，客户端触发面未建（wave4） |
| PARTY_SEARCH_REGISTER(220) | **不能做**：s9 `PartySearchRegisterHandler` 空 stub |
| MTS | **不能做**：缺完整 UI + 复杂 c2s |
| 每日签到双端 | **不能做**：缺协议闭环 |
| 勋章图标 IMG | **不能做**：缺素材且 W1 禁盲改；DamageSkin 本体 live 已有 |
| 构建 | GATE OK；独立 log `build_blocker_clear.log`；`EXITCODE=0`；exe `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（12:11，5025792 B）。联机未验证 |
| git | 未 commit |
