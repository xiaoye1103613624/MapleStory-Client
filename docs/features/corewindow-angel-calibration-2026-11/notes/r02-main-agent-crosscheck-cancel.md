# r02 · 主 agent 独做：Angel 083 ↔ 当前端「核心窗口」跨端锚点销案复核

> 性质：只读取证 + 逐字销案，不修改任何产品源码、不构建、不实机、不提交。
> 基准固定 **083/v83**（Angel 13339 带符号 IDB 为权威）；**禁止引 095 线符号/IDB 作依据**（见 §5）。
> 与 r01（Angel 原生一手语义）、三路 subagent（`18924e1b` 当前端源码盘点 / `4cfbbb14` 既有文档四态 / `2956105c` IMG 样式）互补：本笔记专做「原生逐字 → 当前端逐字」的销案/推翻判定，三路 subagent 不覆盖此层。
> 时间：2026-11-08（goal `goal-11388c42` Round 2）。

---

## 0. 结论速览

1. **原生单体方法名在当前端全部不存在**：`SendAbilityUpRequest` / `AutoApUp` / `ToggleDetail` / `CCurseProcess::ProcessString` / `SetToolTip_*` / `GetFontByType` / `ShowItemToolTip` 等 Angel 083 方法名，全仓 `*.cpp` grep **零命中** → 当前端是独立重写，不是移植同名方法。[观察]
2. **但「功能面」大多已实现，只是常量体系/编号口径/命名不同**：能力值窗、整理背包、技能加点、任务窗、键位设置、世界地图均有对应类与 TYPE 绑定。典型 = `UIStatsInfo.cpp` 已能 `AutoAssignApPacket(...).dispatch()`，只是没有 `CUIStat::AutoApUp` 这个壳。[观察]
3. **结构性差异（本面最重要）**：Angel 083 = 单体 `CUIToolTip`（26 方法统一引擎） + 单体 `CUIStat`（8 方法） + 单体 `CUIItem`（12 方法）；当前端 = `IO/Components/` 下 5 个独立 Tooltip 类（`EquipTooltip`/`ItemTooltip`/`SkillTooltip`/`TextTooltip`/`MapTooltip`）+ `UIStatsInfo` + `UIItemInventory`/`UIEquipInventory` 拆分。与登录面「拆成多类」、HUD 面「`CUIStatusBar` 拆 10 类」是同一模式。[观察+推断]
4. **真缺口（跨面共性）**：`CCurseProcess::ProcessString` 脏词过滤**在 HUD 面 `CChatHelper::TryChat` 与本面 `CUIMacroSys::OnButtonClicked` 两处原生都存在，当前端全仓零命中** → 脏词/禁言是整族缺失，不是单窗遗漏。[观察]
5. **可零风险先改的一致性疑点**：`IO/UITypes/UIStatsInfo.cpp:146` 注释引 `Maplestory95 CUIStat::OnCreate(0x867b90)`（095 线地址）作坐标依据 → 按 083 基准应改引 Angel `CUIStat::OnCreate @ 0x8c4c7f`，坐标结论本身（BT_AUTO(106,198)）与截图实测一致可保留。[观察+推断]
6. **EA 交叉验证再次成立**：13 个核心窗口关键 Angel 方法在 BeiDou(13337) 同 EA 全为 `sub_*` 无名 → 与登录面 327/328、HUD 面 547/548 一致（本面 439/441，见 §3）。[观察]

---

## 1. 取证方法与门禁

- REA→IDA→IMG 三联：IDA MCP 13339（Angel，带符号）+ 13337（BeiDou，无符号 EA 交叉）已用；**IMG 层记阻塞**（orange-wz MCP 未接入本会话 Host，正确端截图/`.nx` 节点引用由 subagent `2956105c` 归档）。
- 权威源：`evidence/angel-corewindow-decompile-full.json`（296 函数逐字 Hex-Rays，7 个截断大函数已 `/output/<uuid>.json` 补全并归一化）、`evidence/angel-corewindow-ea.json`（441 具名函数 / 62 类）、`evidence/angel-beidou-corewindow-ea-crosscheck.json`。
- 当前端逐字：`grep` + `read` 直读 `IO/UITypes/*.cpp`、`IO/Components/*.cpp`、`IO/UIStateGame.cpp`。
- 标注：`[观察]`=源码/伪代码逐字；`[推断]`=基于观察的判断；`[s9 适配]`=改回原生会断联机，须交用户裁决。

---

## 2. 逐锚点跨端销案

### 2.1 `CUIStat::OnButtonClicked @ 0x8c522f`（能力值窗按钮）— ⚠️ 功能在、口径不同

