# c2s 出站形状 vs s9 读包契约 系统对账（2026-11-06）

## 目的

承接 `cb59477e`（NPC 收口包按 s9 读包契约分派，修掉 `NpcTalkMorePacket(msgtype,-1)` 对
msgtype∈{2,5,6,13} 的短包）后，用 `protocol/opcode-matrix.json` 作桥，系统核对
「带剩余长度守卫 / 特殊读法」的 s9 channel handler 与客户端 `Net/Packets/*.h` 写包形状，
找下一处「有证据、能纯代码闭环」的不一致。

**结论：无新增不一致。** 15 个守卫 handler + 5 处 `readLong` + `readPos` + `readBytes`
逐字段对过，形状全部吻合；`cb59477e` 的 NPC 收口短包是本轮唯一实质缺陷，已修并提交。
剩余未闭环项均为「证据阻塞」或「产品裁决」，按门禁5（禁想象修改）不动。

## 方法名全集（先钉死，避免正则漏匹配）

客户端 `Net/Packets/` 的 `write_*` 直方图：
`write_byte` 238 / `write_int` 209 / `write_short` 90 / `write_string` 79 /
`write_time` 16 / `write_point` 6 / `write_long` 5 / `write_hardware_info` 5 /
`write_damage_line` 2 / `write_random` 1。

s9 `.../net/server/channel/handlers/*.java` 的 `read*` 直方图：
`readInt` 277 / `readByte` 184 / `readShort` 121 / `readString` 102 /
`readLong` 5 / `readMapleAsciiString` 2（均为注释）/ `readUnsignedByte` 1 /
`readShortPoint` 1（注释）/ `readPos` 1 / `readBytes` 1。

`Net/OutPacket.cpp:90-134` 字节数（决定形状是否可能错位）：
- `write_time()` = `write_int(timestamp)` → **4B**（不是 8B；服务端对应读 `readInt`）
- `write_point()` = 2×`write_short` → 4B（对应 `readPos()` 的 2×`readShortLE`）
- `write_string()` = `short` 长度前缀（**GBK 字节数**）+ N 字节
- `write_random()` = `write_int` → 4B、`write_long` → 8B、`write_hardware_info` = 2×string

## 特殊读法对齐（8B / 4B 易错点，逐处坐实）

| s9 `readLong`(8B) | 客户端 `write_long` | 结论 |
| --- | --- | --- |
| `MovePetHandler.java:37` int petId + long | `PetPackets.h:64-67` `write_int(petId)+write_long(0)+write_byte(1)+writemovement` | ✓ |
| `PetAutoPotHandler.java:36-40` byte+long+int+short+int | `PetPackets.h:125-129` `write_byte+write_long+write_int+write_short+write_int` | ✓ |
| `NPCMoreTalkHandler.java:80` AskPet uniqueId | `NpcInteractionPackets.h:158` `write_long(unique_id)` | ✓ |
| `AbstractDealDamageHandler.java:703` long_lines 真实伤害 | `AttackAndSkillPackets.h:53-57` long_lines 分支 `write_int(vanilla)+write_long(real)` | ✓（受 `DamageLinesConfig.isLongLines()` / 客户端 `get_damage_long_lines()` 成对开关） |
| `CashShopSurpriseHandler.java:44` cashId | 客户端无对应 c2s 触发类（收银台惊喜由服务端内部发起） | 无出站，N/A |
| `MoveDragon/MoveSummon` skip8 | `SummonDragonPackets.h:48` `write_long(0)` | ✓ |

`readPos()`（`ByteBufInPacket.java:45-49` = 2×`readShortLE`，4B）↔ `write_point()`：
`ItemPickupHandler.java:44-47` `readInt(ts)+readByte+readPos+readInt(oid)` ↔
`GameplayPackets.h:266-269` `write_int(0)+write_byte(0)+write_point+write_int(oid)` ✓

`readBytes(length-9)`（`NPCAnimationHandler.java:44` NPC Move 分支 length>6）：客户端
`NpcActionMovePacket` 已接但缺真机 197 length>6 hex 样例（m01905 真机冻结，不动）。

## 15 个带守卫 handler 逐字段对账

