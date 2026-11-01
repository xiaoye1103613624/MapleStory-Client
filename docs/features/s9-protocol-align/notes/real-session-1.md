# 真机会话 #1（T7 首轮取证，2026-10-30 20:27–20:29）

## 0. 采集元数据（可回溯）

| 项 | 值 |
| --- | --- |
| 客户端 exe | `E:\MXD\BeiDou-Client-xy\MapleStory.exe`，构建 `2026-10-30 20:08:01`，sha256 前缀 `f735afdf09b473c9` |
| 原始 dump | 同目录 `packet_dump.log`，11493 字节 / **57 行**，sha256 前缀 `4b2ddb6e59165fcc` |
| 仓库内副本 | `evidence/pairing/client-packet_dump-20261030-2029.redacted.log`（**56 行**，剥掉了 L1） |
| 剥 L1 的原因 | 那是 `0x0001` 登录请求，正文含**明文账号/密码字节**，不进仓库（`client-path.md` §3 的硬约束） |
| 会话窗口（服务端日志） | `20:27:47` 连接登录端口 → `20:27:57` 连接大区 0 频道 1 → `20:29:39` 保存角色 `Admin`（退出） |
| 服务端 | PID 40688 存活，本轮**未重启**；`logs/out.log` 当日 215 行（sha 前缀 `506cb1a92b5d9b93`），窗口内只有上面 3 行 + 事件脚本/自动保存行；**包日志类记录 0 条**（`收到封包`=0、`ClientSend`=0、档 F 行式=0）。会话边界 27 行已抽存 `evidence/pairing/server-out-20261030-2029.session-lines.txt`（log4j2 一天后会删 `out.log`） |
| ↳ 编码判定 | `out.log` **strict UTF-8 解码通过、GBK 解码抛异常** ⇒ 上面那几个 0 是在正确解码下数的，不是 cp936 终端造成的假阴性 |
| 配对报告 | `evidence/pairing/report-20261030-clientonly.md`，`pair_logs` rc=0，**56 包解析、unparsed=0、截断=0** |

⇒ `pair_logs.py` 第一次跑真数据：解析器对 T9 之后的新格式（`[SEND]/[RECV]`）零失配。这是 T9 "工具正确性"格子外的第一份真实输入验证；**跨向配对仍是 0 对**，因为服务端侧还没有日志（见 §4）。

## 1. 本轮抓到的东西按阶段分类（12 c2s + 44 s2c）

| 阶段 | 包（客户端行号 = dump 原始行序，L1 已剥） |
| --- | --- |
| 登录 | SEND `0x0001`(登录请求，已剥) → RECV `0x0000 LOGIN_STATUS`(47B，含 `admin`) → SEND `0x000B SERVERLIST_REQUEST` → RECV `0x000A SERVERLIST`(79B) + `0x000A FF`(结束标记) |
| 世界/角色 | SEND `0x0005 CHARLIST_REQUEST` → RECV `0x001A`、`0x001B`、`0x000B CHARLIST`(153B，角色 `Admin`) → SEND `0x0013 CHAR_SELECT`(48B，含 MAC `DC-32-62-CF-1C-22` 与机器串 `A085272FD3B0_282CAA78`) → SEND `0x0014 PLAYER_LOGGEDIN` |
| 进频道 | RECV `0x000C SERVER_IP`(19B，`7F 00 00 01` + 端口 `1D 97`=7575) → `0x0044` → `0x007D SET_FIELD`(**617B**，角色全量) → `0x0090 ADMIN_RESULT` ×2 → `0x001F STAT_CHANGED` → `0x014F KEYMAP`(453B) → `0x009F`/`0x007C` → `0x0150/0x0151` 自动药水 → `0x00DE/0x00DD` UI 锁 |
| 地图内容 | `0x008A FIELD_EFFECT`(maplemap/enter/10000) → `0x0101 SPAWN_NPC` + `0x0103 ..._REQUEST_CONTROLLER` 成对，共 3 只 × **两轮完全相同**（L25–30 与 L31–36 逐字节一致，登记为待查） |
| 系统 | **RECV `0x0023 FORCED_STAT_RESET`** → `0x003F BUDDYLIST` ×2 → `0x0064 FAMILY_PRIVILEGE_LIST`(1282B) → `0x005F` → `0x003A SET_GENDER` → `0x002F CLAIM_STATUS_CHANGED` → `0x0024 UPDATE_SKILLS` → `0x007A SCRIPT_PROGRESS_MESSAGE` → `0x0107 SET_NPC_SCRIPTABLE` |
| 交互/收尾 | SEND `0x00CF PLAYER_MAP_TRANSFER`(2B，进图完成确认) → SEND `0x0029 MOVE_PLAYER` ×4（走动，坐标 `05 3A`→`06 3A`）→ PING/PONG 两轮 → SEND `0x0031 GENERAL_CHAT`(17B) |

