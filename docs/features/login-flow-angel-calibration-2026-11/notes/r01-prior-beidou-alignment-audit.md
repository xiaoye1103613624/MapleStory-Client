# 上一轮 BeiDou(13337) 登录对齐 · 只读四态清单

> 来源：subagent `7d02e984`（只读文档审计），2026-11-08 18:10 发回。
> 审计口径：已加载并遵守 `xiaoye-maple-story-dev`（只读、相对路径+行号、观察/推断/待实机标注）。**未修改任何文件**。全部路径相对 `E:\project\MapleStory-Client`。
> 标注 `[观察]`=IDA 字面量/实测/构建结果，`[推断]`=由证据推导未坐实，`[待实机]`=文档明确需真机/用户确认。
> 路径前缀说明：本节 `README.md` / `notes/x.md` / `evidence/x.md` / `plan-charselect-remap.md` 均指 `docs/features/login-flow-ui-ida-align/` 下文件（为可 grep，下面尽量给全路径）。

## 0. 先给主 agent 的证据口径结论（影响 Angel 复核优先级）
- `docs/features/login-flow-ui-ida-align/notes/ida-coords.md:3-4` — 本轮「权威版式坐标」自称来源是「参考客户端（北斗 gms083，`E:\MXD\Maplestory95.i64` 同源反编译产物）」；而 `docs/features/feature-gap-inventory-2026-11/README.md:5` 明令「095 线（浩浩岛095 / PLUS2 / 怀旧岛192 / `Maplestory95.i64`）不作本线参照与入库源」。→ 上一轮坐标的 IDB 溯源可能混入 095 命名 IDB，是 Angel 083/v83 带符号 IDB 首要复核点 [推断|待实机]
- `docs/features/login-flow-ui-ida-align/README.md:66` IDA MCP 端点 `http://127.0.0.1:13338/mcp`；`docs/features/login-flow-ui-ida-align/notes/ida-coords.md:259-260` 端点 `http://127.0.0.1:13337/mcp`（工具 `list_funcs`/`lookup_funcs`/`decompile`）。两端点并存，未说明差异 [观察]
- `docs/features/login-flow-ui-ida-align/notes/r09-login-flow-protocol.md:4` 参考端 `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe`（base `0x400000`，sha256 `1198fa57…`），协议事实源为服务端 `E:\project\BeiDou-Server_s9`（Java）[观察]
- `docs/features/login-flow-ui-ida-align/notes/r10-login-flow-missing-features.md:5-6` — 「rea 加载的仅是启动器，游戏逻辑在 `Gr2D_DX8.dll` 未加载，故协议证据以服务端 s9 源码为准」→ r09/r10 的协议结论**不是客户端二进制直接取证** [观察]

> **主 agent 复核（2026-11-08，本轮新增）**：EA 交叉验证 `evidence/angel-beidou-ea-crosscheck.json` 显示 Angel 328 个登录具名函数 EA 中 327 个在 BeiDou(13337) 同 EA 存在 → 两端是同一份 v83 二进制的不同符号状态，`ida-coords.md:3` 的「Maplestory95.i64 同源」措辞属文档污染，坐标本身来自 BeiDou.exe 无误。此疑点已用硬证据钉死。

## 1. 已校准（IDA 字面量 / 双分辨率实测 / 构建通过）
- `docs/features/login-flow-ui-ida-align/README.md:10` — 「登录 → 选世界 → 选频道 → 选角色 全链路已打通并逐屏对齐复验通过。」（逐屏实机截图）[观察]
- `docs/features/login-flow-ui-ida-align/README.md:16-21` — 世界槽位 y=40→110；chBackgrn 顶 74→139；`worldNoticeMessage` (58,-50)→(58,-90)；「到选择的世界去」判定区并入（改前「永远点不动」）[观察]
- `docs/features/login-flow-ui-ida-align/README.md:23` — 「世界槽位排布（步进 27、30 槽、提示图标 +5）经核对与 IDA 完全一致，未改动。」[观察]
- `docs/features/login-flow-ui-ida-align/README.md:30-31` — 条幅 Y 由写死 110 改为 `viewpos.y + WS_BANNER_OBJ_Y(-857)`；1280x720/1024x768 双分辨率「预测 111/135，实测横杆 111/135、槽位挂钩 112/136（±1px）」[观察]
- `docs/features/login-flow-ui-ida-align/README.md:34-37` — 参考端 `CWnd::CreateWnd / CDialog::CreateDlg / CFadeWnd::CreateFadeWnd` 末参 `W4UIOrigin@CWnd`，`CWndMan::GetOrgWindow(UIOrigin)` 提供参照窗口；步进恒 600 [观察+推断]
- `docs/features/login-flow-ui-ida-align/README.md:39-55` — 背景改用 `MapBackgrounds(src["back"])` = `MapLogin.img/back`（rx/ry 视差、tiling、ani，与地图同相机）；修掉 step=2 空白；复验 `step=1` 槽位挂钩仍 `x 235-259, y 112-202` 无回归、背景色数 48598→127583 [观察]
- `docs/features/login-flow-ui-ida-align/README.md:81-90` — CHARLIST 阻塞根因与修复：客户端 `LoginParser::parse_stats` 字段宽度不符导致 `parse_charentry` 抛 `PacketError`；修正后 `UICharSelect` 正常创建 [观察]（**口径冲突见 §4**）
- `docs/features/login-flow-ui-ida-align/README.md:101-111` — 选角 IDA 字面量落地：3 槽/行、步长 130、占用/空 `1384/1348`、窗口 `183×151 / 183×115`、三按钮 `id=1000/1001/1002, x=4, y=-2/36/85`；`GetStepHeight(2)=-1208`，`1384-1208=176`→屏幕 y=482 落桥面 [观察]
- `docs/features/login-flow-ui-ida-align/README.md:122-125` — 第二轮：`buttons` 是 `std::map`（越界静默插入空项/空指针崩溃）与 `channel_gauge` 定长 30 真实越界写已收敛；容量 5 世界→**15 世界 / 30 频道**；「槽位行距」**被证伪**，`PAGESIZE` 12→3、导航列数 4→3 [观察]
- `docs/features/login-flow-ui-ida-align/README.md:136-140` — 第三轮 r04~r08：`set_step` 按 `300*|new-old|+500` ms 补间、端点对齐 `GetStepHeight`、step5 映射 step1；索引守卫消 C4701；`request_pending` 门闩；`UILoginWait` 作请求中标记；新窗口补间结束后激活 [观察]
- `docs/features/login-flow-ui-ida-align/README.md:147-156` — 第四轮 r09：创建角色补 `hairColor` 第 11 字段；SERVERSTATUS 解析 0/1/2；`UIStateLogin::update()` 门闩看护；ADD_NEW_CHAR_ENTRY 失败包不产空角色；CHARLIST 去掉「必须存在活动 Wait」；自动登录改走 `get_worldbyid()`。「构建 EXITCODE=0；自动登录仍可进入游戏」[观察]
- `docs/features/login-flow-ui-ida-align/notes/r10-login-flow-missing-features.md:9-62` — task1 注册 5 个 handler（GENDER_DONE/CONFIRM_EULA_RESULT/CHECK_PINCODE/UPDATE_PINCODE/VIEW_ALL_CHAR）；构建 EXITCODE=0、自动登录回归无回归 [观察]
- `docs/features/login-flow-ui-ida-align/notes/r10-login-flow-missing-features.md:64-90` — task2 删除失败码按 s9 实际 0x09/0x14/0x16/0x1A/0x1D/0 修正，删 `case 10`；`LoginResultHandler`/`SelectCharacterHandler` default 由 `int8_t(reason-1)` 回绕改为 `UNKNOWN_ERROR` [观察]
- `docs/features/login-flow-ui-ida-align/notes/r10-login-flow-missing-features.md:93-99` — task3 `UIWorldSelect::clear_worlds()` 修复重连世界列表叠加 [观察]
- `docs/features/login-flow-ui-ida-align/notes/r10-login-flow-missing-features.md:118-146` — task4 `BtChangePIC` 复用 `request_pic()`（v83 `REGISTER_PIC(29)`）；task5 `ViewAllCharPacket`/`UIViewAllChar`/`BtCharacter` 落地 [观察]
- `docs/features/login-flow-ui-ida-align/notes/r08-step-window-defer.md:78-93` — 2026-10-21 22:02–22:07 1280x720 实机 5 步全链（登录→世界→频道→选角→进 Orbis），留 4 张 `chain_defer_*.png` [观察]
- `docs/features/login-flow-ui-ida-align/evidence/login-panel-offset.md:41-77` — 掩码 NCC 互相关（7 按钮 6 个 ncc=1.000）反推 `TITLE_WINDOW_OFFSET(58,16)`，已落 `IO/UITypes/UILogin.cpp` [观察]
- `docs/features/login-flow-ui-ida-align/notes/plan-charselect-remap.md:160-168` — 步骤 2/5 实测：三按钮竖排间距 38/49px、三槽间距 260=2×130 [观察]
- `docs/features/login-flow-ui-ida-align/notes/plan-charselect-remap.md:170-206` — `CS_FRAME_ORIGIN=(-237,-54)` 由选世界条幅反推并落地；三按钮帧内 (204,-1361/-1323/-1274)；槽位 X 预测 295/425/555 实测 300/430/558 [观察]
- `docs/features/login-flow-ui-ida-align/notes/plan-charselect-remap.md:208-255` — 槽位纵向锚点终解：`CS_LIST_T+176`(占用)/`+140`(空)，`SLOT_ROW_H` 80→151；占用槽屏幕 y=482 实测 ~470 落桥面 [观察]
- `docs/features/create-char-classic-dual-path/README.md:97` — 经典建角页 `kClassicWnd 700x460`、`kClassicMapOrigin(-372,-2128)` 用 `set_step(3)+get_target_viewpos()` 锚定相机，「闭合 `login-flow-ui-ida-align` 登记的『未锚定相机』待办」[观察]（与 README 第 1 条待办的口径冲突，见 §4）

