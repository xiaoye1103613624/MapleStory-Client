# 详细验证清单（验收判据 + 实测结论）

> 收口时间：2026-11-04。图例：
> `[x]` 已实测通过 ｜ `[~]` 部分/等价验证（静态确认 + 部分实机） ｜ `[ ]` 未跑（逐条给出阻塞与补救） ｜ `[-]` 不适用（有代码依据）。
> 汇总：**9 档矩阵 9/9 PASS**、构建门禁 `EXITCODE=0`、系统设置真窗与新增「窗口大小」行实机确认、
> 1440×900 堆损坏崩溃已定位并修复。未跑项集中在「需人工/稳定输入管线的 UI 逐帧判读」与
> 「本机桌面 2560×1600 过大无法触发超桌面回落」，详见 `../README.md` §8.6。
> 自动取证工具：`tools/resolution/{verify_viewport,monitor_viewport,run_tier_matrix,summarize_matrix,analyze_crashdump}.py`。

## V1 设置窗核实与资源

- [~] **V1.1** 12 项游戏设置 + 9 项系统设置控件坐标对 `_settings_map2.txt` 的 SAD ≤ 1。
  `结论`：沿用 `notes/analysis-settings-window.md` 的取证（SAD 0.00/0.07/0.60）；本轮未重跑模板匹配。
- [~] **V1.2** 功能接线与「正确/不正确/预留」结论逐项成立。
  `结论`：子代理跨目录复核确认 9 项系统设置消费方齐全；底栏空壳入口仍为显式预留。
- [~] **V1.3** 新底板 299×396 与 `UISysOpt::dimension` 一致；`--resource-probe` 抽样确认 `GameOpt/SysOpt` 仍为运行时值。
  `证据`：`make_sysopt_bg.py` 输出尺寸 + `live-02-sysopt.png` 实机窗口。 `未跑`：`--resource-probe` 本轮抽样。
- [x] **V1.4** 控件区剥离后整窗无滑条/勾选/下拉/按钮双绘重影。
  `证据`：`evidence/live/live-02-sysopt.png` —— 6 滑条、5 勾选、2 原生下拉、新增分辨率下拉与 OK/Cancel 各一套，无重影。
- [ ] **V1.5** 删除 `sysopt_backgrnd.png` 时回退到 IMG `SysOpt/backgrnd` 且 `dimension.y=396` 仍生效。
  `未跑`：静态确认回退分支存在；未删除运行时资源实测。
- [~] **V1.6** 9 条档位文案与 `rs.cpp` L1974-1984 一致、中英两侧键名齐全。
  `结论`：静态复核 `locale/zh-CN.ini` / `en-US.ini` 两侧 `.0..8` 齐全；`未跑` 游戏内 en-US 切换。
- [ ] **V1.7** `if(!node)` / `try_maple_btn` 守卫不产生 `_ui_asset_diag.log` 新增致命项。
  `未跑`：9 档运行期间无崩溃，但未做日志增量比对。
- [x] **V1.8** 打开系统设置窗后客户端不崩、不「未响应」。
  `证据`：`live-02/03/04/05-sysopt.png` 连续截图，窗口保持可绘制。

## V2 登录态恒定性

- [~] **V2.1** 启动/登录/选世界/选频道/选角客户区恒 1280×720，书框 1:1 不位移不裁切。
  `证据`：9 档矩阵每次启动的 `samples` 前段均为 `[1280,720]`； `未跑` 逐界面截图判读书框像素。
- [x] **V2.2** `Constants` 与配置核对：登录键未被游戏档位覆盖。
  `证据`：`findstr` 核对 `Settings` / `xy-config.yaml`；yaml 仅含注释、无写死 `game_*`。
- [x] **V2.3** 片头与自动登录全自动流程不受影响（登录→世界→频道→选角→进图）。
  `证据`：9 档矩阵 9 次重启均自动进图并达到期望尺寸（验证期 `login.auto=true`，收尾已恢复 `false`）。
- [ ] **V2.4** 游戏档位切换后手动退出到登录/选角，窗口恢复 1280×720。
  `未跑`：`UI::change_state(LOGIN)` 恢复逻辑已静态确认；未做退出态实机。

## V3 进图逐档（9 档全跑）

- [~] **V3.1** 每档 SDL 客户区宽高 = 档位值；`glViewport` / 投影 uniform / `Camera` 视野随分辨率单调扩大。
  `证据`：`evidence/v3-tier-matrix.csv` 9/9 `reached_expected=true`（Win32 `GetClientRect` 实测）。
  `未跑`：运行时直接读取 glViewport / uniform 数值。
