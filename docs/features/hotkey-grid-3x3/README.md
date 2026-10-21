# 3×3 快捷键底板（多透明度）

用于摆放按键/技能图标的 3×3 空槽底板。

## v2（当前）— 跟用户截图

样式、大小、边框按用户提供的 4×2 快捷栏截图像素复刻为 3×3；并生成带键名版。

布局：

```text
1        2        3
Insert   Home     PageUp
Delete   End      PageDown
```

产物目录：[`ui/hotkey-grid/v2/`](ui/hotkey-grid/v2/)

| 文件 | 说明 |
| --- | --- |
| `hotkey-grid-{100,70,50,30,10}.png` | 空底板，**整板原色透明**（RGB 不变，只改 alpha） |
| `hotkey-grid-keys-{100,70,50,30,10}.png` | 带键名；过长按大写换行（PageUp→Page/Up） |
| `hotkey-grid.psd` | 分层源（chrome / SlotFills / Labels / Materials） |
| `ref-*.png` | 用户参考图归档 |
| `build_v2.py` | 生成脚本 |
| `_preview_sheet.png` | 十张对照预览 |

尺寸：114×113；单格 32×31；间距 3×4。

## v1

早期自绘近白格线版，见 [`ui/hotkey-grid/v1/`](ui/hotkey-grid/v1/)。已被 v2 跟图版替代为推荐交付。

## 未做

- 未写入客户端 WZ（目标节点待定）