## 2. 未闭环（已知缺口 / stub / 显式预留，未臆改）
- `docs/features/login-flow-ui-ida-align/README.md:159` — 「## 未完成 / 已知偏差（未臆改）」共 9 条，逐字引见 §5.1 [观察]
- `docs/features/login-flow-ui-ida-align/README.md:161-164` — ① `UIRaceSelect` 未设置 `position`，step3 背景滚动而 UI 不动；「进入创建角色界面超出本轮范围（避免触及写库流程），故登记为待办」[观察]
- `docs/features/login-flow-ui-ida-align/README.md:165-167` — ② 参考版 `CUINewCharRaceSelect` 700x460@(−372,−2128) 与本端 `Login.img/RaceSelect_new` 节点集/结构不同，「仅记录锚点备查」[观察]
- `docs/features/login-flow-ui-ida-align/README.md:168-169` — ③ `CUICharDetailVAC` `SetOption(148,-710)` / `CreateFadeWnd(170,210,0,z=11)` 读 `CLogin+316` 另一角色数组，「已排除『第 2 行槽位』的猜想，具体用途待查」[观察]
- `docs/features/login-flow-ui-ida-align/README.md:170` — ④ 按钮与地图挂链图元轻微重叠，属 NX 版本资源差异 [观察]
- `docs/features/login-flow-ui-ida-align/README.md:171` — ⑤ 旧窗口直接隐藏、无 ghost 绘制；参考端 `OnStepChanged` 补间结束后才销毁/创建（见 r08 §4）[观察]
- `docs/features/login-flow-ui-ida-align/README.md:172` — ⑥ step0 相机 X：参考端 `(28,GetStepHeight(step))` vs 本端 `(137,-20)` [观察]
- `docs/features/login-flow-ui-ida-align/README.md:173` — ⑦ `View All / View Choice` 仍 stub 点击无行为；频道固定 30 个、未实现 60 频道翻页 [观察]
- `docs/features/login-flow-ui-ida-align/README.md:174` — ⑧ 选角/世界请求门闩无超时；断线建议由 `Session` 重连统一清理 [观察]
- `docs/features/login-flow-ui-ida-align/README.md:175` — ⑨ opcode 4/5/6/7/8 只在名称表出现、无 handler（详见 r09 §4；r10 已按 s9 实现 6/7/8，属后轮补做）[观察]
- `docs/features/login-flow-ui-ida-align/notes/r09-login-flow-protocol.md:48-61` — §4 未完成/显式预留 10 项，逐字引见 §5.2 [观察]
- `docs/features/login-flow-ui-ida-align/notes/r10-login-flow-missing-features.md:165-168` — `UIViewAllChar` 的 `BtEnter` 「仅关闭窗口，未实现『从查看全部角色直接进所选角色』」[观察]
- `docs/features/login-flow-ui-ida-align/notes/r10-login-flow-missing-features.md:101-111` — rank/jobrank（协议有数据但参考端不绘）、burning/use_timestamp（s9 v83 协议无数据，v95+ 概念）按「未证实不臆造 UI」**显式预留** [观察]
- `docs/features/login-flow-ui-ida-align/notes/r05-charselect-index-guards.md:52-60` — 请求进行中防重、频道翻页 60、View All/View Choice stub、选角窗口结构 4 项「尚未对齐」[观察]
- `docs/features/login-flow-ui-ida-align/notes/r06-charselect-request-latch.md:36-40` — 超时、成功回包无显式复位、世界侧 `IsRequestValid` 未加（r07 后补）[观察]
- `docs/features/login-flow-ui-ida-align/notes/r07-worldselect-request-latch.md:37-41` — 未移植 `CLogin+368`，以 `UILoginWait` 等价标记；「若服务器长时间不回包，Wait 会一直存在……后续可加超时」[观察]
- `docs/features/login-flow-ui-ida-align/notes/r04-step-scroll-tween.md:33-37` — 窗口切换时机、step0 相机 X、step4 高度 3 项「尚未对齐」[观察]
- `docs/features/login-flow-ui-ida-align/notes/r08-step-window-defer.md:95-98` — 选角页蓝色 "Burning" 文本与「等级」行轻微重叠；白色竖条与瀑布重合「暂按背景图元处理，未改动」[观察]
- `docs/features/login-flow-ui-ida-align/notes/plan-charselect-remap.md:273-274` — 「按钮与地图挂链图元有轻微重叠……暂不调」[观察]
- `docs/features/login-flow-ui-ida-align/notes/ida-coords.md:225-236` — §6 4 条未解项：父窗口帧未唯一确定、`WS_MAX_WORLDS=5`（本轮未改、北斗 ≤5）、登录卡「正在连接到服务器。」、待补导出 `CWnd::CreateWnd`/`CUINewCharRaceSelect` ctor/`dword_BEDA4C`/`dword_BEDA50` [观察]
- `docs/features/feature-gap-inventory-2026-11/README.md:86-102` — 类 A 登录 17 条（A1~A17），其中 ❌：A1 `draw_world()` `if (world.channel_count < 2) continue;` 单频道世界整块跳过（P0，`IO/UITypes/UIWorldSelect.cpp:697-698`）、A2 60 频道翻页、A3 View All/View Choice stub、A4 VAC `BtEnter`、A14 角色槽位扩展；🟡：A5 opcode 4/5/6/7/8、A7 `CS_DETAIL_*` 硬编码/排行区未绘、A8 门闩无超时、A9 `CUICharDetailVAC` 归属、A16 `LOGIN_EMAIL=0x03` 客户端独有；⊘：A6 rank/burning/timestamp 显式预留、A12 不移植高版本建角资源；✅：A15 快速登录 [观察]
- `docs/features/parity-gap-2026-11/README.md:55` — `PICK_ALL_CHAR(14)` 修正误发 19→14/32 [观察]；`:134` 角色槽位扩展 ❌；`:135` 快速登录 ✅ [观察]

