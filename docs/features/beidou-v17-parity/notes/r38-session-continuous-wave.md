# r38 — 连续不停歇会话汇总（r28→本切片）

用户指令：一直往下做、不要停。

## 已落地（编译均 `EXITCODE=0`，OutDir=`E:\MXD\BeiDou-Client-xy\MapleStory.exe`）

### 业务闭环
- Family：ADD/ACCEPT/JOIN/Chart/Precept
- SkillMacro：入站存储、拖技能、`SKILL_MACRO` 保存
- EnergyBar + SHOW_COMBO
- Hammer/Vega：`USE_CASH_ITEM` 拖装备
- Boss HP：`FIELD_EFFECT` mode 5

### W3 / 杂项 UI 壳（开窗或包驱动）
PartyRace · Wedding · Carnival · RPS · Ariant · NewYear · Minigame · TreasureBox · Admin · plant/raise · MapleTV · AdminClaim · classMatch · Megaphone · Report/Coupon/Claim/Incubator · Radio · KarmaScissors · EnchantSkill · ItemProtector · Teleport · SpeedQuiz · InitialQuiz

## 仍浅（壳/日志级）
多数活动窗字段未完整解析；Minigame↔PLAYER_INTERACTION；Messenger 立绘搁置；REA 坐标抛光。
