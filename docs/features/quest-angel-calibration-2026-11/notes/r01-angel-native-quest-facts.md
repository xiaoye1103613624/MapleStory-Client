# 第十一面 · Angel 083/v83 原生任务(Quest)一手事实基线 (r01)

> 取证源：`Angel.exe (pid=29620, port=13339)` 带符号 IDB（`Angel.i64`, v83 GMS, imagebase 0x400000）。
> 对照源：`BeiDou.exe (port=13337)` stripped IDB。采集时间 2026-11-09。
> 性质：纯只读取证 + 伪代码逐字摘录，未改游戏源码/协议/WZ/IMG/XML，未建提交。

## 0. 七条硬结论

1. **十一面 EA 字节级同二进制闭环**：Quest 面 `153/153 by bytes`（crosscheck `152/153`，缺 `0x6cec47 = CNoticeQuestProgress::Update`，该 EA 双端 32 字节逐字相同 → 纯 IDA 分析覆盖差异，见 `angel-beidou-quest-ea-bytelevel-audit.json` 的 `eleven_face_consistency`）。
2. **Angel 只补语义层**：BeiDou 侧 153 个 EA 全部退化为 `sub_XXXXXX`（`beidou_real_named=0`），Angel 侧 153 funcs / 34 classes 全具名。
3. **纯 Quest 专属出站只有 1 个 opcode = `107` (0x6B, QUEST_ACTION)**：`angel-quest-facts.md` §2 汇总的 36 个 `COutPacket` 常量里，其余 35 个来自 `kws=("Quest","QUEST","quest")` 宽口径命中 `Request` 子串（57 个函数 label 含 `Request` 却是跨面业务：`SendTradeRequest`/`SendTransferFieldRequest`/`HttpSendRequestA`…）。**这些是跨面事实，不是 Quest 一手语义，须单列，不得冒充**。
4. **083 的 `107` 包体是「按 action 变长 + 带玩家坐标」**，当前端 `QuestActionPacket` 是「按 action 定长 + 只带 npcid」：`CQuest::StartQuest` 在 action 4/5 与 action 1/2 条件分支里追加 `Encode2(x)+Encode2(y)`；当前端统一 `write_int(npcid)`、无 x/y。这是**结构性差异**，记 D 决策点，不盲改。
5. **083 收包双通道**：`CWvsContext::OnQuestRecordMessage`（`Decode2(questid)+Decode1(state)`）与 `OnQuestRecordExMessage`（`Decode2+DecodeStr`）；当前端 `UPDATE_QUEST_INFO(211)` 是 mode 6/7/8/0x0A/0x0F 分发。两套命名空间，数值≠语义。
6. **orange-wz IMG 门禁本面按 AGENTS.md ACTIVE 真跑**：结构层 `Data/Quest` 枚举 6 个真 `.img` 子节点 WORKS；内容层 `get_node_tree_json`/`search_node` 12 次全 `IMG 解析失败`；命中回退条件后合规走 `.nxcache/Quest.nx`，见 `r03`。
7. **校准 ≠ 改回原生**：s9 协议优先；095（`Maplestory95.i64` / `port-095/**`）全程禁引；D1–D14 交用户裁决后才动源码。

## 1. 三联门禁

| 环 | 端点 | 结果 |
| --- | --- | --- |
| REA | 既有 `evidence/r17/rea_search_Quest.json` | ⚠️ subject=`BeiDou-ClientV17.7/BeiDou.exe` (sha256 `1198fa57…`)，**非 13339**，仅作 stripped 端字符串锚点，单列不冒充一手 |
| IDA Angel | 13339 | ✅ 153 funcs / 34 classes，反编译 `153/153 ok (physical_keys=153)` |
| IDA BeiDou | 13337 | ✅ crosscheck `152/153`，byteaudit 洗 `0x6cec47` 后 `153/153 by bytes` |
| IMG (orange-wz MCP) | 10012 | ✅ 结构层 WORKS / ⚠️ 内容层 `IMG 解析失败` → 合规 `.nx` 回退（见 r03） |

