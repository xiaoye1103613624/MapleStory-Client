# 萧曳 083 客户端平台化总纲

> 状态：M0–M5 客户端侧已闭环（Android arm64 APK 可构建；端到端点测依赖真机/BeiDou）  

> 适用：MapleStory-Client × BeiDou gms083 / Maple v83  
> 产物目录：`docs/features/client-platform-masterplan/`  
> 相关分册：  
> - [resource-runtime-img-first](../resource-runtime-img-first/README.md)  
> - [patch-updater-http](../patch-updater-http/README.md)  
> - [beidou-integration-minimal](../beidou-integration-minimal/README.md)  
> - [perf-budget](../perf-budget/README.md)  
> - [playability-core-loop](../playability-core-loop/README.md)

## 1. 背景与目标

把现有 HeavenClient 系客户端打造成：**IMG 优先、仍兼容 WZ/NX**、可多端部署、与 BeiDou 低耦合集成、启动差异资源同步、核心循环可玩且性能可控的生产客户端。

### 已确认决策

| 项 | 选择 |
|---|---|
| 生产主格式 | IMG（`data.format=img`；保留 WZ/NX） |
| 差异同步 | 独立 HTTP 补丁站；粒度=最终 `.img`/`.wz`/`.nx` 文件 |
| 多端顺序 | Windows → Android arm64 → Linux → iOS |
| v1 可玩性 | 登录→创角→进图→移动战斗→任务/NPC→换图 |
| 文档形态 | 本总纲 + 分 feature 子文档 |
| BeiDou 改动 | 游戏核心不动；独立 Servlet/静态站提供 manifest |

### 非目标（本版）

- 大改 BeiDou 玩法/脚本/存档/opcode
- 运行时热切换数据格式
- XML 资源后端
- 块级二进制差分（整文件替换即可）
- v1 内做组队/好友/商城/公会

## 2. 架构

```text
Boot → PatchUpdater (HTTP manifest) → ResourceService (img/wz/nx)
     → Window/Audio/UI → Session (v83 ↔ BeiDou Login/Channel)
```

原则：游戏封包只打 BeiDou；资源与补丁完全走 HTTP。

## 3. 里程碑

| ID | 内容 | 依赖 | 验收 |
|---|---|---|---|
| M0 | 设计文档闭环 | — | 六份 README + 过时指针修正 |
| M1 | IMG 优先运行时加固 | M0 | 纯 IMG 冷启动进登录 UI；缺非关键根不崩；WZ/NX 可显式启动 |
| M2 | HTTP 按文件同步 | M0 | 改 1 文件只拉该文件；断网可跳过 |
| M3 | 核心循环可玩 | M1/M2、既有登录/任务 | 见 playability 剧本 |
| M4 | 性能预算与观测 | M1 | 分段耗时日志；换图清 atlas |
| M5 | 多端 | M2 | Win 主验；Android arm64 + 可写补丁目录；Linux/iOS 随后 |

建议实现序：M0 → M2 → M1 → M3 → M4 → M5。

## 4. 配置键（部署 `xy-config.yaml`）

```yaml
server:
  ip: 127.0.0.1
  port: 8484
data:
  format: img
  dir: Data
  keymode: auto
  keyfile: ""
patch:
  enabled: true
  url: "http://127.0.0.1:8080/client/patch/manifest.json"
  channel: stable
  timeout_ms: 15000
  retries: 2
  allow_skip: true
```

优先级：`xy-config.yaml` > `Settings` > 代码默认。

## 5. 验收总表

| 场景 | 通过标准 |
|---|---|
| IMG 启动 | 登录 UI 可见，诊断日志含 `format=img` |
| 补丁增量 | manifest 变更后仅下载差异文件且 sha256 通过 |
| BeiDou 进服 | opcode 对齐文档路径可登录进图 |
| 核心循环 | playability 五步剧本全绿 |
| 性能 | 见 perf-budget 目标；无更新冷启动登录 UI ≤ 8s（SSD 参考） |
| Android | arm64 APK 能装；补丁写入外部存储可写目录 |

## 6. 风险与回滚

| 风险 | 对策 |
|---|---|
| IMG 文件过多同步慢 | manifest 只列变更；首装用整包镜像另通道 |
| 大文件整文件替换 | v1 接受；块差分预留 |
| 更新中断 | `.part` + sha256 失败不覆盖 |
| 移动端权限 | Android 外部存储路径写清 |

回滚：`patch.enabled=false`；Data 目录回退备份。

## 7. 附录：后续优化（预留）

- `patch/` 覆盖层与灰度 `channel`
- HTTPS + 签名 manifest
- 组队/好友/交易 → 商城/公会
- 纹理 LRU 精细配额、Map canvas 降采样
- WZ/IMG 一律经 MCP/工具改，禁止手改二进制猜字段

## 8. 过时文档指针

以下文档若与本总纲冲突，**以本总纲及分册为准**：

- `docs/features/resource-backend-abstraction/README.md`（实现已落地，见 resource-runtime-img-first）
- `docs/data-source-loading-matrix.md`
- `Platform/NOTES.md`（Android 进度以 `android/README.md` 为准）
