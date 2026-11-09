# Wave2 2-9 · UIItemInventory 重复 G 键入口遮蔽复核（2026-11-10 round41）

## 0. 结论

- 现场逐字核准发现第 8 例同族缺陷：`IO/UITypes/UIItemInventory.cpp` 的同一 `else if` 链里存在两处 `keycode == 'G' || keycode == 'g'`，第一处归 `GatherItemsPacket(tab)`，第二处归 `UseGachaExpPacket()`；链一命中即跳，第二分支零可达，`USE_GACHA_EXP(158)` 空包等于「补了包、入口被遮蔽」。
- 历史归属用 git 证据钉死：`git blame -L 1182,1186` 与 `git blame -L 1229,1233` 同为 `df31fb1c (xiaoye 2026-11-01 15:09:09 +0800)`；父版本 `git show df31fb1c^:IO/UITypes/UIItemInventory.cpp` 只有 `GatherItemsPacket(tab).dispatch();`，说明 G/g 归整理是既有主路径。
- 最小修复：第二处 G 分支改为链内空位 `X/x`，并写 5 行依据注释；G/g 继续归整理，X/x 成为 `UseGachaExpPacket().dispatch()` 的唯一可达入口。协议/包体/WZ/IMG/XML 零改动。
- 自测实证：`tools/test_iteminventory_gkey.py` = `PASS: 10 static guards + 10 keycode cases (before Gacha reach=0 -> after 2; duplicate G 2->1; regression 8/8)`；`python tools/gen_opcode_matrix.py --check` = `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)`；`tools/build_smoke.bat` = `GATE OK` + `EXITCODE=0`，`build_log.txt` 从 `2026-11-10 01:32:45 / 278340B` 真重编译到 `2026-11-10 01:50:44 / 9426B`，`ERROR_COUNT 0`。
- 真机需联测 G/g 整理、X/x 触发 GACHA_EXP、旧键回归与 X 键语义裁决，已写进 `docs/features/pending-live-tests/README.md` §20。

## 1. 证据优先级与 orange-wz 口径

| 层级 | 本轮证据 | 结论 |
| --- | --- | --- |
| 协议矩阵 | `docs/features/s9-protocol-align/protocol/opcode-matrix.json` + `python tools/gen_opcode_matrix.py --check` | `USE_GACHA_EXP = 158` 合法，门禁 GATE OK |
| 源码构造点 | `Net/Packets/SolomonPackets.h:22-26` `UseGachaExpPacket() : OutPacket(OutPacket::Opcode::USE_GACHA_EXP)`；`Net/OutPacket.h:294-295` `USE_GACHA_EXP = 158` | 包类已存在、空包体正确 |
| 现场运行链 | `IO/UITypes/UIItemInventory.cpp:1182` 与 `:1229`（修复前）双 G 分支 | 第二个分支被第一个遮蔽 |
| git 历史 | `df31fb1c` 同时新增两处 G；父版本已有 Gather | G/g 归 Gather 是主路径，GachaExp 是后加死分支 |
| 真机记录 | 旧账 `docs/features/beidou-v17-parity/notes/r803-aran-solomon-gacha-teleport-rgacha.md:9` 写「SoftKey G → UseGachaExp」 | 与 live G/g 主路径冲突，按证据优先级降级为历史意图 |
| WZ/IMG/XML | 本轮纯 C++ 交互链，未解析任何 WZ/IMG/XML 节点 | 沿用 round40 `evidence/orange-wz-resource-structure.json` / `nx-resource-content-probe.json` 已闭合门禁；AGENTS.md ACTIVE 执行顺序保持有效 |

## 2. live 逐字对照

| 位置 | 修复前 | 修复后 / 处置 |
| --- | --- | --- |
| `IO/UITypes/UIItemInventory.cpp:534-538` | `case Buttons::BT_GATHER: case Buttons::BT_GATHER_SM: GatherItemsPacket(tab).dispatch();` | 保留，证明 G/g 与整理按钮同源 |
| `IO/UITypes/UIItemInventory.cpp:1182-1186` | `else if (keycode == 'G' || keycode == 'g') { GatherItemsPacket(tab).dispatch(); LOG(LOG_DEBUG, "UIItemInventory: Gather tab=" << static_cast<int>(tab)); }` | 保留；G/g 继续归整理 |
| `IO/UITypes/UIItemInventory.cpp:1229-1233`（修复前） | `else if (keycode == 'G' || keycode == 'g') { UseGachaExpPacket().dispatch(); LOG(LOG_DEBUG, "UIItemInventory: UseGachaExp"); }` | 同一链后段，零可达 |
| `IO/UITypes/UIItemInventory.cpp:1234-1238`（修复后） | `else if (keycode == 'X' || keycode == 'x')` + `UseGachaExpPacket().dispatch();` | X/x 成为唯一可达入口；注释说明「X 是本文件 send_key 链内空位」 |
| `Net/Packets/SolomonPackets.h:22-26` | `UseGachaExpPacket` 空包体 | 复用既有包类，包体零改动 |
| 全仓 dispatcher | `UseGachaExpPacket().dispatch();` 唯一构造点位于该死分支 | 修复后从「构造但不可达」变成「构造且可达」 |

