# 095PLUS2 · 客户端/服务端取证与反编译状态记录

> 记录时间：2026-11-01
> 取证对象：`E:\MXD\【2021最新】095PLUS2_10.1国庆版`
> 当前状态：**只读取证**，未修改任何代码/资源

---

## 1. 核心结论

| 问题 | 结论 |
|---|---|
| 095 客户端是否已用 rea 完整反编译 | **否。一次都没有反编译过。** |
| 两个国产 095 客户端能否静态反编译 | **不能，均已加壳**。浩浩岛095 `开启游戏.vmp.exe`（VMProtect 特征：230 函数/406 字符串全部在加密段 `.vmps2`，前 9 段磁盘 RawSize=0）；PLUS2 `MapleStory.exe`（3 函数/14 字符串，代码与 `.rsrc` 熵均 7.99）。详见 [notes/ida-095-initial.md](file:///E:/project/MapleStory-Client/docs/features/port-095/notes/ida-095-initial.md) |
| 是否有可用的 095 客户端证据源 | **有**：`E:\MXD\Maplestory95.i64` 是一份**未加壳 GMS v95** 的完整 IDB（22,428 函数 / 21,255 具名 / 2,583 字符串，原始路径 `…\v95\Maplestory95.exe`），可作结构/公式/opcode 的替代证据源 |
| 095 服务端 `095.jar` 是否已反编译 | **否，且不可直接反编译**：class magic 为 `00FEBABE`（正常应为 `CAFEBABE`），类文件被加密，javap 报 `unexpected tag at #4: 14` |
| 已取证到什么程度 | 文件级清单、配置、数据库 schema/表结构、服务端脚本数量、83 个 special 脚本正文、服务端 XML-WZ 目录树；客户端二进制为加壳侦察 + v95 GMS IDB 结构取证 |
| 国产 095 客户端二进制 / WZ 内容 | 二进制**受壳保护无法静态解析**；17 个客户端 WZ 仍未解析（待 orange-wz） |

### 已存在的 IDA 数据库（但不能用）

| 文件 | 大小 | 时间 |
|---|---|---|
| `V095冒险岛PLUS2客户端\MapleStory.exe.i64` | 21,703,723 | 2026-10-31 23:51 |
| `E:\MXD\Maplestory95.i64` | 42,343,783 | 2026-09-17 |
| `E:\MXD\Maplestory95.idb` | 48,405,304 | 2023-07-02 |
| `E:\MXD\BeiDou_idb\BeiDou.idb`（当前项目客户端，非 095） | 160,314,517 | 2026-10-27 |

这些是 IDA 分析库，**没有 IDA MCP 或 IDA 桌面端就无法导出函数/字符串/opcode 表**。

> **2026-11-01 更新**：IDA MCP 已就绪并完成首轮分析（见 [notes/ida-095-initial.md](file:///E:/project/MapleStory-Client/docs/features/port-095/notes/ida-095-initial.md)）。结论：
> - `V095冒险岛PLUS2客户端\MapleStory.exe.i64` → 对应 exe **加壳**，IDB 里只有 3 个函数 / 14 个字符串，**不可用**。
> - `E:\MXD\Maplestory95.i64` → **未加壳 GMS v95 完整分析库**（22,428 函数 / 21,255 具名 / 2,583 字符串），**可用**，已作为 095 客户端逻辑的证据源。
> - `E:\MXD\BeiDou_idb\BeiDou.idb` 仍是当前 v83 客户端，与 095 无关。

---

## 2. 版本判定依据（已确认是 v0.95）

| 证据 | 值 |
|---|---|
| `MapleStory.exe` FileVersion / ProductVersion | `1, 0, 0, 95` |
| FileDescription / CompanyName | MapleStory / Wizet |
| 服务端 `world.properties` | `net.sf.odinms.login.serverName=v.95`，`eventMessage=#bv.95`，`serverMessage=v.95` |
| 客户端 WZ | PKG1 格式，17 个包，合计约 6.0 GB |
| 服务端架构 | OdinMS / HeiMS 血统（`net.sf.odinms.*` 配置键、`ZeroMS\095.jar`） |
| 数据库 | 127 张表（含 v95 特有的 potential / bossrank / monsterbook / character_cards 等） |

---

## 3. 客户端资产清单

目录：`E:\MXD\【2021最新】095PLUS2_10.1国庆版\V095冒险岛PLUS2客户端`（68 个条目，约 6.0 GB）

### 3.1 主程序与关键二进制

| 文件 | 大小 | SHA256 / 说明 |
|---|---|---|
| `MapleStory.exe` | 5,660,912 | `36c9b98e717f1ba74663f3f1bd3279b0517d333b54e6b6d0273f2f9b1fec7a7c`，v1.0.0.95 |
| `MapleStory.exe.i64` | 21,703,723 | IDA 64 数据库；已用 IDA MCP 打开 → 对应 exe 加壳，库内仅 3 函数 / 14 字符串 |
| `ijl15.dll` | — | 渲染/输入层 DLL，自研插件常挂载点 |
| `NameSpace.dll` / `Canvas.dll` / `PCOM.dll` / `ResMan.dll` | — | 客户端模块化 DLL |
| `Gr2D_DX8.dll` / `Gr2D_DX9.dll` / `d3dx8d.dll` / `d3dx9_31.dll` | — | 图形层 |
| `HPSocket4C.dll` | — | 网络层 |
| `WzMss.dll` / `WzFlashRenderer.dll` | — | WZ / Flash 渲染 |
| `Sddyn03.dll` / `sdcit.dll` / `dinpu1t8.dll` / `v3hunt.dll` | — | 反外挂/保护相关 |
| `HShield\` / `HShield.bak\` | — | 反外挂目录 |
| `ASPnLchr.exe` / `【AA】冒险岛万能联机登录器.exe` / `[AA]单机登录器.bat` | — | 启动器（改 IP/端口） |
| `mxdproxy.conf` | 1,315 B | 代理配置：`path=D:\095版本\...\MapleStory.exe`、`ip=127.0.0.1`、`login=8484`、`shop=8600`、`channel=7575-7585` |
| `SDDyn.ini` | 75 B | 指向 `http://mxd.autopatch.sdo.com/maplestory/mxdfile/` |

### 3.2 WZ 资源（17 个，未解析）

| 文件 | 大小（字节） |
|---|---|
| Base.wz | 8,915 |
| Character.wz | 1,640,436,795 |
| Effect.wz | 293,585,477 |
| Etc.wz | 21,662,753 |
| Item.wz | 143,802,589 |
| List.wz | 65,470 |
| Map.wz | 776,158,270 |
| Mob.wz | 909,526,106 |
| Morph.wz | 14,889,642 |
| Npc.wz | 73,969,473 |
| Quest.wz | 11,431,224 |
| Reactor.wz | 110,957,095 |
| Skill.wz | 206,595,609 |
| Sound.wz | 526,951,641 |
| String.wz | 5,263,350 |
| TamingMob.wz | 1,315 |
| UI.wz | 53,358,454 |

> 注意：客户端**没有** `Mob` 以外的 Data 目录结构，全部走 PKG1 打包；服务端另有解包后的 XML 树（见 §4.3）。

---

## 4. 服务端资产清单

目录：`E:\MXD\【2021最新】095PLUS2_10.1国庆版\V095冒险岛PLUS2服务端`

### 4.1 核心

| 文件 | 大小/说明 |
|---|---|
| `ZeroMS\095.jar` | 3,295,270 B，SHA256 `50cfe4d2af271fec4471881d84d72b24911ab19a8c0b83e8160944de407c7036`，**class 加密（00FEBABE）** |
| `lib\` | 5 个：`druid-1.1.9.jar`、`mysql-connector-java-bin.jar`、`netty-all-4.1.39.Final.jar`、`slf4j-api-1.7.7.jar`、`slf4j-simple-1.7.7.jar` |
| `启动服务端.exe` | 192,512 B 启动壳 |
| `jrex64\` | 内置 JRE |
| `Mysql\` | 内置 MySQL |

### 4.2 配置

| 文件 | 关键内容 |
|---|---|
| `world.properties` | `serverName=v.95`、`exp=1`、`meso=50`、`drop=1`、`pdjb=10000`、`maxlevel=255`、`kocmaxlevel=255`、`maxCharacters=30`、潜能改变几率 `renewPotentia*` 全 100、事件清单见下 |
| `channel.properties` / `beans.properties` | 频道与抽奖/礼包概率表（`hymzjl`、`dbg`、`zsg` 等） |
| `数据库备份\210522214619.psc` | 1,518,434 B，SHA256 `5bdcda07dae8d2c96754f007858c62a2ab3b512abc4fb1548d9be2d2762d706a`，zlib 压缩备份 |

启用事件（world.properties `net.sf.odinms.channel.events`，共 66 项）：
`Christmas, Olivia, CygnusBattle, ScarTarBattle, VonLeonBattle, Ghost, MV, cpq, cpq2, OrbisPQ, HenesysPQ, Romeo, Juliet, Pirate, Amoria, Ellin, CWKPQ, Rex, DollHouse, BossBalrog_EASY, BossBalrog_NORMAL, HorntailBattle, Nibergen, PinkBeanBattle, ZakumBattle, NamelessMagicMonster, Dunas, Dunas2, 2095_tokyo, ZakumPQ, LudiPQ, ServerMessage, KerningPQ, ProtectTylus, WitchTower_EASY, WitchTower_Med, WitchTower_Hard, Vergamot, ChaosHorntail, ChaosZakum, CoreBlaze, BossQuestEASY, BossQuestMed, BossQuestHARD, BossQuestHELL, Ravana_EASY, Ravana_HARD, Ravana_MED, GuildQuest, Aufhaven, English, PVP, ArkariumBattle, Subway, ProtectPig, shaoling, Dragonica, Ghostbaby, MonsterPark, MesoPQ, HillaBattle, BossBloody_CHAOS, BossPierre_CHAOS, BossBanban_CHAOS, BossBelen_CHAOS, BossMagnus_HARD`

### 4.3 脚本（共 2,808 个文件）

| 目录 | 数量 |
|---|---|
| `npc` | 1,023 |
| `quest` | 520 |
| `event` | 113 |
| `special` | 83 |
| `portal` | 748 |
| `reactor` | 271 |
| `liedetector` | 50 |

> 已提取正文的只有 `special` 的 83 个（见 §5）；`npc/quest/event/portal/reactor/liedetector` 目前只有数量，**正文未归档**。

### 4.4 服务端 XML-WZ（解包树）

| 目录 | XML 文件数 |
|---|---|
| Base.wz | 4 |
| Character.wz | 25,672 |
| Character.wz2 | 25,672（重复副本） |
| Effect.wz | 38 |
| Etc.wz | 39 |
| Item.wz | 421 |
| Map.wz | 7,778 |
| Mob.wz | 2,157 |
| Morph.wz | 98 |
| Npc.wz | 2,081 |
| Quest.wz | 2,886 |
| Reactor.wz | 574 |
| Skill.wz | 127 |
| Sound.wz | 59 |
| String.wz | 14 |
| TamingMob.wz | 12 |
| UI.wz | 31 |

---

## 5. 已提取证据索引（可复核）

| 内容 | 位置 | 方法 |
|---|---|---|
| 数据库完整 SQL（GBK→UTF-8 重建） | `E:\project\_ms_probe\095db_clean.sql`（10,296,148 B） | `.psc` zlib 解压 → 按 NUL 分块 → GBK 解码 → 重组 |
| 127 张表清单 | [evidence/tables.txt](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/tables.txt) | 从 SQL 抽取 |
| 表结构摘要 | [evidence/schema_summary.txt](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/schema_summary.txt) | 同上 |
| world.properties 全文 | [evidence/world_props.txt](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/world_props.txt) | 直接读取 |
| 83 个 special 脚本名 | [evidence/special_names.txt](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/special_names.txt) | 目录枚举 |
| 83 个 special 脚本正文 | `E:\project\_ms_probe\special_utf8\*.js` | GBK→UTF-8 转码 |
| 客户端文件清单 | [evidence/client-files.txt](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/client-files.txt) | `ls -la` |
| 服务端文件清单 | [evidence/server-files.txt](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/server-files.txt) | `ls -la` |
| 脚本数量统计 | [evidence/script-counts.txt](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/script-counts.txt) | `find | wc -l` |
| XML-WZ 数量统计 | [evidence/wz-xml-counts.txt](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/wz-xml-counts.txt) | 同上 |
| 关键产物哈希 | [evidence/hashes.txt](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/hashes.txt) | `sha256sum` |
| 095 客户端 IDA 首轮分析 | [notes/ida-095-initial.md](file:///E:/project/MapleStory-Client/docs/features/port-095/notes/ida-095-initial.md) | IDA MCP（13339 / 13338） |
| PE 段表与熵（两个 095 客户端） | [evidence/pe-sections.txt](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/pe-sections.txt) | 直接读 PE 头 |
| 浩浩岛095 vmp 客户端侦察 | [evidence/ida-survey-vmp.json](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/ida-survey-vmp.json) | `survey_binary` |
| PLUS2 客户端侦察 | [evidence/ida-survey-plus2.json](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/ida-survey-plus2.json) | `survey_binary` |
| vmp 客户端完整导入表（383 项） | [evidence/imports-vmp.txt](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/imports-vmp.txt) | `imports` 分页 |
| GMS v95（未加壳）侦察 | [evidence/ida-survey-v95gms.json](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/ida-survey-v95gms.json) | `survey_binary` |
| v95 `GW_CharacterStat::Decode` 反编译 | [evidence/decompile-v95-GW_CharacterStat_Decode.c](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/decompile-v95-GW_CharacterStat_Decode.c) | `decompile` |
| v95 `DecodeMoney` / `MobStat::SetFrom` | [evidence/decompile-v95-GW_CharacterStat_DecodeMoney.c](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/decompile-v95-GW_CharacterStat_DecodeMoney.c) · [evidence/decompile-v95-MobStat_SetFrom.c](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/decompile-v95-MobStat_SetFrom.c) | `decompile` |
| **v95 opcode 表（接收全量 + 发送 238 项）** | [notes/opcode-v95.md](file:///E:/project/MapleStory-Client/docs/features/port-095/notes/opcode-v95.md) | 反编译分发器 + 批量抽取 `COutPacket` 常量 |
| **v95 ↔ s9 opcode 逐条映射** | [notes/opcode-map-v95-vs-s9.md](file:///E:/project/MapleStory-Client/docs/features/port-095/notes/opcode-map-v95-vs-s9.md) | 人工语义裁定 + 证据等级标注（238 发送 / 274 接收） |
| opcode 证据包（分发器反编译、二级表、发送表、脚本） | [evidence/opcode-v95/](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/opcode-v95/) · [索引 README](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/opcode-v95/README.md) | `decompile` / `xref_query` × 582 / 生成脚本；含复现步骤与校验和 |

---

## 6. 尚未取证（缺口清单）

| # | 缺口 | 依赖 | 影响 |
|---|---|---|---|
| 1 | 国产 095 客户端 opcode 表、packet handler、UI 系统、伤害/经验公式未提取 | 已部分缓解：GMS v95 IDB 可提取结构与公式；**国产自定义协议仍需动态脱壳/抓包** | P1/P3 移植中"国产自定义"部分仍缺协议基线 |
| 1b | GMS v95 IDB 的 opcode 表已导出 ✅；各系统 `Decode` 函数（字段顺序）尚未导出 | 无（IDB 已可读） | 剩余部分即下一步 `notes/v95-decoders.md` |
| 2 | 17 个客户端 WZ 未解析（String/Mob/Map/Item/UI/Skill 等） | orange-wz MCP | 无法确认 v95 字段差异与资源可用性 |
| 3 | `095.jar` 内部逻辑（加密） | 解密器 / 运行期抓包 | 只能按行为复刻，无法逐行对照 |
| 4 | `npc/quest/event/portal/reactor/liedetector` 脚本正文未归档 | 无（可直接读） | P0/P3 具体玩法细节缺失 |
| 5 | 服务端 XML-WZ 字段未抽样（尤其 Mob maxHP/exp 的 int32 边界） | 无（可直接读）或 orange-wz | 影响血量/经验上限方案 |
| 6 | 客户端与 095 服务端的真实封包样例（登录、进图、伤害、Boss 血条） | 抓包工具 + 可运行环境 | 协议升级缺少对照基线 |

---

## 7. 095 服务端系统清单（依据 DB + 脚本）

### 7.1 数据库特有表（相对当前 s9）

- 排名/日志：`bossrank`、`bossrank1`–`bossrank9`、`bosslog`、`aclog`、`addlog`、`battlelog`、`cheatlog`、`compensationlog_confirmed`
- 角色扩展：`achievements`、`character_cards`、`androids`、`familiars`、`monsterbook`、`hyperrocklocations`、`character_slots`
- 社交/联盟：`alliances`、`buddies`、`bbs_threads`、`bbs_replies`
- 商业：`cashshop_limit_sell`、`cashshop_modified_items`、`csequipment`、`donate`、`website_events`、`website_news`、`nxcode`、`nxcodez`
- 潜能：`inventoryequipment.potential1`–`potential5`
- PVP：`pvp*` 系列字段

### 7.2 自定义系统（special 脚本 83 个，按主题归类）

- 签到/日常：`每日签到`、`每日收集`、`每日通缉`、`跑环测试`、`等待補償`
- 排名：`扎昆排名`、`暗黑龙王排名`、`班雷昂排名`、`武陵道场排名`
- 掉落查询：`怪物掉宝查询`、`掉落查询`、`怪物爆率`、`高级检索`
- 兑换/商店：`楓葉兌換`、`楓葉商店`、`水晶兌換`、`雪花兌換`、`钓鱼兑换`、`枫叶换点`、`打王兑换`、`贝勒德兑换`、`礼包10086`、`贊助獎勵`、`紅利點數`
- 强化/潜能：`强化系强化系统`、`强化系传承系统`、`装备潜能`、`潜能魔方`、`砸卷系统`、`装备附加属性`、`奇幻方塊碎片`、`增加砸卷次数`
- 转职/技能：`转职系统`、`萬能轉職`、`学满技能`、`終極轉生`
- 传送/清包：`快捷传送`、`萬能傳送`、`采集传送`、`清包1`–`清包10`、`数字清包`
- 推广/充值：`推广系统`、`推广任务`、`自助开双`、`进入商城`
- 副本入口：`怪物公園1`–`3`、`狮子王城`、`闹钟进入`
- 其他：`万能洗血_`、`洗血系统`、`洗血智戒`、`三国套装`、`漫新套装`、`考古套装`、`公婆戒指`、`公會系統`、`職業坐騎`、`抽奖`、`抽奖积分兑换`、`答题换点券`、`答题换金币`、`个人信息`、`聚合功能`

### 7.3 脚本使用的自定义 API（移植时需兼容层）

`getBossRank9` / `setBossRankCount9` / `getBossRankCountTop9` / `getBossRankCount9`、`getAcLog` / `setAcLog` / `getAcLogS`、`checkMobs` / `checkDrop`、`UpdateDropChance` / `AddDropData`、`getEquipList`、`getPotenttalFyzxlist`、`getMpR`（道具 MP 当强化等级）、`gainNX` / `gainNX2`、`worldMessage`、`getCharacterNameById`、`getCurrentTime`、`getMrqdTime`

---

## 8. 移植分层（与执行方案一致）

| 层 | 内容 | 可行性 | 依赖 |
|---|---|---|---|
| P0 轻量自定义 | 签到、掉落查询、兑换商店、传送、清包、Boss 排名、推广 | 高（脚本 + 兼容 API） | 无需客户端改动 |
| P1 版本核心 | 潜能/魔方、成就、角色卡、超时空石、PVP 基础、机器人/精灵 | 中 | 客户端 UI + 协议 + WZ |
| P3 副本与 Boss | 怪物公园、BossQuest、阿卡伊鲁姆/希拉/麦格纳斯/鲁塔比斯/混沌、MesoPQ、2095 东京、狮王 | 低—中 | 地图 WZ + 怪物 AI + 掉落 + 入场条件 |

---

## 9. 后续取证计划（MCP 就绪后执行）

| 阶段 | 动作 | 产出 |
|---|---|---|
| A | 启用 rea / IDA / orange-wz MCP，先 `ping` 验证 | 可用性记录 ✅ |
| A2 | 侦察两个国产 095 客户端 → **均加壳**；发现未加壳 GMS v95 IDB 可替代 | [notes/ida-095-initial.md](file:///E:/project/MapleStory-Client/docs/features/port-095/notes/ida-095-initial.md) ✅ |
| B | 在 `E:\MXD\Maplestory95.i64` 上导出：opcode 表、`CClientSocket::ProcessPacket` 分发、`CWvsContext::OnPacket`、`CField::OnPacket`、伤害/经验公式、各系统 `Decode` | `notes/opcode-v95.md` ✅（opcode 部分）、`notes/v95-decoders.md`（待做） |
| B2 | （国产自定义协议）REA + Frida 挂 `WS2_32!send/recv` 抓明文，免脱壳取 opcode + 包体 | `evidence/packets/` |
| C | orange-wz 解析 17 个 WZ：String/Mob/Map/Item/UI/Skill 节点抽样，记录 v95 与 v83 字段差异 | `notes/wz-v95-diff.md` |
| D | 服务端脚本全量转码归档（npc/quest/event/portal/reactor/liedetector） | `evidence/scripts/` |
| E | 运行 095 服务端 + 客户端抓包，记录登录/进图/伤害/Boss 血条样例 | `evidence/packets/` |
| F | 汇总为 `notes/packet-matrix-v95-vs-v83.md` 与 `notes/feature-matrix.md` | 移植对照表 |

---

## 10. MCP 服务状态（2026-11-01 01:15）

### 10.1 IDA（三个端点，均已验证 `initialize` + `tools/list`）

| 端口 | 进程 | 目标 | 说明 |
|---|---|---|---|
| 13339 | `ida.exe` pid 32644 | `E:\MXD\【2023.8.8江浩生日福利】怀旧岛192整合版\浩浩岛095(9.1测试端)\开启游戏.vmp.exe.i64` | module `开启游戏.vmp.exe`，imagebase `0x400000`，Hex-Rays 就绪、自动分析就绪；该目录没有 `MapleStory.exe`，此 11.7 MB exe 即 095 客户端主体 |
| 13337 | `ida.exe` pid 31332 | `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64` | 当前 v83 客户端（非 095） |
| 13338 | `idalib-mcp.exe` pid 32604 | 无独立库，adopt 上述两个 GUI 实例 | headless 入口，调用需带 `database` |

> 备注：13339 的函数名均为 `sub_*` / `nullsub_*`，无符号；二进制内可检索到 `MapleStory`、`095` 字符串，未检索到 `VMProtect` / `.vmp0` 段名。
> **已确认加壳**（2026-11-01）：段名为 `.vmps0/.vmps1/.vmps2` + `.mackt` + 随机名 RWX 段；前 9 段磁盘 RawSize=0，全部 230 个函数落在熵 7.78–7.97 的 `.vmps2` 加密数据内。详见 [notes/ida-095-initial.md](file:///E:/project/MapleStory-Client/docs/features/port-095/notes/ida-095-initial.md)。
>
> 另：13338 headless 已同时打开 `E:\MXD\Maplestory95.i64`（未加壳 GMS v95，session `86196e43`），作为 095 客户端逻辑的替代证据源。

### 10.2 orange-wz

- 端口 **10012**，Spring Boot 已监听，`/mcp` 工具列表 29 个（`load_files` / `query_nodes` / `mutate_nodes` / `save_as` / `change_key` 等）。
- 启动方式：`target\jre\bin\java.exe ... -jar target\OrzRepacker.jar`。
- **加密包不可用**：`OrzRepacker-encrypted.jar` 启动报 `Startup failed, invalid password`（缺密码），因此改用未加密的 `OrzRepacker.jar`，功能一致。
- 运行日志：`E:\project\orange-wz\logs\mcp-10012-manual.log`。

### 10.3 REA

- 程序：`D:\software\nodejs-24\node.exe D:\software\nodejs-24\node_modules\rea-agents\scripts\rea.mjs mcp`（rea-agents，MIT，工具数 207）。
- 095 专用配置：`C:\Users\11036\.rea\ida-mcp-095.json` → `http://127.0.0.1:13339/mcp`（attached / http / native）。
- 原 `C:\Users\11036\.rea\ida-mcp.json` 仍指向 13337（BeiDou）。

### 10.4 Qoder 注册结果

| 名称 | 类型 | 地址 | 状态 |
|---|---|---|---|
| `ida-095` | http | 127.0.0.1:13339/mcp | ready，66 工具 |
| `ida-beidou` | http | 127.0.0.1:13337/mcp | ready |
| `ida-headless` | http | 127.0.0.1:13338/mcp | ready |
| `orange-wz` | http | 127.0.0.1:10012/mcp | ready，29 工具 |
| `rea-095` | stdio | rea.mjs mcp + `REA_IDA_MCP_CONFIG=ida-mcp-095.json` | ready，207 工具 |

> 注册写入 `C:\Users\11036\.qoder\settings.json`。**当前会话的工具目录在会话启动时固定**，新注册的 MCP 需重载/重启 Qoder 后才会出现在本会话工具列表中；重载前仍可通过 JSON-RPC（curl）直接调用端点。

---

## 11. 变更记录

| 日期 | 内容 |
|---|---|
| 2026-11-01 | 建立本记录；完成文件级清单、脚本/DB 统计、哈希固化；确认 rea/IDA/orange-wz MCP 不可用、095.jar 加密、客户端未反编译 |
| 2026-11-01 | 启动并注册 MCP：IDA 13339（095 `开启游戏.vmp.exe`）/13337（BeiDou）/13338（headless）、orange-wz 10012（明文 jar）、rea-095（指向 13339）；补充 §10 |
| 2026-11-01 | **IDA 首轮分析**：两个国产 095 客户端均加壳（230 / 3 函数），静态不可反编译；发现 `E:\MXD\Maplestory95.i64` 为未加壳 GMS v95（22,428 函数），已提取 `GW_CharacterStat::Decode`（level=1B、HP/MP/EXP=4B）、`DecodeMoney`（4B）、`MobStat::SetFrom`；新增 [notes/ida-095-initial.md](file:///E:/project/MapleStory-Client/docs/features/port-095/notes/ida-095-initial.md) 与 7 份 evidence；更新 §1/§5/§6/§9/§10.1 |
| 2026-11-01 | **v95 opcode 表导出**：顶层 `CClientSocket::ProcessPacket`（uint16，16–20/23 特例、28–140 → CWvsContext、其余 → g_pStage）+ `CWvsContext::OnPacket` 106 项 + `CField::OnPacket` 141–432 全区间（含 23 个池/对话框二级分发、163=虚函数 `OnClock`、196=无处理、432=OnLogoutGift）+ 客户端发送 217 项（462 个函数 / 582 处 `COutPacket` 构造点）；与 s9 现有编号抽样对照（联盟 167 vs 143、制造 125 vs 113 等）；新增 [notes/opcode-v95.md](file:///E:/project/MapleStory-Client/docs/features/port-095/notes/opcode-v95.md)、[evidence/opcode-v95/](file:///E:/project/MapleStory-Client/docs/features/port-095/evidence/opcode-v95/)（含生成脚本与哈希）；更新 §5/§6/§9 |
| 2026-11-01 | **v95 ↔ s9 opcode 映射完成**：发送侧 217→**238**（582 处构造点全部归因：502 处完整反编译 + 80 处截断/漏采函数二次取证，含同函数共享 `push` 回溯）；新增 [notes/opcode-map-v95-vs-s9.md](file:///E:/project/MapleStory-Client/docs/features/port-095/notes/opcode-map-v95-vs-s9.md)（238 发送 + 274 接收逐条对照，含 12 个新 opcode 的构造点证据与 5 处语义修正：44/45/46、103、144、214、219）；证据包补 `send_sites_all.json`/`map_candidates2.json`/`s9_opcodes.json` 等 6 份数据与 8 个新复现脚本，`hashes.txt` 扩容至 14 项；记录攻击类发送 opcode 静态取不到的阻塞（需抓包） |
