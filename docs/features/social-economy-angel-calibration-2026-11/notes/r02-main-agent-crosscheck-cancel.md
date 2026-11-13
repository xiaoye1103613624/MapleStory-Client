# 社交经济面 · 主 agent 跨端销案复核（r02）

> 本文件是**主 agent 直接读当前端源码**逐字对照 Angel 083 一手语义（r01）的销案/
> 推翻/新增判定，不是三路 subagent 结果（那些见 r03–r05）。
> 性质：[观察]=源码逐字；[推断]=据逐字对照得出；[s9 适配]=编号/形态差异，
> 须交用户裁决，不得盲改。参照基准固定 083/v83；**禁止引 095 线作依据**。

---

## 0. 一句话结论（6 条）

1. **★ G 键「家族」= 公会落 UIUserList 第 3 页签：当前端已正确实现，Angel 083 逐字互证**
   → 大面积销案 `social-loop-audit-2026-11` 用户 2026-11-07 裁决后的落地状态。
2. **★ 5 页签顺序 好友/组员/家族/家族联盟/拒绝列表：当前端与 Angel 逐点吻合**，
   仅「tab 枚举编号口径」不同（当前端 `UNION` 排末尾用 `tab_button()` 显式映射，
   原生是连续 tab index 0..4）→ 形态差异，非缺陷。
3. **★ 学院(FAMILY) ≠ 家族(GUILD) 双类树：当前端分离正确**
   （`UIFamily.cpp:2` 学院 / `UIGuild.h:31-37` 家族页切片+浮窗），
   与 Angel `CUIFamily*` vs `CTabGuild*` 类树分离互证。
4. **MESSENGER 出站协议 mode 0/2/3/5/6 + c2s 122 与 Angel 逐字对齐** ✅。
5. **⚠️ 新发现 095 文档污染面**：`IO/UITypes/UIMessenger.cpp` 有 **10 处 `v95` 注释**
   作为行为依据（打字指示 / blink / chatBalloon / SetCtrl），按 MEMORY.md critical
   必须标注「依据污染，须换引 Angel `CUIMessenger`」——**行为本身可能正确，
   但证据链不合规**（与核心窗口面 `UIStatsInfo.cpp:146` 同型，范围更大）。
6. **真缺口收敛**：`change_target()` 空 stub 死链（HUD 面同款，跨面共性）、
   `UIGuild` 独立浮窗（原生无，s9 适配保留）、部分 Wave A 壳窗深度、
   旧文档 4 类状态未同步。`CUIMessenger` / `CUIUserList` 主体行为层当前端已接。

---

## 1. 销案 1：G 键「家族」= 公会 = UIUserList 第 3 页签 ✅

### Angel 083（r01 §3）
`CUIUserList::Draw @ 0x91bec3` 5 页签分支 `v8=*(this+369)`（tab index 0..4）；
`OnCreate` 公会页控件组 **2030–2041**（BtInvite/BtKick/BtWithdraw/BtUp/BtDown/
BtWhere/BtWhisper/BtNotice/BtChat/BtInfo/BtPartyInvite/BtGuildBBS）即 tab index 2。

### 当前端逐字 [观察]
`IO/UIStateGame.cpp:356-384`：
```
case KeyAction::Id::FRIENDS:
case KeyAction::Id::PARTY:
case KeyAction::Id::BOSSPARTY:
case KeyAction::Id::GUILD:
{
    // v83 G(17) = GUILD「家族」：用户 2026-11-07 指正，原生是
    // 「好友/组员/家族/家族联盟/拒绝列表」共用窗的第 3 页签，
    // 不是独立 UIGuild 浮窗（浮窗仍由底栏社区钮保留入口）。
    UIUserList::Tab tab;
    switch (action) {
        case KeyAction::Id::FRIENDS: tab = UIUserList::Tab::FRIEND; break;
        case KeyAction::Id::PARTY:  tab = UIUserList::Tab::PARTY;  break;
        case KeyAction::Id::BOSSPARTY:
        case KeyAction::Id::GUILD:  tab = UIUserList::Tab::GUILD;  break;
    }
    auto userlist = UI::get().get_element<UIUserList>();
    if (userlist && userlist->get_tab() != tab && userlist->is_active())
        userlist->change_tab(tab);
```
`IO/UITypes/UIUserList.cpp:268` 注释「Guild Tab（家族页 = 共用窗第 3 页签，2026-11-07 续作）」；
`:348 else if (tab == Buttons::BT_TAB_GUILD)`、`:447 draw_guild_tab`、`:623 case BT_TAB_GUILD → change_tab`。

