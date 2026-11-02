# S2 切片：选角 / 建删号 / 查看所有角色 / PIC

> 协议事实源：`BeiDou-Server_s9`（s9）。opcode 名值以 `OutPacket.h`（客户端）与 `RecvOpcode.java`（s9）为准，二者逐一对应（S1 已确认名值全匹配、无漂移）。
> 本切片聚焦 **c2s**（客户端 → s9）。s2c 侧仅列关键验收点（见 §3）。

## 1. 包清单（c2s）

| 客户端 opcode | 值 | s9 RecvOpcode | 值 | s9 handler | 状态 |
|---|---|---|---|---|---|
| NAME_CHAR | 21 (0x15) | CHECK_CHAR_NAME | 0x15 | CheckCharNameHandler | 对齐 |
| CREATE_CHAR | 22 (0x16) | CREATE_CHAR | 0x16 | CreateCharHandler | 对齐 |
| DELETE_CHAR | 23 (0x17) | DELETE_CHAR | 0x17 | DeleteCharHandler | 对齐 |
| SELECT_CHAR | 19 (0x13) | CHAR_SELECT | 0x13 | CharSelectedHandler | 对齐 |
| PLAYER_LOGIN | 20 (0x14) | PLAYER_LOGGEDIN | 0x14 | PlayerLoggedinHandler | 对齐 |
| VIEW_ALL_CHAR | 13 (0x0D) | VIEW_ALL_CHAR | 0x0D | ViewAllCharHandler | 对齐 |
| REGISTER_PIC | 29 (0x1D) | REGISTER_PIC | 0x1D | RegisterPicHandler | 对齐 |
| SELECT_CHAR_PIC | 30 (0x1E) | CHAR_SELECT_WITH_PIC | 0x1E | CharSelectedWithPicHandler | 对齐 |
| PICK_ALL_CHAR | 14 (0x0E) | PICK_ALL_CHAR | 0x0E | （s9 已注册） | 客户端从不发（见 §2.9） |

## 2. 逐字段对账

### 2.1 NAME_CHAR (21) — 重名检测
- 客户端 `NameCharPacket`（`Net/Packets/CharCreationPackets.h:26-33`）：`write_string(name)`。
- s9 `CheckCharNameHandler.handlePacket`（line 34）：`String name = p.readString();`。
- **对齐** ✓（string ↔ string）。

### 2.2 CREATE_CHAR (22) — 建号
- 客户端 `CreateCharPacket`（`CharCreationPackets.h:45-62`）写序：
  `name(str) → job(i) → face(i) → hair(i) → hairColor(i) → skin(i) → top(i) → bottom(i) → shoes(i) → weapon(i) → female(byte)` —— 11 字段。
- s9 `CreateCharHandler`（line 41-53）读序：
  `name(str) → job(i) → face(i) → hair(i) → hairColor(i) → skinColor(i) → top(i) → bottom(i) → shoes(i) → weapon(i) → gender(byte)` —— 11 字段。
- **逐字段对齐** ✓（与 `CharCreationPackets.h:38` 注释"以 gms083 CreateCharHandler 为准"一致；hair + hairColor 增量规则见 r09 登录链路文档）。

### 2.3 DELETE_CHAR (23) — 删号
- 客户端 `DeleteCharPacket`（`Net/Packets/SelectCharPackets.h`）：`write_string("") + write_int(cid)`（enable_pic=false → pic 恒空）。带 PIC 变体 `DeleteCharPicPacket`：`write_string(pic) + write_int(cid)`。
- s9 `DeleteCharHandler.handlePacket`：`String pic = p.readString();` + `int cid = p.readInt();` + `c.checkPic(pic)`。
- **对齐** ✓（pic(str) + cid(int)；空 PIC 下 `checkPic("")` 在 enable_pic=false 时放行，r09 已确认）。

