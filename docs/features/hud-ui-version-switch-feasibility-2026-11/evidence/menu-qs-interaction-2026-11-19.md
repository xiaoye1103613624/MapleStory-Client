# 取证：现代底栏主功能菜单 / 快捷键交互实现清单与图标偏移根因（2026-11-19）

- **性质**：只读取证，未改任何代码/资源。
- **源码**：`IO/UITypes/UIStatusBar.cpp`（3309 行，revision `8efa1e49f…`）、`IO/UITypes/UIStatusBar.h`。
- **用户反馈对照**：图2（正确端）主功能菜单在血条右侧一排彩色按钮、点击出列表；快捷键默认 2 行×10 列、左侧展开/收缩钮、键帽图标定位在背景格上。本端图1：菜单在血条下方、快捷键空灰格无图标、图标偏移。
- **取证工具链**：源码逐行 + orange-wz MCP（`127.0.0.1:10012`，`tools/_mcp_call.py`，128B key `TSPHKw==`）+ IDA MCP（`127.0.0.1:13339`，`tools/_mcp_call.py` / `_ida_decomp_batch.py`，IDB `MapleStoryT.i64` imagebase 0x400000）。
- **注意**：本端 HUD 走 `StatusBar3.img`（modern 主题，见 `Util/UiAssetResolve.cpp`、`Util/UiThemeLayout.cpp:78`）；IDA 13339 的参照端（"192 客户端"）走 `StatusBar2.img`。**keycap `KeyConfig/key` 节点本端 live 有、IDA 参照端主 HUD 用 `UIWindow2`**，两者不能混为一谈（见 §3.4 口径）。

---

## 1. 主功能菜单：6 个主钮点击是否出列表 + 子功能是否真 OpenUI

### 1.1 六钮绘制与命中（已实现，均带行号）

| 环节 | 位置 | 结论 |
|---|---|---|
| 六钮创建 | `UIStatusBar.cpp:474-489` | `place_menu()` 用显式 TL + `MENU_PITCH=35` + `MENU_BTN_H=37` 横排 `button:CashShop / Menu / Setting / Character / Community / Event`，`try_maple_btn` 成功即 `set_active(true)`。源码注释 `:461` 记录 2026-11-19 已修"压血条"重叠。 |
| 后画避遮挡 | `:766-772` | 六钮在 `draw()` 中于 quickSlot 底图之后绘制，避免被 `quickSlot backgrnd/cover` 盖住（800 档尤甚）。 |
| 命中区 | `:1686-1699`（`is_in_range`）、`:2360-2385`（`cursor_on_interactive`）、`UIStateGame.cpp`（interactive 优先 STATUSBAR） | modern 下底栏全宽可点；六钮走 `buttons[].bounds(position)` 命中；`cursor_on_interactive` 保证不被 CHATBAR 宽命中区盖住。 |
| 点击分发 | `:921-1104`（`button_pressed`） | 六钮各有 case（见下），列表弹出走 `toggle_*()`，非空壳。 |

### 1.2 六个主钮点击 → 列表（已实现）

| 主钮 | 点击 case | 列表开关函数 | 列表绘制 | 结论 |
|---|---|---|---|---|
| BT_CASHSHOP（商城） | `:933-937` | — | 直接 `EnterCashShopPacket().dispatch()` 进商城（独立 opcode，非列表） | **已实现**（商城无子菜单，直接进） |
| BT_MENU（菜单） | `:938-965` | `toggle_menu()` `:1885-1903` | `:806-811` + `:838-839`（11 钮列表 + `menubackground[3]` + `menutitle[3]`） | **已实现**（出列表） |
| BT_OPTIONS（设置） | `:1075-1079` | `toggle_setting()` `:1905-1921` | `:812-817` + `:838-839`（5 钮） | **已实现**（出列表） |
| BT_CHARACTER（角色） | `:1080-1084` | `toggle_character()` `:1941-1957` | `:788-793` + `:838-839`（5 钮） | **已实现**（出列表） |
| BT_COMMUNITY（社区/好友） | `:1085-1089` | `toggle_community()` `:1923-1939` | `:794-799` + `:838-839`（4 钮） | **已实现**（出列表） |
| BT_EVENT（活动） | `:1090-1094` | `toggle_event()` `:1959-1974` | `:800-805` + `:838-839`（2 钮） | **已实现**（出列表） |

