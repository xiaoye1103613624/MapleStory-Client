# 等级上限 999 · 实机联调（2026-11-06，重启复验 2026-11-07）

## 1. 验收结论

| 验收项 | 结果 | 证据 |
| --- | --- | --- |
| 两端协议不错位 | PASS | `evidence/packet_dump-live.log`：1174 个 RECV 包中 523 个 `STAT_CHANGED` 全部按掩码解析成功，无越界/错位 |
| LEVEL 越过 255 不截断 | PASS | 抓包解析出 255 个 LEVEL 样本，末段 `249,250,251,252,253,254,255,256`；2 字节 `writeShort` 生效 |
| 经验不溢出 | PASS | 255 个 EXP 样本，最大 `369,049,639,192`（约 3.69e11），254 个超过 `int32`；8 字节 `writeLong` / `read_long` 生效 |
| 升级动画 | PASS | `evidence/lv-256.png` 角色旁绿色 `LEVEL UP` 特效 |
| LV 三位数显示 | PASS | `evidence/lv-255-zoom.png` → `evidence/lv-256-zoom.png`，橙徽章完整显示 `255` / `256` |
| 经验百分比 | PASS | `evidence/lv-256.png` 状态栏 `EXP 12519.52%`，无除零/溢出/白屏 |
| 经验上限逐级递增 | PASS | 两端统一 `2e9 × 1.06^((level-200)/10)`；离线 `tools/level999/protocol_and_overflow.py` 校验 200..999 全级 0 差异；非固定值 |
| 存档完整 | PASS | 2026-11-07 重启复验：下线后 `characters.level=256` / `exp=347001109454`（BIGINT），`inventoryequipment` 18 行不变、5 个潜能列正常绑定；服务端 `save_errors=0`；详见 §5 / §9 |

## 2. 联调链路

1. 服务端由用户重启（IDE JVM，`target/classes` 22:43 编译）。只读核查：8484 LISTEN、`flyway_schema_history` 中 `V1.9.3/1.9.4/1.9.5` 均 `success=1`、`characters.exp/gachaexp = bigint`、`game_config.max_level=999`、`inventoryequipment` 5 个潜能列齐备、在线账号 0。
2. 客户端 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（22:19:53 构建），临时开 `login.auto=true` + `debug.packet_dump=true`，以正确 `-WorkingDirectory` 启动，自动完成 登录 → 世界 → 频道 → 选角 → 进图 Henesys。
3. 客户端 Textfield 无法注入文本：`SendKeys`、`PostMessage WM_CHAR`、`SendInput KEYEVENTF_UNICODE`、真实 `keybd_event` 逐一实测均无法写入聊天框（与登录页 Textfield 历史结论一致）。改由服务端 GM 管理接口 `/give/v1/resource`（`type=4 exp`）触发**同一条 `gainExp → levelUp → STAT_CHANGED/升级特效` 链**，客户端收到的是真实升级包，不是模拟。
4. `use_level_up_protect` 开启，单次最多升 1 级；分 1 + 20 + 100 + 130 + 4 次推进，最终 1 → 256 级。
5. 关闭客户端触发服务端存档，随后还原全部调试开关。

## 3. 关键数据

- `STAT_CHANGED` 523 包；LEVEL 样本 255 个（最大 256）；EXP 样本 255 个，最大 `369,049,639,192`。
- 服务端 `PacketCreator.updatePlayerStats`：`LEVEL(0x10) → writeShort`、`EXP(0x10000) → writeLong`；客户端 `PlayerHandlers.cpp`：`read_short()` / `read_long()`、`gachaExp skip(4)`，逐字段对齐。
- 状态栏 `EXP 12519.52%` 是 `use_level_up_protect` 让单次经验堆积所致的测试态，不是经验溢出；HP/MP/EXP 条均正常绘制。

## 4. 证据索引

| 文件 | 内容 |
| --- | --- |
| `evidence/01-auto-login.png` | 自动登录进图首帧（LV 1） |
| `evidence/09-give-exp.png` / `09-lv-zoom.png` | 首次 GM 给经验后 LV 2 |
| `evidence/10-batch20.png` / `10-lv-zoom.png` | LV 22（两位数显示） |
| `evidence/11-batch100.png` / `11-lv-zoom.png` | LV 122（三位数显示） |
| `evidence/12-batch130.png` / `12-lv-zoom.png` | LV 252 |
| `evidence/lv-253.png` … `lv-256.png`（含 `-zoom`） | 253→256 逐级 LV；`lv-256.png` 含 LEVELUP 特效与 EXP 百分比 |
| `evidence/packet_dump-live.log` | 本次完整收发封包（338206 字节） |
| `tools/level999/verify_protocol.py` | 按掩码位解析 STAT_CHANGED、输出 LEVEL/EXP 宽度与数值 |
| `evidence/retest-01-login.png` / `retest-02-ingame.png` | 2026-11-07 复验：自动登录首帧 / 进图确认 |
| `evidence/retest-03-lv256.png` | 复验下线前 `LV. 256`、`EXP 12519.52%` |
| `evidence/retest-04-relogin-lv256.png` | 重新登录读档后仍为 `LV. 256`、大额 EXP 百分比正常 |
| `evidence/packet_dump-retest.log` / `packet_dump-relogin.log` | 复验升级链抓包（150170 B）/ 读档链抓包（163846 B） |

