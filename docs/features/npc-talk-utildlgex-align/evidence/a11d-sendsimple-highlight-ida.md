# A11d SENDSIMPLE 高亮条 / 选中态 — IDA 取证

> 日期：2026-11-05 ｜ IDB：`BeiDou.exe.i64`（`E:\MXD\BeiDou-ClientV17.7`）

## 1. 绘制入口

| 符号（本 IDB 无 demangle） | 地址 | 角色 |
| --- | --- | --- |
| 正文元素绘制 | `0x9A3E9F` | 遍历 `this+1732` 元素表；含 type0 正文 |
| 键盘上下改 cur | `0x9AC0B6` | VK_UP/DOWN（38/40）改 `cur`，再 `sub_9AC3B4` 滚到可见 |
| LIST 点选取 cur | `0x9ABDE5` / `0x9ABEBD` | LBUTTON → 命中 selection → 设 cur 后收口 |

字段（CUtilDlgEx this）：

| 偏移 | 含义 |
| --- | --- |
| `+244` | `m_nSelect`（selection 值） |
| `+248` | `cur`（列表下标） |
| `+252` | list 指针 |

## 2. 选中态几何 / 颜色（定案）

`sub_9A3E9F` 对「带 selection 的正文元素」（`field[10] != -1`）：

1. **若 `field[10] == list[cur].sel`**：在基线 y 画虚线，颜色 **`-7237231` = `0xFF919191`**，段宽 3、步长 6（3 on / 3 off），覆盖约 `(text_w+8)`。
2. **若 `field[10] == m_nSelect`**：再画 **黑 1px 实线**，宽 = `text_w + 8`，高 = 1，颜色 `-16777216`。
3. **不改字体色**：正文仍按原色绘制（本仓继续 A12M + DARKGREY / 思源宋体）。

无单独「高亮条」位图节点；UtilDlgEx 的 `bar` 是名牌底，不是选项高亮。

## 3. 本仓接线

- `simple_selected` = 原生 `cur`（悬停 + ↑↓）。
- `draw`：当前项下画黑底线 + `#919191` 虚线，宽 = `label_w + 8`；随滑条裁剪。
- 回车 = `KeyAction::Id::RETURN`（修正旧 `keycode==13` 误读）。

## 4. 局限

- 原生虚线 y 用元素 `field[11]` 基线偏置；本仓用命中框底边近似，真机可再校 1px。
- Avatar/Pet 皮肤未单独复测高亮。
