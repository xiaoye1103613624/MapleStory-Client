# T9 日志配对报告

- 客户端：docs/features/s9-protocol-align/evidence/pairing/client-packet_dump-20261031-0055.monitor-f1.log
- 服务端：docs/features/s9-protocol-align/evidence/pairing/server-out-20261031-0055.monitor-f1.log
- 前瞻窗口：50 包；档 A/B 镜像去重：0 行

## 结论

- **c2s**: 配对 11（EXACT 11 / DIFF 0），客户端未配对 3，服务端未配对 0，长度不符 0
- **s2c**: 配对 0（EXACT 0 / DIFF 0），客户端未配对 13，服务端未配对 0，长度不符 0
- 客户端无方向标记的历史行（T9 之前格式）：0
- 服务端超阈值截断行（入站 3000 / 出站 50000，无法配对）：0
- 服务端无法识别的行：2

## c2s 配对明细

| # | opcode | name | match | client len | server len | client line | server line/time | note |
|---|---|---|---|---|---|---|---|---|
| 1 | 0x18 | PONG | EXACT | 2 | 2 | C:L2 | S:L3 2026-10-31 00:55:42.254 |  |
| 2 | 0x18 | PONG | EXACT | 2 | 2 | C:L8 | S:L4 2026-10-31 00:56:12.270 |  |
| 3 | 0x78 | WHISPER | EXACT | 23 | 23 | C:L11 | S:L5 2026-10-31 00:56:34.116 |  |
| 4 | 0x78 | WHISPER | EXACT | 10 | 10 | C:L14 | S:L6 2026-10-31 00:56:40.266 |  |
| 5 | 0x77 | MULTI_CHAT | EXACT | 15 | 15 | C:L16 | S:L7 2026-10-31 00:56:46.524 |  |
| 6 | 0x77 | MULTI_CHAT | EXACT | 15 | 15 | C:L17 | S:L8 2026-10-31 00:56:52.779 |  |
| 7 | 0x18 | PONG | EXACT | 2 | 2 | C:L19 | S:L9 2026-10-31 00:57:22.783 |  |
| 8 | 0x18 | PONG | EXACT | 2 | 2 | C:L21 | S:L10 2026-10-31 00:57:52.795 |  |
| 9 | 0x18 | PONG | EXACT | 2 | 2 | C:L23 | S:L11 2026-10-31 00:58:22.804 |  |
| 10 | 0x18 | PONG | EXACT | 2 | 2 | C:L25 | S:L12 2026-10-31 00:58:52.816 |  |
| 11 | 0x18 | PONG | EXACT | 2 | 2 | C:L27 | S:L13 2026-10-31 00:59:22.820 |  |


## c2s 客户端未配对

| client line | opcode | name | len | hex | 原因 |
|---|---|---|---|---|---|
| L3 | 0x31 | GENERAL_CHAT | 19 | 31 00 0E 00 21 6D 6F 6E 69 74 6F 72 20 41 64 6D 69 6E 01 | no server record with same opcode in window |
| L5 | 0x31 | GENERAL_CHAT | 14 | 31 00 09 00 21 6D 6F 6E 69 74 6F 72 73 01 | no server record with same opcode in window |
| L10 | 0x18 | PONG | 2 | 18 00 | no server record with same opcode in window |


## s2c 配对明细

| # | opcode | name | match | client len | server len | client line | server line/time | note |
|---|---|---|---|---|---|---|---|---|


## s2c 客户端未配对

| client line | opcode | name | len | hex | 原因 |
|---|---|---|---|---|---|
| L1 | 0x11 | PING | 2 | 11 00 | no server record with same opcode in window |
| L4 | 0x4D | SET_WEEK_EVENT_MESSAGE | 17 | 4D 00 FF 0A 00 31 20 D2 D1 B1 BB BC E0 BF D8 00 00 | no server record with same opcode in window |
| L6 | 0x4D | SET_WEEK_EVENT_MESSAGE | 21 | 4D 00 FF 0E 00 41 64 6D 69 6E 20 D2 D1 B1 BB BC E0 CC FD 00 00 | no server record with same opcode in window |
| L7 | 0x11 | PING | 2 | 11 00 | no server record with same opcode in window |
| L9 | 0x11 | PING | 2 | 11 00 | no server record with same opcode in window |
| L12 | 0x87 | WHISPER | 25 | 87 00 12 05 00 41 64 6D 69 6E 00 01 0B 00 77 68 69 73 70 65 72 2D 74 32 36 | no server record with same opcode in window |
| L13 | 0x87 | WHISPER | 11 | 87 00 0A 05 00 41 64 6D 69 6E 01 | no server record with same opcode in window |
| L15 | 0x87 | WHISPER | 15 | 87 00 09 05 00 41 64 6D 69 6E 02 FF FF FF FF | no server record with same opcode in window |
| L18 | 0x11 | PING | 2 | 11 00 | no server record with same opcode in window |
| L20 | 0x11 | PING | 2 | 11 00 | no server record with same opcode in window |
| L22 | 0x11 | PING | 2 | 11 00 | no server record with same opcode in window |
| L24 | 0x11 | PING | 2 | 11 00 | no server record with same opcode in window |
| L26 | 0x11 | PING | 2 | 11 00 | no server record with same opcode in window |

