# 原生客户端 / 插件实现对照与借鉴清单

## 0. 对照源与可信度

| 源 | 路径 | 状态 |
| --- | --- | --- |
| 参照资料 | `E:/MXD/扩展改动/083多分辨率/多分辨率.md`、`基址.txt`、`原文链接.txt`、`分辨率.png`、`多分辨率_帖子讨论.png` | `多分辨率.md` L107-129 的「现状校正（2026-09-17）」写「Phase 2 Custom.wz / SysOpt Combo 未做」，但**当前磁盘源码已过期**：`rs.cpp` + `rs_resman.cpp` 已实现 |
| 已落地插件 | `E:/project/BeiDou-ijl15/ezorsia/compat/rs/rs.cpp`（138 KB）、`rs.h`、`rs_resman.cpp`、`ezorsia/compat/wvs/wnd.h`、`dllmain.cpp` | **权威**，优先级最高 |
| 地址公式 | `E:/MXD/扩展改动/083多分辨率/基址.txt`（385 行 `地址+偏移 = 含 screenWidth/Height 的表达式`） | 原生 exe 校准，仅作定位依据，不改二进制 |
| 已归档 IDA/REA | `docs/features/ui-v83-window-restore/evidence/w8-sysopt-behavior/registry-so.txt`、`ijl15-dropdown-tables.txt`、`docs/features/login-flow-ui-ida-align/notes/ui-positioning-scheme.md` | 本轮 REA 深分析失败，沿用这些既有产物 |

`E:/pro/*`（旧文档里的路径）、kaentake 源码、Cosmic 源码均**不存在**；`E:/MXD/扩展改动/083多分辨率/多分辨率_帖子讨论.png` 文字在当前比例下不可辨读，docs 无转录，故讨论细节以 `多分辨率.md` 现状校正 + `rs.cpp` 为准。

## 1. 档位表（直接借鉴，用户 D2 已确认）

`rs.cpp:49-63 rs_tier_dims()` / `rs.h:4-8 RS_TIER_COUNT 9 / RS_TIER_MAX 8 / MAX 2560×1440`：

| tier | 宽×高 | tier | 宽×高 |
| --- | --- | --- | --- |
| 0 | 800×600 | 5 | 1280×720 |
| 1 | 1024×768 | 6 | 1440×900 |
| 2 | 1366×768 | 7 | 1680×1050 |
| 3 | 1600×900 | 8 | 2560×1440 |
| 4 | 1920×1080 | | |

`rs_tier_from_dims()` L65-76 阈值反查：`≥2500/1400→8`、`≥1900/1000→4`、`≥1680/1000→7`、`≥1600/880→3`、`≥1440/880→6`、`≥1360/700→2`、`≥1280/700→5`、`≥1000/700→1`，否则 0。Combo 文案 L1974-1984（`"800 x 600"` … `"2560 x 1440"`）。

## 2. SysOpt 分辨率 Combo（借鉴版式，不改二进制）

- `CUISysOpt::OnCreate_hook` `rs.cpp:1964-2009`：`CreateCtrl(this, 2000, 0, 76, 338, 166, 18, &cp)`；配色 `0xFFEEEEEE / 0xFFA5A198 / 0xFF999999`；必须用**裸指针 `CCtrlComboBox*`**（L1887-1890：`ZRef::operator=` 会存 MI 调整后的 +8 子对象，导致 `CreateCtrl` this 错误、SysOpt 崩溃）。
- `SetSelect(rs_tier_from_dims(rs_width, rs_height))` L1988-1996 显示**真实当前尺寸**，避免配置默认 0 误显示「800 x 600」；`m_nSelect` 在 `+0x68`，配了读回校验。
- `rs.cpp:3313 Patch4(0x009945BC + 1, 372)`：OK/Cancel 由 y≈342 下移到 **372** 给 Combo 腾空间。
- `rs_resman.cpp`：Custom.wz 探测（DLL 目录 / exe 目录 / CWD）、`pkg->Init(L"83", L"Custom", arch)`、`Mount("/")`，覆盖 `SysOpt/backgrnd`，`InitializeResMan_hook` fail-soft。
- `Destructor_hook` L2011-2025：保存 `m_nSelect`，**窗口销毁后**才 `rs_switchToTier(t)`；注释「switch-under-modal + StatusBar recreate hung」。
- `ApplySysOpt_hook` L2072-2135：`bApplyVideo=0` 时只记录；OK 时**立即**写 `soScreenResolution` + `rs_persist_tier_to_ini()` 再排队延迟切换（L2119-2126 注释：崩溃会丢 WritePrivateProfile 缓存，重启回登录尺寸）。

