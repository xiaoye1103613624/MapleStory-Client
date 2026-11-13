# r02 · 任务框 / NPC 脚本任务窗（Angel ↔ 当前端）

## 1. Angel 一手

- `CScriptMan::OnScriptMessage @0x74660A` 公共头后按类型分派。
- 本轮反编译抽样：
  - `OnSay @0x7467ab`：`(speaker&4)` 可覆写 npc；`DecodeStr`；建 `CUtilDlgEx`。
  - `OnAskYesNo @0x74692c`：`DecodeStr` → `CUtilDlgEx`（AcceptDecline 同函数不同 a7）。
  - `OnAskMenu @0x746fad`：菜单字符串 → `CUtilDlgEx` LIST 态。
- `CUtilDlgEx` 具名族：`OnCreate_TEXT/YESNO/INPUT/MLINPUT/LIST/AVATAR/PET/IMAGE…`（`list_funcs *CUtilDlgEx*` 30+）。

## 2. 当前端路径

```
s2c ScriptMessage
  → NpcDialogueHandler::handle  (NpcInteractionHandlers.cpp:41+)
  → UINpcTalk::change_text / 专用壳 (Quiz/Pet/Avatar/Mirror…)
  → 用户操作
  → NpcTalkMorePacket (end_talk 分 lastMsg 契约)
  → s9 NPCMoreTalkHandler / 脚本 action
```

任务状态变更：

```
QuestLog GIVEUP / Alarm SoftKey / 壳软键
  → QuestActionPacket(action, qid [, npcid])  // OutPacket 107
  → s9 QuestActionHandler
  ← UPDATE_QUEST_INFO / Messaging / CLEAR …
  → QuestHandlers + UIQuestLog::refresh / quest_alarm
```

**观察**：`UINpcTalk` **不**直接 `dispatch` `QuestActionPacket`；接交由脚本 force* 或任务侧 UI 发 107。

## 3. 偏差

| 点 | 判定 |
| --- | --- |
| msgtype 主集与 end_talk s9 契约 | ✅ |
| 单体 UtilDlgEx vs UINpcTalk+壳 | 🟡 形态差 |
| 107 无 x/y、无 action0 | ⚠️ 保持 s9 |
| 任务指引 SetButtonBlink 链 | 🟡 未复刻 |
| CNoticeQuestProgress | 🟡 无独立浮层 |
| UtilDlgEx 薄壳 SoftKey | 🟡 |

## 4. 证据交叉

- `docs/features/npc-talk-utildlgex-align/README.md`
- `docs/features/quest-angel-calibration-2026-11/README.md` §3/§6
- `IO/UITypes/UINpcTalk.h:38-56`
- `Net/Handlers/NpcInteractionHandlers.cpp:43-100`
- `Net/Packets/QuestPackets.h`

## 5. P0 已确认口径（T3 / T5 / T4）

| 项 | 裁决 | 本批 |
| --- | --- | --- |
| T3 Quest 107 x/y·action0 | **不补**（保持 s9） | 零协议改动 |
| T5 NpcTalk 直发 107 | **不直发**；接交走脚本 + QuestLog/Alarm | 不改 UINpcTalk 发包 |
| T4 指引 blink / Progress 浮层 | 挂 **P2** | 本批不做 |