| s9 handler（守卫行） | 读形状 | 客户端 OutPacket | 结论 |
| --- | --- | --- | --- |
| `KeymapChangeHandler:36` `available>=8` | int mode; mode0: int n +(int key,byte type,int action)×n | `PlayerPackets.h:108-120` `write_int(0)+write_int(n)+n×(int,byte,int)` | ✓ |
| `QuickslotKeyMappedModifiedHandler:15` `available==SIZE*4` | SIZE×`readInt` 截断 byte[8] | `SkillMacroPackets.h:43-48` 8×`write_int`；`QuickslotBinding.QUICKSLOT_SIZE=8` | ✓ |
| `ChangeMapHandler:73` `available==0` / `:92` chasing | byte died+int map+string portal+byte+short wheel（+GM chasing 2×int） | `GameplayPackets.h:29-39` 主构造 + 空构造（cash shop 出口 `available==0`） | ✓（`write_short(wheel)` 被 `:90 readByte wheel`+`:92 readByte chasing` 两个 readByte 吃掉） |
| `ChangeMapSpecialHandler:38-40` | byte+string+short | `MapInteractPackets.h:20-22` `write_byte(0)+write_string+write_short(0)` | ✓ |
| `InnerPortalHandler:43` `available>0` / `:87-88` | byte 标志+string portalName | `MapInteractPackets.h:34-35` `write_byte(0)+write_string` | ✓ |
| `QuestActionHandler:81-92,126` | byte action+short questid [+int npc][+short×2 pos][+short selection] | `QuestPackets.h:17-22` `write_byte+write_short` + (1/2/4/5)`write_int(npcid)` | ✓（pos/selection 为可选尾，客户端按需短发，服务端守卫 `available` 兼容） |
| `MultiChatHandler:46-56` | byte type+byte n+int×n+string | `MessagingPackets.h:51-57` `write_byte+write_byte+n×write_int+write_string` | ✓ |
| `AdminCommandHandler:175/177` | mode 0x77: int 或 short | `GameplayPackets.h:187` AdminCommandPacket(Mode) | 仅 GM 测试位，无形状错配 |
| `FamilySeparateHandler:43` `available>0` | 可选 int juniorCid | `FamilyPackets.h:84` SENIOR 写 int / `:95` JUNIOR 空 | ✓ |
| `MTSHandler:65` `available>0` | byte op + 分支 | `MiscOutPackets.h:14` `EnterMtsPacket()` 空体（进 MTS）；上架分支客户端未发 | N/A（按需） |
| `RPSActionHandler:25` `available==0` | byte mode [+byte answer] | `RpsPackets.h:13-18` 单 mode / mode+answer | ✓ |
| `SpecialMoveHandler:128/131/134` | skip(11) / short / `available==5`→point | `AttackAndSkillPackets.h:292-299` `write_time+write_int(skill)+write_byte(level)` +1004 `skip(2)` | ✓（末尾 point 为可选，客户端 TODO 保留） |
| `SkillEffectHandler:53-57` | int skill+byte level+byte flags+byte speed+byte aids | `AttackAndSkillPackets.h:345-349` `write_int+4×write_byte` | ✓ |
| `TakeDamageHandler:70-73,153` | int ts+byte from+byte elem+int dmg+int mobid+int oid+byte dir[+reflect 尾] | `AttackAndSkillPackets.h:232-238` 逐字段 | ✓（reflect 尾 `available>=2` 可选） |
| `PlayerInteractionHandler:322` `available>1` | minigame 密码 string 可选 | `PlayerInteractionPackets.h` 各 action 包（CREATE/SET_ITEMS/BUY/READY…） | ✓（按需） |
| `NPCMoreTalkHandler` 全分支 | 见 `end-talk-read-contract-2026-11-06.md` | `NpcInteractionPackets.h` 各构造 | ✓（`cb59477e` 已修 2/5/6/13 短包） |

## opcode matrix 全量状态

`protocol/opcode-matrix.json` c2s 145 条：**144 MATCHED + 1 CLIENT_ONLY**。
（脚本统计用 `status` 字段；非 `MATCHED` 仅 `LOGIN_EMAIL`。）

## 唯一 CLIENT_ONLY：`LOGIN_EMAIL`(3) — 显式预留，待产品裁决

- 客户端：`Net/OutPacket.h:41 LOGIN_EMAIL = 3`；`Net/Packets/LoginPackets.h:62-85`
  `LoginEmailPacket(email,pass)`；`IO/UITypes/UILogin.cpp:470`
  `if (nexon) LoginEmailPacket(...).dispatch(); else LoginPacket(...)`。
