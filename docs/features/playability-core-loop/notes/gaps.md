# 核心循环缺口跟踪

| 日期 | 现象 | 是否阻塞剧本 | 处理 |
|---|---|---|---|
| 2026-09-27 | 未知 opcode 仅 WARN | 否 | 观察；卡死再修 |
| 2026-09-27 | 启动缺补丁 URL | 否 | `allow_skip` 默认 true |
| 2026-09-28 | `CONFIRM_SHOP_TRANSACTION` 未注册 | 是（NPC 店结果） | 已注册 `ConfirmShopTransactionHandler` |
| 2026-09-28 | 换图 GL 锁定黑屏风险 | 是 | `SetFieldHandlers` try/catch + unlock |
| 2026-09-28 | 缺 UtilDlgEx 卡脚本 | 是 | `NpcDialogueHandler` 回退并 `NpcTalkMorePacket` |

## v1 剧本状态

| # | 步骤 | 客户端侧 | 备注 |
|---|---|---|---|
| 1 | 冷启动→补丁→登录 UI | 就绪 | 依赖 Data + 可选 patch.url |
| 2 | 登录/选服/创角 | 就绪 | 见 login-flow-ui-ida-align |
| 3 | 进图/战斗/拾取 | 就绪 | SetField / Combat handlers |
| 4 | NPC/任务 | 就绪 | Quest + NpcTalk + Shop confirm |
| 5 | 传送门换图 | 就绪 | ChangeMap + unload_atlas |

端到端绿勾需连真实 BeiDou；本表关闭**客户端阻塞项**。