- Angel 083 逐字 10 case：`2000→SendAbilityUpRequest(*dword_BE7918, 2048)` / `2001→0x2000` / `2002→64` / `2003→128` / `2004→256` / `2005→512` / `2006→CUIStat::ToggleDetail(this)` / `2007+2008→CUIStat::AutoApUp(1)` / `2009→CUIStat::AutoApUp(0)` / `default→CUIWnd::OnButtonClicked(a2)`。`CUIStat::OnCreate @ 0x8c4c7f` 用 `SP_1943..1946_..._STAT_BTAPUP/BTAUTO*`、`SP_2064_..._BTDETAIL`、`SP_5153_..._BASICSTAT`。[观察]
- 当前端：`grep 'SendAbilityUpRequest|AutoApUp|ToggleDetail|ABILITY_UP|AUTO_AP'` 全仓 `*.cpp` **零命中**；但 `IO/UITypes/UIStatsInfo.h:30` `static constexpr Type TYPE = UIElement::Type::STATSINFO; FOCUSED=false; TOGGLED=true;`、`:34 UIStatsInfo(const CharStats& stats);`；`IO/UITypes/UIStatsInfo.cpp:59` ctor `UIDragElement<PosSTATS>(Point<int16_t>(212, 150))`、`:358 dimension = Point<int16_t>(212, 318)`。[观察]
  - `:129-135` 已建 `BtApUp/BtHpUp/BtMpUp/BtStrUp/BtDexUp/BtIntUp/BtLukUp` 七颗；`:139-144` 坐标 `(186,115)/(186,133)/(186,245)/(186,263)/(186,281)/(186,299)`。[观察]
  - `:154-155 BT_AUTO(106,198)`、`:159 BT_DETAILOPEN(153,324)`、`:164 BT_DETAILCLOSE(405,326)`。[观察]
  - `:679 case Buttons::BT_AUTO`、`:726-752` `AutoAssignApPacket(to_code(primary), ap, second_code, 0).dispatch()`（`to_code` 0/1/2/3→STR/DEX/INT/LUK）、`:773/:779/:780` 详情开关三 case。[观察]
  - `:54-102 StatLabel` 枚举：Normal 16 项 + Detailed 20 项 + classic 详情板 5 项（MAGIC_D/MDEF_D/ACC_D/AVOID_D/HANDS_D），`:59-61` 注「v83 经典『角色能力』窗（Stat/backgrnd）八行专用」。[观察]
- 判定：**原生 10 个 ctrl id（2000–2009）与当前端 `Buttons` 枚举不是同一套编号**（HUD 面 r05 §D 已证当前端 button id 是 0..47 内部递增）。功能等价（单点加属性 / 自动分配 / 详情切换都在），但原生 `SendAbilityUpRequest` 的「按位 2048/0x2000/64/128/256/512」掩码体系当前端用 `AutoAssignApPacket` 的 StatCode 替代 → **属实现路径差异，非缺失**。[观察+推断]

### 2.2 `CUIItem::OnChildNotify @ 0x81d01f`（背包整理/扩展）— ⚠️ id 语义错位 + 缺「扩展背包」

- Angel 083 逐字：`a2==2000&&a3==500→OnTabChanged(a4)`；`a2==2001&&a3∈[0x12C,0x130]→ClearToolTip+SetLastestGetItemEffect`；`a3==101&&a2!=2003→switch(a2)`：`case 0x7D4(2004)→SP_698_ITEM_SORT+sub_81C2D1`（BtSort）、`case 0x7D5(2005)→SP_4501_EXPAND_INVENTORY+sub_81C2D1`（扩展背包）。`CUIItem::OnCreate` 用 `SP_1851_..._ITEM_BTGATHER` / `SP_1852_..._ITEM_BTSORT` / `SP_5580/5581_..._ITEM_BTFULL/BTSMALL` / `SP_4500_..._ITEM_BTCASHSHOP`。[观察]
- 当前端 `IO/UITypes/UIItemInventory.cpp`：`:123-125` 注释「开窗默认全页签 nState=0 → 显示 BtGather（红底 +）；`SetArrangeButton(m_nItemTI, 0) → AddButton(…/BtGather, id=0x7D3)`」；`:1440`「nState==0 → BtGather(0x7D3)；nState==1 → BtSort(0x7D4)」；`:1867-1868` `place(BT_GATHER, (W-31,4)); place(BT_SORT, (W-31,4));` **两颗叠同一坐标**；`:1457-1495` classic 包缺 `AutoBuild/FullAutoBuild` ⇒ `BT_SORT_SM`/`BT_GATHER_SM` 不创建；`:534-541` `button_pressed` 四 case；`:247/:308/:1872` 仅有 `BtFull/BtSmall`、`BtCashshop` 注释。[观察]
- 判定：**`0x7D4=BtSort` 与 Angel 一致；但 Angel 的 `0x7D5=扩展背包(SP_4501_EXPAND_INVENTORY)` 当前端无对应分支**（`:1872` 仅注「扩充道具栏 (BtCashshop) exists only in the expanded frame」）。当前端多出的 `0x7D3=BtGather`（国服整理）在 Angel GMS v83 `OnChildNotify` 无 case → **属国服/s9 适配 + 扩展背包语义缺失**，改回原生须裁决。[观察+推断，s9 适配]

### 2.3 `CUIMacroSys`（宏窗）— ❌ 脏词过滤整族缺失 + 几何待核

- Angel 083：`GetIndexByPos @ 0x8b8de3` 逐字 `v7=65; while(v7-32>a3||a3>v7){v7+=39;++v6;if(v7>=260)...} *a5=v6; v9=44; while(v9-32>a2||a2>v9){v9+=37;++v8;if(v9>=155)...} *a6=v8; if(a4!=0&&a2>=145&&a2<=177) *a6=3;` → 32 格 / 行 Y65 步进 39（5 行到 260）/ 列 X44 步进 37（3 列到 155）。`OnButtonClicked @ 0x8aa134` 保存宏名走 `CCtrlEdit::GetText` → `ZXString<char>::GetBuffer(1024,1)` → **`CCurseProcess::ProcessString(Buffer,0,1)` 脏词过滤** → `CMacroSysMan::SetMacroInfo(...)` → 成功 `SP_3296_IT_IS_SAVED`，失败 `SP_272_CURSING_IS_NOT_ALLOWED`。[观察]
- 当前端：`grep 'CCurseProcess|ProcessString|脏词|CURSING_IS_NOT_ALLOWED|SP_3296|IT_IS_SAVED'` 全仓 `*.cpp` **零命中**。[观察]
- 判定：**宏名保存的脏词校验当前端完全缺失**（与 HUD 面 `CChatHelper::TryChat` 脏词/刷屏/重复句限制缺失同族）；宏窗几何常量口径（65/39/44/37/260/155）待 `18924e1b` 盘点后核对。[观察+推断]

