# 用户待测清单（真机统一验收）

> 维护约定：测不了/未测的写这里，步骤列全；已闭合标 ✅ + 日期 + 证据路径。  
> 2026-11-05 17:03 用户解除「不做桌面点测」约束，已完成一轮自动驱动真机验收（见下方记录）；未能自动触发的项仍需手测。

相关功能目录：[world-map-players](../world-map-players/) · [use-return-scroll](../use-return-scroll/) · [softkey-tails](../softkey-tails/) · [set-hpmpalert](../set-hpmpalert/) · [pick-all-char](../pick-all-char/) · [heal-over-time](../heal-over-time/) · [touch-monster-attack](../touch-monster-attack/) · [cancel-debuff](../cancel-debuff/) · [guild-bbs-ui](../guild-bbs-ui/) · [guild-boss-hud](../guild-boss-hud/) · [skill-effect-npc-guide-c2s](../skill-effect-npc-guide-c2s/) · [hidden-portal-tristate](../hidden-portal-tristate/)

---

## 汇总

| # | 项 | 状态 | opcode | 备注 |
| --- | --- | --- | --- | --- |
| 1 | WORLD_MAP_PLAYERS | ✅ 2026-11-05 | c2s 277 / s2c 376 | 已真机闭环，证据见下方「本次真机验收记录」；同图 0 玩家符合预期 |
| 2 | USE_RETURN_SCROLL | ⏳ 待测 | c2s 85 | 回城卷双击 |
| 3 | SoftKey：登录遮罩截图 | ✅ 2026-11-05 | — | 根因 `screenshot_directory()` 已修并真机出图；见 `notes/scrolllock-rootcause.md` 与 `evidence/live-20261105-scrolllock-proof.png` |
| 4 | SoftKey：PersonalShop FM | ⏳ 待测 | 雇佣店创建 | 需自由市场 + 许可证 |
| 5 | SoftKey：Messenger 立绘 | ⏳ 待测 | s2c 313 mode1 | 本机 look + 脚点 0.70；代码已闭环 |
| 5b | Messenger blink / chatBalloon | ⏳ 待测 | s2c 313 mode6 | 打字泡 + 标题 blink；见 §5b |
| 6 | SoftKey：Guild BBS 摘要 | ⏳ 待测 | BBS 155 | 公会窗摘要栏（兼容） |
| 7 | SET_HPMPALERT | ⏳ 待测 | c2s 4096 / s2c 扩展 | Wave1 已接线；端到端需服务端配置 |
| 8 | PICK_ALL_CHAR | ⏳ 待测 | c2s 14 / 32 | VAC 选角（本轮修正误发 19） |
| 9 | HEAL_OVER_TIME | ⏳ 待测 | c2s 89 | 自然回复上报 |
| 10 | TOUCH_MONSTER_ATTACK | ⏳ 待测 | c2s 47 | 身体压力/能量满触怪 |
| 11 | CANCEL_DEBUFF | ⏳ 待测 | c2s 99 | s9 handler 空实现；仅协议对齐 |
| 12 | GuildBBS 完整窗 | ⏳ 待测 | c2s 155 / s2c 59 | `UIGuildBBS` 列表/读帖/发帖 |
| 13 | 公会 Boss HUD+物件 | ⏳ 待测 | s2c 287 / 288 | `UIGuildBoss` + `MapGuildBoss` |
| 14 | CharInfo SoftKey 便签 | ⏳ 待测 | c2s 131 | `N` → NoteSend；需商城便签额度 |
| 15 | SKILL_EFFECT 起手广播 | ⏳ 待测 | c2s 93 | 飓风/大炮等白名单技；他人应见特效 |
| 16 | NPC_ACTION 对话姿态 | ⏳ 待测 | c2s 197 | 点 NPC 对话；他人侧 NPC 应有动作 |
| 17 | CLICK_GUIDE | ⏳ 待测 | c2s 162 | SPAWN_GUIDE 后点引导场标 → MIMO/LILIN |
| 18 | 隐藏门三态动画 | ⏳ 待测 | — | 靠近/离开隐藏门看 Start/Continue/Exit |
| 19 | NPC 对话 A11b+A11c | ⏳ 待测 | c2s NPC_TALK_MORE | 下项发 selection；α/TEXT_Y/Next=`W-78`；见 npc-talk-utildlgex-align |
| 22 | NPC tip 817–821 + Pet inset A11g | ⏳ 待测 | — | 校验 tip 样式；Pet speaker=2 输入 inset=25 |
| 23 | ScriptMessage OnPacket + tip 820 A11h | ⏳ 待测 | s2c 304 | 0/1/2/3/4/12 回归；未接 UI `-1` 收口；tip 820 需人为 min_digits>0 |
| 24 | AskAvatar / sendStyle A11i | ⏳ 待测 | s2c 304 / c2s 60 | UtilDlgEx_Avatar 选样式；OK `(7,1,sel)`；Cancel `(7,0)` |
| 25 | AskBoxText A11j | ⏳ 待测 | s2c 304 / c2s 60 | `sendAskBoxText`；OK `(13,1,str)`；Cancel `(13,0)`；单行局限 |
| 29 | G11 场标贴图 | ⏳ 待测 | s2c Catch/Chalk/CharBox/ItemFX | Catch 成败圈；黑板 sample；店招 signboard；ItemEff 无英文名牌 |
| 30 | 风筝 iconReward | ⏳ 待测 | s2c SPAWN_KITE 271 | 508 风筝动画 + 留言白字；收起/无法放置 |

