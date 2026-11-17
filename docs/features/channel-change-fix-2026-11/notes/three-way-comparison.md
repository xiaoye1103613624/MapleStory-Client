# 换频道三方对照（s9 服务端 / MinMS / Angel）

> 只读核对产物。所有结论来自源码 / IDA 反编译，账号等敏感信息脱敏。
> 采集日期：2026-11-16 ~ 2026-11-17

## 1. s9 服务端（权威）

根目录：`E:/project/BeiDou-Server_s9/gms-server/src/main/java/`

- opcode 方向：收包（C→S）`RecvOpcode.CHANGE_CHANNEL=0x27`（RecvOpcode.java:57）；发包（S→C）`SendOpcode.CHANGE_CHANNEL=0x10`（SendOpcode.java:44）。handler 注册 PacketProcessor.java:171。本端发 0x27 / 收 0x10 已正确。
- `ChangeChannelHandler.java:36-51`：读 `readByte()+1` + `readInt()` 丢弃；同频道 disconnect+autoban（:41-44）；商城/小游戏/商店/交易中 enableActions 拒绝（:45-48）；否则 `c.changeChannel(channel)`（:50）。validateState=isLoggedIn()（AbstractPacketHandler.java:27-31）。
- `Client.changeChannel` Client.java:1500-1569：失败分支（封禁/死亡/FieldLimit.CANNOTMIGRATE/迷你地牢/频道禁用）→ enableActions 或 notice；旧频道清理 :1522-1543（广播离场、buff/disease 存档、removePlayer）；saveCharToDB():1545；setSessionTransitionState():1563（loginState=1 LOGIN_SERVER_TRANSITION + 按 IP 登记 cid）；sendPacket(getChannelChange):1565 发给目标频道。未主动断旧 socket，由客户端收 0x10 后自行重连。
- 回包结构 `PacketCreator.java:910-917 getChannelChange` = `writeByte(1)`（成功码）+ 4B IP + 2B port；ByteBufOutPacket.java:27 opcode 2B 小端、:59 writeShortLE。body 共 7B，**无 cid**，port 需小端读。
- 新连接登录 `PlayerLoggedinHandler.java`：validateState=!isLoggedIn()（:108-110）；cid=p.readInt()（:114）；validateCharacteridInTransition(c,cid)（:156，Server.java:1548-1562，use_ip_validation 配置）；updateLoginState(2)（:212）；cserv.addPlayer+恢复 buff/disease（:237-250）；c.sendPacket(getCharInfo)（:252）。
- SET_FIELD 下发 = getCharInfo `PacketCreator.java:1012-1024`：opcode SET_FIELD(125) + writeInt(channel-1) + writeByte(1) + writeByte(1) + writeShort(0) + 3 随机 int + addCharacterInfo + writeLong(time)；mapid/spawnpoint 藏在 addCharStats:215-216。换线登录只走 getCharInfo，不走 getWarpToMap(:1111)。
- 地图实体 spawn 独立包：PlayerLoggedinHandler.java:277 getMap().addPlayer → :278 visitMap；MapleMap.java:2562 sendObjectPlacement(:3088-3136 下发怪/NPC/掉落/reactor)、:2559 broadcastSpawnPlayerMapObjectMessage(:3014-3045)。**SET_FIELD 只带角色自身数据，实体靠后续 spawn 包**。
- 未验证项：客户端 parse_login（本端已自行核对）、use_ip_validation / change_channel_force_return 实际配置值、各 sendSpawnData opcode 细节。

## 2. MinMS（开源客户端对照）

根目录：`E:/project/MinMS/src`

- `ChangeChannelHandler::handle` LoginHandlers.cpp:347-389：:349 success=read_byte()、:350 success!=1 则失败 UI；:371 cid=Stage::get().get_player().get_oid()；:389 `Session::queue_channel_join(addr,port,cid)`（不在此重连）。
- 延迟重连 `Session.cpp:185-221` queue_channel_join/poll_channel_join；关键注释 :187-189 "Never reconnect inside read()/process(): buffer和reading guard还活着，onmessage→read 会丢 channel SET_FIELD"；reading 守卫+状态机；Journey.cpp:120-150 update() read() 在前 poll 在后，join==1 才 PlayerLoginPacket。
- **本端不移植原因**：本端阻塞式 Winsock 同步 init，无异步 onmessage 重入（AGENTS.md 记忆"抄算法不整仓粘贴"）。
- SET_FIELD 落地 `SetfieldHandlers.cpp`：handle(set_channel:182)；transition:118-168（先 clear:166 再 load:145）；set_field:228-350 双分支（from_charselect / has_char_entry:241-244）、was_ingame→change_state(GAME):260-263、loadplayer:259。
- `Stage.cpp`：clear:245-280（chars/npcs/mobs/drops/reactors/doors/mists/summons 全清）、load:133-176、loadplayer/store/has_char_entry:178-198、set/get_channel:200-208。频道号状态源在 Stage，SET_FIELD 更新。

## 3. Angel（原版客户端，IDA 反编译）

IDA session 56a86f32，`E:/MXD/反编译IDA/v83/Angel.i64`，imagebase 0x400000。

- `CClientSocket::ProcessPacket@0x4965f1`：case 0x10 → `OnMigrateCommand@0x496701`（socket 层拦下，不进 CLogin::OnPacket；补证历史笔记"case16 在 CLogin"是错误的）。
- `OnMigrateCommand@0x496701`：Decode1 成功码 !=0 → 若在游戏中先 set_stage(CInterStage) 切过渡 → Decode4 IP + Decode2 port → `CWvsContext::IssueConnect@0xa032df`（先 Close 旧 socket 再建新连）。失败抛 CDisconnectException。
- 请求侧 `CUIChannelShift::SetRet@0x9959bb` → `CField::SendTransferChannelRequest@0x5304af`（COutPacket(39=0x27) + Encode1(channel) + Encode4(time)，置 pending）；拒绝回包 OnTransferChannelReqIgnored@0x531a08。
- 重建 `CStage::OnPacket` case125 → `OnSetField@0x776020`：v102==0 分支（游戏内换线）`CWvsContext::GetCharacterData` 复用快照（同 MinMS has_char_entry）；FieldFactory 按 mapid 新建 field → set_stage(新 CField)；set_stage@0x777347 释放旧 stage（置空+虚析构）再入场。
- field 内对象靠 spawn 子包池填充（CUserPool/CMobPool/CNpcPool/CDropPool）。
- 未验证项：换线成功后 ChannelID 写回 CConfig 的具体调用点（未做 xrefs）。
- IDA 工具坑：`decompile` 传 max_lines/offset 报 "Invalid params:unexpected parameters"；headless 只用 `decompile(addr, include_addresses, database=session)`。

## 4. 综合判定

换线真落地三要素 = **真连新服务端 + 清旧场 + 新频道 spawn 重建**。本端源码三要素全具备且与三方同构。假换线根因 = 旧进程跑旧 canon（旧版 SetFieldHandlers 在 UICharSelect missing 时 return 丢整个 SET_FIELD），非协议或逻辑方向错误。trace 里 channel=1/3/5/11 变化是真重连新频道的铁证（旧版连上了但落地被丢弃）。
