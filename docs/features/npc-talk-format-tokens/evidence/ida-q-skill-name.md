# IDA：`#q` 技能名展开

> 日期：2026-11-05 ｜ IDB：`BeiDou.exe.i64`（`E:\MXD\BeiDou-ClientV17.7`）

## 令牌分类

| 证据 | 结论 |
| --- | --- |
| `sub_9A2DE0`（`0x9A2DE0`） | 小写 `'q'`（113）落入默认支 → enum **17**（与 `#t/#z/#p` 等「正文名」族同槽；**不是**图标 type） |
| 大写 `'Q'` | enum **14**（时限 `daylimit/hourlimit/…`，**与技能名无关**） |

## case 17 正文名分发（`sub_99DD54` / `jpt_99E499`）

`atoi(token+2)`（`sub_9A262F`）后读 `token[1]`：

| `token[1]` | 分支要点 |
| --- | --- |
| `'p'`（112） | NPC 名（string id 1604 系） |
| **`'q'`（113）** | `sprintf("%07d", id)` → `sub_98D0C5` 取 **Skill 名串** → 插入正文元素（`LABEL_494`） |
| `'t'`（116） | `sub_5CF63E` 道具名 |
| `'z'` 等 | 同类正文替换 |

`#q` **不**走 type1/2 canvas；展开后按普通文本量宽排版。

## 本仓映射

- `UINpcTalk::format_text`：`#q<id>#` → `SkillData::get(id).get_name()`（内部已 `extend_id(_,7)` + `String/Skill.img`）
- 闭合 `#` 一并消费；非法/空名：跳过本段，不崩
- LayoutBuilder `case 'q'`：仅兜底吞 `#q`（正常路径不应再见到）

## 色标打字机（同轮附带）

| 证据 | 结论 |
| --- | --- |
| LayoutBuilder `#b/#r/#e/#n/#k…` | 均为 **`skip += 2`** 的两字节色标/样式 |
| 打字机逐字节 | 先喂 `#` 会被「孤立 `#`」吞掉，下一拍字母当正文 → **半截闪现** |
| 修法 | 对 `b/d/e/E/g/k/n/N/r/l` **一次 take=2**；图标令牌仍吃到闭合 `#` |

## 局限

- 不真机；待测脚本串含 `#q…#` 与 `#b…#r…`
- `#c/#B/#m/#o/#x` 带 body 的令牌未在本轮展开/原子化（仍登记）
