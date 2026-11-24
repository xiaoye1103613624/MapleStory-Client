# 部署记录 — 2026-11-23 weather-time-hud

- 目标：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
- 部署前 live SHA：`C2383D21C52259C3369E399E80BE8674376FEB4061D8DA58E8225854C9FFFC23`
- 占用处理：用户授权「由你结束进程后覆盖」→ 结束 PID 22668（`E:\MXD\BeiDou-Client-xy\MapleStory.exe`），覆盖前确认无其它 live 持有者。
- 备份：`MapleStory.exe.bak-before-weathertime-20261123`（SHA 同上，已核对）。
- 产物：`build-weather-time-hud-2026-11\MapleStory.exe`（SHA `4FB49A0F094ED280D77B3AE26E56BA4A94700526405EF78E9CAE41468802A0B8`）。
- 覆盖后 live SHA（立即）：`4FB49A0F094ED280D77B3AE26E56BA4A94700526405EF78E9CAE41468802A0B8` ✅
- 防回滚复核（等待 8 秒）：`4FB49A0F094ED280D77B3AE26E56BA4A94700526405EF78E9CAE41468802A0B8` ✅ 稳定。
- 构建：`BUILD_EXITCODE=0`，日志 0 error。
- 服务端：用户选择自己重启；W4 时段常量改动在 `BeiDou-Server_s9/WeatherService.java`，**重启前不生效**。
- 真机验收：**未做**。需用户用本端客户端截图：
  1. 夜间（16:00 附近）开背包/任意弹窗 → UI 亮度与白天一致，场景仍暗；
  2. 右上 buff 下方 hh:mm 显示，且与画面明暗同步；
  3. 15:00/16:00/17:00 三段夜色由浅变深（服务端重启后）。
  截图必须本端客户区（PrintWindow），不得用参考端图。
