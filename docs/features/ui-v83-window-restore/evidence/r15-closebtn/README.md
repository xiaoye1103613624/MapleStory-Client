# R15 关闭按钮真身：`Basic.img/BtClose`（蓝底白 X），不是 `BtClose2`

用户原始反馈：**「关闭按钮样式不对，怎么是白色的，其他功能窗口也一样」**。
本轮把这条从「猜测」升级为「四张原生标题栏逐像素对照」。

## 1. 两个候选位图（`Data/.nxcache/UI.nx`，`tools/nx_inspect.py` 只读取证）

| 节点 | 尺寸 | 不透明像素比 | 8x 放大观感 | 文件 |
| --- | --- | --- | --- | --- |
| `Basic.img/BtClose/normal/0` | 12x12 | **0.97**（实心） | **蓝色方块 + 白色 X**，四角带黑描边 | `_r21_BtClose_8x.png` |
| `Basic.img/BtClose2/normal/0` | 14x14 | **0.24**（只有轮廓） | 黑色圆角方框线 + 黑 X，**内部全透明** | `_r21_BtClose2_8x.png` |

> 注：ASCII 探针里 `BtClose` 的 `mean=rgb(168,135,65)` 是**通道序写反**的产物
> （`bitmap_pixels()` 返回 BGRA，探针按 RGBA 求均值）。反转后为 `rgb(65,135,168)`，
> 与 8x 渲染的蓝色一致。位图导出路径本身没有这个问题。

`BtClose2` 内部全透明 ⇒ 贴在白色标题栏上，只剩一圈黑线，**看起来就是一块白的**。
这正是用户说的「怎么是白色的」。

## 2. 原生标题栏取证（四扇窗，互不相关，全部同一颗钮）

| 参考帧 | 窗口 | 裁切区域 | 文件 |
| --- | --- | --- | --- |
| `技能展开宏指令.png` | 技能 | (145,0)-(180,22) @10x | `_r21_nat_close.png` |
| `背包栏展开.png` | 背包 | (540,0)-(602,30) @8x | `_r21_nat_bag_close.png` |
| `好友列表框.png` | 好友 | (250,0)-(302,30) @8x | `_r21_nat_buddy_close.png` |
| `角色能力.png` | 能力值 | (0,0)-(428,46) @3x | `_r21_nat_stat_top.png` |

四帧一致：**实心蓝方块 + 白色 X**，方块约 12~13 源像素 ⇒ 与 `BtClose` 的 12x12 对齐。
`背包栏展开.png` 里同排还有两颗红底白符号的方钮（同族 `Basic.img` 状态钮），
说明原生这一排就是「实心色块方钮」体系，而不是描边 X。

## 3. 结论与改动

- `Util/UiAssetResolve.cpp` `close_btn()` 探测顺序由
  `{ BtClose3, BtClose2, BtClose }` 改为 **`{ BtClose, BtClose2, BtClose3 }`**。
  本包没有 `BtClose3`，所以旧顺序实际**永远命中 BtClose2**（错的）。
- `Util/UiAssetResolve.h` 的注释原先断言「`BtClose` 是实心方块不是 X、`BtClose2` 才是原生描边 X」
  —— 与像素证据相反，已重写。旧注释把「实心」当成了缺点，其实原生就是实心。
- 影响面：33 扇走 `close_btn()` 的功能窗（背包 / 能力 / 装备 / 技能 / 任务 / 好友 / 组员 /
  公会 / 商店 / 仓库 / 交易 / 组队 …）一次性回到原生观感。
- **未动**的类 D：`UIWindow.img/BtUIClose`（32x15 蓝色 CLOSE 长条胶囊，UIMessenger 等），
  那是另一族原生件，见 `notes/asset-node-inventory.md` §1.5。

## 4. 遗留

- 位置：33 扇窗的关闭钮坐标是按 14x14 写的，换成 12x12 后右/下边缘各内收 2px。
  逐窗校准属于「混合方案」的后续批次，本轮不铺开。
- `UIMessenger` 已按 v95 `SetCtrl` 改为 `(276,6)` + `BtClose3`/`close_btn()`（见 `messenger-btclose-coords`）。
