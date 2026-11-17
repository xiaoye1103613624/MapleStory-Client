# 客户端 / 部署路径备忘

- 源码仓库：`E:\project\MapleStory-Client`（C++ / SDL3）
- 部署目录（vcxproj OutDir 直接产出，不要再手动拷贝 x64\Release）：`E:\MXD\BeiDou-Client-xy`
- 原生对照客户端：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe`（x86，PE，sha256 `1198fa57ca5a7c489bae43ec13c69681d9cabe0f96762f3dc0357facf2e7d4df`）
- 原生反编译产物：`E:\MXD\BeiDou-Client-xy\REA反编译\`（095REA 子目录含 `CLogin::SendSelectCharPacket @ 0x5da2a0` 等一手证据）
- 服务端（协议/字段口径权威）：`E:\project\BeiDou-Server_s9`（OdinMS 派生 v83），监听 8484，依赖 Java21 + MySQL

## 构建 / 启动注意

1. 构建前必须 `taskkill /F /IM MapleStory.exe`，否则 LNK1104 且 `tools/build_smoke.bat` 的 EXITCODE 仍可能为 0（误判成功）。
2. `Start-Process` 启动客户端必须带 `-WorkingDirectory 'E:\MXD\BeiDou-Client-xy'`，否则 `xy-config.yaml`（CWD 相对解析）读不到，配置静默失效。
3. 实机验证软键盘/输入需服务端在 8484 完成握手；若服务端未就绪，构建通过 + 静态代码复核作为最低验证，实机项记为未跑。
