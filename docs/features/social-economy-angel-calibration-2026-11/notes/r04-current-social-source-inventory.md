# 社交经济面 · 当前端源码现状盘点（r04）

> 本文件 = 第一路只读 subagent `7f2fcff9` 返回 21K 全文的结构化归档 + 主 agent §0 销案复核块。
> 基准：GMS 083 原版；禁止引 095 线符号/IDB 作依据。F 键=学院(FAMILY/UIFamily)；
> G 键=家族(GUILD，原版 UIUserList 第 3 页签)。路径统一正斜杠、相对 `E:\project\MapleStory-Client`。
> [观察]=源码直接证据；[推断]=基于证据判断。全程只读，未修改任何文件。

---

## 0. 主 agent 前置复核块（销案复核 4 项：3 项确认，1 项修正行号）

1. **G 键第 3 页签确认，行号修正**：`IO/UIStateGame.cpp:356-393`（原 r02 记 :361-384
   需扩到 :393）— `:359 case KeyAction::Id::GUILD`、`:361-363` 注释「v83 G(17) =
   GUILD「家族」…原生是「好友/组员/家族/家族联盟/拒绝列表」共用窗的第 3 页签，不是
   独立 UIGuild 浮窗」、`:374-377 BOSSPARTY+GUILD→tab = UIUserList::Tab::GUILD;`、
   `:380-384` 已开则 `change_tab(tab)`、`:388-389` 未开则
   `if (UiAssetResolve::usable(UiAssetResolve::window("UserList"))) emplace<UIUserList>(tab);`。[观察]
   → **else 分支 emplace 在 :388-389，完整逻辑到 :393**，与 Angel r01 公会控件组
   2030–2041（tab index 2）逐点互证。
2. **5 页签结构确认**：`IO/UITypes/UIUserList.h:45-58 enum Tab{FRIEND,PARTY,GUILD,BLACKLIST,
   PARTY_MINE,PARTY_SEARCH,FRIEND_ALL,FRIEND_ONLINE,UNION}`（GUILD 是第 3 个值=2）；
   `:176-179` 注释「家族页 = 083 原版「玩家列表 & 家族」共用窗第 3 页签…数据源
   GuildModel，非 UIGuild 浮窗实例」；`IO/UITypes/UIUserList.cpp:1441-1456 tab_button()`
   `:1447-1448 case Tab::GUILD: return Buttons::BT_TAB_GUILD;`。[观察]
3. **MESSENGER c2s 122 五 mode 确认**：`Net/Packets/MessengerPackets.h:13 MessengerOpenPacket
   +:18 write_byte(0x00)` / `:24 LeavePacket +:29 0x02` / `:34 InvitePacket +:39 0x03`
   / `:45 DeclinePacket +:50 0x05` / `:56 ChatPacket +:61 0x06`，全部
   `OutPacket(Opcode::MESSENGER)`（122）。[观察]
4. **学院≠家族双类树确认 + 污染源扩大**：`IO/UITypes/UIFamily.h:39 TYPE=FAMILY` vs
   `IO/UITypes/UIGuild.h:37 TYPE=GUILD`；`Character/FamilyModel.h:55 class FamilyModel`
   vs `Character/GuildModel.h:48 class GuildModel`（`:5-6` 明写「083 原版『家族』不是
   独立浮窗，而是 UIUserList 第 3 页签」）。[观察]
   **⚠️ 095 污染源比 r02 记的更广**：除 `IO/UITypes/UIMessenger.cpp` 10 处 v95 注释
   （`:136/:159/:168/:237/:278/:290/:410/:486/:537/:549`）外，`IO/UITypes/UIMessenger.h`
   另有 v95 IDA 立即数注 `:106-108`（`CUIMessenger::SetCtrl 0x7F34C0` `(276,6)`）/
   `:121-122`（`StartBlink`）/`:126-127`/`:133`（`m_tRestUpdateState=3000`）——
   **一并计入污染源，处置 = 只换注释依据引 Angel `CUIMessenger`(26 方法)，不改行为/协议**。[观察+推断]

---

## 1. `IO/UIStateGame.cpp` 社交 send_key / emplace 分支 + `IO/UIElement.h` 社交 Type 枚举

### send_key / emplace（960 行）
- `:33/:47/:52/:53/:54/:56` 社交头 include（UIMessenger/UIUserList/UIGuild/UIFamily/UIPartySearch/FamilyPackets）。[观察]
- `:70-108` ctor emplace：`:78 UIStatusMessenger`、`:83-84 UIStatusBar+UIChatBar(if has_game_hud)`、
  `:91-99 UIMiniMap/UIBuffList/UITemporaryStatView/UIEnergyBar/UIComboCounter/UIBossHP/UIClock`、
  `:101-102` 注释「Shop window is inactive until an NPC opens it」、
  `:103-104 if usable(shop_root()) emplace<UIShop>(look, inventory);`
  → **构造期无社交窗常驻 emplace**。[观察]
- `:216-229` 地图空白右键 `UIContextMenu` `:226 cm->open_at(pos, 0, Stage::get().get_player().get_name());`。[观察]
- `:273-288 WHISPER`：`:275 // v83 H(11) = WHISPER：原生「请输入悄悄话的对象」弹层；`
  `UIWhisperTargetDlg` → `makeactive()/open()` 或 `emplace<UIWhisperTargetDlg>()`；回调
  `ChatBar::set_whisper_target`。[观察]
- `:289-295 MANAGELEGION`：`:291 // v83 O(25) = PARTYSEARCH 组队搜索窗（枚举名沿用新版军团管理）`
  `:292-293 if usable(window("PartySearch")) emplace<UIPartySearch>();`。[观察]
- `:296-309 MEDALS`：`:298 // v83 F(33 键) action 26 = FAMILY 学院（家族）窗`
  `:299 // KeyAction.h:55 MEDALS // FAMILY (v83) 为编号错位实证`
  `:302 OpenFamilyPacket().dispatch();` `:303-307` get/emplace<UIFamily>() 或 makeactive()
  → **F 键 = 学院窗 UIFamily**。[观察]
- `:310-318 NOTIFIER`：`:314 // = notifier 列表封包 —— 服务端不存在 ⇒ 按门禁 7 不做假窗`
  `:316 LOG(LOG_DEBUG, "UIStateGame: NOTIFIER key (v83 L) — no s9 notifier backend, intentionally inert");`。[观察]
- `:356-393` FRIENDS/PARTY/BOSSPARTY/GUILD→UIUserList::Tab（见 §0）。[观察]
- `:403-425 MAPLECHAT`：`:405 // Wave A：优先 Messenger；无资源时回退 socialChatEnter/UIChat`
  `:406-413 if usable(window("Messenger")) { emplace<UIMessenger>(); m->try_create_room(); }`
  `:416-422` 否则 socialChatEnter→emplace<UIChat>()。[观察]
- `:471-482` SAY/PARTYCHAT/FRIENDSCHAT/GUILDCHAT/ALLIANCECHAT/TOSPOUSE →
  `chatbar->change_target(action);`（落点 `IO/UITypes/UIChatBar.h:123` 空 stub，跨面死链）。[观察]
- `:530-535 EVENT`：`:532 // 095 自定义中文 UI：暂时复用 EVENT 键打开福利中心…`
  `emplace<UISpecial>();`（095 线自述，非校准证据）。[观察]
- `:799-832` emplace 模板；`:808-820` silent_types 11 类
  （STATUSMESSENGER/STATUSBAR/CHATBAR/MINIMAP/BUFFLIST/ENERGYBAR/COMBOCOUNTER/BOSSHP/CLOCK/NPCTALK/SHOP）
  社交窗不在其中 → `:827 Sound::MENUUP`。[观察]

### `IO/UIElement.h:41-204 enum Type` 社交经济值
- `:59 MESSENGER`/`:60 PARTYSEARCH`/`:61 DELIVERY`/`:62 STOREBANK`/`:67 FAMILY`/
  `:68 FAMILYTREE`/`:69 MEMO`/`:74 MTS`/`:75 PARTYRACE`/`:76 WEDDING`/
  `:77 MONSTERCARNIVAL`/`:78 RPSGAME`/`:79 ARIANTMATCH`/`:80 NEWYEARSCARD`/
  `:81 MINIGAME`/`:82 TREASUREBOX`/`:86 MAPLETV`。[观察]
- `:113 PERSONALSHOP`/`:114 MEMBERSHOP`/`:115 TRADINGROOM`/`:116 ENTRUSTEDSHOP`/
  `:117 CASHTRADINGROOM`/`:118 ITEMSEARCH`。[观察]
