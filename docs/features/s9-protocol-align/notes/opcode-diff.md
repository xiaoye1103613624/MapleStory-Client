# T2 · opcode 差异归因（客户端 ↔ s9）

数据源：`protocol/opcode-matrix.json`（由 `tools/gen_opcode_matrix.py` 从两侧源码生成，`--check` 做门禁）。
规范源：`E:\project\BeiDou-Server_s9`（git head `44d0d5d9ca`，`ServerConstants.VERSION = 83`）。
本文只谈 **opcode 层**（谁发、谁收、值对不对）；body 字段级对账在 P2 的 S1..S11 切片。

## 0. 规模与总账

| 侧 | 定义 | 实际使用 | 结论 |
| --- | --- | --- | --- |
| s9 `RecvOpcode` | 181 常量 / 180 值 | 173 个 opcode 注册了 handler（174 个注册点） | 1 处值撞车（139）、1 处重复注册 |
| s9 `SendOpcode` | 311 常量 / 311 值 | 250 个真正下发 | 无值撞车 |
| 客户端 `OutPacket.h`（c2s） | 103 常量 | **103 全部有构造点** | 无死枚举 |
| 客户端 `PacketSwitch.cpp`（s2c） | 303 常量 | 303 个 handler 注册 | 2 处值撞车（35 / 152） |

c2s 方向：`MATCHED 99 / CLIENT_ONLY 4 / 死枚举 0`。
s2c 方向：`HANDLED 297 / CLIENT_DUP_VALUE 2 / UNHANDLED_EMITTED 3 / DEFINED_NOT_EMITTED 9`。

一个先决事实，决定了下面所有"值可直接比较"的推理成立：s9 读 opcode 走
`ByteBufInPacket.java:30-31`（`readShort() { return byteBuf.readShortLE(); }`），
`Client.java:213` 亦为小端 —— 所以两侧的数值就是线上值，LE 不是差异来源。

## 1. c2s：4 条 CLIENT_ONLY 的归因

**归因主结论：其中 3 条是同一个根因 —— 客户端把 s2c（`SendOpcode`）的值抄进了 c2s 枚举。**
v83 这三个系统天然收发不同值：仓库 62↔309、玩家互动 123↔314、商城 229↔325。
客户端收侧（`PacketSwitch.cpp`）用 309/314/325 是**对的**；发侧（`OutPacket.h`）照抄了同一组数字，
于是服务端 `handlers[309] / handlers[314] / handlers[325]` 三格永远是空的（`PacketProcessor.java:100-105`），
而它真正注册 handler 的 `handlers[62] / handlers[123] / handlers[229]` 客户端永远不发。
不是"服务端缺功能"，是**客户端发错了格子**。

