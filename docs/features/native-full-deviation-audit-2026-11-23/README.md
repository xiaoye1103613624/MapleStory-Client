# 全原生基础功能 / 游戏内容实现偏差全面审计（2026-11-23）

> 性质：**只读审计**（门禁 0/阶段 0–0.5）。未改任何源码、未编译、未真机发包。用户拍板后另立 Plan 实施。
> 触发：用户报「很多基础功能不是少特效就是排版、内容渲染、按钮错误逻辑、职业技能、怪物等方面存在偏差」，要求「所有原生功能都要查一下」，参照 **MinMS 源码** + **Angel 083 IDB 既有证据** + **095/192 客户端交叉**，逐条研判扩展。
> 基线：当前端 `VERSION="0.83"`（`Configuration.h:316`）+ s9 协议；部署 `E:/MXD/BeiDou-Client-xy`。HEAD `f47f08f8`，工作树 209 文件未提交改动（行号取当前工作树真实值，旧文档行号多已漂移）。
> 工具边界：orange-wz/IDA MCP 未暴露为可调用工具；但按用户 D3 授权，用现成脚本（`tools/_d3_native_fix_resource_probe.py` / `_d3_skill_effect_deep.py`，128B `WzAESConstant.DEFAULT_KEY` + IV `TSPHKw==`，`load_files exclusive:false`→`query_nodes`→`unload_all`）直连 10012 完成 D3 资源核对（结果见 §1.1）。IDA IDB 本会话未重跑。**像素/真机观感结论未经本端真机验收**。

## 0. 一句话结论

**不是大面积「功能没实现」**：151 个 UIElement 类型、305 条收包注册、148 个出站 opcode 主链成型；既有 2026-11-22 基线审计已修复三组 P0（快捷栏拖拽/药水键/瞬移二段跳空壳）。本轮 8 域全量扫描确认——相对 MinMS（Angel 语义 C++ 落地版），真实偏差收敛为 **P1×11 / P2×约30 / P3×约20**，集中在四类：**① 接线断链（包类/按钮/函数体空）——修复成本最低、体验最硬；② 特效族系统性缺失（技能/怪物/快捷栏）；③ 位移与坐标系降级（瞬移落点、飞行怪、火花坐标）；④ 数值/守卫缺失（EXP/HP%、display 字节、伤害公式）**。多数 ❌ 为既有文档已登记的显式预留（活动 HUD/小游戏/商城壳），按「不发明协议/不发明能力」门禁不构成缺陷。

## 1. P1 — 已证实的真缺口/偏差（逐条代码行号，均已 lead 源码复核）

