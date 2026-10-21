# v83 全量 HUD + 任务完整闭环

## 目标

BeiDou / v83 资源包仅有 `StatusBar.img` + `UIWindow.img` 时：

1. 进游戏有底部状态栏、聊天输入、快捷栏图标
2. 装备 / 背包 / 属性 / 技能 / 任务 / 按键设置等热键可开或安静失败
3. 任务日志三栏（可接 / 进行中 / 已完成）+ 放弃 + 包同步 + NPC 对话接交

## 已确认决策

- 范围 **1B** 全量 HUD（非仅血条）
- 任务做完整客户端闭环；接/交走 NPC `NpcTalkMore`，日志负责展示与放弃

## 实现逻辑

### 资源解析

[`Util/UiAssetResolve`](../../Util/UiAssetResolve.h)：

- `window` / `window4`：UIWindow2/4 → UIWindow
- `status_bar_root`：StatusBar3 → StatusBar
- `shop_root`：Shop2 → Shop
- `has_game_hud` / `usable`：进场与热键门闩

### 状态栏 / 聊天

- [`UIStatusBar`](../../IO/UITypes/UIStatusBar.cpp)：无 StatusBar3 时 `classic_mode`，读 `StatusBar.img/base|gauge|Bt*| *Key`
  - `BtShort` → KeyConfig
  - `BtWhisper` → ChatBar 耳语焦点（`/w `）
  - `BtClaim` → no-op
  - QuickSlot：按 keymap（Insert/Home/PgUp/…）绘制技能/道具图标
- [`UIChatBar`](../../IO/UITypes/UIChatBar.cpp)：classic 用 `base/chat` + 可选 `chatTarget`
- [`Slider`](../../IO/Components/Slider.cpp) CHATBAR：`status_bar_root` → Basic.VScr 回退

### 窗口守卫

[`UIStateGame::send_key`](../../IO/UIStateGame.cpp) 在 `emplace` 前检查 `usable`；各窗口 ctor 缺根节点时 `active=false`。

### 任务

| 层 | 行为 |
|---|---|
| `QuestData` | 名称/摘要 + Check.img 等级/职业/前置；`list_available` |
| `QuestLog` | started=进行中，completed=已完成（去掉死轨 in_progress） |
| `UIQuestLog` | TAB0 可接 / TAB1 进行 / TAB2 完成；放弃仅 TAB1；搜索三栏生效 |
| `SHOW_STATUS_INFO` mode1 | 接/更/弃/完成 + refresh |
| `UPDATE_QUEST_INFO` mode8 | `set_completed` + refresh |
| `UINpcTalk` | AcceptDecline / YesNo 支持 Q* 按钮别名 |

映射证据：[`evidence/mapping.md`](evidence/mapping.md)  
Check 字段：[`notes/check-img-fields.md`](notes/check-img-fields.md)

## 预留

- 超大小地图 canvas 降采样
- EventList / socialChat（v83 无节点，已安全跳过）
- 快捷栏拖拽改键（仍走 KeyConfig）
- 服务端任务脚本本身

## 验收清单（BeiDou 包）

1. 选角进图：底部条 + 聊天可见；地图/BGM 正常；无 StatusBar3 不黑屏
2. 热键：装备 / 背包 / 属性 / 技能 / 任务 / 按键设置可开或安静失败
3. QS 槽位显示已绑定技能/道具图标；对应键可释放
4. 任务：三栏语义正确；进行中可放弃；NPC 接任务后进进行中；完成通知后进已完成
5. `_enter_game.log` 可达 `fadeprocedure done`

## 构建

Visual Studio：`MapleStory.vcxproj` **Release | x64**（或本机惯用配置）。
