# r03 · 核心窗口 IMG 资源与样式证据（三路子代理之三 + 主 agent 复核）

> 来源：subagent `2956105c-9609-40af-9309-4d93250bff53`「核心窗口 UI 资源证据盘点」12K 全文（只读、未改任何文件），主 agent 结构化归档并加 §0 复核块 / §7 交叉印证。
> 门禁：已加载 `xiaoye-maple-story-dev`；基准固定 **083/v83**；每条带相对路径:行号 + `[观察]/[推断]`。
> 范围：Skill/SkillEx/Macro/Enchant/Quest/QuestAlarm/ToolTip/Item/Equip/Stat/UserInfo/KeyConfig/WorldMap/ItemMaker/CashShop/CashGachapon。
> 时间：2026-11-08（goal `goal-11388c42` Round 2）。

---

## 0. 主 agent 复核块（本笔记最高价值：销案 r02 §8.1 疑点）

### 0.1 ★ `UIToolTip.img` 疑点已销案 —— 不是缺口，是「改造前旧态」

r02 §8.1 曾记：当前端 `EquipTooltip.cpp:53` 实际用 `nl::nx::UI["UIWindow.img"]["ToolTip"]["Equip"]`，而 `docs/features/ui-v83-window-restore/PLAN.md:20,34` 记的是 `UI["UIToolTip.img"]["Item"]`，疑为文档过时。第三路给出闭环证据：

- `docs/features/ui-v83-window-restore/README.md:74` 逐字：**「tooltip 根 `UI["UIToolTip.img"]["Item"]` 全部清零；框体真身 = `TooltipBackdrop` 黑 0.9α + 1px 灰边(0.5,0.5,0.5)，托管 TextTooltip.h；装备框固定宽 264」**。[观察]
- `IO/Components/TextTooltip.h:28-29` 注释「`UIWindow.img/ToolTip` 无九切片、包内无 `UIToolTip.img`」；`docs/features/ui-v83-window-restore/evidence/w5-tooltip/coords.md:9-11` Q1 定案「框体 = `Color(0,0,0,0.9)` 底 + `Color(0.5,0.5,0.5)` 1px 四边框，共享件 `ms::TooltipBackdrop`（定义在 `IO/Components/TextTooltip.h`，5 个 tooltip 组件共用）」；`:19-23` 排除 5 族九切片候选。[观察]
- `docs/features/ui-v83-window-restore/notes/asset-node-inventory.md:11` 本包仅 20 个 UI 根（`Basic BuffIcon CashShop CashShopPreview ChatBalloon DialogImage GuildBBS GuildMark ITC ITCPreview Login Logo MapLogin MapleEzorsiaV2wzfiles MapleTV NameTag Npt_GuideLine StatusBar UIWindow tutorial`），**不存在** `UIToolTip.img` / `UIWindow2/4/6/GL.img` / `StatusBar3.img`；原生 exe 内 `UIToolTip` 0 命中；`:18` `UIWindow.img` 有 **112 个顶层窗体根**含共享件 `BtUIClose/BtUIClose2/IconBase/Macro/ToolTip`。[观察]
- `docs/features/ui-v83-window-restore/notes/asset-node-inventory.md:22-25` `UIWindow.img/ToolTip` = `Equip{Can{0-9,beginner,bowman,magician,max,none,percent,pirate,reqDEX,reqINT,reqLEV,reqLUK,reqPOP,reqSTR,thief,warrior},Cannot{…},Dot{0,1,2},GrowthDisabled,GrowthEnabled,ItemCategory{1-10,12,21},Property{0-18},Speed{0-6},WeaponCategory{30-47子集}}` + `cash[23x5]` + `mesos[30x5]`；**无 c/e/ne/n/nw/w/sw/s/se 九切片**。[观察]
- `docs/features/minimap-mode-switch/README.md:99` 运行时 `UIWindow.img_runtime.json` 的 `/ToolTip` children **只有 `['Equip']`**、无 `WorldMap`。[观察]

→ **判定：`PLAN.md:20,34` 是改造前记录，`README.md:74` 已记「全部清零」；当前端 5 个 Tooltip 组件走 `UIWindow.img/ToolTip/Equip` + 程序描边 `TooltipBackdrop`，是已完成的正确改造，不是「指向不存在节点」的活缺口。** r02 §8.1 与 Q2 清单 C14（`docs/features/feature-gap-inventory-2026-11/README.md:141`）相关条目应标「历史账已销案」，交 README 决策点统一回写。[观察+推断]

### 0.2 与 Q2 清单的一致性冲突（须回写）

`docs/features/feature-gap-inventory-2026-11/README.md:141` C14 仍写「5 个 tooltip 组件指向不存在的 `UI["UIToolTip.img"]["Item"]`」——按 §0.1 属**旧账**：`README.md:74` 已清零、`TextTooltip.h` 已用共享 TooltipBackdrop。与 HUD 面已发现的 Q2 §16 #2/#9、`channel_count<2`、`[DBG-CLK]` 同类（旧账未回查源码），统一交本面决策点裁决是否回写 Q2 文档。[观察+推断]

### 0.3 IMG 层门禁结论（三面一致）

- 本 DSH 会话 `mcp_connector_status` 仅 `[json-photoshop] photoshop`（状态未知/Host 未注册工具），`mcp_connector_catalog(keyword=orange)` = 0 → **orange-wz 未接入本会话**。[观察]
- `C:/Users/11036/.cursor/mcp.json:3-6` 声明 `"orange-wz": {"type":"http","url":"http://127.0.0.1:10012/mcp"}`、`:7-10` `"ida-pro-mcp": "http://127.0.0.1:13337/mcp"`；`netstat` 只读确认 `127.0.0.1:10012 LISTENING PID 28756`、`13337 PID 31332` → **端口在听但属 Cursor 侧配置，非本会话 MCP**。[观察+推断]
- `docs/features/login-flow-regression-fix/notes/img-evidence-audit.md:7`「orange-wz MCP 对这批私服改版 img 内部节点有已知解析缺陷，未使用」；`:4-6` 唯一真值 = 运行时部署 `E:\MXD\BeiDou-Client-xy\Data\UI\*.img` + `REA反编译/wz_dump/*_runtime.json`（R1/R2）；`docs/features/multi-resolution/evidence/rea-ijl15-identity.md:30` 记 `Index 16 out of bounds` 假象。[观察]
- → **IMG 层记阻塞**（与登录面、HUD 面一致），但本面仓库内 IMG 证据比前两面充分：`docs/features/ui-v83-window-restore/notes/raw-nx-dumps/` 有 6 个核心窗节点树（见 §3.1）。[观察+推断]

---

## 1. `UIWindow.img/*` 节点与 `UI["..."]` 字面量

### 1.1 权威节点清单（`tools/nx_inspect.py` 实测，本包唯一真值）

- `docs/features/ui-v83-window-restore/notes/asset-node-inventory.md:3` 取证方式 `python tools/nx_inspect.py list "E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx"`，日期 2026-10-31。[观察]
- `:9-11` 20 个 UI 根 + 三个不存在的根（见 §0.1）。`:13-15` 根换代：`Storage.img→UIWindow.img/Trunk`、`Trade.img→UIWindow.img/TradingRoom`、`BuddyList.img→UIWindow.img/UserInfo`。[观察]
- 逐窗件表：`:27-30` `UIWindow.img/Item` `backgrnd[175x289]/FullBackgrnd[603x289]/Tab{disabled,enabled{0-4}}/New{Tab0{0-4}[34x18],Tab1{0-4}[34x19],inventory{0-3}[36x36]}/BtCashshop[93x18]{4态}/BtFull,BtGather,BtSmall,BtSort[12x12]/Quality{0-3}/activeIcon[36x36]/disabled[32x32]`，✗BtSetup/滚动条/关闭钮。[观察]
- `:32-35` `UIWindow.img/Stat` `backgrnd[208x347]/backgrnd2[218x203]/basicStat[159x121]/BtDetail[46x19]/BtApUp{4态}[12x12]/BtAuto[100x35]/Disabled{DEX,INT,LUK,STR}/Job{main,sub}`，✗BtApDown/BtDetailClose/cover0/cover1。[观察]
- `:37-41` `UIWindow.img/Skill` `backgrnd[175x289]/Tab{disabled,enabled,AranButton}/skill0,skill1[141x35]/line[141x1]/recommend{0}[105x35]/tip0,1,2[193x102]/BtSpUp{4态}[12x12]/CoolTime{0-15}[32x32]`，✗main/宏指令钮(独立根 Macro)/滚动条。[观察]
- `:43-53` `UIWindow.img/Equip` `backgrnd[175x304]/FullBackgrnd[208x291]/pet[177x181]/BtPet1,2,3[30x27]/BtCashshop[32x11]{4态;mo/n 有 0-5}/BtDetail[46x19]`；origin 实测 backgrnd/FullBackgrnd/pet=(0,0)、BtPet1/2/3=(15,13)；槽位相位 x∈{5,38,71,104,137}、行 y=32+33k。[观察]
- `:55-58` `UIWindow.img/Quest` `backgrnd[245x396]/backgrnd2-5/Tab{disabled,enabled}/icon0[13x14]..icon9{0-7}/BtAlert,BtDetail,BtGiveup,BtMarkNpc,BtNo,BtOK(各 5 态)/Gauge{frame[163x14],gauge[1x11],spot[1x11]}/Gauge2{frame[79x8],gauge[1x4]}/notice{0-2}[73x24]/TimeQuest{AlarmClock,TimeBar}`，✗关闭钮/滚动条。[观察]
- `:64-65` `UIWindow.img/WorldMap` **只有** `Border{0[60x33],1[1x33],2[7x32],3,4[7x1],5,7[7x18],6[1x18]}` + `title[1x1]`（极稀疏）。[观察]
- `:94` `CashShop.img` 23 根（`Base CS3thEvent CSChangeName CSChar CSDiscount CSGift CSIcon CSInventory CSItemSearch CSList CLocker CSMemberShop CSNotice CSStatus CSTab CSTransferWorld CSWebMsg CashItem GuideWords NewItem PicturePlate PrepaidCashItem`）。`:95` 全包完整九切片只有 5 族。`:97-146` 三窗实测件表 + 关闭钮 A/B/C/D 四类（D = `UIWindow.img/BtUIClose` 32x15 origin(0,0)）。[观察]

