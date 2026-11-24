# HUD A/B/C 收口 — 2026-11-18

范围：modern 底栏菜单钮 / modern 快捷栏对齐 / UIMiniMap 本地视口。未改 UISideIconBar / StatusBar3 submenu。

## A. modern 底栏菜单六钮

- orange-wz：`mainBar/menu` 六钮齐全；`normal/0.origin.x` 内嵌横排（CashShop=1, Event=-34, Character=-69, Community=-104, Setting=-139, Menu=-174）。
- 同一 `buttonPos`（QS 左侧）创建MapleButton，保留 WZ origin；`toggle_qs` 不再互斥隐藏。
- 点击：CashShop→进商城；其余开 submenu，子项 OpenUI（任务/装备/技能等既有路径）。
- `is_in_range` modern 底栏全宽，保证可点。

## B. modern 快捷栏对齐

- IDA：`CLayoutMan::AddSingleLayerRemoveOrigin`；`s_ptShortKeyPos@0x279c390` 相对控件 TL。
- StatusBar3 `quickSlot/backgrnd` origin=(-12,-3)、`layer:cover`=(-10,0)。
- 加载后 `Texture::shift(origin)` 清零，再按 `MODERN_QS_POS` 画图标+键帽。

## C. UIMiniMap

- `MAX_PANE_W/H = 200×150`；超限裁剪 + `refresh_map_viewport` 跟玩家。
- 标题加宽不放大 `map_view_*`。
- `BT_THEME`：`MapleButton(BtClaim|BtHide)`。
