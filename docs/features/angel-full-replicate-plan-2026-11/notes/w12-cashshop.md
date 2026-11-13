# W12 cashshop — 执行记录

## 对照

- 已有：[cashshop-angel-calibration-2026-11](../../cashshop-angel-calibration-2026-11/README.md)

## 本波

- 无新增差异（校准已闭环）

## MTS — 为何不能做

- s9 **有** `RecvOpcode.MTS_OPERATION(0xFD)` + `MTSHandler`（完整上下架/购物车逻辑），以及 s2c `MTS_OPERATION`/`MTS_OPERATION2`
- 客户端已有 s2c handler 日志壳（`WaveAShellHandlers`）与 `ENTER_MTS` 枚举
- **缺**：完整 MTS UI 窗 + 按 `op` 分支的 c2s 组包（handler 内 case 2/… 字段极多）
- 最小「只加枚举」无入口可点，不能销案；整窗复刻超出本阻塞清扫范围且易破现有商城状态机

**结论**：不能做（缺 UI + 复杂 c2s 面；非单纯 opcode 缺口）。

## 其它

- stub_job/stub_name 文案属 R2 locale，非本轮阻塞强制项
