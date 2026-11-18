# 可行性取证摘要（只读源码，2026-11-16）

## 组件定位

- 右下拾取提示 = `IO/UITypes/UIStatusMessenger`（非 `UIStatusBar`）。
  - `UIStatusMessenger.h:28-43` `StatusInfo{Text text; Text shadow; Linear<float> opacity}`；`:65-67` `deque<StatusInfo>`，`MAX_MESSAGES=6`，FADE_DURATION=8s。
  - `UIStatusMessenger.cpp:28-34` 字体 `A12M`（12px）、`Alignment::RIGHT`；`:36-42` draw 阴影(1,1)+文字；`:63-72` 每条 `shift_y(-14)`；`:127-133` show_status=push_front+pop_back。
  - 锚点 `:52-61,80-88` 右下 Edge，宽屏 +37px 上抬。

## 消息链路

```
SHOW_STATUS_INFO(39)  [Net/PacketSwitch.cpp:90,749]
  → ShowStatusInfoHandler::handle  [MessagingHandlers.cpp:72-130]
      mode0/mode2=0: read itemid(97)/qty(98) → ItemData::get(itemid)(100) → 名称(107)
      → status.item.gained_one/many / lost_one/many(122-130) → show_status(...)
  → 辅助 show_status  [MessagingHandlers.cpp:373-380] → UIStatusMessenger::show_status
  → 每帧 draw  [UI.cpp:59-61 / UIStateGame.cpp:119-141 / UIStatusMessenger.cpp:63-72]
```

- 关键：原 `show_status` 签名只有 `(color, message)`，`itemid` 读完即丢；加图标需扩参。

## 图标管线

- `Data/ItemData.h:28` `Cache<ItemData>`；`:58` `const Texture& get_icon(bool raw) const`；`:80` `BoolPair<Texture> icons`。
- `ItemData.cpp:73-74` `icons[false]=src["icon"]`、`icons[true]=src["iconRaw"]`；`:269-272` get_icon 返回引用。
- `Template/Cache.h:36-44` `get(id)` 缺失时构造 `T{id}` 并缓存 → 懒加载 + 进程级复用。
- `Texture.h:42-52` `draw(DrawArgument)` / `width/height/get_origin`；`DrawArgument.h:42` `(Point,float xscale,float yscale,float opacity)` 精确重载；`Icon.cpp:64-73` 缩放 + origin 对齐范例。
- `get_rectangle`（DrawArgument.h:122-147）视觉左上角 = `pos - scale*origin`，故绘制传 `pos = 视觉左上角 + scale*origin`。

## 字体尺寸

- `GraphicsGL.cpp:225-229` A11M=11/A12M=12/A13M=13；本消息 A12M=12px，行距 14px → 图标目标 14px，`scale ≈ 14/icon.height()`（道具 icon 常 31-32px → ≈0.44）。

## 为何不用内置行内图标

- `Text` 默认 `formatted=true`，`#i<itemid>#` 由 `GraphicsGL.cpp:1089-1097,1200-1210,1527-1560` 解析并调 `ItemData::get(id).get_icon(false)`，但按原生尺寸绘制、行高 `max(h,16)`（`:1328-1333`）→ 32px 图标会把 14px 行撑大，不满足「和字体差不多」，故手动缩放。

## 性能论证

- 事件驱动入队（每次拾取 1 次 push_front，8s 淡出，上限 6 条），非每帧构造。
- 每帧最多 6 次缩放 blit；纹理进程级缓存，无新增 NX 解析/纹理/常驻内存。
- 背包/装备打开抑制已在位（`MessagingHandlers.cpp:79-84,114-116`），不刷屏。
- 结论：影响可忽略。

## 兼容性核对（code-explorer，全量）

- `UIStatusMessenger::show_status` 10 处调用、`ShowStatusInfoHandler::show_status` 9 处调用，2/3 参均由默认参数兼容；无重载、无取地址/绑定调用。
- 无其他同名 `show_status` 定义；`UIStateGame.cpp` 无调用不受影响。
- 图标绘制 API 签名、包含链（Text.h 间接包含 Texture/DrawArgument）、窄化转换均安全；lint 0 诊断。
