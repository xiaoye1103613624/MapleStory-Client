# 当前客户端源码缺失功能清单 · 逐类逐项（2026-11-08）

> 触发：Q1 判定 `Angel.exe`(port 13339) 是带符号的 083/v83 IDB 后，用户问「当前客户端源码实现又缺失哪一些具体功能？每一类、每一个具体功能列举到文档中」。
> 定位：**相对 v83 原版 / s9 服务端的功能面差距台账**。本文只做只读盘点与裁决归并，**不含实现、不含 Plan、不新增代码**。
> 基准裁决：`docs/features/social-loop-audit-2026-11/README.md:7-10` —— 功能与资源一律参照 **GMS 083 原版**（`BeiDou-ClientV17.7\BeiDou.exe` + 其 Data；协议侧 s9 即 v83 派生）；095 线（浩浩岛095 / PLUS2 / 怀旧岛192 / `Maplestory95.i64`）不作本线参照与入库源。

---

## 0. 一句话结论

**当前客户端几乎不缺「窗口文件」，也不缺「出站封包」；缺的是「玩法深度 / 真实渲染 / 服务端闭环 / 控件表还原 / 实机验收」这五层 + 少量特定协议项。** 具体表现是：`IO/UIElement.h` 的 151 个 `UIElement::Type` 枚举里 149 个已有类绑定（唯一无绑定的是哨兵 `NONE`），`IO/UITypes/` + `IO/Components/` 共 247 个文件、163 个 `class UI... : public` 声明、654 处方法体；`Net/OutPacket.h:37-318` 的 **148 个 c2s opcode 全部有构造点（148/148 `client_out_sent`）**、全仓 917 处 `.dispatch()`——旧账「出站封包整片不发」已被 §9.5 推翻。但相当一部分窗口仍是「底板 + 关闭键 + 显式预留注释」的壳窗，`update()` 空体、`button_pressed` 只打日志；真「完全无」的协议项收敛为场地伤害族 / `PARTY_SEARCH_REGISTER` / `MTS_OPERATION` 等 ~16 项（§9.5.3）。

按用户「每一类、每一个具体功能」的要求，下文分 **A–K 十一类 + 明确不做总表 + 真机挂起表 + TOP20 优先级**，每条给出状态、缺口形态、证据相对路径:行号。全文共登记 **具体功能项 ~200 条**（含 §9.5 协议层复核新增 / 覆写项）；状态标记分 ❌ 真缺 / 🟡 部分或待验收 / ⏳ 真机挂起 / ⊘ 明确不做 / ✅ 已闭环（列此仅供去重排除）。⚠️ 协议层状态一律以 §9.5 三态为准，§9.1–§9.4 旧账已作废。

> ⚠️ **2026-11-08 协议层复核（§9.5）**：第三路子代理以 `docs/features/s9-protocol-align/protocol/opcode-matrix.json`（`generated_at=2026-11-08T00:56:27`、s9 head `3d5ccc6f`）重算，`client_out_sent` 从旧账 103 变为 **148**，大量「未接线」判定被源码 `.dispatch()` 推翻。§9 / §7 / §16 中基于旧 `gap-inventory.md` 的读数一律以 **§9.5** 为准。

---

## 1. 盘点基线（先定「不缺什么」，避免误报）

| 项 | 实测 | 证据 | 性质 |
| --- | --- | --- | --- |
| `UIElement::Type` 枚举 | **151 个值**（含首 `NONE`、尾 `NUM_TYPES`） | `IO/UIElement.h:41-204` | 观察 |
| TYPE 绑定 | **152 处 `static constexpr Type TYPE`**，覆盖 **149 个不同枚举值**；无绑定仅 `NONE` 与 `NUM_TYPES`（尾标记） | 全仓 `IO/UITypes/*.h` 逐行；subagent#1 (`33d01e9b`) | 观察 |
| 工厂 | 模板 `template <class T,...> Optional<T> emplace(Args&&...)`，非字符串键注册表；全仓无 `UIFactory`/`create_ui` | `IO/UI.h:79-93` / `:95-102`；`IO/UIStateGame.cpp:802` `pre_add(T::TYPE, T::TOGGLED, T::FOCUSED)` | 观察 |
| 状态机 | `UI::change_state` → LOGIN `UIStateLogin` / GAME `UIStateGame` / CASHSHOP `UIStateCashShop` | `IO/UI.cpp:87-108` | 观察 |
| 自动实例化 | `IO/UIStateGame.cpp:70-108` 构造期 10 个 HUD 窗；`:226-720` 按键/handler 触发 30+ 窗 | `IO/UIStateGame.cpp` | 观察 |
| 文件总数 | `IO/UITypes/` .h=89 / .cpp=85；`IO/Components/` 23/23；`IO/` 根 17/10 → **247** | subagent#1 | 观察 |
| 最大文件 | `UIMiscShells.cpp` 7852 行、`UIChatBar.cpp` 2672、`UIStatusBar.cpp` 2541、`UICommonCreation.cpp` 2382、`UINpcTalk.cpp` 1916 | 同上 | 观察 |
| 壳窗集中营 | `UIMiscShells.h`（1037 行）约 70 个 `UIXxx : public UIDragElement<PosNOTICE>` 壳类（UIReport/UICoupon/UIIncubator/UIAskPet/UIRanking/UIPersonalShop/UITradingRoom/UIItemSearch/UISkillEx/UIQuestAlarm/UIUtilDlgEx/UIUtilDlgExAvatar/UIUtilDlgExPet/UIUtilDlgExMultiPet/UIExceptionItemSearch/UIMobGage/UISkillUp/UIMacroSkill/UIQuestIcon/UIMinigameTable/UIFloatNotice/UIWhisperTargetDlg …） | `IO/UITypes/UIMiscShells.h:12-970` | 观察 |
| 包 / handler 文件 | 几乎全齐：`WeddingPackets.h:63-207` 11 包类、`PlayerInteractionPackets.h:94-135` Minigame*、`FamilyPackets.h` 7、`MessengerPackets.h` 5、`CashShopPackets.h` 10、`MonsterBookPackets.h:27`；`Net/PacketSwitch.cpp:417-782` 注册 **305 条 `emplace<OPCODE, Handler>()`**（旧账「约 180」是 subagent#1 早期口径，以 §9.5 实测为准）；`Net/Handlers/WaveAShellHandlers.cpp` 4802 行壳窗入站专批（168 个 `XHandler::handle(InPacket&)`，仅 4 空参） | 源码 + subagent#1 + §9.5 复核 | 观察 |
| 短/空壳 .cpp（<28 有效行） | **0** | 全仓扫描 | 观察 |

**为什么 r16 里的 ❌ 现在都「有文件」**：`docs/features/beidou-v17-parity/notes/r810-commit-split-ledger.md` §3.1 记 **T14 一次提交补齐 20 个缺失系统窗外壳 + handler + 出站包（54 path）**；`docs/features/beidou-v17-parity/notes/r40-continuous-shell-coverage.md:5,7` 又铺 PersonalShop/MemberShop/TradingRoom/EntrustedShop/CashTradingRoom/ItemSearch/AranSkillGuide/Transform/MonsterKilling/MapleEvent/SkillEx/Book/QuestAlarm/MateMessage/BetaEdition 15 个壳 + Minigame 接 `PLAYER_INTERACTION ROOM/READY/START/EXIT`。`docs/features/parity-gap-2026-11/README.md:107` 明确 `MonsterBook/Maker/Alliance/Storage/Trade/Merchant/Guild/Party/Buddy/Messenger/Delivery/StoreBank/Incubator…` 均已有实体窗（r12/r802/r804 闭环）。

**结论（推断）**：`docs/features/beidou-v17-parity/notes/r16-rea-coverage-gap.md`、`r00-baseline-parity.md:95-143`、`r14-native-feature-catalog.md:117-158`、`01-parity-matrix-and-plan.md:59-122` 的 ❌ 大表是 **2026-10-21 旧账**，大量已销案；本文逐条引用前先查销案，且一律区分「**窗口类/壳存在**」与「**行为闭环**」两层。

---

## 2. 口径与状态定义（先读，否则下文会自相矛盾）

### 2.1 两套口径必须分开

| 口径 | 范围 | 权威源 | 处置 |
| --- | --- | --- | --- |
| **A · v83 原生 parity 线**（本线基准） | 原生 `UIWindow.img` 112 顶层节点 + s9 协议 + BeiDou.exe 行为 | `docs/features/parity-gap-2026-11/README.md`、`docs/features/social-loop-audit-2026-11/README.md` | §3–§12 正文 |
| **B · ijl15 扩展矩阵线** | `ijl15.dll` 插件 33 项 + 萧曳自定义（客户端/协议联动/双端） | `docs/features/beidou-v17-parity/notes/01-parity-matrix-and-plan.md:55-120` | §13 单列，标「用户可选立项」 |

> 口径张力（推断）：`docs/features/beidou-v17-parity/notes/r14-native-feature-catalog.md:247,274` 判「ijl15 与 `E:\MXD\自定义功能实现` 全部扩展 = 本线明确不做」且明令不要用 01 矩阵当本线范围；但 `docs/features/parity-gap-2026-11/README.md:113` 又把同一份 01 矩阵纳入 §3 并现场 grep 复核。`docs/features/beidou-v17-parity/notes/01-parity-matrix-and-plan.md:22` 记 33 项扩展功能**全在 `ijl15.dll` 插件而非 BeiDou.exe**，对齐的是插件行为 + `Data/UI` 资源。故 **§13 是「可选扩展」，不是 v83 原版缺口**，立项与否由用户决定。

### 2.2 状态标记

| 标记 | 含义 |
| --- | --- |
| ❌ | 真缺（全缺 / 核心逻辑未接 / 全仓 grep 0 命中） |
| 🟡 | 壳已铺、或部分接线、或「有 handler ≠ 客户端会发」、或深度未达 C 层证据 |
| ⏳ | 代码已接、需用户实机 / 截图 / 服务端配置才能判定 |
| ⊘ | **明确不做**（既有裁决 / 证据不足显式预留 / 需双端立项 / 高版本属性） |
| ✅ | 已闭环，列在此仅用于**去重排除**，防止把旧账当缺口 |

### 2.3 证据优先级

`协议层 2026-11-08 权威矩阵`（`docs/features/s9-protocol-align/protocol/opcode-matrix.json`，`generated_at=2026-11-08T00:56:27`，s9 head `3d5ccc6f`，`tools/gen_opcode_matrix.py` 可复算）　>　`源码逐字 .dispatch()/emplace 行`　>　`2026-11 现场重算`（`tools/gap_scan_2026_11.py` / `tools/gap_c2s_refined.py` / `tools/_gap_scan_out.json`）　>　`真机记录`　>　`2026-10-22/23 Wave 销案笔记`　>　`2026-10-21 基线（r00/r14/r15/r16/01）`　>　`gap-inventory.md`（旧 head `44d0d5d9ca`，`client_out_sent=103`，**协议层已过时，仅作历史三分类参考**）。

### 2.4 六处文档内部冲突，按「较新现场 / 真机行为」取后者

| # | 冲突 | 取值 | 依据 |
| --- | --- | --- | --- |
| 1 | 婚礼：`parity-gap-2026-11/README.md:83` 判 ⊘不做；`:105` + `wedding-flow/README.md:7-8` 判 ✅ 双端闭环 | **✅ 已闭环**（仅合影仍预留，s9 需外部 WvsMapGen） | `docs/features/social-loop-audit-2026-11/README.md:19` |
| 2 | 每日签到：`:123` 判「无窗类」；`:210-217` 判 `UIDailyGift` 615 行窗已在、双端协议零命中 | **壳在 · 协议缺 · 需双端立项** | `docs/features/parity-gap-2026-11/README.md:206-228` |
| 3 | 新角色骰子 #15：`:131` 判 `set_active(false)` 常驻禁用；`:198-199` 判 `randomize_look()` 已通 | **✅ 假缺口·已通** | `UICommonCreation.cpp:1260`→`:1292` |
| 4 | `gap-inventory.md:119` 汇总把 `0x89/0x8A` 列「不做」，同文件 `:81-82` 已改判「2026-11-06 已接」 | **✅ 已接（wedding-flow）** | `docs/features/social-loop-audit-2026-11/README.md:54` |
| 5 | `WORLD_MAP_PLAYERS(376)`：`parity-gap-2026-11/README.md:20-26,102` 判 s2c 差集 / 回包不处理；`pending-live-tests/README.md:14,88` 已有 2026-11-05 真机 `SEND 277 → RECV 376` | **✅ 真机闭环** | `docs/features/pending-live-tests/README.md:14` |
| 6 | **协议层 c2s 差集读数**：`gap-inventory.md:3,5` 基于旧 head `44d0d5d9ca` 判 `client_out_sent=103 / gap=70`；`opcode-matrix.json:26-38`（2026-11-08）实测 `client_out_constants=148 / client_out_sent=148 / client_in_registrations=305 / s9_registered_handlers=174 / s9_send_emitted=252` | **以 148 为准**：70 差集旧账作废，宠物/好友/道具/扭蛋/AP 分配等大量「未接线」已被源码 `.dispatch()` 推翻 → 见 §9.5 | `docs/features/s9-protocol-align/protocol/opcode-matrix.json:26-41`；`IO/UITypes/UIBuffList.cpp:246`；`IO/UITypes/UIStatsInfo.cpp:752` |

### 2.5 一处 opcode 命名撞值（立项前须核）

`0x3713` 三处命名不同：`docs/features/s9-protocol-align/notes/gap-inventory.md:113` = `CUSTOM_PACKET`（⊘明确不做）；`docs/features/parity-gap-2026-11/README.md:146` 与 `01-parity-matrix-and-plan.md:150` = 团队 Buff。立项前核 `docs/features/s9-protocol-align/protocol/opcode-matrix.json`，勿把「⊘不做」和「❌未做」写成同一个 opcode。

---

## 3. 类 A · 登录 / 世界 / 频道 / 选角 / 建角

