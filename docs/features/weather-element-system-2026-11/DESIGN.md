# 天气 × 元素属性克制系统 — 全链路设计与决策点（2026-11-23，仅设计未实现）

> 需求：①把上一轮"快速切换"改为持久 DB；②不同天气给"怪物相同属性增强、相克属性削弱"，也包含角色技能属性增强/削弱，全面设计。
> 状态：**证据解析 + 设计完成，等用户拍板 6 个决策点后才写代码**（skill 门禁：先证据再分析后抉择再计划）。

## 0. 证据边界（两个子代理只读调查 + lead 抽查关键行号）
- 记忆：Mnemon/文档无既有"元素相克"设计 → greenfield；Hindsight 401 不可用。
- 参考实现：`E:\MXD\扩展改动\天气系统\weather-time-systems\optional\elemental-bonus\WeatherElementBonus.java`（98 行，单元素雷=雨天 1.10 的 worked example）+ 配套 patch/README。
- s9 服务端：OdinMS 重构（无 MobElemAttr 类，怪物元素=抗性表）。

## 1. 现有元素系统（行号坐实）
### 元素枚举
- `org/gms/server/life/Element.java:25`：`NEUTRAL(0), PHYSICAL(1), FIRE(2,true), ICE(3,true), LIGHTING(4), POISON(5), HOLY(6,true), DARKNESS(7)`（LIGHTING/DARKNESS 拼写）。
- `Element.java:43-61` getFromChar：F/I/L/S/H/D/P；`ElementalEffectiveness.java:25-40`：NORMAL/IMMUNE/STRONG/WEAK/NEUTRAL。
### 技能元素（权威=Skill.element，来自 WZ elemAttr）
- `client/Skill.java:33` 字段 element；`:65-71` getter/setter。
- `client/SkillFactory.java:117-126` loadFromData 读 WZ `elemAttr` → `Element.getFromChar(elem.charAt(0))`，无则 NEUTRAL。
- 带元素技能：stock v83 仅冰雷三招（2201005/2211003/2221006）；蓄力（WK/Paladin 火冰雷圣）走 BuffStat 单独分支。
### 怪物元素（不是"自身属性"，是抗性表）
- `server/life/MonsterStats.java:47` `Map<Element, ElementalEffectiveness> resistance`；`:189-200` set/get（缺省 NORMAL）。
- `Monster.java:1115-1135` getElementalEffectiveness/getMonsterEffectiveness；`:1708-1729` setTempEffectiveness。
- 来源 WZ：`LifeFactory.java:455` 读 `elemAttr`，`:589-593` 每 2 字符解析（字母+数字）。
### 伤害链路（关键：区分"上限"与"真实伤害"）
- `AbstractDealDamageHandler.java:979-993`：元素克制只改 `calcDmgMax`（反外挂上限）：WEAK ×1.5（:984），STRONG ×0.5 被注释（:986）。
- **真实伤害是客户端上报**：`:1027` readDamageLine → `:1096` 写入 ret.allDamage → `applyAttack :347-356` 聚合 → `:606` `map.damageMonster(player,monster,totDamageToOneMonster)`。
- FIX_DAMAGE autoban 在 `:531-532`，任何缩放必须在其下游。
- 怪物打玩家：`TakeDamageHandler.java:73` 读客户端 damage → `:280-310` 减伤 → `:309` addMPHP(-damage) → `:313` 广播。**玩家受到的伤害也是客户端上报**，服务端不能凭空改（改服务端 damage 只影响 HP 真实扣除与广播显示不一致）。

## 2. 参考示例（WeatherElementBonus.java）的三条铁律
1. 按 `Skill.getElement()` 过滤（WZ 权威），不硬编码技能 id 列表（`:14-24`）。
2. 必须 `WeatherMapVisibility.hasVisibleSky(mapId)` 门禁（`:82`）——玩家看不见的天气加成=bug。
3. 必须 `WeatherService.skyForMap(mapId)` 而非 `currentSky()`（`:40-45,85`）——天气是区域性的。
4. 缩放在 `:606` 之前、按目标聚合（非按行），且 `Math.round` 后保底 ≥1（0=MISS，`:91-96`）。
5. 建议起始表（README:80-93，平衡建议非代码）：雷=雨/暴风、冰=雪/暴雪、火/圣=晴朗白天、毒=阴/沙尘、暗=夜。

