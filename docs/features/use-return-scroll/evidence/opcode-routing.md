# 证据摘要 · USE_RETURN_SCROLL

## 协议

| 方向 | opcode | 名称 | body |
| --- | --- | --- | --- |
| c2s | 85 / 0x55 | USE_RETURN_SCROLL | int timestamp, short slot, int itemId |
| c2s | 72 / 0x48 | USE_ITEM | 同形（其它消耗品） |

## 对照

- port-095：`SendPortalScrollUseRequest` → 85（证据级）
- s9 `PacketProcessor.java`：`USE_RETURN_SCROLL` → `UseItemHandler`；`USE_ITEM` → 同 handler
- s9 `UseItemHandler`：`isTownScroll` 分支 `applyTo` + 可选 banish 存档

## 客户端判定

```text
is_town_scroll(id) <=> 2030000 <= id < 2030100
```

与 `ItemConstants.isTownScroll` / `ItemId.ANTI_BANISH_SCROLL=2030100` 对齐。
