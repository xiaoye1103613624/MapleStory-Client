# IMG 位置属性对照（运行时数据为准）

## 0. 取证口径与工具状态

- **权威源**：运行时部署 `E:/MXD/BeiDou-Client-xy/Data/UI/*.img`，经 `MapleStory.exe --resource-probe` 的 `RT-DUMP` 产出 `E:/MXD/BeiDou-Client-xy/REA反编译/wz_dump/*_runtime.json`（20 个 UI img 完整树，`RT-CATALOG` 另有 14 命名空间结构目录）。项目红线 R1/R2：**不得信任打包 `E:/project/sdlMS/build/Data2/UI.wz`**（已证实 Stat 底板宽度 175→208 漂移）。
- **orange-wz MCP 不可用**：对这批私服改版 `.img` 内部节点解析抛 `Index 16 out of bounds for length N`，`list_children` 返回空是解析器假象而非数据缺失。故本功能不通过 orange-wz 读写 `.img`，也不裸拷 / 手改二进制；新增底板走工程已有 `Graphics/LocalPngTexture` 本地 PNG 图集通道。
- **辅助**：`tools/nx_inspect.py image <nx> <node路径> <out.png>` 可导出位图；`docs/features/ui-v83-window-restore/evidence/r15-newrefs/_uiwin_full.txt` 存 UIWindow 全树逐节点尺寸。

## 1. 设置窗相关节点

| 节点 | 尺寸 | origin | 语义 / 证据 |
| --- | --- | --- | --- |
| `UIWindow.img/GameOpt/backgrnd` | **158×284** | (0,0) | 唯一子节点；「游戏设置」标题 + 12 行粉色标签 + 12 个「同意」字样全部烘焙 |
| `UIWindow.img/SysOpt/backgrnd` | **299×365** | (0,0) | 唯一子节点；「系统设置」标题 + 10 行标题（画质调整/背景音乐/效果音响/截图保存位置/鼠标速度调整/HP 警报/MP 警报/画面效果/怪物情报/画面模式）全部烘焙 |
| `Basic.img/CheckBox/0` | 12×12 | (0,0) | 未勾（空框） |
| `Basic.img/CheckBox/1` | 12×12 | (0,0) | 已勾（白框红勾） |
| `Basic.img/ComboBox/normal/0` | 5×17 | (0,0) | 下拉左段 |
| `Basic.img/ComboBox/normal/1` | 5×17 | (0,0) | 下拉中段（可按宽度重复铺） |
| `Basic.img/ComboBox/normal/2` | 18×17 | (0,0) | 下拉箭头，原生落在 (222,127)/(222,278) |
| `Basic.img/Slider/left` | 3×8 | (0,-2) | 轨道左端 |
| `Basic.img/Slider/mid` | 1×8 | (0,-2) | 轨道中段 |
| `Basic.img/Slider/right` | 3×8 | (0,-2) | 轨道右端 |
| `Basic.img/Slider/thumbNormal` | 22×14 | (0,0) | 滑钮；实现里轨道绘制 y = row_y+1 正好抵消 origin |
| `Basic.img/BtOK2/normal/0` | 47×19 | 由 `MapleButton` 内部归还 origin | 橙·确定 |
| `Basic.img/BtCancel2/normal/0` | 47×19 | 同上 | 橙·取消 |

> `MapleButton` 必须吃**按钮根节点**（内部自取 normal/pressed/mouseOver/disabled），传叶子会静默失败；`try_maple_btn` 的 `position` 语义是贴图左上角。见 `evidence/w7-option-settings/b7-4-settings-split.md §3`。

## 2. 底栏 / 大地图 / 边框节点

| 节点 | 尺寸 | 说明 |
| --- | --- | --- |
| `StatusBar.img/base/backgrnd` | **817×71** | 原生 1x 不拉伸到 VWIDTH，`ui-v83-window-restore/README.md:67-68` 明确废弃「800 设计网格 + right_adj 居中」，原生**左贴边 x=0** |
| `StatusBar.img/base/backgrnd2` | 570×71 | 古典底栏副板 |
| `StatusBar.img/base/quickSlot` | 466×80 | 快捷格，原生右贴边 `(VWIDTH−qs_panel_w, −8)`（`evidence/w1w3-hud/coords.md:67`） |
| `StatusBar.img/base/chat` / `chatTarget` / `box` | 566×5 / 81×20 / 42×19 | classic 聊天输入行 |
| `StatusBar.img/gauge/bar` / `graduation` | 340×31 | HP/MP/EXP 三段 |
| `StatusBar.img/BtMenu/BtNPT/BtShop/BtShort` | 54×34 | 底栏按钮 |
| `MapleEzorsiaV2wzfiles.img/Common/frame1024` | 1024×768 | origin (400,300)，登录书框边框档位 |
| `.../frame1280` | 1280×720 | origin (400,300)，当前登录基准 |
| `.../frame1366` / `frame1600` / `frame1920` | 1366×768 / 1600×900 / 1920×1080 | origin 均 (400,300)；**是边框位图，不是分辨率下拉节点** |
| `MapleEzorsiaV2wzfiles.img/Base/backgrnd*` | 1280×720 | 启动界面底图 |

