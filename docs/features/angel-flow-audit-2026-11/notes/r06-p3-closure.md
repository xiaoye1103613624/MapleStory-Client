# r06 · P3 完整闭环笔记（2026-11-12）

## 1. 自动指引 idx

| 步骤 | EA / 证据 | 结论 |
| --- | --- | --- |
| 标签分类 | `sub_9A2DE0 @0x9a2de0` | `#E→2 #I→3 #S→4 #K→5`（大写） |
| CT 写入 | `AnalyzeText` case 2..5 @0x99e4f1..528 | type=3，`+0x34`/`v62[13]` = 0/1/2/3 |
| 消费 | `CUtilDlgEx::Draw` @0x9a4241..56 | `mov esi,[esi+34h]; cmp 0..3; SetButtonBlink(esi,1)` |
| 本端 | `apply_guide_blinks_from_text` | 扫原文大写四标签；小写留给色/图标令牌 |
| 降级保留 | style>>8 ∈[0,3] | 次要回退，不发明默认窗 |

**未接**：NPC/传送点/世界坐标「手指精灵」对象 — 本端无等价 CT 画布对象；功能闭环落在 KEY_ANI 四主窗，不伪接地图实体。

## 2. NoticeQuestProgress

- orange-wz 128B `UIWindow.img`：`QuestAlarm` ✅；`NoticeQuestProgress` / Progress 关键字 ❌
- 替代：`UIQuestAlarm::push_progress`（P2 已接线 MessagingHandlers）标为完整替代 + 等价边界表（P3.md）

## 3. 图鉴分类

- `LoadCard`：`*(card+8) = (itemId-2380000)/1000`；挂 `v22[v11+1]`
- `String/MonsterBook.img`：mob 文案，非分类表
- 代码：去掉 mobId 近似，改 `suffix/1000` 钳 [0,8]

## 4. Ignore

- s9 `BuddylistModifyHandler`：mode 1 add / 2 accept / 3 delete only
- 正式 ⊘；UI FindPlayer 代理保留并注释收口

## 5. 证据文件

- `evidence/p3-ida-guide*.txt`、`p3-ida-tagmap.txt`、`p3-ida-ctinfo.txt`
- IMG：本轮 `QuestAlarm` / `Notice*` / `MonsterBook.img` 查询（128B key）
