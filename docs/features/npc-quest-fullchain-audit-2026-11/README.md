# NPC 对话与任务全链路盘查 + Angel / IMG / MinMS 三方对照

> 审计日期：2026-11-17　分支：`feature/dev_0.0.1`
> 用户请求（逐字）：「深入检查一下，npc对话所有逻辑，包含但不限于npc任务流程、npc脚本内容渲染、脚本功能、事件、任务指引等；全链路盘查一下npc、任务。」追加：「对照angel、img、minms」
> 性质：**只读源码/证据审计，零代码改动**。所有结论落到 `路径:行号` 或证据文件；文档旧基线一律经源码复核后采信。

---

## 0. 结论速览（TL;DR）

1. **本端 NPC/任务主链路是闭合的**：点击→发包→s2c 解析→14 类 msgtype 渲染→玩家回包→服务端脚本推进→回包落地任务状态，全链有源码证据，无断点。
2. **脚本内容渲染真身在 `UINpcTalk::format_text`**；`IO/Components/NpcText.cpp` 是**整文件注释死代码**（自述 "This will be moved into UINpcTalk"）——这是排查时最容易踩的认知坑。
3. **两类"任务指引"必须分开看**，这是三方报告的关键分歧裁决点：
   - 「对话内按键闪烁」(KEY_ANI / SetButtonBlink)：本端**已实装**（旧文档 r02 判"未复刻"是过时快照）。
   - 「NPC 头顶任务标记」(灯泡/开书/可交书)：本端**完全空壳**，是三者对照里最大的可见差距。
4. **协议按 s9 裁决，与 Angel 083 结构性分叉**（NPC 58 包体、Quest 107 的 x/y 坐标与 action0 buffer），属刻意决策点（D1/D2），不是 bug，保持 s9。
5. **MinMS 与本端"深在不同维度"**：线协议覆盖本端赢（14 类 msgtype vs MinMS 6 类）；本地任务剧本/头顶标记/勋章窗 MinMS 赢。差距排序（离 Angel 行为真源）：`MinMS ≈ Angel < 本端`。
6. **IMG/WZ 资源本轮未实读**：本会话无 `mcp__orange_wz__*` 工具；所有资源侧结论均为既有文档落盘证据的转述，且发现历史 `.img` 失败是 key 规格用错（65536B），按 AGENTS.md orange-wz 路径应改 128B——此为后续实读的前置修正点。

---

## 1. 链路地图（Narrative Map）

### 1.1 NPC 点击 → 对话开启

```
玩家点击 NPC
  → Stage::send_cursor (Stage.cpp:588-615)
      ├─ statusbar 菜单优先 (:590-599)
      ├─ try_click_guide → CLICK_GUIDE(162) (:601-605)
      └─ MapNpcs::send_cursor (MapNpcs.cpp:85-109)
           └─ is_active && inrange && pressed
                → TalkToNPCPacket(oid).dispatch() (:97)
                    ├─ NpcActionTalkPacket(oid,-1,0) 对话姿态 (Packets.h:68)
                    └─ write_int(oid) → TALK_TO_NPC(58)
```

- 命中检测 `Npc::inrange`：`Npc.cpp:363-381`，以当前 stance 动画 dimension 构造矩形。
- 同 oid 重下发：`MapNpcs::update` 先 `remove` 后 `add`（`MapNpcs.cpp:39-44`），修复脚本挪位后坐标丢失。
- **唯一点击触发点**在 `MapNpcs::send_cursor`；`MapNpcs.cpp:95` 留有 `TODO: Try finding dialog first`（可动 NPC 点击未优先找已有对话）。

### 1.2 s2c 对话解析 → 渲染

