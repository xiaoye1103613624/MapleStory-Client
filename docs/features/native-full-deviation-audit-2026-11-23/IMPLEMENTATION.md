# 原生基础功能偏差全量修复 · 实施记录（2026-11-23/24）

> 用户授权：「一次性全修复，修复完提交本次代码修改」（D1-D6 已拍板，PLAN.md 已确认）。
> 构建：`tools/build_smoke.bat` Release/x64 v142 → **EXITCODE=0 / GATE OK**（最终一次，含全部改动）。
> 真机：**未验收**（需用户启动 s9+MySQL 按 PLAN 清单逐项点验）；本记录只声明源码与构建事实。
> 对照：MinMS 同源 C++ + Angel 083 既有 IDB 证据 + s9 源码；协议 opcode 保持 s9 未动。

## 实施前基线
- HEAD `f47f08f82084679de726c0388b3ea20564927aa1`，分支 `feature/dev_0.0.1`。
- 工作树既有 **467 项未提交改动**（清单 `tools/_baseline_before_native_fix_20261123.txt`）；本轮 commit 隔离既有改动，只提交本轮文件（见 §提交）。

## Wave 0 · 前端接线断链（包类已存在、函数体空 → 补 dispatch）

| 项 | 文件:行（最终） | 改动 |
|---|---|---|
| 仓库金币存取 | `IO/UITypes/UIStorage.cpp:262-281`（dispatch `:277`） | `prompt_meso` 空体 → `UIEnterNumber` 输入 + `StorageMesoPacket(±n)`（s9 case7: 正=取/负=存）；新增 zh/en locale `storage.deposit_qty/withdraw_qty` |
| 仓库存物键(P) | `IO/UITypes/UIStorage.cpp:244-251`（dispatch `:249`） | 空块 → `StorageStorePacket(slot,itemid,qty)`（qty 取 `get_item_count`） |
| 交易金币 | `IO/UITypes/UITrade.cpp:322-338`（dispatch `:332`） | `prompt_meso` 空体 → UIEnterNumber + `TradeSetMesoPacket`（s9 0x10） |
| 交易聊天 | `IO/UITypes/UITrade.cpp:340-348`（dispatch `:345`） | `prompt_chat` 空体 → `UISoftKey` + `TradeChatPacket`（s9 CHAT=6） |
| 交易快捷键加物 | `IO/UITypes/UITrade.cpp:389-405`（dispatch `:398`） | I/A 键算完 inv_slot 后丢弃 → 补 `TradeAddItemPacket(iv,pos,qty,auto_slot)`；无选中给 status 提示 |
| 快递寄送 | `IO/UITypes/UIDelivery.cpp:131-170`（dispatch `:160`）、`:258-270`、`:311-323`（dispatch `:267/:320`） | `prompt_send` 空 `ask_meso` + send_key 两处死 lambda → `UISoftKey` 复合输入 `name|meso[,qty]` 接回 `DueySendPacket`；无选中包裹给 notice |
| 便签新建/回复 | `IO/UITypes/UIMemo.cpp:139-160`（dispatch `:153`） | N 键 + BtSave 空块 → 新增 `prompt_new_note()` 公共助手，`UISoftKey` 输入 `收件人|正文` + `NoteSendPacket`（s9 action0） |
| 商城礼物 | `IO/UITypes/UICashShop.cpp:930-943`（dispatch `:939`）、`:1139-1147`（`:1144`） | BtGift `UIYesNo` 确认回调空 → `CashShopGiftPacket(0,sn,name,"")`；G 快捷键同样接真发包 |
| 商城 BtHelp 错接 | `IO/UITypes/UICashShop.cpp:719-729` | 帮助钮错误跳 `EnterMtsPacket` → 改为本地帮助提示（不发明协议） |
| 商城 send_key 空分支 | `IO/UITypes/UICashShop.cpp:1125-1195` | B/S/T/P/K/I/X/C 空键 → 映射到已有按钮处理器（BuyAvatar/Search/Takeoff/SaveAvatar/DetailPackage/扩栏提示），不发明 opcode |

## Wave 1 · 数值守卫与发包布局

