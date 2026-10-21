# MapleStory-Client · Android 构建与联调

> 状态：**x86_64 模拟器已跑通；arm64-v8a 真机 APK 已可交叉编译产出**（2026-09-28）。  
> 总纲：[client-platform-masterplan](../docs/features/client-platform-masterplan/README.md)

目标架构：`x86_64`（模拟器）与 `arm64-v8a`（真机），`android-24`，NDK 27，OpenGL ES 3.0。

## 工具链与环境

所有工具链安装在 `E:\Android`（不动 C 盘），由 `E:\Android\env.ps1` 提供
`JAVA_HOME` / `ANDROID_HOME` / `ANDROID_NDK` / `PATH`。

- JDK 17（AGP 8 强依赖）
- Android SDK（platform-tools / build-tools / emulator / system-image）
- NDK 27.0.12077973
- Gradle 8.9（首次打包由脚本自动下载到 `E:\Android\gradle-8.9`）

一次性安装：`tools/android/setup_android_env.ps1`

## 依赖（交叉编译）

`libSDL3.a` / `libfreetype.a` / `liblz4.a` / `libsquish.a` 由
`tools/android/build_android_deps.ps1` 用 NDK 工具链交叉编译到
`E:\Android\deps/<lib>/<abi>`。

```powershell
. E:\Android\env.ps1
# 模拟器
& tools\android\build_android_deps.ps1 -Abi x86_64
# 真机
& tools\android\build_android_deps.ps1 -Abi arm64-v8a
```

## 构建 APK

```powershell
. E:\Android\env.ps1
# 模拟器
& tools\android\build_android.ps1 -Abi x86_64 -BuildType Release
# 真机 arm64
& tools\android\build_android.ps1 -Abi arm64-v8a -BuildType Release
```

产物：

| ABI | lib | APK |
|---|---|---|
| x86_64 | `android/app/src/main/jniLibs/x86_64/libmain.so` | `android/app/build/outputs/apk/debug/app-debug.apk` |
| arm64-v8a | `android/app/src/main/jniLibs/arm64-v8a/libmain.so` | 同上（Gradle 会按当前 jniLibs 打包） |

> 注意：`assembleDebug` 会打入当前 `jniLibs/` 下**已存在**的 ABI。真机安装前确认
> `jniLibs/arm64-v8a/libmain.so` 已由上一步复制；可同时保留 x86_64 以便通用 APK。

构建索引见 [`docs/features/client-platform-masterplan/builds/`](../docs/features/client-platform-masterplan/builds/)。

## 可写目录与补丁（外部存储）

启动时 `SDL_main` 将 cwd `chdir` 到 `SDL_GetAndroidExternalStoragePath()`
（应用外部存储根）。相对路径均落在该目录：

| 路径 | 用途 |
|---|---|
| `xy-config.yaml` / `Settings` | 配置（`data.format=img`、`patch.url`、`server.ip`） |
| `Data/...` | IMG/WZ/NX 资源（首装推送或整包镜像；**不进 APK**） |
| manifest 中的 `path` | 补丁下载目标（相对 cwd，原子 `.part` → rename） |
| `patch-state.json` | 最近一次同步摘要 |
| `_init_debug.log` | Android 专用启动诊断（无控制台时） |

日志：`[PATCH] writable_root=...` 打印实际 cwd。  
HTTP 补丁：POSIX 路径使用 `SO_RCVTIMEO`/`SO_SNDTIMEO`；**当前仅支持 `http://`**（HTTPS 预留）。

推荐 `xy-config.yaml` 片段：

```yaml
server:
  ip: 10.0.2.2   # 模拟器访问宿主机；真机改为局域网 IP
  port: 8484
data:
  format: img
  dir: Data
patch:
  enabled: true
  url: "http://10.0.2.2:8080/client/patch/manifest.json"
  channel: stable
  allow_skip: true
```

## 模拟器联调

```powershell
. E:\Android\env.ps1
& tools\android\run_android_test.ps1 -DataDir E:\MXD\BeiDou-Client-xy\Data
```

模拟器访问宿主机服务端用 `10.0.2.2:8484`。

## 真机 arm64 联调（摘要）

1. 构建：`build_android_deps.ps1 -Abi arm64-v8a` → `build_android.ps1 -Abi arm64-v8a`
2. `adb install -r android/app/build/outputs/apk/debug/app-debug.apk`
3. 推送 `Data`（建议归档单文件 push + 设备内解压）与 `xy-config.yaml` 到外部存储路径  
   （与 `SDL_GetAndroidExternalStoragePath()` 一致，常见为  
   `/storage/emulated/0/Android/data/com.maple.msclient/files`）
4. 补丁站与登录服用真机可达的局域网 IP，勿用 `10.0.2.2`
5. `adb logcat` 过滤 `[BOOT]` / `[PATCH]` / `[RES]`

## 移植要点

- 渲染：`USE_OPENGL_ES` → GLES3
- 入口：`__ANDROID__` 下 `SDL_main` + 外部存储 cwd + 配置重载
- 音频：`USE_BASS=0` 静音回退
- 套接字：`SocketPosix`

## 已知限制 / 后续

- 整包 Data 体积大，首装走镜像通道，增量走 HTTP manifest
- HTTPS 补丁、软键盘精修、BASS 移动版：预留
- Linux 桌面：CMake + Posix（总纲 M5.3）；iOS：见 `ios/README.md`（后置）
