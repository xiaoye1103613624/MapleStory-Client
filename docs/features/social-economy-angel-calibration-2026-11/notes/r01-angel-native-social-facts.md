# Angel 083 社交经济面 · 原生 IDB 一手语义（r01）

> 本文件是**主 agent 只读取证产物**：从 Angel(13339) 083/v83 带符号 IDB 的
> Hex-Rays 逐字伪代码抽取的「原生侧事实」，不含当前端源码结论、不含推断性改造建议。
> 当前端源码对照、销案、IMG 证据、六维判定见 r02–r05 与 `../README.md`。
> 性质：观察（逐字伪代码）/ 推断（已显式标注）。参照基准固定 **083/v83**，
> 全文禁止引 095 线符号/IDB 作依据（MEMORY.md critical）。

---

## 0. 一句话结论（7 条）

1. **Angel 与 BeiDou 是同一份 v83 二进制的不同符号状态**（本面 EA 交叉
   **752/752 全命中，0 缺失**），与登录面 327/328、HUD 面 547/548、核心窗口面
   439/441 四面一致。Angel 只补符号语义层 + 行为层，不补功能。
2. **v83 原生社交经济是「少数几个大单体窗 + 一批 Tab/Room 子类」**：
   `CUIUserList`（10 方法，5 页签全在一个窗内）、`CUIMessenger`（26 方法）、
   `CTrunkDlg`（22）、`CTradingRoomDlg`（17）、`CMiniRoomBaseDlg`（15）、
   `CUIGuildBBS`（17）、`CAdminShopDlg`（14）、`CTabFriend`（12）……
   而非「一个系统一个独立 UI 类」。
3. **★ 原生 083 的 G 键「家族」= 公会，确实落在 `CUIUserList` 单体窗第 3 页签**
   （tab index 2 / `2030–2041` 控件组），与 MEMORY.md critical 硬约束逐字互证。
   5 页签顺序 = 好友 / 组员 / **家族(公会)** / 家族联盟 / 拒绝列表。
4. **F 键「学院」= FAMILY 是另一套类**（`CUIFamily` 3 + `CUIFamilyChart` 8 +
   `UFamilyItem` 1 + `CUIFriendGroup` 2），与公会 `CTabGuild`/`CTabGuildAlliance`
   是**不同类树** → 083 原生「学院 ≠ 家族」在符号层再获铁证。
5. `CUIMessenger::OnCreate` 逐字发 `COutPacket(122)` + `Encode1(0)` + `Encode4(a2)`
   → C→S `MESSENGER=122`，与既有 `r22-messenger-protocol.md` 一致；
   `OnPacket` 是 9 分支状态机（case 0/1/2/4/5/6/7/8）。
6. 资源节点走 **StringPool 编译期常量索引**：本面 `SP_` 全集 **347 个**
   （84 个 `UI_UIWINDOWIMG_*`），IMG 层 MCP 未接入，节点路径以仓库内 `.nx`
   引用 + `tools/nx_inspect.py` dump + 正确端截图为替代证据（三面一致）。
7. 判定红线沿用前三面：**「校准 ≠ 改回原生」**。原生单体窗结构与当前端拆分
   形态、以及 s9 协议编号/字段差异，一律列为「须裁决」而非直接缺陷。

---

## 1. 取证基线与门禁

### 1.1 端点（每次先跑 server_health 读 idb_path，端口会变）

