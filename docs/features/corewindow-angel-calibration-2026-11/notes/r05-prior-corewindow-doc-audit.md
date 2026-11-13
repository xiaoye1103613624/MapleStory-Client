# r05 · 核心窗口面 — 既有文档四态审计（第二路 subagent `4cfbbb14` 结构化归档）

> 来源：只读子代理 `4cfbbb14-969a-4dd6-b474-744a7129e529` 于 2026-11-08 20:31 发回的 20K「核心窗口既有文档四态审计清单」。子代理声明：全程只读、未改任何文件、已加载 `xiaoye-maple-story-dev` 门禁；相对路径以 `E:\project\MapleStory-Client` 为根；`[观察]`=文档明写判定/逐字行，`[推断]`=跨文档归并/冲突裁决，`[待实机]`=明写需实机/真机/截图；时间标签 `2026-10 旧账` / `2026-11 现场重算`；三层证据沿用 `docs/features/beidou-v17-parity/notes/r16-rea-coverage-gap.md:29-38`（A=catalog / B=串+xref 锚点 / C=REA decompile+控件表包序；`:38 A ──知道有──▶ B ──找到构造函数──▶ C ──可完美复刻──▶ 实机截图验收`）。
> 主 agent 归档说明：本文为可 grep 的结构化固化，按门禁「源码逐字 > 2026-11 现场重算 > 2026-10 基线」裁定；与 r01/r02/r03/r04 冲突处以本文 §0 裁定为准。

---

## §0 主 agent 前置复核块（现场发现 + 裁定）

- **★ 现场产出确认**：`docs/features/corewindow-angel-calibration-2026-11/` 是本会话第三功能面现场产出。`r01:10`「**EA 交叉验证 439/441 同 EA**…Angel 只补符号语义层 + 行为层」；`r01:12` `SP_` StringPool **401 个**其中 **91 个 `UI_UIWINDOWIMG_*`**；`r01:133` 逐字点名「上一轮 `r16-rea-coverage-gap.md` CoreWindows 批次仅达 B 锚点（`sub_8E49B5` ToolTip / `sub_83EA05` Quest / `sub_87CE25` / `sub_999E4D` CashGachapon / `sub_82A9BC` ViciousHammer / `sub_82BB83` VegaSpell、Maker `sub_822350` 达 C），**Angel 恰好补上这些 `sub_*` 的类名/方法名/调用关系层**」；`r01:135`「坐标层上一轮已用无符号 IDB 对齐过…本面 Angel 补的是 ctrl id→业务语义映射、网格命中常量、脏词过滤、字体分派、跨窗联动等行为层」。[观察]
- **★ 结论（本面定性）**：2026-11 现场把「构造 EA/类名/行为语义」推到具名层（A/B→近 C 前段），但「控件坐标逐点 + 包序 + 像素复核 + 实机验收」仍未达 C。`r02:14`「**校准 ≠ 改回原生**」；`r02:152`「**未做**：像素级视觉复核（`read_image` 对本模型返回 `model "hy4free" does not declare image input`）；实机构建/五屏比对；任何产品源码修改」。[观察+推断]
- **★ 特别复核 `UIToolTip.img` 判定（子代理答「同意第三路，无反证」）**：`docs/features/ui-v83-window-restore/PLAN.md:20,34` 是「HeavenClient 偏新路径 → 走错资产代次」的**修复前根因诊断**，非现状；`README.md:74` / `CHECKLIST.md:70` 已全部清零并定案 `UIWindow.img/ToolTip` + `TooltipBackdrop` 程序描边；当前端源码 `IO/Components/EquipTooltip.cpp:53` 直读为 `UI["UIWindow.img"]["ToolTip"]["Equip"]`。→ **与 r03 §9.1 一致，Q2 清单 C14（`docs/features/feature-gap-inventory-2026-11/README.md:141`）属旧账须回写**。[观察+推断]
- **★ TooltipBackdrop 配色两版口径（r03 §0.1 vs r04 §4）裁定**：`README.md:74` = `Color(0,0,0,0.9)` 黑底 + 1px 灰边 (0.5,0.5,0.5)；`ui-v83-window-restore/notes/open-issues.md:479` B8.1「原生参考帧逐像素反算 ⇒ 底 = 纯黑 **α=0.90**、边框 (128,128,128) 1px、分隔线白 α0.4，三个值与 `TextTooltip.h:37` 现有实现完全吻合」；`r04 §4` 直读 `IO/Components/TextTooltip.h:28-47` = `Color(79/255,79/255,119/255)` 紫底 + 1px 亮边 `(85,85,130)` + 「没有灰色外框」。→ 按门禁「checked-in source > docs」，以 `TextTooltip.h:28-47` 为当前真值，`README.md:74`/`open-issues.md:479` 记中间态；README §8 限制须标注该冲突。[观察+推断]
- **095 线排除（MEMORY.md critical 硬约束）**：`docs/features/c4-btsort-gather-semantics/README.md:26` 预留「`BeiDou.exe.i64` 仍无 `CUIItem` 串锚；本结论锚定 **GMS v95 符号库**，v83 classic 资源键为 `Item/BtGather|BtSort`」；`ui-v83-window-restore/notes/open-issues.md:728`「2026-11-06 定案（Maplestory95.i64）」；`docs/features/userlist-five-tabs/notes/diagnosis-and-plan.md:35`「`095REA` 线虽有 UserList 记录，但按 083/v83 基准**禁用**，不作依据」。→ 这些结论仅登记存在、不作 083 判定依据，应改引 Angel `CUIStat::OnCreate @ 0x8c4c7f` / `CUIItem::OnChildNotify @ 0x81d01f`（见 r02 §5、r04 §0）。[观察+推断]

---

## §1 四态清单

