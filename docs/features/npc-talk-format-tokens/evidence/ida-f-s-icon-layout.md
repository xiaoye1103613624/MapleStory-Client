# IDA：`#f` 路径图 / `#s` 技能图

> 日期：2026-11-05 ｜ IDB：`BeiDou.exe.i64`（`E:\MXD\BeiDou-ClientV17.7`）

## 令牌 → 元素类型

| 证据 | 结论 |
| --- | --- |
| `sub_9A2DE0`（`0x9A2DE0`） | `'f'` → enum **10**；`'F'` → enum **11**；`'s'` → enum **9** |
| `jpt_99E499` case 9 | `#s`：`*elem = 1`（与 #i/#v 同 type1） |
| case 9 体 | `atoi(token+2)` → `sub_75C755`（CSkillInfo）→ icon @+0xAC；`GetWidth`/`GetHeight` → elem[8]/[9] |
| case 10/11 | `#f/#F`：`*elem = 2`；路径 = `Str+2`；IWzProperty 解析 canvas；同取宽高 |

## 行内排版 / 绘制

与 `ida-i-v-icon-layout.md` 共用：

- type∈{1,2,4}：水平推进 `cursor += width`，超宽换行
- 行高 `max(h,16)`，行距 +2；`y_top = line_y + max_h - elem_h`（底对齐）
- `sub_9A3E9F` case 1/2：同 canvas blit，α=255

## 路径样例（脚本/原生串）

- `#fUI/UIWindow.img/QuestIcon/7/0#`
- `#fEffect/CharacterEff/1112908/0/1#`
- `#s1001005#`（技能 id）

## 本仓映射

- `LayoutBuilder`：`#s` → `SkillData::get_icon(NORMAL)`；`#f/#F` → 根名映射 `nl::nx::*` + `resolve(rest)`
- `Layout::Icon` 布局期持有 `Texture`；绘制底对齐基线
- **首行大图标**：`add_line` 在首行有图标且 `line_y < max(h,16)` 时抬高基线，避免图标顶伸到文本原点上方；`finish` 高度跟到抬高后的行盒底

## 局限

- 未接的其它色标 / `#q` 技能名等仍为 TODO
- `#f` 未知根名或缺节点：吞令牌、宽 0（不崩、不露残片）
- 不真机；待测 NPC/任务奖励串含 `#f…#` / `#s…#`
