# 世界地图四合一修复（tooltip / 信息面板数据空 + 怪物名搜索 + 结果列表排版）

> 状态：根因定位 + 全量构建 + 部署 live + 短启无闪退；**真机交互待用户截图验收**。
> 日期：2026-11-21 · 分支 `feature/dev_0.0.1`
> 部署：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` SHA256 `63d2a354a854fb4229d9e0432682e8cda1225bb12f63d08f6314897c58fe141c`（5192192 B，17:19:36）

## 1. 用户反馈（4 项）

1. 鼠标移入世界地图点位不显示 tooltip 怪物/NPC 信息（只剩地图名标题）。
2. 点地图点位 / 右侧搜索出结果后，左侧「地图信息」恒为 `No monsters on this map`。
3. 右侧搜索无法搜索怪物名称。
4. 搜索结果列表排版错、偏上。

截图 `XiaoYe_2026-11-21_14-03-49.png`（PixPin 原图 14:03:49，sha256 `f2ab9a3a…cfd4`）已确认是本端当前部署版：exe mtime/构建日志对得上，非旧进程错配。

## 2. 根因（行号级）

### ①② 共用根因：查询地图数据走错缓存（11-21 闪退根治的副作用）

- 11-21 早间 `docs/features/worldmap-hover-crash-rootfix-2026-11/README.md` 为根治 `0xc0000374` 堆损坏，把世界地图 hover 读 life 从 `get_map_node_name()`（重绑全局 `Map002` → 悬垂 node）改为 `query_map_node() → ImgSource::ensure_map_query()`（`Util/Misc.cpp:263-268/366-370`，`Util/Resource/ImgSource.cpp:555+`，走 `s_dirs["Map"]` 读裸 `.img`）。
- **根因**：live 客户端 `xy-config.yaml:71 nxcache: true`，走 `.nxcache` mmap 后端（`Backends.cpp:169-186`），`ImgBackend::open()` 在 `via_cache_=true` 时**直接 return，不调用 `ImgSource::init()`** → `s_dirs["Map"]` 恒空 → `ensure_map_query()` 在 `ImgSource.cpp`（`auto it = s_dirs.find("Map"); if (it == end) return {};`）恒返回空节点。
- 结果：`get_life_on_map`（tooltip 怪物/NPC）与 `get_mobs_on_map`（左侧信息面板 rebuild_mobs）双双拿不到数据 → 只剩标题 / `No monsters on this map`。**闪退修好了，数据也一起丢了**，而该闪退修复的验收只测「不崩」，未测「数据还在」。

证据（.nx dump，`tools/nx_inspect.py` 只读）：`Map002.nx` 的 `Map/Map1/100040100.img/life` 有 40 个 life 节点（type=m），`String/Mob.img/210100/name` 存在；数据完整，纯粹是查询路径拿不到。

### ③ 搜索索引根本没有怪物名

- `UIWorldMap.cpp:736-764 submit_native_search()` 唯一索引源 = `get_map_search_index()`。
- `Util/Misc.cpp get_map_search_index()` 只遍历 `String["Map.img"]`，只取 `mapName`/`streetName` → 只能搜地图名/街名。输「蜗牛」能出结果是因为地图名叫「蜗牛花园/小蜗牛」等，**不是**怪物「蜗牛」本身。

### ④ 结果列表坐标全硬编码

- `UIWorldMap.h`：原 `NATIVE_ROW_H=16`、`NATIVE_ROW_MAX=20`。
- `UIWorldMap.cpp get_native_result_origin()` 写死 `position + background_dimensions + Point(16, 52)`，与搜索轨白框（实测 backgrnd 147×537、白框 x≈8..137 / y≈18..525）无关 → 列表从窗口顶 +52 起排，压住/贴近标题区、行距与 13px 原生行不匹配。

## 3. 修复（点到面，一次成型）

| 层 | 文件 | 改动 |
|---|---|---|
| 数据源 | `Util/Resource/ImgSource.cpp` `ensure_map_query()` | 增加**预绑定后端只读分支**：`RootTable::root(RootId::Map002)` 已绑全图（mmap/wz/nx），直接只读查 `["Map"]["Map"+prefix][id9+".img"]`；**不调用 ensure_map 重绑、不淘汰 s_map_files**（0xc0000374 根因不回退）。堆式 img 后端预绑未命中时仍走原隔离缓存。 |
| 索引 | `Util/Misc.h/.cpp` | `MapSearchEntry` 加 `kind`(0图/1怪/2NPC)+`map_of`；`get_map_search_index()` 追加 `String/Mob.img`、`String/Npc.img` 名字条目；新增 `find_map_of_life()`（首次单次扫 Map002 建 `(kind,id)->map` 反查表，后续 O(1)）。 |
| 搜索交互 | `IO/UITypes/UIWorldMap.cpp/.h` | `submit_native_search()` 按 kind 生成带「(怪物)/(NPC)」标注的结果行；`on_native_result_selected()` 点地图→选点，点怪物/NPC→反查栖地图、跳分区+选中+联动左侧信息面板拉掉落；结果行图标按 kind 取 resultField/Mob/Npc。 |
| 排版 | `IO/UITypes/UIWorldMap.cpp/.h` | 行高 15（原生 mouseOverBase 13+行距）、`native_row_max()` 由白框区 y=44..520 算出可见行数（不再硬编码 20）、结果原点改 `(10, 44)` 落进白框内、图标/文字偏移按 13px 行校正。 |
| i18n | `locale/zh-CN.ini` / `en-US.ini` | `uiworldmap.search_life_nowhere`（怪物/NPC 无栖地图提示）。 |

## 4. 布局校验（maplestory-ui-layout-qa）

| ID | 规则 | 期望 | 实测 | 判定 |
|---|---|---|---|---|
| L1 | 结果行落在白框内容区 | x≈8..137 / y≈18..525 | origin(10,44)+行高15，宽≤104 → 全落于白框 | PASS |
| L2 | 不压输入框/标题带 | 起始 y≥44（输入行~25/按钮~24 之下） | 44 | PASS |
| L3 | 行数不溢出底边 | (520-44)/15 ≈ 31 行 | native_row_max() 算出并截断 | PASS |
| L4 | 行距一致 | 15px 等距 | shown*15 | PASS |
| L5 | 原生行尺寸 | mouseOverBase 13px | 13px 贴图+2px 行距 | PASS |

## 5. 验证

- **隔离构建**（避开并行会话对 live 的写竞争）：`MSBUILD /p:OutDir=build-wm4fix-verify\` → `MSBUILD_RC=0`，error=0，产物 `build-wm4fix-verify/MapleStory.exe` SHA `63d2a354…e141c`，locale 含新 key。
- **部署**：live 正式 exe 直接覆盖，实时核对 SHA == 产物 SHA；locale 同步。
- **短启**：`tools/_ab_launch.ps1 -Wait 12` → `RESULT=ALIVE`（未触发 0xc0000374 堆损坏）。
- **未做**：真机交互（悬停/点选/搜怪物/列表观感）待用户截图验收，截图须本端。

## 6. 回滚与风险

- 回滚：替换前 live exe 已被并行会话移走（无当日原版 bak），可用 `build-wm4fix-verify/` 之外的历史 bak（如 `MapleStory.exe.bak_before_diffweapon_20261121`，14:46）临时回退——但那版不含本修复，会复现 4 项问题。
- 风险：`find_map_of_life()` 首次调用单次扫全图（Map002 8M 节点）建索引，预计 1 秒内一次性完成，后续 O(1)；已加 memo + 一次性 built 标志，避免每次点击重扫。
- 并行会话：本轮多次遭遇并行构建写同一 live 路径导致 exe 短暂消失、build EXITCODE=-1；本修复最终以隔离 OutDir 构建 + 显式 cp + SHA 核对收口。

## 7. 关联

- 前序闪退根治：`docs/features/worldmap-hover-crash-rootfix-2026-11/README.md`（本轮修其数据丢失副作用，不回退其闪退修复）。
- 192 搜索/三分区：`docs/features/worldmap-192-search/README.md`。
- 左侧信息面板：`docs/features/map-info-panel/`。
