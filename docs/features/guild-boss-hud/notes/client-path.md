# 客户端路径

- 部署：`E:\MXD\BeiDou-Client-xy\`
- IDB：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64`
- NX：`Data\.nxcache\Map002.nx`（地图属性）、`Map.nx`（Obj/guild.img）
- 样板图：`Map/Map9/990000900.img`
- 关键函数：
  - `sub_5587B3` @ `0x5587B3` — 读 healer/pulley
  - `sub_558B0F` @ `0x558B0F` — OnPacket 287/288
  - `sub_558C0C` / `sub_558C3B` — HealerMove / PulleyState
  - `sub_55852B` — healer RelMove
