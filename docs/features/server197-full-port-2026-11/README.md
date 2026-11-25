# 197 TMS280 服务端 → s9 + 192 客户端 全系统移植（2026-11-25）

> Plan：`PLAN.md`；取证：`evidence/`。用户拍板：D1 全 13 系统 / D2 扩 6 槽三端同步 /
> D4 完整导入 197 原生数据 / D5 资源走转 key 流水线。
> 运行服务端 = `E:/project/BeiDou-Server_s9`（PowerShell 查 java 进程命令行坐实，
> 非 `BeiDou-Server`）；客户端 = `E:/project/MapleStory-Client` → live `E:/MXD/BeiDou-Client-xy`。

## 已完成系统（逐系统闭环记录）

### 1. 潜能系统（A2 + D2）—— 完整三端闭环，构建验证通过

**197 数据来源**：`E:/download/冒险岛TMS280/197/2026-05-16服务端带源码/server197/`
- `wz/Item.wz/ItemOption.img.xml`（1,998,287 B，751 option × 20-25 级）
- `src/server/MapleItemInformationProvider.java:790-846` loadPotentialData（loader 蓝本）
- `src/client/inventory/Equip.java:56-61`（6 槽）、`:877 getState()`（独立品级）、`:997 setPotential(-17,...)`

**服务端**（`BeiDou-Server_s9/gms-server`）：
- `wz/Item.wz/ItemOption.img.xml` —— 197 原数据导入
- `src/main/java/org/gms/server/ItemOptionData.java`（新建，约 275 行）—— s9 Data API 懒加载，
  751/751 解析通过，中文 UTF-8 正确，失败降级空 Map；API `getOption/getStatValue/getString/getAllIds`
- `src/main/java/org/gms/combat/provider/PotentialStatProvider.java`（重写）——
  按真实 option id+满级值映射：incPADr→PAD_R / incMADr→MAD_R / incCr→CRIT_RATE /
  incCriticaldamage→CRIT_DAM / ignoreTargetDEF→IGNORE_PDR / incDAMr(+boss)→BOSS_DAM_R|DAM_R
- `Equip.java` —— 6 槽（potential1..3 + bonus1..3）+ 独立品级 potState/addState +
  `copy()` 补拷潜能/强化字段（研究 R7 漏洞）
- `ItemFactory.java` —— INSERT 32→35 占位符，读写 bonusPotential3/potState/addState
- `PacketCreator.java` —— equip 尾段追加 bp3(2B)+potState(1B)+addState(1B)，
  **追加在现有 15B 之后不平移**；`getDropItemGrade` 优先用 potState
- `EnhanceHandler.java` —— 魔方改产 197 option id（301/306/401/601 高价值池），
  品级写快照（非预览不污染 Equip），修复预览 mode 状态污染 bug
- `resources/db/migration/V1.11.28__add_bonus_potential3.sql` —— DB 三列

**客户端**（`MapleStory-Client`）：
- `Net/Handlers/Helpers/ItemParser.cpp` —— 读完 15B 后按 `length()` 门控读 bp3+2×state
- `Character/Inventory/Equip.h/.cpp` —— 6 槽 + state 成员 + `potential_rank_of(id,state)`
  （state 1..4 优先，0 回退旧 id 分段）
- `Character/Inventory/Inventory.h/.cpp` —— add_equip 签名 + bp3/potState/addState 默认参数
- `IO/Components/EquipTooltip.h/.cpp` —— potlines 数组 [5]→[6]，分组 i<3 主 / i=3..5 附加
- `locale/zh-CN.ini` / `en-US.ini` —— potline.1/11/12 改 197 语义，新增 301/306/401/601

**验证**：
- 客户端 `build_197port_6slot_20261125.bat` EXITCODE=0，live exe SHA `d36a29e7…`（15:37 核对一致）
- 服务端 `mvn compile/package` EXIT=0，jar SHA `d49e936c…`，6 class 核对存在
- DB 1 号角色 4 件装备写入 197 option id + state，查询核对通过
- **真机 tooltip / 品级图标未点测**（交用户验收）

## 设计要点 / 诚实边界

- **品级与 option id 分离**：197 的 state（1..4/17..20）与 option id 是两套概念；
  旧实现从 p1 id 分段推品级在 197 id 下会全判 SS，故必须独立 state 字段。
- **字段追加不平移**：equip 尾段 15B → 17B（bp3）→ 19B（+2 state），
  全部追加在尾部；客户端按剩余字节门控读取，旧服/旧客户端不重组。
- **未映射的 49 个属性键**：incSTR/incMHP/boss 等固定值类没有 CombatStatType 对应键，
  存于 ItemOptionData 但不产 Modifier（伤害乘区只接百分比类），详见 ItemOptionData 注释。
- **星之力/火花每级数值**：EnhanceStatProvider 仍用重建值（每星 +1 PAD_R/MAD_R），
  197 原生每星数值表未找到，标注重建。

### 2. 星之力强化（A1）—— 服务端闭环，构建验证通过
- `StarForceConfig.java`（新建）：197 25 档成功率/费用表 + START_DOWN/CURSE/BONUS 常量 + 星星道具 4001839。
- `StarForceHandler.java`（新建，290 行）：`toStarForce()` 逐条对照 197 ItemScrollHandler:662-711
  （千分比随机/降级/损坏/保底）；`apply()` 消耗星星 + modifyInventory + forceUpdateItem + refreshCombatProfile。
