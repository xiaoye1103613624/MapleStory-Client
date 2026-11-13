# 游戏态出界钳制约定（Wave 0）

## 策略

| LayoutPolicy | 行为 |
|---|---|
| `Center` | `center_on_screen` → **再** `clamp_to_view` |
| `Edge` | 贴边公式 → **再** `clamp_to_view`（非贴边轴也被夹回） |
| `SavedPosClamped` | 直接 `clamp_to_view` |
| `HudGroup` | x=`hud_center_x`；y 由 StatusBar/ChatBar 自管 |
| `None` | 登录固定 / 子类自管（登录链不随游戏档缩放） |

实现：`IO/UIElement.cpp` `UIElement::update_screen`。

## 开窗

可拖/弹窗 ctor 设 `layout_policy`；`makeactive` / 拖曳结束 / `update_screen` 后位置必须在客户区内。

辅助：`Util/HudLayout.h` → `clamp_to_view` / `center_on_screen` / `edge_*`。