| # | 域 | 功能项 | 当前端证据 | 对照源证据 | 缺口形态 |
|---|---|---|---|---|---|
| P1-1 | 技能 | 瞬移位移语义降级（无 foothold 探测/吸附） | `Character/Player.cpp:724-758`：`set_x` 直线改坐标、垂直 `vforce` 冲量；`Combat.cpp:328-360` | MinMS `Combat.cpp:1759-1790`（fhid==0 拒绝 + `attach_floor` 落点吸附）、`:1824-2021` find_teleport_target | 落点穿地板/不上台阶；非「空 case」复发而是已接线版的深度不足 |
| P1-2 | 技能 | 技能特效族整体缺失（prepare/screen/special/tile/receiver/keydown/finish） | `Gameplay/Combat/Skill.h:40-55` 仅 useeffect/hiteffect；`Skill.cpp:49-115`；`SkillUseEffect.h:22-115` 仅 6 类；`Combat.cpp:161-282` 不调任何 area/screen 特效 | MinMS `Skill.h:73-82`、`Skill.cpp:214-251`、`Combat.cpp:2333-2478`；Angel r05 `ShowSkillEffect/ShowSkillPrepare/ShowSkillSpecialEffect` | AoE/蓄力/受术技能「打得出看不到」（服务端仍结算）；须先确认本端 Skill.nx 是否含对应节点 |
| P1-3 | 技能 | 攻击包 display 字节恒 0 | `Gameplay/Combat/Attack.h:134 uint8_t display=0`（全工程无赋值）；`Net/Packets/AttackAndSkillPackets.h:85 write_byte(attack.display)`（入站 `AttackHandlers.cpp:48` 有读，出站无写） | MinMS `Combat.cpp:174-190 fill_attack_wire_action` | 他人客户端看到固定默认姿态，组合动作无法表达 |
| P1-4 | HUD | EXP%/HP%/MP% 溢出守卫缺失 | `UIStatusBar.cpp:2096-2105`（exp/required_exp 无满级早退、无 clamp）、`:2112`/`:2120`（hp/maxhp、mp/maxmp 无 ≤0 守卫）；绘制 `:733`/`:3233-3235` | MinMS `UIStatusBar.cpp:1061-1097`（满级 return 0、max≤0→0） | s9 999 级场景 EXP 条错显、HP/MP 可除零 inf/NaN |
| P1-5 | 社交 | 仓库金币存取空函数 | `UIStorage.cpp:262-266 prompt_meso()` 空体；钮 `:151-156`、键 `:223-226`；`StorageMesoPacket`（`StoragePackets.h:68-75`）全仓零 dispatch | s9 `StorageProcessor.java:200-201`（case7 readLong）；MinMS `UIStorage.cpp:75-78` | 点金币存取钮完全无反应（物品存取正常） |
| P1-6 | 社交 | 玩家交易金币设置空函数 | `UITrade.cpp:320-323 prompt_meso()` 空体；钮 `:294-297`、键 `:347-348`；`TradeSetMesoPacket`（`TradePackets.h:85-93`）零 dispatch | MinMS `UITrade.cpp:405-428`（UIEnterNumber + TradeAddMesoPacket）；s9 SET_MESO=0x10 | 交易无法附加金币 |
| P1-7 | 社交 | 快递 Duey 寄送死代码 | `UIDelivery.cpp:238-244`、`:281-285`：`if(!pre.empty()){}else{}` 两分支空，内含 `DueySendPacket(...).dispatch()`（`:235`/`:276`）的两个 lambda 永不可达；`ask_meso`（`:144-147`）空 | s9 `DueyHandler.java:40-50`；`DueyPackets.h:20-37` 字段序已对齐 | 收/领/弃正常，唯「寄出」整条断链 |
| P1-8 | 社交 | 便签新建/回复空块 | `UIMemo.cpp:115-124`（N 键）、`:164-176`（BtSave）：取到 `pre` 后空块；`NoteSendPacket`（`NotePackets.h:31-41`）零 dispatch | s9 `NoteActionHandler.java:46-49` | 列表/删除正常，无法写新便签/回复 |
| P1-9 | 社交 | NPC 头顶任务标记整族缺失 | `UIMiscShells.cpp:6755-6819 UIQuestIcon` 仅背景+tip，构造时对选中任务发包，无世界层按 NPC 状态图标；`Gameplay/MapleMap` 无 QuestMarker 类 | MinMS `QuestNpcMarkers.cpp/.h`（4 态 COMPLETE>AVAILABLE>IN_PROGRESS）；`npc-quest-fullchain-audit README:163,199,216` 标最大可见差距 | 行走地图 NPC 头顶无任何任务提示；需 083 IMG 资源支撑 |
| P1-10 | 商城 | BtGift 礼物确认回调空 | `UICashShop.cpp:930-933`：`UIYesNo` 回调 `if(!yes)return;` 后空，确认后不发任何包（真发包仅在另一 SoftKey 路径 `:713-714`） | Angel `CCashShop` 赠礼 `Encode1(4)`；s9 `CashOperationHandler.java:195-217` action==4→cs.gift | 列表每行礼物钮点「是」无效果；服务端已就绪，纯前端断点 |
| P1-11 | 地图 | 飞行怪物理被钉地 | `Physics.cpp:42-72`：`:45` 无条件 `update_fh`、`:59` FLYING 用 `limit_movement`；`FootholdTree.h:48` 无 `limit_horizontal` | MinMS `Physics.cpp:48-58`（`if(type!=FLYING) update_fh`）、`:74`（`limit_horizontal`）；`FootholdTree.h:43` | 飞行 template 怪（帕普拉图斯/龙王飞龙）沿地面走而非悬浮；改动面小 |

> 另有「每日签到非 v83 原生（192 资源 + 私有 0x3721/0x3722）」为**有意基线偏离**（`UIDailyGift.cpp:77,129-134`，双端已闭环），按用户 D5 认可，登记为有意例外。