| # | 具体功能 | 状态 | 缺口形态 | 证据 |
| --- | --- | --- | --- | --- |
| A1 | **`draw_world()` 单频道世界整块被跳过**（`if (world.channel_count < 2) continue;`） | ❌ **P0 代码缺陷** | 单频道世界不渲染，表现为「世界/频道缺失」；建议改 `< 1`，联调 `channelload_size` | `IO/UITypes/UIWorldSelect.cpp:697-698`；`docs/features/beidou-v17-parity/notes/r15-baseline-ui-gap-vs-screenshots.md:104,110-115,149` |
| A2 | **60 频道翻页**（现固定 `CHANNEL_BUTTON_COUNT=30`） | ❌ | 未实现参考端 60 频道翻页 | `docs/features/login-flow-ui-ida-align/README.md:173`；`r15:104` |
| A3 | **`View All` / `View Choice` 两钮 stub** | ❌ | 点击无行为；参考端 exe 搜不到字面量、语义无证据 | `docs/features/login-flow-ui-ida-align/README.md:173`；`IO/UITypes/UIWorldSelect.cpp:317` |
| A4 | **`UIViewAllChar` 的 `BtEnter`「从查看全部角色直接进所选角色」** | ❌ | 仅关闭窗口未接进角；依赖 s9 `showAllCharacter` 实机点击 | `docs/features/login-flow-ui-ida-align/notes/r10-login-flow-missing-features.md:165-168` |
| A5 | **opcode 4/5/6/7/8 无 handler**（`GENDER_DONE` / `CONFIRM_EULA_RESULT` / `CHECK_PINCODE` / `UPDATE_PINCODE` / `VIEW_ALL_CHAR` 只在名称表出现） | 🟡 | s9 默认 `enable_pic/enable_pin=false`，走 LOGIN_RESULT 闭环，暂不影响主链路；6/7/8 已实现 | `docs/features/login-flow-ui-ida-align/README.md:175`；`r10:9-62` |
| A6 | **rank / jobrank / burning / timestamp 不呈现** | ⊘ 显式预留 | 参考端 `CUICharDetail::Draw` 仅绘职业/等级/人气/四维；s9 协议无 burning/timestamp（v95+ 概念）→ 按「未证实不臆造 UI」 | `docs/features/login-flow-ui-ida-align/notes/r10-login-flow-missing-features.md:101-111`；`r15:122`（观感诉求，按证据优先取 r10） |
| A7 | **选角属性板 `CS_DETAIL_*` 硬编码 / 排行区文案未绘** | 🟡 | 无 NX `detailPos` 时走常量；需与截图像素对齐 | `docs/features/beidou-v17-parity/notes/r15-baseline-ui-gap-vs-screenshots.md:121-125,152` |
| A8 | **选角/世界请求门闩无超时** | 🟡 | 依赖服务器回包或 UI 销毁复位；断线场景建议 `Session` 重连统一清理（`UIStateLogin::update()` 已补取消后门闩看护） | `docs/features/login-flow-ui-ida-align/README.md:174` |
| A9 | **`CUICharDetailVAC` 归属未明**（`SetOption(148,-710)` / `CreateFadeWnd(170,210,0,z=11)` 读 `CLogin+316` 另一角色数组） | 🟡 待查 | 已排除「第 2 行槽位」猜想 | `docs/features/login-flow-ui-ida-align/README.md:168` |
| A10 | **登录/选频背景 step0 相机 X、step1 卷轴背景、Loading bar/取消偏移** | ⏳ 叠图 | 参考端 `CLogin::Init` 用 `(28,GetStepHeight)`，本端 `(137,-20)`；本机渲染黑屏（基线 exe 同黑） | `docs/features/login-flow-ui-ida-align/README.md:57,157,171-172` |
| A11 | **登录遮罩四页截图叠图**（登录/选世界/选角/状态栏） | ⏳ 需用户 | 关闭自动登录逐页按 SCROLL_LOCK，与 `correct_shots/` 半透明叠图；W3 需补「当前端」同场景截图做像素 diff | `docs/features/pending-live-tests/README.md:22,138-152`；`r21-remaining-backlog.md:11-15`；`r15:83,88,97,204-210` |
| A12 | **经典建角 RaceSelect / CustomizeChar / createLimit / MakeCharInfo 资源** | ⊘ 不移植 + ✅ 双路径绕过 | 北斗 UI.nx 无高版本节点导致越界闪退；改用经典三职业双路径，step4 第四轮已实现 | `docs/features/create-char-classic-dual-path/README.md:12,14,75,206-213` |
| A13 | **建角三职业页叠图 / 名字基线 / step4 `(323,150)` 低置信度几何值 / 端到端建角·PIC·查看全部角色** | ⏳ 需用户 | 黑屏环境既有问题不修 | `docs/features/create-char-classic-dual-path/README.md:125,156,202-204,267`；`r10:176` |
| A14 | **角色槽位扩展** | ❌ | 未见选角槽位扩展改动 | `docs/features/parity-gap-2026-11/README.md:134` |
| A15 | **快速登录** | ✅ 已闭环 | `get_auto_login` + `UILogin.cpp:335-392`，旧矩阵 ❌ 已过期 | `docs/features/parity-gap-2026-11/README.md:135` |
| A16 | **`LOGIN_EMAIL = 0x03`** 服务端未实现 | 🟡 客户端独有 | 北斗 `RecvOpcode` 无 0x03，客户端 4 个 `CLIENT_ONLY` 之一，不计入差集 | `docs/features/opcode-align-beidou-gms083/README.md:66`；`gap-inventory.md:5,25` |
| A17 | **`PICK_ALL_CHAR(0x0E/14)` 派发** | ⏳ | 已修正误发 19→14/32，需实机点选 BtEnter | `docs/features/parity-gap-2026-11/README.md:55`；`pending-live-tests/README.md:23` |

---

## 4. 类 B · 状态栏 / 快捷栏 / HUD

| # | 具体功能 | 状态 | 缺口形态 | 证据 |
| --- | --- | --- | --- | --- |
| B1 | **右侧快捷 8 格点击**（`classic_qs_icons` 仅绘制） | ❌ **P0 硬缺口** | 无槽位 `hit → send_key`，且只认 SKILL/ITEM；建议复用 `keyboard.get_maple_mapping` 走 `UIStateGame` 同套 MENU/SKILL/ITEM 分发 | `docs/features/beidou-v17-parity/notes/r15-baseline-ui-gap-vs-screenshots.md:129-135,137-141,150`；`r16-rea-coverage-gap.md:148,152`（`sub_8D01B2` 仅 CreateCtrl 坐标近 C，交互未达 C） |
| B2 | **底栏 `BtShop/BtNPT/BtMenu/BtShort` 回调实机确认** | ⏳ | 钮已建并接线，仍有 `[DBG-CLK]` 写 `_clk.log`，需实机确认进回调 | `r15:129-135,151` |
| B3 | **默认键位与正确端键盘图对齐**（S/E/I/K） | 🟡 | 存档 keymap 为空/旧 alternate 时表现为「按键无效」，应强制 classic `basic_keys` | `r15:134,141,154` |
| B4 | **完整 v83 血条/聊天条 `StatusBar.img` 适配** | 🟡 显式预留 | 超大 minimap 降采样、精细 LRU 配额同预留 | `docs/features/login-layout-1024-fix/README.md:282`；`docs/features/perf-budget/README.md:37` |
| B5 | **HP/MP 闪铃 `hpFlash`/`mpFlash`** | 🟡 | 预留 | `IO/UITypes/UIStatusBar.cpp:2507` |
| B6 | **200+ 经验条显示与溢出**（999 级经验 2.1e11 / 12 位，底栏 EXP 段仅 ~86px） | ⏳ 需截图 | StatusBar / StatsInfo / UIQuit 混合方案已拍板，待真机 | `docs/features/level-cap-256-2026-11/README.md:39,42`；`docs/features/level-cap-999-2026-11/README.md:153,180` |
| B7 | **`SET_HPMPALERT(0x1000)` 端到端回灌** | 🟡 / ⏳ | 上行 + 本地闪铃 + 滑条已接（`UIOptionMenu.cpp:1196-1220` 发 `SetHpMpAlertPacket`，`PacketSwitch.cpp:676` 注册 `UpdateHpMpAlertHandler`）；回灌待 `game_config.use_server_auto_pot=true`（碰配置需用户） | `docs/features/parity-gap-2026-11/README.md:51,183`；`pending-live-tests/README.md:22,68,418-426` |
| B8 | **`CHANGE_QUICKSLOT(183)` 持久化收口** | 🟡 | 2026-11-06 `UIQuickslotConfig` + `commit_quickslots` 已接，持久化列 P0.5 | `docs/features/parity-gap-2026-11/README.md:50,183`；`gap-inventory.md:97`（旧账判未发） |
| B9 | **`TemporaryStatView` 临时属性面板**（Buff 来源/剩余明细悬浮板，按 skill 分组） | ❌ 全缺 | s9 无独立 TEMPORARY_STAT 封包，但本端 `Player::buffs`（掩码+skillid+时长）已在状态包解析链 → **可本地驱动建窗，不等服务端** | `docs/features/parity-gap-2026-11/README.md:99,184`；`docs/features/field-marker-g11/README.md:16` |
| B10 | **`UIKeyConfig` 键位设置窗须走 v83→本枚举映射表** | 🟡 | 不接则「显示名与实际绑定不一致 = 半成品」 | `docs/features/ui-v83-window-restore/notes/open-issues.md:949`；`r15:154` |
| B11 | **`UIBossHP` 多 oid 分段**（双段扎昆同时只显最后一条） | 🟡 | 需 UI 布局决策 | `docs/features/map-object-audit/README.md:67` |

---

## 5. 类 C · 核心窗口（技能 / 任务 / 背包 / 装备 / 地图 / 商店 / NPC 对话 / 选项）

| # | 具体功能 | 状态 | 缺口形态 | 证据 |
| --- | --- | --- | --- | --- |
| C1 | **`UIMTS` 拍卖/ITC 完整列表窗** | 🟡 壳·显式预留 | `UIMTS.cpp:86-95` draw 仅 5 行 Text、`:97-99 void UIMTS::update() {}` 空体；`:111-174` 仅靠 SoftKey 发 `NpcTalkMorePacket`/`EnterMtsPacket`/`UseOwlPacket`/`OwlWarpPacket`；s9 无拍卖后端（Auction 0 文件 / 0 handler） | `IO/UITypes/UIMTS.cpp:2,86-99`；`docs/features/parity-gap-2026-11/README.md:89,104`；`gap-inventory.md:111`；`r12-remaining-features-audit.md:37-46` |
| C2 | **`UIOptionMenu` 退化空壳 / GameOpt·SysOpt 控件** | 🟡 | 本包无 `StatusBar3.img/OptionMenu`；`UIWindow.img/GameOpt` 与 `SysOpt` 底板只有 `backgrnd` 一个子（158×284 / 299×365），勾选框/滑条/下拉需运行时 `Basic.img` 叠加；两颗钮都开同一扇空壳、一个勾选框也没有 | `IO/UITypes/UIOptionMenu.h:66-76`；`IO/UIElement.h:195-199`；`docs/features/ui-v83-window-restore/notes/open-issues.md:686-689`；`docs/features/multi-resolution/README.md:30` |
| C3 | **`UIOptionMenu` 下拉展开态** | 🟡 | `UIOptionMenu.cpp:854-858` 只 log `"expanded list not implemented"`；画质档数 / VSync 阈值无原生依据 | `docs/features/ui-v83-window-restore/PLAN-R28.md:77`；`docs/features/multi-resolution/README.md:20,30-31,82` |
| C4 | **GameOpt 同意掩码三行 / D3 多选原选项** | 🟡 / ⊘ | `UNION_APPLY` 已接（原预留消解）；三行仍预留；底栏空壳入口/两窗居中重锚本版不做 | `docs/features/parity-gap-2026-11/README.md:176`；`docs/features/gameopt-invite-mask/README.md:23,25` |
| C5 | **`Title` 称号窗口**（已获得列表 + 佩戴切换） | ❌ → ⊘ 壳/显式预留 | 无 Type、无 handler；「s9 有 Title 下发面」已证伪（两侧 opcode 与全源码零命中），除非双端加 Title 系统 | `docs/features/parity-gap-2026-11/README.md:100,184` |
| C6 | **`Ranking` 排名窗** | 🟡 壳 | s9 无查询后端 → 空壳；原生本身是「排行不可用」提示，观感已对齐 | `docs/features/parity-gap-2026-11/README.md:103`；`r12:37-46` |
| C7 | **勋章窗** | 🟡 `UIMedal` 已落地（P0 KeyConfig 挂 MEDAL） | 详情窗/图标列仍预留；CharInfo medal stub 可能残留 | `IO/UITypes/UIMedal.*`；`KeyMapTranslate` 27→28；`angel-flow-audit-2026-11` P0 |
| C8 | **装备栏宠物装备三页 `BtPet1/2/3` 穿戴/卸下** | 🟡 预留 | `Equip/pet` 下无 `Slots` 子节点，槽位数据待协议批次 | `IO/UITypes/UIEquipInventory.cpp:567,622` |
| C9 | **`UIEquipInventory::slot_available` 判据** | 🟡 预留 | 「不做看着像就写死的格子表」，见 `evidence/r27-b3-equip/README.md §7` | `IO/UITypes/UIEquipInventory.cpp:564-570` |
| C10 | **背包中间那颗钮 `BtSort` vs `BtGather`** | 🟡 待 IDA | `UIItemInventory.cpp:1640-1641` 两颗叠在同坐标；线上 `_v_zclose.png` 只数出红 `|+` + 蓝 `✛`，中间红 `+` 缺件；需 IDA `UIInventory` 按钮回调表 | `docs/features/ui-v83-window-restore/notes/open-issues.md:691-699` |
| C11 | **任务窗「寻找 NPC 位置」按钮** | ⊘ 显式预留 | 按了没反应；无任何封包/IDA 证据可对照 → 不发明协议 | `docs/features/ui-v83-window-restore/notes/open-issues.md:818-824` |
| C12 | **任务窗 `BtOK,BtNo,BtAlert,BtMarkNpc` 按钮行为 / `UIQuestLog` 多处预留** | 🟡 | 预留标记 | `IO/UITypes/UIQuestLog.cpp:182,903,955`；`docs/features/ui-v83-window-restore/README.md:91,95,99,101` |
| C13 | **SkillEx / 宏 / 附魔 UI 深度、`QuestAlarm`、`ToolTip` tip 族齐全度** | 🟡 | r40 已铺 SkillEx/MacroSkill/QuestIcon/QuestAlarm 壳并接 `UpdateSkillHandler→UISkillUp` tip；深度对照仍 A/B 层 | `r14:91,92,102`；`r00:75`；`r40-utildlg-branches-remaining-shells.md:12,16-18`；`r40-continuous-shell-coverage.md:5` |
| C14 | **5 个 tooltip 组件指向不存在的 `UI["UIToolTip.img"]["Item"]`** | 🟡 资源边界 | `ItemTooltip.cpp:33`/`EquipTooltip.cpp:32`/`SkillTooltip.cpp:30`/`TextTooltip.cpp:30`/`MapTooltip.cpp:32`；`UIWindow.img/ToolTip` 只有 `Equip/cash/mesos`，无 c/e/ne/n/nw/w/sw/s/se；BeiDou.exe `UIToolTip` 0 命中 | `docs/features/ui-v83-window-restore/PLAN.md:20,34` |
| C15 | **装备对比多部位 / 双槽饰品 / 现金 bodyPart / 套装·成长 tooltip 联动** | ⊘ 明确不做 | 不穷举组合、不做停靠联动、不改装备属性计算与服务端；`:82` 回归待执行 | `docs/features/equip-compare/README.md:16,88,90` |
| C16 | **公会 BBS 界面完整度（734×526 多页签）** | 🟡 未验收 | 本端有 `BbsPackets`/`UIGuildBBS`，原生 `GuildBBS.img` 多页签完整度待真机（列表/读帖/发帖/公告/翻页） | `docs/features/parity-gap-2026-11/README.md:87`；`social-loop-audit-2026-11/README.md:31`；`pending-live-tests/README.md:21,27,195-213` |
| C17 | **公会副本 HUD（s2c 287 / 288）** | 🟡 handler 已注册 | `PacketSwitch` 664–665 已 emplace；pulley 上行仍预留；s9 脚本常不发需 GM 验 | `Net/PacketSwitch.cpp:664-665`；`guild-boss-hud`；`angel-flow-audit-2026-11` |
| C18 | **guild pulley 交互上行（`sub_558B45`）** | 🟡 显式预留 | 无服务端脚本消费证据，本轮未接 | `docs/features/pending-live-tests/README.md:229`；`docs/features/guild-boss-hud/README.md:28` |
| C19 | **NPC 对话分支深度：AskAvatar / AskBoxText / Quiz / SpeedQuiz / Mirror / AskPet** | 🟡 壳已铺、深度预留 | `UtilDlgEx_Avatar/_Pet/_MultiPet` 壳已建；仍预留 KeyDown 松开取消、AskPet 亲密/饱食原生位图条（无 gage→ColorBox）、Mirror 动画底/BtMain、Avatar UI、Quiz/SpeedQuiz UI、Pet Say `TEXT_X`、BoxText 真多行（现 Textfield 单行） | `docs/features/beidou-v17-parity/notes/r40-utildlg-branches-remaining-shells.md:8-9`；`pending-live-tests/README.md:263,325,340,342-407` |
| C20 | **`end_talk` UI 收口（msgtype 2/5/6/13）** | ✅ 已修 2026-11-06 | 旧版一律 `(x,-1)` 会踩 s9 `NPCMoreTalkHandler.readString()` 短包，已按读包契约分派 | `docs/features/pending-live-tests/README.md:335` |
| C21 | **`NPC_ACTION` 完整 VecCtrl 与 197 抓包** | 🟡 预留 | 最小上行 `MovePathEncode` 已接；197 对话姿态待真机 | `docs/features/change-quickslot-persist/README.md:34`；`gap-inventory.md:103`；`parity-gap-2026-11/README.md:57` |
| C22 | **`UIMapSearchPanel` 搜索逻辑 / 传送封包 / 结果数据绑定 / 加载状态文案** | 🟡 壳 ✅ 逻辑缺 | 早期稿明确 4 项未实现、传送按钮 `disabled`；2026-11 判「面板 ✅」，两者口径不同需现场复核 | `docs/features/map-search-ui/README.md:3,67-72,74-76`；`docs/features/parity-gap-2026-11/README.md:159` |
| C23 | **`UIMapInfoPanel` / `UIMapMobPanel` / `UIMapDropPanel`（含 278 掉落请求）** | ✅ 已闭环 | 早期 `docs/features/map-info-panel/README.md:38-40` 判未实现；2026-11 已交付 | `docs/features/parity-gap-2026-11/README.md:158` |
| C24 | **世界地图右侧掉落 UI 真数据装载** | 🟡 仅搭壳 | 左侧怪物闭环、右侧掉落壳 + `notes/drop-json-schema.md` 草案；`MobDropData` stub（永远 "No drop data"） | `docs/features/map-mob-drops/README.md:3,72,82`；`docs/features/map-expansion/PLAN.md:29,34` |
| C25 | **`MobDropData` 真掉落表 + 金币掉落区间** | ❌ stub | `Data/MobDropData.cpp:2` `stub loader`、`:12` `valid(false)`、`:15` `no drop tables`、`:17` LOG 空表；`UI/MapInfo/layout.json:255` gold drop interval 未填 | `Data/MobDropData.cpp:1-20`；`docs/features/map-expansion/PLAN.md:17,34` |
| C26 | **`MapMobInfo` 缺 `exp`/`boss` 字段** | ❌ | 信息面板 EXP/BOSS 前置 | `docs/features/map-expansion/PLAN.md:17,33` |
| C27 | **原生缺失资产：`WorldMap/WorldMapSearch` 搜索栏、`BtSearch`/`BtAutoFly_1`/`BtNaviRegister`、`MiniMap/ListNpc`、`MiniMapSimpleMode`** | ⊘ 显式预留 | 本 v83 包无素材，不强行实现 | `docs/features/map-expansion/PLAN.md:18,30,35` |
| C28 | **`UIMiniMap` TODO Toggle scrolling map / SMALL/BIG 未注册 / Mob·Npc tip 图标 / `BtSmall/BtBig/BtNpc`** | 🟡 显式预留·不臆造贴图 | 包内确无，保持空纹理跳过 | `IO/UITypes/UIMiniMap.cpp:516,666`；`docs/features/minimap-mode-switch/README.md:25,48,102,124`；`open-issues.md:684` |
| C29 | **`UIParty` 邀请/离开/队长/开除四钮** | 🟡 显式预留 | 本包 UI.nx 根下无 `Party.img`，v83 也无独立队伍窗根；队伍操作仍有 SoftKey L/E 与菜单入口 | `IO/UITypes/UIParty.cpp:38-45` |
| C30 | **`UIStorage` `BtArrange/BtMesoIn/BtMesoOut`** | 🟡 显式预留 | 本包无 `Storage.img`（v83 真身 `UIWindow.img/Trunk` 463×318），无参考截图也无 IDA 证据 | `IO/UITypes/UIStorage.cpp:40-48` |
| C31 | **`UIUserList::change_target(int32_t)` / `BT_GUILD_CHAT` 本地回显** | 🟡 预留空实现 | v83 家族聊天走聊天框目标 / `/g` 斜杠别名路由（`UIChatBar::input_text_enter_callback` 已接） | `IO/UITypes/UIUserList.cpp:914-920`；`social-loop-audit-2026-11/README.md:115` |
| C32 | **`UIUserList` 第 4 页签「家族联盟」** | ❌ 只有美术 | 无 `GuildUnion` 页面，`BT_TAB_UNION` 显式不切页；在线 2 行/离线 5 行无 Slider（成员更多需补分页） | `docs/features/social-loop-audit-2026-11/README.md:113,116` |
| C33 | **商店/仓库/玩家商店列表加长（#3/4/5）** | 🟡 | 基础链路已通（`UIStorage` 全链 dispatch），「加长/更高列表」未见改动 | `docs/features/parity-gap-2026-11/README.md:132`；`01-parity-matrix-and-plan.md:59-73` |
| C34 | **CashShop 礼物/礼包深度、`CashTradingRoom`、个人商店店主/雇佣变体** | 🟡 旧账→壳已铺 | r40 已铺 PersonalShop/MemberShop/TradingRoom/EntrustedShop/CashTradingRoom 壳，深度仍未达 C 层 | `r14:105,106,110`；`r16:167`；`r40-continuous-shell-coverage.md:5` |
| C35 | **`UICashShop` stub 文案 / 购买未完整** | 🟡 | `cashshop.stub_job`/`stub_name` | `IO/UITypes/UICashShop.cpp:100-102`；`locale/zh-CN.ini:343-344` |
| C36 | **`UIGameMenu`「场景变更」** | ⊘ 显式预留 | 只回聊天栏黄字「场景变更暂未支持。」（本客户端无任何皮肤 UI/协议线，按门禁不发明客户端能力） | `docs/features/ui-v83-window-restore/notes/open-issues.md:54-62` |
| C37 | **「商城暂未支持」/ C5 后续** | 🟡 | 线上已复验黄字提示，W2 交付面正常 | `docs/features/ui-v83-window-restore/notes/open-issues.md:861` |
| C38 | **中文 IME 输入（T13 输入侧）** | ⬜ 预留 | 「先显示后输入」，软键盘替代；`IO/Window.cpp:901-925 begin_ascii_text_input()` **实际已 `SDL_StartTextInput` 开启 IME**（`charset.md:193` 说「仍关 IME」是 2026-10-31 旧态，行号也过时）；真缺口是 `Textfield` 中文光标（属 T13 输入侧） | `docs/features/s9-protocol-align/README.md:60`；`docs/features/parity-gap-2026-11/README.md:171`；`docs/features/s9-protocol-align/notes/charset.md:145,234-236` |
| C39 | **中文显示同源受益面未取真机帧** | 🟡 | 长中文换行、`ChatBalloon` 按 `width()` 拉伸 9 切片、`Text::Background::NAMETAG` 的 `right=left+w+3`、`Textfield` 中文光标 | `docs/features/s9-protocol-align/notes/charset.md:234-236` |
| C40 | **3 个 `UNHANDLED_EMITTED` 功能实现** | ❌ | 属特性开发需补 UI/逻辑 | `docs/features/s9-protocol-align/README.md:60` |

