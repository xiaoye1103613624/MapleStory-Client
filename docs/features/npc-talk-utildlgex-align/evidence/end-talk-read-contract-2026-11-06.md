# NPC ScriptMessage 收口包与 s9 服务端读包契约对齐（2026-11-06）

## 结论（决定性更正）

盘点项「#23 ScriptMessage 未接 UI `-1` 收口」是**过时判断**：收口代码早已存在，但其中藏着一个**真协议 bug**。

`Net/Handlers/NpcInteractionHandlers.cpp` 的 `NpcDialogueHandler::handle` 里有 10 处在「未接 UI / 缺窗体资源 / emplace 失败」时发 `NpcTalkMorePacket(msgtype, static_cast<int8_t>(-1))`。该构造编码为 **仅 2 字节**：`write_byte(lastmsg); write_byte(response=0xFF)`。对 `msgtype ∈ {2, 5, 6, 13}`，s9 服务端 `NPCMoreTalkHandler` 会强制 `readString()`，2 字节包触发 readShort 越界 / 负长度，脚本挂死或读包异常。

## 服务端读包契约（决定性证据）

文件：`E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/net/server/channel/handlers/NPCMoreTalkHandler.java`（118 行）

- `:36` `byte lastMsg = p.readByte();`
- `:38-53` `if (lastMsg == 5 || lastMsg == 6) { String returnText = p.readString(); ... action = 1; }` —— **5/6 无条件 readString，无 action 字节**。
- `:54` `byte action = p.readByte();`
- `:56-74` `if (lastMsg == 2 || lastMsg == 13) { if (action != 0) { String returnText = p.readString(); ... } else { getCM().dispose(); ... } }` —— **2/13 且 action!=0 强制 readString；action==0 直接 dispose**（不读 string、不 invokeFunction）。
- `:75-91` `lastMsg 9/10`：action==1 且 available>=8 读 `long uniqueId`。
- `:92-108` else 分支：按剩余长度读 `int` / `unsignedByte` / `-1`（selection）。
- `:111-117` `cmRouting`：把 (mode, type, selection) 转给脚本。

`readString()` 语义：`E:/project/BeiDou-Server_s9/.../net/packet/ByteBufInPacket.java:45-79`

```java
short length = readShort();
byte[] stringBytes = new byte[length];
byteBuf.readBytes(stringBytes);
```

`Net/OutPacket.cpp:100-117` `write_string` = `write_short(length)` + 逐字节；空串 = `short(0)`。

`NPCScriptManager.java:221-224`（`nextLevel`）：`SEND_LAST_NEXT/SEND_NEXT/SEND_LAST/SEND_OK/SEND_ACCEPT_DECLINE/SEND_YES_NO` 分支 `if (mode == -1) { dispose(c, true); return; }` → 普通 talk 类型 `mode=-1` = 结束对话，正确。

## 逐 msgtype 收口形状（修复后）

| msgtype | 含义 | 服务端读取 | 旧包（bug） | 新包 | 依据 |
|---|---|---|---|---|---|
| 0/1/3/4/7/8/9/10/12/14 | Say / YesNo / GetNumber / Simple / Avatar / Pet / AcceptDecline / Mirror | else 分支读 selection，mode=-1 → dispose | `(x,0xFF)` 2B ✅ | `(x,-1)` 不变 | NPCScriptManager.nextLevel :221-224 |
| 2 / 13 | GetText / AskBoxText 缺 UtilDlgEx | action!=0 强制 readString | `(x,0xFF)` 2B ❌ 短包 | `(x,0)` → dispose | NPCMoreTalkHandler :56-74；UINpcTalk.cpp:533 同形 |
| 5 | InitialQuiz 缺 UI / emplace 失败 | 无条件 readString（无 action） | `(5,0xFF)` 2B ❌ 短包 | `(5, "")` = `05 00 00` 3B | NPCMoreTalkHandler :38-53；NpcInteractionPackets.h:134-139 AskQuiz tag |
| 6 | SpeedQuiz 缺 UI / emplace 失败 | 无条件 readString（无 action） | `(6,0xFF)` 2B ❌ 短包 | `(6, "")` = `06 00 00` 3B | NPCMoreTalkHandler :38-53；NpcInteractionPackets.h:142-148 AskSpeedQuiz tag |

