# R28-B4 · 「系统设置」九项真生效 —— 取证与落点

> 批次：PLAN-R28 §B4。目标不是把窗画对（B7-4 已经画对），而是让这九项**真的改变客户端行为**，
> 并且每一项的量纲/档位/文案都按原生来，不靠猜。原生没这个逻辑的才允许自定，并在此写明。

## 1. 取证源（四条独立路，互相交叉）

| 路 | 具体物件 | 用来定什么 |
| --- | --- | --- |
| 原生截帧 | `%USERPROFILE%\Desktop\正确客户端截图\系统设置.png`（299x365 窗体，10 个选项行） | 行格位、滑钮落位、下拉收起态当前项文案 |
| 原生注册表 | `HKCU\Software\Wizet\MapleStory` 的 `so*` 行（只读取，未写） | 每项存量的**量纲与序号起点** |
| 汉化壳二进制 | `E:\MXD\BeiDou-ClientV17.7\ijl15.dll`（303,616 B）里的 GBK 字符串表 | 两个下拉的**条目顺序与文案** |
| IMG 资产 | `UIWindow.img/SysOpt`、`Basic.img/ComboBox`（经 `Data/.nxcache/UI.nx`） | 哪些文字是烘焙位图、下拉盒体几段 |

原始读数与脚本：`registry-so.txt`（注册表 so* 行，身份字段已剔除）、`ijl15-dropdown-tables.txt` + `scan_ijl15.py`
（下拉字符串表命中与偏移）、`main-exe-negative-scan.txt` + `scan_mobinfo.py`（主 exe / .i64 / Data 下 .nx 的负结果）、
`slider-knobs.txt` + `track_cols.py`（滑钮逐列测量与行程换算）、`nat_sysopt_rows.txt` + `rows_sysopt.py`（10 个选项行的 y 格位）。
脚本输出落在临时目录 `E:\project\_r28\`，本目录保存的是它们的拷贝。

## 2. 关键结论

### 2.1 中文文案不在主 exe，在汉化壳 ijl15.dll —— PLAN §6.4 的结论作废

先前记的「原生 exe 里 UI 文案与注册表键名都是密文，静态逆向已证伪」是**查错了文件**：

- `BeiDou.exe`（8,503,296 B）里 GBK / UTF-16LE 双编码扫「名字和HP都显示 / 只显示名字 / 怪物情报」等串 ⇒ **零命中**（`ijl15-dropdown-tables.txt` 末尾的负结果）。
- 同一批 needle 在 `ijl15.dll` 里**全部命中且各只命中 1 次**，并且以 `\0` 紧邻成串 ⇒ 那就是下拉的数据源。

⇒ 更正后的方法论：**汉化文案去 ijl15.dll 的字符串表里取，别在主 exe 里找**。这条对本轮所有"原生下拉/按钮文字"类问题都适用。

### 2.2 怪物情报 = 四档，序号自 0 起（B4-6）

`ijl15.dll@0x293ec` 起四条紧邻：

| 序号 | 文案 | `Mob::draw` 里的效果 |
| --- | --- | --- |
| 0 | 不显示 | 名字 ✗ / HP 条 ✗ |
| 1 | 只显示名字 | 名字 ✓ / HP 条 ✗ |
| 2 | 只显示HP | 名字 ✗ / HP 条 ✓ |
| 3 | 名字和HP都显示 | 名字 ✓ / HP 条 ✓ |

交叉验证：原生截帧里该下拉收起态显示「名字和HP都显示」，同机注册表 `soMobInfo = 3` ⇒ 序号从 0 起、顺序与表一致。
落点：`Gameplay/MapleMap/Mob.cpp` 的 `MOB_INFO_TIER[4]`（名字与 HP 条拆成两个独立门禁；旧实现把两者捆在 `showhp` 上，
于是"常显名字"根本做不到）。

### 2.3 截图保存位置 = 两项，1 是桌面不是"文档"（B4-7）

`ijl15.dll@0x2177c` 起两条紧邻：`MapleStory文件夹` / `桌面`；截帧当前项是前者且 `soScreenShot = 0`。

⇒ 序号 0 = `SDL_GetBasePath()`（客户端目录），1 = `SDL_GetUserFolder(SDL_FOLDER_DESKTOP)`。
先前按"文档/MapleStory"实现的 1 号分支是猜的，已改。触发键 = SCROLL LOCK（原生帧的键帽图里有这一枚），
走 `KeyAction::Id::SCREENSHOT` → `Window::request_screenshot()`，真正的回读排在 `Window::end()` 里
`flush` 之后、`SDL_GL_SwapWindow` 之前 —— 那时后台缓冲就是刚画完的这一帧。

### 2.4 滑条量纲：鼠标/HP/MP 是索引刻度，画质是离散档吸附（B4-1 / B4-5 / B4-2）

原生帧逐列扫描（`slider-knobs.txt`）：滑钮宽 20 px，三条滑条的钮都落在 x=164..183（中心 173.5），
画质那条落在 x=221..240（中心 230.5）。配上注册表 `soMouseSpeed = 10`、`soHPFlash = 10`、`soMPFlash = 10`、`soVideo = 3`：

- 10 落在轨道行程 51.0%（钮左沿 164 / 行程 88..237）⇒ **鼠标速度 / HP 警报 / MP 警报三条是索引刻度**，不是百分比。
  上限是 19 还是 20 这张帧分辨不出（两者预测差都在 2~3 px 内，见 `slider-knobs.txt`），本包按 `0..19` 反推，最大偏 1 档。
- 画质钮落在 89.3%，而 `soVideo = 3` ⇒ 与索引/百分比都成不了线性关系，**原生画质是离散档吸附**；
  档位总数没取到证据，记在 §4 预留里。

本包的 `ui.sysopt.*` 复用音量滑条管线，落盘是 0..100 百分比，所以：
- 鼠标速度按 `percent * 19 / 100` 反推档号，倍速 `(n + 5) / 10`（公式出处见 PLAN-R28 §D5：095 逆向，只做客户区内相对增益、不改 OS 设置，与用户判断一致）。
- ⚠ 默认 52% ⇒ 约 1.5×，可能偏快。**没擅自改默认**，等实测（PLAN §6.3）。
- HP/MP 警报的消费端还没接（§4）。

### 2.5 行标题是烘焙位图 ⇒ 只有下拉条目进 i18n

`UIWindow.img/SysOpt` 下**只有一个 `backgrnd`（299x365）**，没有单独的文字节点 ⇒ 「画质调整 / 背景音乐 / 效果音响 /
截图保存位置 / 鼠标速度调整 / HP 警报 / MP 警报 / 画面效果 / 画面模式 / 怪物情报」这些行标题全烤在那张底板里，
不属于客户端自绘文字 ⇒ 按「非位图文字一律走 i18n」的口径，**它们天然在 i18n 之外**。
真正由客户端画的只有下拉的当前项与展开条目，所以 i18n 只登记这六条：
`sysopt.combo.screenshot.{0,1}`、`sysopt.combo.mobinfo.{0..3}`（键名后缀 = 注册表序号）。

## 3. 九项 → 落点 → 现状

| # | 原生行 | 存盘键 | 消费方（真生效的落点） | 真机 |
| --- | --- | --- | --- | --- |
| 1 | 画质调整 | `ui.sysopt.quality` | `Graphics/EffectLayer.cpp` 每次 `add()` 现读 ⇒ 同屏特效条数上限 8..100；过半写 `window.vsync` 并立刻 `Window::apply_vsync()` | 未跑 |
| 2 | 背景音乐 | `audio.bgm_volume` | `Music::set_bgmvolume`（B7-4 已接） | 未跑 |
| 3 | 效果音响 | `audio.sfx_volume` | `Sound::set_sfxvolume`（B7-4 已接） | 未跑 |
| 4 | 截图保存位置 | `ui.sysopt.screenshot_path` | `IO/Window.cpp::screenshot_directory()` | 未跑 |
| 5 | 鼠标速度调整 | `ui.sysopt.mouse_speed` | `IO/Window.cpp::handle_mouse_motion()` | 未跑 |
| 6 | HP 警报 | `ui.sysopt.hp_warn` | **无消费方** | 阻塞，见 §4 |
| 7 | MP 警报 | `ui.sysopt.mp_warn` | **无消费方** | 阻塞，见 §4 |
| 8 | 画面效果 | `ui.sysopt.skill_effects` | `Character/Char.cpp::show_attack_effect`（施放侧）+ `Gameplay/MapleMap/Mob.cpp`（受击侧）；状态类特效（中毒/流血/无敌/升级）**不受此开关影响** | 未跑 |
| 9 | 画面模式 | `ui.sysopt.window_mode` / `ui.sysopt.alt_enter` | `Window::set_fullscreen()`（只在用户动过勾时切）+ ALT+回车门禁在 `IO/UI.cpp`；ALT+回车切换后单向镜像回 `window_mode` | 未跑 |

两个下拉（4、怪物情报）的交互按原生：**点盒体在正下方展开条目列表，点条目改序号**，不是点击循环切换。
展开列表自绘（`UISysOpt::draw_combo_list`），配色沿用 `MapleComboBox`（原生没有展开态截帧，此处显式记为自定）；
没有复用 `MapleComboBox` 是因为它在构造时缓存 `parentpos`，窗一拖动条目就错位。

## 4. 未闭环 / 预留（不留半成品）

1. **B4-5 HP·MP 警报**：消费端在 `IO/UITypes/UIStatusBar.cpp`（血条/蓝条闪烁），该文件此刻属于**并行会话的在途改动**，
   按 PLAN §4 的约定不动、不提交。已备好的口径：原生是索引刻度（本包按 0..19 反推，实测钮在行程 51%），本包存 0..100 百分比，
   接上时比较 `hppercent <= threshold`，并把 0 视为「没有」（原生档 0 就是不闪）。
2. **画质档数**：原生 `soVideo` 的取值范围/档位总数无证据（只知 =3 时钮在 92.7%），本包按百分比 + 「≥50 开 VSync」实现，
   阈值**没有原生依据**，是用户决策「接 VSync + 特效密度」下的第一版。
3. **鼠标速度默认值**：52% ⇒ 1.5×，等实测反馈再定是否改映射（不擅改默认）。
4. **下拉展开态外观**：原生无截帧，列表底色/高亮沿用本包 `MapleComboBox` 配色。

## 5. 与 PLAN 的偏差（逐条，都是"按计划做但计划本身要改"）

1. PLAN §B4-6 写「三档」，实据是**四档**（0..3）⇒ `MOB_INFO_COUNT = 4`。
2. PLAN §B4-7 的 1 号分支写"文档文件夹"，实据是**桌面**。
3. PLAN §B4-7 还要「`SysOptScreenshotPath` 从序号改为可用的目录串」+「新增 `Graphics/Screenshot.cpp`，需改 `MapleStory.vcxproj`」：
   两条都没照做。目录串没必要——原生存的就是序号，且只有两个合法取值，改成自由路径反而失去与原生对表的能力；
   回读/落盘直接放在 `IO/Window.cpp`（`screenshot_directory()` + `capture_screenshot()`），**不动 vcxproj**，
   也就避开了与并发会话同改工程文件冲突的那条风险（PLAN §B4-7 自己标了 ⚠）。
4. PLAN §B4-2 原写「原生 0..3 层 LOD：门禁放 `MapTilesObjs::draw` / `MapBackgrounds::drawbackgrounds`，先给 `Background` 补回层号 `no`」，
   并明确「VSync 留在 `window.vsync` 不动」。用户在决策轮 B 里选的是**「接 VSync + 特效密度」**⇒ 按用户口径实现，
   **LOD 远层裁剪未做**（且它需要给 `Background` 加成员，正踩类布局坑）。此项转入 §4 预留。
5. PLAN §B4-3 的「画面效果」原写 `EffectLayer::add()` 早退；实际改成两个技能特效出口各自判定 ——
   施放侧 `Char::show_attack_effect`（`Character/Char.cpp:178`）与怪物受击侧 `Mob::show_effect`（`Gameplay/MapleMap/Mob.cpp:554`）。
   `EffectLayer` 只承担 §B4-2 的密度上限。原因：`add()` 是全局漏斗，在它上面早退会把状态类特效
   （`Char::show_effect_id`：中毒/流血/无敌/被击/升级）一起吞掉，PLAN 自己要求的"不能吞"就用不了。
6. PLAN §6.4 的「静态逆向证伪」结论作废（见 §2.1）。
7. 全程**没给任何公共头的类加数据成员**（`Window` / `EffectLayer` / `UISysOpt` 只加成员函数、`constexpr`、枚举值；
   下拉展开态用文件作用域 static）。原因见 memory `msbuild-header-layout-stale-objs`：改类布局会让增量构建的旧 obj 错位，
   登录阶段 100% AV 且构建 0 error。⇒ 本轮**不需要**全量 Rebuild，也就没触发 PLAN §B6 里"要协调窗口"的那条。

## 6. 构建

`MSBuild MapleStory.vcxproj -t:Build -p:Configuration=Release -p:PlatformToolset=v142 -p:Platform=x64 -m`
⇒ 0 error（仅既有 C4005/C4100/C4101/LNK4098/LNK4099 警告），产物 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
4,547,072 B / 2026-11-03 14:10。日志 `E:\project\_r28\b4d_build.log`（GBK，需 `iconv -f GBK -t UTF-8`）。
