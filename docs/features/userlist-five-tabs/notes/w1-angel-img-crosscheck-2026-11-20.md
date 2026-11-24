# W1 Angel + IMG 对照刷新（2026-11-20）

## Angel IDB

- 会话：idalib-mcp `:13340` → `idb_open` `E:\MXD\反编译IDA\v83\083\v83.i64` session `2f583ce0`
- module：`` `Angel.exe ``（imagebase `0x400000`，hexrays ready）
- 注意：本机 `:13339` GUI 当时挂的是 `MapleStoryT.i64`，**不可**作本波证据。

## 符号确认（list_funcs `*UserList*`）

| EA | 符号 |
|----|------|
| `0x919b73` | `CUIUserList::OnCreate` |
| `0x91e073` | `CUIUserList::SetButton` |
| `0x91dba9` | `CTabFriend::OnClick`（行级 disable） |
| `0x91cf4f` | `CTabFriend::OnButtonClicked` |

反编译 JSON：`evidence/angel-w1-0x*.dec.json`；地址与 r1 / `_ida_pb` 一致，SetButton 语义未变：

1. 先对五组按钮数组全 `SetState(0)`（好友 11 / 组员 9 / 家族 12 / 联盟 11 / 拒绝 2）
2. 再按 `*(this+369)` 页签 enable 对应组：`0=Friend … 3=Alliance … 4=BlackList`

页签顺序（native）：**FRIEND / PARTY / GUILD / GUILD_ALLIANCE / BLACKLIST**。

## IMG（orange-wz）

见 `evidence/img-w1-userlist-children.md`。运行时节点与 r3 绑定树一致；`GuildUnion` 有完整 12 钮 + guildinfo*，可支撑空态页。

## 当前端缺口（相对 Angel，本波修）

1. `Tab` 枚举值：`BLACKLIST` 仍在 `UNION` 前 → 改为 Angel 顺序；`change_tab(buttonid)` 禁止再把 Buttons 值当页签。
2. SoftKey / 悄悄话发包体在后续 chat 提交中被掏空 → 按 Angel 语义恢复（Whisper→`ChatBar::set_whisper_target`；输名对话框→`UISoftKey`）。
3. `set_page_buttons(PARTY)` 未调 `update_party_button_states`。
4. 联盟入盟态成员模型仍预留（W1.5）。

## SoftKey 口径（用户 2026-11-20 澄清）

本窗 **SoftKey ≠ SoftKeyboard.img 软键盘弹窗**。输名/公告走 **物理键盘 → ChatBar / Textfield**；悄悄话走 `UIChatBar::set_whisper_target`（Angel SetWhisperTarget+StartChat）。禁止 `emplace<UISoftKey>` 回潮。

## 截图角色

正确端截图 / `native_*_tab.png` 仅终验；布局常量以 IMG origin + Angel OnCreate 坐标表为准。
