# r22 · Wave A #2 · PartySearch UI 壳

> 范围：开窗 / 关窗 / 底板与 Bt* 日志；**不做** PartySearch 收发包。  
> 证据：[`evidence/r22/partysearch_wz_nodes.txt`](evidence/r22/partysearch_wz_nodes.txt)  
> 部署：同 Release → `BeiDou-Client-xy`

## WZ（`UIWindow.img/PartySearch`）

| 节点 | 说明 |
|------|------|
| `backgrnd` | 305×407 |
| `BtStart` / `BtStop` / `BtPause` / `BtCancel` / `BtReg` | 操作钮（壳阶段仅 LOG） |
| `Party` / `PartyMember` / `Tab` / `check*` / `icon*` | 列表与页签素材，本切片未绑数据 |

## 实现

| 项 | 路径 |
|----|------|
| UI | `IO/UITypes/UIPartySearch.h/.cpp` |
| Type | `UIElement::Type::PARTYSEARCH` |
| 窗位 | `PosPARTYSEARCH` → `(260, 60)` |

## 入口

`UIUserList` Party 页：`BT_TAB_PARTY_SEARCH` / `BT_PARTY_SEARCH_LEVEL` / `BT_PARTY_SEARCH_INVITE` → `emplace<UIPartySearch>()`（TOGGLED）

## 预留

- PartySearch / Alliance 包序（s9 + REA `0x49D0B6` 升 C）
- 列表行 `Party`/`PartyMember` 填充
- CreateCtrl 坐标校准底部按钮