列表绘制主体 `UIStatusBar.cpp:774-840`：三段背景 `menubackground[0..2]`（`:550-552` 取自 `submenu["backgrnd"]["0/1/2"]`）+ 标题 `menutitle[0..4]`（`:589-593`）+ 11 个子钮循环绘制（`:838-839`）。列表高度按 `button_count` 计算（`:825-834`）。互斥/收起见 `remove_menus()` `:1976-1988`、`remove_active_menu()` `:1990-2008`。键盘上下/回车导航在 `send_key` `:1393-1485`。**结论：6 主钮的"点击出列表"代码全部实现，不是空壳；商城走独立进商城包，其余 5 个确有列表。**

### 1.3 子功能逐条：是否真 OpenUI（带行号）

判定标准：case 内有 `UI::get().emplace<UIXxx>()` / `EnterMtsPacket().dispatch()` / `get_element+makeactive/change_tab` = **已实现**；case 只有空 `break` 或根本无 case = **空壳**。

#### "菜单"列表（11 项，`:554-564`）

| 子功能钮 | 枚举 | 点击 case | 实际动作 | 实现 |
|---|---|---|---|---|
| 任务 | BT_MENU_QUEST | `:1105-1113` | `emplace<UIQuestLog>` | **已实现** |
| 怪物收集 | BT_MENU_MONSTER_COLLECTION | `:1114-1119` | `emplace<UIMonsterBook>` | **已实现** |
| 领取/赠送 | BT_MENU_CLAIM | `:1120-1134` | `UIClaim` + `UIDelivery` | **已实现** |
| 奖牌/每日礼 | BT_MENU_MEDAL | `:1135-1140` | `emplace<UIDailyGift>` | **已实现** |
| 成就 | BT_MENU_ACHIEVEMENT | `:1141-1148` | `UIQuestAlarm`（get+makeactive） | **已实现** |
| 联盟 | BT_MENU_UNION | `:1149-1162` | `UIUserList(Tab::UNION)`（共用窗，无独立 UIGuild） | **已实现** |
| 拍卖 | BT_MENU_AUCTION | `:1163-1171` | `EnterMtsPacket().dispatch()`（服务端 use_mts 决定 UI/脚本对话） | **已实现**（协议通道） |
| 怪物生活 | BT_MENU_MONSTER_LIFE | `:1172-1177` | `emplace<UIMonsterBook>` | **已实现** |
| 战斗统计 | BT_MENU_BATTLE | `:1178-1187` | `UIMonsterCarnival`（get+makeactive） | **已实现**（注意：实为怪物嘉年华，非通用战斗统计面板） |
| 钓鱼/种植 | BT_MENU_FISHING | `:1188-1197` | `UIPlant`（get+makeactive） | **已实现** |
| 帮助 | BT_MENU_HELP | `:1198-1204` | `EnterMtsPacket().dispatch()`（同拍卖通道） | **已实现**（协议通道） |

#### "设置"列表（5 项，`:566-570`）

| 子功能 | case | 动作 | 实现 |
|---|---|---|---|
| 频道 BT_SETTING_CHANNEL | `:1205-1211` | `emplace<UIChannel>` | **已实现** |
| 选项 BT_SETTING_OPTION | `:1212-1218` | `emplace<UIOptionMenu>` | **已实现** |
| 键位 BT_SETTING_KEYS | `:1219-1228` | `emplace<UIKeyConfig>` | **已实现** |
| 手柄 BT_SETTING_JOYPAD | `:1229-1235` | `emplace<UIJoypad>` | **已实现** |
| 退出 BT_SETTING_QUIT | `:1236-1242` | `emplace<UIQuit>` | **已实现** |

