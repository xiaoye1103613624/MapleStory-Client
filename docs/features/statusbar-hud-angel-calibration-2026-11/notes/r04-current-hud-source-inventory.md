# r04 — 状态栏 HUD 面「当前端源码现状盘点」（只读归档）

> 来源：subagent `e0827232` 只读取证全文（2026-11-08 19:25，14K），主 agent 结构化归档。全程未改产品源码。
> 门禁：`xiaoye-maple-story-dev` 已加载；参照基准 083/v83；未引 095 线。
> 标注 `[观察]` = 源码/配置直读；`[推断]` = 据直读推导。

## 0. 文件存在性 [观察]
`IO/UIStateGame.cpp`、`IO/UITypes/` 下 12 个 HUD 类的 `.h`+`.cpp`（UIStatusBar / UIChatBar / UIBuffList / UITemporaryStatView / UIEnergyBar / UIComboCounter / UIBossHP / UIClock / UIMiniMap / UIStatusMessenger / UISoftKey / UIQuickslotConfig）、`Configuration.h/.cpp`、`locale/zh-CN.ini`、`IO/Components/*` 全部存在。
`IO/UITypes/UIMobGage.*` **未找到**（glob No files found）；`IO/UITypes/UIBossHP.cpp:31-33` 把 `"MobGage"` 仅当资源节点回退名，不存在同名类文件。

## 1. `IO/UIStateGame.cpp` 构造期 HUD 装配 [观察]
- `:78` 无条件 `emplace<UIStatusMessenger>();`（浮公告先建）
- `:80` 注释 `// Prefer StatusBar3; otherwise classic StatusBar.img (BeiDou / v83).`
- `:81` `if (UiAssetResolve::has_game_hud())` 包住状态栏+聊天栏
- `:83` `emplace<UIStatusBar>(stats);` / `:84` `emplace<UIChatBar>();`
- `:88` 条件失败 `LOG(LOG_WARN, "UIStateGame: no StatusBar3.img or StatusBar.img/base — skipping status/chat HUD");`
- `:91` `emplace<UIMiniMap>(stats);` / `:92` `emplace<UIBuffList>();` / `:93` `emplace<UITemporaryStatView>();`
- `:94-95` `if (auto tsv = ...) tsv->deactivate();` 构造即停用
- `:96-99` `emplace<UIEnergyBar>()` / `UIComboCounter` / `UIBossHP` / `UIClock`（无条件）
- `:101-104` shop 默认停用，`if (UiAssetResolve::usable(UiAssetResolve::shop_root())) emplace<UIShop>(look, inventory);`
- `:106-107` `VWIDTH/VHEIGHT` 取自 `Constants`
- `:139-152` update() 尺寸变化时 `remove(STATUSBAR)` 再 `emplace`，并把 CHATBAR 移到 `elementorder` 尾部；注释「STATUSBAR 重新 emplace 后吞掉整条底栏 y>=649 点击、'+' 展开按钮点了没反应 → 重排回原生优先级」
- `:163` `element->refresh_localized();`
- `:802` `pre_add` 机制 `if (auto iter = pre_add(T::TYPE, T::TOGGLED, T::FOCUSED))`
- `:804-806` `(*iter).second = std::make_unique<T>(std::forward<Args>(args)...);`
- `:808-820` `silent_types` 11 类 = {STATUSMESSENGER, STATUSBAR, CHATBAR, MINIMAP, BUFFLIST, ENERGYBAR, COMBOCOUNTER, BOSSHP, CLOCK, NPCTALK, SHOP}；**不含** TEMPORARYSTATVIEW / QUICKSLOTCONFIG / SOFTKEYBOARD
- `:822-826` 非 silent 才播开窗音效，`T::TYPE==WORLDMAP` 特例
- `[推断]` 构造期共 10 个 HUD 元素；UISoftKey/UIQuickslotConfig 不在此处构造、按需 emplace。

## 2. 逐 HUD 类

