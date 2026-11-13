# r03 — 状态栏 HUD 面「既有文档四态审计」+ 主 agent 销案复核

> 来源：subagent `1605f5df` 只读四态清单（2026-11-08 19:20，13K 全文），主 agent 结构化归档并前置复核块。
> 门禁：`xiaoye-maple-story-dev` 已加载；参照基准 083/v83；**禁止引 095 线**（`docs/features/port-095/notes/opcode-v95.md:758,760` 的 `CUIStatusBar::SendGroupMessage(0x87f7f0)` / `SendCoupleMessage(0x87b3e0)` 仅登记存在、不作依据）。
> 标注 `[观察|推断|待实机]` 沿用子代理原文；主 agent 新增块标 `[主 agent 复核]`。

---

## 0. 主 agent 销案复核块（★ 本轮最重要增量，逐字源码核验）

子代理清单以「既有文档」为准，故把 r16/r15/feature-gap 的 ❌ 全部登记为「未闭环」。主 agent 直接读当前端源码逐字复核，**其中三条 P0 硬缺口实际已被销案**，写六维对照表时必须以源码为准：

- `[主 agent 复核]` **销案 1：底栏按钮回调（r16:147 / r15:131,151 / feature-gap B2）**。`IO/UITypes/UIStatusBar.cpp:747` `Button::State UIStatusBar::button_pressed(uint16_t id)` 完整 switch：`:751 BT_CLASSIC_SHOP→EnterCashShopPacket().dispatch()`、`:759 BT_CASHSHOP→EnterCashShopPacket`、`:764-783 BT_CLASSIC_MENU|BT_MENU→UIGameMenu / toggle_menu()`、`:785 BT_CLASSIC_EQUIP→UIEquipInventory`、`:795 INV→UIItemInventory`、`:805 STAT→UIStatsInfo`、`:815 SKILL→UISkillBook`、`:826 KEYSET→UIKeyConfig`、`:834 QS|QSD→quickslot_active 翻转`、`:841 NPT→EnterMtsPacket().dispatch()`、`:849 SHORT→UIShortCut`、`:865 WHISPER→chatbar->focus_whisper()`、`:871 CLAIM→UISlideMenu.toggle_expand()`、`:883 OPTIONS→toggle_setting()`、`:888 CHARACTER / :893 COMMUNITY / :898 EVENT / :903 FOLD_QS / :908 EXTEND_QS / :913 BT_MENU_QUEST … :1166 BT_EVENT_DAILY`。→ **不是「回调/DBG 未闭环」，是完整接线**。
- `[主 agent 复核]` **销案 2：右侧快捷 8 格点击（r16:148 / r15:133,150 / feature-gap B1 / TOP20 #2）**。`IO/UITypes/UIStatusBar.cpp:2215-2231` `Cursor::State UIStatusBar::send_cursor(bool clicked, Point<int16_t> cursor_position)` → `:2219 classic_qs_slot_at(cursor_position)` → `:2224 activate_classic_qs_slot(slot)` → `:2197-2213` 取 `keyboard.get_quickslot_key(slot)` / `CLASSIC_QS_KEYS[slot]` → `keyboard.get_maple_mapping(maple_key)` → `Stage::get().send_key(mapping.type, mapping.action, true)` + `Sound(BUTTONCLICK).play()`；`:2168-2170` 槽矩形用 `QS_FIRST_X + col*QS_PITCH_X` / `QS_FIRST_Y + row*QS_PITCH_Y`。→ **hit→keymap→send_key 链路完整存在**，旧账「仅绘制 classic_qs_icons、无槽位 hit→send_key」已过时。
- `[主 agent 复核]` **销案 3：按钮 id 旧注释误标（r15:40,135）**。`IO/UITypes/UIStatusBar.cpp:1902-1907` 注释仍写 `QuickSlot=2005/(768,515) QuickSlotD=2006/(573,515)`，而 Angel `CUIStatusBar::OnCreate @ 0x8d01b2` 逐字为 `1006/(768,515)` 与 `1009/(573,515)`（后者 `OnButtonClicked` 1009→`SendClaim()`，节点 `SP_1976_BTCLAIM`）。→ **注释编号错误属零风险可改的一致性缺口**（不改行为）。
- `[主 agent 复核]` **新真缺口 A：聊天业务规则整族缺失**。grep `IO/UITypes/UIChatBar.cpp` 的 `脏|curse|swear|spam|flood|刷屏|重复|禁言|2000|same_line|repeat` **仅 1 命中**（`:2486` 无关注释）。而 Angel `CChatHelper::TryChat @ 0x4904be` 逐字含 `CCurseProcess::ProcessString` 脏词过滤、2000ms 刷屏限流、连续 4 条同句检测、禁言 `SP_893`、系统通道 `CHATLOG_ADD(sString,nType:8)`、`SP_272_CURSING_IS_NOT_ALLOWED` / `SP_891_REPEATING_THE_SAME_LINE...` / `SP_892_TOO_MUCH_CHATTING...`。→ **当前端完全没有这条业务链**（非「未闭环」，是「未实现」）。
- `[主 agent 复核]` **新真缺口 B：IME 候选窗链路差异**。`IO/Window.cpp:901-925` 已 `SDL_StartTextInput`（IME 开启）、`IO/UI.cpp:293`「Desktop uses KEY_DOWN ASCII (IME disabled)」、`:501`「Composed text (IME / clipboard) may be UTF-8 (CJK)」、`IO/Components/Textfield.cpp:279`「IME / clipboard CJK commits render via the FreeType glyph cache」、`Textfield.h:55-58` `accept_unicode` 默认 false。Angel `CCtrlMLEdit` 则有 `OnIMEComp @ 0x4d33c2` / `CreateIMECandWnd @ 0x4d56b4` / `DestroyIMECandWnd @ 0x4d5be0` **原生候选窗状态机**。→ 当前端是「系统 IME + UTF-8 提交」，无原生候选窗，属样式/交互差异（登录面 charset.md:193「仍关 IME」同为旧态，一并销案）。
- `[主 agent 复核]` **新真缺口 C：QS 几何常量不一致**。Angel `GetShortCutIndexByPos @ 0x8de8d5` = 每格 **32×32**、`a1-647` / `a2-427`、8 槽坐标表 `dword_BE2DB4..BE2DF4`；当前端 `UIStatusBar.cpp:106-118` = `QS_SLOT=28 / QS_PITCH_X=35 / QS_PITCH_Y=34 / QS_FIRST_X=9 / QS_FIRST_Y=9`。→ 数值口径不同（当前端是「28 格 + 35/34 间距」的多行网格，原生是「32 格」），但当前端坐标已用正确端截图逐像素对齐（`:1926-1930`、`:1921` 注「格 28x28、首格 (9,9)、列距 35、行距 34——见文件顶部 QS_* 推导」）。**改回原生 32×32 会破坏已对齐的实机排版 → 须裁决，不得盲改**。
- `[主 agent 复核]` **部分销案：聊天窗拖拽缩放**。`IO/UITypes/UIChatBar.cpp` 存在完整拖拽链（`:52 drag_direction`、`:113 dragarea`、`:525-597`、`:858-870 classic_resize_h0 + (classic_resize_y0 - cursor.y())`、`:941-942`、`:982-1445` 三向 DOWN/LEFT/DOWNLEFT、`:2061 indragrange`、`:2153 get_dragarea_position`），`UIChatBar.h:55 send_scroll(double yoffset)` 历史翻页。→ 交互「有」，但常量体系与 Angel `ChangeChatWndSize`（h 26..507 clamp、行高 13、509-h、滚动条 id 1010、`SetScrollRange(m_aChatLog-行+1)`）**不是同一套**，属一致性待核而非缺失。

