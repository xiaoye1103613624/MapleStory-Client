# r03 · Angel 083 `CLogin::OnPacket` 分派表 vs 当前端 s9 登录收包注册（跨端协议对照）

> 性质：主 agent 一手取证（三路 subagent 均未覆盖此维度）。全部逐字证据来自
> `evidence/angel-login-decompile.json`（`CLogin::OnPacket @ 0x5f80ff` 等 55 函数）、
> `evidence/angel-login-decompile2.json`（40 补充函数）、
> `evidence/angel-login-chardecode.json`（`GW_CharacterStat::Decode` / `AvatarLook::Decode`），
> 以及当前端 `Net/PacketSwitch.cpp` / `Net/OutPacket.h` / `Net/Packets/LoginPackets.h` /
> `Net/Handlers/LoginHandlers.cpp` / `Net/Handlers/Helpers/LoginParser.cpp`。
> 参照基准固定 083/v83；当前服务端为 s9（BeiDou-Server_s9），两者编号不一一对应处一律标注「s9 适配」。

## 0. 结论（先看这一节）

- Angel 083 的登录收包分派是 **`CLogin::OnPacket @ 0x5f80ff` 的 21 个显式 case（0–28，含跳号）**，
  每个 case 直连一个具名处理函数；当前端是 **`Net/PacketSwitch.cpp:417-441` 的 15 条
  `emplace<OPCODE, Handler>()`** 稀疏表。
- **10 条编号与语义双对齐**、**5 条编号对齐但字节布局/语义需核**、**10 条 Angel 原生有而当前端无分支**、
  **1 条当前端独有（PING=17）**。
- 关键判定：当前端不是「缺 handler」，而是**为 s9 服务端做了字段宽度/编号适配**；这些适配在 083 基准下
  属于「不一致」，但改回原生会破坏与 s9 的联机，须作为**决策点交用户抉择**，不得盲改。
- VAC/ViewAll 出站编号是最大编号差异：GMS v83 用 **opcode 15 作 VAC 开关（`Encode1(1)` 开 / `Encode1(0)` 关）**，
  s9 用 `VIEW_ALL_CHAR=13 / PICK_ALL_CHAR=14 / VIEW_ALL_WITH_PIC=32`。

## 1. 权威源逐字

### 1.1 Angel 083 `CLogin::OnPacket @ 0x5f80ff` 完整分派表

| case | 处理函数 | EA | 当前端对应 opcode |
|---|---|---|---|
| 0 | `OnCheckPasswordResult` | `0x5f83ee` | `LOGIN_RESULT=0` → `LoginResultHandler` |
| 1 | `OnGuestIDLoginResult` | `0x5f8f27` | —（无） |
| 2 | `OnAccountInfoResult` | `0x5f92df` | —（无） |
| 3 | `OnCheckUserLimitResult` | `0x5f92ae` | `SERVERSTATUS=3` → `ServerStatusHandler` |
| 4 | `OnSetAccountResult` | `0x5fc731` | `GENDER_DONE=4` → `GenderDoneHandler` |
| 5 | `OnConfirmEULAResult` | `0x5fc838` | `CONFIRM_EULA_RESULT=5` → `ConfirmEulaResultHandler` |
| 6 | `OnCheckPinCodeResult` | `0x5fc89d` | `CHECK_PINCODE=6` → `CheckPincodeHandler` |
| 7 | `OnUpdatePinCodeResult` | `0x5fcbc1` | `UPDATE_PINCODE=7` → `UpdatePincodeHandler` |
| 8 | `OnViewAllCharResult` | `0x5facca` | `VIEW_ALL_CHAR=8` → `ViewAllCharHandler` |
| 9 | `OnSelectCharacterByVACResult` | `0x5fb245` | `SELECT_CHARACTER_BY_VAC=9` → `SelectCharacterHandler` |
| 10 | `OnWorldInformation` | `0x5f95b7` | `SERVERLIST=10` → `ServerlistHandler` |
| 11 | `OnSelectWorldResult` | `0x5f9891` | `CHARLIST=11` → `CharlistHandler` |
| 12 | `OnSelectCharacterResult` | `0x5fb541` | `SERVER_IP=12` → `ServerIPHandler` |
| 13 | `OnCheckDuplicatedIDResult` | `0x5f9c72` | `CHARNAME_RESPONSE=13` → `CharnameResponseHandler` |
| 14 | `OnCreateNewCharacterResult` | `0x5fa26c` | `ADD_NEWCHAR_ENTRY=14` → `AddNewCharEntryHandler` |
| 15 | `OnDeleteCharacterResult` | `0x5f9d15` | `DELCHAR_RESPONSE=15` → `DeleteCharResponseHandler` |
| 22 | `sub_5FB83D` | `0x5fb83d` | —（无） |
| 23 | `OnEnableSPWResult` | `0x5fb950` | —（无） |
| 26 | `OnLatestConnectedWorld` | `0x5f82f4` | —（无） |
| 27 | `OnRecommendWorldMessage` | `0x5f8340` | —（无） |
| 28 | `OnCheckSPWResult` | `0x5fba49` | `CHECK_SPW_RESULT=28`（枚举有，未见独立 handler 注册） |

