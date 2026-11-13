# r02 — 状态栏 HUD 面「UI 资源 / 样式」证据盘点（只读归档）

> 来源：subagent `c202a760` 只读取证全文（2026-11-08 19:13），主 agent 结构化归档。全程未改产品源码。
> 门禁：`xiaoye-maple-story-dev` 已加载；参照基准 083/v83；未引 095 线。
> 标注 `[观察]` / `[推断]` 沿用子代理原文。

## 0. 与 Angel 一手证据的关键印证（主 agent 补）

`[观察]` 子代理报 `IO/UITypes/UIStatusBar.cpp:1901-1907` 注释记 IDA BeiDou v17.7 `CreateCtrl(node,id,x,y,type)` `sub_8D01B2` 坐标：
`BtShop=1000/(573,543)`、`BtNPT=(629,543)`、`BtMenu=(685,543)`、`BtShort=1007/(741,543)`、`EquipKey=2000/(618,515)` … `QuickSlotD=2006/(573,515)`；窗体高 578、设计网格宽 800、底条相对 y=CreateY-507。
→ 与 r01 §11 从 Angel `CUIStatusBar::OnCreate` 逐字提取的 **11 个 CreateCtrl 坐标表完全一致**（573/629/685/741 @543；618/648/678/708/738/768/573 @515）。**排版层当前端已用无符号 IDB 对齐过，Angel 补的是 id 语义 + 行为层**。
`[推断]` 唯一疑点：当前端注释把 `(573,515)` 标为「`QuickSlotD=2006`」，而 Angel 逐字该点是 **`1009`**（`OnButtonClicked` 对应 `SendClaim()`，节点 `SP_1976_UI_STATUSBARIMG_BTCLAIM`）；`SP_1985_QUICKSLOTD` 更可能对应 `1006/(768,515)`（`OnButtonClicked` 1006=`ToggleQuickSlot()`）。→ 当年无符号推断的按钮 id 命名/编号有误标，属可销案的一致性疑点（见 README 决策点）。

## 1. HUD 资源根与 `.nx` 节点引用（全仓 `nl::nx::UI` 144 处）

- `Util/UiAssetResolve.cpp:79-80` HUD 根首选 `nl::nx::UI["StatusBar3.img"]`；`:82` 回退 `["StatusBar.img"]`；`:87` `has_statusbar3()`；`:92` classic 判据 `["StatusBar.img"]["base"]`；`:97` `has_game_hud()`；`:102` `is_classic_hud()`。
- `Util/UiAssetResolve.cpp:44,49,59,64,69` `window()` 走 `UIWindow2.img`/`UIWindow.img`/`UIWindow4.img`；`:107,112` `shop_root()`；`:119,123` `close_button()` 在 `Basic.img` 找 `BtClose/BtClose2/BtClose3`。

