# round67 · HUD 主窗 4 钮闪烁按 Angel 083 SetButtonBlink 状态机重修

## 0. 性质与授权
- User said (m09859): "若碰到ui布局、动画一定要确认是否修复了，如果不是按照Angel 布局修复过的，可以重新修复；之前有很多参照BeiDou或者自行修复的ui、动画都有些问题；" —— 本轮验收标准：非按 Angel 083 布局/动画修复的 UI 逐面重修。
- m04292 授权：改游戏源码 / 自测 / 测试完成提交 / 遇 MapleStory.exe 占用直关 / 不能自测记 backlog。
- 范围：`statusbar-hud/D9` 按钮闪烁面。取证只读 → 只实施零风险视觉层+时序 → harness → 真重编 → 三文档 → 原子提交。校准≠改回原生（不动 s9 协议）。

## 1. 缺陷逐字与翻案
外部程序 `d6bdce97` 在 `IO/UITypes/UIStatusBar.cpp:2502` 写「D9：主窗 4 钮闪烁提示叠层（原生 SetButtonBlink；**本包无 KEY_ANI 资源，用脉冲高亮**）」，据此自造半透明白块 7.5Hz 脉冲。三层全错：

| 层 | 外部程序（自行修复） | Angel 083 权威 | 证据 |
| --- | --- | --- | --- |
| 资源 | 断言「本包无 KEY_ANI 资源」→ 手绘白块 | `SP_1986..1989_..._KEY_ANI` 四组真动画；`.nx` 实测 `StatusBar.img/<Key>/ani/{0,1}` 两帧 28x20 delay 500 全在 | `tools/_ida_hud_blink/SetButtonBlink.txt` + `tools/_sb_visual/*.png` |
| 频率 | 每 4 帧翻转 ≈7.5Hz + 0.75s alpha 淡出 | delay 500ms 两帧 = 1Hz，无淡出 | `.nx` `ani/0`,`ani/1` `delay [int] 500` |
| 时序 | 点击开子窗即启动 0.75s 脉冲 | 开子窗 / 子窗通知100 / 90s 超时 = **清除**(a3=0)；升级<=10 / 任务指引 = **启动**(a3=1) | 5 个 xref 调用点 a3 全表 |

像素坐实：`EquipKey_ani_0.png` 亮黄发光版（633B, sha256 `c2382f4e...`）、`ani_1` 常态暗版（427B, sha256 `a66b5b5b...`），`EquipKey_normal_0.png` 的 sha256 与 `ani_1` **完全相同** → 083 KEY_ANI 就是「常态帧 ↔ 发光帧」。与 round66「找错节点后自造兜底」完全同族，正是 m09859 点名的「自行修复 UI 有问题」。

## 2. IDA 083 权威取证（`tools/_ida_hud_blink/*.txt`）
- `?SetButtonBlink@CUIStatusBar@@QAEXJH@Z @0x8dfe21 size 0x5d0`：`if (a2 < 4) { if (a3 == 0) return sub_428712(0); ... }` —— a3==0 清除分支直接 return；a3!=0 才走 `StringPool::GetBSTR`→`IWzResMan::GetObjectA` 加载四组 KEY_ANI；`a3==1` 记 `timeGetTime()`，其它值置 -1。
- 5 个调用点 a3 语义（决定性，方向与外部程序相反）：

| 调用点 | EA | call | a3 | 083 语义 |
| --- | --- | --- | --- | --- |
| `OnButtonClicked` | `0x8d423d` | `0x8d4316` | 0 | 2000-2003 `UI_Toggle` 后**清除** |
| `OnChildNotify` | `0x8d419c` | `0x8d420d` | 0 | a3==100 子窗通知**清除** |
| `Update` | `0x8de964` | `0x8dea40` | 0 | `v8+90000 < timeGetTime() && v8!=-1` **90s 超时清除** |
| `OnStatChanged` | `0xa1fb52` | `0xa1fce9` | 1 | 低等级<=0xA(10) 升级 → STAT 钮(idx2)**启动** |
| `CUtilDlgEx::Draw` | `0x9a3e9f` | `0x9a4256` | 1 | 任务指引 case3, v32<4 → **启动** |

