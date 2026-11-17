# 客户端路径记忆（换频道验证环境）

> 仅记录路径，不含账号/密码明文（凭据由用户在会话内提供，不落库）。

- 客户端运行目录：`E:/MXD/BeiDou-Client-xy/`
- 主 exe：`E:/MXD/BeiDou-Client-xy/MapleStory.exe`（并行构建会反复覆盖，以"含修复标记 + 稳定存活"为准）
- 稳定修复版备份：`E:/MXD/BeiDou-Client-xy/MapleStory_final_fixed_3b178559.exe`（最小 diff FIN 补丁，4/4 稳定）
- 换线/地图落地诊断 trace：`E:/MXD/BeiDou-Client-xy/setfield_trace.log`（Release 下 LOG 是 no-op，落地诊断只能靠该文件垫片；判据关键字 `reusing Stage char snapshot` / `set_field aborted` / `channel=`）
- 客户端配置：`E:/MXD/BeiDou-Client-xy/xy-config.yaml`（`auto: false/true` 控制自动登录；验证脚本改为 auto:true 后须恢复）
- 服务端目录：`E:/project/BeiDou-Server_s9/gms-server/`（BeiDou-Server_s9，日志 logs/out.log）
- 对照客户端源码：`E:/project/MinMS/src`
- Angel IDB：`E:/MXD/反编译IDA/v83/Angel.i64`（IDA MCP session 56a86f32）
- 验证工具脚本：`E:/project/MapleStory-Client/tools/_cc_*.py`（含启动、稳定性测试、bisect、trace 核对等；临时 CCTRIGGER 钩子已还原）
