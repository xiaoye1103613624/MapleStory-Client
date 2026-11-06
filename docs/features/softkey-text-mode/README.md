# SoftKey Text 模式（2026-11-06）

## 问题

`UISoftKey` 按登录 **PIC/二级密码**实现，却被 Messenger 邀请/聊天、UIAdmin 命令等复用：

| Pic 约束 | 对自由文本的影响 |
| --- | --- |
| `set_text_callback` 吞字母数字 | 物理键盘无法输入 |
| OK 最短 6 字符 | 短角色名 / 短命令无法确认 |
| `check_pic` 重复字符 | 合法名字可能被拒 |
| `set_cryptchar('*')` | 明文场景仍显示密文 |

双开测 Messenger 时：空名牌能弹出 SoftKey，但 `type_text("xiaoye")` 不进框、无出站 `7a 00 03`。

## 改动

- `UISoftKey::Mode::{Pic, Text}`；完整构造可传 `Mode`（默认 **Pic**，登录链路不变）。
- **Text**：明文、最短 1、不设 text_callback、OK 跳过 `check_pic`。
- 调用方：`UIMessenger` 邀请/拒绝/聊天 SoftKey、`UIAdmin` SoftKey → `Mode::Text`。
- i18n：`messenger.prompt_invite`（zh-CN / en-US）。

## 验证

- `tools/build_smoke.bat` EXITCODE=0 → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
- Messenger 本机立绘 / 关闭钮真机见 [messenger-look/live-accept-2026-11-06-dual.md](../softkey-tails/evidence/messenger-look/live-accept-2026-11-06-dual.md)
- 邀请/blink：待 Admin 占线清除后复验 SoftKey 键盘输入路径
