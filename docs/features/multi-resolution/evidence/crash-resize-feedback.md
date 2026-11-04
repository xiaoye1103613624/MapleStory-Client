# 崩溃证据：SDL resize 事件回写造成的自反馈堆损坏

> 本文件记录实现期唯一的实机崩溃（1440×900 档），以及定位、修复与 A/B 复验的完整链路。
> 取证工具：`tools/resolution/analyze_crashdump.py`（WER 转储 + DbgHelp/PDB 栈符号还原）、
> `tools/resolution/monitor_viewport.py`（窗口尺寸与进程存活时间序列）、
> `tools/resolution/run_tier_matrix.py`（9 档自动矩阵）。

## 1. 现象

| 运行 | 档位 | 时长 | 结果 |
| --- | --- | --- | --- |
| baseline（旧 exe） | 1280×720 (tier5) | 55s | 稳定，窗口恒 1280×720 |
| 首次切档（旧 exe） | 1440×900 (tier6) | 55s | 窗口切到 1440×900，t=31.0s 进程退出 |
| 复测（旧 exe） | 1440×900 (tier6) | 55s | 窗口切到 1440×900，t=31.0s 进程退出 |
| 枚举宽度对照（旧 exe） | 1366×768 (tier2) | 45s | 稳定 |
| 枚举宽度对照（旧 exe） | 1920×1080 (tier4) | 45s | 稳定 |

进程退出码 `3221226356` = `0xC0000374` = `STATUS_HEAP_CORRUPTION`，Windows 应用事件日志
`Application Error 1000` 记录 faulting module `ntdll.dll`（堆检查触发点，非真凶）。

## 2. 转储证据

| dump | 时间 | 异常码 | 栈符号（按栈偏移递增，同一调用深度） |
| --- | --- | --- | --- |
| `MapleStory.exe.15936.dmp` | 16:53:55 | `0xC0000374` | `operator new` → `Singleton<Window>::instance` → `Window::initwindow+0x846` → `string` → `Configuration::reload_if_changed+0x349` → `Window::check_events+0x38` → `loop+0x18F` → `start+0x116` → `WinMain+0xF4` → `__scrt_common_main_seh+0x10C` |
| `MapleStory.exe.35964.dmp` | 16:57:29 | `0xC0000374` | 与上一行**完全一致**（相同 RVA、相同栈偏移） |

两次崩溃栈完全一致 → 确定性路径，不是随机内存踩踏。`reload_if_changed` 帧为栈上残留
（它只在 `xy-config.yaml` mtime 变化时执行，且不调用 `initwindow`），但
`check_events → initwindow → operator new` 的组合明确指向「反复建窗 + 高频分配」。

结构化证据：`evidence/live/crash-tier6-first.json`、`evidence/live/crash-tier6-second.json`
（由 `analyze_crashdump.py` 生成，含异常记录、主模块基址、解析出的符号与 RVA）。

## 3. 根因

方案 §3.2 计划在 `Window::poll_events()` 补
`SDL_EVENT_WINDOW_RESIZED` / `SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED` 回写
`Constants::set_viewsize()`，以支持「用户拖边框改客户区」。

实查 `IO/Window.cpp::init()`：`SDL_CreateWindow` 的 flags 只有
`SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN`，**没有 `SDL_WINDOW_RESIZABLE`** —— 用户拖不动边框，
这两个事件只可能由 `initwindow()` 自己的 `SDL_SetWindowSize()` 回送。

于是形成自反馈：

```
check_events() 发现 Constants 变化
  → initwindow() → SDL_SetWindowSize()
  → SDL 回送 RESIZED / PIXEL_SIZE_CHANGED
  → poll_events() 把事件尺寸写回 Constants
  → 下一帧 check_events() 又发现「变化」→ initwindow() ...
```

每帧反复 `initwindow()` 会重复 `stbi_load` / `stbi_image_free` / `SDL_CreateSurfaceFrom` /
`operator new` 等分配，最终破坏 CRT 堆，表现即 `0xC0000374`（堆损坏往往延迟到后续分配才被检测，
因此崩溃在切档后约 6 秒而非同帧）。基线 1280×720 无尺寸差分、不触发，枚举宽度档位因事件序列不同未触发，
故只有部分档位必崩。

## 4. 修复

`IO/Window.cpp` 保留两个事件 case 但不再回写 `Constants`，视口只由
`Constants → Window::check_events() → initwindow()` 单向驱动；代码注释写明：将来若要支持
手动拖边框，须先给窗口加 `SDL_WINDOW_RESIZABLE`，且只在事件尺寸与私有 `width/height` 不同时
回写，避免把主动切档误判为用户 resize。

## 5. 复验

| 复验 | exe | 结果 |
| --- | --- | --- |
| 1440×900 单档 | 17:12:44（含修复） | 45s 稳定、窗口 1440×900、无崩溃（`evidence/live/tier6-after-resize-fix.json`） |
| 9 档自动矩阵 | 17:12:44 及之后 | **9/9 PASS**（`evidence/v3-tier-matrix.csv`） |

## 6. 影响与限制

- 用户手动拖窗口边框调整大小：窗口本来不支持（`SDL_WINDOW_RESIZABLE` 缺失），
  因此 `V6.3` 判为「不适用」，不是本修复引入的退化。
- 与并发改动的区分：`16:33:51` 曾有一次 `ImgSource.cpp C2371 s_map_files` 构建失败，
  是并发编辑 `ImgSource.h/.cpp` 的中间态，随后由并发所有者自行修复，与本功能无关。
