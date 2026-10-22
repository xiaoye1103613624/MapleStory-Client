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

## task 3-5 · 计划内待办（未做，本笔记随各 task 推进追加）

- `serverlist-dedup-and-rank`：`ServerlistHandler` 重连去重；`UICharSelect` 呈现 rank/jobrank/burning/timestamp。
- `pic-change-window`：落地 `BtChangePIC` 窗口 + `UpdatePicPacket`（v83 协议；注意 s9 `enable_pic=false`）。
- `view-all-char`：新增 `UIViewAllChar` UI 消费 `UIStateLogin::viewall_worlds`，落地 `BtCharacter`。
