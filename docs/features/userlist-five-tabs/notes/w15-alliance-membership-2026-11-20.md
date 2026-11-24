# W1.5 联盟入盟态（2026-11-20）

## 证据主路径

- Angel 083：`CTabGuildAlliance`（LoadData @0x9126f4 / GUILDITEM / OnPartyInvite @0x9153dc）、
  `CUIUserList::CreateAllianceButton @0x91eba3`、`SetButton @0x91e073`（既有
  `angel-social-facts.md` / `P3-A-BATCH-REST-MAPS.md`；本会话 IDA GUI 挂 MapleStoryT，**未**用 095 作布局）。
- IMG：orange-wz `UIWindow.img/UserList/GuildUnion`（backgrnd + guildinfo* + 12 Bt*）。
- s9：`ALLIANCE_OPERATION` 0x0C/0x0D/0x0E/0x0F/0x18/0x1A/0x1C/0x1D + c2s 0x01/0x02/0x03/0x04/0x0A。

## 实现

| 件 | 内容 |
|----|------|
| `Character/AllianceModel.{h,cpp}` | 常驻模型：头信息 + 成员行（GUILDITEM 对位） |
| `AllianceHandlers.cpp` | 回包写模型 + `UIUserList::refresh_from_models()`；0x0D/0x0F 解析公会块成员 |
| `UIUserList` UNION 页 | 未入盟空态保留；入盟绘公告/横幅/行列；行选中；按钮灰亮 |
| 按钮 | Invite/Withdraw/Chat/Notice/行操作有证据才亮；Kick/Change/Up/Down 初版 DISABLED |

## SoftKey

物理键盘 / ChatBar 提示输名与公告；禁止 `emplace<UISoftKey>`。

## 显式预留（已收口 → 见同目录后续 notes）

- Kick / 职级升降 / ChangeMaster：**已接线**（`notes/w15-alliance-outbound-clear-2026-11-20.md`）。
- Invite/Notice 正文：**已接** `UIEnterString`（`notes/enterstring-physical-kbd-2026-11-20.md`）。
- 真机入盟截图对拍：见终验清单 / 本波截图证据。
