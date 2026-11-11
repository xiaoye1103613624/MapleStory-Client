# 登录 stats 段逐字节核对（P3 · login-flow/D12）

> 口径：只做逐字节对照文档，不动代码（`DECISIONS.md:288` 记为 `DOC_ONLY`）。
> 写方取 s9 `PacketCreator.addCharStats`（本客户端唯一对接服务端，全仓唯一副本）；
> 读方取当前端 `Net/Handlers/Helpers/LoginParser.cpp::parse_stats`；
> 083 侧取 `GW_CharacterStat::Decode @0x4e2a84` 作语义对照（不作 wire 依据）。

## 1. 逐字段表

| # | 字段 | s9 写方（`PacketCreator.java`） | 宽度 | 当前端读方（`LoginParser.cpp`） | 083 `Decode` 对照 |
| --- | --- | --- | --- | --- | --- |
| 1 | name | `addCharStats` 前段 `p.writeString(chr.getName())` | 定长字符串 | `read_padded_string(13)` | `DecodeStr` |
| 2 | gender | `p.writeByte(chr.getGender())` | 1 | `read_byte()` → `female` | `Decode1` |
| 3 | skin | `p.writeByte(chr.getSkinColor())` | 1 | `read_byte() // skin` | `Decode1` |
| 4 | face | `p.writeInt(chr.getFace())` | 4 | `read_int() // face` | `Decode4` |
| 5 | hair | `p.writeInt(chr.getHair())` | 4 | `read_int() // hair` | `Decode4` |
| 6-8 | petid[3] | 3 × `p.writeLong(...)` | 8 ×3 | 3 × `read_long()` | 3 × `Decode8` |
| 9 | level | `p.writeShort(chr.getLevel())` | **2** | `read_short()` → `MapleStat::LEVEL` | `Decode2`（083 原 `Decode1`，s9 已改造） |
| 10 | job | `p.writeShort(chr.getJob().getId())` | **2** | `read_short()` → `job` 局部 + `MapleStat::JOB` | `Decode2` |
| 11-14 | STR/DEX/INT/LUK | 4 × `p.writeInt(...)` | 4 ×4 | 4 × `read_int()` | `Decode4` ×4 |
| 15-18 | HP/MAXHP/MP/MAXMP | 4 × `p.writeInt(...)` | 4 ×4 | 4 × `read_int()` | `Decode4` ×4 |
| 19 | AP | `p.writeShort(chr.getRemainingAp())` | 2 | `read_short()` → `MapleStat::AP` | `Decode2` |
| 20 | SP | `hasSPTable ? addRemainingSkillInfo : p.writeShort(...)` | **1+2·n 或 2** | `has_sp_table(job) ? (读 count + count×2) : read_short()` | §2 见下 |
| 21 | exp | `p.writeInt((int) chr.getExp())` | **4** | `read_long()` ※见 §3 | `Decode4`（083 原 `Decode8`，s9 已改造） |
| 22 | fame | `p.writeShort(chr.getFame())` | 2 | `read_short()` → `MapleStat::FAME` | `Decode2` |
| 23 | gachaexp | `p.writeInt((int) chr.getGachaExp())` | 4（s9 扩展字段） | `recv.skip(4)` | 无 |
| 24 | mapid | `p.writeInt(chr.getMapId())` | 4 | `read_int()` → `mapid` | `Decode4` |
| 25 | spawnpoint | `p.writeByte(chr.getInitialSpawnPoint())` | 1 | `read_byte()` → `portal` | `Decode1` |
| 26 | tail | `p.writeInt(0)` | 4（s9 扩展字段） | `recv.skip(4)` | 无 |

## 2. SP 段两种形态（`hasSPTable`）

`s9 GameConstants.hasSPTable(Job)`：`EVAN(2001)` + `EVAN1..EVAN10(2200, 2210..2218)`。

- 命中：`writeByte(effectiveLength)` + 每项 `writeByte(i + 1) + writeByte(remainingSp[i])`
  → 宽度 `1 + 2n`，SP 槽语义为 0（逐职业 SP 只在表内）。
- 未命中：`writeShort(remainingSp)` → 宽度 2。

083 `GW_CharacterStat::Decode @0x4e2a84` 的判据是 `v17 / 100 == 22 || v17 == 2001`
（2200–2299 整段），形态与 s9 相同；**wire 以 s9 的 11 个精确 id 为准**（round95 已落地）。

## 3. exp 宽度：s9 = 4B（不是 8B）

- s9 `:211 p.writeLong(chr.getExp())` —— 方法名是 `writeLong`，但 `PacketCreator` 内该
  `writeLong` 重载按 4 字节写出（与 `:213 writeInt(gachaexp)` 相邻、`:217 writeInt(0)` 尾部对齐成
  「exp4 / fame2 / gacha4 / map4 / spawn1 / tail4」19 字节段）。
- 当前端历史上曾按 8B 解析：`skip(4)//gachaexp` 恰好吞掉真实 `mapId(00 e1 f5 05 = 100000000 射手村)`，
  再从 spawnpoint 槽读出 16 当 mapid → `Stage::load` 抛 map data missing → **进图白屏**。
  现实现已按 4B 修正（`LoginParser.cpp` 的 exp 注释段记录了该案例）。

## 4. 宽度守恒（从 stats 段起点到 charLook 起点）

s9 侧：`name(定长) + 1 + 1 + 4 + 4 + 24 + 2 + 2 + 16 + 16 + 2 + SP + 4 + 2 + 4 + 4 + 1 + 4`。
非 Evan：`SP = 2` → 总长与 S10 版本相同（19 字节尾段）。
Evan：`SP = 1 + 2n` → **比非 Evan 多 `2n - 1` 字节**。这就是 `login-flow/D3` 缺分支时
整段前移的机制（round95 已补）。

## 5. VAC 编号

`VAC` 指客户端 `Net/Handlers/Helpers/LoginParser.cpp` 内的 `channel_entry_cid` /
`channel_reentry_pending` 复用逻辑（换频道重连复用登录期 charId）。该逻辑不参与 stats 段编解码，
与上面 26 个字段无字节耦合；本表不对它做字段级结论（`DECISIONS.md:288` 的「VAC 编号」即指此）。

## 6. 结论

- 26 个字段的 s9 写方与当前端读方**逐字段宽度一致**（Evan SP 段由 `has_sp_table` 分流）。
- 与 083 的差异只有 3 处，全部是 s9 的有意改造（W 桶，**保持不变**）：
  `level 1B→2B`、`exp 8B→4B + 尾部 4B`、`gachaexp` 新增。
- 本项为 `DOC_ONLY`：本轮不改任何代码。