### 1.2 资源解析层（`Util/UiAssetResolve.cpp`，决定节点路径如何被解析）

- `:42-55 window(const char* name)` = `UI["UIWindow2.img"][name]` → 回退 `UI["UIWindow.img"][name]`，都无则 diag "neither UIWindow2.img nor UIWindow.img has it"。`:57-75 window4()` = UIWindow4→UIWindow2→UIWindow。`:77-83 status_bar_root()` = StatusBar3.img 优先→StatusBar.img。`:105-113 shop_root()` = UIWindow2.img/Shop2→UIWindow.img/Shop。`:115-131 close_btn()` 顺序 `static const char* const order[] = { "BtClose", "BtClose2", "BtClose3" }` 取 Basic.img。`:133-145 usable(nl::node n)` = `n.size()>0 或 data_type()==bitmap`（防空壳）。`:25-40 diag_once(tag,msg)` 追加一行到运行目录 `_ui_asset_diag.log`（同 tag 只写一次）。`Util/UiAssetResolve.h:21,24,27,36,43,46,50` 声明（`:46` 注 `"Basic.img/BtClose (12x12) — a solid blue square carrying a white X"`）。[观察]

### 1.3 核心窗口 `window()/window4()` 定位调用点（逐字）

`IO/UIStateGame.cpp:321,330,339,348,438,450,526,538,399` → `window4("Equip")` / `window("Item")` / `window4("Stat")` / `window("Skill")` / `window("Quest")` / `window("KeyConfig")` / `window("Maker")` / `window("UserInfo")` / `window("WorldMap")`（同文件 :292 PartySearch / :388 UserList / :406 Messenger / :520 MonsterBook / :547 Channel / :714）。[观察]

逐类：`IO/UITypes/UIEquipInventory.cpp:51,53,92` `window4("Equip")`；`UIItemInventory.cpp:78,80,131` `window("Item")`；`UIStatsInfo.cpp:47,49,62` `window4("Stat")` :820 同族；`UISkillBook.cpp:50,52,144` `window("Skill")` :184 `window4("Skill")["skillPoint"]` :248 `window("Item")["New"]`；`UIQuestLog.cpp:145,147` `window("Quest")`；`UIKeyConfig.cpp:54,56,70,73` `status_bar_root()["KeyConfig"]` 回退 `window("KeyConfig")`；`UIQuickslotConfig.cpp:22` `window("KeyConfig")`；`UILoginNotice.cpp:374,474` `window("KeyConfig")["KeyType"]/["alert"]`；`UIWorldMap.cpp:50,52` `window("WorldMap")`；`UICharInfo.cpp:54,56` `window("UserInfo")`；`UIMaker.cpp:55` `window("Maker")`；`UICashGachapon.cpp:31` `window("CashGachapon")`；`UICashShop.cpp:56` `UI["CashShop.img"]`；`UIMiscShells.cpp:717` `window("EnchantSkill")` :3854 ShortCut :5766 GameMenu :7552 UtilDlgEx；`UIUserList.cpp:60,62` `window("UserList")`；`UIStatusBar.cpp:787,797,807,817` `usable(window("Equip"/"Item"/"Stat"/"Skill"))` 四窗可用性探针；`UITrade.cpp:55` `window("TradingRoom")`；`IO/Components/MapTooltip.cpp:33` `window("ToolTip")["WorldMap"]`（运行时不存在）；`IO/Components/Slider.cpp:38` status_bar_root()。[观察]

### 1.4 直接 `nl::nx::UI[...]` 核心窗口字面量

`IO/UITypes/UIQuestLog.cpp:55` `UI["UIWindow.img"]["Quest"]` :150 `uiroot["UIWindow.img"]["BtUIClose"]`；`UIQuickslotConfig.cpp:24` `UI["UIWindow.img"]["KeyConfig"]`；`UIMaker.cpp:47` `UI["UIWindow.img"]["Maker"]["backgrnd"]`；`UIStatusBar.cpp:851,2111` `UI["UIWindow.img"]["KeyConfig"]["icon"]` :2113 `UI["UIWindow2.img"]["KeyConfig"]["icon"]`；`UIStateGame.cpp:447` `UI["StatusBar3.img"]["KeyConfig"]`；`UISkillBook.cpp:175` `UI["Basic.img"]["BtMacro"]` :243 注 `UIWindow.img/Item/New/Tab1=粉底选中、Tab0=灰底` :357 注 `UIWindow.img/Skill/backgrnd 逐像素对齐 xoff/yoff=0` :525 注 `UIWindow.img/SkillMacro 207x289` :1105 注 `Skill/skillPoint 缺失` :174 注宏编辑窗独立根 `UIWindow.img/Macro`；`UIMiscShells.cpp:4705,4761,4788,4789` `UI["UIWindow.img"]["SkillMacro"]` + diag "UIWindow.img/SkillMacro missing"；`UIDailyGift.cpp:82` `UI["UIWindow2.img"]["Item"]["New"]["inventory"]` :84 回退 UIWindow.img；`UIKeyConfig.cpp:112` 注「本包只有 UIWindow.img/KeyConfig 一张底板 629x373，没有新版 button:XXX 命名」；`UIMiniMap.cpp:57,60` `UI["UIWindow2.img"]` / `UIWindow2 = UI["UIWindow.img"]`；`UIStatsInfo.cpp:107,164,215` 注 `cover0/cover1` 不存在、`UI["Basic.img"]["BtHide"]`、Stat SAD 比对；`UIWorldMap.cpp:60,113,654,657,1037` diag "UIWindow.img/WorldMap/Border missing"、`UI["Basic.img"]["BtHide"]["normal"]["0"]`、`nl::nx::Map["WorldMap"][map + ".img"]`；`UIAlliance.cpp:44` `UI["UIWindow.img"]["UserList"]["GuildUnion"]["backgrnd"]`；`IO/Components/EquipTooltip.cpp:53` `UI["UIWindow.img"]["ToolTip"]["Equip"]` :62 `UI["UIWindow.img"]["IconBase"]["0"]` :33 注 reqXXX 47x11 :60 注 IconBase/0 32x32 2x；`SkillTooltip.cpp:33,35,37` `UI["UIWindow.img"]["IconBase"]["0"]`；`MapTooltip.cpp:32,33` 注「ToolTip 下仅 Equip/cash/mesos」+ `window("ToolTip")["WorldMap"]`；`Icon.cpp:53` `Charset(UI["Basic.img"]["ItemNo"], LEFT)`；`Slider.cpp:43,46,58,69` `UI["Basic.img"]["VScr5"]/["VScr"]`；`MapleComboBox.cpp:37` `UI["Basic.img"][combobox]`；`ItemTooltip.cpp:63-69,99` **无** `nl::nx::UI` 引用（name A12B/240、desc A12M/185、`fillwidth=264`、`fillheight=83+name.height()`、框体包围盒外扩 6px 程序描边）；`TextTooltip.cpp:38-47,58-59` 无 `nl::nx::UI`（最小 fillheight 18、`backdrop.draw(pos+(-8,-8), fillwidth+16, fillheight+16)` 外扩 8px）。[观察]

