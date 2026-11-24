# 第十一面 · 任务(Quest) Angel 083/v83 ↔ 当前端 全维校准 (P0 纯文档)

> 目标：`goal-11388c42`「以 Angel(13339) 的 083/v83 带符号 IDB 为权威校准源，逐功能完整校验并校准当前 MapleStory-Client 的功能、业务、逻辑、UI、排版、样式、交互」。本面 = ⑪任务(Quest)，round33/90。
> 方法：REA → IDA(Angel 13339 / BeiDou 13337) → IMG(orange-wz MCP，ACTIVE) 只读取证 → 七维对照清单 → 决策点 + 一次性 Plan → 交用户裁决后再进实现。
> 性质：校准/对照/决策点/Plan 文档，**非实现提交**；全程只读，未改游戏源码/协议/WZ/IMG/XML，未建 git commit。
>
> **r08 增补（2026-11-23）**：用户确认「改全」后，本面首次进入实现——修复任务描述裸 ID（`#p/#t` 未展开）与底带按钮逐页显隐/右锚错位。见 [`notes/r08-bare-id-and-band-fix-2026-11.md`](notes/r08-bare-id-and-band-fix-2026-11.md)。Release 构建通过（EXITCODE=0），**真机未验收**。原 P0 校准文档的「全程只读」性质到 r07 为止，r08 起不再适用。

## 0. 五条结论

1. **十一面 EA 字节级同二进制闭环再 +1**：Quest 面 `153/153 by bytes`（crosscheck `152/153`，缺 `0x6cec47 = CNoticeQuestProgress::Update`，该 EA 双端 32 字节逐字相同 → 纯 IDA 分析覆盖差异，非布局差异）。Angel 153 funcs / 34 classes 全具名，BeiDou 侧 `beidou_real_named=0` 全退化 `sub_XXXXXX` → Angel 只补语义层。
2. **本面最硬的一条诊断：facts「36 opcode」是 `Request` 子串假阳性**。`kws=("Quest","QUEST","quest")` 命中 `Request`，153 funcs 里 57 个 `…Request…` label 是跨面业务。**纯 Quest 专属出站只有 `107` (0x6B QUEST_ACTION)**；其余 35 个常量是全局 opcode 存在性旁证，不得冒充一手。宽口径命中 ≠ 专属事实，颗粒度拉到子串这一层才叫权威校准源。
3. **083 的 `107` 包体与当前端是结构性差异**：083「按 action 变长 + 带玩家坐标 `Encode2(x)+Encode2(y)` + action0 带 `EncodeBuffer(4*count)`」；当前端 `QuestActionPacket`「按 action 定长 + 只 `write_int(npcid)`」。数值同为 107、语义分叉 → 数值重合 ≠ 语义重合；记 D 决策点，不盲改，s9 优先。
4. **orange-wz IMG 门禁按 `AGENTS.md` ACTIVE 真闭合**：结构层 `Data/Quest` 枚举 6 个真 `.img` 子节点 WORKS；内容层 `get_node_tree_json`/`search_node` 12 次全 `IMG 解析失败`；命中回退条件后合规走 `.nxcache/Quest.nx`（root_children=6）。原始报错文本 + 65536B key 规格逐条留证，`no_fake_green`（报错不计 OK）。F 样式维 12 项诚实 ⏳。
5. **判定分布（脚本逐条重数，非手写）**：`✅62 / ⚠️10 / ⏳26 = 98`；七维 A14/B14/C14/D14/E16/F12/G14。无 ❌ / ⊘。遗留集中在 F 样式维（IMG 内容层未恢复）与真机 hex 缺口，不伪造闭环。

## 1. 取证基线

### 1.1 IDA 端点表

| 端点 | pid:port | 状态 | 本面产出 |
| --- | --- | --- | --- |
| Angel (083/v83 带符号) | 29620:13339 | ✅ REACHABLE | 153 funcs / 34 classes，反编译 `153/153 ok (physical_keys=153)` |
| BeiDou (stripped) | 31332:13337 | ✅ REACHABLE | crosscheck `152/153`，byteaudit 洗 1 EA 后 `153/153 by bytes` |
| orange-wz MCP | 10012 | ✅ REACHABLE / `custom-orange_wz` 已连接 | Quest 结构层 6 节点 + 内容层 12 次 `IMG 解析失败` + 合规 `.nx` 回退 |

`imagebase 0x400000`；PROTO `2025-06-18`；SSE `data: ` 行解析；`MCP.call("decompile",{"addr":addr,"include_addresses":True})` on 13339 / `lookup_funcs` 60/chunk on 13337。

### 1.2 三联门禁（AGENTS.md ACTIVE 顺序）

```text
REA(既有 r17 字符串锚点, subject 单列) → IDA Angel13339 ✅ / BeiDou13337 ✅
→ orange-wz MCP(IMG) 结构层 WORKS / 内容层 FAILS
→ 命中「IMG 解析失败」回退条件 → .nx dump(tools/nx_inspect.py) + 运行时 probe
```

