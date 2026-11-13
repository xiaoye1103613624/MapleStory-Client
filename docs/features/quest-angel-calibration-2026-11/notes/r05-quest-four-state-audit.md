# 第十一面 · 既有任务(Quest)笔记四态审计 (r05)

> 审计对象：`docs/features/beidou-v17-parity/notes/r*-quest*.md`、`docs/features/beidou-v17-parity/notes/evidence/r17/rea_*Quest*.json`、`docs/features/change-quickslot-persist/notes/` 及本仓 `Data/QuestData.*`、`IO/UITypes/UIQuestLog.*`、`Net/Packets/QuestPackets.h`、`Net/Handlers/QuestHandlers.*` 注释内既有结论。
> 方法：以 Angel(13339) 083/v83 一手伪代码为权威源，逐条判「销案 / 推翻 / 新增 / 遗留」。既有 REA subject 与 095 线单列，不冒充一手。

## 1. 既有素材索引（ops_glob 45 项，scanComplete=false 因 scan-budget，信号已取）

- 核心笔记：`r189-questlog-start-softkey.md`(6行)、`r240-questalarm-softkey.md`(6行)、`r602-quest-error-expired-silence.md`(5行)、`r55-multiline-questicon.md`(4行)、`r809-j5-quest-cygnus4th.md`(65行)。
- 其余 `r*-quest*.md` 约 40 个（quest icon / quest alarm / quest log tab / chain gauge / quest area / cygnus / medal / party quest 等切片）。
- REA r17：`rea_decompile_Quest_83EA05.json/.txt`、`rea_search_Quest.json`、`rea_search_QuestAlarm.json`、`rea_search_QuestIcon.json`、`rea_search_CancelRequests.json`。
- 源码注释内既有结论：`Data/QuestData.h:16-45`（`IDA sub_83EA05/sub_83FEC6` 连锁 `"%s ( %d / %d )"`）、`UIQuestLog.h`（`draw_chain_page_gauge` 注释）、`QuestPackets.h:10-23`（action 语义表）、`QuestHandlers.cpp`（mode 分发注释）。

## 2. 销案（8 组，既有结论与本面一手一致，保留为证据链）

| 组 | 既有结论 | 本面一手佐证 | 处置 |
| --- | --- | --- | --- |
| S1 | `r189` SoftKey A→`QuestActionPacket(1)` start / F forfeit / G giveup | `CQuest::StartQuest` action1 `COutPacket(107)`+`Encode1(1)`；`ResignQuest` action3 | 销案，数值 107 一致 |
| S2 | `r240` SoftKey qid→`QuestActionPacket(1,qid)` + GameMenu F4→`UIQuestAlarm` | Angel `CUIQuestAlarm` 6 方法具名 + action1 包 | 销案 |
| S3 | `r602` mode `0x0A` quest error / `0x0F` quest expired 时 QuestLog 开→refresh 跳 tip | `QuestHandlers.cpp` mode 分发 + `refresh_questlog_ui()`；083 `OnQuestRecordMessage` 收包通道 | 销案 |
| S4 | `r55` ServerMessage type5 长文>48→`UIMultiLine` / Quest complete→`UIQuestIcon` | `CNoticeQuestProgress` 5 方法 + `CUIQuestInfo` 15 方法族 | 销案（样式像素仍 ⏳ F11） |
| S5 | `r809` J5-⑥ 骑士团四转是数据侧空壳非客户端 bug（Skill.wz 1112.img.xml 仅 278B） | 本面 `CQuestMan::CheckStartDemand/CheckCompleteDemand` 走数据驱动，非硬编码任务 | 销案，数据缺口非协议缺口 |
| S6 | `Data/QuestData.h` 注释「`QuestInfo.img/<id>/{0,1,2…}` 分状态摘要」 | `Data/QuestData.cpp:129-208` 实读 `{0..4}` 页 + `parent/order/area`；`.nx` 结构层 6 sections | 销案 |
| S7 | `QuestPackets.h` action 语义 `1=start,2=complete,4/5=scripted,3=forfeit` | 083 `StartQuest` 4 处 `Encode1(1/2/4/5)` + `ResignQuest Encode1(3)` + `OnCompleteQuestFailed Encode1(0)` | 销案（action 值一致，包体另见 O/N） |
| S8 | `QuestLog.h` `started/completed` 双 map + `set_started` 擦 `completed` | `.cpp:22-90` 实现；083 `OnQuestRecordExMessage` 的 `sub_4E71FA` 记录更新 | 销案 |

## 3. 推翻（1 组，依据来源被证伪）

| 组 | 既有/宽口径结论 | 推翻依据 | 处置 |
| --- | --- | --- | --- |
| O1 | `angel-quest-facts.md` §2「36 个 opcode 字面量」被直读为 Quest 面 36 个专属出站常量 | `kws=("Quest","QUEST","quest")` 命中 `Request` 子串：153 funcs 里 **57 个 `…Request…` label 是跨面业务**（`SendTradeRequest`/`SendTransferFieldRequest`/`HttpSendRequestA`/`CNMRequest*`…）。逐个核 `COutPacket(107)` 只有 6 处且全在 `CQuest::StartQuest` / `ResignQuest` / `OnCompleteQuestFailed`；其余 35 个常量（26/38/39/42/43/58/59/62/63/68/71/72/73/75/76/79/80/83/87/90/91/92/94/95/97/98/103/106/113/123/137/153/155/157/218）属全局 opcode 存在性旁证 | **推翻「36 专属」口径**：纯 Quest 专属出站只有 `107`。结论进 README §2 口径 + §4 硬发现；facts 文件保留原样作证据，不删、不改标尺 |