- `SP_1986..1989` 是 IDA 局部重命名常量：`list_globals filter=*1986*` 与 `py_eval idautils.Names()` 均 0 命中 → 明文节点路径只能靠 `.nx`。

## 3. orange-wz AGENTS.md 门禁（合规走完）
`tools/_sb_probe.py`（endpoint `http://127.0.0.1:10012/mcp`，key 从 `tools/_gms_key.b64` 读避免 87K 入上下文）：`KEY b64_len=87384 raw_len=65536 sha256_16=268a8407a47ee76c iv=TSPHKw==`；`INIT` session `8b40334f-...`；`load_files` `loadedCount=1 rootCount=1 generation=1`（结构层 WORKS）；`query_nodes list_children` 对 `EquipKey`/`EquipKey/ani`/`InvenKey/ani`/`StatKey/ani`/`SkillKey/ani` **五次全部 `IMG 解析失败: StatusBar.img`**（GBK 原文留证）；`unload_all rootCount=0 ok=true`。→ 合规 `.nx` 回退触发条件满足。数据 `E:/MXD/BeiDou-Client-xy/Data/UI/StatusBar.img`(135373B) + `.nxcache/UI.nx`(86919577B)。

## 4. 最小修复（按 083 状态机）
- `IO/UITypes/UIStatusBar.h`：删 `blink_frames`/`BLINK_DURATION`；加 `#include "../../Graphics/Animation.h"` + `<cstdint>`；成员 `std::array<Animation,4> blink_ani`（四组真帧）、`std::array<int64_t,4> blink_elapsed_ms={-1,-1,-1,-1}`（083 v8=-1 未启动）、`blink_last_level`、`BLINK_TIMEOUT_MS=90000`、`BLINK_LEVEL_LIMIT=10`；声明 `start/stop_button_blink` + `blink_index`。
- `IO/UITypes/UIStatusBar.cpp`：
  - `init_classic()` 在 4 个 `add_btn` 后构造 `blink_ani[i] = Animation(sb[<Key>]["ani"])`（`.nx` 实测两帧 500ms）。
  - 4 处开子窗分支 `start_button_blink(id)` → `stop_button_blink(id)`（083 a3=0 清除）。
  - `start_button_blink` = 083 a3==1 启动（elapsed=0 + reset 动画）；`stop_button_blink` = 083 a3==0 清除（elapsed=-1）。
  - `draw()` 用 `blink_ani[i].draw(DrawArgument(r.get_left_top()), alpha)` 叠真发光帧，替换 `ColorBox` 白块。
  - `update_classic()` 逻辑帧累计毫秒（`+= Constants::TIMESTEP`）等价 90s 超时清除 + `blink_ani[i].update()` 推进 1Hz；`stats.get_stat(LEVEL)` 自感知低等级升级触发 STAT 钮启动（对齐 `OnStatChanged @0xa1fce9`，**不动 s9 协议**）。
  - 三处错注释改引 083 真符号与 `.nx` 实测。
- 保留确认：`IO/LoginBackground.cpp` 线性插值（删 smoothstep）经复核确属按 Angel 083 CLogin 修复过，本轮不动（harness G1 守卫）。

