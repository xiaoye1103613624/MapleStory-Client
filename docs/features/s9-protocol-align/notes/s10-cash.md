# S10 切片：商城 / MTS / 仓库扩展 / 个人商店 / 寄存 / 运输 / owl / solomon / gacha

> 协议事实源：`BeiDou-Server_s9`（s9）。opcode 名值以 `OutPacket.h`（客户端）与 `RecvOpcode.java`（s9）为准；字段以各 `*Packets.h` 与 s9 `*Handler.java` 为准。
> **结论：S10 全部 16 个 c2s opcode 的「值」与「字段」均与 s9 逐项对齐，0 错配，无需任何代码改动（除一处陈旧注释修正）。**

## 1. opcode 值核对（16 个 c2s，全部对齐）

| 客户端 opcode | 值 | s9 RecvOpcode | s9 值 | 一致 |
|---|---|---|---|---|
| ENTER_CASHSHOP | 40 | ENTER_CASHSHOP | 40 | ✓ |
| USE_ITEMEFFECT | 52 | USE_ITEMEFFECT | 52 | ✓ |
| NPC_SHOP_ACTION | 61 | NPC_SHOP | 61 | ✓（S6 已核验） |
| USE_CASH_ITEM | 79 | USE_CASH_ITEM | 79 | ✓ |
| STORAGE | 62 | STORAGE | 62 | ✓（S6 已核验，曾修正 309→62） |
| FREDRICK_ACTION | 64 | FREDRICK_ACTION | 64 | ✓ |
| OWL_ACTION | 66 | OWL_ACTION | 66 | ✓ |
| OWL_WARP | 67 | OWL_WARP | 67 | ✓ |
| DUEY_ACTION | 65 | DUEY_ACTION | 65 | ✓ |
| PLAYER_INTERACTION | 123 | PLAYER_INTERACTION | 123 | ✓（曾修正 314→123） |
| CHECK_CASH | 228 | CHECK_CASH | 228 | ✓ |
| CASHSHOP_OPERATION | 229 | CASHSHOP_OPERATION | 229 | ✓（曾修正 325→229） |
| REMOTE_STORE | 59 | REMOTE_STORE | 59 | ✓（S6 已核验） |
| ENTER_MTS | 156 | ENTER_MTS | 156 | ✓ |
| USE_SOLOMON_ITEM | 157 | USE_SOLOMON_ITEM | 157 | ✓ |
| USE_GACHA_EXP | 158 | USE_GACHA_EXP | 158 | ✓ |

> 与 `BUDDYLIST(63→130)` 同型的 3 处修正（STORAGE/PLAYER_INTERACTION/CASHSHOP_OPERATION）本切片再次确认值正确。
> 本切片**未发现第二个 BUDDYLIST 型错配**。

## 2. 字段级对账

### 2.1 CASHSHOP_OPERATION (229) — 9 个子命令全部对齐
客户端 `CashShopPackets.h` ↔ s9 `CashOperationHandler.handlePacket`：

| 子命令 | 客户端写序 | s9 读序 | 一致 |
|---|---|---|---|
| 0x03 购买 | byte(0x03)+byte(0)+int(useNX)+int(sn) | readByte+skip(1)+readInt+readInt | ✓ |
| 0x1E 礼包 | byte(0x1E)+byte(0)+int(useNX)+int(sn) | 同 0x03 分支 | ✓ |
| 0x04 礼品 | byte+int(birthday)+int(sn)+string(name)+string(msg) | readInt+readInt+readString+readString | ✓ |
| 0x05 愿望 | byte+10×int(sn) | 循环 10× readInt | ✓ |
| 0x06 背包扩容 | byte+byte(0)+int(cash)+byte(mode)+[mode0:byte(type) \| mode1:int(sn)] | skip(1)+readInt+readByte(mode)+[readByte \| readInt] | ✓ |
| 0x07 仓库扩容 | byte+byte(0)+int(cash)+byte(mode)[+int(sn)] | skip(1)+readInt+readByte+[nothing \| readInt] | ✓ |
| 0x08 角色栏 | byte+byte(0)+int(cash)+int(sn) | skip(1)+readInt+readInt | ✓ |
| 0x0D 取出 | byte+int(cashId) | readInt(cashId) | ✓ |
| 0x0E 放入 | byte+int(cashId)+skip(4)+byte(invType) | readInt+skip(4)+readByte | ✓ |