#### "社区"列表（4 项，`:572-575`）

| 子功能 | case | 动作 | 实现 |
|---|---|---|---|
| 好友 BT_COMMUNITY_FRIENDS | `:1243-1276`（与 PARTY 共用） | `UIUserList(Tab::FRIEND)`，含 toggle/change_tab | **已实现** |
| 组队/BOSS 招募 BT_COMMUNITY_PARTY | `:1243-1276` | `UIUserList(Tab::PARTY)` | **已实现** |
| 工会 BT_COMMUNITY_GUILD | `:1277-1301` | `UIUserList(Tab::GUILD)`（共用窗，无独立 UIGuild 浮窗） | **已实现** |
| 楓之谷聊天 BT_COMMUNITY_MAPLECHAT | `:1302-1322` | `UIMessenger` 或回退 `UIChat` | **已实现** |

#### "角色"列表（5 项，`:577-581`）

| 子功能 | case | 动作 | 实现 |
|---|---|---|---|
| 角色信息 BT_CHARACTER_INFO | `:1323-1331` | `emplace<UICharInfo>` | **已实现** |
| 能力 BT_CHARACTER_STAT | `:1332-1340` | `emplace<UIStatsInfo>` | **已实现** |
| 技能 BT_CHARACTER_SKILL | `:1341-1350` | `emplace<UISkillBook>` | **已实现** |
| 装备 BT_CHARACTER_EQUIP | `:1351-1359` | `emplace<UIEquipInventory>` | **已实现** |
| 道具 BT_CHARACTER_ITEM | `:1360-1368` | `emplace<UIItemInventory>` | **已实现** |

#### "活动"列表（2 项，`:583-584`）

| 子功能 | case | 动作 | 实现 |
|---|---|---|---|
| 日程 BT_EVENT_SCHEDULE | `:1369-1380` | `UIEvent` + `UIEventWindow` | **已实现** |
| 每日礼 BT_EVENT_DAILY | `:1381-1387` | `emplace<UIDailyGift>` | **已实现** |

**清单总论**：27 个子功能 case **全部有真实 OpenUI 或协议发包动作，无空壳 case**。但有两处语义偏差需注意（非空壳，是映射取舍）：
1. `BT_MENU_BATTLE`（"战斗统计"）实际打开 `UIMonsterCarnival`（怪物嘉年华），不是通用战斗统计面板（`:1178-1187`）。
2. `BT_MENU_AUCTION` / `BT_MENU_HELP` 都走 `EnterMtsPacket()`，最终 UI 由服务端 `use_mts` 决定（`:1163-1171`、`:1198-1204`）；客户端侧不自绘拍卖行。
3. `BT_MENU_MONSTER_COLLECTION` 与 `BT_MENU_MONSTER_LIFE` 都开同一个 `UIMonsterBook`（`:1116` / `:1174`）。
4. 每个子功能 case 末尾均 `remove_menus()`（`:1111` 起）收起列表，行为闭环。

---

## 2. 快捷键：布局 / 展开收缩 / 图标偏移根因

### 2.1 默认布局：不是 2×10，是 4 组 + 末组 4 格 = 28 槽（源码），参照端原生 8/28 随分辨率

