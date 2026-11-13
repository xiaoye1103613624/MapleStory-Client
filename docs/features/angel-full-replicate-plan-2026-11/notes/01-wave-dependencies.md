# 波次依赖与范围

## 依赖链（后波依赖前波基座）

```text
login → HUD → corewindow → inventory → skill → quest → npc
  → map → drop → social → pet → cashshop → activity → medal → gap
```

| 前波 | 后波为何依赖 |
|---|---|
| login | 进游戏前状态机；选世界/选角 |
| HUD | 快捷键、聊天、HP/MP/EXP 显示；多数窗从底栏打开 |
| corewindow | CWnd/拖拽/关闭/页签基座影响全 UI |
| inventory | 装备/道具 tip 被技能/任务/NPC 商店复用 |
| skill/quest/npc | 战斗与任务链进场前需稳定 |
| map/drop | 场地对象与掉落依赖进图 |
| social | 组队血条等挂 HUD |
| pet/cashshop/activity/medal | 相对独立，放后段降低冲突 |

## 每波触碰模块（预估）

| 波 | 模块 |
|---|---|
| 01 | `IO/UITypes/UILogin*` `UIWorldSelect` `UICharSelect` `Net/Handlers/Login*` `WaveAShellHandlers` |
| 02 | `UIStatusBar` `UIChatBar` `UIStateGame` |
| 03 | `IO/UIElement` `UIDragElement` 各系统窗关闭/页签 |
| 04 | `UIItemInventory` `UIEquipInventory` `*Tooltip` |
| 05 | `UISkillBook` |
| 06 | `UIQuestLog` |
| 07 | `UINpcTalk` `UtilDlgEx` |
| 08 | `Gameplay/MapleMap` `Stage` |
| 09 | `Drop` / drop handlers |
| 10 | `UIUserList` `UIBuddy*` `UIParty*` `Messenger` |
| 11 | `Pet` / `UIPetEquip` |
| 12 | CashShop UI/handlers |
| 13 | activity / minigame shells |
| 14 | `UIMedal` |
| 15 | `feature-gap` 残余 ❌ |

## 回滚

每波改动应可按文件/功能局部还原；禁止跨波混提。本任务用户未要求 git commit，回滚靠工作区 diff。
