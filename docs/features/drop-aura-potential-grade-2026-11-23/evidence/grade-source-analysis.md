# 掉落特效 grade 来源取证摘要（2026-11-23）

## 问题 1：192 DropItemEff 宝石是否原生遮挡？

- v214 `DropPool.java:110`：`outPacket.encodeByte(ItemGrade.getHiddenGradeByVal(drop.getItemGrade()).getVal())` —— 高版本按潜能品级下发隐藏档位。
- v214 `ItemGrade.java:11-16`：`HiddenLegendary(4) / HiddenUnique(3) / HiddenEpic(2) / HiddenRare(1) / None(0)`。
- `DropItemEff.img/4/front/0` 为实心八边形宝石素材（此前会话 orange-wz 导出确认）；192 按此在装备落点上叠宝石标记，遮挡是原生设计。
- 本端当前已去掉 `DropItemEff` tier 层，保留四色 `DropAuraEff`；不恢复宝石层。

## 问题 2：grade 语义对齐

| 场景 | 旧实现（Δ属性打分） | 新实现（潜能品级） | 192 原生 |
| --- | --- | --- | --- |
| 金币 | 0 | 0 | None(0) |
| 非装备 | 0 | 0 | None(0) |
| 无潜能装备 | 0（白板 Δ<6） | 0（p1=0） | None(0) |
| 有潜能装备 | 按 Δ属性 1..4 | 按 p1 分段 1..4 | hidden 1..4 |

## p1 → grade 映射（与客户端同构）

服务端（新 `getDropItemGrade`）、客户端（`Equip.cpp:24-35 potential_rank_of`）、v214（`ItemGrade` 隐藏档）三者对齐：

```
p1 = 0        → grade 0 → None          → 无光环
p1 ∈ [1,12]   → grade 1 → Rare / B      → HiddenRare
p1 ∈ [13,24]  → grade 2 → Epic / A      → HiddenEpic
p1 ∈ [25,36]  → grade 3 → Unique / S    → HiddenUnique
p1 ∈ [37,50]  → grade 4 → Legendary / SS→ HiddenLegendary
```

p1 值域由服务端两处坐实为 1..50：
- `CashOperationHandler.java:98`：`equip.setPotential1(r.nextInt(50) + 1)`
- `EnhanceConfig.java:71,73`：`POTENTIAL_RANDOM_BOUND = 50`、`POTENTIAL_MAX = 50`

## 协议兼容性

- 字段：`[grade:1B][playerDrop:1B]`，`PacketCreator.java:2025/:2056` 两处 writeByte/writeBool。
- 客户端 `MapObjectHandlers.cpp:864-873` 按 `available()>=2` 判定新旧服务端：新服务端读 grade，旧服务端无 grade 字节按普通掉落。协议未变，双向兼容。
- grade 经 `spawn({..., grade})` 透传到 `Drop.cpp:31`，front/back 光环按 `has_grade(grade)`/`has_back(grade)` 门控；本次仅补光柱 `grade>=1` 门控。
