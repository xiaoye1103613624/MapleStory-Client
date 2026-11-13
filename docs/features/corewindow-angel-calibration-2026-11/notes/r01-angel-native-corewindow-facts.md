# Angel 083 核心窗口面 · 原生态一手事实（观察层）

> 来源：13339 = `E:\MXD\反编译IDA\v83\`Angel.i64`（v83 GMS 系，input `C:\Users\Admin\Desktop\v83 Server\`Angel.exe`）。
> 证据：`evidence/angel-corewindow-ea.json`（441 具名函数 / 62 类）、`evidence/angel-corewindow-decompile-full.json`（296 函数逐字 Hex-Rays，7 个超大函数经 `/output/<uuid>.json` 补全并归一化）、`evidence/angel-corewindow-facts.md`（自动抽取）、`evidence/angel-beidou-corewindow-ea-crosscheck.json`（EA 交叉验证）。
> 采集脚本：`tools/ida_angel_corewindow_ea.py` / `ida_angel_corewindow_decompile.py` / `ida_angel_corewindow_normalize.py` / `ida_angel_corewindow_facts.py` / `ida_angel_corewindow_crosscheck.py`。
> 本文只记**观察**（逐字伪代码 / 数值 / 符号），推断与对照判定放 README，跨端差异一律标「s9 适配须裁决」。

## 0. 一句话结论（本面事实）

1. **EA 交叉验证 439/441 同 EA**：Angel 441 个核心窗口具名函数，BeiDou(13337) 同 EA 存在 439 个（全部 `sub_*` 无名），仅 `0x6cec47`、`0x8aa4e4` 两个因 IDA 分析覆盖缺失 → 与登录面（327/328）、HUD 面（547/548）结论一致：**同一份 v83 二进制的不同符号状态，Angel 只补符号语义层 + 行为层**。
2. 该面 Angel 索引到 **62 类 441 方法**；本次反编译其中 **296 函数全部 OK**（含 7 个 >50 KB 超大函数补全）。
3. `SP_` StringPool 资源节点 **401 个**，其中 91 个 `UI_UIWINDOWIMG_*`（核心窗口专用）。绝大多数资源走编译期字符串池索引而非明文字符串（解释了 Angel `strings_cache_size=1281` < BeiDou `3889`）。
4. 原生核心窗口是**一批独立 `CUI*` 类**，`CUIToolTip` 单体 **26 方法**、`CWorldMapDlg` **18 方法**、`CUIQuestInfo` **15 方法**、`CCashShop` **14 方法**、`CUIUserInfo` **13 方法**、`CUIItem` **12 方法**、`CUIStat` **8 方法**、`CUIMacroSys`/`CUIMacroSysEx`/`CMacroSysMan` 各 **8 方法**、`CUIKeyConfig` **7 方法**、`CUISkill` **5 方法**、`CUISkillEx` 3、`CUIEnchantDlg` 2、`CUICashItemGachapon` 3。

## 1. 取证基线与门禁状态

| 项 | 值 |
|---|---|
| 端点 13339 | `` `Angel.exe `` @ `E:\MXD\反编译IDA\v83\`Angel.i64`，`auto_analysis_ready/hexrays_ready/strings_cache_ready=true`，`strings_cache_size=1281`，imagebase `0x400000` |
| 端点 13337 | `BeiDou.exe` @ `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64`，strings 3889（仅做 EA 交叉验证） |
| REA→IDA→IMG 三联 | IDA 层已直接取证；**orange-wz(IMG) MCP 未接入本会话**（`mcp_connector_status` 仅 `json-photoshop`；`~/.cursor/mcp.json` 有 orange-wz 但 Host 未注册；`login-flow-regression-fix/notes/img-evidence-audit.md:7` 记该解析器对私服改版 img 有已知缺陷）→ IMG 层记阻塞，资源证据以 `.nx` 节点引用 + `StatusBar.img.xml` 类 dump + 正确端截图为准 |
| 295 vs 296 | 反编译目标来自 `COREWINDOW_TARGETS` 精选（44 类，含 `CCashShop`/`CItemInfo`/`CQuestMan` 部分方法），非全部 441 |
| 095 线 | 全程未引；MEMORY.md 硬约束「参照基准固定 083/v83」 |

