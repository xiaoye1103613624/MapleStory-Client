# 三方对照摘要（证据引用，2026-11-23）

本文件汇总本次审计实际引用的证据定位，供复核。所有路径相对 `E:\project\`。

## A. 本端原生基准（权威）— BeiDou V17.7 IDA

- `MapleStory-Client/docs/features/npc-talk-utildlgex-align/evidence/a11b-sendsimple-next-ida.md:29-42`
  - sub_9AB92A case4：BtNext(id=8193) 取 list[cur].sel 后 Finish；TEXT 按钮 id 0x2000=Prev/0x2001|8193=Next/1=OK/2=Close。
- `.../a11h-scriptmessage-onpacket-ida.md:13-19, 39-56`
  - 公共头：byte speakerType + int templateId + byte msgtype + byte speaker；Say 尾 (speaker&4)?int + string + byte prev + byte next。
- `.../a11i-askavatar-sendstyle-ida.md:29-62`
  - AskAvatar 固定钮槽；OK=(7,1,sel)；Close 静默；本仓 adapt Cancel=(7,0)。
- `.../a11e-gettext-getnumber-layout-ida.md`、`a11j-askboxtext-ida.md`：输入槽几何 / BoxText 出站。
- `.../end-talk-read-contract-2026-11-06.md:13-40`：s9 NPCMoreTalkHandler 读包契约。

## B. 服务端读包契约 — BeiDou-Server_s9

- `BeiDou-Server_s9/gms-server/src/main/java/org/gms/net/server/channel/handlers/NPCMoreTalkHandler.java:36-91`
  - lastMsg 5/6 无条件 readString；2/13 action!=0 readString；9/10 action==1 读 int64；else 读 selection。
- `BeiDou-Server_s9/gms-server/src/main/java/org/gms/scripting/npc/NPCScriptManager.java:190-228`
  - nextLevel：mode==-1 → dispose；mode==0 → level+lastLevel（回退）；mode==1 → level+nextLevel。
  - **这是缺陷 1 的判定依据**：ESC 发 0 会走回退而非关闭。

## C. MinMS（JCC fork，参考）

- `MinMS/src/client/IO/UITypes/UINpcTalk.cpp`
  - `resolve_dialogue_mode():1335-1363`：type 4/5/14=SELECTION；无 style 派生 Next/Prev 变体（fork 差异）。
  - `draw():1365-1397`：fill 拉伸（vtile）、DIALOG_TEXT_X=203（156*1.3）、无打字机、TEXT 顶对齐（fork 定制）。
  - `button_pressed():1604-1865`：OK/Yes=1、Prev/No=0、Close=(type,-1)、Style-cancel=(7,0)。
  - `send_key():2368-2393`：escape 非 STYLE → `NpcTalkMorePacket::close(type)`=(type,-1)。**缺陷 1 交叉旁证**。
  - `Net/Packets/NpcInteractionPackets.h:45-71`：selection=(lastmsg,1)+int；close=(lastmsg,-1)。

## D. v95 GMS（port-095 证据，语义参考，非国产095）

- `MapleStory-Client/docs/features/port-095/notes/ida-095-initial.md:14-20,124-138,208`
  - 中国 095 加壳不可读；数据来自 GMS v95 IDB，且明确「GMS v95 ≠ 国产 095」。
- `.../port-095/evidence/opcode-v95/dec_send.zip`（12 个 OnAsk* 反编译）
  - 响应包统一 `[lastMsg][action][payload]`；Say=0/Text=3/Number=4/Menu=5/BoxText=14/Avatar=8；
  - Say 的 DoModal==3 → 无包（静默）。lastMsg 为「请求+1」重编号，与本端 s9 不同，**不作本端判据**。

## E. 192 客户端（资源侧）

- `MapleStory-Client/docs/features/ui006-cn-port-2026-11/notes/ida-192.md:5,22-64,106-111`
  - 二进制加壳，IDB 仅 3 函数/10 字符串，无 dialog 逻辑可逆向。
- `MapleStory-Client/docs/_uiwindow2_tree_d2.json`
  - `UIWindow2.img/UtilDlgEx` t=519×28 / c=519×13 / s=519×44 / bar=109×19；按钮 BtPrev/Next/OK/Close/Yes/No/Q* 等；无 origin/无状态帧（仅节点树）。版本差异，非本端缺陷。

## F. 本次改动定位

- `IO/UITypes/UINpcTalk.cpp:704-716`：ESC 按类型分派 response（缺陷 1 修复）。
- `locale/en-US.ini:59-60`、`locale/zh-CN.ini:59-60`：tip818/819 换行改为单字面 `\n`（缺陷 2 修复）。
- `Graphics/GraphicsGL.cpp:993-1041`：LayoutBuilder token 解析（`\` + n/r/rn），缺陷 2 的判定依据。
- `IO/UITypes/UINotice.cpp:70-76`：tip 渲染只删真 CRLF，缺陷 2 旁证。