历史 D10/D13/D14「不接 / 阻塞 / 以 .nx 为准」为 2026-11-09 前取证留存，被 YG `AGENTS.md` 覆盖，不再约束当前执行顺序（详见 `notes/r03-img-gate-fallback.md`）。

### 1.3 采集脚本 + evidence + notes

脚本：`tools/ida_angel_quest_{ea,decompile,crosscheck,byteaudit,facts}.py`（第十面已验证 SOP 派生，保留 `normalize_code` 三形态归一 + `label@EA` 防重名覆盖）+ `tools/face11_gate_and_quest_probe.py` + `tools/face11_readme_audit.py`。

evidence 7 件：`angel-quest-symbol-inventory.json`、`angel-quest-decompile.json`、`angel-beidou-quest-ea-crosscheck.json`、`angel-beidou-quest-ea-bytelevel-audit.json`、`angel-quest-facts.md`(2367 行 / 106561 B)、`orange-wz-quest-structure.json`、`nx-quest-content-probe.json`。

notes 3 件：`r01-angel-native-quest-facts.md`、`r03-img-gate-fallback.md`、`r05-quest-four-state-audit.md`。

### 1.4 本面独有技术点

- **子串假阳性隔离**：`kws` 宽口径把 `Request` 吃进来 → 逐条核 `COutPacket(107)` 上下文才定「纯 Quest 专属 = 1 个 opcode」，重写 facts §2 口径而非删证据。
- **decompile 三形态归一 + `label@EA` 防覆盖**：继承第八面 `code_of` + 第十面 output-ref 溢出修复，`physical_keys=153`（无重名合并丢函数）。
- **byteaudit 累积一致性表升级 `eleven_face_consistency`**：补回第九面 npc `71/71`、第十面 map `133/133` 两行（派生脚本 stale 文案诊断已修）。
- **通用 byteaudit**：无硬编码 EA，读 crosscheck 的 `missing_eas` 列表逐个双端 32B 比对。

## 2. 口径

### 2.1 七维 × 五判定

| 维 | 含义 | 条数 |
| --- | --- | --- |
| A | 功能 | 14 |
| B | 业务 | 14 |
| C | 逻辑 | 14 |
| D | 交互 | 14 |
| E | 排版 | 16 |
| F | 样式 | 12 |
| G | 协议/纪律 | 14 |
| 合计 | | **98** |

判定：`✅` 一致/销案 · `⚠️` 结构性差异或分列 · `⏳` 遗留(IMG/真机/未实证) · `❌` 缺失 · `⊘` 不适用。本面无 ❌ / ⊘。

### 2.2 证据优先级

实时运行行为 > 抓包 > active served assets > 当前进程配置 > 持久挑战状态 > 生成 artifacts > 检入源码 > 注释/死代码。本面以 `angel-quest-decompile.json` 内层伪代码 > 当前端源码 > 既有笔记 > REA subject 字符串锚点。

### 2.3 083 vs s9 红线

- **083/v83 唯一基准**：`Angel.i64`；`Maplestory95.i64` / `docs/features/port-095/**` / `ExpeditionIntermediary` 全程禁引为 083 一手依据。
- **校准 ≠ 改回原生**：083 的 x/y 坐标、action0 buffer、独立勋章窗是「对照事实」，不是「当前端必须补齐」；补齐会断 s9 联机。
- **数值重合 ≠ 语义重合**：`107` 双端同号但包体分叉；s2c `211/49/150/151` 与 083 `OnQuestRecordMessage/Ex` 是两套命名空间。
- **子串假阳性禁令**：facts §2 的 36 opcode 常量中 35 个属跨面，不进本面专属结论。

### 2.4 冲突取值表

| 冲突 | 083 一手 | 当前端 | 取值 |
| --- | --- | --- | --- |
| c2s 107 包体 | action4/5/1/2 带 `Encode2(x)+Encode2(y)`；action0 带 buffer | action∈{1,2,4,5} `write_int(npcid)`；无 action0 | 保持 s9 当前端，D1 交用户裁决 |
| s2c 任务记录 | `OnQuestRecordMessage` `Decode2+Decode1` / `OnQuestRecordExMessage` `Decode2+DecodeStr` | `UPDATE_QUEST_INFO(211)` mode 6/7/8/0x0A/0x0F | 分列，不强行对齐 |
| Quest 专属 opcode 数 | 1 个 (`107`) | 1 个 (`QUEST_ACTION=107`) | 取 1，facts 36 判假阳性 |
| IMG 结构 vs 内容 | orange-wz 枚举 6 节点 | `.nx` PKG4 6 sections | 结构层取 orange-wz，内容层 `.nx` 合规回退 |

## 3. 七维 98 条逐条对照

> 每行 `| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |`；判定列由 `tools/face11_readme_audit.py` 逐条重数。

