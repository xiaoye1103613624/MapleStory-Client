# 当前客户端开发 / 修复大致计划（2026-11-09）

> 触发：用户 m04292「如果 Angel(13339) 分析完，就可以准备当前客户端的开发和修复任务，要把现有功能补充、错误逻辑修复、各 UI 拼接、动画、交互也一并修复；涉及到的地方先给我出一份大致的开发计划；可以自测的自己测试，遇到客户端占用直接关闭，功能测试完成就提交代码，不能自测的记录下来后面我一起测试。」

---

## 0. 本轮性质与一句话结论

- **本轮交付 = 这份计划本身**。用户明确「先给我出一份大致的开发计划」；本轮只读产出文档，源码零改动、不建提交。实现从下一轮（用户裁决 §3 决策闸门后）按 §4 Wave 顺序开工，每批「自测通过 → 原子提交」，不能自测的进 §6 人工联测 backlog。
- **结论**：Angel 083/v83 IDB 十一面校准已全闭环（11 份 README + 各 98 条逐条对照 + `GATE OK`），证据层不再是瓶颈；`feature-gap-inventory` §9.5 权威重算已钉死「148/148 c2s 全覆盖、305 条 s2c 注册、917 处 dispatch」——**当前不缺窗口文件、不缺出站封包，真要动的是五层：P0 硬缺陷 / 收包空实现 / UI 拼接与样式 / 真实渲染与动画 / 玩法深度**，协议「补发包」只在 §9.5.3 的少数「完全无」项，且多数需双端立项或用户裁决。
- **抓手**：把工作分成「可立即自测闭环」与「待裁决/待资源/待真机」两层，不放宽验收标尺，不把 083 语义盲改回原生把 s9 联机打断。

---

## 1. 证据基线（全部实读，路径逐字）

| 锚点 | 路径 | 用途 |
| --- | --- | --- |
| 十一面校准 | `docs/features/*angel-calibration-2026-11/README.md`（11 份） | 各面 §3 98 条判定 / §5 四态 / §6 D1–D14 / §7 P0–P3 |
| Angel 083 定稿 | `docs/features/ida-v83-angel-instance/README.md` | 3122 修饰名/361 类/2612 方法；同 v83 二进制不同符号状态 |
| 缺口台账 | `docs/features/feature-gap-inventory-2026-11/README.md`（680 行） | §9.5 协议权威重算 / §14 明确不做 / §15 全局阻塞 / §16 TOP20 / §17 Angel EA 联动 |
| 真机待测 | `docs/features/pending-live-tests/README.md`（475 行） | 汇总 36 项、四类全局阻塞 A/A′/B/C/D、已闭环排除项 |
| 协议权威矩阵 | `docs/features/s9-protocol-align/protocol/opcode-matrix.json` | `s9_git_head=3d5ccc6f8e`，唯一权威协议读数 |
| 构建门禁 | `tools/build_smoke.bat` | Release/x64 + `gen_opcode_matrix.py --check`；OutDir `E:\MXD\BeiDou-Client-xy\MapleStory.exe` |
| 协议门禁 | `tools/gen_opcode_matrix.py --check` | 实测 `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)` |
| 握手烟测 | `tools/test_handshake.ps1` | `127.0.0.1:8484` 读 16B `HANDSHAKE bytes=... hex=...` |
| 提交规范 | skill `git-commit-standard` | Gitmoji + 云效分类 + `(#IssueID)` + 结构化详情 + 零署名 + 提交前维护 `.gitignore` |
| 仓库规范 | skill `xiaoye-maple-story-dev` | 证据→分析→抉择→一次性 Plan→实现；最小改动；原子提交；REA→IDA→orange-wz(IMG)；UI 位图走 Photoshop MCP；DB 只读 |
| IMG 门禁 | `AGENTS.md`（ACTIVE/MANDATORY） | WZ/IMG/XML 一律先 `mcp__orange_wz__*`（`custom-orange_wz` @ `http://127.0.0.1:10012/mcp`），仅 `IMG 解析失败`/`Index N out of bounds`/GBK 错误文本才 `.nx` 回退并留原始报错 + key 规格 |
| 源码锚点 | `Net/OutPacket.h`、`Net/PacketSwitch.cpp`、`IO/UITypes/UIWorldSelect.cpp`、`IO/UITypes/UIStatusBar.cpp`、`Data/MobDropData.cpp` | §4 各功能点定位 |

