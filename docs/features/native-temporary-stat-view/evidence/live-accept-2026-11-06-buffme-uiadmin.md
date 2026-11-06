# TemporaryStatView / UIAdmin `!buffme` — 2026-11-06（续验）

## 代码侧（本轮）

| 项 | 提交 / 状态 |
| --- | --- |
| `formatted=false` + `buffme` 置顶并裁到 7 行命中区 | `6a4830b` |
| 去掉构造期 `UISoftKey`（Login SoftKey 盖住 Block 快捷行） | 本轮续修（随 SoftKey 遮挡修复提交） |
| SoftKey `D` 开/关 TemporaryStatView（ShortCut 关闭时） | 既有 `ec97ee9`；ShortCut 打开时 SoftKey `D` 走 Delivery |

## 真机

| 步骤 | 结果 | 说明 |
| --- | --- | --- |
| 强杀真客户端（幽灵无 Path 忽略） | ✅ | PID 有 `ExecutablePath` 才杀 |
| `login.auto=true` 进图 | ✅ | 测完已改回 `false`（未入库） |
| 界面 → SoftKey `A` 开 Admin | ⚠️ | computer-use / SoftKey 偶发开到 Claim/SoftKey；键注入不稳 |
| 点蓝字 `!buffme` → SEND `31 00`…`buffme` | ❌ | `packet_dump` 无 GENERAL_CHAT；自动化未点中快捷行 |
| SoftKey `D` TemporaryStatView 明细行 | ⏳ | SoftKey `D` 可开半透明板；无 buff 时无明细行；依赖 `!buffme` |

## 手点验收（推荐）

1. 进图 → 点「界面」→ SoftKey `A`（只按一次，勿再 SoftKey `A`/`S` 弹 SoftKey）  
2. 在 Admin Block 内点首行蓝字 `!buffme`  
3. 再点「界面」收起 ShortCut → SoftKey `D` 看「临时属性」明细行  

## 配置

`E:\MXD\BeiDou-Client-xy\xy-config.yaml`：`login.auto` / `debug.packet_dump` 验收后已回 `false`（本地，勿提交）。
