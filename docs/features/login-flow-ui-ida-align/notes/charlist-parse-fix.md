# CHARLIST 包体解析对齐（登录链路「卡在正在连接到服务器。」根因与修复）

## 现象

选世界 → 选频道 → 点「到选择的世界去」后，服务端 **`CHARLIST(0x0B)` 已正确下发**，
但界面停在 `UILoginWait`（「正在连接到服务器。」）永不推进，选角色界面进不去。

## 定位过程

1. 在 `Net/PacketSwitch.cpp` **临时**加包体十六进制转储，抓到真实字节（已移除）：

   ```
   [PKT] opcode=11 payload_len=153 hex=0B 00 00 01 01 00 00 00 41 64 6D 69 6E ...
   ```

   `CharlistHandler` 入口日志只打印了一行、解析后的第二行从未出现 → **在 `parse_charentry` 抛 `PacketError`**。

2. 用已知锚点（`face=20000` @23/99、`hair=30030` @27/104、`FF FF` @128、
   尾 `02 03 00 00 00` @148）反推字段边界，总长 **153 字节完全吻合**：

   | 字段 | 偏移 | 宽度 | 实测值 |
   |---|---|---|---|
   | id | 4 | 4 | 1 |
   | name | 8 | 13 | "Admin" |
   | gender / skin | 21 / 22 | 1 / 1 | 0 / 0 |
   | face | 23 | 4 | 20000 |
   | hair | 27 | 4 | 30030 |
   | petid ×3 | 31 | 8×3 | 0 |
   | **level** | 55 | **1** | 1 |
   | job | 56 | 2 | 0 |
   | str / dex / int / luk | 58 / 60 / 62 / 64 | 2 | 12 / 5 / 4 / 4 |
   | hp / maxhp / mp / maxmp | 66 / 68 / 70 / 72 | 2 | 50 / 50 / 5 / 5 |
   | ap / sp | 74 / 76 | 2 / 2 | 0 / 0 |
   | **exp** | 78 | **8** | 0 |
   | fame | 86 | 2 | 0 |
   | gachaExp | 88 | 4 | 10000 |
   | mapId | 92 | 4 | 2 |
   | spawnPoint | 96 | 1 | 0 |
   | charLook | 97 | 49 | 含 4 件装备(slot 5/6/7/11) |
   | rankInfo ×2 | 146 / 147 | 1 / 1 | 0 / 0（GM 不计排名） |
   | pic / slots | 148 / 149 | 1 / 4 | 2 / 3 |

   → **北斗包体在此没有任何尾部 timestamp(int)**。

3. 与旧 `parse_stats` 的三处差异：

   | 字段 | 旧实现 | 北斗实际 |
   |---|---|---|
   | level | `read_short()` (2) | **`read_byte()` (1)** |
   | exp | `read_int()` (4) | **`read_long()` (8)** |
   | 尾部 | `skip(4)` timestamp | **无** |

   净差 −1 字节 → `parse_look` 整体前移，等值槽位循环读不到 `0xFF` 终止符 → 越界抛错。

## 修复

`Net/Handlers/Helpers/LoginParser.cpp` → `LoginParser::parse_stats`：

- `stats[LEVEL]` 改为 `read_byte()`
- `exp` 改为 `read_long()`（`StatsEntry::exp` 本就是 `int64_t`，无需改结构）
- 删除尾部 `recv.skip(4); // timestamp`

## 复验

`_ws_debug.log`：

```
[WS] CharlistHandler reached, available=1 loginwait=Y active=Y
[WS] CharlistHandler: chcount=1 slots=3 pic=2
[WS] CharlistHandler: UICharSelect emplaced
```

与包体解码结果（charcount=1 / slots=3 / pic=2）完全一致 → 选角色界面成功创建。
截图：`evidence/blocked_charselect_stalled.png`（修复前停住）→ `evidence/charselect_reached.png`（修复后已进入）。

## 修复后新暴露的问题（下一轮）

1. **选角色界面按 800x600 基准摆放、未随分辨率居中**：`UICharSelect` 的
   `position` 固定 `(0,0)`、`dimension` 固定 `(800,600)`，控件混用 `position` 偏移与
   NX 绝对坐标（例如 `namelabel.draw(namePos)` 不带 `position`），因此**不能靠整体平移修**，
   需逐项校准。
2. **step=2 背景空白**：`LoginBackground` 在 step=2 时森林层滚到 `viewh-8-1200 = -488`、
   地图物件层相机到 `viewh/2+1208`，两块都出了地图范围，只剩书页外框
   （见 `evidence/charselect_reached.png`）。原版选角色页应有自己的背景板，
   需按 `Login.img/CharSelect` 的真实节点补画。
