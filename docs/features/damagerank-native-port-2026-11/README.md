# #4 个人伤害统计原生移植 · 实现记录（2026-11-18）

## 决策
- 入口：**左侧竖栏图标**（side_icons.ini Action=OpenUI arg=DamageRank），不占快捷键（F12 已被 UIReset 占用）。
- 授权：用户明确"授权一次性实现到位"。

## 改动清单（一次成型）
| # | 文件 | 动作 |
|---|---|---|
| 1 | `Character/DamageRank.h` | 新建：进程级单例，`PlayerEntry`/`SkillEntry`，map 索引 + 降序 snapshot，带 mutex |
| 2 | `Configuration.h` (~:960) | 加 `PosDAMAGERANK : PointEntry("ui.position.damagerank","(200,100)")` |
| 3 | `IO/UIElement.h` (:243) | `enum Type` 加 `DAMAGERANK`（在 SIDEICONBAR 后、NUM_TYPES 前）→ **须整工程 Rebuild** |
| 4 | `IO/UITypes/UIDamageRank.h/.cpp` | 新建：继承 `UIDragElement<PosDAMAGERANK>`，双页（玩家/技能）、滚轮滚动、拖拽、关闭钮、可选背板，零专用 WZ 依赖 |
| 5 | `IO/UITypes/UISideIconBar.cpp` | include UIDamageRank.h + `open_ui` 加 `damagerank/dpt/damage` 分支 |
| 6 | `Util/SideIconConfig.cpp` | defaults 加 `{ "damagerank", ..., "Basic.img/BtClaim", OpenUI, "DamageRank" }` slot |
| 7 | `Net/Handlers/WaveAShellHandlers.cpp` | `DptTrackerHandler::handle` 改为喂 DamageRank（mode 0/1/2 全字段），窗口打开时 reload；去掉 UIAdmin 调试提示 |
| 8 | `MapleStory.vcxproj` | 登记 UIDamageRank.cpp/.h、DamageRank.h |

## 协议对齐（未改服务端、未重排 opcode）
复用 s9 既有 `DPT_TRACKER(0x3714)` / `CUSTOM_PACKET(0x3713)`：
- mode0 reset：`byte 0`
- mode1 player：`byte 1, int charId, int jobId, string name, long totalDamage`
- mode2 skill：`byte 2, int skillId, string name, long delta/total/max/min, int count`
守卫逐项按 `available()>=N`，空包不越界。

## 关键取舍
- **无专用 WZ 资源也能工作**：背板探测 `UiAssetResolve::window("DamageRank")/backgrnd`，缺失则纯文字；关闭钮用通用 `close_btn()`；双页签用 AreaButton 热区。符合"不碰 live、不新增资源"约束。
- **不抢 F12**：入口统一走侧边栏，避免与 `UIMiscShells.cpp:6314` UIReset 冲突。
- **后台收包不弹窗**：handler 仅 `get_element<UIDamageRank>()` 存在时 reload，入口唯一。

## 验证
- 构建：`tools/_pua_build_damagerank.bat`（整工程 Rebuild → `build-damagerank/`，日志 `build_damagerank.log`）。
- 第 1 轮构建失败（BUILD_EXITCODE=1）：唯一错误 `UIDamageRank.cpp(56): Color::Name::DARKGRAY 未声明`——我臆造了美式拼写；源码 `Graphics/Color.h:64` 真值为 `DARKGREY`（英式）。已改为 `DARKGREY`。
- 第 2 轮构建被超时打断（BUILD_EXITCODE=-1）：日志 0 error 但停在 UIStateLogin.cpp、无链接产物——bash 调用到期把 MSBuild 进程树一起杀了，不是编译失败。
- 第 3 轮改分离式进程（Start-Process cmd.exe /c bat，PID 50912）：日志 3386 行后 cmd 被回收、无产物但无 error。
- 第 4 轮 OPS 后台 job 直跑 bat：`echo BUILD_EXITCODE=-1` 是父 cmd 被 harness 回收时写的，**子 MSBuild 仍存活继续编译**（实测 1 个进程、日志仅 32 行刚到 Char.cpp）。
- **构建方法论教训（沉淀）**：整工程 Rebuild > 单次 bash 上限；bat 的 `echo EXITCODE` 不可信（父 cmd 被回收即写 -1）；判断构建完成的唯一可靠信号 = **日志出现链接行 `MapleStory.vcxproj -> ...MapleStory.exe` + MSBuild 进程数归零 + 产物 exe 存在**，三者齐备才算成功，不能用"0 error"或 bat 退出码替代。
- 第 5 轮 `C1090 PDB API 调用失败`：强杀 MSBuild 遗留坏 `vc142.pdb`，cl 进程死循环重试（CPU 涨、日志停）。RCA=强杀→坏PDB→C1090→再强杀的因果闭环；解法=停 mspdbsrv+cl+MSBuild 全树、删 PDB、构建加 `/p:DebugType=none` 绕开 PDB 写入。
- **最终构建成功（第 6 轮）**：`tools/_pua_build_damagerank_final.bat`（增量 + DebugType=none）→ `BUILD_EXITCODE=0`。三证齐备：① 链接行 `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe` ② error=0 ③ UIDamageRank.obj 19:14 更新。live exe 19:17 更新 SHA256=`81261329A64CF7510F167FF8C756CE429D9CF3FB05513A59A1A5EA0920F147B9`，无进程占用。
- 蓝军自检：`UIStateGame::pre_add:1170` 会 `elementorder.push_back(type)`，emplace<UIDamageRank> 后自动进绘制链，窗口可显示（P1 排除）；`read_long()` 返回 int64_t，static_cast<uint64_t> 正确；所有 Text::Font/Color::Name 枚举经源码核对合法。
- 真机发包/观感：**未验收**，待用户按侧边栏入口复核，如实声明。

## 入口启用（用户侧，一项必做）
live `E:\MXD\BeiDou-Client-xy\side_icons.ini` 已存在旧版 4 slot（quest/event/daily/sysopt），
`ensure_default_file()` 仅在文件不存在时写默认值 → **damagerank 不会自动出现在侧边栏**。
需在 side_icons.ini 末尾追加（slot.4，图标可改为你喜欢的 mainBar/menu 图标）：
```
slot.4.id=damagerank
slot.4.enabled=1
slot.4.icon=StatusBar3.img/mainBar/menu/button:Menu
slot.4.action=OpenUI
slot.4.arg=DamageRank
```
追加后重启客户端，左侧竖栏出现第 5 个图标即入口。此步属改 live 配置，AI 未擅自修改。

## 待办（不属本次范围）
- 专用背板/图标美术：如需精美 UI，按门禁 16 走 Photoshop MCP 出图 + orange-wz 入库（另案，本版不碰 live）。
- i18n：当前 UI 文案为英文硬编码；项目若接入文案表需迁到 Locale（待用户确认是否本版做）。
