# 右键上下文菜单 / 个人情报窗 — Angel v83 对照校准与像素级修复（2026-11-16）

## 需求（原话）
> 进入游戏后，可以在任意位置右击弹出菜单（交易什么的）？不是应该只能右击玩家角色么？而且右击出现的功能列表也是要逻辑的，自己和其他玩家显示也不同吧？
> 对照 angel 查看一下这一块的逻辑，而且列表中显示的角色点击打开的个人情报 ui、功能也是错误的；
> 以右击功能扩展的所有功能全部检查并且像素级修复

## 结论（已实现 + 实机验证）
1. **右键弹出条件**（P0，已修 + 实机验证）：`IO/UIStateGame.cpp::rightclick` 原逻辑在未命中角色时回退为自己并**无条件**弹菜单 → 空地/怪物/NPC 全弹。改为严格按 Angel `CUserLocal::HandleRButtonClk`：**仅命中玩家角色身体矩形（脚底锚 72×110）才弹**；其他玩家走 `MapChars::find_char_at`，自己也按同框判定，空地/怪物/NPC 直接 return。
2. **菜单功能列表逻辑**（P1，已修）：`UIMiscShells.cpp::rebuild_buttons` 按 Angel `CUIContextMenu::OnCreate` 重做——固定 5 项（Info/Save/Trade/Whisper/Claim），可选 Party(未组队)/Guild(有公会)/Family(有家族)由玩家社交状态决定，并附加"仅他人显示邀请类"工程保护；坐标 x=9/y=24 起行距 15、宽 100、高=15*(rows+2)，全部对齐 Angel。
3. **菜单按钮行为**（P1，已修）：BtSave 由错误的"加好友"改为 Angel 语义**复制名字到剪贴板 + 拉起聊天输入**；BtWhisper 改为 **set_whisper_target**（切密语频道预置 /w，保留输入态），与 Angel `SetWhisperTarget` 一致。BtInfo→CharInfoRequestPacket、BtTrade→交易邀请、BtClaim→举报、BtFamily→家族登记保持。
4. **个人情报窗健壮性与功能**（P1，已修）：`PlayerInteractionHandlers.cpp` 修不同 cid 复用同一 UICharInfo 导致的目标错位 bug；`UICharInfo::update_stats` 修 `target_character` 裸解引用崩溃（目标离图/跨频道）；BtVisit 私聊改为 set_whisper_target（Angel 语义）。

## Angel 权威证据（已落盘）
- 右键命中：`evidence/angel-rbutton-HandleRButtonClk.txt`、`angel-sub_9726EA_hittest.txt`（sub_9726EA 遍历 CAvatar 身体框做点-in-rect，未命中返回 0 → 不弹）。
- 菜单构造/按钮：`evidence/angel-contextmenu-decompile.json`、`angel-contextmenu-oncreate-full.txt`（按钮 id 2000..2008、坐标、条件标志 +52/+53/+54）。
- 点击行为：`angel-rbutton-HandleRButtonClk.txt` 中 DoModal==1 的 switch（case 0..8 映射到复制名字/交易/组队/公会/举报/家族等）。
- 个人情报窗：`evidence/angel-userinfo-*.txt`（从 corewindow calibration 的 full.json 提取，含 OnCreate/OnButtonClicked/Draw/SetLayer；确认 state→背景 backgrnd/2/5/7 映射与字段 y 坐标 77/96/113/131/144）。

## IMG 运行时核验（orange-wz 128B key，已落盘）
- `evidence/img_runtime_nodes.md`、`img_ContextMenu_tree.txt`、`img_UserInfo_tree.txt`、`img_userinfo_button_origins.json`。
- ContextMenu：t=100×23/c=97×15/s=97×7，12 个按钮态齐全（代码只用 Angel 的 8 个，BtAlertGM/BtBlock/BtCashTrade 属 BeiDou 定制且 s9 无协议，不接线）。
- UserInfo：扁平 v83 结构（无 character/farm/collect/damage/item 子容器），backgrnd..7 多背景板 + 大量 Bt*。

