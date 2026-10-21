# 交易行 UI 线框与组件展开

> 隶属 `docs/features/trade-house-083/DESIGN.md` §6。出图阶段按 ui-photoshop：原生、1x、组件化、多状态。

## 1. 页面–组件映射

| 页面 | 依赖组件 |
| --- | --- |
| 浏览·直售 | frame, MainTab, ModeTab, CategoryTab, search fields, BtSearch, ColHeader(sortable), Row*, Scrollbar, BtBuy, qty, pagination, 装备筛选组 |
| 浏览·拍卖 | 同上（列换成当前价/起拍/出价数/倒计时）+ BtBid + CountdownText |
| 出价弹层 | BidDialogFrame, FieldBid, BtConfirmBid, BtCancel, 冻结提示字 |
| 我的上架 | list + BtCancelListing（仅 FIXED 行可用） |
| 上架 | ModeTab, 背包 Slot*, Field*, ComboDuration（直售）, 时长只读（拍卖）, BtList |
| 已拍 | WonSlot×50, BtClaim, BtClaimAll, 可选 BtCompact；久存角标 BadgeStashOld（R1）；直售购与拍卖成交均由此领取（R3） || 记录 | RecordSubTab, readonly list |
| 全局 | BtClose, UINotice |

## 2. 浏览表列宽建议（逻辑 px · 总内容宽约 600）

**直售**：图标36 · 名称140 · 数量40 · 单价70 · 总价70 · 卖家90 · 剩余70 · 余量弹性

**拍卖**：图标36 · 名称130 · 当前价70 · 起拍56 · 出价数40 · 卖家90 · 剩余80

## 3. 已拍格阵

- 推荐：原点相对内容区 `(16, 52)`，格 `32×32`，间距 `2`，**10 列 × 5 行**。  
- 选中框外扩 1px 高亮。  
- 占用文案右上或底栏左侧。

## 4. 装备筛选面板

折叠条高 18；展开高约 72–96：

```text
职业 [全部▼]  部位 [全部▼]  等级 [  ]-[  ]
STR[ ]-[ ] DEX[ ]-[ ] INT[ ]-[ ] LUK[ ]-[ ]
攻击[ ]-[ ] 魔攻[ ]-[ ]  [重置] [应用=并入搜索]
```

「应用」不单独协议，条件并入下一次 `TH_SEARCH`。

## 5. 倒计时

- 客户端本地用 `endAt - serverNow`（OPEN/SEARCH 带 serverTime 校准）。  
- `&lt;5min` 橙色；`&lt;1min` 红色闪烁（可选，出图给 warn 字色即可）。

## 6. 出图优先级（供 D6A Plan）

1. chrome（frame/close/panels）  
2. tabs（Main/Category/Mode）  
3. 列表行+表头箭头  
4. 主按钮组  
5. 已拍槽  
6. BidDialog  
7. 拼装整窗 PSD（浏览直售、浏览拍卖、上架双模式、已拍、记录）
