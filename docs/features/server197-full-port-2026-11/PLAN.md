# 197 TMS280 服务端 → s9 + 192 客户端 全系统移植 Plan（2026-11-25）

## 0. 取证结论（先证据后计划）

- 197 源码：`E:/download/冒险岛TMS280/197/2026-05-16服务端带源码/server197/src`，915 java / ~28 万行，`ServerConstants.java:38 MapleVersion=197`。
- 协议硬墙：197 Recv 566/Send 881 opcode（`RecvPacketOpcode.java`/`SendPacketOpcode.java`，值最大 2349+）+ DESede opcode 加密（`tools/OpcodeEncryption.java`、`MEncryptedOpcode.java`1148 行）→ **整包不可直搬，只抽算法/数据/DB 模型，按 s9 opcode 重写**。
- 192 UI 资源（orange-wz 直读 10 个 UI.wz，`docs/_probe_192_ui_search.json`）：VMatrixUI.img(11)、UIFamiliar.img(24)、UIAuction.img、SoulUI.img(8)、GuildUI/GuildUI2 union 页(38)、UIWindow2.img/{Maker,LunchMaker,Cube 系}、UIWindow4.img/MapStarForceUI 均**存在** → 多数系统有原生 UI 可配套。
- 完全无 192 UI 证据的：ItemOption.img（在 197 服务端 Item.wz，非客户端 UI，作数据层移植）。

## 1. 决策点（需用户拍板，阻塞实现）

Skill 门禁：公共 API / 协议 opcode / 全局数值 / 存档结构变更必须先问用户。以下 III 梯队涉及：

- D1 **范围确认**：I+II 梯队 8 系统（星之力/潜能数据/火花/强化卷/拍卖/现金商店MTS/装备制作/精灵收藏/公会增量）是否本轮全做？III 梯队（V矩阵/联盟/V技能/Force）是否本轮全做？
- D2 **存档结构**：潜能槽 s9 现 5 个（3 主+2 附加，Equip.java:79），197 是 6 个(:56-61)。是否扩到 6 槽（需 DB 迁移 + PacketCreator + 客户端 ItemParser 三端同步）？
- D3 **协议风格**：所有新系统统一用 s9 私有 opcode 段 0x374A.. 递增（与既有 0x371D/0x371E 强化族一致），确认？
- D4 **数值口径**：197 的配置值（星之力成功率/火花 roll）直接采用，还是按当前端平衡改？199 万 ItemOption.img 全量导入还是只挑常用 id 段？
- D5 **资源同步**：192 UI img 走 AGENTS.md §3.1 密钥转换流水线导入 live Data/UI，确认？

## 2. 任务分解（用户确认后一次性实现，按依赖排序）

### 批次 A — 强化数据闭环（I 梯队，无新 UI，最快）
A1 星之力强化算法：`ItemScrollHandler.java:600-709` + `ItemConstants.java:451-515,3473-3515` → s9 新 handler，复用 `Equip.starForce`，协议 0x374A/0x374B。闭环判据：客户端双击星星券→开窗→砸星→tooltip 星数变化→DB 落库。
A2 潜能数值表：`MapleItemInformationProvider.java:790-846` loader + 197 `ItemOption.img.xml`(2MB) → s9 `StructItemOption`/`OptionType`，升级 `PotentialStatProvider` 从重建值到真实词条数值。需 D2/D4。
A3 火花容器：`NirvanaFlame.java`(782,20 维) + `NFlameConfig` → s9 Equip 火花属性容器 + roll，复用 `spark` 字段，tooltip 已有显示。闭环判据：火花券→改 spark→tooltip 显示具体词条→伤害乘区。
A4 强化卷品质：`EnchantHandler.java`(605) + `EnchantScrollEntry`(97) → 补 s9 强化卷列表生成，接既有 UIEnchantSkill。

### 批次 B — 独立系统（II 梯队，需 UI 资源但链路清晰）
B1 拍卖行：`AuctionHandler.java`(1005,13 模式) + `AuctionServer` → s9 handler+DB，`UIAuction.img` 移植 + 客户端 `UIAuction` 类。协议 0x374C..，DB 新表 xy_auction。
B2 现金商店/MTS：`BuyCashItemHandler.java`(763)、`MTSCSPacket.java`(637) → 补 s9 CashShop 分支，当前端已有 cashshop native。
B3 装备制作高版本分支：`ItemMakerHandler.java`(1017) → 复用现有 UIMaker/MAKER_SKILL，补服务端分支。
B4 精灵/怪物收藏：`FamiliarPacket`(398)、`MonsterCollection.java`(161) → s9 handler + `UIFamiliar.img` 移植 + 客户端 UI。协议 0x3750..。
B5 公会增量：`MapleGuild.java`(1620)、`GuildPacket`(1110) → s9 现有公会增量（公会技能/搜索），`GuildUI.img` 已有。

### 批次 C — 大系统（III 梯队，需先 UI 后逻辑）
C1 V 矩阵/核心：`PlayerHandler.java:5620-5760`、`VCoreSkillEntry`(188)、`VCoresPacket`(127) → s9 handler + `VMatrixUI.img` 移植 + 客户端 UIVMatrix。协议 0x3760..，DB 新表 xy_vcore。
C2 联盟 Union：`MapleUnionAccount`(492)、`MapleUnionInfo`(381) → s9 + `GuildUI.img/union` 页复用，DB 新表 xy_union。依赖 C1（联盟技能）。
C3 V 技能：`VSkill.java`(3726) → 与 C1 联动，技能效果框架扩展。
C4 Force/奥术：`MapleForce`(189)、`ForcePacket`(589)（注意 `ForceAtomType.java`5 行空壳不可当功能）→ s9 + UI，最后做。

## 3. 每系统闭环判据（统一）

1. 192 UI img 经 orange-wz 转 key 入库 live，`--resource-probe` 无堆损坏。
2. 客户端有 UI 类 + 双击/侧栏入口（按 192 原生触发方式，不挂左侧功能图标）。
3. 协议 opcode 双端对齐（客户端 OutPacket/PacketSwitch ↔ s9 RecvOpcode/SendOpcode），跑 `tools/gen_opcode_matrix.py`。
4. 服务端逻辑 + DB 落库 + 回包刷新（modifyInventory + forceUpdateItem / 新包）。
5. 构建：客户端 Rebuild 0 error + 服务端 mvn package；live exe SHA 实时核对。
6. 真机点测清单交用户回归（我不宣称已点测）。

## 4. 风险与回滚

- 协议墙：所有 197 opcode 常量丢弃，只用 s9 号段；每系统单独 opcode 区间便于回滚。
- 存档：D2 若扩 6 槽需 DB 迁移脚本 + 回滚脚本；未确认前不动。
- 数值：D4 未定用 197 原值但集中配置类可单点改，标注"来自 197，非本端实测"。
- 并发构建：用隔离 obj 输出目录（本会话已验证可绕开 LNK1104）。

## 5. 产物

`docs/features/server197-full-port-2026-11/`（本文 + notes/ + evidence/ 取证 JSON + 每系统 README）。
