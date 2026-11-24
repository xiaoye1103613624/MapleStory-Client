# 世界地图鼠标悬停闪退 —— 根治（0xc0000374 堆损坏）

> 状态：已闭环（根因定位 + 隔离修复 + 构建通过 + 实机压测无闪退）
> 适用版本：BeiDou v16/v17（v83）
> 日期：2026-11-21
> 产物目录：docs/features/worldmap-hover-crash-rootfix-2026-11/

## 1. 现象与结论

现象：打开世界地图（W）后，鼠标移入/划过地图上的城市点位就闪退，偶发进图后崩溃。

**根因**：世界地图 hover 任意城市点 → 读取该地图的 life/mob 数据 →
`Util/Misc.cpp::get_map_node_name()` → `ImgSource::ensure_map(mapid)` →
**把全局渲染根 `nl::nx::Map002` 重绑到被 hover 的地图 file，并经 LRU 淘汰最老的地图 file**。
而当前地图的渲染对象（`Stage.cpp:173`、`GraphicsGL.cpp:1640`）长期持有从旧 `Map002` 取出的
`nl::node`——这些 node 内部是**指向已删除 file 缓冲的裸指针**（`node.hpp:62-64` 官方注释：
file 删除后 node 失效且 `operator bool()` 检测不出来）。渲染继续使用悬垂 node →
**0xc0000374 堆损坏**（ntdll RtlFreeHeap）。

此前 `worldmap-crash-and-window-assets-2026-11` 加的三处 `mapid<=0` 守卫只挡住了 id=0，
**合法 mapid 一样触发重绑+淘汰**，所以没能根治——这正是本轮彻底修复的点。

## 2. 影响范围

| 层级 | 路径 | 变更 |
| --- | --- | --- |
| 客户端代码 | `Util/Resource/ImgSource.h/.cpp` | 新增查询专用 file 缓存 `ensure_map_query` + `s_query_files` |
| 客户端代码 | `Util/Misc.cpp` | 新增 `query_map_node`；`get_life_on_map`/`get_mobs_on_map` 改走隔离查询 |
| 服务端/协议/DB/资源 | — | 无 |

## 3. 最终实现逻辑

### 3.1 隔离原则（核心）

把"读任意地图数据"与"渲染当前地图"用**两套独立的 file 缓存**彻底隔离：

- **渲染路径**（`Stage` / `UIMiniMap::update()` / `GraphicsGL`）：继续用全局 `nl::nx::Map002` +
  `s_map_files`（LRU 32）。`UIMiniMap` 只查当前地图，`ensure_map` 命中即返回，不淘汰，安全。
- **查询路径**（世界地图 hover / 手册 mob 列表）：改用新的 `ensure_map_query(mapid)` +
  `s_query_files`（独立 LRU 32）。它自行读 `Data/Map/Map<MapPrefix>/<id9>.img`、
  `img_file::open_tree`，**只返回该 file 的 root node，绝不 bind 到 `RootTable::Map002`、
  绝不触碰 `s_map_files`**。查询方即时把 life/mob 字段拷贝成 std::string/数值，
  不长期持有 node（node 在查询缓存淘汰后才失效，渲染根不受影响）。

### 3.2 改动明细

1. `ImgSource.h`：
   - 新增 `static nl::node ensure_map_query(int32_t mapid)` 声明（注释标明用途与禁忌）。
   - 新增 `s_query_files` / `QUERY_MAP_KEEP=32`（与 `s_map_files` 隔离，共用 `s_load_mtx`）。
2. `ImgSource.cpp`：
   - 定义 `s_query_files`。
   - 实现 `ensure_map_query`：mapid<=0 早退；持锁查/建独立 file（路径候选与
     `load_single_map` 一致：Map/Map<prefix>/<id9>.img）；MRU 命中提到队尾；超 32 淘汰；
     返回 `root["Map"]["Map<prefix>"][<id9>.img]`。
   - `close()` 清理 `s_query_files`。
3. `Util/Misc.cpp`：
   - 新增 `static nl::node query_map_node(int32_t mapid)`（走 `ensure_map_query`）。
   - `get_life_on_map` / `get_mobs_on_map` 由 `get_map_node_name` 改用 `query_map_node`。
   - `get_map_node_name`（渲染用）保持不变，仅剩渲染调用方。

## 4. 资源与同步

- 无 WZ/IMG 改写、无多端资源同步、无 i18n/密钥/编码变更。

## 5. 验证

- 构建：`tools/build_smoke.bat` → `EXITCODE=0`（2026-11-21 12:48，GATE OK）。
- 部署 exe：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` 刷新，LastWriteTime 2026/11/21 12:48:09。
- lint：`ImgSource.h/.cpp`、`Misc.cpp` 无新增诊断。
- **实机压测**：auto_login 进图（FPS60 稳定），开世界地图后 12 点覆盖悬停 +
  18 次跨城市点来回快速悬停（模拟 LRU 高频淘汰触发场景），客户端全程存活、无闪退、
  在线时长持续增长。截图：`evidence/live-worldmap-*.png`。
- **崩溃证据**：`evidence/rca.md` 记录 0xc0000374 事件日志 + dump 分析 + 根因链。

## 6. 回滚与风险

- 回滚：把 `get_life_on_map`/`get_mobs_on_map` 的 `query_map_node` 改回 `get_map_node_name`
  （会恢复闪退，不推荐）；或删除 ImgSource 查询接口（需同步改回调用）。
- 风险：`ensure_map_query` 每次 hover 新地图会读盘+解压单图（有缓存，最多 32 张），
  与旧 `ensure_map` 开销相当；独立缓存不污染渲染根，是净改善。
- 注意：`query_map_node` 返回的 node 不得跨调用长期持有（查询方已即时拷贝文本）。

## 7. 关联

- 前序（未根治）：`docs/features/worldmap-crash-and-window-assets-2026-11/README.md`（三处 mapid<=0 守卫）。
- 前序：`docs/features/ui-v83-window-restore/evidence/w11-map/worldmap-anchor.md`（spot 坐标系）。
