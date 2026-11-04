# chat-system-overhaul（Q0–Q6 最终实现逻辑）

> 会话接续说明：本文档承接 CodeBuddy 会话 `476ee7afb9d843af8694a4b91433b780`（traceId `d90d330c…`）。
> 该会话完成了 Q0–Q3（todos 全部 completed、plan status=finished），随后用户追问扩出
> Q4/Q5/Q6；其 `plan_create` 连续 4 次返回 `AI_APICallError-Bad Request`（错误码 11115，
> 网关级故障，与内容长短无关——最小纯文本版同样失败），死在计划提交前。
> 2026-11-04 由 DSH 会话读取其 plans/todos/messages 落盘文件复原中断点，按既有设计续作完成。

## 范围与裁决

| # | 内容 | 状态 |
|---|---|---|
| Q0 | 发送后保焦连续输入（UIChatBar） | 前会话已完成（本轮把斜杠分支的 close_input 统一收口到 keep_input_focus） |
| Q1 | ChatChannel 十频道配色 | 前会话已完成 |
| Q2 | 名字热区点击菜单（密语/查角色/加好友） | 前会话已完成 |
| Q3 | 折叠态开历史 + 未最大化滚动 | 前会话已完成 |
| Q4 | 聊天历史上限 **200** | 本轮完成 |
| Q5 | 频道下拉扩至 **8 项** + Enter 按所选频道**路由发送** + 本地回显 | 本轮完成 |
| Q6 | 其余聊天 UI 审计（UIChat/UIMegaphone/UIWhisperTargetDlg/ChatBalloon） | 本轮完成（1 处定向修复 + 审计结论归档） |

### 用户裁决记录
- 历史上限：仓库无原生常量（全代码搜索无 cap），按用户兜底值 **200**；`user_message_history`（↑/↓ 输入历史）同夹。
- 「只要涉及聊天功能或关联的，不管代码还是 UI 都按原生客户端修复」（2026-11-04 追问，即 Q6 的依据）。

## 核心设计：s9 服务端回显矩阵（本轮一手证据，决定本地回显边界）

读 `BeiDou-Server_s9` handler 源码坐实（文件:行见 evidence/server-echo-matrix.md）：

| 频道 | 服务端是否把消息发回发送者 | 客户端本地回显 |
|---|---|---|
| General | ✅ `MapleMap.broadcastMessage(packet)` source=null 不排除 | ❌ 禁（否则双份） |
| Party | ❌ `World.partyChat` 显式 `if(!name.equals(namefrom))` | ✅ 必 |
| Guild | ❌ `Guild.guildChat→broadcast(packet,cid)` exception=自己 | ✅ 必 |
| Alliance | ❌ `allianceMessage(…, player.getId(), …)` | ✅ 必 |
| Buddy | ❌ 只发收件人 | ✅ 必 |
| Spouse | ✅ `SpouseChatHandler` 对 `c` 本人也 sendPacket | ❌ 禁 |
| Whisper | 只回 `WhisperResult` 标志、不回正文 | ✅ 成功时才回显（pending 暂存机制） |
| Megaphone | ✅ `Server.broadcastMessage(world,…)` 全服含自己 | ❌ 禁 |

**由此发现的存量缺陷**（前会话 Q1 只做了收包着色，没做发送回显）：组队/家族/联盟/好友频道自己说的话**从不进本地聊天历史**——只有别人看得见你说话。Q5 一并修复。

## 实现清单

### Q4（UIChatBar.h/.cpp）
- `MESSAGE_HISTORY_CAP=200` + `trim_message_history()`：三个 `show_message` 重载 push_back 后统一裁剪（从最旧擦；offset 是「距最新」语义，不影响贴底）。
- `input_text_enter_callback` 内 `user_message_history` 同夹 200。