- `nexon` 默认 `false`（`UILogin.cpp:56`），仅当用户点登录页 NexonID 切换
  （`UILogin.cpp:558 nexon=true`）才置真；当前北斗账号走 `LOGIN`(1)，正常登录不受影响。
- s9：`RecvOpcode.java` 无 `0x03`（`0x02 GUEST_LOGIN` / `0x04 SERVERLIST_REREQUEST`），
  `PacketProcessor` 无 opcode 3 handler → nexon 渠道在 s9 服务端**无接收方**。
- 性质：「北斗服务端不支持 Nexon 邮箱登录」的产品决策，需用户裁决
  （禁用 nexon 渠道 / 映射回 LOGIN(1) / 补 s9 handler）。按门禁5 不凭空改，
  也不改 opcode matrix 的 `CLIENT_ONLY` 标记（它如实反映现状）。

## 结论 / 未闭环项（按性质，均非「想象可解」）

1. **本轮系统对账无新增可纯代码闭环的形状错配**；协议层主干与 s9 读包契约对齐。
   `cb59477e` 的 NPC 收口短包是唯一实质缺陷，已修。
2. 真机证据阻塞（m01905 冻结）：NPC_ACTION 197 length>6 hex、软键盘 37 键对拍、
   200+ UI 经验条展示、blink/chatBalloon 补拍。
3. 组件/资源证据阻塞：AskBoxText 多行 wire 格式（本仓 Textfield 单行）、
   AskPet 亲密/饱食 gage（UtilDlgEx 无专用节点，现 ColorBox 占位）、
   SKILL_EFFECT 松开取消（KeyDown cancel 证据）。
4. 产品裁决：LOGIN_EMAIL nexon 渠道、`ExpTable` index9/10 反序、
   s9 cap255 / 客户端 LEVELCAP=256 / 原生 LVL3 mod 300 上限策略三者统一。

## 补遗：s2c 全量状态也扫过（确认无「可补的客户端 handler」遗漏）

opcode matrix `s2c` 是 list（字段 `s9_name`/`s9_value`/`client_names[]`/
`client_handler_wins`/`s9_emitted`/`status`），313 条：
**304 HANDLED + 9 DEFINED_NOT_EMITTED**。

9 条 `DEFINED_NOT_EMITTED`（`s9_emitted=false`、`client_names=[]`）经
`SendOpcode.java` 注释 + 全仓 grep 坐实为 **s9 定义了但从不发送的死 enum**：

| s9_value | s9_name | s9 发送方 |
| --- | --- | --- |
| 303 | `ZAKUM_SHRINE` | 0 |
| 307 | `ADMIN_SHOP_MESSAGE` | 0（enum 注释 "lame :P"） |
| 308 | `ADMIN_SHOP` | 0（唯一命中是 `RecvOpcode.java:84` 的 c2s 0x44，非 s2c 发送） |
| 319 | `TOURNAMENT_CHARACTERS` | 0（enum 注释 "they never coded this"） |
| 323 | `CHARGE_PARAM_RESULT` | 0 |
| 326 | `CASHSHOP_PURCHASE_EXP_CHANGED` | 0 |
| 327 | `CASHSHOP_GIFT_INFO_RESULT` | 0 |
| 330 | `CASHSHOP_REGISTER_NEW_CHARACTER_RESULT` | 0 |
| 332 | `CASHSHOP_GACHAPON_STAMP_RESULT` | 0 |

→ 客户端没有对应 handler 是**正确的**（服务端从不发），不是待补实现；
`total empty client_names = 9` 与这 9 条一一对应，无「s9 会发但客户端漏接」的
`HANDLED` 之外的缺口。故两个方向都已收敛：c2s 出站形状对齐，s2c 无漏接。

## 相关物证

- `docs/features/npc-talk-utildlgex-align/evidence/end-talk-read-contract-2026-11-06.md`（本轮起点）
- `docs/features/level-cap-256-2026-11/evidence/verify.txt`（三服务端逐值对账）
- `protocol/opcode-matrix.json`（145 c2s / 313 s2c）
- s9 handler 目录：`E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\net\server\channel\handlers\`
