# round35 · Wave1 现场复核 + UICharSelect 键盘导航边界修复

> 性质：开发/修复阶段（用户 m04292 授权「可自测自己测 / 功能测试完成就提交 /
> 遇 MapleStory.exe 占用直接关闭」）。本轮首次改 `UICharSelect.cpp`；协议/WZ/IMG
> 一律只读，083/v83 唯一基准禁 095，校准≠改回原生，s9 优先，数值重合≠语义重合。
> 味道：🟠 阿里味。失败计数：已确认失败 0 次 → L0（终端 edit old_string 未命中
> 属工具参数问题、非任务失败；开工诊断的具体数字被自测 harness 证伪按事实改写，
> 不隐藏）。

## 0. 结论

1. **Wave1 五项里 1-2 / 1-4 是有意裁决，不是缺口**：`RECALCULATE_STATS(35)`
   撞值已在 `Net/PacketSwitch.cpp:735-740` 明确注销并写明理由（s9 在 35 只发
   `FORCED_STAT_RESET`，被顶掉会导致强制属性结束后属性不回弹）；
   `WEEK_EVENT_MESSAGE(77)` 在 `Net/Handlers/MessagingHandlers.cpp:504-510` 明确
   改走 yellow tip，强接 `UIEvent` 会让 GM 反馈看不见且资源缺失半成品越界崩
   （`UIEvent.cpp:123`，`crash-chatbar.md §8`）。照 11-04 旧账硬改 11-09 已提交
   源码 = 返工。证据优先级 §2.3：协议矩阵 > 源码 > 现场重算 > 真机 > 旧账。
2. **1-1 收包「空实现」多数是 v83 正确语义**：`WeddingCeremonyEndHandler`
   （`WaveAShellHandlers.cpp:507-513`）deactivate `UIWedding`、
   `MonsterCarnivalLeaveHandler`（`:710-714`）deactivate `UIMonsterCarnival` 本就是
   结束/离开该做的事；`MTS_OPERATION`(`:3551-3559`)/`MtsOperation2`(`:3561-3578`)
   已读包 + `set_status` + `set_cash`。`FieldEffectHandler` 的
   `Unknown value` LOG（`TestingHandlers.cpp:186`）是未知 mode 的既有兜底，非死代码。
3. **同构排查揪出一处真实缺陷并修复**：`UICharSelect::send_key` 网格导航分支与
   round34 已修的 `UIWorldSelect::send_key`（commit `72f1a6ce`）是同一类边界病，
   本轮补 2 处守卫（13 行插入，仅 `IO/UITypes/UICharSelect.cpp`）。
4. 自测 `tools/test_charselect_guard.py` = 2 守卫静态核验 + 10 边界 case
   修复前/修复后对照 PASS；构建 `GATE OK` + `EXITCODE=0`；原子提交。

## 1. Wave1 旧账 ↔ live 源码逐字对照（2026-11-09 22:4x 现场）

