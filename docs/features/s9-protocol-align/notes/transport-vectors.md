# T4 · 传输层逐字节对拍（客户端 `Cryptography` ↔ s9 `MapleAESOFB`/`MapleCustomEncryption`）

工具：`tools/transport_oracle.py`（退出码 = 差异条数）。
机器可读向量与逐条结果：`../evidence/vectors/transport-vectors.json`（含三份源文件 SHA-256 指纹）。
本轮结论：**20 项对拍 19 项全等，1 项差异 = 新缺陷 D8（入站长度符号扩展）**。

**T5 复跑（2026-10-30，修完 D8 之后）**：退出码 1 → **0**，20 项全绿。
向量另存 `../evidence/vectors/transport-vectors-t5post.json`（`Cryptography.cpp` 指纹由 T4 的
`d1d2d7557a23e774` 变为 `d26fff5604a90b4a`，即"证据跟着被改的源文件走"，不是复用旧快照）。
oracle 侧同步做了两件事：①`cl_check_length` 默认改为无符号（对齐已修的 C++），
`sign_extend=True` 保留下来专门建模修复前的缺陷；②新增第 13 项"覆盖性"断言，
要求 legacy 模型在**至少一个**用例上偏离 —— 否则第 12 项会退化成"用例根本没进高位区间"的空跑。

## 0. 方法（为什么这次对拍不是自欺）

PLAN §7 的风险是"oracle 与 C++ 错得一样"。本轮的做法是**三方独立**：

1. **s9 侧**：`MapleAESOFB.java` / `MapleCustomEncryption.java` 逐句转写为 Python（`s9_*` 函数），
   保留每一处 Java 截断点（`byte` 复合赋值 = 低 8 位截断）与 `>>>` 无符号移位语义。
2. **客户端侧**：`Cryptography.cpp` 逐句转写（`cl_*` 函数），**不参考 s9 源码**。
3. **第三方参照**：pycryptodome 的 `AES-256/ECB`（密钥取 s9 的 32 字节 `skey`），
   以及一份独立实现的标准 AES-256 密钥扩展（`aes256_expand`）。

三张大表（`funnyBytes` / `subbox` / `maplekey`）**从源文件正则解析**而不是手抄，
源文件一变，指纹变、解析结果变，对拍结果跟着变。小常量（`skey`、`0xF2 0x53 0x50 0xC6`、
`IvSeed`）按 `path:line` 硬编码并注释。

长度样本 62 个：`0,1,2,3,4,5,7,8,15,16,17,31,32,63,64,127,128,255,256,…,1455,1456,1457,
2911,2912,2913,4368,4369,8191,16384,32767,32768,40000,65535` + 24 个随机长度，
专门覆盖 `0x5B0` / `0x5B4` 块切换的边界（1456 = 0x5B0，2912 = 0x5B0+0x5B4）。

## 1. 对拍结果总表

