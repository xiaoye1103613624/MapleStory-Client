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
| 5 | SoftKey：Messenger 立绘 | ✅ 本机+对方 | s2c 313 mode1 | 2026-11-06 p9：SoftKey Text KEY-TEXT 修复后邀请成功；`20261106_p9_*_both.png` |
| 5b | Messenger blink / chatBalloon | 🟡 同房已具备 | s2c 313 mode6 | p9 已同房打字；单帧未稳抓泡/blink → 手点补拍 |
| 5c | Messenger 关闭钮 (276,6) | ✅ 2026-11-06 | — | 真机点关白底归零；见 messenger-btclose-coords + dual 证据 |
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
| 31 | G10 金/银宝箱 | ⏳ 待测 | c2s UseTreasureChest / s2c 117 | 踩普通 Reactor 不弹窗；双击 4280000/1+钥匙见 opened 皮肤 |
| 32 | 结婚全链（wedding-flow） | ⏳ 待测 | c2s 137/138/139 / s2c 72/73/74/75/106/320/321 | 求婚→答复→请柬→礼台→解约 7 步，步骤见 [wedding-flow/README §4](../wedding-flow/README.md)；exe 已部署 23:16 |
| 33 | 联盟补完（invite/0x0D/公告钮） | ⏳ 待测 | c2s 143/144 / s2c 66 | 双公会：盟主邀请→对端 YesNo→0x0D 公会名列表→N 改公告；GameOpt UNION_APPLY 拒绝位生效 |
| 34 | 组队/家族 R28 回归抓帧 | ⏳ 待测 | s2c 62/95/96… | 冷启动登录链全程抓帧：UIParty/UIFamily/UIGuild/UIAlliance/UIWedding 不得出现在未主动打开的帧 |
| 35 | 五死键接线（F 学院/H 密语/G 家族共用窗/O 组队搜索） | ⏳ 待测 | — | 默认键位逐键按：`F`→学院(家族)窗开+自动刷新；`H`→「请输入悄悄话的对象」弹层；`G`→**「玩家列表 & 家族」共用窗第 3 页签「家族」**（2026-11-08 起，不再是独立 `UIGuild` 浮窗；浮窗仍由底栏社区钮打开）；`O`→组队搜索窗；`L`→无反应属预期（NOTIFIER 灭活留证）。证据见 ui-v83-window-restore/notes/f-keymap-action-translate.md 补记 + [userlist-five-tabs/notes/diagnosis-and-plan.md](../userlist-five-tabs/notes/diagnosis-and-plan.md)；exe 2026-11-08 01:41 已部署 |
| 36 | 玩家列表 & 家族五页签共用窗还原 | ⏳ 待测 | c2s BUDDYLIST(130) / PARTY_OPERATION(124) / GUILD_OPERATION / ALLIANCE_OPERATION(143) | 按 `G` 开窗后逐页复拍：好友（0/25 + 分组行 + 底部 12 钮）/ 组员（在线离线 0/0 + 9 钮）/ 家族（[0/0] 计数 + none 行 + 4x3 钮 + 滚动条）/ 家族联盟（还未加入联盟。+ 12 钮）/ 拒绝列表（0/26 + 增加删除）；TAB 循环五格；三张互斥底图不得叠底；按钮发包后对 s9 回包。oracle 帧见 `userlist-five-tabs/evidence/frames/`；exe 2026-11-08 01:41 已部署 |

---


---

## HUD 面（statusbar-hud，2026-11-10 新增，待真机验收）

> 客户端 `feature/dev_0.0.1`（D8/D9/D7/D2 客户端）与 s9 `feature/integration_client`（D2 服务端 mute/curse）已构建验证；以下运行时行为无真机无法自测，交用户联测。

| # | 项 | 状态 | opcode | 备注 |
| --- | --- | --- | --- | --- |
| 37 | D8 组队血条渲染 | ⏳ 待测 | s2c UPDATE_PARTYMEMBER_HP(0xC9) | s9 已下发同图组队成员 HP；客户端 `UIParty::draw` 新增血条（暗底+比例填充，≤30% 转红）；需两人+同图组队核验 |
| 38 | D9 主窗 4 钮闪烁 | ⏳ 待测 | — | `UIStatusBar` 切装备/背包/属性/技能时脉冲高亮（7.5Hz 半透明白块；原生 SP_1986..1989 KEY_ANI 无资源）；需真机看节奏 |
| 39 | D7 聊天三态 | ⏳ 待测 | — | `chat.type` 1/2/3 持久化 + 钳制 26..507；`BtMin` 完整→紧凑→收起、`BtMax` 逐级展开；需五屏核对三态 |
| 40 | D2 聊天规则联测 | ⏳ 待测 | c2s GENERAL_CHAT | 客户端 2000ms 限流 + 连续 4 同句 + 脏词脱敏；服务端 `chat_curse_words` 配置键（逗号分隔）+ `!mute`/`!unmute`(gm3)；需配词库 + 真机 |
| 41 | 字段伤害数字 + 伤害皮肤（A+B） | 🟡 已构建，真机待测 | s2c 246 / 247 | 同图他人/环境对怪伤害数字弹出（接回被丢弃的 DAMAGE_MONSTER damage，服务端零改）；伤害皮肤按 id 切 DamageSkin.img 字形，缺失回退 BasicEff；本地玩家皮肤由 DAMAGE_SKIN(247) 登录下发并用于自身攻击数字 |

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

### 5c. Messenger 关闭钮坐标
- **前置**：已登录进图；能开 Messenger。
- **操作**：开窗 → 看标题栏右上关闭钮是否完整落在窗内（勿越右沿）→ 点击应关窗。
- **期望**：钮位约窗口内 `(276,6)`（12×12 蓝底白叉）；不溢出、可点。
- **功能文档**：[messenger-btclose-coords](../messenger-btclose-coords/)

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
  5. **未接 UI 收口（`end_talk` 按服务端读包契约分派，2026-11-06 修复）**：Mirror（14）/Pet（9/10）→ `(msgtype,-1)`；GetText/AskBoxText（2/13）缺资源 → `(msgtype,0)`；Quiz/SpeedQuiz（5/6）缺资源 → `(5|6, "")` 空串。日志 `reserved/no-UI` / `end_talk`。旧版 10 处一律 `(x,-1)` 对 2/5/6/13 会踩 s9 `NPCMoreTalkHandler.readString()` 短包，已修；详见 [end-talk-read-contract](../npc-talk-utildlgex-align/evidence/end-talk-read-contract-2026-11-06.md)。
  6. **AskAvatar（A11i，另条）**：见 §24；本条不再把 sendStyle 当吞包项。
- **期望**：无崩；0–4/12 行为与 A11g 前一致；预留类型安全结束（2/13/5/6 不再发 2 字节短包）。
- **opcode**：s2c `NPC_TALK=304`；c2s `NPC_TALK_MORE`。
- **文档**：[a11h-scriptmessage-onpacket-ida](../npc-talk-utildlgex-align/evidence/a11h-scriptmessage-onpacket-ida.md) · [end-talk-read-contract](../npc-talk-utildlgex-align/evidence/end-talk-read-contract-2026-11-06.md)
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

## 31. G10 金/银宝箱（TreasureBox）

- **前置**：ETC 有 `4280000`/`4280001`，现金栏有对应钥匙 `5490000`/`5490001`；另备一张有普通 Reactor 的图。
- **操作 / 期望**：
  1. 踩/打普通 Reactor → **不应**弹出 TreasureBox 窗，也不应出现英文 `"Chest opened"` 场标；Reactor 动画/掉落照常。
  2. 双击金/银宝箱 → 出站 UseTreasureChest；成功后入站开 `UITreasureBox` opened 皮肤（金=variant0 / 银=variant1）。
  3. Esc 关窗；无钥匙/栏位满时服务端 tip，客户端不崩。
- **文档**：[g10-treasure-box-semantics](../g10-treasure-box-semantics/)

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

---

## 12. 2026-11-07「14 项回归」续作后仍需用户手点的项

> 自动回归（pass1~pass6 + 本轮新 exe 冒烟）已闭环的项见
> `ui-v83-window-restore/notes/regress-20261107-abifix.md` 与
> `regress-20261107-mapletv.md`；exe = `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
> （2026-11-07 10:49）。以下三项自动脚本无法覆盖，需手点：

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 10 MapleTV 双屏 | 进自由市场 `910000000`，走到两块木框黑板（NPC 9250025）前 | 上屏显示 MapleStory 台标，下屏播放轮播广告，均不越出木框 | 自动登录落在「番茄田/彩虹岛」，脚本没有换图能力 |
| 11 点击事件 | 点 NPC / 点角色 / 点玩家 | NPC 对话 / 角色信息窗 / 玩家信息窗 | pass2/4/5b/6 点击坐标均未命中目标，截图无对话框，属驱动坐标问题非功能结论 |
| 5 装备对比 | 穿上一件装备后悬浮另一件同类装备 | 已装备/对比装备排版与背景色按原生 | 测试号当前无可穿戴/已装备道具 |

另：开播态（使用 MapleTV Messenger 5075xxx 后 15~60s 的角色立绘 + 5 行文案）
属显式预留，当前 `SEND_TV` 只保留了提示窗，待 REA/IDA 取证后另立 Plan。

---

## 13. round34 世界选择键盘导航边界（#devfix-worldselect-guard，需用户一起测）

> 修复内容：`IO/UITypes/UIWorldSelect.cpp:send_key` 4 处守卫（重连越界读 /
> 0 频道 `std::div(x,0)` 除零 / 频道缩水越段 / Channel # SoftKey 延迟回调）。
> 自动自测 `tools/test_worldselect_guard.py` 只覆盖**静态守卫 + 算法复刻**（4 守卫 +
> 10 边界 PASS）+ 构建 GATE OK/EXITCODE=0；**时序竞态与 s9 畸形包必须真机联测**，
> 不冒充已验证。exe = `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2026-11-09 22:28 重编译）。
> 逐字证据见 `development-and-fix-plan-2026-11/notes/wave0-live-recheck-20261109.md`。

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 13-1 重连竞态 | 选世界进频道后，触发重新连接/新一轮 SERVERLIST，世界牌尚未重建瞬间按 上/下/左/右/TAB/回车 | 无崩溃、无非法频道按钮段跳转；按键被守卫安全吞掉 | 需真实登录服 + 掐准 `clear_worlds()` 与 `world_selected` 未复位的时序窗口 |
| 13-2 0 频道畸形 world | 构造/让服务端下发一个 `channel_total==0` 的世界，选中后按方向键、再按 ESC | 方向键无 SIGFPE；ESC 退回世界列表（`world_selected=false` + 面板复位） | 需服务端/代理灌畸形包，客户端本地无法伪造 |
| 13-3 频道数缩水 | 先记 high channel（如 ch20），让服务端把该世界频道数缩到 3，再按方向键 | `selected_channel` 收敛到 0，导航不越出 `rows`×`columns` | 需服务端下发变化的世界列表 |
| 13-4 Channel # SoftKey 延迟 | 弹出「Channel #」输入框后，让 SERVERLIST 刷新（`clear_worlds()`），再确认输入 | 无越界读崩溃，回调被守卫吞掉 | 需真机时序 + 输入框交互 |
| 13-5 常规回归 | 正常单频道 / 多频道（7、30）世界，上下左右/TAB 导航 + 回车 + 点同频道 `enter_world()` | 导航/选中/进世界与修复前一致，无回归 | 需登录服在跑 + 冷启动 + 手工按键 |

---

## 14. round35 选角页键盘导航边界（#devfix-charselect-guard，需用户一起测）

> 修复内容：`IO/UITypes/UICharSelect.cpp:send_key` 网格分支 2 处守卫（空列表早退 /
> `selected_character` 越界夹取），同构 round34 `72f1a6ce` 的 `UIWorldSelect` 守卫。
> 自动自测 `tools/test_charselect_guard.py` 只覆盖**静态守卫 + 算法复刻**
> （2 守卫 + 10 边界 PASS，守卫 A 空列表 presses 2→0 / 守卫 B 越界 misfire 2→0）
> + 构建 GATE OK/EXITCODE=0；**真机时序与登录服交互必须联测**，不冒充已验证。
> exe = `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2026-11-09 22:58 重编译）。
> 逐字证据见 `development-and-fix-plan-2026-11/notes/wave1-charselect-recheck-20261109.md`。

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 14-1 空列表按键 | 新账号（0 角色）进选角页，按 上/下/左/右/TAB/回车 | 不翻页、不弹无关窗、无崩溃 | 需登录服 + 0 角色账号，harness 只覆盖算法 |
| 14-2 删光残留 | 有角色账号删到只剩 1 个，删完瞬间按方向键 | 不误触发 BtPreview / 无越段跳转 | 需真机时序 + 删除链路 |
| 14-3 多页导航回归 | 4 / 7 / 15 角色翻页后上下左右 + 回车进游戏 | 导航/选中/进世界与修复前一致 | 需登录服在跑 + 冷启动 + 手工按键 |
| 14-4 重连竞态 | 选角页触发重连、列表未就绪瞬间按键 | 无崩溃、按键被守卫安全吞掉 | 需掐准 SERVERLIST 刷新窗口 |

---

## 15. round36 Textfield UTF-8 光标边界（#devfix-utf8-caret，需用户一起测）

> 修复内容：`IO/Components/Textfield.cpp:send_key` LEFT/RIGHT 补 UTF-8 码点
> 边界移动（`markerpos` 是字节索引，旧裸 `±1` 会落进多字节汉字续字节，
> 随后 `add_string:294 text.insert` 把字符劈成非法序列）。
> 自动自测 `tools/test_textfield_utf8_caret.py` 只覆盖**静态守卫 + 字节算法
> 复刻**（6 守卫 + 10 边界 PASS，before bad=28/split=5 → after bad=0，
> ASCII 回归逐字节一致）+ 构建 GATE OK/EXITCODE=0；**IME 真机观感与字图缓存
> 渲染必须联测**，不冒充已验证。exe = `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
> （2026-11-09 23:14 重编译）。逐字证据见
> `development-and-fix-plan-2026-11/notes/wave1-login-ime-recheck-20261109.md`。

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 15-1 CJK 左右键 | 地图搜索框输入「你好世界」，HOME 后连按 RIGHT 4 次再插「X」 | 光标每次跳过整个汉字，插入得「X你好世界」/「你X好世界」等，无乱码无半字 | harness 只复刻字节算法，真机需 IME + 字图缓存渲染 |
| 15-2 BACK/DELETE 组合 | 上一步后连按 LEFT + BACK / RIGHT + DELETE | 整字删除，不残留半字节、不误删相邻字 | 需真机输入法提交 + 回删链路 |
| 15-3 ASCII 回归 | 账号/密码/聊天框纯英文，左右键 + 插入 + 回删 | 与修复前逐字节一致，无回归 | 需登录服 + 冷启动手工按键（harness 已证算法等价） |
| 15-4 IME 预编辑 | 中文输入法候选态（未提交）看输入框 | 当前只收已提交串；预编辑态观感须另行取证（notes §5） | 需多输入法矩阵 + 真机截图 |