## 3. 待实机（文档明确需真机/截图/用户）
- `docs/features/login-flow-ui-ida-align/README.md:57-58` — 「> 待确认：改用地图自带 back 层后，step=0/1 的**背景观感**与之前不同（step=1 由木牌签变为卷轴）。> 由于本机没有可运行的原版 gms083 客户端做对照，请以原版截图确认哪种正确。」[待实机]
- `docs/features/login-flow-ui-ida-align/README.md:157` — 「实机截图仍受阻于本机渲染黑屏（基线 exe 同样黑，非本轮改动导致）。」[待实机]
- `docs/features/login-flow-ui-ida-align/README.md:179` — 「若允许进入创建角色界面（**注意：完成建角会写库，仅打开界面不写**），实测 step 3 落点以修正第 1 项」[待实机]
- `docs/features/login-flow-ui-ida-align/notes/r04-step-scroll-tween.md:41-42` — 构建 EXITCODE=0；「实机：登录 → 选世界 → 选频道/角色 的滚动时长与方向待点测截图确认。」[待实机]
- `docs/features/login-flow-ui-ida-align/notes/r05-charselect-index-guards.md:64-65` — 「实机：删除默认角色后重进选角、或把 `DefaultCharacter` 改成越界值，确认不再空白/崩溃。」[待实机]
- `docs/features/login-flow-ui-ida-align/notes/r09-login-flow-protocol.md:43-46` — 「**未完成的实机验证**：本机游戏窗口（含登录界面）截出来全黑……建角端到端（需键盘输入角色名，SendKeys 对当前端无效）待该问题解决后由实机或用户确认。」[待实机]
- `docs/features/login-flow-ui-ida-align/notes/r09-login-flow-protocol.md:65-66` — 「1. 先解决**渲染黑屏**……否则登录链路无法留下实机截图证据。2. 黑屏解决后优先端到端验证建角」[待实机]
- `docs/features/login-flow-ui-ida-align/notes/r10-login-flow-missing-features.md:168` — 「端到端依赖服务端真实下发 `showAllCharacter`/`showAllCharacterInfo`（s9 支持），需用户实机点击验证。」[待实机]
- `docs/features/login-flow-ui-ida-align/notes/r10-login-flow-missing-features.md:176` — 「黑屏环境既有问题不修，端到端建角/PIC/查看全部角色需用户实机确认。」[待实机]
- `docs/features/login-flow-ui-ida-align/notes/round2-hardening.md:159` — 「### 4.3 附带发现：`UIRaceSelect` 未锚定地图相机（潜在缺陷，待实测）」[待实机]
- `docs/features/login-flow-ui-ida-align/notes/round2-hardening.md:165-168` — 「**未修**：修正量需要 step 3 的实测落点才能定……按「无充分证据不臆改」保留，并在此登记为待办。」[待实机]
- `docs/features/login-flow-ui-ida-align/notes/ui-positioning-scheme.md:39-41` — S1 已完成；S2「⏳ 待标定」、S3「⏳ 待做」[待实机]
- `docs/features/login-flow-ui-ida-align/notes/ui-positioning-scheme.md:79-82` — 验收「编译 → 部署 → `_flow.ps1` 自动登录并点击到目标屏 → 截图 → 量化比对」[待实机]
- `docs/features/pending-live-tests/README.md:23` — #8 `PICK_ALL_CHAR` ⏳待测（c2s 14/32）；`:429-436` 操作「点选角色 → BtEnter……抓包为 **14**（无 PIC）或 **32**」[待实机]
- `docs/features/pending-live-tests/README.md:139-152` — #3 登录遮罩四页截图叠图：关闭自动登录逐页按 SCROLL_LOCK → 复制到 `correct_shots/` → 半透明叠图 → 「仅偏差项按 playbook 改对应 knob（勿盲改 `CS_DETAIL_*` / Wait bar）」[待实机]
- `docs/features/feature-gap-inventory-2026-11/README.md:95-98` — A10 step0 相机 X / step1 卷轴 / Loading bar ⏳叠图；A11 四页叠图 ⏳需用户；A13 建角三职业页叠图/名字基线/step4 `(323,150)` ⏳需用户 [待实机]
- `docs/features/feature-gap-inventory-2026-11/README.md:547,571,574` — 挂起表：登录遮罩四页截图叠图、step0 相机 X/step1 背景/Loading bar 叠图（「本机渲染黑屏（基线 exe 同黑）」）、VIEW_ALL_CHAR 端到端 [待实机]
- `docs/features/create-char-classic-dual-path/README.md:125` — 「实机三职业建角页叠图仍需用户启动客户端确认（本轮不真机）」；`:156` 「**三职业建角页叠图本轮未完成**……按规范如实记录为未跑项，未伪称通过。」；`:202-204` 名字基线与字色需实机、`set_text_shift(0,15)` 是字体度量推导值；`:267-269` step4 外观页实机叠图未跑 [待实机]
- `docs/features/login-flow-regression-fix/notes/native-video-analysis-2026-11-04.md:35` — 「加载弹窗：视频/截图中均未出现（本地服瞬时），问题2 定标仍缺正确端参考图，待用户补。」[待实机]
- `docs/features/login-modal-viewport-center/README.md:44-45` — TOS/Notice/Wait 居中「人工」验收两项仍未勾选 [待实机]
- `docs/features/login-input-and-boot-fast/README.md:30-33` — 4 项人工验证未勾；`:35-39` 「进游戏只有 BGM、画面仍异常」未闭环 [待实机]