- [ ] **V3.2** 底栏顶边 `y=H-71`。 `未跑` 需逐档截图判读（`UIStatusBar` 代码为 `VHEIGHT-CLASSIC_BAR_H`）。
- [ ] **V3.3** 大地图按 `((W-666)/2,(H-524)/2)` 居中。 `未跑` 需逐档开窗截图判读。
- [ ] **V3.4** 任务窗 / NPC 对话 / YesNo 弹窗按 `(W-dim)/2` 居中且不溢出。 `未跑` 同上；代码已补 `update_screen()` 重锚。
- [x] **V3.5（新增）** 9 档全部无崩溃、无「未响应」。
  `证据`：矩阵 `crashed=false` × 9；崩溃回归详见 `../evidence/crash-resize-feedback.md`。

## V4 非枚举宽度与底栏居中

- [~] **V4.1** 1600×900 / 1440×900 / 1680×1050 / 2560×1440 下 `UIStatusBar` 无分支崩坏。
  `证据`：4 档矩阵全 PASS（含修复前必崩的 1440×900）；`未跑` 逐控件相对坐标量测。
- [ ] **V4.2** HP/MP/EXP、LV 徽章、快捷格、BtMenu/BtShop 命中矩形相对偏移与 1280×720 一致。 `未跑` 需稳定输入管线。
- [~] **V4.3** 过宽时底栏整组水平居中且快捷栏/聊天输入行相对偏移不变。
  `结论`：代码用同一 `hud_center_x` 偏移整组并保留 `hud_grid_w`；`未跑` 逐档截图判读。

## V5 下拉与生效语义（核心）

- [x] **V5.1** 打开系统设置时下拉显示真实当前尺寸，而非恒为 0 档。
  `证据`：`live-02-sysopt.png` 显示 `1280 x 720`（当时 `game_tier=5`），`UISysOpt::load_values()` 读 `GameTier` 生效。
- [~] **V5.2** 9 条文案与插件一致；展开/收起/条目命中/高亮正确。
  `结论` 文案静态一致；`未跑` 展开态与条目命中（点击受虚拟光标偏移影响，见下）。
- [ ] **V5.3** OK 后下一帧窗口即变、无需重登。 **`未跑`（本轮主要遗留）**：保留实例的下拉/OK 点击未命中，
  且实例随后退出、并发实例存在，未继续驱动；`commit() → save() → update() → apply_viewsize()` 代码链路已静态确认。
  `补救`：用 `SendInput` 相对移动或先归中同步虚拟光标后重跑 `evidence/live/steps-live-switch.json`。
- [ ] **V5.4** 取消 / ESC 不产生任何尺寸变化与落盘。 `未跑`（同上）；`send_key(ESC)` / `CANCEL` 清 pending 的分支已静态确认。
- [ ] **V5.5** 连续快速切换 5 次不黑屏、状态栏不残留旧布局。 `未跑`。
- [ ] **V5.6** `UISysOpt` 控件重排（OK/Cancel 下移 y=372）与 299×396 底板一致。 `部分`：截图可见 OK/Cancel 在新行下方；未做像素量测。

## V6 超桌面与全屏

- [ ] **V6.1** 选超过桌面的档位被拒并回落最大可用档、给出可读提示。
  `未跑（环境限制）`：本机桌面 2560×1600 ≥ 最高档 2560×1440；需缩小桌面或临时改小 `get_max_width/height()` 重跑。
- [~] **V6.2** `window.fullscreen` 不被自动置 true；ALT+ENTER 与 `ui.sysopt.window_mode` 镜像关系不破坏。
  `结论` 强制全屏门禁已删、`toggle_fullscreen()` 改为 `<=` 静态确认；`未跑` ALT+ENTER 实机。
- [-] **V6.3** 手动拖窗口边框触发 resize 后 UI 与命中不错位。
  `不适用`：`SDL_CreateWindow` flags 仅 `OPENGL|HIDDEN`，无 `SDL_WINDOW_RESIZABLE`，用户拖不动边框。
  且本轮已移除 resize 事件回写（其自反馈正是崩溃根因，见 `../evidence/crash-resize-feedback.md`）。
- [x] **V6.4（新增）** 移除 resize 回写后不再出现反复建窗 / `0xC0000374`。
  `证据`：修复前 1440×900 2/2 崩，修复后 45s 稳定 + 9 档矩阵 9/9 PASS。

## V7 输入命中同步

- [ ] **V7.1** 每档点击 NPC、拾取道具、目录气球四钮、拖动滑条、切换勾选、展开并选中下拉、拖拽设置窗、Tooltip 夹回、虚拟光标边界全部命中。
  `未跑`：本轮仅「ESC → 目录气球 → 系统设置」一次点击链成功；后续点击暴露既有工具坑
  （游戏自绘虚拟光标由相对位移累积，与 `SetCursorPos` 物理光标差数十 px，`drive-flow.ps1` 重读 `ClientToScreen` 仍不足）。
- [ ] **V7.2** 高分辨率两侧无死区（插件坑 2/3）。 `未跑` 依赖 V7.1 的输入管线。

## V8 状态机

