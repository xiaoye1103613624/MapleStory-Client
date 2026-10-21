# 性能基线记录

测量环境填写：

| 项 | 值 |
|---|---|
| 机器 | （CPU / RAM / GPU） |
| 存储 | SSD / HDD |
| 数据格式 | img |
| 客户端版本 | 228.3 |
| 日期 | 2026-09-28 |

| 场景 | 目标 | 实测 | 备注 |
|---|---|---|---|
| 补丁无更新 → 登录 UI | ≤ 8000 ms | （跑客户端看 `[BOOT] init_total`） | 代码已打分段 |
| resource 分段 | — | `[BOOT] resource` | |
| patch 分段 | — | `[BOOT] patch` | url 空则近 0 |
| 首次进村 | ≤ 3000 ms | 手工秒表 | |
| 战斗 FPS | ≥ 60 | `SHOW_FPS` | |
| 换图 atlas | — | 日志 `[PERF] unload_atlas` | 每图一次 |

自动化：启动日志已打 `[BOOT]` 分段；将数字抄入本表即可。