## 4. 推测（推断 / 证伪 / 无证据 / 结构不同 / 未照搬 — 逐字引）
- `docs/features/login-flow-ui-ida-align/README.md:95` — 「本实现 `UICharSelect` 是 800x600 的单一 `UIElement` 且大量坐标取自 NX 节点，**结构不同**。」[观察]
- `docs/features/login-flow-ui-ida-align/README.md:96` — 「故**不照搬锚点**，而采用「**原点归一 + 锚点式偏移**」」[推断]
- `docs/features/login-flow-ui-ida-align/README.md:125` — 「**被证伪**：参考版 `y` 与索引无关、`x` 用绝对索引 `%3`、`OnButtonClicked` 无翻页按钮 ⇒ 参考版是「固定 3 槽 + 翻页换角色」。」[观察]
- `docs/features/login-flow-ui-ida-align/README.md:128-130` — 「> **一处前提被证伪**：上一轮记录的「`includes/NoLifeNx/**` 纯行尾符噪声」不成立 —— 那些 `M` 多数只是 **stat 脏（mtime）**，真差异的 14 个文件是**真实功能开发**（新增 `wz_file` 支持）。因此**未做** `git add --renormalize` 归一化提交。」[观察]
- `docs/features/login-flow-ui-ida-align/README.md:173` — 「（参考端 exe 内搜不到字面量，语义无证据）」[推断]
- `docs/features/login-flow-ui-ida-align/notes/ida-coords.md:119-121` — 「> **偏差记录（未改）**：参考版 `dword_BEDA68` 为 **540x167** @ (136,51)，本实现 `chBackgrn` 为 **449x233**（尺寸/切片不同，属不同 NX 版本资源），故**不照搬**其尺寸与绝对坐标，仅用 IDA 的**相对关系**……指导版式。」[观察+推断]
- `docs/features/login-flow-ui-ida-align/notes/ida-coords.md:158-161` — 「`CUICharSelect` 在参考版中是 **101x131 的小窗口**……本实现 `UICharSelect` 是 800x600 的单一 UIElement……**与参考版结构不同**，因此不能把 `(200,-1359,101,131)` 直接搬过来。」[观察]
- `docs/features/login-flow-ui-ida-align/notes/ida-coords.md:165-169` — 「1. `nTop = -804 / -1359 / -2369` 与 `GetStepHeight = -8-600*step` …… **不是 600 的整数倍关系**（差值 -196 / -151 / +39），说明这些坐标并非相对『步进窗口』的简单叠加，其父窗口原点的转换规则无法从现有 dump 唯一确定。2. 登录流程在选频道后卡在『正在连接到服务器。』，**无法进入选角色界面做实测反推**。3. 因此按「无充分证据不臆改」原则，本轮**未改**任何角色/创建界面坐标。」[推断]
- `docs/features/login-flow-ui-ida-align/notes/ida-coords.md:227-230` — 「1. **父窗口帧仍未唯一确定**：已补出 `CLogin::ChangeStep`(0x5F53C0)，但 `CWnd::CreateWnd` / `CWndMan` / `CLogin` 自身窗口的 `MoveTo/SetPosition` 仍未导出……故屏幕绝对 Y 只能由**实测截图反推**（§2.3），未按 `-804` 直接换算。」[推断]
- `docs/features/login-flow-ui-ida-align/evidence/login-panel-offset.md:24` — 「但 **CUITitle 窗口自身的屏幕位置在反编译中未确定**（见 `notes/ida-coords.md` §6「父窗口帧仍未唯一确定」）。」[推断]
- `docs/features/login-flow-ui-ida-align/notes/charselect-node-schema.md:82` — 「**没有任何 `pos` / `LT` / `RB` / `space` 之类的布局节点。**」；`:84-85` 「北斗 `Login.img/CharSelect` 在数据层**只有美术资源**……`pos` / `charSlot` / `pageNew` / `EditCharList` 这些布局容器**根本不存在**。」[观察]
- `docs/features/login-flow-ui-ida-align/notes/charselect-node-schema.md:90` — 「`UICharSelect`（选角色）：**不成立**——该界面节点里没有布局，**所有位置都在原版代码里**」[观察]
- `docs/features/login-flow-ui-ida-align/notes/charselect-node-schema.md:97-100` — 「说明这两个未被命名或内联），方法：从 `off_AF6D90`（ctor 里写的 vtable）沿虚表找 `Draw@PBUtagRECT@@` 槽位」[推断]
- `docs/features/login-flow-ui-ida-align/notes/charselect-node-schema.md:134-137` — 4 个全局窗口「承担内容（**推断**）」表头，其中 `dword_BEDA4C` 类「待定」[推断]
- `docs/features/login-flow-ui-ida-align/notes/charselect-node-schema.md:210` — 「即列表面板的类名尚未确定，下一步应在 `sub_603FF0` / `sub_60418B` 中定位」[推断]
- `docs/features/login-flow-ui-ida-align/notes/ui-positioning-scheme.md:68-71` — 「`position` = 登录地图坐标系原点到……的偏移，**不能凭现有 IDA dump 解析**（参考版 `CUICharSelect` 只是 101x131 的子窗口，且 `nTop = −1359` 的父窗口帧无法唯一确定……），必须靠**原版截图实测反推**」[推断]
- `docs/features/login-flow-ui-ida-align/notes/ui-positioning-scheme.md:75-77` — 「『删除角色』/『排行榜』等按钮创建时**未传位置**……局部坐标默认 `(0,0)` → 停在屏幕左上角。需要按 NX 节点补齐各自偏移。」「`namelabel`/`infolabels`/`charlooks` 未显示：需确认是数据（角色 look/名字）还是坐标问题。」[推断]
- `docs/features/login-flow-ui-ida-align/notes/round2-hardening.md:86` — 「### 2.2 横向原点无法从现有 dump 反推（穷尽的可达路径）」；`:106-107` 「> 与纵向对照：纵向原点可由 `GetStepHeight(2) = -1208` 推出，横向不可 —— 这一不对称性已如实记录，未强行拟合成一个「看起来统一」的公式。」[推断]
- `docs/features/login-flow-ui-ida-align/notes/round2-hardening.md:139` — 「## 4. 创建角色界面：锚点已取，但**不可照搬**（结论：待定）」；`:157` 「与 `CUICharSelect` 101x131 的情形同类。故仅记录锚点备查，**不照搬**。」[观察]
- `docs/features/login-flow-ui-ida-align/notes/round2-hardening.md:174-183` — 「### 5.1 「行尾符噪声」前提被证伪」及 14 个真实功能文件清单 [观察]
- `docs/features/login-flow-ui-ida-align/notes/plan-charselect-remap.md:67` — 「| `UICharSelect` 三面板拆分与定位 | ❌ **待做（本文执行清单）** |」（注：§5.2/§5.4 后续称已标定落地，属文档前后状态叠加）[推断]
- `docs/features/login-flow-ui-ida-align/notes/plan-charselect-remap.md:117` — 「// ← 唯一待标定量」；`:154` 「> 该换算在本文首次闭环；若实测有偏差，只需调整这一个常量」[推断]
- `docs/features/login-flow-ui-ida-align/notes/r09-login-flow-protocol.md:6` — 「> 本轮原则：**只改有双向证据（服务端读写 + 客户端读写）支撑的点**；语义未证实的按钮保持 stub 并登记（见 §4）。」[观察]
- `docs/features/login-flow-ui-ida-align/notes/r10-login-flow-missing-features.md:107` — 「故不臆造 RANK/JOBRANK UI（位置无证据）。」；`:111` 「按 r09 §4「未证实不臆造 UI」原则**显式预留**，不改动 UICharSelect。」[观察]
- `docs/features/login-flow-ui-ida-align/notes/wz-datasource-notes.md:38` — 「初判曾想把 `tools/vendor/**` 的 `*.obj/*.pdb/*.lib` 一并忽略，实测**证伪**」；`:55-59` `WzNxBootstrap::prepare()` 无调用者=死代码、引用 `Error.h` 不存在的枚举 `WZ_CONVERTER_MISSING`/`WZ_CONVERSION_FAILED`（一旦启用 `AUTO_CONVERT_WZ` 即编译失败）→ 已移除 [观察]
- **口径冲突（新增发现，建议 Angel 复核）**：`docs/features/login-flow-ui-ida-align/README.md:88-89` 称「服务端 `hp/maxHp/mp/maxMp` 为 **int32**、`exp` 为 **int64**，而客户端按 **int16**×4 与 **int32** 读取」；但 `docs/features/login-flow-ui-ida-align/notes/charlist-parse-fix.md:47-51` 对同一修复记为 `level` `read_short(2)→read_byte(1)`、`exp` `read_int(4)→read_long(8)`、「尾部 `skip(4)` timestamp → **无**」，且 `:43` 明言「→ **北斗包体在此没有任何尾部 timestamp(int)**。」、`:29-34` 字节表显示 hp/maxhp/mp/maxmp 各 2 字节。两处对根因/字段宽度的描述互斥 [推断]
- **状态冲突（新增发现）**：`docs/features/login-flow-ui-ida-align/README.md:161-164` 仍把 `UIRaceSelect` 未锚相机列为待办，而 `docs/features/create-char-classic-dual-path/README.md:97` 称已「闭合 `login-flow-ui-ida-align` 登记的『未锚定相机』待办」；`docs/features/feature-gap-inventory-2026-11/README.md:97`（A12）亦判 step4 已实现。以较新现场为准，README 9 条清单未回写 [推断]
- `docs/features/login-flow-regression-fix/notes/img-evidence-audit.md:29` — 「## 2. 原生逻辑证据（已归档，未补跑 REA/IDA）」；`:34` 「**未记载任何 S 曲线缓动**」（推翻 r04 自写 `t²(3-2t)`）；`:46` 建议 `current_offset()` 改线性 [推断]
- `docs/features/create-char-classic-dual-path/README.md:247-249` — 「屏幕落点 `(323,150)` 为几何避让推导（左栏 150px、吊牌 x=721、角色 x=627、面板 225x377），**低置信度，明确标为决策点**，未伪装成原生常量。」；`:274` 「step4 与命名态严格先后为流程推断（IDA 只钉到 step4 存在）」；`:275` 「`statTb` 具体落点、`charAlert` 显示时机无 xref」[推断]

## 5. 重点逐字引（README 9 条 / r09 §4 / r10 缺失表）
### 5.1 README `docs/features/login-flow-ui-ida-align/README.md:159-175` 九条
1. `:161` 「**`UIRaceSelect` 未锚定地图相机（潜在缺陷，待实测）**」；`:164` 「修正量需 step 3 的实测落点，而进入创建角色界面超出本轮范围（避免触及写库流程），故登记为待办。」
2. `:165` 「**创建角色界面版式不可照搬**：参考版 `CUINewCharRaceSelect` 为 700x460 @ canvas(−372,−2128)」；`:167` 「与参考版 node 集/结构都不同，仅记录锚点备查。」
3. `:168` 「**`CUICharDetailVAC` 归属未明**：其 `SetOption` 点为 `(148, -710)`、`CreateFadeWnd(170, 210, 0, z=11)`」；`:169` 「已排除「第 2 行槽位」的猜想，具体用途待查。」
4. `:170` 「**按钮与地图挂链图元轻微重叠**：属 NX 版本资源差异，不影响可点击与可读性。」
5. `:171` 「**旧窗口滚动**：参考端 `OnStepChanged` 在补间结束后才销毁/创建窗口，旧窗口会随背景滚出；当前端已在补间结束后才激活新窗口，但旧窗口是直接隐藏，没有 ghost 绘制。见 `r08` §4。」
6. `:172` 「**step 0 相机 X**：参考端 `CLogin::Init` 使用 `(28, GetStepHeight(step))`，当前登录页沿用 `(137, -20)` 锚点对齐书本开口；需实机叠图确认是否为系统性偏移。见 `r04` §4.2。」
7. `:173` 「**View All / View Choice**：`UIWorldSelect` 两个按钮仍是 stub，点击无行为（参考端 exe 内搜不到字面量，语义无证据）；频道按钮固定 30 个，未实现参考端的 60 频道翻页。」
8. `:174` 「**请求超时**：选角/世界请求门闩依赖服务器回包或 UI 销毁复位，未加超时；第四轮已补「取消后门闩看护」（`UIStateLogin::update()`），断线场景仍建议由 `Session` 重连统一清理。」
9. `:175` 「**未注册 handler**：opcode 4 `GENDER_DONE` / 5 `CONFIRM_EULA_RESULT` / 6 `CHECK_PINCODE` / 7 `UPDATE_PINCODE` / 8 `VIEW_ALL_CHAR` 只在名称表出现，无 handler；北斗默认 `enable_pic/enable_pin=false`，暂不影响主链路（详见 r09 §4）。」

