# 当前客户端开发 / 修复大致计划（2026-11-09）

> 触发：用户 m04292「如果 Angel(13339) 分析完，就可以准备当前客户端的开发和修复任务，要把现有功能补充、错误逻辑修复、各 UI 拼接、动画、交互也一并修复；涉及到的地方先给我出一份大致的开发计划；可以自测的自己测试，遇到客户端占用直接关闭，功能测试完成就提交代码，不能自测的记录下来后面我一起测试。」

---

## 0. 本轮性质与一句话结论

- **本轮交付 = 这份计划本身**。用户明确「先给我出一份大致的开发计划」；本轮只读产出文档，源码零改动、不建提交。实现从下一轮（用户裁决 §3 决策闸门后）按 §4 Wave 顺序开工，每批「自测通过 → 原子提交」，不能自测的进 §6 人工联测 backlog。
- **结论**：Angel 083/v83 IDB 十二面校准已全闭环（12 份 README + 各 98 条逐条对照 + `GATE OK`），证据层不再是瓶颈；`feature-gap-inventory` §9.5 权威重算已钉死「148/148 c2s 全覆盖、305 条 s2c 注册、917 处 dispatch」——**当前不缺窗口文件、不缺出站封包，真要动的是五层：P0 硬缺陷 / 收包空实现 / UI 拼接与样式 / 真实渲染与动画 / 玩法深度**，协议「补发包」只在 §9.5.3 的少数「完全无」项，且多数需双端立项或用户裁决。
- **抓手**：把工作分成「可立即自测闭环」与「待裁决/待资源/待真机」两层，不放宽验收标尺，不把 083 语义盲改回原生把 s9 联机打断。

---

## 1. 证据基线（全部实读，路径逐字）

| 锚点 | 路径 | 用途 |
| --- | --- | --- |
| 十二面校准 | `docs/features/*angel-calibration-2026-11/README.md`（12 份） | 各面 §3 98 条判定 / §5 四态 / §6 D1–D14 / §7 P0–P3 |
| 跨面决策矩阵 | `docs/features/angel-calibration-d-matrix-2026-11/README.md`（477 行） | 168 条 D 横向拉平：K1–K4 四类 / 12 组跨面主题 / §5 五条批量裁决 / §5.1 残差 28 条 |
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

十二面校准每面 §6 都留了 D1–D14 决策点，**推荐默认全为「保持 s9 优先、不补 083 发包、不改协议、保留资源降级、不建提交」**，且仍待用户裁决。
168 条已横向汇成 `docs/features/angel-calibration-d-matrix-2026-11/README.md`：**K1 可直接做 11 条 / K2 保持现状 94 条 / K3 待裁决 39 条 / K4 需人工判读 25 条**；经 §5 五条批量裁决可一次覆盖 140 条，**残差仅 28 条**需逐条读原文。故本计划把工作分两层：

| 层 | 范围 | 开工条件 | 风险 |
| --- | --- | --- | --- |
| **L-A 可立即开工** | 纯本地逻辑 / UI 交互 / 收包空实现 / 死代码 / 样式拼接，不触碰 c2s/s2c 包体字节语义 | 本计划确认后即可 | 低，构建+自测可闭环 |
| **L-B 待裁决/双端** | 涉及 083↔s9 包体结构差异、新增 c2s 枚举、s2c 差集 handler、服务端闭环 | 用户逐条裁决 D1–D14 或明示「按 s9 保持」 | 高，盲改断 s9 联机 |

**L-B 清单（不动，等裁决）**：Quest 107 带 x/y、场地伤害族 `0xBF/0xC0/0xC1/0xC2/0xD4` 完全无、`0x04/0x09/0x38/0x1C`、`PARTY_SEARCH_REGISTER(220)`、`MTS_OPERATION(0xFD)`、`CUSTOM_PACKET(0x3713)` 撞值、Title 称号窗、扩展装备栏、每日签到双端、`trade-house-083`、`0x3730→0x3740` 规划。§14.1/§14.2/§14.3 已裁「明确不做」的项同样不重新立项。

> **2026-11-10 裁决（m04292）**：采纳 §3 推荐默认，**L-B 全部 12 项「保持 s9 优先、不补 083 发包、不改协议、保留资源降级、不建提交」，销案为「无变更」**。含 §3.1 的 `InPacket::available()` 类型修复亦维持现状（协议级，盲改会让 393 处死守卫变活路径，须真机逐面回归后才可动，记 `pending-live-tests` §27-A）。盲改会断 s9 联机，故维持现状，不进入实现。详见 §11.1。

### 3.1 L-B 待裁决追加：`InPacket::available()` 返回 bool && 393 处当字节数比较（round49 附带发现）

`Net/InPacket.h:37 bool available() const;` / `Net/InPacket.cpp:30-33 return length() > 0;`，全仓 393 处写成 `recv.available() >= 4` / `>= 8` / `< 5` / `== 21` / `recv.skip(recv.available() - 5)`。`bool` 提升成 int 只有 0/1，于是 `>= 2/4/8` 恒 false（短包守卫全部是死代码）、`< 5/14/24` 恒 true（长度预检全部误判为「不足」）、`available() - 5` 在 `size_t` 下溢。编译器已在 `MessagingHandlers.cpp:207/338/646/776/777/868/909/962` 报 `C4804`。

分布：`WaveAShellHandlers.cpp` 205、`MapObjectHandlers.cpp` 85、`CashShopHandlers.cpp` 75、`MessagingHandlers.cpp` 8、`TradeHandlers.cpp` 8，其余 `AllianceHandlers` 3 / `InventoryHandlers` 3 / `QuestHandlers` 2 / `TestingHandlers` 2 / `FredrickHandlers` 1 / `PlayerHandlers` 1。

**为何不当场修**：一行类型修复（`size_t available() const { return length(); }`）会让 393 处原本恒假的守卫**同时变成活路径**，等于一次性改变几乎所有 handler 的短包行为——属协议级裁决，按既有立项门槛（确定性可达 + 零协议决策）不满足，必须真机逐面回归后才可动。观察项见 `pending-live-tests` §27-A。

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
>
> **round36 现场复核收尾（2026-11-09）**：1-3「死注册」是 `PacketSwitch.cpp:58-62,
> 434-437` + `LoginHandlers.cpp:454-486` 写明理由的有意裁决（s9 不发、注册仅作
> v83 协议完整性；handler 带 2026-11-03 实机残留窗修复）→ 销案；1-5 IME 主体
> （`set_accept_unicode`/`send_text`/`focus_textfield` + BACK/DELETE 码点对齐）
> 已在盘，但揪出 **LEFT/RIGHT 裸 `markerpos±1` 与字节索引口径不对称** → 立项
> 1-7 并已修复/自测/构建。逐字证据见 `notes/wave1-login-ime-recheck-20261109.md`。
> **Wave1 1-1~1-5 至此全部逐字核准完毕。**