### 2.1 UIStatusBar（2542 行 .cpp / 237 行 .h）
- `IO/UITypes/UIStatusBar.h:35-37` `TYPE=STATUSBAR / FOCUSED=false / TOGGLED=true`
- `:39-46` `enum MenuType { MENU, SETTING, COMMUNITY, CHARACTER, EVENT };`
- `:69-70` `QUICKSLOT_MAX=211` / `CLASSIC_BAR_H=71`
- `:85` `activate_classic_qs_slot(size_t slot)` 注释「按 KeyConfig 映射释放技能/使用道具（对齐参考端 OnButton 快捷栏）」
- `:91-93` `button_at` 注释「buttons 是 map，operator[] 只读会插入空 unique_ptr；classic HUD 只建 BT_CLASSIC_*，modern 的 27 个 submenu 按钮根本不存在」
- `:103-154` `enum Buttons`：6 主钮(CASHSHOP/MENU/OPTIONS/CHARACTER/COMMUNITY/EVENT)、FOLD_QS/EXTEND_QS、11 BT_MENU_*、5 BT_SETTING_*、4 BT_COMMUNITY_*、5 BT_CHARACTER_*、2 BT_EVENT_*、13 BT_CLASSIC_*(SHOP/MENU/NPT/SHORT/WHISPER/CLAIM/EQUIP/INV/STAT/SKILL/KEYSET/QS/QSD)
- `:158-168` 成员 Gauge expbar/hpbar/mpbar、Charset statset/hpmpset/levelset、Texture quickslot[2]/menutitle[5]/menubackground[3]、OutlinedText namelabel/joblabel
- `:171-180` W1 注释「量规数值改画 StatusBar.img/number 位图字库(5x7 白数字+2x9 黄绿括号)，LV 徽章改画 Basic.img/LevelNo 橙盒数字」；classic_levelno(11x13)/showlevel(5x12)/bracket(2x12)/exp_dot(1x1)/key_label[7]
- `:183-186` `CLASSIC_QS_SLOTS=26` / `classic_qs_cols=13`（「32 格=16 / 26 格=13 / 8 格=4」）
- `:195-200` 「掉血/掉蓝闪光：原生 5 帧瞬态动画(delay 130ms)，计数 -1=本包无素材/0=空闲/>0=剩余帧」
- `:235` `classic_qs_wide`（true=26 格 quickSlot / false=8 格 quickSlot08）
- `IO/UITypes/UIStatusBar.cpp:85-99` `SEG_HP=0/SEG_MP=109/SEG_EXP=222`、`SEG_W_*=108/107/118`、`GAUGE_X=218/GAUGE_Y=38`
- `:101-118` `QS_SLOT=28/QS_PITCH_X=35/QS_PITCH_Y=34/QS_FIRST_X=9/QS_FIRST_Y=9/QS_ICON_SHIFT_Y=32`
- `:120-138` `CLASSIC_QS_KEYS[26]`（LEFT_SHIFT,INSERT,HOME,PAGE_UP,NUM1-5,A,S,D,F,LEFT_CONTROL,DELETE,END,PAGE_DOWN,Q,W,E,R,T,Z,X,C,V）+ KEYCAP/KEYNAME 表
- `:140-175` `hud_advance` / `draw_hud_number`；「number 字库无 '.' 字形，用 1x1 白点补」
- `:189-198` `classic_mode = UiAssetResolve::is_classic_hud()`，classic→init_classic()、否则 init_modern()
- `:208-234` modern `status_bar_root()["mainBar"]`、EXPBar 多分辨率兜底、expbar Gauge(DEFAULT)
- `:250-282` modern 800 与 ≥1024 两套坐标表(hpmp/hpset/mpset/statset/levelset/namelabel/quickslot/menu)
- `:328-340` hpbar/mpbar Gauge(DEFAULT,hpmp_max=139)、3 个 Charset、namelabel OutlinedText(A13M)
- `:345-361` modern 6 主按钮 `buttonPos=(591+pos_adj,73)`
- `:391-430` modern FOLD_QS/EXTEND_QS 坐标（800:(579,0)/1024:(627+adj,37)/1280:(621+adj,37)/1366:(623+adj,37)/1920:(900+adj,37)/fallback:621+(VW-1280)/2）
- `:441-486` modern `#pragma region Menu`：27 颗 submenu MapleButton(11/5/4/5/2) 全 `set_active(false)` + menutitle[5]
- `:488-533` modern position/dimension：800→(0,480)/(VW-410,140)；1024→(0,648)/…75；1280→(0,600)/…75；1366→(0,648)/…75；1920→(0,960+(VH-1080))/…80；fallback (0,VH-75)/(610,75)
- `:536-676` `draw()`：classic→draw_classic；modern 6 钮 + 三联 Gauge + `statset.draw(exp+"["+xx.xx+"%]")` + `hpmpset.draw("[hp/maxhp]"/"[mp/maxmp]")` + levelset + namelabel + quickslot[0/1] + 菜单三段 `end_y=floor(28.2*button_count)`
- `:678-699` `update()`：classic→update_classic；modern `expbar/hpbar/mpbar.update(...)` + namelabel
- `:747-1176` `button_pressed` 全分支（见 r03 §0 销案复核）：SHOP/CASHSHOP→EnterCashShopPacket；CLASSIC_MENU→UIGameMenu toggle；EQUIP/INV/STAT/SKILL/KEYSET→各 emplace；QS/QSD→`quickslot_active=!...; sync_classic_qs_buttons()`；NPT→EnterMtsPacket；SHORT→UIShortCut；WHISPER→`chatbar->focus_whisper()`；CLAIM→UISlideMenu；modern 各菜单→对应 UI；每分支后 `remove_menus()`
- `:1178-1437` `send_key`：Esc→toggle_setting/remove_menus；RETURN→btn_at lambda 触发；UP/DOWN 菜单内移动；W/S/E/C/H/X/D/K/R/I 调试键弹 UISoftKey 发包
- `:1627-1820` toggle_qs / button_at / set_buttons_active / toggle_menu|setting|community|character|event / remove_menus / remove_active_menu / sync_classic_qs_buttons
- `:1840-1865` getexppercent=`exp/ExpTable::required_exp(level)` / gethppercent / getmppercent
- `:1867-1899` `init_classic()` 取 base/gauge、zero_origin 清脏 origin；`:1879`「快捷栏面板在下方按格数选型后赋值，此处不提前占位」
- `:1900-1924` classic 权威注释：a) IDA sub_8D01B2 CreateCtrl 坐标(BtShop 1000/(573,543)…QuickSlotD 2006/(573,515)，底条相对 y=CreateY-507)；b) StatusBar.img 节点实测(backgrnd 817x71 / backgrnd2 570x71 / gauge 340x31 三联 / number 字库 / key/0..6 / quickSlot08 151x80、26 466x80、32 571x80)；c) 正确端截图底栏左贴边+右快捷格贴右缘
- `:1932-1959` classic hpmp_pos/exp_pos=(218,38)、hpset/mpset/statset_pos、lvbadge(50,47)、joblabel(87,40)/namelabel(87,56)；三联 `Gauge(CLIP, classic_gauge_bar, SEG_W_*,0)+set_clip_left(SEG_*)`
- `:1961-1998` hpFlash/mpFlash Sprite(5 帧)、statset/hpmpset=Charset(sb["number"])、Basic.img/LevelNo→classic_levelno、namelabel=A12M
- `:2002-2015` add_btn lambda `buttons[id]=make_unique<MapleButton>(node,(x+origin.x,y+origin.y))`
- `:2017-2052` 快捷栏选型 `wide=VWIDTH>=1024`→count=26/cols=13/quickSlot26，否则 count=8/cols=4/quickSlot08；`quickslot_pos=(hud_grid_w-qs_panel_w,-8)`、`group_x=quickslot_pos.x-(wide?241:75)`
- `:2055-2074` **13 颗 classic 按钮坐标**（相对 group_x）：SHOP(+0,36)/NPT(+56,36)/MENU(+112,36)/SHORT(+168,36)/EQUIP(+45,8)/INV(+75,8)/STAT(+105,8)/SKILL(+135,8)/KEYSET(+165,8)/QS(+195,8)/QSD(+195,8 同位互斥)/CLAIM(-15,8 条件)/WHISPER(+15,8 条件)
- `:2089-2101` classic_qs_keytext=OutlinedText(A11B,KEYCAPBLUE)、qs_label=Text(A11M,maxwidth=28)、`position=(hud_center_x, VH-71)` / `dimension=(VW,80)`
- `:2106-2160` `refresh_classic_qs_icons`：8 槽走 `Keyboard::get_quickslot_key(i)`（可被 CHANGE_QUICKSLOT 改）、26/32 走 CLASSIC_QS_KEYS 固定表；SKILL/ITEM/MENU-ACTION-FACE 取 icon+name
- `:2162-2231` `classic_qs_cell_at` / `classic_qs_slot_at`（Rectangle 28x28 命中）/ `activate_classic_qs_slot`→**`Stage::get().send_key(mapping.type, mapping.action, true)`** / `send_cursor` classic 先查 slot
- `:2233-2480` `draw_classic`：通栏 backgrnd(`DrawArgument((0,position.y),(VW,0))`)、backgrnd2、三段 CLIP 量规(gray/graduation)、5 帧闪光、LevelNo 等级、芯片两行(y_for_ink_top)、`draw_hud_number` 画 "[hp/maxhp]"/"[mp/maxmp]"、EXP 两行完整数值 + `"[%.2f%%]"` 右缘对齐、快捷栏面板+每格 icon/键帽/中文名
- `:2482-2541` `update_classic`：flash update、expbar.update、HP/MP 警报阈值(`Setting<SysOptHpWarn/MpWarn>::load()`，0=不报警，5 帧重放)、「不再数值一变就闪——那是旧占位行为」「现代皮肤无 hpFlash/mpFlash 节点，本设置对其无观感——已记入 evidence §4 预留」
- `[推断]` HP/MP/EXP 条、快捷栏、菜单、密语、浮公告入口全部集中在 UIStatusBar 单体，且分 modern(StatusBar3) / classic(StatusBar.img) 双实现。

