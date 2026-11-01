# r275 — PersonalShop C 再开店 + Avatar try-on 结论

- `UIPersonalShop`：`C` SoftKey 店名 → `HiredMerchantCreatePacket`（514 优先，与 ctor SoftKey 同路径）
- `UICashShop` ctor：进商城发 `CheckCashPacket` 兜底拉余额
- Avatar try-on：核对 s9 `CashOperationHandler` 无独立 try-on action（仅 0x03 buy / 本地 Preview SoftKey）→ **搁置闭环，非缺实现**

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
