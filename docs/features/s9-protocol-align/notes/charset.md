# T10 · 字符集定盘（s9 用哪套 charset 解/发字符串）

> 范围：**显示链路**的字符集权威结论。输入链路（客户端往包里塞中文）按用户决策"先只管显示，输入预留"，
> 本篇只把接口约束写清，不实现。
> 结论分三层：**库侧事实（只读快照）** / **服务端代码链（path:line）** / **仍未证明的部分**。不混写。

## 0. 一句话结论

**本部署里服务端对字符串一律用 GBK，但"因为账号语言 = 3"和"因为兜底 = SERVICE_LANGUAGE"两条路都会得到 GBK，
目前无法区分**——所以任何"GBK 已验证"的说法都必须写清是哪一条，否则就是假绿。

## 1. 库侧只读快照（2026-10-30，dbx 连接 `75c9875d-…` → mysql `127.0.0.1:3306/beidou`）

全部为 `SELECT`，**本轮没有任何 DDL/DML**。逐条 SQL 与结果（含一次猜错列名的失败记录）见
`../evidence/dbx-charset-snapshot.txt`。

```sql
SELECT language, COUNT(*) FROM accounts GROUP BY language;
```

| language | 账号数 | 语义 |
| --- | --- | --- |
| 3 | 3 | `Language.LANGUAGE_CN` = **GBK** / `zh-CN`（`CharsetConstants.java:55`） |

- **没有任何账号是 `language = 2`（US-ASCII）** ⇒ 用户当初"先查，不合适就换号"的分叉**不需要换号**。
- 表默认值本身也是 3（`accounts.language` 列 `DEFAULT 3`）。

```sql
SELECT a.id, CHAR_LENGTH(a.name), a.language, c.id, c.name, c.gm
  FROM accounts a JOIN characters c ON c.accountid = a.id WHERE c.gm >= 4;
```

| account_id | name_len | language | char_id | char_name | gm |
| --- | --- | --- | --- | --- | --- |
| 1 | 5 | 3 | 1 | Admin | 6 |

- 只有一个高级 GM 角色，**账号名按脱敏约定不入库文档**（只记长度 5）；角色名 `Admin` 是北斗自带默认名，非隐私。
- `gm = 6` ⇒ 权限够（`!showpackets` 需 5、`!monitor` 需 3）；⚠ **前缀必须是 `@`/`!`**，
  以 `/` 开头会被 `GeneralChatHandler` 静默丢弃 ⇒ 档 A/E 前置条件成立但本轮实际没开成（`log-pairing.md` §2、`real-session-1.md` §3）。

档 B 的**改前基线快照**（为将来那一行授权 UPDATE 留对照，本轮**没有执行 UPDATE**）：

```sql
SELECT id, config_type, config_sub_type, config_clazz, config_code, config_value, update_time
  FROM game_config WHERE config_code LIKE 'use_debug%';
```

| id | config_type | config_sub_type | config_clazz | config_code | config_value | update_time |
| --- | --- | --- | --- | --- | --- | --- |
| 220 | server | Debug | java.lang.Boolean | `use_debug` | false | NULL |
| 221 | server | Debug | java.lang.Boolean | `use_debug_show_eqp_exp` | false | NULL |
| 222 | server | Debug | java.lang.Boolean | `use_debug_show_rcvd_packet` | false | NULL |
| 223 | server | Debug | java.lang.Boolean | `use_debug_show_life_move` | false | NULL |
| 224 | server | Debug | java.lang.Boolean | `use_debug_show_packet` | false | NULL |

- 列名是 `config_type/config_sub_type/config_code/config_value`（**不是** `tag/name/value`；我第一遍按 JSON 树的结构猜了列名，`ERROR 1054` 打脸，`describe_table` 才对）。
  `GameConfig.java` 内存里那棵 JSON 是 `server → Debug → {code: {clazz, value}}` 的投影，与库表列名不同形，这正是"看单边结构猜另一边"的典型代价。
- 5 行 `update_time` 全为 `NULL` ⇒ 从未被后台 REST `updateConfig` 写过（那条路径会带时间戳）。**与 §3 的结论一致：库里从没动过，运行期看到的任何翻转都来自内存（GM 命令），重启即回退。**

## 2. 服务端字符串编解码到底用哪个 charset（代码链）

只有 4 个真实调用点，全部走同一个函数：

