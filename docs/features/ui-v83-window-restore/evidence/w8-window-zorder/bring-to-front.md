# R28-B3 证据：激活即置顶（目录 / 主菜单不再被遮挡）

## 1. 症状（用户第三轮验收原话）

> 「"目录"按钮点击后出现的"主菜单"ui层级有时候不对，会被遮挡」

「有时候」正是这条链路的特征：只在**同一个窗被关过再开**时才复现。

## 2. 根因（三行事实）

1. 绘制顺序 = `UIStateGame::draw` 顺序遍历 `elementorder`，**队尾最后画 = 最上层**
   （[UIStateGame.cpp:102-108](../../../../../IO/UIStateGame.cpp#L102)）。
2. 抬层的唯一既有机制在 `pre_add`：[UIStateGame.cpp:757-764](../../../../../IO/UIStateGame.cpp#L757)
   —— 但它**只对 `is_toggled` 的元素**做 `remove + push_back`。
   `UIGameMenu` 的 `TOGGLED = false`（[UIMiscShells.h:594-596](../../../../../IO/UITypes/UIMiscShells.h#L594)），
   走的是 else 分支（`remove(type)` 后重新构造），首次打开确实在队尾。
3. 第二次打开不重走构造：ESC 路径 [UIStateGame.cpp:495-496](../../../../../IO/UIStateGame.cpp#L495)
   与底栏「目录」钮 [UIStatusBar.cpp:727-729](../../../../../IO/UITypes/UIStatusBar.cpp#L727)
   都是 `else if (!gm->is_active()) gm->makeactive();`，而
   `UIElement::makeactive()` 历史上只有一句 `active = true;`（[UIElement.cpp](../../../../../IO/UIElement.cpp)）——
   **翻标志位，不重排**。期间只要有任何别的窗 emplace 过，目录就压在它们下面。

⇒ 缺的是「**激活**」这一等一的抬层动作，而不是「构造」时的抬层。

## 3. 改法

新增一条从元素到层级表的窄通路，只在 **false→true 跃迁**时抬层：

| 文件 | 改动 |
| --- | --- |
| `IO/UIState.h` | `virtual void bring_to_front(UIElement::Type) {}`（登录态无层级表 ⇒ 默认空实现，UIStateNull/UIStateLogin 不需改） |
| `IO/UIStateGame.h/.cpp` | override：已在队尾 / 不在 `elementorder` 里 ⇒ 直接返回；否则 `remove + push_back` |
| `IO/UI.h/.cpp` | `void bring_to_front(UIElement::Type)` 转发给当前 state |
| `IO/UIElement.cpp` | `makeactive()` 记住旧值，跃迁时调 `UI::get().bring_to_front(get_type())`；新增 `#include "UI.h"`（`UIElement.h` 先于 `UI.h` 被读，无循环包含） |

两条防御写在实现里并留了注释：

- **已在队尾就返回**：`std::list::remove` 会失效被删节点的迭代器；`draw()`/`update()`/`send_scroll()`
  都以引用遍历 `elementorder`，返回早退把这条风险面关掉。
- **不在 `elementorder` 就返回**：`emplace` 之前或 `remove()` 之后调用时，若无脑 `push_back`
  会留下指向空元素的幽灵条目（`elements` 是 `EnumMap`，`operator[]` 不会插入但会返回空槽）。

**没有动** `pre_add` 的 toggled 分支语义（那是每帧抬层，改了会让所有活动窗互相抢位），
也**没有**把 `UIGameMenu::TOGGLED` 改成 true（那会连带改变它的开关手感与音效路径）。

## 4. 静态自检：makeactive 会不会在遍历 elementorder 时被调到

对 `IO/**.cpp`、`Gameplay/**.cpp` 全量扫「 enclosing function ∈ {update, draw, update_screen, pre_add,
send_scroll, send_close, doubleclick, rightclick, send_icon, refresh_localized} 且调用 makeactive」：

- 命中 4 处：`MapDrops.cpp:60` / `MapMobs.cpp:41` / `MapNpcs.cpp:40` / `MapReactors.cpp:37` ——
  都是 **`MapObject::makeactive()`**（地图实体激活），与 `UIElement` 无关。
- `UIItemInventory.cpp` 8 处落在 `doubleclick()` 里，而 `doubleclick` 由
  `UIStateGame::doubleclick` 先 `get_front(pos)` 取单元素再调用，**不在遍历中**。
- 底栏三件套（STATUSBAR / CHATBAR / MINIMAP）没有任何 `->makeactive()` 调用点，
  所以不会出现「聊天栏被抬到所有窗上面」的反向回归。
- UIElement 构造函数里没有 `makeactive()` 调用（否则跃迁会发生在元素还没进层级表时）。

⇒ `bring_to_front` 的重排只会发生在 click / send_key / 封包处理里，均为 `elementorder` 遍历之外。

## 5. 验收状态

| 判据 | 状态 |
| --- | --- |
| 编译 0 error | ✅ |
| 链接 + 装到 OutDir | ⛔ `LNK1104 … MapleStory.exe`：并发会话的客户端仍在跑（PID 43744，另有历史僵尸 34476）。按其既定策略等它自己关，不结束进程 |
| 真机：先开装备窗/聊天窗 → ESC → 点目录，主菜单必须完整压在上面 | ⏳ 需用户上手 20 秒（层级问题对前台焦点敏感，脚本抓帧不可靠） |
| 回归：抽 3 个 toggled 窗（底栏、小地图）确认层级行为未变 | ⏳ 同上 |
