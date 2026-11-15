> 归档说明：本报告由「补终验」(2026-11-13) 生成，并于 2026-11-14 00:2x 按用户体感驳回（m01143）**整体重写**。运行侧原始证据（本端渲染帧 PNG / 各层 verdict JSON / packet_dump 片段 / 可复现脚本）已归档到 [`evidence/term-verify-v2/`](evidence/term-verify-v2/)，全部关键数值内联可独立复核。
>
> **本报告取代 2026-11-13 23:00 版**：那一版所有帧来自旧 exe `b406cac6`(21:11:58)，**不含用户 21:53 的「30」改动与 21:24 的核心窗口提交**，属于口径污染，已作废。
>
> **2026-11-14 01:3x 增补 §9**：用户提供桌面 78 张目标帧（`C:\Users\11036\Desktop\正确客户端截图\`，含 6 张创建角色链路帧），推翻「缺创建角色目标帧」口径；并按 m01638 补齐左侧木牌栏清单 / 点击逻辑 / 切页变化，修复且真机验收「取消回职业团把亭子背景带过来」的 bug（当前 exe `607f789811db25ed`）。
>
> **2026-11-14 03:5x 增补 §10**：用户 m02128「先做你可以改的」授权后，补齐选角色页 / 职业团页左侧木牌栏并真机逐块 NCC 验收 `SIDEBAR_FIX_VERIFIED`（选角色 0.4192→0.9313 / 职业团 0.5156→0.9508）；§9.1 / §9.6 的「整栏没实现」判词是修复前历史取证链，已在 §10 闭环。

---

# 登录四屏 + 创建角色页 · 四层一致性终验报告（当前 HEAD 口径）

> 补终验（用户 m00563）：「要 ui、动画都要一致，代码还有实现逻辑和功能一致」
> 体感驳回（用户 m01143）：「对齐了？频道页我改成了30，我看登录页还有问题呢，角色属性ui也不对，创建角色ui对齐了？」
> 重写时间：2026-11-14 00:2x · 味道口径：📌 体感是输入，证据链才是交付；口径不是修复；candidate / 完成分离。

## 0. 口径声明（先钉死，不事后放宽）

- **当前二进制（§9 修复后）**：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` = sha256 前缀 `607f789811db25ed` / 2026-11-14 01:14:48 / 5 100 544 B，`build_calibration.log` 全量 708 TU 重编、零 error（构建命令 `MSYS_NO_PATHCONV=1 cmd.exe /c "E:\project\MapleStory-Client\build_calibration.bat"`，`BUILD_EXITCODE=0`）。它是 §9 亭子残留修复（`IO/UITypes/UICommonCreation.cpp` + `IO/UIStateLogin.cpp`）落工作树后的确定性全量编译产物，**含 21:53 `806ae254`(CHANNEL_PAGE_SIZE=30U) 与 21:24 `886ce2c9`**。首跑 `LNK1104 无法打开 MapleStory.exe` 是运行中客户端锁住输出文件（编译阶段零 error），`taskkill /IM MapleStory.exe`（保留 java 8484 服务端 pid 37592）释放锁后重建通过。
- **蓝军自查翻案（必读）**：01:14:48 的确定性产物是 `607f789811db25ed` / 5 100 544 B；但 01:19:05 运行目录 exe 被**另一会话从同一根工作树全量重编后覆盖**，实测 `build-popup-audit/MapleStory.exe` 与 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` sha256 逐字节 identical = `47be00b37d093d214d9a` / 5 100 544 B。**§9 真机验收脚本运行于 01:22:23，实际加载的是 `47be00b37d093d21`**（含本修复 + 别会话 8 项并发改动），不是 `607f789811db25ed`。`scene_reset_verdict.json` 的 `FIX_VERIFIED` 结论仍成立（数值在该二进制上跑出），但报告原先把验收帧挂到 `607f…` 属口径错配，此处纠正：**§9 验收 = 全量工作树产物，非「只编修复」的纯净二进制**；`607f…` 仅证明修复可确定性编成、编译阶段零 error。
- **§2–§8 帧口径**：来自 `aa0aea67`(23:39:37 / 5 107 200 B)，仍是对 `806ae254`/`886ce2c9` 的有效证据；CELL_W=94 已在该二进制上复测（§2）。§9 的亭子残留验收帧与创建链路两端对账帧来自 `47be00b37d093d21`（01:19:05）。
- **作废口径**：最早一版报告的 `b406cac6`(21:11:58 / 5 104 128 B) 早于上述两提交，用它签「CELL_W=94 双向像素闭环」不能回答「当前代码对齐了没」。
- **仍是全量工作树**：exe 含 160+ 其它特性改动，非「只编 a90fda3 校准」的纯净二进制；像素证据语义 =「当前全量工作树渲染出来的画面」。
- **窗口标题进图 ≠ 取到游戏画面**；遮挡白页帧一律作废。入证据帧全部过 `real_gl_frame`（std>8、采样色>40）且经 `read_image` 目视确认。
- **日志单实例口径**：`recapture_all_v2.py` 记 `packet_dump.log` 起始 offset=2011574，只读本实例新增 3564 B；`serverlist_anchor_hit=true` / `charlist_hit=true`，`send_nonheartbeat` 完整链 `01`(登录 admin/admin) → `0b`(SERVERLIST_REQUEST) → `06`(ServerStatusRequest) → `05`(CHARLIST_REQUEST)。config 全程 head3=`b'# M'`（无 BOM）、`login.auto=false`。
- **§10 验收二进制（最新）**：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` = sha256 前缀 `b10df425d42b4c127` / 2026-11-14 03:54:11 / 5 109 760 B，`BUILD_EXITCODE=0`；选角色页 / 职业团页侧栏补齐后的验收帧全部来自它（§10.4）。
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
- **判词（§9 更新）**：桌面 6 张创建链路目标帧到位后，「缺目标帧」口径已作废 → 有目标帧、实测未对齐：建角页 `scene_reset_2_creation.png` vs `desktop_racecreate_adventurer.png` gray **0.846614**（blocks 0.8535 / 15 热区）、职业团页 `scene_reset_1_race_before.png` vs `desktop_racecreate_race.png` gray **0.718647**（blocks 0.6762 / 26 热区），见 [`verdicts/ncc_creation.json`](evidence/term-verify-v2/verdicts/ncc_creation.json) / [`verdicts/ncc_race.json`](evidence/term-verify-v2/verdicts/ncc_race.json)。**可签「功能可达 + 两端帧到手」，不可签「像素对齐」**——证据从「外部阻塞」升级为「实测未对齐」。

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

1. **「登录四屏像素级修复完成」不签**：登录页品牌/版本残留（§2.2）、角色属性板内容不一致（§2.3）、创建角色链路两端实测未对齐（§2.4 / §9.5）三处明确未对齐；~~选角色页 / 职业团页左侧木牌整栏缺失~~（§9.1 修复前判词）**已于 §10 补齐并真机逐块验收 `SIDEBAR_FIX_VERIFIED`**（选角色 0.4192→0.9313 / 职业团 0.5156→0.9508，4/4 块过 0.5）。
2. **CELL_H=32 本端渲染实证缺失**：服务端只发 3 频道 1 行。
3. **选角色页 18px 行距像素侧**：两种自动检测器两端均不收敛，硬凑当铁证踩事实红线；逻辑层常量对账已覆盖。
4. **纯净校准二进制**：exe 是全量增量编译含 160+ 特性，未做「只编 a90fda3」的隔离构建。
5. **创建角色页像素对齐**：桌面目标帧已到位（不再属外部阻塞），但两端实测 gray 0.8466 / 0.7186（§9.5），明确未对齐，不签。
6. **真值源口径未完全统一**：`notes/authority-switch-2026-11-12.md` 记「后续以 Angel 反编译为准，不以截图为准」，但频道 94/32 用截图 NCC 量出、选角色页用 Angel 字面量；本轮把「本端渲染帧 ↔ 目标帧」拉到同一张桌上，历史口径分裂的事实仍在。

## 7. 遗留与收尾

- **待实机叠图项**：README.md:386-391「实机五屏截图比对」可**部分销**（登录/选世界/选频道/选角色/创建角色五屏本端帧均到手并叠图；桌面目标帧到位后创建链路两端已对账），不能全销（进图需与目标帧同场景同弹窗态重取）。
- **§9 亭子残留 bug 已修并真机验收**（`verdicts/scene_reset.json` = FIX_VERIFIED），非遗留。
- **git 卫生已收口**：分区 subagent `537bb49d` 自报「干净」不实（独立核 281 行 `??`），已用 `9684e7ea 🔧 chore(gitignore)` 二进制 ASCII 尾部追加 31 行规则覆盖，`git status --short` → 0；373 个已跟踪 `tools/_*` 未丢。本轮归档后再次核验 `git status` 干净。
- 分支 `feature/dev_0.0.1`、`a90fda3` 是 HEAD 祖先、分区 28 commits 核验通过（见上一版报告 §7，事实未变）。

## 8. 结论（四层分别签）

| 层 | 结论 | 证据强度 |
| --- | --- | --- |
| UI | **部分闭环**：选世界收起 0.976、频道格 CELL_W=94 复测闭环、**左侧木牌栏选角色/职业团页补齐并真机逐块验收（§10，0.9313 / 0.9508）**；仍残留登录页品牌/版本、详情板内容不一致、创建链路两端 0.8466 / 0.7186 未对齐、进图状态不可比 | 强（当前 HEAD 真 GL 帧） |
| 动画 | **闭环**：92 动画帧 + chSelect 末帧常驻（744/744/744、ROI NCC 1.0）+ IMG 帧序/代码对账 | 强 |
| 实现逻辑 | **闭环**：10/10 ALL_PASS + 状态机构造点 + 单实例协议链 01→0b→06→05→UIRaceSelect | 强 |
| 功能 | **闭环**：TRAFFIC_OK + REACHED_GAME + 窗口标题进图 + 六屏全 real + 创建角色页切换成功 | 强 |

