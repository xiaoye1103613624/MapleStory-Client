# P3 C 批 2 条 083 三向对拍结论（round99）

> 口径：`083/v83 only 禁 095`；`s9 协议优先`（W 桶 6 字段宽度/类型 + 27 wire KEEP 零改动）；
> `数值重合 ≠ 语义重合`；`校准 ≠ 改回原生`（L 桶同字段原始逻辑按 083 还原）。
> 本轮 2 条（批 C 余 2 条）**全部零 wire / 零协议 / 零 WZ / 零源码行为改动**。
> 每条结论必附四项：083 符号或反编译、s9 对端、当前端字面量、DECISIONS 原裁定。

## 0. 结论总表

| # | 决策点 | 判定 | 依据强度 | 本轮动作 |
| --- | --- | --- | --- | --- |
| 15 | `corewindow/D2` 083 原生 vs s9 协议冲突 | `KEEP`（s9 协议优先，BATCH_0_BASELINE） | 083 符号族 + DECISIONS 原裁定 | 只出对照，零改动 |
| 16 | `quest/D5` UIQuestLog::send_key | `KEEP_CURRENT`（DOC_ONLY，空实现维持） | 083 全库 `*quest*key*`=0 + 当前端空 stub | 只出对照，零改动 |

## 1. `corewindow/D2` — 083 原生 vs s9 协议冲突，维持 s9 协议优先

**083 侧（符号族，本轮重取）**

- 083 核心窗完整存在，属原生参考全集：`CUIStat` / `CUIKeyConfig`（ctor `0x832586`、OnButtonClicked `0x8335d0`、OnMouseButton `0x83372c`、OnMouseMove `0x8338f9`）/ `CUISkill` / `CUIItem`（ctor `0x81c414`、OnCreate `0x81c6c9`）/ `CUIEquip`（ctor `0x7fde7c`、OnButtonClicked `0x7feb72`、TogglePetEquip `0x7ffa84`）等。
- `*OnKey*` 在 083 仅出现在 `CCashShop`(`0x469626`)/`CDialog`(`0x4edd21`)/`CCtrlMLEdit`(`0x4d3159`)/`CField`(`0x529968`)/`CCSWnd_*` 等对话框；**核心四窗的键处理走 `OnButtonClicked`/`OnChildNotify`，而非 `send_key` 形态**。

**s9 侧（协议权威）**

- 当前端核心窗由 s9 协议驱动（s9 handler 收包 → 当前端渲染），见 DECISIONS §0 T1「s9 协议优先 / Angel 仅作证据校核源」。

**当前端侧**

- 核心窗（Stat/KeyConfig/Skill/Item/Equip）实现以 s9 协议为准，Angel/083 仅作反编译校核；差异逐项标注于各窗 README（如 `skill-angel-calibration-2026-11`、`corewindow-angel-calibration` 等）。

**DECISIONS 原裁定**：`corewindow/D2`「083 原生证据 vs s9 协议，冲突以谁为准？ → 保持 s9 协议优先，Angel 仅作证据校核源，差异逐项标注。改回原生是新协议。」`BATCH_0_BASELINE` | s9 协议优先 / Angel 083 仅作证据校核源。

**判定**：`KEEP`。本轮重取 083 符号族确认核心窗原生全集可校核，但「冲突以 s9 协议优先」的基线政策不变；**不将任何核心窗改回 083 原生逻辑**（那是新协议，越界）。零改动。

## 2. `quest/D5` — UIQuestLog::send_key 是空实现，083 无对应键处理

**083 侧（反编译 + 符号，本轮重取）**

- 083 任务 UI 类：`CUIQuestInfo`（OnCreate `0x87ec3f`、OnChildNotify `0x87fb2f`、OnTabChanged `0x87fd4f`、LoadData `0x881b18`、ToggleDetail `0x88422c`、OnToggleDetail `0x8843a4`、MarkQuestAsRead `0x882f2f`、SetScrollBar `0x883100`、ResetInfo `0x882f88`…）/ `CUIQuestAlarm`（Draw `0x885df1`、DeleteQuest `0x885df1`、RemoveOldQuest `0x887b5c`）/ `CUIQuestInfoDetail`（Draw `0x87c515`、ResetInfo `0x87c515`）。
- 任务窗键处理**走 `OnChildNotify`（按钮通知）+ `OnCreate`/`OnTabChanged`**，不是 `send_key` 形态。
- 符号检索（083 全库，`ida-pro-mcp` @127.0.0.1:13339 `func_query` name_regex）：
  - `.*quest.*onkey.*` → `data: []`（0 命中）
  - `.*quest.*sendkey.*` → `data: []`（0 命中）
  - `.*quest.*key.*`（宽） → `data: []`（0 命中）
  - `.*UIQuestLog.*` / `.*CUIQuestLog.*` → `data: []`（083 连「QuestLog」类名都没有，用 `CUIQuestInfo` 系列）
  - 对照：`*OnKey*` 全库存在（`CCashShop`/`CDialog`/`CCtrlMLEdit`/`CField`/`CCSWnd_*` 等）证明检索通路正常、非工具空转。

**当前端侧**

- `IO/UITypes/UIQuestLog.cpp:443` `void UIQuestLog::send_key(int32_t keycode, bool pressed, bool escape) {}` —— **空实现**（不处理任何键）。

**s9 侧**

- 任务窗由 s9 协议驱动（Quest 相关收包 → 当前端 `UIQuestLog` 渲染），键交互本就不在 `send_key` 路径；083 亦无 `send_key` 形态，两端一致「无键处理」。

**DECISIONS 原裁定**：`quest/D5`「`UIQuestLog::send_key`，s9 优先，不补当前键位接线注释，标注 P3。」`KEEP_CURRENT`。

**判定**：`KEEP_CURRENT`（DOC_ONLY）。083 全库 `*quest*key*`=0 坐实「083 无任务窗键处理」；当前端空 `send_key` stub 与 083 行为一致（两端都不经 `send_key` 处理键），**不补键接线、不改空实现**。零改动。

## 3. 算术不动

- `168 = PURE76 + WIRE38 + MIXED14 + OTHER40`（round88 三分类，本轮零改）
- `168 = W轨38 + L轨130`（WLE §4，本轮零改）
- `reopen 27 = 已注销 10 + 残余 17`；`残余 17 = P1 0 + P2 1 + P3 16`（round94，本轮零改）
- P3 共 16 条，A(9)+B(5) 已于 round96/97/98 收口，**本轮 C(2) 收口 → P3 全部 16 条完成**；不新增分母、不注销、只补判定层与事实层（`*quest*key*`=0 / 当前端空 stub / 083 核心窗全集可校核）。

## 4. 验证

- 本轮零源码改动 → 只做文档，不做构建门禁回归，**不声称真重编**（同 round98 §7）。
- 083 证据经 `ida-pro-mcp`（Angel.exe @127.0.0.1:13339）`func_query` name_regex 实查；检索通路以 `*OnKey*` 全库命中校验正常。
- 提交零署名（`git log -1 --format=%B` 签名行 0）。

## 5. 约束

- 083/v83 only，禁引 095
- s9 协议优先：W 桶 6 字段宽度/类型 + 27 条 wire KEEP 全程零改动
- 数值重合 ≠ 语义重合
- 校准 ≠ 改回原生（L 桶同字段原始逻辑按 083 还原）
- WZ/IMG/XML 一律先 orange-wz MCP（128B `WzAESConstant.DEFAULT_KEY`）
- 门禁 5 禁想象修改 / 门禁 18 REA→IDA→orange-wz→.nx 只读取证