- `:153 // 悄悄话目标弹层（原生「请输入悄悄话的对象。」那一层）。` + `:154 WHISPERTARGET`。[观察]
- `:161 SHOP`/`:168 USERLIST`/`:171 CHAT`/`:182 STORAGE`/`:183 PARTY`/`:184 BUDDYLIST`/
  `:185 TRADE`/`:186 GUILD`。[观察]
- `:187 /// 公会公告板独立窗（UI/GuildBBS.img）；与 GUILD 成员窗分型，避免互相抢焦点。`
  + `:188 GUILDBBS`；`:189 /// 公会 Boss PQ HUD（healer Y / pulley state，s2c 287/288）`
  + `:190 GUILDBOSS`；`:191 MERCHANT`；`:194 ALLIANCE`。[观察]
- `:195-199` GAMEOPT/SYSOPT 另立两型注释（UIOptionMenu 空壳）；`:200 GAMEOPT`/
  `:201 SYSOPT`/`:202 SPECIALUI`/`:203 NUM_TYPES`。[观察]

---

## 2. `IO/UITypes/` 全部社交经济窗类（glob 174 文件；按钮坐标为窗口内 px）

> **未找到独立文件**：UIFriendGroup / UIPersonalShop / UITradingRoom / UIWhisperTargetDlg
> （后 3 者在 UIMiscShells.h 内）。

### UIParty（独立队伍窗，本包无 Party.img → 预留）
- `UIParty.h:51 TYPE=PARTY`/`:52 FOCUSED=false`/`:53 TOGGLED=false`；
  `:30-39 struct PartyMember{id/name/job/level/channel/mapid/online}`；
  `:41-46 struct Party{id/leader/members array<PartyMember,6>}`；
  `:75-82 enum Buttons{BtInvite,BtLeave,BtClose,BtLeader,BtExpel}`。[观察]
- `UIParty.cpp:36 UIParty::UIParty() : UIElement(Point<int16_t>(520,20), Point<int16_t>(200,300))`
  position(520,20)/dimension(200,300)；`:45 backgrnd sprite`；
  `:49-56 const char* btnname[]={"BtInvite","BtLeave","BtLeader","BtExpel"}` +
  btnpos (20,250)/(100,250)/(20,275)/(100,275)；`:58 try_maple_btn`；
  `:60 try_maple_btn(buttons, BtClose, close_btn(), Point<int16_t>(180,8), "UIParty close");`。[观察]
- `:63-96 draw`（`:68 title.change_text(Locale::format("party.title", …))`、`:80 info`、
  `:84 common.channel`、`:86 common.offline`、`:88 "★"`、`:95 draw_buttons`）；
  `:127-149 button_pressed`（BtInvite:131 prompt_invite / BtLeave:134 PartyLeavePacket+deactivate /
  BtClose:138 deactivate / BtLeader:141 prompt_leader / BtExpel:144 prompt_expel）；
  `:151-187 send_key`（ESC / L leader / E expel / I invite / C PartyCreatePacket /
  X PartyLeavePacket / J UISoftKey+PartyJoinPacket / S,P UIPartySearch / R UIPartyRace）；
  `:98-100 void UIParty::update(){}` 空实现。[观察]
- stub 逐字：`:39 // 本包 UI.nx 根下没有 Party.img，v83 也没有独立队伍窗根`
  `:40 // （UIWindow.img/UserInfo 是好友/信息窗…不能拿来当队伍窗底衬）`
  `:41 // ⇒ 底衬与 邀请/离开/队长/开除 四钮显式预留（队伍操作仍有 SoftKey L/E 与菜单入口）`
  `:42 // 关闭钮与其它功能窗共用同一颗描边 X（close_btn() → BtClose2）`
  `:47 // id 必须显式列出：枚举里 BtClose=2 夹在 BtLeave 与 BtLeader 之间`。[观察]

### UIBuddyList（好友窗，本包无 BuddyList.img → 仅关闭钮）
- `UIBuddyList.h:47 TYPE=BUDDYLIST`/`:48 FOCUSED=false`/`:49 TOGGLED=false`/
  `:73-77 enum Buttons{BtAdd,BtClose}`/`:79 ROW_H=24`；`:29-36 struct Buddy`/`:38-42 BuddyListData`。[观察]
- `UIBuddyList.cpp:35 : UIElement(Point<int16_t>(740,20), Point<int16_t>(200,320))`；
  `:49 buttons[BtClose] = std::make_unique<MapleButton>(close, Point<int16_t>(180,8));`；
  `:52-82 draw`（`:58 buddylist.title`、`:74 common.channel`、`:75 common.offline`）；
  `:88-96 doubleclick → :94 BuddyDeletePacket(selected_cid).dispatch();`；
  `:158-166 button_pressed`（BtAdd:162 / BtClose:165）；`:203 send_key`；
  `:84-86 void UIBuddyList::update(){}` 空实现。[观察]
- stub 逐字：`:42 // 本包 UI.nx 根下没有 BuddyList.img（nx_inspect 实测），也没有 Basic.img/BtAdd`
  `:43 // ⇒ 只有关闭钮能落地，且必须与其它功能窗共用同一颗描边 X`
  `:45 // 旧写法 bn ? MapleButton(bn) : MapleButton(bn,pos) 两个分支都拿不到贴图`
  `:46 // 真值分支还会把坐标丢成窗口原点，属于凭空写出来的逻辑`。[观察]

### UIGuild（家族=公会浮窗，保留底栏社区钮入口；主入口已迁 UIUserList 第 3 页签）
- `UIGuild.h:37 TYPE=GUILD`/`:38 FOCUSED=false`/`:39 TOGGLED=false`/
  `:65-72 enum Buttons{BtCreate,BtInvite,BtNotice,BtClose,BtAlliance}`/`:74 ROW_H=18`；
  `:31-32 // GuildMember / GuildData 已上移至 Character/GuildModel.h（家族页切片 2026-11-07）：
  共用窗 UIUserList 第 3 页签与本浮窗共读同一模型`；
  `:83-86 const GuildData& guild() const { return GuildModel::get().data(); }`。[观察]
- `UIGuild.cpp:39 : UIElement(Point<int16_t>(300,40), Point<int16_t>(300,380))`；
  `:55 buttons[BtClose] = ... Point<int16_t>(280,8)`；
  `:60-102 draw`（`:62 guild.title_plain/title_named`、`:95 guild.member_rank`、
  `:96 guild.member_online/offline`）；
  `:197-218 button_pressed`（BtCreate:201 / BtInvite:204 / BtNotice:207 /
  BtAlliance:210-211 `UI::get().emplace<UIAlliance>()` / BtClose:213）；
  `:220+ send_key`（D prompt_deny / L GuildLeavePacket self）；
  stub `:49 // 四颗钮无原生贴图，只能显式预留；关闭钮与其它功能窗共用同一颗描边 X`。[观察]

### UIAlliance
- `UIAlliance.h:38 TYPE=ALLIANCE`/`:39 FOCUSED=false`/`:40 TOGGLED=true`/
  `:64-71 enum Buttons{BtClose,BtRefresh,BtLeave,BtInvite,BtNotice}`；
  `:33-34` 数据来自 s2c ALLIANCE_OPERATION(66) getAllianceInfo(0x0C)/allianceNotice(0x1C)；
  `:58` 「set_ranks 0x1A 当前展示未用到，先存」。[观察]
- `UIAlliance.cpp:54 BtClose(dimension.x()-21,6)` / `:58 BtRefresh(20,dimension.y()-36)` /
  `:62 BtLeave(100,…)` / `:67 BtInvite(180,…)` / `:70 BtNotice(260,…)`；
  `:93/:94/:101/:123 alliance.name_line/notice_line/guilds_header`；
  `:150-182 button_pressed`；`:184 send_key`。[观察]

### UIFamily（学院=F 键窗，原生「学院.png」重排）
- `UIFamily.h:2 // Wave A · 学院（Family）信息窗 —— 2026-11-07 按原生「学院.png」重排`；
  `:4-6 // 术语钉死：本窗 = 中文客户端「学院」= s9 Family 系统（同学/名声度/名人冒险家特权）
  // 与「家族」= Guild（公会，UIUserList 第 3 页）是两个不同系统，勿再混`；
  `:9 UIWindow.img/Family/backgrnd 224x392`；`:11-12` 布局事实源 academy_top_3x.png / academy_priv_3x.png；
  `:36 class UIFamily : public UIDragElement<PosFAMILY>`；`:39 TYPE=FAMILY`/`:40 FOCUSED=false`/`:41 TOGGLED=true`；
  `:62 enum Buttons{BtClose,BtOK,BtTree,BtLeft,BtRight,BtJunior,BtPrecept,BtSpecial}`。[观察]
