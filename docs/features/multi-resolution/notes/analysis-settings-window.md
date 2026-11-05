# 游戏设置 / 系统设置窗口逐项核实结论

取证口径：运行时部署数据 `E:/MXD/BeiDou-Client-xy/Data/UI/UIWindow.img`（权威 dump `REA反编译/wz_dump/UIWindow.img_runtime.json`）、`Basic.img_runtime.json`；原生截帧逐格模板匹配 `docs/features/ui-v83-window-restore/evidence/w7-option-settings/_settings_map2.txt`；实现结论 `b7-4-settings-split.md`、`evidence/w8-sysopt-behavior/README.md`（R28-B4）。判据：**控件相对底板左上角的落位**是否与烘焙位图逐像素吻合，以及功能是否有真实消费方。

## 1. 结论总表

| 核实项 | 结论 | 证据 |
| --- | --- | --- |
| 真窗入口（目录气球 4 钮、`O` 键） | ✅ 正确 | `IO/UITypes/UIMiscShells.cpp:4483-4491,4518-4529,2906-2909` |
| ESC 关闭设置窗 | ✅ 正确 | `IO/UI.cpp:336-354`（含 `GAMEOPT`/`SYSOPT`），漏加会落到 MAINMENU |
| 底栏「设置→选项」入口 | ❌ 不正确（既有缺陷） | `IO/UITypes/UIStatusBar.cpp:970-976` → `emplace<UIOptionMenu>()`；`UIOptionMenu.cpp:45-66` 缺 `StatusBar3.img/OptionMenu` 时构造早退，只画一张 fallback 底板、不建按钮 |
| `UIGameOpt` 底板 | ✅ 正确 | `UIWindow.img/GameOpt/backgrnd` 158×284 origin(0,0)，`UIOptionMenu.cpp:508,518-520`，`dimension = backgrnd.get_dimensions()` |
| `UIGameOpt` 12 行勾选 | ✅ 正确 | `CHECK_X=130 / CHECK_TOP=34 / CHECK_PITCH=18`，命中 `AreaButton(130,34+18i,12×12)`，纹理 `Basic.img/CheckBox[0/1]`；`_settings_map2.txt:8-21` 12 处 SAD=0.00（未勾 1 处 @y142 亦 SAD=0） |
| `UIGameOpt` 确定/取消 | ✅ 正确 | `OK_X=34 / CANCEL_X=84 / BTN_Y=262`，`Basic.img/BtOK2`/`BtCancel2` 47×19；`_settings_map2.txt:2-5` SAD=0.60（原生→画布 dx=1） |
| `UISysOpt` 底板 | ✅ 正确 | `UIWindow.img/SysOpt/backgrnd` 299×365 origin(0,0)，`UIOptionMenu.cpp:691,701-703`；原生 298×365，dx=0 dy=0 |
| `UISysOpt` 6 条滑条 | ✅ 正确 | `TRACK_X0=99 / TRACK_X1=245`，`SLIDER_ROW={40,70,100,160,190,220}`；`Basic.img/Slider/{left,mid,right,thumbNormal}`，`thumbNormal 22×14 origin(0,0)`，轨道件 origin(0,-2) 故绘制 y=row_y+1；`_settings_map2.txt:40-45` 6 处 SAD=0.00 |
| `UISysOpt` 5 个勾选 | ✅ 正确 | `{{241,71},{241,101},{64,251},{64,304},{64,315}}`，12×12；`_settings_map2.txt:33-39` SAD=0.00 |
| `UISysOpt` 2 个下拉 | ✅ 正确（收起态） | `COMBO_X=157 / COMBO_W=60 / COMBO_TOTAL_W=83 / COMBO_H=17`，`COMBO_Y_PATH=127 / COMBO_Y_MOB=278`；`Basic.img/ComboBox/normal/2` 18×17 箭头落在 (222,127)/(222,278)，`_settings_map2.txt:47-49` SAD=0.00 |
| `UISysOpt` 确定/取消 | ✅ 正确 | `OK_X=194 / CANCEL_X=244 / BTN_Y=342`，`_settings_map2.txt:27-30` SAD=0.07 |
| 下拉展开态 | ⚠ 已实现但自绘 | `draw_combo_list`(cpp:874-900) / `set_combo_rows`(cpp:902-922) / `sysopt_open`(cpp:683)；原生无展开态截帧，配色沿用 `MapleComboBox`，属显式自定 |
| 窗体整体落位 | ❌ 不正确 | `Configuration.h:771-774 PosGAMEOPT(170,24)`、`:778-781 PosSYSOPT(330,24)` 为绝对像素且注释自认「非 oracle 读数」；`IO/UIDragElement.h:79` 从 `Setting<PosXXX>::load()` 读取；三窗均**无 `update_screen` override**（`IO/UIElement.h:200` 是已有钩子） |
| 9 项系统设置功能接线 | ✅ 正确 | 见下表「功能消费方」 |
| 游戏设置 12 位同意掩码 | ✅ 已接线（2026-11-06） | `ui.gameopt.mask` + `UIGameOpt::accepts()`；Party/Trade/Messenger/Family/Whisper/Buddy·Alliance 聊天已拦；`GAME_INVITE`/`FAMILY_CHAT`/`UNION_APPLY` 仍预留，见 [gameopt-invite-mask](../../gameopt-invite-mask/) |
| 分辨率选项 | ❌ 缺失 | 真窗无；空壳 `UIOptionMenu.cpp:92-146` 有 6 档 `SELECT_RES`，OK 分支 cpp:194-227 只写 `Setting<Width/Height>` + `Constants`、**漏 `Configuration::get().save()`**；SoftKey `"WxH"` cpp:327-339 允许任意 w≥640/h≥480，同样不落盘；二者均因空壳早退而不可达 |
| 旧 5 个 tab / UIRESET | ❌ 不可达 | `UIOptionMenu.cpp:73-79,239-248`，同样被空壳早退吞掉 |