---

## 6. 类 D · 社交与经济

| # | 具体功能 | 状态 | 缺口形态 | 证据 |
| --- | --- | --- | --- | --- |
| D1 | **`BUDDYLIST_MODIFY(0x82)` 好友增删发送路径** | 🟡 需先补 UI | 增删发送路径未接线；好友列表 c2s 错配 63→130 已修（不同包，勿混） | `gap-inventory.md:80`；`social-loop-audit-2026-11/README.md:26` |
| D2 | **`PARTY_SEARCH_REGISTER(0xDC/220)`** | 🟡 已代理消化 | s9 0xDC 空 stub，用 `PARTY_SEARCH_START/UPDATE(222/223)` 代理，视同消化 | `gap-inventory.md:109`；`parity-gap-2026-11/README.md:58`；`social-loop-audit-2026-11/README.md:17` |
| D3 | **公会改职衔 `0x08` 出站** | 🟡 缺钮 | 服务端有 handler，客户端缺按钮，低优先 | `social-loop-audit-2026-11/README.md:30` |
| D4 | **创建联盟** | ⊘ 端内无需新包 | s9 走 NPC 脚本 `createAlliance` 非 c2s | `social-loop-audit-2026-11/README.md:30`；`r12:98` |
| D5 | **留言条 `NOTE_ACTION(83/131)` 路径** | 🟡 需复核 | 是否真发待 r-note；s9 `NoteActionHandler` 需 `getAvailableNotes()>0` 才落库 | `parity-gap-2026-11/README.md:88`；`pending-live-tests/README.md:29,231-241` |
| D6 | **怪物图鉴增强版（#36，D7 期望）** | 🟡 | v83 卡册已交付；**angel-flow P1 已接 String/MonsterBook→Mob 名**；便签套筛选 / D7 增强版仍预留；stub 仅 reverse-map 未命中时回退 | `angel-flow-audit-2026-11/P1.md`；`parity-gap-2026-11/README.md:152` |
| D7 | **每日签到：领取 / 杀怪 / 账号日限 / 等级·MVP** | ❌（壳在，双端协议缺） | `UIDailyGift` 615 行 28 天 7×4 窗已从 `UIEvent::239` 开，现用 ColorBox 灰框、点格假领取；`try_claim`/`on_claim_result`/`set_kill_progress`/`set_account_daily_limit`/`set_level_gate`/`set_mvp_tier` 全为阶段 3+ 预留，`grep .dispatch` 零命中；s9 两侧零 DAILY/SIGN/CHECKIN opcode | `IO/UITypes/UIDailyGift.h:28-60`（`:32` 纯客户端假数据、`:33` 道具槽不渲染、`:58` 逻辑预留）；`UIDailyGift.cpp:113,357`；`docs/features/daily-gift-hd/README.md:16,20-21,74-84`；`parity-gap-2026-11/README.md:206-228` |
| D8 | **每日签到 v4 皮肤件（513×346 PNG + 分层 PSD + cell 四态）** | 🟡 素材已备未换 | 换皮属「改已有功能」，按门禁 18 走 Photoshop+IMG 且需用户确认 v4 即目标形态 | `docs/features/parity-gap-2026-11/README.md:212-214,227-228` |
| D9 | **交易所 `trade-house-083`** | ❌ 仅设计 | 有 DESIGN/PLAN-ui，无代码、s9 无后端 | `docs/features/parity-gap-2026-11/README.md:156`；`docs/features/trade-house-083/DESIGN.md` |
| D10 | **抽奖 XyLottery（#39）** | ❌ | 双端皆无；商城「彩票」是 CashGachapon 面 | `docs/features/parity-gap-2026-11/README.md:155` |
| D11 | **婚礼全套（c2s 137/138/139 + s2c 72/73/74/75/106/320/321）** | ✅ 已闭环 2026-11-06 | 曾判「壳/不做」，前提证伪后 s9 RingAction/Wedding/WeddingTalk 全存在；**合影仍预留**（s9 需外部 WvsMapGen）、婚礼服装/门票走既有 CashShop、Amoria 地图物件资源面未盘点 | `parity-gap-2026-11/README.md:83,105`；`social-loop-audit-2026-11/README.md:19`；`docs/features/wedding-flow/README.md:7-8,128-135` |
| D12 | **Messenger 立绘实机确认（0.70 / foot -10）** | ⏳ | 立绘/关闭钮 ✅ 已闭环；blink/chatBalloon 单帧需双角色同房手点补拍 | `docs/features/beidou-v17-parity/notes/r21-remaining-backlog.md:15`；`pending-live-tests/README.md:19,24-26` |
| D13 | **PersonalShop CREATE 自由市场验收** | ⏳ | 需 FM 房间 `910xxxxxx` + `514*/503*` 许可证 | `r21-remaining-backlog.md:14`；`pending-live-tests/README.md:17,154-163` |
| D14 | **MonsterBook / Maker / Alliance / Storage / Trade / Merchant / Guild / Party / Buddy / Messenger / Delivery / StoreBank / Incubator 实体窗** | ✅ 均已有 | r12/r802/r804 已闭环；覆写 r00:95-143 与 r14 §3.2 旧账 | `parity-gap-2026-11/README.md:107`；`social-loop-audit-2026-11/README.md:17-19,27-31` |
| D15 | **`UIWedding`「本端不做判定，只发 action=1」** | 🟡 壳·玩法裁定 | 婚礼链已闭环，深度玩法判定不做 | `IO/UITypes/UIWedding.cpp:280`；`docs/features/wedding-flow/README.md:7-8` |

---

## 7. 类 E · 宠物 / 召唤兽 / 龙（**协议已发，缺玩法深度 / 真渲染 / UI 分支**）

> ⚠️ **本节已被 §9.5 覆写**：`docs/features/parity-gap-2026-11/README.md:69-79` 原话「宠物系统 = 能看到、不能用」+ `gap-inventory.md:57-96` 判「c2s 整片不发」是 **旧 head `44d0d5d9ca`（`client_out_sent=103`）读数**。2026-11-08 `opcode-matrix.json` 实测 `client_out_sent=148`，子代理 15 条抽样 + 主 agent 逐字复核证明 E1/E2/E4/E5/E6/E10/E11/E12/E15/E16/E17/E18 **全部有 `.dispatch()` 发送点**。故本节表格状态列改写为「✅ 协议已发 / 🟡 玩法深度 / ❌ 真缺」三层；**立项勿按旧「补发包」动工**。
> s2c 渲染层已接：`Net/PacketSwitch.cpp:565-599` 注册 `SPAWN_PET/PET_CHAT/PET_NAMECHANGE/PET_COMMAND/SPAWN_SPECIAL_MAPOBJECT→SpawnSummonHandler/REMOVE_SPECIAL_MAPOBJECT`。
> 源码侧：`Character/Inventory/Pet.h:25`、`Character/Look/PetLook.h:28`、`Net/Packets/PetPackets.h` 8 包、`Net/Packets/SummonDragonPackets.h`、`Net/Handlers/MapObjectHandlers.cpp:208,213,218` 齐；**无独立 `UIPet.h`、无真渲染**（`Gameplay/Stage.h:80` `// Field placeholders (summon/mist/door/kite/dragon) until full sprites`）。

