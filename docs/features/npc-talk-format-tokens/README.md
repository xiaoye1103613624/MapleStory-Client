# C6 · NPC 对话 `#p/#h/#t/#z` 全量展开

> 日期：2026-11-05 ｜ 纯客户端 ｜ 承接 open-issues §C6 残留口子

## 问题

`UINpcTalk::format_text` 对 `#p` / `#h` / `#t` **各只替换第一处**；`#t` 只查 `Consume.img`；
闭合 `#` 未吃掉。一段正文多个 `#t<id>#`（常见于 `#i…##t…#`）会露出原始令牌。

## 实现

| 令牌 | 行为 |
| --- | --- |
| `#h…#` / `#h #` / `#h0#` | 全量 → 角色名 |
| `#p#` / `#p<id>#` | 全量 → NPC 名（空 body 用对话 npcid） |
| `#t<id>#` / `#z<id>#` | 全量 → `ItemData::get(id).get_name()`（全品类） |
| `#i<id>#` / `#v<id>#` | 整段剥除（LayoutBuilder 尚未画图标；防数字残片） |

闭合 `#` 一并替换；非法令牌跳过不崩。

## 文件

- `IO/UITypes/UINpcTalk.cpp`：`format_text`
- `notes/open-issues.md` §C6 状态更新

## 验证

- 构建：`tools/build_smoke.bat`
- 真机：NPC 对话含多处 `#t` / `#i…##t…#` 选项，应只见道具名、无令牌残片
