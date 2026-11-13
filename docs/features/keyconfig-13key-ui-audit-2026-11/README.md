# 键盘设置 13 键 · UI 布局/动画/按钮/功能/排版审查（083/v83 基准）

> 用户请求（m00003）：「检查一下 S、E、I、K、P、F、G、R、H、B、C、分号、m 等按键，在键盘设置上按键能触发窗口或者功能的按键，检查一下他们打开的 ui 布局、动画，ui 中的按键、功能是否都实现了 排版是否都正确，使用 rea 对照 Angel 反编译检查一下，设计 img 资源的 也可以使用 mcp 检查一下」。
>
> 审查对象 = 附图「键盘设定.png」上 13 颗可开窗口/功能的键帽；基准固定 **083/v83**（`Angel.exe` IDA `E:\MXD\反编译IDA\v83\Angel.i64`，端口 13339）。本报告只做**只读审查**，0 行行为代码改动。

## 1. 证据链（门禁 18：REA/IDA → orange-wz(IMG) → 本地源码 → 正确客户端截帧）

| 层 | 工具/来源 | 落盘证据 |
| --- | --- | --- |
| Angel(v83) 反编译 | IDA 直连 `tools/_ida_mcp.py` → `http://127.0.0.1:13339/mcp` 的 `decompile` | `evidence/ida_UI_Toggle.json`、`evidence/ida_UI_Open_Close.json`、`tools/_ida_CWvsContext_UseFuncKeyMapped.txt` |
| IMG 资源 | orange-wz MCP（`http://127.0.0.1:10012/mcp`，128B key + `TSPHKw==`，`load_files exclusive:false`，`unload_all` 收口） | `evidence/orange_wz_window_nodes.json`（17 窗口根）、`evidence/orange_wz_window2_nodes.json`（第二根 + 关键字全搜）、`evidence/button_gap_matrix.{json,md}` |
| 本地源码 | `ops_grep`/`ops_read_files` 逐行读 | `evidence/audit_data.json`、`evidence/local_ui_audit.md` |
| 原生截帧 | `read_image` 直读 `C:\Users\11036\Desktop\正确客户端截图\` | 见 §6 截图索引（视觉后端本轮 `VISION_BACKEND_UNAVAILABLE_THIS_TURN`，改本地直读，属门禁 18 合法备选） |
| 键帽逐字 | 放大图 `ui/keycaps_rows_3x.png`（1530x531）+ `read_image` 逐字核对 | `ui/keycaps_rows_3x.png`；`vision_describe` 首轮口供为幻觉，已作废（见 §2 注） |

工具状态：REA CLI 不可用（`npx canceled due to missing packages and no YES option: ["rea-agents@6.7.0"]`），本会话无 REA/IDA MCP 工具（`mcp_connector_tool_search` 0 命中）；以已落盘 `tools/_ida_*` 证据 + 直连 13339 的 `decompile` 补齐，等价覆盖门禁 18 的 REA→IDA 段。

## 2. 键帽 → action 定案（运行时口径，非 staged 表）

> **口径纠偏**：`IO/UITypes/UIKeyConfig.h` 的 `basic_keys`(:181-229) / `alternate_keys`(:134-179) 只是键位设置窗「全部消除 / 默认」用的 **staged 表**；运行时真相是 `Net/Handlers/PlayerHandlers.cpp:378-389` 的 `KeymapHandler` 收 KEYMAP(335) → `UI::add_keymapping` → `Keyboard::assign`，并经 `Net/KeyMapTranslate.h:30-41` 的 `from_server()` 做 v83 编号回译。拿 staged 表当运行时判「绑错窗」是假象，本报告一律以服务端下发链为准。
>
> **视觉口供作废**：`vision_describe` 首轮给出「S→技能 / K→技能消息 / R→组队 / B→组队 / C→组队 / M→地图」等，与放大图逐字不符，判幻觉，整段作废。以下 13 键以 `ui/keycaps_rows_3x.png` 逐字定案。

| 键 | 键帽语义（放大图逐字） | v83 服务端 action | `from_server` 后本地枚举 | 目标 UI 类 | Angel(v83) 权威对照 |
| --- | --- | --- | --- | --- | --- |
| S | 能力值 | 2 | `STATS`(2) | `UIStatsInfo` (`window4("Stat")`) | `UseFuncKeyMapped case2→v20=2` → `UI_Toggle a2=2` → `UI_Open:418 CUIStat` |
| E | 装备窗 | 0 | `EQUIPMENT`(0) | `UIEquipInventory` (`window4("Equip")`) | `case0→v20=1` → `UI_Open:430 CUIEquip` |
| I | 道具 | 1 | `ITEMS`(1) | `UIItemInventory` (`window("Item")`) | `case1→v20=0` → `UI_Open:442 CUIItem` |
| K | 技能 | 3 | `SKILLS`(3) | `UISkillBook` (`window("Skill")`) | `case3→v20=3` → `UI_Open:406 CUISkill` |
| P | 组队 | 19 | `PARTY`(19) | `UIUserList` `Tab::PARTY` | `case19→v21=1 v20=7` → `UI_Toggle a2=7` 特例 |
| F | 学院 | 26 | `MEDALS`(26)（枚举名沿用新版，实为 v83 FAMILY） | `UIFamily` | `case26→v20=27` → `UI_Toggle a2=27` → `UI_Open:205 CUIFamily` |
| G | 家族 | 17 | `GUILD`(17) | `UIUserList` `Tab::GUILD`（第 3 页签，**非独立浮窗**） | `case17→v21=2 v20=7` → 共用窗 |
| R | 好友 | 4 | `FRIENDS`(4) | `UIUserList` `Tab::FRIEND`（第 1 页签） | `case4→v21=0 v20=7` → 共用窗 |
| H | 悄悄话 | 11 | `WHISPER`(11) | `UIWhisperTargetDlg` | `UIStateGame.cpp:273-288` → `UIChatBar::set_whisper_target` 回调 |
| B | 物手册（怪物手册） | 22 | `MONSTERCOLLECTION`（`KeyMapTranslate.h:34`） | `UIMonsterBook` | `case22→v20=9` → 怪物手册 |
| C | 聊天 | 6 | `MAPLECHAT`(6) | `UIMessenger::try_create_room` → 回退 `UIChat` | `case6→CUIMessenger::TryNew(0)` |
| ;（分号） | 勋章 | 27 | `static_cast<Id>(28)` 空位（`KeyMapTranslate.h:37`） | **无 UI**（`UIStateGame` 无 case → 死键） | `case27→v20=31` → `UI_Open:260 sub_840170(-1)` 勋章窗 |
| M | 小地图 | 7 | `MINIMAP`(7) | `UIMiniMap` | `case7→sub_8590F9` → 小地图 |

> **Angel 权威编号错位实证**（`evidence/ida_UI_Open_Close.json`）：v83 `action 26` 枚举名 `MEDALS` 但真实开 `CUIFamily`（学院）；`action 27` 枚举名 `BOSSPARTY` 但真实开勋章（`sub_840170`）；`action 22` 是怪物手册；`action 29` `PROFESSION` → `UI_Open:157 CUIItemMaker`。本地 `IO/UIStateGame.cpp:296` 把 `MEDALS(26)` 接 `UIFamily`、`:524` 把 `PROFESSION(29)` 接 `UIMaker`、`:518` 把 `MONSTERCOLLECTION` 接 `UIMonsterBook`，与 Angel 逐一对上。

## 3. 13 键实现度结论表（本报告主交付）

结论等级：**✅ 一致** = 布局/按钮/功能/排版与 Angel+IMG+截帧吻合；**⚠️ 部分** = 有小差异（个别按钮未绑 / 坐标推断 / 预留 TODO / 近似件）；**❌ 缺失** = 整窗未落地或纯文本桩，与原生差距大。

| 键 | UI 类 | 布局 | 窗口/UI 动画 | 按钮 | 功能 | 排版 | 结论 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| S 能力值 | `UIStatsInfo` | v83 经典 `backgrnd` 208x347 + `basicStat`(8,195) + `backgrnd2` 详细板 | 按钮 4 态帧动画 | 12 引用 / IMG 5：`BT_CLOSE`、`BT_HP/MP/STR/DEX/INT/LUK`(BtApUp 复用)、`BT_AUTO`、`BT_DETAILOPEN`、`BT_DETAILCLOSE`(近似件) | ✅ `update_all_stats`/`update_stat`/`update_ap`/`update_exp_label`/`button_pressed`@641 | 四窗同屏并存（截帧坐实）；`BtDetail` 本地(153,324) vs Angel `(124,324)` 有偏差 | ⚠️ 部分 |
| E 装备窗 | `UIEquipInventory` | `window4("Equip")`，`backgrnd` + 装备槽 iconpositions + 宠物 3 图标 | 按钮 4 态帧动画 | IMG 7 / 代码 7 **全对上**（BtCashshop/BtDetail/BtPet1/2/3/BtPetEquipHide/Show） | ✅ `button_pressed`@276 覆盖 `BT_CLOSE/BT_SHOP/BT_SLOT/BT_PET_*` | 截帧装备窗槽位/整理钮/宠物栏吻合 | ✅ 一致 |
| I 道具 | `UIItemInventory` | `window("Item")`，5 页签 + 道具格 + 金币栏 | 按钮 4 态帧动画 | IMG 6 / 代码 6 **全对上**（BtCoin/BtFull/BtSmall/BtGather/BtSort/BtCashshop） | ✅ `button_pressed`@504 覆盖 20+ case（页签/整理/ gather/sort/金币/兑换…） | 截帧「现有道具窗」5 页签 + 金币 0 + 展开箭头吻合 | ✅ 一致 |
| K 技能 | `UISkillBook` | `window("Skill")`，5 tab + `CLASSIC_SKILL_OFFSET(7,99)` + 技能点 | 按钮 4 态帧动画 | 代码 10 / IMG 仅 1（BtSpUp）；其余 9 走 modern 资源或回退 | ✅ `button_pressed`@515 + `update_skills`@902 | 截帧「冒险家技能」4 行 + 技能点 0 吻合；9 钮无本包 IMG 背书 | ⚠️ 部分 |
| P 组队 | `UIUserList` `Tab::PARTY` | 共用窗 5 页签 `TAB_POS[5]`，`backgrnd` 三张互斥 | 按钮 4 态帧动画 | 代码 39（共用窗全量），`BT_PARTY_*` 齐 | ✅ `button_pressed`@606 + `change_tab` | 截帧 310x386「玩家列表 & 家族」5 页签坐实 | ✅ 一致 |
| F 学院 | `UIFamily` | 224x392（截帧 227x392，差 3px 属裁切容差）+ `backgrnd` | 按钮 4 态帧动画 | IMG 8 / 代码 8 **全对上**（BtClose/BtOK/BtTree/BtLeft/BtRight/BtJuniorEntry/BtFamilyPrecept/BtSpecial） | ✅ `button_pressed`@353 全实现：`FamilyAddPacket`/`FamilyPreceptPacket`/`OpenFamilyPedigreePacket`/`use_selected`/`page_privilege` | 截帧「学院.png」8 钮 + 同学/名声度/特权区逐块吻合 | ✅ 一致 |
| G 家族 | `UIUserList` `Tab::GUILD` | 共用窗**第 3 页签**（非独立 `UIGuild` 浮窗） | 按钮 4 态帧动画 | `BT_GUILD_*` 齐 | ✅ | 截帧第 3 页签「家族」红字选中 + 在线/离线家族双表 + 底部 12 钮坐实 | ✅ 一致 |
| R 好友 | `UIUserList` `Tab::FRIEND` | 共用窗第 1 页签（`backgrnd3`） | 按钮 4 态帧动画 | `BtAddFriend/BtPlusFriend/BtGroupWhisper/...` 齐 | ✅ `CheckReqFriend` 对应 | 截帧「好友列表框.png」坐实 | ✅ 一致 |
| H 悄悄话 | `UIWhisperTargetDlg` | `Basic.img/Notice4(t,c,s)` + `ComboBox` + `UtilDlgEx/BtOK/BtNo`，261x117 | 按钮 4 态帧动画 | ✅ `BtOk`/`BtNo` + 输入框 + 下拉候选 | ✅ `confirm()`@7671 → `set_whisper_target`；`pick_candidate`/`open`/`send_key` 全 | 截帧「悄悄话框.png」261x117 标题/输入行/确定取消吻合 | ✅ 一致 |
| B 物手册 | `UIMonsterBook` | **纯文本列表桩**：400x480 / `backgrnd` + `cover_label`/`total_label` + `list_origin(20,90)` | 无（仅静态文本） | 仅 `BtClose`；IMG 15 节点中 13 未绑 | 仅 `BtClose` + 点卡片 `MonsterBookSetCoverPacket`；无搜索/翻页/便签/页签 | 原生是打开书本双页 540x357 + 左页统计 + 右页星座底纹 + 8 色便签 + 4 页签，**差距大** | ❌ 缺失 |
| C 聊天 | `UIMessenger` → 回退 `UIChat` | `backgrnd/2/3` + `NameBar` + `chat_input` | ✅ `Animation(root["chatBalloon"])` + `blink` 逐帧 | `BtEnter`（IMG）+ `BtClose`（通用）+ `BtClose3`（代码独有，走 modern 资源） | ✅ `try_create_room`@178 → `MessengerOpenPacket(0)`；`button_pressed`@667 | IMG 7 节点仅用 4；无本包独立聊天室截帧对照 | ⚠️ 部分 |
| ; 分号 勋章 | **无** | 未落地 | 未落地 | 未落地 | `KeyMapTranslate.h:37` 明确「本包无勋章窗，压成无 case 的枚举空位，灭掉 BOSSPARTY(27)『; 开错窗』；勋章窗落地后改映射」→ `UIStateGame` `default` 只打 `Unknown KeyAction::Id action: [28]` | 原生截帧「勋章框.png」255x372：5 页签 基本/职业/一般/挑战/活动 + 获得个数 + 空列表 + 滚动条 | ❌ 缺失 |
| M 小地图 | `UIMiniMap` | `Min/Map/MaxMap/MinMap` + `Type::MIN/NORMAL/MAX` 三态 | ✅ `player_marker`/`selected_marker`/`portal_marker`/`npc_marker` 全 `Animation` 逐帧 | 6 钮全实现：`BT_MIN/BT_MAX/BT_SMALL/BT_BIG/BT_MAP/BT_NPC` | ✅ `button_pressed`@479 切态持久化 `minimap.type`、`BT_MAP` 开 `UIWorldMap`、`BT_NPC` 切 npclist；`BT_SMALL/BT_BIG` 留 `// TODO: Toggle scrolling map` | 截帧库 `小地图*.png` 4 张坐实三态；`BtMax/BtMin` 代码独有（走 `Basic.img` 真件） | ⚠️ 部分 |

