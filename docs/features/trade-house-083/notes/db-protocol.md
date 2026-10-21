# 交易行 DB 与协议展开

> 隶属 `DESIGN.md` §7–§8。本文件仅设计，**禁止本阶段 DDL**。

## 1. listing 状态机

```text
FIXED:
  ACTIVE --买至0--> SOLD_OUT（每次购买物入买家已拍，R3）
  ACTIVE --卖家下架--> CANCELLED（物退）
  ACTIVE --到期--> EXPIRED（物退）

AUCTION:
  ACTIVE --结标有人--> SOLD（物入买家已拍）
  ACTIVE --结标无人--> EXPIRED（物退卖家）
  ACTIVE 不可 CANCELLED（禁下架）

FIXED 购买入账（R3）：
  TH_BUY 成功 → 物写入买家 won_stash（source=BUY_FIXED）
  已拍无空槽 → STASH_FULL，事务失败（不写背包）
```

## 2. bid 状态机

```text
ACTIVE（冻结中）--被超--> OUTBID（解冻）
ACTIVE --结标胜--> WON（转实扣）
ACTIVE --挂单异常取消--> CANCELLED（解冻）
```

## 3. won_stash 槽分配

1. 找该角色 `claimed_at IS NULL` 中最小空 `slot` 0..49。  
2. 无空槽 → 上层返回 `STASH_FULL`。  
3. 领取：写背包事务成功后删行或标 `claimed_at`。  
4. 整理：按 slot 重排连续（可选协议 `TH_STASH_COMPACT`）。

## 4. 冗余筛选列（listing）

为避免每次解析 blob，建议冗余：

- `item_type` tinyint（InventoryType）  
- `equip_slot` smallint（玩家向分组枚举，非原始 EquipSlot 亦可）  
- `req_job` int  
- `req_level` smallint  
- `stat_str/dex/int/luk/pad/mad` smallint（主属性缓存，上架时从 item 提取）

## 5. SEARCH 排序键枚举

```text
PRICE, TOTAL_PRICE, QTY, NAME, SELLER, REMAIN_TIME, CREATED,
CURRENT_PRICE, START_PRICE, BID_COUNT, END_AT, REQ_LEVEL
```

非法组合（如直售模式传 BID_COUNT）→ 服务端忽略并回退默认。

## 6. 冻结金币实现备选

| 方案 | 说明 |
| --- | --- |
| A. 角色表 `frozen_meso` | 可用=meso-frozen；结标/超价调整 |
| B. 仅 bid 表 sum(ACTIVE) | 每次校验聚合 |

推荐 **A**（读写简单）；以服务端实现期为准。

## 7. 结标任务

- 扫描 `mode=AUCTION AND status=ACTIVE AND end_at<=now`。  
- 批量限制每 tick N 条；行级锁防并发出价。  
- 防狙击（R2）：出价若 `now > end_at - anti_snipe_sec`（默认 30）→ `end_at = end_at + anti_snipe_sec`（累加延时，非重置为 now+N）。  
- 已拍永不自动删（R1）；仅 UI 警告角标。