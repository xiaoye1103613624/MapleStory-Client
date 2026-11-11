# round88 三方一致性确认（wave3）

触发：m09543（可见 PUA 输出、不记评分）+ m09544（BeiDou==Angel 质疑 / Angel 与服务端一致性 / 若一致全部按 Angel 对齐 / 原 BeiDou 多错）。

## 结论

- 保持原逻辑**不是**因为 BeiDou==Angel：Q1 同二进制=两个 IDA 实例，当前端 HeavenClient 是自研重实现；真实理由 DECISIONS.md §0 = s9 联机优先。
- Angel vs s9：握手/opcode 一致（hello version 83、GATE OK a78ba681a4），字段级 body 不一致（s9 改造 v83：exp 8B / level 2B / HP-MP int / GachaExp 4B / spawn tail）。
- 条件「若一致则全部按 Angel 对齐」在 body 层前提不成立 → 不触发；改回 083 原生包形断联机（LoginParser 白屏实证）。
- 「原 BeiDou 多错」成立：LoginParser 白屏实锤 + 30+ 按 Angel 083 翻案重修 commit。

## round86 纠偏

168 三分类 PURE76/WIRE38/MIXED14/OTHER40；79 KEEP = WIRE27/OTHER25/PURE20/MIXED7。
WIRE27 保持 s9 成立；PURE20+MIXED7=27 条不碰 wire 被一刀切 KEEP 的理由不成立 → 列 reopen 逐条拍板，本轮零源码改动。

## 对齐策略

wire s9 优先 / 字段 body 逐片以 s9 为准（PLAN.md §P2）/ 纯客户端层按 Angel 083 语义继续对齐。

## 限制

0 源码 / 0 协议 / 0 WZ 改动；11 个外部在途 WIP 原样留工作树；27 条 reopen 待逐条裁决，架构级重写不在本轮自动落地。

证据：`../evidence/three-way-consistency-round88.json`；主答复：`../../angel-calibration-d-matrix-2026-11/CONSISTENCY-VERDICT.md`。