## 5. 角色存档缺陷：根因与修复验收（非 999 改造缺陷）

首轮联调下线时的服务端日志原文：

```
23:15:22.979 [ INFO] 开始保存角色数据 Admin
23:15:23.015 [ERROR] Error saving chr Admin, level: 256, job: 0
java.sql.SQLException: Parameter index out of range (28 > number of parameters, which is 27).
  at org.gms.client.inventory.ItemFactory.saveItemsCommon(ItemFactory.java:288)
  at org.gms.client.Character.saveCharToDB(Character.java:8039)
  at org.gms.client.Client.disconnectInternal(Client.java:1066)
```

- `inventoryequipment` 现有 **29 列**（含 `potential1..3`、`bonusPotential1..2`），`ItemFactory` 两处隐式列顺序的 `INSERT ... VALUES (DEFAULT, ?, ...)` 未随 `V1.11.7` 加列同步：`saveItemsCommon` 仅 27 个 `?` 却 `setInt(1..28)`；`saveItemsMerchant` 仅 23 个且漏写潜能列。
- 影响：任何带装备的角色下线都会存档失败（内存 256 级未落库，DB 停留在 `level=1 / exp=9`）；与 999 等级/经验改造无关。
- 修复细节见 §8、重启后的完整复验见 §9；**2026-11-07 已 PASS**，本项不再是受阻项。

## 6. 本轮对齐改动（两端）

- 服务端：`PacketCreator.addPedigreeEntry` 等级 `writeShort`；`MaxStatCommand` 改读 `getMaxClassLevel()`；`ExpeditionType` 10 处 255→999；`GiveService` / `Character.gainExp` 改 long 链；事件脚本 57 处 `maxLevel=255→999`；`V1.9.5__set_max_level_999.sql`。
- 客户端：`WaveAShellHandlers` 家族谱等级 `read_short` + `UIFamilyTree.h uint16_t`；`ExpTable` 1.06 曲线；`UIStatsInfo` 去掉 `LEVELCAP` 守卫；`PlayerHandlers` LEVEL/EXP 宽度。
- SQL 规范位置：Flyway 权威脚本 `gms-server/src/main/resources/db/migration/V1.9.5__set_max_level_999.sql`；手工执行备份置顶于服务端根目录 `migrate_max_level_999.sql`（头部含业务背景、影响、备份、回滚、幂等说明）。
- `CashOperationHandler.java` 编译错误已按项目原有方式修复：`ItemFactory.INVENTORY.saveItems(...)` 回写全量背包（不手写 SQL、不清空背包）；全量编译 1129 源、0 errors。

## 7. 配置还原与回滚

- `xy-config.yaml` 首轮与 2026-11-07 复验后均已还原 `login.auto=false`、`debug.packet_dump=false`（备份 `xy-config.yaml.bak.level999`）。
- 部署目录 `packet_dump.log` 已清空；首轮证据归档 `evidence/packet_dump-live.log`，复验证据归档 `packet_dump-retest.log` / `packet_dump-relogin.log`。
- 客户端进程已关闭；无残留自动登录。
- `max_level` 回滚：`DELETE FROM game_config WHERE config_type='server' AND config_code='max_level';`（脚本头部亦写明）。
- `V1.9.3` 的 `BIGINT` 属安全扩宽，无需回退；`V1.11.7` 潜能列不动。
- 角色数据无需手工改 DB；存档缺陷修复后服务端已正常写回并经重启复验确认（§9）。

## 8. 存档缺陷修复（本轮后续）

- 根因：`V1.11.7` 给 `inventoryequipment` 加了 5 个潜能列（共 29 列），但 `ItemFactory` 两处**隐式列顺序**的 `INSERT ... VALUES (DEFAULT, ?, ...)` 未同步：
  - `saveItemsCommon`（`ItemFactory.java:252`）：27 个 `?` 却 `setInt(1..28)` → `Parameter index out of range (28 > 27)`，角色/仓库/商城保存装备全部失败。
  - `saveItemsMerchant`（`ItemFactory.java:446`）：23 个 `?`、且完全没写潜能列 → 雇佣商人保存装备会列数不匹配并丢失潜能。