### 2.2 UIChatBar（2673 行 .cpp / 479 行 .h，`UIDragElement<PosCHAT>`）
- `IO/UITypes/UIChatBar.h:39-41` `TYPE=CHATBAR/FOCUSED=false/TOGGLED=true`
- `:60-97` `enum MessageGroup{ALL..GROUP,LENGTH}` / `MessageType{UNK0,WHITE,RED,BLUE,YELLOW,GM}` / `enum class ChatChannel{ALL,WHISPER,PARTY,GUILD,BUDDY,ALLIANCE,SPOUSE,MEGAPHONE,SYSTEM,GM}`
- `:122-123` `// TODO: Change these?` + **`void change_target(int32_t action) {}` 空实现 stub**
- `:126-128` `INPUT_TEXT_HEIGHT=18 / MIN_HEIGHT=12 / MAX_HEIGHT=467`
- `:152-162` `enum Buttons { BtMax, BtMin, BtChat, BtHelp, BtItemLink, BtChatEmoticon, BtOutChat, BtWhisper }`
- `:221-227` 「原生无任何聊天日志上限常量…用户 2026-11-04 裁决按 GMS v83 常见值取 200」+ `MESSAGE_HISTORY_CAP=200` + `trim_message_history()`
- `:250-293` classic 布局常量 `CLASSIC_BAR_H=71/INPUT_BAND_TOP=4/INPUT_ROW_H=26/MAX_ROW_W=566/CHAT_TARGET_W=81/CHAT_TARGET_H=20/TARGET_BT_W=12/TARGET_BOX_Y=4/LOG_BT_X=536/LOG_BT_Y=8/FOLD_PANEL_TOP=3/FOLD_PANEL_H=23`
- `:302-341` 展开态 `CLASSIC_LOG_H_DEFAULT=80/MIN=40/MAX=420`、`SCROLL_W=15/SCROLL_INSET=11`、`LOG_FIRST_Y=23/LOG_LINE_H=13/GRIP_H=6`、Slider classic_scrollbar、ColorBox classic_fold_panel（「本包没有任何位图能拼出收起态那条灰带…按原生实测色自己铺不透明矩形」）
- `:361-416` 目标列表「原生 v83 chatTarget 列表就是全频道选择器；本端此前只有 4 项且发送完全不读它…扩到 8 项」+ `ChatTargetIndex{ALL,WHISPER,PARTY,GUILD,BUDDY,ALLIANCE,SPOUSE,MEGAPHONE,COUNT}` + Q5 回显矩阵 + `send_to_target_channel` / `echo_own_message`
- `:447-477` 名字点击菜单 `NameMenuItem{NM_WHISPER,NM_CHARINFO,NM_BUDDY,NM_COUNT}` / `NAME_MENU_W=132/ROW_H=20`
- `IO/UITypes/UIChatBar.cpp:56-113` classic 构造：取 `base["chat"]`/`base["chatTarget"]`；`user_view_x=560`、chat.y 越界回落 80、`position=(0,VH-71+4)` / `dimension=(min(VW,566),26)`、fold_w=dimension.x-SCROLL_W、`view_input=false`、`dragarea=(dimension.x,12)`
- `:119-133` `Slider(CHATBAR, Range(11,user_view_y-11), …)` + `setenabled(false)`
- `:139-165` **聊天输入 `Textfield(A11M,LEFT,BLACK,Rectangle((CHAT_TARGET_W+2,6)..(dimension.x-34,18)),70,11)`**（70 字上限/11 光标高）+ `set_text_shift(0,-9)` + `set_enter_callback(input_text_enter_callback)` + `set_key_callback(ESCAPE,input_text_escape_callback)`
- `:167` `show_message("[Welcome] Welcome to MapleStory!!", YELLOW)`（硬编码欢迎语）
- `:177-200` classic BtWhisper/BtMax/BtMin 经 `try_maple_btn` 绑定(BtWhisper(0,4)、BtMax/BtMin(536,8) 同位互斥) + classic_target_text(A11M,Locale::text("chat.target.all")) + 8 项 Text + 目标列表 ColorBox(灰沿/蓝内区/亮蓝选中条 85,153,187)
- `:207-215` modern 构造 diag「本包(v83 BeiDou)只有 classic StatusBar.img，没有 chat/ingame → modern 贴图/按钮会全部空掉」
- `:264-291` modern 5 颗 BtChat/BtItemLink/BtChatEmoticon/BtHelp/BtOutChat MapleButton(全 set_active(false)) + `input_text_limit=70` + Textfield(A11M,WHITE,283 宽)
- `:293-336` modern enter/ESCAPE/UP/DOWN 四回调 + `toggle_view(Setting<ChatViewMax>)` + `sync_classic_buttons()`
- `:341-478` `draw()` classic 分支：folded(灰带+最近一条) / 展开半透明黑底(α0.5,566xuser_view_y)+Slider / chatTarget 深蓝框(0x1B3159FF) / 8 行目标列表 / 下划线 classic_input_bg / BtMax|BtMin / 历史消息倒排(LOG_LINE_H=13) / `input_text.draw` / name_menu
- `:481-606` `draw()` modern 分支：max_textures 三段平铺、drag、历史行(13px)、input_textures、dragged 八向预览、min_textures
- `:625-729` `button_pressed`：BtMax/BtMin→classic 翻历史框 / modern toggle_view；BtChatEmoticon→UISoftKey+FaceExpressionPacket；BtChat→SpouseChat；BtHelp→EnterMtsPacket；BtWhisper→toggle_classic_target_list；BtOutChat→UIChat；BtItemLink→UseItem/SoftKey
- `:731-785` `is_in_range` classic 命中区修正
- `:787-1463` `send_cursor`：名字菜单 hover/select、可见消息名字热区、收起态点灰带展开、顶边 GRIP 拖拽改高(`set_classic_log_height`、松手 `Setting<ChatViewY>::save` + `Configuration::get().save()`)、目标列表行、Slider、chatTarget 框、Textfield、modern 八向拖拽
- `:1465-1584` `send_key`：Esc 先关目标列表、W→focus_whisper、S/P/Y/G/L/B/F→UISoftKey 各频道、TAB→complete_gm_command、RETURN→toggle_input、其它→`input_text.send_key(TEXT,index,pressed)`
- `:1613-1630` `chat_target_name`：8 键 KEYS[]→Locale::text（前四原生 v83、后四 NO-NATIVE）
- `:1836-1923` 3 个 `show_message` 重载（颜色映射 / `trim_message_history()` cap 200 / `channel_color` 10 频道色 / 名字可点热区量宽）
- `:1925-1998` 名字菜单：3 项文案**硬编码中文**「密语」「查看角色信息」「加好友」(A11M,WHITE)
- `:2000-2059` `focus_whisper`→UIWhisperTargetDlg / `set_whisper_target`→预填 "/w 名字 "
- `:2313-2447` `input_text_enter_callback`：空→escape、斜杠命令 /w /find /p /g /a /b /s 解析 + 本地回显矩阵、非命令按 `classic_chat_target_id` 走 `send_to_target_channel`、兜底 `GeneralChatPacket(message,true)`、GM '!'/'@' 回显、发送后 `keep_input_focus()`
- `:2452-2583` keep_input_focus / collect_online_buddy_cids / send_whisper / on_whisper_result / echo_own_message / `send_to_target_channel`（喇叭通道 `:2574`「显式预留：v83 下拉里没有喇叭项，原生喇叭入口是道具使用」→emplace UIMegaphoneBasic 返 CHANNEL_HELD）
- `:2585-2673` complete_gm_command / set_input / input_text_escape_callback / close_input / change_message（方向键历史）
- **聊天输入框 unicode** [观察]：`set_accept_unicode(true)` 全工程仅 `IO/UITypes/UIMapSearchPanel.cpp:189`、`IO/UITypes/UIMiscShells.cpp:1251/1549`、`IO/UITypes/UINpcTalk.cpp:1645`；`IO/UITypes/UIChatBar.cpp` 无调用。`IO/Components/Textfield.h:84` 默认 `accept_unicode=false`、`Textfield.cpp:281-321` 默认仅收 ASCII 32-127；`IO/UI.cpp:522` 依 `accepts_unicode()` 决定 IME 开关。→ [推断] 当前端聊天输入行仍限 ASCII、未开 unicode/IME。**⚠️ 与登录面 `IO/Window.cpp:901-925` 已 `SDL_StartTextInput` 不冲突：系统 IME 开着，但焦点进聊天 Textfield 时 `IO/UI.cpp:522` 依 `accepts_unicode()` 关掉 → 聊天框中文输入仍被拦。**

