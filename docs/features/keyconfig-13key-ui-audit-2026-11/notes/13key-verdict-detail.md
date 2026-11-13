# 13 键逐键细证（证据链逐条挂行号）

配套 `../README.md` §3 结论表与 `../evidence/verdict_matrix.json`。每条按「键帽 → 运行时下发 → Angel 反编译 → orange-wz IMG → 本地源码 → 原生截帧」六段挂证，判词只写证据能覆盖的范围。

## 0. 运行时口径（防 staged 表假象，全 13 键共用）

- `Net/Handlers/PlayerHandlers.cpp:378-389` `KeymapHandler::handle(InPacket&)`：`recv.skip(1)` → `for (uint8_t i=0;i<90;i++) { type=read_byte(); action=read_int(); UI::get().add_keymapping(i, type, KeyAction::from_server(action)); }` → 收完 `kc->makeactive()` 刷新已开键位窗。
- `Net/KeyMapTranslate.h:30-41` `from_server(int32_t)`：`case 22: return MONSTERCOLLECTION;`（v83 MONSTERBOOK→本枚举 UIMonsterBook 入口）；`case 23: return CASHSHOP;`；`case 24: return ALLIANCECHAT;`；`case 27: return static_cast<Id>(28);`（注释「本包无勋章窗，压成无 case 的枚举空位，灭掉 BOSSPARTY(27)『; 开错窗』；勋章窗落地后改映射」）；`default: 原样透传`。`:43-53` `to_server` 双射回译保证服务端存档仍是 v83 语义。
- `IO/Keyboard.cpp:148-157` `assign(uint8_t key,uint8_t tid,int32_t action)` → `keymap[Keytable[key]]=mapping; maplekeys[key]=mapping`；`IO/UI.cpp:542-545` `UI::add_keymapping` → `keyboard->assign`。
- `IO/UITypes/UIKeyConfig.h:181-229 basic_keys` / `:134-179 alternate_keys` 是 `stage_mapping()/unstage_mapping()`（`:1065-1089`）与「默认/全部消除」（`:758/:760 staged_mappings=alternate_keys/basic_keys`，`:1257-1258 ChangeKeyMapPacket`）用的**暂存表**，不是运行时 dispatch。F 键不在这两张表却仍能触发，正是服务端 KEYMAP 下发的实证。
- `IO/KeyAction.h:27-109` 枚举 21+ 段沿用新版编号（注释 `//TOSPOUSE v83` / `//MONSTERBOOK v83` / `//FAMILY v83` / `//MEDAL v83`），`from_server` 是唯一回译边界。
- `IO/UIStateGame.cpp:250-599` `send_key` 的 `switch (KeyType::Id::MENU) → switch (action)` 是最终落点；`default:585-589` 只 `LOG_DEBUG "Unknown KeyAction::Id action: [" << action << "]"`。

## 1. Angel(v83) 反编译主表（全部 13 键的上游）

`CWvsContext::UseFuncKeyMapped(_DWORD *this, signed int a2) @0xa07759`（`tools/_ida_CWvsContext_UseFuncKeyMapped.txt` 215 行全文已读）：
`v2=BYTE2(a2); if(v2==54) v2=42; v4=*(v2 + dword_BED5A0 + 4*v2 + 5);` → `a2>=0 switch(v4)`：
`case0→v20=1`（装备）/ `case1→v20=0`（道具）/ `case2→v20=2`（能力）/ `case3→v20=3`（技能）/ `case4→v21=0,v20=7`（好友）/ `case5→CWorldMapDlg::CreateWorldMapDlg` / `case6→CUIMessenger::TryNew(0)`（聊天）/ `case7→sub_8590F9`（小地图）/ `case8→v20=6` / `case9→v20=5` / `case14→sub_A06CBC` / `case15→CUIStatusBar::ToggleQuickSlot` / `case16→SetChatType(1或3)` / `case17→v21=2,v20=7`（家族）/ `case19→v21=1,v20=7`（组队）/ `case20→sub_888230` / `case22→v20=9`（怪物手册）/ `case23→SendMigrateToShopRequest` / `case25→v20=22` / **`case26→v20=27`（FAMILY 学院）** / **`case27→v20=31`（MEDAL 勋章）** / `default return 0`；`LABEL_6 @:208` → `CWvsContext::UI_Toggle(this, a2: v20, a3: v21)`。

