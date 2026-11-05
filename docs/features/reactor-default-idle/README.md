# 反应堆 idle / default 悬浮动画（2026-11-05）

> 状态：代码已闭环；真机 → pending-live-tests  
> 承接：`map-object-parity` 预留「反应堆 default 悬浮动画」；`Reactor.cpp` TODO

## 1. 选型

闭合 `Reactor.cpp` 空闲分支「default / 悬浮」TODO：暗黑龙王裂隙等反应堆应循环播放多帧 idle，而非冻在第 0 帧。

## 2. 证据

| 源 | 结论 |
| --- | --- |
| IDA `sub_71165A` @`0x712213` `push 3C0h`（string id **960**） | 原生可读属性名 `default`（见 `native-map-object-re` §3.2） |
| IDA `sub_739A92` | `Reactor/%s.img` 按状态节点读 `event` / `hit` / 帧 |
| 079/s9 `Reactor.wz` 全量 XML | **零** `name="default"` 子键 |
| `2201004.img.xml`（HT 裂隙） | `0/` 下 canvas `0..6`，`delay=120` —— 悬浮即 **state 多帧** |
| 统计 | state0 含 `canvas/1` 的反应堆 **523+** |

**【已证实】** 本包「悬浮」= 当前 state 下数字 canvas 循环；`default` 子键为兼容预留。  
**【差异】** 修前 `normal=src[0]` 且 idle 分支从不 `update()` ⇒ 全图多帧 idle 冻结。  
**【推荐】** `reload_idle()` + idle 每拍 `update()`；hit 结束后按新 state 重载。无待决项。

## 3. 实现

- `Reactor::reload_idle()`：`state/default` → `state` → 根 `default`
- `update()`：idle 循环 `normal.update()`；hit 结束调用 `reload_idle()`
- `set_state` / `destroy`：无 hit 转场时立刻 `reload_idle()`
- 初始 state 驱动 idle / hittable 探测（不再写死 `src[0]`）

## 4. 验证

- 构建：见 `builds/`
- 真机（本轮不跑）：进暗黑龙王前置 / 任意多帧反应堆图，确认 idle 帧在动；打击后 hit 播完切新态 idle

## 5. 触碰

仅 `Gameplay/MapleMap/Reactor.{h,cpp}` + 本文档 / parity 台账；未动服务端。
