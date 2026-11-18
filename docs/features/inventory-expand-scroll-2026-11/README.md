# 背包上限×2 + 展开态交互增强 — 可行性与方案调研

> 调研日期：2026-11-17 ｜ 分支：`feature/dev_0.0.1`
> 状态：**代码已实现、双端已构建；真机首轮暴露 3 处 UI 缺陷已修（见 §8），等待复验**
> 详细代码级证据见 [notes/inventory-ui-findings.md](notes/inventory-ui-findings.md)；
> 实现计划/构建证据/P0 修复见 [notes/implementation-plan.md](notes/implementation-plan.md)。
>
> **已拍板（用户 m00159）**：选**方案 B**，翻页三件套整体置于展开态**右下角**，排列为
> `上一页 → 输入框 → 下一页`；输入框仅数字、默认 `1`、上限为服务端值、超上限钳到服务端值、
> 数值随翻页联动变化。布局与规则见 §4.2.1，真机修复见 §8。

---

## 0. 一句话结论

- **"展开态上下滑动"当前物理上不成立**：展开态是 4 页横向全铺、纵向 6 行全部可见，96 格一屏铺完，没有隐藏内容可滚。要滚就必须引入纵向分页/滚动层，这必然改变"一屏全铺"的内容组织。
- **"翻页 UI + 输入框"是更贴合你两个约束的方案**：它能做到**格子区 UI 样式零改动**（单屏仍是 4 页横向 96 格），只新增翻页按钮/页码/输入框，且 `UICashShop.cpp` 有 100% 可移植的完整样板。
- **上限×2 首先是服务端问题**：客户端上限由服务端 `InventoryGrowHandler` 下发；展开态展示上限 `max_full_slots=96` 是客户端硬编码，与服务端 `slotmax` 解耦——两个方案都必须先把这两者对齐。
- **你的"先填满 6 格高度→从左侧 7 格高度新位置开始"在当前源码里没有对应实现**，需你确认所指（详见 §5 抉择点）。

---

## 1. 你的三个诉求 · 逐条代码级结论

| # | 诉求 | 结论 | 关键证据（行号） |
| --- | --- | --- | --- |
| 1 | 未展开有下拉滑动、展开后没有 | 确认属实。根因是两处：展开态不画滑块 + 鼠标事件被 `!full_enabled` 挡掉；滚轮调用了但 slider 无有效 rowmax | `draw` cpp:364-381；`send_cursor` cpp:1053-1055；`send_scroll` cpp:1271-1277；`setrows` Slider.cpp:116-121/138-141 |
| 2 | 背包最大上限×2 | 需服务端先下发 `limit`（原 96→192）；客户端 `max_full_slots=96` 硬编码要改成跟随 `get_slotmax(tab)` | `InventoryHandlers.cpp:242-250`；`Inventory.cpp:111-114/247-252`；`UIItemInventory.cpp:183` |
| 3 | 不改展开态 UI 样式实现上下滑动 | **不可行（严格意义上）**。展开态纵向无隐藏内容；滚动必然改变"一屏 4 页全铺"的内容组织 | `get_slotpos` cpp:1677-1704（PAGE_COLS=4/PAGE_ROWS=6/page_dx={0,149,298,447}）；`max_full_slots` cpp:183 |

**根因链（展开态为什么滑不动）**：

```
Window.cpp:828 (滚轮)
  → UI::send_scroll (UI.cpp:156-158)
    → UIStateGame::send_scroll (UIStateGame.cpp:970-979)
      → UIItemInventory::send_scroll (cpp:1271-1277)  ← 无条件调 slider.send_scroll，但展开态 slider rowmax 无有效值 = 死代码

鼠标拖动:
  UIItemInventory::send_cursor (cpp:1053)  if (!full_enabled && slider.isenabled())  ← 展开态恒 false
    → slider.send_cursor 永远不被调用 = 滑块收不到拖动

绘制:
  UIItemInventory::draw (cpp:364-381)  full_enabled 分支只画 full_backgrnd*，不画 slider = 滑块不可见
```

---

## 2. 可行性判定