`CWvsContext::UI_Toggle(_DWORD *this, signed int a2, int a3) @0xa04f7a`（`evidence/ida_UI_Toggle.json` 25355 字符/629 行）：`switch(a2) @:129`；`case 0 @:609-627`、`1 @:530-548`、`2 @:585-603`、`3 @:560-578`、`5 @:346-364`、`6 @:371-392`、`9 @:394-413`、`10 @:415-436`、`22 @:446-462`、`23 @:321-337`、`26 @:131-150`、`27 @:153-172`、`28 @:175-194`、`29 @:197-216`、`30 @:219-238`、`31 @:241-262`、`32 @:265-284`、`33 @:287-308`；每支先读 `*(this+3438+2*idx)` 判开关，再 `StringPool::GetStringW(SP_2302_MENUUP 开 / SP_2303_MENUDOWN 关)` → `play_ui_sound` → `UI_Open/UI_Close(同索引,-1)`；`case 7 @:469-506` 特例 `CheckReqFriend + sub_9196F2 + sub_A1452C + sub_A05A8C`；`a2==3` 前置 `@:511-516` `GetCharacterData/_ZtlSecureFuse<short>` 判 `job/100==22 || ==2001` → `a2=33`。

`CWvsContext::UI_Open @0xa05ab8`（`evidence/ida_UI_Open_Close.json` 18924 字符）：`switch(v5) @:104` 窗口索引→构造：
`:106 case22` 组队搜索；`:157 case23 CUIItemMaker`（Alloc 3404，专业技能/制作）；`:170 case26` Ranking；**`:205 case27 CUIFamily`（Alloc 2832，FAMILY 学院）**；`:218 case28 CUIFamilyChart`（族谱）；`:231 case29 CUIOpBoardState`；`:246 case30 sub_8758D3`；**`:260 case31 sub_840170(-1)`（Alloc 2944，MEDAL 勋章）**；`:273 case32 sub_A14C24`；`:276 case33 sub_8B981D`；`:297 CUIEnergyBar`；`:314 sub_861B12`；`:331 CUIUserInfo`；`:348 CUIGuildBBS`；`:363 sub_86DBB3`；`:394 CUIKeyConfig`（3244）；`:406 CUISkill`（1648）；`:418 CUIStat`（1636）；`:430 CUIEquip`（1516）；`:442 CUIItem`（1568）；`:455 CUIQuestInfo`（1608）。

> **编号错位实证**：v83 `action26` 枚举名 `MEDALS` 但真实开 `CUIFamily`；`action27` 枚举名 `BOSSPARTY` 但真实开勋章 `sub_840170`；`action22` 是怪物手册；`action29` `PROFESSION` → `CUIItemMaker`。本地 `IO/UIStateGame.cpp:296` 把 `MEDALS(26)` 接 `UIFamily`、`:518 MONSTERCOLLECTION` 接 `UIMonsterBook`、`:524 PROFESSION` 接 `UIMaker`、`:289 MANAGELEGION` 接 `UIPartySearch`，与 Angel 逐一对上。

## 2. orange-wz IMG 门禁（AGENTS.md §1-§3）

`tools/_key13_wz_probe.py` / `tools/_key13_wz_probe2.py` 自建 HTTP 会话 `http://127.0.0.1:10012/mcp`（`initialize → notifications/initialized → load_files → query_nodes/search_node → unload_all`）；key = 128B `tools/_gms_key_128.b64`（`B64_LEN 172`，`EwAAAFIAAAAqAAAAWwAAAAgA...`），`ivBase64="TSPHKw=="`，`exclusive:false`。`load isError:false loadedCount:1`（probe1）/ `loadedCount:2 rootCount:2`（probe2），全程无 `IMG 解析失败` / `out of bounds`，故按 AGENTS.md §2 **不触发 .nx 回退**。

