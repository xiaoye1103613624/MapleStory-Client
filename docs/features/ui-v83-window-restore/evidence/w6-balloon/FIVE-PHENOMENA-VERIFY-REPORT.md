# 五现象强制重编 + 真机验证报告（2026-11-16/17，最终版）

## 0. 背景

用户（m00002）问游戏内「目录」「界面」两功能的五个"为什么"：互斥性、点两次才显示、上下键切选中、ESC 开目录后上下键、回车生效。源码 RCA 结论：五个历史 bug 均已修复，用户看到的是修复前表现。用户（m00070）"可以"授权进入强制重编 + 真机验证闭环。

**本轮关键突破**：真机验证不仅坐实了五个现象，还挖出并修复了一个此前 RCA 漏掉的真因——高亮块绘制层序错误（详见 §3.2）。这是"冰山下面的冰山"：上一轮只修了键码兼容，却没发现高亮被按钮贴图盖住，导致用户"上下键不切选中"的问题实际一直没解决。

## 1. 强制重编入链 —— ✅ 完整闭环（两轮）

MSBuild 增量编译假通过前科在案（编辑不改 mtime → 旧 obj 出包）。两轮均采用独立 IntDir/OutDir 全量编译，删净旧 obj/tlog 强制重编，绕开并发 session 踩踏：

| 验收项 | 第一轮（键码修复基线） | 第二轮（+draw 层序修复，最终） | 结果 |
| --- | --- | --- | --- |
| 构建返回码 | BUILD_RC=0 / error C=0 / LNK=0 | BUILD_RC=0 / error C=0 / LNK=0 | ✅ |
| 产物 SHA | `0fde5c68900c19cd55f36506` | `e9bae1980e1bb1202c954837b3d5612ae69c13bb7f84` | ✅ 变化 |
| obj mtime > cpp | UIMiscShells.obj 1794842046 | 1794870272 > cpp 1794846258 | ✅ |
| 修复特征串 | exe 命中 `gamemenu.tip_hints` | 命中 1 | ✅ |

构建命令（VS2019 BuildTools，v142）：
```
MSBuild.exe MapleStory.sln /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v142
  /p:OutDir=...\\_balloon_verify\\out\\ /p:IntDir=...\\_balloon_verify\\int\\ /m /nologo /v:minimal
```
脚本：`_balloon_verify/rebuild_now.sh`。

## 2. 部署 —— ✅ 未触碰 live 基线/活实例

两轮 exe 均部署为独立名 `E:\MXD\BeiDou-Client-xy\MapleStory_balloon.exe`（最终 SHA `e9bae198`，与产物一致）。live 基线 `MapleStory.exe` SHA 未变，活实例未动。运行时依赖 SDL3.dll 在位。

## 3. 真机验证 —— ✅ 五现象全部通过（最终）

验证脚本：`evidence/w6-balloon/five-phenomena-alt.py`（第二账号 xiaoye auto 登录 → 等 `window.740` 进图 → 12s HUD 稳定 → 三阶段点击/按键 → 逐步抓帧 24 张）。
判据工具：`b6-menu-anchor-check.py`（蓝钮段数）、`verify_highlight.py`（GRAYOLIVE 高亮块 selected 判定）、像素差分。
产物：`E:/project/_ms_probe/balloon_verify/_fp_*.png`（修复后）+ `before-fix/`（修复前 24 张对照）+ `_fp.txt`（完整轨迹）。

### 3.1 ✅ 现象2「点击开合 / 再点一次收起」、现象1「互斥」

蓝钮段数判据（`_fp.txt` + 逐帧段数）：

| 帧 | 段数 | 事实 |
| --- | --- | --- |
| baseline | 0 | 无气球 |
| 目录 try0/1/2/3 | 0/4/0/4 | 点一次开、再点关、再点开（toggle 正常）|
| 界面 try0/1/2/3 | 0/8/0/8 | 同上 |
| 09 界面开着时点目录 | 4（界面 8 段消失）| **互斥生效**：开目录先收界面 |
| 07 ESC / 14 final | 4→0 | ESC 关闭生效 |

