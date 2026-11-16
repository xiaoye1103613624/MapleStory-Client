# r1 · Angel 083 IDA 取证：玩家列表五页签（CUIUserList + CTab*）

> 取证环境：Angel 083 客户端 IDB（module=\`Angel.exe, imagebase 0x400000, hexrays ready），idalib-mcp headless @127.0.0.1:13339（DB session `e344ff00`，路径 `E:\MXD\反编译IDA\v83\083\v83.i64`）。
> 脚本：`C:\Users\11036\.rea\angel_start.ps1` / `angel_call.ps1`（-Tool/-ArgsJson/@file: 参数，database 自动注入）。
> 反编译产物：`tools/_ida_userlist_r1/*.dec.txt`（31 个文件，索引见 `evidence/ida-r1-decompile-index.md`）。
> 取证日期：2026-11-16。round68 已落盘 `tools/_ida_pb/*`（OnCreate/Draw/SetButton/SetScrollBar/CUIPartyHP）不再重复取证。

## 1. 窗口结构（CUIUserList::OnCreate @0x919b73）

- 页签控件：`CCtrlTab` id=2001 @(6,23) w=270，资源 `Tab/enabled` + `Tab/disabled`，5 格循环。
- **页签顺序（native）：FRIEND=0 / PARTY=1 / GUILD=2 / GUILD_ALLIANCE=3 / BLACKLIST=4**。
  ⚠️ 当前端 Tab 枚举 BLACKLIST=3 / UNION=4 与 Angel 顺序相反，须对齐（Angel 第 4 格=家族联盟、第 5 格=拒绝列表）。
- 按钮全部 `CCtrlButton`（四态），创建后 push 进 CUIUserList 偏移 +1484 起的按钮数组：
  - `+1484` 11 个（好友）｜`+1572` 9 个（组员）｜`+1644` 12 个（家族）｜`+1788` 11 个（联盟）｜`+2004` 2 个（拒绝列表）

## 2. 按钮全表（id / 资源 / 坐标 / 语义）

### 好友页（2010..2020）
| id | 资源 | 坐标 | 语义（CTabFriend::OnButtonClicked 分发） |
|----|------|------|------|
| 2010 | BtAddFriend | (5,81) | 添加好友（sub_909FCA(0)） |
| 2011 | BtAddGroup | (72,81) | 添加分组（sub_90ADD3(名,1)） |
| 2012 | BtGroupWhisper | (8,341) | 组 Whisper（sub_90B0B6） |
| 2013 | BtShowOnline | (232,81) | 在线可见筛选（sub_90B0C1） |
| 2014 | BtMod | (8,362) | OnModify：选中好友→改其分组；选中组行→重命名组（默认组不可改删） |
| 2015 | BtDelete | (49,362) | 删除好友（sub_90A303） |
| 2016 | BtChat | (99,341) | 好友聊天（Messenger 邀请） |
| 2017 | BtWhisper | (254,341) | OnWhisper → SetWhisperTarget+StartChat |
| 2018 | BtMessage | (254,362) | OnSendMemo：现金道具 slot5（Memo）→ SendConsumeCashItemUseRequest；无 → Notice SP_729 |
| 2019 | BtParty | (99,362) | OnPartyInvite → SendJoinPartyMsg(名) |
| 2020 | BtBlock | (182,362) | OnBlockFriend：翻转 block 位 + ChangeBlockOption(cid,1/0,block) + 聊天回显 SP_4143/4144；贴图 BtBlock/BtUnblock(SP_2103/2104) 切换 |

### 组员页（2200..2208）
| id | 资源 | 坐标 | 语义 |
|----|------|------|------|
| 2200 | BtCreate | (13,355) | 创建队伍（sub_90E131） |
| 2201 | BtInvite | (72,365) | OnInvite：CUtilDlgEx 输名字 → SendJoinPartyMsg |
| 2202 | BtKick | (190,344) | OnKick → SendKickPartyMsg(选中名) |
| 2203 | BtWithdraw | (190,364) | 退出（sub_90E2CE） |
| 2204 | BtWhisper | (249,345) | OnWhisper → SetWhisperTarget+StartChat |
| 2205 | BtChat | (249,365) | OnChat → CUIMessenger::TryNew + SendInviteMsg |
| 2206 | BtChangeBoss | (131,365) | OnChangeBoss：YesNo 确认 → SendChangePartyBossMsg(名) |
| 2207 | BtHP | (131,345) | 血条开关（sub_90E51C） |
| 2208 | BtSearch | (72,345) | UI_Toggle(22)＝寻找组队窗 |

### 家族页（2030..2041 + 动态 2070..2075）
| id | 资源 | 坐标 | 语义 |
|----|------|------|------|
| 2030 | BtInvite | (9,316) | 邀请入会（sub_9111BE） |
| 2031 | BtKick | (159,336) | OnKick → SendKickGuildMsg(名) |
| 2032 | BtWithdraw | (80,316) | 退会（sub_9112B4） |
| 2033 | BtUp | (9,336) | 地位↑（sub_911341(1)） |
| 2034 | BtDown | (80,336) | 地位↓（sub_911341(0)） |
| 2035 | BtWhere | (159,316) | 位置（sub_911C5C） |
| 2036 | BtWhisper | (238,316) | Whisper（sub_9113C0） |
| 2037 | Btnotice | (287,80) | 公告（sub_9114F1） |
| 2038 | BtChat | (238,336) | OnChat → Messenger::SendInviteMsg |
| 2039 | BtInfo | (238,363) | 资料切换（vtable[14]，家族信息/成员列表双态） |
| 2040 | BtPartyInvite | (159,363) | OnPartyInvite → SendJoinPartyMsg |
| 2041 | BtGuildBBS | (80,363) | UI_Open(11)＝家族留言板 |
| 2070..2075 | 动态二级钮 | 动态 | DestroyGuildButton + sub_911D25(1/0)/sub_911D63(1/0)/sub_911DA5(1/0)（二级确认/展开） |

### 家族联盟页（2050..2060 静态 + 2080..2094 动态翻页）
| id | 坐标 | 语义 |
|----|------|------|
| 2050 (9,316) | sub_914CFE | 2051 (159,336) sub_914EB4 | 2052 (80,316) sub_914E39 | 2053 (9,336) sub_91505B(1) 地位↑ | 2054 (80,336) sub_91505B(0) 地位↓ | 2055 (238,316) sub_9150E7 Whisper | 2056 (238,336) sub_91517B Chat | 2057 (238,363) vtable[15] 资料切换 | 2058 (159,363) OnPartyInvite→SendJoinPartyMsg | 2059 (287/288,80) 公告 | 2060 (159,316) sub_915983 |
| 2080..2084 | 动态 | DestroyAllianceButton + sub_915A02(idx) |
| 2085..208E 奇偶 | 动态 | sub_915A3B / sub_915A78（联盟多页/分支切换） |

### 拒绝列表页（2150..2151）
| id | 资源 | 坐标 | 语义 |
|----|------|------|------|
| 2150 | BtAdd | (60,355) | OnAdd：CUtilDlgEx 输入（≤12 字符，SP_725）→ is_valid_character_name（否则 SP_727）→ 非自己（SP_726）→ 非重复（SP_728）→ 按名排序插入 → 计数=(n+1)/2 → sub_53792E(名) 发包 |
| 2151 | BtDelete | (180,355) | 删除（sub_919506） |

## 3. SetButton disabled 总规则（CUIUserList::SetButton @0x91e073）

1. 先把 5 组按钮**全部 SetState(0)（disabled）**；
2. 按 `*(this+369)`（当前页签）只 enable 对应组：
   - tab0 好友 → `+1484` 11 个 → (1)
   - tab1 组员 → `+1572` 9 个 → (1)
   - tab2 家族 → `+1644` 12 个 → (1)
   - tab3 联盟 → `+1788` 11 个 → (1)
   - tab4 拒绝列表 → `+2004` 2 个 → (1)
3. 每个分支尾部 `sub_4DFF00(0)`。
   ⚠️ 注意：这层只是"页签整组 enable/disable"；**行级 disabled 在 CTabFriend::OnClick 里二次过滤**（见 §4）。

## 4. 好友页行级 disabled 规则（CTabFriend::OnClick @0x91dba9）★disabled 态权威

按钮数组序号（好友组内 index）：[0]2010 BtAddFriend [1]2011 BtAddGroup [2]2012 BtGroupWhisper [3]2013 BtShowOnline [4]2014 BtMod [5]2015 BtDelete [6]2016 BtChat [7]2017 BtWhisper [8]2018 BtMessage [9]2019 BtParty [10]2020 BtBlock。

- **无选中 + 点分组行**：先 disable 全部 7 个行操作钮（offset 32..88 step 8 → index 4..10）→ 若组名 ≠ "Default Group" 则 enable {4,5}（BtMod/BtDelete）→ BtBlock 贴图按 IsGroupBlocked 切换 SP_2103/SP_2104 → 额外 enable `+1572[0]`（组 Whisper 控件）。
- **选中好友（在线）**：enable 全部 7 行钮（index 4..10）。
- **选中好友（离线）**：enable {6,7,9}（BtChat/BtWhisper/BtParty 仍可点，但双击走 OnSendMemo）。
- **选中好友（被屏蔽，item[20]/[21] 任一非零）**：只 enable {8}（BtMessage 便签）+ BtBlock→BtUnblock 贴图切换。
- 离线时不触发场上位置查询（sub_52F9C6）。

## 5. 行命中/坐标（像素校准金标准）

- **组员页** `CTabParty::GetMemberIDFromPoint @0x90e554`：`y = 33*row + 53`（行高 33px、首行 y=53），三层列表：`this+10`（在线）→ `this+12`（待定）→ `this+11`（离线）；x 宽 a2-6。
- **好友页** OnClick/OnDblClick 同构（组行折叠 `this+20` 翻转 + sub_91EF31 重算 + SetScrollBar(0)）。
- 双击语义（好友页 OnDblClick @0x91de7b）：被屏蔽 → YesNo("You can use it after unblocking...") → OnBlockFriend；离线 → OnSendMemo；在线 → OnWhisper；**组行双击 → OnModify**。
- 组员/家族双击 → CTabParty::OnChat / CTabGuild::OnChat（Messenger 邀请）。

## 6. StringPool 文案（供 locale/{zh-CN,en-US}.ini）

- SP_725 "PLEASE ENTER THE NAME OF THE CHARACTER YOU'D LIKE TO ADD TO THE BLACKLIST"
- SP_726 "YOU MAY NOT ENTER YOURSELF IN THE BLACKLIST"
- SP_727 "THIS CHARACTER NAME DOES NOT EXISTS\r\nPLEASE CHECK AGAIN"
- SP_728 "THIS NAME HAS ALREADY BEEN REGISTERED\r\nPLEASE CHECK AGAIN"
- SP_729 "YOU CAN USE IT AFTER PURCHASING THE MEMO"
- SP_4143 "BUDDY_CHAT_WITH_%s_HAS_BEEN_DISABLED" / SP_4144 "..._ENABLED"（CHATLOG type 0xC）

## 7. 对当前端的修正清单（进入 pixel-relayout/disabled-states/button-wiring 的输入）

1. Tab 枚举顺序对齐 Angel：FRIEND/PARTY/GUILD/UNION/BLACKLIST（第 4 格=联盟、第 5 格=拒绝列表；Tab disabled 用 `Tab/disabled` 整格换图）。
2. 45+ 按钮坐标全表（§2）覆盖目视初值；家族/联盟页二级动态钮（2070..2075 / 2080..2094）需要补实现。
3. disabled 三层规则：SetButton 页签整组（§3）+ OnClick 行级（§4）+ 家族/联盟未入会整组灰（ui-v83-window-restore oracle 已证）。
4. 行操作钮 disabled 贴图：img 四态 `Bt*/disabled` 自动绑定；BtBlock↔BtUnblock 是 SP_2103/SP_2104 双资源切换（不是同一资源的 disabled 态）。
5. 拒绝列表计数 (n+1)/2 的分页语义（0/26 → 26 槽、13 页）；增加钮常亮、删除钮无选中灰。
6. Memo 便签 = 现金道具 slot5（FindCashItemSlotPosition(5)），s9 无对应消费逻辑时按钮保持可点+Notice 提示（不臆造包）。
7. BtSearch=UI_Toggle(22)、BtGuildBBS=UI_Open(11)：与当前端 UIPartySearch/家族留言板入口对齐。
8. 联盟页数据加载 CTabGuildAlliance::LoadData @0x9126f4 / MakeNoticeLayer @0x9146e4 未逐行解析（留待实施家族/联盟页时按需取证；横幅/动态钮行为已覆盖）。

## 8. 会话与复现

- 服务：`idalib-mcp.exe --host 127.0.0.1 --port 13339 "E:\MXD\反编译IDA\v83\083\v83.i64"`（日志 `~\.rea\angel13339_boot.log`）。
- idb_open → session_id（如 e344ff00）→ 所有工具调用带 `database:<session_id>`。
- 工具参数：decompile `{addr, database}`；list_funcs `{queries:{filter,count}, database}`；lookup_funcs `{queries:[...], database}`。
- 脚本：`~\.rea\angel_start.ps1`（启动+探测）、`angel_call.ps1`（单工具调用）、`angel_decompile_batch.ps1` / `angel_decompile_tabs.ps1` / `angel_list_tabs.ps1`（批量）。