## 2. 四条硬结论

**① 传输层与握手在真机上全通。** 客户端一次性走完登录→选角→进频道→进图→移动→正常退出，
44 个 s2c 包全部被客户端正确解密并按 opcode 派发（`SET_FIELD` 617B、`KEYMAP` 453B、`FAMILY` 1282B 都拿到了），
13 个 c2s 包服务端全部受理（会话没被打断、没有 F6 的"聋"症状）。
⇒ `transport-vectors.md` 的 D8 修复 + `Session.cpp` 长度守卫在真机链路上无副作用。

**② 结构事实：`packet_dump.log` 里永远不会有 hello。** hello 是 4 字节纯头部、无正文，
而 `Session.cpp` 的 dump 调用点在正文组装之后 ⇒ 想用 dump 取握手是取不到的。
握手证据的正确出处是探针：`evidence/probe/probe-t7-state3.*` 的 hello 与 oracle 逐字节一致
（`0e 00 53 00 01 00 31 46 72 7a ** 52 30 78 ** 08`），真机侧则以"①会话完整走通"为凭。
⇒ PLAN §4 T7 的握手格子按这个口径收口，不再等 dump。

**③ T6/D1 的修复拿到真机验收。** dump L37 是 **RECV `0x0023` = `FORCED_STAT_RESET`**，
出现在进图序列里 ⇒ 坐实"s9 在 35 这个值上发的是 `FORCED_STAT_RESET`"（`opcode-diff.md` §4 第 5 项的前提）。
客户端侧 `emplace<FORCED_STAT_RESET, ForcedStatResetHandler>`（`PacketSwitch.cpp:640`）保留、
`emplace<RECALCULATE_STATS, ...>` 已注销 ⇒ 该包被正确派发且角色属性正常显示。

**④ 中文链路拿到真机佐证（但不改变 `charset.md` 的结论边界）。** 三个 s2c 包里的中文串按 **GBK** 完全可读：
- `0x007A`：`B5 C7 C2 BC C1 CB D3 CE CF B7` = **登录了游戏**（前缀 `Admin Admin `）
- `0x0107`：`C4 A2 B9 BD CD F5 B9 FA` = **蘑菇王国**（地图名，后面 `Rescue Gaga!` 是 ASCII）
- `0x0064 FAMILY_PRIVILEGE_LIST`：`D6 B1 BD D3 D2 C6 B6 AF B5 BD D1 A7 D4 BA B3 C9 D4 B1 C9 ED B1 DF` = **直接移动到学院成员身边**（家族特权文案整表 1282B）

⇒ 服务端**出站字符串 = GBK** 得证；但**不能**据此说"账号语言参与了编解码"：本部署账号 `language=3` 与
`SERVICE_LANGUAGE=zh-CN` 都映射 GBK，两条路字节相同（`charset.md` §0/§3 的边界原文有效，DoD 那行不用改）。

## 3. `/showpackets` 没生效：根因是**命令前缀**，不是封包

真机证据链：
1. 客户端**确实发了**：dump 最后一行 `31 00 | 0c 00 | 2f 73 68 6f 77 70 61 63 6b 65 74 73 | 01`
   = `GENERAL_CHAT(49)` + `write_string("/showpackets")` + `write_byte(show)`（`Net/Packets/MessagingPackets.h:29-34`）。
2. 服务端**解析是对的**：`ByteBufInPacket.readString()` 是 `readShort()` 长度 + 按长度取字节（`:51-57`），
   与客户端 `write_string` 的"短长度 + 单字节串"完全对齐 ⇒ 解出 `s = "/showpackets"`。