```
NPC_DIALOGUE(304) → NpcDialogueHandler::handle (NpcInteractionHandlers.cpp:41-409)
  公共头: skip(1) speakerType + read_int(npcid) + read_byte(msgtype) + read_byte(speaker) (:45-49)
  switch(msgtype):
    0 Say / 1 YesNo / 2 GetText / 3 GetNumber / 4 Simple
    5 Quiz / 6 SpeedQuiz / 7 AskAvatar / 8 MembershopAvatar
    9 AskPet / 10 AskPetAll / 12 AcceptDecline / 13 AskBoxText / 14 Mirror
    （case 11 服务端未使用，与 Angel 一致）
  └─ 尾部 (:381-409): 资源哨兵 UiAssetResolve::window 缺失即 end_talk 防幽灵窗
       → 复用 UINpcTalk 槽（get 优先，已 active 则 bring_to_front）
       → change_text(npcid,msgtype,style,speaker,text,input) (:408)
            → UINpcTalk::change_text (UINpcTalk.cpp:1278-1631)
                 → resolve_say_style → format_text (:1308)
                 → 剥 #E/#I/#S/#K (:1310-1322)
                 → layout_simple_choices (:1342)
                 → 打字机状态/立绘/高度/按钮激活
                 → apply_guide_blinks_from_text (:1597-1609)
```

### 1.3 玩家回应 → c2s → 服务端脚本

```
按钮/输入 → UINpcTalk::button_pressed (.cpp:389-591) / dispatch_* 系列
  → NpcTalkMorePacket(60) 各变体 (NpcInteractionPackets.h:83-170)
  → 服务端 NPCScriptManager.nextLevel / dispose
end_talk 契约 (NpcInteractionHandlers.cpp:63-89)：
  case 2/13    → (x,0) 短包（action!=0 触发服务端 readString）
  case 5/6     → 空串（5/6 无条件 readString，让脚本 getText 放弃）
  case 其它    → (x,-1) dispose
```

### 1.4 任务状态落地（s2c → UI）

```
主状态变更: SHOW_STATUS_INFO(39) mode=1 (MessagingHandlers.cpp:164-251)
  short qid + byte status + string progress + skip(5)
  status=0 → remove_started + forfeit tip
  status=2 → set_completed + complete tip
  status=其它 → set_started(qid,progress) + 进度 tip + blink_menu_quest
  → refresh UIQuestLog

UPDATE_QUEST_INFO(211) (QuestHandlers.cpp:60-143):
  mode 6/7 时限增减, 8 完成通知, 0x0A 错误, 0x0F 过期(remove_started)
QUEST_CLEAR(49)/SET_QUEST_CLEAR_FIELD(150) → set_completed
SET_QUEST_TIME(151) → UIClock；SCRIPT_PROGRESS_MESSAGE → QuestAlarm tip
```

客户端 NPC 对话层**不直发 Quest 107**（`UINpcTalk.cpp`/`NpcInteractionHandlers.cpp` 全文无 `QuestActionPacket`）；任务变更交由服务端脚本 force + 客户端收包更新。

---

## 2. 分模块审计结论

### 2.1 脚本内容渲染（用户关注：脚本内容渲染）

`UINpcTalk::format_text(const std::string& tx, const int32_t& npcid)` @ `IO/UITypes/UINpcTalk.cpp:865-1099`：

| token | 展开为 | 实现行 |
| --- | --- | --- |
| `#h` / `#h0#` | 角色名 | :867+ |
| `#p<id>#` | NPC 名（`String/Npc.img`），空 body 用当前 npcid | :867+ |
| `#t<id>#` / `#z<id>#` | 道具名 `ItemData::get(id).get_name()` 全品类 | :867+ |
| `#q<id>#` | 技能名 `SkillData` | :867+ |
| `#m<id>#` | 地图名 `NxHelper::Map::mapName` | :867+ |
| `#o<id>#` | 怪物名 `String/Mob.img/<id>/name` | :867+ |
| `#c<id>#` | `#c` + 持有数（保留色标，LayoutBuilder 橙色） | :1004-1014 |
| `#x<qid>#` | 进度串 `a/b/c`：`b≠0`→`+c`；否则 `c%`（未接=`0%`） | :1023-1095 |
| `#i/#v/#s/#f/#B` | 保留给 LayoutBuilder 行内图标/进度条 | :1097 |
| `#L<n>#..#l` | `strip_simple_choices` 剥除并转选项 | :1117+ |