### §1.1 已校准（原生 C 层证据 / 源码已改已销案）
- `r16-rea-coverage-gap.md:25/:55/:180` — 已达 C 样板：Maker `sub_822350`（完整伪代码 + 控件 id）；StatusBar `sub_8D01B2` CreateCtrl 坐标表 `:56` **C−**（点击/QS/聊天行为未闭环）；证据 `evidence/rea_MakerUI_sub_822350.c`、`rea_decompile_MakerUI.json`、`evidence/r15/statusbar_createctrl_coords.txt`。[观察｜2026-10]
- `r01-angel-native-corewindow-facts.md:10-13` — Angel 62 类 441 方法 / 296 反编译全 OK / 439/441 EA 同址；类方法数 `CUIToolTip 26 / CWorldMapDlg 18 / CUIQuestInfo 15 / CCashShop 14(精选,ea.json 66) / CUIUserInfo 13 / CUIItem 12 / CUIStat 8 / CUIMacroSys·CUIMacroSysEx·CMacroSysMan 各 8 / CUIKeyConfig 7 / CUISkill 5 / CUISkillEx 3 / CUIEnchantDlg 2 / CUICashItemGachapon 3`；`r02:100` 交叉验证 441/439 同址；`r02:87` `CUICashItemGachapon::OnCreate 0x999e4d → sub_999E4D 0x43c` 与 `r16:65` EA+size 逐字吻合（Angel↔BeiDou 同二进制 BenchMark）。[观察｜2026-11 现场]
- `ui-v83-window-restore/notes/open-issues.md:785-789` — C8 任务详情缺页码图标「已闭环 2026-11-05」IDA `sub_83EA05`（total>1 画刻度）+ `sub_83FEC6` 格式 `"%s ( %d / %d )"` + pool 4497 `Chain Quest`（源码已改销案；`:789` 真机点测仍待）；`quest-page-icons/README.md:3/:28/:30`「代码已闭环、无待决项」。[观察｜2026-11]
- `c4-btsort-gather-semantics/README.md:3/:10` — Gather/Sort 叠槽表示「下一步动作」代码已闭环（`:26` v95 借证，见 §0）；`open-issues.md:728` 2026-11-06 定案 `OnGatherItemResult→SetArrangeButton(ti,1)` 显 Sort / `OnSortItemResult→(ti,0)` 显 Gather / `OnTabChanged` 恢复 `m_anSortBtState[ti]`；`:723-726` C4 中间槽 `UIItemInventory.cpp:1238` 加 `sm_pair` 守卫 + `:112` 初值 `sort_enabled(true)→false`（源码已改销案，`:726` 注「若 IDA 后续证明 `_SM` 才是原生展开态真身，去掉守卫即可」）。[观察｜2026-11]
- `open-issues.md:479-484` B8 五条实测：B8.1 原生 tooltip 底 α=0.90 / 边框 (128,128,128) 1px / 分隔线白 α0.4「与 `TextTooltip.h:37` 完全吻合，透出背景正是原生行为」；B8.2 `EquipTooltip.cpp:57-64` DEX/INT y `71→77` 三行统一 15px；B8.3b 加 `reqstr.size()<3` 守卫；B8.4「⛔ 本条判定反了，已回退」职业名 36×11 白块同原点叠绘（`EquipTooltip.cpp:644-651/:751-753` 回退为同原点叠绘，但「若用户更看重可读性改横排」列 CHECKLIST 待拍板）；B8.5「证伪：原生就是英文」`ATT INCREASE`/`Type: TOP`/`Defense: 3`/`Remaining Enhancements: 7`/`Hammers Applied: 0` 不改。[观察｜2026-11]
- `open-issues.md:899-901` — HUD「商城」钮「40 vs 229 跨线必失败」结论过时（40 是 `ENTER_CASHSHOP` 非 `CASHSHOP_OPERATION`），s9 `EnterCashShopHandler` 可用 ⇒ C5/D4「点了没反应/黄字提示」作废。[观察｜2026-11]
- `r40-continuous-shell-coverage.md:5/:7/:9` — 15 壳已铺（PersonalShop/MemberShop/TradingRoom/EntrustedShop/CashTradingRoom/ItemSearch/AranSkillGuide/Transform/MonsterKilling/MapleEvent/SkillEx/Book/QuestAlarm/MateMessage/BetaEdition）+ Minigame 接 PLAYER_INTERACTION + 编译持续通过；`r40-utildlg-branches-remaining-shells.md:12/:16/:17` 7 壳 + `UpdateSkillHandler→UISkillUp` / `ShowMobHpHandler→UIMobGage` tip。[观察｜2026-10]
- `feature-gap-inventory-2026-11/README.md:320-326/:388` — §9.5 协议层权威重算 `client_out_sent` 103→**148/148**、`client_in 306/305`、`s9_registered_handlers 174`、c2s 构造点 **238**、`.dispatch()` **917**；「协议已发、玩法深度/真渲染/UI 分支未闭环」推翻「包都发不出」。[观察｜2026-11]
- `feature-gap-inventory-2026-11/README.md:73` — `WORLD_MAP_PLAYERS(376)`「✅ 真机闭环」2026-11-05 `SEND 277 → RECV 376`；`:147` C20 `end_talk` ✅ 已修；`:185` D11 婚礼 ✅ 双端闭环。[观察｜2026-11]
- `equip-compare/README.md:3/:80` — 客户端层已闭环、MSBuild Release/x64/v142 `EXITCODE=0`（`:82` 回归待执行）；`asset-node-inventory.md:46-50` 2026-11-02 裁决「原生『槽位不可用』= 对同一张 backgrnd 逐像素调色 `R'=snap17(0.75R+68.25)`、`G'=B'=snap17(0.75V)`，15,134 张位图扫描无粉色、两端 UIWindow.img md5 一致 ⇒ 不是素材缺口」；`open-issues.md:706-707` 证伪 `:700-701`「有一颗本包不存在」：六颗 Bt 全在且各 4 态 12×12。[观察｜2026-11]

### §1.2 未闭环（A/B 层、壳窗、显式预留、与当前源码冲突）
- `r16-rea-coverage-gap.md:169/:75-77` — 「整批功能面多已有，深度均未达 C」「Core 各窗深度行为仍停 A」。[观察｜2026-10]
- `feature-gap-inventory-2026-11/README.md:11/:30/:56/:58` — 「不缺窗口文件也不缺出站封包；缺玩法深度/真实渲染/服务端闭环/控件表还原/实机验收五层」、壳窗 `UIMiscShells.h`（1037 行）约 70 壳 `:12-970`、🟡/⊘ 定义。[观察｜2026-11]
- **冲突主条目**：`feature-gap:140` C13「SkillEx/宏/附魔 UI 深度、QuestAlarm、ToolTip tip 族齐全度 🟡｜r40 已铺壳…深度对照仍 A/B 层」；`:141` C14「5 tooltip 组件指向不存在的 `UI["UIToolTip.img"]["Item"]` 🟡｜`ItemTooltip.cpp:33`/`EquipTooltip.cpp:32`/`SkillTooltip.cpp:30`/`TextTooltip.cpp:30`/`MapTooltip.cpp:32`；UIWindow.img/ToolTip 只有 Equip/cash/mesos；BeiDou.exe `UIToolTip` 0 命中」；`:161` C34「CashShop 礼物/礼包深度 🟡」；`:162` C35「`UICashShop` stub 文案/购买未完整 🟡 `cashshop.stub_job/stub_name`」。[观察｜2026-11｜**须回写**]
- `feature-gap:128` C1 `UIMTS::update(){}` 空体 / `:129` C2 `UIOptionMenu` 退化空壳 / `:130` C3 下拉展开「只 log `"expanded list not implemented"`；画质档数/VSync 阈值无原生依据」；`:135` C8 宠物装备三页 / `:136` C9 `slot_available()` / `:139` C12 任务窗 `BtOK,BtNo,BtAlert,BtMarkNpc` 预留；`:138` C11「寻找 NPC 位置」⊘「无封包/IDA 证据 → 不发明协议」；`:163` C36 `UIGameMenu`「场景变更」⊘；`:154` C27 原生缺失资产 ⊘。[观察｜2026-11]
- `open-issues.md:818-824` C13「寻找NPC位置按了没反应 — 显式预留，不是漏实现」需 ① npc→map 反查表 ② `UIWorldMap` 标记 NPC 入口「且没有任何封包/IDA 证据 ⇒ 不发明协议」；`:686-689` C3「`UIOptionMenu` 只能是空壳」GameOpt/SysOpt 只有 backgrnd（158×284 / 299×365）、无 StatusBar3.img/UIWindow2/4/6.img 根；`:100` 三窗「窗内按钮坐标仍缺」；`:146` `BtUIClose`/`BtUIClose2` 32x15「按 `dim.x()-21` 摆放会越窗 11px」。[观察｜2026-10/11]
- `r00-baseline-parity.md:75/:85/:204`、`r14-native-feature-catalog.md:102`、`r11-baseline-replication.md:119/:131` — 旧账「QuestAlarm 未接」「现金商城购买/结算未接」「礼物(0x04)/礼包(0x1E)/购买结果回包 handler 未接」「店主上架 SET_ITEMS 出站包已具备但未接 UI 按钮」。[观察｜2026-10-21 旧账]
- `r02:46` 新缺口 — 「`0x7D5=扩展背包(SP_4501_EXPAND_INVENTORY)` 当前端无对应分支；当前端 `0x7D3=BtGather`（国服整理）在 Angel GMS v83 `OnChildNotify` 无 case ⇒ 国服/s9 适配 + 扩展背包语义缺失，改回原生须裁决」；`r02:51/:15` 脏词整族缺失（HUD `CChatHelper::TryChat` + 本面 `CUIMacroSys::OnButtonClicked` 两处原生都有、当前端全仓零命中）。[观察+推断｜2026-11]
- `r02:64/:76/:153`、`r01:139-140` — 「恢复默认键位/清空键位/宏键位拖拽」「礼物/礼包/愿望清单/点券查询/技能加点门槛/任务近期更新分类/宠物头像」等深度分支当前端覆盖度**不预判**（本面由 r04 补齐）；`CUIQuestAlarm::Draw`、`CUIToolTip::SetToolTip_Equip/Package`、`CUIUserInfo::Draw`、`CUIStat::Draw`、`CUIStatDetail::Draw`、`CUIFadeYesNo::Draw`、`CQuest::OnCompleteQuestFailed` 7 个 >50KB 超大函数已 `/output/` 补全但**未逐行人工核完**；296 函数是精选（非全部 441）。[观察｜2026-11]
- `feature-gap:142` C15 装备对比多部位 ⊘「明确不做」（`equip-compare/README.md:16/:90`）；`:159` C32 `UIUserList` 第 4 页签「家族联盟」❌「只有美术」**但** `userlist-five-tabs/notes/diagnosis-and-plan.md:65` 纠正「GuildUnion 有 20 个直接子节点（12 按钮+7 位图+独立 backgrnd），足以还原整个联盟页」（2026-11-08 现场推翻）；`:157` C30 `UIStorage` 🟡 但 `asset-node-inventory.md:103-113` 已给 Trunk 完整件表 + 名字映射 `BtArrange→BtSort`/`BtMesoIn→BtInCoin`/`BtMesoOut→BtOutCoin`/`BtClose→BtExit`（「语义按件名，未见原生文案」）；`:149` C22 `UIMapSearchPanel`「早期稿 4 项未实现 vs 2026-11 判面板 ✅，口径不同需现场复核」；`:152` C25 `MobDropData` 真掉落表 ❌ stub + `UI/MapInfo/layout.json:255` gold drop interval 未填。[观察+推断｜2026-11 部分销案]
- `skill-effect-npc-guide-c2s/README.md:21-22/:26` 持续技 KeyDown 状态机 / NPC_ACTION 移动分支未接；`asset-node-inventory.md:69` `ShortCut` 11 钮 + `backgrnd 93x245`「当前代码一个都没实例化」；`r16:196-198` ViciousHammer `sub_82A9BC` / VegaSpell `sub_82BB83` / CashGachapon `sub_999E4D` ❌ 仅 B 锚点待 decompile。[观察｜2026-10/11]

