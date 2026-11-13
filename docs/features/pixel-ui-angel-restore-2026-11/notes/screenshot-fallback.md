# 截图兜底启用记录

| 日期 | 窗体 | 截图 | 为何启用 | 解析工具 |
|---|---|---|---|---|
| 2026-11-13 | （台账初始化） | `evidence/screens/*.png` | Wave 0 规范化截图库；主路径优先，OmniParser 待后端就绪后按窗启用 | pending |
| 2026-11-13 | 用户指定权威目录 | `C:\Users\11036\Desktop\正确客户端截图` → `evidence/screens/` | 覆盖同步 + `screenshot-map.md`；量测见 `pixel-measure-2026-11-13.md` | done |

启用规则：仅 REA/IDA/IMG 主路径已尽仍对不上时写本表并产出 `notes/layout-*.json`。