### 3.1 A 功能 14 条（✅6 / ⚠️6 / ⏳2）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| A1 | QUEST_ACTION opcode 数值 | `COutPacket(107)` @`0x717087` 等 6 处 | `Net/OutPacket.h:192` `QUEST_ACTION=107`；`QuestPackets.h:10` `OutPacket(QUEST_ACTION)` | ✅ | 数值一致，进正面清单 |
| A2 | action 0 (完成失败 buffer) | `CQuest::OnCompleteQuestFailed @ 0x718b58` `Encode1(0)+Encode2+Encode4(count)+EncodeBuffer(4*count)` | 当前端 `QuestActionPacket` 无 action0 分支 | ⚠️ | 083 专属动作，分列；D2 裁决是否补空监听 |
| A3 | action 1 (start) | `CQuest::StartQuest @ 0x716fe1` `Encode1(1)+Encode2(questid)+Encode4(npcid)` + 条件 x/y | `QuestPackets.h` action1 `write_int(npcid)` | ✅ | 语义对齐（包体差异见 A8） |
| A4 | action 2 (complete) | `Encode1(2)` + `Encode4(v30)` 尾 + 条件 x/y @`0x71745c` | `QuestPackets.h` action2 `write_int(npcid)` | ✅ | 语义对齐（尾 int 记 D3） |
| A5 | action 3 (forfeit) | `CWvsContext::ResignQuest @ 0xa26ea7` `Encode1(3)+Encode2(questid)` 最短包 | `QuestPackets.h` action3 无 npc；`UIQuestLog.cpp:926` GIVEUP→`QuestActionPacket(3, id)` | ✅ | 最短包两端一致 |
| A6 | action 4 (start script-linked) | `IsStartScriptLinkedQuest` 分支 `Encode1(4)+Encode2+Encode4+Encode2(x)+Encode2(y)` | `QuestPackets.h` action4 归入「带 npc」组，无 x/y | ⚠️ | 结构性差异，D1 裁决 |
| A7 | action 5 (complete script-linked) | `IsCompleteScriptLinkedQuest` 分支 `Encode1(5)` 同形带 x/y @`0x717104` | `QuestPackets.h` action5 归入「带 npc」组，无 x/y | ⚠️ | 结构性差异，D1 裁决 |
| A8 | c2s 107 包体宽度 | 按 action 变长 + `Encode2(x)+Encode2(y)` | 按 action 定长 + 只 `write_int(npcid)` | ⚠️ | 不盲改，s9 优先 |
| A9 | `sub_721008(questid)==0` 才追加 x/y | `StartQuest` action1/2 条件分支 @`0x71751c` | 当前端无条件分支 | ⏳ | 未具名函数语义待 IDA 补符号；L2 |
| A10 | 083 收包通道 1 | `OnQuestRecordMessage @ 0xa20f4c` `Decode2(questid)+Decode1(state)` + `if(state==0)` | `QuestHandlers.cpp` `UPDATE_QUEST_INFO(211)` mode 分发 | ⚠️ | 两套命名空间分列 |
| A11 | 083 收包通道 2 | `OnQuestRecordExMessage @ 0xa2160b` `Decode2+DecodeStr+sub_4E71FA+ResetInfo` | 当前端 `refresh_questlog_ui()` | ⚠️ | 分列，不强行对齐 |
| A12 | 任务需求数据驱动 | `CQuestMan::CheckStartDemand(20890B)` / `CheckCompleteDemand(20060B)` | `Data/QuestData.cpp:319` `meets_requirements` + `Check.img/0` | ✅ | 数据驱动同构 |
| A13 | script-linked 判定 | `IsStartScriptLinkedQuest` / `IsCompleteScriptLinkedQuest` 具名 | `QuestData.h` `is_chain_quest` / `parent` 连锁 | ✅ | 判定族具名可追 |
| A14 | 自动接/完成族 | `IsAutoStartQuest` / `IsAutoAutoAcceptQuest` / `IsAutoCompleteQuest` | 当前端无对应自动态机 | ⏳ | L4 任务族续作 |

