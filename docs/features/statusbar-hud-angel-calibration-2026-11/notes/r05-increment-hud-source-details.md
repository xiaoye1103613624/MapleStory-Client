# r05 — 状态栏 HUD 面「当前端源码盘点·增量补充包」（只读归档）

> 来源：subagent `e0827232` 只读取证增量全文（2026-11-08 19:30，8.8K），回应主 agent 点名项后重新直读复核。
> 主 agent 归档说明：本条**与 r04 是同一子代理的两批交付**，冲突处以本条为准（本条行号更精确、含 `UIStateGame::send_key` 分支 / `Buttons` 枚举数值 / `UIElement::Type` 数值 / `[DBG-CLK]` 销案证据四组 r04 未覆盖内容）。全程未改产品源码；基准 083/v83，未引 095 线作校准依据（源码内现存一行 095 字样仅如实记录）。
> 标注 `[观察]` = 源码直读；`[推断]` = 据直读推导。

## A. 点名小类（逐个，尺寸 / position / 枚举 / stub 逐字）

### UIBuffList
- `IO/UITypes/UIBuffList.h:68-70` `TYPE=BUFFLIST` / `FOCUSED=false` / `TOGGLED=false` [观察]
- `IO/UITypes/UIBuffList.h:39-47` `FLASH_TIME = 3'000` / `ICON_SIZE = 32` / `LABEL_TOP = 21`；逐字「参考端（ijl15!FormatBuffTime / DrawBuffTimeLabelOnCanvas）把倒计时画在 32x32 的图标画布上：文字顶边 y=21，水平居中，黄色正文 + 黑色 1px 描边。」「>= 1h -> "%dh%dm"；>= 1m -> "%dm%ds"；否则 "%ds"；秒数向上取整且最小为 1。」 [观察]
- `IO/UITypes/UIBuffList.h:101-102` 「ui.buff_timer：是否在图标上叠加剩余时间文本。」+ `bool show_timer;` [观察]
- `IO/UITypes/UIBuffList.h:81-87` 逐字「Wave 3：右键点掉增益图标 = 取消该 buff（s9 CancelBuffHandler 只认 int sourceid=技能id，回 s2c CANCEL_BUFF(33) 后列表自然移除）。命中判定必须自算…右键全落进地图 ContextMenu。」 [观察]
- `IO/UITypes/UIBuffList.cpp:40-64` 图标来源 `SkillData::get(...).get_icon()` 否则 `ItemData::get(...).get_icon()`（带 cover） [观察]
- `IO/UITypes/UIBuffList.cpp:66-96,140-157` draw 图标+IconCover+8 方向黑描边+黄正文；`format_time` 三档 `%dh%dm / %dm%ds / %ds` [观察]
- `IO/UITypes/UIBuffList.cpp:159-161` `UIBuffList() : show_timer(Configuration::get().get_buff_timer())` [观察]
- `IO/UITypes/UIBuffList.cpp:196-200` `position=(new_width-35,55); dimension=(position.x(),32);` [观察]
- `IO/UITypes/UIBuffList.cpp:207-211` 逐字「命中框 = 条带并集 = [position.x - 32*(n-1), position.x + 32) × [position.y - 32, position.y]」 [观察]
- `IO/UITypes/UIBuffList.cpp:224-250` 右键→`CancelItemEffectPacket(skillid).dispatch()` / `CancelBuffPacket(skillid).dispatch()` [观察]
- `IO/UITypes/UIBuffList.cpp:257-311` send_key S/T 弹 UISoftKey 输 buffid→`add_buff(id,60000)`、C/X 清、D 切 UITemporaryStatView [观察]

### UIBossHP
- `IO/UITypes/UIBossHP.h:15-17` `TYPE=BOSSHP`；`:27,30` `set_hp(int32_t oid,int32_t cur,int32_t max,uint8_t tag_color,uint8_t tag_bg)` / `BAR_INNER_W=640` [观察]
- `IO/UITypes/UIBossHP.h:41-42` fill 逐字「DualMobGauge/gauge：1×9 填充条（横向拉伸）」、textlay「DualMobGauge/text：791×26 名牌底（可选）」 [观察]
- `IO/UITypes/UIBossHP.cpp:31-33` 三分件取 `UiAssetResolve::window("DualMobGauge")` 的 `backgrd`/`text`/`gauge`，缺则回退 `window("MobGage")` [观察]
- `IO/UITypes/UIBossHP.cpp:43-46` 逐字「REA 反编译 sub_56C9B7 依次加载 backgrd/text/gauge 三分件（docs/features/beidou-v17-parity/notes/evidence/r17/rea_decompile_DualMobGauge_56C9B7.txt）。修前 fill_w 算了却被 (void) 丢弃，血条只有背景+百分比文字、无填充。」 [观察]
- `IO/UITypes/UIBossHP.cpp:62-63` `LOG(LOG_WARN, "UIBossHP: no DualMobGauge/MobGage backgrnd")`；`:67-79` `hide_ms = visible ? 8000 : 0;` + `label.change_text(std::to_string(pct) + "%")` [观察]
- `IO/UITypes/UIBossHP.cpp:81-106` draw 填充条 `fill.draw(DrawArgument(fillpos, Point<int16_t>(fill_w, fh)))` [观察]
- `IO/UITypes/UIBossHP.cpp:119-127` `position=((new_width-bw)/2,20); dimension=(bw, backgrnd ? backgrnd.height() : 43);`（bw 默认 681） [观察]
- `IO/UITypes/UIBossHP.cpp:129-170` send_key S/T/H 弹 SoftKey 设 cur,max|pct|tip、V 切 visible、C 清 [观察]
- `IO/UITypes/UIMobGage.*` **未找到**；`IO/UITypes/UIMiscShells.h:879` 有 `TYPE=MOBGAGE` 的其它壳类，非同名文件 [观察]

