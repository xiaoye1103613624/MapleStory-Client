# 快捷栏「自带功能键图标」拖拽修复（2026-11-22）

## 一、问题与根因

用户报："右下角快捷栏里自带的按键图标无法拖拽，技能/药品可以拖"（明确不是键盘设定窗、也不是侧栏弹层）。

**根因**：右下角快捷栏 `UIStatusBar` 的拖拽载荷构造 `setup_qs_drag_icon()` 只有
`ITEM/SKILL/MACRO` 三支，缺 `MENU/ACTION/FACE`（自带功能键：装备/背包/技能/地图/聊天等）分支
→ 抓起的是空 `Texture`，拖拽预览不可见；同时落点 `send_icon()` 只认
`get_skill_id()/get_macro_index()/get_item_id()`，功能键三者全空 → `return false`，放下也不绑定。
技能(SKILL)/药品(ITEM)两条链路完整，所以只有它们能拖。

## 二、修复（逐条，全部带行号）

以 HEAD 基线行号为准（工作区含前序 HUD 重构，行号已偏移，以函数名为准）：

1. **`IO/Components/Icon.h`** — `Icon::Type` 新增带默认实现的虚方法
   `virtual bool get_binding(uint8_t&, int32_t&) const { (void)...; return false; }`，
   `Icon` 加非虚转发 `bool get_binding(...)`。
   *刻意用带默认实现的虚方法而非纯虚*：装备/背包/技能等所有其它 `Icon::Type`
   子类零改动，穿戴/卸下装备路径不受影响。
2. **`IO/Components/Icon.cpp`** — `Icon::get_binding()` 转发到 `type->get_binding()`。
3. **`IO/UITypes/UIStatusBar.h`** — `QsIconType` override `get_binding()`：
   仅 `bind_type ∈ {MENU, ACTION, FACE, CASH}` 时透出 `(KeyType, KeyAction)`；
   技能/道具/宏/装备仍走各自 getter。`drop_on_equips()` 仍空实现、`drop_on_items()` 仍 `true`，
   装备隔离不变。
4. **`IO/UITypes/UIStatusBar.cpp` `setup_qs_drag_icon()`** — 补
   `MENU/ACTION/FACE` 分支，取 `KeyConfig/icon/<action>`（与 `refresh_modern_qs_icons()` /
   `refresh_classic_qs_icons()` 绘制端同源），`tex.shift(origin)` 抹平底边锚点，
   使抓起的功能键有可见预览、跟手不偏。
5. **`IO/UITypes/UIStatusBar.cpp` `send_icon()`** —
   - 判定链在 skill/macro/item 后补 `icon.get_binding()` 分支识别功能键，
     复用同一套 `kb.assign() + ChangeKeyMapPacket + 去重清旧格 + redraw_quickslots()` 原生换位逻辑；
   - 去重扫描键表由固定 `CLASSIC_QS_KEYS/classic_qs_count` 改为按 `classic_mode` 选择
     modern 用 `MODERN_QS_KEYS/modern_qs_count` —— 否则 modern 格拖动时旧格清不掉。

## 三、与「穿戴/卸下装备」的隔离（用户硬约束）

- 本次只改 4 个文件：`Icon.h/.cpp`、`UIStatusBar.h/.cpp`。
- **装备路径文件零改**，修改时间实证（本会话起点 2026-11-22 23:00）：
  - `IO/UITypes/UIEquipInventory.cpp` 最后修改 20:41、`.h` 16:29
  - `Character/Look/EquipSlot.h` 19:54、`Character/Inventory/Inventory.cpp` 19:55
  - 均为本会话之前的前序改动；本次改动文件时间为 23:12–23:14。
- `QsIconType::drop_on_equips` 仍空、`drop_on_items` 仍恒 `true`，拖到装备窗不做任何事。
- `Icon::Type::get_binding` 默认实现返回 false，装备类 Icon 无新行为。

## 四、验证与产物

- **构建**：VS2019 BuildTools MSBuild，`Release|x64|v142`，EXITCODE=0。
  输出 `build_qs_funckey_drag/MapleStory.exe`，SHA256
  `e438a1caf591846c4f1eecaf498c4a9d405bf9a5d9c55956a8a34340173c1102`。
- 我引入的 `Icon.h C4100` 警告已清理（重建后日志命中数 0）；其余警告均为项目既有。
- 构建脚本 `build_qs_funckey_drag.bat`、日志 `build_qs_funckey_drag.log`。

## 五、取证边界（如实声明）

- 本轮 REA/IDA MCP 工具未暴露（ops_discover_tools 无匹配），未做新的二进制反汇编取证。
  本次实现是**复用同文件已被 IDA 取证并验证可用的 SKILL/ITEM 原生拖拽语义**
  （CHANGE_KEYMAP 换位 + `KeyConfig/icon` 资源），不是新造偏移/签名/协议。
- **真机未验收**：未做客户端实机拖拽交互测试。以上结论来自源码逐条行号 + 构建通过。
  真机复测要点（用本端截图）：功能键格能抓起可见图标、拖到另一格完成换位、
  技能/药品拖拽仍正常、穿戴/卸下装备无回归。
- 产物尚未部署 live（是否部署由用户决定；如部署须实时核对 live exe SHA，
  登录器可能回滚）。