## 2. 关键类逐字语义（★ = 当前端须核对的硬锚点）

### 2.1 ★ `CUIStat::OnButtonClicked @ 0x8c5245`（AP 分配按钮 10 分支）

逐字 case 表：

| ctrl id | 动作 | 参数 |
|---|---|---|
| 2000 | `CWvsContext::SendAbilityUpRequest(*dword_BE7918, v2)` | `v2 = 2048` (0x800) |
| 2001 | 同上 | `v2 = 0x2000` (8192) |
| 2002 | 同上 | `v2 = 64` (0x40) |
| 2003 | 同上 | `v2 = 128` (0x80) |
| 2004 | 同上 | `v2 = 256` (0x100) |
| 2005 | 同上 | `v2 = 512` (0x200) |
| 2006 | `CUIStat::ToggleDetail(this)` | — |
| 2007 | `CUIStat::AutoApUp(1)` | — |
| 2008 | `CUIStat::AutoApUp(1)` | 与 2007 合流 |
| 2009 | `CUIStat::AutoApUp(0)` | — |
| default | `CUIWnd::OnButtonClicked(a2)` | 转发父 |

→ 6 个能力掩码 `0x800/0x2000/0x40/0x80/0x100/0x200` + 详情开关 + 自动分配（有/无参数两种）。`CUIStat::OnCreate` 用 `SP_1943..1946_UI_UIWINDOWIMG_STAT_BTAPUP/BTAUTO/BTAUTO1/BTAUTO2`、`SP_2064_..._BTDETAIL`、`SP_5153_..._BASICSTAT`、`SP_1955_..._JOB`、`SP_1947_..._DISABLED_`。

### 2.2 ★ `CUIMacroSys::GetIndexByPos @ (见 ea.json)`（宏窗网格命中）

逐字：`v7=65; while(v7-32 > a3 || a3 > v7){ v7+=39; ++v6; if(v7>=260) goto LABEL_7; } *a5=v6; ... v9=44; while(v9-32 > a2 || a2 > v9){ v9+=37; ++v8; if(v9>=155) goto LABEL_14; } *a6=v8; ... if(a4!=0 && a2>=145 && a2<=177) *a6=3;`
→ **每格 32、行 Y 起点 65 步进 39（5 行，到 260）、列 X 起点 44 步进 37（3 列，到 155）**；`a4` 拖动态下 X∈[145,177] 强制第 3 列。`CUIMacroSys::OnCreate` 用 `SP_4154_UI_UIWINDOWIMG_SKILLMACRO_BACKGRND`、`SP_1257_UI_BASICIMG_BTOK2`、`SP_4343_SAVING_THE_SKILL_NAME`。

### 2.3 ★ `CUIMacroSys::OnButtonClicked`（宏名保存含脏词过滤）

逐字：`if(a2==3000 && *(this+361)!=-1){ CCtrlEdit::GetText(*(this+34), &a2); Buffer=ZXString<char>::GetBuffer(1024,1); v14=CCurseProcess::ProcessString(Buffer,0,1); ... CMacroSysMan::SetMacroInfo(v5, nMacroIdx:v11, sName:v12, bShout:v13); ... v8=SP_3296_IT_IS_SAVED; } else { v8=SP_272_CURSING_IS_NOT_ALLOWED; } ... CUtilDlg::Notice(...)`。
→ **宏名保存走 `CCurseProcess::ProcessString` 脏词过滤**（与 HUD 面 `CChatHelper::TryChat` 同一个脏词器）。

### 2.4 ★ `CUIKeyConfig::OnButtonClicked`（键位设置 5 分支）