- **本端 modern**：`MODERN_QS_SLOTS = 28`（`UIStatusBar.h:267`）；`MODERN_QS_POS[28]`（`UIStatusBar.cpp:138-146`）明确写死坐标：槽格 32×32，4 组每组 4 列×2 行（x: 7/40/73/106 → 150/183/216/249 → 282/315/348/381 → 414/447），**末组仅 4 格**（idx24-27 = {414,15},{447,15},{414,48},{447,48}），即 4×(4×2)+4 = 28。源码注释 `:136-137` 标明取自参照端 `s_ptShortKeyPos@0x279c390`。
- **默认数量**：`modern_qs_count = (layout_w <= 1024) ? 8 : MODERN_QS_SLOTS`（`:643`）。即 800/1024 档 8 槽，1280/1366/1920 档 28 槽。**不是 2 行×10 列**。
- **关于用户反馈"默认 2 行×10 列"**：本端代码无 2×10 布局（无 `UIActionBar.img`，live `Data/UI/` 下也不存在该文件；`UIActionBar.img` 是更新版本客户端的 ActionBar 资源，见 `wz_qs_keycap.out` 搜索命中，仅 192 对照端 UI_002.wz 内有 `ActionBar/0/0/keyName`）。classic 布局是 **13 列×2 行 = 26 槽**（`CLASSIC_QS_KEYS[26]`，`:126-134`；几何 `QS_PITCH_X=35/QS_PITCH_Y=34`，`:110-112`）。图2 的"2 行×10 列"若是真机观感，更可能是 28 槽 mapping 在宽屏下视觉分组，或来自另一套 ActionBar 资源；**当前源码/资源证据不支持"2×10"作为本端实现**，需向用户确认图2 客户端版本/主题。这是取证发现的**待用户确认项**（不臆造）。

### 2.2 展开/收缩：有按钮、有动画式位移，但**不改变槽位布局**

- 按钮：`BT_FOLD_QS`（收起）、`BT_EXTEND_QS`（展开）在 `:491-539` 按分辨率创建（`button:Fold/Extend` + 800 档的 `...800` 变体），初始 active 态 `:541-547`、`:673-677`。
- 点击：`case BT_FOLD_QS → toggle_qs(false)`（`:1095-1099`）、`case BT_EXTEND_QS → toggle_qs(true)`（`:1100-1104`）。`toggle_qs(bool)`（`:1773-1806`）只改 `quickslot_active` 标志 + 切换两个按钮的 active 态；**不改 `modern_qs_count`、不改 `MODERN_QS_POS`、不改槽格坐标**。
- 实际视觉变化来自 `quickslot_adj` X 位移动画：`update()` `:862-887` 按 `quickslot_active` 在 `QUICKSLOT_MAX` 与 `quickslot_min` 间平移；`get_quickslot_pos()` `:2033-2044` 在 active 时返回 Y 偏移（800 档 -73、其它 -31），影响子菜单 Y 定位。底图/图标整体随 `quickslot_adj` 移动（`:743-745`、`modern_qs_cell_at` `:2348-2358` 的 base 含 `quickslot_adj`）。
- **对照 IDA**：参照端 `CQuickSlot::Toggle@0x16972a0`（`evidence/_raw_quickslot_toggle.c`）：翻转 `m_bShowSlide` 并用 `IWzVector2D::RelMove(nX=m_nInitX, nY=m_nInitY+(m_bShowSlide?0:67))` 把 slide 层整体移动 —— **收起时下移 67px**。`Init@0x16A80F0` 同样用该 RelMove（`:184-194`）。即：原生展开/收缩也是"整层平移 67px"，**不改变槽位相对布局**，与本端 `quickslot_adj` 方案语义一致。**结论：展开/收缩实现存在且语义正确（整层滑动），只是本端用 X/Y 双位移、参照端用 Y=67 单位移，数值口径不同；不改布局这点两端一致。**

### 2.3 槽位/展开的 IDA 原生证据（地址+行号）