### §1.3 待实机（明写需实机/真机/截图/未跑）
`equip-compare/README.md:82` 回归 4 步待执行；`quest-page-icons/README.md:43` 连锁任务 Gauge spot 刻度真机；`c4-btsort/README.md:22` Gather↔Sort 切钮 + tab 独立真机；`skill-effect-npc-guide-c2s/README.md:32-34` A11b/A11d/A11e 待真机；`open-issues.md:489/:522` B9「背包窗第一次 Esc 不关」已修待复测、`:622` 能力窗/背包/技能窗三处真机对账同时报 Δ=1（`slot_rows 49 vs 50`）、`:418`「文案/图标级判据必须 crop ≥3x 放大」、`:795-805` C10 键帽字母「`Basic.img` 71 根里不存在 ⇒ 需裁决走补素材或 Photoshop MCP 重绘」、`:792-793` C9 HUD `EXP [0.00%]` 数字位缺度量依据、`:764-782` C7 技能窗三疑点（原生 WZ 插入顺序在 .nxcache 已丢 / `0000012` 光板需线上复验 / BookName「冒险的技术」vs「冒险家技能」禁止硬编码）；`userlist-five-tabs/notes/diagnosis-and-plan.md:103/:135-139/:162-178` `layout.py mark/bias` 五页签 oracle 探针未跑（当前环境抓屏全黑，须用户屏前复拍）、`:34` `rea` provider=ida `list_documents`/`binary_overview`/`search_strings` 均 `unreadable_output` 记阻塞、`:38`「布局 ground truth 只能是运行时 IMG 节点树」；`feature-gap:143` C16 公会 BBS 完整度待真机、`guild-bbs-ui/README.md:11/:27`；`feature-gap:527-567` §15 ⏳ 挂起总表（`:559` NPC 对话 A11b+A11c/tip 817-821+Pet inset/ScriptMessage+tip 820/AskAvatar/AskBoxText 全部「已接线待真机」、`:567`「本环境历史抓屏全黑，不假装已验证」）；`r21-remaining-backlog.md:11-15/:17-19` 登录遮罩图/PersonalShop CREATE/Messenger 立绘下一优先 + Avatar try-on 搁置；`world-map-players/README.md:30-31`；`r02:23`/`r01:141` IMG 层阻塞待第三路盘点（已由 r03 收口）。[待实机]

### §1.4 推测（推断层/证伪/推翻/冲突裁决）
`feature-gap:34/:36/:63/:582/:674-679` — 「r16/r00/r14/01 的 ❌ 大表是 2026-10-21 旧账」（r810 §3.1 T14 一次补齐 20 壳 + r40 15 壳 + `parity-gap:107` 实体窗）；证据优先级逐字「`协议层 2026-11-08 权威矩阵` > `源码逐字 .dispatch()/emplace 行` > `2026-11 现场重算` > `真机记录` > `2026-10-22/23 Wave 销案笔记` > `2026-10-21 基线` > `gap-inventory.md`（旧 head `44d0d5d9ca`，`client_out_sent=103`，协议层已过时）」；§9.5 TOP20 覆写「原 ❌ 降级为玩法深度/真机验收，勿再按补发包立项」；性质标注定义。[推断｜2026-11]
`parity-gap-2026-11/README.md:100/:3` — `Title` ❌ 全缺「证伪（2026-11-04）：s9 两侧 opcode 与全源码零命中，裁决改保持壳/显式预留」+ 现场重算脚本 `tools/gap_scan_2026_11.py`/`tools/gap_c2s_refined.py`；`open-issues.md:517/:712` 自我推翻「v83 原生背包右上角本来就没有 ✕」（更正：有，只是长得不像 ✕）；`:695-701`→`:706-707`→`:715-718` 背包中间钮「有一颗不存在」被证伪、真因 `set_sort()` 只点亮 `BT_SORT_SM`/`BT_GATHER_SM`（仅 FullAutoBuild 分支创建，本包走 classic）；`:483` B8.4 判定反了已回退。[推断→证伪｜2026-10/11]
`r02:14/:40/:70/:12/:16/:133` — 结构性差异「Angel 单体 `CUIToolTip` 26 方法 + `CUIStat` 8 + `CUIItem` 12 ↔ 当前端 5 Tooltip 组件 + UIStatsInfo + UIItemInventory/UIEquipInventory 拆分，与登录面/HUD 面同一模式」；原生 10 ctrl id（2000–2009）与当前端 `Buttons` 不是同一套编号、`SendAbilityUpRequest` 掩码体系用 `AutoAssignApPacket` StatCode 替代属实现路径差异；原生 666×524 + 无在线玩家列表再坐实、当前端右上列表 s9 适配保留不判缺失；`UIStatsInfo.cpp:146` 095 线地址污染，坐标结论可保留只换依据；`port-095/notes/opcode-v95.md:758,760` 仅登记存在不作依据。[观察+推断｜2026-11]
`quest-page-icons/README.md:14/:47` DualMobGauge `sub_56C9B7` 证伪「不是多目标队列：PartyRace 专用 ctor 硬编码 `Mob/9700036`，FIELD_EFFECT mode5 走 MobGage `sub_5336CA`」、多目标队列本轮不实现；`world-map-players/README.md:13/:15` 协议 s9 自定义、原生 IDA 无在线玩家列表 UI；`feature-gap:65-74` §2.4 六处冲突「按较新现场/真机取后者」、冲突 #6「以 148 为准，70 差集旧账作废」。[观察+推断]

---

## §2 四个已知冲突的当前状态（逐条对账）

| # | 冲突项 | 旧账 | 2026-11 当前状态 | 判定 |
|---|---|---|---|---|
| 1 | **Skill/SkillEx/Macro/Enchant 🟡** | `r16:161` A「SkillEx 等无 EA」；`feature-gap:140` C13 🟡 | Angel `r01:13` 已给 `CUISkill`5/`CUISkillEx`3/`CUIMacroSys`8/`CUIMacroSysEx`8/`CMacroSysMan`8/`CUIEnchantDlg`2 具名 EA；r40 铺壳；但 `r02:153`「未预判」+ `feature-gap:140`「深度仍 A/B 层」未撤 | **EA 缺口销案；深度 🟡 未闭环** |
| 2 | **QuestAlarm 🟡 未接** | `r16:162`/`r00:75`/`r14:102`/`feature-gap:140`「Alarm 未接」 | r40 铺 `QuestAlarm` 壳；Angel `CUIQuestAlarm` 6 方法 EA（`angel-corewindow-ea.json:1818-1853` DeleteQuest/RemoveQuestByIndex/DeleteQuestByIndex/Draw/RemoveOldQuest/Create）；`r01:140` Draw 是 >50KB「未逐行人工核完」；**无 2026-11 文档逐条撤销「未接」** | **壳铺+EA 已取，「未接」挂账未重判 → 未销案** |
| 3 | **ToolTip 🟡** | `r16:165` B `sub_8E49B5`；`feature-gap:140` C13 + `:141` C14；`PLAN.md:20,34` | Angel `CUIToolTip` 26 方法全表 + `r01:78-82` 单体引擎/13+ 档字体；B8 五条实测源码已改；`_w5_tooltip_root.txt` 149 行节点树证无九切片；`asset-node-inventory.md:11-12`「不存在 `UIToolTip.img`…原生 exe 内 0 命中」；r03 §9.1 终判已清零 | **原生引擎已校准；C14 资源边界旧账须回写 + IMG 阻塞** |
| 4 | **CashShop 礼物/礼包深度 🟡** | `r16:167` A；`r11:119`「礼物(0x04)/礼包(0x1E)/购买结果回包 handler 未接」；`feature-gap:161` C34 + `:162` C35 | Angel `CCashShop` 66 方法含 `OnBuyPackage`/`IsUsableItemGift`/`OnGiftMateInfoResult`/`SendGiftsPacket`（`r02:74`）+ `r01:91-96` 4 子窗 Char/Tab/List/Status；r40 铺 5 交易壳；`open-issues:899-901` 废「点了没反应」；`asset-node-inventory.md:94`「CashShop.img 23 根…素材齐，是接线/协议问题」；r02 §8.4b 已证 `UICashShop.cpp` 深度接线（Take/Put/Buy/Package 0x1E/4 槽位包/wishlist/3 余额/UICoupon/exit） | **入口/素材/协议销案；礼物/礼包深度与 stub 文案 🟡 未闭环** |