| 位置 | 用途 |
| --- | --- |
| `net/packet/ByteBufInPacket.java:56` | **入站 `readString`**：`new String(bytes, CharsetConstants.getCharset(ThreadLocalUtil.getClientLang()))` |
| `net/packet/ByteBufOutPacket.java:79` | 出站 `writeString` |
| `net/packet/ByteBufOutPacket.java:91` | 出站定长写（`Arrays.copyOf(..., fixed)`） |
| `net/util/HexTool.java:83` | **日志的 `<TEXT>` 栏**（档 B） |

`getClientLang()` 的语义就是要害：

```java
// util/ThreadLocalUtil.java:22-24
return Optional.ofNullable(threadLocal.get()).map(Client::getLanguage).orElse(0);
```

- **`threadLocal` 只有一个写入点**：`client/Client.java:222`，
  且它在 `if (handler != null && handler.validateState(this))` 里面，`finally` 里 `removeCurrentClient()`（`:231`）。
- `CharsetConstants.getCharset(int)`（`:27-28`）→ `Language.fromLang(lang)`；`fromLang`（`:70-77`）遍历不到匹配值就
  **返回 `SERVICE_LANGUAGE`**。`SERVICE_LANGUAGE`（`:39-47`）读 `serviceProperty.getLanguage()`，
  本部署 `gms-server/src/main/resources/application.yml:52` 是 `language: zh-CN` ⇒ `LANGUAGE_CN` ⇒ **GBK**。

所以语言值的来源链是（**库决定，客户端发的字段不参与**）：

| 阶段 | 位置 | 动作 |
| --- | --- | --- |
| 登录 | `client/Client.java:664,682` | `SELECT id, password, gender, banned, pin, pic, characterslots, tos, language FROM accounts WHERE name = ?` → `lang = rs.getInt("language")` |
| 进图装载角色 | `client/Character.java:6767-6770` | `accountService.findById(...)` → `chr.getClient().setLanguage(accountsDO.getLanguage())` |
| `PlayerLoggedin` | `net/server/channel/handlers/PlayerLoggedinHandler.java:224` | `c.setLanguage(player.getClient().getLanguage())` —— **自赋值 no-op**（信息项） |
| GM 命令 | `client/command/commands/gm0/ChangeLanguageCommand.java:41` | `c.setLanguage(Integer.parseInt(params[0]))`，**内存态**，不改库 |

`grep language` 在 `net/server/channel/handlers/` 下**零命中** ⇒ **没有任何登录/进图 handler 从包头里读 language**。
客户端即便发这个字段，服务端也不看；这消除了"客户端写死一个语言字节"的顾虑，但也意味着
**S1/S2 做 body 对账时不能假设那个字段有意义**（它是填充/版本号一类，按实际字节对，不按语义猜）。

## 3. 本轮最有价值的陷阱：日志的 `<TEXT>` 不能证明账号 charset

`InPacketLogger`/`OutPacketLogger` 是 `addBefore("Client", …)` 挂在 `Client` handler **之前**
（`ServerChannelInitializer.java:55-56`），而档 A 的 `收到封包` 打印点在 `Client.java:216-218`——
**也早于** `:222` 的 `setCurrentClient`。两处都调 `HexTool.java:83` 的 `getClientLang()`，
此时 `threadLocal` 里还没有 Client ⇒ 恒取 `orElse(0)` ⇒ `fromLang(0)` 落 `SERVICE_LANGUAGE`。

推论，直接改变取证设计：

1. **档 B 的 `<TEXT>` 栏永远按 `SERVICE_LANGUAGE`（本部署 GBK）解码，与账号语言无关**。
   它可以证明"这串字节按 GBK 解出可读中文"，**不能**证明"账号 `language` 参与了解码"。
2. 本部署 `SERVICE_LANGUAGE` 恰好 = 账号语言 = GBK ⇒ **两条路结果相同，任何实机观察都无法区分**。
   要真的区分，只能制造差异：把 `application.yml` 的 `language` 改成非 `zh-CN`（服务级重启），
   或把某账号 `language` 改成 2（**写库，未授权，不做**），或用一个 handler 内部才读字符串的包（角色名回显、商店留言）
   去比对——因为只有那时 `threadLocal` 里有 Client。第三种是唯一"不改配置不写库"就能验证的路，归 **S 切片**。