### 2.4 `CUIToolTip`（26 方法统一引擎）— ⚠️ 结构性拆分，非缺失

- Angel 083 26 方法全表（`0/1ItemToolTipParam`、`AddInfoEx`、`ClearToolTip`、`DrawItemIcon`、`DrawToolTip_Equip`、`GetFontByType @ 0x8f36a1`（case 1–13+ 取 `this+265..278` IWzFont）、`MakeLayer`、`PrintValue`、`SetBasicInfo`、`SetToolTip_{Bundle,Equip,Equip2,EquipExt,Equip_Basic,MacroSysSkill,Package,Pet,Ring,Skill,SlotInc,String,String2,String_MultiLine,WorldMap}`、`ShowItemToolTip`）；`SetToolTip_String @ 0x8e6e7d` 用 `SetBasicInfo(this,1,v8+8,v19,-1)`。[观察]
- 当前端：`grep 'SetToolTip_|GetFontByType|SetBasicInfo|SetToolTip_Equip|SetToolTip_Skill|SetToolTip_String|ShowItemToolTip|MakeLayer'` 在 `IO/*.cpp` **零命中**；`glob IO/Components/*Tooltip*` = 11 文件：`Tooltip.h` + `EquipTooltip.{h,cpp}` / `ItemTooltip.{h,cpp}` / `SkillTooltip.{h,cpp}` / `TextTooltip.{h,cpp}` / `MapTooltip.{h,cpp}`。`IO/Components/EquipTooltip.cpp:51 EquipTooltip::EquipTooltip()`、`:135 set_equip(Tooltip::Parent parent, int16_t ivp)`（`:155-159` case ITEMINVENTORY/SHOP/EQUIPINVENTORY）、`:651 draw(Point<int16_t> pos) const`、`:807 draw_preview(Point<int16_t> pos) const`、`:680 backdrop.draw(pos+(-6,-6), FRAME_WIDTH+12, height+32)`。[观察]
- 判定：**当前端把原生单体引擎拆成 5 个按数据类型分发的 Tooltip 组件类**，功能是自绘重写（含 preview 预览对比、潜能标志 `potflag`、需求属性 `reqstattextures`、成长 `growthset`、装备对比 `diff`），不是 26 个 `SetToolTip_*` 的直译 → 与登录面/HUD 面同一「拆分」模式。⚠️ 已知 `docs/features/ui-v83-window-restore/PLAN.md:20,34` 记 5 个 tooltip 组件指向不存在的 `UI["UIToolTip.img"]["Item"]` 节点（`ItemTooltip.cpp:33` 等）—— Angel 用 `SP_*` StringPool 索引而非明文路径，须由 `2956105c` 复核当前端节点引用是否有效。[观察+推断]

### 2.5 `CUIKeyConfig @ 0x8335d0` / `CUIStatDetail` / `CUIEnchantDlg` / `CUICashItemGachapon` — 待 subagent 复核

- Angel 083：`CUIKeyConfig::OnButtonClicked` 5 分支（1=Save / 2=InvalidateRect StatusBar / 2000=恢复默认键位+ReDrawQuickslot / 2001=清空 445 字节 / 2002=sub_836C9B）+ `UI_Close(5)`；`GetShortCutIndexByPos @ 0x833a65` 32×32 命中表基址 `dword_BE27E4..BE2C6C` + 特例 `case '6':42; 'Y':29; 'Z':56`。`CUIStatDetail` 仅 2 方法 `Draw/OnButtonClicked`；`CUIEnchantDlg` 2 方法 `MakeClone/RefreshToolTip`；`CUICashItemGachapon` 3 方法 `OnCreate/Update/OnCashItemGachaponResult`（`SP_5066/5067_..._CASHGACHAPON_BTOPEN/BTCLOSE`）。[观察]
- 当前端 `IO/UIStateGame.cpp:459 emplace<UIKeyConfig>`；`UIQuickslotConfig.cpp:92-144` `apply_slot_key→kb.commit_quickslots()` + `CHANGE_QUICKSLOT(183)`（HUD 面 r04 已记）。[观察]
- 判定：存在对应类；「恢复默认键位/清空键位/宏键位拖拽」等原生 5 分支当前端覆盖度待 `18924e1b` + `4cfbbb14` 清单判定，本笔记不预判。[观察]

### 2.6 `CWorldMapDlg @ 0x9eb366` — ⚠️ 原生无在线玩家列表（与既有文档一致）

- Angel 083 逐字 `CDialog::CreateDlg(this, a2:67, a3:0, a4:666, a5:524, a6:10, a7:1, a8:0)` → **666×524 z=10 bScreenCoord=1**；18 方法含 `SetWorldMapDeeper/Shallower`（层级缩放）、`CheckLinkInfo/CheckSpotInfo`；`OnButtonClicked @ 0x424492` 仅 `a2==1/2`；`MakeBorder` 用 `SP_2281..2288_..._WORLDMAP_BORDER_7..0`；`OnCreate` 用 `SP_2289_..._WORLDMAP_TITLE` / `SP_1240_WORLDMAPOPEN` / `SP_5577_UI_BASICIMG_BTCLOSE`。**无在线玩家列表 UI**。[观察]
- 当前端 `IO/UIStateGame.cpp:400 emplace<UIWorldMap>()`、`:824/:851/:860` WORLDMAP 特殊处理；`docs/features/world-map-players/README.md:15` 已记「原生 IDA 世界地图字符串仅 `Map/WorldMap/`、`MapLink`、`linkMap`、`npcPos`，**无在线玩家列表 UI**，本端右上列表为 s9 协议挂接布局」。[观察]
- 判定：**原生 666×524 尺寸与「无在线玩家列表」结论在本面用带符号 IDB 再次坐实**；当前端右上列表属 s9 适配，应保留并标注，不判缺失。[观察+推断，s9 适配]

