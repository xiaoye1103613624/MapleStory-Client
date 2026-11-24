# Phase 0 · 雇佣商店差距与 SoftKey 结论（2026-11-20）

## 对照结论

**未按原生代码级全部还原。** 本版范围 A：FM CREATE + 上架 + 访客购买 + `UIWindow.img/PersonalShop` 原生 UI。

## SoftKey

- Angel：聊天/店名 = `CCtrlEdit` + 物理键；`CSoftKeyboardDlg` 仅 PIC。
- 本端曾 ~400+ 处 SoftKey 热键滥用；2026-11-17 已物理删除非 PIC。
- 商店 `C`/`B` SoftKey 体被掏空且未接回 Textfield → CREATE 断线。
- **本版口径**：窗内 Textfield + 原生按钮；SoftKey 仅 PIC。

## 证据

| 层 | 结论 |
|---|---|
| 封包 | CREATE/SET_ITEMS/BUY/EXIT/OPEN_STORE + ROOM 4/5 + 0x19 骨架在；CREATE 无调用点 |
| UI | `UIMerchant` 曾用 `CashShop.img/CSList`；Personal/Entrusted 仅 backgrnd+tip |
| IMG | live `UIWindow.img/PersonalShop` 含 BtBuy/BtStart/BtExit/BtClose/…（orange-wz） |
| IDA | MapleStoryT `CPersonalShopDlg` 钮 ID：Start=0x3E9 Buy=0x3EB Close=0x3EA Exit=0x3EF Info=0x3EC；布局走 UIWindow2/LayoutMan，本端用 UIWindow.img + 资产几何落位 |
| Angel | `angel-social-facts.md` `CPersonalShopDlg` BuyItem/OnKey/MoveItemToInventory |

## 二期预留

黑名单 / 已售 / VisitList / 完整拖拽 PutItem / 场标点击进店 / owner 包体分支。
