# round39 · Wave2 2-7 UIGuildBBS 翻页上界守卫复核与修复（2026-11-10）

## 0. 结论

- 继 round34 `UIWorldSelect` / round35 `UICharSelect` / round36 `Textfield` / round37 `UINpcTalk` / round38 `UIRaceSelect` 之后，**第 6 例「主体路径有守卫、旁路分支漏守卫」同族病**：`UIGuildBBS::button_pressed` 的 `BtPageLeft` 有 `if (page_index > 0)` 下界守卫，`BtPageRight` 却直接 `request_list(page_index + 1)` 无上界。
- 后果（逐字链）：鼠标点 BtPageRight、键盘 `'.'`/`']'` 两条旁路都灌进同一个 `button_pressed` → 末页再点「下一页」发一个必然返回空列表的越界页请求 → `apply_list():163-169` 把 `threads` 换空、`selected_row = threads.empty() ? -1 : 0` 清成 `-1`、`set_page(Page::List)` 窗口空白、`page_index` 停在越界值，只能再点「上一页」回退。
- 最小修复：`IO/UITypes/UIGuildBBS.cpp` 与 `BtPageLeft` 对称补「当前页未满即末页」上界守卫（`threads.size() >= LIST_MAX_ROWS`），`4 insertions(+), 1 deletion(-)`；不改协议/包体/WZ/IMG/XML，不依赖 `total_threads` 的服务端语义。
- 自测 `tools/test_guildbbs_page_guard.py`：5 静态守卫 + 10 边界 case PASS，`before bad=5 -> after 0`，合法满页翻页与 `BtPageLeft` 回归 `4/4`。协议 `GATE OK`，构建 `GATE OK + EXITCODE=0` + `ERROR_LINES 0`，`build_log.txt` 真重编译。

## 1. 证据优先级与开工纪律

沿用 `feature-gap-inventory` §2.3：`opcode-matrix.json` > 源码 `.dispatch()/emplace` > 现场重算 > 真机记录 > 旧账。本轮**不拿 11-04 旧账立项**，全部从 live `IO/UITypes/UIGuildBBS.cpp`（HEAD `9ea38a57`）逐字读起；`git diff --stat HEAD -- IO/UITypes/UIGuildBBS.cpp` 修复前为空，确认是已提交态、非本会话残留。

**orange-wz 门禁口径**：本轮定位的是 C++ 交互逻辑（`button_pressed` 翻页分支与 `apply_list` 状态更新），**不解析/查询/核对任何 WZ/IMG/XML 节点**，故不触发 AGENTS.md 的 orange-wz 前置条件。同一 Wave2 批次的 IMG 环已在 round37 由 `tools/wave2_gate_npctalk_probe.py` 真闭合（`evidence/orange-wz-npctalk-ui-structure.json` 结构层 `Data/UI` 20 个真 .img 子节点 WORKS / 内容层 4 次 `IMG 解析失败` FAILS，65536B GMS key 规格 `268a8407a47ee76c` iv `TSPHKw==` 留证 → 合规 `.nx` 回退 `UI.nx UtilDlgEx controls=24`），本轮直接复用该证据，不重复造门禁、也不虚构节点查询。

立项落点取开发计划 §4 Wave2 2-3 明确点名的「玩家列表五页签 / UIGuildBBS 多页」：`UIUserList` 五个顶层页签与三个子页签族逐字核准后均有 `tab_button()` 显式映射 / 守卫（`change_party_tab` / `change_friend_tab` / `change_blacklist_tab`），无同族缺陷；真缺口收敛到 `UIGuildBBS` 翻页上界。

## 2. live 源码逐字对照