## 修复（原子改动）

文件：`Net/Handlers/NpcInteractionHandlers.cpp`（唯一代码改动；`git diff --stat` = 47 insertions / 19 deletions）

在 `drain_rest` lambda 之后、`switch (msgtype)` 之前插入集中分派 lambda：

```cpp
auto end_talk = [](int8_t lastmsg)
{
    switch (lastmsg)
    {
    case 2:
    case 13:
        NpcTalkMorePacket(lastmsg, static_cast<int8_t>(0)).dispatch();
        break;
    case 5:
        NpcTalkMorePacket(NpcTalkMorePacket::AskQuiz{}, std::string()).dispatch();
        break;
    case 6:
        NpcTalkMorePacket(NpcTalkMorePacket::AskSpeedQuiz{}, std::string()).dispatch();
        break;
    default:
        NpcTalkMorePacket(lastmsg, static_cast<int8_t>(-1)).dispatch();
        break;
    }
};
```

- 10 处 `NpcTalkMorePacket(msgtype, static_cast<int8_t>(-1)).dispatch();` → `end_talk(msgtype);`（旧行号 :149/:163/:192/:206/:273/:287/:315/:329/:346/:363）。
- 9 处 LOG 文案 `NpcTalkMore(-1)` → `end_talk`（保留 `reserved/no-UI` 上下文）。
- 不改 `IO/UITypes/UINpcTalk.cpp`（并行会话在制品 M，且其 5 处 CLOSE `-1` 属普通 talk 类型，服务端 dispose 正确）；不改 `Net/Packets/NpcInteractionPackets.h`（AskQuiz/AskSpeedQuiz tag 构造已存在）。
- 类型核对：`NpcInteractionHandlers.cpp:48 const int8_t msgtype = recv.read_byte();` 与 lambda 参数匹配；`:35 #include <string>` 已存在。

触发场景（修复前会挂脚本）：InitialQuiz/SpeedQuiz 缺 `UIWindow.img/InitialQuiz` / `SpeedQuiz` 资源；UtilDlgEx 缺资源且 msgtype=2/13；`end_unhandled` default 分支落到 msgtype 2/5/6/13。

## 验证状态

- ✅ 服务端读包契约：三处源码逐行读齐（NPCMoreTalkHandler / ByteBufInPacket / NPCScriptManager）。
- ✅ 客户端 wire 形状：`Net/Packets/NpcInteractionPackets.h:80-170` 全构造 + `Net/OutPacket.cpp` write_string。
- ✅ 静态核对：`end_talk` 10 处替换、唯一残留 `-1` 在 lambda default、LOG 9 处、tag 构造签名。
- ⏳ build_smoke：`GATE OK` + `EXITCODE=0`（后台 job 收口中）。
- ⏳ 真机回归：按用户 m01905 决定挂起，待用户截图/指派；不视为通过也不视为失败。

## 相关物证

- `docs/features/npc-talk-utildlgex-align/evidence/a11h-scriptmessage-onpacket-ida.md`（IDA `sub_74660A` switch 表 0..14；:69 未接 UI 类型读完后收口避免脚本挂死）。
- `docs/features/npc-talk-utildlgex-align/evidence/a11j-askboxtext-ida.md`（AskBoxText OK=`13+1+string` / 非OK=`13+0` / Close 静默；本仓 Close/Esc 发 `(13,0)`）。
- `docs/features/npc-talk-utildlgex-align/evidence/a11j-quiz-speedquiz-ida.md`（Quiz/SpeedQuiz 出站 `byte5|6 + string`，无 action；空串=超时/放弃）。
- `docs/features/pending-live-tests/README.md` §23 / §25 / §26 / §27 / §28（验收清单，§23 期望已同步更正）。