### UIClock
- `IO/UITypes/UIClock.h:16-18` `TYPE=CLOCK`；`:28-37` 逐字「type 1 = wall clock text, type 2 = countdown seconds, else free text」/ `set_clock(int8_t type, const std::string& text, int32_t seconds)` / `stop()` [观察]
- `IO/UITypes/UIClock.cpp:19` `label(A18M, …, YELLOW)`；`:26-43` mode2→`Locale::format("clock.seconds_left", to_string(seconds))`，否则 `Locale::text("clock.title")` 或自由 text [观察]
- `IO/UITypes/UIClock.cpp:62-79` update 递减 `remain_ms`，到 0 → `Locale::text("clock.zero")`；`:81-88` `position=(new_width/2,40); dimension=(160,24);` [观察]
- `IO/UITypes/UIClock.cpp:90-118` send_key S/T→SoftKey 设秒或文本、C/X→stop、R→SoftKey 文本 [观察]
- `locale/zh-CN.ini:355-357` `clock.seconds_left={}s` / `clock.title=Clock` / `clock.zero=0s` [观察]

### UIComboCounter
- `IO/UITypes/UIComboCounter.h:15-17` `TYPE=COMBOCOUNTER`；`.cpp:18` `label(A18M, CENTER, YELLOW)` [观察]
- `IO/UITypes/UIComboCounter.cpp:24-37` `window("Combo")` 回退 `window("Number")`；逐字「本客户端的 UI.nx/UIWindow.img 里没有 Combo/Number 节点…因此只降级为纯文字计数，不再打成功日志。」 [观察]
- `IO/UITypes/UIComboCounter.cpp:40-52` 逐字「服务端 AranComboHandler.java:41 是「距上次超过 3000ms 且连击>0 就归零」…本地用同样的 3000ms 静默期隐藏」；`hide_ms = count > 0 ? 3000 : 0;` / `label.change_text(std::to_string(combo) + " Combo")` [观察]
- `IO/UITypes/UIComboCounter.cpp:75-82` `position=(new_width/2, new_height/2-80); dimension=(120,24);` [观察]

### UIEnergyBar
- `IO/UITypes/UIEnergyBar.h:14-16,27-28` `TYPE=ENERGYBAR` / `FOCUSED=false/TOGGLED=false` / `MAX_ENERGY=10000` / `BAR_INNER_W=70` [观察]
- `IO/UITypes/UIEnergyBar.cpp:29-42` `UiAssetResolve::window("EnergyBar")` 取 w/c/e/Gage，缺则 `LOG(LOG_WARN, "UIEnergyBar: missing EnergyBar assets")` [观察]
- `IO/UITypes/UIEnergyBar.cpp:45-67` draw `fill = energy*BAR_INNER_W/MAX_ENERGY` 逐像素铺 bar_c；`:69-76` update `visible = player.has_buff(Buffstat::Id::ENERGY_CHARGE)` [观察]
- `IO/UITypes/UIEnergyBar.cpp:78-86` `position=(new_width/2-120, new_height-90); dimension=(90,14);`（逐字「Above classic status bar HP/MP area (bottom-center-left)」） [观察]
- `IO/UITypes/UIEnergyBar.cpp:88-114` send_key S/T/U→UISoftKey+`UseSkillPacket(id,lv).dispatch()`、C/X deactivate [观察]

### UITemporaryStatView（192 行薄壳口径）
- `IO/UITypes/UITemporaryStatView.h:2,19-21` 逐字「W5 TemporaryStatView — 本地 buff 明细板（与 UIBuffList 分工：列表取消 vs 明细）」；`TYPE=TEMPORARYSTATVIEW` / `FOCUSED=false/TOGGLED=true` [观察]
- `IO/UITypes/UITemporaryStatView.cpp:2` 逐字「原生节点仅 32×32 图标；明细列表本地绘制（宋体/原色）」；`:26-29` `title_label(…, "临时属性")`（硬编码中文）/ `position=(16,80); dimension=(220,200);` [观察]
- `IO/UITypes/UITemporaryStatView.cpp:32-38,42-117` `window("TemporaryStatView")` 取 root["1"] 图标；rebuild_rows 按 skillid 聚合 Buffstat、名称取 `ItemData::get_name` / `SkillData::get_name` / `#`+id，`snprintf("%s  x%d  %ds")`，`h=28+rows*16` 钳 [64,280] 后 `dimension=(220,h)` [观察]
- `IO/UITypes/UITemporaryStatView.cpp:109` 逐字「A11M = 宋体 11；WHITE 对齐原生明细字色」；`:121` 逐字「无 buff 时不渲染（避免一块空的半透明黑板叠在场景/小地图上）」；`:125-127` `ColorBox(220,h,BLACK,0.55f)` [观察]
- `IO/UITypes/UITemporaryStatView.cpp:143-186` update 本地倒计时 / send_key X/Esc deactivate；`IO/UIStateGame.cpp:94-95` 构造即 `tsv->deactivate()` [观察]

