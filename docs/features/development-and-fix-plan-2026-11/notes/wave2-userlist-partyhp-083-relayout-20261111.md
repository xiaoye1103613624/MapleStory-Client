# round68 · 好友窗 / 组队窗按 Angel 083 CUIUserList 真资源重修（D8 组队血条）

## 0. 性质与授权
- User said (m09859): "若碰到ui布局、动画一定要确认是否修复了，如果不是按照Angel 布局修复过的，可以重新修复；之前有很多参照BeiDou或者自行修复的ui、动画都有些问题；" —— 本轮验收标准：非按 Angel 083 布局/动画修复的 UI 逐面重修。
- m04292 授权：改游戏源码 / 自测 / 测试完成提交 / 遇 MapleStory.exe 占用直关 / 不能自测记 backlog。
- 范围：`statusbar-hud/D8 组队血条` + `好友窗几何`。取证只读 → 只实施零风险真资源替换 → harness → 真重编 → 三文档 → 原子提交。校准≠改回原生（不动 s9 协议）。

## 1. 缺陷逐字与翻案
外部程序 `7105da94`（好友窗）与 `d6bdce97`（D8 血条）两处「自行修复」经 IDA + `.nx` 翻案，均为「找错节点后自造兜底」同族病（第三/第四例，前两例见 round66 `alert/backgrnd`、round67 `KEY_ANI`）：

| 面 | 外部程序（自行修复） | Angel 083 权威 | 证据 |
| --- | --- | --- | --- |
| 好友窗根 | `UIWindow.img/FriendRecommendations/FriendsList`（全库 0 命中）→ 无背景无钮 | `CUIUserList::OnCreate @0x919b73` 用 `UIWindow.img/UserList/backgrnd`(312×389) + `Friend/BtAddFriend`(5,81) | `.nx` 顶层 113 节点含 `UserList`(非 `Recommendations`) |
| 好友窗几何 | 自定 182×310 / 关闭(165,4) / 加好友(146,4) | 共用窗 312×389；083 按钮坐标最大 x=254 y=365，背景可容纳 | `tools/_uw_visual/UserList_backgrnd.png` |
| 组队窗根 | `Party.img`（全库 0 命中）→ 四钮空壳 | `UIWindow.img/UserList/backgrnd` + `Party/Bt*` 真按钮 | `.nx` `UserList/Party` 9 个 `Bt*` |
| D8 血条 | `UIParty::draw` 里 `ColorBox` 手绘暗底+比例填充+30% 转红 | 独立悬浮窗 `CUIPartyHP @0x91f001`，真位图 `GaugeBar/{bar,gauge,graduation}`，6 人、宽≥150、`SP_4433_SHOWPARTYHP` 开关 | `.nx` `UserList/Party/PartyHP/GaugeBar` |

`.nx` 精确名计数：`FriendRecommendations` / `Recommendations` / `FriendsList` / `BuddyList.img` / `Party.img` / `BtAddBuddy` 全 **0 命中** → 运行时窗口直接空白。

## 2. IDA 083 权威取证（`tools/_ida_pb/*.dec.txt`）
- `CUIPartyHP::Create @0x91f934`：`CWnd::CreateWnd(z:10, bScreenCoord:1)` 独立悬浮窗；`while(v26<6)` 6 人；`v27+=100; if(v27<150) v27=150`（最小宽 150）；高=行数×(字高+5)+5；`CreateUIWndPosSaved` 位置记忆。`ctor @0x91f001` 用 8 方向边框 + `SP_4422_GAUGEBAR/SP_4423_GRADUATION/SP_4424_BAR/SP_1960_GAUGE`。
- `CConfig::GetShowPartyHP @0x49f1ac`：`SP_4433_SHOWPARTYHP` 配置开关默认 1。
- `CUIUserList::OnCreate @0x919b73`：`SP_2090_..._BACKGRND` → `CWnd::SetBackgrnd @0x919cf5`；`CCtrlTab` id=2001 (6,23,w270)；61 个 `SP_` 节点名（好友/组队/公会/黑名单/字体）。好友页 `SP_2099_BtAddFriend → 2010 (5,81) @0x91a0de`；组队页 2200..2208 九钮（含 `SP_2116_BtHP` 血条开关）。
- `CUIPartyHP` 只有 ctor/Create/CreateInstance 三符号、**无 `Draw`** → 比例填充画法用同仓 `UIStatusBar.cpp:2321-2346` 的 `Texture::draw(DrawArgument, {}, Range(0,...))` 裁右样板，而非反编译。

## 3. orange-wz AGENTS.md 门禁（合规 `.nx` 回退）
`tools/_uw_probe.py`（endpoint `http://127.0.0.1:10012/mcp`）：`KEY b64_len=87384 raw_len=65536 sha256_16=268a8407a47ee76c iv=TSPHKw==`；`load_files loadedCount=1 rootCount=1`（结构层 WORKS）；六个 `query_nodes list_children`（`UserList` / `UserList/backgrnd` / `UserList/Party` / `UserList/Party/PartyHP` / `Recommendations` / `Recommendations/FriendsList`）**全部 `IMG 解析失败: UIWindow.img`**（GBK 原文留证）；`unload_all rootCount=0 ok=true` → 合规 `.nx` 回退条件满足。
`.nx` 实测：`UIWindow.img/UserList` 含 `backgrnd [bitmap 312x389]`；`Friend/BtAddFriend` 四态 65×18；`Party/PartyHP/GaugeBar/{bar 63x7 origin(-3,-3), gauge 1x7, graduation 69x13 origin(0,0)}`；`Tab/{enabled,disabled}` 五页签；`UIWindow.img/BtUIClose` 32×15。