逐字：`CUIWnd::OnButtonClicked(a2)` 后：`a2==1 → CFuncKeyMappedMan::SaveFuncKeyMap + CQuickslotKeyMappedMan::SaveQuickslotKeyMap`；`a2==2 → CWnd::InvalidateRect(CUIStatusBar::ms_pInstance,0)`；`a2==2000 → DoModal(CNoticeDlg(0))==1 则 DefaultFuncKeyMap + DefaultQuickslotKeyMap + CUIStatusBar::ReDrawQuickslot`；`a2==2001 → DoModal(CNoticeDlg(1))==1 则 memset(dword_BED5A0+4, 0, 0x1BD=445)`；`a2==2002 → sub_836C9B(this)`；末尾 `CWvsContext::UI_Close(*dword_BE7918, 5)`。
→ 「恢复默认键位」会**连带重绘状态栏快捷栏**（`CUIStatusBar::ReDrawQuickslot`），是跨窗联动。

### 2.5 ★ `CUIKeyConfig::GetShortCutIndexByPos`（键位格命中 32×32）

逐字：`v4=dword_BE27E4; while(1){ v5=*v4; v7[0]=*(v4-1); v7[2]=v7[0]+32; v7[1]=v5; v7[3]=v5+32; if(dword_BF0484(v7,a2,a3)!=0) break; ++v8; v4+=2; if(v4>=dword_BE2C6C) return -1; if(dword_BE27E0[2*v8]==0 && dword_BE27E4[2*v8]==0) return -1; } switch(v8){ case '6': return 42; case 'Y': return 29; case 'Z': return 56; default: return v8; }`
→ 与 HUD 面 `CUIStatusBar::GetShortCutIndexByPos` 同构（32×32 矩形命中表），但坐标表基址不同（`dword_BE27E4..BE2C6C` vs `dword_BE2DB4..BE2DF4`），且有 `'6'/'Y'/'Z'` 三个特殊键位返回 42/29/56。

### 2.6 ★ `CUISkill::OnSkillLevelUpButton`（技能升级前置校验）

逐字：`if(CWvsContext::CanSendExclRequest(500,0)!=0)` → 取 `GetCharacterData`，条件 `(*(*(this+366)+60)==0 || _ZtlSecureFuse<short>(v4+137, *(v4+141))>0) && (*(*(this+366)+60)!=0 || get_novice_skill_point(cd:v4)>0)`；错误文案 `SP_3509_YOURE_LACKING_LEVEL_1_OR_2_SKILLS` / `SP_3510_YOURE_LACKING_LEVEL_1_SKILLS` / `SP_3511_YOU_ARE_LACKING_IN_EITHER_1ST_2ND_OR_3RD_LEVEL_SKILLS`（case 0/1/2）。
→ **技能点升级有「前置技能点不足」三档拦截 + `CanSendExclRequest(500ms)` 独占请求门闩**。
`CUISkill::OnCreate` 用 `SP_1923_..._SKILL_BTSPUP`、`SP_1924/1925_..._SKILL_SKILL0/1`、`SP_1926_..._SKILL_RECOMMEND_0`、`SP_2660_..._SKILL_LINE`、`SP_4341_YOU_MAY_OPEN_CLOSE_YOUR_OWN_SKILL_WINDOW`、`SP_4344_UI_BASICIMG_BTMACRO`。

### 2.7 ★ `CUIItem::OnChildNotify`（背包 tab / 排序 / 扩展）

逐字：`a2==2000 && a3==500 → OnTabChanged(a4)`；`a2==2001 && a3∈[0x12C,0x130] → CUIToolTip::ClearToolTip(this+116) + dword_BF0EB4[*(this+377)]=v4 + InvalidateRect + SetLastestGetItemEffect`；`a3==101 && a2!=2003 → switch(a2)`：`case 0x7D4(2004) → SP_698_ITEM_SORT + sub_81C2D1`（BtSort 排序）；`case 0x7D5(2005) → SP_4501_EXPAND_INVENTORY + sub_81C2D1`（扩展背包）。
→ 背包排序（`BtSort`/`BtGather`）与扩展背包是**两个独立 ctrl id 2004/2005**；`CUIItem::OnCreate` 用 `SP_1848_..._ITEM_BTCOIN`、`SP_1851_..._ITEM_BTGATHER`、`SP_1852_..._ITEM_BTSORT`、`SP_2662/2663_..._ITEM_TAB_ENABLED/DISABLED`、`SP_4498_..._ITEM_FULLBACKGRND`、`SP_4499_..._ITEM_BACKGRND`、`SP_4500_..._ITEM_BTCASHSHOP`、`SP_4529_..._ITEM_DISABLED`、`SP_5580/5581_..._ITEM_BTFULL/BTSMALL`、`SP_1253_UI_BASICIMG_ITEMNO`。

