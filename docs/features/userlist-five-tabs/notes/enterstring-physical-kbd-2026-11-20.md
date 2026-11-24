# Invite/Notice/箴言等物理键 Textfield 对话框（2026-11-20）

## SoftKey 口径

`SoftKey` = 物理键盘 / ChatBar / 原生 Notice Textfield。**禁止** `emplace<UISoftKey>`（SoftKeyboard.img）。

## 实现

新增 `UIEnterString`（`IO/UITypes/UINotice.{h,cpp}`）：复用 `UINotice`/`UIEnterNumber` 的 Notice4 框 + `Textfield` + BtOK2/BtCancel2；确认回调发包。

| 入口 | 对话框文案 key | 发包 |
|------|----------------|------|
| 好友加 | `userlist.enter_name` | `BuddyAddPacket` |
| 好友加组 | `userlist.enter_group` | 本地 `BuddyListModel::set_active_group`（s9 无独立建组 opcode） |
| 组队邀 | `userlist.enter_name` | `PartyInvitePacket` |
| 黑名单增 | `userlist.enter_name` | 本地 blacklist + `FindPlayerPacket` |
| 公会邀/公告 | enter_name / enter_notice | `GuildInvitePacket` / `GuildNoticePacket` |
| 联盟邀/公告 | enter_guild_name / enter_notice | `AllianceInvitePacket` / `AllianceNoticePacket` |
| 学院箴言 / 加人 | enter_precept / enter_name | `FamilyPreceptPacket` / `FamilyAddPacket` |

Angel 对照：`CUtilDlgEx` INPUT_STR（联盟 Invite `sub_914CFE`、Notice `sub_915214`）。

## 未接 / 已收口

- `CWndAllianceGrade` 职衔名五段编辑（0x08）已接 `UIAllianceGrade`（输名仍走本窗的 `UIEnterString`）。见 `notes/alliance-grade-name-2026-11-20.md`。
- DENY_ALLIANCE 双字段（UIAlliance `D` 键）已串两段 `UIEnterString`。
