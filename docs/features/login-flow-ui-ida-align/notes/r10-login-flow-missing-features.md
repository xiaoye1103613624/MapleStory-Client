# r10 · 登录链路功能补全与缺失 UI 重建（2026-10-22）

> 范围：在 r09（协议与流程正确性，提交 76aa6e5）基础上，补全登录链路剩余功能，
> 并新增查看全部角色（VIEW_ALL_CHAR）与 PIC 修改窗口两套 UI。
> 参考端：`BeiDou.exe V17.7`（但 rea 加载的仅是启动器，游戏逻辑在 `Gr2D_DX8.dll` 未加载，
> 故协议证据以服务端 `E:\project\BeiDou-Server_s9` 源码为准，与 r09 一致）。
> 计划：`plan_create` 输出 `bfae4baf15ea4a53bac4a0e0c17da73f`。

## task 1 · register-missing-handlers（已完成）

### 决策：注册哪几个、为什么

读 `s9` 的 `SendOpcode` 枚举与 `PacketCreator` 发送点，结论：

| opcode | s9 是否发送 | 客户端原状 | 处理 |
|---|---|---|---|
| `GENDER_DONE (4)` | **否**（性别走 `LOGIN_RESULT` 闭环） | 无 | 注册 handler，仅协议完整性（连 s9 不触发） |
| `CONFIRM_EULA_RESULT (5)` | **否**（EULA 走 `LOGIN_RESULT` 闭环） | 无 | 同上 |
| `CHECK_PINCODE (6)` | 是（`pinOperation`） | 无 | 注册 + 实现 |
| `UPDATE_PINCODE (7)` | 是（`pinRegistered`） | 无 | 注册 + 实现 |
| `VIEW_ALL_CHAR (8)` | 是（`showAllCharacter/Info`） | 无 | 注册 + 基础解析（UI 在 view-all-char 落地） |

> 关键事实：s9 的 `SetGenderHandler` / `AcceptToSHandler` 成功后都回 `getAuthSuccess`
> （即 `LOGIN_RESULT`），`LoginResultHandler`（行 116-131）已据此闭环性别/EULA 流程。
> 所以 `GENDER_DONE` / `CONFIRM_EULA_RESULT` 在 s9 下永不下发，注册其 handler 是协议完整性，
> 非臆造功能；若未来 s9 启用独立回包或连参考端，即生效。

### 协议证据（s9）

- `SendOpcode`：`GENDER_DONE(0x04)` `CONFIRM_EULA_RESULT(0x05)` `CHECK_PINCODE(0x06)`
  `UPDATE_PINCODE(0x07)` `VIEW_ALL_CHAR(0x08)`。
- `PacketCreator.pinOperation(mode)`：`CHECK_PINCODE` + `writeByte(mode)`，
  mode 0=接受 / 1=注册新 PIN / 2=无效重输 / 3=系统错误 / 4=请输入 PIN。
- `PacketCreator.pinRegistered()`：`UPDATE_PINCODE` + `writeByte(0)`（注册成功）。
- `showAllCharacter(totalWorlds,totalChrs)`：`writeByte(totalChrs>0?1:5)` + `writeInt(totalWorlds)` + `writeInt(totalChrs)`。
- `showAllCharacterInfo(worldid,chars,usePic)`：`writeByte(0)` + `writeByte(worldid)` + `writeByte(count)` + 逐项 `addCharEntry(p,chr,true)` + `writeByte(usePic?1:2)`。

### 客户端改动

| 文件 | 改动 |
|---|---|
| `Net/PacketSwitch.cpp` | `RecvOpcode` 枚举补 `GENDER_DONE=4` `CONFIRM_EULA_RESULT=5` `CHECK_PINCODE=6` `UPDATE_PINCODE=7` `VIEW_ALL_CHAR=8`；构造 `emplace` 这 5 个 handler |
| `Net/OutPacket.h` | `Opcode` 枚举补 `VIEW_ALL_CHAR=13`（发包，供 `BtCharacter` 在 view-all-char 使用） |
| `Net/Handlers/LoginHandlers.h` | 声明 5 个 handler 类 |
| `Net/Handlers/LoginHandlers.cpp` | 实现 5 个 handler |
| `IO/UIStateLogin.h` / `.cpp` | 新增 `ViewAllCharWorld` 结构与静态缓冲 `viewall_worlds` / `viewall_ready`，供 `VIEW_ALL_CHAR` 解析结果暂存、后续 UI 消费 |