### 2.8 ★ `CUIToolTip` 26 方法（单体 tooltip 引擎）

完整方法集：`0ItemToolTipParam / 1ItemToolTipParam / AddInfoEx / ClearToolTip / DrawItemIcon / DrawToolTip_Equip / GetFontByType / MakeLayer / PrintValue / SetBasicInfo / SetToolTip_Bundle / SetToolTip_Equip / SetToolTip_Equip2 / SetToolTip_EquipExt / SetToolTip_Equip_Basic / SetToolTip_MacroSysSkill / SetToolTip_Package / SetToolTip_Pet / SetToolTip_Ring / SetToolTip_Skill / SetToolTip_SlotInc / SetToolTip_String / SetToolTip_String2 / SetToolTip_String_MultiLine / SetToolTip_WorldMap / ShowItemToolTip`。
`GetFontByType` case 1–13+ 按字体类型取 `this+265..278` 的 IWzFont（**13 档以上字体**）。`SetToolTip_String` 用 `SetBasicInfo(this, 1, v8+8, v19, -1)`。
→ 原生 tooltip 是**按对象类型分派的单体引擎**（装备/ bundled/ Package/ 宠物/ 戒指/ 技能/ 槽位扩展/ 世界地图/ 宏技能/ 多行文本各一支），非按窗口各画各的。

### 2.9 ★ `CWorldMapDlg` 18 方法（世界地图）

`CreateWorldMapDlg` 逐字 `CDialog::CreateDlg(this, a2:67, a3:0, a4:666, a5:524, a6:10, a7:1, a8:0)` → **666×524、z=10、bScreenCoord=1**。
方法集：`CheckLinkInfo / CheckSpotInfo / ClearToolTip / CreateWorldMapDlg / Draw / HitTest / LoadMapList / MakeBorder / OnButtonClicked / OnCreate / OnDestroy / OnKey / OnMouseButton / OnMouseEnter / OnMouseMove / SetWorldMap / SetWorldMapDeeper / SetWorldMapShallower`。
`OnButtonClicked` 仅 `a2==1/2 → (*(*this+52))(1/2)`；`OnCreate` 用 `SP_2289_UI_UIWINDOW_WORLDMAP_TITLE`、`SP_1240_WORLDMAPOPEN`、`SP_5577_UI_BASICIMG_BTCLOSE`；`MakeBorder` 用 `SP_2281..2288_UI_UIWINDOWIMG_WORLDMAP_BORDER_7..0`（八片边框）。
→ **有 `SetWorldMapDeeper/Shallower` 层级缩放 + `CheckLinkInfo/CheckSpotInfo` 链接/地点命中**，无「在线玩家列表 UI」（与 `docs/features/world-map-players/README.md:15` 一致）。

### 2.10 ★ `CCashShop` 14 方法（现金商城）

方法集：`Close / Init / IsUsableItem / IsUsableItemGift / OnBuyAvatar / OnBuyPackage / OnChangedCategory / OnChangedCategorySub / OnChangedPage / OnDefaultAvatar / OnGiftMateInfoResult / OnKey / OnTakeOff / SendBuyAvatarPacket`。
`Init` 用 `SP_1301/1302/5361_UI_CASHSHOPIMG_BASE_BACKGRND/1/2`、`SP_1249_SOUND_BGMUIIMG_SHOPBGM`、`SP_40_ARCH_MAGEICELIGHTNING`。
`OnKey` 逐字：`a3>=0 && a2==20 → IsControlingUserPreview → SetUserPreviewControl(!)`；随后按 `CCSWnd_Char::OnKeyRet(this+1360)` → `CCSWnd_Tab::OnKeyRet(this+5384)` → `CCSWnd_List::OnKeyRet(this+5560)` → `CCSWnd_Status::OnKeyRet(this+7644)` 依次分派。
→ 现金商城是**4 个子窗（Char/Tab/List/Status）组合 + 用户预览控制**，有「礼物可用性」`IsUsableItemGift` 与「好友送礼结果」`OnGiftMateInfoResult`。