| # | 标签 | 功能点 | 证据锚点 | 改动范围 | 自测方式 | 提交 scope |
| --- | --- | --- | --- | --- | --- | --- |
| 1-1 | 错 | ~~5 个收包空实现需补语义~~ | `WaveAShellHandlers.cpp:507-513,710-714,3551-3578` deactivate/`set_status` 即 v83 结束/离开语义；`TestingHandlers.cpp:184-186` 未知 mode 兜底 | 无（语义正确） | 灌回包看 UI 状态 | ✅ 销案（旧账 #9 已闭环） |
| 1-2 | 错 | ~~`RECALCULATE_STATS`(35) 撞值死代码~~ | `Net/PacketSwitch.cpp:735-740` 已注销该 emplace 并写明 s9 只发 `FORCED_STAT_RESET`、被顶掉属性不回弹；`PlayerHandlers.cpp:275` handler 仍在 | 无（有意裁决） | `gen_opcode_matrix.py --check` | ✅ 销案（有意裁决，强接会破坏属性回弹） |
| 1-3 | 错 | ~~`GENDER_DONE(4)`/`CONFIRM_EULA_RESULT(5)` 事实死注册需收敛~~ | `PacketSwitch.cpp:58-62,434-437` 注释「s9 下不发、注册仅作协议完整性」；`LoginHandlers.cpp:454-486` 两 handler 带 2026-11-03 实机残留窗修复；`:50-57` LOGIN_RESULT 无条件清 GENDER/TOS/LOGINNOTICE | 无（有意裁决） | 登录链烟测无回归 | ✅ 销案（强删破坏 v83 兼容 + 实机修复） |
| 1-4 | 错 | ~~`WEEK_EVENT_MESSAGE(77)` 需接 `UIEvent` 数据通路~~ | `MessagingHandlers.cpp:501-510` 已改走 yellow tip；注释「强开窗让 GM 反馈看不见且 `UIEvent.cpp:123` 资源缺失越界崩」 | 无（有意裁决） | 灌 77 回包看 yellow tip | ✅ 销案（强接会崩） |
| 1-5 | 交 | ~~中文 IME `Textfield` 光标 + 显示链路需补齐~~ | `Textfield.h:55-58` `set_accept_unicode`/`accepts_unicode`；`IO/UI.cpp:496-527` `send_text`/`focus_textfield`；`UIMapSearchPanel.cpp:188` 启用中文 IME；`Textfield.cpp:192-202,230-238` BACK/DELETE 码点对齐 | 主体零改动（仅 1-7 补 LEFT/RIGHT） | `python tools/test_textfield_utf8_caret.py` | ✅ 主体销案（真缺陷转 1-7） |
| **1-6** | 错 | **UICharSelect 键盘导航边界守卫**：空列表 RIGHT 误翻页 `BtPageR` / 删角色后 `selected_character` 残留越界致 `button_pressed` 落到 `CHARACTER_SLOT0` 之前（实测 `8=BtPreview`） | live `UICharSelect.cpp:828-894`（`remove_character():955-987` + `select_last_slot():1322-1335` 残留越界）；同构 round34 `72f1a6ce`；`notes/wave1-charselect-recheck-20261109.md` §2 | `UICharSelect.cpp` 2 处守卫（13+） | `python tools/test_charselect_guard.py`（2 守卫 + 10 边界 PASS，A:press 2→0 / B:misfire 2→0）+ `build_smoke.bat` GATE OK + EXITCODE=0 | ✅ `#devfix-charselect-guard`（round35 已提交） |
| **1-7** | 交 | **Textfield UTF-8 光标边界守卫**：`markerpos` 是字节索引（`GraphicsGL.cpp:1349`），LEFT/RIGHT 裸 `±1` 落进多字节汉字续字节 → `add_string:294 text.insert` 劈成非法 UTF-8；BACK/DELETE 已有码点对齐，左右键漏守卫 | live `Textfield.cpp:174-187` vs `:192-202,230-238`；`Textfield.cpp:78 textlabel.advance(markerpos)`；`notes/wave1-login-ime-recheck-20261109.md` §2 | `Textfield.cpp` LEFT/RIGHT 补码点边界（ASCII 逐字节等价） | `python tools/test_textfield_utf8_caret.py`（6 守卫 + 10 边界 PASS，before bad=28/split=5 → after bad=0，ASCII 回归 OK）+ `build_smoke.bat` GATE OK + EXITCODE=0 | ✅ `#devfix-utf8-caret`（本轮已提交） |

### Wave 2 · 需 orange-wz 内容层 / Photoshop 位图（L-A，UI 拼接·动画·样式，多数当前 ⏳）