- **逐处展开**：C6 收口，旧实现每 token 只替第一处，现已全量；lambda `replace_named` guard 上限 256。
- **解析失败整段令牌移除**，不留字面 `#` 残片。
- `#L` 无闭合（`#L0#选项\r\n#L1#选项`）是移植脚本的常见写法，解析器按"下一 #L / 断行 / 末尾"取右界（`:1161-1167`）。
- 打字机按 UTF-8 首字节整段喂入 `#i/#v/#s/#f/#B`（`:334-363`），色标/样式一次 2 字节。
- **死代码**：`IO/Components/NpcText.cpp` 全文 `/*...*/`（`:22-166`），旧 parseL/parsez/parseh/parsev/draw 已迁 UINpcTalk。

### 2.2 脚本功能矩阵（用户关注：脚本功能）

`TalkType` 枚举 @ `UINpcTalk.h:41-57`；`TalkInputParams` @ `:77-96`。

| msgtype | UI/壳 | 玩家操作 → c2s 包 |
| --- | --- | --- |
| 0 Say(OK/Next/Prev) | UINpcTalk（style 派生 type@:847-863） | NEXT/OK→`(lastmsg,1)`、PREV→`(lastmsg,0)`、CLOSE→`(lastmsg,-1)` (`.cpp:453-501`) |
| 1 YesNo / 12 AcceptDecline | UINpcTalk | NO→0、YES→1；AcceptDecline 多 QAFTER→1 (`:502-550`) |
| 2 GetText | UINpcTalk 输入框 | 长度校验→`(2,1,"txt")` (`:1841`)；CLOSE→`(2,0)` |
| 3 GetNumber | UINpcTalk 输入框 | 数字/边界校验→`(3,1,int)` (`:1896`) |
| 4 Simple | UINpcTalk `#L#l` 列表 | 点选→`(4,1,int)` (`:665` / `:1111`)；CLOSE→`(4,0)` |
| 5 Quiz | `UIInitialQuiz` (`UIMiscShells.cpp:1811`) | submit→`(5,"ans")` (`:1920`)；cancel/超时→空串 |
| 6 SpeedQuiz | `UISpeedQuiz` (`:1466`) | submit→`(6,..)` (`:1599`)；giveup→`"__GIVEUP__"` (`:1621`) |
| 7 AskAvatar / 8 MembershopAvatar | UINpcTalk avatar 预览 | OK→`(msgtype,sel)` (`:2019-2025`)；cancel→`(msgtype,0)` 防挂死 (`:2030`) |
| 9 AskPet / 10 AskPetAll | `UIAskPet` (`:2127`) | OK→`(lastmsg,1,uid)` (`:2259`)；cancel→`(lastmsg,0)` |
| 13 AskBoxText | UINpcTalk 输入框(cols×rows) | OK→`(13,1,string)` (`:1826`)；CLOSE→`(13,0)` 防脚本挂死 (`:556`) |
| 14 Mirror | `UIDimensionalMirror` (`:2538`) | OK→`(1,id)` (`:2623`)；cancel→`(0)` |

**资源缺失统一降级**：所有壳在 `UiAssetResolve::window` 缺失时 end_talk 或静默不开窗（`NpcInteractionHandlers.cpp:381-393`），不弹幽灵窗。

### 2.3 事件驱动（用户关注：事件）

客户端为**被动响应模型**，不解析脚本本身：

- `SET_FIELD=125` → 传送/换图；`SET_NPC_SCRIPTABLE=263` → `WaveAShellHandlers.cpp:3261-3274` 仅 `set_stance("alert")`（纯表现，不改 scripted 标志）。
- 对话与任务提示互斥：`WaveAShellHandlers.cpp:1528-1551` PlayerHint——UINpcTalk active 时不弹 QuestAlarm，避免对话中被打断。
- `MapNpcs::broadcast_tv_message`（`MapNpcs.cpp:74-83`）SEND_TV 文字层广播到当前图 MapleTV NPC。

### 2.4 任务指引（用户重点关注，含三方分歧裁决）

本端有 **5 条指引通道**，但形态与 083 原生不同：