### 1.5 头文件节点路径注释

`IO/Components/SkillTooltip.h:44` `Texture iconbase; // UIWindow.img/IconBase/0 灰底板`；`IO/UITypes/UIQuickslotConfig.h:15` 原生 `UIWindow.img/KeyConfig/quickslotConfig`；`UISkillBook.h:171,179,186` / `UIMiscShells.h:316,634,658,813` / `UISoftKey.h:27,30` / `UIUserList.h:124,188` / `UITrade.h:37,72` / `UIOptionMenu.h:70` / `UIMessenger.h:158` 均为 `UIWindow.img` 节点路径注释。[观察]

---

## 2. 通用组件定义与关键常量

- `IO/Components/Button.h:28-36` `enum State { NORMAL, DISABLED, MOUSEOVER, PRESSED, IDENTITY, NUM_STATES }`（5 态枚举；MapleButton 只用 4 态）。[观察]
- `IO/Components/MapleButton.h:28-29` 注 "A standard MapleStory button with 4 states and a texture for each state"；`:32-34` 三构造；`:44 Texture textures[Button::State::NUM_STATES]`；`:45 Animation animations[...]`；`:57-60` 注 "`position` is the sprite's **top-left**, not its anchor"；`:61-62 try_maple_btn(...)` 签名。[观察]
- `IO/Components/MapleButton.cpp:25-28` `maple_btn_nopos()` 返回 `Point<int16_t>(-32768,-32768)`；`:45-46 origin = Texture(src["normal"]["0"]).get_origin()`；`:50` position+origin；`:55-71` 构造取 `src["normal"]`(size>1→Animation)/`["pressed"]["0"]`/`["mouseOver"]["0"]`/`["disabled"]["0"]`；`:85-89 animations[state].update(6)`；`:91-108` bounds=parentpos+position-get_origin()。[观察]
- `IO/Components/MapleFrame.h:24-44` 九切片 `Texture center,east,northeast,north,northwest,west,southwest,south,southeast` + `int16_t xtile,ytile`；`IO/Components/MapleFrame.cpp:26-40` 取 `src["c"]["e"]["ne"]["n"]["nw"]["w"]["sw"]["s"]["se"]`、`xtile=max(north.width(),1)`、`ytile=max(west.height(),1)`；`:42-72` draw `numhor=rwidth/xtile+2`、`numver=rheight/ytile`、`left=position.x()-width/2`、`top=position.y()-height`（底边中心锚）九片平铺。[观察]
- `IO/Components/TwoSpriteButton.h:26-45` 二态 `BoolPair<Texture> textures` + `Point<int16_t> npos,spos`。[观察]
- `IO/Components/Charset.h:29-34` `enum Alignment { LEFT, CENTER, RIGHT }` :37 构造 :45 `std::unordered_map<int8_t,Texture> chars`；`Charset.cpp:30-43 charset_char(name)` Lbracket→'['、Rbracket→']'、slash 或 "\\"→'/'、percent→'%'、其余 name[0]；:100 CENTER 每字 `width+2`、:113 LEFT `getw(c)+1`、:118-129 RIGHT 逆序。[观察]
- `IO/Components/Gauge.h:27-34` `enum Type : uint8_t { DEFAULT, CASHSHOP, WORLDSELECT, CLIP }`（CLIP：自纹理左侧裁 `percentage*maximum` 宽，用于 v83 StatusBar HP/MP）；:36 默认 `type(DEFAULT),maximum(1),percentage(0),target(0),step(0),clip_left(0)`；`Gauge.cpp:31` `int16_t length = static_cast<int16_t>(percentage * maximum)` :35-40 DEFAULT 画 barfront/barmid/barend :41-45 CASHSHOP `pos_adj=(45,1)`。[观察]
- `Graphics/Text.h:33-48` `enum Font { A11M,A11B,A12M,A12B,A13M,A13B,A14M,A14B,A15M,A15B,A18M,A18B,NUM_FONTS }` = **12 档字号**；:50-55 Alignment :57-61 Background{NONE,NAMETAG}。[观察]
- `IO/Components/Icon.h:32-40` `enum IconType : uint8_t { NONE,SKILL,EQUIP,ITEM,KEY,NUM_TYPES }`；`:105-106 static constexpr int16_t cell_width = 32; cell_height = 32;`；`:91-104` 注 R27-B3-4 几何归 Icon 所有（避免装备窗 +4,+4 / 键盘设定 -adjust / 技能窗 SKILL_META_OFFSET 三处分叉）。[观察]
- `IO/Components/Textfield.h:32-37` `enum State { NORMAL, DISABLED, FOCUSED }`；:40 构造；`:56 set_accept_unicode(bool accept)` 默认 false；`Textfield.cpp:33-46` 构造 `marker=ColorLine(marker_height,text_color,1.0f,true)`、`state=NORMAL`。[观察]
- `IO/Components/ScrollingNotice.h:26-41` `ColorBox background; Text notice; Linear<double> xpos; bool active; int16_t width;`；`ScrollingNotice.cpp:22-30` `width=800; background=ColorBox(width,23,BLACK,0.535f); notice=Text(A12M,LEFT,YELLOW)` :55-62 宽度跟随 `Constants::get().get_viewwidth()` :64 `xpos-=0.5` :68-74 注原生滚出左界后连同 23px 全宽黑底消失。[观察]
- `IO/Components/Tooltip.h:26-42` `class Tooltip` + `enum Parent { NONE, EQUIPINVENTORY, ITEMINVENTORY, SKILLBOOK, SHOP, EVENT, TEXT, KEYCONFIG, WORLDMAP, MINIMAP }` + `virtual void draw(Point<int16_t> cursorpos) const = 0`。[观察]
- `docs/features/ui-v83-window-restore/evidence/w7-equip/coords.md:15-18` 坐标系口径：`try_maple_btn` 的 position=贴图左上角（`normal/0` origin 加回 anchor）；`MapleButton(src,pos)`/`TwoSpriteButton` 的 pos=anchor；`TwoSpriteButton` 的 nsrc=NORMAL、ssrc=MOUSEOVER/PRESSED。[观察]

---

## 3. 截图 / 布局 JSON / Photoshop 产物 / IMG 节点 dump

### 3.1 ★ 核心窗口节点树 dump（存在，6 个 —— 本面 IMG 证据强于前两面）

- `docs/features/ui-v83-window-restore/notes/raw-nx-dumps/01_tooltip.txt:1-149` `===== UIWindow.img/ToolTip`（Equip/Can 0-9[5x7]..reqXXX[47x11]/Cannot/Dot/Growth…/cash[23x5]/mesos[30x5]），**无九切片**。[观察]
- `raw-nx-dumps/Equip.txt:1-81` / `Item.txt:1-134` / `Quest.txt:1-334` / `Skill.txt:1-95` / `Stat.txt:1-79`（各 `===== UIWindow.img/<窗>` 完整件表）。[观察]
- `raw-nx-dumps/{basic1,slidemenu,statusbar_base}.txt:1`（非核心窗）；`docs/features/ui-v83-window-restore/evidence/w10-quest/_nx_quest_list.txt:1` 第二份完整 Quest 转储；`w10-quest/coords.md:4-9` 取证口径 `python tools/nx_inspect.py list "…/UI.nx" <depth> UIWindow.img/Quest`，拼合图 `w10-buttons.png/w10-strips.png/w10-icons2.png/w10-close-candidates.png`，参考截图 `pngs/ref-quest-full.png` **548x396 = 整窗**。[观察]
- **缺失**（[推断]）：raw-nx-dumps 下无 KeyConfig / WorldMap / UserInfo / Maker / CashShop / CashGachapon / Macro / SkillMacro / EnchantSkill / SkillEx / QuestAlarm 的独立节点树 dump。

### 3.2 正确端截图（`docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/`）

- `打开了S-E-I-K页面.png` 1280x720 = 技能/装备/物品/属性键设置页（核心窗口族）；`键盘设置.png` **629x369**，与 `UIWindow.img/KeyConfig` 底板 629x373（`IO/UITypes/UIKeyConfig.cpp:112`）宽度一致、高度差 4px → 推断为 KeyConfig 窗裁图。[观察+推断]
- 其余 5 张 1280x720 属登录链路；`current_shots/README.md:3-9` 要求同分辨率按 correct_shots 文件名对齐，配合 `notes/r201-login-overlay-playbook.md` 叠图；**correct_shots/ 无配套 layout JSON、无 manifest.md**；`r20/overlay_diff.py` 叠图对比脚本。[观察]

