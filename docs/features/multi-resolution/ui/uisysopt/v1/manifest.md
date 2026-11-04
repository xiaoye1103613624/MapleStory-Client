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
| 生成脚本 | `tools/resolution/make_sysopt_bg.py`（可复跑），日志 `docs/features/multi-resolution/patches/make_sysopt_bg.log` |

## 为什么不是「整张图直接贴」

参考图是**完整窗口截帧**（含运行时叠加的 6 滑条 / 5 勾选 / 2 下拉 / OK·Cancel，以及 y≈370..390 的蓝色下拉选中态）。逐像素比对：前 365 行与运行时纯底板有 **59251 个像素差异（54.29%）**（脚本日志）。若整张作为 `backgrnd`，`UISysOpt` 代码再叠加同一批控件即双绘重影（项目红线 R5）。

处理：
1. 前 365 行**整体取运行时纯底板**（等价于把参考图的控件区剥离），保留全部烘焙行标题与边框；
2. 新增 31 行取参考图 y=364..371 的浅灰背景条带平铺（该区域不含蓝色控件），底边补 1px 黑框；
3. 蓝色选中条**不烘焙**（它是下拉的运行时状态）；
4. 「窗口大小」行标签、分辨率下拉、确定/取消仍由代码绘制，坐标对齐插件 `rs.cpp`：Combo `(76,338) 166×18`、OK/Cancel `y=372`、行标签走 i18n。

最终视觉 = 用户指定图的「多一行分辨率选择」布局，但控件保持运行时可交互、不重影。

## 规范例外记录（skill 门禁 16）

用户明确决策「新 ui 图片用它就行了」= 跟图、1x、不做重绘。本次**未产出 PSD 可编辑图层版本**：原图是现成参考截帧，本版只做像素级控件区剥离与行扩展，未绘制新控件位图（控件仍用 `Basic.img/ComboBox` 原生资产）。原因与替代物（本 manifest + 可复跑脚本 + 差异日志）在此显式留档，不是遗漏。

## 复跑

```powershell
python -u e:/project/MapleStory-Client/tools/resolution/make_sysopt_bg.py
```

复跑会覆盖 `UI/SysOpt/sysopt_backgrnd.png`、归档副本与日志；如需回到原生单分辨率行为，删除该 PNG 即可（代码自动回退到 299×365 IMG 底板）。
