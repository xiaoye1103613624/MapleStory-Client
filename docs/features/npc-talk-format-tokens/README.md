# C6 · NPC 对话格式令牌（`#p/#h/#t/#z` + `#i/#v`）

> 日期：2026-11-05 ｜ 纯客户端 ｜ 承接 open-issues §C6

## 问题

`UINpcTalk::format_text` 对 `#p` / `#h` / `#t` 曾各只替换第一处；`#t` 只查 `Consume.img`；
闭合 `#` 未吃掉。`#i/#v` 曾整段剥除（布局器不会画图）。

## 实现

| 令牌 | 行为 |
| --- | --- |
| `#h…#` / `#h #` / `#h0#` | 全量 → 角色名 |
| `#p#` / `#p<id>#` | 全量 → NPC 名（空 body 用对话 npcid） |
| `#t<id>#` / `#z<id>#` | 全量 → `ItemData::get(id).get_name()`（全品类） |
| `#i<id>#` / `#v<id>#` | **保留**；`LayoutBuilder` 行内画 `info/icon`（IDA type1） |

闭合 `#` 一并消费；非法令牌跳过不崩。打字机对 `#i/#v` 整段原子喂入。

## 文件

- `IO/UITypes/UINpcTalk.cpp`：`format_text` + 打字机
- `Graphics/Text.h` / `Text.cpp`：`Layout::Icon` + 绘制
- `Graphics/GraphicsGL.h` / `GraphicsGL.cpp`：`LayoutBuilder` 解析与占位
- `evidence/ida-i-v-icon-layout.md`：IDA 取证

## 验证

- 构建：`tools/build_smoke.bat`
- 真机（待测）：含 `#i…##t…#` 的 NPC 选项应见图标 + 道具名；折行不露令牌残片
