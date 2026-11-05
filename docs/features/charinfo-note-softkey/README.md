# CharInfo SoftKey 便签（NOTE_ACTION）

## 状态（2026-11-05）

| 项 | 状态 |
| --- | --- |
| 发包 | ✅ `UICharInfo` SoftKey `N` → `NoteSendPacket`（c2s 131 action0） |
| 对照窗 | `UIClaim` 早已同包 |
| s9 | `NoteActionHandler`：仅 `availableNotes>0` 落库 |

## 动机

parity-gap §1.2-E：「CharInfo 写 note 路径需复核是否真发」——此前 CharInfo 无 NOTE 出口，仅 Claim 壳有 SoftKey。

## 真机

见 [pending-live-tests §14](../pending-live-tests/README.md)。
