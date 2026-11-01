# T8 · 在线协议探针（`tools/probe/s9_probe.py`）

对**正在运行的 s9** 发真包，验证 T4 的 `transport_oracle.py` 不是"两边各自转写、错得一样也对得上"的纸面自证。
本轮（2026-10-30）已跑通并给出五条实测结论；其中第 5 条**推翻了探针最初设计的判据**，是本篇最重要的内容。

- 脚本：`tools/probe/s9_probe.py`（复用 `tools/transport_oracle.py`，**探针内不重写任何编解码**）
- 门禁清单：`tools/probe/opcode_blocklist.txt`（89 条）、`tools/probe/opcode_allowlist.txt`（1 条）
- 原始证据：`evidence/probe/probe-final.{txt,hex,json}`、`evidence/probe/netty-ByteToMessageDecoder-channelRead.txt`、`evidence/probe/outlog-excerpt.txt`

---

## 1. 用法

```bash
python tools/probe/s9_probe.py --dry-run                                   # 不联网，只验门禁与发送计划
python tools/probe/s9_probe.py                                             # 握手 + 探测 + 一轮 PING/PONG（约 70s）
python tools/probe/s9_probe.py --no-ping                                   # 只测 c2s，不等 PING
python tools/probe/s9_probe.py --ping-rounds 1 --length-probe --negative-control   # 全套（约 180s）
```

默认目标 `127.0.0.1:8484`（`application.yml:69` 的 login-port）。前置条件只有一个：服务端在跑。

**退出码 = 未通过的门禁断言条数**（0 = 全绿）。negative control 的结论走 `rep.measure()`，不计入退出码 —— 它推翻假设是**产出**，不是探针失败；否则退出码会永远红着，反而掩盖真缺陷。

每次运行落三份证据到 `evidence/probe/`：`.txt`（人读）、`.hex`（逐包原始字节，IV 随机字节以 `**` 脱敏）、`.json`（结构化 checks/measurements/目标清单/被拦清单/观察到的 s2c 长度）。

## 2. 安全模型：三层闸门，命令行绕不过去

| 层 | 规则 | 依据 |
|---|---|---|
| 1 | 只发 **s9 侧没有注册 handler** 的 recv opcode ⇒ `Client.channelRead()` 走 null 分支，只 `updateLastPacket()`，对账号/角色/世界/DB 零影响 | `Client.java:207-236` |
| 2 | `opcode_blocklist.txt` 里的值**第二道否决**，即使满足第 1 层（例如 `0x0044 ADMIN_SHOP`、`0x0054 USE_TELEPORT_ROCK` 本来无 handler，仍按名拦掉） | 清单文件头写明三条硬规则 |
| 3 | `opcode_allowlist.txt` 是唯一豁免通道，当前只有 `0x0018 PONG`：`KeepAliveHandler` 只做 `c.pongReceived()` 写一个 volatile long（`Client.java:1179-1181`），`validateState()` 恒 true | `PacketProcessor.java:139` |

清单的**名字 ↔ 值**每次运行都现场用 `gen_opcode_matrix.py` 的解析器对着 `RecvOpcode.java` / `PacketProcessor.java` 复核，对不上直接 `ABORT`（rc=99）。这条不是形式主义：dry-run 第一轮就抓到 blocklist 里写了 `LOGIN_EMAIL` —— s9 根本没有这个常量（它是客户端 `OutPacket.h` 的枚举名），值抄对了但名字是幻觉。**清单会漂移，只有交叉校验能发现。**

当前实测的闸门输出：

```
# 门禁清单校验通过：blocklist=89 allowlist=1
# s9 RecvOpcode 常量=181  有效 handler=172
# 计划发送 7 个 opcode：
#   0x000C PLAYER_DC / 0x0019 CLIENT_START_ERROR / 0x001A CLIENT_ERROR
#   0x00D6 MATCH_TABLE / 0x00ED CLOSE_ITEMUI / 0xFFFE MAPLETV   （均 no-handler）
#   0x0018 PONG                                                （allowlist:KeepAliveHandler）
# 被拦下 2 个：0x0044 ADMIN_SHOP / 0x0054 USE_TELEPORT_ROCK     SKIP(blocklist)
```