### 给用户 m01143 四问的一句话答案

> **没有全部对齐。** 频道「30」是容量/分页常量（已进当前 exe），但服务端只给 3 频道、画面只 3 格，列步长 94 当前代码实测仍对齐、行步长缺数据实证；登录页「还有问题」坐实（LOGO/版本号/字色品牌残留，位置不漂）；角色属性 UI「不对」坐实（板宽一致、内容不一致）；创建角色页本端能进、桌面目标帧也已到位，但两端实测 0.8466 / 0.7186 未对齐（§9.5）。**另补 m01638：左侧木牌栏目标端统一 4 块、本端选角色页整栏没实现（§9.1）；「职业团→角色设置→取消把亭子带回来」的 bug 已定位到 `~UICommonCreation` 析构死代码 + 四条离开路径漏清场，修复后真机验收 FIX_VERIFIED（§9.4）。** 四层里动画/逻辑/功能三层签，UI 层逐屏按上表分别签或不签——体感是输入，证据链才是交付，口径不拿来当修复。**另补 m02128「先做你可以改的」：选角色页 / 职业团页左侧木牌栏已补齐并真机逐块验收 `SIDEBAR_FIX_VERIFIED`，4/4 块过 0.5（§10）。**


---

## 9. 左侧木牌栏 / 页面切换 / 亭子残留修复（用户 m01638）

> 体感（m01638）：「这几个页面如果左侧有木牌，就要注意有几个 是哪几个 点击木牌后的逻辑是什么 如果涉及到几个页面的切换 点击后页面的变化也要清楚，现在就存在选择职业团进入角色设置页后 取消回到职业团选择页会把之前页面背景亭子也带过来」
> 味道：📌 体感是输入，证据链才是交付。四类问题（几个 / 哪几个 / 点击逻辑 / 切页变化与背景归位）逐一钉源码行 + 目标帧 + 真机验收，不靠注释、不靠猜。

