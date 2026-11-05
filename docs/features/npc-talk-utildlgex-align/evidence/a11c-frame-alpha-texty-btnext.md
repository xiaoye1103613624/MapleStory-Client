# A11c — 框体 α / TEXT_Y 居中 / BtNext x450 vs 474

> 日期：2026-11-05 ｜ IDB：`BeiDou.exe.i64` ｜ IMG：`UI.nx` → `UIWindow.img/UtilDlgEx`（nx_inspect）

## 1. 框体 α（定案：整体 1.0 + 素材逐像素）

| 证据 | 结论 |
| --- | --- |
| IDA `sub_9A4F1D` 拼 t/c/s 前反复 `sub_402FAB(255, 3)` | IWzCanvas blit **不透明度 = 255** |
| `nx_inspect.bitmap_pixels` 对 `t/c/s` 的 α 直方图 | 主体 α=255（t 88.2% / c 98.9% / s 93.7%）；其余为描边抗锯齿中间档（17..136），**不是**整框 0.9 平涂 |
| 本仓 `Texture::draw` / `DrawArgument` 默认 opacity **1.0f** | 与原生 blit 255 同义 |

⇒ **无需再调程序 α**。框体观感由 UtilDlgEx 位图自身 RGBA 决定；改成别的全局乘数反而偏离原生。

复现：

```text
python docs/features/npc-talk-utildlgex-align/evidence/_probe_alpha.py
# 另见本文件附带的直方图脚本输出（同目录可重跑 nx 像素统计）
```

## 2. BtNext x450 vs 474（定案：两码各有主）

此前 R25 把参考帧右下 **46×18** 钮量成 474，又把 `NPC对话2.png` 的 450 当成「孤例未编码」。IDA 拆开后：

| 按钮 | IDA（`sub_9A7C8B` / `sub_9A8FEF`） | W≈528/529 时 X | 像素帧 |
| --- | --- | --- | --- |
| **BtOK** | `width - 54` | **474 / 475** | 参考帧 `ref_btn (474,519,46)` |
| **BtNext** | `width - (avatar?140:10) - 68` | **450 / 451** | `npc2 (450,493,44)` |
| **BtPrev** | `width - inset - 122` | **396 / 397** | — |
| **BtClose** | TEXT=`9` / LIST=`10` | 9~10 | 参考帧量 12（裁剪噪声） |

纵向（相对本仓 `height` = 正文拉伸高；R25 底带已校准 `BTN_DY=49` ≈ 原生 `wnd_h-26`）：

| 按钮 | IDA | 本仓 |
| --- | --- | --- |
| OK / Close / Yes / No | `wnd_h - 26` | `height + BTN_DY`（49） |
| Next / Prev | `wnd_h - 77` | `height + BTN_DY_NEXT`（49−51=−2） |

⇒ **474 是 OK，450 是 Next**；不是裁剪冲突。本仓已拆 `x_ok` / `x_next` / `y_bottom` / `y_next`。

## 3. TEXT_Y 居中（定案：按 IDA 动态算）

`sub_9A64FC`（LayoutControls，非 Avatar/Pet）：

```text
if text_h <= content_h:
  y = (content_h - text_h) / 2 + 28 + 7   # 28 = t.height
else:  # 本仓滑条路径
  y = 28 + 7
```

三帧墨迹顶 39/58/72 本就不是固定偏移——正是「随正文高居中」的表象。本仓：

- 成员 `text_draw_y` / `text_block_height`
- `change_text` 按上式写入；`draw` / SENDSIMPLE 命中共用
- 废除写死 `TEXT_Y=48`

## 4. 宋体 / 色

未改字体路径：名牌 `A11M` 白、正文 `A12M` `DARKGREY`，仍走思源宋体。
