# Layout Contract — 商城窗口 / v1（验收基准）

- 基准来源：用户提供的**原生正确态**截图（sha256 前缀 577061394736…，800×600）。按用户定性为验收参考，非本端证据。
- 版本口径：用户 2026-11-21 拍板**沿用 live 热卖版**（tab 文字/一级顺序/二级 12 类不动）。因此本 contract 只用于校验"与版本口径无关的真残缺"补全后的色区/对齐/安全区；tab/二级分类数量不适用于此 contract。

```yaml
layout:
  scale: 1x
  bg: { id: CashShop.img/Base/backgrnd, size: [800, 600] }
  content_regions:
    - { name: preview_well, rect: [20, 45, 212, 165], fill_hint: preview_bg, default_align: center }
    - { name: mode_tabs, rect: [20, 218, 212, 20], fill_hint: title_blue, default_align: center }
    - { name: char_id, rect: [20, 258, 212, 40], fill_hint: parchment, default_align: left }
    - { name: purchased_grid, rect: [21, 349, 206, 70], fill_hint: slot_white, cols: 6, rows: 2 }
    - { name: backpack_grid, rect: [23, 481, 140, 105], fill_hint: slot_white, cols: 4, rows: 3 }
    - { name: bag_ext_buttons, rect: [176, 470, 56, 125], fill_hint: panel_gray, rows: 5 }
    - { name: product_grid, rect: [275, 95, 412, 405], fill_hint: card_blue, cols: 2, rows: 5 }
    - { name: best_panel, rect: [695, 130, 80, 340], fill_hint: purple_band, rows: 5 }
    - { name: search_bar, rect: [690, 97, 80, 24], fill_hint: title_blue, default_align: center }
    - { name: balance_rows, rect: [280, 540, 205, 42], fill_hint: white_field, rows: 3 }
    - { name: pager, rect: [560, 503, 100, 18], fill_hint: strip, default_align: center }
  overlays:
    - { id: mode_tab_0..3, region: mode_tabs, align: center, gap: 4 }
    - { id: badge, region: product_grid, anchor: top-left, inset: [4, 2, 0, 0] }
    - { id: card_icon, region: product_grid, anchor: top-left, inset: [5, 5, 0, 0], cell: 64 }
    - { id: card_name, region: product_grid, anchor: top-left, inset: [74, 17, 0, 0] }
    - { id: card_price, region: product_grid, anchor: top-left, inset: [74, 38, 0, 0] }
    - { id: card_btns, region: product_grid, align: bottom, gap: 39 }
    - { id: best_icon_i, region: best_panel, align: center, cell: 32 }
    - { id: balance_value_i, region: balance_rows, align: right }
    - { id: page_label, region: pager, align: center }
  sibling_rules:
    - { group: [BtBuy, BtGift, BtCart], align: bottom, gap: 39 }
    - { group: [ext_equip, ext_consume, ext_setup, ext_etc, ext_trunk], align: left, gap: 25 }
  grid: 4
  tol_px: 1
```

## 验收维度（工匠返回后逐项填 PASS/FAIL）

| ID | 检查 | 期望 | 实测 | 判定 |
| --- | --- | --- | --- | --- |
| L1 | 徽章 | 落在卡片左上蓝色 EUEW 区，不压 icon well | 待填 | 待验 |
| L1 | 4 模式 tab | 落在预览窗下方蓝条内，不压角色/按钮 | 待填 | 待验 |
| L1 | 扩栏六按钮 | 落在背包右侧灰色区，不压格子 | 待填 | 待验 |
| L2 | 商品卡三按钮 | 全在卡片浅色面板内，不溢出底边 | 待填 | 待验 |
| L3 | tooltip | 悬浮时出现在卡片右侧，不遮当前卡片 | 待填 | 待验 |
| L4 | 三按钮对齐 | 底对齐、gap=39、误差≤1px | 待填 | 待验 |
| L4 | 余额数字 | 右对齐在同一 x，三行等距 | 待填 | 待验 |
| L5 | 分页串 | `<1> [2] [3]…` 居中在底条 | 待填 | 待验 |

## 说明

- 真机截图须为**本端**方可作为 L1-L5 实测凭据；参考图只定"目标观感"，不能充当验收证据（USER.md 2026-11-17 教训）。
- tab/二级分类 12≠13 属已拍板的版本口径差异，不判 FAIL。
