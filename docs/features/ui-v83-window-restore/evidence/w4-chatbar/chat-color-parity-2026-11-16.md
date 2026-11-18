# 聊天：光标垂直几何 + Angel/MinMS 颜色对照（2026-11-16）

## 1. 输入态竖杠（caret）垂直几何

上一轮把 classic 输入文字 `text_shift` 从 `(0,-9)` 改为 `(0,-2)` 后，文字墨迹顶对齐 `position.y()+8`（定版模型 `ink_top = draw_y + linespace(17) − SimHei bitmap_top(11) = draw_y + 6`），在白带内区 `[0,26]` 居中；但竖杠仍偏低。

`IO/Components/Textfield.cpp:53-84`：

```cpp
Point<int16_t> absp = bounds.get_left_top();
absp.shift(position);
Point<int16_t> text_pos = absp + text_shift;
// ...
Point<int16_t> mpos = text_pos + Point<int16_t>(textlabel.advance(markerpos) - 1, marker_y/*=2*/) + marker_adjust;
marker.draw(mpos); // ColorLine(11, color, 1.0, vertical) → [mpos.y, mpos.y+11]
```

classic 几何（`UIChatBar.cpp` 构造 + `draw`）：bounds y = `position.y()+6`，`draw` 传 `(1,-2)`、`text_shift=(0,-2)` ⇒ `text_pos.y = position.y()+2`。

| 参数 | `mpos.y` | 竖杠占 | 文字墨迹占 | 结论 |
|---|---|---|---|---|
| 旧 `marker_adjust=(1,8)` | `position.y()+12` | `[12,23]` | `[8,19]` | 低 4px、贴槽底 |
| 新 `marker_adjust=(1,4)` | `position.y()+8` | `[8,19]` | `[8,19]` | 完全重合、带内居中 |

modern 分支为另一套 `text_shift=0` 几何，未改。

## 2. Angel 侧取证（IDA `0x84ec53` `CUIMessenger::AddChatText`）

从 `E:/MXD/反编译IDA/v83/\`Angel.i64`（IDA GUI 会话 13339，REA 直连）反编译确认：

```c
if (nIdx == -1) v38 = 1;
else if (nIdx == 0) v38 = 11;
else if (nIdx == 1) v38 = 6;
else if (nIdx == 2) v38 = 13;
else v38 = 1;
// *(chatline_obj + 4) = v38;  →  CChatLog 构造的 nType
```

- `CChatLog::CChatLog@0x8db485` 把该 `nType` 以 `_ZtlSecureTear` 写入对象 `+4`；`CChatLog::Draw@0x8dbb93` 读取它。
- **RGB 语义无权威解码**：`ChatLogDraw` 的反编译受 MCP 响应截断（47k 字符），`func_profile` 常量表虽出现 `0x1/0x6/0xb/0xd`，但无法据此唯一确定各索引对应的具体颜色。仓库内亦无文档把 1/6/11/13 解码为色值。
- **没有 GM 专属色**：`nIdx` 仅 −1/0/1/2 四个值，无 GM 分支。

结论：Angel 只能证实「按类型取 4 种颜色索引」，不能作为具体 RGB 的裁决依据；具体色值以 MinMS 源码为准。

## 3. MinMS 侧取证（`E:/project/MinMS`）

`src/client/IO/UITypes/UIChatBar.h:51-60`：`LineType { UNK0, WHITE, RED, BLUE, YELLOW, GREY }`。
`src/client/IO/UITypes/UIChatBar.cpp:506-531` 色值映射；RGB 见 `Graphics/GraphicsGL.cpp:1467-1484`：

| LineType | Text 色 | RGB | 本端对应 `Color::Name` |
|---|---|---|---|
| WHITE/UNK0 | WHITE | `#FFFFFF` | `WHITE` |
| RED | DARKRED | `#FDA9A9` | 无精确项（`DARKRED` 本端 `#CC4D4D`） |
| BLUE | MEDIUMBLUE | `#00BFFF` | `MEDIUMBLUE` 精确一致 |
| YELLOW | YELLOW | `#FFFF00` | `YELLOW` 精确一致 |
| GREY | CHATGREY | `#B9B9B8` | 无精确项 |

逐项证据（入站 / 本地自显）：

- ALL：`ChatReceivedHandler` `MessagingHandlers.cpp:866-867` → WHITE；本地 `UIChatBar.cpp:982-987` 只发包+气球，不预写行。
- WHISPER：无入站处理器（`PacketSwitch.cpp:321-332`）；本地 `UIChatBar.cpp:1028-1029` → WHITE。
- PARTY：入站 `MultiChatReceivedHandler` mode=1 `MessagingHandlers.cpp:910-913` → BLUE；本地 `send_targeted_message()` → WHITE。
- BUDDY：入站 mode=0 `MessagingHandlers.cpp:906-909` → BLUE；本地 → WHITE。
- GUILD / ALLIANCE：mode=2/3 `MessagingHandlers.cpp:914-919` → WHITE。
- SPOUSE：只有发包，无入站聊天行；本地 → WHITE。
- MEGAPHONE：`ServerMessageHandler` type=3 只消费字段（`:802-806`），type=4 走滚动公告；**不产生聊天行**。
- GM：`ChatReceivedHandler` 读 `gm` 布尔（`:850`），注释为 "GM white balloon background"，聊天行 `:866-867` 无条件 WHITE；`Character::speak()` 也不接收 GM 参数。
- 服务器通知 0/5/6：`MessagingHandlers.cpp:789-825` → WHITE；MapleTip `WeekEventMessageHandler :833-844` → YELLOW。
- RED/GREY 是状态提示（组队失败、卷轴物品提示 / 疾病中毒），不是频道色：`:978-1150`、`PlayerHandlers.cpp:125-158`。

## 4. 本端收敛结果

| 频道 | 旧值 | 新值 | 依据 |
|---|---|---|---|
| ALL | WHITE | WHITE | 一致 |
| WHISPER | BLUE | WHITE | MinMS 本地自显 WHITE，无入站 |
| PARTY | GREEN | MEDIUMBLUE | MinMS 入站 BLUE=#00BFFF |
| GUILD | ORANGE | WHITE | MinMS mode=2 WHITE |
| BUDDY | LIGHTBLUE | MEDIUMBLUE | MinMS 入站 BLUE |
| ALLIANCE | MALIBU | WHITE | MinMS mode=3 WHITE |
| SPOUSE | PINK | WHITE | MinMS 本地 WHITE，无入站 |
| MEGAPHONE | SUPERNOVA | SUPERNOVA（保留） | MinMS 不产生聊天行，无对应证据 |
| SYSTEM | YELLOW | WHITE | MinMS 服务器通知 0/5/6 WHITE |
| GM | VIOLET | WHITE | MinMS GM 聊天行 WHITE；Angel 无 GM 色 |

`MessageType::GM` 本地 `!`/`@` 回显同由 VIOLET 改 WHITE。

改动文件：`IO/UITypes/UIChatBar.cpp`（`show_message`、`channel_color`、classic `draw`）、`IO/UITypes/UIChatBar.h`（枚举注释）、`Net/Handlers/MessagingHandlers.cpp`（注释）。