---

## 16. round37 NPC 对话 SENDSIMPLE 键盘导航可视带（#devfix-npctalk-visible，需用户一起测）

> 修复内容：`IO/UITypes/UINpcTalk.cpp:send_key` SENDSIMPLE 分支插入 `ensure_visible()`
> lambda（与 `draw():225-263` / `send_cursor():630-651` 同源可视带判据），UP/DOWN
> 后把选中项滚回视野并用 `slider.setrows(offset, unitrows, rowmax)` 同步 thumb；
> 修复前长菜单键盘可把高亮移到滚出可视带的行、回车 `dispatch_simple_selection()`
> 仍提交不可见项。 orange-wz 门禁按 AGENTS.md 先跑：结构层 `Data/UI children=20`
> WORKS / 内容层 4 次 `IMG 解析失败` FAILS（原文+65536B key 规格留证）→ 合规 `.nx`
> 回退 `UI.nx UtilDlgEx controls=24`。
> 自动自测 `tools/test_npctalk_simple_visible.py` 只覆盖**静态守卫 + 几何算法复刻**
> （7 守卫 + 10 场景 PASS，before bad=16 → after 0，no-slider/边界回归一致）+
> 构建 GATE OK/EXITCODE=0；**真机字图渲染、UtilDlgEx 资源、服务端 selection 回包
> 必须联测**，不冒充已验证。exe = `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
> （2026-11-10 00:08 重编译）。逐字证据见
> `development-and-fix-plan-2026-11/notes/wave2-npctalk-recheck-20261109.md`。

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 16-1 长菜单键盘 | 拍卖行 9900001 / 长 `#L` 列表对话，连按 DOWN 越过可视带 | 高亮虚线始终跟随、滑块 thumb 同步下滚、无「看不见却被选中」 | harness 只复刻几何，需真机字图渲染 + UtilDlgEx 资源 |
| 16-2 回车提交 | 16-1 末尾直接回车 | 提交的是当前高亮可见项，与点「下项」一致 | 需登录服 + 服务端脚本回包验证 selection |
| 16-3 回滚 UP | 16-1 后连按 UP 回首行 | offset 收敛回 0、thumb 归位、无残留滚动 | 需真机滑条控件状态 |
| 16-4 短菜单回归 | 2/3/5/8 项短对话（无滑条）上下键 + 回车 | 与修复前一致、无回归 | 需多个 NPC 脚本触发点 |

---

## 17. round38 · UIRaceSelect SoftKey 数字选职业可视窗口守卫（#devfix-raceselect-softkey，需用户一起测）

> 自动自测 `tools/test_raceselect_softkey_guard.py` 只覆盖**静态守卫 + bid 映射算法复刻**
> （4 守卫 + 10 边界 PASS，before bad=6 → after 0，合法职业按钮回归 4/4）+
> `GATE OK` + `build_smoke.bat` `EXITCODE=0`（00:36 真重编译）；
> **真机 SoftKey 输入、RaceSelect_new 资源、翻页后窗口、字图渲染必须联测**，不冒充已验证。
> exe = `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2026-11-10 00:36 重编译）。逐字证据见
> `development-and-fix-plan-2026-11/notes/wave2-raceselect-recheck-20261110.md`。

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 17-1 翻页后小号数字 | modern 职业选择页用右箭头翻过一页（index_shift>0），SoftKey 输 `0`/`2` | 不切世界、不开创建确认框、不翻页，输入被忽略 | harness 只复刻 bid 映射，需真机 SoftKey + UI 状态 |
| 17-2 大号数字越界 | 不翻页 SoftKey 输 `6`/`12`，或翻页后输窗口外 idx | 无 `class_index[]` 越界读、零崩溃、输入被忽略 | 需真机 RaceSelect_new 资源与内存安全表现 |
| 17-3 合法数字回归 | 当前可见窗口边界 `index_shift`/`index_shift+5` 及中间项输数字 + 回车 | 选中对应职业，高亮/音效与鼠标点击一致 | 需真机立绘/音效 |
| 17-4 classic 模式 | SoftKey `C` 切 classic 后输 `0/1/2` | 与修复前一致、无回归 | classic 分支未改，需真机三职业卡 |

---

## 18. round39 · UIGuildBBS 翻页上界守卫（#devfix-guildbbs-page，需用户一起测）

> 自动自测 `tools/test_guildbbs_page_guard.py` 只覆盖**静态守卫 + 翻页算法复刻**
> （5 守卫 + 10 边界 PASS，before bad=5 → after 0，合法满页翻页与 BtPageLeft 回归 4/4）+
> `GATE OK` + `build_smoke.bat` `EXITCODE=0`（00:52 真重编译）；
> **真机 BBS 回包分页、公告条是否占一行、末页按钮观感必须联测**，不冒充已验证。
> exe = `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2026-11-10 00:52 重编译）。逐字证据见
> `development-and-fix-plan-2026-11/notes/wave2-guildbbs-recheck-20261110.md`。

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 18-1 末页点下一页 | 公会 BBS 列表停在最后一页（本页不满 12 行），鼠标点 BtPageRight 或按 `.`/`]` | 不发越界页请求、窗口不空白、`selected_row` 不清成 -1、页码不跳到越界值 | harness 只复刻 `threads.size()` 判定，需真机 s9 BBS 回包 |
| 18-2 满页翻页回归 | 本页恰好 12 行时点「下一页」 | 正常请求下一页，与修复前一致、无回归 | 需真机多页数据 |
| 18-3 公告条占行 | 首屏有公告条（notice）时末页行数判定 | 若公告条额外占一行导致本页「满 12 但实为末页」，记录观感；本轮守卫用 `threads.size() >= LIST_MAX_ROWS` 偏保守，是否需按 `has_notice` 扣一行交用户裁决 | 需真机公告条布局 |
| 18-4 上一页/键盘回归 | 任意页点 BtPageLeft 或按 `,`/`[`；非 list 页按 ESC | 与修复前一致、无回归 | BtPageLeft 未改，需真机确认三页状态切换 |

---

## 19. round40 · 召唤兽 MOVE_SUMMON 影子缓存自愈（#devfix-summon-movepath，需用户一起测）

> 自动自测 `tools/test_summon_movepath_flood.py` 只覆盖**静态守卫 + 8 槽/键控状态机算法复刻**
> （10 守卫 + 10 场景 PASS，F 系 `before flood=59/29/119 → after 0`，累计 `207→0`，回归 7/7）+
> `GATE OK` + `build_smoke.bat` `EXITCODE=0`（01:32 真重编译）；
> **真机 MOVE_SUMMON 上行频率、召唤兽跟随观感、换图回收、多召唤不串扰必须联测**，不冒充已验证。
> exe = `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2026-11-10 01:32 重编译）。逐字证据见
> `development-and-fix-plan-2026-11/notes/wave2-summon-movepath-recheck-20261110.md`。
> orange-wz 资源层门禁：`Data/Skill`(76)/`Effect`(17)/`Npc`(7198) 结构层 WORKS、内容层 6 次 `IMG 解析失败` → 合规 `.nx` 回退留原文+65536B GMS key 规格。

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 19-1 僵尸洪泛 | 连续召唤→死亡 8 次以上（撑满旧 8 槽），再召一只**静止**召唤兽站立不动，抓上行包 30s | `MOVE_SUMMON` 只在召唤首帧/真实位移时出现，不再每帧刷（旧版 ≈60/s） | harness 只复刻状态机，需真机服务器抓包 |
| 19-2 正常移动 | 召唤兽跟随玩家移动 / 远离 120·80 阈值被拉近 | 每个真实位移仍正常发一次 `MOVE_SUMMON`，跟随不卡住 | 需真机位置/物理 |
| 19-3 换图 clear | 带召唤兽换图后重新召唤 | `clear()` 清缓存、新图首帧正常登记，无残留 oid 串扰 | 需真机换图流程 |
| 19-4 多召唤 + 他人召唤 | 同时存在自己多只召唤兽 + 他人召唤兽 | 裁剪判据 `get_owner()!=local_cid` 生效，只上报自己的、互不串扰、无越界 | 需多角色/多人联机 |

---

## 20. round41 · UIItemInventory 重复 G 键入口遮蔽修复（#devfix-iteminventory-gkey，需用户一起测）

> 自动自测 `tools/test_iteminventory_gkey.py` 只覆盖**静态守卫 + else-if 优先链算法复刻**
> （10 守卫 + 10 键位 case PASS，before Gacha reach=0→after 2，duplicate G 2→1，回归 8/8）+
> `GATE OK` + `build_smoke.bat` `EXITCODE=0`（01:50 真重编译）；
> **真机 G/g 整理、X/x 触发 GACHA_EXP、旧键回归与 X 键语义必须联测**，不冒充已验证。
> exe = `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2026-11-10 01:50 重编译）。逐字证据见
> `development-and-fix-plan-2026-11/notes/wave2-iteminventory-recheck-20261110.md`。

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 20-1 G/g 整理回归 | 背包打开按 G 或 g | 与修复前一致，执行整理（GatherItemsPacket），X 分支不抢 | harness 只复刻键位链，需真机服务器响应 |
| 20-2 X/x 触发 GACHA_EXP | 背包打开按 X 或 x | 发送 `USE_GACHA_EXP(158)` 空包，服务端按 GachaExp 道具态响应 | 需真机账号持有 GachaExp 道具 + 服务端闭环 |
| 20-3 旧键回归 | O/U/D/S/Y/E 等既有单键 | 与修复前一致、无串扰 | 需真机各功能路径 |
| 20-4 X 键语义裁决 | 确认 X 是否与其他全局快捷键/客户端功能冲突 | 若冲突，交用户裁决是否迁到菜单按钮或其它键 | 需用户确认最终键位意图；旧账 r803 曾写 G→GachaExp，本次按 live 主路径让 G 归整理 |

---

## 21. round42 · UICommonCreation avatarSel 空节点下溢守卫（#devfix-commoncreation-avatar，需用户一起测）
> 编号说明：本节沿用 round 链 §19(round40) / §20(round41) / §21(round42)，与前文 A11f 的 §21（`:298`）按标题中的 roundNN 区分；历史编号原样保留，不回溯改动。

