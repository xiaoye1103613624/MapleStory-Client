# 构建产物索引

大文件不进 git；此处只记路径与校验方式。

## Android arm64-v8a（2026-09-28）

| 项 | 路径 |
|---|---|
| 静态依赖 | `E:\Android\deps\{sdl3,freetype,lz4,squish}\arm64-v8a\` |
| `libmain.so` | `build-android/libmain.so` → `android/app/src/main/jniLibs/arm64-v8a/` |
| APK | `android/app/build/outputs/apk/debug/app-debug.apk`（约 6.2 MB，含当前 jniLibs） |
| 构建日志 | 本目录 `arm64-build.log`（若已生成） |

命令：

```powershell
. E:\Android\env.ps1
& tools\android\build_android_deps.ps1 -Abi arm64-v8a
& tools\android\build_android.ps1 -Abi arm64-v8a -BuildType Release
```

## Android x86_64

见 `android/README.md`；依赖在 `E:\Android\deps\*\x86_64\`。

## Windows

`MapleStory.vcxproj` 或顶层 `CMakeLists.txt` Release。