`0xFFFE MAPLETV` 能发是因为 `PacketProcessor.getHandler()` 对负数/越界返回 null（`RecvOpcode` 值域内无该注册），不是"我们确认它安全"。

探针**不做登录、不建 session、不碰数据库**。全程只连 login port，最多活到被空闲踢掉。

## 3. 五阶段与实测结果

`python tools/probe/s9_probe.py --ping-rounds 1 --length-probe --negative-control --tag final`
→ **20 项门禁断言 0 失败 + 2 项实测结论**（`evidence/probe/probe-final.txt`），阶段 1/3/4/5 连跑三轮一致。

| 阶段 | 断言 | 结果 |
|---|---|---|
| 1 握手 | hello 16 字节的 opcode/version/flag/byte6/`Frz`/`R0x` 前缀/byte15，并与 `O.s9_get_hello()` **逐字节**相等 | 8/8 OK ⇒ F1 成立 |
| 2 探测 | 连发 7 个无副作用 opcode 后连接未被 TCP 层打断 | 通过，但**故意降格为弱证据**（见 §4） |
| 3 s2c + c2s | 服务端 PING 能被我们解出（`hdr=d4 d1 d6 d1`，`opcode=0x0011`，头部过 `sendCypher.isValidHeader`，两侧长度公式相等）；回 PONG 后**活过 20s** | 全 OK ⇒ 双向端到端打通 |
| 4 包长 | 依次发 body 长 2 / 3 / 255 / 256 / 4095 / 4096 / 8192 / 32767 / **32768** / **40000** 的包，扫完再用 PONG oracle 复查派发 | 全 OK ⇒ 服务端长度公式覆盖 16 位全域，大包头不破坏帧同步；**D8 是纯客户端缺陷** |
| 5 negative control | 另开一条连接，第一件事就发**打坏版本号**的头部，看服务端反应 | **坏头不断线 + 后续 PONG 永不被派发**（见 §5） |

第 4 阶段的关键改动：早先版本把"扫完没断线"写成"包长全部被接受"，那是**错的判据** —— §4 说明为什么。现在扫描之后必须再跑一次 PONG ack 轮，才允许下结论。

服务端 `decodePacketLength` 是 `((header >>> 16) ^ (header & 0xFFFF))` 再交换字节（`GMSV83PacketProtocol.java:53-57`），返回值的范围就是 `0..65535`，且 `new byte[packetLength]` 之后才 `readBytes`。所以线路长度的硬上界是 **65535**，探针对 40000 的实测已经把 D8 的触发域（`32768..65535`）钉死为"服务端确实可能发出、客户端一定解错"。

## 4. 方法论修正：连接存活 ≠ 被接受

探针最初的判据是"服务端要是拒绝我们的头，就会当场断线；所以没断线 = 头是对的"。**这个蕴含式的前提被实测否证了**（§5）。后果是严重的：任何以"连上去发了一串包，连接还活着"为凭据的结论都不成立，包括 T8 早前几轮报告的"7 个包被接受""包长 2..40000 被接受"。

替代判据必须是**服务端可观测的副作用**。这里唯一干净的一个是 PONG：

```
Client.checkIfIdle()            // Client.java:1183-1201
  sendPacket(PacketCreator.getPing())          // :1185
  schedule(15s) { if (lastPong < pingedAt)      // :1188
                    closeMapleSession()          // :1194 → finally closeSession() → ioChannel.close()
```