| 位置 | 逐字 | 含义 |
| --- | --- | --- |
| `UIGuildBBS.h:93` | `enum class Page : uint8_t { List, View, Write };` | 三页状态 |
| `UIGuildBBS.h:97` | `static constexpr int16_t LIST_MAX_ROWS = 12;` | 单页满行阈值（本轮守卫唯一常量） |
| `UIGuildBBS.h:121-122` | `int32_t page_index = 0;` / `int selected_row = -1;` | 页码 / 列表选中 |
| `UIGuildBBS.cpp:149-156` | `void UIGuildBBS::request_list(int32_t page_i){ if (page_i < 0) page_i = 0; page_index = page_i; BbsListPacket(page_index).dispatch(); ... }` | **已有负页下界，但没有上界** |
| `UIGuildBBS.cpp:158-175` | `apply_list(...)` 内 `threads = std::move(page_rows);` / `if (page_index_hint >= 0) page_index = page_index_hint;` / `selected_row = threads.empty() ? -1 : 0;` / `set_page(Page::List);` | 空列表把选中态清成 -1、页码锁在越界值 |
| `UIGuildBBS.cpp:188-193` | `const BbsThreadSummary* selected_summary() const { if (selected_row >= 0 && selected_row < static_cast<int>(threads.size())) ... return nullptr; }` | 读帖/删除已有守卫（主体安全） |
| `UIGuildBBS.cpp:122-147` | `refresh_button_visibility()`：`show(BtPageLeft, list);` / `show(BtPageRight, list);` | 两钮在 List 页常驻 active，**没有按末页禁用** |
| `UIGuildBBS.cpp:512-515` | `case BtPageLeft: if (page_index > 0) request_list(page_index - 1); break;` | **修复前有下界守卫** |
| `UIGuildBBS.cpp:516-518` | `case BtPageRight: request_list(page_index + 1); break;` | **修复前无上界守卫（不对称）** |
| `UIGuildBBS.cpp:557-560` | `else if (keycode == ',' \|\| keycode == '[') button_pressed(BtPageLeft);` / `else if (keycode == '.' \|\| keycode == ']') button_pressed(BtPageRight);` | 键盘旁路灌进同一 `button_pressed`，与鼠标同源 |
| `UIGuildBBS.cpp:326-341` | `send_cursor` List 页 `if (row >= 0 && row < threads.size())` 才设 `selected_row` | 主体命中带守卫，旁路翻页漏守卫 |

`BtPageRight` 无上界不是「多发一个无害请求」：`apply_list` 的空列表回包会把窗口状态写坏（选中态 -1、页码越界），是真 UI 状态缺陷，不是纯性能浪费。

## 3. 最小修复（逐字）

`tools/_apply_wave2_bbs_fix.py` 动态探测 tab 缩进（`ANCHOR case_indent=3 body_indent=4`，round34/35/37/38 教训：edit 工具对 tab + 中文注释的 old_string 易失配，python 精确替换更稳），`OLD_FOUND 1` 后写入：

```cpp
			case BtPageRight:
				// 与 BtPageLeft 的 page_index > 0 下界对称：当前页未满即末页，
				// 不发必然返回空列表的越界页请求（否则 selected_row 被清成 -1、
				// 窗口空白、page_index 停在越界值，只能再点「上一页」回退）。
				if (static_cast<int>(threads.size()) >= LIST_MAX_ROWS)
					request_list(page_index + 1);
				break;
```

设计取舍（P8 颗粒度）：
1. 用 `threads.size() >= LIST_MAX_ROWS` 判「本页满 = 可能还有下一页」，**纯客户端铁事实**，不依赖 `total_threads` 的服务端分页语义（s9 的 total 计数/公告条是否占行各版本不一致，拿它算页数会引入新耦合）；
2. 与既有 `request_list(page_i < 0)` 下界、`BtPageLeft(page_index > 0)` 下界形成对称三件套；
3. 满页时行为与修复前逐字一致（回归 4/4 实证），只在「本页未满」这一唯一真越界条件上拦截；
4. 不碰 `refresh_button_visibility()` 的按钮态（那是观感层，真机联测项，不在本轮最小改动内）。

## 4. 自测与门禁实证

