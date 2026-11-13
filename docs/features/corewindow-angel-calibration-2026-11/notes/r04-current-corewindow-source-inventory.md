# r04 · 核心窗口面 — 当前端源码现状盘点（第一路 subagent `18924e1b` 全文归档）

> 来源：只读子代理 `18924e1b-152b-4261-af34-2931ce340f2c` 于 2026-11-08 20:28 发回的 21K「核心窗口·当前端源码盘点」。子代理声明：已加载 `xiaoye-maple-story-dev` 门禁、全程只读、未改任何文件、每条带相对路径:行号并标 [观察]/[推断]；凡带 095/Maplestory95 字样的注释仅作现状罗列、不作 083 证据。术语按会话约定 F=学院(FAMILY)、G=家族(GUILD/公会，UIUserList 第 3 页签)。
> 主 agent 归档说明：本文为可 grep 的结构化固化，供 README 六维对照表引用；所有行号以子代理直读为准，与 r02 冲突处以本文为准并已在文末 §9 标注。

---

## §0 主 agent 前置复核块（把本文与 r01/r02/r03 的增量钉死）

- **★ 095 文档污染范围扩大（MEMORY.md critical 硬约束相关）**：除 r02 §5 已记的 `IO/UITypes/UIStatsInfo.cpp:146 // BtAuto：Maplestory95 CUIStat::OnCreate(0x867b90)` 外，本文新发现 4 处 095 线注释：`IO/UITypes/UIItemInventory.h:49-50 // 对齐原生 CUIItem::SetArrangeButton(nTI, nState)（Maplestory95 @ 0x7CCC90）。`、`IO/UITypes/UIItemInventory.cpp:124 // 证据：Maplestory95 CUIItem::OnCreate @ 0x7CE520 将 m_anSortBtState[0..5]=0 后`、`:530-532 // 原生 CUIItem::OnButtonClicked @ 0x7CC4F0：0x7D3 → OnGather / 0x7D4 → OnSort`、`Net/Handlers/InventoryHandlers.cpp:56/:71`（Decode1→CUIItem::SetArrangeButton 095 线引用）。→ **这些地址全部是 095 二进制，不得作 083 判定依据**；应改引 Angel 083 的 `CUIStat::OnCreate @ 0x8c4c7f` / `CUIItem::OnChildNotify @ 0x81d01f` / `CUIItem::OnCreate`（r01 §2）。坐标结论与截图实测一致可保留、只换依据。[观察+推断]
- **★ 一一对应拆分结论（本文 §8 ①，是 README 结构性差异的核心证据）**：当前端**不是**把 Angel 62 类一一拆成独立 UI 类，而是「按功能合并 + 自命名」重构 = 12 个独立窗 + `UIMiscShells` 聚合 20+ 壳 + `IO/Components` 5 tooltip 组件 + 单一 `EnumMap<Type,...>` 调度（`IO/UIElement.h:203 NUM_TYPES`）。全库 grep Angel 原生类名仅 **8 处命中且全是注释/095 引用**，无任何 `class CUISkill/.../CCashShop/...` 定义。[观察+推断]
- **★ 结构性收缩 5 个硬锚点（本文 §8 ②）**：`CUIMacroSys 32 格 → Character/SkillMacros.h:23 MAX_SLOTS = 5`；`CUIToolTip 26 方法 → 5 组件（Tooltip.h:46 仅 1 纯虚 draw）`；`CCashShop 66 方法 → UICashShop.h:38-51 仅 11 override`；`CWorldMapDlg 666×524 → 当前端无对应整机尺寸常量（UIMapInfoPanel.cpp:58-94 只有 240×490 子面板）`；`CUIStat 10 case → UIStatsInfo.cpp:638-697 仅 8 有效 case`。→ 均为「形态/数量差异」而非纯缺失，须交决策点裁决，符合三面一致的「校准 ≠ 改回原生」。[观察+推断]
- 与 r02 §8.4b「CashShop 不是 stub 窗」不冲突：本文 §1.1 也列出 `UICashShop` 22 钮枚举 + 深度协议，stub 仅指 `:100/:102` 两个 Text。[观察]

---

## §1 核心窗口类清单

### §1.1 有独立 .h/.cpp 的核心窗口（12 个）