- [~] **V8.1** 换图 / 传送 / 使用商城后不回弹到 1280×720。
  `结论`：`SetFieldHandler::transition()` 已改为所有 `SET_FIELD` 恢复游戏档位（修掉商城退出残留 1024×768）；`未跑` 实机换图/商城。
- [ ] **V8.2** 未设置游戏档位时进图保持登录尺寸。 `未跑`：`Configuration::load()` 缺键迁移逻辑已实现。
- [x] **V8.3** 选 1280×720 游戏档位（与登录同尺寸）不误判登录态、不黑屏。
  `证据`：矩阵 tier5 及每次启动的登录→进图切换，9 次无黑屏。
- [~] **V8.4** 退出到登录/选角恒恢复 1280×720。
  `结论`：`UI::change_state(LOGIN)` 恢复逻辑静态确认；`未跑` 退出态实机。

## V9 持久化与安全性

- [x] **V9.1** `Settings` 含 `window.game_width/game_height/game_tier`，矩阵逐档读写成功。 `证据`：`findstr` + 9 份 JSON。
- [~] **V9.2** 「点 OK 后重启仍保持档位」。
  `等价验证`：矩阵把档位写入 `Settings` 后重启，进图均为对应档位；`未跑` 通过 OK 按钮写入后重启（受 V5.3 阻塞）。
- [ ] **V9.3** 删除新键或填非法值平滑回落 1280×720、不崩溃。 `未跑`：迁移逻辑静态确认。
- [x] **V9.4** 不写入明文口令、不改动既有 `login.account/login.pass`。 `证据`：配置 diff 仅 `login.auto` 临时开关且已恢复。

## V10 重锚广播与回归

- [~] **V10.1** `update_screen` 广播对 active 元素生效，四窗/状态栏数值不重影。
  `结论`：`UIWorldMap/UIQuestLog/UINotice/UINpcTalk` 均已 override；`未跑` 逐窗实机。
- [ ] **V10.2** 关闭态大地图/任务窗/设置窗重新打开时按新视口居中。 `未跑` 依赖 V7 输入管线。
- [ ] **V10.3** 四窗 I/K/S/E、Q 任务窗、聊天框、小地图、系统消息整体回归。 `未跑`（静态确认未改这些类的定位常量）。

## V11 协议与回归

- [x] **V11.1** 登录链路 8 个 c2s 包与 s9 对齐不变；零协议 / 零服务端改动。
  `证据`：构建门禁 `GATE OK: opcode matrix matches both sources (s9 44d0d5d9ca)`；本功能未改 `Net/Packets` / `RecvOpcode`。
- [~] **V11.2** 片头动画、自动登录、四窗、状态栏、任务窗等既有功能可用。
  `结论`：9 次自动登录进图全通；`未跑` 逐窗人工点检。
- [x] **V11.3** 无新增控制台调试输出。 `证据`：子代理全仓复核，本次改动未加 `[DBG-]` / 临时文件日志。

## V12 构建门禁

- [x] **V12.1** 构建前结束 `MapleStory.exe` 进程（多次执行 `Stop-Process` / `taskkill`）。
- [x] **V12.2** `tools/build_smoke.bat` 最终 `EXITCODE=0`，部署 exe 时间戳与大小刷新（17:26:07，4,658,176 B），`build_log.txt` 无 error。 `证据`：`../builds/build.log`。
- [x] **V12.3** 未改 `UIElement::Type` 枚举 / 公共头类布局，无需 `-t:Rebuild`。
- [x] **V12.4** Release 无控制台日志；全仓 `[DBG-]` / 临时日志零残留（本次改动）。
- [x] **V12.5** `read_lints` 对 `UINpcTalk.*` / `SetFieldHandlers.cpp` / `verify_viewport.py` 零诊断。

## V13 交付与回滚

- [x] **V13.1** `docs/features/multi-resolution/` 的 README / notes / evidence / patches / builds / resources / `ui/uisysopt/v1` 齐备且非空。
- [x] **V13.2** 记录 REA 会话与 orange-wz 阻塞、等效证据来源、未跑项原因（`../README.md` §6 / §8.6）与崩溃证据（`../evidence/crash-resize-feedback.md`）。
- [ ] **V13.3** 删键回滚演练。 `未跑`：避免破坏用户配置现场；回滚步骤见 `../README.md` §8.7。
- [x] **V13.4** 单功能原子提交：按规范**未提交**（工作区存在与本功能无关的并发改动）。

## V14（新增）崩溃根因闭环

- [x] **V14.1** 两次 WER 转储异常码与栈符号还原完成。 `证据`：`evidence/live/crash-tier6-{first,second}.json`。
- [x] **V14.2** 根因（SDL resize 回写自反馈 → 反复建窗 → 堆损坏）定位并修复，A/B 复验通过。 `证据`：`../evidence/crash-resize-feedback.md` §3-§5。