### 3.2 B 业务 14 条（✅12 / ⚠️1 / ⏳1）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| B1 | 任务日志双表 | `OnQuestRecordMessage` 更新 started / completed 记录 | `Character/QuestLog.h:27-50` `std::map<int16_t,std::string> started; std::map<int16_t,int64_t> completed;` | ✅ | 数据结构对齐 |
| B2 | set_started 互斥 | `sub_4E71FA` 记录更新时擦对侧 | `QuestLog.cpp:22-90` `set_started` 擦 `completed` | ✅ | 一致 |
| B3 | 日志 CRUD | `OnQuestRecordExMessage` Insert/Remove | `QuestLog.h` add/set/remove/is_/get_last_started | ✅ | 一致 |
| B4 | 进度字符串 | `DecodeStr(&v9)` 进度文本 | `QuestLog.h` `get_progress(int16_t qid)` | ✅ | 一致 |
| B5 | 完成时间 | `OnQuestRecordMessage` state 记录 | `QuestLog.h` `get_completion_time(int16_t qid)` int64 | ✅ | 一致 |
| B6 | mode 6 时限下发 | 083 state==0 分支时限写记录 | `QuestHandlers.cpp:1-144` mode6 `short count×(short qid,int time)` add | ✅ | 业务对齐 |
| B7 | mode 7 移除 | `OnQuestRecordExMessage` Remove 语义 | `QuestHandlers.cpp` mode7 remove | ✅ | 一致 |
| B8 | mode 8 完成通知 | `CNoticeQuestProgress::NoticeProgressChange` | `QuestHandlers.cpp` mode8 `short qid` / `int npc` / `int 或 short 尾` | ✅ | 一致 |
| B9 | mode 0x0A quest error | 083 `OnCompleteQuestFailed` 失败提示 | `QuestHandlers.cpp` mode 0x0A + `r602` 静默 | ✅ | 销案(r05 S3) |
| B10 | mode 0x0F quest expired | 083 记录过期 | `QuestHandlers.cpp` mode 0x0F `remove_started` | ✅ | 销案(r05 S3) |
| B11 | s2c 附加下行号 | 083 无独立 `QUEST_CLEAR/SET_QUEST_TIME` 号（走 record message） | `PacketSwitch.cpp:265 QUEST_CLEAR=49`、`:347 SET_QUEST_CLEAR_FIELD=150`、`:348 SET_QUEST_TIME=151` | ⚠️ | s9 专属 s2c，分列 |
| B12 | 告警/刷新 helper | `CUIQuestAlarm` 6 方法 / `CUIQuestInfo::ResetInfo` | `QuestHandlers.cpp` `quest_alarm` / `refresh_questlog_ui` / `notify` | ✅ | 一致 |
| B13 | QuestLog 开→refresh 跳 tip | 083 `ResetInfo` 原地刷 | `QuestHandlers.cpp` helper + `r602-quest-error-expired-silence.md` | ✅ | 销案 |
| B14 | 083 state==0 分支细语义 | `OnQuestRecordMessage` `if(v5==0){…}` 内部动作 | 当前端 mode 6/7/8 平铺 | ⏳ | 未逐 case 对照；P2 |

### 3.3 C 逻辑 14 条（✅13 / ⏳1）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| C1 | 任务数据缓存 | `CQuestMan::LoadQuestInfo(22479B)` 装载 | `Data/QuestData.h:16` `class QuestData : public Cache<QuestData>` `static get(int32_t id)` | ✅ | 一致 |
| C2 | 分状态摘要页 | `QuestInfo.img/<id>/{0,1,2…}` | `QuestData.cpp:129-208` 读 `{0,1,2,3,4}` | ✅ | 销案(r05 S6) |
| C3 | tab0 摘要选取 | 083 `CUIQuestInfo` 首页 | `QuestData.cpp:235-259` tab0 pick`{0,1}` | ✅ | 一致 |
| C4 | tab1 摘要选取 | 083 进行中页 | tab1 pick`{2,1,0}` | ✅ | 一致 |
| C5 | tab2 摘要选取 | 083 完成页 | tab2 回退 back | ✅ | 一致 |
| C6 | 连锁 parent/order | IDA `sub_83EA05/sub_83FEC6` `"%s ( %d / %d )"` | `QuestData.cpp` `parent` / `order` / `get_chain_parent` | ✅ | 一致 |
| C7 | 连锁总数索引 | 083 `LoadQuestInfo` 扫 parent | `QuestData.cpp:32-68` `ensure_chain_index()` 建 `chain_totals` | ✅ | 一致 |
| C8 | 起始需求 | `CQuestMan::LoadDemand / CheckStartDemand` | `QuestData.cpp:129-208` `Check.img[sid]["0"]` lvmin/lvmax/npc/job/quest | ✅ | 一致 |
| C9 | 需求判定 | `CheckStartDemand(20890B)` | `QuestData.cpp:319-354` `meets_requirements(level,job,log)` | ✅ | 一致 |
| C10 | 可接任务列表 | `CQuestMan` 需求族 | `QuestData.cpp:356-388` `list_available(level,job,log)` | ✅ | 一致 |
| C11 | 区域/职业约束 | 083 job/preq 数据 | `QuestData.h` `get_jobs/get_prequests/get_area` + `UIQuestLog.cpp:468` `Locale::text("quest.area.20"/"30")` 不编地名 | ✅ | 一致 |
| C12 | 开始需求名具名 | `CQuestMan::CheckStartDemand @ 0x721163` | `meets_requirements` 实现 | ✅ | 数据驱动同构 |
| C13 | 完成需求名具名 | `CQuestMan::CheckCompleteDemand @ 0x721d2c` | `Check.img/0` 完成态 | ✅ | 数据驱动同构 |
| C14 | `sub_721008` 条件逻辑 | `StartQuest` action1/2 `if(sub_721008(questid)==0)` 才追加 x/y | 当前端无该条件 | ⏳ | 未具名，L2 待补符号 |

