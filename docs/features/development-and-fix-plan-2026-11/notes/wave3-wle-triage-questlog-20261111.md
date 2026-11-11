# round89 · W/L/E 三桶 + UIQuestLog 真实对齐补丁（wave3）

## 1. 触发与口径

- m09729：字段级 body 改造过的字段/类型不影响，但一样字段的原始逻辑可以对齐；目的是按最完全反编译客户端还原当前客户端原始逻辑和代码，保证基础功能一致，扩展功能可以不对齐。
- m09737：其余功能也按这个方向开发、修复、完善。
- round88 §2.2「body 层不一致→不触发全部对齐」过宽 → 收窄为 W（6 字段保持 s9）/ L（同字段原始逻辑按 083 还原）/ E（扩展不对齐）三桶，历史证据链不删。

## 2. 本轮真实补丁（零 wire）

- `IO/UITypes/UIQuestLog.cpp::change_tab`：`tab = tabid;` 后补 `selected_id = -1;`。
- 083 依据：`CUIQuestInfo::OnTabChanged @0x87fd4f` 首行 `*(this + 369) = -1;`（`tools/_ida_r89/CUIQuestInfo_OnTabChanged.code.txt` 逐字），随后 `LoadData/SetScrollBar/…/TryShowTopQuest/ToggleDetail(0)/SyncScrollBarWithSelected`。
- 当前端 `rebuild_list()` 在 `selected_id` 不在新列表时自动选第一条可见任务；`update_detail()` 对 `selected_id < 0` 有空守卫 → 补这一行即复现 083 语义且无副作用。
- `quest/D5 send_key` 空实现保持：083 全库无 Quest 键盘 handler 符号，门禁 5 禁想象修改。

## 3. 三桶闭合

- W 轨 38（WIRE，含 must_keep 27）+ L 轨 130（PURE76+MIXED14+OTHER40）= 168，未决 0。
- E 桶 5 项 s9 私有扩展为 L 轨内豁免标记，不参与算术。
- reopen 27 条逐条登记 P1；drop/D2、D12 已有 083 反编译锚点，下一批还原。

## 4. 自测与交付

- harness `tools/test_round89_wle_triage.py`；GATE `gen_opcode_matrix.py --check`；真重编 `tools\build_smoke.bat`（touch UIQuestLog.cpp 后 build_log/EXE/覆盖三证）。
- 精确 add 零署名提交；17 个外部在途 WIP 原样隔离；feedback 无 rating；goal 保持 active。

## 5. 限制

- 本轮 1 行源码 + 4 行注释，零 wire / 零协议 / 零 WZ / 零新封包。
- 架构级重写与真机观感项仍单点裁决 / 用户认领。
