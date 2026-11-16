# evidence · r1 IDA 反编译产物索引（tools/_ida_userlist_r1/）

来源：Angel 083 IDB（v83.i64 @ idalib-mcp 13339, session e344ff00），2026-11-16 批量导出。
round68 既有产物在 `tools/_ida_pb/`（CUIUserList_OnCreate/Draw/SetButton/SetScrollBar + CUIPartyHP + CConfig_GetShowPartyHP + symbols/decompile_summary）。

| 文件 | 地址 | 说明 |
|------|------|------|
| CUIUserList_OnMouseButton.dec.txt | 0x91d515 | 左键/双击按页签分派到各 CTab 子类 |
| CUIUserList_OnMouseMove.dec.txt | 0x91d82f | 悬停（tooltip/行高亮） |
| CUIUserList_ResetInfo.dec.txt | 0x91dfaf | 页签切换后数据刷新 |
| CUIUserList_CreateAllianceButton.dec.txt | 0x91eba3 | 联盟页动态按钮创建（SectionData） |
| CUIUserList_DestroyAllianceButton.dec.txt | 0x91ee49 | 联盟页动态按钮销毁 |
| CUIUserList_DestroyGuildButton.dec.txt | 0x91eb52 | 家族页动态按钮销毁 |
| CTabFriend_OnButtonClicked.dec.txt | 0x91cf4f | ★全部 45+ 按钮 id→行为中心分发表 |
| CTabFriend_OnModify.dec.txt | 0x90a1f6 | 修改分组/重命名组（默认组不可改删） |
| CTabFriend_OnWhisper.dec.txt | 0x90a630 | SetWhisperTarget+StartChat |
| CTabFriend_OnPartyInvite.dec.txt | 0x90a995 | SendJoinPartyMsg(选中名) |
| CTabFriend_OnSendMemo.dec.txt | 0x90aa2c | 便签=现金道具 slot5；无则 Notice SP_729 |
| CTabFriend_OnBlockFriend.dec.txt | 0x90aba4 | 翻转 block + ChangeBlockOption + 回显 SP_4143/4144 |
| CTabFriend_ChangeBlockOption.dec.txt | 0x90b148 | block 选项持久化 |
| CTabFriend_OnClick.dec.txt | 0x91dba9 | ★行级 disabled 规则（离线{6,7,9}/屏蔽{8}/组行{4,5}） |
| CTabFriend_OnDblClick.dec.txt | 0x91de7b | 双击：屏蔽→解锁确认/离线→Memo/在线→Whisper/组行→Modify |
| CTabFriend_IsGroupBlocked.dec.txt | 0x91db1b | 组是否全屏蔽 |
| CTabParty_OnInvite.dec.txt | 0x90e13d | CUtilDlgEx 输名 → SendJoinPartyMsg |
| CTabParty_OnKick.dec.txt | 0x90e236 | SendKickPartyMsg |
| CTabParty_OnWhisper.dec.txt | 0x90e2da | SetWhisperTarget+StartChat |
| CTabParty_OnChat.dec.txt | 0x90e37a | Messenger::TryNew+SendInviteMsg |
| CTabParty_OnChangeBoss.dec.txt | 0x90e41a | YesNo 确认 → SendChangePartyBossMsg |
| CTabParty_GetMemberIDFromPoint.dec.txt | 0x90e554 | ★行命中 y=33*row+53、三层列表 |
| CTabGuild_OnKick.dec.txt | 0x9112c0 | SendKickGuildMsg |
| CTabGuild_OnChat.dec.txt | 0x911454 | Messenger::SendInviteMsg |
| CTabGuild_OnPartyInvite.dec.txt | 0x911687 | SendJoinPartyMsg |
| CTabGuildAlliance_LoadData.dec.txt | 0x9126f4 | 联盟页数据加载 |
| CTabGuildAlliance_OnPartyInvite.dec.txt | 0x9153dc | GetGuildItem→SendJoinPartyMsg |
| CTabBlackList_OnAdd.dec.txt | 0x919214 | ★拒绝列表添加全流程（SP_725/726/727/728） |
| CTabBlackList_MemberIDFromPoint.dec.txt | 0x919619 | 拒绝列表行命中 |
| _funcs_CUIUserList.json | — | CUIUserList 全部 10 符号清单 |
| _funcs_CTabFriend.json / _funcs_CTabParty.json / _funcs_CTabGuild.json / _funcs_CTabBlackList.json | — | 四类符号全景（CTabGuildAlliance 无独立符号，经 OnButtonClicked 分发） |
| _funcs_onbuttonclicked.json | — | OnButtonClicked 名字查询结果（仅 CTabFriend 命名） |

结论汇总：`notes/r1-angel-ida-forensics.md`。