**汇总：✅ 7 键一致（E/I/P/F/G/R/H）／⚠️ 4 键部分（S/K/C/M）／❌ 2 键缺失（B 物手册、; 分号勋章）= 13 键。**

## 4. 差异清单（按优先级，本轮 0 改动，仅登记）

### P0 · 整功能缺失（必修，直接对应键帽语义）

- **D1 分号键勋章窗整体未落地**：`Net/KeyMapTranslate.h:37` `case 27: return static_cast<Id>(28);` 把 v83 MEDAL(27) 压成无 case 空位，`UIStateGame.cpp:585-589` 落 default 死键。原生 `UI_Open:260 sub_840170(-1)`（Alloc 2944）对应 255x372 勋章窗（正确截图「勋章框.png」）。IMG 侧复核：orange-wz 在 `UIWindow.img`/`UIWindow2.img` 搜 `Medal` 均 `matches:[]`，两 img 的 `win:Medal`/`win:MedalQuest`/`win:MedalList` 全 `节点路径不存在`；`E:/MXD/BeiDou-Client_S9/Data/UI/` 24 个 `.img` 无 `Medal.img`。→ 客户端包内连勋章窗素材根都缺，需补资源 + `UIMedal` 类 + `from_server(27)` 映射 + `UIStateGame` case。
- **D2 B 键怪物手册是纯文本桩**：`IO/UITypes/UIMonsterBook.cpp:46-123` 只画 `backgrnd` + 两行 label + 卡片文本列表，仅 `BtClose` + 点击设封面。原生「怪物手册.png」540x357 是书本双页：左页 卡片图标/等级/登记数/特殊/一般/书签，右页 星座底纹，左侧 8 色便签竖条，右侧 4 页签 基本情报/怪物情报/战利品/出没地区。IMG `UIWindow.img/MonsterBook` 15 节点（`cardSlot/infoPage/select/cover/fullMark/arrowLeft/arrowRight/icon/BtClose/BtSearch/LeftTab/LeftTabInfo/RightTab/ContextMenu`）本地仅用 2（`backgrnd` + 通用 close），13 未绑。→ 需按书本双页重排版 + 绑 13 节点 + 补搜索/翻页/便签/页签功能。

