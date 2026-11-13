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