各窗根节点字面量：
- `IO/UITypes/UIStatusBar.cpp:1983` `Basic.img`（classic LevelNo/ShowLevel）；`:2111` `UIWindow.img/KeyConfig/icon`、`:2113` 回退 `UIWindow2.img`。
- `IO/UITypes/UIStateGame.cpp:447` `StatusBar3.img/KeyConfig`。
- `IO/UITypes/UIOptionMenu.cpp:51` `StatusBar3.img/OptionMenu`；`:396` `Basic.img/Slider`；`:881/:950` `Basic.img/ComboBox/normal/0`、`:887/:956` `/1`、`:888/:957` `/2`。
- `IO/UITypes/UIChatBar.cpp:175` `Basic.img`；`:177` `sbroot["BtWhisper"]`；`:183-184` `basic["BtMax"]/["BtMin"]`；`:210-214` 注释「本包（v83 BeiDou）只有 classic StatusBar.img，没有 chat/ingame」→ modern chat view/input textures 空；`:265-266` `view["btMax"]/["btMin"]`；`:272-276` `input["button:chat"|"button:itemLink"|"button:chatEmoticon"|"button:help"|"button:outChat"]`。
- `IO/UITypes/UIMiniMap.cpp:57` `UIWindow2.img` / `:60` 回退 `UIWindow.img` / `:63` `window("MiniMapSimpleMode"/"MiniMap")` / `:68` `ListNpc` / `:82` `BtMap` / `:94` `Basic.img` / `:96-97` `BtMin` / `:101-102` `BtMax` / `:111-112` `MiniMapSimpleMode/DefaultHelper` / `:117` `iconNpc` / `:716-718` `Min/MinMap/MaxMap`；`:80-93` 注释 `BtMap` 是「大地图」36x12 橙底白字真件；`:522-524` 贴图 `UIWindow.img/MiniMap/BtMap/normal/0`；`:711-714` simpleMode 本包无 → 回退。
- `IO/UITypes/UICharSelect.cpp:371` `StatusBar.img/number`（选角等级字库复用 HUD）。
- `IO/Components/NameTag.cpp:126` `NameTag.img`；`IO/Components/ChatBalloon.cpp:48` `ChatBalloon.img/typestr`、`:95` `Login.img/WorldNotice/Balloon`、`:158` `BalloonForLoginProcess`。
- `IO/Components/Slider.cpp:41,43,46,58,69` `Basic.img/VScr5` `/VScr`，注释「v83: chat scroll lives under Basic.img VScr (StatusBar.img has no chat/common)」。
- `IO/Components/MapleComboBox.cpp:37` `Basic.img/[combobox]`；`Icon.cpp:53` `Basic.img/ItemNo`；`SkillTooltip.cpp:35` `UIWindow.img/IconBase/0`；`EquipTooltip.cpp:53,62` `UIWindow.img/ToolTip/Equip`+`IconBase/0`。
- `UIQuickslotConfig.cpp:24` `UIWindow.img/KeyConfig`；`UIWorldMap.cpp:113` `Basic.img/BtHide/normal/0`；`UIStatsInfo.cpp:164` `Basic.img/BtHide`；`UISkillBook.cpp:175` `Basic.img/BtMacro`；`UIShop.cpp:733` `Basic.img/ItemNo`；`UIMessenger.cpp:138` `Basic.img/BtClose3`。

### UIStatusBar 节点树（根由 `UiAssetResolve::status_bar_root()` 判定）
- modern `init_modern()` `IO/UITypes/UIStatusBar.cpp:201-534`：`:208` `mainBar`；`:209` `status|status800`；`:210-219` `EXPBar` + 五档宽 800/1024/1280/1366/1920 + `menu/quickSlot/submenu`；`:223-232` `EXPBar["backgrnd"]/layer:back/layer:gauge/layer:cover/layer:effect`；`:320-338` `status` 的 `backgrnd/layer:cover/layer:Lv/gauge/hp/layer:0/gauge/mp/layer:0/number/lvNumber`；`:342-343` `quickSlot` 底/盖；`:356-361` `menu["button:CashShop|Menu|Setting|Character|Community|Event"]`；`:373-379` `quickSlot["button:Fold"/"button:Extend"]`；`:442-485` `submenu` 五组按钮（menu/setting/community/character/event）+ `title`。
- classic `init_classic()` `IO/UITypes/UIStatusBar.cpp:1867-2104`：`:1869-1877` `base=sb["base"]`/`gauge=sb["gauge"]` + `base["backgrnd"]/["backgrnd2"]`/`gauge["graduation"]/["bar"]/["gray"]`；`:1964-1972` `gauge["hpFlash"]["0"]`/`["mpFlash"]["0"]`；`:1976-1993` `sb["number"]`→statset/hpmpset、`basic["LevelNo"]`/`["ShowLevel"]`+`showlevel["0"]`+`["Bracket"]["right"]`；`:2027,2033` `base["quickSlot32"]/["quickSlot26"]/["quickSlot08"]`；`:2055-2074` `sb["BtShop"|"BtNPT"|"BtMenu"|"BtShort"|"EquipKey"|"InvenKey"|"StatKey"|"SkillKey"|"KeySet"|"QuickSlot"|"QuickSlotD"|"BtClaim"|"BtWhisper"]`；`:2084` `sb["key"][0..6]`。