`KeepAliveHandler` 收到 PONG 只做 `c.pongReceived()` → `lastPong = now`（`Client.java:1179-1181`）。
于是 **"收到 PING → 回 PONG → 活过 15s"** 等价于 **"我们的包被解密 + 派发 + 执行到了 handler"**：解密错就派不上，派发错就刷不了 `lastPong`，刷不了就被踢。三个环节缺一个都会表现为断线，而不断线就是全通。

`--ack-wait` 默认 20s，必须 > 15s；`--ping-wait` 默认 40s，必须 > `IdleStateHandler` 的 30s allIdle。

## 5. 坏头部到底发生了什么（本轮最有价值的发现）

同一 IV 下正确头是 `29 b3 2b b3`，我们把 byte0 异或 `0xFF` 发出去变成 `d6 b3 2b b3`（**只打坏头部，绝不打坏正文** —— 正文前两字节是 opcode，打坏它可能命中 172 个已注册 handler 里的任意一个，其中大量会写库/改世界）。三轮的随机 IV 各不相同，但结论一致（`probe-final.txt` 第 57-67 行）。实测：

1. **不断线**：5s 内没有 FIN。
2. **出站方向还活着**：约 30s 后仍收到服务端 PING，并且被我们正常解开（`opcode=0x0011`）⇒ 服务端 `sendCypher` 的链没乱。
3. **入站方向永久废了**：随后回的 PONG 没有被派发 —— 15s 复查时 `lastPong` 仍是旧值，会话被空闲踢掉。`out.log` 里对应 `17:5x:32 连接到了登陆端口` → `17:5x:17 由于空闲而断开连接 127.0.0.1。原因：ALL_IDLE`，中间**没有任何异常栈、没有 warn**（`evidence/probe/outlog-excerpt.txt`）。

**可复现性**：同一套 `--negative-control` 连跑三轮（probe-ack / probe-final 轮 1 / 轮 2）结论一致，且**时间差正好对上模型**：

| 轮次 | negative control connect | 空闲断线 | 差值 |
|---|---|---|---|
| ack | 17:50:52.561 | 17:51:37.585 | 45.02s |
| final 轮 1 | 17:58:32.657 | 17:59:17.684 | 45.03s |
| final 轮 2 | 18:02:23.614 | 18:03:08.636 | 45.02s |

`45s = IdleStateHandler allIdle 30s + checkIfIdle 的 15s lastPong 复查`（`ServerChannelInitializer.java:25,51` 的 `IDLE_TIME_SECONDS=30` + `Client.java:1200` 的 `SECONDS.toMillis(15)`）。同一轮里的主连接因为 PONG 被正常处理，**不会**留下这行 —— 这就是"唯一变量是坏头"的对照。

机制取证（`evidence/probe/netty-ByteToMessageDecoder-channelRead.txt`）：

- `PacketDecoder extends ReplayingDecoder<Void>`（`PacketDecoder.java:11`）。
- netty 4.1.109 的 `ByteToMessageDecoder.channelRead` 把 `decode()` 抛出的异常**重新包装成 `io.netty.handler.codec.DecoderException`** 再 `fireExceptionCaught` —— 反汇编的异常表 `from 16 to 82 target 237 type java.lang.Exception`，随后 `239: new DecoderException` / `245: invokespecial DecoderException.<init>(Throwable)`。
- 于是 `Client.exceptionCaught` 里的 `cause instanceof InvalidPacketHeaderException`（`Client.java:253`）**恒为 false** —— 那段本来会调 `SessionCoordinator.closeSession(this, true)`，而 `SessionCoordinator.java:320-322` 见 `immediately==true` 就无条件 `client.closeSession()`。它没被执行，说明类型判定确实落空了；`instanceof IOException`（`:255`）同样落空 ⇒ **异常被静默吞掉**。
- `decode()` 在 `in.readInt()` 之后才校验头（`GMSV83PacketProtocol.java:31-35`），`crypt()`（唯一推进 IV 的地方，`MapleAESOFB.java:137-139` 的 `updateIv`）根本没走到 ⇒ 按"坏包不消费 IV"建模，我们本地 out IV 也不推进（`pack_without_advancing()`）。**按这个模型，我们随后发的 PONG 头与 IV 对服务端而言都是正确的，本该被解出**；它没被解出，只能是帧层面出了问题：`ReplayingDecoder` 放弃失败的那一遍、不推进底层 `readerIndex`，于是坏头一直卡在累积缓冲区头部，之后每次读都重新抛、`out` 再也产不出包（这一条是**推断**，与前两条实测一致，但我没有直接证据读到 netty 内部 buffer）。

