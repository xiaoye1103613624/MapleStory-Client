# 跨平台构建（Windows / Linux / macOS / MinGW / 手机端）

> 状态：进行中（桌面端已可用，手机端为脚手架 + 待办）
> 适用：MapleStory-Client（BeiDou gms083 客户端）

## 1. 为什么要做

原仓库只有 MSBuild 的 `MapleStory.sln` / `MapleStory.vcxproj`（仅 Windows Release/x64）。
本变更新增 `CMakeLists.txt`，让同一份源码可在 **Windows（MSVC/MinGW）、Linux、macOS**
下构建；并为 **Android / iOS** 提供构建入口脚手架。

源码层做了平台抽象（见第 4 节），保证代码本身能在上述平台编译。

## 2. 桌面端构建

### 2.1 Windows（MSVC，已验证 ✅）

两种等价方式：

```bat
rem 方式 A：沿用原 MSBuild 流程（产物直接进部署目录 E:\MXD\BeiDou-Client-xy）
tools\build_smoke.bat

rem 方式 B：CMake（产物在 build_cmake\Release）
cmake -S . -B build_cmake -G "Visual Studio 17 2022" -A x64
cmake --build build_cmake --config Release
```

`CMakeLists.txt` 的 Windows 分支直接使用 `includes/` 下预编译的库：
`glew32s / freetype / SDL3 / bass / NoLifeNx(源码编译) / zlib / libsquish / liblz4`，
并自动把 `freetype.dll`、`liblz4.so.1.8.2.dll` 复制到可执行文件旁。
`bass.dll` **不在仓库内**，运行期由部署包（E:\MXD\BeiDou-Client-xy 已含）提供。

### 2.2 Windows（MinGW）

```sh
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

MinGW 复用 `includes/` 下同一套 Windows 库（路径与 MSVC 一致）。

### 2.3 Linux / macOS

依赖通过 `find_package` 解析（推荐用 vcpkg / Homebrew / 系统包）：

```sh
# 依赖：SDL3, Freetype, OpenGL, GLEW, zlib, libsquish, lz4, NoLifeNx(源码)
#   vcpkg: vcpkg install sdl3 freetype glew
#   brew : brew install sdl3 freetype glew

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH="$HOME/vcpkg/installed/x64-linux"   # 若用 vcpkg
cmake --build build
```

说明：
- `NoLifeNx`、`libsquish`、`lz4` 仓库未提供 Linux/macOS 预编译库，需自行编译或经 vcpkg 提供，
  通过 `NOLIFENX_ROOT` 等变量指到对应 `.so/.dylib`。
- `BASS` 为可选音频后端：提供 `BASS_ROOT` 即链接，否则编译为**静音回退**（`USE_BASS=0`）。
- 渲染器为桌面 OpenGL（GLEW），在 Linux（GLX）/ macOS 均可工作。

## 3. 手机端（脚手架 + 待办）

`CMakeLists.txt` 含 `ANDROID` 分支（链接 `SDL3 + GLESv2/EGL + log/android`）。
`android/`、`ios/` 目录提供了构建入口骨架。

**当前限制（必须完成才能真正运行）：**
1. **渲染器是桌面 OpenGL（GLEW）**，手机需要 **OpenGL ES** 后端。需新增
   `Graphics/GraphicsGLES.*` 并以 `USE_OPENGL_ES` 宏切换（`MapleStory.h` 已预留）。
2. **输入层是键鼠**（`IO/Keyboard`、`Mouse`），手机需 **触控** 抽象（SDL touch 事件）。
3. **音频用 BASS**，手机需改用 **SDL_audio / OpenSL / AAudio**（或提供移动端 BASS 库）。
4. 资源（`.nx`/`.wz`）需随包进入 APK/IPA 的 assets，并由 `DataFiles`/NoLifeNx 以
   移动端文件接口读取。

详见 `Platform/NOTES.md`。

## 4. 已做的源码平台抽象

| 模块 | 改动 | 说明 |
| --- | --- | --- |
| `MapleStory.h` | 自动选择后端 | Windows 默认 `USE_BASS`；非 Windows 自动 `USE_POSIX_SOCKET`（不再依赖 Winsock/Asio 头） |
| `Net/SocketPosix.{h,cpp}` + `Net/Session.h` | 新增 POSIX(BSD) socket | 非 Windows 用 BSD 套接字，接口与 Winsock/Asio 一致；握手读取逻辑不变 |
| `Util/HardwareInfo.h` | HWID 按平台实现 | Win 走盘序列号+MAC；POSIX/macOS 读 `/etc/machine-id` + `getifaddrs` MAC；其它平台留空 |
| `IO/UITypes/UILogin.cpp`、`UICashShop.cpp`、`UICharSelect.cpp` | `ShellExecuteA` → `SDL_OpenURL` | 浏览器打开链接统一走 SDL3 跨平台 API；移除多余 `<windows.h>` 依赖 |
| `Util/WzRuntime.cpp` | `<Windows.h>` 限定 `_WIN32` | WZ 运行时（非默认后端）仅 Windows 走 Windows API |
| `Audio/Audio.cpp` | BASS 守卫 `USE_BASS` | 无 BASS 时编译为静音回退，保证非 Windows 可构建 |
| `Gameplay/Physics/PhysicsObject.h` | 补 `#include <cmath>` | 提供 `std::round`（原先靠 MSVC 强制包含） |

> 注：原 `UICharSelect.cpp` 中 `CS_SLOT_INSET_EMPTY` / `CS_SLOT_INSET_OCCUPIED` 两个常量定义
> 曾在前次 UI 改动中被误删（仍被引用），本次已恢复（`1348-1208` / `1384-1208`）。

## 5. 复验清单

- [x] Windows MSBuild 部署构建：EXITCODE=0（exe 落 E:\MXD\BeiDou-Client-xy）
- [x] Windows CMake 构建：Release/x64 干净通过，产出 `MapleStory.exe` + DLL 复制
- [ ] Linux / macOS 构建：需提供 NoLifeNx/libsquish/lz4 + 可选 BASS，本环境无法验证
- [ ] MinGW 构建：同上（路径复用 includes/）
- [ ] 手机端运行：依赖第 3 节渲染/输入/音频移植（未在本环境完成）