→ **判定 [观察+推断]：✅ 已校准**。G/BOSSPARTY → `UIUserList::Tab::GUILD`（第 3 页签），
与 Angel 083 公会控件组 2030–2041 语义逐点吻合；MEMORY.md critical 术语在源码层已落地。
`docs/features/social-loop-audit-2026-11/README.md` 的「一律参照 GMS083 原版」裁决闭环。

---

## 2. 销案 2：5 页签顺序与底板选择 ✅（编号口径差异单列）

### 当前端逐字 [观察]
`IO/UITypes/UIUserList.cpp:88-110`：
```
// 页签条落位：oracle 锚 = Tab/enabled/0 在原生帧上的 (4,23)，五格相对锚
// x = 0 / 31 / 62 / 92 / 145，y = 0 / -1 / -1 / 0 / -1。原生五格依次是
// 好友 组员 家族 家族联盟 拒绝列表 —— 原先只接了前四格，「拒绝列表」错拿了
// 3 号（家族联盟）的美术，第三格（家族）被当成 BossParty 页用。
static const Point<int16_t> TAB_POS[5] = {(4,23),(35,22),(66,22),(96,23),(149,22)};
make_tab(BT_TAB_FRIEND, 0); make_tab(BT_TAB_PARTY, 1);
make_tab(BT_TAB_GUILD, 2); make_tab(BT_TAB_UNION, 3);
make_tab(BT_TAB_BLACKLIST, 4);
```
`IO/UITypes/UIUserList.h:45-58` `enum Tab { FRIEND, PARTY, GUILD, BLACKLIST,
PARTY_MINE, PARTY_SEARCH, FRIEND_ALL, FRIEND_ONLINE, /* 顶层第 4 格家族联盟
排在末尾避开上面四个子页签值；顶层页签 → 按钮 id 一律走 tab_button() 显式映射 */ UNION };`
`IO/UITypes/UIUserList.cpp:1131` 同口径注释；`:1441 tab_button(uint8_t page)` 显式映射；
`:1121-1129 change_tab` 三张互斥底（好友=backgrnd3 / 家族·联盟=backgrnd2 /
组员·拒绝=backgrnd，2026-11-08 五张 oracle 帧 meanSAD 量出，禁叠底）；
`:529`「五格顺序 = 原生页签条：好友 组员 家族 家族联盟 拒绝列表」+ `order[(i+1)%5]`。

### 对照 Angel
`CUIUserList::Draw` 5 层 `if` 分支（tab 0..4 连续）；`SetButton` 5 组全隐藏再按
`*(this+369)` 显一组；`OnCreate` 页签条 ctrl **2001** `(a4:1,a5:6,a6:23,a7:270)`。

→ **判定 [观察+推断]：✅ 5 页签顺序/显隐逻辑/逐格坐标已校准**；
⚠️ **形态差异（非缺陷）**：当前端 `Tab` 枚举把 UNION 排末尾、用 `tab_button()`
显式映射绕开「顶层页签与子页签共用枚举」的历史包袱，原生是连续 0..4 +
`SetButton` 全隐藏/单组显示。两套编号口径不同但语义等价 → 列「一致性提示」，
不改源码。当前端 `TAB_POS` 是**正确端截图 oracle 实测**（比 Angel IDA 坐标更贴实机），
按三面一致红线「校准 ≠ 改回原生」保留。

---

## 3. 销案 3：学院(FAMILY) ≠ 家族(GUILD) 双类树 ✅

| 类 | 当前端逐字 | Angel 083 |
|---|---|---|
| 学院 Family | `IO/UITypes/UIFamily.cpp:2`「学院（Family）UI + OPEN_FAMILY 出站 —— 2026-11-07 原生『学院.png』重排」；`UIFamilyTree.cpp`；`Character/FamilyModel.h`；`Net/Packets/FamilyPackets.h` | `CUIFamily 3` / `CUIFamilyChart 8` / `UFamilyItem 1` / `CUIFriendGroup 2` |
| 家族 Guild | `IO/UITypes/UIGuild.h:31-37`「GuildMember / GuildData 已上移至 Character/GuildModel.h（家族页切片，2026-11-07）：共用窗 UIUserList 第 3 页签与本浮窗共读同一模型」`TYPE=GUILD`；`UIGuildBBS.cpp` | `CTabGuild 7` / `CTabGuildAlliance 7` / `CWndGuildGrade 3` / `CWndAllianceGrade 3` / `CUIGuildBBS 17` |

