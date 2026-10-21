# 可玩性 v1：核心循环

> 状态：客户端阻塞项已闭环；端到端需连 BeiDou 手工点测  
> 产物：`docs/features/playability-core-loop/`  
> 相关：  
> - [login-flow-ui-ida-align](../login-flow-ui-ida-align/README.md)  
> - [v83-hud-quest](../v83-hud-quest/README.md)  
> - [opcode-align-beidou-gms083](../opcode-align-beidou-gms083/README.md)

## 1. v1 范围

登录 → 创角 → 进图 → 移动战斗 → 任务/NPC → 换图。

**不进 v1**：组队、好友、交易、商城、公会、拍卖。

## 2. 验收剧本

| # | 步骤 | 通过标准 |
|---|---|---|
| 1 | 冷启动 → 补丁 → 登录 UI | 书框/账号框可见；无崩溃 |
| 2 | 账密 → 世界/频道 → 角色列表/创角 | 列表或创角 UI；错误有提示 |
| 3 | 进新手图 → 走跳攻击 → 拾取 | 角色可控；伤害数字；掉落可捡 |
| 4 | NPC 对话 / 接交任务 | NpcTalk；QuestLog；店确认包已处理 |
| 5 | 传送门换图 → 再进图 | 新图加载；atlas 卸载；关键状态不丢 |

## 3. 阻塞项处理策略

只修**阻塞上述剧本**的问题。缺口见 [`notes/gaps.md`](notes/gaps.md)。

本里程碑已关闭：

- `CONFIRM_SHOP_TRANSACTION` 注册
- 换图 `GraphicsGL::unload_atlas`
- 缺 UtilDlgEx 时结束对话避免卡脚本（既有）

## 4. 代码锚点

| 环节 | 模块 |
|---|---|
| 补丁 | `Util/Patch/PatchUpdater` |
| 资源 | `Util/Resource/ResourceService` |
| 登录 UI | `IO/UITypes/UILogin*` |
| 进图 | `Net/Handlers/SetFieldHandlers` |
| 任务 | `Net/Handlers/QuestHandlers`、`UIQuestLog` |
| NPC 店 | `ConfirmShopTransactionHandler` |
| HUD | `UIStatusBar`、`UiAssetResolve` |

## 5. 验证记录

手工点测勾选上表；构建：Windows `MapleStory.vcxproj` 或 CMake Release。