### 3.4 D 交互 14 条（✅10 / ⚠️2 / ⏳2）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| D1 | 任务日志窗型 | 083 `CUIQuestInfo` / `CUIMedalQuestInfo` 窗族 | `IO/UITypes/UIQuestLog.h:1-150` `UIQuestLog : UIDragElement<PosQUEST>` TYPE=QUESTLOG/FOCUSED=false/TOGGLED=true | ✅ | 独立真窗 |
| D2 | 按钮枚举 | 083 `CUISkillEx::OnButtonClicked` 式 id 分发(跨面范式) | `UIQuestLog.h` TAB0/1/2,CLOSE,COLLAPSE,MARKNPC,DETAIL,GIVEUP,OK,NO,ALERT | ✅ | 一致 |
| D3 | GIVEUP 发包 | `ResignQuest` `Encode1(3)+Encode2(questid)` | `UIQuestLog.cpp:926-964` GIVEUP→`QuestActionPacket(3, selected_id).dispatch()` | ✅ | 端到端闭环 |
| D4 | 预留按钮不发明协议 | 083 未见 MARKNPC 发包 | `UIQuestLog.cpp` MARKNPC/OK/NO/ALERT 显式预留(注释「需 npc→map 反查表+UIWorldMap 标记入口，本包两者都没有，不发明协议」) | ✅ | 克制正确 |
| D5 | send_key | 083 `CWndSkillGuide::OnKey` ESC 关窗(跨面) / 任务窗 OnKey 具名 | `UIQuestLog.cpp:443-445` `send_key` 空实现 | ⚠️ | 交互缺口，D5 裁决 |
| D6 | 连锁页码绘制 | IDA `sub_83EA05/4497` 连锁 gauge | `UIQuestLog.h` `draw_chain_page_gauge()` | ✅ | 一致 |
| D7 | 行/头高亮 | 083 `CUIQuestInfo` 列表绘制 | `UIQuestLog.h` `row_highlight` / `header_highlight` | ✅ | 一致 |
| D8 | 三页签激活态 | 083 分页 | `UIQuestLog.h` `tab_fill_active[3]` | ✅ | 一致 |
| D9 | gauge 三态 | 083 `Quest/Gauge` 节点族 | `UIQuestLog.h` gauge 三态 | ✅ | 一致 |
| D10 | SoftKey A/F/G | 083 action1/3 具名 | `r189-questlog-start-softkey.md` A→start / F→forfeit / G→giveup | ✅ | 销案(r05 S1) |
| D11 | SoftKey qid + F4 | 083 `CUIQuestAlarm` 具名 | `r240-questalarm-softkey.md` qid→`QuestActionPacket(1,qid)` + F4→`UIQuestAlarm` | ✅ | 销案(r05 S2) |
| D12 | 083 任务窗 ESC/快捷键 | 083 `CUIQuestInfo` 独立 OnKey 语义 | 当前端 `send_key` 空 + `MARKNPC` 预留 | ⏳ | L4 任务族续作 |
| D13 | 任务告警浮层 | `CUIQuestAlarm` 6 方法独立实现 | 当前端 F4→`UIQuestAlarm` 浮层(既有切片) | ⚠️ | 083 更细，分列 |
| D14 | 勋章任务窗 | `CUIMedalQuestInfo` 10 + `CUIMedalQuestInfoDetail` 2 方法 | 当前端无独立勋章窗 | ⏳ | L4 + F7 |

### 3.5 E 排版 16 条（✅9 / ⏳7）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| E1 | 可拖拽元件 | 083 `CUIWnd` 派生窗族 | `UIQuestLog.h` `UIDragElement<PosQUEST>` | ✅ | 一致 |
| E2 | 位置持久化 | 083 `Pos` 模板族(跨面范式) | `PosQUEST` 位置状态 | ✅ | 一致 |
| E3 | 三页签布局 | 083 分页控件 | `UIQuestLog.h` TAB0/1/2 | ✅ | 一致 |
| E4 | 页签状态分类 | 083 进行/完成/可接分类 | `QuestData.cpp:235-259` get_summary_for_tab | ✅ | 一致 |
| E5 | 折叠态 | 083 `CUIWnd` 折叠 | `UIQuestLog.h` COLLAPSE | ✅ | 一致 |
| E6 | 连锁 gauge 坐标 | IDA `sub_83EA05/4497` 页码绘制 | `draw_chain_page_gauge()` | ✅ | 一致 |
| E7 | 行高亮矩形 | 083 列表绘制 | `row_highlight` | ✅ | 一致 |
| E8 | 头高亮矩形 | 083 表头绘制 | `header_highlight` | ✅ | 一致 |
| E9 | 区域名不编造 | 083 area 数据 | `UIQuestLog.cpp:468-479` `Locale::text("quest.area.20"/"30")` | ✅ | 克制正确 |
| E10 | 083 `CUIQuestInfo` 独立窗尺寸 | `CUIQuestInfo::OnCreate` 类族 | 当前端合并 `UIQuestLog` | ⏳ | F/IMG 未恢复，L3 |
| E11 | 083 勋章窗版式 | `CUIMedalQuestInfo` 10 方法 | 无对应 | ⏳ | L4 + r03 F7 |
| E12 | 083 进度浮层位置 | `CNoticeQuestProgress::Update` | 无对应 | ⏳ | L3 + r03 F8 |
| E13 | `Quest/Gauge/{spot,gauge,frame}` | REA r17 节点锚点 | `tab_fill_active[3]` / gauge 三态 | ⏳ | r03 F3 |
| E14 | `Quest/Gauge2/{gauge,frame}` | REA r17 节点锚点 | gauge 三态 | ⏳ | r03 F4 |
| E15 | `Etc/QuestCategory.img` | REA r17 分类图标 | `area_name` Locale 文本 | ⏳ | r03 F5 |
| E16 | `UIWindow.img/Quest` 底图 | 083 具名窗族 | 当前端 `UiAssetResolve` 解析 | ⏳ | IMG 内容层阻塞 |

