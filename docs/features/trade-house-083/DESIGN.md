# 083 交易行（Trade House）— 详细设计（已拍板）

> **状态**：阶段 1 抉择 **已确认** · 阶段 1.5 详细设计 **已落地** · 残留 R1–R3 **已确认** · **下一里程碑：D6A UI 出图（待确认 `PLAN-ui.md` 后开 Photoshop；仍不写业务代码）**  
> **本版交付**：设计文档 + UI 出图 Plan（业务实现 / Photoshop 出图 / DDL **全部预留至 Plan 确认后**）  
> **工作区**：`MapleStory-Client`（HeavenClient 系 · v83 · 对接 BeiDou gms083）  
> **规范**：xiaoye-MapleStory-dev · ui-photoshop（设计层引用；本阶段不调用 Photoshop）  
> **产物根**：`docs/features/trade-house-083/`  
> **子文档**：`notes/ui-wireframes.md` · `notes/db-protocol.md` · **`PLAN-ui.md`（D6A 出图完整 Plan）**

---

## 0. 一句话结论

做 **双模式全服交易行**：**直接出售**（自定义价、可下架）+ **拍卖**（起拍价、默认 1 小时倒计时、不可下架、价高者得）；**仅金币**；**自定义封包 + 原生自制窗**；**直售购买与拍卖成交统一入「已拍」50 格**（再由玩家领取到背包；已拍满则购买/出价失败）；浏览列表 **公开卖家名**；一级分类对齐背包，装备支持职业/部位/属性区间筛选，结果列可排序；与面交/FM/NPC 店并存不改。

---

## 1. 已确认决策摘要（D1–D13）

| ID | 抉择 | 状态 |
| --- | --- | --- |
| D1 | **双模式**：① 直接出售；② 拍卖（默认 1h 倒计时、不可下架、价高者得） | **已确认** |
| D2 | **A** 自定义封包 + 客户端 UI | **已确认** |
| D3 | **仅金币** | **已确认** |
| D4 | **B** 设计后先出图 | **已确认** |
| D5 | **A** 原生自制窗 | **已确认** |
| D6 | **A** 拍板后先 PSD/PNG，仍不写业务代码 | **已确认** |
| D7 | **A** 本版只设计表（不 DDL） | **已确认** |
| D8 | **A** 不改面交 / FM / NPC 店 | **已确认** |
| D9 | **已拍道具容量 50**：成交物入「已拍列表」格子（非简单仓库/邮件）；**直售购买与拍卖成交均强制入已拍**（R3） | **已确认** |
| D10 | **C** NPC + 状态栏入口均可 | **已确认** |
| D11 | **A** 列表公开卖家名 | **已确认** |
| D12 | **A** 实现时再定是否自动 commit | **已确认** |
| D13 | **B** 一级=背包分类；装备另有职业/部位/属性区间；搜索结果列可排序 | **已确认** |

合理默认与 **R1–R3 已确认规则**（见 §11）：

| 项 | 规则 |
| --- | --- |
| 货币 | 仅 meso；无点券开关首版 |
| 直售有效期 | 24h / 48h / 7d 三档可选 |
| 拍卖时长 | **固定 1 小时**（配置可改，UI 首版不开放自定义） |
| 最低加价 | `max(1, ceil(当前价 × 1%))`，可配置 |
| 出价冻结 | 出价时冻结「本次出价全额」；被超价立即解冻退回 |
| **购买/成交入账（R3）** | **直售购买与拍卖成交一律强制入已拍栏**；再由玩家领取到背包。**禁止**「背包优先、满才溢出」。已拍满 → 直售购买失败 / 拍卖禁止出价（结标前校验） |
| **已拍过期（R1）** | **永不自动删除**；占用格子直至领取；满 N 天仅 **警告角标**；**禁止到期销毁** |
| **防狙击（R2）** | 拍卖最后 **30s** 内出价 → `end_at += 30s`；**默认开启**（`auction_anti_snipe_sec=30`，配 0 可关） |
| 手续费 | 上架固定费 + 成交比例费（可配）；拍卖流拍不退上架费 |
| 改价 | 本版不做（直售须下架再上；拍卖不可改） |
| 下架/流拍退回 | 仍可 **优先退卖家背包**；背包满再占已拍空格（退回路径，非购买路径） |
---