### 度量常量 / IDA / 运行时 / 截图校准注释
- `IO/UITypes/UIStatusBar.cpp:90-95` `SEG_HP=0/SEG_MP=109/SEG_EXP=222/SEG_W_HP=108/SEG_W_MP=107/SEG_W_EXP=118`；`:98-99` `GAUGE_X=218/GAUGE_Y=38`；`:106-118` `QS_SLOT=28/QS_PITCH_X=35/QS_PITCH_Y=34/QS_FIRST_X=9/QS_FIRST_Y=9/QS_ICON_SHIFT_Y=32/QS_CAP_X=4/QS_CAP_Y=4/QS_LABEL_Y=14`；`:140-144` number 字库 0-9(5x7)+Lbracket/Rbracket(2x9)+slash/percent(7x7)、无 '.'、推进 getw+2/getw+1/'.'=4；`:145-175` `hud_advance()/draw_hud_number()`。
- `IO/UITypes/UIStatusBar.cpp:1908-1921` nx_inspect 运行时实测：`base/backgrnd 817x71`、`backgrnd2 570x71`、`gauge/bar & graduation 340x31`、`hpFlash|mpFlash 109x18`、`FontMemo 0-9(6x7)`、`key/0..6` 键名贴纸、`base/quickSlot08 151x80/quickSlot26 466x80/quickSlot32 571x80`，origin 全 (0,0)；`:1922-1924` 正确端截图「底栏左贴边、右快捷格贴右边、backgrnd 横向铺满」。
- `IO/UITypes/UIStatusBar.cpp:1926-1930` `docs/.../correct_shots/进入游戏页.png` 1280x720 底栏 position 顶=649，逐像素判 graduation 跨 x218..557、边框 y700..717、HP 红填充 y702..715；`:1936-1951` hpset/mpset/statset 锚点、`lvbadge_pos=(50,47)`、`joblabel_pos=(87,40)`/`namelabel_pos=(87,56)`；`:2054` 按钮簇 y=543→36、y=515→8；`:2098-2101` position/dimension（classic 80 高）；`:1912` 注「LV. 字样已烘焙」；`:2076` 注「HP/MP/EXP 用 gauge/bar 图集烘焙，不建 Text」。
- `IO/UITypes/UIStatusBar.cpp:238-318,488-533` modern 五档坐标 + `:528` `[MR] StatusBar modern fallback for VWIDTH=`。
- `IO/UIStateGame.cpp:80-104` HUD 装配：`has_game_hud()` 才 `emplace<UIStatusBar>+<UIChatBar>`，否则 WARN「no StatusBar3.img or StatusBar.img/base — skipping status/chat HUD」；无条件 `<UIMiniMap>/<UIBuffList>/<UITemporaryStatView>`(随后 deactivate)/`<UIEnergyBar>/<UIComboCounter>/<UIBossHP>/<UIClock>`；Shop 按 `usable(shop_root())`。
- `IO/UITypes/UIBossHP.cpp:30-35,47,54` 走 `UiAssetResolve::window("DualMobGauge")` 优先、回退 `window("MobGage")`，取 `backgrd/gauge/text` 三分件（`:43-45` 注释 REA `sub_56C9B7`，证据 `docs/features/beidou-v17-parity/notes/evidence/r17/rea_decompile_DualMobGauge_56C9B7.txt`）；`:119-127` 顶栏居中 `((new_width-bw)/2,20)`、bw=681/h=43。
- `IO/UITypes/UIBuffList.cpp:54-56,60` buff 图标来自 `nl::nx::Skill[...]["skill"][strid]["icon"]`，负 buffid 走 `ItemData::get(-buffid).get_icon(true)`；`:159-166` `show_timer(Configuration::get().get_buff_timer())` 为 `ui.buff_timer` 唯一消费方；`:196-200` `position=(new_width-35,55)`。
- `IO/UITypes/UIMiscShells.cpp:7028-7032` `UIMobGage` 走 `load_window_bg("MobGage",backgrnd,dimension)`；`:4673` `qs_w = wide ? 466 : 151 // StatusBar.img/base/quickSlot26 / 08`；`:4187,5806` 注释「本包无 StatusBar3.img/OptionMenu」。