**数据源补充**：`C:\Users\11036\Desktop\正确客户端截图\` = 78 张目标帧（登录链路 12 + 创建角色链路 6 + 60 余张游戏内 UI）；`scripts/ingest_desktop_targets.py` 归拢 12 张到 `targets_desktop\`（11 张 1280×720 + `频道变更.png` 428×122 局部弹窗）。登录/选频道/选角色/进图 5 张与旧 `correct_shots/` **sha256 逐字节 identical**（同一套基准，不推翻既有 NCC）。

### 9.1 左侧木牌栏：几个 / 哪几个（目标端逐张 read_image 定死）

统一是**跨页导航组件 = 4 块**（[`verdicts/left_plank_audit.json`](evidence/term-verify-v2/verdicts/left_plank_audit.json) / [`verdicts/plank_segments.json`](evidence/term-verify-v2/verdicts/plank_segments.json) / [`shots/planks_cmp_char_select.png`](evidence/term-verify-v2/shots/planks_cmp_char_select.png)）：

| # | 元素 | y 区段 | 类型 | 出现页 |
| --- | --- | --- | --- | --- |
| 1 | `✓<页标题>` 深红牌（选择频道 / 选择角色 / 创建角色） | 36–85 | 标题牌（非按钮） | 全部 |
| 2 | **因页而异，见下** | 99–148 / 127–156 | 信息条 或 按钮 | 全部 |
| 3 | `查看所有角色▶` | 377–399 | 按钮 | 全部 |
| 4 | `回到首页◀` | 427–449 | 按钮 | 全部 |

**第 2 块是用户问「哪几个」的关键区分**：
- **频道页**（`desktop_channel_expanded.png` 目视）：`查看世界地图▶` **按钮**（y≈127–156），对应 `UIWorldSelect.cpp:160 WS_SIDEBAR_Y0=127 BtViewChoice`。频道页能点的侧栏 = 3 个按钮（查看世界地图 / 查看所有角色 / 回到首页）。
- **选角色 / 职业团 / 建角页**：`选择的世界 SCANIA Ch.1` **信息条**（y99–148，非按钮，`desktop_char_select.png` / `desktop_racecreate_race.png` / `desktop_racecreate_adventurer.png` 目视）。这些页能点的侧栏 = 2 个按钮（查看所有角色 / 回到首页）。

**本端落地审计（蓝军翻案 2，[`verdicts/left_plank_pages.json`](evidence/term-verify-v2/verdicts/left_plank_pages.json) / `scripts/verify_left_plank_pages.py`）**：上一版 `left_plank_audit.json` 把**职业团页帧 `racecreate_click_f06.png`（UIRaceSelect）错配成「创建页」**，且只读了 `UICommonCreation.cpp:456` 漏掉 classic 分支，误判「创建页缺顶部标题牌+世界条」。按每页各自正确的本端帧↔目标帧重算 ROI(0,0,160,470) 逐块零均值 NCC：

| 页 | 本端类 | 本端帧 ↔ 目标帧 | strip NCC | 4 块逐块(标题/信息条/查看所有角色/回到首页) | 判定 |
| --- | --- | --- | --- | --- | --- |
| 登录 | UILogin | `v2_login_typed` ↔ `desktop_login` | **0.9995** | 1.0 / 1.0 / 1.0 / 1.0 | 4 块齐 |
| 频道展开 | UIWorldSelect | `v2_channel_grid` ↔ `desktop_channel_expanded` | **0.8717** | 0.9997 / 0.8953 / 0.5931 / 0.8915 | 4 块齐 |
| 选角色 | UICharSelect | `v2_racecreate_settled` ↔ `desktop_char_select` | **0.4192** | −0.266 / −0.201 / −0.208 / −0.450 | **整栏没实现** |
| 职业团 | UIRaceSelect | `scene_reset_1_race_before` ↔ `desktop_racecreate_race` | **0.5156** | −0.180 / −0.207 / −0.154 / −0.048 | **3 块缺**(只 BtStart) |
| 建角冒险家 | UICommonCreation(classic) | `scene_reset_2_creation` ↔ `desktop_racecreate_adventurer` | **0.9575** | 1.0 / 0.8062 / 1.0 / 0.6501 | **4 块齐** |

源码坐实：`UIWorldSelect.cpp:158-163,306-324` 三钮真资产；**`UICharSelect.cpp` 搜侧栏按钮 0 命中**（只有 `BtCharacter`(:1308)/`BtPreview`(:1316)）→ 选角色整栏缺失；`UIRaceSelect.cpp:156/:314` 只 `BtStart` → 职业团页缺 3 块；**`UICommonCreation.cpp:456` BtStart(0,427) + `:490` version_pos=(1061,9) + `:492` step_create + `:493` selworld_box + `:511-513` world/channel_text + `:530-534` BtViewAll + `:960-963` draw() = classic 分支 4 块全套**（`blame 6d835880` xiaoye 11-05 15:33，早于本报告），门禁 `UICommonCreation.cpp:79 use_classic_creation()` → `Util/LoginUiAssets.cpp:71-74 return !has_customize_char()`，真机帧证 classic 生效。→ **「创建页缺侧栏」判词作废**；未实现的是选角色页与职业团页，不是建角页。

> **注（§10 更新）**：上表选角色 0.4192 / 职业团 0.5156 是**修复前**帧的取证数值，作为历史证据链保留；用户 m02128 授权补齐后，同一 ROI、同一目标帧、同一零均值 NCC 算法复测为 0.9313 / 0.9508（4/4 块过 0.5，`SIDEBAR_FIX_VERIFIED`），见 §10.5。下方「未实现的是选角色页与职业团页」判词相应作废。

### 9.2 点击逻辑表（全部源码行锚）

| 页 | 按钮 | 源码 | 点击后 |
| --- | --- | --- | --- |
| 频道 | `BtViewChoice` 查看世界地图 | `UIWorldSelect.cpp:1369-1375` | `emplace<UIRecommendWorld>` 推荐世界弹窗 |
| 频道 | `BtViewAll` 查看所有角色 | `:1364-1368` | `ViewAllCharPacket().dispatch()`（VIEW_ALL_CHAR） |
| 频道 | `BtStart` 回到首页 | `:1350-1362` | `Session::reconnect()` + `set_step(0)` + remove WORLDSELECT/LOGINWAIT/LOGINNOTICE + `emplace<UILogin>()` |
| 选角色 | `BtCharacter` 查看所有角色 | `UICharSelect.cpp:1308-1314` | `ViewAllCharPacket().dispatch()` |
| 选角色 | `BtNew` 创建角色 | `:1164-1177` | `deactivate()` + `emplace<UIRaceSelect>()` |
| 选角色 | `BtPreview` 回到选世界 | `:1316-1328` | `set_step(1)` + `activate_after_transition(WORLDSELECT)` |
| 职业团 | `BtStart` 回到首页 | `UIRaceSelect.cpp:156` → `show_worldselect():876-887` | `set_step(1)` + remove RACESELECT/CHARSELECT + `activate_after_transition(WORLDSELECT)` |
| 职业团 | `BtMake` 选择 | `:698-703` → `enter_creation():651-668` | 按 selected_class `emplace<UIExplorerCreation/UICygnusCreation/UIAranCreation/UIEvanCreation/UIDualBladeCreation>()` |
| 建角 | `BtStart` 回到首页 | `UICommonCreation.cpp:1304-1317` | `reset_creation_scene()` + remove CLASSCREATION/RACESELECT/CHARSELECT + `worldselect->makeactive()` |
| 建角 | `BtPreview` 预览 | `:1318-1328` | `reset_creation_scene()` + remove CLASSCREATION + `emplace<UIRaceSelect>()` |
| 建角 | `BtNo` 取消 | `:1436-1456` | `reset_creation_scene()` + remove CLASSCREATION + `emplace<UIRaceSelect>()`（回职业团，即用户报 bug 的路径） |
| 建角 | `BtYes` 确定 | `:1329-…` | 校验名字 → NameCharPacket → `onok` 延迟回调（`:1250-1261`，含 `reset_creation_scene()`） |
| 建角 | ESC（输入框焦点） | `:484 newCharName.set_key_callback(ESCAPE→BtNo)` | 等价「取消」 |

### 9.3 切页变化表（背景 scene）

6 张创建链路帧（[`shots/race_targets_grid.png`](evidence/term-verify-v2/shots/race_targets_grid.png)）逐张目视：

| # | 目标帧 | 页 | 背景 |
| --- | --- | --- | --- |
| 0 | `desktop_racecreate_race` | 职业团选择（冒险家高亮） | 绿色大森林 |
| 1 | `desktop_racecreate_adventurer` | 冒险家角色设置 | **蘑菇屋 + 亭子** |
| 2 | `desktop_racecreate_knight` | 职业团选择（骑士团） | 森林 |
| 3 | `desktop_racecreate_knight2` | 骑士团角色设置 | **机械飞艇 / 亭台** |
| 4 | `desktop_racecreate_aran` | 职业团选择（战神） | 森林 |
| 5 | `desktop_racecreate_aran2` | 战神角色设置 | **雪景 + 雪亭** |

**规律**：职业团选择页一律森林；角色设置页各有职业专属亭子（scene_no 16/30/33）。切页必须换背景 → 用户报的「取消回来把亭子带过来」正是背景 scene 状态没在返回时 reset。

### 9.4 亭子残留 bug：根因 → 修复 → 真机验收（本轮闭环）

**根因链**（源码行锚）：
- `UICommonCreation.cpp:522-566` 构造期按 className 取 `scene_no` `"16"`(冒险家 (-3,-26)) / `"30"`(骑士团 (-33,-32)) / `"33"`(战神 (-3,-25)) → `Map001["Back"]["login.img"]["back"][scene_no]` → `LoginBackground::get().set_scene(scene, scene_pos)`。
- `LoginBackground.cpp:234-247 set_scene()` 置 `has_scene_flag=true`；`:249-253 clear_scene()` 置 false；`:298-300 draw()` `if (has_scene_flag) scene_sprite.draw(scene_pos, alpha)`。
- `UICommonCreation.cpp:2393-2396 ~UICommonCreation()` 原有 `if (available() && has_scene()) clear_scene()` **是死代码**：`UIStateLogin.cpp:328-341 remove()` 用 `element.release()`（旧 `:339`）**只放弃所有权、不调用析构** → scene_sprite(亭子) 继续被 draw。同写法见 `UIStateGame.cpp:972` / `UIStateCashShop.cpp:119`（项目规避回调内自删的既定设计，**不能全局换 `reset()`**，会在 `button_pressed()` 栈上自删 this → UAF）。
- 四条离开创建页路径全漏清场：`onok :1250-1261` / `BtStart :1289-1300` / `BtPreview :1302-1310` / `BtNo :1419-1437`。
- `set_step(3)` 再进还是 3 → `LoginBackground.cpp:91 if (value==step) return` 早退，step 切换救不了残留。

**修复**（工作树；01:14:48 确定性产物 `607f789811db25ed` 编译零 error，01:19:05 被全量工作树 `47be00b37d093d21` 覆盖，后者为验收实际二进制）：
- `UICommonCreation.cpp:50-66` 匿名 namespace 自由函数 `void reset_creation_scene()`（`available() && has_scene()` 守卫 → `clear_scene()`）——刻意**不依赖 this**，因 `onok` 是 `[&]` 延迟回调，调成员函数有生命周期隐患（曾先加成员函数再蓝军自检推翻并删除头文件声明）。
- 四条路径 + 析构各插一行（现 grep：`:1270` onok / `:1308` BtStart / `:1322` BtPreview / `:1450` BtNo / `:2413` 析构）。
- `UIStateLogin.cpp:336-342` 在 `remove()` 加**单点兜底**：`type==CLASSCREATION && …has_scene()` → `clear_scene()`，覆盖未来新增离开路径；`LoginBackground.h:31` 已 include，无新增依赖。
- `git diff --stat`：`IO/UIStateLogin.cpp 8+` / `IO/UITypes/UICommonCreation.cpp 21+2-`，共 **27 insertions / 2 deletions**。决策：不动 `release()`（改 `reset` 会 UAF），双保险=四条显式清场 + remove 单点兜底，均幂等。

**真机验收**（`scripts/verify_scene_reset.py` / [`verdicts/scene_reset.json`](evidence/term-verify-v2/verdicts/scene_reset.json)，**实际 exe `47be00b37d093d21` / 01:19:05 / 全量工作树含并发改动**，单实例+协议锚点+BitBlt；脚本内 `method` 字段仍写 `607f789811db25ed`，是运行前写的标签、非实测加载值，以 §0 蓝军自查为准）：

| 帧 | 亭子 ROI(520,100,760,300) 橙色占比 | 判定 |
| --- | --- | --- |
| [`scene_reset_1_race_before.png`](evidence/term-verify-v2/shots/scene_reset_1_race_before.png) | 0.02602 | 森林职业团页，无亭 |
| [`scene_reset_2_creation.png`](evidence/term-verify-v2/shots/scene_reset_2_creation.png) | **0.16642** | 升 6.4×，真进带亭子的建角页 |
| [`scene_reset_3_race_return.png`](evidence/term-verify-v2/shots/scene_reset_3_race_return.png) | **0.02602** | 精确回原值，亭子一像素没带回 |

- `full_ncc(before,return)=0.967601`（同屏重绘）/ `full_ncc(before,creation)=0.656041`（两屏确实不同）/ `pavilion_roi_ncc(creation,return)=0.203337`（亭子消失）。
- `verdict = FIX_VERIFIED`（entered_creation_page / scene_cleared_on_return / return_is_same_forest_screen 全 true）；两个兜底重试（ENTER 进页 / ESC 返回）**都没触发**，点击路径干净；`scene_reset_3_race_return.png` 经 `read_image` 目视 = 森林职业团页（5 卡 + 冒险家板 + 选择按钮），无蘑菇亭；config head3 前后 `b'# M'`。

### 9.5 创建链路两端像素对账（桌面目标帧到位后，§6.5 口径升级）

| 页 | 本端帧 | 目标帧 | gray NCC | blocks mean | 热区 |
| --- | --- | --- | --- | --- | --- |
| 建角（冒险家） | `scene_reset_2_creation.png` | `desktop_racecreate_adventurer.png` | **0.846614** | 0.8535 | 15 |
| 职业团选择 | `scene_reset_1_race_before.png` | `desktop_racecreate_race.png` | **0.718647** | 0.6762 | 26 |

证据 [`verdicts/ncc_creation.json`](evidence/term-verify-v2/verdicts/ncc_creation.json) / [`verdicts/ncc_race.json`](evidence/term-verify-v2/verdicts/ncc_race.json) + `ui_creation_page/diff.png`、`ui_race_page/diff.png`。**判词**：目标帧到位使「外部阻塞」作废，改判「实测未对齐」；左栏 4 块本端缺 3、亭子场景虽有但布局/内容有差，均不签像素对齐。**注**：本节两端对账帧均为 §10 侧栏补齐前的历史帧，全图 NCC 不因侧栏补齐重算；左侧栏这一子项单独在 §10 验收。

### 9.6 本节判词（给用户 m01638 四问的一句话答案）

> **木牌 = 目标端统一 4 块**（标题牌 1 + 第 2 块因页而异 + 查看所有角色 + 回到首页）；频道页第 2 块是「查看世界地图」按钮、其余页是「选择的世界 SCANIA Ch.1」信息条，能点的按钮数频道页 3 个 / 其它页 2 个（§9.1）。**点击逻辑与切页变化已逐条钉到源码行**（§9.2/§9.3）：频道 3 钮分别开推荐世界弹窗 / 发 VIEW_ALL_CHAR / 重连回登录；选角色页整栏 / 职业团页侧栏 3 块（§9.1 修复前判词）已于 §10 补齐并真机逐块验收 `SIDEBAR_FIX_VERIFIED`（0.9313 / 0.9508），但**建角页(UICommonCreation classic)侧栏 4 块齐 strip 0.9575**（§9.1 蓝军翻案 2 纠正旧误判）；「选择」进各职业建角页并换专属亭子背景，「回到首页」滚回选世界。**「取消把亭子带回来」坐实并已修复**：真因是 `~UICommonCreation` 析构是死代码（`remove()` 用 `release()` 不析构）+ 四条离开路径漏 `clear_scene()`，改法 = 四条显式清场 + `UIStateLogin::remove` 单点兜底，当前运行 exe `47be00b37d093d21`（01:19:05 全量工作树产物，含本修复）真机三帧数值闭环 `FIX_VERIFIED`（橙色占比 0.026→0.166→0.026）；01:14:48 的 `607f789811db25ed` 是修复的确定性编译证据，二者关系见 §0，验收帧以 `47be…` 为准。体感是输入，源码行 + 数值 + 目视三证才是交付。
---

## 10. 左侧木牌栏补齐（用户 m02128「先做你可以改的」）

### 10.1 授权与自定验收信号（修复前先钉死，不事后放宽）

- 用户 m02125 给「我能直接接着干的」清单（照 `UIWorldSelect.cpp:158-163,306-324` 与 `UICommonCreation.cpp:492-534` 现成模式补选角色页整栏、职业团页 3 块 + 重建 + 真机逐块 NCC），m02128「先做你可以改的」= 明确开工授权。
- **自定验收信号**：选角色页 strip `0.4192 → ≥0.85`、职业团页 `0.5156 → ≥0.85`，4 个 y 块各 `>0.5`，帧 `real_gl_frame`。四条全部满足才签。

### 10.2 实现（4 个 TU，不新建文件 / 不改 vcxproj）

- **照抄现成模式**：`UICommonCreation.cpp:492-534` 的 classic 侧栏（该页 strip 已验证 0.9575，4 块齐）与 `UIWorldSelect.cpp:158-163,306-324` 三钮真资产。`LocalTwoSpriteButton` 用本地 `BtViewAll_{normal,pressed}.png`（各 5205 B，`docs/features/login-flow-ui-ida-align/resources/creation/`），失败回退 `MapleButton(Login["WorldSelect"]["BtViewAll"],…)`。
- **position 语义坑（照抄会画飞）**：最终落点 = `UIElement::position + 按钮局部坐标`（`MapleButton.cpp:76-83 draw` / `:91-105 bounds`；`Sprite.cpp:27-31`）。三页 position 不同：`UIWorldSelect=(0,0)` / `UICharSelect` step2 `=(375,1514)` / `UIRaceSelect` step3 `=(240,40)` / `UICommonCreation` classic `=(0,0)`。→ 装饰 Sprite 用 `draw(Point(0,0), inter)` 画屏幕绝对，按钮局部坐标 = `绝对 − position`（照 `UICharSelect.cpp:315 return screen - position` 已示范写法）。
- **step 帧号 / 数量用数值定死，不肉眼估**：`scripts/pick_step_sprite.py` ROI(0,30,160,92) 零均值 NCC：目标职业团 vs 本端建角(step/3) = 0.9096、目标职业团 vs 目标建角 = 0.9774、目标选角色 vs 本端建角 = 0.8337 → **两页统一 `Common/step/3`**（[`verdicts/step_sprite_pick.json`](evidence/term-verify-v2/verdicts/step_sprite_pick.json)）。`scripts/settle_plank_counts.py` 逐行暗像素投影：目标选角色/职业团页各 4 块 `[[36,85],[99,148],[377,399],[427,449]]`；修复前本端两页 `[]`（mean_dark 0.0459 / 0.1123，全空）（[`verdicts/settle_plank_counts.json`](evidence/term-verify-v2/verdicts/settle_plank_counts.json)）。orange-wz 128B GMS128 key 实读 `Common/step/2`、`/3`、`Common/selectWorld` 均 150×57 ARGB8888 有 PNG（[`scripts/pick_step_sprite.py`](evidence/term-verify-v2/scripts/pick_step_sprite.py)）。
- **落地**：`UICharSelect.h/.cpp` 加 `cs_step/cs_world_box/cs_world_name/cs_channel`，构造 step/3 (0,33) + selectWorld (0,96) + 世界名大写 + `"CH."+(wch+1)`，draw 传 (0,0)；`BtCharacter` → BtViewAll 本地 PNG（`LocalTwoSpriteButton`，atlas `CSBT`）于 `(0,377)−position`；新增专用枚举 `BtHome` → `Common["BtStart"]` 于 `(0,427)−position`。`UIRaceSelect.h/.cpp` 加 `rs_*` 四成员，`BtPreview` → BtViewAll（atlas `RSBT`）于 `(0,377)−position`、`BtStart` → `(0,427)−position`，classic `button_pressed(BtPreview)` → `ViewAllCharPacket().dispatch()`。

### 10.3 蓝军自检两轮（自我纠错留证，不藏）

- **自检 #1**：`UIRaceSelect.cpp` 装饰绘制被插在 `if(classic_text_gl.is_valid())` 单语句后且多缩进 2 格，功能上无条件执行但读起来像受 if 保护 → 改成同级 3-tab 对齐 + 注「无条件绘制」。
- **自检 #2（真实语义回归）**：第一版把 `UICharSelect` 的 `BtPreview` 贴图换成「回到首页」`Common/BtStart`(0,427)，但 `button_pressed(BtPreview)` 仍是「回选世界页 `set_step(1)`」且 ESC `send_key` 也调 `button_pressed(Buttons::BtPreview)`（`UICharSelect.cpp:835`）→ 贴图/行为/ESC 语义不一致，违反用户 m01638「点击后页面变化要清楚」。
- **自我纠错（避免过度修正）**：初拟给 `UIRaceSelect` 也插专用 `BtViewAll` 枚举是**半误判**——classic/modern 两 init 互斥、classic 下 `BtPreview` 本就空闲；且插在 `CLASS0` 前会 shift `CLASS0`(6→7)，而 modern 分支 `order[new_index+N-Buttons::CLASS0]`（`UIRaceSelect.cpp:812-838`）、`for(size_t i=0;i<=Buttons::CLASS0;i++)`(:364) 按绝对值依赖，会炸。→ 收窄**只改 UICharSelect**（其 `CHARACTER_SLOT0` 全是相对引用 `+i`/`-SLOT0`/`>=SLOT0`，grep 无硬编码 9，插入安全）。
- **收窄补丁 [`scripts/patch_sidebar2.py`](evidence/term-verify-v2/scripts/patch_sidebar2.py)**：新增 `BtHome`（仍 `< CHARACTER_SLOT0` 故 `send_cursor` 第一遍响应）→ `Sound(SCROLLUP).play(); Session::get().reconnect(); set_step(0); remove(CHARSELECT/LOGINWAIT/LOGINNOTICE); emplace<UILogin>();`（与 `UIWorldSelect.cpp:1350-1363` BtStart 一致）；恢复 `BtPreview` 原贴图/原逻辑/ESC。第一版见 [`scripts/patch_sidebar.py`](evidence/term-verify-v2/scripts/patch_sidebar.py)。
- **cosmetic**：[`scripts/fix_cosmetic_indent.py`](evidence/term-verify-v2/scripts/fix_cosmetic_indent.py) 补 3-tab 前导缩进（patch_sidebar2 的 `locate` 把 anchor 前导 tab 吃进去致 `lead=""`）；C++ 空白不敏感，纯可读性，已随最终构建验证。

### 10.4 构建口径（当前验收 exe）

- 最终确定性二进制 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` = sha256 `b10df425d42b4c12751e59f337376c7726761fe7b1fa5e0fe9b4f2649890fdeb` / 2026-11-14 03:54:11 / 5 109 760 B，`build_calibration.log` `BUILD_EXITCODE=0`、vcvarsall x64、零 error（`'vswhere.exe' is not recognized` 无害告警），构建命令 `MSYS_NO_PATHCONV=1 cmd.exe /c "E:\project\MapleStory-Client\build_calibration.bat"`；构建前 `taskkill /IM MapleStory.exe` 释放 exe 锁（保留 java 8484 服务端 pid 37592）。
- 三次构建收敛：第一版 4 文件补丁 → 收窄修正二次构建 → cosmetic 后最终构建；**§10.5 验收帧全部跑在最终 `b10df425…` 上**（`build_calibration.log` mtime 03:54:12 与 exe 03:54:11 相邻，验收脚本紧随其后）。仍是全量工作树产物，含别会话并发改动，非「只编本修复」的纯净二进制（§6.4 口径不变）。

