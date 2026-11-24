# 怪物手册原生完整度审计 · 阶段 0 摘要（2026-11-20）

> 只读取证，未改业务代码。对照源：Angel IDA 历史证据 + 当前 IDA `MapleStoryT.i64`（含完整 `CUIMonsterBook` 符号）+ orange-wz live `UIWindow.img/MonsterBook` + 既有 fullchain 文档。

## 结论（一句话）

**不是「按原生完美实现」**：书壳布局 / 翻页 / 便签筛选 / 封面协议 / 基本情报页已大致对齐 v83；仍缺 ContextMenu、LeftTabInfo、真卡面立绘与计数、RightTab 的 AREA/REWARD 详情层；`BtSearch` 故意空实现有 Angel 支撑。  
另有一份「自定义增强」插件（`E:\MXD\自定义功能实现\怪物图鉴`）可借鉴**掉落/搜索/解锁**，但那是偏离原生的增强，不是原生还原。

## 证据源

| 层 | 来源 | 要点 |
| --- | --- | --- |
| IMG | orange-wz live `UIWindow.img/MonsterBook` | 15 子节点：含未接线的 `LeftTabInfo`、`ContextMenu{t,c,s,BtRegister,BtRelease}`；`backgrnd 475×349` / `cardSlot 174×256` / `infoPage 170×231` / `icon/0..7 22×27` |
| Angel | `ida_monsterbook_ctor.json` + fullchain §9–11 | 建钮立即数与本端一致；`UpdateCheckList`/`UpdateUI`/`LoadBookIcon`/`LoadMobInfo` 存在；**无 Search 具名方法** |
| MapleStoryT | IDA 当前库 | `CUIContextMenu::OnCreate` 绑定 Register(2000@9,5)/Release(2001@9,20)；`DrawSelectLayer` 用 cover/select + `GetMonsterBookCover`；结构含 `AREA`/`REWARD` 数组 |
| 本仓 | `UIMonsterBook.*` + `monsterbook-fullchain-2026-11` | 布局与数据链已修多轮；真机全链路验收仍标「未验收」 |
| 增强参考 | `自定义功能实现/怪物图鉴` | 插件补丁灰卡/HP???/Tab 锁 + 服务端 `0x372B/0x372C` 掉落搜索；**非原生** |

## 差距表（相对「原生完美」）

| 区域 | 本仓 | 原生证据 | 完整度 |
| --- | --- | --- | --- |
| 书壳/坐标/箭头/页签位 | 已绑 | Angel 立即数 + IMG | 高 |
| 便签筛选 + 激活态 | 已实现 | UpdateCheckList | 高 |
| 基本情报数值 | 本地现算 | infoPage DrawTextA | 中高（特殊卡口径 vs 服务端不一致） |
| 封面显示 + C 设封面 | 有 | cover + SetCover 包 | 中高（无右键菜单 Register/Release） |
| ContextMenu | **未实现** | MapleStoryT OnCreate + IMG | 缺 |
| LeftTabInfo | **未绑** | Angel ctor SP_2590 | 缺 |
| 卡面立绘 / 拥有计数 | 占位 icon 轮播 | LoadMobInfo + ItemNo | 缺/浅 |
| 未拥有卡灰态全库浏览 | 只列已拥有 | 原生全库+灰/??? | 行为差 |
| RightTab 地图/掉落 | 原始 ID 状态行 | AREA/REWARD 结构 | 浅 |
| BtSearch | 空实现 | Angel 无 Search handler | 对齐原生空壳 |
| 掉落概率/物品搜索 | 无 | 仅增强插件有自定义 opcode | 非原生范围 |

## 192 / 增强可移植性

- 192 整合包侧主要是 `String.wz/MonsterBook` / DB 表，**未见可直接移植的 CUI 源码**。
- `自定义功能实现/怪物图鉴` 基于 Angel 地址补丁 + ijl15：适合「全书可读 + 掉落/搜索增强」方案，**不适合**作为「完美还原原生」主路径；若选增强需单独协议与服务端接线。