| 判定项 | 结论 | 说明 |
| --- | --- | --- |
| 展开态"纯上下滑动、UI 一像素不动" | ❌ 不可行 | 96 格已一屏全铺，无内容可滚；滑动需要有"屏幕外的内容" |
| 展开态滑动，但改变格子组织（纵向分页/缩小格子） | ⚠️ 可行但改 UI | 需重做 `get_slotpos`/`slot_by_position`/`is_not_visible`，与"不改 UI 样式"冲突 |
| 翻页 UI + 输入框，格子区样式不变 | ✅ 可行且低风险 | 每屏仍 4 页横向 96 格；192 格分 2 屏；只加翻页控件；有现成样板 |
| 上限×2 客户端独立实现 | ❌ 不行 | 上限由服务端 `set_slotmax` 下发，客户端只能跟随展示 |

---

## 3. 两方案对比

| 维度 | 方案 A：展开态上下滑动 | 方案 B：翻页 UI + 输入框（推荐） |
| --- | --- | --- |
| 核心做法 | 解除 `!full_enabled` 限制、展开态画滑块、重算 slider rowmax、把 `get_slotpos` 从横向 4 页改为纵向滚动 | 保留 4 页横向 96 格单屏样式；192 格分 2 屏；复用 `UICashShop` 翻页三件套 + `Textfield` 跳页 |
| 展开态格子区 UI 样式 | ❌ 改变（一屏全铺 → 纵向滚动/缩格） | ✅ 完全不变 |
| 新增可见控件 | 滑块（复用 VScr 资源，但展开态新出现） | 翻页按钮×2 + 页码标签 + 输入框（复用现成组件） |
| 组件复用度 | 复用 `Slider`（已有，但展开态需重新配置） | 复用 `UICashShop` 翻页逻辑 + MonsterBook 箭头按钮 + `Textfield`，**几乎零新造轮子** |
| 改动面 | 大：`draw`/`send_cursor`/`send_scroll`/`get_slotpos`/`slot_by_position`/`is_not_visible`/`set_full` 全要动 | 中：新增 offset 成员 + 翻页逻辑 + 3 处区间函数接入 offset + 控件绘制/事件 |
| 命中判定风险 | 高：`slot_by_position` 是 `get_slotpos` 逆运算，纵向化后易错位 | 低：只在现有 `get_slotpos` 上加 `offset`，逆运算同改即可 |
| 上限×2 适配 | 需服务端 + 客户端动态化 `max_full_slots` | 同左（两个方案共同前置） |
| 真机验收成本 | 高（拖动/滚轮/点选/拖拽全测） | 中（翻页/滚轮/输入框/点选/拖拽） |
| 与你约束的契合度 | 与"不改 UI 样式"直接冲突 | 同时满足"不改格区样式"+"组件复用" |

---

## 4. 方案设计

### 4.0 两个方案的共同前置（必须先做）

1. **服务端**：把背包 `limit` 从 96 改成 192（`InventoryGrowHandler` 下发，`InventoryHandlers.cpp:242-250`）。
2. **客户端展示上限动态化**：`UIItemInventory.cpp:183` 的 `max_full_slots = slot_col * max_slots`（=96）改为跟随 `inventory.get_slotmax(tab)`；`draw` 循环 cpp:389、`is_not_visible` cpp:1470-1478 同步。
3. 校验 `uint8_t limit`（`InventoryHandlers.cpp:247` 强转）能容纳 192（能，<256）。

### 4.1 方案 A：展开态上下滑动（复用 Slider）

改动清单（全部行号级）：

| 步骤 | 文件:行 | 动作 |
| --- | --- | --- |
| A1 | `UIItemInventory.cpp:1053` | 去掉 `!full_enabled &&`，改为 `if (slider.isenabled())`，让展开态滑块接收鼠标 |
| A2 | `UIItemInventory.cpp:364-371` | 展开态 `draw` 分支增加 `slider.draw(position)`（VScr 资源，视觉与折叠态一致） |
| A3 | `UIItemInventory.cpp:679-680` | 切页时 `slider.setrows(...)` 的 rowmax 用动态 `get_slotmax(tab)/slot_col`，覆盖 192 格 |
| A4 | `UIItemInventory.cpp:1677-1704` | `get_slotpos` full 分支由"page_dx 横向 4 页"改为"基于 `slotrange[tab].first` 的纵向行偏移"，`row = (slot - first)/slot_col` |
| A5 | `UIItemInventory.cpp:1630-1663` | `slot_by_position` 逆运算同步 A4 |
| A6 | `UIItemInventory.cpp:1470-1478` | `is_not_visible` 上限由 96 改动态 |
| A7 | `UIItemInventory.cpp:1271-1277` | `send_scroll` 保持调 `slider.send_scroll`，确认 rowmax 生效 |

