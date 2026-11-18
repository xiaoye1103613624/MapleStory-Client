# 背包上限×2 + 展开态翻页 · 实现 Plan

> 状态：实现中（2026-11-17）。源：用户 m00257「实现」+ m00267 两项拍板（保留现有布局 / 我一并改服务端）。
> 分支：`feature/dev_0.0.1`。本 Plan 修正 README §4.2 表 B1 的一处设计误区（见 §0）。

## 0. 关键设计修正（相对/绝对槽位分离）

README §4.2 B1 原写「`get_slotpos` full 分支 `absslot = slot + full_page_offset`」——**这是错的，不采用**：
`get_slotpos` 展开态用 `page_dx[4] = {0,149,298,447}`（cpp:1689）只覆盖 96 格几何；
把绝对槽 97..192 传进去会被 `clamp(page,0,3)` 全堆到第 4 页重叠。

正确模型 —— **几何用相对格，数据用绝对格**：

| 概念 | 取值 | 用途 |
| --- | --- | --- |
| 相对格 `rel` | 1..96（每屏固定 96 格几何） | `get_slotpos(rel)` 算屏幕坐标；可见范围 firstrel/lastrel |
| 绝对格 `abs` | `full_page_offset + rel`（服务端槽位 1..192） | `icons` map key、`inventory.get_item_id`、`get_icon`、`modify`、服务端协议 |
| 页 `page` | 1..`page_count`，`page_count=ceil(slotmax/96)` | `full_page_offset=(page-1)*96` |

**`get_slotpos` 函数体一行不改**（格区几何零改动，满足"不改展开态 UI 样式"）。
只在调用侧把"几何参数"传 `rel`、"数据参数"传 `abs`。

## 1. 客户端改动（C++，`E:\project\MapleStory-Client`）

### 1.1 `IO/UITypes/UIItemInventory.h`
- `Buttons` 枚举加 `BT_PAGE_PREV`、`BT_PAGE_NEXT`（放在 `BT_CASHSHOP` 后）。
- 新增成员：`int16_t full_page_offset=0`、`Textfield page_input`、`int16_t page_count() const`、`void apply_page(int16_t n)`、`void change_page(int16_t delta)`、`void sync_page_chrome()`。
- 需 `#include "../Components/Textfield.h"`（Slider.h 已包含，但显式加）。

### 1.2 `IO/UITypes/UIItemInventory.cpp`
- 构造函数：用 MonsterBook `arrowLeft/arrowRight` 建 `BT_PAGE_PREV/NEXT`（`try_maple_btn`，坐标 = cashshop 左侧固定锚点）；`page_input=Textfield(...)`，`set_digits_only(true)`，`set_limit(3)`，`change_text("1")`，`set_enter_callback → apply_page(解析数字)`；`sync_page_chrome()`。
- `draw`（:383-416）：`for rel`，`abs=full_page_offset+rel`，几何用 `get_slotpos(rel)`、图标用 `icons[abs]`；disabled 用 `abs>numslots` 且 `rel<=lastrel`；draw 尾加 `page_input.draw(position)`（仅 full）。
- `slot_by_position`（:1630-1663）：遍历 `rel`，几何 `get_slotpos(rel)`，命中返回 `abs`。
- `is_not_visible`（:1470-1478）：full 分支按 `[offset+1, offset+96]` 且 `<=get_slotmax(tab)` 判绝对格。
- `set_full`（:1753-1811）：进入 full 时 `apply_page(1)`（offset 归零 + 输入框复位）；三件套按钮 `set_active(full_enabled)`。
- `send_scroll`（:1271-1277）：full → `change_page(yoffset>0?-1:1)`（上滚上一页）；折叠 → 原 `slider.send_scroll`。
- `button_pressed`（switch）：加 `BT_PAGE_PREV → change_page(-1)`、`BT_PAGE_NEXT → change_page(+1)`。
- `send_cursor`：full 态让 `page_input.send_cursor` 先接收点击聚焦（在 slot 命中之前）；三件套按钮点击由 `UIElement::send_cursor` 自动分发到 `button_pressed`。
- `remove_cursor`：`page_input.set_state(NORMAL)` 失焦。
- `change_tab`（button_pressed 页签切换处 :677-691）：切页签 `apply_page(1)`（页码归 1，保留整理态，满足 U3）。

### 1.3 `IO/Components/Textfield.h/.cpp`（公共组件，向后兼容开关）
- 加 `set_digits_only(bool)` + 成员 `bool digits_only=false`。
- `add_string`（:335 keep 判定）与 `send_key` TEXT 分支：digits_only 时只保留 '0'..'9'。
- 默认 false，其它调用点零影响。

## 2. 服务端改动（Java，`E:\project\BeiDou-Server_s9\gms-server`）

依据：子代理 srv-probe 全链路取证（见会话 m00345）。