| # | 具体功能 | opcode | 状态（§9.5 覆写后） | 证据 |
| --- | --- | --- | --- | --- |
| E1 | 召唤宠物上线 / 使用 | `0x62 SPAWN_PET` (98) | ✅ 协议已发 | `IO/UITypes/UIItemInventory.cpp:830` `SpawnPetPacket(...).dispatch();`；`Net/Packets/PetPackets.h:19` |
| E2 | 宠物喂食 | `0x4C PET_FOOD` (76) | ✅ 协议已发 | `Net/Packets/PetPackets.h:34` + `Net/Packets/InventoryPackets.h:139` 动态分派（`UseItemPacket`，`UIItemInventory.cpp:815`） |
| E3 | 坐骑喂食 | `0x4D USE_MOUNT_FOOD` (77) | ✅ 协议已发 | `Net/Packets/InventoryPackets.h:135-151` `UseItemPacket` 动态分派 |
| E4 | 宠物移动 | `0xA7 MOVE_PET` (167) | ✅ 协议已发 | `Net/Packets/PetPackets.h:62` `MovementPacket(OutPacket::Opcode::MOVE_PET)` |
| E5 | 宠物对话 | `0xA8 PET_CHAT` (168) | ✅ 协议已发 | `IO/UITypes/UIMiscShells.cpp:6870` `PetChatPacket(...).dispatch();`；`Net/Packets/PetPackets.h:77` |
| E6 | 宠物指令 | `0xA9 PET_COMMAND` (169) | ✅ 协议已发 | `IO/UITypes/UIMiscShells.cpp:6857` `PetCommandPacket(...).dispatch();`；`Net/Packets/PetPackets.h:93` |
| E7 | 宠物拾取 | `0xAA PET_LOOT` (170) | 🟡 有构造类、运行时 dispatch 未逐条证 | `Net/Packets/PetPackets.h:108` |
| E8 | 宠物自动药水 | `0xAB PET_AUTO_POT` (171) | 🟡 有构造类、运行时 dispatch 未逐条证 | `Net/Packets/PetPackets.h:123` |
| E9 | 宠物排除物品 | `0xAC PET_EXCLUDE_ITEMS` (172) | 🟡 有构造类、运行时 dispatch 未逐条证 | `Net/Packets/PetPackets.h:139` |
| E10 | 召唤袋使用 | `0x4B USE_SUMMON_BAG` (75) | ✅ 协议已发 | `Net/Packets/InventoryPackets.h:135-151` `UseItemPacket` 动态分派 |
| E11 | 召唤兽移动 | `0xAF MOVE_SUMMON` (175) | ✅ 协议已发 | `Net/Packets/SummonDragonPackets.h:19` `MovementPacket(...MOVE_SUMMON)`；`Net/PacketSwitch.cpp:604` 有 s2c |
| E12 | 召唤兽攻击 | `0xB0 SUMMON_ATTACK` (176) | ✅ 协议已发 | `Net/Packets/SummonDragonPackets.h:42`；`Net/PacketSwitch.cpp:605` 有 s2c |
| E13 | 召唤兽受伤上报 | `0xB1 DAMAGE_SUMMON` (177) | 🟡 有构造类、运行时 dispatch 未逐条证 | `Net/Packets/SummonDragonPackets.h`；`Net/PacketSwitch.cpp:606` 有 s2c |
| E14 | 凝视体 Beholder | `0xB2 BEHOLDER` (178) | 🟡 有构造类、运行时 dispatch 未逐条证 | `Net/Packets/SummonDragonPackets.h` |
| E15 | 战神龙移动 | `0xB5 MOVE_DRAGON` (181) | ✅ 协议已发 | `Net/Packets/SummonDragonPackets.h:96` `MovementPacket(...MOVE_DRAGON)`；`Net/PacketSwitch.cpp:609` 有 s2c |
| E16 | 捕捉道具（怪物捕捉球） | `0x51 USE_CATCH_ITEM` (81) | ✅ 协议已发 | `IO/UITypes/UIItemInventory.cpp:801` `UseCatchItemPacket(slot, item_id, mob_oid).dispatch();` |
| E17 | 死亡道具（复活币类） | `0x35 USE_DEATHITEM` (53) | ✅ 协议已发 | `IO/UITypes/UIItemInventory.cpp:824` `UseDeathItemPacket(item_id).dispatch();` |
| E18 | 远程扭蛋 | `0x74 USE_REMOTE` (116) | ✅ 协议已发 | `IO/UITypes/UIRemoteGachapon.cpp:83,99` `UseRemoteGachaponPacket(...).dispatch();`；入口 `UIItemInventory.cpp:834,887` |
| E19 | AskPet 亲密/饱食原生位图条 | — | ❌ 真缺·无 gage 资源 → ColorBox 占位 | `pending-live-tests/README.md:263,407` |
| E20 | Summon/Dragon/Door/Portal/MonsterStatus/Catch/ItemFX/CharBox 场标 → 真精灵 | — | ❌ 真缺·部分已修（G11/G6/G7），余者仅 FieldMarker 文本 | `Gameplay/Stage.h:80`；`docs/features/map-object-audit/evidence/codebuddy-baseline.txt:173,193,199-203`；`README.md:40-46` |
| E21 | **宠物装备三页穿戴 / 卸下**（`BtPet1/2/3`） | — | ❌ 真缺·显式预留 | `IO/UITypes/UIEquipInventory.cpp:622` 「Equip/pet 下无 Slots 子节点，槽位数据待协议批次」 |
| E22 | **`PARTY_SEARCH_REGISTER(220)` 真发包**（现用 `PARTY_SEARCH_START` 代理） | `0xDC` (220) | ❌ 真缺·客户端无枚举 | `IO/UITypes/UIPartySearch.cpp:246`；§9.5.2 #10 |

> **类 E 收敛结论（推断）**：整族 c2s 已 148/148 全覆盖，旧账「与原生差距最大的一整块玩法面」应改写为「**协议面已对齐，玩法深度 / 真渲染 / 宠物装备三页 / 场标精灵 / AskPet 位图条 / 部分运行时 dispatch 未逐条验收**」。真缺收敛为 E19–E22 四项 + §9.5.3 的场地伤害族。

---

## 8. 类 F · 活动与小游戏

| # | 具体功能 | 状态 | 缺口形态 | 证据 |
| --- | --- | --- | --- | --- |
| F1 | 雪球 `SNOWBALL(211)` / 椰子 `COCONUT(213)` / 怪物嘉年华 `MONSTER_CARNIVAL(218)` | ⊘ 明确不做 | 活动小游戏/内容，维持既有裁决 | `gap-inventory.md:105,107,108`；`parity-gap-2026-11/README.md:83` |
| F2 | Minigame / RPS / MonsterCarnival / PartyRace / DualMobGauge / NewYearsCard / TreasureBox / plant·raise | 🟡 旧账 ❌ → 壳/接线已铺 | r40 已铺 `MinigameTable` 并接 `PLAYER_INTERACTION ROOM/READY/START/EXIT`；TreasureBox 由 `g10-treasure-box-semantics` 销案；深度玩法未达 C 层 | `r14:147-158`；`r40-continuous-shell-coverage.md:5,7`；`r40-utildlg-branches-remaining-shells.md:12,18` |
| F3 | 每日礼物 F 动画逻辑 / A1 服务端闭环 | ⊘ 仅 UI | A1 仅 UI 不做服务端闭环、F 动画全做逻辑不做、E 仅预留；本地 VS 编译验收待用户 | `docs/features/daily-gift-hd/README.md:16,20,21,74,81` |
| F4 | MapleTV 开播态（Messenger 5075xxx 后 15~60s 立绘 + 5 行文案） | 🟡 显式预留 | `SEND_TV` 只保留提示窗，待 REA/IDA 取证后另立 Plan | `docs/features/pending-live-tests/README.md:473-474` |
| F5 | MapleTV 双屏 / 点击事件 / 装备对比（2026-11-07 回归余项） | ⏳ 需手点 | 自动脚本无换图能力 / 坐标未命中 / 测试号无可穿戴道具 | `docs/features/pending-live-tests/README.md:467-471` |
| F6 | `UISpecial` 自造「福利中心」5 钮发 `SpecialRequestPacket(0..4)` | 🟡 推断非 v83 原生 | 95 行薄窗 | `IO/UITypes/UISpecial.cpp`（95 行） |

---

## 9. 类 G · 协议与封包（s2c 差集 / c2s 旧账三分类 / **§9.5 权威覆写**）

> ⚠️ **本节 §9.1–§9.4 基于 `gap-inventory.md`（旧 head `44d0d5d9ca`），是 2026-10-31 历史三分类账，协议层已被 §9.5 覆写；立项/接线一律以 §9.5 + `wave4-battle-packets/README.md` 为准，勿按本节旧表动工。**
> 旧口径（`gap-inventory.md:1-5`）：`gap = {PacketProcessor.registerHandler 生效值} − {客户端 OutPacket::Opcode client_sent=true}`。
> 旧读数：s9 注册 handler **172**、客户端 `client_out_sent` **103**、矩阵 `c2s MATCHED 99 / CLIENT_ONLY 4`；`LOGIN_EMAIL=3` 客户端独有不计入。客户端注册 **301** handler、s9 下发 **255**。
> 旧账 `tools/probe/opcode_blocklist.txt` 89 条；gap 内探针禁发 39 / 探针安全 31。

### 9.1 s2c 差集 3 个（s9 会发、客户端没接）

| opcode | s9 名 | 状态 | 缺口 | 证据 |
| --- | --- | --- | --- | --- |
| 287 | `GUILD_BOSS_HEALER_MOVE` | 🟡 已注册 | handler 在；真机/脚本下发待验 | `PacketSwitch.cpp:664` |
| 288 | `GUILD_BOSS_PULLEY_STATE_CHANGE` | 🟡 已注册 | 同上；pulley 上行预留 | `PacketSwitch.cpp:665` |
| 376 | `WORLD_MAP_PLAYERS` | ✅ 真机闭环 | 静态扫描判「回包不处理」，但 2026-11-05 真机 `SEND 15 01…`(277)→`RECV 78 01…`(376) UI `0 players` 闭环 | `pending-live-tests/README.md:14,88,115-125` |

两个假阳性已澄清：`278 WORLD_MAP_DROP_REQUEST` 已接（`UIMapInfoPanel.cpp:231`）、`63 HIRED_MERCHANT_REQUEST` 走 `PLAYER_INTERACTION(123)` 覆盖（`parity-gap-2026-11/README.md:30-32`）。

### 9.2 c2s「可实现 31 条」——已按 §9.5 权威三态逐条覆写

> ⚠️ 原表是 `parity-gap-2026-11/README.md`(2026-11-04/05) 的现场判定，比 `gap-inventory.md` 新但**比 §9.5(2026-11-08 源码逐字) 旧**。两处硬推翻：`CANCEL_ITEM_EFFECT(0x49)`、`AUTO_DISTRIBUTE_AP(0x58)` 原标 ❌，§9.5.2 已用 `IO/UITypes/UIBuffList.cpp:246`、`IO/UITypes/UIStatsInfo.cpp:752` 的 `.dispatch()` 证为 ✅；场地伤害族 `0xBF/0xC0/0xC1/0xC2/0xD4` 与 `0x04/0x09/0x38` 原标 🟡，§9.5.3 证为 ❌ **完全无**（`Net/OutPacket.h` 零枚举）。下表状态列已改为 §9.5 三态。

| opcode | 名 | §9.5 三态（权威） | 证据 |
| --- | --- | --- | --- |
| `0x0004` | `SERVERLIST_REREQUEST` | ❌ 完全无（无枚举无发包） | §9.5.3 #2 |
| `0x0009` | `AFTER_LOGIN` | ❌ 完全无 | §9.5.3 #2 |
| `0x001C` | `RELOG` (28) | ⊘ 明确不做 | `Net/OutPacket.h:42` 注「裁决明确不做」 |
| `0x002F` | `TOUCH_MONSTER_ATTACK` (47) | ✅ 有发送点 | `Gameplay/Stage.cpp:356`（§9.5.2 #1） |
| `0x0038` | `MOB_BANISH_PLAYER` (56) | ❌ 完全无 | §9.5.3 #2 |
| `0x0049` | `CANCEL_ITEM_EFFECT` (73) | ✅ 有发送点 | `IO/UITypes/UIBuffList.cpp:246`（§9.5.2 #2） |
| `0x004E` | `SCRIPTED_ITEM` (78) | ✅ 有构造（`UseItemPacket` 动态分派） | `Net/Packets/InventoryPackets.h:135-151` |
| `0x0052` | `USE_SKILL_BOOK` (82) | ✅ 有构造（同上） | `Net/Packets/InventoryPackets.h:135-151` |
| `0x0058` | `AUTO_DISTRIBUTE_AP` (88) | ✅ 有发送点 | `IO/UITypes/UIStatsInfo.cpp:752`（§9.5.2 #3） |
| `0x0059` | `HEAL_OVER_TIME` (89) | ✅ 已接自然回复上报 | `parity-gap-2026-11/README.md:49` |
| `0x005C` | `CANCEL_BUFF` (92) | ✅ 有发送点 | `IO/UITypes/UIBuffList.cpp:246`（else 分支 `CancelBuffPacket`） |
| `0x005D` | `SKILL_EFFECT` (93) | ✅ 有发送点（KeyDown 松开取消未接） | `Gameplay/Combat/Combat.cpp:181,222`（§9.5.2 #6） |
| `0x0063` | `CANCEL_DEBUFF` (99) | ✅ 已接（空包，真解除走药剂 72） | `parity-gap-2026-11/README.md:40` |
| `0x0064` | `CHANGE_MAP_SPECIAL` (100) | 🟡 有枚举、dispatch 未逐条证 | `parity-gap-2026-11/README.md:52` |
| `0x0065` | `USE_INNER_PORTAL` (101) | 🟡 同上 | `parity-gap-2026-11/README.md:52` |
| `0x006D` | `GRENADE_EFFECT` (109) | 🟡 依赖内容未建（Wave4 判真缺且已接） | `parity-gap-2026-11/README.md:64` |
| `0x0070` | `USE_ITEM_REWARD` (112) | 🟡 有构造、dispatch 未逐条证 | `parity-gap-2026-11/README.md:52` |
| `0x0073` | `USE_TREASUER_CHEST` (115) | 🟡 有构造、dispatch 未逐条证 | `docs/features/map-object-audit/README.md:45` |
| `0x00A2` | `CLICK_GUIDE` (162) | ✅ 已接 | `parity-gap-2026-11/README.md:57` |
| `0x00B7` | `CHANGE_QUICKSLOT` (183) | ✅ 有发送点 | `IO/Keyboard.cpp:186`、`IO/UITypes/UIKeyConfig.cpp:1245`（§9.5.2 #4） |
| `0x00BD` | `AUTO_AGGRO` (189) | 🟡 依赖内容（Wave4 判真缺且已接） | `parity-gap-2026-11/README.md:64` |
| `0x00BF` | `FIELD_DAMAGE_MOB` (191) | ❌ 完全无 | §9.5.3 #1 |
| `0x00C0` | `MOB_DAMAGE_MOB_FRIENDLY` (192) | ❌ 完全无 | §9.5.3 #1 |
| `0x00C1` | `MONSTER_BOMB` (193) | ❌ 完全无 | §9.5.3 #1 |
| `0x00C2` | `MOB_DAMAGE_MOB` (194) | ❌ 完全无 | §9.5.3 #1 |
| `0x00C5` | `NPC_ACTION` (197) | ✅ 已接（完整 VecCtrl 待真机） | `parity-gap-2026-11/README.md:57` |
| `0x00CE` | `TOUCHING_REACTOR` (206) | 🟡 有枚举、dispatch 未逐条证 | `Net/OutPacket.h:177` |
| `0x00D4` | `LEFT_KNOCKBACK` (212) | ❌ 完全无 | §9.5.3 #1 |
| `0x00EE` | `USE_ITEMUI` (238) | 🟡 Raise/Plant 变体 | `gap-inventory.md:110` |
| `0x1000` | `SET_HPMPALERT` (4096) | ✅ 有发送点（端到端回灌待配置） | `IO/UITypes/UIOptionMenu.cpp:1216`（§9.5.2 #5） |

> Wave 4 覆写：`docs/features/parity-gap-2026-11/README.md:64-67` 明令「未来会话接线一律以 `docs/features/wave4-battle-packets/README.md` 为准，勿按本节原表动工」；协议三态再以 §9.5 为准。

### 9.3 「需先补 UI」21 条 —— 已被 §9.5 逐条覆写（详见类 E / §9.5.4）