### P1 · 动画与窗口级按钮缺口

- **D3 窗口开合补间未落地（全局，影响全部 13 键）**：Angel `CWvsContext::UI_Toggle @0xa04f7a`（`evidence/ida_UI_Toggle.json`）每个 case（0/1/2/3/5/6/9/10/22/23/26/27/28/29/30/31/32/33）先读 `*(this+3438+2*idx)` 判开关，再取 `StringPool::GetStringW(SP_2302_MENUUP / SP_2303_MENUDOWN)` → `play_ui_sound` → `UI_Open/UI_Close`。本地 `IO/UIStateGame.cpp` 是 `makeactive()/deactivate()` 瞬时切换；`IO/UIState.h:58 activate_after_transition` 默认空实现、`:60 bring_to_front` 默认空实现，仅 `UIStateLogin.cpp:291` 覆写了补间后激活。→ 游戏态没有原生 MENUUP/MENUDOWN 音效 + 菜单伸缩窗口动画。按钮内帧动画（`MapleButton::update` 的 `animations[state].update(6)`）与 `UIElement::draw(alpha)` 插值透传是有的，缺的是**窗口级**这一层。
- **D4 `UIMaker`（PROFESSION action29，与 B 键同源链）3 钮未绑**：`IO/UITypes/UIMaker.cpp:39-78` 只 `close_btn` + `UISoftKey` 输入 itemId → `MakerCreatePacket`。IMG `UIWindow.img/Maker` 有 `BtStart/BtCancel/BtDown` + `GaugeBar`/`ComboBox1`/`ComboBox2`/`randomMake`/`randomRecipe` 未接；`button_pressed`@163 只有 `BtClose`。原生 `UI_Open:157 CUIItemMaker`（Alloc 3404）是完整配方制作窗。→ 制作按钮/进度条/下拉/随机配方缺口。
- **D5 `UIKeyConfig` 自身 `BtHelp` 未绑**：`evidence/button_gap_matrix.md:10` IMG 7 / 代码 6，`img_only=BtHelp`。正确截图键盘设定右上确有「?」帮助钮（历史截图「点击"帮助"按钮打开的脚本框.png」佐证），本地无绑定。