> 结论：子代理清单的「未闭环」段是 **2026-10-21 ~ 10-23 的历史账**；按证据优先级（源码逐字 > 现场重算 > 旧基线），HUD 面的真缺口已从「坐标/回调/QS 点击」收敛为「业务规则（脏词/刷屏/禁言）、IME 候选窗、SetChatType 三态、CUIPartyHP、按钮闪烁/通知链、原生几何常量口径」六类。

---

## 1. 已校准（子代理原文，路径:行号保留）

- `docs/features/statusbar-hud-angel-calibration-2026-11/evidence/angel-hud-ea.json:2-4,1498-1743` — 本次校准产物已落盘：`"source": "13339 Angel v83 (statusbar-hud face)"`、`total_funcs: 548`、`total_classes: 47`，`CUIStatusBar` 自 `:1498` 起 41 方法含 `ea`/`decorated`/`demangled`。[观察]
- `.../angel-hud-ea.json:1500` / `.../angel-hud-decompile.json:27-30` / `docs/features/beidou-v17-parity/notes/evidence/r15/ida_decompile_8D01B2.json:2` — `CUIStatusBar::OnCreate = sub_8D01B2 = 0x8d01b2`，Angel 与 BeiDou 同代同址（imagebase 均 `0x400000`）。[观察]
- `.../angel-hud-decompile.json:42-45` — `OnButtonClicked @0x8d423d` 全分发表已反编译（case `0x3E8`→`SendMigrateToShopRequest`、`0x3E9`→`sub_A12522`、`0x3EA`→`UI_Menu`、`0x3EB`→`SetChatType(1)`、`0x3EC`→`SetChatType(3)`、`2000`→`UI_Toggle(1)`、`0x7D1/2/3`→`UI_Toggle(0/2/3)`+`SetButtonBlink`）。[观察]
- `.../angel-hud-decompile.json:167-170` — `GetShortCutIndexByPos @0x8de8d5`：`v3=a1-647`、`v7[2]=v7[0]+32`、`v7[3]=v5+32`、`dword_BF0484(...,a2-427)`、坐标表 `dword_BE2DB4..&dword_BE2DF4` ⇒ QS 原点(647,427)、槽 32×32、8 槽×2 int。[观察]
- `.../angel-hud-decompile.json:162-165` — `HitTest @0x8de7da`：`v7=507`、`v8=507-*(this+831)`、`a2<580 && *(this+832)==1`、`a2>647` / `a3>=427`、`LABEL_25: a2>580&&a2<647&&a3<507`。[观察]
- `.../angel-hud-decompile.json:192-195` — `MakeCtrlEdit @0x8dfb36`：聊天编辑框 `a3=1011, a4=85, a5=520, a6=440, a7=12`（id=1011 @(85,520) 440×12）+ `ZAllocEx::Alloc(184)` / `Alloc(268)`。[观察]
- `.../angel-hud-decompile.json:32-35` — `OnChildNotify @0x8d419c`：`a2==1010` 且 `0x12C<=a3<=0x130`→`ChatLogDraw`、`a2==1012&&a3==600`→`sub_8D4D43`、`0x7D0<=a2<0x7D4&&a3==100`→`SetButtonBlink(a2-2000,0)`。[观察]
- `.../angel-hud-decompile.json:187-190,207-210,695` — `ToggleQuickSlot @0x8df4b1`（`*(this+836)` ↔ `CConfig+22`）、`CQuickslotKeyMappedMan::OnInit @0x72b82a`（`DecodeBuffer(...,0x20u)`、空则 `DefaultQuickslotKeyMap`、`ReDrawQuickslot @0x8d82a7`）、`CUIKeyConfig::OnButtonClicked @0x8335d0` 已落盘。[观察]
- `docs/features/beidou-v17-parity/notes/evidence/r15/statusbar_createctrl_coords.txt:1-11` — 11 行 CreateCtrl `id/x/y` 表与 `init_classic` 网格一致。[观察]
- **`docs/features/v83-hud-quest/evidence/StatusBar.img.xml:1-519` — 完整 `StatusBar.img` IMG 节点表**（`base/backgrnd 800×71`、`backgrnd2 570×71`、`quickSlot 151×80`、`chatTarget 81×20`、`chat 566×5`、`gauge/bar & graduation 340×31`、`hpFlash/mpFlash 109×18 delay=130`、`BtShop/Menu/NPT/Short 54×34 四态`、`BtWhisper 12×19`、`BtClaim 20×19`、*`Key 28×20 delay=500`、`key/0..7`、`number 5×7`、`FontMemo 6×7`）→ 这是比 r02 更全的资源侧一手表。[观察]
- `docs/features/beidou-v17-parity/notes/r15-baseline-ui-gap-vs-screenshots.md:35` — 「两端 `Data/UI/*.img` SHA256 全同 → 底栏『不像』不是缺资源」。[观察]
- `docs/features/beidou-v17-parity/notes/r15-baseline-ui-gap-vs-screenshots.md:164` / `evidence/r15/rea_search_StatusBar.txt:65-66,70-71` — `UI/StatusBar.img/BtNPT @ 0xb3bab4`、`UI/StatusBar.img/base/chatTarget @ 0xaf72e0`。[观察]
- `docs/features/ui-v83-window-restore/notes/open-issues.md:204-227` — classic HUD `buttons[id]` 空指针解引用「已修 + 已线上复验」（构建 `6AE608CB` / md5 `4231856851ccfa29`），修法 `UIStatusBar.h` + `.cpp:1612-1740` 加 `button_at(...)` / `set_buttons_active(...)`。[观察]
- `docs/features/level-cap-999-2026-11/README.md:181` — EXP `[3470.01亿/27.72亿]` 描边文字、`[100.00%]` 位图字库居中、中文「亿」正常已实证。[观察]
- `docs/features/parity-gap-2026-11/README.md:50-51` — `183 CHANGE_QUICKSLOT` 的 `UIQuickslotConfig` + `commit_quickslots` 已接；`4096 SET_HPMPALERT` ✅ Wave1 上行+回灌（端到端待 `use_server_auto_pot`）。[观察]
- `docs/features/minimap-mode-switch/README.md:24` — minimap 悬浮 tip 框体「已与原生一致」（`TooltipBackdrop`，实心深蓝 α1.0 + 灰 1px 边）。[观察]
- `docs/features/feature-gap-inventory-2026-11/README.md:612` — Q1 定稿：Angel(13339)=同一代 v83 另一份带 MSVC 修饰名 IDB（3122 修饰名/361 类/2612 方法 vs BeiDou 50/10/0），函数 55016 vs 55655、版本串逐字相同 →「符号详细 ≠ 功能更多」。[观察]
- `docs/features/beidou-v17-parity/notes/r201-login-overlay-playbook.md:14` — 正确端 `进入游戏页.png` 标注 `UIStatusBar（CreateCtrl 已锁）`。[观察]

