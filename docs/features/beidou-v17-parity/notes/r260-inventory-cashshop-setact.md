# r260 — Inventory CashShop `set_act` 覆盖修复

全尺寸态 `BT_CASHSHOP` 先 `set_act(!enabled)` 后被 `set_act(enabled)` 覆盖（无 `_SM` 变体）。去掉第二次调用，商城钮与 Coin/Extract 同态。