- `UIFamily.cpp:59 ctor`；`:86 if (root[name]) buttons[id] = std::make_unique<MapleButton>(root[name], Point(x,y));`；
  `:113/:116/:119/:121/:146/:184 family.none/junior_count/rep_value/today_value/priv_none/priv_used`；
  大量 family.softkey_* / family.ask_* `:202/:254/:266/:268/:283/:291/:323/:331/:341/:346/:372/:378`；
  `:226 draw_buttons`；`:229 send_key`；`:353 button_pressed`。[观察]

### UIFamilyTree（学院谱系树）
- `UIFamilyTree.h:2 // Family pedigree tree — FAMILY_CHART_RESULT (showPedigree)`；
  `:15-25 struct FamilyTreeEntry`；`:27 UIDragElement<PosFAMILYTREE>`；
  `:30 TYPE=FAMILYTREE`/`:31 FOCUSED=false`/`:32 TOGGLED=true`；
  `:49 enum Buttons{BtClose,BtBye,BtLeft,BtRight,BtJunior}`/`:51 ROW_H=18`/`:52 ROW_TOP=52`。[观察]
- `UIFamilyTree.cpp:23 ctor`；`:47 按钮 root[n](x,y)`；`:54/:69/:70/:71/:75/:76 familytree.*`；
  `:92 draw_buttons`;`:121 send_key`;`:143/:153/:162 family.ask_*`;`:210 button_pressed`。[观察]

### UIMessenger
- `UIMessenger.h:2 // Wave A · Messenger 聊天室（协议闭环：出站 122 / 入站 313）`；
  `:22 UIDragElement<PosMESSENGER>`；`:25 TYPE=MESSENGER`/`:26 FOCUSED=false`/`:27 TOGGLED=true`；
  `:39 try_create_room()`；`:54-58 enum Buttons{BtClose,BtEnter}`;`:60-69 struct Slot`；
  `:71-73 // 底板画了 4 枚名牌（NameBar/0..3…），模型却只留 3 格 ⇒ 服务端发到 pos=3 时
  set_member 直接 return，第 4 枚名牌永远是空的。按绘制面收敛成 4`；
  `:74-76 NAMEBAR_COUNT=4/SLOT_COUNT=4/CHAT_LINES=8`；
  `:85-134` 版式常量 `CHAT_LOG_TOP=231/CHAT_LOG_LINE_H=14/CHAT_LOG_VISIBLE=4/
  CHAT_INPUT_X=12,Y=318,W=228,H=15/CHAT_INPUT_TEXT_SHIFT_Y=-9/BT_ENTER_X=247,Y=322/
  BT_CLOSE_X=276,Y=6/LOOK_SCALE=0.70f/BALLOON_DX=-31,DY=-70/BLINK_TICKS=30/
  TYPING_THROTTLE_MS=3000`；
  `:106-108 // 关闭钮：v95 CUIMessenger::SetCtrl（0x7F34C0）… x=276,y=6…（v95 污染）`。[观察]
- `UIMessenger.cpp:24 ctor`；`:145 buttons[BtClose]` / `:153 buttons[BtEnter]`；
  `:416/:426/:436/:448/:449/:624/:631/:633/:644 messenger.*` i18n；
  `:516 draw_buttons`;`:599 send_key`;`:667-675 button_pressed`（BtClose:671/BtEnter:675）；
  占位 `:187 // 也先乐观占位 0 号座 + 本机立绘，避免回包前白窗`；
  `:414 // 回显行单独走 i18n：占位名 "Me" 是协议侧字段`。[观察]

### UITrade（实时面交）
- `UITrade.h:36 // 实时面交窗（Type::TRADE），由 TradeHandler 按 s9 PLAYER_INTERACTION 包驱动`；
  `:37-38 // 底图/按钮取自 UIWindow.img/TradingRoom；与 UIMiscShells 里的 UITradingRoom
  // （DIY 调试壳 Type::TRADINGROOM）不是同一个窗`；`:42 TYPE=TRADE`/`:43 FOCUSED=false`/`:44 TOGGLED=false`；
  `:70-77 enum Buttons{BtCoin,BtReset,BtTrade,BtCancel,BtChat}`，`:76 BtChat // 原生无此钮，仅由快捷键触发`。[观察]
- `UITrade.cpp:49 : UIElement(Point<int16_t>(260,120), Point<int16_t>(565,474))`；
  `:57 backgrnd`;`:67 try_maple_btn(buttons, d.id, trade[d.name], d.pos, "UITrade")`;
  `:74 trade.title`;`:131 draw_buttons`;`:245-268 button_pressed`
  （BtCoin:249/BtReset:253/BtTrade:260/BtCancel:263/BtChat:268）;`:302 send_key`；
  `:54 // WZ 里 MapleButton 节点不带坐标，必须显式传 Point（见 MapleButton.h）`。[观察]

### UIMerchant / UIStorage / UIShop / UIWedding / UIPartySearch / UIDelivery / UIStoreBank / UIMemo
- `UIMerchant.h:31-32` 数据来自 PLAYER_INTERACTION action ROOM=5 + updateHiredMerchant(0x19)；
  `:36 TYPE=MERCHANT`/`:37 FOCUSED=true`;`:43 void update() override {}` 空实现；
  `:63 MAX_SLOTS=16u`;`:65-69 enum Buttons{BtClose,BtBuy}`；
  `UIMerchant.cpp:37 ctor`;`:249/:253/:258 buttons[BtBuy + i]`（16 格）;`:267 merchant.owner_shop`;
  `:81-83 button_pressed`（只判 BtClose）；
  `:265-266 // 不拆成 owner + Locale::text(后缀)：中文「名字 的商店」/ 英文「Name's Shop」…整句进带占位符的键`。[观察]
- `UIStorage.h:42 TYPE=STORAGE`/`:43 FOCUSED=false`/`:44 TOGGLED=false`/
  `:65-71 enum Buttons{BtArrange,BtMesoIn,BtMesoOut,BtClose}`/`:73 COLS=8`/`:74 CELL=38`;
  `UIStorage.cpp:38 : UIElement(Point<int16_t>(200,20), Point<int16_t>(348,360))`;
  `:144-157 button_pressed`（BtArrange:148/BtMesoIn:151/BtMesoOut:154/BtClose:157）;
  stub `:44 // 目前既无参考截图也无 IDA 证据 ⇒ 底衬与 BtArrange/BtMesoIn/BtMesoOut 显式预留`。[观察]
- `UIShop.h:28 UIDragElement<PosSHOP>`/`:31 TYPE=SHOP`/`:32 FOCUSED=true`/`:33 TOGGLED=true`;
  `:65-96 enum Buttons{BUY_ITEM,SELL_ITEM,EXIT,CHECKBOX,OVERALL,EQUIP,USE,ETC,SETUP,CASH,BUY0..8,SELL0..8}`;
  `UIShop.cpp:59-62 4 张 backgrnd`;`:64/:65/:66 BtBuy/BtSell/BtExit`;`:78 CHECKBOX=AreaButton`;
  `:83 OVERALL=TwoSpriteButton`;`:180-240 button_pressed`。[观察]
- `UIWedding.h:2 // Wedding invitation / progress shell`/`:16 UIDragElement<PosNOTICE>`/
  `:19 TYPE=WEDDING`/`:20 FOCUSED=true`;`:31-32` 数据来自 WEDDING_GIFT_RESULT 0x09/0x0A/0x0B/0x0F;
  `:43 enum Buttons{BtOK}`;
  `UIWedding.cpp:99 buttons[BtOK]`;`:100/:125/:170/:184/:190/:249/:260/:266/:282 wedding.*`;
  `:183 // 无请柬/无订婚关系：保持可见反馈，不再假装 Whisper`;
  `:280 // 本端不做判定，只发 action=1（原生按钮即发 1），服务端必回 enableActions`。[观察]
- `UIPartySearch.h:2 // Wave A #2 · PartySearch（队长自动匹配；s9 START/UPDATE，无列表板回包）`/
  `:16 TYPE=PARTYSEARCH`/`:17 FOCUSED=false`/`:18 TOGGLED=true`/
  `:34-42 enum Buttons{BtClose,BtStart,BtStop,BtPause,BtCancel,BtReg}`;
  `UIPartySearch.cpp:228-245 button_pressed`;
  stub `:246 // PARTY_SEARCH_REGISTER(220) 在 s9 为空 stub → Start 代理（与 SoftKey S 一致）`。[观察]