- `evidence/orange_wz_window_nodes.json`：17 窗口根 `list_children`。13 有真节点：`KeyConfig(12)/Equip(11)/Item(15)/Stat(11)/Skill(11)/UserList(9)/Family(10)/Maker(10)/MiniMap(5)/Messenger(7)/WorldMap(2)/MonsterBook(15)/PartySearch(13)`；4 `节点路径不存在`（GBK 原始字节，UTF-8 显示乱码）：`QuestLog/Whisper/Medal/GuildBBS`。
- `evidence/orange_wz_window2_nodes.json`：补查 `UIWindow2.img` 第二根 + 6 关键字全搜。`search:Medal` 在**两根均 `matches:[]`**；`win:Medal/MedalQuest/MedalList` 在两根全 `节点路径不存在`；`search:Whisper` 命中 `ContextMenu/BtWhisper`、`UserList/Friend/BtWhisper`、`UserList/Friend/Popup/GroupWhisper`、`UserList/Friend/BtGroupWhisper`、`UserList/Party/BtWhisper`、`UserList/Guild/GuildInfo/BtWhisper`；`search:Quest` 命中 `Quest/QuestAlarm/TimeQuest/QuestIcon/ShortCut/BtQuest/CancelRequests`；`search:UtilDlg` 命中 `UtilDlgEx/UtilDlgEx_Avatar/UtilDlgEx_Pet/UtilDlgEx_MultiPetEquip`；`search:Notice` 命中 `UtilDlgEx/notice`、`Quest/notice*`、`FloatNotice`。
- `E:/MXD/BeiDou-Client_S9/Data/UI/` 24 个 `.img`（`ops_glob` 全量）：`Basic/BuffIcon/CashShop/CashShopPreview/ChatBalloon/DeathCount/DialogImage/GuildBBS/GuildMark/ITC/ITCPreview/Login/Logo/MainNotice/MapLogin/MapleEzorsiaV2wzfiles/MapleTV/NameTag/New/Npt_GuideLine/StatusBar/UIWindow/UIWindow2/tutorial`，**无 `Medal.img`**。
- `evidence/button_gap_matrix.md`（v2 口径 = 源码任意 `"Bt[A-Za-z0-9_]*"` 字面量，覆盖 `put()`/`try_maple_btn()`/`[""]` 三写法 + `uses_generic_close_btn`）：17 窗口逐窗 IMG 子节点 / IMG Bt* / 代码 Bt* / 已对上 / 通用关闭钮 / IMG 独有 / 代码独有全表。

## 3. 逐键六段细证

### S · 能力值 → `UIStatsInfo`（⚠️ PARTIAL）
- 截帧键帽「能力值」；v83 action 2 → `STATS(2)`；`UIStateGame.cpp:337-345` → `UIStatsInfo(stats)`。
- Angel：`case2→v20=2` → `UI_Toggle a2=2 @:585-603` → `UI_Open:418 CUIStat`（1636）。`tools/_ida_CUIStat_OnCreate.txt` `CUIStat::OnCreate @0x8c4c97` 逐字只建 `2006/(124,324)` 的 BtDetail，无 BtAuto 创建点。
- IMG `UIWindow.img/Stat` 11 子节点：`basicStat/BtApUp/BtDetail/Job/BtAuto/BtAuto1/BtAuto2/Disabled/backgrnd2/backgrnd/backgrnd4`；IMG Bt* 5 / 代码 12，已对上 3。
- 本地 `IO/UITypes/UIStatsInfo.cpp:59-179`：`:62 window4("Stat")`，`:75 classic = !usable(main)`，`:89-90 sprites.emplace_back(Stat["backgrnd"])`，`:92 classic_detail=Stat["backgrnd2"]`，`:93 classic_help=Stat["basicStat"]`（绘于 (8,195)），`:95-98 Disabled/STR..LUK`；`:127 BT_CLOSE(191,5)`；`:129-135` 六颗 `BtApUp` 复用件（modern 名回退 `ap_up`）；`:139-144 BT_HP/MP/STR/DEX/INT/LUK` 六行 `(186,115/133/245/263/281/299)`；`:146-158` 长注释自述 `BtAuto x=106` 由 `UIWindow2.img/Stat/main/BtAuto` canvas.origin(-94,-198) 推、属推断；`:162 BT_DETAILOPEN(153,324)`；`:167 BT_DETAILCLOSE` 用 `Basic.img/BtHide`(13x12) 近似，落 `(405,326)`；`:169-170` 明示 HyperStatOpen/Close、detail/BtAbility、detail/BtHpUp 为本包不存在的现代节点，死代码已删。
- 功能：`:540 update_all_stats` / `:601 update_stat` / `:641 button_pressed` / `:845 update_ap` / `:858 update_exp_label` / `:883 update_simple` / `:888 update_basevstotal` / `:911 update_buffed` 全实现。
- 排版：`打开了S-E-I-K页面.png` S 窗左中，名字/职业/等级/经验/HP/MP/人气度表 + 基本属性点说明 + 右侧 攻击力/物防/魔防/回避/命中/手技/移速/跳跃 面板 + 底部详细能力钮 + 右上 X；四窗同屏互不遮挡。
- 判词：布局/功能/排版吻合，**D6** `BtDetail` 本地(153,324) vs Angel(124,324) 差 29px；**D7** 近似件+推断坐标+比例文字无位图字库（`ui-v83-window-restore/README.md:75`）→ PARTIAL。

