# 多分辨率实时切换 · 分析结果与实施方案

> 状态：**已实现**（分析 / 方案 / 代码 / 构建门禁 / 9 档实机矩阵均闭环；最终实现逻辑、偏差与未跑项见 [§8](#8-实现结果最终实现逻辑)）。
> 适用范围：`e:/project/MapleStory-Client`（C++ / SDL3 / OpenGL / nlnx），部署目录 `E:/MXD/BeiDou-Client-xy`。

## 1. 需求与已确认决策

用户原话：

> 当前客户端游戏设置窗口中的各个功能、ui位置是否正确，然后增加多分辨率实时切换的功能（E:\MXD\扩展改动\083多分辨率），登录分辨率还是保持不变，选择角色进入游戏后，按设置的分辨率显示游戏，游戏内切换分辨率实时生效；游戏ui定位、偏移逻辑先按照原生客户端的保留，分辨率过宽时，底部信息栏可以保持底部居中显示，大地图、弹窗、任务对话等还是可以保持居中打开；整体先对照原生客户端和img位置属性给我出一份分析结果和实施方案，要求验证点必须详细；

补充：`之前插件方式开发（E:\project\BeiDou-ijl15）应该是实现过了，可以借鉴一下；`

决策轮（用户已拍板，不得再改）：

| # | 决策 |
| --- | --- |
| D1 | 登录分辨率**保持当前 1280×720**（登录/选角窗口尺寸完全不变），只有进入游戏后才切到所选档位 |
| D2 | 档位沿用插件 `rs_tier_dims()` **9 档（0–8）**；档位大于当前桌面时**拒绝并保持原档 / 回落最大可用档**，不允许窗口超出桌面、不强制全屏 |
| D3 | 系统设置窗使用用户指定新 UI 图 `E:\MXD\扩展改动\083多分辨率\分辨率.png`（299×396，已含「窗口大小」行）+ 在其上新增分辨率选择；多选原选项（修复底栏空壳入口、两窗居中重锚）**本版不做**，列显式预留 |
| D4 | 点「确定」**立即生效并落盘**：新增独立 `window.game_*` 键，OK 时写配置 + 写 `Constants`，下一帧生效；取消/ESC 不生效；重启后仍按上次游戏档位 |

硬要求：**先给分析结果 + 实施方案，验证点必须详细**。

## 2. 分析结果（摘要，全文见 notes/）

### 2.1 游戏设置窗口

- **正确的是**：目录气球「游戏设置 / 系统设置」与 `O` 键分别打开 `UIGameOpt` / `UISysOpt` 两扇真窗；两窗控件相对底板的坐标全部与 IMG 烘焙行逐格 SAD≈0 对齐；9 项系统设置已真接线到渲染/输入/音频/告警/截图等消费方；ESC 白名单已含 `GAMEOPT/SYSOPT`。
- **不正确的是**：底栏「设置→选项」仍指向退化空壳 `UIOptionMenu`（本包缺 `StatusBar3.img/OptionMenu`，构造早退、只画底板无控件）；`PosGAMEOPT(170,24)` / `PosSYSOPT(330,24)` 为无 oracle 依据的绝对像素且无 `update_screen` 重锚；真窗**完全没有分辨率选项**（旧 `SELECT_RES` 6 档下拉与 SoftKey `WxH` 都在空壳里不可达，OK 分支还漏 `Configuration::save()`）。
- **部分正确 / 预留**：`GameOptMask` 12 位同意掩码可存可查但未接入邀请回包；下拉展开态为自绘（原生无展开态截帧）；画质档数与 VSync 阈值无原生依据。

详见 [notes/analysis-settings-window.md](notes/analysis-settings-window.md)。

### 2.2 IMG 位置属性

`UIWindow.img/GameOpt/backgrnd 158×284 origin(0,0)`、`SysOpt/backgrnd 299×365 origin(0,0)`（两者根节点各只有一张底板，行标题全部烘焙）；`Basic.img` 的 `CheckBox 12×12`、`ComboBox/normal/0..2 = 5×17/5×17/18×17`、`Slider/left|mid|right 3×8|1×8|3×8 origin(0,-2)`、`thumbNormal 22×14`、`BtOK2/BtCancel2 47×19`；`StatusBar.img/base/backgrnd 817×71`（不拉伸）、`quickSlot 466×80`、`gauge/bar 340×31`。全库检索 `Resolution / soScreenResolution / Custom` 零命中 ⇒ **IMG 内不存在分辨率选择节点**，该行是插件通过 `Custom.wz` 新增，不是原生 v83 资产。

用户指定新图实测 299×396（比运行时底板多 31px），y=370/380/390 为蓝色高亮条；它是**完整窗口截帧（含运行时控件）**，整张当底板会与代码叠加控件双绘，必须做控件区剥离。详见 [notes/img-position-attributes.md](notes/img-position-attributes.md)。

### 2.3 原生 / 插件对照

`E:/project/BeiDou-ijl15/ezorsia/compat/rs/rs.cpp` 已落地实现（比 `多分辨率.md` 的「Phase 2 未做」结论新）：9 档表、SysOpt Combo `(76,338) 166×18`、OK/Cancel 下移 `y=372`、登录 800×600 / Field 切档、OK 立即持久化 + 窗口销毁后延迟切换、九宫格 Origin 重锚、大地图/任务弹窗居中、输入命中坐标同步。必须规避：BPP 16 不可置 0、HD 死区、boot 立即数需重放、1280×720 与登录同尺寸时不能误判登录态。用户要求的「过宽时底部居中」与原生「贴底左对齐、快捷栏右贴边」明确冲突，本版按用户要求实现并标注偏离。详见 [notes/native-plugin-parity.md](notes/native-plugin-parity.md)。

### 2.4 当前链路与缺口

已有完整实时通道：`Constants` → `Window::check_events()` 差分 → `initwindow()`（SDL 尺寸 / glViewport / 虚拟光标复位）→ `GraphicsGL::reinit()`（VWIDTH/VHEIGHT / 投影 uniform）→ `Camera` → `UIStateGame::update_screen` 广播。**只需接通「档位 → Constants」入口，不重写渲染框架**。缺口：缺 SDL resize 事件；强制全屏门禁与「超桌面回落」冲突；`UIStatusBar` 只枚举 5 种宽度；`SetFieldHandler` 每次 SET_FIELD 从旧键回灌 Constants；Android 触屏换算用 Window 私有宽高；各 `PosXXX` 不重锚；广播仅对 active 元素生效。

## 3. 实施方案（摘要）

1. **配置与档位**：新增 `window.game_width / game_height / game_tier` 与头文件内 `inline constexpr` 档位服务 `Util/ResolutionTiers.h`；缺键时游戏档位 = 登录 1280×720（旧配置零回归）；登记 `SAVE_ORDER`，同步 `xy-config.example.yaml`，部署 `xy-config.yaml` 仅追加新键。
2. **实时切换**：`SetFieldHandlers.cpp` 的 `enter_game` 分支写游戏档位到 `Constants`（回调内禁止 `check_events`，下一帧主循环生效）、退登录恢复 1280×720；去掉每次换图的旧键回灌；`Window.cpp` 补 `SDL_EVENT_WINDOW_RESIZED / PIXEL_SIZE_CHANGED` 回写、把强制全屏门禁改为档位校验与回落、触屏换算改用 `Constants`、同步命中坐标。
3. **UI 重锚（保留原生定位）**：不做全局 transform / DPI 缩放；`UIStatusBar` 补非枚举宽度通用分支（`y=H-71`），过宽时底栏/快捷栏/聊天输入行整组水平居中（按用户要求，标注偏离原生）；大地图、任务窗、公告/弹窗按 `(W-dim.w)/2,(H-dim.h)/2` 居中并夹回视口。
4. **系统设置窗**：`tools/resolution/make_sysopt_bg.py` 对 `分辨率.png` 做控件区剥离 → 299×396 纯底板，经 `Graphics/LocalPngTexture` 接入（独立 atlas id，回退到原 `SysOpt/backgrnd`）；`UISysOpt` 新增第 3 个下拉（9 档文案走 `locale/zh-CN.ini`），显示真实当前尺寸，OK 立即持久化 + 窗口销毁后延迟应用（规避模态窗内重建状态栏黑屏），取消/ESC 不生效。
5. **不动**：`UIElement::Type` 枚举、公共头类布局、`MapleStory.vcxproj`、底栏空壳入口、两窗居中重锚、`GameOptMask` 邀请接线、服务端与协议。

## 4. 详细验证点

见 [notes/verification-checklist.md](notes/verification-checklist.md)（13 类、逐条可勾选判据，含期望数值、工具与证据落盘路径）。用户硬要求「验证点必须详细」，该文件是唯一权威清单。

## 5. 产物目录

```
docs/features/multi-resolution/
├── README.md                          # 本文件（实现闭环后补最终实现逻辑）
├── notes/analysis-settings-window.md  # 设置窗逐项核实
├── notes/img-position-attributes.md   # IMG 位置属性对照与取证口径
├── notes/native-plugin-parity.md      # 原生 / 插件对照与借鉴清单
├── notes/verification-checklist.md    # 详细验证点
├── notes/client-path.md               # 路径记忆
├── evidence/                          # REA/IDA 摘要、像素比对、实机截图
├── patches/  builds/  resources/
└── ui/uisysopt/v1/                    # 原图、纯底板、manifest
```

## 6. 阻塞与如实记录

- REA MCP 本会话先返回 `target_unavailable`，随后 `open_binary` 成功打开 `ijl15.dll`（PE/x86，sha256 已归档），但 `binary_overview` / `search_strings` / `list_strings` 全部返回 `unreadable_output`（`execution_failure`, `retryable=false`）⇒ **未产出新的反编译结论**。见 [evidence/rea-ijl15-identity.md](evidence/rea-ijl15-identity.md)。等效证据来自仓库已归档 IDA/REA 产物（`registry-so.txt`、`ijl15-dropdown-tables.txt`、`ui-positioning-scheme.md`、基址表）与插件源码 `rs.cpp`。
- orange-wz 对这批私服改版 `.img` 内部节点解析有已证实缺陷（`Index 16 out of bounds`），不可用 ⇒ IMG 证据一律以运行时 nlnx dump / `--resource-probe` / `nx_inspect.py` 为准（项目红线 R1/R2）。
- 实机逐档验证依赖 s9 服务端在 `127.0.0.1:8484` 运行；服务端需用户启动（Java 21 + MySQL）。若未启动，无法跑通进图验证，须在验证清单中记为未跑项并给出可执行步骤。

## 7. 显式预留（本版不动，非静默半成品）

底栏「设置→选项」空壳入口；`UIGameOpt/UISysOpt` 绝对落位与 `update_screen` 重锚；`GameOptMask` 邀请拦截接线；下拉展开态外观自绘；画质档数与 VSync 阈值无原生依据；REA/IDA 新增反编译取证与 orange-wz 通道恢复。每项均在对应 notes 标注原因与接线点。

## 8. 实现结果（最终实现逻辑）

### 8.1 实际落地清单

| 层 | 文件 | 作用 |
| --- | --- | --- |
| 配置 | `Configuration.h/.cpp`、`Constants.h`、`xy-config.example.yaml` | 新增 `window.game_width/game_height/game_tier`（`game_tier=255` 表示旧配置未选档），登记 `SETTINGS_ORDER`，`load()` 做缺键迁移；新增 `Constants::set_viewsize()` 成对写入，避免半帧中间态 |
| 档位服务 | `Util/ResolutionTiers.h`（新增，全头文件 inline） | 9 档表、档位↔尺寸互转、`clamp_to_desktop()` 回落、`resolve_game_size()`；不新增 `.cpp`、不动 `vcxproj` |
| 进图切换 | `Net/Handlers/SetFieldHandlers.cpp`、`IO/UI.cpp` | 所有 `SET_FIELD`（首次进游戏 / 换图 / 传送 / 退出商城）按 `window.game_*` 恢复视口；退出到 `LOGIN` 恢复 1280×720；删除每次换图的旧键回灌 |
| 窗口链路 | `IO/Window.cpp` | `check_events()` 差分驱动 `initwindow()`；移除「宽度≥桌面即强制全屏」门禁；Android 触屏换算改用 `Constants`；`toggle_fullscreen()` 门禁放宽为 `<=` |
| UI 重锚 | `IO/UITypes/UIStatusBar.cpp`、`UIWorldMap.*`、`UIQuestLog.*`、`UINotice.*`、`UINpcTalk.*` | 非枚举宽度兜底、过宽时底栏整组水平居中；大地图 / 任务窗 / 公告弹窗族 / NPC 对话构造期与 `update_screen()` 按视口居中，1280×720 保留原生绝对定位 |
| 系统设置窗 | `IO/UITypes/UIOptionMenu.h/.cpp`、`locale/*.ini` | 299×396 纯底板（`LocalPngTexture` 独立 atlas id）、第 3 个 9 档下拉、OK 落盘 + 下一帧延迟应用、取消/ESC 回滚、下拉显示真实当前档 |
| 资源 | `UI/SysOpt/sysopt_backgrnd.png`、`tools/resolution/make_sysopt_bg.py`、`ui/uisysopt/v1/` | 用户指定 299×396 完整截帧做控件区剥离，避免双绘；归档原图 / 生成图 / manifest 与「未出 PSD 分层」决策 |
| 取证工具 | `tools/resolution/verify_viewport.py`、`monitor_viewport.py`、`run_tier_matrix.py`、`summarize_matrix.py`、`analyze_crashdump.py` | 只读窗口尺寸、时间序列、9 档自动矩阵、CSV 汇总、WER 转储栈符号还原 |

### 8.2 登录 / 游戏双分辨率语义

`window.width/height`（恒 1280×720）只服务登录、选世界、选频道、选角；`window.game_*` 只服务游戏态。配置优先级是 `xy-config.yaml > Settings > 内置默认`，因此部署 yaml **不写死** `game_*`（否则会覆盖用户在设置窗保存的档位），只保留注释说明。

进图数据流：`SetFieldHandler` fade 回调按档位写 `Constants`（回调内禁止 `check_events()`）→ 下一帧 `Window::check_events()` 差分 → `initwindow()`（`SDL_SetWindowSize` / `glViewport` / 虚拟光标复位）→ `GraphicsGL::reinit()`（`VWIDTH/VHEIGHT` / 投影 uniform）→ `Camera` → `UIStateGame` 重建状态栏并广播 `update_screen()`。档位超出桌面时 `resolve_game_size()` 回落到「不超过桌面的最大可用档」，不自动切全屏、不改写 `window.fullscreen`。

游戏内点「确定」：`UISysOpt::commit()` 先写 `Setting<GameTier/GameWidth/GameHeight>` 并 `Configuration::save()` 落盘，然后置 `sysopt_pending_apply`；`update()` 下一帧改 `Constants` 并 `deactivate()`，规避模态窗回调内重建 `UIStatusBar` 的竞态（插件 `rs.cpp` 实测坑）。取消 / ESC 清 pending 并 `load_values()` 回滚。

### 8.3 实现期关键发现：SDL resize 自反馈导致堆损坏（已修）

逐档实机时 1440×900 档在切档后约 6 秒必崩，两次 WER 转储（`MapleStory.exe.15936.dmp` / `.35964.dmp`）均为 `0xC0000374`（堆损坏），调用栈残留帧完全一致：`loop → Window::check_events → initwindow → operator new`；而 1280×720 / 1366×768 / 1920×1080 均稳定。

根因：计划中「补 `SDL_EVENT_WINDOW_RESIZED / PIXEL_SIZE_CHANGED` 回写 `Constants`」实装后，窗口创建时**没有 `SDL_WINDOW_RESIZABLE`**（`SDL_CreateWindow` 仅 `OPENGL|HIDDEN`），这两个事件只可能来自 `initwindow()` 自己的 `SDL_SetWindowSize`；回写 `Constants` 与 `check_events()` 构成自反馈，反复建窗（`stbi_load` / `operator new` 高频分配）最终破坏堆。

修复：`IO/Window.cpp` 保留事件 case 但不再回写，视口只由 `Constants → check_events → initwindow` 单向驱动；代码注释记录「将来要支持手动拖边框须先加 `SDL_WINDOW_RESIZABLE` 且只在事件尺寸与私有 `width/height` 不同时回写」。修复后新 exe 在 1440×900 稳定 45s，9 档矩阵 9/9 PASS。证据：`evidence/live/crash-tier6-first.json`、`crash-tier6-second.json`、`tier6-after-resize-fix.json`、[evidence/crash-resize-feedback.md](evidence/crash-resize-feedback.md)。

### 8.4 与实施方案的偏差（逐条备案）

1. **resize 回写**：方案 §3.2 要补，实装后按 §8.3 移除；`V6.3` 手动拖边框因此不适用（窗口本就不可拖边框调整大小）。
2. **档位恢复点**：方案只在 `enter_game=true` 写 `Constants`；实际改为所有 `SET_FIELD` 都恢复，否则商城（`SetCashShopHandler::transition()` 强制 1024×768）退出后视口会停在 1024×768。
3. **NPC 对话**：方案只列 `UINotice` 弹窗族；实际补 `UINpcTalk::update_screen()/reanchor_talk()`，因为「任务对话保持居中」落在这个类。
4. **部署 yaml**：方案写「仅追加新键」；改为只加注释，避免部署级覆盖压掉 `Settings` 的用户档位。
5. **modern HUD 兜底**：子代理复核后补 `EXPBarRes` 回退 1280、QS 折叠/展开按钮非枚举宽度建钮、判空保护，避免空指针 / `std::out_of_range`。

### 8.5 实机验证结果

- **构建门禁**：`tools/build_smoke.bat` 最终 `EXITCODE=0`（协议矩阵 gate OK），部署 exe 刷新（17:26:07，4,658,176 B）；`read_lints` 对改动文件零诊断；全仓 `[DBG-]` / 临时文件日志本次改动零残留。
- **9 档矩阵**：`tools/resolution/run_tier_matrix.py` 自动逐档（重启 → 自动登录 → 进图 → 采样 42s），**9/9 PASS**，每档登录态恒 1280×720、进图达到期望尺寸、无崩溃，汇总见 [evidence/v3-tier-matrix.csv](evidence/v3-tier-matrix.csv)，逐档 JSON 在 `evidence/live/tier<id>-<WxH>.json`。桌面实测 2560×1600，9 档均不超桌面。
- **设置窗实机**：ESC → 目录气球 →「系统设置」成功打开真窗，截图 `evidence/live/live-02-sysopt.png` 显示 299×396 新底板、原有 6 滑条/5 勾选/2 下拉、新增「窗口大小」行，且下拉正确显示**真实当前档** `1280 x 720`（不是恒 0 档），无控件双绘重影。
- **崩溃 A/B**：1280×720 / 1366×768 / 1920×1080 稳定；1440×900 修复前 2/2 崩、修复后 45s 稳定。
- **配置现场**：验证期临时把 `xy-config.yaml` 的 `login.auto` 改为 `true`，收尾已恢复 `false`；`Settings` 的 `game_tier` 由矩阵脚本自动恢复为 `5`（1280×720）。

### 8.6 未跑项与阻塞原因（如实记录，不静默略过）

| 项 | 状态 | 原因 / 可执行补救 |
| --- | --- | --- |
| `V5.3` 点「确定」即时生效、`V5.4` 取消/ESC 不生效、`V5.5` 连切 5 次 | **未跑** | 设置窗已成功打开，但后续点击受既有工具坑影响：游戏自绘虚拟光标由相对位移累积，与 `SetCursorPos` 物理光标存在数十 px 偏差（`drive-flow.ps1` 重读 `ClientToScreen` 仍不足），下拉/OK 点击未命中；且保留实例随后退出、并发实例存在，未继续驱动。代码链路（`commit()` → `save()` → `update()` → `apply_viewsize()`）已静态确认。补救：用 `SendInput` 相对移动或先归中同步虚拟光标后重跑 `evidence/live/steps-live-switch.json` |
| `V6.1` 超桌面回落 | **未跑（环境限制）** | 本机桌面 2560×1600 ≥ 最高档 2560×1440，无法触发拒绝/回落；需缩小桌面或临时改小 `get_max_width/height()` 后重跑 |
| `V3.2/V3.3/V3.4`、`V4.2/V4.3`、`V7.*`、`V10.2` 逐档 UI 截图判读 / 命中全量 | **未跑** | 需人工逐档操作或稳定输入管线；本轮以窗口尺寸 + 进程存活 + 无崩溃作为自动判据 |
| `V3.1` 的 `glViewport` / 投影 uniform 直接读数、`V8.*` 换图/商城状态机 | **未跑** | 需运行时日志/断点取证；代码路径已静态复核（所有 `SET_FIELD` 恢复档位） |
| `V1.3` `--resource-probe` 抽样、`V1.5` 删 PNG 回退实测、`V1.6` en-US 切换 | **未跑** | 尺寸由生成脚本与实机截图互证；回退分支与 i18n 键由静态复核确认 |
| `V13.3` 删键回滚演练 | **未跑** | 避免破坏用户配置现场；`Configuration::load()` 缺键迁移逻辑已实现 |
| REA / orange-wz | **阻塞** | 见 §6，沿用已归档 IDA 产物与运行时 nlnx dump |

### 8.7 回滚

最小回滚：删除 `Settings` 的 `window.game_width / game_height / game_tier` 三键 → `Configuration::load()` 缺键迁移回落到登录 1280×720，游戏态与登录态同尺寸，登录 UI 完全不变。完整回滚：再还原 §8.1 表格中的源码文件与 `locale/*.ini`，移除 `UI/SysOpt/sysopt_backgrnd.png`。零协议、零服务端改动，回滚不涉及数据库或存档。

### 8.8 提交状态

**未提交**。工作区存在与本功能无关的并发改动（`ImgSource.*`、`MapleStory.vcxproj`、perf 仪器等），按规范只在用户授权后做单功能原子提交，不擅自 `git add/commit/push`。
