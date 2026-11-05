# 登录链 SCROLL_LOCK 截图（2026-11-05）

## 缺口

- `Window::request_screenshot` / `capture_screenshot` 已通。
- 触发仅挂在 `UIStateGame` 的 `KeyAction::SCREENSHOT`；登录态 `UIStateLogin::send_key` 不处理，且 KEYMAP 未下发时 `keymap` 无 SCROLL_LOCK。
- SoftKey 尾巴「登录遮罩截图」验收因此在登录页按键无 PNG。

## 改动

- `Keyboard` 构造：默认 `SCROLL_LOCK → MENU/SCREENSHOT`。
- `UI::send_key`：全局消费 SCROLL_LOCK 或已映射 SCREENSHOT → `request_screenshot()`（登录/游戏一致）。

## 验收

pending-live-tests §3（叠图仍为 pending）。