| # | 标签 | 功能点 | 证据锚点 | 阻塞 | 恢复条件 |
| --- | --- | --- | --- | --- | --- |
| 2-1 | UI/动 | 各面 F 维 12 项样式降级清单（技能名色 BLACK/EMPEROR、TV 双屏、名牌、hit 框、AskAvatar、QuestIcon、宏按钮等） | 各面 `notes/r03-img-*-fallback.md` | orange-wz 内容层 `IMG 解析失败` | 内容层恢复后按 orange-wz 节点逐像素对齐；否则按 AGENTS.md 合规 `.nx` 回退留证 |
| 2-2 | 动 | `DamageSkin` 运行时资源、宠物亲密/饱食 gage 条、场标→真精灵、MapleTV 双屏轮播 | `feature-gap` §16 #17 / §7 E19-E22 / §14.3；`Gameplay/MapleMap/Npc.cpp:151-198` | 资源层 | Photoshop MCP 出图 + native 动画规范 |
| 2-3 | UI | NPC 对话排版 A11b–A11j（下项/高亮/GetText/GetNumber/AskAvatar/AskBoxText）、玩家列表五页签、UIGuildBBS 多页 | `pending-live-tests` #19/22-25/36；`UINpcTalk.h:35-347` | 样式资源 + 真机 | 先做可离屏渲染的拼接，观感项进 §6 |
| 2-4 | 动 | 隐藏门三态 Start/Continue/Exit、SKILL_EFFECT 起手广播、NPC_ACTION 对话姿态 | `pending-live-tests` #15/16/18；`Portal.h:52-58` | 真机触发 | 代码已接线，动画观感进 §6 |
| **2-5** | 交 | **UINpcTalk SENDSIMPLE 键盘导航可视带守卫**：长 #L 菜单 UP/DOWN 只挪 `simple_selected` 不动 `offset`，draw/send_cursor 有可视带裁剪而 send_key 漏守卫 → 回车提交不可见项 | live `UINpcTalk.cpp:733-758` vs `:225-263/:630-651`；`Slider.cpp:116-121` 三参 `setrows` 不触发 onmoved；`notes/wave2-npctalk-recheck-20261109.md` | 无（纯本地 L-A，可离屏自测） | `UINpcTalk.cpp` 插入 `ensure_visible()`（45+）+ 同步 thumb；orange-wz 结构层 WORKS/内容层 FAILS 后合规 `.nx` 回退留证 |
| | | ↳ 自测/门禁 | `tools/test_npctalk_simple_visible.py`（7 守卫+10 场景 PASS，before bad=16→after 0，回归 OK）+ `python tools/gen_opcode_matrix.py --check`（GATE OK）+ `build_smoke.bat`（GATE OK + EXITCODE=0，00:08 真重编译） | | ✅ `#devfix-npctalk-visible`（round37 已提交）；真机观感项进 §6 / `pending-live-tests` §16 |
| **2-6** | 交 | **UIRaceSelect SoftKey 数字选职业可视窗口守卫**：modern 分支只卡 `idx < order.size()` 上界，翻页后输小号数字把 `bid` 落到 `BtStart/BtMake/BtRightArrow` 功能钮、输大号数字经 `:742` 无上界分支越界读 `class_index[selected]`；classic 分支 `:660` 反而有上界（同族不对称） | live `UIRaceSelect.cpp:539-557` vs `:651-679/:742-747/:761-768`；`UIRaceSelect.h:66,70-82,139-142`；`notes/wave2-raceselect-recheck-20261110.md` §2 | 无（纯本地 L-A，可离屏自测，不解析 WZ/IMG/XML） | `UIRaceSelect.cpp` SoftKey 补 `[index_shift, index_shift+INDEX_COUNT)` 窗口守卫 | `tools/test_raceselect_softkey_guard.py`（4 守卫+10 边界 PASS，before bad=6→after 0，回归 4/4）+ `python tools/gen_opcode_matrix.py --check`（GATE OK）+ `build_smoke.bat`（GATE OK + EXITCODE=0，00:36 真重编译） | ✅ `#devfix-raceselect-softkey`（round38 已提交）；真机 SoftKey 观感项进 §6 / `pending-live-tests` §17 |
| **2-7** | 交 | **UIGuildBBS 翻页上界守卫**：`BtPageLeft` 有 `if (page_index > 0)` 下界守卫，`BtPageRight` 直接 `request_list(page_index + 1)` 无上界 → 末页再点「下一页」发必然返回空列表的越界页请求，`apply_list():167` 把 `selected_row` 清成 -1、窗口空白、`page_index` 停在越界值（鼠标 + 键盘 `.`/`]` 同源） | live `UIGuildBBS.cpp:512-518` vs `:149-156/:158-175/:557-560`；`UIGuildBBS.h:97,121-122`；`notes/wave2-guildbbs-recheck-20261110.md` §2 | 无（纯本地 L-A，可离屏自测，不解析 WZ/IMG/XML） | `UIGuildBBS.cpp` 与 `BtPageLeft` 对称补 `threads.size() >= LIST_MAX_ROWS` 上界守卫（纯客户端铁事实，不依赖 `total_threads` 服务端语义） | `tools/test_guildbbs_page_guard.py`（5 守卫+10 边界 PASS，before bad=5→after 0，回归 4/4）+ `python tools/gen_opcode_matrix.py --check`（GATE OK）+ `build_smoke.bat`（GATE OK + EXITCODE=0） | ✅ `#devfix-guildbbs-page`（round39 已提交）；真机 BBS 观感项进 §6 / `pending-live-tests` §18 |
| **2-8** | 动/交 | **召唤兽 MOVE_SUMMON 影子缓存洪泛**：`last_summon_oid[8]` 三字段只在 `clear()` 复位，die 播完 / lifetime 到期 / `RemoveSummonHandler` 三条回收路径都从 `markers` 移除对象却不清数组槽 → 8 槽被僵尸 oid 占满后新静止召唤每 TIMESTEP 重发 `MoveSummonPacket`（≈60/s）直到换图 | live `MapFieldMarkers.cpp:304-311,344-367`（修复前）；`MapObjects.cpp:59-95,119-129` / `MapSummon.cpp:190-193` / `MapObjectHandlers.cpp:1423-1434`；`notes/wave2-summon-movepath-recheck-20261110.md` §2 | orange-wz 资源层结构 WORKS / 内容 6 次 `IMG 解析失败` FAILS → 合规 `.nx` 回退留原文+65536B key 规格 | `MapFieldMarkers.{h,cpp}` 换键控 `unordered_map` + 每帧自愈裁剪（24+/23-），不改协议/包体/WZ/IMG/XML | `tools/test_summon_movepath_flood.py`（10 守卫+10 场景 PASS，before flood=207→after 0，回归 7/7）+ `GATE OK` + `build_smoke.bat` `EXITCODE=0`（01:32 真重编译 278340B / ERROR_COUNT 0） | ✅ `#devfix-summon-movepath`（round40 已提交）；真机观感项进 `pending-live-tests` §19 |
| **2-9** | 交 | **UIItemInventory 重复 G 键入口遮蔽**：同一 else-if 链 `:1182` G/g→Gather 与 `:1229` G/g→UseGachaExp 互相遮蔽，后段零可达 → `USE_GACHA_EXP(158)` 空包「补了包没入口」；`df31fb1c` 同时新增双 G、父版本已有 Gather | live `UIItemInventory.cpp:1182,1229`（修复前）/`:534-538`；`Net/Packets/SolomonPackets.h:22-26`；`Net/OutPacket.h:294-295`；`git blame df31fb1c`；`notes/wave2-iteminventory-recheck-20261110.md` §2 | 无（纯本地 L-A，可离屏自测，不解析 WZ/IMG/XML） | `UIItemInventory.cpp` 第二处 G 分支改链内空位 X/x，G/g 继续归整理 | `tools/test_iteminventory_gkey.py`（10 守卫+10 键位 case PASS，before Gacha reach=0→after 2，duplicate G 2→1，回归 8/8）+ `gen_opcode_matrix.py --check`（GATE OK）+ `build_smoke.bat`（GATE OK + EXITCODE=0，01:50 真重编译 9426B / ERROR_COUNT 0） | ✅ `#devfix-iteminventory-gkey`（round41 已提交）；真机键位体感项进 §6 / `pending-live-tests` §20 |
| **2-10** | 交/稳 | **UICommonCreation avatarSel 空节点循环无符号下溢**：`init_modern :181` `for (size_t i = 0; i < avatarSel.size() - 1; i++)`，`avatarSel = CustomizeChar["avatarSel"]`（`:163`）可空且 `init_modern` 无空门控，`nl::node::size()` 返回 `size_t`（`nlnx/node.hpp:118`） → 空节点 `size()-1 == SIZE_MAX` 构造期挂死；`init_classic :383` 反而有 `if (!NewChar) return;`（同族不对称） | live `IO/UITypes/UICommonCreation.cpp:163,181-202`（修复前）/`:383-387`；`includes/NoLifeNx/nlnx/node.hpp:118`；`notes/wave2-commoncreation-recheck-20261110.md` §2 | 无（纯本地 L-A，可离屏自测，不解析 WZ/IMG/XML 内容） | `UICommonCreation.cpp` 循环上界改 `i + 1 < avatarSel.size()`（上界无减法、size>=1 逐字等价、size==0 零迭代） | `tools/test_commoncreation_avatar_guard.py`（9 守卫 + 8 场景 PASS，before HANG 1→after 0，回归 7/7） + `gen_opcode_matrix.py --check`（GATE OK） + `build_smoke.bat`（GATE OK + EXITCODE=0，02:22 真重编译 16010B / ERROR_COUNT 0） | ✅ `#devfix-commoncreation-avatar`（round42 已提交）；真机 modern/classic 新建角色与资源裁剪态项进 §6 / `pending-live-tests` §21 |
| **2-11** | 交/稳 | **UIOptionMenu 分辨率下拉框 sButtonUOL 空串 UB / stoi 抛异常**：构造期 `:96` `graphic["combo:resolution"]["sButtonUOL"].get_string()` 在子节点裁剪时返空串（`nlnx/node.cpp:199-217`），`:97` 空串 `.back()` UB、`:98` 裸 `std::stoi` 非数字抛 `invalid_argument` → 设置窗构造崩；`:53` 根 `usable()` fallback 护不住子节点，同文件 `:335-345` 另两处 stoi 已包 try/catch（反证漏守卫） | live `IO/UITypes/UIOptionMenu.cpp:51,53,77,96-98,150,335-345`（修复前）；`includes/NoLifeNx/nlnx/node.hpp:106` / `node.cpp:199-217`；`IO/Components/MapleComboBox.h:32-44` / `.cpp:30-37`；`IO/UITypes/UIGender.cpp:56`；`git blame 0c294c6c`；`notes/wave2-optionmenu-recheck-20261110.md` §2 | 无（纯本地 L-A，可离屏自测，不解析 WZ/IMG/XML） | `UIOptionMenu.cpp` 改 `DEFAULT` 初值 + `!empty()` + 末位数字守卫，合法数字逐字等价（13+/2-） | `tools/test_optionmenu_combo_guard.py`（12 守卫+13 场景 PASS，before UB/THROW 4→after 0，回归 9/9）+ `gen_opcode_matrix.py --check`（GATE OK） + `build_smoke.bat`（GATE OK + EXITCODE=0，02:43 真重编译 4962B / ERROR_LINES 0） | ✅ `#devfix-optionmenu-combo`（round43 已提交）；真机下拉框外观/裁剪态项进 §6 / `pending-live-tests` §22 |
| **2-12** | 交/稳 | **UIEnterNumber 数字框空串 vacuous-true / 10 位溢出崩交互**：`handlestring :342` `bool has_only_digits = (numstr.find_first_not_of("0123456789") == std::string::npos);` 对空串返 `npos` （vacuous true）→ 跳过 `:350` 提示分支 → `:358 num = std::stoi(numstr);` 裸奔抛 `invalid_argument`；`:244 numfield_limit = 10` 是字符数上限，`9999999999` 全过字符校验但 `std::stoi` 抛 `out_of_range`。四条入口 `:252` enter_callback / `:300` RETURN / `:329` OK 三条无门控，唯 `:310` SoftKey 有 `if (s.empty()) return;`；`:247` 初值非空但 `Textfield.cpp:215,:253 text.erase(...)` 可退格清空 | live `IO/UITypes/UINotice.cpp:244,247,252,300,310,312,329,339-359`（修复前）；`IO/Components/Textfield.cpp:215,:253`；`UIItemInventory.cpp:1957,:1977` / `UIShop.cpp:822,:938` 业务调用点；`git blame 1ca11fdd`；`notes/wave2-uinotice-recheck-20261110.md` §2 | 无（纯本地 L-A，可离屏自测，不解析 WZ/IMG/XML） | `UINotice.cpp` 改 `!numstr.empty() &&` 挡空串 + `stoi` 包 `try/catch` 复用既有提示兜溢出，合法 1..max 路径逐字等价（17+/2-） | `tools/test_uinotice_handlestring_guard.py`（22 守卫+13 场景 PASS，before THROW 5→after 0，回归 8/8） + `gen_opcode_matrix.py --check`（GATE OK） + `build_smoke.bat`（GATE OK + EXITCODE=0，03:00 真重编译 3521B / ERROR_LINES 0） | ✅ `#devfix-uinotice-handlestring`（round44 已提交）；真机丢弃/买卖数字框体感项进 §6 / `pending-live-tests` §23 |
| **2-13** | 交/稳 | **UIItemInventory 资源驱动 slot_col 除零旁路守卫**：`UiAssetResolve::window("Item")` 明确支持 `UIWindow2.img`/`UIWindow.img` 多资源路径，`if(pos)` 旁路 `:144` 裸信 `pos["slot_col"]/["slot_row"]`，nlnx `get_integer()` 缺失/非整数返默认 0 (`nlnx/node.cpp:161-179`) → `:310`/`:689`/`:690` 三处整数除零 + `max_slots` 归零；主体 `else :159` 反而有位图实测常量兜底（同族不对称）。当前 shipped v83 包 `UIWindow.img/Item` 无 `pos` 走 else 不可达，换带 pos 裁剪包即触发，诚实标注不虚报当前必崩 | live `IO/UITypes/UIItemInventory.cpp:131,140-167,310,689,690`（修复前）/ `UIItemInventory.h:157-158`；`Util/UiAssetResolve.cpp:42-55`；`includes/NoLifeNx/nlnx/node.hpp:104,116-118` / `node.cpp:161-179`；`git blame 16d01198/df31fb1c`；`notes/wave2-iteminventory-slotcol-recheck-20261110.md` §2 | orange-wz 结构层 WORKS / 内容层 `IMG 解析失败: UIWindow.img` FAILS → 合规 `.nx` 回退（65536B GMS key `268a8407a47ee76c` / iv `TSPHKw==`） | `UIItemInventory.cpp` `if(pos)` 块补 `slot_col<=0 || slot_row<=0` 整组回退到与 else 相同常量（14+），合法包零改动 | `tools/test_iteminventory_slotcol_guard.py`（17 守卫+3 计数守卫+13 场景 PASS，before DIV0 9/BAD 6→after BAD 0，回归 7/7） + `gen_opcode_matrix.py --check`（GATE OK） + `build_smoke.bat`（GATE OK + EXITCODE=0，03:29 真重编译 9426B / ERROR_LINES 0） | ✅ `#devfix-iteminventory-slotcol`（round45 已提交）；真机裁剪包/切页签与 C4701 消警项进 §6 / `pending-live-tests` §24 |
| **2-14** | 交/稳 | **UICharInfo 子窗口稀疏 map 裸 at(i) 抛异常**：`draw()` 主窗口 `:225-230` 与 `:305` BtBook 已用 `find+判空`（16d01198/93accb4f 修），子窗 Collect `:278-280` / Damage `:294-296` 仍留裸 `buttons.at(i).get()`；`buttons` 是 `std::map<uint16_t,unique_ptr<Button>>`（`IO/UIElement.h:265` 稀疏），`try_btn :77-81` 只在资源节点存在时插入 → 缺节点即缺键，`at()` 确定性抛 `std::out_of_range`；库层 `IO/UIElement.cpp:168-176` 注释明言「必须判空后再解引用」。当前 shipped v83 包 `UIWindow.img/UserInfo` 无 collect/damage/character 子节点，子窗循环不常进，多资源/裁剪包下确定性抛异常，诚实标注不虚报当前必崩 | live `IO/UITypes/UICharInfo.cpp:221,225-230,278-280,294-296,305,77-81,267,289,731-749`（修复前）/ `UICharInfo.h:55-76`；`IO/UIElement.h:265` / `IO/UIElement.cpp:166-192`；`git blame 25347525/16d01198/93accb4f`；`notes/wave2-charinfo-recheck-20261110.md` §2 | orange-wz 结构层 WORKS / 内容层 `IMG 解析失败: UIWindow.img` FAILS → 合规 `.nx` 回退（65536B GMS key `268a8407a47ee76c` / iv `TSPHKw==`）；否决 UIKeyConfig `.at()` 候选（get_keytype 永不返 ITEM/SKILL、无可达路径） | `UICharInfo.cpp` 两处子窗循环改回 `find+判空`（对齐 `:225-230` 既有范式、`buttons.size()` 上界与循环边界一字不动），合法键齐备构图逐字等价（16+/4-） | `tools/test_charinfo_button_guard.py`（13 守卫+2 计数守卫+13 场景 PASS，before THROW 6→after 0，回归 7/7） + `gen_opcode_matrix.py --check`（GATE OK） + `build_smoke.bat`（GATE OK + EXITCODE=0，04:00 真重编译 5256B / ERROR_LINES 0） | ✅ `#devfix-charinfo-buttonguard`（round46 已提交）；真机子窗渲染/裁剪态项进 §6 / `pending-live-tests` §25 |
| **2-15** | 交/稳 | **UICharInfo damage 子窗循环上界错用元素个数**：`draw()` 三段键域里主窗 `i < Buttons::BtArrayGet`、Collect `i < Buttons::BtFAQ` 都是枚举分段，唯独 Damage `:294` 写 `i < buttons.size()`——`buttons` 是稀疏 `std::map`（`IO/UIElement.h:265`），`try_btn :77-81` 只在资源节点存在时插键，元素个数与键域无数量关系：`size<16` 循环一次不跑（BtFAQ/BtRegist 全漏绘）、`size==17` 漏 BtRegist、`size>=19` 多跑 i=18 与 `:317` 重复绘 BtBook；happy-path `size==18` 前后等价故 round46 回归未暴露（同族「主体正常、旁路漏同步」） | live `IO/UITypes/UICharInfo.cpp:294-296,317-318,77-81,225-230,281-286`（修复前）/ `UICharInfo.h:55-76`；`IO/UIElement.h:265`；`notes/wave2-charinfo-damage-recheck-20261110.md` §1 | 无（纯本地 L-A，可离屏自测，不解析 WZ/IMG/XML） | `UICharInfo.cpp` 上界 `buttons.size()` → `Buttons::BtBook`，与主窗/Collect 同构枚举分段（4+/4-），并改写 round46「保留原循环边界」旧注释 | `tools/test_charinfo_damage_bound.py`（14 守卫 + 13 场景 PASS，before BAD 9 / BtBook 重复 3 → after 0，回归 4/4） + `gen_opcode_matrix.py --check`（GATE OK） + `build_smoke.bat`（GATE OK + EXITCODE=0，04:38 真重编译 / ERROR_LINES 0） | ✅ `#devfix-charinfo-damage-bound`（round47 已提交）；§25-4 台账同步销案；真机 damage 子窗渲染项进 §26 / `pending-live-tests` §26 |
| **2-16** | 交/稳 | **SHOW_STATUS_INFO mode 5 语义错位（金币当日志 SP）**：客户端把 `mode == 5` 当 SP，`read_short` 只吃掉服务端 4 字节 `int gain` 的低 2 字节，再弹 `UISkillUp` 写「You have gained SP (+N)」。s9 `PacketCreator.java:1763-1773 getShowMesoGain(gain, inChat=true)` 写的是 `writeByte(5) + writeInt(gain) + writeShort(0)`（同文件 `:1713` 注释 `5 = mesos`）；Angel 083 `CWvsContext::OnMessage case 5 → OnIncMoneyMessage@0xa221f3` 也是 `Decode4` + `SP_290/SP_292` + `CHATLOG_ADD(nType:7)`，无任何 UISkillUp 调用。SP 在两端都不走本包（s9 `changeRemainingSp`→`STAT_CHANGED`；客户端 `ChangeStatsHandler::need_skillbook_update` 已含 SP）。高频路径：任务金币奖励 / 商店售出 / 传送扣费（`spendMesoTransactionally`）每次都发 | live `Net/Handlers/MessagingHandlers.cpp:313-350`（修复前）；`E:/project/BeiDou-Server_s9/.../PacketCreator.java:1713,1763-1773`；`notes/wave2-showstatusinfo-mode5-recheck-20261110.md` §1-§2 | 无（纯文本提示路径，可离屏自测，不解析 WZ/IMG/XML） | mode 5 改读 `int32_t gain = recv.read_int()` + 金币双文案 + 仅 `UIChatBar`（亏损 RED），删除错的三分支 SP 刷新（17+/32-） | `tools/test_showstatusinfo_mode5.py`（15 守卫 + 6 场景 PASS，before 误标 6/6 → after 0，稳定 6/6） + `gen_opcode_matrix.py --check`（GATE OK） + `build_smoke.bat`（GATE OK + EXITCODE=0，10:21 真重编译 / ERROR_LINES 0） | ✅ `#devfix-showstatusinfo-mode5`（round49）；**附带发现**：`InPacket::available()` 返回 `bool` 却有 393 处当字节数比较，已登记 §3 待裁决（协议级，需真机回归） |
| **2-17** | 稳 | **K1 批量零风险销案（drop/D7 + drop/D4 + statusbar-hud/D5 + corewindow/D5）**：跨面 168 条决策点里 K1=10 条可直接做，本轮取 P0 代表项 4 条。①`Drop::Drop`（`Gameplay/MapleMap/Drop.cpp:22-54`）`switch(mode)` 缺 `default` 而 `Drop.h:55 State state;` 无初值 → 非法 mode 下 `update()` 首次比较即 UB（s9 `DROP_ITEM mod` 实测只发 {1,2,3}，当前不可达，属防御性护栏）；②`MapDrops.cpp:80` `else if (ItemData::get(itemid))` 为假时掉落被静默丢弃且全文件无 `LOG(` → 补 `else` + DEBUG 日志（不改变不实例化行为）；③`UIStatusBar.cpp:1905` 按钮 id 注释误标 `KeySet=2004 / QuickSlot=2005 / QuickSlotD=2006`，Angel `CUIStatusBar::OnCreate@0x8d01b2` 逐字为 `1005/(738,515)` / `1006/(768,515)` QuickSlot / `1009/(573,515)` BtClaim（该行仅注释、无代码引用）；④corewindow 4 处 095 线地址换 083 实证（`CUIStat::OnCreate@0x8c4c7f` 逐字只建 BtDetail 无 BtAuto、`CUIItem::OnCreate@0x81c6c9`、`OnChildNotify@0x81d01f`、`SetArrangeButton@0x81e3a6`、`CWvsContext::OnGatherItemResult@0xa1e943` / `OnSortItemResult@0xa1e96d`） | live `Gameplay/MapleMap/Drop.cpp:22-54` / `MapDrops.cpp:80-84` / `IO/UITypes/UIStatusBar.cpp:1905` / `UIStatsInfo.cpp:146-153` / `UIItemInventory.cpp:124,544-548,1457` / `UIItemInventory.h:50` / `Net/Handlers/InventoryHandlers.cpp:56,71`；`notes/wave2-k1-batch-round52-20261110.md` §1 | 无（4 条全是纯注释 / 纯日志 / 非法 mode 护栏，不解析 WZ/IMG/XML） | `Drop.cpp` 补 `default` 兜底 DROPPED；`MapDrops.cpp` 补 else+LOG 与两个 include；`UIStatusBar.cpp` 注释 1005/1006/1009；6 处 083 EA 换掉 095（7 files / 60+/25-） | `tools/test_k1_batch_round52.py`（22 静态守卫 + meso 四档 8 组 + Drop 构造 6 非法 mode，`PASS` / `TEST_RC=0`）+ `gen_opcode_matrix.py --check`（GATE OK） + `build_smoke.bat`（GATE OK + EXITCODE=0，11:21 真重编译 181614B / ERROR_LINES 0） | ✅ `#devfix-k1-batch-round52`（round52）；真机 4 项进 `pending-live-tests` §28；残留 5 处 095 污染进 notes §7 台账 |
| **2-18** | 交/稳 | **UIChatBar::change_target 空 stub 接线（六个聊天频道键复活）**：`IO/UITypes/UIChatBar.h:122-123` 是 `// TODO: Change these?` + `void change_target(int32_t action) {}` 空函数体，而 `IO/UIStateGame.cpp:471-482` 把 `SAY / PARTYCHAT / FRIENDSCHAT / GUILDCHAT / ALLIANCECHAT / TOSPOUSE` 六键全导向它 → 按了既不切目标也不开输入行；发送侧 `send_to_target_channel()` 与设置侧 `set_classic_target()` 早已就绪，缺的只是中间那段映射。同族不对称：`TOGGLECHAT`(`:497-512`) 有完整开框分支、六键没有 | live `IO/UITypes/UIChatBar.h:122-123,365-376`（修复前）/ `UIChatBar.cpp:1613-1640,2175-2210,2313+`；`IO/UIStateGame.cpp:471-482,497-512`；`IO/UITypes/UIUserList.cpp:916`（自承「预留空实现」）；Angel 083 `CWvsContext::UseFuncKeyMapped @ 0xa0773d`；`notes/wave2-chattarget-keybind-recheck-20261110.md` §1 | 无（纯本地 L-A，可离屏自测，不解析 WZ/IMG/XML） | `UIChatBar.{h,cpp}` 加 `KeyAction::Id` → `ChatTargetIndex` 映射 + `set_classic_target()` + `if (!view_input) toggle_input(true)`，`default: return` 保证无关键零副作用（56+/2-） | `tools/test_chattarget_keybind.py`（头文件占位消失 + 六键映射正则 + `default: return` + 开框条件 + 083 依据 + `UIStateGame.cpp` 六 case 原样 + 行为复刻六键命中/四键未命中 + 已开框不收框 + 目标号域，`PASS` / `TEST_RC=0`） + `gen_opcode_matrix.py --check`（GATE OK） + `build_smoke.bat`（GATE OK + EXITCODE=0） | ✅ `#devfix-chattarget-keybind`（round53 已提交）；真机六键切目标/modern 观感/无频道回执项进 §6 / `pending-live-tests` §29 |
| **2-19** | 交/稳 | **聊天输入框未启用系统 IME（中文完全打不进来）**：`IO/UITypes/UIChatBar.cpp` 的 classic `:139-146` 与 modern `:287-291` 两处 `input_text` 构造都没调 `set_accept_unicode(true)`，走 `Textfield.h:84 accept_unicode = false` 默认值；`IO/Components/Textfield.cpp:291-318 add_string()` 的非 unicode 分支 `if (c < 32 || c >= 128) continue;` 把中文 UTF-8 字节全部丢弃，`modifytext():320-339` 二次同口径过滤；`IO/UI.cpp:507-527 focus_textfield()` 在 `accepts_unicode()` 为假时调 `end_ascii_text_input()`，系统 IME 根本不激活，而 `IO/UI.cpp:294-297` 的 KEY_DOWN TEXT 分支只认 32..127 —— 两条通路都进不了中文。渲染侧早已支持 CJK（`Graphics/GraphicsGL.cpp:283 FT_ENCODING_UNICODE` + `:445-480 get_wide_glyph` 按码点取宽字 + `Configuration.h:556` 思源宋体含 CJK），全仓另有 4 处同 API 先例（`UIMapSearchPanel.cpp:189` / `UIMiscShells.cpp:1251`、`:1549` / `UINpcTalk.cpp:1690`），聊天框是唯一漏掉的。修复 = 两处各加 `input_text.set_accept_unicode(true);`；`limit = 70` 是字节上限（中文 3 字节/字 ⇒ 约 23 字）本轮不改，s9 `GeneralChatHandler.java:46` 按字符限 127 的换算属行为变更，记台账。harness `tools/test_d3_chat_unicode.py` 17 静态守卫 + 10 场景（修前 CJK 全空 / 修后完整、ASCII 与超限逐字不变）。决策依据见 `docs/features/angel-calibration-d-matrix-2026-11/README.md` §5 `statusbar-hud/D3`。
| **2-20** | 交/稳 | **K1 收尾（social-economy/D1 后半 + corewindow/D14）**：`UIMessenger.cpp` 10 处 + `.h` 6 处 + `messenger-btclose-coords/README.md:16` 的 `v95` 注释依据全部换为 Angel 083 真符号/地址（`SetCtrl 0x8505e2` / `OnCreate 0x84ddb0` / `SetAvatar 0x84f44b` / `Update 0x84e930` / `OnChat 0x8515d7` / `ProcessChat 0x851d3b`），并证伪旧注释里的 `0x7F34C0`（083 实为 `sub_7F3483`）；`UIKeyConfig.cpp` 三处硬编码英文确认文案改 `Locale::text_or` + `locale/zh-CN.ini`/`en-US.ini` 各补 3 键（en-US 值按决策 B 保持英文原值）。纯注释 + 文案来源，零行为/零协议/零坐标变更。harness `tools/test_round55_d1_d14.py` A–E 52 项全绿，GATE OK，build `EXITCODE=0`。销 `social-economy/D1`（后半）与 `corewindow/D14`（英文提示部分）|
| **2-21** | 交/稳 | **「查看推荐世界」弹窗按 Angel 083 布局重修（WIP 收口）**：外部程序留下的 WIP 把 `Login.img/WorldSelect/alert/backgrd` 写成大小写+拼写双错的节点名后误判「R6 裁剪」，改拿 `chBackgrn` 当背景、用 `Text` 手绘标题与 `<`/`>` 导航；`.nx` 实测 `alert` 子树完整、真背景 bitmap 540×167（导出 `tools/_rw_visual/backgrd.png` 可见深棕边框+金色标题+左右浅色面板），资源根本没缺 —— 属用户 m09859 点名要重修的「自行修复 UI」。按 083 `OnCreate @0x62b451` 重修：真背景进基类 sprites、dimension(540,167)、三颗真位图按钮 BtArrowL(45,70)/BtArrowR(215,70)/BtChoice(113,114) 经 `try_maple_btn` 落地、消息区 {78,61,212,110} 点击选中、换行宽 200、世界数>1 才启用箭头、`move_world` 补 `wc==0` 守卫。配套删 `UICashShop.cpp:98` 重复赋值；`LoginBackground.cpp` 线性插值经复核确属按 Angel 修复过，保留 | `tools/_ida_recommend/*.txt`（9 函数反编译）；`tools/_rw_visual/backgrd.png`；orange-wz 门禁 `IMG 解析失败: Login.img` 留证 + 65536B GMS key 合规 `.nx` 回退；`notes/wave2-recommendworld-083-relayout-20261110.md` §2-§4 | 门禁 18 已走（IDA→orange-wz→.nx 回退）；校准≠改回原生（数据源沿用 SERVERLIST + `change_world(World)`，不动 `RecommendedWorldMessageHandler`） | `UIRecommendWorld.h/.cpp` 整文件重写 + 3 处错注释换 083 依据 + 删重复赋值（9 files） | `tools/test_recommendworld_layout.py`（46 守卫 + 19 场景 PASS / TEST_RC=0）+ `gen_opcode_matrix.py --check`（GATE OK）+ `build_smoke.bat`（EXITCODE=0，build_log 3073B→460448B 真重编，`UIRecommendWorld.cpp` 编到 2 次，ERROR_COUNT 0） | ✅ `#devfix-recommendworld-083-relayout`（round66）；真机 5 项进 `pending-live-tests` §32；名牌/消息 anchor 精确对齐、滚动条 id=1003、opcode 27 自动弹窗属后续增强，登记 backlog |
| **2-22** | 交/稳 | **HUD 主窗 4 钮闪烁按 Angel 083 SetButtonBlink 状态机重修**：外部程序 `d6bdce97` 误判「本包无 KEY_ANI 资源」→ 手绘半透明白块 7.5Hz 脉冲、点击开子窗即启动；`.nx` 实测 `StatusBar.img/<Key>/ani/{0,1}` 四组真帧齐全（28x20 delay 500ms=1Hz，`ani_0` 亮黄发光版、`ani_1` 常态暗版且 `normal_0` sha256 与 `ani_1` 全同），083 `SetButtonBlink @0x8dfe21` 5 调用点 a3 语义更与外部程序方向相反（开子窗/子窗通知100/90s 超时 = a3=0 清除；低等级<=10 升级 `OnStatChanged @0xa1fce9` / 任务指引 `CUtilDlgEx::Draw @0x9a4256` = a3=1 启动）—— 资源/频率/时序三层全错，正是用户 m09859 点名「自行修复 UI 有问题」。按 083 重修：真 `Animation(sb[<Key>]["ani"])` 两帧替换 `ColorBox` 白块、4 处开子窗改 `stop_button_blink`(清除)、`elapsed_ms` 逻辑帧累计等价 90s 超时、`stats.get_stat(LEVEL)` 自感知低等级升级触发 STAT 钮（不动 s9 协议）、三处错注释改 083 依据；`LoginBackground` 线性插值复核确属按 Angel 修过予以保留 | `tools/_ida_hud_blink/*.txt`（SetButtonBlink+5 调用点反编译）/ `tools/_sb_visual/*.png`（真帧导出）/ `evidence/hud-button-blink-083-round67.json`；orange-wz 门禁 `IMG 解析失败: StatusBar.img` 留证 + 65536B GMS key 合规 `.nx` 回退；`notes/wave2-hud-button-blink-083-relayout-20261111.md` §1-§4 | 门禁 18 已走（IDA→orange-wz→.nx 回退）；校准≠改回原生（不动协议，启动链用 UI 层等级自感知） | `UIStatusBar.h/.cpp`（真动画成员+状态机方法+draw 真帧+update 90s/升级+纠错注释） | `tools/test_hud_button_blink_083.py`（37 守卫 + 13 场景 PASS / TEST_RC=0） + `gen_opcode_matrix.py --check`（GATE OK）+ `build_smoke.bat`（EXITCODE=0，build_log 460448B→71067B 真重编，`UIStatusBar.cpp` 编到 22 次，ERROR_COUNT 0） | ✅ `#devfix-hud-button-blink-083`（round67）；真机 5 项进 `pending-live-tests` §33；启动链是否接 s9 升级 handler、任务指引 blink、D8 组队血条/好友窗几何待逐面复核 |
| **2-23** | 交/稳 | **好友窗 / 组队窗按 Angel 083 CUIUserList 真资源重修（D8 组队血条）**：`7105da94`/`d6bdce97` 用全库 0 命中的虚构根 `UIWindow.img/FriendRecommendations/FriendsList` 与 `Party.img` + 自定 182x310/200x300 + `UIParty::draw` 里 `ColorBox` 手绘血条；`.nx` 实测真根 `UIWindow.img/UserList/backgrnd`(312x389)、`Friend/BtAddFriend`(65x18)、`Party/PartyHP/GaugeBar/{bar 63x7,gauge 1x7,graduation 69x13}` 全在，083 `CUIPartyHP::Create @0x91f934` 是独立悬浮窗+6 人+真位图，`CUIUserList::OnCreate @0x919b73` 是五页签共用窗 —— 第三/四例「找错节点后自造兜底」同族病。按 083 重修：两窗真根+dimension(312,389)+真按钮位图(BtAddFriend (5,81) / BtInvite(13,355) / BtWithdraw(190,344) / BtChangeBoss(249,345) / BtKick(72,365))、D8 血条改 GaugeBar 真位图按 cur/max 裁右、行命中 190→302、关闭钮 (294,6)；不动 s9 协议/`memberhp`；`LoginBackground` 线性插值复核按 Angel 修过予以保留 | `tools/_ida_pb/*.dec.txt`（CUIPartyHP ctor/Create + CUIUserList OnCreate 61 SP_ 名）/ `tools/_uw_visual/UserList_backgrnd.png` / `evidence/userlist-partyhp-083-round68.json`；orange-wz 门禁 `IMG 解析失败: UIWindow.img` 留证 + 65536B GMS key 合规 `.nx` 回退；`notes/wave2-userlist-partyhp-083-relayout-20261111.md` §1-§4 | 门禁 18 已走（IDA→orange-wz→.nx 回退）；校准≠改回原生（独立悬浮窗/五页签属架构级决策点，本轮不新建窗口类型） | `UIBuddyList.cpp` / `UIParty.cpp` / `UIParty.h`（3 files, 73+/50-） | `tools/test_round68_userlist_083.py`（37 守卫 + 16 场景 PASS / TEST_RC=0）+ `gen_opcode_matrix.py --check`（GATE OK）+ `build_smoke.bat`（EXITCODE=0，build_log 71067B→57500B 真重编，`UIParty.cpp`/`UIBuddyList.cpp` 各 2 次，ERROR_COUNT 0） | ✅ `#devfix-userlist-partyhp-083`（round68）；真机 5 项进 `pending-live-tests` §34；独立 CUIPartyHP 悬浮窗/五页签/配置开关/完整9钮待架构级裁决 |
| **2-24** | 文/稳 | **D7 聊天三态按 Angel 083 SetChatType 只读取证复核 + 口径纠正**：外部程序 `d6bdce97` 注释把「nType 1/2/3 拖高 26..507」当原生、自造 `CLASSIC_LOG_H_COMPACT=120` 紧凑态、BtMin/BtMax 逐级 3→2→1 且 `chat.type` 持久化；083 `SetChatType @0x8d4a6d` 逐字为「1 收起(h24/无输入框/509-h=513) / 2 输入中(h24/1行+输入框/509-h=486，StartChat 焦点驱动、EndChat 清回、不持久、非按钮) / 3 完整(h=CConfig+23，26..507 内可拖高，越界回落默认 70)」，按钮只有 1003→1 / 1004→3，功能键 case16 3↔1，无逐级、无紧凑固定高度 —— **第 5 例同族病但错法=状态机语义套壳（资源根 `Basic.img/BtMax|BtMin` 经 .nx 实测是对的）**。按校准 README:288 D7 决策点「先五屏实机截图核对再定」+ 用户 m09859 授权边界（布局/动画重修非交互连根拔），本轮只出对照清单/决策点 D7-A/一次性 Plan，**不改任何行为字面量**，仅纠正 3 文件错误依据注释；`LoginBackground` 线性插值复核确属按 Angel 修过予以保留 | `tools/_ida_chattype/*.dec.txt`（SetChatType/ChangeChatWndSize/OnButtonClicked/StartChat/EndChat/UseFuncKeyMapped/OnGameStageChanged/OnCreate 8 函数 + 7 xrefs） / `evidence/chattype-083-round69.json`；orange-wz 门禁 `IMG 解析失败: StatusBar.img` 留证 + 65536B GMS key 合规 `.nx` 回退（`.nx`：`StatusBar.img/chat` 与 `StatusBar.img/BtMax|BtMin` NOT FOUND，`Basic.img/BtMax|BtMin` 真存在） / `notes/wave2-chattype-083-review-20261111.md` §1-§5 | 门禁 18 已走（IDA→orange-wz→.nx 回退）；校准≠改回原生（行为状态机整体改回 083 属 **决策点 D7-A 交用户裁决**，本轮不实施）；`ChangeChatWndSize` clamp 后 `h%13==0→+2` 边界 507→509 / 26→28 已进 harness 真值表 | `UIChatBar.h` / `UIChatBar.cpp` / `Configuration.h`（3 files, 17+/9-，纯注释） | `tools/test_round69_chattype_083.py`（A1-A6 静态守卫含 A4 行为字面量逐字不动 + B 20 场景 083 状态机真值表 PASS / TEST_RC=0） + `gen_opcode_matrix.py --check`（GATE OK）+ `build_smoke.bat`（EXITCODE=0 真重编覆盖 3 文件） | ✅ `#devfix-chattype-083-review`（round69）；真机 5 项进 `pending-live-tests` §35；**决策点 D7-A（是否整体改回 083 三态状态机）待用户裁决**；D2 聊天规则待真机 |
| **2-25** | 文/稳 | **伤害数字 / 伤害皮肤按 Angel 083 CAnimationDisplayer 只读取证复核 + 口径标注**：全仓扫「自造兜底」选 `302eff38` 伤害数字/皮肤面；083 `CMob::ShowDamage @0x6691d3` → `CAnimationDisplayer::Effect_HP @0x437d0f`（逐位图 + `lWidth=3*v23/5` 动态字宽 + canvas 57 + 奇偶/随机 y 抖动 + 400/600/1000ms 三段动画）、`Effect_Miss @0x438a21`（SP_984_MISS 位图 + BasicFloat），`*DamageNumber*`/`*DamageSkin*`/`*CharSet*` 全 0 命中 ⇒ 083 无此类/无皮肤系统。当前端是 Journey DamageNumber + s9 DAMAGE_SKIN(247) 适配，资源根与缩放注释经 .nx 实测属实且自称 Journey，**不是「自造兜底冒称 083」的病，而是架构/版本差异**。用户 m10982 强线索「catalog_Effect.json 漏 64MB DamageSkin.img ⇒ 整根合并失败」经最小 C++ 探针 `_tools/dmg_skin_probe` 5 组 `open_merge` 实测**证伪**：WHOLE_ALL 18 文件合并 `Effect["DamageSkin.img"]` 有效 624 皮肤节点、catalog 全量遍历零异常；漏文件改判「过期快照」（catalog 11-05 17 文件 < Effect 目录 11-10 18 文件），非死链。本轮按「校准≠改回原生 / s9 优先」整体不重写，交付只读取证 + 对照 + **决策点 D-A**（皮肤间距动态化，视觉/架构级待裁决）+ 一次性 Plan + 真机 backlog，仅补 4 处纯注释（083 依据 / Journey+s9 身份 / 可达性实测 / D-A 说明），0 行为字面量 | `tools/_ida_dmg/*`（ShowDamage/Effect_HP/Effect_Miss + 8 xrefs）/ `_tools/dmg_skin_probe/*` / `tools/_dmg_probe_stdout.txt` / `evidence/damagenumber-083-round70.json`；orange-wz 门禁 `IMG 解析失败: BasicEff.img`/`DamageSkin.img` 留证 + 65536B GMS key 合规 `.nx` 回退 / `notes/wave2-damagenumber-083-review-20261111.md` §1-§5 | 门禁 18 已走（IDA→orange-wz→.nx 回退→运行时 open_merge 探针）；校准≠改回原生（083 无 DamageNumber/皮肤，不动 s9 DAMAGE_SKIN(247)）；`lWidth=3*v23/5` 动态字宽与硬编码表 `{24,20,22,22,24,23,24,22,24,24}` 差异已进 D-A | `DamageNumber.cpp` / `DamageNumber.h`（2 files, 纯注释 0 行为改动） | `tools/test_round70_damagenumber_083.py`（静态守卫 083 依据注释 / 行为字面量逐字不动 / 无「死链」误判措辞 / LoginBackground 保留 + 场景：083 3/5 模型 vs 硬编码表 / MISS 'M'→98x38 / 皮肤回落 / catalog 时序） + `gen_opcode_matrix.py --check`（GATE OK）+ `build_smoke.bat`（EXITCODE=0 真重编覆盖 DamageNumber.cpp） | ✅ `#devfix-damagenumber-083-review`（round70）；真机 5 项进 `pending-live-tests` §36；**决策点 D-A（是否按 083 动态字宽重排伤害数字动画）待用户裁决**；`IO/UIElement.h:199-205` GAMEOPT/SYSOPT 属下轮候选 |
| **2-26** | 文/稳 | **GAMEOPT / SYSOPT 设置窗按 Angel 083 CUIGameOpt/CUISysOpt 只读取证复核 + 口径标注**：台账 `IO/UIElement.h:199-205` 挂账的自造版式面。活 IDB 现场反编译 `CUIGameOpt::OnCreate @0x993a45`（12 CCtrlCheckBox x128 y33+18i、OK(34,262)/CANCEL(84,262)）与 `CUISysOpt::OnCreate @0x99417a`（6 滑条 40/70/100/160/190/220、OK/CANCEL y342、无 9 档分辨率下拉）。当前端 `UIGameOpt` 与 083 仅差印刷行内 +2/+1px，round71 v2 像素检测拆出 12 独立洋红格证实 083 与当前端各落第 0..11 格（真绿；初版 215px 宽带 inside=12/12 是假绿已作废）；`UISysOpt` +28/+30、9 档分辨率、BTN_Y=372 属参照 rs.cpp 插件+截图 SAD 的本端扩展。运行时 open_merge 探针实测 SysOpt/backgrnd 299x396（旧 .nx 299x365 是两张完全不同图，逐像素 0/365 命中），先前「+28 越界」推断作废。按「校准≠改回原生 / 数值重合≠语义重合」+ 用户 m09859 授权边界，本轮**不改行为常量**，仅补 083 依据注释（4+1 处纯注释，0 行为字面量）+ 决策点 **D-B**（SysOpt 版式是否回退 083，待用户裁决 + 五屏截图）+ 一次性 Plan | `tools/_ida_opt/*.dec.txt`（8 符号 + 4 反编译）/ `_tools/opt_probe/*` / `tools/_opt_*_stdout.txt` / `tools/_opt_visual/*.png` / `evidence/optionmenu-083-round71.json`；orange-wz 门禁 `IMG 解析失败: UIWindow.img`/`Basic.img` 留证 + 65536B GMS key 合规 `.nx` 回退 → 运行时 open_merge 探针（门禁 18 全链） / `notes/wave2-optionmenu-083-review-20261111.md` §1-§5 | 门禁 18 已走（IDA→orange-wz→.nx→运行时探针）；校准≠改回原生（SysOpt 回退属 D-B 交裁决）；GameOpt 已按 Angel 布局对齐闭环 | `UIOptionMenu.h` / `UIElement.h`（2 files 纯注释） | `tools/test_round71_optionmenu_083.py`（静态守卫 083 依据注释 / 行为字面量逐字不动 / LoginBackground 保留 + 场景：12 格映射 / 083 vs +28 滑条 / 按钮 47x19 与 396 高 / UIWindow2-StatusBar3 0 命中）+ `gen_opcode_matrix.py --check`（GATE OK）+ `build_smoke.bat`（EXITCODE=0 真重编覆盖 UIOptionMenu.cpp） | ✅ `#devfix-optionmenu-083-review`（round71）；真机 5 项进 `pending-live-tests` §37；**决策点 D-B 待用户裁决** |

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

