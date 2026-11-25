# 技能原生完整度 · 表现层复原（2026-11-25）

> 用户授权「确定就直接修复，深入抓证据链复原，保证功能原始完整性」。严格只读取证（MinMS 源码 + 既有三端 IDA 报告），实改代码并编译验证。真机未验。

## 0. 证据链（逐项）
- apply_actions 爬梯/绳重定向 + 姿态强制：MinMS Skill.cpp:491-525，Angel SendSkillUseRequest ladder→46/rope→47。当前端 Skill.cpp apply_actions 原仅 action->apply()。
- SACRIFICE(1311005) 强制无视防御：MinMS Skill.cpp:638-641，Angel CalcDamage::PDamage hard-skip PDD；WZ 无 ignoredef 键。
- 三蜗牛必中：MinMS Skill.cpp:660-665，Angel 走 DoActiveSkill_MagicAttack + WZ fixdamage。
- stepped_range=false：MinMS Skill.cpp:680-686，WZ 显式 lt/rb 保持轴对齐命中区，不叠加锥形。
- Attack 增 always_hit/stepped_range 字段（MinMS Attack.h 同名字段）。

## 1. 实改（逐条 file:line）
| 项 | 证据 | 改动 |
|---|---|---|
| SkillAction.is_appointed | SkillAction.h:32-35 | 基类新增虚方法（默认 true），RegularAction override false（无 WZ action 不重定向）。对齐 MinMS SkillAction.h:32 |
| apply_actions | Skill.cpp:224-250 | 爬梯/绳时对 appointed action 改播 ladder2/rope2；LUCKY_SEVEN(4121006)/TRIPLE_THROW 强制 RANGED；三蜗牛/施法技强制 MAGIC |
| SkillId 增补 | SkillId.h:158-165 | LUCKY_SEVEN=4121006、NINJA_STORM=4121008、BACK_SPIN_BLOW=5101002（原生技能常量，供后续接线） |
| SACRIFICE ignoredef | Skill.cpp:243-245 | skillid==SACRIFICE 时 attack.ignoredef=1.0f |
| 三蜗牛必中 | Skill.cpp:271 + Mob.cpp:1024 | 设 always_hit；calculate_damage 里 if(attack.always_hit) hitchance=1.0f |
| stepped_range | Attack.h:82 + Skill.cpp:288-294 | 增字段；WZ 显式 range 时置 false |
| Char.cpp 编译阻塞修复 | Char.cpp:435 | 并行工作流引入的 tomb_fall.is_valid()（Animation 无此方法）改为 get_frame_count()>0，否则整个工程无法编译；此行属另一路工作流，非本轮业务，但必须修以解锁构建 |

## 2. 未纳入本轮（如实声明，非遗漏）
skill-audit 盘点的其余项需要更深证据或更大范围，按"点到面一次成型但证据先行"原则不塞进本批：
- 数字层（魔法 mastery 完整公式、被动增幅族 BURST_FIRE/ADVANCED_CHARGE/BULLSEYE/ELEMENTAL_BOOST、ASSASSINATE 潜行倍率）：s9 服务端不二次计算、客户端须算，但 MinMS 的 pre-BB 魔法公式与当前端 (INT*4+LUK)/100*MAD 完全不同，属重写级变更，需单独取证数值链后立项；3 个取证子代理本轮均失败（工具异常），不以二手结论冒险改数值。
- finalattack 动作池：需新增 FinalAttackAction 类 + Combat 延迟触发队列，属独立功能批。
- NINJA_STORM 击退 / BACK_SPIN_BLOW 聚怪：需 Mob.push_back/gather 原语（当前端无），MinMS 语义取自技能描述+IDA @0x952453；需单独实现位移原语。
- get_bullet_rotate_period：需 SkillBullet 接口扩展，低优先。
- KeyDown 蓄力状态机：s9 无 opcode92，需独立设计（本地条 + attack.charge 发包）。
- 高版本技能（Demon/Evan/Dual/Mech）s9 范围外，不盲改。

## 3. 验证
- 五个改动 TU（Skill.cpp / Mob.cpp / FootholdTree.cpp / Player.cpp / Char.cpp）用 vcxproj 等价参数语法编译 0 错误（tools/_vs_syntax_check.bat 可复现）。
- 完整 build_smoke.bat 本轮被并行工作流的 PDB 锁（C1041，vc142.pdb 被占用，非代码错误）阻断：多次尝试均为纯 C1041、0 个非 PDB 错误；待并行构建结束需重跑确认链接。真机未验；截图未采信。