## 2. 未闭环（历史账，逐条标注当前真实状态）

- `docs/features/beidou-v17-parity/notes/r16-rea-coverage-gap.md:20,25,44,56` — StatusBar 仅 CreateCtrl 坐标近 C；已达 C 表标 `sub_8D01B2` C−（行为/点击分发未闭环）。[观察] → `[主 agent 复核]` 已被 Angel r01 + 源码销案。
- `r16-rea-coverage-gap.md:147` — `BtShop/BtNPT/BtMenu/BtShort | C− | 🟡（回调/DBG 未闭环） | decompile sub_8D01B2 内 button handler`。[观察] → **销案**（`UIStatusBar.cpp:747` 完整 switch）。
- `r16-rea-coverage-gap.md:148` — `右侧快捷 8 格点击 | ❌ 仅绘制 | 同函数内 QS hit→keymap 分发`。[观察] → **销案**（`UIStatusBar.cpp:2215-2231`）。
- `r16-rea-coverage-gap.md:149` — `聊天条/chatTarget | B：chatTarget @ 0xaf72e0 | 🟡 | decompile 聊天输入绑定`。[观察] → 部分销案（`UIChatBar` 输入/拖拽/频道齐），`change_target` 仍空实现。
- `r16-rea-coverage-gap.md:150` — `HP/MP/EXP 条 | 坐标侧有注释痕迹 | 🟡 | 从 8D01B2 抽 gauge 控件 id`。[观察] → 部分销案（`draw_classic` gauge/bar 已烘焙）。
- `r16-rea-coverage-gap.md:152,227,137` — 「坐标近 C，交互行为未达 C」/「StatusBar 行为 C：补 handler / QS / 聊天（P0）」/「进图后 HUD 挂载顺序 🟡」。[观察]
- `r15-baseline-ui-gap-vs-screenshots.md:16,24,27` — 「没有。」（BeiDou.exe 未全量反编译）；缺「StatusBar 完整伪代码与控件表、进图后 HUD 点击/快捷键分发」。[观察] → `[主 agent 复核]` 本轮 Angel 247 函数伪代码 + 11 控件表已补齐大半。
- `r15:37,40` — 「底栏快捷格无点击分发、仍留 `[DBG-CLK]`」；「控件 id 与注释 2004/2005 编号不完全一致（坐标已对齐，见 §3.5）」。[观察] → 前者销案，后者仍在（`UIStatusBar.cpp:1905`）。
- `r15:131,133,135,138-141,150-151,196` — 四钮「仍有 `[DBG-CLK]` 写 `_clk.log` 需实机确认」；「快捷格仅绘制 classic_qs_icons（只认 SKILL/ITEM）；无槽位 hit→send_key」；CreateCtrl 证据 `sub_8D01B2 size 0x392d`；P0「底栏快捷格不可点」「四按钮回调未闭环」。[观察] → **核心两条销案**，实机确认仍挂起。
- `r15:167` / `evidence/r15/rea_decompile_StatusBar_8D01B2.txt:1-4,81` — REA `Analysis failed / unreadable_output`，空 facets 是 unknown 而非 absence。[观察]
- `docs/features/feature-gap-inventory-2026-11/README.md:110,113,114,117,118,119,120` — B1 快捷格 ❌ P0 / B4 完整 v83 血条聊天条 🟡 / B5 hpFlash·mpFlash 🟡 预留 `IO/UITypes/UIStatusBar.cpp:2507` / B8 CHANGE_QUICKSLOT 持久化 🟡 / B9 TemporaryStatView ❌ 全缺 / B10 UIKeyConfig 映射表 🟡 / B11 UIBossHP 多 oid 🟡。[观察] → B1 销案；其余维持。
- `feature-gap-inventory-2026-11/README.md:587,593,594,597` — TOP20 #2 快捷格 ❌ 维持 P0、#8 TemporaryStatView ❌、#9 `channel_count<2` 单频道世界跳过 ❌ P0、#12 公会副本 HUD（s2c 287/288 无 handler）❌。[观察] → #2 销案；#9 行号已过时（登录面复核 `:697` 实为 `clear_selected_world()`）。
- `feature-gap-inventory-2026-11/README.md:11,36,155,423` — 「缺玩法深度/真实渲染/服务端闭环/控件表还原/实机验收五层」；旧 ❌ 大表大量已销案；C28/I7 `UIMiniMap BtSmall/BtBig/BtNpc` 🟡 显式预留。[观察]
- `docs/features/ui-v83-window-restore/notes/open-issues.md:56,62` — 「只有占位文本 + `send_key` 调试壳，5 颗钮一个都没建」（A2 UIGameMenu 已改为 5 真按钮）；场景变更显式预留。[观察]
- `open-issues.md:663-666` — `Map["miniMap"]["canvas"]` 缺失、`kMaxCanvasEdge=2048` 越界保护把 `has_map` 置 false 退回 MIN。[观察]
- `open-issues.md:684,686-689` — `BtSmall/BtBig/BtNpc` 包内确无；C3 UIOptionMenu 只能是空壳（本包 GameOpt/SysOpt 只有 `backgrnd` 158×284 / 299×365，无 StatusBar3.img、无 UIWindow2/4/6.img 根）。[观察]
- `open-issues.md:792-793,795-801,950` — C9 HUD EXP `[0.00%]` 数字位需位图字库度量；`key/7` 本包是红箭头 28×20 不是 Pdn、字母位图字库 Basic.img 全 71 根不存在 ⇒ OutlinedText；`KeyAction::Id` 被 UIKeyConfig/UIStatusBar/存档 Settings 引用，历史存档 action 会集体错位 ⇒ 要写迁移。[观察]
- `docs/features/multi-resolution/README.md:20,30,31,47,82,84,123` — 底栏空壳入口/两窗居中重锚本版不做；UIOptionMenu 退化空壳无分辨率选项；UIStatusBar 只枚举 5 种宽度；`## 7. 显式预留`；REA/IDA 新增取证与 orange-wz 通道恢复；modern HUD 兜底补 EXPBarRes 回退 1280 / QS 折叠展开非枚举宽度建钮 / 判空保护。[观察]
- `docs/features/login-layout-1024-fix/README.md:276,279,282` — 无 StatusBar3 时跳过 UIStatusBar/UIChatBar；超大小地图 canvas 5944×1311 已跳过；预留「完整 v83 血条/聊天条需另做 StatusBar.img 适配」。[观察]
- `docs/features/v83-hud-quest/README.md:55-60` — 预留：超大小地图降采样、EventList/socialChat（v83 无节点已安全跳过）、快捷栏拖拽改键（仍走 KeyConfig）、服务端任务脚本。[观察]
- `docs/features/change-quickslot-persist/README.md:28-29,33` / `evidence/live-accept-2026-11-06-qsi.md:38-39` — 「键设置→变更快捷键→改槽 ⏳ 待手点」「重登持久化 / DB quickslotkeymapped ⏳」；「槽内技能/道具绑定仍走 KEYMAP(135)」。[观察]
- `docs/features/social-loop-audit-2026-11/README.md:117` — `UIChatBar::change_target(int32_t)` 仍是全客户端共用的预留空实现。[观察]
- `docs/features/chat-system-overhaul/README.md:59,61,62` — F/B 键与 CHAT_FRIEND 按钮、P/G/A 键共 5 处 MultiChat 软键盘出口全部无本地回显（定向修复已完成）；「最近密语」数据源未取证不编造。[观察]
- `docs/features/minimap-mode-switch/README.md:20,100-101` — 城镇强制 MAX、构造只注册 BT_MAP、切态不写回配置；全包无 minimap 专属 Mob/Npc tip 节点。[观察]
- `docs/features/map-search-ui/README.md:25,27,53,71,74-76` — 搜索逻辑/传送封包/结果数据绑定/加载状态文案明确未实现（WorldMap 搜索栏，非 StatusBar，仅登记）。[观察]
- `docs/features/hotkey-grid-3x3/ui/hotkey-grid/v1/manifest.md:8` — 样板无实测节点导出（orange-wz 未接入）。[观察]
- `docs/features/beidou-v17-parity/notes/r00-baseline-parity.md:102,174` / `r14-native-feature-catalog.md:136` — 公会「无独立节点（状态栏入口）」/ `SlideMenu/GameMenu/ShortCut/ContextMenu 🟡 部分在 STATUSBAR`。[观察]
- `docs/features/beidou-v17-parity/notes/01-parity-matrix-and-plan.md:88,153` — HP/MP 警报 `0x1000` 待校验（旧账，登录面已确认出站 `UIOptionMenu.cpp:1216` 已发）。[观察]
- `docs/features/level-cap-999-2026-11/README.md:164-165` — classic 底栏 EXP 只画 `[xx.xx%]` 无当前值/最大值；数字字库只含 0-9 `[ ] / %`，无小数点（1×1 白点补）、无字母无中文 ⇒ 中文亿/万改用 OutlinedText。[观察]
- `docs/features/beidou-v17-parity/notes/r40-utildlg-branches-remaining-shells.md:12,16-18` / `r40-continuous-shell-coverage.md:5,7,9` — 剩余壳 `MobGage·SkillUp·MacroSkill·QuestIcon·MinigameTable·FloatNotice` / `QuestAlarm`，**两份 r40 均无 StatusBar 控件表或行为记录**。[观察]
- `docs/features/beidou-v17-parity/notes/r17-full-native-analysis-and-dependency-plan.md:43` — `StatusBar CreateCtrl sub_8D01B2 | C− | 坐标表齐；QS 点击/handler 行为未抽全`。[观察]