3. 换句话说：**T10 的"定盘"定的是显示链路（服务端→客户端）用 GBK，这一点在本部署成立**；
   "账号语言参与哪一段"仍是未证明项，且已给出可执行的判别实验，不留悬案。

## 4. s9 侧缺陷登记（只记录，不改服务端）

| # | 事实 | 后果 | 判定 |
| --- | --- | --- | --- |
| C1 | `CharsetConstants.java:56-58` 三条 `LANGUAGE_PT_BR/THAI/KOREAN` 的 `lang` **都是 `-1`** | `fromLang(-1)` 只命中第一个（`ISO-8859-1`），`TIS620`/`MS949` **经数值永远取不到**；而 `-1` 恰好不与任何真实客户端语言值冲突，所以不会误伤 2/3 | 死枚举项，信息项 |
| C2 | `orElse(0)` 兜底出的 `0` 也不是任何 `Language` 的 `lang` | 无 Client 绑定时**静默**落到 `SERVICE_LANGUAGE`，日志里看不出这是兜底 | 观察项；正是 §3 的机制根因 |
| C3 | `constants/string/LanguageConstants.java:110` 用 `message[chr.getClient().getLanguage()]`，而所有 `message` 数组都是 `new String[4]`（`:29-33` 等） | 账号 `language ≥ 4` 或为负 ⇒ **`ArrayIndexOutOfBoundsException`**（GM0 命令 `ChangeLanguageCommand:41` 可把内存值改成任意整数，不校验） | 当前安全（库内全是 3）；登记。**若将来开放建号 `language` 输入，这是崩溃点** |
| C4 | `util/PacketCreator.java:7471` 在 `isZhCN()` 分支里硬编码 `getCharset(3)` | 该包（NPC 可脚本化列表的名字）**绕过账号语言**，直接 GBK | 与本部署一致，信息项。说明"服务端并非处处走 `getClientLang()`" |

## 5. 对客户端（T11）的约束，本篇只定契约不实现

1. **封包边界 = GBK**：服务端 `readString`/`writeString` 按 §2 取值，本部署恒 GBK
   ⇒ 客户端 `InPacket::read_string` / `OutPacket::write_string` 必须按 **GBK** 解/发，
   转换只允许发生在封包边界（PLAN §P1 的既有约束，不散落到 UI）。
2. ~~**中文源字面量不得 UTF-8 直塞**：C++ 源码里的窄字符串按仓库现状是 GBK~~ ⇒ **会话 #6 复核：这条写反了，已更正**。
   构建带 `/utf-8`（`CMakeLists.txt:115-117` 明确"源字符集与执行字符集都按 UTF-8"，`MapleStory.vcxproj` 六处配置同口径），
   实测 737/737 个非 ASCII 源字面量按 UTF-8 解码通过 ⇒ **客户端内部表示就是 UTF-8**，封包边界做一次 `GBK ↔ UTF-8` 就是全链闭合，
   **不存在**"字面量已是 GBK 再转一次会双重转换"的问题。真正会双重转换的是"调用点直接塞裸 GBK 字节"，
   由 §7 的失败即原样发出（strict UTF-8 校验）兜住。
3. **长度陷阱**：`ByteBufOutPacket.java:91` 的定长写是**按字节** `Arrays.copyOf(..., fixed)`，
   一个汉字 GBK 占 2 字节、UTF-8 占 3 字节 ⇒ 任何"名字上限 N 字节"的字段，客户端若按字符数算就会截断/越界。
   S 切片对账时要逐包确认上限是**字节**还是**字符**。
4. 变长字符串前缀：服务端 `:79` 写 `writeInt(len)` 还是 `writeShort(len)` 因包而异，**不能统一假设**，归 body 对账。

## 6. 未决 / 依赖

| 项 | 说明 |
| --- | --- |
| 探针实测"同一串中文以 GBK vs ASCII 发出，服务端各解出什么" | PLAN T10 原计划里的这一步**已被 §3 修正为不必要的实验**：入站 `readString` 用 `getClientLang()`，而探针会话在绑定前发的那些包根本没进 handler，解出的东西说明不了账号语言。真要测就测**handler 内的回显**（归 S 切片 + T7 真机） |
| 客户端登录包是否携带 language 字段、字节宽度多少 | 属 body 级对账（S1/S2）。本篇只证明"服务端不读它" |
| 中文**输入**（玩家打字进包） | 按用户决策显式预留，不在本批次 |