**下一步**：§3 决策闸门已于 2026-11-10 由 m04292 裁决——L-B 全部「保持 s9 优先，无变更」（见 §11.1）。Wave 0–2 全部 L-A 项已 ✅ / 核验完成（见 §11.2），无遗留实现项。剩余仅 `docs/features/pending-live-tests/README.md` 真机观感验收（需用户启 s9 + 截屏）。复现命令同上。

---

## 11. 收尾销案记录（2026-11-10 · m04292）

### 11.1 L-B 决策闸门已裁决
用户 m04292 于 2026-11-10 采纳 §3 推荐默认：**L-B 全部 12 项「保持 s9 优先、不补 083 发包、不改协议、保留资源降级、不建提交」**。即 Wave 3 清单（Quest 107 x/y、场地伤害族 `0xBF/0xC0/0xC1/0xC2/0xD4`、0x04/0x09/0x38/0x1C、`PARTY_SEARCH_REGISTER(220)`、`MTS_OPERATION(0xFD)`、`CUSTOM_PACKET(0x3713)`、Title 称号窗、扩展装备栏 #38、每日签到双端、`trade-house-083`、`0x3730→0x3740`、`InPacket::available()` 类型）全部**销案为「无变更」**，不进入实现。依据：盲改会断 s9 联机；现状已与 s9 联机对齐。

