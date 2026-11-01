# T9 笔记：双端封包日志配对管线

服务端根：`E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\`（下文 `path:line` 都相对这里）
客户端根：`E:\project\MapleStory-Client\`

## 1. 为什么要这条管线

T4 的 oracle 只证明「加解密算法与 s9 一致」，T8 的探针只证明「头部被接受、包被派发执行」。
两者都碰不到 **body 字段级**（P2 的 S1..S11）。字段级对账需要第四方证据：

> 同一条真实 TCP 流里，客户端 `encrypt()` 之前看到的字节，必须与服务端 `decrypt()` 之后
> 交给 handler 的字节**逐字节相同**；反向同理。

这就是 `tools/pair_logs.py`：把客户端 `packet_dump.log` 与服务端 `logs/out.log` 按方向逐包配对。

---

## 2. 取证开关分档（本轮再加一档，并更正命令前缀）

| 档 | 开关 | 覆盖范围 | 生效方式 | 需要重启？ |
|---|---|---|---|---|
| **档 A** | `use_debug_show_rcvd_packet` | **只有入站**；且被 `LoggingUtil.isIgnoredRecvPacket` 过滤掉 7 个 opcode | 游戏内 **`!showpackets`**（GM5）→ `GameConfig.update()` **只改内存** | **不需要** |
| **档 B** | `use_debug_show_packet` | 入站 + 出站，**不过滤任何 opcode** | 只在 `GameConfig` 单例构造时从库读一次 | **需要重启**（或走 REST，见下） |
| **档 C** | 客户端 `debug.packet_dump` | 客户端出站（加密前）+ 入站（解密后），全量 | `xy-config.yaml` 打开后重启客户端 | 客户端重启 |
| **档 F** ⚠**新增，字段级对账首选** | 无开关，按角色挂载 | **只有入站**，但给**完整 hex**（`HexTool.toHexString`，大写、空格分隔）；过滤 7 个 opcode（`MOVE_PLAYER`、`GENERAL_CHAT`、`TAKE_DAMAGE`、`MOVE_PET`、`MOVE_LIFE`、`NPC_ACTION`、`FACE_EXPRESSION`） | 游戏内 **`!monitor [角色名]`**（GM3，`command_info` 实测 level=3/enabled=1）→ `MonitoredChrLogger.toggleMonitored()` 往内存 `HashSet` 加/撤 | **不需要**，且不读 `GameConfig`、不碰库 |

> 编号说明：档位 A/B/C/D/E 已被 PLAN §3 的能力表占用（**E = 改 `log4j2.xml` 调日志级别**），
> 所以 `MonitoredChrLogger` 这条新通道从 **F** 起编，别和 PLAN 的 E 混。

关键证据链：

- ⚠⚠ **命令前缀是 `@` / `!`，不是 `/`**（`client/command/CommandsExecutor.java:53-54`：
  `USER_HEADING='@'`、`GM_HEADING='!'`；`isCommand` 在 `:64-70` 只认这两个头）。
  以 `/` 开头的文本在 `net/server/channel/handlers/GeneralChatHandler.java:53-55` 里
  既不进 `isCommand` 分支、又因 `heading != '/'` 为假而不广播 ⇒ **整条消息被静默丢弃**（不执行、不广播、不回包）。
  真机已验证过一次：`/showpackets` 的包确实按正确格式发出并被服务端正确解码（`notes/real-session-1.md` §3），
  但档 A 没开就是这个原因。**要开日志一律打 `!showpackets` / `!monitor`（GM 角色 `!` 与 `@` 都收）。**
- 命令表来自库不是代码：`CommandsExecutor.loadCommandsExecutor()` 走
  `service/CommandService.java:37-54` → `commandInfoMapper.selectAll()`，硬编码那批 `addCommand(...)` 整块被注释掉
  ⇒ 判断"某命令能不能用"要查 `command_info`（运行期库是 `application.yml:18` 的 **`beidou`**，不是 `beidou_s9`）。
- 档 A 的读取点：`client/Client.java:216`（`if (GameConfig.getServerBoolean("use_debug_show_rcvd_packet") && !LoggingUtil.isIgnoredRecvPacket(opcode))`）。
- 档 A 的过滤清单：`net/packet/logging/LoggingUtil.java:9-23` —— `MOVE_PLAYER(41)`、`HEAL_OVER_TIME(89)`、`SPECIAL_MOVE(91)`、`QUEST_ACTION(107)`、`MOVE_PET(167)`、`MOVE_LIFE(188)`、`NPC_ACTION(197)`。**这七个恰好是移动/NPC 高频包**，也就是说档 A 天然看不见角色到处跑动时最该对账的包。
- `!showpackets` 只改内存：`client/command/commands/gm5/ShowPacketsCommand.java:39-44` → `config/GameConfig.java:73-82`（`update()` 只往 `properties` JSONObject 里 `put("value", ...)`，**没有任何 mapper 写库**）。⇒ 即时生效、重启即回落到库里的 `false`。
- 档 F 的读取点与行式：`net/packet/logging/MonitoredChrLogger.java:56-76`
  （`log.info("{}-{} {}-{}", accountName, chr.getName(), packetId, hex)`，`packetId` 是**十进制**），
  调用点 `client/Client.java:223`——在 handler 分支内部，所以 `setCurrentClient` 已绑定；
  挂载/撤销 `toggleMonitored(chrId)` 纯内存 `HashSet`，`!monitors` 可列出当前被监控角色。
  **它比档 A 更适合字段级对账**：档 A 打的是 `ByteBufInPacket` 的 toString（字符串字段变成 `<TEXT>` 且插入 `_` 标读位置），
  档 F 打的是原始字节。
- 档 B 的两个读取点：`net/packet/logging/InPacketLogger.java:21`、`OutPacketLogger.java:22`；两个 handler 用 `addBefore("Client", ...)` **无条件挂载**（`net/netty/ServerChannelInitializer.java:55-56`，字段定义在 :26-27），所以只受这一个开关控制。
  顺带确定了顺序：入站是 `InPacketLogger` 先、`Client.channelRead` 后 ⇒ 同一条包的 `ClientSend` 行必在 `收到封包` 行之前，这是镜像去重的前提。
- 档 B 必须重启的原因：`config/GameConfig.java:22-29` 单例 `private GameConfig()` 里 `configService.loadGameConfigs()` 只在类加载时跑一次；`service/ConfigService.java:255-256` 的第二次调用只服务于导出 yml，不回写单例。⇒ **手写 SQL 改 `game_config` 对运行中的服务端无效**。
- 例外（不用重启的档 B 路线）：`controller/ConfigController.java:52-54` `POST /config/v1/updateConfig` → `service/ConfigService.java:117-134` 先 `gameConfigMapper.update(...)` 落库、再 `GameConfig.update(gameConfigDO)` 刷内存。代价：它是后台鉴权接口，且会顺带 `langResourceService.insertOrUpdateI18n(...)` 往 `lang_resource` 写一条 `game_config` 记录（副作用面比单行 UPDATE 大）。

**结论**：T7 会话 #1 用「档 C（客户端全量）+ 档 A（服务端入站，即时）」起步，但档 A 因前缀写错没开成（见上）。
**下一轮首选档 F**：`!monitor` 即时生效、不读 `GameConfig`、不碰库、给完整 hex，是唯一能直接支撑字段级对账的免重启通道；
需要覆盖聊天/受击类 opcode 时叠档 A，仍缺出站方向时才动档 B（改库 + 重启前报备）。
PLAN §3 里「档 B 只靠 DML 开启」的说法要按上表更正为「改库必须配重启；`!showpackets` / `!monitor` 是纯内存开关」。

**本轮未执行的两步（卡在同一外部依赖）**：`accounts.language` 只读快照、`game_config` 快照 + 那一行已授权 UPDATE。
dbx MCP 已注册进 Qoder（serverId `88b1adfa-6c4a-4ad3-ab78-bcc339709e95`，status ready）。
T9 那一轮会话内看不到它的工具（会话启动时不热重载）；**2026-10-30 T5/T6 收尾时已可见**（18 个 `mcp__dbx__*` 工具，
连接 `75c9875d-…` = mysql `127.0.0.1:3306/beidou`）⇒ 卡住的两项库侧步骤（`accounts.language` 只读快照、
`game_config` 快照 + 那一行授权 UPDATE）从 T10 起可执行。**截至本轮，仍然没有对数据库做任何写操作。**

---

## 3. 日志行格式（五种，逐字对齐源码，不凭记忆）

### 3.1 档 C：客户端 `packet_dump.log`（本仓库 `Net/Session.cpp:36-56`）

```
[SEND] PacketDump : 01 00 00 00 61 61 61 00 00 00 00 00 
[RECV] PacketDump : 00 00 04 00 00 00 64 
```

- 小写十六进制、`"%02x "` 循环 ⇒ 每字节后一个空格（**行尾也有一个空格**）。
- `[SEND]` = `Session.cpp:166`，在 `cryptography.encrypt()` **之前**打；`[RECV]` = `Session.cpp:132`，在 `cryptography.decrypt()` **之后**打 ⇒ 两侧都是**明文正文**。
- 正文**含 2 字节小端 opcode、不含 4 字节头**（头是 `create_header` 另加的）。这一点对齐了服务端所有格式，是配对的基础。
- `[SEND]/[RECV]` 前缀是 T9 本次新加的；之前的历史行是裸 `PacketDump : ...`，**方向未知，工具会剔除并单独计数**。

### 3.2 档 B：`ClientSend` / `ServerSend`

```
ClientSend:LOGIN_PASSWORD [1] (12) <HEX> 01 00 00 00 61 61 61 00 00 00 00 00 <TEXT> ....aaa.....
<UnknownPacket> ServerSend:null [FFAC] (4) <HEX> AC FF 00 00 <TEXT> ....
AB CD ...
```

- 模板：`InPacketLogger.java:38-39`、`OutPacketLogger.java:39-40`。
- `<HEX>`：`util/HexTool.java:41-43` = `HexFormat.ofDelimiter(" ").withUpperCase()` ⇒ **大写、空格分隔、行尾无空格**。
- `<TEXT>`：`HexTool.java:73-84`，控制码（0..31）替成 `.`，其余按 `CharsetConstants.getCharset(ThreadLocalUtil.getClientLang())` 解码 ⇒ **T10/T11 判断字符集时直接读这一栏**。
- 括号里的数字 = `packet.getBytes()` 长度。`ByteBufInPacket.getBytes()`/`ByteBufOutPacket.getBytes()` 都是 `ByteBufUtil.getBytes(byteBuf)`（readerIndex..writerIndex），在两个 logger 触发的时刻 readerIndex=0 ⇒ **含 opcode、不含 4 字节头** ⇒ 应与客户端 `packet_dump` 的字节数**严格相等**；工具用 `length_check` 强制核对。
- opcode 名查不到时：`<UnknownPacket> ` 前缀 + SLF4J 把 `null` 打成字面 `null`。工具把这种记录的名字清空，改用 T1 矩阵反查。
- **超阈值只打前 2 字节，且两侧阈值与格式都不一样**：入站 `InPacketLogger.java:17,40-41` 阈值 **3000**、模板 `"{}..."`（**无空格**，如 `AB CD...`）；出站 `OutPacketLogger.java:18,41-42` 阈值 **50000**、模板 `"{} ..."`（**有空格**）。⇒ 方向信息丢失，工具归类 `server_trunc`，**不参与配对**，只在汇总里计数。

### 3.3 档 A：`收到封包`

```
2026-10-30 19:20:00.123 [ INFO] ==> 收到封包 包头ID [0x1] 内容：ByteBufInPacket[0100_00006161610000000000]
```

- 模板：`Client.java:216-218`；`packet` 的 `toString()` 见 `net/packet/ByteBufInPacket.java:92-102`。
- 十六进制**连续无空格、大写**（`ByteBufUtil.hexDump().toUpperCase()`），并在 `2*readerIndex` 处插入 `_`
  （`ByteBufInPacket.java:104-108`）。因为 `Client.java:214` 已先 `readShort()` 取走 opcode，
  所以正常包的 `_` **恒在第 4 个 hex 字符处**（= reader_pos 2）；这个下划线必须剥掉才能比对。
- 同一条入站包如果档 B 也开着，会先出 `ClientSend` 再出这一行 ⇒ 工具按「相邻且字节完全相同」做镜像去重，不重复计数。

### 3.4 档 F：`admin-Admin 1-00 00 61 61 61 ...`（`!monitor`）

```
2026-10-30 19:20:00.130 [ INFO] ==> admin-Admin 1-00 00 61 61 61 00 00 00 00 00
2026-10-30 19:20:00.131 [ INFO] ==> admin-Admin 24-<empty>
```

- 模板：`MonitoredChrLogger.java:79`（`log.info("{}-{} {}-{}", ...)`）⇒ **账号-角色 空格 十进制opcode-十六进制**。
- hex 由 `util/HexTool.java:41-43` 产出：`HexFormat.ofDelimiter(" ").withUpperCase()` ⇒ **大写、空格分隔**；
  body 为空时打字面量 `<empty>`（`MonitoredChrLogger.java:78`）。
- ⚠ **这一行的 hex 不含 opcode**：入参是 `packet.getBytes()`，而
  `ByteBufInPacket.java:18-20` = `ByteBufUtil.getBytes(byteBuf)` 从 **readerIndex** 起算，
  `Client.java:213` 早已 `readShort()` 吃掉开头 2 字节 ⇒ 解析器按日志里的十进制 opcode **补回小端前缀**，
  补完才与客户端 dump 的 `[SEND]` 行同形可比（`tools/pair_logs.py` 的 `MON_RE` 分支）。
- ⚠ **调用点在派发门槛之内**（`Client.java:220` 的 `if (handler != null && handler.validateState(this))`，打印在 :223）
  ⇒ **未注册 opcode 的包、以及 `validateState` 拒掉的包，档 F 一条都不给**。档 A（`Client.java:216`）在同一函数名的**之前**、门槛之外 ⇒ 档 A 能看到档 F 看不到的那部分。
  这与 F4（s9 对未认领 opcode 静默）叠成两层盲区。
- ⚠ **命令必须带参数**：`MonitorCommand.java:43-45` 是 `if (params.length < 1) { yellowMessage(message2); return; }`，
  裸输 `!monitor` 只回用法（真机实测回包 GBK = `输入：!monitor <玩家名>`），**不产生任何 toggling**。
  正解 `!monitor <角色名或角色id>`（`MonitorCommand.java:47-50`：先按名查在线角色，名查不到且是数字则按 id 查），
  生效与否可用 `!monitors`（`CommandsExecutor.java:296` → `MonitorsCommand`，遍历 `getMonitoredChrIds()`）当场自检。
- 方向恒为 c2s（只在 `handleInPacket` 里调用，没有出站对应物）⇒ 出站方向仍需档 B 或客户端 dump。

- 去重：与档 A/B 是镜像关系。真实顺序 `ClientSend`(B) → `收到封包`(A) → 档 F，
  工具只在「前一条是 B 或 A」时折叠档 F，**两条相邻的档 F 绝不互删**（那确实是两个包）。
- 角色名里含 `-` 会让这条正则失配 ⇒ 计入 `unparsed`（不静默丢弃），届时按真数据补规则，别提前猜。

### 3.5 log4j2 行前缀

`gms-server/src/main/resources/log4j2.xml:6-7`：`%d{yyyy-MM-dd HH:mm:ss.SSS} [%5level] ==> %m%n`
⇒ 工具用正则剥掉时间戳/级别，并把时间戳留进报告（`S:L4 2026-10-30 19:20:00.125`）用于跨方向对齐时序。

注意 `log4j2.xml:5` `basePath=logs` 是**相对服务端进程 CWD** 的；实测活动文件为
`E:\project\BeiDou-Server_s9\logs\out.log`。
**`DefaultRolloverStrategy` 里有 `<Delete ... IfLastModified age="1d"/>`（log4j2.xml:32-37）** ⇒ 落进 `logs/` 的证据必须在 1 天内拷进 `evidence/`，否则自动删除。

---

## 4. 配对判据

同方向（c2s / s2c）在单条 TCP 流里顺序确定，因此做**带前瞻窗口的贪心顺序匹配**：

1. 从窗口起点往后找第一条 **opcode 相同**的服务端记录：
   - 字节也完全相同 ⇒ `EXACT`（立即采用）。
   - 只有 opcode 相同 ⇒ 先记为 `DIFF`（附**首个差异字节偏移** + 两侧该字节值 + 两侧长度），继续找窗口内有没有 EXACT，优先 EXACT。
2. 窗口内 opcode 都对不上 ⇒ 该客户端包记 `UNMATCHED-CLIENT`，**不消耗**服务端队列（避免一个丢包把后面全部错位）。
3. 被跳过的服务端记录一律进 `UNMATCHED-SERVER`，报告里逐条给出行号、时间、opcode、原因；PLAN §4 T9 的验收标准就是「配不上的每一条都要解释」。
4. `declared_len != 客户端字节数` 单独作为长度违规上报，并且是默认的非零退出码条件；`--strict` 时连 `DIFF` 也算失败。

前瞻窗口默认 50 包（`--window`）：用来吸收档 A 的 7 个过滤 opcode、日志开关中途切换、客户端 dump 晚于会话启动等造成的错位。

---

## 5. 用法与自检

```bash
# 离线自检（不需要服务端/客户端在线；真实配对日志要等 T7）
python tools/pair_logs.py --self-test

