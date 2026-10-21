# char-stats detail UI · v4 manifest

| 项 | 值 |
| --- | --- |
| scale | 1x |
| style | 跟附图彩色双列（非经典灰蓝分区条） |
| canvas | 658×392（Main 300 + gap6 + Detail 352） |
| label_w | 80px，字左对齐（tx+3） |
| value_w | 80px；仅「物理攻击力」240px |
| font | 微软雅黑 11（标题 12），整数像素 |
| height | Detail = Main = 392 |
| generator | `_tools/compose_char_stats_ui.py --v4` |
| preview | `docs/features/char-stats-ui-ps/preview/v4/` |
| layers | `docs/features/char-stats-ui-ps/layers/v4/` |
| psd | `docs/features/char-stats-ui-ps/Character_Stats_Detail_V4.psd` |
| psd_layers | CN（Main_CN + Detail_CN 可见）；EN（隐藏）；Reference/Flat_CN_ref（opacity 0） |
| values | EMPTY |
| buttons | NOT baked |
| 样板 | 附图 mockup + UIStatsInfo.h detailed |
| 入库 | 未入库 WZ（本版仅 UI 图产物） |

## 组件清单（拼装）

- 面板铬：圆角灰底 + 标题栏 + 底 bevel
- 标签色块：80×16，按语义着色
- 数值框：白底描边，普通 80 / 物攻 240
- 通栏行：物理攻击力
- 双列行：左/右各 19 项（见 README 字段）