⇒ 本工程借鉴：**显示真实档、OK 立即持久化、延迟到窗口销毁后应用**，但不用裸指针/不改 vtable/不 patch。

## 3. 登录态 / 游戏态切换（借鉴状态机）

- `rs.cpp:30-36`：`rs_width/height` 初值 800×600，`rs_login_w/h=800/600`，`rs_field_follow_login=true`；`rs_set_login_dims()` L42-47。
- `rs_afterSuccessfulSwitch(nw,nh,grChanged,loginUi)` L1505-1609：`loginUi=true` → 关 Origin、重放 `UpdateResolution` 登录布局、强制登录原点居中 `org->RelMove(-W/2,-H/2)`（L1543-1549）；`loginUi=false` → 首次装 hooks + 重建 HUD，后续只 rematch Origin/metrics（L1587-1601）。
- L1505-1509 明确坑：**SysOpt 选 1280×720（与 config.ini 登录尺寸相同）曾误走登录路径**，导致 Origin 关闭 + 无 ReloadBack → 黑屏/挂。L1560-1565：HD + 无 Origin 会留下 800×600 孤岛外的点击/丢道具死区与 Notice/YesNo 偏移。
- `多分辨率.md:79`：`set_stage_hook` Field 用所选分辨率、登录/选角回 800×600（本工程按 D1 改为登录保持 1280×720）。

⇒ 本工程必须**用显式状态（登录/游戏）区分，而不是比较宽高数值**；进图 `SET_FIELD enter_game` 写游戏档位，退出登录/选角恢复登录尺寸。

## 4. UI 重锚（借鉴原则：不缩放，只按 Origin 重绑）

- `wnd.h:18-29` 九宫格 `Origin_LT/CT/RT/LC/CC/RC/LB/CB/RB`；`rs.cpp:49-50`「RT/RB abs X = W-800」；`rs.cpp:2801-2833 rs_GetOrg_hook` 按返回地址分派 CT(BossHP/道场)、RT(Combo counter)、CC(screen effect)、StatusBar、ScreenMsg、QuickSlot；`:2835-2848 RsMoveWndToAbsPos()`；`:2899-2905` 边缘吸附；`:2915-2917` **不调原生 `OnMoveWnd`**（其硬编码 800×600 吸附会让窗口在 CC/HD 原点下跑飞）。
- 底栏：`rs.cpp:985`「LB StatusBar org abs=(0,H-600)」、`:1045-1049 RsStatusBarMenuOffsetX() = W-800+407`；本工程仓库证据 `docs/features/ui-v83-window-restore/README.md:67-68` 明确「817×71 不再拉伸到 VWIDTH，废弃 right_adj 居中，原生就是左贴边」，`evidence/w1w3-hud/coords.md:3` 底栏 `(0,649)`@1280×720、`:67` 快捷格右贴边。
- 大地图：`rs_refreshWorldMapCenter()` L949-957 `((W-666)/2,(H-524)/2)`；注释警告 **必须喂 screen-LT 中心**，CC+(67,38) 会双偏移且 `GetAbsLeft()` 后卡死。
- 任务 / NPC / 弹窗：`CUtilDlgEx::CreateUtilDlgEx_hook` L970-979 `nLeft = W/2 - m_wndWidth/2`、`nTop = H/2 - m_wndHeight/2`，`CreateWnd` 保持 LT 原点；本工程 `login-modal-viewport-center/README.md:29` 同式，`whisper-dlg-prep.md:140-154` 理论 `(507,299)` 实测 `(505,300)`。
- 本工程 `ui-layout-v83-unify/README.md:10-11`：**不是单一全局 transform**（`set_world_transform` 已回退、禁止重开）⇒ 保留绝对定位，只补重锚。