| 计划项 | 旧账口径（`feature-gap` §9.5.3 / 计划 §4） | live 源码逐字 | 判定 |
| --- | --- | --- | --- |
| 1-2 撞值 35 | `RecalculateStatsHandler` 死代码，需去重/明确优先级 | `PacketSwitch.cpp:735-740` 已把 `emplace<RECALCULATE_STATS,...>()` 注销，注释逐字「后注册的赢 ⇒ 原先这条把 :640 的 ForcedStatResetHandler 顶掉了……本行因此注销，枚举项保留作 v83 命名参考」；`PlayerHandlers.cpp:275-280` handler 仍在（未删，供其它调用） | ✅ 有意裁决销案，零改动 |
| 1-4 WEEK_EVENT 77 | `UIEvent` 16 条硬编码无数据通路，需接线 | `MessagingHandlers.cpp:501-510` 先 `while(recv.available()) skip_byte()` 消费 s9 尾 short，再 `chatbar->show_message(..., YELLOW)`；注释逐字「此前强行开窗既让 GM 命令的反馈文字看不见，又会在资源缺失的半成品元素上越界崩」；`PacketSwitch.cpp:749` 仍 `emplace<WEEK_EVENT_MESSAGE, WeekEventMessageHandler>()` | ✅ 有意裁决销案，零改动（强接会崩） |
| 1-1 WEDDING_CEREMONY_END 321 | 仅 deactivate，需补最小语义 | `WaveAShellHandlers.cpp:507-513` LOG + `ui->deactivate()`，v83 结束语义正确 | ✅ 语义正确销案 |
| 1-1 MONSTER_CARNIVAL_LEAVE 295 | 仅 deactivate | `WaveAShellHandlers.cpp:710-714` `ui->deactivate()`，离开语义正确 | ✅ 语义正确销案 |
| 1-1 MTS_OPERATION 348/347 | 仅 LOG/return | `WaveAShellHandlers.cpp:3551-3559` 按 mode 组 tip + `emplace<UIMTS>()` + `set_status`；`:3561-3578` 读 nx/mp + `set_cash` + `set_status` | ✅ 已有处理销案 |
| 1-1 FIELD_EFFECT 138 | 仅 LOG | `TestingHandlers.cpp:184-186` 未知 mode 的 `skip_byte()` 兜底 + `Unknown value` LOG；已知 mode 在 `:178-180` 有 `set_message` 分支 | ✅ 兜底非死代码销案 |
| 1-3 死注册 4/5 | s9 不下发，需标注/收敛 | 本轮未逐字核准 `LoginHandlers.cpp:459,476` / `PacketSwitch.cpp:58-60,436-437` | ⏳ 保留待续（不伪称已核） |
| 1-5 IME 光标 | `Textfield` 中文输入链路 | 本轮未逐字核准 `IO/Components/Textfield` | ⏳ 保留待续（不伪称已核） |

## 2. 真实缺陷立项（本轮动刀处，同构于 round34）

live `IO/UITypes/UICharSelect.cpp:761-920 send_key`：构造期
`:148-156` 已夹 `selected_character`、`button_pressed(BtSelect):1016-1018` 与
`BtDelete:1081-1083` 均有越界守卫，但**网格导航分支 `:828-894` 缺前置守卫**——
与 `UIWorldSelect::send_key`（round34 `72f1a6ce` 修的重连越界/0 频道除零/越段）
是同一类「入口守卫不对称」病。

- **守卫 A · 空列表**：`characters_count<=0`（新账号 / 删光角色 / 重连后列表未就绪）
  时仍进入网格：`:886` RIGHT 分支 `if (selected_index != characters_count - 1)`
  即 `0 != -1` 成立 → `button_pressed(Buttons::BtPageR)`，对空列表做无意义翻页
  （`page_count` 由 `slots` 算非 0，`BtPageR:1152-1168` 会改 `selected_page`）。
- **守卫 B · 越界残留**：`remove_character():955-987` 在 `selected_character >=
  characters_count` 时调 `select_last_slot():1322-1335`，但后者只在当前页找到
  `index < characters_count` 才 `button_pressed`；若 `selected_page` 指向已空的页，
  一次都不按 → `selected_character` 保持残留越界绝对索引。随后 `:850` UP 分支
  `button_pressed(next_index - COLUMNS + CHARACTER_SLOT0)` 减法落到
  `CHARACTER_SLOT0` 之前的按钮段（实测 `8 = BtPreview`），且 `std::div(sel, columns)`
  的 quot 越出 `rows`。

### 自我批判（事实驱动，不硬撑开工诊断）

开工 `[PUA-DIAGNOSIS]` 曾写「空列表 UP 算出 `6 = BtResetPIC`、越界误触发
`6 = BtResetPIC`」。首版 harness 的修复前/修复后对照把它证伪：
- 空列表 UP/DOWN 修复前走 `next_index == selected_character` 的 `return` 分支，
  **不 misfire**；真实症状是 RIGHT 的 `BtPageR` 误翻页。
