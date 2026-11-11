# round94 · reopen 残余 17 条 P1/P2/P3 分流（账目轮）

**触发**：User said (m11928)「处理掉：剩余 reopen 中 21 条属 P1/P2/P3 分流；DECISIONS.md 11 处待拍板是哪些？算术未动指的是什么计算？客户端还是服务端？」

## 1. 先纠算："21 条" 是错的

上一轮答复说「剩余 reopen 中 21 条」——漏算 round91 的 3 条注销（`corewindow/D8`、`skill/D4`、`quest/D8`）。
现场实数：27 条 reopen 已被前四轮注销 **10** 条，**残余 17 条**。

| 轮次 | 注销 | 明细 |
| --- | --- | --- |
| round90 | 2 | `drop/D2` → F 维 P2（缺 COM 渲染栈）；`drop/D12` → W 桶 wire 宽度分叉 |
| round91 | 3 | `corewindow/D8` → DOC_ONLY 已发货；`skill/D4` → 架构级不新增；`quest/D8` → W 桶 wire 条件追加段 |
| round92 | 4 | `corewindow/D9`/`skill/D12`/`login-flow/D9`/`statusbar-hud/D12` → 全是**编号错配** |
| round93 | 1 | `activity-minigame/D1` → 095 注释改判本端扩展 |

## 2. 分流结果：P1 = 0 / P2 = 1 / P3 = 16

- **P2（1）**：`login-flow/D3` 骑士团条件解码 —— 缺 s9 是否发追加段的协议事实，拿到抓包即升 P1。
- **P3（16）**：`login-flow/D12`、`login-flow/D13`、`statusbar-hud/D1`、`corewindow/D2`、`corewindow/D10`、
  `social-economy/D3`、`social-economy/D9`、`social-economy/D12`、`pet-summon/D2`、`cashshop/D2`、
  `skill/D9`、`skill/D11`、`map/D1`、`map/D4`、`map/D8`、`quest/D5`。
- **P1 = 0**：P1 门槛三条并发（083 状态机证据 + 零 wire + 当前端确有缺陷），残余 17 条的 canonical 裁定
  原文全是「保持 / 不补 / 不做 / 不改」，属**已裁定不实施**，硬塞 P1 就是 round90/91/92 那类「矩阵文本→P1 登记」同族病。

## 3. DECISIONS.md 11 处「待裁决」的真相

这 11 行虽带「待裁决」「待拍板」字样，但**裁定状态已定**——字样是 round86 批量裁决时留下的历史措辞：

| 行 | 面 | ID | 状态 |
| --- | --- | --- | --- |
| 132 | pet-summon | D3 | `P0_SHIPPED`（round78 已判合法混合面） |
| 133 | pet-summon | D4 | `BATCH_4_STANCE`（维持不做） |
| 136 | pet-summon | D7 | `P2_LIVE`（先出像素改色对照） |
| 142 | pet-summon | D13 | `BATCH_4_STANCE`（资源分列） |
| 151 | activity-minigame | D3 | `BATCH_4_STANCE`（wire later 壳） |
| 152 | activity-minigame | D4 | `BATCH_4_STANCE`（黄字降级） |
| 158 | activity-minigame | D10 | `KEEP_S9`（s9 无 295 LEAVE opcode） |
| 248 | quest | D5 | `KEEP_CURRENT`（P3，083 无 Quest OnKey） |
| 265 | drop | D3 | `KEEP_S9`（s9 无掉落归属超时语义） |
| 271 | drop | D9 | `KEEP_CURRENT`（P3 增强） |
| 272 | drop | D10 | `KEEP_S9`（按 s9 移除语义） |

**结论**：不是 11 个待拍板决策，是 **11 行历史措辞**。真要拍板的是 0 个。
若要让文档自洽，正确做法是把这 11 行的「待裁决」改成对应状态名——属文档债，登记 P3，本轮不动
（改写会动 DECISIONS.md 344 行里 11 行的措辞，风险低但会与 round86 已提交记录不一致，须单列一轮做）。

## 4. 「算术未动」= 客户端校准决策台账的条数闭合

**不是**客户端代码里的任何数值，**不是**服务端逻辑，**不是** GATE 的 opcode 计数。是这张决策表的分母：

| 公式 | 含义 |
| --- | --- |
| `168 = PURE 76 + WIRE 38 + MIXED 14 + OTHER 40` | round88 四分类，每轮复用不复算 |
| `168 = W 轨 38 + L 轨 130` | WLE 三桶路由，W 轨 = `class:WIRE` |
| `reopen 27 = 25 P1 候选 + 1 W 桶 + 1 F 维 P2` | round90 登记层口径 |
| `reopen 27 = 已注销 10 + 残余 17` | 本轮 |
| `残余 17 = P1 0 + P2 1 + P3 16` | 本轮 |

**为什么必须守恒**：任一数字被单方面改动（注销却不记账、语义再分类、把 P2 算进 P1），168 或 27 的分母
就断裂，后续每轮的进度百分比全失真。所以每轮只动「登记层」，**分母永不变**。
这也是为什么 round90/91/92 三次纠偏都只在 P1 登记层注销条目、从不做语义再分类。

## 5. 本轮性质与验证

- **账目轮**：零源码改动 / 零 wire / 零协议 / 零 WZ；产物 = WLE §11 + dev-plan 2-42 + PLT §53 + evidence + 本 notes。
- 外部在途 WIP 零夹带（`IO/UIStateGame.cpp` 的 UIMedal hunk 等全部原样留工作树）。
- 验证见 harness `tools/test_round94_reopen_triage.py` 与 GATE。