### UIStatusMessenger（125 行）
- `IO/UITypes/UIStatusMessenger.h:41-42,49-51,65-67` 逐字「8 seconds」+ `FADE_DURATION=8'000`；`TYPE=STATUSMESSENGER` / `FOCUSED=false/TOGGLED=false`；`MAX_MESSAGES=6` / `std::deque<StatusInfo>` [观察]
- `IO/UITypes/UIStatusMessenger.cpp:26-48` StatusInfo `A12M RIGHT text(color)` + BLACK shadow；draw(+1,+1)；update FADE_STEP [观察]
- `IO/UITypes/UIStatusMessenger.cpp:58-67` draw 每条上移 `infopos.shift_y(-14)`；`:75-80` `int16_t y_adj=(new_width>800)?37:0; position=(new_width-6, new_height-145+y_adj);` [观察]
- `IO/UITypes/UIStatusMessenger.cpp:87-125` send_key S/T→SoftKey show_status(YELLOW)、C/X clear、R→RED tip、W→WHITE tip；show_status `push_front` + 超 6 `pop_back` [观察]

### UIMiniMap（BtMin/BtMax/BtMap / simpleMode / kMaxCanvasEdge / has_map 回退）
- `IO/UITypes/UIMiniMap.h:29-34` `UIDragElement<PosMINIMAP>` / `TYPE=MINIMAP` / `FOCUSED=false/TOGGLED=true`；`:78-86` `enum Buttons { BT_MIN, BT_MAX, BT_SMALL, BT_BIG, BT_MAP, BT_NPC }`；`:88-93` `enum Type { MIN, NORMAL, MAX }`；`:99-101` `bool simpleMode; bool big_map; bool has_map;` [观察]
- `IO/UITypes/UIMiniMap.cpp:44-55` 构造 `Point(128,20)`、`big_map=true` / **`has_map=false`** / `listNpc_dimensions=(150,170)`；`type=Setting<MiniMapType>::get().load()` / `simpleMode=Setting<MiniMapSimpleMode>::get().load()` [观察]
- `IO/UITypes/UIMiniMap.cpp:57-67` `UIWindow2.img` 回退 `UIWindow.img`；`node = simpleMode ? "MiniMapSimpleMode" : "MiniMap"` 经 `UiAssetResolve::window(node.c_str())` 取；simpleMode 缺件再回退 `window("MiniMap")`；`listNpc = window("MiniMap")["ListNpc"]`、`MapHelper = Map["MapHelper.img"]` [观察]
- `IO/UITypes/UIMiniMap.cpp:71-76` 缺 MiniMap → `active=false; dimension=(0,0); return;` [观察]
- `IO/UITypes/UIMiniMap.cpp:80-104` **三颗钮**：`BtMap` 来自 `UIWindow.img/MiniMap/BtMap`（逐字「「大地图」钮（36x12 橙底白字）…语义是打开世界地图」）；`BT_MIN`/`BT_MAX` 用全包通用 `Basic.img/BtMin`(蓝−)/`BtMax`(灰+)，逐字「12x12、normal/pressed/mouseOver/disabled 四态齐全，聊天栏已按同一口径绑定（UIChatBar.cpp:183-184）…MiniMap 子树下没有这两颗钮，但它们本就不是 MiniMap 专属资源」；缺件走 `UiAssetResolve::diag_once("minimap btmap/btmin/btmax", …)` [观察]
- `IO/UITypes/UIMiniMap.cpp:107-109` `region_text/town_text = OutlinedText(A12B, LEFT, WHITE, BLACK)`、`combined_text = Text(A12M, LEFT, BLACK)` [观察]
- `IO/UITypes/UIMiniMap.cpp:196-207` **kMaxCanvasEdge 逐字注释**「A tutorial map can ship a giant miniMap canvas bitmap (e.g. 5944x1311) that freezes the 8192 atlas upload on the main thread right after enter-game (black screen + Not Responding). Guard ONLY the canvas bitmap pixels (bmp_w/bmp_h) with the 2048 edge limit.」「miniMap["width"]/["height"] are the map's WORLD extents (Henesys = 7444x1391), NOT the canvas bitmap size (Henesys canvas = 465x86). Comparing those world extents to 2048 used to reject virtually every normal map: has_map=false collapsed the mini-map into the MIN strip — no region icon, no two-line label, no scene preview.」 [观察]
- `IO/UITypes/UIMiniMap.cpp:208` `constexpr int32_t kMaxCanvasEdge = 2048;`（局部常量，非类成员）；`:209-233` `canvas_w/canvas_h` 取 world extents、`bmp_w/bmp_h` 取 `miniMap["canvas"]` 位图真尺寸；`meta_bad`/`bmp_bad`/`canvas_too_large` 三判据 [观察]
- `IO/UITypes/UIMiniMap.cpp:235-250` **has_map 回退**：`if (!miniMap || canvas_too_large) { has_map=false; type=Type::MIN; LOG(LOG_WARN,"UIMiniMap: skip oversized canvas …"); } else { has_map=true; }` [观察]
- `IO/UITypes/UIMiniMap.cpp:592` `b->set_state(has_map ? NORMAL : DISABLED);`（缩放钮按 has_map 禁/启）；`:748-750,792,800,812,850` update_canvas/draw 各处以 `has_map` 决定画地图精灵还是退回 MIN 条 [观察]
- `IO/UITypes/UIMiniMap.cpp:482` 逐字「Setting<>::save 只改内存项，落盘必须显式 Configuration::get().save()。」；`:516` `// TODO: Toggle scrolling map`；`:666` 逐字「SMALL/BIG 未注册（本包无件，显式预留）时 small_w=0，不推进；」 [观察]
- `Configuration.h:966-982` `MiniMapType("minimap.type","2")` / `MiniMapSimpleMode("minimap.simple","false")` / `MiniMapDefaultHelpers("minimap.default_helpers","false")` [观察]

