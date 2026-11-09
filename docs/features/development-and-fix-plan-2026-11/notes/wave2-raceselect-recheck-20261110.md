# round38 · Wave2 2-6 UIRaceSelect SoftKey 可视窗口守卫复核与修复（2026-11-10）

## 0. 结论

- 继 round34 `UIWorldSelect` / round35 `UICharSelect` / round36 `Textfield` / round37 `UINpcTalk` 之后，**第 5 例「主体路径有守卫、旁路分支漏守卫」同族病**：`UIRaceSelect::send_key` 的 SoftKey 数字选职业分支只卡上界 `idx < order.size()`，未卡当前可见窗口 `[index_shift, index_shift + INDEX_COUNT)`。
- 后果（逐字复刻 `button_pressed` 后实测）：翻页后输小号数字 → `bid = CLASS0 + idx - index_shift` 落到 `BtStart/BtPreview/BtMake/BtLeftArrow/BtRightArrow` 功能钮（误切世界/误开确认框/误翻页）；输大号数字 → `button_pressed` 的 `if (buttonid >= CLASS0)` **无上界**，`select_class()` 读 `class_index[selected]` 越界。
- 最小修复：`IO/UITypes/UIRaceSelect.cpp` SoftKey 分支补可见窗口守卫（4 行条件 + 1 行注释，源码 diff `4 insertions(+), 1 deletion(-)` 语义等价 3 行净增），不改协议/包体/WZ/IMG/XML。
- 自测 `tools/test_raceselect_softkey_guard.py`：4 静态守卫 + 10 边界 case PASS，`before bad=6 -> after bad=0`，合法职业按钮回归 `4/4`。协议 `GATE OK`，构建 `GATE OK + EXITCODE=0` + `ERROR_LINES 0`，`build_log.txt` `00:08:03 → 00:36:04` 真重编译。

## 1. 证据优先级与开工纪律

沿用 `feature-gap-inventory` §2.3：`opcode-matrix.json` > 源码 `.dispatch()/emplace` > 现场重算 > 真机记录 > 旧账。本轮**不拿 11-04 旧账立项**，全部从 live `IO/UITypes/UIRaceSelect.cpp`（HEAD `09c9a0ec`）逐字读起；`git diff --stat HEAD -- IO/UITypes/UIRaceSelect.cpp` 修复前为空，确认是已提交态、非本会话残留。

**orange-wz 门禁口径**：本轮定位的是 C++ 交互逻辑（`send_key` SoftKey 分支与 `button_pressed` 映射），**不解析/查询/核对任何 WZ/IMG/XML 节点**，故不触发 AGENTS.md 的 orange-wz 前置条件。同一 Wave2 批次的 IMG 环已在 round37 由 `tools/wave2_gate_npctalk_probe.py` 真闭合（`evidence/orange-wz-npctalk-ui-structure.json` 结构层 `Data/UI` 20 个真 .img 子节点 WORKS / 内容层 4 次 `IMG 解析失败` FAILS，65536B GMS key 规格 `268a8407a47ee76c` iv `TSPHKw==` 留证 → 合规 `.nx` 回退 `UI.nx UtilDlgEx controls=24`），本轮直接复用该证据，不重复造门禁、也不虚构节点查询。

## 2. live 源码逐字对照