| 结论 | 证据 |
|---|---|
| 槽坐标取自内存表、不是硬编码 | `GetPosByIndex@0x16968f0`（`_raw_quickslot_getposbyindex.c:3-7`）：`if(nIdx < m_nQuickSlotDisplayNo){ *x=s_ptShortKeyPos[nIdx].x; *y=s_ptShortKeyPos[nIdx].y; }` |
| 表内容：原点(7,15)、pitch 33、第二行 y=48、idx8+ x从150起 | `ida-statusbar2-layout-2026-11-19.md:171-182`（表 @`0x279C390`，IDA `get_wide_dword` 实读；`s_ptShortKeyPos_0@0x27C5978` 全 -1 是哨兵，勿用）。与本端 `MODERN_QS_POS`（`:138-146`）逐项一致。 |
| 分辨率决定槽数 | `CQuickSlot::ChangeScreenResolution@0x169b710`（`_raw_quickslot_changescreenresolution.c:242-251`）：`if(w==800&&h==600 || w==1024&&h==768) v21=8; else v21=28; m_nQuickSlotDisplayNo=v21;` —— 本端 `:643` 的 `<=1024→8, 其它→28` 与此完全吻合。 |
| 展开/收缩 = slide 层 RelMove 67px | `Toggle@0x16972a0`（`_raw_quickslot_toggle.c:14-17,36-42`）与 `ida-statusbar2-layout-2026-11-19.md:184-194`。 |
| 绘制用槽坐标 + 图标 origin 修正 | `CQuickSlot::Draw@0x16bbe20`（本次现场 decompile，spill 96441 字符）：`:549-551` 读 `s_ptShortKeyPos[i].x/.y`；`:780-785` 等处 `IWzCanvas::Copy(nDstLeft = x - originX, nDstTop = y - originY + 32)`，`:883-888`、`:1249-1254` 同模式（`- Getcx/Getcy + 32`）。即原生图标落点 = 槽左上 + (0,32) 再减图标自身 origin。 |

### 2.4 键帽纹理来源（已核实）

- 源码 `UIStatusBar.cpp:645-660`：`classic_qs_cap[id] = Texture(qskey[std::to_string(id)])`，`qskey = UiAssetResolve::status_bar_root()["KeyConfig"]["key"]`（即本端 `StatusBar3.img/KeyConfig/key`），回退 `UIWindow.img` / `UIWindow2.img` 的 `KeyConfig/quickslotConfig/key`；id 1..88。
- **本次 live orange-wz 核实**（`StatusBar3.img`，GMS key `TSPHKw==`，128B key，`exclusive:false`，`unload_all` 收口 rootCount=0）：
  - `KeyConfig/key`：**70 个 canvas 节点**，id 集 = 1,2..52,56,57,59..68,70,71,73,79,81,82,83,87,88；尺寸多为宽 7~34 × **高 9**（少数 6×5/7×11/22×11/34×11 等异形）；**全部 origin=(0,0)**。→ 是顶带小键帽条，与源码注释 `:2495`（"顶带小条约 28×9"）一致，源码画法 `CAP_TOP(1,0)` + `cell_pos`（`:2497-2501`）把它贴格顶，**正确**。
  - `KeyConfig/icon`：**69 个 canvas 节点**，id 0..55（缺 24/28/32/36/38/55 部分）含 100..106 / 200..206 / 300；尺寸 **32×32**（node 300 是 1×1 占位）；**全部 origin=(0,32)**。→ 槽图标底锚点在贴图下方 32px，正是源码 `:2472-2478` `icon_dy = get_origin().y(); draw(cell_pos + (0, icon_dy))` 与常量 `QS_ICON_SHIFT_Y=32`（`:115-116`）处理的对象。**键帽来源正确，图标 origin 处理逻辑本身也有对应实现。**

### 2.5 图标偏移 / 空灰格根因（核心结论）

按证据链逐条定性：