### 5.2 r09 §4 `docs/features/login-flow-ui-ida-align/notes/r09-login-flow-protocol.md:48-61` 十项
- `:48` 「## 4. 未完成 / 显式预留（未臆改）」
- `:52` 「| `BtViewChoice` / `BtViewAll` | 保持 stub | 参考端 exe 内搜不到 `BtViewChoice` 字面量（`search_strings` 0 命中），语义无证据；需定位 `CUIWorldSelect::OnButtonClicked` 对应分支 |」
- `:53` 「| `BtChangePIC` / `BtResetPIC` | stub（`show_pic_btns` 恒 false） | 依赖 PIC 体系，服务端默认关闭；v83 数据缺 `KeyConfig/KeyType`，无法构造输入界面 |」
- `:54` 「| `BtCharacter`（查看全部角色） | 空实现 | 需新增 `VIEW_ALL_CHAR(8)` 请求与 `showAllCharacter(Info)` 两个回包 handler + 新 UI，工程量独立 |」
- `:55` 「| opcode 4 `GENDER_DONE` / 5 `CONFIRM_EULA_RESULT` / 6 `CHECK_PINCODE` / 7 `UPDATE_PINCODE` | 未注册 handler | 仅在 opcode 名称表出现；北斗默认 `enable_pin=false` 且 EULA/性别流程不触发，暂不影响主链路 |」
- `:56` 「| 删除失败码细化 | 仅 10/20 有文案 | 服务端注释给出 0x16/0x18/0x1A/0x1D 等，但客户端 `UILoginNotice` 无对应枚举，需先补文案 |」
- `:57` 「| `LoginResultHandler` default 分支 | `int8_t(reason-1)` 回绕 | reason>128 会落到错误文案；待与服务端 reason 取值域核对后加固 |」
- `:58` 「| `ServerlistHandler` 重复下发 | `worlds` 累加未清理 | 重连场景会重复世界条目；需先确认是否由看护层统一清理 |」
- `:59` 「| `rank`/`jobrank`、`burning`、`use_timestamp` | 未呈现 | 需参考端选角面板版式证据（与 r08 的 Burning 文本重叠同一处） |」
- `:60` 「| `UIRegion` / `BtRegion` | 按钮不可达 + `set_region` 空 | 北斗单区；需先确认参考端是否有大区入口 |」
- `:61` 「| 请求超时 | 未加 | 现有 Cancel 按钮 + 门闩看护已可恢复；自动超时可能误伤慢速登录，需先定阈值依据 |」
- 注：r10 已消解 `:52`(部分)/`:53`/`:54`/`:55`/`:56`/`:57`/`:58`，但 r09 §4 正文未回写 [推断]

### 5.3 r10 缺失功能表 `docs/features/login-flow-ui-ida-align/notes/r10-login-flow-missing-features.md`
- `:15-21` task1 决策表：`GENDER_DONE(4)`/`CONFIRM_EULA_RESULT(5)` s9 **否**发送（走 `LOGIN_RESULT` 闭环）；`CHECK_PINCODE(6)`/`UPDATE_PINCODE(7)`/`VIEW_ALL_CHAR(8)` s9 是发送 [观察]
- `:23-26` 「> 关键事实：s9 的 `SetGenderHandler` / `AcceptToSHandler` 成功后都回 `getAuthSuccess`……所以 `GENDER_DONE` / `CONFIRM_EULA_RESULT` 在 s9 下永不下发，注册其 handler 是协议完整性，非臆造功能」[观察]
- `:50-55` 5 个 handler 行为（含 `ViewAllCharHandler` 首字节 0=详情 / 其余=概览）[观察]
- `:91` 「### task 3 · serverlist-dedup-and-rank（已完成 serverlist 去重；rank/burning/timestamp 不呈现）」[观察]
- `:101-111` 「rank / jobrank / burning / timestamp 不呈现（证据判定，非 bug）」：「**不臆造 RANK/JOBRANK UI（位置无证据）**」「（v83 创建角色无"燃烧"概念，burning 是 v95+ 引入。）无数据不呈现。」[观察]
- `:137-141` task4 限制：「s9 默认 `enable_pic=false`，且 `RegisterPicHandler` 不支持「修改」（已设即断会话）」[观察]
- `:165-168` task5 风险/待办：「`BtEnter` 仅关闭窗口，未实现「从查看全部角色直接进所选角色」」「端到端依赖服务端真实下发……需用户实机点击验证」[观察|待实机]
- `:172-176` 验证：构建 EXITCODE=1 系陈旧日志、真实 link 成功；「黑屏环境既有问题不修，端到端建角/PIC/查看全部角色需用户实机确认」[观察|待实机]

## 6. 其它 docs/features 登录相关挂起 / 缺口（路径:行号）
- `docs/features/feature-gap-inventory-2026-11/README.md:86-102` — 类 A 全表（见 §2）；`:511-512` 明确不做：「不移植高版本 `RaceSelect_new` 资源包、不改服务端建角协议字段」「地图锚点登录页（`UILogin`/`UIWorldSelect`/`UICharSelect`）、协议/封包 | 明确不做」；`:522` 「rank/jobrank/burning/timestamp 选角 UI | 未证实不臆造 UI」[观察]
- `docs/features/feature-gap-inventory-2026-11/README.md:529-574` — §15 挂起表：`:547` #3 登录遮罩四页截图叠图 ⏳；`:552` #8 `PICK_ALL_CHAR`（实机点选 BtEnter）；`:571` 叠图确认 step0 相机 X / step1 背景 / Loading bar（「本机渲染黑屏（基线 exe 同黑）」）；`:574` VIEW_ALL_CHAR 端到端 [待实机]
- `docs/features/parity-gap-2026-11/README.md:55` — 「世界地图查看后选角 | 14 `PICK_ALL_CHAR` | ✅ 2026-11-05 修正误发 19→14/32」[观察]；`:83` `REGISTER_PIN`/PIC 31/32 明确不做 [观察]
- `docs/features/pending-live-tests/README.md:16` — #3 SoftKey 登录遮罩截图 ✅ 2026-11-05（根因 `screenshot_directory()` 已修，见 `notes/scrolllock-rootcause.md`）[观察]；`:23` #8 PICK_ALL_CHAR ⏳；`:90` 「登录链路……✅ 零手动全自动 登录→世界→频道→选角→进图 Henesys」[观察]；`:112` 「SoftKey『截图键』……请手按 SCROLL_LOCK 复核」[待实机]；`:139-152` 四页叠图步骤 [待实机]；`:463-475` 三项手点 [待实机]
- `docs/features/beidou-v17-parity/notes/r15-baseline-ui-gap-vs-screenshots.md:83-88` — 登录界面背景/木牌/Logo/Loading 对照（P1「实机叠图验收 step0」「偏差则只调 `LoginBackground::target_offset(0)`」）[待实机]；`:104` 「**硬缺陷：** `draw_world()` 中 `if (world.channel_count < 2) continue;`」；`:110-115` 代码块 `IO/UITypes/UIWorldSelect.cpp:697-698` 与「建议修复项（P0）：将过滤改为 `channel_count < 1`」[观察]；`:121-125` 选角属性板 `CS_DETAIL_*` 硬编码 / 排行区「排行不可用」未绘 [观察]；`:204-210` W3「是否需要你再补「当前端」同场景截图以便叠图像素 diff？ | 建议补」[待实机]
- `docs/features/beidou-v17-parity/notes/r21-remaining-backlog.md:13` — 「1. 登录遮罩图截图 → evidence/r20/current_shots/」[待实机]
- `docs/features/beidou-v17-parity/notes/r201-login-overlay-playbook.md:3` — 「本笔记固定登录叠图 knobs，待当前端截图后调」；`:18-22` 5 个优先 knobs（step0 `{137,-20}`、`signboard_map_pos(137,21)`+`TITLE_WINDOW_OFFSET(58,16)`、`CH_SLIDE_DIST`、`CS_CHARINFO_OFF(-93,-208)`、Wait bar `(99,97)` 勿回退 46,98）[观察|待实机]
- `docs/features/create-char-classic-dual-path/README.md:97,125,156,202-204,267-275` — 见 §1/§3/§4 [观察|待实机|推断]
- `docs/features/login-flow-regression-fix/notes/img-evidence-audit.md:4-9` — 运行时 IMG 真值来源与「orange-wz MCP……未使用」；`:11-27` 运行时节点属性表；`:29-39` 「原生逻辑证据（已归档，未补跑 REA/IDA）」；`:65-68` 构建 EXITCODE=0 + 「正确客户端截图……仅作**最后**视觉校准」[观察]
- `docs/features/login-flow-regression-fix/notes/native-video-analysis-2026-11-04.md:6-35` — 原生视频抽帧语义（频道卷筒 4 帧 250/50/50、末帧 513x401 常驻、选角 scroll/2 末帧 217x193 常驻、详情滚筒顶≈(385,155)、加载弹窗缺正确端参考图）[观察|待实机]
- `docs/features/login-modal-viewport-center/README.md:14` — 「**不做**：不改地图锚点登录页（`UILogin` / `UIWorldSelect` / `UICharSelect`）；不动协议/封包。」[观察]
- `docs/features/login-input-and-boot-fast/README.md:35-39` — 「进游戏只有 BGM、画面仍异常」未闭环，转 `enter-game-blackscreen-fix/` [待实机]
- `docs/features/login-layout-1024-fix/evidence/login-frame-compare.md:4` 数据源 `UI.nx`；`:26-29` 「**整个 UI.nx 中 1024×768 的位图只有 `frame1024` 一张**……原版登录 UI 是按 1280×720 做的」[观察]

