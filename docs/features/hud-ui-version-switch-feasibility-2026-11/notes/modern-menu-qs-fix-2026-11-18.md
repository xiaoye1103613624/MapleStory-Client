# modern 底栏：菜单六钮 + quickSlot（2026-11-18）

## 真因

1. **菜单缺失**：`StatusBar3` 六钮靠 `normal/0.origin.x` 横排（Menu=-174 / Setting=-139）。`Texture::sanitize_origin` 清掉 `|ox|>132` → Menu/Setting 叠到 CashShop 上「看不见」。
2. **菜单不可点**：`UIStateGame::get_front` 优先 CHATBAR 宽命中区，盖住右侧六钮/QS。
3. **quickSlot 空/偏移**：底板 `backgrnd` origin=(-12,-3) 未 RemoveOrigin 时，槽相对 `s_ptShortKeyPos` 偏左上；8 槽仍画 557 宽底板显「大片空」。

## 修复

- `UIStatusBar.cpp`：显式 TL + pitch35 + `try_maple_btn`；六钮后画；QS `shift(origin)` + 8 槽裁宽；`cursor_on_interactive`。
- `UIStateGame.cpp`：interactive 命中优先 STATUSBAR。

## 验收

1. ThemePack → Modern：右下六钮可见，点 Menu/Character/Setting 能出 submenu / OpenUI。
2. quickSlot 格内有键帽（及已绑定图标），与底板格子对齐。
