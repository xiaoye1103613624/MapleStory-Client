# 登录布局 1024 修复与构建输出

> 状态：已闭环；**2026-09-22 晚追加第 7 节**（书框改回原版素材 + 默认分辨率 1280×720）、
> **2026-09-23 追加第 8 节**（双书页修复 + 面板相对木牌对齐 (58,16)）  
> 适用客户端/协议版本：BeiDou gms083（**1280×720**，见第 7 节）  
> 项目名称：MapleStory-Client  
> 作者/日期：2026-09-22  
> 产物目录：docs/features/login-layout-1024-fix/

## 1. 背景与目标

- **问题**：登录页出现双书页/左侧露边，木牌与连接/密码/底部按钮错位。
- **根因**：未完成的 WIP 把登录场景改成「逻辑 1280×720 + `GraphicsGL::set_world_transform`」，但四边形仅入队、`flush` 前已 `reset`，变换从不生效；同时 UI 经 `to_screen` 缩放，背景按 1280 像素当屏幕坐标画，坐标系分裂。另注释掉 `Common/frame`，与北斗「书框 + 内页地图」不一致。
- **目标**：恢复与地图同相机的锚点布局；单次绘制书框；产物输出到 `E:\MXD\BeiDou-Client-xy\`。
- **不做**：不新增 CMake；不引入新的 world-transform 方案。

## 2. 影响范围

| 层级 | 路径/模块 | 变更类型 |
| --- | --- | --- |
| 客户端 | `IO/LoginBackground.*`、`IO/UITypes/UILogin.cpp`、`IO/UIStateLogin.cpp` | 回退到 HEAD 已验证布局 |
| 客户端 | `Graphics/GraphicsGL.*` | 移除失效 world-transform API |
| 客户端 | `Gameplay/MapleMap/MapBackgrounds.*` | 回退；登录 back 用真实 VWIDTH/VHEIGHT |
| 客户端 | `Constants.h` | 默认 1024×768（与 Settings/xy-config 对齐） |
| 配置 | `Configuration.*` | **保留**嵌套 xy-config / URL 热更（无关布局） |
| 工程 | `MapleStory.vcxproj` | `OutDir` → `E:\MXD\BeiDou-Client-xy\` |

## 书框只画一次（MapleEzorsia Common）

> **本节已被第 7 节取代**：`MapleEzorsiaV2wzfiles.img/Common/frameNNNN` 的开口太小，
> 现在优先用原版 `UI/Login.img/Common/frame`，Ezorsia 仅作兜底。
> "只画一张、禁止叠画"的规则仍然有效。

双书页/版权字重复的原因是书框资源被叠画。当时的用法（现为兜底）：

| 分辨率 | 只用这一张 |
| --- | --- |
| 1280×720 | `UI/MapleEzorsiaV2wzfiles.img/Common/frame1280` |
| 1024×768 | `UI/MapleEzorsiaV2wzfiles.img/Common/frame1024` |
| 其它 | 同目录对应 `frameNNNN`；若无则退回 `Login.img/Common/frame` |

`LoginBackground` 按窗口宽高**只选一张**并 `frame.draw` 一次；禁止遍历/叠画 `MapleEzorsiaV2wzfiles.img/Common` 下多张 frame。

### 登录输入框与选频道（续）

- 北斗登录：不再绘制白底 `ColorBox`；文字用浅色并整体上移 2px，贴合木牌雕刻槽。
- 选频道：BeiDou 下 `channelsrc_pos` 锚到 `Map/Obj/login.img/WorldSelect/signboard/0` 卷轴中心，避免 chBackgrn 与地图卷轴错位。

## 4. 验证

- [x] `msbuild MapleStory.sln /p:Configuration=Release /p:Platform=x64` 成功（产物 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`）
- [x] 冒烟启动：进程可起窗（标题 MapleStory），约 12s 后正常结束
- [ ] 人工目视确认：单书框、木牌与按钮重合、点击命中区（请本地再看一眼）

## 5. 构建说明

本仓库使用 **Visual Studio / MSBuild**（`MapleStory.sln`），**没有 CMakeLists.txt**，属工程选型而非缺失。

```text
MSBuild MapleStory.sln /p:Configuration=Release /p:Platform=x64
```