### 3.3 ui-v83-window-restore 逐窗证据

`evidence/w7-equip/coords.md:1-20`（引用 UI.nx → UIWindow.img/Equip/*，window4 映射）、`w8-item/coords.md`、`w9-skill/coords.md`、`w6-stats/coords.md`、`w11-map/coords.md`、`w10-quest/coords.md`、`w5-tooltip/coords.md:1-5`（复用 `_w5_tooltip_root.txt`/`_w5_chatballoon.txt`/`_w5_worldnotice.txt`/`_w5_partyhp.txt`/`_w5_iconbase.txt` + `pngs/pixel_report.txt`）；`evidence/r27-oracle/EquipTooltip.json` + `EquipTooltip_native_crop.png` + `frames/EquipTooltip_native_crop.png`；`notes/native-ui-asset-strings.txt` BeiDou.exe 内 38 条 `UI/*.img/*` 字面量；该目录**无 manifest.md**。[观察]

### 3.4 布局 JSON（`**/layout*.json` 共 5 个，均非核心窗口）

`docs/features/char-stats-ui-ps/ui/detail/v4/layout_cn.json:2-30` canvas 1346x600 → final 658x392，items 含「角色资料」标题(12px CENTER)，配套 `Character_Stats_Detail_V4.psd` + `assemble_v4.jsx` + `build_v4_componentized.jsx` + `components/*.png`（约 30 件）+ `INDEX.txt`；658x392 与原生 `UIWindow.img/Stat backgrnd 208x347` 差异大 ⇒ 属 PS 自绘扩展面板、非 v83 原生底板对应 [推断]。其余 4 个 = map-search-ui×2 / map-info-panel / `UI/MapInfo/layout.json`（240x537），均地图搜索/信息面板。[观察]

### 3.5 Photoshop 出图产物（`**/ui/*/vN/` 356 路径）

`docs/features/char-stats-ui-ps/ui/detail/v4/`（Stat 族 PS 工程，有 layout_cn.json、**无 manifest.md**，manifest 只在 v3）；`docs/features/hotkey-grid-3x3/ui/hotkey-grid/v1,v2/`（热键网格 PSD + PNG，KeyConfig 相关非原生 IMG）；`docs/features/multi-resolution/ui/uisysopt/v1/reference_分辨率.png`；`daily-checkin/ui/window/v3/manifest.md`、`addon-equipment/ui/window/v3/manifest.md`（自造 UI 非核心窗口族）；`**/*img.xml` 全仓仅 `docs/features/v83-hud-quest/evidence/StatusBar.img.xml`（StatusBar 非核心窗口）；`**/*dump*.txt` 5 个均登录/软键盘系；`**/*runtime*.json` 仓库内仅 4 个 `.runtimeconfig.json`，**无 `*_runtime.json` IMG dump**。[观察]
- `docs/features/ui-layout-v83-unify/evidence/ida-layout-tables.md:3-30` **IDA 布局表：KeyConfig ctor `0x832586`、资源 `UI/UIWindow.img/KeyConfig`、槽位表 `dword_BE27E0`（32×32 命中）、GRAVE(14,66)..F10；Equip ctor 宽 175（或龙装备 208）× 高 304、槽位表 `dword_BE2260`**。[观察]
- `docs/features/ui-layout-v83-unify/evidence/*.png` 15 张（acct_slot 220x60、btn_menu/btn_npt/btn_shop/btn_short 54x34、charInfo 183x115、charInfo1 183x151、gauge_bar/gauge_grad 340x31、help0 311x77、panel_mid 350x270、pass_slot 220x70、qs 466x80、qs08 151x80）。[观察]
- `docs/features/multi-resolution/notes/img-position-attributes.md:5` 权威源 = 运行时部署 `E:/MXD/BeiDou-Client-xy/Data/UI/*.img` 经 `MapleStory.exe --resource-probe` 产 `REA反编译/wz_dump/*_runtime.json`（20 个 UI img 完整树）；红线 R1/R2 不得信任打包 `E:/project/sdlMS/build/Data2/UI.wz`（已证实 Stat 底板宽度 175→208 漂移）；`Util/Resource/ResourceProbe.cpp:552` `std::string out_name = std::string(imn) + "_runtime.json";`。[观察]
- `docs/features/ui-v83-window-restore/notes/r28-equip-tooltip-align.md:26` 仓库外 `E:\MXD\BeiDou-Client-xy\REA反编译\wz_dump\UIWindow.img_runtime.json`：`ToolTip/Equip/Can`+`Cannot` 的 reqLEV/reqSTR/reqDEX/reqINT/reqLUK/reqPOP 均 47×11、数字 0-9 5×7、none 5x1、percent 5x7；`IconBase/0` 32×32（**仓库外，仓库内无此 dump**）。[观察]

---

## 4. locale 核心窗口文案 key 全集

### 4.1 zh-CN.ini（635 行）

- `:11-24` equip tooltip 标签/性别/潜能（`equip.label_type=装备分类：{}`、`label_wepspeed=攻击速度：{}`、`label_slots=可升级次数：{}`、`label_hammers=金锤子已提高的强化次数：{}`、`label_atkinc=攻击力增加`、`label_itemid=道具ID：{}`、`label_max=上限`、`gender.male=(男)`、`pot.hidden=(潜在属性)`..`pot.legendary=(传说物品)`）。[观察]
- `:27-41` `equip.stat.STR=力量：{}` / DEX / INT / LUK / MaxHP / MaxMP / Attack_Power / Magic_Attack / Defense / MAGIC_DEFENSE / ACCURACY / AVOID / HANDS / Speed / Jump（15 键，后缀=EquipStat::names 原文）。[观察]
- `:44-75` `equip.type.*` 32 键（HAT=帽子 … CASH=时装）；`:78-86` `equip.speed.FAST_(1..4)=快 (n)` / `NORMAL_(5..6)=普通 (n)` / `SLOW_(7..9)=慢 (n)`。[观察]
- `:95-97` 注「只登记有原生截图证据的命名空间」+ `skill.bookname.000=冒险家技能`（唯一 skill 键）；`:228 worldmap.players.online=在线: {}`（唯一 worldmap 键）。[观察]
- `:344-347` **`cashshop.stub_job=Illium` / `cashshop.stub_name=ShomeiZekkou`（显式 stub，英文假数据）** + `cashshop.meta=CS inv {} gift {} wish {}` / `meta_stor=stor{}`；`:348-354` **`charinfo.medal_stub=Junior Adventurer`（stub）** + `charinfo.pet_wish=Pet {} Wish {}` / `medal_id=Medal#{}` / `married=Married` / `book_count=Book {}` / `prompt_note=请输入便签正文` / `prompt_note_to=请输入收件人角色名`。[观察]
- `:393-397` `maker.bad_item_id=Bad item id` / `craft=Craft {}` / `no_recipe=No recipe {}` / `cost_meso={} meso` / `cost_req=(Lv{})`（英文原值占位）。[观察]
- `:444-454` `quest.area.20=彩虹岛` / `quest.area.30=金银岛` / `quest.detail.id=任务编号: {}` / `level_range=等级范围: {} ~ {}` / `min_level=需要等级: {}` / `area=所属区域: {}` / `npc=相关NPC编号: {}` / `pre_none=前置任务: 无` / `pre=前置任务: {}` / `quest.chain_page=连锁任务 ( {} / {} )`；`:453` 注「IDA pool 4497 "Chain Quest" + 格式 "%s ( %d / %d )"」。[观察]
- `:514-520` `enchantskill.e_equip_s_scroll_enter=E=equip S=scroll Enter` / `enchant_slot=Enchant slot #{}` / `scroll=Scroll {}→{}` / `need_scroll_equip=Need scroll+equip` / `equip=Equip #{}` / `scroll_2=Scroll #{}` / `scroll_3=Scroll {}`（英文原值）。[观察]
- `:570-573` `skillex.skillex_softkey_skillid=SkillEx — SoftKey skillid` / `bad_id` / `use_lv` / `use`；`:577 questalarm.start=Start {}`；`:604,607,618` `macro.macro_softkey_name` / `common.macro_full=Macro full` / `macroskill.macroskill_softkey_name`；`:616-617 skillup.skill_up=Skill Up` / `skillup.sp=SP {}`；`:619-621 questicon.q=Q{}` / `done=Done {}` / `forfeit=Forfeit {}`；`:179-187 userlist.none/cur_location/default_group/friend_group/location/union_none/need_data/use_alliance_window`。[观察]
- **无独立 key 前缀**（[推断]）：`tooltip.*`、`stat.*`（仅 equip.stat.*）、`item.*`（ItemTooltip 走程序绘制 + 英文硬编码，`IO/Components/ItemTooltip.cpp:55-61`）、`keyconfig.*`、`userinfo.*`（走 charinfo/userlist）。

### 4.2 stub / NO-NATIVE 标注台账（zh-CN.ini 注释）

`:102-103` Q5 chat.target 后四项 NO-NATIVE（决策 B 先建键）；`:151-153` NO-NATIVE 版本号前缀（`evidence/r27-b1c-i18n-scan/pending-copy.md`）；`:167-174` NO-NATIVE 地图侧栏；`:176-187` NO-NATIVE userlist/viewallchar；`:195-199` NO-NATIVE R27-B1c-4 中文硬编码批次；`:249-253` R27-B1c-5a 壳窗批次「zh 侧照抄原文 = 运行时像素级零变化…只建键、不翻译」；`:327-332` R27-B1c-5b 壳窗批次（含 **UICashShop / UIMaker / UICashGachapon / UICharInfo**）「值 = 改动前代码里的英文原字面量（用户 2026-11-02 决策 B：保持英文原值，只建键）」；`:443` R27-B1c-5c 中文盲区四窗（UIQuestLog / UIGuild / UIMerchant）；`:480-483` R27-B1c-6 UIMiscShells 批次（**134 键 / 261 站点**，含 enchantskill.* / skillex.* / questalarm.* / macro.* / macroskill.* / skillup.* / questicon.*）；`:629-630` R27-B1c-7 接线器补收。[观察]
→ 结论（[推断]）：**equip.*（含 stat/type/speed）为已译中文**；skill.bookname / quest.detail / quest.area / quest.chain_page / worldmap.players.online 为中文；**cashshop.* / charinfo.* / maker.* / enchantskill.* / skillex.* / questalarm.* / macro.* / macroskill.* / skillup.* / questicon.* 全为英文原值占位（NO-NATIVE 决策 B）**，且 `cashshop.stub_*` / `charinfo.medal_stub` 是显式 stub。

### 4.3 en-US.ini（632 行，行号略异）

`:11-83` equip.* 全英文对应；`:93 skill.bookname.000=Adventurer Skills`；`:226 worldmap.players.online=Online: {}`；`:344-347` cashshop.* 与 zh 同值；`:391-395` maker.* 同值；`:441-451` 注「R27-B1c-5c 中文盲区四窗（英文措辞为本包自造，无原生依据 ⇒ pending-copy 待用户定稿）」，`quest.area.20=Rainbow Island` / `quest.area.30=Kernese` / `quest.chain_page=Chain Quest ( {} / {} )`；`:511-517` enchantskill.*；`:567-570` skillex.*；`:574` questalarm.start；`:601` macro；`:613-618` skillup/macroskill/questicon；`:97,99,127,149,151,156,164,173,186,189,193,247,326,477,479-480,626-627` NO-NATIVE / pending-copy 台账。[观察]

---

## 5. Configuration 核心窗口设置项与默认值

### 5.1 Configuration.cpp 描述表

`:140-143` 界面开关 `ui.buff_timer`（buff 图标剩余时间）/ `ui.equip_compare`（装备悬停显示同部位已装备对比）/ `ui.language`（zh-CN 默认、en-US 热重载）/ `ui.maplepoints.local`；`:145-178` 界面位置 **33 项** `ui.position.stats/eqinv/inv/skill/quest/map/userlist/chat/minimap/shop/notice/maplechat/channel/joypad/event/keyconfig/optionmenu/charinfo/messenger/partysearch/delivery/storebank/vicioushammer/vegaspell/cashgachapon/remotegachapon/family/familytree/memo/gameopt/sysopt/dailygift/partyrace`（关闭客户端时的最后窗口位置）；`:122-130` `ui.sysopt.quality/mouse_speed/hp_warn/mp_warn/skill_effects/window_mode/alt_enter/mob_info/screenshot_path`；`:133 ui.gameopt.mask`；`:136-137 ui.autopot.hp_item/mp_item`；`:88 window.fullscreen`。[观察]
- `:549-551` 历史 bug 注「早期版本反着 prepend（"a.b.c" 会变成 "c.a.b"）… **31 个 ui.position.* 窗口坐标**、audio.*.volume、font.path.*、ui.autopot.* 等 39 项在 Settings 里改了也不生效…每次启动都退回代码默认值」（现已修，正向拼接 `:552-559`）；`:195` 把 "ui.position.stats" 拆成 ["ui","position","stats"]。[观察]

### 5.2 Configuration.h 默认值

`:529 BuffTimer() : BoolEntry("ui.buff_timer","true")`；`:537 EquipCompare() : BoolEntry("ui.equip_compare","true")`；`:100-101,112-114` get_buff_timer()/get_equip_compare()/ui.language 声明。[观察]
- 位置项：`:664 PosSTATS "(72,72)"` / `:671 PosEQINV "(250,160)"` / `:678 PosINV "(24,24)"` / `:685 PosSKILL "(96,96)"` / `:692 PosQUEST "(300,160)"` / `:700 PosMAP "(313,64)"` / `:707 PosUSERLIST "(104, 104)"`（`:788` 注「导致生成的 Settings 中 ui.position.userlist 出现两次并互相覆盖」）/ `:782 PosKEYCONFIG "(144, 144)"` / `:893 PosCHARINFO "(264, 264)"` / `:930 PosCASHGACHAPON "(340, 160)"` / `:791,799,806 PosOPTIONMENU "(170, -1)"` / `PosGAMEOPT "(170, 24)"` / `PosSYSOPT "(330, 24)"`（`:795-796,:803` 注「非 oracle 读数，可拖拽落盘」）/ `:714,726,733,740,747,754,761,768,775,899,905,910,915,920,925,935,940,945,950,956,961` 其余（chat `"(0, 572)"`、minimap `"(5, 24)"`、shop `"(146, 48)"`、notice `"(400, 285)"`、messenger `"(500, 80)"` 等）。[观察]
- 分辨率/视口：`:465 Fullscreen "false"` / `:472 Width "1280"` / `:479 Height "720"` / `:488 GameWidth "1280"` / `:493 GameHeight "720"` / `:498 GameTier "255"`（TIER_NONE）/ `:505 VSync "true"`；`:482-485` 注「登录态恒 1280×720，进游戏后按三项切换，档位表 Util/ResolutionTiers.h」；`locale/zh-CN.ini:140-149 sysopt.label.resolution=窗口大小` + `sysopt.combo.resolution.0..8`（800x600/1024x768/1366x768/1600x900/1920x1080/1280x720/1440x900/1680x1050/2560x1440，注「v83 原生无此行，档位表来自插件 rs_tier_dims」）。[观察]
- `:46` 取值优先级 `xy-config.yaml > Settings > Entry 构造参数中的内置默认值`；`:660-661,667-668,674-675,681-682,688-689,695-696,778-779,889-890` 各 Pos* 对应 UI 类（UIStatsInfo/UIEquipInventory/UIItemInventory/UISkillBook/UIQuestLog/UIWorldMap/UIKeyConfig/UICharInfo）。[观察]
- **缺口**（[观察]）：Configuration.h 无 `PosMAKER` / `PosCASHSHOP` / `PosENCHANT` / `PosQUESTALARM` / `PosMACRO` / `PosTOOLTIP`（这些窗无独立位置落盘项）。

---

## 6. UI 资源证据覆盖度小结（子代理原文结论）

**A. 有 IMG 节点证据（raw-nx-dumps + 件表 + 代码引用 + 截图/coords.md）**：ToolTip（01_tooltip.txt 149 行；框体 TooltipBackdrop 程序描边；**UIToolTip.img 确认不存在**）、Item（Item.txt 134 行）、Equip（Equip.txt 81 行 + ida-layout-tables 175/208×304）、Stat（Stat.txt 79 行 + 208x347；char-stats-ui-ps 为自绘扩展面板）、Skill（Skill.txt 95 行）、Quest（Quest.txt 334 行 + w10-quest 548x396 整窗）、KeyConfig（UIWindow.img/KeyConfig 629x373 + ida-layout-tables 槽位表 dword_BE27E0 + 键盘设置.png 629x369；无独立 raw-nx-dump）、WorldMap（Border{0..7}+title 极稀疏；w11-map/coords.md）、CashShop（23 根；无 dump）、Maker（无 dump/件表）、UserInfo/UserList（无独立 dump）。
**B. 只有代码坐标/注释/文案，无 IMG 节点 dump**：Macro/SkillMacro、Enchant/EnchantSkill、SkillEx、QuestAlarm、CashGachapon；`Skill/skillPoint`（UISkillBook.cpp:1105 注「本包 WZ 中缺失」）、`Stat/cover0,cover1`（UIStatsInfo.cpp:107 注不存在）、`WorldMap/Border`（UIWorldMap.cpp:60 diag missing）→ 属「代码引用但本包缺件」。
**C. 完全无 IMG 证据**：`UIToolTip.img` 整个根、`UIWindow2.img`/`UIWindow4.img`/`StatusBar3.img`（被优先探测但本包不存在，全走回退）、CashGachapon/SkillEx/QuestAlarm 独立节点树、Macro/SkillMacro 完整件表、CashShop/Maker/UserInfo/KeyConfig/WorldMap 的 raw-nx-dump；仓库内无 `*_runtime.json` IMG 全量树、无核心窗口 `manifest.md`、核心窗口无配套 `layout*.json`。

---

## 7. ★ 主 agent 交叉印证：Angel 083 一手语义 ↔ 仓库内 IMG/IDA 布局表

> 本节是 r03 归档后主 agent 追加的最硬增量：把 r01 的 Angel 逐字常量与 §1/§3 的既有 IMG/IDA 证据对表，证明「上一轮 BeiDou 无符号校准的坐标与 Angel 同源」。

1. **KeyConfig 32×32 命中表 —— 两端同一张表（★ 最强印证）**
   - Angel `CUIKeyConfig::GetShortCutIndexByPos @ 0x833a65`（r01 §2）：`32×32 命中表`、基址 `dword_BE27E4..BE2C6C`、特例 `case '6':42; 'Y':29; 'Z':56`。
   - 仓库 `docs/features/ui-layout-v83-unify/evidence/ida-layout-tables.md:3-30`（上一轮 BeiDou 无符号 IDB 提取）：**KeyConfig ctor `0x832586`、资源 `UI/UIWindow.img/KeyConfig`、槽位表 `dword_BE27E0`（32×32 命中）、GRAVE(14,66)..F10**。
   - → `dword_BE27E0` vs `dword_BE27E4` 相差 4 字节（同一全局表的起始 vs 首个 `x,y` 对），**两份独立取证坐实同一张 32×32 槽位表**；当前端 `IO/Components/Icon.h:105-106` 也写死 `cell_width=32; cell_height=32`。[观察+推断]

2. **Equip 窗 175×304 / 208（龙装备）**
   - Angel `CUIEquip`（4 方法 `OnMouseMove/OnButtonClicked/ToggleDragonEquip/TogglePetEquip`）+ `CUIEquip` 未直接给尺寸；仓库 `asset-node-inventory.md:43-53` `UIWindow.img/Equip backgrnd[175x304] / FullBackgrnd[208x291] / pet[177x181] / BtPet1,2,3[30x27]`、`ida-layout-tables.md:3-30`「Equip ctor 宽 175（或龙装备 208）× 高 304、槽位表 `dword_BE2260`」。
   - → Angel `ToggleDragonEquip` / `TogglePetEquip` 两个切换方法正好解释「175 与 208 两套宽」「BtPet1/2/3 三页」；当前端 `IO/UITypes/UIEquipInventory.cpp:567,622` 记 `BtPet1/2/3` 三页宠物装备「显式预留、槽位数据待协议批次」（Q2 清单）。[观察+推断]

3. **Stat 窗 208×347 与 `CUIStat::OnCreate` 按钮体系**
   - 仓库 `asset-node-inventory.md:32-35` `Stat backgrnd[208x347] / backgrnd2[218x203] / BtDetail[46x19] / BtApUp{4态}[12x12] / BtAuto[100x35]`；当前端 `UIStatsInfo.cpp:358 dimension=(212,318)`、`:155 BT_AUTO(106,198)`「本包 BtAuto 为 100x35」、`:159 BT_DETAILOPEN(153,324)`「BtDetail 46x19」→ **尺寸逐点吻合**。
   - Angel `CUIStat::OnCreate @ 0x8c4c7f` 用 `SP_1943..1946_..._STAT_BTAPUP/BTAUTO*`、`SP_2064_..._BTDETAIL`、`SP_1947_..._DISABLED_` → 与件表的 `BtApUp/BtAuto/BtDetail/Disabled{DEX,INT,LUK,STR}` **一一对应**（Angel 只是把明文节点名编译成 StringPool `SP_*` 索引）。[观察+推断]

4. **Item 窗 BtGather/BtSort/BtFull/BtSmall 与 Angel `CUIItem::OnChildNotify`**
   - 仓库 `asset-node-inventory.md:27-30` `Item/BtCashshop[93x18]{4态} / BtFull,BtGather,BtSmall,BtSort[12x12]`；Angel `CUIItem::OnCreate` 用 `SP_1851_..._ITEM_BTGATHER` / `SP_1852_..._ITEM_BTSORT` / `SP_5580/5581_..._ITEM_BTFULL/BTSMALL` / `SP_4500_..._ITEM_BTCASHSHOP` → **4 个节点全部对上**（Angel `SP_*` 是同一批节点的编译期索引）。
   - 但 Angel `OnChildNotify` 只有 `0x7D4=BtSort` / `0x7D5=扩展背包` 两个 case（无 BtGather），当前端 `UIItemInventory.cpp:1440` 用 `0x7D3=BtGather / 0x7D4=BtSort` → 见 r02 §2.2：属国服/s9 适配 + 扩展背包语义缺失，须裁决。[观察+推断]

5. **Skill 窗 BtSpUp / CoolTime 与 Angel `CUISkill::OnSkillLevelUpButton`**
   - 仓库 `asset-node-inventory.md:37-41` `Skill/BtSpUp{4态}[12x12] / skill0,skill1[141x35] / CoolTime{0-15}[32x32] / tip0-2[193x102]`；Angel `CUISkill::OnCreate` 用 `SP_1923_..._SKILL_BTSPUP`、`SP_1924/1925_..._SKILL_SKILL0/1`、`SP_1926_..._SKILL_RECOMMEND_0`、`SP_2660_..._SKILL_LINE` → 对应；`OnSkillLevelUpButton @ 0x8acf1a` 的 `CanSendExclRequest(500,0)` + SP 校验是「点 BtSpUp 后的服务端前置」。当前端 `UISkillBook.cpp:1105` 注「`Skill/skillPoint` 本包 WZ 中缺失」→ 升级确认面板属包内缺件显式预留。[观察+推断]

6. **Quest 窗 548×396 整窗与 Angel `CUIQuestInfo` 15 方法**
   - 仓库 `w10-quest/coords.md:4-9` `pngs/ref-quest-full.png` **548x396 整窗**；`asset-node-inventory.md:55-58` `Quest/backgrnd[245x396] / backgrnd2-5 / icon0-9 / BtAlert,BtDetail,BtGiveup,BtMarkNpc,BtNo,BtOK / Gauge / Gauge2 / notice{0-2} / TimeQuest{AlarmClock,TimeBar}`；Angel `CUIQuestInfo::OnCreate` 用 `SP_1897..1901_..._QUEST_ICON0/1/2/4`、`SP_2664/2665_..._QUEST_TAB_ENABLED/DISABLED`、`SP_3232_QUEST_QUESTINFOIMG`。
   - 当前端 `UIQuestLog.cpp:168-185` 建 `MARKNPC/OK/NO/ALERT` 四钮但 `:903-906` 置 false、`:951-960` 显式预留（见 r02 §8.3）→ **Angel 原生 `OnChildNotify` / 15 方法体系里这四颗有行为，当前端按「无封包证据」显式不做**，属决策性缺口而非漏实现。[观察+推断]

7. **WorldMap 极稀疏 Border 与 Angel `MakeBorder`**
   - 仓库 `asset-node-inventory.md:64-65` `UIWindow.img/WorldMap` 只有 `Border{0..7}` + `title[1x1]`；Angel `CWorldMapDlg::MakeBorder` 用 `SP_2281..2288_UI_UIWINDOWIMG_WORLDMAP_BORDER_7..0`（**正好 8 个**）、`OnCreate` 用 `SP_2289_..._WORLDMAP_TITLE` → **8 片 Border + title 逐一对上**；当前端 `UIWorldMap.cpp:60` diag "UIWindow.img/WorldMap/Border missing"（可能走 `MapleFrame` 程序九切片替代，待 `18924e1b` 复核）。[观察+推断]

### §7 结论

**Angel 083 的 `SP_*` StringPool 索引 与 仓库内 `tools/nx_inspect.py` 实测的 `UIWindow.img/*` 明文节点名，是同一批资源节点的两种表示**；上一轮 BeiDou 无符号校准提取的坐标/槽位表（`dword_BE27E0` / `dword_BE2260` / 175×304 / 208×347 / 548×396）在 Angel 带符号 IDB 里全部能找到对应类方法。→ **本面再次钉死「排版层已对齐，Angel 只补符号语义 + 行为层」**，与登录面、HUD 面三面一致；「校准 ≠ 改回原生」的推荐默认值在本面同样成立。

---

## 8. 限制

- 子代理声明全程只读、未改任何文件、未连库、未改 MCP 配置。
- `read_image` 对 PNG 返回 `model "hy4free" does not declare image input` → 未做像素级视觉复核，截图只作尺寸/文件名证据。
- 仓库外 `wz_dump/*_runtime.json`（20 个 UI img 完整树）本会话未直接读取，仅按 `r28-equip-tooltip-align.md:26` 与 `img-position-attributes.md:5` 引用；若后续需 IMG 层完备证据须按 R1/R2 从运行时部署导出。
- 待 `18924e1b`（当前端源码盘点）与 `4cfbbb14`（既有文档四态）返回后并入 README 六维对照表。

---

## 9. 增量（第三路收尾版 20:24 补回）— `UIToolTip.img` 疑点终判 + 覆盖度精化

> 子代理 `2956105c` 于 20:24:59 发回收尾版完整清单（自包含 6 节 + 特别复核判定），主 agent 复核后把「新增且不与 §0–§8 重复」的部分固化成本节。该子代理声明全程只读、未改任何文件、已加载 `xiaoye-maple-story-dev` 门禁。

### 9.1 ★ `docs/features/ui-v83-window-restore/PLAN.md:20,34` 的 `UI["UIToolTip.img"]["Item"]` 终判

**判定：已过时（作为「修复前根因诊断」保留有效，绝非当前端实际代码）；且已被修正并定案。** 按门禁「live runtime behavior → checked-in source → comments and dead code」优先级，以当前端源码为准。[推断]

- `docs/features/ui-v83-window-restore/PLAN.md:20` 逐字「原生 `BeiDou.exe` 里字符串 `UIToolTip` **命中 0 次** —— 说明当前端引用的那批资产根根本不属于 v83」；`:22`「**走错资产代次**（HeavenClient 偏新路径：`UIToolTip.img` / `UIWindow2/4/6.img` / `StatusBar3.img` / `chat/ingame` / `VScr9`）」；`:34` (RC3)「5 个 tooltip 组件指向不存在的 `UI["UIToolTip.img"]["Item"]` → `MapleFrame` 拿到 9 个空纹理，什么都不画」+ 修复动作「先把根改到 `UIWindow.img/ToolTip`」；`:35` (RC4)「需求/属性图标实际在 `UIWindow.img/ToolTip/Equip/{Can,Cannot,Property,Growth*,Speed,WeaponCategory,ItemCategory}`，代码找 `UIToolTip.img/Item/Equip/Can`」+ 修复动作「改路径」。→ **这是对 HeavenClient 旧代码的根因诊断，不是现状描述**。[观察]
- `docs/features/ui-v83-window-restore/README.md:74` **修正后定案**逐字「tooltip：根 `UI["UIToolTip.img"]["Item"]`（不存在）**全部清零**；内容图标改 `UIWindow.img/ToolTip/Equip/{Can,Cannot,Property,Growth*,Speed,WeaponCategory,ItemCategory}` + `cash`/`mesos`；**框体真身定案 = 程序绘制 `TooltipBackdrop`**（黑 0.9α + 1px 灰边 (0.5,0.5,0.5)，托管在 `TextTooltip.h`）…装备框固定宽 264」；`CHECKLIST.md:70` (C1)「5 处 `UI["UIToolTip.img"]["Item"]` 引用**全部清零**」。[观察]
- 当前端代码实证（真值）：`IO/Components/EquipTooltip.cpp:53 nl::node Equip = nl::nx::UI["UIWindow.img"]["ToolTip"]["Equip"]`、`:62 ["IconBase"]["0"]`；`IO/Components/SkillTooltip.cpp:35 nl::node ib = nl::nx::UI["UIWindow.img"]["IconBase"]["0"]`、`:37 iconbase = Texture(ib)`；`IO/Components/MapTooltip.cpp:33 UiAssetResolve::window("ToolTip")["WorldMap"]`、`:32` 注「ToolTip 下仅 Equip/cash/mesos」；`IO/Components/ItemTooltip.cpp:63-69,99` 与 `IO/Components/TextTooltip.cpp:38-47,58-59` grep `UI[|window(|UiAssetResolve|.img|Texture(|Charset(|MapleFrame(` **No matches found** → 框体纯程序描边（ItemTooltip 外扩 6px / TextTooltip `:58-59 backdrop.draw(pos + (-8,-8), fillwidth+16, fillheight+16)` 外扩 8px），无 IMG 节点依赖；`IO/Components/TextTooltip.h:28-29` 注「`UIWindow.img/ToolTip` 无九切片、包内无 `UIToolTip.img`」。[观察]
- `docs/features/ui-v83-window-restore/evidence/w5-tooltip/coords.md:9-11,23` Q1 定案框体 = 程序描边 `TooltipBackdrop`；`UIWindow.img/ToolTip` 本体「**无 c/e/ne/n/nw/w/sw/s/se 任何子件**」；`notes/asset-node-inventory.md:11` 本包 20 个 UI 根里不存在 `UIToolTip.img`、原生 exe `UIToolTip` 0 命中。[观察]
- → **PLAN.md:20,34 不可再当作当前端引用路径引用**；Q2 清单 C14（`docs/features/feature-gap-inventory-2026-11/README.md:141`）属旧账，须在核心窗口面 D 决策点里列「回写」。[观察+推断]

### 9.2 节点字面量补充（§1 未列全的逐字引用）

- `IO/UIStateGame.cpp:447 nl::nx::UI["StatusBar3.img"]["KeyConfig"]`；`IO/UITypes/UIOptionMenu.cpp:51 nl::nx::UI["StatusBar3.img"]["OptionMenu"]`；`IO/UITypes/UIQuickslotConfig.cpp:24 nl::nx::UI["UIWindow.img"]["KeyConfig"]`、`:22 UiAssetResolve::window("KeyConfig")`。[观察]
- `IO/UITypes/UIMiniMap.cpp:57 nl::nx::UI["UIWindow2.img"]`、`:60 UIWindow2 = nl::nx::UI["UIWindow.img"]`；`IO/UITypes/UIDailyGift.cpp:82 nl::nx::UI["UIWindow2.img"]["Item"]["New"]["inventory"]`、`:84` 回退 `["UIWindow.img"]...`。[观察]
- `IO/UITypes/UISkillBook.cpp:175 nl::nx::UI["Basic.img"]["BtMacro"]`、`:248 window("Item")["New"]`、`:174` 注「宏编辑窗是独立根 `UIWindow.img/Macro`」、`:243` 注 `UIWindow.img/Item/New/Tab1=粉底选中、Tab0=灰底`、`:357` 注「`UIWindow.img/Skill/backgrnd` 逐像素对齐 xoff/yoff=0」、`:525` 注「`UIWindow.img/SkillMacro 207x289`」、`:1105` 注「`UIWindow.img/Skill/skillPoint` 在本包 WZ 中缺失」。[观察]
- `IO/UITypes/UIMiscShells.cpp:4705,4761 nl::nx::UI["UIWindow.img"]["SkillMacro"]`、`:4788-4789 diag_once("UISkillMacroEx", "UIWindow.img/SkillMacro missing")`、`:717 UiAssetResolve::window("EnchantSkill")`；`IO/UITypes/UIStatsInfo.cpp:107` 注「cover0/cover1 是现代 UI 件，本包 `UIWindow.img/Stat` 下不存在」、`:164 nl::nx::UI["Basic.img"]["BtHide"]`。[观察]
- `IO/UITypes/UIAlliance.cpp:44 nl::nx::UI["UIWindow.img"]["UserList"]["GuildUnion"]["backgrnd"]`；`IO/UITypes/UICharInfo.cpp:54,56 UiAssetResolve::window("UserInfo")`；`IO/UITypes/UIUserList.cpp:60,62 window("UserList")`；`IO/UITypes/UIMaker.cpp:47 nl::nx::UI["UIWindow.img"]["Maker"]["backgrnd"]`、`:55 window("Maker")`；`IO/UITypes/UICashGachapon.cpp:31 window("CashGachapon")`；`IO/UITypes/UICashShop.cpp:56 nl::nx::UI["CashShop.img"]`。[观察]
- `IO/Components/Icon.cpp:53 Charset(nl::nx::UI["Basic.img"]["ItemNo"], Charset::Alignment::LEFT)`；`IO/Components/Slider.cpp:43,46,58,69 nl::nx::UI["Basic.img"]["VScr5"] / ["VScr"]`；`IO/Components/MapleComboBox.cpp:37 nl::nx::UI["Basic.img"][combobox]`。[观察]

### 9.3 权威件表补充尺寸（§1 未列全）

- `asset-node-inventory.md:72-78` `StatusBar.img/base`：`backgrnd[817x71]`、`backgrnd2[570x71]`、`box[42x19]`、`chat[566x5]`、`chatTarget[81x20]`、`quickSlot[466x80]`；gauge 兄弟根 `bar[340x31]`；`number` 数字 5x7。`:80-86` `Basic.img` 71 根、`BtClose3` 不存在、只有 `VScr..VScr8`（无 VScr0/VScr9）、`ItemNo{0-9}[8x11]`。`:97-146` §1.5 三窗实测件表 + 关闭钮四类（D: `UIWindow.img/BtUIClose` 32x15 origin(0,0)）。[观察]
- `asset-node-inventory.md:22-25` `UIWindow.img/ToolTip` = `Equip{Can{0-9,beginner,bowman,magician,max,none,percent,pirate,reqDEX,reqINT,reqLEV,reqLUK,reqPOP,reqSTR,thief,warrior},Cannot{…},Dot{0,1,2},GrowthDisabled,GrowthEnabled,ItemCategory{1-10,12,21},Property{0-18},Speed{0-6},WeaponCategory{30-47子集}}`、`cash[23x5]`、`mesos[30x5]`；✗无九切片。`:43-53` Equip 槽位 x∈{5,38,71,104,137}、y=32+33k；`:55-58` Quest `BtAlert/BtDetail/BtGiveup/BtMarkNpc/BtNo/BtOK`（各 5 态）、`Gauge2{frame[79x8],gauge[1x4]}`、`notice{0-2}[73x24]`、`TimeQuest{AlarmClock,TimeBar}`。`:64-65` WorldMap `Border{0[60x33],1[1x33],2[7x32],3,4[7x1],5,7[7x18],6[1x18]}` + `title[1x1]`。[观察]

### 9.4 截图 / dump / layout 覆盖度精化

- `docs/features/ui-v83-window-restore/notes/raw-nx-dumps/` **9 个 IMG 节点树 dump**（比 §3.1 记的 6 个多 3 个）：`01_tooltip.txt:1-149`（`===== UIWindow.img/ToolTip`，无 c/e/ne/n/nw/w/sw/s/se，`:147/:149` origin `[vector]` 乱码）、`Equip.txt:1-81`、`Item.txt:1-134`、`Quest.txt:1-334`、`Skill.txt:1-95`、`Stat.txt:1-79`、`basic1.txt`、`slidemenu.txt`、`statusbar_base.txt`；另有 `w10-quest/_nx_quest_list.txt:1` 第二份 Quest 全量转储。`w{0-icon,1w3-hud,2-hud-buttons,4-chatbar,5-tooltip,6-stats,7-equip,8-item,9-skill,10-quest,11-map,12-nametag}/coords.md` **12 份逐窗坐标表**。`evidence/r27-oracle/{EquipTooltip.json,EquipTooltip_native_crop.png,frames/EquipTooltip_native_crop.png}` 装备 tooltip oracle 裁图+JSON。[观察]
- `**/layout*.json` 共 5 个均非核心窗口：`char-stats-ui-ps/ui/detail/v4/layout_cn.json:2-30`（canvas 1346x600→final 658x392 + 配套 `Character_Stats_Detail_V4.psd` + `assemble_v4.jsx` + `build_v4_componentized.jsx` + `components/*.png` 约 30 件 + `INDEX.txt`；658x392 与原生 Stat 208x347 差异大 ⇒ PS 自绘扩展面板 [推断]）、`map-search-ui/resources/layout.json`、`map-search-ui/ui/search_panel_backgrnd/v3_pipeline/layout.json`、`map-info-panel/resources/layout.json`、`UI/MapInfo/layout.json`。`**/ui/*/vN/` 356 路径 PS 产物（char-stats-ui-ps / hotkey-grid-3x3 v1,v2 / multi-resolution uisysopt v1 / daily-checkin window v1..v5 / addon-equipment window v1..v5）；`manifest.md` 仅 daily-checkin v3 与 addon-equipment v3（自造 UI，非核心窗）。[观察]
- `**/*img.xml` 仅 `docs/features/v83-hud-quest/evidence/StatusBar.img.xml`（非核心窗）；`**/*dump*.txt` 5 个均登录/软键盘系；仓库内 `**/*runtime*.json` 仅 4 个 `.runtimeconfig.json`（_tools/… net10.0-windows），**无 `*_runtime.json` IMG dump**（权威 dump 在仓库外 `E:\MXD\BeiDou-Client-xy\REA反编译\wz_dump\`）。`docs/features/ui-layout-v83-unify/evidence/*.png` 15 张实测尺寸：`acct_slot 220x60`、`btn_menu/btn_npt/btn_shop/btn_short 54x34`、`charInfo 183x115`、`charInfo1 183x151`、`gauge_bar/gauge_grad 340x31`、`help0 311x77`、`panel_mid 350x270`、`pass_slot 220x70`、`qs 466x80`、`qs08 151x80`。[观察]
- **缺失**：KeyConfig / WorldMap / UserInfo / Maker / CashShop / CashGachapon / Macro / SkillMacro / EnchantSkill / SkillEx / QuestAlarm 均无独立节点树 dump [推断]；`correct_shots/` 无配套 layout JSON、无 `manifest.md`。[观察]

### 9.5 orange-wz(IMG) 门禁的补充证据（结论不变：未接入 / 记阻塞）

- `mcp_connector_status` 共 1 条 `[json-photoshop] photoshop`（`json/none/stdio` · 状态未知 · 诊断「Host 尚未注册该 stdio Server 的工具…检查时间 2026-11-08T09:37:06.627Z」）；`mcp_connector_catalog(keyword=orange)` = 「共 0 个已上架连接器」。**无任何 orange-wz 连接**。[观察]
- `C:/Users/11036/.cursor/mcp.json:3-6` 声明 `"orange-wz": {"type":"http","url":"http://127.0.0.1:10012/mcp"}`、`:7-10 "ida-pro-mcp": "http://127.0.0.1:13337/mcp"`、`:11-18 photoshop(stdio npx)`；`netstat` 只读确认 `127.0.0.1:10012 LISTENING PID 28756` / `127.0.0.1:13337 LISTENING PID 31332`，**但属 Cursor 侧配置，不是本 DSH 会话的 MCP 连接**。[观察+推断]
- 仓库内 `**/mcp*.json` 仅 3 个 `docs/features/beidou-v17-parity/notes/evidence/{mcp_req_init.json,mcp_req_2.json,mcp_out_2.json}`（请求/响应样本，非连接配置）；`**/*.mcp.json` 与仓库内 `.cursor/mcp.json` → No files found。[观察]
- `docs/features/multi-resolution/evidence/rea-ijl15-identity.md:30`「orange-wz 对这批私服改版 `.img` 内部节点解析抛 `Index 16 out of bounds`，不可用 ⇒ 改用运行时 nlnx dump」；`docs/features/login-flow-angel-calibration-2026-11/notes/r01-prior-beidou-alignment-audit.md:169`「orange-wz IMG 节点树证据：未找到…均来自 `tools/nx_inspect.py`、运行时 [DBG-UI] 转储或 `wz_dump/*_runtime.json`（仓库外）」；`img-position-attributes.md:5` R1/R2 红线（运行时 `wz_dump/*_runtime.json` 为准，打包 `UI.wz` 有 Stat 175→208 漂移）。[观察]
- → **给 README 的校准提示**：红线 R1/R2 要求运行时 dump；仓库内 `raw-nx-dumps/*.txt` + `asset-node-inventory.md` 是 `tools/nx_inspect.py` 对 `UI.nx` 的实测，属「仓库内现成 IMG 节点 dump」最强证据；核心窗口面校准据此推进即可，无需等连库。[推断]

### 9.6 本节性质标注

- 本节全部为 [观察]（逐字引文件行号）或 [推断]（终判/覆盖度归类），无臆造；子代理声明全程只读、未改任何文件、未连库、未改 MCP 配置。
- 「`UIToolTip.img` 已清零」的结论与 §0.1 一致且证据链更完整（PLAN.md:20,34 修复前 → RC3/RC4 修复动作 → README.md:74 / CHECKLIST.md:70 定案 → 当前端源码实证），README §5 销案表可直接引用本节。