## 2. `IO/Components/` 通用组件常量

- `IO/Components/MapleButton.h:28-45` 四态 `Texture textures[NUM_STATES]`/`Animation animations[...]`；`.cpp:45-66` 取 `src["normal"]["0"]`(origin)/`pressed`/`mouseOver`/`disabled`（normal size>1 走 Animation）、`:70` state=NORMAL、`:88` `animations[state].update(6)`；`.h:52,57-62` `maple_btn_nopos()`/`try_maple_btn(...)` position 为 sprite 左上角、draw=pos-origin。
- `IO/Components/TwoSpriteButton.h:26-45` 二态 `BoolPair<Texture>`、npos/spos。
- `IO/Components/Textfield.h:32-37` 三态 NORMAL/DISABLED/FOCUSED；`:40` ctor(Text::Font,Alignment,Color,Rectangle,limit,marker_height=12)；`:55-58` `set_accept_unicode(bool)`/`accepts_unicode()`（默认 false 限 ASCII，`UI::focus_textfield` 据此开关 IME）；`:84` `bool accept_unicode=false`。`.cpp:274-301,303-336` `add_string/modifytext`：真收 `c>=32`（UTF-8/IME/剪贴板中文），假只 `32<=c<128`；`:305-306` 注释 FreeType atlas 仅 ASCII 32–127，回填 UTF-8/GBK 会画竖线伪影；`:324-329` cryptchar 掩码；`:192` 按 UTF-8 码点删除；`:87-99` 光标闪烁 elapsed>256。
- `IO/Components/ScrollingNotice.h:25,36-40` `ColorBox background`/`Text notice`/`Linear<double> xpos`/`bool active`/`int16_t width`；`.cpp:22-30` width=800、`ColorBox(width,23,BLACK,0.535f)`、`Text(A12M,LEFT,YELLOW)`；`:55-62` 按 viewwidth 更新全宽黑底；`:64` `xpos-=0.5`；`:68-74` 原生公告滚出左界后连 23px 全宽黑底一起消失（旧实现永久黑条压暗天空/小地图标题栏）。
- `IO/Components/MapleFrame.h:24-44` 九切片 c/e/ne/n/nw/w/sw/s/se + xtile/ytile；`.cpp:26-40` 取键缩写、`:38-39` `xtile=max(north.width(),1)`/`ytile=max(west.height(),1)`；`:42-72` draw 平铺 `numhor=rwidth/xtile+2`。
- `IO/Components/NameTag.h:29-50` `NameTag.img/<id>` 三切片 `w/e/c/clr`（ARGB，int 或 string 如 '-4096'）、`[medal|pet]/<id>` 同构、MIN_ID=3/MAX_ID=254；`.cpp:148,201-250` `root[to_string(id)]`、load_v83 `w/e/c/clr`、load_triple `<state>/<0|1|2>`（pre-v83 `Login.img/CharSelect/nameTag`）；`:38-81` read_argb 手动 ARGB→RGBA。
- `IO/Components/Charset.h:26-47` `enum Alignment{LEFT,CENTER,RIGHT}`、`Charset(nl::node,Alignment)`、`draw(int8_t)/draw(string)/getw(int8_t)`；`.cpp:24-43` `charset_char()` 转义 `Lbracket->'['/Rbracket->']'/slash->'/'/percent->'%'`，`:25-29` 注释旧实现取节点名首字符导致 HP/MP "hp/maxhp"、EXP "12%" 画成 "000" 乱码；`:85-174` CENTER 推进 width+2 再 shift-=total/2、LEFT 推进 getw(c)+1、RIGHT 反向。
- `IO/Components/Gauge.h:27-53` `enum Type{DEFAULT,CASHSHOP,WORLDSELECT,CLIP}`，`:32-33` CLIP 注释「自纹理左侧裁出 percentage*maximum 宽（不拉伸），用于 v83 StatusBar HP/MP」；`.cpp:29-68` draw `length=percentage*maximum`，DEFAULT 拉伸 front+mid+end、WORLDSELECT||CLIP 走 `Range<int16_t>(left_omit,right_omit)` 横裁；`:70-93` update 平滑 `step=(target-percentage)/24`。
- `Graphics/Text.h:33-48` 字号档 `A11M..A18B` 共 12 档；`:50-55` Alignment；`:80-92` Layout::Icon 行内 #i/#v/#s/#f/#B（`:81-83` IDA i/v→enum7/8、s→9、f/F→10/11、B→12→type2）；`:146-164` linespace/ink_offset/y_for_ink_top 垂直落位唯一真源（`:148-158` SimHei CJK bitmap_top=9/10，R28-B5 起换思源宋体）。
- Gauge 消费点：`IO/UITypes/UIStatusBar.cpp:228-229,333-334`(modern DEFAULT)、`:1954-1958`(classic CLIP)、`:2257-2275` `Gauge_seg segs[3]`；其它 `UIQuestLog.cpp:99-107,393`、`UIWorldSelect.cpp:353-354,1057`(WORLDSELECT)、`UICashShop.cpp:124-125`(CASHSHOP)、`Graphics/GraphicsGL.cpp:990,1672-1714`（#B 进度条 pool 1374=`UI/Login.img/Notice/Loading/bar/%d`）、`UILoginWait.cpp:48,122`（v83 bar/0..10 共 11 帧）。

