# 隐藏门 portalStart / Continue / Exit 三态

> 日期：2026-11-05 ｜ 证据：`map-object-parity/evidence/wave3-research-ledger.md` §6
> 状态：代码已闭环；真机 → pending-live-tests

## 资源

`MapHelper.img/portal/game`：

| 族 | 路径 | 用途 |
| --- | --- | --- |
| ph | `default/{portalStart,portalContinue,portalExit}` | `HIDDEN` |
| psh | `default`（缺则 `1`）同形三态 | `SCRIPTED_HIDDEN`；全空回退 ph |
| pv | `default` 或本体 0..7 | 显式门共享动画（既有） |

## 状态机（每门独立 Animation 副本）

```
NONE ──进入──▶ START ──播完──▶ CONTINUE ──离开──▶ EXIT ──播完──▶ NONE
```

显式门仍共享 `regular_anim`（`MapPortals::update` 统一推进），避免 N 份帧不同步。

## 触碰

`Portal.{h,cpp}`、`MapPortals.{h,cpp}`；销案 map-object-parity §5「隐藏门三态」预留。