> 另：前九面 r03/README 的「orange-wz 未连接 / 阻塞 / 以 .nx dump 为准」口径已由 `AGENTS.md`（YG 2026-11-09）覆盖，本面 r03 按 ACTIVE 规则真跑门禁后回退，属跨面口径刷新，不计本面 O 组（第十面 r05 O1 已记）。

## 4. 新增（8 组，Angel 083 一手才挖出）

| 组 | 新增事实 | 一手来源 |
| --- | --- | --- |
| N1 | 083 `107` 包体**按 action 变长 + 带玩家坐标 x/y**：action 4/5 `Encode4(npcid)+Encode2(x)+Encode2(y)`；action 1/2 `if(sub_721008(questid)==0){Encode2(x);Encode2(y);}`；action 2 另有尾 `Encode4(v30)` | `CQuest::StartQuest @ 0x716fe1` 4 处逐字（r01 §3.1） |
| N2 | action 0 = `OnCompleteQuestFailed` 带 `Encode4(count)+EncodeBuffer(4*count)` 的 N×int buffer | `CQuest::OnCompleteQuestFailed @ 0x718b58` |
| N3 | action 3 = `ResignQuest` 最短包（仅 `Encode1(3)+Encode2(questid)`，无 npc / 无 x/y） | `CWvsContext::ResignQuest @ 0xa26ea7` |
| N4 | 083 收包双通道：`OnQuestRecordMessage`(`Decode2+Decode1`) 与 `OnQuestRecordExMessage`(`Decode2+DecodeStr+sub_4E71FA+ResetInfo`) | `0xa20f4c` / `0xa2160b` |
| N5 | `CQuestMan` 17 方法全具名：`IsStartScriptLinkedQuest`/`IsCompleteScriptLinkedQuest`/`IsAutoStartQuest`/`IsAutoAutoAcceptQuest`/`IsAutoCompleteQuest`/`CheckStartDemand(20890B)`/`CheckCompleteDemand(20060B)`/`LoadQuestInfo(22479B)`/`LoadPartyQuestInfo`/`LoadPartyQuestRank` | symbol-inventory + decompile |
| N6 | `CUIMedalQuestInfo` 10 方法 + `CUIMedalQuestInfoDetail` 2 方法 + `CUIQuestInfo` 15 方法 + `CUIQuestInfoDetail` 4 方法 = 083 有独立勋章/任务信息窗族，当前端 `UIQuestLog` 合并 | Angel 具名清单 |
| N7 | 十一面同二进制再 +1：`153/153 by bytes`（`0x6cec47` 双端 32B identical） | `angel-beidou-quest-ea-bytelevel-audit.json` `eleven_face_consistency` |
| N8 | orange-wz Quest 结构层 6 真 `.img` 子节点 + 内容层 12 次 `IMG 解析失败` + 合规 `.nx` 回退 | `face11_gate_and_quest_probe.py` + 2 evidence（r03） |

## 5. 遗留（5 组，诚实挂起，不伪造闭环）

| 组 | 遗留 | 恢复条件 |
| --- | --- | --- |
| L1 | 当前端无 c2s `107` 真机 hex 抓包；083 action 0/2/4/5 分支无服务端回包对照 | P2 真机双开 + 抓包 |
| L2 | 083 的 x/y 坐标语义（`Encode2(v50/v51)` 是玩家坐标还是 NPC 坐标）未实证 | P2 IDA 追 `v50/v51` 数据流 |
| L3 | F 样式维 12 项（r03 §5）全 ⏳ | orange-wz 内容层恢复或正确端截图逐像素 |
| L4 | `CUIMedalQuestInfo` / `CUIQuestInfo` 独立窗与当前端合并窗的版式差异 | P2 真机观感 |
| L5 | `PQuest.img` / `Exclusive.img` 数据族无当前端 UI 消费路径 | P3 任务族续作 |

## 6. REA subject 单列（不冒充 13339 一手）

`evidence/r17/rea_search_Quest.json` subject = `BeiDou.exe` @ `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe`，sha256 `1198fa57ca5a7c489bae43ec13c69681d9cabe0f96762f3dc0357facf2e7d4df`，base 0x400000。26 命中大半是 `CNMRequest*`/`HttpSendRequestA`/manifest 子串（与 O1 同源假阳性），真节点仅 `Etc/QuestCategory.img`、`UI/UIWindow.img/Quest/Gauge/{spot,gauge,frame}`、`Quest/Gauge2/{gauge,frame}`、`est/PQuest.img`。**stripped 端字符串锚点，非 Angel 反编译，单列作 F 维素材**。

## 7. 095 处置

- `docs/features/port-095/**`、`Maplestory95.i64`（`ExpeditionIntermediary` 等）全程禁引为 083 一手依据。
- `Data/QuestData.h` 注释里的 `IDA sub_83EA05 / sub_83FEC6` 是 BeiDou 端 RVA（同 v83 二进制），非 095，保留但不升格为 Angel 具名语义。

## 8. 性质标注

审计/对照/决策点文档，非实现提交。全程只读，未改游戏源码/协议/WZ/IMG/XML，未建 git commit。
