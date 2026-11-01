# r277 — CASHSHOP_OPERATION showGifts(0x4D) 解析

- s9 `showGifts`：`byte 0x4D` + `short count` + 每项 gift `addCashItemInformation`（long + itemId + 13B + 73B）
- `CashShopOperationHandler` 按项 skip，避免未处理告警

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
