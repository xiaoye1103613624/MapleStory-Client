> 归档说明：本报告由「补终验」(2026-11-13) 生成，并于 2026-11-14 00:2x 按用户体感驳回（m01143）**整体重写**。运行侧原始证据（本端渲染帧 PNG / 各层 verdict JSON / packet_dump 片段 / 可复现脚本）已归档到 [`evidence/term-verify-v2/`](evidence/term-verify-v2/)，全部关键数值内联可独立复核。
>
> **本报告取代 2026-11-13 23:00 版**：那一版所有帧来自旧 exe `b406cac6`(21:11:58)，**不含用户 21:53 的「30」改动与 21:24 的核心窗口提交**，属于口径污染，已作废。

---

# 登录四屏 + 创建角色页 · 四层一致性终验报告（当前 HEAD 口径）

> 补终验（用户 m00563）：「要 ui、动画都要一致，代码还有实现逻辑和功能一致」
> 体感驳回（用户 m01143）：「对齐了？频道页我改成了30，我看登录页还有问题呢，角色属性ui也不对，创建角色ui对齐了？」
> 重写时间：2026-11-14 00:2x · 味道口径：📌 体感是输入，证据链才是交付；口径不是修复；candidate / 完成分离。

## 0. 口径声明（先钉死，不事后放宽）

