# 社交经济面 · IMG 资源与样式证据（r03）

> 本文件 = 第三路只读 subagent `c7c28ccd` 返回全文的结构化归档 + 主 agent §0 复核块。
> 全程只读、未改文件、未连库。参照基准固定 083/v83；F=学院(FAMILY/UIFamily)，
> G=家族(GUILD/公会，原版入口 UIUserList 第 3 页签，五页签 好友/组员/家族/家族联盟/拒绝列表)。
> 性质：[观察]=源码/文件逐字；[推断]=据逐字得出。

---

## 0. 主 agent 复核块（★ 与 r01/r02 互证的硬增量）

1. **★ 083 顶层清单无 Guild/Alliance/Academy/BossParty 独立窗**
   （`docs/features/v83-hud-quest/evidence/UIWindow.img.top-level.txt` 112 节点，
   `:49 UserList` / `:90 Family` / `:91 FamilyTree` 有，Guild/Alliance 无）
   → 逐字坐实 r01 §3 + r02 §1/§3：**G 键公会是 UserList 子节点第 3 页签**，
   **F 键学院是独立 Family 顶窗**，**095 的 Academy/BossParty 面板 083 侧不存在**。
   与 MEMORY.md critical「只 095 才有的判不做」三条同时互证。[观察+推断]
2. **★ 5 页签运行时 IMG 真值表存在**：`docs/features/userlist-five-tabs/evidence/userlist_runtime_nodes.txt:1-802`
   是 `tools/nx_inspect.py list <UI.nx> 4 UIWindow.img/UserList` 完整 dump；
   `:797/:799/:801 backgrnd/2/3 均 312x389 origin(0,0)` 与 r02 §2 当前端
   `change_tab` 三张互斥底（312x389）逐值吻合 → **排版层当前端已用正确端截图
   + 运行时节点树对齐过，Angel 只补符号语义层**（四面一致）。[观察]
3. **★ 节点名失配是真缺口形态**：`docs/features/userlist-five-tabs/evidence/node_ref_audit.txt:3`
   `IMG distinct node names: 158 | code literal refs: 48 | missing in IMG: 25`（52% 失配）
   → 与登录面「按名查表」、HUD 面「资源根回退」同型，属**当前端沿用 Journey v83
   开源命名**而非缺功能。[观察+推断]
4. **★ 正确端截图库可直接坐实术语**：`C:\Users\11036\Desktop\正确客户端截图\`
   78 文件含 `学院.png 227x392`（F 键）+ `好友、组员、家族、家族联盟、拒绝列表.png
   310x386`（G 键五页签），与 r01 §3 的 5 页签文件名逐项对应 → r01 §7 术语表
   从「IDA 符号互证」升级为「IDA + 原生截图 + 运行时 IMG」三重证据。[观察+推断]
5. **IMG 层三面一致阻塞**：`mcp_connector_catalog(keyword=orange)`=0、
   `mcp_connector_status` 仅 `[json-photoshop] photoshop`（Host 未注册工具）、
   `C:\Users\11036\.cursor\mcp.json` 仅声明未接入、
   `login-flow-regression-fix/notes/img-evidence-audit.md:7` 已知解析缺陷
   → 替代链成立：`.nx` 字面量 → `nx_inspect.py` dump → 正确端截图落位。[观察]
6. **★ NO-NATIVE 自绘 PSD 单列**：`docs/features/trade-house-083/` 166 路径的
   Photoshop 产物 manifest 自证「样板：无（v083经验基线）/ 采样路径未实测 /
   目标节点待定 UITradeHouse」，且 083 顶层清单无 TradeHouse → 属自定扩展，
   非原生还原，列决策点（是否与 083 基准冲突）。[观察+推断]
7. **⚠️ locale 反向中文硬编码**：`locale/en-US.ini:199-218` `common.channel=频道{}`
   / `trade.*` / `party.title=队伍 #{}` / `storage.meso=金币: {}` 与 zh-CN 同值
   → 英文 locale 塞中文，语言分流缺失，与 HUD 面 `en:160 排行不可用` 同型
   跨面共性。[观察]

---

## 1. 083 原版顶层窗清单（权威）