| 窗口 | 类/TYPE（行号） | 尺寸/常量 | 按钮枚举 | draw / button_pressed / send_key |
|---|---|---|---|---|
| UIStatsInfo | `UIStatsInfo.h:27 class UIStatsInfo : public UIDragElement<PosSTATS>`；`:30 TYPE=STATSINFO`、`:31-32 FOCUSED/TOGGLED` | `.h:50-52 INK_TOP_NONE=-1` 墨迹哨兵（退回 statoffsets 原始 y）；`.h:149-153`「v83 经典布局（Stat 无 main 子节点）：backgrnd2 详细板 (208,144)、basicStat (8,195)、Disabled{STR..LUK} 48x16」；`.cpp:59 PosSTATS(212,150)`、`:358 dimension=(212,318)` | `.h:113-129` BT_CLOSE/BT_HP/BT_MP/BT_STR/BT_DEX/BT_INT/BT_LUK/BT_AUTO/BT_HYPERSTATOPEN/CLOSE/BT_DETAILOPEN/CLOSE/BT_ABILITY/DETAIL_DETAILCLOSE；`.h:133-141 Ability{RARE..LEGENDARY,NONE}`；`.h:54-103 StatLabel` Normal 16 + Detailed 20 + classic 5 | `.cpp:364 draw`；`:638-697 button_pressed`（`:644-648 BT_CLOSE→deactivate`、`:649-678 六项→send_apup(MapleStat::Id::X)`、`:679-697 BT_AUTO` 按 AP + get_primary(weapontype) 算 autostr/dex/int/luk）；`:480 send_key`；`:726-752 AutoAssignApPacket(to_code(primary), ap, second_code, 0).dispatch()`；`:814` classic 详情钮注释 |
| UIItemInventory | `UIItemInventory.h:27 : public UIDragElement<PosINV>`；`:30 TYPE=ITEMINVENTORY` | `.cpp:1774 PAGE_COLS=4`、`:1775 PAGE_ROWS=6`、`:1858 dimension=full_enabled?bg_full_dimensions:bg_dimensions`、`:1896 dragarea=(dimension.x(),20)`；`.h:181-183 slot_labels[128]`；`.h:140-141`「v83 tab 底衬 = New/Tab0(gray)/Tab1(pink) plate」 | `.h:103-133` BT_CLOSE/BT_TAB_EQUIP/USE/ETC/SETUP/CASH/DEC/BT_COIN/BT_POINT/BT_GATHER/BT_SORT/BT_FULL/BT_SMALL/BT_UPGRADE/BT_APPRAISE/BT_EXTRACT/BT_DISASSEMBLE/BT_TOAD + _SM + BT_CASHSHOP；`.h:167-168`「每页签整理钮状态：false=Gather，true=Sort（原生 m_anSortBtState[nTI]）」 | `.cpp:333-377 draw`（`:345-346 meso_y=mesolabel.y_for_ink_top(269)` + `mesolabel_pos=position+(137,meso_y)`、`:348-355 full 三底图`、`:356-365 折叠三底图+slider`、`:367-377 slotrange/numslots/icons`）；`:488-532 button_pressed`（`:494-499 BT_CLOSE`、`:500-529 六 tab`）；`:1144 send_key`；`:577 emplace<UIMaker>()`、`:781 emplace<UIEnchantSkill>()`、`:877 emplace<UICashGachapon>()` |
| UIEquipInventory | `UIEquipInventory.h:28 : public UIDragElement<PosEQINV>`；`:31 TYPE=EQUIPINVENTORY` | `.cpp:191-203 base/expanded_dimension/bg_dimensions/dragarea`、`:606 dimension=pet_panel_open?expanded:base`；`.h:102-107 TAMING_SKILL=1004 / TINTABLE_SLOTS={TAMEDMOB,SADDLE,SHIELD}`；`.h:117-125 宠物面板字段` | `.h:84-95 BT_CLOSE/BT_SLOT/BT_SHOP/BT_PET_SHOW/BT_PET_HIDE/BT_PET_TAB0/1/2/BT_PET_TABE` | `.cpp:209-257 draw`（`:214-215 pet_backgrnd`、`:217 draw_buttons`、`:219-238 TINTABLE_SLOTS 不可用槽改色 disabled_cells[i].draw(position-bg_origin+(ip.x(),ip.y()+3))`、`:240-256 icons set_cell(ip)`）；`:259-298 button_pressed`（`:263-265 BT_CLOSE`、`:266-269 BT_SHOP→EnterCashShopPacket()`、`:270-298 BT_SLOT→UseCashItemBasicPacket(slot,itemid)` 或 SoftKey）；`:388 send_key` |
| UISkillBook | `UISkillBook.h:34 : public UIDragElement<PosSKILL>`；`:37 TYPE=SKILLBOOK` | `.h:166-216 ROWS=12 / ROW_HEIGHT=40 / ROW_WIDTH=143 / SKILL_OFFSET(11,93) / CLASSIC_SKILL_OFFSET(10,87) / CLASSIC_ROWS=4 / CLASSIC_SPUP_OFFSET(130,20) / TAB_PLATE_TOPLEFT(4,23) / TAB_PLATE_PITCH=34 / BOOK_TOPLEFT(11,57) / MACRO_BTN_TOPLEFT(121,265) / CLASSIC_SLIDER_X=158 / TOP=87 / BOTTOM=258`；`.h:170-172`「原生经典窗单列技能行 y=87..257 行高 40 可见 4 行，行左缘 x=10」；`.h:251-253`「v83 经典 Skill 无 main，backgrnd 175x289」 | `.h:134-164` BT_CLOSE/BT_HYPER/BT_GUILDSKILL/BT_RIDE/BT_MACRO/BT_MACRO_OK/BT_CANCLE/BT_OKAY/BT_SPDOWN/BT_SPMAX/BT_SPUP/BT_TAB0..4/BT_SPUP0..11 | `.cpp:350-394 draw`（`:354 if(classic)`、`:375 splabel.draw(position+(109,y_for_ink_top(268)))`、`:388-389 单列技能行 (10,87) 行距 40 可见 4 行`）；`:515-594 button_pressed`（`:517 cur_sp=stoi(splabel)`、`:521 BT_CLOSE`、`:524-543 BT_MACRO→emplace<UISkillMacroEx>()` + `anchor_right_of(*this,-3)`、`:544 BT_CANCLE`、`:547-560 BT_OKAY spend_sp 循环+change_sp()`、`:561-583 BT_SPDOWN`、`:584 BT_SPMAX`）；`:797 send_key`；`:1105` 注「`UIWindow.img/Skill/skillPoint` 本包 WZ 中缺失」 |
| UIQuestLog | `UIQuestLog.h:27 : public UIDragElement<PosQUEST>`；`:30 TYPE=QUESTLOG` | `.h:161-180 Slider/list_area/detail_area/slot positions`；`.h:138-142 ColorBox tab/row highlight`；`.h:144-147 gauge textures/origin`；`.h:74-80 struct Row`；`.cpp:50-53 tab_x{2,66,131}/tab_w{64,65,65}/tab_y(22)/tab_height(18)` 3 页签 | `.h:58-72 TAB0/TAB1/TAB2/CLOSE/COLLAPSE/MARKNPC/DETAIL/GIVEUP/OK/NO/ALERT/COUNT` | `.cpp:237 draw`、`:343 draw_chain_page_gauge`、`:395 draw_gauge`、`:443 send_key`；`:926-964 button_pressed`（`:930-934 TAB0/1/2→change_tab`、`:935-937 CLOSE`、`:938-940 COLLAPSE`、`:941-945 DETAIL`、`:946-950 GIVEUP→QuestActionPacket(3,selected_id).dispatch()`、`:951-960 MARKNPC/OK/NO/ALERT 全 return NORMAL`）；`:966-1000 row_index_at/send_cursor` |
| UIWorldMap | `UIWorldMap.h:45 : public UIDragElement<PosMAP>`；`:48 TYPE=WORLDMAP` | `.h:82 MAPSPOT_TYPE_MAX=4u`、`:85-91 SideState 4 态`、`:95-96 TAB_W=26/TAB_H=92`、`:103 SIDE_EXPAND_MS=220.0f`、`:216-221 三子面板 UIMapMobPanel/UIMapSearchPanel/UIMapInfoPanel + kUseMapInfoPanel=true`、`:37-43 struct WorldMapPlayer + 注 s9 WORLD_MAP_PLAYERS(0x178)`；`UIMapInfoPanel.cpp:58-94 UPPER 189x237 / LOWER 203x174 / GRID 5x5 / SLOT=32 / TAB_NAMES[4]={all,equip,consume,etc}` | `.h:137-155 BT_CLOSE/BT_SEARCH/BT_AUTOFLY/BT_NAVIREG/BT_ALLSEARCH/BT_SEARCH_CLOSE/BT_LINK0..9` | `.cpp:191-230 draw`（`:193-200 三面板`、`:211-212 底图 origin (320,235) 补偿 base_img.draw(position+base_position+origin)`、`:216-217 draw_tab`、`:219-230 link_images hover`）、`:902 draw_tab`、`:378 send_key`、`:504 button_pressed` |
| UIKeyConfig | `UIKeyConfig.h:28 : public UIDragElement<PosKEYCONFIG>`；`:31 TYPE=KEYCONFIG` | `.cpp:78 dimension=(0,0)`、`:123-124 dimension=bg_dimensions; dragarea=(bg_dimensions.x(),20)`、`:334-337 row_x=9/row_y0=267/slot=34/cols=17`；`.h:134-179 alternate_keys`、`:181-229 basic_keys`（Q→QUESTLOG/W→WORLDMAP/E→EQUIPMENT/I→ITEMS/P→PARTY/S→STATS/G→GUILD/K→SKILLS/SEMICOLON→MEDALS） | `.h:81-89 CLOSE/CANCEL/DEFAULT/DELETE/KEYSETTING/OK`；`.h:91-107 class KeyIcon : public Icon::Type` | `.cpp:599 draw`、`:718-757 button_pressed`（`:722-728 CLOSE/CANCEL`、`:729-756 DEFAULT→硬编码英文 "Would you like to revert to default settings?" + staged_mappings=alternate_keys/basic_keys + bind_staged_action_keys()`、`:757 DELETE`）、`:911 send_key`；`:138 // BeiDou v83：IDA CUIKeyConfig 快捷键位表（dword_BE27E0，32x32 槽）`、`:649` 命中格显式算偏移 |
| UICashShop | `UICashShop.h:31 : public UIElement`；`:34 TYPE=CASHSHOP`、`:35 FOCUSED=true`、`:36 TOGGLED=false` | `.h:56 MAX_ITEMS=7*2+1=15`、`:58-116 Item + enum Label(ACTION..WONDER_BERRY/WORLD_SALE/NONE)`、`:142-178 BestNew_dim/preview_sprites[3]/menu_tabs[9]/promotion/mvp_sprites[7]/mvp_gauge/charge_charset/items[MAX_ITEMS]/list_slider/list_offset`、`.cpp:197 dimension=Texture(backgrnd).get_dimensions()` | `.h:118-140` BtPreview1..3/BtExit/BtChargeNX/BtChargeRefresh/BtWish/BtMileage/BtHelp/BtCoupon/BtNext/BtPrev/BtDetailPackage/BtNonGrade/BtBuyAvatar/BtDefaultAvatar/BtInventory/BtSaveAvatar/BtTakeoffAvatar/BtBuy（22 钮） | `.cpp:203 draw`、`:294-343 button_pressed`（`:305-330 BtExit→Setting<Width/Height> + fadestep=0.04f + ChangeMapPacket().dispatch() + GraphicsGL::clear + Stage::clear`、`:331-341 BtNext/BtPrev promotion_index`）、`:656 send_key`、`:762/:766/:791 BtWish/BtMileage/BtDetailPackage`、`:435-437 emplace<UIItemInventory>`、`:849 emplace<UICashGachapon>()`；`:100/:102 bind_text(job_label,"cashshop.stub_job") / bind_text(name_label,"cashshop.stub_name")` |
| UICharInfo | `UICharInfo.h:26 : public UIDragElement<PosCHARINFO>`；`:29 TYPE=CHARINFO` | `.cpp:63 dimension=(0,0)`、`:209-210 dimension=backgrnd_dim; dragarea=(dimension.x(),20)`；`.h:78-127 Main Window texts / target_character/target_cid / Sub Windows / Farm / Personality / Collect / Damage / Item 子窗字段` | `.h:55-76` BtClose/BtCollect/BtDamage/BtFamily/BtItem/BtParty/BtPersonality/BtPet/BtPopDown/BtPopUp/BtRide/BtTrad/BtFriend/BtVisit/BtArrayGet/BtArrayName/BtFAQ/BtRegist/BtBook | `.cpp:221 draw`、`:322-371 button_pressed`（`:332-345 BtFamily→FamilyAddPacket(n)/OpenFamilyPacket() + emplace<UIFamily>()`、`:346-354 BtParty→PartyInvitePacket(n)`、`:355-360 BtItem`、`:361-366 BtBook→emplace<UIMonsterBook>()`、`:367-371 BtCollect/Personality/Ride/Pet/Damage`）、`:530 send_key`；`:171 medal_text=Locale::text("charinfo.medal_stub")`、`:452 UseCashItemBasicPacket(slot,itemid).dispatch()`、`:460 // 皮肤切换与资源装载按证据不足显式预留，不凭空造节点` |
| UIUserList | `UIUserList.h:27 : public UIDragElement<PosUSERLIST>`；`:30 TYPE=USERLIST` | `.h:45-58 enum Tab{FRIEND,PARTY,GUILD,BLACKLIST,PARTY_MINE,PARTY_SEARCH,FRIEND_ALL,FRIEND_ONLINE,UNION}`；`.h:83-144 长 Buttons 枚举`；`.h:176-179`「Guild tab（家族页 = 083 原版『玩家列表 & 家族』共用窗第 3 页签，数据源 GuildModel，非 UIGuild 浮窗实例）」 | `.h:55-56`「顶层第 4 格家族联盟，页签→按钮 id 走 tab_button() 显式映射」、`:109-123 家族页钮语义 + 无联盟时画 disabled`、`:124-125 2026-11-08 追加运行时 UIWindow.img/UserList 节点绑定` | `.cpp:317 draw`、`:447 draw_guild_tab`、`:1479 draw_friend_tab`、`:1498 draw_party_tab`、`:1516 draw_blacklist_tab`、`:1529 draw_union_tab`、`:519 send_key`、`:606 button_pressed`、`:951/:954/:1007/:1092 分支`；`.h:40`「家族页成员行不是按钮，须光标事件自行命中」、`.cpp:324`「title/base 不存在，恒为空，只剩硬编码 Text("none")」、`:916 // v83 家族聊天由聊天框目标/斜杠别名路由；change_target 仍是预留空实现`、`:1131` 页签 id 非线性、`:1282` 只接受四态按钮容器 |
| UIMaker | `UIMaker.h:35-36 // 制造/工匠窗口：从 Maker.img 列出配方（可滚动），点击配方行发送创建请求。` + `class UIMaker : public UIElement`；`:39 TYPE=MAKER` | `.cpp:39-44 title Text("Maker") / status "No maker recipes available." / dimension=(380,460)`、`:61 list_origin=(24,86)`；`.h:55-60 struct Recipe{itemId,meso,reqLevel}`、`:69-80 Sprite background / has_background / scroll / row_h=56 / list_origin / selected_item_id` | `.h:64-67 enum Buttons { BtClose }`（仅 1 钮） | `.cpp:135-161 draw`（`visible=(dimension.y()-list_origin.y()-16)/row_h` + icons/names/costs）、`:163-173 button_pressed`（仅 BtClose，其余 DISABLED）、`:175-192 send_cursor`（命中行→`MakerCreatePacket(selected_item_id).dispatch()`）、`:194-207 send_scroll`、`:209-251 send_key`（ESC/C 制造/R load_recipes()+"Recipes reloaded"/S/T SoftKey）；`:91 // 与参考端(BeiDou.exe v83)一致：配方来自本地 Maker.img`、`:92 nl::nx::Maker["recipe"]`、`:98-100 recipe["targetItem"/"cost"/"reqLevel"]`、`:107 // 兜底：内嵌 s9 makercreatedata 静态表` |
| UICashGachapon | `UICashGachapon.h:7 : public UIDragElement<PosCASHGACHAPON>`；`:9 TYPE=CASHGACHAPON`、全文件仅 25 行 | `.cpp:29 position=(340,160); dimension=(170,172); dragarea=(170,20)`、`:16-24 find_gacha_ticket 扫 CASH 1..96 且 id/10000==522`、`:31 root=UiAssetResolve::window("CashGachapon")` | `.h:21 enum Buttons { BtClose, BtOpen }`（2 钮） | `.cpp:54-61 draw`（`if(!has_assets) return` + backgrnd + draw_buttons + status）、`:62-71 send_key`（ESC / 'O'||'o'||13 / 'X'||'x'||'C'||'c'）、`:73-88 button_pressed`（BtOpen→emplace<UISoftKey> + `UseCashItemBasicPacket(slot,itemid).dispatch()`） |