## 7. IMG 节点树 / 截图 layout JSON 证据检索结论
- **orange-wz IMG 节点树证据：未找到**。`glob **/orange-wz*/**/*` 无结果；反向证据 `docs/features/login-flow-regression-fix/notes/img-evidence-audit.md:7` — 「orange-wz MCP 对这批私服改版 img 内部节点有已知解析缺陷，未使用。」全程登录链路证据均来自自建 `tools/nx_inspect.py`、运行时 `[DBG-UI]` 转储或 `E:\MXD\BeiDou-Client-xy\REA反编译\wz_dump\*_runtime.json`（该目录**在仓库外**，仓库内 `glob **/*_runtime.json` / `**/wz_dump/**/*` 均无结果）[观察]
- **登录用 `layout-*.json`：未找到**。全仓 `**/*layout*.json` 仅 `docs/features/char-stats-ui-ps/ui/detail/v4/layout_cn.json`、`docs/features/map-search-ui/resources/layout.json`、`docs/features/map-search-ui/ui/search_panel_backgrnd/v3_pipeline/layout.json`、`docs/features/map-info-panel/resources/layout.json`、`UI/MapInfo/layout.json`，均非登录链路。`**/layout-*.json` 无结果 [观察]
- **`evidence/screens/` 目录：未找到**（`glob **/evidence/screens/**/*` 无结果）。截图证据散落在功能目录 `evidence/` 下 [观察]
- 现有登录 IMG/节点类证据（**均非 orange-wz，非标准节点树 JSON**）：
  - `docs/features/login-flow-ui-ida-align/evidence/charselect_node_dump.txt`（31 行 `[CS-DUMP]`，`Login.img/CharSelect` 布局节点全 `<absent>`）、`charselect_node_dump_full.txt`（237 行，叶子 `origin/z/delay/a0/a1`，无 `pos/LT/RB`）——来自客户端 `[DBG-UI]` 临时块 [观察]
  - `docs/features/login-flow-ui-ida-align/evidence/MapLogin_logo_metrics.txt:1` — `[DBG-ML] logo 397x219 o=198,109 sign 368x236 o=184,118 eff 238x298 o=0,0`；`MapLogin_sign_metrics.txt:1-2` 仅 2 行背景路径调试日志 [观察]
  - `docs/features/login-flow-regression-fix/notes/img-evidence-audit.md:11-27` — 运行时 IMG 属性最完整文本表（chBackgrn 449x233 o(224,116)、scroll/0 四帧 513x152/416/408/401 delay 250/50/50、scroll/1 反序、chSelect 四帧 23x11→59x33、WorldNotice 九宫拼框无 184x74 单 canvas、charInfo1 183x151 o(45,57)、charInfo 183x115 o(45,57)、scroll/2 四帧 217x30/196/195/193、`Login.img/Title` 仅 MSTitle+7 Bt*+check+effect ⇒ `bedou=true`、木牌 368x236 o(184,118)）[观察]
  - `docs/features/login-flow-ui-ida-align/evidence/CUIWorldSelect_DrawWorldItems_0x624e13.txt` 等 13 个逐函数 IDA dump（清单 `notes/ida-coords.md:244-257`），以及 `CUICharSelect_OnCreate_0x60148d.txt`、`CUICharDetail_Draw_0x602adc.txt`、`CFadeWnd_SetOption_0x51f93d.txt`、`CFadeWnd_CreateFadeWnd_0x51f9ad.txt`、`CUICharDetail_ctor_0x60292f.txt`、`CUICharDetailVAC_ctor_0x60745a.txt`、`CUINewCharRaceSelect_ctor_0x615596.txt` 等 [观察]
  - `docs/features/create-char-classic-dual-path/evidence/img-node-layout.txt` / `runtime-scene-nodes.txt` / `nx-probe.txt` + `native_*.png`（约 30 张 IMG 位图导出）、`measure_charset.py` [观察]
  - `docs/features/login-layout-1024-fix/evidence/` — `chBackgrn.png`、`ws-signboard{,1,2}.png`、`ws-selectWorld.png`、`ws-step1.png`、`ws-balloon-title.png`、`ws-populated1.png`、`cs-btselect.png`、`cs-effect0/1.png`、`cs-signboard.png`、`cs-fx0/1.png`、`bt-viewall.png`、`bt-viewchoice.png`、`btworld-{0,e}.png`、`world-name-0.png`、`noticescroll1-{0,1}.png`、`bflp-{nw,n,c}.png` 等登录控件位图 + `login-frame-compare.md` [观察]
  - `docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/` — 7 张正确端截图（登录界面/选择频道页/选中频道页/选择角色页/进入游戏页/打开了S-E-I-K页面/键盘设置）；`current_shots/` **仅有 README.md，无当前端截图**（`README.md:3-8` 要求补齐后叠图）[观察|待实机]；`overlay_diff.py` 叠图脚本 [观察]
  - `docs/features/login-flow-ui-ida-align/evidence/` — `verified_worldselect.png`、`verified_worldselect_1024x768.png`、`verified_channel.png`、`charselect_*.png`（bg_fixed/slots3/ida_buttons/origin_calibrated/final/buttons_visible/slots_on_bridge/reached/blocked_charselect_stalled/pagesize3_verify）、`chain_defer_{world,channel,charselect,ingame}_final.png`、`chain_verify_after_world_guard.png`、`worldselect_expanded_capacity.png`、`probe_1024x768_login.png` [观察]
  - `docs/features/create-char-classic-dual-path/evidence/live_*.png`、`docs/features/pending-live-tests/evidence/live-20261105-*.png`（真机帧，含 softkey/scrolllock）[观察]
  - 仓库根 `_tmp_Login.img_Notice_Loading_bar_{0,5,10}.png` 3 张 Loading bar 位图 [观察]
  - `docs/features/beidou-v17-parity/notes/evidence/r18/Login_CharSelect_search_summary.txt:7-9` — 「`Notice/Loading, signboard, Common/frame, Ranking, CUICharSelect, BtGoWorld: NO ascii hits`」「结论: 登录木牌/Loading/排行等路径多半不在 exe 明文 ASCII；续跑需 UTF-16 / 动态拼接 / 更大 CUI* 构造函数定位」[观察]

## 8. 上一轮校准常量表（常量名 → 值 → 来源行号 → 验证方式）