> ⚠️ 旧账 `gap-inventory.md:57,60,64,70,77,80,86-96,109,118` 把下列整族判「❌ 客户端发不出、需先补 UI」。**§9.5.2 / §9.5.4 / §7 类 E 已逐字推翻**：`0x0035 USE_DEATHITEM`(`UIItemInventory.cpp:824`)、`0x004B USE_SUMMON_BAG` / `0x004C PET_FOOD` / `0x004D USE_MOUNT_FOOD`(`InventoryPackets.h:135-151` + `UIItemInventory.cpp:815`)、`0x0051 USE_CATCH_ITEM`(`UIItemInventory.cpp:801`)、`0x0062 SPAWN_PET`(`UIItemInventory.cpp:830`)、`0x0074 USE_REMOTE`(`UIRemoteGachapon.cpp:83,99`)、`0x0082 BUDDYLIST`(`UIBuddyList.cpp:94-283` + `BuddyPackets.h:32,43,53`)、`0x00A7/A8/A9 MOVE_PET/PET_CHAT/PET_COMMAND`(`PetPackets.h:62` + `UIMiscShells.cpp:6857,6870`)、`0x00AF/B0/B5 MOVE_SUMMON/SUMMON_ATTACK/MOVE_DRAGON`(`SummonDragonPackets.h:19,42,96`) 均已是 **✅ 有发送点**。
> 全表 21 条：`0x000E PICK_ALL_CHAR` `0x0035 USE_DEATHITEM` `0x004B USE_SUMMON_BAG` `0x004C PET_FOOD` `0x004D USE_MOUNT_FOOD` `0x0051 USE_CATCH_ITEM` `0x0062 SPAWN_PET` `0x0074 USE_REMOTE` `0x0082 BUDDYLIST` `0x00A7 MOVE_PET` `0x00A8 PET_CHAT` `0x00A9 PET_COMMAND` `0x00AA PET_LOOT` `0x00AB PET_AUTO_POT` `0x00AC PET_EXCLUDE_ITEMS` `0x00AF MOVE_SUMMON` `0x00B0 SUMMON_ATTACK` `0x00B1 DAMAGE_SUMMON` `0x00B2 BEHOLDER` `0x00B5 MOVE_DRAGON` `0x00DC PARTY_SEARCH_REGISTER`。
> **唯一真剩**：`0x00DC PARTY_SEARCH_REGISTER(220)` 客户端无枚举无发包（`IO/UITypes/UIPartySearch.cpp:246` 用 `PARTY_SEARCH_START` 代理，§9.5.2 #10 / §7 E22）；`0xAA/AB/AC/B1/B2` 五条为「🟡 有构造类、运行时 dispatch 未逐条证」。`0x000E PICK_ALL_CHAR` 仍是 r10 概览解析未接线（`gap-inventory.md:56`）。旧「共 20 / 实列 21」计数漂移见 §9.5.3 #10。

### 9.4 其它协议完整度

| # | 具体功能 | 状态 | 证据 |
| --- | --- | --- | --- |
| G1 | ~~70 个 c2s opcode 客户端从不发~~ → **旧账作废**：2026-11-08 实测 `client_out_sent=148/148`（149 命名常量），「70 差集」是 `gap-inventory.md` 旧 head `44d0d5d9ca` 读数；真「完全无」收敛为 §9.5.3 的场地伤害族 / `0x04/0x09/0x38/0x1C` / `PARTY_SEARCH_REGISTER` / `MTS_OPERATION` / `CUSTOM_PACKET` 等 ~16 项 | 🟡 **已被 §9.5 覆写** | `docs/features/s9-protocol-align/protocol/opcode-matrix.json:26-38`；§9.5.1 / §9.5.3 |
| G2 | 战斗/环境内部报文 9 条（56/109/189/191/192/194/193/212/47） | 🟡 | `parity-gap-2026-11/README.md:60-67` |
| G3 | `BBS_OPERATION(155)` 本端有 `BbsPackets`，界面完整度未验收 | 🟡 | `parity-gap-2026-11/README.md:87` |
| G4 | `NOTE_ACTION(83)` CharInfo 写 note 是否真发 | 🟡 | `parity-gap-2026-11/README.md:88` |
| G5 | `CASHSHOP_OPERATION(229)` / `CHECK_CASH(236)` / `USE_ITEMUI(238)` 判已接（商城购买 `CashShopBuyPacket` 四处 dispatch）；MTS/拍卖窗为壳 | 🟡 | `parity-gap-2026-11/README.md:89` |
| G6 | 3 个 `UNHANDLED_EMITTED` 功能实现 | ❌ | `docs/features/s9-protocol-align/README.md:60` |

### 9.5 ★ 协议层 2026-11-08 权威重算（覆写 §9.1–§9.4 全部旧判定）

> 来源：第三路只读子代理 `432be4cd` + 主 agent 逐字复核。权威源 `docs/features/s9-protocol-align/protocol/opcode-matrix.json`（`generated_at=2026-11-08T00:56:27`，`s9_git_head=3d5ccc6f8e48f79be266448eee28cf1a72e75f30`，`generated_by=tools/gen_opcode_matrix.py`）。客户端注册表 **`Net/PacketSwitch.cpp:417-782` 的 `emplace<OPCODE, Handler>()`**（非 `registerHandler`/`PacketProcessor`）；出站枚举 **`Net/OutPacket.h:37-318`**。

#### 9.5.1 权威读数（旧账 → 新账）

| 指标 | `gap-inventory.md`（旧 head `44d0d5d9ca`） | `opcode-matrix.json`（2026-11-08） | 证据 |
| --- | --- | --- | --- |
| `client_out_constants` / `client_out_sent` | 103 | **148 / 148**（149 命名常量，`PLAYER_UPDATE=223` 是 `PARTY_SEARCH_UPDATE` 别名 `Net/OutPacket.h:225`） | `opcode-matrix.json:29-30`；`Net/OutPacket.h:37-318` |
| `client_in_constants` / `client_in_registrations` | 301 | **306 / 305** | `opcode-matrix.json:27-28`；`Net/PacketSwitch.cpp:52-415,417-782` |
| `s9_registered_handlers` | 172 | **174**（`s9_registration_sites=175`，`s9_send_emitted=252`） | `opcode-matrix.json:31-37` |
| c2s 构造点 | — | **238 处 `Net/Packets/*.h` 的 `OutPacket::Opcode::X`**，约 45 个 header | `Net/Packets/*.h` |
| `.dispatch()` 调用点 | — | **917 处**（全仓 `XxxPacket(...).dispatch()`） | 全仓 grep |

**三态判据**：`有发送点`=有 `OutPacket` 构造且 `.dispatch()` 可达；`仅定义`=仅枚举/构造无运行时发送；`完全无`=客户端 C++ 源码零命中（含无枚举）。子代理 15 条抽样中「仅定义」态 **0 命中** —— 当前 148 枚举全部有构造类，无发送的都是「完全无」（连枚举都没有），不存在「有枚举无构造」的中间态。〔观察到〕

#### 9.5.2 推翻旧判定的关键项（抽样 15 条逐字复核）

| # | opcode / 旧账判定 | 新三态 | 逐字证据（`路径:行号`） |
| --- | --- | --- | --- |
| 1 | `TOUCH_MONSTER_ATTACK(0x2F/47)` 旧「可实现/禁发」 | ✅ **有发送点** | `Gameplay/Stage.cpp:356` `AttackPacket(...,OutPacket::Opcode::TOUCH_MONSTER_ATTACK).dispatch()`；`Net/OutPacket.h:77` |
| 2 | `CANCEL_ITEM_EFFECT(73)` 旧「❌ 无取消入口」 | ✅ **有发送点** | `IO/UITypes/UIBuffList.cpp:244-246` `if (skillid>=1000000) CancelItemEffectPacket(skillid).dispatch();`；`Net/OutPacket.h:124` |
| 3 | `AUTO_DISTRIBUTE_AP(88)` 旧「❌ UIStatsInfo 无该钮」 | ✅ **有发送点** | `IO/UITypes/UIStatsInfo.cpp:716,752` `AutoAssignApPacket(to_code(primary), ap, second_code, 0).dispatch();`；`Net/OutPacket.h:158` |
| 4 | `CHANGE_QUICKSLOT(183)` 旧「未发」 | ✅ **有发送点** | `IO/Keyboard.cpp:186`、`IO/UITypes/UIKeyConfig.cpp:1245`（`UIQuickslotConfig.cpp:143` 仅 LOG 不发包） |
| 5 | `SET_HPMPALERT(0x1000)` 旧「🟡 上行已接」 | ✅ **有发送点**（确认） | `IO/UITypes/UIOptionMenu.cpp:1216` `SetHpMpAlertPacket(...).dispatch();`；`Net/Packets/MiscOutPackets.h:59-71`；`Net/OutPacket.h:306` |
| 6 | `SKILL_EFFECT(93)` 旧「可实现/禁发」 | ✅ **有发送点** | `Gameplay/Combat/Combat.cpp:181,222`；`Net/OutPacket.h:184` |
| 7 | `PET_FOOD(76)` 旧「❌ 喂食未接线」 | ✅ **有发送点** | `Net/Packets/PetPackets.h:34` + `Net/Packets/InventoryPackets.h:139` 动态分派（`UseItemPacket` 14 处 dispatch，如 `IO/UITypes/UIItemInventory.cpp:815`） |
| 8 | `SPAWN_PET(98/0x62)` 旧「❌ 发不出」 | ✅ **有发送点** | `IO/UITypes/UIItemInventory.cpp:830` `SpawnPetPacket(static_cast<int8_t>(slot), lead).dispatch();`；`Net/Packets/PetPackets.h:19` |
| 9 | `BUDDYLIST_MODIFY→BUDDYLIST(130)` 旧「🟡 好友增删未接线」 | ✅ **有发送点** | `Net/Packets/BuddyPackets.h:32,43,53` + `IO/UITypes/UIBuddyList.cpp:94,189,198,219,227,237,245,256,264,275,283`（增删/接受逐字命中）；`IO/UITypes/UICharInfo.cpp:381`；`IO/UITypes/UIChatBar.cpp:1995`；`IO/UITypes/UIMiscShells.cpp:5339` |
| 10 | `PARTY_SEARCH_REGISTER(0xDC/220)` 旧「已代理消化」 | ❌ **完全无**（真实缺口） | `Net/OutPacket.h` 无 220（仅 `PARTY_SEARCH_START=222@:222`/`UPDATE=223@:223`）；`IO/UITypes/UIPartySearch.cpp:246` 「s9 空 stub → Start 代理」，真发包 `:133/:146` |
| 11 | `USE_REMOTE(116/0x74)` 旧「❌ 抽一次发不出」 | ✅ **有发送点** | `Net/Packets/ItemAClassPackets.h:61-65` + `IO/UITypes/UIRemoteGachapon.cpp:83,99` `UseRemoteGachaponPacket(...).dispatch();`；入口 `IO/UITypes/UIItemInventory.cpp:834,887` |
| 12 | `GUEST_LOGIN(0x02)` 旧「⊘不做」 | ✅ **完全无**（维持裁决） | 全仓 `*.cpp/*.h` 仅 `gap-inventory.md:45` 命中；`tools/probe/opcode_blocklist.txt:18` |
| 13 | `WEDDING_TALK_MORE(0x8B)` 旧「⊘不做」 | ✅ 有等价包 `WEDDING_TALK=139` | `Net/Packets/WeddingPackets.h:210` + `IO/UITypes/UIWedding.cpp:281`；`Net/OutPacket.h:317` |
| 14 | `MTS_OPERATION(0xFD)` 旧「⊘不做」 | ✅ **c2s 完全无**（真实缺口） | c2s 无枚举（仅 `ENTER_MTS=156@Net/OutPacket.h:291`）；`Net/PacketSwitch.cpp:325` 348 是 s2c，emplace@:675，`Net/Handlers/WaveAShellHandlers.cpp:3551` 仅 LOG |
| 15 | `CUSTOM_PACKET(0x3713)` 旧「⊘不做」 | ✅ **完全无**（维持裁决） | 客户端 C++ 源码零命中；扩展收包仅 `DPT_TRACKER=0x3714@Net/PacketSwitch.cpp:327`、`LIMIT_BREAK_SYNC=0x3730@:414` |

`明确不做` 类另有 `REGISTER_PIN/STRANGE_DATA/ADMIN_CHAT/ADMIN_LOG/WORLD_TRANSFER/NAME_TRANSFER/VIEW_ALL_PIC_REGISTER/CASHSHOP_SURPRISE` 全仓零命中〔观察到〕；`RELOG=0x1C` 仅 `Net/OutPacket.h:42` 注释「已裁决明确不做，不占枚举位」。

> **主 agent 逐字复核（2026-11-08）**：`UIBuffList.cpp:246`、`UIStatsInfo.cpp:752`、`UIItemInventory.cpp:801,824,830`、`UIBuddyList.cpp:94-283`、`UIRemoteGachapon.cpp:83,99`、`UIMiscShells.cpp:6857`(PetCommand)/`:6870`(PetChat) 的 `.dispatch()` 全部命中；`Net/OutPacket.h` 内 `FIELD_DAMAGE_MOB/MOB_DAMAGE_MOB/MONSTER_BOMB/LEFT_KNOCKBACK/MOB_BANISH_PLAYER/SERVERLIST_REREQUEST/AFTER_LOGIN/MTS_OPERATION/PARTY_SEARCH_REGISTER` 零命中（仅 `TOUCHING_REACTOR=206@:177` 有枚举）。推翻成立。

#### 9.5.3 协议层真缺口 TOP 10（覆写 §9.2/§9.3/§16 的旧优先级）

| # | 缺口 | 判定 | 证据 |
| --- | --- | --- | --- |
| 1 | `FIELD_DAMAGE_MOB(0xBF)/MOB_DAMAGE_MOB_FRIENDLY(0xC0)/MONSTER_BOMB(0xC1)/MOB_DAMAGE_MOB(0xC2)/LEFT_KNOCKBACK(0xD4)` c2s **完全无**（无枚举无发包） | ❌ 真缺·两者都无 | 全仓 `*.cpp/*.h` 零命中；仅 s2c `LEFT_KNOCK_BACK` 有 handler `Net/PacketSwitch.cpp:502`；`gap-inventory.md:99-102,106` |
| 2 | `MOB_BANISH_PLAYER(0x38)/SERVERLIST_REREQUEST(0x04)/AFTER_LOGIN(0x09)/RELOG(0x1C)` c2s **完全无** | ❌ 真缺 | 全仓零命中；RELOG 仅 `Net/OutPacket.h:42` 注释 |
| 3 | `PARTY_SEARCH_REGISTER(0xDC/220)` 客户端无枚举无发包，UI 用 `PARTY_SEARCH_START` 代理 | ❌ 真缺 | `IO/UITypes/UIPartySearch.cpp:246` |
| 4 | `MTS_OPERATION(0xFD)` c2s 完全无；仅 `ENTER_MTS=156`；s2c handler 仅 LOG | ❌ 真缺·**仅 UI 有** | `Net/OutPacket.h:291`；`Net/PacketSwitch.cpp:325,675`；`WaveAShellHandlers.cpp:3551` |
| 5 | `LOGIN_EMAIL(3)` 是 **CLIENT_ONLY**（s9 无 handler） | 🟡 客户端独有 | `opcode-matrix.json`（`status=CLIENT_ONLY`） |
| 6 | `RECALCULATE_STATS`(recv 35) 与 `FORCED_STAT_RESET`(35) 撞值，注册注销致 `RecalculateStatsHandler` 死代码 | 🟡 一致性债 | `Net/PacketSwitch.cpp:735-740` + `Net/Handlers/PlayerHandlers.cpp:275`；`opcode-matrix.json:3-5,10-16` |
| 7 | `GENDER_DONE(4)/CONFIRM_EULA_RESULT(5)` 事实死注册（s9 不下发） | 🟡 死注册 | `Net/Handlers/LoginHandlers.cpp:459,476`；`Net/PacketSwitch.cpp:58-60,436-437` |
| 8 | `WEEK_EVENT_MESSAGE(77)` 降级为 yellow tip、`UIEvent` 16 条硬编码无数据通路 | 🟡 功能降级 | `Net/Handlers/MessagingHandlers.cpp:506-510`；`Net/PacketSwitch.cpp:749` |
| 9 | `WEDDING_CEREMONY_END(321)/MONSTER_CARNIVAL_LEAVE(295)/MTS_OPERATION(348)/MTS_OPERATION2(347)/FIELD_EFFECT(138)` 收包仅 deactivate/LOG/return | 🟡 收包空实现 | `Net/Handlers/WaveAShellHandlers.cpp:507,710,3551,3565`、`Net/Handlers/TestingHandlers.cpp:186` |
| 10 | **文档漂移**：`gap-inventory.md` 基于 `client_out_sent=103`、`opcode_blocklist.txt` 89 条按旧态生成，实测 148 sent；31+20 条「未接线」多数已接（宠物/好友/组队/家族/婚礼/远程扭蛋/道具分派）；计数与实列不一致（`:118` 标 20 实列 21、`:119` 标 19 实列 18） | 🟡 需重跑门禁 | `tools/gen_opcode_matrix.py --check` |

