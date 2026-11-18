# 装备 tooltip 词条全景盘点与原生缺口（2026-11-18）

> 个人学习/技术研究，单机离线。逐条行号级结论；参考端截图只作"目标样式"，不作本端验收凭据（USER.md 教训）。

## 0. 背景

用户提供参考端截图（百度水印、英文 REQ LEV/REQ STR、名称上方整排星星、SS级（传授物品）、不可交换、可交易1次、%属性行、潜在属性/附加潜能、底部描述），问：潜能要不要参照、底部描述有没有、剪刀后可交易次数怎么显示、angel/095 有无类似逻辑、本端 tooltip 有哪些词条没实现、后期升星星星格式。

## 1. 本端词条全景（EquipTooltip.cpp/h 现状，逐条行号）

| 词条 | 本端是否有 | 代码位置 | 颜色/样式 | 数据源 |
| --- | --- | --- | --- | --- |
| 名称 | ✅ | cpp:878 name=A14B/CENTER | quality 着色 | itemdata.get_name() |
| 道具ID行 | ✅ | cpp:891 | WHITE | itemid |
| 装备分类 | ✅ | cpp:892 | WHITE | equipdata.get_type() |
| 攻击速度 | ✅ | cpp:921 | WHITE | WeaponData |
| 需求块(等级/STR/DEX/INT/LUK/人气) | ✅ | cpp:371-379 等 | Can/Cannot 绿/红位图 | get_reqstat |
| 成长两行(道具等级/经验) | ✅(条件) | cpp show_growth | 位图 | itemlevel/itemexp |
| 职业行(6职业) | ✅ | draw_job_line | 白/红双态 | okjobs |
| 属性行 STR..JUMP(15项) | ✅ | cpp:952-972 | WHITE + 卷轴括号 | get_stat(es) |
| 差值(悬停-已装备) | ✅ | cpp:707 statdeltas_preview | 正GREEN/负RED | hovered-equipped |
| 可升级次数 | ✅ | cpp:928-940 slots | WHITE | get_slots |
| 金锤子 | ✅ | cpp:939 hammers | WHITE | get_vicious |
| 潜能行(3主+2附加) | ✅ | cpp:984-1009 potlines | WHITE, (B)前缀 | get_potential/get_bonus_potential → locale equip.potline.* |
| 火花/宝珠/星之力 | ⚠️代码有数据无 | cpp:1019-1059 | spark/soul WHITE, star YELLOW | 服务端零赋值恒0 |
| 交易限制(唯一/不可交易) | ✅ | cpp:293-302 trade_line | ORANGE 名称下方 | is_unique/is_untradable |
| owner 所有者 | ✅ | cpp:904-911 owner_line | ORANGE 名称下方 | get_owner |
| 底部描述 desc | ✅ | cpp:885-891/1343-1347 | A12M/CENTER/TOOLTIPBODY | ItemData.cpp:88 desc |
| 名称上方星星 | ❌ | 无 | — | — |
| quality 副标题(SS级/传授物品) | ❌ | 仅 get_quality 用于名称着色 cpp:835-849 | — | — |
| 可交易次数计数 | ❌ | 无 | — | 原生写 desc 文本 |
| %属性(所有属性+3%) | ❌ | EquipStat.h 无 Rate 字段 | — | 原生来自潜能选项文本 |

## 2. 原生证据（angel v83 / 095）

取证手段：IDA MCP 13337(Angel.exe 具名符号)、orange-wz MCP(128B key)、095 IDA(13338 受限)。

### 2.1 底部描述
- angel: SetToolTip_Equip@0x8e8252 → @0x8e8e75 GetItemDesc → StringPool("desc") → GetItemString(itemid,"desc")。有。
- 095: String.wz/Eqp.img/Eqp/&lt;cat&gt;/&lt;id&gt;/desc（含 #c 宏文本）。有。
- 结论：本端 ItemData.cpp:88/:259 + EquipTooltip.cpp:885-891/:1343-1347 已对齐。

### 2.2 剪刀/可交易次数
- angel: 仅 IsTradeBlockItem@0x5d4db9 读 tradeBlock int32 布尔，无计数函数（func_query *TradeBlock* 仅2个重载）。
- 095: 无独立 trade 字段，"可交易N次"写在 desc 文本（search_node "交易" 命中大量 Eqp.img desc 含 "1次"）。
- 结论：原生是"布尔标记 + desc 文本"，本端 trade_line + desc 已对齐，不要新增协议字段。

