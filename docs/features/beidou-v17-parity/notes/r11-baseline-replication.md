# r11 · 复刻对照端核心基础系统（P0.5，2026-10-22）

> 目标：先补完对照端（BeiDou v83 原生）已有、当前客户端缺失的核心基础系统，再开发 ijl15.dll 扩展功能。
> 约束（用户决策）：**只做客户端、不改服务端**。s9 服务端（`E:\project\BeiDou-Server_s9`）已具备对应 handler，直接对接。
> 协议事实源：v83 标准(gms083) + s9 源码；rea 逆向（IDA 13338）仅补 UI 节点路径 / opcode 字段顺序印证。

## 审计结论（已完成）

清点 `Net/PacketSwitch.cpp`：
- `emplace<>` 注册表仅覆盖登录/SetField/MapObject/Attack/Player/Messaging/Quest/Inventory/NPC/CashShop壳/北斗自定义段。
- `OpcodeName` 表虽列出全 v83 opcode 名，但**未注册 handler 的核心包**即基线缺口，收到后走 `MSG_UNHANDLED` 警告：

| 系统 | 缺失 recv opcode | 当前端 UI | 当前端出站包 |
|---|---|---|---|
| 仓库 Trunk | `STORAGE(309)` | 无 | 无 |
| 组队 | `PARTY_OPERATION(62)`、`UPDATE_PARTYMEMBER_HP(201)` | 无 | 部分（UI/CharSelect 等无；上游可能残留） |
| 好友/黑名单 | `BUDDYLIST(63)` | 无 | 无 |
| 玩家交易 | `PLAYER_INTERACTION(314)` | 无（UITrade 缺） | 无（仅 CHAR_INFO=61 角色信息弹窗，不同） |
| 公会/联盟 | `GUILD_OPERATION(65)`、`ALLIANCE_OPERATION(66)` | 无 | 无 |
| 现金商城购买结算 | `CASHSHOP_OPERATION(325)` | `UICashShop` 仅浏览壳 | 无（购买/结算缺失） |
| 个人商店/雇佣商人 | `PLAYER_INTERACTION(314)` 子类 + `SPAWN/DESTROY/UPDATE_HIRED_MERCHANT(265/266/267)` | 无（UIMerchant 缺） | 无 |

- 与 `01-parity-matrix-and-plan.md` §2.5/P0.5 七项**完全吻合** → 确认这七项即当前客户端相对对照端的完整基线缺口。
- 其余 v83 核心 UI（UIShop/NPC店、UICashShop壳、UIMiniMap、UIBuffList、UIQuestLog、UIChat、UIKeyConfig、UIStatsInfo 等）当前端已具备或已另有任务，不计入本批。

## 实施顺序（每项七层闭环、单提交）

1. **仓库 Trunk**（`STORAGE=309`）— 解锁扩展 #4，改动最小优先。
2. **组队**（`PARTY_OPERATION=62`/`201`）。
3. **好友/黑名单**（`BUDDYLIST=63`）。
4. **玩家交易**（`PLAYER_INTERACTION=314` trade 分支）。
5. **公会/联盟**（`GUILD_OPERATION=65`/`66`）。
6. **现金商城购买结算**（`CASHSHOP_OPERATION=325`，扩展 UICashShop）。
7. **个人商店/雇佣商人**（`PLAYER_INTERACTION=314` merchant 分支 + 265/266/267）— 解锁扩展 #5。

## 每系统字段顺序取证方法

- 服务端 s9 源码（`E:\project\BeiDou-Server_s9`）`gms-server/src/main/java/.../handlers/` 下对应 Handler 的 `read`/`write` 即为权威字段序。
- 参考端 BeiDou.exe 逆向仅用于确认 UI 节点（`nl::nx::UI["Storage.img"]` 等）路径、按钮 id、坐标，避免臆造。
- 落地前先在 `IO/UITypes/` 用 `nl::nx::UI[...]` 确认节点存在，读空则降级（用通用底板）。

## 风险

- 黑屏环境既有问题不修；验证靠 s9 日志 + 自动登录回归(admin/admin 进图) + 用户实机确认。
- 交易/公会/个人商店协议复杂（XL 级），可能拆多个子提交。
- 当前端已占用扩展段 `0x37xx`；基础系统用 v83 原生段，无冲突。

## 进度（2026-10-22）