> 自动自测 `tools/test_commoncreation_avatar_guard.py` 只覆盖**静态守卫 + 修复前/后循环上界算法复刻**
> （9 守卫 + 8 场景 PASS，before HANG 1→after 0，回归 7/7）+
> `GATE OK` + `build_smoke.bat` `EXITCODE=0`（02:22 真重编译）；
> **真机 modern/classic 新建角色、资源裁剪态不挂死与各职业 avatarSel 子节点观感必须联测**，不冒充已验证。
> exe = `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2026-11-10 02:22 重编译）。逐字证据见
> `development-and-fix-plan-2026-11/notes/wave2-commoncreation-recheck-20261110.md`。

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 21-1 modern 新建角色正常态 | 冒险家 / 战神 / 骑士团各进 modern 创角界面 | 外观预览层数与修复前一致（跳过最后一个子节点的语义不变），不闪退不空白 | harness 只复刻循环上界，需真机 WZ 资源渲染 |
| 21-2 资源裁剪态不挂 | 用缺 `avatarSel` 节点的裁剪资源包进 modern 创角 | 构造期不挂死，窗口仍能加载（空贴图路径），可正常退出 | 需真机裁剪 WZ + 计时观察；自动测无法构造真 `nl::node` |
| 21-3 classic 回归 | classic 新建角色流程 | `if (!NewChar)` 门控行为不变，与修复前一致 | 需真机 classic 资源分支 |
| 21-4 各职业子节点排布 | 对比 adventurer / aran / cygnusKnight 的 `avatarSel` 子节点排布 | `t` 偏移与纵坐标与修复前一致，无错位 | 需真机逐职业像素观感 |

---

## 22. round43 · UIOptionMenu sButtonUOL 空串守卫（#devfix-optionmenu-combo，需用户一起测）
> 编号说明：本节沿用 round 链 §19(round40) / §20(round41) / §21(round42) / §22(round43)，与前文 A11g 的 §22（`:313`）及 A11f 的 §21（`:298`）按标题中的 roundNN 区分；历史编号原样保留，不回溯改动。

> 自动自测 `tools/test_optionmenu_combo_guard.py` 只覆盖**静态守卫 + 修复前/后转换算法复刻**
> （12 守卫 + 13 场景 PASS，before UB/THROW 4→after 0，回归 9/9）+
> `GATE OK` + `build_smoke.bat` `EXITCODE=0`（02:43 真重编译 4962B / ERROR_LINES 0）；
> **真机设置窗下拉框外观、资源裁剪态不崩且回退 DEFAULT、分辨率切换与各数字后缀观感必须联测**，不冒充已验证。
> exe = `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2026-11-10 02:43 重编译）。逐字证据见
> `development-and-fix-plan-2026-11/notes/wave2-optionmenu-recheck-20261110.md`。

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 22-1 设置窗正常态 | 打开选项/设置窗，点开分辨率下拉框 | 下拉框贴图/展开/收起与修复前一致（`ComboBox1` 数字后缀逐字等价），不闪退 | harness 只复刻字符串→Type 转换，需真机 WZ 资源渲染 |
| 22-2 资源裁剪态不崩 | 用缺 `StatusBar3.img/OptionMenu/graphic/combo:resolution/sButtonUOL` 的裁剪资源包进设置窗 | 构造期不 UB/不抛异常，窗口以 `DEFAULT` 下拉框正常加载、可退出 | 需真机裁剪 WZ + 崩与否观察；自动测无法构造真 `nl::node` |
| 22-3 分辨率切换回归 | 在设置窗切换分辨率并确认/返回 | 选中项、ComboBox 贴图与修复前一致，`:335-345` 已有 try/catch 行为不变 | 需真机交互 + 显示链路 |
| 22-4 各数字后缀观感 | 对比 `ComboBox0/1/2/3/9` 及无数字后缀 `/` 尾部残缺 UOL | 合法数字逐字透传、无数字/空串回退 DEFAULT，无异常弹窗/空白框 | 需真机逐资源像素观感；非枚举值合法性归 Wave3 L-B 裁决 |

---

## 23. round44 · UIEnterNumber 数字框空串/溢出守卫（#devfix-uinotice-handlestring，需用户一起测）
> 编号说明：本节沿用 round 链 §19(round40) / §20(round41) / §21(round42) / §22(round43) / §23(round44)，与前文
> `## 23. ScriptMessage OnPacket 全量 + tip 820 A11h`（`:328`）、`## 24. AskAvatar / sendStyle A11i`（`:343`）
> 及 A11g 的 §22（`:313`）、A11f 的 §21（`:298`）按标题中的 roundNN 区分；历史编号原样保留，不回溯改动。

> 自动自测 `tools/test_uinotice_handlestring_guard.py` 只覆盖**静态守卫 + 修复前/后转换算法复刻**
> （22 守卫 + 13 场景 PASS，before THROW 5→after 0，回归 8/8）+
> `GATE OK` + `build_smoke.bat` `EXITCODE=0`（03:00 真重编译 3521B / ERROR_LINES 0）；
> **真机丢弃/买卖数量框退格清空、超长数字提示观感、SoftKey 数字入口必须联测**，不冒充已验证。
> exe = `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2026-11-10 03:00 重编译）。逐字证据见
> `development-and-fix-plan-2026-11/notes/wave2-uinotice-recheck-20261110.md`。

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 23-1 丢弃/买卖正常态 | 道具栏丢弃数量、商店买/卖数量，输入合法 1..max 确认 | 数量提交、窗口关闭与修复前逐字一致，回调次数不变 | harness 只复刻字符串→整数转换，需真机交互链路 |
| 23-2 退格清空后确认 | 数字框退格清空，分别按 RETURN / 点 OK / Textfield 回车 | 不闪退，弹既有 `Only numbers are allowed.` 提示，可重试输入 | 需真机 Textfield 退格 + 三条入口分发观察；自动测无法构造真 UI 事件 |
| 23-3 超长/溢出数字观感 | 输入 `9999999999` / `2147483648` 及 SoftKey 超长串 | 不崩，弹同一提示（文案是否另立 too-large 专文交裁决），可退出 | 需真机 int32 溢出观察 + 产品文案体感 |
| 23-4 SoftKey 数字入口回归 | 用 S/N 打开 SoftKey 送合法数字与空串 | 合法数字与修复前一致；空串仍被 `:310 if (s.empty()) return;` 挡住 | 需真机 SoftKey 弹窗 + 输入分发，观测冗余守卫是否重复提示 |
---

## 24. round45 · UIItemInventory 资源驱动 slot_col 除零旁路守卫（#devfix-iteminventory-slotcol，需用户一起测）
> 编号说明：本节沿用 round 链 §19(round40) / §20(round41) / §21(round42) / §22(round43) / §23(round44) / §24(round45)，与前文
> `## 21. NPC GetText Avatar + GetNumber min/max A11f`（`:298`）、`## 22. NPC tip 817–821 + Pet inset A11g`（`:313`）、
> `## 23. ScriptMessage OnPacket 全量 + tip 820 A11h`（`:328`）、`## 24. AskAvatar / sendStyle A11i`（`:343`）
> 按标题中的 roundNN 区分；历史编号原样保留，不回溯改动。

> 自动自测 `tools/test_iteminventory_slotcol_guard.py` 只覆盖**静态守卫 + 修复前/后算法复刻**
> （17 守卫 + 3 计数守卫 + 13 场景 PASS，before DIV0 9/BAD 6→after BAD 0，回归 7/7）+
> `GATE OK` + `build_smoke.bat` `EXITCODE=0`（03:29 真重编译 9426B / ERROR_LINES 0）；
> **当前 shipped v83 包 `UIWindow.img/Item` 无 `pos` 节点走 else 分支恒安全，`if(pos)` 旁路当前不可达**；
> 带 `pos` 且 `slot_col`/`slot_row` 缺失或为 0 的裁剪资源包、切页签/slider 行数观感必须真机联测，不冒充已验证。
> exe = `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2026-11-10 03:29 重编译）。逐字证据见
> `development-and-fix-plan-2026-11/notes/wave2-iteminventory-slotcol-recheck-20261110.md`。

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 24-1 当前 shipped 包正常态回归 | 用现有 v83 数据开道具栏、切 6 页签、拖滚动条 | 与修复前逐字一致（走 else `slot_col=4`），无空白/越界 | harness 只复刻算法，需真机 UI 渲染与鼠标分发 |
| 24-2 带 pos 合法资源包 | 换入含完整 `Item/pos`（col>0 且 row>0）的资源包开道具栏 | 布局按资源值渲染，与修复前逐字一致，守卫不触发 | 需真机替换 WZ/IMG 资源包观察排版，自动测无法构造 |
| 24-3 裁剪/畸形 pos 包 | 换入 `pos` 存在但 `slot_col`/`slot_row` 缺失、为 0 或为负的资源包 | 不闪退，整组回退到 4×6 默认布局，可正常切页签 | 需真机加载畸形资源 + 观察构造/切页签，harness 仅复刻整数路径 |
| 24-4 C4701 消警复核 | 后续轮次按 §5 建议加 `default:` 或 `newtab = tab` 后重编译 | 编译输出不再出现 `UIItemInventory.cpp(1194): warning C4701`，行为无变化 | 属编译器告警处置，需真机编译输出比对，本轮保守误报不立项 |

## 25. round46 · UICharInfo 子窗口稀疏 map 裸 at(i) 守卫（#devfix-charinfo-buttonguard，需用户一起测）
> 编号说明：本节沿用 round 链 §19(round40) / §20(round41) / §21(round42) / §22(round43) / §23(round44) / §24(round45) / §25(round46)，与前文
> `## 21. NPC GetText Avatar + GetNumber min/max A11f`（`:298`）、`## 22. NPC tip 817–821 + Pet inset A11g`（`:313`）、
> `## 23. ScriptMessage OnPacket 全量 + tip 820 A11h`（`:328`）、`## 24. AskAvatar / sendStyle A11i`（`:343`）、
> `## 25. AskBoxText A11j`（`:357`）按标题中的 roundNN 区分；历史编号原样保留，不回溯改动。

> 自动自测 `tools/test_charinfo_button_guard.py` 只覆盖**静态守卫 + 修复前/后稀疏 map 算法复刻**
> （13 守卫 + 2 计数守卫 + 13 场景 PASS，before THROW 6→after 0，回归 7/7）+
> `GATE OK` + `build_smoke.bat` `EXITCODE=0`（04:00 真重编译 5256B / ERROR_LINES 0）；
> **当前 shipped v83 包 `UIWindow.img/UserInfo` 无 collect/damage/character 子节点，子窗循环不常进**；
> 带 collect/damage 资源包或缺按钮节点的裁剪包、子窗按钮渲染必须真机联测，不冒充已验证。
> exe = `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2026-11-10 04:00 重编译）。逐字证据见
> `development-and-fix-plan-2026-11/notes/wave2-charinfo-recheck-20261110.md`。

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 25-1 当前 shipped 包回归 | 打开人物信息窗、看主窗口按钮与 BtBook 渲染 | 与修复前逐字一致（走 find+判空），无空白/闪退 | harness 只复刻算法，需真机 UI 渲染 |
| 25-2 带 collect/damage 资源包 | 换入含完整 collect/damage 子窗节点的资源包开人物信息、切子窗 | 子窗按钮按资源渲染，与修复前逐字一致，守卫不触发 | 需真机替换 WZ/IMG 资源包观察排版 |
| 25-3 裁剪/畸形资源包 | 换入缺部分子窗按钮节点（BtArrayGet/BtFAQ 等）的资源包 | 不闪退，缺失/空指针键跳过，其余按钮照常渲染 | 需真机加载畸形资源 + 观察 draw()，harness 仅复刻稀疏 map 路径 |
| ~~25-4 buttons.size() 上界语义裁决~~（**已于 round47 销案**：纯代码可证上界错，见 §26） | 真机观察 damage 子窗（`i < buttons.size()`）按钮是否画全 | 若发现按钮被漏画，另立 L-B 裁决改键域上界（本轮不裁决） | 属上界语义正确性，需真机观测后决定是否立项 |

## 26. round47 · UICharInfo damage 子窗循环上界销案（#devfix-charinfo-damage-bound，需用户一起测）
> 编号说明：本节沿用 round 链 §19(round40) / §20(round41) / §21(round42) / §22(round43) / §23(round44) / §24(round45) / §25(round46) / §26(round47)，与历史
> `## 21. NPC GetText Avatar + GetNumber min/max A11f`、`## 22. NPC tip 817–821 + Pet inset A11g`、
> `## 23. ScriptMessage OnPacket 全量 + tip 820 A11h`、`## 24. AskAvatar / sendStyle A11i`、
> `## 25. AskBoxText A11j` 按标题中的 roundNN 区分；历史编号原样保留，不回滚改动。

> 本节同时**销案 §25-4**：round46 把「`i < buttons.size()` 上界语义」留作 L-B 待真机裁决，本轮逐字读 live 源码确认
> `buttons.size()` 是元素个数、而 damage 子窗键域是枚举区间 `[BtFAQ, BtBook)`，两者无数量关系——纯代码即可证伪，故不再挂 L-B。
>
> 自动自测 `tools/test_charinfo_damage_bound.py` 只覆盖**静态守卫 + 上界 × 键集算法复刻**
> （14 守卫 + 13 场景 PASS，before BAD 9 / BtBook 重复 3 → after 0，回归 4/4）+
> `GATE OK` + `build_smoke.bat` `EXITCODE=0`（04:38 真重编译 / ERROR_LINES 0）；
> **真机渲染仍需人眼确认**：harness 证明的是「算出来的键集合」，不是「屏幕上画出来的像素」。
> exe = `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2026-11-10 04:38 重编译，4928512B）。逐字证据见
> `development-and-fix-plan-2026-11/notes/wave2-charinfo-damage-recheck-20261110.md`。

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 26-1 当前 shipped 包回归 | 打开人物信息窗、切 damage 子窗 | 与修复前视觉逐字一致，无空白/闪退 | harness 只复刻键集合算法，需真机 UI 渲染 |
| 26-2 完整 damage 资源包 | 换入含 `BtFAQ` / `BtRegist` 两节点的资源包后切子窗 | 两个按钮都渲染出来（修复前 `size<16` 时全漏） | 需真机替换 WZ/IMG 资源包观察 |
| 26-3 裁剪包 | 换入缺 damage 子窗按钮节点的包 | 不闪退，缺失键跳过，其余按钮照常渲染 | 需真机加载畸形资源 + 观察 draw() |
| 26-4 BtBook 重影 | 键齐（含 BtBook 18）时看 damage 子窗 | BtBook 只画一次，无重影（修复前 `size>=19` 会画两次） | 重影属视觉像素现象，需人眼/截图比对 |

## 27. round49 · SHOW_STATUS_INFO mode 5 语义错位销案（#devfix-showstatusinfo-mode5，需用户一起测）
> 编号说明：本节沿用 round 链 §19(round40) / §20(round41) / §21(round42) / §22(round43) / §23(round44) / §24(round45) / §25(round46) / §26(round47) / §27(round49)，
> 与历史 `## 27. SpeedQuiz A11k（msgtype 6）` 按标题中的 roundNN 区分；历史编号原样保留，不回滚改动。

