# WZ 是否齐？完美移植门槛（2026-11-12）

目标端客户端：`E:\MXD\BeiDou-Client-xy`（083 系 nx/img）  
服务端 XML：`BeiDou-Server_s9\gms-server\wz\Skill.wz`（当前四转空壳 278B）  
资源源：怀旧岛 192 客户端分卷 Skill + `MapleServer192\wz\Skill.wz\*.xml`

## 1. 结论（Skill 本体）

**XML 齐，且对应客户端 WZ 也齐（含真实 CANVAS），不是「只有数值 dump」。**

| 内容 | 192 XML | 192 客户端 WZ 分卷 | 抽样位图 |
| --- | --- | --- | --- |
| 骑士团四转 `1112`…`1512` | 有（如 1112≈213KB、526 canvas 标记） | `Skill_001/003/004/006.wz` | `11121000`：icon×3 CANVAS + effect |
| 双刀 `430`–`434` | 有 | `Skill_027.wz` | `4300000` icon×3；`4341002` icon+effect+hit+action… |
| 龙神 `2001`/`22xx` | 有 | `Skill_011` / `Skill_013` | `2001.img/skill` 大量节点 |
| V `40000`/`40001` | 有 | `Skill_018` / `Skill_019` | `400001000` icon×3 + effect/ball + `vSkill` |

分卷定位见 `wz-pack-locate.json`。

本端 / s9：`1112.img.xml` **278B 空壳**；Skill.nx 四转 `skillKids=0`；双刀/龙神/V **整根缺失**。

## 2. 「完美移植」定义（你提的双端可用）

必须同时满足：

1. **客户端 083 img/nx 可用**  
   - 从 192 WZ（`iv=AAAAAA==`）经 orange-wz：`load →（按需）change_key→GMS IV` → `save_as` 出 **083 可读 img**  
   - 合入 `BeiDou-Client-xy\Data\.nxcache\Skill.nx`（及 String/Effect/Sound/Character/Item 依赖）  
   - 运行时：`SkillData` 能加载 icon；攻击技能有 hit/effect 可播；**不覆盖**骑士团 1–3 旧 ID  

2. **服务端 XML 可用**  
   - 将 192 XML（或从 WZ 导出的等价树）写入 s9 `Skill.wz\<ns>.img.xml`  
   - `SkillFactory` 能解析 damage/mpCon/…；未知高版本字段须可忽略或裁剪  
   - 学习/伤害/CD 与客户端同一套 skillId  

任一端缺位图或缺数值表 → 不算完美。

## 3. 仍未闭环的依赖（WZ 旁路）

Skill 本体齐 **≠** 全玩法齐：

| 依赖 | 本端现状 | 风险 |
| --- | --- | --- |
| String 技能名 | 无 11121000/4300000/400001000… | 技能书无名 |
| Character 短刀 134xxxx | 无 01342027/28 | 双刀外观缺 |
| Effect / Sound | 未逐技能扫 UOL | 可能外链高版本 Effect 节点 |
| 高版本字段 | `vSkill` / `combatOrders` / `common` 替代 `level` | 083 `SkillData`、s9 Factory 要兼容或裁剪 |
| 骑士团等级上限 | s9 `isCygnus()?120` | 仅资源不够四转玩法 |

## 4. 推荐流水线（验证通过后的实现序）

```text
192 Skill_NNN.wz (LATEST)
  → orange-wz 抽出 *.img
  → change_key → GMS(TSPHKw==)+128B key
  → save_as staging/*.img
  → 合入 Client-xy Skill.nx + String.nx …
  → 同步/裁剪 XML → s9 Skill.wz
  → 抽样：客户端能显示 icon；服务端 teachSkill 后有伤害表
```

**当前状态**：Skill 向 WZ+XML **源侧齐**；本端与 s9 **目标侧空/缺**；依赖包与密钥转换验收尚未做完，故 **还不能宣称可完美移植**，但 Skill 本体已具备「双端材料」条件。