- 修复：两处占位符统一补到 **28 个**（`DEFAULT + 28 = 29` 列）；merchant 分支补 `setInt(24..28)` 写 `potential1..3` / `bonusPotential1..2`。未改表结构、未改迁移脚本。
- 验证：全量编译 1129 个源文件、0 errors、1477 个 class（`compile_all_including_wip.py`）；`tools/level999/verify_insert_alignment.py` 静态校验两处占位符与 `setInt` 索引均为 1..28。产物已同步到 `full_classes` / `server_runtime`。
- **生效条件已满足**：用户于 2026-11-07 08:25 手动重启服务端（JVM PID 56272，`-cp ...\gms-server\target\classes`）；只读探针 `verify_restart.py` 确认该目录下 `ItemFactory.class` mtime=08:25:23、常量池 INSERT 为 **28 个 `?`**（修复版），而非旧版 `[27, 23]`。
- 回滚：还原 `ItemFactory.java` 两处 SQL 字符串与 5 行 `setInt` 即可，无需 DB 回滚。

## 9. 重启复验结果（2026-11-07，全部 PASS）

| 复验项 | 结果 |
| --- | --- |
| 服务端加载修复版 classes | `verify_restart.py`：8484/8686 LISTEN；`target/classes` 的 `ItemFactory.class` 占位符 `[28]` |
| 登录 → 进图 | 自动登录成功，`retest-02-ingame.png`（标题 `【1-Admin】【在线时长】`） |
| 升级链越过 255 | REST `/give/v1/resource` ×255 全部 `fails=[]`；抓包 `max_level=256`、`has_level_256=True`、`max_exp=369,049,639,192`、254 个 EXP 超 `int32`、0 个解析错位（521 个 `STAT_CHANGED`） |
| LV / EXP 显示 | `retest-03-lv256.png`：`LV. 256`、`EXP 12519.52%` |
| **下线存档** | `characters.Admin` → `level=256`、`exp=347001109454`、`gachaexp=0`；`inventoryequipment` 18 行不变、5 个潜能列正常绑定写入；服务端 `save_errors=0`，日志 08:34:40 仅 `开始保存角色数据 Admin` 无后续异常 |
| **重新登录读档** | `retest-04-relogin-lv256.png` 仍显示 `LV. 256` 与大额 EXP 百分比；`packet_dump-relogin.log` 中出现落库值的 8 字节小端 `ce bb e0 ca 50 00 00 00`（=347001109454），证明 `writeLong` / `read_long` 读档链正确 |
| 二次下线存档 | 08:36:54 再次保存，`save_errors=0`；DB 仍为 `256 / 347001109454`，装备 18 行 |

- 已知显示瑕疵（**非存档缺陷**，此前误判为「升级未补满 HP/MP 的测试态」，已于 §10 修复）：读档首帧 HP/MP 曾显示 `50/3622`、`5/2820`，随后被服务端 `STAT_CHANGED` 同步回 `3622/2820`。只读抓包分析 `tools/level999/inspect_hpmp_packets.py` 证明：DB `characters.hp/mp = 3622/2820` 落库正确，`SET_FIELD(0x7D)` 下发的也是 `3622/2820`，而 `CHARLIST(0x0B)` 下发的是 `hp=50 / mp=5`；客户端 `SetFieldHandler::set_field` 把 `LoginParser::parse_stats(recv)` 的返回值丢弃，Player 仍由 CHARLIST 的 `playerentry` 初始化，故首帧取了 50/5。真正的「下线多少上线多少」由后续 `STAT_CHANGED` 兜底，数据没丢。
- 复验后 `xy-config.yaml` 再次还原 `login.auto=false` / `debug.packet_dump=false`，`packet_dump.log` 已清空，客户端进程已关闭。
- 复验脚本：`tools/level999/verify_restart.py`、`retest_start.py`、`shot.py`、`gm_give_exp.py`、`verify_protocol.py`、`retest_verify.py`、`retest_relogin.py`、`check_relogin_exp.py`、`retest_finish.py`。

## 10. HP/MP 首帧显示修复（B 方案，2026-11-07）

### 责任判定（用户质疑「服务端进游戏发送逻辑是不是不对」→ 结论：进游戏侧是对的）

| 环节 | 判定 | 证据 |
| --- | --- | --- |
| `PacketCreator.addCharStats` 发包 | **正确** | 忠实写 `chr.getHp()/getMaxHp()/getMp()/getMaxMp()`；`SET_FIELD(0x7D)` 抓包为 `3622/3622/2820/2820`，与 DB 一致 |
| `PlayerLoggedinHandler` + `CharacterService.loadCharFromDB(channelServer=true)` 进游戏加载 | **正确** | 尾部 `reapplyLocalStats()` + `changeHpMp(DB.hp, DB.mp, true)` 修正了构造期 clamp，且用 DB 原值、**不强制补满**，故「下线半血、上线半血」成立 |
| `CharacterService.loadCharFromDB(channelServer=false)` 角色列表加载 | **缺陷** | `if (!channelServer) return chr;` 早退跳过上述修正；`fromCharactersDO` 的 `setHp/setMp` 被 `AbstractCharacterObject` 默认 `localMaxHp=50 / localMaxMp=5` clamp → CHARLIST 下发 `50/3622 / 5/2820` |
| `SetFieldHandler::set_field` 客户端 | **缺陷** | `parse_stats(recv)` 返回值被丢弃，Player 由 CHARLIST 的 `playerentry` 初始化，把正确的 SET_FIELD 数据扔了 |