- 越界 sel=5/count=1 的误触发按钮实测 `8 = BtPreview`，不是 6。
- `:842 if (columns > 0)` 是既有除零守卫，本轮 A/B 补的是 `button_pressed`
  误触发/越段，不是补除零。

机制判断（越界 sel → 减法落到 `CHARACTER_SLOT0` 之前、列表清空后残留索引）
成立，但**具体按钮数字与空列表症状写错**。已按 harness 实证重写诊断与
验收标尺：守卫 A 用「空列表 presses 数」验收，守卫 B 用「越界 misfire 数」
验收，两条守卫各给各的证据，不混着吹。

## 3. 最小修复（2 处守卫 / 13 行插入 / 1 文件）

`IO/UITypes/UICharSelect.cpp:830` 网格分支起始处插入：

```cpp
// 空角色列表（新账号 / 删光角色 / 重连后列表未就绪）：网格导航会把
// next_index - COLUMNS + CHARACTER_SLOT0 算到 CHARACTER_SLOT0 之前，
// 误触发 BtResetPIC 等无关按钮；与 UIWorldSelect::send_key 的 0 频道
// 守卫同口径，此处不进入网格导航。
if (characters_count <= 0)
    return;

// 删除角色后 selected_character 可能残留越界的绝对索引（remove_character
// 只在该值 < characters_count 时刷新选中态）。收敛到当前角色集合，
// 否则 std::div 的 quot 越出 rows，导航落到不存在的槽段。
if (selected_character >= static_cast<uint8_t>(characters_count))
    selected_character = static_cast<uint8_t>(characters_count - 1);
```

`git diff --stat` = `IO/UITypes/UICharSelect.cpp | 13 +++++++++++++`（1 file
changed, 13 insertions(+)）。不改协议、不改包体字节、不碰 WZ/IMG/XML、不动其它文件。

## 4. 自测证据

`tools/test_charselect_guard.py`（本轮新建）：
- §1 静态核验 2 守卫逐字在盘 → 2×`[OK]`。
- §2 算法复刻 `send_key` 网格前置分支 + 4 方向 buttonid，10 边界 case 分
  A（空列表 2 例）/ B（sel 越界 3 例）/ N（常规回归 5 例）三组，每组都跑
  修复前（`guarded=False`）与修复后（`guarded=True`）。
- §3 两条守卫**分别**闭环，不合并计数：
  - 守卫 A：空列表 `presses 2 → 0`（修复前 2 次 `BtPageR` 误翻页）。
  - 守卫 B：越界 `misfire 2 → 0`（修复前 2 处 `8=BtPreview` 误触发）。
  - 回归：常规 case `misfire 0 → 0`，守卫未破坏正常导航。

实跑输出：

```
[OK] A_empty_list_early_return (count=1)
[OK] B_selected_character_clamp (count=1)
[OK] C01[A] ... before(press=1,mis=[]) -> after(no_grid,press=0,mis=[])
[OK] C03[B] ... before(press=4,mis=[('UP', 8)]) -> after(grid,press=0,mis=[])
[守卫 A] 空列表 presses: 修复前=2 -> 修复后=0
[守卫 B] 越界 sel misfire: 修复前=2 -> 修复后=0
[回归] 常规 case misfire: 修复前=0 -> 修复后=0
PASS: 2 guards + 10 boundary cases (A:press 2->0, B:misfire 2->0)
TEST_RC=0
```

构建门禁实证（后台 job `bash-a2db4bda23af42e6` exit 0，真实执行）：
`cmd.exe /d /c "tools\build_smoke.bat"`（MSYS2_ARG_CONV_EXCL 规避 MSYS 假执行）
→ 逐字输出：