### 1.1 D3 资源核对结论（orange-wz 直连 10012，128B key，`isError:false`）

| 资源项 | 核对结果 | 对 Plan 的影响 |
|---|---|---|
| 技能特效族（P1-2） | 本端 live `Skill/*.img` 实际节点前缀 `skill/<skillid>/`；search 结果**只有 `effect`（10）、`hit`（4），无 `prepare/keydown/keydown0/keydownend/finish/screen/special/tile/receiver` 任何节点** | P1-2 按「资源存在部分移植、其余显式预留」处理：只移植 `effect`/`hit` 对应逻辑，8 类缺失族在 WZ 无源，**只预留播放点不造资源**；特效族不作为可闭环项，降级为「资源阻塞预留」 |
| NPC 任务标记（P1-9） | `UIWindow.img` 存在 `QuestIcon`、`QuestAlarm`、`Quest`、`Quest/BtMarkNpc`、`MobGage` 节点（LIST_PROPERTY） | 资源在位，P1-9 可做；但 MinMS `QuestNpcMarkers` 4 态算法依赖 `Quest/Check.img` + NPC 位置数据，实施期再核 `Quest.img` 具体节点 |
| 受击火花（怪物 P2） | `Effect/BasicEff.img/CoolHit` 存在 → 火花 P2 是**纯坐标 bug**，与资源无关，可直接修 `Mob.cpp:925` 为 `Point(0,-15)` | 直接修 |
| 怪物出生特效（地图 P2） | `Mob/` 目录无任何 `SpecialEff`/`*Eff*.img` → 资源确实缺源 | 按既有裁决保留预留，不造资源 |

## 2. P2 — 主要偏差（精选，完整表见各域报告）