1. **空灰格、无图标（用户在图1看到）** —— 主要是 8 槽模式裁宽/`quickslot_active` 与绑定数据问题，非"图标没画"：
   - `draw_modern_quickslot()`（`:2460-2463`）开头 `if (!quickslot_active) return;` —— 折叠态整个图标层不画，只剩 `quickslot[0/1]` 底图（`:759-760` 画 557×67 / 562×73）。若 `BT_FOLD_QS` 状态与 active 不一致（参见 `:541-547`、`:673-677`、`:1794-1797` 三处 active 切换），会出现空底图。
   - `refresh_modern_qs_icons()`（`:2387-2458`）只在前 8 槽读 `keyboard.get_quickslot_key(i)`（`:2409-2411`），第 8 槽以后用固定 `MODERN_QS_KEYS[i]`；`maple_key==0` 时回退（`:2412-2413`）。若玩家这些固定键（NUM1-8/QWER/ASDF/ZXCV 等）**未绑定技能/道具且无对应 `KeyConfig/icon` 节点**（如某些 action id 在 69 个节点里不存在），`classic_qs_icons[i]` 保持空 `Texture()`（`:2402`、`:2455` 仅在 n 存在时赋值）→ **格子空**。这就是"空灰格"的直接机制：图标完全依赖 keyboard binding，绑定表空或 action 无 icon 节点则不画。
   - `:911-918` 每秒才刷新一次图标；绑定刚变化到下次刷新前仍是旧/空图标（延迟观感）。