#### 9.5.4 UI ↔ 发包路径三态（类 E / D 的准确口径）

| 功能族 | UI 有 | 发包有 | 判定态 | 逐字证据 |
| --- | --- | --- | --- | --- |
| 宠物 Pet | ✓（无独立 `UIPet.h`，内嵌） | ✓ | **两者都有** | UI `IO/UITypes/UIMiscShells.cpp:6857,6870,6897,6908`、`IO/UITypes/UIItemInventory.cpp:828-831`；包 `Net/Packets/PetPackets.h:19,34,62,77,93,108,123,139`；`Gameplay/Stage.cpp:491`、`Character/Player.cpp:203` |
| 好友 Buddy | ✓ | ✓ | **两者都有** | `IO/UITypes/UIBuddyList.cpp:94-307`；`Net/Packets/BuddyPackets.h:32,43,53` |
| 组队搜索 PartySearch | ✓ | ✓（REGISTER 缺） | 两者都有·部分缺口 | `IO/UITypes/UIPartySearch.cpp:133,146`；`Net/Packets/PartySearchPackets.h:15,28`；`PARTY_SEARCH_REGISTER(220)` 完全无 |
| 家族 Family / 学院 | ✓（同一功能） | ✓ | 两者都有 | `IO/UITypes/UIFamily.cpp:2,195-386`、`UIFamilyTree.cpp:130-228`；`Net/Packets/FamilyPackets.h`(9)；`IO/UIStateGame.cpp:298` |
| Messenger | ✓ | ✓ | 两者都有 | `IO/UITypes/UIMessenger.cpp:185,215,279,409,435,447`；`Net/Packets/MessengerPackets.h:16,27,37,48,59` |
| 婚礼 Wedding | ✓ | ✓ | 两者都有 | `IO/UITypes/UIWedding.cpp:242,248,271,281`；`Net/Packets/WeddingPackets.h`(11，:210 WEDDING_TALK) |
| MonsterBook / 现金礼物 / 个人商店 / 小游戏 Omok·MatchCard | ✓ | ✓ | 两者都有（小游戏弱接线） | `UIMonsterBook.cpp:136,151,160`；`UICashShop.cpp:390,413,703,721`；`UIMerchant.cpp:96-227`、`UIMinigame.h:2`「via PLAYER_INTERACTION — wire later」 |
| **MTS / ITC 交易** | ✓ | ✗（c2s） | **仅 UI 有** | `IO/UITypes/UIMTS.cpp:149,155,168`（仅 ENTER_MTS/OWL）；`MTS_OPERATION(0xFD)` c2s 完全无 |
| 战斗内部包（MOVE_MONSTER/DAMAGE_REACTOR/PLAYER_UPDATE/PONG） | ✗ | ✓ | **仅发包有**〔推断：无 UI 概念，自动触发〕 | `Net/Packets/GameplayPackets.h:235,274,298`；`Net/Handlers/SetFieldHandlers.cpp:335`；`Net/Handlers/CommonHandlers.cpp:36` |
| 场地伤害类（FIELD_DAMAGE_MOB 等 9 项） | ✗ | ✗ | **两者都无**（§9.5.3 #1） | 全仓 `*.cpp/*.h` 零命中 |

> 反例（避免误判）：`PING(17)/PingHandler` 空参但发 `PongPacket().dispatch()`——**功能性，非 stub**（`Net/Handlers/CommonHandlers.cpp:34,36`）。`Net/Handlers/WaveAShellHandlers.cpp` 有 168 个 `XHandler::handle(InPacket&)`（:182-4780），仅 4 个空参变体，绝大多数为解析+UI 更新〔观察到〕。

#### 9.5.5 对上文各节的覆写结论

- **§7 类 E**：E1/E2/E4/E5/E6/E10/E11/E12/E15/E16/E17/E18 的「❌ c2s 发不出 / 需先补 UI」判定**作废** —— 这些 opcode 现在都有 `.dispatch()` 发送点。类 E 的真缺口收窄为：**E19 亲密/饱食位图条（无 gage 资源）、E20 场标→真精灵、宠物装备三页穿戴（`IO/UITypes/UIEquipInventory.cpp:622` BtPet1/2/3 预留）、`PARTY_SEARCH_REGISTER` 缺**。即「**协议已发、玩法深度/真渲染/UI 分支未闭环**」，而非「包都发不出」。
- **§9.2/§9.3**：`TOUCH_MONSTER_ATTACK / CANCEL_ITEM_EFFECT / AUTO_DISTRIBUTE_AP / CHANGE_QUICKSLOT / SET_HPMPALERT / SKILL_EFFECT / PET_FOOD / SPAWN_PET / BUDDYLIST / USE_REMOTE` 全部改为 ✅ 有发送点；`PARTY_SEARCH_REGISTER / MTS_OPERATION / 场地伤害族 / AFTER_LOGIN / SERVERLIST_REREQUEST / MOB_BANISH_PLAYER / RELOG` 才是真「完全无」。
- **§14.1**：⊘ 裁决本身不变（这些 opcode 客户端本就零命中），但 `0x008B WEDDING_TALK_MORE` 有等价 `WEDDING_TALK=139` 落地、`0x0089/0x008A` 已接，见 §2.4。
- **§16 TOP20**：#1「宠物族 c2s 发送」降级为「玩法深度」；#3 回城卷、#4 技能书、#5 一键 AP、#6 Buff 解除、#10 道具 UI 分支需按 §9.5.2 重判（均有发送点，缺的是 UI 分支/真机验收）。

---

## 10. 类 H · 伤害与数值

| # | 具体功能 | 状态 | 缺口形态 | 证据 |
| --- | --- | --- | --- | --- |
| H1 | 原生 `CalcDamage::PDamage/MDamage` 逐字段还原 | ✅ 已做但简化 | 约 10KB/2.5KB 复杂函数 Hex-Rays 重建文本未逐字段还原；v2 公式是「证据驱动的近似补齐」，玩家受伤仍沿用 Fyms085 近似式 | `docs/features/damage-calc-cap-parity-2026-11/README.md:24,28` |
| H2 | **`DamageRank` / `DPT_TRACKER(0x3714)` 个人伤害统计（#31）** | ❌ | 客户端无 handler、包结构未取证 ⚠ 但 `Net/PacketSwitch.cpp:677` 已注册 `DptTrackerHandler` → README 过时/口径差异，需现场核 | `docs/features/damage-calc-cap-parity-2026-11/README.md:30`；`parity-gap-2026-11/README.md:147` |
| H3 | **团队 Buff 显示（#30，`0x3713`）** | ❌ | 通道未启 + 明确不做 `PartyBuffs` 面板（见 §2.5 撞值） | `parity-gap-2026-11/README.md:146`；`docs/features/buff-timer/README.md:14`；`01-parity-matrix-and-plan.md:150` |
| H4 | **伤害皮肤 `DamageSkin`（#18）** | ❌ 运行时资源缺失 | `UI.nx/UIWindow.img` 无 `DamageSkin` 节点、`Effect.nx/BasicEff.img` 无 `damageSkin` 节点；`NoRed*/NoCri*/NoViolet*` 已守卫；`UICharInfo.cpp:452-460` 只保留「按现金道具 id 发送使用包」调试入口 | `parity-gap-2026-11/README.md:122,199`；`damage-calc-cap-parity-2026-11/README.md:29`；`IO/UITypes/UICharInfo.cpp:452-470` |
| H5 | **300 级 / 长伤害数值链路** | ❌ 且需双端 | `Character/ExpTable.h:26 LEVELCAP=201`、`` Gameplay/Combat/DamageNumber.h:40 `int32_t damage` ``；服务端未同步开放上限则只能显示兼容 | `parity-gap-2026-11/README.md:119,120,188` |
| H6 | 破功值 DB 持久化 | ⊘ 明确不做 | 无授权 DDL/DML，内存态默认 2^53；`s9 SendOpcode` 无 `LIMIT_BREAK_SYNC` | `damage-calc-cap-parity-2026-11/README.md:32`；`docs/features/window-title-xiaoye/README.md:119` |
| H7 | `DAMAGE_BONUS` / `FINAL_DAMAGE` 高版本属性 | ⊘ 明确不做 | 用户明确不纳入（决议：`damage-calc-cap-parity-2026-11/README.md:5`）；`CRITICAL_DAMAGE` 已改真实 `mincrit` | `damage-calc-cap-parity-2026-11/README.md:31` |
| H8 | `summonAttack` / `PacketCreator.damageMonster` HP 弹字 / 梅索爆炸 C2S 专用分支 | 🟡 按原版式保留 | 插件文档 §15 leftovers | `damage-calc-cap-parity-2026-11/README.md:33` |
| H9 | 金币超上限显示格式化 / 上限校验（#6） | 🟡 | 读取侧已 `int64_t`（`Configuration`/`01-matrix:40`），显示格式化/上限校验未收口 | `parity-gap-2026-11/README.md:133`；`01-parity-matrix-and-plan.md:40` |
| H10 | 血量突破上限（#7） | ❌ | 无关键字命中 | `parity-gap-2026-11/README.md:121,201` |
| H11 | 已闭环的三项（供排除）：Buff 倒计时文本 `ui.buff_timer`、装备悬停对比 `ui.equip_compare`、属性详情 `UIStatsInfo` 已扩 9 行 | ✅ | `Configuration.cpp:140-141,236,870,894-896`；`IO/UITypes/UIBuffList.cpp:160`；`IO/Components/EquipTooltip.cpp:85` | `01-parity-matrix-and-plan.md:55-73` |

---

## 11. 类 I · UI 资源与贴图

| # | 具体功能 | 状态 | 缺口形态 | 证据 |
| --- | --- | --- | --- | --- |
| I1 | `UIWindow.img` 112 顶层节点 vs 78 窗类 + 58 壳窗 → 真缺 ~5 | 🟡 | 两端 `Data/UI/*.img` SHA256 逐字节全同 → 窗口缺失不是资源问题 | `parity-gap-2026-11/README.md:13,95`；`r00:13-16` |
| I2 | G8 Mob 出生特效 `SpecialEff.img` / G9 怪物 debuff 图标 | 🟡 参照端资源同缺 | `catalog_Mob.json` 2057 文件零命中 `SpecialEff.img`；`Mob.img` 无 status/level 节点 | `docs/features/map-object-audit/README.md:43-44` |
| I3 | G4 雾区旧口径 | ✅ 已改 | 已改接 `Skill/.../tile` 平铺，`docs/features/mist-area-render/README.md:23` 证旧预留过时 | `docs/features/map-object-audit/README.md:63-73` |
| I4 | 商店盒 / 风筝 / 聊天气泡壳贴图 | 🟡 预留 | 波1/2/3 已闭、隐藏门三态/雾区/idle/G11/风筝 iconReward 后续闭合；余商店盒按证据边界预留；聊天气泡壳本版宋体 A11M 白字 | `docs/features/map-object-parity/README.md:3,84,102`；`docs/features/kite-texture/README.md:4,29` |
| I5 | **扩展装备栏（辅助装备 / 神秘徽章，#38）** | ❌ 仅 UI 稿 | `docs/features/addon-equipment/ui/v5`、`mystic_badge` 切图归档；无窗类/无槽位/无服务端结构；`docs/features/mystic_badge/README.md:40` 角部装饰为简化卷草纹 | `parity-gap-2026-11/README.md:154,185,188` |
| I6 | 每日签到 v4 皮肤件（见 D8） | 🟡 素材已备未换 | — | `parity-gap-2026-11/README.md:212-214` |
| I7 | minimap tip 的 Mob/Npc 图标、`BtSmall/BtBig/BtNpc` | 🟡 显式预留 | 包内确无，保持空纹理跳过 | `docs/features/minimap-mode-switch/README.md:25,48,102,124` |

---

## 12. 类 J · 地图对象与场标

| # | 具体功能 | 状态 | 缺口形态 | 证据 |
| --- | --- | --- | --- | --- |
| J1 | `Gameplay/Stage.h:80` `// Field placeholders (summon/mist/door/kite/dragon) until full sprites` | 🟡 | 场标 → 真对象部分已修（G1/G2/G3/G5/G6/G7/G10/G11），余者为文本 | `Gameplay/Stage.h:80`；`Gameplay/MapleMap/FieldMarker.h:2`；`MapFieldMarkers.h:2`；`MapSummon.cpp:20` |
| J2 | Mob 情报浮窗 / Boss 分阶段专属逻辑 / `activates_by_touch`·`reactInBox` | ⊘ 明确不做·显式预留 | G8 资源缺源、s9 客户端仅 HP 条+tagColor、服务端裁决 | `docs/features/map-object-audit/README.md:48` |
| J3 | 真机冒烟 6 项：Evan 带龙 / 神秘门 / 双人门 / 多 debuff 场标 / 宠物拾取 / 宝箱乱序 | ⏳ 需用户 | 交付判据留用户 | `docs/features/map-object-audit/README.md:55,61` |
| J4 | 隐藏门三态动画 | ⏳ | 代码销案、动画待真机 | `pending-live-tests/README.md:37`；`docs/features/hidden-portal-tristate/README.md:26` |
| J5 | G11 场标贴图 / 风筝 iconReward / G10 金银宝箱 | ⏳ | 需对应 s2c 触发场景 | `pending-live-tests/README.md:42-44,409-416` |

---

## 13. 类 K · 平台 / 工程 / 构建（扩展矩阵线 B 与工程面合并）

> **本节属「两套口径」中的 B 线 + 工程债**：`r14:247,274` 判 ijl15 全部扩展 = 本线 ⊘明确不做；`parity-gap-2026-11/README.md:113` 又纳入现场复核。故所有 # 项标「用户可选立项」，不是 v83 原版缺口。

| # | 具体功能 | 状态 | 证据 |
| --- | --- | --- | --- |
| K1 | 商店/仓库/玩家商店列表加长 #3/4/5 | 🟡 | `parity-gap-2026-11/README.md:132` |
| K2 | 金币超上限 #6 / 血量突破上限 #7 | ❌/🟡 | `:121,133` |
| K3 | 解锁 300 级 #8 / 长伤害显示 #12 | ❌ | `:119,120` |
| K4 | 角色槽位扩展 #9 | ❌ | `:134` |
| K5 | 潜能卷轴拖拽 #11 / 侧边栏工具 #13 / AllowCashTrade #17 | ❌ | `:136,137,139,201`（全仓 grep 0 命中） |
| K6 | 新角色骰子 #15 | ✅ 假缺口·已通 | `:198-199` |
| K7 | RS 分辨率同步 #16 | 🟡 | `:138` |
| K8 | 伤害皮肤 #18 / 每日签到 #19 | ❌ | `:122,123,199,202` |
| K9 | 美容院 #20 / 七彩棱镜染色 #21 / 魔方结果窗 #23 / 背包格子锁定 #24 / 骑宠预览 #27 / 弹跳技能 #28 | ❌ | `:124-130,201`（`Prism`/`Cube`/`LockSlot`/`MountPreview`/`BounceSkill` 全仓 0 命中） |
| K10 | 天气系统 #26（s2c `BLOW_WEATHER` 已接、UIEvent 有雨；客户端主导自定义天气 `0x373D/373F` 未做） | 🟡 | `:128`；`Net/PacketSwitch.cpp:223,514` |
| K11 | 套装系统 #29 / 团队 Buff #30 / 个人伤害统计 #31 / 他人装备详情 #32 `0x3726/27` / 装备成长 Tip #33 / 融合外观注能 #34 / 收纳背包 #35 `0x3724/25` | ❌ | `:145-151` |
| K12 | 怪物图鉴增强 #36 | 🟡 | `:152` |
| K13 | 窗口现金商城 #37（`0x3730 CASHSHOP_WINDOW_ACTION` vs `LIMIT_BREAK_SYNC` 撞值，→0x3740 规划未执行） | ❌ | `:153`；`01-parity-matrix-and-plan.md:26,149-164` |
| K14 | 扩展装备栏 #38 | ❌ 仅 UI 稿 | `:154` |
| K15 | 抽奖 XyLottery #39 / 交易所 trade-house-083 | ❌ | `:155,156` |
| K16 | 家族三系创角 / 高清每日礼物 / 地图信息·搜索面板 | ✅ 已闭环 | `parity-gap-2026-11/README.md:157-160` |
| K17 | Android arm64 端到端真机点测 | 🟡 | `parity-gap-2026-11/README.md:169` |
| K18 | Linux / iOS | ⬜ 明示「随后」未启动 | `:170` |
| K19 | i18n pending-copy / 联机冒烟清单逐项勾绿 / `B11 _ui_asset_diag` | 🟡 | `:172,173,175` |
| K20 | 工作树 214 文件未提交（并发在途 hunk 未原子提交） | ⚠ 工程债（非功能缺失） | `:174,186` |
| K21 | 1440×900 档切档约 6 秒崩 `0xC0000374`（堆损坏） | ✅ 已修（V1.3/V1.5/V1.6 未跑） | `docs/features/multi-resolution/README.md:109-115,141`；`parity-gap-2026-11/README.md:111` |
| K22 | 启动提速 / 按需根装载 / prewarm 二阶段 | 🟡 预留未做 | `docs/features/perf-instrument-2026-11/README.md:191,233,265,272` |
| K23 | 本地完整 VS 编译验收（`MapleStory.sln`/vcxproj） | ⏳ | `docs/features/daily-gift-hd/README.md:81`；`docs/features/equip-compare/README.md:82` |
| K24 | 不新增 CMake（本仓库用 VS/MSBuild `MapleStory.sln`） | ⊘ 工程选型 | `docs/features/login-layout-1024-fix/README.md:15,57` |
| K25 | 真机联调阻塞：8484 端口未监听、s9 未启动（需用户 Java 21+MySQL） | ⏳ | `damage-calc-cap-parity-2026-11/README.md:113`；`multi-resolution/README.md:80` |

