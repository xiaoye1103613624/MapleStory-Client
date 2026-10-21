# HTTP 差异补丁更新器（按最终文件）

> 状态：设计已落地；模块 `Util/Patch/*` + `tools/patch/gen_manifest.py`  
> 产物：`docs/features/patch-updater-http/`

## 1. 目标

启动时与独立补丁站比对，只下载变更的最终 `.img` / `.wz` / `.nx` 文件；**不改 BeiDou 游戏封包**。

非目标：块级差分；把整包打进 APK。

## 2. Manifest 契约

```json
{
  "clientMinVersion": "228.3",
  "channel": "stable",
  "baseUrl": "http://127.0.0.1:8080/v83/",
  "files": [
    {
      "path": "Data/Map/Map/Map0/000010000.img",
      "size": 12345,
      "sha256": "abcdef...",
      "format": "img"
    }
  ]
}
```

- `path`：相对客户端工作目录（通常含 `Data/` 前缀）。
- `sha256`：小写十六进制。
- `baseUrl`：文件 URL = `baseUrl` + `path`（若 path 已是绝对 URL 则直接用）。

## 3. 客户端流程

1. 读 `patch.enabled`；false → 跳过。
2. `patch.url` 空 → 跳过并 WARN。
3. GET manifest；失败且 `patch.allow_skip` → WARN 继续，否则失败可重试。
4. 过滤 `channel`；比对本地 size+sha256。
5. 下载到 `path + ".part"` → 校验 → 原子 rename。
6. 写 `patch-state.json`（最近成功的 manifest 摘要）。
7. 再进入 `ResourceService::init()`。

## 4. 配置键

| 键 | 默认 | 含义 |
|---|---|---|
| `patch.enabled` | `true` | 总开关 |
| `patch.url` | `""` | manifest 完整 URL |
| `patch.channel` | `stable` | 频道过滤 |
| `patch.timeout_ms` | `15000` | 超时 |
| `patch.retries` | `2` | 重试次数 |
| `patch.allow_skip` | `true` | 失败是否允许进游戏 |

## 5. HTTP 实现

| 平台 | 后端 |
|---|---|
| Windows | WinHTTP（HTTP/HTTPS） |
| Linux / Android / 其它 | POSIX socket 明文 HTTP；HTTPS 需后续 OpenSSL/curl |

## 6. 发布工具

```bash
python tools/patch/gen_manifest.py --root publish/ --base-url http://host/v83/ --out manifest.json --channel stable
```

扫描根下 `.img`/`.wz`/`.nx`，写 sha256/size/path。

## 7. BeiDou 侧

静态 Nginx/对象存储，或独立 Servlet：`GET /client/patch/manifest` 返回同上 JSON。见 [beidou-integration-minimal](../beidou-integration-minimal/README.md)。

## 8. 验收

- [ ] 改 1 个 `.img` 重生 manifest → 客户端只拉该文件
- [ ] sha256 失败不覆盖原文件
- [ ] 断网 + `allow_skip=true` 可进登录并打 WARN
- [ ] `patch.enabled=false` 完全跳过