`docs/features/v83-hud-quest/evidence/UIWindow.img.top-level.txt`（112 顶层节点）：
```
:8 Shop  :9 Messenger  :13 GameOpt  :14 SysOpt  :17 UserInfo  :19 TradingRoom
:20 PersonalShop  :21 Channel  :29 Trunk  :35 SoftKeyboard  :37 Memo
:49 UserList  :50 EntrustedShop  :54 StoreBank  :55 MemberShop  :59 Wedding
:60 Delivery  :61 MapleTV  :78 PartySearch  :90 Family  :91 FamilyTree
:102 CashTradingRoom  :108 PartyRace
```
**无** Guild / Alliance / Academy / BossParty / GuildBBS / TradeHouse 顶层窗
→ 家族(Guild)/家族联盟(Alliance) 是 UserList 子节点 [观察+推断]。

### 1.1 UiAssetResolve 回退链（唯一解析入口，`Util/UiAssetResolve.cpp`）
- `:42-55 window()`：UIWindow2.img→UIWindow.img，失败 `diag_once("neither UIWindow2.img nor UIWindow.img has it")`
- `:57-75 window4()`：UIWindow4→2→img 三级
- `:77-83 status_bar_root()`：StatusBar3→StatusBar
- `:105-113 shop_root()`：`UIWindow2.img/Shop2`→`UIWindow.img/Shop`
- `:115-131 close_btn()`：static order `{"BtClose","BtClose2","BtClose3"}` 取 Basic.img 首个 usable()；注释 BtClose 是 v83 原生蓝底白X，BtClose2/3 仅空壳回退
- `:133-145 usable()`：size>0 或 bitmap 才算可用（v83 BeiDou 有 empty shells）
- `:25-40 diag_once()` → `_ui_asset_diag.log`

### 1.2 window()/window4()/shop_root() 调用点（逐字）
```
IO/UITypes/UIUserList.cpp:62 window("UserList") / :221 window("Messenger")["BtEnter"]
  / :633/:639/:1035 usable(window("PartySearch"))
UIFamily.cpp:75 window("Family")（F键学院） / UIFamilyTree.cpp:31 window("FamilyTree")
UIMessenger.cpp:71 window("Messenger") + :75 LOG "UIWindow.img/Messenger missing"
UITrade.cpp:55 window("TradingRoom") + :38 注释 backgrnd 565x474 分区
UIPartySearch.cpp:35 window("PartySearch") / UIStoreBank.cpp:31 window("StoreBank")
UIWedding.cpp:83/:111 window("Wedding") [:89-90 Invitation→Cathedral否则Vegas, :98 BtOK]
UIMapleTV.cpp:20 window("MapleTV")[:23 backgrnd否则backgrd,:32 BtOk]
  + :88 window("AdminClaim") + :148 window("classMatch")
UIMemo.cpp:29 window("Memo") / UIShop.cpp:45 shop_root()
UIChat.cpp:60 window("socialChatEnter") / :283 window("socialRank")
UIOptionMenu.cpp:55-56 window("GameOpt")/window("SysOpt")
UIUserList.cpp:269 UserList/Guild/GuildInfo / :632 PartySearch 独立壳
```