---


## 第二轮验收记录（2026-11-05 18:30–19:05，Plan 续作）

> 范围：用户授权「全量闭环」+ 自备 dbx MCP + 一并修截图键。截图键修复已于 `b8559ee` 原子提交（#softkey-tails）。

### 已闭环

| 项 | 结论 | 证据 |
| --- | --- | --- |
| SoftKey 登录遮罩截图（#3） | ✅ 根因 = `IO/Window.cpp::screenshot_directory()` 对**已存在**的 exe 目录调用 `SDL_CreateDirectory` 失败即 `return ""`，请求全链路正常但从不落盘；改为 best-effort 创建并补 `SHOT` 节点日志；另加 `GLFW_KEY_PRINT_SCREEN` 等价键 | `notes/scrolllock-rootcause.md`；`boot_perf.log` 的 `request kc=281 → pending → consume → enter → dirfn → dir → write → saved`；`evidence/live-20261105-scrolllock-proof.png`（源 `MapleStory-20261105_185119.png`，1,112,547 字节） |
| dbx 通道与只读基线 | ✅ stdio 直连用户自备 MCP（18 个工具，连接 `localhost/beidou`），导出角色/配置/键位/道具快照 | `notes/character-snapshot-before.json`、`notes/db-change-list.md` |
| 构建与部署 | ✅ `tools/build_smoke.bat` 多次 `EXITCODE=0`；18:50:34 部署 exe 含修复（二进制含 `SHOT`、无旧 `screenshot: cannot create`）；19:02 并行会话重建后修复仍在 | — |

### 仍待测 / 阻塞（显式预留）

| 阻塞 | 影响项 | 补验路径 / 责任方 |
| --- | --- | --- |
| 聊天框 GM 命令自动注入失败：`!` 与空格被 `IO/UI.cpp:264-269` 的文本映射吞掉；`SendKeys` / 剪贴板 `Ctrl+V` / Unicode `SendInput` 三种方式均未产生 `0x31` 命令包（只出现普通聊天 `level30` 或动作包 `2c 00`） | #2/#9/#10/#11、原 harness 遗留 4 项（手雷 `0x6D` / 一键 AP `0x58` / Buff 取消 / 书袋双击）、#15/#16 等需等级/技能/道具前置的项 | **用户手输** `notes/gm-prereq-commands.md` 的命令补齐前置后点验；或授权执行 `notes/db-change-list.md` 的逐条 DML 后重登再自动验 |
| 需第二角色 / 公会 / 自由市场 / 商城额度 | #4/#5/#5b/#6/#8/#12/#13/#14 | 需受控 DML 造数据（清单已列、**未执行**），由用户逐条确认 |
| `game_config.use_server_auto_pot=false` | #7 `SET_HPMPALERT(4096)` 端到端回灌 | 优先管理端 8686 `/config/{latest}/updateConfig`；改后若需重启 s9 由用户执行 |
| 并行会话高频改同一仓库 / 重建 exe / 结束客户端进程（17:05–19:02 连续提交，19:02:23 重建 exe），后段新客户端登录后未进频道 | 所有需长时间稳定客户端会话的项 | 待并行会话收工后重跑；本轮已避让其 `UINpcTalk` / `UIMessenger` / `LoginBackground` 等在途文件 |
| 快照角色仍 lv1 / job0 / ap0 / 装备 8 件，无回城卷/技能书/召唤袋 | #2 及依赖道具的项 | 同 GM / DML 路径 |

### 环境收尾

`xy-config.yaml` 已按备份逐字节还原（`login.auto=false`、`debug.*` 全关）；调试日志作为证据保留。临时驱动脚本集中在本轮工作区 `c:\Users\11036\CodeBuddy\20261103175716\_*`（批量清理审批超时未执行，可手动删除）；项目仓库内未留临时文件。

---

## 本次真机验收记录（2026-11-05 17:03，自动驱动）

> 环境：s9 登录服 `127.0.0.1:8484`（java PID 6968）；部署 exe 时间戳 `16:43:32`；
> 验收期临时开启 `login.auto` / `debug.packet_dump` / `debug.perf_log` / `debug.gm_smoke`，验收后已还原。
> 证据：`evidence/live-20261105-*.png`；封包 `packet_dump.log`、探针 `boot_perf.log` 的本次增量。

### 已闭环

| 项 | 证据 | 结论 |
| --- | --- | --- |
| WORLD_MAP_PLAYERS | `[SEND] 15 01 e5 6e 51 06`(277) → `[RECV] 78 01 e5 6e 51 06 00 00`(376)；`live-20261105-mapclick3.png` | ✅ 请求/回包同 mapId，UI 玩家区 `0 players`（同图仅自己，符合预期）；怪物/掉落面板正常，说明 `ensure_map` 懒加载生效 |
| 登录链路 | 本次 SEND 73 包，opcode 1/5/11/19/20/24/41×63/48/128/189/207 | ✅ 零手动全自动 登录→世界→频道→选角→进图 Henesys |
| gm_smoke 自驱动 | `[PERF] SMOKE summon mob=100100 x3 map=100000000`；`[SEND] 80 00 17 04 87 01 00 03 00 00 00` | ✅ |
| AUTO_AGGRO | `[SEND] bd 00 20 ca 9a 3b`(189) | ✅ 刷怪后碰撞上报 |
| 四窗渲染 | `live-20261105-worldmap*.png` / `stats.png` / `skillbook.png` / `bag.png` | ✅ W/I/S/K 均正常开窗渲染，无崩溃 |