### §1.2 无独立文件、合并进 UIMiscShells 的壳类（判据：构造仅 position/dimension/dragarea + load_window_bg + draw 仅 backgrnd + tip_label(12,40)，非原生 441 函数级复刻）[观察/推断]

全部在 `IO/UITypes/UIMiscShells.h`（声明）+ `UIMiscShells.cpp`（实现）：

- **UISkillEx** `.h:554-564 TYPE=SKILLEX(:556)`；`.cpp:3522-3555` `position(280,100); dimension(200,300); dragarea(200,20)` + `load_window_bg("SkillEx")` 回退 "SkillUp" + 读 `UISkillBook::get_selected_skill_id()` → `UseSkillPacket(id,lv).dispatch()`；draw=backgrnd+tip_label(12,40)；send_key `:3556`。
- **UIMacro** `.h:822-832 TYPE=MACROUI(:824)`；`.cpp:6674-6704` `position(280,120); dimension(289,164)` + `load_window_bg("Macro")` 回退 "MacroSkill" + `:6683 // 同 UISkillMacroEx：构造时不弹安全键盘，改名走 send_key 的 S/R 分支`；draw=backgrnd+tip_label；send_key `:6690`（S/R→`SkillMacros::get().get_slot(0)` 改名→`ChangeSkillMacroPacket().dispatch()`）。
- **UIMacroSkill** `.h:903-913 TYPE=MACROSKILL(:905)`；send_key `.cpp:7166`。
- **UISkillMacroEx**（相对最完整）`.h:643-669 TYPE=SKILLMACROEX(:645)`、`:647 // 原生技能组合栏一屏三行（Macroicon/0..2 逐像素命中，行距 45）`、`:648 VISIBLE_ROWS=3`、`:649 // SkillMacros::MAX_SLOTS 的镜像`、`:650 MACRO_COUNT=5`、`:657-669 shell_ok/backgrnd/macroslot/macroname/line02/macro_icon[5]/macro_icon_sel[5]/Slider slider/offset/selected/name_label/shown_name`；send_key `.cpp:4934`。
- **UISkillUp** `.h:890-902 TYPE=SKILLUP(:892)`；send_key `.cpp:7112`；`Net/Handlers/PlayerHandlers.cpp:303 // UISkillUp 保留为手动调试工具`。
- **UIEnchantSkill**（Angel CUIEnchantDlg 语义位）`.h:97-110 class UIEnchantSkill : public UIDragElement<PosNOTICE>`、`:99 TYPE=ENCHANTSKILL`、FOCUSED=false/TOGGLED=true、字段 effect/tip_label/equip_slot/scroll_slot（**无 Buttons 枚举**）；send_key `.cpp:745`。
- **UIQuestAlarm**（Angel CUIQuestAlarm 语义位）`.h:576-588 class UIQuestAlarm : public UIElement`、`:578 TYPE=QUESTALARM`、FOCUSED/TOGGLED=false、update_screen/set_tip；send_key `.cpp:3690`。
- 其它同壳：UIQuestIcon `.h:914-924 TYPE=QUESTICON(:916)` / send_key `.cpp:7251`；UIBook `.h:565-575 TYPE=BOOKUI(:567)` / `.cpp:3642`；UIShortCut `.h:622-641 TYPE=SHORTCUTUI(:624)` / `:634 // UIWindow.img/ShortCut 的 8 颗真按钮（此前一个都没实例化 → 「图标缺失」）` / `:637-640 BT_ITEM/EQUIP/STAT/SKILL/COMM/QUEST/MOBBOOK/MESSENGER` / button_pressed `.cpp:3901`；UIGameMenu `.h:802-821 TYPE=GAMEMENU(:804)` / `:813-814 // UIWindow.img/GameMenu 的 4 颗真按钮…此前只画底板 + 一行英文按键提示，等于把开发用的调试壳暴露给了玩家` / `:817-819 BT_CHANNEL/GAMEOPT/SYSOPT/QUIT` / button_pressed `.cpp:5795` / send_key `:5823` / `:6660-6672 TAIL_CX=46`；UIMobGage `.h:879 TYPE=MOBGAGE` / send_key `.cpp:7039`；UIFloatNotice `.h:951 TYPE=FLOATNOTICE` / `.cpp:7477`；UIWhisperTargetDlg `.h:972 TYPE=WHISPERTARGET` / Basic.img/Notice4 t/c/s 竖拼、截帧 261x117、FRAME_H=21+22+78 / send_key `.cpp:7814` / button_pressed `:7836`。