## 7. T11 落地（会话 #6，2026-10-31）

> 证据：`../evidence/charset/t11-oracle-20261031.md` + 同目录 `oracle-20261031-raw.txt`（73 条断言 failures=0）。

### 7.1 接缝只有一个

| 文件 | 改动 |
| --- | --- |
| `Net/PacketCharset.h` / `Net/PacketCharset.cpp`（新建） | `packet_to_internal`（线上 GBK→内部 UTF-8）、`packet_from_internal`（反向）。除 `<string>`/`<windows.h>` 外无依赖，可独立编译做对拍 |
| `Net/InPacket.cpp:88 read_padded_string` | 逐字节收进 `raw`（跳过 `\0`）后过一次 `packet_to_internal`。`read_string` 走的是同一条路（`:83` 读短前缀 → `read_padded_string`），所以**入站 175 个调用点一处覆盖** |
| `Net/OutPacket.cpp:105 write_string` | **先转码、再按转码后的字节数写 short 前缀**（§5.3 长度陷阱）。出站 74 个调用点同理一处覆盖 |

`sip` 之外的旁路都不需要动：`Session.cpp:41` 的 `packet_dump` 是纯 hex（不经字符串解码），
`skip_string`/`skip_padded_string` 只推偏移量。

### 7.2 三条设计决定及其理由

| 决定 | 选择 | 为什么 |
| --- | --- | --- |
| 转换实现 | Win32 `MultiByteToWideChar`/`WideCharToMultiByte` + `CP 936`，`#ifdef _WIN32`；非 Windows 编译成**不转换**（原样收发） | 与参考端同源：`BeiDou.exe` 的导入表里就有这两个 API（`../../beidou-v17-parity/notes/evidence/beidou_exe_ascii_full.txt:1314,1738`）。零数据文件、Windows 上与线上字节完全一致。仓库确实有 `android/CMakeLists.txt:40` 复用顶层 CMake，所以必须有非 Windows 分支才能编译，但那条路上没有 GBK 需求 ⇒ 用恒等而不是再引一张表 |
| ASCII 快路径 | 任一模子若无 `>=0x80` 字节 ⇒ 原样返回 | 把"175 个既有调用点绝大多数是 ASCII"变成**可证明的字节级不变**，而不只是"应该没影响"。也让新链路对已联调切片（登录/进图/移动/聊天）零回归风险 |
| 失败策略 | 入站：任一步 `<=0` 或计数不符 → 原样发出。出站：`MB_ERR_INVALID_CHARS` 判定非法 UTF-8 → 原样发出 | "转换失败"必须降级成**维持转换前的行为**，不能升级成"丢字段"。真机观测到的 best-fit 细节（`41 D2 42` 把 `B` 当尾字节吃掉、落单引导字节变 `?`）见证据文件 §5，只对**非法**输入不判定 |

### 7.3 顺手修掉的潜伏缺陷

`read_padded_string` 原用 `int16_t i` 遍历 `uint16_t count`（`count > 32767` 时符号溢出）⇒ 改成 `uint16_t`。
这条与 D8（">32KB 的 s2c 包"）同源，属于我这次要重写函数体时才暴露出来的，不额外扩大范围。

### 7.4 真机闭环（2026-10-31）

