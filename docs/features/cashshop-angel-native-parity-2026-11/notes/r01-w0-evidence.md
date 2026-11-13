# r01 · W0 证据摘要（2026-11-12）

## 决策锁定

- 协议跟 s9；Angel 只校布局/交互/状态机
- 范围：商城主窗 + 普通面交 + 现金交易房 + 个人/雇佣店（不含 MTS）

## IDA（Angel.i64 · server_health OK）

| 符号 | EA | 用途 |
|---|---|---|
| `CTradingRoomDlg::OnCreate` | `0x7c23d0` | 控件：Edit(296,245)、BtEnter(520,244)、BtClaim(494,241)、BtTrade(116,49)、BtCoin(170,244)、BtReset(116,71)、Scroll(542,23) |
| `CTradingRoomDlg::PutMoney` / `Trade` | `0x7c37ca` / `0x7c39a0` | 面交放钱/确认 |
| `CCashTradingRoomDlg::PutMoney` / `Trade` | `0x485c3f` / `0x485dcd` | 出站 `COutPacket(123)` → 对齐 s9 `PLAYER_INTERACTION` |
| `CPersonalShopDlg::*` | `0x6fa0f5` 族 | BuyItem/PutItem/MoveItemToInventory |

既有商城 48-case / 7 子窗：见 `cashshop-angel-calibration-2026-11`。

## orange-wz

- `load_files` CashShop.img + UIWindow.img（128B GMS key，`exclusive:false`）→ 两文件均返回 `IMG 解析失败`
- 回退：客户端运行时 `.nx`（`nl::nx::UI` / `UiAssetResolve::window`）+ 既有校准 facts
- 已 `unload_all`

## 实现映射

| Angel 面板 | 当前端节点 |
|---|---|
| CCSWnd_Char | `CashShop.img/CSChar` |
| CCSWnd_List | `CSList` |
| CCSWnd_Status | `CSStatus` |
| CCSWnd_Tab | `CSTab` |
| CTradingRoomDlg | `UITrade` ← `UIWindow.img/TradingRoom` |
| CCashTradingRoomDlg | `UICashTradingRoom` → 委托 `UITrade` |
| CPersonalShopDlg | `UIPersonalShop` + `UIMerchant` |
