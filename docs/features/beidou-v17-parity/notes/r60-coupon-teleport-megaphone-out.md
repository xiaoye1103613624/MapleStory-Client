# r60 — Coupon / Teleport / Megaphone 出站

对齐 s9 `CouponCodeHandler` / `UseCashItemHandler`(504/507) / `TrockAddMapHandler`：

| UI | 出站 | 布局 |
|----|------|------|
| `UICoupon` SoftKey | `COUPON_CODE(230=0xE6)` | short(0) + string code |
| `UITeleport` Enter / SoftKey | `USE_CASH_ITEM(79)` 504* | slot + id + byte vip + int map \| string name |
| `UITeleport` S | `TROCK_ADD_MAP(102=0x66)` | byte(1) + byte vip |
| `UIMegaphoneBasic` SoftKey | `USE_CASH_ITEM(79)` 507* | slot + id + string〔+ whisper if 5072〕 |

附：`TeleportRock::first_valid` / getters；RaceSelect → `UICreateCygnus`（`UIMiscShells.h`）。

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
