# 第十二面 · orange-wz(IMG) 门禁实证与合规回退 (r03)

> 本面按 `AGENTS.md`（YG 2026-11-09，ACTIVE/MANDATORY）执行：**WZ/IMG/XML 一律先走 orange-wz MCP**，仅当服务端返回解析异常才回退 `.nx` dump + 运行时 probe，回退须记录原始报错文本与 key 规格。前十一面「orange-wz 未连接 / 阻塞 / 以 .nx 为准」为历史取证留存，不再约束当前顺序。

## 1. 连接实证（round50）

| 项 | 值 |
| --- | --- |
| 连接 key | `custom-orange_wz`（`mcp_connector_status` 已连接） |
| 端点 | `http://127.0.0.1:10012/mcp` |
| 传输 | `scope=global` / `serverName=orange_wz` / `streamable-http` / `authMode=none` |
| 工具集 | 29 tools（`mcp__orange_wz__*`） |
| serverInfo | `orange-wz-http-mcp v1.157.49` |
| header | `{"Accept":"application/json, text/event-stream"}` |
| 端点可达 | `orange-wz 10012 REACHABLE`；`Angel-083 13339 REACHABLE`；`BeiDou 13337 REACHABLE` |

## 2. key 规格（避免历史越界真因重演）

| 项 | 值 |
| --- | --- |
| `key.userKeyBase64` | `tools/_gms_key.b64` = `key_gms` **65536 字节完整 key 表**，b64_len 87384 / sha256_16 `268a8407a47ee76c` / b64_head `lq4/pEj63ZBGdgVhl854aCugRI/BVn4y/OH1sxQUxSL1w2gu` |
| `key.ivBase64` | `TSPHKw==`（GMS 4B `bytes([0x4D,0x23,0xC7,0x2B])`） |
| `key.name` | `GMS`；`load_files` 传 `exclusive:false` |
| 收口 | 同会话 `load_files → query/list/find → unload_all` → `rootCount=0` |
| `search_node` | 用 `keyword` 参数（传 `query` 报「缺少参数: keyword」） |

## 3. 结构层：WORKS

```
load_files(paths=["E:\MXD\BeiDou-Client-xy\Data\Item\Special"], exclusive=false)
  → {"loadedCount":1,"rootCount":1,"exclusive":false,"generation":2}
list_children(Data\Item\Special)
  → 0900.img / 0910.img / 0911.img / MaplePoint.img   （4 个真 .img）
```

## 4. 内容层：8 次全 `IMG 解析失败`（GBK 原文，UTF-8 误读呈乱码）

| # | 调用 | 返回 |
| --- | --- | --- |
| 1 | `get_node_tree_json nodePath="0900.img"` | `IMG 解析失败: 0900.img` |
| 2 | `search_node keyword="0900"` | `IMG 解析失败: 0900.img` |
| 3 | `get_node_tree_json nodePath="0910.img"` | `IMG 解析失败: 0910.img` |
| 4 | `search_node keyword="0910"` | `IMG 解析失败: 0910.img` |
| 5 | `get_node_tree_json nodePath="0911.img"` | `IMG 解析失败: 0911.img` |
| 6 | `search_node keyword="0911"` | `IMG 解析失败: 0911.img` |
| 7 | `get_node_tree_json nodePath="MaplePoint.img"` | `IMG 解析失败: MaplePoint.img` |
| 8 | `search_node keyword="MaplePoint"` | `IMG 解析失败: MaplePoint.img` |

收口 `unload_all` → `{"rootCount":0}`。汇总：`[STRUCT] Data/Item/Special top children: 4 parse_fails: 8`。

## 5. 合规回退：`.nxcache/Item.nx`

```
[NX] Item.nx root_children=6 sections=['Cash','Consume','Etc','Install','Pet','Special']
```

`Special` 下 4 子节点与结构层一一对应（`0900.img` child=4 / `0910.img` child=538 / `0911.img` child=5 / `MaplePoint.img` child=37）；`0900.img` 下 `09000000 / 09000001 / 09000002 / 09000003` 各 1 子节点 `iconRaw`（child=4）。

→ **`MapDrops::init()` 的四个 meso 图标路径在当前 shipped 包完整存在**，`if (!src) return;` 的降级分支在当前包不会触发。

## 6. 降级清单（F 样式维 12 项诚实 ⏳）

`.nx` 能给出**结构**（节点名、child 数）但不能给出**像素**（`iconRaw` 的 `width/height/format/PNG` 需 IMG 内容层）。故 F1–F12 全部标 ⏳，不以「结构存在」冒充「样式一致」——`no_fake_green`。

## 7. 复现

```bash
python tools/face12_gate_and_drop_probe.py
# 产出 evidence/orange-wz-drop-structure.json + evidence/nx-drop-content-probe.json
```