### 1.3 `nl::nx::UI[...]` 直连字面量
```
UIAlliance.cpp:44 ["UIWindow.img"]["UserList"]["GuildUnion"]["backgrnd"](233x304)
  / :56/:60/:65/:68 Basic.img/{BtOK,BtCancel}
UISoftKey.cpp:72 ["UIWindow.img"]["SoftKeyboard"]
UIMessenger.cpp:138 ["Basic.img"]["BtClose3"] / :140 回退 close_btn
  / :136 注释 v95 @(276,6)  ← r02 §4.2 污染面之一
UIShop.cpp:733 ["Basic.img"]["ItemNo"]
UIMerchant.cpp:41 ["CashShop.img"]["CSList"]["Base"] / :51 BtBuy
  / :48 注释 CSTab 只有 Tab 无 BtExit / :50 close_btn
UICashShop.cpp:56 ["CashShop.img"]
UIStorage.cpp:46 ["Storage.img"] + :42-44 逐字「本包 UI.nx 根下没有 Storage.img
  （v83 真身 UIWindow.img/Trunk 463x318）… 既无参考截图也无 IDA 证据
  ⇒ 底衬与 BtArrange/BtMesoIn/BtMesoOut 显式预留」+ :50-56 btnpos
  (20,320)/(120,320)/(220,320)/(300,20)
UIGuildBBS.cpp:39 ["GuildBBS.img"]["GuildBBS"] + :38 注释 Data/UI/GuildBBS.img(runtime.json 证实)
UIParty.cpp:43 ["Party.img"] + :39 注释包内无 Party.img
UIBuddyList.cpp:38 ["BuddyList.img"] + :42 注释 nx_inspect 实测无
UIGuild.cpp:44 ["GuildUI.img"] + :48 注释 nx_inspect 实测无
UIOptionMenu.cpp:51 ["StatusBar3.img"]["OptionMenu"]
UIMiscShells.cpp:5805 注释 UIWindow.img/GameOpt 与 /SysOpt
```
**节点名失配**（[观察]）：`UIStorage.cpp:46 Storage.img` × 083顶层 `:29 Trunk`、
`UIMerchant.cpp:41 CashShop.img/CSList` × 083顶层 `:20 PersonalShop`。

### 1.4 头文件/注释内节点路径与尺寸
`UIFamily.h:9` Family/backgrnd 224x392；`UITrade.h:37/:72` TradingRoom/BtCoin；
`UIMessenger.h:158` Messenger/blink 12x10；
`UIUserList.h:124/:188/:176-183/:190-232` 五页签与 guildinfo 尺寸
（guildinfo0/1 281x27 在线/离线横条、guildinfo6 282x20 公告白条 @(12,78)、
guildinfo7 281x18 列头 @(12,139)、guildmark 20x20、friend0 281x27、
friend4 281x27、friend1 281x19、friend3 281x6、friend6 306x1、
party0/4 281x27、party1 279x18、party5 279x18、party2 279x4、
blacklist0 281x27、blacklist1 281x18、blacklist2 281x6）。

### 1.5 raw-nx-dump（IMG MCP 替代链核心）
`docs/features/userlist-five-tabs/evidence/userlist_runtime_nodes.txt:1-802`
= `tools/nx_inspect.py list <UI.nx> 4 UIWindow.img/UserList` 完整 802 行
（节点名/类型/尺寸/origin）：
```
:3 BlackList  :28 Friend  :240 Guild  :527 GuildUnion  :653 Party  :774 Tab
:797/:799/:801 backgrnd/2/3 均 312x389 origin(0,0)
:774-796 Tab/enabled|disabled/0..4 五格位图
:201-213 friend0..6   :401-419 guildinfo0..7+guildmark 20x20
:762-773 party0..3    :22-27 blacklist0..2
:525 Guild/backgrnd 233x304  :636 GuildUnion/backgrnd 233x304
:286 GuildBoard/backgrnd 312x389
:4-21/29-183/331-400/528-635/654-734 各按钮 normal/mouseOver/pressed/disabled/0 四态
```
`docs/features/userlist-five-tabs/evidence/node_ref_audit.txt`：
- `:1-2` 口径 `runtime IMG E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx -> UIWindow.img/UserList (depth 4)` × UIUserList.cpp
- `:3` `IMG distinct node names: 158 | code literal refs: 48 | missing in IMG: 25`
- `:5-30` 25 失配字面节点：`Boss,BtEnter,BtMessenger,BtPartyLevel,BtPartyMake,BtPartyOut,BtPartySettings,BtPlusFriend,BtSheetIClose,CbCondition,Main,PartySearch,Sheet1,Sheet2,Sheet6,TapShowAll,TapShowGuild,TapShowIndividual,TapShowOnline,base,partyName,request,table,text,title`
- `:35-51` 各页直接子节点 Friend38/Party18/BlackList5/GuildInfo23/GuildUnion20/Tab2
- `docs/features/userlist-five-tabs/notes/diagnosis-and-plan.md:14-26` 代码沿用开源 Journey v83 命名、48 引用 25 失配(52%)、`:9` 主因 IMG 节点未整套按运行时校验（非 REA 失败）、`:38-39` ground truth 只能是运行时 IMG 节点树+原生截图落位校准

---