采集脚本（均由第十面已验证 SOP 派生 + 三形态归一 `normalize_code` + `label@EA` 防重名覆盖）：
`tools/ida_angel_quest_{ea,decompile,crosscheck,byteaudit,facts}.py` + `tools/face11_gate_and_quest_probe.py` + `tools/face11_readme_audit.py`。
产物：`evidence/angel-quest-symbol-inventory.json`、`angel-quest-decompile.json`、`angel-beidou-quest-ea-crosscheck.json`、`angel-beidou-quest-ea-bytelevel-audit.json`、`angel-quest-facts.md`(2367 行 / 106561 B)、`orange-wz-quest-structure.json`、`nx-quest-content-probe.json`。

## 2. 153 funcs / 34 classes 清单（top 分布）

`CWvsContext 45 / CQuestMan 17 / CUIQuestInfo 15 / CUIMedalQuestInfo 10 / <free> 6 / CUIQuestAlarm 6 / CUserLocal 6 / CNoticeQuestProgress 5 / CTrunkDlg 5 / CNpc 4 / CUIQuestInfoDetail 4 / CField 3 / CQuest 3 / CItemInfo 2 / CUIGuildBBS 2 / CUIMedalQuestInfoDetail 2 / …(1 各: CAdminShopDlg, CConfig, CDragon, CField_MonsterCarnival, CItemSpeakerDlg, CLogin, CSecurityClient, CUIAvatarVAC, CUICharSelect, UMedalQuestInfo…)`。

Quest 核心语义族（`CQuestMan` 17 方法逐字）：`GetCompleteDemand@0x6cdd06` / `LoadDemand@0x71d8df` / `GetQuestByNpc@0x71ddec` / `SetRecentlyUpdatedQuest@0x71dee4` / `RemoveRecentlyUpdatedQuest@0x71df1c` / `GetQuestDisplayMedal@0x71df41` / `IsStartScriptLinkedQuest@0x720e88` / `IsCompleteScriptLinkedQuest@0x720f09` / `IsAutoStartQuest@0x720f8a` / `IsAutoAutoAcceptQuest@0x720fab` / `IsAutoCompleteQuest@0x720fcc` / `CheckStartDemand@0x721163(20890B)` / `CheckCompleteDemand@0x721d2c(20060B)` / `LoadQuestInfo@0x722823(22479B)` / `LoadPartyQuestInfo@0x723341` / `LoadPartyQuestRank@0x7238dd` / `GetPartyQuestRankCount@0x724722`。

⚠️ 口径警示：`CNoticeQuestProgress::Update@0x6cec47`（missing EA）伪代码逐字 `int *__thiscall CNoticeQuestProgress::Update(_DWORD *this){ CWnd::Update(this); /*0x6cec4a*/ return CNoticeQuestProgress::NoticeProgressChange(this); /*0x6cec56*/ }` —— 32 字节 `[86,139,241,232,28,0,0,0,246,68,36,8,1,116,14,141,70,248,80,185,0,11,191,0,232,117,43,211,255,141,70,248]` 与第九面 `_GCWorldMapDlg` 同源，双端 identical。

## 3. 核心伪代码逐字（`COutPacket(107)` 全部 6 处上下文）

### 3.1 `CQuest::StartQuest @ 0x716fe1 (10701 B)` — 4 处发包