2. **图标偏移（用户在图1看到）** —— origin 对齐有两处隐患，均可致"键帽/图标相对背景格偏上/偏左"：
   - **底板 origin 未统一**：`backgrnd` origin=(-12,-3)、`layer:cover` origin=(-10,0)（本次 live 实测）。源码 `:453-456` 对两者各自 `shift(get_origin())` 后再以**同一** `qs_draw` 绘制（`:743-760`）——两个不同 origin 被分别抹掉后画到同一点，**底图之间的相对偏移被抹平但整体相对 `MODERN_QS_POS` 槽坐标仍存在**：`MODERN_QS_POS` 来自参照端 `s_ptShortKeyPos`（原点 7,15 是相对某个父层原点），而本端 `qs_draw` 原点是 `position+quickslot_pos+quickslot_adj`，两者父层锚点未必同一 → 图标按 `MODERN_QS_POS` 定位、底图按 `shift(origin)` 后同点定位，**若父层锚点差≠0 就偏移**（`modern-menu-qs-fix-2026-11-18.md` 已记录同类问题："底板 backgrnd origin=(-12,-3) 未 RemoveOrigin 时槽相对 s_ptShortKeyPos 偏左上"）。
   - **图标 dy 符号/基准**：源码 `:2476-2477` 用 `icon_dy = get_origin().y()`（对 `KeyConfig/icon` 是 **+32**）并把图标 `draw(cell_pos + (0, +32))`。原生 `CQuickSlot::Draw@0x16bbe20` 用 `nDstTop = y - originY + 32`（`:783`、`:886`、`:1252`）——**当 originY=32 时原生算得 `y - 32 + 32 = y`，图标左上角落在槽左上 `(x, y)`**；而本端画在 `(x, y+32)`，即**比原生低 32px**！这是"图标偏移"最明确的根因：**本端把 +32 当成了下沉量直接加，原生是把 +32 当作图标自身底锚点在 Copy 时先减掉再加回槽位基准，净效果为 0（图标左上=槽左上）**。对于 Skill/Item 图标（origin 已被清零，见 `:3055` 注释），本端 `icon_dy=0` 画在 `cell_pos`，与原生 `-0+32=y+32` 比又**高 32px**。两类图标各自偏 32px 且方向相反 → 真机观感混乱。
   - 键帽条（key/*）origin=(0,0)、源码画 `cell_pos+(1,0)`（`:2497-2501`），而原生键帽/数字标号走 `CUIKeyConfig::GetShortCutPos@0x1396510`（`ida-statusbar2-layout-2026-11-19.md:54,227`），口径不同，可能叠加 1~4px 细偏。

**根因排序（按证据强度）**：
- **A（最强，代码可证）**：图标 dy 处理与原生 Copy 公式相反（本端 `cell_pos+origin.y`，原生 `cell_pos-origin.y+32`），造成 KeyConfig/icon 图标偏下 32px、Skill/Item 图标偏上 32px（`:2474-2478` vs IDA Draw `:780-785/:883-888/:1249-1254`）。
- **B（强）**：空图标来自绑定表/图标节点缺失，无兜底绘制（`:2402` 空 Texture 后无 fallback），且 1 秒刷新延迟（`:911-918`）。
- **C（中）**：底板 `backgrnd(-12,-3)` / `cover(-10,0)` 各自 `shift(origin)` 后与 `MODERN_QS_POS`（父层相对坐标）共用 `qs_draw`，父层锚点差异未显式对齐（`:448-456,743-760` 与 `modern-menu-qs-fix-2026-11-18.md`）。

### 2.6 修复建议（取证方建议，未实施）

1. **图标 dy 对齐原生**：把 `draw_modern_quickslot`（`:2474-2478`）改为按原生 `Copy(nDstTop = y - originY + 32)` 语义：对 `KeyConfig/icon`（origin.y=32）净偏移应为 0，即图标左上角直接画 `cell_pos`；对 Skill/Item（origin 已清零）应画 `cell_pos + (0,32)`（与原生 `-0+32` 一致）。可统一写为 `draw(cell_pos + (0, 32 - icon.get_origin().y()))`，彻底消除两类图标的 32px 反向偏移。
2. **空图标兜底**：`refresh_modern_qs_icons()` 在 `classic_qs_icons[i]` 为空时，按当前 `mk` 反查 `KeyConfig/icon/<对应actionId>` 给一张默认键位图（本端该节点有 69 项），或在 `draw_modern_quickslot` 空分支画一个占位框/键名，避免"空灰格"；同时把 `:911-918` 的刷新间隔在绑定变更事件里主动触发一次（而非等满 1 秒）。
3. **底板/槽位同一父层锚点**：将 `MODERN_QS_POS` 的原点基准改为与 `quickslot[0]` `shift(origin)` 后的 `qs_draw` 同一坐标（或反过来，在 `modern_qs_cell_at` 的 base 里补回 `backgrnd.origin=(-12,-3)` 的差值），并用真机抓帧量取"第一格左上像素"验收；对齐 `layer:cover` origin=(-10,0) 与 `backgrnd`(-12,-3) 的 2px/3px 差。
4. **澄清"2 行×10 列"**：向用户确认图2 客户端是否使用 `UIActionBar.img`/ActionBar 主题（2×10 是新版本 ActionBar 布局，本端无此资源）。若确认要还原该形态，需另行 IMG 取证 ActionBar 节点后再评估，属新功能/范围决策，勿直接套 28 槽表。
5. **展开/收缩**：现有 `quickslot_adj` 方案语义正确，建议仅为视觉对齐把 Y 位移改到与原生一致（收起下移 67px，见 IDA Toggle `:17`），X 位移保留与否按真机观感定。

---

## 3. 局限与口径

- 本取证为**源码 + MCP 只读解析**，未做真机抓帧；所有"偏移像素/观感"结论需真机按"菜单六钮 → 出列表 → 点任务 → UIQuestLog 弹出"路径复核。
- IDA 13339 参照端（192/StatusBar2）与本端（xy/StatusBar3）是**不同版本不同资源**：IDA 的槽表/67px 位移用于语义与结构对照，不作为本端像素数直接照搬。
- Hex-Rays 对 COM 返回值存在误建模（见 `ida-statusbar2-layout-2026-11-19.md:11,194`），`m_nInitX/m_nInitY` 初值不可信，故未采信任何由 COM 返回值推出的像素；可信的是 `s_ptShortKeyPos` 内存表、分辨率分支常量 8/28、`Toggle` 的 67 相对位移、Draw 的 `Copy(...+32)` 公式。
- `draw_modern_quickslot` 的 `-origin.y+32` 与 `+origin.y` 差异是从两端源码/伪代码字面推出的**强证据推断**，真机像素方向仍建议抓帧确认（不排除本端 Texture 绘制接口已内建 origin 抵消，若如此则 A 项需按实测修正）。