### §1.3 明确「未找到」的独立文件 [观察]
`glob IO/UITypes/UIToolTip.*` → **No files found**（复核两次）；`UISkill.*`、`UISkillEx.*`、`UIMacro.*`、`UIQuestAlarm.*`、`UIEnchant.*`、`UIItemMaker.*`、`UIQuestIcon.*`、`UIUserInfo.*` 均无独立 .h/.cpp。Tooltip 以 `IO/Components/` 5 组件替代（§4）；CUIUserInfo 语义由 `UICharInfo` 承接 [推断]。

---

## §2 装配入口（IO/UIStateGame.cpp + IO/UIElement.h）

### §2.1 构造期 emplace `IO/UIStateGame.cpp:70-107`
- `:70 ctor : stats(...), dragged(nullptr)`、`:72 focused=NONE`、`:73 tooltipparent=Tooltip::Parent::NONE`。
- HUD 类（非本面）：`:78 UIStatusMessenger`、`:83 UIStatusBar`、`:84 UIChatBar`（`:81-89 has_game_hud 守卫`）、`:91 UIMiniMap`、`:92 UIBuffList`、`:93 UITemporaryStatView`、`:96 UIEnergyBar`、`:97 UIComboCounter`、`:98 UIBossHP`、`:99 UIClock`、`:104 UIShop`。

### §2.2 send_key MENU action 分支（本面窗口，逐行）
- `:319-327 EQUIPMENT→emplace<UIEquipInventory>`；`:328-336 ITEMS→emplace<UIItemInventory>`；`:337-345 STATS→emplace<UIStatsInfo>`；`:346-355 SKILLS→emplace<UISkillBook>(stats,skills)`。
- `:356-396 FRIENDS/PARTY/BOSSPARTY/GUILD 共用 UIUserList`；`:361-364` 逐字「v83 G(17) = GUILD『家族』：用户 2026-11-07 指正，原生是『好友/组员/家族/家族联盟/拒绝列表』共用窗的第 3 页签，不是独立 UIGuild 浮窗（浮窗仍由底栏社区钮保留入口）」；`:368-377 映射`；`:380-393 获取/change_tab/emplace`。
- `:397-402 WORLDMAP→emplace<UIWorldMap>()`；`:436-444 QUESTLOG→emplace<UIQuestLog>(quests)`；`:445-470 KEYBINDINGS→emplace<UIKeyConfig>(inventory,skills)` 或 close()；`:513-517 CASHSHOP→EnterCashShopPacket().dispatch()`；`:524-529 PROFESSION→emplace<UIMaker>()`；`:536-544 CHARINFO→emplace<UICharInfo>(oid)`。
- `:269-272` 逐字「§f-keymap 遗留五死键接线（2026-11-06）…v83 的 11/17/25/26 透传后落在 WHISPER/GUILD/MANAGELEGION(MEDALS槽)/MEDALS(FAMILY槽)，原先无 case ⇒ 键发了、没反应。窗口本体全部已存在」；`:296-309 MEDALS（v83 F=学院）OpenFamilyPacket().dispatch() + emplace<UIFamily>()`；`:289-295 MANAGELEGION（v83 O=组队搜索）emplace<UIPartySearch>()`；`:310-318 NOTIFIER（v83 L）:314 // = notifier 列表封包 —— 服务端不存在 ⇒ 按门禁 7 不做假窗`、`:316 LOG(LOG_DEBUG, "UIStateGame: NOTIFIER key (v83 L) — no s9 notifier backend, intentionally inert")`；`:530-535 EVENT→emplace<UISpecial>()`、`:532 // 095 自定义中文 UI：暂时复用 EVENT 键打开福利中心`（095 线，不作 083 证据）。

### §2.3 tooltip 入口 `:729-797`
`:729 clear_tooltip` 清 eqtooltip/ittooltip/tetooltip/matooltip；show_equip/show_item/show_skill/show_text/show_map 分别设置各 tooltip + tooltipparent；draw `:120-121` tooltip 绘于 cursor+(0,22)。

### §2.4 emplace 模板 / focus / order `:799-919`
`:799-832` emplace 以 `T::TYPE, T::TOGGLED, T::FOCUSED` 调 pre_add，非 silent_types 播 MENUUP/WORLDMAPOPEN + `send_cursor(false)`；silent_types `:808-820`；`:834-887 pre_add` toggle/remove/order/focus；`:889-904 remove`；`:906-919 bring_to_front`。

### §2.5 UIElement::Type 枚举 `IO/UIElement.h:41-204`（本面相关逐值）
`:65 CASHGACHAPON`、`:67 FAMILY`、`:98 ENCHANTSKILL`、`:123 SKILLEX`、`:124 BOOKUI`、`:125 QUESTALARM`、`:130 SKILLMACROEX`、`:145 MACROUI`、`:148 SKILLUP`、`:149 MACROSKILL`、`:150 QUESTICON`、`:162 STATSINFO`、`:163 ITEMINVENTORY`、`:164 EQUIPINVENTORY`、`:165 SKILLBOOK`、`:166 QUESTLOG`、`:167 WORLDMAP`、`:168 USERLIST`、`:176 KEYCONFIG`、`:180 CHARINFO`、`:181 CASHSHOP`、`:186 GUILD`、`:193 MAKER`、`:200 GAMEOPT`、`:201 SYSOPT`、`:202 SPECIALUI`、`:203 NUM_TYPES`。
`:195-199` 逐字「v83 目录气球的两颗独立设置窗（游戏设置/系统设置）。本包无 StatusBar3.img/OptionMenu，UIOptionMenu 只剩空壳，故另立两型。注意：这两项让 UIElement::Type::NUM_TYPES 变大，而 EnumMap<Type, UPtr, NUM_TYPES> 是 UIStateGame 的成员 —— 实例尺寸变了，必须整工程 Rebuild（增量编译会留下按旧尺寸编译的目标文件）」。
基类虚方法 `:208 draw`、`:209 update`、`:211 update_screen`、`:240 virtual Button::State button_pressed(uint16_t) { return DISABLED; }`、`:245 is_in_range`、`:247 send_cursor`、`:248 send_scroll`、`:249 send_key`、`:251 get_type() 纯虚`；成员 `:265 buttons/:266 sprites/:267 position/:268 dimension/:269 active`。

### §2.6 其它装配点
`IO/UIStateCashShop.cpp:39 emplace<UICashShop>()`；`Gameplay/MapleMap/MapFieldMarkers.cpp:43-44 emplace<UIQuestAlarm>()`；`IO/UITypes/UICashGachapon.cpp` 由 `Net/Handlers/WaveAShellHandlers.cpp:245-246`、`IO/UITypes/UICashShop.cpp:849`、`IO/UITypes/UIItemInventory.cpp:877` 打开。

---

## §3 协议 / 数据 / Character

