# P6 · 天气资源入库 Runbook（orange-wz MCP）

> 用途：任何**带 `mcp__orange_wz__*` 工具**的 DSH 会话，照本文件即可执行天气资源入库，无需重新分析。
> 背景：`custom-orange_wz` 已全局注册（`C:\Users\11036\.dsh\storages\mcp_connector.json:32-47`，`kind:manual` / `injectionMode:always` / `serverName:orange_wz` / `url: http://127.0.0.1:10012/mcp`）。工具是否出现取决于**会话启动时是否注入**，与注册无关。若某会话没有该前缀，用新会话即可，不要重新注册（会重复）。

## 0. 前置
- 服务可用性：`curl http://127.0.0.1:10012/mcp` 返回 HTTP 400（SSE 端点正常）；不可用时跑 `E:\project\orange-wz\start-mcp.bat`（runtime memory 2026-11-17 m01002）。
- 源资源根：`E:\MXD\扩展改动\天气系统\weather-time-systems\wz\client\`（11 个 img + MapBack/MapObj/Sound/Effect 子树）。
- **关客户端再动 live Data**；所有落盘走 staging 异路径 → 原子替换，禁止 `save_as` 回写 loaded 根，禁止 `Copy-Item`/robocopy 裸拷 `.img`（AGENTS.md §2/§3.1）。

## 1. 核心闭环必须入库的 img（D1，7 个）
| # | 路径 | 用途 | 来源/风险 |
| --- | --- | --- | --- |
| 1 | `Effect/WeatherParticles.img` | sandstorm/blizzard/leaves/blossom 四天空的粒子集 | 从 `Map/MapHelper.img/weather` 复制重编组（IMPLEMENTATION.md:884-900） |
| 2 | `Map/Back/MapleMoonSky.img` | 夜空渐变（本端代码按字符串路径打开，**文件名不可改**） | drawn |
| 3 | `Map/Back/WeatherFog.img` | 4 条雾带；blizzard 靠它 | drawn |
| 4 | `Map/Back/WeatherSnow.img` | 两层降雪平面 | **IMPORTED 第三方 bank，zlib blob 原样拷贝（:893-894）→ 必须转 key** |
| 5 | `Map/Back/WeatherRainbow.img` | 雨后彩虹 | drawn |
| 6 | `Map/Obj/MapleMoonLamp.img` | 可注入灯柱 + 3 光晕 | drawn；代码按字符串打开，名不可改 |
| 7 | `Map/Obj/MapleMoonLamp2.img` | 42 导入灯 sprite（1.5MB） | **IMPORTED 另一服务端 bank（:902-904）→ 必须转 key**；`lamps.cpp` 无条件 include 其表，缺它不编译 |

本版**不入库**（D1 预留）：`Effect/WeatherSplash.img`、`WeatherPuddle.img`、`WeatherAccum.img`、`Sound/Weather.img`。

## 2. orange-wz MCP 流水线（每个 IMPORTED 包必走）
```
load_files(源路径, key=源规格, exclusive:false)
  → query/list/find 校验节点（key.userKeyBase64 用 128B DEFAULT_KEY 的 base64，见 AGENTS.md §3；
     orange-wz MCP 路径专用 128B，不是 65536B NoLife 全量表）
  → save_as 到 staging 异路径
  → batch_convert_key(目标端key, verify:true)  ← 两个 IMPORTED 包强制，change_key 单独不算过门证据
  → unload_all
  → 短启验收：--resource-probe 目标 img 出现在 UI 根，进程无 0xc0000374 堆损坏
  → 原子替换进 live Data（客户端未占用）
```
- `load_files` 传 `exclusive:false`；同一会话 `load_files → query → unload_all` 收口，避免根锁残留。
- `search_node` 用 `keyword` 参数（传 `query` 报"缺少参数: keyword"）。
- 服务端错误文本是 GBK，UTF-8 解码会乱码，按原始字节留证。
- 图标默认 ARGB4444；`config.ini` 不带 UTF-8 BOM。

## 3. stock 文件节点合并（不是整包覆盖）
- `Item/Cash/0512.img`：追加 5 个 art pointer 节点 `05120995`–`05120999`（纯 art pointer，无 icon/iconRaw，4/9 天空依赖）；若取 WeatherDebuff 再加 `05120994`（本版 debuff 预留，可不加）。
  - 工具备选：`E:\MXD\自定义功能实现\天气系统\resources\tools\AddWeatherCashItems.java` / `VerifyWeatherCashItems.java`（MapleLib 路线，**仅当 orange-wz 不可用时经用户例外批准**；本 runbook 主路径用 orange-wz 节点合并）。
  - 缺失 inline item icon 是少数能带崩客户端的情况，追加后必须抽样校验 path/type。
- `String/Cash.img`：仅 debuff 文案 `5120994`，本版可不加。
- 3 个非 stock lamp sprite（`ShamanBT.img/koCave/object/28`、`ShaolinCN.img/house15/base/11`、`arcana.img/lightForest/acc/0`）缺失不崩，但 Ellinia 12 处手动灯静默失效；本版核心闭环可不补，记预留。
- 两个文件名 `MapleMoonSky.img` / `MapleMoonLamp.img` 是 DLL/本端代码打开的字符串路径，禁止改名。

## 4. 验收
- orange-wz `query_nodes` 各 img 根节点可见、`isError:false`。
- 短启 `--resource-probe`：目标 img 出现在 UI 根，无 `0xc0000374`。
- 记录所用 key 名称（不写明文 key/IV 进库）、每个 img 的 SHA256、probe 日志，落 `evidence/`。

## 5. 为什么本会话做不了（证据，非借口）
- `custom-orange_wz` 已全局注册（见上），但本会话 projcache（`session-344a1494-...json`，16:22）无 `mcp__orange_wz__*` / `mcp_connector_*` 工具快照；今早会话（`session-4a3959e4-...json`，09:49）有全套 8 个 `mcp__orange_wz__*` 工具 + `mcp_connector_status/health_check/reconnect`。
- `ops_discover_tools` 无法激活该能力（实测返回 no capability matched）。
- → 结论：新开会话即可自动带上全局 orange_wz 工具，照本 runbook 执行 P6；不要为"每次指定"重复注册（配置本就全局）。
