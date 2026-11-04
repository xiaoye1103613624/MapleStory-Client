# UISysOpt 新底板 v1 · 资源清单

## 来源与决策

| 项 | 值 |
| --- | --- |
| 用户指定参考图 | `E:/MXD/扩展改动/083多分辨率/分辨率.png`，实测 **299×396** |
| 归档原图 | `reference_分辨率.png`（与上者逐字节一致） |
| 运行时纯底板 | `UIWindow.img/SysOpt/backgrnd` **299×396 origin(0,0)**（用户经 img MCP 把该节点替换为参考图整帧；替换前为 299×365。运行期 `--resource-probe` dump 已核验为 299×396） |
| 运行时接入 | 恢复 v83 原生 `Texture(SysOpt["backgrnd"])` 直接加载（`UIOptionMenu.cpp` 构造函数取 `dimension = backgrnd.get_dimensions()`、`draw()` 直接 `backgrnd.draw()`），**不再**经 `LocalPngTexture` 读散落 PNG |
| 已弃用·散落底板 | `UI/SysOpt/sysopt_backgrnd.png`（299×396）及 `docs/.../sysopt_backgrnd.png`、`tools/resolution/make_sysopt_bg.py` 仅留作历史证据，**运行时不再引用** |

## 决策变更：整帧直接贴（替代初版「剥离控件」方案）

初版为避开「双绘重影」红线，把参考图剥离控件后只取背景条带拼成 299×396 底板。但剥离版破坏了控件与底图的坐标对齐，实机出现三类错乱：
1. 背景音乐 / 效果音响滑条滑块右沿压住「大」字；
2. 截图保存位置、怪物情报两个下拉的当前项长文案溢出选择框；
3. 底部「窗口大小」标签由代码自造、被 64px 限宽裁成「大小」。

经核对，v83 原生本就是「完整窗口底板图 + 代码交互控件覆盖」模式（原版 `UIWindow.img/SysOpt/backgrnd` 即含控件烘焙），代码再画交互态覆盖其上并非双绘。因此**推翻原剥离决策，底图采用参考图整帧（含新增「窗口大小」行）**：

- 静态排版（标签、边框、配色、底部「窗口大小」行）全部由 `UIWindow.img/SysOpt/backgrnd` 节点（299×396 整帧）提供，代码不再自造静态标签 / 矩形；
- 仅交互控件（6 滑条、5 勾选、2 原生下拉、新增分辨率下拉、OK·Cancel）由代码在其上叠加实时态；下拉收起态盒体沿用 `Basic.img/ComboBox`（与原版一致），当前项文字加 `max_width` 防溢出。

### 决策变更（v3）：回归原生 IMG 加载，弃用散落 PNG

v2 为让底板生效，在 `UISysOpt` 里加 `LocalPngTexture` 散落 PNG 覆盖（读 `UI/SysOpt/sysopt_backgrnd.png` 相对路径）。但客户端从 `E:/MXD/BeiDou-Client-xy` 启动，该相对路径在运行目录不存在 → 静默回退到 WZ 旧 `SysOpt/backgrnd`（299×365），实机「底图没切换」。

用户纠正：**UI 替换应直接改客户端 Data 里的 IMG 节点，而非散落 PNG**。故 v3：
- 用户经 img MCP 把 `E:/MXD/BeiDou-Client-xy/Data/UI/UIWindow.img` 的 `SysOpt/backgrnd` 节点替换为 299×396 整帧（运行期 `--resource-probe` dump 核验通过）；
- 撤销 `LocalPngTexture` 覆盖逻辑，`UISysOpt` 恢复 v83 原生 `Texture(SysOpt["backgrnd"])` 加载，`dimension = backgrnd.get_dimensions()` 自动取 299×396；
- `UI/SysOpt/sysopt_backgrnd.png`、`docs/.../sysopt_backgrnd.png`、`tools/resolution/make_sysopt_bg.py` 不再被运行时引用，仅留作历史证据。

控件坐标校准基线：参考图逐像素量取（滑条轨道右端 x≈229、下拉当前项文字限宽等），其余沿用 `_settings_map2.txt` 的 SAD=0.00 原生坐标。

## 规范例外记录（skill 门禁 16）

用户明确决策「完整的用我给的图，不需要代码重绘，按我给的图重新排版信息」= 跟图、1x、静态外观不重绘。本版底图整帧采用 `reference_分辨率.png`，静态排版（标签/边框/配色/底部「窗口大小」行）由图提供，代码不再自造静态标签与矩形；**仅交互态**（滑条位置、勾选态、下拉展开列表、下拉当前项文字）由代码叠加——这是 v83 原生「底板图 + 代码控件覆盖」模式，非双绘，无法纯烘焙。控件位图仍用 `Basic.img/ComboBox` 原生资产，故**未产出 PSD 可编辑图层版本**；替代物（本 manifest + 差异日志）在此显式留档。

## 历史脚本（已弃用）

`tools/resolution/make_sysopt_bg.py` 及产物 `UI/SysOpt/sysopt_backgrnd.png`、`docs/.../sysopt_backgrnd.png` 为 v2 散落 PNG 方案遗留，**v3 起运行时不再引用**，仅留作演进回溯。底图改法见上方 v3：直接改 `UIWindow.img/SysOpt/backgrnd` 节点（经 img MCP），不要再靠该脚本。