### E · 装备窗 → `UIEquipInventory`（✅ OK）
- v83 action 0 → `EQUIPMENT(0)`；`UIStateGame.cpp:319-327` → `UIEquipInventory`（`window4("Equip")`）。
- Angel：`case0→v20=1` → `UI_Toggle a2=1 @:530-548` → `UI_Open:430 CUIEquip`（1516）。
- IMG `UIWindow.img/Equip` 11 子节点，IMG Bt* 7 / 代码 7 **全对上**（`evidence/button_gap_matrix.md:11,36-40`）。
- 本地 `UIEquipInventory.cpp:92 window4("Equip")`，`:112-132 iconpositions` 装备槽坐标，`:134-142 backgrnd`，`:145 close(bg.x-19,5)`，`:150 BtSlot(4,bg.y-25)`，`:152 BtCashshop(54,bg.y-21)`，`:190/:204` 宠物钮；`:276 button_pressed` 覆盖 `BT_CLOSE/BT_SHOP/BT_SLOT/BT_PET_SHOW/BT_PET_HIDE/BT_PET_TAB0..2`（`:636-637` 注释「不能用 DISABLED 表示未选页，用 set_state(PRESSED/NORMAL)」）。
- 截帧：帽子/脸饰/眼饰/戒指×2/披风/项链/武器/盾牌/手套/上衣/腰带-指环/指环/裤子/鞋子/勋章 槽 + 底部「装备整理」+ 宠物 3 图标 + 左下 X。
- 判词：六维全对齐 → OK。

### I · 道具 → `UIItemInventory`（✅ OK）
- v83 action 1 → `ITEMS(1)`；`UIStateGame.cpp:328-336`。
- Angel：`case1→v20=0` → `UI_Toggle a2=0 @:609-627` → `UI_Open:442 CUIItem`（1568）；`tools/_ida_CUIItem_OnCreate.txt @0x81c6df` `SP_4499_UI_UIWINDOWIMG_ITEM_BACKGRND` + `CCtrlTab(2000,1,3,23,170)`。
- IMG `UIWindow.img/Item` 15 子节点，IMG Bt* 6 / 代码 6 **全对上**。
- 本地 `UIItemInventory.cpp:133 window("Item")`，`:211 Tab/enabled+disabled`，`:214-215 New/Tab1+Tab0`，`:268-280 BtCoin/BtFull/BtSmall/BtGather/BtSort/BtCashshop`；`:504 button_pressed` 20+ case（`:510 BT_CLOSE`、`:516-541` 六页签、`:552-695` gather/sort/full/small/coin/extract/disassemble/point/upgrade/appraise/toad、`:697 BT_CASHSHOP`）。
- 截帧「现有道具窗」5 页签 + 道具格 + 金币 0 + 展开/收起箭头。
- 判词：OK。

### K · 技能 → `UISkillBook`（⚠️ PARTIAL）
- v83 action 3 → `SKILLS(3)`；`UIStateGame.cpp:346-355`。
- Angel：`case3→v20=3` → `UI_Toggle a2=3 @:560-578`（前置 `@:511-516` job/100==22 或 2001 → a2=33）→ `UI_Open:406 CUISkill`（1648）。
- IMG `UIWindow.img/Skill` 11 子节点但 **IMG Bt* 只有 `BtSpUp` 1 颗**；代码 10 颗（`button_gap_matrix.md:14,54-58`）：`code_only = BtCancle/BtGuildSkill/BtHyper/BtMacro/BtOK/BtOkay/BtRide/BtSpDown/BtSpMax`。
- 本地 `UISkillBook.cpp:144 window("Skill")`，`:161 main/backgrnd`，`:170-175 BtHyper/BtGuildSkill/BtRide/BtMacro`，`:184 skillPoint`，`:192-196 BtCancle/BtOkay/BtSpDown/BtSpMax/BtSpUp`，`:237 Tab`；`:515 button_pressed`，`:902 update_skills`。
- 排版：`ui-v83-window-restore/README.md:78` 5 tab 槽0中心21 间距33 + `CLASSIC_SKILL_OFFSET(7,99)` + `BtMacro(121,265)`；截帧「冒险家技能」+ 团队治疗/疾风步/蜗牛投掷术/精灵的祝福 4 行 + 技能点 0 + 布技能↑。
- 判词：功能/排版吻合，9 钮无本包 v83 IMG 背书（modern 资源或回退）→ PARTIAL。