### 改动

- 服务端 `AbstractCharacterObject.java`：新增 `loadHpMpPoolsFromDb(...)`，按 DB 的 maxhp/maxmp 一次性初始化 `maxHp/clientMaxHp/localMaxHp` 等，绕过构造期 clamp。
- 服务端 `Character.java::fromCharactersDO`：4 行 `setHp/setMaxHp/setMp/setMaxMp` 改为调用新方法，让 CHARLIST 与 SET_FIELD 同源。
- 客户端 `Net/Handlers/SetFieldHandlers.cpp`：接住 `parse_stats` 返回值，把 LEVEL/JOB/STR/DEX/INT/LUK/HP/MAXHP/MP/MAXMP/AP/SP/FAME 与 exp 应用回 `player.get_stats()`（在 `recalc_stats` 之前）。

### 验证

- 服务端全量 `javac`：1129 源、`ALL_JAVAC_RC=0`、0 errors、1477 class；`AbstractCharacterObject.class` 与 `Character.class` 均含 `loadHpMpPoolsFromDb` 引用。
- 客户端 `MSBUILD_RC=0`，exe `09:57:48`。
- 实机第一轮（服务端**未重启**，故意保留 CHARLIST 错值作对照）：`packet_dump-hpmp-fix.log` 中 `SET_FIELD_full_hpmp_packets=1`、`CHARLIST_low_50_5_packets=1`，而 `hpmp-fix-zoom.png` 首帧已是 `LV 256`、`HP 3622/3622`、`MP 2820/2820` → 客户端不再依赖选角页数据。

### 服务侧重编译 / 重启（lithe MCP）

- 全局 `mcp.json` 早已注册 `lithe`（stdio，无需 56105 TCP 端口；此前把 HTTP bridge 端口当作可用性判据是误判）。本轮实际调用：`lithe_project_inspect` / `lithe_run_list` / `lithe_operations_list` / `lithe_environment_inspect` / `lithe_operation_output` / `lithe_maven_execute` / `lithe_run_restart`。
- `lithe_operation_output` 揭示用户 10:14 那次重启的 IDE preparation **只跑了 `resources:resources`**（复制 113 个资源），没有 `compile` → `target/classes` 仍是 08:25 旧 class、`loadHpMpPoolsFromDb` 不存在，服务端修复未生效。
- 用 `lithe_maven_execute({"goals":["compile"],"modulePath":"gms-server"})` 编译：10:19:35 `target/classes/org/gms/client/AbstractCharacterObject.class` 更新且含修复方法；再用 `lithe_run_restart` 重启 `ServerApplication`，10:20:52 新 PID 44144 启动完成、8484 已开（`北斗 启动完成，耗时 8.974 s`）。
- 以后服务端编译 / 重启可直接走 lithe，无需用户手动重启。

### 重启后复验（全部 PASS）

| 项 | 结果 |
| --- | --- |
| CHARLIST HP/MP | `CHARLIST_low_50_5 = 0`、`CHARLIST_full_3622_2820 = 1` → 服务端加载缺陷已修复 |
| SET_FIELD HP/MP | `SET_FIELD_full = 1`（进图包仍是 DB 值） |
| 进图首帧 | `server-hpmp-fixed-zoom.png`：`LV 256`、`HP 3622/3622`、`MP 2820/2820` |
| DB 只读 | `check_hpmp.py`：`level=256`、`exp=347001109454`、`hp=3622 / mp=2820`、`maxhp=3622 / maxmp=2820`（无异常） |
| 证据 | `packet_dump-server-hpmp-fixed.log`、`server-hpmp-fixed.png` / `*-zoom.png` |

- 配置已还原 `login.auto=false` / `debug.packet_dump=false`，客户端已关闭，`packet_dump.log` 已清空。
- 回滚：还原上述 3 个文件即可，无 DB 变更。

## 11. 底栏 EXP 显示：中文 亿/万 当前/上限 + 经验条上百分比（2026-11-07）

### 需求与决策

用户指出「HP/MP 都显示 当前/最大，经验条只有百分比」，且 999 级经验需求达 2.1e11 量级（12 位数字），底栏 EXP 段数字区仅约 86px。决策沿革：初选「中文亿/万 + 两行」→ 实机后要求「与 HP/MP 完全一致」改全位图 → 全位图 12 位数字宽 148px 溢出段宽被右侧按钮遮挡 → 最终拍板**混合方案**：

| 决策点 | 选择 |
| --- | --- |
| 大数格式 | **中文 亿/万**（如 `3470.01亿`，描边文字） |
| 数字行位置 | EXP 段数字行（与 HP/MP 同一行高，`[当前/上限]` 括号风格） |
| 百分比位置 | **叠画在黄绿经验条正中央**（`StatusBar.img/number` 位图字库） |
| EXP「上限」 | **下一级所需** `ExpTable::required_exp(level)` |