## 2. 仓库现状摘要（只读结论）

| 项 | 现状 |
| --- | --- |
| 客户端 | HeavenClient / Journey 系 C++，v83，对接 BeiDou gms083 |
| 交易行 | **无**现成 UI / opcode / DB |
| 可复用 | `MapleFrame` / `MapleButton` 四态 / `TwoSpriteButton` / `Textfield` / `Slider` / Tooltip；外挂 PNG 范式（`UIMapSearchPanel`）；背包页签语言见 `UIItemInventory`（EQUIP/USE/SETUP/ETC/CASH） |
| 装备槽 | `EquipSlot`（HAT/FACE/…/WEAPON/RING…）；筛选 UI 用玩家向分组，映射到槽位集合 |
| 边界 | UI+协议壳落本仓；挂单/竞价/结算/DB 落配套 BeiDou（不在本工作区） |
| 本阶段不做 | 业务代码、Photoshop、DDL |

---

## 3. 双模式玩法规则

### 3.1 模式对照

| 维度 | 直接出售（FIXED） | 拍卖（AUCTION） |
| --- | --- | --- |
| 卖家定价 | 自定义 **单价**（可堆叠）/ 总价展示 | 设 **起拍价**（装备强制 qty=1） |
| 买家行为 | 按价购买（可拆数量） | 出价竞拍；结束时价高者得 |
| 时长 | 24h / 48h / 7d | **默认 1 小时**倒计时 |
| 下架 | **允许**卖家主动下架（未售部分退回） | **禁止**下架；无「取消拍卖」 |
| 加价 | 无 | 须 ≥ 当前价 + 最低加价 |
| 金币 | 购买时扣全款 | **出价冻结**；成交扣冻结额；被超价解冻 |
| 成交物去向 | **强制入买家已拍栏**（有空格才可买；满则失败） | **强制入买家已拍栏**（出价前校验空位） |
| 流拍 / 到期 | 到期未售 → 退回卖家背包（满则已拍栏占格退回） | 无人出价 → 退回卖家；有出价 → 最高价成交入已拍 |
| 手续费 | 上架费（上架时扣）+ 成交费（从卖家收款扣） | 同左；流拍 **不退**上架费 |
| 防狙击 | — | 最后 30s 出价 → `end_at += 30s`（R2，默认开） |

### 3.2 直接出售 — 上架表单

| 字段 | 规则 |
| --- | --- |
| 来源格 | 背包可交易物；非绑定/非商城禁交易；非已装备 |
| 数量 | 可堆叠可选 1..持有；装备=1 |
| 单价 | 整数 meso；受 `unit_price_min/max` 配置约束 |
| 时长 | 三档单选 |
| 预估 | 上架费 = flat + rate×(单价×数量)；展示到期时间 |
| 提交 | 扣物入托管 + 扣上架费 → `listing.mode=FIXED` · `status=ACTIVE` |

**下架**：`TH_CANCEL` → 物退背包，满则写入已拍栏空格并通知；无空格则 **禁止下架**（提示清理已拍/背包）。已售清的单不可下架。

**购买**：事务内锁挂单 → 校验 ACTIVE/库存/非自购/金币 → **校验买家已拍栏有空位（不足则 `STASH_FULL` 失败）** → 扣款 → **物写入买家已拍栏空格**（`source=BUY_FIXED`）→ 挂单减量 → 卖家入账（成交额−成交费）→ 写流水。**不**写入背包；玩家须在「已拍」页领取。

### 3.3 拍卖 — 上架表单

