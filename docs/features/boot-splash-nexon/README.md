# 启动 Logo 加载动画（BootSplash）／原生启动动画（Logo.img/Nexon）

> 状态：已闭环（2026-10-23 追加"原生启动动画"阶段与 `boot.logo` 开关）
> 适用：BeiDou gms083 / HeavenClient / `data.format=img`
> 部署：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
> 决策：1B = `Logo.img/Nexon`；2A = 资源前程序化 spinner；资源加载完直接进登录页（跳过 UILogo）
> 追加决策（2026-10-23）：把 Nexon 片头从"pump 循环的静态背景"升级为**独立播放阶段**（`BootSplash::play_logo()`），
> 由 `boot.logo` / `boot.logo.frame_ms` 控制，默认开；放在 init 末尾，衔接"片头 → 登录页"。

## 1. 背景与目标

- 冷启动窗口起来后原先只有黑底 + 「Loading data」，体感像卡死。
- 目标：先出窗 → 程序化加载动画 → 资源就绪后播 Nexon 片头 → **直接进登录页**（不再播 Wizet/UILogo）。

## 2. 影响范围

| 层级 | 路径/模块 | 变更类型 |
| --- | --- | --- |
| 客户端 | `Util/BootSplash.{h,cpp}` | 新增 / 2026-10-23 增加 `play_logo()` |
| 客户端 | `MapleStory.cpp` | `boot_pump` → `BootSplash`；`set_start_shown(true)`；init 末尾 `play_logo()` |
| 客户端 | `IO/Window.{h,cpp}` | 2026-10-23 增加 `get_input_events()`（pre-UI 阶段判断"用户想跳过"） |
| 客户端 | `Configuration.{h,cpp}` | 2026-10-23 增加 `boot.logo` / `boot.logo.frame_ms` |
| 工程 | `MapleStory.vcxproj` / `.filters` | 登记源文件 |
| WZ/IMG | 无改动（读既有 `UI/Logo.img/Nexon`） | — |

## 3. 最终实现逻辑

### 3.1 主流程

```text
Window::init
  → BootSplash.pump("Loading...")          # 程序化 spinner（WZ/IMG 还没装）
  → PatchUpdater::sync
  → BootSplash.pump("Loading data...")
  → ResourceService::init                   # UI 根就绪
  → BootSplash.try_load_nexon()             # Logo.img/Nexon 帧清单
  → BootSplash.pump(...)                    # 加载期：Nexon 首帧 + 底部状态字
  → Session / Sound / Music / Stage ...
  → set_start_shown(true)
  → BootSplash.pump("Preparing...")
  → BootSplash.play_logo()                  # 原生片头：137 帧逐帧播放，约 5.5s
  → LoginBackground::init → UI::init        # UIStateLogin → UILogin
```

片头放在 init 末尾而不是资源就绪后立即播放，是为了"片头 → 登录页"直接衔接，
中间不再插回 spinner（Session 握手失败时报错路径仍在片头之前）。

### 3.2 资源节点

- `UI.nx` / `UI.img` → `Logo.img/Nexon`（137 帧；`0`=800×600，其余 720×480）
- 节点下同时存在 `Wizet`（UILogo 用；本数据集无该节点的可用外观，且 `start_shown` 恒为 true，故不启用）
- 缺节点 / 开关关闭时留在程序化 splash，打 `[BOOT] Logo.img/Nexon missing` 警告

### 3.3 原生片头的三个坑（2026-10-23 实证）

1. **容器节点不能用 `data_type()` 判存在性**：`Logo.img/Nexon` 是容器，数据层对容器一律报
   `type::none`，早先的 `Nexon.data_type() == type::none → 放弃` 让片头从未生效；
   必须用 `operator bool`（`if (!Nexon)`）。
2. **帧纹理逐帧构建**：137 帧全量常驻约 190MB 显存；`play_logo()` 每帧现场构造 `Frame`、
   用完即析构，峰值只留 1 帧。
3. **帧长写死**：wz 节点不带 `delay`（`Frame::get_delay()` 返回默认 100ms），原版节奏写在客户端代码里；
   这里用可配置的固定步长 `boot.logo.frame_ms`，默认 40ms（整段约 5.5s），并扣除本帧解码/上传耗时，
   避免越播越慢。

### 3.4 跳过

- 片头期间逐帧 `Window::check_events()`，任意键按下或鼠标按下立即结束
  （`Window::get_input_events()` 计数器，不依赖 UI 是否已初始化）。
- 关窗同样立即退出片头，交给主循环收尾。

### 3.5 配置

| 键 | 默认 | 说明 |
| --- | --- | --- |
| `boot.logo` | `true` | 原生片头总开关；false 时保持原有加载动画（行为与旧版一致） |
| `boot.logo.frame_ms` | `40` | 单帧时长（10~1000 限幅），137 帧 × 40ms ≈ 5.5s |

两项都会写进用户 `Settings`（SAVE_ORDER 已登记"启动动画"分组），也可在 `xy-config.yaml` 部署级覆盖。

### 3.6 跳过 UILogo

- `UIStateLogin` 读 `Configuration::get_start_shown()`；为 true 时直接 `emplace<UILogin>()`
- init 末尾强制 `set_start_shown(true)`，避免 Settings 里为 false 时再播一遍 Wizet

## 4. 验证清单（2026-10-23 实测）

- [x] 冷启动：窗口出现后立刻有 spinner / 进度条，而非纯黑字
- [x] 资源装完后播 Nexon 片头：137/137 帧，实测 5594ms，画面与数据一致（白底 + NEXON Logo 逐帧动画）
- [x] 片头结束后**直接**到登录木牌页，不再出现 Wizet
- [x] 任意按键/点击可跳过片头（`get_input_events()` 增量判断）
- [x] `boot.logo: false` 时退回加载动画，启动流程不变
- [x] 无 `Logo.img/Nexon` 时仍能进登录（程序化兜底）

## 5. 构建

```text
tools\build_smoke.bat        # MSBuild MapleStory.sln Release x64，产物直写部署目录
```

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
