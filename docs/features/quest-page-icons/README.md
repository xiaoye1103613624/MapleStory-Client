# C8 任务页码图标（连锁任务 Gauge spot）

> 状态：代码已闭环；真机 → pending-live-tests  
> 承接：`ui-v83-window-restore/notes/open-issues.md` §C8  
> 日期：2026-11-05  
> 产物：`docs/features/quest-page-icons/`

## 1. 选型

本轮优先尝试 **BossHP 多目标队列**（IDA 深读 DualMobGauge），证据不足后转 **C8**。

| 候选 | 结论 |
| --- | --- |
| DualMobGauge `sub_56C9B7` | **不是**多目标队列：PartyRace 专用 ctor，硬编码 `Mob/9700036`，仅数据 xref；FIELD_EFFECT mode5 走 `MobGage` `sub_5336CA` |
| MobGage 多 mob 缓存 | 既有 RE 结论仍成立：客户端无字面量 mob 上限；s9 `announceBossHpBar` 已 5s 单目标节流 |
| **C8 页码图标** | 证据充分：连锁任务 Gauge spot + `"%s ( %d / %d )"` |

## 2. 证据

| 源 | 结论 |
| --- | --- |
| IDA `sub_83EA05` | `total>1` 才画 Gauge；按页界写 spot；fill 跟当前 order |
| IDA `sub_83FEC6` | 悬停格式 `"%s ( %d / %d )"`；pool **4497** = `Chain Quest` |
| QuestInfo.img | `parent`（链名）+ `order`（1-based）；`0/1/2` 为**状态摘要页**非长文翻页 |
| NX `UIWindow.img/Quest/Gauge/{frame,gauge,spot}` | 页码视觉载体（非 QuestIcon 奖励条） |
| QuestIcon/3..9 | 奖励区 `#f` 横幅（选择道具/金币/经验…），属 NPC 对话令牌，非本窗页码 |

**【已证实】** C8「第 N / M 页」= 连锁任务进度条上的 **spot 刻度** + `连锁任务 ( n / m )` 标签。  
**【差异】** 修前把 QuestInfo 0/1/2 拼成一段 summary，且 Gauge 只服务需求进度、不认 parent/order。  
**【推荐】** `QuestData` 保留分页 + 链字段；`UIQuestLog` 优先画连锁页码条。无待决项。

## 3. 实现

- `QuestData`：`pages[]`、`chain_parent`/`chain_order`；`get_summary_for_tab`；懒建 parent→total 索引
- `UIQuestLog::draw_chain_page_gauge`：frame + 按 order/total 填充 + 页界 spot + 宋体标签
- `draw_gauge`：连锁优先，否则原需求进度
- 简述态按 tab 取对应状态页；切 tab 刷新详情
- i18n：`quest.chain_page`

## 4. 验证

- 构建：见 `builds/`
- 真机（本轮不跑）：打开「瑞恩的冒险岛问答1/2」等连锁任务，确认 Gauge 有分段 spot 与 `( n / m )`；三分类页摘要不串页

## 5. BossHP 预留

多目标队列本轮**不实现**：DualMobGauge 非状态机；原生 MobGage 为单槽按 mobId 换皮。服务端已有切换节流。后续若要做双槽 UI 需另取证布局。

## 6. 触碰

客户端 `QuestData` / `UIQuestLog` + locale + 本文档 / open-issues；未动服务端。