- `tools/test_guildbbs_page_guard.py`：静态核验 `right_guard` / `right_guard_comment` / `left_lower_bound_kept` / `list_max_rows_const` / `request_list_lower_bound_kept` 5 段逐字在盘；动态 `page_left` / `page_right(page_index, n_rows, guarded)` 复刻修复前（无条件右翻）与修复后（未满即拦），`request(p)` 记录发出的页请求。
  - 10 case：`A_empty_page0` / `B_last_3rows` / `C_last_11rows` / `D_last_8rows_p2` / `G_last_3rows_p5` 五个末页（未满）修复前均发越界 `request(n+1)`、修复后 `None`；`E_full_page0` / `F_full_page2` 两个满页修复前后均 `request(n+1)`（合法回归）；`H_left_at_0` / `I_left_at_2` 验 `BtPageLeft` 未改；`J_keyboard_route` 静态确认 `'.'`/`']'` → `button_pressed(BtPageRight)` 与鼠标同源。
  - `PASS: 5 static guards + 10 cases (before bad=5 -> after 0; regression OK)` `TEST_RC=0`。
- `python tools/gen_opcode_matrix.py --check` → `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)`。
- `tools\build_smoke.bat`（`MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c`，round34 教训：裸 `cmd.exe //c` 在 MSYS 下只回显 banner 是假执行）：`GATE OK` + `EXITCODE=0`；`build_log.txt` mtime/size 见构建 job 回填；`ERROR_LINES 0`；tail `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe` + 复制文件行。既有 warning `LNK4098 LIBCMT` / `LNK4099 vc120.pdb` / `/LTCG` 提示，无 error/fatal。
- `taskkill /F /IM MapleStory.exe /T`：34476 / 6108 / 56016 三个 32K 空壳报 `Access is denied`（无 ExecutablePath、不占资源、不阻断构建）；50412 已于 round34 基线轮终止。用户 m04292「遇客户端占用直接关闭」已执行。

## 5. 不在本轮范围

- 真机 BBS 观感（登录服在跑、公会 BBS 回包、末页按钮 disabled 态、公告条是否占一行导致 `LIST_MAX_ROWS` 判定偏保守）→ `pending-live-tests` §18 人工联测，不冒充已验证。
- Wave2 2-1（各面 F 维 12 项样式降级，等内容层恢复按 orange-wz 节点逐像素对齐）、2-2（Photoshop MCP 资源层）、2-4（隐藏门三态/SKILL_EFFECT/NPC_ACTION 真机触发）仍挂起。
- Wave3 L-B（场地伤害族 c2s、Quest 107 x/y、`PARTY_SEARCH_REGISTER(220)`、`MTS_OPERATION(0xFD)` 等涉包体结构项）待用户裁决 D1–D14，本计划不自动开工。
- 083/v83 唯一基准禁 095；校准≠改回原生；s9 优先；数值重合≠语义重合；校准十一面文档只读（本轮按 m04292 授权改游戏源码）。

## 6. 提交 scope

精确 `git add` 6 文件，一功能一原子提交、零署名、`<Emoji> <Type>(<Scope>): <标题> (#<IssueID>)`：

| 文件 | 状态 | 用途 |
| --- | --- | --- |
| `IO/UITypes/UIGuildBBS.cpp` | M | 最小修复 |
| `tools/_apply_wave2_bbs_fix.py` | A | 一次性精确补丁（可复现） |
| `tools/test_guildbbs_page_guard.py` | A | 离线自测 harness |
| `docs/features/development-and-fix-plan-2026-11/notes/wave2-guildbbs-recheck-20261110.md` | A | 本复核记录 |
| `docs/features/development-and-fix-plan-2026-11/README.md` | M | §4 Wave2 新增 2-7 |
| `docs/features/pending-live-tests/README.md` | M | §18 人工联测 4 项 |

calibration evidence / 截图 / `*.obj` / `_*.log` / `build_log.txt` / `.nx` 不进功能提交。`.gitignore` 混合编码（round34 已证 `ERR_INVALID_ENCODING`，既有规则已覆盖 `__pycache__`/`*.pyc`/`*.log`/`*.obj`/`*.nx`）→ 不改 `.gitignore`，改精确 `git add`。

---

**性质标注**：源码修复 + 离线自测 + 构建门禁 + 原子提交；校准文档仍为只读证据链，本轮未改任何协议/包体/WZ/IMG/XML 资源。
