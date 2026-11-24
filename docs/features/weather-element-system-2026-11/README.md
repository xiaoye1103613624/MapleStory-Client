# 天气 × 元素属性系统 — 最终实现交付（2026-11-23）

状态：P1-P6 全部实现、三端构建通过、客户端已部署 live（SHA 6B5CA7C6 稳定）。
**服务端未重启、DB migration 未执行、真机未验收** —— 见文末诚实边界。

## 决策（用户拍板）
1. 怪物自身元素 = 从已同步抗性是 IMMUNE/STRONG 推导（1A），不建新表、不导资源。
2. 伤害真实缩放 + 同步广播（乙）。
3. 倍率按建议：同类增强 1.15 / 削弱 0.85（DB 可配 0.5-2.0）。
4. 客户端 HUD 加天气元素提示。
5. 快速切换 = 重置为自然（清冻结 + 清 DB override，时间/天气从当前自然推进）。
6. 仅室外有天空地图。

## P1 DB migration
`V1.11.19__create_weather_element_rule.sql`：表 `weather_element_rule`（sky_id 0..8 + 9=夜晚, element 0..7, scope 0=技能/1=怪物承受/2=怪物攻击, multiplier, enabled, UK(sky,element,scope)）+ 幂等种子（雷雨/冰雪/火晴/圣晴/毒阴沙尘/暗夜）。
**只写文件，未执行 DDL**：由服务端重启时 Flyway 自动建表（与 witch DB 化同流程），符合"无同意禁写库"。

## P2 后端配置层
- `dao/entity/WeatherElementRuleDO.java` + `dao/mapper/WeatherElementRuleMapper.java`
- `server/weather/WeatherElementManager.java`：volatile 不可变快照 + synchronized reload（ServerManager.getApplicationContext().getBean 取 mapper）；静态 API `multiplierForMap(mapId, element, scope)` = skyForMap 倍率 × 夜晚倍率，夹 [0.5,2.0]；未配/未加载=1.0。
- `service/WeatherElementService.java`：CRUD + @Transactional + reload，boundary 校验 (sky 0..9, element 0..7, scope 0..2, multiplier 0.5..2.0)。
- `controller/WeatherElementController.java`：`/weatherElement/v1/{list,save,delete,reload}`。
- `@PostConstruct` reload，表不存在时静默用默认不阻断启动。

## P3 伤害真实缩放（关键：显示与扣血同源）
- `server/weather/WeatherElementDamage.java`：
  - `scaleFactor(skillElement,mapId,visibleSky,scope)`：NEUTRAL/无天空→1.0。
  - `monsterInMultiplier / monsterOutMultiplier`：按怪物自身元素（抗性 IMMUNE/STRONG 推导，取枚举序最小者）。
  - `scaleValue`：保底≥1（0=MISS）、保留负数暴击标志、夹 int 上限。
- **玩家→怪挂钩**：`AbstractDealDamageHandler.java` applyAttack 中，免疫/dojo 夹断后、聚合前（原 :345）缩放 `onedList`，并 `attack.allDamage.put` 回写 → 聚合扣血（map.damageMonster）与攻击封包回显（第三方看到的数字）**同源一致**；两轴倍率（技能元素×怪物承受）乘积夹 [0.5,2.0]；FIX_DAMAGE(:531) 在其下游、固定伤害技能元素=NEUTRAL 不受影响；parseDamage 的 autoban 校验读的是缩放前客户端值，不误封。
- **怪→玩家挂钩**：`TakeDamageHandler.java`（原 :225 前）按 attacker 自身元素缩放 `damage` → 防御减伤、扣血(:309)、广播包(:313) 同源。
- 室外门禁 `WeatherMapVisibility.hasVisibleSky(mapId)`。

## P4 快速切换改重置语义
`WeatherAdminService.apply()`：清 `clearTimeOverride()` + `clearSkyOverride()` + `weatherConfigService.clearOverride()`（含 reload 重发包）+ 黄字提示。时间从当前墙钟继续走、天气按 ROLL_INTERVAL_MS 自然变。UI 文案改"重置为自然"。
解除循环依赖：`WeatherConfigService.state()` 的等价指令改为内联拼装（不再注入 WeatherAdminService）。

## P5 前端
- `api/weatherElement.ts`（list/save/delete/reload + 类型）
- `views/game/weatherElement/index.vue`（规则表 CRUD + reload + 双轴下拉）+ `locale/{zh-CN,en-US}.ts`
- 路由 `game.ts` 加 weatherElement；全局 locale `{zh-CN,en-US}.ts` import + spread。
- quick tab 文案改"重置为自然"（zh-CN/en-US）。

## P6 客户端 HUD
`UIBuffList.h/.cpp`：新增 `elementhint_label/outline`（A11M）+ `elementhint_shown_key`。
`update()` 按 `State::sky()` 本地映射（雨/暴风→雷、雪/暴雪→冰、晴→火/圣、阴/沙尘→毒、19:00-04:00→暗）生成"天气·XX 增强"，只显示元素名不显示倍率（倍率服务端权威且可配）；`draw()` 在时间大字下方 y=position.y+52 画 1px 描边文本。无 0x373D 隐藏。不改协议、不改 WZ。

## 构建 / 部署证据（实测）
- 后端 `mvn -q -DskipTests compile` → EXIT=0（中途修过 2 个编译错误：ResultBody.success(Map,Map) 无重载→单参；clearOverride 静态误用→实例调用）。
- 前端 `vue-tsc` 天气模块 0 错（仅既有 file/index.vue 3 错）；`vite build` EXIT=0。
- 客户端 MSBuild v142 / Rebuild / m:1 → BUILD_EXITCODE=0，日志 0 error。
- 产物 `build-weather-element-2026-11/MapleStory.exe` SHA `6B5CA7C692749E8C5D90E9D1D5142C55BAEFDFA9D9E9D3B3DE11942C5AF4DDD8`。
- live 部署：无占用进程；备份 `MapleStory.exe.bak-before-weatherelement-20261123`（旧 SHA A326CBC9）；覆盖后立即+8s 均 `6B5CA7C6…DDD8`，无回滚。

## 诚实边界（未验收）
- **服务端未重启**：Flyway V1.11.19 未执行、新接口/伤害挂钩未生效；需用户重启（IDE target/classes 或常规方式）。重启前 web 新页调用会 404、伤害不变。
- **真机未验收**：雨天雷系伤害×1.15 数字与 HP 扣减一致、autoban 不误报、怪物受击显示一致、HUD 提示正确、重置后时间自然走/天气自然 roll——均需用户本端截图/录屏验证，截图必须本端客户区。
- DB 写操作：本轮只写 migration 文件，未连库未执行 DDL/DML（符合门禁）。
- 客户端 HUD 元素映射是本地静态表（与种子一致），若 DB 改了映射规则，HUD 文本不会自动跟着变（只显示元素名不显示倍率，故影响有限）。
- 未提交 git（用户未要求）。