### 前置体检（原 harness 会话 `e2643c27` 卡住未采集到的输出，本次补齐）

```text
[PERF] t=31244ms SMOKE probe ap=0 lv=1 job=0
[PERF] t=31244ms SMOKE skill grenade=0 poisonbomb=0
[PERF] t=31244ms SMOKE summon mob=100100 x3 map=100000000
```

角色 `Admin` 为 lv1 新手：AP=0、无手雷/毒弹技能、技能点 0、背包 0 件道具（`live-20261105-bag.png`）、HP/MP 满（50/50、5/5）。

### 仍待测 / 阻塞原因（均非代码缺陷）

- 原 harness 遗留 4 项：手雷 `0x6D`、一键 AP `0x58`、Buff 取消、书袋双击 —— 技能等级/AP/道具/buff 源全缺，需 GM 改号或换有前置的角色（**改角色数据属写库，未获授权，未做**）。
- `HEAL_OVER_TIME(89)`：HP/MP 满，`Gameplay/Stage.cpp:403` 在 `heal_hp==0 && heal_mp==0` 时直接 return，不发属预期；需掉血或高 `info/recovery` 图。
- `TOUCH_MONSTER_ATTACK(47)`：无 BODY_PRESSURE / ENERGY_CHARGE；`CANCEL_DEBUFF(99)`：无疾病 buff。
- `USE_RETURN_SCROLL(85)` / `USE_SKILL_BOOK(82)` / 召唤袋：背包空，无对应道具。
- `PICK_ALL_CHAR(14/32)`、SoftKey 各尾巴、GuildBBS 完整窗、公会 Boss HUD、`SET_HPMPALERT` 回灌：需对应场景/道具/公会/服务端配置。
- SoftKey「截图键」：自动注入 `{SCROLLLOCK}` 未产出 `MapleStory-*.png`；W/I/S/K 注入均生效，判为特殊键映射未触达而非注入失效，**请手按 SCROLL_LOCK 复核**。

---

## 1. WORLD_MAP_PLAYERS（大地图玩家列表）

- **前置**：客户端已登录进图；同图或其它频道有 ≥1 在线角色；服务端 `feature/integration_client`。
- **操作**：
  1. 打开世界地图（默认 `W` / 小地图入口）。
  2. 点选一个有人的地图节点。
  3. 观察右上「在线: N」列表（名/等级/频道）。
  4. 切换到另一节点 → 旧列表应清空。
  5. SoftKey `L` 手动刷新当前点。
- **期望**：选中 mapId 与回包一致；陈旧回包丢弃；人数 ≤40。
- **道具/地图/opcode**：任意城镇即可；c2s `WORLD_MAP_PLAYERS=277` body=`int mapId`；s2c `376`。

## 2. USE_RETURN_SCROLL（回城卷）

- **前置**：背包消耗栏有 `2030000..2030099` 回城/传送卷（不要用 `2030100` 防驱逐卷）。
- **操作**：
  1. 打开背包 → 双击回城卷。
  2. （可选）服务端 `!showpackets` 确认收包头 `0x55`。
- **期望**：扣 1 个道具并传送回城；opcode **85** 而非 72。
- **道具/地图/opcode**：道具 `2030xxx`；任意野外图；c2s `USE_RETURN_SCROLL=85`（body 同 USE_ITEM：ts/slot/itemId）。

## 3–6. SoftKey 尾巴

### 3. 登录遮罩截图
- **代码**：knobs 已锁（`r201-login-overlay-playbook`）；本轮补登录链全局 `SCROLL_LOCK`→截图（此前仅 `UIStateGame` 响应，登录页按键无落盘）。叠图偏移仍靠截图验收，不改 knob。
- **前置**：当前部署端 `E:\MXD\BeiDou-Client-xy`；分辨率建议 **1280×720**；关闭自动登录以便停在各页。
- **操作**：
  1. 启动客户端，依次停在：登录页 → 选世界/频道页（含选中态）→ 选角页 → 进游戏后状态栏页。
  2. 每页按 **SCROLL_LOCK** 截一张；文件落到可执行目录或桌面（`ui.sysopt.screenshot_path` 0/1），文件名形如 `MapleStory-*.png`。
  3. 将 PNG 复制到 `docs/features/beidou-v17-parity/notes/evidence/r20/current_shots/`，文件名对齐：
     - `登录界面.png`
     - `选择频道页.png` / `选中频道页.png`
     - `选择角色页.png`
     - `进入游戏页.png`
  4. 与同目录上级 `correct_shots/` 半透明叠图；仅偏差项按 playbook 改对应 knob（勿盲改 `CS_DETAIL_*` / Wait bar）。
  5.（可选）叠图无偏差则本项标 ✅，把叠图证据放进 `current_shots/` 旁 `overlay_notes.md` 一行结论即可。
- **期望**：四页与 correct 叠图可接受；截图键有时间戳文件 + 一条成功日志。
- **对照**：`notes/r201-login-overlay-playbook.md` · `evidence/r20/current_shots/README.md`