### P · 组队 → `UIUserList Tab::PARTY`（✅ OK）
- v83 action 19 → `PARTY(19)`；`UIStateGame.cpp:356-396` 共用窗 Tab 分发。
- Angel：`case19→v21=1 v20=7` → `UI_Toggle a2=7 @:469-506`（`CheckReqFriend` 特例）。
- IMG `UIWindow.img/UserList` 9 子节点 `backgrnd/backgrnd2/backgrnd3/Friend/Party/Guild/Tab/BlackList/GuildUnion`，根级 **IMG Bt* = 0**（按钮全在 `UserList/Party/*`、`UserList/Friend/*` 子层）；代码 39。
- 本地 `UIUserList.cpp:58-100`：`UIDragElement<PosUSERLIST>(Point(260,20))`，`:62-73 Main=UserList["Main"] 回退 UserList`，`:75-80` 三张互斥底按页签单张选画（好友=backgrnd3 meanSAD 11.6 / 家族联盟=backgrnd2 7.6 / 组员·拒绝=backgrnd），`:83 close_btn→BT_CLOSE(288,4)`，`:92-98 Tab/enabled+disabled` 五格 `TAB_POS[5]={(4,23),(35,22),(66,22),(96,23),(149,22)}`；`:339 update`、`:606 button_pressed`。`UIUserList.h:45-58` 9 个 Tab；`:83-144` 约 60 个 Buttons；`:75 bind_runtime_assets`。
- 截帧「好友、组员、家族、家族联盟、拒绝列表.png」310x386 坐实 5 页签 + 底部 3 行×4 列 12 钮。
- 判词：OK。

### F · 学院 → `UIFamily`（✅ OK）
- 键帽「学院」；v83 action 26（服务端 KEYMAP 下发，不在 staged 表）→ 本枚举 `MEDALS(26)`；`UIStateGame.cpp:296-309` `OpenFamilyPacket().dispatch() + UIFamily`，注释 `:298`「v83 F(33 键) action 26 = FAMILY 学院（家族）窗」。
- Angel：`case26→v20=27` → `UI_Toggle a2=27 @:153-172` → `UI_Open:205 CUIFamily(a1: v53, a2: v54, a3: (this+3585), a4: (this+3591))`（Alloc 2832）。
- IMG `UIWindow.img/Family` 10 子节点，IMG Bt* 8 / 代码 8 **全对上**（`button_gap_matrix.md:16,66-70`）。
- 本地 `UIFamily.cpp:59-106`：`position(280,60) dimension(224,392) dragarea(224,20)`；`:85-87 auto put = [&](Buttons id,const char* name,int16_t x,int16_t y){ if(root[name]) buttons[id]=make_unique<MapleButton>(root[name],Point<int16_t>(x,y)); };`；`:89 BtClose(dim.x-21,5)`、`:91 BtOK(92,dim.y-26)`、`:93 BtTree(166,76)`、`:95 BtLeft(12,152)`、`:96 BtRight(196,152)`、`:98 BtJuniorEntry(96,76)`、`:99 BtFamilyPrecept(160,76)`、`:101 BtSpecial(150,322)`；`:108-130 refresh_from_model`。`:353-389 button_pressed`：`BtClose/BtOK deactivate`、`BtTree → OpenFamilyPedigreePacket + emplace<UIFamilyTree>`、`BtJunior → FamilyAddPacket`（无目标名则 `UISoftKey`）、`BtPrecept → FamilyPreceptPacket`、`BtSpecial → use_selected()`、`BtLeft/BtRight → page_privilege(±1)`、else 族谱 + LOG。
- 截帧「学院.png」227x392：「(还没有冒险学院)」蓝条 + 同学 0/2「登录同学」「↑小谱」+ 名声度 0/0「今天获得的名声 +0」+「名人冒险家特权 [01/11]」左右箭头 + 消耗名声 300 / 限制次数 0/1 + 描述「[对象]我 [效果]直接移动到学院成员身边」+ 右下「使用特权」+ 底部「确认」橙钮；8 钮全部可见。
- 判词：8 钮/功能/排版全对齐，仅 **D8** 224 vs 227 3px 裁切容差 → OK。

### G · 家族 → `UIUserList Tab::GUILD`（✅ OK）
- v83 action 17 → `GUILD(17)`；`UIStateGame.cpp:361-363` 注释「v83 G(17) = GUILD「家族」…不是独立 UIGuild 浮窗」；`UIMiscShells.cpp:3964 emplace<UIUserList>(Tab::GUILD)`、`:5903/:5907-5908` 复用/切页签。
- Angel：`case17→v21=2 v20=7` → 共用窗。
- IMG `search:Guild` 命中 `UserList/Guild`、`UserList/Guild/GuildInfo`、`guildmark`、`guildinfo0/1`；本地 `UIUserList.cpp:49-56` `GUILD_ROW_H=15 / ONLINE_ROWS=2 Y=157 / OFFLINE_ROWS=5 Y=217`。
- 截帧「好友、组员、家族、家族联盟、拒绝列表.png」**第 3 页签「家族」红字选中**：「没有参加的家族」横条 + 右侧小钮；在线家族 [0/0][1/1] 蓝条 + 名字/职业/等级/地位列头 + none；离线家族同；右侧滚动条；底部 12 钮。
- 判词：坐实「共用窗第 3 页签」而非独立浮窗，与 `MEMORY.md` 口径一致 → OK。