## 2. 通用组件常量（`IO/Components/` 47 文件）

- `Button.h:28-36` enum State{NORMAL,DISABLED,MOUSEOVER,PRESSED,IDENTITY,NUM_STATES}（注释称 4 态但数组按 NUM_STATES 分配）
- `MapleButton.h:28-29` "button with 4 states"；`:44-45` `Texture textures[NUM_STATES]; Animation animations[NUM_STATES];`；`:48-62` maple_btn_nopos 哨兵 + try_maple_btn
- `MapleButton.cpp:25-28` nopos=(-32768,-32768)；`:30-53` try_maple_btn 先 usable() 再由 `src["normal"]["0"]` 取 origin 加到 position；`:55-70` 逐字四态键名 `normal/mouseOver/pressed/disabled`
- `TwoSpriteButton.h:26/:29-32/:42-44` class + 四构造 + `BoolPair<Texture> textures; Point<int16_t> npos,spos;`（UIUserList 五页签绑 enabled/disabled 两图）
- `Textfield.h:32-37` enum{NORMAL,DISABLED,FOCUSED}；`:40` 构造含 `Rectangle<int16_t> bounds, size_t limit, int16_t marker_height=12`；`:53-58` `set_accept_unicode(bool)` 默认 false
- `Charset.h:29-34` Alignment{LEFT,CENTER,RIGHT}；`:45-47` `unordered_map<int8_t,Texture> chars`
- `MapleFrame.h:24-30/:32-43` 九切片 center/east/northeast/north/northwest/west/southwest/south/southeast + xtile/ytile；`MapleFrame.cpp:26-40` 字面量 c/e/ne/n/nw/w/sw/s/se、`:38-39` xtile=max(north.width(),1)/ytile=max(west.height(),1)、`:42-72` numhor=rwidth/xtile+2
- `Gauge.h:27-34` enum{DEFAULT,CASHSHOP,WORLDSELECT,CLIP}；`:36` 默认 maximum=1；`Gauge.cpp:31/:35-40` length=percentage*maximum
- `Icon.h:91-106` `static constexpr int16_t cell_width=32; cell_height=32;`
- `ScrollingNotice.cpp:22-30` width=800 / ColorBox(width,23,BLACK,0.535f) / A12M/YELLOW；`:51-75` xpos-=0.5、`:68-74` 公告滚出连同 23px 黑底一起消失
- `NameTag.h:29-37` NameTag.img/<id> 三切片{w,e,c,clr}；`:49-50` MIN_ID=3/MAX_ID=254；`:105-115` load_v83/load_triple
- `Tooltip.h:30-42` enum Parent{...,SHOP,...}（社交经济窗无专属 Parent）
- `Slider.h:29` 构造、`:44-75` enum Type 含 DEFAULT_SILVER、`:96-102` dbase/dnext/dprev/thumb（UIUserList 五页用 DEFAULT_SILVER，`UIUserList.cpp:1430-1438` Range(105,285),x=292,6,6）

---

## 3. 截图 / layout JSON / Photoshop 出图

### 3.1 外部正确截图库（★ 术语三重证据）
`C:\Users\11036\Desktop\正确客户端截图\` 78 文件（纯 PNG + 1 个 mp4「聊天功能展示」，无 layout JSON/manifest.md；`read_image` 对 hy4free 报 `does not declare image input` ⇒ 只据仓库内逐像素实测/注释，不臆造）：
```
学院.png 227x392 197538B（F键学院，标题/名声度/今日/特权3x3行带，据 UIFamily.h:9-22）
好友、组员、家族、家族联盟、拒绝列表.png 310x386 251487B（UIUserList 五页签共用窗）
好友列表框.png 302x384 / 组员列表框.png 313x386 / 家族联盟.png 309x387
  / 拒绝列表框.png 310x391
悄悄话框.png 261x117 / 商城.png 1280x720 / 进入游戏页.png 1280x720
  / 打开了S-E-I-K页面.png 1280x720
聊天框展开时候.png 569x103 / 聊天框收起的时候.png 566x40
  / 聊天选择对话的类型.png 191x169 / 游戏设置.png 156x283
  / 系统设置.png 298x365 / 键盘设置.png 629x369
  / 底部信息栏和功能按钮.png 816x142 / 角色头上提示信息.png 150x148
  / 个人请报框.png 511x197 / 勋章框.png 255x372