### 2.7 `CCashShop`（66 方法）/ `CUISkill`（5）/ `CUIQuestInfo`（15）/ `CUIUserInfo`（13）— 存在对应类，深度待复核

- Angel 083 `CCashShop` 全量 66 方法（`OnPacket`/`OnQueryCashResult`/`OnCashItemRes*`/`SendGiftsPacket`/`OnSetWish`/`OnRemoveWish`/`ProcessBuy`/`OnCashItemGachaponResult` 等），`Init` 用 `SP_1301/1302/5361_UI_CASHSHOPIMG_BASE_BACKGRND/1/2`、`SP_1249_SOUND_BGMUIIMG_SHOPBGM`；`OnKey` 按 `CCSWnd_Char→CCSWnd_Tab→CCSWnd_List→CCSWnd_Status` 分派。`CUISkill::OnSkillLevelUpButton @ 0x8acf1a` 前置 `CanSendExclRequest(500,0)` + SP 校验 + `SP_3509/3510/3511` 文案。`CUIQuestInfo` 15 方法（`LoadData/OnTabChanged/ToggleDetail/SetScrollBar/SyncScrollBarWithSelected/TryShow*Quest/MarkQuestAsRead`）。`CUIUserInfo` 13 方法含 `CreatePetAvatar/SetMultiPetInfo/SetPetItemList/SetWishItemInfo`。[观察]
- 当前端：`IO/UIStateGame.cpp:349 emplace<UISkillBook>`、`:439 emplace<UIQuestLog>`；`IO/UITypes/UICashShop.cpp` / `UIStateCashShop.cpp` 存在（Q2 已证）。[观察]
- 判定：大体存在对应类；「礼物/礼包/愿望清单/点券查询/技能加点门槛/任务近期更新分类/宠物头像」等原生深度分支的当前端覆盖度，交由 `18924e1b`（源码盘点）+ `4cfbbb14`（四态审计）产出后写入 README 六维表，本笔记不预判。[观察]

---

## 3. EA 交叉验证补表（Angel 13339 ↔ BeiDou 13337，13 个关键方法）

逐字取自 `evidence/angel-beidou-corewindow-ea-crosscheck.json`；全部 `beidou_name = sub_*`（无符号），佐证「同一份 v83 二进制不同符号状态」。

| Angel 方法 | Angel EA | BeiDou | size |
|---|---|---|---|
| `CCashShop::OnKey` | `0x469626` | `sub_469626` | `0x8b` |
| `CUICashItemGachapon::OnCreate` | `0x999e4d` | `sub_999E4D` | `0x43c` |
| `CUIItem::OnChildNotify` | `0x81d01f` | `sub_81D01F` | `0x211` |
| `CUIKeyConfig::OnButtonClicked` | `0x8335d0` | `sub_8335D0` | `0x15c` |
| `CUIKeyConfig::GetShortCutIndexByPos` | `0x833a65` | `sub_833A65` | `0x99` |
| `CUIMacroSys::OnButtonClicked` | `0x8aa134` | `sub_8AA134` | `0xe8` |
| `CUIMacroSys::GetIndexByPos` | `0x8b8de3` | `sub_8B8DE3` | `0x80` |
| `CUISkill::OnSkillLevelUpButton` | `0x8acf1a` | `sub_8ACF1A` | `0x31e` |
| `CUIStat::OnCreate` | `0x8c4c7f` | `sub_8C4C7F` | `0x435` |
| `CUIStat::OnButtonClicked` | `0x8c522f` | `sub_8C522F` | `0x6b` |
| `CUIToolTip::SetToolTip_Equip` | `0x8e8252` | `sub_8E8252` | `0x1e47` |
| `CUIToolTip::GetFontByType` | `0x8f36a1` | `sub_8F36A1` | `0xdc` |
| `CWorldMapDlg::CreateWorldMapDlg` | `0x9eb366` | `sub_9EB366` | `0x3f5` |

本面总读数：`angel_funcs 441 / beidou_present 439 / missing 2`（`0x6cec47`、`0x8aa4e4`，均为 IDA 分析覆盖差异），conclusion = `same binary layout (439/441 EAs identical; differences are IDA analysis coverage, not code)`。

---

## 4. Angel 侧 12 类完整方法 EA 表（供 r01 / README 引用）

取自 `evidence/angel-corewindow-ea.json`（441 函数 / 62 类），逐字 decorated/demangled 已在该 JSON。关键类方法全集：

