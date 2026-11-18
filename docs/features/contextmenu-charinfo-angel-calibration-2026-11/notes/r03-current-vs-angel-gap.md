# r03 — 当前实现 vs Angel 差异清单与修复映射

综合 r01（Angel 反编译）+ r02（IMG 核验）+ code-explorer 审计。

## 差异总表

| # | 模块 | 差异 | 严重度 | 修复动作 |
|---|---|---|---|---|
| 1 | 右键命中 | `UIStateGame::rightclick` 未命中回退自己并弹菜单（空地/怪/NPC 全弹） | **P0** | 命中失败即 return；仅命中玩家身体框才弹 |
| 2 | 右键命中 | 自己命中框在 UIStateGame 重复实现（与 MapChars 同矩形） | P2 | 保留（Angel 自身也可右击自己）；可抽公共函数但非必须 |
| 3 | ContextMenu BtSave | 当前=BuddyAddPacket；Angel=复制名字到剪贴板 + StartChat(0) | **P1** | 改：Window::setclipboard(name) + UIChatBar StartChat |
| 4 | ContextMenu BtTrade | 当前=dispatch_trade_invite(cid)；Angel=SendInviteTradingRoomMsg(name) | P2 | 语义等价（s9 要求 CREATE+INVITE），保留 |
| 5 | ContextMenu BtWhisper | 当前=focus_whisper(一次性SoftKey发包)；Angel=SetWhisperTarget 留状态 | **P1** | 改：set_whisper_target(name) |
| 6 | ContextMenu BtParty | 当前=PartyInvitePacket(name)；Angel=SendJoinPartyMsg | P2 | 语义等价，保留 |
| 7 | ContextMenu 按钮集 | 运行时 IMG 12 按钮，代码 8 个；缺 BtAlertGM/BtBlock/BtCashTrade | **P1** | 补 3 按钮接线（GM警告/屏蔽/现金交易），按 Angel 语义与 s9 支持度决定实际行为 |
| 8 | ContextMenu 条件 | 当前 other=(cid!=self) 决定 Party/Guild/Family；Angel 按玩家自身状态标志 +52/+53/+54 | P1 | 按 Angel 重做条件：Party=未组队/同频道、Guild=有公会且同公会/权限、Family=家族可收徒；BtClaim 恒有 |
| 9 | UICharInfo 布局 | 用 Journey 层级子容器(farm/personality/collect/damage/item)，运行时为扁平结构 → 子面板全失效、大量真实按钮未接线 | **P0** | 按扁平 UserInfo 重写构造：接 backgrnd..7 + 全部真实 Bt*；实现 show_bottom/right_window 对齐 Angel SetState(1/2/3) |
| 10 | UICharInfo 自己禁用 | 禁用 BtParty/BtPopDown/BtPopUp/BtFriend（其中 3 个按钮 IMG 不存在）；BtTrad/BtFamily/BtVisit 仍可点 | P1 | 按 Angel/扁平按钮重做禁用逻辑 |

## 菜单按钮映射（Angel id → 行为，供重写参考）
- 2000 BtInfo → CharInfoRequestPacket(cid)
- 2001 BtSave → 复制名字到剪贴板 + StartChat；BtBlock → 屏蔽（需确认 s9 opcode）
- 2002 BtTrade → dispatch_trade_invite；BtCashTrade → 现金交易（s9 无支持则 disabled）
- 2004 BtWhisper → set_whisper_target
- 2005 BtParty → PartyInvitePacket；2006 BtGuild → GuildInvitePacket
- 2007 BtClaim → UIReport 举报；2008 BtFamily → FamilyAddPacket
- BtAlertGM → GM 警告（需确认 s9 opcode，无则 disabled）

## 个人情报按钮映射（Angel CUIUserInfo OnButtonClicked）
- 1000 关闭；2000/2001 家族(show_bottom)；2002 组队；2003 交易；
- 2004/2005 人气 +/-；2007/2009/2011/2012 关子面板；2008/2010/2013-2015 SetState；2016 私聊/好友。
- 运行时 IMG 扁平按钮：BtParty/BtTrade/BtItem/BtWish/BtPresent/BtPetHide/Show/BtTamingHide/Show/BtBookHide/Show/BtExceptionShow/Hide/BtMeso/BtRegister/BtDelete/BtSearch/BtFamily/BtCollectionHide/Show/Marry/bossPetCrown/BtPet1-3。

## 实机验证结论（2026-11-16，s9 在线，1280×720）
- 差异1（右键命中）：**已端到端验证通过**。调试日志证实空地(900,100)、怪物(780,410)、点偏位(535,350) 均 `MISS -> return` 不弹；正确角色身体位(640,330) `HIT cid=1 name=Admin -> open menu` 弹菜单。`evidence/live_dbg3_self_hit.png` 可见菜单。
  - 坐标核对：`get_position()+camera.position()` 与绘制用 `phobj.get_absolute(camera.realposition(),alpha)` 实测一致（均 =(640,361)），命中框算法正确。调试期曾误判角色在 x≈535（因 read_file 显示图按 1080 宽缩放，×(1280/1080) 后实为 640）——勿再踩此坑。
- 差异3/5/8（菜单行为/条件）：自己右击弹 6 项（查看情報/保存名字/申请交易/申请好友/悄悄话/举报），无 Party/Guild/Family，符合"邀请类仅他人显示"工程保护。点"查看情報"成功发 `CharInfoRequestPacket` 并打开个人情报窗（`live_dbg5_info_open.png`）。
- 差异9/10（个人情报窗）：主窗字段（等级/职业/人气度/家族/联盟）与背景对齐正确（`live_charinfo_crop.png` 放大确认：350/黑骑士/0/-/-）。中部 3 个灰色空框、底部空白子面板为 Journey 层级残留 sprite，缺正确端截图对照，列为未决预留（未臆改）。
- 调试日志 `ctxmenu_dbg.log` 已在收尾时从源码全部删除，最终 build EXITCODE=0。

## 待决项（修复时按证据/最小风险默认处理）
- BtAlertGM/BtBlock/BtCashTrade：s9 若不支持对应 opcode，按钮加载但点击 disabled/提示，不强造协议（门禁5/7）。
- 自己 vs 他人的菜单条件：按 Angel 三个标志实现，但它依赖全局玩家状态（是否组队/公会/家族），当前端能否取到这些状态需要在实现时核对；取不到则退化为按 cid==self 区分 + 保留常量结构。
