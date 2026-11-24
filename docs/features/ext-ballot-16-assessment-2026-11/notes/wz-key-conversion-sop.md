# 阶段一：WZ/IMG 资源转 key 流水线 SOP（orange-wz MCP）

> 建立：2026-11-18。基于 orange-wz v1.157.49 实机握手 + 29 工具签名实取 + WzAESConstant.java 源码。
> 适用：扩展改动 16 项中 5 类 A 资源（动漫皮肤/阿尔泰皮肤/卡片名称/083时装/NameTag）的导入。
> 硬门禁：跨端/跨 IV 资源 **禁止裸拷**（Copy-Item/robocopy/拖拽），一律走本流水线。

## 0. 密钥规格（已实核，严禁混用）

| 路径 | key 文件 | 规格 | 用途 |
|---|---|---|---|
| orange-wz MCP（主路径） | `tools/_gms_key_128.b64` | 128B DEFAULT_KEY 的 base64（172 字符，头 `EwAAAFIAAAAq...`） | MCP 的 `load_files`/`create_img_file` 的 `userKeyBase64` |
| `.nx`/nlnx 回退 | `tools/_gms_key.b64` | 65536B NoLife 全量表（87384 字符，头 `lq4/pEj63Z...`） | 仅当 MCP 报 `IMG 解析失败`/`Index N out of bounds`/GBK 乱码时回退 |

- IV：GMS `bytes([0x4D,0x23,0xC7,0x2B])` = base64 `TSPHKw==`（源码 `WzAESConstant.java:6` 实核）。
- 仅当 MCP 路径解析异常才用 65536B；两条路径 keystream 派生不同，互换必失败。
- 16B/32B 零 key 会触发 `Index 16/32 out of bounds`——不要用。

## 1. MCP 连接（本会话工具未注入时的 HTTP 直连法）

端点 `http://127.0.0.1:10012/mcp`（`mcp-runtime/endpoint.json` 实核），streamable-http：
1. `POST initialize`（body 见下）→ 响应头取 `Mcp-Session-Id`
2. 后续所有请求带 `Content-Type: application/json` + `Accept: application/json, text/event-stream` + `Mcp-Session-Id: <id>`
3. `POST tools/list` 取工具清单；`tools/call` 调用具体工具

```json
{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"probe","version":"1.0"}}}
```
> 注意：服务端返回 `protocolVersion: 2025-03-26`，以其为准。缺省 session 头会报 `缺少 Mcp-Session-Id 请求头`(-32001)。

## 2. 关键工具签名（v1.157.49 实取）

| 工具 | required | 关键 props |
|---|---|---|
| `load_files` | paths, key | exclusive（默认 true，跨会话写同路径须 false） |
| `create_img_file` | fileName, key | — |
| `query_nodes` | — | op, rootPath, nodePath, keyword, searchIn, type, maxDepth, autoParse |
| `copy_paste_nodes` | sources, targets | strategy, clearClipboard, autoParse, releaseSourceCache |
| `mutate_nodes` | — | operations, rootPath, nodePath, op, value, base64Png, continueOnError |
| `save_as` | rootPath, filePath | clearCache, autoParse, unloadAfterSave, nodePath |
| `change_key` | rootPath, key | save, verify, version |
| `batch_convert_key` | outputDir, sourceKey, targetKey | paths, sourceDir, sourceRoot, parallelism, overwrite, verify, targetVersion |

## 3. 标准流水线（每次导入必走）

