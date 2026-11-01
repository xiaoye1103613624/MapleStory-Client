# W8 — 背包窗 (UIItemInventory) 版式取证

Root causes: RC2 (缺失按钮坐标) + RC11 (set_full 不重排).
Pack: `E:/MXD/BeiDou-Client-xy`. 资源根: `UI.nx` → `UIWindow.img/Item/*`（`UiAssetResolve::window("Item")` 映射到此；单独写 `Item.img` 会 NOT FOUND）。
`classic_item = !Item/pos && is_classic_hud()` → **TRUE**（本包有 StatusBar.img/base、无 StatusBar3、无 Item/pos），故走 classic 分支与 classic tab 路径。

坐标系: `try_maple_btn(...,position,...)` 的 `position` = **精灵左上角**(top-left)，内部再叠 `normal/0` 的 origin。
`TwoSpriteButton(nsrc,ssrc,pos)` 的 `pos` 是 anchor；因 `New/Tab*` origin=(0,0)，anchor==top-left。
`set_full` 里 `place(id,topleft)` 执行 `b->set_position(topleft + b->origin())`，因为按钮当初是按 `topleft+origin` 建的。
**依据(basis)**: ①=参考截图实测 (`正确客户端截图/背包栏（折叠）.png` 179×289 / `背包栏展开.png` 602×291)；②=nx_inspect 节点 W×H+origin (可信)；③=v83 惯例/烘焙美术推理。

---

## 节点清单 (W×H + origin，全部 basis ②)

| 节点 | 路径 | W×H | origin | 角色 |
|---|---|---|---|---|
| backgrnd | UIWindow.img/Item/backgrnd | 175×289 | (0,0) | 收缩框体（"现有道具"标题/红下划线/格网/金币字/滚动槽均烘焙在内）|
| FullBackgrnd | .../Item/FullBackgrnd | 603×289 | (0,0) | 展开框体 |
| New/Tab0 | .../Item/New/Tab0/{0..4} | 34×18 | (0,0) | **灰色底板**（未选中 tab）→ TwoSpriteButton normal |
| New/Tab1 | .../Item/New/Tab1/{0..4} | 34×19 | (0,0) | **粉色底板**（选中 tab）→ TwoSpriteButton mouseOver/pressed |
| Tab/enabled | .../Item/Tab/enabled/{0..4} | 27×16 | (0,0) | 文字层-白（选中态叠加）|
| Tab/disabled | .../Item/Tab/disabled/{0..4} | 26×14 | (0,0) | 文字层-灰（未选中叠加）|
| BtCoin | .../Item/BtCoin | 14×14 | (0,0) | 金币按钮 |
| BtFull | .../Item/BtFull | 12×12 | (6,0) | 展开钮（收缩态显示）|
| BtSmall | .../Item/BtSmall | 12×12 | (6,0) | 收起钮（展开态显示）|
| BtGather | .../Item/BtGather | 12×12 | (6,0) | 整理钮 |
| BtSort | .../Item/BtSort | 12×12 | (6,0) | 排序钮 |
| BtCashshop | .../Item/BtCashshop | 93×18 | (46,0) | "扩充道具栏"（仅展开态）|
| BtClose2 | Basic.img/BtClose2 | 14×14 | (0,0) | 共享关闭钮（见下"缺失件"）|
| VScr base | Basic.img/VScr/enabled/base | 12×12 | (0,0) | 滚动条夹持宽=12 |

> BtFull/BtSmall 与 BtSort/BtGather 各 **共用同一槽位**（同坐标，只激活其一）→ 解决了 RC2 中「两钮叠在 (109,6)」的问题。

---

## 布局表 A — 收缩态 (frame W=175, H=289)

| 控件 | 节点 | 左上角 (x,y) | 推导 | 依据 |
|---|---|---|---|---|
| Tab ×5 | New/Tab0\|1 [i] | (5 + i·34, 24), i=0..4 → x=5/39/73/107/141 | 分隔暗柱实测 x=6/39/73/107/141/175，板宽34 | ①② |
| Tab 文字层 | Tab/enabled\|disabled [i] | tab 左上 + (3,1) | (34-27)/2≈3, (18-16)/2=1 居中 | ②③ |
| Close | BtClose2 | (W-15, 3) = (160, 3) | 右上角内缩 | ① |
| 切换槽(Full) | BtFull | (W-51, 4) = (124, 4) | 右侧第三格 | ① |
| 整理/排序槽 | BtGather 或 BtSort | (W-31, 4) = (144, 4) | 右侧第二格；set_sort 决定显示哪个 | ① |
| Coin | BtCoin | (7, 266) | 左下金币 | ① |
| Meso 数字 | Text(A11M,RIGHT) | (141, 275) | 右对齐贴烘焙"金币"字左侧 | ①③ |
| Scrollbar | Slider(VScr) | x=157, 纵向 Range(46,250) | 12px 槽在窗口 x≈157；跨度覆盖 6 行格区 | ①② |
| Cashshop | BtCashshop | 不激活 (set_act false) | 收缩态无此钮 | ① |

