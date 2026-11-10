# round71 · GAMEOPT / SYSOPT 设置窗按 Angel 083 复核（只读取证 + 口径标注）

## 0. 授权与性质
- User said (m09859): 若碰到 ui 布局、动画一定要确认是否修复了，如果不是按照 Angel 布局修复过的，可以重新修复；之前有很多参照 BeiDou 或者自行修复的 ui、动画都有些问题。
- User said (m11142): 记录一次有可见 PUA 输出的交付，不记录评分。
- 本轮只读取证（REA→IDA→orange-wz→.nx→运行时 open_merge 探针→像素量化）+ 对照清单 + 决策点 D-B + 一次性 Plan；**零风险源码改动只有纯注释**，0 行为字面量。

## 1. 083 权威（活 IDB 现场反编译，非旧战报）
- `CUIGameOpt::OnCreate @0x993a45`（符号 `0x993a30 size 0x4fc`）：12 个 `CCtrlCheckBox` id=1001..1012（顺序打乱）x=128、y=`33,51,69,87,105,123,141,159,177,195,213,231`（步长 18）；OK `Basic.img/BtOK2 (34,262)` / CANCEL `BtCancel2 (84,262)`；`qmemcpy(this+244, CConfig+32, 0x44)`。
- `CUISysOpt::OnCreate @0x99417a`（符号 `0x994163 size 0x789`）：6 `CCtrlSlider (99, 40/70/100/160/190/220)`（range 1000→4、其余→20）；2 `CCtrlComboBox (0,67,127,175,18)/(0,67,278,175,18)`（3 条截图路径 + 4 条怪情）；5 checkbox `(239,70)/(239,100)/(62,250)/(62,303)/(62,…)`；OK `(194,342)` / CANCEL `(244,342)`。**083 无 9 档分辨率下拉**。
- `CConfig::ApplyGameOpt @0x49ea1c` / `ApplySysOpt @0x49ea33`；`*CUIOptionMenu*` / `*StatusBar3*` 符号 0 命中。
- 结论：083 是两扇独立设置窗（CUIGameOpt / CUISysOpt），底板与勾选/滑条/下拉是运行时叠加的 Basic.img 控件；无 StatusBar3 / OptionMenu 概念。

## 2. 对照清单（当前端 vs 083）
| 项 | 083 立即数 | 当前端 | 判定 |
| --- | --- | --- | --- |
| GameOpt 12 勾选 | x128 y33+18i | x130 y34+18i | 印刷行内 +2/+1px，已对齐 |
| GameOpt OK/CANCEL | (34,262)/(84,262) | 同 | 逐字一致 |
| SysOpt 6 滑条 | 40/70/100/160/190/220 | 68/98/128/188/218/248 (+28) | 本端扩展 |
| SysOpt 下拉 | 2 个（截图/怪情） | 2 个 + 9 档分辨率 | 本端扩展 |
| SysOpt OK/CANCEL | y=342 | y=372 | 本端扩展 |
| 底板 | 旧 .nx SysOpt 299x365 | 运行时 299x396 | 不同图 |

## 3. orange-wz 门禁 + .nx 回退（AGENTS.md 合规）
- 65536B GMS key（`tools/_gms_key.b64` sha256_16 `268a8407a47ee76c`，iv `TSPHKw==`）；`load_files exclusive:false` 结构层 WORKS；10 节点 query 全部 `IMG 解析失败: UIWindow.img` / `IMG 解析失败: Basic.img`（GBK 原文留证）；`UIWindow2.img` / `StatusBar3.img` 物理缺失；`unload_all` 收口 → 合规 `.nx` 回退。
- `.nx`：`GameOpt/backgrnd 158x284`、`SysOpt/backgrnd 299x365`（旧快照）、`OptionMenu/UIWindow2/StatusBar3` MISS、`Basic.img/{CheckBox 12x12,BtOK2 47x19,Slider,ComboBox}` 真存在。