## 3. 待实机（⏳）

- `feature-gap-inventory-2026-11/README.md:111` B2 四钮回调实机确认（仍有 `[DBG-CLK]` 写 `_clk.log`）；`:115,116` B6 200+ 经验条溢出需截图、B7 SET_HPMPALERT 端到端回灌待 `use_server_auto_pot=true`；`:96,527-529,547,551,554,567,569,571,572` §15 真机挂起主源 `docs/features/pending-live-tests/README.md:12-45`（A11 登录遮罩四页叠图、#7 HPMPALERT、#13 公会 Boss HUD、社交五窗按 G、200+ 复验 StatusBar/StatsInfo/UIQuit、叠图确认、buff 文本位置）。[待实机]
- `docs/features/buff-timer/README.md:84,88` — 待人工确认 buff 文本位置/59s→1m0s 切换/`ui.buff_timer=false`；风险 duration 0/负恒显 1s。[待实机]
- `docs/features/softkey-text-mode/README.md:61` — 真机对拍键帽不漂出底板/不盖 Messenger（用户 2026-11-06 17:19）。[待实机]
- `docs/features/chat-system-overhaul/README.md:69` — 真机进图逐频道手测（下拉 8 项、组队自己那行、`/w` 不存在只出红字、喇叭项开道具窗留文本）。[待实机]
- `docs/features/level-cap-256-2026-11/README.md:42` — 200+ UI 展示复验 StatusBar/StatsInfo/UIQuit 溢出。[待实机]
- `r15-baseline-ui-gap-vs-screenshots.md:151,184-185,210` — P0 四钮点击是否进回调（否）/ D 进图 HUD `进入游戏页.png` / E 快捷键四窗 `打开了S-E-I-K页面.png`·`键盘设置.png` / W3 建议补当前端同场景截图。[待实机]
- `r21-remaining-backlog.md:11,13-15` — 登录遮罩图截图 / PersonalShop CREATE / Messenger 立绘（0.70 / foot -10）。[待实机]
- `docs/features/beidou-v17-parity/notes/evidence/r20/current_shots/README.md:3,8,10` — 当前端同分辨率截图未落盘（仅 README）。[待实机]
- `r201-login-overlay-playbook.md:26-27` — 当前端截图放 `current_shots/` 后与 correct_shots 半透明叠图，仅偏差项改 knob。[待实机]
- `docs/features/v83-hud-quest/README.md:62-68` — 验收清单：选角进图底部条+聊天可见、QS 槽显示绑定图标且对应键可释放、`_enter_game.log` 达 `fadeprocedure done`。[待实机]
- `open-issues.md:8,358-362` — A13 `_boot.py`「已进图」判据不可信，须换底栏血蓝条固定色块/小地图顶带。[待实机]
- `parity-gap-2026-11/README.md:51` — 端到端回灌待 `use_server_auto_pot`。[待实机]

