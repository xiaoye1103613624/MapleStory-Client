# SCROLL_LOCK 截图键根因与修复（2026-11-05）

## 结论

截图键失效的根因是 `IO/Window.cpp` 的 `screenshot_directory()`：

```cpp
// 修复前
if (!SDL_CreateDirectory(base.c_str()))
{
    LOG(LOG_WARN, "screenshot: cannot create " << base << " (" << SDL_GetError() << ")");
    return "";
}
```

`base` 是 exe 所在目录（或桌面），**本来就存在**；SDL3 的 `SDL_CreateDirectory` 对已存在路径返回失败，函数直接 `return ""`，`capture_screenshot()` 在 `if (directory.empty()) return;` 处早退，所以截图请求被完整处理但从不落盘。

与 KEYMAP 下发覆盖、默认键位绑定、SDL 扫描码翻译**均无关**——这三处经全链路日志实证都正常（见下）。

## 判定过程

| 层 | 检查 | 结论 |
| --- | --- | --- |
| 默认绑定 | `IO/UITypes/UIKeyConfig.h:144,191`、`IO/Keyboard.cpp:89-90` | SCROLL_LOCK → SCREENSHOT 已存在 |
| 扫描码翻译 | `IO/Window.cpp:124-125` `SDL_SCANCODE_SCROLLLOCK → GLFW_KEY_SCROLL_LOCK(281)` | 正常，日志实证 `kc=281 p=1` |
| KEYMAP 下发 | 服务端从 `keymap` 表装载；客户端 `IO/UI.cpp:212-228` 有**不依赖键表的全局截图兜底** | 不会因 KEYMAP 覆盖而失效 |
| 分派 | `IO/UI.cpp:222-227` → `Window::request_screenshot()` | 正常 |
| 消费 | `IO/Window.cpp:966-984` `end()` 在 SwapWindow 前回读 | 正常 |
| 落盘 | `screenshot_directory()` 对已存在目录 `SDL_CreateDirectory` 失败即 `return ""` | **缺陷所在** |

判定用的临时探针（每按键日志）已在验证后删除；保留的是截图路径上的低频节点日志 `perf::mark("SHOT", ...)`，受 `debug.perf_log` 开关控制。

## 修复

1. `IO/Window.cpp` `screenshot_directory()`：把目录创建改为 best-effort，不再因已存在路径的失败而放弃；真正错误由 PNG 写入结果报告。
2. `IO/Window.cpp` / `IO/UI.cpp`：补 `SHOT` 节点日志（请求 → pending → 消费 → 进入 → 目录 → 写入 → 成功/失败）。
3. `IO/UI.cpp`：追加 `GLFW_KEY_PRINT_SCREEN` 作为等价截图键（标准截图键，不替代原 SCROLL_LOCK 绑定）。

构建 `tools/build_smoke.bat` `EXITCODE=0`，部署 exe 时间戳 `2026-11-05 18:50:34`。

## 真机验证证据

注入方式：底层 `keybd_event(VK_SCROLL=0x91, scan=0x46)`（`SendKeys` 的 `{SCROLLLOCK}` 在本机不送达，详见下节）。

`boot_perf.log` 本次增量：

```text
[PERF] t=36903ms SHOT request kc=281
[PERF] t=36903ms SHOT pending
[PERF] t=36903ms SHOT consume
[PERF] t=36903ms SHOT enter
[PERF] t=36903ms SHOT dirfn ordinal=0
[PERF] t=36904ms SHOT dir=E:\MXD\BeiDou-Client-xy
[PERF] t=36915ms SHOT write E:\MXD\BeiDou-Client-xy\MapleStory-20261105_185119.png
[PERF] t=37118ms SHOT saved
```

产物：`MapleStory-20261105_185119.png`（1,112,547 字节），归档副本 `evidence/live-20261105-scrolllock-proof.png`。

## 自动化工具限制（非客户端缺陷）

- `SendKeys "{SCROLLLOCK}"` / `SendKeys "{PRTSC}"`：在本机**不产生** SDL 键盘事件（`UI::send_key` 无任何记录）。
- 底层 `keybd_event(VK_SCROLL, 0x46)`：**能**送达并得到 `kc=281 p=1`。
- 因此「登录遮罩截图」这类验收项必须用底层注入，不能用 `key_seq.ps1` 的 `key` 步骤直接发 SCROLL_LOCK。
- 真机手按物理 SCROLL_LOCK 与底层注入等价，应可正常出图。