- **`CUIStat`(8)** `0x8c4c7f OnCreate` / `0x8c522f OnButtonClicked` / `0x8c545d ToggleDetail` / `0x8c5845 EnableApUpButton` / `0x8c59d4 ResetInfo` / `0x8c59ff Draw` / `0x8c79f5 RestoreButtons` / `0x8c8692 AutoApUp`
- **`CUIStatDetail`(2)** `0x8c2870 Draw` / `0x8c477f OnButtonClicked`
- **`CUISkill`(5)** `0x8aa4c5 CloseSkillGuide` / `0x8aaa88 OnCreate` / `0x8acf1a OnSkillLevelUpButton` / `0x8bcdfa GetSkillRootVisible` / `0x961063 OpenCurSkillGuide`
- **`CUIQuestInfo`(15)** `0x87b921 GetQuestInfo` / `0x87ec3f OnCreate` / `0x87fb2f OnChildNotify` / `0x87fd4f OnTabChanged` / `0x881b18 LoadData` / `0x882932 TryShowRecentlyUpdatedQuest` / `0x88296e TryShowRecentlyViewedQuest` / `0x88297e TryShowSuitableLevelQuest` / `0x882a71 TryShowTopQuest` / `0x882c8a SyncScrollBarWithSelected` / `0x882f2f MarkQuestAsRead` / `0x882f88 ResetInfo` / `0x883100 SetScrollBar` / `0x88422c ToggleDetail` / `0x8843a4 OnToggleDetail`
- **`CUIToolTip`(26)** `0x483eed 0ItemToolTipParam` / `0x483f18 1ItemToolTipParam` / `0x8e6e23 ClearToolTip` / `0x8e6e52 SetBasicInfo` / `0x8e6e7d SetToolTip_String` / `0x8e7028 SetToolTip_String_MultiLine` / `0x8e7150 SetToolTip_String2` / `0x8e7434 SetToolTip_WorldMap` / `0x8e7836 PrintValue` / `0x8e7975 SetToolTip_Ring` / `0x8e8252 SetToolTip_Equip` / `0x8eba5b SetToolTip_Equip2` / `0x8eca0c SetToolTip_Equip_Basic` / `0x8ed0d2 DrawToolTip_Equip` / `0x8ed8f5 SetToolTip_Pet` / `0x8edfc7 SetToolTip_Bundle` / `0x8efcb7 SetToolTip_Package` / `0x8f19a8 SetToolTip_SlotInc` / `0x8f1eaa SetToolTip_EquipExt` / `0x8f2276 SetToolTip_MacroSysSkill` / `0x8f25d0 SetToolTip_Skill` / `0x8f3141 MakeLayer` / `0x8f36a1 GetFontByType` / `0x8f39e1 AddInfoEx` / `0x8f421e DrawItemIcon` / `0x8f5b20 ShowItemToolTip`
- **`CUIUserInfo`(13)** `0x8fecb1 OnCreate` / `0x8ff5b0 OnButtonClicked` / `0x8ff7fa SetState` / `0x8ff863 SetLayer` / `0x9010ba Update` / `0x901185 Draw` / `0x903c50 SetPetInfo` / `0x903ce7 SetMultiPetInfo` / `0x903e24 SetAvatarInfo` / `0x9041c8 SetWishItemInfo` / `0x90433a CreatePetAvatar` / `0x904967 ResetInfo_Pet` / `0x904aea SetPetItemList`
- **`CWorldMapDlg`(18)** `0x424492 OnButtonClicked` / `0x9e98b7 ClearToolTip` / `0x9ea93b SetWorldMap` / `0x9eb366 CreateWorldMapDlg` / `0x9eb75b OnCreate` / `0x9eb94a OnDestroy` / `0x9eb98e HitTest` / `0x9eb9c6 Draw` / `0x9ebfc3 MakeBorder` / `0x9ed38b LoadMapList` / `0x9edd67 CheckSpotInfo` / `0x9ee00e CheckLinkInfo` / `0x9ee2b3 OnMouseMove` / `0x9ee2ec OnMouseEnter` / `0x9ee30e SetWorldMapDeeper` / `0x9ee507 SetWorldMapShallower` / `0x9ee6e5 OnMouseButton` / `0x9ee717 OnKey`
- **`CUIKeyConfig`(7)** `0x8335d0 OnButtonClicked` / `0x83372c OnMouseButton` / `0x833749 OnDestroy` / `0x8338f9 OnMouseMove` / `0x833a65 GetShortCutIndexByPos` / `0x83550d TryBeginDragFuncKeyMappedIcon` / `0x83665b 0CNoticeDlg`
- **`CUIMacroSys`(8)** `0x8a8504 OnCreate` / `0x8a98dc OnKey` / `0x8aa134 OnButtonClicked` / `0x8b8daa ResetInfo` / `0x8b8de3 GetIndexByPos` / `0x8b8e63 OnSelected` / `0x8b8f11 OnMouseButton` / `0x8b956a OnMouseMove`
- **`CCashShop`(66)** 全表见 `angel-corewindow-ea.json`（本笔记 §2.7 列关键 14 法 + 收包族）
- **`CUIItem`(12)** `0x81c6b7 ChangeTab` / `0x81c6c9 OnCreate` / `0x81d01f OnChildNotify` / `0x81d230 OnTabChanged` / `0x81d42e OnMouseButton` / `0x81d8ad OnMouseMove` / `0x81d996 OnDropMoney` / `0x81db7e GetSlotPositionFromPoint` / `0x81dc20 Draw` / `0x81e2c8 GetItemSlotRect` / `0x81e3a6 SetArrangeButton` / `0x81e5da SetLastestGetItemEffect`
- **`CUIEquip`(4)** `0x7fe9bb OnMouseMove` / `0x7feb72 OnButtonClicked` / `0x7ff8dd ToggleDragonEquip` / `0x7ffa84 TogglePetEquip`

---

## 5. ⚠️ 095 文档污染标注（按 MEMORY.md 硬约束）

`IO/UITypes/UIStatsInfo.cpp:146-150` 逐字：

```
// BtAuto：Maplestory95 CUIStat::OnCreate(0x867b90) 对
// UI/UIWindow2.img/Stat/main/BtAuto 走 CLayoutMan::AddButton(nOffset=0,0)
// （现代 STR y=244 / 经典 245），故 y 直取 198。本包 BtAuto 为 100x35
```

→ 该注释把「BT_AUTO 坐标 (106,198)」的依据挂在 **095 线 `Maplestory95.i64` 的 `CUIStat::OnCreate(0x867b90)`** 上。按 083 基准，正确权威源应为 Angel `CUIStat::OnCreate @ 0x8c4c7f`（本面 EA 交叉验证 BeiDou `sub_8C4C7F` 同址）。**坐标结论本身与正确端截图实测一致（r02 HUD 面同款「注释污染、坐标无误」形态），建议只换依据、不动坐标**；是否回写源码交 README 决策点裁决。[观察+推断]

