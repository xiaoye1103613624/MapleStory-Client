# GameOpt 同意掩码 → 邀请/聊天入站拦截

> 日期：2026-11-06 ｜ 纯客户端 ｜ 承接 `UIOptionMenu` B7-4 R1 / multi-resolution analysis

## 背景

`UIGameOpt` 12 行同意位早已落盘 `ui.gameopt.mask`（默认 `0xFFF`），`UIGameOpt::accepts(Option)` 可查，
但各邀请/聊天入站 handler 从未消费 → 勾「拒绝」仍弹 YesNo / 仍显示频道消息。

## 接线表（本轮已接）

| Option | 行义（底板烘焙） | 消费点 | 拒绝行为 |
| --- | --- | --- | --- |
| `WHISPER` | 悄悄话 | `WhisperResultHandler` 入站显示 | 静默丢弃显示 |
| `BUDDY_CHAT` | 好友聊天 | `MultiChatHandler` mode=0 | 静默丢弃 |
| `CHAT_INVITE` | 聊天邀请 | `MessengerHandler` mode=0x03 | `MessengerDeclinePacket` |
| `TRADE_APPLY` | 交易申请 | `TradeHandler` case 2 | `TradeExitPacket` |
| `PARTY_APPLY` | 组队邀请 | `PartyHandler` 0x04（非 PS） | `DenyPartyRequestPacket` |
| `PARTY_SEARCH` | 寻找组队 | `PartyHandler` 0x04（`PS: ` 前缀） | 同上 + 清 PartySearch 态 |
| `FAMILY_APPLY` | 家族邀请 | `FamilyJoinRequestHandler` | `FamilyAcceptPacket(..., false)` |
| `ACADEMY_INVITE` | 学院招待 | `FamilySummonRequestHandler` | `FamilySummonResponsePacket(..., false)` |
| `UNION_CHAT` | 联盟聊天 | `MultiChatHandler` mode=3 | 静默丢弃（含联盟窗 notice） |
| `UNION_APPLY` | 联盟邀请 | `AllianceInfoHandler` 0x03（`allianceInvite`） | 不发 `AllianceAcceptPacket`（2026-11-06 wedding-flow 同轮接上；原「显式预留」已消解） |

## 显式预留（证据不足，本轮不动）

| Option | 原因 |
| --- | --- |
| `GAME_INVITE` | 小游戏/迷你屋邀请入站 YesNo 路径未定位到独立分支 |
| `FAMILY_CHAT` | MultiChat 无独立 family mode；无其它家族聊天入站 |

好友申请 / 公会邀请不在上述 12 行枚举语义内，本轮不映射。

## 验证

- `tools/build_smoke.bat` Release|x64 GATE OK（占用 B）
- 真机：游戏设置取消「组队邀请」→ 收邀请应无 YesNo、对端见拒绝；默认全同意行为不变

## 触碰文件

`PartyHandlers` / `TradeHandlers` / `MessengerHandlers` / `WaveAShellHandlers`（Family）/
`MessagingHandlers` / `UIOptionMenu.{h,cpp}` 注释 / `Configuration.h` 注释
