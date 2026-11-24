# 装备槽位对照审计：192 vs Journey（xy）

> 日期：2026-11-23  
> 性质：**只读取证 + 缺口矩阵 + 决策点**（本轮不实现大改）  
> 方法：orange-wz MCP（192 零 IV `AAAAAA==` + 128B GMS key；xy GMS IV `TSPHKw==` + 同 128B key）+ 客户端 / s9 源码 + 既有 `addon-to-native-android.md` / `official-slot-mapping.md`  
> **布局决策计划（2026-11-23 重核后）**：见同目录 [`equip-slot-192-layout-plan.md`](equip-slot-192-layout-plan.md)（含 +2 戒不可用结论、totem→192、Addon 残留分流、像素提案）。

## 0. 直接回答用户问题

| 问题 | 结论 | 证据摘要 |
| --- | --- | --- |
| 高版本装备槽有没有增加？ | **有（相对 classic v083 主栏）** | 192 `Equip/Equip/Slots` 除 1–17 外另有 21–38 等命名槽；另有 Android / Dragon / Mechanic / Coordinate / Haku / **totem** 页签 |
| 基础装备槽有没有增加？ | **有，但是 BP 号段与 v083 不同** | 192 主栏水印：`21=MEDAL`、`22=BELT`、`23=SHOULDER`、`33=口袋`、`36=徽章`、`37=符号`、`38=第二吊坠`；戒指仍是 **4** 个 |
| 是否 6 戒 + 2 项链？ | **否 / 半对** | **戒指：192 仅 12/13/15/16（4 戒），无 52/53**。**项链：192 有 2 个吊坠 UI**（`17` + `31`/`38` 同格别名），不是 6+2 整包 |
| 辅助/Addon 是否有十几个图腾槽？ | **否** | 192 `Equip/totem` 仅 `backgrnd` **141×70，可视 3 格图腾**；本端 Addon 图腾 **4** 格。无 dozen+ |
| 辅助装备 UI 是不是只有 4 格？ | **位图是 2×4=8 格；逻辑绑 7 格** | live `AddonEquip/backgrnd` 上排 4 图腾 + 下排 纹章/机器人/心脏/徽章；代码 `UIAddonEquip::SLOTS` = 7（机器人格已空出不绑） |
| 机器人槽是否应挪到 Android 绘制 UI？ | **是（且穿戴路径已迁）** | 192 原生：`Equip/Android/Slots/1200…1206` + Angel `CAndroidTab`；本端 `UIAndroidEquip` 已接 1200–1206。Addon 位图仍烤着「机器人」水印 = **视觉债** |

---

## 1. 资源证据（orange-wz）

### 1.1 源路径与密钥

| 端 | 路径 | Key |
| --- | --- | --- |
| 192 | `E:\MXD\【2023.8.8江浩生日福利】怀旧岛192整合版\怀旧岛192客户端\冒险岛online192\Data\UI\UI_005.wz` → `UIWindow4.img/Equip` | IV `AAAAAA==` + 128B `tools/_gms_key_128.b64` |
| xy live | `E:\MXD\BeiDou-Client-xy\Data\UI\UIWindow4.img`（已含 192 Equip 转钥产物） | IV `TSPHKw==` + 同 128B |
| xy Addon | `E:\MXD\BeiDou-Client-xy\Data\UI\UIWindow.img/AddonEquip` | 同上 |
| xy classic Equip | `UIWindow.img/Equip`（无 `Slots` 子树；坐标硬编码在 `UIEquipInventory`） | 同上 |

`exclusive:false`；会话末 `unload_all`。

### 1.2 192 / xy `UIWindow4.img/Equip` 直接子节点（一致）

`backgrnd`, `backgrnd2`, `tabbar`, `Equip`, `Cash`, `Pet`, `Android`, `Dragon`, `Mechanic`, `Coordinate`, `Tab`, `Zero_Cash`, `Angel_Cash`, `Haku`, **`totem`**