## 3. 正确截图 / layout JSON / Photoshop 产物

- `docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/进入游戏页.png` 1280x720：新手村+操作说明对话+底栏 Lv/HP/MP/EXP+商城/帮助/目录/界面+右侧快捷键格+聊天/小地图/任务告知（依据 `r15-baseline-ui-gap-vs-screenshots.md:57-59`）；无配套 layout JSON。
- 同目录 `打开了S-E-I-K页面.png` 1280x720（四窗+底栏+快捷格）、`键盘设置.png` 629x369（E=装备 I=道具 K=技能 Q=任务 W=大地图）、`登录界面/选择频道页/选中频道页/选择角色页.png` 均 1280x720（登录流，非 HUD）；`current_shots/` 空；`overlay_diff.py:16-42` 半透明叠图；`r201-login-overlay-playbook.md:9-14` 记录 `进入游戏页.png` 对应 UIStatusBar CreateCtrl 已锁。来源 `C:\Users\11036\Desktop\正确客户端截图\`。
- `docs/features/ui-layout-v83-unify/evidence/*.png` 14 张 v83 底栏真件裁图（PNG 头实测）：`btn_shop/btn_npt/btn_menu/btn_short` 各 54x34、`gauge_bar/gauge_grad` 各 340x31、`qs.png` 466x80、`qs08.png` 151x80、`help0.png` 311x77、`panel_mid.png` 350x270、`charInfo.png` 183x115、`charInfo1.png` 183x151、`acct_slot.png` 220x60、`pass_slot.png` 220x70；与 `UIStatusBar.cpp:1908-1921` 一一对应；`README.md:4`「适用：BeiDou gms083 / 1280×720 / data.format=img（无 StatusBar3）」、`:6` 策略 A+范围 A(登录流+局内 HUD)+乱码 A+校准 A(IDA)；同目录 `_tmp_status_extract.py`/`_extract_wz_ui{,2,3}.py`/`ida-layout-tables.md`；无 layout JSON。
- `docs/features/minimap-mode-switch/evidence/01_minus_btn_mouseover.png` 560x360 等 4 张小地图缩放态实机帧（非原生正确端）；`docs/features/beidou-v16-parity/evidence/current_{06,16,17,18}_ingame*.png`、`enter-game-blackscreen-fix/evidence/test_ingame.png`、`login-flow-ui-ida-align/evidence/chain_defer_ingame.png` 1296x759 为进图帧。
- layout JSON 全集 `**/layout*.json`=5，**均非 HUD**：`char-stats-ui-ps/ui/detail/v4/layout_cn.json`（1346x600→658x392，2064 行）、`map-search-ui/resources/layout.json`、`map-search-ui/ui/.../layout.json`、`map-info-panel/resources/layout.json`、`UI/MapInfo/layout.json`（panel 240x537、slot_size 32、row_h 78、visible_mob_rows 3）。grep `statusbar|minimap|chat|hud|quickslot` 仅命中 `layout_cn.json:956,1142,1960,1978` 的 label_chip_boss/buff → **无任何 HUD/StatusBar 布局 JSON**。
- `UI/**` 37 张 PNG 全为 MapInfo/MapSearch/MapSide/MapMobDrops/SysOpt 窗体重绘件（如 `UI/MapSearch/BtBack_{normal,mouseOver,pressed,disabled}.png` 44x18 = MapleButton 四态出图），非状态栏 HUD 件。
- PSD 出图产物 50+（`char-stats-ui-ps/Character_Stats_Detail*.psd`、`tools/stat_out/stat_page.psd`、`inventory-ui-ps/*`、`hotkey-grid-3x3/ui/.../hotkey-grid.psd`、`trade-house-083/ui/**`、`map-*/ui/**`）**均非状态栏 HUD 底板/HP·MP·EXP/快捷栏产物**。
- `[推断]` 当前模型不声明图像输入，`read_image` 对 `进入游戏页.png` 返回 `cannot read ... as an image: model "hy4free" does not declare image input`，未做像素级视觉复核。

## 4. locale HUD 文案 key

- `locale/zh-CN.ini:104-111` / `en-US.ini:100-107`：`chat.target.all/whisper/party/guild/buddy/alliance/spouse/megaphone` = 对所有人/悄悄话/组队/公会/好友/联盟/夫妻/喇叭 ↔ Everyone/Whisper/Party/Guild/Buddy/Alliance/Spouse/Megaphone。
- `zh:99-103`/`en:95-99` 注释「前四项原生 v83 中文包文案；后四项 NO-NATIVE 自定（/b /a /s 与喇叭道具语义），决策 B 先建键等 v83 依据」。
- `zh:112` `whisper.hint_target=请输入悄悄话的对象。` ↔ `en:108` Enter the name of the character to whisper.（对应 Angel `SP_782_PLEASE_ENTER_THE_PERSON_YOU_WANT_TO_WHISPER_TO`）。
- `zh:117-123`/`en:113-119` `messenger.*`（self/no_room/invited/declined/prompt_chat/prompt_decline/prompt_invite）。
- `zh:131-136`/`en:128-133` `sysopt.combo.screenshot.0/1`、`sysopt.combo.mobinfo.0..3`（不显示/只显示名字/只显示HP/名字和HP都显示）。
- `zh:125-130`/`en:121-127` 注释窗体行标题 `UIWindow.img/SysOpt/backgrnd` 烘焙位图不归 i18n，条目文字来自 ijl15.dll@0x293ec/@0x2177c 并以「系统设置.png」截帧交叉验证；en `:127` 自注 NO-NATIVE 英文自造。
- `zh:140-149`/`en:136-145` `sysopt.combo.resolution.0..8` 纯数字档位（800x600 … 2560x1440）；`zh:138-139` 注「v83 原生无此行，档位表来自插件 rs_tier_dims」。
- `zh:614` `mobgage.mob=Mob` ↔ `en:611`、`mobgage.txt={}%`（BossHP/怪物血条）；`skillup.skill_up=Skill Up` `zh:616`/`en:613`、`skillup.sp=SP {}`。
- `[推断]` HP/MP/EXP 标签、LV.、菜单按钮文字均位图烘焙（`UIStatusBar.cpp:1912`「LV. 字样已烘焙」、`:2076`「HP/MP/EXP 用 gauge/bar 图集烘焙，不建 Text」），locale 无对应 key。
- stub/中文残留：`en-US.ini:160` `charselect.rank_unavailable=排行不可用`（`:156-159` 注释「source literal 是中文，无英文依据，保留同值」）；`en:198-245` R27-B1c-4 中文硬编码批次原样保留中文（`buddylist.title=好友 ({})`、`common.channel=频道{}`、`common.offline=离线`、`party.title=队伍 #{}`、`storage.meso=金币: {}`、`trade.*` 十余条、`mapsearch.*`、`mapinfo.*`、`dailygift.*` 全中文）；`en:193-196` 注释「en-US repeats the same value as a placeholder until real copy is evidenced (decision B)」；`en:441` 章节标题含中文「R27-B1c-5c 中文盲区四窗…pending-copy 待用户定稿」；`zh:151-153/:195-199/:249-253/:327-332/:480-483` 注释标 NO-NATIVE/决策 B。
- 键集合差异：`zh:353-354` `charinfo.prompt_note`/`prompt_note_to` 在 en 无对应键；`zh:635` `cube.prompt_slot` ↔ `en:632`。

## 5. Configuration HUD 设置项（键→默认值→行号）

- `Configuration.h:527-530` `ui.buff_timer="true"`（getter `:100-101`，唯一消费方 `UIBuffList.cpp:159-166`）；`:535-538` `ui.equip_compare="true"`（getter `:112-113`）；`:542-545` `ui.language="zh-CN"`（getter `:114-115`）。
- `Configuration.h:463-506` `window.fullscreen="false"(:463-466)/width="1280"(:470-473)/height="720"(:477-480)/game_width="1280"(:486-489)/game_height="720"(:491-494)/game_tier="255"(:496-499)/vsync="true"(:503-506)`。
- `Configuration.h:641-658` `chat.max="false"(:641-644)/chat.x="0"(:648-651)/chat.y="0"(:655-658)`；`:712-715` `ui.position.chat="(0, 572)"`；`:719-727` `ui.position.minimap="(5, 24)"`（`:721-726` 注释引正确端截图小地图九切片顶边 y=24、ScrollingNotice 黑底 0..22、`ColorBox(viewwidth,23,BLACK,0.535f)`）。
- `Configuration.h:814-887` `ui.gameopt.mask="4095"(:814-817)/ui.sysopt.quality="98"(:822-825)/mouse_speed="52"(:828-832)/hp_warn="52"(:836-839)/mp_warn="52"(:843-846，两者注释「当前无消费方，仅落盘」)/skill_effects="true"(:850-853)/window_mode="true"(:861-864)/alt_enter="false"(:868-871)/mob_info="3"(:876-880)/screenshot_path="0"(:884-887)`。
- `Configuration.h:966-983` `minimap.type="2"(:966-969)/minimap.simple="false"(:973-976)/minimap.default_helpers="false"(:980-983)`；`:1050-1064` `ui.maplepoints.local="0"/ui.autopot.hp_item="0"/mp_item="0"`；`:299` `VERSION="0.83"`。
- `Configuration.cpp:61-191` `SETTINGS_ORDER[]`（`:111-114` 聊天栏三项、`:116-119` 小地图三项、`:121-130` 系统设置含 HP/MP 警报 0-100、`:139-143` 界面开关、`:145-178` ui.position.*）；`:209-310` ctor `settings.emplace<...>`（`:235` BuffTimer/`:236` EquipCompare/`:237` UiLanguage/`:253-255` ChatView*/`:263` PosCHAT/`:264` PosMINIMAP/`:277-285` SysOpt*/`:300-302` MiniMap*）；`:312-321` 析构注释「优雅退出跑不到析构，要立刻生效的项必须显式 save()」；`:428-460` 登录 1280x720/游戏档位回灌；`:870-873` `get_buff_timer()`/`:894-897` `get_equip_compare()`/`:930-934` `get_ui_language()`。