> 改前把「聊天窗金币提示」解析成「获得 SP」：`read_short` 只吃掉服务端 4 字节 `int gain` 的低 2 字节，
> 再弹 `UISkillUp` 显示 `You have gained SP (+N)`。s9 `getShowMesoGain(gain, inChat=true)` 写 `writeByte(5)+writeInt(gain)+writeShort(0)`；
> Angel 083 `OnIncMoneyMessage@0xa221f3` 也是 `Decode4` + `SP_290/SP_292` + `CHATLOG_ADD(nType:7)`。
> 自动自测 `tools/test_showstatusinfo_mode5.py` 只覆盖**静态守卫 + 包体读法复刻**
> （15 守卫 + 6 场景 PASS，before 误标 6/6 → after 0）；**真机渲染仍需人眼确认**。
> exe = `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2026-11-10 10:21 重编译，4928512B）。
> 逐字证据见 `development-and-fix-plan-2026-11/notes/wave2-showstatusinfo-mode5-recheck-20261110.md`。

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 27-1 任务金币奖励 | 交一个给金币的任务 | 聊天窗出现 `You have gained mesos (+N)`，**不弹**技能加点窗 | 需真机跑任务链触发服务端包 |
| 27-2 商店售出 / 传送扣费 | 卖出一件道具或 GM 传送扣费 | 红色 `You have lost mesos (-N)` | 需真机走 `spendMesoTransactionally` 路径 |
| 27-3 SP 通道未受影响 | 升级获得 SP | 技能窗 SP 正常刷新（由 `ChangeStatsHandler` 负责） | 需真机升级观察技能窗 |
| 27-4 大额金币 | 获得 ≥ 65536 金币 | 数值显示完整，无负数/截断（改前 short 截断） | 需真机大额交易 |

### 27-A 待裁决：`InPacket::available()` 返回 bool 却被当字节数（393 处）

> `Net/InPacket.h:37 bool available() const;` / `Net/InPacket.cpp:30-33 return length() > 0;`。
> 全仓 393 处写成 `recv.available() >= 4` / `>= 8` / `< 5` / `== 21` / `recv.skip(recv.available() - 5)`。
> `bool` 提升成 int 只有 0/1：`>= 2/4/8` 恒 false（短包守卫全是死代码）、`< 5/14/24` 恒 true（长度预检全误判）、
> `available() - 5` 在 `size_t` 下溢。编译器已在 `MessagingHandlers.cpp:207/338/646/776/777/868/909/962` 报 `C4804`。
>
> 分布：`WaveAShellHandlers.cpp` 205、`MapObjectHandlers.cpp` 85、`CashShopHandlers.cpp` 75、
> `MessagingHandlers.cpp` 8、`TradeHandlers.cpp` 8，其余 `AllianceHandlers` 3 / `InventoryHandlers` 3 / `QuestHandlers` 2 /
> `TestingHandlers` 2 / `FredrickHandlers` 1 / `PlayerHandlers` 1。
>
> 一行类型修复（`size_t available() const { return length(); }`）会让 393 处原本恒假的守卫**同时变成活路径**，
> 等于一次性改变几乎所有 handler 的短包行为——属协议级裁决，需在真机下逐面回归后才可动。
| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 27-A1 短包行为基线 | 当前版本下走一遍登录 / 进图 / 商城 / 交易 / 聊天 | 记录现有可观察行为作为对照基线 | 需真机全链路，且缺陷本身是「守卫失效」非崩溃 |
| 27-A2 类型修复后回归 | 若裁决修复 `available()` 返回类型，重跑 27-A1 全链路 | 各面行为不变或按预期变好，无新增断线/丢状态 | 需真机逐面比对，自动测只能覆盖算法 |

## 28. round52 · K1 批量零风险销案（#devfix-k1-batch-round52，需用户一起测）

> 与历史 `## 28. NPC AskPet(9/10) + Mirror(14)` 按标题 roundNN 区分，历史编号原样保留。

范围：`drop/D7`（`Drop::Drop` switch 补 default）、`drop/D4`（`MapDrops` 缺 ItemData 补 DEBUG 日志）、
`statusbar-hud/D5`（状态栏按钮 id 注释 1005/1006/1009）、`corewindow/D5`（6 处 095 线地址换 083 EA）。
四条全部属「纯注释 / 纯日志 / 非法 mode 护栏」，**不改协议、不改坐标、不改 happy-path 行为**。

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 28-1 掉落与金币 | 正常刷怪、丢弃、拾取金币与物品 | 掉落弧线、拾取提示、金币四档（<50 / 50-99 / 100-999 / >=1000）图标与修复前一致 | 需真机渲染比对；本批只加 default 与日志，属零行为改动 |
| 28-2 三窗按钮位置 | 打开属性窗 / 道具栏 / 角色信息窗 | BtAuto(106,198)、BtGather/BtSort、BtDetail 等落点与修复前逐像素一致 | 只换注释依据不动坐标，自动测只能验证字符串 |
| 28-3 Gather/Sort 切钮 | 道具栏点 BtGather 与 BtSort | 服务端回包后按钮切换与修复前一致（切钮源理解已更正，代码未动） | 需真机联服，客户端单侧无回包 |
| 28-4 MapDrops 调试日志 | 把 `LOG_LEVEL` 调到 DEBUG 后进图刷怪 | 若出现 `MapDrops: no ItemData for itemid=... oid=...`，说明该 itemid 无 WZ 条目（既有台账「无真实掉落表」），不应伴随崩 | 需真机 + 特定 itemid；自动测只能验证字符串存在 |

## 28-A. 已闭环：095 线依据污染 9 处纯注释纠偏（round72 §38；原 5 处 + 全仓补 4 处，行为字面量 0 改动）

> 与 `corewindow/D5` 同源，但不在该决策点的 4 处范围内，且 `Player.cpp:211` 的 083 函数名尚未定位，故只登记不擅改。

| # | 位置 | 现值（095） | Angel 083 真地址（已实测） |
| --- | --- | --- | --- |
| 28-A1 | `Net/Packets/MovePathEncode.h:2-3` | `Maplestory95.i64 @ 0x666e20` | `?Encode@CMovePath@@QAEXAAVCOutPacket@@PAH@Z @ 0x68a563`（0x269） |
| 28-A2 | `Net/Packets/NpcInteractionPackets.h:42,44` | `CNpc::GenerateMovePath @ 0x671590`、`Encode 权威 0x666e20` | `?GenerateMovePath@CNpc@@IAEXJJ@Z @ 0x6d3273`（0x11f）、`CMovePath::Encode @ 0x68a563` |
| 28-A3 | `Net/Handlers/WaveAShellHandlers.cpp:3585` | `IDA 0x666e20` | 同上 `0x68a563` |
| 28-A4 | `Gameplay/MapleMap/Npc.cpp:253` | `CMovePath::Encode @ 0x666e20` | `0x68a563` |
| 28-A5 | `Character/Player.cpp:211` | `对照 port-095 CWvsContext::CheckTemporaryStatDuration` | 083 IDB 该符号 0 命中；相关真符号 `OnTemporaryStatSet @0xa202be` / `OnTemporaryStatReset @0xa2071f`（round72 已按 s9 协议口径纠偏，见 §38） |

反证：`0x666e20` 在 083 落在 `sub_666362`、`0x671590` 落在 `?TrySpeaking@CMob@@IAEXJJ@Z`——都不是路径编码函数。
## 29. round53 · 聊天目标快捷键接线（#devfix-chattarget-keybind，需用户一起测）

> 编号说明：历史 `## 28. NPC AskPet(9/10) + Mirror(14)`、`## 27. SpeedQuiz A11k` 等早期节与本系列按标题 roundNN 区分，历史编号原样保留。

范围：`statusbar-hud/D4` + `social-economy/D1` 合并 —— `UIChatBar::change_target` 由空 stub 改为
`KeyAction::Id` → `ChatTargetIndex` 映射 + `set_classic_target()` + 按需 `toggle_input(true)`。
不新增协议、不改 opcode、不改包体，六个聊天键从「按了没反应」恢复为切目标 + 开输入行。

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 29-1 六键切目标 | classic HUD 下依次按 SAY / PARTYCHAT / FRIENDSCHAT / GUILDCHAT / ALLIANCECHAT / TOSPOUSE | 聊天框目标名依次变为 全部 / 组队 / 好友 / 公会 / 联盟 / 配偶，且输入行打开 | 需真机看目标文案与输入行观感，自动化只能断言映射表 |
| 29-2 modern HUD 观感 | 切到 modern HUD 后按上述六键 | 输入行展开、5 个 Bt（聊天/物品链接/表情/帮助/收框）active 翻转正常 | `toggle_view(true, true)` 的视觉结果需真机 |
| 29-3 输入行已开时再按 | 输入行打开状态下按其它频道键 | 只切目标、不收框（harness 已断言状态机，真机确认观感） | 需真机确认没有闪烁或焦点丢失 |
| 29-4 无频道时发送 | 切到组队/公会/联盟但当前无对应频道，输入文字回车 | 走 `CHANNEL_HELD` / `CHANNEL_FALLBACK`，有提示且不误发到公共频道 | 需联服才能验证服务端回执与该分支 |

## 30. round54 · 聊天输入框启用系统 IME（#devfix-chat-unicode-ime，需用户一起测）

> 与 §26/§27/§28/§29 等历史节按标题 roundNN 区分，历史编号原样保留。

本轮改 `IO/UITypes/UIChatBar.cpp` 两处 `input_text` 构造（classic `:139-146` / modern `:287-291`），各加一行 `set_accept_unicode(true);`，让聊天框走 SDL_EVENT_TEXT_INPUT 收 UTF-8（系统 IME），不再被 `Textfield::add_string` 的 `c >= 128` 过滤丢弃。`limit = 70` 未改（字节上限，中文约 23 字）。

| ID | 场景 | 期望 | 判定 |
| --- | --- | --- | --- |
| 30-1 | 经典 HUD 聊天框输中文 | 输入法弹出、候选窗可用、上字正常、字图无竖线/白线伪影 | ⏳ |
| 30-2 | 现代 HUD 聊天框（`view_max` 展开态）输中文 | 同上；白字在深底可读 | ⏳ |
| 30-3 | 中文发送后对端接收 | s9 收到 GBK 原文，无乱码；23 字上限内不截断 | ⏳ |
| 30-4 | 账号/密码框回归 | 仍只收 ASCII，中文被丢弃（ASCII-only 默认值未被误改） | ⏳ |
| 30-5 | 地图搜索 / SoftKey 文本 / NPC GetText 回归 | 与修前一致（这三处本就开着） | ⏳ |

已知限制（非缺陷）：`limit = 70` 按**字节**计数，中文 3 字节/字 ⇒ 实际约 23 字；s9 `GeneralChatHandler.java:46` 按**字符**限 127，两侧口径不同。放宽上限属行为变更，另立裁决。
## 31. round55 · K1 收尾：v95 注释换 083 依据 + 三处确认文案 i18n（#devfix-k1-d1-d14，需用户一起测）

> 编号说明：本节按标题 `roundNN` 区分；历史 `## 31. G10 金/银宝箱（TreasureBox）` 保留原编号。

| # | 项 | 观察点 | 预期 |
| --- | --- | --- | --- |
| 31-1 | 家族聊天窗（Messenger）关闭钮 | 打开信使窗，看右上关闭钮位置 | 仍在 (276,6)，与改前一致（本轮只换注释依据） |
| 31-2 | 打字指示（Messenger） | 输入框空/非空来回切换，对端观察 | 对端仍显示「正在输入」且不重复刷包；blink 起止节奏不变 |
| 31-3 | 快捷键设置窗三处确认框 | zh-CN 下点「默认/删除/OK 关闭」 | 分别弹「确定要恢复默认设置吗？」「确定要清除所有键位绑定吗？」「要保存本次修改吗？」 |
| 31-4 | 语言切换后文案 | en-US 下重开同一三处确认框 | 显示改前英文原值（决策 B：en-US 值不改） |

### 31-A. 待裁决（本轮取证未改）

| # | 项 | 现状 | 建议 |
| --- | --- | --- | --- |
| 31-A1 | `docs/features/guild-bbs-ui/README.md:20` 的 `port-095 v95 CUIGuildBBS::...` 对照行 | 属「对照记录」而非「以 095 为权威依据」，本轮未动 | 是否统一标注「仅对照、非判定依据」由用户裁决 |
| 31-A2 | `locale/en-US.ini` 中 44 条中文残留（`trade.*` 19 / `dailygift.*` 14 / `mapsearch.*` 5 / `mapinfo.*` 3 / 其它 3） | 决策 B 范围内保留原值 | 是否纳入翻译批次由用户裁决 |
## 32. round66 · UIRecommendWorld 按 Angel 083 布局重修（#devfix-recommendworld-083-relayout，需用户一起测）

> 编号说明：本节按标题 `roundNN` 区分。用户 m09859 硬指令「ui 布局、动画一定要确认是否按 Angel 布局修复过」。