**当前运行时读数**：`Angel-083 13339 REACHABLE` / `BeiDou 13337 REACHABLE` / `orange-wz 10012 REACHABLE`；`custom-orange_wz` 已连接 29 工具；分支 `feature/dev_0.0.1`；`git status --short` 有 307 项未跟踪（校准 evidence / 截图 / 构建产物 / 历史 `*.obj`、`_*.log`）——**提交时严防把这批脏文件误纳入功能提交**。

---

## 2. 执行纪律（改源码阶段全程生效）

1. **083/v83 唯一基准，禁引 095**：`Maplestory95.i64` / `docs/features/port-095/**` / `ExpeditionIntermediary` / 韩系 `Academy` / `ijl15.dll` 扩展一律不作依据（§14.2）。
2. **校准 ≠ 改回原生 / s9 协议优先**：083 与 s9 是两套命名空间，**数值重合 ≠ 语义重合**。典型：Quest 面 083 `COutPacket(107)` 按 action 变长且带玩家 `x/y`、当前端 s9 通道定长只带 `npcid`（`Net/Packets/QuestPackets.h:10-23`）；map 面 083 `135/183` 与 s9 `WHISPER=135`/`REMOVE_DRAGON=183` 撞号不同义；npc 面 c2s `197` 与 s2c `260` 同名不同号。**不得仅凭数值判等，不得盲改协议**。
3. **IMG 顺序固定**：`orange-wz MCP →（仅内容层解析异常）→ .nx dump + 运行时 probe`。第十/十一面已实证「结构层 WORKS（目录枚举出真节点）、内容层 FAILS（`IMG 解析失败`）」；F 样式维 12 项诚实标 ⏳，不伪绿。key 用 `tools/_gms_key.b64`（65536B `key_gms`，`iv_b64 TSPHKw==`）。
4. **验证门禁**：每批改动后 `python tools/gen_opcode_matrix.py --check`（GATE OK）+ `tools\build_smoke.bat`（`EXITCODE=0`）+ 本批自测项证据；改过协议必跑握手烟测。**没有输出证据的完成不算完成**。
5. **最小改动 / 原子提交**：单功能一次提交，按 `git-commit-standard` 模板；提交前查 `.gitignore` 覆盖 `build_log.txt`/`*.obj`/`_*.log`；提交信息零署名。
6. **客户端占用**：实现/部署/自测阶段若 `MapleStory.exe` 占用目标 exe/资源，按 m04292 直接 `taskkill /F /IM MapleStory.exe`（实测在跑 4 个：`34476/6108/56016/50412`）；`GameViewer*`、`UDClientService`、`GameBarPresenceWriter` 等非目标进程避开。
7. **DB 默认只读**：`pending-live-tests` 里造角色/公会/道具前置需 DML，未经单独明确同意不写库，只进 §6 backlog。
8. **UI 位图/动画贴图**走 Photoshop MCP + 原生 UI 规范；截图库 `C:\Users\11036\Desktop\正确客户端截图\` 仅作体感输入。

---

## 3. 决策闸门（必须先分两层，否则会断联机）

十一面校准每面 §6 都留了 D1–D14 决策点，**推荐默认全为「保持 s9 优先、不补 083 发包、不改协议、保留资源降级、不建提交」**，且仍待用户裁决。故本计划把工作分两层：

| 层 | 范围 | 开工条件 | 风险 |
| --- | --- | --- | --- |
| **L-A 可立即开工** | 纯本地逻辑 / UI 交互 / 收包空实现 / 死代码 / 样式拼接，不触碰 c2s/s2c 包体字节语义 | 本计划确认后即可 | 低，构建+自测可闭环 |
| **L-B 待裁决/双端** | 涉及 083↔s9 包体结构差异、新增 c2s 枚举、s2c 差集 handler、服务端闭环 | 用户逐条裁决 D1–D14 或明示「按 s9 保持」 | 高，盲改断 s9 联机 |

**L-B 清单（不动，等裁决）**：Quest 107 带 x/y、场地伤害族 `0xBF/0xC0/0xC1/0xC2/0xD4` 完全无、`0x04/0x09/0x38/0x1C`、`PARTY_SEARCH_REGISTER(220)`、`MTS_OPERATION(0xFD)`、`CUSTOM_PACKET(0x3713)` 撞值、Title 称号窗、扩展装备栏、每日签到双端、`trade-house-083`、`0x3730→0x3740` 规划。§14.1/§14.2/§14.3 已裁「明确不做」的项同样不重新立项。

> 用户 m04292 说「功能补充、错误逻辑修复也一并修复」——**「补充」限定在 L-A 的可本地闭环项**；L-B 若要动，需用户对本节逐条拍板。这是本计划唯一需要用户回应的地方，其余按 Wave 自动推进。

---

## 4. 分批开发计划（五聚类标签：功/错/UI/动/交）

标签：`功`=功能补充 `错`=错误逻辑 `UI`=UI 拼接 `动`=动画/真渲染 `交`=交互。每项给「证据锚点 / 改动范围 / 自测方式 / 提交 scope」。

### Wave 0 · 立即可自测（L-A，纯本地，不碰协议）

> **round34 现场复核修订（2026-11-09）**：0-1/0-2/0-3 系 `feature-gap` §16 的
> 2026-11-04 旧账，live 源码 11-09 已闭环 → **销案、零改动**；现场逐字复核反而发现
> `UIWorldSelect::send_key` 3 类真实 UB → 立项 0-5 并已修复/自测/构建。逐字证据见
> `notes/wave0-live-recheck-20261109.md`。证据优先级 §2.3：源码 > 旧账。

| # | 标签 | 功能点 | 证据锚点 | 改动范围 | 自测方式 | 提交 scope / 状态 |
| --- | --- | --- | --- | --- | --- | --- |
| 0-1 | 交 | ~~右侧快捷 8 格点击：仅绘制、无 hit~~ | `UIStatusBar.cpp:2174-2231 classic_qs_slot_at/activate_classic_qs_slot/send_cursor` → `:2212 Stage::get().send_key(...)` | 无（已实现） | 构建后本地点击复核 | ✅ 销案（旧账 #2 已闭环） |
| 0-2 | 错 | ~~单频道被 `channel_count<2` 跳过~~ | `UIWorldSelect.cpp:689` 无该分支 + `:943-952` fixup 兜底 + `:1210` 注释「仅跳过 ==0」 | 无（分支不存在） | 单频道 world 链路复核 | ✅ 销案（旧账 #9 已闭环） |
| 0-3 | 功 | ~~`TemporaryStatView` 需新增/补齐~~ | `UITemporaryStatView.h:1-49` + `IO/UI.cpp:39,456-463` + `IO/UIStateGame.cpp:93` + `.vcxproj:341,698`（git `412f36ea`） | 无（类/注册/构建齐全） | 开窗渲染复核 | ✅ 销案（旧账 #8 已闭环） |
| 0-4 | 错 | UI 拼接既有增量复核：tab_plate 借 `UiAssetResolve::window("Item")["New"]`、`set_macro` 显式 `mex->deactivate()`、`send_spup` 资源缺失降级 | `UISkillBook.cpp:142-250,1098-1152,1249`；skill 面 r05 | 复核为主，缺则补齐 | 技能窗展开/收起 + 点「+」升级 | `#devfix-uiglue-review`（待续） |
| **0-5** | 错 | **世界选择键盘导航边界守卫**：重连越界读 `worlds[worldid]` / `channel_total==0` → `std::div(x,0)` 除零 UB / 频道缩水导航越段 / Channel # SoftKey 延迟回调无守卫 | live `UIWorldSelect.cpp:757-911`（`clear_worlds():1022-1027` 不复位 `world_selected`；`get_selected_world()` 有守卫反证 send_key 不对称）；`notes/wave0-live-recheck-20261109.md` §2 | `UIWorldSelect.cpp` 4 处守卫（29+/1-） | `python tools/test_worldselect_guard.py`（4 守卫 + 10 边界 PASS）+ `build_smoke.bat` GATE OK + EXITCODE=0 | ✅ `#devfix-worldselect-guard`（本轮已提交） |

