# Wave2 · 底栏 / 聊天 / 快捷键

## 公式（权威）

`Util/HudLayout.h`：

- `VWIDTH < 800+qs` → `quickSlot08`，`qs_x = hud_grid_w - qs08_w`
- `VWIDTH >= 800+qs` → 宽 QS，`content_w=800+qs`，整组 `hud_center_x` 居中，QS 贴 800 右侧
- ChatBar 共用 `hud_center_x` / `hud_content_w`

## 防闪

`UIStatusBar::update_screen` 在 QS 宽窄翻转时直接 return；`UIStateGame` `qs_flip` 整窗重建（先卸后 emplace），避免一帧双绘。

## 1280 oracle

截图：`正确客户端截图\底部信息栏、按钮、快捷键窗.png` → `evidence/screens/statusbar-hud.png`（1282×142）。

量测：`1282 − 816(功能钮条) = 466` = `HudLayout::QS_PANEL_W_26`；`test_hudlayout.py` → `HUDLAYOUT_OK`。