**(a) action 4 = start script-linked（带 x/y 坐标）：**
```c
if ( *(this + 5) == 0 && CQuestMan::IsStartScriptLinkedQuest(a1: *(this + 6)) != 0 ) /*0x717080*/
{
  COutPacket::COutPacket(a1: 107); /*0x717087*/
  COutPacket::Encode1(this: &v52, a2: 4); /*0x717094*/          // action=4
  COutPacket::Encode2(a1: *(this + 6)); /*0x7170a3*/              // questid (short)
  COutPacket::Encode4(this: &v52, a2: *(this + 4)); /*0x7170ae*/ // npcid (int)
  COutPacket::Encode2(a1: v50); /*0x7170b9*/                      // player x
  COutPacket::Encode2(a1: v51); /*0x7170c4*/                      // player y
  CClientSocket::SendPacket(this: dword_BE7914, a2: &v52); /*0x7170d3*/
}
```
**(b) action 5 = complete script-linked（同形，带 x/y）：** `COutPacket(107)` @`0x717104` + `Encode1(5)` + `Encode2(questid)` + `Encode4(npcid)` + `Encode2(v50)` + `Encode2(v51)` + SendPacket @`0x717150`。
**(c) action 1 = start（条件追加 x/y）：**
```c
COutPacket::COutPacket(a1: 107); /*0x7174db*/
COutPacket::Encode1(this: &v49, a2: 1); /*0x7174e9*/
COutPacket::Encode2(a1: *(this + 6)); /*0x7174f8*/
COutPacket::Encode4(this: &v49, a2: *(this + 4)); /*0x717503*/
if ( sub_721008(a1: *(this + 6)) == 0 ) /*0x71751c*/ {
  COutPacket::Encode2(a1: v50); /*0x717524*/
  COutPacket::Encode2(a1: v51); /*0x71752f*/
}
CClientSocket::SendPacket(...); /*0x71753e*/
```
**(d) action 2 = complete（条件 x/y + 尾 int）：** `COutPacket(107)` @`0x71745c` + `Encode1(2)` + `Encode2(questid)` + `Encode4(npcid)` + `if(sub_721008==0){Encode2(v50);Encode2(v51);}` + `Encode4(v30)` @`0x7174b9` + SendPacket @`0x7174c8`。

### 3.2 `CWvsContext::ResignQuest @ 0xa26ea7 (2215 B)` — action 3 = forfeit（最短包）
```c
COutPacket::COutPacket(a1: 107); /*0xa26f5a*/
COutPacket::Encode1(this: &v13, a2: 3); /*0xa26f70*/   // action=3
COutPacket::Encode2(this: &v13, a2); /*0xa26f79*/       // questid (short), 无 npc/无 x/y
CClientSocket::SendPacket(this: dword_BE7914, a2: &v13); /*0xa26f88*/
```

### 3.3 `CQuest::OnCompleteQuestFailed @ 0x718b58 (44961 B)` — action 0 + buffer
```c
COutPacket::COutPacket(a1: 107); /*0x719aba*/
COutPacket::Encode1(this: &pvarg, a2: 0); /*0x719ac7*/        // action=0
COutPacket::Encode2(a1: v115[6]); /*0x719ad6*/                  // questid
COutPacket::Encode4(this: &pvarg, a2: v125); /*0x719aed*/       // count
COutPacket::EncodeBuffer(a1: v212, a2: 4 * v126); /*0x719b08*/  // N×int
```

### 3.4 083 收包 `CWvsContext::OnQuestRecordMessage @ 0xa20f4c (12650 B)`
```c
LOWORD(v4) = CInPacket::Decode2(this: a2); /*0xa20f67*/   // questid
v5 = CInPacket::Decode1(this: v2); /*0xa20f76*/            // state
if ( v5 == 0 ) { ... } /*0xa20f9d*/
```
`OnQuestRecordExMessage @ 0xa2160b`：`v3 = CInPacket::Decode2(this: a2); /*0xa21628*/ CInPacket::DecodeStr(this: a2, a2: &v9); /*0xa2162e*/ sub_4E71FA(this: v4, a2: v3, a3: &v9); /*0xa21651*/` + `CUIQuestInfo::ResetInfo` / `CUIMedalQuestInfo::ResetInfo`。

## 4. c2s / s2c opcode 对照

