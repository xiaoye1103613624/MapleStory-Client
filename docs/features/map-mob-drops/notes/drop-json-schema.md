# MobDropData JSON schema（预留）

本版客户端 **不读取** 任何掉落文件；`MobDropData` 构造时 `valid=false`、`entries` 为空。下列草案供后续装载对齐服务端 drop 表。

## 约定

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `mob_id` | int | 怪物 id（文件名或对象键） |
| `drops[].item_id` | int | 物品 id；展示前按升序排序 |
| `drops[].chance` | number | **概率 [0, 1]**（例如 `0.05` = 5%）。若源表为万分比，导入时除以 10000 |
| `drops[].min_qty` | int | 最小数量（默认 1） |
| `drops[].max_qty` | int | 最大数量（默认 = min） |

## 示例（单文件 per mob）

路径建议：`Data/MobDrops/{mob_id}.json`

```json
{
  "mob_id": 100100,
  "drops": [
    { "item_id": 4000000, "chance": 0.45, "min_qty": 1, "max_qty": 1 },
    { "item_id": 2000000, "chance": 0.10, "min_qty": 1, "max_qty": 3 }
  ]
}
```

## 示例（聚合表）

路径建议：`Data/MobDrops/drops.json`

```json
{
  "version": 1,
  "mobs": {
    "100100": [
      { "item_id": 4000000, "chance": 0.45, "min_qty": 1, "max_qty": 1 }
    ]
  }
}
```

## 接入步骤（后续）

1. 在 `MobDropData::MobDropData` 中读文件 / 内存表，填充 `entries`，设 `valid=true`。
2. 填充后 `sort` by `item_id`（构造末尾已预留 sort）。
3. UI 无需改字段；有行即画图标+ID+名+掉率+数量。
4. 导出工具应对齐私服 SQL/XML，避免手填。
