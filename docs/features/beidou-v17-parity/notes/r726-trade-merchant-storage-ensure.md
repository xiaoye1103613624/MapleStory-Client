# r726 — Trade / Merchant / Storage ensure-open

## Ensure-open
- Trade `ensure_trade` 补 `makeactive`；meso/item/lock 走 ensure
- Merchant 开房 + `0x19` update：`ensure_merchant`（emplace + makeactive）
- Storage store/take/arrange/meso：复用 `ensure_open()` 再刷

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