**致命约束**：A4 必然改变"一屏 4 页横向全铺"的布局 → 与"不改展开态 UI 样式"冲突。若你能接受"格子区变纵向滚动"，此方案才成立。

### 4.2 方案 B：翻页 UI + 输入框（推荐）

设计要点：**保持单屏 4 页横向 96 格样式零改动**，把 192 格分成 2 屏（page 0: slot 1..96, page 1: slot 97..192），翻页切换。

新增成员（`UIItemInventory.h`）：

```cpp
int16_t full_page_offset = 0;      // 当前展开态页的首格偏移（0 / 96 / 192...）
Button*  bt_page_prev = nullptr;   // 复用 MonsterBook arrowLeft
Button*  bt_page_next = nullptr;   // 复用 MonsterBook arrowRight
Text     page_label;               // Text(A11B, CENTER, WHITE)
Textfield page_input;              // 跳页输入框
```

改动清单：

| 步骤 | 位置 | 动作 | 复用样板 |
| --- | --- | --- | --- |
| B1 | `UIItemInventory.cpp:1677-1704` | `get_slotpos` full 分支：`absslot = slot + full_page_offset`，保留 4 页横向 page_dx 计算 | 现成公式，仅加 offset |
| B2 | `UIItemInventory.cpp:386-387` | `firstslot = full_page_offset+1`, `lastslot = min(full_page_offset+96, slotmax)` | — |
| B3 | `UIItemInventory.cpp:1630-1663` | `slot_by_position` 逆运算接入 offset（slot = 命中格 + full_page_offset） | — |
| B4 | `UIItemInventory.cpp:1470-1478` | `is_not_visible` 按 offset 区间判定 | — |
| B5 | 新增 `page_count()` / `sync_page_chrome()` / `change_page(delta)` | 照搬 `UICashShop.cpp:1380-1408`：`pages=(slotmax+95)/96`, `cur=offset/96`, label=`cur+1/pages`, prev/next `set_state(DISABLED/NORMAL)` | `UICashShop.cpp:1380-1408` |
| B6 | `UIItemInventory.cpp:1271-1277` | `send_scroll` 改为 `change_page(upwards?-1:1)` | `UICashShop.cpp:252` |
| B7 | `send_cursor` / `button_pressed` | 处理 `bt_page_prev/bt_page_next` 点击 | `UICashShop.cpp:505-509` |
| B8 | `Textfield` 回车回调 | 解析输入页码（仅数字）→ 统一走 `apply_page(n)`（内部 clamp 到 `[1, page_count]`）| `Textfield.h:60` + `UI::focus_textfield(UI.cpp:546)`；详见 §4.2.1 |
| B9 | `draw` | 展开态画三件套 `bt_page_prev / page_input / bt_page_next`（右下角水平排列） | `UICashShop.cpp:431`；坐标见 §4.2.1 |
| B10 | `set_full` cpp:1753-1811 | 进入展开态 offset=0、`page_input` 复位 `"1"`、`sync_page_chrome()`；三件套仅展开态可见 | — |

**摆放规则兼容**：每屏仍是原 4 列×6 行 × 4 页 = 96 格；第 97 格起进入 page 1 的第 1 页位置——天然满足"先填满原有格子高度，再从新位置开始"。

---

#### 4.2.1 用户拍板：右下角翻页三件套（布局 + 输入契约）

> 来源：用户 m00159「方案B，展开时翻页组件整体布局放置右小角，布局：上一页、输入框（只支持数字输入，默认1，最大值为服务端值，超过还是服务端值，数值随翻页变化）、下一页」。

