# M5 Android arm64 闭环笔记

日期：2026-09-28

## 已完成

1. `build_android_deps.ps1 -Abi arm64-v8a` → `E:\Android\deps\*\arm64-v8a\`
2. 修复 `Color::Name::LIGHTBLUE`（`UINpcTalk` 引用缺失）后  
   `build_android.ps1 -Abi arm64-v8a` 成功产出 `libmain.so` + `app-debug.apk`（约 6.2 MB）
3. 可写补丁根：启动 `chdir` 外部存储；`[PATCH] writable_root=` 日志；POSIX HTTP 超时
4. 文档：`android/README.md`、`ios/README.md`、`builds/index.md` 已更新

## 真机点测（手工预留）

- [ ] `adb install` arm64 APK
- [ ] 推送 Data + xy-config（局域网 IP）
- [ ] 登录 UI / 补丁拉取 / 连 BeiDou

## Linux（M5.3 摘要预留）

顶层 CMake + `SocketPosix` + 桌面 GL；补丁走同一 `HttpClient` POSIX 路径。