### Q5（UIChatBar.h/.cpp + locale + MessagingHandlers）
- `ChatTargetIndex` 4→8 项（ALL/WHISPER/PARTY/GUILD/BUDDY/ALLIANCE/SPOUSE/MEGAPHONE）；下拉高度/命中/位置全由 TARGET_COUNT 驱动，零额外布局改动。
- `locale/*.ini` 补 4 键×双侧（前 4 项原生 v83 文案；后 4 项 NO-NATIVE 措辞，沿用决策 B）。
- `send_to_target_channel(text)` 三态返回：SENT / HELD（密语没对象→弹对象层、文本留输入行；喇叭→开 `UIMegaphoneBasic` 窗）/ FALLBACK（→GeneralChat 兜底）。
- 斜杠命令保留为别名，路由同矩阵；`/w` 走 `send_whisper`（正文暂存 `pending_whisper_*`），`WhisperResultHandler` ok=true 时经 `on_whisper_result` 回显 `(To X) 我 : 正文`（对齐原生，替代旧 "Whisper sent to X" 白字行）；失败仍红字。
- `echo_own_message`：与收包端同构前缀 `[Party] ` 等 + 自身角色名 → 自己看到的行与他人看到的行一致；名字可点沿用 Q2。
- `collect_online_buddy_cids`：好友收件人取自 UIBuddyList（名单只来自 updateBuddylist 推送，窗没开过=空表=静默失败，与原生一致，已在注释钉死数据边界）。
- `keep_input_focus()`：斜杠分支原先各自 `close_input()`，与 Q0 连续输入口径冲突，统一收口。
- `set_whisper_target` 同步记录 `whisper_target_name`（下拉悄悄话直发的目标来源；UIWhisperTargetDlg.confirm 原本就回调这里，链路自然闭合）。

### Q6（审计结论）
- **UIChat（socialChatEnter）**：命中同类缺陷——F/B 键与 CHAT_FRIEND 按钮、P/G/A 键共 5 处 MultiChat 软键盘出口全部无本地回显。定向修复：文件内 `echo_to_chatbar` helper 复用 UIChatBar public 的 `show_message(prefix,speaker,text,ch)`，**不扩接口、不动发送语义**。Duo→UIMessenger、Random→FindPlayer 属另两套协议面，不在聊天历史矩阵内，未动。
- **UIMegaphoneBasic**：`find_megaphone(5071000/5072000/5076000)` + `UseMegaphonePacket(slot,id,msg,whisper)` 与 s9 `UseCashItemHandler case 2/6` 字节序对得上；世界广播含自己，无需回显。结论：对齐，不动。
- **UIWhisperTargetDlg**：confirm→`chat->set_whisper_target(name)` 既有回调在 Q5 下自动升级为频道直发目标来源；候选名单（队友+在线好友）与原生截帧口径一致（whisper-dlg-prep.md §2.3 已注明数据源未取证，不编造"最近密语"）。结论：对齐，不动。
- **ChatBalloon（头顶气泡）**：工作树在途 hunk 全部落在 `LoginProcessNotice`（并发会话的登录公告九宫修复），本体气泡类未动、未受影响。结论：不归本任务，归属清单见下。

## 验证
- `tools/build_smoke.bat` 两轮 **EXITCODE=0**（协议门禁 GATE OK，opcode 矩阵对齐 s9 `44d0d5d9ca`），0 error；产物 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` 时间戳 11:09/11:14 刷新。
- 二进制核验新键齐全：`chat.target.megaphone`、`(To `、`[Party] `、`Unable to whisper` 各 1 处。
- 部署 `locale/*.ini` 双侧 `chat.target.*` 8 键齐全（xcopy /d 生效）。
- R5：本次未新增任何 img 节点（下拉=既有自绘 ColorBox+Text）；列表几何 8×15=120px 屏内不越界（推算已记）。
- 未跑项（如实披露）：真机进图逐频道手测（发/收/回显/密语成功失败路径）需要在线角色，留给用户冒烟——重点：①下拉 8 项选择+选中直发 ②组队发言自己历史里有没有自己那行 ③/w 不存在的人是否只出红字 ④喇叭项按下是否开道具窗留文本。

## 并发会话归属（不得混入本功能提交）
- `IO/UITypes/UIChatBar.h/.cpp`：GM 命令列表窗（`#include "GMCommandList.h"`、TAB 补全分支、`complete_gm_command`、`set_input`）——与聊天 hunk 同文件交织。
- 未跟踪：`IO/UITypes/GMCommandList.h`、`UIGMCommandList.{h,cpp}`；`MapleStory.vcxproj` 两个 ClInclude/ClCompile 条目。
- `IO/Components/ChatBalloon.{h,cpp}`：LoginProcessNotice 九宫修复（另一并发会话）。
- 提交拆分决策已单独请示用户（partial staging 会产生不可编译快照，见 git 操作说明）。