| 项 | 文件:行 | 改动 |
|---|---|---|
| 攻击包 display 恒 0 | `Gameplay/Combat/Attack.h:134`、`Net/Packets/AttackAndSkillPackets.h:85`（原恒 0）→ 修复：`Character/Look/Stance.cpp:141-204`（新增 `body_action`/`body_action_by_name` 含 PC 组合动作覆盖 alert2→41 等）、`Character/Look/Stance.h:49-51`、`Character/Look/CharLook.{h:64,cpp:639} get_action_name()`、`Gameplay/Combat/Combat.cpp:49-66 fill_attack_wire_action` + `:223` 调用 | MinMS Combat.cpp:174-190 移植；s9 PacketCreator.addAttackBody:2604 原样广播 display，旧恒 0 使他人看到默认姿态 |
| EXP/HP/MP 百分比溢出 | `IO/UITypes/UIStatusBar.cpp:2096-2135` | `getexppercent` 满级早退（`LEVELCAP=256`）+ need≤0 守卫 + clamp 0..1；`gethppercent/getmppercent` 加 max≤0 除零守卫 + clamp（s9 999 级场景） |

## Wave 2 · 位移与坐标系

| 项 | 文件:行 | 改动 |
|---|---|---|
| 受击火花坐标 bug | `Gameplay/MapleMap/Mob.cpp:925-934` | `Point(phobj.get_x(), phobj.get_y()-15)`（世界坐标当相对偏移，火花甩到离怪几百像素）→ `Point(0,-15)` relative；D3 确认 CoolHit 节点在位 |
| 飞行怪被钉地 | `Gameplay/Physics/Physics.cpp:42-63`、`Gameplay/Physics/FootholdTree.{h:49,cpp:123-155}` | 新增 `FootholdTree::limit_horizontal`（水平钳制无垂直吸附，MinMS FootholdTree.cpp:235）；FLYING 豁免 `update_fh` 并改用 `limit_horizontal` |
| 瞬移光圈 | `Character/Player.cpp:836-854` | 成功位移后播 `BasicEff.img/Teleport`（MinMS Combat.cpp:1783）；D3 orange-wz 实测节点在位；受 `SysOptSkillEffects` 开关约束 |
| 旁观怪技能 VFX | `Gameplay/MapleMap/Npc.cpp` 无关；实际 `Gameplay/MapleMap/Mob.cpp:779-788`（send_movement skill 分支） | 非受控怪施法只切 stance 不播 effect/hit/tile → 补 `play_mob_skill_effect(skillid,skilllevel)`（s9 不下 VFX 包，纯客户端驱动） |
| 相机震屏漂移 | `Gameplay/Camera.{h:44-46,66-69,cpp:35-66,90-116,142-158}`、`Gameplay/Stage.cpp:218` | 抖动值旧实现写回 x/y（随机游走）→ 独立 `tremble_ox/oy`（只在 position() 加、不回写）+ 每帧衰减 + `clear_tremble()`；换图调用清屏 |

**说明**：审计 P1-1「瞬移无 foothold 探测」经复核为**过期结论**——当前 `Character/Player.cpp:734-838 apply_teleport` 已含 120ms 节流 + 1/4 递进 foothold 落点探测 + 边界夹取（前序会话已修），未重复移植。

## Wave 3 · 特效与标识

| 项 | 文件:行 | 改动 |
|---|---|---|
| 技能特效族 8 类 | — | **未改（资源缺源预留）**：D3 orange-wz 实测 live `Skill/*.img` 只有 `effect`/`hit`，无 prepare/screen/special/tile/receiver/keydown；按「不造资源」原则不加播放点 |
| 快捷格技能 CD 遮罩 | `Character/Player.h:121,cpp:969 get_cooldown_remaining`、`IO/UITypes/UIStatusBar.cpp:3365-3379` | 新增剩余 CD 读取；classic QS 绘制时绑定为 SKILL 且在 CD → 半透明黑 `ColorBox(30,30,BLACK,0.6f)` 遮罩（本端 SkillData 不解析 WZ cooltime，无法给精确比例，用二元反馈不发明数值，MinMS UIStatusBar.cpp:440-459） |
| NPC 头顶任务标记 | `Gameplay/MapleMap/Npc.h:78-80,cpp:228-269` | 新增任务标记 state（0无/1可接/2进行中）+ 全局 1s 可接集缓存（`QuestData::list_available`）+ 每 NPC 500ms 重算 + `QuestIcon/<kind>/0` 动画绘制；D3 确认节点在位；"可交"态需 Check.img 进度解析，按证据不足不臆造 |
| 冲锋/后坐力/技能门禁/蓄力状态机 | — | **未改（预留）**：RECOIL_SHOT 在 SkillId.h 无定义、蓄力状态机需 Skill keydown 资源族（D3 缺源），按「不发明协议/数值」不改；display 字节已修（见 Wave1） |