## B. `IO/UIStateGame.cpp:70-108` HUD 装配顺序逐行
- `IO/UIStateGame.cpp:70-76` 构造签名 `UIStateGame::UIStateGame() : stats(...), dragged(nullptr)` + `focused=NONE` / `tooltipparent=NONE` / 取 `CharLook& look`、`Inventory& inventory` [观察]
- `IO/UIStateGame.cpp:78` ① `emplace<UIStatusMessenger>();`（无条件，最先） [观察]
- `IO/UIStateGame.cpp:80-89` ② 条件 `if (UiAssetResolve::has_game_hud())`（逐字「Prefer StatusBar3; otherwise classic StatusBar.img (BeiDou / v83).」）→ `:83 emplace<UIStatusBar>(stats);` `:84 emplace<UIChatBar>()`；else `:88 LOG(LOG_WARN, "UIStateGame: no StatusBar3.img or StatusBar.img/base — skipping status/chat HUD");` [观察]
- `IO/UIStateGame.cpp:91` ③ `emplace<UIMiniMap>(stats);`（无条件）；`:92` ④ `emplace<UIBuffList>();` [观察]
- `IO/UIStateGame.cpp:93-95` ⑤ `emplace<UITemporaryStatView>();` 紧跟 `if (auto tsv = UI::get().get_element<UITemporaryStatView>()) tsv->deactivate();` [观察]
- `IO/UIStateGame.cpp:96` ⑥ `UIEnergyBar`；`:97` ⑦ `UIComboCounter`；`:98` ⑧ `UIBossHP`；`:99` ⑨ `UIClock` [观察]
- `IO/UIStateGame.cpp:101-104` 逐字「Shop window is inactive until an NPC opens it; still construct only when shop UI assets exist so missing Shop2 cannot affect enter-game.」+ `if (UiAssetResolve::usable(UiAssetResolve::shop_root())) emplace<UIShop>(look, inventory);`（非 HUD） [观察]
- `IO/UIStateGame.cpp:106-107` `VWIDTH/VHEIGHT = Constants::Constants::get().get_viewwidth()/get_viewheight()` [观察]
- `IO/UIStateGame.cpp:139-152` update() 视口变化时 `remove(STATUSBAR)` 再 `emplace<UIStatusBar>` 并把 CHATBAR 移 `elementorder` 尾部；逐字「视图尺寸一变就把 STATUSBAR 重新 emplace（push_back 到队尾），它反过来吞掉整条底栏 y>=649 的点击 —— 真机实测「+」展开按钮点了没反应。重排回原生优先级。」 [观察]
- `IO/UIStateGame.cpp:799-826` `emplace` 模板：`:802 if (auto iter = pre_add(T::TYPE, T::TOGGLED, T::FOCUSED))`、`:804-806 (*iter).second = std::make_unique<T>(std::forward<Args>(args)...);`、`:808-820 silent_types` 11 类（STATUSMESSENGER/STATUSBAR/CHATBAR/MINIMAP/BUFFLIST/ENERGYBAR/COMBOCOUNTER/BOSSHP/CLOCK/NPCTALK/SHOP；**不含** TEMPORARYSTATVIEW/QUICKSLOTCONFIG/SOFTKEYBOARD）、`:822-826` 非 silent 才播音效 + WORLDMAP 特例 [观察]

