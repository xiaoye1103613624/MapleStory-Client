# 地图 NPC 全部消失 — 最终根因与修复（2026-11-19）

## 现象
进图后地图场景/小地图正常，但主画面所有 NPC 立绘与名牌全部不显示（魔法密林 101000000、神木村 100000000 均复现）。

## 排查证据链（packet_dump + 运行时落盘日志）

1. **服务端正常发包**：`packet_dump.log` 抓到 56+ 个 `SPAWN_NPC`（opcode 0x101），SET_FIELD 顺序正常，字段解析与 s9 `PacketCreator.spawnNPC`（oid:int/id:int/x:short/cy:short/flip:bool/fh:short/rx0/rx1/byte1）逐字段对齐，id=1032000 等资源在 `Data/Npc/` 存在。→ 排除服务端/资源问题。

2. **排除旧 RCA 的 FIXATED 假设**：早前 `npc-disappear-rca-2026-11-19.md` 归因为 `Npc.cpp` 把 NPC 设 FIXATED 导致 fhlayer=0 被地形遮挡。实测该改动在 10:14 已从源码撤销（`Npc.cpp` 无条件 `physics.move_object`），但 NPC 仍消失 → 假设不成立。

3. **运行时日志锁定断点**：在 `MapNpcs::spawn()/update()/clear_spawns()` 落盘取证（已撤销）：
   ```
   clear_spawns (dropping 0)
   spawn oid=1000000002 ... （共 112 次，包全部入队）
   clear_spawns (dropping 112)   ← 队列被清空
   update#300 spawns_before=0 active=0 ...
   instantiate=0, active=0
   ```
   结论：NPC 包成功入 `spawns` 队列，但 `MapNpcs::update()` 消费前队列被二次 `clear_spawns()` 清空，从未实例化。

## 真正根因

`Net/Handlers/SetFieldHandlers.cpp::transition()` 的 fadeout 回调里，load 之后有一次多余的二次清理：
- `Stage::load()` 前（:145）有一次 `poll_events()`，它把**服务端在 fadeout 期间就发来的【新图】NPC/mob/drop spawn 包**提前 dispatch 入队；
- `Stage::load()` 后（原 :164-166）的 `clear_spawns()` 把这批新图包连同旧图残留一起清掉；
- 后续 `poll_events()`（原 :169）已无包可泵 → 场上永远没有 NPC。

而 `transition()` 入口 `Stage::get().clear()`（:234）早已清过旧图残留，二次清理本就多余。

## 修复（2 处）

1. **`Net/Handlers/SetFieldHandlers.cpp`**：删除 load 后的二次 `clear_spawns()`（npc/mob/drop 三行），保留入口 `clear()`。让 :145 提前入队的新图 NPC 包存活到主循环 `update()` 实例化。

2. **`IO/UITypes/UIStatusBar.h`**：排查中发现独立崩溃 bug——`classic_qs_icons/names/counts` 数组按 `MODERN_QS_SLOTS`(20) 分配，而 `refresh_classic_qs_icons()` 遍历 `CLASSIC_QS_SLOTS`(26)，i=20..25 越界写 `Texture()`/`std::string::clear()` 破坏堆 → 进图构造 UIStateGame 时 0xC0000005 崩溃。改为 `CLASSIC_QS_SLOTS`(26)。

## 实机验收

- 构建 `build`：Release/x64，EXIT=0，exe 23:13:06。
- 干净启动自动登录进图（魔法密林 101000000），客户端稳定存活（FPS 63-67，在线 54s，不再崩溃）。
- 截图 `npc-final-fixed.png`：工作人员×2、补生子、队长老板、Wyvern 等 NPC 名牌与立绘全部正常显示，小地图标记与主画面一致。
- 证据：`npc-check-*.png`（故障态/登录态）、`npc-live-inmap.png`（修复前无 NPC）、`npc-fixed-live.png`（崩溃修复后仍无 NPC）、`npc-final-fixed.png`（NPC 恢复）。

## 备注
- 所有临时诊断日志（npc_trace.log / uistategame_ctor.log 落盘代码）已全部撤销，源码仅留上述 2 处实质修复。
- 排查中 exe 于 23:22 被并发会话重新构建；本次实机验收基于 23:13 含修复的 exe，源码改动已落地（git diff 确认）。
