# W12 名牌 `NameTag.img` 三切片 — 实测与公式

> 取证方式（只读）：`python tools/nx_inspect.py list "E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx" <depth> <path>`
> 尺寸/origin/clr 全部来自部署包 `UI.nx` 本体，非截图量边。日期：2026-10-31。
> 代码：[IO/Components/NameTag.cpp](../../../../IO/Components/NameTag.cpp)、[IO/Components/NameTag.h](../../../../IO/Components/NameTag.h)

## 1. 资产事实

- 根：`UI/NameTag.img`（本包 14 个 UI 根之一，✓ 存在）。✗ 无 `Frame` 根。
- 结构：`NameTag.img/<id> -> {w, e, c, clr}`；`NameTag.img/medal/<id>`、`NameTag.img/pet/<id>` **同构**。
- id 名义范围 **3…254**，但**区间内并非每个 id 都有件**：实测 `3..254` 里只有 **92 个** id 存在（33…39、41、43…57、58…92 之间大段缺失）⇒ 必须按节点存在性判定，不能只按范围。
- `medal{}` 130 个 id（含 `0`、`1` ⇒ medal 的 id 空间**不受** 3…254 约束）、`pet{}` 289 个 id。
- 三切片角色（用 `image` 导出后肉眼复核，见 §2 结论）：
  - `w` = **左帽**：alpha 列剖面左端渐隐、右端实心（id 10：`11,33,44,99,144,166,144,121,210,…,232,221,210`）。
  - `e` = **可横向重复的中段**：中段列 alpha 恒定（id 10 满列 232）⇒ 平铺无缝。
  - `c` = **右帽/端头**：整片列 alpha 完全恒定（id 10 全 210、id 124 全 213、id 254 全 204）。
  - 复合 `w|e|e|c` 渲染出来分别是：蝴蝶+深灰盒+端条（id 10）、卷轴+端条（id 254）、胡子+「I♥」+端条（id 124）⇒ **装配顺序 = w → e → c**，与任务给定口径一致。

## 2. 三切片尺寸实测表（抽样）

`o(...)` = 节点 `origin`（PKG4 向量内联解码后可信）。单位 px。

| 组 | id | `w` 左帽 | `e` 中段 | `c` 右帽 | `clr` |
| --- | --- | --- | --- | --- | --- |
| base | 3 | 7×18 o(7,0) | 7×18 o(0,0) | 4×18 o(0,0) | int 4294967295 |
| base | 5 | 7×18 o(7,0) | 7×18 o(0,0) | 4×18 o(0,0) | int 4278190080 |
| base | 10 | 20×23 o(20,0) | 20×23 o(0,0) | 4×23 o(0,0) | int 4294967295 |
| base | 12 | 22×17 o(22,0) | 22×17 o(0,0) | 4×17 o(0,0) | int 4294967295 |
| base | 15 | 15×21 o(15,0) | 15×21 o(0,0) | 4×21 o(4,0) | int 4294967295 |
| base | 20 | 17×25 o(17,0) | 13×25 o(0,0) | 2×25 o(0,0) | int 4294967295 |
| base | 40 | 27×32 o(27,0) | **31×34 o(4,0)** | **14×19 o(0,0)** | int 4294967295 |
| base | 112 | 28×28 o(28,0) | 13×28 o(0,0) | 1×21 o(0,0) | int 4294967295 |
| base | 117 | 29×32 o(29,0) | 31×33 o(0,0) | 1×20 o(0,0) | int 4283513316 |
| base | 124 | 37×21 o(37,0) | 15×21 o(0,0) | 14×21 o(0,0) | int 4278190080 |
| base | 178 | 26×23 | 26×23 | 3×17 | **string `'-4096'`** → 0xFFFFF000 |
| base | 250 | 52×29 o(52,0) | 20×24 o(0,0) | 1×23 o(0,0) | int 4282319008 |
| base | 252 | 35×34 o(35,0) | 17×27 o(0,0) | 1×24 o(0,0) | int 4278190080 |
| base | 254 | 11×17 o(11,0) | 20×17 o(0,0) | 3×17 o(0,0) | int 4278190080 |
| medal | 0 | 6×15 o(6,0) | 6×15 o(0,0) | 4×15 o(0,0) | int 4294967295 |
| medal | 10 | 11×17 o(11,0) | 11×17 o(0,0) | 5×17 o(0,0) | int 4294901708 |
| pet | 10 | 5×20 o(5,0) | 5×20 o(0,0) | 4×20 o(0,0) | int 4294967295 |
| pet | 100 | 6×20 o(6,0) | 7×20 o(0,0) | 2×20 o(0,0) | int 4278190080 |