### P2 · 坐标/近似件/容差（不阻塞功能，留档）

- **D6 Stat `BtDetail` 坐标**：本地 `(153,324)`（`UIStatsInfo.cpp:162`，来源 = 截帧 SAD=0.25 锚算），Angel `CUIStat::OnCreate @0x8c4c97` 逐字只建 `2006/(124,324)` 的 BtDetail。差 29px，属「截图实测 vs IDA 立即数」两套证据，历史文档已按截帧口径定案。
- **D7 Stat 近似件与推断坐标**：`BtHide` 用 `Basic.img/BtHide`(13x12) 近似（`UIStatsInfo.cpp:167`）；`BtAuto` x=106 由 `UIWindow2.img/Stat/main/BtAuto` canvas.origin 推（`:146-158` 长注释自述「属节点 origin + 截图实测推断，漂移风险记 backlog」）；Stat 数值色历史判词「原生是比例文字，无位图字库可用」。
- **D8 学院窗 3px 差**：`UIFamily.cpp:59` `dimension(224,392)` vs 截帧 227x392。
- **D9 KeyConfig 5 按钮 y=234 无 083 背书**：`UIKeyConfig.cpp:118-125` 长注释明确 5 颗 `y=234` 来自 `正确客户端截图/键盘设置.png` 逐像素，非 `CUIKeyConfig` ctor `0x832586` / `OnButtonClicked 0x8335d0` / `GetShortCutIndexByPos 0x833a65` 的立即数（.nx 12 节点无坐标属性）。属门禁 18 合法备选（截帧），但与 IDA 证据不同源。
- **D10 MiniMap `BT_SMALL/BT_BIG` 预留**：`UIMiniMap.cpp:516` `// TODO: Toggle scrolling map`；历史文档 `ui-v83-window-restore/README.md:101` 已记「BtSmall/BtBig/BtNpc 显式预留」。