### §3.1 发包类 `Net/Packets/`
- `PlayerPackets.h:91/94/97 SpendSpPacket / SPEND_SP / write_int(skill_id)`；`:32 SpendApPacket`、`:35 SPEND_AP`、`:51 AutoAssignApPacket`、`:54 enum StatCode : int32_t`、`:63 AUTO_DISTRIBUTE_AP`；`:103/:106/:119 ChangeKeyMapPacket / ctor vector<tuple<KeyConfig::Key,KeyType::Id,int32_t>> / write_int(KeyAction::to_server(...))`。
- `SkillMacroPackets.h:2 // SKILL_MACRO(110) — save macros (s9 SkillMacroHandler)`、`:13 ChangeSkillMacroPacket`、`:16 Opcode::SKILL_MACRO`、`:19-21 钳位 SkillMacros::MAX_SLOTS`、`:25-29 write_string(s.name); write_byte(s.shout); write_int(s.skill1..skill3)`；`:34-39 // Opcode: CHANGE_QUICKSLOT(183 / 0xB7) … 改键 UI 接线待 v83 KeyConfig 快捷页实体化（Wave 6），本包类先行登记防重复造`、`:40 ChangeQuickslotPacket`、`:43-48 8× int`。
- `QuestPackets.h:2 // BeiDou QUEST_ACTION = 0x6B`、`:10-11 QUEST_ACTION(107) / Layout: byte action, short questid [, int npcid]`、`:12 QuestActionPacket`、`:17-18 write_byte(action); write_short(questid)`、`:20-22 // 1=start, 2=complete, 4/5=scripted need npc; 3=forfeit no npc`。
- `MakerPackets.h:26 // MAKER_SKILL(0x71) 对接 s9 MakerProcessor(type=1 创建)`、`:27 MakerCreatePacket`、`:32-33 write_int(1); write_int(toCreate)`、`:35-40 // 装备类配方服务端还会读 stimulant + reagents；此处占位 + if((toCreate/1000000)==1){ write_byte(0); write_int(0); }`。
- `GameplayPackets.h:43/47 EnterCashShopPacket / Opcode::ENTER_CASHSHOP`；`CashShopPackets.h:30/43/58/73/84/97/110/124/138/162/176/183` Buy(0x03)/Wish(0x05)/Gift(0x04)/Take(0x0D)/Put(0x0E)/Package(0x1E)/InvSlot(0x06)/InvSlotSn/StorageSlot(0x07)/CharSlot(0x08)/CheckCash/CubeReroll(0x40)，均 `Opcode::CASHSHOP_OPERATION`。
- `CashItemPackets.h:13/28/44/56/69/82/96/111/132` UseViciousHammer/UseVegaSpell/UseItemProtector/UseKarmaScissors/UseTeleportRock/UseTeleportRockToPlayer/UseMegaphone/UseItemMegaphone/CouponCode（均 `Opcode::USE_CASH_ITEM`）。
- `InventoryPackets.h:29/41/53/68/76/110/156/:159/:166` GatherItemsPacket/SortItemsPacket/MoveItemPacket/EquipItemPacket/UnequipItemPacket/UseItemPacket（`:137-143 USE_SUMMON_BAG/USE_SKILL_BOOK/PET_FOOD/USE_MOUNT_FOOD/SCRIPTED_ITEM/USE_RETURN_SCROLL/USE_ITEM 动态分派`）/ScrollEquipPacket。
- `AttackAndSkillPackets.h:32 AttackPacket`、`:41 // 显式 opcode 重载：TOUCH_MONSTER_ATTACK(47) 与 CLOSE_ATTACK 同 body`、`:287 UseSkillPacket`、`:299 skip(2); // TODO: No idea what this could be`、`:301 // TODO: A point (4 bytes) could be added at the end`、`:309 SkillEffectPacket`。
- `FamilyPackets.h:10/16/22/31/43/54/67/80/91` OpenFamily/OpenFamilyPedigree/FamilyAdd/Accept/Precept/SummonResponse/Use/SeparateSenior/SeparateJunior。
- 全局预留：`GameplayPackets.h:64 // 预留：那 4 字节在原生 v83 客户端里的**语义**未取证（IDA 未定位到构造点）`；`Net/OutPacket.h:42 //（RELOG=0x1C 已裁决"明确不做"，不占枚举位`、`:203 /// 实体化（Wave 6 素材线），本枚举+包类先行登记，暂不接线`。

### §3.2 Handler `Net/Handlers/`
`QuestHandlers.h:10-11 UPDATE_QUEST_INFO(211)`；`QuestHandlers.cpp:49-50 emplace<UIQuestAlarm>()`；`PlayerHandlers.h:81-82 UPDATE_SKILL(36) / :88-89 SKILL_MACROS(124)`；`PlayerHandlers.cpp:383/397/415 kc->makeactive()`、`:139 // 实际什么都不做」的占位包（PacketCreator.java:4573），没有可信布局可依`、`:303 // UISkillUp 保留为手动调试工具`。
`CashShopHandlers.h:25/36/43/52/59/66/73` SetCashShop/QueryCashResult/CashShopOperation/CashItemGachaponResult/CashShopCheckNameChange/NameChangePossible/TransferWorldPossible。
`InventoryHandlers.h:25/31/37/43/49` GatherResult/SortResult/ModifyInventory/InventoryGrow/SkillLearnItemResult；`InventoryHandlers.cpp:56 // Decode1(); ti=Decode1(); CUIItem::SetArrangeButton(ui, ti, 1) → 显示 BtSort`、`:71 // ... → 显示 BtGather`（095 线引用，仅罗列）。
`WaveAShellHandlers.cpp:239-246 CashGachaponOpenHandler→emplace<UICashGachapon>()`、`:1253-1270 批量 emplace UIItemInventory/UIEquipInventory/UISkillBook/UIKeyConfig/UIStatsInfo/UIQuestLog`、`:1291/:1369/:2854 emplace<UIMaker>`、`:1397 emplace<UICharInfo>`、`:2144/:4227 emplace<UIQuestLog>`、`:4372 emplace<UISkillUp>()`；`WaveAShellHandlers.h:587 MakerResultHandler / :437 QuestClearHandler / :857 SetQuestClearHandler / :863 SetQuestTimeHandler`。
`Net/PacketSwitch.cpp:129 SHOW_ITEM_GAIN_INCHAT = 206, // TODO: Rename this (Terribly named)`、`:465 emplace<CASHSHOP_CASH_GACHAPON_OPEN_RESULT, CashGachaponOpenHandler>()`。
`MessagingHandlers.cpp:189/856 emplace<UIQuestAlarm>、:302/909 emplace<UICharInfo>、:341 emplace<UISkillUp>、:506 // UIEvent 侧没有任何从本包取数据的通路（构造函数塞的是 16 条硬编码占位）`；`PlayerInteractionHandlers.cpp:94 charinfo = UI::get().emplace<UICharInfo>(character_id)`。
**tooltip 查询：未找到独立「tooltip 查询」发包类/handler**，tooltip 全由本地 `Data/*` + `Character/Inventory/*` 现场拼装。[观察/推断]

### §3.3 Data 层
`Data/SkillData.h:28 class SkillData : public Cache<SkillData>`、`:32-53 struct Stats`（damage/matk/fixdamage/mastery/attackcount/mobcount/bulletcount/bulletcost/hpcost/mpcost/chance/critical/ignoredef/hrange/range/criticaldamage）、`:57 Flags/:65 Icon/:74-100 方法/:103 // Skill 根尚未装载时构造出的是空壳，空壳进缓存会被永久锁死`、`:115-130 unordered_map + bool rooted`；`SkillData.cpp:40/111/144/209` 空壳/TODO 行。
`Data/ItemData.h:28/32-65/73-76/79-93`；`Data/QuestData.h:16/22-45 get_summary_for_tab(uint16_t tab)/get_area/get_jobs/get_chain_parent/.../:51-64 字段`；`Data/EquipData.h:30/41/43/45/49`；`Data/WeaponData.h:29/40/42/46/48/54`；`Data/JobData.h:28/32/34/36`；`Data/MakerRecipes.h:9-17 struct MakerS9Recipe`；`Data/MobDropData.h:15-20 struct MobDropEntry`；`Data/MobDropData.cpp:2 // MobDropData — stub loader (no drop table source in this client build)`、`:17 LOG(LOG_INFO, "[MobDropData] stub empty drops for mob_id=" << mob_id);`。

### §3.4 Character 层
`glob Character/Skill/*` → **No files found**（复核两次）；技能书实体在 `Character/SkillBook.h`（被 `UISkillBook.h:27`、`UIKeyConfig.h:24` include）。
`Character/SkillBook.h:26 class SkillBook`、`:29 set_skill(id,level,masterlevel,expiration)`、`:44-49 struct SkillEntry { int32_t level; int32_t masterlevel; int64_t expiration; }`、`:51 unordered_map<int32_t, SkillEntry> skillentries`（对应 Angel SKILLENTRY 数据结构位）[观察/推断]。
`Character/SkillMacros.h:11/20/23/75/85 struct SkillMacroSlot / class SkillMacros / static constexpr size_t MAX_SLOTS = 5; / get_slot(size_t) / std::array<SkillMacroSlot, MAX_SLOTS> slots{}`（**对应 Angel CUIMacroSys 32 格 → 当前端仅 5 槽**）[观察/推断]。
`Character/QuestLog.h:27/40-45`；`Character/Inventory/Inventory.h:36/39/47/84-112/124`；`Character/Inventory/Equip.h:26/29/42-57`；`Character/Inventory/InventoryType.h:27/46`；`Character/Look/EquipSlot.h:27/39/46/47/50-58/66`（12 行 TODO）；`Character/SkillId.h:24/38/119/142`；`Character/Player.h:145/149/151`。
095 线（不作 083 证据）：`Character/Inventory/Equip.cpp:24 // 095 潜能/魔方：…`。

---

## §4 组件层 `IO/Components/`（含 Tooltip 5 组件）

