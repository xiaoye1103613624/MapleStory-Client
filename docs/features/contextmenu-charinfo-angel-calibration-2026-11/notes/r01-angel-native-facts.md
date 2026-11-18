# r01 — Angel v83 右键上下文菜单原生事实（反编译证据）

来源：GUI-bound IDA 13337 对 `\`Angel.i64` 的 Hex-Rays 反编译；证据原件见 `evidence/`。
局限：伪代码为观察层；偏移/条件语义部分为基于调用与数据流的推断（标注「推断」）。未执行运行时。

## 1. 弹出条件（HandleRButtonClk @ 0x95028f）

调用链：`CUserLocal::HandleRButtonClk` → `sub_9726EA(dword_BEBFA8, &cursor)`（`0x9726ea`）→ 命中返回角色对象 `v29`；**仅 `if (v29 != 0)` 才 `new CUIContextMenu` 并 `DoModal`**。

`sub_9726EA` 命中算法（已逐字确认）：
```
v2 = *(this+12)                    // 遍历角色链表（CAvatar 列表，推断为同屏玩家）
while (v2):
    avatar = v2+4
    CAvatar::GetBodyRect(avatar+16+136, &rect, 1)   // 取角色「身体矩形」
    if dword_BF0484(&rect, cursor.x, cursor.y) != 0: break  // 点-in-rect 命中
    ... 继续遍历；链表走完 return 0
return avatar+16 的某字段（角色标识）
```
结论：**Angel 只在光标落入某个玩家角色身体矩形内时弹菜单；空地/怪物/NPC 不弹。** 印证用户需求。

附带行为：右击先做「雇员气球改名」（CEmployeePool::FindBalloon）与「临时状态图标取消」（CTemporaryStatView::FindIcon → 取消技能/道具），与本菜单无关。

## 2. 窗口构造（CUIContextMenu::CUIContextMenu @ 0x996570）

签名：`(ZXString<char> sCharacterName, POINT pt, int)`。`this+50` 存角色 cid（初值 -1，实际由外部/OnCreate 用），名字由构造存入。

行数与条件标志（关键）：
```
this+51 = 5        // 基础行数
this+52 = 0; +53 = 0; +54 = 0    // 三个可选按钮标志
if ( *(*ctx+11988)==0 || *(*ctx+12166)==*(*ctx+8352) ):   // 未组队 或 同频道
    this+52 = 1; this+51 = 6
if ( *ctx+12360 != 0 && (**ctx+12372 == *ctx+8352 || sub_53088F(*ctx+8352)==2) ):  // 有公会且(同公会/某权限)
    ++this+51; this+53 = 1
if ( *ctx+14420 != 0 && *ctx+14376 < *ctx+14378 ):       // 家族系统开且人数<上限
    ++this+51; this+54 = 1
height = 15 * (this+51 + 2)
CreateDlg(x, y, 100, height, 10, 1, 0)
```
推断：`+52/+53/+54` 分别控制 Party / Guild / Family 三个可选按钮的显示；条件基于**玩家自身状态**（是否组队、是否有公会、是否可收晚辈），而非简单 `cid != self`。这与当前端 `other = cid!=self` 的判定**语义不同**。

## 3. 菜单项与按钮（OnCreate @ 0x9967fb）

固定/条件与坐标（按钮 id 为 Angel 内部 id；`a4=x, a5=y`）：

| 项 | SP_ 节点 | 按钮id | 坐标 | 显示条件 |
|---|---|---|---|---|
| BtInfo | SP_2601 | 2000 | (9,24) | 固定 |
| BtSave | SP_2602 | 2001 | (9,39) | 固定 |
| BtTrade | SP_2610 | 2002 | (9,54) | 固定 |
| BtWhisper | SP_2603 | 2004 | (9,69) | 固定 |
| BtParty | SP_2608 | 2005 | (9,84) | `this+52 != 0` |
| BtGuild | SP_2609 | 2006 | (9,99/v111) | `this+53 != 0` |
| BtClaim | SP_2604 | 2007 | (9,v111) | 固定 |
| BtFamily | SP_2611 | 2008 | (9,v111+15) | `this+54 != 0` |

按钮控件大小：每个 `Alloc(1444)` + `CCtrlButton`（与 Item 窗按钮同规格）。t/c/s 三节点：`SP_2605/2606/2607 = UI/UIWindow.img/ContextMenu/{t,c,s}`。

## 4. 点击行为（HandleRButtonClk 中 DoModal==1 后 switch(this+50)）

`this+50` 即所选按钮结果 id = `按钮id - 2000`（case 0..8）：

| case | 对应项 | 行为 |
|---|---|---|
| 0 | BtInfo | `CWvsContext::SendCharacterInfoRequest(cid, 0, 0)` 打开个人情报 |
| 1 | BtSave | `SetClipboardText(name)` + `CUIStatusBar::StartChat(0)` —**复制名字到剪贴板并拉起聊天输入框（不是加好友！）** |
| 2 | BtTrade | `CField::SendInviteTradingRoomMsg(name, 0)` |
| 3 | (交易变体) | `SendInviteTradingRoomMsg(name, 1)` |
| 4 | BtWhisper | `CUIStatusBar::SetWhisperTarget(name, 1)` 设置私聊目标（由 StatusBar 处理），不直接发包 |
| 5 | BtParty | `CField::SendJoinPartyMsg(name)` |
| 6 | BtGuild | `sub_5306D5(name)`（邀请加入公会） |
| 7 | BtClaim | `SendClaimRequest(GetChatLog(), name)` 举报 |
| 8 | BtFamily | `CWvsContext::SendRegisterJunior(name)` 登记晚辈（家族） |

> 注意：按钮 id 2000..2008 中缺 2003；case 2/3 都是交易（bTrade 参数 0/1，推断为「生成交易/愿望交易」二选一，可能对应 BtTrade 的不同态）。BtSave=复制名字这一条与当前端「加好友」实现存在明确语义冲突，须以 IMG 节点文案（BtSave 贴图）交叉确认后定案。

## 5. 与个人情报窗的关系
- BtInfo → `SendCharacterInfoRequest` → 服务端回 CHAR_INFO → 本端 `PlayerInteractionHandlers.cpp` 打开 `UICharInfo`。
- Angel 个人情报窗 `CUIUserInfo` 全方法反编译已从 corewindow calibration 提取到 `evidence/angel-userinfo-decompile-extracted.md`（17 方法：OnCreate / OnButtonClicked / Draw / SetState / SetLayer / Update / SetPet* / Avatar* 等），待 r03 逐项对照。