- **无** `AddonEquip` / `MysticBadge` 节点（192 与 UIWindow4 均无）。
- `Tab/enabled` = `0..7`（多页签壳）。

### 1.3 192 主栏 `Equip/Equip/Slots`（名 = BodyPart）

节点名（与 xy live 一致）：

```text
1..13, 15, 16, 17,
21, 22, 23, 26, 27, 28, 29, 30, 30_2, 31,
33, 34, 35, 36, 37, 37_2, 38
```

**没有** `49/50/51/52/53/54–61/70–75/1200` 出现在主栏 Slots。

水印读图（`evidence/equip-slot-audit-pngs/`）+ origin：

| BP | 水印 | origin (x,y) | 备注 |
| ---: | --- | --- | --- |
| 12 | 戒指 | (−14,−173) | 戒 1；13/15/16 为 1×1 锚 |
| 17 | 吊坠 | (−55,−132) | 项链 1 |
| 21 | MEDAL | (−178,−132) | 勋章；与 26「奖牌」同 origin |
| 22 | BELT | (−55,−214) | 腰带；与 29「腰带」同 origin |
| 23 | SHOULDER | (−137,−173) | 护肩；与 30「肩部」同 origin |
| 31 | PENDANT | (−55,−91) | 与 38「吊坠」同 origin → **第二项链 UI** |
| 33 | 口袋 | (−137,−91) | Pocket |
| 34 | 机器人 | (−137,−255) | 主栏上的 Android 入口/占位水印 |
| 35 | 心 | (−178,−255) | Android Heart |
| 36 | 徽章 | (−178,−91) | Badge |
| 37 | 符号 | (−178,−50) | Arcane/Authentic Symbol 类 |
| 38 | 吊坠 | (−55,−91) | 第二吊坠（中文水印） |

→ **192 主栏可视：4 戒 + 2 吊坠 + 勋章/腰带/护肩/口袋/徽章/符号/机器人水印/心脏**，不是「只多两戒」。

### 1.4 Android / totem / 其它页签

| 页签 | Slots | 结论 |
| --- | --- | --- |
| `Android` | **1200…1206**（7）+ `backgrnd` 162×209 + `BtShop` | 原生机器人栏 BodyPart |
| `totem` | **仅 `backgrnd` 141×70**（可视 **3** 格「图腾」） | **无 Slots 子节点；非 dozen+** |
| `Dragon` | 1000…1003 | 龙神 |
| `Mechanic` | 1100…1104 | 机械 |
| `Coordinate` | 1000, 1300…1304, 2000 | 坐标/时装相关 |
| `Haku` | 5200 | 一尾 |
| `Pet` | Pet1–3, 114, 121–125, Skill*, 200/201 | 宠物已离主栏 |

### 1.5 xy 自绘 Addon / Mystic

| 节点 | 尺寸 | 内容 |
| --- | --- | --- |
| `UIWindow.img/AddonEquip/backgrnd` | 152×105 | 标题「辅助装备」；上 4×图腾；下 纹章 / **机器人** / 心脏 / 徽章 |
| `UIWindow.img/MysticBadge` | 存在 | 本端扩展；**192 无对应节点** |

---

## 2. 客户端 / s9 现状

### 2.1 `EquipSlot.h`（枚举值 == wire |BodyPart|）

| 组 | Id | 说明 |
| --- | ---: | --- |
| 基础 | 1–13,15–20,49–51 | 含 **4 戒**、PENDANT1=17、PENDANT2=**51**、SHOULDER=**20**、MEDAL=49、BELT=50 |
| Addon | 54–61 | BADGE/TOTEM1–4/EMBLEM/ANDROID_SLOT(弃用60)/HEART |
| Mystic | 70–75 | 扩展；非 192 |
| Android 原生 | **1200–1206** | 与 192 / Angel 对齐 |

**未采纳（历史定案）**：RING5/6=`−52/−53`、POCKET=`−33`（与宠物 BP 冲突风险）、AUX_WEAPON=`−62`。

### 2.2 UI 归属

