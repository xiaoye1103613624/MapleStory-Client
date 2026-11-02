# S1 登录切片 —— 字段级对账

> 切片范围（PLAN §4-P2 S1）：握手后 `LOGIN_PASSWORD`/`LOGIN_EMAIL` → `LOGIN_STATUS`/`ACCOUNT_INFO`/`SERVERLIST` → `CHARLIST_REQUEST`/`SERVERSTATUS_REQUEST` → `ACCEPT_TOS`/`SET_GENDER` → `PLAYER_LOGIN` → `SERVER_IP`。
> 方法：客户端 `Net/Packets/LoginPackets.h` 各 `OutPacket` 构造序列 ↔ s9 `net/server/handlers/login/*` 的 `InPacket` 读序列，逐字段对拍；s2c 侧以真机会话 #1（`notes/real-session-1.md` + `evidence/pairing/client-packet_dump-20261030-2029.redacted.log`）的“44 s2c 全解全派”为已验收证据，body 级细查留待各 s2c handler 单独切片。
> 字符集：s9 出站在本部署按 GBK（`T10` 已定盘、`T11` 接缝已落地），客户端经 `Net/PacketCharset` 转 UTF-8 后渲染——本切片所有字符串字段不受影响。

## 1. c2s 包清单与字段结论

| client opcode | s9 opcode | s9 handler | 客户端构造（`LoginPackets.h`） | s9 读取序列 | 结论 |
|---|---|---|---|---|---|
| 1 `LOGIN` | 0x01 `LOGIN_PASSWORD` | `LoginPasswordHandler` | `write_string(acc); write_string(pass); skip(6); write_byte(h);write_byte(w);write_byte(i);write_byte(d)` | `readString()`×2；`skip(6)`；`readBytes(4)` | **✅ 逐项对齐** |
| 3 `LOGIN_EMAIL` | —（s9 无此常量） | 无 handler | 同 `LOGIN_PASSWORD` 体，仅 opcode=3 | null 分支忽略 | ⚠ **客户端独有**（PLAN §2 F5）：s9 `RecvOpcode` 无 `LOGIN_EMAIL`，发出后服务端走 null 分支只 `updateLastPacket()`，零副作用；建议后续从 `OutPacket.h` 删除以免误导 |
| 5 `CHARLIST_REQUEST` | 0x05 `CHARLIST_REQUEST` | `CharlistRequestHandler` | `write_byte(0); write_byte(world); write_byte(channel)` | `readByte()`(丢弃0)；`readByte()`(world)；`readByte()+1`(channel) | **✅ 对齐**（channel 客户端 0 基、服务端 +1，符合 v83 约定） |
| 6 `SERVERSTATUS_REQUEST` | 0x06 `SERVERSTATUS_REQUEST` | `ServerStatusRequestHandler` | `write_short(world)` | `readShort()`(world) | **✅ 对齐** |
| 7 `ACCEPT_TOS` | 0x07 `ACCEPT_TOS` | `AcceptToSHandler` | `write_byte(1)` | `readByte()` 须 ==1（否则断连）；其后 `acceptToS()` | **✅ 对齐** |
| 8 `SET_GENDER` | 0x08 `SET_GENDER` | `SetGenderHandler` | `write_byte(1); write_byte(female)` | `readByte()`(confirmed==1)；`readByte()`(gender) | **✅ 对齐** |
| 11 `SERVERLIST_REQUEST` | 0x0B `SERVERLIST_REQUEST` | `ServerlistRequestHandler` | 空包（仅 opcode） | 不读，直接回 SERVERLIST | **✅ 对齐**（无 body） |
| 20 `PLAYER_LOGIN` | 0x14 `CHAR_SELECT` | `CharSelectedHandler` | `write_int(cid)`（**仅 4 字节**） | `readInt()`(charId)；`readString()`(macs)；`readString()`(hostString) | **❌ 字段数错配**（见 §2） |

## 2. 发现：PLAYER_LOGIN 字段数「错配」= 误报（opcode 归因错误，已纠正）

**客户端（`Net/Packets/LoginPackets.h:143-152` + 调用点 `LoginHandlers.cpp:342`）**
```cpp
PlayerLoginPacket(int32_t cid) : OutPacket(OutPacket::Opcode::PLAYER_LOGIN) {
    write_int(cid);   // 仅此一个字段
}
```
触发点：`ServerIPHandler::handle`（收到 s2c `SERVER_IP` 后）读 cid 并 `PlayerLoginPacket(cid).dispatch()`——**不发 macs/hostString**。

**s9（`CharSelectedHandler.java:53-57`）**
```java
int charId = p.readInt();
String macs = p.readString();
String hostString = p.readString();
```
期望 `int + string + string` 三段。

**纠正：初版 S1 把 0x14 的 int-only 包误归因到 `CharSelectedHandler`（0x13 的 handler），从而错误地得出"源码/运行分叉"结论。实际 0x14 ≠ `CharSelectedHandler`。**