## 4. 推测（推断/映射/须排除异线符号）

- `feature-gap-inventory-2026-11/README.md:578,580,582` — TOP20 排序=推断；§9.5 覆写把原 #1/#3/#4/#5/#6/#7/#10 的 ❌ 降级为「玩法深度/真机验收」；`:63,36` 证据优先级（协议矩阵 > 源码 .dispatch() > 现场重算 > 真机 > Wave 销案 > 2026-10-21 基线）。[推断]
- `docs/features/multi-resolution/README.md:30,31,37,53` — `PosGAMEOPT(170,24)/PosSYSOPT(330,24)` 无 oracle 依据；下拉展开态自绘；全库 `Resolution/soScreenResolution/Custom` 零命中 ⇒ IMG 无分辨率节点；UIStatusBar 补非枚举宽度 `y=H-71` 整组水平居中（标注偏离原生）。[推断]
- `r15:31,40,134,139` — 「主要是对照/实现未闭环，不是没把 exe 全反编译完」；keymap 空/旧 alternate 表现为按键无效；`is_in_range` 在非 800 宽下是否盖住按钮待核。[推断]
- `docs/features/v83-hud-quest/evidence/mapping.md:13-20,36,38` — v95→v83 节点映射推断（`mainBar/status*/backgrnd → base/backgrnd(+backgrnd2)` 高约 71、`mainBar/EXPBar → gauge/gray+gauge/tempExp`、`StatusBar3/chat → base/chat+chatTarget`、`KeyConfig → UIWindow/KeyConfig`、`MiniMap → UIWindow/MiniMap`）。[推断] ⚠️ 该文件名带 v95 字样，仅作映射参考，**不作 083 功能判定依据**。
- `feature-gap-inventory-2026-11/README.md:630,633` — HUD 细节 EA 段 `CUIStatusBar/CUIEnergyBar/CUIPartyHP/CUISoftKeyboard 0x7f9bad../0x91f934/0x832979 41/2/1/1`；使用约束「083 基准不变、禁止引 095 线；控件 id/坐标/包序先 Angel 定位再回 13337 交叉验证，冲突以 13337 + 实机 WZ/IMG 为准；Angel 字符串缓存仅 1281；端口会变每次先跑 server_health」。[推断]
- `feature-gap-inventory-2026-11/README.md:118` — B9「s9 无独立 TEMPORARY_STAT 封包，但 Player::buffs 已在状态包解析链 → 可本地驱动建窗」。[推断]
- `docs/features/level-cap-999-2026-11/README.md:28` — 状态栏 EXP 12519.52% 是 `use_level_up_protect` 测试态非溢出。[推断]
- `docs/features/minimap-mode-switch/README.md:46,151` — `minimap_5x.png` 顶带蓝 −/灰 + = `Basic.img/BtMin|BtMax`；判「本包有没有某控件」必须遍历全包 `*.img_runtime.json`。[推断]
- `open-issues.md:9,792-793` — C 需外部证据（IDA/缺素材/服务端）；EXP 数字位度量需 StatusBar 位图字库依据（W1 遗留）。[推断]
- `docs/features/hotkey-grid-3x3/notes/client-path.md:4` / `multi-resolution/README.md:37` / `v83-hud-quest/evidence/StatusBar.img.xml:4,8` — **尺寸口径冲突**：`base/quickSlot 151×80`（XML `:8`）vs `quickSlot 466×80`（multi-resolution:37）vs 8 槽 TL `28×25`（change-quickslot-persist:15）；`backgrnd 817×71`（multi-resolution:37）vs XML `:4` `800×71`。[推断] → `[主 agent 复核]` r02 已用 `UIStatusBar.cpp:1908-1921` nx_inspect 运行时实测解释：817×71 是 `base/backgrnd`、800×71 是设计网格、466×80 是 `quickSlot26`、151×80 是 `quickSlot08`，**三套口径对应不同节点，不是冲突**。
- **`docs/features/port-095/notes/opcode-v95.md:758,760` — `140 CUIStatusBar::SendGroupMessage (0x87f7f0)` / `142 CUIStatusBar::SendCoupleMessage (0x87b3e0)` 属 095 线，按门禁须排除，仅登记存在。** [推断]
- `r00-baseline-parity.md:174` / `r05-ui-layout.md:42` — 状态栏入口已有补 UI 与协议；`StatLabel 新增 LEVEL、EXP…经验显示为 exp(pct%)`（旧基线推断，须以 Angel 覆核）。[推断]

