# C6 · NPC 对话格式令牌（`#p/#h/#t/#z` + `#i/#v/#s/#f`）

> 日期：2026-11-05 ｜ 纯客户端 ｜ 承接 open-issues §C6

## 问题

`UINpcTalk::format_text` 对 `#p` / `#h` / `#t` 曾各只替换第一处；`#t` 只查 `Consume.img`；
闭合 `#` 未吃掉。`#i/#v` 曾整段剥除；`#s/#f` 未接线。

## 实现

| 令牌 | 行为 |
| --- | --- |
| `#h…#` / `#h #` / `#h0#` | 全量 → 角色名 |
| `#p#` / `#p<id>#` | 全量 → NPC 名（空 body 用对话 npcid） |
| `#t<id>#` / `#z<id>#` | 全量 → `ItemData::get(id).get_name()`（全品类） |
| `#i<id>#` / `#v<id>#` | **保留**；`LayoutBuilder` 行内画 `info/icon`（IDA type1） |
| `#s<id>#` | **保留**；行内画技能 `icon`（IDA type1） |
| `#f<path>#` / `#F<path>#` | **保留**；行内画 WZ 路径 canvas（IDA type2） |

闭合 `#` 一并消费；非法/缺图令牌跳过不崩。打字机对 `#i/#v/#s/#f` 整段原子喂入。
首行大图标基线按 IDA 行盒底对齐微调（见 evidence）。

## 文件

- `IO/UITypes/UINpcTalk.cpp`：`format_text` + 打字机
- `Graphics/Text.h` / `Text.cpp`：`Layout::Icon`（持 Texture）+ 绘制
- `Graphics/GraphicsGL.h` / `GraphicsGL.cpp`：`LayoutBuilder` 解析与占位
- `evidence/ida-i-v-icon-layout.md`、`evidence/ida-f-s-icon-layout.md`

## 验证

- 构建：`tools/build_smoke.bat`
- 真机（待测）：`#i…##t…#` 见图标+名；`#fUI/…#` / `#s…#` 见图；折行不露令牌残片