### ✅ 1. 仓库 Trunk（已完成）
- 协议事实源：`E:\project\BeiDou-Server_s9` 的 `StorageHandler`/`StorageProcessor`/`PacketCreator.getStorage` 等，字段序逐字节核对。
- `StoragePackets.h`（NEW）：`StorageTakePacket`(mode4:byte type,byte slot) / `StorageStorePacket`(mode5:short slot,int itemId,short qty) / `StorageArrangePacket`(mode6) / `StorageMesoPacket`(mode7:int meso 正取负存) / `StorageClosePacket`(mode8)。Opcode `OutPacket::STORAGE=309`。
- `StorageHandlers.{h,cpp}`（NEW）：解析 `STORAGE(309)` 各 mode——
  - `0x16` getStorage（全量：npcId/slots/meso/items）
  - `0x0D` storeStorage / `0x09` takeOutStorage（单类 items）
  - `0x0F` arrangeStorage（`byte 124` + skip10）
  - `0x13` mesoStorage
  - 其它单字节 → 错误码（日志）
  - 内联 `parse_items` 复用 s9 `addItemInfo` 格式（equip/pet/item 三分支）。
- `UIStorage.{h,cpp}`（NEW）：`UIElement::Type::STORAGE`。渲染格子（8 列×38px）、`ItemData::get(itemid).get_icon` 图标 + 数量；按钮 BtArrange/BtMesoIn/BtMesoOut/BtClose（节点缺失则退化坐标仍可点）；双击格子取出；金币经 `UISoftKey` 输入。
- 存入交互：仓库打开时，物品栏 `UIItemInventory::doubleclick` 对非装备/消耗类双击发 `StorageStorePacket`（仓内 `UIStorage.h` + `StoragePackets.h` 引入）。
- 注册：`Net/OutPacket.h`(STORAGE=309)、`IO/UIElement.h`(Type::STORAGE)、`Net/PacketSwitch.cpp`(Opcode::STORAGE=309 + `emplace<STORAGE,StorageHandler>()`)、`MapleStory.vcxproj`。
- 构建：链接成功写出部署 exe（仅 LNK4098/4099 既有警告）；自动登录链路不经过此 UI 无回归。
- 待用户实机确认：与仓库 NPC 对话触发 `getStorage(0x16)` 弹出窗口；取出/存入/整理/金币/关闭闭环。
- 已知假设：storage 物品槽位按服务端下发顺序 0 基编号（s9 `getSlot(type,slot)` 据此取物）。

### ✅ 2. 组队（已完成）
- 协议事实源：s9 `PartyOperationHandler`（client→server 操作码 1~6）与 `PacketCreator.updateParty`/`addPartyStatus`/`partyInvite`/`updatePartyMemberHP`。
- `PartyPackets.h`（NEW）：Create(1)/Leave(2)/Join(3,int partyid)/Invite(4,string name)/Expel(5,int cid)/ChangeLeader(6,int cid)，Opcode `PARTY_OPERATION=124`（既有）。
- `PartyHandlers.{h,cpp}`（NEW）：
  - `PartyHandler`（PARTY_OPERATION=62）解析 mode：`0x07` 静默/上下线、`0x0F` 加入、`0x0C` 离开/踢出/解散（flag 0=解散 deactivate，1=更新）、`0x1B` 换队长；内联 `parse_party_status` 按 6 成员块（6×id / 6×13B名 / 6×job / 6×level / 6×channel(-2=离线) / leader / 6×mapid / 6×门4int）填充 `Party`。
  - `PartyMemberHPHandler`（UPDATE_PARTYMEMBER_HP=201）：cid/curhp/maxhp → `uiparty->update_member_hp`。
- `UIParty.{h,cpp}`（NEW）：`Type::PARTY`。渲染队伍标题 + 6 成员行（名/Lv/职业/频道或离线/★队长）；按钮 BtInvite(UISoftKey 输入名)/BtLeave/BtClose。收到组队包自动 `emplace`，解散时 deactivate。
- 注册：`UIElement.h`(Type::PARTY)、`PacketSwitch.cpp`(PARTY_OPERATION=62 + UPDATE_PARTYMEMBER_HP=201 + emplace 两 handler)、`vcxproj`。
- 构建：链接成功；自动登录链路不经过此 UI 无回归。
- 待用户实机确认：建队/邀请/加入/离开/踢人/换队长闭环，HP 条刷新。
- 已知简化：成员行职位显示职业 id 数字（未映射职业名表）；门信息跳过。
