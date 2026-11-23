# 原生基础功能偏差全量修复 · 一次性完整 Plan（2026-11-23）

> 用户已拍板：**D1 一起修（P1+P2 全量）/ D2 MinMS 移植+Angel 交叉 / D3 允许资源核对（已完成，见 README §1.1）/ D4 服务端已启动、由用户真机验收 / D5 认可每日签到例外 / D6 单独原子 commit**。
> 性质：本文件是 skill 阶段 2 的完整任务计划；**用户确认本 Plan 后**才进入实现。全程遵守门禁 5（二进制/IMG 用证据）、8（WZ 走 orange-wz）、18（原生还原逻辑+IMG），不发明协议、不造资源。

## 目标与已确认决策摘要
- 目标：一次成型修复 README 中 §1 的 P1×11 + §2 的 P2×约30（含怪物/地图/技能特效坐标批次），P3 仅处理「零成本顺手项」（过期注释等），显式预留项不动。
- 决策：位移/特效/物理以 MinMS 现成实现移植（同源、注释明写 Angel 依据）；协议保持 s9（91/93 等）不动，遵守 101/92/93 同号异义红线；每日签到（192 资源+私有协议）登记为有意例外，不动。
- D3 已证实（§1.1）：技能 8 类特效族中仅 `effect`/`hit` 在 live WZ 有节点，其余 6 类（prepare/screen/special/tile/receiver/keydown）**资源缺源 → 只预留播放点，不作为可闭环项**；NPC 任务标记节点在位（QuestIcon/BtMarkNpc）可做；CoolHit 在位（纯坐标 bug）；怪物出生特效资源缺源（保留预留）。

## 任务分解（有序，含依赖；闭环/预留标注）

### Wave 0 · 纯前端接线断链（P1-5~8、P1-10；无协议/资源风险，先做，最快可见）
1. 仓库金币存取 — `UIStorage.cpp:262-266 prompt_meso` 空体：接 `UIEnterNumber` 输入 + `StorageMesoPacket`（`StoragePackets.h:68-75`，字段序对齐 s9 `StorageProcessor.java:200-201`）dispatch，复用 `UIMerchant.cpp:276-285` 成熟写法。**闭环**
2. 玩家交易金币 — `UITrade.cpp:320-323 prompt_meso` 空体：接 UIEnterNumber + `TradeSetMesoPacket`（`TradePackets.h:85-93`，s9 SET_MESO=0x10）。**闭环**
3. 玩家交易聊天 — `UITrade.cpp:325-328 prompt_chat` 空体：接 SoftKey 文本输入 + `TradeChatPacket`（`TradePackets.h:75-83`，s9 CHAT=6）。**闭环**
4. 交易快捷键加物品 — `UITrade.cpp:353-380` 算出 inv_slot/auto_slot 后空块：按同文件双击路径 `:171` 的 `TradeAddItemPacket` 写法发包。**闭环**
5. 快递寄送 — `UIDelivery.cpp:238-244/281-285` 空 if 分支：把已写好的 `DueySendPacket` lambda 调用接回分支（复用 `:144-147 ask_meso` 补齐）。**闭环**
6. 便签新建/回复 — `UIMemo.cpp:115-124/164-176` 空块：接 `NoteSendPacket`（`NotePackets.h:31-41`，s9 `NoteActionHandler:46-49`）。**闭环**
7. 商城 BtGift — `UICashShop.cpp:930-933` 空回调：在 `if(yes)` 内 `CashShopGiftPacket(sn, pre).dispatch()`（复用 `:713-714` SoftKey 路径包类）。**闭环**

### Wave 1 · 数值守卫与发包布局（P1-3、P1-4；低风险，与 Wave0 并行可）
8. display 字节 — `Attack.h:134`：新增 `fill_attack_wire_action`（MinMS `Combat.cpp:174-190`）在 attack 构造处赋值 `Stance::body_action_by_name`；保持 `AttackAndSkillPackets.h:85` 写出。**闭环**（需真机他人姿态确认）
9. EXP/HP/MP 守卫 — `UIStatusBar.cpp:2096-2120`：满级早退 `if(level>=LEVELCAP) return 0.0f` + clamp；`gethppercent/getmppercent` 加 `max<=0→0` 守卫（MinMS `UIStatusBar.cpp:1061-1097`）。**闭环**