- **当前二进制**：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` = sha256 前缀 `aa0aea67` / 23:39:37 / 5 107 200 B，`build_calibration.log` 零 error（构建命令 `MSYS_NO_PATHCONV=1 cmd.exe /c "E:\project\MapleStory-Client\build_calibration.bat"`，`BUILD_EXITCODE=0`）。它是当前 HEAD `46419e4f` 的确定性全量编译产物，**含 21:53 `806ae254`(CHANNEL_PAGE_SIZE=30U) 与 21:24 `886ce2c9`**。
- **作废口径**：上一版报告的 `b406cac6`(21:11:58 / 5 104 128 B) 早于上述两提交，用它签「CELL_W=94 双向像素闭环」不能回答「当前代码对齐了没」。本轮全部帧来自 `aa0aea67`，CELL_W 已在该二进制上复测（§2）。
- **仍是全量工作树**：exe 含 160+ 其它特性改动，非「只编 a90fda3 校准」的纯净二进制；像素证据语义 =「当前全量工作树渲染出来的画面」。
- **窗口标题进图 ≠ 取到游戏画面**；遮挡白页帧一律作废。入证据帧全部过 `real_gl_frame`（std>8、采样色>40）且经 `read_image` 目视确认。
- **日志单实例口径**：`recapture_all_v2.py` 记 `packet_dump.log` 起始 offset=2011574，只读本实例新增 3564 B；`serverlist_anchor_hit=true` / `charlist_hit=true`，`send_nonheartbeat` 完整链 `01`(登录 admin/admin) → `0b`(SERVERLIST_REQUEST) → `06`(ServerStatusRequest) → `05`(CHARLIST_REQUEST)。config 全程 head3=`b'# M'`（无 BOM）、`login.auto=false`。
- **本报告是证据链 candidate**，不是「登录四屏像素级修复完成」盖章。四层分别签，不签的列残留。

## 1. 取帧方法学（为什么这批帧可作证据）

| 路径 | 结果 | 处置 |
| --- | --- | --- |
| 内置截图键注入（PRINT_SCREEN / SCROLL_LOCK / PostMessage / SendInput-scancode / window 键 'C'） | 多次全败，exe 目录零新 PNG；`'C'` 软键路径亦不切屏（疑 KeyAction 映射吃掉字符） | 作废，换路 |
| `PrintWindow(PW_RENDERFULLCONTENT=2)` | 对 GL 返回静止合成图（已知坑） | 作废 |
| 自动链首跑 60 帧 | 全被 Cursor Agents 白色网页遮挡（mean≈242.61 / uniq=47 全同） | 作废 |
| **单实例 + 协议锚点 + force_game_top(最小化遮挡窗) + BitBlt(screen DC)** | **REAL_GL_FRAMES**（本端六屏 + 进图共 8+7 帧全 real） | **入证据** |

- 输入打通：x64 `INPUT` 结构须含 `MOUSEINPUT`（`sizeof(INPUT)=40`、`u.offset=8`），否则 `SendInput` 整批返 0；修复后用 `KEYEVENTF_UNICODE` 产 WM_CHAR 喂 SDL3 文本输入。`recapture_all_v2.py` 的 `clear_and_type()` 对账号框 `(720,308)`、密码框 `(720,336)` 各点框→15×VK_BACK 清空→UNICODE 重填，修掉首跑「密码错误！请确认！」七帧全废的根因（只填了密码框没显式填账号框）。
- 按钮前置条件：`UIWorldSelect::enter_world()` 在 `channelid==0xFF` 时 `return`（`UIWorldSelect.cpp:1530-1531`），必须先点 CH.1 选中才发 CHARLIST_REQUEST(05)。
- **创建按钮真值定位**：从 `Login.img` 的 `CharSelect/BtNew/normal/0`（101×35 ARGB8888，orange-wz 128B key）提取模板，alpha 掩码零均值 NCC 在清晰选角色帧上滑窗 → 峰 `ncc=1.0 @(815,246)`，中心 **(865,263)**；原肉眼估 `(866,265)` 仅差 1~2px。见 [`verdicts/btnew_locate.json`](evidence/term-verify-v2/verdicts/btnew_locate.json) + [`shots/btnew_overlay.png`](evidence/term-verify-v2/shots/btnew_overlay.png)。
- **创建页真因 = 点击时机**：`v2` 流程点 GOWORLD 后仅 1.2s 抓的 `v2_charselect` 是「空木牌过场帧」（按钮未激活），那一击被吞；`v2_racecreate` / `_settled`（2s 后）才是清晰选角色页。附着 settle 实例按真值中心点击一次命中 → [`shots/racecreate_click_f06.png`](evidence/term-verify-v2/shots/racecreate_click_f06.png) = UIRaceSelect（5 职业卡 + 冒险家介绍 + BtSelect 木牌）。

## 2. 逐屏回答「对齐了没」（用户 m01143 四问，逐条判死）

### 2.1 频道页「我改成了 30」 — 常量是真的，但「显示 30 格」不签

- **用户所指的 30** = `IO/UITypes/UIWorldSelect.h:44` `static constexpr uint16_t CHANNEL_PAGE_SIZE = 30U;`（配套 `:45 CHANNEL_CAPACITY=60U`、`:46 CHANNEL_BUTTON_COUNT=CHANNEL_PAGE_SIZE`），来自提交 `806ae254`(2026-11-13 21:53:35)，依据 Angel `CLogin::ChangeStep step1 sub_5FDD2D(15)/sub_5FDD2D(60)`：面板可视槽 30、>30 翻页。当前 exe `aa0aea67` 已编入此改动。
- **另一个独立的 30**：CHARLIST 包（本实例 offset 2011574）尾 5 B `02 1e 00 00 00` → `pic=2, slots(int32LE)=30`；`charcount=3 < slots=30` → `BtNew` 为 NORMAL 可点（`UICharSelect.cpp:1373-1376`），**不是槽满被禁**。两处 30 语义不同，别混。
- **实际渲染**（[`shots/v2_channel_grid.png`](evidence/term-verify-v2/shots/v2_channel_grid.png)）：本端服务端只发 3 频道 → 只有 CH.1/CH.2/CH.3 三格 1 行；目标 `targets/channel_selected.png` 是 4 列×5 行 20 格。gray NCC **0.932072**（[`verdicts/ncc_channel.json`](evidence/term-verify-v2/verdicts/ncc_channel.json)）、8×8 blocks mean 0.8783、15 热区全在下半屏（空的第 2~5 行，r4c3 −0.0024 / r4c4 0.0051）→ **频道数是数据状态差异，非坐标漂移**。
- **CELL_W=94 在当前 HEAD 二进制复测成立**：同一自相关算法（`verify_grid_step.py`，首行条带 y336-362 / x450-760）在 `v2_channel_grid.png` 上 `best_lag=94 corr=0.8177`；模板 NCC 峰 x=[458,552,646] → 步长 [94,94]。见 [`verdicts/grid_step.json`](evidence/term-verify-v2/verdicts/grid_step.json)（`overall=PIXEL_CLOSED_CELL_W`，目标帧 `best_lag=94 corr=0.7396`）。
- **CELL_H=32 仍无本端渲染实证**：本端只有 1 行，量不出行步长；保留目标帧 y=[337,369,401,433,465] + `UIWorldSelect.cpp:180-181` 常量对账的 caveat。
- **判词**：30 是容量/分页常量，已进当前二进制；「当前画面显示 30 格」不成立（服务端只给 3 个）。网格列步长 94 在当前代码实测仍对齐；行步长缺数据实证。

### 2.2 登录页「还有问题」 — 坐实，差在品牌内容不是位置

- 当前 HEAD 帧 `v2_login.png vs login.png`：gray NCC **0.952193**（R 0.958846 / G 0.953642 / B 0.965342）、8×8 blocks mean 0.9395、9 热区集中在 LOGO（r2c3 0.3529 / r2c4 0.3698 最低）与版本号（r0c6 0.7439）。见 [`verdicts/ncc_login.json`](evidence/term-verify-v2/verdicts/ncc_login.json)。
- **NO_MASK 逐 ROI**（[`verdicts/gap_audit_login.json`](evidence/term-verify-v2/verdicts/gap_audit_login.json)）：LOGO 区 **0.4195** / 版本号区 0.8265 / 木牌账号密码 0.736 / 三行链接 0.7652 / 下排按钮 0.8187；而背景树 0.9995 / 草地 1.0 / 右下品牌 1.0。
- **结构同位**（[`verdicts/gap_audit_char.json`](evidence/term-verify-v2/verdicts/gap_audit_char.json) `login_board_projection`）：木牌 ROI(620,270,940,465) 行投影 NCC **0.957** / 列投影 **0.8905** → 位置不漂。
- 旧帧 `01_login.png`（`b406cac6`）mask 掉 LOGO/版本/光标后 gray **0.987664**、row-projection 0.997046 / col-projection 0.991335（[`verdicts/login_masked.json`](evidence/term-verify-v2/verdicts/login_masked.json)）—— 同一「结构对、内容差」的独立佐证。
- **判词**：**「登录页还有问题」坐实 = 品牌 LOGO（本端 MapleStory 叶子 vs 目标 BeiDouMS）、版本号（本端 `Beta Version` / 无 `Ver. 0.83`）、木牌字色/输入态残留，不是位置漂**。要「像素级对齐」需替换品牌素材，属内容/资源改动，本轮校准未做 → **不签登录页已对齐**。

#### 2.2.1 根因钉到节点（orange-wz 128B key 实读，非推测）

- `E:\MXD\BeiDou-Client-xy\Data\UI\Login.img` 根节点 `list_children` = 19 个子节点，只有 `Title`（**无 `Title_new`**，与 `UILogin.cpp:82` 的 BeiDou 兼容注释一致）；`Title` 子节点里 `Title/MSTitle` 是 `CANVAS_PROPERTY`。
- `get_node_detail` 解出 `Title/MSTitle` = **397×219 ARGB8888**，目视 [`shots/wz_Title_MSTitle.png`](evidence/term-verify-v2/shots/wz_Title_MSTitle.png) = **"MapleStory" 字标 + 红枫叶 + "Beta Version" 小字全部烘在这张贴图里** → 品牌/版本差异是**资源内容**，不是代码坐标或常量。
- `Common` 的 `list_children` 共 12 个子节点（BtWselect/BtExit/shadow/selectWorld/step/loginlocation/BtStart2/BtEnableSPW/BtDisableSPW/SoftKey/BtStart/frame），**没有 `Common/version`**；`Common/frame` 是 1280×720 `CANVAS_PROPERTY`。→ `UILogin.cpp:77` / `:80` 两次 `version_pos = Login["Common"]["version"]["pos"]` 取到空节点，`:382 version.draw(position + version_pos - (0,5))` 落点无效；而 `Configuration.h:311 VERSION="0.83"` + `locale/*.ini:version.prefix=Ver. {}` 这层代码 Text 其实是对的，却因 WZ 缺节点没显示在右上角。
- **可执行修法（本轮未擅自执行）**：① 把 `Title/MSTitle` 替换为正确端 BeiDouMS 品牌贴图（需用户提供目标素材；live 改写须按 AGENTS.md §3 安全流：关客户端→staging→`save_as` 到异路径→原子替换）；② 给 `version_pos` 加 `Common/version` 缺失时的回退坐标（右上角），让既有 `Ver. 0.83` Text 层可见。二者都属**新增授权范围**，不擅自改 live 资源，故本轮只定位、披露、不签对齐。

### 2.3 角色属性 UI「也不对」 — 坐实，板宽一致但内容不一致

- 清晰常态帧 `charselect_clean_f05.png / v2_racecreate_settled.png`（3 角色 Admin/aaaaaa/Xiaoye2，选中中间）vs 目标 `char_select.png`（1 角色 Admin）：全图 gray **0.67476**、27 热区（r2c6 −0.6234）；当前 HEAD 的 `v2_racecreate` 亦 0.720698。见 [`verdicts/ncc_char.json`](evidence/term-verify-v2/verdicts/ncc_char.json)。
- **详情板几何**（[`verdicts/charselect_geom.json`](evidence/term-verify-v2/verdicts/charselect_geom.json)）：板边 shot=[521,736] / target=[364,579]，**宽 215 = 215 完全一致**；scene_bg ROI 0.9199、bridge 0.7585、sign_buttons 0.557。X 差 117px 因 3 角色选中中间位 vs 1 角色，属状态差异。
- **板内容对齐后仍不一致**（[`verdicts/gap_audit_char.json`](evidence/term-verify-v2/verdicts/gap_audit_char.json) `board_aligned_CORRECT_BBOX`）：按浅米色掩码量真实边界 shot=[467,140,736,363] w269 / target=[316,140,579,394] w263 后，`full=0.2292`、rows 0-33% 0.2545 / 33-66% 0.4323 / 66-100% **0.0244**（排行带最差）。
- ⚠️ **蓝军自查披露**：[`verdicts/gap_audit_login.json`](evidence/term-verify-v2/verdicts/gap_audit_login.json) 里的 `B_charselect_board_aligned`（`board_full=-0.026`）是早先**手猜裁框 (521,165,736,345)/(364,165,579,345) 造出的假证据**，已被 `gap_audit2` 的正确测量推翻；该字段作废，仅留作「裁错框会打脸」的记录，不得引用为结论。
- 逻辑层 `CS_DETAIL_*` 常量对账 Angel（职业[46,1]/等级[46,19]/人气[136,19]/力量[46,37]/智力[136,37]/敏捷[46,55]/运气[136,55]、排行 Y=99、BLACK×8）在 `verify_function.json` 10/10 签过；但**像素侧 18px 行距自动检测两端都不收敛**（本端 spacings `[46,23,12,13,11,10,19,17]` 混噪声、目标 `[15,12,11,46,15,26,11]`；`shot_has_18x2=true` 是噪声碰巧凑出）→ 明确不签，不硬凑。
- **判词**：**「角色属性 ui 也不对」坐实 = 板框几何一致（215=215）但内容（数值/字色/排行带/3角色vs1角色状态）高度不一致**。逻辑层常量对齐 ≠ 像素侧对齐，本轮不签。

### 2.4 创建角色 UI「对齐了？」 — 本端帧到手，无目标帧，不签对齐

- **功能可达**：`BtNew` WZ 真值 NCC=1.0 定位 (865,263) + 时序修正（等清晰选角色页 settle 再点）→ [`shots/racecreate_click_f06.png`](evidence/term-verify-v2/shots/racecreate_click_f06.png) = UIRaceSelect 落地（5 职业卡横排、冒险家介绍、700×460 窗框、BtSelect 木牌）。`racecreate_click` 7 帧全 real，mean 145.42→131.91（点击后显著切屏）→136.38→135.52（[`verdicts/racecreate_click.json`](evidence/term-verify-v2/verdicts/racecreate_click.json)）。`fg_match=true`、config 前后 `b'# M'`。
- 状态机源码：`UICharSelect.cpp:1164-1177` `case Buttons::BtNew:` → `deactivate(); tab_index=0; UI::get().emplace<UIRaceSelect>()`；`UIStateLogin.cpp:303-315 is_step_window` 含 RACESELECT/CLASSCREATION。
- **目标端缺失**：`correct_shots/` 7 张（login/channel_select/channel_selected/char_select/ingame/keymap/seik）**无创建角色目标帧**；审计文档 `create-char-classic-dual-path/notes/native-step4-layout.md` 自标「step4 屏幕绝对落点置信度低，推导值，明确标为决策点」。`806ae254` 只改 UIRaceSelect(88 行)，未碰 UILogin/UICharSelect/UICommonCreation。
- **判词**：**可签「创建角色页本端渲染帧到手 / 功能可达」，不可签「创建角色 UI 已像素对齐 Angel」**——缺目标端帧属外部阻塞（用户未提供），须披露而非脑补。

### 2.5 其余两屏（对账留证，不混入四问结论）

| 屏 | 当前 HEAD 帧 | 目标帧 | gray NCC | 判定 |
| --- | --- | --- | --- | --- |
| 选世界·卷轴收起 | `v2_world_collapsed.png` | `channel_select.png` | **0.975993**（blocks 0.9739，仅 2 热区 r0c0 0.757 / r0c6 0.7399） | 结构几乎完全同位 |
| 进图 | `screen_01.png`（旧口径帧，仅作状态说明） | `ingame.png` | **0.085227** | **NOT_COMPARABLE**（目标=蘑菇屋户外+「基本操作法」弹窗，本端=地下城室内无弹窗，场景状态根本不同，非坐标漂移） |

## 3. 动画层

| 项 | 证据 | 判定 |
| --- | --- | --- |
| 多帧动画活 | [`verdicts/seq_analysis.json`](evidence/term-verify-v2/verdicts/seq_analysis.json)：100 帧 / `total_animated_frames=92` / clusters_with_animation=2 / `MULTI_FRAME_ANIMATION_OBSERVED` | **PASS** |
| chSelect 红圈末帧常驻 | [`verdicts/chselect_anim.json`](evidence/term-verify-v2/verdicts/chselect_anim.json)：红像素 744/744/744、质心 [482.1,337.5] 零漂移、CH.1 ROI NCC=1.0/1.0 → `LAST_FRAME_PERSIST_VERIFIED` | **PASS** |
| 代码/IMG 对账 | `UIWorldSelect.cpp:497-518` 定格 `size()-1`、`:720-721` 非 playing 画末帧；IMG 真值 `chSelect /0 d100 /1 d50 /2 d50 /3 no-delay`；落点 (469,328) 与 `CH_SELECT_CIRCLE_OFFSET(-2,-9)`+末帧 origin(29,16) 自洽 | **PASS** |
| CharSelect/scroll/2 | IMG 真值 `217×30 d250 → 217×196 d50 → 217×195 d50 → 217×193 末帧常驻`；a90fda3 H5 调用方取末帧绘制 | **PASS**（逻辑层） |

- 诚实标注：红圈三帧 `bg_sky_mean_abs=0.0`（频道网格屏云背景本就静止），`bg_animation_alive=false` 不影响「末帧常驻」判定。
- 当前 HEAD 六屏帧 `v2_*` 序列 mean 130.39→129.53→144.60→152.04→151.83→139.55→143.92→144.35 是**屏间跳变**，不是同一屏内的动画；逐屏动画证据仍以上表为准。

## 4. 实现逻辑层

- [`verdicts/verify_function.json`](evidence/term-verify-v2/verdicts/verify_function.json) → **C_logic = ALL_PASS 10/10**：`channel_cell_step`(94/32)、`cell_offset_formula`、`charselect_angel_coords`、`charselect_rank_y99`、`charselect_black_text`、`scroll_frame_origin`、`animation_update_loops`、`charselect_lastframe_persist`、`login_ime_unicode`、`global_screenshot_keys`。
- 状态机构造点（源码钉死）：`Net/Handlers/LoginHandlers.cpp:254-286 ServerlistHandler::handle` → `emplace<UIWorldSelect>()`；`:288-340 CharlistHandler::handle` → `emplace<UICharSelect>(characters,charcount,slots,pic)`。选世界/选频道/选角色三屏代码可达；自动链跳过是 `get_auto_login()` 设计，不是没实现。
- 单实例协议链 `01 → 0b → 06 → 05`（[`verdicts/recapture_v2.json`](evidence/term-verify-v2/verdicts/recapture_v2.json)）串起登录→选世界→选频道→选角色，点击创建角色后再进 UIRaceSelect，逻辑全链走通。

## 5. 功能层

- `A_packet = TRAFFIC_OK`：packet_dump.log 14 595 行、SEND=9371 / RECV=5224；send_opcode_top `29`:7122 `BC`:1534 `18`:322 `CA`:67；本实例新增链 `01/0b/06/05`。
- `B_setfield = REACHED_GAME`：setfield_trace.log `Stage::load=11` / `fade done=11` / maps `104000200/104000300/104000400/914000000`。
- 窗口标题进图：`XiaoYE MS v0.0.1【3-aaaaaa】【2000-战童技能】【破功值：199999】...【在线时长：00 00:00:32】`。
- 当前 HEAD 六屏 `real_gl_frame=true` 全通过，创建角色页切换后 7 帧全 real。

## 6. 不能签的（candidate / 完成分离）

1. **「登录四屏像素级修复完成」不签**：登录页品牌/版本残留（§2.2）、角色属性板内容不一致（§2.3）、创建角色页无目标帧（§2.4）三处明确未对齐。
2. **CELL_H=32 本端渲染实证缺失**：服务端只发 3 频道 1 行。
3. **选角色页 18px 行距像素侧**：两种自动检测器两端均不收敛，硬凑当铁证踩事实红线；逻辑层常量对账已覆盖。
4. **纯净校准二进制**：exe 是全量增量编译含 160+ 特性，未做「只编 a90fda3」的隔离构建。
5. **创建角色页像素对齐**：缺 Angel 目标帧，属外部阻塞（用户未提供），披露不脑补。
6. **真值源口径未完全统一**：`notes/authority-switch-2026-11-12.md` 记「后续以 Angel 反编译为准，不以截图为准」，但频道 94/32 用截图 NCC 量出、选角色页用 Angel 字面量；本轮把「本端渲染帧 ↔ 目标帧」拉到同一张桌上，历史口径分裂的事实仍在。

## 7. 遗留与收尾

- **待实机叠图项**：README.md:386-391「实机五屏截图比对」可**部分销**（登录/选世界/选频道/选角色/创建角色五屏本端帧均到手并叠图），不能全销（进图需与目标帧同场景同弹窗态重取；创建角色需补目标端帧）。
- **git 卫生已收口**：分区 subagent `537bb49d` 自报「干净」不实（独立核 281 行 `??`），已用 `9684e7ea 🔧 chore(gitignore)` 二进制 ASCII 尾部追加 31 行规则覆盖，`git status --short` → 0；373 个已跟踪 `tools/_*` 未丢。本轮归档后再次核验 `git status` 干净。
- 分支 `feature/dev_0.0.1`、`a90fda3` 是 HEAD 祖先、分区 28 commits 核验通过（见上一版报告 §7，事实未变）。

## 8. 结论（四层分别签）

| 层 | 结论 | 证据强度 |
| --- | --- | --- |
| UI | **部分闭环**：选世界收起 0.976、频道格 CELL_W=94 当前 HEAD 复测闭环、登录页结构同位（投影 0.957/0.891）但品牌内容残留、详情板 215=215 但内容不一致、创建角色页本端到手无目标帧、进图状态不可比 | 强（当前 HEAD 真 GL 帧） |
| 动画 | **闭环**：92 动画帧 + chSelect 末帧常驻（744/744/744、ROI NCC 1.0）+ IMG 帧序/代码对账 | 强 |
| 实现逻辑 | **闭环**：10/10 ALL_PASS + 状态机构造点 + 单实例协议链 01→0b→06→05→UIRaceSelect | 强 |
| 功能 | **闭环**：TRAFFIC_OK + REACHED_GAME + 窗口标题进图 + 六屏全 real + 创建角色页切换成功 | 强 |

### 给用户 m01143 四问的一句话答案

> **没有全部对齐。** 频道「30」是容量/分页常量（已进当前 exe），但服务端只给 3 频道、画面只 3 格，列步长 94 当前代码实测仍对齐、行步长缺数据实证；登录页「还有问题」坐实（LOGO/版本号/字色品牌残留，位置不漂）；角色属性 UI「不对」坐实（板宽一致、内容不一致）；创建角色页本端能进、帧已抓到，但**没有 Angel 目标帧，无法签对齐**。四层里动画/逻辑/功能三层签，UI 层逐屏按上表分别签或不签——体感是输入，证据链才是交付，口径不拿来当修复。