同类提醒：`docs/features/port-095/notes/opcode-v95.md:758,760` 登记的 `140 CUIStatusBar::SendGroupMessage (0x87f7f0)` / `142 CUIStatusBar::SendCoupleMessage (0x87b3e0)` 属 095 线，仅登记存在、**不作本面依据**。

---

## 6. 跨面共性结论（登录面 / HUD 面 / 核心窗口面三面统一）

1. **EA 交叉验证三面一致**：327/328（登录）→ 547/548（HUD）→ 439/441（核心窗口），差异均为 IDA 分析覆盖（1–2 个 EA），非代码差异 → Angel 与 BeiDou 是同一份 v83 二进制的不同符号状态，已三面钉死。
2. **拆分模式三面一致**：登录 `CLogin` 多窗 → 当前端多 `UI*`；HUD `CUIStatusBar` 41 方法 → 10+2 类；核心窗口 `CUIToolTip` 26 方法 → 5 个 `*Tooltip` 组件 + `CUIItem`/`CUIStat` 拆多类 → 「校准 ≠ 改回原生」，维持拆分是三面共同推荐。
3. **脏词过滤跨面整族缺失**：HUD `CChatHelper::TryChat` + 核心窗口 `CUIMacroSys::OnButtonClicked` 两处原生 `CCurseProcess::ProcessString`，当前端全仓零命中 → 建议合并为一个 P1 决策点（先确认 s9 服务端是否已实现，避免双罚）。
4. **s9 适配红线三面一致**：编号/字节布局/几何常量冲突处一律标注「改回原生会断联机/破坏实机排版，须用户裁决」，不盲改。
5. **IMG 层三面均记阻塞**：orange-wz MCP 未接入本会话 Host，资源层证据以仓库内 `.nx` 节点引用 + 正确端截图 + `nx_inspect.py` 转储为准。

---

## 7. 限制与待 subagent 复核

- 本笔记只做「主 agent 独做的跨端锚点销案」，**不替代**三路 subagent：当前端全类盘点（`18924e1b`）、既有文档四态（`4cfbbb14`）、IMG 样式证据（`2956105c`）返回后将并入 README 六维对照表。
- **未做**：像素级视觉复核（`read_image` 对本模型返回 `model "hy4free" does not declare image input`）；实机构建/五屏比对；任何产品源码修改。
- **未预判**：`CCashShop` 66 方法 / `CUIQuestInfo` 15 方法 / `CUIKeyConfig` 5 分支 / `CUISkill` 加点门槛的当前端逐条覆盖度，等 `18924e1b` + `4cfbbb14` 清单。
- **须裁决**：§2.2 扩展背包语义缺失 + `BtGather` 国服适配、§5 095 注释是否回写、§2.3 脏词是否双端立项。

---

## 8. 增量：当前端核心窗口 stub / 预留 / NO-NATIVE 逐字清单（主 agent 直读，供六维表引用）

> 本节为 r02 落地后追加的独立复核，逐字来自当前端源码，标注 `[观察]`。与 §2 的「跨端锚点销案」互补：§2 对 Angel 方法，§8 对当前端自身的显式缺口标记。
> 全部为「显式预留 / stub 文案 / 包内缺节点」，**不是漏实现**，按技能门禁「不发明客户端能力」应保留并标注，除非交决策点裁决。

### 8.1 Tooltip 组件（Angel `CUIToolTip` 26 方法引擎 → 当前端 5 组件拆分）

- `IO/Components/EquipTooltip.cpp:53` `nl::node Equip = nl::nx::UI["UIWindow.img"]["ToolTip"]["Equip"];`、`:62` `nl::nx::UI["UIWindow.img"]["IconBase"]["0"]`；`IO/Components/SkillTooltip.cpp:35` 同取 `IconBase/0` → **当前端确实用 `UIWindow.img/ToolTip/Equip`（有效节点），不是 `docs/features/ui-v83-window-restore/PLAN.md:20,34` 记的 `UI["UIToolTip.img"]["Item"]`**；后者疑点须由 `2956105c` 复核是否已被修正。[观察+推断]
- `IO/Components/MapTooltip.cpp:32` 逐字「Mob/Npc 图标在包内 `UIWindow.img/ToolTip` 下不存在（仅 Equip/cash/mesos），取到空纹理即不绘。」+ `:29 // TODO: Get number of available quests` + `:38 // TODO: Get list of where party members are` → 印证 `ui-v83-window-restore/notes/open-issues.md:684`「BtSmall/BtBig/BtNpc 包内确无，仍为显式预留」。[观察]
- `IO/Components/ItemTooltip.cpp:30 // TODO: Add blue dot next to name`；`:63-69` `name/desc/qual` 用 `Text(A12B/A12M,...)` + `fillwidth=264` / `fillheight=83+name.height()`；`:99-100` 框体「内容包围盒外扩 6px（程序描边，见 TextTooltip.h TooltipBackdrop）」`backdrop.draw(pos+(-6,-6), fillwidth+28, fillheight+adj_t+adj_d+18)` → 当前端 Tooltip 底板是**程序描边九切片**，Angel 是 `MakeLayer` + IWzGr2DLayer，实现路径不同。[观察]
- `IO/Components/SkillTooltip.cpp:98 // TODO: Blank`；`IO/Components/EquipTooltip.cpp:214 // TODO: Remove from UIItemInventory and move into Job?`、`:250 // End of TODO`。[观察]

### 8.2 宏窗几何：Angel 32 格 ↔ 当前端 5 宏 / 3 可见行（⚠️ 重要一致性差异）