---

## 14. ⊘ 明确不做 / 显式预留 总表（含理由，避免重复立项）

### 14.1 协议裁决（`gap-inventory.md:41-119`「明确不做 19 条」）

| opcode | 名 | 理由 |
| --- | --- | --- |
| `0x0002` | `GUEST_LOGIN` | 本项目用账号体系 |
| `0x000A` | `REGISTER_PIN` | s9 `enable_pin=false` |
| `0x0010` | `NAME_TRANSFER` | 写库、v83 私服小众 |
| `0x0012` | `WORLD_TRANSFER` | 写库 |
| `0x001B` | `STRANGE_DATA` | handler 即空操作 |
| `0x001F`/`0x0020` | `VIEW_ALL_PIC_REGISTER` / `VIEW_ALL_WITH_PIC` | PIC 关闭、无注册流程（s9 `RegisterPicHandler` 不支持修改）；`r10:137-141` |
| `0x0076`/`0x0081` | `ADMIN_CHAT` / `ADMIN_LOG` | GM 专属；客户端仅 `ADMIN_COMMAND=0x80` 已发 |
| `0x008B` | `WEDDING_TALK_MORE` | 撞值 0x8B、同 WEDDING 链（注：婚礼 137/138/139 已改判 ✅ 闭环） |
| `0x00A1` | `CASHSHOP_SURPRISE` | 现金商店内容 |
| `0x00D3`/`0x00D5`/`0x00DA` | `SNOWBALL` / `COCONUT` / `MONSTER_CARNIVAL` | 活动小游戏/内容 |
| `0x00FD` | `MTS_OPERATION` | s9 无拍卖后端 |
| `0x3713` | `CUSTOM_PACKET` | 自定义/调试通道（另见 §2.5 撞值） |

> `gap-inventory.md:119` 汇总误把 `0x0089/0x008A` 列入，同文件 `:81-82` 已改判「2026-11-06 已接」，取已接。
> ⚠️ **§9.5 补充复核**：⊘ 裁决本身不变（这些 opcode 客户端 C++ 源码本就零命中，`GUEST_LOGIN/REGISTER_PIN/STRANGE_DATA/ADMIN_CHAT/ADMIN_LOG/WORLD_TRANSFER/NAME_TRANSFER/VIEW_ALL_PIC_REGISTER/CASHSHOP_SURPRISE/CUSTOM_PACKET` 全仓零命中，见 §9.5.2 #12/#15）。但两点须校正：① `0x008B WEDDING_TALK_MORE` 有等价 `WEDDING_TALK=139` 已落地（`Net/Packets/WeddingPackets.h:210` + `IO/UITypes/UIWedding.cpp:281`，§9.5.2 #13），婚礼链实为 ✅ 闭环；② `0x00FD MTS_OPERATION` 列为 ⊘ 的同时也是 §9.5.3 #4 的「❌ 真缺·仅 UI 有」（c2s 完全无、仅 `ENTER_MTS=156`、s2c handler 仅 LOG）——若将来立项 MTS/ITC 交易窗，这是必须先补的 c2s 包。另 `0x3713` 三处命名撞值见 §2.5，立项前核 `protocol/opcode-matrix.json`。

### 14.2 基准裁决（083 原版边界）

| 条目 | 理由 | 证据 |
| --- | --- | --- |
| 韩系导师 `Academy`（师生/学分） | **只在 095 线符号出现**；083 `UIWindow.img` 顶层 0 命中、s9 零命中，做成即偏离 GMS083 基准的加戏。v83 中文所称「学院」= 家族（action 26 FAMILY），`UIFamily` 已闭环 | `social-loop-audit-2026-11/README.md:20,70-72`；`wedding-flow/README.md:134` |
| 远征独立招募面板 `BossParty` / `ExpeditionIntermediary` | 095 线证据；GMS113 `UserList` 页签实为「好友/组员/家族/家族联盟/拒绝列表」五页，无 BossParty 第 6 页；083 原版本就是队伍+NPC+事件流程，玩法本端已可参与 | `social-loop-audit-2026-11/README.md:22,73-75` |
| 095 PLUS2 / 怀旧岛192 / 浩浩岛 系资源 | 不作本线参照与入库源（一律取 083 适配来源） | `social-loop-audit-2026-11/README.md:76-78` |
| `ijl15.dll` 与 `E:\MXD\自定义功能实现` 全部扩展 | 本线 v83 原生 parity 范围（另见 §2.1 口径张力） | `r14:247,274`；`01-parity-matrix-and-plan.md:22` |

### 14.3 功能/资源/工程裁决

| 条目 | 理由 | 证据 |
| --- | --- | --- |
| 团队 Buff 面板 `PartyBuffs`、buff 计时/续期、参考端「影子画布」缓存、`nID==5221006/35001002` 白名单 | 明确本版不改 | `docs/features/buff-timer/README.md:14,57` |
| 装备对比多部位/双槽饰品/现金 bodyPart 组合、SetItem/EquipGrowth tooltip 联动、装备属性计算与服务端 | 明确不做 | `equip-compare/README.md:16` |
| 世界地图：服务端封包拉表、改 MapTooltip 悬停、搜索栏以外玩法 | 明确不做 | `docs/features/map-mob-drops/README.md:17` |
| 不移植高版本 `RaceSelect_new` 资源包、不改服务端建角协议字段 | 北斗 UI.nx 无高版本节点导致越界闪退 | `create-char-classic-dual-path/README.md:14` |
| 地图锚点登录页（`UILogin`/`UIWorldSelect`/`UICharSelect`）、协议/封包 | 明确不做 | `docs/features/login-modal-viewport-center/README.md:14` |
| 不新增 CMake、不引入新 world-transform 方案、不做全局 transform/DPI 缩放 | 工程选型 | `login-layout-1024-fix/README.md:15`；`multi-resolution/README.md:53` |
| `forbid-fall-down` step4 外观面板 / 真机点测 / 服务端改动 | 本轮用户指定不真机；s9 无该字段逻辑 | `docs/features/forbid-fall-down/README.md:13` |
| 掉落物归属（暴怒炸金币）、外部 `Skill.wz` 资产、`UIComboCounter` 美术 | 非目标 | `docs/features/job-families/README.md:22` |
| 每日礼物 A1 服务端闭环、F 逻辑 | 仅 UI | `daily-gift-hd/README.md:16,21` |
| `resource-backend-abstraction` 非目标 | 本版不做 | `docs/features/resource-backend-abstraction/README.md:24` |
| Mob 情报浮窗 / Boss 分阶段 / `activates_by_touch`·`reactInBox` | 资源缺源 / 服务端裁决 | `map-object-audit/README.md:48` |
| 破功值 DB 持久化、`DAMAGE_BONUS`/`FINAL_DAMAGE` | 无授权 DDL/DML / 用户明确不纳入 | `damage-calc-cap-parity-2026-11/README.md:31,32` |
| `GameOpt` D3 多选原选项 / 底栏空壳入口 / 两窗居中重锚 | 本版不做，列显式预留 | `multi-resolution/README.md:20,82` |
| Avatar try-on 独立 opcode | 搁置 | `r21-remaining-backlog.md:17-19` |
| rank/jobrank/burning/timestamp 选角 UI | 未证实不臆造 UI | `r10:101-111` |
| 场景变更 / 任务窗「寻找 NPC 位置」 | 无任何皮肤 UI/协议线、无封包/IDA 证据，不发明客户端能力 | `open-issues.md:61-62,818-824` |

---

## 15. ⏳ 需用户实机 / 截图 / 服务端配置才能推进的挂起项

主源 `docs/features/pending-live-tests/README.md:12-45`（2026-11-05 17:03 解除「不做桌面点测」，已完成一轮自动驱动真机验收；未能自动触发的仍需手测）。# 号沿用该表。

### 15.1 四类全局阻塞

| 阻塞 | 影响项 | 证据 |
| --- | --- | --- |
| **A** GM 命令自动注入失败：`!` 与空格被 `IO/UI.cpp:264-269` 文本映射吞掉；SendKeys/Ctrl+V/Unicode SendInput 三种方式均未产 `0x31` 命令包（只有普通聊天 `level30` / 动作包 `2c 00`） | #2/#9/#10/#11 + 手雷 `0x6D`/一键 AP `0x58`/Buff 取消/书袋双击 + #15/#16 | `pending-live-tests/README.md:66` |
| **A′** 原 harness 遗留 4 项前置全缺（角色 lv1/job0/ap0、背包 0 件、HP/MP 满、无 BODY_PRESSURE/疾病 buff/回城卷/技能书/召唤袋） | 需 GM 改号或换有前置的角色（写库未获授权，未做） | `:102,106-110` |
| **B** 需第二角色/公会/自由市场/商城便签额度 | #4/#5/#5b/#6/#8/#12/#13/#14 | `:67` |
| **C** `game_config.use_server_auto_pot=false` | #7 回灌（管理端 8686 `/config/{latest}/updateConfig`，改后如需重启 s9 由用户执行） | `:68` |
| **D** 并行会话高频改同一仓库/重建 exe/结束客户端进程 | 所有需长稳定会话的项 | `:69` |

### 15.2 逐 # 挂起

| # | 项 | 状态 | 阻塞/需用户动作 |
| --- | --- | --- | --- |
| 1 | WORLD_MAP_PLAYERS | ✅ 2026-11-05 | 已真机闭环（供排除） |
| 2 | USE_RETURN_SCROLL 回城卷双击 | ⏳ | 需 `2030000..2030099` 道具 |
| 3 | 登录遮罩四页截图叠图 | ⏳ | 关闭自动登录逐页按 SCROLL_LOCK |
| 4 | PersonalShop CREATE 自由市场 | ⏳ | 需 FM 房间 + 许可证 |
| 5/5b/5c | Messenger 立绘 / blink·气泡 / 关闭钮 | ✅/🟡 | 立绘与关闭钮已闭环；#5b 需双角色同房手点补拍 |
| 6/#12 | Guild BBS 摘要 + 完整窗 | ⏳ | 需已入公会，逐项点 |
| 7 | SET_HPMPALERT 端到端回灌 | ⏳ | 阻塞 C |
| 8 | PICK_ALL_CHAR（c2s 14/32 非 19） | ⏳ | 实机点选 BtEnter |
| 9/10/11 | HEAL_OVER_TIME / TOUCH_MONSTER_ATTACK / CANCEL_DEBUFF | ⏳ | 需掉血/BODY_PRESSURE/疾病 buff |
| 13 | 公会 Boss HUD + Healer/Pulley 物件 | ⏳ | s9 脚本当前不发 287/288，需 GM/临时脚本 |
| 14 | CharInfo SoftKey 便签 `NOTE_ACTION(131)` | ⏳ | 需商城便签额度 |
| 15 | SKILL_EFFECT 起手广播 | ⏳ | 无 KeyDown 条时只发起手一包，松开取消未接 |
| 16/17 | NPC_ACTION 对话姿态 / CLICK_GUIDE | ⏳ | 2026-11-05 已接线，待真机 |
| 18 | 隐藏门三态动画 | ⏳ | 代码销案、动画待真机 |
| 19/22/23/24/25 | NPC 对话 A11b+A11c / tip 817–821+Pet inset / ScriptMessage+tip 820 / AskAvatar / AskBoxText | ⏳ | 全部「已接线，待真机」；BoxText 多行局限 |
| 29/30/31 | G11 场标贴图 / 风筝 iconReward / G10 金银宝箱 | ⏳ | 需对应 s2c 触发场景 |
| 32 | 结婚全链 7 步 | ⏳ | exe 23:16 已部署，需双角色实机 |
| 33 | 联盟补完 invite/0x0D/公告钮 | ⏳ | 需双公会 |
| 34 | 组队/家族 R28 回归抓帧 | ⏳ | 冷启动登录链全程抓帧，五窗不得出现在未主动打开帧 |
| 35 | 五死键接线（F 学院/H 密语/G 公会/O 组队搜索/L 预期无反应） | ⏳ | exe 2026-11-07 00:02 已部署，需逐键按 |
| 续作 3 | MapleTV 双屏 / 点 NPC·角色·玩家 / 装备对比 | ⏳ | 自动脚本无换图、坐标未命中、测试号无可穿戴道具 |
| 截图键 | `{SCROLLLOCK}` 自动注入未产出 PNG | ⏳ | 判特殊键映射未触达，请手按 SCROLL_LOCK 复核 |
| 真机冒烟 | 社交五窗按 G 逐项点选 | ⏳ | 本环境历史抓屏全黑，不假装已验证 |
| 真人验收 6 | J7 四条伤害路径 / J6 连击 / J2 转职名表 | ⏳ | `docs/features/job-families/README.md:3,124` |
| 200+ 复验 | StatusBar/StatsInfo/UIQuit 经验条 | ⏳ | `level-cap-256-2026-11/README.md:39,42` |
| 逐档验证 | 多分辨率 V1.3/V1.5/V1.6 | ⏳ | 依赖 s9 在 8484 |
| 叠图确认 | step0 相机 X / step=1 背景 / Loading bar 偏移 | ⏳ | 本机渲染黑屏（基线 exe 同黑） |
| buff 文本 | 位置/切换/开关 | ⏳ | `docs/features/buff-timer/README.md:84` |
| 编译验收 | 本地 `MapleStory.sln`/vcxproj 完整 VS 编过 | ⏳ | `daily-gift-hd/README.md:81` |
| VIEW_ALL_CHAR 端到端 | 建角/PIC/查看全部角色 | ⏳ | 黑屏环境既有问题不修 |

---

## 16. TOP 20 优先级（排序=推断，条目状态=观察）

依据 `parity-gap-2026-11/README.md:180-188` 收敛顺序（P0→P3）+ `r15:147-156` P0 硬缺陷 + 玩法影响面。