> `[观察]` `ui.position.chat="(0, 572)"` 与 Angel 原生 `SetStatusValue` 画布 800×578、聊天区基准 509-h、按钮 y=543/515 **同源**（当前端 y=572 ≈ 1280x720 下 800 宽画布的换算），进一步佐证排版已对齐。

## 6. orange-wz(IMG) MCP 可用性

- `mcp_connector_status` 本会话仅 `[json-photoshop] photoshop`（json/none/stdio、全局、状态未知、Host 未注册 stdio Server 工具）；**无 orange-wz 注册**。
- `C:\Users\11036\.cursor\mcp.json:3-6` 写有 `"orange-wz": {"type":"http","url":"http://127.0.0.1:10012/mcp"}`、`:7-10` ida-pro-mcp:13337、`:11-18` photoshop stdio，但 DSH 会话实际未接入。
- `docs/features/login-flow-regression-fix/notes/img-evidence-audit.md:7`「orange-wz MCP 对这批私服改版 img 内部节点有已知解析缺陷，未使用。」；`:4-5` 唯一真值=运行时部署 `E:\MXD\BeiDou-Client-xy\Data\UI\*.img` 与 `wz_dump\*_runtime.json`；`:6` 打包 Login_img.json/MapLogin1_img.json 仅版本差异对照不作准。
- **结论：IMG MCP 未接入，资源层证据以仓库内 `.nx` 节点引用 + 运行时 dump + 正确端截图为准**（技能门禁 REA→IDA→IMG 三联的 IMG 层记阻塞，与登录面一致）。

