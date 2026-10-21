# R04 · 登录 6 步滚动补间（CLogin::ChangeStep）

> 日期：2026-10-21  
> 依据：`BeiDou.exe` IDA，`CLogin::ChangeStep @0x5F53C0`、`CLogin::GetStepHeight @0x5FC0E4`、`CLogin::OnStepChanged @0x5F5962`

## 1. 参考端行为

| 项 | 证据 | 结论 |
| --- | --- | --- |
| step 数 | `ChangeStep` 的 `this+90` 分支 | 0 登录 / 1 选世界 / 2 频道·角色 / 3 新建角色 / 4 角色预览 / 5 其他 |
| 高度 | `GetStepHeight`: `return -8 - 600 * (v2 + a2)` | `step_height(step) = -8 - 600*step` |
| step 5 | `ChangeStep`：`if (v67 == 5) a2 = GetStepHeight(1)` | step 5 使用 step 1 的高度 |
| 时长 | `v65 = 300 * abs(v67 - v2) + 500` | `300*|new-old| + 500` ms |
| 补间 | `sub_43E7C7` 向 `IWzGr2D` 中心写入补间命令 | 相机在旧/新高度之间平滑滚动 |
| 音效 | `play_ui_sound(SP_1236_SCROLLUP)` | 每次切步播放 `SCROLLUP` |
| 窗口切换 | `sub_5F4C16` 在 `now - *(this+364) > 0` 时才调用 `OnStepChanged` | **滚动结束后**才销毁/创建窗口 |

## 2. 当前端缺口

`LoginBackground::get_viewpos()` 之前直接返回 `center - (28, step_height(step))`，
step 变化时相机瞬间跳变；`set_step()` 也只是赋值，没有补间。

## 3. 本次实现

`IO/LoginBackground.{h,cpp}`：

- `set_step(value)`：记录当前插值位置为起点，按 `300*|new-old|+500` ms 启动补间。
- `target_offset(step)`：step 0 保留既有登录页锚点 `(137, -20)`；其余为 `(28, -8-600*step)`；step 5 映射到 step 1。
- `current_offset()`：按 elapsed/duration 做平滑 S 曲线（`t²(3-2t)`），两端速度为 0。
- `get_viewpos()`：用插值后的偏移计算屏幕位置，所有登录 UI 继续通过 `viewpos` 锚定，自动跟随滚动。
- `update()`：补间结束后清除 active 标记。

## 4. 尚未对齐

1. **窗口切换时机**：参考端在滚动结束后才 `OnStepChanged`；当前端在 step 变化时立即 `emplace` 新窗口。视觉上新窗口会随相机滑入，而参考端是旧窗口滚出后新窗口出现在终点。需要 UI 层延迟切换才能完全一致。
2. **step 0 相机 X**：参考端 `Init` 设置 `IWzGr2D` 中心 `(28, GetStepHeight(step))`；当前端登录页沿用 `(137, -20)` 锚点以对齐书本开口。二者是否存在系统性 X 偏移，需实机叠图确认。
3. **step 4 高度**：`GetStepHeight` 在 step==4 时使用 `this+133` 的额外偏移；当前端未使用 step 4。

## 5. 验证

- 构建：MSBuild Release / x64 / v142，`EXITCODE=0`；产物 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`。
- 实机：登录 → 选世界 → 选频道/角色 的滚动时长与方向待点测截图确认。
