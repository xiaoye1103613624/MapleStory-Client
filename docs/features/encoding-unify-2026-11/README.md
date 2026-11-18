# 编码统一与全链路编码审计（2026-11）

对客户端 `MapleStory-Client` 与服务端 `BeiDou-Server_s9` 做编码现状盘点与治理，统一运行相关文本文件为「无 BOM UTF-8」，并对 IMG 资源、网络协议、国际化、客户端配置四条链路做只读审计。

## 口径（已确认）

- 统一范围：仅运行相关（源码/配置/脚本/SQL/自有工具脚本）；排除 `docs/evidence` 临时日志、第三方 vendor、worktree 副本。
- 数据库：本次只出编码清单，改造另排期；未改库、未新增 Flyway 脚本。
- 特殊文件：仅修 `log4j2.xml`；VS 文件（`.sln`/`.vcxproj`）不动保留 BOM。
- IMG/协议/i18n：只读审计与风险清单，不改代码——默认 `zh-CN` 部署下端到端已对齐，改协议编码属业务决策。

## 一、转码记录（已完成）

| 文件 | 原编码 | 新编码 | 处理 | 校验 |
|---|---|---|---|---|
| 服务端 `gms-server/src/main/resources/log4j2.xml` | GBK + 声明 `encoding="GBK"` | 无 BOM UTF-8 + 声明 `encoding="UTF-8"` | GBK 解码→UTF-8 写回，声明同步改 | 无 BOM、strict UTF-8、首行 UTF-8 ✓ |
| 客户端 `tools/level999/server_runtime/log4j2.xml` | GBK + 声明 `encoding="GBK"` | 无 BOM UTF-8 + 声明 `encoding="UTF-8"` | 同上 | ✓ |
| 客户端 `Data/MakerRecipes.h` | UTF8-BOM | 无 BOM UTF-8 | 剥 3 字节 BOM，正文不变 | ✓ |
| 客户端 `_tools/imgdump/main.cpp` | UTF8-BOM | 无 BOM UTF-8 | 剥 3 字节 BOM，正文不变 | ✓ |

写回统一用 `[System.IO.File]::WriteAllText(..., UTF8Encoding(false))` / `WriteAllBytes`，未使用会加 BOM 的 `Out-File -Encoding UTF8`。

## 二、编码现状清单

### 2.1 客户端（聚焦扫描：排除 `.git/.agents/target/build/x64/docs/evidence/vendor`）

- 无 BOM UTF-8：约 4722 个（运行相关）；全仓扫描约 69000 文件。
- 运行相关异常：**已全部转码**（见上表）。
- 已豁免（保留，不阻塞）：
  - `MapleStory.sln` — VS 标准 BOM，强改会被 IDE 写回，不影响 `/utf-8` 构建。
  - `includes/NoLifeNx/nlnx/node_impl.hpp`（GBK）、`includes/NoLifeNx/nlnx/includes/lz4_*/example/fullbench-dll.vcxproj`（BOM）— 第三方 vendor，上游维护。
  - `tools/_staging/UIEquipInventory_mine.cpp`（BOM）— staging 临时文件，不参与构建。
- 排除的一次性产物：`docs/features/**/evidence/*`、`tools/_*`、`build_log_*`、`*_probe_stdout.txt` 等（BOM/UTF16LE/GBK 混杂，无运行收益，改了污染 git 历史）。

### 2.2 服务端

- 无 BOM UTF-8：约 66969 个（多为 wz XML）；`pom.xml` 与 `gms-server/pom.xml` 均声明 `UTF-8` + `project.build.sourceEncoding=UTF-8`。
- 运行相关异常：**仅 `log4j2.xml`（已转码）**。
- 已豁免：`wz/String.wz/MonsterBook.img.xml`、`Skill.img.xml`（BOM 对 XML 合法，且运行时用 `wz-zh-CN` 这套已是 UTF-8）、`.tmp_ph_ids.json`（临时）。

### 2.3 数据库（只读盘点，未改动）

