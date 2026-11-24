# 原生引擎全盘校验 + 192 装备 UI 补齐（1C / 跟192）

> 日期：2026-11-22｜决策：1C + 跟192｜协议：s9

## 结论摘要

- **不是整引擎未实现**：核心 UI/Net/渲染已成型；缺口集中在交互命中、扩展装备侧栏、以及若干壳窗/数据空态。
- **双击背包装备开个人情报**：已修（`UIStateGame::doubleclick` 命中 UI 必 return，对齐 MinMS）。
- **龙咆哮 effect≈怪物 hit 观感**：**NO-FIX**（WZ 分节点、播放链正确；紫圈残留此前已修）。
- **HP 能否到 0/1 以下**：受击可到 0 并死亡；自损技（龙咆哮）服务端拒自杀。**禁止**无权威 clamp 到 1。
- **神秘徽章窗**：新建 `UIMysticBadge`，BP70–75。
- **装备槽布局锁定（2026-11-23）**：见 [`equip-slot-192-layout-plan.md`](equip-slot-192-layout-plan.md) — **六戒 52/53**、图腾→192 三格、`Addon` 分流后删开窗、口袋 BP33 解宠冲突。
- **侧栏**：`[神秘][机器人(+心脏)][角色装备][图腾×3]`；不再打开 `UIAddonEquip`。
- **192 Equip 壳已入库 + Android / totem 已接线**：主窗 classic；原生壳 `equip_native_root()`；s9 RING/POCKET/ANDROID 已同步。

## Wave 对照

| Wave | 状态 |
| --- | --- |
| 0 证据矩阵 | [`evidence/gap-matrix.md`](evidence/gap-matrix.md) |
| 1 可玩交互 | 双击情报 bug 已修；ITEM 键/拖拽见同日 baseline；HP/咆哮终裁见上 |
| 2 装备 UI | EquipSlot MYSTIC1–6；UIMysticBadge；s9 `MYSTIC(Mb)`；Addon 对拍保持 |
| 3 引擎台账 | 见下文 §引擎台账 |
| 4 验证 | `build_smoke` + 真机清单见 §验收 |

## 改动文件（客户端）

- `IO/UIStateGame.cpp` — 双击命中 UI 不落 CharInfo
- `Character/Look/EquipSlot.{h,cpp}` — MYSTIC1..6 = 70..75
- `Character/Inventory/Inventory.cpp` — 图腾/神秘找空位
- `IO/UIElement.{h,cpp}` — `MYSTICBADGE` 类型 + `anchor_below_of`
- `IO/UITypes/UIMysticBadge.{h,cpp}` — 新窗
- `IO/UITypes/UIEquipInventory.cpp` — `sync_side_panels` 挂载神秘窗
- `Net/Handlers/InventoryHandlers.cpp` — 刷新神秘窗
- `IO/UI.cpp` — ESC/Tab 白名单
- `MapleStory.vcxproj(.filters)`
- 资源：`UIWindow.img/MysticBadge/backgrnd`（bak: `UIWindow.img.bak_pre_mystic_20261122`）

## 改动文件（服务端 s9）

- `EquipSlot.java` — `MYSTIC("Mb", -70..-75, cash -170..-175)`
- **未做 DDL**（装备位走既有 inventoryequipment；写库需另授权）

## 引擎台账（Wave3 重扫摘要）

| 类 | 项 | 标记 |
| --- | --- | --- |
| Combat | 瞬移/二段跳 | 同日已接 MinMS 位移；真机待验 |
| Combat | 龙咆哮特效错挂 | NO-FIX 观感 |
| Input | 快捷栏拖拽/ITEM 键 | 同日已修；真机待验 |
| Inventory | 双击→CharInfo | **本轮已修** |
| Equip | Addon 机器人等 | 已有；与 192 Tab 页签形态不同但槽位对齐 |
| Equip | 神秘徽章 | **本轮已接** |
| Equip | 192 Equip 多 Tab 壳 | 预留（资源空壳） |
| Equip | 六戒 −52/−53 + 口袋 −33 | **本轮已接**（BP33 出宠装表；−133 仍宠） |
| Equip | AUX_WEAPON −62 | NO-FIX（原始武器号段） |
| Net | 常规 opcode 注册 | 仅 RECALCULATE_STATS 有意注销 |
| Map | 雇商进店 | 二期预留 |
| Weather | 打滑 | NO-FIX（三端无原生） |
| UI | 活动 HUD 空键 | 预留（需活动协议） |

## Layout Contract（装备侧栏）

- 宿主：`UIEquipInventory` active → `sync_side_panels`
- 顺序（顶对齐，L→R）：`UIMysticBadge` | `UIAndroidEquip`(+HEART) | 主窗 | `UITotemEquip`（192 `Equip/totem`）
- Android：`Equip/backgrnd2` + `Android/backgrnd`；心脏底栏第 8 格
- 图腾：贴 `base_dimension` 右缘；仅 TOTEM1–3
- Addon：开窗流删除（残留实例强制 deactivate）
- 资源门：Mystic=`window4("MysticBadge")`；Android=`Android/backgrnd`∧`backgrnd2`；Totem=`totem/backgrnd`

## 构建

- **equipfix（双击回归）**：见 `_build_out_equipfix\MapleStory.exe` / live `MapleStory.equipfix.exe`；日志 `evidence/rebuild-equipfix-20261122c.log`。
- 前序侧栏：`_build_out_equip_sidepanels`（20:50:38）。
- `/p:MultiProcessorCompilation=false /m:1` + 独立 `IntDir`/`OutDir`。
- Addon blank 包：`Data\UI\UIWindow.img.AddonEquip_blank_robot.img`（关客户端后覆盖 `UIWindow.img`，清 `.nxcache\UI.nx`）。
- s9 ANDROID 1200 段已同步；**未做 DDL**。

## 真机验收清单

1. 背包装备双击 → 穿戴，**不**开个人情报（21:47 回归已修：取消 doubleclick 的 draggedicon 门闩）
2. 角色装备栏双击 → 卸下
3. 打开装备栏 → 顶齐四栏；Android 可见色板+人影；`Slots/1206` 有框则画出
4. 辅助仍为自绘；机器人水印格应空白（替换 blank 包后）；无假冒 192
5. 神秘无锁提示（设计如此）

## 证据索引

- MinMS `UIStateGame::doubleclick` 无 draggedicon 短路
- MapleStoryT 串：`UI/UIWindow4.img/Equip/Android/*`（高版本页签形态）
- 既有：`docs/features/dragon-roar-hp-flicker-audit-2026-11/`
- 既有：`docs/features/native-baseline-audit-2026-11-22/`
