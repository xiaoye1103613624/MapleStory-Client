# 等级上限 256 / LEVEL 无符号解析（2026-11-06）

> 遗留链子卡 5：接续 Cursor 会话 `0e415681` 中断处，把三处未提交改动验证并原子提交。
> 用户（m01905）：「先别真机测试了，先直接下一步开发任务，这个记录，后面我测试截图」。

## 变更清单（3 个代码文件，Cursor 会话遗留、mtime 2026-11-06 11:04~11:05）

| 文件 | 改动 |
| --- | --- |
| `Character/ExpTable.h:26` | `constexpr size_t LEVELCAP = 201;` → `256` |
| `Character/ExpTable.cpp` | 在表尾 L201 `1697021059,` 后补 55 个值至 `41832114835`，共 256 项 |
| `Net/Handlers/Helpers/LoginParser.cpp:140` | `statsentry.stats[MapleStat::Id::LEVEL] = static_cast<uint8_t>(recv.read_byte());`，防 `0xFF` 符号扩展成 `-1` |

`ExpTable::values` 声明为 `std::array<int64_t, LEVELCAP>`；扩表后有 51 个值 ≥ `INT32_MAX`，必须用 `int64_t`。

## 验证结论（逐值见 `evidence/verify.txt`）

- `LEVELCAP = 256`、`values` 元素数 `= 256`、`MATCH = True`、first/last `= 1 / 41832114835`、全部 `< 2^63`。
- 非单调相邻 2 处：`[(10,1242,1144), (163,223471711,223471711)]`。`git show HEAD:Character/ExpTable.cpp` 证实 **HEAD 原表（201 项）同样有这两处**，非本次引入，保留不动。
- 消费点边界：`IO/UITypes/UIQuit.cpp:249`、`IO/UITypes/UIStatusBar.cpp:1839`、`IO/UITypes/UIStatsInfo.cpp:861` 三处全部与**同一个** `LEVELCAP` 常量比较后再索引 `values[level]`，扩表后 max index `255 < 256` 恒安全。
- 类型链：`Net/InPacket.h:46 int8_t read_byte()` → `static_cast<uint8_t>` → `Net/Login.h:53 EnumMap<MapleStat::Id, uint16_t> stats` → `Character/CharStats.h` 的 `uint16_t`，干净扩宽。
- 构建门禁：`tools/build_smoke.bat` 2026-11-06 17:41 复跑 → `GATE OK: opcode matrix matches both sources (s9 9dcd23e331)` + `EXITCODE=0`。（16:45 全量 /m 构建亦已编过这三个稳定文件。）

## ⚠️ 显式预留：201–256 是占位近似值，非原生对齐

- `L201..L256` 段 56 项严格递增，但相邻比率 **min/max = 1.060000 / 1.060000**，即按固定 `1.06` 等比外推的近似值，**不是原生官方经验数值**。
- 原生证据 `E:\MXD\BeiDou-Client_S9\level300_debug.txt`：`AttachLevel300Mod begin sentinel=LVL3 (0x4C564C33) UseVirtuProtect=1`、`FixMovzx_ExpTableLookup_Level @ 0x0078DAEB: patched 3 bytes`、`GetNextLevelExp_max @ 0x0078D16A: imm 200 -> 300`（另有 `ExpHelper_max_A/B/C`、`ExpPercent_max` 同改 200→300）、`ok=40 fail=0`。→ **原生 S9 客户端是 200 级封顶 + `LVL3` mod 运行时 patch 到 300 级，本身没有官方 201+ 经验表可抄**。
- 文档佐证：`docs/features/beidou-v17-parity/notes/01-parity-matrix-and-plan.md:38`「等级上限 | 插件 patch 到 300 | ExpTable::LEVELCAP = 200 | 300 级需扩表 | 经验表 + 升级 UI + 服务端」、`:64`「8 解锁 300 级 ❌（经验表 200 级封顶）」、`docs/features/parity-gap-2026-11/README.md:119`「LEVELCAP = 201；服务端侧也未开」。
- 服务端侧存在 `org.gms.constants.game.ExpTable`（见 `docs/features/damage-calc-cap-parity-2026-11/patches/s9.patch:11/:633`），本轮未读、未对齐。

**结论**：本次提交只解除「客户端数组/类型越界」这一硬阻塞，把 256 级容器与无符号等级解析落地；**201–256 数值是客户端占位表，须与服务端 `ExpTable` 重新对齐后才可宣称 parity**。后续闭环项（未纳入本提交）：

1. 读取服务端 `org.gms.constants.game.ExpTable` 的 201–255 真实值，替换 1.06 外推段；
2. 升级 UI / StatusBar / StatsInfo 在 200+ 的展示与溢出复验；
3. 与 `LVL3` mod 的 300 级上限策略对齐（当前客户端封顶 256，原生 mod 是 300）。

## 提交范围

仅 `Character/ExpTable.cpp`、`Character/ExpTable.h`、`Net/Handlers/Helpers/LoginParser.cpp` + `docs/features/level-cap-256-2026-11/`。工作区其它 `M`/`??` 属并行会话在制品，不进本功能提交。