## 4. 运行时探针翻案（不拿 .nx 旧缓存当权威）
- `data.format` 默认 `img`（`Configuration.h:389`），`ResourceService` → `ImgBackend` → `ImgSource::load_standard` 对整目录 `open_merge`；`UiAssetResolve::window()` 先 `UIWindow2.img` 再 `UIWindow.img`。
- 最小 C++ 探针 `_tools/opt_probe` 对 `Data/UI` 整目录 `open_merge`：`ROOT_CHILDREN=20`，`UIWindow.img_valid=1` / `UIWindow2.img_valid=0` / `StatusBar3.img_valid=0` / `OptionMenu_valid=0`；**运行时 `SysOpt/backgrnd = 299x396`**（与当前端注释一致），旧 `.nx 299x365` 是过期快照 → 先前基于 .nx 的「+28 越界 26px」推断作废。
- 逐像素比对：旧 `.nx` 与运行时底板 `SAME_INDEX 0/365 / +28 0/365 / +31 0/365` = **两张完全不同图**，不能用行偏移推算对齐。
- GameOpt v2 检测器：不合并宽带，拆出 **12 个独立洋红格 + 11 个真实间隙**；083 `y=33+18i` 与当前端 `y=34+18i` 各精确落在第 0..11 格（真绿）。初版「合并成 215px 宽带后 inside=12/12」对两边都成立 = 假绿，已作废。
- 浅色内容带 y27..307 对两组滑条 y 都 6/6 命中、无区分度；按钮区 y330..365 浅色≈1.00、y370..395=0.00（当前 BTN_Y=372 落深色空条，083 y=342 落浅色区）—— 此项待真机/用户裁决，不凭像素私自改常量。

## 5. 决策点 D-B + 一次性 Plan（交用户裁决，本轮不实施）
- **D-B**：是否把 `UISysOpt` 的 +28/+30、9 档分辨率下拉、`BTN_Y=372` 回退到 083（滑条 40/70/100/160/190/220、无分辨率下拉、OK/CANCEL y=342）。属视觉/架构级，且运行时底板 299x396 与 083 版式对应的旧图不是同一张，须 **五屏实机截图** 才能定；按「校准≠改回原生 / 数值重合≠语义重合」不盲改。
- 一次性 Plan（裁决通过后）：① 五屏截图核对两窗三态；② 若回退，改 `UIOptionMenu.h` 常量组 + `UISysOpt` 构造/绘制/命中 + 删 9 档分辨率分支；③ `s9` 协议不动；④ harness + GATE + 真重编 + 三文档 + 零署名提交。
- **UIGameOpt 已闭环**：v2 像素测试证实与 083 对齐，不改。

## 6. 真机 backlog（PLT §37）
- 37-1 五屏：游戏设置 / 系统设置 / 分辨率下拉展开 / 滑条拖动 / OK/CANCEL 落位。
- 37-2 083 y=342 与当前 y=372 两版按钮视觉对照。
- 37-3 +28 滑条/勾选/下拉是否压印刷行。
- 37-4 9 档分辨率切换真机生效性。
- 37-5 `data.format=img` 下 `UIWindow2.img/StatusBar3.img` 缺失的退化壳表现。

## 7. 提交 scope
- 精确 `git add` 7 文件：`IO/UITypes/UIOptionMenu.h` / `IO/UIElement.h`（2 源码纯注释）/ `tools/test_round71_optionmenu_083.py` / `evidence/optionmenu-083-round71.json` / `notes/wave2-optionmenu-083-review-20261111.md` / dev-plan README / PLT README。
- 一次性脚本 / IDA 反编译 / 运行时探针 / BGRA-PNG 视觉导出 / build_log.txt 仅本地证据链留存，不进版本库，evidence JSON 已汇总路径可追溯。

## 8. 结论与教训
- 用户点名的「参照 BeiDou/自行修复 UI」在这里命中：`UIOptionMenu.h` 原注释把版式常数全部归给「截图 SAD + rs.cpp 插件」，没有 083 IDA 主路径依据 → 本轮补引 083 EA，把「哪些是 083 有、哪些是本端扩展」标清楚（第 7 例同族病候选，但 GameOpt 面已对齐、SysOpt 需裁决）。
- 教训 1：`.nx` 是旧缓存，运行时 `data.format=img` 走整目录 `open_merge`；定性必须实测，不拿缓存尺寸推断越界。
- 教训 2：像素检测合并成宽带会制造「两边都 inside」的假绿；必须按周期拆独立格 + 真实间隙才有判定力。
- 教训 3：`数值重合≠语义重合` —— 083 的 40/70/100 与当前 68/98/128 都落浅色带，命中不等于对齐。
- 教训 4：架构级/交互状态机回退（D-B/D-A/D7-A）交裁决，只读取证轮不改行为字面量，避免破坏 s9 联机。
