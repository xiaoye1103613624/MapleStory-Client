# Wave2 Dual Blade + Evan 交付记录（2026-11-12）

## 客户端资源

- Skill imgs：`430–434`、`2001/2200/221x` 已入 `BeiDou-Client-xy/Data/Skill/`（空壳 `2210/2212/2213/2215/2216/2218` 源真 242B）
- Skill.nx：重建部署 **493292167**；探针 `430.img/skill` 含 icon；`11121000` 保留
- String：s9 `String.wz/Skill.img.xml` 合入 381 条双刀/龙神名
- Character：Weapon `01342*.img`×111、Dragon×12；Character.nx 结构化重建部署 **384615702**（含 `Weapon/01342000` 等）
- katara：客户端 `Weapon::KATARA=134`

## s9

- Creator：`EvanCreator` / `DualBladeCreator`；`CreateCharHandler` job 3/4
- Job 枚举 430–434；i18n zh/en；`JobCommand` ≤2218
- Quest WZ 合入双刀/龙神节点；NPC/portal/quest 脚本落 `scripts-zh-CN`
- 配置迁移：`V1.11.8__enable_evan_dual_blade.sql`

## 客户端代码

- `UIRaceSelect` classic 五选；`UIEvanCreation` / `UIDualBladeCreation`
- `Job.cpp` 名/层级/`can_use`；locale job.* 键
