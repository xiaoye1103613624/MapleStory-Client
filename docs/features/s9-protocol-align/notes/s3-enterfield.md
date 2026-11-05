# S3 切片：进图（频道登录 / 频道选定 / 地图初始化）

> 协议事实源：`BeiDou-Server_s9`（s9）。opcode 名值以 `OutPacket.h`/`PacketSwitch.cpp`（客户端）与 `RecvOpcode.java`/`SendOpcode.java`（s9）为准。
> 本切片 c2s 实际只有 `PLAYER_LOGIN`（已在 **S2 §2.5** 逐项对齐）；其余进图数据流均为 **s2c 服务端驱动**，由客户端解析渲染、已实机验证成功。

## 1. 进图握手时序（c2s + s2c）

```
客户端                                            s9
  │                                              │
  │ SELECT_CHAR(0x13) ───────────────────────►  CharSelectedHandler（登录服）
  │ ◄────────────────────────── SERVER_IP        （频道 IP/端口）
  │  （连接频道服）                              │
  │ PLAYER_LOGIN(0x14) ──────────────────────►  PlayerLoggedinHandler（频道服，仅 readInt）
  │ ◄──────────── CHANNEL_SELECTED(s2c,0x14)     （客户端读 int 频道号 + 跳过其余；见 §2）
  │ ◄──────────────────────── SET_FIELD(s2c)    （角色数据 + 地图初始化）
  │ 渲染地图 / 进入游戏                            │
```

## 2. s2c 关键包对账

### 2.1 CHANNEL_SELECTED（s2c，InPacket 值 20）
- 客户端 `ChannelSelectedHandler`（`Net/Handlers/WaveAShellHandlers.cpp:4428`）：
  ```cpp
  int32_t ch = recv.available() >= 4 ? recv.read_int() : 0;
  while (recv.available()) recv.skip_byte();   // 防御式：跳过其余字节
  ```
- 行为：读 int（频道号），其余字节全部跳过。
- **与 s9 兼容** ✓：服务端 `SendOpcode.CHANNEL_SELECTED` 发送的包体即便含额外字段，客户端也一律 `skip_byte` 消化，无字段级错配风险（防御式解析）。

### 2.2 SET_FIELD（s2c，地图/角色初始化）
- 服务端经 `PacketCreator.getWarpToMap` / `updatePlayer` 等推送地图、角色、周边实体。
- 客户端 `Stage::get().get_player().init()` + `UI::change_channel` 路径解析。
- **验证状态**：r13 真机会话捕获到 `SET_FIELD`（进图 617B，含 `maplemap/enter/10000`）且客户端成功加载地图；W10 实机（2026-11-01）进入 Henesys 并正常渲染 HUD/四窗/小地图，无字段解析异常或崩溃。
- **结论**：SET_FIELD 为服务端驱动、结构复杂但客户端解析已对齐，无字段级矛盾证据。本切片不逐字节比对 SET_FIELD（规模不匹配且已被实机 render 验证覆盖）。

## 3. c2s 部分（引用 S2）
- `PLAYER_LOGIN (20)`：客户端 `PlayerLoginPacket` = `write_int(cid)`，服务端 `PlayerLoggedinHandler.handlePacket` = `readInt()` 仅读 int → **逐字段对齐** ✓（详见 S2 §2.5，含 S1 误归因纠正）。

## 4. 结论
- S3 进图层的唯一 c2s 包 `PLAYER_LOGGEDIN` 已与 s9 逐项对齐（S2）。
- s2c 握手 `CHANNEL_SELECTED` 客户端防御式解析、天然容错；`SET_FIELD` 服务端驱动且实机渲染成功。
- 进图整链与 s9 一致，**无字段级错配**。
- 本切片**未动任何代码**，符合授权边界。

## 5. 后续修正（2026-11-05）：§2.2/§4 的「无字段级错配」已作废

> 上述 §2.2「SET_FIELD 实机渲染成功、无字段解析异常」和 §4「进图整链与 s9 一致、无字段级错配」是基于**旧会话（mapid=1）**的结论，已被 2026-11-05 的实机负对照推翻：admin 进图纯白屏、看不到角色、小地图无预览。

- 真因是当前 `BeiDou-Server_s9` 的 `PacketCreator.addCharStats:208` 把 `exp` 写成 `writeInt` 4 字节 + `:213` 尾部 `writeInt(0)` 4 字节；客户端 `LoginParser::parse_stats` 按 `read_long` 8 字节（S10/主线口径）且无尾部 skip ⇒ 字段整体平移 4 字节，`skip(4)//gachaexp` 吞掉真实 mapId `100000000`，再从 spawnpoint 槽读出 `16` 当 mapid → `Stage::load` 抛 `map data missing` → 白屏。
- 两种布局总宽都是 19 字节，**不抛异常、只平移字段**，故旧实机"看起来进图成功"掩盖了错配。
- 已修复并实机验证闭环（`Stage::load ok map=100000000`、射手村地图/角色/小地图 MAX 态全恢复）。完整证据链、字节表、6 文件修复清单与防复发判据见 **`notes/enterfield-exp-misalign.md`**。
- 修正结论：**SET_FIELD 存在字段级错配（exp 宽度），本切片「无错配」结论作废**；opcode 名值对齐部分仍然成立。
