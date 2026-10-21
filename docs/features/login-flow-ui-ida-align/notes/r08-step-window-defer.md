# R08 · 步进窗口延迟激活与端点定位

> 日期：2026-10-21  
> 依据：`BeiDou.exe` IDA，`CLogin::ChangeStep @0x5F53C0`、`sub_5F4C16`、`CLogin::OnStepChanged @0x5F5962`

## 1. 参考端时序

`ChangeStep` 只负责启动补间：

- 记录 `tween_from = current_offset()`、`tween_to = GetStepHeight(next)`；
- `tween_duration = 300 * |next - cur| + 500`；
- 把结束时间写入 `CLogin+364`。

`sub_5F4C16` 是唯一调用 `OnStepChanged` 的位置：

```c
if (now - *(this + 364) > 0)
    CLogin::OnStepChanged(this);
```

也就是说，新窗口的销毁/创建发生在**补间结束之后**，滚动期间不会提前显示下一屏。

## 2. 当前端问题

1. `UIStateLogin`/`UI::emplace` 在包体处理时立即构造并激活新窗口；
2. 窗口构造函数在 `set_step()` 之后调用 `get_viewpos()`，此时补间刚开始，
   返回的是**插值起点**而不是目标端点；
3. 结果：新窗口一出现就停在半途坐标，等背景滚到端点后与告示牌/木牌错位。

## 3. 本次实现

### 3.1 端点视角

`IO/LoginBackground.h` 新增：

```cpp
Point<int16_t> get_target_viewpos() const;
```

返回 `屏幕中心 - target_offset(step)`，与补间进度无关。

构造期改用该端点：

- `UIWorldSelect`：`worldsrc_pos` / `channelsrc_pos`
- `UICharSelect`：`position` 与木牌按钮反推
- `UIRaceSelect`：`position`
- `UILogin`：告示牌 `sign_topleft`

### 3.2 延迟激活

`UIState` 新增构造后钩子：

```cpp
virtual void post_add(UIElement::Type, UIElement*) {}
virtual void activate_after_transition(UIElement::Type) {}
```

- `UI::emplace` 在构造完成后调用 `state->post_add()`；
- `UIStateLogin::post_add` 对 `LOGIN / WORLDSELECT / CHARSELECT / RACESELECT / CLASSCREATION`
  检查 `LoginBackground::is_transitioning()`，若补间中则 `deactivate()` 并记录 `pending_activation`；
- `UIStateLogin::update` 在补间结束后 `makeactive()` 并清空待激活项；
- “返回上一页”不重建窗口，改走 `UI::activate_after_transition()`：
  - `UICharSelect::BtPreview` → 世界选择；
  - `UIRaceSelect::show_charselect` → 选角；
  - `UIRaceSelect::show_worldselect` → 世界选择。

## 4. 观感

- 滚动期间只显示背景，新窗口在端点出现；
- 参考端会把旧窗口一起滚出，本实现直接隐藏旧窗口，属于已知差异（旧窗口没有做 ghost 绘制）。

## 5. 验证

### 5.1 构建

- MSBuild Release / x64 / v142 构建通过。

### 5.2 实机（2026-10-21 22:02–22:07，1280x720）

同一份部署目录启动新构建，按完整前置链路操作：

1. 登录面板与木牌输入槽对齐；
2. 登录后滚动到世界选择，世界吊牌与「蓝蜗牛」条幅对齐；
3. 选择世界后频道面板滑入，CH.1–CH.3 与「到选择的世界去」可点；
4. 进入选角页，角色/空槽/开始游戏木牌落在桥面与右侧木牌上；
5. 点击「开始游戏」成功进入游戏（Orbis，Welcome to Scania）。

截图：

- [`evidence/chain_defer_world_final.png`](evidence/chain_defer_world_final.png)
- [`evidence/chain_defer_channel_final.png`](evidence/chain_defer_channel_final.png)
- [`evidence/chain_defer_charselect_final.png`](evidence/chain_defer_charselect_final.png)
- [`evidence/chain_defer_ingame.png`](evidence/chain_defer_ingame.png)

### 5.3 观察到的遗留项

- 选角页左侧 `charInfo` 面板中蓝色 “Burning” 文本与「等级」行有轻微重叠，属面板内标签布局问题，待单独核对；
- 选角页角色槽上方的白色竖条与地图瀑布位置重合，暂按背景图元处理，未改动。
