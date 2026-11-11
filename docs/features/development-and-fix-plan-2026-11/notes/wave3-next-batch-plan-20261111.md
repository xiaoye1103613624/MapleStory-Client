# round91 · 下一批一次性 Plan 取证笔记（wave3）

日期：2026-11-11　触发：m10739「开始 下一批 的计划」+ m10738「记录一次有可见 PUA 输出的交付，不记录评分。」

## 1. 做了什么

对 WLE-TRIAGE-PLAN.md 登记的「下一批 L 桶 3 条」做四方复核（083 活 IDA 12 份反编译 / s9 5 个 handler / 当前端 4 个文件 / DECISIONS 5 行裁定），证伪三条全部；按门禁 1 产出一次性 Plan（计划轮零源码改动）。

## 2. 取证清单

| 工具 | 命令 / 文件 | 结果 |
| --- | --- | --- |
| `tools/_r91_ida.py` | 083 活 IDB symbol query + 6 处 decompile | `tools/_ida_r91/*.code.txt` |
| `tools/_r91_ida2.py` | 10 组候选符号密度 | `*CUIChannel*` 6 / `*CUIMacro*` 17 / `*ShiftMacroUIState*` 1 |
| `tools/_r91_ida3.py` | 5 处新候选 decompile | 宏 / 频道网格反编译落盘 |
| `tools/_r91_img_gate.py` | orange-wz 128B `DEFAULT_KEY` + `UIWindow.img` 三根 | **GATE_OK_128B_IMG_REACHABLE** |
| `tools/_r91_deliver.py` | 幂等生成器 | evidence / Plan / WLE §8 / dev 2-39 / PLT §50 / notes |

## 3. 三条判定（一句话各一条）

- `quest/D8`：`sub_721008` 控的是 c2s 107 包尾部 4B x/y 的**条件追加**，是 wire 不是逻辑；s9 用 `available() >= 4` 同时容忍两种形态，当前端无后缀合法 ⇒ W 桶保持。
- `skill/D4`：问的是「要不要新增一整个独立子窗族」，架构级单点裁决；083 全库只 2 个符号，当前端 ESC 已闭环 ⇒ 不新增。
- `corewindow/D8`：K3 DOC_ONLY，round75 已发货；083 的 500ms 门闩 / 6 掩码 / 5 分支是参考值，s9 无限速且 Stat 编码不同，当前端实现**正是 s9 正确形态** ⇒ 只补逐字对照，不改代码。

## 4. 同族病（第三次）

round90 挖出 `drop/D2`+`D12` 误登记，round91 又挖出 §7 自己写的「无同族误分类」也是误判。根因=从矩阵推荐默认文本直接抄进 P1 登记层。硬规则已写进 `NEXT-BATCH-PLAN.md §3` 与 WLE §8：登记前必须附 083 反编译 + s9 handler + 当前端字面量 + DECISIONS 原裁定四项。

## 5. 限制与验收

- 0 源码 / 0 wire / 0 协议 / 0 WZ / 0 新封包；计划轮，用户确认 Plan 后才实现。
- 交付物不写 commit hash / `git log`（round88 B9b）。
- IMG 门禁：128B `WzAESConstant.DEFAULT_KEY` base64（172 字符）+ iv `TSPHKw==`；`.nx` 路径才用 65536B `key_gms`。
- 验收：harness 静态守卫 + 场景全绿、`python tools/gen_opcode_matrix.py --check` GATE OK、build 门禁回归、零署名精确 add 提交、feedback 无 rating、goal 保持 active。