### 11.2 计划内项目 ✅ 补标
| # | 结论 | 证据 |
| --- | --- | --- |
| 0-4 | ✅ 复核完成，无缺项：`tab_plate` 借 `UiAssetResolve::window("Item")["New"]`、`set_macro` 显式 `mex->deactivate()`、`send_spup` 资源缺失降级——三项标记均在 `UISkillBook.cpp` 就位（grep 核验），无代码改动 | `UISkillBook.cpp` 标记计数 + r05 §2 S8 |
| 2-1 | ✅ 核验完成：F 维核心节点（`CSEffect`/`CSList`/`Base` 等）运行时数据均在；缺失项（`CSMVPBanner`/`BestNew`/`CSGLChargeNX`/`CSPromotionBanner`）为精简数据 R6 缺资源，已加 `[R6]` 守卫并构建部署；像素级观感进 `pending-live-tests` | `CashShop_img.json`(1213) + `UICashShop.cpp` 守卫 + `build_smoke EXITCODE=0` |
| 2-2 | ✅ 核验完成，无代码改动：DamageSkin 随 A+B 落地；宠物亲密/饱食 gage 在部署数据无专用位图（`Pet_img.json` 为外观精灵、`UIWindow.img` 无 pet gage 节点）→ `ColorBox` 降级兜底为正确 R6；场标→真精灵 7/8 已落真对象；MapleTV 为守卫良好的喊话输入窗（计划「双屏轮播」系误称） | `Pet_img.json`/`UIWindow_img.json` + `UIMapleTV.cpp` 守卫 + pet README D9/F02/F03 |
| 2-3 | ✅ 代码闭环：SENDSIMPLE 键盘导航可视带守卫（`ensure_visible`）已落 `UINpcTalk.cpp`（round37 `#devfix-npctalk-visible`）；A11b–A11j 逐像素对齐属观感，进 `pending-live-tests` §16 | `UINpcTalk.cpp:21784` + `wave2-npctalk-recheck-20261109.md` |
| 2-4 | ✅ 代码闭环：Reactor idle/多帧循环 + hit 转场 + state 层 Hit 音效已落 `Reactor.cpp`（`reactor-default-idle`）；隐藏门三态/动画观感进 `pending-live-tests` §18 | `Reactor.cpp` + `reactor-default-idle/README.md` |
| 2-19 | ✅ 已提交（`d6029c63`）：`UIChatBar.cpp` 两处 `input_text` 构造均加 `set_accept_unicode(true)`，中文 IME 可入聊天 | `test_d3_chat_unicode.py` + git `d6029c63` |
| 2-20 | ✅ 已提交（`fef8a56d`）：`UIMessenger.cpp` 095→083 地址替换 + `UIKeyConfig.cpp` 三处确认文案 i18n（locale/zh-CN.ini + en-US.ini） | `test_round55_d1_d14.py` + git `fef8a56d` |