- `UIDelivery.h:2 // Delivery / Duey — 收件列表 + 寄件/领取出站`/`:27 TYPE=DELIVERY`/
  `:48 enum Buttons{BtClose,TabSend,TabRecv,TabTax}`/`:49 enum Tab{SEND=0,RECV=1,TAX=2}`;
  `UIDelivery.cpp:54/:56/:58 TwoSpriteButton TabSend(10,24)/TabRecv(72,24)/TabTax(110,24)`;
  `:353-368 button_pressed`。[观察]
- `UIStoreBank.h:2 // Wave A #4 · StoreBank / Fredrick 寄存窗`/`:24 TYPE=STOREBANK`/
  `:42 enum Buttons{BtExit,BtGetAll}`;`UIStoreBank.cpp:55 BtExit(dimension.x()-21,5)`/
  `:57 BtGetAll(70,dimension.y()-40)`;`:184-191 button_pressed`。[观察]
- `UIMemo.h:2 // Wave A · Memo / 便签`/`:26 TYPE=MEMO`/`:44 enum Buttons{BtClose,BtOpen,BtSave,BtDelete}`;
  `UIMemo.cpp:41 BtClose(dimension.x()-21,5)`/`:43 BtOpen(20,by)`/`:44 BtSave(100,by)`/
  `:45 BtDelete(180,by)`;`:165-185 button_pressed`。[观察]

### UIUserList（★ 玩家列表 & 家族 共用窗 / 五页签）
- `UIUserList.h:27 UIDragElement<PosUSERLIST>`/`:30 TYPE=USERLIST`/`:31 FOCUSED=false`/`:32 TOGGLED=true`;
  `:34 UIUserList(uint16_t tab)`;`:40 // 家族页成员行不是按钮，须在光标事件里自行命中`;
  `:45-58 enum Tab`（见 §0）;`:55-56 // 顶层第 4 格「家族联盟」… 按钮 id 一律走 tab_button() 显式映射`;
  `:83-144 enum Buttons`（大枚举 `:85-88 BT_TAB_FRIEND/PARTY/GUILD/BLACKLIST`、
  `:96-99 BT_PARTY_CREATE/INVITE/LEAVE/SETTINGS`、`:102-106 BT_FRIEND_ADD/ADD_GROUP/EXPAND/MESSENGER/GROUP_0`、
  `:110-121 家族页 12 钮 BT_GUILD_INVITE/WITHDRAW/KICK/WHO/WHISPER/CHAT/RANKUP/RANKDOWN/NOTICE/BBS/PARTYINVITE/INFO`、
  `:123 BT_TAB_UNION`、`:127-142 运行时节点追加`、`:143 BT_PAGE_LAST`）;
  `:176-179 家族页第 3 页签注释`;`:180-186 guild_notice_bar(282x20 @(12,78))/guild_online_banner(281x27 @(12,111))/
  guild_offline_banner(@(12,189))/guild_col_header(281x18 @(12,139))`;
  `:188-189 // ---- 2026-11-08 五页签运行时资源（全部取自 UIWindow.img/UserList 真实节点）----
  // 落位为 5 张 oracle 帧的目视初值，后续用 layout.py 逐格校准（见 notes §7）` + `:190-232` Texture/Text/Slider;
  `:233 // 当前页签选中行（好友/组队/拒绝/联盟空态下仅占位，行数据待真机联调）` + `:234 page_selected_row = -1`。[观察]
- `UIUserList.cpp:58 : UIDragElement<PosUSERLIST>(Point<int16_t>(260,20)), tab(t)`;
  `:83 BT_CLOSE(288,4)`;`:103 顶层页签 TwoSpriteButton(tabdis[index],taben[index],TAB_POS[index])`;
  `:319 draw_sprites`/`:330 draw_guild_tab`/`:332 draw_union_tab`/`:336 draw_buttons`;
  `:447 draw_guild_tab`/`:1529 draw_union_tab`;
  `:606-1091 button_pressed`（`:618 BT_CLOSE`/`:621-624 四顶层页签→change_tab`/
  `:650 BT_FRIEND_MESSENGER`/`:701-739 队伍 create/invite/leave/settings`/`:775-793 黑名单`/
  `:810 BT_TAB_UNION`/`:813-926 家族页 12 钮含 :914 BT_GUILD_CHAT / :926 BT_GUILD_BBS`/
  `:941-1091 运行时节点追加钮含 :1038/:1039 PARTY_CHAT/UNION_CHAT / :1091 BT_GUILD_BOARD`）;
  `:519 send_key`;`:1101 change_tab`/`:1203 change_party_tab`/`:1229 change_friend_tab`/
  `:1247 change_blacklist_tab`;`:1441-1456 tab_button`;
  `:1416-1428` 五页签运行时文本初值（`friend_count_text "(0/25)"`/`party_online_count_text "0/0"`/
  `blacklist_count_text "0/26"`/`union_none_text`/`guild_online_count_text "[0/0]"` 等）;
  占位 `:194`/`:916`（家族聊天 change_target 预留空实现）/`：1131`（UNION 在枚举末尾走显式映射）/
  `:1282 // 只接受四态按钮容器，空壳节点跳过`。[观察]

### UIGuildBBS / UIGuildBoss / UIMapleTV / UIMiscShells 集合壳
- `UIGuildBBS.h:2 // Guild BBS 原生窗 — UI/GuildBBS.img（列表 / 读帖 / 发帖）`/
  `:3 // 证据：BeiDou-Client-xy REA wz_dump/GuildBBS.img_runtime.json`/
  `:4 // s9 GuildPackets.BBSThreadList / showThread；c2s BBS_OPERATION(155)`;
  `:51 TYPE=GUILDBBS`/`:52 FOCUSED=false`/`:53 TOGGLED=true`/`:78-91 enum Buttons 11 项`/
  `:93 enum class Page{List,View,Write}`/`:95-98 LIST_ROW_H=22/LIST_ROW_Y0=78/LIST_MAX_ROWS=12/BTN_Y_LIST=488`;
  `UIGuildBBS.cpp:74-98 11 个 try_maple_btn`、`:312 draw_buttons`/`:471-516 button_pressed`/`:525 send_key`;
  注册 `Net/PacketSwitch.cpp:346 GUILD_BBS_PACKET = 59` + `:696 emplace<GUILD_BBS_PACKET, GuildBbsHandler>()`。[观察]
- `UIGuildBoss.h:2 // 公会 Boss PQ HUD — s2c GUILD_BOSS_HEALER_MOVE(287) / PULLEY(288)`/
  `:3 // 地图物件真驱动见 MapGuildBoss（IDA sub_5587B3 / sub_558B0F）；本窗仅同步 HUD`;
  `:18 TYPE=GUILDBOSS`/`:19 FOCUSED=false`/`:20 TOGGLED=false`;`:24-28 draw/update/update_screen/send_key/get_type`;
  `:30-31 set_healer_y/set_pulley_state`;**全文件无 button_pressed、无 Buttons 枚举**;
  `UIGuildBoss.cpp:59/:64/:70 guildboss.title/healer_y/pulley_state`;
  注册 `Net/PacketSwitch.cpp:662-663`。[观察]
- `UIMapleTV.h:6 UIDragElement<PosNOTICE>`/`:8 TYPE=MAPLETV`/`:9 FOCUSED=true`/`:10 TOGGLED=false`;
  `:19 enum Buttons{BtOk,BtCancel}`;另 `:24-39 UIAdminClaim`/`:40-55 UIClassMatch`;
  `UIMapleTV.cpp:32 buttons[BtOk](60,by)`/`:33 buttons[BtCancel](140,by)`/
  `:76 tip_label.change_text(Locale::text("mapletv.no_512"))`;
  壳级 handler `Net/Handlers/WaveAShellHandlers.cpp:863 MapleTVUseResHandler`/
  `:899 EnableTVHandler`/`:914 SendTVHandler`/`:961 RemoveTVHandler` 主体均 `LOG + emplace<UIMapleTV>()`;
  注册 `Net/PacketSwitch.cpp:487-490`。[观察]
