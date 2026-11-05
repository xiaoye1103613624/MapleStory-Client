# C4 BtSort/BtGather 原生点击语义

> 状态：代码已闭环；真机 → pending-live-tests  
> 承接：`ui-v83-window-restore` open-issues §C4  
> 日期：2026-11-06  
> 证据：`docs/features/ui-v83-window-restore/evidence/c4-btsort-gather/`

## 解析摘要

已用 Maplestory95.i64（idalib）+ HeavenClient 对照：`BtGather`/`BtSort` 叠槽表示**下一步动作**——点击只发 `SendGather`/`SendSort`，UI 切钮仅由 `OnGatherItemResult`/`OnSortItemResult` 调 `SetArrangeButton`；状态按页签 `m_anSortBtState` 记忆。

## 实现

| 文件 | 改动 |
| --- | --- |
| `IO/UITypes/UIItemInventory.h/.cpp` | `arrange_bt_state[]` + `set_arrange_button`；点击仍只发包；切 tab 恢复页签态 |
| `Net/Handlers/InventoryHandlers.cpp` | Gather/Sort Result 读 `pad+ti` 后写对应页签 |

## 验证

- 构建：`tools/build_smoke.bat`（占用策略 B）  
- 真机：开背包 → Gather → 钮变 Sort → Sort → 钮变 Gather；切 tab 后各页签状态独立（不真机本轮）

## 预留

- BeiDou.exe.i64 仍无 CUIItem 串锚；本结论锚定 GMS v95 符号库。v83 classic 资源键为 `Item/BtGather|BtSort`（非 UIWindow2），行为契约一致。