| 字段 | 规则 |
| --- | --- |
| 起拍价 | 整数 meso；≥ `auction_start_min` |
| 时长 | 首版 UI **写死 1 小时**（只读展示）；配置键可改默认分钟数 |
| 数量 | 首版拍卖 **仅允许 qty=1**（含可堆叠物一次拍 1 个；多件请直售） |
| 禁止 | 无下架钮；无改起拍价 |

### 3.4 拍卖 — 出价与冻结

```text
玩家出价 bidAmount
  1. 挂单须 ACTIVE 且 mode=AUCTION 且 now < end_at
  2. bidAmount ≥ current_price + min_increment(current_price)
  3. 买家 ≠ 卖家；已拍栏有空位（或本角色已是当前最高且仅加价——仍须保证结标时能入账）
  4. 冻结 bidAmount：从可用金币划入 frozen_meso（角色字段或 bid 表占用）
  5. 若存在前最高价：解冻其冻结额，写 OUTBID 通知
  6. 更新 listing.current_price / current_bidder_id；写 trade_house_bid
  7. **防狙击（R2，默认开）**：若 `now > end_at - anti_snipe_sec`（默认 30）→ `end_at += anti_snipe_sec`
```

**最低加价**：`min_increment = max(auction_min_step, ceil(current_price * auction_step_rate))`  
默认 `auction_min_step=1`，`auction_step_rate=0.01`。

**结标任务**（定时或到期触发）：

| 情形 | 处理 |
| --- | --- |
| 无有效出价 | `status=EXPIRED`；物退卖家（背包→已拍）；不退上架费 |
| 有最高出价 | 扣买家冻结额（已冻则转实扣）；卖家 +(成交价−成交费)；物入买家已拍栏空格；`status=SOLD`；写 record；释放其余失败者冻结 |
| 买家已拍满 | 结标前应已禁止使其成为最高（出价时校验）；若竞态满格：取消成交、解冻、物退卖家并记异常日志（实现期单测覆盖） |

### 3.5 手续费（统一可配）

```text
tradehouse.list_fee_flat
tradehouse.list_fee_rate          # 相对挂单总价（直售）或起拍价（拍卖）
tradehouse.deal_fee_rate          # 相对成交额，从卖家所得扣
tradehouse.auction_duration_sec = 3600
tradehouse.auction_min_step / auction_step_rate
tradehouse.auction_anti_snipe_sec = 30   # R2：最后 N 秒内出价则 end_at += N；0=关闭
tradehouse.won_stash_capacity = 50
tradehouse.max_listings_per_char
tradehouse.unit_price_min / unit_price_max
```

---

## 4. 已拍道具栏（Won Stash）· 50 格

### 4.1 定位

独立于背包/仓库/邮件的 **交易行专属领取仓**，容量 **固定 50**。用于：

1. **拍卖成交物**（强制入已拍）  
2. **直售购买成交物**（强制入已拍，与拍卖一致；**非**背包优先溢出）  
3. 直售下架 / 流拍退回时背包满的溢出（退回路径）  

**不是**通用仓库；仅交易行 UI「已拍」页操作。玩家从已拍领取后物才进入背包。

### 4.2 数据模型

**表 `trade_house_won_stash`**

| 列 | 说明 |
| --- | --- |
| id | PK |
| character_id | 归属角色 |
| slot | 0..49，角色内唯一 |
| item_id / quantity | |
| item_data | 完整道具 blob（属性/砸卷） |
| source | BUY_FIXED / BUY_AUCTION / RETURN_CANCEL / RETURN_EXPIRE |
| source_listing_id / source_record_id | 可空溯源 |
| created_at | 入栏时间 |
| claimed_at | 领取后软删或移历史；活跃行 claimed_at IS NULL |

约束：每角色 `claimed_at IS NULL` 的行数 ≤ 50；`(character_id, slot)` 活跃唯一。

**与挂单库存关系**：