| # | 检查项 | 结果 | 证据摘要 |
| --- | --- | --- | --- |
| 1 | 源表解析完整性（funny/subbox/maplekey 长度） | ✅ | 256 / 256 / 256 |
| 2 | **静态 key 扩展**：客户端硬编码 `maplekey[0..239]` 是否 = s9 32B key 的标准 AES-256 扩展 | ✅ | 15 个 round key（240 字节）逐字节全等；表尾 16 字节为 0 且未被引用（`Cryptography.cpp:291`） |
| 3 | 客户端手搓 AES vs JCE AES-256/ECB | ✅ | 200 个随机块全等 ⇒ `subbytes/shiftrows/mixcolumns/addroundkey` 与标准一致 |
| 4 | 自定义加密 6 遍（`encryptData` vs `mapleencrypt`） | ✅ | 62 个长度全等 |
| 5 | 自定义解密 6 遍（`decryptData` vs `mapledecrypt`） | ✅ | 62 个长度全等 |
| 6 | 交叉可逆（s9 加密 → 客户端解密还原明文） | ✅ | 62 个长度 |
| 7 | **`updateIv` / `funnyShit` 链** | ✅ | 从同一 IV 起迭代 **20000 步**逐步全等（含 `>>>0x1d` / `<<3` 的 32 位回绕语义） |
| 8 | **OFB 块切换 + `multiplyBytes` IV 展开**（keystream） | ✅ | 62 个长度密文逐字节全等 |
| 9 | OFB 调用后置 IV | ✅ | 与 #7 独立复证 |
| 10 | c2s 头部长度能否被服务端解出（`create_header` → Netty `readInt` → `decodePacketLength`） | ✅ | 全长度正确（Netty 大端读 + 客户端小端摆位，两次换序互相抵消） |
| 11 | c2s 头部过 `isValidHeader`（服务端 receive 侧版本 `0x5300`） | ✅ | 全长度通过 |
| 12 | **s2c 头部长度客户端解为无符号 `0..65535`（D8 已修）** | ✅ | 62 个长度全等；`Cryptography.cpp:89` 现为 `static_cast<uint16_t>` |
| 13 | D8 断言的**覆盖性**（legacy `int16_t` 模型确有偏离） | ✅ | 14 个长度只有修复前的模型会解错（最小 `len=32768 → -32768`）⇒ 第 12 项不是空跑 |
| 14 | 端到端 c2s（客户端 `encrypt` → 服务端 `decode`） | ✅ | 还原明文 |
| 15 | 端到端 c2s 后置 IV 同步 | ✅ | 两侧 IV 收敛到同值 |
| 16 | 端到端 s2c（服务端 `encode` → 客户端 `decrypt`） | ✅ | 还原明文 |
| 17 | handshake 16 字节布局 | ✅ | 客户端 `sendiv` = 服务端 `recvIv`（`'F''r','z',rand`），`recviv` = 服务端 `sendIv`（`'R','0','x',rand`） |
| 18 | 版本常量 `0xFFFF-83` vs `83` | ✅ | 见 §3.6 |
| 19 | 整场会话 300 包连续 keystream + IV 链（长度 2…10000 混排） | ✅ | 每包密文与 IV 都不分叉 |
| 20 | 双向会话 60 包（客户端 encrypt → 服务端 decode）还原 | ✅ | 含 1456/1457/8192 跨界长度 |

**总判定：加解密管线（自定义 6 遍 → AES-OFB → 头部）在字节层与 s9 完全一致。**
F3 得证；PLAN §2 的 F1（握手）由 #17 静态复证，仍留 T7 真机取证。

## 2. D8 · 入站长度符号扩展（T5 已修；本节保留为归因记录）

