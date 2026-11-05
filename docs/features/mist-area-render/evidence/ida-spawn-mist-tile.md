# IDA：SPAWN_MIST → tile 平铺

IDB：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64`（2026-11-05）

## 分发

| 地址 | 角色 |
| --- | --- |
| `sub_531325` | 场包分发；273–274 → `sub_431A3E` |
| `sub_431A3E` | 273→`sub_431A63`，274→`sub_43234D` |
| `sub_431A63` | 读 oid/type/owner/skill/level/delay/box；按 skill 分支 |

## 包字段（与 s9 一致）

`int oid, int type, int owner, int skill, byte level, short delay, int x1,y1,x2,y2, int0`

## 布局函数

| 函数 | 用途 |
| --- | --- |
| `sub_432776` | 通用/毒雾：宽高中心收缩带；随机变体；x 步进 `35+(rng%20)`；y 步进 `spriteH*0.8` |
| `sub_432A1E` | 烟幕：只取子键 `"0"`，落点 `((x1+x2)/2, y2)` |
| `sub_43E357(..., count=20)` | MobSkill 130 特例（落脚点吸附）；本版用通用散射近似 |

## 资源串

- `0xAFE1F4`：`Skill/MobSkill.img/%03d/level/%d/tile`（由 `sub_7626BD` 读入 MobSkill 等级表）
- 玩家雾：`Skill/<job>.img/skill/<id>/tile`（IMG 实测 2111003 / 4221006 等）