1. **KEY_ANI 四钮闪烁**（✅ 已实装，旧文档过时）——`apply_guide_blinks_from_text` (`UINpcTalk.cpp:1243-1271`) 扫正文 `#E/#I/#S/#K`→ `UIStatusBar::start_guide_blink(idx)`；`UIStatusBar.cpp:1789-1817` + `blink_ani[4]` (`:2216-2219`) 两帧 1Hz + `BLINK_TIMEOUT_MS` 归零；`change_text` 换句先 stop 再重启 (`:1281-1283`)。对应 Angel `sub_9A2DE0` 分类 + `CUtilDlgEx::Draw @0x9a4256 SetButtonBlink`。
2. **菜单「任务」钮 blink**（T4 降级）——`UIStatusBar::blink_menu_quest()`，由 `MessagingHandlers.cpp:243-244` 触发；非原生 KEY_ANI 四主窗链。
3. **QuestAlarm 通知+进度浮层**（`CNoticeQuestProgress` 替代）——`UIMiscShells.cpp:3802-3886`，5 槽 FIFO / `SLOT_MS=3000`，在 tip 下方叠画进度槽；无独立 IMG 节点（orange-wz 确认 `UIWindow.img` 有 QuestAlarm、无 NoticeQuestProgress）。
4. **进度条/连锁页码条**（UIQuestLog 内）——`UIQuestLog.cpp:396-438 draw_gauge`、`342-392 draw_chain_page_gauge`。
5. **寻路指引（弱）**——`WaveAShellHandlers.cpp:1553-1575 SpawnGuideHandler` 用 `FieldMarker::Kind::DOOR` 生成临时场标；`FieldMarker.h:25-42` 的 Kind 枚举**无 QUEST 专属光柱**。

#### 🔴 关键分歧裁决：两类"指引"不是同一件事

| 指引类型 | 本端状态 | 证据 |
| --- | --- | --- |
| 对话内按键闪烁（KEY_ANI） | ✅ 已实装 | `UINpcTalk.cpp:1243` → `UIStatusBar.cpp:1789` |
| **NPC 头顶任务标记**（灯泡/开书/可交书） | ❌ **完全空壳** | `Gameplay/MapleMap` grep `quest_marker` 37 条全为 FieldMarker 技能/门族，无 NPC 标记；`UIMiscShells.h:1053` `UIQuestIcon` 仅 `has_assets/backgrnd/tip_label`，`:8016` 缺资源 LOG_WARN |

Angel 报告说"指引功能收口"指的是前者，MinMS 报告说"指引几乎空壳"指的是后者——**两份都对，对象不同**。本端真正的可见差距是"行走在地图上、NPC 头顶看得见的指引"完全缺失。

- `MARKNPC`「寻找 NPC 位置」在 `UIQuestLog.cpp:979-983` 明确保留 no-op：缺 npc→map 反查表 + UIWorldMap 入口 + 封包证据，属决策性缺口（不发明协议）。

---

## 3. 三方对照（Angel / IMG / MinMS）

### 3.1 Angel（IDB/二进制，行为真源）

- **NPC OnScriptMessage @0x74660A**（`evidence/verify-onScriptMessage.txt:2`）：公共头 `Decode1(speakerType)→Decode4(npc)→Decode1(msgtype)→Decode1(speaker)` + `switch(msgtype)`。case 表 0..10/12-14 与本端逐 case 对齐，case 11 双方均缺。case 1 vs 12 = 同函数不同 a7（AcceptDecline）。
- **CUtilDlgEx 族**（单体类，30+ 具名方法）vs 本端 `UINpcTalk`+Quiz/Pet/Avatar/Mirror 专用壳——**类非 1:1，功能齐**。排版常量：`MAX_HEIGHT=248`、`TEXT_X=166/WIDTH=310`、`BTN_DY=49`、`AVATAR 419×306`。
- **Quest c2s 107 六动作**（`CQuest::StartQuest @0x716fe1`）：action0=OnCompleteQuestFailed buffer、1=start、2=complete、3=forfeit(3字节最短包)、4/5=script-linked 带 x/y。**本端定长无 x/y、无 action0**——结构性分叉，按 s9 裁决保持（D1/D2）。
- **Quest 收包双通道**：`CQuestMan::OnQuestRecordMessage @0xa20f4c` / `OnQuestRecordExMessage @0xa2160b`；本端走 UPDATE_QUEST_INFO(211) mode 6/7/8/0x0A/0x0F，命名空间分列。
- **任务指引浮层**：`CNoticeQuestProgress` 独立类 5 方法 + `OnQuestProgressUpdated_{Item,Mob,Script}`（槽 5×3000ms）→ 本端 QuestAlarm 内叠槽降级替代，功能等价、像素/动画不 1:1。
- **TAG 分类** `sub_9A2DE0 @0x9a2de0`：`'E'/'I'/'K'/'S'` 等分类 → 本端 `#E/#I/#S/#K` blink 对齐。
- 具名符号：NPC 71 funcs / 15 classes `71/71 by bytes`；Quest 153 funcs / 34 classes `153/153 by bytes`（洗 1 EA 后）。

