# Angel / 当前端 / s9 服务端 三方一致性确认书（round88）

> 触发（m09544）：「没有按Angel修改且保持原先逻辑的，是因为BeiDou和Angel一样的原因？如果不是的话，需要确认Angel是否和服务端一致，如果一致就全部按Angel 反编译逻辑对齐；原先BeiDou有很多功能都是错误的；」

## 0. 一句话结论

**不是**因为 BeiDou==Angel 才保持；Angel 与服务端**握手/opcode 层一致、字段级 body 层不一致**（s9 是改造过的 v83），所以「全部按 Angel 对齐」的前提不成立、照 083 原生包形改回会断联机。但「原先功能很多是错的」**成立且一直在修**——纯客户端层按 Angel 083 语义对齐已发货 30+ 个原子提交。

## 1. 判断题：79 条「保持原逻辑」的真实理由

| 问 | 答 |
| --- | --- |
| BeiDou 和 Angel 一样吗？ | 一样，但只是**同一份 v83/083 GMS 二进制的两个 IDA 实例**（Angel 13339 带 3122 修饰名 / BeiDou 13337 只有 50；EA 95/97 类、809 命名函数逐字节一致）。 |
| 保持的是谁的逻辑？ | 当前端 **HeavenClient（从零自研重实现，README.md 自证）**，不是 BeiDou.exe 的代码。二者无继承关系。 |
| 所以「因为一样所以保持」成立吗？ | **不成立**。这是把「符号还原状态」和「代码来源」混为一谈。 |
| 真实理由 | `DECISIONS.md §0`：**s9 联机优先，Angel 083 仅作语义校准源；校准≠改回原生；数值重合≠语义重合**。 |

## 2. Angel 与 s9 服务端是否一致（分层判定，答案不同）

| 层 | 判定 | 证据 |
| --- | --- | --- |
| 握手 / 版本 / 加解密 | ✅ 一致 | `PacketCreator.java:630-635 getHello` 声明 `u16 0x0E / version 83 / u8 49`；IV 按 `0xFFFF-83`；`PLAN.md F1` 握手已对齐；`GMSV83PacketProtocol` ↔ `Cryptography.cpp` T4 逐字节对拍通过。 |
| opcode 名/值 + 注册 | ✅ 一致 | `GATE OK (s9 a78ba681a4)`：`client_out_sent=148`、`s9_recv_constants=182`、`s9_send_constants=314`、`s9_send_emitted=253` 三方对账。 |
| **字段级 body** | ❌ **不一致** | **s9 是改造过的 v83**，见下表。 |

### 2.1 s9 相对 083 原生的 wire 改造（源码逐字，`PacketCreator.java`）

| # | 行 | s9 源码 | 083 原生 | 改造原因（源码注释） |
| --- | --- | --- | --- | --- |
| 1 | 192 | `p.writeShort(chr.getLevel());` | level 1B | 等级上限 999，writeByte >255 溢出 |
| 2 | 198-205 | `p.writeInt(chr.getHp());` ×4 | short | 血量上限 99999 超 short(65535) |
| 3 | **211** | **`p.writeLong(chr.getExp());`** | **exp 4B** | 999 级累积经验远超 int32，必须 8 字节 |
| 4 | 213 | `p.writeInt((int) chr.getGachaExp());` | 无此字段 | s9 GachaExp 扩展通道 |
| 5 | 217 | `p.writeInt(0);`（spawnpoint 后） | 无尾部 | s9 固定补 int(0) |
| 6 | 2624 | `removeItemFromMap` animation≥2 恒写 4B chrId | `CDropPool::OnPacket v2==4` 读 Decode2 delay | 数值重合≠语义重合 |

全仓只有**一份** `PacketCreator.java`（find 单命中）——不存在「另一份和 083 一样的服务端」可切。

### 2.2 条件指令判定

> 用户条件：「如果一致就全部按 Angel 反编译逻辑对齐」。

**body/wire 层前提不成立 → 不触发全部对齐。** 把 exp 8B→4B、level 2B→1B、删 GachaExp/tail 改回 083 原生，会把 s9 包的中间字段整体平移、直接复现白屏。

## 3. 「原先功能很多是错的」——成立，实锤 + 已修链

`Net/Handlers/Helpers/LoginParser.cpp:150-174` 自述白屏案例：旧代码按 8B exp 解析 s9 包 → `skip(4)` 吞掉真实 `mapId=100000000` → 从 spawnpoint 槽读出 `16` 当 mapid → `Stage::load` 抛 `map data missing` → **进图白屏**。现代码 `recv.read_long()` + 末尾 `recv.skip(4)` 已与 s9 `:214-217` 逐字段对齐。

按 Angel 083 翻案重修已发货（节选，全 30+ 见 evidence `shipped_align_commits`）：