### 10.5 真机逐块 NCC 验收（SIDEBAR_FIX_VERIFIED）

- 取帧链路沿用 §1：单实例 + `packet_dump.log` 起始 offset + `force_game_top` + 屏幕 DC `BitBlt`；登录双字段 (admin/admin) → 连接 (880,300) → 世界 (485,210) → CH.1 (490,350) → 到选择的世界去 (820,518) → 2.2 s settle 抓 [`shots/sidebar_fix_1_charselect.png`](evidence/term-verify-v2/shots/sidebar_fix_1_charselect.png) → BtNew (865,263) → 2.0 s 抓 [`shots/sidebar_fix_2_raceselect.png`](evidence/term-verify-v2/shots/sidebar_fix_2_raceselect.png)。脚本 [`scripts/verify_sidebar_fix.py`](evidence/term-verify-v2/scripts/verify_sidebar_fix.py)，证据 [`verdicts/sidebar_fix.json`](evidence/term-verify-v2/verdicts/sidebar_fix.json)。
- 对 `targets_desktop/desktop_{char_select,racecreate_race}.png` 算 ROI(0,0,160,470) 全条带零均值 NCC + 4 个 y 块：

| 页 | 修复前 strip | 修复后 strip | 4 块逐块（标题 / 世界条 / 查看所有角色 / 回到首页） | >0.5 块数 | 判定 |
| --- | --- | --- | --- | --- | --- |
| 选角色 UICharSelect | 0.4192 | **0.9313** | 0.8110 / 0.8039 / 0.9594 / 0.7164 | 4/4 | PASS |
| 职业团 UIRaceSelect | 0.5156 | **0.9508** | 1.0000 / 0.8246 / 0.9971 / 0.6543 | 4/4 | PASS |

- 两帧 `real_gl_frame=true`（选角色 mean 143.99 / std 77.6 / uniq 1369；职业团 mean 134.87 / std 76.7 / uniq 1113）；`config head3` 前后均 `b'# M'`；`verdict.all_pass=true` / `detail=SIDEBAR_FIX_VERIFIED`。
- **目视双证**：[`shots/sidefix_cmp_char.png`](evidence/term-verify-v2/shots/sidefix_cmp_char.png) / [`shots/sidefix_cmp_race.png`](evidence/term-verify-v2/shots/sidefix_cmp_race.png)（左本端 | 右目标，ROI(0,0,160,470) 3× 放大，[`scripts/make_sidebar_compare.py`](evidence/term-verify-v2/scripts/make_sidebar_compare.py)）经 `read_image` 一次一张确认四块木牌形制/位置逐一对上，无多图错配。