```

### 3.2 仓库内归档
- `docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/` 7 PNG（登录界面/选择频道页/选中频道页/选择角色页/进入游戏页/打开了S-E-I-K页面 均 1280x720、键盘设置 629x369）；与外部库同名图字节数全等（打开了S-E-I-K页面 1620971B）⇒ 同批入库 [观察+推断]
- `docs/features/social-loop-audit-2026-11/evidence/`：academy_top_3x 681x450 / academy_priv_3x 681x570 / academy_bottom_3x 681x186 / family_backgrnd 224x392；`guildpage/` 下 guild_backgrnd 233x304、native_top_3x 930x330、native_bottom_3x 930x270、banners_sheet 292x164、btn_sheet 73x219、guildinfo0~7（281x27/281x27/281x18/281x18/280x1/2x1/282x20/281x18）、guildmark 20x20、fam_BtTree 55x18 + 12 个 Bt*.png；有 notes/client-path.md、notes/guild-tab-implementation.md、README.md，无 JSON
- `docs/features/userlist-five-tabs/evidence/frames/` native_friend_tab 302x384 / native_party_tab 313x386 / native_guild_tab 310x386 / native_union_tab 309x387 / native_blacklist_tab 310x391（字节数与外部库同名图一致）；无 layout JSON/manifest.md
- `docs/features/social-loop-audit-2026-11/README.md:7-10` 一律参照 GMS 083 原版 095 不参照；`:20-22` F=v83 action26 FAMILY(中文「学院」)即 UIFamily，韩系 Academy 非 083 不做；`:100` G 键 KeyAction::Id::GUILD 改开 UIUserList(Tab::GUILD)、UIGuild 浮窗保留底栏入口；`:97` 家族页资源取运行时 UserList/Guild/GuildInfo 全节点；`:113-115` 第 4 页签家族联盟按 GuildUnion 还原；`:116-119` 在线 2 行/离线 5 行无滚动条、真机冒烟未跑
- `docs/features/social-loop-audit-2026-11/notes/client-path.md:15-18` 083 参照端 `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe` / IDB `E:\MXD\BeiDou_idb\` / 部署 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
- `docs/features/userlist-five-tabs/notes/diagnosis-and-plan.md:78-80` 三张互斥底 meanSAD 好友=bg3(11.6)/联盟=bg2(7.6)/组员·拒绝=bg1；`:103-104` 家族=bg2(25.06)；`:141-178` §8 绑五页 158 节点+44 按钮+5 Slider GATE OK；`:162-178` 预留（目视初值未用 layout.py / oracle 未跑抓屏全黑 / 列表仅空态 / 按钮语义按节点名推断无原生证据 / 联盟按钮不臆造 action / BT_GUILD_CHANGE 与 PartyHP 未接线）

### 3.3 layout JSON（6 个，均非社交经济）
`docs/features/char-stats-ui-ps/ui/detail/v4/layout_cn.json`、`map-search-ui/resources/layout.json`、
`map-search-ui/ui/search_panel_backgrnd/v3_pipeline/layout.json`、`map-info-panel/resources/layout.json`、
`UI/MapInfo/layout.json`；`social-loop-audit-2026-11/**/*.json` = No files found。

### 3.4 ★ NO-NATIVE 自绘 Photoshop 产物（单列决策点）
`docs/features/trade-house-083/` 166 路径，`ui/{chrome,tabs,buttons,list,grid,inputs,feedback,overlay}/v1/` +
`ui/assembly/v1|v2/` 各带 manifest.md，DESIGN.md/PLAN-ui.md：
- `ui/chrome/v1/manifest.md:3-24` 样式来源原生 scale1x、BtClose 四态 normal/mouseOver/pressed/disabled、九宫角 16x16 边可平铺、titleBar 22px 青蓝、panel_bg 米黄；`:9-10`「样板：无（v083经验基线）」「采样路径/客户端：未实测」；`:11`「目标节点路径：待定(UIWindow.img/.../TradeHouse/)」；`:28-41` nw/ne/sw/se 16x16、n/s 32x16、w/e 16x32、c 32x32、titleBar 96x22、BtClose_* 13x13、assembly_preview 640x420、chrome.psd 860x460
- `ui/assembly/v1/manifest.md:5-14` 主窗 640x420 / BidDialog 280x160 6 场景；`:27-32` 文案英文占位无客户端路径实测；`v2/manifest.md:3-11/:45` 目标节点待定 UITradeHouse、orange-wz 入库另开任务
→ **判定 [推断]：v083 经验基线 + 中文占位 + 目标节点待定的自绘 PSD，非原生 IMG 采样，
NO-NATIVE 自定；083 顶层清单无 TradeHouse 窗**。

---

## 4. locale 社交文案 key + NO-NATIVE 台账

### 4.1 zh-CN.ini 已中文化
`locale/zh-CN.ini:104-111 chat.target.*`（对所有人/悄悄话/组队/公会/好友/联盟/夫妻/喇叭）、
`:112 whisper.hint_target`、`:117-123 messenger.*`、`:179-186 userlist.*`、
`:200 buddylist.title`、`:201-204 common.channel=频道{}/offline=离线/party.title=队伍 #{}/storage.meso=金币: {}`、
`:205-220 trade.*` 全 20 条中文、`:370-385 family.*`（F 键学院）中文、
`:455-465 guild.*`（家族/公会）中文、`:466-474 guildbbs.*` 中文、`:475-477 guildboss.*`、
`:478 merchant.owner_shop={} 的商店`。

