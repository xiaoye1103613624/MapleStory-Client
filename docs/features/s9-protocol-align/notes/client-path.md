# 客户端路径记忆（用户指定，2026-10-30）

> 用户原话："当前服务端对应的客户端：`E:\MXD\BeiDou-Client-xy\`"
> 本文件是**路径与运行时事实的唯一出处**；协议结论不放这里，放 `transport-vectors.md` / `opcode-diff.md` / `charset.md`。

## 1. 目录与部署机制

| 事实 | 证据 |
| --- | --- |
| **构建直接写进客户端目录**，没有独立"部署/同步"步骤 | [MapleStory.vcxproj:75,80,85,90](file:///E:/project/MapleStory-Client/MapleStory.vcxproj#L75) 四个配置的 `<OutDir>` 全部是 `E:\MXD\BeiDou-Client-xy\` |
| 第三方 DLL 由 post-build `copy /y /d` 带过去（freetype / bass / liblz4→改名 `liblz4.dll`） | `MapleStory.vcxproj:112-114`、`:136-138` |
| 产物是 **x64**（PE machine `0x8664`），不是 32 位 | 读 `MapleStory.exe` 的 `e_lfanew`→`Machine` 实测；构建口径见 `tools/build_smoke.bat:69`（`Release / x64 / v142`） |
| ⇒ **不能拿这套构建当"与原生参考端一致"的证据**：参考端 `BeiDou.exe` 是 32 位 PE | PLAN §8 末尾那条 x64 提醒；此处只是把出处对齐 |
| 目录里留有历史轮次的 exe 备份 `MapleStory.exe.bak_r22` / `.bak_r28` | 实测 `ls`。替换前先看这两个，别把"我这次的产物"和"上一轮的"搞混 |

其它运行时文件（实测存在）：`MapleStory.exe`、`MapleStoryDbg.exe`、`MapleStory.pdb`、`SDL3.dll`、`Data/`、`UI/`、
`Settings`、`xy-config.yaml`、`packet_dump.log`、`_audio.log` 等一堆 `*.img_runtime.json`（IMG 运行时缓存/清单）。

**进程状态（会话 #8 实测 2026-10-31 02:2x，写这条是为了下次不用猜）**：T12b 产物**已部署并真机 A/B 通过**。
运行目录 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` = `sha256[:16]=6da3c2f6ca40936a` size 4,400,640
（与快照 `E:\MXD\_build_snapshots\20261031-0220_t12b-cjk-metrics\` **同 sha**，COFF `TimeDateStamp` = 2026-10-30 18:20:14Z UTC）；
备份链现为两级：`MapleStory.exe.bak_pre-t12b-20261031` = `e1394e5137e498a1`（T11 构建，TimeDateStamp 17:41:53Z）
→ `MapleStory.exe.bak_pre-t11-20261031` = `8d3f221c83658b46` ⇒ **回滚 T12b 就拷回前者，回滚到 T11 之前用后者**。
当前进程：`MapleStory.exe` PID **29420**（启动 02:21:23，hwnd 5903712），已自动登录进游戏 `[1-Admin]`（彩虹岛）。
会话 #7 那条"PID 42456 在跑"已失效（该实例被本轮替换构建后重启）。

**这次 A/B 用的正是这条备份链**：把 `e1394e51`（修复前）与 `6da3c2f6`（修复后）在同一屏、同一贴图上的
像素差读出来（`对所有人` 芯片左内缩 3→16 px），所以两个 exe 都必须在档、且能凭 sha/TimeDateStamp 说清哪帧是哪版。

会话 #6 那段"构建不进运行目录"的约束仍然成立，只是当时的原因（PID 45248 在跑、覆盖撞文件锁）已随用户退出客户端解除；
源码侧核对留档：00:12 之后只有 `Net/PacketCharset.{h,cpp}`（新）、`Net/InPacket.cpp`、`Net/OutPacket.cpp` 被改过
（`find … | xargs ls -l` 实测）⇒ 那份 exe 相对上一版**只含 T11 的增量**，不是混了历史未提交改动的脏产物。
服务端 `java.exe` 当时在跑的有 6252 / 10108 / 11892 / 37628 / 37896 / 40688 / 43620 / 43668 / 44352（登录服 + 频道服 + 若干辅助），
**会话 #7、#8 两轮均未重启**；数据库**未写入**。
真机证据：`evidence/charset/t11-live-20261031.md`（线上 hex ↔ 截图 ↔ s9 源码三方对账）、
`evidence/render/t12b-live-20261031.md`（度量修复的像素 A/B）。


## 2. 两个配置文件（优先级与编码，都是实测）

| 文件 | 角色 | 编码 | 换行 | 会不会被客户端改写 |
| --- | --- | --- | --- | --- |
| `xy-config.yaml` | **部署级**，优先级最高 | UTF-8 无 BOM | **CRLF**（55 处，0 处裸 LF） | 不会（客户端不写它） |
| `Settings` | 用户级 | UTF-8 无 BOM | LF（173 行，0 处 CRLF） | **会在退出时整体重写** |
| 内置默认值 | 兜底 | — | — | — |

- 优先级来自 `Settings` 文件头自己的注释 + 代码：[Configuration.cpp:517-518](file:///E:/project/MapleStory-Client/Configuration.cpp#L517)。
- ⚠ **`xy-config.yaml` 的头部注释写着"换行：LF"，实际是 CRLF**——文件头不可信，要看字节。
  我改它时用 UTF-8 + CRLF 追加，保持原样（`Configuration.cpp:360-362` 按 `\n` 切行、
  值再过 `trim`（`:391-392`，空白含 `\r`）⇒ CRLF 能被正确解析，这才敢动）。
- ⚠ **显示层会骗人**：本机 shell 是 cp936，`awk`/`Read` 看这两个文件的中文全是 `浣滅敤锛?` 这种乱码，
  但 `python` 里 `d.decode('utf-8')` 全通过、`decode('gbk')` 抛异常 ⇒ **文件本身是好的 UTF-8，别去"修编码"**。
  判定一律用字节十六进制，不用终端渲染。
- 键名写法是缩进层级：`debug.packet_dump` 要写成 `debug:` 之下的 `packet_dump:`（`:486` 支持点分与层级两种）。
- 一个易误判点：`Configuration.cpp:281-296` 那段 `[DBG-CFG]` 临时诊断（写 `_config_debug.log`）在
  **`#ifdef __ANDROID__`** 里，Windows 构建不产生该文件——看到它别以为是有残留调试日志要清。

## 3. 连接参数（当前部署）

| 键 | 值 | 说明 |
| --- | --- | --- |
| `server.ip` | `127.0.0.1` | 登录服 |
| `server.port` | `8484` | 需与服务端 `gms.service.login-port` 一致 |
| 频道端口 | 由服务端登录后下发 | 探针实测 `127.0.0.1:7575` 可握手（`evidence/probe/probe-channel.*`） |

自动登录在 `xy-config.yaml` 里预填（`login.auto/account/pass`）。**凭据不进文档**；
但它有一个副作用值得记：`packet_dump.log` 的第一行就是这条出站包（opcode `0x0001` + 两个 5 字节名字），
所以**开 dump 后日志里会有明文账号/密码字节** ⇒ 取证时拷出的 evidence 必须先剥掉首包内容或只留 hex 的前 2 字节，
这条是 T7 的硬约束，写在这里免得真机跑完才发现泄密。

## 4. T7 的使能开关（本轮已替你打开）

- 开关：`debug.packet_dump`，定义在 [Configuration.h:845-847](file:///E:/project/MapleStory-Client/Configuration.h#L845)
  （`BoolEntry("debug.packet_dump", "false")`，**默认关**）；读取点 [Net/Session.cpp:38](file:///E:/project/MapleStory-Client/Net/Session.cpp#L38)。
- **本轮往 `xy-config.yaml` 追加了**：

  ```yaml
  # ---- 协议联调 (T7) ----
  debug:
    packet_dump: true
  ```

  改前 `size=2552 sha256=bd9bf0388acb275f…`，改后 `size=2614 sha256=d41bdd9d633985e0…`（都是只追加，没动原内容）。
  撤销：删掉文件末尾那 4 行（含前面的空行）即可回到 `bd9bf0388acb275f`。
- ⚠ **必须从该目录启动客户端**：日志用相对路径打开
  （[Net/Session.cpp:52](file:///E:/project/MapleStory-Client/Net/Session.cpp#L52) `std::ofstream file("packet_dump.log", std::ios::app)`）
  ⇒ CWD 不是客户端目录时，日志会落到别处，表现就是"开了开关却没有日志"。
  这是 `Setting` 单例在 `chdir` 前构造那一类坑的同族（见 `Configuration.cpp:282` 的注释），别混淆成因。
- ⚠ **改了配置必须重启客户端，热重载在当前构建里根本不存在**：
  `Configuration::reload_if_changed()`（[Configuration.cpp:468](file:///E:/project/MapleStory-Client/Configuration.cpp#L468)）
  **全仓零调用点**（`grep -rn reload_if_changed` 只命中定义、声明，以及 `Configuration.h:41` 那句"按 mtime 自动热重载"的注释）
  ⇒ 注释与 `xy-config.yaml` 文件头那句"修改后会自动热重载"**都是假的**，值只在进程启动 `load()` 时读一次。
  `Setting<PacketDumpLog>::get().load()`（`Session.cpp:38`）每包都读，但读的是内存里的 Entry，不会重看文件。

## 5. 现有 `packet_dump.log` 的状态（取证基线）

| 项 | 实测 |
| --- | --- |
| 大小 / 行数 | 10383 字节 / **46 行** |
| 最后写入 | 2026-10-29 11:57（**早于**本轮加的方向标记） |
| 带 `[SEND]`/`[RECV]` 前缀的行 | **0 行** ⇒ 全是旧格式"裸 `PacketDump : …`" |
| 本轮处置 | 已改名归档为 `packet_dump.log.pre-t7-20261029`（sha 前缀 `ccafb4ac3eb4b5ed`，内容未动） |

- 为什么改名而不是留着：`packet_dump()` 用 `std::ios::app` **追加**，而 dump 行里**没有任何时间戳**
  ⇒ 新一轮真机数据会和这 46 行混成同一个文件，`pair_logs.py` 无法按时间切开，只能整体作废重取。
  归档后 `packet_dump.log` 不存在，客户端下次启动会新建 ⇒ T7 拿到的每一行都属于本轮会话。
  恢复原状：`mv packet_dump.log.pre-t7-20261029 packet_dump.log`。
- ⚠ 这 46 行**不复制进 `evidence/`**：首包含明文账号/密码字节，仓库里只留行数、哈希与路径。

⇒ 两个结论：① T9 给 `pair_logs.py` 加的"无方向标记旧格式也认"不是臆想需求，真数据里就是 46/46 都这样，
自检里那条用例（`ST_CLIENT` 含一行无标记）覆盖到了真实形态；② 这 46 行是**只有方向未知**的历史包，
**不要**拿它们冒充 T7 的首轮真机数据。


## 6. 与产物目录的关系

`docs/features/s9-protocol-align/` 下所有"真机"证据（`evidence/handshake-*`、`evidence/pairing/*`）
都必须能回溯到这个目录里的原始文件 + 采集时间；大文件（`Data/`、`UI/`）不复制，只记路径与哈希。