### 2.3 UIBuffList
- `IO/UITypes/UIBuffList.h:39-47` `FLASH_TIME=3000` / `ICON_SIZE=32` / `LABEL_TOP=21`；「倒计时画在 32x32 图标画布：顶 y=21、水平居中、黄正文+黑 1px 描边」「>=1h→%dh%dm / >=1m→%dm%ds / 否则 %ds」
- `:65-70` `TYPE=BUFFLIST/FOCUSED=false/TOGGLED=false`；`:81-87`「右键点掉增益=CancelBuffHandler(s9 只认 int sourceid)/命中判定必须自算」；`:101-102`「ui.buff_timer」+ `bool show_timer`
- `IO/UITypes/UIBuffList.cpp:66-96` draw 图标+IconCover+8 方向黑描边+黄正文；`:140-157` format_time 三档 snprintf；`:159-161` `show_timer(Configuration::get().get_buff_timer())`
- `:196-200` `update_screen`：`position=(new_width-35,55)` / `dimension=(position.x,32)`
- `:224-250` 右键→CancelItemEffectPacket / CancelBuffPacket(skillid)
- `:257-311` send_key S/T→UISoftKey 输 buffid→`add_buff(id,60000)`、C/X 清、D 切 UITemporaryStatView

### 2.4 UITemporaryStatView（构造即停用）
- `IO/UITypes/UITemporaryStatView.h:19-21` `TYPE=TEMPORARYSTATVIEW/FOCUSED=false/TOGGLED=true`
- `.cpp:2` 「原生节点仅 32×32 图标；明细列表本地绘制（宋体/原色）」
- `:26-29` `title_label(...,"临时属性")`（硬编码中文）/ `position=(16,80)` / `dimension=(220,200)`
- `:42-117` rebuild_rows 按 skillid 聚合 Buffstat、名称取 ItemData/SkillData、`snprintf("%s  x%d  %ds")`、`h=28+rows*16` 钳 [64,280]
- `:119-141` draw「无 buff 时不渲染（避免空半透明黑板叠场景/小地图）」+ `ColorBox(dimension,BLACK,0.55f)`
- `:143-186` update 本地倒计时 / send_key X/Esc deactivate

### 2.5 UIEnergyBar
- `IO/UITypes/UIEnergyBar.h:14-28` `TYPE=ENERGYBAR/FOCUSED=false/TOGGLED=false` / `MAX_ENERGY=10000` / `BAR_INNER_W=70`
- `.cpp:29-42` `UiAssetResolve::window("EnergyBar")` 取 w/c/e/Gage，缺则 `LOG(LOG_WARN,"UIEnergyBar: missing EnergyBar assets")`
- `:45-86` draw 按 `fill=energy*70/10000` 铺 bar_c；update `visible=player.has_buff(ENERGY_CHARGE)`；`update_screen position=(new_width/2-120,new_height-90)/dimension=(90,14)`
- `:88-114` send_key S/T/U→UISoftKey+UseSkillPacket、C/X deactivate

### 2.6 UIComboCounter
- `IO/UITypes/UIComboCounter.h:15-17` `TYPE=COMBOCOUNTER/FOCUSED=false/TOGGLED=false`；`.cpp:18` `label(A18M,CENTER,YELLOW)`
- `.cpp:24-37` `window("Combo")` 回退 `window("Number")`；「本客户端 UI.nx/UIWindow.img 里没有 Combo/Number 节点…因此只降级为纯文字计数」
- `:40-52` set_combo：3000ms 静默期(对齐 AranComboHandler.java:41) / `hide_ms=count>0?3000:0` / `label.change_text(to_string(combo)+" Combo")`
- `:54-82` draw 文字+可选 backgrnd；`update_screen position=(new_width/2,new_height/2-80)/dimension=(120,24)`

### 2.7 UIBossHP
- `IO/UITypes/UIBossHP.h:15-30` `TYPE=BOSSHP` / `set_hp(oid,cur,max,tag_color,tag_bg)` / `BAR_INNER_W=640`；fill「1×9 填充条」textlay「791×26 名牌底(可选)」
- `.cpp:31-46` `window("DualMobGauge")` 回退 `window("MobGage")`；「REA 反编译 sub_56C9B7 依次加载 backgrd/text/gauge…修前 fill_w 算了却 (void) 丢弃，血条只有背景+百分比文字、无填充」
- `:62-106` 缺 backgrnd `LOG_WARN` / set_hp `hide_ms=8000`+`pct%` / draw 填充条 `fill.draw(DrawArgument(fillpos,(fill_w,fh)))`
- `:119-170` `update_screen position=((new_width-bw)/2,20)/dimension=(bw,backgrnd?height:43)`(bw=681) / send_key S/T/H/V/C 调试

### 2.8 UIClock
- `IO/UITypes/UIClock.h:16-37` `TYPE=CLOCK` /「type 1=挂钟文本, 2=倒计时秒, else 自由文本」/ `set_clock` / `stop()`；`.cpp:19` `label(A18M,YELLOW)`
- `.cpp:26-43` set_clock：mode2→`Locale::format("clock.seconds_left",to_string(seconds))`、否则 `Locale::text("clock.title")` 或 text
- `:62-88` update 倒计时到 `Locale::text("clock.zero")`；`update_screen position=(new_width/2,40)/dimension=(160,24)`
- `:90-118` send_key S/T/R→UISoftKey、C/X→stop