### Wave 1 · 可自测但需构建/协议门禁（L-A，收包与死代码，不改包体字节）

> **round35 现场复核修订（2026-11-09）**：1-2 / 1-4 是 `PacketSwitch.cpp:735-740`
> 与 `MessagingHandlers.cpp:504-510` 写明理由的**有意裁决**（强接会崩），1-1 的
> deactivate / LOG 多为 v83 正确语义 → 销案、零改动；同构排查反而揪出
> `UICharSelect::send_key` 空列表误翻页 / 越界残留误触发 → 立项 1-6 并已修复/
> 自测/构建。逐字证据见 `notes/wave1-charselect-recheck-20261109.md`。
> 1-3 死注册 / 1-5 IME 光标本轮未逐字核准，保留 ⏳ 待续，不伪称已核。

| # | 标签 | 功能点 | 证据锚点 | 改动范围 | 自测方式 | 提交 scope |
| --- | --- | --- | --- | --- | --- | --- |
| 1-1 | 错 | ~~5 个收包空实现需补语义~~ | `WaveAShellHandlers.cpp:507-513,710-714,3551-3578` deactivate/`set_status` 即 v83 结束/离开语义；`TestingHandlers.cpp:184-186` 未知 mode 兜底 | 无（语义正确） | 灌回包看 UI 状态 | ✅ 销案（旧账 #9 已闭环） |
| 1-2 | 错 | ~~`RECALCULATE_STATS`(35) 撞值死代码~~ | `Net/PacketSwitch.cpp:735-740` 已注销该 emplace 并写明 s9 只发 `FORCED_STAT_RESET`、被顶掉属性不回弹；`PlayerHandlers.cpp:275` handler 仍在 | 无（有意裁决） | `gen_opcode_matrix.py --check` | ✅ 销案（有意裁决，强接会破坏属性回弹） |
| 1-3 | 错 | `GENDER_DONE(4)`/`CONFIRM_EULA_RESULT(5)` 事实死注册（s9 不下发） | `LoginHandlers.cpp:459,476`；`PacketSwitch.cpp:58-60,436-437` | 标注/收敛死注册 | 登录链烟测无回归 | ⏳ `#devfix-dead-register`（round35 未逐字核准，待续） |
| 1-4 | 错 | ~~`WEEK_EVENT_MESSAGE(77)` 需接 `UIEvent` 数据通路~~ | `MessagingHandlers.cpp:501-510` 已改走 yellow tip；注释「强开窗让 GM 反馈看不见且 `UIEvent.cpp:123` 资源缺失越界崩」 | 无（有意裁决） | 灌 77 回包看 yellow tip | ✅ 销案（强接会崩） |
| 1-5 | 交 | 中文 IME `Textfield` 光标 + 显示链路（IME 已在 `IO/Window.cpp:901-925` 开启） | `feature-gap` §16 #13 | `IO/Components/Textfield` | 本地输入中文看光标/回显 | ⏳ `#devfix-ime-caret`（round35 未逐字核准，待续） |
| **1-6** | 错 | **UICharSelect 键盘导航边界守卫**：空列表 RIGHT 误翻页 `BtPageR` / 删角色后 `selected_character` 残留越界致 `button_pressed` 落到 `CHARACTER_SLOT0` 之前（实测 `8=BtPreview`） | live `UICharSelect.cpp:828-894`（`remove_character():955-987` + `select_last_slot():1322-1335` 残留越界）；同构 round34 `72f1a6ce`；`notes/wave1-charselect-recheck-20261109.md` §2 | `UICharSelect.cpp` 2 处守卫（13+） | `python tools/test_charselect_guard.py`（2 守卫 + 10 边界 PASS，A:press 2→0 / B:misfire 2→0）+ `build_smoke.bat` GATE OK + EXITCODE=0 | ✅ `#devfix-charselect-guard`（本轮已提交） |

