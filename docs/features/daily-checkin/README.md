# 每日签到 UI

## 用途

跟图重绘「每日签到」窗体位图（中文）；另提供可拼装的单格「已领取」部件。当前仅归档，**未入库**。

## 文案

| 位置 | 文案 |
| --- | --- |
| 标题 | 每日签到 |
| 提示条 | 每天回来领取更多奖励！ |
| 格子 | 第1天 … 第28天 |
| 已领取态 | 已领取 |

## 产物路径

**当前验收版：window/v4 + cell/v1**

### 整窗

- `docs/features/daily-checkin/ui/window/v4/normal.png`（513×346，1x）
- `docs/features/daily-checkin/ui/window/v4/daily-checkin.psd`
- `docs/features/daily-checkin/ui/window/v4/manifest.md`

### 单格部件（可复用拼装）

- `docs/features/daily-checkin/ui/cell/v1/`
  - `normal.png` — 空态底板 67×71
  - `mask.png` — 半透明灰蒙版（alpha=128 ≈ 50%）
  - `claimed-label.png` — 「已领取」绿+白描边
  - `claimed.png` / `claimed-preview.png` — 合成样例
  - `cell.psd` + `manifest.md`

历史：`window/v1/`–`v3/`（编号/糊边/排版问题已由 v4 修复）

## 字体与清晰度（v4）

- 字体：新宋体（NSimSun）字模表，13px，`SingleBitPerPixelGridFit` → 1bit blit
- 去糊：禁止中间灰；粉头区 grayMid=0
- **文字清晰度：通过**

## 单格叠层参数

| 项 | 值 |
| --- | --- |
| 单格尺寸 | 67×71 |
| 粉头高 | 19 |
| 蒙版透明度 | alpha 128（≈50.2%） |
| 已领取填充 | RGB(34,170,68) |
| 已领取描边 | RGB(255,255,255) |

## 验收（window/v4 + cell/v1 · 已通过）

- [x] 整窗第1–28天唯一连续；粉头 grayMid=0；硬边清晰、对齐
- [x] `mask.png`：67×71，RGB(68,68,68) alpha=128，覆盖粉头+米黄
- [x] `claimed-label.png`：绿 RGB(34,170,68) + 白描边，透明底，可独立叠在蒙版之上
- [x] `claimed-preview.png` / `cell.psd`（normal / mask / claimed-label 分层）齐全

## 状态

- 样式：跟图；WZ 入库：未做