## 5. 动画专项结论

| 维度 | Angel(v83) 权威 | 本地实现 | 判定 |
| --- | --- | --- | --- |
| 窗口开合 | `SP_2302_MENUUP`/`SP_2303_MENUDOWN` + `play_ui_sound` + 菜单伸缩（`UI_Toggle` 全 case） | `makeactive()/deactivate()` 瞬时；`UIStateGame` 未覆写 `activate_after_transition` | ❌ 缺（D3） |
| 按钮 4 态 | `CCtrlButton` state 动画 | `MapleButton::update` `animations[state].update(6)` + `draw` 按 state 选帧 | ✅ 一致 |
| 帧插值 | — | `UIElement::draw(float alpha)` → `sprite.draw(position, alpha)` | ✅ 一致 |
| 窗内逐帧动画 | — | `UIMiniMap` `player_marker/portal_marker/npc_marker`、`UIMessenger` `chatBalloon/blink` = `Animation` | ✅ 一致（仅 M/C 两键有窗内逐帧，其余键窗内为静态精灵 + 按钮态动画） |

> 原生 v83 的「动画」主体是**音效驱动的开关伸缩**，不是 canvas 逐帧窗口动画；本地把逐帧动画做在了按钮/小地图/聊天气泡上，唯独缺窗口开合这一层。

