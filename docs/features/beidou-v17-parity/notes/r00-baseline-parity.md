# R00 · 基础功能对照审计（V17.7 参考端 vs 当前端）

> 日期：2026-10-21
> 触发：扩展功能（P1）开工前未验证基础功能是否对齐；本审计把「基础」与「扩展」分开核对。
> 结论先行：**UI 资源完全一致，但代码层存在系统性基础缺口；当前端 v1 明确只做单机核心循环，社交/经济系统未移植。**

---

## 0. 结论摘要

| 维度 | 结论 | 证据 |
| --- | --- | --- |
| UI 资源 | ✅ 无缺口 | `Data/UI` 下 20 个 `.img` 与参考端 **逐字节一致**（SHA256 全同） |
| UI 窗口 | 🟡 核心窗口有，社交/经济窗口缺 | 参考端 `UIWindow.img` 有 **112** 个顶层节点；当前端 **44** 个 `IO/UITypes` 类 |
| 入站协议 | 🟡 值对齐，覆盖不足 | 客户端注册 **59** 个 handler；北斗服务端 `SendOpcode` 有 **332** 项（含自定义/活动） |
| 出站协议 | 🟡 值对齐，能力不足 | 客户端 `OutPacket` 仅 **46** 项；北斗服务端 `RecvOpcode` 有 **193** 项 |
| 服务端支撑 | ✅ 基础系统已实现 | `StorageHandler`、`PlayerInteractionHandler`、`PartyOperationHandler`、`GuildOperationHandler`、`BuddylistModifyHandler`、`MessengerHandler`、`CashOperationHandler` 等均存在 |

**关键纠正**：`opcode-align-beidou-gms083` 证明的是「已实现的 46 个出站包数值与服务端一致」，**不等于**「服务端 193 个出站能力都已在客户端实现」。覆盖率和数值对齐是两件事。

**根因**：`docs/features/playability-core-loop/README.md` §1 明确写「**不进 v1**：组队、好友、交易、商城、公会、拍卖」。后续扩展功能直接从这个 v1 基线往上叠，跳过了这些基础系统。

---

## 1. 审计方法

| 维度 | 方法 | 产物 |
| --- | --- | --- |
| 资源 | 对两端 `Data/UI/*.img` 做 SHA256 比对 | 本文件 §2 |
| 窗口 | 用 `tools/nx_inspect.py` dump 当前端 `Data/.nxcache/UI.nx` 的 `UIWindow.img` 顶层节点；与 `IO/UITypes`、`UIElement::Type` 对照 | 本文件 §3 |
| 入站 | 解析 `Net/PacketSwitch.cpp` 的 `emplace<...>()` 与 `OpcodeName` 表 | 本文件 §4 |
| 出站 | 解析 `Net/OutPacket.h` 的 `enum Opcode` | 本文件 §4 |
| 服务端 | 枚举 `gms-server/.../channel/handlers/*.java` 与 `RecvOpcode`/`SendOpcode` | 本文件 §4 |

复现命令：

```powershell
# 资源一致性
Get-ChildItem E:\MXD\BeiDou-ClientV17.7\Data\UI -Filter *.img | ForEach-Object {
  (Get-FileHash $_.FullName).Hash -eq (Get-FileHash (Join-Path 'E:\MXD\BeiDou-Client-xy\Data\UI' $_.Name)).Hash
}

# 窗口节点
python tools\nx_inspect.py list E:\MXD\BeiDou-Client-xy\Data\.nxcache\UI.nx 1 UIWindow.img

# 覆盖统计
(Select-String -Path Net\PacketSwitch.cpp -Pattern "emplace<").Count
(Get-Content Net\OutPacket.h | Select-String -Pattern "^\s*[A-Z0-9_]+\s*=").Count
```

> 注：参考端 `V17.7\Data\UI\*.img` 与当前端同为加密 IMG，直接读文件头是随机字节；审计使用当前端已生成的 `UI.nx` 缓存。由于两端 `.img` 哈希全同，该缓存即代表参考端窗口集合。

