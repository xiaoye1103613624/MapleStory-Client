# 客户端残留进程排查报告（2026-11-21）

> **后续状态**：本报告的 A/B 修复 + 商城退出黑屏修复已全部落地、构建通过，`implementation.md` 为最终实现记录。

## 一、结论先行

当前 5 个 `MapleStory.exe` 中，**4 个是僵尸进程（Zombie）**，1 个是正常运行的游戏进程。

1. **不是内存泄漏**：4 个残留进程各仅占 28–36 KB，`ThreadCount=0`、`HandleCount=0`，说明进程早已结束，残留的只是 Windows 内核进程对象壳。真正的内存泄漏表现为 WorkingSet 持续增长（百 MB 级），与此处现象完全不符。
2. **不是客户端 C++ 代码 fork 出来的**：全仓 `*.cpp` 中 `CreateProcess* / ShellExecute* / WinExec / _spawn* / SDL_CreateProcess` 命中为 0，客户端自身不派生子进程。
3. **直接成因是外部启动器/守护脚本**：活跃进程 PID 48988 的父进程是 `D:\software\Pulse\pulse.exe`；僵尸进程的父进程均已退出，内核对象壳未被回收。`taskkill /F` 对 0 线程的僵尸进程返回 `Access is denied`，无法通过常规手段清除。
4. **代码里确实存在独立的“关不掉/卡死”隐患**（与僵尸残留无因果关系，但会造成“点关闭没反应、窗口还在”）：退出标志死值、后台线程 `detach` 无 join、主线程无超时自旋。详见第四节。

## 二、实测证据

### 2.1 进程快照（2026-11-21）

| PID | 父 PID | 线程数 | 句柄数 | 内存 | 状态 | 创建时间 | 窗口标题 |
|---|---|---|---|---|---|---|---|
| 26292 | 33612（已退出） | 0 | 0 | 36 K | Unknown | 11-17 23:29 | N/A |
| 36840 | 29236（已退出） | 0 | 0 | 28 K | Unknown | 11-18 14:53 | N/A |
| 42860 | 50140（已退出） | 0 | 0 | 32 K | Unknown | 11-18 15:09 | N/A |
| 10856 | 41368（已退出） | 0 | 0 | 32 K | Unknown | 11-19 20:38 | N/A |
| **48988** | **27000 = `pulse.exe`** | **13** | **748** | **120,168 K** | **Running** | 11-21 07:49 | XiaoYE MS v0.0.1【1-Admin】… |

活跃进程命令行：`"E:\MXD\BeiDou-Client-xy\MapleStory.exe"`。

### 2.2 kill 实测

```
taskkill /F /PID 26292
ERROR: The process with PID 26292 could not be terminated.
Reason: Access is denied.   (EXIT=1)
```

僵尸进程没有活动线程可以接收 `TerminateProcess`，因此杀不掉属预期行为，不是权限配置问题。

### 2.3 仓库既有同记录

- `docs/features/chat-input-toggle-fix-2026-11-17/README.md:44`：残留一个被登录器守护的客户端进程 PID 26292，`taskkill` 后被自动拉起。
- `docs/features/wm-triad-port-2026-11/notes/01-live-verify.md:12`：幽灵进程 pid 47560（`HandleCount=0`、无 `ExecutablePath`、`Access Denied` 杀不掉）。
- `docs/features/s9-protocol-align/evidence/crash/SHOP_UI_FOURDEFECTS_EXIT_REPORT.md:54`：3 个 32K 僵尸 MapleStory 进程无法 `taskkill -F` 终止，但无窗口无影响。

## 三、为什么不是内存泄漏

- 4 个残留进程 WorkingSet 仅 28–36 KB，且线程/句柄均为 0。进程生命周期已结束，不存在“持续增长的堆”。
- 客户端进程没有子进程创建路径，不存在“子进程泄漏父进程未 wait”的代码来源。
- 僵尸壳由外部登录器（`pulse.exe`）与历史部署/启动脚本（`tools/_launch_via_explorer.ps1`、`tools/_forceclean_launch.ps1`）反复拉起、父进程先退造成，属于 Windows 内核对象回收问题。