| # | 项 | 观察点 | 预期 |
| --- | --- | --- | --- |
| 32-1 | 真背景观感 | 世界选择页点 `BtViewChoice` 弹窗，与 `C:\Users\11036\Desktop\正确客户端截图\` 原生截图对照 | 深棕边框 + 金色 Recommended World 标题 + 左右两块浅色面板，尺寸 540×167；不再是手绘「推荐世界」+ chBackgrn 假背景 |
| 32-2 | 三颗真位图按钮 | 左右箭头（28×30）、选择（62×26）的 disabled/normal/mouseOver/pressed 四态 | 位置 (45,70)/(215,70)/(113,114)，四态用真位图；单世界/零世界时箭头显示 disabled 态 |
| 32-3 | 切世界 / 选中 | 点左右箭头循环切世界，点「选择」或消息区 | 箭头环绕切换名牌/消息；点「选择」或消息区 {78,61,212,110} 关窗并落到对应世界 |
| 32-4 | 名牌/消息对齐 | 世界名牌图、名称文字、消息文字（含超 200 宽换行） | 左/右浅色面板内排版正确、不溢出；精确像素对齐以原生截图为准（本轮按面板几何居中，未冒充完全精确） |
| 32-5 | 登录背景动画（确认保留） | 登录页背景滚动补间 | 匀速线性（按 Angel 083 CLogin 修复过，保留），非 smoothstep S 曲线 |

已知限制（非缺陷）：世界名牌图/消息文本 anchor 083 反编译未给精确立即数，按真背景两浅色面板几何居中；083 滚动条 id=1003 与按 `'\r'` 分行未复刻（当前端 `event_message` 为单行）；opcode 27 目前只把推荐消息当聊天提示、未自动弹窗，弹窗由 `BtViewChoice` 驱动 —— 是否接自动弹窗属协议级裁决。

## 33. round67 · HUD 主窗 4 钮闪烁按 Angel 083 SetButtonBlink 状态机重修（#devfix-hud-button-blink-083，需用户一起测）

> 本面按 roundNN 编号沿用「用户 m09859 硬指令：ui 布局、动画一定要确认是否按 Angel 布局修复过，不是则重新修复」。外部程序 d6bdce97 的「本包无 KEY_ANI 资源 → 7.5Hz 半透明白块」经 .nx 翻案为三层全错（资源/频率/时序），本轮按 083 真帧+真状态机重修。

| # | 项 | 观察点 | 预期 |
| --- | --- | --- | --- |
| 33-1 | 升级启动 | 低等级（1-10 级）打怪升级瞬间看属性栏 STAT 钮 | STAT 钮按 1Hz 亮黄发光帧（28x20）闪烁，非白块、非 7.5Hz |
| 33-2 | 开子窗清除 | 闪烁中点击装备/背包/属性/技能任一子窗 | 对应钮闪烁立刻消失（083 a3=0 清除，与外部程序「开子窗启动」相反） |
| 33-3 | 90s 超时 | 触发闪烁后不开任何子窗静置 90 秒 | 闪烁自动停止（083 Update +90000ms 超时清除） |
| 33-4 | 真帧对齐 | 闪烁时对照 `C:\Users\11036\Desktop\正确客户端截图\` HUD 底栏 | 亮黄发光帧与常态按钮位像素对齐，无错位/无残影/无淡出拖尾 |
| 33-5 | 未复刻场景 | 任务指引、其它需 blink 的 083 场景 | 当前端未接 `CUtilDlgEx::Draw` 任务指引链，观感是否符合预期交用户裁决 |

已知限制（非缺陷，登记 backlog）：升级启动用 UI 层 `stats.get_stat(LEVEL)` 自感知，未改 s9 协议 handler；083 的 `CCtrlScrollBar` 与任务指引实体类不照搬（校准≠改回原生）；名牌/其它按 BeiDou 或自行修复的 UI/动画（D8 组队血条、好友窗几何）待后续 round 逐面复核。

## 34. round68 · 好友窗 / 组队窗按 Angel 083 CUIUserList 真资源重修（#devfix-userlist-partyhp-083，需用户一起测）

> 本面按 roundNN 编号沿用「用户 m09859 硬指令：ui 布局、动画一定要确认是否按 Angel 布局修复过，不是则重新修复」。外部程序 7105da94/d6bdce97 用全库 0 命中的虚构根 FriendRecommendations/FriendsList 与 Party.img + ColorBox 手绘血条，经 .nx 翻案为「找错节点后自造兜底」第三/四例，本轮按 083 真资源重修。

| # | 项 | 观察点 | 预期 |
| --- | --- | --- | --- |
| 34-1 | 好友窗真背景 | 按 s2c BUDDYLIST=63 弹好友窗（或热键） | 显示 312x389「玩家列表 & 家族」浅蓝共用窗背景，加好友钮 BtAddFriend(5,81) 真位图可见可点，非空白 |
| 34-2 | 组队窗真背景/四钮 | 组队后打开队伍窗 | 显示 312x389 真背景，邀请/离开/队长/开除四钮为 083 真位图且坐标对齐底栏 |
| 34-3 | D8 血条真位图 | 组队成员掉血（s9 UPDATE_PARTYMEMBER_HP） | 血条为 GaugeBar graduation 刻度外框 + bar 亮黄填充，满/半/空按 cur/max 裁右，无 30% 转红白块 |
| 34-4 | 血条像素对齐 | 对照 `C:\\Users\\11036\\Desktop\\正确客户端截图\\` 组队窗 | 63 宽血条与成员行 y+16 位置对齐，bar origin(-3,-3) 内缩正确，无错位/越界 |
| 34-5 | 窗口布局 | 312 宽下好友行/成员行点击、关闭钮(294,6) | 行命中区覆盖到 x=302，关闭钮右上角可点，无遮挡 |

已知限制（非缺陷，登记 backlog）：083 独立可拖拽 CUIPartyHP 悬浮窗、五页签 CUIUserList 共用窗、SP_4433_SHOWPARTYHP 配置开关(默认1)、SP_2116_PARTY_BTHP 血条开关钮、组队页完整 9 钮、CUtilDlgEx 任务指引链均属架构级决策点，本轮未复刻（不新增 UIElement::Type 枚举、不动 s9 协议）；血条与队伍行像素级对齐需真机截图核。

## 35. round69 · D7 聊天三态按 Angel 083 SetChatType 只读取证复核（#devfix-chattype-083-review，需用户一起测 / 含决策点 D7-A）

> 本面按 roundNN 编号沿用「用户 m09859 硬指令：ui 布局、动画一定要确认是否按 Angel 布局修复过，不是则重新修复」。外部程序 d6bdce97 资源根 Basic.img/BtMax|BtMin 用对了（.nx 实测），但把「nType 1/2/3 拖高 26..507」「紧凑态固定高度 120」「BtMin/BtMax 逐级 3→2→1 + chat.type 持久化」整套 s9 适配状态机套到 083 名下 —— 第 5 例同族病，错法=语义套壳。本轮依据校准 README:288 决策点只做只读取证+对照+纠错注释，不改行为，是否整体改回 083 交用户裁决（D7-A）。

| # | 项 | 观察点 | 预期 |
| --- | --- | --- | --- |
| 35-1 | 五屏截图（D7-A 前置） | 收起 / 输入中 / 完整最小 / 完整默认 / 完整拖高 五态逐屏截图 | 确认三态可见差异，作为是否整体改回 083 状态机的裁决依据 |
| 35-2 | 紧凑态观感 | 点击 BtMin 从完整→紧凑 | 本端显示 120 高固定紧凑框；083 输入中态是 1 行历史 + 输入框（h 恒 24），两者观感差异记录 |
| 35-3 | 拖拽边界 | 历史框拖到最小/最大 | 本端 clamp 26..507；083 ChangeChatWndSize 在 clamp 后 `h%13==0→+2` 会到 28/509，记录视觉表现差异 |
| 35-4 | 按钮键位 | BtMax/BtMin 同位互斥点击 + 功能键切换 | 本端逐级 3→2→1；083 只有 1003→1 / 1004→3、功能键 case16 3↔1，记录对应差异 |
| 35-5 | D2 聊天规则真机 | 启 s9 服务端：2000ms 限流 / 连续 4 同句 / 脏词 / !mute !unmute(gm3) | 服务端已双改，联机回归限流与禁言文案，与 D7 三态互不干扰 |

已知限制（非缺陷，登记 backlog）：本轮**不改任何行为字面量**（chat_type 赋值 / CLASSIC_LOG_H_* 常量 / button_pressed 分支 / Setting key），harness A4 逐条守卫「仍在」；只纠正 3 文件错误依据注释。083 三态整体改回（删紧凑态、nType2 改焦点瞬态不持久、去逐级、chat.type 持久化键语义、滚动条 id 1010、MakeCtrlEdit(0/1) 输入行有无）属 **决策点 D7-A**，须用户裁决 + 五屏截图后再按一次性 Plan 实施；校准≠改回原生，s9 协议优先。

## 36. round70 · 伤害数字 / 伤害皮肤按 Angel 083 CAnimationDisplayer 只读取证复核（#devfix-damagenumber-083-review，需用户一起测 / 含决策点 D-A）

> 本面按 roundNN 编号沿用「用户 m09859 硬指令：ui 布局、动画一定要确认是否按 Angel 布局修复过，不是则重新修复」。083 伤害数字 = `CAnimationDisplayer::Effect_HP @0x437d0f` 逐位图动画（动态 3/5 字宽 / canvas 57 / 奇偶随机抖动 / 400-600-1000ms 三段），MISS 走 SP_984_MISS 专用位图；083 无 DamageNumber 类、无伤害皮肤系统。当前端是 Journey DamageNumber + s9 DAMAGE_SKIN(247) 适配，资源根与缩放注释经 .nx 实测属实 —— 属架构/版本差异而非「自造兜底冒称 083」。用户 m10982 强线索「catalog 漏 DamageSkin.img ⇒ 整根合并失败」经 5 组 open_merge 探针证伪：资源可达（624 皮肤节点），漏文件是过期快照。本轮只做只读取证 + 对照 + 纠错/依据注释，不改行为，是否按 083 动态字宽重排交用户裁决（D-A）。

| # | 项 | 观察点 | 预期 |
| --- | --- | --- | --- |
| 36-1 | 伤害皮肤实际显示 | 启 s9，装备皮肤 id 1 / 10 / 100，对比默认 BasicEff 逐屏截图 | `Effect["DamageSkin.img"]` 可达且字形切换生效（探针已证 624 节点） |
| 36-2 | 皮肤数字间距（D-A 前置） | 同一伤害值在默认 / 皮肤 A / 皮肤 B 下的数字排布 | 记录是否因硬编码 BasicEff 宽度表出现间距错位，作为 D-A 裁决依据 |
| 36-3 | MISS 显示 | 打出 MISS | 显示 98×38 专用 MISS 位图（经 charset_char('Miss')='M' 解析），非字符 M |
| 36-4 | 超长伤害缩放 | 16 位（接近 2^53）伤害数值 | 等比缩放不截断、不横向溢出（MAX_DAMAGE_WIDTH=260） |
| 36-5 | 三型字形 | 普通 / 暴击 / TOPLAYER | NoRed / NoCri / NoViolet 三套字形与颜色正确 |

已知限制（非缺陷，登记 backlog）：本轮**不改任何行为字面量**（getadvance 表 / MAX_DAMAGE_WIDTH=260 / FADE_TIME=500 / rowheight 36-30 / apply_skin 分支 / init 字符集），harness A2 逐条守卫「仍在」；只补 4 处纯注释。083 整体重排（动态 3/5 字宽 + canvas 57 + 三段动画 + 皮肤间距修正）属 **决策点 D-A**，须用户裁决 + 五屏截图后再按一次性 Plan 实施；校准≠改回原生（083 无 DamageNumber/皮肤系统）、s9 协议优先；DamageSkin.img 可达性已由最小探针闭环，不再是决策点。

## 37. round71 · GAMEOPT / SYSOPT 设置窗按 Angel 083 CUIGameOpt/CUISysOpt 只读取证复核（#devfix-optionmenu-083-review，需用户一起测 / 含决策点 D-B）

| 编号 | 真机检查项 | 预期 | 关联 |
| --- | --- | --- | --- |
| 37-1 | 五屏截图：游戏设置 / 系统设置 / 分辨率下拉展开 / 滑条拖动 / OK-CANCEL 落位 | 与 083 版式及当前底板逐格对照 | D-B |
| 37-2 | 083 y=342 与当前 BTN_Y=372 两版 OK/CANCEL 视觉对照 | 当前 372 落在 299x396 底板底部深色空条，083 342 落浅色区 | D-B |
| 37-3 | +28 滑条 / 勾选 / 下拉是否压住印刷行名 | 浅色带 y27..307 无区分度，须真机定 | D-B |
| 37-4 | 9 档分辨率下拉切换真机生效性 | 083 无此下拉，属本端扩展 | D-B |
| 37-5 | `data.format=img` 下 UIWindow2.img / StatusBar3.img / OptionMenu 缺失的退化壳表现 | 运行时 open_merge 探针 0 命中，UIOptionMenu 退化壳 | 口径 |

已知限制（非缺陷，登记 backlog）：本轮**不改任何行为字面量**（CHECK_X=130 / CHECK_TOP=34 / SLIDER_ROW +28 / 9 档分辨率 / BTN_Y=372 全在，harness 逐条守卫「仍在」）；只补 083 依据纯注释。`UIGameOpt` 经 v2 像素检测已闭环（按 Angel 布局对齐）；`UISysOpt` 是否回退 083（滑条 40/70/100/160/190/220、无分辨率下拉、OK/CANCEL y=342）属 **决策点 D-B**，须用户裁决 + 五屏截图后再按一次性 Plan 实施；运行时底板 299x396 与旧 .nx 299x365 是两张完全不同的图，不得按行偏移盲改常量；校准≠改回原生、数值重合≠语义重合、s9 协议优先。

## 38. round72 · NPC_ACTION / CANCEL_DEBUFF 依据链按 Angel 083 复核纠偏（#devfix-npcaction-083-review，需用户一起测 / 含决策点 D-C）

> 性质：纯注释依据链纠偏，0 行为字面量；9 块 11 行 095 字面量清零，083 真符号仅作对照。PLT §28-A 九处台账同轮标闭环。

| 项 | 真机观测点 | 预期 | 备注 |
| --- | --- | --- | --- |
| 38-1 | NPC_ACTION 移动段（length>6）联机观感 + 服务端 log | 尾 9 丢弃与 s9 NPCAnimationHandler 契约一致 | 本端下行解析器未改，仅注释口径 |
| 38-2 | CANCEL_DEBUFF(99) 空包服务端是否收到 | s9 CancelDebuffHandler 空实现、发了无副作用 | 客户端已协议对齐 |
| 38-3 | 决策点 D-C：是否按 083 CMovePath::Encode 重写移动包形 | 待用户裁决（协议级，会破坏 s9 联机） | 本轮不实施 |
| 38-4 | 决策点 D-C2：s9 服务端是否解禁 CancelDebuffHandler | 待用户裁决（服务端） | 本轮不实施 |
| 38-5 | 若 D-C 通过：五屏截图核对 NPC 移动动画与 083 观感 | 逐项对齐后另立 wave | 依赖 38-3 |

已知限制：083 `CMovePath::Encode @0x68a563` 头仅 x/y+count、attr switch 到 0x16；本端按 s9 契约为 x/y/vx/vy+count、attr 含 0x23/0x24，两者不同版本；旧 095 `0x666e20` 证据文件仅作本地翻案留证、不作代码依据。`*CheckTemporaryStatDuration*` / `*CancelDebuff*` 在 083 符号表 0 命中。

## 39. round73 · UILoginWait 加载进度条按 Angel 083 CLogin 等待态只读取证复核（#devfix-loginwait-083-review，需用户一起测）

> 本轮为**只读取证 + 口径纠正（0 行为字面量）**，无功能改动；以下为需启 s9 + 正确端截图对照的观感/交互项。
> 依据：`evidence/loginwait-083-round73.json`、`notes/wave2-loginwait-083-review-20261111.md`（门禁 18：IDA→orange-wz→合规 .nx 回退）。

| 编号 | 真机验收项 | 期望 | 关联 |
| --- | --- | --- | --- |
| 39-1 | bar 像素对齐 | `Login.img/Notice/Loading/bar/0..10` 11 帧 109x8 画在 `(131,98)`，与 `backgrnd 282x144` 的 LOADING 凹槽逐像素对齐；083 无布局立即数，`(131,98)` 是截图 SAD 派生，非 083 权威 | 决策点：偏差是否按真机重定标 |
| 39-2 | 动画帧序 | 0..9 帧 delay=100ms、10 帧缺省 100ms；等待期循环 0→100%→回 0，服务端响应后 `begin_finish` 补满到 100% 再切页 | `tools/_lw_visual/bar_{0,5,10}.png` |
| 39-3 | 取消钮 | `BtCancel 65x23` 在弹窗内 `(188,44)` 与「正在连接到服务器。」同行；点击 `Session::reconnect()` + 关窗，未满条不执行被取消的续场 | 截图 SAD 派生坐标 |
| 39-4 | **10s 超时（决策点 D-D）** | 083 `CLogin::Update @0x5f4c16` 十秒未响应即 `RemoveNoticeConnecting` + 回标题；当前端**无超时、永久循环** → 是否补齐需用户裁决（会改变「等到服务端响应」语义，s9 慢登录可能误回标题） | round73 零实施，待裁决后按一次性 Plan 单独提交 |
| 39-5 | 反例复核 | 确认本窗用的是真 IMG 资源（非手绘兜底），不因 m09859 逐面复核被误当同族病返工 | 保留项 |

已知限制：083 无独立 Loading 窗类（`*LoginWait*`/`*Loading*`/`*CUIWait*` 全 0 命中），等待态是 `CLogin` 状态机，故无 083 版式立即数可逐像素对齐；`bar` 几何与「循环增长」语义均为当前端 Journey + 截图派生，仅作对照。`IO/LoginBackground.cpp:77` 线性插值经复核按 Angel 083 CLogin 修过，保留不动。

## 40. round74 · UILogin/CUITitle 标题窗按 Angel 083 OnCreate 立即数复核（#devfix-logintitle-083-review）

> 本轮为**只读取证 + 口径标注（0 行为字面量）**，结论 = **反例**：当前端按 Angel 083 立即数实现，非「截图 SAD 自造版式」，无需重修布局。依据：`evidence/logintitle-083-round74.json`、`notes/wave2-logintitle-083-review-20261111.md`（门禁 18：IDA→orange-wz→.nx→运行时 open_merge 探针）。

| 编号 | 真机验收项 | 期望 | 关联 |
| --- | --- | --- | --- |
| 40-1 | 7 按钮像素对齐 | `CUITitle::OnCreate @0x62054a` 的 (213,-5)/(37,74)/(144,73)/(226,74)/(14,122)/(114,120)/(214,120) 相对 `panel_pos` 落位，与 `Login.img/Title` 7 Bt* 位图叠合无缝 | 083 立即数逐字命中 |
| 40-2 | 输入框雕刻槽 | 账号 (67,12) 132x15、密码 (67,40) 120x15 落木牌自带槽内，A11M 字形不出槽、中文 IME 可输入 | `TEXTFIELD_LIMIT=12`、round54 IME |
| 40-3 | 窗原点 (58,16) | 截图像素对齐全屏 1280x720 标题页；该量为截图 SAD 派生、083 OnCreate 无此立即数，真机五屏核对观感 | notes §3 ⚠️ |
| 40-4 | bedou 真节点 | `Login.img/Title` 运行时 13 节点、`mapleID/PW/backgrnd` 缺 → 不叠白底 ColorBox；`Title_new`/`Common/backgrnd` 缺 → 回退 `Title` + `Common/frame` 正确 | open_merge 探针 |
| 40-5 | **决策点 D-E** | 083 `OnButtonClicked @0x620ca5` 5 个 SP_ 外链常量 vs 本端 4 个 `Configuration` URL（`SDL_OpenURL`）：确认注册/官网/找回密码/找回账号四个外链可达；是否对齐 083 SP_ 常量交用户裁决 | 校准≠改回原生 / s9 适配 |

已知限制：083 `CUITitle::OnCreate` **未给窗口屏幕原点立即数**，`(58,16)` 属截图 SAD 派生且注释已自认，不作 083 背书；083 外链为 Nexon 官方 StringPool 常量，本端按 s9/配置适配不照搬；本轮 0 行为字面量，真机观感（尤其 (58,16) 全屏配准与雕刻槽字形）需启 s9 + 五屏截图核。

## 41. round75 · UIKeyConfig 键盘设置窗按 Angel 083 CUIKeyConfig 复核（#devfix-keyconfig-083-review）

本轮为 read-only 复核 + 纯注释口径纠正（0 行为字面量），需启 s9 + 五屏截图做观感验收。

| 项 | 验收动作 | 预期 |
| --- | --- | --- |
| 41-1 | 打开键盘设置窗，对照 083 原生截图 | 底图 629x373、7 颗 Bt* 真按钮、键位槽 32x32 对齐 |
| 41-2 | 核对 5 颗功能钮（确定/取消/取消修改/全部消除/变更快捷键）一行 | y=234 截图派生坐标观感对齐，无重叠/错位 |
| 41-3 | 点 DEFAULT / DELETE 弹确认 | 266x116 确认框语义（本端 UIOk + text_or 属 s9 适配） |
| 41-4 | classic 键位高亮 / 拖拽映射 | keys_pos 来自 dword_BE27E0，NUM1=(48,66) 起步长 34 |
| 41-5 | StatusBar.img/KeyConfig 缺失回退 | 自动回退 UIWindow.img/KeyConfig，窗口不空白 |

已知限制：083 主路径（ctor/OnButtonClicked/区间 CreateCtrl/.nx 12 子节点）无 5 按钮坐标立即数，y=234 为门禁 18 截图备选派生、非 083 背书；是否按真机截图微调坐标属决策点 D-F，本轮零实施。D-E 同族：083 5 个 SP_ 外链 vs 本端 4 个 Configuration URL 属 s9 适配。

## 42. round76 · UIGuild 公会浮窗按 Angel 083 只读取证复核（#devfix-guild-window-083-review）

本轮为 read-only 复核 + 纯注释口径纠正（0 行为字面量），需启 s9 + 五屏截图做观感验收。

| 项 | 验收动作 | 预期 |
| --- | --- | --- |
| 42-1 | G(17) 打开公会窗，对照 083 原生截图 | 主路径走 UIUserList 共用窗第 3 页签（312x389），非 UIGuild 浮窗 |
| 42-2 | 底栏社区钮打开本端 UIGuild 旁路 | 现为全文字自绘 + 300x380 空底衬，无 GuildUI.img 背景（虚构根已标注） |
| 42-3 | 若裁决 D-G 换真资源 | UserList/Guild/backgrnd 233x304 或 GuildBBS.img/GuildBBS 734x526 需同步 dimension |
| 42-4 | 五按钮（Create/Invite/Notice/Alliance/Close） | 仅 Close 有 close_btn() 贴图，其余为键盘快捷键入口 |
| 42-5 | B 键打开公会看板 | UIGuildBBS 用 GuildBBS.img/GuildBBS 真资源（734x526），与本浮窗互证 |

已知限制：083 无独立 CUIGuild 浮窗类，公会主体 = CTabGuild::Draw @0x90f17c 挂 CUIUserList；GuildUI.img 在 083/物理/.nx/运行时五证为 0，属第 8 例同族病（虚构根 + 自造全文字窗）。改窗/换真资源会动 UIElement::Type、GuildModel、底栏入口、s9 handler 与 dimension，属架构级决策点 D-G，round76 零实施；**round85 已按用户 m13177 授权把「入口收敛」这一层收口（见 §43），但「删类 / 换真资源重绘」仍属架构级、本端保留 UIGuild 为 dead-code 兼容占位**。主路径 G 键已按 083 路由共用窗，未破坏联机。

## 43. round85 · D-G 公会浮窗入口收敛 + orange-wz key 规格翻案（#devfix-guild-dg-resolution，需用户一起测）

本轮为**行为级入口收敛**（4 处 `emplace<UIGuild>` → `UIUserList` 共用窗页签）+ 仓库规范补正，需启 s9 + 五屏截图做观感验收。

| 项 | 验收动作 | 预期 |
| --- | --- | --- |
| 43-1 | 底栏「社区 → 家族」钮 | 打开 `UIUserList` 共用窗第 3 页签 `Tab::GUILD`（312x389 真资源），不再是 300x380 全文字 `UIGuild` 浮窗；反复点按 toggle 收起/展开，与「好友/组员」钮行为一致 |
| 43-2 | 菜单「家族联盟」钮 | 打开 `UIUserList::Tab::UNION`，非独立浮窗 |
| 43-3 | 快捷窗 `7` 键 / `U`\|`u` 键 | 均落到 `Tab::GUILD`；`U` 键在窗已存在但页签不同时 `change_tab(GUILD)` 而非重开 |
| 43-4 | G(17) 主键 | 仍走 `IO/UIStateGame.cpp:361-392` 的 `UIUserList::Tab::GUILD`（回归守卫，确保本轮未破坏主路径） |
| 43-5 | 真机公会看板 B 键 / 12 个 gi_btn | `UIGuildBBS` 与 `UIUserList` Guild 页真资源互证，无空白窗 / 无虚构根日志 |

已知限制：

- `UIGuild.cpp/.h`、`UIElement::Type::GUILD` 保留为 dead-code 兼容占位（无任何用户可触达入口再实例化）；彻底删除属架构级决策，本轮不做。
- 本轮**不动** s9 协议 / `GuildModel` / GuildPackets / `dimension` / 五按钮行为，仅收敛 4 个入口。
- **orange-wz key 规格翻案（本轮实证）**：`AGENTS.md:23` 旧「`userKeyBase64` 必须 65536B」对 orange-wz 路径是错规格。三次实验：①65536B 全零 key → `IMG 解析失败`；②真 65536B `key_gms`（`tools/_gms_key.b64`，87384 字符 base64）→ 仍 `IMG 解析失败`；③**128B `WzAESConstant.DEFAULT_KEY`**（`tools/_gms_key_128.b64`，172 字符）→ `query_nodes` 返回 `Equip`/`BtPetEquipShow`/`BtPetEquipHide`/`ToolTip/Equip`、`isError:false`。真因=`WzMutableKey.getTrimmedUserKey()` 走「128B→32B AES 派生」，与 `.nx`/nlnx 的 65536B 全量 XOR 表互不通用；非会话过期/非重名/非服务端故障/非零值。AGENTS.md 已按路径分两套规格补正。
- 真机五屏截图观感、D1–D14 其余裁决、`InPacket::available()` 393 处、跨面矩阵残差 28、L-B 销案口径仍 open。

## 44. round86 · 跨面 168 条批量裁决销案 + 真机认领清单（#devfix-batch-adjudication，需用户一起测）

本轮**零源码 / 零协议 / 零 WZ 改动**：只把 `docs/features/angel-calibration-d-matrix-2026-11/README.md` 的 168 条按 s9 优先五条批量裁决 + 残差 28 条逐条裁定结案（104+36+28=168，未决 0）。裁决书见同目录 `DECISIONS.md`。

| 项 | 归口 | 验收动作 | 本轮状态 |
| --- | --- | --- | --- |
| 44-1 | 登录 / 世界选择 / 选角 / 标题四屏 | 启 s9 冷启动，截图配准 7 按钮 / 2 输入框 / 推荐世界弹窗 / 11 帧进度条 | ⬜ 未执行，用户认领 |
| 44-2 | HUD / 状态栏一屏 | 4 钮 KEY_ANI 1Hz 闪烁、聊天三态、六频道键、G(17)/社区家族钮落 UIUserList 第 3 页签 | ⬜ 未执行，用户认领 |
| 44-3 | 核心窗 / 社交经济一屏 | 好友组队窗 312x389 真资源、Guild 页 12 个 gi_btn、SysOpt/GameOpt 版式、UIKeyConfig 5 钮 y=234 | ⬜ 未执行，用户认领 |
| 44-4 | 战斗 / 掉落 / 技能一屏 | 伤害数字动态字宽（D-A）、DamageSkin 皮肤间距、金币提示 mode5、掉落四档 | ⬜ 未执行，用户认领 |
| 44-5 | NPC / 任务 / 地图 / 商城一屏 | NPC 对话排版 A11b–A11j、隐藏门三态、商城 9 tab、F 维 12 项逐像素 | ⬜ 未执行，用户认领 |

已知限制：

- 本文件 60 节 / 142 处真机标记**全部由用户认领**，本轮零真机执行；不得据 `DECISIONS.md` 把任何观感项标「已验证」。
- T5 旧「orange-wz 内容层阻塞」前提已按 round85 `4b2452c4` 翻案：128B `WzAESConstant.DEFAULT_KEY` 实测 `query_nodes isError:false`；F 维挂 P2 仅因真机观感未采，非 MCP 故障。
- L-B 12 项与 `InPacket::available()` 393 处经 m09211 追认维持现状（§27-A 仍挂真机逐面回归），零协议改动。
- 外部程序在途 EXP WIP 原样留工作树，不夹带进本轮提交。

## 45. round78 · UIEquipInventory 装备窗宠物区 083 复核 + 真机认领清单（#devfix-equip-window-083-review，需用户一起测）

本轮 **0 行为字面量**：只补 083 依据纯注释 + 四文档 + harness。定性 = 合法混合面反例（真资源根 + 截图 SAD 坐标 + 083 独立子窗架构差异），非虚构根同族病；不重写 Journey 内嵌宠物面板架构。

| 项 | 归口 | 验收动作 | 本轮状态 |
| --- | --- | --- | --- |
| 45-1 | 装备窗宠物面板展开帧 | 启 s9 冷启动开装备窗 → 点 BtPetEquipShow 展开宠物面板，截图核对 `pet 177x181` 相对主窗右下角 (right-3, bottom-h) 与底边齐平 | ⬜ 未执行，用户认领 |
| 45-2 | BtPet1/2/3 三页签 origin | 展开帧量测 `BtPet1/2/3` 四态 origin 是否 (15,13)；现有 y 按 (15,0) 推可能整体高 13px（注释「只记不改」），待原生帧定 | ⬜ 未执行，用户认领 |
| 45-3 | BtDetail / BtCashshop 默认隐藏 | 截图复核两钮全图模板匹配 mae 55.8 / 61.1（对照显示钮 ≤0.8）→ 原生帧内是否存在，当前端 `set_active(false)` 是否正确 | ⬜ 未执行，用户认领 |
| 45-4 | 独立子窗 vs 内嵌面板交互观感 | 083 是 `CUIPetEquip` 独立浮窗（+123 偏移 / 2000 收回 / 2001-2003 三页签），本端内嵌面板 + BT_PET_SHOW/HIDE + 三页签，核对交互语义等价与观感差异是否可接受 | ⬜ 未执行，用户认领 |

已知限制：

- orange-wz MCP 路径用 128B `WzAESConstant.DEFAULT_KEY`（`tools/_gms_key_128.b64` 172 字符 + iv `TSPHKw==`）；旧 65536B 全量表 `IMG 解析失败` 是规格错配假阻塞，非 MCP 故障（round85 `4b2452c4`）。
- 083 无 `CUIEquip`/`CUIPetEquip` 的 `OnCreate` 符号 ⇒ 无布局立即数，坐标按门禁 18 走截图 SAD 备选；不得据 IDA 缺符号判「虚构」。
- 本轮不改 `iconpositions` / `pet_panel_pos` / 任何按钮坐标 / 任何行为字面量；§44 的 60 节 / 142 处真机标记与五屏仍全部由用户认领。
- 外部程序在途 11 个源码 WIP 原样留工作树，不夹带进本轮提交。

## 46. round87 · 五面复核归档 + 真机认领清单（#devfix-five-face-review-archive，需用户一起测）

本轮 **0 源码 / 0 协议 / 0 WZ 改动**：只把 round73-77 / round84 会话内五面只读取证结论结构化归档，不重做取证、不改已对齐坐标。

| 项 | 面 | 验收动作 | 本轮状态 |
| --- | --- | --- | --- |
| 46-1 | UIChannel 选频道 | 真机开频道窗，核对 plate 三态 / y=99 双钮 / 末排灰板与原生帧一致 | ⬜ 未执行，用户认领 |
| 46-2 | UICharSelect 选角色 | 真机核对 sub_602ADC 标签值坐标表 / 占用1384 空1348 / PAGESIZE=3 三按钮 | ⬜ 未执行，用户认领 |
| 46-3 | UICommonCreation 建角 | 真机核对 init_modern NX 资源驱动 + classic 1280x720 截图锚 / BtViewAll 真位图 | ⬜ 未执行，用户认领 |
| 46-4 | UIMiscShells 杂项壳 | 真机核对 load_window_bg 真 NX 底板 6 处 / 界面气球 7 件 / 宏指令 379x291 | ⬜ 未执行，用户认领 |
| 46-5 | UIItemInventory 物品栏 | 真机核对 slot_col=4 / slot_row=6 / slot_pos(8,50) 展开态逐格对齐 | ⬜ 未执行，用户认领 |

已知限制：

- 本归档是只读取证层结论，不能替代实机观感验收；五屏截图（60 节 / 142 处标记）全部由用户认领。
- 五面均判「已对齐 / 合法混合面」：真 NX 资源根 + 真 083 EA 立即数 + 真位图；截图 SAD 仅补 083 无 OnCreate 立即数处（门禁 18 备选合法）。
- 结构性差异（UIMiscShells DIY 壳 / UICommonCreation classic 截图锚）按「校准≠改回原生 / s9 优先」登记 P3 长尾，不做架构级重写。
- 外部程序在途 11 个源码 WIP 原样留工作树，不夹带进本轮提交；本轮 build 为门禁回归（零源码改动，不声称真重编覆盖）。

## 47. round88 · 三方一致性确认（只读取证 / 零源码改动）

47-1 **Q1 复核**：Angel(13339)/BeiDou(13337) 同一份 v83 二进制两 IDA 实例；当前端 HeavenClient 自研重实现，79 条保持与「两者一样」无因果，真实理由 s9 联机优先。
47-2 **握手/opcode 一致**：s9 hello version 83、GATE OK `a78ba681a4`（client_out_sent 148 / s9_send 314）。
47-3 **body 层不一致**：s9 改造 v83（exp 8B / level 2B / HP-MP int / GachaExp 4B / spawn tail int0 / removeItemFromMap 恒写 4B cid）→「全部按 Angel 对齐」前提不成立，改回断联机。
47-4 **原逻辑错误已证**：`LoginParser.cpp:150-174` 白屏案例 + 30+ 按 Angel 083 翻案重修 commit。
47-5 **reopen 27 条**（PURE20+MIXED7，不碰 wire 却被一刀切 KEEP）列 `three-way-consistency-round88.json::reopen_candidates`，须逐条拍板；架构级重写须单点裁决，本轮不自动改。

限制：① 0 源码/0 协议/0 WZ 改动，只读取证；② 11 个外部在途 WIP 原样留工作树；③ 真机五屏截图 60 节/142 处仍由用户认领；④ reopen 27 条为「待裁决」非「已改」。

## 48. round89 · W/L/E 三桶 + UIQuestLog 清选中（L 桶首例）

48-1. 口径修订：round88 §0/§2.2/§5 的「不触发全部对齐」收窄为 W（6 字段保持 s9）/ L（同字段原始逻辑按 083 还原）/ E（扩展不对齐）；CONSISTENCY-VERDICT.md 追加 §7，历史证据链不删。
48-2. 真机归口（用户认领）：切换任务分类页签后，摘要页是否显示新页第一条任务、不再残留上一页选中项；展开态/空列表/无可见任务三种边界。
48-3. 真机归口（用户认领）：宠物装备面板等 L 桶后续批次的观感对齐（承接 §45 四项）。
48-4. ~~drop animation=0/4 分支还原后的消亡/爆炸帧观感~~ **round90 已注销（见 §49）**：D12 判 W 桶 wire 宽度分叉、D2 判 F 维 P2，两者均不还原、不需要该观感验收；真机仍认领 `Drop::expire` 0/1/2/4/5 五档消失行为回归。
48-5. 验收：harness 全绿 + GATE OK + 真重编三证（build_log/EXE/UIQuestLog.cpp 覆盖）+ 零署名提交 + 17 个外部在途 WIP 零夹带。

限制：① 本轮 1 行源码 + 4 行注释，零 wire / 零协议 / 零 WZ；② 17 个外部在途 WIP 原样留工作树；③ reopen 27 条为 P1 待逐条拍板，非本轮全改；④ 架构级重写单点裁决、真机五屏截图 60 节/142 处仍由用户认领。

## 49. round90 · drop 面 W/L/E 误分类纠偏（纯注释 / 零行为改动）

49-1. 触发：m10601「还有未完成的或者你可以自测的任务吗？」→ 按 m09211/m09729/m09737 授权自测推进下一批，先复核 round89 §2.3/§5 的 P1 登记，挖出两处误分类。
49-2. 决定性证据：083 活反编译 `tools/_ida_r90/OnDropLeaveField.code.txt:83-99`（269 离场，非 268 进场）= `v2==4 → Decode2(delay)` + `*(v4+68)=get_update_time()+delay`、`v2==0 → IWzGr2DLayer COM`；s9 `PacketCreator.java:2624-2642` 对 `animation>=2` 恒写 `writeInt(chrId)` 4B。
49-3. 判定：`drop/D12` = **W 桶**（wire 宽度分叉，保持 s9 读 int cid，改回 083 读 short 平移字段断联机）；`drop/D2` = **F 维 P2**（COM 画布栈缺失，48 帧淡出语义等价，不补）。均从 `L.p1_registered` 注销；168 条历史裁定与 §4 算术不动，reopen `27 = 25 P1 + 1 W + 1 F-P2` 闭合。
49-4. 真机归口（用户认领）：`Drop::expire` 0/1/2/4/5 五档消失行为回归；承接 §45/§46/§47/§48 其余真机项不变。
49-5. 验收：harness 全绿 + GATE OK + 真重编三证（build_log / EXE / Drop.cpp 覆盖）+ 零署名提交 + 25 个外部在途 WIP 零夹带 + feedback 无 rating。

限制：① `Drop.cpp` 仅补三方对账纯注释，0 行为字面量 / 零 wire / 零协议 / 零 WZ / 零新封包；② 25 个外部在途 WIP 原样留工作树；③ 下一批改走 `quest/D8` / `skill/D4` / `corewindow/D8` 合法 L 桶候选，门禁 5 禁想象修改；④ 架构级重写仍单点裁决、真机五屏截图 60 节/142 处仍由用户认领。
## 50. round91 · 下一批一次性 Plan（3 条候选纠偏 / 计划轮零源码改动）

触发：m10739「开始 下一批 的计划」+ m10738「记录一次有可见 PUA 输出的交付，不记录评分。」
主交付：`docs/features/angel-calibration-d-matrix-2026-11/NEXT-BATCH-PLAN.md`（门禁 1：确认计划后才实现）。

50-1. **3 条候选全部不进 P1**：`quest/D8` = W 桶 wire 条件追加段（083 `CQuest::StartQuest @0x71751c` `COutPacket(107)` → `if (sub_721008(questid)==0){Encode2(x);Encode2(y);}`；`sub_721008 = IsAutoStartQuest || sub_720FED`；s9 `QuestActionHandler.java:48-64` `if (p.available() >= 4)` 容忍有/无 x/y；当前端 `Net/Packets/QuestPackets.h` 无后缀=合法 s9 形态）→ 保持 s9 零改动；`skill/D4` = 架构级（`CWndSkillGuide::OnKey @0x8aa4e4` 全库 2 符号，当前端 `UISkillBook.cpp:797` ESC 闭环）→ 不新增；`corewindow/D8` = K3 DOC_ONLY 已发货（round75 `c07b1a7b`），083 6 掩码/500ms/5 分支属参考值，s9 `DistributeSPHandler` 无 500ms 限速 + `AssignAPProcessor` 不同编码，当前端 `PlayerPackets.h:48-72` 已注明勿复用 → 只补逐字对照。
50-2. **同族病第三次**：WLE-TRIAGE-PLAN.md `:96/:123/:152-153` 三处纸面登记（含「无同族误分类」）被 083 活 IDA 12 份反编译 + s9 5 handler + 当前端 + DECISIONS 四方证伪。硬规则写进 Plan §3 / WLE §8：任何「下一批 L 桶」登记前必附 083 反编译 + s9 handler + 当前端字面量 + DECISIONS 原裁定四项，不从矩阵文本推导批次。
50-3. **真干净下一批（自带 083 锚点）**：P1 首选 `skill/D12`/`corewindow/D9` 宏选中态 `CUIMacroSysEx::OnSelected @0x8a984d`（20 行纯 UI）+ `CUIMacroSys::OnKey @0x8a98dc`（ESC→`ProcessBasicUIKey(27,a2)`），零 `COutPacket`/`Encode`/`SendPacket`；P1 次选 `CUIChannelShift::OnKey @0x995863` 6 列方向键纯索引，`CUIChannelSelect::OnKey @0x628a1c` 须先拆 L/W（导航=L / `SendViewAllCharacterPacket`=W 保持 s9）；P2/F 维 `CUISkillEx::ShiftMacroUIState @0x8bad5a`（243 行 COM 画布栈，当前端无 COM 不补）。
50-4. **门禁与算术**：orange-wz 128B `WzAESConstant.DEFAULT_KEY` base64（172 字符）+ iv `TSPHKw==` 对 `UIWindow.img` 的 `KeyConfig`/`Stat`/`Skill` 三根全 `isError=false` → `GATE_OK_128B_IMG_REACHABLE`（`tools/_ida_r91/img_gate.json`）；168 条 DECISIONS 历史裁定 / WLE §4 `38+130=168` / reopen `27 = 25 P1 + 1 W + 1 F-P2` **全不动**，仅 P1 登记层注销 3 条；`GATE OK s9 a78ba681a4`；harness 全绿；零署名原子提交。
50-5. **限制**：计划轮 **0 源码 / 0 wire / 0 协议 / 0 WZ / 0 新封包**；`tools/_ida_r91/` 只读取证留痕；工作树外部在途 WIP 原样保留、精确 add 隔离；真机五屏截图 60 节/142 处、`InPacket::available()` 393 处 §27-A 逐面回归、D2 聊天规则仍由用户认领；下一批实施前须用户确认一次性 Plan。
## 51. round92 · 实施片1+片2（编号错配纠偏 + UIChannel 下溢修复）

- **51-1** 本服 `channel_count=3` 打开换频窗，按 ↑/↓ 不再出现 253/254/255 幽灵选中帧或 3/4/5 越界高亮；左右键与 ESC 行为不变。
- **51-2** 多频道服（>=7）换频窗 ↑/↓ 跨行正常、6 列落位与 083 `CUIChannelShift` 一致。
- **51-3** 片1 宏组合栏选中态真机观感（DOC_ONLY 未改，`shown_name` 变更检测保持）。
- **51-4** 硬规则：NEXT-BATCH-PLAN.md 任何 P1 候选登记须附 083 反编译 + s9 handler + 当前端字面量 + DECISIONS 四项（harness A12 守卫）。
- 限制：① 零 wire / 零协议 / 零 WZ 改写 / 零新封包；② 4 个错配编号只在 P1 登记层注销，168 条 DECISIONS 裁定与 reopen 27 算术不动；③ W 桶 6 字段 + 27 wire KEEP 零改动；④ 真机五屏截图仍由用户认领。
## 52. round93 · 095/v95 依据链纠偏（5 处纯注释）

- **52-1** 福利中心（EVENT 键）打开/关闭行为与旧版完全一致（本轮零行为改动，只改注释依据）。
- **52-2** 传送门行为不变：碰触类（TOUCH / SCRIPTED_TOUCH）行走经过即触发，其余门按上键触发；弹跳（SPRING1/2）不受影响。
- **52-3** 若后续要把 083 `CUserLocal::CheckPortal_Collision @0x94dac6` 的「按 portal type 分派」（`9` 脚本门 / `12` / 其余传送）搬进本端 `MapPortals::find_warp_at` 两通道抽象，属行为级变更，须先立决策点（当前判「不搬」：本端 PortalScope 是等价抽象，搬会改触发时机）。
- **52-4** 硬规则：注释里引 083 符号必须先跑 `entity_query` 确认符号真实存在；`*CPortal@@*`/`*CPortal::*` 这类旧引用在 083 全库命中为 0，写进注释即依据链污染。
- 限制：① 纯注释 5 处，零行为 / 零 wire / 零协议 / 零 WZ；② `DECISIONS.md:130` 历史行不改写（只在本节记明）；③ 真机五屏截图仍由用户认领。
## 53. round94 · reopen 残余 17 条 P1/P2/P3 分流（账目轮 / 零源码改动）

**触发**：User said (m11928)「处理掉：剩余 reopen 中 21 条属 P1/P2/P3 分流」。

**先纠算**：上一轮答复里「21 条」是漏算 round91 的 3 条注销。现场实数：
27 条 reopen 已被前四轮注销 **10** 条 → **残余 17 条**（`27 - 10 = 17`）。

注销台账：

| 轮次 | 注销条数 | 条目 |
| --- | --- | --- |
| round90 | 2 | `drop/D2`（F 维 P2，缺 COM 渲染栈）、`drop/D12`（W 桶 wire 宽度分叉） |
| round91 | 3 | `corewindow/D8`（DOC_ONLY 已发货）、`skill/D4`（架构级 → 不新增）、`quest/D8`（W 桶 wire 条件追加段） |
| round92 | 4 | `corewindow/D9`、`skill/D12`、`login-flow/D9`、`statusbar-hud/D12`（均为**编号错配**，真议题另有其人） |
| round93 | 1 | `activity-minigame/D1`（095 注释已改判本端扩展 E 桶） |

**分流结果**：**P1 = 0 / P2 = 1 / P3 = 16**（算术 `0 + 1 + 16 = 17`）。

- **P2（1 条）**：`login-flow/D3` 骑士团条件解码分支 —— 缺 s9 侧是否发追加段的协议事实；
  拿到 s9 抓包即可升 P1，拿到前按门禁 5 不发明解码分支。
- **P3（16 条）**：`login-flow/D12`、`login-flow/D13`、`statusbar-hud/D1`、`corewindow/D2`、
  `corewindow/D10`、`social-economy/D3`、`social-economy/D9`、`social-economy/D12`、
  `pet-summon/D2`、`cashshop/D2`、`skill/D9`、`skill/D11`、`map/D1`、`map/D4`、`map/D8`、
  `quest/D5` —— 全部是 canonical 已裁定「保持 / 不补 / 不做 / 不改」的现状保持项，登记在册防重复翻案。
- **P1 = 0**：P1 门槛（083 状态机证据 + 零 wire + 当前端确有缺陷）三条并发，残余 17 条无一条满足。

**验证**
- `tools/test_round94_reopen_triage.py`：静态守卫 + 场景全绿（下条命令）。
- `python tools/gen_opcode_matrix.py --check` → `GATE OK`（账目轮零源码改动，不声称真重编）。
- 算术不动：168 条 DECISIONS 裁定 / `PURE76+WIRE38+MIXED14+OTHER40=168` / W 轨 38 + L 轨 130 = 168 /
  reopen 27 分母不变（17 分流 + 10 注销）。
- 外部在途 WIP 零夹带。

## 54. round95 · D3 命名纠偏 + parse_stats SP 表分支 + 11 处措辞 + P3 转重做

- **54-1** `Net/Handlers/Helpers/LoginParser.cpp`：`parse_stats` 增加 `has_sp_table(job)` 分支。
  真机待验：**Evan / 龙神系角色（2001 / 2200 / 2210–2218）登录 + 换频道 + 进图**，
  选角页 SP 显示为 0、进图后技能窗 SP 不再错位；非 Evan 角色 SP 行为不变。
- **54-2** `DECISIONS.md` 11 处「待裁决/待拍板」措辞改为状态表述（0 行为变更，纯文档）。
- **54-3** `P3-ANGEL-REBUILD-PLAN.md`：P3 16 条转 A/B/C 三批重做，非本轮实施。
- **54-4** 083 门控：`GW_CharacterStat::Decode @0x4e2a84` 判据 `JOB/100 == 22 || JOB == 2001`
  为 **Evan 系**（2200–2299），非骑士团（Cygnus 1000–1599）；旧命名已纠偏。
- **54-5** 限制：① 未抓包，协议事实取自 s9 源码（`PacketCreator.addCharStats` /
  `GameConstants.hasSPTable`）；② 多职业 SP 表内数值只消费不落库（不发明模型）；
  ③ W 桶 6 字段 + 27 条 wire KEEP 零改动。

## 55. round96 · P3 A 批前 3 条（D12 / D11 / D10）

- **55-1** `login-flow/D12` `DOC_ONLY` 完成：`docs/features/angel-calibration-d-matrix-2026-11/BYTE-LEVEL-STATS.md`
  逐字段对照。真机待验：**非 Evan 与 Evan 角色各一个**，登录 → 选角 → 进图 → 换频道，
  确认 stats 段不错位（等级 / SP / exp / 地图 id 四项即可判断）。
- **55-2** `skill/D11` 定性：`BT_HYPER / BT_GUILDSKILL / BT_RIDE` 三钮**不属于 083**
  （`Skill` 无 `main` 子节点；083 `CUISkill::OnCreate` 按钮 id 为 2010..2013 / 2023）。
  当前端保留为扩展占位 + DISABLED。真机待验：技能窗打开后三钮不显示、不响应（预期）。
- **55-3** `corewindow/D10`：083 外层硬编码 **666×524**、内容区 `(12,34) 642×472`；
  本端按 `Border` 件推导 **654×521**、内容 `(7,33) 640×470`，差 12×3。
  真机待验：世界地图窗运行时四周边框是否完整、内容是否贴边无空隙。
  若观感有差，须连同 `Border` 九切片重排一起改，不单独挪尺寸。
- **55-4** 三条均零 wire / 零行为改动；源码侧只有 `UISkillBook.cpp` 与 `UIWorldMap.cpp` 的注释。
- **55-5** 限制：本轮未做 A 批余 6 条与 B/C 批；`DECISIONS.md:101/197/288` 的历史结论
  保留原样，新口径写在 WLE §13 与 P3 Plan §6。

## 56. round97 · P3 A 批余 6 条（六条全 DOC_ONLY / 零源码改动）

- **56-1** `map/D1`：083 `SaveFuncKeyMap @0x58df2f` 89 槽 × 5B 差分 + `sub_4E43FA` 5B 条目，
  s9 mode0 同形；本端 `ChangeKeyMapPacket` 已对齐。真机待验：改 2~3 个键 → 退出重登，
  确认键位保留且未被重置（服务端 `keymap` 表应只增改对应行）。
- **56-2** `map/D8`：083 `SaveQuickslotKeyMap @0x72b87e` = `183 + 0x20` raw；s9 严格 32B。
  真机待验：改快捷栏触发键 → 重登确认 8 槽保留；若整包被服务端丢弃（键位回到默认），
  即为长度不匹配信号。
- **56-3** `cashshop/D2`：083 `CCSWnd_*` 7 类 / 70 符号 vs 本端单类，**不拆窗**。
  真机待验：商城开窗 → 切分类/翻页/预览角色/看 locker/wish，确认无空白页与错位。
- **56-4** `map/D4`：083 `CWorldMapDlg` 21 符号含 `Deeper/Shallower` 层级导航；
  IMG `WorldMap` 仅 `title`+`Border`、`WorldMapSearch` 不存在。真机待验：世界地图开窗 →
  点击地图节点跳转，确认侧栏与传送正常（搜索框预期不显示）。
- **56-5** `statusbar-hud/D1`：083 `CUIStatusBar` 45 方法簇 `8/7/1/8/14/7` 全部有本端归属。
  真机待验：HUD 全功能走一遍（快捷栏折叠/展开、聊天三态、Boss 血条、Buff 列表、时钟、
  组队、低血闪烁），确认无缺失交互。
- **56-6** `social-economy/D3`：083 无独立联盟窗（`CUIAlliance` 0 符号），入口是共用窗按钮；
  本端保留 `UIAlliance` 浮窗 + UNION 页签。真机待验：G 键 → 家族页签 → 联盟按钮，
  确认浮窗可开可关、页签不串位。
- **56-7** 限制：A 批 9 条完成；B 批 5 条、C 批 2 条未做。本轮**零源码改动**，
  故不声称真重编覆盖。构建门禁回归结果如实记录：`GATE OK`（协议门禁通过），
  但 MSBuild 被**工作树里他人外部在途 WIP** 阻断 —— `UIChatBar.cpp(2581/2595) error C2664`、
  `UIGender.cpp(189) fatal error C1070`、`UIMedal.cpp(38/67/161)`、`UISpecial.cpp(22/96)`、
  `UITemporaryStatView.cpp(193)`，共 9 error / 3 fatal，全部落在本轮从未触碰的文件
  （`git status` 均为 `M`/`??` 的外部改动，且这些文件里不含本轮任何标记串）。
  `EXITCODE=1`。round96 真重编（19:22）时这些文件尚未坏，坏点来自此后外部推进，
  与本轮交付无关；待外部 WIP 修好后需重跑一次门禁回归确认基线回绿。