## 5. HUD 常量表（子代理原文，主 agent 补「Angel 一手」列）

### A. IDA 函数 EA
| 常量 | 值 | 来源 |
|---|---|---|
| `CUIStatusBar::OnCreate` = `sub_8D01B2` | `0x8d01b2` | `angel-hud-decompile.json:27`；`evidence/r15/ida_decompile_8D01B2.json:2`；`r16:56` |
| `sub_8D01B2` size | `0x392d` | `evidence/r15/rea_decompile_StatusBar_8D01B2.txt:38-40`；`r15:135` |
| `OnChildNotify` / `OnButtonClicked` | `0x8d419c` / `0x8d423d` | `angel-hud-decompile.json:32,42` |
| `ConvertWhisperToNormal`/`GetChatLog`/`SendClaim`/`SetChatType` | `0x8d4348`/`0x8d468d`/`0x8d4a22`/`0x8d4a6d` | `angel-hud-ea.json:1500-1560` |
| `ChatLogDraw`/`HitTest`/`GetShortCutIndexByPos`/`Update` | `0x8dbb93`/`0x8de7da`/`0x8de8d5`/`0x8de964` | `angel-hud-decompile.json:157,162,167,172` |
| `ToggleQuickSlot`/`MakeCtrlEdit`/`SetButtonBlink`/`SetShortcutTooltip` | `0x8df4b1`/`0x8dfb36`/`0x8dfe21`/`0x8e03f1` | `angel-hud-ea.json:1700-1743` |
| `GetWhisperTarget`（最小 EA）/`ReDrawQuickslot` | `0x4c6db2`/`0x8d82a7` | `angel-hud-ea.json:1500`；`angel-hud-decompile.json:207-210` |
| `CQuickslotKeyMappedMan::OnInit`/`CUIKeyConfig::OnButtonClicked` | `0x72b82a`/`0x8335d0` | `angel-hud-decompile.json:207,695` |
| `sub_A12522`/`sub_8D4D43`/`sub_8E0634` | `0xa12522`/`0x8d4d43`/`0x8e0634` | `angel-hud-decompile.json:42-45,32-35,172-175` |
| 字符串 `UI/StatusBar.img/BtNPT` / `base/chatTarget` | `0xb3bab4` / `0xaf72e0` | `evidence/r15/rea_search_StatusBar.txt:66,65`；`r15:164`；`r16:149` |
| `BtShop` 字面量检索 | `n:0 / matches:[]` 零命中 | `evidence/r15/rea_search_BtShop.txt:63-67` |
| IDB md5/sha256/crc32/filesize | `44c1030ecd8484e849c83892eb8fa025`/`1198fa57...4df`/`0xd458ef06`/`0x81c000` | `evidence/r15/rea_decompile_StatusBar_8D01B2.txt:43-50` |
| Angel/BeiDou 函数总数·imagebase | `55016/55655`、`0x400000` | `feature-gap-inventory-2026-11/README.md:612` |
| **HUD 面 EA 交叉验证** | **Angel 548 具名 EA → BeiDou 547 present（542 无名 / 5 具名），1 missing `0x6cec47`** | `evidence/angel-beidou-hud-ea-crosscheck.json`（主 agent 本轮） |

### B. CreateCtrl 控件坐标 / id（`evidence/r15/statusbar_createctrl_coords.txt:1-11` = Angel `OnCreate` 逐字）
| id | 坐标 | Angel `OnButtonClicked` 语义 |
|---|---|---|
| 1000 | (573,543) | `SendMigrateToShopRequest(2000,0)` |
| 1001 | (629,543) | `sub_A12522(*dword_BE7918)` |
| 1002 | (685,543) | `UI_Menu()` |
| 1007 | (741,543) | `sub_A06CBC(*dword_BE7918)` |
| 2000 | (618,515) | `UI_Toggle(1,-1)+SetButtonBlink(0,0)` |
| 2001 | (648,515) | `UI_Toggle(0,-1)+SetButtonBlink(1,0)` |
| 2002 | (678,515) | `UI_Toggle(2,-1)+SetButtonBlink(2,0)` |
| 2003 | (708,515) | `UI_Toggle(3,-1)+SetButtonBlink(3,0)` |
| 1005 | (738,515) | `UI_Toggle(5,-1)`（KeySet） |
| 1006 | (768,515) | `ToggleQuickSlot()`（QuickSlotD） |
| 1009 | (573,515) | `SendClaim()`（BtClaim） |
| 1003/1004 | — | `SetChatType(1)` / `SetChatType(3)`（无坐标，聊天类型切换） |
| 1008 | — | **无分支** |
| 旧注释编号 | `2004–2006` vs 实测 `1005/1006/1009` | `r15:40,135`；`UIStatusBar.cpp:1902-1907` |
| `CCtrlEdit` 聊天编辑框 | `id=1011` `(85,520)` `440×12` | `angel-hud-decompile.json:192-195` |

