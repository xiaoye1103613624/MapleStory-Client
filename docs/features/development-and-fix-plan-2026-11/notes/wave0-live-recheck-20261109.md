# Wave0 现场复核销案记录（round34 / 2026-11-09）

> 性质：只读复核 + 一处真实缺陷最小修复的证据链留档。用户 m04292 授权
> 「可以自测的自己测试，遇到客户端占用直接关闭，功能测试完成就提交代码，
> 不能自测的记录下来后面我一起测试」。本文件随修复一并原子提交。

## 0. 结论（先给结果）

- `feature-gap-inventory` §16 的 **#2 / #9 / #8 三项 P0/功能缺口是 2026-11-04 旧账**，
  live 源码（11-09）已闭环。**不拿旧账改已修代码 = 不返工**，三项销案、零改动。
- 现场逐字复核 `UIWorldSelect::send_key` 时**新发现 3 类真实 UB**（非旧账、live 仍在）：
  重连越界读 / `std::div(x,0)` 除零 / 频道数缩水导航越段 + SoftKey 延迟回调无守卫。
  → 立项 Wave0 0-5，已做最小修复（4 守卫）、自测、构建门禁，按授权原子提交。
- 证据优先级（`feature-gap` §2.3）：`opcode-matrix.json` > 源码 `.dispatch()/emplace`
  > 现场重算 > 真机记录 > 旧账。旧账与 live 冲突时以后者为准。

## 1. 旧账 ↔ live 源码逐字对照（三项销案）

| 旧账项 | 旧账主张 | live 源码逐字证据 | 结论 |
| --- | --- | --- | --- |
| #2 右侧快捷 8 格点击 | 「仅绘制、无 hit→send_key（P0 硬缺陷）」 | `IO/UITypes/UIStatusBar.cpp:2174-2195 classic_qs_slot_at()`：`if (!classic_mode \|\| !quickslot_active \|\| !classic_quickslot) return -1;` + `Rectangle cell(...QS_SLOT...)` + `cell.contains(cursor_position)` 命中；`:2197-2213 activate_classic_qs_slot()`：`:2203 keyboard.get_quickslot_key(slot)` → `:2206 keyboard.get_maple_mapping(maple_key)` → **`:2212 Stage::get().send_key(mapping.type, mapping.action, true)`**；`:2215-2231 send_cursor()` 点击派发。`UIStatusBar.h:183 CLASSIC_QS_SLOTS = 26`（8/26/32 三布局）。 | ✅ 全链已实现且不止 SKILL/ITEM；销案，零改动 |
| #9 单频道 `channel_count<2` 跳过 | 「`IO/UITypes/UIWorldSelect.cpp:697-698` 有跳过分支（P0 代码缺陷）」 | live `:689 if (world_selected && !channels_bounds.contains(cursorpos) && !inside_goworld)` **无 `channel_count<2` 跳过**；`:943-952` `if (world.channel_count < world.channel_capacities.size())` fixup 兜底 + `LOG(LOG_INFO, "[WorldSelect] fixup channel_count id=")`；`:1039-1049` `limit` 与 `CHANNEL_BUTTON_COUNT`/`capacities` 三者取最小；`:1210` 注释「draw_world 仅跳过 channel_count==0」；`draw_world():918-997` 无该跳过。 | ✅ 分支不存在、有兜底；销案，零改动 |
| #8 TemporaryStatView 缺失 | 「需新增/补齐 UI 类 + `UIElement::Type` 绑定」 | live `IO/UITypes/UITemporaryStatView.h:1-49`（TYPE/`Row`/`rebuild_rows`/`remain_by_skill`）+ `IO/UI.cpp:39 #include` + `:456-463` SoftKey D 开关 + `IO/UIStateGame.cpp:93 emplace<UITemporaryStatView>()` + `MapleStory.vcxproj:341,698` 收录；git `412f36ea feat(ui): TemporaryStatView 中文标题与半透明明细底`。 | ✅ 类/注册/构建全入库；销案，零改动 |

补充：`git diff --stat HEAD -- IO/UITypes/UIStatusBar.cpp IO/UITypes/UIWorldSelect.cpp
IO/UITypes/UITemporaryStatView.cpp Net/PacketSwitch.cpp` 复核前为空——这 4 文件与已提交
HEAD 一致，均为既有提交态，非本会话未提交改动。Wave1 的 `WEEK_EVENT_MESSAGE(77)` 走
`UIEvent` 是**有意裁决不做**（`MessagingHandlers.cpp:504-510` 注释：强行开窗会资源缺失越界
崩 + GM 反馈不可见），`RECALCULATE_STATS/FORCED_STAT_RESET` 撞值已在 `PacketSwitch.cpp:735-740`
注释注销——均不盲改。

## 2. 现场新发现的真实缺陷（`UIWorldSelect::send_key :757-911`，live 仍在）

`draw():591` 有 `worldid < worlds.size()` 守卫，`UIWorldSelect.h:66-69 get_selected_world()`
也返回 `worldid < worlds.size() ? &worlds[worldid] : nullptr`，但 `send_key` 的
`world_selected` 分支**不对称**：

- **A 越界读**：`if (world_selected) { World selectedWorld = worlds[worldid]; ... }` 只受
  `world_selected` 约束。`clear_worlds():1022-1027` 只 `worlds.clear(); worldcount = 0;
  world_list_finalized = false;`，**不复位 `world_selected`** → 重连/新一轮 SERVERLIST 到达但
  世界牌尚未重建时按键即 `worlds[worldid]` 越界读。
- **B 除零 UB**：`:764-768` `uint8_t columns = std::min(channel_total, COLUMNS); ...
  div_t div = std::div(selected_channel, columns);` → 服务端下发 `channel_total==0` 畸形
  world 时 `columns==0` → `std::div(x, 0)` SIGFPE。
