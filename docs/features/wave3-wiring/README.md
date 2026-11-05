# Wave 3 · A 类接线 + Wave 2a 召唤袋（2026-11-04）

> 构建：全量 Rebuild EXITCODE=0（exe 19:54）；矩阵 c2s MATCHED 99 → **109**、`client enums never constructed = 0`；
> 几何对拍：`tools/buff_hit_layout_check.py` → OK（1/2/5/15 格全边界）。

## 接线清单（每条 = s9 逐字段对账 + 消费点 + 探针）

| # | 功能 | opcode | s9 证据 | 消费点 | 判据 |
| --- | --- | --- | --- | --- | --- |
| 3.1 | Buff 右键取消 | CANCEL_BUFF(92) c2s，body=int sourceid | CancelBuffHandler（白名单走专用、default cancelEffect(skill(sourceid))） | `UIBuffList::rightclick`（新增；连基类矩形命中缺陷一起修：dimension=(x,32) 使条带左段永远打不中→右键全落 ContextMenu） | `[PERF] BUFF cancel request skillid=` |
| 3.2 | AP 一键自动分配 | AUTO_DISTRIBUTE_AP(88)，body=ts(4B)+4B pad+2×{int type,int val} | AssignAPProcessor 标准分支（DB 种子 `use_server_auto_assigner=false`）；type=Stat.getBy5ByteEncoding STR64/DEX128/INT256/LUK512 | `UIStatsInfo` BT_AUTO YesNo：旧 **N 次 SPEND_AP 循环 → 1 包**；第二对合法类型码+0 值（gainStatByType `gain<=0` 早退，switch 不求值 null） | `[PERF] AP autoassign primary=…` |
| 3.3 | 技能书使用 | USE_SKILL_BOOK(82)（body=USE_ITEM 同形） | SkillBookHandler:47-49 `readInt/short slot/int`；通用 UseItemHandler **不查 teach 表**（发 72 学不进技能）；243 前缀门=ItemInformationProvider:1596 同规则 | `UseItemPacket` 构造内分流（7 个调用点全收敛：背包双击/快捷格/聊天命令/壳窗） | 端到端点测（用户） |
| 2a | 召唤袋刷怪 | USE_SUMMON_BAG(75)（body 同形） | UseSummonBagHandler：扣 1 个→`getSummonMobs`（ItemData info/mob 子表，s9 无前缀规则、2022 useType 是即时传送段不可混用） | 同 `UseItemPacket` 分流；`ItemData` 构造期解析 `summonbag=src["mob"]` 位 | 端到端点测（用户） |

**设计收敛点**：消耗品使用路由（普通/技能书/召唤袋）**全部下沉在 `UseItemPacket` 构造**，不在调用点散传标志位——新增调用点零成本继承（第一版曾把 summonbag 参数摊给背包单点，自查后推翻重做）。

## 台账纠偏（本批复核，parity-gap §1.2-A 的实账）

| gap 条目 | 复核结论 |
| --- | --- |
| `USE_RETURN_SCROLL(85)` | **假缺口**：s9 注册的就是 UseItemHandler（body 与 USE_ITEM 同形），本端 USE_ITEM 通道早已覆盖 |
| `CANCEL_DEBUFF(99)` | **假缺口**：s9 handler 全注释（类名带 "TIP: BAD STUFF LOL!"）；真通道=USE_ITEM 药剂 dispel（AllCure/Eyedrop/Tonic/HolyWater，UseItemHandler:37-52 坐实），本端发 72 已通 |
| `USE_ITEM_REWARD(112)` | **依赖未建**：掉落物 name tag 右键菜单不存在，单独接包无意义 |
| `TOUCH_MONSTER_ATTACK(47)` | **不做**：s9 仅在 `energyBar==15000 || BODY_PRESSURE` buff 时结算（身体压力技能）；本端无该技能数据 → 发了也被丢 |
| `HEAL_OVER_TIME(59)` | **需先补功能**：s9 是"客户端自然回血回报加 MP"；本端无 HP/MP 自然回复模型（grep regen 零命中），先建回复模型才有回报对象 |

## 触碰文件

`IO/UITypes/UIBuffList.{h,cpp}`（is_in_range/rightclick/draw_order 快照）、`IO/UITypes/UIStatsInfo.cpp`、`IO/UITypes/UIItemInventory.cpp`（回归单参调用）、`Net/OutPacket.h`（4 枚举）、`Net/Packets/InventoryPackets.h`（路由下沉）、`Net/Packets/CommonPackets.h`（CancelBuffPacket）、`Net/Packets/PlayerPackets.h`（AutoAssignApPacket）、`Data/ItemData.{h,cpp}`（summonbag 位）、`tools/buff_hit_layout_check.py`。

> 头文件布局变更（UIBuffList/ItemData 加成员）→ 已按 stale-obj 教训走全量 Rebuild。真机点测：右键点增益图标、能力窗一键 AP、背包双击技能书/召唤袋。
