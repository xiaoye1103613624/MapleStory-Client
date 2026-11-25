# 立项 I1 · finalattack 动作池原生复原实现（2026-11-25）

> 用户「需要」授权立项 I1 实现。四处联动一次成型，完整 build EXITCODE=0 / 0 errors。真机未验。

## 0. 证据链（充分）
- live WZ 实证：Skill/100.img 的 skill/1001004(Power Strike) 挂 finalAttack 节点，含 1100002/1100003/1200002/1200003/1300002/1300003 及武器类型(30/40/31/41/32/42/43/44)；Skill/110.img 的 skill/1100002(终极剑) 有 skillType [integer 3]。用 tools/_wz.py 直读 live img 取证。
- MinMS 蓝本：SkillData.cpp:422-455(finalAttack/skillType 解析)/:601(is_final_attack)、SkillAction.cpp:62(FinalAttackAction)、Char.cpp:1432(attack_final)/CharLook.cpp:598-625(OF 动作池)、Combat.cpp:201/256/1157-1232(pending_final_attacks + maybe_queue_final_attack)。
- 三端 IDA：Angel get_random_melee_attack_action skillType3 表；skillType3 = swingOF/stabOF 池。
- s9 权威：AbstractDealDamageHandler 把 FA 技能列进 PASSIVE_SKILL_ID_SET(:1491-1504，仅反作弊跳过)，FA 伤害由客户端发攻击包、服务端按 skill=fa_id 结算——客户端必须真正发包，不只是播动作。

## 1. 四处联动实现（逐条 file:line）
| 步 | 文件 | 改动 |
|---|---|---|
| 1 | Data/SkillData.h | 增 FinalAttackLink{skill_id,weapon_types} + get_final_attack_links() + is_final_attack() + skill_type/final_attack_links 成员；补 <vector> |
| 1 | Data/SkillData.cpp | ctor 解析 WZ finalAttack/<fa_id>/<i>=武器类型 + skillType；skillType3+hit+damage 合并 ATTACK 位；is_final_attack()/get_final_attack_links() 实现；is_attack() 保守保持 !passive && ATTACK（当前端无 MinMS 的 crash/seal/summon 排除，放开 passive 会回归） |
| 2 | Gameplay/Combat/SkillAction.h/.cpp | 增 FinalAttackAction::apply -> Char::attack_final() |
| 2 | Gameplay/Combat/Skill.cpp | ctor 分支：data.is_final_attack() -> FinalAttackAction + overregular |
| 3 | Character/Char.h/.cpp | Char::attack_final()（MinMS Char.cpp:1432 同源） |
| 3 | Character/Look/CharLook.h/.cpp | CharLook::attack_final()：按武器 attack 类随机取 OF 池(SWINGOF/STABOF/SWINGPF/STABTF/SWINGTF/SHOOTF)，PRONE->PRONESTAB，无池回退 getattackstance；补 <array> |
| 4 | Gameplay/Combat/SpecialMove.h / Skill.h/.cpp | 增 is_final_attack() 虚接口（FA 被动 is_attack()=false，需单独识别） |
| 4 | Gameplay/Combat/Combat.h/.cpp | pending_final_attacks TimedQueue + ctor lambda(调 cast_final_attack) + update() 泵 + maybe_queue_final_attack(命中后按链接/武器/prop 概率入队，delay=attackdelay 下限 280ms) + cast_final_attack(被动走独立攻击路径：prepare_attack->动作->apply_stats->命中判定->AttackPacket) |

## 2. 关键设计决策（防回归）
- 不动 is_attack() 语义：MinMS 放开 passive 的 ATTACK 判定是配套 crash/seal/summon 排除逻辑，当前端没有那套，贸然放开会让这些被动误判成攻击。FA 用独立 is_final_attack() + cast_final_attack() 走攻击路径。
- FA 真发包：s9 不主动触发 FA，只按收到的攻击包结算。cast_final_attack 发 AttackPacket(skill=fa_id)，与原生一致；队列点还检查 get_skilllevel(fa_id)>0（玩家仍持有该被动才施放）。
- delay 下限 280ms：MinMS get_attack_anim_duration() 本端无等价，用 get_attackdelay(0)+280ms 下限兜底，保证主攻击动作播完再触发。

## 3. 验证
- 6 个改动 TU 经 vcxproj 等价参数语法编译 0 错误（tools/_vs_syntax_check.bat 可复现）。
- 完整 build_smoke.bat：EXITCODE=0 / 0 errors。
- 真机未验：FA 触发时机/动作观感/伤害需在 s9+MySQL 环境点验；截图未采信（来源端未确认）。