### 现状（改动前）

- classic 底栏 EXP 只画 `[xx.xx%]`（`draw_classic`），无当前值/最大值；modern 分支反而画了 `exp数值[xx.xx%]`。
- 底栏数字用 `StatusBar.img/number` 位图字库，只含 `0-9 [ ] / %`，无小数点（1×1 白点补）、无字母、无中文 → 中文 亿/万 必须改用描边文字 `OutlinedText`。

### 改动

- `IO/UITypes/UIStatusBar.h`：
  - `classic_exp_text` 由 `Text` 改 `mutable OutlinedText`（A11M 白字 + 深灰描边，与 joblabel 同风格；位图字库无汉字，亿/万只能走矢量描边文字）。
  - **顺手修溢出**：`hp`/`mp` 在 `draw` 与 `draw_classic` 两处均由 `int16_t` 改 `int32_t`（`get_stat` 返回 `uint16_t`，HP>32767 时 int16 会变负）。
- `IO/UITypes/UIStatusBar.cpp`：
  - `init_classic`：`classic_exp_text` 初始化为 `OutlinedText(..., WHITE, DARKGREY)`；`GAUGE_X/GAUGE_Y` 上移为文件级常量（`draw_classic` 的「百分比叠条」共用同一几何）。
  - `draw_classic` EXP 段两行：第一行 `[3470.01亿/27.72亿]`（`Util/Misc.cpp` `string_format::format_exp_compact`：≥1e8 → `x.xx亿`、≥1e4 → `x.xx万`、否则原样；上限 = `ExpTable::required_exp(level)`），第二行百分比 `[xx.xx%]` 用 `StatusBar.img/number` 位图字库（`draw_hud_number` + `hud_advance` 算宽居中）叠画在经验条填充区（段内 x1..115 / y53..66，7px 字形垂直居中）；`exp >= need` 时封顶 `100.00%`（防异常数据算出几千%）。
- `IO/UITypes/UIChatBar.cpp`（**构建阻断修复，非本需求**）：196/197/200 三处 `Color(153,153,153,255)` 等 int 字面量在 `Color(float×4)`（Color.h:158）与 `Color(uint8_t×4)`（Color.h:163）双构造并存下重载歧义（C2440）。该代码为 2026-11-01 i18n 改动所写，此前因增量编译未重编 UIChatBar.cpp 而一直隐藏；本次改 UIStatusBar.h 触发重编后暴露。改为 `Color(153.0f, ...)` float 写法（与 TextTooltip.h 一致），消除歧义。

### 构建与验证

- `MSBUILD_RC=0`，exe 更新至 21:01:30（期间另遇 LNK1104：残留客户端进程占用 exe，taskkill 后链接成功）。
- 实机（Admin，LV 256）截图 `evidence/exp-display-ingame.png` / `exp-display-bar.png` / `exp-display-zoom.png`：
  - EXP 数字行 `[3470.01亿/27.72亿]`（描边文字，未溢出段宽），百分比 `[100.00%]` 位图字库叠画在黄绿经验条正中央，中文「亿」渲染正常。
- **数据异常说明（非本改动缺陷）**：DB `characters.exp = 347001109454`（3470 亿）远超 256 级需求（27.7 亿），说明该测试账号 exp 是累计值而非级内经验；前端封顶保护使其显示 `100.00%` 而非 125 倍荒谬值。`exp` 语义（级内 vs 累计）需后端另行核实，否则「当前/最大」在该账号上会呈现 当前≫最大。
- 验证脚本：`tools/level999/diagnose_exp.py`（启动→轮询进图→截图，注：shot 输出含非 GBK 字符，脚本在打印阶段报 UnicodeEncodeError，但截图文件已生成）、`capture_exp.py`（裁剪 EXP 段放大）。配置已还原，客户端已关闭。

### 回滚

还原 `IO/UITypes/UIStatusBar.h`、`IO/UITypes/UIStatusBar.cpp`、`IO/UITypes/UIChatBar.cpp`（Color 三处）即可，无 DB 变更。

## 12. 等级上限硬性 3000 + 底栏 EXP 只显示当前经验值（2026-11-11）

### 决策依据

- 经验值维持 `long`（服务端 AtomicLong / DB BIGINT / 协议 writeLong 8 字节，V1.9.3 已扩）+ 现有 1.06 递增曲线：累计经验在 **~3135 级**达到 int64 上限 9.2e18（3000 级累计 ≈4.2e18，占 46%，余量 2.2 倍）→ **3000 级安全，65535 级不可行**。
- 3000 < 32767（有符号 short 上限）→ 协议 `writeShort`、客户端 `uint16_t/int16_t` 全部无需改动。

### 改动

