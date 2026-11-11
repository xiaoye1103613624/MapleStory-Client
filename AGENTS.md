# 仓库级 Agent 规范：WZ / IMG 资源优先 orange-wz

适用：本仓库（E:\project\MapleStory-Client）内所有涉及 `.wz` / `.img` / `.xml` 资源读取、节点查询、资源改写的 Agent 会话。

## 1. 当前生效规则（YG 2026-11-09 指令）

1. 凡需解析、查询、核对、改写 WZ / IMG / XML 节点，**一律先调用全局已注册的 orange-wz MCP**：工具前缀 `mcp__orange_wz__*`，DSH 全局连接 `custom-orange_wz`，端点 `http://127.0.0.1:10012/mcp`，工具集 29 个。
2. 该连接已由 `mcp_connector_configure` 注册为 `scope=global`、`serverName=orange_wz`、`transport=streamable-http`、`authMode=none`、`headersJson={"Accept":"application/json, text/event-stream"}`；`mcp_connector_status` 与全量 `mcp_connector_health_check` 均显示已连接且 1/1 Server 可用。
3. 与 skill `xiaoye-maple-story-dev` 门禁 8「WZ/IMG/XML 走 MCP：优先 orange-wz MCP」、门禁 18「REA → IDA → orange-wz(IMG)」及 `C:\Users\11036\.agents\skills\xiaoye-MapleStory-dev\toolchain.md:363-418` §6 一致。

## 2. 历史文档口径覆盖

`docs/features/**/README.md` 中 64 处 orange-wz 相关记录、以及 D10 / D13 / D14 等「不接 / 阻塞 / 以 .nx dump 为准」结论，均为 2026-11-09 之前的历史取证记录，仅作证据链留存，**不再约束当前执行顺序**。当前顺序固定为：

```text
orange-wz MCP → （仅当服务端返回 `IMG 解析失败` / `Index N out of bounds for length N` / GBK 错误文本等解析异常时）→ `.nx` dump + 运行时 probe
```

回退时须在结论中记录 orange-wz 的原始报错文本与所用 key 规格。

## 3. 调用要点（避免历史越界真因重演）

- **`key.userKeyBase64` 按路径分两套规格（round85 实测翻案，务必区分）**：
  - **orange-wz MCP 路径**（本文件 §1/§2 主路径）：必须传 **128 字节** `WzAESConstant.DEFAULT_KEY` 的 base64（`E:/project/orange-wz/src/main/java/orange/wz/provider/WzAESConstant.java:14-23`；落盘 `tools/_gms_key_128.b64`，`B64_LEN 172 HEAD EwAAAFIAAAAqAAAAWwAAAAgA`）。原因：`E:/project/orange-wz/src/main/java/orange/wz/provider/tools/WzMutableKey.java:84-90` `getTrimmedUserKey()` 逐字 `byte[] key = new byte[32]; for (int i = 0; i < 128; i += 16) { key[i / 4] = userKey[i]; } return key;` —— 只吃前 128B、每 16B 取 1B 拼 32B AES keystream。传 65536B NoLife 全量表会因前 128B 布局不同导致 keystream 全错 → `WzImageFile.parse()` 抛 `IMG 解析失败: UIWindow.img`（抛出点 `DefaultMcpWorkspaceService.java:1035/:1122-1126/:1214-1218` + `NodePathResolver.java:104-107`）。128B 实测 `query_nodes` 返回 `Equip`/`Equip/BtPetEquipShow`/`Equip/BtPetEquipHide`/`ToolTip/Equip`，`isError:false`（`tools/_mcp_retest_128.py`）。
  - **`.nx` / nlnx / `tools/_wz.py` 回退路径**：才用 **65536 字节完整 key 表**（`tools/vendor/NoLifeWzToNx-master/Keys.cpp` 的 `key_gms`，base64；`tools/_gms_key.b64`，sha256_16 `268a8407a47ee76c`）。这是全量 XOR 表，与 orange-wz 的 AES 派生规格互不通用。
  - 16B / 32B 零 key 会触发 `Index 16 out of bounds for length 16` / `Index 32 out of bounds for length 32`；`ivBase64` 两条路径都用 GMS 4B `bytes([0x4D,0x23,0xC7,0x2B])` = `TSPHKw==`。
  - 历史结论「零 key 是唯一真因 / 内置密钥无故障、换真 key 即通」已证伪：真 65536B `key_gms` 同样 `IMG 解析失败`，**真因是 key 规格错配，不是零值、不是会话过期、不是重名、不是服务端故障**。
- `load_files` 传 `exclusive:false`；同一会话内按 `load_files → query/list/find → unload_all` 收口，避免根锁残留。
- `search_node` 用 `keyword` 参数（传 `query` 会返回 `缺少参数: keyword`）。
- 服务端错误文本为 GBK，UTF-8 解码会呈乱码，按原始字节留证。
- live 改写安全流：关闭客户端 → 复制 live 到 staging → `load_files` → `mutate_nodes` / `copy_paste_nodes` → `save_as` 到**不同** output 路径 → `unload_all` → 原子替换回 live。**禁止** `save_as` 回写与 loaded 根相同路径；**禁止**用 `Copy-Item` / `robocopy` 裸拷 `.img`。
- 图标默认 ARGB4444；`config.ini` 不带 UTF-8 BOM；只改 `Item` 时须用 `analyze_resource_links` 同步 `String` / `Character`。

## 4. 客户端配置

- `C:\Users\11036\.cursor\mcp.json` 已有 `orange-wz` HTTP 项。
- `C:\Users\11036\.codebuddy\mcp.json` 已补入同名 HTTP 项（BOM / LF / 4 空格缩进保持）。
- DSH 全局连接以 `mcp_connector_status` 的 `custom-orange_wz` 为准；manual 连接无 catalog `connectorId`，用 `mcp_connector_health_check`（全量）与 `mcp_connector_tool_search` 复核，勿用 `mcp_connector_tools_list(connectionKey=...)`。