### 4. PersonalShop FM
- **前置**：角色在**自由市场房间**（`910xxxxxx` 且 `>910000000`，入口 `910000000` 不算）；现金栏持有 `514xxxx`（个人店）或 `503xxxx`（雇佣商人）。
- **操作**：
  1. （负向）在非 FM 图双击许可证或 SoftKey 店名 → 应红字 tip「Stores only in the free market」，**不发包**。
  2. 进 FM 房间；双击 `514*` → 开 `UIPersonalShop` SoftKey；或开 PersonalShop 后 `C`/SoftKey 输入店名。
  3. 确认出站 `PLAYER_INTERACTION(123)` CREATE：`00 | createType(4=514/5=503) | mapleStr title | 00 00 00 | int cashItemId`。
  4. 成功：个人店应开 `UIMerchant` 货架（ROOM sub=4）；雇佣商人 ROOM sub=5；店名 tip 更新。
  5. （可选）非 FM 若绕过客户端仍发包，服务端回 MiniRoomError 15 → 红字 tip，**不得**误开交易窗。
- **期望**：仅 FM 可 CREATE；错误码 15 可见；店面/货架出现。
- **功能文档**：[softkey-tails](../softkey-tails/) · [notes/personalshop-fm-2026-11-05.md](../softkey-tails/notes/personalshop-fm-2026-11-05.md)

### 5. Messenger 立绘
- **前置**：已登录进图；`UIWindow.img/Messenger` 资源可用；建议再开第二角色测对方立绘。
- **操作**：
  1. 快捷键开 Messenger（状态栏社交 / `V` 等入口，以当前键位为准）→ 应自动建房。
  2. **本机**：0 号 NameBar（灰，左上）上方应立即出现本机 STAND1 立绘（scale≈0.70，脚点在名牌中线上沿）。
  3. SoftKey / 空槽点邀请第二角色 → 对方接受后，对应色 NameBar 上方出现对方立绘 + `Ch.N` 标签。
  4. 发一句聊天 → 下方白框有字；本机立绘不消失。
  5.（可选）抓包：建房后应收到 s2c `MESSENGER=313` **mode=1** `joinMessenger(pos=0)`；对方进房有 mode=0 `addMessengerPlayer`（含 look）。
  6. 截图存 `docs/features/softkey-tails/evidence/messenger-look/`（若脚点悬空/裁切，记偏移再微调 `LOOK_FOOT_*`）。
- **期望**：本机与对方立绘均可见；不悬空、不裁进标题条；关窗再开可重建。
- **道具/地图/opcode**：任意城镇；c2s `MESSENGER=122`；s2c `313`。
- **功能文档**：[softkey-tails](../softkey-tails/) · [notes/messenger-look-2026-11-05.md](../softkey-tails/notes/messenger-look-2026-11-05.md)

### 5b. Messenger blink / chatBalloon
- **前置**：双角色同 Messenger 房（或本机自测打字出站）；资源含 `Messenger/blink`、`Messenger/chatBalloon/0..4`。
- **操作**：
  1. A 开房，B 进房；B 在输入框打字（未回车）→ A 侧 B 立绘上方应出现白泡省略号动画（chatBalloon）。
  2. B 清空输入 / 发出消息 → 泡应停；A 标题条右上橙标（blink）应闪约一轮。
  3. A 发一句聊天 → 双方历史有字；可选抓包确认 typing 为 mode6 且 payload 为 `Name0`/`Name1`（无冒号）。
  4. 截图存 `docs/features/softkey-tails/evidence/messenger-blink/`（若泡位偏，记 `BALLOON_DX/DY`）。
- **期望**：打字泡不进聊天历史；真聊天进白框；无崩。
- **局限**：原生 blink 仅最小化标题条闪；本端开窗也会闪（适配）。
- **功能文档**：[notes/messenger-blink-chatballoon-2026-11-05.md](../softkey-tails/notes/messenger-blink-chatballoon-2026-11-05.md)

### 6. Guild BBS 摘要
- **前置**：已加入公会。
- **操作**：开公会窗 → 看 `[BBS]` 摘要栏（打开完整 BBS 窗后回包也会同步摘要）。
- **期望**：列表/帖摘要与公告分栏。
- **opcode**：c2s `BBS_OPERATION=155` / s2c `GUILD_BBS_PACKET=59`。

### 12. GuildBBS 完整窗（GuildBBS.img）
- **前置**：角色已加入公会；服务端 `bbs_threads` 可为空。
- **操作**：
  1. 打开公会窗 → 按 `B` → 应弹出 `GuildBBS.img` 大窗（734×526）并自动发 List(page=0)。
  2. 空库：列表为空、无崩；有帖：可见标题行与回复数；有公告（localid=0）时顶栏黄字 `[公告]`。
  3. 单击选中行 → 双击或 `O` → 进读帖页（subject/body/replies）。
  4. `W` 或 BtWrite → SoftKey 标题 → SoftKey 正文 → 发帖（mode 0）；`N` 发公告帖；`1/2/3` 选 icon。
  5. `E` / BtRetouch → 编辑选中帖；`D` 删帖；读帖页 `P` 回复、BtReplyDelete 删回复。
  6. `,` / `.` 或翻页钮切页；Esc 从写/读退回列表，再 Esc 关窗。
  7.（可选）抓包确认 List=`mode2`、Open=`mode3`、Post=`mode0`、Reply=`mode4`。
- **期望**：窗体贴图来自 `Data/UI/GuildBBS.img`；协议字段与 s9 `GuildPackets` 一致；公会窗摘要栏同步更新。
- **道具/地图/opcode**：任意城镇；c2s `155`；s2c `59` mode `0x06`/`0x07`。
- **功能文档**：[guild-bbs-ui](../guild-bbs-ui/)