对照修复前旧 exe 日志 `/e/project/_ms_probe/alt/_b6alt.txt` 的 `after click#3 balloon_open=True`（收不起 bug），现象2 修复前后对照成立。

### 3.2 ✅✅ 现象3「上下键切选中」、现象4「ESC 开目录后上下键」—— 挖出并修复真因

**第一轮复验发现真 bug**：上下键帧的四颗蓝钮 RGB 逐字节相同（R=82/79/79/78，B-R≈140），高亮块始终 selected=0，无论 DOWN/UP。而同批回车帧蓝钮区大变、全屏 74655px 变化——证明 `send_key` 可达、selected 逻辑在变、回车激活成功。**矛盾锁定为绘制层序，而非键码。**

**RCA 真因**：`UIMiscShells.cpp` 的 `UIShortCut::draw`(:4057) 与 `UIGameMenu::draw`(:6523) 中，高亮 `ColorBox hl.draw()` 画在 `UIElement::draw(inter)` **之前**；而 `UIElement::draw` 内部 `draw_buttons`(:219) 画的是不透明蓝钮贴图——高亮先画、按钮后画，被整块盖住。上一轮 RCA 只修了三套键码兼容（265/264/38/40/KeyAction），没修这层，所以键盘改了 `selected` 却看不见。

**修复**：两个 draw 中把 `hl.draw()` 移到 `UIElement::draw()` 之后（带注释说明层序）。第二轮全量重编部署后复验，`verify_highlight.py` 前后对比铁证（selected=argmax(R-0.5×(B-R))）：

| 帧 | 修复前 selected | 修复后 selected | 修复后 R 分布 |
| --- | --- | --- | --- |
| 01 目录开 | 0 | 0 ✅ | [127,79,79,78] |
| 02 DOWN | 0 ❌ | 1 ✅ | [82,125,79,78] |
| 03 DOWN | 0 ❌ | 2 ✅ | [82,79,125,78] |
| 04 UP | 0 ❌ | 1 ✅ | [82,125,79,78] |
| 11a ESC 开 | 0 | 0 ✅ | [127,79,79,78] |
| 12 ESC 后 DOWN | 0 ❌ | 1 ✅ | [82,125,79,78] |
| 13 ESC 后 DOWN2 | 0 ❌ | 2 ✅ | [82,79,125,78] |

高亮块随 UP/DOWN 精确 26px 步进，B-R 从 140 收到 62（GRAYOLIVE alpha0.5 叠加特征）。现象3、现象4 真机铁证成立。

### 3.3 ✅ 现象5「回车激活」

`04_menu_up1`（回车前，菜单开 4 段）→ `05_menu_enter`（回车后）：菜单区内变化 10839/12600px，菜单区外变化 62928px（6.4%，bbox x157..1282 y11..750），对照 DOWN 帧区外仅 3.1%——回车弹出了新窗口/执行了 `button_pressed(MENU_IDS[selected])`。现象5 真机通过。

## 4. 环境复原

- 清理本次启动的全部 `MapleStory_balloon.exe` 残留实例（残留数=0）。
- `E:\MXD\ms-alt-b53\xy-config.yaml` 恢复 `auto:false` / `xiaoye`（UTF-8 BOM + CRLF 完好，备份 `xy-config.yaml.bak.before-balloon`）——验证期间需临时改 `auto:true` 自动登录，收尾已还原。
- live 基线、活实例、服务端配置均未改动。

## 5. 结论