→ **判定 [观察+推断]：✅ 分离正确**，与 Angel 类树分离互证。
⚠️ 但当前端**多一个 `UIGuild` 独立浮窗**（`UIStateGame.cpp:363` 注释自证「浮窗仍由
底栏社区钮保留入口」）——083 原生公会入口只有 UIUserList 第 3 页签 → 列
**[s9 适配]** 决策点：保留双入口（当前默认）还是收敛为单页签，交用户裁决。

---

## 4. 销案 4：MESSENGER 出站协议 ✅ + ⚠️ 095 注释污染（本面最重要新增）

### 4.1 协议对齐 [观察]
`Net/Packets/MessengerPackets.h:2`「Messenger 出站包：Opcode MESSENGER(122)，
对照 s9 MessengerHandler mode 序」：
- `:13 MessengerOpenPacket(int32_t messengerid)` → `write_byte(0x00); write_int(messengerid);`
  （mode 0 建房/接受邀请）
- `:24 MessengerLeavePacket` → `write_byte(0x02)`
- `:34 MessengerInvitePacket(name)` → `write_byte(0x03); write_string(name);`
- `:45 MessengerDeclinePacket(sender)` → `write_byte(0x05); write_string(sender);`
- `:56 MessengerChatPacket(text)` → `write_byte(0x06); write_string(text);`

`IO/UITypes/UIMessenger.cpp` 调用点逐字：`:185 MessengerOpenPacket(0).dispatch()` /
`:215 MessengerLeavePacket().dispatch()` / `:279 MessengerChatPacket(self+"1"/"0")` /
`:409 MessengerChatPacket(payload)` / `:435/:447 MessengerInvitePacket` /
`:623/:630 MessengerDeclinePacket` / `:667 button_pressed`。

Angel `CUIMessenger::OnCreate` 逐字 `COutPacket(122) + Encode1(0) + Encode4(a2) + SendPacket`
（mode 0 + messengerid），`OnButtonClicked` 5 case（1000 TryDelete / 1001+1002
SetState / 1003 ProcessChat / 1004 SendClaim），`OnPacket` 9 分支。

→ **判定 [观察]：✅ c2s 122 + mode 0/2/3/5/6 与 Angel 及 `r22-messenger-protocol.md`
逐字对齐**；`PacketSwitch.cpp:161` s2c `MESSENGER=313` 是收包方向，不冲突。
`Net/PacketSwitch.cpp:453 emplace<MESSENGER, MessengerHandler>()` 已注册。

### 4.2 ⚠️ 095 文档污染（MEMORY.md critical 违规面，须换依据不改行为）
`grep v95|V95|Maplestory95|095 IO/UITypes/UIMessenger.cpp` **10 命中**：
```
:136 // v95 SetCtrl：StringPool → UI/Basic.img/BtClose3 @ (276,6)（展开态）。
:159 // blink：标题条注意标（v95 m_pLayerBlink；本包单帧 12×10 橙泡）
:168 // chatBalloon/0..4：座位上方打字气泡（delay=120）；v95 LoadLayer(…/chatBalloon)
:237 // 对照 v95 StartBlink(1)：仅在未闪时置 30；本端收真聊天 / 进离房都会触发。
:278 // v95 Update：EncodeStr(charName + "0"|"1")，无冒号 ⇒ OnChat 走打字分支。
:290 // v95 OnChat：有 " : " → 真聊天；否则末字符 0/1 为打字指示。
:410 // 发真聊天后清打字态（对照 v95 ProcessChat 置 m_bInputState=0）。
:486 // chatBalloon：打字中循环播；相对脚点 BALLOON_DX/DY（v95 RelMove 缩放近似）
:537 // blink 计数：对齐 v95 Update（每 tick -1；到 0 重置为 30 继续闪，直到停）
:549 // 打字指示出站（对照 v95 Update m_bInputState ↔ 输入框空/非空）
```
→ **判定 [观察+推断]：⚠️ 依据污染**。这些行为的 083 权威源应是 Angel
`CUIMessenger`（26 方法）、`CChatBalloon`（`SP_1388..1425_CHATBALLOONIMG_*`
九切片 / `SP_1858_MESSENGER_BLINK` / `SP_1860_CHATBALLOON`）、`CUIMessenger::OnPacket`
case 6 `OnChat`。与核心窗口面 `UIStatsInfo.cpp:146`（`Maplestory95 CUIStat::OnCreate(0x867b90)`）
同型但**范围更大（10 处 vs 1 处）**。
**处置（零风险，列 D 决策点）**：只把注释依据换引 Angel 083 符号，
**不改任何行为代码**（当前实现已用正确端资源+协议闭环，行为面无反证）。
严禁据此删功能/改协议。