- `Tooltip.h:26 class Tooltip`、`:30-42 enum Parent { NONE,EQUIPINVENTORY,ITEMINVENTORY,SKILLBOOK,SHOP,EVENT,TEXT,KEYCONFIG,WORLDMAP,MINIMAP }`（10 态）、`:46 virtual void draw(Point<int16_t> cursorpos) const = 0`。
- **TextTooltip / TooltipBackdrop**：`TextTooltip.h:28-38` Q1 定案逐字「v83 原生 tooltip 框体是程序描边而非贴图 —— 包内无 UIToolTip.img、UIWindow.img/ToolTip 无九切片，且包内全部九切片族均为浅底气泡」「5 个 tooltip 组件共用」「背板填充 = 实心 (79,79,119)，四边 1px 亮边 = (85,85,130)，**没有灰色外框**」「旧实现填充 (85,85,130) + 灰边 (128,128,128)」；`:39-59 class TooltipBackdrop`、`:44 ColorBox fill(..., Color(79/255,79/255,119/255,1),1)`、`:47 const Color edge(85/255,85/255,130/255,1)`；`:61-74 class TextTooltip`、`:68 bool set_text(std::string text, uint16_t maxwidth=340, bool formatted=true, int16_t line_adj=2)`。⚠️ 与 r03 §0.1 记的 `README.md:74` 黑 0.9α + 1px 灰边 (0.5,0.5,0.5) 是不同口径（前者黑底灰边、后者 (79,79,119) 紫底亮边 + 无灰框）—— 以源码 `TextTooltip.h:28-47` 为真值，README §8 限制须标注两版结论冲突。
- `ItemTooltip.h:27/34/37-48`（itemid/fillwidth/fillheight/descdelta/itemicon/name/desc/qual/backdrop/untradable/unique）；`ItemTooltip.cpp:30 // TODO: Add blue dot next to name`。
- `EquipTooltip.h:43 // 内容右界 ≈261（reqset 177+54+数字位宽），框宽取 264`、`:44 FRAME_WIDTH=264`、`:63-73 2026-11-08 原生复校新增 iconbase(UIWindow.img/IconBase/0 32x32)/growthset/growth_lev_label/growth_exp_label/growth_percent/fame_none`、`:73 // 用户抉择：仅当 itemlevel/itemexp 任一非零时才显示成长两行`、`:111-112 prank/prank_preview`、`:114-118 // 095 潜能/魔方：3 条主潜能 + 2 条 bonus + bool haspotlines[5] + Text potlines[5]`（095 线，不作 083 证据）；`EquipTooltip.cpp:214/250` TODO。
- `SkillTooltip.h:27/34/44 Texture iconbase; // UIWindow.img/IconBase/0 灰底板`；`SkillTooltip.cpp:98 // TODO: Blank`。
- `MapTooltip.h:41 MAX_LIFE=10u`、`:42-44 SEPARATOR_ADJ(1,10)/LIFE_LABEL_ADJ(16,3)/LIFE_ICON_ADJ(1,9)`、`:46-54 draw_worldmap/draw_minimap/set_*`、`:69-70 mob_labels[10]/npc_labels[10]`；`MapTooltip.cpp:29 // TODO: Get number of available quests`、`:38 // TODO: Get list of where party members are`。
- `Gauge.h:27-34 enum Type{DEFAULT,CASHSHOP,WORLDSELECT,CLIP}`、`:32 // 自纹理左侧裁出 percentage*maximum 宽（不拉伸），用于 v83 StatusBar HP/MP`、`:36 默认值`、`:45 set_clip_left`、`:48 get_percentage`；`Gauge.cpp:70-93 step=(target-percentage)/24`。
- `Icon.h:32-40 IconType 5 态`、`:47-57 虚函数`、`:102 // 显式重载`、`:105-106 cell_width=32; cell_height=32`、`:108-114 set_cell/rect/contains/draw_at/start_drag_at`；`StatefulIcon.h:27-33 State 3 态`；`IconCover.h:25 // transparent rectangle 30x30`、`:29-33 Type{BUFF,COOLDOWN}`。
- `Button.h:28-36 State{NORMAL,DISABLED,MOUSEOVER,PRESSED,IDENTITY,NUM_STATES}`；`MapleButton.h:44-45 textures[NUM_STATES]/animations[NUM_STATES]`。
- `Slider.h:44-75 enum Type 24+ 值`、`:96-102 dbase/dnext/dprev/base + TwoSpriteButton`；`MapleFrame.h:24/30/33-43` 九切片 + xtile/ytile。
- 其它：`Charset.h:29 Alignment` + `Charset.cpp:84 TODO`；`ChatBalloon.h:40 DURATION=4000`；`MapleComboBox.h:32/61/75/83` + `HEIGHT=16`；`NameTag.h:41/49/50 MIN_ID=3/MAX_ID=254`；`Textfield.h:32 State`。

---

## §5 全库「预留/stub/TODO/未实现/暂不/显式/不做/假/placeholder/硬编码/无依据」字样（本面强相关，逐字）

> 全库 `*.{h,cpp}` grep → Found 250 of 287 matches（完整 spill `23cd5502dde2-grep-results.txt`）。第三方 `includes/stb/*`、`includes/freetype/*`、`includes/SDL3/*` 的 @TODO 为库噪声（约 130+ 行，范围 stb.h:653..14149 等），不逐一陈列，仅声明存在。

### §5.A 核心窗口 / 协议 / 组件（强相关）
`IO/UIStateGame.cpp:314 // = notifier 列表封包 —— 服务端不存在 ⇒ 按门禁 7 不做假窗`、`:316 LOG(LOG_DEBUG, "UIStateGame: NOTIFIER key (v83 L) — no s9 notifier backend, intentionally inert")`、`:532 // 095 自定义中文 UI：暂时复用 EVENT 键打开福利中心`；`IO/UIElement.h:196 // StatusBar3.img/OptionMenu，UIOptionMenu 只剩空壳，故另立两型`。
`UIQuestLog.cpp:182 // 接受/拒绝/任务讯息：底带实测只证到 寻找NPC位置，其余三颗无位可放且不发包 → 预留（见 coords.md）`、`:903 // 接受/拒绝/任务讯息：参考截图底带里没有这三颗，且无对应窗口可开 → 预留不显示`、`:947 // 复用 UIMiscShells 里既有的放弃协议（action 3 = forfeit），不发明新包`、`:955 // 显式预留（不是漏实现）`、`:956 //  MARKNPC「寻找NPC位置」原生是"把该任务 NPC 所在地图在世界地图上高亮点出"、`、`:957 //    需要 npc→map 反查表（服务端数据）+ UIWorldMap 的标记入口，本包两者都没有、`、`:958 //    且没有任何可对照的封包证据 —— 不发明协议，见 notes/open-issues.md`、`:959 //  OK/NO/ALERT 只在"接受/完成确认"弹层里出现，本窗口底带不显示（activate_buttons 已置 false）`；`UIQuestLog.h:103 // 命中区必须等于绘制区，否则右侧 241px 的点击会被窗口自己吞掉`。
`UIEquipInventory.cpp:567 // 判据缺口见 evidence/r27-b3-equip/README.md §7 —— 不做「看着像就写死」的格子表`、`:622 // 预留：BtPet1/2/3 三页宠物装备槽的穿戴/卸下（Equip/pet 下无 Slots 子节点，槽位数据待协议批次）`；`UIEquipInventory.h:104-105 // 本包拿得出运行期判据的槽位；其余（宠物面板 4 格、POCKET 等）无判据 → 不改色…`。
`UIItemInventory.cpp:1618 // TODO: Remove from EquipTooltip and move into Job?`、`:1684 // End of TODO`；`UIItemInventory.h:49-50 / .cpp:124 / :530-532` 095 线 CUIItem 注释（见 §0）。
`UIKeyConfig.cpp:649 // 抓取时按「命中的那一格」显式算偏移…`、`:731 static const std::string& message = "Would you like to revert to default settings?";`（硬编码英文）；`UICharInfo.cpp:171 charinfo.medal_stub`、`:460 // 皮肤切换与资源装载按证据不足显式预留，不凭空造节点`；`UICashShop.cpp:100/:102 cashshop.stub_job/stub_name`。
`UIWorldMap.h:64 // 双击右侧搜索结果行 → 跳到该图所属分区并选中点位（todo 8）`、`:219 // 启用时替代 UIMapMobPanel 的占位（D2 并存…）`；`UIWorldMap.cpp:1036 // 静默换图会让人以为「跳错了地方」，所以这里显式拒绝并留日志`；`UISkillBook.cpp:1268 // 「展开后收缩不了」。收起必须显式 deactivate…`；`UIUserList.cpp:324 // title/base 等不存在的节点，恒为空，只剩一个硬编码 Text("none")）`、`:916 // v83 家族聊天由聊天框目标/斜杠别名路由；change_target 仍是预留空实现`、`:1131` 页签 id 非线性、`:1282 // 只接受四态按钮容器，空壳节点跳过`；`UIMiscShells.cpp:4188 // UIOptionMenu 只剩空壳，所以直接落到 UIGameOpt（见 R27-B7-4）`、`:6683 // 同 UISkillMacroEx：构造时不弹安全键盘，改名走 send_key 的 S/R 分支`；`UIMiniMap.cpp:516 // TODO: Toggle scrolling map`、`:666 // SMALL/BIG 未注册（本包无件，显式预留）…`。
`IO/Components/`：ItemTooltip.cpp:30、SkillTooltip.cpp:98、MapTooltip.cpp:29/:38、EquipTooltip.cpp:214/:250、Icon.h:102、Charset.cpp:84、StatefulIcon.cpp:24（逐字见 §4）。
`Net/Packets/`：GameplayPackets.h:64、AttackAndSkillPackets.h:41/:299/:301、SkillMacroPackets.h:37-39、Net/OutPacket.h:42/:203、PacketSwitch.cpp:129（逐字见 §3.1）。
`Net/Handlers/`：PlayerHandlers.cpp:139/:303、MessagingHandlers.cpp:506、MapObjectHandlers.cpp:417/:675/:905/:906/:928、TestingHandlers.h:24/:30、HiredMerchantBoxHandlers.h:25、Helpers/CashShopParser.cpp:69、Helpers/LoginParser.cpp:123。
`Data/`：SkillData.cpp:40/:111/:144/:209、SkillData.h:103、MobDropData.cpp:2/:17。

