# available() >= N 系统性 bool 误用 — 三方全面排查报告

> 状态：排查完成，四方证据链闭合，未改码。改码需用户确认。
> 味道：🔴 华为（RCA 根因分析 + 蓝军自攻击）。日期：2026-11-17。

## 一、执行摘要

| 项 | 结论 |
| --- | --- |
| 根因 | `bool available()` 返回 0/1，`available()>=N`(N≥2) 恒假；修复应改调用点为 `length()>=N` |
| 规模 | 客户端 Net/Handlers 下 **329 处**（12 文件），其中 **279 处落在已注册活跃 handler** |
| 已迁移 | CashShopHandlers 76 处已全部改 length()；ItemParser 2 处已修；属"未完成的人工迁移" |
| 跨端坐实 | MinMS 同样 bool available()，但 0 处 available>=N、51 处 length>=N → 修复方向正确 |
| Angel | 无相关原生证据，且 Angel(v83 原生) 与 emulator 是两套代码库，不混用证据 |
| 高危实锤 | GiveForeignBuff 所有 buff 失效、DamagePlayer damage 错位、quest skip5 漏跳、商城购物车空 |

## 二、根因（定义层铁证）

- `Net/InPacket.h:37` `bool available() const;`
- `Net/InPacket.cpp:30-33` `bool InPacket::available() const { return length() > 0; }`
- `Net/InPacket.h:39` / `InPacket.cpp:35-38` `size_t length() const { return top - pos; }`
- C++ 整数提升：bool→int 为 0/1，`1 >= N`（N≥2）恒假。

| 写法 | 求值 | 结论 |
| --- | --- | --- |
| `available() >= N`，N≥2 | 1>=N / 0>=N 均假 | 🔴 恒假，bug |
| `available()==1` / `>0` / 单独 / `>=1` | 等价 available() | 🟢 无害 |
| 三元 `available()>=N ? read_x() : default` | 恒取 default | 🔴 字段恒被吞成默认值 |

## 三、三方证据汇总

### 3.1 客户端（本地，已闭环）
- 精确清单 329 处 / 12 文件，CSV：`evidence/client-sites.csv`（列 file,line,N,function,form,source）。
- 分布：WaveAShellHandlers 200、MapObjectHandlers 88（占 87.5%）、PlayerInteraction 13、Messaging 8、Trade 8、Alliance 3、Inventory 3、Quest 2、Common/Fredrick/Player/Testing 各 1。
- 活跃度：279/329 落在 `Net/PacketSwitch.cpp` 已 emplace 注册的 handler（注册总数 304）。
- 安全机制：全部为 if 门控或三元，尾部普遍 `while(recv.available()) skip_byte()`；改 length()>=N 后 length()<N 仍走 default/跳过，**不越界**。
- 详见 `notes/client-side-findings.md`。

### 3.2 MinMS 收包侧（跨端对照）
- 两端 InPacket 定义一致（见 §二）；MinMS 全面用 length()>=N（51 处），本端 available>=N 是偏离范式的 bug。
- 本端迁移状态：CashShop 76 处已全改、MapObject 已有 7 处 length() 但仍有 88 处 available>=N → 未完成迁移。
- 字段对照三类：一致（SpawnMist/DamagePlayer/SkillEffect/quest skip5）、版本差（DamageMonster 多 skin int、SpawnChar level short vs byte、GiveForeignBuff mask 语义）、无法对应（WaveAShell 200 处、Alliance、Chalkboard 等 MinMS 无此 handler）。
- MinMS 19 处组合门控写法值得借鉴（值条件 && length()>=N）。
- 详见 `evidence/minms-crosscheck.md`。

### 3.3 Angel 侧（取证边界）
- 无相关原生证据：18 个 drop 函数反编译中 CInPacket 仅作参数、仅 4 个解码方法、无剩余字节判断。
- 关键认知：Angel v83 原生 C++ 与 emulator `available()` 是两套代码库，证据不可直接混用。
- 补强条件：Angel.exe+IDB / IDA MCP 恢复 / 用 BeiDou.exe.i64 查 4 个解码地址函数体。
- 详见 `evidence/angel-forensics.md`。

## 四、服务端发包侧裁决（四方证据，已抽验）

