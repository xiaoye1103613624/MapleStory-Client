# R23-A 地图驱动位移：履带到底是谁驱动的

用户报的现象：站在「番茄田」的传送带上没有任何反应，问「这是一类反应堆事件还是怎么驱动的？」

## 结论

**不是反应堆。** v83 的履带是**纯客户端**的物理属性，挂在 foothold 叶子的 `force` 键上：

```
Map/MapX/<mapid>.img/foothold/<layer>/<chunk>/<fhid>/{x1,y1,x2,y2,prev,next[,force][,forbidFallDown]}
```

`force` = 每秒像素数，**符号即滚动方向**（负=向左，正=向右）。证据链：

| 证据 | 文件 |
| --- | --- |
| 番茄田两张图的 `reactor` 子树为空，`force` 却成批存在 | `_r23_tomato.txt`、`_r23_force.txt` |
| 全量 5364 张图里只有 **51 张**带 `force` 落脚点，共 **855** 条 | `_r23_force_survey.txt` |
| 同一条 foothold 上 `force` 的取值只有 ±100 一档 | `_r23_force.txt` |

## 单位标定（不是拍脑袋）

`Constants.h:29` `TIMESTEP = 8` ms；`Player::get_walkforce() = 0.05f + 0.11f * SPEED / 100`
⇒ SPEED 100 时终端速度 ≈ 0.96 px/tick ≈ **120 px/s**。
所以把 `force` 按 px/s 折算：`hbelt = force * TIMESTEP / 1000` ⇒ `force=100` 是 0.8 px/tick，
比走路略慢（能靠反向走挣脱），和原生手感自洽。

## 为什么必须单独开一个 `hbelt`

把 `force` 直接加进 `hspeed` 会被 `FRICTION` / `GROUNDSLIP` 那一项当"滑行速度"一路吃掉，
站上去只挪一两帧就停。所以位移分两层：

* `PhysicsObject::hbelt` —— 履带推送量，`move()` 里 `x += hspeed + hbelt`；
* `Physics::move_object` 每 tick 先 `hbelt = 0` 再按 `phobj.fhforce` 重算（离地/换类型不残留）。

## 受益面与同类审计

`_r23_mapfeat.txt` 是全量 5364 张图的键普查（1.1s 跑完，脚本 `scripts/mapfeat.py`）。
拿它对照客户端实际解析的键，"站上去没反应"这一类的完整清单是：

| 地图键 | 出现量 | 客户端 | 状态 |
| --- | --- | --- | --- |
| `foothold/*.force` | 51 图 / 855 条 | **本轮接上** | ✅ 修 |
| `foothold/*.forbidFallDown` | 400 图抽样里 322 条 | 全仓库 0 引用 | ⚠ 预留（见 open-issues） |
| `ladderRope` | 3097 图 | `MapInfo.cpp:57` | ✅ 本来就有 |
| `seat` | 99 图 | `MapInfo.cpp:54` | ✅ 本来就有 |
| `portal` / `info.link` | 3097 / 2266 图 | `MapPortals` / `Npc.cpp:39` | ✅ 本来就有 |
| `vportal`、`machine` | **0 图** | 0 引用 | ✅ 不是缺陷：这批数据里根本没有这两个节点 |
| `reactor` | 3067 图 | 由服务端下发，不读地图节点 | ✅ 与履带无关 |

也就是说：**这一类问题只有 `force`（已修）和 `forbidFallDown`（数据有、客户端没实现）两条**，
不存在"还有别的驱动方式没接"的隐藏面。