| UI | 槽 | 说明 |
| --- | --- | --- |
| `UIEquipInventory` | classic 主栏坐标 | 4 戒 + PENDANT2(51) + SHOULDER(20) 等；吃 `UIWindow.img/Equip`，**不**吃 192 `Equip/Equip/Slots` |
| `UIAndroidEquip` | ANDROID1…7 = 1200…1206 | `backgrnd2` + `Android/backgrnd`；**正确归属** |
| `UIAddonEquip` | TOTEM×4 + EMBLEM + HEART + BADGE（7） | 自绘；**(44,65) 机器人不绑**；位图仍有「机器人」 |
| `UIMysticBadge` | MYSTIC×6 = 70–75 | 自绘扩展 |

### 2.3 s9 `EquipSlot.java`

- RING：`−12/−13/−15/−16`（**无** −52/−53）
- PENDANT：`−17/−51`
- TOTEM：`−55…−58`
- ANDROID：`−1200…−1206`（+ cash −1300…）并兼容 `−60/−160`
- MYSTIC：`−70…−75`
- **无** Pocket / Symbol / 192 BP21–38 重映射表

### 2.4 穿戴空位

`Inventory::find_equipslot`：戒指扫 RING2–4；图腾扫 TOTEM1–4；Android 扫 1200–1206。无第 5/6 戒。

---

## 3. 与既有文档对照

| 文档 | 对齐点 |
| --- | --- |
| `notes/addon-to-native-android.md` | Addon ≠ 192；Android 1200–1206；机器人应离 Addon — **本审计确认** |
| `evidence/192-equip-android.md` | Android Slots 1200–1206 — **确认** |
| `evidence/gap-matrix.md` G8/G8b/G9 | Android 迁移中；Addon 自定义；六戒/口袋 NO-FIX — **仍成立**；本审计补充 **192 主栏 BP 重映射与 totem=3** |
| `equip-slot-expansion…/official-slot-mapping.md` | S10 六戒/口袋为私服 sidecar；**不等于 192 UIWindow4 原生 Slots** |

MinMS：无 192 Android/totem 多页签壳；Angel：`CAndroidTab` wire −1200…−1206（既有笔记）。

---

## 4. 缺口矩阵（192 vs Journey）

### 4.1 基础装备

| 槽语义 | 192 UI BP | Journey BP / UI | Gap |
| --- | ---: | --- | --- |
| 戒指 | **4**（12/13/15/16） | **4**（同） | **无「缺 6 戒」相对 192**；相对 S10 六戒则缺 −52/−53（故意 NO-FIX） |
| 项链 | **2**（17 + 31/38） | **2**（17 + **51**） | **数量齐，BP 号不一致**（192=`38`，本端=`51`） |
| 勋章 | **21**（26 别名） | **49** | BP 重映射未接 192 主栏 |
| 腰带 | **22**（29 别名） | **50** | 同上 |
| 护肩 | **23**（30 别名） | **20** | 同上 |
| 口袋 | **33** | 无（NO-FIX） | 192 有；本端刻意不做 |
| 徽章 | **36**（主栏） | **54**（Addon） | 语义在、面板不同 |
| 符号 | **37** | 无 | 高版本进度装备；未立项 |
| 心脏 | **35**（主栏水印）+ Android 体系 | **61**（Addon） | 应否随 Android 面板 — 待抉择 |
| 机器人入口 | **34** 主栏水印 + **Android 页** | 侧栏 `UIAndroidEquip` | 穿戴已对；主栏水印未接 |

Journey 主窗仍是 **classic `UIWindow.img/Equip` 坐标**，**未**切换到 192 `Equip/Equip/Slots` 布局。

### 4.2 辅助 / 图腾

| 项 | 192 | Journey | Gap |
| --- | --- | --- | --- |
| 产品形态 | `Equip/totem` 页签，**3** 格 | 自绘 `UIAddonEquip`，图腾 **4** + 纹章/心脏/徽章 | 产品不同；**非 dozen+** |
| Addon 总格 | 无 AddonEquip | 位图 8 / 逻辑 7 | 用户「4 格」≠ 位图事实（可能只数上排或旧印象） |