### 13. 公会 Boss HUD + 地图物件（Healer / Pulley）
- **前置**：
  1. 进 **`990000900`**（莎莲娜王室，地图根有 `healer`/`pulley`）；或任意图仅测 HUD。
  2. s9 当前脚本**不发** `GuildBoss_*`（仅 `GuildPackets` 工厂）→ 需 GM/临时脚本 `broadcastPacket(GuildPackets.GuildBoss_HealerMove(y))` / `PulleyStateChange(state)`，或改 PQ 事件补发。
- **操作**：
  1. 进 `990000900` → 日志应有 `[GuildBoss] map healer path=Map/Obj/guild.img/...` 与 pulley；屏幕可见治疗精灵（约 x=295,y=-564）与滑轮（约 x=-59,y=-394）。
  2. 发 HealerMove(y)（建议 y 在 yMin=-564 .. yMax=-350）→ 精灵 Y 插值移动；顶栏 HUD 显示 `治疗精灵 Y=…`。
  3. 发 PulleyStateChange(state) → 日志 `pulley state=`；HUD 刷新；精灵帧仍循环（原生 state 只门控交互）。
  4. 12s 无新包 HUD 自动隐藏；Esc 立即关 HUD（物件仍在）。
  5. 确认 **不再** 往 QuestAlarm 灌 tip。
  6. 换到无 healer 节点的图再发包 → HUD 仍更新；物件侧 debug「no map object」。
- **期望**：Y/state 与包体一致；`990000900` 上精灵可见且随 287 移动；无崩溃。
- **opcode**：s2c `GUILD_BOSS_HEALER_MOVE=287` body=`short y`；`GUILD_BOSS_PULLEY_STATE_CHANGE=288` body=`byte state`。
- **功能文档**：[guild-boss-hud](../guild-boss-hud/)
- **预留**：pulley 交互上行（原生 `sub_558B45`）未接——无服务端消费证据。

## 14. CharInfo SoftKey 便签（NOTE_ACTION）

- **前置**：打开角色信息窗（点其他角色 / 自己）；s9 `NoteActionHandler` **仅当** `CashShop.getAvailableNotes()>0` 才落库（商城赠送便签额度）。
- **操作**：
  1. CharInfo 聚焦 → 按 `N`。
  2. 已有目标名：SoftKey 输入正文 → 发 `NOTE_ACTION=131` action0。
  3. 无目标名：SoftKey 先收件人名，再正文。
  4.（可选）`UIClaim` SoftKey 同包对照；抓包 body=`byte0 + mapleStr name + mapleStr msg`。
- **期望**：有额度时对方可在 Memo 收到；无额度时客户端仍发包、服务端静默跳过（不崩）。
- **opcode**：c2s `NOTE_ACTION=131`。
- **备注**：与 `UIClaim` 同走 `NoteSendPacket`；本项补的是 CharInfo 快捷键路径。

## 15. SKILL_EFFECT（充能/持续技起手）

- **前置**：角色已学白名单技之一（如弓手飓风 3121004、海盗手雷 5201002、奇攻 4211001）。
- **操作**：释放该技；抓包应有 `[SEND] …` opcode **93**；第二角色视角应见施法特效。
- **期望**：无崩；非白名单技不发 93。
- **局限**：无 KeyDown 条时只发起手一包（松开取消未接）。
- **文档**：[skill-effect-npc-guide-c2s](../skill-effect-npc-guide-c2s/)

## 16–17. NPC_ACTION / CLICK_GUIDE

- **16**：任意城镇点 NPC 对话 → 应先发 197（6B）再发 58；双端同图时对方可见 NPC 姿态。
- **17**：新手图有 SPAWN_GUIDE 时点引导标记 → 空包 162 → 开 MIMO/LILIN 脚本窗。
- **文档**：同上。
- **A11b「下项」（2026-11-05 已接线，待真机）**：`SENDSIMPLE` 右下「下项」应激活；悬停选项后点下项 / 回车 → 出站 `NPC_TALK_MORE` + selection（与点 `#L` 同包）；点 `#L` 直发仍通；宋体/A12M 不变。证据：[npc-talk-utildlgex-align](../npc-talk-utildlgex-align/)。
- **A11c 框体/正文/槽位（2026-11-05 已接线，待真机）**：α 保持 1.0；正文按 content 高居中；Next=`W-78`、OK=`W-54`、Next 纵向高于 Close 51px。证据：[npc-talk-utildlgex-align](../npc-talk-utildlgex-align/)。
- **A11d 高亮（2026-11-05 已接线，待真机）**：悬停/↑↓ 后选项下应有黑底线 + 灰虚线 `#919191`；字色仍 DARKGREY。证据：[npc-talk-utildlgex-align](../npc-talk-utildlgex-align/)。
- **A11e GetText/GetNumber（2026-11-05 已接线，待真机）**：正文下黑/白输入槽 + 内嵌光标；右下「确定」可点；OK/回车发包。证据：同上。
- **A11f Avatar + min/max（2026-11-05 已接线，待真机）**：`speaker=1` 无黑白框、Edit 约 226 宽；GetNumber 超 min/max 不发包不关窗。证据：[npc-talk-utildlgex-align](../npc-talk-utildlgex-align/evidence/a11f-gettext-avatar-minmax-ida.md)。
- **A11g tip + Pet inset（2026-11-05 已接线，待真机）**：校验失败弹 tip 817–821（A12 白+`#355A84` 描边）；Pet `speaker&6` 输入 inset=25。证据：[a11g](../npc-talk-utildlgex-align/evidence/a11g-tip-817-821-pet-inset-ida.md)。
- **A11h ScriptMessage OnPacket + tip 820（2026-11-05 已接线，待真机）**：见 §23。
- **仍预留**：KeyDown 持续技松开取消（无 cancel 上行证据）；AskPet 亲密/饱食原生位图条（无 gage 资源，ColorBox）。Mirror 动画底/BtMain 见 §28；Avatar UI 见 §24；Quiz UI 见 §26；SpeedQuiz UI 见 §27。