**排列**：水平三件套，置于展开态窗口**右下角**，左→右为 `上一页(◀) ｜ 输入框 ｜ 下一页(▶)`。
不再需要独立的 `page_label`——**输入框本身兼作页码显示**（默认显示 `1`，随翻页自动更新），
比原设计的"label + 隐藏输入框"更省空间、也与用户描述一致。

**输入框契约（逐条，可直接做验收）**：

| 项 | 规则 | 实现要点 |
| --- | --- | --- |
| 输入类型 | **仅数字**，非数字字符丢弃 | `set_accept_unicode(false)`（`Textfield.h:56`，默认即 false）+ 在 `ontext`/回车回调里过滤 `isdigit` |
| 默认值 | `1` | 构造后 `change_text("1")`（`Textfield.h:50`）；进入展开态 `set_full(true)` 复位为 `"1"` |
| 上限 | **服务端值**（页总数 `page_count = ceil(slotmax / 96)`） | 上限源 = `inventory.get_slotmax(tab)`（服务端下发，`InventoryHandlers.cpp:242-250`），不是写死 2 |
| 超上限 | 输入 > 上限时**钳到上限**（显示与生效值都取上限） | `n = clamp(n, 1, page_count)` 后再写回 `change_text(std::to_string(n))` |
| 联动 | 数值随翻页变化 | `change_page(delta)` 与输入框回车解析**共用同一个 `clamp + apply_page(n)`**：翻页后由 `sync_page_chrome()` 把输入框文本刷成 `cur+1`；回车输入后调 `apply_page(n)` → `full_page_offset=(n-1)*96` → `sync_page_chrome()` 回写 |
| 边界 | 输入 `0` / 空 → 钳到 `1`；非数字 → 忽略 | 同上 `clamp` |

**共享函数（消除"翻页改了输入框不跟着变"的双写 bug）**：

```cpp
void UIItemInventory::apply_page(int16_t n) {          // 唯一改页入口
    int16_t pages = page_count();                       // ceil(slotmax/96)
    n = std::clamp<int16_t>(n, 1, std::max<int16_t>(1, pages));
    full_page_offset = static_cast<int16_t>((n - 1) * 96);
    page_input.change_text(std::to_string(n));         // 数值随翻页回写
    sync_page_chrome();                                // prev/next 按钮 enabled 态
}
void UIItemInventory::change_page(int16_t delta) { apply_page(offset/96 + 1 + delta); }
// Textfield 回车回调：apply_page(std::stoi(page_input.get_text()));  // 内部再 clamp
```

> ▎[PUA生效 🔥] 把"翻页"和"输入框"收敛到**同一个 `apply_page` 入口**——双写页码是这类 UI 最典型的 bug 温床（点下一页输入框不变 / 输入页码按钮态不刷）。一个入口，两个视图，闭环。

**右下角坐标（✅ 已拍板 m00232，行号级依据）**：

| 控件 | 现有位置 | 依据 |
| --- | --- | --- |
| `BT_CASHSHOP`（"扩充道具栏"） | `(bg_full_dimensions.x() - 113, 266)` | 构造 cpp:273；`set_full` 重定位 cpp:1786、cpp:1790（仅展开态 active） |
| `BT_COIN` | `(7, 266)` | cpp:268 |
| 格区最后一行格顶 | `slot_pos.y + 5*34 = 50 + 170 = 220`，格高 32 → 底边 252 | `slot_pos=(8,50)`, `step_y=34`（cpp:1669-1675） |

用户拍板（m00232 verbatim）：**「共存时在"扩充道具栏" 左侧显示，位置固定，不受"扩充道具栏"是否显示影响」**。

固化规则：

- 三件套整体锚定在 `BT_CASHSHOP` **左侧**，共用同一条底边基线 `y = 266`（与格区底边 252 错开 14px，不压格区）。
- 横向锚点 = `BT_CASHSHOP` 左边缘 `bg_full_dimensions.x() - 113`，再向左让出三件套总宽 `W3`：
  `bt_page_prev` / `page_input` / `bt_page_next` 依次排在 `[W-113-W3, W-113)` 区间，`下一页` 最靠近 cashshop 按钮、`上一页` 最左。
