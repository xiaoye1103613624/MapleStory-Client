# GuildBBS 布局常量

IMG 无按钮绝对坐标节点（仅 bitmap + origin）。落位按底板量测：

| 控件 | 相对窗左上 | 依据 |
| --- | --- | --- |
| BtQuit | `(W-48, 6)` | 标题栏右上，贴图 43×18 |
| BtWrite / Retouch / Delete | `(24 + i*65, 488)` | 底栏，贴图 57×26 |
| BtReply / ReplyDelete | 底栏续排 | 贴图 20×20 |
| MoveEmoticon L/R | `(W-70 / W-48, 488)` | 翻页 |
| backgrnd2/3 浮层 | 居中叠主窗 | 327×288 |
| Register/Cancel/Notice | 浮层底 `(ox+40/120/200, oy+248)` | 发帖态 |

真机若与烘焙槽位偏差，优先改本表常量，勿猜改贴图。