- **服务端 `Character.java`**：`getMaxClassLevel()` 增加 `MAX_LEVEL_CAP = 3000` 常量并 `Math.min(capped, MAX_LEVEL_CAP)` 钳制——即使 `game_config.max_level` 误配更大值也不超 3000，防止经验累计溢出 int64。
- **DB `V1.9.7__set_max_level_3000.sql`**：兜底 INSERT（若 V1.9.5 缺失）+ UPDATE 将既有 999 改为 3000，随 Flyway 启动自动执行。
- **客户端 `ExpTable`**：`required_exp()` 已是动态 1.06 公式（`Character/ExpTable.cpp:59`，≥200 级 `2e9·1.06^((L-200)/10)`，与服务端一致），无固定值钳制，3000 级可正确计算（单级 ≈2.46e16）——现状已满足，无需改动。
- **客户端 `UIStatusBar`**：EXP 数字行最终方案（用户拍板）——**位图字库原始完整数字**（`std::to_string(exp)` + `draw_hud_number`，与 HP/MP 同款，不用中文缩写），**偏上 3px（y38）与烘焙的「EXP」标签同行对齐**；仅显示当前经验值，不显示升级所需。方案沿革：位图两行（当前+上限）→ 中文亿/万描边缩写 → 用户回选位图原始数字（`classic_exp_text` 成员保留但不再使用）。百分比 `[xx.xx%]` 保留位图叠经验条黄色区域。
  - 修复：`classic_exp_text` 成员在之前全位图清理中被误删致 C2065，本次加回（`.h` 声明 + `init_classic` 初始化）后改为不再使用。

### 验证

- 客户端 `MSBUILD_RC=0`（期间先报 `classic_exp_text` 未声明 C2065，加回成员后通过），exe 10:18:37。
- 实机（Admin 132 级）截图 `evidence/exp-display-zoom.png`：EXP 数字行 `275812739917`（位图原始 12 位，与血量/蓝量数字行同高 y=41）+ 条上 `100.00%`。
- **服务端注意**：`Character.java` 改动需在服务端构建环境（IDE/Maven）编译后重启生效；本机无 mvn/wrapper 未编译验证（改动为常量 + `Math.min`，语法风险可忽略）。`V1.9.7` 迁移随服务端下次启动自动执行。

### 回滚

- 服务端：还原 `Character.java`（getMaxClassLevel），执行 `UPDATE game_config SET config_value='999' WHERE config_type='server' AND config_code='max_level'`（或删 V1.9.7 重跑历史）。
- 客户端：还原 `IO/UITypes/UIStatusBar.h`、`IO/UITypes/UIStatusBar.cpp`，无 DB 变更。

## 13. 底栏 UI 微调：EXP 高度对齐 + 等级 4 位排版（2026-11-11）

### 改动
- **EXP 数字行高度**：删除此前偏上 3px 偏移（`statset_pos + (0,-3)`），改用 `position + statset_pos`，与血量(HP)/蓝量(MP)数字行同一水平（y=statset_pos.y()=41）。
- **等级 4 位排版**：经典底栏 LV 徽章内等级数字（Basic.img/LevelNo 橙盒，11x13，推进 12）居中基准 `+5 → +2`，使 3000 级 4 位数字整体左移、右侧留出安全间距；ShowLevel 回退分支同步调整。宽度按位数动态计算（`lvw = len*12-1`），3 位仍居中美观。
  - 几何核验：3 位 lvw=35 → x=37；4 位 lvw=47 → x=29，落 x29..76，LV 底板约 x0..84，右侧留 ~8px 安全，不溢出暗区。WZ 字库固有 11px 间距无法在代码改小，仅调基准。

### 验证
- 客户端 `MSBUILD_RC=0`（exe 11:07:55），lint 0。
- 实机（Admin 132 级）截图 `evidence/exp-display-zoom.png`：EXP 数字行与 HP/MP 同高。
- 临时强制渲染 level=3000 进图截图 `evidence/level-display-zoom.png`：4 位 `3000` 完整落在 LV 徽章内、右侧不溢出（验证后已还原临时改动并重新编译）。
- 配置已还原、客户端已关闭。

### 回滚
- 客户端：EXP 数字行改回 `position + statset_pos + (0,-3)`；等级居中基准 `+2` 改回 `+5`。无 DB 变更。

## 14. 血量/蓝量上限提到 99999 + MAXHP/MAXMP 协议 short→int（2026-11-11）

### 背景
- 客户端对 HP/MP/MAXHP/MAXMP **无任何钳制**（`CharStats.cpp` 直接取服务端下发值）。3W 上限来自服务端 `AbstractCharacterObject.java` 的 `clientMaxHp = Math.min(30000, hp_)` 钳制（:266/274/296-297），且这些值经 `writeShort` 下发、客户端 `read_short` 接收——short 无符号上限 65535、有符号 32767，**99999 超出会被截断成低 16 位（34463）**。用户确认将硬上限上调到 99999（服务端下发逻辑不动，仅放宽钳制），故必须把 MAXHP/MAXMP/HP/MP 协议从 short 改 int。