### 4.2 stub / 英文占位 / NO-NATIVE 台账
- `:168` 逐字「这三块面板是本包自造的 UI，原生 v83 没有对应窗体，文案不存在依据；mappanel.mesos」= **唯一显式 NO-NATIVE 声明**；`:173 mappanel.mesos=mesos`
- `:179-182` userlist 4 条在 zh-CN 里仍是英文 stub（正确端要求中文「目前位置 - ...」见 diagnosis:73）→ 反向英文占位
- `:299-325 wedding.*` 27 条全英文（README 记值暂英文遵 R27-B1c 决策B）
- `:336-338 alliance.*` 英文；`:344-347 cashshop.stub_job=Illium/stub_name=ShomeiZekkou/meta/meta_stor` stub
- `:359/:368 delivery` 英文；`:386-392 familytree.*` 英文占位（Family Tree/Lv.{}/[ON]/[OFF]/rep {}/Pedigree/can add junior）
- `:396 maker.cost_meso={} meso`、`:398 mapletv.no_512`
- `:424-431 partyrace/partysearch` 全英文
- `:433-440 memo.*/storebank.mesos=Mesos: {}/getall_sent=GetAll sent` 英文
- `:484-487 common.open_store=Open store…/shop.no_514_503_permit/shop.create/personalshop.open=PlayerShop open`、`:489-492 miniroom.err_*`
- `:542 counsel.whisper=Whisper → {}`、`:592-599 contextmenu.whisper=Whisper/party=Party/trade=Trade/buddy=Buddy/messenger=Messenger/guild=Guild/family=Family`（右键菜单未汉化）
- `:550/:553-554/:562-563` SoftKey 自定提示串（personalshop.shop_softkey_c_title_o_a_b_x / membershop.membershop_softkey_title / tradingroom.trade_softkey_meso_chat_c_x / entrustedshop.entrusted_softkey_o_open / cashtradingroom.cashtrade_softkey_meso_chat_c_x）= NO-NATIVE 自定
- `:631-632 shop.setitem*` 英文

### 4.3 ⚠️ en-US.ini 反向中文硬编码（跨面共性）
`:199-218` `common.channel=频道{}` / `common.offline=离线` / `party.title=队伍 #{}` /
`storage.meso=金币: {}` / `trade.*(:203-218)` **全部中文且与 zh-CN 同值** ⇒ 英文 locale 内塞中文；
`:100-107 chat.target.*` 英文、`:113-119 messenger.*`、`:176-183 userlist.*`、
`:298-324 wedding.*` 全英文同 zh-CN、`:336-338 alliance.*`、`:368-383 family.*` 英文、
`:452-462 guild.*` 英文、`:628-629 shop.setitem*`。
→ **判定 [推断]：两份 locale 在 trade.*/party.title/storage.meso/common.channel/common.offline
段落键值完全一致（均中文）⇒ 中文硬编码未做语言分流**。