## 实机验证（服务端 s9 在线，1280×720）
- `evidence/live_00_game.png` 游戏场景；`live_dbg3_self_hit.png` **右击自己角色成功弹出菜单**（6 项：查看情報/保存名字/申请交易/申请好友/悄悄话/举报）。
- `evidence/live_dbg2_self.png`（调试期点偏 x=535 未命中，日志证实 SELF MISS）+ `ctxmenu_dbg` 日志：空地(900,100)/角色(535,350)/怪物(780,410) 均 MISS=不弹，正确角色位(640,330) HIT=弹 → 弹出条件修复**端到端验证通过**。
- `evidence/live_dbg5_info_open.png` + `live_charinfo_crop.png` 放大：点"查看情報"成功打开个人情报窗；字段值（等级350/职业黑骑士/人气0/家族-/联盟-）与背景对齐正确。

## 第二轮：个人情报窗按正确端截图像素级还原（用户补充截图后完成）
用户指出正确端截图早已提供（`C:\Users\11036\Desktop\正确客户端截图\个人请报框.png`，511×197），上一轮"缺截图无法对照"为遗漏。本轮以该截图为权威像素基准完成修复：

- **根因**：构造函数无条件 `sprites.emplace_back(backgrnd2)`（275×399"子窗全展开"大背景板），叠出中部 3 个空框 + 底部空白面板；所有按钮 pos=(0,0) 堆叠；BtPet/BtRide 被强制 DISABLED 成灰框；孤立 BtBook@(235,40) 压在"想买的"上。
- **修复**（`UICharInfo.h/.cpp`）：
  1. 默认态只画 `backgrnd`(275×199)，删除无条件 backgrnd2。
  2. 用 orange-wz 导出全部 UserInfo 按钮位图确认语义（见 `notes/r05-img-button-semantics.md`）：**学院 = BtFamily(蓝房子40×37)**、组队邀请=BtParty、交易邀请=BtTrade、道具=BtItem、想买的=BtWish、骑宠=BtTamingShow、宠物=BtPetShow、收藏=BtCollectionShow、排除道具=BtExceptionShow；按参照图逐像素量取坐标显式设置（见 `notes/r04-correct-screenshot-baseline.md`）。
  3. 枚举末尾追加 `BtWish`/`BtException`（保留旧值不破坏 send_key/draw 分段），draw 显式绘制；删除孤立 BtBook 加载与绘制。
  4. 去掉 BtPet/BtRide 强制禁用；看自己时禁用 BtParty + BtTrad（交易邀请），对齐参照图灰态。
- **实机对照**（`evidence/compare_final_ref_vs_live.png` 左参照右实机）：标题/立绘框/爱心/名字条、学院+组队邀请+交易邀请+道具+想买的顶部按钮、5 字段区、底部骑宠/宠物/收藏/排除道具全部逐像素对齐；空框/空白面板完全消除，BOOK INFO 重叠消除。字段区为保护区域未改（实测本来就对齐）。

## 第三轮：全功能与全子窗补齐（用户"个人情报上的所有功能都要补齐"）
以服务端 s9 `PacketCreator.charInfo` 为数据权威、运行时 UserInfo.img 背景板为像素权威，把此前所有空壳补齐（取证见 `notes/r06-subwindow-full-fill.md`）：