---

## 5. 销案 5：社交协议 handler 面完整 ✅

`Net/PacketSwitch.cpp` 逐字（社交经济相关已注册，约 40 条）：
```
:158 PARTY_OPERATION=62  :159 BUDDYLIST=63   :160 GUILD_OPERATION=65
:161 MESSENGER=313       :162 UPDATE_PARTYMEMBER_HP=201
:164 STORAGE=309         :170 MEMO_RESULT=41
:171 FAMILY_INFO_RESULT=95 :172 FAMILY_CHART_RESULT=94 :173 FAMILY_RESULT=96
:174 FAMILY_JOIN_REQUEST=97 :175 FAMILY_JOIN_REQUEST_RESULT=98
:182 WEDDING_GIFT_RESULT=74 :183 WEDDING_PROGRESS=320 :184 WEDDING_CEREMONY_END=321
:187 MONSTER_CARNIVAL_PARTY_CP=291 :271 FAMILY_JOIN_ACCEPTED=99
:272 FAMILY_NOTIFY_LOGIN_OR_LOGOUT=102 :273 FAMILY_SUMMON_REQUEST=104
:302 FAMILY_PRIVILEGE_LIST=100 :303 FAMILY_REP_GAIN=101 :304 FAMILY_SET_PRIVILEGE=103
:336 WEDDING_PHOTO=43 :340 PARTY_VALUE=91 :346 GUILD_BBS_PACKET=59
:360 GUILD_NAME_CHANGED=202 :361 GUILD_MARK_CHANGED=203
:395-396 ALLIANCE_OPERATION=66（对接 s9 66 send / 143 recv）
:406-408 GUILD_BOSS_HEALER_MOVE=287 / PULLEY_STATE_CHANGE=288
```
`emplace` 注册点 `:442-478`（STORAGE/PARTY/BUDDYLIST/MESSENGER/FAMILY_*/MEMO/
WEDDING_*/MONSTER_CARNIVAL_PARTY_CP）、`:550-554`（ALLIANCE/GUILD）、
`:619-621`（FAMILY_JOIN_ACCEPTED/NOTIFY/SUMMON）、`:650-652`（PRIVILEGE/REP/SET）、
`:662-663`（GUILD_BOSS_*）、`:686/:690/:696/:710-711`（WEDDING_PHOTO/PARTY_VALUE/
GUILD_BBS/GUILD_NAME|MARK_CHANGED）。

→ **判定 [观察]：✅ 收包 handler 面完整**，与 Q2 §9.5「148/148 出站全有发送点」
结论一致；社交经济面不缺 handler，缺的是少数**玩法深度/真弹窗/文档一致性**。

---

## 6. 当前端源码文件存在性（对照 Angel 56 类）[观察]

`glob IO/UITypes/UI{Family,FamilyTree,Guild,GuildBBS,Alliance,BuddyList,Party,
Messenger,Trade,PersonalShop,Storage,Memo,Delivery,StoreBank,Wedding,MapleTV,
UserList,WhisperTargetDlg,FriendGroup}*` 命中 **37 个 .h/.cpp**：