---

## 5. Configuration 社交经济设置项

`Configuration.h`：
```
:643 chat.max=false  :650 chat.x=0  :657 chat.y=0
:707 ui.position.userlist=(104,104)（玩家列表&家族共用窗）
:714 chat=(0,572)  :733 shop=(146,48)
:940 ui.position.family=(280,60)（F键学院，与 UIFamily.cpp:70 默认一致）
:899 messenger=(500,80)  :905 partysearch=(260,60)  :910 delivery=(280,80)
:915 storebank=(300,100)  :945 familytree=(120,80)  :950 memo=(280,100)
:956 PosGUILDBBS ui.position.guildbbs=(40,40)  :961 partyrace=(20,80)
:799 gameopt=(170,24)  :806 sysopt=(330,24)  :791 optionmenu=(170,-1)
:740 notice=(400,285)  :747 maplechat=(50,46)  :754 channel=(215,100)
:893 charinfo=(264,264)  :816 ui.gameopt.mask=4095
:824-893 ui.sysopt.quality=98/mouse_speed=52/hp_warn=52/mp_warn=52/
  skill_effects=true/window_mode=true/alt_enter=false/mob_info=3/screenshot_path=0
:664-782 stats=(72,72)/eqinv=(250,160)/inv=(24,24)/skill=(96,96)/quest=(300,160)/
  map=(313,64)/minimap=(5,24)/joypad=(312,134)/event=(99,100)/dailygift=(99,80)/
  keyconfig=(144,144)
:920-935 vicioushammer/vegaspell/cashgachapon/remotegachapon
:787-788 注释历史 bug「此前误用 "PosUSERLIST" 作为键名，与用户列表位置撞名」
:796 gameopt 默认值不是 oracle 读数
```
`Configuration.cpp`：
```
:112-114 聊天栏 chat.max/x/y  :122-130 ui.sysopt.*  :133 ui.gameopt.mask
:146-178 界面位置中文窗名 userlist=用户列表/chat=聊天栏/shop=NPC 商店/notice=公告面板/
  maplechat=枫叶聊天·排行榜/messenger=信使聊天室(:164)/partysearch=组队搜索(:165)/
  storebank=寄存StoreBank(:167)/family=家族(:172)/familytree=家族树(:173)/
  memo=便签Memo(:174)/gameopt=游戏设置窗·sysopt=系统设置窗(:175-176)/partyrace=组队竞赛(:178)
:256-299 settings.emplace<Pos*>() 全注册（:262 PosUSERLIST / :265 PosSHOP /
  :287 PosMESSENGER / :295 PosFAMILY / :298 PosGUILDBBS）
:547-551 历史 bug「早期版本反着 prepend("a.b.c"→"c.a.b")…31 个 ui.position.* 窗口坐标
  + audio.*.volume + font.path.* + ui.autopot.* 等 39 项在 Settings 里改了也不生效，
  每次启动都退回代码默认值」
```
→ **判定 [推断]：`ui.position.guildbbs` 在 `Configuration.cpp:146-178` 描述表未列但 `:298`
有 emplace；gameopt/sysopt 在 h 与 cpp 均有 ⇒ 描述表与注册项不完全对齐**。

---

## 6. orange-wz(IMG) MCP 门禁