### Wave 2 · 位移与坐标系（P1-1、P1-11、地图/怪物 P2；MinMS 移植，真机重点）
10. 瞬移吸附 — `Player.cpp:724-758 apply_teleport`：移植 MinMS `Combat.cpp:1759-1790`（fhid==0 拒绝 + `find_teleport_target` + `fht.attach_floor` 落点吸附 + 15-tick 冷却 + `BasicEff/Teleport` 光圈特效）；当前端需把 MinMS Combat 侧逻辑落到 Player/Combat 合适位置，保持 s9 `USE_SKILL(91)` 不动。**闭环（落点）+ 特效部分（Teleport 节点需核，缺则预留）**
11. 飞行怪物理 — `Physics.cpp:42-72`：`if(type!=FLYING) update_fh` + FLYING 分支改 `limit_horizontal`（从 MinMS `FootholdTree.cpp:235-271` 移植该方法到本端 `FootholdTree.cpp/.h`）。**闭环**
12. 受击火花坐标 — `Mob.cpp:925`：`Point(phobj.get_x(), phobj.get_y()-15)` → `Point<int16_t>(0, MOB_HIT_EFFECT_Y_OFFSET)`（MinMS `Mob.cpp:7331-7347`；CoolHit 已证在位）。**闭环**
13. 相机震屏 — `Camera.cpp:35-94`：移植 MinMS `Camera.cpp:103-123,125-203` 的独立 `tremble_ox/oy` + 0.85/0.92 衰减 + `clear_tremble()`，改 `position()` 只加 offset 不回写 x/y；换图/技能清屏调用 clear。**闭环**
14. 非受控怪 MobSkill VFX — `Mob.cpp:558-573/752-797`：在 `send_movement` 处理 skillbyte 分支补 `play_mob_skill_effect` 调用（受控支已有，旁观支缺）。**闭环**（VFX 纯客户端驱动，s9 无包）

### Wave 3 · 特效与标识（P1-2 部分、P1-9、P2 特效；资源门禁后实施）
15. 技能特效族 — `Skill.h/.cpp`：移植 MinMS 中 `effect`/`hit` 对应播放逻辑（当前端已有 useeffect/hiteffect，重点补 area/screen 中与 `effect` 节点相关的部分）；`prepare/screen/special/tile/receiver/keydown` **资源缺源 → 加注释预留播放点，不改 WZ、不造资源**。**部分闭环 + 显式预留**
16. 快捷格 CD 遮罩 — `UIStatusBar.cpp:3273-3284`：按 MinMS `UIStatusBar.cpp:440-459` 算 cooldown_remaining/total 画 ColorBox(30,h,BLACK,0.6f) 遮罩（需 skill cooldown 数据源，若本端无则记录预留）。**闭环/预留视数据源**
17. NPC 头顶任务标记 — `UIMiscShells.cpp:6755-6819` + `Gameplay/MapleMap`：移植 MinMS `QuestNpcMarkers.cpp/.h` 的 4 态 resolve + build_index（QuestIcon 节点在位），挂到地图层按 NPC 状态渲染。**闭环**（需实施期确认 Quest.img Check 节点，缺则预留）
18. 其他技能 P2 — 瞬移光圈（并入 #10）、冲锋地形步进+magnet_pull（`Combat.cpp:376-384` ← MinMS `2065-2122`）、RECOIL_SHOT 位移（`Combat.cpp:362-374` ← MinMS `2047-2063`，需补 SkillId.h 定义）、蓄力状态机（`AttackAndSkillPackets.h:305-308` 注释自认无，MinMS `Skill.cpp:214-217,411-421`，**资源缺源配合 → 预留**）、技能专属门禁（`Combat.cpp:127-159` ← MinMS `403-531`）、元素克制与魔法 mastery/ignoredef（**s9 权威结算，仅客户端显示层，低优先，与用户确认后再定**）。**按依赖分批闭环/预留**

### Wave 4 · HUD/商城/任务 P2 + 顺手 P3
19. 快捷栏 unbind — `UIStatusBar.h:147,150 drop_on_stage/drop_on_bindings` 空实现：移植 MinMS `UIStatusBar.cpp:915-935 unbind_key`，松手落空白/拖出清除绑定。**闭环**
20. 商城快捷键 8 处空分支 — `UICashShop.cpp:1111-1163`：按 `button_pressed`（已接真发包 `:682-887`）对应补齐 send_key 调用；BtHelp `:719-724` 错接 EnterMtsPacket → 改为正确的帮助/问号行为（无原生依据则注释预留，不误接拍卖）。**部分闭环 + 预留**
21. 任务日志四钮 — `UIQuestLog.cpp:1080-1089`：MARKNPC 依赖 #17 的世界地图标记入口，随 #17 一起接；OK/NO/ALERT 底带不显示（activate_buttons false）维持预留。send_key 空体 `:464-466` 补 ESC 关窗。**部分闭环**
22. 顺手 P3 — `WeatherGround.h:8-16` 过期注释（环境音已实现）、`PacketSwitch.cpp:447` 0x3742 陈旧注释（注册正确）、怪物手册 ContextMenu/LeftTabInfo（既有审计已登记，资源在位但属增强，本轮不动，仅记录）。**文档清理**

