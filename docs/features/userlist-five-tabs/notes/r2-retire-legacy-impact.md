# r2 · 旧浮窗（UIBuddyList/UIParty）退役影响面清单

> 2026-11-16 code-explorer 全量扫描。目标：删除 `IO/UITypes/UIBuddyList.{h,cpp}`、`IO/UITypes/UIParty.{h,cpp}`，handler 同步统一走 UIUserList。

## A. 必删文件与工程条目
- `IO/UITypes/UIBuddyList.h/.cpp`、`IO/UITypes/UIParty.h/.cpp`。
- MapleStory.vcxproj ClCompile :388(UIParty.cpp)/:389(UIBuddyList.cpp)；**无对应 ClInclude/filters 条目**，删 cpp 即可。
- `IO/UIElement.h:211-212` 的 `Type::PARTY`/`Type::BUDDYLIST` 删类后无使用者（已核实全工程无其它引用），一并删除。

## B. 必改引用点
**运行时可达（需改走 UIUserList）**
1. `WaveAShellHandlers.cpp:1341-1347` OpenUIHandler case 0x0C 强开 UIParty → 改 emplace/makeactive UIUserList+切组员页（加 `UiAssetResolve::usable` 守卫，对照 UIStateGame.cpp:453-454）。
2. `WaveAShellHandlers.cpp:1348-1354` case 0x0D 强开 UIBuddyList → 同上，切好友页。
3. `WaveAShellHandlers.cpp:4063-4078` PartyValueHandler（调试）→ emplace UIParty 改 UIUserList（或直接降级为仅 ChatBar 显示）。
4. `MessagingHandlers.cpp:866-870` Whisper Find 结果 makeactive UIBuddyList → 改 UIUserList 好友页。
5. `UIChat.cpp:19/:130/:227`、`UIChatBar.cpp:20/:1547-1548` 悄悄话取 `get_element<UIBuddyList>()->get_selected_cid()` → 改 UIUserList 好友选中 cid；**须先给 UIUserList 补只读 getter（现 friend_selected_cid 私有）**。
6. `BuddyHandlers.h:22`、`UIMiscShells.cpp:46-47` 死 include，直接删。

## C. 双写/重复逻辑（handler 侧改为唯一写入口）
- BuddyHandlers.cpp：:73-75(模型+浮窗 update_buddies)、:119-121(channel)、:130-132(capacity)、:159(is_active 抑制 tip)。删浮窗分支后：模型写完 → `get_element<UIUserList>()` 若活则 `refresh_from_models()`；失败 tip 改恒弹（ChatBar）。
- PartyHandlers.cpp：:53-57/:124/:153 sync_party_ui、:137-140(DISBAND clear+deactivate)、:166-168(set_leader)、:197(is_active)。同上改法。
- 旧窗自身回写模型（UIParty.cpp:44/:159-160/:170-171；UIBuddyList.cpp:40/:137-138/:148-149/:158-159）随删除消失——handler 先写模型，回写冗余，安全。
- 模型：`Character/BuddyListModel.*`、`Character/PartyModel.*` 保留（UIUserList/ChatBar/handler 依赖）。

## D. 血条（PartyHP）迁移路径 ★
- 现状：**UIParty 即血条载体**（无独立 CUIPartyHP 窗）。数据：PartyHandlers.cpp:257-268 PartyMemberHPHandler(UPDATE_PARTYMEMBER_HP=201)→`uiparty->update_member_hp(cid,cur,max)`→UIParty.h:76 memberhp map；绘制 UIParty.cpp:110-140（`UserList/Party/PartyHP/GaugeBar` 的 partyhp_graduation+partyhp_bar 63x7 裁右，资源加载 :56-58）。
- UIUserList 组员页**不绘 HP**；BT_PARTY_HP 点击仅 guild_hint（UIUserList.cpp:1524/:1231-1233）。
- 迁移：① `PartyModel` 新增 `set_member_hp/get_member_hp`（常驻存储，修复"先收血包后开窗丢失"——现 handler 只在 UIParty 活时存，:266-267）；② handler 改写模型+get_element\<UIUserList\> 刷新；③ UIUserList 组员页 draw 照搬 UIParty.cpp:110-140 画法（GaugeBar 裁右填充）；④ BtHP(2207) 作为 UIUserList 内显示开关；⑤ SP_4433 SHOWPARTYHP 配置开关仍缺（backlog 已登记）。

## E. 测试守卫影响
- `tools/test_round68_userlist_083.py` **整体失效**（:28-30 read UIBuddyList.cpp/UIParty.cpp/UIParty.h；A1-E3 断言锚定旧窗源码 :44-117/:137）→ 必须重写为对 UIUserList.cpp 的断言（r1 取证坐标表+disabled 规则）或下线合并进新守卫。
- tools/_uw_*.py、_pb_decompile.py、_ida_*.py 为历史一次性脚本，勿再执行。

## F. 风险与遗留
1. locale 死键：`buddylist.title`/`party.title`（zh/en ini:224/:227）可留可删。
2. OpenUIHandler 0x0C/0x0D 映射页签需定：0x0C→组员页、0x0D→好友页。
3. UIPartySearch 不依赖旧窗，保留不动（被 UIUserList:777/:783/:1218、UIStateGame:454、UIMiscShells:6044/:6061/:6371 打开）。
4. UIStateGame/UIStatusBar/UIGameMenu 全量扫描零旧窗引用，无遗漏 emplace。
5. 血条数据丢失窗口见 D①（PartyModel 常驻存储是修复点）。