### 11.3 无残留确认
- Wave 0（0-1~0-5）、Wave 1（1-1~1-7）、Wave 2（2-1~2-20）全部 ✅ 或销案为「无变更/已核验」；无遗留未决实现项。
- L-B（Wave 3）已裁决为「保持 s9 优先，无变更」——无实现项。
- **口径差异披露（round66，需用户追认）**：§11.1 把「L-B 全部 12 项保持 s9 优先、无变更」记为 m04292 裁决，但 m04292 原文只授权「开发和修复、自测、测试完成提交、不能自测的记录下来一起测」，并未逐条裁决跨面 D 矩阵；此前挂着的 K3+K4 64 条（残差 28）待裁决被整体销案。本轮不改动该裁决口径。另 §11.2「2-1 ✅ 已构建部署」写就时 `UICashShop.cpp` R6 守卫尚未提交、未进构建日志，本轮真重编已覆盖（32 次），口径现与现场对齐。
- 仅余 `docs/features/pending-live-tests/README.md` 中真机观感项（A+B 伤害数字/皮肤、2-1~2-4 像素级、各类 UI 实机点击）需用户启 s9 + 截屏联测，不属代码缺口。

## 10. 性质标注

本文件是**开发/修复计划与决策清单**（校准证据 → 缺口分层 → 一次性 Plan → 分批提交与自测边界），非实现提交、非评分表。2026-11-10 收尾更新（§11）：仅文档补充 L-B 裁决与 Wave 0–2 ✅ 补标，未动游戏源码/协议/WZ/IMG/XML。目标 `goal-11388c42` 保持 active；实现工作按 §4 分批续作。