**r16 四个 B 锚点复核**：Maker `sub_822350` 达 C（`r16:25/:55/:180`、`r01:133`），Angel 侧 `CUIItemMaker` **22 方法**（`angel-corewindow-ea.json:1458-1595` OnCreate/SetItemSlotRect/DoesSatisfyPreCondition/IsAbleToMake/IsExistMakableItem/IsMatched_WithCurrentSetting/SetComboBox_Category/ClearItemSlot/Update/Draw/GetTotalCostToMake/GetTotalCostToDisassemble/DrawMeso/DrawRecipe/DrawGem/OnMouseMove/SetToolTip/OnButtonClicked/ConfirmMake/StartItemMake/IsEnoughMeso/RequestItemMake/IsExistEmptySlot）→ C 维持 + 具名升级；`sub_8E49B5`（size `0x215f`）**不在** Angel `CUIToolTip` 26 方法起始 EA 表内（最接近 `0x8e6e23 ClearToolTip`/`0x8e8252 SetToolTip_Equip 0x1e47`）→ 见复核点 #11；ViciousHammer `sub_82A9BC 0x3bc`/VegaSpell `sub_82BB83 0x708` 本次 `angel-corewindow-ea.json` grep **未命中具名类**（需另查），但 `evidence/r24/{vicioushammer,vegaspell,cashgachapon}_wz_nodes.txt` 已有完整 WZ 节点树（r16 未引用）；CashGachapon `sub_999E4D 0x43c` 与 Angel 逐字吻合。[观察+推断]

---

## §3 常量表

### A. 原生 IDA 函数 EA / size（r16 B 锚点表 `:63-69`，全 `sub_*`，2026-10 旧账）
ViciousHammer `0xb394e8` gauge → `sub_82A9BC 0x3bc`｜VegaSpell `0xb395d4` → `sub_82BB83 0x708`｜CashGachapon `0xb3de68` → `sub_999E4D 0x43c`｜PartySearch/Alliance `goPartySearch 0xaf2748` → `sub_49D0B6 0x833`/`sub_49D90D 0x66c`｜Quest → `sub_83EA05`（及 `sub_87CE25`）`0xaa0`｜UserList → `sub_917546 0x60a`｜ToolTip → `sub_8E49B5 0x215f`｜PartyRace `0xaf4dd8` → `sub_56BD3C 0x860`｜DualMobGauge `0xaf4fbc` → `sub_56C9B7 0x866`｜Delivery `indow.img/Delivery/* @ 0xafcc7c` xref→构造未落盘｜PartySearch 控件叶 `indow.img/PartySearch/Bt* @ 0xb3a714` 回指 `sub_49D0B6`｜Maker（唯一 C）`sub_822350`｜StatusBar CreateCtrl `BtNPT @ 0xb3bab4` → `sub_8D01B2` CreateCtrl `1000–1007`｜DrawWorldItems `sub_624E13` B+｜MobGage `sub_5336CA` / FIELD_EFFECT mode5 `sub_5330F7`｜连锁页码 `sub_83EA05` / `sub_83FEC6("%s ( %d / %d )")` pool 4497=`Chain Quest`。

### B. Angel 083 具名类 EA 与 size（`r02:84-100`/`r02:108-119`，2026-11 现场）
`CCashShop::OnKey 0x469626 0x8b`｜`CUICashItemGachapon::OnCreate 0x999e4d 0x43c`｜`CUIItem::OnChildNotify 0x81d01f 0x211`｜`CUIKeyConfig::OnButtonClicked 0x8335d0 0x15c`｜`CUIKeyConfig::GetShortCutIndexByPos 0x833a65 0x99`｜`CUIMacroSys::OnButtonClicked 0x8aa134 0xe8`｜`CUIMacroSys::GetIndexByPos 0x8b8de3 0x80`｜`CUISkill::OnSkillLevelUpButton 0x8acf1a 0x31e`｜`CUIStat::OnCreate 0x8c4c7f 0x435`｜`CUIStat::OnButtonClicked 0x8c522f 0x6b`（`r01:27` 写 `0x8c5245`）｜`CUIToolTip::SetToolTip_Equip 0x8e8252 0x1e47`｜`CUIToolTip::GetFontByType 0x8f36a1 0xdc`｜`CWorldMapDlg::CreateWorldMapDlg 0x9eb366 0x3f5`｜`CUIToolTip::ClearToolTip 0x8e6e23`｜`SetBasicInfo 0x8e6e52`｜`SetToolTip_String 0x8e6e7d`｜`ShowItemToolTip 0x8f5b20`｜`CUIQuestInfo 0x87b921 GetQuestInfo`…`0x8843a4 OnToggleDetail`（15 方法）｜`CUIQuestAlarm 6 方法`（`angel-corewindow-ea.json:1818-1853`）｜`CUIItemMaker 22 方法`（`:1458-1595`）｜`CUIMedalQuestInfo 10 方法`（`:1742-1801`）；`r01:10` `angel_funcs 441 / beidou_present 439 / missing 2 (0x6cec47, 0x8aa4e4)`；`feature-gap:612` Angel 3122 修饰名/361 类/2612 方法、函数总数 55016 vs 55655、imagebase `0x400000`、strings 1281 vs 3889。

### C. ctrl id / 掩码 / 几何常量
| 常量 | 值 | 来源 |
|---|---|---|
| `CUIStat::OnButtonClicked` 10 case | `2000→2048(0x800)` `2001→0x2000(8192)` `2002→64(0x40)` `2003→128(0x80)` `2004→256(0x100)` `2005→512(0x200)` `2006→ToggleDetail` `2007/2008→AutoApUp(1)` `2009→AutoApUp(0)` `default→CUIWnd::OnButtonClicked` | `r01:27-45` |
| `CUIItem::OnChildNotify` ctrl id | `2000`+`a3==500→OnTabChanged`；`2001`+`a3∈[0x12C,0x130]→ClearToolTip`；`a3==101` 分支 `0x7D4(2004)=SP_698_ITEM_SORT+sub_81C2D1`、`0x7D5(2005)=SP_4501_EXPAND_INVENTORY+sub_81C2D1` | `r01:73-76`/`r02:44` |
| 当前端背包 ctrl id | `0x7D3=BtGather`(nState==0) / `0x7D4=BtSort`(nState==1)；`place(BT_GATHER,(W-31,4)); place(BT_SORT,(W-31,4))` 叠同坐标 | `r02:45` 引 `UIItemInventory.cpp:123-125,1440,1867-1868` |
| `CUIMacroSys::GetIndexByPos` 网格 | 每格 **32**；行 Y 起点 **65** 步进 **39**（5 行到 **260**）；列 X 起点 **44** 步进 **37**（3 列到 **155**）；拖动 `a2∈[145,177]→*a6=3` | `r01:47-50`/`facts.md:813-823` |
| `CUIMacroSysEx::GetIndexByPos` 网格 | `v8>=223`/`v6!=3`；`v10>=163`/`v9!=3`；`a2∈[146,178]`（**与 CUIMacroSys 不同套**） | `facts.md:954-965` |
| `CUIMacroSys::OnCreate` 滚动条 | `id=3010, a5:3, a6:185, a7:54, a8:125`；`+52=100`；`SetScrollRange(...,3)` | `facts.md:842-855` |
| `CMacroSysMan` 记录长 | `memset(...,0,0x1Du)` / `29*a2+4*a3` ⇒ **29 字节/条**；`FlushToSvr COutPacket(110)`；`CUISkillEx::OnButtonClicked COutPacket(165)`、case `[0x7DA,0x7DE)→a2-2010`、`a2==2023 ShiftMacroUIState` | `facts.md:493-505,1050-1056,1085-1093` |
| `CUIKeyConfig::OnButtonClicked` 5 分支 | `1=Save` `2=InvalidateRect(StatusBar)` `2000=DoModal(CNoticeDlg(0))→Default+ReDrawQuickslot` `2001=DoModal(CNoticeDlg(1))→memset(dword_BED5A0+4,0,0x1BD=445)` `2002=sub_836C9B`；末尾 `UI_Close(...,5)` | `r01:57-60` |
| `CUIKeyConfig::GetShortCutIndexByPos` | 32×32 命中表基址 `dword_BE27E4..BE2C6C`（HUD 面 `dword_BE2DB4..BE2DF4` 不同）；特例 `'6'→42`、`'Y'→29`、`'Z'→56` | `r01:62-65`/`r02:62` |
| `CUISkill::OnSkillLevelUpButton` | `CanSendExclRequest(500,0)`（**500ms 独占门闩**）+ 三档 `SP_3509/3510/3511` | `r01:67-71` |
| `CWorldMapDlg::CreateWorldMapDlg` | `CDialog::CreateDlg(this, a2:67, a3:0, a4:666, a5:524, a6:10, a7:1, a8:0)` ⇒ **666×524, z=10, bScreenCoord=1** | `r01:86` |
| `CCashShop` 子窗偏移 | `CCSWnd_Char this+1360` / `CCSWnd_Tab this+5384` / `CCSWnd_List this+5560` / `CCSWnd_Status this+7644` | `r01:95` |
| `SP_` StringPool | **401 个**，其中 **91 个 `UI_UIWINDOWIMG_*`** | `r01:12` |