### 2.9 UIMiniMap
- `IO/UITypes/UIMiniMap.h:29-34` `UIDragElement<PosMINIMAP>` / `TYPE=MINIMAP/FOCUSED=false/TOGGLED=true`
- `:52-64` `CENTER_START_X=64/BTN_MIN_Y=4/ML_MR_Y=17/MAX_ADJ=43/MARK_X=8/MARK_Y=24/M_START=36/LISTNPC_ITEM_HEIGHT=17/LISTNPC_ITEM_WIDTH=140/LISTNPC_TEXT_WIDTH=114/WINDOW_UL_POS=(0,0)`
- `:78-93` `enum Buttons{BT_MIN,BT_MAX,BT_SMALL,BT_BIG,BT_MAP,BT_NPC}` / `Type{MIN,NORMAL,MAX}`；`:123-138`「MAX 顶带 72px 浅蓝底，地图名白字黑描边」+ OutlinedText region_text/town_text + `Slider listNpc_slider`
- `.cpp:44-76` 构造 `Point(128,20)`、`listNpc_dimensions=(150,170)`、type/simpleMode 读 Setting；资源取 `UIWindow(2).img/MiniMap(SimpleMode)`，缺则 `active=false/dimension=(0,0)` return
- `:80-104` BtMap / Basic.img/BtMin|BtMax 三颗 MapleButton(0,0)、缺件 diag_once
- `:120-183` draw MIN(min_sprites+combined_text) / NORMAL(normal_sprites+town_text+portal+movable+npclist) / MAX(max_sprites+region_text(46,22)+town_text(46,39)+MAX_ADJ)
- `:490-544` `button_pressed`：BT_MIN/MAX 逐级切 type+persist（`:482`「Setting<>::save 只改内存项，落盘必须显式 Configuration::get().save()」）、BT_SMALL/BIG→`big_map=!big_map` + **`// TODO: Toggle scrolling map`(:516)**、BT_MAP→UIWorldMap、BT_NPC→set_npclist_active
- `:569-686` toggle_buttons 6 按钮坐标排布；`:666`「SMALL/BIG 未注册（本包无件，显式预留）时 small_w=0，不推进」
- `:704-807` update_canvas：min/normal/max 三套 dimensions + 9 切片 npclist

### 2.10 UIStatusMessenger（浮公告）
- `IO/UITypes/UIStatusMessenger.h:41-42`「8 seconds」+ `FADE_DURATION=8000`；`:49-51` `TYPE=STATUSMESSENGER/FOCUSED=false/TOGGLED=false`；`:65-67` `MAX_MESSAGES=6` / `deque<StatusInfo>`
- `.cpp:26-48` StatusInfo A12M RIGHT text(color)+BLACK shadow / draw(+1,+1) / update FADE_STEP
- `:58-80` draw 每 14px 上移；`update_screen y_adj=(new_width>800)?37:0` / `position=(new_width-6,new_height-145+y_adj)`
- `:87-125` send_key S/T/R/W→show_status 调试 / show_status push_front+超 6 pop_back

### 2.11 UISoftKey（虚拟键盘，按需 emplace）
- `IO/UITypes/UISoftKey.h:27-32`「V17.7 原生 UIWindow.img/SoftKeyboard 宽 QWERTY…与 Login.img/Common/SoftKey 是两套键盘…Pic 密文最短 6/Text 明文最短 1」
- `:45-47` `TYPE=SOFTKEYBOARD/FOCUSED=true/TOGGLED=false`；`:74-83` `enum Buttons{BtClose,BtBackspace,BtEnter,BtCapsLock,BtLShift,BtRShift,BtKey0}`；`:85-88` `KEY_COUNT=37/PIC_MIN_TEXT_LEN=6/TEXT_MIN_TEXT_LEN=1/MAX_TEXT_LEN=16`
- `.cpp:38-59` `KEY_CHARS[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-"`、`KEY_X[37]/KEY_Y[37]` 逐键网格
- `:62-117` 构造 `UIElement((216,224),(0,0))`；back/frame(315x127)/title(147x34)/tray(278x95) 三层 Sprite；BtClose(299,2)、6 功能键、37 键、Textfield(A11M,BLACK,(10,130)-(210,148),MAX_TEXT_LEN=16)
- `:121-155` Pic set_cryptchar('*') / Text 物理键盘直写 / dimension=frame 尺寸回退 (315,127)
- `:390-408` **`bool requirements=false; // TODO: Add check for required amount of characters`（占位）**

### 2.12 UIQuickslotConfig（快捷键改键，按需 emplace）
- `IO/UITypes/UIQuickslotConfig.h:2,15-16`「quickslotConfig — KeyConfig 变更快捷键：点槽再按键 → CHANGE_QUICKSLOT(183)」「原生底板文案已烘焙；交互=点 8 槽之一→按键→立刻上行 183」
- `:20-22` `TYPE=QUICKSLOTCONFIG/FOCUSED=true/TOGGLED=false`；`:44-49` `array<Texture,8> slot_caps` / `SLOT_X[4]={67,102,137,172}/SLOT_Y[2]={114,148}/SLOT_W=28/SLOT_H=25`
- `.cpp:17-89` 构造 `dimension=(266,238)`；取 `window("KeyConfig")` 回退 `UIWindow.img/KeyConfig`、backgrnd/BtQuickSetting/key 表(40+ Key 映射 NUM1=2..F12=88)；position 居中
- `:92-144` refresh_key_caps 读 `kb.get_quickslot_key(i)`、slot_at 2×4、apply_slot_key 同键互换 + `kb.commit_quickslots()` + `LOG("…→CHANGE_QUICKSLOT")`
- `:158-226` draw 8 键帽+选中 overlay / send_cursor 点槽/点外关窗 / send_key Esc 取消 / close→deactivate

## 3. `locale/zh-CN.ini` 状态栏/HUD 文案 key 全集 [观察]
- `:104-111` `chat.target.all=对所有人 / whisper=悄悄话 / party=组队 / guild=公会 / buddy=好友（NO-NATIVE）/ alliance=联盟（NO-NATIVE）/ spouse=夫妻（NO-NATIVE）/ megaphone=喇叭（NO-NATIVE，源码明示 v83 下拉无此项）`
- `:112` `whisper.hint_target=请输入悄悄话的对象。`
- `:117-123` `messenger.self/no_room/invited/declined/prompt_chat/prompt_decline/prompt_invite`
- `:131-136` `sysopt.combo.screenshot.0/1`、`sysopt.combo.mobinfo.0..3`
- `:140` `sysopt.label.resolution=窗口大小`（注释 `:138`「v83 原生无此行，档位表来自插件 rs_tier_dims」）
- `:141-149` `sysopt.combo.resolution.0..8`（800x600 / 1024x768 / 1366x768 / 1600x900 / 1920x1080 / 1280x720 / 1440x900 / 1680x1050 / 2560x1440）
- `:201` `common.channel=频道{}`；`:229` `mapinfo.type_boss=BOSS`；`:556` `common.chat_sent=Chat sent…`
- `:355-357` `clock.seconds_left={}s` / `clock.title=Clock` / `clock.zero=0s`
- `:399-401` `megaphone.empty=Empty` / `no_539=No 539*` / `sent=Sent…`；`:548-549` `megaphonebasic.*`
- `:591-600,633` `contextmenu.info/whisper/party/report/trade/buddy/messenger/guild/family/fame` / `contextmenu.title_fallback=Menu`（UIChatBar 名字菜单实际硬编码，见 `UIChatBar.cpp:1957-1959`）
- **stub/占位文案逐字**：`:238 dailygift.rule_3=· 每个账号每日限领 1 次（本版假数据）` / `:239 rule_4=· 格子中部道具槽预留，本版不渲染物品` / `:240 rule_5=· 点击格子即可假领取并显示已领取标识` / `:241 rule_6=· 无协议：不会真实入包 / 不计真实杀怪`；`:344-345 cashshop.stub_job=Illium / cashshop.stub_name=ShomeiZekkou`；`:348 charinfo.medal_stub=Junior Adventurer`；`:93 monsterbook.stub_mob=怪物编号 {}`；`:5-6 worldselect.stub_view_map/stub_view_all=…功能暂未开放。`
- `:102-103` 注释「后四项是 NO-NATIVE 措辞（按斜杠命令 /b /a /s 与喇叭道具语义自定），沿用决策 B『先建键、等有 v83 依据再换』」
- `[推断]` 无 UIStatusBar/UIChatBar 名字菜单、HP/MP/EXP 标签、快捷栏键帽、等级芯片文案 key —— 位图烘焙或代码硬编码。

