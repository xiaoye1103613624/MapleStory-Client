# 第十面 地图(Map) — orange-wz IMG 门禁实证与合规回退

> **ACTIVE 规则（YG 2026-11-09，AGENTS.md / m03858「orz mcp已经注册了，img资源都要使用orzmcp」）**：
> 凡解析 / 查询 / 核对 WZ / IMG / XML 节点，**一律先调用全局已注册 orange-wz MCP**（`mcp__orange_wz__*`，`custom-orange_wz`，`http://127.0.0.1:10012/mcp`，29 工具）。
> 仅当服务端返回 `IMG 解析失败` / `Index N out of bounds for length N` / GBK 错误文本等**解析异常**时，才回退 `.nx` dump + 运行时 probe；回退须记录 orange-wz 原始报错文本与所用 key 规格。
> 历史 `docs/features/**/README.md` 中 D10/D13/D14「不接 / 阻塞 / 以 .nx dump 为准」均为 2026-11-09 前取证留存，**不再约束当前执行顺序**。

**本面是九面以来第一次真正调用 orange-wz MCP 并留下真实节点证据**——不再用「未连接」口径跳过 IMG 环。

## 1. 连接实证（不是猜的）

| 项 | 实测 | 来源 |
| --- | --- | --- |
| 端点 | `orange-wz 10012 REACHABLE` | socket 探测 |
| 连接 | `mcp_connector_status` 2 条：`json-photoshop`（128 工具）+ **`custom-orange_wz`（已连接，global / streamable-http / authMode=none）** | connector status |
| 健康 | `mcp_connector_health_check` 全量 2/2 正常，各 1/1 Server | health_check |
| 工具集 | `tools/list` = **29 tools**：`load_files / list_loaded_roots / unload_node / unload_all / clear_cache / create_wz_file / create_img_file / list_children / find_node / get_node_detail / get_node_tree_json / search_node / batch_find_nodes / query_nodes / create_child_node / delete_node / copy_nodes / paste_nodes / copy_paste_nodes / batch_update_nodes / mutate_nodes / list_dirty_roots / save_dirty_roots / analyze_resource_links / verify_canvas_formats / save_node / save_as / change_key / batch_convert_key` | `tools/orange_wz_schema_probe.py` |
| 初始根 | `mcp__orange_wz__list_loaded_roots` → `rootCount 0` | 会话起始 |
| 端点 | `Angel-083 13339 REACHABLE` / `BeiDou 13337 REACHABLE` | 同轮探测 |

`load_files` schema required `["paths","key"]`，key = `{name, ivBase64, userKeyBase64}`，`exclusive` 可选（按规则传 `false`）；`search_node` 必须用 `keyword`（传 `query` 返回 `缺少参数: keyword`）；`list_children / find_node / get_node_tree_json` 均带 `autoParse`。

## 2. key 规格（65536B 全表，避免历史越界真因重演）

| 项 | 值 |
| --- | --- |
| 来源 | `tools/vendor/NoLifeWzToNx-master/Keys.cpp` 的 `key_gms` / `key_bms`；`tools/_wz.py:32 return [grab("key_bms",65536), grab("key_gms",65536), grab("key_kms",0x20000)]` |
| GMS | `tools/_gms_key.b64` = 65536 B → `b64_len 87384` / `sha256_16 268a8407a47ee76c` / `b64_head lq4/pEj63ZBGdgVhl854aCugRI/BVn4y/OH1sxQUxSL1w2gu` |
| BMS | `tools/_bms_key.b64` = 65536 B → `b64_len 87384` / `sha256_16 de2f256064a0af79` |
| IV | `ivBase64 = TSPHKw==`（GMS 4B `[0x4D,0x23,0xC7,0x2B]`） |
| 反例 | 16B / 32B 零 key 触发 `Index 16 out of bounds for length 16` / `Index 32 out of bounds for length 32`（历史真因）；本面全程 65536B 全表，未触发 |
| 回退留证 | `evidence/orange-wz-gate-verdict.json` 的 `key_spec` 字段 |

因 65536B key 不能塞进模型上下文，P1–P4 全部走 `tools/orange_wz_*_probe.py` 自包含 JSON-RPC/SSE 桥（自己管 `Mcp-Session-Id`、容忍 `notifications/initialized` 空 body、解析 `data: ` 行）调用**同一个 orange-wz server**，等价 `mcp__orange_wz__*`。

## 3. P1–P4 四条本质不同路径（决定性闭环）

### P1 `tools/orange_wz_node_probe.py` — standalone `.img` 内容层
5 个 `.img`（`UI/StatusBar.img` / `UI/MapLogin.img` / `UI/UIWindow.img` / `smap.img` / `zmap.img`）：`load_files` OK、`list_children` 空、`query_nodes list` 空、`get_node_tree_json` / `search_node keyword=map` 一律 `IMG 解析失败`。
结果 `{'ok': 14, 'parse_fail': 10, 'total': 24}` → `evidence/orange-wz-node-probe.json`。
**严格 classify**：文本含 `IMG 解析失败 / out of bounds / Index / 缺少参数 / 失败 / Error` 判 `PARSE_FAIL`，**不记 OK**。

