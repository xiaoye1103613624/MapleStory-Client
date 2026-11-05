# C4 — BtSort / BtGather 原生点击语义

> 日期：2026-11-06  
> IDB：`E:\MXD\Maplestory95.i64`（本轮 idalib 只读；当前 Cursor MCP 挂的 BeiDou.exe.i64 **无** `BtGather`/`CUIItem` 串锚）  
> 对照客户端：HeavenClient（同 Journey 系，点击/回包假说一致）  
> s9：`PacketCreator.finishedSort` / `finishedSort2`

## 已证实

| 函数 | 地址 | 行为 |
| --- | --- | --- |
| `CUIItem::OnButtonClicked` | `0x7CC4F0` | `0x7D3`→`OnGather`；`0x7D4`→`OnSort`；`0x7D5/0x7D6`→`Toggle` 展开 |
| `CUIItem::OnGather` | `0x7CBB40` | **仅** `CWvsContext::SendGatherItemRequest(m_nItemTI)`，不切钮 |
| `CUIItem::OnSort` | `0x7CBB70` | **仅** `SendSortItemRequest`，不切钮 |
| `CUIItem::SetArrangeButton(nTI,nState)` | `0x7CCC90` | `m_anSortBtState[nTI]=nState`；当前页签时：`0`→`UIWindow2…/BtGather`(id `0x7D3`)，`1`→`…/BtSort`(id `0x7D4`) |
| `CWvsContext::OnGatherItemResult` | `0x9F1280` | `Decode1`+`ti=Decode1` → `SetArrangeButton(ui, ti, **1**)` |
| `CWvsContext::OnSortItemResult` | `0x9F12B0` | 同上 → `SetArrangeButton(ui, ti, **0**)` |
| `CUIItem::OnTabChanged` | `0x7CF040` | `SetArrangeButton(m_nItemTI, m_anSortBtState[m_nItemTI])` 恢复页签状态 |
| `CUIItem::OnCreate` | `0x7CE520` | `m_anSortBtState[0..5]=0` 后 `SetArrangeButton(…,0)` → 默认 Gather |

结论：**按钮表示「下一步动作」**；切换**只靠** Gather/Sort Result，点击本身不改态；状态按页签记忆。

## 与本仓库差异（修前）

- 点击发包 + Result 切钮极性已对（GatherResult→Sort / SortResult→Gather）。
- 缺口：① Result **未读** `ti`；② 切 tab **强制** `set_sort(false)`，丢掉页签记忆。

## 本轮实现

- `set_arrange_button(type, show_sort)` + `arrange_bt_state[]`
- Result handler 读 `byte + ti` 后写对应页签
- 切 tab 恢复该页签状态

## 产物

反编译：`OnGather.c` / `OnSort.c` / `SetArrangeButton.c` / `OnGatherItemResult.c` / `OnSortItemResult.c` / `CUIItem_OnButtonClicked.c` / `caller_7cf040_*.c`  
脚本：`_idalib_c4*.py`
