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

## 28-A. 待裁决：残留 095 线地址污染（5 处，本轮已取证未改）

> 与 `corewindow/D5` 同源，但不在该决策点的 4 处范围内，且 `Player.cpp:211` 的 083 函数名尚未定位，故只登记不擅改。

| # | 位置 | 现值（095） | Angel 083 真地址（已实测） |
| --- | --- | --- | --- |
| 28-A1 | `Net/Packets/MovePathEncode.h:2-3` | `Maplestory95.i64 @ 0x666e20` | `?Encode@CMovePath@@QAEXAAVCOutPacket@@PAH@Z @ 0x68a563`（0x269） |
| 28-A2 | `Net/Packets/NpcInteractionPackets.h:42,44` | `CNpc::GenerateMovePath @ 0x671590`、`Encode 权威 0x666e20` | `?GenerateMovePath@CNpc@@IAEXJJ@Z @ 0x6d3273`（0x11f）、`CMovePath::Encode @ 0x68a563` |
| 28-A3 | `Net/Handlers/WaveAShellHandlers.cpp:3585` | `IDA 0x666e20` | 同上 `0x68a563` |
| 28-A4 | `Gameplay/MapleMap/Npc.cpp:253` | `CMovePath::Encode @ 0x666e20` | `0x68a563` |
| 28-A5 | `Character/Player.cpp:211` | `对照 port-095 CWvsContext::CheckTemporaryStatDuration` | 未定位（083 IDB `*CheckTemporaryStat*` 0 命中，需换检索维度） |

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