## 6. 排版专项结论 + 原生截帧索引

`read_image` 直读（视觉后端本轮不可用，属门禁 18 合法备选）：

| 截图 | 尺寸 / sha256（前 8） | 坐实结论 |
| --- | --- | --- |
| `打开了S-E-I-K页面.png` | 1280x720 / `1911ac29` | S/E/I/K 四窗同屏并存、互不遮挡主逻辑、底栏 classic HUD 完整 |
| `好友、组员、家族、家族联盟、拒绝列表.png` | 310x386 / `dcdec7a8` | G 键是共用窗第 3 页签「家族」（红字选中），非独立浮窗；5 页签 + 底部 12 钮 |
| `学院.png` | 227x392 / `e736991e` | F 键 8 钮坐标族 + 同学/名声度/特权区 |
| `勋章框.png` | 255x372 / `d849d87c` | 分号键真窗 → **D1 FIXED**（UIMedal 落地 + turn10 逐像素色带对齐，量测 `evidence/medal_frame_metrics.json`） |
| `怪物手册.png` | 540x357 / `eba58249` | B 键书本双页真窗 → **D2 FIXED**（UIMonsterBook 重写 15 节点全绑） |
| `悄悄话框.png` | 261x117 / `ca7369bf` | H 键 `UIWhisperTargetDlg`（复用 `Basic.img/Notice4`，非独立 Whisper 根） |
| `键盘设置.png`（用户附图 m00001） | 629x369 / `cb76514e` | 13 键帽语义 + 底部 5 按钮 + 4 行 10 列空白槽 |

已对排版：S/E/I/K 同屏、G 共用窗、F 学院、H 悄悄话、M 三态、KeyConfig 629x373（`backgrnd` origin(314,186) z0）。**修复后收口**：B 书本双页（D2 三方坐标 SAD 坐实）、; 勋章框（D1 turn10 逐像素色带量测对齐）、KeyConfig 5 按钮 y=234（D9 SAD 0.00~11.58 坐实）、UIMaker 两钮坐标（D4 IDA 立即数，ComboBox/GaugeBar 无数据流件有据预留）。

## 7. 产物与验收信号

| 类别 | 文件 |
| --- | --- |
| 主报告 | `docs/features/keyconfig-13key-ui-audit-2026-11/README.md` |
| 逐键细证 | `docs/features/keyconfig-13key-ui-audit-2026-11/notes/13key-verdict-detail.md` |
| 机读结论 | `docs/features/keyconfig-13key-ui-audit-2026-11/evidence/verdict_matrix.json` |
| IMG 节点 | `evidence/orange_wz_window_nodes.json`、`evidence/orange_wz_window2_nodes.json`、`evidence/button_gap_matrix.{json,md}` |
| IDA 反编译 | `evidence/ida_UI_Toggle.json`、`evidence/ida_UI_Open_Close.json` |
| 本地源码摘要 | `evidence/audit_data.json`、`evidence/local_ui_audit.md` |
| 键帽放大图 | `ui/keycaps_rows_3x.png` |
| 只读探针脚本 | `tools/_key13_wz_probe.py`、`tools/_key13_wz_probe2.py`、`tools/_key13_ida_uitoggle.py`、`tools/_key13_audit_data.py`、`tools/_key13_button_gap.py` |