现场：[Net/Cryptography.cpp:89](file:///E:/project/MapleStory-Client/Net/Cryptography.cpp#L89)
（T4 取证时该表达式在 `:83`，修复注释占位后下移到 `:89`；旧 JSON 证据里保留的是 `:83`）

```cpp
return static_cast<int16_t>((headermask >> 16) ^ (headermask & 0xFFFF));   // 修复前
```

`s9` 对应实现返回**无符号** 16 位：[GMSV83PacketProtocol.java:53-57](file:///E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/net/encryption/protocol/GMSV83PacketProtocol.java#L53)
（`MapleAESOFB.getPacketLength():155-159` 同形），即合法长度域 = `0..65535`。
客户端把同一表达式截成 `int16_t`，而 `check_length` 的返回类型是 `size_t` ⇒ **符号扩展**：

| 服务端发出的长度 | 客户端 `length` 实得 |
| --- | --- |
| 32767 | 32767 ✅ |
| 32768 | `int16 = -32768` → `size_t = 18446744073709518848` |
| 65535 | `int16 = -1` → `size_t = 18446744073709551615` |

对拍里有 **14 个长度**落在这个区间（32768、33877、35739、36716、38742、40000、41589、43138、
48094、51987、55448、56722、64197、65535）。

后果链（[Net/Session.cpp:106-155](file:///E:/project/MapleStory-Client/Net/Session.cpp#L106)）：

1. `:111` `length` = 天文数字；
2. `:118` `towrite = length - pos` 同样天文数字 → `:120` 被 `available` 夹住，**这一次不会立刻溢出**；
3. `:127` `pos >= length` 永假 ⇒ 该包永不完成、永不派发（表现＝"卡住"，与丢包一模一样）；
4. 后续每一个新包的字节继续 `memcpy(buffer + pos, …)` 且 `pos` 单调增长 ⇒ **越界写**
   （`buffer` 只有 `MAX_PACKET_LENGTH = 131072`，[Net/Session.h:66](file:///E:/project/MapleStory-Client/Net/Session.h#L66)）；
5. 期间 `decrypt`/`packet_dump`/`forward` 都不会执行 ⇒ 服务端日志有"收到"、客户端 `packet_dump.log` 无记录，
   取证时极易误判成"服务端没发"。

可达性：**头部的长度字段本身只有 16 位，`32768..65535` 是合法值**；s9 全仓没有任何包体上限守卫
（`PacketEncoder.java:18-20` → `GMSV83PacketProtocol.encode():60-67` 直接写 header，
grep `65535|MAX_PACKET` 在 `org.gms` 下只命中 opcode/属性位，无一处限制包体）。
所以只要某个 s2c 包（大地图信息、商城列表、多角色装备列表、公会/仓库全量等）超 32KB 就会踩中。
本轮**没有**实测证据表明它已经发生：现存 `packet_dump.log`（46 条，`E:\MXD\BeiDou-Client-xy`）
里最大包体 1282 字节。⇒ 定级为**潜伏缺陷**，触发条件待 T8 探针实测 s2c 长度分布给上界。

修法（T5 已落盘）：

```cpp
// Net/Cryptography.cpp:89
return static_cast<uint16_t>((headermask >> 16) ^ (headermask & 0xFFFF));
```

同时加了下限/上限守卫，把"坏头"变成可观测事件而不是静默卡死（[Net/Session.cpp:115-126](file:///E:/project/MapleStory-Client/Net/Session.cpp#L115)）：

```cpp
if (length < OPCODE_LENGTH || length > MAX_PACKET_LENGTH)   // 解出 0/1 或超出接收缓冲
{   socket.close();  connected = false;   return;   }
```

两个边界各自的可达性要分开说，别以为上半句修完就都安全了：

- **下界是真正可达的**：`length == 0/1` 时 `pos >= length` 立刻成立，会拿 0/1 字节去 `forward`
  （读 2 字节 opcode 直接越界）。D8 修的是"解太大"，这条守的是"解太小"。
- **上界当前不可达**（`USE_CRYPTO` 在 `MapleStory.h:42` 硬定义，长度被 `uint16_t` 夹在 65535 < 131072）。
  保留它是给 `Cryptography.cpp:90-96` 那条 4 字节直拼的 `#else` 分支兜底；而那条分支引用了
  **未定义的 `HEADERLEN`**，现在关不掉也编不过 —— 见 §4 的登记。

F6 的处置也落在这里：s9 侧坏头不断线、不记日志（`notes/probe.md` §5），
客户端若继续走下去就是"越界写 + 静默聋"；主动断线把两者换成一次可观测的掉线。

## 3. 为什么这些"看起来不一样"其实等价（防假警报）

这节是本轮真正的苦工：每一处两侧写法不同、但被证明等价的地方，逐条给出理由。

1. **6 遍的顺序**：s9 是 `for j=0..5` 按 `j%2` 交替正/逆（`MapleCustomEncryption.java:39-67`）；
   客户端是 `for j=0..2` 内嵌"先正后逆"（`Cryptography.cpp:96-120`）。展开后都是
   `正,逆,正,逆,正,逆` ⇒ 同序。解密同理（s9 `j=1..6` 奇=逆、偶=正；客户端 3×（逆,正））。
2. **截断点位置不同**：Java 每步复合赋值都截 8 位，客户端把 `(rollleft + datalen) ^ remember` 一次性截。
   `+ / ^ / ~` 在低 8 位上同态 ⇒ 截断可交换，结果相同。
3. **`~cur` 后再 `+0x48`**：客户端写成 `static_cast<int8_t>((~cur) & 0xFF) + 0x48`（`:106`），
   加了之后仍在赋值时截断 ⇒ 与 Java `cur = (byte)((~cur)&0xFF); cur += 0x48;` 同值。
4. **`datalen--` 的 C++ UB**：`int8_t` 减到 `-129` 形式上是 UB，MSVC 实际按 8 位回绕；
   对拍按 `& 0xFF` 建模，与 s9 的 `byte` 定义行为一致（若哪天换编译器需复查）。
5. **OFB 块推进**：客户端 `offset += blocklength`（加整块，可能越过 `length`），
   s9 `start += llength; remaining -= llength`（加夹住后的实际处理量）。
   两者都在"处理完尾块"后立刻终止循环 ⇒ 等价（已由 62 长度 × 300 包会话实测一致）。
6. **版本常量不对称**：`ClientCyphers.java:15-16` 给 send 侧 `(short)(0xFFFF-83)`、receive 侧 `83`，
   构造函数 `MapleAESOFB.java:97` 再做高低字节交换 ⇒ 服务端**发**用 `0xACFF`、**收**用 `0x5300`。
   客户端 `create_header` 只用裸 `83`（`Cryptography.cpp:55,57`）：
   - `83` 只影响低字节，而服务端收侧校验是 `packet[0]^iv[2]==0x53`、`packet[1]^iv[3]==0x00`
     （`checkPacket():161-164`）⇒ 客户端 `b0 = iv[2]^83`、`b1 = iv[3]` **正好命中**（`b1` 不需要 XOR 0xFF，
     因为 `0x5300 & 0xFF == 0`）。
   - 服务端**发**用 `0xACFF` 时，客户端 `check_length` 是"高 16 位 XOR 低 16 位"，
     版本位两边同现同消 ⇒ 长度解出与版本无关。
   - 副作用（观察项，不算缺陷）：**客户端完全不校验入站头部的版本字节**，任何伪造头只要 XOR 出合法长度就进解码。
     单机环境可接受，若将来要防外部连接需另加校验。
7. **JCE 默认变换**：`Cipher.getInstance("AES")`（`MapleAESOFB.java:89`）在 SunJCE 下 =
   `AES/ECB/PKCS5Padding`，16 字节输入补一整块 ⇒ 输出 32 字节，而 `:122` 只 `arraycopy` 前 16 字节
   = 纯 ECB 结果。`gms-server/pom.xml` 无 BouncyCastle（只引 netty 4.1.109），provider 假设成立；
   若未来引入 BC，需复查此处。
8. **"Only works for versions lower than version 118"**（`Cryptography.cpp:274`）：
   对 v83 成立——检查 #2 已证明这张表恰是 s9 那 32 字节密钥的标准 AES-256 扩展前 240 字节。

## 4. 其余登记项（本轮不改代码）

| 项 | 事实 | 判定 |
| --- | --- | --- |
| > 64KB 包体 | 两侧都把长度截进 16 位（一致地坏），流一旦错位后续全乱 | s9 侧能力问题，登记；客户端加守卫只能"早发现"不能"能收" |
| `length == 0` | 修复前：`Session::process()` 末尾用 `bytes + towrite` 递归，`towrite=0` ⇒ 同参数无限递归 → 栈溢出 | **T5 已挡**：守卫在 `Session.cpp:120-126` 直接断线，0/1 到不了递归 |
| **`HEADERLEN` 未定义** | `Cryptography.cpp:67`（`create_header` 的 `#else`）与 `:93`（`check_length` 的 `#else`）各引用一次，**全仓没有任何定义**（`grep -rn HEADERLEN` 只命中这两行；正式常量是 `NetConstants.h:24 HEADER_LENGTH = 4`） | 两处都在 `#ifdef USE_CRYPTO` 的 `#else` 分支里，而 `USE_CRYPTO` 在 `MapleStory.h:42` 硬定义 ⇒ **当前是编不过的死代码**。谁若关掉 USE_CRYPTO 会立刻撞编译错误；届时这条分支的 4 字节小端直拼可给出到 4294967295 的长度，`Session.cpp` 的上界守卫才真正上岗。本轮不改（改名会动到未编译代码，无实测手段验证），仅登记 |
| 客户端不做入站版本/长度校验 | 见 §3.6 | 观察项；长度侧 T5 起有守卫，版本位仍不校验 |

## 5. 与后续任务的衔接

- **T5** ✅：**D8**（`check_length` 符号扩展）由本 oracle 复验——修完后 #12/#13 转绿，退出码 1 → **0**。
  **D2**（`write_long` 只写 4 字节）不在本 oracle 的覆盖面内：oracle 建模的是
  `Cryptography`/头部/OFB 这条**传输**链，而 D2 在 `OutPacket` 的**字段序列化**层，
  且全仓零调用点 ⇒ 没有任何真实包能触发它。D2 的验收只能是"改后构建通过 + 等第一个
  `long` 字段出现时由 T9 日志配对抓首差异"，本轮**不声称已被对拍证明**。
- **T8 探针**：`tools/probe/s9_probe.py` **直接 `import` 本工具的 `s9_*` 函数**
  （模块级可导入，`main()` 有守卫），避免"探针与 oracle 两份实现漂移"。
  探针首个动作就是实测 s2c 包体长度分布，为 D8 定"是否已实际触发"。
- **T7 真机**：#17 已给出期望的 16 字节布局（`'Frz'`/`'R0x'` 前缀可作快速识别），
  真机 hex 只需与 F1 逐字段比对。
- 复跑命令：`python tools/transport_oracle.py --json docs/features/s9-protocol-align/evidence/vectors/transport-vectors.json`
  （退出码 = 差异条数；T4 时 = 1 即 D8 未修，**T5 修完 = 0**）。
  T5 之后的快照另存 `transport-vectors-t5post.json`，**不覆盖** T4 的 `transport-vectors.json` ——
  旧快照是"当时确实存在差异"的证据，覆盖了就等于把历史改成自证清白。