### §5.B 外围模块（本面外围，逐字）
`UIDailyGift.h:32/:33/:58`、`UIDailyGift.cpp:113/:357`；`UIStorage.cpp:44`、`UIParty.cpp:41/:47`、`UIPartySearch.cpp:246`、`UIGuild.cpp:49`、`UIMonsterBook.cpp:43`；`UIOptionMenu.h:73`、`UIOptionMenu.cpp:1233`、`UICommonCreation.h:63`、`UIJoypad.cpp:32`、`UISoftKey.cpp:400`、`UINpcTalk.h:183`；`UIStatusBar.cpp:860/:2507`、`UIChatBar.h:31/:122`、`UIChatBar.cpp:2574`、`UIMiniMap.cpp:482`；`Configuration.cpp:1010/:1011`、`Audio/Audio.cpp:284`、`Character/SkillId.h:142`、`Character/Look/EquipSlot.h:39/46/47/50-58`、`Character/Char.cpp:333`、`Character/EquipStat.cpp:46`、`Character/CharStats.cpp:217`、`Character/Job.cpp:110`；`Gameplay/Stage.h:80 // Field placeholders (summon/mist/door/kite/dragon) until full sprites`、`FieldMarker.cpp:57/:122`、`MapBackgrounds.cpp:69`、`MapChars.cpp:71`、`Tile.cpp:33`、`Reactor.cpp:37`、`Mob.cpp:474`；`Graphics/Utf8.h:31`、`GraphicsGL.h:243`、`GraphicsGL.cpp:234/:280/:1466/:1546`、`Net/SocketWinsock.cpp:26/:160`、`Util/Misc.cpp:143`、`Util/ScreenResolution.h:55`。
095 线/外围（不作 083 证据）：`UILogin.h:83`、`UILogin.cpp:113/:196`、`UIAdmin.cpp:59`、`UIGMCommandList.h:11`。

---

## §6 当前端核心窗口「已实现功能面」20 条 [观察]
1. `IO/UIStateGame.cpp:319-355` E/I/S/K 四键完整装配装备/物品/能力/技能窗，TYPE+emplace 闭环
2. `IO/UIStateGame.cpp:361-393` G 键家族落 UIUserList 第 3 页签（083 原版口径），非独立浮窗
3. `IO/UIStateGame.cpp:296-309` F 键（MEDALS/FAMILY 学院）OpenFamilyPacket + emplace<UIFamily>()
4. `UISkillBook.cpp:350-394` 技能窗 classic 175x289 分区（标题/tab/书封/列表 y87..257 行距 40 可见 4 行）+ modern 双布局
5. `UISkillBook.cpp:547-560` BT_OKAY 批量加 SP（spend_sp 循环 + change_sp()）
6. `UISkillBook.cpp:524-541` BT_MACRO 只开 UISkillMacroEx 且 anchor_right_of(*this,-3)
7. `UIItemInventory.cpp:333-377` 物品栏折叠/展开两套底图 + 金币墨迹顶 269 锚点 + slider + icons
8. `UIItemInventory.cpp:500-529` 六页签切换 + Gather/Sort 钮
9. `UIEquipInventory.cpp:219-238` 装备不可用槽由 slot_available() 运行期判据逐像素改色（非写死格子表）
10. `UIStatsInfo.cpp:638-697` 能力窗 HP/MP/STR/DEX/INT/LUK 六项 + AUTO 自动分配（按主属性）
11. `UIQuestLog.cpp:926-950` 任务窗三页签/折叠/详细切换/放弃（QuestActionPacket action 3）
12. `UIQuestLog.cpp:343/:395` 连锁任务页码条 draw_chain_page_gauge + draw_gauge（Gauge spot 刻度）
13. `UIWorldMap.cpp:191-230` 世界地图 origin 补偿 + 三子面板（mob/info/search）+ 侧栏 draw_tab
14. `UIMapInfoPanel.cpp:58-94` 地图信息面板 4 页签 + 5×5 掉落格 + 完整布局常量
15. `UIKeyConfig.cpp:718-757` 改键窗 CLOSE/DEFAULT/DELETE + alternate_keys/basic_keys 默认表
16. `UICashShop.cpp:294-343` 现金商城 22 钮枚举 + preview/exit/next/prev + fadeout 回城
17. `UICashGachapon.cpp:16-88` 现金扭蛋扫 CASH 522 段 + BtOpen→UseCashItemBasicPacket
18. `UICharInfo.cpp:322-371` 角色信息窗 Family/Party/Item/Book 多分支发包
19. `TextTooltip.h:28-59` 5 个 tooltip 共用 TooltipBackdrop 程序描边（v83 逐像素结论）
20. `Net/Packets/* + Net/Handlers/*` 技能升级/宏/任务/制造/商城/物品/装备出站包与 handler 齐备（§3）

## §7 明显缺口面 20 条 [观察]
1. `UIQuestLog.cpp:951-959` MARKNPC/OK/NO/ALERT 四钮全 return NORMAL，缺 npc→map 反查 + 世界地图标记（显式预留）
2. `UIQuestLog.cpp:182/:903` 接受/拒绝/任务讯息底带三钮「无位可放且不发包 → 预留」
3. `UIEquipInventory.cpp:622` BtPet1/2/3 三页宠物装备槽穿戴/卸下未接
4. `UIEquipInventory.h:104-105` POCKET 等槽「无判据 → 不改色」
5. `UICashShop.cpp:100/:102` job/name 用 stub 文案
6. `UICharInfo.cpp:171` medal stub、`:460` 皮肤切换/资源装载显式预留
7. `UIWorldMap.h:64` 双击搜索结果跳分区（todo 8）未完成
8. `MapTooltip.cpp:29/:38` 可用任务数 / 队员分布 TODO 未取数
9. `SkillTooltip.cpp:98 // TODO: Blank`
10. `ItemTooltip.cpp:30` 蓝点、`EquipTooltip.cpp:214/:250` TODO
11. `UIMiscShells.cpp:3522-3555` UISkillEx 仅「底图 + tip_label」调试壳，非 CUISkillEx 441 函数级复刻 [观察/推断]
12. `UIMiscShells.cpp:6674-6704` UIMacro 仅改名 1 槽（get_slot(0)），非 CUIMacroSys 32 格 [观察/推断]
13. `Character/SkillMacros.h:23 MAX_SLOTS = 5` 远少于 Angel CUIMacroSys 32 格 [观察/推断]
14. `IO/UIStateGame.cpp:310-318` NOTIFIER（v83 L）服务端无后端 → intentionally inert
15. `IO/UIStateGame.cpp:289-295` MANAGELEGION 名沿用新版军团管理，实为 v83 PARTYSEARCH
16. `SkillMacroPackets.h:37-39`「改键 UI 接线待 v83 KeyConfig 快捷页实体化（Wave 6）」
17. `Net/OutPacket.h:203`「本枚举+包类先行登记，暂不接线」
18. `Data/MobDropData.cpp:2/:17` 掉表 stub loader
19. `glob Character/Skill/*` 无技能子系统目录；`glob IO/UITypes/UIToolTip.*` 无原生 CUIToolTip 对应窗
20. 全库 grep 原生类名仅 8 命中且全是注释/095 引用，无 CUISkill/CMacroSysMan/CQuestMan/CItemInfo/CCashShop 等类定义

---

## §8 三个明确回答

