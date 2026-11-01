# 闪退取证：两次客户端崩溃（2026-10-30 会话 #2 / #4）

本文记的是**同一晚两次不同的 AV**，都取到了源码行：
- §1-§5 = **会话 #2**，`ESC 关聊天 → classic HUD 聊天栏空按钮`（`Button.cpp:33`）；
- §8 = **会话 #4**，`!monitors 后一秒闪退 → Event List 窗口越界读空 vector`（`UIEvent.cpp:123`），根因在协议侧。
两条链各自独立，取证方法相同（WER → 同代 PDB → 源码行 → 与代码不变量对照）。

---

## 1. 取证链（每一步都可回溯）

会话 #2 用户现象原话：**"输入完 `!monitor` 和 `!showpackets` 后，关闭不了聊天，客户端就闪退了"**。
本轮把它拆成两个独立问题，都定位到了源码行，并修掉了会导致 AV 的那个。

| # | 事实 | 来源 |
| --- | --- | --- |
| 1 | 崩溃时间 `2026-10-30 20:59:27`，进程 `MapleStory.exe`，异常码 **`0xc0000005`**（写访问违例），故障偏移 **`0x000000000007fb70`** | Windows 应用程序日志 Event 1000；同事件 20:59:31 的 WER Event 1001 给出同一组签名 `P3/P6=6ae488a0 P7=c0000005 P8=000000000007fb70`，归档在 [`evidence/crash/wer-maplestory.txt`](../evidence/crash/wer-maplestory.txt) |
| 2 | WER 的模块时间戳 `0x6ae488a0` == 当时 exe 的 PE `TimeDateStamp` ⇒ **旁挂的 `MapleStory.pdb` 与崩溃构建严格同代**，符号解析有效 | `python tools/symbolize_offset.py`（打印 `exe preferred imagebase=0x140000000 TimeDateStamp=0x6ae488a0`） |
| 3 | `RVA 0x7fb70 → VA 0x14007fb70`，行号解析为 **`IO/Components/Button.cpp:33`**（`SymFromAddr` rc=87，函数名拿不到，行号可拿） | 同上 |
| 4 | 那行是可执行语句 `position = pos;` —— 函数体唯一一条写操作 | [IO/Components/Button.cpp:32-35](file:///E:/project/MapleStory-Client/IO/Components/Button.cpp#L32) |
| 5 | `buttons` 是 `std::map<uint16_t, std::unique_ptr<Button>>` ⇒ `buttons[id]` 对**从未插入过的 id 会默认构造一个空 unique_ptr 并插入**，随后 `->` 是 `this == nullptr` 的写访问 | [IO/UIElement.h:214](file:///E:/project/MapleStory-Client/IO/UIElement.h#L214) |

⇒ 异常码、符号、源码三者互相印证：崩溃点是**空 `this` 上调 `Button::set_position`**，不是内存踩踏。

> ⚠ 本轮修 bug 重新构建后，旁挂 PDB 已换代（`0x6ae488a0` → `0x6ae497c6`），上表第 2/3 步**不可复跑**。
> 结论以本文件 + 证据文件为准。构建指纹登记在 [`../builds/exe-stamps.md`](../builds/exe-stamps.md)——
> 以后要解析崩溃偏移，先照那张表确认 WER 签名与手边 PDB 同代，不同代就先快照再构建。

---

## 2. 触发路径

`classic_mode`（[UIChatBar.cpp:47](file:///E:/project/MapleStory-Client/IO/UITypes/UIChatBar.cpp#L47) 由 `UiAssetResolve::is_classic_hud()` 判定，本机为真）下，构造函数在 classic 分支里**只创建 `BtWhisper` 一个按钮就 `return`**，`BtMax / BtMin / BtChat / BtItemLink / BtChatEmoticon / BtHelp / BtOutChat` 从未插入 map。

于是两条既有代码路径必然踩空：

| 路径 | 链路 | 崩溃行 |
| --- | --- | --- |
| **P1（用户实际撞到的）** | `ESC` → `input_text_escape_callback()` → `toggle_input(false)` → `update_view(true)` → `if (view_max)` 分支 | [UIChatBar.cpp:1529](file:///E:/project/MapleStory-Client/IO/UITypes/UIChatBar.cpp#L1529) `buttons[Buttons::BtMin]->set_position(...)` |
| **P2（同族，靠代码审查发现，未等用户撞）** | `TOGGLECHAT` 键 → `UIStateGame.cpp` → `toggle_view()` → `toggle_view(!view_max, true)` → `update_view(pressed)` | 同上 + `buttons[BtMax]->set_active(...)` |

---

## 3. 为什么"关闭不了聊天"

这是**第二个、独立的问题**，与空指针无关，修法也不同：

- `input_text_enter_callback()` 末尾是 `GeneralChatPacket(message, true).dispatch(); input_text.change_text("");` —— **只发送和清空，不收输入框**；
- 发送键的 RETURN 分支在 `view_input` 已为真时只是重新聚焦；
- ⇒ 能关闭输入框的只有 `ESC`，而 `ESC` 恰好就是 P1 的崩溃路径。

所以用户看到的是"回车关不掉 → 只好按 ESC → 一按就闪退"。两条现象同源不同因。

---

## 4. 修复

改 [IO/UITypes/UIChatBar.cpp](file:///E:/project/MapleStory-Client/IO/UITypes/UIChatBar.cpp)，三处 `classic_mode` 守卫，让 classic HUD 不再碰这些不存在的按钮（classic 的输入行位置本就由构造函数固定，跳过这些布局代码不改变外观）：

| 位置 | 改法 |
| --- | --- |
| `update_view(bool)`，[UIChatBar.cpp:1491-1494](file:///E:/project/MapleStory-Client/IO/UITypes/UIChatBar.cpp#L1491) | 文本框状态设置之后 `if (classic_mode) return;`，挡掉全部 `set_position` |
| `toggle_input(bool)`，[UIChatBar.cpp:1445-1446](file:///E:/project/MapleStory-Client/IO/UITypes/UIChatBar.cpp#L1445) | 五个 `set_active` 之前 `if (classic_mode) return;`（该 return 即函数结尾，不丢逻辑） |
| `toggle_view(bool,bool)`，[UIChatBar.cpp:1464](file:///E:/project/MapleStory-Client/IO/UITypes/UIChatBar.cpp#L1464) 与 [:1476](file:///E:/project/MapleStory-Client/IO/UITypes/UIChatBar.cpp#L1476) | 两处分别包 `if (!classic_mode) { … }` |

选择"守卫"而不是"给 classic 也建全套按钮"：classic HUD 的按钮布局参考端只有输入行右端那一个蓝书箭头，凭空造 6 个不可见按钮会引入新的绘制/命中区域，属于超出本次修复范围的改动。

**构建状态**：`tools/build_smoke.bat` → `GATE OK`（opcode 矩阵对上 s9 `44d0d5d9ca`）+ `EXITCODE=0`，产物已直接写入运行目录 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（21:12，`0x6ae497c6`）。
**复测结果（会话 #3，22:30:51–22:32:49）**：`0xc0000005 @ 0x7fb70` 这条签名**没有再出现**；整场 834 行 dump 期间唯一的崩溃是退出时的 `0xc0000374`（§6）。
边界要说清：会话 #3 客户端确实发了两条 `GENERAL_CHAT`（`!monitor` / `!showpackets`，证明输入框被用过），
但日志**无法证明 ESC 关闭路径本身被走过** ⇒ "同代构建、同一签名不再复发"是正面证据，不是该代码行的直接执行证明。

---

## 5. 本轮顺手排除的一个错误归因

闪退看起来很像"GM 命令引起的"，但 dump 里 **`!monitor` 一行都不存在**：
会话 #2 的最后一行是 `31 00 0c 00 21 73 68 6f 77 70 61 63 6b 65 74 73 01` = `GENERAL_CHAT("!showpackets", true)`，
更早只有 `/showpackets`。⇒ 用户以为输了 `!monitor`，实际没有发出该包；崩溃与命令内容无关，只与"按 ESC 关聊天"有关。

同时，**档 A 依旧没有产出任何包日志**：服务端日志里 `玩家 Admin 执行了命令：ShowPacketsCommand`（20:59:16.382）有，
但 `收到封包` / `ClientSend` 计数仍为 0 ⇒ 档位开启与日志产出之间还有一段没验通的路。

> **会话 #3（22:30）追记，上面两段都被推翻了**：那次 `!monitor` 真的发出去了（dump L164），
> 但服务端回的是用法提示 —— 命令**要求带玩家名参数**（`MonitorCommand.java:43-45`）；
> 而档 A 的"零产出"也有了解释：20:59 那次 `!showpackets` 把内存开关翻成了 ON（会话 #2 之后客户端立刻崩了，没有后续包可打），
> 会话 #3 从登录起就在打 `收到封包`，直到 22:31:34 用户又输了一次 `!showpackets` ⇒ **当次把它翻回 OFF**，之后整场为零。
> 两档的真实语义、过滤集与首次成功配对记在 [`log-pairing.md`](log-pairing.md) §3.4/§8 与 PLAN §8，本文件只留崩溃部分。

---

## 6. 仍未查的两条崩溃签名

1. **`0xc0000374`（`STATUS_HEAP_CORRUPTION`）**：**退出时**触发，`20:29:40`（构建 `0x6ae488a0`）与 `22:32:49`（修复后的构建 `0x6ae497c6`）各一次，
   同一签名在 `0x6ae35f5b`、`0x6ae2fa88` 等旧构建上也反复出现 ⇒ 长期存在、与本次 classic HUD 改动无关，本轮**未调查**。
   ⚠ **一次自我纠错**：我一度把 `错误偏移 0x118095` 用本 exe 的 PDB 解成了 `IO/UITypes/UIStoreBank.cpp:56`——**这是假的**。
   原始事件里 **`出错模块名称： ntdll.dll`**（`tools/wer_event_raw.ps1` → [`wer-event-1000-raw.txt`](../evidence/crash/wer-event-1000-raw.txt)），
   偏移属于 ntdll 不属于我们的 exe；铁证是同一个 `0x118095` 出现在四个不同 exe 构建上（ntdll 没变，我们的代码变了）。
   ⇒ 规则：**符号化前必须先看 `出错模块名称`**，`错误偏移` 永远相对于"出错模块"而非主程序；该规则已写进 `tools/symbolize_offset.py` 文件头。
   对照：20:59 那次 `出错模块名称： MapleStory.exe` ⇒ `0x7fb70` → `Button.cpp:33` 的解析**依然有效**。

2. **14 天 75 条 MapleStory 崩溃记录**（见证据文件首行计数，2026-10-30 23:25 重跑后从 73 涨到 75）：只做了时间/签名分布，没做归因。
   这类"退出时堆损坏"通常要 Application Verifier / 页堆才能定位，成本高，先挂着。

---

## 8. 会话 #4：`!monitors` 之后一秒闪退（2026-10-30 22:52:42）

用户现象原话：**"输入 `!monitors` 一会闪退了"**。这次不是聊天栏，是 **Event List 窗口的越界读**，
而且**根因在协议侧**——s9 复用了 `SET_WEEK_EVENT_MESSAGE(0x004D)` 发 yellow message。

### 8.1 取证链

| # | 事实 | 来源 |
| --- | --- | --- |
| 1 | `22:52:42` WER Event 1000：`异常代码 0xc0000005`，`错误偏移 0x191880`，**`出错模块名称 = MapleStory.exe`**（不是 ntdll，故 §6 的教训不适用，符号化合法） | [`evidence/crash/wer-event-1000-raw.txt`](../evidence/crash/wer-event-1000-raw.txt) 第 1 条 |
| 2 | 崩溃构建签名 `0x6ae497c6` == 21:12 那次修复构建的 PE `TimeDateStamp` ⇒ 旁挂 PDB 同代 | [`../builds/exe-stamps.md`](../builds/exe-stamps.md) |
| 3 | `RVA 0x191880 → VA 0x140191880` 解析为 **`IO/UITypes/UIEvent.cpp:123` (+0xb)**，即 `BoolPair<bool> evnt = events[slot];` | 输出全文抄进 [`evidence/crash/session4-uievent-oob.txt`](../evidence/crash/session4-uievent-oob.txt) §1（⚠ 符号化必须在覆盖前做：那份 PDB 随后就被 23:23/23:34 两次修复构建覆盖，**现已不可复跑**） |
| 4 | 构造函数在 `EventList/main` 资源不可用时 `return`，而 `events` 的填充在它**之后** ⇒ 空 vector；`event_count` 却仍是开头设的 16 ⇒ `slot 0 >= 16` 为假，第 121 行的守卫放行 | [IO/UITypes/UIEvent.cpp:43](file:///E:/project/MapleStory-Client/IO/UITypes/UIEvent.cpp#L43) / [:48-53](file:///E:/project/MapleStory-Client/IO/UITypes/UIEvent.cpp#L48) / [:71-77](file:///E:/project/MapleStory-Client/IO/UITypes/UIEvent.cpp#L71) |
| 5 | 反过来说：**能读到越界，就证明构造函数确实早退了**（没早退的话 `events` 有 16 条，下标永远合法）。第 3 步的行号同时是第 4 步的证据 | — |
| 6 | 谁点亮了这个半成品元素：`WeekEventMessageHandler` 收到 `0x004D` 后 `get_element → emplace / makeactive`；而 `UIElement::makeactive()` 只做 `active = true`，不重建任何字段 | [IO/UIElement.cpp:69-72](file:///E:/project/MapleStory-Client/IO/UIElement.cpp#L69) |
| 7 | **"第一条不崩、第二条才崩"有机制解释，不是巧合**：游戏内 `UIStateGame` 没有覆盖 `post_add`，基类是空实现（`IO/UIState.h:56` `virtual void post_add(UIElement::Type, UIElement*) {}`）⇒ 首条包走 `emplace` 时元素**保持构造函数留下的 `active=false`**，`draw()` 根本不跑；元素已存在于 map，第二条包落到 `else if (!ev->is_active()) ev->makeactive()` ⇒ 下一帧 `draw()` 越界。会话 #3 全场只有 1 条 `0x004D`，故从未进入第二条 | [IO/UI.h:80-93](file:///E:/project/MapleStory-Client/IO/UI.h#L80) / [IO/UIState.h:56](file:///E:/project/MapleStory-Client/IO/UIState.h#L56) / [IO/UIStateGame.cpp:712-765](file:///E:/project/MapleStory-Client/IO/UIStateGame.cpp#L712)；dump 里 `0x004D` 共 3 条：会话 #3 L165，会话 #4 L1000/L1003，见 evidence §3 |
| 8 | 服务端时间线：`22:52:34.987 MonitorCommand`（`!monitor Admin` 成功，回 `1 已被监控`）→ `22:52:41.597 MonitorsCommand`（回 `Admin 已被监控`）→ **崩溃 22:52:42**，`22:52:46.690` 服务端察觉掉线并存档 | `E:\project\BeiDou-Server_s9\logs\out.log` L238-L242，抄在 evidence §4 |

⇒ **顺带的正面结果**：`!monitor Admin` 这次**真的生效了**（chrId=1 进监控集），档 F 首次挂上；
只是挂上后 7 秒内入站只有 `MOVE_PLAYER(41)` / `GENERAL_CHAT(49)`，都在屏蔽表里，所以档 F 首份真数据仍为零（evidence §5）。

### 8.2 语义错位：这个 opcode 根本不是周活动列表

s9 侧 `PacketCreator.java:5437-5443` 的 `sendYellowTip(String)` = `SET_WEEK_EVENT_MESSAGE` + `writeByte(0xFF)` + `writeString(tip)` + `writeShort(0)`；
`Character.java:6952-6954` 的 `yellowMessage(String)` 直接调它，另有 7 处 GM 广播/警告也走同一条（`AutobanFactory`、`ItemInformationProvider:1855`、`GmCommand:58` 等）。
我们把它当"周活动自显窗口"，窗口内容又全是构造函数里硬编码的 16 条占位 —— **包里的文字没有任何通路能显示出来**。
所以用户输完 GM 命令看不见反馈，只看见一个空白/异常窗口，然后闪退。

对照表门禁对此**完全无感**：`opcode-matrix.json` 里 `client_names=[WEEK_EVENT_MESSAGE]` / `s9_name=SET_WEEK_EVENT_MESSAGE` / `value=77` / `status=HANDLED`
—— 名字、值、有无 handler 三项全过，缺的是"body 语义"这一维。⇒ 归进任务 #24 的缺失系统清单，新增一类 **"opcode 名与值都对、但服务端把该包复用成另一种消息"**。

### 8.3 修复（两处，一崩一语义）

| 位置 | 改法 | 作用 |
| --- | --- | --- |
| [IO/UITypes/UIEvent.cpp:48-56](file:///E:/project/MapleStory-Client/IO/UITypes/UIEvent.cpp#L48) | 资源缺失早退时，除了 `active=false; dimension=(0,0)` 再把 `event_count = 0` | 越界的**充要**堵法：`slot 0 >= 0` 为真，`draw()` 第 121 行守卫直接 break，不再碰 `events[0]` |
| [Net/Handlers/MessagingHandlers.cpp:481-502](file:///E:/project/MapleStory-Client/Net/Handlers/MessagingHandlers.cpp#L481) | `WeekEventMessageHandler` 不再 `emplace/makeactive` UIEvent；跳过尾部 short 后把正文按 `MessageType::YELLOW` 送进 `UIChatBar`；顺手删掉已无用的 `#include "../../IO/UITypes/UIEvent.h"`（原 :28） | GM 反馈可见；不再制造那个没有数据源的窗口。UIEvent 本体保留，仍可由 `EVENT` 键打开 |

**没有**去做"给 UIEvent 灌 yellow 文本"：`UIEvent.cpp` 里 `event_title/event_date` 的数据源是硬编码占位，接上真实文本等于新写一个窗口，超出协议对账范围。

### 8.4 构建与待复测

`tools/build_smoke.bat` → `GATE OK`（opcode 矩阵对上 s9 `44d0d5d9ca`）+ `EXITCODE=0`。
两次构建：**23:23:01（`0x6ae4b655`）** = 修复本体；**23:34:09（`0x6ae4b8f0`，当前待复测）** = 仅追加注释订正，代码相同。
产物直接写入运行目录 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`，exe+pdb 已快照到 `E:\MXD\_build_snapshots\20261030-2334_uievent-oob-fix-final\`（详见 [`../builds/exe-stamps.md`](../builds/exe-stamps.md)）。

复测清单（尚未执行，别把"构建通过"当成"修好了"）：
1. 进游戏 → `!monitor Admin` 一次 → `!monitors` 一次 → **不应再闪退**，且两条反馈文字应出现在聊天栏；
2. 按 `EVENT` 键 → 应**什么都不开**：键路本身有同一道 `usable()` 门禁（[IO/UIStateGame.cpp:435-440](file:///E:/project/MapleStory-Client/IO/UIStateGame.cpp#L435)），
   这也侧面印证 §8.1 第 4 行的前提（本机 `EventList/main` 不可用）；能点亮 UIEvent 的只剩 s9 的 yellow tip，而它已被改路由到聊天栏；
3. 连发 3 条以上 GM 命令（含一条 `!monitors`）→ 观察是否仍能稳定活过 1 分钟，专门验"第二条 yellow tip 不再触发 makeactive"；
4. 之后按 [`log-pairing.md`](log-pairing.md) §7 走档 F 采集；**全程不要再输 `!showpackets`**（它是取反开关，多按一次就把档 A 关了）。

**会话 #5 实测（2026-10-30 23:38 起，构建 `0x6ae4b8f0`）**：到 00:00 已连续在线 22 分钟无崩溃，
服务端 `logs/out.log` 该会话只有 `207`（登录→频道）与周期性 `24`（ping），无异常。
⚠ 但用户这轮**没有再发 yellow tip**（`!monitor` / `!monitors` 都没输），所以 §8.3 的语义修复只算"没崩"，
清单第 1/3 条仍未执行；档 F 首份真数据也仍悬着（`grep -i monitor logs/out.log` 只有 22:31 / 22:52 三条，全属会话 #3/#4）。

---

## 9. 会话 #5 顺手做的两件聊天栏事（2026-10-31 00:05）

用户原话"其他问题解决没"→ 我列了两条未了项，用户回"顺手先做"：
① 回车关不掉聊天输入框；② "游戏内聊天 UI 有问题"（此前只修了崩溃，外观一直没动）。

### 9.1 取证方法：旁路抓帧，不打断真机复测

用户在游戏里，`tools/capture/cap_hud.py` 那套会 `taskkill` + 抢前台，**不能用**。
改用 `PrintWindow(hwnd, hdc, PW_RENDERFULLCONTENT=2)` + `CreateDIBSection`：不切前台、不发消息，
对 D3D9 窗口同样有效（实测 1280x720 全帧可读）。两个坑：

- `GetClientRect` 给的是客户区尺寸，但 PrintWindow 画的是**整个窗口**，所以帧左上角含标题栏与边框：
  本机换算 **client = image - (8, 30)**。用两个独立锚点校准过（chatTarget 贴图左上、状态面板上沿）。
- `ctypes.wintypes` 没有 `BITMAPINFO`/`LONG`，得自己拼 `BITMAPINFOHEADER`（`ctypes.c_long` / `c_ushort`）。

抓到的缺陷现场：[`../evidence/chat/classic-row-before.png`](../evidence/chat/classic-row-before.png)（含 2x 版）。

### 9.2 一条通则：`Text::draw(y)` 的字形顶部在 y 之下约 10px

A11M 经 FreeType 出图后，`GraphicsGL::drawtext` 走 `char_y = y + ay - ch.bt`，实测偏移 **+10**：
本行 `[Welcome]` 请求 `y=588`，字形起于 598（Latin 与 CJK 同偏移，"对所有人"实测起于请求 y+10）。
classic 聊天行整行只有 24px、下沿就是那条 5px 下划线，所以**所有 y 偏移都得先扣掉这 10px**——
§9.3 三处错位全是同一个原因。

### 9.3 修复

| 位置 | 改法 | 现象 |
| --- | --- | --- |
| [IO/UITypes/UIChatBar.cpp:1671-1676](file:///E:/project/MapleStory-Client/IO/UITypes/UIChatBar.cpp#L1671) + `input_text_enter_callback` 内 8 处发送分支（:1605–:1659） | 新增 `close_input()`（清空 + `input_text_escape_callback()` 收框），把原先只 `change_text("")` 的 8 处全换成它 | 回车只清文字不收框，输入行一直挂着，只能靠 ESC |
| [IO/UITypes/UIChatBar.h:72](file:///E:/project/MapleStory-Client/IO/UITypes/UIChatBar.h#L72) / [:74-76](file:///E:/project/MapleStory-Client/IO/UITypes/UIChatBar.h#L74) / [IO/UIStateGame.cpp:406-418](file:///E:/project/MapleStory-Client/IO/UIStateGame.cpp#L406) | 加 `is_classic()`；TOGGLECHAT 在 `has_input()` 为假且是 classic 时改调 `toggle_input(true)`（并把 `toggle_input` 提为 public） | classic 没有 BtMax/BtMin，`toggle_view()` 只翻历史框 ⇒ 一旦收框键盘再也开不回来 |
| [IO/UITypes/UIChatBar.cpp:96](file:///E:/project/MapleStory-Client/IO/UITypes/UIChatBar.cpp#L96) · [:304](file:///E:/project/MapleStory-Client/IO/UITypes/UIChatBar.cpp#L304) · [:333](file:///E:/project/MapleStory-Client/IO/UITypes/UIChatBar.cpp#L333) | 输入框 `set_text_shift(0,-9)`、光标 `marker_adjust` 配平到 +8；chatTarget 标签按蓝框（81x20，画在 `input_pos+(0,2)`）水平+垂直居中，y 由 `+7` 改 `-4` | "对所有人"整串字压在蓝框下沿以外、打在状态面板上；输入的文字同样压到面板上 |
| [IO/UITypes/UIChatBar.cpp:287](file:///E:/project/MapleStory-Client/IO/UITypes/UIChatBar.cpp#L287) · [:320-329](file:///E:/project/MapleStory-Client/IO/UITypes/UIChatBar.cpp#L320) · [:591-596](file:///E:/project/MapleStory-Client/IO/UITypes/UIChatBar.cpp#L591) | 历史框 `log_pos` 去掉多减的 `INPUT_ROW_H`（底边与输入行顶边齐平，即 545..625，正好等于 `min_view_y` 当初算的位置）；消息首行 `message_y` 由 `INPUT_ROW_H+13` 改 `23`、上限改 `user_view_y+10`；命中矩形跟着改成 `(0,user_view_y)..(0,0)` | 展开历史框时框底和输入行之间空 24px，而**最新那条**恰好画在框外 |

### 9.4 构建状态：**编译+链接通过，尚未部署**

`OutDir` 默认就是运行目录（`MapleStory.vcxproj:75`），客户端在跑 ⇒ 覆盖不了 exe，所以这轮**没有**跑
`tools/build_smoke.bat`。改为单独指定 `OutDir` 做完整验证：

- 门禁：`python tools/gen_opcode_matrix.py --check` → `GATE OK ... s9 44d0d5d9ca`；
- 链接：MSBuild Release|x64 → `EXITCODE=0`，只剩既有的 `LNK4098` / `LNK4099`；
- 产物 `E:\project\MapleStory-Client\build_validate\MapleStory.exe`，PE `TimeDateStamp = 0x6ae4c134`（00:09:25），
  CodeView `90c912ab-71a7-5e42-a248bbbd7b90fcc5` age=2。

⚠ 中途真崩过一次编译：`UIStateGame.cpp:415` 调 `toggle_input` 撞上 private（C2248），已把头文件的
`toggle_input` 移到 public 后重编通过——这类越权调用只有链接前那一步能抓到，旁路抓帧替代不了编译。

### 9.5 部署与真机验证（2026-10-31 00:12 起，构建 `0x6ae4c1fd`）

用户退出客户端后跑 `tools/build_smoke.bat`：`GATE OK` + `EXITCODE=0`，exe `8d3f221c83658b46` /
pdb `15c96673fb5b3cd7`，`TimeDateStamp = 0x6ae4c1fd`（00:12:45），已快照
`E:\MXD\_build_snapshots\20261031-0012_chatbar-classic-layout\`，登记在 [`../builds/exe-stamps.md`](../builds/exe-stamps.md)。

**布局**（PrintWindow 逐像素量，证据 [`../evidence/chat/classic-row-after.png`](../evidence/chat/classic-row-after.png)）：
chatTarget 蓝框占 image 行 658..675，"对所有人"字形带 661..669 **完整落在框内**且水平居中；
`[Welcome]` 由原来的框底以下移到输入行上方 2..12px 处。§9.3 四处改动全部按预期生效。

**行为**（SendInput 带 scancode 打键 + 光标闪烁判据，脚本 `E:\project\_w_drive.py`；
证据 [`../evidence/chat/caret-armed-2x.png`](../evidence/chat/caret-armed-2x.png)、
[`../evidence/chat/typed-hello-2x.png`](../evidence/chat/typed-hello-2x.png)、
[`../evidence/chat/sent-echo-2x.png`](../evidence/chat/sent-echo-2x.png)）：
输入光标是构造时给的 11px 竖杠，落在 image (106, 661..671)，每 256ms 翻一次
`showmarker`——所以"是否已聚焦"用**相邻两帧在该列上的差分**判定，比看文字可靠：

| 操作 | 观测 | 结论 |
| --- | --- | --- |
| 收起态 → Enter | 光标列差分 11/11/0（在闪） | Enter 能开输入行（§9.3 第 2 行） |
| 开 → 打 `hello` | 文字带差分 94px，bbox (106,660)-(134,667) | 打出的字正常渲染在行内 |
| 有文字 → Enter | 历史区新增 `Admin : hello`，光标列不再闪 | 发送成功**并**收框（用户报的"回车关不掉"）；`Admin : ` 前缀只可能来自服务端回播，即真走了一趟 出站 `GENERAL_CHAT(49)` → 广播 → 入站 `CHAT_RECEIVED(162) |
| 空文字 → Enter | 光标列停止翻转 | 空回车=收框，不再需要 ESC |
| 开 → ESC | 同上 | ESC 收框仍可用，且**没崩**（§1–§4 那条空指针路径） |
| 收 → Enter → 收 → Enter（连做 3 轮） | 每轮都能重新闪 | 收框后键盘能再开回来 |

⚠ 取证副作用两条，都要记账：
① 未聚焦时打的字母会漏成热键——`hi`/`bye` 里那个 `h` 打开了「角色能力」窗口（已 ESC 关闭）；
更早一轮 `test123` 漏成热键，**消耗了一次随机传送石**，角色从猎人平原被扔到 Orbis（不可逆，玩法自然写入，未改数据库）。
② 服务端日志里查不到这条聊天：`MonitoredChrLogger.java:84` 把 `GENERAL_CHAT` 列进了 recv 屏蔽名单，
所以"日志没有 49"**不能**当作"包没发出去"的证据，得靠客户端历史区的回播行。

---

## 7. 工具

| 工具 | 用途 |
| --- | --- |
| [`tools/wer_crash_query.ps1`](../../../../tools/wer_crash_query.ps1) | 从 `Get-WinEvent` 抓 14 天内 Id=1000/1001 的 MapleStory 崩溃，按 P1..P8 签名写成 UTF-8 证据文件（本仓库 shell 按 cp936 解 UTF-8，中文别走 stdout，一律落盘） |
| [`tools/symbolize_offset.py`](../../../../tools/symbolize_offset.py) | ctypes 调 dbghelp，把 Event 1000 的 `错误偏移`（RVA）解析成源码行；先校验 PE `TimeDateStamp` 与 WER 签名同代。**前提：该事件的 `出错模块名称` 必须是 MapleStory.exe**（见 §6⚠） |
| [`tools/wer_event_raw.ps1`](../../../../tools/wer_event_raw.ps1) | 导出近 3 小时 Id=1000 的**完整**事件正文（含 `出错模块名称` / `异常代码` / `错误偏移` 三件套），符号化前必读 |

已知缺口：`SymFromAddr` 恒返 rc=87 ⇒ 拿不到外层函数名，只有行号；够用，因为行号已能落到具体语句。
