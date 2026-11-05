# A11e GetText / GetNumber 布局 — IDA 取证

> 日期：2026-11-05 ｜ IDB：`BeiDou.exe.i64`（`E:\MXD\BeiDou-ClientV17.7`）

## 1. 函数地图

| 角色 | 地址 | 说明 |
| --- | --- | --- |
| 布局分发 | `0x9A6370` | `dlgType 2/3` → `sub_9A83F6`（非 Avatar）/ `sub_9A8716`（Avatar） |
| 建钮 + 输入槽 | `0x9A83F6` | Edit(1000) + BtOK(1) + BtClose(2) + 黑/白框 |
| 内容 inset | `0x9ACC20` | `this+128` / `this+132` |
| OK 校验收口 | `0x9AB50A` | 读 Edit 文本后校验；**不是**布局函数 |
| 内容宽 | `0x9A3D07` | type2/3 → **341**（Avatar 再减） |

> 用户提示「0x9AB50A 一带」：收口在此，布局在 `0x9A83F6` / `0x9ACC20`。

## 2. 几何（非 Avatar，定案）

`sub_9ACC20`：

| 字段 | 值 |
| --- | --- |
| `this+128` content_left | **157**（Avatar=25；有滑条时 −2） |
| `this+132` content_top | `(content_h - text_h - 6)/2 + 28`（装不下则 22） |

`sub_9A83F6`：

| 元素 | 公式 |
| --- | --- |
| Edit(1000) | `(left+2, top+text_h+5)`，`w=W-(2*left+2)`，`h=14` |
| 黑框 | `(left-1, top+text_h+2)`，`w=W-2*left+3`，`h=18`，色 `0xFF000000` |
| 白底 | `(left, top+text_h+3)`，`w=W-2*left+1`，`h=16`，色 `0xFFFFFFFF` |
| BtOK | `(W-54, H-26)` |
| BtClose | `(9, H-26)` |

最小 content 高（type≤4）：`sub_9A3DC1` → **120**。

## 3. 本仓接线

- `INPUT_LEFT=157`；黑/白框 + 内嵌 `Textfield`（A12M 黑字，Unicode 仅 GetText）。
- GetText/GetNumber 均激活 **BtOK @ W-54**（此前 GetText 无 OK）。
- 正文区为输入栈预留 `INPUT_STACK`，再在剩余高度内垂直居中正文。
- OK / Enter → 发包；空文本 / 非整数拒绝（对齐收口精神，min/max 线参未进包则本版不验）。
- SoftKey 旁路已撤（布局以原生 Edit 为准）。

## 4. 局限 / pending

- ~~Avatar 变体 `0x9A8716`~~ → **A11f 已接**（见 a11f）。
- ~~线参 min/max~~ → **A11f 已接**（GetNumber）；GetText 包尾 int>0 才作 min_len。
- 真机：输入槽相对正文间距、光标基线 ±1px。
- 原生 tip 文案未接。
