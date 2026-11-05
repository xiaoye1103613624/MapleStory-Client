# IDA：连锁任务页码 Gauge

> 日期：2026-11-05 ｜ IDB：`BeiDou.exe.i64`

## 函数

| 地址 | 角色 |
| --- | --- |
| `sub_83EA05` | Quest Gauge 绘制：仅当 `order∈[0,total)` 且 `total>1` 画 frame/gauge/spot |
| `sub_83FEC6` | 悬停：`"%s ( %d / %d )"` + pool **4497** `Chain Quest` |
| `sub_83F4A5` | 取链长度（页总数） |
| `sub_56C9B7` | DualMobGauge：**否**，PartyRace ctor，硬编码 `Mob/9700036` |

## 数据

- `QuestInfo.img/<id>/parent`：链名（string；int 形态忽略）
- `QuestInfo.img/<id>/order`：1-based 步号
- `QuestInfo.img/<id>/{0,1,2}`：状态摘要页（可接/详细/进行）

## 资源

- `UI/UIWindow.img/Quest/Gauge/{frame,gauge,spot}`
- 非 `QuestIcon/*`（后者是奖励 `#f` 横幅）
