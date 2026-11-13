# 第十二面 · 既有掉落(Drop)笔记四态审计 (r05)

> 审计对象：`docs/features/beidou-v17-parity/notes/r98-mesodrop-shop-open.md`、`r107-inventory-btcoin-mesodrop.md`、`r134-changemap-pickup-softkey.md`、`r180-inventory-use-drop-move-softkey.md`、`r183-statusbar-chair-door-pickup-softkey.md`、`r471-pickup-tip.md`、`r649-mob-npc-loot-reactor-effect-rem.md`、`r788-shop-sell-reactor-pickup.md`、`r789-shortcut-statusbar-nearest.md`，以及本仓 `Gameplay/MapleMap/Drop.*`、`MapDrops.*`、`Net/Handlers/MapObjectHandlers.*`、`docs/features/map-mob-drops/README.md`、`docs/features/map-object-parity/README.md` 内既有结论。
> 方法：以 Angel(13339) 083/v83 一手伪代码为权威源，逐条判「销案 / 推翻 / 新增 / 遗留」。既有 REA subject 与 095 线单列，不冒充一手。

## 1. 既有素材索引（9 篇）

| 笔记 | 行数 | 结论摘要 |
| --- | --- | --- |
| `r98-mesodrop-shop-open.md` | 7 | `MESO_DROP = 94`（0x5E）；GameMenu `D` SoftKey → `MesoDropPacket` |
| `r107-inventory-btcoin-mesodrop.md` | 6 | `BT_COIN`/`BT_COIN_SM` → SoftKey 金额 → `MesoDropPacket` |
| `r134-changemap-pickup-softkey.md` | 7 | GameMenu `'` SoftKey oid → `PickupItemPacket(oid, player_pos)` |
| `r180-inventory-use-drop-move-softkey.md` | 9 | `D` SoftKey `slot,qty` → `MoveItemPacket` drop（dst=0）；`Y` → `MesoDropPacket` |
| `r183-statusbar-chair-door-pickup-softkey.md` | 7 | StatusBar `K` SoftKey pickup oid |
| `r471-pickup-tip.md` | 6 | `RemoveLoot mode>1` 且本机 → `map_tip` Pickup |
| `r649-mob-npc-loot-reactor-effect-rem.md` | 4 | Mob/Npc/Loot rem + Reactor/Effect/Damage rem |
| `r788-shop-sell-reactor-pickup.md` | 10 | GameMenu `'` 脚下掉落 `find_loot_at` → Pickup |
| `r789-shortcut-statusbar-nearest.md` | 11 | StatusBar `K` 拾取最近 |

## 2. 销案（6 组，既有结论与本面一手一致）

| 组 | 既有结论 | 本面一手佐证 | 处置 |
| --- | --- | --- | --- |
| S1 | `268/269` 双端同号 | `CDropPool::OnPacket @0x5058db` `268→Enter` / `269→Leave`；`Net/PacketSwitch.cpp:142-143` | 销案，数值一致 |
| S2 | `94` 包体 `write_time + write_int(amount)` | `SendDropMoneyRequest @0xa23de5` `Encode4(update_time)+Encode4(amount)` | 销案，字段序一致 |
| S3 | `RemoveLootHandler` `mode>1` 读 cid | s9 `removeItemFromMap` `if (animation >= 2) writeInt(chrId)` | 销案，逻辑等价 |
| S4 | 金币四档阈值 | `sub_506BFE @0x506bfe` `<50/50..99/100..999/>=1000` vs `MapDrops.cpp:78` | 销案（本面新增一手，既有笔记未记） |
| S5 | 宠物拾取 `PET_LOOT(170)` + 13 空字节 | s9 `PetLootHandler` 注释同序；`PetPackets.h:102-115` | 销案 |
| S6 | 尾吞余量防错位 | `DropLootHandler:861` / `RemoveLootHandler:894` 均 `while (recv.available()) skip_byte()` | 销案 |

## 3. 推翻（1 组，依据来源被证伪）

| 组 | 既有结论 | 本面一手 | 处置 |
| --- | --- | --- | --- |
| O1 | 「083 与 s9 `animation=4` 同语义（都当 cid 读）」 | 083 `OnDropLeaveField` `v2==4` 读 `Decode2` 并写 `*(v4+68)=update_time+delay`；s9 写 4 字节 `chrId` | **推翻**：数值同 4、包体宽度不同；当前端取 s9 正确，不得按 083 改 |

## 4. 新增（7 组，本面首次取得的一手结论）

| 组 | 新结论 |
| --- | --- |
| N1 | `18/18 EAs identical`，`missing=0`，第三次独立复证 Angel/BeiDou 同二进制 |
| N2 | 本面唯一 c2s 出站 = `94`；捡物/宠物捡物是 s9 新增通道，083 内联在 `TryPickUpDrop` |
| N3 | `DROP_ITEM mod` s9 实际发 `{1,2,3}`（`disappearing*` 传 3，无 dropfrom） |
| N4 | booby trap 全链 083 独有（3 函数 + `SendBoobyTrapAlert`），当前端零实现 |
| N5 | 拖拽族 9 函数 083 独有（`CDraggable*` + `CWnd::OnDragDrop` + `Begin/EndDragDrop` + `SetCursorState(11)`） |
| N6 | `sub_506BFE` 四档阈值（靠 `GetMoneyIcon` 调用点反查 + 单独反编译才取得） |
| N7 | orange-wz 结构层 WORKS / 内容层 8 次 `IMG 解析失败` → `.nx` 合规回退（见 r03） |

## 5. 遗留（5 组）

| 组 | 遗留项 | 原因 |
| --- | --- | --- |
| L1 | 真机双端 `269` animation=4 抓包对照 | 需双开 + 抓包 |
| L2 | F 维 12 项像素（四档图标尺寸/配色/描边/抗锯齿） | IMG 内容层 `IMG 解析失败` |
| L3 | `animation=0` 消亡特效复刻 | 当前端无 083 `IWzGr2DLayer` COM 栈 |
| L4 | 拾取 `map_tip`（`r471` 记「mode>1 且本机 → map_tip Pickup」）未落地 | 需真机确认观感后再定 |
| L5 | 真实掉落表（`map-mob-drops` 记「接口已就绪、无真实掉落表、显式空态」） | 数据缺口非协议缺口，不编造 |

## 6. 与 `map-object-parity` / `map-mob-drops` 台账的关系

- `docs/features/map-object-parity/README.md:69` 已记「DropLoot/RemoveLoot 宽度复核 OK；宠物拾取（anim=5+slot）不再吞 looter」→ 本面**复核为已闭**（S3/S5），不重复报。
- `docs/features/map-mob-drops/README.md` 记「右侧掉落 UI 与 `MobDropData` 接口已就绪，无真实掉落表」→ 本面归 L5/B10 ⏳，属数据缺口。
- `r471-pickup-tip.md` 记「`RemoveLoot mode>1` 且本机 → map_tip Pickup」→ **本面核实现状无 tip**（`MapObjectHandlers.cpp:890` 只有 `Sound(Sound::Name::PICKUP).play()`），归 D10 ⏳ / L4。