---

## 2. 资源对照：无缺口

参考端与当前端 `Data/UI` 各 20 个 `.img`，**SHA256 全部相同**，大小也完全一致：

`Basic`、`BuffIcon`、`CashShop`、`CashShopPreview`、`ChatBalloon`、`DialogImage`、`GuildBBS`、`GuildMark`、`ITC`、`ITCPreview`、`Login`、`Logo`、`MapleEzorsiaV2wzfiles`、`MapleTV`、`MapLogin`、`NameTag`、`Npt_GuideLine`、`StatusBar`、`tutorial`、`UIWindow`。

**含义**：窗口缺失不是资源问题，`Trunk`、`TradingRoom`、`PersonalShop`、`Messenger`、`Family`、`MonsterBook` 等节点都已随客户端分发，可直接复用（符合 D4）。

---

## 3. 窗口对照（UIWindow.img 112 个顶层节点）

### 3.1 已实现（核心单机循环）

| 系统 | 参考端节点 | 当前实现 | 状态 |
| --- | --- | --- | --- |
| 装备栏 | `Equip` | `UIEquipInventory` | ✅ |
| 道具栏 | `Item` | `UIItemInventory` | ✅ |
| 属性 | `Stat` | `UIStatsInfo` | ✅ |
| 技能 | `Skill` / `SkillEx` | `UISkillBook`（含 `macro`） | ✅ |
| 任务 | `Quest` / `QuestIcon` | `UIQuestLog` | 🟡 日志可用；`QuestAlarm` 未接 |
| 小地图 | `MiniMap` | `UIMiniMap` | ✅ |
| 世界地图 | `WorldMap` | `UIWorldMap` | ✅ |
| NPC 商店 | `Shop` | `UIShop` | ✅ |
| NPC 对话 | `UtilDlgEx` / `UtilDlgEx_*` | `UINpcTalk` | 🟡 基础对话可用；头像/宠物/多选分支不全 |
| 按键设置 | `KeyConfig` | `UIKeyConfig` | ✅ |
| 选项 | `GameOpt` / `SysOpt` | `UIOptionMenu` | ✅（`GameOpt` 优先，`SysOpt` 回退） |
| 频道 | `Channel` | `UIChannel` | ✅ |
| 公告 | `Notice` | `UINotice` | ✅ |
| 事件 | `EventWindow` | `UIEvent` | ✅（自定义活动面板） |
| 现金商城 | `CashShop`（独立 UIState） | `UICashShop` | 🟡 可进入、可浏览；购买/结算未接 |

### 3.2 有 UI 骨架但未接通

| 系统 | 参考端节点 | 当前实现 | 缺口 |
| --- | --- | --- | --- |
| 组队 | `UserList`（Party 页）/ `PartySearch` | `UIUserList` 已有 Party/Friend 页、按钮与出站包 | 入站 `PARTY_OPERATION(62)`、`UPDATE_PARTYMEMBER_HP(201)` 未注册，队伍状态不更新 |
| 好友/黑名单 | `UserList`（Friend 页） | `UIUserList` 已有好友页 | 入站 `BUDDYLIST(63)`、出站 `BUDDYLIST_MODIFY` 均未实现 |
| 现金商城购买 | `CashShop` | `UICashShop` | 入站 `CASHSHOP_OPERATION(325)` 等、出站购买包未实现 |

### 3.3 完全未实现（基础系统）

