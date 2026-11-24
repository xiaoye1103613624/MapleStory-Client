# 装备槽布局决策计划：192 优先（Journey / xy）

> 日期：2026-11-23  
> 性质：**决策已锁定 + 本轮已实现**  
> 上游审计：[`equip-slot-192-audit.md`](equip-slot-192-audit.md)  
> 方法：orange-wz（192 零 IV / xy GMS `TSPHKw==` + 128B GMS）+ 本仓 C++ / s9

---

## 0. 一句话结论（LOCKED）

| ID | 锁定 | 本轮落地 |
| --- | --- | --- |
| **D-RING** | **上 6 戒** | RING5/6 = **BP 52/53**；主栏右上 (104,32)/(137,32)；双端 wire + `find_equipslot` |
| **D-TOTEM4** | **废弃** | `UITotemEquip` ← 192 `Equip/totem`（3 格）；`find_equipslot` 只扫 55–57 |
| **D-ADDON** | **分流后删除** | 徽章/纹章→主栏；心脏→`UIAndroidEquip`；开窗流不再 emplace Addon |
| **D-POCKET** | **立项解冲突** | POCKET=33 / `Po` / −33；PetBodyPart / s9 剔除 BP33；−133 仍宠装 |
| 第二项链 | **保持 BP51** | 无改号 |

---

## 1. 六戒 ID 证据（079 / S10 / 社区）

| 来源 | 结论 | 路径 / URL |
| --- | --- | --- |
| **S10（本机权威）** | `RING("Ri", -12,-13,-15,-16,`-52`,`-53`,-152,-153)`；注释「6 戒…新增 −52/−53」 | `E:\project\BeiDou-Server_S10\gms-server\src\main\java\org\gms\constants\inventory\EquipSlot.java` |
| **本仓映射表** | RING5/6 = −52/−53（曾 Round4 不采纳，本轮 D-RING 翻案） | [`equip-slot-expansion-2026-11/official-slot-mapping.md`](../equip-slot-expansion-2026-11/official-slot-mapping.md) §1 |
| **MapleWiki** | 「ChinaMS can wear 6 rings at once」（国服六戒事实；未列 BP 号） | https://maplestory.fandom.com/wiki/Ring |
| **192 UIWindow4** | 主栏仅 12/13/15/16（**无** 52/53）→ 六戒属 **CMS079/S10 sidecar**，非 192 原生 | orange-wz `Equip/Equip/Slots` |
| **公开 CMS079 BodyPart 枚举** | 本轮未找到可引用的开源 `BodyPart` 源码把 RING5/6 标成 52/53 的直链；**采用 S10 + 社区 sidecar 映射 52/53**，与用户「prefer CMS079/S10；若无则 52/53」一致 | — |

**采用 ID：RING5 = 52，RING6 = 53**（wire −52/−53；cash −152/−153）。

扫描序：`RING2→3→4→1→5→6`。

---

## 2. 192 orange-wz 重核（仍有效）

根：`…\Data\UI\UI_005.wz` → `UIWindow4.img/Equip`（零 IV）/ xy live 同结构（GMS IV）。

- 戒指 UI 节点：仅 `12/13/15/16`
- `Equip/totem/backgrnd`：**141×70**，可视 3 格；无 Slots
- 主栏有 `33` 口袋 / `36` 徽章 / `35` 心 水印；无 AddonEquip

---

## 3. Keep / Move / Remove（锁定后）

| 项 | 决策 | 实现要点 |
| --- | --- | --- |
| 戒指 ×6 | **上 6 戒** | `EquipSlot` RING5/6；`UIEquipInventory` (104,32)/(137,32)；s9 RING 含 −52/−53 |
| 第二项链 | **BP51** | 不变 |
| 图腾 | **192 三格窗** | `UITotemEquip`；TOTEM4 兼容下行/卸下，不新穿 |
| 徽章 | **主栏 HV** | iconpositions (137,197) |
| 纹章 | **主栏空格** | (71,32) |
| 心脏 | **Android 侧** | `UIAndroidEquip` 第 8 格底栏 |
| 口袋 | **主栏 + 解冲突** | (104,197)；BP33 出宠装表 |
| Addon 窗 | **删除开窗** | `sync_side_panels` 强制 deactivate；类型位保留 |

### 3.1 classic 主栏坐标（现行）

| 槽 | 坐标 |
| --- | --- |
| RING5 / RING6 | (104, 32) / (137, 32) |
| EMBLEM | (71, 32) |
| RING3 / RING4 | (104, 65) / (137, 65) |
| RING1 / RING2 | (104, 164) / (137, 164) |
| POCKET / BADGE | (104, 197) / (137, 197) |
| PENDANT2 | (38, 98) |

### 3.2 口袋 vs 宠物（已落地解法）

| 层 | 改动 |
| --- | --- |
| 客户端 `PetBodyPart` | TAB1 / `is_pet_bodypart` **剔除 33**；`is_pet_bodypart(133)=true` |
| `EquipSlot::from_wire` | case 33→POCKET；**133 不减 100** |
| s9 `PET_BODY_PARTS` | 去掉 33；`isPetEquipStorageSlot` 对 133 特判 true |
| `EquipData` / IIP | prefix 116 / islot `Po` → POCKET |

残留风险：旧档若把非口袋装写在 −33，或宠装仍占 BP33，需进图后人工卸下；见 README。

---

## 4. 实现文件清单

- 客户端：`EquipSlot.{h,cpp}`、`Inventory.cpp`、`PetBodyPart.h`、`EquipData.cpp`、`UIEquipInventory.cpp`、`UITotemEquip.{h,cpp}`、`UIAndroidEquip.{h,cpp}`、`UIElement.h`、`UI.cpp`、`InventoryHandlers.cpp`、`Player.cpp`、`MapleStory.vcxproj`
- s9：`EquipSlot.java`、`ItemInformationProvider.java`
- 文档：本文件 + feature README 口袋小节

---

## 5. 证据索引

- S10 `EquipSlot.java` RING −52/−53  
- MapleWiki Ring（ChinaMS 六戒）  
- orange-wz：xy `UIWindow4.img/Equip/totem/backgrnd` 141×70  
- 审计：`equip-slot-192-audit.md`、`official-slot-mapping.md`