## 7. HUD UI 资源证据覆盖度小结 `[推断]`

- **三证齐全（.nx 字面量 + 运行时 dump 尺寸 + IDA 坐标）**：底板/HP·MP·EXP 条（StatusBar.img classic 与 StatusBar3.img modern 两套完整节点树）、快捷栏（quickSlot08/26/32 + key/0..6 + EquipKey..KeySet）、菜单按钮（BtShop/BtNPT/BtMenu/BtShort、modern menu/submenu 全套）、buff 图标（Skill/Item WZ，有 `UIBuffList.cpp` 消费）、小地图（UIWindow.img/MiniMap + BtMap/BtMin/BtMax）、BossHP（DualMobGauge/MobGage 三分件 + REA `sub_56C9B7` 注释）、NameTag（三切片）、全部通用组件。
- **半覆盖**：聊天框 classic `BtWhisper`/`Basic.img/BtMax`/`BtMin` 有字面量与 `w4-chatbar` 141 件逐像素 coords；modern chat/ingame view/input 贴图本包缺失（`UIChatBar.cpp:210-214`），仅代码坐标占位。
- **只有代码坐标、无独立节点 dump**：modern StatusBar3.img 五档 position/dimension（本包无 StatusBar3 无法实测）、`UIBuffList` 时间标签 `(new_width-35,55)`、`UIBossHP` 顶栏 y=20、ScrollingNotice 23px 黑底高。
- **完全无证据**：`StatusBar3.img/OptionMenu`（本包缺，UIOptionMenu 只剩空底板壳）、modern chat/ingame 贴图、**HUD 状态栏专属 layout JSON（0 个）**、**HUD 状态栏专属 Photoshop 出图产物（0 个）**、HP/MP/EXP 烘焙标签的 locale key。
- **跨节硬约束**：`VERSION="0.83"`、BeiDou gms083、data.format=img 无 StatusBar3；`r15-baseline-ui-gap-vs-screenshots.md:35`「两端 Data/UI/*.img SHA256 全同 → 底栏『不像』不是缺资源」；底栏四键 id 2000–2003 对应 E→I→S→K（`r18-wave0-rea-incremental.md:248`）。全程未引 095 线。

（子代理只读盘点完成，未修改任何文件。）