| 系统 | 参考端节点 | 当前端 | 影响 |
| --- | --- | --- | --- |
| 仓库 | `Trunk` / `StoreBank` | 无 UI、无 handler、无出站包 | 装备/道具保管；**扩展 #4「更高仓库列表」的前置** |
| 玩家交易 | `TradingRoom` / `CashTradingRoom` | 无 | 玩家间经济闭环 |
| 个人商店 | `PersonalShop` / `EntrustedShop` / `MemberShop` | 无 | 摆摊/雇佣商人；**扩展 #5「玩家商店加长」的前置** |
| 公会 | 无独立节点（状态栏入口） | 无 UI、无 handler、无出站包 | 社交/公会技能 |
| 家族 | `Family` / `FamilyTree` | 无 | 社交 |
| 信使 | `Messenger` | 无（`UIStatusMessenger` 只是右下角提示条） | 私聊/群聊 |
| 怪物图鉴 | `MonsterBook` | 无 | D7 已决定新增，不计入 parity |
| MapleTV | `MapleTV` | 无 | 内容系统 |
| 快递/包裹 | `Delivery` / `Parcel` | 无 | 离线道具流转 |
| 名声 | `UserInfo` 等 | 只显示，无赠送入口 | 社交 |
| 备忘录 | `Memo` | 无 | 社交 |

> 其余节点（`Admin`、`Coupon`、`Wedding`、`Ranking`、`SpeedQuiz`、`MonsterCarnival`、`AriantMatch`、`RpsGame` 等）属活动/管理/已废弃系统，暂不纳入基础 parity；需要时可单独立项。

---

## 4. 协议与功能缺口

### 4.1 客户端现状

- 入站：`Net/PacketSwitch.cpp` 注册 **59** 个 handler。
- 出站：`Net/OutPacket.h` 定义 **46** 个 opcode。
- 服务端：`RecvOpcode` **193** 项、`SendOpcode` **332** 项（含北斗自定义与活动）。
- 数值对齐：已实现部分与服务端**逐值一致**（`opcode-align-beidou-gms083`），但这只是「已实现部分」的对齐。

### 4.2 基础系统缺口表

| 功能 | 入站 opcode | 客户端入站 | 客户端出站 | 服务端 | 分级 |
| --- | --- | --- | --- | --- | --- |
| 仓库 | `STORAGE(309)`、`FREDRICK(310/311)` | ❌ | ❌ | ✅ `StorageHandler` | 影响主循环 |
| 玩家交易 | `PLAYER_INTERACTION(314)` | ❌ | ❌ | ✅ `PlayerInteractionHandler` | 影响主循环 |
| 组队 | `PARTY_OPERATION(62)`、`UPDATE_PARTYMEMBER_HP(201)` | ❌ | ✅ `PartyOperationPacket` | ✅ `PartyOperationHandler` | 影响主循环 |
| 好友 | `BUDDYLIST(63)` | ❌ | ❌ | ✅ `BuddylistModifyHandler` | 影响主循环 |
| 公会/联盟 | `GUILD_OPERATION(65)`、`ALLIANCE_OPERATION(66)` | ❌ | ❌ | ✅ `GuildOperationHandler`、`AllianceOperationHandler` | 影响主循环 |
| 信使 | `MESSENGER(313)` | ❌ | ❌ | ✅ `MessengerHandler` | 体验缺失 |
| 现金商城购买 | `CASHSHOP_OPERATION(325)` 等 | ❌ | ❌ | ✅ `CashOperationHandler` | 影响主循环 |
| 个人商店/雇佣商人 | `PLAYER_INTERACTION(314)`、`HIRED_MERCHANT(265-267)` | ❌ | ❌ | ✅ `HiredMerchantRequest` | 影响主循环 |
| 名声 | `FAME_RESPONSE(38)` | ❌ | ❌ | ✅ `GiveFameHandler` | 体验缺失 |
| 备忘录 | `MEMO_RESULT(41)` | ❌ | ❌ | ✅（Note 相关） | 体验缺失 |
| 快递 | `PARCEL(322)`、`DUEY` | ❌ | ❌ | ✅ `DueyHandler` | 体验缺失 |
| 家族 | `FAMILY_*`（94–104） | ❌ | ❌ | ✅ `Family*Handler` | 体验缺失 |
| 怪物图鉴 | `MONSTER_BOOK_*`（83/84） | ❌ | ❌ | ✅ `MonsterBookCoverHandler` | 体验缺失（D7 新增） |
| MapleTV | `MAPLE_TV_USE_RES(109)`、`341–343` | ❌ | ❌ | ✅（TV 相关） | 体验缺失 |
| MTS | `MTS_OPERATION(347/348)` | ❌ | ❌ | ✅ `MTSHandler` | 体验缺失 |
| 强化 | `VICIOUS_HAMMER(354)`、`VEGA_SCROLL(358)` | ❌ | ❌ | ✅ `UseHammerHandler` | 体验缺失 |