- Angel `CUIMacroSys::GetIndexByPos @ 0x8b8de3`：行 Y65 步进 39（5 行到 260）/ 列 X44 步进 37（3 列到 155）= 网格约 5×3 命中区（r01 §2 记「32 格」是含 `a4!=0&&x∈[145,177]→*a6=3` 特例的推断）。[观察+推断]
- 当前端 `IO/UITypes/UIMiscShells.h:643-670` `class UISkillMacroEx : public UIDragElement<PosNOTICE>`：`TYPE = UIElement::Type::SKILLMACROEX`、`:647-648`「原生技能组合栏一屏三行（Macroicon/0..2 在截图里逐像素命中，行距 45）」`VISIBLE_ROWS = 3`、`:649-650`「SkillMacros::MAX_SLOTS 的镜像」`MACRO_COUNT = 5`、`:658-659`「true = UIWindow.img/SkillMacro 原生拼装外壳；false = 退到 SkillMacroEx 整块底衬」`shell_ok`、`:663` `mutable Slider slider`「宏条数随封包变化，draw() 里同步 setrows」、`:664-665` `offset`/`selected`。「原生技能组合栏一屏三行」与 Angel `CUIMacroSys` 网格口径不同 → **宏窗几何属「正确端截图逐像素对齐」而非照搬原生，改回原生会破坏实机排版**（同 HUD 面 D6 裁决）。[观察+推断，s9 适配]
- `glob IO/UITypes/*Macro*` **零命中** → 当前端没有独立 `UIMacroSys.cpp`；宏窗实现全部集中在 `IO/UITypes/UIMiscShells.h/.cpp` 的 `UISkillMacroEx`(`:643`) / `UIMacro`(`:822`, `TYPE=MACROUI`) / `UIMacroSkill`(`:903`, `TYPE=MACROSKILL`) 三个壳类。[观察]

### 8.3 任务窗四按钮显式预留（Angel `CUIQuestInfo` 15 方法对照）

- `IO/UITypes/UIQuestLog.cpp:168-185` measure/建 `MARKNPC(BtMarkNpc)/OK(BtOK)/NO(BtNo)/ALERT(BtAlert)` 四颗；`:182` 逐字「接受/拒绝/任务讯息：底带实测只证到 寻找NPC位置，其余三颗无位可放且不发包 → 预留（见 coords.md）」。[观察]
- `:903-906` `act(OK,false); act(NO,false); act(ALERT,false);`「参考截图底带里没有这三颗，且无对应窗口可开 → 预留不显示」。[观察]
- `:951-960` `case Buttons::MARKNPC/OK/NO/ALERT:` 逐字「**显式预留（不是漏实现）**：MARKNPC『寻找NPC位置』原生是"把该任务 NPC 所在地图在世界地图上高亮点出"，需要 npc→map 反查表（服务端数据）+ UIWorldMap 的标记入口，本包两者都没有，且没有任何可对照的封包证据 —— 不发明协议，见 notes/open-issues.md」；`:947-949` `GIVEUP` 复用 `QuestActionPacket(3, selected_id).dispatch()`。[观察]
- Angel `CUIQuestInfo` 有 `TryShowRecentlyUpdatedQuest/TryShowRecentlyViewedQuest/TryShowSuitableLevelQuest/TryShowTopQuest`（4 个「最近/推荐任务」分类）+ `MarkQuestAsRead/SyncScrollBarWithSelected/SetScrollBar` → 当前端 `UIQuestLog` 的 `TAB0/1/2` 三页签 + `change_tab` 是否覆盖这 4 类待 `18924e1b` 判定。[观察+推断]

### 8.4 其它核心窗口 NO-NATIVE / stub 文案

- `IO/UITypes/UICashShop.cpp:100-102` `bind_text(job_label, "cashshop.stub_job"); bind_text(name_label, "cashshop.stub_name");` → 与 Q2 清单记的 `locale/zh-CN.ini:343-344 cashshop.stub_job/stub_name` 一致，现金商城职业/名字为占位文案。[观察]
- `IO/UITypes/UICharInfo.cpp:171` `medal_text = Text(..., Locale::text("charinfo.medal_stub"))`、`:172 medal_total = Text(..., "2")` 硬编码；`:457-460` 逐字「伤害皮肤：运行时 UI.nx/UIWindow.img 无 DamageSkin 节点、Effect.nx BasicEff.img 无 damageSkin 节点（notes/r001 §4），s9 也无对应协议，故只保留『按现金道具 id 发送使用包』的调试入口；皮肤切换与资源装载按证据不足显式预留，不凭空造节点」；`:452 UseCashItemBasicPacket(slot, itemid).dispatch()`。[观察]
- `IO/UITypes/UIWorldMap.cpp:171` 逐字「旧 WorldMapSearch 轨在本 v83 包缺素材（backgrnd 为空）→ 默认隐藏，避免右侧漂浮无框搜索文字」；`:64 nl::node backgrnd = WorldMapSearch["backgrnd"]`、`:140 BtAllsearch`、`:556/:618 UI::get().show_text/show_map(Tooltip::Parent::WORLDMAP,...)`、`:641 dimension = bg_dimensions + Point<int16_t>(bg_search_dimensions.x(), 0)` → 当前端世界地图是「主框 + 可选右侧搜索轨 + 多面板（mob_panel/map_search_panel/info_panel）」复合结构，与 Angel `CWorldMapDlg` 单体 666×524 不同（§2.6 已记）；`SP_2281..2288 WORLDMAP_BORDER_*` 九宫边框当前端是否用 `MapleFrame` 替代待 `2956105c`。[观察+推断]

### 8.4b ★ CCashShop 深度接线销案（主 agent 追加，纠正「stub 窗」误判）

> 本节推翻一个潜在误判：`UICashShop.cpp:100-102` 的 `cashshop.stub_job/stub_name` 只是**顶部职业/名字标签的占位文案**，不代表整个现金商城窗是 stub。逐字复核 `IO/UITypes/UICashShop.cpp` 后确认当前端 CashShop 协议接线非常深，与 Angel `CCashShop` 66 方法大部分一一对应。

