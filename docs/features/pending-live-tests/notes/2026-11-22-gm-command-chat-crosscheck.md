# 2026-11-22 GM 命令与聊天链路三端交叉检查（只读）

> 用户报 `!item 1112429 1` 无反应 / 聊天框无反馈。本文为只读取证：**未改任何源码**。工具：源码逐行、s9 服务端、MinMS 源码、192 WZ（orange-wz）、Angel 既有 IDA 产物（REA 当前 IDA provider `search_strings` 返回 malformed sha256，本次不依赖新反编译）。

## 1. GM 命令不是客户端实现——三端一致：客户端只发聊天文本，服务端解析

| 端 | 证据 | 结论 |
|---|---|---|
| 本端客户端 | `IO/UITypes/UIChatBar.cpp:2989` `GeneralChatPacket(message, true)`；`:2995-2997` 仅对 admin 做本地紫字回显；全仓无 `!item` 分支 | `!item` 走普通聊天包 `GENERAL_CHAT(0x31)`，客户端不解析 |
| s9 服务端 | `GeneralChatHandler.java:53-55` `isCommand` → `CommandsExecutor.handle`；`CommandsExecutor.java:53-54` `!`=GM 前缀；`:260` `addCommand("item",2,ItemCommand.class)`；`gm2/ItemCommand.java:44-104` 解析 id/数量/发背包/回 `dropMessage(5)` | 命令在服务端执行，成功/失败都经 SERVERMESSAGE(0x44) 回显 |
| MinMS（v83 client-only） | `src/client/IO/UITypes/UIChatBar.cpp:976-1037` 发包分流对 `!` 无特殊判断；仅 `/` 走 `handle_party_command()`；`docs/ms-network-protocol.md:1045` 明示 `!` 由服务端 command handler 处理；无 ADMIN_CHAT 发包 | 与本端架构一致，印证"原生客户端不实现 GM 命令" |
| 192 / Angel | 192 为打包 WZ + 原生客户端，GM 命令同样不在 IMG 资源内；Angel 既有 IDA 产物 `REA反编译/chat/startscript_71e1d6.c` 为脚本引擎 `startscript`，非命令解析器；静态搜索 `!item` 无命中 | 无证据表明原生客户端内置命令解析器；不能把命令失效归因于"客户端 GM 模块" |

**结论**：不存在"原生客户端 GM 命令模块"可对照恢复；本端架构与 MinMS / 原生一致，NO-FIX。

## 2. `!item 1112429 1` "无反应"的可解释路径（按优先级）

1. **道具 ID 不属于 v83 数据**：`1112429` 是 192 版本道具编号；s9 v83 `ItemInformationProvider.getName(1112429)` 很可能为 null → `ItemCommand.java:56-59` 回 `yellowMessage`（SERVERMESSAGE type5）。若该回包被聊天显示吞掉，就表现为"什么反应也没有"。**这是最可能原因，需实机/服务端日志确认**。
2. **非"对所有人"目标改道**：`UIChatBar.cpp:2972-2987` 经典 HUD 下若当前目标是密语/组队/公会等，`!item` 被 `send_to_target_channel()` 改发 MultiChat/Whisper 并 return，**服务端不会当命令**。若密语无对象则 CHANNEL_HELD，文本留在输入行、完全不发（`:2980-2985`），和"无反应"完全吻合。
3. **无本地回显**：`:2995-2997` 仅 `get_admin()`(=gmLevel>1) 回显紫字；非 admin 或回显条件不满足时，服务端执行了但看不到自己输入。
4. **历史注入限制（非真实用户输入）**：`pending-live-tests/notes/gm-prereq-commands.md:28-39` 四种脚本注入均失败（SendKeys `!`/空格丢失，`31 00` 包文为 `level30`），属自动化键位映射限制，不等于手输了也不行。
5. **回包存在但UI未显示**：见第 3 节。

## 3. 聊天功能 / 历史 / 样式检查

### 3.1 收发链路（本端）
- 输入：`UIChatBar.cpp:246-251` Enter → `input_text_enter_callback` → `:2989` GeneralChat 或密语/组队等分流；`:3002-3003` 发送后保持输入框聚焦。
- 收包：`Net/Handlers/ChatHandlers.cpp` 处理 CHATTEXT/MULTI_CHAT/WHISPER；`show_message` 写入 `message_history`。
- 与 MinMS 同构：MinMS `UIChatBar.cpp:163-177/976-1037`、`MessagingHandlers.cpp:847-929`。

### 3.2 历史记录显示（本端 classic 分支，已含一处历史修复）
- 收起态 `UIChatBar.cpp:552-576`：只画 `message_history.back()` 一条。
- 展开态 `:579-600`：半透明底板 + 滚动条 + 倒序逐行。
- 输入态 `:601-627`：**已有修复**——注释明确"过去完全不画 message_history，服务端 GM 命令回包和本地 !/@ 回显都进数组却看不见，表现成 `!item` 服务端执行了却没结果"；现补最近 4 行。说明这类"无反馈"曾发生过并已修，若再现需查是否走输入态/折叠态分支。
- MinMS 对照：`UIChatBar.h:190` `unordered_map<int16_t,Text> rowtexts` 无裁剪上限；`UIChatBar.cpp:262-309` 绘制；折叠 `chatopen` 全仓无外部置 false，切换实际未接线（比本端更弱）。

### 3.3 样式
- 本端：classic 目标框/输入带/历史行见 `UIChatBar.cpp:225-320`（A11M/LEFT/WHITE、ColorBox 半透明、行距常量）；modern 走 StatusBar3 资源（`:324-334`）。
- MinMS：`UIChatBar.h:136-147` 行高 16、最多 16 行、文本宽 480、字体 A11M/A12M；`UIChatBar.cpp:513-537` 频道颜色映射；`GraphicsGL.cpp` 字体与色表。
- 192/Angel：样式来自 IMG（StatusBar.img/StatusBar3.img + chat 子节点），由 orange-wz 资产侧对照，非代码常量；本端已按此路线校准过（见 evidence/wz-statusbar3-chat-qs-2026-11-19.md）。

## 4. 决策点与建议 Plan（待用户拍板，本次未改代码）

- P0 确认（不需改码）：请在游戏内**手输**（非脚本）`!item <一个 v83 存在的道具ID> 1`，并把聊天目标切回「对所有人」；同时看服务端 `logs/out.log` 是否出现 `ItemCommand` 执行日志。若有效 → 问题是此前注入/目标/ID 三者之一，代码 NO-FIX。
- P1 若仍无反馈：开 `debug.packet_dump` 抓 `31 00` 聊天包与 `44 00` SERVERMESSAGE 回包，区分"没发出"还是"没显示"。
- P2 代码修复候选（按 P0/P1 定因择一，不盲目全做）：① 命令前缀在 `send_to_target_channel` 之前强制走 GeneralChat；② admin 本地回显门禁放宽到能覆盖症状；③ 输入态/折叠态历史显示条数或优先级调整。
- P3 聊天历史增强（可选，用户未要求）：历史条数上限、折叠按钮接线等对齐 MinMS 行为。
