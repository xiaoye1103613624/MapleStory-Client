# UISysOpt 新底板 v1 · 资源清单

## 来源与决策

| 项 | 值 |
| --- | --- |
| 用户指定参考图 | `E:/MXD/扩展改动/083多分辨率/分辨率.png`，实测 **299×396** |
| 归档原图 | `reference_分辨率.png`（与上者逐字节一致） |
| 运行时纯底板 | `UIWindow.img/SysOpt/backgrnd` **299×365 origin(0,0)**，导出件 `docs/features/ui-v83-window-restore/evidence/r15-newrefs/_nx_all/UIWindow.img_SysOpt_backgrnd.png` |
| 生成底板 | `sysopt_backgrnd.png` **299×396**，1x、无 DPI 缩放 |
| 运行时接入 | `UI/SysOpt/sysopt_backgrnd.png` → `LocalPngTexture::load(0x5359534Full, {...})`（独立 atlas id，避开 `UIMapSearchPanel::ATLAS_BASE=0x4D535248` 及其后续） |
| 回退 | PNG 缺失时回退 `Texture(UIWindow.img/SysOpt/backgrnd)`（299×365），并写 WARN 日志；控件坐标仍按原生 |
| 生成脚本 | `tools/resolution/make_sysopt_bg.py`（**整帧复制**，可复跑），日志 `docs/features/multi-resolution/patches/make_sysopt_bg.log` |

## 决策变更：整帧直接贴（替代初版「剥离控件」方案）

初版为避开「双绘重影」红线，把参考图剥离控件后只取背景条带拼成 299×396 底板。但剥离版破坏了控件与底图的坐标对齐，实机出现三类错乱：
1. 背景音乐 / 效果音响滑条滑块右沿压住「大」字；
2. 截图保存位置、怪物情报两个下拉的当前项长文案溢出选择框；
3. 底部「窗口大小」标签由代码自造、被 64px 限宽裁成「大小」。

经核对，v83 原生本就是「完整窗口底板图 + 代码交互控件覆盖」模式（原版 `UIWindow.img/SysOpt/backgrnd` 即含控件烘焙），代码再画交互态覆盖其上并非双绘。因此**推翻原剥离决策，改为整帧直接贴**：

- `sysopt_backgrnd.png` 直接等于 `reference_分辨率.png`（299×396 整帧），由 `tools/resolution/make_sysopt_bg.py` 复制生成（不再剥离）；
- 静态排版（标签、边框、配色、底部「窗口大小」行）全部由图提供，代码不再自造静态标签 / 矩形；
- 仅交互控件（6 滑条、5 勾选、2 原生下拉、新增分辨率下拉、OK·Cancel）由代码在其上叠加实时态；下拉收起态盒体沿用 `Basic.img/ComboBox`（与原版一致），当前项文字加 `max_width` 防溢出。

控件坐标校准基线：参考图逐像素量取（滑条轨道右端 x≈229、下拉当前项文字限宽等），其余沿用 `_settings_map2.txt` 的 SAD=0.00 原生坐标。

## 规范例外记录（skill 门禁 16）

用户明确决策「完整的用我给的图，不需要代码重绘，按我给的图重新排版信息」= 跟图、1x、静态外观不重绘。本版底图整帧采用 `reference_分辨率.png`，静态排版（标签/边框/配色/底部「窗口大小」行）由图提供，代码不再自造静态标签与矩形；**仅交互态**（滑条位置、勾选态、下拉展开列表、下拉当前项文字）由代码叠加——这是 v83 原生「底板图 + 代码控件覆盖」模式，非双绘，无法纯烘焙。控件位图仍用 `Basic.img/ComboBox` 原生资产，故**未产出 PSD 可编辑图层版本**；替代物（本 manifest + 可复跑脚本 + 差异日志）在此显式留档。

## 复跑

```powershell
python -u e:/project/MapleStory-Client/tools/resolution/make_sysopt_bg.py
```

复跑会覆盖 `UI/SysOpt/sysopt_backgrnd.png`、归档副本与日志；如需回到原生单分辨率行为，删除该 PNG 即可（代码自动回退到 299×365 IMG 底板）。
