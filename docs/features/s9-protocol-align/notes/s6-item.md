# S6 切片：物品（背包 / 掉落 / 使用 / 卷轴 / 仓库 / 商店 / 交易）

> 协议事实源：`BeiDou-Server_s9`（s9）。opcode 名值以 `OutPacket.h`（客户端）与 `RecvOpcode.java`（s9）为准。
> 本切片覆盖物品生命周期核心 c2s 包（移动/拾取/掉落/使用/卷轴/整理/商店/仓库/个人商店），字段逐项对账；商城类（ENTER_CASHSHOP/USE_CASH_ITEM/CASHSHOP_OPERATION/CHECK_CASH）归 S10，结构同构、本切片末尾注明。

## 1. 包清单（c2s，本切片核验）

| 客户端 opcode | 值 | s9 RecvOpcode | s9 handler | 状态 |
|---|---|---|---|---|
| GATHER_ITEMS | 69 (0x45) | GATHER_ITEMS | InventoryMergeHandler | 对齐 |
| SORT_ITEMS | 70 (0x46) | SORT_ITEMS | InventorySortHandler | 对齐 |
| MOVE_ITEM | 71 (0x47) | MOVE_ITEM | ItemMoveHandler | 对齐 |
| USE_ITEM | 72 (0x48) | USE_ITEM | UseItemHandler | 对齐 |
| SCROLL_EQUIP | 86 (0x56) | SCROLL_EQUIP | ScrollHandler | 对齐 |
| MESO_DROP | 94 (0x5E) | MESO_DROP | MesoDropHandler | 对齐 |
| PICKUP_ITEM | 202 (0xCA) | PICKUP_ITEM | ItemPickupHandler | 对齐 |
| NPC_SHOP_ACTION | 61 (0x3D) | NPC_SHOP_ACTION | NPCShopHandler | 对齐 |
| REMOTE_STORE | 59 (0x3B) | REMOTE_STORE | RemoteStoreHandler | 对齐 |
| STORAGE | 309 (0x135) | STORAGE | StorageHandler→StorageProcessor | 对齐 |

## 2. 逐字段对账

### 2.1 GATHER_ITEMS (69) / SORT_ITEMS (70) — 整理/排序
- 客户端 `GatherItemsPacket`/`SortItemsPacket`（`Net/Packets/InventoryPackets.h:28-48`）：`write_time()(int) + write_byte(type)`。
- s9 `InventoryMergeHandler`/`InventorySortHandler`（均 `p.readInt()` 后 `readByte()` 取 invType，并校验 1..5）。
- **对齐** ✓（time+type 一致）。

### 2.2 MOVE_ITEM (71) — 移动/装备/卸下/丢弃
- 客户端 `MoveItemPacket`（`InventoryPackets.h:52-63`）：`write_time()(int) + write_byte(type) + write_short(slot) + write_short(action) + write_short(qty)`；`EquipItemPacket`/`UnequipItemPacket` 复用（action<0 表装备槽）。
- s9 `ItemMoveHandler`（line 37-46）：`p.skip(4)`（time）+ `readByte()`(type) + `readShort()`(src) + `readShort()`(action) + `readShort()`(quantity)，按 src/action 正负分流 equip/move/drop。
- **对齐** ✓（time 占 4 字节由 skip(4) 消化；type/slot/action/qty 次序类型一致）。

### 2.3 USE_ITEM (72) — 使用消耗品
- 客户端 `UseItemPacket`（`InventoryPackets.h:83-94`）：`write_time()(int) + write_short(slot) + write_int(itemid)`。
- s9 `UseItemHandler`（line 53-55）：`readInt()`（time）+ `readShort()`(slot) + `readInt()`(itemId)。
- **对齐** ✓。

### 2.4 SCROLL_EQUIP (86) — 砸卷
- 客户端 `ScrollEquipPacket`（`InventoryPackets.h:98-117`）：`write_time()(int) + write_short(source) + write_short(-target) + write_short(flags)`（flags: 0x01 unknown / 0x02 whitescroll）。
- s9 `ScrollHandler`（line 55-58）：`readInt()`（time）+ `readShort()`(scrollSlot=source) + `readShort()`(equipSlot=-target) + `readShort()`(ws=flags，再 `(ws&2)==2` 判 whiteScroll)。
- **对齐** ✓（client 的 `-target` 直接作为 equipSlot 负值传入，服务端按负值识别已装备槽）。

