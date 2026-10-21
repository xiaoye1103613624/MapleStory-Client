# 平台抽象与手机端移植边界

> **进度以总纲与 android/README 为准（2026-09）**：  
> [`docs/features/client-platform-masterplan/README.md`](../docs/features/client-platform-masterplan/README.md) ·  
> [`android/README.md`](../android/README.md)。  
> Android：x86_64 模拟器已跑通；**arm64-v8a 依赖与 APK 可交叉编译**（见 `android/README.md`）。  
> 补丁写入外部存储 cwd；Linux/iOS 仍后置。冲突时以总纲为准。

本文件说明当前已做的平台抽象边界，以及手机端（Android/iOS）真正跑起来还需要哪些改动。
本仓库的 `CMakeLists.txt` 与 `android/`、`ios/` 提供构建入口；桌面与 Android 模拟器能力见上述文档。

## 1. 已抽象、跨平台已干净的部分

- **网络套接字**：`Net/Session.h` 按平台选后端 —
  Windows → `SocketWinsock`，非 Windows → `SocketPosix`（BSD socket）。
  握手（HANDSHAKE_LEN 字节读取）逻辑三套后端一致。
- **机器指纹 HWID**：`Util/HardwareInfo.h` —
  Windows 走盘序列号 + MAC；POSIX/macOS 读 `/etc/machine-id` + `getifaddrs` MAC；
  其它平台留空（不影响启动，仅影响账号/机器绑定类逻辑）。
- **音频**：`Audio/Audio.cpp` 用 `USE_BASS` 守卫。有 BASS 库则正常发声，
  否则编译为静音回退（方法签名不变，链接仍然通过）。
- **外部链接/浏览器**：`ShellExecuteA` 已全部替换为 `SDL_OpenURL`。
- **窗口/事件/GL 上下文**：`IO/Window.cpp` 已是 SDL3（`SDL_CreateWindow` + `SDL_WINDOW_OPENGL`），
  桌面跨平台无差异。

## 2. 手机端必须补的三块（按优先级）

### 2.1 渲染器：桌面 GL → OpenGL ES
- 当前：`Graphics/GraphicsGL.cpp` 使用 GLEW + 桌面 OpenGL；`MapleStory.cpp` 创建
  `SDL_WINDOW_OPENGL` 上下文。
- 需要：
  1. 新增 `Graphics/GraphicsGLES.cpp`（或 `#ifdef USE_OPENGL_ES` 分支），用 `GLES/gl.h`、
     `EGL` 创建/绑定上下文，替换 GLEW 初始化。
  2. 着色器与扩展（如果用到 GL 3.x 特性）需降到 GLES 2.0/3.0 子集。
  3. `CMakeLists.txt` 已留 `MOBILE_OPENGL_ES` 选项 → 定义 `USE_OPENGL_ES` 供渲染器切换。
- 验证：Android 上能看到登录界面即过。

### 2.2 输入：键鼠 → 触控
- 当前：`IO/Keyboard.cpp`、`IO/Mouse.cpp`、SDL 键鼠事件（`Window::check_events`）。
- 需要：在 `Window::check_events` 增加 `SDL_FINGERDOWN / SDL_FINGERMOTION / SDL_FINGERUP`
  的映射，把触点换算成 UI 坐标，复用现有 `UI` 命中测试。
- 移动端还需「虚拟键盘」输入账号密码（SDL 软键盘 / 原生 EditText 桥接）。

### 2.3 音频：BASS → 移动端后端
- 选项 A：提供 Android/iOS 版 `libbass`，沿用现有代码（最简单，需授权库）。
- 选项 B：加一个 `AudioSDL`（基于 SDL_audio）作为 `USE_BASS` 之外的后端。

### 2.4 资源打包
- `.nx`/`.wz` 需随 APK/IPA 进 assets；`DataFiles`/NoLifeNx 需改为从
  `SDL_RWops` / `AAssetManager` / `NSBundle` 读取，而非直接文件路径。

## 3. 入口脚手架现状

- `android/CMakeLists.txt`：以 `add_library(main SHARED ...)` 产出 `libmain.so`，
  经 SDL 的 Android 主循环调用本仓库 `main()`；当前**依赖 2.1 渲染器移植**才能链接/运行。
- `android/build.gradle`：最小 Gradle 骨架，引用 SDL 的 `SDLActivity` 模板。
- `ios/README.md`：Xcode / CMake 的 iOS 工具链提示。

完成 2.1–2.4 后，手机端即可进入联调。