验收信号（审查阶段）：13 键结论齐全（7 ✅ / 4 ⚠️ / 2 ❌，与 §3 汇总行一致）；所有 `evidence/*.json` 可 `json.load`；orange-wz `load isError:false / loadedCount:2 / unload_all` 收口；审查阶段 0 行行为代码改动（修复阶段改动见 §8）。

## 8. 修复落地（2026-11-11，用户 m00325「就按这个修复」）

审查交付后按 P0→P2 落地，全量机读表见 `evidence/fix_matrix.json`，仲裁方法见 `notes/d5-d10-arbitration.md`。

| 缺陷 | 处置 | 关键证据 |
| --- | --- | --- |
| D1 分号勋章窗 | **FIXED**：新建 `UIMedal.h/.cpp`；`KeyAction MEDAL=28` 填空洞、`UIElement::Type::MEDAL`、`from_server(27)↔to_server` 双射恢复、`UIStateGame` case、vcxproj/filters 接入（`tools/_d1_wire.py` 6 链接 + 13 postchecks）。**turn7-8 升级**：真实数据对接（`QuestInfo.img viewMedalItem` 82 条 × `QuestLog::is_completed`，**无新协议**——用户指正后 .nx 探针翻案）+ 原生模型对齐（ctor 260x374 / Draw 步进 49\|23 / clip 7..291 / OnMouseButton 行选中 / SP_5301-5302 状态文本 / tab100「基本=已获得全量」语义）；Title 件与 Item 114xxxx 图标经探针否定（SAD 46~214 无坐实 / Item.nx 零命中），保留 ColorBox 不盲换。**turn10 排版像素级对齐**（用户 m01261 钉靶「勋章按键 按下后打开的勋章ui」）：对 勋章框.png 逐像素量测色带（顶深蓝线 85,119,153 / 标题下蓝线 119,153,187 / 页签带 170,170,170 选中粉红 238,102,136 / 计数条白 y46..79 / 列表面板 238,238,255 y78..350 / 滚动条槽 x239..253 / 底条 153,187,204 y351..371），ColorBox 逐带复刻；原生 IDA clip(7,48,230,291) 与截帧 LIST 带分属两套坐标系，双证据独立编码 `CLIP_*` 与 `LIST_*`，断言不放宽不删而收紧；`Color(int×4)` 重载歧义用 `rgb()` helper（std::array）消解，Text 只吃 Color::Name → 计数文字用 `MINESHAFT`(51,51,51) | 勋章框.png + UI_Open:260 + d1b_ida.json + d1c_detail.json + d1b_title_assets.json + medal_frame_metrics.json + d1b_ida.json + tools/_d1_questnx_probe{,2}.py + tools/_medal_frame_measure.py |
| D2 怪物手册 | **FIXED**：`UIMonsterBook` 重写书本双页，15 节点全绑；坐标三方对齐（IDA sub_86256D 立即数 + orange-wz 尺寸 + 截帧 SAD：backgrnd 锚(34,7)、infoPage(40,35)、cardSlot(261,53)、LeftTab y=131+20i SAD0、fullMark(236,335) SAD0）；搜索/翻页/点格设封面/9 便签勾选态接线；locale 4 新键（UTF-8 无 BOM，换行与文件主导 CRLF 一致——turn7 口径修正，旧文误记 LF） | ida_monsterbook_ctor.json + d2_canvas_anchor.json |
| D3 开合淡入 | **FIXED**：`UIElement::fade_in`（mutable），ctor/makeactive 归零，draw_sprites+draw_buttons 双推进点（消费即推进不卡半透明，~150ms 收敛），sprite alpha 乘入；自绘窗手动乘 fade。音效层 MENUUP/MENUDOWN 本地已有。淡出不做（draw 只画 active，关闭反馈由 MENUDOWN 承担，注释内） | UI_Toggle SP_2302/2303 |
| D4 UIMaker 两钮 | **FIXED**：BtStart=20000@(82,356) 制作选中、BtCancel=20001@(163,356) 关窗，id 沿用 IDA 立即数；ComboBox/GaugeBar/BtDown 等无数据流件显式预留（注释逐条） | CUIItemMaker::OnCreate @0x822350 |
| D5 BtHelp | **NOT-A-DEFECT**：原生帧全窗 SAD=175 无落点 + IDA 析构只 6 颗 CCtrlButton → v83 根本不建，不绑=正确 | d569_match.json |
| D6 BtDetail | **CONFIRMED-CORRECT**：SAD 实测 frame=(154,326) SAD0.62，本地 (153,324) 差 1~2px；IDA a4:124 是锚点口径非 top-left | d569_match.json |
| D7 BtAuto | **DOWNGRADED**：角色能力.png 无可信落点（SAD147 误配区）→ 原生不绘此件，本地增强保留 | d569_match.json |
| D8 学院 3px | **NOT-A-DEFECT**：IMG 真身 224x392，截帧 227 含边 | orange-wz |
| D9 5 钮 y=234 | **CONFIRMED-CORRECT**：五颗期望坐标 SAD 0.00~11.58 全点覆盖，口径由「截图目测」升级为「SAD 实测」 | d569_match.json |
| D10 MiniMap | **RESERVED**：IMG 无 BtSmall/BtBig 素材，TODO 注释升级为带证据引用 | orange-wz |

