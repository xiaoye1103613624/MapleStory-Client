# IDA：`#i` / `#v` 行内道具图标

> 日期：2026-11-05 ｜ IDB：`BeiDou.exe.i64`（`E:\MXD\BeiDou-ClientV17.7`）

## 令牌 → 元素类型

| 证据 | 结论 |
| --- | --- |
| `sub_9A2DE0`（`0x9A2DE0`） | `'i'` → enum **7**；`'v'` → enum **8** |
| `sub_99DD54` case 7/8 | 共用路径：`*elem = 1`（与绘制侧 type1 一致） |
| `sub_9A262F` | `atoi(token + 2)` 解析道具 id |
| `sub_5D3BD8(..., itemid, 1, 0)` | 取 **info/icon**（非 iconRaw；string id 2356 分支） |
| `elem[8]/[9]` | `GetWidth` / `GetHeight`（canvas 真值，非写死 32） |

`#i` 与 `#v` 在本 IDB **无分流**（同 type1、同 icon 节点）。

## 行内排版

| 证据 | 结论 |
| --- | --- |
| 水平（`0x9A21CF`） | type∈{1,2,4}：`x = cursor`；`cursor += width`；超 `maxwidth` 换行 |
| `sub_9A2D01` | 同行 `max(height)`；**下限 16** |
| `sub_9A2D95` | `y_top = line_y + max_h - elem_h`（行盒底对齐） |
| 行距 | 下一行 `line_y += max_h + 2` |

## 绘制

`sub_9A3E9F` case 1/2：`IWzCanvas` blit 于 `(content_left + x, content_top + y_top)`，α=255。

## 本仓映射

- `LayoutBuilder` 解析 `#i/#v`，宽高取 `ItemData::get_icon(false)`。
- 图标底对齐当前行基线（≈ 行盒底）；含图标行换行走 `max(h,16)+2`。
- 纯文本行仍用既有 `linespace` / `line_adj`（宋体 NPC 正文已对齐，避免全局改行距）。

## 局限 / 预留

- 首行若以大图标开头，基线初值仍是 `font.linespace()`，图标顶可能略伸到文本原点上方（内容区通常有 padding；真机可再校）。
- `#f` 路径图、`#s` 技能图未接。
- 未在本轮用 orange-wz 抽具体道具 canvas 尺寸样例（逻辑跟 GetWidth/Height，不烤死 32）。
