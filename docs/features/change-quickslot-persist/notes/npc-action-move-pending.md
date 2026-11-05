# NPC_ACTION 移动分支 — 取证未齐（显式预留）

## 已证实（s9）
`NPCAnimationHandler`：
- `length == 6`：对话姿态（int oid + 2×byte）→ 广播同形
- `length > 6`：移动：`readBytes(length - 9)` 原样转发（实现怪异，需对照原生发包长度）

本端已有：`NpcActionTalkPacket`（对话）挂在 `TalkToNPCPacket`；下行 `NpcActionHandler` 粗分 `rem>0 → move / else stand`。

## 缺什么
1. **客户端上行移动包体**（REA/IDA：谁在何时 Encode 移动路径、字段序、与 Player move 是否同形）
2. **真机双端观感**：第二客户端看 NPC 是否播 move；单人无法验收「他人侧」
3. 不解 `length-9` 前不可瞎拼 body（禁止发明）

## 用户可怎么排查
- IDA/REA 搜 `NPC_ACTION` / `CNpc` move / `SendNpcAction`
- GM + 第二号同图，点会走动的 NPC，抓 c2s 197 长度>6 样例
- 对照 s9 日志 `MonitoredChrLogger` 对 NPC_ACTION 的监控输出