## 涉及文件/模块预估（按 Wave）
- Wave0：`IO/UITypes/{UIStorage,UITrade,UIDelivery,UIMemo,UICashShop}.cpp`，复用 `Net/Packets/{StoragePackets,TradePackets,DueyPackets,NotePackets,CashShopPackets}.h` + `UIEnterNumber`/`UISoftKey`。
- Wave1：`Gameplay/Combat/Attack.h` + attack 构造点（`Combat.cpp`/`Skill.cpp`）、`IO/UITypes/UIStatusBar.cpp`、`Character/ExpTable.cpp`。
- Wave2：`Character/Player.{h,cpp}`、`Gameplay/Combat/Combat.cpp`、`Gameplay/Physics/{Physics,FootholdTree}.{h,cpp}`、`Gameplay/MapleMap/Mob.cpp`、`Gameplay/Camera.{h,cpp}`。
- Wave3：`Gameplay/Combat/{Skill.h,Skill.cpp,SkillUseEffect.h}`、`IO/UITypes/UIStatusBar.cpp`、`IO/UITypes/UIMiscShells.cpp`、`Gameplay/MapleMap/` 新增 QuestNpcMarkers（如移植）。
- Wave4：`IO/UITypes/{UIStatusBar.h,UICashShop.cpp,UIQuestLog.cpp,WeatherGround.h}`、`Net/PacketSwitch.cpp`。
- 构建工程：`MapleStory.vcxproj`（若新增 QuestNpcMarkers 文件）；所有改动加入 D6 单一原子 commit。

## REA / IDA / WZ MCP 步骤
- WZ：D3 已用 orange-wz 核对（§1.1）；实施期若需精确节点（Quest/Check.img、Skill/210 的 effect 子结构），继续用 `tools/_d3_*.py` 同规格脚本，改 live 前关客户端、另存 staging、原子替换（AGENTS.md §3）。
- IDA：本轮不重跑（D2 既定）；位移/特效以 MinMS 源码注释中的 Angel 依据为准，既有 IDB evidence 交叉校验。
- 插件/服务端：不改服务端、不改 opcode；`CashShopGiftPacket`/`DueySendPacket` 等包类字段序已与 s9 对齐，仅补 dispatch。

## 验证门禁
- 构建：`tools/build_smoke.bat`（VS2019 BuildTools v142 Release/x64）→ EXITCODE=0 / GATE OK；构建前结束 MapleStory.exe。
- 单测/静态：跑 `docs/features/equip-slot-expansion-2026-11/verify_no_conflict.py`（装备槽不变量回归）；全仓 grep 确认 Wave0 各包类从「零 dispatch」变「≥1 dispatch」；`Attack.display` 赋值点存在。
- 真机（D4，用户启动 s9+MySQL 后执行，编译通过 ≠ 功能对）：
  1. 仓库金币存取/交易金币/交易聊天/快递寄送/便签新建/商城礼物——逐钮点击验证服务端回包与余额/物品变化；
  2. 满级/低 HP 场景 EXP 条与 HP/MP 条不溢出；
  3. 法师瞬移落点（含上台阶）、无穿地板；二段跳（已修）回归；
  4. 飞行 template 怪悬浮、受击火花贴怪、多人同屏 Boss 技能旁观 VFX、相机震屏无漂移；
  5. 快捷格 CD 遮罩、技能特效（本端 WZ 有的部分）、NPC 头顶任务标记；
  6. 他人客户端观察攻击姿态 display 非固定 0。
- 截图验收：**必须先确认来源端**（参考端截图不得当本端验收证据，USER.md critical）；本端截图由用户在真机环境下提供。

## 产物与文档交付
- 本 README（审计）+ 本 PLAN（计划）；实施后补 `IMPLEMENTATION.md`（逐项 file:line 改动 + 验证结论 + 预留清单）。
- evidence：D3 探针输出 `tools/_d3_native_fix_resource_probe.out.txt` / `_d3_skill_effect_deep.out.txt`。
- 原子 commit（D6）：单一功能一次提交，说明写清；当前工作树 209 文件既有改动**不纳入**本 commit（只 add 本轮改动文件 + 产物），避免混入。

## 风险与回滚
- 位移/物理/相机改动影响全客户端手感，真机重点回归；每处改动保留 MinMS 原注释作依据，便于 revert。
- Wave0 补 dispatch 后若 s9 字段序与包类不符（已逐字节核对，风险低），真机报错即按 s9 源码微调。
- 特效族资源缺源项按 D3 结论预留，不造资源、不改 WZ，避免闪退（AGENTS.md §3.1 密钥转换坑）。
- 回滚：按 IMPLEMENTATION.md 文件列表逐项 revert；本轮未提交前与既有 209 文件改动混存，commit 前用 `git add <明确文件列表>` 隔离。

## 明确不做（本 Plan 边界）
- 活动 HUD/小游戏空按键（NO-FIX，需服务端活动协议）；雇佣商人进店（独立二期）；宠物说话/自动药/排除列表（Angel 原生无 c2s）；Mob 出生特效/技能 6 类特效族（资源缺源，只预留）；元素克制/mastery 若用户后续不追可留（s9 权威，仅显示层）。