| 常量/事实 | 值 | 来源（路径:行号） | 验证 |
|---|---|---|---|
| `CLogin::GetStepHeight` | `-8 - 600*(v2+a2)` | `notes/ida-coords.md:16-22` | IDA dump |
| 步进位移 / step 数 | 600 / 6 步（0登录 1选世界 2频道·角色 3新建 4预览 5其他） | `README.md:36,72-73`；`notes/ida-coords.md:129-138` | IDA |
| 补间时长 | `300*abs(new-old)+500` ms；step5→step1 高度 | `notes/r04-step-scroll-tween.md:11-13`；`README.md:136` | IDA |
| 补间实现 / 音效 | `sub_43E7C7` 写 `IWzGr2D` 中心；`SP_1236_SCROLLUP` | `notes/r04-step-scroll-tween.md:14-15` | IDA（本端自写 S 曲线被 `img-evidence-audit.md:34` 判无依据） |
| 窗口切换门 | `now - *(this+364) > 0` 才 `OnStepChanged` | `notes/r08-step-window-defer.md:16-19` | IDA |
| `CLogin+360/368/364` | step / 请求门闩 / 补间结束时间 | `notes/r05-charselect-index-guards.md:25-28`；`notes/r06…:11`；`notes/r08…:12` | IDA |
| `CUIWorldSelect` ctor `sub_6246CE` | `CreateDlg(-140,-804,400,100,z:10,bScreenCoord:0)`，30 槽 id 基址 1000 | `notes/ida-coords.md:39-47,150`；`README.md:74` | IDA |
| `CUIChannelSelect` `sub_62B2CF` / `dword_BEDA68` | `CreateDlg(136,51,540,167,z:211,bScreenCoord:1)` | `notes/ida-coords.md:113-117,151`；`README.md:74` | IDA |
| `CUICharSelect` `0x60135B` | `CreateWnd(200,-1359,101,131,z:10,bScreenCoord:0)` | `notes/ida-coords.md:152`；`README.md:75` | IDA |
| `sub_60E157` / `sub_592992` / `sub_593A8D` | `(0,125,125,352,z210)` / `(275,100,250,314)` / `(275,100,250,343)` | `notes/ida-coords.md:153-155`；`README.md:75-76` | IDA |
| 角色列表窗口 `sub_603FF0` | `(-290,-1218,600,80,z10)` | `plan-charselect-remap.md:15` | IDA（类名未定） |
| `CUINewCharRaceSelect` `0x615596` | `CreateWnd(-372,-2128,700,460,z10,bScreenCoord:0)` | `notes/round2-hardening.md:143-150`；`README.md:165` | IDA |
| `CUICharDetailVAC` `0x60745A` | `SetOption(148,-710)`、`CreateFadeWnd(170,210,0,z=11)`、读 `CLogin+316` | `notes/round2-hardening.md:120`；`README.md:168` | IDA（用途待查） |
| 世界槽位 | 步进 X **27**、**30** 槽（`v63<960`, 960/32）、图标 **+5**、槽位 Y=0 | `notes/ida-coords.md:54-73`；`README.md:23` | IDA + 实测 |
| 容器规模 | **15 世界 / 60 频道**，每项 **684** 字节 | `notes/ida-coords.md:143-144`；`README.md:77`；`notes/charselect-node-schema.md:197-200` | IDA |
| 选角槽位 | 每行 **3**（`a4%3`）、步长 **130**、X `−220/−90/+40` | `notes/charselect-node-schema.md:167-181`；`README.md:105` | IDA |
| 详情面板 | `183×151`(占用)/`183×115`(空)、Y `1384/1348` | `notes/charselect-node-schema.md:170,180-181`；`README.md:106-107` | IDA |
| 三按钮 `sub_60148D` | `id 1000/1001/1002, x=4, y=-2/36/85`（窗口内）→帧内 `(204,-1361/-1323/-1274)` | `notes/charselect-node-schema.md:119-127`；`README.md:108`；`plan-charselect-remap.md:196` | IDA + 实测 38/49px |
| `GetStepHeight(2)` 换算 | `-1208`；`1384-1208=176`→屏幕 y=**482**（占用）/ `1348-1208=140`→**436**（空） | `README.md:110`；`plan-charselect-remap.md:230-236,253` | IDA + 桥面实测 |
| `CS_CANVAS_TO_FRAME_X` | **140**（刚性平移反推，无 IDA 依据） | `notes/round2-hardening.md:97-107`；`plan-charselect-remap.md:197` | 实测反推 [推断] |
| `CS_FRAME_ORIGIN` | **(−237,−54)**，`position = viewpos + CS_FRAME_ORIGIN` | `plan-charselect-remap.md:172-182` | 由选世界条幅反推 [推断] |
| `CS_LIST_T / SLOT_ROW_H / PAGESIZE / COLUMNS` | `−1218` / 80→**151** / 12→**3** / 4→**3** | `plan-charselect-remap.md:77-96,245`；`notes/round2-hardening.md:129-131` | IDA + 逐像素复验 |
| 容量常量 | `WORLD_BUTTON_COUNT=15`、`CHANNEL_BUTTON_COUNT=30`；`WS_SLOT_COUNT=30`、`WS_SLOT_STEP_X=27` | `notes/round2-hardening.md:50-51,63` | IDA |
| `WS_BANNER_Y` / `WS_BANNER_OBJ_Y` | **110** → `viewpos.y + (-857)` | `notes/ida-coords.md:177,206-208`；`README.md:30` | 双分辨率实测 111/135 |
| viewpos.y | 1280x720=**968**，1024x768=**992**（Δ=+24=Δviewh/2） | `notes/ida-coords.md:210-218` | 实测 |
| 登录页锚点 | 参考 `(28,GetStepHeight)` vs 本端 `(137,-20)`；`signboard_map_pos(137,21)` | `README.md:172`；`notes/r04…:28,36`；`r201-login-overlay-playbook.md:18-19` | IDA + 待实机叠图 |
| `TITLE_WINDOW_OFFSET` | **(58,16)**（7 按钮 NCC 6 个=1.000） | `evidence/login-panel-offset.md:43-53,69` | 参考客户端截图互相关 |
| `CUITitle::OnCreate` 控件 | BtLogin(213,-5)95x45 / IDSave(37,74)51x23 / IDLost(144,73)59x23 / PasswdLost(226,74)56x23 / BtNew(14,122)92x38 / HomePage(114,120)93x38 / Quit(214,120)81x40 / 账号(67,12)132x15 / 密码(67,40)120x15 / check(12,73) | `evidence/login-panel-offset.md:9-22`；`img-evidence-audit.md:38` | IDA `0x62054A` |
| 登录地图元素 | logo `397x219 o(198,109)`、sign `368x236 o(184,118)`、eff `238x298 o(0,0)` | `notes/ida-coords.md:27-31`；`evidence/MapLogin_logo_metrics.txt:1` | IDA dump |
| chBackgrn | `449×233`，origin `(224,116)` | `notes/ida-coords.md:95`；`img-evidence-audit.md:15` | 运行时 IMG |
| CHARLIST 包体 | 总长 **153** 字节；`level` 1B、`exp` 8B、**无尾 timestamp(int)** | `notes/charlist-parse-fix.md:18-43,59-61` | 抓包十六进制（与 `README.md:88-89` 冲突） |
| 建角字段 | 11 字段含 `hairColor`；hair 基础男 `30000/30020/30030` 女 `31000/31040/31050`；hairColor `0/2/3/7` | `notes/r09-login-flow-protocol.md:12,21-29` | s9 服务端源码 |
| SERVERSTATUS | `0` 正常 / `1` 拥挤 / `2` 满员 | `notes/r09-login-flow-protocol.md:31-32` | s9 源码 |
| 删除失败码 | s9 实发 `0x09/0x14/0x16/0x1A/0x1D/0`；从不下发 `0x0A/0x12` | `notes/r10-login-flow-missing-features.md:68-80` | s9 源码 |
| 登录 opcode | Recv `GENDER_DONE(4)/CONFIRM_EULA_RESULT(5)/CHECK_PINCODE(6)/UPDATE_PINCODE(7)/VIEW_ALL_CHAR(8)`；Out `VIEW_ALL_CHAR=13`；`REGISTER_PIC=29`；`PICK_ALL_CHAR=14`/`VIEW_ALL_WITH_PIC=32` | `notes/r10…:30-31,43,122,133`；`docs/features/pending-live-tests/README.md:435-436` | s9 源码 |
| `CS_CHARINFO_OFF` / Wait bar / charInfo origin | `(−93,−208)` / `(99,97)`（勿回退 46,98） / charInfo1 `183x151 o(45,57)` | `r201-login-overlay-playbook.md:21-22`；`img-evidence-audit.md:20-21` | 运行时 IMG + 叠图 knob |