### P2 `tools/orange_wz_autoparse_probe.py` — 两假设排除
假设 A「懒解析未触发」/ 假设 B「key.name 规格不对」：单 `StatusBar.img` × 4 种 `key.name`（`GMS` / `gms` / `BMS` / `<empty>`）× 7 工具（`unload / load / list_children autoParse:true / find_node autoParse:true / get_node_tree_json autoParse:true / search_node keyword / query_nodes search autoParse:true`）。
GMS/gms/BMS 三者 `find_node` 只回 `{"node":{"name":"StatusBar.img","rootPath":"...","nodePath":"","type":"IMAGE"}}` 外壳、`children=[]`、tree/search 全 `IMG 解析失败: StatusBar.img`（9 PARSE_FAIL）；`<empty>` → `参数为空: name` + `找不到根节点`。
`{'OK': 17, 'PARSE_FAIL': 9, 'OK_EMPTY_NODES': 4}` → `evidence/orange-wz-autoparse-probe.json`。**结论：autoParse 与 key.name 两条假设排除**。

### P3 `tools/orange_wz_dir_nx_probe.py` — 目录整体 / BMS 全表 / 喂 .nx
1. **目录整体加载 `Data/UI` → `list_children` NODES `children=20`**（真 `.img` 子节点）；`Data` → `children=19`；`get_node_tree_json` 目录级出树 ✅ **结构层 WORKS**。
2. BMS 65536B 全表喂单 `StatusBar.img` / `smap.img` 仍 `IMG 解析失败`。
3. `.nx` PKG4 被 `load_files` 拒绝：`仅支持加载目录或 .wz/.img/.xml 文件: E:/MXD/BeiDou-Client-xy/Data/.nxcache/smap.nx`（smap/zmap/UI.nx 同）。
`{'NODES': 2, 'FAIL': 18, 'OK': 26, 'TEXT': 3}` → `evidence/orange-wz-dir-nx-probe.json`。

### P4 `tools/orange_wz_drilldown_probe.py` — 目录加载后下钻 `.img` 三形态
- `child_as_rootpath`（rootPath=Basic.img 全路径）→ `找不到根节点: E:\MXD\BeiDou-Client-xy\Data\UI\Basic.img`
- `child_as_nodepath`（rootPath=UI 目录 + nodePath=Basic.img）→ `find`=IMAGE 外壳、`children=[]`、tree/query_search → `IMG 解析失败: Basic.img`
- `child_as_nodepath_noext` → `节点路径不存在: rootPath=..., nodePath=Basic`
`{'OK': 7, 'NODES': 1, 'PARSE_FAIL': 6, 'TEXT': 4}` → `evidence/orange-wz-drilldown-probe.json`。

### 决定性结论

> **orange-wz「不是没接，是结构层能跑、内容层解析失败」。**
> 目录级枚举 NODES=20/19 成功 = WZ **结构层闭合**；任何 internal `.img` tree/search 一律 `IMG 解析失败`，`autoParse:true` + GMS↔BMS 65536B 全表 + 下钻三形态都救不回 → **正是 AGENTS.md 写死的回退触发条件**，原始报错 + key 规格已逐条留证。

## 4. 三份 evidence 固化 + 合规回退

`tools/face10_gate_and_map_probe.py` 写 `evidence/orange-wz-gate-verdict.json`：`rule / endpoint / connection(29 tools) / key_spec / P1–P4 phases / raw_error_texts_verbatim / conclusion`。

**原始报错文本逐字留证（`raw_error_texts_verbatim`）**：
```
IMG 解析失败: StatusBar.img
IMG 解析失败: Basic.img
IMG 解析失败: 01010000.img
IMG 解析失败: smap.img
IMG 解析失败: MapLogin.img
仅支持加载目录或 .wz/.img/.xml 文件: E:/MXD/BeiDou-Client-xy/Data/.nxcache/smap.nx
参数为空: name
找不到根节点: E:\MXD\BeiDou-Client-xy\Data\UI\Basic.img
节点路径不存在: rootPath=..., nodePath=Basic
缺少参数: keyword
```
（服务端错误文本为 GBK，UTF-8 解码会呈乱码，按原始字节留证。）

`conclusion`：
```
structure_layer: WORKS
content_layer: FAILS
fallback_condition_met: True
fallback_source: .nxcache/*.nx via tools/nx_inspect.py
fallback_recorded: True
no_fake_green: True
```

**`no_fake_green` 教训**：`tools/orange_wz_gate_smoke.py` v2 首跑打印 `steps 6 ok 6`，但脚本把 `IMG 解析失败: smap.img` 文本当成功了 → 假绿。已自我批判并改为严格 classify；此结论禁止放进第十面 F 维。标尺不严就是自嗨。

**结构层真实留证**（`evidence/orange-wz-map-structure.json`）：orange-wz 枚举 `Data/Map` → top `children=8` = `Back / Map / Obj / Tile / WorldMap / Effect.img / …`；子层 `Back`(Amoria.img…)、`Map`(Map0…)、`Obj`(acc1.img…)、`Tile`(allblackTile.img…)、`WorldMap`(WorldMap.img…)、`Effect.img`(children=[])。`unload_all` → `roots=[]` 收口，无根锁残留。