default 逐字：`if (a2<125||a2>127){ if (a2>=128&&a2<=130) CMapLoadable::OnPacket(0x644446); } else CStage::OnPacket(0x775fe6);`
→ 原生把 125–127 交给 `CStage`、128–130 交给 `CMapLoadable`；当前端无此兜底语义。

### 1.2 当前端 `Net/PacketSwitch.cpp:52-76` recv 枚举（逐字）

`LOGIN_RESULT=0, SERVERSTATUS=3, GENDER_DONE=4, CONFIRM_EULA_RESULT=5, CHECK_PINCODE=6,
UPDATE_PINCODE=7, VIEW_ALL_CHAR=8, SELECT_CHARACTER_BY_VAC=9, SERVERLIST=10, CHARLIST=11,
SERVER_IP=12, CHARNAME_RESPONSE=13, ADD_NEWCHAR_ENTRY=14, DELCHAR_RESPONSE=15, PING=17,
CHANGE_CHANNEL=16, CHECK_SPW_RESULT=28`。

`:58-60` 逐字注释：「GENDER_DONE/CONFIRM_EULA_RESULT 在 s9 下不发送（性别/EULA 已由
LOGIN_RESULT(auth success) 闭环），仅作协议占位；CHECK_PINCODE/UPDATE_PINCODE/VIEW_ALL_CHAR 由 s9 实际发送」。
`:433-440` 同口径复述。

### 1.3 当前端 `Net/OutPacket.h:39-64` 出站枚举（逐字）

`LOGIN=1, LOGIN_EMAIL=3, CHARLIST_REQUEST=5, SERVERSTATUS_REQUEST=6, ACCEPT_TOS=7,
SET_GENDER=8, VIEW_ALL_CHAR=13, PICK_ALL_CHAR=14, SELECT_CHAR=19, PLAYER_LOGIN=20,
NAME_CHAR=21, CREATE_CHAR=22, DELETE_CHAR=23, PONG=24, REGISTER_PIC=29,
SELECT_CHAR_PIC=30, VIEW_ALL_WITH_PIC=32, SERVERLIST_REQUEST=11`；
`:42-45` `RELOG=0x1C` 裁决「明确不做」不占枚举位。无 `CHECK_PASSWORD`/`CHECK_DUPLICATED_ID`/
`WORLD_STATUS`/`SERVERSTATUS` 命名（查重由 `NAME_CHAR=21` 承担）。

## 2. 双对齐 10 条（编号一致 + 语义一致，可直接确认「正确」）

`0 / 5 / 6 / 7 / 8 / 9 / 13 / 14 / 15 / 28`。其中：

- case 13 Angel `OnCheckDuplicatedIDResult`（`DecodeStr(name)` + `Decode1(result)`，
  0=可用 / 1=`Error(5)` 已占用 / 2=`Error(10)` 非法名 / 其它=`Error(18)`）
  ↔ 当前 `CharnameResponseHandler` `Net/Handlers/LoginHandlers.cpp:364-377`
  （`read_string()` 名 + `read_bool()` used → `UIRaceSelect::send_naming_result`）。**一致**。
- case 14 Angel `OnCreateNewCharacterResult`（首字节 `Decode1` 结果码，0 成功后
  `GW_CharacterStat::Decode` + `AvatarLook::Decode`；失败码 10/26/30）
  ↔ 当前 `AddNewCharEntryHandler` `LoginHandlers.cpp:379-402`（首字节 `read_byte()` 结果码，
  `result != 0` 丢弃，否则 `LoginParser::parse_charentry`）。**一致**（当前端注释 :381-384 已自证）。
