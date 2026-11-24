# E:\MXD\扩展改动 → 本端实现盘点与 SideMenu 挂载建议

| 扩展改动目录 | 本端是否已有 UI/入口 | 建议 SideMenu 分类 | 种子绑定 |
|---|---|---|---|
| 个人伤害统计 | 是 `UIDamageRank` | 战斗 | `damagerank` |
| 每日签到 | 是 `UIDailyCheckin` | 日常 | `dailycheckin` |
| 装备染色 / 装备染色功能 | 是 `UIColorPrism` | 工具 | `colorprism` |
| cashshop-window | 是 `UIWindowCashShop` | 活动 | `windowcashshop` |
| GMS083 Kaentake 聊天表情包 | 是 `UIChatEmoticon` | 活动 | `chatemoticon` |
| 世界地图 tip | 是 `UIWorldMap` + tip 逻辑 | 工具 | `worldmap` |
| 左侧小图标 / 左侧快捷栏 | 是本功能宿主 | — | SideMenu 自身 |
| 血量突破上线 | 是 `UIBreakthroughStone` + 0x3730 | 战斗 | `breakthroughstone` |
| （文档内魔女森林） | 是 `UIWitchForest` | 战斗 | `witchforest` |
| （强化 UI_004） | 是 `UIEnhance` | 战斗 | `enhance` |
| 天气系统 | 是（场景层，非窗） | 不挂侧栏 | — |
| 套装系统 | 是（数据/tooltip） | 不挂侧栏 | — |
| 扩展背包 / 拖拽调整背包 | 是（背包窗内） | 不挂侧栏 | — |
| 扩展装备栏 / 护肩槽 | 是（装备窗） | 不挂侧栏 | — |
| 掉落特效 | 是（场景） | 不挂侧栏 | — |
| Bot UI / buffer计时 / 团队buffer | 部分或未统一窗入口 | 暂不挂 | 待补 FeatureRegistry |
| 商店列表加长 / 回购 / 玩家商店 | 商店窗改造 | 不挂侧栏 | — |
| 角色摇骰子 / 角色槽位 | 登录/创角 | 不挂侧栏 | — |
| 伤害上限 / 伤害玩法 / 伤害皮肤 | 协议/配置向 | 战斗仅挂「伤害统计」窗 | — |
| 高版本怪物卡 / 卡片收集 | `UIMonsterBook` | 工具 | `monsterbook` |
| 遗忘山谷 / 妖精学院 等地图包 | 地图资源 | 不挂侧栏 | — |
| 一些脚本 | 服务端脚本 | 可用 NpcScript 叶子绑 npcId | 需运营填 id |

已写入种子的 Builtin：见 `resources/side_menu.json` 的 `features[]`。