**合规 `.nx` 回退**（`evidence/nx-content-probe.json`，`tools/nx_inspect.py` 读 PKG4：头 `"PKG4"` + u32 node_count @4、string_count/table @0x10、bitmap_count @0x1C、bitmap_data @0x20；`class Nx::node(i)` off=64+i*20）：`Nx.children(0)` 顶层 = `smap.nx 147` / `zmap.nx 151` / `Map.nx 7` / `UI.nx 20` / `String.nx 20` 个 root children；smap/zmap 另存 `depth2_sample`（前 8 顶层各前 20 子节点 `name/type/value`）。

## 5. 资源盘点（本面实测，非推断）

- `E:/MXD/BeiDou-Client-xy/Data/.nxcache/` **20 个 .nx**：`Character 382136832B / Effect / Etc / Item 44491098B / Map.nx 1381106158B / Map001 / Map002 189997312B / Mob 2376113300B / Npc 192116843B / Quest / Reactor / Skill / Sound / String 8694112B / UI.nx 86919577B / smap.nx 6720B / zmap.nx 6704B / StandardPDD / TamingMob`。
- `Data/UI/*.img` 21 个（StatusBar / Basic / Login / UIWindow / CashShop / MapleTV / MapLogin 等）；`Data/Map/{Back,Map,Obj,Tile,WorldMap}/*.img` 2589+（ops_glob `scanComplete=false` matchedObserved 2589）。
- **全客户端树 0 个 `.wz`、0 个 `.xml`、20 个 `.nx`** → orange-wz 的 `.img` 内容层失败不是「没有资源」，是这些 `.img` 已被转 PKG4 预烘焙，目录层只留外壳。
- `.img` 头 16B：`73f86c77fc798327195800001200fc5e`(StatusBar) / `73f86c77fc7983271958000080930000`(smap)。

## 6. F 维 12 项降级清单（⏳ → P2 真机）

| # | 项 | orange-wz 状态 | 回退证据 | 判定 |
| --- | --- | --- | --- | --- |
| F1 | `UI/MapLogin.img` 登录地图底图 | `IMG 解析失败` | `Map.nx` root children=7 | ⏳ |
| F2 | `UI/UIWindow.img` 世界地图框 | `IMG 解析失败` | `UI.nx` root children=20 | ⏳ |
| F3 | `Data/Map/WorldMap/WorldMap.img` 分区贴图 | `IMG 解析失败` | `Map.nx` 子层 | ⏳ |
| F4 | `Data/Map/Back/*` 背景层 | `IMG 解析失败` | `Map.nx` Back 子层 | ⏳ |
| F5 | `Data/Map/Obj/*` 物件层 | `IMG 解析失败` | `Map.nx` Obj 子层 | ⏳ |
| F6 | `Data/Map/Tile/*` 瓦片层 | `IMG 解析失败` | `Map.nx` Tile 子层 | ⏳ |
| F7 | `smap.img` 小地图裁剪图 | `IMG 解析失败` | `smap.nx` 147 children + depth2 | ⏳ |
| F8 | `zmap.img` 小地图缩放图 | `IMG 解析失败` | `zmap.nx` 151 children + depth2 | ⏳ |
| F9 | `String.nx` 地图名 / `SP_267` 提示串 | `IMG 解析失败` | `String.nx` 20 children | ⏳ |
| F10 | `MapleTV` 双屏贴图（第九面交叉） | `IMG 解析失败` | `UI.nx` / 源码常量 | ⏳ |
| F11 | `UIWindow.img/Skill/skillPoint`（第八/九面 NOT FOUND 延续） | `IMG 解析失败` | 源码 `send_spup` 降级 | ⏳ |
| F12 | 正确端截图逐像素（`C:\Users\11036\Desktop\正确客户端截图\`） | 非 orange-wz 范围 | 体感输入，不冒充节点证据 | ⏳ |

**P2 恢复条件**：orange-wz 服务端 internal `.img` 内容层解析恢复（或改用可解析的 `.wz` 原包）→ 逐项重跑 `load_files → get_node_tree_json → verify_canvas_formats → 像素比对`，把 12 项 ⏳ 翻 ✅。当前不做 live 改写，不 `save_as`，不动 `.img`。

## 7. 纪律

- **不再写**「orange-wz 未连接 / 阻塞」口径；本面 = 已连接 + 结构层真实节点 + 内容层原文报错 + 合规回退。
- **095 禁引**：`docs/features/port-095/**` 与 `Maplestory95.i64` 相关只作历史注记。
- **全程只读**：未改 WZ/IMG/XML、未改游戏源码/协议、未建提交；`unload_all` 已收口。
- live 改写（若将来 P1 裁决要做）须走 AGENTS.md 安全流：关客户端 → staging 复制 → load → mutate → `save_as` 到**不同** output → `unload_all` → 原子替换；禁止回写同路径、禁止裸拷 `.img`。