### C. 几何 / 命中 / 时序
| 常量 | 值 | 来源 |
|---|---|---|
| QS 原点 / 槽尺寸 / 坐标表 | `(647,427)` / `32×32` / `dword_BE2DB4..BE2DF4`（8 槽×2 int） | `angel-hud-decompile.json:167-170` |
| HitTest 基线 / x 分段 / y 界 | `507` / `580`,`647` / `427`；`-16*(*(v6+60))` 折叠偏移；`this+836/749/831/832` | `angel-hud-decompile.json:162-165` |
| QS 展开态标志 | `*(this+836)` ↔ `CConfig+22` | `angel-hud-decompile.json:187-190` |
| Update 定时 | `%32`、`90000 ms`、`0x3E8`(1000 ms)、`while(v2<4)` | `angel-hud-decompile.json:172-175` |
| QUICKSLOT keymap 长度 | `0x20u`(32B) `DecodeBuffer`/`memcpy` | `angel-hud-decompile.json:207-210` |
| `CUIKeyConfig` 清零 | `memset(...,0x1BDu)`(445) | `angel-hud-decompile.json:695` |
| 当前端 QS 常量 | `QS_SLOT=28/PITCH_X=35/PITCH_Y=34/FIRST=(9,9)` | `IO/UITypes/UIStatusBar.cpp:106-118` |
| 当前端 gauge 段 | `SEG_HP=0/MP=109/EXP=222`；`W 108/107/118`；`GAUGE_X=218/Y=38` | `UIStatusBar.cpp:90-99` |
| classic EXP 段 / 底栏 EXP 数字区 | `x1..115 / y53..66` 7px 字形；`~86px`；999 级需求 `2.1e11`(12 位) | `level-cap-999-2026-11/README.md:174,153` |
| 底栏高 / 非枚举宽度重锚 | `71` / `y=H-71` | `v83-hud-quest/evidence/mapping.md:13`；`multi-resolution/README.md:53` |
| `kMaxCanvasEdge` / `miniMap/canvas` | `2048` / `~5944×1311` | `open-issues.md:663-666` |
| `PosGAMEOPT`/`PosSYSOPT` | `(170,24)`/`(330,24)` | `multi-resolution/README.md:30,37` |
| 建议截图分辨率 | `1280×720` | `r201-login-overlay-playbook.md:26` |

### D. StatusBar.img IMG 节点尺寸（`docs/features/v83-hud-quest/evidence/StatusBar.img.xml:1-519`）
`base/backgrnd 800×71`(:4) / `backgrnd2 570×71`(:6) / `quickSlot 151×80`(:8) / `chatTarget 81×20`(:10) / `base/box 42×19`(:12) / `chat 566×5`(:20) / `gauge/graduation & bar 340×31`(:24,27) / `gauge/gray & tempExp 1×16`(:30,32) / `hpFlash[0..1] & mpFlash[0..1] 109×18 delay=130`(:34-58) / `BtShop/BtMenu/BtNPT/BtShort 54×34 四态`(:61-147) / `BtWhisper 12×19`(:149-190) / `BtClaim 20×19` / `EquipKey/InvenKey/StatKey/SkillKey/KeySet/QuickSlot/QuickSlotD 28×20 ani delay=500`(:193-415) / `key/0..7 28×11,18×11,19×11,22×13,24×11,19×11,21×11,22×11`(:417-442) / `number Lbracket,Rbracket 2×9; slash,percent 7×7; 0..9 5×7`(:443-486) / `FontMemo/0..9 6×7（1 为 3×7）`(:487-518)。对照口径 `backgrnd 817×71 / quickSlot 466×80 / gauge/bar 340×31`（`multi-resolution/README.md:37`，运行时实测）。`KeyConfig/quickslotConfig backgrnd 266×238`、8 槽 TL `(67/102/137/172, 114/148)` 约 28×25（`change-quickslot-persist/README.md:15`）。`Basic.img/BtMin|BtMax 12×12 四态`（`open-issues.md:668-684`）、`/MobGage/Mob 25×25`（`minimap-mode-switch/README.md:100-101`）、`GameOpt/backgrnd 158×284`/`SysOpt/backgrnd 299×365`（`open-issues.md:686-689`）。键帽 `KEY_CHARS/KEY_X[37]/KEY_Y[37]` 37 字符 `y=37/60/83/106`（`softkey-text-mode/evidence/keys/keycoords.txt:1-7`）。

### E. 协议 / 枚举
| 常量 | 值 | 来源 |
|---|---|---|
| `CHANGE_QUICKSLOT` / `QUICKSLOT_INIT` / `KEYMAP` | `183` / `0x9F` / `135` | `change-quickslot-persist/README.md:7,33` |
| `SET_HPMPALERT` | `0x1000`(4096) | `01-parity-matrix-and-plan.md:88,153`；`parity-gap-2026-11/README.md:51` |
| `QUEST_ACTION`/`UPDATE_QUEST_INFO` | `0x6B`/`0xD3` | `v83-hud-quest/evidence/mapping.md:4-7` |
| 公会副本 HUD s2c | `287/288`（无 handler） | `feature-gap-inventory-2026-11/README.md:554,597` |
| `LEVELCAP` 比较锚点 | `UIQuit.cpp:249`、`UIStatusBar.cpp:1839`、`UIStatsInfo.cpp:861` | `level-cap-256-2026-11/README.md:22` |
| `IO/UIStateGame.cpp` 构造期 HUD 窗 / 触发窗 | `70-108`（10 窗）/ `226-720`（30+ 窗） | `feature-gap-inventory-2026-11/README.md:27` |
| 源码体量 | `UIMiscShells.cpp 7852`、`UIChatBar.cpp 2672`、`UIStatusBar.cpp 2541` | `feature-gap-inventory-2026-11/README.md:29` |
| `IO/UIElement.h:214` buttons | `std::map<uint16_t, std::unique_ptr<Button>>` | `open-issues.md:205` |
| `hpFlash/mpFlash` 源码锚点 | `IO/UITypes/UIStatusBar.cpp:2507` | `feature-gap-inventory-2026-11/README.md:114` |
| `channel_count < 2` 硬缺陷 | `IO/UITypes/UIWorldSelect.cpp:697-698` | `r15:104,110-113`；`feature-gap:594`（**行号已过时**，登录面复核 `:697` 实为 `clear_selected_world()`） |

## 6. Angel 应优先复核的 15 点（子代理原文）