### R · 好友 → `UIUserList Tab::FRIEND`（✅ OK）
- v83 action 4 → `FRIENDS(4)`；Angel `case4→v21=0 v20=7`；`UIMiscShells.cpp:3926 emplace<UIUserList>(Tab::FRIEND)`。
- IMG `UIWindow.img/UserList/Friend` 有 `BtWhisper/BtGroupWhisper/Popup/GroupWhisper`（`search:Whisper` 命中）；代码 `BtAddFriend/BtPlusFriend/BtBlock/...`。
- 截帧「好友列表框.png」坐实第 1 页签。
- 判词：OK。

### H · 悄悄话 → `UIWhisperTargetDlg`（✅ OK）
- v83 action 11 → `WHISPER(11)`；`UIStateGame.cpp:273-288` 开 `UIWhisperTargetDlg`（回调 `UIChatBar::set_whisper_target`）。
- Angel：`UseFuncKeyMapped a2<0` 分支 `CUIStatusBar::ChangeWhisperTarget`；`tools/_ida_CUIStatusBar_OnKey.txt @0x8d53ab` 的 Enter/Tab 处理对应。
- IMG：`UIWindow.img/Whisper` **NOT FOUND**；真资源 = `Basic.img/Notice4(t,c,s)` + `Basic.img/ComboBox` + `UIWindow.img/UtilDlgEx/BtOK,BtNo`（`orange_wz_window2_nodes.json` `search:UtilDlg` 命中 4 个 UtilDlgEx* 根）。
- 本地 `IO/UITypes/UIMiscShells.cpp:7506-7858` 长注释：版式真源 = 原生截帧 `whisper-dlg-native.png`(261x117) + `Basic.img/Notice4` 逐行左带反查（`utildlg-lookup.py` / `_utildlg_lookup.txt`），折算 = 截帧坐标 + (3,1)，Notice4/c 原生纵向拉到 22 故 `MID_H=22`；`:7521 ctor` `dimension(FRAME_W, frame_height())` + `center_on_screen()`；`:7530-7549` 三态 ComboBox；`:7551-7555 has_assets` 缺件 diag；`:7557-7564` 确定/取消用 `UtilDlgEx/BtOK,BtNo`（实测 46x18，`Basic.img/BtOK` 是 65x24 会溢出右沿）；`:7570-7581` enter 回调 / ESC 两级退 / `set_text_shift(0,-9)`；`:7640 open`、`:7660 pick_candidate`、`:7671 confirm → chat->set_whisper_target(name)`、`:7694 update`、`:7720 draw`、`:7767 send_cursor`、`:7821 send_key`、`:7843 button_pressed`。`UIChatBar.cpp:1759/:1764/:2129/:2136` 复用/新建弹层，`:2140 set_whisper_target`。
- 截帧「悄悄话框.png」261x117：「请输入悄悄话的对象。」+ 单行输入框 + 底部「确定」「取消」两颗橙钮。
- 判词：布局/按钮/功能/排版全对齐，资源复用口径有据 → OK。

### B · 物手册（怪物手册）→ `UIMonsterBook`（❌ MISSING）
- 键帽「物手册」；v83 action 22 → `KeyMapTranslate.h:34 from_server(22)=MONSTERCOLLECTION` → `UIStateGame.cpp:518-523 UIMonsterBook`。（staged 表绑 `PROFESSION(29)` 是「默认键位」口径，运行时已被回译覆盖，不是绑错窗。）
- Angel：`case22→v20=9` → 怪物手册。
- IMG `UIWindow.img/MonsterBook` **15 子节点**：`backgrnd/cardSlot/infoPage/select/cover/fullMark/arrowLeft/arrowRight/icon/BtClose/BtSearch/LeftTab/LeftTabInfo/RightTab/ContextMenu`；本地仅用 `backgrnd` + 通用 close（`button_gap_matrix.md:21,96-100` IMG Bt* 2 / 代码 0，13 未绑）。
- 本地 `UIMonsterBook.cpp:46-123`：`:51 dimension(400,480)`，`:54-60` 有 `backgrnd` 则 `dimension=background.get_dimensions()`，`:62-65` 仅 `close_btn → BtClose(dim.x-21,6)`，`:68 list_origin(20,90)`，`:72-89 load_cards()` 从 `Stage::get().get_player().get_monsterbook()` 读 cards 生成纯文本行，`:96-111 draw` 画 `background/title/cover_label/total_label/card_labels/buttons`，`:113-123 button_pressed` **只有 `BtClose`**，`:125-139 send_cursor` 点卡片 `MonsterBookSetCoverPacket`。
- 截帧「怪物手册.png」540x357：打开的书本双页造型，左页 卡片图标×1 +「怪物手册等级 0 / 总登记卡片数量 0 / 特殊卡片数量 0 / 一般卡片数量 0 / 手册书签」；右页 空白星座状底纹；左侧 8 色便签竖条；右侧 4 页签 基本情报/怪物情报/战利品/出没地区；右上 X。另有「怪物手册-选择便签.png」「怪物手册2.png」佐证便签/翻页态。
- 判词：**D2 P0**：纯文本列表桩 vs 书本双页，无搜索/翻页/便签/页签/卡片图标，13 个 IMG 节点未绑，排版差距大 → MISSING。