### D. 控件尺寸/坐标（IMG + 源码实测）
`UIWindow.img` 顶层 **112** / UI 根 **20**（`asset-node-inventory.md:18/:9`）｜`GameOpt`/`SysOpt` backgrnd **158×284** / **299×365**（`open-issues:687`）｜`Item backgrnd 175x289 / FullBackgrnd 603x289 / BtCashshop 93x18 / BtCoin 14x14 / BtFull,BtGather,BtSmall,BtSort 12x12 / disabled 32x32`（`:28-30`）｜`Stat backgrnd 208x347 / backgrnd2 218x203 / basicStat 159x121 / BtDetail 46x19 / BtAuto 100x35 / BtAuto1/2 73x17 / BtApUp 12x12`（`:33`）｜`Skill backgrnd 175x289 / skill0,1 141x35 / line 141x1 / recommend/0 105x35 / tip0..2 193x102 / CoolTime/0..15 32x32 / BtSpUp 12x12`（`:38`）｜`Equip backgrnd 175x304 / FullBackgrnd 208x291 / pet 177x181 / BtPet1/2/3 30x27 / BtPetEquipShow 67x18 / 槽位 x∈{5,38,71,104,137} y=32+33k / 调色 R'=snap17(0.75R+68.25)`（`:44,46-52`）｜`Quest backgrnd 245x396 / 2 305x396 / 3 296x289 / 4 305x396 / 5 266x117 / Gauge{frame 163x14,gauge 1x11,spot 1x11} / Gauge2{frame 79x8,gauge 1x4} / 6 颗 Bt 各 5 态 / TimeQuest{AlarmClock,TimeBar}`（`:56-58`）｜`ToolTip cash 23x5 / mesos 30x5 / req* 47x11 / Property 31–160x9 / beginner 40x13 / 无九切片`（`_w5_tooltip_root.txt`/`:23-25`）｜`WorldMap Border{0 60x33,1 1x33,2 7x32,4 7x1,7 7x18,6 1x18}+title 1x1`（`:64-65`）｜`Trunk 463x318 / TradingRoom 565x474 / UserInfo 275x199(+backgrnd2 275x399/5 275x361/7 275x404) / BtSort 71x18 / BtPut,Get,GetAll,Exit 70x19 / BtTrade 60x19`（`:103,115,124`）｜关闭钮 `Basic.img/BtClose2 14x14(描边X 0.24) / BtClose 12x12(实心 0.97) / BtUIClose/2 32x15 / 33 窗走 close_btn()`（`:141-146`）｜背包 `W≈596≈FullBackgrnd 603 / 红心 W−51 / 蓝心 W−15 / W−31 槽空 / 折叠高 289`（`open-issues:713-714,627`）｜tooltip 像素 `底 α=0.90 / 边框 (128,128,128) 1px / 分隔线白 α0.4 / DEX/INT y 71→77 / 行距 15px`（`:479-480`）｜NPC 对话框 `α=1.0 / Next=W-78 / OK=W-54 / inset=157`（`skill-effect:33-34`）｜协议 opcode `SKILL_EFFECT 93/0x5D / NPC_ACTION 197/0xC5 / CLICK_GUIDE 162/0xA2 / WORLD_MAP_PLAYERS 0x115(277) / 376 s2c / MAX_PLAYERS=40 / CACHE_TTL_MS=750`（`skill-effect:9-11`/`world-map-players:13-14`）｜协议层总账 `client_out_sent 148/148 / client_in 306/305 / s9_registered_handlers 174 / .dispatch() 917 / c2s 构造点 238 / UIElement::Type 151(149 绑定) / IO/UITypes+Components 247 文件/163 class/654 方法体`（`feature-gap:11,320-326`）｜UserList 五页 `运行时 158 节点名 / 代码 48 字面引用 / 25 不存在(52% 失配) / backgrnd/2/3 312x389 / friend0 281x27 / party1 279x18 / guildmark 20x20 / GuildUnion 20 直接子节点 / meanSAD 好友 11.6 联盟 7.6`（`diagnosis-and-plan.md:14,20,50,59-65,78-80`）｜构建产物 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（`equip-compare:80` 2,548,224 B 2026-10-21；`userlist:155` 4,920,832 B 2026-11-08 01:41；`EXITCODE=0`）。

---

## §4 Angel 新校准应优先复核的 15 个点
1. `CUIStat::OnButtonClicked @ 0x8c522f/0x8c5245` 10 ctrl id + 6 掩码：逐掩码核 `AutoAssignApPacket(to_code(primary), ap, second_code, 0)`（`UIStatsInfo.cpp:726-752`）是否覆盖 `0x800/0x2000/0x40/0x80/0x100/0x200`，及 `2006 ToggleDetail` 与当前端三 case（`:773/:779/:780`）是否等价。
2. `CUIItem::OnChildNotify @ 0x81d01f` 的 `0x7D5`（`SP_4501_EXPAND_INVENTORY`）：当前端 `UIItemInventory.cpp:1872` 仅注「BtCashshop exists only in the expanded frame」无分支；当前端多出 `0x7D3=BtGather` 在 GMS v83 无 case ⇒ 扩展背包语义缺失 + 国服适配须裁决。
3. `CUIMacroSys::GetIndexByPos @ 0x8b8de3` 网格 `65/39/260`、`44/37/155`、每格 32、拖动 `X∈[145,177]→第3列` → 与 `UIMacroSkill` 壳逐点比对。
4. `CUIMacroSysEx::GetIndexByPos` 不同网格 `v8>=223 / v10>=163 / X∈[146,178]`，勿与 CUIMacroSys 混用坐标（对应 `UIWindow.img/SkillMacroEx` `UIWindow.img.top-level.txt:111`）。
5. `CCurseProcess::ProcessString` 脏词过滤整族：`CUIMacroSys::OnButtonClicked` 有 `ProcessString(Buffer,0,1)`+`SP_272`/`SP_3296`；**但 `facts.md:966-978 CUIMacroSysEx::OnButtonClicked` 逐字无 ProcessString** ⇒ 需确认 Ex 版原生确实不做脏词过滤、还是反编译截断；当前端两处全仓零命中。
6. `CUIKeyConfig::OnButtonClicked @ 0x8335d0` 5 分支（1=Save / 2=InvalidateRect / 2000=恢复默认+ReDrawQuickslot / 2001=清空 445 字节 / 2002=sub_836C9B），当前端仅 `UIQuickslotConfig.cpp:92-144 apply_slot_key→commit_quickslots()` + `CHANGE_QUICKSLOT(183)`。
7. `CUIKeyConfig::GetShortCutIndexByPos @ 0x833a65` 32×32 命中表基址 `dword_BE27E4..BE2C6C` vs HUD 面 `dword_BE2DB4..BE2DF4`；特例 `'6'→42`/`'Y'→29`/`'Z'→56`；与 Q2 §16 #2「右侧快捷 8 格无 hit→send_key」同族。
8. `CUISkill::OnSkillLevelUpButton @ 0x8acf1a` `CanSendExclRequest(500,0)` 独占门闩 + `SP_3509/3510/3511` 三档前置 + `_ZtlSecureFuse<short>`/`get_novice_skill_point` → `UISkillBook` 逐条核。
9. `CUISkillEx::OnButtonClicked`（`facts.md:493-505`）`a2∈[0x7DA,0x7DE)→OnSkillLevelUpButton(a2-2010)` / `a2==2023→ShiftMacroUIState` / `a2!=2024→COutPacket(165)`；`ShiftMacroUIState` 9527 B 用 `SP_4345/4346_..._BTMACROIN/BTMACROOUT`。
10. `CUIToolTip` 26 方法单体引擎 ↔ 当前端 5 组件：`GetFontByType` case 1–13+ 取 `this+265..278` IWzFont（13 档以上字体）；`SetToolTip_{Bundle,Package,Pet,Ring,Skill,SlotInc,WorldMap,MacroSysSkill,String_MultiLine}`；当前端 11 文件 + `EquipTooltip.cpp:135 set_equip/:651 draw/:807 draw_preview/:680 backdrop.draw(pos+(-6,-6), FRAME_WIDTH+12, height+32)`；`UI["UIToolTip.img"]["Item"]` 本包不存在（r03 §9.1 已终判）。
11. `sub_8E49B5`（r16 ToolTip B 锚点 size `0x215f`）↔ Angel 26 方法 EA 表**无此地址**（最小 `0x483eed`、ClearToolTip `0x8e6e23`、SetToolTip_Equip `0x8e8252 0x1e47`）⇒ 需确认是 ctor/helper/另一实例/误标；同理 `sub_83EA05`（Quest B 锚点 `0xaa0`）不在 `CUIQuestInfo` 15 方法范围 `0x87b921..0x8843a4`，但 `quest-page-icons:22` 确用它证 Gauge 逻辑 ⇒ 映射到 `CQuest`/`CQuestMan` 数据层或 `CUIQuestInfoDetail`。
12. `CWorldMapDlg` 666×524 / z=10 / bScreenCoord=1 + `SetWorldMapDeeper/Shallower` + `MakeBorder` 八片 `SP_2281..2288`：r02:70 判 s9 适配保留，但尺寸与八片边框拼接**尚未与当前端 `UIWorldMap` 对点**；本包 `WorldMap` 只有 `Border`+`title[1x1]`。
13. `CCashShop` 66 方法深度分支（`OnBuyPackage`/`IsUsableItemGift`/`OnGiftMateInfoResult`/`SendGiftsPacket`/`OnQueryCashResult`/`OnCashItemRes*`/`OnSetWish`/`OnRemoveWish`/`ProcessBuy`）↔ `feature-gap:161` C34 🟡 + `:162` C35 stub、`r11:119` 旧账；`open-issues:899-901` 已废「点了没反应」但 stub 未撤（r02 §8.4b 已证协议深度接线）。
14. `CUIQuestAlarm` 6 方法 + `CUIItemMaker` 22 方法 ↔ `r00:75`/`r14:102`/`feature-gap:140`「QuestAlarm 未接」与 r40 已铺壳；`Draw` 是 >50KB 超大函数未逐行人工核完；Maker 22 方法 ↔ `r16:180` 达 C 精修。
15. ViciousHammer `sub_82A9BC 0x3bc` / VegaSpell `sub_82BB83 0x708` 的 Angel 具名映射：本次 `angel-corewindow-ea.json` grep 未命中（CashGachapon 已吻合）；但 `evidence/r24/{vicioushammer,vegaspell,cashgachapon}_wz_nodes.txt` 已有完整 WZ 节点树（`backgrnd 178x206`/`178x235`/`170x172`、`GaugeBar{bar 164x17,gauge 1x9}`、`EffectE`/`EffectSuccess`/`EffectJackpot` 全帧表）⇒ 建议 ①补采两类 EA ②用 r24 节点树把 r16「仅 B 锚点」三窗推到 C 前段 ③IMG 层阻塞由 r03 收口。

