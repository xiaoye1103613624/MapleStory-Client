# 掉落特效对齐 192 潜能品级（2026-11-23）

## 一、需求与结论

用户问两点：
1. 192 的 `DropItemEff` 是一枚宝石会遮住实际掉落的装备，原来就是这个逻辑吗？
2. 对齐 192 潜能品级。

**结论：**
- 是的，`DropItemEff/4/front/0` 官方资源就是**实心八边形宝石**，192 原生按潜能品级叠一枚宝石标记，遮挡是设计如此，不是误导入。本端此前叠加四色光环导致遮挡更重，已去掉 `DropItemEff` 层、只保留四色 `DropAuraEff`——**本次不恢复宝石层**。
- grade 来源由旧的「Δ属性打分」改为「第一条潜能 id（p1）分段」，与客户端 `Equip::get_potrank()` 和 v214 原生 `ItemGrade.getHiddenGradeByVal` 语义对齐。

## 二、证据链（行号级）

| 证据 | 文件:行 | 内容 |
| --- | --- | --- |
| 高版本原生 grade 下发 | `E:/MXD/265/v214 src/.../DropPool.java:110` | `ItemGrade.getHiddenGradeByVal(drop.getItemGrade()).getVal()` |
| 品级取值 | `.../ItemGrade.java:11-16` | `HiddenLegendary=4 / HiddenUnique=3 / HiddenEpic=2 / HiddenRare=1 / None=0` |
| 本端客户端潜能分段 | `Character/Inventory/Equip.cpp:24-35` | `p1: 0→NONE / 1-12→Rare / 13-24→Epic / 25-36→Unique / 37-50→Legendary` |
| 本端服务端 p1 取值 | `BeiDou-Server_s9/.../Equip.java:800-801` | `getPotential1()` 返回 `int` |
| p1 值域（随机场景） | `.../CashOperationHandler.java:98` | `r.nextInt(50)+1` → 1..50 |
| p1 值域（魔方场景） | `.../EnhanceConfig.java:71,73` | `POTENTIAL_RANDOM_BOUND=50 / POTENTIAL_MAX=50` |
| 协议 grade 字段 | `Net/Handlers/MapObjectHandlers.cpp:864-879` | `[grade:1B][playerDrop:1B]`，`available()>=2` 版本门控 |
| grade 透传 | `Gameplay/MapleMap/Drop.cpp:31` | `grade = grd` |

## 三、改动清单

### 服务端 `gms-server/src/main/java/org/gms/util/PacketCreator.java`
- `:1942-1979` `getDropItemGrade(MapItem drop)` 重写：
  - 金币 / 非装备 → 0。
  - 装备按 `equip.getPotential1()`（第一条潜能 id）分段：
    - `p1 <= 0` → 0（无潜能，不上光环）
    - `1-12` → 1（Rare / B）
    - `13-24` → 2（Epic / A）
    - `25-36` → 3（Unique / S）
    - `≥37` → 4（Legendary / SS，值域上限 50）
  - 删除旧 Δ属性打分逻辑（不再依赖 `getEquipById` 白板对比）。
- 下发点 `:2025`/`:2056` 不变，协议字节不变（旧客户端兼容）。

### 客户端 `Gameplay/MapleMap/Drop.cpp`
- `:52-57` 光柱挂载门控加 `grade >= 1`：`grade=0` 的普通掉落不再挂共用光柱；front/back 光环本就由 `has_grade`/`has_back` 内部按 grade 门控，无需改。

**未改**：协议 opcode/字段、`Equip.cpp` 映射、`DropAuras.cpp/.h`、`DropItemEff.img` 资源。

## 四、验证（实测，非「应该没问题」）

- **服务端**：`mvn -q -o compile -DskipTests` → `MVN_EXIT=0`（JDK 21 / Maven 3.9.16）。
- **客户端**：x64 Release 全量构建中 `Drop.cpp` 编译干净（编译日志第 216-218 行，仅全工程既有 `USE_NX` 宏重定义 warning，无 error）。
  - 全量构建 EXIT=1 的唯一 error 是 `Data/SkillData.cpp` 的 `SkillData::Stats::Stats` 缺默认构造函数——该文件属工作区既有未提交改动（git status 实证），**与本次掉落特效改动无关**。
  - 说明：本工作区为长期脏工作区（客户端数十个 modified 文件、服务端 82 个 modified 文件），本次只在 `Drop.cpp` 加了 1 行条件门控。
- **真机未验收**：未进游戏实拍掉落特效；服务端 grade 需重启后生效。按用户规范如实声明。

## 五、风险与回滚

- p1 值域 1..50 已用 `CashOperationHandler` / `EnhanceConfig` 双源确认，映射无溢出。
- 回滚：还原 `PacketCreator.java:1942-1979` 与 `Drop.cpp:52-53` 两处即可；协议字段未变，无需客户端/服务端同步回滚。
- 配置 DB 无需变更（grade 直接读装备实例 p1）。

## 六、产物

- `patches/apply_potential_grade.py` —— 本次 CRLF 精确切块补丁脚本（可重放/复核）。
- `evidence/` —— 上述行号级证据摘要。
