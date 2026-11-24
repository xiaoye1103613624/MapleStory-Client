# 窗口内商城（方案 C）— 客户端实现

> 2026-11-21，由 cs-client-impl 实现；用户统一构建/部署，本会话不构建、不部署、不 commit。

## 决策与协议依据

- 路线：方案 C（游戏态内嵌窗口），完整还原（网格 + 购物车 + 搜索 + 焦点灯 + 活体预览）。
- 配置：`ui.cashshop.window_mode` BoolEntry 默认 `false`；true→窗口商城，false/缺配置→原生商城。
- 协议逐字节对照：`E:/MXD/扩展改动/cashshop-window/server/server/cashshop/CashShopWindowPackets.java`、
  参考包 `E:/MXD/扩展改动/cashshop-window/client/cashshopwnd.cpp:73-108,3323-3456`。
- 布局常量逐项取自参考包 `:585-906`（760×495；6×4 网格 86×88；预览井 182×165；购物车 5×4×34；Buy/BtCart 76×18；梯中 x≈146）。
- 预览沙盒逐行复用本端 `IO/UITypes/UICashShop.cpp:1208-1371`（Player + Physics + Ladder + sandbox FHT + cash_item_effect）。

## 改动文件

| 文件 | 内容 |
| --- | --- |
| `Configuration.h` | `WindowCashShopEnabled`（默认 false）、`PosWINDOWCASHSHOP` |
| `Net/OutPacket.h` | `CASHSHOP_WINDOW_ACTION = 14128` |
| `Net/Packets/CashShopWindowPackets.h`（新） | 6 个 action 发包类 |
| `Net/PacketSwitch.cpp` | `CASHSHOP_WINDOW_SYNC = 14129` + handler emplace + opcode 名 |
| `Net/Handlers/WaveAShellHandlers.h/.cpp` | `CashShopWindowSyncHandler::handle`：7 种 resp 解析 + 入队（整包守卫） |
| `Net/Handlers/CashShopWindowSync.h`（新） | 事件结构 + 线程安全队列（enqueue/pump/clear） |
| `IO/UITypes/UIWindowCashShop.h/.cpp`（新） | 主窗口：网格/分类/搜索/购物车/预览/焦点灯/拖拽/ESC |
| `IO/UI.h/.cpp` | `open_cashshop_entry()` helper；`UI::update` 调 `pump()`；ESC 白名单 |
| `IO/UIElement.h` | `Type::WINDOWCASHSHOP` |
| `IO/UITypes/UIStatusBar.cpp`、`UIChatBar.cpp` | 三处入口改调 helper |
| `locale/zh-CN.ini`、`en-US.ini` | `windowcashshop.*` 词条 |
| `MapleStory.vcxproj` / `.filters` | 新文件入工程 |

## 线程模型

- 收包线程：`CashShopWindowSyncHandler::handle` 只解析字节 → `CashShopWindowSync::enqueue`（mutex + queue），
  不 emplace/不操作 UI。
- 主线程：`UI::update()` → `CashShopWindowSync::pump()` swap 队列后转发；OPEN 事件负责 emplace 窗口。
- `PacketSwitch::forward` 已 `read_short` 吃掉 opcode，handler 第一个字节即 resp（与本项目其它扩展 handler 一致；未重复读 opcode）。

## 功能实现程度

| 功能 | 状态 | 说明 |
| --- | --- | --- |
| 配置开关 + 三入口 | 完成 | 默认 false 走原生；true 走窗口 |
| 开窗/关窗/拖拽/ESC | 完成 | UIDragElement + center_if_position_unset + send_key(escape) |
| 商品网格 6×4 | 完成 | 图标/名称/价格；hover tooltip 走 UI::show_item(SHOP) |
| 分类 tab + INDEX/TAXONOMY | 完成 | INDEX 到达清 buckets 重建；按需 REQUEST_CATEGORY；TAXONOMY 落地并点选发 CLICK 包准备 |
| 搜索 | 完成（降级为本地 ASCII 子串） | 无协议，仅本地过滤 name/itemId |
| 购物车 | 完成 | 双击 pin（≤20）、点条目移除、BUY_CART 发包、成功后 retired |
| 焦点灯 | 完成 | Fill/Border 两态都画，缺贴图用 ColorBox 绿/红兜底 |
| 活体预览 | 完成 | 复用本端沙盒（走路/跳跃/爬梯、三背景切换、试穿 add_equip、cash_item_effect） |
| 余额/购买结果 | 完成 | CASH 刷新；BUY/BUY_CART code 全 0..6 映射 i18n |
| 滚动条 | 简化 | 用本端 Slider(DEFAULT_SILVER) 代替 Basic.img/VScr4（同功能、免新增资源） |

## 资源节点

- `UI/CashShop.img/Base/WndBg`（760×495 底板）
- `UI/CashShop.img/Base/BtBuy`、`BtCart`（四态 76×18）
- `UI/CashShop.img/Base/Preview/0..2`（212×165）
- `UI/Basic.img/close_btn`（关闭钮）
- `Item/Cash/<prefix>.img/<id>/effect`（现金特效，照搬 UICashShop）
- 道具图标：`ItemData::get(itemId).get_icon(false)`

## 构建注意

1. 新增 `UIElement::Type::WINDOWCASHSHOP` 改变 EnumMap 尺寸 → **必须整工程 Rebuild**，不能增量。
2. 新增 3 个 .cpp/.h 已写入 vcxproj 与 filters；CMake 用 GLOB_RECURSE 自动包含。
3. locale 根节点 `locale/` 已改；部署时确认打包脚本把 `windowcashshop.*` 带进产物。
4. 未在收包线程 emplace UI；若实测发现 OPEN 后窗口晚一帧出现属正常（下一帧 state->update 接管）。
5. 服务端须已实现 0x3730/0x3731 协议；配置默认 false，不开开关对现网零影响。

## 已知降级/限制

- 搜索为本地 ASCII 子串，不做服务端模糊查询（协议无搜索 action）。
- 分类 tab 名称 INDEX 未下发 name 字段，默认显示 `Tn`；若服务端 TAXONOMY 下发 name 则以 TAXONOMY 为分类源。
- 焦点灯贴图节点 `Base/FocusLight` 若 live 包不存在，走 ColorBox 纯色兜底（功能不缺）。