| 域 | 功能项 | 当前端证据 | 对照源证据 | 形态 |
|---|---|---|---|---|
| 技能 | 瞬移施放光圈特效缺失 | `Player.cpp:724-758` 无 show_attack_effect | MinMS `Combat.cpp:1783-1786` BasicEff/Teleport | 缺特效 |
| 技能 | 瞬移 15-tick 冷却门禁缺失 | `Combat.cpp:127-159 use_move` 无 teleport 判断 | MinMS `Combat.cpp:383-384,1788` | 连按可每帧位移+发包 |
| 技能 | 冲锋位移降级（无地形步进/怪物吸附） | `Combat.cpp:376-384` 仅停最远怪 x | MinMS `Combat.cpp:2065-2122`+magnet_pull | 距离过短、无拽拉动画 |
| 技能 | 后坐力射击(RECOIL_SHOT)位移缺失 | `Combat.cpp:362-374` 无该 case；`SkillId.h` 无定义 | MinMS `Combat.cpp:2047-2063`（引 Angel TryDoingShootAttack） | 弩手后跃无本地位移 |
| 技能 | 动作帧选择降级（最终一击/攀爬/双飞斩等） | `Skill.cpp:117-145,172-175` | MinMS `Skill.cpp:318-352,491-528` | 动作帧用默认 |
| 技能 | 元素克制缺席 | `Attack.h:28-78` 无 element；`Mob.cpp:960-1047` 无 adjust | MinMS `Attack.h:78`+`Skill.cpp:576-593`+`Mob.cpp:7438-7470` | 仅影响客户端显示（s9 权威结算） |
| 技能 | 魔法 mastery/ignoredef/专属倍率差异 | `Player.cpp:661-673`（min=max）；`Skill.cpp:212` 加法近似 | MinMS `Skill.cpp:608-654` | 同上 |
| 技能 | 蓄力(KeyDown)状态机缺失 | `AttackAndSkillPackets.h:305-308` 注释自认 | MinMS `Skill.cpp:214-217,411-421` | 持续技按住/松开缺口 |
| 技能 | 技能专属使用门禁缺失（能量/变身/船上/工匠之魂） | `Combat.cpp:127-159` 仅两分支 | MinMS `Combat.cpp:403-531` | 越权施放/无开窗响应 |
| HUD | 快捷栏拖出清除/unbind 整段缺失 | `UIStatusBar.h:147,150` 空实现；`UIStateGame.cpp:273` no-op | MinMS `UIStatusBar.cpp:915-935` | 拖半路松手图标留原格 |
| HUD | 快捷格技能 CD 倒计时遮罩缺失 | `UIStatusBar.cpp:3273-3284` 只画裸 Texture | MinMS `UIStatusBar.cpp:440-459` ColorBox 遮罩 | 缺特效 |
| 地图 | 相机震屏累加漂移 + 无衰减 + 无 clear | `Camera.cpp:60,82-94`（抖动写回 x/y） | MinMS `Camera.cpp:103-123,125-203` 独立 offset + decay | 随机游走漂移 |
| 地图 | MapEffect 能力缺口 | `MapEffect.cpp:28-51`（52 行，单 path 单动画固定坐标） | MinMS `MapEffect.cpp` 640 行（scene 分段/音效/token 变体） | 部分地图特效静默不播 |
| 地图 | FIELD_OBSTACLE 降级为玩家脚下假文本标记 | `Obj.cpp:26-59`；`WaveAShellHandlers.cpp:1917-1990` | MinMS `Obj.cpp:38-69`（真实 hitbox/damage） | 陷阱无视觉无伤害碰撞 |
| 地图 | 物理常量分歧（摩擦/坡度/游泳重力） | `Physics.cpp:22-28`（0.5/0.1/0.03） | MinMS `Physics.cpp:23-32`（0.3/0.15/0.055） | 手感不一致，需真机定夺 |
| 地图 | 怪物出生特效 effect>0 未播 | `Mob.cpp:171-174` 仅存值 | `SpecialEff.img` 资源缺源（已披露） | 出生烟雾/石棺无特效 |
| 怪物 | 受击火花(CoolHit)坐标系错误 | `Mob.cpp:925`：`Point(phobj.get_x(), phobj.get_y()-15)` 世界坐标当相对偏移；父位 `absp` 见 `:667/:702` | MinMS `Mob.cpp:7331-7347` `Point(0,-15)`；同仓 `show_effect:888-905` | 火花被甩到离怪几百像素，常规打怪几乎看不到 |
| 怪物 | 非受控/旁观怪 MobSkill 特效缺失 | `play_mob_skill_effect`（`Mob.cpp:558-573`）唯一调用点 `:548 update_movement` 受控支；`send_movement:752-797` 只切 stance | s9 VFX 纯客户端驱动；MinMS 施法路径不区分受控 | 多人同屏 Boss 技能/毒雾等旁观无 VFX |
| 登录 | View Choice 选世界后频道板不渲染 | `UIRecommendWorld.cpp:253-266`→`UIWorldSelect.cpp:1351-1381 change_world` 未置 `world_selected=true`/未触发展开；绘制门闩 `:697/:749`；正常路径 `:1530-1552` | Angel `CUIRecommendWorld::SelectWorld@0x62b96b` | 经推荐世界选世界后停在空面板 |
| 社交 | 交易聊天/快捷键加物品/二次确认 | `UITrade.cpp:325-328`（chat 空）、`:353-380`（算完丢弃不发包）、`:281-288`（单击即锁） | MinMS `UITrade.cpp:391-428` | 画了聊天框却不能发言、快捷键不能放物 |
| 社交 | 公会徽章编辑/好友分组窗/MapleTV | `WaveAShellHandlers.cpp:4786-4802` 只收不发；`UIUserList.cpp:1834` 仅文本；`UIMapleTV.cpp:60-62` 空 | Angel 各窗具名方法 | 有按钮无实质行为 |
| 商城 | 快捷键 8 处空分支 + BtHelp 错接 | `UICashShop.cpp:1111-1163`（B/S/G/T/P/K/I/X/C 空）；`:719-724` BtHelp→EnterMtsPacket | Angel `CCashShop` 功能族 | 键盘操作无效、帮助钮跳拍卖 |
| 任务 | 任务日志四钮 MARKNPC/OK/NO/ALERT 点击无行为 | `UIQuestLog.cpp:1080-1089` 统一 return | Angel 原生「寻找NPC位置」 | 显式预留（无 npc→map 反查表） |

## 3. P3 — 次要/空态/已文档化差异（精选）