- 挂单托管物在 `trade_house_listing.item_data`，**不占**已拍格。  
- 成交/退回成功写入已拍后，挂单侧数量归零或状态变更，托管结束。  
- 已拍格只存「待玩家领走」的实例，与 ACTIVE 挂单互斥不重复占同一实例。

### 4.3 UI（格子列表）

- 页签「已拍」：**5×10 或 10×5** 共 50 槽（出图锁定一种；推荐 **10 列 × 5 行**，贴近背包密度）。  
- 每格：图标 + 数量角标；空格 `SlotEmpty`；满格可选中。  
- 底栏：`领取`（选中格→背包对应类型空位）· `全部领取`（能领尽领，领不完保留）· 已用 `n/50`。  
- Tooltip：物品详情 + 来源（拍卖拍得 / 直售购买 / 下架退回 / 流拍退回等）+ 入栏时间；久存角标见 §4.6。

### 4.4 领取规则

| 动作 | 规则 |
| --- | --- |
| 单格领取 | 目标背包页有空位（及堆叠可合）→ 写入背包并清空该已拍槽；否则 Notice「背包已满」 |
| 全部领取 | 按 slot 升序尝试；部分成功回包 `claimedCount` |
| 不可 | 已拍格之间拖拽整理可做（实现期可选）；本设计允许「整理」按钮压缩到前部连续槽 |

### 4.5 满时行为

| 场景 | 行为 |
| --- | --- |
| 拍卖出价 | 已拍 `used≥50` 且当前角色 **不是** 该单最高价者 → **拒绝出价**；若已是最高仅再加价 → 允许（不新增占格） |
| 拍卖结标 | 理论上出价时已保证；竞态满则取消成交并退卖家（§3.4） |
| **直售购买（R3）** | 已拍无空格 → `STASH_FULL` **购买失败**（不扣款、不改挂单）。**不**尝试写入背包 |
| 下架/流拍退回 | 背包满且已拍满 → **禁止下架** / 流拍任务写入 `pending_return` 重试队列（卖家登录提示清理） |

### 4.6 过期是否占用（R1 · 已确认）

- **占用**：未领取的已拍物 **一直占格**，直至玩家领取。  
- **永不自动删除**：禁止到期销毁、禁止后台静默清格。  
- **仅警告角标**：`created_at + won_stash_warn_days`（默认 30）后 UI 显示「存放较久」角标（`BadgeStashOld`），**不**删物、**不**迁邮件。  
- 若运营日后要「到期销毁」，须 **另开决策**；本版 **禁止**。

---

## 5. 分类 · 装备筛选 · 列排序

### 5.1 一级分类（对齐背包）

| Tab | 对应 `InventoryType` | 说明 |
| --- | --- | --- |
| 全部 | — | 不过滤大类 |
| 装备 | EQUIP | item 段 1xxxxx |
| 消耗 | USE | 2xxxxx |
| 设置 | SETUP | 3xxxxx |
| 其他 | ETC | 4xxxxx |
| （不上架） | CASH | 默认禁止上架；无浏览 Tab |

另有 **模式过滤**：浏览区二级切换 `全部模式 / 直接出售 / 拍卖`（或与主列表列「类型」并存）。

### 5.2 装备高级筛选（仅一级=装备时展开）

| 条件 | UI | 字段 / 语义 |
| --- | --- | --- |
| 职业 | 下拉/多选芯片 | `reqJob` 掩码：全部、初心者、战士、魔法师、弓箭手、飞侠、海盗（v83）；匹配装备 `reqJob` |
| 部位 | 下拉分组 | 帽子/脸饰/眼饰/耳环/上衣/裤裙/鞋/手套/披风/盾牌/武器/戒指/项链/腰带/勋章/其它；映射 `EquipSlot` 集合 |
| 等级 | min–max | `reqLevel` |
| 属性区间 | 多行 min–max | `incSTR/DEX/INT/LUK`、`incPAD/MAD`、`incPDD/MDD`、`incACC/EVA`、`incSpeed/Jump`；空=不限 |
| 砸卷/星 | 可选 | `upgradeSlots` 剩余、`level`（星力若有）— 首版做「剩余卷数 ≥」即可，星力视服务端字段预留 |