### 2.4 SELECT_CHAR (19) — 选角（登录服）
- 客户端 `SelectCharPacket`（`SelectCharPackets.h`）：`write_int(cid) + write_hardware_info()`。
- `write_hardware_info()`（`Net/OutPacket.cpp:127-134`）：`write_string(macs) + write_string(hwid)`。
- s9 `CharSelectedHandler`（line 53-57）：`readInt() + readString(macs) + readString(hostString)`（hostString 即 hwid，经 `Hwid.fromHostString` 解析）。
- **三段完全匹配** ✓。

### 2.5 PLAYER_LOGIN (20) — 进频道（频道服）
- 客户端 `PlayerLoginPacket`（`Net/Packets/LoginPackets.h:143-152`）：`write_int(cid)`（仅此一字段）。
- s9 `PlayerLoggedinHandler.handlePacket`（line 114）：`final int cid = p.readInt();`（仅读 int）。
- **对齐** ✓（S1 初版误将本包归因到 `CharSelectedHandler` 导致假阳性，详见 s1-login.md §2 纠正；r13 `14 00 01 00 00 00` 即此合法 int-only 形态）。

### 2.6 VIEW_ALL_CHAR (13) — 查看所有角色
- 客户端 `ViewAllCharPacket`（`SelectCharPackets.h`）：空包（仅 opcode）。
- s9 `ViewAllCharHandler.handlePacket`：未读包体（`p` 未使用），直接 `showAllCharacter` + `showAllCharacterInfo`。
- **对齐** ✓（空 ↔ 不读）。

### 2.7 REGISTER_PIC (29) — 注册 PIC
- 客户端 `RegisterPicPacket`（`SelectCharPackets.h`）：`skip(1) + write_int(cid) + write_hardware_info() + write_string(pic)`。
- s9 `RegisterPicHandler.handlePacket`：`readByte() + readInt(cid) + readString(macs) + readString(hostString)`（会话校验）+ `readString(pic)`。
- **对齐** ✓（`skip(1)`↔`readByte`；cid；`hardware_info`=macs+hwid ↔ macs+hostString；pic ↔ pic）。

### 2.8 SELECT_CHAR_PIC (30) — 带 PIC 选角
- 客户端 `SelectCharPicPacket`（`SelectCharPackets.h`）：`write_string(pic) + write_int(cid) + write_hardware_info()`。
- s9 `CharSelectedWithPicHandler.handlePacket`：`readString(pic) + readInt(cid) + readString(macs) + readString(hostString)`。
- **对齐** ✓。

### 2.9 PICK_ALL_CHAR (14) — 选角确认（2 步）
- s9 `RecvOpcode.PICK_ALL_CHAR = 0x0E (14)` 已注册，但**客户端全仓 grep `PickAllChar` 无命中**（无 `PickAllCharPacket`）——本端以 `SELECT_CHAR`/`PLAYER_LOGIN` 直接进入，不走 v83 的 2 步选角确认。
- 归类：gap-inventory 的"客户端从不发"项；属此私有端对查看所有角色流程的简化，**非缺陷**。

## 3. s2c 侧关键验收点（本切片范围）
- `CHARLIST`（选角列表）：客户端 `UICharSelect` 渲染 `has_rank`/名次/±箭头（r09 已确认，无需改）。
- `SERVER_IP`：驱动 `PlayerLoginPacket`（见 §2.5）。
- `ALL_CHAR_LIST`（查看所有角色）：`UIStateLogin::viewall_worlds` 消费（r10 已注册 `VIEW_ALL_CHAR`=8/13 双路径）。

## 4. 结论
- S2 全部 **8 个有效 c2s 包**（NAME_CHAR / CREATE_CHAR / DELETE_CHAR / SELECT_CHAR / PLAYER_LOGIN / VIEW_ALL_CHAR / REGISTER_PIC / SELECT_CHAR_PIC）**字段逐项对齐，0 处错配**。
- `PICK_ALL_CHAR` 客户端从不发（对 v83 查看所有角色流程的简化），非缺陷。
- 登录 → 选角 → 建删号 → 查看角色 → PIC 整链协议层与 s9 逐字节一致，验证完结。
- 本切片**未动任何代码**，符合"先对账、后改动、逐条给证据"的授权边界。
