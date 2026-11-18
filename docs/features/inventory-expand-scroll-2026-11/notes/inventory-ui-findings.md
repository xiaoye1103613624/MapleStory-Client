# 背包展开/折叠与滚动 — 代码级取证（2026-11-17）

> 只读取证，未改业务代码。所有行号基于分支 `feature/dev_0.0.1` 当前工作树。
> 证据来源：`IO/UITypes/UIItemInventory.h/.cpp`、`IO/Components/Slider.h/.cpp`、
> `IO/UITypes/UICashShop.cpp`、`IO/Components/Textfield.h`、
> `Character/Inventory/Inventory.cpp`、`Net/Handlers/InventoryHandlers.cpp`。

## 1. 为什么"未展开有下拉滑动、展开后没有"

| 环节 | 折叠态（`full_enabled=false`） | 展开态（`full_enabled=true`） |
| --- | --- | --- |
| 背景绘制 | `draw()` cpp:372-381 画 `backgrnd*` + **`slider.draw(position)`** | cpp:364-371 画 `full_backgrnd*`，**不画 slider** |
| 鼠标拖动/点击 | `send_cursor()` cpp:1053 `if (!full_enabled && slider.isenabled())` → cpp:1055 `slider.send_cursor()` | 条件为 false，**直接跳过**，滑块收不到鼠标事件 |
| 滚轮 | cpp:1271-1277 `slider.send_scroll(yoffset)` 无条件调用 | 同样调用，但展开态 slider 的 `rowmax` 未按 96 格配置（见 §2），实际无有效滚动量 |
| 可见 slot 区间 | `firstslot=range.first, lastslot=range.second`（cpp:386-387） | `firstslot=1, lastslot=max_full_slots=96`（cpp:386-387） |

结论：展开态"没有滑动"是**两处导致的**：`draw` 不渲染滑块 + `send_cursor` 用 `!full_enabled` 把滑块事件挡掉；滚轮则落到"调用了但无滚动量"的死代码。

## 2. 展开态布局的真实结构（决定"上下滑动"可行性）

`get_slotpos()` full 分支（cpp:1677-1704）：`PAGE_COLS=4, PAGE_ROWS=6`，
`page_dx[] = {0,149,298,447}`，`page = absslot/24, rem = absslot%24`，
`row = rem/4, col = rem%4`，返回 `slot_pos + (col*36 + page_dx[page], row*34)`。
即**4 页横向全铺、每页 4列×6行、纵向 6 行全部可见**；`max_full_slots = slot_col * max_slots = 96` 硬编码（cpp:183）。

关键矛盾：展开态纵向**没有隐藏内容**——96 格在 4 列宽 × 6 行高的窗口里已全部铺开，`page_dx` 是横向偏移。所谓"展开背包上下滑动"，在当前布局下没有可滚的东西；要支持上限×2（≈192 格）并"上下滑"，必须引入纵向分页/滚动层，这会改变当前"一屏铺开"的内容组织方式（是否算"改 UI 样式"需用户拍板）。

另：`slot_by_position()`（cpp:1630-1663）命中判定是 `get_slotpos` 的逆运算；`is_not_visible()`（cpp:1470-1478）full 态按 `1..max_full_slots(96)` 判定。任何滚动/翻页方案必须同步这两处，否则**点选命中会错位**。

## 3. 上限的真正来源

`Inventory.cpp:111-114 set_slotmax` 直接写 `slotmaxima[type]`；`get_slotmax` 在 cpp:247-252。
`Net/Handlers/InventoryHandlers.cpp:242-250 InventoryGrowHandler::handle` 读 `type/limit` 后
`set_slotmax(inv, static_cast<uint8_t>(limit))`。
→ **客户端背包上限由服务端下发**；"最大上限×2"首先是服务端配置/协议问题，客户端只负责按 `get_slotmax` 展示。客户端目前只用 `max_full_slots=96` 作为展开态展示上限，与服务端 slotmax **解耦**。

## 4. "先填满原 6 格高度 → 从左侧 7 格高度新位置开始"

当前源码里没有直接对应：展开态是 4 页横向、每页 4列×6行；折叠态是 4列×N行。
该摆放规则可能指：①另一版本客户端；②服务端格子编号布局；③equip 格 vs 普通格。
**需用户确认具体所指**，否则方案会建立在错误假设上。

## 5. 可复用的现成组件

### 5.1 翻页（UICashShop.cpp 已落地，可直接移植）
- 按钮 `BtPagePrev/BtPageNext` 复用 MonsterBook 的 `arrowLeft/arrowRight`（cpp:232-238，注释 `UIMonsterBook.cpp:170-173`）；`page_label = Text(A11B, CENTER, WHITE)`（cpp:240）。
- `list_offset` 成员（cpp:65）；`draw` 画 `page_label`（cpp:431）；`send_scroll` 中 `change_list_page(upwards?-1:1)`（cpp:252）；按钮回调 cpp:505-509。
- 完整逻辑：`list_page_count()`=(total+MAX-1)/MAX（cpp:1380-1383）；`sync_page_chrome()` 算 `pages / cur = offset/MAX`，label 文本 `cur+1/pages`，prev/next 按钮 `set_state(DISABLED/NORMAL)`（cpp:1385-1394）；`change_list_page(delta)` clamp 后 `new_offset=cur*MAX`，更新 `update_items()+sync_page_chrome()`（cpp:1396-1408）；`update_items` 用 `i+list_offset` 索引（cpp:1410-1415）。

### 5.2 跳页输入框
`Textfield.h`：构造 `(font, alignment, color, bounds, limit, marker_height)`(:40)；`set_enter_callback(std::function<void(std::string)>)`(:60)；`get_text()`(:68)；`set_state/change_text`(:49-50)；`send_key/send_cursor`(:46/:64)；`UI::focus_textfield(UI.cpp:546)` 管理焦点。输入框回车回调解析页码 → `change_list_page` 绝对跳转，是现成模式。

## 6. 滚轮事件路由（验证过）
`Window.cpp:828` → `UI::send_scroll(UI.cpp:156-158)` → `UIStateGame::send_scroll(UIStateGame.cpp:970-979，遍历 active element)` → `UIItemInventory::send_scroll(cpp:1271-1277)`。链路通畅，问题只在展开态 slider 无有效 rowmax。