---

## §5 HUD/登录两面旧账冲突在核心窗口面是否有同类（逐条回答）
| 旧账冲突 | 原始出处 | 核心窗口面同类 | 判定 |
|---|---|---|---|
| `channel_count<2` 单频道世界整块跳过 | `r15:104`；`feature-gap:597` §16 #9 P0 维持 `UIWorldSelect.cpp:697-698` | **无同型硬缺陷**：WorldMap `r16:163` A ✅、`feature-gap:73` 把 `WORLD_MAP_PLAYERS(376)` 判「✅ 真机闭环」、`r02:70` 保留 s9 适配；最接近 `feature-gap:149` C22 `UIMapSearchPanel`「早期 4 项未实现 vs 2026-11 面板 ✅ 口径冲突」 | 不同型；C22 有同类口径冲突 |
| `[DBG-CLK]` 底栏按钮回调未证实 | `r15:131`/`r16:147`/`feature-gap:111` B2 | **有同类（本面主缺口形态）**：`feature-gap:158` C31 `UIUserList::change_target/BT_GUILD_CHAT` 预留空实现、`:139` C12 任务窗 `BtOK,BtNo,BtAlert,BtMarkNpc` 预留（`UIQuestLog.cpp:182,903,955`）、`open-issues:818-824` C13、`feature-gap:128` C1 `UIMTS::update(){}`；共同形态 = 控件已建/坐标齐/点击可达但回调未抽全，正是 `r16:152`「坐标近 C，交互行为未达 C」 | 有同类且是主缺口形态 |
| `TemporaryStatView` 全缺 | `parity-gap:99` ❌、`feature-gap:594` §16 #8 维持 ❌ | `UIWindow.img.top-level.txt:57 TemporaryStatView` 节点存在；Angel 侧 `CTemporaryStatView` 仅 `angel-corewindow-facts.md:188` 一次 `SP_3550_GUILD_QUEST`，r01/r02 12 类清单未列 | **同类真全缺，Angel 本面未补其 EA → 建议主 agent 补采** |
| Q2 §16 #2 右侧快捷 8 格点击 | `feature-gap:588`/`:110` B1「无槽位 hit→send_key，只认 SKILL/ITEM」 | 同型「命中表未落地」：`r01:62-65` `CUIKeyConfig::GetShortCutIndexByPos` 32×32 命中表 + 特例、`r01:47-50` `CUIMacroSys::GetIndexByPos` 网格、`r02:52` 宏窗几何待核、`r02:64` 宏键位拖拽覆盖度待定 | 有同类（原生常量已取、当前端未核） |
| Q2 §16 #9 单频道 P0 | 同上 | 见第一行 | 不同型 |

**§16 中核心窗口面「维持 ❌」项**：`#8 TemporaryStatView`、`#12 公会副本 HUD`、`#14 Title→壳维持`、`#17 伤害皮肤+背包格锁`、`#18 窗口现金商城 #37`、`#19 扩展装备栏 #38 + 签到双端`、`#20 交易所+图鉴增强+300级`；「维持 🟡」`#13 中文 IME`（`feature-gap:165` C38 已证 `SDL_StartTextInput` 开启、`charset.md:193`「仍关 IME」是 2026-10-31 旧态）、`#15 公会 BBS`、`#16 地图搜索逻辑`。

---

