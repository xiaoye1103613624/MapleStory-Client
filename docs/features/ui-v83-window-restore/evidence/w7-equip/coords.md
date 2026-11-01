# W7 — 装备窗 (UIEquipInventory) 版式取证

Root cause: **RC2**（`try_maple_btn` 无坐标 → 叠在窗口原点 (0,0)）+ **§6-Q3**（左上两钮「二级密码」）。
Pack: `E:/MXD/BeiDou-Client-xy`。资源根：`UI.nx` → `UIWindow.img/Equip/*`（`UiAssetResolve::window4("Equip")` 映射到此）。

## 0. Q3 定案（沿用，不重复排查）

「二级密码」是**误读**：那是烤在绿色按钮贴图里的字。左上两钮真身 =
`UIWindow.img/Equip/BtDetail`（46×19，贴图文字「详细信息」）与 `Equip/BtCashshop`（32×11，文字「Shop」，`normal/0..5` 共 6 帧动画）。
证据：GBK+UTF-8 字节检索「二级密码」于 `BeiDou.exe` / 本仓库 / 整个部署目录（含 `Data/`）→ **0 命中**。
两者叠在左上角是 RC2 的表现（无坐标 → 落 (0,0)）。

## 1. 坐标系口径（batch-conventions §2）

- `ms::try_maple_btn(buttons, id, src, position, site)` 的 `position` = **贴图左上角**，内部把 `normal/0` 的 origin 加回成 anchor；资源缺失或未传坐标 → 不创建 + 一行 diag。
- `ms::MapleButton(src, pos)` / `TwoSpriteButton(n, p, pos)` 的 `pos` 是 **anchor**（`Texture::draw` 内部 `rect = pos - origin`，`bounds()` 同式）→ 两者与 `try_maple_btn` 的换算一致。
- `TwoSpriteButton(nsrc, ssrc, pos)` 内部 `textures[state==MOUSEOVER||PRESSED]`，`BoolPair` 的 `operator[](false)=first=nsrc` → **`nsrc` 是 NORMAL 态美术、`ssrc` 是 MOUSEOVER/PRESSED 态美术**（易记反，本批页签就靠这条将「灰=未选、彩=当前」落到正确位置）。
- 本包宠物页签需要「两张贴图表示两种状态」，`ms::try_maple_btn` 只会造单贴图 `MapleButton` → 保留一个本地 `try_pet_tab_btn`（`TwoSpriteButton` + 自己补 origin），**口径与 `try_maple_btn` 完全一致**：入参是贴图左上角、缺件时 `diag_once` 且不创建，绝不静默落 (0,0)。状态→美术的映射见 §7。
- 显隐用 `Button::set_active(false)`：`MapleButton::draw` / `TwoSpriteButton::draw` 都以 `if (active)` 为前提，`UIElement::send_key` 只命中 `is_active()` 的钮 → 收起即不可点，语义正确。

## 2. 实测方法

参考截图 `C:\Users\11036\Desktop\正确客户端截图\角色装备.png` = **348×305**，是紧贴窗口的裁图。
资产位图用 `python tools/nx_inspect.py image` 只读导出（未写任何 `.img`/`.nxcache`），再对截图做**逐像素模板匹配**（掩膜 `alpha>128`，度量 = 掩膜内 RGB 平均绝对差 mae）。脚本与原始输出：`meas_template_match.txt`、`meas_top3.txt`、`meas_edges.txt`；裁图 `pngs/`。

**窗口原点**：`Equip/backgrnd`(175×304) 全图最优 = abs **(0,0)**，mae 8.92；次优 (2,0) 已恶化到 20.8 → 截图左上角即窗口原点（mae 非 0 是背景半透明边与地图混色所致，不影响定位）。
下表 `窗口相对左上角` 与 `截图绝对左上角` 数值相同。

## 3. 节点清单（W×H + origin，全部来自 `nx_inspect`，可信）

