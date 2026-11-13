# 第十一面 · orange-wz(IMG) 门禁实证与合规回退 (r03)

> 本面按 `AGENTS.md`（YG 2026-11-09，ACTIVE/MANDATORY）执行：**WZ/IMG/XML 一律先走 orange-wz MCP**，仅当服务端返回解析异常才回退 `.nx` dump + 运行时 probe，回退须记录原始报错文本与 key 规格。前九面「orange-wz 未连接 / 阻塞 / 以 .nx 为准」为历史取证留存，不再约束当前顺序。

## 1. 连接实证（round33）

| 项 | 值 |
| --- | --- |
| 连接 key | `custom-orange_wz`（`mcp_connector_status` 已连接） |
| 端点 | `http://127.0.0.1:10012/mcp` |
| 传输 | `scope=global` / `serverName=orange_wz` / `streamable-http` / `authMode=none` |
| 工具集 | 29 tools（`mcp__orange_wz__*`），`tools/list` 实证 |
| header | `{"Accept":"application/json, text/event-stream"}` |
| 端点可达 | `orange-wz 10012 REACHABLE`；`Angel-083 13339 REACHABLE`；`BeiDou 13337 REACHABLE` |
| 全量 health_check | 2/2 正常（`json-photoshop` 128 工具 + `custom-orange_wz` 1/1 Server） |

## 2. key 规格（避免历史越界真因重演）

| 项 | 值 |
| --- | --- |
| `key.userKeyBase64` | `tools/_gms_key.b64` = `key_gms` **65536 字节完整 key 表**（来源 `tools/vendor/NoLifeWzToNx-master/Keys.cpp`），b64_len 87384 / sha256_16 `268a8407a47ee76c` / b64_head `lq4/pEj63ZBGdgVhl854aCugRI/BVn4y/OH1sxQUxSL1w2gu` |
| `key.ivBase64` | `TSPHKw==`（GMS 4B `bytes([0x4D,0x23,0xC7,0x2B])`） |
| `key.name` | `GMS`；`load_files` 传 `exclusive:false` |
| 收口 | 同会话 `load_files → query/list/find → unload_all` → `roots=[]` |
| `search_node` | 用 `keyword` 参数（传 `query` 报「缺少参数: keyword」） |

> 16B / 32B 零 key 会触发 `Index 16 out of bounds for length 16` / `Index 32 out of bounds for length 32`；本面用全表，未复现该越界。

## 3. 本面实证（`tools/face11_gate_and_quest_probe.py`）

脚本从第十面 `tools/face10_gate_and_map_probe.py:1-253` 派生的自包含 JSON-RPC/SSE 桥（`post()` 容忍空 body、解析 `data: ` 行、`init()` 管 `Mcp-Session-Id`），严格 `classify()`：含 `IMG 解析失败 / out of bounds / Index / 缺少参数 / 失败 / Error` 判 `PARSE_FAIL`，**禁止把报错文本计为 OK**（第十面 `no_fake_green` 教训）。

**结构层 WORKS（真实 orange-wz 节点）**：
- `load_Data_Quest` → OK；`Quest_top_children` → **6 个真 `.img` 子节点**：`Act.img` / `Check.img` / `Exclusive.img` / `PQuest.img` / `QuestInfo.img` / `Say.img`。写入 `evidence/orange-wz-quest-structure.json`。
- 与第十面 `Data/Map children=8`、`Data/UI children=20`、`Data children=19` 同构 → 目录级枚举是 orange-wz 稳定能力。

**内容层 FAILS（回退触发条件命中）**：
- 6 个 `.img` 各跑 `descend_*.img`（`get_node_tree_json`）与 `search_*.img`（`search_node keyword`）→ **12 次全 `IMG 解析失败`**，原始报错文本逐字：`IMG 解析失败: Act.img` / `IMG 解析失败: Check.img` / `IMG 解析失败: Exclusive.img` / `IMG 解析失败: PQuest.img` / `IMG 解析失败: QuestInfo.img` / `IMG 解析失败: Say.img`。
- 这正是 `AGENTS.md` §2 写死的回退条件。第十面已排除的三条救不回路径（P2 `autoParse:true` + key.name GMS/gms/BMS 全试、P3 目录整体加载 + BMS 全表 + 喂 `.nx` 被拒、P4 下钻三形态）本面不重复无效实验，直接按规则回退。
- `unload_all` → `roots=[]`，无根锁残留。

