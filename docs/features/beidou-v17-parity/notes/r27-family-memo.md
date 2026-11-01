# r27 · Family 信息解析 + Memo 列表

## Family

- 出站：`OPEN_FAMILY`(146) / `OPEN_FAMILY_PEDIGREE`(145)（BtTree）
- 入站：`FAMILY_INFO_RESULT`(95) → `cur/tot/today rep` + juniors + leader + name/message + entitlements skip → `UIFamily::set_info`

## Memo

| Mode | 包 | UI |
|------|-----|-----|
| 3 | ShowNotesPacket | `set_notes`（id/from/msg/fame） |
| 4 | SendNoteSuccess | tip |
| 5 | noteError | tip + error byte |
| C→S | `NOTE_ACTION`(131) discard | BtDelete → `NoteDiscardPacket` |

## 预留

Family ADD/ACCEPT/USE；Memo 时间戳显示；选行点击。
