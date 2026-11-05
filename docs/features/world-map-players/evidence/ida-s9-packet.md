# 证据摘要 · WORLD_MAP_PLAYERS

## IDA（BeiDou.exe.i64，2026-11-05）

- `find_regex` WorldMap：`p/WorldMap/WorldMap%d.img`、`Map/WorldMap/`、`Map/MapHelper.img/worldMap/npcPos`
- xref `0xb3f268` → `sub_9EA1E1`：加载 `MapLink` / `link` / `linkMap`，拼接 `Map/WorldMap/*.img`
- **无**玩家列表 / WORLD_MAP_PLAYERS 字符串或命名符号

## s9 源码对账

- `RecvOpcode.WORLD_MAP_PLAYERS(0x115)` → `WorldMapPlayersHandler`
- `SendOpcode.WORLD_MAP_PLAYERS(0x178)` 写出：`writeInt(mapId); writeShort(n); ×{writeString, writeShort(level), writeByte(channel)}`
- 缓存键 `worldId:mapId:gmFlag`，TTL 750ms，上限 40

## 局限

原生客户端无对等 UI；列表坐标为本端约定（底图右上），非 IMG 节点还原。