| 方向 | 083 一手 | 当前端 source | 判定 |
| --- | --- | --- | --- |
| c2s QUEST_ACTION | `COutPacket(107)` action 0/1/2/3/4/5，包体随 action 变长、附 `Encode2(x)+Encode2(y)` | `Net/Packets/QuestPackets.h:10-23` `QuestActionPacket` `QUEST_ACTION=107`，`write_byte(action)+write_short(questid)`，action∈{1,2,4,5} 追加 `write_int(npcid)`，action3 无 npc | ⚠️ 数值一致(107)、包体结构性差异 |
| s2c 任务记录 | `OnQuestRecordMessage` `Decode2+Decode1` / `OnQuestRecordExMessage` `Decode2+DecodeStr` | `Net/PacketSwitch.cpp:93 UPDATE_QUEST_INFO=211` + `QuestHandlers.cpp` mode 6/7/8/0x0A/0x0F；`:265 QUEST_CLEAR=49`、`:347 SET_QUEST_CLEAR_FIELD=150`、`:348 SET_QUEST_TIME=151` | ⚠️ 两套命名空间，数值≠语义 |

> 083 facts §2 的其余 35 个 opcode（26/38/39/42/43/58/59/62/63/68/71/72/73/75/76/79/80/83/87/90/91/92/94/95/97/98/103/106/113/123/137/153/155/157/218）全部来自 `Request`/`OnXxxRequest` 子串命中的跨面函数，**不是 Quest 专属**，仅作「本 IDB 全局 opcode 存在性」旁证，不进本面 §3 校准行。

## 5. 数据族（083 SP_/IMG 节点 + 当前端 .nx）

- 083 侧（facts 命中）：`CNpc::ShowQuestList` `COutPacket(58)`（与第九面 NPC 同源，属任务列表开窗，跨面交叉）、`CQuestMan::LoadQuestInfo` `LoadDemand` `LoadPartyQuestInfo` `LoadPartyQuestRank`、`CUIQuestInfo` / `CUIMedalQuestInfo` / `CUIQuestAlarm` / `CNoticeQuestProgress` 族。
- 当前端 `.nxcache/Quest.nx`（`nx-quest-content-probe.json`）：root 6 sections = `Act.img` / `Check.img` / `Exclusive.img` / `PQuest.img` / `QuestInfo.img` / `Say.img`；`Data/Quest/*.img` 6 个裸文件（QuestInfo.img 1211640B / Say.img 2354999B / Check.img 695814B / Act.img 647228B / PQuest.img 6521B / Exclusive.img 173B）。
- 当前端数据语义（`Data/QuestData.cpp`）：`QuestInfo.img/<id>/{0,1,2…}` 分状态摘要页 + `parent`/`order` 连锁 + `area` 分区；`Check.img/<id>/0` 的 `lvmin/lvmax/npc/job/quest` 起始需求；`ensure_chain_index()` 扫 parent 建 `chain_totals`。

## 6. 限制与未知

- **子串假阳性**：`kws=("Quest","QUEST","quest")` 会命中 `Request`，153 funcs 中 57 个为跨面 `…Request…` label；facts §2 的「36 opcode」不能直读为 Quest 专属。已在 §2 口径与 README 明确隔离。
- **IMG 内容层未恢复**：orange-wz `IMG 解析失败`，F 样式维 12 项诚实 ⏳，等 P2 内容层恢复 + 真机观感。
- **REA subject 单列**：`rea_search_Quest.json` 的 26 命中大半是 `CNMRequest*`/`HttpSendRequestA`/manifest 子串，且 subject 非 13339，不冒充一手。
- **095 禁引**：`Maplestory95.i64` / `docs/features/port-095/**` 全程不作 083 依据。
- **真机缺口**：当前端无 c2s 107 完整 hex 抓包、无 083 action 0/2/4/5 分支的服务端回包对照，均记遗留。

## 7. 复现命令

```bash
python tools/ida_angel_quest_ea.py          # 153/34 -> symbol-inventory
python tools/ida_angel_quest_decompile.py   # DONE 153/153 ok (physical_keys=153, 三形态归一)
python tools/ida_angel_quest_crosscheck.py  # 152/153, missing 0x6cec47
python tools/ida_angel_quest_byteaudit.py   # VERDICT 153/153 by bytes
python tools/ida_angel_quest_facts.py       # WROTE 2367 lines / 106561 B
python tools/face11_gate_and_quest_probe.py # orange-wz structure + 合规 .nx 回退
```