> ⚠️ **§9.5 覆写（2026-11-08）**：原 TOP20 里 #1/#3/#4/#5/#6/#7/#10 是 `parity-gap-2026-11/README.md`(2026-11-04/05) 的「补发包」判定，已被协议层权威重算 + 源码逐字 `.dispatch()` 推翻。下表「§9.5 校正」列给出新判定；**原 ❌ 一律降级为「玩法深度 / 真机验收」，勿再按补发包立项**。P0 硬缺陷（#2/#9）与真正「完全无」的协议项（§9.5.3）优先级上升。

| # | 功能 | 原状态 | §9.5 校正（权威） | 出处 |
| --- | --- | --- | --- | --- |
| 1 | 宠物/召唤/龙整族 c2s 发送 + 命令 UI（类 E 全表） | ❌ 最大玩法面 | ✅ **c2s 已 148/148 全覆盖**；降级为「玩法深度 / 真渲染 / 宠物装备三页 / 场标精灵」→ 见 §7 E19–E22 | §9.5.2 / §9.5.4 / §7 |
| 2 | 右侧快捷 8 格点击（仅绘制、无 hit→send_key） | ❌ P0 硬缺陷 | ❌ **维持 P0**（UI 交互缺陷，与协议无关） | `r15:129-135,147`；`r16:148,152` |
| 3 | `USE_RETURN_SCROLL(85)` 回城卷发送路径 | ❌ | ✅ 有构造（`UseItemPacket` 动态分派，`InventoryPackets.h:135-151`）；剩真机双击验收 | §16 挂起 #2；`Net/Packets/InventoryPackets.h` |
| 4 | `USE_SKILL_BOOK(82)` 技能书消费 | ❌ | ✅ 有构造（同上）；剩 `UISkillBook` 浏览窗→消费分支 | §9.2；`Net/Packets/InventoryPackets.h:135-151` |
| 5 | `AUTO_DISTRIBUTE_AP(88)` 一键分配 AP | ❌ | ✅ **有发送点** `IO/UITypes/UIStatsInfo.cpp:752` `AutoAssignApPacket(...).dispatch();` | §9.5.2 #3 |
| 6 | `CANCEL_ITEM_EFFECT(73)` Buff 解除入口 | ❌ | ✅ **有发送点** `IO/UITypes/UIBuffList.cpp:246` `CancelItemEffectPacket(...).dispatch();` | §9.5.2 #2 |
| 7 | `SCRIPTED_ITEM(78)` 任务脚本一次性道具 | ❌ | ✅ 有构造（`UseItemPacket` 动态分派）；剩真机点选 | §9.2；`InventoryPackets.h:135-151` |
| 8 | `TemporaryStatView` 临时属性面板（可本地驱动、不等服务端） | ❌ 全缺 | ❌ **维持**（UI 全缺，与协议无关） | `:99,184` |
| 9 | 单频道世界被 `channel_count<2` 跳过 | ❌ P0 代码缺陷 | ❌ **维持 P0** `IO/UITypes/UIWorldSelect.cpp:697-698` | `r15:104,110-115` |
| 10 | `USE_CATCH_ITEM(81)`/`USE_DEATHITEM(53)`/`USE_SUMMON_BAG(75)` 三个道具 UI 分支 | ❌ | ✅ 三条均有 `.dispatch()` / 动态分派：`UIItemInventory.cpp:801`（`UseCatchItemPacket`）、`:824`（`UseDeathItemPacket`）、`InventoryPackets.h:135-151` | §9.5.2 #7/#16/#17；§7 E2/E10/E16/E17 |
| 11 | `CHANGE_MAP_SPECIAL(100)`/`USE_INNER_PORTAL(101)`/`TOUCHING_REACTOR(206)`/`USE_TREASUER_CHEST(115)`/`USE_ITEM_REWARD(112)` 隐藏点·反应器·宝箱交互 | ❌ 半通 | 🟡 有枚举 / 有构造，运行时 dispatch 未逐条证；剩真机 | `:52`；§9.2 |
| 12 | 公会副本 HUD（s2c 287/288 无 handler） | ❌ | ❌ **维持**（s2c 差集，与 c2s 无关） | `:20-26,101` |
| 13 | 中文 IME 输入（T13）+ 3 个 `UNHANDLED_EMITTED` | ⬜/❌ | 🟡 **IME 实际已开启**（`IO/Window.cpp:901-925` `SDL_StartTextInput`，`charset.md:193`「仍关 IME」为 2026-10-31 旧态）；真剩 `Textfield` 中文光标 + 显示链路同源受益面 + 3 个 UNHANDLED_EMITTED | `IO/Window.cpp:901-925`；`s9-protocol-align/notes/charset.md:145,230-236`；`s9-protocol-align/README.md:60` |
| 14 | `Title` 称号窗口（无数据源、需双端立项） | ❌ → 壳 | ❌ → 壳 **维持** | `:100,184` |
| 15 | 公会 BBS 完整度 + `UIGuildBBS` 多页签验收 | 🟡 | 🟡 **维持** | `:87` |
| 16 | 地图搜索逻辑/传送封包/结果绑定 | 🟡 壳 ✅ 逻辑缺 | 🟡 **维持** | `map-search-ui/README.md:74-76`；`:159` |
| 17 | 伤害皮肤 `DamageSkin`（运行时资源缺失）+ 背包格子锁定 #24 | ❌ | ❌ **维持**（资源层，与协议无关） | `:122,127,199` |
| 18 | 窗口现金商城 #37 + `0x3730→0x3740` 冲突规划 | ❌ | ❌ **维持**（扩展矩阵线 + opcode 撞值） | `:153`；`01-matrix:26` |
| 19 | 扩展装备栏 #38 + 每日签到双端协议 | ❌ | ❌ **维持**（双端立项） | `:154,210-228` |
| 20 | 交易所 `trade-house-083` + 怪物图鉴增强 #36 + 300 级/长伤害数值链路（均需双端） | ❌/🟡 | ❌/🟡 **维持** | `:119,120,152,156,188` |
| ★ | **协议层真「完全无」**（§9.5.3）：场地伤害族 `0xBF/0xC0/0xC1/0xC2/0xD4`、`0x04/0x09/0x38/0x1C`、`PARTY_SEARCH_REGISTER(220)`、`MTS_OPERATION(0xFD)`、`CUSTOM_PACKET(0x3713)` | — | ❌ **新增 P1**：这些才是真正需要「补枚举+补发包」的项；`RECALCULATE_STATS` 撞值死代码 / 5 个收包空实现 / 文档漂移为 🟡 一致性债 | §9.5.3 #1–#10 |

---

## 17. 与 Q1 的联动：Angel 083 带符号 IDB 可直接补的原生证据

Q1 已定稿 `docs/features/ida-v83-angel-instance/README.md`：`Angel.exe`(port 13339, pid 29620) = `E:\MXD\反编译IDA\v83\`Angel.i64`，是**同一代 v83 主程序的另一份带 MSVC 修饰名 IDB**（3122 修饰名 / 361 类 / 2612 方法 vs BeiDou 50 修饰名 / 10 类 / 0 业务方法），函数总数 55016 vs 55655、imagebase 均 `0x400000`、版本串逐字相同 → **符号详细 ≠ 功能更多**。

它对本清单的价值是「**给上面这些 ❌/🟡 缺口提供原生 C 层证据（控件表 / 包序 / 方法语义）**」。`docs/features/ida-v83-angel-instance/evidence/angel-gap-feature-ea.json`（`matched_classes 95 / total_named_funcs 809`）已把缺口类映射到可 `decompile` 的具体 EA：

| 本清单缺口 | Angel 类 | 可 decompile EA | 方法数 |
| --- | --- | --- | --- |
| C1 MTS / 拍卖壳 | `CUIMTS`/`CAdminShopDlg` | `CAdminShopDlg 0x6fb07b..`（见 JSON） | — |
| C16 公会 BBS 深度 | `CUIGuildBBS` | `0x8133a9..0x817369` | 17 |
| C19 NPC 对话分支 | `CUtilDlgEx` | `0x519e3a..0x9acc20` | 43 |
| D6 怪物图鉴 | `CUIMonsterBook` | `0x862527..0x867be6` | 5 |
| D12 Messenger 立绘 | `CUIMessenger` | `0x84db06..0x851d3b` | 26 |
| 学院（家族）术语校准 | `CUIFamily` / `CUIFamilyChart` | `0x80862b..0x8091d2` / `0x809ccc..0x80bb81` | 3 / 8 |
| 组队搜索 | `CUIPartySearch` | `0xa12073` (`StartSearch`) | 1 |
| 扭蛋 / 现金商城购买 | `CCashShop` / `CUICashItemGachapon` | `0x468f3e..0x47c75e` / `0x999e4d..0x99aa92` | 66 / 3 |
| 活动小游戏 | `CUIMonsterCarnival` / `CField_Wedding` | `0x86e047..0x8707c4` / `0x5814fd..0x581b78` | 16 / 4 |
| 宠物/召唤 | `CPet` / `CVecCtrlPet` / `CUIPetEquip` / `CSummoned` / `CDragon` | `0x703314..` / `0x9c406b..` / `0x800f7b..` / `0x7a3366..` / `0x4feb23..` | 28/6/2/17/10 |
| 任务/QuestAlarm | `CQuestMan` / `CUIQuestInfo` / `CUIQuestAlarm` / `CUIMedalQuestInfo` | `0x6cdd06..` / `0x87b921..` / `0x885df1..` / `0x84056b..` | 17/15/6/10 |
| 键位/宏/Tooltip | `CUIKeyConfig` / `CUIMacroSys` / `CUIToolTip` | `0x8335d0..` / `0x8a8504..` / `0x483eed..` | 7/8/26 |
| HUD 细节 | `CUIStatusBar` / `CUIEnergyBar` / `CUIPartyHP` / `CUISoftKeyboard` | `0x7f9bad..` / `0x91f934` / `0x832979` | 41/2/1/1 |
| 玩家列表五页 | `CUIUserList` / `CTabFriend` / `CFriend` | `0x919b73..` / `0x90952e..` / `0xa3ff8b..` | 10/12/6 |

> 使用约束（沿用 Q1 §5）：083 基准不变、禁止引 095 线符号；Angel 是另一份 v83 整合端（`C:\Users\Admin\Desktop\v83 Server\`Angel.exe`），控件 id/坐标/包序先在 Angel 定位再回 13337 交叉验证，冲突以 13337 + 实机 WZ/IMG 为准；Angel 字符串缓存仅 1281（BeiDou 3889），字符串锚点仍以 13337 为准；端口会变，每次先跑 `server_health` 读 `idb_path`。

---

## 18. 证据文件索引 / 复现 / 销案说明

### 18.1 主证据源（按权威顺序）

1. **`docs/features/s9-protocol-align/protocol/opcode-matrix.json`**（2026-11-08 协议层权威矩阵，`generated_at=2026-11-08T00:56:27`、s9 head `3d5ccc6f`，`tools/gen_opcode_matrix.py` 可复算）→ §9.5 唯一权威协议读数
2. `docs/features/parity-gap-2026-11/README.md`（228 行，2026-11-04/05 现场重算，**功能面/窗口面最权威**）
3. `docs/features/social-loop-audit-2026-11/README.md`（117 行，用户 2026-11-06 裁决「一律参照 GMS083 原版」）
4. `docs/features/pending-live-tests/README.md`（474 行，真机挂起主源）
5. `docs/features/beidou-v17-parity/notes/r15-baseline-ui-gap-vs-screenshots.md`（219 行，P0 硬缺陷）
6. `docs/features/beidou-v17-parity/notes/r16-rea-coverage-gap.md`（280 行，REA C 层证据债务，非功能缺失主源）
7. `docs/features/beidou-v17-parity/notes/r40-continuous-shell-coverage.md`（9 行）/ `r40-utildlg-branches-remaining-shells.md`（23 行），壳铺销案
8. `docs/features/damage-calc-cap-parity-2026-11/README.md`（40 行）、`docs/features/map-expansion/PLAN.md`（129 行）、`docs/features/login-flow-ui-ida-align/README.md`（180 行）、`docs/features/ui-v83-window-restore/notes/open-issues.md`（981 行）
9. 源码侧：`IO/UIElement.h`（151 枚举）、`IO/UI.h`（模板工厂）、`IO/UIStateGame.cpp`（实例化）、`IO/UITypes/*.{h,cpp}`（163 类 / 654 方法体）、`Net/PacketSwitch.cpp`（**305 条 `emplace<OPCODE, Handler>()`，`:417-782`**）、`Net/OutPacket.h`（**148 出站枚举 / 149 命名常量，`:37-318`**）、`Net/Packets/*.h`（**238 处 `OutPacket::Opcode::X` 构造**）、全仓 **917 处 `.dispatch()`**
10. `docs/features/s9-protocol-align/notes/gap-inventory.md`（128 行）—— **旧 head `44d0d5d9ca` / `client_out_sent=103` / gap=70 三分类总账，协议层已过时，仅作历史三分类参考**，引用前一律先查 §9.5

### 18.2 旧账（引用前一律先查销案）

`docs/features/beidou-v17-parity/notes/r00-baseline-parity.md:95-143`、`r12-remaining-features-audit.md:66-98`、`r14-native-feature-catalog.md:117-158,247,274`、`01-parity-matrix-and-plan.md:59-122`（2026-10-21）均为早期状态，已被 `r40-continuous-shell-coverage.md:5,7`、`parity-gap-2026-11/README.md:107`、`social-loop-audit-2026-11/README.md:17-19`、`r810-commit-split-ledger.md` §3.1（T14）大面积闭环。

### 18.3 复现命令

```bash
# ★ 协议层权威矩阵（2026-11-08 读数由此产生，§9.5 唯一权威源）
python tools/gen_opcode_matrix.py --check   # 生成/校验 docs/features/s9-protocol-align/protocol/opcode-matrix.json
# 协议差集重算（2026-11-04/05 现场，功能面复核）
python tools/gap_scan_2026_11.py
python tools/gap_c2s_refined.py          # 输出 tools/_gap_scan_out.json
# 旧账（仅历史参考，勿据此立项）
# docs/features/s9-protocol-align/notes/gap-inventory.md 基于 44d0d5d9ca / client_out_sent=103
# IDA 两端元数据（端口会变，先 server_health 读 idb_path）
python tools/ida_instance_probe.py       # tools/_ida_probe_out.json
python tools/ida_class_scan.py           # tools/_ida_class_scan_out.json
python tools/ida_angel_gap_ea.py         # evidence/angel-gap-feature-ea.json
# 构建
tools\build_smoke.bat                   # OutDir E:\MXD\BeiDou-Client-xy\MapleStory.exe
```

### 18.4 本文性质标注

- **观察** = 文档明写判定或源码逐字行；**推断** = 跨文档去重、大类归并、冲突裁决、优先级排序（§2.4 六处冲突、§2.5 撞值、§16 排序、§0/§1 结论段、§9.5.5 覆写结论）。
- 全部引用为相对路径:行号，拟引用 34 个证据文件已逐一确认存在；§9.5 新增权威源 `docs/features/s9-protocol-align/protocol/opcode-matrix.json` 已读 `:1-55` 复核。
- 本文为**只读盘点**，不含实现、不含 Plan、不新增代码、不提交（原子提交须用户授权）。
- 未完成项以 `Open objectives` 形式在本会话继续跟踪；本文落地即 Q2（m00014 / m00016）交付物。
- **2026-11-08 补录说明**：第三路子代理 `432be4cd`（协议与封包缺口清单）返回后，以 `opcode-matrix.json`(head `3d5ccc6f`) 重算推翻旧 `gap-inventory.md`(head `44d0d5d9ca`) 的 `client_out_sent=103 / gap=70` 判定，已在 §0、§2.3、§2.4、§7、§9、§9.2、§9.3、§9.4 G1、新增 §9.5、§14.1、§16、§18.1、§18.3、§18.4 逐处覆写；子代理为只读，未修改/未写/未提交任何文件。