X/x 选择依据：全仓单字母绑定里 X/x 有多处，但 `IO/UITypes/UIItemInventory.cpp` 链内 `grep -cE "keycode == 'X'|keycode == 'x'"` = 0；同文件 A/B/H/J/K/L/N/P/Q/R/T/V/W/Z 也为 0。选 X 作为本文件链内空位，避免抢占 G/g 的整理语义，也避免触碰原生键位映射表。

## 3. 最小修复

- 文件：`IO/UITypes/UIItemInventory.cpp`
- diff：`1 file changed, 6 insertions(+), 1 deletion(-)`；`git diff --check HEAD -- IO/UITypes/UIItemInventory.cpp` 为空。
- 内容：第二处条件 `keycode == 'G' || keycode == 'g'` 替换为 `keycode == 'X' || keycode == 'x'`，并在上方补注释：
  - `// r803 旧账写 "SoftKey G -> UseGachaExp"，但本文件存在早先 G/g -> Gather；`
  - `// 同一 else-if 链一命中即跳，后段 G 分支零可达。X/x 是本文件 send_key 链内空位，`
  - `// 故把 USE_GACHA_EXP(158) 入口迁到 X/x；G/g 继续归整理，协议/包体零改动。`
- 修复后关键行：
  - `IO/UITypes/UIItemInventory.cpp:1182` `else if (keycode == 'G' || keycode == 'g')`
  - `IO/UITypes/UIItemInventory.cpp:1184` `GatherItemsPacket(tab).dispatch();`
  - `IO/UITypes/UIItemInventory.cpp:1234` `else if (keycode == 'X' || keycode == 'x')`
  - `IO/UITypes/UIItemInventory.cpp:1236` `UseGachaExpPacket().dispatch();`

## 4. 自测与门禁

- `python -m py_compile tools/test_iteminventory_gkey.py` → `PYCOMPILE_OK`
- `PYTHONIOENCODING=utf-8 python tools/test_iteminventory_gkey.py` → `STATIC GUARDS OK (10 needles)`；10 键位 case 全 `[OK]`；`[reach] before Gacha-dispatched keys=0 -> after=2`；`[duplicate] duplicate G cond: before=2 -> after=1`；`PASS: 10 static guards + 10 keycode cases (before Gacha reach=0 -> after 2; duplicate G 2->1; regression 8/8)`；`TEST_RC=0`
- `python tools/gen_opcode_matrix.py --check` → `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)`
- `tools/build_smoke.bat` → `GATE OK` + `EXITCODE=0`；`build_log.txt` `01:32:45 / 278340B` → `01:50:44 / 9426B`；`ERROR_COUNT 0`；tail `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe`；既有 warning 仅 `LNK4098 LIBCMT` / `LNK4099 vc120.pdb` / `/LTCG`。
- 占用处理：`taskkill /F /IM MapleStory.exe /T` 对 34476/6108/56016 三个 32K 空壳报 `Access is denied`，这些进程无资源路径、不阻断构建；用户 m04292 授权策略已执行。

## 5. 本轮范围边界

- 本轮只解决「同一 else-if 链双 G 遮蔽」这一入口链缺陷。
- `USE_GACHA_EXP(158)` 的服务端效果、GachaExp 道具态、背包资源展示与 X 键最终用户体感，归 `pending-live-tests` §20 联测。
- 旧账 r803 的键位意图保留为历史记录；是否把 X 再迁到 UI 菜单按钮或其它全局键位，交用户裁决后另立小改。
- Wave2 2-1 / 2-2 / 2-4、Wave3 L-B 决策闸门、⑫掉落(Drop) 校准续作仍 open。

## 6. 真机联测 backlog

见 `docs/features/pending-live-tests/README.md` §20：20-1 G/g 整理回归、20-2 X/x 触发 GACHA_EXP、20-3 旧键 O/U/D/S/Y/E 回归、20-4 X 键语义裁决。

## 7. 提交 scope

精确 `git add` 5 文件：

- `IO/UITypes/UIItemInventory.cpp`
- `docs/features/development-and-fix-plan-2026-11/README.md`
- `docs/features/development-and-fix-plan-2026-11/notes/wave2-iteminventory-recheck-20261110.md`
- `docs/features/pending-live-tests/README.md`
- `tools/test_iteminventory_gkey.py`

提交标题：`🐛 fix(ui): 道具栏重复 G 键入口遮蔽修复（GachaExp 改 X 键）(#devfix-iteminventory-gkey)`，零署名。

## 8. 性质标注

本轮为用户 m04292 授权下的当前客户端开发/修复任务：源码一处最小改动 + 自测 harness + 计划/销案/backlog 文档归档；十一面 Angel 083/v83 校准文档保持只读，校准结论只作为证据锚点。全程 083/v83 only 禁 095、校准≠改回原生、s9 优先、数值重合≠语义重合。