```text
前置：关闭客户端（3 进程运行时禁止动 live，本次探测到 PID 26292/36840/42860）
  ↓
0. 备份 live 目标文件（复制 full 到 staging）
  ↓
1. load_files(源路径, key=源规格, exclusive:false)
   —— 源是 GMS083：userKeyBase64=128B, ivBase64=TSPHKw==
  ↓
2. query_nodes / find_node 取证（核对节点存在、UOL、帧序）；只改 Item 时须 analyze_resource_links 同步 String/Character
  ↓
3. create_img_file / copy_paste_nodes / mutate_nodes（按需要；异 key 粘贴建议先与源同 key）
  ↓
4. save_as 到 **异于 loaded 根** 的 staging 路径（禁止 save_as 回写 loaded 根）
  ↓
5. batch_convert_key(sourceKey=GMS083, targetKey=目标规格, outputDir=独立目录, verify:true)
   —— change_key 单用不可信（实测可 verified:true 但落盘按源 IV）；必须 batch_convert_key + verify
  ↓
6. save_as 最终产物（异路径）→ unload_all
  ↓
7. 原子替换进 live（客户端未占用）
  ↓
8. --resource-probe / 短启验收：目标 .img 须出现在 UI/Character 根，进程不 0xc0000374 堆损坏
  ↓
9. SHA256 留证（源 / staging / 最终 / live 四处）
```

## 4. 5 类资源包精确清点（2026-11-18 只读）

| 包 | 文件构成 | 路径特征 | 初步结论 |
|---|---|---|---|
| 动漫皮肤 | 96 img + 1 xml | `Character/Shoes/01076xxx.img` 等，Character 部件 | 角色模型部件（非仅图标）；须完整导入否则缺图 |
| 111款阿尔泰皮肤 | 112 img + 113 xml + 3js + 1pdf | `客户端/Data/Character/Cap/010089xx.img` | 带客户端/服务端(WZ XML)双份，GMS083 适配；量最大 |
| 卡片名称 | Ins.img + Ins.img.xml（1对） | 根级 | **实测特殊**：Ins.img 头 `73f851bc8ad8ac1093c2`，与 GMS 标准头 `73f86c77fc7983271958`（动漫/111款皮肤）不同；用 128B GMS key `load_files` 成功但 `get_node_tree_json` 报 `IMG 解析失败: Ins.img`，isError:true。**该包数据是明文 XML**（Ins.img.xml 含 name/desc 如"休闲椅"）。需单独定 key 规格或直接从 XML 走服务端 String/Ins 导入，禁止用 GMS key 硬套 |
| 修复NameTag错位 | NameTag.img（5.6MB，单文件） | 根级 | 纯资源替换；须先确认当前端 NameTag 现状再换 |
| 适用于083时装 | 15 wz + 说明.txt | PKG1 签名（`50 4b 47 31`），国服218道具 | wz 整包，走 create_wz_file/batch_convert_key；含高属性GM装备 |

## 4.5 全量加密规格分布（2026-11-18 一次性全扫，210 img）
| 包 | img 数 | 头字节(前10B) | 规格判定 |
|---|---|---|---|
| 动漫皮肤 | 96 | `73f86c77fc7983271958` | GMS 标准 |
| 111款阿尔泰皮肤 | 112 | `73f86c77fc7983271958` | GMS 标准 |
| 修复NameTag错位 | 1 | `73f86c77fc7983271958` | GMS 标准 |
| 卡片名称 Ins.img | 1 | `73f851bc8ad8ac1093c2` | **唯一非标准特例** |

**结论：卡片名称是全部 210 个 img 中唯一的例外**，其余 209 个统一走 128B GMS key 标准流水线，无需逐包重验。

## 4.6 第 5 类：PKG1 wz 容器实测（2026-11-18）
- `适用于083时装/` 的 15 个 `.wz` 是 **PKG1 "Package file v1.0" 容器**（头 `504b4731` + ASCII `Package file v1.`），与前 4 类裸 img 不同类，是完整 wz 容器。
- 实测：最小样本 `gms083全部的名字.wz`(1.8MB) 用同一套 128B GMS key + `TSPHKw==` → `load_files` loadedCount:1、`get_node_tree_json` isError:false，解析出 `Eqp.img`(IMAGE)→`Eqp`(LIST_PROPERTY) 真实节点。
- 结论：**PKG1 wz 容器同样走本流水线，无需单独 key**。导入应整包 load → 按需 query/copy_paste → `batch_convert_key` → save_as 异路径，而非逐 img 拆。
- 注意：这批 wz 含 `gms083武器-1.wz` 达 1.08GB，批量转 key 须分片 + 监控内存，禁止一次性全 load。

