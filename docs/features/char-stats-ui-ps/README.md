# 角色能力详情页（Photoshop · 中英双版）

对齐用户参考图：粉/绿/深灰左栏 + 详情右栏，数值框留空。  
**v1** 初版；**v2** 详细资料扩展；**v3** 标签宽统一 + 数值框分档 + 左栏补全 AP/Job/Damage；  
**v4** 彩色双列跟附图；标签 80 左对齐；值框 80 / 仅物理攻击力 240；左右同高；清晰 11pt 雅黑。  
各阶段产物**分目录保留，禁止覆盖**。

## 版本目录映射（强制）

| 阶段 | 目录（脚本实际） | 说明 |
| --- | --- | --- |
| V1 | `preview/`（根下默认文件，无子目录） | 历史路径保留；等价「V1」 |
| V2 | `preview/v2/` | 小写 `v2`（已有，不动） |
| V3 | `preview/v3/` | 小写 `v3` |
| V4 | `preview/v4/` | 小写 `v4`（新建）；文档中写作 V4 |

图层缓存同理：`layers/`、`layers/v2/`、`layers/v3/`、`layers/v4/`。  
之后新阶段继续用 `preview/v5/`…，**不得覆盖**旧阶段文件。

## 目录

```text
docs/features/char-stats-ui-ps/
  Character_Stats_Detail.psd              # v1 PSD
  Character_Stats_Detail_v2.psd           # v2 PSD（另存）
  Character_Stats_Detail_V3.psd           # v3 PSD
  Character_Stats_Detail_V4.psd           # v4 PSD（CN 可见；EN 图层隐藏）
  preview/                                # === V1 ===
  preview/v2/                             # === V2 ===
  preview/v3/                             # === V3 ===
  preview/v4/                             # === V4 ===
    Character_Stats_Detail_v4_*.png
    Character_Stats_Main_v4_*.png
    Character_Stats_DetailPanel_v4_*.png
    Character_Stats_Detail_v4_EN_CN_sheet.png
  ui/detail/v4/manifest.md
  layers/  layers/v2/  layers/v3/  layers/v4/
  compose_meta*.txt  README.md
```

## v1 / v2 / v3 / v4

| | v1 | v2 | v3 | v4 |
| --- | --- | --- | --- | --- |
| 输出 | `preview/*` | `preview/v2/*_v2_*` | `preview/v3/*_v3_*` | `preview/v4/*_v4_*` |
| 右栏宽 | ~420 | ~380 | **~440** | **352** |
| 右栏列 | 全宽 + 双列 | 重要行 2 列，其余最多 3 列 | 同 v2 | **通栏物攻 + 双列**（无分区条） |
| 标签宽 | 全宽/双列两套 | 随槽位伸缩 | **统一 66** | **统一 80**，字左对齐 |
| 数值框 | 偏宽 | 按 format 多档 | 多数 34；ATK/魔攻区间 144 | **多数 80**；**仅物理攻击力 240** |
| 左栏 | Name/Level/… | 同 v1 | Job/Damage/AP | 同 v3 字段；标签宽 **80**；字号 11 |
| 高度 | 各自测量 | 各自测量 | 左 392 / 右 347 | **左右同高 392** |

## 布局（v4）

| 面板 | 尺寸 | 结构 |
| --- | --- | --- |
| 左 · 角色资料 | **300×392** | 粉身份（Job/Damage/AP）→ 绿四维 → 深灰 3×2 抗性；标签 80 |
| 右 · 详细资料 | **352×392** | 通栏物理攻击力（80+240）→ 彩色标签双列（80+80）；行距均分对齐左栏 |

并排画布：**658×392**（300 + gap6 + 352）。数值留空；预览不烤按钮。

## 色标

| 用途 | 色 |
| --- | --- |
| 身份/资源标签 | 粉 `#D66293` |
| 四维标签 | 浅绿 `#B5CF7B` |
| 抗性标签 | 深灰 `#555555` |
| 详情分区条（v1–v3） | 深蓝 `#59749B` |
| 详情属性行标签（v1–v3） | 蓝 `#6E93B5` |
| v4 通用战斗 | `#5B8BB8` |
| v4 暴击几率 / 暴击伤害 | 红 `#C44545` / 绿 `#5BA85B` |
| v4 伤害 / 最终 / 首领 / 无视 | `#3D5A80` / `#D45B8C` / `#7B5EA7` / `#2AA5A5` |
| v4 异常抗性 / Buff / 稳如泰山 | `#8B3A3A` / `#3D9B7A` / `#8B6914` |
| v4 道具 / 金币 / 荣誉 / 异常细分 | `#D4833A` / `#C9A227` / `#6E93B5` / `#6A7A8C` |

## 字段

**左 · v1/v2**：Name/Level/Guild/HP/MP/EP/EXP/Fame；STR/DEX/INT/LUK；抗性  

**左 · v3/v4**：Name、**Job**、Level、Guild、Fame、**Damage**、HP、MP、**AP**、EXP、EP；STR/DEX/INT/LUK；抗性  

**右 · v1**：攻击力、魔攻、命中；暴击伤害/率；攻速/施法；防御；HP/MP 恢复；移速/跳跃；回避/格挡；异常减免  

**右 · v2/v3 字段集**：同 v2（Boss伤害、额外伤害、最终伤害、无视防御、Buff持续、道具/金币爆率、荣誉等）；v3 仅改标签/数值框几何  

**右 · v4**：通栏物理攻击力；左列魔攻/物防/魔防/命中/回避/手技/攻速/施法/移速/跳跃/暴击几率/暴击伤害/格挡/HP·MP恢复 + 避免流血·点燃·冰缓·感电；右列伤害/额外/最终/首领/无视防御率/状态异常抗性/增益持续/稳如泰山/道具掉落/金币获得/荣誉 + 避免中毒·致盲·晕眩/诅咒减免 + 火/冰/电/神圣抗性

## 重做

```bat
REM v1 → preview/（默认，不改旧路径）
python _tools\compose_char_stats_ui.py

REM 仅 v2 → preview/v2/
python _tools\compose_char_stats_ui.py --v2

REM 仅 v3 → preview/v3/（不覆盖 v1/v2）
python _tools\compose_char_stats_ui.py --v3

REM 仅 v4 → preview/v4/（不覆盖 v1/v2/v3）
python _tools\compose_char_stats_ui.py --v4

REM v1 + v2 + v3 + v4
python _tools\compose_char_stats_ui.py --all
```