### 2.3 名称上方星星【最大缺口】
- angel v83: 无星之力系统（func_query *StarForce* 空）。
- 095: UI.wz/UIWindow.img/ToolTip/Equip/Star（LIST）含 Star=CANVAS 13×13 ARGB8888 黄实心五角星、Star2=CANVAS 26×23 桂冠星；按星数在名称上方画整排。本端 live UIWindow.img/ToolTip/Equip 只11子节点、无 Star/AlienStone。
- 本端现状: EquipTooltip.cpp:1047-1059 star_line="星之力 N ***" YELLOW，画在属性区底部 :1336-1338——与原生图形星不同。
- 结论：❌ 缺口。要参照须①从095拷Star/Star2 canvas到本端UIWindow.img；②名称上方按 get_starforce() 画N颗星；③决定底部star_line去留。

### 2.4 潜能/附加潜能格式
- angel: 无潜能系统（func_query *Potential* 空）。
- 095: 受限——String/Etc/Item.wz 无 Potential 文案节点（Etc.wz 仅 ItemPotLifeInfo.img），095 IDA VMP壳无法反编译绘制分支；"(B)"前缀/颜色无法原生确证。
- 本端: cpp:984-1009 白字 + (B)前缀 + locale equip.potline.*（缺键回退"潜能N"）。
- 结论：⚠️ 证据不足，本端为自洽实现，不要据此改写；像素级还原需095文案表或运行时抓图。

### 2.5 %属性（所有属性+3%）
- angel: Equip 无百分比字段。
- 095: PacketCreator.java:477-493 只写 STR..JUMP 固定 short；Equip.java:78-86 无 incAllStatRate；本端 ItemParser.cpp:145-157 只解析 spark/soul/starForce+5潜能。"所有属性+3%"来自潜能选项 id→文案，非数值字段。
- 本端: EquipStat.h 无 Rate 字段。
- 结论：✅ 不需加百分比字段；缺的是潜能 id→文案映射（与2.4同根因）。

## 3. 缺口排序

| 优先级 | 缺口 | 依据 | 建议 |
| --- | --- | --- | --- |
| P0 | 名称上方整排图形星 | 095 Star(13×13)/Star2(26×23) canvas | 拷资源+改绘制；本端包无此节点 |
| P1 | 潜能 id→文案表 | 095走选项文本，本端无Potential资源 | 补资源或locale映射 |
| P2 | (B)前缀/潜能颜色 | 原生证据不足 | 暂不改，待运行时抓图 |
| — | desc/trade/%字段 | 已对齐或本就不存在 | 不动 |

## 4. 取证受限
095 `开启游戏.vmp.exe`(13338) VMP壳静态只有nullsub/系统API，func_query *ToolTip* 空，潜能颜色/(B)前缀/星星绘制分支无法逐行确认，只能资源+参考图取证。

## 5. P0 星星实现（2026-11-18）

用户要求：名称上方一整排星，激活黄 / 未激活灰，图标可复用或绘制。

方案（零 WZ/IMG 改动）：
- 从 095 `UI.wz/UIWindow.img/ToolTip/Equip/Star/Star`（13×13 黄星 canvas）用 orange-wz 导出 PNG → 归档 `resources/star095_Star.png` / `star095_Star2.png`。
- 解码为 BGRA 像素常量 `Graphics/EquipStarPixels.h`（YELLOW 原色 / GRAY 灰度，保留 alpha，独立 atlas id 0x53544152/0x53544153）。
- `EquipTooltip` 新增两个 `MemoryTexture`（star_yellow/star_gray，走 GraphicsGL::ensure_raw/draw_raw，范式同 TooltipLattice），ctor 里 set；新增 `starforce`/`starforce_preview` 成员在 set_equip 缓存。
- 新增 `draw_stars()`：名称上方按 STAR_SLOTS=10 画一排，前 sf 颗黄、其余灰，sf=0 不画；在主窗与对比窗 name.draw 前调用（画在背板顶沿 pos.y-6）。
- vcxproj 加 `Graphics\EquipStarPixels.h` 项。
- 底部原 `star_line` 文字保留（不删，避免波及布局）。

顺带修复（非本任务、并发会话半成品）：`IO/UITypes/UICharInfo.h:231` `bool operator bool() const` 多余返回类型致 MSVC C2549，全量构建挂死；删去多余 bool（转换运算符无使用点，最小改动），使构建恢复。此为并发会话正在写的代码，已在交付中如实标注。

验证：`build_verify.bat` 全量构建（error C=0、EXITCODE=0 为准）。
