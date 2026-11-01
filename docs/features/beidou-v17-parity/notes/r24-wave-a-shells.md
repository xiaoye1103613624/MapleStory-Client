# r24 · Wave A #3–#9 UI 壳批量（含 FamilyTree）

> 开窗壳 + 入站 emplace；业务包字段预留。  
> WZ：[`evidence/r24/`](evidence/r24/)  
> Handler：[`WaveAShellHandlers`](../../../Net/Handlers/WaveAShellHandlers.cpp)

## 清单

| # | UI | WZ | Type | 入站开窗 |
|---|-----|-----|------|----------|
| 3 | UIDelivery | Delivery | DELIVERY | PARCEL 322 |
| 4 | UIStoreBank | StoreBank | STOREBANK | FREDRICK 311 |
| 5 | UIViciousHammer | ViciousHammer | VICIOUSHAMMER | VICIOUS_HAMMER 354 |
| 6 | UIVegaSpell | VegaSpell | VEGASPELL | VEGA_SCROLL 358 |
| 7a | UICashGachapon | CashGachapon | CASHGACHAPON | CASHSHOP_CASH_GACHAPON_OPEN_RESULT 334 |
| 7b | UIRemoteGachapon | RemoteGachapon | REMOTEGACHAPON | SUCCESS_IN_USE_GACHAPON_BOX 117 |
| 8a | UIFamily | Family | FAMILY | FAMILY_INFO_RESULT 95 |
| 8b | UIFamilyTree | FamilyTree | FAMILYTREE | FAMILY_CHART_RESULT 94；Family BtTree |
| 9 | UIMemo | Memo | MEMO | MEMO_RESULT 41 |

## 验收

对应 opcode → 窗弹出；关钮/Esc；按钮 LOG；Release 部署 `BeiDou-Client-xy`。

## 下一刀

各窗 **包往返**（优先 Duey / Fredrick），或 W1 Skill 宏。
