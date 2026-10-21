# UI manifest — cell / v1

- 样式来源：跟图（自签到窗 v4 量取单格）
- scale：1x
- 目标尺寸：67×71（单格）
- 状态列表：normal / mask / claimed（可拼装部件，非整窗盖满）
- 样板：无 / 跟签到窗
- 采样：未实测
- 目标节点路径：待定
- psd：./cell.psd（图层：`cell/normal`、`cell/mask`、`cell/claimed-label`）
- 文件：
  | 文件 | 说明 |
  | --- | --- |
  | `normal.png` | 单格空态底板（粉头+米黄，无「第N天」字） |
  | `normal-day1.png` | 参考：含「第1天」的单格裁切 |
  | `mask.png` | 单格半透明灰蒙版（RGBA，alpha=128 ≈ 50.2%） |
  | `claimed-label.png` | 「已领取」绿字+白描边，透明底 |
  | `claimed.png` | mask + 已领取字 合成（叠到格子上的一键态） |
  | `claimed-preview.png` | normal + claimed 预览样例 |
- 叠法（锚点）：
  1. 以格子左上角为 (0,0)，尺寸 67×71
  2. `mask.png` 全格对齐覆盖（粉头+米黄）
  3. `claimed-label.png` 叠在蒙版之上；锚点建议米黄区居中：  
     `x = (67 - labelW) // 2`，`y = 19 + ((71-19) - labelH) // 2`
  4. 「已领取」必须完整可见，不被蒙版裁切（文字层在蒙版之上、不透明）
- 蒙版：RGB `(68,68,68)`，alpha `128`（约 50%，区间 40–55%）
- 「已领取」字色：填充 RGB `(34,170,68)` / `#22AA44`；描边 RGB `(255,255,255)`（1px 8 邻域硬边）
- 字体：同窗体 — 新宋体 SingleBit 字模 1bit blit
- 验收：硬边无糊；蒙版半透明可见下层；已领取在蒙版之上完整