- case 15 Angel `OnDeleteCharacterResult`（`Decode4(cid)` + `Decode1(state)`，失败码
  6/9/10/18/20/22/24/26/29）↔ 当前 `DeleteCharResponseHandler` `LoginHandlers.cpp:404-452`
  （`read_int()` cid + `read_byte()` state，s9 实发 `0x09/0x14/0x16/0x1A/0x1D/0`）。
  **字段布局一致，失败码集合因服务端而异（已注明 s9 适配）**。
- case 28 Angel `OnCheckSPWResult`（`Decode1` → `CLoginUtilDlg::Error(93)` + `SetFocus` + 清门闩）
  ↔ 当前 `CHECK_SPW_RESULT=28` 命名一致，需核是否注册独立 handler [推断]。

## 3. 编号对齐但字节布局/语义需核 5 条（❌ 一致性缺口，须逐条决策）

| case | Angel 083（权威） | 当前端 s9 | 冲突性质 |
|---|---|---|---|
| 3 | `OnCheckUserLimitResult` @`0x5f92ae`：`Decode1`×2 → `CUIWorldSelect::UserLimitResult(v2,v3)`（`@0x6283c1`），语义 = [世界索引][人数上限结果] | `SERVERSTATUS=3 → ServerStatusHandler` `LoginHandlers.cpp:176`，`read_short()` 一次 → `set_server_status(0/1/2)`「0 Normal/1 Highly populated/2 Full」 | 同为 2 字节但解释不同；原生是「按世界查人数上限」，当前是「单一 short 拥挤度」。s9 适配 |
| 4 | `OnSetAccountResult` @`0x5fc731`：`Decode1`×2；成功分支 `COutPacket(9)` + `Encode1(1)`×2 + `EncodeStr("")` 请求世界表 | `GENDER_DONE=4 → GenderDoneHandler` `LoginHandlers.cpp:454-471`：`read_byte()` gender → remove GENDER/LOGINWAIT → `ServerRequestPacket()` | 语义同（性别/EULA 后取世界表）但原生读 2 字节、当前读 1 字节；原生出站 opcode 9 vs 当前 `SERVERLIST_REQUEST=11` |
| 10 | `OnWorldInformation` @`0x5f95b7`：`Decode1`/`DecodeStr`(世界名)/`Decode1`/`DecodeStr`(世界描述)/`Decode2`×2/`Decode1`/循环 `DecodeStr`(频道名)+`Decode4`+`Decode1`×3 / `Decode2` / 尾部 `Decode2`×2+`DecodeStr`；末尾 `DrawWorldItems` + `CLogin::ChangeStep(this,1)` | `SERVERLIST=10 → ServerlistHandler` `LoginHandlers.cpp:255-286`：`LoginParser::parse_world` 逐世界 + `0xFF` 结束包 → `draw_world()` | 拆包方式不同：原生单包含世界+频道全表并直接切 step=1；s9 多包 0xFF 终止。s9 适配 |
| 11 | `OnSelectWorldResult` @`0x5f9891`：错误码后 `Decode1`(charcount) + 循环 `GW_CharacterStat::Decode` + `AvatarLook::Decode` + 后续 rank 字段 | `CHARLIST=11 → CharlistHandler` `LoginHandlers.cpp:288-334`：`read_byte()` channel_id + `read_byte()` charcount + `parse_charentry` + `read_byte()` pic + `read_int()` slots | 原生首字节是错误码（0/12/23 才进解析），s9 首字节是 channel_id；字段宽度见 r04。s9 适配 |
| 12 | `OnSelectCharacterResult` @`0x5fb541`：`Decode1`(v4)+`Decode1`(v17)，switch 失败码 2–10/13–15/16,17,21,25 内嵌 switch；成功 `Decode4+Decode2+Decode4+Decode1+Decode4` + `SP_1243_GAMEIN` | `SERVER_IP=12 → ServerIPHandler`：`parse_login` 4×IP+short port + `cid=read_int()` + `PlayerLoginPacket(cid)` | 同为「进游戏」但字段布局完全不同（原生含 v17 二级码与多段 int，s9 是 4 段 IP + port + cid）。s9 适配 |

## 4. Angel 原生有、当前端无对应分支 / 不发 10 条（❌ 缺失，须标注）

