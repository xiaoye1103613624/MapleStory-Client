# r200 — PersonalShop CREATE 带店名（s9）

## 证据

s9 `PlayerInteractionHandler`：店名在 **CREATE(0)**（`createType` 4|5 + maple-string + skip3 + itemId），**不是** OPEN_STORE(0x0B)。

## 改动

- 新增 `HiredMerchantCreatePacket(title, cashItemId, createType=4)`
- SoftKey 有店名 → CREATE（Personal/Member 优先 514*，Entrusted 优先 503*）
- SoftKey 空 / `O` → 仍 `HiredMerchantOpenPacket`

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
