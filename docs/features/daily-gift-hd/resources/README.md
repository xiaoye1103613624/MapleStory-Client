# Daily Gift · 扩展 UI 资源清单（含 0–9）

生成方式：Python/Pillow 程序绘制简易 HD 占位 PNG（本机字体）。  
可复现：在仓库根执行  
`python docs/features/daily-gift-hd/resources/_gen_placeholders.py`（若脚本已归档）。

**本版客户端不强制加载这些 PNG**；运行时灰框 + Text 数字。对接见 `../notes/hd-integration.md`。

## 0–9 数字（必交付）

| 文件 | 尺寸 | 用途 |
| --- | --- | --- |
| `digits/digit_0.png` … `digit_9.png` | 20×28 | 格子「第 N 天」、进度数字拼装 |
| `digits/digit_date_0.png` … `digit_date_9.png` | 16×22 | 顶栏日期范围拼装 |
| `preview/digits_0_9_strip.png` | 200×28 | 0–9 一览 |

代码对应：`UIDailyGift::digit_glyphs[10]` / `draw_digits` / `draw_day_label` / `draw_date_range`。

## 窗口 Chrome

| 文件 | 用途 | 运行时现状 |
| --- | --- | --- |
| `chrome/window_frame.png` | 窗口底框 | ColorBox 灰框 |
| `chrome/banner_green.png` | 顶栏绿横幅 | ColorBox 绿条 |
| `chrome/close_normal.png` 等四态 | 关闭钮 | 优先 Basic.img BtClose3/2 |

## 格子底板（中部空槽，无道具）

| 文件 | 用途 |
| --- | --- |
| `slots/slot_normal.png` | 普通格 |
| `slots/slot_highlight.png` | 重点天 |
| `slots/slot_hover.png` | hover |
| `slots/slot_pressed.png` | pressed |
| `slots/slot_claimed.png` | 已领取灰显 |
| `slots/claimed_mark.png` | 已领取角标 |

## 进度条

| 文件 | 用途 |
| --- | --- |
| `progress/bar_empty.png` | 空轨 |
| `progress/bar_fill.png` | 填充条（可拉伸） |
| `progress/bar_full.png` | 满条参考 |
| `progress/bar_cap.png` | 端帽 |

## 领取按钮四态

| 文件 | 状态 |
| --- | --- |
| `buttons/BtClaim_normal.png` | normal |
| `buttons/BtClaim_mouseOver.png` | mouseOver |
| `buttons/BtClaim_pressed.png` | pressed |
| `buttons/BtClaim_disabled.png` | disabled |

## 预览

| 文件 | 说明 |
| --- | --- |
| `preview/asset_sheet.png` | 资源总览拼板 |
| `source/` | 预留：从 WZ 导出的原片 |

## 标题/标语

整句中文标语本版用 `Text` 组件（不便整句位图）；数字必须用 0–9 资源拼装（代码 + docs PNG 双轨）。