- DDL 层：`gms-server/src/main/resources/db/migration/` 共 111 个 Flyway 脚本。字符集声明统计：`utf8mb4=14`、`COLLATE utf8mb4_general_ci=14`，**全为 utf8mb4，无 utf8/gbk/latin1 例外**。
- 连接层：`application.yml:18` = `jdbc:mysql://localhost:3306/beidou?useUnicode=true&characterEncoding=utf-8&useSSL=false&serverTimezone=Asia/Shanghai`；`src/test/java/CodeGen.java:23` = `...?useInformationSchema=true&characterEncoding=utf-8`。
- 服务语言：`application.yml:52` = `zh-CN`（与协议 GBK 对齐的前提）。
- **差异标注**：连接层 `characterEncoding=utf-8` 在 MySQL Connector/J 中等价 `utf8mb3`，与 DDL 的 `utf8mb4` 不一致——无法承载 emoji/部分生僻字。改造（改 JDBC 参数为 `utf8mb4` + 连接排序规则）另排期，需连库核对实时 `information_schema.SCHEMATA/TABLES/COLUMNS`。

## 三、四条编码链路审计（只读，结论与风险）

### 3.1 IMG / NX 资源字符串

- 主路径：`Util/WzRuntime.cpp:57-65` embed vendor 解析器；`tools/vendor/NoLifeWzToNx-master/NoLifeWzToNx.cpp:370-380` `convert_str()` 用 `codecvt_utf8_utf16` 将解密后的 UTF-16 转 UTF-8；`:423-439` 加密 UTF-16 走此路径；`WzRuntime.cpp:284-289` `get_string()` 零二次转换。**当前 GMS 密钥 + 标准资源下内部统一 UTF-8，不乱码。**
- NX 路径：`includes/NoLifeNx/nlnx/node.cpp:247-255,318-324`、`file.cpp:134-137` 直接返回长度前缀后的原始字节，零转换，前提是 NX 由本仓库转换器生成（已是 UTF-8）。
- 乱码风险（按严重度）：
  1. `NoLifeWzToNx.cpp:457-462` 加密 ASCII 串中 `>=0x80` 字节按 cp1252 映射，GBK 双字节必乱码。
  2. `:93,486-509` `deduce_key()` 只试内置 bms/gms/kms，非标准密钥私服 img 直接解析失败（外部 key 不参与字符串密钥推断）。
  3. `:403-421` plaintext ASCII 原样透传，GBK 字节混入 UTF-8 `std::string`。
  4. `Util/Resource/ResourceProbe.cpp:136-153` `rt_json_escape()` 不校验编码，GBK 字节被写成「UTF-8 JSON」。
- UI 消费侧：`Gameplay/MapleMap/Npc.cpp:115-124` 用 `codecvt_utf8_utf16.from_bytes` 明确假设 UTF-8，GBK 会抛 `range_error`；`IO/UITypes/UINpcTalk.cpp`、`UIMonsterBook.cpp`、`UIStatusBar.cpp` 直接取 `get_string()` 渲染，无 GBK 兜底。
- 建议（另排期）：如需支持非标准密钥/GBK 私服 img，需在 vendor 字符串层增加 key 候选与 GBK→UTF-8 回退；本次不动。

### 3.2 服务端 ↔ 客户端通信

- 客户端：`Net/PacketCharset.cpp:18` `PACKET_MB_CSET=936`；`:35-71` 入站 GBK→UTF-8（`packet_to_internal`）；`:73-109` 出站 UTF-8→GBK（`packet_from_internal`）；`Net/InPacket.cpp:118` / `OutPacket.cpp:105-117` 仅在 `read_string/write_string` 层转换，长度按转码后字节数写；`Net/` 对 `Graphics/Utf8.h` 零引用。
- 服务端：`org/gms/net/packet/ByteBufOutPacket.java:78-82,91` 与 `ByteBufInPacket.java:52-57` 按 `CharsetConstants.getCharset(ThreadLocalUtil.getClientLang())` 编解码。
- **结论：默认 `zh-CN` + 账号 `lang=3` 下双端 GBK 对齐，中文正常**；JDBC 的 `utf-8` 只作用于 Java String↔DB 字节，与 wire GBK 是独立环节，不冲突。正确链路：DB(UTF8)→Java String→wire(GBK)→客户端转内部 UTF-8。
- 乱码风险：
  1. `lang=2` → `US-ASCII`，中文被替换为 `?`（`CharsetConstants.java:54-55`）。
  2. 无客户端上下文时 `lang=0` 回落服务语言（`ThreadLocalUtil.java:22-24`）；服务端主动推送按服务语言编码而客户端恒按 GBK 解，有错配风险。
  3. `CharsetConstants.java:53-58` 中 THAI/KOREAN 的 `lang` 均为 `-1`，`fromLang(-1)` 永远先命中 PT_BR，泰/韩 charset 实际不可达。
  4. emoji/生僻字在 GBK 有损（Java 编码器替换为 `?`）。
