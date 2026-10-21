# 资源运行时：IMG 优先（兼容 WZ / NX）

> 状态：已闭环（M1）  
> 协议/版本：BeiDou gms083 客户端资源  
> 产物：`docs/features/resource-runtime-img-first/`

## 1. 背景与目标

- **生产默认 IMG**：代码默认与 `xy-config.example.yaml` 均为 `data.format: img`。
- **零转换**：`ImgSource` 内存解析，不落 `.nxcache`。
- **兼容**：显式 `wz` / `nx` 与 `auto` 探测仍可用。
- **最小侵入**：业务层继续用 `nl::nx::*`。

非目标：运行时热切换格式；XML 后端。

## 2. 影响范围

| 层级 | 路径 | 变更 |
|---|---|---|
| 客户端 | `Util/Resource/*` | 门面/后端/定位/密钥 |
| 配置 | `Configuration` `data.*` | 默认 `img` |
| 文档 | 本文件 | |

## 3. 最终实现逻辑

### 3.1 启动

`MapleStory::init` →（补丁同步后）→ `ResourceService::init()`：

1. 读 `data.format`：默认 `img`；亦可 `auto`/`wz`/`nx`；未知 → `DATA_FORMAT`。
2. `ResourceLocator::locate` 候选链：`XY_DATA_DIR` → `data.dir` → exe/Data/…。
3. 打开对应 Backend，绑定 `RootTable` → `nl::nx::*`。
4. 上线日志：`[RES] …` + `ResourceService::diagnostics(true)`。

### 3.2 能力

| 后端 | 惰性 | 缺根 |
|---|---|---|
| Img | per-root 懒加载 | 仅要求 `UI/`；其余 WARN 容忍 |
| Wz | 元数据一次解析，像素按需 | 容忍 BeiDou 缺根 |
| Nx | mmap + 按需 | 缺失根空节点 |

### 3.3 密钥

`data.keymode` / `data.keyfile` → `Keyring`；明文不进仓库。

## 4. 配置

```yaml
data:
  format: img    # 生产默认；开发可用 auto / wz / nx
  dir: Data
  keymode: auto
  keyfile: ""
```

## 5. 验收

- [x] 代码默认 `data.format=img`（`Configuration.h`）
- [x] 缺非关键根（如 Sound）仅 WARN，不致命（`ImgSource.cpp`）
- [x] 启动日志含 format/backend 诊断（`ResourceService` / `[BOOT] resource`）
- [ ] Windows 纯 IMG 目录冷启动进登录 UI（手工）
- [ ] `data.format=wz` / `nx` 样例目录可启动（手工回归）

## 6. 与旧文档关系

`resource-backend-abstraction` 为历史设计稿；**实现状态以本文件 + 源码为准**。
`docs/data-source-loading-matrix.md` 已加指针，勿再按 `DataFiles`/`nxcache` 操作。
