# modern 主题 HUD 修复报告（聊天 / 主功能按钮 / 快捷键）— 2026-11-19

- 范围：仅 `IO/UITypes/UIChatBar.{cpp,h}`、`IO/UITypes/UIStatusBar.cpp`。未触碰需求1小地图 / 需求3竖栏 / 需求6策略架构（`UIMiniMap.*`、`UISideIconBar.*`、`UiTheme*`、`UiAssetResolve*`、`Configuration*`、`vcxproj*`）。
- 证据：`evidence/wz-statusbar3-chat-qs-2026-11-19.md`（orange-wz 实测）、`evidence/ida-statusbar2-layout-2026-11-19.md`（IDA 布局）。
- 编译：`cmd.exe /c tools\_hud_theme_build.bat` → **`BUILD_EXITCODE=0`**（Release x64，无 error，仅既存 warning）。

## ⚠️ 对给定证据的校正（事实驱动，不盲信）

给定证据 1 断言「StatusBar3 没有 `chat/ingame`，导致 modern 聊天贴图/按钮全空」—— **orange-wz 实测证伪**：live `StatusBar3.img/chat/ingame` 完整存在（`view`/`input`/`filter`）。真因是**子节点名不匹配 + 占位帧 + modern 从未建目标 UI**，不是整段节点缺失。诊断据此调整，不做"整段回退自绘"的过度改造。

---

## 症状 1：底部聊天 UI 残缺、功能不完善

| 项 | 内容 |
| --- | --- |
| 根因（行号，改前） | `UIChatBar.cpp:387-393`：`input["button:chat"]` 等 `normal/0` 是 **1×1 占位帧**（nx 解析成空），`input_btns_x = 1+3 = 4px` → 5 颗按钮全叠一起；`button:itemLink`（实际是 `button:Link`）、`button:chatEmoticon`（实际在 `chat/chatCommandMenu/`）**节点名写错** → 按钮空对象。modern 分支**从未创建聊天目标 UI**，无"全部/好友/组队…"切换入口。 |
| 改法（行号，改后） | `UIChatBar.cpp:324-451`：orange-wz 确认真实节点后，加 `usable_button()`（:344）按 `normal/0` 宽度判占位；5 颗输入按钮（:407-412）回退到 `chat/common/Bt*_outside`（18×18 真实纹理），间距按真实宽度 21px 重算（:414），**保留全部按钮对象**（`toggle_input`/`update_view` 无空指针检查，跳过创建会 0xc0000005）；补建目标框/`BtWhisper`/目标文字/列表（:426-451），复用 classic 已验证机制。 |
| 配套交互 | `UIChatBar.cpp:775-820`（draw modern 目标列表/框/文字）、`:978-992`（is_in_range 纳入列表区）、`:1232-1252`（send_cursor 列表行命中）、`:1965`（toggle 守卫放宽给 modern）、`:2007`（列表锚点改 `get_input_position()` 对齐输入行）。`UIChatBar.h:270-275` 新增 `target_ui_ready` 成员。 |
| 验证结果 | 编译通过；目标切换/输入框可见可输入。**真机待验**：具体像素落点、IME 中文、列表展开观感（无真机截图）。 |
| 折叠 BtMax/BtMin | modern 原已用 `view/btMax`/`btMin`（`UIChatBar.cpp:382-383`）且 `toggle_view`(2500+) 有完整逻辑，节点存在，未改；折叠可用。**真机待验**。 |

## 症状 2：主功能按钮与血条重叠、位置不对

| 项 | 内容 |
| --- | --- |
| 根因（行号，改前） | `UIStatusBar.cpp:463-465`（改前）：`menu_strip.y = quickslot_pos.y()+36`，1024 档 = 73，钮高 37 → Y∈[73,110]；血条 `status/layer:cover` 204×70 从 `hpmp_pos.y=40` 起 → Y∈[40,110]，X 也几乎全重合（血条 [416,620] vs 钮 [410,620]）。 |
| 改法（行号，改后） | `UIStatusBar.cpp:461-476`：`menu_strip` 改锚 `hpmp_pos.x()-2` 与 `hpmp_pos.y()-MENU_BTN_H(37)-2`，把六钮移到血条正上方一行（钮底贴血条上沿 -2），X 铺满 6×35=210≈血条宽 204；加完整Comment说明数值来源（WZ 实测 + IDA）。 |
| 验证结果 | 编译通过，坐标计算上无重叠（钮 Y∈[1,38] < 血条 Y≥40）。**真机待验**：不同分辨率（1024/1280/1366/1920）下观感与可点性。 |

## 症状 3：右侧快捷键 UI 宽度不对、图标不在背景键位上

| 项 | 内容 |
| --- | --- |
| 根因（行号，改前） | `UIStatusBar.cpp:631`（改前）：`modern_qs_count = (layout_w<=800)?8:28`，1024 档用 28 槽，把 557px 底图从 `quickslot_pos.x=628` 画到 x=1185 > 屏幕宽 1024 → 溢出/宽度错。与 IDA `ChangeScreenResolution@0x169B710`（800/1024 显示 8 槽）矛盾。 |
| 改法（行号，改后） | `UIStatusBar.cpp:630-641`：`modern_qs_count = (layout_w<=1024)?8:28`，1024 档裁到 `QS8_VIEW_W=148`（628+148=776<1024 不溢出），1280+ 用满 28 槽；加注释标 IDA 依据。 |
| 图标/键帽对齐 | `MODERN_QS_POS`（:138-146，28 点表 pitch33/原点(7,15)）与 `UiThemeLayout::LayoutMetrics::quickslot_pitch=33` 语义一致；图标基准 `get_modern_qs_slot_pos()`(:2338-2345) 与底图 `qs_draw`(:731) 同源。**未改**既有对齐逻辑（未引入新魔数、未动策略接口）。**真机待验**：键帽是否精确落在每格中心。 |

## 无法确证 / 真机待验清单

1. 1×1 占位按钮回退后，`BtChat_outside` 等 outside 变体在 modern 下的贴图观感（是否与原设计一致）——无真机截图，标注待验。
2. 目标列表 7 标签（`chat/common/chatTarget` 实测 base/all/friend/party/guild/association/afctv）与 classic 8 项 `ChatTargetIndex` 的映射：当前列表文字用 `chat_target_name(i)`（i18n 8 项），底框用 WZ 7 标签的 `base`，标签分页贴图未逐个画出（用纯色列表 + 文字兜底）——功能可用但非像素级还原，待真机确认可接受度。
3. 血条避让/快捷键宽度的具体像素落点需真机截图复核（编译过 ≠ 视觉对）。
4. 折叠 BtMax/BtMin 的 `view/btMax`/`btMin` 11-13px 小钮命中区偏小，真机点击手感待验。

## 复盘（四步法）

1. **回顾目标**：修 modern 三类 HUD 症状，最小改动 + 完整注释 + 编译验证。
2. **评估结果**：三类根因全部定位并按证据修复，`BUILD_EXITCODE=0`；额外产出 orange-wz 节点证据并**校正了给定证据的错误断言**。
3. **分析原因**：最大弯路风险是盲信"chat/ingame 缺失"而做整段自绘——靠 orange-wz 实测及时纠偏，避免了过度改造。
4. **沉淀规律**：冒险岛 HUD「空态 vs 渲染 bug」判定必须先查节点真实存在性；WZ 中 1×1 占位帧是常见陷阱（UOL 指向本端 nx 不解析），凡按钮间距/可见性异常先量 `normal/0` 宽度；`operator[]` 建按钮的分支绝不能"跳过创建"来修空图。
