# s9 服务端聊天回显矩阵（2026-11-04 源码取证）

来源：`E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms`（opcode 门禁基线 s9 `44d0d5d9ca`）。
本表决定 Q5/Q6 客户端本地回显的**精确边界**——该回显的不回显=自己看不见自己说话；不该回显的回显=历史里双份。

| 频道 | 证据（文件:行为） | 发送者收到回包？ | 客户端动作 |
|---|---|---|---|
| General | `net/server/channel/handlers/GeneralChatHandler.java` → `chr.getMap().broadcastMessage(PacketCreator.getChatText(...))`；`server/maps/MapleMap.java` 单参重载转调 `broadcastMessage(null, packet, +Inf, null)`，核心循环 `if (chr != source)` 当 source=null 时恒真 | ✅ 含自己 | 禁止本地回显 |
| Party | `net/server/world/World.java:1071 partyChat`：`if (!(partychar.getName().equals(namefrom)))` 显式跳过发送者 | ❌ | 必须本地回显 `[Party] ` |
| Guild | `Server.guildChat → Guild.guildChat:463 → broadcast(PacketCreator.multiChat(...,2), cid)`（exception=发送者 cid） | ❌ | 必须本地回显 `[Guild] ` |
| Alliance | `Server.java:849 allianceMessage(..., player.getId(), -1)` exception 排除 | ❌ | 必须本地回显 `[Alliance] ` |
| Buddy | `World.java:1082 buddyChat` 只遍历 recipients（发送者不在自己的收件列表里） | ❌ | 必须本地回显 `[Buddy] ` |
| Spouse | `SpouseChatHandler.java`：`spouse.sendPacket(...)` **且** `c.sendPacket(...)` | ✅ 本人也回 | 禁止本地回显 |
| Whisper | `WhisperHandler.handleWhisper`：`target.sendPacket(getWhisperReceive)` + `user.sendPacket(getWhisperResult(name,!hidden))` —— 发送者只得**结果标志**，不得正文 | ⚠️ 仅结果 | 正文暂存；WhisperResult ok=true 才回显 `(To X) 我 : 正文` |
| Megaphone | `UseCashItemHandler case 2/5/6 → Server.broadcastMessage(int world, Packet)`（`Server.java:1130`，遍历世界全频道全玩家） | ✅ 全服含自己 | 禁止本地回显 |

## 附：数据边界取证
- 好友名单（BUDDY 收件人）：`Net/Handlers/BuddyHandlers.cpp case 7 updateBuddylist` 是唯一把 `BuddyListData` 写进 `UIBuddyList` 的路径；窗未创建=表空=发 0 收件人=服务端循环不命中（等价原生静默失败）。客户端 `collect_online_buddy_cids` 注释已钉死此边界。
- 普通聊天防刷屏/长度门禁（spam(7)、length>127 PACKET_EDIT）在 General/Multi/Whisper handler 一致存在；Q5 多出的回显是纯本地行为，不触发任何服务端路径。
- `MapleMap.broadcastMessage(Character source, ...)` 三参重载带 `repeatToSource`，GeneralChat 不走它 —— 结论不受该重载影响。