> 权威源：`E:/project/BeiDou-Server_s9/.../PacketCreator.java`。**关键陷阱**：客户端注释里的 `PacketCreator.java:行号` 已全部漂移失效（如注释称 giveForeignBuff :3040 实为 updateQuestInfo、称 spawnMist :4074 实为 party op）。必须按方法名定位真实方法体。详见 `evidence/server-sendercrosscheck.md`。

- 已核 ~40 个发包方法 / 约 25 个 opcode；高危点全部亲验，行号一字不差。
- **确实下发（改 length() 正确）**：GiveForeignBuff、DamagePlayer skill==-3、DamageMonster 含 skin、SpawnChar level short、quest skip5、SpawnMist、SpawnSummon、SkillEffect、ApplyMonsterStatus 等大批 handler。
- **有条件/版本差（保留门控）**：UPDATE_QUEST_INFO case8 双版本、getFindResult type 条件、damagePlayer skill==-3/-4、spawnMonsterInternal 尾部分支、CharInfo mount/pet。
- **死 opcode（改了也不生效）**：SET_ITEM_FINAL_DAMAGE(0x175) 等 0x176/0x17A 全库 0 处发包。

## 五、三级风险分级（定稿）

| 分级 | 判据 | 处置 | 代表点（带行号） |
| --- | --- | --- | --- |
| **A · 确定修** | 服务端确认下发 + 两端字段宽度一致 + 门控恒假致字段被吞 | 直接 `available()>=N`→`length()>=N` | GiveForeignBuff(:1086/1099/1101/1103，buff 全失效)、DamagePlayer(:1058/1062)、DamageMonster(:1453-1459)、quest skip5(Messaging:212)、SpawnMist(:1287-1297)、已修先例 CashShop 76 处 |
| **B · 值条件/双分支，需手动核对** | 门控与前置字段值耦合，或服务端存在版本分支 | 改 length() 但**保留值条件 && / 双分支语义** | QuestHandlers.cpp:96-99 `>=4 read_int / >=2 read_short` 双版（**命门，盲改必错位**）；getFindResult type==RT_SAME_CHANNEL；damagePlayer skill==-3/-4；spawnMonsterInternal 尾分支 |
| **C · 无对照/死码，需决策** | MinMS 无此 handler，或服务端根本不发 | 先确认是否值得修 / 直接标死码 | WaveAShell 200 处（无跨端对照，需逐条核服务端 CASHSHOP 分支）；Alliance 3 处；SET_ITEM_FINAL_DAMAGE(CommonHandlers.cpp:87，死 opcode) |

### 冰山下的模式结论
- 这是**未完成的人工迁移**，不是新 bug：CashShop 已迁移完、ItemParser 已修，MapObject/Messaging/WaveAShell 未迁移。
- 全局唯一"改 length() 会真的读错位"的命门只有 `QuestHandlers.cpp:96-99` 双分支一处；其余恒假门控改 length() 都只会"读到真值"（修 bug），不会引入错位。
- 死 opcode 那几处改了也不生效，属可清理死代码，优先级最低。

## 六、修复建议与门禁

- **建议范围**：A 类 329 处中确定下发的部分可直接改；B 类（尤其 QuestHandlers 双分支）必须人工保留语义；C 类 WaveAShell 200 处需先逐条核服务端 CASHSHOP_OPERATION 分支再定；死 opcode 单独列为清理项。
- **强制门禁**（本仓库 MSBuild 增量陷阱，已有踩坑记录）：touch 改过的源文件 + `rm x64/obj-Release/<文件>.obj` → 核对 obj mtime > cpp mtime、构建 error C=0、部署 exe SHA 相对基线变化。
- **真机验收**：改完必须真机触发对应 opcode，核对字段不再被吞（如 GiveForeignBuff 的 buff 生效、DamagePlayer 伤害值正确）。**编译过 ≠ 修好了**；真机发包未验收须如实声明。
- **本轮未改码**，以上为排查结论与修复建议，实施需用户确认。

## 七、产物索引
- `evidence/client-sites.csv` — 329 处精确清单（file,line,N,function,form,source）
- `notes/client-side-findings.md` — 客户端定义铁证 + 范式 + 机制 + 先例
- `evidence/minms-crosscheck.md` — MinMS 跨端对照（一致/版本差/无法对应）
- `evidence/angel-forensics.md` — Angel 取证边界（无相关证据 + 两套代码库认知）
- `evidence/server-sendercrosscheck.md` — 服务端发包裁决（注释行号漂移陷阱 + 三分类 + 命门）