- **关键解耦**：坐标在构造时按上式一次性算定并写死；`BT_CASHSHOP` 的显示/隐藏只由 cpp:1790 `set_act(..., full_enabled)` 切 active 状态，**不参与三件套坐标计算**。因此无论"扩充道具栏"是否显示，三件套位置恒定——这正是用户要的"位置固定、不受影响"。
- 三件套对现有控件的零改动：`BT_CASHSHOP`、`BT_COIN` 坐标与 `set_act` 逻辑一律不动；仅在新增的 draw 分支里按固定锚点绘制三件套。

> ▎[PUA生效 🔥] 把坐标从"运行时依赖 cashshop 状态"改成"构造时固定锚点"——一个解耦，同时消掉"按钮显隐导致翻页控件跳动"和"两控件抢同一像素"两个隐患。冰山下面还有冰山，这刀砍得值。

精确像素（三件套各组件宽度、间距、输入框内边距）需实施阶段用展开态原生帧量取；锚点公式与解耦契约已定死，不留二义。

---

#### 4.2.2 用户约束：翻页与「对齐/整理」功能保持统一（m00199）

> 用户 m00199「要注意背包对齐和整理功能，也要保持统一」。
> 结论：**对齐/整理本身是服务端权威 + 客户端切按钮态**，不本地重排物品；
> 因此"保持统一"= 翻页 offset 与整理链共用同一套刷新/复位口径，不能各改各的。

**整理/对齐链取证（行号级）**：

| 环节 | 位置 | 行为 |
| --- | --- | --- |
| 触发（按钮） | `UIItemInventory.cpp:552-563` | `BT_GATHER* → GatherItemsPacket(tab)`（对齐）/ `BT_SORT* → SortItemsPacket(tab)`（整理），仅发包，无本地重排 |
| 触发（快捷键） | cpp:1174-1183 | `G` → Gather、`O` → Sort，同样仅发包 |
| 回包 | `Net/Handlers/InventoryHandlers.cpp:54-68 GatherResultHandler` | `set_arrange_button(type, true)` → 显示 BtSort |
| 回包 | `Net/Handlers/InventoryHandlers.cpp:70-85 SortResultHandler` | `set_arrange_button(type, false)` + `clear_new()` → 显示 BtGather |
| 按钮态切换 | `UIItemInventory.cpp:1348-1394 set_arrange_button` | 按 `sort_enabled` 在 BtSort/BtGather（及 _SM 变体）间切 active；**展开态有独立 _SM 分支**（cpp:1377） |
| 切页签恢复 | `UIItemInventory.cpp:677-691` | 切 tab 时 `set_arrange_button(tab, arrange_bt_state[tab])` 恢复该页签上次对齐/整理态，不一律复位 |
| 物品重排 | `Net/Handlers/InventoryHandlers.cpp:87+ ModifyInventoryHandler` | 整理结果由服务端逐格下发，客户端被动刷新图标 |

**"保持统一"的 4 条设计契约（可直接验收）**：

| # | 契约 | 实现要点 |
| --- | --- | --- |
| U1 | **按钮态与翻页同源刷新** | 翻页 `apply_page()` 里**不碰** `sort_enabled`/`arrange_bt_state`（翻页只改 offset，不改整理态）；整理回包 `set_arrange_button` 也**不碰** `full_page_offset`。两者正交，互不串扰 |
| U2 | **整理后留在当前页** | 服务端重排是按整页签重排（slot 编号 1..slotmax），不随客户端 offset 变；整理回包后**保持 `full_page_offset` 不变**，输入框页码不变，用户看到的是"当前页物品被重排"，符合直觉 |
| U3 | **切页签 = 翻页归零，但整理态按页签恢复** | 复用 cpp:677-691 既有逻辑：切 tab → `apply_page(1)`（offset 归零、输入框复位 `"1"`）+ 保留 `set_arrange_button(tab, arrange_bt_state[tab])`。**不破坏**现有"各页签记住自己的整理态"行为 |
| U4 | **展开/折叠切换保持整理态，翻页归零** | `set_full(true)` 进展开态：`apply_page(1)` + 三件套显示；`set_full(false)`：offset 归零。**始终不动** `arrange_bt_state` / `sort_enabled` |