## 5. 自测门禁（全绿）
- `tools/test_hud_button_blink_083.py`：`STATIC GUARDS checked=37 failed=0` / `SCENARIOS 13, bad=0` / `PASS` / `TEST_RC=0`。守卫含：真动画成员/083 常量/三方法、旧自造痕迹清除（`blink_frames`/`ColorBox`/`7.5Hz`/误判注释）、4 组真 KEY_ANI 构造、状态机方法、draw 用真帧、update 90s+动画+升级、LoginBackground 保留。场景含：初态全灭、升级启动 STAT、开子窗清除、90s 超时、未超时不清除、高等级不触发、等级 9->10/10->11 边界、真帧 500ms 翻转、stop 幂等、四钮独立、超时长期保持。
- harness 自身 1 次口径校准（不计任务失败）：场景8 `500/8=62.5` 帧才翻一帧，初版整除 62 断言失败 → 改 `ceil=63`。
- `git diff --check` → `DIFFCHECK_CLEAN`；`python tools/gen_opcode_matrix.py --check` → `GATE OK: opcode matrix matches both sources (s9 3741089c0f)`。
- 真重编：`tools\build_smoke.bat` `GATE OK` x2 + `EXITCODE=0`；`build_log.txt` 23:16:51/460448B → 00:02:51/71067B（真重编）；`UIStatusBar.cpp` 编到 22 次；`ERROR_COUNT 0`；`MapleStory.exe` 00:02:50 / 4944384B 重链接。

## 6. 范围边界
- 只改 `UIStatusBar.h/.cpp`；不动 s9 协议、不动 `RecommendedWorldMessageHandler`、不动其它 handler。
- 启动链：083 的 `CUtilDlgEx::Draw` 任务指引（idx<4）当前端无对应实体，**未复刻**；升级启动用 UI 层等级自感知（等价 `OnStatChanged` 视觉效果，非协议接线）。
- 校准≠改回原生：`SetButtonBlink` 的原生 `CCtrlScrollBar`/`ZArray<RECOMMENDWORLDMSG>` 等不照搬，仅复用当前端已有的 `Animation`/`Button`/`UIElement` 框架。

## 7. 真机 backlog（进 pending-live-tests §33）
- 33-1 升级（<=10 级）时 STAT 钮是否按 1Hz 亮黄发光帧闪烁。
- 33-2 开装备/背包/属性/技能子窗时对应钮闪烁是否立刻清除（083 a3=0）。
- 33-3 90 秒未开子窗时闪烁是否自动停止。
- 33-4 真发光帧（28x20）与常态按钮位的像素对齐、无错位/无残影。
- 33-5 任务指引/其它需 blink 的场景当前端未接线，观感是否符合预期（交裁决）。

## 8. 提交 scope（实际 7 文件，与提交清单逐字一致）
精确 `git add` 7 文件：`IO/UITypes/UIStatusBar.h` / `IO/UITypes/UIStatusBar.cpp` / `tools/test_hud_button_blink_083.py` / `docs/features/development-and-fix-plan-2026-11/evidence/hud-button-blink-083-round67.json` / `docs/features/development-and-fix-plan-2026-11/notes/wave2-hud-button-blink-083-relayout-20261111.md` / `docs/features/development-and-fix-plan-2026-11/README.md` / `docs/features/pending-live-tests/README.md`。零署名（`<Emoji> <Type>(<Scope>): <标题> (#<IssueID>)`）。
一次性补丁/探针脚本（`tools/_sb_patch.py` / `tools/_sb_docs_patch.py` / `tools/_sb_probe.py`）、IDA 反编译 `tools/_ida_hud_blink/*.txt`（13 个）、真帧导出 `tools/_sb_visual/*.png`（9 个）与 `build_log.txt` 按 round66 同款惯例仅作本地证据链留存，不进版本库；evidence JSON 已汇总其路径与结论，保证可追溯。

## 9. 证据优先级与教训
优先级：live 运行时（083 反编译立即数）> `.nx` 节点实测 > 校准 README 旧结论 > 外部程序自写注释。
- 教训1：注释里「本包无资源」是**未查 `.nx` 的断言**，与 round66 同族；AGENTS.md 门禁（orange-wz→`.nx`）就是用来翻这种案。
- 教训2：翻案要核「语义链」不只核「资源」——本例资源/频率/时序三层全反，只换素材仍错。
- 教训3：改公共头布局（`std::array<Animation,4>` 成员）必须真重编确认覆盖，build_log 里要看到 `UIStatusBar.cpp` 被重编（22 次）。
- 教训4：`Animation::update()` 按 `delay -= timestep` 递减，delay=500/TIMESTEP=8 → ceil 63 帧翻一帧；harness 场景不能用整除。