| # | 客户端 | s9 真实 c2s 值 | s9 handler | 该值在 s9 的实际身份 | 判定 |
| --- | --- | --- | --- | --- | --- |
| 1 | `STORAGE = 309`<br>[OutPacket.h:136](file:///E:/project/MapleStory-Client/Net/OutPacket.h#L136) | `STORAGE(0x3E)`=62<br>`RecvOpcode.java:78` | `StorageHandler`<br>`PacketProcessor.java:210` | 309=0x135 是 **`SendOpcode.STORAGE`**<br>`SendOpcode.java:315` | 发侧值错，改 309→62 |
| 2 | `PLAYER_INTERACTION = 314`<br>[OutPacket.h:162](file:///E:/project/MapleStory-Client/Net/OutPacket.h#L162) | `PLAYER_INTERACTION(0x7B)`=123<br>`RecvOpcode.java:131` | `PlayerInteractionHandler`<br>`PacketProcessor.java:204` | 314=0x13A 是 **`SendOpcode.PLAYER_INTERACTION`**<br>`SendOpcode.java:320` | 发侧值错，改 314→123 |
| 3 | `CASHSHOP_OPERATION = 325`<br>[OutPacket.h:168](file:///E:/project/MapleStory-Client/Net/OutPacket.h#L168) | `CASHSHOP_OPERATION(0xE5)`=229<br>`RecvOpcode.java:199` | `CashOperationHandler`<br>`PacketProcessor.java:233` | 325=0x145 是 **`SendOpcode.CASHSHOP_OPERATION`**<br>`SendOpcode.java:335` | 发侧值错，改 325→229 |
| 4 | `LOGIN_EMAIL = 3`<br>[OutPacket.h:41](file:///E:/project/MapleStory-Client/Net/OutPacket.h#L41) | **s9 recv 表没有 3**：`RecvOpcode.java:27-29` 是 0x01/0x02/**0x04**，0x03 被跳过 | — | 3=0x03 在 s9 只是 `SendOpcode.SERVERSTATUS`（`SendOpcode.java:29`，s2c 方向） | 非北斗协议（Nexon 渠道登录），本对接不实现 |

交叉验证（不是靠我读代码推断）：

- 三条的"收侧是对的"独立成立：`PacketSwitch.cpp:163` = 309、`:162` = 314、`:403` = 325，
  且矩阵里这三条 s2c 行全为 `HANDLED`，生效 handler 恰好是各系统的**回包处理器** ——
  309→`StorageHandler`、314→`TradeHandler`、325→`CashShopOperationHandler`。
  同一份客户端里只有发侧被抄错，收侧是对的，这一点是硬证据而非推测。
- 第 3 条客户端作者**自己已经写在注释里**：
  [OutPacket.h:167](file:///E:/project/MapleStory-Client/Net/OutPacket.h#L167)
  `/// s9 RecvOpcode.CASHSHOP_OPERATION=0xE5；历史客户端误用 325（Send 侧），购买仍走此值`。
  这条注释对 s9 的引用是**准确的**（`RecvOpcode.java:199` 确实 0xE5），错的是紧跟着的枚举值本身。
  矩阵新增的 `annotation_consistent` 字段按"注释里的 s9 值是否与 s9 源码相符"来判，
  结果 `OutPacket.h` 全部 `/// s9 RecvOpcode.X = 0xNN` 断言 **100% 与 s9 源码一致（0 条过期）**；
  这既交叉验证了本报告的取值，也说明这套注释可以当作可信索引使用。
- 三条都有活的 UI 触发链（不是死代码），逐条给到实例化点：
  - `StoragePacket` 被 `StoragePackets.h:38/49/61/68/78` 的 6 个子类继承使用（仓库窗口一整套操作）。
  - `HiredMerchantOpenPacket`/`CreatePacket`/`SetItemPacket`/`BuyPacket` →
    `UIItemInventory.cpp:703`、`UIMerchant.cpp:182/198/220`、`UIMiscShells.cpp:118/135/1566/1635`。
  - `CashShopBuyPacket`/`GiftPacket`/`TakePacket`/`PutPacket`/`PackagePacket`/`WishPacket` →
    `UICashShop.cpp:386/409/444/461/499/511/532/563/627/672/701/719/732/781`。
  - 复核命令：`python tools/opcode_row_evidence.py`（枚举构造点 → 所属 packet 类 → 类实例化点）。

第 4 条的处理理由：`LoginEmailPacket` 只在 Nexon 渠道分支发（[UILogin.cpp:457-461](file:///E:/project/MapleStory-Client/IO/UITypes/UILogin.cpp#L457-L461)），
默认走北斗账号 `LOGIN = 1` → s9 `LOGIN_PASSWORD(0x01)`，值是对的（`PacketProcessor.java:149` `LoginPasswordHandler`）。
Nexon 开关初值 `nexon(false)`（`UILogin.cpp:57`），由 `:533`/`:546` 两个按钮切换。也就是说：**用户手动点到
"NexonID" 模式就会发出一个服务端根本不认的 opcode，登录框永远转圈**。这属于"本版本没有的功能"，
不是对接缺陷，但必须显式预留（见 §4 修复清单第 4 项），不能让它伪装成丢包。

### 取证要点（决定 §4 每条验收怎么做）

s9 `Client.java:207-235` 的派发是：读 opcode（`:213`）→ `getHandler(opcode)` → **handler 为 null 时整块跳过，
不打任何日志**（`:220` 的 `if (handler != null && …)` 没有 else），只 `updateLastPacket()`（`:235`）。
也就是说发错 opcode 在服务端是**完全静默**的，既没有 warn 也没有异常。唯一能把"发了但没人收"和
"根本没收到"区分开的开关，是 `Client.java:216-218` 的 `use_debug_show_rcvd_packet`（在 null 检查**之前**打
`收到封包 包头ID [0x..] 内容：…`）。所以 §4 的每条改值验收都必须成对做：

1. 改前发 309/314/325 → 服务端有"收到封包"且**没有**任何 handler 后续日志；
2. 改后发 62/123/229 → 同一条"收到封包"之后出现对应 handler 的业务日志或回包。

一个必须提前知道的口子：这条日志对 **7 个高频 recv opcode 被硬编码屏蔽**
（`net/packet/logging/LoggingUtil.java:9-23`：41 `MOVE_PLAYER`、89 `HEAL_OVER_TIME`、91 `SPECIAL_MOVE`、
107 `QUEST_ACTION`、167 `MOVE_PET`、188 `MOVE_LIFE`、197 `NPC_ACTION`）—— 本报告 §4 的三条改值
（62/123/229）不在屏蔽表里，取证不受影响；但 S3/S4（移动、任务）切片走这条路拿不到证据，
得改用档 B 双向日志或从客户端 `packet_dump.log` 反推。已把限制写回 PLAN §3 档 A。

没有这个开关，opcode 层缺陷在日志上和"网络丢包"长得一模一样 —— 这正是 PLAN §3 里档 A/B 的价值所在。

## 2. s2c：客户端撞值 2 条

客户端 handler 表是定长数组 + **覆盖语义**：
`PacketSwitch.h:77-85` 里 `handlers[O] = make_unique<T>(...)`，重复注册只 `warn(MSG_REREGISTER)` 不阻断，
**后注册的赢**。`emplace` 在 `PacketSwitch.cpp` 构造函数里按行序执行，所以"谁在下面"决定"谁生效"。

| 值 | 两个枚举名 | emplace 行 | 生效 handler | 被覆盖 | s9 在该值上发什么 | 后果 |
| --- | --- | --- | --- | --- | --- | --- |
| 35 | `RECALCULATE_STATS`（`:85`）/ `FORCED_STAT_RESET`（`:299`） | `:640` / `:726` | `RecalculateStatsHandler` | `ForcedStatResetHandler` | `FORCED_STAT_RESET(0x23)`（`SendOpcode.java:62`，`PacketCreator.java:7305-7307` 空 body，**实发**） | **真缺陷**：强制属性解除包被派给错 handler，`g_forced_active` 的备份还原（`WaveAShellHandlers.cpp:2672-2687`）永不执行，只做了 `recalc_stats(false)`（`PlayerHandlers.cpp:245-250`）→ GM/活动强制属性结束后属性不回弹 |
| 152 | `ARIANT_RESULT`（`:190`）/ `WARN_MESSAGE`（`:348`） | `:474` / `:689` | `WarnMessageHandler` | `AriantResultHandler` | `ARIANT_RESULT(0x98)`（`SendOpcode.java:185`，**从不实发**，250 条 emitted 里没有它）；s9 侧无任何名为 `WARN*` 的 SendOpcode | **当前无害**：s9 不发 152。但 `WARN_MESSAGE = 152` 是外来版本的值，一旦 s9 启用 ARIANT 就会派错。登记待清理，不动 |

第 1 条为什么不是"删掉一个枚举名就行"：`RecalculateStatsHandler` 与 `ForcedStatResetHandler` 是两个不同实现，
`RECALCULATE_STATS` 在 s9 里根本没有对应 SendOpcode —— 也就是说 v83 客户端在 35 上只会收到 `FORCED_STAT_RESET`。
修法是让 35 归 `FORCED_STAT_RESET`（把 `:726` 的 emplace 改成注册 `FORCED_STAT_RESET`，或调整两条 emplace 的先后），
`RECALCULATE_STATS` 枚举项保留但不再注册；动手前需确认客户端里没有别处依赖 `RecalculateStatsHandler` 的副作用。

**T6 已按上述修法落地**（本表描述的是**修复前**的行为，保留作归因）：`:726` 的 emplace 已注销，
35 现在只注册 [PacketSwitch.cpp:640](file:///E:/project/MapleStory-Client/Net/PacketSwitch.cpp#L640) 的 `ForcedStatResetHandler`；
"别处依赖"已 grep 复核，结论与连带后果见 §4 表末。152 那条**故意没动**（s9 不发，改了无法验证）。

## 3. s2c：服务端会发、客户端无处理 3 条 / 服务端只定义不发 9 条

- `UNHANDLED_EMITTED`（s9 真会发，客户端连枚举都没有）：
  `GUILD_BOSS_HEALER_MOVE(287)`、`GUILD_BOSS_PULLEY_STATE_CHANGE(288)`、`WORLD_MAP_PLAYERS(376)`。
  客户端行为：`PacketSwitch.cpp:809` 走 `warn(MSG_UNHANDLED)` 后丢弃，**不崩、不影响后续包**
  （`forward()` 在 `:772` 读 opcode、`:778-790` 找 handler，找不到只 warn）。
  归因：前两条属公会 Boss 玩法（北斗不开此内容），第三条是世界地图上的玩家分布点渲染。
  判定：本版不做，显式预留（缺的是 UI 侧渲染，不是协议字段）。
- `DEFINED_NOT_EMITTED`（s9 定义但全仓无 `OutPacket.create` / `super(...)`）9 条：
  303 `ZAKUM_SHRINE`、307/308 `ADMIN_SHOP(_MESSAGE)`、319 `TOURNAMENT_CHARACTERS`、
  323 `CHARGE_PARAM_RESULT`、326/327/330/332 `CASHSHOP_*` 结果包。
  判定：**信息项，不是缺口** —— 服务端自己都不发，客户端不接不影响任何功能。

## 4. 修复清单（opcode 层，客户端改动，逐条可验收）

改动前先明确：这些都是**单值改动**，不改协议结构；每条改完必须由真机/探针实测，不接受"看着对"。

| 项 | 文件:行 | 改法 | 验收 | **状态（T6 后）** |
| --- | --- | --- | --- | --- |
| 1 | [OutPacket.h:138](file:///E:/project/MapleStory-Client/Net/OutPacket.h#L138) | `STORAGE = 309` → `62` | 打开仓库窗口 → 服务端 `StorageHandler` 有日志；改前该包在 s9 侧应完全没有 handler 命中 | ✅ 值已改，**验收未做** |
| 2 | [OutPacket.h:166](file:///E:/project/MapleStory-Client/Net/OutPacket.h#L166) | `PLAYER_INTERACTION = 314` → `123` | 雇佣商人上架/购买、交易、迷你游戏三类各点一次 | ✅ 值已改，**验收未做** |
| 3 | [OutPacket.h:173](file:///E:/project/MapleStory-Client/Net/OutPacket.h#L173) | `CASHSHOP_OPERATION = 325` → `229` | 商城购买/取放各一次 | ✅ 值已改，**验收未做** |
| 4 | [OutPacket.h:41](file:///E:/project/MapleStory-Client/Net/OutPacket.h#L41) | `LOGIN_EMAIL = 3` **不改值**；在枚举上方补 `/// s9 无此 opcode（Nexon 渠道），北斗不可用`，并把 `UILogin.cpp` 的 nexon 分支标注为不接入 | 点到 NexonID 模式时不再表现为"静默转圈"，而是有明确提示或不可选 | ⏸ 未做（属 UI 行为改动，非本协议批次） |
| 5 | [PacketSwitch.cpp:726](file:///E:/project/MapleStory-Client/Net/PacketSwitch.cpp#L726) | 35 的注册归 `FORCED_STAT_RESET`（顺序修正，`RECALCULATE_STATS` 不再占槽） | 触发 s9 `resetForcedStats()` 后属性回弹；`ForcedStatResetHandler` 日志出现 | ✅ 已注销，**验收未做** |
| 6 | [PacketSwitch.cpp:689](file:///E:/project/MapleStory-Client/Net/PacketSwitch.cpp#L689) | **暂不改**（152 上 s9 当前不发包），只在 §2 登记 | — | ⏸ 故意不改（撞值数因此是 1，不是 0） |
| 7 | — | 3 条 `UNHANDLED_EMITTED` 写进 `PLAN.md` 预留节，不写代码 | — | ✅ 已登记 |

**第 5 项的连带后果（不藏）**：`RecalculateStatsHandler`（[PlayerHandlers.cpp:245](file:///E:/project/MapleStory-Client/Net/Handlers/PlayerHandlers.cpp#L245)，
只做本地 `recalc_stats(false)` + 刷新 `UIStatsInfo`，不读包体）从此**没有注册点 = 不可达**。
这正是 §5 第 1/2 条批评 s9 的那个形态，这里明确记一笔以免下次又被当成"handler 存在就能生效"。
代码不删：枚举项留着作 v83 命名参考，删 handler 要同时动 `PlayerHandlers.h/.cpp` 两处，超出"最小改动"。

> 注：第 1~3 项的 body 是否也已对齐，本轮**不下结论**——opcode 修好只是让包能进 handler，
> 字段顺序/宽度由 P2 切片（S6 商城、S7 仓库/商店、S8 互动）逐个对账。

## 5. s9 侧缺陷（只记录，不改服务端）

1. **同值双枚举导致 handler 不可达**：`WEDDING_TALK(0x8B)` 与 `WEDDING_TALK_MORE(0x8B)`
   同值（`RecvOpcode.java:145-146`），两者都注册（`PacketProcessor.java:309/310`），
   而 `registerHandler` 是 `handlers[code.getValue()] = handler` 的数组覆盖写
   （`PacketProcessor.java:100-105`）⇒ slot 139 最终归 `WeddingTalkMoreHandler`，
   `WeddingTalkHandler` **永远不可达**。
2. **同 opcode 重复注册**：`registerHandler(RecvOpcode.CHECK_CHAR_NAME, ...)` 出现两次
   （`PacketProcessor.java:153` `CheckCharNameHandler` / `:169` `TransferNameResultHandler`），
   数组覆盖写 ⇒ **`:169` 的 `TransferNameResultHandler` 生效，`:153` 的 `CheckCharNameHandler` 不可达**。
   矩阵字段：`_meta.s9_shadowed_registrations = {CHECK_CHAR_NAME: {effective: TransferNameResultHandler,
   shadowed: [CheckCharNameHandler], lines: [153, 169]}}`。
   注意：客户端 `NAME_CHAR = 21`（`OutPacket.h:48`）发的正是这个 opcode，落到的是
   `TransferNameResultHandler` 而不是重名校验 handler —— 创角流程能否走通由 S1/S2 切片实测判定；
   若实测卡住，属服务端问题，需用户决定是否动服务端。
3. **8 个 s9 定义了但没注册 handler 的 recv opcode**（`181 - 173`）：
   `PLAYER_DC(12)`、`CLIENT_START_ERROR(25)`、`CLIENT_ERROR(26)`、`ADMIN_SHOP(68)`、
   `USE_TELEPORT_ROCK(84)`、`MATCH_TABLE(214)`、`CLOSE_ITEMUI(237)`、`MAPLETV(65534)`。
   实测结论：**客户端一条都不发**（与 103 个 c2s 值零交集，脚本已核），所以当前零影响，仅登记。
   其中 `USE_TELEPORT_ROCK(84)` 值得单独记一笔：客户端的缩地石走的是
   `USE_CASH_ITEM(79)` → s9 `UseCashItemHandler`（`PacketProcessor.java:191`），
   没有去用 s9 那个未注册的 84，方向是对的。

## 6. 生成器本轮修正记录（防止旧数字继续被引用）

第一版矩阵跑出 `MATCHED_NO_HANDLER 4` 是**我的解析器缺陷**，不是协议事实，已修：

| 缺陷 | 症状 | 修正 |
| --- | --- | --- |
| `PAT_REGISTER` 只吃无参 `new XHandler()` | `UseCashItemHandler(channelDeps.noteService())` 这类带参注册全漏 → 误报 4 条"服务端没 handler" | 允许任意实参；并支持 `XHandler.getInstance()` 单例写法（`PacketProcessor.java:172` `STRANGE_DATA`）→ handler 数 166→173（= 全部注册点） |
| `s9_emitted` 只认 `OutPacket.create(...)` | 漏 `super(SendOpcode.X)` 子类写法 | 加 `PAT_SUPER`（`SendNoteSuccessPacket.java:9`、`ShowNotesPacket.java:14`） |
| 撞值时只认第一个名字 | 139 上可能把"有 handler 的那条"读成没注册 | 优先取真注册了 handler 的名字，并新增 `s9_names_at_value` |
| 撞值表 key 是 int | JSON 把整数 key 变字符串 ⇒ `--check` 把自己刚写的文件判成漂移 | `dup_map` 输出字符串 key；顺带证明门禁真的会报警 |
| 门禁只比 5 个文件的 hash | 全树扫描出来的 `s9_emitted` / `client_sent` 变了抓不到 | `--check` 增加**语义级**行比较（`c2s`/`s2c` 逐行 + `_meta` 非易变项），实测能抓出人为改动的 `status`/`s9_emitted` |
| `parse_registrations` 默认"先注册者生效" | 把 `CheckCharNameHandler` 当成现役 handler，方向说反 | s9 是 `handlers[value] = handler` 覆盖写 ⇒ **后注册生效**；改为取最大注册行，并新增 `s9_reg_line` / `s9_shadowed_by` / `s9_registration_sites(=174)` |
| c2s 的 `s9_name` 按枚举名 join | 撞值（139）与跨名生效（35/152 同理）会认错现役 handler | 改为按**值**取该值上运行时生效的那条注册（`s9_effective`） |
| `annotation_consistent` 用 join 结果自证 | 注释明明写对（0xE5），却因枚举值错被判"注释过期" | 改为按 s9 枚举名查真值再比对 ⇒ 过期数 1→0，问题正确归位到枚举值 |

新增两个维度（这是本轮把"4 条 CLIENT_ONLY 到底算不算缺陷"问清楚的关键）：

- `client_sent` / `client_sent_at`：客户端是否真有 `OutPacket(Opcode::X)` 构造点 —— 结果 **103/103 全有**，
  死枚举假设被否证。
- `s2c.client_handler_wins` / `client_shadowed_handlers`：客户端 303 个 `emplace` 里，同一数值的
  最后一次注册才是现役 handler（`PacketSwitch.h:77-85` 覆盖写）—— 用来定位 §2 的两条被盖掉的 handler。

## 7. 未决 / 依赖

| 项 | 等什么 |
| --- | --- |
| §1 三条改值的**真机证据**（改前发不出、改后 handler 命中） | T7 真机 handshake 取证 + T9 服务端日志配对；T8 探针可先做负例（发 309 应无 handler 命中）。**值已改完（T6），这一格等的只是取证** |
| body 是否随之对齐 | P2 切片 S6/S7/S8（opcode 对了不代表字段对了） |
| ~~客户端 `RECALCULATE_STATS` 有无其他引用方~~ | **已核（T6）**：注销后剩余引用只有枚举本体 `PacketSwitch.cpp:85`、注释 `:726-731`、调试名表 `:874`，以及 handler 自身 `PlayerHandlers.h:75`/`.cpp:245` —— 构建 `EXITCODE=0`，无悬空调用。代价见 §4 末"连带后果" |
| s9 那 8 个"定义未注册"的 opcode | 已核客户端零发送 ⇒ 本协议层无影响；剩下的是"北斗是否需要这些功能"，归 T14 缺失系统归因清单 |