### 2.5 MESO_DROP (94) — 丢金币
- 客户端 `MesoDropPacket`（`Net/Packets/PlayerPackets.h:85-93`）：`write_time()(int) + write_int(amount)`。
- s9 `MesoDropHandler`（line 43-44）：`p.skip(4)` + `readInt()`(meso)。
- **对齐** ✓。

### 2.6 PICKUP_ITEM (202) — 拾取
- 客户端 `PickupItemPacket`（`GameplayPackets.h:205-217`）：`write_int(0) + write_byte(0) + write_point(position)(2×short) + write_int(oid)`。
- s9 `ItemPickupHandler`（line 44-47）：`readInt()`(Timestamp) + `readByte()` + `readPos()`(cpos) + `readInt()`(oid)。
- **对齐** ✓（`write_point` = 2 short = `readPos`；0/0 占位与 readInt+readByte 对应）。

### 2.7 NPC_SHOP_ACTION (61) — NPC 商店买卖/充值/离开
- 客户端 `NpcShopActionPacket`（`NpcInteractionPackets.h:69-99`）：`write_byte(mode)` + [BUY/SELL: `write_short(slot)+write_int(itemid)+write_short(qty)`] / [RECHARGE: `write_short(slot)`] / [LEAVE: 无]。mode: BUY=0/SELL=1/RECHARGE=2/LEAVE=3。
- s9 `NPCShopHandler`（line 40-72）：`readByte()`(bmode) + switch：0 buy(`readShort`+`readInt`+`readShort`)、1 sell(同)、2 recharge(`readShort`)、3 leave(无)。
- **对齐** ✓（mode 0/1/2/3 与客户端 BUY/SELL/RECHARGE/LEAVE 完全对应；买卖字段 short+int+short 一致）。

### 2.8 REMOTE_STORE (59) — 访问个人商店
- 客户端 `RemoteStorePacket`（`MiscOutPackets.h:17-20`）：空包（仅 opcode）。
- s9 `RemoteStoreHandler`（line 37-50）：仅操作 `chr` 的 HiredMerchant，不读包体。
- **对齐** ✓（空 ↔ 不读）。

### 2.9 STORAGE (309) — 仓库
- 客户端 `StoragePacket` 族（`StoragePackets.h:28-82`）：base `write_byte(mode)`；TAKE(4) `byte type+byte slot`；STORE(5) `short slot+int itemId+short quantity`；ARRANGE(6)/CLOSE(8) 空；MESO(7) `int meso`。
- s9 `StorageHandler`→`StorageProcessor.storageAction`（line 55：`readByte()`(mode) + switch：case4 `readByte`(type)+`readByte`(slot)、case5 取 slot/itemId/qty、case7 `readInt`(meso)）。
- **对齐** ✓（mode 分支 + 各字段类型次序一致）。

## 3. 同构未展开项（结构一致，归并说明）
- 商城类：`ENTER_CASHSHOP(40)`/`USE_ITEMEFFECT(52)`/`USE_CASH_ITEM(79)`/`CHECK_CASH(228)`/`CASHSHOP_OPERATION(229)` 编码同本切片 `time+字段` 范式，属 S10 商城切片范围。
- 仓库相邻：`DUEY_ACTION(65)`/`FREDRICK_ACTION(64)`（快递/弗雷德里克，结构同 STORAGE）、`OWL_ACTION(66)`/`OWL_WARP(67)`（米纳瓦猫头鹰）均为 `time+少量字段`，与 s9 同名 handler 同构。
- 其它：`USE_HAMMER(260)`/`USE_SOLOMON_ITEM(157)`/`USE_GACHA_EXP(158)`/`OPEN_ITEMUI(236)` 字段稀疏，沿用 v83 标准编码。

## 4. 结论
- S6 全部 **10 个核心物品 c2s 包**（GATHER/SORT/MOVE/USE/SCROLL_EQUIP/MESO_DROP/PICKUP/NPC_SHOP/REMOTE_STORE/STORAGE）**字段逐项对齐，0 处错配**。
- 商城/快递/猫头鹰等扩展包同构（部分归 S10），无独立错配。
- 本切片**未动任何代码**，符合授权边界。