## 4. 合规 `.nx` 回退（`evidence/nx-quest-content-probe.json`）

| 项 | 值 |
| --- | --- |
| 回退源 | `E:/MXD/BeiDou-Client-xy/Data/.nxcache/Quest.nx`（7092544 B，PKG4） |
| 读取器 | `tools/nx_inspect.py`（头 `"PKG4"` + u32 node_count @4，node 20B，`Nx.children(0)`） |
| 结果 | `Quest.nx root_children=6 / sections=6`，与 orange-wz 结构层 6 个 `.img` 子节点一一对应 |
| 裸 `.img` 盘点 | `Data/Quest/`：QuestInfo.img 1211640B / Say.img 2354999B / Check.img 695814B / Act.img 647228B / PQuest.img 6521B / Exclusive.img 173B |

## 5. F 样式维 12 项降级清单（全部 ⏳，P2 恢复条件）

| # | 校准点 | 083 语义来源 | 当前端 | 状态 |
| --- | --- | --- | --- | --- |
| F1 | `QuestInfo.img/<id>/{0,1,2…}` 分状态摘要页 | `CQuestMan::LoadQuestInfo` | `Data/QuestData.cpp:129-208` | ⏳ 内容层未恢复 |
| F2 | 连锁任务 `"%s ( %d / %d )"` 页码 | IDA `sub_83EA05` / `sub_83FEC6` | `UIQuestLog::draw_chain_page_gauge()` | ⏳ |
| F3 | `UI/UIWindow.img/Quest/Gauge/{spot,gauge,frame}` | REA r17 字符串锚点 | `tab_fill_active[3]` / gauge 三态 | ⏳ |
| F4 | `Quest/Gauge2/{gauge,frame}` | REA r17 | 三态 gauge | ⏳ |
| F5 | `Etc/QuestCategory.img` 分类图标 | REA r17 | `area_name` 用 `Locale::text("quest.area.20"/"30")` | ⏳ |
| F6 | 任务告示 `CUIQuestAlarm` 版式 | `CUIQuestAlarm` 6 方法 | `r240-questalarm-softkey.md` | ⏳ |
| F7 | 勋章任务 `CUIMedalQuestInfo` 10 方法 | Angel 具名 | 无对应独立窗 | ⏳ |
| F8 | `CNoticeQuestProgress::Update` 进度浮层 | `0x6cec47` 32B identical | 无对应 | ⏳ |
| F9 | `CUIQuestInfo` / `CUIQuestInfoDetail` 15+4 方法 | Angel 具名 | `UIQuestLog` 合并窗 | ⏳ |
| F10 | `PQuest.img` / `Exclusive.img` 数据族 | Angel `LoadPartyQuestInfo` | 无 party quest UI | ⏳ |
| F11 | 多行任务图标换行 | `r55-multiline-questicon.md` | `UIMultiLine` | ⏳ |
| F12 | 正确端截图逐像素对齐 | `C:\Users\11036\Desktop\正确客户端截图\` | 待真机 | ⏳ |

> 恢复条件：orange-wz 内容层 `IMG 解析失败` 消除（internal `.img` tree/search 可返回节点）或真机截图对照；在此之前 12 项一律诚实 ⏳，不以 `.nx` 结构名冒充像素级样式结论。

## 6. 结论与纪律

- **本面 orange-wz 门禁真闭合**：结构层 WORKS（6 真节点）+ 内容层 FAILS（`IMG 解析失败` 原文 12 次）+ key 规格留证 + 合规 `.nx` 回退。未写「未连接/阻塞」旧口径。
- `no_fake_green`：报错文本不计 OK；`parse_fails=12` 如实入账。
- 095 禁引：`docs/features/port-095/**` 与 `Maplestory95.i64` 不作 083 依据。
- 全程只读：未改游戏源码/协议/WZ/IMG/XML，未建提交；live 改写安全流（关客户端→staging→`save_as` 到不同 output→`unload_all`→原子替换）本面未触发（P0 纯文档）。