## Wave 4 · HUD/商城/任务 P2 + 遗留隐患

| 项 | 文件:行 | 改动 |
|---|---|---|
| 快捷栏拖出清除绑定 | `IO/UITypes/UIStatusBar.h:152-154`（QsIconType 增 `source_key=0`、`drop_on_stage()` 声明）、`UIStatusBar.cpp:2833-2847`（实现 + setup 填 source_key） | MinMS unbind_key：`drop_on_stage` 清源键绑定 + `ChangeKeyMapPacket(135)` 持久化 + 重绘（重绘由 UIStateGame 松手收口） |
| 任务窗 send_key 空体 | `IO/UITypes/UIQuestLog.cpp:464-473` | 空体 → ESC 显式 `deactivate()`（原靠 UIStateGame 全局兜底，本窗自身快捷键无效） |
| 遗留编译错误（既有隐患） | `IO/UITypes/UIWorldMap.{h:93,cpp:1362}`、`IO/UITypes/UIQuestLog.cpp:1039` | 基线中 `UIQuestLog.cpp:1032` 调用私有 `UIWorldMap::jump_to_map_region` 导致 **构建失败**（既有问题，非本轮引入）：新增公开 `show_map_region()` 包装，UIQuestLog 改调之，解编译阻塞 |

## 验证
- **构建**：`tools/build_smoke.bat` → `GATE OK` / `EXITCODE=0`（最终一次含全部改动，Release/x64，exe 输出 `E:\MXD\BeiDou-Client-xy\`）。
- **静态门禁**：新 dispatch 点逐条 grep 确认（见各项 dispatch 行号）；装备槽不变式 `docs/features/equip-slot-expansion-2026-11/verify_no_conflict.py` 未涉及装备枚举/ItemParser，无需重跑。
- **真机验收（待用户，D4）**：仓库/交易/快递/便签/商城礼物逐钮点击；EXP/HP% 满级不溢出；他人攻击姿态非默认；飞行怪悬浮、火花贴怪、旁观 Boss VFX、相机无漂移；CD 遮罩、快捷格拖出清格、NPC 头顶标记。截图须先确认来源端。

## 有意不修（证据不足 / 已裁决 / 资源缺源）
- 技能 8 类特效族、蓄力(KeyDown)状态机、RECOIL_SHOT、武装 mount action：D3 已证 live WZ 无对应资源 / SkillId 无定义，按「不发明能力」不造。
- 冲锋 MinMS 地形步进 + magnet_pull：当前 `apply_rush` 只停最远怪处（P2），逻辑改动面大且需真机手感定夺，本轮保留。
- 元素克制/魔法 mastery/ignoredef：s9 服务端权威结算伤害，客户端仅显示层，避免联机数值不一致风险，本轮不动。
- 每日签到（192 资源 + 私有 0x3721/0x3722）：D5 用户认可为有意例外，不动。
- 活动 HUD/小游戏空键、宠物说话/自动药/排除列表：既有 NO-FIX / Angel 原生无 c2s，不动。

## 提交
- 采用隔离提交：仅 `git add` 本轮涉及源码 + 本文档 + 审计 README/PLAN + 必要 locale 键，不纳入基线 467 项既有改动。
- 因多个目标文件（UIStatusBar/UICashShop/Player 等）在基线中已有大量未提交改动，隔离方式为整文件 add（这些文件的既有改动随本轮一并提交，属工作树现状）；提交说明列明功能范围。
