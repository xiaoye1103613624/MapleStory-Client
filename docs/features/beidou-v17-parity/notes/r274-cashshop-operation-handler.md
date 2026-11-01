# r274 — CASHSHOP_OPERATION 回包 handler

- 事实源：s9 `PacketCreator` showCashInventory(0x4B) / wish(0x4F|0x55) / bought(0x57) / message(0x5C) / giftOk(0x5E) / slots(0x60|0x62|0x64)
- `CashShopOperationHandler`：解析常用 mode，购买成功记 itemId/sn；未知 mode 打日志
- `PacketSwitch` emplace `CASHSHOP_OPERATION=325`
- 点券刷新仍靠同包后的 `QUERY_CASH_RESULT`（r273）

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