- `mcp_connector_catalog(keyword=orange)` = **0 个**已上架连接器
- `mcp_connector_status` 仅 1 条 `[json-photoshop] photoshop · json/none/stdio · 状态未知 · 全局 → stdio · npx`，诊断 `Host 尚未注册该 stdio Server 的工具`；**无 orange-wz、无 ida-pro-mcp**
- `C:\Users\11036\.cursor\mcp.json` 存在（511B, 2026-09-27）内容 `{"mcpServers":{"orange-wz":{"type":"http","url":"http://127.0.0.1:10012/mcp"},"ida-pro-mcp":{"type":"http","url":"http://127.0.0.1:13337/mcp"},"photoshop":{...}}}` ⇒ 文件里仅**声明**，DSH Host 目录/状态里无这两条 ⇒ **orange-wz(IMG) MCP 未接入 Host**（只读检查，未连库、未改配置）
- `docs/features/login-flow-regression-fix/notes/img-evidence-audit.md:7` 逐字 `orange-wz MCP 对这批私服改版 img 内部节点有已知解析缺陷，未使用。`；同文件 `:4-6` 真值口径 = 运行时部署 `E:\MXD\BeiDou-Client-xy\Data\UI\*.img` + `REA反编译\wz_dump\*_runtime.json`，打包 Login_img.json 不作准
- `docs/features/userlist-five-tabs/notes/diagnosis-and-plan.md:34-36` rea provider=ida 的 list_documents/binary_overview/search_strings 均 `unreadable_output`、`current_document=unsupported_provider`，exe 静态取证 `UIWindow.img/UserList` 仅 1 处（file off `0x73C2DF`）无立即数引用按名查表 ⇒ REA/IDA 侧亦无逐节点常量，坐实「以运行时 IMG 节点树+截图为准」

→ **IMG 层记阻塞（四面一致）**；替代链 = `.nx` 字面量 → `nx_inspect.py` dump → `node_ref_audit.txt` 失配审计 → `social-loop-audit-2026-11/evidence/` 与 `C:\Users\11036\Desktop\正确客户端截图\` 原生截图落位。

---

## 7. 社交经济 UI 资源证据覆盖度小结

**① 三重证据齐全（.nx 引用 + raw-nx-dump + 原生截图）**：
`UIUserList` 最强（802 行 dump / 158 节点 / 48 引用·25 失配 / frames 5 张 oracle 帧 /
evidence/guildpage 逐像素行带 / bind_runtime_assets 绑定），家族(Guild)/家族联盟(GuildUnion)
作为 UserList 子节点同享；`UIFamily`（F 键学院）= window("Family") + family_backgrnd
224x392 + 学院.png 227x392 + academy 3x 实测 + 8 按钮坐标。

**② 有 .nx 引用 + 083 清单但无 raw dump/无专属截图（代码坐标级）**：
Messenger(:9)、TradingRoom(:19 565x474 布局注释)、Shop(shop_root,:8)、StoreBank(:54)、
Wedding(:59 Invitation/Cathedral/Vegas)、MapleTV(:61)、Memo(:37)、PartySearch(:78)、
FamilyTree(:91)、GameOpt/SysOpt(:13/:14)、SoftKeyboard(:35)、CashShop.img、
GuildBBS.img（仅 runtime.json 证实根名）。

**③ 节点名失配/显式 NO-NATIVE/完全无资源证据**：
`UIStorage(Storage.img)×083 Trunk(:29)`、`UIMerchant(CashShop.img/CSList)×083 PersonalShop(:20)`
节点名失配；`UIStorage.cpp:42-44` 明写「既无参考截图也无 IDA 证据 ⇒ 底衬与
BtArrange/BtMesoIn/BtMesoOut 显式预留」= 资源层空白；
`UIParty(Party.img)/UIBuddyList(BuddyList.img)/UIGuild(GuildUI.img)` 根级 .img 包内
nx_inspect 实测不存在，仅 ctor 坐标 + close_btn = 完全无 IMG 资源证据；
`docs/features/trade-house-083/` 自绘 PSD 明写「样板无/采样未实测/目标节点待定
v083经验基线」且 083 顶层清单无此窗 = NO-NATIVE 自定；
mappanel.mesos / SoftKey 提示串 / userlist.* 英文 stub / en-US.ini 反向中文硬编码 =
文案层 NO-NATIVE/占位。

**流程阻塞**：orange-wz(IMG) MCP 未接入 Host，REA/IDA 侧对 `UIWindow.img/UserList`
亦无逐节点常量 ⇒ 原生还原主路径 REA→IDA→IMG 在「IMG 解析」环节阻塞，
当前「仓库内 .nx 节点引用 + nx_inspect.py dump + 正确端截图」替代链成立。

---

*性质：本文件为只读证据归档；[观察]/[推断]/[s9 适配] 已逐条标注。
子代理声明「全程只读、未改文件、未连库、未改 MCP 配置」；主 agent 未修改任何产品源码、
未构建、未实机、未提交。*