### C · 聊天 → `UIMessenger`（⚠️ PARTIAL）
- 键帽「聊天」；v83 action 6 → `MAPLECHAT(6)`；`UIStateGame.cpp:403-425 UIMessenger::try_create_room` 回退 `UIChat`。
- Angel：`case6→CUIMessenger::TryNew(0)`。
- IMG `UIWindow.img/Messenger` 7 子节点：`backgrnd3/blink/chatBalloon/BtEnter/NameBar/backgrnd/backgrnd2`；代码 Bt* 2（`BtEnter` 对上 + `BtClose3` 独有走 modern 资源，`button_gap_matrix.md:19,84-88`）；7 节点仅用 4。
- 本地 `UIMessenger.cpp:170 balloon_proto=Animation(root["chatBalloon"])`（真逐帧动画），`:172-175 chat_input CHAT_INPUT_* 常量定死（切片 origin 全 0，窗尺寸=backgrnd 295x364）`，`:178-189 try_create_room → MessengerOpenPacket(0) + set_local_pos(0) 乐观占位`，`:191-205 toggle_active`，`:207 leave_room`，`:599 send_key`（`:61-62` 注释「焦点在输入框时按键由 UI::send_key 整条交给 Textfield，本窗 send_key 走不到」），`:667-683 button_pressed`（`BtClose/BtEnter`）。
- 判词：功能与窗内逐帧动画有，但 IMG 7 节点仅用 4、无本包独立聊天室截帧对照 → PARTIAL。

### ;（分号）· 勋章 → **无 UI**（❌ MISSING）
- 键帽「勋章」（放大图上 `;` 键帽，视觉口供曾误作「技能消息」，已作废）；v83 action 27 → `KeyMapTranslate.h:37 from_server(27)=static_cast<Id>(28)`（无 case 空位）→ `UIStateGame.cpp:585-589 default` 只打 `Unknown KeyAction::Id action: [28]` → **死键**。
- Angel：`case27→v20=31` → `UI_Toggle a2=31 @:241-262` → `UI_Open:260 sub_840170(-1)`（Alloc 2944）真勋章窗。
- IMG：`UIWindow.img` 与 `UIWindow2.img` `search:Medal` 均 `matches:[]`；`win:Medal/MedalQuest/MedalList` 两根全 `节点路径不存在`；`Data/UI/` 24 个 `.img` 无 `Medal.img` → 客户端包连勋章窗素材根都缺。
- 截帧「勋章框.png」255x372：标题「勋章」+ 5 页签 基本/职业/一般/挑战/活动（基本红选中）+「获得过的勋章个数:0个」横条 + 空列表 + 右侧滚动条。
- 判词：**D1 P0**：整窗未落地。修复面 = 补资源 + `UIMedal` 类 + `from_server(27)` 映射 + `UIStateGame` case + `to_server` 双射 → MISSING。