### 3.2 IMG/WZ 资源（⚠️ 本轮未实读）

- **本会话无 `mcp__orange_wz__*` 工具**（`ops_discover_tools` 返回 `No allowed, implemented capability matched`），未调用任何 WZ/IMG/XML 读取；以下为既有文档落盘证据转述。
- **关键前置修正**：历史 `.img` 内容层 12 次 `IMG 解析失败` 是**key 规格用错**——旧文档走 65536B key；按 AGENTS.md，orange-wz 路径须用 **128B WzAESConstant.DEFAULT_KEY**（`tools/_gms_key_128.b64`），`.nx` 回退才用 65536B。后续任何实读须先按此重测，不能沿用旧"未连接/失败"结论。
- 已落盘结构：`Data/Quest` 6 真 `.img` 子节点；`UtilDlgEx/{t,c,s,bar,BtOK/Yes/No/Prev/Next}` 齐全；`Npc.img` 由 `CNpcTemplate::GetNpcTemplate @0x6dcb16` + `SP_1666 STRING_Npc.img`/`SP_955 NPC/%07d.img` 加载，本端 `Npc.cpp:41-149` 同构。
- `UIWindow.img` 有 `QuestAlarm`、**无** `NoticeQuestProgress` 节点；`QuestAlarm`/`QuestIcon` 在 stripped 符号层 0 命中。
- 脚本动作资源锚点：`SP_3190 QUESTICON_D`/`SP_1665 DCMARK`/`SP_3191..3194 UtilDlgEx List0-3`/`SP_3232 QUEST_QUESTINFOIMG`（像素待 IMG 实读）。

### 3.3 MinMS（源码，可借鉴工程）

| 文件 | MinMS | 本端 | 深在哪 |
| --- | --- | --- | --- |
| `UINpcTalk.cpp` | 4442 | 2034 | MinMS 2.2×：Say.img 剧本/QuestMenu/Quiz/auto-advance |
| `NpcInteractionHandlers.cpp` | 469 | 496 | **本端略深**：本端 14 类 msgtype，MinMS 仅 6 类(0/1/2/3/7/14) |
| `UIQuestAlarm` | 632/126 | 壳 | MinMS 完整 QuestBlock/objective/auto |
| `UIMedalQuest.cpp/.h` | 1242/170 | 0 | MinMS 独有：5 tab 勋章目录+详情 |
| `QuestNpcMarkers.cpp/.h` | 1059/77 | 0 | MinMS 独有：NPC 头顶任务标记全套 |

- **MinMS 可借鉴算法**（抄算法在本端类重写，禁整仓粘贴）：
  - `QuestNpcMarkers::resolve` 4 态优先级 `COMPLETE>AVAILABLE>IN_PROGRESS`（`QuestNpcMarkers.cpp:631-709`）、`build_index` 扫 Check.img（`:533-589`）——对应本端最大差距，最高 ROI。
  - 脚本自动推进 `try_auto_advance_quest_script`（`UINpcTalk.cpp:4091-4142`）+ volatile scripted quest 状态机。
  - 本地任务剧本 `open_quest_say`/`apply_quest_say_choice`/Say.img stop 分支按未满足项选文本（`:3328-3352`）。
- **禁移植项**：GLFW/GL 图形层（本端 SDL3）、nlnx/.img 资源假设（本端 orange-wz + NX）、命名空间 `jrc`（本端 `ms`）、公会 NullHandler、s9 包体红线（保持 MoreTalk）。
- **坐标常量本端更精细**（`UINpcTalk.h:106-213` AskAvatar 419×306 等），不用从 MinMS 抄。