筛选在服务端对 `item_data` 解析或冗余列（`req_job`,`equip_slot`,`req_level`, 主属性缓存）执行；实现期可冗余列加速。

### 5.3 列表列与排序

**直售列表列**

| 列 | 可排序 | 默认 |
| --- | --- | --- |
| 图标 | 否 | |
| 名称 | 是（按 itemId 或名） | |
| 数量 | 是 | |
| 单价 | 是 | **默认：单价升序** |
| 总价 | 是 | |
| 卖家 | 是（公开，D11） | |
| 剩余时间 | 是 | |
| 上架时间 | 是 | |

**拍卖列表列**

| 列 | 可排序 | 默认 |
| --- | --- | --- |
| 图标 | 否 | |
| 名称 | 是 | |
| 当前价 | 是 | **默认：剩余时间升序**（临近结束优先） |
| 起拍价 | 是 | |
| 出价次数 | 是 | |
| 卖家 | 是 | |
| 剩余时间 | 是 | 次要推荐 |
| 结束时间 | 是 | |

**表头交互**：

- 单击列头 → 该列升序；再点 → 降序；箭头 `▲/▼`。  
- 同时仅一列主排序；切换列重置为该列「自然默认向」（价格类默认升，时间类默认升=更快到期在前）。  
- 不可排序列无箭头、点击无效果。

搜索请求带：`sortKey` + `sortDir`（ASC/DESC）。

---

## 6. UI 信息架构 · 组件 · 状态 · 线框

> 出图约定：原生 · 1x · 组件化 · MapleButton 四态 / Tab 二态；产物 `docs/features/trade-house-083/ui/<组件>/vN/`。  
> 更细线框见 `notes/ui-wireframes.md`。

### 6.1 信息架构

```text
UITradeHouse（主窗）
├─ 顶栏：标题「交易行」 + BtClose
├─ 主导航 Tab
│   ├─ 浏览（默认）
│   ├─ 我的上架
│   ├─ 上架
│   ├─ 已拍（n/50）
│   └─ 记录
├─ 【浏览】
│   ├─ 模式：全部 | 直接出售 | 拍卖
│   ├─ 一级分类：全部/装备/消耗/设置/其他
│   ├─ 搜索条：关键词 · ID · 价格区间 · 搜索
│   ├─ 【装备时】高级筛选面板：职业 · 部位 · 等级 · 属性区间 · 收起/展开
│   ├─ 结果表：可排序表头 + 行 + 滚动
│   ├─ 拍卖行：倒计时（mm:ss / hh:mm:ss）
│   └─ 底栏：持有金币 ·（直售）购买数量+购买 |（拍卖）出价…
├─ 【我的上架】
│   ├─ 子过滤：全部/直售/拍卖
│   ├─ 列表（拍卖行无「下架」、显示当前价与倒计时）
│   └─ 直售：下架；拍卖：仅查看
├─ 【上架】
│   ├─ 模式切换：直接出售 | 拍卖
│   ├─ 背包选择区
│   ├─ 直售表单：数量·单价·时长·费用预估
│   ├─ 拍卖表单：起拍价·时长只读1h·费用预估
│   └─ 确认上架
├─ 【已拍】
│   ├─ 50 格槽位阵 + 选中高亮
│   └─ 领取 / 全部领取 · 占用 n/50
└─ 【记录】
    ├─ 购买 | 销售
    └─ 只读列表（含模式列）

弹层：
├─ UITradeHouseBidDialog     # 拍卖出价：当前价·最低加价·输入·冻结提示·确认
├─ UITradeHouseBuyConfirm    # 直售高价二次确认（可复用 UINotice）
└─ Toast / Notice            # 错误码文案
```

**入口（D10）**：专用 NPC 对话「交易行」→ `TH_OPEN`；状态栏按钮同样 `TH_OPEN`。