| 位置 | 逐字 | 含义 |
| --- | --- | --- |
| `UIRaceSelect.h:66` | `static constexpr uint16_t INDEX_COUNT = 6;` | 可见职业槽数 / `class_index[]` 长度 |
| `UIRaceSelect.h:139-142` | `uint16_t class_index[INDEX_COUNT];` / `uint16_t index_shift;` / `uint16_t selected_index;` | 当前窗口起点（翻页累加） |
| `UIRaceSelect.h:70-82` | `BtStart=0, BtPreview=1, BtMake=2, BtLeftArrow=3, BtRightArrow=4, CLASS0=5 … CLASS4=9` | 功能钮在 CLASS0 之前 |
| `UIRaceSelect.cpp:304-305` | `index_shift = 0; selected_index = 0;` | modern 初始化 |
| `UIRaceSelect.cpp:553` | `} else if (idx < static_cast<int>(order.size())) {` | **修复前只有全局上界** |
| `UIRaceSelect.cpp:554` | `button_pressed(static_cast<uint16_t>(Buttons::CLASS0 + idx - index_shift));` | idx 是全局 order 下标，减去窗口起点才是按钮号 |
| `UIRaceSelect.cpp:742-747` | `if (buttonid >= Buttons::CLASS0) { uint16_t index = buttonid - Buttons::CLASS0 + index_shift; select_class(index); return Button::State::IDENTITY; }` | **职业分支无 buttonid 上界** |
| `UIRaceSelect.cpp:761-768` | `uint16_t previous = previous_index - index_shift;` / `uint16_t selected = selected_index - index_shift;` / `selected_class = class_index[selected];` | `class_index[selected]` 越界读点 |
| `UIRaceSelect.cpp:701-739` | `BtLeftArrow`/`BtRightArrow` 各自 `index_shift--/++` 并重排 `class_index[0..5]` | 窗口会移动，SoftKey 必须跟着窗口走 |
| `UIRaceSelect.cpp:651-679` | `classic_mode` 分支 `buttonid >= CLASS0 && buttonid < CLASS0 + CLASSIC_CLASS_COUNT` 有上界 | **经典模式反而有守卫，modern 漏了**（同族不对称） |
| `UIRaceSelect.cpp:539-557` | SoftKey lambda `idx|make`，`if (idx < 0) return;` | 唯一数字入口 |

主路径 `send_cursor`（`:455-490`）只把 `btit.first`（构造期由 `CLASS0..CLASS4` 生成的合法 buttonid）传给 `button_pressed`，因此鼠标点击永远安全；**只有 SoftKey 数字旁路能把非法 buttonid 灌进来**——这就是「主体有守卫、旁路漏守卫」。

## 3. 最小修复（逐字）

`tools/_apply_race_softkey_fix.py` 动态探测 tab 缩进（`ANCHOR indent=5 cont=7`，round34/35/37 教训：edit 工具对 tab + 中文注释的 old_string 易失配，python 精确替换更稳），断言锚点各出现 1 次后写入：

```cpp
				} else if (idx >= index_shift
						&& idx < static_cast<int>(index_shift) + INDEX_COUNT
						&& idx < static_cast<int>(order.size())) {
					// 仅当前可见窗口的职业按钮可经 SoftKey 触发；越界 idx 会误触功能按钮或越界读 class_index[]。
					button_pressed(static_cast<uint16_t>(Buttons::CLASS0 + idx - index_shift));
				}
```

三条约束的用意：
1. `idx >= index_shift` —— 挡住「翻页后输小号数字」，避免 `bid < CLASS0` 落到功能钮；
2. `idx < index_shift + INDEX_COUNT` —— 挡住「输大号数字」，避免 `class_index[selected]` 越界；
3. `idx < order.size()` —— 保留原全局上界，防止 `order` 尾部窗口不足 6 项时越界读 `order`。

`classic_mode` 分支不动（已有 `idx < CLASSIC_CLASS_COUNT` 上界且 `select_classic_class():627` 自带 `index >= CLASSIC_CLASS_COUNT return`）。

## 4. 自测与门禁实证