`e1394e5137e498a1` 部署到 `E:\MXD\BeiDou-Client-xy\`（旧产物备份为 `MapleStory.exe.bak_pre-t11-20261031`）后实测：

| 观测 | 结果 |
| --- | --- |
| 入站 CJK 渲染 | ✅ `!monitor Admin` ⇒ 聊天区 `[Notice] 1 已被监控` + `Admin 正在监控 1`；改动前同一包型只剩 `[Notice] 1` |
| 长度前缀口径 | ✅ 线上 `0a 00`=10 = `"1 "`(2) + `已被监控`(8 GBK)，证明"先转码再量长度" |
| 出站链路 | ✅ GM 命令到达并在服务端执行；`0e 00`=14 的 `!monitor Admin` 与改动前**逐字节一致**（ASCII 快路径无回归） |
| 假阳性排除 | ✅ 客户端 0 处、服务端 Java 0 处含这些字面量 ⇒ 文案只能来自 i18n 资源经线上字节进来 |

逐字段对账与截图：[`evidence/charset/t11-live-20261031.md`](../evidence/charset/t11-live-20261031.md)。

**没有单独真机观测的**：NPC 文本、公会/联盟公告、密语正文。它们与上面走的是同一条
`InPacket::read_padded_string` 接缝（全库只有这一处入站字符串解码），所以离线 oracle 覆盖它们；
但按 T12 的教训（一次屏幕观测 ≠ 逐层读代码），这里只记"接缝共用 + oracle 覆盖"，不记"已真机验证"，
留给 S 切片各自的功能复测顺手取一条真帧。

输入侧按"先只管显示，输入预留"没动（`IO/Window.cpp:709 begin_ascii_text_input` 仍关 IME）。



## 8. 中文链路的最后一环：渲染层字宽度量（T12b，会话 #8，2026-10-31）

T11 把字节→UTF-8 的接缝修好后，屏幕上还剩一类"字是活的、位置是错的"现象（中文标签贴左沿、
长中文不换行）。根因与字符集无关，在渲染层：**同一份 UTF-8 解码被手写了两遍**，两遍口径不一样。

| 位置 | 修复前 | 后果 |
| --- | --- | --- |
| 度量 `LayoutBuilder::add`（`advances` 循环） | 逐**字节**，非 ASCII 一律记 `font.chars[' '].ax` | 一个汉字（3 字节）记 3 个空格宽（18–24 px），实际只画 1 个字形（11–15 px） |
| 度量 `wordwidth` 循环 | 只对 `32..127` 累加 | 纯中文串永不触发过长拆分 ⇒ 只溢出不换行 |
| 绘制 `drawtext` | 逐**码点**，取真字形 `ch.ax` | 画出来的是对的 ⇒ 量出来的宽度和画出来的推进量天然不等 |

所以"度量偏大、绘制正确"，居中算式 `(框宽 − width())/2` 的内缩就偏小，文字被顶到框左沿。

**修法不是补系数，而是让两侧共用一个解码器**：新增 `Graphics/Utf8.h::ms::utf8_next`，
`LayoutBuilder::measure()`（含图集 `locked` 时用的 `wide_advance()`）与 `drawtext` 同调它，
不变式变成结构性的。`advances` 保持**按字节下标**（`Textfield` 光标走 `advance(byte)`），
同一码点的各字节共享该码点的起笔。

**必须先离线证明的承重假设**：度量侧在图集 `locked` 时也要能算，故走
`FT_Load_Glyph(FT_LOAD_DEFAULT)` + `advance.x >> 6`，而绘制侧是 `FT_LOAD_RENDER` ——
两种 flags 的 advance 必须相等。`tools/probe/utf8_metrics_check.cpp` 比了 3570 次、0 反例。
同脚本还跑 26 条解码向量（`tools/probe/gen_utf8_vectors.py` 用 Python `utf-8` codec 现算期望值），
其中 6 条 `lax:` 向量钉住解码器**不校验尾字节 / 不拒过长编码 / ≥0xF8 引导字节照收**的既有宽松行为：
这是修复前后一贯的行为，写进注释而不是偷偷改掉渲染结果。

**真机判据**要挑"把 `Text::width()` 直接写进几何、且屏幕上恒定有中文"的地方 ——
`IO/UITypes/UIChatBar.cpp:301-305` 的聊天目标芯片按 `target_x = (81 - width())/2` 手动居中，
同一串 `对所有人`、同一个 81px 贴图，修复前后各读一次像素：

| 构建 | 字形实宽 | 左内缩 | 字形中心 − 蓝底中心 |
| --- | --- | --- | --- |
| 修复前（`e1394e51`） | 41 px | 3 px | **−13.5 px** |
| 修复后（`6da3c2f6`） | 42 px | 16 px | **0.0 px** |

字形实宽不变 ⇒ 绘制侧没被顺手改动；内缩 3→16 与算式预测 `(81−72)/2=4` → `(81−44)/2=18` 吻合。
对照帧 `evidence/render/t12b-chip-ab-6x.png`，全过程 `evidence/render/t12b-live-20261031.md`。

**仍未取真机帧的同源受益面**（不并入"中文链路已闭环"）：长中文换行、`ChatBalloon` 按
`width()` 拉伸 9 切片、`Text::Background::NAMETAG` 的 `right = left + w + 3`、
`Textfield` 中文光标（属 T13 输入侧）。