## 4.7 live 目标锁定（2026-11-18）
- **当前仓库 `MapleStory-Client/Data` 是 C++ 源码目录**（EquipData.cpp/ItemData.cpp…），不是游戏资源目录——SOP 里"替换回 live"的模糊说法在此澄清。
- **真正的 live 资源目录 = `E:\MXD\BeiDou-Client-xy\Data`**（Character/Effect/Item/UI/String/Map…），这是阶段一导入的唯一目标根。
- 目标节点现状（实测）：`Character/Shoes/01076000.img` 不存在（新增）；`Character/Cap/01008900.img` 已存在（覆盖，须先备份）；`String/Eqp.img` 存在；`Effect/ChatEmoticon.img` 存在。
- 进程占用实测修正：3 个 MapleStory PID（26292/36840/42860）的 Path/首模块均为空（疑似僵尸/系统占位进程）；实测只读打开 `Data/Character/Cap/01008900.img` 成功 → **live 资源文件未被独占锁定**。前几轮"3进程=硬阻塞"的判断修正为：**真实阻塞是缺 live 改写授权，而非文件占用**。

## 5. 本轮状态与阻塞

- orange-wz 为全局注册 MCP（端点 127.0.0.1:10012）；正常应自动带 `mcp__orange_wz__*` 工具，未带时跑 `E:\project\orange-wz\start-mcp.bat` 拉起，不要以"工具没暴露"为由跳过资源校验。本会话采用 HTTP 直连握手（initialize→session→tools/call）完成全部实测，等价于 MCP 调用。
- ~~客户端 3 进程运行中 → live 改写阻塞~~（2026-11-18 修正，见 §4.7：文件实测未被锁定，真实阻塞=缺 live 改写授权）。执行第 7 步前仍须用户明确授权改 `E:\MXD\BeiDou-Client-xy\Data`，并先确认无真实游戏进程占用。
- **实测验证（2026-11-18）**：
  - ✅ **标准 GMS 包主干可行**：`动漫皮肤/Character/Shoes/01076000.img`（头 `73f86c77…`）用 128B GMS key + IV `TSPHKw==`，`load_files`→`get_node_tree_json` 返回真实节点树（`walk1/0/default1` = `CANVAS_PROPERTY` 98×84 ARGB4444），`search_node(keyword="info")` 命中 `info` 节点，全程 isError:false。**证明标准 GMS 资源走本流水线可完整解析、可导入。**
  - ⚠️ **特殊包例外**：`卡片名称/Ins.img` 同 key `load_files` → loadedCount:1 但 `get_node_tree_json` → `IMG 解析失败: Ins.img` isError:true。根因=该 img 非标准 GMS083 加密头（见 §4 卡片名称行）。**证明"load 成功≠可解析"，流水线第 2 步 query 取证不可跳过。**
  - ✅ **最大最复杂包通过**：`111款阿尔泰皮肤/客户端/Data/Character/Cap/01008900.img`（112 img 的双份资源包）128B key 下 load + get_node_tree_json 均 isError:false，返回 47760 字符完整节点树（`info/reqJob`/`info/reqLevel` 等）。
  - 工具参数细节：`list_children` 对 IMAGE 根返回空 children（节点要穿透用 `get_node_tree_json`）；`query_nodes op=search` 会报参数错误，搜索请用独立 `search_node(keyword=, rootPath=)`。
- 本 SOP 已落盘；实际导入未动 live（无授权 + 占用 + 卡片名称包规格待定），待用户关闭客户端并确认后执行。

## 6. 红线复述（防重蹈 StatusBar3 覆辙）

- 首次 StatusBar3 从 192 零 IV 源 `create_img_file(GMS)+copy_paste+save_as` 未显式 `batch_convert_key` → 堆损坏 0xc0000374，probe 不见节点。
- 结论：跨端流程**必须**显式转 key + 本端 probe/短启，不得宣称"已入库可测"。
