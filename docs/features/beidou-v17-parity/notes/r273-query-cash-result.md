# r273 — QUERY_CASH_RESULT 点券真值

- 事实源：s9 `PacketCreator.showCash` → `SendOpcode.QUERY_CASH_RESULT(0x144=324)`  
  读序：`int NX_CREDIT` / `int MAPLE_POINT` / `int NX_PREPAID`
- `QueryCashResultHandler`：写入 `UIStateCashShop::cash_*` + `cash_balances_ready`；同步 `MaplePointsLocal`
- `CheckCashPacket`（`OutPacket::CHECK_CASH=228` = s9 `RecvOpcode.CHECK_CASH`）→ `TouchingCashShopHandler`
- `UICashShop` 充值面板改读服务端三币种；`BtChargeRefresh` 发 CHECK_CASH
- 注册：`PacketSwitch` emplace `QUERY_CASH_RESULT`

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
