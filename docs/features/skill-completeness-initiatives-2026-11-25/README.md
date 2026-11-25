# 技能原生完整度 · 剩余大项立项（2026-11-25）

> 用户指示「第二点需要立项」。本文件把三个多文件子系统正式立项，避免与已提交批次混淆。
> Hindsight 在线立项因 API key 未配置返回 401，改用本地产物文档立项（符合 xiaoye-maple-story-dev 产物保留门禁）。

## 已完成批次（销案）
- `b36a1d44`：apply_actions 爬梯/绳帧 + 飞侠姿态强制 + SACRIFICE 无视防御 + 三蜗牛必中 + stepped_range。
- `7287d8ee`：NINJA_STORM 击退 + BACK_SPIN_BLOW 聚怪（Mob.push_back/gather + AttackUser.position）。
- 完整 build EXITCODE=0 / 0 errors（PDB 锁解除后）。

## 立项 I1：finalattack 动作池（原生表现层）
- **现状**：当前端无。s9 服务端已把 finalattack 当被动权威结算（`AbstractDealDamageHandler:1495-1504` PASSIVE_SKILL_ID_SET），客户端只需补表现层动作。
- **证据**：MinMS `SkillData::is_final_attack()`=`passive && skill_type==3 && flags&ATTACK`（SkillData.cpp:601）；`FinalAttackAction::apply`→`Char::attack_final()`（SkillAction.cpp:62 / Char.cpp:1432）；Angel skillType3 swingOF/stabOF。
- **所需改动（4 处联动）**：
  1. `SkillData` 解析 WZ `skillType` 属性 + `is_final_attack()`；
  2. `SkillAction` 增 `FinalAttackAction`（`Char::attack_final()`）；
  3. `CharLook::attack_final()` 播 STABOF/SWINGOF 池（Stance 枚举已有 `STABOF/SWINGOF`）；
  4. `Combat` 增 `pending_final_attacks` TimedQueue + `maybe_queue_final_attack`（普攻命中后按概率入队）。
- **风险**：需确认 live WZ 的 skillType 节点在位（orange-wz 取证）；与 s9 结算不冲突（客户端表现层）。
- **状态**：立项，待 Plan → 实现。

## 立项 I2：get_bullet_rotate_period（投射物旋转）
- **现状**：当前端 `Bullet` 只用 animation.draw，无旋转周期概念。
- **证据**：MinMS `SkillBullet::get_rotate_period`（SingleBullet=ball.rotate_period / BySkillLevelBullet=按等级取），`Skill.cpp:776-779` 暴露接口。
- **所需改动**：SkillBullet 基类增虚方法（默认 0）+ Single/BySkillLevel override；Bullet 绘制消费 rotate_period（帧旋转）。
- **状态**：立项，低优先（观感项），待取证 live WZ ball 节点的 rotatePeriod 字段。

## 立项 I3：KeyDown / 蓄力状态机（原生能力缺口）
- **现状**：当前端 Net/ 无 charge 处理、无 opcode92（s9 的 92=CANCEL_BUFF）。
- **证据**：三端 IDA 证实原生齐（083 is_keydown_skill@0x4fb08f、OnKeyDownSkillEnd@0x95bedf、DrawKeyDownBar@0x95c3e3；095/192 同族）；s9 有 attack.charge 字段（`AbstractDealDamageHandler:770`）+ SkillEffectHandler。
- **所需改动**：本地 keydown 白名单/状态跟踪 + KeyDownBar UI + 松手发带 charge 攻击包（不改 opcode，走现有 attack 包）。
- **风险**：真机手感项，须用户点验；UI 条走 orange-wz/Photoshop 契约。
- **状态**：立项，独立设计后实现。

## 声明
以上三项均为多文件子系统，本轮**只立项不实现**（遵循 skill 门禁「先证据→Plan→实现」，不拿二手结论冒进）。三项均真机未验。
