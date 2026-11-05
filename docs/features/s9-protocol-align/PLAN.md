# s9 协议对接计划

> 目标仓库：`E:\project\MapleStory-Client`（C++ 客户端，git 分支 `feature/dev_0.0.1`）
> 规范源：`E:\project\BeiDou-Server_s9`（Java 21 + Netty，`ServerConstants.VERSION = 83`）
> 制定日期：2026-10-30 ｜ **v2 修订**：按用户授权放开验证边界（可重启服务端、疑问须实证闭合），并新增 T8 Python 协议探针、T9 服务端日志配对管线
> 状态：**待用户确认后开工**

---

## 0. 目标 / 非目标

**目标**

1. 客户端封包层与 s9 在**字节级**对齐，且每条结论都有 s9 `path:line` 证据 + 真机抓样，可复核。
2. 建立**不会过期**的对账门禁：对照表由脚本从两侧源码生成，源码一变即失效。
3. 闭合用户选定的四层：基线（传输 + opcode）、字段级 body、GBK 中文链路、缺失系统归因与补齐。

**非目标（本次明确不做）**

- 不改服务端：不改代码、不改 `application.yml`/`log4j2.xml`、不重启进程。你 IDE 里跑着的 PID 40688 我不碰。
- 不做 DDL/DML。库内验证只用 dbx **只读** SELECT。
- 不再回到逆向 BeiDou.exe 取协议常数。既有决策不变：**协议以 s9 为准**，逆向只在 s9 缺字段时回指。
- 不动 `Data/*.img` 与 WZ 内容（本计划纯协议层）。

---

## 1. 规范源清单（唯一事实源，全部只读）

| 关注点 | 文件 : 行 | 用途 |
|---|---|---|
| 版本 | `constants/net/ServerConstants.java:6` `VERSION = 83` | header 数学与密钥派生的输入 |
| c2s opcode | `net/opcodes/RecvOpcode.java`（227 行，181 常量 / 180 去重值） | 客户端 `OutPacket` 的对照基准 |
| s2c opcode | `net/opcodes/SendOpcode.java`（378 行，311 条） | 客户端 `PacketSwitch` 的对照基准 |
| 注册表 | `net/PacketProcessor.java:138-319`（174 行 `registerHandler`） | opcode → handler 可达性判定 |
| 分派 | `client/Client.java:207-236`（`:213` 正文 `readShort` LE） | 证明 opcode 不在 header |
| 组帧 | `net/encryption/protocol/GMSV83PacketProtocol.java:30-43`（收）`:60-67`（发） | 收发顺序与 header 语义 |
| header | `net/encryption/MapleAESOFB.java:141-171` | `getPacketHeader` / `isValidHeader` |
| 加密 | `net/encryption/MapleAESOFB.java:38-135,173-218` | 静态 key、OFB 块 `0x5B0→0x5B4`、`updateIv` |
| 二次混淆 | `net/encryption/MapleCustomEncryption.java:38+`（6 遍交替） | AES 之内的一层，易漏 |
| IV | `net/encryption/InitializationVector.java:14-26`、`ClientCyphers.java:15-16` | send `{82,48,120,rnd}` / recv `{70,114,122,rnd}`；密钥按 `0xFFFF-83` / `83` |
| hello | `util/PacketCreator.java:619-629` | 明文握手 16 字节 |
| 字符串 | `net/packet/ByteBufOutPacket.java:77-92`、`ByteBufInPacket.java:51-57` | `u16 LE 长度 + 字节`，字符集见 §5 |
| 字符集 | `constants/string/CharsetConstants.java:53-58,76` | lang 3→GBK、2→US-ASCII，未知回退 `service.language` |

服务端**无任何协议文档**（`docs/features/*` 是空脚手架），因此对照表只能自造。

---

## 2. 开工前已核实的事实与缺陷

| # | 结论 | 证据 | 处置 |
|---|---|---|---|
| F1 | 握手已对齐，无需改动 | s9 hello = `u16 0x0E, u16 83, u16 1, u8 49, 4B recvIv, 4B sendIv, u8 8`（`PacketCreator.java:619-629`），recvIv 落在偏移 7；客户端 `Cryptography.cpp:22-31` 取 `handshake[i+7]` 作 sendiv、`[i+11]` 作 recviv | P0-T7 只做真机取证确认 |
| F2 | opcode 不在 header，在正文 LE 2 字节 | `Client.java:213` ↔ `PacketSwitch.cpp:772` | 无需改动 |
| F3 | 收发次序同形：发 = custom→AES，收 = AES→custom | `GMSV83PacketProtocol.java:30-43,60-67` ↔ `Cryptography.cpp:37-51` | **T4 已对拍证实**（20 项检查 19 项全等，含 300 包连续会话） |
| **F4** | **s9 对"没人认领的 opcode"完全静默**：`getHandler()` 返回 null 时整块跳过，不打日志、不断线，只 `updateLastPacket()` | `Client.java:207-235`（`handler != null &&` 一支没有 else） | 取证必须依赖 recv 日志开关：`Client.java:215-217` 的 `use_debug_show_rcvd_packet` 在 null 检查**之前**打印，是唯一能区分"没收到"与"收到但没人收"的手段（详见 `notes/opcode-diff.md` §1 取证要点） |
| **F5** ✅ **值已改（T6），实机验收未做** | 客户端 c2s 枚举里混进了 s2c 值：309/314/325 是 s9 `SendOpcode` 的 STORAGE/PLAYER_INTERACTION/CASHSHOP_OPERATION，收侧对、发侧错 | `OutPacket.h:138,166,173`（改后行号；`F5-n` 注释在 `:136/:164/:171`）↔ `RecvOpcode.java:78,131,199` ↔ `SendOpcode.java:315,320,335`（三值收侧 `HANDLED`，生效 `StorageHandler`/`TradeHandler`/`CashShopOperationHandler`） | **T6 已按 `RecvOpcode` 改值**；但"包能进 handler"≠"字段对得上"——body 级对齐与三条实机取证一起挂 **P2 的 S6/S7/S8** |
| **F6** | **一个错头部不会断线，而是永久废掉这条连接的入站派发**（出站方向照常发 PING，我们回的一切包再不被解出），且服务端**零日志** | T8 探针实测 `evidence/probe/probe-final.txt` 阶段 5 + `evidence/probe/outlog-excerpt.txt`（连接→45s 后仅"由于空闲而断开"）；机制：netty 4.1.109 `ByteToMessageDecoder.channelRead` 把 `decode()` 的异常重包成 `DecoderException`（反汇编 `evidence/probe/netty-ByteToMessageDecoder-channelRead.txt`），于是 `Client.java:253` 的 `instanceof InvalidPacketHeaderException` 恒 false，既不 `closeSession` 也不记日志 | **判据纪律**：以后"服务端接受了吗"一律由 **PONG ack oracle**（`Client.checkIfIdle` 的 15s `lastPong` 复查）或 T9 服务端日志回答，**不得**用"连接还活着"；`notes/probe.md` §4/§5。副作用：客户端任何一处头部/长度算错都表现为"连着但聋了、约 45s 掉线"，D8 正落在这个域里 ⇒ T5 除 `int16_t`→`uint16_t` 外还要加长度守卫，把静默聋变成可观测事件 |
| **D1** ⚠ **35 已修（T6）/ 152 保留** | **recv 撞值，后注册静默覆盖** | `PacketSwitch.cpp:51-407`：`35` = `RECALCULATE_STATS` 与 `FORCED_STAT_RESET`；`152` = `ARIANT_RESULT` 与 `WARN_MESSAGE`；`emplace` 写数组（`:64-99`） | 35：注销后注册的那条，归 `ForcedStatResetHandler`。152：**不改**——s9 当前不发 152，改了也无从验证（`notes/opcode-diff.md` §4 第 6 项） |
| **D2** ✅ **已修（T5），零调用点故仍属潜伏** | `write_long` 只写 4 字节（MSVC `long`=32 位），而 `read_long` 消费 8 | `OutPacket.cpp:77-87`（现 `sizeof(int64_t)`）↔ `InPacket.h:67` | **当前 0 个调用点**（全仓 grep），回归风险≈0；真实验收要等第一个 `long` 字段出现（届时由 T9 配对抓首差异） |
| **D3** | 线路字符集由库内一列决定，客户端不参与 | `Client.java:664,682`（登录读 `accounts.language`）、`Character.java:6770`（进图再设）、`CharsetConstants.java:53-58` | P1-T8 先钉死，否则 §5 全部字符串结论不可信 |
| **D4** | 中文渲染硬阻塞 | `IO/Components/Textfield.cpp:286-297` FreeType 图集只收 32–127 | P1-T10 spike |
| **D5** | 既有对照文档已过期且对齐目标错 | `docs/features/opcode-align-beidou-gms083/README.md` 记"RecvOpcode 193 / SendOpcode 332"（实为 181/311），且对齐 gms083 非 s9 | P0-T3 标记 superseded，本目录接管 |
| D6 | 服务端自身有不可达 handler | `RecvOpcode.java:145-146` `WEDDING_TALK`/`WEDDING_TALK_MORE` 同值 `0x8B`，`PacketProcessor.java:102` 后写覆盖 → `:309` 永不触发 | **只记录**，不改服务端 |
| D7 | ~~客户端封包层零字符集转换~~ **已修（T11，会话 #7 真机闭环）** | 全仓 `GBK/936/MultiByteToWideChar/CP_ACP/iconv` 仅命中 IDA 证据文本。**会话 #5 升级为真机实证**：入站 `0x4D` 的 GBK 中文到达 `std::string` 后被 UTF-8 渲染层静默丢弃，屏幕只剩 `[Notice] 1`（`notes/log-pairing.md` §9.5 + `evidence/chat/notice-cjk-blank-20261031.png`）。**会话 #7 复测反转**：同一包型同一命令，装上 `Net/PacketCharset` 后屏幕出 `[Notice] 1 已被监控`，`evidence/chat/t11-notice-cjk-rendered-20261031.png` | ~~P1-T11 新建~~ ✅ T11 落地（对账管线见 T9） |
| **D8** ✅ **已修（T5）** | **入站头部长度被 `static_cast<int16_t>` 符号扩展**：s2c 包体落在 `32768..65535` 时客户端解出 `size_t` 天文数字 ⇒ 该包永不完成（表现为"卡住"），后续包继续 `memcpy(buffer+pos)` 越界写 | 修复前 `Cryptography.cpp:83`（现 `:89`）↔ `GMSV83PacketProtocol.java:53-57`（服务端返回无符号 16 位）；`Session.cpp:111-127` + `Session.h:66`（`buffer[131072]`） | **T4 对拍新发现**（oracle 第 13 项，62 个长度里 14 个命中）。触发与否取决于是否存在 >32KB 的 s2c 包：现存 `packet_dump.log` 最大仅 1282B，故定级"潜伏"。**T5 已改 `uint16_t` 并加长度守卫，oracle 退出码归 0**；是否真有 >32KB 的 s2c 包仍待 T7 真机长度分布（守卫只保证踩到时是干净掉线，不保证收得下） |
| **D9** （新登记，T5 收尾时发现，**不改**） | `Cryptography.cpp` 两条 `#else` 分支引用了**全仓未定义的 `HEADERLEN`** | `Cryptography.cpp:67`（`create_header`）与 `:93`（`check_length`）；`grep -rn HEADERLEN` 只命中这两行，正式常量是 `NetConstants.h:24 HEADER_LENGTH = 4` | 两处都在 `#ifdef USE_CRYPTO` 的 `#else` 里，而 `USE_CRYPTO` 在 `MapleStory.h:42` **硬定义** ⇒ 死代码，所以现在不报错。后果：**这份客户端的"非加密直通模式"其实编译不过**，任何"关掉加密来调试"的想法都会撞编译错误。本轮不改（动未编译代码 = 没有实测手段验证，违反"证据驱动"），仅登记；将来若要支持，第一步是 `HEADERLEN`→`HEADER_LENGTH` 并补一次实机构建 |

---

## 3. 取证能力矩阵（v2 修订：边界已按用户授权放开）