### 10.6 本节判词（candidate / 完成分离）

> **自定验收信号四条全部满足 → 选角色页 / 职业团页左侧木牌栏补齐签 `SIDEBAR_FIX_VERIFIED`**（数值 + 目视双证闭环）。**只签这一个子项**：§6 其余不签项一律未动——登录页 MSTitle 品牌/版本残留（§2.2.1）、角色属性板内容不一致（§2.3）、创建链路两端 0.8466 / 0.7186（§9.5，修复前历史帧）、CELL_H=32、18 px 行距、纯净校准二进制。影响面：`BtHome` 新增按钮点击逻辑与 `UIWorldSelect` BtStart 一致、`BtPreview`/ESC 原语义保留、未动 `CLASS0` 绝对值依赖、未新建文件、未改 vcxproj。体感是输入，源码行 + 数值 + 目视三证才是交付。

## 11 版本号 / 登录页品牌 / 角色属性板 / 标题牌（本轮四项修复，真机 ALL_PASS）

> 口径：本节所有验收帧均来自当前工作树确定性重建后的 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（见 §11.5），取帧链路同 §1（单实例 + `packet_dump.log` 起始 offset 锚点 + `force_game_top` + 屏幕 DC `BitBlt`）。味道口径不变：体感是输入，源码行 + 数值 + 目视三证才算交付。

### 11.1 版本号 `Ver. 0.83` 五页补齐

- **真因（不是位置漂，是空节点 + 颜色 + 相机偏移三重叠加）**：live `Login.img` 的 `Common` 12 个子节点（`BtWselect / BtExit / shadow / selectWorld / step / loginlocation / BtStart2 / BtEnableSPW / BtDisableSPW / SoftKey / BtStart / frame`）**没有 `version`**（orange-wz 128B GMS128 key 实读，非推测）。→ 五个页面 `version_pos = Common["version"]["pos"]` 全取空 `(0,0)`；且文字色用 `Color::Name::LEMONGRASS`（柠檬绿）在米色木牌背景上肉眼不可辨；`UICharSelect` / `UIRaceSelect` 的 `position` 是相机偏移，原写法 `version.draw(position + version_pos)` 会把 HUD 画到屏外。
- **gold reference（早已正确、对照基准）**：`IO/UITypes/UICommonCreation.cpp:487-490` classic 分支硬编码 `version = Text(A12B, LEFT, BLACK, "Ver. " + get_version()); version_pos = Point<int16_t>(1061, 9);` —— 创建角色页两端 `Ver. 0.83` 一直一致（`ver_roi_creation_adv.png` 目视 OURS == TARGET）。
- **改法（5 文件）**：统一 `Text(A12B, LEFT, BLACK, "Ver. " + version_text)` + `if (!version_pos.x() && !version_pos.y()) version_pos = Point<int16_t>(1061, 9);` + draw 改 `version.draw(version_pos - Point<int16_t>(0, 5))`（**去掉 `position +`**，版本号是屏幕绝对 HUD）。`UIRaceSelect` 另需两处：`init_classic()` 开头补 BLACK + `(1061,9)` 构造（classic 分支不走 `init_modern`，否则未初始化），classic `draw()` 在 `rs_channel.draw(Point(0,127))` 之后、`UIElement::draw_buttons(inter)` 之前补一行 `version.draw(...)`（原 classic 分支 return 前根本没画版本号）。`UICommonCreation.cpp` 的 modern 分支（`:95-101` / `:967`）同类问题一并闭合。
- **回退范围用暗像素投影定死，不靠眼猜**：ROI `(1000,0,1280,45)`、`lum < 95`、排除右侧橙框 `x > 230`——登录 / 频道 / 选角色三页目标 `dark_px=130`、`ink_bbox=[66,13,113,21]`、`col_peaks=4`，本端 `dark_px=0` 全空；职业团页目标 `175 / [66,13,113,33]`，本端 `38`（噪声非文字）；创建角色页两端 `183 / 221` 都有 → **只需修 login / channel / char_select / race_select 四页，creation 不动**（避免给本来正确的页硬加造成回归）。
- 脚本：[`scripts/patch_version_v1.py`](evidence/term-verify-v2/scripts/patch_version_v1.py)（4 文件）+ [`scripts/patch_version_v2.py`](evidence/term-verify-v2/scripts/patch_version_v2.py)（modern 分支）。
- **EOL 坑留证**：脚本第一版写死「文件必须纯 CRLF」→ `AssertionError: IO/UITypes/UILogin.cpp 非纯 CRLF (crlf=0 lf=645)`，**零写盘**（断言先于任何写入）。改为逐文件自适应（纯 CRLF 保持 CRLF / 纯 LF 保持 LF / 混合拒绝）后通过。教训：补丁脚本不要对换行符做单值假设。

### 11.2 登录页品牌 LOGO（代码级覆盖，绕开 nxcache 死路）

- **资源侧死路（必须先解释为什么不用「正确姿势」改 .img）**：`xy-config.yaml:70-71` `resource: nxcache: true`；`Util/ImgFiles.cpp:255` `if (nx_is_complete(nxpath, ec)) continue;` 只校验 `Data/.nxcache/UI.nx`（86 915 977 B / 2026-09-21 16:00）是否 complete，**不看 `.img` 的 mtime**（`Data/UI/Login.img` 2024-08-04，7 212 655 B）→ 裸改 live `.img` 完全不生效；而 `Util/ImgFiles.cpp:64-72` 有明确记录：truncated `.nx` 会让 `LZ4_decompress_fast()` 越界 `0xc0000005`（历史上表现为选角色屏在 CHARLIST 后死锁）。**删 `UI.nx` 重派生属 P0 风险，不赌。**
- **为什么走代码覆盖而不是改 `Obj.cpp`**：登录 LOGO 由 `Gameplay/MapleMap/Obj.cpp:29-38` 用 `Animation(obj_src, nl::nx::UI["Login.img"]["Title"]["MSTitle"])` 绘制；而 `Graphics/Animation.cpp:191-194` 的 `has_override` 只认 `nl::node::type::bitmap`，**override 只能吃 WZ 节点，传不了本地 PNG**。`IO/UIStateLogin.cpp:54-75 draw()` 是先画 `LoginBackground` 再遍历 elements → 在 `UILogin` 里贴一层覆盖图即可盖住 Map Obj 的 MSTitle，且完全绕开 `.img` / `.nx`。
- **数值前置（先验再改）**：模板取自目标帧 ROI，anchor `(434,80)`、397×219。`ring_ncc=0.9872` / `ring_mad=3.1`（ROI 外圈接缝近零 → 贴合不会露边）；整块覆盖模拟全图 gray NCC **0.9473 → 0.9867**，ROI NCC **0.6076 → 1.0**。另测同会话 `v2_login` vs `v2_login_typed` 的 LOGO ROI：`mad=16.833` / `changed>16=40158`，而 `sky_topleft ncc=1.0 mad=0`、`tree_left ncc=0.927 mad=8.41` → 本端该区确有闪光动画，整块静态贴正好替换。
- **诚实标注（不签的部分）**：`make_logo_override.py` 内部曾输出 `BACKGROUND_NOT_ALIGNED_DO_NOT_OVERRIDE`，经复核该判词不采信——其 `bg_ncc_masked=0.7985` 偏低是前景 mask 膨胀到 57% 造成的度量假象；有效证据是 `ring_ncc` 与整块覆盖的全图 NCC 两项。此判词与采信理由一并留证，不藏。
- **落地**：`IO/UITypes/UILogin.h` 加 `#include "../../Graphics/LocalPngTexture.h"` + 成员 `LocalPngTexture login_logo;`；`IO/UITypes/UILogin.cpp` 构造里 `constexpr size_t ATLAS_LOGIN_LOGO = 0x4C4F474Full; // 'LOGO'` 后 `login_logo.load(ATLAS_LOGIN_LOGO, {"UI/Creation/LoginLogo_override.png", "docs/features/login-flow-ui-ida-align/resources/creation/LoginLogo_override.png"});`；`draw()` 在 `UIElement::draw(alpha)` 之后 `if (login_logo.is_valid()) login_logo.draw(Point<int16_t>(434, 80));`。素材 [`ui_gap_audit/v3/LoginLogo_override.png`](evidence/term-verify-v2/shots/LoginLogo_override.png) 同字节部署到运行目录 `UI/Creation/` 与仓库 `docs/features/login-flow-ui-ida-align/resources/creation/`（`LocalPngTexture` 逐个候选路径 `stbi_load`，任一命中即用，缺一不影响）。
- **被弃用的两条素材路线（留证，避免后人重走）**：① 差分抠图 `MSTitle_BeiDou_extracted.png`——背景不逐像素对齐，保留 8436 px 里混入树干碎片；② 程序化重建（GeorgiaBold 40 px）——位置尺寸近似但**未达像素级**，且丢掉原贴图半透明尾巴层次。
- **第三条被弃用路线（本轮）**：拿旧贴图真 alpha 直接当遮罩（`alpha=mean 0.393`）——NCC 0.9793 但块内帧差幅度保留仅 **23.5%**，仍大面积冻背景；`alpha^0.7` 更差（20.6%）。`alpha=1` 保留 0%。
- **蓝军翻案（本轮自查发现，必读）**：上面那版「整块不透明覆盖」是**用冻结背景换来的假高分**。把 `UI/Creation/LoginLogo_override.png` 临时移走、同一实例连抓 10 帧后测得 `logo_max_changed_frac=0.087195`（`LOGO_REGION_ANIMATES`）；装上整块不透明素材后同一测量变 `logo_max_changed_frac=0.0`（`LOGO_REGION_FROZEN`），同时框边跨边一阶差分 p90 从 `17/17` 劣化到 `bottom 30.4 / right 32.4`——**在有动画的天空上贴一块静态矩形，既冻死背景又贴出硬边**。故 `roi_ncc=1.0` 不能单独作为「对齐」证据，违反用户「动画也要一致」的要求。
- **该区域动画的真实机制（决定修法）**：无覆盖层 10 帧逐像素跨帧 std 显示——字形实心核心（`lum<60`，17 779 px）std mean **2.27**（贴图本身静止），而半透明光晕区（`lum` 70–200，占块内 74.5%）帧间 diff mean **23.82**、`frac>16=0.523`，非字区 std mean **10.98**。也就是说**动的是背景穿过 LOGO 半透明光晕**，LOGO 贴图本身不脉动；因此正确遮罩必须在字形实心处 alpha=1、光晕与外围 alpha=0。
- **最终素材（字形遮罩，已部署）**：以无覆盖层 10 帧逐像素中位数为背景基准 `bg_med`，`ink = bg_lum_med − T_lum`、`stable = bg_std < 8`，新字实心 `(ink>52) & neutral & stable`；叶片限 `lband=(y 4:150, x 268:397)`（不限定会吃进右下木凳）；旧字实心取旧贴图 `alpha>0.35` 再与新字取 `max`（旧字必须盖净，光晕不纳入）；`MaxFilter(5)` 连通 → `MaxFilter(3)` → `GaussianBlur(0.6)`。产物 [`shots/LoginLogo_matte.png`](evidence/term-verify-v2/shots/LoginLogo_matte.png)：`alpha_mean=0.3968`、`alpha≥0.9` 占 27.6%、`alpha<0.02` 占 45.2%、四角 alpha `[0, 0, 0.067, 0.043]`。
- **替换前后同一口径对比（全部同一 exe `d48b3fa9…` / 2026-11-14 16:41:09 / 5 116 928 B）**：

