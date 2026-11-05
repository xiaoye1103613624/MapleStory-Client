# WORLD_MAP_PLAYERS（世界地图玩家层）

> 日期：2026-11-05 ｜ 范围：客户端 handler + UI 挂点；服务端 s9 已有

## 目标

对接 s9 `SendOpcode.WORLD_MAP_PLAYERS = 0x178`（376），选中世界地图点位后请求并显示该图在线玩家（名字/等级/频道）。

## 取证结论

| 项 | 结论 |
| --- | --- |
| 协议 | **s9 自定义**（非原生 BeiDou 包）。c2s `RecvOpcode.WORLD_MAP_PLAYERS=0x115(277)` body=`int mapId`；s2c body=`int mapId / short count / N×{string name, short level, byte channel}` |
| 服务端 | `WorldMapPlayersHandler`：世界全频道扫描，`MAX_PLAYERS=40`，`CACHE_TTL_MS=750`，隐藏角色对非 GM 过滤 |
| 原生 UI | IDA（BeiDou.exe.i64）世界地图字符串仅 `Map/WorldMap/`、`MapLink`、`linkMap`、`npcPos`——**无在线玩家列表 UI**；本端右上列表为 s9 协议挂接布局 |
| 客户端挂点 | `UIWorldMap::select_map_spot` 选中即 `WorldMapPlayersPacket`；`WorldMapPlayersHandler`→`set_map_players`；SoftKey `L` 手动刷新 |

## 实现

- `Net/Handlers/WorldMapHandlers.{h,cpp}`：`WorldMapPlayersHandler` + 既有 drop result
- `Net/Packets/MiscOutPackets.h`：`WorldMapPlayersPacket`
- `IO/UITypes/UIWorldMap.{h,cpp}`：缓存/陈旧应答丢弃/右上渲染；文案 `worldmap.players.online`

## 服务端

无需改动（`feature/integration_client` 已注册 handler）。

## 验证

- 构建：`tools/build_smoke.bat`
- 真机步骤：[pending-live-tests §1](../pending-live-tests/README.md)
