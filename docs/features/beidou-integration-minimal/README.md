# BeiDou 集成（最小改动）

> 状态：设计已落地  
> 协议基线：[opcode-align-beidou-gms083](../opcode-align-beidou-gms083/README.md)  
> 产物：`docs/features/beidou-integration-minimal/`

## 1. 目标

客户端对接 BeiDou gms083：**游戏进程零改或仅加只读补丁 API**；登录/频道协议沿用已对齐 opcode。

## 2. 边界

| 允许 | 禁止 |
|---|---|
| 静态站托管 manifest + 文件 | 改 Login/Channel 封包结构 |
| 独立 Servlet / 管理端只读 GET | 改 opcode 表、存档、倍率 |
| 配置里写 `patch.url` / `server.ip` | 用游戏会话传资源字节 |

## 3. 推荐部署

```text
[玩家]
  ├─ TCP 8484 → BeiDou Login → Channel
  └─ HTTP    → patch.example.com/manifest.json + files
```

可选：同域名反代

```text
https://game.example.com/          → 管理页
https://game.example.com/client/patch/manifest.json → 静态或 Servlet
https://game.example.com/v83/...   → 资源文件
```

### Servlet 草案（Java，示意）

```java
// GET /client/patch/manifest → application/json
// 从配置目录读 manifest.json 原样写出；无鉴权或仅 CDN Token
```

不进入游戏 Handler 树。

## 4. 客户端配置映射

| 客户端 | BeiDou / 运维 |
|---|---|
| `server.ip` / `server.port` | `gms.service.login-port`（默认 8484） |
| `patch.url` | 补丁站 manifest URL |
| `MAPLEVERSION=83` | gms083 |

## 5. 验收

- [ ] 不启补丁站时，`allow_skip` 仍可登录既有服
- [ ] 启补丁站后差异文件更新不影响进服
- [ ] 抓包确认资源流量不在 8484 游戏口

## 6. 预留

- CDN 鉴权头、灰度 channel
- 管理端一键发布（调用 `gen_manifest.py`）