### 2.11 ★ `CUIUserInfo` 13 方法（含宠物 / 心愿单）

`CreatePetAvatar / Draw / OnButtonClicked / OnCreate / ResetInfo_Pet / SetAvatarInfo / SetLayer / SetMultiPetInfo / SetPetInfo / SetPetItemList / SetState / SetWishItemInfo / Update`。
用 `SP_2051/2052/2055/2057_UI_UIWINDOWIMG_USERINFO_BACKGRND/2/5/7`、`SP_2564_..._USERINFO_REQLV`、`SP_2577/2578_..._USERINFO_MARRY_MARRIED_0/UNMARRIED_0`、`SP_4682_..._USERINFO_BOSSPETCROWN`。
→ 原生 UserInfo 含**宠物头像 / 多宠物 / 宠物装备列表 / 心愿物品 / 婚姻状态**子页。

### 2.12 其它类关键事实

- `CUIQuestInfo` 15 方法：`LoadData / OnCreate / OnTabChanged / OnToggleDetail / ToggleDetail / OnChildNotify / SetScrollBar / SyncScrollBarWithSelected / TryShowRecentlyUpdatedQuest / TryShowRecentlyViewedQuest / TryShowSuitableLevelQuest / TryShowTopQuest / GetQuestInfo / MarkQuestAsRead / ResetInfo`；`OnCreate` 用 `SP_1897..1901_..._QUEST_ICON0/1/2/4`、`SP_2664/2665_..._QUEST_TAB_ENABLED/DISABLED`、`SP_3232_QUEST_QUESTINFOIMG`、`SP_1891_..._QUEST_BACKGRND2`、`SP_1896_..._QUEST_LINE`、`SP_1410_CANVASFONT` / `SP_1411_BA` / `SP_5527_ARIAL`。`OnChildNotify` 与 CUIItem 同构（2000 tab / 2001 滚动 / 100 转发）。
- `CUIEnchantDlg` 2 方法：`MakeClone / RefreshToolTip`（强化窗是「复制 tooltip」型，不是独立大窗）。
- `CUICashItemGachapon` 3 方法：`OnCreate / Update / OnCashItemGachaponResult`；`OnCreate` 用 `SP_5066/5067_UI_UIWINDOWIMG_CASHGACHAPON_BTOPEN/BTCLOSE`。
- `CUIFadeYesNo` 19 方法；`OnCreate` 类用 `SP_1284/1285_..._FADEYESNO_BTOK/BTCANCEL`、`SP_1286..1294 + 5161_..._ICON0..9`、`SP_1295/1296/1298/5160_..._BACKGRND/2/4/6`（多底板多图标变体）。
- `CUIShortCutMenu` 2 方法；`OnCreate` 用 `SP_2228..2237_UI_UIWINDOWIMG_SHORTCUT_BT{ITEM,EQUIP,STAT,SKILL,QUEST,COMM,MOBBOOK,MESSENGER}`（与 HUD 面同一套）。
- `CMacroSysMan` 8 方法含 `SetMacroInfo(nMacroIdx, sName, bShout)`；`CUIMacroSysEx` 8 方法用 `SP_4163_..._SKILLMACROEX_MACROSLOT2`、`SP_4164_..._SKILLMACROEX_MACROICON_D_ICON`；`CUISkillEx` 3 方法用 `SP_4345/4346_..._SKILLMACROEX_BTMACROIN/BTMACROOUT`。
- `CCSWnd_Status` 4 方法（`OnCreate / OnButtonClicked / Draw / OnKeyRet`）→ 现金商城状态子窗。
- `CItemInfo` 47 方法、`CQuestMan` 17 方法（数据层，本次仅精选反编译）。

## 3. SP_ 节点全集要点（401 个，91 个 UIWINDOWIMG）

