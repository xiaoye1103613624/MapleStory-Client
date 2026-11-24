# 残留进程 + 商城退出黑屏 修复实现记录（2026-11-21）

## 一、用户诉求

1. 排查"客户端结束后残留进程 / 多份进程存活 / 杀不掉"是否为内存泄漏。
2. 确认"进入商城后再退出商城，游戏画面黑屏"是否与排查结论同源。
3. A、B 两组修复全部落地。

## 二、诊断结论（回顾）

- **残留进程 ≠ 内存泄漏**：4 个残留 `MapleStory.exe` 均为 0 线程 / 0 句柄 / 28–36 KB 的僵尸进程，`taskkill` 返回 `Access is denied`；活跃游戏进程父进程为外部登录器 `D:\software\Pulse\pulse.exe`。客户端源码无任何 `CreateProcess/ShellExecute`，不派生子进程。属外部登录器反复拉起 + 父进程退出未回收的内核对象残留，重启会话即清除。
- **商城黑屏 = GraphicsGL lock 同源、不同出口**：与上一轮报告的 `GraphicsGL::draw` 的 `if (locked) return` 同一机制，但触发点是 EXIT 按钮出口：
  - 11-17 修过 BtExit 的 `unlock()`，11-18 修过 ESC 路径 `exit_cashshop()` 的本地换图；
  - 但 BtExit 出口只发 `ChangeMapPacket()` + `change_state(GAME)` + `unlock()`，**没有本地 `Stage::load`**——Stage 经 `Stage::clear()` 停在 INACTIVE、世界还是商城预览，画面恢复全押服务端 SET_FIELD 回包；回包迟到/丢失就永久黑屏（进程活着、关不掉）。
  - `Window::updateopc()` 的 catch 只恢复 opacity、不 `unlock()`——fadeout 回调在 `unlock()` 之前抛异常时（如 `Stage::load`/`change_state` 抛错），黑罩揭开但 GL 仍锁，同样永久黑屏。

## 三、实现改动（共 8 处 / 7 文件）

### A 组 — 自旋与后台线程收口

| 文件 | 改动 |
| --- | --- |
| `Net/Handlers/LoginHandlers.cpp` | 登录等 `game_roots_ready` 自旋加 60s 超时 + `UI::not_quitted()` / `Window::not_closed()` 退出门闩；超时/退出时记 WARN 并 break，不再永久吞掉退出请求。 |
| `Util/Resource/ResourceService.cpp` | detach 预热线程把 `ensure_game_roots()` / `ImgSource::ensure_remaining()` 各自包 try/catch，异常只记 ERROR 日志，**保证 `game_roots_ready_` 与 `bg_loading` 标志在退出路径一定落地**。 |
| `Util/Resource/ImgSource.cpp` | `close()` 等 `s_bg_loading` 自旋加 60s 上限，超时记 WARN 后继续清表（互斥锁仍保护容器）。 |

### B 组 — 退出标志与商城 send_close

| 文件 | 改动 |
| --- | --- |
| `IO/Window.h` / `IO/Window.cpp` | 新增 `Window::request_close()` 置位 `close_requested`（此前该标志只在构造期赋 false，全仓无处置 true，`not_closed()` 恒真）。 |
| `IO/UI.cpp` | `UI::quit()` 联动调用 `Window::get().request_close()`，两个退出门闩一致。 |
| `IO/UIStateCashShop.h` / `.cpp` | 商城态 `send_close()` 由空实现改为取 `UICashShop` 调 `exit_cashshop()`——点窗口 X / SDL_QUIT 退回游戏，不再"点关闭没反应"。 |

### 商城黑屏（本轮新增）

| 文件 | 改动 |
| --- | --- |
| `IO/Window.cpp` | include `GraphicsGL.h`；`updateopc()` 的 catch 补 `GraphicsGL::get().unlock()`，与三条正常退出路径的显式 unlock 幂等双保险。 |
| `IO/UITypes/UICashShop.cpp` | BtExit fadeout 回调在 `change_state(GAME)` 后补本地 `stage.load(真实 mapid, portalid)` + `transfer_player()`（对齐 11-18 的 ESC 路径），并 try/catch 包住保证 `enable()`/`unlock()` 一定执行。服务端 SET_FIELD 随后权威刷新一次（同 mapid 幂等）。 |

## 四、验证

- lint：9 个改动文件 `read_lints` = 0 诊断。
- 构建：`tools/build_smoke.bat`，协议门 `GATE OK`，`EXITCODE=0`；`build_log.txt` 显示 `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe`，仅既有 `glew32s.lib warning LNK4099`（与本次改动无关）。
- 部署产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`，5,125,120 B，2026-11-21 09:21:08，SHA256 `A43B0A1642AF8C0C5A72BAEFC4B315E0C1B91CE9BE92B8E9CDB5C8C18CE82883`（改前 `456073F8…`，已刷新）。
- 构建前已 taskkill 客户端进程与 mspdbsrv.exe（规避 LNK1104/C1041）。

## 五、未跑项与边界说明

- **未做真机交互验收**：本会话无法可靠驱动游戏内点击（历史记录输入注入受阻）。需用户在屏前实测：①进商城→点 EXIT→画面恢复游戏地图、FPS 持续跳；②商城内点窗口 X→退回游戏；③Alt+F4→确认退出→进程真正消失；④登录期断网/服务端无响应时加载条不永久卡住。
- 僵尸进程无法从代码消除：需重启会话或排查 `pulse.exe` 句柄持有；本次修复只防"活进程关不掉"，不处理已成僵尸的 4 个残留壳。
- 60s 超时是保守值：正常预热 ~25s，超时仅作异常兜底，不影响正常路径。

## 六、回滚

改动均为独立 patch 点，无新增依赖/配置。回滚按文件还原即可；无需数据迁移。
