# build_smoke 记录（2026-11-12）

- 命令：`tools/build_smoke.bat`
- GATE：`GATE OK: opcode matrix matches both sources (s9 a0fa1926b6)`
- 首次调用：日志重定向遇「文件正被另一进程使用」（并行 MSBuild 占用 `build_log.txt`），脚本仍打印 `EXITCODE=0` 不可信
- 现场：MSBuild pid 仍在编译；`UIWorldSelect.cpp` / `WaveAShellHandlers.cpp` 已出现在 log 且无 error 行（仅既有 warning）
- 终态补记：log 末段 `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe`；部署 exe `LastWriteTime=2026-11-12 11:24:56` Size=4993024；GATE OK；未见 `error C*`；并行第二 MSBuild 可能因 exe 占用告警，以首次成功链接为准。
