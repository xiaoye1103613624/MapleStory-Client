# R27-B7-3 最终实现逻辑：频道更换（CHANGE_CHANNEL）三处断链

用户报的缺陷原话：

> 频道更换点击也没有反映

「没有反映」其实是**三个独立断点叠在一起**，缺一都还是没反映。全部按服务端本体取证，不猜字段。

---

## 0. 服务端取证源（本机 `E:\project\BeiDou-Server_s9`，只读）

| 事实 | 出处 |
| --- | --- |
| 出站 `CHANGE_CHANNEL(39)` = `byte channel0基` + `int32`（读走即弃） | [ChangeChannelHandler.java:37-51](file:///E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/net/server/channel/handlers/ChangeChannelHandler.java) `int channel = p.readByte() + 1; p.readInt();` |
| 回包 `CHANGE_CHANNEL(16)` = `byte 1` + 4 字节 IPv4 + `short port` | [PacketCreator.java:891-897 getChannelChange](file:///E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/util/PacketCreator.java) |
| 频道服端口 = `7575 + (channel-1) + world*100` | [Channel.java:82,136](file:///E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/net/server/channel/Channel.java) ⇒ 本世界 Scania(id 0) CH.1/2/3 = 7575/7576/7577 |
| 换频后重连的是**频道服**，入场券是 `PLAYER_LOGIN(20)` = `int charId` | [Client.java:1515-1567 changeChannel](file:///E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/client/Client.java) → `getChannelChange(socket[0], socket[1])`；[PlayerLoggedinHandler.java:110-114](file:///E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/net/server/channel/handlers/PlayerLoggedinHandler.java) `final int cid = p.readInt();`，`validateState = !c.isLoggedIn()` |
| `SET_FIELD(125)` 的首个 int = **当前 0 基频道号** | [PacketCreator.java:994-996 getCharInfo](file:///E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/util/PacketCreator.java) `p.writeInt(chr.getClient().getChannel() - 1); p.writeByte(1); p.writeByte(1); p.writeShort(0);` |

## 1. 断点一：出站包少 4 字节 ⇒ 服务端抛异常，永无回包

[Net/Packets/GameplayPackets.h](../../../../../Net/Packets/GameplayPackets.h) 的 `ChangeChannelPacket`
原来只写 1 字节 channel，服务端那句 `p.readInt()` 直接抛 ⇒ 连 `getChannelChange` 都发不出来。

```cpp
ChangeChannelPacket(uint8_t channel) : OutPacket(OutPacket::Opcode::CHANGE_CHANNEL)
{
    write_byte(channel);   // 0 基
    write_int(0);          // 服务端读走即弃
}
```

> **预留**：那 4 字节在原生 v83 客户端里的**语义**未取证（IDA 打开 `BeiDou.exe.i64`，8 处 `channel` 名字
> 全是字符串，没定位到封包构造点）。服务端把它丢弃，填 0 不影响换频；若日后换服务端实现需按原生值校准。

## 2. 断点二：重连到新频道服以后，客户端一句话不说

回包只给 IP/port，**不带 charId**；正常登录时那句 `PLAYER_LOGIN(cid)` 是 `ServerIPHandler` 发的
（`SERVER_IP(12)` 的包尾正好带 charId）。换频路径上没有任何人补这一手，于是：

客户端连上 7576 → 服务端 `ChannelServerInitializer` 记一条「连接到了大区 0, 频道 2」→ 双方干等。
真机表现就是人还站在原地、`packet_dump.log` 里只剩心跳。

**处置**（全部落在本线可改的文件里，避开并行会话持有的 `PlayerHandlers.cpp`）：

| 文件 | 改动 |
| --- | --- |
| [Net/Handlers/LoginHandlers.cpp](../../../../../Net/Handlers/LoginHandlers.cpp) `ServerIPHandler` | 进这条路径先 `LoginParser::clear_channel_reentry()`（防上一次换频没走到回包而残留标记），再把包尾 charId 交给 `note_channel_entry_cid(cid)` |
| [Net/Handlers/Helpers/LoginParser.cpp](../../../../../Net/Handlers/Helpers/LoginParser.cpp) `parse_login` | 重连成功后，若挂过标记就补发 `PlayerLoginPacket(channel_entry_cid)` |
| [IO/UITypes/UIChannel.cpp](../../../../../IO/UITypes/UIChannel.cpp) `change_channel` | 发包前 `LoginParser::expect_channel_reentry()` |

`Session::init()` 是同步的（`socket.open` → 阻塞读握手密钥 → `connected = true`），
所以补发点放在 `reconnect()` 之后是安全的；`cid == 0` 或未连上时只留一条 `LOG(LOG_NETWORK)`，不硬发。

## 3. 断点三：`Configuration::channelid` 从不更新 ⇒ 「当前频道」标记永远亮在旧格

`SET_FIELD` 的首个 int 就是权威频道号，但 [Net/Handlers/SetFieldHandlers.cpp](../../../../../Net/Handlers/SetFieldHandlers.cpp)
`SetFieldHandler::handle` 读出来只喂给 `change_map/set_field`，没有回写 `Configuration`。补：

```cpp
int32_t channel = recv.read_int();
Configuration::get().set_channelid(static_cast<uint8_t>(channel));
```

另外 `UIChannel` 是**常驻单例**（各处都是 `get_element<UIChannel>()`，取不到才 `emplace`），
`current_channel` 只在 ctor 里抓过一次 ⇒ 换频成功后再开窗，亮的还是登录时那条。
`UIStateGame::update()` 只对 `is_active()` 的窗体派发，所以把刷新点放在 `UIChannel::update()`，
等价于「每次打开窗体时重取」。

## 4. 真机验收（判据 = 协议面 + 画面面，两条都要）

`_live/_r27_b73c_run.txt` 原样：

```
OUTBOUND 0x27: [SEND] PacketDump : 27 00 01 00 00 00 00
期望包体（CH.2 -> channel=1）：27 00 01 00 00 00 00 -> MATCH
INBOUND 0x10: [RECV] PacketDump : 10 00 01 7f 00 00 01 98 1d
OUTBOUND 0x14 PLAYER_LOGIN 条数=1 [SEND] PacketDump : 14 00 01 00 00 00
换频后 SET_FIELD 首 int（0 基频道号）=1
换频前 UIChannel 当前槽(0基)=0
再次打开 UIChannel，当前槽(0基)=1
```

逐条读法：

| 证据 | 结论 |
| --- | --- |
| `27 00 01 00 00 00 00` | 出站 39 + 0 基频道 1（CH.2）+ 4 字节 0，包体与 `ChangeChannelHandler` 的读法逐字节对齐 |
| `10 00 01 7f 00 00 01 98 1d` | 回包 = `byte 1` + `127.0.0.1` + `0x1d98` = **7576** = CH.2 的频道服端口（§0 的端口公式） |
| `14 00 01 00 00 00` ×1 | 重连后补发的 `PLAYER_LOGIN(charId=1)`，**恰好一条**（正常登录路径没被双发） |
| `SET_FIELD` 首 int = **1** | 服务端确认角色现在在 0 基频道 1 = **CH.2**。换频前同一条包是 `0` |
| 当前槽 0 → 1 | `_live/_r27_b73c_08_channel_again.png`：CH.2 是蓝底「当前」板，CH.1/CH.3 是灰板 |
| 服务端日志 | `客户端 127.0.0.1 连接到了大区 0, 频道 2`（`ChannelServerInitializer.initChannel.info1`） |

画面面另有一处旁证：换频前站在 Orbis 的星空站台，换频后落在 Henesys 的木台——
新频道服是另一份 map 实例，人确实被搬走了。

> 帧与日志都不含账号口令；`_live/` 里刻意没放登录页帧和 `01 00` LOGIN 包行。

### 4.1 连续第二次换频（CH.2 → CH.3）—— 排除「单次是运气」

`_live/_r27_b73d_run.txt`。这一轮**中间没有再经过 `SERVER_IP`**，所以它钉的是另外两点：
`channel_entry_cid` 必须「登录期存一次、之后一直可用」，而「待重入」标记每轮都要重新挂、重新消费。

```
本轮开局 UIChannel 当前槽(0基)=1（应为 1 = CH.2）      ← 上一轮的状态活到了这一轮
OUTBOUND 0x27: [SEND] PacketDump : 27 00 02 00 00 00 00   ← channel=2（CH.3）
INBOUND 0x10: [RECV] PacketDump : 10 00 01 7f 00 00 01 99 1d  ← 0x1d99 = 7577 = CH.3
OUTBOUND 0x14 PLAYER_LOGIN 条数=1 [SEND] PacketDump : 14 00 01 00 00 00
换频后 SET_FIELD 首 int（0 基频道号）=2（应为 2 = CH.3）
再次打开 UIChannel，当前槽(0基)=2
```

`_live/_r27_b73d_06_channel_again.png`：蓝底「当前」板已在 CH.3，CH.1/CH.2 都是灰板；
画面左侧出现了另一个玩家——不同频道是各自的一份 map 实例，这也算一条旁证。

## 5. 未取证 / 预留

| # | 项 | 状态 |
| --- | --- | --- |
| R1 | 出站那 4 字节的原生语义 | 未取证（IDA 未定位到构造点）。服务端丢弃，填 0；换服务端实现时需按原生值校准 |
| R2 | 「同频道换频」服务端直接 `disconnect` | 客户端已有同频道保护（`button_pressed` 拒绝点当前格 + `change_channel` 同值走 `cancel()`），但**保护依赖 §3 的 channelid 回写**；没回写就会误发同频道包被服务端踢线 |
| R3 | 迷你地牢 / 现金商店 / 交易 / 摆摊中换频 | 服务端回 `enableActions` 或 `serverNotice(5, …)`，客户端只解 `10 00` 的 IP/port，那两条拒绝路径**未接**（属协议线 T14，不在本批） |
| R4 | 「频道变更」窗里那排空槽（第 4–6 格） | 按 `channel_count` 向上取整到整排画空板，与原生 `频道变更.png` 的 6 格 1 行一致，见 [channel-layout.md §6.2](channel-layout.md) |

## 6. 复跑

```
python E:/project/_r27_b73c.py     # 全量：登录 → 进世界 → 目录 → 频道变更 → CH.1→CH.2
python E:/project/_r27_b73d.py     # 追加：接在同一台客户端上再换一次 CH.2→CH.3（不重新登录）
```

两个脚本都只驱动已在跑的窗口；`_r27_b73d.py` 找不到窗口就直接退出，不会自己登录。

脚本从 `xy-config.yaml` 读账号口令，只在环境变量里传，日志仅打印长度；
所有键鼠走 `send_key`/`send_click`（SendInput）并在每次发送前断言前台。