- `EnhanceHandler.java`：新增 action 21 接线（matSlot=statMode）。
- 客户端：`UIEnhance.cpp` 加族 21 + hint；`UIItemInventory.cpp` ETC 页双击星星(4001839)开壳。
- 差异（诚实）：无 EQUIP_MARK（损坏只降星不删装）、BONUS_TIME 用进程内 Map 近似、无 VIP 折扣/superior 表。
- DB：1 号角色发星星 300 + 兑换券；live exe SHA `22b5a8ad…`。

### 3. 火花/涅槃火焰（A3）—— 服务端闭环，构建验证通过
- `NirvanaFlame.java`（新建，19 维属性容器）、`Equip.java` 加字段 + copy 复制。
- `FlameStatProvider.java`（新建）：nbossDamage→BOSS_DAM_R / nignorePDR→IGNORE_PDR / ntotalDamage→DAM_R
  （固定值 15 项无 CombatStatType 对应键，由基础属性层消费，不产 Modifier）。
- `FlameRollHandler.java`（新建）：概率表**完整取自 197 config/nflame.properties**
  （entry.level.rate 全 9 档 + 19 个 flag.chance，2026-11-25 用真实表纠正了子代理的重建值）。
- `EnhanceHandler.java`：新增 action 22（5064501 黑=cap4 / 5064502 觉醒=cap9）。
- 客户端：`UIEnhance.cpp` 族 22 + hint；`UIItemInventory.cpp` CASH 页双击火焰开壳。
- DB：1 号角色发 5064501/5064502 各 5 个。

### 4. 拍卖行（B1）—— 双端闭环（构建验证通过，真机未验收）
- **资源**：192 原生 UIAuction.img 从 `UI_002.wz` 经 orange-wz 导出（零IV源）→
  `batch_convert_key(AAAAAA==→TSPHKw==, verify:true, fingerprint b6613ef3…)` →
  装 live `Data/UI/UIAuction.img`（SHA `ea5a253a…`，2,036,761 B，30 节点）→
  `--resource-probe` 报告 `[UIAuction.img] children=30`、无堆损坏。
- **服务端**：`V1.11.29__auction.sql`（109 行，已在 beidou 库建表 auctionitems/auctionequipment）、
  `AuctionService.java`（737 行）、`AuctionHandler.java`（377 行，13 模式中 8 个已实现、5 个 TODO）、
  opcode Send 0x3750..0x3754 / Recv 0x3751，PacketProcessor:303 已注册。
- **客户端**：`UIAuctionState.h`（单例状态）、`UIAuction.h/.cpp`（ctor 读 UIAuction.img + 7 按钮、
  三视图列表/滚轮/点行购买领取）、`AuctionPackets.h`（enter/list/take_down/buy/claim/search 工厂，
  字段序与服务端逐字对齐）、`AuctionHandlers.h/.cpp`（5 个回包 handler，read_entry 36B 门控）；
  `UIElement.h:299 AUCTION`、`OutPacket.h:362 AUCTION_ACTION=14161`、`PacketSwitch.cpp` 5 enum+注册+msg、
  `UIStatusBar.cpp:1196 BT_MENU_AUCTION` 从 ENTER_MTS 改开 UIAuction（原生入口，非侧栏图标）。
- **协议闭环**：opcode matrix 全 6 项 MATCHED/HANDLED（bidirectional），无新增冲突。
- **诚实边界**：① Search 发空关键词（未接 Textfield）；② 上架面板 slotUI 未接拖拽/定价/时长；
  ③ 列表用通用壳风格、视觉待精修；④ 回包是简表（itemId→名字+数量+单价/卖家），非 197 全量装备字节；
  ⑤ 5 个模式（离开/重上架/购买数量/出售窗口/完成窗口）服务端留 TODO；⑥ **未真机联调**。
- live exe SHA `88bce0da…`（含拍卖 UI）。

## 192 UI 资源取证（orange-wz 全 10 个 UI.wz，正确路径）
`docs/_probe_192_ui_allimgs.json`：Auction→UIAuction.img(UI_002.wz)、Familiar→UIFamiliar.img、
VMatrix→VMatrixUI.img、Union→GuildUI.img/GuildUI2.img、StarForce→UIWindow4.img/MapStarForceUI、
Soul→SoulUI.img、Maker→UIWindow2.img/Maker、Guild→GuildUI.img。**所有系统 192 都有原生 UI 资源**，
但当前客户端**没有任何一个对应的 UI 类**（除 UIMaker/UIEnchantSkill），需从零写渲染+交互+协议解析。

## 未完成（诚实边界）
- 客户端 UI 类：UIAuction/UIFamiliar/UIVMatrix/UIUnion/UISoul 等全部未写 →
  拍卖/精灵/V矩阵/联盟/灵魂充能/Familiar 等系统**服务端逻辑可跑，但玩家无法从客户端操作**，不算完整闭环。
- MTS/现金商店、装备制作高版本分支、公会增量、V 技能、Force：未实现。
- 强化卷品质（A4）：197 EnchantScrollEntry 是砸卷列表生成，当前端走既有 UIEnchantSkill + 砸卷链路，未单独移植。
- 上述所有系统**均未真机发包验收**（客户端进程权限/用户回归）。

## 数量级说明
本会话完成的是「强化数据链（潜能/星之力/火花）+ 拍卖服务端骨架」。
13 系统全客户端闭环需为每个系统新建 UI 类（数百行渲染/交互/协议）+ 资源导入 + 真机联调，
是数周级工作量，单会话 20 轮无法全部完成——剩余系统按 goal 续作推进。