**opcode 归位（s9 `RecvOpcode.java:42-46`）**：
- `CHAR_SELECT = 0x13 (19)` → `CharSelectedHandler`（登录服，PacketProcessor.java:148 注册）
- `PLAYER_LOGGEDIN = 0x14 (20)` → `PlayerLoggedinHandler`（频道服，PacketProcessor.java:183 注册）

客户端 `OutPacket.h`：`SELECT_CHAR = 19`、`PLAYER_LOGIN = 20` —— 值与 s9 逐一对应、**名值全匹配**。

**两条路径各自对齐**：
1. **登录服 `CHAR_SELECT (0x13)`**：客户端 `SelectCharPacket`（SelectCharPackets.h:26）= `write_int(cid) + write_hardware_info()`；`write_hardware_info()`（OutPacket.cpp:127-134）写 `macs string + hwid string`。服务端 `CharSelectedHandler.readInt() + readString(macs) + readString(hostString)`（hostString 即 hwid，经 `Hwid.fromHostString` 解析）—— **三段完全匹配** ✓
2. **频道服 `PLAYER_LOGGEDIN (0x14)`**：客户端 `PlayerLoginPacket`（LoginPackets.h:143-152）= `write_int(cid)` 仅此一字段；服务端 `PlayerLoggedinHandler.handlePacket`（line 114）= `final int cid = p.readInt();` **只读 int** ✓

**结论（RESOLVED：误报，非缺陷）**：r13 的 `14 00 01 00 00 00` 正是频道服 `PLAYER_LOGGEDIN` 的合法形态（int-only 符合其 handler），并非 `CharSelectedHandler` 的入站；登录服那条 `CHAR_SELECT` 包客户端经 `write_hardware_info()` 发了 macs+hwid 双串，与 `CharSelectedHandler` 完全吻合。S1 原"PLAYER_LOGIN 缺发 macs/hostString 的字段错配"是 **opcode 误归因导致的假阳性，已纠正**——**客户端登录链路 8 个 c2s 包全部字段对齐，0 处错配**。无需任何回归处置，客户端维持现状、不改。

## 3. s2c 侧（本切片范围，沿用真机会话 #1 验收）

| s9 opcode | 客户端 PacketSwitch 处理 | 状态 |
|---|---|---|
| `LOGIN_STATUS`（登录结果/AuthSuccess） | `LoginResultHandler` | ✅ 真机进 `getAuthSuccess` 链（含 `account.female` 回填，r09/r10） |
| `SERVERLIST` | `ServerListHandler` | ✅ 真机渲染大区/频道 |
| `CHARLIST` | `CharlistHandler` | ✅ 真机 `emplace<UICharSelect>` |
| `SERVER_IP` | `ServerIPHandler` | ✅ 真机读 cid → 发 `PLAYER_LOGIN`（见 §2） |
| `SERVERSTATUS`（拥挤度） | `ServerStatusHandler` | ✅（s9 仅写 1 个 short，world 不存在回 2） |
| `SET_GENDER`/`ACCEPT_TOS` 回包（AuthSuccess） | 同 `LoginResultHandler` | ✅ |
| 进频道后 `PLAYER_LOGGEDIN`/`SET_FIELD` 等 | `LoginHandlers`/`MapHandlers` | ✅ 真机 44 s2c 全解全派 |

字符集：以上 s2c 字符串（角色名/职业/世界名等）服务端按 GBK 发，客户端 `T11` 接缝转 UTF-8 渲染；`notes/real-session-1.md §5` 已记录 `登录了游戏`/`蘑菇王国` 等 GBK 实测可读。

## 4. 回归纪律

S1 触碰最敏感链路（登录→选角→进图）。任何改动后必须跑一次 登录→选角→进图 回归（PLAN §4-P2 末段）。`PLAYER_LOGIN` 在 §2 裁决前**不改动**，以免破坏已联调链路。

## 5. 结论

- 8 个 c2s 包**全部字段逐项对齐**（含 HWID 4 字节 nibble 顺序、`skip(6)` 占位、channel +1 约定、建号 11 字段顺序、`CHAR_SELECT`/`PICK_ALL_CHAR` 的 macs+hwid 双串、`PLAYER_LOGGEDIN` 的 int-only），S1 的 c2s body 层已实质对齐。
- `PLAYER_LOGIN` 字段「错配」原是**误报**（详见 §2）：r13 的 `14 00 01 00 00 00` 是频道服 `PLAYER_LOGGEDIN`（opcode 0x14）的合法 int-only 形态，其 handler `PlayerLoggedinHandler` 本就只读 int；登录服 `CHAR_SELECT (0x13)` 那条客户端经 `write_hardware_info()` 发了 macs+hwid 双串，与 `CharSelectedHandler` 完全匹配。S1 初版把 0x14 误归因到 `CharSelectedHandler` 导致假阳性，已纠正——**0 处错配**。
- 附带清理建议：`LOGIN_EMAIL=3` 为客户端独有、s9 无 handler 的死枚举，建议删除。
- 本切片**未动任何代码**，符合“先对账、后改动、逐条给证据”的授权边界。
