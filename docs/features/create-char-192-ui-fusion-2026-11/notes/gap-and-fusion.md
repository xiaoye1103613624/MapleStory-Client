# 本端 vs 192 创角融合差距与推荐

## 本端现状（BeiDou-Client-xy）

| 页 | 路径 | 资源 | 行为 |
| --- | --- | --- | --- |
| 页1 职业团 | classic `UIRaceSelect::init_classic` | `Login.img/RaceSelect` 仅 `knight/normal/aran` | 代码扩成五卡；龙神/双刀 **回落 aran/normal 图** + 中文标签 |
| 页2 命名/外观 | classic `UICommonCreation` | `NewChar*` + MakeCharInfo Info/Premium/Orient | 龙神/双刀复用战神模板/场景；发包 job=3/4 |
| 探测开关 | `LoginUiAssets::use_classic_race_select` | 有 RaceSelect 则 **强制 classic** | 注释写明：modern 曾导致 Dual/Evan 不可见 |

`nx_inspect`：xy `Login.img` **无** `RaceSelect_new` / `CustomizeChar`。

技能/创角逻辑波次（`job-expansion-192-align`）：骑士团四转、双刀、龙神资源与 s9 Creator job 3/4 **已落地**；高版本其它职业团未开创角。

## 192 现状

| 页 | 路径 | 资源 |
| --- | --- | --- |
| 页1 | `RaceSelect_new` 书架 + 翻页 | UI_001.wz；`order/192` 约 30 职业；button/4、/8 独立 |
| 页2 | `CustomizeChar/<jobId>` | 含 2001 / 000_1 等专属板 |
| 经典 RaceSelect | **不存在** | — |

192 主程序强加壳 → 行为权威继续靠 WZ/XML + 本端 Angel/s9 协议，不靠 192 exe 静态反编译。

## 可行性摘要

1. **高版本职业「玩法逻辑」移植**：对已开的双刀/龙神 **可行且大半完成**；再扩 Mercedes/恶魔等属新波次（Skill/String/Character/脚本/CREATE_CHAR/开关），与「是否换职业团 UI」可解耦。
2. **采用 192 全量职业团 UI**：**资源上可行**（已有完整 RaceSelect_new），但落地成本高：跨钥入库、本端 Login 双体系冲突、modern 禁用表/order 键、页2 CustomizeChar、未开放职业的服务端与外观池。
3. **轻量补龙神/双刀专属卡到 classic**：**架构已预留**（`RaceSelect["evan"|"dual"]`），可从 192 `button/4`、`button/8`（及 Back0/Back2/Back3 说明图按需）裁切/改写成 classic 四槽后生效，**不改协议**。

## 推荐默认（待用户确认）

**近端闭环：方案 A（classic 骨架 + 192 抽卡）**  
保留已校准的 classic 页1/页2 与 job 0–4；从 192 转钥导入龙神/双刀卡图进 `RaceSelect/evan`、`RaceSelect/dual`；页2 外观板可二期再接 `CustomizeChar/2001`/`000_1`。

**战略可选项：方案 B/C**（全量或过滤版 RaceSelect_new）见 README 决策表。