## 4. 最小修复（零风险，不动协议）
| 文件 | 改动 |
| --- | --- |
| `UIBuddyList.cpp` | dimension(182,310)→(312,389)；虚构根换 `UIWindow.img/UserList` + `usable(backgrnd)` 守卫；`try_maple_btn(BtAdd, Friend/BtAddFriend, (5,81))`；关闭钮 `close_btn()` (294,6)；行命中上界 190→302 |
| `UIParty.cpp` | dimension(200,300)→(312,389)；`Party.img` 换 `UserList` 真背景；`GaugeBar` 真位图 `partyhp_bar/partyhp_graduation`；四功能钮换 `BtInvite(13,355)/BtWithdraw(190,344)/BtChangeBoss(249,345)/BtKick(72,365)`（btnid→行为不变）；**删 D8 `ColorBox` 血条**改 `graduation.draw` + `bar.draw(..., Range(0,BAR_W-fill_w))` 裁右；行命中上界 190→302 |
| `UIParty.h` | 加 `#include Texture.h` + `Texture partyhp_bar; Texture partyhp_graduation;` |

`IO/LoginBackground.cpp` 线性插值经 harness 守卫复核确属按 Angel 修过，保留不动。

## 5. 范围边界（决策点，交用户裁决）
完整复刻 083「独立可拖拽 `CUIPartyHP` 悬浮窗 + 五页签 `CUIUserList` 共用窗」是架构级：新增 `UIElement::Type` 会改 `NUM_TYPES`/`EnumMap` 尺寸须整工程 Rebuild、位置记忆、`SP_4433_SHOWPARTYHP` 配置开关、页签状态机。本轮**不新建窗口类型/枚举**，只换真资源根+真 dimension+真按钮位图+真 GaugeBar 血条按百分比裁右+放宽行命中区；不动 s9 协议/handler/`memberhp` 数据源；键盘操作/按钮 id→行为全部保留。

## 6. 自测门禁（全绿）
- `tools/test_round68_userlist_083.py`：37 静态守卫 + 16 场景 `PASS` / `TEST_RC=0`（真根/083 立即数/虚构根清除/真血条裁剪语义/6 人独立/083 几何容纳/LoginBackground 保留）。harness 自身守卫 E4 字面量口径就地放宽 1 次，不计任务失败。
- `python tools/gen_opcode_matrix.py --check` → `GATE OK: opcode matrix matches both sources (s9 3741089c0f)`；`git diff --check` → `DIFFCHECK_CLEAN`。
- `tools\build_smoke.bat` → `GATE OK`×2 + `EXITCODE=0`；`build_log.txt 00:02:51/71067B → 01:00:05/57500B` 真重编；`UIParty.cpp` 2 次 + `UIBuddyList.cpp` 2 次；`ERROR_COUNT 0`；EXE 01:00:04 重链接。

## 7. 真机观感验收（`pending-live-tests` §34）
34-1 好友窗真背景/加好友钮；34-2 组队窗真背景/四钮；34-3 血条真 GaugeBar 观感（满/半/空）；34-4 血条与队伍行像素对齐；34-5 窗口拖拽/312 宽布局。已知限制：独立悬浮窗、五页签、配置开关、完整 9 钮、任务指引链未复刻（架构级裁决）。

## 8. 提交 scope
精确 `git add` 8 文件：`UIBuddyList.cpp` / `UIParty.cpp` / `UIParty.h` / `tools/test_round68_userlist_083.py` / `evidence/userlist-partyhp-083-round68.json` / `notes/wave2-userlist-partyhp-083-relayout-20261111.md` / dev-plan README / PLT README。`tools/_uw_*.py`、`tools/_ida_pb/*`、`tools/_uw_visual/*` 仅本地证据链留存不进版本库，evidence JSON 已汇总可追溯路径。零署名原子提交。

## 9. 证据优先级与教训
优先级：`opcode-matrix.json` > 源码 `.dispatch()/emplace` > 运行时 `.nx` > 真机记录 > 旧台账。
教训：① 083 用 `sub_4284FF` vtable 调 `CCtrlButton::Create`，`ops_grep` 找 `CreateCtrl` 字面量 0 命中，须换 `SP_\d+_` 抽坐标；② 第三/第四例同族病 = 找错资源根后「自造兜底」，取证先 `.nx` 列名再定方案；③ CRLF 文件精确替换带 `\r\n` 归一化（`edit` 工具对 tab+中文失配走 python 脚本）；④ harness 匹配真实插值式口径，留证注释里的旧错误字面量用 `code_only()` 排除（round66 同款误报）。