## §6 IMG 节点树 / 截图 / layout JSON 证据清单
**总判定**：orange-wz MCP **未接入本会话 Host**（`r01:21`/`r02:23`/`r02:145` 三处逐字确认，`mcp_connector_status` 仅 `json-photoshop`；`~/.cursor/mcp.json` 有 orange-wz 但 Host 未注册；`login-flow-regression-fix/notes/img-evidence-audit.md:7` 已知解析缺陷）→ **不是「文件未找到」而是「MCP 通道阻塞」**。仓库内替代证据：
- `docs/features/v83-hud-quest/evidence/UIWindow.img.top-level.txt` — 112 顶层节点全集，核心行 `:3 Item :4 Equip :5 Stat :6 Skill :7 ToolTip :8 Shop :17 UserInfo :18 WorldMap :22 Quest :23 QuestAlarm :31 KeyConfig :32 MiniMap :46 Macro :47 QuestIcon :49 UserList :57 TemporaryStatView :62 EnchantSkill :74 SkillMacro :84 Maker :88 MacroSkill :96 ViciousHammer :97 CashGachapon :102 CashTradingRoom :106 SkillUp :107 VegaSpell :111 SkillMacroEx :112 SkillEx`。
- `docs/features/ui-v83-window-restore/notes/asset-node-inventory.md` — 权威件表 **147 行**（`:3` 命令 `python tools/nx_inspect.py list "E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx" <depth> <path>`）、20 根/112 顶层/各窗 WxH/换代三窗件表/关闭钮 4 类；`:16` 警告「`origin [vector]` 行不可信（指针样乱码）」、`:17` 2026-11-01 已修 `value()`。
- `docs/features/ui-v83-window-restore/evidence/_w5_tooltip_root.txt` — **149 行** `UIWindow.img/ToolTip` 节点树（`Equip/Can|Cannot|Dot|GrowthDisabled|GrowthEnabled|ItemCategory|Property|Speed|WeaponCategory` + `cash 23x5` + `mesos 30x5`；无九切片）。
- `docs/features/beidou-v17-parity/notes/ui_layout_dump/`：`_skill_nodes.txt`（UTF-16LE 45 行）、`_item_nodes.txt`（87 行，尾部混入 `nx_inspect.py` PowerShell 报错栈）、`_spup_nodes.txt`（7 行）、`_spup_nodes2.txt`（15 行 BtSpUp 4 态各 12x12）。
- `evidence/r24/vicioushammer_wz_nodes.txt`（36 行 `EffectE/0..13 172x86`/`EffectP/0..2 51x51`/`GaugeBar{bar 164x17,gauge 1x9}`/`backgrnd 178x206`/`BtCancel/BtStart`）、`r24/vegaspell_wz_nodes.txt`（93 行 `backgrnd10/60 178x235`/`Count/0..9 9x10`/`FailWnd/SuccessWnd 152x159`）、`r24/cashgachapon_wz_nodes.txt`（50 行 `backgrnd 170x172`/`EffectJackpot/0..16 166x77`/`BtClose/BtOpen`）→ **r16 标「仅 B 锚点」三窗其实已有完整 WZ 节点树，但 r16 未引用**。
- 其它：`evidence/r21/messenger_wz_nodes.txt`、`r22/partysearch_wz_nodes.txt`、`r24/{remotegachapon,family,memo,delivery,duey,storebank}_wz_nodes.txt`、`evidence/ida_uiwindow_nodes_in_exe.txt`（9 行串计数 `PartyRace 8 / Maker 5 / DualMobGauge 4 / Quest 3 / CashGachapon 3 / ViciousHammer 2 / VegaSpell 2 / ToolTip 2 / UserList 1`）、`w2-hud-buttons/keyconfig-nodes.txt`、`userlist-five-tabs/evidence/userlist_runtime_nodes.txt`（**802 行**节点名/类型/尺寸/origin）+ `node_ref_audit.txt`（48 字面引用逐项对照）；`v83-hud-quest/evidence/StatusBar.img.xml` 唯一 `*.img.xml`（HUD 面非核心窗口）。
- 截图：`docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/打开了S-E-I-K页面.png` — **唯一 Stat/Equip/Item/KeyConfig 四窗同屏正确端实拍**（`r16:160`）；同目录 `键盘设置.png`（`r16:164`）等 7 张；`ui-v83-window-restore/evidence/r15-newrefs/rgb/img2_tooltip4x.png`(+4xb/4xc)/`crop_img5_tooltip_4x.png`/`crop_img4_tooltip_6x.png`/`crop_img5_tooltip_6x.png`（B8 依据 + `e75687e9` 原生参考帧）；`quest-page-icons/evidence/UIWindow.img_QuestIcon_{3..9}_0.png`/`UIWindow.img_Quest_{basic,summary,reward,select,prob}.png`。**核心窗口整窗实机截图（Skill/SkillEx/Macro/Enchant/CashShop/Maker/QuestAlarm/WorldMap 单窗）在前 100 命中中未见**；`open-issues.md:980` 参考帧逐窗裁片 `../evidence/refcrops/` 线索。
- layout JSON：`docs/features/**/layout*.json` **仅 4 个均非核心窗口**（`char-stats-ui-ps/ui/detail/v4/layout_cn.json` Stat 扩展、`map-search-ui/resources/layout.json`、`map-search-ui/ui/search_panel_backgrnd/v3_pipeline/layout.json`、`map-info-panel/resources/layout.json`）+ `UI/MapInfo/layout.json`；**核心窗口专用 layout*.json 未找到**；`refs/*.xml` No files found。

---