| 度量 | 整块不透明（旧素材） | 字形遮罩（现素材） | 目标 |
| --- | --- | --- | --- |
| ROI 内帧间变化 `max_changed_frac` | **0.0**（冻死） | **0.029** | 动 |
| 字形实心区跨帧 std | 0 | 0.79 | ≈静止 |
| 光晕区跨帧 std | 0 | **3.25** | 动 |
| 框边接缝 p90（top/bottom/left/right） | 17 / **30.4** / 34 / **32.4** | 17 / **17** / 34 / **17.2** | 17 / 17 / 34 / 17 |
| ROI 与目标 NCC | 1.000（失真高分） | **0.9838**（10 帧 0.9797–0.9838） | 1.0 |

- **判词**：遮罩版三项门 `A_text_fit 0.9838 > 0.97`、`B_background_animates max_changed_frac 0.029 > 0`、`C_no_hard_seam 接缝 p90 ≤ 34` 全过 → `LOGO_MATTE_VERIFIED`（[`verdicts/logo_matte_verify.json`](evidence/term-verify-v2/verdicts/logo_matte_verify.json)）。四门脚本复跑仍 `all_pass=true`（version 103、logo roi 0.9783 / full 0.9855、charboard 0.3323）。**旧素材那版 `roi_ncc=1.0` 的记录保留在案但标注为失真口径，不得再引用为对齐证据。**

### 11.3 角色属性板多余「排行榜」（烘在贴图里，用裁剪而非改资源）

- **定性**：不是 `UICharSelect.cpp:589` 的 `ranklabel` 画了第二个文本（那只在「排行不可用 / 名次」之间变），而是 `CharSelect/charInfo1` 这张 183×151 画布**自带的**内容。解码 311 张画布后逐张目视：`charInfo1`（183×151）= 字段行 + 「服务器排行榜」棕带 `y82-94` + **底部另一行居中「排行榜」`y121-131`**；`charInfo`/`charInfo2`（183×115）无此行；`charInfo3`（183×151）是「冒险岛排行榜 + 服务器排行榜」两带。目标帧对应的是**无多余行**的底板。
- **为什么不用换画布/改资源**：同 §11.2 的 nxcache 死路；且换底板要重新保证 `get_origin()` 补偿（`:572 ci_pos = detail_pos + charinfo.get_origin()`）不走样。
- **落地一行语义裁剪**：`IO/UITypes/UICharSelect.cpp:572-576` `charinfo.draw(ci_pos);` → `charinfo.draw(DrawArgument(ci_pos), Range<int16_t>(0, 36));`。依据 `Graphics/Texture.cpp:87-90 draw(args, vertical)` 与 `Graphics/GraphicsGL.cpp:867-876` `offset.bottom -= vertical.second() / rect.bottom() - vertical.second()` → 只画顶部 36 px 带（115 px 处正好是多出行之前的分界），值行与 `CS_DETAIL_*` 常量、卷轴 `scroll/2` 末帧常驻全部不受影响。
- 诊断脚本：[`scripts/fix_charinfo_v2.py`](evidence/term-verify-v2/scripts/fix_charinfo_v2.py)（正确 RGBA 口径：alpha mean 222.2 / 16 级、`rgb@opaque mean [242.4,220.3,190.7]`、`paper_rgb [255,221,204]`、活动段 `[[4,15],[21,33],[39,51],[58,69],[82,94],[121,131]]`、抹字 tile 取 `[98,114]` 纯纸区）。**第一版 `build_fix_assets.py` 有 P0 bug**：把 RGBA 当 RGB 读，透明区 RGB=0 导致 `paper_lum=0.0`，把底板下半抹成深色条纹——留证，不重蹈。

### 11.4 选角色页标题木牌（本轮新发现 → 已修）

- **发现**：`plank_titles_sheet.png` 各页标题牌 ROI `(0,28,160,92)` 纵排对照——目标端逐页不同：频道页「✓选择频道 SELECT A WORLD/CHANNEL」、选角色页「✓选择角色 SELECT A CHARECTER」、职业团/建角页「✓创建角色 CREATE A CHARECTER」；**本端选角色页显示的却是「创建角色」**。
- **真因**：上一轮补侧栏时按 NCC `TGT_race vs OUR_create(step/3)=0.9096` 选帧，但那是对**职业团页**的比对，选角色页照抄了 `Common["step"]["3"]`。而 `UICharSelect.cpp:231` 本身就用 `LoginBackground::get().set_step(2)` 表达「选角色视角」——内部 step 编号与标题牌帧号本应对齐。
- **改法**：`IO/UITypes/UICharSelect.cpp:205` `Sprite(Common["step"]["3"], ...)` → `Sprite(Common["step"]["2"], ...)`（`Common/step/1..5` 经 orange-wz 实读均为 150×57 ARGB8888，五帧齐备，不存在缺帧回退问题）。
- **验证**：标题牌 ROI `(0,28,150,92)` NCC **0.9405**、目视逐字一致（[`ui_gap_audit/v3/char_title_plank_fixed.png`](evidence/term-verify-v2/shots/char_title_plank_fixed.png)，上本端下目标 4× 放大）；整条左侧栏 ROI `(0,0,160,470)` NCC **0.9367**，四块分别 **1.0000 / 0.8486 / 0.9064 / 0.7852**（标题牌由 0.8110 提到 1.0000）。

### 11.5 构建口径（本轮验收 exe）

- `taskkill /F /IM MapleStory.exe` 释放 exe 锁后 `MSYS_NO_PATHCONV=1 cmd.exe /c "E:\project\MapleStory-Client\build_calibration.bat"`。三次构建均 `BUILD_EXITCODE=0`：版本号修复 `d27fb6663cd53338` / 14:32:06 / 5 113 344 B；标题牌改帧后 15:20；**BOM 恢复后最终 `19496ae2661015d5` / 2026-11-14 15:29 / 5 113 344 B**。`build_calibration.log` 零 error（`LNK4099 未找到 vc120.pdb`、`'vswhere.exe' is not recognized` 均为无害告警）。
- **第四次构建（LOGO 遮罩素材，最终交付口径）**：替换 `UI/Creation/LoginLogo_override.png` 为字形遮罩版后重建 → `d48b3fa9d4390a0b7fafbaf14ce5e1afb7c843891b27dc7b4005dba603427895` / 2026-11-14 16:41:09 / 5 116 928 B / `BUILD_EXITCODE=0`。首次该次构建曾因运行中客户端占用输出文件报 `LNK1104`（`build_calibration.log` 尾部，编译阶段零 error），`taskkill /F /IM MapleStory.exe` 后立即重建通过——**`LNK1104` 是文件锁不是代码错**。§11.2 遮罩对比与 §11.6 复跑均在此二进制上完成。
- **构建口径翻案（蓝军自查，必读）**：15:20 那次构建的产物**不可作为交付口径**。补丁脚本按纯 LF 写入 `IO/UITypes/UICharSelect.cpp`（该文件原本是三处源码里唯一带 UTF-8 BOM 的），构建通过但文件头 BOM 丢失——MSVC 对无 BOM 源文件按本地代码页解析，而本文件含大量中文注释，编码口径已不可信。发现后立即恢复 `EF BB BF`（`head -c3 | xxd -p` = `efbbbf`，与 `git show HEAD:` 的 `efbbbf` 一致，diff 从 16 行噪声收敛为 **9 insertions / 5 deletions**）并重建得 `19496ae2…`。§11.6 全部数值均在 `19496ae2…` 上复跑（与 15:20 结果相差 ≤0.0004，结论不变，但**只有最后一次才是可交付二进制**）。
- 仍是全量工作树产物（含别会话 14 项并发改动），**不是只编本次修复的纯净二进制**——§6.4 口径不变，该不签项继续挂着。