3. **卡在命令判定**：`GeneralChatHandler.java:53` 调 `CommandsExecutor.isCommand(c, s)`，
   而 `CommandsExecutor.java:53-54` 写死 `USER_HEADING='@'`、`GM_HEADING='!'` ⇒ `'/'` 不是命令头，`isCommand` 返回 false；
   紧接着 `:55` 的 `else if (heading != '/')` 又因为首字母**正是** `/` 而落空 ⇒ **整条消息既不执行命令也不广播，静默丢弃**。
   这与 dump 里"发完聊天后客户端再没收到任何包"完全吻合。
4. 命令本身是可用的：`command_info`（库 `beidou`，即 `application.yml:18` 的数据源）里
   `syntax='showpackets' level=5 enabled=1 clazz=ShowPacketsCommand`，角色 `Admin` gm=6 ⇒ 满足门槛；
   且 `ShowPacketsCommand.java:39-44` → `GameConfig.update()` 纯内存，不写库、免重启（与 `log-pairing.md` §2 一致）。

⇒ **正确姿势是输入 `!showpackets`**（GM 用 `!`，普通玩家 `@`；GM 角色两者都收）。
⇒ 用户看到的"聊天 UI 有问题"至少有一半是这个：以 `/` 开头的消息在服务端被静默吞掉，界面上不会有任何回显。
   这条**不是客户端缺陷**，不用改代码；但客户端把参考端的 `/` 习惯带过来是文档层面的坑，记在这里。

## 4. "档 A 到底开没开"的判定方法（下一轮可直接复用）

`/showpackets` 是内存开关、服务端进程还活着 ⇒ 状态可以从外部探测，无需再依赖真机：
- 向登录端口 `8484` 发 **两个 s9 未注册的 opcode**（`0x0352/0x0353`，`probe --send 352,353`），
  档 A 的打印点在 `Client.java:216-218`，位于 handler 查找**之前**（F4），所以"没人认领的包"也照样会打印。
- **负结论要成立必须先证明包真的走到了派发点**：同一次运行里跑完 PING→PONG→活过 20s 观察窗
  （`probe-t7-state3.txt` 三条 `[OK]`），⇒ `Client.handleInPacket` 被进入 ⇒ 打印点被经过。
- 实测：`grep -c 收到封包 out.log` = **0** ⇒ **档 A 当前是关的** ⇒ `/showpackets` 从未生效（与 §3 一致）。
- 反向可用性：`tools/probe/opcode_blocklist.txt` 把 `0x0003` 值级否决挡住了（T8 的门禁设计生效），
  第一次尝试 `--send 3` 是 `SKIP(blocklist)`，改用未注册值才发出去。

## 5. 待查（不在本轮结论里，留给 S 切片）

1. `SPAWN_NPC(257)` + `259` 的三组包**逐字节重复了两轮**（L25–30 vs L31–36）。要么服务端真发了两遍，
   要么我们的 dump 调用点被走了两次——两种都要查，因为关系到 NPC 会不会重复渲染。
2. `SET_FIELD` 里 `d3 cf e5 8f 98 ce 5c 75 f6 65 fd 9c` 这段：GBK 解成 `酉鍙樜\u鰁郎`、UTF-16LE 解成 `CFD3/8FE5/...`，
   两边都不像文本 ⇒ 判定为**非文本字段**（哈希/加密块），字段归属等 `SET_FIELD` 逐字段对账时定，不在这里猜。
3. `0x0107 SET_NPC_SCRIPTABLE` 里 `FF FF FF 7F` 与后续 8 字节重复出现——`SPOUSE_CHAT` 之类不相关，
   但它和 `0x0090 ADMIN_RESULT` 一起出现，说明 GM 身份相关的包还没对过账。
4. 跨向配对（本轮 pairs=0）等下一轮服务端日志。首选 **`!monitor`（档 F）**：即时生效、给完整 hex、不碰 `GameConfig` 也不写库，
   比 `!showpackets`（档 A 的 `<TEXT>` 会吞掉字符串字段）更适合字段级对账；`tools/pair_logs.py` 已支持其行式（`log-pairing.md` §3.4）。
5. **会话 #2（20:59）没拿到档 F 数据，别当成"档 F 已验证"**：dump 里根本没有 `!monitor` 包，客户端在采样前因
   classic HUD 聊天栏的空指针闪退（`0xc0000005`）。取证与修复见 [`crash-chatbar.md`](crash-chatbar.md)，动线要求见 PLAN §8 的会话 #2 留痕。

