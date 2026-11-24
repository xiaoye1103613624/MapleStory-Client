# 主功能按钮全失效 RCA（2026-11-20 18:49 截图）

## 症状（用户报）
截图 `XiaoYe_2026-11-20_18-49-30.png`：经典底条「上面一排图标 + 下面几个大按钮（商城/帮助/目录/界面）」点击全部无反应。

## 根因（行号级）
`IO/UITypes/UIStatusBar.cpp::UIStatusBar::send_cursor(bool clicked, Point)`。

- 当前工作副本（未提交拖拽修复）在 `if (clicked)` 分支里，**无论光标是否落在快捷格内**，都 `return Cursor::State::CLICKING`（原 2946 行）。
- 经典模式初始化必把 `quickslot_active = true`（`init_classic` 第 2356 行），所以每次点击都进这条分支 → 点击被状态栏吞掉，永远到不了 `UIElement::send_cursor`（`IO/UIElement.cpp:318-371`，那里才真正调 `button_pressed`）。
- 主功能按钮 `BtShop/BtNPT/BtMenu/BtShort` 定位在 `group_x+0/56/112/168`（2279-2282 行），而快捷格起点是 `quickslot_pos = group_x + gap`（2271-2276 行）；点在按钮上时 `classic_qs_slot_at()`（2685 行）返回 `slot<0`。旧 HEAD 版本此处 `slot<0` 时 `return UIElement::send_cursor(...)`，按钮正常；回归版丢掉了这个分支。

一句结论：**拖拽修复把「非快捷格点击也吞掉」，主功能按钮的 down 帧全部到不了 button_pressed**，与资源、IMG、服务端均无关。

## 修复
恢复 HEAD 口径（拖拽三修复全部保留）：
`send_cursor` 的 `if (clicked) && !qs_dragging` 分支内，仅当 `slot >= 0` 才记录起点并返回 `CLICKING`；`slot < 0`（主功能按钮/空白区）改为 `return UIElement::send_cursor(clicked, cursor_position)`，交回基类派发。

改动文件：`IO/UITypes/UIStatusBar.cpp`（约 2941-2956 行，含说明注释）。

## 验证
- MSBuild Release|x64 增量 Build `EXIT=0`、`error C/LNK/MSB` 零命中（构建日志 `build_clickfix.log`）。
- 产物（隔离 staging，未写 live）：`build-clickfix-stage/MapleStory.exe`，SHA256 `05b717f08a4cb68760d9ac25871a3840903ca72e4d548b294a0c1712b06911ba`。

## 部署状态：已部署（2026-11-20 19:22-19:24 实时核对）
- 前置：用户当面确认截图是本端、客户端已结束。部署前实时核对 live SHA = `67551b0b…`（18:16 并行会话版本），自 19:04 构建后未被再次替换；tasklist 里 4 个 `MapleStory.exe` 经 CIM/Get-Process 复核为 11/17–11/19 遗留僵死项（WS=0/无路径/无命令行），不锁文件。
- 覆盖：`05b717f0…` → live `E:\MXD\BeiDou-Client-xy\MapleStory.exe`，覆盖后即时 SHA = staging SHA。
- 防回滚三次复查（既有教训「立即启动 + 8s/53s」）：
  | 时点 | SHA256 | 进程 |
  |---|---|---|
  | 19:23:00 启动即时 | `05b717f0…` | PID 16716 起 |
  | 19:23:14 T+8s | `05b717f0…` | WS 2865MB |
  | 19:24:09 T+53s | `05b717f0…` | WS 7349MB，未崩 |
- **真机交互尚待用户验收**：请点一下截图里那两排主功能键（商城/帮助/目录/界面 + 上排图标），确认点击有反应；客户端版本以 live SHA `05b717f0…` 为准。