对本项目的三个后果：

1. **判定纪律**：以后所有"服务端接受了吗"的问题，一律用 PONG ack 或 T9 的服务端日志回答，不看连接状态。
2. **客户端侧头部/长度正确性是安全关键的**：一个错头不会报错、不会断线，只会让这条连接"看起来连着、实际聋了"。这正是原生客户端与自研客户端调试里最难查的症状（表现为"登录后一直转圈，约 45s 后掉线"），而且**服务端零日志**。D8（`Cryptography.cpp:89`，取证时为 `:83`）的 `int16_t` 符号扩展落在这个域里：解出天文数字长度 ⇒ 包头永远不完成 ⇒ 同一种聋态。**T5 已把 `int16_t`→`uint16_t`，并在 `Session.cpp:115-126` 加了长度守卫**（`length < OPCODE_LENGTH || length > MAX_PACKET_LENGTH` ⇒ 主动断线），把这条客户端侧的静默聋变成可观测掉线；服务端侧的 F6 本身没修（不改服务端）。
3. **不要在已污染的会话上连续做后续测试**：每条测试连接只承担一个变量；探针现在按这个原则给 negative control 单开连接。

## 6. 已知限制 / 显式预留

| 项 | 状态 |
|---|---|
| 只测了 login port `8484` | 频道端口 `7575/7576/7577`（out.log 启动日志证实**确实在听**，修正早前"只有 8484"的判断）留给 T7 真机取证；协议类与握手同源，但 field 阶段的 `CHANNEL` 会话类型会影响 `closeMapleSession()` 分支 |
| s2c 只见过 PING 一种包 | 探针不登录 ⇒ 拿不到业务回包。字段级 s2c 对账靠 T9（服务端双向日志）+ T7（真机 hex） |
| 长度上界只验到 40000 | 65535 是数学上界（`decodePacketLength` 无符号 16 位），未单独发 65535/65536；65536 无法用 4 字节头表达 |
| 坏头"不断线"的 netty 内部机制 | `DecoderException` 包装 = 反汇编取证；"坏头卡在累积缓冲区头部" = 推断。要坐实需要服务端侧加日志（属改服务端，本计划默认不做） |
| 探针未接入构建门禁 | 与 oracle 同口径：联网工具不进 `build_smoke.bat`，改为"动过加密/传输层手工复跑" |

## 7. 复跑清单

1. `grep -E "已开启登录端口|频道" E:/project/BeiDou-Server_s9/logs/out.log | tail -5` 确认服务端活着（或直接 `netstat -ano | grep 8484`）。
2. `python tools/probe/s9_probe.py --dry-run` —— 先确认门禁没漂（改了 s9 源码或清单后必跑）。
3. `python tools/probe/s9_probe.py --ping-rounds 1 --length-probe --negative-control --tag <本轮标签>`。
4. 核对 `evidence/probe/probe-<标签>.txt`：阶段 1/3/4 应全 `[OK]`；阶段 5 应出现两条 `[MEAS]`，若变成"坏头当场断线"，说明 s9（或 netty 版本）改了异常处理，§4/§5 的判据要重新评估。
5. 顺手把该时段 `out.log` 的网络层行追进 `evidence/probe/outlog-excerpt.txt`。