### ① 当前端是否把 Angel 原生 62 类一一对应拆成独立 UI 类？**否。** [观察]
全库 grep Angel 原生类名仅 8 命中（`UIKeyConfig.cpp:138`、`UIStatsInfo.cpp:146` 095、`UIItemInventory.h:50` 095、`UIItemInventory.cpp:124/:530`、`Net/Handlers/InventoryHandlers.cpp:56/:71`、`Graphics/GraphicsGL.cpp:1546`），全是注释或 095 线引用，无任何 `class CUISkill/CUISkillEx/CUIMacroSys/CUIMacroSysEx/CUIEnchantDlg/CUIQuestInfo/CUIQuestAlarm/CUIToolTip/CUIItem/CUIEquip/CUIStat/CUIUserInfo/CUIKeyConfig/CWorldMapDlg/CUIItemMaker/CCashShop/CUICashItemGachapon/CMacroSysMan/CQuestMan/CItemInfo/CSkillInfo/SKILLENTRY` 定义。当前端是「按功能合并 + 自命名」重构：`UIStateGame` 单一 `EnumMap<Type,...>` 调度（`IO/UIElement.h:203 NUM_TYPES`），核心面拆成 12 独立窗 + `UIMiscShells` 聚合 20+ 壳 + `IO/Components` 5 tooltip 组件，与 Angel 62 类**不是一一映射**。[观察/推断]

**语义位近似的一一对应（10 个）**：CUIStat→UIStatsInfo、CUIItem→UIItemInventory、CUIEquip→UIEquipInventory、CUISkill→UISkillBook、CUIKeyConfig→UIKeyConfig、CUIQuestInfo→UIQuestLog、CWorldMapDlg→UIWorldMap、CUIUserInfo→UICharInfo、CCashShop→UICashShop、CUICashItemGachapon→UICashGachapon、CUIItemMaker→UIMaker（均带类定义行号，见 §1.1）。CUIToolTip→无独立类，拆为 5 组件（`IO/Components/Tooltip.h:26` 统一接口）。[观察/推断]

**合并进 UIMiscShells 壳（TYPE 已注册、实现为底图+tip_label+SoftKey 调试壳）**：CUISkillEx→UISkillEx、CUIMacroSys/CUIMacroSysEx→UIMacro/UISkillMacroEx/UIMacroSkill、CUIEnchantDlg→UIEnchantSkill、CUIQuestAlarm→UIQuestAlarm、UIQuestIcon/UISkillUp/UIBook/UIMobGage 等 20+ 壳。[观察/推断]

**原生类在当前端完全没有对应窗口**：CUIToolTip（单一窗口类无）、CMacroSysMan/CQuestMan/CItemInfo/CSkillInfo（Angel 管理器/数据单例 → 当前端以 `Data/SkillData.h:28`、`Data/ItemData.h:28`、`Data/QuestData.h:16`、`Character/SkillBook.h:26`、`Character/QuestLog.h:27` Cache/数据类替代，无同名窗口、无 441 函数级逻辑）、SKILLENTRY→`Character/SkillBook.h:44-49 struct SkillEntry`（仅 3 字段）、CUIUserInfo 的 Farm/Personality/Collect/Damage 子窗在 `UICharInfo.h:78-127` 有字段但 `:460` 显式预留。[观察/推断]

### ② 与 Angel r01 锚点的逐锚点对应 [观察/推断]
| Angel 锚点 | 当前端 | 判定 |
|---|---|---|
| `CUIStat::OnButtonClicked 10 case` | `UIStatsInfo.cpp:638-697` 仅 8 有效 case（BT_CLOSE + 6 属性 + BT_AUTO）；`:146` 注释引 095 CUIStat::OnCreate | 部分对应，case 数不足 |
| `CUIItem::OnChildNotify 2004/2005` | `UIItemInventory.cpp:530-532` 以「0x7D3→OnGather / 0x7D4→OnSort」注释对齐 CUIItem::OnButtonClicked（095 线，非 OnChildNotify 2004/2005）；`.h:49-50` 对齐 SetArrangeButton | 语义近似，消息 id 口径不同（s9 适配） |
| `CUIToolTip 26 方法` | 无 CUIToolTip 类；5 组件方法总数远少于 26（`Tooltip.h:46` 仅 1 纯虚 draw） | 未对应，方法面严重收缩 |
| `CWorldMapDlg 666×524` | `UIWorldMap.h` 未定义整机尺寸常量；`UIMapInfoPanel.cpp:58-94` 仅子面板 240×490；`UIWorldMap.cpp:211-212` 底图 origin (320,235) | 无对应常量，布局口径不同 |
| `CCashShop 66 方法` | `UICashShop.h:38-51` 仅 11 override、`UICashShop.cpp:294-343` 有效 case 少、`CashShopPackets.h` 14 出站包类 | 未对应，方法面收缩（但 r02 §8.4b 已销案「非 stub 窗」） |
| `CUIQuestInfo 15 方法` | `UIQuestLog.h:36-55` 约 12 方法、`UIQuestLog.cpp:926-960` 仅 5 有效分支 + 4 预留 | 部分对应，含显式预留缺口 |
| `CUIMacroSys 32 格` | `Character/SkillMacros.h:23 MAX_SLOTS = 5`、`UIMiscShells.h:650 MACRO_COUNT=5`、`:648 VISIBLE_ROWS=3`；`SkillMacroPackets.h:34-39` 快捷栏 8 键另计 | 5 槽 vs 32 格，数量级不符 |

### ③ 结论一句
当前端已把 083/v83 核心窗口按「独立窗（12）+ 聚合壳（UIMiscShells 20+）+ 组件（5 tooltip + gauge/icon/slider/frame）」重构落地，主要交互（装备/物品/技能/能力/任务/世界地图/改键/商城/扭蛋/角色信息）可跑通并配套出站包与 handler；但**不是** Angel 62 类一一对应，宏系统（32→5 格）、tooltip 方法面（26 方法→5 组件）、管理器类（CMacroSysMan/CQuestMan/CItemInfo/CSkillInfo）、CWorldMapDlg 666×524 整机布局、CCashShop 66 方法等存在结构性收缩，且 `UIMiscShells` 多数壳仍是「底图+tip_label」调试态，任务窗 MARKNPC/宠物装备槽/商城 stub 文案等有显式预留。[观察/推断]

---

## §9 与 r01/r02/r03 的冲突取值（主 agent 裁定）

1. **TooltipBackdrop 配色两版口径冲突**：r03 §0.1 引 `README.md:74` = `Color(0,0,0,0.9)` 黑底 + 1px 灰边 (0.5,0.5,0.5)；本文 §4 直读 `IO/Components/TextTooltip.h:28-47` = `Color(79/255,79/255,119/255)` 紫底 + 1px 亮边 `(85,85,130)` + 「没有灰色外框」。→ **按门禁「checked-in source > docs」以源码 `TextTooltip.h:28-47` 为真值**；README §8 限制须标注该冲突，判为「已迭代改造、README.md:74 是中间态」。[观察+推断]
2. **CUIItem 整理钮 id 口径**：r02 §2.2 记当前端 `0x7D3=BtGather / 0x7D4=BtSort`、Angel `OnChildNotify 0x7D4=BtSort / 0x7D5=扩展背包`；本文 §0 新发现 `UIItemInventory.cpp:530-532` 这些注释本身引的是 **095 线** `CUIItem::OnButtonClicked @ 0x7CC4F0` → 结论不变（s9/095 适配），但依据须换成 Angel `CUIItem::OnChildNotify @ 0x81d01f`，且**当前端注释里的 095 地址属文档污染**，列入 D 决策点回写清单。[观察+推断]
3. **CashShop 状态**：r02 §8.4b 与本文 §1.1/§6 一致 —— 非 stub 窗，stub 仅指 `:100/:102` 两个 Text；本文进一步给出 22 钮枚举与 11 override 的收缩数字，供 README 结构性对照引用。[观察]
4. **§1.2 壳类数量**：本文把 UIMiscShells 聚合壳定为「20+」，r02 §8 记的是「~70 个 UIXxx : public UIDragElement<PosNOTICE> 壳类（UIMiscShells.h:12-970）」——两者口径不同（前者=Type 已注册且与核心窗口语义相关的壳，后者=文件内全部壳类声明），README 采用「12 独立窗 + UIMiscShells 20+ 核心相关壳 / 全文件约 70 壳声明」双口径表述。[观察+推断]

---

## §10 性质标注
- 本文全部结论为 [观察]（逐字引文件行号）或 [推断]（拆分结论/覆盖度归类/锚点对应），无臆造；子代理声明全程只读、未改任何文件、未连库、未改 MCP 配置。
- 095/Maplestory95 注释仅作现状罗列、未作 083 证据；第三方 stb/freetype TODO 噪声已在 §5 声明范围。
- 与 r01（Angel 一手语义）、r02（主 agent 跨端销案）、r03（IMG 资源与样式）共同构成核心窗口面阶段 0 取证集，供 README 六维对照表引用。