抽样得到的三条硬约束（已写进实现）：

1. **`w` 的 `origin.x == w.width`**（92 个 id 全样本成立，仅 19/24/126 例外：20×21 o(18,0)、23×21 o(22,0)、30×30 o(27,0)）→ 左帽是**按右边缘挂点**授权的；`e`、`c` 多为 `origin.x == 0`，但 11/14/15/16/17/18/19/40 的 `c`/`e` 带 `o(4,0)`。⇒ 实现**不信 origin**，一律 `draw_pos = 目标左边缘 + tex.get_origin()` 反算（`DrawArgument::get_rectangle` 的 `rlt = pos - center - origin`，[Graphics/DrawArgument.h:134](../../../../Graphics/DrawArgument.h#L134)），使三片的可见盒左边缘精确落在算出的 x 上。
2. **同一片内三块高度可以不同**（id 40：32/34/19；id 250：29/24/23）⇒ 纵向**逐片**居中，不用统一 y。
3. `clr` 在 92 个 base id 里 **91 个 int + 1 个 string**（178）。

## 3. 宽度公式（验收判据的来源）

```
text_w   = OutlinedText::width()            // == Text::Layout::width()，T12b 度量
text_left = pos.x - text_w / 2              // CENTER 对齐：GraphicsGL add_line 里 line_x -= ax/2

left  盒 = [text_left - w.width , text_left]
body  盒 = [text_left , text_left + text_w]        // 平铺覆盖，见下
right 盒 = [text_left + text_w , + c.width]

框宽 = w.width + text_w + c.width          // 字形总宽 + 2 帽，**恒等，偏差 0px**
```

- `text_w` **只**取 `name.width()`（`OutlinedText::width()` → `Text::width()` → `Text::Layout::width()`），其值由 T12b 修好的 `LayoutBuilder::measure/wide_advance` 逐码点累加（[Graphics/GraphicsGL.cpp:1196-1234](../../../../Graphics/GraphicsGL.cpp#L1196)），**没有**再用 ASCII 估宽，也没有再引入 `6` 这类烤死余量（旧实现 `startpos = position - (6 + width/2)` 就是这个 6px 让中文名牌左右各差一截）。
- 中段 `e` 平铺：`tiles = max(1, round(text_w / e.width))`，每片宽 `text_w / tiles`，**最后一片吃掉余数** ⇒ 覆盖总宽精确等于 `text_w`（不拉伸单片，避免长名字糊成一条）。
- 纵向：沿用引擎自己的 NAMETAG 版式带 `top = pos.y + 6`、`height = layout_height - 2`（[Graphics/GraphicsGL.cpp:1321-1324](../../../../Graphics/GraphicsGL.cpp#L1321)），每片按自身高度在该带内居中。单行 A13M 算得框顶 ≈ `pos.y + 3`，与旧 CharSelect 路径的 `position.shift(1,2)` 实测差 ≤1px ⇒ 角色选择屏不退化。

## 4. `clr` 取值方式

- `clr` 是 **ARGB**（`0xFFFFFFFF` 白 / `0xFF000000` 黑 / `0xFFFFF000` 淡黄 / `0xFFA14046` 砖红 …）。
- 读取按 `nl::node::data_type()` 分派（[IO/Components/NameTag.cpp](../../../../IO/Components/NameTag.cpp) `read_argb`）：
  - `integer` / `real` → `get_integer()`（`node::get_integer` 对这两类不抛）。
  - `string` → `std::strtoll(base 10)`，**不走** `get_integer()`：`node::get_integer` 对 string 走 `std::stoll` 会抛 `std::invalid_argument`（[includes/NoLifeNx/nlnx/node.cpp:169-170](../../../../includes/NoLifeNx/nlnx/node.cpp#L169)）。
  - `none/vector/bitmap/audio` → 判为无 `clr`。
  - 统一 `& 0xFFFFFFFF` 后按 **A=R>>24, G=>>16, B=>>8, a=低字节** 组装；**不能**用 `Color(uint32_t)`（那是 RGBA 序，会把 alpha 当红）。
- `clr` 用作**文字色**（不是底色）：同一 id 的框体深浅与 `clr` 严格互补 —— id 10 深灰盒→白 `0xFFFFFFFF`、id 124 白底「I♥」→黑 `0xFF000000`、id 254 羊皮纸→黑。底由三切片位图自己提供。
- `Text` 只认固定 `Color::Name` 表，所以精确 ARGB 通过 `DrawArgument` 的乘色通道下发：五层字形基色改 `WHITE`，`abscolor = args.get_color() * colors[WHITE]` ⇒ 落色 == `clr`（[Graphics/GraphicsGL.cpp:1345-1353](../../../../Graphics/GraphicsGL.cpp#L1345)）。描边取同色 ×0.25。

## 5. medal / pet 入口与降级

- `NameTag(int32_t id, Group group, Text::Font, std::string)`，`Group::{NONE, MEDAL, PET}` → `NameTag.img/<id>`、`NameTag.img/medal/<id>`、`NameTag.img/pet/<id>`。3…254 的范围门闩**只**加在 `NONE`（medal 有 0/1）。
- 旧入口 `NameTag(nl::node, Font, name)` **保留**：`Login.img/CharSelect/nameTag` 实测就是 `<state>/<0|1|2>` 三片（`0`: 8×20,9×20,7×20；`1`: 8×22,9×22,10×22）→ 仍走 `load_triple`，选中/未选中两态与 `set_selected` 的 EAGLE↔WHITE 换色语义不变；只有当传进来的节点本身是 `{w,e,c}` 时才走 v83 路径。
- 缺件降级：`w/e/c` 任一不是位图 → 该态 `Frame()` 清空 → `draw()` **只画字不画底**；每个 tag 只 `UiAssetResolve::diag_once` 一行（tag 里带 id/组名，构造期探针安全，不逐帧刷屏）。范围外 id 同样降级 + 一行 diag。
- 带 `clr` 的框体只有一套（`NameTag.img/<id>` 没有选中态变体）→ 选中态沿用同一片框，只换文字色语义；不影响角色选择屏。

## 6. 本包没有 / 已放弃

- ✗ `NameTag.img/Frame`（旧代码从未取过，这里也不假设有）。
- ✗ `NameTag.img/<id>` 的**选中态**第二套切片 ⇒ 选中差异只能落在文字色上。
- ✗ 引擎没有「按 id 取名牌」的现成接线：`Npc`/`Mob`/`Char`/`PetLook` 的名签仍走 `Text::Background::NAMETAG` 的**程序画矩形**（`GraphicsGL::drawtext` case，近似绘制）。本批只授权 `NameTag.{h,cpp}`，**未动**任何调用点；`ms::NameTag` 现已具备 id/组/clr/度量全套能力，等主线把 id 来源（NPC/Mob 字段或 medal/pet id）接进去。
- ⚠ 本批未构建、未部署、未抓帧（按 batch-conventions §4）；`≤2px` 判据目前是**构造性恒等**（框宽 = 帽 + 量宽 + 帽，中段覆盖精确等于量宽），线上复验归主线统一抓帧轮。