> ▎[PUA生效 🔥] 把"翻页"和"整理"定义为**正交维度**：翻页只动 `full_page_offset`，整理只动 `arrange_bt_state/sort_enabled`。谁碰谁的字段，谁就是 bug 源头。A6/U1–U4 一起写进验收清单，端到端闭环，不是头痛医头。

**新增验收项（追加到 §7）**：
- [ ] 展开态整理/对齐后：物品在当前页重排、页码与输入框**不跳变**（U2）
- [ ] 切页签后：页码归 1、输入框显示 1，但**该页签上次的对齐/整理态被保留**（U3）
- [ ] 展开↔折叠切换：整理态保留、翻页归零（U4）
- [ ] 翻页不会改变 BtSort/BtGather 的选中态（U1）

---

## 5. 待你拍板的抉择点（不拍板不进入实现）

| # | 抉择点 | 为什么必须你定 |
| --- | --- | --- |
| 1 | **"不改 UI 样式"的边界**：格子区一像素不动（→只能选 B），还是允许格子区变纵向滚动（→可选 A）？ | 这是 A/B 的分水岭；A 与严格"不改 UI 样式"直接冲突 |
| 2 | **"先填满 6 格高度→从左侧 7 格高度新位置开始"具体指什么**？当前源码无对应实现，可能是：①另一版本客户端布局；②服务端格子编号；③ equip 格 vs 普通格 | 目标布局理解错，方案全废 |
| 3 | **上限×2 是服务端已做/将做，还是要客户端一并改服务端**？ | 客户端无法独立决定上限；需确认 `InventoryGrowHandler` 的 limit 谁改 |
| 4 | ~~翻页控件的视觉/位置/底图~~ | ✅ **已拍板（m00159 + m00232）**：方案 B，右下角三件套「上一页/输入框/下一页」，输入框仅数字、默认1、上限=服务端值、超限钳位、随翻页联动；三件套固定在"扩充道具栏"左侧、位置固定、不受其显示影响。详见 §4.2.1；仅剩各组件精确像素宽度待实施阶段量取 |

---

## 6. 复用的现成组件（无需新造轮子）

| 组件 | 来源 | 关键接口/行号 |
| --- | --- | --- |
| 翻页逻辑三件套 | `UICashShop.cpp` | `list_page_count`/`sync_page_chrome`/`change_list_page` :1380-1408 |
| 翻页箭头按钮 | MonsterBook `arrowLeft/arrowRight` | `UICashShop.cpp:232-238`；`UIMonsterBook.cpp:170-173` |
| 页码标签 | `Text(A11B, CENTER, WHITE)` | `UICashShop.cpp:240` |
| 滚轮翻页 | `send_scroll → change_list_page` | `UICashShop.cpp:252` |
| 跳页输入框 | `Textfield` | `Textfield.h:40/60/68`；`UI::focus_textfield(UI.cpp:546)` |
| 滑块（方案 A） | `Slider` | `Slider.cpp:28-102/116-141/222-386` |

---

## 7. 验收清单（真机，未执行）

- [ ] 服务端下发 limit=192 后，展开态能显示全部格子
- [ ] 翻页按钮/滚轮/输入框跳页均生效，页码 `cur/pages` 正确
- [ ] 每屏格子点选命中正确（`slot_by_position` 无错位）
- [ ] 物品拖拽/使用/装备在翻页后仍正确
- [ ] 切页签（Equip/Use/Etc...）后 offset 正确重置
- [ ] 折叠态原有滑块行为无回归
- [ ] 展开态格子区 UI 样式与改前逐像素比对（方案 B 应满足）
- [ ] 展开态整理/对齐后：物品在当前页重排、页码与输入框不跳变（U2）
- [ ] 切页签后：页码归 1、输入框显示 1，但该页签上次的对齐/整理态被保留（U3）
- [ ] 展开↔折叠切换：整理态保留、翻页归零（U4）
- [ ] 翻页不改变 BtSort/BtGather 选中态（U1）
- [ ] 三件套固定在"扩充道具栏"左侧：`BT_CASHSHOP` 显示/隐藏切换时三件套坐标恒定、不跳动、不被覆盖（m00232）

---

## 8. 真机首轮反馈 · 3 处 UI 缺陷修复（2026-11-17）

