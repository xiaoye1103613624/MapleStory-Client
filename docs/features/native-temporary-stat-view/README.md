# W5 — TemporaryStatView（原生明细窗）

## 资源
`UIWindow.img/TemporaryStatView/1`（32×32 图标；无完整 backgrnd，明细列表本地绘制）

## 数据
本地 `Player::buffs`（GIVE_BUFF 链，无新协议）：按 skillid 聚合显示名称 / 数值 / 剩余秒

## 入口
- 游戏进图创建窗（默认 deactivate）
- `UIBuffList` SoftKey/`D` 切换开关

## 与 BuffList 分工
| 窗 | 职责 |
| --- | --- |
| UIBuffList | 图标条 + 右键取消 |
| UITemporaryStatView | 按技能分组的来源/剩余明细 |