| 端口 | idb_path | module | strings | 用途 |
|---|---|---|---|---|
| 13339 | `E:\MXD\反编译IDA\v83\`Angel.i64` | `` `Angel.exe `` | 1281 | 本轮权威（带符号） |
| 13337 | `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64` | `BeiDou.exe` | 3889 | EA 交叉验证（无符号） |
| 13338 | headless `Maplestory95.i64` | — | — | 095 线，本任务**不用** |

两端 `auto_analysis_ready=true` / `hexrays_ready=true` / 66 工具齐全
（`decompile/lookup_funcs/xrefs_to/py_eval/get_bytes/...`）。

### 1.2 三联门禁（技能 `xiaoye-maple-story-dev`）

- **REA**：历史轮已用 `D:\software\nodejs-24\rea.cmd` + `REA_ANALYSIS_PROVIDER=ida`
  挂 13337 完成；本轮 IDA MCP 直连复用。
- **IDA**：本轮 ✅ —— 端点确认 + EA 索引 + 277 函数逐字反编译 + 752/752 交叉。
- **IMG（orange-wz）**：⚠️ **阻塞** —— 本 DSH 会话 `mcp_connector_status` 仅
  `json-photoshop`；`C:\Users\11036\.cursor\mcp.json:3-6` 声明
  `orange-wz(http://127.0.0.1:10012/mcp)` 但 Host 未注册；
  `docs/features/login-flow-regression-fix/notes/img-evidence-audit.md:7`
  记该 MCP 对这批私服改版 img 有 `Index 16 out of bounds` 解析缺陷。
  替代链 = 仓库内 `.nx` 节点引用 + `tools/nx_inspect.py` 运行时 dump +
  `C:\Users\11036\Desktop\正确客户端截图\`（学院.png / 好友、组员、家族、
  家族联盟、拒绝列表.png 等）。

### 1.3 采集脚本与产物（均在 `docs/features/social-economy-angel-calibration-2026-11/evidence/`）

| 脚本 | 产物 | 读数 |
|---|---|---|
| `tools/ida_angel_social_ea.py` | `angel-social-ea.json` | **752 具名函数 / 56 类** |
| `tools/ida_angel_social_decompile.py` | `angel-social-decompile.json` | 277 函数逐字伪代码 |
| `tools/ida_angel_social_normalize.py` | `angel-social-decompile-full.json` / `-truncated.json` | `total 277 trunc_fixed 5 empty 0` |
| `tools/ida_angel_social_crosscheck.py` | `angel-beidou-social-ea-crosscheck.json` | **752/752 同 EA / 0 缺失** |
| `tools/ida_angel_social_facts.py` | `angel-social-facts.md` | **4716 行 / 235,841 B** |

脚本模板 = 前三面同一套（IDA 侧 `py_eval` + `new_locals=True` 写本地文件避开
MCP stdout 截断；大函数走 `GET http://127.0.0.1:13339/output/<uuid>.json`
补全；`extract_code()` 递归解嵌套 JSON 字符串取 `.get("code")`）。

### 1.4 EA 交叉验证逐字结论

```
Angel social named funcs to crosscheck: 752
BeiDou lookup rows: 752
{"angel_funcs": 752, "beidou_present": 752, "beidou_sub_named": 747,
 "beidou_real_named": 5, "beidou_missing": 0, "missing_eas": [],
 "conclusion": "same binary layout (752/752 EAs identical; differences are
                IDA analysis coverage, not code)"}
```

样本（Angel 具名 ↔ BeiDou 同 EA 无名）：
`CAdminShopDlg::OnPacket 0x423db7 → sub_423DB7(0x533)`、
`SetAdminShopDlg 0x424745 → sub_424745(0x44c)`、
`ResetInfo 0x424c44 → sub_424C44(0x2a)`、
`OnCreate 0x424c6e → sub_424C6E(0xc76)`、
`OnMouseButton 0x425d73 → sub_425D73(0xd5)`、
`SetUserItems 0x426215 → sub_426215(0x201)`、
`SetNPC 0x426636 → sub_426636(0x54f)`。

---

## 2. 类索引（56 类 / 752 具名函数）

按 `tools/_angel_class_index_full.json`（`classes` = list of
`[类名, 方法数, [方法名...]]`）社交经济关键词筛得 52 类 / 354 方法；
EA 索引脚本 `exact` 集合补入共享基类后得 **56 类 / 752 函数**。per_class 全表：

```
CCashShop 66        CWvsContext 197    CUserLocal 120    CUIMessenger 26
CTrunkDlg 22        CUserRemote 21     CTradingRoomDlg 17 CUIGuildBBS 17
CMiniRoomBaseDlg 15 CAdminShopDlg 14   CTabFriend 12     CChatBalloon 11
CTabParty 10        CPersonalShopDlg 10 CUIUserList 10   CShopDlg 9
CUIFamilyChart 8    CUIRecommendWorld 7 CUIAdminShopWishList 7
TabPartySearch 7    CTabGuild 7        CTabGuildAlliance 7 CFriend 6
CCashTradingRoomDlg 5 CChatHelper 5    CUIAdminShopWishListSearchResult 5
CField_Wedding 4    CBlackListDlg 3    CMapleTVMan 3     CUIFamily 3
CWndGuildGrade 3    CWndAllianceGrade 3 CTabBlackList 3   UCS_COMMODITY 2
CField_GuildBoss 2  CUIFriendGroup 2   CUIShopScanResult 2
CWvsCommodityEx 1   PARTYDATA 1        GW_Friend 1       CEmployeePool 1
CField_PartyRaid 1  CField_PartyRaidBoss 1 CField_PartyRaidResult 1
CMemoryGameDlg 1    CParcelDlg 1       CUIQuestDelivery 1 CUIMapleTV 1
UFamilyItem 1       CEmployee 1        CUIPartyHP 1      CUIPartySearch 1
PARTYSERACH_SETTING 1 StringPool 6     CWnd 32           CWndMan 27
```

`angel-social-facts.md` §2 对其中 **49 类**做了逐函数关键行抽取
（`grep -n '^### '` 行号 359–4907），§3 是坐标/尺寸/按钮 id 候选行（:5115 起）。

---

## 3. ★ CUIUserList —— 单体窗 5 页签（本面最硬增量）

### 3.1 10 方法（逐字 EA + 字节数）

`CreateAllianceButton 0x…(4633B)` / `DestroyAllianceButton(748)` /
`DestroyGuildButton(744)` / `Draw 0x91bec3 (35089B)` /
`OnCreate (59169B)` / `OnMouseButton(5323)` / `OnMouseMove(3037)` /
`ResetInfo(1429)` / `SetButton(3494)` / `SetScrollBar(1707)`。

### 3.2 5 页签分支（`CUIUserList::Draw @ 0x91bec3` 逐字）

```
v8 = *(this + 369);
if ( v8 != 0 ) { v9 = v8 - 1;
  if ( v9 != 0 ) { v10 = v9 - 1;
    if ( v10 != 0 ) { v11 = v10 - 1;
      if ( v11 != 0 ) { if ( v11 == 1 ) ... } } } }
```
→ tab index **0..4 共 5 页签**（`this+369` = 当前页签）。列表行高 **18 px**
（`a3: 18 * vN + 43 / 572 / 110 / 116 / ...`，列表 x = `a2: 281` / `a2: 312`）。
`CUIUserList::SetButton` 逐字先 5 次
`(*(*(*vN + 4) + 36))(a1: ..., a2: 0)` 全隐藏，再按 `v14 = *(this + 369)`
把对应组置 `a2: 1` → 页签切换即控件组显隐。

### 3.3 控件 id 分组（`CUIUserList::OnCreate` 逐字 `CreateCtrl`）

底板 `SP_2090_UI_UIWINDOWIMG_USERLIST_BACKGRND`，页签条
`(*(**(this+366)+8))(a1, a2:this, a3:2001, a4:1, a5:6, a6:23, a7:270, a8:0)`
+ `SP_2670/2671_..._TAB_ENABLED/DISABLED` + `SetScrollBar(this, 1)`。

| 页签 | ctrl id 段 | 按钮（id, x, y）逐字（a4=x, a5=y） |
|---|---|---|
| 好友 | **2010–2020** | BtAddFriend(5,81) / BtAddGroup(72,81) / BtGroupWhisper(8,341) / BtShowOnline(232,81) / BtMod(8,362) / BtDelete(49,362) / BtChat(99,341) / BtWhisper(254,341) / BtMessage(254,362) / BtParty(99,362) / BtBlock(182,362) |
| 组员 | **2200–2208** | BtCreate(13,355) / BtInvite(72,365) / BtKick(190,344) / BtWithdraw(190,364) / BtWhisper(249,345) / BtChat(249,365) / BtChangeBoss(131,365) / BtHP(131,345) / BtSearch(72,345) |
| **家族(公会) tab index 2** | **2030–2041** | BtInvite(9,316) / BtKick(159,336) / BtWithdraw(80,316) / BtUp(9,336) / BtDown(80,336) / BtWhere(159,316) / BtWhisper(238,316) / BtNotice(287,80) / BtChat(238,336) / BtInfo(238,363) / BtPartyInvite(159,363) / BtGuildBBS(80,363) |
| 家族联盟 | **2050 / 2058 / 2059 / 2060** | `CreateAllianceButton` 动态 id `a3: 2*a2+2085` / `2*a2+2086`；含 (9,316)/(80,)/(159,316)/(238,)/(288,80)/(159,363) 组 |
| 拒绝列表 | **2150 / 2151** | BtAdd(60,355) / BtDelete（`SP_2165/2166_USERLIST_BLACKLIST_BTADD/DELETE`） |

`Draw` 内 `DestroyGuildButton(this, 0/2/3)` + `DestroyGuildButton(this, 1/4/5)`
→ 公会页按权限动态销毁部分按钮（原生已有分级显隐）。

### 3.4 术语定案（与 MEMORY.md critical 互证）

- **G 键 = 家族 = 公会**：083 原生把公会做进 `CUIUserList` 第 3 页签
  （上表 2030–2041），**不是独立浮窗**。
- 5 页签 = 好友 / 组员 / 家族 / 家族联盟 / 拒绝列表，与
  `C:\Users\11036\Desktop\正确客户端截图\好友、组员、家族、家族联盟、拒绝列表.png`
  文件名逐项对应。
- `SP_2051..2057_UI_UIWINDOWIMG_USERINFO_BACKGRND..7` 是 UserInfo 7 张底板
  （与公会 tab 的 BtInfo 关联），非公会窗本体。

---

## 4. CUIMessenger —— 26 方法 / C→S 122 / 9 分支状态机

- `OnButtonClicked (749B)` 逐字 5 case：
  `0x3E8(1000)→TryDelete` / `0x3E9(1001)`+`0x3EA(1002)→SetState(a2: v2%3)` /
  `0x3EB(1003)→ProcessChat` / `0x3EC(1004)→SendClaim`。
- `OnCreate (11723B)`：`SetLayer` + `SetCtrl` + Arial 字体
  （`SP_1410_CANVASFONT` / `SP_5527_ARIAL`）；
  帮助文案 `SP_800___MAPLE_MESSENGER_HELP__` /
  `SP_801_INVITE___INVITE_CHARACTER_NAME` / `SP_802_END___Q` /
  `SP_1859__________________________` 逐条 `AddChatText`；
  `OnSelfEnter(this,0,0)` 后逐字发：
  ```
  COutPacket::COutPacket(a1: 122);
  COutPacket::Encode1(this: &v61, a2: 0);
  COutPacket::Encode4(this: &v61, a2);
  CClientSocket::SendPacket(this: dword_BE7914, a2: &v61);
  ```
  → **C→S `MESSENGER=122` Open，`Encode1(0)`=mode 0，与
  `r22-messenger-protocol.md`（0x00 Open / 0x02 Leave / 0x03 Invite /
  0x05 Decline / 0x06 Chat）口径一致**。
- `OnPacket (1212B)`：`OnInvite(a1)` + `switch(v1)`
  case 0/1/2/4 → `sub_851293`、case 5 → `sub_851387`、
  case 6 → `OnChat`、case 7 → `sub_85194F`、case 8 → `sub_8519F2`。
- 资源节点：`SP_1858_UI_UIWINDOWIMG_MESSENGER_BLINK` /
  `SP_1860_..._CHATBALLOON` / `SP_1861_..._BACKGRND` /
  `SP_1867_UI_UIWINDOW_MESSENGER_NAMEBAR_D` / `SP_800/801/802/1859`。

---

## 5. 其它关键类一手语义（facts §2 精要）

- **学院 FAMILY 独立类树**：`CUIFamily` 3 / `CUIFamilyChart` 8 /
  `UFamilyItem` 1 / `CUIFriendGroup` 2。与公会 `CTabGuild` 7 /
  `CTabGuildAlliance` 7 / `CWndGuildGrade` 3 / `CWndAllianceGrade` 3 /
  `CUIGuildBBS` 17 **类树分离** → 「学院 ≠ 家族」符号层铁证。
- **Tab 子类**（`CUIUserList` 内部数据视图）：`CTabFriend` 12 /
  `CTabParty` 10 / `CTabGuild` 7 / `CTabGuildAlliance` 7 /
  `CTabBlackList` 3 / `TabPartySearch` 7 / `CFriend` 6 / `GW_Friend` 1 /
  `CBlackListDlg` 3。
- **组队扩展**：`PARTYDATA` 1 / `PARTYSERACH_SETTING` 1 /
  `CUIPartyHP` 1 / `CUIPartySearch` 1（StartSearch）/ `CField_PartyRaid*` 3 /
  `CField_GuildBoss` 2。
- **仓库 / 交易 / 商店 / 迷你房**：`CTrunkDlg` 22（`SP_2859/2860_TRUNK_TAB_*`、
  `SP_1822..1825_TRUNK_EN/BTGET/BTPUT/BTSORT`）、`CTradingRoomDlg` 17
  （`SP_1818_TRADINGROOM_BTTRADE`）、`CMiniRoomBaseDlg` 15、
  `CPersonalShopDlg` 10、`CShopDlg` 9（`SP_1542..1548_SHOP_*`）、
  `CAdminShopDlg` 14、`CCashTradingRoomDlg` 5、
  `CUIAdminShopWishList` 7 / `...SearchResult` 5 / `CUIShopScanResult` 2 /
  `CEmployeePool` 1 / `CEmployee` 1 / `CParcelDlg` 1 /
  `UCS_COMMODITY` 2 / `CWvsCommodityEx` 1。
- **聊天泡 / 帮助**：`CChatBalloon` 3（含 `SP_1388..1425_CHATBALLOONIMG_*`
  九切片 NW/N/NE/E/W/C/SW/S/SE/ARROW/B/BA/CNUM/MNUM/PROGRESS/OMOK/MINIROOM）、
  `CChatHelper` 4（含 `TryChat` 脏词/刷屏/重复句，HUD 面已记整族）。
- **婚礼 / 地图 / 活动**：`CField_Wedding` 4、`CUIMapleTV` 1 /
  `CMapleTVMan` 3、`CMemoryGameDlg` 1、`CUIQuestDelivery` 1。
- **CCashShop** 精选 10 方法（全量 66 方法见 `angel-social-ea.json`）：
  `IsUsableItemGift` / `OnBuyPackage` / `OnGiftMateInfoResult` 等
  （核心窗口面已销案「当前端 CashShop 不是 stub 窗」）。

---

## 6. SP_ 资源节点（347 个 / 84 个 `UI_UIWINDOWIMG_*`）

按 owning class 反查全集见 `evidence/angel-social-facts.md` §1。关键分组：

| 组 | 节点 |
|---|---|
| UserList | `SP_2090 BACKGRND` / `SP_2097..2109 FRIEND_BT*`(11) / `SP_2110..2117+4044 PARTY_BT*`(9) / `SP_2144..2153+3342+3741 GUILD_GUILDINFO_BT*`(13) / `SP_2165/2166 BLACKLIST_BTADD/DELETE` / `SP_2670/2671 TAB_ENABLED/DISABLED` |
| Trunk / 交易 / 商店 | `SP_2859/2860 TRUNK_TAB_*` / `SP_1822..1825 TRUNK_*` / `SP_1818 TRADINGROOM_BTTRADE` / `SP_1542..1548 SHOP_*` |
| Messenger | `SP_1858..1868 MESSENGER_*` |
| ChatBalloon | `SP_1388..1425 CHATBALLOONIMG_*`（九切片 20+） |
| UserInfo | `SP_2051..2057 USERINFO_BACKGRND..7` |
| 共享 | `SP_145__INVITE` / `SP_1260/1261/1262 UI_BASICIMG_BTCLAIM/BTMIN/BTMAX` / `SP_1410_CANVASFONT` / `SP_5527_ARIAL` / `SP_1253 UI_BASICIMG_ITEMNO` / `SP_2081/2082_D_D` |

Angel 明文 WZ 路径有限（strings_cache 1281），绝大多数节点走 StringPool
编译期索引 → **IMG 节点路径须以仓库内 `.nx` 引用 + 运行时 dump + 正确端截图为准**。

---

## 7. 与 MEMORY.md 083 术语硬约束互证

| 约束 | Angel 083 证据 | 结论 |
|---|---|---|
| G 键 = 家族 = 公会，入口 UIUserList 第 3 页签 | `CUIUserList::Draw` tab index 2 ↔ 2030–2041 公会按钮组 | ✅ 逐字互证 |
| 5 页签 好友/组员/家族/家族联盟/拒绝列表 | `Draw` 5 层分支 + `OnCreate` 5 组控件 | ✅ |
| F 键 = 学院 FAMILY ≠ 公会 | `CUIFamily/CUIFamilyChart/UFamilyItem` 与 `CTabGuild*` 类树分离 | ✅ |
| 只 095 才有的 Academy / BossParty 面板「不做」 | Angel 083 无 `Academy`/`ExpeditionIntermediary` 类 | ✅ 083 侧无此需求 |

---

## 8. 与前三面一致性

| 面 | EA 交叉 | 拆分模式 | s9 红线 | IMG |
|---|---|---|---|---|
| 登录 | 327/328 | 单体 `CLogin` → 状态机拆分 | 083↔s9 字节/编号 | 阻塞 |
| HUD | 547/548 | 单体 `CUIStatusBar` 41 → 10+2 类 | 「校准≠改回原生」 | 阻塞 |
| 核心窗口 | 439/441 | 62 类 → 12 窗 + 20+ 壳 + 5 组件 | 同上 | 阻塞 |
| **社交经济** | **752/752** | 单体 `CUIUserList` 5 页签 / `CUIMessenger` 26 → 当前端多窗 | 同上 | 阻塞 |

四面共同结论：**Angel 是同一份 v83 二进制的不同符号状态，只补符号/行为层；
排版层当前端已用无符号 IDB + 正确端截图对齐过；真缺口收敛在「业务规则、
显式预留、文档一致性、少量协议差异」，不是「缺窗口/缺排版」**。

---

## 9. 限制与未决

1. `read_image` 对本环境 PNG 返回 `model "hy4free" does not declare image input`
   → 未做像素级视觉复核（三面一致限制）。
2. `CUIUserList::OnCreate (59169B)` / `Draw (35089B)` 等 5 个大函数经
   `/output/` 补全为完整伪代码，但**未逐行人工核完**（§3 只抽了
   CreateCtrl/坐标/页签分支/按钮 id 关键行）。
3. 277 函数是 `SOCIAL_TARGETS` 精选（49 类），非全部 752 具名函数。
4. IMG 层 orange-wz MCP 未接入 Host，节点树证据依赖替代链。
5. 与当前端源码的逐字销案、旧文档四态、六维判定 → 见 r02–r05 + README；
   跨端协议编号/字段冲突一律标「s9 适配，须裁决，不得盲改」。

---

*性质：本文件为只读取证归档；标注[观察]者为 Hex-Rays 逐字，[推断]者已显式写明。
未修改任何产品源码、未构建、未实机、未提交。*