- `UIMiscShells.h:442-453 UIPersonalShop`(`:444 TYPE=PERSONALSHOP`) / `:454-464 UIMemberShop`(`:456`) /
  `:465-475 UITradingRoom`(`:467`) / `:476-487 UIEntrustedShop`(`:478`) /
  `:488-498 UICashTradingRoom`(`:490`) / `:499-509 UIItemSearch`(`:501`) —
  全族 `draw/send_key/get_type` + `bool has_assets=false; Texture backgrnd; Text tip_label;`,
  **无 button_pressed / 无 Buttons 枚举**；
  `UIMiscShells.cpp:2807-2811 UIPersonalShop::draw` 仅底图+一行 tip;
  `:2812 send_key`（ESC/X→MinigameExitPacket / C→SoftKey CREATE / O→HiredMerchantOpenPacket）;
  `:2958 UITradingRoom::draw` 同;`:2963 send_key`（C/T→TradeConfirmPacket;
  `:2976-2977 // s9 Action 枚举虽留 TRANSACTION(0x14)，但 handler 没有 0x14 分支：
  // 成交只能由双方各发一次 CONFIRM(0x11) 完成握手`)。[观察]
- `UIMiscShells.h:729-749 UIContextMenu`(`:731 TYPE=CONTEXTMENU`, `:743 enum Buttons{BtInfo,BtWhisper,BtClose}`,
  `:738 open_at(...)`) / `:761-778 UISlideMenu`(`:763`, `:773 enum Buttons{BtToggle,BtEvent,BtPartySearch}`) /
  `:963-1036 UIWhisperTargetDlg`(`:963 /// 悄悄话目标弹层（原生「请输入悄悄话的对象。」那一层）`,
  `:972 TYPE=WHISPERTARGET`, `:983 /// 打开即刷新候选名单（队友 + 好友在线），并把输入框聚焦`,
  `:990 enum Buttons{BtOk,BtNo}`, `:992-1007 版式 FRAME_W=266/TOP_H=21/MID_H=22/BOT_H=78/
  HINT_INK_TOP=30/HINT_X=25/FIELD_X=22/FIELD_Y=47/FIELD_W=238/FIELD_H=17/NAME_X=27/
  OK_X=162/NO_X=212/BTN_Y=91/ROW_H=15`, `:1018 refocus_field()`)。[观察]

---

## 3. 协议与数据层

### `Net/OutPacket.h` enum Opcode（:37-318）社交经济值
- `:92 GENERAL_CHAT=49`/`:93 MULTI_CHAT=119`/`:95 WHISPER=120`/`:96 SPOUSE_CHAT=121`。[观察]
- `:99 TALK_TO_NPC=58`/`:100 NPC_TALK_MORE=60`/`:101 NPC_SHOP_ACTION=61`/`:104 CHAR_INFO_REQUEST=97`。[观察]
- `:197 MESSENGER=122`（`:196 注释 入站 PacketSwitch 313`）/`：198 PARTY_OPERATION=124`/
  `:222 PARTY_SEARCH_START=222`/`:223 PARTY_SEARCH_UPDATE=223`/`:224-225 @deprecated 误名
  PLAYER_UPDATE=PARTY_SEARCH_UPDATE 别名`/`:226 DENY_PARTY_REQUEST=125`/`:228 DENY_GUILD_REQUEST=127`。[观察]
- `:231 STORAGE=62`/`:236 BUDDYLIST=130`（`:232-235 好友 handler 重映射 130，63 是漂移`）/
  `:238 FREDRICK_ACTION=64`/`:246 NOTE_ACTION=131`。[观察]
- `:247 OPEN_FAMILY_PEDIGREE=145`/`:248 OPEN_FAMILY=146`/`:249 ADD_FAMILY=147`/
  `:251 SEPARATE_FAMILY_BY_SENIOR=148`/`:253 SEPARATE_FAMILY_BY_JUNIOR=149`/
  `:254 ACCEPT_FAMILY=150`/`:255 USE_FAMILY=151`/`:256 CHANGE_FAMILY_MESSAGE=152`/
  `:258 FAMILY_SUMMON_RESPONSE=153`/`:259 DUEY_ACTION=65`/`:260 GUILD_OPERATION=126`。[观察]
- `:263 PLAYER_INTERACTION=123`/`:281 REMOTE_STORE=59`/`:285 ALLIANCE_OPERATION=143`/
  `:287 DENY_ALLIANCE_REQUEST=144`/`:289 BBS_OPERATION=155`/`:291 ENTER_MTS=156`。[观察]
- `:309 RING_ACTION=137`/`:312 WEDDING_ACTION=138`/`:317 WEDDING_TALK=139`;
  `:42-45 注释 RELOG 明确不做`。[观察]

### `Net/Packets/` 社交经济发包类（88 类）
- `PartyPackets.h:27/:36/:45/:55/:65/:75` Create/Leave/Join/Invite/Expel/ChangeLeader；
  `BuddyPackets.h:29/:40/:50` Add/Accept/Delete；
  `GuildPackets.h:27/:37/:47/:58/:69/:80/:92/:104` Create/Invite/Join/Leave/Expel/Notice/ChangeRank/Deny；
  `AlliancePackets.h:27/:37/:48/:62/:73` Action/Notice/Invite/Accept/Deny；
  `MessengerPackets.h:13/:24/:34/:45/:56`（见 §0）；
  `MessagingPackets.h:26/:38/:63/:73/:85` General/Multi/Spouse/Whisper/FindPlayer；
  `TradePackets.h:36/:46/:57/:66/:75/:85/:95/:116/:125` 9 类；
  `PlayerInteractionPackets.h:26/:39/:51/:68/:82/:94/:103/:112/:122/:135` 10 类；
  `StoragePackets.h:28`（单类）；
  `WeddingPackets.h:63/:76/:88/:104/:117/:133/:147/:165/:180/:193/:207` 11 类；
  `BbsPackets.h:13/:24/:35/:46/:59/:75/:93` 7 类；
  `DueyPackets.h:11/:20/:39/:49/:59` 5 类；
  `FredrickPackets.h:10/:19` 2 类；
  `NotePackets.h:13/:31/:44` 3 类；
  `PartySearchPackets.h:11/:25/:32` 3 类；
  `FamilyPackets.h:10/:16/:22/:31/:43/:54/:67/:80/:91` 9 类；
  `NpcInteractionPackets.h:174 NpcShopActionPacket`。[观察]

### `Net/Handlers/` + `Net/PacketSwitch.cpp` 注册
- 独立文件：PartyHandlers/BuddyHandlers/GuildHandlers/AllianceHandlers/MessengerHandlers/
  MessagingHandlers/TradeHandlers/StorageHandlers/HiredMerchantBoxHandlers/
  PlayerInteractionHandlers/DueyHandlers/FredrickHandlers（各 .h/.cpp）+
  NpcInteractionHandlers.cpp/PlayerHandlers.cpp/CommonHandlers.cpp。[观察]
- `WaveAShellHandlers.h:2 // Wave A 壳开窗 handler：Hammer/Vega/Gacha/Family/Memo`
  `:3 // Fredrick 已迁至 FredrickHandlers；Duey 在 DueyHandlers`；
  社交类声明 `:35 FamilyInfoHandler`/`:41 FamilyChartHandler`/`:47 FamilyJoinRequestHandler`/
  `:53 FamilyJoinResultHandler`/`:59 FamilyResultHandler`/`:65 MemoResultHandler`/
  `:71 WeddingProgressHandler`/`:77 WeddingCeremonyEndHandler`/`:83 WeddingGiftResultHandler`/
  `:120 GuildBossHealerMoveHandler`/`:126 GuildBossPulleyStateHandler`/`:162 MapleTVUseResHandler`/
  `:169 EnableTVHandler`/`:175 SendTVHandler`/`:181 RemoveTVHandler`/`:419 FamilyNotifyLoginHandler`/
  `:425 FamilySummonRequestHandler`/`:431 FamilyJoinAcceptedHandler`/`:473 TradeMoneyLimitHandler`/
  `:479 EntrustedShopCheckHandler`/`:485 ShopLinkResultHandler`/`:593 FamilyPrivilegeListHandler`/
  `:599 FamilyRepGainHandler`/`:605 FamilySetPrivilegeHandler`/`:719 MtsOperationHandler`/
  `:725 MtsOperation2Handler`/`:773 MarriageRequestHandler`/`:779 MarriageResultHandler`/
  `:785 NotifyMarriedPartnerMapTransferHandler`/`:791 WeddingPhotoHandler`/
  `:797 DestroyShopResultHandler`/`:827 PartyValueHandler`/`:851 GuildBbsHandler`/
  `:941 GuildNameChangedHandler`/`:947 GuildMarkChangedHandler`。[观察]
