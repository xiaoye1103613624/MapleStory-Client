# UI manifest — feedback / v1

- 样式来源：原生
- scale：1x
- 目标尺寸：见下表
- 状态列表：
  - BadgeAuction / BadgeFixed：静态模式角标（拼装时叠字）
  - Countdown：字色参考 `normal` / `warn(<5min)` / `critical(<1min)`
  - EmptyState：静态空列表底板
  - BadgeStashOld：已在 `grid/v1/`（R1），本批不重复
- 样板：无（v083 经验基线）
- 采样路径 / 客户端：未实测（见 `notes/client-path.md`）
- 目标节点路径：待定
- psd：./feedback.psd
- 组件列表：
  - BadgeAuction（36×12 橙褐）
  - BadgeFixed（36×12 青蓝）
  - Countdown_normal|warn|critical（48×12 色条）
  - EmptyState（120×48）
- 生成说明：角标硬边色块供叠字；倒计时仅色参考，非字体位图
- 验收：§13 本批已过；待路径后 v2

## 文件表

| 文件 | 尺寸 | 说明 |
| --- | --- | --- |
| BadgeAuction.png | 36×12 | 拍卖角标 |
| BadgeFixed.png | 36×12 | 直售角标 |
| Countdown_normal.png | 48×12 | 常态字色 |
| Countdown_warn.png | 48×12 | &lt;5min 橙 |
| Countdown_critical.png | 48×12 | &lt;1min 红 |
| EmptyState.png | 120×48 | 空态底板 |
| feedback.psd | 280×100 | 源 |