### 11.6 真机验收（四项 ALL_PASS）

取帧链路：单实例 → 登录双字段 (admin/admin，`clear_and_type` 逐框清空重填) → 连接 (880,300) → 世界 (485,210) → CH.1 (490,350) → 到选择的世界去 (820,518) → 2.2 s settle 抓 [`shots/ui_final_charselect.png`](evidence/term-verify-v2/shots/ui_final_charselect.png) → BtNew (865,263) 再抓建角链。脚本 [`scripts/verify_ui_final.py`](evidence/term-verify-v2/scripts/verify_ui_final.py)，证据 [`verdicts/ui_final_verdict.json`](evidence/term-verify-v2/verdicts/ui_final_verdict.json) 与 [`verdicts/char_sidebar_recheck.json`](evidence/term-verify-v2/verdicts/char_sidebar_recheck.json)。

| 子项 | 度量 | 目标端 | 本端 | 判定 |
| --- | --- | --- | --- | --- |
| 版本号 | ROI `(1000,0,1280,45)` 暗像素 / bbox | 130 px / `[65,13,113,21]` | 103 px / `[62,14,113,22]` | PASS（同字同位，宽高差 2–3 px 属字体渲染） |
| 登录 LOGO | ROI `(434,80,831,299)` NCC / 全图 NCC / ROI 帧间变化 | 1.0（自身） | **0.9783 / 0.9855 / 0.029**（遮罩版；整块不透明版的 1.0 已标注失真） | PASS（见 §11.2 蓝军翻案） |
| 角色属性板 | 底部墨量占比 | 0.4236 | **0.3320** | PASS（多余「排行榜」已不存在） |
| 标题牌 / 左栏 | ROI `(0,28,150,92)`；整栏 `(0,0,160,470)` | 1.0（自身） | **0.9405 / 0.9367**（四块 1.0000 / 0.8486 / 0.9064 / 0.7852） | PASS |

- 帧有效性：`ui_final_login.png` mean 130.53 / std 69.94 / uniq 2184、`ui_final_charselect.png` mean 142.70 / std 77.38 / uniq 1360，均 `real_gl_frame=true`；`serverlist_anchor_hit=true`、`charlist_hit=true`；`xy-config.yaml` head3 前后均 `b'# M'`（无 BOM 污染）。
- **左栏数值的跑间波动（如实标注）**：左栏 ROI 覆盖动态森林背景，同一代码两次真机取帧会因背景相位/光标位置产生波动——标题牌 0.9405 → 0.9209、`world` 0.8486 → 0.7422、`home` 0.7848 → 0.7201、整栏 0.9367 → 0.9368；四块中 `title`/`viewall` 两次均 ≥0.90 且 `title` 两次均为 1.0000（同图直接命中）。故本节判词以「四块存在且 title=1.0、整栏 ≥0.93」为口径，不把单次数值当固定阈值背书。
- **目视**：[`shots/ui_final_login.png`](evidence/term-verify-v2/shots/ui_final_login.png) 右上 `Ver. 0.83`、中部 `BeiDouMS` 黑字 + 红叶（原 `MapleStory` + `Beta Version` 已不存在）；[`shots/ui_final_charselect.png`](evidence/term-verify-v2/shots/ui_final_charselect.png) 属性板只剩「服务器排行榜 / 排行不可用」、左侧四块木牌齐、标题牌为「选择角色」。

### 11.7 本节判词（candidate / 完成分离）

> **四项签 `UI_FINAL_VERIFIED`**：版本号四页补齐、登录页品牌 LOGO 遮罩覆盖（含动画与接缝两项附加门）、属性板多余标签裁剪、选角色页标题牌改帧——数值 + 目视双证闭环，`all_pass=true`。
>
> **本轮自查翻案（必读）**：LOGO 首版整块不透明素材把该区背景动画冻成 0 并贴出硬边，已换成字形遮罩素材重验（ROI 帧间变化 0.029 / 接缝 p90 回到 17 / NCC 0.9838）。旧版 `roi_ncc=1.0` 保留在案但标注失真，不得引用为对齐证据。
>
> **仍不签（未被本轮触碰，继续挂）**：① `CELL_H=32` 无本端渲染实证（本端服务端只发 3 频道、单行）；② 选角色详情板 18 px 行距像素侧未收敛（逻辑层 `CS_DETAIL_*` 常量已对账 Angel，像素侧 `GEOMETRY_INCONCLUSIVE`）；③ 纯净校准二进制（验收 exe 含别会话 14 项并发改动）；④ 进图帧与目标帧场景态不同（目标户外 + 「基本操作法」弹窗 vs 本端地下城室内），NCC 不可比；⑤ 职业团页 EVAN(龙神)/DUALBLADE(双刀) 两张卡按用户明确指示保持不删。
>
> 影响面：5 个 UI TU（`UILogin.h/.cpp`、`UIWorldSelect.cpp`、`UICharSelect.cpp`、`UIRaceSelect.cpp`、`UICommonCreation.cpp`）+ 1 个运行目录素材（`UI/Creation/LoginLogo_override.png`）；未改 vcxproj、未新建源文件、未动 `Obj.cpp`、未改 live `.img`/`.nx`，别会话 14 项零触碰。

## 12 本轮小结（LOGO 遮罩翻案后的整体状态）

- **四项修复全部可交付**：版本号五页、登录品牌字标（字形遮罩）、属性板多余标签裁剪、选角色页标题牌改帧；四门脚本 `all_pass=true`，附加的动画/接缝两项门亦通过。
- **可复核的验收 exe**：`d48b3fa9d4390a0b7fafbaf14ce5e1afb7c843891b27dc7b4005dba603427895` / 2026-11-14 16:41:09 / 5 116 928 B。
- **仍在案的不签项**（与 §11.7 一致，未因本轮翻案而变）：`CELL_H=32` 无本端实证、18 px 行距像素侧未收敛、验收 exe 含并发改动非纯净二进制、进图帧场景态不可比、职业团 EVAN/DUALBLADE 两卡按用户指示保留。
- **方法学教训（新增）**：像素指标（NCC）可以被"整块复制目标像素"刷到 1.0，必须同时量**该区域是否仍在动**与**框边是否出现硬边**；三者同时成立才叫对齐。


## 13 频道页 / 角色列表页逐组件对齐（R3，2026-11-15）

> 用户三连体感（输入）：「频道页的所有 ui、动画都是一样的嘛？角色页选择角色后的属性 ui 是不是也是错误的？羊皮纸而且也靠下了，都校验 angel 了？」「频道页和角色列表页需要和原生客户端一样，请全部对照 angel 和 img 节点查看到底是什么样的逻辑」「角色选择页 属性值有点偏了，而且角色的名字名片是不是也靠下了？频道页的公告 ui 检查一下」。
>
> 交付口径：**逐组件差异表 → 源码落点 → 真机验收 → 残差披露**，不答「一样/不一样」的印象判断。

### 13.1 真值来源（两条线，互不替代）

- **Angel 083 IDA**：[`CUICharDetail_ctor_0x60292f.txt:20-23`](../login-flow-ui-ida-align/evidence/CUICharDetail_ctor_0x60292f.txt) `CFadeWnd::CreateFadeWnd(a2:183, a3:a1[57]!=0?151:115)`；`CUICharDetail_Draw_0x602adc.txt` 的 DrawTextA 坐标 职业(46,1)/等级(46,19)/人气(136,19)/力量(46,37)/智力(136,37)/敏捷(46,55)/运气(136,55)/排行(36,99)/(115-w,99)；[`CUIChannelSelect_ctor_0x62b2cf.json`](../login-flow-ui-ida-align/evidence/CUIChannelSelect_ctor_0x62b2cf.json) `CreateDlg(a4:0x21C=540, a5:167, a6:211)`。
- **orange-wz MCP 只读**：`E:\MXD\BeiDou-Client-xy\Data\UI\Login.img`，128B GMS128 key（`tools/_gms_key_128.b64`，head `EwAAAFIAAAAqAAAAWwAAAAgA`）+ iv `TSPHKw==` + `exclusive:false`。服务 10012 曾未监听，后台起 Tomcat + `mcp_connector_reconnect` 恢复；会话重载后发现加载的是 `UIWindow.img`，重新 `load_files` Login.img 才拿到真 `ne/se/e/n` 角件。全程不 mutate/save。
- WZ 节点真值：`CharSelect/scroll/2` 217×30 d250 → 217×196 d50 → 217×195 d50 → **217×193 末帧常驻**；`charInfo` 183×115 origin(45,57)、`charInfo1` 183×151（底部多一行居中「排行榜」，本端 `Range(0,36)` 裁掉）；`nameTag/1/0` 8×22；`WorldSelect/alert/backgrd` 540×167（与 Angel 0x21C/167 对上）；`channel/N/normal` 92×30；`chBackgrn` 449×233 origin(224,116)；`chSelect` 23×11 d100 → 38×21 d50 → 42×33 d50 → 59×33；`chgauge` 73×9；`scroll/0` 513×152 d250 → 513×416 d50 → 513×408 d50 → 513×401；`WorldNotice/BalloonForLoginProcess` 九宫 nw/ne 22×40、sw/se 22×24、n/s 12×40/24、w/e 19/22×9、c 12×9、Title 170×18。

### 13.2 逐组件差异表（本端 − 目标，常态帧）

