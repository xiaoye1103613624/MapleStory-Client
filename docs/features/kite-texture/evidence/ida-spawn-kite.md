# IDA：SPAWN_KITE 链

日期：2026-11-06  
IDB：本机 MapleStory（`user-ida-pro-mcp`）

## 分发

- `sub_531325`：场 OnPacket 大 switch  
- `pExceptionObject` ∈ [270,272] → `sub_65AC81`  
  - 270 → `sub_65ACB4`（CANNOT_SPAWN_KITE，弹系统串）  
  - 271 → `sub_65ACDF`（SPAWN_KITE）  
  - 272 → `sub_65B2CA`（REMOVE_KITE）

## SPAWN_KITE（`sub_65ACDF`）摘要

1. `oid = read_int`；查重表 `sub_65B896`  
2. `item = read_int`  
3. 两 `read_string`（msg / name，顺序与 s9 `msg` 后 `name` 一致）  
4. `x,y = read_short ×2`；绘制原点 `x-3`、`y-100`  
5. `sub_5D3322(item)` 取道具 property；子键常量 **1588** → 场上动画节点（对照 NX = `iconReward`）  
6. 后续挂聊天气泡 / layer（`sub_426C7E` / `sub_43EA3E`）——本版客户端用 A11M 白字近似，气泡壳预留

## 与 s9 差

`PacketCreator.spawnKite` 末尾 `writeShort(pos.x); writeShort(ft)`，第二 short 为 foothold。  
客户端不得 `read_point()`；用包内 x + 本地角色脚底 y，再套原生 `-3/-100`。

## opcode（s9_list）

- 270 `CANNOT_SPAWN_KITE`  
- 271 `SPAWN_KITE`  
- （272 为 REMOVE，见同链）