- `e1be5eea 📝 docs(calibration): 五面复核归档 round87（UIChannel/UICharSelect/UICommonCreation/UIMiscShells/UIItemInventory） (#devfix-five-face-review-archive)`
- `649cf66c 📝 docs(ui): UIEquipInventory 装备窗按 Angel 083 只读取证复核 + 口径标注 (#devfix-equip-window-083-review)`
- `4b2452c4 🐛 fix(ui): D-G 公会浮窗按 Angel 083 收口 4 处旁路 + orange-wz key 规格纠偏 (#devfix-guild-dg-resolution)`
- `187e2bc1 📝 docs(ui): UIGuild 公会浮窗按 Angel 083 复核 + 口径标注 (#devfix-guild-window-083-review)`
- `c07b1a7b 📝 docs(ui): UIKeyConfig 键盘设置窗按 Angel 083 CUIKeyConfig 复核 + 口径标注 (#devfix-keyconfig-083-review)`
- `08c5f297 📝 docs(ui): UILogin/CUITitle 标题窗按 Angel 083 OnCreate 立即数复核 + 口径标注 (#devfix-logintitle-083-review)`
- `edf25ed5 📝 docs(ui): UILoginWait 加载进度条按 Angel 083 CLogin 等待态复核 + 口径标注 (#devfix-loginwait-083-review)`
- `9cee87fc 📝 docs(net): NPC_ACTION 移动包形与 CANCEL_DEBUFF 依据链按 Angel 083 复核纠偏 (#devfix-npcaction-083-review)`
- `7fd2ee02 📝 docs(ui): GAMEOPT/SYSOPT 设置窗按 Angel 083 CUIGameOpt/CUISysOpt 复核 + 口径标注 (#devfix-optionmenu-083-review)`
- `55fd53c5 📝 docs(combat): 伤害数字/皮肤按 Angel 083 CAnimationDisplayer 复核 + 口径标注 (#devfix-damagenumber-083-review)`
- `93160f15 📝 docs(ui): D7 聊天三态按 Angel 083 SetChatType 取证复核 + 口径纠正 (#devfix-chattype-083-review)`
- `1c69da3f 🐛 fix(ui): 好友窗/组队窗按 Angel 083 CUIUserList 真资源重修 (#devfix-userlist-partyhp-083)`
- `b13f8e94 🐛 fix(ui): HUD 主窗 4 钮闪烁按 Angel 083 SetButtonBlink 状态机重修 (#devfix-hud-button-blink-083)`
- `cfd3c10b 🐛 fix(ui): 推荐世界弹窗按 Angel 083 布局重修（WIP 收口）(#devfix-recommendworld-083-relayout)`

这些都曾是「参照 BeiDou 或自行修复」的错版，按 Angel 083 真资源/真立即数重修。

## 4. round86 一刀切的口径缺口（本轮纠偏，不自动改代码）

168 条按「碰不碰 wire」重分类：`PURE 76 / WIRE 38 / MIXED 14 / OTHER 40 = 168`。

79 条「保持」拆开：`WIRE 27 / OTHER 25 / PURE 20 / MIXED 7 = 79`。

| 类 | 条数 | 处置 |
| --- | --- | --- |
| WIRE 27 | 27 | **保持 s9 是硬约束**（改回 083 断联机），理由成立。清单见 evidence `must_keep_s9`。 |
| PURE 20 + MIXED 7 | **27** | **理由不成立**：不碰 wire，却被 round86 用「s9 联机优先」一刀切 KEEP。列 `reopen_candidates`，按 Angel 083 语义**逐条重新拍板**；架构级重写（单体窗合并 / 7 子窗拆分 / 双布局收敛）须单点裁决，门禁 5 禁想象修改，本轮零源码改动。 |
| OTHER 25 | 25 | 文档/措辞/低优先类，维持。 |

### 4.1 reopen 清单（27 条，face/id）