**处理建议**：这类僵尸不会影响系统，重启会话即自动清除；若需在不重启下消除，需找到仍持有其进程句柄的第三方（登录器、杀软、进程工具）并关闭该句柄，常规 `taskkill` 无效。

## 四、代码里真实存在的退出缺陷（独立问题，建议修复）

以下几点不会制造僵尸进程，但会造成“活进程关不掉/卡死/无响应”，是排查中发现的实质隐患：

### 4.1 退出标志是死值

- `IO/Window.cpp:429`：`close_requested = false;` —— 全仓仅此一处赋值，从不置 `true`。
- `IO/Window.cpp:602-605`：`not_closed()` 因此恒为 `true`。
- `IO/Window.cpp:789-803`：`SDL_EVENT_QUIT` / `SDL_EVENT_WINDOW_CLOSE_REQUESTED` 只调 `UI::get().send_close()`，不设标志、不拆窗口。
- `IO/UIStateCashShop.h:76`：`void send_close() override {}` —— 商城态点 X 完全无效。
- 主循环 `MapleStory.cpp:240-245, 262` 的 `while (running())` 实际只靠 `UI::quit()` 退出。

### 4.2 后台预热线程 detach 后无法收口

- `Util/Resource/ResourceService.cpp:158-193`：`std::thread(...).detach()`，无 join、无取消、无超时，捕获静态单例 `this`。
- `ensure_game_roots()` 内的 `CharLook::init()` 不在 try 中；异常时 `game_roots_ready_` 永远为 false。

### 4.3 主线程无超时自旋（最危险）

- `Net/Handlers/LoginHandlers.cpp:136-149`：

```cpp
while (!ms::res::ResourceService::get().game_roots_ready())
{
    Window::get().poll_events();
    UI::get().update();
    Window::get().begin();
    Stage::get().draw(0.0f);
    UI::get().draw(1.0f);
    Window::get().end();
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
}
```

该循环不检查 `UI::not_quitted()` / `Window::not_closed()`，且内部持续泵事件和绘制：窗口表面“活着能动”，但退出请求被吞掉——最符合“进程关不掉”的代码形态。

### 4.4 `ImgSource::close()` 无超时自旋

- `Util/Resource/ImgSource.cpp:632-650`：`while (s_bg_loading.load()) sleep 20ms;`，无超时、无异常出口。

### 4.5 退出收口依赖静态析构

- `MapleStory.cpp:319-325`：退出时仅显式调用 `Sound::close()`；`Session` / `ResourceService` / `Window` 依赖未指定顺序的静态析构。
- `Graphics/GraphicsGL.cpp:849-850`：`locked` 时 draw 全跳过；若 `lock()` 后的 fadeout 回调未 `unlock()`，表现为画面冻结型“关不掉”（历史 RCA 见 `SHOP_UI_FOURDEFECTS_EXIT_REPORT.md`，当前商城 EXIT 路径已修，其它 lock 路径仍有同类风险）。

## 五、建议修复顺序（待确认后执行）

1. `LoginHandlers.cpp:138` 自旋加：**超时上限 + 退出标志检查 + 线程异常兜底**，超时后跳出并提示而不是永久挂起。
2. `ResourceService.cpp:173-192` 把 detach 线程改为可 join 的成员线程（或 `std::jthread` + stop_token），并用 try/catch 保证任何路径都落地 `game_roots_ready_` / `s_bg_loading`。
3. `Window.cpp` 在真正退出时置 `close_requested = true`；`UIStateCashShop::send_close()` 补实现。
4. `ImgSource::close()` 自旋加超时上限。
5. `WinMain` 退出前显式调用 `ResourceService::get().close()` 收口后台资源，再让静态对象析构。

> 状态：本次仅排查与取证，**未修改任何代码**。
