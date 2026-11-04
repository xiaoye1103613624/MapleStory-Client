# 路径记忆（本功能）

## 工程与部署

| 用途 | 路径 |
| --- | --- |
| 客户端源码（工作区） | `e:/project/MapleStory-Client` |
| 构建脚本 | `e:/project/MapleStory-Client/tools/build_smoke.bat`（MSBuild v142 Release/x64） |
| 构建产物（直写） | `E:/MXD/BeiDou-Client-xy/MapleStory.exe`（vcxproj 的 OutDir，**不要**再从 `x64/Release` 拷贝） |
| 运行时 img 数据 | `E:/MXD/BeiDou-Client-xy/Data/UI/*.img`（nlnx 实际读取路径） |
| 运行时权威 dump | `E:/MXD/BeiDou-Client-xy/REA反编译/wz_dump/*_runtime.json`（`UIWindow.img_runtime.json` 等 20 个） |
| 结构型目录 | `E:/MXD/BeiDou-Client-xy/REA反编译/img_catalog/catalog_*.json` |
| 部署配置 | `E:/MXD/BeiDou-Client-xy/xy-config.yaml`（`window.width=1280` / `height=720`；中文注释疑 GBK，追加时保持原字节） |
| 用户级配置 | `E:/MXD/BeiDou-Client-xy/Settings`（UTF-8 无 BOM + LF，`Configuration::save()` 写出） |
| 示例配置 | `e:/project/MapleStory-Client/xy-config.example.yaml` |
| 资源探测 | `MapleStory.exe --resource-probe`（RT-DUMP / RT-CATALOG，无需服务端）、`tools/nx_inspect.py` |
| 服务端（实机验证依赖，需用户启动） | `E:/project/BeiDou-Server_s9`，`gms-server/target/BeiDou.jar`，登录端口 `8484`，MySQL `beidou`（root/root），Java 21 |

## 参照实现与资料

| 用途 | 路径 |
| --- | --- |
| 已落地插件（权威） | `E:/project/BeiDou-ijl15/ezorsia/compat/rs/rs.cpp` / `rs.h` / `rs_resman.cpp` / `ezorsia/compat/wvs/wnd.h` / `ezorsia/dllmain.cpp` |
| 参照资料目录 | `E:/MXD/扩展改动/083多分辨率/`（`多分辨率.md`、`基址.txt`、`原文链接.txt`、`分辨率.png` 299×396、`多分辨率_帖子讨论.png`） |
| 用户指定新 UI 图 | `E:/MXD/扩展改动/083多分辨率/分辨率.png` |
| 原生客户端 / 汉化壳 | `E:/MXD/BeiDou-ClientV17.7/BeiDou.exe`、`E:/MXD/BeiDou-ClientV17.7/ijl15.dll`（303,616 B） |
| 原生 UI 逐像素证据 | `e:/project/MapleStory-Client/docs/features/ui-v83-window-restore/evidence/w7-option-settings/_settings_map2.txt` |
| 系统设置行为取证 | `.../evidence/w8-sysopt-behavior/README.md`、`registry-so.txt`、`ijl15-dropdown-tables.txt`、`slider-knobs.txt` |
| 原点方案（IDA） | `e:/project/MapleStory-Client/docs/features/login-flow-ui-ida-align/notes/ui-positioning-scheme.md` |
| 底栏 / 大地图定位 | `.../ui-v83-window-restore/README.md`、`evidence/w1w3-hud/coords.md` |
| 弹窗居中 | `.../login-modal-viewport-center/README.md`、`evidence/w4-chatbar/whisper-dlg-prep.md` |

## 不存在的路径（勿再找）

- `E:/pro/*`（`多分辨率.md` 里的旧路径）、kaentake 源码、`_kaentake_ref/src/resolution.cpp`、Cosmic 源码、`E:/project/BeiDou-ijl15/Custom.wz`。
- 部署目录下 `Custom.wz` / `*.wz` / `config.ini`；`wz_dump/` 中任何分辨率选择 IMG 节点。
- `E:/MXD/BeiDou-Client-xy/docs/features/`、`.../REA反编译/docs/features/`（本功能文档在 `e:/project/MapleStory-Client/docs/features/multi-resolution/`）。

## 工具链坑

- orange-wz HTTP MCP：对这批私服改版 `.img` 内部节点解析报 `Index 16 out of bounds`，不可用；且会话独占锁需 `exclusive=false` + `unload_all`。
- REA MCP：本会话 `current_document` 先返回 `target_unavailable`；`open_binary` 打开 `ijl15.dll` 成功但深分析 `unreadable_output`。见 `evidence/rea-ijl15-identity.md`。
- `read_file` 显示截图降采样到 1080 宽；点击坐标须 ×1280/1080 换算。
- Release 下 `LOG()` 编译为 no-op；排障写一次性文件日志，闭环后删除。
- 构建前必须结束 `MapleStory.exe`；改 `UIElement::Type` 枚举 / 公共头类布局必须整工程 `-t:Rebuild`。
- 用户允许直接启停 MapleStory 客户端（`taskkill` / 启动 exe），无需逐次询问。