### M · 小地图 → `UIMiniMap`（⚠️ PARTIAL）
- 键帽「小地图」；v83 action 7 → `MINIMAP(7)`；`UIStateGame.cpp:426-435 UIMiniMap::send_key 或 emplace<UIMiniMap>`。
- Angel：`case7→sub_8590F9`。
- IMG `UIWindow.img/MiniMap` 5 子节点 `title/MinMap/MaxMap/Min/BtMap`；代码 Bt* 3（`BtMap` 对上 + `BtMax/BtMin` 独有走 `Basic.img` 真件，`button_gap_matrix.md:18,78-82`）。
- 本地 `UIMiniMap.h:78-86` 6 钮 `BT_MIN/BT_MAX/BT_SMALL/BT_BIG/BT_MAP/BT_NPC`，`:107 Animation player_marker`，`:123-125 OutlinedText region_text/town_text`；`.cpp:116 player_marker=Animation(marker["user"])`、`:117 selected_marker=Animation(MiniMap["iconNpc"])`、`:142/:170 portal_marker`、`:815-843` 玩家/NPC/another/portal 逐帧 draw、`:980 npc_marker`；`:479-544 button_pressed`：`BT_MIN/BT_MAX` 逐级切 `Type::MIN/NORMAL/MAX` + `persist()`（`Setting<MiniMapType>::save` + `Configuration::get().save()`）+ `toggle_buttons()`、`BT_SMALL/BT_BIG → big_map=!big_map` 后 `:516 // TODO: Toggle scrolling map`、`BT_MAP → UIWorldMap`（`:522-525` 注释 BtMap 贴图 36x12 橙底白字「大地图」）、`BT_NPC → set_npclist_active`。
- 截帧库 `小地图.png`/`小地图1.png`/`小地图2（展开）.png`/`小地图收起状态.png` 4 张 + `射手村大地图、小地图.png` 坐实三态；`ui-v83-window-restore/README.md:101` 记「BtSmall/BtBig/BtNpc 显式预留」。
- 判词：布局/动画/主要功能/排版吻合，**D10** `BT_SMALL/BT_BIG` 滚动地图 TODO → PARTIAL。

## 4. 关联缺口（不在 13 键内但同源，P8 揪头发顺手登记）

- **D4 `UIMaker`（`PROFESSION` action29，与 B 键回译链同源）**：`UIStateGame.cpp:524-529` → `UIMaker(window("Maker"))`；Angel `UI_Open:157 CUIItemMaker`（Alloc 3404）。`UIMaker.cpp:39-78` 只 `close_btn` + `UISoftKey` 输入 itemId → `MakerCreatePacket(want).dispatch()`；`:80-110 load_recipes` 优先 `nl::nx::Maker["recipe"]` 兜底 `g_maker_s9_recipes`。IMG `UIWindow.img/Maker` 10 节点 `BtStart/BtCancel/BtDown/GaugeBar/ComboBox1/ComboBox2/randomMake/randomRecipe/notUse/backgrnd` 中 3 钮 + 进度条 + 下拉未绑；`:163 button_pressed` 只有 `BtClose`。
- **D5 `UIKeyConfig` 自身 `BtHelp` 未绑**：IMG 7 / 代码 6（`button_gap_matrix.md:10,30-34`），正确截图键盘设定右上「?」+「点击"帮助"按钮打开的脚本框.png」佐证。5 颗按钮 `y=234`（`UIKeyConfig.cpp:127-131`）来自截帧逐像素（`:118-125` 长注释），无 083 IDA 立即数背书 → **D9**。
- **D3 窗口开合补间（全局 13 键）**：`UI_Toggle` 每支 `SP_2302_MENUUP/SP_2303_MENUDOWN + play_ui_sound`；本地 `UIStateGame` 未覆写 `UIState.h:58 activate_after_transition`（默认空）、`:60 bring_to_front`（默认空），仅 `UIStateLogin.cpp:291` 覆写；`makeactive()/deactivate()`（`UIElement.cpp:128+`）瞬时切。按钮帧动画 `MapleButton.cpp:85-88 animations[state].update(6)` 与 `UIElement.cpp:99-116 draw(float alpha)` 插值透传都在，缺的是**窗口级**这层。

## 5. 本轮未采信/已排除

- `vision_describe` 首轮键帽口供（S→技能、K→技能消息、R→组队、B→组队、C→组队、分号→技能消息、M→地图）与放大图逐字冲突，判幻觉整段作废；`vision_ocr` 后端 `VISION_BACKEND_UNAVAILABLE`（Qwen3.5 unexpected response shape / Qwen2.5-VL timeout），本轮不重试，改 `read_image` 直读原生帧（门禁 18 合法备选）。
- v1 差集脚本只抓 `["Bt*"]` 字面量导致 `Family/Maker/MonsterBook code_buttons=0` 假象；v2 重写口径后 Family 8/8 全对，Maker/MonsterBook 的缺口是真缺口（已逐行读源码确认）。
- 「分号/B 绑错窗」初判（基于 staged 表）已由 `KeyMapTranslate.h:34/:37` 消解：B 运行时正确开怪物手册，分号是**整窗未落地**而非绑错。
- `StatusBar.img/KeyConfig NOT FOUND` 与 `UIWindow.img/KeyConfig` 12 节点并存；本地 `UIStateGame.cpp:445-470` 先 `StatusBar3.img/KeyConfig` 回退 `window("KeyConfig")`，属双路兜底设计。
