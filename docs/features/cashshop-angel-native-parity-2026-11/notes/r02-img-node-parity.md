# r02 · orange-wz / nx IMG 节点对照（2026-11-12 续）

## Key 纪律（勿再犯）

- MCP 路径必须用 **完整** `tools/_gms_key_128.b64`（B64_LEN=172）。
- 禁止截断、手工凑零、或把 65536B `_gms_key.b64` 传给 orange-wz。
- `ivBase64=TSPHKw==`，`exclusive:false`，会话末 `unload_all`。

## 本轮实测（key 正确后）

| 根 | 结果 |
|---|---|
| `CashShop.img` | load OK |
| `UIWindow.img` | load OK |

### CashShop.img 关键结论

| 节点 | 结论 | 代码处置 |
|---|---|---|
| `Base/backgrnd` | 800×600 ARGB8888 | 预览/状态钮收进 800 宽 |
| `CSStatus` | **仅** BtCharge/BtCheck/BtCoupon/BtExit | 映射 ChargeNX/Refresh/Coupon/Exit；无 Wish/Mileage/Help |
| `CSChar` | **仅** BtBuyAvatar/BtDefaultAvatar/BtTakeoffAvatar | try_maple_btn；无 Save/Inventory |
| `CSList` | BtBuy + **BtGift**/BtReserve/BtRemove | 每槽补 BtGift |
| `CSTab/Tab` | 子键 **1..9** | `menu_tabs[i]=Tab[i+1]` |
| `CSGLChargeNX` | **不存在** | 充值走 CSStatus |

### TradingRoom（UIWindow.img）

| 节点 | 结论 |
|---|---|
| backgrnd | 565×474，origin (0,0) |
| BtTrade/Reset/Cancel/Coin | 四态齐全；origin (0,0) → **坐标靠 IDA OnCreate** |
| BtEnter | **不在 TradingRoom**；Messenger/PersonalShop 有 → UITrade 借用 Messenger/BtEnter |

## 工具链

1. orange-wz MCP（主）  
2. `tools/nx_inspect.py` + `Data/.nxcache/UI.nx`（交叉）  
3. IDA OnCreate（绝对坐标）