## §7 其它核心窗口挂起项（补摘）
- `r16:16/:103/:115/:127/:139/:203/:219/:270` 各批「未达 C」、`:152`「坐标近 C，交互行为未达 C，是主链最大可操作缺口」、`:231`「Core 深度 `sub_83EA05` Quest / `sub_917546` UserList / SkillMacro 叶（P1）」、`:232` Wave A、`:235`「**不要做**：`rea analyze` 整包 BeiDou.exe」、`:280`「验收以正确客户端截图 + s9 包序为准；C 层证据是对照手段不是目标本身」；`r15:31`「主要是对照/实现未闭环/接线与条件过滤有问题，不是没把 exe 全部反编译完」、`:150-154` P0/P1/P2 表。
- `feature-gap:473-520` §14 ⊘ 明确不做总表（`:520` GameOpt D3 多选/底栏空壳入口/两窗居中重锚本版不做）、`:527-567` §15 ⏳ 挂起表、`:644` 主源 `pending-live-tests/README.md`（474 行）；`:99/:100/:103` TemporaryStatView/Title 已证伪/Ranking 观感已对齐。
- `guild-bbs-ui/notes/layout.md:14`「真机若与烘焙槽位偏差，优先改本表常量，勿猜改贴图」；`gameopt-invite-mask/README.md:25/:23` 显式预留 + `UNION_APPLY` 原预留已消解；`map-expansion/PLAN.md:18/:45` 原生缺失资产 `WorldMap/WorldMapSearch` 搜索栏/`BtSearch`/`BtAutoFly_1`/`BtNaviRegister`/`MiniMap/ListNpc`/`MiniMapSimpleMode` 保持显式预留不编造行为。
- `open-issues.md:565`「A 窗根没有关闭钮 Item/Stat/Skill/Quest/Trunk/TradingRoom/UserInfo…旧写法造出空贴图，`Texture::draw` 在 `is_valid()` 直接 return（`Graphics/Texture.cpp:94`）」、`:949`「`UIKeyConfig` 必须同时走 v83→本枚举映射表，否则显示名与实际绑定不一致 — 不做就是半成品」、`:936` `` ` `` 键 41 MENU 23 ALLIANCECHAT 走错分支不开商城、` :941-942`「净结果 7 死键 + 3 语义错位」；`quest-page-icons/README.md:47` 多目标队列本轮不实现；`hotkey-grid-3x3/ui/hotkey-grid/v1/manifest.md:8`「样板：无实测节点导出（orange-wz 未接入）」；`feature-gap:612-633` §17 Angel 使用约束（每次先跑 `server_health` 读 `idb_path`）。

---

## §8 性质标注
- 本文全部结论为 [观察]（逐字引文件行号）/[推断]（四态归类/冲突裁决/覆盖度）/[待实机]（明写需实机），无臆造；子代理声明全程只读、未改任何文件、未连库、未改 MCP 配置。
- 095/Maplestory95 与 REA provider 不可用均按 083/v83 基准如实标注、不作依据；Q2 要求的「四态 + 常量表 + 15 复核点 + 4 冲突状态 + 旧账同类核对 + IMG/截图/layout 证据」全部覆盖。
- 与 r01（Angel 一手语义）、r02（主 agent 跨端销案）、r03（IMG 资源与样式）、r04（当前端源码盘点）共同构成核心窗口面阶段 0 取证集，供 README 六维对照表引用。

---

## §9 增量（第二路收尾重发版 20:38 补回）— `docs/features/ui-v83-window-restore/README.md:68-79` W5–W9 改前→改后定案

> 子代理 `4cfbbb14` 于 20:38:18 重发全文，新读到 `docs/features/ui-v83-window-restore/README.md:68-79` 后补出「5 重点增量对账」。主 agent 复核后把**不与 §1–§8 重复**的排版层硬证据固化成本节。该段是 2026-11 逐窗「改前→改后」定案，直接推翻多条 2026-10/11 台账，并进一步强化三面一致的「排版层已对齐、Angel 只补符号语义+行为层」结论。

### §9.1 W5–W9 逐窗改前→改后定案（逐字，`ui-v83-window-restore/README.md`）

- `:68` 快捷格：「按格数选 `base/quickSlot08|26|32`（**151/466/571 宽**，几何由图集反推：格 **28**、列距 **35**、行距 **34**、首格 **(9,9)**）；键帽层 = `key/0..6` 贴纸 + `FontMemo` 位图数字（1–5）+ 描边文字回退；**废弃旧的「800 设计网格 + right_adj」居中**（IDA 簇 x 末端 795/796 反证原生就是左贴边）」→ 与 HUD 面 `UIStatusBar.cpp:106-118 QS_SLOT=28/PITCH_X=35/PITCH_Y=34/FIRST=(9,9)` 逐值吻合， Angel `GetShortCutIndexByPos @ 0x8de8d5` 给的 32×32/647/427 是**命中表**而非绘制几何，两者不同层勿混。[观察+推断]
- `:69` 「`界面`→ 只开 `UIShortCut`，**11 颗真 `Bt*`**（`x=6, y=20+20i`，底板 **93×245**）并接到既有窗口；`商城`→ 明确「暂未支持」提示，**opcode 不动**；`键盘设定`窗 **5 颗钮补坐标**（此前因哨兵被整批跳过＝「窗里没按钮」）」→ 反证 `asset-node-inventory.md:69`「`ShortCut`…当前代码一个都没实例化」与 `feature-gap:119` B10「「窗里没按钮」前提」。[观察]
- `:71` **功能级根因**：「classic 模式 `is_in_range` 把命中区整体上移 **100px** ⇒ 光标事件**从不**派发给聊天栏，这才是「点了没反应」」→ 与 HUD 面 `feature-gap:111` B2 `[DBG-CLK]`「需实机确认进回调」同族，但**已定位代码根因**，非纯实机待证。[观察]
- `:74` tooltip 根清零 + `TooltipBackdrop` 定案（见 §0 / r03 §9.1）。[观察]
- `:75` 能力值：「`backgrnd` **208×347** 基准重排；`basicStat` 总是画 **(8,195)**；**13 行数值重测（DEX/INT/LUK 曾低 10/18/28px）**；`BtApUp`×6 → **(186,115…299)**；`BtDetail` 角锚 kx=9/ky=4 → **(153,324)**；**收缩钮真身 = `Basic.img/BtHide`（13×12，SAD 模板精确命中）在 (405,326) 且仅展开态可点** —— 旧代码取不存在的 `main/BtDetailClose` 使 `set_detail(false)` 永不可达。数值色定案 **BLACK+A11M**」→ 与 r04 `UIStatsInfo.cpp:129-144/:154-155/:159/:164` 逐坐标吻合；反证 `asset-node-inventory.md:34`「✗ `BtDetailClose`」被当成素材缺口。[观察]
- `:76` 装备：「「左上两钮弹二级密码」定案为**误读烤图** —— 那是 `Equip/BtDetail`「详细信息」46×19 与 `Equip/BtCashshop`「Shop」32×11（全仓/全客户端/exe 内「二级密码」**0 字节命中**），原生**默认隐藏**两颗（匹配度 mae 55.8/61.1）」→ 旧账证伪。[观察]
- `:77` 背包：「tab 底衬 = `Item/New/Tab0`(灰)+`New/Tab1`(粉=选中) 叠 `Item/Tab/enabled|disabled` 文字，`x=5+34i, y=24` 且**只有 0–4**（旧代码取到 5 → 静默失败，SETUP/ETC 错位）；`BtFull`/`BtSmall` 合并到同槽 **(W−51,4)**（曾两颗叠 (109,6)），`BtGather`/`BtSort` **(W−31,4)**；`set_full` 按 `dimension.x()` 重排全部工具位，金币标签 **(141,275)** 不再溢出 289 框高」→ 与 r02 §2.2「两颗叠同坐标 (W−31,4)」并存，但 README.md:77 定案**叠槽是原生设计非缺陷**；反证 `asset-node-inventory.md:30`「代码若取 `"5"` 会静默失败」的现行判定。[观察]
- `:78` 技能：「5 tab 槽0 中心 **21**、间距 **33**（由新手圆点实测 (16,29) 反推）；`CLASSIC_SKILL_OFFSET` **(10,87)→(7,99)**（旧值＝贴图与文字 12px 错位的来源）；`+SP`→**(132,119)**；宏指令钮 `Basic.img/BtMacro` **(121,265)**（**不在** `Skill` 根下）；`recommend[0]`（105×35，origin 16,0）作顶部职业横幅接在 **(5,20)**」→ 反证 `asset-node-inventory.md:40`「✗ 宏指令钮在本根」被当成待办。[观察]

### §9.2 ⚠️ 与 r04 直读源码的冲突（主 agent 标注，须交决策点，不臆造裁决）

- **技能窗偏移口径冲突**：`ui-v83-window-restore/README.md:78` 定案 `CLASSIC_SKILL_OFFSET (10,87)→(7,99)`；但 r04（子代理 20:28 直读当前源码）记 `IO/UITypes/UISkillBook.h:166-216 CLASSIC_SKILL_OFFSET(10,87)` 且 `:170-172` 注释「列表区顶 y=87..257、行高 40、可见 4 行、行左缘 x=10」，`.cpp:388-389` 同样写「起点 (10,87) 行距 40 可见 4 行」。→ **按门禁「checked-in source > docs」以 r04 直读的 (10,87) 为当前真值**；`README.md:78` 记的 (7,99) 或是 W5–W9 当时改过、其后代码回退，或属文档与源码未同步。两种可能都不能凭文档臆断，列 **D 决策点「先实机比对技能窗文字/贴图 12px 错位」** 再定。[观察+推断]
- **统计含义**：本冲突是「文档定案 vs 当前源码」的新一类（前三面主要冲突是「旧台账 vs 现场重算」），README §2.4 冲突表须补一行；不影响「排版层大体已对齐」的总判断，但**局部坐标以源码逐字为准**。[推断]
- **主 agent 再次直读复核（20:39，`read IO/UITypes/UISkillBook.h:160-259`）**：源码逐字 `:173 static constexpr Point<int16_t> CLASSIC_SKILL_OFFSET = Point<int16_t>(10, 87);`，且 `:170-175` 注释「原生（BeiDou V17.7）经典窗单列技能行基准：列表区顶 y=87..257（170px），行高 40 → 可见 4 行；行左缘 x=10…`CLASSIC_ROWS = 4`」；同文件 `:196 TAB_SLOT0_CENTER = 21`、`:210 MACRO_BTN_TOPLEFT = Point<int16_t>(121, 265)`、`:176-177`「行内 SP 升级『+』钮…实测 + 号框左上角 (132,119)」`CLASSIC_SPUP_OFFSET = (130,20)`、`:207 BOOK_TEXT_CENTER_X = 105` → **与 `README.md:78` 记的「槽0中心 21 / 间距 33 / 宏指令钮 Basic.img/BtMacro (121,265) / recommend[0] origin 16,0 接 (5,20)」逐值吻合**，唯独 `CLASSIC_SKILL_OFFSET` 文档写「(10,87)→(7,99)」而源码仍是 (10,87)。→ 判定：`README.md:78` 的 (7,99) 属「文档曾改过后源码回退 / 或文档笔误」，**不得据此改源码**；且 `:177 (130,20)` 相对行起点 + `:173 (10,87)` 恰得绝对 (140,107)，与文档「+SP→(132,119)」也不是同一套换算，进一步说明该段坐标口径混用。列 **D 决策点：先实机比对技能窗文字/贴图 12px 错位，再定是否动 `CLASSIC_SKILL_OFFSET`**。[观察+推断]

### §9.3 对 Q2 清单旧账的进一步反证（补 §1.2）

- `:69` 11 颗真 `Bt*` 已实例化 + 键盘设定窗 5 钮补坐标 → 反证 `asset-node-inventory.md:69`「一个都没实例化」与 `feature-gap:119` B10「窗里没按钮」前提。[观察+推断]
- `:71` `is_in_range` 上移 100px → 反证 `feature-gap:111` B2「需实机确认进回调」（已定位代码根因）。[观察]
- `:75` 收缩钮 `Basic.img/BtHide` 定案 → 反证「`BtDetailClose` 素材缺口」。[观察]
- `:77` 背包 tab 仅 0–4 + 金币标签不溢出 → 反证「SETUP/ETC 错位是缺资源」。[观察]
- 与 §1.4「§9.5 覆写 TOP20」「冲突 #6 以 148 为准」「冲突 #5 376 真机闭环」同属「2026-11 现场重算推翻旧账」族。[观察]

### §9.4 ⑤ `UIToolTip.img` 复核：子代理明确「同意第三路终判，无反证」

`4cfbbb14` 独立读证同向：`ui-v83-window-restore/README.md:74` 逐字「根 `UI["UIToolTip.img"]["Item"]`（不存在）**全部清零**…框体真身定案 = 程序绘制 `TooltipBackdrop`（黑 0.9α + 1px 灰边 (0.5,0.5,0.5)，托管 `TextTooltip.h`）…装备框固定宽 **264**」；交叉印证 `asset-node-inventory.md:11-12`、`_w5_tooltip_root.txt:1-149`、`open-issues.md:479` B8.1 逐像素 α=0.90/边框 (128,128,128)。**唯一提醒（非反证，台账同步项）**：`feature-gap:141` C14 与 r02 §2.4 仍写「5 组件指向不存在的 `UIToolTip.img/Item`」→ 文档层未同步，README 定稿标注销案即可。[观察+推断]

### §9.5 本节性质标注
- 本节全部 [观察]（逐字引文件行号）或 [推断]（冲突裁决/覆盖度归类）；子代理声明全程只读、未改任何文件。
- 本节与 §0–§8 共同构成 r05 完整四态审计；r01–r05 五份笔记 + 6 份 evidence 齐备，核心窗口面阶段 0 取证收口。