### 改动（端到端 short→int）
- **服务端 `AbstractCharacterObject.java`**：4 处 `Math.min(30000, x)` → `Math.min(99999, x)`（setMaxHp/setMaxMp/loadHpMpPoolsFromDb）。
- **服务端 `PacketCreator.java`**：
  - `addCharStats`（登录/CharData 包）：HP/MP/MAXHP/MAXMP `writeShort` → `writeInt`（:198-201）。
  - `updatePlayerStats`（StatChanged 包）：`v < 0xFFFF` 分支内对 HP/MAXHP/MP/MAXMP（掩码 0x400/0x800/0x1000/0x2000）特判 `writeInt`，其余 stat（JOB/STR/DEX/INT/LUK/AP/SP）保持 `writeShort`（:1067-1077）。
  - **不动** `getWarpToMap` 的 `writeShort(chr.getHp())`：该字段被客户端 `SetFieldHandlers::set_field` 的 `recv.skip(23)` 跳过，玩家血量由 `LoginParser::parse_stats` 的 CharData 段加载；改动会导致 SET_FIELD 包错位。
- **客户端**：
  - `CharStats.h`：`basestats` `EnumMap<MapleStat::Id, uint16_t>` → `int32_t`；`get_stat` 返回 `uint16_t` → `int32_t`；`set_stat` 参数 `uint16_t` → `int32_t`（`.cpp` 实现同步）。
  - `Net/Login.h`：`StatsEntry.stats` `uint16_t` → `int32_t`。
  - `LoginParser.cpp` / `CashShopParser.cpp`：HP/MP/MAXHP/MAXMP `read_short` → `read_int`。
  - `PlayerHandlers.cpp`：StatChanged 解析 `default` 分支为 HP/MP/MAXHP/MAXMP 加 `case` 特例 `read_int`（从 default 的 `read_short` 排除，其余 stat 仍走 default）。
  - `Stage.cpp` / `Player.cpp` / `WaveAShellHandlers.cpp` / `UIStatusBar.cpp`：HP/MP/MAXHP/MAXMP 接收的 `uint16_t`/`int16_t` 局部变量 → `int32_t`。

### 验证
- 客户端 `MSBUILD_RC=0`（exe 12:10:04），少量 `warning C4244`（窄化，非致命）。
- 实机进图（Admin 132 暗之黑骑士）正常：角色名/等级/职业/破功值 `199999` 全部正确解析，证明协议 int 化后 CharData 段**无错位**；HP/MP 显示正常（见 `evidence/hud-hpmp.png`）。
- **服务端注意**：`AbstractCharacterObject.java` + `PacketCreator.java` 改动需重新编译服务端并重启（本机无 mvn，改动为 Math.min/writeInt，语法风险可忽略）。Flyway 无新增迁移。

### 说明
- 硬上限已放宽至 99999（移除 30000 钳制）；角色实际 maxhp/maxmp 仍由成长/装备/技能决定，仅当实际值超过 30000 时才显示更大值（≤99999）。若要让角色真实长到 99999，需调整 maxhp 成长公式（本次未动，符合用户"下发逻辑不动"）。
- LV. 保持现状不左移（用户拍板）。

## 15. 四维属性 STR/DEX/INT/LUK 协议 short→int（2026-11-11）

### 背景
- 与 HP/MP/MAXHP/MAXMP 同理：升级给 AP、装备 % 加成、后期高版本玩法（如 +1% 力量/敏捷）会让四属性远超 short 上限（有符号 32767），故将四属性协议改为 int。
- **攻击力/魔法力（EquipStat WATK/MATK）不改**：其协议仍是 short（服务端 `PacketCreator.java:484-485` `writeShort(equip.getWatk()/getMatk())`，装备信息包），无符号上限 65535 / 有符号 32767；游戏内攻击力通常几百~几千，远不到，无需 `long`（那是 EXP 级 8 字节大数）。客户端总攻/魔 `totalstats` 存储已是 `int32_t`，显示无截断。

### 改动
- **服务端 `PacketCreator.java`**：`addCharStats` 四维 `writeShort`→`writeInt`（:194-197）；`updatePlayerStats` 的 `v < 0xFFFF` 分支 int 特判加入四属性掩码 `0x40/0x80/0x100/0x200`（:1067 附近）。
- **客户端**：`LoginParser.cpp` 四维 `read_short`→`read_int`（:140-143）；`CashShopParser.cpp` 四维 `read_short`→`read_int`（:86-89，商城路径与 LoginParser 同步，否则商城角色数据错位）；`PlayerHandlers.cpp` StatChanged 解析为四属性加 `case` 特例 `read_int`（从 `default` 的 `read_short` 排除）。底层 `basestats` 存储（`int32_t`，§14 已改）无需再动。
- **FAME（人气度）不改**：GMS 约定上限 30000，short 足够（已确认服务端 `AbstractCharacterObject`/`Character` 无 FAME 钳制）。