修复后 build：`tools/build_smoke.bat` **EXITCODE=0**（GATE OK；UIElement:149 / UIMaker:1456 / UIMedal:1504 / UIMonsterBook:1674 四 TU 重编；exe 18:24）。自检门 `tools/_key13_verify.py` 扩 D1-D4 断言 ALL PASS。备份：`patches/`（23 个 .orig + UIMedal 新建）。0 commit / 0 push。

**turn8 收口 build**：D1 升级后 `EXITCODE=0`（exe 21:59 产出 4,982,784 B）。期间 LNK1104 输出文件被运行中客户端（pid 59256）占用——经用户裁决选 B「AI 自动结束进程后重试链接」，taskkill 后重试通过；该裁决同步记入 MEMORY（后续同冲突自动执行）。自检门扩 D1T8 断言 13 项后 **ALL PASS**（帧 260x374 / 步进 49|23 / clip 7..291 / tab100 语义 / 行选中接线 / 详情预留 / 状态文案 / const 正确性 / 写侧页签变色 / locale 键 / IDA 证据文件 / ColorBox 保留判定 / turn7 备份）。

**turn10 收口 build（排版对齐 · 用户 m01261 钉靶）**：D1 排版重写后 `tools/build_smoke.bat` **EXITCODE=0**（GATE OK + 全量链接）。编译错误链逐字修复：① `C2440 "<function-style-cast>": 无法从"initializer list"转换为"ms::Color"`（15 处 `Color(int,int,int,int)` 在 float×4 / uint8_t×4 两重载间歧义）→ `rgb()` helper 走 `std::array<uint8_t, Color::LENGTH>` 明确构造；② `C2664 "ms::Text::Text(...)": 无法将参数 3 从"ms::Color"转换为"ms::Color::Name"`（Text 构造只吃 Color::Name）→ `count_label` 改 `Color::Name::MINESHAFT`(51,51,51 = 量测深灰精确值)。自检门 turn8 旧断言 `D1T8 list clip 7..291` 因坐标系升级 FAIL → **不放宽/不删，收紧为双证据**：新增 `D1T8 native clip 7/48/230/291`(IDA 立即数) + `D1T10 frame band metrics`(截帧 TAB_Y=28/COUNT_Y=46/LIST_Y=78/LIST_BOT=351/LIST_X=5/LIST_W=232) + 7 项排版断言（色带成员声明/绘制/无占位注释残留/rgb helper/量测证据）→ **ALL PASS**。备份 `patches/UIMedal.{h,cpp}.turn10`；fix_matrix.json D1 条目、`tools/_medal_frame_measure.py`、`evidence/medal_frame_metrics.json` 同步收口。

**修复后 13 键状态：D1/D2 两把 ❌ 清掉 → 13 键全部有实现或有据预留（9 ✅ / 4 ⚠️ 有据预留，0 ❌）。分号键勋章窗排版对 勋章框.png 完成逐像素色带对齐（m01261）。**
