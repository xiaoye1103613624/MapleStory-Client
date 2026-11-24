# RCA：世界地图 hover → 0xc0000374 堆损坏

日期：2026-11-21

## 1. 崩溃证据

Windows 应用程序事件日志（Application/Id=1000）：反复出现
`Faulting application MapleStory.exe ... Exception code: 0xc0000374`，
故障模块 `ntdll.dll`（RtlFreeHeap），波及所有历史变体 exe（说明是逻辑 bug，非某次构建退化）。

WER 完整 dump：`C:\Users\11036\AppData\Local\CrashDumps\MapleStory.exe.{33444,48988,...}.dmp`
（14~16MB full dump）。用自写 `tools/_parse_minidump.py` 提取确认异常码 3221226356 = 0xC0000374
（x64 栈 unwind 需 PDB，部署目录无 PDB，故以静态根因链 + 实机 A/B 定案）。

## 2. 根因链（带证据）

1. `includes/NoLifeNx/nlnx/node.hpp:62-64` 官方注释：node 持有 file 裸指针，
   **file 被删除后 node 失效且 `operator bool()` 检测不出**。→ 悬垂 node 是静默的。
2. `Util/Resource/ImgSource.cpp::load_single_map()`（约 :500-548）：
   `RootTable::bind(RootId::Map002, file->root())` 把全局渲染根重绑到新 file，
   并经 LRU `s_map_files.erase(begin())` 销毁最老 file（MAP_KEEP=32）。
3. `Gameplay/MapleMap/Stage.cpp:173` 与 `Graphics/GraphicsGL.cpp:1640`：
   当前地图的 sprite/atlas/layer node 都从 `nl::nx::Map002` 取出并长期持有。
4. `IO/UITypes/UIWorldMap.cpp:653-687`：hover 命中城市 spot →
   `show_map(..., spot_map_id, ...)` → `MapTooltip::set_mapid()` → `get_life_on_map(mapid)`。
5. `Util/Misc.cpp`（修复前 :278）`get_life_on_map` 调 `get_map_node_name(mapid)` →
   `ensure_map(mapid)`（:354-360）→ **重绑 Map002 + LRU 淘汰**。
   hover 不同城市 = 反复用别的地图 id 重绑渲染根 → 当前地图渲染 node 悬垂 → RtlFreeHeap 崩溃。

## 3. 为什么三处 mapid<=0 守卫没根治

`worldmap-crash-and-window-assets-2026-11` 在 `get_map_node_name`/`set_minimap_mapid`/
`set_worldmap_mapid` 加了 `mapid<=0` 早退，挡住了"无关联点位 hover 传 0"的触发。
但 hover **真实城市**（合法 mapid，如 100000000）仍走 `ensure_map(合法id)` → 重绑+淘汰 → 崩溃。
守卫治标不治本。

## 4. 修复与 A/B 验证

修复：新增 `ImgSource::ensure_map_query` + 独立 `s_query_files`，
`get_life_on_map`/`get_mobs_on_map` 改走隔离查询，永不重绑 Map002、永不淘汰渲染 file。

A/B 逻辑：
- 修复前：hover 城市点 → dump 实证 0xc0000374（多次，全 exe 变体）。
- 修复后：同场景 12 点覆盖 + 18 次跨点快速悬停，客户端全程存活（FPS60、在线时长增长）。

## 5. 遗留

- 提权僵尸客户端进程（MainWindowHandle=0，4 个）当前会话无权限清理，需用户重启或管理员处理；
  它们不参与本次验证（验证用新启动的 PID 45220，已关闭）。
