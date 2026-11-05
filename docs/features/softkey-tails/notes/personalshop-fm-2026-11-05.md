# PersonalShop CREATE FM 代码补强（2026-11-05）

## 证据（s9）

- `PlayerInteractionHandler` CREATE `createType` 4|5：`isFreeMarketRoom(mapId)` 失败 → `getMiniRoomError(15)`。
- `getMiniRoomError`：`ROOM(5) + byte0 + status`。
- `getPlayerShop`：`ROOM(5) + 4 + 4 + …`（个人店开张）。
- `getHiredMerchant`：`ROOM(5) + 0x05 + 0x04 + …`。
- FM 判定：`mapid/1000000==910 && mapid > 910000000`。

## 本轮缺口与改动

| 缺口 | 改动 |
| --- | --- |
| ROOM `sub==0` 误开交易 | `TradeHandler` 解析 MiniRoomError → SM/壳 tip（15=仅 FM） |
| ROOM `sub==4` 误开交易 | 解析 PlayerShop → 刷新已有 PersonalShop tip + `UIMerchant` 货架 |
| 非 FM 仍发 CREATE | `dispatch_shop_create_or_open` 客户端前置 FM 校验 |
| 背包双击 514/503 走 generic USE_CASH | `UIItemInventory` → PersonalShop / EntrustedShop |

## 实机仍待

见 [pending-live-tests §4](../../pending-live-tests/README.md)。