| 节点 | 路径 | W×H | origin | 角色 |
|---|---|---|---|---|
| backgrnd | UIWindow.img/Equip/backgrnd | 175×304 | (0,0) | 窗体底图；**「角色装备」标题 + 全部槽名已烤进贴图** |
| pet | .../Equip/pet | 177×181 | (0,0) | 宠物装备面板底图；**13 个槽名（自动HP/宠物HP/自动MP/宠物MP/自动拣物/金币拾取/物品过滤/范围提升/项圈/宠物装备/速度提升/宠物戒指×2）已烤进贴图**；无 Slots 子节点 |
| BtPetEquipShow | .../Equip/BtPetEquipShow/normal/0 | 67×18 | (0,0) | 「宠物装备▶」展开钮（收起态显示）|
| BtPetEquipHide | .../Equip/BtPetEquipHide/normal/0 | 17×16 | (8,0) | 绿色左箭头，收起钮（展开态显示）|
| BtPet1/2/3 | .../Equip/BtPet{1,2,3}/{normal,pressed,mouseOver,disabled}/0 | 30×27 | (15,0)（四态同 origin） | 宠物面板三页签。四态美术语义（§7 实测）：`disabled`=灰底灰字=参考截图里的常态/未选页，`normal`=彩底红字=当前页，`pressed`=深灰按下，`mouseOver`=悬停 |
| BtDetail | .../Equip/BtDetail/normal/0 | 46×19 | (0,0) | 「详细信息」（Q3 钮 A）|
| BtCashshop | .../Equip/BtCashshop/normal/{0..5} | 32×11 | (0,0) | 「Shop」（Q3 钮 B）；6 帧 → `MapleButton` 自动走 animations |
| FullBackgrnd | .../Equip/FullBackgrnd | 208×291 | (0,0) | 存在，**本批未接入**，见 §6 |

## 4. 布局表 — v83 classic flat 分支（`classic_flat = !usable(Equip["Equip"])` → TRUE）

`right = bg_dimensions.x() = 175`，`bottom = bg_dimensions.y() = 304`。
依据：①=参考截图实测（模板匹配）②=nx_inspect 节点度量 ③=v83 惯例/兜底推理。

| 控件 | 节点 | W×H + origin | 代码算式 | 窗口相对左上角 | 实测 mae | 依据 |
|---|---|---|---|---|---|---|
| 窗体底图 | Equip/backgrnd | 175×304 (0,0) | sprite，随窗口 | (0,0) | 8.92 | ① |
| 关闭钮 | Basic.img/BtClose2 | 14×14 | `(right-19, 5)` | (156,5) | — | 既有 |
| **BtPetEquipShow** | .../BtPetEquipShow/normal/0 | 67×18 (0,0) | `(right-73, bottom-24)` | **(102,280)** | **0.07** | ① |
| **pet 面板底图** | Equip/pet | 177×181 (0,0) | `(right-3, bottom-181)` | **(172,123)** | 15.23（取左 120 列；面板半透明） | ① |
| **BtPet1** | .../BtPet1/disabled/0（未选页美术，见 §7） | 30×27 (15,0) | `(right+7, bottom-30)` | **(182,274)** | **0.00** | ① |
| **BtPet2** | .../BtPet2/disabled/0 | 30×27 (15,0) | `(right+41, bottom-30)` | **(216,274)** | **0.00** | ① |
| **BtPet3** | .../BtPet3/disabled/0 | 30×27 (15,0) | `(right+75, bottom-30)` | **(250,274)** | 16.5（该槽被手型光标压住，见 §7） | ① |
| **BtPetEquipHide** | .../BtPetEquipHide/normal/0 | 17×16 (8,0) | `(right+152, bottom-22)` | **(327,282)** | **0.74** | ① |
| BtDetail | Equip/BtDetail/normal/0 | 46×19 (0,0) | `(4, bottom-25)` | (4,279)；**默认 set_active(false)** | 全图最优 55.81 → 截图内不存在 | ③ 兜底角位 |
| BtCashshop | Equip/BtCashshop/normal/0 | 32×11 (0,0) | `(54, bottom-21)` | (54,283)；**默认 set_active(false)** | 全图最优 61.06 → 截图内不存在 | ③ 兜底角位 |

展开态命中区：`base_dimension = (175,304)`；`expanded_dimension = (pet_panel_pos + pet) = (349,304)`（与截图 348 宽一致，右缘被裁 1px）。`set_pet_panel()` 切换 `dimension`，`is_in_range()` 用 `dimension` → 面板打开时整块可点、关闭时收回。

绘制顺序：`draw_sprites` → **pet 面板底图** → `draw_buttons` → 图标。面板压在窗口右缘 3px 之上、页签与收起钮之上，不会盖住按钮。

## 5. pet 面板锚点复核（任务要求）

任务给的先验值与本次实测的差异及裁定：

