# 技能特效资源挨个校验（全量 92 img · orange-wz 实测 2026-11-24）

> 用户质疑「确定缺资源？技能挨个校验一遍」。本轮对 live `E:/MXD/BeiDou-Client-xy/Data/Skill` 下**全部 92 个 Skill/*.img** 用 orange-wz MCP（128B `WzAESConstant.DEFAULT_KEY` + IV `TSPHKw==`，`load_files exclusive:false` → `query_nodes queries 批量 search` → `unload_all`，isError:false）逐 img 扫描 11 类特效节点。原始输出 `skillfx-full-scan.json` / `skillfx-exact-scan.json`。**纯只读，未改任何源码/资源；真机观感未验。**

## 0. 结论先行：之前"技能特效族资源缺源"的判断**错了**
之前只探 `Skill/210.img` 就断言 prepare/keydown/screen/special/tile "资源缺源、只预留不造"——全量实测证明**除了 receiver 之外，每一类特效族都在 live WZ 大量存在**。这是我以偏概全的错误，本文件纠正并作为权威口径。

## 1. 全量逐类实测结果（精确匹配：节点末段名 == 类别名）

| 特效族 | 含该节点的 img 数 | 节点总数 | 之前判断 | 实测结论 |
|---|---|---|---|---|
| effect | 82 / 92 | 647 | 有（已知） | ✅ 大量存在 |
| hit | 77 / 92 | 378 | 有（已知） | ✅ 大量存在 |
| **prepare** | **22** | 25 | ❌误判"缺源" | ✅ **存在**（112/1212/1311/1411/1412/1510/520…） |
| **keydown** | **17** | 19 | ❌误判"缺源" | ✅ **存在**（1112/1212/1311/1412/520/522…） |
| **keydownend** | **8** | 8 | ❌误判"缺源" | ✅ **存在**（1212/1311/1312/1412/2217/312/434/522） |
| **screen** | **8** | 14 | ❌误判"缺源" | ✅ **存在**（000/1000/1212/1312/1412/1512/2000/2001） |
| **special** | **31** | 54 | ❌误判"缺源" | ✅ **大量存在**（000/1112/1212/1311/1411/2211/520…） |
| **tile** | **14** | 23 | ❌误判"缺源" | ✅ **存在**（1211/1411/1412/211/212/2217/311/312/322/422/512/MobSkill…） |
| **finish** | **3** | 7 | ❌误判"全库 0" | ✅ **存在**（111.img/1111.img/520.img） |
| **receiver** | **0** | 0 | 缺源 | ✅ 全量确认 **真·全局缺源**（唯一确实缺的一类） |

> 变体节点也存在但不计入核心类：`effect0/effect1/effect_1H`、`hit0/hit1/hitAfter`、`prepare0/prepare_1H`、`keydown0/keydown_1H`、`specialAction/specialActionFrame`。这些是同一特效族的职业/武器变体，进一步说明资源齐全。

## 2. 逐家族代表技能（可复核锚点）
- **prepare**：112.img `skill/1121001/prepare`、1412.img `skill/14121003/prepare`、520.img 等 22 个 img。
- **keydown / keydownend**：1112.img `skill/11121052/keydown`、1311.img `skill/13111002/keydown`+`keydownend`、1412.img `skill/14121004/keydown`+`keydownend`。
- **screen**：000.img `skill/0001009/screen`、1412.img `skill/14121052/screen`、1000/2000/2001.img（英雄/圣骑士类全屏特效）。
- **special**：1411.img、1311.img、2211.img `skill/22110014/special`、000.img `skill/0001014/special`、520.img 等 31 img；含 specialAction/specialActionFrame 变体。
- **tile**：1412.img `skill/14121004/tile`（含 effectDistance）、1211.img `skill/12111003/tile`、MobSkill.img（怪物技能也有 tile）。
- **finish**：111.img `skill/1111003/finish`、1111.img `skill/11111002/finish`、520.img `skill/5201005/finish`（此前说"finish 全库 0"错误）。

## 3. 对代码的影响与纠正
1. **ANIMFX-PHASE0.md / ROUND2-README.md 中「prepare/keydown/screen/special/tile 资源缺源、finish/receiver 全库 0」的表述作废**，以本文件为准：只有 `receiver` 真缺，其余 8 类（含 finish）都有源。
2. 上一轮已实装的 `ExtraNodeUseEffect`（Skill.cpp:186-208 挂接 prepare/keydown/keydownend/screen/special/tile）**方向正确**，当时基于"1412.img 有源"的部分实证；本轮全量证实它对 22/17/8/8/31/14 个 img 的技能都适用，而非极少数。
3. `finish`（3 img / 7 节点）目前**未挂接**——下一步可评估是否为 `5201005`（恶魔复仇者类）等补 finish 播放（需先确认原生语义，属单项）。
4. `receiver` 全库 0 命中，与 095/192 三端一致（ev-res 已交叉），维持"真缺、不造"结论。
5. special/tile 的**位移/延时参数**（x/y/fall/start/interval/count/duration、effectDistance）资源在位，但当前 `ExtraNodeUseEffect` 只做"原地逐帧动画"降级；已实装的 `Char::show_falling_effect/show_particle_effect/show_offset_effect` + EffectLayer `add_fall/add_particle/add_offset` 提供了完整能力，逐技能接线（把 special 参数翻译成 fall/offset 调用）是下一步可做项，需逐技能 REA/WZ 证据定时序。

## 4. 下一步建议（待人拍板）
- **A**：为 `finish`（3 img）补播放挂接（低风险，与现有 ExtraNodeUseEffect 同构）。
- **B**：选 1-2 个带完整 special 参数的技能（如 13111000 的 x/y/fall/interval），把 special 接到 `show_falling_effect`，实现原生雨滴特效（需 REA/WZ 定时序，属单项立项）。
- **C**：tile 的 effectDistance 落地（接到 foothold 距离判定）——改动面大，单独评估。

## 5. 声明
全量 orange-wz 实测（92 img，逐类批量 search，精确末段名匹配），原始 JSON 可复核。未改源码/资源，未编译，未真机发包。截图未采信（来源端未确认）。此前"资源缺源"结论的过错已如实纠正。
