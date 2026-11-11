# round97 · P3 A 批余 6 条（六条全 DOC_ONLY）

## 1. 本轮做了什么

承接 round96（A 批前 3 条：`login-flow/D12`、`skill/D11`、`corewindow/D10`），
本轮把 A 批**余 6 条**逐条走完 083 活 IDA 取证：`map/D1`、`map/D8`、`cashshop/D2`、
`map/D4`、`statusbar-hud/D1`、`social-economy/D3`。

方法固定为三向对拍：**083 反编译（活 IDB 13339）→ s9 服务端源码（唯一副本）→ 当前端字面量**，
再叠 `DECISIONS.md` 原裁定。任一项缺证即只登记不实施（P3 Plan §0 硬规则 1）。

## 2. 关键证据（决定判定的那几行）

- `map/D1`：`SaveFuncKeyMap @0x58df2f` 的 `while ( v2 < 89 )` + `v3 += 5` + `v3[445]` 三段
  把「89 槽 × 5B + 445B 影子」钉死；`sub_4E43FA` 全文只有一行 `EncodeBuffer(5u)`。
  s9 `KeymapChangeHandler` mode0 的 `readInt/readByte/readInt` 合起来同样是 5B —— 总长一致，
  字段切分不同但语义等价，故不动协议。
- `map/D8`：083 `EncodeBuffer(this + 4, 0x20u)` 与 s9 `QUICKSLOT_SIZE * Integer.BYTES`
  是同一个 32；s9 的 `(byte) readInt` 截断只在键值 ≥ 0x100 才有损，而 v83 触发键域不含此值。
- `cashshop/D2`：`*CCSWnd*` 用正则按类聚合后得到 **7 类**，方法数 `5+18+9+18+8+4+7 = 69`
  再加 `ZArray` 模板实例 1 条 = 70 符号 —— 与 `DECISIONS.md:170`「66 方法面」不是同一计数口径
  （66 指 `CCashShop` 本体的早期估计，实测 67），本轮以实测为准。
- `statusbar-hud/D1`：45 方法按簇求和 `8+7+1+8+14+7 = 45` 现场减算闭合，无孤儿方法。
- `social-economy/D3`：`*CUIAlliance*` **0 命中**是本轮最硬的一条 —— 083 确实没有独立联盟窗，
  但入口按钮 `CreateAllianceButton @CUIUserList 0x91eba3` 说明它挂在共用窗上，与本端
  round85 的收敛方向一致。

## 3. 为什么不改代码

六条全部落在两类：**(a) 本端已等价**（`map/D1` 差分、`map/D8` 32B）、
**(b) 架构差异且改动等于重写**（`cashshop/D2` 七子窗、`map/D4` 侧栏、`statusbar-hud/D1` 单体重构、
`social-economy/D3` 双入口）。P3 Plan §0 硬规则 2 要求「一片一原子 commit、W 桶零改动」，
把 (b) 类硬改成 083 形态会同时破坏 s9 联机与已对齐排版 —— 这正是 round90/91/92 三轮
同族病的反面。故本轮只交证据 + 映射表，零源码行为改动。

## 4. 与历史结论的关系

`DECISIONS.md` 这 6 条的裁定原文（`BATCH_0_BASELINE` / `BATCH_1_KEEP`）**不改**：
本轮不是翻案，而是把 round94 里「按当时证据判定保持」升级成「按 083 逐条取证后判定保持」，
证据强度不同、结论相同。历史证据链保留，新口径写在 WLE §14 与 P3 Plan §7。

## 5. 验证

- harness：静态守卫覆盖六条 083 锚点、s9 关键行、当前端字面量、四文档落位、EOL、WIP 隔离；
  场景对算术做真值表（`89*5=445`、`8*4=32`、`7` 子窗、`8+7+1+8+14+7=45`）。
- GATE：`python tools/gen_opcode_matrix.py --check`。
- 构建：**零源码改动，不声称真重编覆盖**。门禁回归如实记录：协议门禁 `GATE OK`，
  但 MSBuild `EXITCODE=1` —— 被工作树里**他人外部在途 WIP** 阻断（`UIChatBar.cpp(2581/2595) C2664`、
  `UIGender.cpp(189) C1070`、`UIMedal.cpp(38/67/161)`、`UISpecial.cpp(22/96)`、
  `UITemporaryStatView.cpp(193)`，9 error / 3 fatal）。这些文件本轮从未触碰、
  `git status` 全为外部 `M`/`??`、且不含本轮任何标记串；round96（19:22）真重编时尚未坏。
  外部 WIP 修好后须重跑门禁回归确认基线回绿 —— 本轮不把外部坏点记成自己的失败，也不假装绿。
- 提交：零署名，精确 `git add`，不夹带工作树里的他人 WIP。