## 9. Angel(13339, 083/v83 带符号 IDB) 新校准应优先复核的 15 个点
1. **IDB 溯源/版本纯度**：`ida-coords.md:3` 自称 `Maplestory95.i64` 同源产物，与 `feature-gap-inventory-2026-11/README.md:5` 「095 线不作本线参照」冲突；另有 13337/13338 两端点、`r09:4` BeiDouV17.7 base `0x400000`、`r10:5-6` `Gr2D_DX8.dll` 未加载。先用带符号 083 IDB 钉死 base、模块归属与所有 EA，确认上一轮坐标未混入 095/启动器。**（主 agent：已由 EA 交叉验证钉死，见 §0 复核块）**
2. **符号映射复核**：上一轮在「无 CUI* 符号、全 sub_*」下靠 vtable/xref 把 `sub_6246CE`→`CUIWorldSelect` ctor、`sub_60148D`→`OnCreate`、`off_AF6D90` slot3/slot11 等推断命名（`charselect-node-schema.md:97-100,109-117`）。带符号 IDB 应逐条验证/改名，尤其 `:210` 「`dword_BEDA4C` 列表面板类名尚未确定」与 `:205` 「`0x601790` IDA 未识别所属函数」。
3. **地址一致性消歧**：`CUICharDetail` ctor 出现 `0x602960`（`charselect-node-schema.md:160`、`plan-charselect-remap.md:27`）与 `0x60292F`（`round2-hardening.md:75`、`plan-charselect-remap.md:212`）两值；`CUIWorldSelect` ctor `0x6246CE` vs `OnButtonClicked 0x624AE8`/`IsRequestValid 0x62540A`（`r07:4`）；`CUIChannelSelect 0x62B2CF`。用符号表定唯一值。
4. **父窗口帧 / `UIOrigin` 换算规则**：`ida-coords.md:167,227-230`、`round2-hardening.md:86-93`、`login-panel-offset.md:24` 都卡在「父窗口原点无法唯一确定」「`CWndMan::GetOrgWindow(0x48BBA5)` 登录链路未见调用点」「`CWnd::CreateWnd(0x9DE4D2)` 无 UIOrigin 解析」。带符号 083 应能给出 `W4UIOrigin@CWnd` 枚举与父子窗口表，把 `nTop=-804/-1359/-2128/-2369` 的换算钉死，替换全部「实测反推」。
5. **CHARLIST `parse_stats` 字段宽度**：`README.md:88-89` 与 `charlist-parse-fix.md:43,47-51` 对同一修复描述互斥（int32×4+int64 vs level 2→1/exp 4→8/删尾 4B）。以 083 客户端 `LoginParser::parse_stats` 符号 + 真实包体重算 `charLook`/`0xFF` 终止符对齐。
6. **`+140` 与 `CS_FRAME_ORIGIN` 的真实来源**：`round2-hardening.md:97-107`、`plan-charselect-remap.md:172-182` 的 `CS_CANVAS_TO_FRAME_X=140`、`(-237,-54)` 均为实测刚性平移反推、明确「横向不可从 IDA 推出」。带符号 IDB 应给出 `CFadeWnd::SetOption` 与 `CreateWnd` 坐标系的真实偏移，替换或证伪这两常量。
7. **补间缓动与 `sub_43E7C7`**：`r04:29` 本端自写 `t²(3-2t)`；`img-evidence-audit.md:34` 「**未记载任何 S 曲线缓动**」、`:46` 判应线性。用符号 IDB 钉死 `CLogin::ChangeStep 0x5F53C0` 的补间命令语义与 `GetStepHeight(4)` 的 `this+133` 额外偏移（`r04:37` 本端未用 step4）。**（主 agent：Angel 已给出 `7000*(v63-a2)/v62/2` 常量速度，见 b20 §3）**
8. **step0 相机 X 与 `CLogin::Init 0x5F42CD`**：参考 `(28,GetStepHeight)` vs 本端 `(137,-20)`（`README.md:172`、`r04:36`、`feature-gap A10`）。先用符号给出 Init 准确常量，再决定是否需实机叠图（`r201:18` 列为 knob #1）。
9. **60 频道翻页 / View All / View Choice 语义**：`README.md:173`、`r05:56-58`、`r09:52` 称 exe 搜不到 `BtViewChoice` 字面量、语义无证据，但 `r05:56` 又记参考端 `CUIChannelSelect` 有 1000/1001 翻页按钮（60 频道）。带符号 IDB 定位 `CUIWorldSelect::OnButtonClicked 0x624AE8` / `CUIChannelSelect` 分支，确认两 stub 与 60 频道原生行为。**（主 agent：Angel 已给出 `OnButtonClicked`→`SendCheckUserLimitPacket` + `CUIRecommendWorld::OnCreate` 4 个 SP_ 节点，见 b20 §5）**
10. **opcode 4/5/6/7/8 与 PIN/VAC 的客户端侧证据**：r10 是**按 s9 服务端源码**注册/实现（`r10:5-6,13-26,30-36`），非客户端二进制取证。用 083 符号核对客户端 `RecvOpcode` 是否真有这些分支、`CHECK_PINCODE` mode 0~4、`VIEW_ALL_CHAR=8` 详情/概览首字节语义与 `BtCharacter`/`BtEnter` 调用点。**（主 agent：Angel `OnPacket 0x5f80ff` 分派表 case 0–28 全有，见 b20 §2）**
11. **`PICK_ALL_CHAR` 与 VAC 进角**：`parity-gap-2026-11/README.md:55` 记「修正误发 19→14/32」、`pending-live-tests:23,429-436` 仍 ⏳、`r10:167` `BtEnter` 仅关窗。复核 083 发包 opcode(14/32)、`VIEW_ALL_WITH_PIC` 及 `VIEW_ALL_CHAR` 概览→详情聚合条件。
12. **建角字段顺序（客户端侧）**：`r09:12,21-29` 的 11 字段含 `hairColor` 依据 s9 `CreateCharHandler.java`。用 083 `CreateCharPacket` 符号复核写序 `name→job→face→hair→hairColor→skinColor→top→bottom→shoes→weapon→gender` 与 hair 基础值/颜色增量。
13. **`CUICharDetailVAC` / `dword_BEDA4C` / `dword_BEDA48` 归属**：`README.md:168-169`、`charselect-node-schema.md:134-137,202-210` 三处「待定/归属未明/类未命名」。带符号 IDB 直接定名，并确认 `CLogin+316` 另一角色数组用途（是否 VAC/跨世界角色列表）。
14. **`OnStepChanged 0x5F5962` 时序与旧窗口 ghost**：`README.md:171`、`r08:70`、`img-evidence-audit.md:36` 记「参考端会把旧窗口一起滚出，本实现直接隐藏旧窗口，属已知差异」。用符号确认销毁/创建是否在补间结束后、旧窗口是否参与滚动绘制。
15. **选角信息板 `CUICharDetail::Draw 0x602ADC` 与 rank/jobrank**：`r10:103-107` 据「`CUICharDetail::Draw` 仅绘制职业/等级/人气/四维」判不臆造 RANK UI，但 `r15:121-125`、`:152`、`feature-gap A7` 又把「排行区文案未绘 / `CS_DETAIL_*` 硬编码 / Burning 重叠」列为缺口。用 083 符号 + IMG 复核 Draw 逐图元字面量、排行区是否原生存在、`CS_DETAIL_*` 真实值。

> 补充（非计数项）：登录链路**全程缺 orange-wz IMG 节点树证据与 layout JSON**（§7），与 skill 门禁 18 / 阶段 0「REA→IDA→orange-wz(IMG) 三联」不符；Angel 校准建议同步用 orange-wz 补 `UI/Login.img/*`、`MapLogin.img/back`、`Obj/login.img/Title/signboard` 的节点树（origin/帧序/UOL/控件矩形），把现有 `nx_inspect.py`/`[DBG-UI]` 文本转储升级为可复核节点证据。