## C. `IO/UIStateGame.cpp:226-720` 触发窗里与 HUD 相关的分支（`send_key` 的 MENU 分支为主）
- `IO/UIStateGame.cpp:226-229` 右键 `emplace<UIContextMenu>()` + `open_at(pos, 0, player.get_name())` [观察]
- `IO/UIStateGame.cpp:231-249` `send_key` 开头焦点分发 + 脏焦点清理；逐字「关掉背包后按第一下 `i` 打不开、要按第二下，就是这条。」 [观察]
- `IO/UIStateGame.cpp:251-259` **ESC 兜底**：逐字「s9 的 KEYMAP 是从库里装载的，该账号没存 ESC 那一格（KeyConfig::Key::ESCAPE = 84）时下发的是 type=0/action=0 ⇒ mapping 为 NONE，下面那条 MAINMENU 分支永远进不来…只在 classic 底栏下兜，避免改到 StatusBar3 的 ESC 语义。」→ `if (escape && pressed && type==NONE && UiAssetResolve::is_classic_hud()) { type=MENU; action=MAINMENU; }` [观察]
- `IO/UIStateGame.cpp:269-272` 逐字「===== §f-keymap 遗留五死键接线（2026-11-06）===== 本枚举 21+ 段沿用新版编号，v83 的 11/17/25/26 透传后落在 WHISPER/GUILD/MANAGELEGION(MEDALS槽)/MEDALS(FAMILY槽)，原先无 case ⇒ 键发了、没反应。窗口本体全部已存在。」 [观察]
- `IO/UIStateGame.cpp:273-288` `WHISPER` → `UIWhisperTargetDlg` makeactive/open；逐字「v83 H(11) = WHISPER：原生「请输入悄悄话的对象」弹层；其确认回调 = ChatBar::set_whisper_target（切密语频道+预置 /w）。与底栏密语钮、下拉选「悄悄话」同一条路（pick_classic_target）。」 [观察]
- `IO/UIStateGame.cpp:310-317` `NOTIFIER`：逐字「查证：s9 SendOpcode 无任何 NOTIFIER/SERVERNOTICE 窗口化下发（18 号在 s9 未使用），原生通知窗内容源 = notifier 列表封包 —— 服务端不存在 ⇒ 按门禁 7 不做假窗。通知文案实际全走 serverNotice/chat（已接）。保留灭活并留证。」+ `LOG(LOG_DEBUG, "UIStateGame: NOTIFIER key (v83 L) — no s9 notifier backend, intentionally inert")` [观察]
- `IO/UIStateGame.cpp:426-435` `MINIMAP` → `UIMiniMap::send_key(action,pressed,escape)`，对象不存在则 `emplace<UIMiniMap>(stats)`；逐字「M 关过窗体后对象可能随换图销毁：重建即回到 MIN 态。」 [观察]
- `IO/UIStateGame.cpp:445-470` `KEYBINDINGS` → 先取 `nl::nx::UI["StatusBar3.img"]["KeyConfig"]` 再回退 `window("KeyConfig")`，然后 `emplace<UIKeyConfig>(inventory,skills)` / `keyconfig->close()` [观察]
- `IO/UIStateGame.cpp:471-482` `SAY/PARTYCHAT/FRIENDSCHAT/GUILDCHAT/ALLIANCECHAT/TOSPOUSE` → `if (auto chatbar = UI::get().get_element<UIChatBar>()) chatbar->change_target(action);` → [推断] 落点是 `IO/UITypes/UIChatBar.h:123` 的 `void change_target(int32_t action) {}` **空 stub**，与 `IO/UITypes/UIUserList.cpp:916`「v83 家族聊天由聊天框目标/斜杠别名路由；change_target 仍是预留空实现，」互证；这 6 个频道快捷键实际靠 UIChatBar 的 chatTarget 下拉/斜杠命令生效，非此分支 [观察+推断]
- `IO/UIStateGame.cpp:483-489` `MENU` → `statusbar->toggle_menu()`；`:490-496` `QUICKSLOTS` → `statusbar->toggle_qs()` [观察]
- `IO/UIStateGame.cpp:497-512` `TOGGLECHAT` → `chatbar->has_input()` 时转发 send_key；classic 无 BtMax/BtMin 走 `chatbar->toggle_input(true)`；否则 `toggle_view()`；逐字「classic HUD 没有 BtMax/BtMin，Enter 就是"开输入行"；原来这里调 toggle_view() 只翻历史框，收框后再也开不回来。」 [观察]
- `IO/UIStateGame.cpp:513-516` `CASHSHOP` → `EnterCashShopPacket().dispatch()` [观察]
- `IO/UIStateGame.cpp:530-535` `EVENT` → `emplace<UISpecial>()`；逐字「095 自定义中文 UI：暂时复用 EVENT 键打开福利中心（后续补独立 SPECIAL 键位 + 服务端 keymap）。」（**当前端源码现存注释，如实记录；本次校准不以 095 为依据**） [观察]
- `IO/UIStateGame.cpp:551-575` `MAINMENU`：classic → `UIGameMenu` 开/关（逐字「底栏「目录」钮走的是 UIGameMenu 开/关，这里复用同一条路。」）；非 classic → `statusbar->send_key(action,pressed,escape)`；并记「旧实现只把 MAINMENU 转给 UIStatusBar::send_key，而那个 send_key 里没有 MAINMENU 分支…classic 底栏下 ESC 是条死线」 [观察]
- `IO/UIStateGame.cpp:576-584` `SCREENSHOT` → `Window::get().request_screenshot()` [观察]
- `IO/UIStateGame.cpp:607-697` `send_cursor`：focused 元素优先、否则 `get_front` 反向扫 `elementorder`、点击时把命中元素 `remove` 后 `push_back` 到队尾（HUD 点击优先级核心） [观察]
- `IO/UIStateGame.cpp:699-708` `send_scroll` 对 `elementorder` 内每个 active 元素广播 `send_scroll(yoffset)`（UIChatBar/小地图共用） [观察]
- `IO/UIStateGame.cpp:710-722` `send_close`：`UIWindow6.img/askReward` 可用→`UIQuit`，否则 `UIQuitConfirm`；逐字「缺失时只画黑色遮罩、没有退出提示（用户实测反馈）。退回退出确认弹窗。」 [观察]