## 3. 设计草案（三张 DB 表 + 三层代码 + Web 页）
### 3.1 表（照 witch 范式，V1.x migration，带 enabled/remark/create_time/update_time）
1. `weather_element_map`（天气→元素增强矩阵）：sky_id(0..8) + element(0..7) + multiplier(默认1.0) + enabled → "下雨天雷系技能 ×1.15"。唯一键 (sky_id, element)。
2. `weather_monster_element`（怪物"自身元素"判定）：map_id 或 monster_id + own_element + enabled → 怪物是火/冰/…；**因为服务端没有该字段**，二选一：
   - A：从 `resistance` 推导（STRONG/IMMUNE 的元素视为"自身属性"），零新字段但与原生 WZ 抗性语义耦合；
   - B：新表显式声明（更可控，需配置每条怪物/地图）。← 需用户拍板。
3. `weather_element_counter`（同类天气下的相克规则，可选）：own_element + counter_element + multiplier → "火怪被冰系 ×1.3、被水系 ×0.8"。若只需"增强/削弱"两档，可合并进表1。

### 3.2 后端三层（照 WitchConfigManager 骨架）
- `WeatherElementConfigManager`（volatile 不可变快照 + synchronized reload + 静态读取 API）。
- `WeatherElementConfigService`（CRUD + reload）+ `WeatherElementController`（`/weatherElement/v1/**`）。
- 读取门禁统一：`WeatherService.skyForMap(mapId)` + `WeatherMapVisibility.hasVisibleSky(mapId)` + `skill.getElement()` ≠ NEUTRAL。

### 3.3 伤害挂钩（两种深度，需用户拍板）
- **方案甲（保守·服务端自洽）**：只缩放 `calcDmgMax` 校验上限（`:979-993` 扩展）——不改真实伤害，玩家看不出增强，但绝不会误封。
- **方案乙（推荐·真实生效）**：在 `:606` 前对 `totDamageToOneMonster` 乘倍率（同步抬高 autoban 上限防误封）+ 广播包 `:603` 用缩放后的值 → 玩家看到的数字与扣血一致。怪物打玩家在 `TakeDamageHandler :309` 前乘倍率，并同步 `:313` 广播值。
- 两类都要保底 ≥1、上限 Integer.MAX_VALUE；按目标聚合不按行。

### 3.4 客户端（需不需要改？）
- 若方案乙：伤害数字由服务端广播包带（`PacketCreator.damageMonster/damagePlayer` 的 damage 参数），**客户端无需改**即可显示新数值。
- 可选增强（需用户拍板）：右下/天气 HUD 加当前天气的元素提示图标（如"雨天·雷+15%"），可在 `UIBuffList` 时间 HUD 旁加一行文本（复用 A11M）；不改 WZ。

### 3.5 快速切换改持久 DB（需求①）
- 复用 `weather_override` 表（不新建）：`WeatherAdminService.apply()` 末尾把语义翻译成 `weatherConfigService.setOverride(profileId=skyId, timeMinute=minute, bareSky, holdMs, remark="web quick apply")`；auto/release 分支改调 `clearOverride()`。
- "永不过期"语义缺口：`weather_override` 靠 `expire_at`（now+holdMs）自愈，没有"永久"表达 → 需决定 holdMs 默认值（1h/24h/自定义）或扩字段 `permanent`。← 需用户拍板。

## 4. 待用户拍板（6 点）
1. 怪物"自身元素"来源：A 从抗性推导 / B 新表显式声明（推荐 B）？
2. 伤害生效深度：甲只改上限（保守）/ 乙真实缩放（推荐，需同步广播包）？
3. 增强/削弱模型：单一天气→单一元素倍率（表1），还是再加"属性相克矩阵"（表3）？配默认倍率多少（建议 1.15/0.85，克制 1.3/0.7）？
4. 客户端提示：要不要在 HUD 加"天气·元素加成"文本？
5. 快速切换持久化的"持续时间"：1h / 24h / 永久（扩字段）？
6. 作用范围门禁：只在室外有天空地图生效（推荐，对齐参考实现）/ 全地图生效？

## 5. 不做（默认）
- 不改 WZ 里怪物/技能的 elemAttr（数据源保持 WZ 权威）。
- 不碰数据库写操作（实现时由用户按 dbx 规范授权；本轮纯设计）。
- 不动客户端二进制（除非决策4要 HUD 提示）。

## 6. 闭环边界
- 实现后需 mvn package + vite build + 真机（本端）验证伤害数字与 HP 扣减一致、autoban 不误封；服务端重启由用户执行。