## 18. 隐藏门三态

- **前置**：含 `pt=HIDDEN` 的地图（常见教学/任务隐藏门）。
- **操作**：走近门 → 应播出现动画并循环；走开 → 播消失后隐藏。
- **文档**：[hidden-portal-tristate](../hidden-portal-tristate/)

## 19. NPC SENDSIMPLE「下项」A11b + 高亮 A11d

- **前置**：任意会走 `SENDSIMPLE`（可选项列表）的 NPC 脚本（如任务/传送多选）。
- **操作**：
  1. 打开对话后确认右下「下项」为激活态（非灰）。
  2. 鼠标悬停 / ↑↓ 不同选项 → 该项下出现黑 1px 底线 + 灰虚线；字色仍暗灰。
  3. 再点「下项」或按回车 → 应发 `NPC_TALK_MORE` 且 body 为当前高亮项 `selection`。
  4. 直接点 `#L…#l` 选项行 → 仍直发同包（不经下项）。
  5. 肉眼：正文 A11M/A12M 思源宋体；短文在正文区垂直居中；「下项」约在 `W-78`（勿与「确定」`W-54` 混淆）；框体不额外半透明；未回 SimHei。
- **期望**：服务端按 selection 推进脚本；无崩；关闭钮仍关窗。
- **opcode**：c2s `NPC_TALK_MORE`（既有 `NpcTalkMorePacket`）。
- **文档**：[npc-talk-utildlgex-align](../npc-talk-utildlgex-align/)
- **仍预留（非本项阻塞）**：KeyDown 持续技松开取消。

## 20. NPC GetText / GetNumber 布局 A11e

- **前置**：脚本 `cm.sendGetText` / `cm.sendGetNumber`（或等价 msgtype 2/3），`speaker=0`。
- **操作**：
  1. 打开后确认正文下方有黑边白底输入槽，光标可打字；右下「确定」激活、左下「关闭」仍在。
  2. 输入文本（GetText）或整数（GetNumber）→ 点确定或回车 → 出站 `NPC_TALK_MORE`（msgtype=2 带 string / msgtype=3 带 int）。
  3. 非数字（GetNumber）点确定 → 不应发包、窗保持可改。
  4. 肉眼：输入槽左缘约 x=157（相对窗）；宋体/A12M；非 Avatar 皮肤。
- **期望**：服务端收到文本/数字并推进；无崩。
- **opcode**：c2s `NPC_TALK_MORE`。
- **文档**：[npc-talk-utildlgex-align](../npc-talk-utildlgex-align/)

## 21. NPC GetText Avatar + GetNumber min/max A11f

- **前置**：
  - Avatar：`cm.sendGetText(text, speaker=1)` 或等价（玩家头像皮肤）。
  - min/max：`cm.sendGetNumber(text, def, min, max)`（如 min=1 max=100）。
- **操作**：
  1. Avatar：确认**无**黑白输入框，Edit 约在窗中偏下、宽约 226；「确定」约 x=158、「关闭」约 x=208（相对窗左）；宋体不变。
  2. GetNumber：输入 `<min` 或 `>max` 或含非数字 → 点确定/回车 → **不发包、窗不关**；输入范围内整数 → 发包并关窗。
  3. GetNumber 打开时输入框应预填 `def`。
  4. 非 Avatar GetText/Number 回归：黑白框与 A11e 行为仍在。
- **期望**：服务端仅在合法提交时推进；无崩；未回 SimHei。
- **opcode**：c2s `NPC_TALK_MORE`。
- **文档**：[npc-talk-utildlgex-align/evidence/a11f](../npc-talk-utildlgex-align/evidence/a11f-gettext-avatar-minmax-ida.md)
- **仍预留（非本项）**：见 §22 / ScriptMessage 全量 OnPacket。

## 22. NPC tip 817–821 + Pet inset A11g

- **前置**：
  - tip：任意 `sendGetNumber(def,min,max)` / 带最小长度的 `sendGetText`。
  - Pet：`speaker=2`（或 `(speaker&6)!=0`）的 GetText/GetNumber（非 Avatar 皮肤）。
- **操作**：
  1. GetNumber 输入字母 → 确定 → 弹 tip「Only numbers are allowed.»；对话窗不关；关 tip 后可继续改。
  2. 输入 `<min` → tip 含「higher than {min}」；`>max` → 「lower than {max}」。
  3. GetText 短于最小长度 → tip「at least {n} characters」。
  4. 肉眼 tip：A12 白字 + 蓝灰描边（`#355A84`），α 实心；宋体路径（非 SimHei）。
  5. Pet GetNumber：黑白输入槽左缘约 **x=25**（相对窗；非 157）；Avatar `speaker=1` 仍走固定槽、无黑白框。
- **期望**：非法提交不发包；合法仍发包关窗；无崩。
- **文档**：[a11g-tip-817-821-pet-inset-ida](../npc-talk-utildlgex-align/evidence/a11g-tip-817-821-pet-inset-ida.md)
- **仍预留**：Pet Say 正文 `TEXT_X`（无独立 inset 证据）。见 §23 tip 820 / OnPacket。

## 23. ScriptMessage OnPacket 全量 + tip 820 A11h