## D. `IO/UITypes/UIStatusBar.h:103-154` `Buttons` 枚举全表（id→数值）
**枚举无显式赋值，`uint16_t` 从 0 顺序递增，数值按文件行序推算 [推断]**；`IO/UITypes/UIStatusBar.h:140` 逐字「// v83 StatusBar.img direct keys」。
- 主/快捷 8 项：`:105` BT_CASHSHOP=0、`:106` BT_MENU=1、`:107` BT_OPTIONS=2、`:108` BT_CHARACTER=3、`:109` BT_COMMUNITY=4、`:110` BT_EVENT=5、`:111` BT_FOLD_QS=6、`:112` BT_EXTEND_QS=7 [观察+推断]
- BT_MENU_* 11 项：`:113` QUEST=8、`:114` MEDAL=9、`:115` UNION=10、`:116` MONSTER_COLLECTION=11、`:117` AUCTION=12、`:118` MONSTER_LIFE=13、`:119` BATTLE=14、`:120` ACHIEVEMENT=15、`:121` FISHING=16、`:122` HELP=17、`:123` CLAIM=18 [观察+推断]
- BT_SETTING_* 5 项：`:124` CHANNEL=19、`:125` OPTION=20、`:126` KEYS=21、`:127` JOYPAD=22、`:128` QUIT=23 [观察+推断]
- BT_COMMUNITY_* 4 项：`:129` FRIENDS=24、`:130` PARTY=25、`:131` GUILD=26、`:132` MAPLECHAT=27 [观察+推断]
- BT_CHARACTER_* 5 项：`:133` INFO=28、`:134` STAT=29、`:135` SKILL=30、`:136` EQUIP=31、`:137` ITEM=32 [观察+推断]
- BT_EVENT_* 2 项：`:138` SCHEDULE=33、`:139` DAILY=34 [观察+推断]
- BT_CLASSIC_* 13 项：`:141` SHOP=35、`:142` MENU=36、`:143` NPT=37、`:144` SHORT=38、`:145` WHISPER=39、`:146` CLAIM=40、`:147` EQUIP=41、`:148` INV=42、`:149` STAT=43、`:150` SKILL=44、`:151` KEYSET=45、`:152` QS=46、`:153` QSD=47 [观察+推断]
- 共 48 项（0..47）。[推断] 这套是**客户端 UI 内部 button id**，与 `IO/UITypes/UIStatusBar.cpp:1901-1907` 记录的 IDA CreateCtrl 原生 ctrl id（BtShop=1000/(573,543)、EquipKey=2000/(618,515)…QuickSlotD=2006）**不是同一套编号**；`init_classic` 用 `buttons[BT_CLASSIC_*]` 装 MapleButton、坐标才取 CreateCtrl 网格 [观察+推断]
- `IO/UITypes/UIStatusBar.cpp:91-93` 逐字「buttons 是 map，用 operator[] 只读访问会插入空 unique_ptr…classic HUD 只建 BT_CLASSIC_* 那一组，modern 的 27 个 submenu 按钮根本不存在」；`:1191` 逐字「修复：buttons 可能有缺失条目，禁止 operator[] 只读访问（会插入空 unique_ptr 再解引用）。」 [观察]

