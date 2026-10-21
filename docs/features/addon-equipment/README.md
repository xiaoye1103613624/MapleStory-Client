# 辅助装备 UI

## 用途

跟图重绘「辅助装备」窗体位图（中文），2×4 槽位留空；当前仅归档，**未入库**。

## 文案

| 位置 | 文案 |
| --- | --- |
| 标题 | 辅助装备 |
| 上排四格 | 图腾 |
| 下排 | 纹章 / 机器人 / 心脏 / 徽章 |

## 产物路径

**当前验收版：v5**

- `docs/features/addon-equipment/ui/window/v5/normal.png`（152×105，1x）
- `docs/features/addon-equipment/ui/window/v5/addon-equipment.psd`
- `docs/features/addon-equipment/ui/window/v5/manifest.md`

历史：`v1/`–`v4/`（v4 标题贴顶；v5 居中 + 硬边强化）

## 字体与清晰度（v5）

- 标题：新宋体 **10px** GDI `SingleBitPerPixelGridFit`；白条内 **gap_top=3 / gap_bot=1**（相对 v4 blit y `4→12`，+8px）
- 槽位水印：11px SingleBit、字距 0；色 `#405468`
- 导出后强制二值清边；槽位 **grayMid=0**；标题字区仅黑/白
- 末格：**徽章**（非勋章）
- **文字清晰度：通过**

## 验收（v5 · 已通过）

- [x] 标题「辅助装备」白条内垂直居中（不贴顶）
- [x] 槽位图腾×4 / 纹章 / 机器人 / 心脏 / 徽章；硬边；grayMid=0
- [x] `normal.png` + `addon-equipment.psd` + `manifest.md` 齐全

## 状态

- 样式：跟图；WZ 入库：未做
