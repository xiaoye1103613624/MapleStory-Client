# 性能预算与观测

> 状态：观测与换图卸载已落地；基线数字待参考机填写  
> 产物：`docs/features/perf-budget/`

## 1. 目标（Windows 参考机，SSD，IMG）

| 场景 | 目标 | 基线填写 |
|---|---|---|
| 补丁无更新 → 登录 UI | ≤ 8s | 见 `notes/baseline.md` |
| 首次进常见村 | ≤ 3s 可操作 | 同上 |
| 战斗稳态 1080p | ≥ 60 FPS | `SHOW_FPS` |
| 进图后工作集 | 记录峰值后设上限 | 同上 |

## 2. 观测点（上线级）

启动日志前缀 `[BOOT]`：

| 阶段 | 含义 |
|---|---|
| `patch` | 补丁同步耗时 |
| `resource` | ResourceService::init |
| `window` | Window/GL |
| `ui` | UI + LoginBackground |
| `init_total` | init 总计 |

换图：`SetFieldHandlers` 调用 `GraphicsGL::unload_atlas()`（强制清空 GPU atlas）。  
压力：`clear()` 在占用 >60% 时回收。  
纹理：`Offset.last_use` 记录绘制代数；打包 atlas 满时 `clearinternal`（整表 LRU）。

## 3. 手段

- IMG per-root 懒加载（已有）
- 换图强制 `unload_atlas`；按需重传
- 删除 `Texture` 硬编码调试落盘
- 热点禁止 INFO 逐 tick；保留 FPS 可选开关
- 预留：精细 LRU 配额、超大 minimap 降采样

## 4. 验收

- [x] 冷启动日志含各 `[BOOT]` 分段（`BootTimer`）
- [x] 换图路径调用 `unload_atlas`
- [ ] 参考机填入 `notes/baseline.md` 实测数字
- [ ] 连续换图 10 次内存无明显只增不减（手工）
- [ ] `SHOW_FPS` 战斗场景可达目标或记录缺口

## 5. 测量步骤

见 [`notes/baseline.md`](notes/baseline.md)。
