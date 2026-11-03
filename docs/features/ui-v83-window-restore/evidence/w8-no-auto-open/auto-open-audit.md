# R28-B2 证据：学院 / 公会回包不再无端开窗

## 1. 症状与根因（用户第三轮验收原话）

> 「点击"更换频道"怎么打开学院ui了？」

链路取证（R27-B7-3 复核时确认）：

1. 点「目录 → 频道变更」→ `UIGameMenu::button_pressed(BT_CHANNEL)` → `emplace<UIChannel>()`，同时收起目录气球。
2. 进图前后服务端会**主动**发 opcode 95（`FAMILY_INFO_RESULT`，[Net/PacketSwitch.cpp:170](../../../../../Net/PacketSwitch.cpp#L170)），
   该封包**不是本仓任何出站请求的回包**——登录链每次都会收到。
3. 修复前 [WaveAShellHandlers.cpp `FamilyInfoHandler`](../../../../../Net/Handlers/WaveAShellHandlers.cpp#L285) 的写法是
   `get_element → 取不到就 emplace → 取到但没开就 makeactive()` ⇒ **收到即开窗**，学院窗压在频道窗上面。

> R27-B7-3 当时报的「PASS」是我的误判：`_r27_b73c_06_after_change.png` 里学院窗就盖在频道窗上，
> 而驱动脚本随后按了 ESC 把它关掉，症状被我自己隐藏了。教训同「元素在位 ≠ 版式修好」。

同一条 `ensure-or-open` 惯用法在这两个文件里共有 **57** 处命中，其中 **家族/公会/联盟/结婚 4 类壳窗 24 处**是本轮范围。

## 2. 改法（决策 D3：用户选「全部改成『窗已开才更新』」）

惯用法统一替换，**纯删 2 行**，开窗动作全部交回给出「用户主动开」的出站请求方：

```cpp
-		auto ui = UI::get().get_element<UIFamily>();
-		if (!ui) ui = UI::get().emplace<UIFamily>();      // 无端造窗
-		else if (!ui->is_active()) ui->makeactive();      // 无端抬窗
-		if (ui)
+		auto ui = UI::get().get_element<UIFamily>();
+		if (ui && ui->is_active())
```

`get_element<T>()` 在窗被关闭后仍返回对象（框架只 deactivate 不 remove），所以条件必须是 `is_active()`，
只判 `if (ui)` 不够。**没落地的分支不是丢数据**：这 4 类窗的数据来源都是「开窗方先 dispatch 请求，回包再填窗」——
[UICharInfo.cpp:338-339](../../../../../IO/UITypes/UICharInfo.cpp#L338)（学院）、[UIFamily.cpp:158-159](../../../../../IO/UITypes/UIFamily.cpp#L158)（族谱）
都是 `dispatch` 后紧接 `emplace`，回包到达时窗已在栈且 active。

## 3. 24 处清单（改后行号）

| 文件 | 行 | 处理器 | 目标窗 | 窗没开时的去向 |
| --- | ---: | --- | --- | --- |
| WaveAShellHandlers.cpp | 285 | `FamilyInfoHandler` | UIFamily | 丢弃（本轮症状源） |
| | 361 | `FamilyChartHandler` | UIFamilyTree | 丢弃（族谱由 UIFamily 'T' 自己开） |
| | 366 | `FamilyChartHandler` | UIFamily | 丢弃 |
| | 409 | `FamilyJoinResultHandler` | UIFamily | 场地标记 `0x33F00001` |
| | 442 | `FamilyResultHandler` | UIFamily | 场地标记 `0x33F00000^type` |
| | 1932 | `FamilyNotifyLoginHandler` | UIFamily | 场地标记 `0x33F00003` |
| | 1978 | `FamilyJoinAcceptedHandler` | UIFamily | 场地标记 `0x33F00002` |
| | 2736 | `FamilyPrivilegeListHandler` | UIFamily | 丢弃（特权列表是窗内请求的回包） |
| | 2750 | `FamilyRepGainHandler` | UIFamily | 场地标记 `0x33F00004` |
| | 2790 | `FamilySetPrivilegeHandler` | UIFamily | 场地标记 `0x33F00005` |
| | 3873 | `GuildBbsHandler` | UIGuild | 丢弃（公会公告板是窗内请求的回包） |
| | 4164 | `GuildNameChangedHandler` | UIGuild | 角色 `show_iron_body()` |
| | 4196 | `GuildMarkChangedHandler` | UIGuild | 角色 `show_iron_body()` |
| MessagingHandlers.cpp | 359 | `ShowStatusInfoHandler` mode==6 (GP) | UIGuild | 聊天栏（负值 RED / 正值 YELLOW） |
| | 686 | `MultiChatHandler` mode==2 | UIGuild | 聊天栏 BLUE |
| | 695 | `MultiChatHandler` mode==3 | UIAlliance | 聊天栏 BLUE |
| | 788 | `SpouseChatHandler` | UIWedding | 聊天栏 YELLOW |
| | 884 / 893 | `NotifyLevelUpHandler` | UIGuild / UIFamily | 聊天栏 YELLOW |
| | 913 / 921 / 930 | `NotifyMarriageHandler` | UIWedding / UIGuild / UIFamily | 聊天栏 YELLOW |
| | 954 / 963 | `NotifyJobChangeHandler` | UIGuild / UIFamily | 聊天栏 YELLOW |

合计 24 处，diff 规模 **24 insertions / 72 deletions**（每处 3 行 → 1 行），可用
`git show <本批 commit> --stat -- Net/Handlers/WaveAShellHandlers.cpp Net/Handlers/MessagingHandlers.cpp` 复核。

## 4. 明确没改的（避免「顺手改」）

- **`OpenUIHandler`（WaveAShellHandlers.cpp:1116）**：opcode 本身就是「服务端命令客户端开窗」，
  `case 8 → UIFamily`（[:1153](../../../../../Net/Handlers/WaveAShellHandlers.cpp#L1153)）、
  `case 0x1B → UIFamilyTree`（[:1245](../../../../../Net/Handlers/WaveAShellHandlers.cpp#L1245)）等分支**必须**保留 `emplace`，改了这条协议就失效。
- 其余 **33 处同惯用法的非家族/公会窗**（扭蛋、怪物嘉年华、RPS、Ariant、MapleTV、Admin、Memo、Clock、
  QuestAlarm、StoreBank、MTS、Trade、EnergyBar、VegaSpell、ViciousHammer…）：本轮未动。
  它们里面确有同类「服务端推一条消息就弹整窗」的嫌疑，但**没有对应的真机症状**，按最小改动原则留待下轮，
  需要时同样一条命令可批处理（脚本见 §6）。
- `WeddingProgressHandler` / 婚礼礼物结果：婚礼是服务端主导的仪式流程，开窗属于正确行为，未纳入。

## 5. 验收状态

| 判据 | 状态 |
| --- | --- |
| 编译 0 error | ✅（仅仓内存量 C4804 警告） |
| 链接 + 装到 OutDir | ⛔ 仍被并发会话的运行中客户端占用（`LNK1104 … MapleStory.exe`），按其选择等它自己关后重试 |
| 冷启动登录链全程抓帧，`UIFamily`/`UIGuild` 不出现在未主动打开的帧 | ⏳ 待链接后跑 `_drive.py` 链 |
| 手动开学院窗后信息仍完整 | ⏳ 同上 |

## 6. 复跑

```bash
# 审计当前仍有多少 ensure-or-open 惯用法（57 = 本轮前的基线）
python /e/project/_r28_b2_rewrite.py            # 只读 dry-run，报告写 E:/project/_r28_b2_sites.txt
python /e/project/_r28_b2_rewrite.py apply      # 按 WHITELIST 行号批处理
```

脚本按 4 行字面匹配（`auto X = UI::get().get_element<T>();` / `if (!X) X = …emplace<T>();` /
`else if (!X->is_active()) X->makeactive();` / `if (X)`），变量名与类型三处必须一致才动，行号白名单之外零改动。

## 7. 遗留（本轮显式预留）

1. 这 24 处里 `tip` / `line` 仍是**硬编码英文**（`"X joined your family."`、`"<Guild> Y reached Lv.N"` 等）。
   按用户「非位图文字一律走 i18n」的定调应进 `locale/*.ini`，但 `locale/zh-CN.ini`、`locale/en-US.ini`
   正被并发会话改动（未提交），本批**不碰**，记为欠账，待其落地后单独一批接入。
2. `MultiChatHandler` mode 2/3 在「公会窗恰好开着」时把**聊天文字写进公会窗 notice**，窗没开才落聊天栏——
   这是本轮之前就存在的语义（原生应恒走聊天栏）。本轮只收紧开窗条件，未改这条语义。