- **C 导航越段**：`channelid >= channel_total`（重连后频道数变少）时 `div.quot` 越出
  `rows`，导航落到非法按钮段。
- **D 延迟回调**：`:830` 「Channel #」SoftKey 回调 `World selectedWorld = worlds[worldid];`
  无守卫，弹出到确认间可能已 `clear_worlds()`。

`:841-908` else（未选世界）分支已用 `navigable_world_count()` 守卫，故只修 `world_selected`
分支即可，改动面收敛到一处函数。

## 3. 最小修复（`IO/UITypes/UIWorldSelect.cpp`，29 insertions / 1 deletion）

| 守卫 | 位置 | 逐字 |
| --- | --- | --- |
| A | `:759` 主路径 | `if (worldid >= worlds.size()) return;`（与 `draw()` 同口径） |
| B | `:764` 0 频道 | `if (channel_total == 0) { if (escape) { world_selected = false; channel_slide = 1.0f; panel_expand = 1.0f; clear_selected_world(); } return; }`（仅 ESC 逃生回世界列表） |
| C | `:768` 频道收敛 | `if (selected_channel >= channel_total) selected_channel = 0;` |
| D | `:855` SoftKey 回调 | `if (worldid >= worlds.size()) return; const World& selectedWorld = worlds[worldid];`（顺带去拷贝） |

> 注：tab 缩进 + 中文注释的 old_string 用 edit 工具 4 次未命中（含一次路径 typo
> `UITYpes`），改用 `ops_bash` python 逐字替换 `OLD_FOUND 1 WROTE_OK`。教训：C++ tab
> 缩进精确替换优先用脚本字节级匹配，不靠可视化 old_string。

## 4. 自测证据（可自测项，全部实跑）

| 项 | 命令 | 结果 |
| --- | --- | --- |
| 守卫 + 边界 | `python tools/test_worldselect_guard.py` | 4 guards `[OK]` + 10 boundary cases `[OK]` + `PASS: 4 guards + 10 boundary cases` + `TEST_RC=0`（未选世界 / worlds 清空 / worldid 越界 / 0 频道方向键 / 0 频道 ESC / channelid 越界 / 0xFF 哨兵 / 单频道 / 30 频道末位 / 7 频道中位） |
| 协议门禁 | `python tools/gen_opcode_matrix.py --check`（build_smoke.bat 内） | `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)` |
| 构建 | `MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c "tools\build_smoke.bat"` | `GATE OK` + `EXITCODE=0`；`build_log.txt` mtime `22:10:57`→`22:28:34`、197B→9600B（真重编译）、tail `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe` + `已完成代码的生成` |
| 日志 | `grep -iE "error\|错误\|失败\|fatal"` build_log.txt | 零 error/fatal；仅既有 warning（C4005/C4100/C4244/C4458/LNK4098/LNK4099），非本改动引入 |
| 占用 | `taskkill /F /IM MapleStory.exe /T` | 50412 已终止；34476/6108/56016 `Access is denied`（32K 空壳，不占目标资源、不影响构建）。用户 m04292「遇客户端占用直接关闭」已执行 |

> 第一次 `cmd.exe //c "tools\build_smoke.bat"` 在 MSYS 下只回显 Windows banner =
> **假执行**（build_log mtime 未变）。改 `MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c` 后
> 才拿到真实门禁输出并留证。教训：构建门禁必须核验产物 mtime/字节数，不能只看退出码。

## 5. `.gitignore` 提交边界决策

- `.gitignore` 为混合编码（UTF-8 中文注释 + 单个非 UTF-8/非 GBK 残留字节 `0xb1`
  @ pos 2415），`ops_read_files` 报 `ERR_INVALID_ENCODING`，**不做有损文本改写**。
- 本轮新增产物 `tools/test_worldselect_guard.py`、本 notes、两份 README 修订**均应入库**；
  构建产物 `build_log.txt`（`/build_log.txt`）、`*.log`/`*.obj`/`*.tlog`/`*.aps`/`*.nx`/`*.wz`
  已被现有 `.gitignore` 覆盖。
- 因此**不改 `.gitignore`**，改用精确 `git add <具体文件列表>` 保证原子提交边界，
  `git show --stat` 复核提交内容。满足 `git-commit-standard` 「提交前维护 .gitignore」的实质。

## 6. 真机联测 backlog（不能自测，交用户一起测）

见 `docs/features/pending-live-tests/README.md` §13「round34 世界选择键盘导航边界」：
重连瞬间按键竞态 / 0 频道畸形 world / 频道数缩水 / Channel # SoftKey 延迟回调 / 常规回归，
共 5 项。自动脚本只能覆盖静态守卫 + 算法复刻，时序与 s9 畸形包须真机联测。

## 7. 提交 scope

```
IO/UITypes/UIWorldSelect.cpp                                              (M)
tools/test_worldselect_guard.py                                            (A)
docs/features/development-and-fix-plan-2026-11/README.md                    (A, 计划落库 + Wave0 销案/0-5 立项)
docs/features/development-and-fix-plan-2026-11/notes/wave0-live-recheck-20261109.md (A)
docs/features/pending-live-tests/README.md                                 (M, §13 backlog)
```
`🐛 fix(ui): 世界选择键盘导航边界守卫（重连/0频道/越界）(#devfix-worldselect-guard)`
零署名、单功能一次提交。校准 evidence / 截图 / `*.obj` / `_*.log` / `build_log.txt`
均不进本次提交。083/v83 only 禁 095；校准≠改回原生；s9 优先；数值≠语义。