- `WaveAShellHandlers.cpp:265 FamilyInfoHandler::handle`/`:313 FamilyChartHandler`/
  `:395 FamilyJoinRequestHandler`（`:423 UIYesNo + :424 FamilyAcceptPacket(...).dispatch()`）/
  `:428 FamilyJoinResultHandler`/`:455 FamilyResultHandler`/`:490 WeddingProgressHandler`（`:497 emplace<UIWedding>()`）/
  `:507 WeddingCeremonyEndHandler`/`:515 WeddingGiftResultHandler`（mode 0x09/0x0A/0x0B/0x0F）/
  `:863 MapleTVUseResHandler`/`:1070 MemoResultHandler`；
  `:1244-1447 OpenUIHandler::handle` 大 switch 逐 id emplace
  （`:1277 UIGuild`/`:1284 UIFamily`/`:1305 UIGuild`/`:1312 UIParty`/`:1319 UIBuddyList`/
  `:1326 UIMessenger`/`:1334 UIStorage`/`:1341 UIDelivery`/`:1348 UIPersonalShop`/
  `:1362 UIPartySearch`/`:1383 UIFamilyTree`/`:1404 UIMemo`/`:1418 UIAlliance`/
  `:1425 UIWedding`/`:1439 UIStoreBank`）= **s2c OPEN_UI 开窗总路由，含多窗并存**。[观察]
- `PacketSwitch.cpp` emplace 注册（逐条）：`:443 STORAGE`/`:446 PARTY_OPERATION`/`:447 UPDATE_PARTYMEMBER_HP`/
  `:450 BUDDYLIST`/`:453 MESSENGER`/`:456 PARCEL`/`:457 FREDRICK`/`:458 FREDRICK_MESSAGE`/
  `:467-471 FAMILY_INFO_RESULT/FAMILY_CHART_RESULT/FAMILY_RESULT/FAMILY_JOIN_REQUEST/FAMILY_JOIN_REQUEST_RESULT`/
  `:472 MEMO_RESULT`/`:473-475 WEDDING_PROGRESS/WEDDING_CEREMONY_END/WEDDING_GIFT_RESULT`/
  `:487-490 MAPLE_TV_USE_RES/ENABLE_TV/SEND_TV/REMOVE_TV`/
  `:508 MULTICHAT`/`:509 WHISPER`/`:510 SPOUSE_CHAT`/`:539 PLAYER_INTERACTION`/
  `:542-544 SPAWN_HIRED_MERCHANT/DESTROY_HIRED_MERCHANT/UPDATE_HIRED_MERCHANT`/
  `:551 ALLIANCE_OPERATION`/`:554 GUILD_OPERATION`/`:625 TRADE_MONEY_LIMIT`/
  `:626 ENTRUSTED_SHOP_CHECK_RESULT`/`:627 SHOP_LINK_RESULT`/
  `:650-652 FAMILY_PRIVILEGE_LIST/FAMILY_REP_GAIN/FAMILY_SET_PRIVILEGE`/
  `:662-663 GUILD_BOSS_HEALER_MOVE/GUILD_BOSS_PULLEY_STATE_CHANGE`/
  `:674-675 MTS_OPERATION2/MTS_OPERATION`/`:683-686 MARRIAGE_REQUEST/MARRIAGE_RESULT/
  NOTIFY_MARRIED_PARTNER_MAP_TRANSFER/WEDDING_PHOTO`/`:690 PARTY_VALUE`/`:692 DESTROY_SHOP_RESULT`/
  `:696 GUILD_BBS_PACKET`。[观察]

### `Character/` Social 数据类（glob Character/**/*Social* = 无匹配）
- `FamilyModel.h:2 // FamilyModel — 学院（Family 系统）会话级数据层`/`:4-9` 存在理由
  （FAMILY_PRIVILEGE_LIST=100 与 FAMILY_INFO_RESULT=95 登录时就下发）/`:11-18` 字段序事实源
  （getFamilyInfo PacketCreator.java:5992 / loadFamily :5932）/`:28-43 struct FamilyInfoData{cur_rep/tot_rep/
  today_rep/juniors/juniors_max=2/leader_id/name/message/used}`/`:42 has_family()`/
  `:46-53 struct FamilyPrivilegeDef`/`:55-76 class FamilyModel`。[观察]
- `GuildModel.h:2 // GuildModel — 家族（Guild 系统）会话级数据层`/`:5-6 // 083 原版「家族」不是
  独立浮窗，而是「玩家列表 & 家族」共用窗（UIUserList）第 3 页签（好友/组员/家族/家族联盟/拒绝列表，
  原生截图为证）`/`:7-10 R28-B2 后 handler 不再为回包开窗… UIGuild 浮窗（状态栏钮入口）保留，同样改读模型`/
  `:12-13 数据事实源 s9 GuildPackets.showGuildInfo（mode 0x1A）`/`:23-32 struct GuildMember`/
  `:34-46 struct GuildData{id/name/rankTitles[5]/members/capacity/gp/allianceId/notice}`/`:48-67 class GuildModel`。[观察]
- `MarriageInfo.h:2 // 结婚/订婚数据层`/`:4-5 来源 s9 PacketCreator.addRingInfo（:6894-6913）
  与 MARRIAGE_RESULT mode 11`/`:16-18 为什么在登录解析里存下来`/`:27-70 struct MarriageInfo
  (enum Status{SINGLE,ENGAGED,RESERVED,MARRIED})`。[观察]
- **未找到** 独立 `Character/Party` / `Buddy` / `Alliance` / `Messenger` 数据类（数据内嵌
  `UIParty.h:30-46` / `UIBuddyList.h:29-42`）。[观察]

---

## 4. locale 社交经济文案 key（zh-CN.ini 635 行 / en-US.ini 632 行）

- `zh-CN.ini:100-103` 注释「原生 v83 中文包文案按『对所有人 / 悄悄话 / 组队 / 公会』四项；
  Q5 下拉补全为全部可发送频道；后四项是 NO-NATIVE 措辞（按斜杠命令 /b /a /s 与喇叭道具语义自定），
  沿用决策 B「先建键、等有 v83 依据再换」」；
  `:104 chat.target.all=对所有人`/`:105 悄悄话`/`:106 组队`/`:107 公会`（原生 4 项）/
  `:108 buddy=好友`/`:109 alliance=联盟`/`:110 spouse=夫妻`/`:111 megaphone=喇叭`（NO-NATIVE 4 项）/
  `:112 whisper.hint_target=请输入悄悄话的对象。`（原生截帧）。[观察]
- `:114-123 messenger.*`（self=我 / no_room / invited / declined / prompt_chat / prompt_decline / prompt_invite）。[观察]
- `:176 # NO-NATIVE 好友/黑名单窗（UIUserList）与查看全部角色（UIViewAllChar）`；
  `:179-187 userlist.*`（none=none / cur_location=My Location - {} / default_group=Default Group (0/0) /
  friend_group=Default Group / location=目前位置 / union_none=还未加入联盟。 / need_data=暂无可用数据 /
  use_alliance_window=请在联盟窗内完成该操作 / viewallchar.world=World {}）= NO-NATIVE + 英文占位。[观察]
- `:195-199 # NO-NATIVE R27-B1c-4 中文硬编码批次…en 侧先同值占位（决策 B）`；
  `:200-220 buddylist.title/common.channel/common.offline/party.title/storage.meso/trade.*` 中文硬编码逐字照抄。[观察]
- `:299-325 wedding.* 26 键全英文值`（NO-NATIVE 英文占位未译）。[观察]
- `:327-332 # NO-NATIVE R27-B1c-5b 壳窗批次…zh 侧先落英文、不编中文译文`；
  `:336-338 alliance.name_line/notice_line/guilds_header`；`:344 cashshop.stub_job=Illium`/
  `:345 cashshop.stub_name=ShomeiZekkou`/`:348 charinfo.medal_stub=Junior Adventurer`（stub 键）。[观察]
- `:359-369 delivery.*`/`:370-385 family.*`（中英混合）/`：386-392 familytree.*`/`:398 mapletv.no_512`/
  `:413-419 mts.*`/`:428-431 partysearch.*`/`:433-439 memo.*`/`:440-441 storebank.*` 英文占位。[观察]
- `:443 # R27-B1c-5c 中文盲区四窗…`/`:455-465 guild.*`/`:466-474 guildbbs.*`/
  `:475-477 guildboss.*`/`:478 merchant.owner_shop={} 的商店`。[观察]
- `:480-483 # NO-NATIVE R27-B1c-6 壳窗·UIMiscShells 批次（134 键 / 261 站点）`/
  `:484-493 common.open_store/shop.no_514_503_permit/shop.create/personalshop.open/miniroom.err_*`/
  `:500-632 common.sent/note/exit/buy/meso/chat_sent/confirm/transaction/visit/additem/shop.setitem_*`。[观察]