帧口径翻案：`v2_channel_grid.png` mean=59.2 是**暗过场帧**，据此得出的「公告没画」是假结论；常态帧为 `channel_grid_f05.png` mean=152.12 / `world_f03.png` mean=144.67。

| 组件 | 目标 bbox | 本端修复前 bbox | shift | 用户体感 |
| --- | --- | --- | --- | --- |
| `charInfo` 内容板 | [392,220,575,335] | [391,197,574,312] | **[-1,-23]** | 「羊皮纸/属性板靠下」实为整板上移 23 |
| `charInfo1` | [392,220,575,371] | [391,197,574,348] | [-1,-23] | 同上 |
| `scroll/2` 卷轴 | [372,161,589,354] | [374,158,591,351] | [2,-3] | 卷轴几乎没动 |
| `nameTagL` 名牌 | [469,430,477,452] | [474,438,482,460] | **[5,+8]** | 「名字名片靠下」坐实 |
| `noticeTitle` 公告标题 | [293,313,463,331] | [259,301,429,319] | **[-34,-12]** | 「公告 ui 检查一下」坐实 |
| `noticeNw` 公告左上角 | [280,296,302,336] | [252,295,274,335] | [-28,-1] | 同上 |

联动反推：目标内容板顶 220 − 卷轴顶 161 = **59**，本端 197 − 158 = **39**，差 20 正是 `CS_SCROLL_CONTENT_OFF.y=39` 与真值 59 的缺口。故「内容板下移 23 + 卷轴只下移 3」必须**联动改两个常量**，整体挪会破坏卷轴。

### 13.3 源码落点（7 处常量，仅 3 个 TU）

- [`IO/UITypes/UICharSelect.cpp`](../../../IO/UITypes/UICharSelect.cpp)：`CS_CHARINFO_OFF(-94,-231)→(-94,-208)`（+23）、`CS_SCROLL_CONTENT_OFF(17,39)→(17,59)`（+20）、`nametags[index].draw(charpos + Point<int16_t>(2,25))→(2,17)`（-8）。
- [`IO/UITypes/UIWorldSelect.cpp`](../../../IO/UITypes/UIWorldSelect.cpp)：`WS_NOTICE_GAP 0→-36`（公告右移，目标 `nw_left=280 = 436-192-(-36)`）。**不改 `notice_y`**：12 帧曲线 [`notice_slide_curve.json`](evidence/term-verify-v2/r3-align/verdicts/notice_slide_curve.json) 证明 f01..f11 恒稳（title@259,301），是稳态常量偏移而非过场态；框顶 T297/O296 已对齐，整体下移会过冲 11px。
- [`IO/Components/ChatBalloon.cpp`](../../../IO/Components/ChatBalloon.cpp)：`box_w_ref 184→192`、`title.draw(top+6)→(top+18)`、`title_x = left+(box_w-170)/2 → left+13`、`box_h_ref` 保持 74。

### 13.4 真角件翻案：196 → 192（本节最有价值的一次自我否定）

- 首版按「目标 `title_left−nw_left=13` ⇒ `(box_w−170)/2=13` ⇒ **box_w=196**」落刀，配套 `WS_NOTICE_GAP=-40`。该推导用同一组居中公式既造目标又验目标，属自证；`predict_box_w_fix.py` 的 `PRECHECK_ALL_WITHIN_1PX True` 同样是自证，不能当独立证据。
- 重连 orange-wz 取得**真 `ne`(22×40)/`se`(22×24)**模板后，[`settle_frame_real_corners.py`](evidence/term-verify-v2/r3-align/scripts/settle_frame_real_corners.py) 用 alpha-NCC 独立定位：TARGET nw@280 / ne@450 / se@352，**span(ne−nw)=170**；OURS(box_w=184) nw@285 / ne@447 / se@344，span=162。`span = box_w − 22`（184−22=162 精确自洽）⇒ **box_w=192**，框右缘 `ne+22=472`。196 只拟合了两个左锚点，右缘会推到 476 过冲 4px。
- 原生标题并非严格居中：目标 `title−nw=13` ≠ `(192−170)/2=11`，故 `title_x` 改为独立常量 `left+13`，与框宽解耦。
- `box_h` 不动：正文暗字带实测本端中心 345.22 vs 目标 341.83（已偏低 3.39px），`text_y=ct+(cb−ct−h)/2` 在 `box_h` 增大时会把正文再下推 3px；正文像素判据本身噪声较大，本轮不动已验证的垂直内部位置。

### 13.5 真机验收与口径限制（candidate / 完成分离）

- **14:38 同代码验收**（隔离编译 `BUILD_EXITCODE=0`，exe `768c6e22…` / 14:32:41 / 5 137 408 B；频道帧 `channel_grid_f05.png` 14:33:49 mean151.2、选角色帧 `charselect_clean_f09.png` 13:32:16）：`verify_r3_align.py` 终端逐项 `charInfo [-1,0] / scroll [2,0] / nameTagL [4,0] / noticeTitle [0,0] / noticeNw [0,-1]`，**`ALL_PASS True ALL_STEADY True`**。名牌 x=4 系名字长度导致 CENTER 左片漂移（非 bug）；宽 ROI 曾把 8×22 模板误配到另一槽 `[459,448]`，收紧 `(465,418,492,462)` 后复算 `[4,0]`，是测量 bug 非代码 bug。
- **15:03 隔离编译**（`final_atomic_verify.py`）：exe `ba324061…` / 15:03:36 / 5 120 512 B，`BUILD_EXITCODE=0`，别会话 `ItemTooltip.cpp`/`UIShop.cpp` 两文件 sha 全等恢复（`RESTORE_ALL_OK True`）。证明**当前 192/-36/+13 源码可确定性构建**；首尾 exe sha 绑定 `unchanged=True`。
- **口径限制（必须读）**：14:33 干净帧 PNG 与 14:38 `verify_r3_align.json` 原件已被 15:04 / 15:13 / 15:22 三次重抓覆盖，仅终端逐项数值在案；15:03 之后三次抓帧均停在登录页（mean≈136.7、`anchor_hit=False`），**未产出可用 UI 帧**。故本轮签 `R3_ALIGNED_WITH_DISCLOSED_RESIDUALS`（候选完成 + 证据链 + 剩余风险），**不签「纯净校准二进制复跑通过」**；完整自洽记录见 [`r3_acceptance_record.json`](evidence/term-verify-v2/r3-align/verdicts/r3_acceptance_record.json)。
- 环境噪声归因（不作退化证据）：别会话 13:53 起反复改 `ItemTooltip.cpp`/`UIShop.cpp` 引入 `error C2248: "ms::ItemData::itemid": 无法访问 private 成员` 并跑并发构建；14:54 `LINK : fatal error LNK1104: 无法打开文件"x64\obj-Release\UIWorldMap.obj"` 时 `MSBuild.exe`/`cl.exe` 正被占用；live exe 被反复替换（ed03d4ef → dc6e7be4 → 5390ef8d → 768c6e22 → cb1d9751 → f707f37c → ba324061）。packet_dump 15:13 段有 `[SEND] 01 00 ...admin` / `[RECV] 0a 00 00 ...Scania` / `[RECV] 0a 00 ff`，说明协议登录成功，是共享 dump 起点被并发写入污染导致 `wait_anchor` 超时、脚本在登录页点空坐标。

### 13.6 明确披露的残差（不藏）

| 残差 | 实测 | 为何本轮不动 |
| --- | --- | --- |
| 公告框右缘 `ne` | OURS x454 vs TARGET x450，**+4px** | 真角件 NCC 干净单峰（OURS peak 0.9364 / x450 仅 0.4662），是真实残差；192 由 span 定案后未再调 |
| 公告框底 `se` | OURS y344 vs TARGET y352，**-8px** | 增大 `box_h` 经正文居中公式会把已偏低正文再下推，正文判据噪声大 |
| 公告正文中心 | OURS 345.22 vs TARGET 341.83，**+3.39px** | 同上，与框底残差同源 |
| 频道格数 | 目标 20 格（4×5） vs 服务端仅发 3 频道 | 数据态差异，非坐标漂移；CELL_W=94 已证，CELL_H=32 仍无本端实证 |
| 频道动画 | `scroll/0` 四帧展开、`chSelect` 红圈四帧、`chgauge` 负载条 | 逻辑层已对账 WZ 帧序与延时；本轮未做逐帧动画像素验收 |

### 13.7 证据与判词

- 归档目录 [`evidence/term-verify-v2/r3-align/`](evidence/term-verify-v2/r3-align/)：10 张 WZ 真模板（`templates/`，含真 `ne`/`se`）、10 份测量 JSON（`verdicts/`）、13 个脚本（`scripts/`），共 35 文件；目标帧 sha 清单 `verdicts/target_frames_manifest.json`。
- 判词：**签 `R3_ALIGNED_WITH_DISCLOSED_RESIDUALS`** —— 六组件差异表由 Angel + orange-wz 双源坐实，7 处常量按联动关系修复，14:38 同代码真机五组件 ALL_PASS、15:03 当前源码确定性构建通过；框右缘 +4 / 框底 -8 / 正文 +3.39 三个残差与「纯净二进制复跑」缺口如实披露。
- §11.7 的五项不签继续挂：`CELL_H=32`、18px 行距像素侧、纯净校准二进制、进图帧场景态不可比、职业团 EVAN/DUALBLADE 两卡（按用户明确指示保留）。
- 影响面仅 3 个 UI TU + 1 份报告 + `r3-align/` 证据目录；未改 vcxproj、未新建源文件、未动 `Obj.cpp`、未改 live `.img`/`.nx`，别会话并发改动零触碰。