## 4. `Configuration.cpp/h` HUD 相关设置 [观察]
- `Configuration.h:100-101` `bool get_buff_timer() const;`；`:112-113` `bool get_equip_compare() const;`
- `:525-530` `struct BuffTimer : BoolEntry("ui.buff_timer","true")` 默认 **true**；「1 秒粒度，对齐 BeiDou V17.7 BuffTimer，关闭后仅保留图标与冷却遮罩」
- `:532-538` `struct EquipCompare : BoolEntry("ui.equip_compare","true")` 默认 true
- `:470-499` 分辨率 `window.width=1280` / `window.height=720` / `window.game_width=1280` / `window.game_height=720` / `window.game_tier=255(TIER_NONE)`；「登录态 1280×720 恒定，进图按三项切换」
- `Configuration.cpp:435-460` `dims_from_tier(game_tier)` 优先，否则 game_width/height，缺键或 <640×480 回落登录尺寸，回灌 + LOG `[MR] login=… game=… tier=…`
- `Configuration.h:639-658` `ChatViewMax BoolEntry("chat.max","false")` / `ChatViewX ShortEntry("chat.x","0")` / `ChatViewY ShortEntry("chat.y","0")`
- `:710-715` `PosCHAT : PointEntry("ui.position.chat","(0, 572)")`
- `:717-727` `PosMINIMAP : PointEntry("ui.position.minimap","(5, 24)")`；「顶边实测 y=24，y0..22 是 ScrollingNotice 全宽黑底公告条」
- `:834-846` `SysOptHpWarn ByteEntry("ui.sysopt.hp_warn","52")` / `SysOptMpWarn("ui.sysopt.mp_warn","52")`；注释「左端没有=0 不报警；当前无消费方，仅落盘」（与 `IO/UITypes/UIStatusBar.cpp:2508` 已接消费方，注释略滞后）
- `:966-982` `MiniMapType("minimap.type","2")` / `MiniMapSimpleMode("minimap.simple","false")` / `MiniMapDefaultHelpers("minimap.default_helpers","false")`
- `Configuration.cpp:140-141` SETTINGS_ORDER「界面 / ui.buff_timer 是否在 buff 图标上显示剩余时间」「ui.equip_compare 装备悬停对比」
- `Configuration.cpp:235-236,253-255,279-280` `settings.emplace<BuffTimer/EquipCompare/ChatViewMax/ChatViewX/ChatViewY/SysOptHpWarn/SysOptMpWarn>()`
- `Configuration.cpp:316-319`「拖框高需显式调 Configuration::get().save()（UIChatBar 老 HUD 拖框高即如此）」
- `Configuration.cpp:659-677` `save()` 二进制写出 UTF-8 无 BOM + LF，优先级 xy-config.yaml > Settings > 内置默认
- `Configuration.cpp:870-873,894-897` `get_buff_timer()` / `get_equip_compare()` 转调 Setting
- **快捷栏配置** [观察]：Configuration.h/cpp 内**无** quickslot 专用键；8 槽触发键存 `Keyboard::quickslots`，由 `IO/UITypes/UIQuickslotConfig.cpp:125-144` 的 `kb.commit_quickslots()` + CHANGE_QUICKSLOT(183) 改；`IO/UITypes/UIStatusBar.cpp:2122-2124` 8 槽从 `keyboard.get_quickslot_key(i)` 读、26/32 格用 CLASSIC_QS_KEYS 固定表。→ [推断] 快捷栏配置不在 Configuration 而在 Keyboard/KeyConfig。
- 聊天窗口尺寸：classic 用 `chat.y`(历史框高，默认回落 80，`UIChatBar.cpp:72-80`)、`chat.max=false`/`chat.x=0`；modern 才用 ChatViewMax/X/Y。

## 5. 含「预留/stub/TODO/未实现/暂不/显式/不做/假/placeholder/仅绘制/硬编码/占位」字样的 HUD 源码行（逐字）[观察]
- `IO/UITypes/UIStatusBar.cpp:860` `// 再点一次「界面」必须收起。旧实现在这里什么都不做，`；`:1879` `// 快捷栏面板在下方按格数选型后赋值，此处不提前占位。`；`:2501` `// 不再「数值一变就闪」—— 那是旧占位行为，与「警报」设置项语义无关，`；`:2507` `// 无 hpFlash/mpFlash 节点，本设置对其无观感 —— 已记入 evidence §4 预留。`
- `IO/UITypes/UIChatBar.cpp:1024,1070,1120,1139,1164,1198` `// TODO: The top/right gets shifted by a pixel`（6 处）；`:1701` `// 是 MapleButton，由 UIElement::draw 画，只能显式 deactivate。`；`:2365` `// Q5 回显矩阵：服务端 partyChat 显式排除发送者（World.java`；`:2463` `// 与原生「没开过好友列表发 /b 无人收到」一致，不假装能发。`；`:2512` `default: break; // WHISPER 等由调用方给显式前缀`；`:2574` `// 显式预留：v83 下拉里没有「喇叭」项，原生喇叭入口是道具使用；`；`:884` `// 里仍是 0），所以手势结束这一处显式存一次，保证「拖完记住」成立。`
- `IO/UITypes/UIChatBar.h:31` / `:122` `// TODO: Change these?`（`:123` `change_target(){}` 空 stub）；`:413` `// Party/Guild/Alliance/Buddy：服务端广播显式排除发送者 → 必须本地回显；`
- `IO/UITypes/UIMiniMap.cpp:482` `// Setting<>::save 只改内存项，落盘必须显式 Configuration::get().save()。`；`:516` `// TODO: Toggle scrolling map`；`:666` `// SMALL/BIG 未注册（本包无件，显式预留）时 small_w=0，不推进；`
- `IO/UITypes/UISoftKey.cpp:400` `// TODO: Add check for required amount of characters`
- 其它 HUD 邻接类：`IO/UITypes/UIUserList.cpp:916`「v83 家族聊天由聊天框目标/斜杠别名路由；change_target 仍是预留空实现，」；`IO/UITypes/UIDailyGift.h:32-33,58,117`；`IO/UITypes/UICashShop.cpp:100/102/153`；`IO/UITypes/UICharInfo.cpp:171`；`IO/UITypes/UIGuild.cpp:49`；`IO/UITypes/UIParty.cpp:41`；`IO/UITypes/UIQuestLog.cpp:903/955`；`IO/UITypes/UIStorage.cpp:44`；`IO/UITypes/UIMonsterBook.cpp:43`；`IO/UITypes/UIEquipInventory.cpp:622`；`IO/UITypes/UIWorldMap.h:219`；`IO/UITypes/UILogin.h:83`；`IO/UITypes/UIGMCommandList.h:11`；`IO/UITypes/UIUserList.h:233`；`IO/Components/Charset.cpp:84` `// TODO: The two below draw methods need combined adding hspace...`
- [推断] 硬编码类：`IO/UITypes/UIStatusBar.cpp:2085-2088`（键帽无字母字形改描边文字回退）、`IO/UITypes/UITemporaryStatView.cpp:26`（`title_label("临时属性")`）、`IO/UITypes/UIChatBar.cpp:1957-1959`（名字菜单三项中文）、`IO/UITypes/UIComboCounter.cpp:49`（`" Combo"`）、`IO/UITypes/UIChatBar.cpp:167`（`"[Welcome]…"`）。