| 项 | 先验（条带估读） | 本次模板匹配 | 裁定 |
|---|---|---|---|
| pet 面板 | (175,124) | **(172,123)** | y 一致（差 1px）；x 差 3px —— 面板**压住窗口右缘 3px**，不是紧贴 x=175。交叉验证：① 面板底边 123+181=**304 = 窗口底边**（严丝合缝）；② 收起钮 (327,282) − 面板 (172,123) = 面板相对 **(155,159)**，x 与先验「约 (155,148)」完全吻合；③ 三页签 182..280 落在面板跨度 172..349 内。→ 采用 **(172,123)**。 |
| BtPetEquipShow | (97,272) | **(102,280)** | 实测 mae **0.07**，且邻位 (104,280)/(100,280) 立刻恶化到 38.1/38.6 → 唯一像素级解。先验是条带估读，偏 5/8px。→ 采用 **(102,280)**。 |
| BtPet1/2/3 | 左边缘 181/216/251，间距 35，y≈268 | **182/216/250，间距 34，y=274** | 暗色描边列剖面（`meas_edges.txt`，y=276）：图标主体分别占 185..199 / 219..233 / 253..267，中心 192/226/260 → 30px 位图左缘 = 182/216/250；页签底缘描边在 y=299..301 = 274+27 ✓。→ 采用 **182/216/250 @ y274**。 |
| BtPetEquipHide | 面板内约 (155,148) | 窗口相对 **(327,282)** = 面板相对 (155,159) | x 吻合；y 差 11（先验按条带估读）。实测 mae **0.74** 为像素级。→ 采用 (327,282)。 |

结论：**四个宠物区控件 + 面板全部按实测落位，与截图偏差 ≤1px**（面板因半透明混色 mae 15.2 属正常，位置由锐利极小值确定；页签用 §7 定案的 `disabled` 美术后 mae 0.0）。

## 6. 本包不存在 / 已清理 / 未接入

- ✗ `Equip/backgrnd2` — 包内不存在。已删除非 classic 分支里残留的 `if (Equip["backgrnd2"]) sprites.emplace_back(...)`（classic 分支的同类引用此前已清）。
- ✗ `Equip/Equip`、`Equip/Cash`、`Equip/Pet`、`Equip/Android` 子根不存在 → `classic_flat = TRUE`，本包只走 flat 分支；多页签分支（含 `Equip/tabbar`、`Slots`、`disabled/disabled2`）在本包不可达，保持原样不动。
- **`FullBackgrnd`(208×291) 核对结论**：全图搜索最优 mae **20.68 @(0,0)**，明显劣于 `backgrnd` 的 8.92；参考截图（宠物面板展开态，348×305）里出现的正是 `backgrnd` + `pet` 两块，**没有 FullBackgrnd 的立足位置**（208 宽既容不下 flat 版式的 175+177=352 跨度，也不等于 304 高）。判定它是**多页签（HeavenClient 代次）布局的宽底图**，v83 classic 路径不使用 → 本批不接入，仅在此记录，避免后续批次重复猜。
- 标题/槽名重画核查（任务必做 4）：`UIEquipInventory.cpp` 全文无 `Text` / `Font` / `Charset` 取用（grep 仅命中 `Texture`）→ 「角色装备」标题与全部槽名**只有** `Equip/backgrnd` 烘焙这一份来源，宠物槽名**只有** `Equip/pet` 烘焙这一份来源，**无双影**，无需删除任何重画代码。

## 7. 页签四态实测（决定「谁当未选页美术」）

对三个页签槽位 (182 / 216 / 250 @ y274) 逐一取 `BtPet{1,2,3}` 的**全部四态**位图做模板匹配（`meas_tabstate.txt`，mae 越小越像）：

| 美术态 | x=182 | x=216 | x=250 |
|---|---|---|---|
| `normal` | 33.3 | 41.6 | 48.8 |
| `pressed` | 84.2 | 86.9 | 94.2 |
| `mouseOver` | 29.5 | 37.2 | 43.5 |
| **`disabled`** | **0.0** | **0.0** | 16.5 |

裁定（**推翻本批前一轮的猜测**）：

1. 参考截图里的三页签 = **`disabled` 美术**，`BtPet1@182`、`BtPet2@216` 都是 **mae 0.0（逐像素相同）**；`BtPet3@250` 的 16.5 只因为手型光标正压在该页签右半（见 `pngs/ref_tabs_zoom.png`、`pngs/BtPet1_states4_vs_ref.png` 下排）。
2. 四态美术真身（`pngs/BtPet1_states4_vs_ref.png` 上排，4× 放大）：`normal` = 浅蓝底 + **红**数字，`mouseOver` = 浅蓝底 + 蓝描边宠物，`pressed` = **深灰**按下，`disabled` = 灰底 + **灰**数字 ← 与截图同色。
3. 因此页签用 `TwoSpriteButton(disabled 美术, normal 美术, anchor)`：`Button::State::NORMAL` → 灰（未选页，= 截图态），`PRESSED`/`mouseOver` → 彩（当前页 / 悬停反馈）。`pressed` 美术是「按下瞬间」的深灰，不用于选中态（截图内不存在，mae 84~94）。
4. **为什么不能直接用 `Button::State::DISABLED` 表示未选页**：`Button::is_active() = active && state != DISABLED`，而 `UIElement::send_key` 只对 `is_active()` 的钮做命中 → 未选页会点不中。所以「灰」必须是 **disabled 美术**（贴图），态仍是 `NORMAL`。
5. 默认 `pet_tab = 0xFFFF`（哨兵）→ 三页签全灰、无高亮，与截图一致；`change_pet_tab()` 的范围校验对哨兵天然 no-op。截图该帧三页全灰，最合理的解释是**未召唤宠物**（原生把三页都置灰），宠物在场门闩列入 §9 预留。
6. 合成自检（`meas_composite_mae.txt`，用本表坐标把资产贴回画布与截图逐块比 mae）：展开钮 0.85、收起钮 1.61、页签行 4.81、宠物面板 9.16、整窗 12.05（残差来自半透明底图与地图混色，非位移）。