- `login-flow/D3` K3 [KEEP_S9][PURE] 骑士团条件解码分支 | 先确认 s9 是否发追加段再补
- `login-flow/D9` K4 [KEEP_CURRENT][PURE] 60 频道翻页 | 补翻页（15 世界 / 60 频道 + 翻页控件）
- `login-flow/D12` K4 [DOC_ONLY][MIXED] 登录发包字段布局 / VAC 编号 | 只做逐字节核对文档、不动代码
- `login-flow/D13` K2 [BATCH_0_BASELINE][MIXED] CHARLIST 外层固定 15 槽 vs 按 charcount | 保持 s9
- `statusbar-hud/D1` K2 [BATCH_1_KEEP][PURE] 单体 `CUIStatusBar` → 当前端 10+2 类拆分，是否合并回单体？ | 保持拆分（功能性等价优先，合并会重写全部 HUD 类
- `statusbar-hud/D12` K2 [BATCH_1_KEEP][PURE] `UIMobGage.*` 空 Type（MOBGAGE=102）是否补实体类？ | 保持（`UIBossHP` 已覆盖 Boss 血条，空
- `corewindow/D2` K2 [BATCH_0_BASELINE][MIXED] 083 原生语义 vs s9 联机，冲突处以谁为准？ | 保持 s9 联机优先，Angel 仅作语义校准源，差异逐条标注「改回原生会断联机」
- `corewindow/D8` K3 [DOC_ONLY][PURE] `CUIStat` 6 掩码 / `CUISkill` 500ms 门闩 / `CUIKeyConfig` 5 分支覆盖度？ | 先出逐条对
- `corewindow/D9` K2 [BATCH_1_KEEP][PURE] 宏系统 32 格 → 当前端 5 槽是否补？ | 保持 5 槽（`SkillMacros::MAX_SLOTS=5` 与 s9 `SKILL
- `corewindow/D10` K2 [BATCH_0_BASELINE][MIXED] `CWorldMapDlg 666×524` 是否改当前端世界地图？ | 保持 s9 布局，原生「无在线玩家列表」结论已坐实，右上列表为 s
- `social-economy/D3` K2 [BATCH_1_KEEP][PURE] 联盟双入口（页签 + `UIAlliance` 浮窗）/ `UIGuild` 浮窗是否合并回原生？ | 保持现状（083 原生无独立窗，但
- `social-economy/D9` K2 [BATCH_1_KEEP][PURE] `UIFriendGroup` / 公会徽章编辑 / MapleTV 完整 UI 三个「整族缺失」是否立项？ | 保持明确不做/搁置：083
- `social-economy/D12` K2 [BATCH_1_KEEP][PURE] `wedding.*` 26 键全英文 / en-US 中文硬编码 / stub 文案是否译？ | 保持决策 B「保持英文原值、只建键」，等
- `pet-summon/D2` K2 [BATCH_0_BASELINE][MIXED] 两处 s9 字节丢弃（`:526` 8B uid→int32、`:520-523`/`:1414-1417` 字段读了就丢） | 保持 s9
- `activity-minigame/D1` K4 [KEEP_WITH_NOTE][PURE] `IO/UIStateGame.cpp:532` + `IO/UITypes/UISpecial.h:9` 095 注释污染 | 做：只换注
- `cashshop/D2` K2 [BATCH_1_KEEP][PURE] 是否拆 7 子窗 | 不拆，保持单窗
- `skill/D4` K2 [BATCH_1_KEEP][PURE] 是否新增独立 `CWndSkillGuide` 子窗 | 不新增
- `skill/D9` K2 [BATCH_1_KEEP][PURE] 双布局分支 `classic` 是否收敛为原生单布局 | 保持双布局
- `skill/D11` K2 [BATCH_1_KEEP][PURE] `BT_HYPER/GUILDSKILL/RIDE` 三空按钮是否接管 | 保持 DISABLED
- `skill/D12` K2 [BATCH_1_KEEP][PURE] 组合栏 `deactivate()` 与 `ShiftMacroUIState` 形态差异 | 保持当前端显式修复
- `map/D1` K2 [BATCH_0_BASELINE][MIXED] 083 `SaveFuncKeyMap` 89 键差分 + `sub_4E43FA` 条目编码 vs s9 全量 `ChangeKeyMap
- `map/D4` K2 [BATCH_1_KEEP][PURE] 083 `CWorldMapDlg` MFC 大窗 vs 当前端 `UIWorldMap` 侧栏状态机 + 付费传送 | 保持当前端
- `map/D8` K2 [BATCH_1_KEEP][MIXED] `CHANGE_QUICKSLOT` 服务端 `(byte)readInt` 截断 vs 客户端 8×`write_int` | 保持既有注
- `quest/D5` K3 [KEEP_CURRENT][PURE] 是否实现 `UIQuestLog::send_key` | 待裁决
- `quest/D8` K2 [BATCH_1_KEEP][PURE] 是否把 `sub_721008` 条件分支搬进当前端 | 不搬
- `drop/D2` K2 [BATCH_1_KEEP][PURE] 是否补 `animation=0` 的 `IWzGr2DLayer` 消亡动画 | 不补
- `drop/D12` K2 [BATCH_1_KEEP][PURE] 是否补 `animation=4` 的延时爆炸帧 | 不改

## 5. 对齐策略（今后按此执行）

1. **wire / opcode 层**：s9 源码优先，不可动（27 条 must_keep）。
2. **字段级 body**：`PLAN.md §P2` 11 片逐片探针 + 一片一原子 commit，以 s9 ByteBuf 序列为准。
3. **纯客户端 UI/排版/动画/交互**：按 Angel 083 语义继续对齐（本会话一直在做；27 条 reopen 逐条拍板后 P1 实施）。

## 6. 限制 / 真机归口

- 本轮 0 源码 / 0 协议 / 0 WZ 改动，只读取证 + 判定；工作树 11 个外部程序在途 WIP 原样保留。
- 27 条 reopen 是「需重新拍板」不是「已改」；架构级重写不在本轮自动落地。
- 真机五屏截图 60 节/142 处（用户认领）
- reopen_candidates 27 条需逐条拍板（架构级重写单点裁决，非本轮自动改）
- InPacket::available() 393 处 §27-A 真机逐面回归
- PLAN.md §P2 字段级 body 11 片逐片探针 + 一片一原子 commit