- **前置**：城镇可对话 NPC；可选 `cm.sendStyle` / Quiz / Mirror 脚本；GetNumber 普通路径即可（线路径 `number_min_digits=0`）。
- **操作 / 期望**：
  1. **回归 0/1/2/3/4/12**：Say / YesNo / GetText / GetNumber / Simple / AcceptDecline 仍正常开窗、发包、关窗；宋体/A12 不变。
  2. **Say `speaker&4`**：若脚本带 override npc，立绘应对齐覆盖后的 templateId（无此类脚本则跳过）。
  3. **GetText short×2**：s9 `writeInt(0)` 路径与原先一致（minLen=0 不弹 tip 821）。
  4. **GetNumber tip 820**：正常 `sendGetNumber` **不应**弹「more than %d digits」；若要肉眼验 tip 820，需临时把 `TalkInputParams::number_min_digits` 调为 >0（或调试断点改字段）再输入偏短数字串。
  5. **未接 UI 收口**：Mirror（14）、SpeedQuiz/Pet（6/9/10）→ 客户端应 `NpcTalkMore(-1)` 结束；日志 `reserved/no-UI`。Quiz（5）见 §26；AskBoxText（13）见 §25。
  6. **AskAvatar（A11i，另条）**：见 §24；本条不再把 sendStyle 当吞包项。
- **期望**：无崩；0–4/12 行为与 A11g 前一致；预留类型安全结束。
- **opcode**：s2c `NPC_TALK=304`；c2s `NPC_TALK_MORE`。
- **文档**：[a11h-scriptmessage-onpacket-ida](../npc-talk-utildlgex-align/evidence/a11h-scriptmessage-onpacket-ida.md)
- **仍预留**：Pet/Mirror 完整 UI；KeyDown cancel；SpeedQuiz 见 §27。

## 24. AskAvatar / sendStyle A11i

- **前置**：理发/整容/护肤 NPC（如 `cm.sendStyle`）；库存有对应优惠券更易走完脚本。
- **操作 / 期望**：
  1. 触发 `sendStyle` → 开 **UtilDlgEx_Avatar** 419×306 窗；正文宋体 A12M；角色预览在右下。
  2. **←/→ 或 BtPrev/BtNext** 切换样式，预览与名牌（非肤色）更新。
  3. **BtOff** 看原貌、**BtOn** 看新样式（互斥）。
  4. **BtOK2** → 上行 `(7,1,sel)`，脚本按 selection 继续；**Cancel2/Exit/ESC** → `(7,0)` 结束。
  5. MembershopAvatar（msgtype 8，若有脚本）同 UI，出站 lastMsg=8。
- **期望**：无崩；样式选择可走完；取消不挂脚本。
- **opcode**：s2c `NPC_TALK=304` msgtype7/8；c2s `NPC_TALK_MORE=60`。
- **文档**：[a11i-askavatar-sendstyle-ida](../npc-talk-utildlgex-align/evidence/a11i-askavatar-sendstyle-ida.md)
- **仍预留**：SpeedQuiz / Pet / Mirror UI。

## 25. AskBoxText A11j

- **前置**：脚本 `cm.sendAskBoxText(text, def, cols, rows)`（s9 本轮新增）；或等价发包 msgtype=13。
- **操作 / 期望**：
  1. 开 UtilDlgEx；正文下出现按 `6*cols` / `12*rows` 尺寸的输入框（黑/白框）。
  2. 输入中文/ASCII → **OK/Enter** → 上行 `(13,1,string)`；`getText()` 可读。
  3. **Close/Esc** → `(13,0)`，脚本结束不挂死。
  4. 多行 `rows>1`：框变高；本仓 Textfield 仍单行（↩ 提交，不插行）——已知局限。
- **期望**：无崩；文本回程进脚本。
- **opcode**：s2c 304 msgtype13；c2s 60。
- **文档**：[a11j-askboxtext-ida](../npc-talk-utildlgex-align/evidence/a11j-askboxtext-ida.md)
- **仍预留**：Pet/Mirror；BoxText 真多行换行；SpeedQuiz 见 §27。

## 26. InitialQuiz A11j（msgtype 5）

- **前置**：脚本 `cm.askQuiz(title, problem, hint, min, max, remainSec)`（s9 已补）；客户端有 `UIWindow.img/InitialQuiz`。
- **操作 / 期望**：
  1. 触发 → 开 **InitialQuiz** 288×229；标题/题面/提示宋体 A12M；倒计时 num1；输入槽可打字。
  2. 合法答案（len∈[min,max]）→ **OK/Enter** → 上行 **`5 + string`（无 action）**；`cm.getText()` 可读。
  3. 非法长度 → tip 英文明文（3920/3921 占位），**不关窗**。
  4. **超时 / Esc / Cancel** → 上行空串，脚本不挂死。
  5. **回归**：`cm.sendStyle` Avatar（msgtype 7）仍 `(7,1,sel)`；旧错误 msgtype 6/7 发包不再出现。
- **期望**：无崩；SpeedQuiz（6）见 §27。
- **opcode**：s2c 304 msgtype5；c2s 60 body=`byte5 + string`。
- **文档**：[a11j-quiz-speedquiz-ida](../npc-talk-utildlgex-align/evidence/a11j-quiz-speedquiz-ida.md)
- **仍预留**：Pet/Mirror。

## 27. SpeedQuiz A11k（msgtype 6）