- **数据层**（`PlayerInteractionHandlers.cpp`）：CharInfoHandler 按 s9 顺序完整解析此前被 skip 的明细——宠物（3只 itemid/名字/等级/亲密度/饱清感/装备）、坐骑（id/等级/经验/疲劳度）、心愿单（itemid 数组）、图鉴（等级/普通/特殊/总卡片/封面）、勋章（id + 任务id 数组），构造 `CharInfoData` 交 UICharInfo。字段顺序逐项对应服务端，尾部安全跳过。
- **数据结构**（`UICharInfo.h`）：新增 `CharInfoData`（唯一数据源）、`SubPanel` 状态枚举（PET/MOUNT/WISH/ITEM/MEDAL/EXCLUDE，互斥+toggle）、`update_full()` 与各子窗 Text 成员。
- **UI 层**（`UICharInfo.cpp`）：7 张独立背景板全部启用——backgrnd2=宠物 / backgrnd3=想买的 / backgrnd4=道具 / backgrnd5=坐骑 / backgrnd6=排除道具 / backgrnd7=勋章图鉴；按真实数据渲染各子窗文本框；人气↑↓用 ColorBox 程序化绘制（IMG 无节点，R6 不造贴图），人气发包仍走 +/−/F 快捷键。
- **按钮接线**：BtWish→心愿子窗、BtItem→道具子窗、BtPet→宠物子窗、BtRide→坐骑子窗、BtCollect→勋章图鉴子窗、BtException→排除道具子窗；BtBook 接 UIMonsterBook 并加 `O` 快捷键（主窗无独立位置以保默认态对齐）；T/P/B 快捷键改为尊重按钮禁用态（看自己不再能发组队/交易/好友包）。
- **实机验证**（`evidence/live_sub_*.png`、`live_final2_*.png`）：心愿/道具/勋章图鉴/宠物四个子窗逐一展开成功，互斥切换正常，数据按 Admin 实际情况（0/无）正确显示；默认态保持上一轮像素对齐不被破坏。构建 EXITCODE=0，exe 16:54:47。

## 第四轮：右键链路全子 UI 闭环与全问题修复（用户"全部都补齐…有问题的都罗列"）
对右键菜单 8 项功能逐条审计发包→服务端 opcode→handler→UI 链路，修复所有发现问题（**完整映射表见 `notes/r07-full-chain-closure.md`**）：
- **举报链路**（问题最集中）：修打开即发包（改为 Enter 确认才 submit + 防重复提交）、补 1-4 理由选择 + D 描述 + 回执 UI（`SueCharacterHandler`→`UIReport::on_report_result`）；服务端 `ReportHandler.java` 补成功回包 mode=0 + 次数用尽回 mode=2 + 描述 sanitize；16 条 `report.*` locale 同步到部署目录；R08/R09 实机发现并修复文字重叠（目标名移入米色输入框、删压字标记、理由改 1-4 键选）。
- **交易链路**：`UITrade::send_icon` 反解 3×3 槽派发 `TradeAddItemPacket`，槽位/金币范围校验。
- **邀请/弹窗/聊天**：公会邀请加 `UIGameOpt::GAME_INVITE` 门禁；`load_window_bg`/UIYesNo 按钮加 `usable()` guard 防空壳可点；私聊回显统一走 `send_whisper_external` 出口。
- **CharInfo 健壮性**：CharInfoHandler 最小包长/宠物数/wishlist_size/图鉴/勋章全字段 `available()` 边界防御。
- **R09 实机验证**：auto login 进图 → 右键玩家 → 点"举报" 打开窗口，`evidence/live_r09_report.png` + `live_r09_report_crop.png` 确认标题/粉色条/米色框目标名/FOR/警告文字/底部蓝条状态均无重叠。构建 EXITCODE=0，exe 20:48:11 刷新。
- **未实机复现项（环境限制）**：SendKeys 与 keybd_event 均无法把按键送进该 SDL3 客户端（UIPI 拦 raw 输入），1-4/D/Enter/Esc 交互未截图确认，代码层 `send_key` 四分支齐全；服务端回执需用户重启 s9 后确认。
- 归档：`patches/full-chain-closure.diff`（相关 9 文件全量 diff）。

