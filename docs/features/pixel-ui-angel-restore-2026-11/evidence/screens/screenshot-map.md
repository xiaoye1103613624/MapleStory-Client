# 正确客户端截图对照映射

> **权威视觉源（用户 2026-11-13 指定）**：`C:\Users\11036\Desktop\正确客户端截图`  
> 本目录为可引用工作副本；一律覆盖式同步，不以旧副本为准。

## 同步约定

```text
Copy-Item "C:\Users\11036\Desktop\正确客户端截图\*" `
  "docs/features/pixel-ui-angel-restore-2026-11/evidence/screens\" -Force
```

中文原名与 Wave 规范化别名并存；像素量测优先读权威目录原名，文档引用可用英文别名。

## Wave 规范化别名 → 权威原名

| 别名（evidence） | 权威原名（Desktop） | 用途 | 实测尺寸 |
|---|---|---|---|
| `login.png` | `登录界面.png` | Wave1 登录 1280 | 1280×720 |
| `world-select.png` | `选择频道页.png` | Wave1 世界/频道 | 1280×720 |
| `channel-selected.png` | `选中频道页.png` | Wave1 频道选中 | 1280×720 |
| `char-select.png` | `选择角色页.png` | Wave1 选角 | 1280×720 |
| `enter-game.png` | `进入游戏页.png` | Wave1 进图过渡 | 1280×720 |
| `create-char.png` | `创建角色页.png` | Wave1 种族选择 | 1280×720 |
| `create-adventurer.png` | `创建冒险家角色页.png` | Wave1 建角 | 1280×720 |
| `statusbar-bar.png` | `底部信息栏和功能按钮.png` | Wave2 底栏 | 816×142 |
| `statusbar-hud.png` | `底部信息栏、按钮、快捷键窗.png` | Wave2 底栏+QS | 1282×142 |
| `stats.png` | `角色能力.png` | Wave3 S | 428×349（主+详合图） |
| `equip.png` | `角色装备.png` | Wave3 E | 348×305 |
| `inventory-full.png` | `背包栏展开.png` | Wave3 I | 602×291 |
| `skill.png` | `技能框+技能组合框.png` | Wave3 K | 120150B / 见原图 |
| `quest.png` | `任务栏.png` | Wave3 Q | 见原图 |
| `userlist.png` | `好友、组员、家族、家族联盟、拒绝列表.png` | Wave3 P | 见原图 |
| `hotkeys-seik.png` | `打开了S-E-I-K页面.png` | Wave3 多窗同屏 | 1280×720 |
| `chat-expanded.png` | `聊天框展开时候.png` | Wave2 聊天 | 569×103 |
| `chat-collapsed.png` | `聊天框收起的时候.png` | Wave2 聊天 | 566×40 |

## 其它权威原名（未改名，直接引用）

| 原名 | 用途 |
|---|---|
| `创建战神.png` / `创建战神2.png` | 建角·战神 |
| `创建骑士团页.png` / `创建骑士团2.png` | 建角·骑士团 |
| `底部快捷按钮窗.png` | QS 单独裁切 460×76 |
| `键盘设置.png` | KeyConfig 629×369 |
| `小地图*.png` / `世界地图*.png` | 小地图/世界地图 |
| `商城.png` | CashShop |
| `系统设置.png` / `游戏设置.png` | 设置 |
| `NPC对话*.png` / `任务*.png` | NPC/任务 UI |
| `聊天区域功能展示.mp4` | 聊天动效参考（非静帧） |

## S 窗口径（与截图关系）

- 权威截图 `角色能力.png` = **主栏+详情合图** 428×349；竖向边缘能量峰约 **x=206–208** → 更接近原生 classic 主栏宽，而非单独 300×392 画布。
- live `UIWindow.img/Stat` 若 `backgrnd` ≥290×380 则代码走 `UIStatsLayoutV4`（值进框 + 详情底对齐）；像素门禁以 **live 资源几何 + V4 表** 为准，合图截图作相对布局/文案对照，不作 300 宽绝对框。
- 其它原生窗（登录链 / E/I/K / 底栏）一律以本表权威截图 ≤1px 对照。
