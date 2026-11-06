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
### 键位几何还原（同日第三刀，接续 UI 审计硬约束）

旧 `UISoftKey` 用 `Login.img/Common/SoftKey` 140px 窄底板 + Journey `keypos`（12 列×45px，
x≤522）摆键 → 键帽漂出底板盖住 Messenger（实锤 `20261106_p7_admin_softkey.png`）。
V17.7 原生**两套软键盘并存**：Login.img 是 PIC 二级密码竖版随机键盘，
`UIWindow.img/SoftKeyboard` 才是窗口式 37 键 QWERTY。按用户决策（m00825）**改用后者**：

- 底板 `backgrnd/1/back` 315×127 金色卷轴 + title 147×34 + tray 278×95（三层 Sprite 各传 origin 抵消）。
- `key/0..36` 共 37 键 21×22 四态，逐键 `normal/0/origin` 即网格（列距/行距 23），
  字符 `KEY_CHARS="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-"` 由位图实证。
- 共享锚点 `ANCHOR_WINDOW=(283,30)`，`T=ANCHOR-raw_origin` 作为 `try_maple_btn` 的 top-left，
  键帽全部锁进 315×127 底板；k36 `'-'` 与 k35 `'Z'` 共用 origin，手工错位到数字行第 11 位。
- 5 funckey（CapsLock/左右 Shift/Enter/Backspace）+ BtClose 全还原；CapsLock 持久、
  Shift 一次性，默认输入小写。textfield/tooltip 从旧 x419 散落位收回窗口下沿。
- 保留 `Mode::{Pic,Text}`、5 构造签名（468 处调用点依赖）、物理键盘、回调、`check_pic`。

证据闭环：运行时 `UI.nx` 位图 → `evidence/keys/{keys_sheet.png,origins.json,keycoords.txt,
preview_full.png}`；终版审计
[softkey-layout-audit-2026-11-06.md](evidence/softkey-layout-audit-2026-11-06.md)。
`tools/build_smoke.bat` GATE OK + EXITCODE=0（16:45 新 exe，UISoftKey.obj 零 error）。

**真机对拍（键帽不漂出底板 / 不盖 Messenger）⏳ 待用户截图验证**：用户 2026-11-06 17:19
决定跳过本轮真机驱动（卡在登录页未进图），后续由用户手点截图验收，不视为已通过也不视为失败。

## 配置

验收期临时 `login.auto=true`（仓库外 `xy-config.yaml`）；测毕改回 `false`；口令不入库。
