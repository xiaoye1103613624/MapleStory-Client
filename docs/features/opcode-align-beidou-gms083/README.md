# opcode 对齐核对：MapleStory-Client (v83) ↔ 北斗 gms083

## 结论

**MapleStory-Client 与北斗 gms083 的 opcode 本就完全对齐，无需修改任何代码。**

- 协议版本已为 83：`Net/Cryptography.cpp:55` → `static const uint8_t MAPLEVERSION = 83;`
- s2c（服务端→客户端）：**57 / 57** 按值精确匹配北斗 `SendOpcode.java`
- c2s（客户端→服务端）：**44 / 45** 按值精确匹配北斗 `RecvOpcode.java`，唯一例外见下

因此本项（align-opcodes）的产出是**核对结论**，而非代码改动。

## 证据

全量对照表见 [`notes/opcode-align-full.md`](notes/opcode-align-full.md)。

核对方法：按**值**比对（同一数值在线路上即同一封包），而非按名称。
两份 opcode 的命名体系不同（客户端为 HeavenClient 命名，北斗为 OdinMS 命名），
故名称不同但值相同即视为同一封包，例如：

| 客户端名称 | 值 | 北斗名称 | 语义 |
|---|---|---|---|
| LOGIN_RESULT | 0x00 | LOGIN_STATUS | 登录结果 |
| CHAR_MOVED | 0xB9 | MOVE_PLAYER | 玩家移动 |
| ATTACKED_CLOSE | 0xBA | CLOSE_RANGE_ATTACK | 近战攻击 |
| MODIFY_INVENTORY | 0x1D | INVENTORY_OPERATION | 背包操作 |
| CHANGE_STATS | 0x1F | STAT_CHANGED | 属性变更 |
| SPAWN_CHAR | 0xA0 | SPAWN_PLAYER | 生成玩家 |

## 主链路对照（登录 → 进图 → 操作）

| 环节 | 客户端 | 值 | 北斗 | 值 |
|---|---|---|---|---|
| 登录 | LOGIN | 0x01 | LOGIN_PASSWORD | 0x01 |
| 服务器列表 | SERVERLIST_REQUEST | 0x0B | SERVERLIST_REQUEST | 0x0B |
| 角色列表 | CHARLIST_REQUEST | 0x05 | CHARLIST_REQUEST | 0x05 |
| 选择角色 | SELECT_CHAR | 0x13 | CHAR_SELECT | 0x13 |
| 玩家登录 | PLAYER_LOGIN | 0x14 | PLAYER_LOGGEDIN | 0x14 |
| 进图（服务端） | SET_FIELD | 0x7D | SET_FIELD | 0x7D |
| 移动 | MOVE_PLAYER | 0x29 | MOVE_PLAYER | 0x29 |
| 说话 | GENERAL_CHAT | 0x31 | GENERAL_CHAT | 0x31 |
| NPC 对话 | TALK_TO_NPC | 0x3A | NPC_TALK | 0x3A |
| 捡物 | PICKUP_ITEM | 0xCA | ITEM_PICKUP | 0xCA |

## 唯一差异与处置

- **`LOGIN_EMAIL = 0x03`**：北斗 `RecvOpcode` 中**无** 0x03 定义，即服务端未实现邮箱登录。
  - 处置：客户端走 `LOGIN`（0x01，密码登录）路径即可，邮箱登录不可用。
  - 本项不影响握手、登录、选角、进图、移动、攻击、NPC 对话主链路。

## 复现方式

用按值比对的脚本分别解析：

- 客户端 s2c：`Net/PacketSwitch.cpp` 的 `enum Opcode`（InPackets）
- 客户端 c2s：`Net/OutPacket.h` 的 `enum Opcode`（OutPackets）
- 北斗：`gms-server/src/main/java/org/gms/net/opcodes/SendOpcode.java`（s2c）、`RecvOpcode.java`（c2s）

再按数值做映射即可复现上表。

## 重要备注：cms085 不可用于对接北斗

`XyMapleStory-cms085` 是 **85** 版本客户端，其 `Protocol/Fyms085Opcodes.h`（cms085 服务端）与北斗 gms083 差异巨大：

- c2s：39 项中 **30 项不同**
- s2c：65 项中 **48 项不同**

即两者是**两套不同的 opcode 表**，不可混用。这反过来印证：
**MapleStory-Client（v83）才是对接北斗 gms083 的正确基线项目。**