> 分级口径：
> - **阻断进游戏**：当前未发现（登录/选角/进图/移动/战斗/NPC/商店/任务均可用）。
> - **影响主循环**：多人交互与经济闭环（仓库、交易、组队、好友、公会、商城购买、个人商店）。
> - **体验缺失**：社交增强与内容系统（信使、名声、备忘录、快递、家族、图鉴、TV、MTS、强化）。

---

## 5. 对扩展功能的影响

| 扩展项 | 依赖的基础系统 | 结论 |
| --- | --- | --- |
| #4 更高仓库列表 | 仓库 `Trunk` | **必须先补基础仓库**，否则无容器可扩展 |
| #5 玩家商店加长 | 个人商店 / 雇佣商人 | 需先补基础商店，否则只能改 WZ 尺寸 |
| 窗口版现金商城 | 现金商城购买/结算 | 需先补 `CASHSHOP_OPERATION` 等入站/出站 |
| #2 装备悬停对比（已完成） | 装备 tooltip/属性 | 无基础依赖，已闭环 |
| #1 Buff 倒计时（已完成） | Buff 列表 | 无基础依赖，已闭环 |

---

## 6. 重排后的优先级（建议）

### P0.5 · 基础功能补齐（新增，先于剩余扩展）

按「服务端已支持 + 资源已存在 + 依赖最少」排序：

1. **仓库（Trunk + StoreBank/Fredrick）**：解锁 #4，验证「NPC 打开容器 → 存取 → 服务端持久化」的完整链路。
2. **组队（入站 62/201 + 出站已有）**：UI 与出站包已存在，补入站即可跑通，性价比最高。
3. **好友/黑名单（63 + BUDDYLIST_MODIFY）**：UI 已有，补协议与列表更新。
4. **玩家交易（314）**：复用 `TradingRoom` 资源，建立玩家间经济闭环。
5. **公会/联盟（65/66）**：状态栏入口已有，补 UI 与协议。
6. **现金商城购买/结算（325 等）**：在现有 `UICashShop` 壳上补交易链路。
7. **个人商店/雇佣商人**：解锁 #5。

### P1 · 恢复扩展功能

基础系统验收后，按原矩阵继续 #3、#6…；#4/#5 在对应基础系统完成后进行。

### P2/P3 · 体验与内容系统

信使、名声、备忘录、快递、家族、怪物图鉴（D7 新增）、MapleTV、MTS、强化、活动。

---

## 7. 每个基础功能的验收口径

以仓库为例（其余同构）：

| 层 | 验收点 |
| --- | --- |
| 资源 | 复用 `UIWindow.img/Trunk`，不新增切图 |
| 入站 | 注册 `STORAGE(309)`，解析打开/存取/金币/槽位扩展 |
| 出站 | 实现 `STORAGE` 请求（打开、存、取、金币、扩展） |
| UI | `UIStorage`：槽位网格、金币、存取按钮、双击移动、与背包拖拽 |
| 服务端 | `StorageHandler` 对照字段顺序，确认 v83 包结构 |
| 联调 | 存入 → 关闭 → 重开 → 取出；金币存取；满槽/不足额提示 |
| 回归 | 登录→选角→进图→背包→商店→仓库→任务 主链 |

---

## 8. 待确认

1. 基础补齐的深度：只补「影响主循环」6 项，还是一次性补到信使/名声/备忘录？
2. 仓库与组队的先后：仓库解锁扩展 #4，组队改动最小；建议仓库优先，是否同意？
3. 现金商城是否纳入首批（涉及购买/扣点/发货，联调成本最高）？
