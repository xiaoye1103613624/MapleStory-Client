# 辅助装备左上紧靠 + 生命周期同步（2026-11-20）

## 决策

- 辅助不可独立拖窗，只跟主装备
- 快捷键 / StatusBar / 主窗关闭·ESC 全绑同开同关
- 神秘装备本轮只预留挂靠，不加窗
- `gap = 0` 无缝贴紧

## 贴靠公式

```text
addon.x = host.x - addon.width - gap   // gap=0
addon.y = host.y                       // 顶边齐
```

实现：`UIElement::anchor_left_of(host, 0)`（左侧越界时翻到右侧，再夹回视口）。

## 生命周期

| 入口 | 行为 |
| --- | --- |
| 装备快捷键 / StatusBar 装备钮 | 只 `emplace<UIEquipInventory>` |
| 主窗 ctor / makeactive | `sync_side_panels`：无辅助则创建，再 `makeactive` + 贴靠 |
| 主窗 deactivate / 关钮 / ESC | 辅助 `deactivate` |
| 主窗拖拽 / remove_cursor | 重算贴靠 |
| 辅助 ESC / 假关闭 | 转宿主 `toggle_active` |

辅助窗基类改为 `UIElement`（`LayoutPolicy::None`），不再写 `PosEQINV`。

## 神秘装备预留

`UIEquipInventory::sync_side_panels` 内注释占位：日后在辅助下方、主窗左侧挂靠，并随宿主同开同关。

## 验收点

- [ ] 开装备 → 辅助在左上、顶边齐、无缝
- [ ] 拖主窗 → 辅助跟位
- [ ] 关主窗 / ESC / 再按装备键 → 辅助同关同开
- [ ] StatusBar 与快捷键一致
- [ ] 辅助标题区不可拖走