- `en-US.ini:95-99` NO-NATIVE 注释 /`:100-108 chat.target.* + whisper.hint_target`/
  `:113-119 messenger.*`/`:147-149 # R27-B1c: entries below have NO evidence-backed native copy`/
  `:173-184 userlist.*`/`:193-197 # NO-NATIVE R27-B1c-4 Chinese-hardcoded batch`/
  **`:198-214 buddylist.title=好友 ({})/common.channel=频道{}/party.title=队伍 #{}/storage.meso=金币: {}/
  trade.* 与 zh-CN 同值中文硬编码占位**（en-US 未英译）。[观察]

---

## 5. Configuration.h/.cpp 社交经济设置项与默认值
- `Configuration.h:156-159 get/set_rightclicksell(bool)`（`:318 bool rightclicksell = false;`）；
  `:160-163 get/set_show_weekly(bool)`（`:319 bool show_weekly = true;`）。[观察]
- `:639-644 ChatViewMax("chat.max", false)`/`:646-651 ChatViewX("chat.x", 0)`/`:653-658 ChatViewY("chat.y", 0)`。[观察]
- 社交窗位置 PointEntry 默认值：`:703-708 PosUSERLIST("ui.position.userlist","(104, 104)")`/
  `:710-715 PosCHAT("(0, 572)")`/`:729-734 PosSHOP("(146, 48)")`/`:743-748 PosMAPLECHAT("(50, 46)")`/
  `:889-894 PosCHARINFO("(264, 264)")`/`:896-900 PosMESSENGER("(500, 80)")`/
  `:902-906 PosPARTYSEARCH("(260, 60)")`/`:908-911 PosDELIVERY("(280, 80)")`/
  `:913-916 PosSTOREBANK("(300, 100)")`/`:938-941 PosFAMILY("(280, 60)")`/
  `:943-946 PosFAMILYTREE("(120, 80)")`/`:948-951 PosMEMO("(280, 100)")`/
  `:953-957 PosGUILDBBS("(40, 40)")`/`:959-962 PosPARTYRACE("(20, 80)")`。[观察]
- `:809-817 GameOptMask() : ShortEntry("ui.gameopt.mask", "4095")`（`:813 // 12 位同意掩码；
  消费方 UIGameOpt::accepts → 邀请/聊天入站拦截`）。[观察]
- `Configuration.cpp:113-114` 中文标签/`:262 settings.emplace<PosUSERLIST>()`/`:265 <PosSHOP>()`/
  `:267 <PosMAPLECHAT>()`/`:276 <GameOptMask>()`/`:287 <PosMESSENGER>()`/`:295 <PosFAMILY>()`/
  `:296 <PosFAMILYTREE>()`;`:1057-1074 get/set_rightclicksell/show_weekly` 实现。[观察]
- **未见** UIParty/UIBuddyList/UIGuild/UITrade/UIMerchant/UIWedding/UIAlliance/UIStorage 的
  `ui.position.*` 专用项（用硬编码 UIElement(Point,Point) 构造坐标，见 §2）。[观察]

---

## 6. 「预留/stub/TODO/未实现/暂不/显式/不做/假/placeholder/空实现/intentionally inert」逐字行
- `IO/UIStateGame.cpp:314 // = notifier 列表封包 —— 服务端不存在 ⇒ 按门禁 7 不做假窗`/
  `:316 LOG(LOG_DEBUG, "…intentionally inert");`。[观察]
- `UIParty.cpp:39-42/:47`、`UIBuddyList.cpp:42-46`、`UIGuild.cpp:49`、
  `UIStorage.cpp:44`、`UIPartySearch.cpp:246`（见 §2，均含「显式预留」）。[观察]
- `UIUserList.cpp:194`（占位符留在模板里）/`：916`（change_target 预留空实现）/`：1131`（显式映射）/
  `:1282 // 只接受四态按钮容器，空壳节点跳过`；`UIUserList.h:233 // …行数据待真机联调`。[观察]
- `UIMessenger.cpp:187 // 也先乐观占位 0 号座…`/`:414 // 占位名 "Me" 是协议侧字段`；
  `UIMerchant.cpp:266`（整句进带占位符的键）/`UITrade.cpp:54`（必须显式传 Point）。[观察]
- `UIWedding.cpp:183 // 保持可见反馈，不再假装 Whisper`/`:280 // 本端不做判定，只发 action=1`。[观察]
- `UIMiscShells.cpp:1592 // tip StringPool 3920/3921（英文明文占位）`/
  `:2243 // 跳过 id==-1 的「无处可去」占位`/`:4188 // UIOptionMenu 只剩空壳`/
  `:2976-2977 // …成交只能由双方各发一次 CONFIRM(0x11) 完成握手`。[观察]
- `UIChatBar.cpp:1616`（后四项 NO-NATIVE）/`:2463 // 与原生「没开过好友列表发 /b 无人收到」一致，
  不假装能发`/`:2512 default: break; // WHISPER 等由调用方给显式前缀`/`:2574 // 显式预留：
  v83 下拉里没有「喇叭」项`；`UIChatBar.h:31/:122 // TODO: Change these?`。[观察]
- `UICashShop.cpp:100/:102 cashshop.stub_job/stub_name`;`UICharInfo.cpp:171 charinfo.medal_stub`;
  `UIMonsterBook.cpp:43 monsterbook.stub_mob`。[观察]
- `UIDailyGift.h:32 // 本版：纯客户端假数据 + 视觉动画；领取/杀怪/账号日限/等级·MVP 仅预留接口`/
  `:33 // 道具槽预留…`/`:58 // 逻辑预留（阶段 3+ 接服务端；本版空实现 / 本地假态）`;
  `UIDailyGift.cpp:113/:357/:391`（相邻福利壳 NO-NATIVE 假态对照）。[观察]
- `IO/UIElement.h:196 // UIOptionMenu 只剩空壳，故另立两型`/`IO/UIState.h:59`（登录态默认空实现）/
  `IO/UIElement.cpp:94`（不必编假文案）/`IO/Components/MapTooltip.cpp:38 // TODO: Get list of where party members are`。[观察]

---

## 7. 三个明确结论

### ① 当前端社交经济是否一一对应原生 Angel 侧 56 单例类？——[推断] **否**
本端**未做** 56 类一一对应：大量能力合并进 `UIUserList` 五页签、`UIMiscShells.h` 集合壳、
单一 `WaveAShellHandlers.cpp`，协议/数据层铺得很全但独立窗类远少于 56。
- **A. 有独立窗类（~30，均带 .h/.cpp 或明确 TYPE）**：UIParty/UIBuddyList/UIGuild/UIAlliance/
  UIFamily/UIFamilyTree/UIMessenger/UITrade/UIMerchant/UIStorage/UIShop/UIWedding/UIPartySearch/
  UIDelivery/UIStoreBank/UIMemo/UIUserList/UIGuildBBS/UIGuildBoss/UIMapleTV/UIMTS/UIMinigame/
  UIMonsterCarnival/UIAriantMatch/UIPartyRace/UIRpsGame/UINewYearsCard/UIMegaphone/
  UIWhisperTargetDlg(UIMiscShells.h:972)/UIContextMenu(:731)/UISlideMenu(:763)。[观察]
- **B. 只有协议/数据层（无 v83 独立原生窗类，或仅 DIY 壳 / 并入共用窗）**[推断]：好友分组
  (UIFriendGroup)→仅 `UIUserList.h:103 BT_FRIEND_ADD_GROUP`/`:106 BT_FRIEND_GROUP_0` 按钮桩；
  家族联盟→拆成 `UIUserList.h:57 Tab::UNION` 页签 + `UIAlliance` 浮窗(`UIGuild.cpp:211`)双轨；
  拒绝列表→仅 `UIUserList.h:50 Tab::BLACKLIST` 页签；
  个人/会员商店/交易房/委托商店/现金交易房/道具搜索→仅 `UIMiscShells.h:442-509` 六个 DIY 壳；
  悄悄话→`UIWhisperTargetDlg`+`UIChatBar`；公会徽章/更名→仅 `WaveAShellHandlers.h:941/:947` 声明；
  公会 Boss PQ→仅 `UIGuildBoss.h` HUD。[观察]
- **C. 完全无**[推断]：v95 导师 Academy、远征 BossParty 独立面板（095 才有，用户已判「不做」，
  `UIStateGame.cpp:374-377` 把 BOSSPARTY 并入 GUILD 页签）；原生「拒绝列表独立窗」「好友分组
  独立窗」无单独 Type/文件；MapleTV 完整播放/送礼 UI 无。[观察]