## E. `IO/UIElement.h` 的 `Type` 绑定（HUD 全表）
`IO/UIElement.h:41-204` `enum Type` 无显式赋值，从 `NONE=0` 顺序递增；数值按文件行序推算 [推断]，绑定语句本身 [观察]。
- `IO/UITypes/UIStatusMessenger.h:49` → `IO/UIElement.h:58` STATUSMESSENGER=15；`IO/UITypes/UISoftKey.h:45` → `:55` SOFTKEYBOARD=12；`IO/UITypes/UIEnergyBar.h:14` → `:70` ENERGYBAR=27；`IO/UITypes/UIComboCounter.h:15` → `:71` COMBOCOUNTER=28；`IO/UITypes/UIBossHP.h:15` → `:72` BOSSHP=29；`IO/UITypes/UIClock.h:16` → `:73` CLOCK=30 [观察+推断]
- `IO/UITypes/UIMiscShells.h:879` → `IO/UIElement.h:147` MOBGAGE=102（有 Type 值但无 UIMobGage.* 文件）；`:154` WHISPERTARGET=108（逐字「悄悄话目标弹层（原生「请输入悄悄话的对象。」那一层）。」） [观察+推断]
- `IO/UITypes/UIStatusBar.h:35` → `IO/UIElement.h:155` STATUSBAR=109；`IO/UITypes/UIChatBar.h:39` → `:156` CHATBAR=110；`IO/UITypes/UIBuffList.h:68` → `:157` BUFFLIST=111；`IO/UITypes/UITemporaryStatView.h:19` → `:158` TEMPORARYSTATVIEW=112；`:159` NOTICE=113、`:160` NPCTALK=114、`:161` SHOP=115 [观察+推断]
- `IO/UITypes/UIMiniMap.h:32` → `IO/UIElement.h:169` MINIMAP=123；`IO/UITypes/UIQuickslotConfig.h:20` → `:177` QUICKSLOTCONFIG=131 [观察+推断]
- `IO/UIElement.h:203` `NUM_TYPES`=150；`:195-199` 逐字「v83 目录气球的两颗独立设置窗（游戏设置 / 系统设置）。本包无 StatusBar3.img/OptionMenu，UIOptionMenu 只剩空壳，故另立两型。注意：这两项让 UIElement::Type::NUM_TYPES 变大，而 EnumMap<Type, UPtr, NUM_TYPES> 是 UIStateGame 的成员 —— 实例尺寸变了，必须整工程 Rebuild（增量编译会留下按旧尺寸编译的目标文件）。」 [观察+推断]

## F. 点名类 stub / 预留 / TODO 逐字（补充 r04 第 5 组）
- `IO/UITypes/UIChatBar.h:31` / `:122` `// TODO: Change these?`；`:123` `void change_target(int32_t action) {}` 空实现；`:413` 逐字「Party/Guild/Alliance/Buddy：服务端广播显式排除发送者 → 必须本地回显；」 [观察]
- `IO/UITypes/UIChatBar.cpp:1024/1070/1120/1139/1164/1198` 六处 `// TODO: The top gets shifted by a pixel` / `// TODO: The right gets shifted by a pixel`；`:1701`「是 MapleButton，由 UIElement::draw 画，只能显式 deactivate。」；`:2365`「Q5 回显矩阵：服务端 partyChat 显式排除发送者（World.java」；`:2463`「与原生「没开过好友列表发 /b 无人收到」一致，不假装能发。」；`:2512` `default: break; // WHISPER 等由调用方给显式前缀`；`:2574`「显式预留：v83 下拉里没有「喇叭」项，原生喇叭入口是道具使用；」；`:884`「里仍是 0），所以手势结束这一处显式存一次，保证「拖完记住」成立。」 [观察]
- `IO/UITypes/UIStatusBar.cpp:860`「再点一次「界面」必须收起。旧实现在这里什么都不做，」；`:1879`「快捷栏面板在下方按格数选型后赋值，此处不提前占位。」；`:2501`「不再「数值一变就闪」—— 那是旧占位行为」；`:2507`「无 hpFlash/mpFlash 节点，本设置对其无观感 —— 已记入 evidence §4 预留。」 [观察]
- `IO/UITypes/UIMiniMap.cpp:482/516/666` 见 A 节三条逐字；`IO/UITypes/UISoftKey.cpp:400` `// TODO: Add check for required amount of characters`（紧邻 `bool requirements=false;`）；`IO/UITypes/UIComboCounter.cpp:34-36`「因此只降级为纯文字计数，不再打成功日志。」；`IO/UITypes/UIUserList.cpp:916`「v83 家族聊天由聊天框目标/斜杠别名路由；change_target 仍是预留空实现，」 [观察]

## G. 两个明确回答

### G1. 当前端是否把 v83 原生单体 CUIStatusBar 拆成多类、各自承担哪部分职责
**是** [推断，依据 `IO/UIStateGame.cpp:78-99` 的 10 次独立 `emplace` + 各类独立 `static constexpr Type TYPE` 绑定（E 节）+ 各自独立 .h/.cpp]。共 **10 个构造期 HUD 类 + 2 个按需类**：