1. 取全 `OnCreate`（126188 chars）抽全 CreateCtrl 表 / gauge 控件 id / HP·MP·EXP 绑定，闭环 `r16:150`、`r15:24`。→ `[主 agent 复核]` 已补全（`evidence/angel-hud-decompile-full.json` + `angel-statusbar-oncreate-full.json`）。
2. 取全 `ChatLogDraw`（59158 chars）+ `ConvertWhisperToNormal`/`GetChatLog`/`SetChatType`/`SendGroupMessage` 串起聊天输入绑定、`chatTarget @0xaf72e0`，闭环 `r16:149`。→ 已补全。
3. CreateCtrl 坐标表逐行对账，专核 `r15:40,135` 的 `2004–2006` vs `1005/1006/1009`。→ 已对账并定位当前端注释仍误标。
4. 四按钮回调落地：`OnButtonClicked` case → `init_classic` + `button_pressed`，确认 `[DBG-CLK]` 可删。→ 已销案（`UIStatusBar.cpp:747`）。
5. QS 8 格 hit→keymap 分发：`GetShortCutIndexByPos` + `HitTest` + `OnMouseButton`/`OnKey`。→ 已销案（`UIStatusBar.cpp:2215-2231`）。
6. 导出 QS 坐标表解尺寸冲突（151×80 / 466×80 / 28×25 三套口径）。→ r02 已用 nx_inspect 解释（不同节点）。
7. QS 展开/折叠态联动（`this+836` ↔ `CConfig+22`、`-16*offset`）。→ 待裁决。
8. 聊天编辑框与通知链闭合（1011 / 1010 / 1012 / `UIChatBar::change_target` 空实现）。→ 部分（`change_target` 仍空）。
9. 键帽名表与 IMG 差异（`SetShortcutTooltip` 的 `aPageup/aPagedown/aHome` vs `key/0..7`、`key/7` 是红箭头、字母字库不存在 ⇒ OutlinedText）。→ 待裁决。
10. Quickslot 持久化闭环（`OnInit 0x20u` / `DefaultQuickslotKeyMap` / `ReDrawQuickslot` ↔ 183 / 0x9F / DB `quickslotkeymapped` / KEYMAP(135)）。→ ⏳ 待手点。
11. 键位设置窗闭环（`CUIKeyConfig::OnButtonClicked 0x8335d0` ↔ basic_keys ↔ v83→本枚举映射表 ↔ 存档 action 迁移）。→ 待裁决。
12. HP/MP/EXP 条与闪铃（`SetStatusValue`/`Draw`/gauge 控件 id ↔ `hpFlash/mpFlash 109×18 delay130`）。→ 🟡 预留。
13. 200+ 经验条溢出（~86px、中文亿/万 OutlinedText、`[100.00%]` 封顶）。→ ⏳ 需截图。
14. minimap 侧（`kMaxCanvasEdge=2048`、`BtSmall/Big/Npc` 无、`BtMin/Max 12×12`、城镇强制 MAX、tip 节点确无）→ 用 Angel `UIMiniMap` EA 复核哪些属预留/可还原。→ 待后续面。
15. 证据口径与门禁（每次先 `server_health` 读 `idb_path`； Angel 定位→13337 交叉验证→冲突以 13337+实机 WZ/IMG 为准；剔除 095 线 `opcode-v95.md:758,760`；`r15:35` 两端 IMG SHA256 全同把差异收窄到行为层）。→ 本轮已遵守。

## 7. 五、仓库内 CUIStatusBar 取证产物 / 正确端截图（子代理明确结论）

- **IDA 反编译产物：有**。`evidence/angel-hud-ea.json`（548 funcs / 47 classes，CUIStatusBar 41 方法 `:1498-1743`）、`evidence/angel-hud-decompile.json`（41 个 `CUIStatusBar::*` 伪代码，约 10 个 `Output truncated`）、`evidence/r15/ida_decompile_8D01B2.json:2-353`（BeiDou 单行伪代码 + ~60 refs）、`evidence/r15/rea_decompile_StatusBar_8D01B2.txt`（REA `Analysis failed / unreadable_output` 失败信封，有效内容仅 addr/size/metadata）、`docs/features/ida-v83-angel-instance/evidence/angel-gap-feature-ea.json:661-865`（CUIStatusBar 41 修饰名）、`ida-class-scan.json:52`、`angel-idb-msvc-class-index.json:853`。`port-095/notes/opcode-v95.md:758,760` 属 095 线须排除。
- **控件表：部分有**。`evidence/r15/statusbar_createctrl_coords.txt:1-11`（11 行 id/x/y，无语义/尺寸/回调）、`v83-hud-quest/evidence/StatusBar.img.xml:1-519`（完整 IMG 节点尺寸表）、`v83-hud-quest/evidence/mapping.md:13-20,36,38`、`change-quickslot-persist/README.md:15`（8 槽 TL）、`softkey-text-mode/evidence/keys/keycoords.txt:1-7`。**未找到**带按钮回调/尺寸/命中矩形的完整 IDA 控件表 → 本轮 r01 §2 已用 Angel 逐字补全语义列。
- **正确端截图：有** `docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/` 7 张：`进入游戏页.png`（**用户任务写的「进入了游戏页.png」不是实际文件名**；`r15:57` 覆盖新手村/操作说明对话/底栏 Lv·HP·MP·EXP/商城·帮助·目录·界面/右侧快捷键格/聊天·小地图·任务告知；`r201:14` 标注 `UIStatusBar（CreateCtrl 已锁）`；`r15:184` 为 D 进图 HUD 验收帧）、`打开了S-E-I-K页面.png`（四窗+底栏+快捷格）、`键盘设置.png` 629x369、其余 4 张登录链。`current_shots/` 只有 README，**当前端同场景截图未找到**。`open-issues.md:980` 索引 `../evidence/refcrops/`（bottombar/keycaps/chatbar/minimap/questnotice）glob **未找到** PNG 实物。

---

（子代理全程只读，未创建/修改/删除任何文件；另通读 `r40-utildlg-branches-remaining-shells.md`、`r40-continuous-shell-coverage.md`、`r21-remaining-backlog.md`、`world-map-players/README.md`、`enter-game-blackscreen-fix/README.md:18,48-49`、`parity-gap-2026-11/README.md:50-51`、`ida-v83-angel-instance/README.md:152,202`、`login-flow-angel-calibration-2026-11/README.md:382`。主 agent 复核块为本轮新增，依据逐字源码读取。）