- 强制重编 + 入链 + 部署：**完整闭环**，两轮全量重编，最终 exe SHA `e9bae198` 含 draw 层序 + 键码双修复。
- 真机验证：**五现象全部通过铁证**。现象3/4 的验证意外挖出并修复了真因（高亮层序），这是本轮最大价值——从"键码已修但用户仍见不到高亮"的外部表象，追到了"高亮被按钮贴图盖住"的真实病根。
- 复验方法：`five-phenomena-alt.py` + `verify_highlight.py`，24 帧完整证据链，修复前旧帧存 `before-fix/` 可直接对比。

## 6. 追加：用户新反馈驱动的「点一次打不开」修复（2026-11-17 上午）

用户（m00805）追加反馈两点：①「列表高亮发虚」②「目录/界面有时点一次打不开，第二次才可以」。源码 RCA：

### 6.1 「点一次打不开」真因 + 修复 —— ✅ 入链闭环，真机受自动化时序限制

**真因**：`IO/UIElement.cpp:295` `UIElement::send_cursor()` 的 NORMAL 分支（:306）收到 click 时只 `set_state(MOUSEOVER)` + `ret=CANCLICK`，**不调 `button_pressed`**；只有 MOUSEOVER 分支（:313）的 `:315 if(clicked)` 才触发。即第一次点击仅建立 hover，第二次才开。这解释上一轮「try0=CLOSED/try1=OPEN」被误读成 toggle 正常——实为首次点击被吞的 bug。

**修复**（`IO/UIElement.cpp` NORMAL 分支）：在 `set_state(MOUSEOVER)` 后补 `if(clicked){ Sound(BUTTONCLICK).play(); btit.second->set_state(button_pressed(btit.first)); ret=IDLE; }`；MOUSEOVER 分支原样不动，正常「先 hover 再点」零影响。蓝军自攻击：一次物理点击 = SDL 一次 clicked=true + 一次 false，只在 true 触发，无多触发风险。

**入链铁证**：

| 验收项 | 数据 | 结果 |
| --- | --- | --- |
| 构建 | BUILD_RC=0 / error C=0 / LNK=0（删净独立 IntDir obj 全量重编）| ✅ |
| 部署 exe SHA | `d0d964e5797c16f7e90f5fc5`（= 构建产物）| ✅ |
| 修复前对照 exe SHA | `e9bae198...`（备份 `MapleStory_balloon_before-clickfix.exe`）| ✅ 变化 |
| UIElement.obj mtime | 08:53:47 > cpp 08:51:18 | ✅ 入链 |

**真机验证 — 逻辑闭合但自动化无法给出 try0=OPEN 铁证（如实披露）**：源码派发链逐行确认闭合（UI.cpp:166 `send_cursor(true)→CLICKING` → UIStateGame.cpp:871/:936-965 遍历 active+in_range → UIElement.cpp NORMAL+clicked 新分支 → button_pressed）。真机三条注入路径全部穷尽且被环境约束挡住：① SendInput（`_drive.send_click`）— 本会话 GUI 常驻前台，SetForegroundWindow 被前台锁拒；② AttachThreadInput+ALT tap 抢前台 — NEW3 抢到前台仍 segs=0；③ PostMessage 鼠标消息 — SDL3 走 Raw Input 不认普通 window message。回到唯一验证过能复现 bug 的原版 `five-phenomena-alt.py`（含 hover trick）跑新 exe，`try0=CLOSED/try1=OPEN` 与旧 exe 逐帧一致——根因是脚本 `click()`（:155-166）的 hover trick（两次 SetCursorPos + sleep）本身制造了一帧前置 move，掩盖了游戏逻辑修复；真人操作（cursor 实时更新、MOUSEOVER 正常建立）走的是 MOUSEOVER 分支，本就不受此 bug 影响，而瞬移/快速点击场景才是本次修复的目标。**结论**：修复已在部署 exe 内且逻辑闭合；真机自动化因注入时序与前台锁无法产出「try0 一次开」的对照帧，需真人用独立名 exe 自验快速点击场景。