| 类 | TYPE 绑定 | 从原生单体剥离的职责 | 证据 |
|---|---|---|---|
| UIStatusBar | `UIElement.h:155` STATUSBAR | 底栏主体：**HP/MP/EXP 条**、等级/名字芯片、**快捷栏**、13 classic 按钮、modern 27 submenu、商城/装备/统计/技能/键盘入口、HP/MP 警报 | `IO/UITypes/UIStatusBar.cpp:536-676,747-1176,2233-2541` |
| UIChatBar | `UIElement.h:156` CHATBAR | **聊天输入/历史/收起灰带/8 频道下拉/斜杠命令/名字点击菜单/Slider/拖拽改高** | `IO/UITypes/UIChatBar.cpp:139-165,341-478,2313-2583` |
| UIMiniMap | `UIElement.h:169` MINIMAP | 小地图三态/地图名/NPC 列表/世界地图入口 | `IO/UITypes/UIMiniMap.cpp:120-183,490-544` |
| UIBuffList | `UIElement.h:157` BUFFLIST | buff 图标条带 + 倒计时 + 右键取消 | `IO/UITypes/UIBuffList.cpp:66-96,224-250` |
| UITemporaryStatView | `UIElement.h:158` TEMPORARYSTATVIEW | buff 明细板（与 UIBuffList 分工：列表取消 vs 明细），构造即停用 | `IO/UIStateGame.cpp:94-95`、`UITemporaryStatView.cpp:119-141` |
| UIEnergyBar | `IO/UIElement.h:70` ENERGYBAR | ENERGY_CHARGE 能量条 | `IO/UITypes/UIEnergyBar.cpp:45-86` |
| UIComboCounter | `IO/UIElement.h:71` COMBOCOUNTER | Aran SHOW_COMBO 连击计数 | `IO/UITypes/UIComboCounter.cpp:40-61` |
| UIBossHP | `IO/UIElement.h:72` BOSSHP | FIELD_EFFECT mode 5 Boss 血条 | `IO/UITypes/UIBossHP.cpp:81-106` |
| UIClock | `IO/UIElement.h:73` CLOCK | CLOCK/STOP_CLOCK 时钟/倒计时 | `IO/UITypes/UIClock.cpp:26-79` |
| UIStatusMessenger | `IO/UIElement.h:58` STATUSMESSENGER | 右上浮公告(6 条×8s 淡出) | `IO/UITypes/UIStatusMessenger.cpp:58-80` |
| UISoftKey（按需） | `IO/UIElement.h:55` SOFTKEYBOARD | 虚拟键盘，被各 HUD `send_key`/按钮 emplace 复用作输入器 | `IO/UITypes/UISoftKey.cpp:62-117` |
| UIQuickslotConfig（按需） | `IO/UIElement.h:177` QUICKSLOTCONFIG | 8 槽改键窗 → CHANGE_QUICKSLOT(183) | `IO/UITypes/UIQuickslotConfig.cpp:125-144` |

要点 [推断]：
- **HP/MP/EXP 条 + 快捷栏 + 菜单 + 按钮并未继续拆，仍集中在 `UIStatusBar` 单类**，只是该类内部再分 modern(StatusBar3) / classic(StatusBar.img) 两套实现（`IO/UITypes/UIStatusBar.cpp:189-198` `classic_mode = UiAssetResolve::is_classic_hud()`）。
- **聊天面彻底独立成 `UIChatBar`**，与状态栏靠 `IO/UIStateGame.cpp:139-152` 的 elementorder 重排协调点击优先级（避免 STATUSBAR 吞掉底栏 y>=649 点击）。
- **浮公告拆成 `UIStatusMessenger`**，另有全宽 23px `ScrollingNotice`（`IO/Components/ScrollingNotice.cpp:22-30`）由 `UI::draw` 画在 state 之上。
- `UITemporaryStatView` 默认停用、`UIMobGage.*` 未找到（`UIBossHP.cpp:31-33` 仅把 `"MobGage"` 当资源节点回退名）。

### G2. 是否仍留 `[DBG-CLK]` 写 `_clk.log`
**否，当前源码已无该调试残留** [观察]。证据：
- 全工程 grep `DBG-CLK|_clk\.log|_clk|clk\.log` → **仅命中 5 个 docs 文件，源码零命中**：`docs/features/beidou-v17-parity/notes/r15-baseline-ui-gap-vs-screenshots.md:37,131,140,151`、`docs/features/beidou-v17-parity/notes/r19-w0-baseline-ui-impl.md:16`、`docs/features/feature-gap-inventory-2026-11/README.md:111`、`docs/features/statusbar-hud-angel-calibration-2026-11/notes/r03-prior-hud-doc-audit-and-cancel.md:55-56,82,188`。
- 对 `IO/UITypes/UIStatusBar.cpp` 单独 grep `DBG-CLK|_clk|ofstream|fopen|freopen|\.log|#include <fstream>` → **No matches found**（文件内既无 `[DBG-CLK]` 字符串也无任何文件写流/日志落盘头文件） [观察]。
- `docs/features/statusbar-hud-angel-calibration-2026-11/notes/r03-prior-hud-doc-audit-and-cancel.md:188` 逐字「4. 四按钮回调落地…确认 `[DBG-CLK]` 可删。→ 已销案（`UIStatusBar.cpp:747`）。」 [观察]
- [推断] docs 里的 `[DBG-CLK]` / `_clk.log` 是历史基线记录与待办清单措辞，代码侧已删除；README 若沿用 r15/r19 旧表须标「已销案」，不要写成当前源码仍存在。

（增量补充包完 · 只读审计，未改任何文件）
