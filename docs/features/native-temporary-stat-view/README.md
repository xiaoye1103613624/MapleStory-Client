# W5 — TemporaryStatView（原生明细窗）

## 资源
`UIWindow.img/TemporaryStatView/1`（32×32 图标；无完整 backgrnd，明细列表本地绘制）

## 数据
本地 `Player::buffs`（GIVE_BUFF 链，无新协议）：按 skillid 聚合显示名称 / 数值 / 剩余秒

## 入口
- 游戏进图建窗（默认 deactivate）
- `UIBuffList` SoftKey/`D` 切换开关

## 与 BuffList 分工
| 窗 | 职责 |
| --- | --- |
| UIBuffList | 图标条 + 右键取消 |
| UITemporaryStatView | 按技能分组的来源/剩余明细 |

## 2026-11-06 核对补缺
- 标题改为中文「临时属性」（宋体 A12B / WHITE）
- 明细行 A11M + WHITE；半透明黑底 α=0.55 托字（原生无整窗底板）
- 无独立 TEMPORARY_STAT 协议（s9 零命中）——继续本地驱动
- 真机：有 buff 时按 `D` 开窗，见名称/层数/倒计时（pending 造 buff）
