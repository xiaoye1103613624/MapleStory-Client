# iOS 构建说明（脚手架 · 后置）

> 状态：**未跑通**（总纲 M5.4，排在 Windows / Android arm64 / Linux 之后）。  
> 进度以 [client-platform-masterplan](../docs/features/client-platform-masterplan/README.md) 为准。

## 前置条件

- macOS + Xcode + iOS SDK
- SDL3 iOS 框架
- OpenGL ES（`USE_OPENGL_ES`；不可用 GLEW）
- 触控与音频后端（见 `Platform/NOTES.md`）
- 可写沙箱目录存放 `Data` 与 HTTP 补丁（同 Android：APK/IPA 不打整包资源）

## 构建方式（两种）

1. **CMake + iOS 工具链**：扩展顶层 `CMakeLists.txt` 的 iOS/GLES 分支。  
2. **Xcode + SDL iOS 模板**：入口仍由 SDL 调用本仓库 `main`/`SDL_main`。

## 关键差异点

- 上下文：EAGL / Metal；无 GLEW  
- 资源：首装进 Documents；增量走 `patch.url`（`http://` 或后续 HTTPS）  
- 输入：UIKit 触控 → SDL finger → 现有 UI 命中  

完成 GLES/触控/可写目录后进入联调。
