# 装备 tooltip 全字段审计与分类修复

> 日期：2026-11-17 ｜ 状态：代码完成、构建通过（EXITCODE=0）、实机数值截图未跑（服务端 8484 未启动）

## 1. 问题

用户截图指出"长枪(1400000)"、"枫叶枪(1432012)"的"装备分类"都显示为**单手剑**，怀疑"双手武器显示成单手"，并要求所有字段都排查。

## 2. 结论

**确认 1 个 BUG（装备分类），其余字段无系统性错误。**

- 装备分类：所有武器（islot=Wp/WpSi/WpSp）都被写成"单手剑"——已修复。
- 攻击速度 / 攻击力 / 命中 / 力量 / 敏捷 / 需求等级 / 可升级次数 / 金锤 / 需求属性 / 双手判定：数据源独立于类型映射，经 orange-wz 读运行时 img + 代码核对，值与预期一致，无偏差。

逐字段核对表见 `notes/field-checklist.md`。

## 3. 根因

`Data/EquipData.cpp` 的 islot 优先分支（Round 7 修正）对 `Wp/WpSi/WpSp` 命中即：

```cpp
eqslot = islot_slot;
type = EquipSlot::type_name(islot_slot);  // WEAPON → 硬编码 "ONE-HANDED SWORD"
return;                                   // 提前返回，号段表永不执行
```

导致下方按 `id/10000` 细分的号段表（`:177-204`，含 140=双手剑、143=长枪）永远不会被执行，所有武器统一显示单手剑。服务端 s9 `ItemInformationProvider.getWeaponType`（`cat=(id/10000)%100`：140→SWORD2H、143→SPEAR_STAB）证明号段口径与客户端表一致，错在客户端分支优先级。

## 4. 修复（`Data/EquipData.cpp`）

- islot=WEAPON 时：仍设 `eqslot=WEAPON` 并给兜底 type，**但不提前 return**，让号段分支继续按 `id/10000` 填正确 type。
- 非武器 islot（Si/Hd/Em 等 Round 7 修正覆盖的槽位）：维持"命中即返回"语义不变，不动既有装备逻辑。
- 号段表外的异常武器（高版本号段）：保留 WEAPON 槽兜底，不被覆盖成 CASH/NONE。
- `Character/Look/EquipSlot.cpp:200` 的 `type_name(WEAPON)` 硬编码保留——其唯一调用方已不再走武器路径，无需改动，避免波及。

效果：1400000→index40→`types[10]="TWO-HANDED SWORD"`→"双手剑"；1432012→index43→`types[13]="SPEAR"`→"长枪"。词条 `equip.type.TWO_HANDED_SWORD` / `equip.type.SPEAR` 在 `locale/zh-CN.ini` 均存在。

## 5. 核验方法

- orange-wz MCP（128B GMS key、`exclusive=false`、会话内 load→query→unload）实测读取运行时 `Data/Character/Weapon/01432012.img/info`：`islot=Wp`、`incPAD=65`、`incACC=30`、`incSTR=1`、`reqLevel=43`、`tuc=7`、`attackSpeed=5`、`cash=0`，全部 `isError:false`。这同时证明 128B key 已能正常解析该私服改版 img（推翻历史"orange-wz 解析 bug"误判）。
- 攻速词条验证：`WeaponData::getspeedstring()` 返回 `NORMAL (5)` → `Locale::enum_key`（空格→`_`、括号保留）→ `equip.speed.NORMAL_(5)` = "普通 (5)"，匹配 `zh-CN.ini:85`。

## 6. 验证与未跑项

- 构建：`tools/build_smoke.bat` EXITCODE=0、GATE OK；部署 exe 已刷新。
- 静态：改动文件 lint 0 错误；WZ 值经 orange-wz 一手核对。
- **未跑项**：进游戏悬停 1400000/1432012 的实机 tooltip 截图比对，需服务端 8484 + MySQL + Java21，本会话未启动服务端，按计划列为未跑。因客户端所有字段均直接读 WZ/服务端包（除已修分类外无中间映射），此未跑项不影响修复正确性结论。