## 2. 功能消费方（9 项系统设置，逐项真生效）

| 原生行 | 存盘键 | 消费方（证据） |
| --- | --- | --- |
| 画质调整 | `ui.sysopt.quality` | `Graphics/EffectLayer.cpp:39`（特效密度上限），`UISysOpt::commit` 过半写 `window.vsync` 并 `Window::apply_vsync()` |
| 背景音乐 | `audio.bgm.volume` | `Music::set_bgmvolume`（`IO/UI.cpp:127-131`、`UIOptionMenu.cpp:1049`） |
| 效果音响 | `audio.sfx.volume` | `Sound::set_sfxvolume`（同上） |
| 截图保存位置 | `ui.sysopt.screenshot_path` | `IO/Window.cpp:322 screenshot_directory()`（0=exe 目录 / 1=桌面，源自 `ijl15.dll@0x2177c`） |
| 鼠标速度调整 | `ui.sysopt.mouse_speed` | `IO/Window.cpp:268-270 mouse_speed_index()`（只缩放相对位移，不回写系统光标） |
| HP 警报 | `ui.sysopt.hp_warn` | `UIStatusBar.cpp:2379-2380`（classic `gauge/hpFlash`，0=不报警） |
| MP 警报 | `ui.sysopt.mp_warn` | 同上（`mpFlash`） |
| 画面效果 | `ui.sysopt.skill_effects` | `Character/Char.cpp:184`、`Gameplay/MapleMap/Mob.cpp:702` |
| 画面模式 | `ui.sysopt.window_mode` / `ui.sysopt.alt_enter` | `Window.cpp:502`（启动回灌）、`:1079`（ALT 镜像）、`IO/UI.cpp:193`、`UISysOpt::commit:1040-1046` |
| 怪物情报 | `ui.sysopt.mob_info` | `Mob.cpp:545-547 MOB_INFO_TIER[4]`（源自 `ijl15.dll@0x293ec`） |

> ⚠ SysOpt 部分条目注释可能仍写「仅落盘」——以本表「功能消费方」为准。`GameOptMask` 邀请/聊天拦截已于 2026-11-06 接线。

## 3. 落盘链路

- `UIGameOpt::commit()` `UIOptionMenu.cpp:610-622`：`Setting<GameOptMask>.save(mask)` + `Configuration::get().save()`。
- `UISysOpt::commit()` `UIOptionMenu.cpp:1012-1075`：11 项 save + `Window::set_fullscreen` + 音量 + VSync + `Configuration::get().save()`（L1069）。
- `Configuration::save()` `Configuration.cpp:605-609` → `Settings`（cwd 相对路径，UTF-8 无 BOM + LF），`SAVE_ORDER` 在 cpp:84-178；`xy-config.yaml` 优先级更高（cpp:359-362）。
- **新需求必须补的缺口**：旧 `SELECT_RES` OK 分支没有 `Configuration::get().save()`，新下拉的 OK 必须调用；否则重启丢档。

## 4. 与「原生定位」的关系

原生 v83 的窗口位置由 `CWndMan::GetOrgWindow(UIOrigin)` 九宫格原点换算（见 `docs/features/login-flow-ui-ida-align/notes/ui-positioning-scheme.md`），分辨率变化后 `update_screen(new_w,new_h)` 重算；本工程用 `PosXXX` 绝对像素。两扇设置窗当前**没有**该重锚；但用户 D3 决定本版不动，故列为显式预留。9 档中最小 800×600 也能容纳两窗（最大 299×365 + (330,24) = (629,389)），不会出现「跑出视口不可见」，只是不居中。

## 5. 本版要改的与不动的

- 改：`UISysOpt` 底板换成用户指定 299×396 纯底板（控件区剥离）、新增「窗口大小」行 9 档下拉、OK 即时生效并落盘、显示真实当前档、延迟应用防竞态、文案走 i18n。
- 不动（按 D3）：底栏空壳入口、`PosGAMEOPT/PosSYSOPT` 居中重锚、`GameOptMask` 邀请接线、下拉展开态外观。验证一律走目录气球「系统设置」入口。