## 6. 通用组件（状态数 / 切片 / 字号常量）[观察]
- `IO/Components/Button.h:28-36` `enum State { NORMAL, DISABLED, MOUSEOVER, PRESSED, IDENTITY, NUM_STATES }`（**6 状态**）
- `IO/Components/MapleButton.h:28-46` `class MapleButton : Button`，`Texture textures[NUM_STATES]` + `Animation animations[NUM_STATES]`；send_cursor 恒返回 IDLE
- `IO/Components/MapleButton.cpp:55-71` 加载 4 态节点 `normal/pressed/mouseOver/disabled`（normal>1 帧走 Animation）；`:76-108` draw / `animations[state].update(6)`；`MapleButton.h:48-62` `maple_btn_nopos()` + `try_maple_btn(...)`；`MapleButton.cpp:25-53` nopos=(-32768,-32768)，缺资产「skipped instead of stacking at (0,0)」
- `IO/Components/MapleFrame.cpp:26-40` **9 切片** `c/e/ne/n/nw/w/sw/s/se`，`xtile=max(north.width,1)/ytile=max(west.height,1)`；`:42-72` draw 平铺 `numhor=rwidth/xtile+2`
- `IO/Components/Textfield.h:32-37` `enum State { NORMAL, DISABLED, FOCUSED }`（3 态）；`:40` 构造 `(font,alignment,text_color,bounds,limit,marker_height=12)`；`:53-58` `set_accept_unicode(bool)` / `accepts_unicode()`；`:84` `bool accept_unicode = false;`
- `IO/Components/Textfield.cpp:274-301` `add_string`：accept_unicode 时仅滤 c<32，否则滤 c<32||c>=128（**ASCII 32-127**）；`:303-336` modifytext 同过滤 + crypt 掩码；`:406-410` `belowlimit()`
- `IO/UI.cpp:522` `if (focusedtextfield->accepts_unicode())` 决定开/关系统 IME
- `IO/Components/ScrollingNotice.cpp:22-30` `width=800` / `background=ColorBox(800,23,BLACK,0.535f)`（**23px 全宽黑底公告条**）/ `notice=Text(A12M,LEFT,YELLOW)`；`:39-76` draw / update `xpos-=0.5`、`xpos.last()<-notice.width()` 即 `active=false`（「原生公告文字完整滚出左边界后连同 23px 黑底一起消失」）
- `IO/Components/NameTag.h:29-50`「NameTag.img/<id> 三切片 w(左cap)/e(body 平铺)/c(右cap)/clr(ARGB 标签色)」；`enum Group{NONE,MEDAL,PET}`、`MIN_ID=3/MAX_ID=254`；`:73-101` `struct Frame{left,mid,right}`
- `IO/Components/Charset.h:29-42` `enum Alignment{LEFT,CENTER,RIGHT}`；`IO/Components/Charset.cpp:24-43` `charset_char`：v83 StatusBar.img/number 转义名 `Lbracket→'[' / Rbracket→']' / slash或'\'→'/' / percent→'%'`（「旧实现只取节点名首字符导致 HP/MP/EXP 画错」）；`:48-64` 构造；`:85-174` draw CENTER 推进 `getw+2` / LEFT `getw+1` / RIGHT 倒序
- `Graphics/Text.h:33-47` `enum Font { A11M, A11B, A12M, A12B, A13M, … A18M, NUM_FONTS }`（12 字号档；HUD 用 A11M/A11B/A12M/A12B/A13M/A18M）
- `IO/UITypes/UISoftKey.cpp:38-59` 37 键 `KEY_CHARS`/`KEY_X[37]`/`KEY_Y[37]`；`IO/UITypes/UIMiniMap.h:52-64` 布局常量

## 7. 小结 A｜当前端 HUD 已实现功能面（24 条，带行号）[观察]
1. `IO/UIStateGame.cpp:78-99` 10 个 HUD 元素构造期统一 emplace
2. `IO/UIStateGame.cpp:802` pre_add 去重/占位
3. `IO/UIStateGame.cpp:808-820` HUD 元素 silent_types 不播开窗音
4. `IO/UITypes/UIStatusBar.cpp:189-198` modern/classic 双皮肤自动切换
5. `IO/UITypes/UIStatusBar.cpp:1932-1959` HP/MP/EXP 三联 CLIP 量规
6. `IO/UITypes/UIStatusBar.cpp:2350-2394` 位图字库画 [hp/maxhp]/[mp/maxmp]/EXP 两行完整数值+[xx.xx%]
7. `IO/UITypes/UIStatusBar.cpp:2315-2344` Basic.img/LevelNo 等级徽章 + 职业名/角色名两行
8. `IO/UITypes/UIStatusBar.cpp:2055-2074` 13 颗 classic 底栏按钮
9. `IO/UITypes/UIStatusBar.cpp:2017-2052` 8/26/32 格快捷栏面板按分辨率选型
10. `IO/UITypes/UIStatusBar.cpp:2212` 快捷栏格点击 `Stage::get().send_key(...)`（hit→send_key 接通）
11. `IO/UITypes/UIStatusBar.cpp:2106-2160` 快捷栏图标(技能/道具/菜单动作)
12. `IO/UITypes/UIStatusBar.cpp:2482-2541` HP/MP 低量警报(5 帧闪光, 阈值可配)
13. `IO/UITypes/UIStatusBar.cpp:441-486` modern 27 颗 submenu 分层菜单
14. `IO/UITypes/UIStatusBar.cpp:747-1176` 商城/主菜单/装备/物品/统计/技能/键盘/快捷栏/密语/举报全分支
15. `IO/UITypes/UIChatBar.cpp:139-165` classic 聊天 Textfield + enter/escape 回调
16. `IO/UITypes/UIChatBar.cpp:341-478` 收起灰带/展开半透明历史框/8 项目标列表/Slider 滚动
17. `IO/UITypes/UIChatBar.cpp:2313-2583` 斜杠命令+8 频道路由+本地回显矩阵+喇叭移交
18. `IO/UITypes/UIChatBar.cpp:1925-1998` 名字点击菜单(密语/角色信息/加好友)
19. `IO/UITypes/UIBuffList.cpp:66-96,224-250` buff 图标倒计时+右键取消
20. `IO/UITypes/UIQuickslotConfig.cpp:125-144` 8 槽改键+CHANGE_QUICKSLOT(183)
21. `IO/UITypes/UIMiniMap.cpp:120-183,490-544` 小地图三态+地图名+NPC 列表+世界地图入口
22. `IO/UITypes/UIBossHP.cpp:81-106` Boss 血条填充条+百分比
23. `IO/UITypes/UIClock.cpp:26-79` / `UIComboCounter.cpp:40-61` / `UIEnergyBar.cpp:45-86` / `UIStatusMessenger.cpp:58-80` 时钟/连击/能量/浮公告
24. `IO/Components/ScrollingNotice.cpp:22-76` 顶部全宽 23px 公告条滚出即消

