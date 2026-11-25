# 立项 I3 · KeyDown/蓄力状态机原生复原（2026-11-25）

> 用户「都做」授权 I3。功能闭环部分已实现（蓄力毫秒数进攻击包），KeyDownBar UI 因 live WZ 无资源显式预留。真机未验。

## 0. 证据链
- Angel v83 IDA：`is_keydown_skill@0x4fb08f`（白名单：1121001/1221001/1321001 战士、2121001/2221001/2321001 法师 BIG_BANG、3121004/3221001 弓手、22121000/22151001 Evan）、`OnKeyDownSkillEnd@0x95bedf`（charge = get_update_time()-起始时刻，钳制 [30, cap]）、`sub_500971@0x500971`（cap：战士/法师/BIG_BANG=1000ms，弓手/Evan=2000ms）。
- s9：`AbstractDealDamageHandler:769-773` 对 9 个技能 readInt() 读 charge；但仅解析字段、不用它做伤害结算 → charge 是客户端蓄力时长上报。
- 当前端：`AttackAndSkillPackets.h:79-80` 已有 `if(has_charge_field(skill)) write_int(attack.charge)`，但全工程无一处给 charge 赋值（注释 :77 自承）→ 缺的就是输入状态机。

## 1. 实现（逐条 file:line）
| 文件 | 改动 |
|---|---|
| Gameplay/Combat/Attack.h | Attack 增 `int32_t charge`；AttackResult(Attack) 拷贝构造透传 charge |
| Gameplay/Combat/Combat.h | 增 `begin_keydown`/`end_keydown` 接口；增 keydown 状态（skill/start/pending_charge）+ `<chrono>` |
| Gameplay/Combat/Combat.cpp | `is_keydown_cast_skill`（白名单）/ `keydown_cap`（1000/2000）；`begin_keydown` 按下沿记时、`end_keydown` 松手算 charge 钳 [30,cap] 后 `use_move`；`apply_move` 把 pending_charge 写进 Attack.charge |
| Gameplay/Stage.cpp | SKILL 分支：down 先试 `begin_keydown`（true=已接管不普通施放）；up 调 `end_keydown` |

## 2. 关键决策
- **不改 opcode**：s9 无 083 的 92（92=CANCEL_BUFF），但 attack 包已有 charge 字段通道；走现成通道，不发明协议。
- **非蓄力技能零影响**：begin_keydown 对非白名单返回 false，走原 down-only 施放；瞬移/二段跳不会连发。
- **真机未验**：蓄力手感/时长观感需 s9+MySQL 点验。

## 3. 显式预留（非遗漏，不发明能力）
- **KeyDownBar UI**：三端原生有（Angel DrawKeyDownBar@0x95c3e3），但 live WZ UI 根未找到对应节点，当前端无该 UI 契约；不造不存在的资源，预留后续单项（需先 orange-wz 定位节点/或用通用进度条组件）。
- **蓄力特效时序**：当前端 extra_nodes 把 prepare/keydown/keydownend 在施放时一次性播；原生起手播 prepare、松手播 keydownend 的细分时序属增强，未在本次最小闭环内改。

## 4. 验证
- 改动 TU 经 vcxproj 等价参数语法编译 0 错误（tools/_vs_syntax_check.bat 可复现）。
- 完整 build_smoke.bat 本轮受并行工作流 PDB 锁（C1041，0 个非 PDB 代码错误）阻断，待并发结束重跑确认链接。