## 8. 显隐语义

| 态 | BtPetEquipShow | pet 面板 | BtPet1/2/3 | BtPetEquipHide | dimension |
|---|---|---|---|---|---|
| 收起（默认） | 可见 | 不画 | 隐藏 | 隐藏 | (175,304) |
| 展开 | 隐藏 | 画在 (172,123) | 可见，默认三页全 `disabled` 美术（灰，= 截图态）；点击后当前页换 `normal` 美术（彩底红字） | 可见 | (349,304) |

- 默认收起：与「原生默认不显示额外面板」一致；截图里面板是打开态，因为用户已点过展开钮。
- `BtDetail` / `BtCashshop`：构造后立即 `set_active(false)` → **默认不显示**（Q3 定案：参考截图两钮缺席）。`change_tab()` 里加了 `if (classic_flat) return;` 哨兵，防止页签显隐表把 `BT_SLOT`/`BT_SHOP` 重新点亮。
- 版式自检：`pngs/composite_vs_ref.png` = 左参考截图 / 右按本表坐标合成的渲染（backgrnd + pet + show + 三页签(disabled 美术) + hide），逐块对位一致；分块 mae 见 `meas_composite_mae.txt`。

## 9. 预留（本批明确不做，代码内留一行注释）

1. **宠物装备槽穿戴/卸下逻辑**：`Equip/pet` 下**没有 Slots 子节点**、无槽位命中区资产，面板只有这一张整图 → 本批只做「视觉 + 显隐 + 三页签切高亮」。注释位置：`UIEquipInventory::change_pet_tab()` 末尾。
2. **三页签不切换面板内容**：包内无分页宠物面板底图（只有一张 `pet`），`BtPet1/2/3` 只切「当前页美术」（disabled ↔ normal）。
3. **宠物在场门闩**：参考截图三页全 `disabled` 美术，最合理解释是**未召唤宠物时三页皆灰**（原生把页签置灰但本批为保持可点仍用 `NORMAL` 态 + 灰美术，见 §7.4）。真正的「无宠物 → 页签不可点」需要宠物状态数据，属协议/宠物批次，本批不接。
4. **BtDetail「详细信息」的真实行为**：无原生显隐/触发依据（截图内不存在该钮）。当前 `BT_SLOT` 分支沿用多页签布局的「现金槽扩充」语义（上一批接线），若将来要启用「详细信息」面板，需单独取证（原生 `CUIEquip` 该钮的 handler）。
5. **BtCashshop 点击**：已接 `EnterCashShopPacket`（出站 opcode 40）。注意 PLAN §2 RC7 判定该 opcode 与可用商城请求（`CASHSHOP_OPERATION=229`）不同线，属 s9 协议线待办；本批未改协议，钮本身默认隐藏，不产生新入口。

## 10. 交付文件

- 代码：`IO/UITypes/UIEquipInventory.cpp`、`IO/UITypes/UIEquipInventory.h`（仅这两个文件）。
- 本目录：`meas_template_match.txt`（全量模板匹配）、`meas_top3.txt`（锐利度/次优对照）、`meas_edges.txt`（边缘像素剖面）、`meas_tabstate.txt`（页签四态 × 三槽位 mae，§7 数据源）、`meas_composite_mae.txt`（合成图分块 mae，§7.6）、`pngs/ref_strip*.png`（截图条带裁图）、`pngs/ref_tabs_zoom.png`（截图页签放大）、`pngs/BtPet1_states4_vs_ref.png`（四态美术 vs 截图，§7 判读图）、`pngs/BtPet1_normal.png`、`pngs/BtPet1_disabled.png`、`pngs/pet.png`（资产导出）、`pngs/composite_vs_ref.png`（版式合成自检）。
- 临时裁图/脚本留在 `E:\project\_w7_*.png`、`E:\project\_w7_*.py`（未入库，W13 归档时清理）。
