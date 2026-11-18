# 客户端与逆向路径记忆（contextmenu/charinfo calibration）

## 参照端 Angel v83（权威）
- IDA 数据库：`E:\MXD\反编译IDA\v83\`Angel.i64`
- GUI-bound IDA MCP：**端口 13337**（`server_health` 显示 idb_path=`...\`Angel.i64`，module=`\`Angel.exe`，imagebase=0x400000，hexrays_ready=true，auto_analysis_ready=true）
  - 该端口**无需 database 参数**，`decompile` / `py_eval` / `find_regex` / `xrefs` 均可用。
  - 工具名与参数：`lookup_funcs` 需要 `queries`（数组）；`find_regex` 用 `pattern`；`search_text` 用 `pattern`；`py_eval` 用 `code` + `new_locals`。
- 端口 13339 是 headless 多会话版（`idb_list`/`idb_open`/`database` 参数），无法直接打开被 GUI 锁定的 `\`Angel.i64`（报 `Failed to open database`）。本功能取证一律走 **13337**。

## 当前端（被修）
- 源码：`E:\project\MapleStory-Client`
- 运行时部署数据（像素级 ground truth）：`E:\MXD\BeiDou-Client-xy\Data\`（nlnx 访问）
- 构建产物 OutDir：`E:\MXD\BeiDou-Client-xy\`（build_smoke.bat 直写，无需 robocopy）
- 构建：`tools/build_smoke.bat`（VS2019 BuildTools MSBuild v142，Release/x64）；构建前先结束 MapleStory.exe。

## 取证脚本（本次新增，均只读）
- `tools/ida_angel_gui_pyeval.py` — 在 13337 上定位函数地址
- `tools/ida_angel_gui_decompile.py` — 反编译 CUIContextMenu ctor/OnCreate + xref
- `tools/ida_angel_gui_rbutton.py` — 反编译 CUserLocal::HandleRButtonClk
- `tools/ida_angel_gui_hit.py` — 反编译命中检测 sub_9726EA / sub_53088F
- 参考：`tools/ida_angel_corewindow_decompile.py`（其 full.json 已含 CUIUserInfo 全方法）
