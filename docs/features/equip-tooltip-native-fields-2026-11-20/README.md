# 装备 tooltip：Angel 原生词条全景 / 强化标识 / 本端缺口（2026-11-20）

> 个人学习/技术研究，单机离线。逐条行号级结论；真机未验收如实声明；参考端截图不作本端验收凭据（USER.md）。

## 0. 版本前提（最关键，先厘清）

用户问"原生 angel 装备 tooltip 支持的所有属性和词条、强化标识也有吗"——**强化标识（星之力/火花/宝珠/潜能/附加潜能）是 095 越版特性，angel v83 原生没有**。

- 历史 IDA 取证（13337 = Angel.exe 具名 IDB）：`func_query *Potential*` / `*StarForce*` 空；交易仅 `IsTradeBlockItem@0x5d4db9` 读 `tradeBlock` int32 布尔，无计数函数。来源：`docs/features/equip-tooltip-3issues-2026-11/tooltip-fields-gap-analysis.md` §2。
- **本轮 IDA 未复核**：13337/13338 端点均未连通（404/超时），本机未找到 Angel.exe/i64/idb。故 IDA 部分为历史证据，本轮未一手复验，如实标注。
- 本轮一手证据：orange-wz MCP（10012，在线）128B key 读 live `Data/UI/UIWindow.img`。

## 1. Angel v83 原生支持的属性与词条（历史 IDA + 079 参照帧）

| 类别 | 词条 | 本端代码位置 | 数据源 | 原生性 |
| --- | --- | --- | --- | --- |
| 属性 15 项 | STR/DEX/INT/LUK/MaxHP/MaxMP/Attack Power/Magic Attack/Defense/Magic Defense/ACCURACY/AVOID/HANDS/Speed/Jump | `EquipStat.h:28-43` 枚举 15 槽；`EquipStat.cpp:34-54` 名称表；`EquipTooltip.cpp:1029-1043`(主)/`:751-774`(对比) | `get_stat(es)`；属性名位图 `ToolTip/Equip/Property/0..18`（本轮 orange-wz 实读 19 张 CANVAS） | ✅ angel |
| 需求块 | 等级/STR/DEX/INT/LUK/人气(Can 绿/Cannot 红) | `EquipTooltip.cpp:257-286` 位图映射、`:281-286` 行距 12px | `get_reqstat`；`reqLEV/reqSTR/.../reqPOP` | ✅ angel |
| 职业行 | 6 职业可用白/不可用红 | `draw_job_line`；`EquipTooltip.h:55` | `okjobs`；`Can/{6}`/`Cannot/{6}` 位图 | ✅ angel |
| 攻击速度 | FAST(1-4)/NORMAL(5-6)/SLOW(7-9) | `EquipTooltip.cpp:1002` | `WeaponData::getspeedstring` | ✅ angel |
| 可升级次数 | slots | `EquipTooltip.cpp:1009-1013` | `get_slots()` | ✅ angel |
| 金锤子 | vicious | `EquipTooltip.cpp:1018-1020` | `get_vicious()` | ✅ angel |
| 底部描述 | desc（居中 A12M） | `EquipTooltip.cpp:885-891`/`:1343-1347` | `ItemData.cpp:88`；angel `GetItemDesc@0x5cf69e`→String.wz desc | ✅ angel |
| 交易限制 | 唯一/不可交易（橙字，名称下方） | `EquipTooltip.cpp:293-302`/`:900-905` | `tradeBlock`/`only` 布尔 | ✅ angel |
| 名称 | quality 着色（灰/橙/蓝/紫/金） | `EquipTooltip.cpp:835-849` | `get_quality()` | ✅ angel |
| 道具 ID / 装备分类 | 元信息行 | `EquipTooltip.cpp:972-973` | itemid / `equipdata.get_type()` | ⚠️ 本端增强（angel 无独立 ID 行的强证据，属本端构型） |
| 成长两行 | 道具等级/经验 | `EquipTooltip.cpp` show_growth 分支 | `itemlevel/itemexp` | ⚠️ 本端/095 构型 |

## 2. 强化标识：angel 无，095 越版；本端已实现