### handler 行为

- `GenderDoneHandler`：读 gender 字节 → 发 `ServerRequestPacket()`（标准 v83 语义；s9 不触发）。
- `ConfirmEulaResultHandler`：读 result，==1 时移除 TOS 并发 `ServerRequestPacket()`（s9 不触发）。
- `CheckPincodeHandler`：按 `mode` 用 `UILoginNotice` 提示（NEW_PIC_REQ / INCORRECT_PIC / AN_ERROR_OCCURED / PIC_REQ / UNKNOWN_ERROR）；mode 0 不提示。
- `UpdatePincodeHandler`：`writeByte(0)` → `PIC_ACTIVATED`，否则 `UNKNOWN_ERROR`。
- `ViewAllCharHandler`：首字节 0=某世界详情（`worldid`+`count`+`parse_charentry`×count+`use_pic`），
  其余(1/5)=概览（`totalWorlds`+`totalChrs`），解析结果累积到 `UIStateLogin::viewall_worlds`。

### 验证

- `tools/build_smoke.bat`（Release|x64，OutDir=`E:\MXD\BeiDou-Client-xy\`）EXITCODE=0。
- 自动登录回归（admin/admin）：`【1-Admin】` 进图成功（`_enter_game.log` 走到 `transfer_player ok`），无回归。
- 注意：本轮新增 handler 在 s9 默认配置下均不触发（GENDER_DONE/EULA 走 LOGIN_RESULT，
  PIN/VIEW_ALL_CHAR 仅在特定条件/用户操作下触发），故不影响既有链路。

## task 2 · delete-login-error-codes（已完成）

### 证据

- `s9` `DeleteCharHandler` 实际下发的 state（`PacketCreator.deleteCharResponse(cid, state)`）：
  `0x09` 未知错误 / `0x14` PIC 错误 / `0x16` 公会会长 / `0x1A` 待世界转移 / `0x1D` 有家族 / `0` 成功。
  **删除流程从不下发 `0x0A`/`0x12`**（那属登录/BIRTHDAY 流程）。
- 旧 `DeleteCharResponseHandler` 把 `case 10 → BIRTHDAY_INCORRECT` 是错配（服务端删除不发 10）。
- `UILoginNotice` 已有枚举：`INCORRECT_PIC(0x14)`、`CANNOT_DELETE_GUILD_LEADER(0x16)`、
  `CANNOT_DELETE_FAMILY_LEADER(0x1D)`、`UNKNOWN_ERROR`、`TOO_MANY_REQUESTS`。
  世界转移 `0x1A` 无专门枚举，复用 `UNKNOWN_ERROR`。

### 改动（`Net/Handlers/LoginHandlers.cpp`）

- `DeleteCharResponseHandler`：state 映射按 s9 实际码修正
  `0x14→INCORRECT_PIC` / `0x16→CANNOT_DELETE_GUILD_LEADER` / `0x1A→UNKNOWN_ERROR` /
  `0x1D→CANNOT_DELETE_FAMILY_LEADER` / `0x09 与 default→UNKNOWN_ERROR`；删除原错配的 `case 10`。
- `LoginResultHandler` 与 `SelectCharacterHandler` 的 default 分支：原
  `static_cast<int8_t>(reason - 1)` 在 `reason > 128` 时回绕成负，再隐式转 `uint16_t` 当
  `Message id` → 错乱文案。改为未穷举 reason 统一归一至 `UNKNOWN_ERROR`（v83 reason 是固定小集合，
  case 2/5/7/13/23 已覆盖，其余不再臆造映射）。

### 验证

- 构建 EXITCODE=0（仅 LNK4098/LNK4099 链接警告）。
- 自动登录回归（admin/admin）：`【1-Admin】` 进图成功，无回归。

## task 3 · serverlist-dedup-and-rank（已完成 serverlist 去重；rank/burning/timestamp 不呈现）

### serverlist 重连去重（已完成）

- 证据：`UIWorldSelect::add_world`（UIWorldSelect.cpp:721）只 `worlds.emplace_back` + `worldcount++`，
  不清旧；`ServerlistHandler`（LoginHandlers.cpp:211）在 while 中反复 add。重连时服务端重发完整世界列表
  → 新旧叠加，世界选择页出现重复世界。
- 改动：新增 `UIWorldSelect::clear_worlds()`（清 `worlds` + `worldcount`），`ServerlistHandler` 解析前调用。
  首次登录 worlds 为空，clear 无副作用；重连清旧、加新。

### rank / jobrank / burning / timestamp 不呈现（证据判定，非 bug）

- `rank`/`jobrank`：协议**有数据**——`LoginParser::parse_charentry`（LoginParser.cpp:86）已读
  rankinfo 并写入 `StatsEntry.rank/jobrank`（pair<int32,int8>）。但参考端 `CUICharDetail::Draw`
  仅绘制**职业/等级/人气/四维**（见 UICharSelect.cpp:473 逆向注释，角色名由脚下 nametag 显示）；
  本端 `InfoLabel` 枚举也仅 JOB/FAME/STR/DEX/INT/LUK，无 RANK 槽位。参考端 UI 不呈现 rank，
  故不臆造 RANK/JOBRANK UI（位置无证据）。
- `burning`/`use_timestamp`：s9 v83 协议**无数据**——`CharlistHandler`（LoginHandlers.cpp:235）下发
  字段为 `channel_id / characters / pic / slots`；`parse_charentry` / `parse_stats` 均不含 burning/timestamp。
  （v83 创建角色无"燃烧"概念，burning 是 v95+ 引入。）无数据不呈现。
- 结论：三项数据/UI 缺口按 r09 §4「未证实不臆造 UI」原则**显式预留**，不改动 UICharSelect。

### 验证

- 构建 EXITCODE=0（仅 LNK4098/LNK4099 链接警告）。
- 自动登录回归（admin/admin）：`【1-Admin】` 进图成功，无回归。

## task 4 · pic-change-window（已完成 BtChangePIC）

### 证据

- s9 `RegisterPicHandler`（REGISTER_PIC）**仅支持首次注册 PIC**：读序
  `readByte() + readInt(charId) + readString(macs) + readString(hostString) + readString(pic)`，
  且 `c.getPic()` 已设时直接 `closeSession`（不支持「修改」）。服务端无独立 UpdatePinCodeHandler。
- 现有基础：客户端 `UICharSelect::request_pic()`（UICharSelect.cpp:1261）已完整实现
  「两次 UISoftKey 输入新 PIC 比对 → 发 `RegisterPicPacket`（含 `write_hardware_info()`）」；
  `RegisterPicPacket`（SelectCharPackets.h:38）发包结构正确对齐 s9 读序。UISoftKey 软键盘
  长度限制 6-16（PIC 长度）。

### 改动

- `BtChangePIC`（UICharSelect.cpp:1037 原空 stub）点击后复用 `request_pic()`（先校验
  `selected_character` 有效）。v83 注册/修改 PIC 共用 `REGISTER_PIC(29)`，无需新增
  `UpdatePicPacket`（计划原拟的 `UPDATE_PINCODE=7` 是 RecvOpcode 收包，非发包）。
- `BtResetPIC` 此前已落地（打开 `Configuration::get_resetpic()` 网页），不在本项范围。

### 限制（服务端依赖，已登记）

- s9 默认 `enable_pic=false`，且 `RegisterPicHandler` 不支持「修改」（已设即断会话）。
  点击 BtChangePIC 发包后服务端行为取决于 s9 配置（`enable_pic=true` 且需扩展 handler）。
  客户端在此完成功能闭环（窗口 + 发包）。

### 验证

- 构建 EXITCODE=0（仅 LNK4098/LNK4099 链接警告）。
- 自动登录回归（admin/admin）：`【1-Admin】` 进图成功（BtChangePIC 仅手动选角页点击，不影响自动链路）。

## task 5 · 计划内待办（未做）

- `view-all-char`：`VIEW_ALL_CHAR` handler 已在 task1 注册并做基础解析（暂存
  `UIStateLogin::viewall_worlds`）；本项需新增 `UIViewAllChar` 多世界角色列表 UI 消费该缓冲，
  并落地 `BtCharacter`（UICharSelect.cpp:1048 原空 stub）发送 `VIEW_ALL_CHAR(8)` 请求。