# 真实会话
python tools/pair_logs.py \
  --server  E:/project/BeiDou-Server_s9/logs/out.log \
  --client  E:/MXD/BeiDou-Client-xy/packet_dump.log \
  --report  docs/features/s9-protocol-align/evidence/pairing/<slice>.md
```

- stdout 只输出 ASCII（本仓库的 shell 按 cp936 解 UTF-8，中文会花），中文报告一律 `--report` 落盘。
- 退出码：`0` = 无长度违规；`1` = 有长度违规，或 `--strict` 下存在 `DIFF`。
- 自检覆盖（`--self-test`，2026-10-30 真机会话后仍全绿 rc=0，35 处断言调用）：四种逐包格式各造样例行（客户端 dump、档 B、档 A、档 F）、
  方向映射、小端 opcode、`_` 剥离、`<UnknownPacket>`、两种超阈值截断写法（`AB CD...` / `AB CD ...`）分类、
  非封包行计入 `unparsed`（不静默丢）、档 A/B/F 三方镜像去重（含"两条相邻档 F 不得互删"这条负向用例）、
  档 F 的 opcode 前缀补回与 `<empty>` 退化形态、EXACT/DIFF/UNMATCHED 三条路径、`first_diff` 偏移、`length_check`、
  以及 `run()` 的两种退出码。
- **真机输入验证**（不是自检）：T7 会话 #1 的 56 行新格式 dump 解出 56 条记录、`unparsed=0`、`legacy=0`，rc=0
  （`evidence/pairing/report-20261030-clientonly.md`）。

---

## 6. 已知限制

1. **档 A 看不到 7 个高频 opcode**，也**没有出站**方向。
   档 F 也过滤 7 个，但**名单不同**：`MonitoredChrLogger.java:84` 屏蔽
   `MOVE_PLAYER`、`GENERAL_CHAT`、`TAKE_DAMAGE`、`MOVE_PET`、`MOVE_LIFE`、`NPC_ACTION`、`FACE_EXPRESSION`
   ——比档 A 多屏蔽 `GENERAL_CHAT`/`TAKE_DAMAGE`/`FACE_EXPRESSION`，同时**放行**档 A 屏蔽的
   `HEAL_OVER_TIME`/`SPECIAL_MOVE`/`QUEST_ACTION`（档 A 名单见 `LoggingUtil.java:9-17`，含行内注释的十进制值 41/89/91/107/167/188/197）。
   ⇒ 两档名单互补，**字段级 body 对账首选档 F**（给完整 hex，档 A 的 `toString()` 会把字符串字段换成 `<TEXT>` 而吞掉长度前缀），
   需要覆盖聊天/受击类再叠档 A。
   ⚠ **但交集里那四个谁都没有**：`MOVE_PLAYER(41)`、`MOVE_PET(167)`、`MOVE_LIFE(188)`、`NPC_ACTION(197)` ——
   真机会话 #3 的 774 个 `[SEND]` 里 **759 个是 `MOVE_PLAYER`**，两档全部沉默，一条服务端记录都不会产生。
   移动类与服务端动作类封包的**入站 body 对账只能走档 B**（Netty 层 `InPacketLogger.java:21`，无任何过滤名单），
   而档 B **没有对应 GM 命令**（gm5 的 `DebugCommand` 是地图物件调试，与它无关；`ShowMoveLifeCommand` 只打 `MOVE_LIFE` 的**解码字段**，不是原始 hex）
   ⇒ 只能按已授权流程改库 + 重启（**重启前报备**）。

2. **超阈值的包只能核对 opcode**（入站正文 >3000、出站 >50000 时退化行只留 2 字节），逐字节配对不可行；命中时报告会显式计数。
3. 服务端自发（无对应客户端包）的记录会全部落进 `UNMATCHED-SERVER`，这是**预期行为**，不是缺陷。
4. 工具按**顺序**配对，不认连接边界：一次 `out.log` 里若混了多个角色/多次登录会话，应先用时间戳切段再跑（T7 会做）。
   自 §9.6 起 `pair_stream()` 带**前瞻护栏**（不再越过"后面仍要用的"服务端记录），
   代价是"档位时间窗错位"的行会诚实留在 `um_client`，而不是错配到几十秒后的同名包上。
5. 客户端历史 `packet_dump.log`（无 `[SEND]/[RECV]`）不可用于方向判定，必须用 T9 之后构建的客户端重跑会话。
6. `logs/` 里的日志 1 天后被 log4j2 自动删除 ⇒ 采到证据当轮就拷进 `evidence/pairing/`。

---

## 7. T7 实跑步骤（会话 #1 已完成客户端侧；下面是会话 #2 的服务端侧）

1. ~~客户端 `Settings.ini` 开 `debug.packet_dump=true`~~ ⇒ **已更正并按实际位置做完**：
   键属于部署级 `E:\MXD\BeiDou-Client-xy\xy-config.yaml`（`Settings` 也会被退出时整体重写，不适合放联调开关），
   已在文件末尾追加 `debug:` / `  packet_dump: true`（UTF-8 + CRLF，改 `bd9bf0388acb275f`/2552B → 改后 `d41bdd9d633985e0`/2614B，
   撤销 = 删末尾 4 行）。生效前提：**重启客户端**——`reload_if_changed()` 全仓零调用点，热重载不存在（`client-path.md` §4）。
   产物 exe `2026-10-30 20:08:01` 晚于最新源码 `19:56:39` ⇒ 含 `[SEND]/[RECV]` 方向标记，构建口径不用再验。
   旧的 46 行历史 dump 已改名归档 `packet_dump.log.pre-t7-20261029`（无时间戳、方向未知，留着会与本轮混写）。
2. **从该目录启动** `MapleStory.exe`（CWD 决定 `packet_dump.log` 落点，见 `client-path.md` §4），登录 → 进图 → 走动几步，
   游戏内 GM 角色（实测 `Admin`，`gm=6`）打 **`!monitor Admin`** 开档 F（**必须带角色名/角色 id**，裸输只回用法，见 §3.4），
   再用 **`!monitors`** 自检是否真的挂上（列出当前被监视角色）；
   需要覆盖聊天/受击类再补 **`!showpackets`** 开档 A——注意它是**取反 toggle**（`ShowPacketsCommand.java:39-45`，写入值 = `!当前值`），
   输两次 = 打又关，**一场会话里只输一次**，别按"确认一下"的习惯重复输；
   **前缀必须是 `!` 或 `@`，`/` 会被服务端静默丢弃**（本节第一条）。
   然后走动 / 交互 / 开几个 UI 产生入站包，正常退出。
   ⚠ **动线要"动作多样"而不是"走得久"**：会话 #3 走了 2 分钟，774 个发送里 759 个是 `MOVE_PLAYER`（两档都不打），
   有效配对样本只有 15 个。要的是每类 opcode 各来一次：说话/受击/用道具/捡钱/开商店/对话 NPC/传送/坐椅/升级……
3. 需要出站方向或那 7 个被过滤的 opcode 时，按已授权流程改 `game_config.use_debug_show_packet` 单行 + 重启（**重启前报备**），前后各存一次只读快照，用完改回 `false`。
4. `E:\project\BeiDou-Server_s9\logs\out.log` 与客户端 `packet_dump.log` **当轮**立刻拷进 `evidence/pairing/`（log4j2 一天后删），
   **拷贝前脱敏**：客户端首包是 `0x0001` 登录请求，含明文账号/密码字节 ⇒ 只留前 2 字节或整条剔除（`client-path.md` §3）。
5. 跑 `python tools/pair_logs.py --client … --server …` 出首次真实配对报告；未配对项逐条归因，写入 `notes/opcode-diff.md` 或对应切片笔记。

---

## 8. 会话 #3 实跑结果（2026-10-30 22:30:51–22:32:49，**首次跨向配对成功**）

切片（都按 §7 第 4 条脱敏，剔掉了含明文账号/密码的 `0x0001` 登录行）：
[`client-packet_dump-20261030-2232.redacted.log`](../evidence/pairing/client-packet_dump-20261030-2232.redacted.log)（834 行）、
[`server-out-20261030-2232.redacted.log`](../evidence/pairing/server-out-20261030-2232.redacted.log)（13 行）、
报告 [`report-20261030-2232-paired.md`](../evidence/pairing/report-20261030-2232-paired.md)。

```
client pkts=834  server pkts=7 (trunc=0, dedup=0, unparsed=6)
pairs=7  diff=0  len_mismatch=0  um_client=827  um_server=0     rc=0
```

| # | opcode | 阶段（按服务端会话边界判定，非 opcode 名） | match | 长度 | 备注 |
| --- | --- | --- | --- | --- | --- |
| 1 | `0x000B` | 登录侧 | EXACT | 2 | |
| 2 | `0x0005` | 登录侧 | EXACT | 5 | |
| 3 | `0x0013` | 登录侧 | EXACT | 48 | 含机器标识字符串 |
| 4 | `0x0014` | 频道侧 | EXACT | 6 | |
| 5 | `0x00CF` | 频道侧 | EXACT | 2 | |
| 6 | `0x0031` | `GENERAL_CHAT("!monitor")` | EXACT | 13 | 命令体是 ASCII，长度前缀 + GBK 逐字节一致 |
| 7 | `0x0031` | `GENERAL_CHAT("!showpackets")` | EXACT | 17 | 同上 |

**这一轮把 T7 的配对格子从"零"推到"通"**：7 条服务端入站记录与客户端 `[SEND]` **全部逐字节 EXACT**，
`diff=0 / len_mismatch=0`，服务端侧无残留（`um_server=0`）。`unparsed=6` 是 `out.log` 里的非封包行（连接/存档），按设计计入不静默丢。
第 6/7 对顺带给 `charset.md` §0 再加一条实测：出站 `write_string`（2 字节长度前缀 + GBK）与 s9 `readString()` 在真机上同形同长。

**未配对 827 条的构成，正好量化了 §6 第 1 条的盲区**：774 个 `[SEND]` 里 **759 个是 `MOVE_PLAYER(0x29)`**，
档 A 与档 F 的过滤名单都含它 ⇒ 服务端一条都不会打。剩余 15 个发送中 7 个已配对，其余是 `0x3C`×4 / `0x28`×2 / `0x18`×1（待归因）。

**档位语义的三条真机定论**（都可复核）：
1. **档 A = 纯内存、即时生效、跨连接存活、进程重启即回库值**。`!showpackets` 走 `ShowPacketsCommand.java:39-45`
   → `GameConfig.update()`，而 `GameConfig.java:73-79` 只 `valueProp.put("value", …)` 改缓存 JSONObject，**没有任何 DAO/SQL**；
   `getServerBoolean()`（`GameConfig.java:434-439`）读的也是同一份缓存 ⇒ 免重启。
   库侧只读快照印证：`game_config` 里 5 个 `use_debug*` 键**全为 `false` 且 `update_time IS NULL`**（从未被写过）。
   ⚠ 我一度据"20:59 开的、22:30 还在打"判定它写库，**是错的**——JVM 全程没重启（PID 40688）就足以解释存活。
2. **档 A 是取反 toggle**：20:59:16 开（当场之后客户端立刻崩了，没有后续包 ⇒ 零行不代表没开）；
   会话 #3 从 22:30:53 起在打（沿用 20:59 的 ON），到 **22:31:34 用户又输一次 ⇒ 当场翻回 OFF**，之后 75 秒 Gameplay 零记录。
   这解释了"只有 7 条"，不是档位失效。
3. **档 F 未被触发**：`!monitor` 缺参数，服务端回的是用法提示。客户端 dump 里紧跟其后的 RECV 行
   `4d 00 ff 17 00 ca e4 c8 eb a3 ba 21 6d 6f 6e 69 74 6f 72 20 3c cd e6 bc d2 c3 fb 3e 00 00`
   = opcode `0x004D`，正文从第 4 字节起按 GBK 解出 `输入：!monitor <玩家名>` ⇒ 与 `MonitorCommand.java:43-45` 的 `params.length < 1` 分支对上；
   `0x004D` 的字段归属（`ff` 与 `17 00` 是什么）留到切片对账，不在这里猜。⇒ **§9.2 已闭合**。

---

## 9. 会话 #5 实跑结果（2026-10-31 00:54:45–00:59:22，**档 F 首份真数据 + 首个字段级配对**）

会话 #3 的 `!monitor` 因缺角色名参数而没挂上监视（服务端只回用法，见 §8 第 3 条）；会话 #4 挂上了 chrId=1，
但窗口内入站全落在屏蔽表里 ⇒ 前两档合起来仍是 0 条真记录。本轮带上了角色名
（`MonitorCommand.java:39-56` 需要 name/id），`!monitors`（`MonitorsCommand.java:38-42`）自检通过且客户端不崩
（⇒ #26 验证点 A/B 双通过），才拿到第一批真记录。

切片（档 C 起点 = `packet_dump.log` 第 2599 行，服务端起点 = `out.log` 第 109 行，时刻 00:54:45）：
[`client-packet_dump-20261031-0055.monitor-f1.log`](../evidence/pairing/client-packet_dump-20261031-0055.monitor-f1.log)（27 行：14 SEND / 13 RECV）、
[`server-out-20261031-0055.monitor-f1.log`](../evidence/pairing/server-out-20261031-0055.monitor-f1.log)（13 行）、
报告 [`report-20261031-0055-monitor-f1.md`](../evidence/pairing/report-20261031-0055-monitor-f1.md)。

```
client pkts=27  server pkts=11 (trunc=0, dedup=0, unparsed=2)
c2s: pairs=11  EXACT=11  DIFF=0  len_mismatch=0  um_client=3  um_server=0     rc=0
```

### 9.1 c2s 字段级配对（第一次拿到，4 个非心跳包全部逐字段对齐）

| opcode | 客户端写出 | 真机字节（正文，不含 opcode） | s9 读入 | 判定 |
| --- | --- | --- | --- | --- |
| `0x78` WHISPER | `write_byte(6)`+`write_string(recipient)`+`write_string(text)`（`MessagingPackets.h:73-82`，字节在 `:79`） | `06 05 00 41 64 6D 69 6E 0B 00 77 68 69 73 70 65 72 2D 74 32 36` | `WhisperHandler.java:49-50` `readByte`+`readString`，case `WHISPER\|REQUEST`(=0x02\|0x04=6，`PacketCreator.java:5757-5764`) 内 `:63 readString` | **EXACT / 逐字段一致** |
| `0x78` WHISPER | `write_byte(5)`+`write_string(name)`（`MessagingPackets.h:85-93`，字节在 `:90`） | `05 05 00 41 64 6D 69 6E` | case `LOCATION\|REQUEST`(=0x01\|0x04=5) `:59-60` | **EXACT** |
| `0x77` MULTI_CHAT | `write_byte(type)`(`:51`)+`write_byte(recipients.size())`(`:52`)+N×`write_int`+`write_string`(`:57`)（`MessagingPackets.h:38-58`） | `02 00 09 00 67 75 69 6C 64 2D 74 32 36` | `MultiChatHandler.java:44-53` `readByte` type → `readByte` numRecipients → N×`readInt` → 正文 | **EXACT**，type=2=GUILD 两侧枚举同序（客户端 `BUDDY,PARTY,GUILD,ALLIANCE` ↔ s9 行内注释 "0 buddies, 1 parties, 2 guilds, 3 alliances"） |
| `0x77` MULTI_CHAT | 同上 | `01 00 09 00 70 61 72 74 79 2D 74 32 36` | 同上，type=1=PARTY | **EXACT** |
| `0x18` PONG | 空正文（`OutPacket.cpp:41`） | `<empty>` | `KeepAliveHandler`，`Client.java:223` 打的是 `packet.getBytes()` ⇒ **opcode 已被 `readShort()` 吃掉**，所以档 F 的 hex 天生只含正文 | **EXACT ×7** |

两点顺带钉死：① 客户端 `MultiChatPacket` 传的是**空收件人列表**（`00`），s9 `:47` 的
`numRecipients > available/4` 护栏对 0 恒真，于是走服务端自己的成员广播 ⇒ 本轮 `/g`、`/p` 都可见，不算错位；
② `GENERAL_CHAT(0x31)` 两条（`!monitor Admin` / `!monitors`）在报告里是 `um_client`，
**不是没发**，是 `MonitoredChrLogger.java:84` 把 `GENERAL_CHAT` 屏蔽了（§6 第 1 条），会话 #3 的档 A 已 EXACT 过同两种包体。

### 9.2 s2c 侧：本轮无出站档位 ⇒ 改用「真机字节 ↔ s9 源码」逐字段对账

档 F 只打**入站**（`Client.channelRead`），档 E/B 未开 ⇒ 报告里 s2c `pairs=0` 是**档位设计如此**，不是缺口。
13 条 RECV 全部就地归因（8×`0x11` + 2×`0x4D` + 3×`0x87`），按形状共 5 组，逐组与 s9 源码完成字段级对账：

| RECV | 真机字节 | s9 写出 | 结论 |
| --- | --- | --- | --- |
| `0x11` PING ×8 | `11 00` | `PacketCreator.java:636-637 getPing()`，由 `Client.java:1182-1185 checkIfIdle(IdleStateEvent)` 触发 | 空正文 EXACT；**8 发 7 收的缺口见 §9.4** |
| `0x4D` ×2 | `4d 00 ff 0a 00 31 20 d2 d1 b1 bb bc e0 bf d8 00 00` / `4d 00 ff 0e 00 41 64 6d 69 6e 20 d2 d1 b1 bb bc e0 cc fd 00 00` | `sendYellowTip()`（`PacketCreator.java:5437-5443`）= `writeByte(0xFF)`+`writeString(tip)`+`writeShort(0)` | **逐字段闭合**：`ff`=常量、`0a 00`/`0e 00`=2 字节小端串长、尾 `00 00`=`writeShort(0)`。§8 第 3 条留的问号到此清零 |
| `0x87` ×3 | `87 00 12 05 00 41 64 6d 69 6e 00 01 0b 00 whisper-t26` | `getWhisperReceive()`（`:5799-5807`）byte(`WHISPER\|RECEIVE`=0x02\|0x10=0x12)+string(sender)+byte(channel)+bool(fromAdmin)+string(msg) | **EXACT**：channel=0、fromAdmin=1（GM 发出） |
| `0x87` | `87 00 0a 05 00 41 64 6d 69 6e 01` | `getWhisperResult()`（`:5791-5796`）byte(`WHISPER\|RESULT`=0x0a)+string+bool | **EXACT**，success=1 |
| `0x87` | `87 00 09 05 00 41 64 6d 69 6e 02 ff ff ff ff` | `getFindResult()`（`:5776-5787`）byte(`RESULT\|LOCATION`=0x09)+string+byte(type)+int | **EXACT**，type=2=`RT_CASH_SHOP`（`WhisperHandler.java:44`）+`fieldOrChannel=-1` ⇒ 走 `:78` 的 `target.getCashShop().isOpened()` 分支；`type!=1` 故无 x/y，客户端 `MessagingHandlers.cpp:747` 的 `if (type == 1)` 判断与 s9 `:5783` 的 `RT_SAME_CHANNEL(=1)` 完全一致 |

⇒ **`WHISPER_RESULT(0x87)` 的三种形状 + `WEEK_EVENT_MESSAGE(0x4D)` 可以勾掉 P2 的对应格子**，
客户端解析器 `WhisperResultHandler`（`MessagingHandlers.cpp:704-755`）与 `WeekEventMessageHandler`（`:481-502`）字段序均无偏差。

### 9.3 会话 #3 遗留三个未配对 opcode 的归因（`0x18` / `0x28` / `0x3C`）

三个都**不是协议错位**——opcode-matrix 里 `status=MATCHED` 且 s9 有注册 handler，缺的是"服务端当时没打日志"：

1. `0x18 PONG` ⇒ **本轮正面闭合**：空正文，7 条 EXACT 配对（§9.1）。
2. `0x28 ENTER_CASHSHOP`（会话 #3 的 L304/L310）：档 A 在 **22:31:34 被 `!showpackets` 翻回 OFF**（§8 第 2 条），
   而 40 不在 `LoggingUtil.java:9-17` 的 7 项名单里 ⇒ 当时**没有任何档位能记录它**。
   字段级预对账已完成：客户端正文为空（`GameplayPackets.h:43-47`，`EnterCashShopPacket() : OutPacket(ENTER_CASHSHOP) {}`），
   s9 `EnterCashShopHandler.java:37-60` **一个字段都不从 `p` 读**（只用 `c.getPlayer()`）⇒ 空正文正确，无缺字段。
3. `0x3C NPC_TALK_MORE`（会话 #3 的 L827-L833，正文 `00 01`/`00 00`）：同上，档 A 已关。
   字段级预对账：客户端 `write_byte(lastmsg)`+`write_byte(response)`（`NpcInteractionPackets.h:40-44`），
   派生构造 `(2,1)+write_string`（`:46-49`）、`(4,1)+write_int`（`:51-54`）、`(3,1)+write_int`（`:59-64`）；
   s9 `NPCMoreTalkHandler.java:36-37` 先 `readByte` lastMsg、`readByte` action，
   `lastMsg==2` 才 `readString`（`:41`），否则按 `available()>=4 → readInt` / `>0 → readUnsignedByte`（`:60-63`）
   ⇒ 四条客户端形状全部落在 s9 的读取分支内，`00 01`（lastMsg=0 → else → available=0 → selection=-1）也对得上。
   ⚠ 唯一需要留意的**语义**差：s9 用 `lastMsg` 判"上一屏类型"，客户端把 `(4,1)` 当"选项"、把 `(3,1)` 当"数字"，
   而 s9 对 3/4 都走 `else` 的 `readInt` ⇒ 结果一致，不构成错位；`/find` 之类要扩 lastMsg 语义时再议。

⇒ **仍要真机补的两条**：`0x28`、`0x3C` 的正面对账，得在**档 A 开着**或**档 F 挂上**的窗口里点一次商城、答一次 NPC 对话。

### 9.4 心跳缺口的解释（8 发 PING，只配到 7 条 PONG）

原始 `out.log`（非切片，`logs/out.log` 第 113-118 行）确实只有 `00:56:12` 与 `00:57:22` 之间没有 `24-<empty>` ⇒ 不是切片掉的。
根因两条，都能复核：
- **PING 不是 30 秒 cron，是 Netty 空闲事件**：`Client.java:1182-1185 checkIfIdle(IdleStateEvent)` 才 `sendPacket(getPing())`，
  任何读写活动都会重置空闲计时 ⇒ 00:56:34–00:56:52 的四条密语/喇叭把相位推到了 `00:57:22`（= 最后一次活动 +30s），之后恢复 30 秒节奏。
- 客户端切片起点（00:54:45）**早于**档 F 挂上的时刻（服务端 `MonitorCommand` 执行 = 00:55:05，监控集是内存里的 `monitoredChrIds`，不落库），所以最前面那对 `11/18` 服务端不可能有记录。
⇒ 报告里 C:L10 的 `um_client` 是**档位时间窗**造成的，不用改代码。

### 9.5 中文链路的第一条硬证据（喂给 #21 T11 / #22 T12，并**修正了我自己中途写下的错判**）

`s9` 在真机上**确实发了中文**：`0x4D` 正文里 `d2 d1 b1 bb bc e0 bf d8` 按 GBK = `已被监控`、`… cc fd` = `已被监听`，
完整串是 `1 已被监控` / `Admin 已被监听`。客户端实际只显示 `[Notice] 1`
（截图 [`evidence/chat/notice-cjk-blank-20261031.png`](../evidence/chat/notice-cjk-blank-20261031.png)：两行 `[Notice] 1` 后面**什么都不剩**，既不是方块也不是乱码）。

逐层排除：
1. **解码层没丢**（⚠ 本条描述的是 **T11 之前** 的行为：当时 `read_padded_string` 只做逐字节 `push_back`，末尾没有 `packet_to_internal`；T11 上线后同一函数在 `Net/InPacket.cpp:105` 转成 UTF-8 才返回，见 `notes/charset.md` §7.4 + `evidence/charset/t11-live-20261031.md`）：`InPacket::read_padded_string`（`Net/InPacket.cpp:88-106`）逐字节 `push_back`，GBK 高位字节原样进 `std::string`；
2. **拼装层没丢**：`WeekEventMessageHandler`（`Net/Handlers/MessagingHandlers.cpp:486-490`）直接 `"[Notice] " + message`，再 `:500-501` 交给 `UIChatBar::show_message`；
3. **渲染层不是"没有中文字形"**：部署目录带 `fonts/SimHei/simhei.ttf`，由 `Configuration.h:517-525`（`font.path.normal/bold`）加载，
   `GraphicsGL.cpp:298` 显式 `FT_Select_Charmap(face, FT_ENCODING_UNICODE)`，`:390 get_wide_glyph()` 按码点取 CJK 字形，
   `:1299-1328` 是 **UTF-8 → 码点** 的解码循环 ⇒ 同一张截图里聊天目标按钮的 `对所有人` 正常显示，**证明 CJK 渲染通路已经在工作**。
4. ⇒ **丢点 = 渲染层"假定输入是 UTF-8"而封包层给的是 GBK**：`d2 d1` 被 `:1311` 判成"2 字节序列"（`extra=1`），
   按 `:1319-1322` 拼出码点 `U+0491`（西里尔字母，非中文），`get_wide_glyph` 取不到字形 → `:1328 continue` → **整对字节静默消失**。
   这与"没有方块"的观测完全一致。

⚠ **自我修正留痕**：我本节第一稿写的是"丢在渲染层 ⇒ 归 #22 T12"，**错了**——是因为没看部署目录里的 SimHei 与 `GraphicsGL` 的 UTF-8 解码循环。
正确分工是：**#22 T12（CJK 渲染）事实上已完成并被真机反证**（资源串 `对所有人` 可显示），
**#21 T11 才是唯一缺口**：在封包边界做 `GBK ↔ UTF-8` 转换（读侧 `read_string`、写侧 `write_string`），
渲染层不需要动。附带好处：转换后 `:1316` 的越界 `break` 与 `:1313` 的 `continue` 分支都不会再被 GBK 喂进错误码点。

⇒ 顺带解释了为什么"输入预留"是必要的：`IO/Window.cpp:709-742 begin_ascii_text_input` 目前主动关 IME + `SDL_StopTextInput`，
输入侧要等 T11 落地后再接，否则会往 UTF-8 管线里灌 GBK。

### 9.6 配对器改动：前瞻护栏（`tools/pair_logs.py`）

改前这份窗口跑出来是 `pairs=7 / um_server=4`——4 条本该 EXACT 的档 F 记录被判成"服务端未配对"，
原因是纯贪心：C:L10 那条 PONG 向前扫到 60 秒后的 S:L9 并**越过**了中间的 S:L5-L8，指针再也回不去。
`pair_stream()` 现在在向前扫时加护栏：**遇到 opcode 不匹配、但该 opcode 在后续未处理客户端行中还会出现，就停止前扫**
（`tools/pair_logs.py:296-326`，附改动理由注释）。改后 `pairs=11 / um_server=0`，且：
- `--self-test` 全绿；
- 回归会话 #3 那份 834×7 窗口：`pairs=7 / um_client=827 / um_server=0`，**与改前逐字一致** ⇒ 只修误配，不动已配。
⚠ 副作用记录：护栏会让"服务端确实没记到"的客户端行留在 `um_client`（本轮 C:L3/L5 两条 `GENERAL_CHAT` 就是），这是**想要的**表现。




### 9.7 会话 #6 追加：§9.5 那句"T12 事实上已完成"**只对了一半**

§9.5 的结论是拿截图反证的：同屏 `对所有人` 显示正常 ⇒ CJK **字形通路**可用。这话本身没错，但我在 T11 落地后
顺着"转换出的 UTF-8 到底怎么被排版"读了一遍 `GraphicsGL::LayoutBuilder`，发现**字形通了、度量没通**：

| 位置 | 事实 | 后果 |
| --- | --- | --- |
| `Graphics/GraphicsGL.cpp:1142-1149` | `wordwidth` 只对 `uc >= 32 && uc < 128` 累加，非 ASCII 字节贡献 0 | 纯中文串永不触发 `:1151` 的"过长就地拆分" ⇒ **长中文文本只横向溢出，不换行** |
| `:1184-1198` + `:1187` | 每个非 ASCII 字节按 `font.chars[' ']` 计推进量 | 一个汉字 = 3 字节记成 3 个空格宽，而 `:1299-1333` 实际只画 1 个字形 ⇒ 中文之后的 word 起点、`layout.width` 双双偏小 ⇒ **居中/裁切/气泡与 yellow tip 背景宽度对不上** |
| `:1287` | 词首取 `layout.advance(word.first)`（来自上面那张表），词内按字形累加 | 所以**词内**是对的，错在**词间**与总宽——这正是短串看不出来的原因 |

⇒ 短消息（`[Notice] 1 已被监控`、按钮上的 `对所有人`）落在"单个短词 + 不需要换行"的舒适区，所以 §9.5 的真机反证
**不足以关闭 T12**。T12 已按此重新表述（PLAN §4 P1 表），拆出任务 **#28 T12b**，与 T11 分开提交：
它动的是全仓文本度量，英文 UI 布局必须一起回归，不该混在封包层的改动里。

⚠ 留痕：这是本次会话内的第二次自我修正（第一次是 §9.5 把丢点错判成渲染层）。共同根因是**用一次屏幕观测代替了对代码路径的逐层读**——
观测能证伪"没字形"，证不了"度量正确"。
