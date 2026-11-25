# 数字层技能乘区原生复原（2026-11-25）

> 用户"继续往下排查原生差异"授权。核心翻案：s9 是伤害权威（用客户端发来的 damage 打怪），客户端缺乘区=伤害缺失，此前"怕双算不修"是误判。完整 worktree build RC=0/0错误/exe 生成(5,697,536B)。真机未验。

## 0. 决定性证据（推翻"怕双算"）
- s9 `AbstractDealDamageHandler:1055` `damage = readDamageLine(p)` 从包读；`:320-332` `map.damageMonster(attacker, monster, damage)` 用客户端值打怪；`calcDmgMax:834` 的公式只是防作弊上限，不重算。
- 结论：客户端才是伤害权威。客户端不算的乘区永久缺失，不是"服务端会补"。

## 1. 四组实现（逐条 file:line）
### A. 魔法 pre-BB 公式
- CharStats.h/cpp：新增 `total_magic_attack()`（=装备MAD+INT，对齐 s9 localmagic=INT+装备MAD）、`get_magic_maxdamage(spell_attack)`、`get_magic_mindamage(spell_attack,mastery)`（MAD²/1000+MAD*mastery*0.9 的 pre-BB 二次式）。
- Skill.cpp 魔法分支：旧的线性 `(INT*4+LUK)/100*MAD` 且 min=max，改为 `skill_mastery=max(get_mastery(),0.1+stats.mastery*0.05)` + mindamage/maxdamage 用 pre-BB 二次式，区分下限/上限。
- 证据：Angel `CalcDamage::MDamage@0x791617` 反编译坐实 MAD 二次项（0.058/3.3/0.5 常量）+ mastery 增量；MinMS CharStats.cpp:797-824。

### B. 被动增幅族
- Skill.cpp apply_stats 增 `passive_upgrade` lambda + 四分支：PIRATE_DOUBLE_SHOT→BURST_FIRE、CHARGE→ADVANCED_CHARGE、HOMING_BEACON_SKILL→BULLSEYE（ratio 用被动 damage%）。
- BULLSEYE 关键：`report_upgrade` 时 `attack.skill=BULLSEYE`（否则 s9 damage check 390% vs 148% 超 1.5x 判失败）。
- ELEMENTAL_BOOST 对 FLAME_THROWER/ICE_SPLITTER 加算 ratio（Angel PDamage 0x79006f）。
- SkillId.h 补 8 个权威常量（s9 constants/skills 核对）。

### C. ASSASSINATE 潜行秒数倍率
- SkillData.h/cpp：Stats 增 `time` 字段（WZ `time` 节点，作为倍率 cap）。
- Player.h/cpp：DARKSIGHT 施加时刻 `dark_sight_applied`（give_buff 记时/cancel_buff 清零，steady_clock，与服务端 reset 解耦）；`stealth_seconds()` 返回整秒。
- Skill.cpp：`factor = min(stealth_seconds, stats.time)/3+1`（整数除法），mindamage/maxdamage 同乘（Angel PDamage 0x79028F）。

### D. CHARGE 元素回退
- Skill.cpp：`charge_element_code`（8 属性蓄力折 火2/冰1/雷3/圣5）+ `element_letter`；CHARGE 且 WZ elemAttr 空时按当前 WK_CHARGE buff 补 attack.element，使 Mob 元素弱点判定生效。

## 2. 防回归决策
- 未改 is_attack() 语义（同上批）。
- 不动 SACRIFICE（上一批已修 ignoredef=1.0）。
- 不碰 Buff 结构（constexpr 构造广泛），改用 Player 单独时间戳。
- hunk 级暂存隔离并行工作流的 cooldown_totals(D2)/finish 注释 hunk。

## 3. 验证
- 改动 TU 语法编译 0 错误（tools/_vs_syntax_check.bat）。
- 独立 worktree + CL=/FS + 重定向输出，拷贝本轮 9 文件后增量重建：RC=0、0 error、2213 行编译、exe 5,697,536B 时间戳新鲜，确认 Skill.cpp/CharStats.cpp/Player.cpp 被重新编译。
- **真机未验**：魔法伤害数值/被动增幅/BULLSEYE 报技能/ASSASSINATE 倍率/蓄力元素弱点需 s9+MySQL 点验；截图未采信。