### 4.3 Android

| 项 | 192 | Journey | Gap |
| --- | --- | --- | --- |
| BodyPart | 1200–1206 | 同 + 兼容 60 | **对齐** |
| UI | `Equip/Android` | `UIAndroidEquip` | **应对齐归属**；已实现侧栏 |
| 仍在 Addon | — | 位图「机器人」水印 + 历史格位 | **应清视觉 / 勿再绑穿戴** |
| 心脏 | 主栏 35 + Android 生态 | Addon HEART | 是否并入 Android UI — **决策点** |

### 4.4 Mystic

192 **无**；本端 70–75 扩展可保留（gap-matrix G7）。

---

## 5. 推荐与决策点（不自动实现）

### 已有证据可直接推荐

1. **Android 槽归属 `UIAndroidEquip`（1200–1206）** — 与 192 / Angel 一致；Addon 不应再承担穿戴。  
2. **不要按「dozen+ 图腾」扩槽** — 192 totem 页 = **3**；本端 Addon = **4** 已多于 192 页签。  
3. **不要为对齐 192 而强上 6 戒** — 192 原生 UI 只有 4 戒；6 戒是 S10 sidecar，非本轮 192 IMG 证据。

### 待用户抉择

| # | 议题 | 选项 | 建议 |
| --- | --- | --- | --- |
| D1 | 第二项链 BP | A) 维持 `51`（v083/s9）　B) 改跟 192 `38`　C) 双号兼容 | 短期 **A**；若整窗切 192 主栏再评 B/C |
| D2 | 主栏是否切 192 `Equip/Equip/Slots` | A) 继续 classic + 侧栏　B) 整页迁 UIWindow4 Equip 页签 | **B 为大工程**；需 Plan |
| D3 | 勋章/腰带/护肩 BP | A) 维持 49/50/20　B) 跟 192 的 21/22/23 | 与 D2 绑定；单改号易穿戴错位 |
| D4 | 心脏 | A) 留 Addon　B) 迁入 Android UI　C) 主栏 35 + Android | 推荐 **B 或 C**（192 心脏贴近机器人） |
| D5 | Addon 去留 | A) 保留自绘（清机器人水印）　B) 改接 192 `totem` 3 格 + 其它分流 | 推荐 **A 清视觉债**；B 需产品确认图腾 3 vs 4 |
| D6 | 口袋 / 符号 / 六戒 | 继续 NO-FIX / 立项 | 口袋与宠物冲突仍在；符号需独立玩法 |

### Plan 就绪条目（确认后）

1. Addon 位图 blank「机器人」格 → 原子替换 live `UIWindow.img`（关客户端）。  
2. 文档/UI 文案：辅助 ≠ 192；totem 3 vs Addon 4 写清。  
3. （若选 D2）192 主栏 Slots origin → `UIEquipInventory` 坐标表 + s9 BP 映射表。  
4. （若选 D4）`HEART` 命中矩形迁 `UIAndroidEquip`。

---

## 6. 证据附件

- PNG / origins：`evidence/equip-slot-audit-pngs/`（`origins.json`、`slot*.png`、`totem_backgrnd.png`、`addon_backgrnd.png`）
- 既有：`evidence/192-equip-android.md`、`notes/addon-to-native-android.md`、`evidence/gap-matrix.md`
- 源码：`Character/Look/EquipSlot.{h,cpp}`、`IO/UITypes/UI{EquipInventory,AddonEquip,AndroidEquip,MysticBadge}.*`、s9 `EquipSlot.java`

---

## 7. 一句话摘要（中文）

**192 相对 classic 主栏确实加了槽（勋章/腰带/护肩/口袋/徽章/符号/第二吊坠等），但戒指仍是 4 不是 6；项链 UI 是 2（BP 与本端 51 不同）；图腾原生页只有 3 格不是十几个；机器人应（且已）在 Android UI（1200–1206），辅助窗位图上的机器人应清掉。**
