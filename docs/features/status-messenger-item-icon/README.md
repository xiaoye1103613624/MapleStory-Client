# 右下拾取飘字显示道具图标

- 状态：已实现（构建部署通过，实机视觉验证预留）
- 日期：2026-11-16
- 关联仓库：MapleStory-Client（SDL 冒险岛客户端）

## 1. 需求与结论

需求：右下角拾取提示在拾取道具时增加道具图标展示，图标大小与字体差不多；不可行则放弃。

结论：**可行，性能影响可忽略**，已实现。图标缩放到约 **21px**（初版 14px，2026-11-16 按用户实机反馈放大 1.5 倍），垂直与文字墨迹中心对齐，仅在「道具获得/丢失」飘字左侧显示；金币、经验、任务、系统提示等其他消息不变。

## 2. 可行性依据（只读源码取证，见 notes/feasibility.md）

- 右下拾取提示组件是 `UIStatusMessenger`（非 `UIStatusBar`）：`A12M` 12px 右对齐文字，8 秒淡出，队列上限 6 条。
- 拾取消息走 `SHOW_STATUS_INFO(39)` → `ShowStatusInfoHandler::handle`：`mode==0/mode2==0` 分支**已读到 `itemid`/`qty`**，只是原来只把格式化后的字符串传给飘字，`itemid` 丢弃。
- 图标管线现成：`ItemData::get(id).get_icon(false)` 返回 `const Texture&`，底层 `Template/Cache` 进程级懒加载缓存；`Texture::draw(DrawArgument(pos, xscale, yscale, opacity))` 原生支持缩放。
- 内置 `#i<id>#` 行内图标按原生 32px 绘制，会把 14px 行高撑大，不符合「和字体差不多」，故**不用**行内机制，改为手动缩放绘制。

## 3. 实现逻辑

数据流（单向，不新增模块）：

```
收包 SHOW_STATUS_INFO(39)
  → ShowStatusInfoHandler::handle（道具分支读 itemid/qty）
  → 辅助 show_status(color, message, itemid) 透传
  → UIStatusMessenger::show_status 构造 StatusInfo(含 itemid) 入队
  → 每帧 draw() 按 itemid 取缓存图标，缩放画在右对齐文字左侧
```

改动文件：

- `IO/UITypes/UIStatusMessenger.h/.cpp`
  - `StatusInfo` 增 `int32_t itemid`（默认 0）；`show_status` 增 `int32_t itemid=0`。
  - `draw()`：`itemid>0` 时取 `ItemData::get(id).get_icon(false)`，按 `scale = ICON_PX / icon.height()` 缩放（`ICON_PX=21`）；图标视觉矩形贴在文字左缘左侧（`position.x() - text.width() - 3 - iconw`），垂直位置 `icon_top = position.y() + INK_CENTER(9) - ICON_PX/2` 与文字墨迹中心对齐（消除初版偏上约 2px），用 `get_origin()` 补回 origin 偏移，透明度用 `opacity.get(alpha)` 与文字同步淡出。
  - 无效 id / 图标纹理不可用时跳过图标，文字照常；不改变淡出、锚点、队列上限。
- `Net/Handlers/MessagingHandlers.h/.cpp`
  - `ShowStatusInfoHandler::show_status` 增 `int32_t itemid=0` 并透传。
  - 仅 `mode==0/mode2==0` 的 `status.item.gained_*/lost_*` 分支传入真实 `itemid`；金币（mode2==1）、经验、任务、其他模式不传（默认 0，无图标）。

兼容性：19 处 `show_status` 调用点（2 参/3 参）全部兼容，无重载冲突、无取地址调用被破坏（code-explorer 全量核对）。

## 4. 性能评估

- 事件驱动入队：每次拾取一次 `push_front`，8 秒淡出，上限 6 条，非每帧构造。
- 每帧增量：最多 6 条 × 1 次缩放 blit（纹理来自缓存，无 NX 解析）。
- 纹理复用：`Cache<ItemData>` 进程级懒加载，同一道具重复拾取零成本；首次拾取某道具才有一次 NX 解析。
- 已有抑制：背包/装备窗打开时不弹飘字（`MessagingHandlers.cpp` 的 `inv_open` 判断），大批量拾取不刷屏。
- 结论：整体影响可忽略，无新增纹理、无构建期资源改动。

## 5. 验证记录

- 构建：taskkill 后 MSBuild Release/x64（v142，OutDir=E:\MXD\BeiDou-Client-xy）`EXITCODE=0`，仅既有 `glew32s.lib LNK4099`（缺 vc120.pdb）warning；部署 exe 刷新至 `2026/11/16 22:21:21`。
- 静态核对：`show_status` 调用点、图标绘制 API 签名、头文件包含链、窄化转换均安全（无 lint 诊断）。
- 微调记录（2026-11-16）：依据截图 `XiaoYe_2026-11-16_22-40-47.png` 反馈「图标偏上、放大 1.5 倍」，`ICON_PX` 14→21，新增 `INK_CENTER=9` 并将 `icon_top` 由 `position.y()` 改为与文字墨迹中心对齐；其余逻辑不变。
- 实机视觉验证：**预留**——需服务端 8484 握手 + 用户在屏前实际拾取道具，确认放大后尺寸与垂直居中观感；当前无服务端可跑，不擅自启动服务端/操作 DB。若观感仍需微调，仅调整 `INK_CENTER`（垂直）/ `ICON_PX`（尺寸）两个常量即可。

## 6. 回滚

- 源码改动集中在 3 个文件（UIStatusMessenger.h/.cpp、MessagingHandlers.h/.cpp），回滚即撤销这些签名扩展与 draw 图标段。
- 无 WZ/IMG/XML、无协议/opcode、无配置改动。
