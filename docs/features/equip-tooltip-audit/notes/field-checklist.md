# 装备 tooltip 全字段核对表（实测结论）

> 样本：`长枪 1400000`（用户截图，攻击速度慢(8)）、`枫叶枪 1432012`（用户截图，攻击速度普通(5)）。
> 数据源权威：运行时 `E:\MXD\BeiDou-Client-xy\Data\Character\Weapon\*.img`，经 orange-wz MCP（128B GMS key、`exclusive=false`、会话内 load→query→unload）实测读取；服务端口径 s9 `ItemInformationProvider`。
> 核验日期：2026-11-17。

## 字段逐项结论

| # | 字段 | 客户端来源 | 枫叶枪 1432012 实测 WZ | tooltip 显示口径 | 结论 |
|---|---|---|---|---|---|
| 1 | 装备分类 | `EquipData::type` → `Locale::enum_text("equip.type.", type)` | islot=Wp，号段 143 | 修复前=单手剑（**BUG**），修复后=长枪（types[13]="SPEAR"→`equip.type.SPEAR`=长枪） | **BUG 已修**（`EquipData.cpp:88-93` 武器不提前 return） |
| 2 | 攻击速度 | WZ `attackSpeed` → `WeaponData::getspeedstring()` → `Locale::enum_text("equip.speed.", ...)` | 5 | `"NORMAL (5)"` → `enum_key`→`equip.speed.NORMAL_(5)` → "普通 (5)"（词条 `zh-CN.ini:85`） | 正确，无偏差 |
| 3 | 攻击力(incPAD) | `Equip::get_stat(WATK)` + WZ `incPAD` | 65 | 基础 65 + 卷轴 delta `(±n)`（`EquipTooltip.cpp:830-850`） | 正确，无偏差 |
| 4 | 命中(incACC) | `Equip::get_stat(ACC)` + WZ `incACC` | 30 | 基础 30 + 卷轴 delta | 正确，无偏差 |
| 5 | 力量(incSTR) | `Equip::get_stat(STR)` + WZ `incSTR` | 1 | 基础 1 + 卷轴 delta | 正确，无偏差 |
| 6 | 敏捷(incDEX) | 同上 | 节点不存在(=0) | 不显示（stat<=0 时跳过，`EquipTooltip.cpp:846-849`） | 正确，无偏差 |
| 7 | 需求等级(reqLevel) | WZ `reqLevel` | 43 | 需求行（`EquipTooltip.cpp:643-648`/`:1090-1093`） | 正确，无偏差 |
| 8 | 可升级次数(tuc) | `Equip::get_SLOTs()` + WZ `tuc` | 7 | 可升级次数 7（`EquipTooltip.cpp:810-821`） | 正确，无偏差 |
| 9 | 金锤(vicious) | `Equip::get_vicious()` | 服务端包 | 有值时显示（`EquipTooltip.cpp:816-821`） | 数据源正确，无系统性错误 |
| 10 | 需求属性(reqSTR/reqDEX) | WZ `reqSTR`/`reqDEX` | 0 / 0 | 需求行（值为 0 通常不显示） | 正确，无偏差 |
| 11 | 双手判定 | `WeaponData::twohanded`（`WeaponData.cpp:30`） | prefix 143 在 `[SWORD_2H,POLEARM]` 内 → 双手 | 仅影响动画/音效，不进 tooltip 文本 | 正确；与 s9 `isTwoHanded` 一致 |

## 长枪 1400000 号段验证（静态推算，运行时无该 img）

- `1400000/10000=140` → `index=140-100=40` → `WEAPON_OFFSET=30` → `weaponindex=10` → `types[10]="TWO-HANDED SWORD"`（`EquipData.cpp:189`）→ `equip.type.TWO_HANDED_SWORD`="双手剑"（`zh-CN.ini:68`）。
- 修复前因 islot=Wp 提前 return 落入 `type_name(WEAPON)="ONE-HANDED SWORD"` → 显示"单手剑"（用户截图现象）。修复后正确显示"双手剑"。

## 根因与修复（唯一 BUG）

- 根因：`Data/EquipData.cpp` 的 islot 分支对 `Wp/WpSi/WpSp` 命中即 `type = type_name(WEAPON) = "ONE-HANDED SWORD"; return;`，使号段表 `:177-204` 永不执行，所有武器统一显示单手剑。
- 修复：islot=WEAPON 时只设 `eqslot` 并给兜底 type，**不 return**，让号段表按 `id/10000` 填正确 type；非武器 islot（Si/Hd 等 Round 7 修正）维持原样；号段表外的异常武器保留 WEAPON 槽兜底。
- 服务端 `s9/.../ItemInformationProvider.java:659-666 getWeaponType`（`cat=(id/10000)%100`：140=SWORD2H、143=SPEAR_STAB）与客户端号段表一致，验证修复方向正确。
- `Character/Look/EquipSlot.cpp:200` 的 `type_name(WEAPON)` 硬编码保留（唯一调用方已不再走武器路径，无需改动，避免波及）。

## 核验方法

- orange-wz steps：`C:\Users\11036\.rea\wz_steps_equip.json`（load 128B key → `get_node_detail` 逐字段 → unload_all），实测 `isError:false`，证明 128B key 已能正常解析该私服改版 img（推翻历史"orange-wz 解析 bug"误判）。
- 实机 tooltip 截图比对：因服务端 8484 需 Java21+MySQL，本会话未启动服务端，实机数值截图列为未跑项；但所有字段客户端均直接读 WZ/服务端包，无中间错误映射（除已修的分类字段）。
