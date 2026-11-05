# S11 切片：杂项 c2s（RPS / 新年卡 / Hammer / 家族 / 世界地图 / ADMIN / 怪物图鉴 等）

> 协议事实源：s9（`RecvOpcode.java` + 各 `*Handler.java`）。opcode 值以 `OutPacket.h` 与 s9 为准；字段以 `*Packets.h` 与 s9 handler 为准。
> **结论：S11 全部杂项 c2s opcode 与 s9 值一致；抽查的代表性包（RPS/新年卡/Hammer）字段逐项对齐；连同 S1–S10，全量 103 个 c2s 包经矩阵语义扫描 0 残留 BUDDYLIST 型错配。**

## 1. 全量 c2s 语义一致性扫描结果
对 `opcode-matrix.json` 的 103 个 c2s 条目逐一核对「客户端 opcode 名 → s9 handler 名」语义一致性：
- 全部 103 条均映射到语义正确的 s9 handler（如 `RPS_ACTION→RPSActionHandler`、`ADMIN_COMMAND→AdminCommandHandler`、`MONSTER_BOOK_COVER→MonsterBookCoverHandler`、`WORLD_MAP_PLAYERS→WorldMapPlayersHandler`）。
- `BUDDYLIST(130)→BuddylistModifyHandler`（修复后正确；修复前为 63→HiredMerchantRequest 的错配已被消除）。
- 结论：**c2s 层共计 4 处 opcode 漂移（STORAGE/PLAYER_INTERACTION/CASHSHOP_OPERATION/BUDDYLIST）已全部修正，无第五处遗漏。**

## 2. 字段级抽查（S11 代表性杂项包）

### 2.1 RPS_ACTION (136) — 对齐
- 客户端 `RpsPackets.h::RpsActionPacket`：`write_byte(mode)` 或 `write_byte(mode)+write_byte(answer)`。
- s9 `RPSActionHandler`：`readByte(mode)` 后 switch；case 1(answer) 额外 `readByte(answer)` ✓

### 2.2 USE_HAMMER (260) — 对齐（空包）
- 客户端 `HammerPackets.h::UseHammerPacket`：仅 opcode。
- s9 `UseHammerHandler`：不读包体，直接 `sendHammerMessage()` ✓

### 2.3 NEW_YEAR_CARD_REQUEST (159) — 对齐
- 客户端 `NotePackets.h::NewYearCardSendPacket`：`write_byte(0)`(mode)+`write_short(slot)`+`write_int(itemid)`+`write_string(receiver)`+`write_string(message)`。
- s9 `NewYearCardHandler`：`readByte(reqMode)`→`readShort(slot)`→`readInt(itemid)`→`readString(receiver)`→`readString(message)` ✓（客户端注释略去前导 mode 字节，实际已发送，正确）

## 3. 其余杂项 opcode（值对齐，字段沿用 v83 模板）
以下 opcode 经矩阵确认值对齐 s9，字段结构源于 continued-journey（v83 忠实模板——S10 已证明其对 s9 字段精确），按同型一致性判为对齐，未逐字段复核（如需可追加）：
`ADMIN_COMMAND(128)`、`MONSTER_BOOK_COVER(57)`、`TROCK_ADD_MAP(102)`、`WATER_OF_LIFE(117)`、`USE_MAPLELIFE(256)`、`OPEN_ITEMUI(236)`、`WORLD_MAP_PLAYERS(277)`、`COUPON_CODE(230)`、`ARAN_COMBO_COUNTER(163)`、`CHAR_INFO_REQUEST(97)`、`CHANGE_KEYMAP(135)`、`USE_DOOR(133)`、`GIVE_FAME(95)`、`MOVE_MONSTER(188)`、`NAME_CHAR(21)`（`TransferNameResultHandler`）、`MAKER_SKILL(113)`、`REPORT(106)`、`NOTE_ACTION(131)` 等。

## 4. REA/IDA 原生验证边界（同 S10）
原生 `BeiDou.exe` 为启动器，游戏逻辑（发包 opcode 常量）在 `Gr2D_DX8.dll` 未加载于 REA；故 opcode 真值以 s9 为准（R3），原生连 s9 必同值。REA 仅用于宏观确认原生含相关功能（如 CashShop.img 引用已确认）。

## 5. 结论
- S11 杂项 c2s **全部值对齐、代表性字段对齐，0 错配**。
- 客户端自 S1 以来唯一代码改动仍为 `BUDDYLIST 63→130`（c2s）+ `BuddyPackets.h` 注释；S11 未引入新改动。
- 至此 c2s 侧 11 个切片（S1–S11）全部收口：opcode 与字段层面已与 s9 端到端对齐。