### Wave 2 · 需 orange-wz 内容层 / Photoshop 位图（L-A，UI 拼接·动画·样式，多数当前 ⏳）

| # | 标签 | 功能点 | 证据锚点 | 阻塞 | 恢复条件 |
| --- | --- | --- | --- | --- | --- |
| 2-1 | UI/动 | 各面 F 维 12 项样式降级清单（技能名色 BLACK/EMPEROR、TV 双屏、名牌、hit 框、AskAvatar、QuestIcon、宏按钮等） | 各面 `notes/r03-img-*-fallback.md` | orange-wz 内容层 `IMG 解析失败` | 内容层恢复后按 orange-wz 节点逐像素对齐；否则按 AGENTS.md 合规 `.nx` 回退留证 |
| 2-2 | 动 | `DamageSkin` 运行时资源、宠物亲密/饱食 gage 条、场标→真精灵、MapleTV 双屏轮播 | `feature-gap` §16 #17 / §7 E19-E22 / §14.3；`Gameplay/MapleMap/Npc.cpp:151-198` | 资源层 | Photoshop MCP 出图 + native 动画规范 |
| 2-3 | UI | NPC 对话排版 A11b–A11j（下项/高亮/GetText/GetNumber/AskAvatar/AskBoxText）、玩家列表五页签、UIGuildBBS 多页 | `pending-live-tests` #19/22-25/36；`UINpcTalk.h:35-347` | 样式资源 + 真机 | 先做可离屏渲染的拼接，观感项进 §6 |
| 2-4 | 动 | 隐藏门三态 Start/Continue/Exit、SKILL_EFFECT 起手广播、NPC_ACTION 对话姿态 | `pending-live-tests` #15/16/18；`Portal.h:52-58` | 真机触发 | 代码已接线，动画观感进 §6 |