- `tools/test_raceselect_softkey_guard.py`：静态核验 `cond_lower`/`cond_upper`/`cond_global`/`comment` 4 段守卫逐字在盘；动态 `softkey(idx, index_shift, guarded)` 复刻 `bid = uint16(CLASS0 + idx - index_shift)` 与 `button_pressed` 真实分支（`bid<CLASS0` → 功能钮 MISFIRE；`bid>=CLASS0` 无上界 → `class_index[bid-CLASS0]` OOB）。10 case 分 MIS/OOB/REG 三类。
  - 实跑输出：`A_small_btstart before=MISFIRE:BtStart(show_worldselect) after=None`、`B_btmake before=MISFIRE:BtMake(confirm-dialog) after=None`、`C_btright before=MISFIRE:BtRightArrow after=None`、`D_far_oob before=OOB:class_index[10] after=None`、`I_just_past before=OOB:class_index[6] after=None`、`J_shift10_tail before=OOB:class_index[9] after=None`；`E/F/G/H` 四个合法职业按钮前后均 `CLASS:0/5`。
  - `PASS: 4 static guards + 10 cases (before bad=6 -> after bad=0; regression 4/4)` `TEST_RC=0`。
  - **事实驱动自净**：首版 harness 曾把大号 idx 标成 `no_button`，与 live `:742` 无上界分支不符（真实是 `class_index[]` OOB 读）；落盘前按 live 逐字改为 `OOB:class_index[n]`，不改标尺、不删 case。
- `python tools/gen_opcode_matrix.py --check` → `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)`。
- `tools\build_smoke.bat`（`MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c`，round34 教训：裸 `cmd.exe //c` 在 MSYS 下只回显 banner 是假执行）：`GATE OK` + `EXITCODE=0`；`build_log.txt` mtime `2026-11-10 00:08:03 → 00:36:04`、size `3551 → 5737`（真重编译）；`ERROR_LINES 0`；tail `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe` + 复制文件行。既有 warning `LNK4098 LIBCMT` / `LNK4099 vc120.pdb` / `/LTCG` 提示，无 error/fatal。
- `taskkill /F /IM MapleStory.exe /T`：34476 / 6108 / 56016 三个 32K 空壳报 `Access is denied`（无 ExecutablePath、不占资源、不阻断构建）；50412 已于 round34 基线轮终止。用户 m04292「遇客户端占用直接关闭」已执行。

## 5. 不在本轮范围

- 真机 SoftKey 输入观感（登录服在跑、RaceSelect_new 资源、字图渲染）→ `pending-live-tests` §17 人工联测，不冒充已验证。
- Wave2 2-1（各面 F 维 12 项样式降级，等内容层恢复按 orange-wz 节点逐像素对齐）、2-2（Photoshop MCP 资源层）、2-4（隐藏门三态/SKILL_EFFECT/NPC_ACTION 真机触发）仍挂起。
- Wave3 L-B（场地伤害族 c2s、Quest 107 x/y、`PARTY_SEARCH_REGISTER(220)`、`MTS_OPERATION(0xFD)` 等涉包体结构项）待用户裁决 D1–D14，本计划不自动开工。
- 083/v83 唯一基准禁 095；校准≠改回原生；s9 优先；数值重合≠语义重合；校准十一面文档只读（本轮按 m04292 授权改游戏源码）。

## 6. 提交 scope

精确 `git add` 6 文件，一功能一原子提交、零署名、`<Emoji> <Type>(<Scope>): <标题> (#<IssueID>)`：

| 文件 | 状态 | 用途 |
| --- | --- | --- |
| `IO/UITypes/UIRaceSelect.cpp` | M | 最小修复 |
| `tools/_apply_race_softkey_fix.py` | A | 一次性精确补丁（可复现） |
| `tools/test_raceselect_softkey_guard.py` | A | 离线自测 harness |
| `docs/features/development-and-fix-plan-2026-11/notes/wave2-raceselect-recheck-20261110.md` | A | 本复核记录 |
| `docs/features/development-and-fix-plan-2026-11/README.md` | M | §4 Wave2 新增 2-6 |
| `docs/features/pending-live-tests/README.md` | M | §17 人工联测 4 项 |

calibration evidence / 截图 / `*.obj` / `_*.log` / `build_log.txt` / `.nx` 不进功能提交。`.gitignore` 混合编码（round34 已证 `ERR_INVALID_ENCODING`，既有规则已覆盖 `__pycache__`/`*.pyc`/`*.log`/`*.obj`/`*.nx`）→ 不改 `.gitignore`，改精确 `git add`。

---

**性质标注**：源码修复 + 离线自测 + 构建门禁 + 原子提交；校准文档仍为只读证据链，本轮未改任何协议/包体/WZ/IMG/XML 资源。