### ② G 键家族入口 / F 键学院窗类名？——[观察] **是 / UIFamily**
- `UIStateGame.cpp:359 case GUILD → :374-377 case BOSSPARTY/GUILD: tab = UIUserList::Tab::GUILD; →
  :380-393 get_element<UIUserList>()/change_tab(tab)(:384)/if usable(window("UserList")) emplace<UIUserList>(tab)(:388-389)`。[观察]
- `UIUserList.h:45-49 GUILD 第 3 个值`；`UIUserList.cpp:1447-1448 case Tab::GUILD: return Buttons::BT_TAB_GUILD;`；
  `UIUserList.h:176-179`/`Character/GuildModel.h:5-6` 同述。[观察]
- `UIStateGame.cpp:296-309 case MEDALS(action 26 = FAMILY) → :302 OpenFamilyPacket().dispatch() +
  :303-307 emplace<UIFamily>()`；`UIFamily.h:36 class UIFamily : public UIDragElement<PosFAMILY>`/
  `:39 TYPE=FAMILY` → **F 键窗类名 = UIFamily**，与 GUILD(UIGuild) 双类树分离。[观察]

### ③ 哪些原生能力是「整族缺失」而非「壳/占位」？——[推断]
> 本端协议层(OutPacket opcode + Net/Packets + Net/Handlers + PacketSwitch emplace)几乎覆盖
> 全部 v83 社交面，故严格「四无」整族缺失极少；绝大多数是「有协议+数据层，但窗合并进共用窗 /
> 降级为 DIY 壳 / 显式预留」。
- **v95 导师 Academy、远征 BossParty 独立面板**：095 线专有，用户已判「不做」（属「不做」非「缺失」）。[推断]
- **好友分组独立管理窗（UIFriendGroup）整族缺失**：glob 未找到文件，`IO/UIElement.h` 无对应 Type，
  仅 `UIUserList.h:103/:106` 两颗按钮桩，无独立数据模型/opcode/handler/ini 文案组。[观察+推断]
- **公会徽章/标志编辑（Guild Mark/Emblem editor）整族缺失**：仅 `WaveAShellHandlers.h:941/:947`
  两条 handler 声明，PacketSwitch 未见对应 emplace，无窗类、无 ini 文案组。[观察+推断]
- **MapleTV 完整播放/送礼 UI 整族缺失**：`UIMapleTV.h:19` 仅 BtOk/BtCancel + `UIMapleTV.cpp:76
  "mapletv.no_512"` 一行 tip；4 个 handler(`WaveAShellHandlers.cpp:863/:899/:914/:961`)主体仅
  `LOG + emplace<UIMapleTV>()`。[观察+推断]
- **原生独立「拒绝列表窗」「家族联盟独立窗」整族缺失（被合并）**：并入 `UIUserList.h:50 BLACKLIST`/
  `:57 UNION` 页签（`UIUserList.cpp:332/:1529`）。[观察+推断]
- **属「壳/占位」而非缺失（勿误判）**：UIParty 底衬+4钮(`UIParty.cpp:41`)、UIBuddyList 仅关闭钮
  (`UIBuddyList.cpp:42-46`)、UIStorage 3钮(`UIStorage.cpp:44`)、UIGuild 4钮(`UIGuild.cpp:49`)、
  UIPartySearch REGISTER stub(`UIPartySearch.cpp:246`)、UIUserList 五页签行数据占位(`UIUserList.h:233`)、
  UIUserList 家族聊天 change_target 空实现(`UIUserList.cpp:916`)、UIMiscShells 六壳
  (`UIMiscShells.h:442-509`)、NOTIFIER intentionally inert(`UIStateGame.cpp:316`)。[观察]

---

## 8. 已实现功能面（22 条）
- `UIStateGame.cpp:356-393` G/FRIENDS/PARTY/BOSSPARTY 四键统一路由 UIUserList 五页签（含 G→第 3 页签）。[观察]
- `UIUserList.cpp:1441-1456` tab_button() 五页签→按钮 id 显式映射；`:447`/`:1529` draw_guild_tab/draw_union_tab；
  `:606-1091` button_pressed 覆盖 5 页签全按钮（含家族页 12 钮 :813-926）。[观察]
- `UIStateGame.cpp:296-309` + `FamilyPackets.h:10-13` F 键→UIFamily + OpenFamilyPacket(146) 闭环；
  `FamilyModel.h:28-76` 学院常驻数据层；`GuildModel.h:34-67` 家族常驻数据层共用窗/浮窗共读共写。[观察]
- `MessengerPackets.h:13-64` 出站 122 五 mode；`PartyPackets.h:27-75` 6 类；`GuildPackets.h:27-104` 8 类；
  `WeddingPackets.h:63-207` 11 类；`TradePackets.h:36-125` 9 类。[观察]
- `PacketSwitch.cpp:443-554` 社交主体 handler 注册；`:346/:696` GUILD_BBS_PACKET=59→GuildBbsHandler。[观察]
- `WaveAShellHandlers.cpp:265-475` Family 全套 handler；`:1244-1447` OPEN_UI 大 switch 逐窗 emplace。[观察]
- `UITrade.cpp:245-268` 面交 5 钮→发包；`UIShop.cpp:64-66/:180-240` NPC 商店买/卖/页签完整；
  `UIMessenger.cpp:416-644` 聊天/邀请/拒绝/软键盘 i18n；`UIGuildBBS.cpp:74-98/:471-516` 公告板 11 钮；
  `UIMiscShells.h:963-1036` UIWhisperTargetDlg 悄悄话目标弹层。[观察]
- `Configuration.h:703-962` + `zh-CN.ini:104-632` 社交窗位置持久化 + 社交文案 i18n 全覆盖。[观察]

## 9. 明显缺口面（25 条）
- `UIParty.cpp:39-41` 无 Party.img 底衬+4 钮预留；`:98-100 update(){}` 空实现；
  `UIBuddyList.cpp:42-46` 无 BuddyList.img 仅关闭钮；`:84-86 update(){}` 空实现；
  `UIGuild.cpp:49` 四钮无贴图；`UIStorage.cpp:44` 3 钮无证据预留。[观察]
- `UIPartySearch.cpp:246` REGISTER(220) 空 stub→Start 代理；`UIUserList.h:233` 五页签空态占位；
  `UIUserList.cpp:916` 家族聊天 change_target 预留空实现；`:1416-1428` 计数全 "(0/0)"/"0/26"/"[0/0]" 硬编码初值。[观察]
- `UIStateGame.cpp:314-316` NOTIFIER intentionally inert；`UIMessenger.cpp:136/:159/:168/:237/:278/:290/:410/:486/:537/:549`
  10 处 v95 注释污染 + `UIMessenger.h:106-108/:121-122/:126-127/:133` v95 IDA 立即数注。[观察]
- `UIMiscShells.h:442-509` 六窗 DIY 壳无按钮枚举；`UIMiscShells.cpp:2807-2811` UIPersonalShop::draw 仅底板+tip。[观察]
- `UITrade.h:76` BtChat 原生无此钮；`UIGuildBoss.h:15-45` 纯 HUD 无 button_pressed；
  `UIMapleTV.cpp:76` + `WaveAShellHandlers.cpp:863-972` MapleTV 仅 "No 512* map effect" + 4 handler 只 LOG/开窗。[观察]
- `zh-CN.ini:299-325` wedding.* 26 键全英文；`:327-332` 壳窗批次 zh 侧英文原值；
  `en-US.ini:198-214` 中文硬编码同值占位未英译。[观察]
- `Configuration.h`（无对应行）UIParty/UIBuddyList/UIGuild/UITrade/UIMerchant/UIWedding/UIAlliance/UIStorage
  无 ui.position.* 专用项（硬编码构造坐标）；glob 未找到 UIFriendGroup 独立文件；
  `Character/` 无独立 Party/Buddy/Alliance/Messenger 数据类；
  `UIWedding.cpp:280` 本端不做判定只发 action=1；`UIDailyGift.h:32-58` 相邻福利壳纯假数据。[观察]

---

**总判定 [推断]**：协议/数据/文案/配置四层对 v83 社交经济覆盖度很高；缺口集中在
「原生单例窗类的一一拆分」——大量能力合并进 UIUserList 五页签与 UIMiscShells 集合壳，
或退化为显式预留/DIY 壳/占位空态；G 键第 3 页签与 F 键学院窗（UIFamily）术语与路由正确。
唯一需按禁引 095 规则清理的是 UIMessenger 的 10 处（+头文件 4 处）v95 注释污染。
**全程只读，未改任何文件、未写任何文档。**