### Wave 3 · L-B 待裁决 / 双端立项（**本计划不自动开工**，见 §3）

场地伤害族 c2s、Quest 107 x/y、`PARTY_SEARCH_REGISTER(220)`、`MTS_OPERATION(0xFD)`、公会副本 HUD s2c 287/288（需 s9 脚本）、Title 称号窗、扩展装备栏 #38、每日签到双端、怪物图鉴增强 #36、300 级/长伤害数值链路、交易所 `trade-house-083`。逐条对应 `feature-gap` §9.5.3 / §16 #12,#14,#18,#19,#20。

---

## 5. 自测矩阵（可自测项的验收信号）

| 门禁 | 命令 | 通过信号 |
| --- | --- | --- |
| 协议 | `python tools/gen_opcode_matrix.py --check` | `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)` |
| 构建 | `tools\build_smoke.bat` | `GATE OK` + `EXITCODE=0`，日志 `build_log.txt` |
| 握手 | `tools\test_handshake.ps1` | `HANDSHAKE bytes=16 hex=...`（依赖 s9 8484 在跑） |
| 登录链 | 部署后冷启动 | `pending-live-tests` 记录形态：SEND 包序列 → 世界→频道→选角→进图，零崩溃 |
| 交互 | 本地点击/开窗/输入 | 每项 §4 表格「自测方式」列的具体观察点 |
| 回归 | 各改动相邻类 | 改过 UI 拼接跑相邻窗，改过收包跑对应回包，改过协议跑全门禁 |

**部署**：构建产物 OutDir `E:\MXD\BeiDou-Client-xy\MapleStory.exe`；部署前按 §2.6 关占用进程；部署后先握手烟测再功能自测。**每批「GATE OK + build EXITCODE=0 + 本批自测证据」齐了才原子提交**。

---

## 6. 人工联测 backlog（不能自测的，交用户一起测）

- **主源**：`docs/features/pending-live-tests/README.md` 汇总 36 项（#2 USE_RETURN_SCROLL、#4 PersonalShop FM、#6/#12 GuildBBS、#7 SET_HPMPALERT、#8 PICK_ALL_CHAR、#9 HEAL_OVER_TIME、#10 TOUCH_MONSTER_ATTACK、#11 CANCEL_DEBUFF、#13 公会 Boss HUD、#14 便签、#15 SKILL_EFFECT、#16/17 NPC_ACTION/CLICK_GUIDE、#18 隐藏门、#19/22-25 NPC 对话、#29-31 场标/风筝/宝箱、#32 结婚全链、#33 联盟、#34 R28 抓帧、#35 五死键、#36 五页签、MapleTV 双屏/点选/装备对比）。
- **四类全局阻塞**（`feature-gap` §15.1）：A GM 命令 `!`/空格被 `IO/UI.cpp:264-269` 吞；A′ 角色 lv1/job0/ap0、背包空；B 需第二角色/公会/FM/商城额度；C `game_config.use_server_auto_pot=false`；D 并行会话改同一仓库/重建 exe/结束进程。
- **记录格式**：每项写「编号 / 操作 / 期望 / 为何自动测不了 / 需用户动作」，沿用 `pending-live-tests` 既有格式；实现阶段新发现的不能自测项即时追加，不攒到最后。
- **写库前置**：造角色/公会/道具需 DML，按 §2.7 未获单独同意不执行，只列 `notes/db-change-list.md` 由用户逐条确认。