### 6.2 主窗尺寸基线

建议 **640×420**（含高级筛选展开时可长高到 **640×480**）；出图前按 shop/inventory 样板微调锁定。

### 6.3 线框（摘要）

**浏览 · 直售**

```text
+----------------------------------------------------------------+
| [=] 交易行                                              [X]    |
+----------------------------------------------------------------+
| [浏览] [我的上架] [上架] [已拍 3/50] [记录]                      |
+----------------------------------------------------------------+
| (·全部 ·直售 ·拍卖)  [全部][装备][消耗][设置][其他]              |
| 名称[____] ID[____] 价[__]~[__]  [搜索]                         |
| ▸ 装备筛选（展开时）职业[▼] 部位[▼] 等级[__]~[__] …             |
+----------------------------------------------------------------+
| 图标 | 名称▲ | 数量 | 单价▼ | 总价 | 卖家 | 剩余    |           |
|  ..  | ......| .... | ..... | .... | .... | ..:..   |  ←选中   |
+----------------------------------------------------------------+
| 金币: nnnnn    数量[- 1 +]              [<]1/N[>]  [购买]       |
+----------------------------------------------------------------+
```

**浏览 · 拍卖**

```text
| 图标 | 名称 | 当前价▼ | 起拍 | 出价数 | 卖家 | 剩余▲ |     |
| ...  底栏：[出价…]（打开 BidDialog）  倒计时红字临近结束         |
```

**出价弹层**

```text
+----------------------------------+
| 拍卖出价                    [X]  |
| 物品名 / 图标                    |
| 当前价: N    最低加价后: M       |
| 我的出价: [________]             |
| 将冻结金币: …   可用: …          |
| [取消]              [确认出价]   |
+----------------------------------+
```

**已拍 50 格**

```text
| [已拍] 占用 12/50                |
| [格][格]...×10                   |
| ×5 行                            |
| [领取] [全部领取] [整理]         |
```

**上架 · 模式切换**

```text
| 模式: (·直接出售  ·拍卖)         |
| 左: 背包页签+格  右: 表单字段    |
| 拍卖时时长显示「1 小时」不可改   |
```

### 6.4 组件树

```text
TradeHouse/
├─ chrome/     frame九宫格 · titleBar · panel/bg · listBg · searchBg · divider · BtClose
├─ tabs/       MainTab_* · CategoryTab_* · ModeTab_* · RecordSubTab_*   (二态)
├─ buttons/    BtSearch · BtBuy · BtBid · BtList · BtCancelListing · BtClaim · BtClaimAll
│              BtSort（表头可点热区+箭头图）· BtPrev/Next · BtQtyUp/Down · BtFilterToggle
├─ inputs/     FieldKeyword · FieldItemId · FieldPriceMin/Max · FieldQty · FieldUnitPrice
│              FieldBid · FieldAttrMin/Max_* · ComboJob · ComboSlot · ComboDuration
├─ list/       ColHeader{sortable} · RowNormal/Selected/MouseOver · Scrollbar*
├─ grid/       WonSlotEmpty · WonSlotFrame · WonSlotSelected   # 已拍 32×32
├─ item/       SlotEmpty · SlotFrame（上架背包）
├─ overlay/    BidDialogFrame · BidDialogBg
└─ feedback/   BadgeAuction · BadgeFixed · BadgeStashOld · CountdownText · EmptyState
```

### 6.5 状态表

| 组件 | 契约 | 状态 |
| --- | --- | --- |
| 主操作钮 | MapleButton | normal / mouseOver / pressed / disabled |
| 主导航/分类/模式 Tab | TwoSpriteButton | normal / selected（+可选 mouseOver） |
| 表头排序 | 热区+箭头 | unsorted / asc / desc |
| 列表行 | 底图 | normal / mouseOver / selected |
| 已拍格 | 槽 | empty / filled / selected |
| 倒计时 | 字色 | normal / warn(&lt;5min) / critical(&lt;1min) |
| Textfield | | idle / focus / disabled |
| 拍卖「下架」 | | **不渲染**或永久 disabled |
| 总开关关闭 | 入口 | disabled + Notice |