| case | 原生语义（逐字） | 当前端状态 |
|---|---|---|
| 1 | `OnGuestIDLoginResult` @`0x5f8f27`：GUEST 登录回包，`Decode1`×2 + 错误码 switch | 无（`GUEST_LOGIN` 在 Q2 明确不做总表） |
| 2 | `OnAccountInfoResult` @`0x5f92df`：`Decode1` + 错误码 switch(13/14/15 → `Error(21)`/`YesNo2(27)`+开官网/`YesNo2(26)`) | 无 |
| 22 | `sub_5FB83D` @`0x5fb83d` | 无 |
| 23 | `OnEnableSPWResult` @`0x5fb950`：`Decode1`×2，成功 `Notice(39/40)`，失败码 6/9/20/22/23 → `Error(18/91/92/93)` | 无（SPW 二级密码开启结果） |
| 26 | `OnLatestConnectedWorld` @`0x5f82f4`：`Decode4` worldid != 253 → `CUIWorldSelect::SetFocusWorld(0x6272d0)` 高亮上次登录世界 | 无（**交互缺口**：当前端无「记住上次登录世界并高亮」） |
| 27 | `OnRecommendWorldMessage` @`0x5f8340`：`Decode1` count → 循环 `Decode4`+`DecodeStr` 填 `ZArray<RECOMMENDWORLDMSG>` → `CLogin::ViewRecommendWorld` | 无（**UI 缺口**：`CUIRecommendWorld` 推荐世界弹窗在当前端无对应类） |
| default 125–127 | `CStage::OnPacket(0x775fe6)` | 无兜底 |
| default 128–130 | `CMapLoadable::OnPacket(0x644446)` | 无兜底 |
| `PING=17` | Angel 分派表无此 case | 当前端 `:420 emplace<PING, PingHandler>()`（`Net/Handlers/CommonHandlers.cpp:34,36` 功能性非 stub）→ **当前端独有** |
| `CHECK_SPW_RESULT=28` | 有 case | 当前端枚举有、独立 handler 待核 [推断] |

## 5. ★ VAC / ViewAll 出站编号差异（最大编号不一致）

- Angel 083 逐字：`CLogin::MakeVACDlg @ 0x5fb0e1` = `COutPacket(a1: 15)` + `Encode1(1)`
  （打开 VAC 请求）；`CLogin::ResetVAC @ 0x5fb155` = `COutPacket(a1: 15)` + `Encode1(0)`
  （关闭 VAC）。**GMS v83 用单个 opcode 15 + 1 字节开关表示 VAC 开/关**。
- Angel `CUIAvatarVAC::OnButtonClicked @ 0x60af0a`：仅 `a2==1001`（BtEnter）→
  `CLogin::SendSelectCharPacketByVAC(0x5f76ae)`；`CUILoginStart::SendViewAllCharacterPacket @ 0x629d0a`
  门 `*(v3+91)==0` → `CLogin::SendViewAllCharPacket(0x5f6d6a)`。
- 当前端：`Net/OutPacket.h` `VIEW_ALL_CHAR=13`（`ViewAllCharPacket()` 空 body）/ `PICK_ALL_CHAR=14` /
  `VIEW_ALL_WITH_PIC=32`；`IO/UITypes/UIViewAllChar.cpp:180-231` 逐字注释「VAC 选角必须走
  PICK_ALL_CHAR(14) / VIEW_ALL_WITH_PIC(32)，误发 CHAR_SELECT(19) 会少一个 world int」。
- 判定 [观察]：编号体系不同（083 单 15 开关 vs s9 三 opcode），属 s9 服务端协议适配；
  当前端注释已自证「少一个 world int」的坑，逻辑与 083 的 `SendSelectCharPacketByVAC`
  （遍历 world 找 cid 再编码）语义一致。**不改**，仅标注。

## 6. 与 Q2 / 083 基准的关系

- Q2（`docs/features/feature-gap-inventory-2026-11/README.md` §9.5）已证明协议层
  「不缺出站封包」；本轮进一步证明登录链路**「不缺收包 handler，缺的是 Angel 原生独有的
  4 个交互/UI 分支（case 1/2/23/26/27）与 4 类字节布局一致性」**。
- 按运行时 MEMORY 硬约束：功能判定只以 083 系证据为准。本文件所有「原生」列均来自 Angel 083 IDB；
  「当前端」列来自 s9 适配源码。**二者冲突处不得自动判当前端为「错」，须交用户抉择**
  （改回原生会断 s9 联机）。
- 参见 `notes/r04-charentry-byte-layout.md`（CHARLIST 逐字段一手字节对照）。