- `:152-154` 逐字「商品列表来自 `SetCashShopHandler` 解析的主商品目录（`UIStateCashShop::cashshop_commodities`）」+ `const std::vector<CashCommodity>& commodities = UIStateCashShop::cashshop_commodities;` → 有真实商品目录数据层。[观察]
- `:230-234` `if (UIStateCashShop::cash_balances_ready)` 读 `cash_nx_credit / cash_maple_point / cash_nx_prepaid` 三种余额 → 对应 Angel `OnQueryCashResult`。[观察]
- `:371-373/:398-400` `get_element<UICoupon>()` / `emplace<UICoupon>()` 优惠券窗弹层 → 对应 Angel `OnCashItemResUseCouponDone/Failed`。[观察]
- `:430-469` SoftKey 协议族：`CashShopTakePacket(cashId).dispatch()`（BtInventory Take）、`CashShopPutPacket(cashId, invt).dispatch()`（Put）；`:732-758` 同族 Take/Put 调试入口 → 对应 Angel `OnCashItemResLoadLockerDone`（储物箱取/放）。[观察]
- `:508-514` 「Mileage = buy 时 useNX=2（MAPLE_POINT），无独立 action」；`:618-630` `CashShopBuyPacket(useNX, buy_sn).dispatch()`（BtBuy）；`:675` SoftKey buy；`:770` 「Package buy 0x1E：useNX,sn 或单 sn」→ 对应 Angel `OnBuyPackage` / `ProcessBuy` / `SendBuyAvatarPacket`（**原生 `0x1E` 包号在当前端注释里逐字保留**）。[观察]
- `:795-844` `CashShopInvSlotSnPacket` / `CashShopInvSlotPacket` / `CashShopStorageSlotPacket` / `CashShopCharSlotPacket` 四种槽位扩展包 → 对应 Angel `OnCashItemResLoadGiftDone` / `OnSetWish` / `OnBuyTransferWorldItem` 族。[观察]
- `:872-880` 愿望清单计数 `for (int32_t sn : UIStateCashShop::cash_wishlist)` + `cashshop.meta` 格式化 `CS inv {} gift {} wish {}` + `:879-880 cash_storage_slots` → 对应 Angel `FindWishList` / `OnSetWish` / `OnRemoveWish`。[观察]
- `:659 if (escape) { exit_cashshop(); return; }`、`:884 void UICashShop::exit_cashshop()` → 对应 Angel `Close`。[观察]

→ **判定：当前端 CashShop 不是 stub 窗**，`CCashShop` 66 方法里的「购买/礼包/优惠券/储物箱/礼物/愿望清单/余额查询/槽位扩展」行为面大多已接线（走 `CashShop*Packet` 独立包类 + `UIStateCashShop` 数据层），只有**顶部 `stub_job`/`stub_name` 两个 Text 是占位** + `SP_1301/1302/5361 CASHSHOPIMG_BASE_BACKGRND/1/2` 三张底板是否逐一对上待 `2956105c`。这与 Q2 清单「CashShop 礼物/礼包深度 🟡」口径一致，不得误报「完全没做」。[观察+推断]

### 8.4c ★ CUIQuestInfo 4 分类 ↔ 当前端 3 页签（形态差异，非缺失）

- Angel `CUIQuestInfo` 15 方法含 `TryShowRecentlyUpdatedQuest` / `TryShowRecentlyViewedQuest` / `TryShowSuitableLevelQuest` / `TryShowTopQuest`（4 个「最近/推荐任务」分类）+ `MarkQuestAsRead` / `SyncScrollBarWithSelected` / `SetScrollBar`；`OnCreate` 用 `SP_2664/2665_..._QUEST_TAB_ENABLED/DISABLED` 贴图页签。[观察]
- 当前端 `IO/UITypes/UIQuestLog.cpp:41 tab(0)`、`:50-53 tab_x{2,66,131} / tab_w{64,65,65} / tab_y(22) / tab_height(18)` = **3 页签**；`:94-96 tab_border/tab_fill_active/tab_fill_idle` 用 `ColorBox` **程序绘制**页签框与粉/灰底（`Color(0xEE6688FF)` / `Color(0xBBBBBBFF)`）；`:113-138` 也读 `quest["Tab"]["disabled"][i]/["enabled"][i]` 贴图（可用时优先贴图、缺件回退程序绘制）；`:694-702 change_tab(uint16_t tabid)` `if (tabid > 2) return;` + `set_state(PRESSED/NORMAL)`；`:674 qd.get_summary_for_tab(tab)`。[观察]
- → **差异在形态与数量**：原生 4 个「最近更新/最近查看/适合等级/热门」智能分类 vs 当前端 3 个静态页签；页签视觉走「贴图优先 + ColorBox 程序兜底」而非纯贴图。属实现路径差异（s9 服务端是否下发这 4 类数据未知），改回原生须裁决，不判纯缺失。[观察+推断，s9 适配]

### 8.5 本节结论

- 当前端核心窗口的缺口形态与登录面/HUD 面**完全一致**：不是「窗没做」，而是「显式预留（按截图/包内证据不足，不发明协议）+ stub 占位文案 + 拆分重写 + 几何按实机截图校准」。
- **§8 可零风险先改项**：无（全部是显式预留或 s9 适配，须裁决）；**§8 可零风险先改的文档一致性项**：§8.1 `ui-v83-window-restore/PLAN.md:20,34` 记的 `UIToolTip.img/Item` 节点引用与当前端实际用的 `UIWindow.img/ToolTip/Equip` 不符，须复核该 PLAN 是否已过时（交 README 决策点）。
- **§8 须裁决项**：§8.2 宏窗几何（改回原生 5×3/45 行距会破坏截图对齐）、§8.3 任务窗 4 按钮是否补 npc→map 反查表（需服务端数据）。