```
BEFORE_MTIME 2026-11-09 22:28:34   BEFORE_SIZE 9600
=== BUILD ===
GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)
GATE OK
EXITCODE=0
=== AFTER ===
AFTER_MTIME 2026-11-09 22:58:42    AFTER_SIZE 5644
ERROR_LINES 0
TAIL ... MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe
     已复制 1 个文件 ×3 / 复制了 0 个文件
=== GATE_LINES ===
GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e) / GATE OK / EXITCODE=0
```

`build_log.txt` mtime `22:28:34 → 22:58:42`（真重编译，size 9600→5644），
零 `error|fatal|失败` 行；tail 仅既有 `LNK4099 vc120.pdb` warning。占用按用户
策略 `taskkill /F /IM MapleStory.exe /T` 先行：3 个 32K 空壳
（34476 / 6108 / 56016）`Access is denied`（不占资源、不阻断构建），可终止的
50412 已于 round34 基线轮终止。

## 5. 不在本轮范围（如实披露，不伪称已修）

- `selected_character` 是**绝对**角色索引，但网格导航把它当**页内 slot** 用
  （`:878` / `:890` 与 `selected_page*PAGESIZE` 混算）。这是既有设计/另一处
  缺陷，需真机多页角色键盘导航取证后另立 Plan，本轮守卫只解决「空列表误翻页」
  与「越界残留误触发」，不扩大改动面。
- 1-3 死注册、1-5 IME 光标本轮未逐字核准，保留 `⏳ 待续`。
- Wave2（IMG 内容层）/ Wave3（L-B 涉包体结构、需用户裁决 D1–D14）按计划挂起。

## 6. 真机联测 backlog（交用户一起测，写入 `pending-live-tests` §14）

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 14-1 空列表按键 | 新账号（0 角色）进选角页，按 上/下/左/右/TAB/回车 | 不翻页、不弹无关窗、无崩溃 | 需登录服 + 0 角色账号，harness 只覆盖算法 |
| 14-2 删光残留 | 有角色账号删到只剩 1 个，删完瞬间按方向键 | 不误触发 BtPreview / 无越段跳转 | 需真机时序 + 删除链路 |
| 14-3 多页导航回归 | 4 / 7 / 15 角色翻页后上下左右 + 回车进游戏 | 导航/选中/进世界与修复前一致 | 需登录服在跑 + 冷启动 + 手工按键 |
| 14-4 重连竞态 | 选角页触发重连、列表未就绪瞬间按键 | 无崩溃、按键被守卫安全吞掉 | 需掐准 SERVERLIST 刷新窗口 |

## 7. 提交 scope（精确 add，零署名）

- M `IO/UITypes/UICharSelect.cpp`（13 行守卫）
- A `tools/test_charselect_guard.py`（自测 harness）
- A `docs/features/development-and-fix-plan-2026-11/notes/wave1-charselect-recheck-20261109.md`（本记录）
- M `docs/features/development-and-fix-plan-2026-11/README.md`（§4 Wave1 表销案 + 新增 1-6）
- M `docs/features/pending-live-tests/README.md`（新增 §14）

`.gitignore` 混合编码（`ops_read_files` 报 `ERR_INVALID_ENCODING`，byte 0xb1
@ pos 2415），沿用 round34 结论：不做整文件重写，改精确 `git add`；
`__pycache__`/`*.pyc`/`build_log.txt` 等已被既有 `.gitignore` 覆盖。
提交模板按 `git-commit-standard` skill：`<Emoji> <Type>(<Scope>): <标题> (#<IssueID>)`
+ 结构化变更列表 + 零署名（禁 `Powered by`/`Co-authored-by`/`Generated by`）。

## 8. 性质标注

本记录是「开发/修复阶段现场复核 + 最小修复 + 自测 + 提交」证据，非校准文档、
非实现 Plan 的替代品。校准十一面（login→quest）仍只读闭环；本轮按用户 m04292
授权进入当前客户端源码修复。全程不建 095 引用、不改协议、不碰 WZ/IMG/XML。