## 3. 分辨率选择节点：不存在

对 `wz_dump/` 全库检索 `"Resolution" | "ScreenResolution" | "soScreenResolution" | "Custom"` ⇒ **0 命中**（唯一 `Screen` 字样是 EULA/说明文案）。部署目录 `E:/MXD/BeiDou-Client-xy/` 全树 `Custom.wz` / `*.wz` / `config.ini` ⇒ 0 文件。原生注册表 `HKCU\Software\Wizet\MapleStory` 有 `soScreenResolution=0`（`registry-so.txt:3-15`），但汉化壳 `ijl15.dll` 的下拉字符串表（`ijl15-dropdown-tables.txt:2-7`，怪物情报 4 档 @0x293ec、截图路径 2 档 @0x2177c）**没有分辨率条目**。

⇒ 结论：分辨率 Combo 是插件 `E:/project/BeiDou-ijl15` 通过 `Custom.wz`（密钥 `83`，`rs_resman.cpp` 覆盖 `SysOpt/backgrnd`）新增的，**不是原生 v83 资产**；本工程用用户指定 PNG + `LocalPngTexture` 复现这一行，不伪造 IMG 节点。

## 4. 用户指定新图 `分辨率.png` 的像素属性

- 实测尺寸 **299×396**（Python/PIL 只读量取），比运行时 `SysOpt/backgrnd 299×365` 多 **31px**；新增行即「窗口大小」，底板高度需同步改为 396。
- 底部像素采样（x=150）：y=330 `(221,221,221)`、y=340 `(221,221,204)`、y=350/360 `(221,221,221)`、y=370 `(187,204,221)`、y=380 `(153,187,204)`、y=390 `(170,187,204)` ⇒ y≈365..395 有蓝色高亮横条（下拉展开/选中态样式）。
- 图中可见 11 行文字、滑条、下拉箭头、蓝色条 ⇒ 这是**完整窗口截帧（含运行时控件）**，不是纯底板。若整张作为 `backgrnd` 直接绘制，会与 `UISysOpt` 运行时叠加的 6 滑条 / 5 勾选 / 2 下拉 / OK·Cancel **双绘重影**（项目红线 R5：已实现功能须核验布局与动画，防双绘）。
- **处置**（`tools/resolution/make_sysopt_bg.py`）：与运行时 `SysOpt/backgrnd 299×365` 逐像素比对，前 365 行中「运行时控件区域」用原底板像素覆盖（保留烘焙文字与行样式），保留 y=365..395 的新增行与蓝色条样式，输出 299×396 纯底板 `UI/SysOpt/sysopt_backgrnd.png`。脚本可复跑校验，输出像素差异统计进入 `evidence/`。
- **控件落位（新增行）**：对齐插件 `rs.cpp:1973` `CreateCtrl(this,2000,0,76,338,166,18,&cp)` ⇒ 分辨率下拉盒体 `(76,338) 166×18`；插件 `rs.cpp:3313` 把 OK/Cancel 下移到 `y=372`。本工程保留原 OK/Cancel `(194,342)/(244,342)` 是否下移，须按 299×396 新图逐像素复核后决定（新图 y=338 一带仍是「画面模式/窗口大小」行区，OK/Cancel 不应与蓝色条重叠）；决策写入 `ui/uisysopt/v1/manifest.md`。
- **归档与规范**：用户明确「用它就行了」= 跟图、1x、不做 Photoshop 重绘；原图 + 纯底板 + manifest 归档到 `docs/features/multi-resolution/ui/uisysopt/v1/`，并显式记录「未出 PSD 可分图层」的决策与原因（skill 门禁 16 的例外记录）。

## 5. R5/R6 核验提示

BeiDou 的 img 是定制精简版，标准节点缺失属预期（R6）；本功能涉及节点 `GameOpt/SysOpt/backgrnd`、`Basic.img/CheckBox|ComboBox|Slider|BtOK2|BtCancel2` 在运行时 dump 中均存在，代码已有 `UiAssetResolve::usable()`、`if(!node)`、`try_maple_btn` 守卫。新增本地 PNG 缺失时须回退原 `Texture(window("SysOpt")["backgrnd"])`，并写 WARN 级日志，不得静默空白。