聊天 Guild/Alliance/System 色全白（`UIChatBar.cpp:2356-2377`，已裁决）；BossHP tag_color/tag_bg 解析后渲染丢弃（`UIBossHP.cpp:82-109` vs `:111-150`）；HP/MP 闪铃 5 帧 vs Angel 2 帧（已文档化）；被动 buff 资源覆盖缺失（`UIBuffList.cpp` 零命中 vs MinMS `set_fill_ratio`）；怪物手册 ContextMenu/LeftTabInfo/真卡面（既有审计已登记）；DAMAGE_MONSTER 飘字用 head 位置兼存在判断（`Combat.cpp:422/424/427` vs MinMS `2165/2170`）；登录 10s 超时缺失（`UILoginWait.cpp:129-133`）；jobrank 不绘、stub 死文案、空按键分支、IME 待核等。

## 4. 明确**不是**偏差（防重复立项 / 防误报）

- 怪物攻击帧、死亡多段、hit 回退、技能槽位映射——已补齐（`Mob.cpp:93-151,244-251`）；前轮「攻击帧缺失」担忧证伪。
- MobDropData「stub loader」是误判（handler 填充，链路完整）；怪物手册 HP/MP `???` 是原生正确空态（MinMS 同样恒定）。
- 掉落四档金币图标/光环、NPC 对话协议、BossHP 多 oid、伤害行 12B/4B 双模、PK 包掩码——对齐。
- 2026-11-22 已修三组 P0（快捷栏拖拽/药水键/瞬移二段跳空壳）不再列为新缺口。
- 富文本展开、设置窗、装备 tooltip 全字段、宠物装备三页、KeyConfig v83 映射——当前源码已重写对齐/超越。
- 宠物命令 opcode169、说话/自动药/排除列表——Angel 裁决「原生无 c2s」或已对齐，非漏移植。
- 活动 HUD / 小游戏 / 商城壳族空按键——既有裁决 NO-FIX（需服务端活动协议），按门禁不发明能力。

## 5. 待用户拍板的决策点（阶段 1 → 阶段 2 Plan 前）

- **D1 范围**：本轮只修 P1×11，还是 P1+P2 一并？推荐 **P1 全修 + P2 中「纯前端断链/特效/坐标」批次**（skill P1-1/2/3、社交 P1-5~8、商城 P1-10、地图 P1-11、HUD P1-4、怪物 2×P2），因其修复成本低、体验最硬；P1-2/P1-9 需先确认 IMG 资源。
- **D2 对照源**：位移/特效/物理直接以 MinMS 现成实现移植（同源、注释明写 Angel 依据）+ Angel 既有 evidence 交叉校验，重跑 IDB/原端截图成本高且结论不变。
- **D3 资源门禁**：P1-2 技能特效族、P1-9 NPC 任务标记、地图出生特效需先经 orange-wz MCP 核对本端 Skill.nx / UIWindow.img / SpecialEff.img 是否含节点；缺失则属适配限制，只预留播放点不造资源。
- **D4 真机验收**：EXP/HP%、display、瞬移落点、飞行怪、火花坐标、旁观 VFX 等需用户启动 s9+MySQL 真机复核；编译通过 ≠ 功能对。截图必须先确认来源端（参考端截图不得当本端验收）。
- **D5 有意例外登记**：每日签到（192 资源+私有协议）、P1-9 若立项涉及资源同步，是否接受「Angel v83 权威」口径的例外。
- **D6 提交**：产物落盘 + 用户确认后按功能原子 commit（当前工作树 209 文件未提交，需先明确本轮 diff 边界，避免混入）。

## 6. 证据索引

- 各域完整报告：本轮 8 个只读审计（login/hud/corewin/skill/mob/map/social/misc）回传结论（本 README 为汇总，逐条证据 file:line 已列）。
- 既有权威台账：`feature-gap-inventory-2026-11/README.md`（~200 项，§9.5 协议覆写）、`beidou-v17-parity/notes/r14-native-feature-catalog.md`（112 节点）、`native-baseline-audit-2026-11-22/README.md`（3 组 P0 已修）。
- 对照源：`E:/project/MinMS/src/client`（同源 C++）、`E:/MXD/反编译IDA/v83/'Angel.i64`（Angel 083 带符号 + 归档 evidence）、095/192 客户端（资源交叉）、`E:/project/BeiDou-Server_s9`（协议权威）。
- 本轮状态：**只读审计，未改源码/未编译/未真机/未提交**。
