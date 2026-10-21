# Builds

| 配置 | 命令 | 结果 |
| --- | --- | --- |
| Release \| x64 | `MSBuild MapleStory.vcxproj /p:Configuration=Release /p:Platform=x64` | 本功能相关 TU 已产出 obj：`MobDropData.obj`、`UIMapMobPanel.obj`、`UIMapDropPanel.obj`、`UIWorldMap.obj`（`x64/obj-Release/`，2026-09-28）。全量链接仍可能被无关单元 `Util/Patch/Sha256.cpp` 的 `fopen` C4996 挡住（非本功能引入）。 |

CMake：`Data/`、`IO/` 使用 `GLOB_RECURSE`，新增文件无需改 `CMakeLists.txt`。
