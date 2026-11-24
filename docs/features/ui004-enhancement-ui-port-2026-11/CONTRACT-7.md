# CONTRACT-7 — 占位族 14..20 真实逻辑补充契约

> 依据：一手 `tools/_192txt/Cash.u8.txt`（CMS192 道具描述）+ 公开资料（见 DELIVER 报告来源）。
> 原则：概率/奖池/权重一律配置化（`EnhanceConfig7`），公开数字仅作初值；CMS192 未确证项标注。
> 复用 API：`AbstractPlayerInteraction.gainItem(id, qty, false)`、`canHoldAllAfterRemoving`、`Equip` setter、`MakerItemFactory`。全部改动走 `modifyInventory + forceUpdateItem` 同步客户端。

## 族 14 — 石净化 stonePurify（5060023 混沌净化石）
- 输入：cashId=5060023（净化石）+ 混沌箱（配置 `PURIFY_BOX_ID`，默认取 192 箱 id；背包需持有 ≥1）
- 逻辑：消耗净化石×1 + 混沌箱×1 → 从奖池 `PURIFY_REWARDS`（itemId,weight）随机 1 件 → gainItem
- 成功/失败：必出 1 件（一手文本"获得一种"）；UI success/fail 动画按是否抽到稀有档（isRare）决定
- 概率：奖池权重配置化，初值用 TMS 活动奖池简化（无 CMS192 官方权重 → 标注）

## 族 15 — 宝石炼金 jewelCraft（Jewel Craft 宝石合成）
- 输入：matSlot/cashId 两个同级宝石（itemId 属 `JEWEL_IDS`）
- 逻辑：按当前等级取概率 `JEWEL_RATE[lv]`（C→B 70% / B→A 50% / A→S 30%，社区整理值）
  - 成功：消耗 2 → 产出 1 个高一级宝石
  - 失败：消耗 2 → 返还 1 个原级宝石（一手社区"失败返还原级"）
- 上限：S 级不可再合成；CMS192 是否实装未确证 → 配置开关 `JEWEL_ENABLED` 默认 true（因 UI 存在）

## 族 16 — 协同机 SynergyMachine（2048400/2048401/2048402/2048406）
- 输入：两个消耗品（cashId + matSlot 对应 itemId）属 `SYNERGY_RECIPES`
- 逻辑：2 件 → 1 件，100% 成功（公开取证）；产物按配方配置池随机
  - 2048402 宝石合成机在 CMS192 标注"现在无法使用" → 配置 `SYNERGY_2048402_ENABLED=false` 默认关闭
- 排除产物类不可再合成（配置 `SYNERGY_RESULT_IDS`）

## 族 17 — 卷轴协同 ScrollSynergyMachine（2048403 卷轴合成机）
- 输入：2 张卷轴（USE 页 itemId 以 204 开头，且非神秘卷轴书产物）
- 逻辑：消耗 2 → 产出 1 本神秘卷轴书（`SCROLL_SYNERGY_RESULT` 配置池，100%），产物 untradeable
- 注意：这是"卷轴合成"不是"砸卷"，砸卷走既有 ScrollEquip，不动

## 族 18 — 物品罐 itemPot（药剂罐 5820000/5820001）
- 持久字段缺失：s9 玩家无 potionPot 容量字段 → 用现有 `Equip`? 否。改用**客户端本地 UI 状态 + 背包药水消费**：
- 逻辑：cashId 为 HP/MP 药水（200xxxx）时，按 120% 系数折算 → 直接恢复角色 HP/MP（`chr.setHp/Mp`），消耗药水×1
- 容量/自动填充/时效：属客户端 UI 持久状态，s9 无字段 → **显式预留**（注释写明），本版实现"拖入药水→回血回蓝 120%"核心效果
- 配置：`POT_HEAL_FACTOR=1.2`

## 族 19 — 大师作品/神奇铁砧 MasterPiece（5062400）
- 输入：cashId=5062400（铁砧）+ equipSlot=功能件 + matSlot=外形件（均为 Equip）
- 校验：同部位类别（12 部位白名单）、同性别或通用、非 Zero 武器、两件不同 slot
- 逻辑：100% 成功 → 新 Equip = 复制功能件全部属性 + 外形件的 itemid 作为外观（服务端 Equip 有 `skin`? 若无 → 用 `Equip.setSkin` 或等价外观 id 字段；**若无外观字段则退化为"保留功能件属性、消耗外形件"并标注外观同步预留**）
- 消耗：功能件 + 外形件 + 铁砧×1 → 产出合成件
- 配置：`MASTERPIECE_ALLOWED_PARTS`（12 类）

## 族 20 — 灵魂收集 SoulCollector
- 输入：cashId=灵魂碎片（配置 `SOUL_SHARD_IDS`，192 常见 boss 碎片 id）
- 逻辑：
  - 碎片 ≥10 同种 → 消耗 10 → 产出 1 个 Boss 魂（配置 `SOUL_RESULT_IDS`，词缀/属性按权重随机，普通魂 30 天）
  - 若 cashId 为灵魂武器 + matSlot 为魂 → 装魂：soul 字段 = 魂的 option id，+20 攻魔（写 watk/matk），覆盖旧魂（100%）
- 配置：`SOUL_SHARD_TO_SOUL=10`、`SOUL_EQUIP_ATK=20`

## 同步与回包
- 所有族执行后：`modifyInventory` + `forceUpdateItem`（若涉及 Equip）+ `sendEnhanceResult`（action/result/equipSlot）
- 消耗与产出后发 `gainItem` 自身的背包包（复用现有 API）
- 失败（校验不通过/材料不足）：result=0，**不消耗**任何道具
- 日志：入口/校验失败原因/消耗/产出/DB 失败 = INFO/WARN/ERROR，无 DBG 残留

## 诚实边界（写入 DELIVERY）
- 概率/奖池/权重是社区或跨版本资料初值，非 CMS192 官方；全部配置化可改
- 药剂罐容量/自动填充、铁砧外观字段、Soul option 数值表在 s9 无字段/无数据 → 显式预留，本版实现核心可落地部分
- 192 私有端这些类的源码级逻辑无法 web 取证，本契约是"公开机制 + 本端数据结构"的重建，非 192 原样复刻
