> 归档说明：本报告由「补终验」(2026-11-13) 生成并入库。运行侧原始证据（本端渲染帧 PNG / 各层 verdict JSON / packet_dump 片段）位于本机 `C:\Users\11036\AppData\Local\Temp\term_verify\`，为会话期取证产物、不入库；本报告已把全部关键数值内联，可独立阅读与复核。

---

# 登录四屏 · 四层一致性终验报告

> 补终验（用户 m00563）：「要 ui、动画都要一致，代码还有实现逻辑和功能一致」
> 生成时间：2026-11-13 23:00 · 证据根：`C:\Users\11036\AppData\Local\Temp\term_verify\`
> 味道口径：📌 体感是输入，证据链才是交付；口径不是修复；candidate / 完成分离。

## 0. 口径声明（先钉死，不事后放宽）

- 本报告是**证据链 candidate**，不是「登录四屏像素级修复完成」的盖章。上一轮（a90fda3）标题写「像素级校准」，实为三类性质不同的改动混一个 commit，缺「本端改后渲染帧 ↔ correct_shots」这一端；本轮补的就是这一端。
- 四层**分别签**，逐层给数值证据；不签的项明确列残留，不用「整体完成」糊过去。
- **口径污染警示**：运行 exe = `E:\MXD\BeiDou-Client-xy\MapleStory.exe` hash `b406cac6` / 21:11:58 / 5104128 B，是「当前工作树全量增量编译」产物，**含 160+ 其它特性改动**，不是「只编 a90fda3 校准」的纯净二进制。所有像素证据的真实含义是「当前全量工作树渲染出来的画面」，不是「校准 commit 单独的效果」。
- **窗口标题进图 ≠ 取到游戏画面**：首跑 100 帧里 60 帧被 Cursor Agents 白色网页遮挡（mean≈242.61/uniq=47 全同）一律作废；本轮所有入证据帧都过 `real_gl_frame` 判据（std>8、采样色>40）并经 `read_image` 目视确认。
- **日志串扰已排除**：早期 4 个 MapleStory 实例共享一份 packet_dump.log / setfield_trace.log，进图记录来自别的实例。本轮用「单实例 + 记 packet_dump 起始 offset 只读本实例新增字节 + 协议锚点」重取，结论才签。

## 1. 取帧方法学（为什么这批帧可作证据）

| 路径 | 结果 | 处置 |
| --- | --- | --- |
| 内置截图键注入（PRINT_SCREEN / SCROLL_LOCK / PostMessage / SendInput-scancode） | 3+2 次全败，exe 目录零新 PNG | 作废，换路 |
| `PrintWindow(PW_RENDERFULLCONTENT=2)` | 对 GL 返回静止合成图（已知坑） | 作废 |
| 自动链 100 帧首跑 | 60 帧白色网页遮挡 | 作废 |
| **单实例 + 协议锚点 + HWND_TOPMOST + BitBlt(screen DC)** | **REAL_GL_FRAMES** | **入证据** |

- 输入打通：x64 `INPUT` 结构须含 `MOUSEINPUT`（`sizeof(INPUT)=40`、`u.offset=8`），否则 `SendInput` 整批返 0；修复后用 `KEYEVENTF_UNICODE` 产 WM_CHAR 喂 SDL3 文本输入，密码框出现 `*****`。
- 协议锚点：等本实例新增日志出现 `RECV 0a 00 ff`（SERVERLIST end）才抓「选世界」；出现 CHARLIST 才抓「选角色」。
- 配置全程未改：`xy-config.yaml` head3=`b'# M'`（无 BOM）、`login.auto=false`。UNCIODE 输入 + 点击走手动链，不靠 auto 一闪而过。
- 按钮前置条件坐实：`UIWorldSelect::enter_world()` 在 `channelid==0xFF` 时 `return`（`UIWorldSelect.cpp:1530-1531`），必须先点 CH.1 选中才发 CHARLIST_REQUEST(05)——首次点「到选择的世界去」无 `05` 不是坐标错，是前置状态没满足。

## 2. UI 层（本端改后渲染帧 ↔ correct_shots）

| 屏 | 本端帧 | 目标帧 | 关键数值 | 判定 |
| --- | --- | --- | --- | --- |
| 登录 | `shots/01_login.png` | `targets/login.png` | 全图 gray NCC **0.927424**；mask 品牌LOGO/版本号/光标后 **0.987664**（839550px）；R/G/B masked 0.988/0.990/0.991；edge 0.926774；**row-projection 0.997046 / col-projection 0.991335** | **PASS**（结构几乎完全同位、无整体平移；残余差异=品牌LOGO MapleStory叶子 vs BeiDouMS、无版本号、输入态） |
| 选世界·卷轴收起 | `shots/world_f03.png` | `targets/channel_select.png` | gray NCC **0.977045**；R 0.97536 G 0.975501 B 0.991383；8×8 blocks mean 0.9735；仅 2 热区(r0c0 0.757 / r0c6 0.7399，按钮文字/版本) | **PASS**（结构几乎完全同位） |
| 选频道·网格展开 CELL_W | `shots/channel_grid_f05.png` | `targets/channel_selected.png` | 双向列信号自相关：**本端 best_lag=94 corr=0.8177**、**目标 best_lag=94 corr=0.7396**；本端模板 NCC 峰 x=[458,552,646] y=336 → 步长 [94,94]，ncc=[0.9998,0.9343,0.9305] | **PIXEL_CLOSED_CELL_W**（把「照目标帧改 CELL_W100→94」升级为「本端渲染帧实测=目标帧」） |
| 选角色 | `shots/charselect_clean_f05.png` | `targets/char_select.png` | 清晰常态帧（非 fade 暗帧提亮）；**详情板宽 215=215 完全一致**；scene_bg ROI NCC **0.9199**；全图 gray NCC 0.67476 | **GEOMETRY_PARTIAL**（全图不可比：本端 3 角色选中中间 aaaaaa、目标 1 角色 Admin，X 差 117px） |
| 进图 | `shots/screen_01.png` | `targets/ingame.png` | gray NCC **0.085227**、blocks mean 0.0811、hot 64 | **NOT_COMPARABLE**（目标帧=蘑菇屋户外+「基本操作法」弹窗，本端=地下城室内无弹窗，场景状态根本不同，非坐标漂移） |

- UI 证据文件：`ui_login/evidence.json`+`verdict.json`、`ui_worldselect/evidence.json`、`ui_channel/grid_step_verdict.json`、`ui_charselect_clean/evidence.json`+`geom_verdict.json`、`ui_ingame/evidence.json`。
- **不签**：选角色页 18px 行距的像素侧自动检测（水平梯度峰 / 暗墨投影两端都不收敛，本端 spacings `[46,23,12,13,11,10,19,17]` 混噪声、目标 `[15,12,11,46,15,26,11]`；`shot_has_18x2=true` 是噪声碰巧凑出，硬凑当 Angel 铁证踩事实红线）。该行距已在**逻辑层**用 `CS_DETAIL_*` 常量对账 Angel 10/10 签过，不重复造像素仪式。

## 3. 动画层

| 项 | 证据 | 判定 |
| --- | --- | --- |
| 多帧动画活 | `shots/seq_analysis.json`：100 帧、**total_animated_frames=92**、clusters_with_animation=2、verdict=`MULTI_FRAME_ANIMATION_OBSERVED`；cluster0 帧0-8(世界背景, 8帧动画) / cluster1 帧10-99(进图, 84帧动画) | **PASS** |
| chSelect 红圈末帧常驻 | `ui_channel/chselect_anim_verdict.json`：点 CH.1 后 0.5s 连抓 3 帧，红色像素 **744/744/744**、质心 **[482.1,337.5] 零漂移**、CH.1 ROI NCC=**1.0/1.0** → `LAST_FRAME_PERSIST_VERIFIED` | **PASS** |
| 代码/IMG 对账 | `UIWorldSelect.cpp:497-518` 播完定格 `size()-1`；`:720-721` 非 playing 画末帧；IMG 真值 `chSelect /0 d100 /1 d50 /2 d50 /3 no-delay`；落点 `(469,328)` 与 `CH_SELECT_CIRCLE_OFFSET(-2,-9)`+末帧 origin(29,16) 自洽 | **PASS** |
| CharSelect/scroll/2 | IMG 真值 `217×30 d250 → 217×196 d50 → 217×195 d50 → 217×193 末帧常驻`；a90fda3 H5 `scroll_frame.draw` 加回 `get_origin()`；调用方取末帧绘制 | **PASS**（逻辑层） |

- 诚实标注：红圈三帧 `bg_sky_mean_abs=0.0`（频道网格屏云背景本就静止），`bg_animation_alive=false` 不是抓帧死，不影响「末帧常驻」判定（该判定靠红色像素稳定 + ROI 静态）。

## 4. 实现逻辑层

- `packets/verify_function.json` → **C_logic = ALL_PASS 10/10**：
  `channel_cell_step`(CELL_W=94/CELL_H=32)、`cell_offset_formula`(22+94·rem / 56+32·quot)、`charselect_angel_coords`(JOB[46,1] LEVEL[46,19] FAME[136,19] STR[46,37] INT[136,37] DEX[46,55] LUK[136,55])、`charselect_rank_y99`、`charselect_black_text`(BLACK×8)、`scroll_frame_origin`、`animation_update_loops`、`charselect_lastframe_persist`、`login_ime_unicode`、`global_screenshot_keys`。
- 状态机可达性（源码钉死，非猜测）：`Net/Handlers/LoginHandlers.cpp:254-286 ServerlistHandler::handle` 收 SERVERLIST 时 `emplace<UIWorldSelect>()`；`:288-340 CharlistHandler::handle` 收角色列表 `emplace<UICharSelect>(...)` → 选世界/选频道/选角色三屏代码可达，自动链跳过是 `get_auto_login()` 链路设计，不是没实现。
- 单实例协议链：`shots/clean_world_summary.json` 本实例新增 SEND=2（`01` 登录 / `0b` SERVERLIST_REQUEST）、RECV=5；`channel_grid_summary.json` 点世界新增 `06 00 00 00`(ServerStatusRequest)；`charselect_clean_summary.json` `charlist_hit=true` + SEND `05 00 00 00 00`(CHARLIST_REQUEST)。三段串起来 = 登录→选世界→选频道→选角色全链逻辑走通。

## 5. 功能层

- `A_packet = TRAFFIC_OK`：packet_dump.log 14595 行、SEND=9371 / RECV=5224、有认证流量；send_opcode_top `29`:7122 `BC`:1534 `18`:322 `CA`:67；send_first3 `01 00` / `0b 00` / `05 00`。
- `B_setfield = REACHED_GAME`：setfield_trace.log `Stage::load=11` / `fade done=11` / maps `104000200 / 104000300 / 104000400 / 914000000`；尾部 `Stage::load ok` / `GraphicsGL unlocked` / `transfer_player ok` / `fadeprocedure done locked=0`。
- 窗口标题进图：`XiaoYE MS v0.0.1【3-aaaaaa】【2000-战童技能】【破功值：199999】...【在线时长：00 00:00:32】`。
- 协议可达 CHARLIST：单实例 `05 00 00 00 00` 坐实（非多实例串扰）。

## 6. 不能签的（candidate / 完成分离）

1. **「登录四屏像素级修复完成」仍不签**：UI 层只有登录屏 / 选世界收起态 / 频道格 CELL_W 三处有本端渲染帧闭环；选角色页全图因 3角色vs1角色状态差异不可比；进图因场景状态根本不同不可比。
2. **CELL_H=32 本端渲染实证缺失**：本端服务端只发 3 频道 1 行，无法量行步长；以目标帧 y=[337,369,401,433,465] + `UIWorldSelect.cpp:180-181` 常量对账，属代码/目标帧侧证据，非本端像素侧。
3. **选角色页 18px 行距像素侧**：两种自动检测器两端都不收敛，明确不签（逻辑层常量对账已覆盖）。
4. **纯净校准二进制**：exe 是全量增量编译含 160+ 特性，未做「只编 a90fda3」的隔离构建；两次 `build_calibration.bat` 重建尝试（MSYS `//c` 假成功、`cmd.exe /c` 相对路径 not recognized）均未产出新的确定性 log。
5. **真值源口径未完全统一**：`notes/authority-switch-2026-11-12.md` 记「登录链后续全部以 Angel 反编译为准，不以截图为准」，但频道 94/32 恰用截图 NCC 量出、选角色页用 Angel 字面量；本轮用「本端渲染帧 ↔ 目标帧」把两者在像素侧拉到同一张桌上，但其口径分裂的历史事实仍在。

