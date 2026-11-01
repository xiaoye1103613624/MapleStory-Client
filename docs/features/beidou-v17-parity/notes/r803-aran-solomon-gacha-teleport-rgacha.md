# r803 — AranCombo / Solomon / GachaExp / Teleport / RemoteGacha

## Opcode
- `ARAN_COMBO_COUNTER = 163` → `AranComboPacket`
- `USE_SOLOMON_ITEM = 157` / `USE_GACHA_EXP = 158`

## SoftKey / Button
- ComboCounter SoftKey `+`：发 AranCombo 再本地 +1
- ItemInventory SoftKey `U`：2022xxx → UseSolomon；SoftKey `G` → UseGachaExp
- Teleport BtOk：优先 WorldMap 选中 / CharInfo VIP 名，再退回已存地图
- RemoteGachapon 数字按钮：直接 UseCash（去掉二次 SoftKey）

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