### 6.6 交互流转（关键）

```text
打开 → TH_OPEN → 配置摘要（费率/已拍占用/开关）
浏览改条件 → TH_SEARCH → 刷新表
直售购买 → 确认 → TH_BUY → **已拍栏刷新**（物不入背包）
拍卖出价 → BidDialog → TH_BID → 冻结金币刷新；他人超价 → TH_NOTIFY_OUTBID
结标（服务端）→ 买家 TH_NOTIFY_WON；已拍 +1
上架选模式填表 → TH_LIST
直售下架 → TH_CANCEL；拍卖无此操作
已拍领取 → TH_CLAIM / TH_CLAIM_ALL
```

---

## 7. DB 表草案（不执行 DDL）

> 展开索引与状态机见 `notes/db-protocol.md`。

### 7.1 `trade_house_listing`

| 列 | 说明 |
| --- | --- |
| id | PK |
| seller_id / seller_name | 公开名冗余（D11） |
| mode | 0 FIXED · 1 AUCTION |
| item_id / item_type / equip_slot / req_job / req_level | 分类与筛选冗余 |
| quantity | 剩余；拍卖恒 1 |
| unit_price | 直售单价；拍卖可空 |
| start_price | 拍卖起拍价 |
| current_price | 拍卖当前价（无出价=起拍价） |
| current_bidder_id | 可空 |
| bid_count | 拍卖出价次数 |
| currency | 0 meso（固定） |
| item_data | blob |
| status | 0 ACTIVE 1 SOLD_OUT/SOLD 2 CANCELLED 3 EXPIRED 4 SETTLED |
| fee_paid | 上架费 |
| created_at / expire_at / end_at | 直售用 expire；拍卖用 end_at |
| version | 乐观锁 |

### 7.2 `trade_house_bid`

| 列 | 说明 |
| --- | --- |
| id | PK |
| listing_id / bidder_id | |
| amount | 出价 |
| frozen | 是否仍冻结 |
| status | ACTIVE / OUTBID / WON / CANCELLED |
| created_at | |

### 7.3 `trade_house_won_stash`

见 §4.2（50 槽）。

### 7.4 `trade_house_record`

成交流水：listing_id、mode、buyer/seller、item 快照、qty、price、fee、created_at。

### 7.5 `trade_house_pending_credit` / `pending_return`

离线卖家入账；退回重试队列（已拍与背包皆满时）。

---

## 8. 协议指令草案（逻辑名 · 数值实现期避让官方表）

### 8.1 C2S

| 指令 | 载荷要点 |
| --- | --- |
| TH_OPEN | — |
| TH_SEARCH | mode, category, keyword, itemId, priceMin/Max, job, slot, reqLevelMin/Max, attrs{}, sortKey, sortDir, page, pageSize |
| TH_LIST | mode, invType, slot, qty, unitPrice \| startPrice, durationHours? |
| TH_CANCEL | listingId（仅 FIXED） |
| TH_BUY | listingId, qty（仅 FIXED） |
| TH_BID | listingId, amount（仅 AUCTION） |
| TH_MY_LISTINGS | modeFilter, page |
| TH_STASH_LIST | — |
| TH_CLAIM | stashSlot |
| TH_CLAIM_ALL | — |
| TH_RECORDS | side, page |

### 8.2 S2C

| 指令 | 载荷要点 |
| --- | --- |
| TH_OPEN_RESULT | enabled, fees, stashUsed/Cap, auctionDurationSec… |
| TH_SEARCH_RESULT | total, page, entries[]（含 mode、sellerName、currentPrice、endAt、bidCount…） |
| TH_LIST_RESULT / TH_CANCEL_RESULT / TH_BUY_RESULT / TH_BID_RESULT | code + 上下文 |
| TH_MY_LISTINGS_RESULT | entries[] |
| TH_STASH_RESULT | slots[50] 稀疏或列表 |
| TH_CLAIM_RESULT | code, claimed |
| TH_RECORDS_RESULT | entries[] |
| TH_NOTIFY | WON / OUTBID / SOLD / RETURNED / EXPIRED |