```
UIMemo.h/.cpp          UIParty.h/.cpp        UIBuddyList.h/.cpp
UIFamilyTree.h/.cpp    UIStorage.h/.cpp      UIMapleTV.h/.cpp
UIPartySearch.h/.cpp   UIDelivery.h/.cpp    UIPartyRace.h/.cpp
UIMessenger.h/.cpp     UIStoreBank.h/.cpp   UITrade.h/.cpp
UIWedding.h/.cpp       UIAlliance.h/.cpp    UIFamily.h/.cpp
UIGuild.h/.cpp         UIUserList.h/.cpp    UIGuildBBS.h/.cpp
UIGuildBoss.h/.cpp     UIMemo...
```
未找到独立文件（与 Angel 对照预期）：`UIPersonalShop`（在 `UIMiscShells.h:442`）、
`UITrunkDlg`（UIStorage）、`CTradingRoomDlg`（UITrade）、`CMiniRoomBaseDlg` 系列
（`UIMiscShells` 壳）、`CParcelDlg`、`CUIQuestDelivery`（UIDelivery）、
`CTab*` 全部（合入 UIUserList 内部方法 `draw_friend_tab/draw_party_tab/
draw_guild_tab/draw_union_tab/draw_blacklist_tab`）。

→ **判定 [推断]：拆分模式与前三面一致** —— Angel「单体窗 + 一批 Tab/Room 子类」
→ 当前端「`UIUserList` 单类吞 5 个 Tab 的 draw + 独立 `UIFamily/UIGuild/UIGuildBBS/
UIAlliance/UIBuddyList/UIParty/UIMessenger/UITrade/UIStorage/...` 多窗 +
`UIMiscShells` 20+ 壳」。**不是 56 类一一对应**，属结构性重写，列 [s9 适配/形态差异]。

---

## 7. 真缺口初判（待 r03–r05 与六维表覆写）

| # | 缺口 | 证据 | 性质 |
|---|---|---|---|
| 1 | `change_target()` 空 stub 死链 | `IO/UITypes/UIUserList.cpp:916`「v83 家族聊天由聊天框目标/斜杠别名路由；change_target 仍是预留空实现」；HUD 面 `UIChatBar.h:123` 同款 6 频道快捷键死链 | 跨面共性，❌ 真缺 |
| 2 | `UIGuild` 独立浮窗（原生无） | `UIGuild.h:37 TYPE=GUILD` + `UIStateGame.cpp:363` 自证 | [s9 适配] 保留 |
| 3 | Messenger 10 处 v95 注释依据 | §4.2 | ⚠️ 文档污染，零风险可换 |
| 4 | Wave A 壳窗深度（Deliver/StoreBank/FamilyTree/Memo/MTS 等） | 待 r04/r05 复核；Q2 清单已大面积销案 | 待核 |
| 5 | 旧文档四态未同步 | `feature-gap-inventory-2026-11` §6/§14/§16 部分仍标 ❌ | 文档一致性 |

---

## 8. 与前三面共性（第五次确认）

| 面 | EA 交叉 | 拆分 | 结论 |
|---|---|---|---|
| 登录 | 327/328 | `CLogin` → 状态机 | 不缺窗口/不缺排版 |
| HUD | 547/548 | `CUIStatusBar` 41 → 10+2 | 同上 |
| 核心窗口 | 439/441 | 62 类 → 12 窗+20+壳+5 组件 | 同上 |
| **社交经济** | **752/752** | `CUIUserList` 5 页签/`CUIMessenger` 26 → 多窗 | 同上 |

**「校准 ≠ 改回原生」四面一致**；s9 适配红线（改回原生会断联机）继续生效；
IMG 层四面阻塞；`change_target` / 095 注释污染是**跨面重复出现的两类根因**。

---

## 9. 冲突取值表（按门禁「checked-in source > docs」）

| 冲突 | 旧文档 | 当前源码/IDA | 取值 |
|---|---|---|---|
| G 键公会入口 | 旧账「独立 UIGuild」 | `UIStateGame.cpp:361-384` 第 3 页签 | 源码（已校准） |
| 第 3 格美术 | `open-issues` 曾误当 BossParty | `UIUserList.cpp:90-91` 明确纠正 | 源码 |
| 5 页签底板 | 早期叠两张全不透明底 | `:1121-1129` 三张互斥底单画 | 源码 |
| Messenger 行为依据 | 10 处 v95 注释 | Angel `CUIMessenger`（083） | Angel 083（只换注释依据） |
| Tab 枚举编号 | 原生连续 0..4 | 当前端 UNION 排末尾 + `tab_button()` | 保留当前端（形态差异） |

---

*性质：本文件为主 agent 只读取证归档；未修改任何产品源码、未构建、未实机、未提交。
[观察]/[推断]/[s9 适配] 已逐条标注。*
