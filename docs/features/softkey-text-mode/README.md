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

### 影响联调的第二刀（同日续）

Text 聚焦后 `focus_textfield` → `begin_ascii_text_input`，`UI::send_key` 在
`text_input_active` 时**跳过** `KeyType::TEXT`，只等 `SDL_EVENT_TEXT_INPUT`。
自动化扫码与部分宿主不产 TEXT_INPUT ⇒ SoftKey 框一直空，邀请发不出，联调会误判
协议/立绘坏了。

- **Text**：`update()` 内 `end_ascii_text_input()`，走 KEY TEXT（ASCII 邀请名/命令）。
- **Pic**：保持 StartTextInput + 鼠标点键。
- Text tip：`show_text(..., formatted=false)`，避免 `#` 色码吞提示明文。

## 验证

- `tools/build_smoke.bat` EXITCODE=0 → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（10:06 部署）
- 双开邀请：Admin SoftKey 键入 `xiaoye` → 对方同意 → **双方立绘**见
  [messenger-look/live-accept-2026-11-06-dual.md](../softkey-tails/evidence/messenger-look/live-accept-2026-11-06-dual.md)
  与 `20261106_p9_admin_both.png` / `20261106_p9_xy_both.png`
- SoftKey **鼠标键位散落**（BeiDou SoftKey.img 底板与 Journey `keypos` 几何不匹配）仍挡
  Pic/点键路径 → 显式预留，见 live-accept「UI 挡操作」

## 配置

验收期临时 `login.auto=true`（仓库外 `xy-config.yaml`）；测毕改回 `false`；口令不入库。