## 8. 小结 B｜当前端 HUD 明显缺口面（24 条，带行号）[观察+推断]
1. `IO/UITypes/UIChatBar.h:122-123` `change_target(){}` 空 stub，目标切换改走 set_classic_target
2. `IO/UITypes/UIChatBar.cpp:2574` 喇叭下拉项「显式预留」
3. `IO/UITypes/UIMiniMap.cpp:516` `// TODO: Toggle scrolling map`
4. `IO/UITypes/UIMiniMap.cpp:666` SMALL/BIG 缩放钮「本包无件，显式预留」未注册
5. `IO/UITypes/UISoftKey.cpp:400` PIC 复杂度校验占位
6. `IO/UITypes/UIChatBar.cpp:1024/1070/1120/1139/1164/1198` 6 处 modern 拖拽 1px 偏差
7. `IO/UITypes/UIComboCounter.cpp:24-37` 无 Combo/Number 位图节点，降级纯文字计数
8. `IO/UITypes/UIBossHP.cpp:31-33` DualMobGauge 缺则回退 MobGage 节点，无 UIMobGage 类文件
9. `IO/UITypes/UIMobGage.*` 未找到
10. `IO/UITypes/UIStatusBar.cpp:2507` modern 皮肤无 hpFlash/mpFlash 节点
11. `IO/UITypes/UIStatusBar.cpp:2085-2088` FontMemo 无字母字形，键帽改描边文字回退
12. `IO/UITypes/UIStatusBar.cpp:140-175` number 字库无 '.' 字形，1x1 白点补
13. `IO/UITypes/UITemporaryStatView.cpp:2` 明细列表本地自绘(宋体/原色)非原生
14. `IO/UIStateGame.cpp:94-95` UITemporaryStatView 构造即 deactivate
15. `IO/UITypes/UIChatBar.cpp:207-215` modern chat/ingame 节点本包缺失
16. `IO/UITypes/UIChatBar.h:335-339` 收起态灰带无位图可拼，自铺 ColorBox(119,119,119)
17. `IO/Components/Textfield.h:84` + `Textfield.cpp:281-321` 聊天输入框未 `set_accept_unicode(true)`，仍限 ASCII 32-127 / 未开 IME
18. `locale/zh-CN.ini:108-111` 4 个频道文案 NO-NATIVE 自定
19. `locale/zh-CN.ini:238-241` dailygift 假数据/道具槽预留/无协议
20. `locale/zh-CN.ini:344-348` cashshop.stub_job/stub_name、charinfo.medal_stub 占位名
21. `IO/UITypes/UIStatusBar.cpp:2038-2040,2096-2097` >1280 宽屏整组居中属用户决策偏离原生
22. `IO/UITypes/UIChatBar.cpp:1957-1959` 名字菜单三项中文硬编码，未走 locale key
23. `Configuration.h:834-846` HP/MP 警报注释仍写「当前无消费方，仅落盘」，与 `UIStatusBar.cpp:2508` 已接消费方不一致
24. `IO/UITypes/UIUserList.cpp:916` 家族聊天 change_target 仍预留空实现

## 9. 拆分结论（明确回答）[推断，依据 `IO/UIStateGame.cpp:78-99` 的 10 次独立 emplace + 各类独立 TYPE]

**当前端确实把原生单体 `CUIStatusBar`（v83 41 方法：聊天/快捷栏/HP·MP·EXP 条/菜单/密语/浮公告/按钮全在里面）拆成多个独立 UI 类**，共 **10 个构造期 HUD 类 + 2 个按需类**：

| 类 | 职责（从原生单体剥离的面） | 证据 |
|---|---|---|
| `UIStatusBar` | 底栏主体：HP/MP/EXP 条、等级/名字芯片、快捷栏、13 classic 按钮、modern 27 submenu 菜单、商城/装备/统计/技能/键盘入口、HP/MP 警报 | `IO/UITypes/UIStatusBar.h:35-37`、`UIStatusBar.cpp:536-676,747-1176,2233-2541` |
| `UIChatBar` | 聊天输入/历史/收起灰带/8 频道下拉/斜杠命令/名字点击菜单/滚动条/拖拽改高 | `IO/UITypes/UIChatBar.h:39-41`、`UIChatBar.cpp:139-165,341-478,2313-2583` |
| `UIMiniMap` | 小地图三态/地图名/NPC 列表/世界地图入口 | `IO/UITypes/UIMiniMap.h:29-34`、`UIMiniMap.cpp:120-183` |
| `UIBuffList` | buff 图标条带 + 倒计时 + 右键取消 | `IO/UITypes/UIBuffList.h:65-70`、`UIBuffList.cpp:66-96,224-250` |
| `UITemporaryStatView` | buff 明细板（与 UIBuffList 分工：列表取消 vs 明细），构造即停用 | `IO/UITypes/UITemporaryStatView.h:19-21`、`IO/UIStateGame.cpp:94-95` |
| `UIEnergyBar` | ENERGY_CHARGE 能量条 | `IO/UITypes/UIEnergyBar.h:14-16`、`UIEnergyBar.cpp:45-86` |
| `UIComboCounter` | Aran SHOW_COMBO 连击计数 | `IO/UITypes/UIComboCounter.h:15-17`、`UIComboCounter.cpp:40-61` |
| `UIBossHP` | FIELD_EFFECT mode 5 Boss 血条 | `IO/UITypes/UIBossHP.h:15-17`、`UIBossHP.cpp:81-106` |
| `UIClock` | CLOCK/STOP_CLOCK 时钟/倒计时 | `IO/UITypes/UIClock.h:16-18`、`UIClock.cpp:26-79` |
| `UIStatusMessenger` | 右上浮公告(6 条×8s 淡出) | `IO/UITypes/UIStatusMessenger.h:49-51`、`UIStatusMessenger.cpp:58-80` |
| `UISoftKey`（按需） | 虚拟键盘，被 HUD 各 send_key/按钮 emplace 复用作输入器 | `IO/UITypes/UISoftKey.h:45-47`、`UISoftKey.cpp:62-117` |
| `UIQuickslotConfig`（按需） | 8 槽改键窗 → CHANGE_QUICKSLOT(183) | `IO/UITypes/UIQuickslotConfig.h:20-22`、`UIQuickslotConfig.cpp:125-144` |

要点：
- **HP/MP/EXP 条 + 快捷栏 + 菜单 + 按钮仍集中在 `UIStatusBar` 单类**（并非再拆），只是内部分 modern(StatusBar3) / classic(StatusBar.img) 两套实现分支（`UIStatusBar.cpp:189-198`）。
- **聊天面被彻底独立成 `UIChatBar`**（自带 Textfield/历史/频道路由/名字菜单），与状态栏通过 `IO/UIStateGame.cpp:139-152` 的 elementorder 重排协调点击优先级。
- **浮公告从状态栏拆成 `UIStatusMessenger`** + 另有全宽 `ScrollingNotice`（`IO/Components/ScrollingNotice.cpp:22-30`，由 `UI::draw` 画在 state 之上）。
- `UIMobGage.*` 未找到；`UIBossHP` 用 `window("MobGage")` 仅作资源节点回退。
- 参照基准全程 083/v83（`StatusBar.img`/IDA BeiDou v17.7 sub_8D01B2、ijl15 文案），未引 095 线符号/IDB。

（完 · 只读审计，未改任何文件）