### 2.2 FREDRICK_ACTION (64)
- `FredrickGetAllPacket`=`byte(0x1A)` → s9 `case 0x1A: fredrickRetrieveItems` ✓
- `FredrickExitPacket`=`byte(0x1C)` → s9 `case 0x1C: //Exit` ✓
- s9 仅 `readByte()` 后 switch，包体即单字节 operation ✓

### 2.3 DUEY_ACTION (65)
- `DueyOpenRecvPacket`=`byte(0x00)` → `TOSERVER_RECV_ITEM` ✓
- `DueySendPacket`=`byte(0x02)+byte(invent)+short(slot)+short(amount)+int(mesos)+string(recipient)+byte(quick)+[quick?string(msg)]` → s9 `TOSERVER_SEND_ITEM`: readByte+readShort+readShort+readInt+readString+readByte→quick+[quick?readString] ✓
- `DueyClaimPacket`=`byte(0x04)+int(package_id)` → `TOSERVER_CLAIM_PACKAGE`: readInt ✓
- `DueyDiscardPacket`=`byte(0x05)+int(package_id)` → `TOSERVER_REMOVE_PACKAGE`: readInt ✓
- `DueyClosePacket`=`byte(0x07)` → 第二个 `TOSERVER_CLAIM_PACKAGE` 分支(dueySendTalk) ✓

### 2.4 USE_SOLOMON_ITEM (157)
- 客户端 `UseSolomonPacket`=`write_time()(int)+write_short(slot)+write_int(itemid)` ↔ s9 `UseSolomonHandler`: `readInt()`(跳过 time)+`readShort()`(slot)+`readInt()`(itemId) ✓

### 2.5 USE_CASH_ITEM (79)
- 基础 `UseCashItemBasicPacket`=`write_short(cash_slot)+write_int(cash_itemid)` ↔ s9 `UseCashItemHandler`: `readShort(position)+readInt(itemId)`，随后 `itemType=itemId/10000` 按类型分支。
- 专用包（incubator 孵化器 +int egg_inv+int egg_slot；petName 宠物取名 +string；chalkboard/mapEffect/avatarMega 等 +string×N）附加字段与 s9 各 `itemType` 分支（504 缩地石读 byte+int mapId、506 孵化器读 int+int、507/517 读 string…）一一对应 ✓

### 2.6 空包（opcode 对齐即足够）
- `ENTER_CASHSHOP(40)` / `CHECK_CASH(228)` / `ENTER_MTS(156)` / `OWL_ACTION(66)` / `USE_GACHA_EXP(158)`：客户端为空包，s9 对应 handler 不读包体（仅 opcode 触发服务端计算下发）✓
- `OWL_WARP(67)`：`write_int(ownerid)+write_int(mapid)` ↔ s9 `OwlWarpHandler` 读 ownerid+mapid ✓

## 3. REA/IDA 原生客户端验证（边界说明）

- 用 REA 加载原生 `BeiDou.exe`（base 0x400000）后 `search_strings` 确认原生**引用 `/CashShop.img/CSItemSearch/Price`、`/CashShop.img/CS3thEvent/0`、`CashShop`** —— 证明原生客户端**确实含现金商城资产/功能**，回应「原生是否缺代码」疑虑：**原生不缺代码**。
- **架构边界**：`BeiDou.exe` 仅是启动器，实际游戏逻辑（含发包 opcode 常量）在 `Gr2D_DX8.dll` 等 DLL，**未在 REA 加载**。因此 opcode 级真值以 s9 为准（R3，由用户确立）；原生若能连 s9 必然也用同一套 opcode（如好友=130、商城=229），与本客户端修复一致。
- 结论：S10 协议层（opcode+字段）已与 s9 端到端对齐，REA 仅用于确认「原生含该功能」的宏观事实，不依赖其读出 opcode 常量。

## 4. 结论
- S10 共 16 个 c2s 包，**16/16 opcode 值与 s9 一致、字段逐项对齐，0 错配**。
- 唯一改动：修正 `CashShopPackets.h:28` 一处陈旧注释（325→229，值早已修正）。
- 无 BUDDYLIST 型错配。本切片**未引入功能改动**，符合「先对账、后改动、逐条给证据」纪律；后续若要做商城 UI 功能开发，应优先以原生客户端（DLL）逻辑 + `wz_dump` 的 `CashShop*.img` 运行时数据为准（原生优先、其次扩展）。