### ⚠ 与用户要求的明确偏离

用户要求「分辨率过宽时，底部信息栏可以保持底部居中显示」。原生证据只支持「贴底 + 左贴边 x=0」与「快捷栏右贴边」，**不支持水平居中**。本版按用户 D 要求实现「过宽时底栏整组水平居中」，并在代码注释与本文档标注：这是唯一明确偏离原生定位的地方；底栏/快捷栏/聊天输入行共用同一水平偏移，彼此相对偏移保持不变，不做缩放。

## 5. 必须规避的坑（来自插件实测）

1. **BPP**：`多分辨率.md:116` — `0x009F7A9B` 的 `mov eax,[dword_BF1AC8]` 是 BPP(实测 16)，旧注释误标「force windowed」；置 0 会让 `IWzGr2D::Initialize` 失败 `0x8876086C`。窗口模式应在初始化成功后施加。
2. **输入命中坐标**：`多分辨率.md:117-127` + `rs.cpp:1604-1605` — `CInputSystem` 的 `SetCursorVectorPos`（`0x0059A15D+2` / `0x0059A169+2` 写 `-(H/2)` / `-(W/2)`）、`SetCursorPos`（`0x0059A898+1` / `0x0059A8B1+1`）、`UpdateMouse`（`0x0059AC09+1` / `0x0059AC22+1`）必须随分辨率同步，否则 HD 下点不到 NPC / 丢道具死区。本工程对应 `IO/Window.cpp:220-227,281-301,871-889`（触屏换算还用 Window 私有宽高，必须改）。
3. **boot 立即数**：`多分辨率.md:129` — `UpdateResolution()` 在 boot 按 1280×720 写死一批屏幕尺寸立即数，切到更高档必须重放，否则字段世界鼠标仍按 1280 中心换算。本工程 `GraphicsGL::reinit()` 已在 `initwindow()` 中统一刷新投影 uniform，须验证 `glViewport` / `SCREEN` / `Camera` / 虚拟光标全链同步。
4. **模态窗 + StatusBar 重建**：见 §2，必须延迟应用。
5. **同尺寸误判登录态**：见 §3。

## 6. 基址公式表摘要（`基址.txt`）

385 行全部是 `地址+偏移 = 含 screenWidth/screenHeight 的表达式`，代表值：

- 屏幕尺寸 / 半宽：`0x009F7B1D+1=H`、`0x009F7B23+1=W`、`0x009E2E85+1=-(H/2)`、`0x009E2E8B+1=-(W/2)`、`0x00BE2738=W/2`、`0x00BE2DF0/2DF4=W/2,H/2`。
- 贴底：`0x008D3F73+1=H-93`、`0x008D1FF4+1=H-22`、`0x008CFD4B+1=H-22`、`0x008DFA6F+1=H-81`。
- 居中：`0x007B2C97+2=((H-600)/2)+277`、`0x007B2CB5+3=((W-800)/2)+397`、`0x00992BA7+1=W/2`、`0x00992BAC+1=H/2`、`0x0060F7AC+1=(W/2)-201`。
- 缓冲：`0x00744E2A+1=3*W*H`、`0x00744DA6+1=4*W*H`。

本工程是源码级客户端，**不 patch 二进制**；此表只用于确认「原生确实是 W/H 相对锚点而非固定像素」，以及给「底栏贴底、弹窗居中」的语义背书。
