# USE_RETURN_SCROLL（回城卷）

> 日期：2026-11-05 ｜ 仅客户端上行 opcode 对齐；服务端已挂 UseItemHandler

## 目标

消耗品使用路由中，回城/传送卷（2030000..2030099）发送原生 opcode `USE_RETURN_SCROLL=85`，不再误走 `USE_ITEM=72`。

## 取证结论

| 项 | 结论 |
| --- | --- |
| 原生 | v95 证据表 `CWvsContext::SendPortalScrollUseRequest` → `USE_RETURN_SCROLL`；port-095 记为「证据」 |
| s9 | `RecvOpcode.USE_RETURN_SCROLL(0x55)` 与 `USE_ITEM` **同挂** `UseItemHandler`；body 同形 `int ts / short slot / int itemId` |
| 回城分支 | `ItemConstants.isTownScroll`：`2030000 <= id < 2030100`（`ANTI_BANISH_SCROLL=2030100` 除外） |
| 先前台账 | wave3 曾标「假缺口」（因发 72 也能进同一 handler）；parity-gap 仍列缺口。**原生对齐以发 85 为准** |

## 实现

- `Net/OutPacket.h`：`USE_RETURN_SCROLL = 85`
- `Net/Packets/InventoryPackets.h`：`UseItemPacket::is_town_scroll` + ctor 路由

## 服务端

无需改动。

## 验证

- 构建：`tools/build_smoke.bat`
- 冒烟：背包双击 `2030000` 段回城卷 → 服务端收 opcode 85 → 扣物并传送；`2030100` 仍走 72