滚动行程: `rowmax = get_slotmax(tab)/slot_col`（非硬编码 6）。行高 = (250-46-12·2)/rowmax。

---

## 布局表 B — 展开态 (frame W=603, H=289)

| 控件 | 节点 | 左上角 (x,y) | 推导 | 依据 |
|---|---|---|---|---|
| Tab ×5 | 同 A | (5+i·34, 24) | 顶部分类左锚定，**不随宽度移动** | ① |
| Tab 文字层 | 同 A | tab + (3,1) | 同上 | ②③ |
| Close | BtClose2 | (W-15, 3) = (588, 3) | 跟随展开框右缘 | ① |
| 切换槽(Small) | BtSmall | (W-51, 4) = (552, 4) | 右缘第三格 | ① |
| 整理/排序槽 | BtGather\|BtSort | (W-31, 4) = (572, 4) | 右缘第二格 | ① |
| Coin | BtCoin | (7, 266) | 左下不变 | ① |
| Meso 数字 | Text | (141, 275) | 烘焙字在框体左下，两态共用同坐标（旧 +84→y=359 溢出 289 已删）| ①③ |
| Cashshop | BtCashshop | (FullBackgrnd.x-113, 266) = (490, 266) | "扩充道具栏"位于展开框右下；旧 (80,285) 落在收缩框 289 之下=“窗口外” | ①② |
| Scrollbar | 隐藏 | — | 展开态显示全部格，无需滚动 | ③ |

`set_full` 用 `dimension = full_enabled?bg_full_dimensions:bg_dimensions` 重算 `W`，对 CLOSE/GATHER/SORT/FULL/SMALL 逐钮 `place()` 重排；CASHSHOP 恒按 `bg_full_dimensions.x()-113`。激活位: FULL=!full, SMALL=full, CASHSHOP=full, COIN=always。

---

## Tab 选型结论与理由

**方案: `New/Tab0`(灰板) 作按钮 normal、`New/Tab1`(粉板) 作 selected，`Item/Tab/{enabled,disabled}` 纯文字叠加于板上。**

- `Item/Tab/{enabled,disabled}` 尺寸 27×16 / 26×14 且实测是**透明底+字**（无底板像素）→ 单独做按钮即“底衬缺失”(用户症状 a)。
- `Item/New/Tab0|Tab1` 才是有填充的**底板** (34×18/34×19)；Tab0=未选灰、Tab1=已选粉。
- 故按钮挂底板（整 34px 可点、hover/press 自动出粉色反馈），文字按 `tab==tab_type_at(i)` 选白/灰在 draw() 叠加。
- 索引顺序由烘焙标签锁定: 0=装备(EQUIP) 1=消耗(USE) 2=设置(SETUP) 3=其它(ETC) 4=特殊(CASH)。修掉了原代码 SETUP/ETC 与图像 2/3 标签错位（仅显示路由，不碰封包）。

**只存在 0–4**：`Item/Tab` 与 `New/Tab*` 均无 index "5"，DEC(6) 无 tab。`tab_type_at(5)`/`get_tabpos` 越界返回 NONE/-1 → 若沿用“5”会静默透明；已显式限 5 个 `add_tab`。

---

## 缺失/保留件（本包没有，已绕行）

| 件 | 状态 | 处理 |
|---|---|---|
| BtSetup | ✗ 不存在，代码亦无引用 | 未使用 |
| 窗内专属 close | ✗ | 用共享 `Basic.img/BtClose2` |
| BtClose3 | ✗ 不存在 | `basic_close_btn()` 已删该候选，仅回 BtClose2 |
| Item/pos | ✗ | → classic_item=TRUE，走 classic 分支 |
| Tab index "5" / DEC tab | ✗ | 仅建 0–4 |
| backgrnd 标题“现有道具” | 已烘焙在图内 (175×289) | 无代码重绘 Text/Font（要求⑥满足，文件内本就无标题 Text 节点）|
| FullBackgrnd2/3, backgrnd2/3 | 可选装饰 | draw() 里 `.draw()` 无节点即空操作，保留 |

> 备注: 线上角色背包为空，道具格内容/tooltip 无法取证——不影响本批版式，未改取数逻辑、未造数据。