| 强化项 | 本端实现 | 数据链路 | 原生性 |
| --- | --- | --- | --- |
| 星之力（文字行） | `EquipTooltip.cpp:1124-1137` `star_line`"星之力 N ***" YELLOW | `ItemParser.cpp:142-157` 读 spark(2B)+soul(2B)+star_force(1B)；`Equip.h:68 get_starforce()` | ❌ angel 无 / 095 有 |
| 星之力（名称上方图形星） | `EquipTooltip.cpp:1214-1240 draw_stars` 黄/灰 10 格，`MemoryTexture star_yellow/star_gray`（`Graphics/EquipStarPixels.h` 像素常量），`:1289`/`:1524` 主窗+对比窗调用 | 同上 | ❌ angel 无 / 095 有（095 `UIWindow.img/ToolTip/Equip/Star`=13×13 黄星、Star2=26×23 桂冠星） |
| 潜能（3 主 + 2 附加） | `EquipTooltip.cpp:1053-1094`，白字 `(B)` 前缀，locale `equip.potline.*` | `get_potential(0..2)`/`get_bonus_potential(0..1)`；`Equip.h:59-60` | ❌ angel 无 / 095 有 |
| 火花 | `EquipTooltip.cpp:1096-1108` | `Equip.h:64 get_spark()` | ❌ angel 无 / 095 有 |
| 宝珠 | `EquipTooltip.cpp:1110-1122` | `Equip.h:66 get_soul()` | ❌ angel 无 / 095 有 |
| 对比窗镜像 | 上述三件套 preview 侧 `EquipTooltip.cpp:1668+`、`:735-743` | 已装备 equip 数据源 | ❌ 越版增强 |

**关键结论**：用户问"强化标识也有吗"——若指 angel 原生：**没有**；若指当前本端客户端：**已经有，且是代码层完整实现**（星星、火花、宝珠、潜能三主两附、对比窗镜像全在）。

## 3. 本端真正未实现 / 缺口（逐条）

| # | 缺口 | 证据（行号级） | 性质 | 真机验收 |
| --- | --- | --- | --- | --- |
| 1 | 潜能 id→完整文案表缺失 | locale `equip.potline.1-12` 仅 12 条占位；`EquipTooltip.cpp:1057/:1092` 缺键回退"潜能N"；live UIWindow.img 无 Potential 资源 | 095 文案表未移植，非"没画" | 未验收 |
| 2 | 火花/宝珠文案表只有占位 | locale `equip.spark.1-10`/`equip.soul.1-5` | 同上 | 未验收 |
| 3 | spark/soul/starForce 数据可能恒 0 | `ItemParser.cpp:150` 按 `recv.length()>=5` 门控读取；若 s9 `PacketCreator` 未写这 5B 则全 0 —— **代码已实现，数据看服务端** | 非缺代码 | 未验收 |
| 4 | 名称上方星星的"真机观感" | `draw_stars:1214-1240` 已实现 | 历史文档 `tooltip-fields-gap-analysis.md` §5 前版本记为"P0 未实现"，**当前代码已落地，记忆过时，以代码为准** | 未真机截图 |
| 5 | %属性（所有属性+3%） | `EquipStat.h` 仅 15 基础属性、无 Rate 字段；来自潜能选项文本 | **不是缺口**，禁止给 EquipStat 加百分比字段 | — |
| 6 | quality 副标题（SS级/传授物品）、可交易次数计数 | 参考端截图样式；angel/095 原生走 desc 文本，无独立字段 | **不是 angel 原生缺口** | — |
| 7 | 底部 desc 的 `#c…#` 颜色宏 | `Text()` 是否解析宏未确认，按字面会露 `#c` | 优化项非缺口 | 未验收 |

## 4. 一手 IMG 证据（orange-wz，2026-11-20）
- 本端 live `Data/UI/UIWindow.img` `ToolTip/Equip` 仅 11 子节点：`Can/Cannot/Dot/WeaponCategory/ItemCategory/Speed/Property/mesos/cash/GrowthEnabled/GrowthDisabled`；**无 Star/Star2/AlienStone**（本端星星走像素常量 + MemoryTexture，不需 img 节点）。
- `ToolTip/Equip/Property/0..18` = 19 张 CANVAS（属性名位图槽）。
- 079 原生 `20dalu_079MS/客户端/Data/UI/UIWindow.img` 用 128B key 解析报 `IMG 解析失败` → 按 AGENTS.md §2 回退，未取到原生节点树，记为取证受限。

## 5. 如实声明
- 本轮 **IDA 未复核**（端点不通、无 IDB）；angel 原生结论引用历史 IDA 证据，非本轮一手。
- 所有"未实现/缺口"结论均为**源码层 + IMG 节点层**，**未真机截图验收**；按 USER.md，任何截图须先确认来源端（本端 vs 参考端）方可作验收凭据。

## 6. 相关
- `docs/features/equip-tooltip-3issues-2026-11/tooltip-fields-gap-analysis.md`（095↔angel↔本端三方对照原始调查）
- `docs/features/equip-tooltip-3fixes-2026-11-18/README.md`（词条行距/颜色/名称资源三问题修复）