- **前置**：脚本 `cm.askSpeedQuiz(type, answer, correct, remain, remainSec)`（s9 已补）；客户端有 `UIWindow.img/SpeedQuiz`。
- **操作 / 期望**：
  1. `type=0` → 开 **SpeedQuiz** 286×410，题面 `Npc/{answer:07d}/stand/0`；`type=1` → Mob；`type≥2` → 道具图标。
  2. 显示 `%2d pts`（correct）与余题 `%2d`（remain）；倒计时 num1。
  3. 输入 ≥2 字 → **OK/Enter** → 上行 **`6 + string`（无 action）**。
  4. **Next / 超时 / Esc** → 上行空串；**Giveup** → `"__GIVEUP__"`。
  5. **回归**：Quiz(5)、Avatar(7) 不受影响；场 OX 包走 FloatNotice。
- **期望**：无崩；答案进 `cm.getText()`。
- **opcode**：s2c 304 msgtype6；c2s 60 body=`byte6 + string`。
- **文档**：[a11k-speedquiz-ui-ida](../npc-talk-utildlgex-align/evidence/a11k-speedquiz-ui-ida.md)
- **Timer SFX**：✅ 已接（`MiniGame.img/Timer`，remain≤120 每秒）；原生音量斜坡未复刻（无逐次音量 API）。

## 28. NPC AskPet(9/10) + Mirror(14)

- **前置**：AskPet 需脚本发 msgtype 9/10（本仓 s9 少见）；Mirror 点维度镜 NPC `9010022`（`cm.sendDimensionalMirror`）。
- **操作 / 期望**：
  1. **Mirror**：开 SlideMenu 235×165；**底图多帧动画**；左右切目的地时 Choice 内 BtMain 图标切换；前往=BtMove → 上行 `14,1,int(id)`；取消/Esc → `14,0`。
  2. **AskPet**：UtilDlgEx_Pet 419×297；Prev/Next 轮播；stand0 立绘 + nameTag + Lv/亲密/饱食条；OK → `9|10,1,int64(uid)`；取消 → `action=0`；空列表 → `action=2`。
  3. 宋体 A12M；α 不改。
- **期望**：不再吞包 `-1`；脚本不挂死；现金宠等级/条在 uniqueId 命中时显示。
- **opcode**：s2c 304 msgtype 9/10/14；c2s 60。
- **文档**：[a11n-mirror-ani-btmain](../npc-talk-utildlgex-align/evidence/a11n-mirror-ani-btmain.md) · [a11m](../npc-talk-utildlgex-align/evidence/a11m-askpet-dlgtype6-full-ui.md) · [a11l](../npc-talk-utildlgex-align/evidence/a11l-askpet-mirror-ida.md)
- **仍预留**：AskPet 亲密/饱食原生位图条（无 gage 资源，ColorBox）。

## 7. SET_HPMPALERT

- **前置**：进游戏；系统设置可改 HP/MP 报警滑条。端到端回灌需 s9 `use_server_auto_pot=true`（**碰库，需用户授权**）。
- **操作**：
  1. 打开系统设置 → 调 HP/MP 报警百分比 → 确定。
  2. 查 `boot_perf.log` / debug：`[PERF] HPA out hp=… mp=…`。
  3.（可选开回灌）重登 → 期望 `[PERF] HPA hp=…`。
- **期望**：上行 step 0..19；本地闪铃仍按落盘百分比。
- **opcode**：c2s `SET_HPMPALERT=4096` body=`byte hp_step, byte mp_step`。

## 8. PICK_ALL_CHAR（VAC 选角）

- **前置**：登录后进入「查看全部角色」。
- **操作**：
  1. 点选角色 → BtEnter。
  2. 若该世界要 PIC → 输入 PIC 后确认。
- **期望**：进频道选角成功；抓包为 **14**（无 PIC）或 **32**（有 PIC），**不是** 19。
- **opcode**：`PICK_ALL_CHAR=14` body=`int cid, int world, macs, hwid`；`VIEW_ALL_WITH_PIC=32` body=`pic, cid, world, macs, hwid`。

## 9. HEAL_OVER_TIME

- **前置**：进图且存活；建议桑拿/高 recovery 图（`info/recovery>1`）更易观察。
- **操作**：站立等待 ≥10s → 查日志 `[Stage] HEAL_OVER_TIME` / `[PERF] HOT`；HP/MP 应上涨（未满时）。
- **期望**：healHP ≤ `77*recovery*1.5`；healMP∈(0,1000)。
- **opcode**：c2s `HEAL_OVER_TIME=89` body=`skip(8)+short hp+short mp`。

## 10. TOUCH_MONSTER_ATTACK

- **前置**：角色有 BODY_PRESSURE buff，**或** ENERGY_CHARGE≥15000；身边有怪。
- **操作**：贴近怪物碰撞 → 期望怪掉血；日志 `[Stage] TOUCH_MONSTER_ATTACK`。
- **期望**：服务端仅在上述条件结算；无条件发了也会被丢。
- **opcode**：c2s `TOUCH_MONSTER_ATTACK=47`（body 同 CLOSE_ATTACK / parseDamage）。

## 11. CANCEL_DEBUFF

- **前置**：身上有疾病（封印/虚弱/毒/黑暗/晕眩等），且客户端拿到了带 duration 的 GIVE_BUFF。
- **操作**：等待本地 duration 归零。
- **期望**：发空包 opcode 99；s9 **当前不处理**（空 handler），仅确认客户端有发送/无崩溃。真解除仍走药剂 USE_ITEM(72)。
- **opcode**：c2s `CANCEL_DEBUFF=99`（空 body）。
