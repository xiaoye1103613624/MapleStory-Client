# 每日礼物 / Daily Gift（灰框 UI 骨架）

> 状态：客户端 UI 骨架已闭环（假数据可点）；无协议 / 无真实领取  
> 适用：MapleStory-Client · 本版 HD **仅 docs 归档**，运行时以 Geometry/Text 灰框为主  
> 产物目录：`docs/features/daily-gift-hd/`  
> 日期：2026-09-28

## 1. 背景与目标

在 StatusBar 事件菜单「每日礼物」打开可拖拽签到窗，严格 **28 天 7×4** 布局；本版交付可编译、可开关、可动画、假领取的 UI 骨架。

### 已确认决策

| 代号 | 结论 |
| --- | --- |
| A1 | 仅 UI；不做服务端闭环 |
| B1 | 新建 `UIDailyGift` + `BT_EVENT_DAILY` 接线 |
| C | 灰框占位；HD 归档 docs，不强制运行时加载 |
| D1 | 28 天 7×4；重点天 9/12/17/24/26 |
| E | 领取/杀怪/日限/等级·MVP 仅预留 |
| F | UI 动画都做、逻辑不做 |
| 补充 | 点格假领取 + 已领取标识；**格子中部不放道具**；数字用 **0–9 独立资源拼装** |

## 2. 如何打开窗口

1. 登录进图  
2. 点击 StatusBar **事件**菜单  
3. 点击 **每日礼物**（`button:dailyGift` / `BT_EVENT_DAILY`）  
4. 打开 `UIDailyGift`

关闭：标题栏关闭钮、Escape、或再切其它窗。

## 3. 假领取 / 已领取验证

| 操作 | 预期 |
| --- | --- |
| 点击任意未领格子 | 该格灰显 + 青绿角标「V」+ 短闪光 |
| 再点同一格 | 无变化（已 Claimed） |
| 点底栏「领取礼物」 | 领取第一个未领格（假） |
| 重点天 9/12/17/24/26 | 金色边框区分（未领时） |
| 格子中部 | **仅空道具槽**，无 ItemData / 图标 / 道具名 |

## 4. 数字拼装（0–9）

客户端使用 `digit_glyphs[0..9]`（每位独立 Text「0」…「9」）拼：

- 格子「第」+ *digits* +「天」
- 顶栏日期 `MM月 DD日 ~ MM月 DD日!`
- 底栏进度 `当前 / 需求`

**不写死整句位图。** HD PNG 已归档于 `resources/digits/`，对接见 [hd-integration.md](notes/hd-integration.md)。

## 5. 改动文件清单

### 新建

- `IO/UITypes/UIDailyGift.h` / `.cpp`
- `docs/features/daily-gift-hd/**`（本目录全套）

### 修改

- `IO/UIElement.h` — `Type::DAILYGIFT`（贴 `EVENT` 后）
- `IO/UITypes/UIStatusBar.cpp` — `BT_EVENT_DAILY` → `emplace<UIDailyGift>`
- `Configuration.h` / `.cpp` — `PosDAILYGIFT`
- `IO/UI.cpp` — Tab 列表加入 `DAILYGIFT`
- `MapleStory.vcxproj` / `.filters` — 登记新文件

### 明确未改

- `Net/**` Opcode / PacketSwitch / Handlers  
- `UIEvent` 假数据、其它 StatusBar 菜单  
- UI.wz / nx 写回  

## 6. 预留接口

`try_claim` / `on_claim_result` / `set_kill_progress` / `set_account_daily_limit` / `set_level_gate` / `set_mvp_tier`  
注释标明阶段 3+；本版 `try_claim` 仅本地 Claimed 视觉。

## 7. 已知限制

- 本环境未强制完整 VS 编过：请本地用 `MapleStory.sln` / vcxproj 编译验收  
- 无磁盘 PNG→Texture 通用加载：运行时灰框 + Text 数字；PNG 在 docs 待换皮  
- 关闭钮优先复用 `Basic.img/BtClose3|2`；无节点则可能无关闭贴图（Escape 仍可用）  
- 无真实杀怪 / 入包 / 协议  

## 8. 相关笔记

- [layout-contract.md](notes/layout-contract.md)  
- [decisions.md](notes/decisions.md)  
- [hd-integration.md](notes/hd-integration.md)  
- [resources/README.md](resources/README.md) — **已扩展 UI 清单（含 0–9）**  
