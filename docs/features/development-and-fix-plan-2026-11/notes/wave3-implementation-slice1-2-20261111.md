# round92 实施片 1 + 片 2 · 编号错配纠偏 + UIChannel 下溢修复（2026-11-11）

> 触发 m10905「开始下面的计划」→ 进入 NEXT-BATCH-PLAN.md §5 实施阶段。
> 本轮不再从矩阵文本推导批次，先四方取证再定性。

## 1. 片 1（skill/D12 / corewindow/D9 → 宏选中态）= DOC_ONLY 编号错配

- round91 Plan §4 把 `CUIMacroSysEx::OnSelected @0x8a984d` 挂到这两个编号下，判为「L 桶纯逻辑补丁」。
- DECISIONS 原文：`corewindow/D9`（:100）真议题是 32 格→5 槽数量、`skill/D12`（:198）真议题是
  `deactivate()` 与 `ShiftMacroUIState` 形态差异 —— **都不是宏选中态状态机**。
- 三项现场证据坐实「无干净补丁」：
  1. orange-wz 128B 门禁 `search_Shout/shout/BtShout` 全 `matches:[]`，`Check` 仅 `Memo/check0/1`
     → 083 三个宏资源根（`Macro`/`SkillMacro`/`SkillMacroEx`）都无 shout 勾选框位图；
  2. `CUIMacroSys::OnCreate @0x8a8504:98-114` 是 COM 控件栈（`ZAllocEx::Alloc(108)`
     `CCtrlCheckBox` id=3011 @(166,241) + `CCtrlEdit`），当前端无 COM 控件体系；
  3. 当前端全仓 Checkbox 实现类 0（仅 `UIOptionMenu.h:78` 注释），`shout` 只在数据层/发包层出现。
- 当前端 `UISkillMacroEx` 是 5 槽技能组合栏（corewindow README:13 明确「不是 62 类一一对应」），
  `UIMiscShells.cpp:4880-4884` 已用 `shown_name` 变更检测等价实现 083 `CCtrlEdit::SetText`。
- **判定：零源码改动**，仅登记编号错配。

## 2. 片 2（login-flow/D9 / statusbar-hud/D12 → 频道网格）= 编号错配 + 真实 L 桶缺陷

- DECISIONS 原文：`login-flow/D9`（:62）= 60 频道翻页控件（P3 增强 KEEP）、
  `statusbar-hud/D12`（:84）= `UIMobGage.*` 空 Type（保持）——也都不是键盘导航。
- 但对应窗 `CUIChannelShift`（083 游戏内换频，6 列）↔ 当前端 `UIChannel`（`SLOTS_PER_ROW=6`）
  的门控对拍挖到真实缺陷：
  - 083 `CUIChannelShift::OnKey @0x995863` 门控 `count>=2` 才放 LEFT/RIGHT、`count>=7` 才放 UP/DOWN；
    `CUIChannelShift::CUIChannelShift @0x994bcf` 中 `*(this+32)=*(*dword_BE7918+8280)` 即频道总数，
    `v4 = 21*(v3/6)+125` 按 6 列分行决定窗高。
  - 当前端 `UIChannel::step_channel`（:179-206）单次 wrap + `send_key`（:220-235）无门控。
    本服 `channel_count=3` 时 UP(-6) 让 `sel-6+3` 仍为负 → `uint8_t` 下溢 253/254/255，
    DOWN(+6) 得 3/4/5 越界（`tools/test_round92_channel_nav_083.py` B1 逐值复现）。
- **补丁**：`UIChannel::send_key` 的 UP|DOWN 合并分支加 `if (channel_count > SLOTS_PER_ROW)`、
  LEFT|RIGHT 加 `if (channel_count > 1)`，逐字还原 083 门控语义；`step_channel` 函数体零改动。
  `count>=7` 时 `delta=±6` 单次 wrap 数学闭合（B3 穷举证明），无需改取模算法。
- 零 wire / 零协议 / 零 WZ 改写 / 零新封包。

## 3. 同族病升级为 harness 守卫

- 「矩阵文本→P1 登记层错配」已连续第 4 轮（round90 drop → round91 三候选 → round92 四编号）。
- round91 WLE §8 已写硬规则「登记前必附 083 反编译 + s9 handler + 当前端字面量 + DECISIONS 四项」，
  但**同批生成的 Plan §4 自己没回扫** —— 规则写了没落地等于没写。
- 本轮 WLE §9 写死：任何 P1 候选登记必须四项齐备；harness A12 对 NEXT-BATCH-PLAN.md §4
  三个候选逐条断言 083 锚点存在。

## 4. 算术与限制

- 168 条 DECISIONS 历史裁定 / WLE §4 `38+130=168` / reopen `27=25 P1+1 W+1 F-P2` **全不动**，
  仅在 P1 登记层注销 4 个错配编号（不改语义再分类）。
- W 桶 6 字段宽度/类型 + 27 条 wire KEEP 零改动；`quest/D8`/`skill/D4`/`corewindow/D8` 零改动。
- 真机五屏观感（含本服 3 频道换频窗 ↑/↓、多频道服跨行）由用户认领，见 PLT §51。