- 局部兜底：`PacketCreator.java:7658-7664` 在 `isZhCN()` 时对 NPC 名硬编码 `getCharset(3)`。

### 3.3 国际化（i18n）

- `I18nConfig.java:19,27,35` 三个 MessageSource 均 `setDefaultEncoding("UTF-8")`；`I18nUtil.java:25-30` 文案按 clientLang 的 Locale 取，日志/异常用服务语言。
- `gms-server/wz` 与 `wz-zh-CN` 两套 XML 均声明 `encoding="UTF-8"`；`WZFiles.java:32-46` + `RuntimePaths.java:54-59` 按 `service.language` 选 `wz-<lang>`，不存在回落 `wz`。
- 风险不在资源层，而在 3.2 的 charset 映射（多语言/服务端推送场景）。

### 3.4 客户端配置文件

- `Configuration.cpp:465-575` 二进制读，`:481-490` 显式剥 UTF-8 BOM，`:570` 字节原样存 `std::string` 无校验；`Configuration.h:47,302,307` 明确 `Settings` 与 `xy-config.yaml` 用 UTF-8 无 BOM LF；`:663-679` 写盘无 BOM。
- `Util/Locale.cpp:44-76` 同模式并加载 `locale/zh-CN.ini`（实际 UTF-8）；`Data/SetItemData.cpp:44` 文本模式读 `SetItem.txt`（内容为 ASCII，无 BOM 处理但不影响）。
- 风险：`Configuration.cpp` 把字节原样当 UTF-8，若被编辑器另存为 ANSI/GBK，中文值静默乱码且无告警（注释里的中文因 `#` 跳过不受影响）。

## 四、防回归：校验脚本

- 新增 `tools/check_encoding.py`（Python 3 标准库，无依赖）。
- 能力：白名单扩展名 + 排除规则（`.git/.agents/target/build/docs/vendor/worktree` 等）扫描；检测 UTF-8 BOM、UTF-16LE/BE BOM、非 UTF-8（并判断是否可 GBK 解码以建议转换）；`--root` 自定义根目录；`--json` 机器报告；`--fix` 仅剥 UTF-8 BOM，`--dry-run` 预览；GBK/UTF-16 不自动转码（避免猜错代码页）。
- 内置**带理由的豁免清单** `WAIVED`：VS `.sln`、vendor、`_staging`、临时 JSON、wz 数据 XML 等已知项报告但不阻塞，确保门禁只在「新增偏差」时失败。
- 本次运行结果：`Scanned 69009 files; 8 total, 8 waived, 0 blocking; EXITCODE=0`。JSON 报告见 `tools/_encoding_report.json`。

## 五、验证状态

- 目标文件静态校验：5/5 均为无 BOM、strict UTF-8 合法；两个 `log4j2.xml` 首行声明 `encoding="UTF-8"` ✓。
- 校验脚本自检：8 豁免 / 0 blocking / EXITCODE=0 ✓。
- 客户端构建冒烟：**未执行**——会话中有两个 `MapleStory.exe` 进程（PID 26080/47560）占用部署 exe，`taskkill` 审批两次超时未获放行。转码仅涉及剥 BOM（C++ 文件）与运行时配置（XML），不改变 C++ 语义；仍建议关闭客户端后手动跑一次 `tools\build_smoke.bat` 确认（注意脚本 EXITCODE 不可靠，需核对 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` 时间戳是否刷新）。
- 服务端运行验证：转码后中文日志需启动 s9 才能确认；本端只做静态校验。

## 六、遗留项（另排期）

1. 数据库：`characterEncoding=utf-8` → `utf8mb4`，并连库核对 `information_schema` 实时字符集/排序规则；需 DDL/DML 授权。
2. IMG：非标准密钥 / GBK 私服 img 的字符串级回退（key 候选 + GBK→UTF-8）。
3. 协议：多语言场景的 charset 策略——`lang=2` 承载中文、THAI/KOREAN 的 `lang=-1` 冲突、服务端主动推送无上下文时的编码约定。
4. 配置：`Configuration.cpp` 增加 UTF-8 合法性校验，避免另存为 GBK 后静默乱码。