### 6.2 「高亮发虚」——✅ 已按 v83 原生 mouseOver 贴图重做（2026-11-17 下午，MCP 恢复后）

**MCP 取证（orange-wz，端到端可用后）**：`UIWindow.img/ShortCut/BtItem`、`GameMenu/BtChannel` 每个按钮只有 `normal/pressed/disabled/mouseOver` 四态（81×25 ARGB8888），**无独立 selected/focus 素材**。三态像素实测：normal RGB(64,171,224) → mouseOver(28,172,240)（R 降、B 升＝亮蓝）→ pressed(46,152,200)。结论：v83 原生「选中焦点态」就是按钮自身的 **mouseOver 亮蓝贴图**；旧版 `ColorBox(GRAYOLIVE,0.5f)` 灰绿半透明盖深蓝钮混出灰蓝（真机 B-R 140→62、R 79→125），发虚且非原生。证据落盘 `evidence/w6-balloon/native-highlight-mcp-evidence.md`。

**修复**：`UIShortCut::draw` / `UIGameMenu::draw` 两处弃用 ColorBox，改为「临时把 `MENU_IDS[selected]` 对应按钮切到 MOUSEOVER 态单独重画、画完 `set_state(saved)` 立即恢复」——复用原生亮蓝贴图、零公共 API 改动、不污染鼠标 hover 状态机（`MapleButton::send_cursor` 空实现，state 由 UIElement 管，故必须画完恢复；画在 `UIElement::draw` 之后盖住该颗 NORMAL 钮）。

**入链**：全量重编 BUILD_RC=0 / error C=0 / LNK=0；新产物 SHA `095b8e97…`（旧 `d0d964e5`，size 5227008→5235712）；`UIMiscShells.obj` mtime 14:39:41 > cpp 14:35:42；已备份旧部署版 `MapleStory_balloon_before-highlight-native.exe`（d0d964e5）后原子部署新 exe到 `MapleStory_balloon.exe`（095b8e97）。

**真机验证（原生判据 7/7 全 OK）**：`five-phenomena-alt.py` 重跑 RC=0、24 帧，判据 `_ms_probe/balloon_verify/verify_highlight_native.py`（B-R 最大＝选中项，mouseOver 方向）：

| 帧 | 期望 selected | 实测 | R 分布 | B-R 分布 |
| --- | --- | --- | --- | --- |
| 01_menu_try1 | 0 | 0 ✅ | [72,79,79,78] | [160,143,143,143] |
| 02_menu_down1 | 1 | 1 ✅ | [82,64,79,78] | [140,170,143,143] |
| 03_menu_down2 | 2 | 2 ✅ | [82,79,65,78] | [140,143,168,143] |
| 04_menu_up1 | 1 | 1 ✅ | [82,64,79,78] | [140,170,143,143] |
| 11a_esc_open | 0 | 0 ✅ | [72,79,79,78] | [160,143,143,143] |
| 12_esc_menu_down | 1 | 1 ✅ | [82,64,79,78] | [140,170,143,143] |
| 13_esc_menu_down2 | 2 | 2 ✅ | [82,79,65,78] | [140,143,168,143] |

选中项 B-R 从 140 拉大到 170/168（mouseOver 亮蓝），R 从 82 降到 64/65，与 MCP 实测原生贴图方向完全一致；上下键 0→1→2→1、ESC 后同样 0→1→2 全部随动。**发虚问题解决，且已是原生素材观感。**

### 6.3 收尾状态

配置 `xy-config.yaml` 已还原 `auto:false`/xiaoye（BOM+CRLF 完好）；残留僵尸实例（pid47560/54544/12524 各 32K）为历史遗留，非本轮启动；`MapleStory_pua24.exe`（pid29784）属其他 session，未动；live 基线 `MapleStory.exe` SHA 未变。独立名 exe `MapleStory_balloon.exe` 是否合入正式包由用户拍板。