用户提供**本端真机截图**（底部可见我加的孤立页码"1"，确认为修复后当前端，可作验收凭据），
反馈三问题，逐条行号级根因 + 修复：

| # | 真机现象 | 根因（行号级） | 修复 |
| --- | --- | --- | --- |
| 1 | 滚轮后翻页 UI（箭头）消失 | `sync_page_chrome` 原用 `set_active(full_enabled && cur>pages)`；服务端未下发192时 `page_count=ceil(96/96)=1`，两箭头 `set_active(false)` → `MapleButton::draw`(MapleButton.cpp:78)整颗不画 | 改 `set_active(full_enabled)` 常显 + `set_state(usable?NORMAL:DISABLED)` 变灰，对齐 `UICashShop.cpp:1390-1393`。`is_active()`=active&&state!=DISABLED(Button.cpp:57)，DISABLED 不变灰可点也不被 hover 点亮(UIElement.cpp:304-306) |
| 2 | 页码位置偏、没有可见输入框 | `Textfield::draw` 边框只在 `#if LOG_LEVEL>=LOG_UI` 画(Textfield.cpp:57-59)，Release 裸文字 | 背包 UI 新增 `ColorBox page_input_frame`（浅灰底 0xDDDDDD），draw 时在 `page_input.draw` 前画(`:458`)，Release 可见 |
| 3 | 页码/输入框离右边"扩充道具栏"太近且按钮重叠 | 布局 prev=W-167/input尾=W-129/next=W-131，next 与 input 重叠、next 距 cashshop(W-113)仅 5px | 重排：以 cashshop 左缘 anchor=W-113 往左，next=anchor-8-14、input=next-3-24、prev=input-3-14 → prev[423,437]/input[440,464]/next[467,481]/cashshop[489,]，两两 3px、离 cashshop 8px、零重叠 |

**改动文件**：仅 `IO/UITypes/UIItemInventory.h/.cpp`（不动公共 Textfield）。
**编译**：全新目录 Rebuild（`inv_expand_build2.log`）→ `BUILD_EXITCODE=0`，error C / error LNK / fatal error 全 0；产物 4,531,712 字节，mtime 2026-11-17T20:49:05+08:00，SHA256=`17c03fa3fc304d559093d05cd37a98eaeab3b9c6ba07464325f82dc4ec859793`（与修复前 `f558a18d…e368e` 不同，证明新代码入链）。

**部署（2026-11-17 21:14 用户指令"exe编译到live目录"）**：
- 部署前无 MapleStory 进程占用（Win32_Process/tasklist 均空）。
- live canon = `E:\MXD\BeiDou-Client-xy\MapleStory.exe`，部署前 SHA `47bd997e992589cff7b96939b28fb433c2bc6c3d8dc25aeb800dcb92ffdc0769`（19:56 的另一功能线包）。
- 回滚点：`E:\MXD\BeiDou-Client-xy\MapleStory_before-inventory-pages_47bd997e.exe`（SHA 前16 `47bd997e992589cf`）。
- 原子覆盖（临时名→rename）后 live SHA = `17c03fa3fc304d559093d05cd37a98eaeab3b9c6ba07464325f82dc4ec859793`，`MATCH_NEW=True`；延迟 8 秒复查仍一致（未被即时回滚）。
- **风险**：记忆红线——登录器/后台可能再次回滚覆盖 exe，旧部署声明会失效；下次真机验收前须实时重核 live SHA。服务端翻页上限是否下发192（`@setslot 192` / DB 字段）仍未做，客户端单页时箭头灰色常显属正确行为。
**待复验**：用户真机重新部署后确认——右下角完整显示 `◀ [1] ▶` 三件套、带可见输入框底框、与扩充道具栏有间距；`@setslot 192` 后翻页按钮变为可用态、滚轮/点击翻页生效。

> **剩余风险声明**：真机首轮暴露了静态验证无法覆盖的 UI 布局问题，这类问题只能靠真机截图发现。所有"已修"仅代表代码层修复 + 编译通过，**未经真机复验前不宣称修复完成**。不要拿参考端截图当本端证据；声明"已部署/已修复"前须实时核对 live exe SHA。