**先更正一处我这轮早前的错误判断**：我曾说 GM5 `/ShowPackets` 会 `UPDATE game_config`。查源码后不成立——`ShowPacketsCommand.java:39-44` 只调 `GameConfig.update(...)`，而 `config/GameConfig.java:73-127` 仅改动内存里那棵 JSON 树（`valueProp.put("value", …)`）**全程没有 mapper/DAO 调用**，重启即回退。真正写库的是后台路径 `service/ConfigService.java:117-134`（`gameConfigMapper.update(...)` + 写 `lang_resources` + `GameConfig.update`）。两条路径必须分清。

**T9 追加更正（本轮）**：档 B 的生效方式我原来写成"只能走后台 REST"，两边都不准确——`GameConfig` 单例只在 `config/GameConfig.java:22-29` 构造时 `loadGameConfigs()` 读一次库（`service/ConfigService.java:255-256` 的第二次调用只服务于导出 yml，不回写单例），所以 **单纯 UPDATE 库里的 `use_debug_show_packet` 对运行中的服务端完全无效，必须搭配档 D 重启**；反过来 `controller/ConfigController.java:52-54` 的 REST `updateConfig` 是"落库 + 刷内存"两件事一起做，**不用重启**，代价是要后台鉴权并且顺带写 `lang_resources`。档 B 还有一条优势：`InPacketLogger.java:21` / `OutPacketLogger.java:22` 里**没有 `isIgnoredRecvPacket` 判断** ⇒ 档 A 被排除的 7 个 opcode 在档 B 全能看到，且档 B 有出站方向、`<TEXT>` 栏可直接喂 T10/T11 的字符集判断。

| 档 | 能力 | 前置 | 授权状态 |
|---|---|---|---|
| A | **服务端收包日志**：GM5 `!showpackets` 翻转 `use_debug_show_rcvd_packet` → `Client.java:216-218` 打印 `收到封包 包头ID [0x..] 内容： ByteBufInPacket[3100_..]`，落 `logs/out.log`（`log4j2.xml` RollingFile） | 零：不写库、不改配置、不重启（**真机已证**：`GameConfig.java:73-79` 只改内存缓存 JSONObject，全程无 DAO；库里 5 个 `use_debug*` 键 `update_time IS NULL`） | **已真机可用**（会话 #3 拿到 7 条、与客户端 `[SEND]` **7/7 EXACT**，见 `notes/log-pairing.md` §8）。**五点限制须知**：① 全局布尔、内存态，服务端重启即回退；② 必须先用一个能进游戏的 GM5 号打命令；③ **它是取反 toggle**（`ShowPacketsCommand.java:39-45` 写 `!当前值`），会话 #3 就是被第二次输入当场关掉的 ⇒ 一场只输一次；④ F4 已证 s9 对无 handler 的 opcode 不走派发，但档 A 的打印在 null 检查**之前**，所以它是区分"没收到 / 收到没人收"的唯一手段；⑤ **7 个 recv opcode 被硬编码排除**（`LoggingUtil.java:9-17`：`MOVE_PLAYER(41)`、`HEAL_OVER_TIME(89)`、`SPECIAL_MOVE(91)`、`QUEST_ACTION(107)`、`MOVE_PET(167)`、`MOVE_LIFE(188)`、`NPC_ACTION(197)`）⇒ 会话 #3 的 759 个 `MOVE_PLAYER` 一条没打 |
| B | **服务端双向包日志**：`use_debug_show_packet` 翻转后 `InPacketLogger`/`OutPacketLogger` 同时打收发（Netty handler 用 `addBefore("Client", …)` 无条件装在 `ServerChannelInitializer.java:55-56`，字段在 :26-27）；**不过滤任何 opcode**，且带 `<TEXT>` 按账号 charset 解码的正文栏（`HexTool.java:73-84`）。退化行两侧不同：入站阈值 3000、模板 `"{}..."` 无空格（`InPacketLogger.java:17,40-41`）；出站阈值 50000、模板 `"{} ..."` 有空格（`OutPacketLogger.java:18,41-42`） | 两条路：① 经 dbx 改 `game_config` 单行 + **档 D 重启**（单例只在启动时读库）；② REST `POST /config/v1/updateConfig`（落库+刷内存，**免重启**，但要后台鉴权、会写 `lang_resources`） | **①已授权**（那一行 UPDATE 你已点头，用完改回 `false`，前后各存只读快照；重启前报备）；② 未走（需后台凭据） |
| C | **协议探针**：`tools/probe/s9_probe.py`（pycryptodome 3.23.0 已装）直接连 8484 完成 handshake→IV→AES-OFB→custom-enc，任意 opcode 收发。**T8 已跑通并升级为三档判据**：①握手逐字节；②s2c 能解（PING）；③**c2s 派发用 PONG ack oracle 裁决**（回 PONG 后活过 15s `lastPong` 复查 = 真被处理），`--negative-control` 已证伪"连接存活=被接受"（F6） | 服务端在跑即可；`--dry-run` 先验门禁没漂 | **已可用** |
| D | **重启服务端** | 复制 IDE 命令行：`C:\Users\11036\.jdks\ms-21.0.12.1\bin\java.exe -cp target\classes;E:\repository\… org.gms.ServerApplication`，**工作目录必须 `gms-server`**；或用后台 REST `/server/v1/restartServer`（8686，JWT） | **已授权**（"需要重启就重启"），但首次执行前我会先报备 |
| E | 改 `log4j2.xml` 调日志级别 | `monitorInterval="300"` → **不需要重启**，5 分钟内自动生效 | 已含在授权范围内 |
| **F** | **逐角色入站全 hex**：游戏内 `!monitor <角色名或角色id>`（gm3，**参数必填**：`MonitorCommand.java:43-45` 无参时只回用法、不 toggle；可用 `!monitors` 当场自检）→ `MonitoredChrLogger.toggleMonitored(chrId)` 把角色 ID 加进内存 `HashSet`，`Client.java:223` 对每个入站包 `log.info("{account}-{char} {十进制opcode}-{HEX}")`。**给原始字节而非档 A 的 `toString()`**（后者把字符串字段变成 `<TEXT>` 并插 `_`）⇒ 字段级对账首选 | 零：不读 `GameConfig`、不写库、不重启。限制：① 只有入站；② **打印点在 `Client.java:220` 的 `handler != null && validateState()` 之内** ⇒ 未注册 opcode 与被状态校验拒掉的包一条都不打（档 A 在门外，能看到这部分）；③ 屏蔽 `MOVE_PLAYER`/`GENERAL_CHAT`/`TAKE_DAMAGE`/`MOVE_PET`/`MOVE_LIFE`/`NPC_ACTION`/`FACE_EXPRESSION`（`MonitoredChrLogger.java:84`，与档 A 的 7 项**互补**，但 `MOVE_PLAYER`/`MOVE_PET`/`MOVE_LIFE`/`NPC_ACTION` **两档共同失明**）；④ hex **不含 opcode**（`getBytes()` 从 readerIndex 起算），解析时按十进制字段补回小端前缀 | **未验证 → 会话 #4 改为"档位有效性已证、数据仍为零"**：`!monitor Admin` 确实生效（服务端 `MonitorCommand` @22:52:34.987，客户端收到 yellow tip `1 已被监控`，chrId=1 进监控集），但挂上到客户端崩溃（22:52:42）之间入站只有 `MOVE_PLAYER(41)`/`GENERAL_CHAT(49)`——正落在屏蔽表里 ⇒ `Admin-Admin` 行全会话 **0 条**（`evidence/crash/session4-uievent-oob.txt` §5）。管线侧 `tools/pair_logs.py` 已支持该行式（`notes/log-pairing.md` §3.4）；采证动线见 PLAN §8 会话 #4 末段 |
| — | 手写 INSERT/UPDATE/DELETE/DDL | — | **仍然禁止**（你选的仍是"仅玩法自然写入"） |

判据因此升级为**四方交叉**：客户端 `packet_dump.log`（`Session.cpp:34-54`）↔ 服务端 `logs/out.log` ↔ Python oracle ↔ Python 探针实测。任一方与其余三方不一致就是缺陷，而不是"以某一方为准"。**探针这一方还有一条硬纪律（F6）**：判断"包是否被服务端处理"只能用 PONG ack oracle 或服务端日志，禁止用"连接有没有断"——s9 对错头不断线、不记日志、且之后不再处理任何入站包。

客户端**没有测试框架**（repo 内除 SDL/stb 头文件外无 gtest/catch2/doctest）→ 断言放 Python（oracle + 探针），不往客户端工程塞测试目标。

`mvn` 不在 PATH（`D:/software/apache-maven-3.9.16`）、PATH 上的 `java` 是 1.8：只有当需要重编服务端时才处理，本计划默认不重编。

---

## 4. 阶段与任务分解

### P0 基线层（先做，产出工具 + 真缺陷修复）