## 第五轮：右键菜单居中 + 举报窗可关闭（用户截图反馈，2026-11-16 晚）
用户实机截图反馈"列表偏右/名称偏左下没居中"与"举报窗打开关闭不了"，本轮修复（详见 `notes/r08-center-and-close.md`）：
- **菜单居中**（`UIMiscShells.h/.cpp`）：标题改 `Alignment::CENTER` + 锚点 `(窗口宽/2, 1)`（垂直锚点须避开 `font.linespace()≈12` 的加成，否则被标题栏底边截断）；按钮 x 由固定 9 改为 `(窗口宽-85)/2` 居中。实机 `evidence/live_r10_menu_final.png` 确认标题完整居中、列表居中。
- **举报窗可关**（`UI.cpp`）：根因是 Esc 派发白名单缺 `REPORT`/`CONTEXTMENU`（Esc 落全局兜底开目录）、X 键不经白名单。补白名单 + X 分支，复用 `get_front(types)` 层级优先（SoftKey 先关再关宿主，不动焦点系统）；`UISoftKey.cpp` 补 X→BtClose 避免吞键。实机 `evidence/live_r10_report_after_esc.png` 与 `..._after_x.png` 确认 Esc/X 均能关闭、不弹目录。
- 构建 EXITCODE=0，exe 21:40:01；驱动脚本 `r10_flow.ps1`（动态 PID + TOPMOST）比旧脚本可靠。

## 未决项（协议边界，非缺陷）
- **他人装备列表**：s9 的 CHAR_INFO（v83 协议）本身**不下发**装备明细，`UIEquipInventory` 只接受本地 Inventory 无法显示他人装备——客户端单侧无法补齐，属协议边界，不臆造。已注释说明，道具子窗板保留但无明细数据。
- **排除道具子窗数据**：PET_EXCEPTION_LIST(173) 数据当前由 `PetExceptionListHandler` 记录日志丢弃；子窗背景板已可用，如需展示具体排除项需另行收集该包数据（未在本轮范围）。
- 底部按钮颜色差异（参照图骑宠/宠物灰、实机橙）：数据驱动状态差异（角色是否有坐骑/宠物），非布局错误。
- 运行时 IMG 的 BtAlertGM/BtBlock/BtCashTrade：s9 服务端无对应 opcode（code-explorer 审计确认），按门禁不造协议，保持不接线。

## 构建
- `tools/build_smoke.bat` Release/x64，最终 EXITCODE=0，GATE OK（opcode matrix 匹配 s9），exe 20:48:11 刷新到部署目录（晚于 UIMiscShells.cpp 20:16:15，确认含 R09 举报窗修复）。
- 构建前须结束 MapleStory.exe；本次所有临时调试日志（`ctxmenu_dbg`）已从源码删除。

## 改动文件
- `IO/UIStateGame.cpp` — rightclick 命中判定
- `IO/UITypes/UIMiscShells.cpp/.h` — UIContextMenu 列表/条件/行为 + **第四轮举报窗重写（目标名入米色框/1-4 理由/回执/去重叠）+ load_window_bg usable guard**
- `IO/UITypes/UICharInfo.h/.cpp` — 空指针防护 + BtVisit 语义 + 第二轮像素级重构 + 第三轮全子窗 + 人气快捷键禁用态门禁
- `Net/Handlers/PlayerInteractionHandlers.cpp` — CHAR_INFO 目标 cid 复用修复 + 第四轮全字段边界防御
- `IO/UITypes/UITrade.cpp/.h` — send_icon 拖拽发包 + 槽位/金币校验
- `IO/UITypes/UINotice.cpp` — UIYesNo 按钮 usable guard
- `IO/UITypes/UIChatBar.cpp/.h` — 私聊回显统一出口
- `Net/Handlers/GuildHandlers.cpp` — 公会邀请 GameOpt 门禁
- 服务端（s9 仓库）：`ReportHandler.java` — 成功回包/次数用尽/sanitize
- 部署：`locale/*.ini` 16 条 report.*；`xy-config.yaml` auto 验证后已恢复 false

## 工具/证据（新增）
- `tools/ida_angel_gui_*.py`、`tools/wz_query_*.py`、`tools/ctxmenu_capture.ps1`、`tools/ctxmenu_verify_elevated.ps1`
- `notes/client-path.md`、`notes/r01-angel-native-facts.md`、`notes/r02-img-node-audit.md`、`notes/r03-current-vs-angel-gap.md`
- `evidence/*` 反编译原文 + IMG 树 + 实机截图