---

## 7. 提交策略（`git-commit-standard`）

```
<Emoji> <Type>(<Scope>): <简短标题> (#<IssueID>)
- <具体变更点 1>（类名/方法名 + 动作）
- <具体变更点 2>（逻辑补充/修正）
- <具体变更点 3>（新增测试/文档）
```

- 分类映射：`feat ✨`（功能补充）/ `fix 🐛`（错误逻辑）/ `test ✅`（自测用例）/ `refactor ♻️`（拼接整理）/ `docs 📝`（仅文档）/ `chore 🔧`（`.gitignore`、工程）。
- `IssueID` 用 §4 表格「提交 scope」列的功能 slug 或云效 Issue；历史 commit 用 `(#s9-protocol-align)` / `(#ui-v83-window-restore)` 同形态。
- **一功能一提交**，提交前维护 `.gitignore`；只 `git add` 本功能相关源码 + 必要文档，**校准 `evidence/`、截图、`*.obj`、`_*.log`、`build_log.txt` 不进功能提交**（当前 307 项脏文件严防误纳）。
- 零署名：无 `Powered by` / `Co-authored-by` / `Generated by`。
- **功能测试通过才提交**；未测项不混进「已完成」提交，留在工作区或注记。

---

## 8. 验收闭环与风险披露

- 本计划验收：`README.md` 落盘 + §1 全部锚点在盘（实测 13/13 OK）+ 11 面校准 README 计数 11 + `GATE OK` + present。
- 实现阶段验收：逐 Wave 见 §5；**不放宽标尺、不伪绿**（第十面 `no_fake_green` 教训：`IMG 解析失败` 文本不得当成功；第八/十面 facts 多态归一不得假闭环）。
- 已知边界：
  - orange-wz 内容层当前 `IMG 解析失败` → Wave 2 的动画/样式多数 ⏳，按 AGENTS.md 合规 `.nx` 回退，不臆造资源。
  - ⑫掉落(Drop) 独立校准面未单列，但掉落物归属/暴怒炸金币在 `feature-gap` §14.3 已裁「非目标」、G10 宝箱/风筝在 `pending-live-tests` 待测，不阻塞开发计划；用户若要补第 12 面可与本计划并行。
  - 全局阻塞 A/A′/B/C/D 决定 §6 大量项只能人工联测，实现阶段不假装已验证。

---

## 9. 复现 / 下一步

```bash
python tools/gen_opcode_matrix.py --check     # GATE OK
tools\build_smoke.bat                          # 实现阶段每批必跑
tools\test_handshake.ps1                       # 部署后握手烟测
```

**下一步**：用户对 §3 决策闸门逐条拍板（或直接采纳「推荐默认：保持 s9 优先」）后，按 Wave 0 → Wave 1 开工；Wave 0-1 每批「改源码 → 自测 → GATE OK + build 0 → 原子提交」；Wave 2 等 orange-wz 内容层 / Photoshop 出图；Wave 3 等裁决；不能自测的实时追加到 `docs/features/pending-live-tests/README.md`。

---

## 10. 性质标注

本文件是**开发/修复计划与决策清单**（校准证据 → 缺口分层 → 一次性 Plan → 分批提交与自测边界），非实现提交、非评分表。本轮全程只读产出文档，未改游戏源码/协议/WZ/IMG/XML，未建 Git 提交。目标 `goal-11388c42` 保持 active；实现工作按 §4 分批续作。