## 6. 回滚

- 布局相关文件已与修复前 HEAD 对齐；仅保留 Configuration 嵌套解析与 vcxproj OutDir。
- 若需改回源码旁输出：把 `OutDir` 改回 `$(SolutionDir)$(PlatformTarget)\$(Configuration)\`。

## 7. 追加（2026-09-22）：书框改回原版素材 + 分辨率改 1280×720

### 7.1 现象与根因

反馈："登录界面没铺满窗口"。实测（见 `evidence/login-frame-compare.md`）：

| 项 | `MapleEzorsiaV2wzfiles.img/Common/frame1024`（当时在用） | `Login.img/Common/frame`（原版） |
| --- | --- | --- |
| 尺寸 | 1024×768 | 1280×720 |
| 书页开口 | 约 611×461（占窗口 60%×60%） | 约 1091×581（占窗口 85%×81%） |
| 与截图匹配 | 客户端画面 1:1 命中 79% | 参考客户端画面 1:1 命中 98.8% |

根因：**书框素材选错**。整幅书框其实是铺满窗口的，但第三方素材包那本的"书页开口"很小，
登录地图只能从中间 6 成见方的小窗透出，观感就是"没铺满"。原版数据里 1024×768 的书框
**不存在**（`nx_inspect.py size 1024x768` 在 UI.nx 中只命中 Ezorsia 那一张），
原版登录 UI 本身就是按 1280×720 做的。

### 7.2 决策（用户取舍：选 C）

- A 换原版书框 + 拉伸铺满（1024×768 下横向压 0.8 / 纵向拉 1.067，书页轻微变形）
- B 换原版书框 + 等比（裁掉书脊与页边，或上下留白）
- **C（采纳）分辨率改 1280×720**：与参考客户端 1:1，无变形，代价是要复验各界面锚点
- D 保留 Ezorsia 书框、缩地图去贴小开口（否决）

### 7.3 改动清单

| 层级 | 路径 | 变更 |
| --- | --- | --- |
| 客户端 | `Constants.h` | 默认 `VIEWWIDTH/VIEWHEIGHT` 1024×768 → **1280×720** |
| 客户端 | `Configuration.h` | `Setting<Width>/<Height>` 默认 "1024"/"768" → **"1280"/"720"** |
| 客户端 | `IO/LoginBackground.cpp/.h` | 书框优先原版 `Login.img/Common/frame`；`MapleEzorsia…/frameNNNN` 降为兜底（仍只取一张） |
| 客户端 | `IO/UITypes/UIQuit.cpp` | 退出回登录时用**配置分辨率**，不再写死 1024×768（否则书框又被拉伸） |
| 客户端 | `IO/Window.cpp` | 启动时把桌面尺寸写入 `Configuration` 的 `MAXWIDTH/MAXHEIGHT` |
| 部署配置 | `E:\MXD\BeiDou-Client-xy\xy-config.yaml` | `Width/Height` → 1280 / 720（优先于 Settings 与二进制默认） |

关于 `Window.cpp` 那处：此前 `MAXWIDTH/MAXHEIGHT` **从未赋值（恒为 0）**，
`Window::check_events()` 里 `new_width >= max_width` 恒真，任何改分辨率的分支都会被
强制切全屏（且 `toggle_fullscreen()` 因 `width < max_width` 恒假而完全失效）。
默认分辨率改成 1280×720 后"进商城（固定 1024×768）"会踩到这条，故顺带补齐。
如不想要这处改动，可只回退 `IO/Window.cpp`；代价是商城/切分辨率时会被强制全屏。

### 7.4 书框绘制方式（保持不变）

`Sprite` 的绘制矩形恒为 `(0,0)-(vieww,viewh)`（`Texture::draw` → `get_rectangle`），
位图被拉伸到该矩形：**1280×720 下即原图 1:1**，其它分辨率下自动铺满窗口。

### 7.5 验证

- [x] `tools\build_smoke.bat`（MSBuild Release/x64）成功，产物已直接输出到 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
- [x] 资源核对：`nx_inspect.py list … Login.img/Common/frame` → `bitmap 1280x720`
- [ ] **人工目视**：启动客户端，登录界面书页开口应铺满窗口（与参考客户端一致），
      木牌/按钮仍贴在告示牌上；随后复验大区选择、频道选择、角色选择、创建角色
      （这几处按 `vieww` 居中或锚在地图上，理论自适应，但需实测确认）

### 7.6 复用工具

`tools/nx_inspect.py`（只读）：列出 NX 节点 / 按尺寸找位图 / 解码位图成 PNG。
用法见文件头注释与 `evidence/login-frame-compare.md`。

## 8. 追加（2026-09-23）：双书页修复 + 面板相对木牌对齐

### 8.1 双书页（两层书框）

**现象**：1280×720 下登录界面出现两层书框（外层偏移约 (+240,+60)），版权行出现两次。

**根因**：初始化顺序问题。
`MapleStory.cpp` 里 `UI::get().init()` 先于 `LoginBackground::get().init()`：
`UI::init()` 会创建 `UIStateLogin` → `UILogin`，而 `UILogin` 构造时用
`LoginBackground::available()` 判断「背景是否已由 LoginBackground 接管」；
此时 `available()` 仍为 false，于是 `UILogin` 自己把 `Login.img/Common/frame`
作为背景 sprite 加进了 `sprites`（`sprites.emplace_back(backgrd_src, screen_center)`）。
该书框 origin = (400,300)，所以它被画在 `screen_center - origin` = (240,60)；
`LoginBackground` 随后又在 (0,0) 画一张 → 叠加成双书页。

> 之前 1024×768 下之所以看不明显：`UILogin` 那张 1280×720 书框的**书页开口**
> 恰好露出地图（可见区约 804×580），而 Ezorsia 书框开口只有 611×461，
> 于是"地图只在中间一小块"的观感其实有一部分来自这张重复书框。

**修复**：`MapleStory.cpp` 调整顺序（LoginBackground 先 init，UI 后 init）。
非 BeiDou 数据（LoginBackground 初始化失败 → available() 仍为 false）时，
`UILogin` 的兜底背景照旧生效。

验证：掩码互相关定位 `Login.img/Common/frame` 的 logo 贴图，本实现截图里只应出现一次。

### 8.2 面板相对木牌的偏移（按钮/输入框）

**现象**：按钮整体偏左上，输入框没落在木牌自带的输入区里。

**根因**：`CUITitle::OnCreate` 的按钮/输入框坐标是**相对 CUITitle 窗口**的，
而窗口自身屏幕位置在反编译里未确定（`notes/ida-coords.md` §6）。
此前实现把「木牌左上角」直接当窗口原点，导致整块面板相对木牌偏左上。

**修复**：新增 `TITLE_WINDOW_OFFSET = (58, 16)`（面板原点 = 木牌左上角 + 该偏移），
按钮/输入框/勾选框统一以 `panel_pos` 为基准。
数值来自参考客户端 1280×720 截图反推：7 个按钮逐个给出的差值都是 (58,16)，
6 个匹配度 ncc=1.000；勾选框 (12,73) 与账号输入框 (67,12)/(67,40) 也吻合。
完整表格与复现方法见
[`docs/features/login-flow-ui-ida-align/evidence/login-panel-offset.md`](../login-flow-ui-ida-align/evidence/login-panel-offset.md)。

效果：输入框正好落在木牌自带的两个深色内嵌输入区上，按钮落在对应木板上。

### 8.3 涉及文件

| 路径 | 变更 |
| --- | --- |
| `MapleStory.cpp` | 登录背景先于 UI 初始化（消除双书页） |
| `IO/UITypes/UILogin.cpp` | `panel_pos = sign_topleft + (58,16)`；控件偏移全部改以 `panel_pos` 为基准 |

验证：
- [x] `tools\build_smoke.bat` 成功，产物直接输出到 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
- [ ] **人工目视**：只应有一层书框；按钮与"木牌木板"重合、输入框落在木牌自带输入区内

## 9. 追加（2026-09-23）：输入框去白底 + 选频道锚卷轴 + `/utf-8`

### 9.1 登录输入框

- 去掉账号/密码白底 `ColorBox`（木牌已有雕刻槽）。
- 文字改浅色（CREAM），纵向再上移 2px，贴近雕刻槽。

### 9.2 选世界/选频道错乱（已二次修订）

**根因（第一轮误锚）**：曾把 `channelsrc_pos` 锚到 `WorldSelect/signboard/0`。
该节点是**横杆+下垂绳扣**（674×171 @ `(51,-888)`），不是羊皮卷，导致 chBackgrn 压在绳子上。

**正确地图物体**（`MapLogin.img/1/obj`）：

| 节点 | 地图坐标 | 含义 |
| --- | --- | --- |
| `signboard/0` | (51, -888) | 横杆与绳扣 |
| `signboard/2` | (42, -753) | 蓝端头羊皮卷（频道面板应对齐） |
| `signboard/1` | (50, -1044) | 两侧挂钩装饰 |

**本轮修复**（`UIWorldSelect.cpp`，2026-09-23 三轮）：

1. 木牌 X/Y 对齐 `signboard/0`：水平以横杆中心居中，竖直挂在底边绳扣下（`pole_bottom + 4`）。
2. 频道面板顶边在 `signboard/2` **底边之下**（`CH_BELOW_GAP=8`），不再以卷心为面板中心；选中后约 250ms 自上滑入。
3. `world/[id]` 横向图标+名仅选中后画在 chBackgrn 表头；空槽 `BtWorld/e` 默认可见。

### 9.2b 选角色桥位（同日）

| 项 | 做法 |
| --- | --- |
| 三按钮 | 锚 `MapLogin CharSelect/signboard/0@(253,-1258)` 三块横板 |
| 角色/影 | 槽位 Y = `CS_LIST_T + 140 - 100`（桥面）；一页固定画 3 影 |
| 选中光 | `effect/1` 脚底 → 角色 → `effect/0` 身光（`CS_FX_FOOT/BODY`） |

### 9.3 工程：启用 `/utf-8`

源码已是 UTF-8，但 MSVC 默认按系统代码页（936）解析，中文注释可导致误解析/级联错误。
`MapleStory.vcxproj` 四个配置均加 `<AdditionalOptions>/utf-8 …</AdditionalOptions>`。

### 9.4 同步策略

客户端占用导致 LNK1104 时：按用户选择 **B**——结束 `MapleStory.exe` 后覆盖同步（本会话后续同策略）。

### 9.5 验证

- [x] 结束占用进程后 `MSBuild … Release|x64` 成功
- [x] 产物 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（WorldSelect/CharSelect 修订后覆盖）
- [ ] **人工目视**：木牌挂绳扣下；频道页在羊皮卷下方滑入；角色/三影在桥上；三按钮贴右侧木牌；白光在角色上

### 9.6 登录 BGM 无声（2026-09-23）

- **现象**：登录界面一直无背景音乐（`BgmUI.img/Title` 资源与 BASS 独立探测均正常）。
- **修复**：
  1. `Music::play`：按 BASS 约定使用 `length = total-82`；创建/播放失败时不写入 sticky `bgmpath`；播放成功后重新应用 `BGMVolume` 并 `BASS_Start`。
  2. `BASS_Init` 绑定 SDL 窗口 HWND。
  3. 焦点静音改为按 `SDL_WINDOW_INPUT_FOCUS` 同步（避免启动时 LOST 却无 GAINED 导致音量永久为 0）。
- **诊断**：运行目录写 `_audio.log`（成功时应有 `playing BgmUI.img/Title … vol=50`）。
- **验证**：冒烟已见到 `[Music] playing BgmUI.img/Title len=3769552 vol=50`；请人工听感确认。

### 9.7 选服/退出弹窗对齐图2（2026-09-23）

决策：图2 v0.83 书页 UI；公告用北斗 `WorldNotice/BalloonForLoginProcess`；左侧四钮全建；1280×720 + viewpos。

| 项 | 改动 |
| --- | --- |
| 退出确认 | `UIQuitConfirm` 按 `backgrnd` 尺寸居中标题/确定取消，窗体居中视口 |
| GoWorld | `WS_GOWORLD_OFF` → 卷内右下 `(CH/2-140-16, CH/2-41-12)` |
| 世界名 | `WS_WORLD_NAME_ON_PANEL(-180,-95)` |
| 公告 | `LoginProcessNotice` 画「公告」底板 + `event_message`，在 `chBackgrn` **左侧** |
| 侧栏 | `step/1` 装饰 + `BtViewChoice`/`BtViewAll` stub + `BtStart` 回 `UILogin` |

Evidence：`evidence/ws-balloon-title.png`、`ws-step1.png`、`ws-selectWorld.png`。

### 9.8 选服公告/侧栏与选角微调（2026-09-24）

- 公告：改用 `WorldNotice/Balloon` 小九宫连续拼接 + `BalloonForLoginProcess/Title`，消除三段断裂。
- 侧栏：整体下移、间距加大（`Y0=128`、`GAP=16`）；北斗资源里 `BtViewChoice`/`BtViewAll` 贴图文案均为「查看世界地图」。
- 蓝蜗牛：`WS_WORLD_NAME_ON_PANEL(-222,-114)`。
- 选角：角色右移（`CS_CANVAS_TO_FRAME_X=330`）并下移（`BRIDGE_DY=20`）；右侧三钮上移（`PLANK_Y={-28,32,96}`）。

### 9.9 登录木牌后 logo → MSTitle（保留忽隐忽现）（2026-09-24）

- **原资源**：`Map/Obj/login.img/Title/logo/0`（两帧 a0/a1，delay=8000 淡入淡出），由 `MapLogin.img/0/obj/0` 挂载。
- **新贴图**：`UI/Login.img/Title/MSTitle`（397×219 单帧）。
- **实现**：`Obj` 对 `oS=login & l0=Title & l1=logo` 走 `Animation(logo源, MSTitle)`——时序/透明度仍读原 logo 帧，位图换成 MSTitle，origin 对齐原帧。
- **不改**：NX / MapLogin 物体坐标。
- **验证**：构建 Release|x64；目视木牌后为 MSTitle 且仍忽隐忽现。

### 9.10 进游戏黑屏 + 未响应（2026-09-24）

- **现象**：选角进游戏后黑屏，地图 BGM（如 `FirstStepMaster`）已播，窗口点击显示未响应。
- **根因**：
  1. `SetField` 在 fade 期间 `GraphicsGL::lock()`，且把 `UI::State::GAME` 建在解锁之前；BeiDou `UI.nx` 无 `StatusBar3.img` / `UIWindow2.img`（仅有 v83 `StatusBar.img` / `UIWindow.img`），现代 HUD 构造易拖死主线程。
  2. 若 `Stage::load` 抛错，GL 可能永不 `unlock` → 永久黑屏 + BGM 仍在播。
- **修复**：
  1. fade 回调内：先 `Stage::load`，再 `unlock`，再 `change_state(GAME)`；`try/catch` 保证解锁。
  2. 无 `StatusBar3` 时跳过 `UIStatusBar`/`UIChatBar`；`UIMiniMap`/`UIShop` 回退到 `UIWindow.img`。
  3. foothold `int16` 遍历防回绕；背景 `cx/cy<=0` 防死循环；BGM 路径无 `/` 时不再 `substr` 抛异常。
- **诊断**：运行目录 `_enter_game.log`（应有 `Stage::load ok` → `GraphicsGL unlocked` → `UI State::GAME ok` → `transfer_player ok` → `fadeprocedure done`）。
- **续修（同日）**：进图后仍黑屏未响应——`_enter_game.log` 已到 `UI State::GAME ok`，卡在随后 `UIMiniMap` 上传教学图 `miniMap/canvas`（约 5944×1311）。已跳过边长 >2048 的 canvas，仅保留最小化条。
- **续修（2026-09-28）**：日志已 done 仍纯黑+未响应——淡入后 `opacity` 仅 ~0.025、进图仍 `unload_atlas`、atlas 首帧无泵事件、双击窗过紧/页索引错误。已：淡入后 `opacity=1`、仅换图清空 atlas、`getoffset` 泵事件、canvas bitmap 二次门禁、双击 500ms+页内槽。详见 `docs/features/enter-game-blackscreen-fix/`。
- **产物**：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`（须先结束旧进程再覆盖；本会话策略 B）。
- **预留**：完整 v83 血条/聊天条需另做 `StatusBar.img` 适配；超大小地图可后续做降采样显示。