| ID | 任务 | 产出 | 验收 |
|---|---|---|---|
| T1 | **opcode 矩阵生成器** `tools/gen_opcode_matrix.py` ✅ 完成 | `protocol/opcode-matrix.json`：每行 client 名/值/行号 ↔ s9 名/值/行号 ↔ **生效** handler ↔ `status` ↔ `client_sent`/`client_sent_at` ↔ 注解一致性；`_meta` 含 s9 git head、5 个源文件 sha256、撞值表、被覆盖注册表 | 实测：c2s `MATCHED 99 / CLIENT_ONLY 4`（无死枚举，103/103 有构造点），s2c `HANDLED 297 / CLIENT_DUP 2 / UNHANDLED_EMITTED 3 / DEFINED_NOT_EMITTED 9`；s9 173 个生效 handler / 174 个注册点 |
| T2 | **差异归因报告** ✅ 完成 | `notes/opcode-diff.md`：4 条 CLIENT_ONLY 逐条归因（3 条发侧抄了 s2c 值 + 1 条 Nexon 渠道无对应），2 条客户端撞值的**实际生效方**与后果，3 条已发未接 / 9 条定义未发，5 条 s9 侧缺陷，7 项修复清单带验收 | 每条都有 path:line 与实例化点，**无 unknown**；生成器本轮 5 个解析缺陷已单列 §6 修正记录 |
| T3 | **门禁接入** ✅ 完成（**实现偏离原计划，见下**） | 未新建 `check_opcode_matrix.ps1`：门禁做成 `gen_opcode_matrix.py --check`（同一份解析逻辑，避免两套真相），并接在 `build_smoke.bat` 的 **MSBuild 之前**（事实矛盾 rc=1 → `EXITCODE=2` 不建；矩阵未生成 rc=2 / 无 python → 提示后放行） | 实测三态：一致→`GATE OK`；改 `status`→`GATE FAIL`+rc2 且 MSBuild 未执行；只改行号→`GATE STALE` 放行。偏离理由：门禁拦在构建前才不浪费一次构建；单一解析源防止 ps1/python 两套正则漂移 |
| T4 | **传输层逐字节对拍** ✅ 完成 | `tools/transport_oracle.py`（s9 / 客户端 / pycryptodome **三方独立**转写，62 个长度、2 万步 IV 链、300 包连续会话）→ `notes/transport-vectors.md` + `evidence/vectors/transport-vectors.json`（含 3 份源文件 sha256 指纹） | 实测 **20 项：19 全等 / 1 差异**。全等项覆盖 PLAN 点名的全部条目（静态 key 扩展、`multiplyBytes` 展开、`funnyBytes` 6 遍、OFB 块切换、`updateIv`、`getPacketHeader`/`isValidHeader`、`0xFFFF-83` vs `83`）；**唯一差异 = 新缺陷 D8**（入站长度符号扩展）。做法偏离：不是"客户端 vs oracle"两方，而是**两侧各自转写 + JCE 参照**三方，防止 PLAN §7 的"错得一样"自欺 |
| T5 | **修 D2 + D8** ✅ 完成 | ①[Net/Cryptography.cpp:89](file:///E:/project/MapleStory-Client/Net/Cryptography.cpp#L89) `int16_t`→`uint16_t`；②[Net/Session.cpp:115-126](file:///E:/project/MapleStory-Client/Net/Session.cpp#L115) 长度守卫（坏头 = 主动断线，把 F6 的"静默聋 + 越界写"变成可观测掉线）；③[Net/OutPacket.cpp:77-86](file:///E:/project/MapleStory-Client/Net/OutPacket.cpp#L77) `write_long` 循环上界 `sizeof(long)`→`sizeof(int64_t)`。**oracle 同步**：`cl_check_length` 默认改为无符号（对齐修好的 C++），`sign_extend=True` 保留做负向对照，并新增"覆盖性"断言防止用例没进 `32768..65535` 导致空跑 | 构建 `GATE OK`+`EXITCODE=0`；**oracle 退出码 1 → 0（20 项全绿）**；新快照 `evidence/vectors/transport-vectors-t5post.json`（`Cryptography.cpp` 指纹 `d1d2d755…`→`d26fff56…`），**不覆盖** T4 快照。**D2 未被对拍证明**（oracle 只建模传输链，且 `write_long` 全仓零调用点）⇒ 验收推迟到第一个 `long` 字段出现时由 T9 配对抓 |
| T6 | **修 D1 + F5** ✅ 代码完成，实机验收未做 | ①D1：注销 [Net/PacketSwitch.cpp:726](file:///E:/project/MapleStory-Client/Net/PacketSwitch.cpp#L726) 的 `emplace<RECALCULATE_STATS,…>`，35 归 `ForcedStatResetHandler`（枚举项保留作 v83 命名参考，附 `SendOpcode.java:62` 证据——s9 在 35 上没有 `RECALCULATE_STATS`）；②F5 三条改值 [OutPacket.h:138](file:///E:/project/MapleStory-Client/Net/OutPacket.h#L138) `STORAGE 309→62`、`:166` `PLAYER_INTERACTION 314→123`、`:173` `CASHSHOP_OPERATION 325→229`，各自上方 `:136/:164/:171` 附 `RecvOpcode.java:78/:131/:199` 证据 | 矩阵数字即预期证明：`c2s MATCHED 99→102`、client `handlers 303→302`、`s2c CLIENT_DUP_VALUE 2→1`（剩 152 那条按 §4 第 6 项**故意不改**）、`stale client s9-assertion comments: 0`。**三条 F5 的实机验收（开仓库/互动/商城各点一次 + 服务端 handler 命中）与 body 级对齐一起挂在 P2 的 S6/S7/S8** |
| T7 | 真机取证：抓一次真实 handshake 16 字节 + 登录前后若干包，存 evidence（脱敏，不记明文密钥）。 | **频道侧已补齐**：`evidence/probe/probe-channel.{txt,hex,json}`（目标 `127.0.0.1:7575`，16 checks / 0 failed，hello 逐字节 `0e 00 53 00 01 00 31 46 72 7a ** 52 30 78 ** 08`，两轮 PING→PONG 活过 20s 复查窗口）⇒ 登录服与频道服共用 `GMSV83PacketProtocol.java:75` 的同一份握手代码，两侧都有实测而非推断。**真机会话 #1 已采集（2026-10-30 20:27–20:29）**：`notes/real-session-1.md` + `evidence/pairing/client-packet_dump-20261030-2029.redacted.log`（57 行原始、剥 1 行登录请求后 56 行进仓库），`pair_logs` rc=0 / 56 包解析 / `unparsed=0` | **已闭合**：① 传输层真机走通（登录→选角→进频道→进图 617B `SET_FIELD`→4 次移动→退出，44 s2c 全解全派、13 c2s 全受理，无 F6 症状）；② **结构事实**：hello 无正文 ⇒ `packet_dump.log` 永远取不到握手，改以"会话走通 + 探针 hello"为凭；③ **T6/D1 拿到真机验收**：RECV `0x0023 FORCED_STAT_RESET` 确在进图序列里。**仍缺**：跨向配对（`pairs=0`，服务端侧零日志）——根因不是封包而是**命令前缀**：s9 用 `@`/`!` 而非 `/`（`CommandsExecutor.java:53-54`），`/showpackets` 落进 `GeneralChatHandler.java:53-55` 的两个分支之外被静默丢弃 ⇒ 正确输入是 **`!showpackets`**；档 A 当前状态已用探针判定为**关**（`probe-t7-state3`，负结论由 PONG oracle 先证明派发点被走到才算数）。**下一轮改用档 F（`!monitor`，见 §3 能力表）**：同样免重启，但给入站**原始 hex**而不是档 A 的 `toString()`，是字段级对账的正解；`tools/pair_logs.py` 已支持该行式。**会话 #3（22:30）已把跨向配对从 0 推到通**：档 A 出 7 条入站记录，与客户端 `[SEND]` 配对 **7 EXACT / 0 DIFF / rc=0**（`evidence/pairing/report-20261030-2232-paired.md`，`notes/log-pairing.md` §8）；档 F 仍空转——`!monitor` 缺角色名参数，服务端只回了用法（`MonitorCommand.java:43-45`）。**会话 #4（22:52）把 `!monitor Admin` 挂上了（chrId=1）却仍零产出**：窗口内入站只有被屏蔽的 41/49，且客户端 8 秒后崩在 `UIEvent` 越界（该崩同时暴露一处 **s2c 语义错位**：`0x004D` 被 s9 复用为 yellow tip），详见 §8 会话 #4 与 `notes/crash-chatbar.md` §8。**会话 #5（2026-10-31 00:55）拿到档 F 首份真数据并完成首个字段级配对**：`!monitor Admin` + `!monitors` 双双生效且不崩（任务 #26 验证点 A/B 通过），4 条非屏蔽入站（`/w` `/find` `/g` `/p`）在客户端 `[SEND]` 与档 F hex 之间 **4/4 逐字节 EXACT**，整窗口 `pairs=11 EXACT / DIFF=0 / len_mismatch=0 / um_server=0`（`evidence/pairing/report-20261031-0055-monitor-f1.md`，详解 `notes/log-pairing.md` §9）；s2c 侧无出站档位 ⇒ 改用"真机字节 ↔ s9 源码"对 `0x4D`（`sendYellowTip` `PacketCreator.java:5437-5443`）与 `0x87` 三种形状（`getWhisperReceive/getWhisperResult/getFindResult`，`:5776-5807`）逐字段闭合；`0x18/0x28/0x3C` 三个遗留未配对项**全部归因完毕**（§9.3：`0x18` 正面 EXACT×7，`0x28`/`0x3C` 是档 A 当时被 `!showpackets` 翻回 OFF 且不在 7 项过滤名单 ⇒ 无档位可记，二者字段级预对账已完成且无错位）；顺带钉死两条**新事实**：PING 是 Netty 空闲事件不是 30s cron（`Client.java:1182-1185`，解释心跳缺口），以及**服务端确实发了 GBK 中文**（`d2 d1 b1 bb bc e0 bf d8`=`已被监控`）而客户端解码层未丢、丢在"渲染层按 UTF-8 解码 GBK 字节"这一步 ⇒ **CJK 渲染通路已被真机反证为可用**（同屏 `对所有人` 正常显示，SimHei + `GraphicsGL.cpp:1299-1328` UTF-8 解码），**唯一缺口收敛到 #21 T11 的封包层 GBK↔UTF-8**（详见 §9.5，含我中途"归 T12"的错判修正）。⚠ 该轮还修了配对器一处真实缺陷：纯贪心会把本该 EXACT 的内容包越过指针误判成未配对，已加前瞻护栏（`tools/pair_logs.py:296-326`，`--self-test` 全绿 + 会话 #3 那份 834×7 窗口逐字回归一致） |

| **T8** | **协议探针** ✅ 完成 `tools/probe/s9_probe.py`：connect→读 16B hello→取 IV→custom/AES 双向编解码→发任意 opcode→解回包，全部复用 T4 的 oracle（探针内不重写编解码）。pycryptodome 3.23.0 | `notes/probe.md` + 门禁清单 `tools/probe/opcode_{blocklist,allowlist}.txt` + 每次会话 `evidence/probe/probe-*.txt/hex/json` | 实测 22 项门禁断言全绿：**hello 与 oracle 逐字节一致**；服务端 PING 被我们正确解出（s2c 打通）；**回 PONG 后活过 `checkIfIdle` 的 15s 复查窗口 ⇒ c2s 被解密+派发+执行到 handler**；包长 2..40000（含 D8 临界 32768）扫完仍能派发 ⇒ 服务端长度公式覆盖 16 位全域，**D8 确认为纯客户端缺陷**。附带产出 **F6**：坏头不断线且永久毒化入站 ⇒ "连接存活"从此不作为接受证据 |
| **T9** | **服务端日志取证管线** ✅ 工具完成：`tools/pair_logs.py` 解析五种格式（客户端 `[SEND]/[RECV] PacketDump :`、档 B `ClientSend/ServerSend`、档 A `收到封包 ByteBufInPacket[...]`、档 F `account-char 十进制opcode-HEX`、超阈值退化行），按方向做前瞻窗口贪心逐包配对，输出 EXACT/DIFF（带首差异字节偏移）/UNMATCHED 与 `declared_len` 长度违规；为此给 [Net/Session.cpp:36-56](file:///E:/project/MapleStory-Client/Net/Session.cpp#L36) 的 `packet_dump` 加了方向标记（`Session.cpp:132` RECV / `:166` SEND），构建 `GATE OK`+`EXITCODE=0` | `tools/pair_logs.py` + `notes/log-pairing.md` | **`--self-test` 全绿（35 处断言调用，rc=0）**：五种格式各造样例行、方向映射、小端 opcode、档 A 的 `_` 剥离（reader_pos=2）、档 F 的 opcode 前缀补回（其 hex 不含 opcode）、`<UnknownPacket>`、**两种截断写法**（入站无空格/出站有空格）、A/B/F 三方镜像去重（含"相邻两条档 F 不得互删"负向用例）、非封包行不静默丢、EXACT/DIFF/UNMATCHED 三路径、退出码两态。**真数据那一步（"配对成功率 100%、配不上的每条都解释"）挂在 T7 下一轮** |
| T9b | 双向日志 `use_debug_show_packet`（档 B）：**已授权**经 dbx 改这一行（前后只读快照，用完改回 `false`）。**本轮新增约束：单改库不生效，必须配档 D 重启**；免重启的替代是 REST `updateConfig`，但要后台鉴权且顺带写 `lang_resources` | 同 T9 | 需要出站方向或档 A 那 7 个 opcode 时才开；**重启前报备**。dbx 工具**已可见**（T10 起，见 `notes/charset.md` §1），改前基线已存 `evidence/dbx-charset-snapshot.txt` 第 [4] 条（`id=224 … false`）；**那一行 UPDATE 至今未执行**——档 A 免重启已够 T7 首轮用 |

**P0 不做任何功能改动**，只建表、建门禁、建探针与配对管线、修已定位缺陷（D1 撞值、D2 `write_long`、D8 入站长度符号扩展）与 F5 的 3 条 c2s 改值。

### P1 GBK 链路（含两个 spike，可能失败，失败要显式预留）

| ID | 任务 | 说明 |
|---|---|---|
| T10 | **钉死字符集** ⚠ 静态+库侧完成，实机判别留 S 切片：`notes/charset.md`。dbx 只读快照 = **3 个账号 `language` 全 = 3（GBK），无需换号**；服务端 4 个字符串调用点（`ByteBufInPacket:56`、`ByteBufOutPacket:79/91`、`HexTool:83`）一律走 `ThreadLocalUtil.getClientLang()`，其值只由库决定（`Client.java:664,682` 登录 SELECT、`Character.java:6770` 进图 set），**没有任何 handler 从包头读 language** | **原计划的"探针分别以 GBK/ASCII 发同一串中文"被我自己的发现否掉了**（charset.md §3）：`setCurrentClient` 只在 `Client.java:222` 的 `handler != null && validateState` 分支内绑定，而两档日志都在绑定**之前**打印 ⇒ `<TEXT>` 恒按 `SERVICE_LANGUAGE`（本部署 `application.yml:52 zh-CN` = GBK）解码，**证明不了账号语言参与**。加上 `SERVICE_LANGUAGE` 恰好也等于 GBK，本部署**两条路结果相同、无法区分**；可执行的判别实验只剩"看 handler 内的回显"，归 S 切片。附带登记 4 条 s9 侧缺陷（charset.md §4），其中 `LanguageConstants.java:110` 用 `language` 直接索引 `String[4]` ⇒ 值 ≥4 会 AIOOBE |
| T11 | **转换接缝 ✅ 已落地**（会话 #6）`Net/PacketCharset.{h,cpp}`：`packet_to_internal`（线上 GBK→内部 UTF-8）/ `packet_from_internal`（反向），Win32 `MultiByteToWideChar`/`WideCharToMultiByte` + `CP 936`，非 Windows 编译成不转换。接入两处即覆盖全仓：**入站 175 个 `read_string`** 走 [Net/InPacket.cpp:88](file:///E:/project/MapleStory-Client/Net/InPacket.cpp#L88) `read_padded_string`，**出站 74 个 `write_string`** 走 [Net/OutPacket.cpp:105](file:///E:/project/MapleStory-Client/Net/OutPacket.cpp#L105)（**先转码再按转码后的字节数写 short 前缀**，正面解掉 D 层的长度陷阱）。内部表示是 UTF-8 这一条除真机反证外还有构建口径背书：`CMakeLists.txt:115-117` + `MapleStory.vcxproj` 六处 `/utf-8`，737/737 个非 ASCII 源字面量按 UTF-8 解码通过 ⇒ **不存在双重转换**（`notes/charset.md` §5.2 那条"字面量是 GBK"的旧结论已更正） | **离线对拍 73 条断言 failures=0**（`tools/probe/gen_charset_vectors.py` 用 Python `cp936`/`gbk` codec 现算期望值 → `charset_vectors.h`；`tools/probe/charset_check.cpp` 独立编译跑 `Net/PacketCharset.cpp`；证据 `evidence/charset/t11-oracle-20261031.md` + `oracle-20261031-raw.txt`）。交叉验证：向量 5 的期望 GBK = `d2 d1 b1 bb bc e0 bf d8`，与 §9.5 真机入站字节逐字节相同。三条安全阀：纯 ASCII 恒等（既有调用点绝大多数零回归）、入站任一步失败即原样、出站用 `MB_ERR_INVALID_CHARS` 严格校验非法 UTF-8 也原样。**全量构建通过**（临时 OutDir，客户端在跑不能覆盖）。✅ **真机已复测**（会话 #7，`e1394e5137e498a1` 部署后）：`!monitor Admin` ⇒ 屏幕 `[Notice] 1 已被监控` + `Admin 正在监控 1`，与 `packet_dump.log` L59/L60 的 `0a 00`=10 / `10 00`=16 逐字段对上；同一命令出站改动前后逐字节一致 ⇒ ASCII 快路径无回归；客户端与服务端源码各 0 处含这些字面量 ⇒ 排除假阳性。全窗口原帧 + 前后对比 + 服务端 `MonitorCommand.java:55-58` / `message_zh_CN.properties:424-427` / `PacketCreator.java:5437-5443`、`:1292-1298` 链路见 `evidence/charset/t11-live-20261031.md`。⚠ 现场遗留：服务端 `Admin` 监控当前为 **ON**（`!monitor Admin` 可关）。输入侧按"先只管显示，输入预留"未动 |
| T12 | ✅ **度量层已闭合**（会话 #8，任务 #28）。~~渲染 spike：FreeType 图集扩到 CJK~~ ⇒ 会话 #5 真机反证"字形通路已可用"，会话 #6 复核判定**不能算关闭**（度量仍 ASCII-only），会话 #8 修完度量并读出像素差 | 当时的两处证据（行号为修复前）：①`GraphicsGL.cpp:1142-1149` 的 `wordwidth` 只对 `32..127` 累加 ⇒ 纯中文串永不触发 `:1151` 的过长拆分 ⇒ **长中文 NPC 文本只溢出不换行**；②`:1184-1198` 把每个非 ASCII 字节按 `font.chars[' ']`（`:1187`）计推进量 ⇒ 一个汉字（3 字节）记成 3 个空格宽、实际只画 1 个字形 ⇒ **`layout.width` 与中文之后各 word 的起点双双偏大**（此处原写"偏小"，会话 #8 依实测修正：度量偏大 ⇒ 居中算式 `(81−w)/2` 的内缩偏小 ⇒ 文字被顶到框左沿，正是实测到的 −13.5 px 偏心）。绘制侧一直用真字形推进量，所以短串看不出来，会话 #5 的"已可用"只证到字形那一层。**修法**：新增 `Graphics/Utf8.h::ms::utf8_next`，度量侧 `LayoutBuilder::measure`/`wide_advance`（现 `:1224`/`:1236`）与绘制侧 `drawtext`（现 `:1354-1359`）同调一个解码器 ⇒ 不变式变成结构性的；`advances` 保持按字节下标（`Textfield` 光标仍可用），同一码点各字节共享其起笔。**门禁**：`tools/probe/utf8_metrics_check.cpp` 26 条解码向量 failures=0 + 3570 次 `FT_LOAD_DEFAULT` vs `FT_LOAD_RENDER` advance 比对 0 反例（度量侧在图集 `locked` 时也要能算，这条相等性是承重假设）+ 6 条 `lax:` 向量钉住宽松解码的既有行为。**真机 A/B**：`UIChatBar.cpp:301-305` 的 `对所有人` 芯片按 `width()` 居中，修复前后同屏同贴图实测左内缩 3→16 px、偏心 −13.5→0.0 px、**字形实宽 41→42 px 不变**（预测位移 14 px vs 实测 13 px）；英文同屏无回归。证据 `evidence/render/t12b-live-20261031.md` + `t12b-chip-ab-6x.png`。**仍缺真机帧**（同源同算式、本轮未观测）：长中文换行、`ChatBalloon` 气泡拉伸、`NAMETAG` 名牌背景、`Textfield` 中文光标（属 T13） |
| T13 | 输入侧能力评估：客户端能否经 IME 输入中文 | 若不能，"发中文"是闭环还是预留 —— §8 待决 |

### P2 字段级 body 对账（11 个切片，按游戏流程，你选的顺序）

每片固定交付物（模板统一，便于机器复核）：

```
notes/sN-<slice>.md            # 包清单表：方向 | client opcode | s9 opcode | handler
                               # 逐字段：client 读写序列 vs s9 ByteBuf 写读序列 | 结论 | 状态
evidence/sN/packet_dump.txt    # 客户端出站/入站 hex（脱敏）
evidence/sN/out_log_pair.txt   # 服务端 logs/out.log 对应 recv 行（档 A 开启后），逐包配对结果
evidence/sN/probe_*.hex        # 该片用探针预演过的包与服务端反应
patches/sN.diff                # 若有修复
opcode-matrix.json             # 回写该片相关行状态
```

**每片先探针、后客户端**：先用 `s9_probe.py` 按 s9 源码规则拼包发出去，确认服务端接受并回什么；再改客户端发同一包，比对两侧字节与服务端反应。这样"字段顺序对不对"由服务端当场裁决，不靠我读代码推断，也不每次启动游戏客户端。

| 片 | 覆盖范围 | 已知关联 |
|---|---|---|
| S1 | 登录：hello → `LOGIN_PASSWORD`/`LOGIN_EMAIL` → `LOGIN_STATUS`/`ACCOUNT_INFO`/`SERVERLIST` | 依赖 T10（字符集）、已联调链路 |
| S2 | 选角/建删号/`VIEW_ALL_CHAR`/PIC/PIN（s9 默认 `enable_pic=false`） | 已实现，回归风险最高 |
| S3 | 进图：`PLAYER_LOGGEDIN`、field set、`SERVER_IP`/`CHANNEL_SELECTED` | 已实现 |
| S4 | 属性/统计/Buff/HP-MP：`STAT_CHANGED`/`GIVE_BUFF`/`CANCEL_BUFF`、扩展通道 `0x1000` HPMPALERT | 客户端 `extended_handlers`（`PacketSwitch.h:60,88-99`）与 s9 `SET_HPMPALERT(0x1000)` 配对，需确认语义 |
| S5 | 移动/攻击/伤害/复活 | 抽样已核：客户端 `MOVE_MONSTER=188` ↔ s9 `RecvOpcode.java:178` `MOVE_LIFE = 0xBC`（=188）**吻合**；该片其余包仍需逐条查是客户端错还是版本差 |
| S6 | 物品：背包/掉落/使用/卷轴/仓库/商店/交易 | `replace_8c2870` 等 exe 证据只在 s9 缺字段时回指 |
| S7 | 技能/技能宏/技能面板 | |
| S8 | NPC/脚本对话/传送门/反应器 | 脚本是 GraalVM JS，客户端只管对话协议 |
| S9 | 社交：whisper/messenger/party/partysearch/guild/alliance/BBS/memo/family | 近期已复刻 alliance/maker/monsterbook，优先复核 |
| S10 | 商城/MTS/Fredrick/Duey/Storage/个人商店 | 含 `0x37xx` BeiDou 扩展通道与 `DPT_TRACKER(0x3714)` |
| S11 | 杂项：RPS/quest/新年卡/maplelife/hammer/admin/maker/monsterbook | |

**每片结束都跑一次登录→进图回归**，防止把已联调链路改坏。发现"客户端能跑但与 s9 不一致"时按你的授权：**允许改，逐条给证据**（s9 `path:line` + 改前改后字节样例 + 可能回归项）。

### P3 缺失系统

| ID | 任务 |
|---|---|
| T14 | 归因清单：对每个"服务端有 handler、客户端从不发包"的 opcode，判定 (a) 客户端有无对应 UI/逻辑 (b) 服务端该包是否有真实玩法 (c) 优先级 → 分 `可实现` / `需先补 UI` / `明确不做（附理由）` |
| T14b | **第三类缺陷（会话 #4 新发现，必须单列）**：opcode 名、值、"有无 handler"三项都对，但**服务端把该包复用成另一种消息**，客户端仍按参考端语义实现 ⇒ 对照表门禁完全无感。首例：`SET_WEEK_EVENT_MESSAGE(77)` 在 s9 是 yellow tip（`PacketCreator.java:5437-5443`），我们当周活动列表窗口，直接崩（`notes/crash-chatbar.md` §8）。排查法：对每个 s2c opcode 在 s9 侧 grep **全部 `OutPacket.create(SendOpcode.X)` 调用点与 `PacketCreator` 方法名**，与客户端 handler 的解读逐字段对；仅比 opcode 名不够 |
| T15+ | 按 §4-P2 同序分批实现，每批一次原子提交 |

P3 的实现量级是**功能开发**，与协议对接不同量级；T14 出完清单后再定批次，不预先承诺全部实现。归因时探针可白捡一个副产品：对每个"客户端从不发"的 opcode 用探针发一次，看服务端是忽略、报错还是回包——这直接判定该包在 s9 里是否有真实玩法，比读 handler 代码硬。

---

## 5. 影响面预估

| 层 | 文件 |
|---|---|
| 工具 | `tools/gen_opcode_matrix.py`(新)、`tools/check_opcode_matrix.ps1`(新)、`tools/oracle/*.py`(新)、`tools/probe/s9_probe.py`(新)、`tools/probe/opcode_blocklist.txt`(新)、`tools/pair_logs.py`(新) |
| 缺陷 | `Net/OutPacket.cpp`(write_long)、`Net/PacketSwitch.cpp`(35/152) |
| 字符集 | `Net/InPacket.{h,cpp}`、`Net/OutPacket.{h,cpp}`、新增 `IO/Charset.*`；渲染 `IO/Components/Textfield.cpp` |
| 切片 | `Net/Packets/*.h`(37)、`Net/Handlers/*Handlers.{h,cpp}`(25×2)、`Net/OutPacket.h` 枚举 |
| 文档 | `docs/features/s9-protocol-align/`（本目录）、`docs/features/opcode-align-beidou-gms083/README.md` 加 superseded 头 |

不改：`MapleStory.sln` 之外的构建结构、`Data/*.img`、服务端任何文件。

---

## 6. 产物目录

```
docs/features/s9-protocol-align/
  PLAN.md                     # 本文件
  README.md                   # 收尾时写「最终实现逻辑」
  protocol/opcode-matrix.json # 机器可读对照表（T1 生成）
  notes/                      # opcode-diff / transport-vectors / probe.md / log-pairing.md / charset.md /
                              # client-path.md（运行期客户端目录事实：OutDir 即部署目录、配置编码与 EOL、
                              # debug.packet_dump 开关改动与撤销）/ real-session-1.md（真机会话 #1 取证与归因）/
                              # crash-chatbar.md（会话 #2 / #4 两次闪退取证：WER 偏移 → PDB → 源码行；
                              #   §1-§5 classic HUD 空指针修复，§8 Event List 越界 + 0x004D 语义错位修复）/
                              # sN-<slice>.md / gap-inventory.md
  evidence/                   # probe/（探针 txt+hex+json、netty 反汇编、out.log 摘录；含 probe-channel.* = 频道端口
                              # 7575 的握手与 PONG ack 取证）、vectors/（oracle 对拍：T4 快照 + t5post 复跑快照）、
                              # pairing/（pair_logs 报告 + 原始两份日志摘录）、
                              # crash/（wer-maplestory.txt 签名普查、wer-event-1000-raw.txt 事件正文含"出错模块名称"、
                              #   session4-uievent-oob.txt 崩溃会话时间线抄件——符号随 PDB 换代已不可复跑，以此件为准）、
                              # dbx-charset-snapshot.txt（字符集盘点的库侧只读快照，T9b 改前的 game_config 基线）
  patches/                    # 每片 diff
  builds/                     # build_smoke EXITCODE 记录 + 产物 hash
```
密钥不进文档：只记"由 `VERSION=83` 派生"这类名称与推导式，不写静态 key 明文。

---

## 7. 风险与回滚

| 风险 | 缓解 |
|---|---|
| 改字段顺序弄坏已联调链路（登录/进图最敏感） | 逐条证据 + 每片回归 + **一片一原子 commit**，可单点 `git revert`；未授权不 push |
| Python oracle 与 C++ 实现"错得一样"从而对拍自欺 | **T4 已按此执行并升级**：oracle 不是"只按 s9 写"，而是 **s9 侧 + 客户端侧各自独立转写 + pycryptodome(=JCE AES-256/ECB) 第三方参照**，三方向任一不一致即定位到具体一侧（本轮就靠它抓到我自己的 `gmul` 转写错与真缺陷 D8）；T8 探针（直接 `import` oracle 的 `s9_*` 函数，避免两份实现漂移）能否被服务端接受并正确回包作第四方裁决；F1/T7 真机 hex 作第五方 |
| CJK 图集 spike 失败拖住 P2 | P1 与 P2 解耦：先做不含中文字段的切片，中文相关包标 `BLOCKED(T12)` |
| 字符集判断错，把"字符集吞字"误诊成"字段偏移错" | T10 已做（`notes/charset.md`）。**原写法"每条字符串字段结论必须附真机 hex + 服务端 `out.log` 解码文本"不够格**：档 B 的 `<TEXT>` 在 `setCurrentClient` 之前解码，恒用 `SERVICE_LANGUAGE`（charset.md §3），所以它只能证明"这串字节按 GBK 可读"，不能证明账号语言参与。判据升级为：**要么给出该包所属 handler 已绑定 Client 的证据，要么显式标"此处 charset 未定盘"** |
| **探针发任意包会触发真实 handler 副作用**（探针不是只读的：handler 会改地图、存档，甚至删号） | 只在**一次性测试号 + 测试角色**上探针；先建 `tools/probe/opcode_blocklist.txt`，破坏性/持久化 opcode（删号、仓库、交易、商城扣款、公会/家族写操作）默认拉黑，需你逐条放行才发。**T8 已落地并实测**：三层闸门（无 handler / blocklist 否决 / allowlist 唯一豁免）+ 清单值现场对源码复核（不符 ABORT rc=99）+ 只连 login port 8484（不登录、不建 session）⇒ 本计划内探针不需要测试角色即可安全跑；negative control 亦只打坏**头部**（走不到派发），绝不打坏正文 |
| 重启服务端打断你的 IDE 会话 | 已获授权但**首次执行前报备**；先记录 PID 40688 的完整命令行与工作目录，优先试 REST `/server/v1/restartServer`（只重启游戏服、不动 JVM），重启后校验 8484/8686 与 `logs/out.log` 恢复写入 |
| 双向日志 `use_debug_show_packet` 开启代价比预想大 | **T9 更正**：`GameConfig` 只在启动时读一次库 ⇒ 光改库无效，必须配重启。已获授权走"dbx 单行 UPDATE + 重启（重启前报备）"；未开时用档 A（内存即时、被过滤 7 个 opcode）+ 档 C 探针自解码回包顶替。另：`logs/` 有 `IfLastModified age="1d"` 自动清理（`log4j2.xml:32-37`），采到的日志必须当轮拷进 `evidence/pairing/` |
| 工具链缺失（Maven 不在 PATH、PATH 里 java 是 1.8） | 本次不需要构建服务端；客户端只用 `tools/build_smoke.bat`（VS2019 v142，x64 Release，OutDir `E:\MXD\BeiDou-Client-xy\`） |

---

## 8. 已定与待决

**本轮已拍板，写入计划不再问**

| 事项 | 结论 |
|---|---|
| 范围 | 四层全做：基线 + 字段级 body + GBK + 缺失系统 |
| 对照表 | 脚本生成 + 纳入门禁（T1/T3） |
| 切片顺序 | 按游戏流程，先保已联调（S1→S11） |
| 改动授权 | 允许改已联调链路，**逐条给 s9 `path:line` 证据** |
| 库 | dbx 只读；`dbx-mcp.exe` 已在 `C:\Users\11036\.dbx\bin\`，**由我注册进 Qoder 的 MCP 设置**（规范原文写 `~/.cursor/mcp.json`，本环境是 Qoder Desktop，按实际工具走）；建议你在 dbx 侧把 allowlist 设只读 |
| 写入 | 仅玩法自然写入；我绝不手写 SQL |
| 服务端 | 需要时可重启（首次报备）；日志配置改动靠 `monitorInterval=300` 免重启 |
| 字符集测试号 | 先只读查 `accounts.language`；若为 2（US-ASCII）则换号，不改库 |

**开工时确认的最后 2 点（2026-10-30，用户拍板）**

1. **档 B 已获限定授权**：用户明确授权我经 dbx 对 `game_config` 中 `use_debug_show_packet` **这一行**执行 UPDATE 以开启服务端双向包日志。范围仅限该 key：用完须改回 `false`；其它任何表的任何写操作仍未授权；执行前后各记一次只读快照到 `evidence/`。
2. **中文边界 = 先只管显示，输入预留**：本版闭合"解 GBK + 能显示中文"；若客户端无 IME 输入能力，"能发中文"写为显式预留并说明阻塞，不假装闭环。

计划已确认，开工起点：**T1 opcode 矩阵生成器**。

**T1~T3 完成留痕（2026-10-30）**

- 三项均已完成，产物：`tools/gen_opcode_matrix.py`、`tools/opcode_row_evidence.py`、
  `protocol/opcode-matrix.json`、`notes/opcode-diff.md`、`tools/build_smoke.bat`（门禁接入）。
- 顺带发现并修掉了我自己第一版解析器的 5 个缺陷（详见 `notes/opcode-diff.md` §6）——其中
  "误报 4 条服务端没 handler"和"把先注册者当成现役 handler"两个方向性错误，如果不做交叉验证
  就会直接写进结论。**教训落规范**：矩阵里凡是"谁生效"的判断，必须以覆盖写的语义（s9
  `handlers[value]=handler`、客户端 `handlers[O]=make_unique<T>`）按**最后写入**计算，不能按声明顺序猜。
- `tools/build_smoke.bat` 注释改为**纯 ASCII**：cmd.exe 用 cp936 读 .bat，UTF-8 中文 `rem` 行会被解码成
  字节粥后**当成命令执行**（实测回显一串"不是内部或外部命令"）。今后本仓库 `.bat` 一律 ASCII 注释，
  中文说明放 md/脚本里。这是「编码防乱码」门禁在 Windows 批处理上的具体形态。
- 本轮未做任何 git 提交（用户未要求）；工作区仍留有大量前会话的未提交改动，未触碰。

**T4 完成留痕（2026-10-30）**

- 产物：`tools/transport_oracle.py`（20 项对拍，退出码=差异数）、`notes/transport-vectors.md`、
  `evidence/vectors/transport-vectors.json`（含 `Cryptography.cpp` / `MapleAESOFB.java` /
  `MapleCustomEncryption.java` 三份 sha256 与逐条向量）。
- 结论：**传输层字节规则与 s9 完全一致**（含 62 长度、2 万步 IV 链、300 包连续会话、双向端到端）；
  唯一差异是**新缺陷 D8**（入站长度 `int16_t` 符号扩展），已并入 T5。
- 顺带证伪了三个"看起来像差异"的点，避免误报：客户端 `maplekey[256]` 恰是 s9 那把 32 字节密钥的
  标准 AES-256 扩展（对 v83 成立，"only works < v118" 的注释属实）；服务端 send 侧用 `0xFFFF-83`、
  客户端只用 `83` 并不矛盾（`MapleAESOFB.java:97` 换序后收侧版本低字节为 0，且客户端解长度时版本自动抵消）；
  `Cipher.getInstance("AES")` = ECB/PKCS5Padding，`arraycopy` 只取前 16 字节 ⇒ 与纯 ECB 同值（pom 无 BouncyCastle）。
- 同样靠三方交叉抓到了**我自己转写的错**：第一版把 C++ `(uint8_t)((int8_t)x >> 7)` 当成 `1`，
  实际是 `0xFF`，导致 `client-aes != JCE` 假警报。教训：转写老 C++ 里的位技巧时，
  **必须用第三方标准实现校验**，不能只跟"另一半自己写的代码"比。
- 门禁口径：oracle **不**接入 `build_smoke.bat`（它是分析工具、依赖 pycryptodome，且 D8 未修时退出码恒为 1，
  会把构建拦成红灯）。改由"源文件指纹 + 手工复跑"约束：动过 `Cryptography.*` 或 s9 加密类就复跑一次。

**T8 完成留痕（2026-10-30）**

- 产物：`tools/probe/s9_probe.py`、`tools/probe/opcode_blocklist.txt`（89 条）、`tools/probe/opcode_allowlist.txt`（1 条）、
  `notes/probe.md`、`evidence/probe/probe-*.{txt,hex,json}`、`evidence/probe/netty-ByteToMessageDecoder-channelRead.txt`、
  `evidence/probe/outlog-excerpt.txt`。探针**复用** `transport_oracle.py`，不另写一份编解码——否则"探针通过"不等于"oracle 正确"。
- **T4 的 oracle 不是自欺**：对着正在运行的 s9，hello 逐字节一致、服务端 PING 被正确解出、回 PONG 后活过 15s 复查窗口
  ⇒ 收发两侧的字节规则与线上真实实现一致。包长 2..40000 扫完派发仍正常 ⇒ **D8 定级为纯客户端缺陷**，服务端侧 16 位全域可用。
- **本轮最大产出是推翻了探针自己的判据**（登记为 F6）：坏头部既不断线也不记日志，之后所有入站包永不被处理，
  会话只能等 pong 超时被踢。原写法"连发 N 包后连接仍存活 = 每个头部都被接受"因此**不成立**，
  早前 `probe-fast/s2c/full` 三轮里"被接受"那类措辞的强度都要下调；现在的结论一律由 PONG ack oracle 或服务端日志给出。
  机制靠 netty 反汇编坐实到 `DecoderException` 重包（`Client.java:253` 的 `instanceof` 因此恒 false），
  "坏头卡在累积缓冲区头部"那一步只在 notes 里标为**推断**，不当成已证明。
- 门禁设计经受住了自查：`--dry-run` 抓到 blocklist 里的 `LOGIN_EMAIL` 根本不是 s9 `RecvOpcode` 常量（那是客户端枚举名），
  说明手抄清单必然漂移；改成"名字/值现场对源码复核，不符即 ABORT(rc=99)"，并对这类客户端专有值支持 `*` 通配（值级否决）。
- 探针的退出码只计**门禁断言**；negative control 的结论走 `measure()`，不计入 rc（推翻假设是产出，红字会掩盖真缺陷）。
- 本轮未做任何 git 提交（用户未要求）；工作区仍有前会话未提交改动，未触碰。

**T9 完成留痕（2026-10-30，工具层闭环，真数据留到 T7）**

- 产物：`tools/pair_logs.py`（四种日志格式解析 + 双向逐包配对 + 长度校验 + markdown 报告）、
  `notes/log-pairing.md`（格式规格逐字对齐源码、三档开关生效方式、判据、限制、T7 实跑步骤）。
- 为使能改动：`Net/Session.cpp:36-56` 的 `packet_dump()` 增加 `bool outbound` 形参，在保留参考端
  `"PacketDump : " + "%02x "` 格式的前提下前置 `[SEND]`/`[RECV]`；调用点 `Session.cpp:132`（解密后的入站）、
  `:166`（加密前的出站）。理由：收发混写同一文件，光靠行序无法定方向，而配对必须按方向配。
  构建复跑 `GATE OK`（opcode 矩阵与两侧源码一致）+ `EXITCODE=0`，产物 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` 已更新。
- 验收口径的偏离与理由：PLAN §4 原写"一次登录会话内配对成功率 100%"，**本轮无法达成**——需要真实会话日志，
  而那依赖 T7。因此把 T9 拆成"工具正确性（本轮，`--self-test` 24 条断言全绿）"与"真实配对（T7）"两格，
  后者仍在 T7 的验收里，不冒充已完成。
- 两处与先前记录不同的实测更正，都写进了 §3：① **档 B 不是"改库即可"**，`GameConfig.java:22-29` 只在单例构造时读库，
  改库必须配重启；② **截断行两侧格式与阈值都不同**（入站 3000/`{}...`，出站 50000/`{} ...`），
  解析器若只认一种就会把入站截断行错分进 `unparsed`。
- 仍未做的两项，都卡在同一个外部依赖：**dbx MCP 已注册（serverId `88b1adfa-…`，status ready）但本会话
  `mcp_list` 看不到它的工具**，需重载 MCP / 重启 Qoder。因此 ① `accounts.language` 只读快照、
  ② `game_config` 快照与那一行授权 UPDATE，都还没执行——**没有对库做任何写操作**。
  （**该依赖已在 T10 解除**：dbx 工具可见后，①②的只读快照都补做了，见 `evidence/dbx-charset-snapshot.txt`；
  那一行 UPDATE 仍未执行，也没有必要执行——档 A 免重启就够 T7 首轮用。）

**T5+T6 完成留痕（2026-10-30，四个已定位缺陷的代码侧修复）**

- 落盘改动全部集中在传输/序列化层，**零功能改动**：
  D8 [Net/Cryptography.cpp:89](file:///E:/project/MapleStory-Client/Net/Cryptography.cpp#L89)（`int16_t`→`uint16_t`）+
  [Net/Session.cpp:115-126](file:///E:/project/MapleStory-Client/Net/Session.cpp#L115) 长度守卫；
  D2 [Net/OutPacket.cpp:77-87](file:///E:/project/MapleStory-Client/Net/OutPacket.cpp#L77)；
  F5 [Net/OutPacket.h:138/166/173](file:///E:/project/MapleStory-Client/Net/OutPacket.h#L138)；
  D1 [Net/PacketSwitch.cpp:726](file:///E:/project/MapleStory-Client/Net/PacketSwitch.cpp#L726) 注销 `RECALCULATE_STATS` 注册。
- **改代码的同时改了"裁判"**：oracle 的客户端模型 `cl_check_length` 默认从符号扩展改为无符号，
  否则它对已修的 C++ 建模错，第 12 项会永远报 DIFF——这类"工具落后于被测物"的假红，比真红更消耗信任。
  同时补第 13 项**覆盖性断言**（要求 legacy 模型至少在一个长度上偏离）：D8 修完后第 12 项变成"全等"，
  若用例集恰好没进 `32768..65535`，它会以"全等"的名义什么都不证明。现在明写 14 个长度只有旧模型解错。
- 复跑：oracle 退出码 **1 → 0（20 项全绿）**，`pair_logs.py --self-test` 仍 rc=0，构建 `GATE OK`+`EXITCODE=0`。
  新快照另存 `transport-vectors-t5post.json`（源文件指纹随代码一起变），**T4 的旧快照不覆盖**。
- 顺手抓到一条新缺陷 **D9**（`HEADERLEN` 未定义 ⇒ "非加密直通模式"其实编不过），按"不动未编译代码"原则只登记不修。
- 三条 **不声称已证明** 的边界，写进 §2/§9 而不是藏起来：① **D2 无对拍覆盖**（oracle 建模的是传输链，
  `write_long` 又零调用点）；② **F5 只改了 opcode 值**，body 字段顺序/宽度未对账，实机取证挂 S6/S7/S8；
  ③ **D8 的守卫不解决 >32KB 包体本身**——两侧都把长度截进 16 位，守卫只保证踩到时是干净掉线而不是越界写。
- 本轮**未做 git 提交**（用户未要求）；**未重启服务端**。库侧：T5/T6 代码批本身没碰库；
  随后 T10 用刚可见的 dbx 工具做了**只读**快照（`accounts.language` 分布、GM 角色、`game_config` 五行），
  **仍然没有任何 DDL/DML**，那一行授权 UPDATE 也没执行（档 A 免重启够用，档 B 还没到需要开的时机）。
  dbx MCP 的工具本轮起已可见（上轮登记的"须重载 MCP"已解除），T9b/T10 的库侧步骤不再被这个依赖卡住。

**T10 完成留痕（2026-10-30，静态 + 库侧；实机判别留给 S 切片）**

- 产物：`notes/charset.md`（六层结构：库侧快照 / 服务端 4 个字符串调用点 / 绑定窗口陷阱 / s9 侧 4 条缺陷 C1–C4 /
  对 T11 的契约约束 / 未决）。
- **推翻了 T10 自己的实验设计**（同 T8 推翻"连接存活=被接受"那次同类）：原计划"探针分别以 GBK 与 ASCII 发同一串中文，
  看服务端解出什么"回答不了"账号语言是否参与"——因为 `ThreadLocalUtil.setCurrentClient` 只在
  `Client.java:220` 的 `handler != null && validateState` 分支内绑定，探针发的那些包多数根本不进 handler，
  而**两档日志更早**（`Client.java:216-218` / `addBefore("Client")`）⇒ 观测到的永远是 `SERVICE_LANGUAGE`。
  本部署 `SERVICE_LANGUAGE` 与账号语言都是 GBK，**结果相同即无法区分**；唯一不写库、不改配置的判别路径是
  看 handler 内部才做的字符串回显，归 S 切片。
- 库侧结论替用户消掉一个分叉：**无需换号**（3 个账号 `language` 全 = 3/GBK，表默认值也是 3）。
  `game_config` 五行 `update_time` 全 `NULL` ⇒ 印证"库里从没写过，运行期翻转只可能来自内存的 GM 命令"。
- 一次猜列名失败并纠正：按 `GameConfig` 的 JSON 结构去猜库表列（`tag`/`name`/`value`）直接 `ERROR 1054`，
  `describe_table` 后才知道是 `config_type/config_sub_type/config_code/config_value`。
  教训写进 notes：**内存树与库表不同形，跨层查询一律先 describe**。
- **真机补证（T7 会话 #1 之后回填）**：服务端出站的三个 s2c 字符串包按 GBK 逐字可读
  （`notes/charset.md` §0 的"本部署一律 GBK"由推断升为实测），**而 §3 的边界结论不变**——
  两档路径都映射到 GBK，字节相同就无法区分，所以这批 hex 支撑的是"编码是 GBK"，不是"账号语言参与了编解码"。
- 又一次 `describe` 前踩坑并当场纠正：查命令表时先假设 `showpackets` 只在代码里注册，
  实际 `CommandsExecutor.loadCommandsExecutor()` 走 `commandService.loadCommands()` → `command_info` 全表，
  硬编码那批 `addCommand(...)` 整块被注释掉了；且库有两个同构 schema，运行期用的是 `application.yml:18` 的 `beidou`。

`tools/build_smoke.bat` 的构建目标仍是 **x64**，而 `BeiDou.exe` 是 32 位 PE——这与协议对接无关（封包层与指针宽度无关，除了 D2 那个 `long` 陷阱），但记在这里，免得以后有人拿它当"原生客户端一致性"的证据。

**T7 真机会话 #1 留痕（2026-10-30，采集成功 / 配对仍缺服务端一侧）**

- 产物：`notes/real-session-1.md`、`evidence/pairing/client-packet_dump-20261030-2029.redacted.log`（56 行）、
  `evidence/pairing/report-20261030-clientonly.md`、`evidence/pairing/server-out-20261030-2029.session-lines.txt`（服务端会话边界 27 行 + 编码判定）、
  `evidence/probe/probe-t7-state{,2,3}.*`（档 A 状态判定三轮）。
  原始 57 行留在客户端目录未改，仓库内副本**剥掉了 L1 登录请求**（含明文账号/密码字节）。
- **用户报的"聊天 UI 有问题"归因完成，且不在客户端**：`GENERAL_CHAT` 发包格式与服务端
  `ByteBufInPacket.readString()`（短长度 + 按长度取字节）**逐字节对齐**，包也确实发出去了；
  卡的是 `CommandsExecutor.java:53-54` 把命令头写死成 `@`/`!`，而 `/` 开头的消息在
  `GeneralChatHandler.java:53-55` 的两个分支之外被**静默丢弃**（不执行、不广播、不回包）。
  ⇒ 结论：不改代码，改用 `!showpackets`；同时把"s9 命令前缀 ≠ 参考端 `/`"当成使用侧的既成事实登记。
- **自我纠错一处方法论**：判"档 A 关"如果只看"日志里没有行"是站不住的——探针的包可能压根没被解开。
  所以负结论必须挂在一个正结论上：同一次探针运行里 PING→PONG→活过 20s 复查窗口已证明
  `Client.handleInPacket` 被进入、派发点被走到，而档 A 的打印点在派发**之前** ⇒ 没打印才等于没开。
- 一次门禁按预期工作：`--send 3` 被 `opcode_blocklist.txt` 的**值级否决**（`0x0003 *`）挡成 `SKIP(blocklist)`，
  实际发送改用两个 s9 未注册值（`0x0352/0x0353`）——既零副作用（走 null 分支，只 `updateLastPacket()`）
  又能命中派发前的打印点。
- 会话窗口复核（写这条是为了下一轮不用重查）：`logs/out.log` 当日 215 行，**包日志类记录 0 条**
  （`收到封包`/`ClientSend`/档 F 行式各计数均为 0），尾部只有 `20:27:47 连接登陆端口`、`20:27:57 连接大区0频道1`、
  `20:29:39 保存角色数据 Admin` 和 20:33–20:34 我自己三次探针的登录连接行 ⇒ 与"档 A/F 都没开"一致，配对缺口确证在服务端侧。
- ⚠ **档位字母纠偏**：`MonitoredChrLogger` 这条通道我最初写成"档 E"，而 §3 能力表的 **E 已被"改 log4j2 级别"占用** ⇒ 统一改称**档 F**，
  `notes/log-pairing.md`、`tools/pair_logs.py` 与 §3 表格已同步；新增通道编号一律从 §3 表往后排，别复用。
- 顺带三条真机事实：① **hello 取不到**（4 字节纯头部、无正文，dump 调用点在正文之后）⇒ 握手证据只能来自探针 + 会话走通；
  ② **T6/D1 得到真机验收**：进图序列里出现 RECV `0x0023 FORCED_STAT_RESET`；
  ③ 三个 s2c 包的中文按 GBK 完全可读（`登录了游戏` / `蘑菇王国` / 家族特权整表）⇒ 出站字符串=GBK 得证，
  但按 `charset.md` §0 的边界，它仍不能证明"账号语言参与"（两条路都是 GBK）。
- 待查已登记在 `real-session-1.md` §5：`SPAWN_NPC` 三组包逐字节重复两轮、`SET_FIELD` 里一段既不像 GBK 也不像 UTF-16LE 的非文本域。
- 本轮仍未做 git 提交、未重启服务端、未对库做任何写操作（只跑了 `information_schema` 与 `command_info` 的只读 SELECT）。

**T7 真机会话 #2 留痕（2026-10-30，闪退已根因定位并修复 / 跨向配对仍未拿到）**

- 产物：`notes/crash-chatbar.md`（取证链 + 修复 + 未查清的另两条崩溃签名）、
  `evidence/crash/wer-maplestory.txt`（14 天 73 条 MapleStory 崩溃的 WER 签名）、
  可复跑工具 `tools/wer_crash_query.ps1` + `tools/symbolize_offset.py`。
- **用户报"关闭不了聊天、客户端闪退"根因不在封包层**：`0xc0000005 @ RVA 0x7fb70` 经旁挂 PDB（时间戳 `0x6ae488a0` 与 WER 签名同代）解析到
  `IO/Components/Button.cpp:33` 的 `position = pos;`，即 **classic HUD 下 `buttons[BtMin]->set_position` 的空 `this` 写访问**——
  `buttons` 是 `std::map<uint16_t, std::unique_ptr<Button>>`（`IO/UIElement.h:214`），`operator[]` 对未创建的 id 插入空指针；
  classic 构造函数只建 `BtWhisper` 就 `return`，而 `update_view()` 无条件碰 6 个不存在的按钮 ⇒ 必崩。详见 `notes/crash-chatbar.md` §1–§2。
- 修复＝三处 `classic_mode` 守卫（`update_view` / `toggle_input` / `toggle_view`），构建 `GATE OK` + `EXITCODE=0`，
  已写入运行目录新 exe（`0x6ae497c6`，21:12）。**"回车关不掉输入框"是另一个独立缺陷**（`input_text_enter_callback` 只发送+清空、不收框），
  本轮只登记未改：改了会偏离参考端行为，留到 UI 切片。**运行态复测未做**，需用户真机验证 ESC / TOGGLECHAT 不再闪退。
- ⚠ **档 F 至今未被真正验证**：dump 里 **没有任何 `!monitor` 包**（会话 #2 最后一行是 `GENERAL_CHAT("!showpackets", true)`，更早只有 `/showpackets`），
  且档 A 依旧零产出（服务端有 `玩家 Admin 执行了命令：ShowPacketsCommand` @20:59:16.382，但 `收到封包`/`ClientSend` 计数仍为 0）
  ⇒ 会话 #2 的失败原因是**客户端在采样前就崩了**，不是档位无效。下一轮把"进图 → `!monitor` → 操作 → 退出"当作验收动线重跑。

**T7 真机会话 #4 留痕（2026-10-30 22:52，`!monitors` 后 1 秒闪退 / 档 F 首次挂上但零产出）**

- 产物：`notes/crash-chatbar.md` §8、`evidence/crash/session4-uievent-oob.txt`（符号化输出 + WER 正文 + dump 尾部逐字段 + 服务端日志窗口 + 两条负结果）、
  `builds/exe-stamps.md` 新增 `0x6ae4b655` 行。
- **根因是协议语义错位，不是 UI bug**：s9 把 `SET_WEEK_EVENT_MESSAGE(0x004D=77)` **复用为 yellow tip**
  （`PacketCreator.java:5437-5443 sendYellowTip` = `0xFF` + `writeString` + `writeShort(0)`；`Character.java:6952-6954` 的 `yellowMessage` 及 7 处 GM 广播走它），
  我们的 `WeekEventMessageHandler` 却据此 `emplace/makeactive` 一个**没有数据源**的 `UIEvent`；
  其构造函数在 `EventList/main` 资源不可用时早退（`events` 未填、`event_count` 仍 16），`makeactive()` 只做 `active = true`（`IO/UIElement.cpp:69-72`）
  ⇒ `draw()` 越界读空 vector，`0xc0000005 @ RVA 0x191880` → `UIEvent.cpp:123`。
  **"第一条不崩、第二条才崩"**可解释用户为什么是"输完 `!monitors` 一会才闪退"：`emplace` 出来的元素仍 `active=false`，第二条走 `makeactive()`。
- ⚠ **对照表门禁抓不到这类错**：`opcode-matrix.json` 里 77 的名字、值、`status=HANDLED` 三项全过，缺的是"body 语义"维度
  ⇒ 给任务 #24 新增一类 **"opcode 名与值都对、但服务端把该包复用成另一种消息"**，与"客户端缺 handler""opcode 撞值"并列。
- 修复＝两处：`UIEvent.cpp:48-56` 早退时把 `event_count` 归零（堵越界的**充要**位置）；
  `MessagingHandlers.cpp:481-502` 改为跳尾 short + 按 `MessageType::YELLOW` 送进 `UIChatBar`（**GM 反馈文字第一次真正可见**）。
  构建 `GATE OK` + `EXITCODE=0`，23:34 写入运行目录（`0x6ae4b8f0`；23:23 的 `0x6ae4b655` 是同代码首版，仅差注释），exe+pdb 已快照到 `E:\MXD\_build_snapshots\20261030-2334_uievent-oob-fix-final\`。**运行态复测未做**（任务 #26）。
- **档 F 状态从"未验证"升级为"档位有效性已证、数据仍为零"**：`!monitor Admin` 成功（服务端 `MonitorCommand` @22:52:34.987，客户端收到 `1 已被监控`，chrId=1 进监控集），
  但挂上到崩溃（22:52:42）之间入站只有 `MOVE_PLAYER(41)` 与 `GENERAL_CHAT(49)`——都在 `MonitoredChrLogger.java:84` 屏蔽表里 ⇒ `Admin-Admin` 行全会话 **0 条**。
  下一轮要点：`!monitor Admin` **只输一次**、**不要再输 `!monitors`**（本会话就是它触发的崩）、**不要再输 `!showpackets`**（取反开关，会话 #3 就是被第二次输入关掉的），
  然后正常玩（打怪/拾取/开包/交易等**非屏蔽** opcode），退出后再取日志。

**插叙：真机会话 #5 顺手清的两条聊天栏遗留（2026-10-31 00:12 部署并验证通过）**

- 会话 #5 本身**没跑复测脚本**：23:38 登录到 00:00 在线 22 分钟无崩溃（服务端只有 `207` + 周期 `24`），
  但 `!monitor` / `!monitors` 一条都没输 ⇒ §8.3 的语义修复仍停在"没崩"，档 F 首份真数据继续悬着（任务 #26 不变）。
- 正面产出两条：回车收框（`close_input()`）与 **classic 聊天行文字错位**（真机抓帧实测，非猜）⇒ 详见 `notes/crash-chatbar.md` §9。
- 方法沉淀：**`PrintWindow(PW_RENDERFULLCONTENT)` 旁路抓帧**——不抢前台、不 `taskkill`，用户在线也能取证；
  换算 `client = image - (8, 30)`。通则：`Text::draw(y)` 的字形顶部在 y 之下约 10px（A11M/FreeType），
  classic 那类"贴边小行"的 y 偏移必须扣掉它，否则文字掉进下沿控件里。
- 构建状态：用户退出后跑 `tools/build_smoke.bat` 正式部署 `0x6ae4c1fd`（快照 + `builds/exe-stamps.md` 登记），
  真机复核**布局与行为都过**（§9.5 六行状态表：Enter 开 / 打字渲染 / Enter 发送并收框且服务端回播 /
  空回车收框 / ESC 收框不崩 / 收后再开×3）。两条记账：未聚焦时字母会漏成热键（误耗一次随机传送石，猎人平原→Orbis，不可逆）；
  `MonitoredChrLogger.java:84` 屏蔽了 `GENERAL_CHAT` 的日志 ⇒ 服务端日志查不到 49 不等于包没发出去。

**插叙：真机会话 #7 —— T11 部署与 CJK 端到端复测（2026-10-31 01:44–01:55，通过）**

- 部署：用户退出后把 `e1394e5137e498a1`（快照 `20261031-0205_t11-packetcodec`）拷进运行目录，旧版留档为
  `MapleStory.exe.bak_pre-t11-20261031`；**服务端未重启、库未写**（只读口径不变）。
- 判据：同一条 0x004D 包型，会话 #5 那版只剩 `[Notice] 1`，这版出 `[Notice] 1 已被监控` + 同帧粉字
  `Admin 正在监控 1` ⇒ 归因唯一（前后差的就是 T11 那一层）。线上 `0a 00`=10 = `"1 "`(2)+`已被监控`(8 GBK)
  正面证明"先转码再量长度"的顺序；同一命令的 `[SEND]` 与改动前**逐字节一致**，ASCII 快路径零回归。
- 假阳性排除：`已被监控`/`不在被监控`/`正在监控`/`停止监控` 在客户端源码 0 命中、在服务端 `.java` 0 命中
  ⇒ 文案只能来自 `message_zh_CN.properties:424-427` 经线上 GBK 字节进来。
- 自我约束：NPC 文本、公会/联盟公告、密语正文**没有**单独真机取帧，只记"共用 `read_padded_string` 接缝 + oracle 覆盖"
  （T12 的教训：一次屏幕观测 ≠ 逐层读代码），留给 S 切片各取一帧；度量层当时仍 ASCII-only ⇒ #28 不随本条关闭，
  度量层本身由会话 #8 单独闭环（见下方 #8）。
- 现场遗留：服务端 `Admin` 的包监控当前为 **ON**（`!monitor Admin` 可关）；客户端 PID 42456 在跑
  （该实例已于会话 #8 被 T12b 构建替换，现况见下方 #8）。
- 证据：`evidence/charset/t11-live-20261031.md`（逐字段对账 + 前后截图 + s9 链路）、
  `evidence/chat/t11-notice-cjk-rendered-20261031.png`、`evidence/chat/t11-full-after-20261031.png`。

**插叙：真机会话 #8 —— T12b 字宽度量落地并读出像素差（2026-10-31 02:0x–02:4x，通过）**

- 根因不是"少乘了个系数"，而是 **UTF-8 解码被手写了两遍**：度量侧按字节累加、绘制侧按码点取字形
  ⇒ 量出来的宽度和画出来的推进量天然对不上。所以修法是把解码收成一个函数
  （新增 `Graphics/Utf8.h::ms::utf8_next`），两侧同调，让不变式变成**结构性**的而不是靠约定。
- 承重假设先离线证明再上线：度量侧在图集 `locked` 时也要能算，故走
  `FT_Load_Glyph(FT_LOAD_DEFAULT)` + `advance.x >> 6`，而绘制侧是 `FT_LOAD_RENDER`。
  两种 load flags 的 advance 是否相等 = 这次修复的地基，`tools/probe/utf8_metrics_check.cpp`
  比了 **3570 次、0 反例**才敢用（另有 26 条解码向量 failures=0，含 6 条 `lax:` 向量把解码器
  "不校验尾字节 / 不拒过长编码 / ≥0xF8 引导字节照收"的既有宽松行为钉住并写进注释，不当已修也不静默改）。
- **判据选点**：全仓唯一"把 `Text::width()` 直接写进几何、且屏幕上恒定有中文"的地方 ——
  classic 聊天栏目标芯片用 `target_x = (81 - width())/2` 手动居中（`UIChatBar.cpp:301-305`）。
  同一串 `对所有人`、同一个 81px 贴图，修复前后各读一次像素：
  **左内缩 3 px → 16 px，居中偏差 −13.5 px → 0.0 px，而字形实宽 41 → 42 px 不变**。
  预测位移 14 px（`(81−72)/2` → `(81−44)/2`）对实测 13 px；字形实宽不变正好证明绘制侧没被顺手改动。
- 英文回归：ASCII 分支是同一张表的同一个表达式，且真机同屏 `Ver. 0.0.1` /
  `[Welcome] Welcome to MapleStory!!` / `Admin` 名牌排布与修复前一致。
- **未观测项显式记账**（不并入"CJK 已闭环"）：长中文**换行**只在离线侧证到，真机本轮没有足够长的中文串可取帧；
  `Textfield` 中文光标属 T13（输入预留）；气泡（`ChatBalloon` 按 `width()` 拉伸 9 切片）与
  名牌（`NAMETAG` 的 `right = left + w + 3`）与芯片同源同算式，但本轮**未取真机帧**。
  快捷槽下方 `对所/有人`、`对/组员` 两帧**换行点相同** —— 该按钮 `maxwidth` 恰好落在新旧度量值之间，
  不构成判据，故不拿来充数。
- 部署：`6da3c2f6ca40936a`（快照前 T11 构建留档 `MapleStory.exe.bak_pre-t12b-20261031`）；
  备份链现为两级（`pre-t12b` = T11 构建、`pre-t11` = 再前一版）。**服务端未重启、库未写、未 commit**。
  客户端 PID 29420（hwnd 5903712）在跑，已自动登录 `[1-Admin]`。
- 证据：`evidence/render/t12b-live-20261031.md`（算式预测 ↔ 实测像素）、
  `evidence/render/t12b-chip-ab-6x.png`（上 old / 下 new 对照帧）。

---

## 9. 完成定义（DoD）

- [x] `opcode-matrix.json` 由脚本生成、门禁接入、两侧源码变更即失效（T1/T3；门禁分硬/软两档：协议事实矛盾拦构建，仅引用行号搬动放行）
- [x] 撞值数 = 0（客户端），`write_long` 修复且构建通过 —— **T6/T5 清 35；2026-11-02 T14b 清 152（`WARN_MESSAGE` 撞 `ARIANT_RESULT` 的废弃注册已删除，见 `t14b-s2c-scan.md` §2）**：客户端收包枚举撞值归零，`opcode-matrix.json` 重生成后 `client_in collisions` 无 152 项、`s2c HANDLED` 298→299；`write_long` 已修 + 构建 `GATE OK`/`EXITCODE=0`。F5 三条（STORAGE/PLAYER_INTERACTION/CASHSHOP_OPERATION）实机取证随 S6/S7/S8 切片完成；`BUDDYLIST 63→130`（c2s）与 `WARN_MESSAGE` 撞车（s2c 152）为本轮新增修复。真机会话 #1 已给 D1 补验收——RECV `0x0023 FORCED_STAT_RESET` 出现在进图序列。
- [x] 传输层 6 项（静态 key / IV 展开 / funnyBytes 6 遍 / OFB 块切换 / updateIv / header 数学）逐条有 vector 对拍结论（T4：20 项 19 全等，唯一差异 = 新缺陷 D8，向量见 `evidence/vectors/transport-vectors.json`；**T5 修完后复跑 20 项全绿、退出码归 0，快照 `transport-vectors-t5post.json`**）
- [x] **探针能被真实服务端接受并正确回包**（T8：20 项门禁断言 0 失败 / 2 项实测结论，连跑两轮可复现。c2s 由 **PONG ack oracle** 裁决，s2c 由 PING 解密裁决；证据 `evidence/probe/probe-final.*`）
- [x] **日志配对管线工具就绪**（T9：`tools/pair_logs.py` + `notes/log-pairing.md`；`--self-test` 覆盖四种日志格式与 EXACT/DIFF/UNMATCHED/长度违规四判据，全绿 rc=0）
- [x] **真机会话取证（T7 首轮）**：一次完整 登录→选角→进频道→进图→移动→退出 采集到 57 包（12 c2s + 44 s2c，含 617B `SET_FIELD`），
      传输层与 opcode 派发在真机上全通；`pair_logs` 对新格式 56 包 **`unparsed=0`、rc=0**；证据与归因见 `notes/real-session-1.md`
- [x] **服务端入站日志配对管线跑通**（原写"档 A"，按 §3 能力表改名后应为**档 F 优先、档 A 兜底**）：**两档都跑通**——会话 #3 用档 A 拿到 7 条服务端入站记录，与客户端 `[SEND]` 配对 **7 EXACT / 0 DIFF / 0 长度不符，rc=0**（`evidence/pairing/report-20261030-2232-paired.md`）；**会话 #5 用档 F 拿到首份原始 hex 并完成首个字段级配对**：整窗口 **11 EXACT / 0 DIFF / 0 长度不符 / `um_server=0`**，其中 4 条是非心跳包（`WHISPER`×2 + `MULTI_CHAT`×2，逐字段与 s9 handler 对齐，`notes/log-pairing.md` §9.1）。未配对项已逐条解释（774 发送里 759 个 `MOVE_PLAYER` 被两档共同过滤；`GENERAL_CHAT` 被档 F 屏蔽表挡下；心跳缺口由"PING 是 Netty 空闲事件 + 档 F 挂上时刻晚于切片起点"两条闭合）。`0x3C/0x28/0x18` 三个未配对 opcode 已全部归因（§9.3，三者 `status=MATCHED`，缺的只是当时无档位记录，`0x28`/`0x3C` 的字段级预对账也已做完）。**仍缺**：`0x28`/`0x3C` 的**正面对账**——需在档 A 开着或档 F 挂着的窗口里点一次商城、答一次 NPC 对话
- [x] **两次真机 AV 都根因到源码行并修复**（会话 #2 `Button.cpp:33` 空 `this` / 会话 #4 `UIEvent.cpp:123` 越界读空 vector）：取证链、修复与**运行态复测边界**都在 `notes/crash-chatbar.md`；会话 #4 顺带暴露并修掉一处**协议语义错位**（`0x004D` 被 s9 复用为 yellow tip），该类问题对照表门禁不可见 ⇒ 归入任务 #24 新增分类
      —— **客户端一侧已就绪**（上面那条），缺的只是服务端一侧的日志。原以为是"档 A 没开成"，
      真查下来是**命令头不对**：s9 认 `@`/`!` 不认 `/`（`CommandsExecutor.java:53-54`），故正确输入是 `!showpackets`（档 A）或 **`!monitor`（档 F，给原始 hex，字段级对账正解）**；
      档 A 当前状态已用探针判为**关**（`probe-t7-state3`，负结论挂在 PONG ack oracle 上才算成立）；今日 `out.log` 实测**零条包日志**（只有连接/存档行），与判定一致
- [ ] 11 个切片的包清单表全部落盘，每条字段差异要么修复、要么记为"已知偏差 + 理由"
- [ ] 字符集权威结论（哪个 lang、哪张表决定、登录/进图两阶段差异）写入 notes，并有真机 hex + 服务端解码文本支撑
      —— **T10 已闭合"静态 + 库侧"两半**（`notes/charset.md`：库内 3 账号全 `language=3`=GBK，
      服务端只从 `accounts.language` 取，包头里的 language 不被任何 handler 读），
      **还差真机那一半**；且 T10 发现**档 B 的 `<TEXT>` 不参与账号语言**（绑定前解码），
      所以"服务端解码文本"这条判据本身要按 charset.md §3 重新表述，不能照原样打勾。
      —— 真机会话 #1 已补上 **hex 那一半**：`0x007A`/`0x0107`/`0x0064` 三个 s2c 包的中文按 GBK 逐字可读
      （`登录了游戏`/`蘑菇王国`/家族特权整表），但**它仍不构成"账号语言参与"的证明**（本部署两条路都是 GBK），
      所以本条**保持未勾**，不是缺数据而是这条判据本身无法把两者分开。
- [x] **封包边界字符集转换落地（T11）**：`Net/PacketCharset.{h,cpp}` 单一接缝接进 `InPacket::read_padded_string`
      与 `OutPacket::write_string`（先转码再按转码后的**字节数**写前缀），一处覆盖入站 175 + 出站 74 个调用点。
      离线对拍 **73 条断言 failures=0**，期望值由 Python `cp936`/`gbk` codec 现算（`tools/probe/gen_charset_vectors.py`），
      其中 `已被监控` 的期望 GBK 与真机入站字节逐字节相同 ⇒ 不是自证；全量构建通过。
      两条安全阀限定回归面：纯 ASCII 恒等、转换失败原样收发。
      **真机已复测**（`e1394e5137e498a1` 部署到 `E:\MXD\BeiDou-Client-xy\`）：`!monitor Admin` ⇒ 屏幕 `[Notice] 1 已被监控` +
      `Admin 正在监控 1`，线上 `0a 00`=10 与 `"1 "`+8 GBK 逐字段对上；同一命令出站改动前后逐字节一致（ASCII 快路径无回归）。
      证据与现场遗留状态（服务端 `Admin` 监控仍为 ON）见 `evidence/charset/t11-live-20261031.md`。
- [x] CJK 渲染：**显示侧真机跑通（含字宽度量），输入侧显式预留**，不许半成品
      —— 渲染层本身已由真机反证可用（SimHei + `GraphicsGL.cpp:1354-1359` UTF-8 解码，同屏 `对所有人` 正常显示）；
      T11 部署后的端到端验收也过了：同一条 0x004D yellow tip 包型，改动前屏幕只剩 `[Notice] 1`，改动后 CJK 尾巴完整显示，
      且 0x0044 type 5 粉字广播行 `Admin 正在监控 1` 同帧可见 ⇒ 入站解码→内部 UTF-8→渲染 三段闭合。
      **度量层（原预留①）已于会话 #8 随任务 #28 关闭**：解码收成一个共享函数 `Graphics/Utf8.h`，度量与绘制同调，
      真机像素 A/B 读出 `对所有人` 芯片偏心 −13.5→0.0 px 而字形实宽不变（`evidence/render/t12b-live-20261031.md`）。
      **仍算预留的只剩一条**：② 玩家**输入**中文未做——按用户决策"先只管显示，输入预留"，`IO/Window.cpp:709 begin_ascii_text_input` 仍主动关 IME，
      所以"自己打一句中文发出去"这条至今没有真机样本，归 T13。
      另有三处同源同算式、**本轮未取真机帧**的度量受益面（长中文换行 / `ChatBalloon` 拉伸 / `NAMETAG` 名牌背景），
      随 S 切片各取一帧，不并入本条勾选。

- [ ] `gap-inventory.md` 三分类完成，且每个"客户端从不发"的 opcode 有探针结论（忽略/报错/回包）；已实现批次各自原子提交
- [ ] 每片后的登录→进图回归记录在 evidence
- [x] 探针 `opcode_blocklist.txt` 存在且生效，破坏性包未经放行不发送（89 条值级否决 + 1 条 allowlist；清单名字/值每次运行现场对 s9 源码复核，不符即 ABORT rc=99；`--dry-run` 已实测拦下 `ADMIN_SHOP`/`USE_TELEPORT_ROCK`，并抓到一处手抄错名）
- [ ] 临时调试日志清零（只留 `debug.packet_dump` 这类可配置上线级输出）
- [ ] `README.md` 最终实现逻辑文档已补

---

## 10. 开工顺序（v2）

T1→T2→T3（建表与门禁）→ T4（oracle 静态对拍 ✅，发现 D8）→ **T8（探针 ✅：oracle 不是自欺；产出 F6，判据改为 PONG ack oracle）** → **T9（日志配对管线 ✅ 工具 + 自检全绿；真数据待 T7）** → **T5/T6（✅ 代码完成：F5 三条改值 + D1 撞值 + D2 `write_long` + D8 入站长度；oracle 复跑归 0。实机验收与 body 对账挂 S6/S7/S8）** → T7（真机 handshake 取证 + 首次真实配对；**频道侧已用探针补齐 `evidence/probe/probe-channel.*`；真机会话 #1 已采集 `notes/real-session-1.md`——传输层/派发真机全通、D1 拿到真机验收、GBK 得实测；跨向配对只差服务端日志，卡点是命令头应为 `!showpackets` 而非 `/showpackets`**）→ T10（字符集定盘，**静态 + 库侧 ✅ `notes/charset.md`；实机判别归 S 切片**；dbx 工具本轮起可见）→ S1..S4（先把已联调链路钉死）→ T11..T13（GBK 三 spike）→ S5..S11 → T14（缺口归因）→ T15+（按批补齐）。

把探针排在改客户端之前，是为了让后续每一片的"对不对"由服务端当场裁决，而不是由我读代码推断。
