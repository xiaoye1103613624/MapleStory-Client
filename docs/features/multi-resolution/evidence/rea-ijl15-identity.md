# REA 原生证据核验记录（2026-11-04）

## 1. 调用序列与结果

| # | 工具 | 参数 | 结果 |
| --- | --- | --- | --- |
| 1 | `rea.current_document` | `{}` | `target_unavailable` — `No app is open. Ask the user which app to investigate, then call open_binary with its local path.` |
| 2 | `rea.binary_overview` | `{}` | 同上 `target_unavailable` |
| 3 | `rea.search_strings` | `ScreenResolution` / `soScreenResolution` | 同上 `target_unavailable` |
| 4 | `rea.open_binary` | `{"path":"E:\\MXD\\BeiDou-ClientV17.7\\ijl15.dll"}` | **成功**：`{"path":"E:\\MXD\\BeiDou-ClientV17.7\\ijl15.dll","format":"pe","kind":"executable","sha256":"db2dbe66ad6217fcd43772a2df04aac52f496f44bdc4623b0a66f143ed3b29aa","architecture":"x86"}` |
| 5 | `rea.binary_overview` | `{}` | `unreadable_output`（`execution_failure`, `retryable=false`）— `Analysis returned an unreadable result. Run 'rea doctor' and review the reported details` |
| 6 | `rea.search_strings` | `soScreenResolution` / `ScreenResolution` / `800 x 600` / `CUISysOpt`（literal, case-insensitive） | 全部 `unreadable_output`（同上） |
| 7 | `rea.list_strings` | `{}` | `unreadable_output`（同上） |

## 2. 已取得的可信结论

- 目标二进制身份：`ijl15.dll`，PE / x86，SHA-256 `db2dbe66ad6217fcd43772a2df04aac52f496f44bdc4623b0a66f143ed3b29aa`。这是汉化壳（原生 SysOpt 下拉文案的载体，`w8-sysopt-behavior/README.md §2.1` 已用 `ijl15.dll@0x293ec` / `@0x2177c` 定位过字符串表）。
- **本轮没有产出任何新的反编译 / 字符串结论**：provider 对所有分析类请求返回不可读输出，工具明确提示「重复不变请求不会成功」，故未重试、未编造命中地址。

## 3. 阻塞与等效证据

- 阻塞：REA 深分析 provider（IDA headless 链路）对本次打开的 DLL 返回 `unreadable_output`。按项目规范**如实记录阻塞，禁止假装已解析**；如需补证，需先 `rea doctor` 排查 provider 输出、或换 `BeiDou.exe` 目标 / 换 provider，再把函数名、地址、伪代码摘要与局限写入本目录。
- 等效证据（已归档，本功能全部引用）：
  - `docs/features/ui-v83-window-restore/evidence/w8-sysopt-behavior/registry-so.txt`：原生注册表 `so*` 行，含 `soScreenResolution=0`。
  - `.../ijl15-dropdown-tables.txt` + `scan_ijl15.py`：`ijl15.dll@0x293ec` 怪物情报 4 档、`@0x2177c` 截图路径 2 档；**原生下拉表内无分辨率条目**。
  - `.../main-exe-negative-scan.txt`：主 exe 对这些中文串零命中（文案在汉化壳，不在主 exe）。
  - `docs/features/login-flow-ui-ida-align/notes/ui-positioning-scheme.md`：`CWndMan::GetOrgWindow(UIOrigin)` 九宫格原点 + `update_screen(new_w,new_h)`。
  - `E:/MXD/扩展改动/083多分辨率/基址.txt`：385 条 `地址+偏移 = 含 screenWidth/Height 的表达式`（原生 W/H 相对锚点）。
  - `E:/project/BeiDou-ijl15/ezorsia/compat/rs/rs.cpp`：插件已落地的档位表、Combo、切换时机、Origin 与坑（源码即证据，非反编译猜测）。
- IMG 侧主路径同样有阻塞：orange-wz 对这批私服改版 `.img` 内部节点解析抛 `Index 16 out of bounds`，不可用 ⇒ 改用运行时 nlnx dump `wz_dump/*_runtime.json` 与 `--resource-probe`（项目红线 R1/R2）。

## 4. 对方案的影响

分析结论不依赖本轮新增反编译数据：档位表、Combo 版式、切换状态机、重锚原则、输入命中坑均来自插件源码与已归档 IDA/IMG 证据；本功能不改任何二进制、不 patch 地址，因此 REA 深分析失败**不阻塞实现**，只影响「新增原生定位证据」这一项，已在 README / 验证清单 V13.2 记为显式预留。