### 2.1 必经改动（byte 溢出 P0）
- `org/gms/client/inventory/Inventory.java:56` `protected byte slotLimit` → `protected short slotLimit`。
- `:74` `getSlotLimit()` 返回 `byte` → `short`；`:83-103` `setSlotLimit` 内 `(byte) newLimit` → `(short)`。
- `PacketCreator.java:534 / :2594` 发包保持单字节：`writeByte((byte) chr...getSlotLimit())`（192→0xC0，客户端按 uint8 读=192，协议不变）。
- `Character.java:8622` `slots <= 96` → `slots <= 192`；`canGainSlots` 相关上限同步。
- `Character.java:599` 新角色 `byte b=24` → `short b`（CASH 仍 96）；`:646-649` 新角色 setSlotLimit 值按需 192。
- DB migration：新增 `Vx__inventory_slots_192.sql`：`UPDATE characters SET equipslots=192,useslots=192,setupslots=192,etcslots=192;` + schema default 24→192。

### 2.2 已发现待核（既有 bug，非本任务引入）
- `Character.java:6971-6974` 写回用 `getSlots(0..3)` 与加载映射错位一格（getSlots(0)=UNDEFINED）。本任务范围外，仅记录，不擅自改（需用户拍板）。

### 2.3 服务端构建（已实测 2026-11-17）
- 构建方式：Maven 3.9.16 + JDK 21（Microsoft），`mvn clean compile -DskipTests`。
- 结果：`BUILD SUCCESS`，`Compiling 1133 source files`，error=0，`SRV_CLEAN_EXITCODE=0`（日志 `gms-server/srv_clean_compile.log`）。
- 注意：首次 `mvn compile` 报 "Nothing to compile - all classes are up to date" 是旧 class 假绿灯；以 `mvn clean compile` 为准才算真编译过 byte→short 改动。
- 仅改 Java 源码，不改 wz。

## 3. 验证闭环（静态部分已实测 2026-11-17）
- 客户端：**全新目录 Rebuild**（`inv_expand_out/`），MSBuild（VS18 Community, MSBuild.exe 直接调用，日志 `inv_expand_build.log` 为 UTF-16LE）。结果：`BUILD_EXITCODE=0`，`MapleStory.vcxproj -> inv_expand_out\MapleStory.exe`，error C / error LNK / fatal error 计数均为 0。
  - 产物：`inv_expand_out/MapleStory.exe`，4,529,152 字节，mtime 2026-11-17T18:09:11+08:00，SHA256=`f558a18de3ec662a1812123c5b3bc020dabfe8a8794abe61c1446461939e368e`。这是**测试构建产物**，非已部署 live exe（部署须另核 live SHA，见红线）。
- 服务端：`mvn clean compile -DskipTests` → BUILD SUCCESS（见 §2.3）；`grep byte slotLimit`=0、`getSlotLimit` 返回 short（已核）；DB 列 INT(11) 可存 192，无需 migration。
- 静态逻辑核对（逐行）：`draw` 几何传 rel(1..96)/数据传 abs=offset+rel（cpp:420-421）；`apply_page` clamp+offset+页码+联动唯一入口（cpp:1337-1350）；`load_icons` 循环到 numslots 覆盖 192（cpp:532）；滚轮方向 `yoffset>0→-1` 与 UICashShop 样板一致（cpp:1326 / UICashShop.cpp:252）；Textfield `digits_only` 过滤（Textfield.cpp:335）。
- **既有 bug（P0，已修 2026-11-17 用户拍板"修P0"）**：`Character.java:6971-6974` 原写回错位（setEquipslots←getSlots(0)=UNDEFINED、setUseslots←getSlots(1)=EQUIP、setSetupslots←getSlots(2)=USE、setEtcslots←getSlots(3)=SETUP），而加载 :6776-6779 用的是 EQUIP/USE/SETUP/ETC=`getType()`=1/2/3/4（见 `InventoryType.java:33-37`）。`gainSlots` 走 `saveCharToDB`（:8632）→ 原 save 用错位的 cdo → ETC 槽重启被写回成 SETUP 的值，破坏 192 扩容持久性。
  - 已改为（4 行，精确映射）：
    - `cdo.setEquipslots((int) chr.getSlots(InventoryType.EQUIP.getType()));`
    - `cdo.setUseslots((int) chr.getSlots(InventoryType.USE.getType()));`
    - `cdo.setSetupslots((int) chr.getSlots(InventoryType.SETUP.getType()));`
    - `cdo.setEtcslots((int) chr.getSlots(InventoryType.ETC.getType()));`
  - 验证：UTF-8 无 BOM 无 fffd、diff 仅这 4 行；`mvn clean compile -DskipTests` → `Compiling 1133 source files` / `BUILD SUCCESS` / 0 error（`srv_clean_compile_p0fix.log`，`SRV_P0FIX_EXITCODE=0`）。
  - 仍需真机验证：4 个背包槽位设为 192 → 重启 → 仍为 192（未真机前"持久性已验证"不成立）。
- 真机（**未做**）：登录看背包上限 192、展开翻页三件套、翻页物品命中、整理/切页签正交（README §7 全表）。可用 `@setslot 192` 测（SetSlotCommand.java:44 `/4*4`、:51 gainSlots）。未真机前一律声明"未验收"。