### 3.6 F 样式 12 条（⏳12，IMG 内容层阻塞，r03 §5 降级清单）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| F1 | `QuestInfo.img/<id>/{0..4}` 分状态摘要页 | `CQuestMan::LoadQuestInfo` | `Data/QuestData.cpp:129-208` | ⏳ | 内容层未恢复 |
| F2 | 连锁页码 `"%s ( %d / %d )"` | IDA `sub_83EA05/sub_83FEC6` | `draw_chain_page_gauge()` | ⏳ | P2 真机 |
| F3 | `UI/UIWindow.img/Quest/Gauge/{spot,gauge,frame}` | REA r17 字符串锚点 | `tab_fill_active[3]` / gauge 三态 | ⏳ | P2 |
| F4 | `Quest/Gauge2/{gauge,frame}` | REA r17 | gauge 三态 | ⏳ | P2 |
| F5 | `Etc/QuestCategory.img` 分类图标 | REA r17 | `Locale::text("quest.area.20"/"30")` | ⏳ | P2 |
| F6 | `CUIQuestAlarm` 版式 | Angel 6 方法具名 | `r240-questalarm-softkey.md` | ⏳ | P2 |
| F7 | `CUIMedalQuestInfo` 样式 | Angel 10 方法具名 | 无独立窗 | ⏳ | P2 |
| F8 | `CNoticeQuestProgress` 浮层 | `0x6cec47` 32B identical | 无对应 | ⏳ | P2 |
| F9 | `CUIQuestInfo`/`CUIQuestInfoDetail` 15+4 方法 | Angel 具名 | `UIQuestLog` 合并窗 | ⏳ | P2 |
| F10 | `PQuest.img`/`Exclusive.img` 数据族 | `LoadPartyQuestInfo`/`LoadPartyQuestRank` | 无 party quest UI | ⏳ | P3 任务族 |
| F11 | 多行任务图标换行 | `r55-multiline-questicon.md` | `UIMultiLine` | ⏳ | P2 截图 |
| F12 | 正确端截图逐像素 | `C:\Users\11036\Desktop\正确客户端截图\` | 待真机 | ⏳ | P2 |

### 3.7 G 协议/纪律 14 条（✅12 / ⚠️1 / ⏳1）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| G1 | c2s opcode 数值 | `COutPacket(107)` 6 处 | `OutPacket.h:192` `QUEST_ACTION=107` | ✅ | 正面清单 |
| G2 | c2s / s2c 命名空间分列 | 083 record message 双通道 | s9 `211/49/150/151` 独立 s2c | ✅ | 分列纪律 |
| G3 | 数值重合 ≠ 语义重合 | 083 x/y + buffer | 当前端只 npcid | ✅ | 写进 §2.3 |
| G4 | 校准 ≠ 改回原生 | 083 是权威对照源 | 当前端 s9 联机实现 | ✅ | 不盲改 |
| G5 | s9 协议优先 | 083 差异作决策点 | `gen_opcode_matrix.py --check` 门禁 | ✅ | 保持 |
| G6 | 083 action 全枚举 | 0/1/2/3/4/5 六动作逐字 | 当前端 {1,2,4,5} 带 npc / 3 无 npc | ✅ | 一手齐 |
| G7 | 当前端带 npc 组 | `Encode4(npcid)` | `if(action 属于 1/2/4/5) write_int(npcid)` | ✅ | 一致(组) |
| G8 | action3 无 npc | `ResignQuest` 最短包 | `QuestPackets.h` action3 不写 npc | ✅ | 一致 |
| G9 | action0 buffer 缺口 | `OnCompleteQuestFailed` `EncodeBuffer(4*count)` | 当前端无 action0 | ⚠️ | D2 裁决 |
| G10 | `QUEST_ACTION` 注释来源 | 083 具名函数 | `OutPacket.h:192` 注释 BeiDou `RecvOpcode.QUEST_ACTION` | ✅ | 一致 |
| G11 | 095 禁引 | `Angel.i64` 唯一基准 | `docs/features/port-095/**` 不作依据 | ✅ | 纪律 |
| G12 | REA subject 单列 | 13339 一手 | r17 subject=`BeiDou-ClientV17.7/BeiDou.exe` sha256 `1198fa57…` | ✅ | 不冒充 |
| G13 | 真机 hex 缺口 | 083 action0/2/4/5 无回包对照 | 当前端无 c2s 107 抓包 | ⏳ | L1 P2 |
| G14 | D1–D14 交用户裁决 | 本面全决策点不擅自实现 | P1 待拍板 | ✅ | 交付口径 |

### 3.8 判定汇总（脚本重数校对）

| 维 | ✅ | ⚠️ | ⏳ | 合计 |
| --- | --- | --- | --- | --- |
| A 功能 | 6 | 6 | 2 | 14 |
| B 业务 | 12 | 1 | 1 | 14 |
| C 逻辑 | 13 | 0 | 1 | 14 |
| D 交互 | 10 | 2 | 2 | 14 |
| E 排版 | 9 | 0 | 7 | 16 |
| F 样式 | 0 | 0 | 12 | 12 |
| G 协议/纪律 | 12 | 1 | 1 | 14 |
| **合计** | **62** | **10** | **26** | **98** |

> `62+10+26=98`；无 ❌ / ⊘。以 `python tools/face11_readme_audit.py` 输出的 `OVERALL` 为准。

## 4. 十条硬发现

1. `153/153 by bytes`（洗 `0x6cec47` 1 EA）→ 第十一面同二进制闭环，`eleven_face_consistency` 补回 npc/map 两行。
2. facts「36 opcode」= `Request` 子串假阳性（57 跨面 label），**纯 Quest 专属出站只有 107**。
3. `CQuest::StartQuest` 一处函数内 4 次 `COutPacket(107)`，action 4/5/1/2 各自包体，带 `Encode2(x)+Encode2(y)`。
4. `CWvsContext::ResignQuest` action3 是 3 字节最短包（无 npc/无坐标）。
5. `CQuest::OnCompleteQuestFailed` action0 带 `Encode4(count)+EncodeBuffer(4*count)` 的 N×int buffer，当前端无此动作。
6. 083 收包双通道 `OnQuestRecordMessage`(Decode2+Decode1) / `OnQuestRecordExMessage`(Decode2+DecodeStr+sub_4E71FA+ResetInfo)。
7. `CQuestMan` 17 方法全具名，含 `IsAutoAutoAcceptQuest` 等 5 个自动态判定 + 两个 2 万字节级需求检查函数。
8. `CUIMedalQuestInfo`(10) / `CUIQuestInfo`(15) / `CUIMedalQuestInfoDetail`(2) / `CUIQuestInfoDetail`(4) = 083 独立窗族，当前端 `UIQuestLog` 合并。
9. orange-wz Quest 结构层 6 真 `.img` + 内容层 12 次 `IMG 解析失败` + `.nx` 合规回退（`no_fake_green`）。
10. `CNoticeQuestProgress::Update @ 0x6cec47` 32 字节逐字 identical，与第九面 `_GCWorldMapDlg` 同源字节模式。

## 5. 四态（详见 `notes/r05-quest-four-state-audit.md`）

| 态 | 组数 | 摘要 |
| --- | --- | --- |
| 销案 | 8 | S1 SoftKey A/F/G、S2 qid+F4、S3 mode 0x0A/0x0F、S4 多行图标、S5 J5-⑥ 数据侧、S6 分状态摘要、S7 action 语义、S8 QuestLog 双 map |
| 推翻 | 1 | O1 facts「36 专属 opcode」口径（子串假阳性） |
| 新增 | 8 | N1 x/y 包体、N2 action0 buffer、N3 action3 最短包、N4 收包双通道、N5 CQuestMan 17 方法、N6 独立窗族、N7 153/153 by bytes、N8 orange-wz 门禁 |
| 遗留 | 5 | L1 真机 hex、L2 x/y 语义、L3 F 维 12 项、L4 独立窗版式、L5 party quest 数据族 |

## 6. 决策点 D1–D14（推荐默认：保持 s9 优先、不补 083 发包、不改协议、保留资源降级、不建提交）

| # | 决策点 | 推荐默认 | 依据 |
| --- | --- | --- | --- |
| D1 | 是否给 `QuestActionPacket` 补 `Encode2(x)+Encode2(y)` | 不补 | 改回原生会断 s9 联机；083 作对照 |
| D2 | 是否补 action0 `OnCompleteQuestFailed` buffer 监听 | 不补 | 当前端无该 UI 路径，s9 无对应下行 |
| D3 | 是否补 action2 尾 `Encode4(v30)` | 不补 | 083 专属尾字段，语义未实证(L2) |
| D4 | 是否把 083 双 record 通道并进 `UPDATE_QUEST_INFO(211)` | 不并 | 两套命名空间分列 |
| D5 | 是否实现 `UIQuestLog::send_key` | 待裁决 | 当前端空实现，083 具名 OnKey 更细 |
| D6 | 是否拆 `CUIQuestInfo`/`CUIMedalQuestInfo` 独立窗 | 不拆 | 当前端合并窗已能跑，拆窗需 F 维资源 |
| D7 | 是否补 `IsAutoStartQuest` 等自动态机 | 不补 | 数据驱动已覆盖手动路径 |
| D8 | 是否把 `sub_721008` 条件分支搬进当前端 | 不搬 | 函数名无符号，语义待补(L2) |
| D9 | 是否新增 c2s 107 真机抓包工具 | 挂 P2 | 需真机双开，非纯文档 |
| D10 | 是否修 orange-wz 内容层 `IMG 解析失败` | 挂 P2 | 服务端解析能力，非当前端任务 |
| D11 | 是否补 F 样式维 12 项 | 挂 P2 | 等 IMG 内容层恢复或正确端截图 |
| D12 | 是否补 `CUIMedalQuestInfo` 独立窗 | 挂 P3 | 任务族续作 |
| D13 | 是否把 36 opcode 常量写回 facts 作专属结论 | 否 | 已判跨面假阳性，保留原样作证据 |
| D14 | 是否建 git commit / 改源码 | 否 | 全程只读，原子提交须用户授权 |

## 7. 一次性 Plan

| 阶段 | 内容 | 状态 |
| --- | --- | --- |
| P0 | 纯文档闭环：本 README + r01/r03/r05 + 7 evidence + `GATE OK` + `AUDIT OK` + present + 答复 | ✅ 本轮完成 |
| P1 | D1–D14 裁决后才动源码（推荐默认全「保持 s9 优先」） | ⏸ 待用户拍板 |
| P2 | orange-wz 内容层恢复 + 真机 c2s 107 hex + 双开观感 + x/y 语义追 `v50/v51` + F 维 12 项 + `build_smoke.bat` | ⏸ 挂起 |
| P3 | ⑫掉落(Drop) 续作 + 任务族独立窗/party quest 数据族 | ⏸ 下一面 |

## 8. 限制与未知

- **子串假阳性**：facts §2 的 36 opcode 常量 35 个跨面，已隔离；宽口径 `kws` 后续面若沿用须加 `Request` 负向过滤。
- **IMG 内容层未恢复**：F 维 12 项诚实 ⏳，`no_fake_green`（`IMG 解析失败` 不计 OK）。
- **REA subject 单列**：r17 subject=`BeiDou-ClientV17.7/BeiDou.exe` sha256 `1198fa57…`，非 13339，不冒充一手。
- **095 禁引**：`Maplestory95.i64` / `port-095/**` 不作 083 依据。
- **真机缺口**：当前端无 c2s 107 完整 hex、083 action0/2/4/5 无服务端回包对照、x/y 语义未追数据流。
- **decompile 形态**：三形态归一 + `label@EA` 已防覆盖（`physical_keys=153`），无 output-ref 残留。

## 9. 证据索引与复现

### 9.1 文件清单

```
docs/features/quest-angel-calibration-2026-11/
├── README.md                                   (本文件, 98 条 + D1–D14 + P0–P3)
├── notes/
│   ├── r01-angel-native-quest-facts.md         (153/34 + 6 处 COutPacket(107) 逐字)
│   ├── r03-img-gate-fallback.md                (ACTIVE 门禁 + 12 次 IMG 解析失败 + .nx 回退)
│   ├── r05-quest-four-state-audit.md           (销案8/推翻1/新增8/遗留5)
│   ├── r07-field-level-2026-11.md              (字段级还原 + IMG origin 一手)
│   ├── r08-bare-id-and-band-fix-2026-11.md     (裸 ID 修复有效; 按钮结论已被 r09 纠正作废)
│   ├── r09-band-button-correction-2026-11.md   (底带按钮二维显隐纠正: 详细=详情开关, 寻找NPC仅详情态)
│   └── r10-find-npc-location-2026-11.md        (寻找NPC位置功能移植 MinMS: 本地查表+世界图定位, 我的文件零error/全量受并行WIP阻塞/真机未验)
└── evidence/
    ├── angel-quest-symbol-inventory.json       (153 funcs / 34 classes)
    ├── angel-quest-decompile.json              (DONE 153/153 ok, physical_keys=153)
    ├── angel-beidou-quest-ea-crosscheck.json   (152/153, missing 0x6cec47)
    ├── angel-beidou-quest-ea-bytelevel-audit.json (VERDICT 153/153 by bytes, eleven_face_consistency)
    ├── angel-quest-facts.md                    (2367 行 / 106561 B / 36 opcode 常量含假阳性)
    ├── orange-wz-quest-structure.json          (Data/Quest 6 真 .img 子节点)
    ├── nx-quest-content-probe.json             (Quest.nx root_children=6 / sections=6)
    └── orange-wz-quest-tree-r08.json           (r08: UIWindow.img/Quest 全子树 + 按钮精灵, 128B key/autoParse)
```

### 9.2 复现命令

```bash
python tools/ida_angel_quest_ea.py           # 153/34 -> symbol-inventory
python tools/ida_angel_quest_decompile.py    # DONE 153/153 ok (physical_keys=153, 三形态归一)
python tools/ida_angel_quest_crosscheck.py   # 152/153, missing 0x6cec47
python tools/ida_angel_quest_byteaudit.py    # VERDICT 153/153 by bytes
python tools/ida_angel_quest_facts.py        # WROTE 2367 lines / 106561 B
python tools/face11_gate_and_quest_probe.py  # orange-wz structure + 合规 .nx 回退
python tools/gen_opcode_matrix.py --check    # GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)
python tools/face11_readme_audit.py          # TOTAL_ROWS 98 / AUDIT OK
```

## 10. 性质标注

本文档是**校准 / 对照 / 决策点 / 一次性 Plan**，不是实现提交。全程只读：未改游戏源码 / 协议 / WZ / IMG / XML，未建 git commit，未触发 live 改写安全流。P1 实现须等用户对 D1–D14 拍板；P2 挂起等 orange-wz 内容层恢复与真机；P3 下一面 ⑫掉落(Drop)。`goal-11388c42` 保持 active/armed。
