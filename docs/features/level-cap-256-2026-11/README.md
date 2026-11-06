# 等级上限 256 / LEVEL 无符号解析（2026-11-06）

> 遗留链子卡 5：接续 Cursor 会话 `0e415681` 中断处，把三处未提交改动验证并原子提交。
> 用户（m01905）：「先别真机测试了，先直接下一步开发任务，这个记录，后面我测试截图」。
> 用户（m02072）：「所有任务没完成的话 就继续开发」→ 本文件第二刀：把 `dbcefb0f` 的想象值对齐到服务端真值。

## 变更清单

| 提交 | 文件 | 改动 |
| --- | --- | --- |
| `dbcefb0f`（子卡5） | `Character/ExpTable.h:26` | `constexpr size_t LEVELCAP = 201;` → `256` |
| `dbcefb0f`（子卡5） | `Net/Handlers/Helpers/LoginParser.cpp:140` | `statsentry.stats[MapleStat::Id::LEVEL] = static_cast<uint8_t>(recv.read_byte());`，防 `0xFF` 符号扩展成 `-1` |
| `dbcefb0f`（子卡5） | `Character/ExpTable.cpp` | 表尾 L201 `1697021059,` 后补 55 个值（当时为 `1.06` 等比外推占位） |
| 本轮（数值对齐） | `Character/ExpTable.cpp:45-53` | 把上述 55 个 `1.06` 外推值全部替换为服务端 flat `2000000000`，共 256 项 |

`ExpTable::values` 保持 `std::array<int64_t, LEVELCAP>`；对齐后所有值 `< INT32_MAX`，类型不变。

## 验证结论（逐值见 `evidence/verify.txt`）

- `LEVELCAP = 256`、`values` 元素数 `= 256`、`MATCH = True`、`values[0]=1`（dummy）、`values[200]=1697021059`、`values[201]=values[255]=2000000000`、全部 `< 2^63`。
- `values[201..255]`（55 项）全部 `== 2000000000`，与三份服务端 `getExpNeededForLevel(level>200)` 契约一致。
- 消费点边界：`IO/UITypes/UIQuit.cpp:249`、`IO/UITypes/UIStatusBar.cpp:1839`、`IO/UITypes/UIStatsInfo.cpp:861` 三处全部与**同一个** `LEVELCAP` 常量比较后再索引 `values[level]`，扩表后 max index `255 < 256` 恒安全。
- 类型链：`Net/InPacket.h:46 int8_t read_byte()` → `static_cast<uint8_t>` → `Net/Login.h:53 EnumMap<MapleStat::Id, uint16_t> stats` → `Character/CharStats.h` 的 `uint16_t`，干净扩宽。
- 构建门禁：`tools/build_smoke.bat` 每次变更后复跑 → `GATE OK: opcode matrix matches both sources (s9 9dcd23e331)` + `EXITCODE=0`。

## 服务端契约对齐（决定性证据，第二刀）

`dbcefb0f` 的 `L201..L256` 段是固定 `1.06` 等比外推的**想象值**（到 `41832114835`，约为服务端契约的 20 倍），违反「禁想象修改」门禁。用户授权继续开发后读到了服务端真值：

- `E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/constants/game/ExpTable.java:31`
  `public static int getExpNeededForLevel(int level) { return level > 200 ? 2000000000 : exp[level]; }`
  `exp[]` 共 201 项（index `0..200`，`exp[200]=1697021059`），**没有 201–255 递进真值**。
- 三份服务端 `BeiDou-Server_s9` / `BeiDou-Server_S10` / `BeiDou-Server` 的 `exp[]` 逐值相同、flat 契约相同；`S10`/`base` 另声明 `MAX_LEVEL = 200`。全服务端树 grep `41832114835|level300|300级` = 0 命中。
- s9 `Character.java:5054` `getMaxClassLevel() = 255`（非 Cygnus）、`:3074` `while (exp >= ExpTable.getExpNeededForLevel(level)) levelUp(true)`、`:3088` 满级清零 → **s9 有效上限 255 级，正好对应客户端 `LEVELCAP=256`（有效 level 1..255）**。
- 逐值对账：`client values[1..200]` 与 `server exp[1..200]` 仅 index `9/10` 互换（client `1242/1144` vs server `1144/1242`），这是 HEAD v83 表既有的 quirk（`git show HEAD:Character/ExpTable.cpp` 证实），非本次引入，保留不动；`values[200] == exp[200] == 1697021059` 边界吻合。

**结论**：`dbcefb0f` 解除了「客户端数组/类型越界」硬阻塞；本轮把 200+ 经验条分母对齐到 s9 服务端 flat 契约，删除全部想象值。200+ 数值不再是占位表。

## 仍显式预留 / 待裁决（不冒充已完成）

1. **index 9/10 反序**：HEAD v83 表既有 quirk 与服务端不一致，本轮按「不改既有数据」原则保留，是否向服务端对齐待用户裁决。
2. **200+ UI 展示复验**：StatusBar / StatsInfo / UIQuit 的 200+ 经验条显示与溢出需真机验收，按 m01905 挂起、由用户后续截图判定。
3. **上限策略差异**：s9 服务端 cap `255`、客户端 `LEVELCAP=256`、原生 `LVL3` mod 运行时 patch 到 `300`（`E:\MXD\BeiDou-Client_S9\level300_debug.txt`：`GetNextLevelExp_max @ 0x0078D16A: imm 200 -> 300`、`ok=40 fail=0`）。三者策略不同，需要用户裁决目标上限后再统一，不凭想象改常量。

## 提交范围

- `dbcefb0f`：`Character/ExpTable.cpp`、`Character/ExpTable.h`、`Net/Handlers/Helpers/LoginParser.cpp` + `docs/features/level-cap-256-2026-11/`。
- 本轮：`Character/ExpTable.cpp` + `docs/features/level-cap-256-2026-11/README.md`、`evidence/verify.txt`。
- 工作区其它 `M`/`??` 属并行会话在制品，不进本功能提交。