---

## 4. 差距 / 隐患清单

| # | 项 | 状态 | 判定 |
| --- | --- | --- | --- |
| D1 | NPC 58 / Quest 107 包体与 083 分叉（x/y 坐标、action0 buffer、npcid） | 结构性差异 | ⊘ 保持 s9，决策点 |
| D2 | NpcTalk 直发 107 | 本端不发 | ⊘ 红线，服务端 force 模型 |
| D3 | **NPC 头顶任务标记缺失**（UIQuestIcon 空壳） | ❌ 空壳 | 🔴 最大可见差距，抄 MinMS 算法（资源换 NX 路径） |
| D4 | 勋章任务窗 `CUIMedalQuestInfo` | 本端无 | ⏳ 数据/协议依赖，W-M6 |
| D5 | Quest 自动接/完成态机（`IsAutoStartQuest` 等） | 本端无 | ⏳ A14/D7 |
| D6 | 单体 CUtilDlgEx 形态 | 类异形 | 🟡 功能齐，非 1:1 |
| D7 | AskAvatar/BoxText/Quiz/Pet/Mirror 深分支真机 | 挂起 | 🟡 pending-live-tests #37–#40 |
| D8 | F 样式维（NPC 12 + Quest 12 项） | ⏳ 全待验 | IMG 内容层未实读，解阻靠 128B key orange-wz |
| D9 | KEY_ANI 四钮闪烁 | ✅ 已实装 | 旧文档 r02"未复刻"判定证伪，文档需更新 |
| D10 | NoticeQuestProgress 独立浮层 | 降级替代 | QuestAlarm 内叠槽，像素/动画不 1:1 |
| D11 | `MapNpcs.cpp:95` `TODO: Try finding dialog first` | 残留 TODO | 可动 NPC 点击未优先找已有对话 |
| D12 | `UIMiscShells.cpp:4100-4105` `[TEMP-DBG]` 探针 | 残留调试码 | 写 `ul_dbg.log`，在 BT_COMM 好友分支（与 NPC/任务无关），标注"验证后删除"——未擅自删，待授权 |
| D13 | IMG/WZ 实读 | 本轮未做 | 本会话无 orange-wz 工具；历史失败系 key 规格错配 |

---

## 5. 建议优先级（P8 Owner 视角）

1. **最高 ROI**：把 MinMS `QuestNpcMarkers` 的 resolve/索引算法抄进本端既有 QuestData/Npc 类，资源换本端 NX 路径 → 补上 D3（NPC 头顶任务标记），直接拉近与 Angel 的可见差距。本端 `QuestData::list_available()`（`QuestData.h:45`）已有算法底子，属"补层不造轮子"。
2. **次高**：勋章任务窗（D4）受数据/协议依赖，排 W-M6。
3. **解阻前置**：用 128B key 走 orange-wz 实读 `.img`，把 F 样式维（D8）从 ⏳ 变成可验。
4. **清理**：D12 残留探针（需授权）、D11 TODO 评估。
5. **文档同步**：更新 r02/README 中 SetButtonBlink"未复刻"的过时判定（D9）。

---

## 6. 证据链索引

- Angel：`docs/features/angel-flow-audit-2026-11/`（r02、P2/P3、evidence/p3-ida-tagmap.txt、verify-onScriptMessage.txt）、`docs/features/quest-angel-calibration-2026-11/README.md`、`docs/features/npc-angel-calibration-2026-11/README.md`、`docs/features/npc-talk-format-tokens/README.md`。
- MinMS：`docs/features/angel-minms-port-plan-2026-11/README.md`；源码 `E:\project\MinMS\`。
- 本端源码：`Gameplay/MapleMap/{Npc,MapNpcs,MapFieldMarkers,FieldMarker}.{cpp,h}`、`IO/UITypes/{UINpcTalk,UIQuestLog,UIMiscShells,UIShop}.{cpp,h}`、`Net/{Handlers/Packets}` 下 Npc/Quest 相关、`Character/QuestLog.{cpp,h}`、`Data/QuestData.{cpp,h}`。
- 三份子 agent 原始报告见本会话收口消息（npc-chain / quest-chain / minms-compare / angel-wz-compare）。