## 7. 遗留与收尾

- **待实机叠图项**：README.md:386-391 的「实机五屏截图比对」可**部分销**（登录/选世界/选频道/选角色四屏已取到本端渲染帧并叠图，进图帧也取到但状态不可比），不能全销（进图需与目标帧同场景同弹窗态重取）。
- **git 卫生**：分区 subagent `537bb49d` 自报「git status 干净」不实——独立核验 `git status --short` 仍有 **281 行 `??`**（`tools/_angel_*.json` / `tools/_mcp*.json` / `_uw_*` / `face1x_*` / `test_*.py` / `_tmp_*` / `verify_out/` / `tools/vendor` 探针），`git check-ignore` 抽样全 `NOT_IGNORED`，`89f3dc45` 的 .gitignore 模式没盖住。见本报告交付后的收尾动作。
- git 分区本体已核验通过：分支 `feature/dev_0.0.1`、`a90fda3` 是 HEAD 祖先、`git diff --name-only`=0、`git diff --cached`=0、`a90fda3..HEAD`=28 commits（27 特性 + 89f3dc45 chore）。仅 2 文件出现 2 次（同一文件两特性各改一次，非误提交）。

## 8. 结论

| 层 | 结论 | 证据强度 |
| --- | --- | --- |
| UI | **部分闭环**：登录屏 masked 0.988 + 投影 0.997/0.991、选世界 0.977、频道格 CELL_W=94 双向像素闭环；选角色页几何 215=215/scene_bg 0.92 但全图状态不可比；进图状态不可比 | 强（本端真 GL 帧） |
| 动画 | **闭环**：92 动画帧 + chSelect 末帧常驻（744/744/744、ROI NCC 1.0）+ IMG 帧序/代码对账 | 强 |
| 实现逻辑 | **闭环**：10/10 ALL_PASS + 状态机构造点 + 单实例协议链 01→0b→06→05 | 强 |
| 功能 | **闭环**：TRAFFIC_OK + REACHED_GAME + 窗口标题进图 + CHARLIST 单实例坐实 | 强 |

> 拿数据闭环：四层里三层全签、UI 层三屏签两屏半签，且所有「不签」项都有可核对的具体原因与残留证据，不是「差不多就行」。这叫证据链交付，不叫口径修复。