- **ITEM（背包）**：`BTCOIN / BTGATHER / BTSORT / BTCASHSHOP / BTFULL / BTSMALL / ACTIVEICON / TAB_ENABLED / TAB_DISABLED / FULLBACKGRND / BACKGRND / DISABLED`。
- **SKILL**：`BTSPUP / SKILL0 / SKILL1 / RECOMMEND_0 / LINE`。
- **STAT**：`BTAPUP / BTAUTO / BTAUTO1 / BTAUTO2 / BTDETAIL / BASICSTAT / JOB / DISABLED_`。
- **QUEST**：`ICON0/1/2/4 / TAB_ENABLED / TAB_DISABLED / BACKGRND2 / LINE`。
- **SKILLMACRO**：`BACKGRND / MACROICON_D_ICON / MACROSLOT2 / BTMACROIN / BTMACROOUT`。
- **USERINFO**：`BACKGRND/2/5/7 / REQLV / MARRY_MARRIED_0 / MARRY_UNMARRIED_0 / BOSSPETCROWN`。
- **WORLDMAP**：`BORDER_0..7`（八片边框）、`TITLE`。
- **FADEYESNO**：`BTOK / BTCANCEL / ICON0..9 / BACKGRND/2/4/6`。
- **CASHGACHAPON**：`BTOPEN / BTCLOSE`。
- **SHORTCUT**：`BTITEM/BTEQUIP/BTSTAT/BTSKILL/BTQUEST/BTCOMM/BTMOBBOOK/BTMESSENGER`。
- **KEYCONFIG**：`NOTICE_D`。
- **CASHSHOP**：`UI_CASHSHOPIMG_BASE_BACKGRND/1/2`、`SOUND_BGMUIIMG_SHOPBGM`。
- 完整 401 个见 `evidence/angel-corewindow-facts.md` §1。

## 4. 与上一轮 BeiDou 校准 / Q2 清单的关系（观察）

- BeiDou(13337) 该面 439/441 全为 `sub_*`，上一轮 `r16-rea-coverage-gap.md` CoreWindows 批次仅达「B 锚点」（`sub_8E49B5` ToolTip、`sub_83EA05` Quest、`sub_87CE25`、`sub_999E4D` CashGachapon、`sub_82A9BC` ViciousHammer、`sub_82BB83` VegaSpell、Maker `sub_822350` 达 C），**Angel 恰好补上这些 `sub_*` 的类名/方法名/调用关系层**。
- 具体映射（Angel 具名 ↔ BeiDou `sub_*`，来自 crosscheck 表）：`CCashShop::Init 0x468f3e → sub_468F3E(0x65c)`、`CUIStat::OnButtonClicked 0x8c5245 → sub_8C5245`、`CUIItem::OnChildNotify → 同 EA sub_*` 等。
- **坐标层上一轮已用无符号 IDB 对齐过**（HUD 面已证 `UIStatusBar.cpp:1901-1907` 与 Angel 逐点吻合）；本面 Angel 补的是 **ctrl id→业务语义映射、网格命中常量、脏词过滤、字体分派、跨窗联动** 等行为层。

## 5. 限制 / 未知

- 296 函数是**精选**（非全部 441）；`CItemInfo`/`CQuestMan`/`SKILLENTRY` 数据层仅部分方法反编译，需时按 `tools/ida_angel_corewindow_decompile.py <out.json> <Class>` 补采。
- `CUIToolTip::SetToolTip_Equip/Package`、`CUIUserInfo::Draw`、`CUIQuestAlarm::Draw`、`CUIStat::Draw`、`CUIStatDetail::Draw`、`CUIFadeYesNo::Draw`、`CQuest::OnCompleteQuestFailed` 是 7 个 >50 KB 超大函数，已 `/output/` 补全但**未逐行人工核完**，细节以 `evidence/angel-corewindow-decompile-full.json` 为准。
- IMG 层 MCP 未接入；`UIWindow.img` / `UIToolTip.img` 节点树真值须以仓库内 `.nx` 引用 + `StatusBar.img.xml` 类 dump + 正确端截图为准（待第三路 subagent 盘点）。
- 所有「s9 适配 / 改回原生会否断联机」判定不在本文，见 README §6 决策点。