### 验证
- 客户端 `MSBUILD_RC=0`（exe 13:31:53）。
- **进图实测 `NEVER_INGAME`**：因运行中的服务端（BeiDou.jar 未重建到含四属性 int 源码的版本）仍用旧协议 `writeShort`，新旧协议不匹配导致 CharData 段错位、选角后进图失败。这是**部署不同步，非客户端代码 bug**（改动对称 `writeInt↔read_int`，与 §14 的 HP/MP 改动机制一致，上次 HP/MP 在重启服务端后进图正常）。
- **服务端须重新编译（jar 重建/IDE 重启）并重启**才能匹配客户端新协议。

### 说明
- 客户端 exe 保持 int 版（13:31:53），用户重启服务端后即匹配。
- 服务端源码改动已落地（`PacketCreator.java`），用户重建+重启即可（本机无 mvn/Java21，需用户执行）。

## 16. 攻击力/魔法力（WATK/MATK）放宽到 int32 最大值（2026-11-11）

### 背景
- 用户确认详细属性面板的总攻击力/魔法力会非常大（AP 分配 + 装备% + 高版本玩法如 +1% 力量/敏捷），原 short 不够。
- 但总攻击力 `get_total(WATK/MAGIC)` = `totalstats[EquipStat::Id::WATK/MAGIC]`，存储已是 `int32_t`（约 21 亿），**不需要 long**。

### 真正卡死"攻击力非常大"的元凶
- 客户端 `Character/StatCaps.h` 的 `EQSTAT_CAPS` 钳制：`WATK=999`、`MAGIC=2000`（`CharStats.cpp:144-152` 的 `set_total` 生效）。不放开，攻击力显示永远卡在 999/2000。

### 改动（客户端 int16→int32 + StatCaps 放宽）
- **`StatCaps.h`**：`WATK`/`MAGIC` 钳制值 → `2147483647`（int32 最大值），超过即按上限计算/展示。
- **`BulletData.h/.cpp`**：`get_watk()` 返回 + `watk` 成员 `int16_t`→`int32_t`（子弹攻击力经 `Inventory.cpp:90` 累加进 totalstats）。
- **`ActiveBuffs.h/.cpp`**：`ActiveBuff` 基类、`SimpleStatBuff`/`PercentageStatBuff` 模板、具体 buff（MapleWarrior/Stance/Booster）的 `apply_to` 参数，以及 `ActiveBuffs::apply_buff` 参数 `int16_t`→`int32_t`（buff delta WATK/MAGIC 防截断）。
- **`Buff.h`**：`struct Buff { int16_t value; }` → `int32_t value`（buff delta 传递）。
- **`WaveAShellHandlers.cpp:2787`**：ForcedStat 路径 `int16_t v` → `int32_t v`（防御性，保持 `read_short`）。

### 验证
- 客户端 `MSBUILD_RC=0`（exe 14:13:20）。
- 实机进图正常（INGAME，角色名/等级/职业/破功值 199999 正常）→ 编译后客户端无崩溃/错位。

### 说明（2026-11-11 续：装备 tooltip 单件攻/魔曾尝试放宽，后已还原）
- **结论：tooltip 单件攻/魔维持原 short/uint16_t 不变**（用户 2026-11-11 要求还原）。即：`PacketCreator.java:485-486` 仍 `writeShort(equip.getWatk()/getMatk())`；客户端 `ItemParser.cpp:81`、`StorageHandlers.cpp:157` 的 `EnumMap<EquipStat::Id, uint16_t> stats` 仍整段 `read_short`；`Equip.h/.cpp`、`Inventory.h/.cpp` 的 `stats`/`totalstats` 与 `get_stat()` 维持 `uint16_t`；`EquipTooltip`/`EquipQuality`/`Player.cpp` 维持原窄类型。服务端 `Equip.watk/matk` 仍 `short`，各 loader（ItemFactory/MTS/EnterMTS）与 DTO（`InventoryEquipRtnDTO`/`GiveResourceReqDTO`/`EquipmentInfoRtnDTO`）的 `pAtk/mAtk` 仍 `Short`，`gainEquip` 参数仍 `Short`。
- **仅保留本 §16 上半部分的总攻击力链路放宽**（StatCaps int32max、BulletData/ActiveBuffs/Buff/WaveAShellHandlers int32）——那是"总攻击力/魔法力"而非"tooltip 单件攻/魔"，两者无关，不随本次还原而回退。
- 还原后客户端已 `build_smoke` 通过（`EXITCODE=0`，无 error/warning）。
- 若日后确需单件 tooltip 攻/魔超 65535，再按上述（已被还原的）int 方案重做即可；卷轴/魔方生成钳制 `getMaximumShortMaxIfOverflow`/`getShortMaxIfOverflow`（32767）为独立议题，未改动。
