# Wave 3 · 跨面 168 条批量裁决销案（2026-11-11 · round86）

## 1. 背景与触发

上轮 `4b2452c4`（round85 D-G 收口）交付后，七维对账答「校准层闭环、修复层未全绿」，挂三类待拍板：协议级（`InPacket::available()` 393 处 / L-B 12 项）、版式观感（真机五屏）、批量裁决（D 矩阵 K3+K4 64 条 / 残差 28）。用户 m09211「全部继续，如果能自测的可以自己启动测试」= 按推荐默认（s9 优先）一次性裁掉；m09210 要求记录一次可见 PUA 输出的交付、不记录评分。

## 2. 证据链（现场逐字，非记忆）

- `docs/features/angel-calibration-d-matrix-2026-11/README.md` 477 行全读：§2.1 K1 10 / K2 94 / K3 39 / K4 25；§3 十二面 14 条表；§4 12 组主题 + §4.1 明细；§5 五条批量裁决（覆盖 140 / 残差 28）；§5.1 残差清单；§6 P0–P3 一次性 Plan；§7 限制；§9 性质。
- `docs/features/development-and-fix-plan-2026-11/README.md` §3 / §3.1 / §5 / §6 / §7 / §11.1 / §11.2 / §11.3 全读；§11.3 自述「需用户追认」。
- `docs/features/pending-live-tests/README.md` 60 节 / 142 处真机标记；§43（round85）末尾为追加点。
- `evidence/d-matrix.tsv` 168 行但 UTF-8 读为乱码（历史 GBK 混合），**不拿来当判据**；改由脚本从矩阵 README §3 的 12 张表逐字解析，断言 168 行 / K 分布 / 每面 14 条全绿。
- T9 P0 现场核验：`2-17`（round52）已销 `drop/D4`、`drop/D7`、`statusbar-hud/D5`、`corewindow/D5`；`2-18` `6ca463e1` 已接 `change_target`；`2-19` `d6029c63` 已开中文 IME；`2-20` `fef8a56d` 已补 i18n。→ 不重复返工。

## 3. 裁决（核心结论）

1. **第 0 号 T1**：s9 联机优先 / Angel 083 仅语义校准源 / 校准≠改回原生 / 禁引 095。
2. **第 1–4 号**：94 条 K2 保持现状销案；T5∪T6 39 条采纳但**前提翻案**（§4）；T3 7 条回写授权=裁决态销案不重写历史证据；T11∪T4∪T10 三条口径认可。
3. **T5 翻案**：round85 三实验坐实 orange-wz 无故障，真因 `WzMutableKey.java:84-90` 128B→32B AES 派生 vs `.nx` 65536B 全量 XOR 表规格错配；128B `WzAESConstant.DEFAULT_KEY` 实测 `isError:false`。旧「MCP 阻塞」不得再当今天的前提。
4. **L-B / InPacket 追认**：12 项 + `Net/InPacket.h:37` 393 处维持现状，零协议改动；§11.3 口径差异结案。
5. **残差 28 条**：逐条裁定（见 `DECISIONS.md` §5），其中 4 条已发货引用既有 commit，2 条 `095` 注释是本端扩展身份标注（按 round72 教训保留并补口径注记，不机械换地址造二次谎言），`quest/D14`、`drop/D14` 裁 NO_OP 并入本轮统一提交。
6. **算术闭合**：104 + 36 + 28 = 168，未决 0。

## 4. 自测（本轮实际跑过的命令）

```text
python tools/test_round86_batch_adjudication.py   → STATIC GUARDS / SCENARIOS PASS
python tools/gen_opcode_matrix.py --check          → GATE OK (s9 a78ba681a4)
MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c 'tools\build_smoke.bat'  → GATE OK + EXITCODE=0
```

本轮**零源码 / 零协议 / 零 WZ 改动**：构建输出为门禁回归，不声称真重编覆盖；上一轮 `4b2452c4` 已用 touch 三 cpp + build_log 19174B + EXE mtime 三证完成真重编留证。工作树里的 11 个源码 WIP（`Character/CharStats.{cpp,h}` / `Character/Player.cpp` / `Gameplay/Stage.cpp` / `IO/UITypes/UIStatusBar.{cpp,h}` / `Net/Handlers/Helpers/CashShopParser.cpp` / `LoginParser.cpp` / `Net/Handlers/PlayerHandlers.cpp` / `WaveAShellHandlers.cpp` / `Net/Login.h`）全部是外部程序在途改动，现场 git diff 可见，**本轮一个字节未碰**；用 hunk 级 `git apply --cached` 隔离，原样留工作树不夹带。

## 5. 未决（交用户认领，不假装完成）

- 真机五屏截图：PLT 60 节 / 142 处标记，本轮零真机执行，归 §44 五屏归口清单。
- P2：F 维 12 项 × 12 面逐像素、`cashshop/D9` 24 个 `sub_47xxxx` 补符号、`activity-minigame/D2` crosscheck 阈值。
- P3 明确不做：booby trap 全链 / 拖拽丢弃族 / 独立 `CUIPartyHP` 悬浮窗 / 小游戏 DIY 壳 / Quest 107 抓包工具。

- 提交：`#devfix-batch-adjudication` 零署名原子提交；`goal-11388c42` 保持 active，全目标未达成不标 complete。
