# 天气×元素系统 — 决策定稿 + 一次性实施 Plan（2026-11-23）

> 用户 2026-11-23 拍板：1A（从抗性推导，但先确认客户端资源）、2乙（真实缩放+同步广播）、3按建议、4加HUD提示、5快速切换=重置回自然推进（非持久化冻结）、6仅室外。
> 本文件是实施蓝图；Hindsight 401 阻塞，决策落本地 + Mnemon。

## 0. 用户第1点问题的证据回答（客户端怪物属性/抗性是否已同步？能否直接用客户端资源？）
**已同步，且服务端正是从同一份 WZ 资源读的。**
- `LifeFactory.java:455`：`decodeElementalString(stats, DataTool.getString("elemAttr", monsterInfoData, ""))` —— 服务端从 **Mob.wz 的 monsterInfo `elemAttr`** 读怪物抗性。
- `LifeFactory.java:589-593`：每 2 字符一组，`Element.getFromChar(字母)` + `ElementalEffectiveness.getByNumber(数字)`（1=IMMUNE/2=STRONG/3=WEAK/4=NEUTRAL）。
- 落点 `MonsterStats.java:47` `Map<Element,ElementalEffectiveness> resistance`；运行时 `Monster.java:1115-1135` 可读（`getElementalEffectiveness`）。
- 技能侧同理：`SkillFactory.java:117-126` 从 Skill.wz 的 `elemAttr` 读技能元素 → `Skill.element`。
- 结论：**不需要再导入任何客户端资源，也不需要 new 属性字段**。怪物"自身元素"直接从已同步的 `resistance` 推导（用户选 1A）：
  - `IMMUNE`/`STRONG` 的元素 = 该怪物的"自身/抗性属性"（火怪对火 IMMUNE/STRONG）；
  - `WEAK` 的元素 = 该怪物的"相克属性"（火怪怕冰 WEAK）。
  - 天气规则即可表述为：某天气下，怪物对"自身元素"的抗性再增强（同类增强）、对"相克元素"的弱点再加深（相克削弱），双向都可配倍率。

## 1. 决策定稿
1. **怪物属性来源=A 抗性推导**：不建 `weather_monster_element` 表；`Monster.getElementalEffectiveness(e)` 为唯一判定源。
2. **伤害真实缩放+广播同步（乙）**：
   - 玩家→怪：缩放点 `AbstractDealDamageHandler.java:606` 前对 `totDamageToOneMonster` 乘倍率（在 FIX_DAMAGE :531-532 下游），同步改 `:603` 广播包 damage、抬高 autoban 上限（:1057 maxWithCrit 与 :1069 5x 阈值）。
   - 怪→玩家：`TakeDamageHandler.java:73` 读到 damage 后、`:309` addMPHP 前乘倍率，同步 `:313` 广播包 damage。
   - 保底 ≥1、上限 Integer.MAX_VALUE、按目标聚合不按逐行。
3. **按建议倍率**：同类增强 ×1.15、相克削弱 ×0.85；克制矩阵 ×1.3 / ×0.7（建议起始值，DB 可配）。
4. **HUD 提示**：`UIBuffList` 时间行下方加"天气·元素"文本（A11M，复用现有 Text），显示当前地图天气影响的元素与倍率（如"雨天·雷 +15%"）。
5. **快速切换=重置回自然推进（改变语义）**：
   - 不再写 `weather_override` 的持久行；`apply()` 的 time/sky 分支改为**重置**：`clearTimeOverride()`（时间从当前墙钟继续走）+ `clearSkyOverride()`（天空回到区域自动 roll）+ `clearOverride()`（清 DB 生效行），然后广播。
   - 语义变为"把世界交还自动"= 时间从当前时刻逐渐增加、天气按 ROLL_INTERVAL_MS 自然变化（:745 rollIfDue）。
   - 明确：这与原 GM `!weather night` 的"冻结时段"语义不同——用户要的是"重置/刷新为自然状态"，不是定时冻结。
   - midnight/clock/指定时刻等"冻结"选项保留在旧 override 页（DB 持久），quick tab 只做重置/增强/天气强制等即时语义（具体 UI 文字调整）。
6. **室外门禁**：`WeatherMapVisibility.hasVisibleSky(mapId)`（:44）+ `WeatherService.skyForMap(mapId)`（:511），不用 `currentSky()`。

## 2. DB 表（照 witch 范式，一张即可，因为怪物属性从抗性推导）
`V1.x__weather_element_config.sql`（带 enabled/remark/create_time/update_time，幂等种子）：
- `weather_element_rule`：`id, sky_id(0..8), element(0..7), multiplier(DECIMAL(4,2) 默认1.00), kind(TINYINT 0=同类增强 1=相克削弱), enabled, remark, create_time, update_time`；UK `(sky_id, element, kind)`。
- 种子：雷/雨1.15、冰/雪1.15、火/晴1.15、圣/晴1.15、毒/阴1.15、暗/夜1.15（同类）；对应相克削弱 0.85；克制矩阵条目 ×1.3/×0.7 由 kind 区分或另加列。
- 不建 monster_element 表（决策1A）。

## 3. 后端三层 + 挂钩
1. Migration + DO/Mapper（`WeatherElementRuleDO` / `WeatherElementRuleMapper`，MyBatis-Flex QueryWrapper）。
2. `WeatherElementConfigManager`：volatile 不可变快照 + synchronized reload + 静态 API `multiplierFor(byte skyId, Element own, Element counter, int kind)`。
3. `WeatherElementConfigService`：CRUD + reload；`WeatherElementController`：`/weatherElement/v1/**`（list/save/delete/reload）。
4. 伤害挂钩（`WeatherElementDamage` 工具类，静态）：
   - `scalePlayerDamage(skill, monster, damage, mapId)`：门禁→取天气→推怪物自身/相克元素→查表→缩放。挂 `AbstractDealDamageHandler:606` 前。
   - `scaleMonsterDamage(attacker, damage, mapId)`：挂 `TakeDamageHandler:73` 后。
5. 快速切换改语义：`WeatherAdminService.apply()` 三分支改写（time/sky/auto 全部→clear* + 广播）；controller 不变。

## 4. 前端
- 新增 `gms-ui/src/api/weatherElement.ts` + `views/game/weatherElement/index.vue`（规则表 CRUD + reload）+ 路由 `game.ts` + 双语 i18n。
- quick tab 文案调整为"重置为自然天气/时间"语义（决策5）。
- 客户端 HUD 提示（`UIBuffList.cpp` 时间行下方）：新增 `Text weatherelement_label`（A11M），从新包或现有天气状态取"天气+元素"文本——需服务端在 0x373D 增加字段或在 HUD 复用本地 State 的 sky + 元素表（推荐后者：客户端按 sky + 地图元素的本地映射显示，不改协议）。

## 5. 验证门禁
- 后端 `mvn package` EXIT0；前端 `vue-tsc` 天气模块 0 错、`vite build` EXIT0；客户端 MSBuild EXIT0 + 产物 SHA + 本端部署防回滚核对。
- 真机（本端截图）：雨天雷系伤害数字×1.15 且 HP 扣减一致、autoban 不误报、怪物打玩家受击显示一致、HUD 元素提示正确、快速切换后时间自然走动/天气自然 roll。
- DB 写操作：实现阶段由用户按 dbx 规范授权；本轮不连库不写数据。

## 6. 诚实边界
- 真机未验收；服务端重启由用户执行；未提交 git。
- "怪物自身元素=抗性 IMMUNE/STRONG"是推导语义，WZ 里没显式自身元素字段——已在文档标明，用户如需精确可调 rule 表 kind 区分。