**错误码（增补）**：`AUCTION_NO_CANCEL`, `BID_TOO_LOW`, `BID_FROZEN_FAIL`, `STASH_FULL`, `AUCTION_ENDED`, `NOT_AUCTION`, `NOT_FIXED`, …

---

## 9. 配置开关（摘要）

见 §3.5；另：`tradehouse.enabled`、`tradehouse.max_listings_per_char`、`tradehouse.search_cooldown_ms`、`tradehouse.allow_cash_items=false`、`tradehouse.won_stash_warn_days=30`。

---

## 10. 本版明确不做（预留）

- [ ] 客户端 `UITradeHouse` 等业务代码  
- [ ] 服务端 Handler / 定时结标任务  
- [ ] DDL / DML  
- [ ] Photoshop PSD/PNG（**见 `PLAN-ui.md`；用户确认后再出图**）  
- [ ] opcode 最终数值锁定  
- [ ] 改面交 / FM / NPC 店  
- [ ] 点券货币  

---

## 11. 残留决策点（R1–R3 · **已确认**）

> 用户已拍板；下列为定稿，正文规则已同步。本版 **无待决残留**。

| ID | 用户原话 / 问题 | 落实（定稿） | 状态 |
| --- | --- | --- | --- |
| **R1** | 永不删除 | 已拍栏 **永不自动删除**；仅可 **警告角标**；**禁止到期销毁**。见 §4.6 | **已确认** |
| **R2** | 出价 +30s | 拍卖最后 **30s** 内出价 → `end_at += 30s`（防狙击）；默认开启（`auction_anti_snipe_sec=30`） | **已确认** |
| **R3** | 购买的存入已拍栏 | **直售购买也统一入已拍栏**（与拍卖成交一致）；**不是**「背包优先、满才溢出」。买到的东西进已拍 50 格，再由玩家领取到背包；已拍满 → 购买失败。见 §3.1 / §3.2 / §4 | **已确认** |

---

## 12. 已确认决策摘要（文末）+ 下一阶段

**已确认（D1–D13 + R1–R3）**：双模式（直售+拍卖 1h 不可下架价高者得）· 自定义封包+原生窗 · 仅金币 · 已拍 50 格 · **购买/成交统一入已拍** · 已拍永不自动删（仅警告）· 拍卖防狙击 +30s · NPC+状态栏入口 · 公开卖家名 · 背包一级分类+装备高级筛选+列排序 · 不改面交/FM/NPC 店 · 本阶段设计表 only · 下一步先出图不写代码。

**下一阶段（D6A · 见 `PLAN-ui.md`）**：

1. ~~用户确认残留 R1–R3~~（**已完成**）。  
2. 用户确认 **`PLAN-ui.md`** 出图完整计划后，再开 Photoshop MCP 出图。  
3. **仍不写**客户端/服务端业务代码、不 DDL。  
4. 出图验收通过后，再另开「业务实现 Plan」。

---

## 13. 文档索引

| 路径 | 内容 |
| --- | --- |
| `docs/features/trade-house-083/DESIGN.md` | **主入口（本文）** |
| `docs/features/trade-house-083/PLAN-ui.md` | **D6A UI 出图完整 Plan（待用户确认后执行）** |
| `docs/features/trade-house-083/notes/ui-wireframes.md` | 线框与页面组件映射展开 |
| `docs/features/trade-house-083/notes/db-protocol.md` | 表状态机与协议字段展开 |
| `docs/features/trade-house-083/notes/client-path.md` | 客户端 Data 路径（开工前索取后落盘） |
