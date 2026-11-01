# v95 ↔ s9（v83）Opcode 映射

> 目的：为「095 特性移植 + 协议 v2」提供 opcode 对照。**两套编号不可混用**，本表只回答「同一系统在两边叫什么、值是多少、证据强度」。
> 输入：`notes/opcode-v95.md`（v95 收发表，582 ctor 点 / 238 发送 opcode / 375 接收 opcode）、`E:\project\_ms_probe\send_sites_all.json`、`s9_opcodes.json`。
> 生成脚本：`E:\project\_ms_probe\gen_opcode_map.py`；机器候选：`map_candidates2.json`。
> 证据源：`E:\MXD\Maplestory95.i64`（GMS v95 未加壳，IDA 13338）；s9 侧为 `BeiDou-Server_s9` 的 `RecvOpcode.java` / `SendOpcode.java`。

## 0. 结论

1. **v95 发送侧已 100% 覆盖**：582/582 个 `COutPacket(long)` 构造点解析为 **238** 个发送 opcode（旧表 217，含 15 个误判“动态 opcode”+ 80 个漏点）。
2. 两边**只有 7 个同名同义**的直配（VIEW_ALL_CHAR/DELETE_CHAR/SKILL_EFFECT/PET_FOOD/USE_REMOTE/WATER_OF_LIFE/USE_ITEM_REWARD），其余必须按系统语义手工对齐。
3. v95 把 v83 的多个 opcode **合并**：技能统一进 103、交互统一进 144、攻击统一进 `CUserRemote::OnAttack(nType)`（接收 214）；s9 侧仍是拆分的多包。移植时**不能一对一照搬**，需按 s9 的拆分粒度重排。
4. 攻击发送侧仍有缺口：近战/远程/普攻/体术的发送 opcode **未能静态取证**（见 §5.3），必须抓包校准。
5. 中文 095（PLUS2 / 加壳）的自定义 opcode 仍未知；本表只代表 GMS v95 语义。

## 1. 口径与证据等级

| 等级 | 含义 |
|---|---|
| 证据 | 函数名/系统语义 + 数值可直接对上；可直接用于移植设计 |
| 待核 | 系统对得上，但 s9 侧拆分粒度、子码或入口不唯一；需抓包或读 s9 handler 确认 |
| 无对应 | v95 有、s9 完全没有（或反之），移植时需新增协议或明确不做 |

“v95 发送”= v95 客户端 → 服务端，对应 s9 `RecvOpcode`（服务端收包）。
“v95 接收”= 服务端 → v95 客户端，对应 s9 `SendOpcode`（服务端发包）。

## 2. 顶层分发对比

| 维度 | v95（GMS） | s9（v83） |
|---|---|---|
| opcode 宽度 | uint16 | uint16 |
| 分发方式 | 登录 0–27（含连接层特例 16–23）→ `CLogin`；28–140 → `CWvsContext`；141–432 → `g_pStage`（`CField`/`CCashShop`） | `Net/PacketSwitch.cpp` 线性 switch（302 个名字） |
| 发送 opcode 空间 | 1–313（实测 238 个在用） | 1–260 + 277 + 4096 + 14099（181 个在用） |
| 接收 opcode 空间 | 0–432（375 个在用） | 0–376 + 4096 + 14100（311 个在用） |
| 攻击拆分 | 接收侧单包 `OnAttack(nType)`；发送侧近战/远程/魔法分散 | 收发都拆成 CLOSE_RANGE / RANGED / MAGIC / ENERGY |
| 交互拆分 | 144 一个包覆盖交易房/个人商店/委托商店/小游戏 | 123 `PLAYER_INTERACTION` 一个包 + 子码 |
| 快捷键 | 20/32 连接层、25 心跳、35 登录前导 | 无 |

> 结论：v95 的“区间分发”比 s9 的线性 switch 更省，但 opcode 数值完全不可复用。

## 3. v95 发送 → s9 收包（`RecvOpcode`）

表中“v95 发送函数”只列首个调用点（完整列表见 `notes/opcode-v95.md` §5 与 `send_ops.json`）。

### 3.1 登录 / 角色

| v95 | v95 发送函数（首个） | s9 RecvOpcode | 值 | 置信 | 备注 |
|---|---|---|---|---|---|
| 1 | `CLogin::SendCheckPasswordPacket` | `LOGIN_PASSWORD` | 1 | 证据 | 同名 |
| 4 | `CLogin::ChangeStepImmediate`（共 2 个函数） | — | — | 无对应 | 登录阶段切换，客户端内部 |
| 6 | `CLogin::SendCheckUserLimitPacket` | `SERVERSTATUS_REQUEST` | 6 | 待核 | 检查世界人数 ≈ 世界状态请求 |
| 7 | `CLogin::OnAcceptLicense`（共 2 个函数） | `ACCEPT_TOS` | 7 | 证据 | 同名 |
| 8 | `CLogin::SendCancelGenderPacket`（共 2 个函数） | `SET_GENDER` | 8 | 证据 | 同名 |
| 9 | `CLogin::OnCheckPasswordResult`（共 4 个函数） | `AFTER_LOGIN` | 9 | 证据 | 同名 |
| 10 | `CLogin::OnCheckPinCodeResult` | `REGISTER_PIN` | 10 | 证据 | PIN 注册 |
| 11 | `CLicenseDlg::OnButtonClicked`（共 3 个函数） | `SERVERLIST_REQUEST` | 11 | 证据 | 牌照/服务器列表请求 |
| 12 | `CLogin::GotoWorldSelect`（共 2 个函数） | — | — | 待核 | 世界选择信息请求，s9 无独立包 |
| 13 | `CLogin::SendViewAllCharPacket` | `VIEW_ALL_CHAR` | 13 | 证据 | 同名 |
| 14 | `CLogin::SendSelectCharPacketByVAC` | `PICK_ALL_CHAR` | 14 | 证据 | VAC 选角 |
| 15 | `CLogin::MakeVACDlg`（共 2 个函数） | — | — | 无对应 | VAC 对话框本地逻辑 |
| 16 | `CCashShop::SendCheckNameChangePossiblePacket` | `NAME_TRANSFER` | 16 | 待核 | 改名校验 |
| 18 | `CCashShop::SendCheckTransferWorldPossiblePacket` | `WORLD_TRANSFER` | 18 | 证据 | 同名 |
| 19 | `CLogin::SendSelectCharPacket` | `CHAR_SELECT` | 19 | 证据 | 同名 |
| 20 | `CClientSocket::OnConnect` | — | — | 无对应 | 连接建立（v95 新增） |
| 21 | `CCashShop::SendCheckDuplicateIDPacket`（共 2 个函数） | `CHECK_CHAR_NAME` | 21 | 证据 | 重复名检查 |
| 22 | `CLogin::SendNewCharPacket` | `CREATE_CHAR` | 22 | 证据 | 建角 |
| 23 | `CLogin::SendNewCharPacket` | — | — | 待核 | SendNewCharPacket 第二段，语义未定 |
| 24 | `CLogin::SendDeleteCharPacket` | `DELETE_CHAR` | 23 | 证据 | 删角（值 23，非 24） |
| 25 | `CClientSocket::OnAliveReq` | `PONG` | 24 | 证据 | 心跳回应 |
| 27 | `CSecurityClient::OnCheckClientIntegrityRequest` | — | — | 无对应 | 客户端完整性校验（HS 系） |
| 28 | `CLogin::SendSelectCharPacket` | `CHAR_SELECT_WITH_PIC` | 30 | 待核 | 带 PIC 的选角 |
| 29 | `CLogin::SendSelectCharPacket` | `VIEW_ALL_WITH_PIC` | 32 | 待核 | 带 PIC 的 VAC 选角 |
| 30 | `CLogin::SendSelectCharPacketByVAC` | — | — | 无对应 | VAC 选角 |
| 31 | `CLogin::SendSelectCharPacketByVAC` | — | — | 无对应 | VAC 选角 |
| 32 | `CClientSocket::OnConnect` | — | — | 无对应 | 连接建立第二包（v95 新增） |
| 34 | `CLogin::Init` | — | — | 无对应 | CLogin::Init |
| 35 | `CLogin::SendCheckPasswordPacket`（共 3 个函数） | — | — | 待核 | 登录/选角前导包，语义未定 |
| 36 | `CWvsApp::SendBackupPacket`（共 2 个函数） | — | — | 无对应 | 备份/堆栈日志上报 |

### 3.2 地图 / 移动 / 聊天 / 表情

| v95 | v95 发送函数（首个） | s9 RecvOpcode | 值 | 置信 | 备注 |
|---|---|---|---|---|---|
| 41 | `CCashShop::SendTransferFieldPacket`（共 3 个函数） | `CHANGE_MAP` | 38 | 证据 | 换图 |
| 42 | `CField::SendTransferChannelRequest` | `CHANGE_CHANNEL` | 39 | 证据 | 换频道 |
| 43 | `CWvsContext::SendMigrateToShopRequest` | `ENTER_CASHSHOP` | 40 | 证据 | 进商城 |
| 44 | `CUserLocal::OnKey` | — | — | 待核 | 单键动作（v95 新），s9 无通用按键包 |
| 45 | `CUserLocal::HandleXKeyDown`（共 2 个函数） | `CANCEL_CHAIR` | 42 | 证据 | 起身 |
| 46 | `CWvsContext::SendSitOnPortableChairRequest` | `USE_CHAIR` | 43 | 证据 | 坐下（不是攻击！） |
| 51 | `CUserLocal::TryDoingSmoothingMovingShootAttackPre...` | — | — | 待核 | 移动射击预备（v95 新） |
| 52 | `CUserLocal::SetDamaged`（共 2 个函数） | — | — | 待核 | 受击/延迟上报，语义未定 |
| 54 | `CField::SendChatMsg`（共 2 个函数） | `GENERAL_CHAT` | 49 | 证据 | 普通/斜杠聊天 |
| 55 | `CUserLocal::HandleLButtonClk` | `MOVE_PLAYER` | 41 | 证据 | 点击移动请求 |
| 56 | `CUserLocal::Update`（共 2 个函数） | `FACE_EXPRESSION` | 51 | 证据 | 表情 |
| 57 | `CWvsContext::SendActiveEffectItemChange` | `USE_ITEMEFFECT` | 52 | 证据 | 道具特效 |
| 58 | `CUserLocal::RequestUpgradeTombEffect` | `USE_DEATHITEM` | 53 | 待核 | 墓碑升级特效 |
| 61 | `CUserLocal::SendBanMapByMobRequest` | `MOB_BANISH_PLAYER` | 56 | 证据 | 怪物驱逐玩家 |

### 3.3 NPC / 商店 / 仓库 / 快递 / 委托

| v95 | v95 发送函数（首个） | s9 RecvOpcode | 值 | 置信 | 备注 |
|---|---|---|---|---|---|
| 63 | `CNpc::ShowQuestList`（共 2 个函数） | `NPC_TALK` | 58 | 证据 | 对话 NPC |
| 64 | `CWvsContext::SendRemoteShopOpenRequest` | `REMOTE_STORE` | 59 | 证据 | 远程商店 |
| 65 | `CScriptMan::OnAskAvatar`（共 14 个函数） | `NPC_TALK_MORE` | 60 | 证据 | 对话选项/回复 |
| 66 | `CShopDlg::SendBuyRequest`（共 4 个函数） | `NPC_SHOP` | 61 | 证据 | NPC 商店 |
| 67 | `CTrunkDlg::SendGetItemRequest`（共 6 个函数） | `STORAGE` | 62 | 证据 | 仓库 |
| 68 | `CWvsContext::SendEntrustedShopCheckRequest` | `HIRED_MERCHANT_REQUEST` | 63 | 证据 | 雇佣/委托商店 |
| 69 | `CStoreBankDlg::SendCalculateFeeRequest`（共 3 个函数） | `FREDRICK_ACTION` | 64 | 证据 | 商店银行（Fredrick） |
| 70 | `CParcelDlg::CloseParcelDlg`（共 6 个函数） | `DUEY_ACTION` | 65 | 证据 | 快递（Duey） |
| 71 | `CUserLocal::SendSkillEffectRequest` | `SKILL_EFFECT` | 93 | 证据 | 技能特效请求 |
| 72 | `CUIShopScanner::OnCreate` | `OWL_ACTION` | 66 | 证据 | 商店搜索（猫头鹰） |
| 73 | `CUIShopScanResult::OnButtonClicked` | `OWL_WARP` | 67 | 证据 | 搜索结果传送 |
| 74 | `CAdminShopDlg::OnPacket`（共 4 个函数） | `ADMIN_SHOP` | 68 | 证据 | 管理商店 |

### 3.4 物品 / 装备 / 技能 / 能力

| v95 | v95 发送函数（首个） | s9 RecvOpcode | 值 | 置信 | 备注 |
|---|---|---|---|---|---|
| 75 | `CWvsContext::SendGatherItemRequest` | `ITEM_SORT` | 69 | 证据 | 整包/合并 |
| 76 | `CWvsContext::SendSortItemRequest` | `ITEM_SORT2` | 70 | 证据 | 整理 |
| 77 | `CWvsContext::SendChangeSlotPositionRequest` | `ITEM_MOVE` | 71 | 证据 | 槽位移动 |
| 78 | `CWvsContext::SendStatChangeItemUseRequest` | `USE_ITEM` | 72 | 证据 | 使用道具 |
| 79 | `CWvsContext::SendStatChangeItemCancelRequest` | `CANCEL_ITEM_EFFECT` | 73 | 证据 | 取消道具效果 |
| 80 | `CWvsContext::TryRecovery` | `HEAL_OVER_TIME` | 89 | 待核 | TryRecovery（恢复请求） |
| 81 | `CWvsContext::SendMobSummonItemUseRequest` | `USE_SUMMON_BAG` | 75 | 证据 | 召唤袋 |
| 82 | `CWvsContext::SendPetFoodItemUseRequest` | `PET_FOOD` | 76 | 证据 | 宠物食物 |
| 83 | `CWvsContext::SendTamingMobFoodItemUseRequest` | `USE_MOUNT_FOOD` | 77 | 证据 | 坐骑食物 |
| 84 | `CWvsContext::SendScriptRunItemRequest` | `SCRIPTED_ITEM` | 78 | 证据 | 脚本道具 |
| 85 | `CItemSpeakerDlg::_SendConsumeCashItemUseRequest`（共 6 个函数） | `USE_CASH_ITEM` | 79 | 证据 | 现金道具 |
| 86 | `CWvsContext::SendActivatePetRequest` | `SPAWN_PET` | 98 | 证据 | 召唤宠物 |
| 87 | `CWvsContext::SendBridleItemUseRequest` | `USE_CATCH_ITEM` | 81 | 证据 | 捕捉（驯服）道具 |
| 88 | `CWvsContext::SendSkillLearnItemUseRequest` | `USE_SKILL_BOOK` | 82 | 证据 | 技能书 |
| 89 | `CWvsContext::SendSkillResetItemUseRequest` | — | — | 待核 | SP 重置道具，s9 无独立包 |
| 90 | `CWvsContext::SendShopScannerItemUseRequest` | — | — | 无对应 | 商店扫描器道具 |
| 91 | `CWvsContext::SendMapTransferItemUseRequest` | `USE_TELEPORT_ROCK` | 84 | 证据 | 传送石 |
| 92 | `CWvsContext::SendPortalScrollUseRequest` | `USE_RETURN_SCROLL` | 85 | 证据 | 回城卷 |
| 93 | `CWvsContext::SendUpgradeItemUseRequest` | `USE_UPGRADE_SCROLL` | 86 | 证据 | 强化卷 |
| 94 | `CWvsContext::SendHyperUpgradeItemUseRequest` | — | — | 待核 | 超级强化（v95 新增） |
| 95 | `CWvsContext::SendItemOptionUpgradeItemUseRequest` | — | — | 待核 | 潜能强化（v95 新增） |
| 96 | `CWvsContext::SendUIOpenItemRequest` | — | — | 无对应 | UI 开启道具 |
| 97 | `CWvsContext::SendItemReleaseRequest` | — | — | 待核 | 解封道具 |
| 98 | `CWvsContext::SendAbilityUpRequest` | `DISTRIBUTE_AP` | 87 | 证据 | AP 分配 |
| 99 | `CWvsContext::SendAbilityUpRequest` | `AUTO_DISTRIBUTE_AP` | 88 | 证据 | 自动分配 AP |
| 100 | `CWvsContext::SendStatChangeRequest` | `DISTRIBUTE_SP` | 90 | 待核 | 属性变更请求 |
| 101 | `CWvsContext::SendStatChangeRequestByItemOption` | — | — | 待核 | 潜能属性变更 |
| 102 | `CWvsContext::SendSkillUpRequest` | `DISTRIBUTE_SP` | 90 | 证据 | 技能点 |
| 103 | `CGrenade::SendTimeBombInfo`（共 15 个函数） | `SPECIAL_MOVE` | 91 | 证据 | 统一技能：主动技能/召唤/怪物磁铁/吞噬 |
| 104 | `CUserLocal::SendSkillCancelRequest` | `CANCEL_BUFF` | 92 | 证据 | 取消技能 |
| 105 | `CUserLocal::DoActiveSkill_Prepare` | — | — | 待核 | 技能预备（v95 新） |
| 106 | `CWvsContext::SendDropMoneyRequest` | `MESO_DROP` | 94 | 证据 | 丢金币 |
| 107 | `CWvsContext::SendGivePopularityRequest` | `GIVE_FAME` | 95 | 证据 | 人气 |
| 109 | `CWvsContext::SendCharacterInfoRequest` | `CHAR_INFO_REQUEST` | 97 | 证据 | 查看角色信息 |
| 110 | `CWvsContext::SendActivatePetRequest` | `SPAWN_PET` | 98 | 证据 | 宠物激活第二入口 |
| 111 | `CWvsContext::CheckTemporaryStatDuration` | `CANCEL_DEBUFF` | 99 | 待核 | 临时状态到期 |
| 112 | `CUserLocal::CheckPortal_Collision`（共 2 个函数） | `CHANGE_MAP_SPECIAL` | 100 | 待核 | 传送门/上键 |
| 113 | `CUserLocal::TryRegisterTeleport` | `TROCK_ADD_MAP` | 102 | 证据 | 传送石记录地图 |
| 114 | `CWvsContext::SendMapTransferRequest` | `CHANGE_MAP_SPECIAL` | 100 | 待核 | 地图转移请求 |
| 115 | `CWvsContext::SendAntiMacroItemUseRequest` | — | — | 待核 | 反外挂道具 |
| 116 | `CUserLocal::DoAntiMacroSkill` | — | — | 待核 | 反外挂技能 |
| 117 | `CUIAdminAntiMacro::SetRet`（共 2 个函数） | — | — | 待核 | 反外挂结果回执 |
| 118 | `CWvsContext::SendClaimRequest` | — | — | 待核 | 找回请求 |
| 119 | `CQuest::OnCompleteQuestFailed`（共 3 个函数） | `QUEST_ACTION` | 107 | 证据 | 任务开始/放弃/完成 |
| 120 | `CWvsContext::CheckDarkForce`（共 4 个函数） | `CANCEL_BUFF` | 92 | 待核 | 状态重置/被动结算 |
| 121 | `CUserLocal::ThrowGrenade` | `GRENADE_EFFECT` | 109 | 证据 | 手雷 |
| 122 | `CMacroSysMan::FlushToSvr` | `SKILL_MACRO` | 110 | 证据 | 技能宏 |
| 123 | `CWvsContext::SendSelectNpcItemUseRequest` | `USE_ITEM_REWARD` | 112 | 证据 | NPC 选择道具 |
| 124 | `CWvsContext::SendLotteryItemUseRequest` | `USE_TREASUER_CHEST` | 115 | 待核 | 抽奖道具 |
| 125 | `CUIItemMaker::RequestItemMake` | `MAKER_SKILL` | 113 | 证据 | 制造 |
| 126 | `CField::SendChatMsgSlash` | `ADMIN_CHAT` | 118 | 待核 | 斜杠命令 |
| 127 | `CWvsContext::SendUseBoxGachaponItemRequest` | `USE_GACHA_EXP` | 158 | 待核 | 扭蛋盒 |
| 128 | `CWvsContext::SendUseGachaponRemoteRequest` | `USE_REMOTE` | 116 | 证据 | 远程扭蛋 |
| 129 | `CWvsContext::SendWaterOfLife` | `WATER_OF_LIFE` | 117 | 证据 | 生命之水 |
| 130 | `CRepairDurabilityDlg::SendRepairDurabilityAll` | — | — | 待核 | 耐久修复（全部） |
| 131 | `CRepairDurabilityDlg::SendRepairDurability` | — | — | 待核 | 耐久修复 |
| 132 | `CDamageMeter::SaveDamageInfo` | — | — | 无对应 | DPS 记录 |
| 133 | `CUserLocal::UpdateClientTimer` | — | — | 待核 | 客户端计时器同步 |
| 134 | `CWvsContext::SendFollowCharacterRequest` | — | — | 待核 | 跟随角色（v95 新） |
| 136 | `CUIPQReward::SelectReward` | — | — | 待核 | PQ 奖励选择 |
| 137 | `CUIPQReward::RequestReward` | — | — | 待核 | PQ 奖励领取 |
| 138 | `CWvsContext::OnSetPassenserRequest`（共 2 个函数） | — | — | 待核 | 跟随/同乘 |
| 139 | `CField::SendChatMsgSlash` | `ADMIN_CHAT` | 118 | 待核 | 斜杠命令 |

### 3.5 社交 / 组队 / 公会 / 联盟 / 家族

| v95 | v95 发送函数（首个） | s9 RecvOpcode | 值 | 置信 | 备注 |
|---|---|---|---|---|---|
| 140 | `CUIStatusBar::SendGroupMessage` | `MULTI_CHAT` | 119 | 证据 | 组队/多聊 |
| 141 | `CField::OnWhisper`（共 3 个函数） | `WHISPER` | 120 | 证据 | 私聊 |
| 142 | `CUIStatusBar::SendCoupleMessage` | `SPOUSE_CHAT` | 121 | 证据 | 夫妻聊 |
| 143 | `CFadeWnd::SendCloseMessage`（共 7 个函数） | `MESSENGER` | 122 | 证据 | 信使 |
| 144 | `??_ECMemoryGameDlg@@W7AEPAXI@Z`（共 57 个函数） | `PLAYER_INTERACTION` | 123 | 证据 | 小游戏/交易房/个人商店/委托商店统一 |
| 145 | `CField::SendChangePartyBossMsg`（共 6 个函数） | `PARTY_OPERATION` | 124 | 证据 | 组队 |
| 146 | `CFadeWnd::SendCloseMessage`（共 3 个函数） | `DENY_PARTY_REQUEST` | 125 | 证据 | 拒绝组队 |
| 147 | `ExpeditionIntermediary::OnPacketExpNoti_Invite`（共 9 个函数） | — | — | 无对应 | 远征队（s9 收包无） |
| 148 | `CUIFadeYesNo::OnButtonClicked`（共 8 个函数） | `PARTY_SEARCH_REGISTER` | 220 | 证据 | 组队招募（另有 START 222 / UPDATE 223） |
| 149 | `CField::InputGuildName`（共 11 个函数） | `GUILD_OPERATION` | 126 | 证据 | 公会 |
| 150 | `CFadeWnd::SendCloseMessage`（共 2 个函数） | `DENY_GUILD_REQUEST` | 127 | 证据 | 拒绝公会 |
| 151 | `CField::SendChatMsgSlash`（共 3 个函数） | — | — | 待核 | 斜杠命令/进图按键 |
| 152 | `CField::SendChatMsgSlash` | `ADMIN_COMMAND` | 128 | 待核 | 斜杠命令 |
| 153 | `CField::SendAcceptFriendMsg`（共 4 个函数） | `BUDDYLIST_MODIFY` | 130 | 证据 | 好友 |
| 154 | `CCashShop::OnCashItemResLoadGiftDone`（共 3 个函数） | `NOTE_ACTION` | 131 | 证据 | 便签 |
| 156 | `CField::TryEnterTownPortal`（共 2 个函数） | `USE_DOOR` | 133 | 证据 | 城镇传送门 |
| 157 | `COpenGatePool::TryEnterOpenGate` | `USE_DOOR` | 133 | 待核 | 开洞传送门（s9 合并入 133） |
| 158 | `CField::SendChatMsgSlash` | `ADMIN_LOG` | 129 | 待核 | 斜杠命令 |
| 159 | `CFuncKeyMappedMan::ChangePetConsumeItemID`（共 3 个函数） | `CHANGE_KEYMAP` | 135 | 证据 | 键位映射 |
| 160 | `CRPSGameDlg::OnBtContinue`（共 6 个函数） | `RPS_ACTION` | 136 | 证据 | 猜拳 |
| 161 | `CEngageDlg::SetRet`（共 7 个函数） | `RING_ACTION` | 137 | 证据 | 戒指/订婚 |
| 162 | `??_ECWishListInputDlg@@W7AEPAXI@Z`（共 3 个函数） | `WEDDING_ACTION` | 138 | 待核 | 结婚心愿单 |
| 163 | `CField_Wedding::OnWeddingProgress` | `WEDDING_ACTION` | 138 | 证据 | 婚礼进度 |
| 164 | `CField_Wedding::OnWeddingProgress` | `WEDDING_TALK` | 139 | 证据 | 婚礼对话 |
| 165 | `CWvsContext::SendBoobyTrapAlert` | — | — | 待核 | 陷阱警报 |
| 166 | `CUIMiniMap::OnMouseButton` | `WORLD_MAP_PLAYERS` | 277 | 待核 | 小地图/世界地图玩家 |
| 167 | `CTabGuildAlliance::OnChangeMaster`（共 10 个函数） | `ALLIANCE_OPERATION` | 143 | 证据 | 联盟 |
| 168 | `CFadeWnd::SendCloseMessage`（共 2 个函数） | `DENY_ALLIANCE_REQUEST` | 144 | 证据 | 拒绝联盟 |
| 169 | `CWvsContext::SendFamilyChartRequest` | `OPEN_FAMILY_PEDIGREE` | 145 | 证据 | 家族族谱 |
| 170 | `CWvsContext::SendFamilyInfoRequest` | `OPEN_FAMILY` | 146 | 证据 | 家族信息 |
| 171 | `CWvsContext::SendRegisterJunior` | `ADD_FAMILY` | 147 | 证据 | 登记晚辈 |
| 172 | `CWvsContext::SendUnregisterJunior` | `SEPARATE_FAMILY_BY_SENIOR` | 148 | 证据 | 解除晚辈 |
| 173 | `CWvsContext::SendUnregisterParent` | `SEPARATE_FAMILY_BY_JUNIOR` | 149 | 证据 | 解除长辈 |
| 174 | `CWvsContext::SendFamilyInviteResult` | `ACCEPT_FAMILY` | 150 | 证据 | 家族邀请结果 |
| 175 | `CWvsContext::SendUseFamilyPrivilege` | `USE_FAMILY` | 151 | 证据 | 家族特权 |
| 176 | `CWvsContext::SendSetFamilyPrecept` | `CHANGE_FAMILY_MESSAGE` | 152 | 证据 | 家族箴言 |
| 177 | `CWvsContext::OnFamilySummonRequest` | `FAMILY_SUMMON_RESPONSE` | 153 | 证据 | 家族召唤响应 |
| 179 | `CUIGuildBBS::OnComment`（共 6 个函数） | `BBS_OPERATION` | 155 | 证据 | 公会 BBS |

### 3.6 商城 / ITC / 活动 / 其他 UI

| v95 | v95 发送函数（首个） | s9 RecvOpcode | 值 | 置信 | 备注 |
|---|---|---|---|---|---|
| 180 | `CWvsContext::SendMigrateToITCRequest` | `ENTER_MTS` | 156 | 证据 | 进 ITC/MTS |
| 181 | `CWvsContext::SendExpUpItemUseRequest` | `USE_GACHA_EXP` | 158 | 待核 | 经验道具 |
| 182 | `CWvsContext::SendTempExpUseRequest` | `USE_GACHA_EXP` | 158 | 待核 | 临时经验道具 |
| 183 | `CUIFadeYesNo::OnButtonClicked`（共 2 个函数） | `NEW_YEAR_CARD_REQUEST` | 159 | 证据 | 新年卡 |
| 184 | `CUIRandomMorphDlg::_SendMorphRequest` | — | — | 待核 | 随机变形 |
| 185 | `CUICashItemGachapon::OnButtonClicked` | `CASHSHOP_SURPRISE` | 161 | 待核 | 现金扭蛋 |
| 186 | `CUICashGachapon::OnButtonClicked` | `CASHSHOP_SURPRISE` | 161 | 待核 | 现金扭蛋 |
| 187 | `CCashShop::SendChangeMaplePoint` | — | — | 待核 | MaplePoint 变更 |
| 188 | `CUserLocal::HandleLButtonDblClk` | — | — | 待核 | 双击交互 |
| 189 | `CUserLocal::RequestIncCombo` | `ARAN_COMBO_COUNTER` | 163 | 证据 | 连击计数 |
| 190 | `CMobPool::OnMobCrcKeyChanged` | — | — | 无对应 | 怪物 CRC key 同步 |
| 191 | `CWvsContext::SendRequestSessionValue` | — | — | 待核 | 会话值请求 |
| 192 | `CWvsContext::CheckOpBoardHasNew` | — | — | 待核 | 公告板新内容检查 |
| 193 | `CUIAccountMoreInfo::SendLoadAccountMoreInfoRequest`（共 2 个函数） | — | — | 待核 | 账号附加信息 |
| 194 | `CUIFindFriend::SendMyInfoRequest`（共 3 个函数） | — | — | 待核 | 好友搜索 |
| 195 | `CUserLocal::OnAskAPSPEvent` | — | — | 待核 | APSP 事件 |
| 196 | `CWvsContext::SendDragonBallBoxRequest` | — | — | 待核 | 龙珠盒 |
| 197 | `CWvsContext::SendDragonBallSummonRequest` | — | — | 待核 | 龙珠召唤 |
| 274 | `CCashShop::TrySendQueryCashRequest` | `CHECK_CASH` | 228 | 证据 | 查询点券 |
| 275 | `CCashShop::ApplyWishListEvent`（共 25 个函数） | `CASHSHOP_OPERATION` | 229 | 证据 | 商城操作 |
| 276 | `CCashShop::OnStatusCoupon` | `COUPON_CODE` | 230 | 证据 | 优惠券 |
| 283 | `CUIRaiseWndBase::OnCreate` | `OPEN_ITEMUI` | 236 | 证据 | Raise 窗口打开 |
| 284 | `COutPacket::COutPacket`（共 2 个函数） | `OPEN_ITEMUI` | 236 | 待核 | Raise 窗口构造 |
| 285 | `CUIRaiseWnd::SendPutItem` | `USE_ITEMUI` | 238 | 证据 | Raise 放入 |
| 286 | `CUIRaisePieceWnd::SendPutItem` | `USE_ITEMUI` | 238 | 证据 | Raise 分片放入 |
| 291 | `CClassCompetition::SendRequestAuthKey` | — | — | 待核 | 职业竞赛鉴权 |
| 292 | `CUIWebEvent::SendRequestAuthKey` | — | — | 待核 | 网页活动鉴权 |
| 296 | `CUIItemUpgrade::Update` | — | — | 待核 | 道具升级窗口 |
| 299 | `CBattleRecordMan::RequestOnCalc` | — | — | 待核 | 战斗记录计算 |
| 306 | `CITC::OnStatusCharge` | `MTS_OPERATION` | 253 | 证据 | ITC 充值 |
| 307 | `CITC::TrySendQueryCashRequest` | `MTS_OPERATION` | 253 | 待核 | ITC 查点券 |
| 308 | `CITC::OnBuy`（共 18 个函数） | `MTS_OPERATION` | 253 | 证据 | ITC 买卖/登记 |
| 311 | `CUICharacterSaleDlg::SendCheckDuplicateIDPacket` | — | — | 待核 | 角色出售重名检查 |
| 313 | `CUILogoutGift::OnButtonClicked` | — | — | 待核 | 登出礼物 |

### 3.7 宠物 / 召唤兽 / 龙 / 快捷槽

| v95 | v95 发送函数（首个） | s9 RecvOpcode | 值 | 置信 | 备注 |
|---|---|---|---|---|---|
| 199 | `CVecCtrlPet::EndUpdateActive` | `MOVE_PET` | 167 | 证据 | 宠物移动 |
| 200 | `CPet::DoAction` | `PET_COMMAND` | 169 | 证据 | 宠物指令 |
| 201 | `CPet::ParseCommand` | `PET_CHAT` | 168 | 证据 | 宠物说话 |
| 202 | `CPet::SendDropPickUpRequest` | `PET_LOOT` | 170 | 证据 | 宠物拾取 |
| 203 | `CWvsContext::SendStatChangeItemUseRequestByPetQ` | `PET_AUTO_POT` | 171 | 证据 | 宠物自动药 |
| 204 | `CPet::SendUpdateExceptionListRequest` | `PET_EXCLUDE_ITEMS` | 172 | 证据 | 宠物排除列表 |
| 207 | `CVecCtrlSummoned::EndUpdateActive` | `MOVE_SUMMON` | 175 | 证据 | 召唤兽移动 |
| 209 | `CSummoned::SetDamaged` | `DAMAGE_SUMMON` | 177 | 证据 | 召唤兽受伤 |
| 210 | `CSummoned::TryDoingGiveBuff`（共 4 个函数） | `SUMMON_ATTACK` | 176 | 证据 | 召唤兽攻击/治疗/召唤 |
| 211 | `CSummoned::SendRemove` | — | — | 待核 | 移除召唤兽 |
| 214 | `CVecCtrlDragon::EndUpdateActive` | `MOVE_DRAGON` | 181 | 证据 | 龙移动 |
| 216 | `CQuickslotKeyMappedMan::SaveQuickslotKeyMap` | `CHANGE_QUICKSLOT` | 183 | 证据 | 快捷槽 |
| 217 | `CUserLocal::UpdatePassiveSkillData` | — | — | 待核 | 被动技能数据 |
| 218 | `CWvsContext::UI_Menu` | — | — | 待核 | UI 菜单 |
| 219 | `CUserLocal::TryDoingMagicAttack` | `MAGIC_ATTACK` | 46 | 证据 | 魔法攻击（push 0DBh @0x92dc52） |
| 220 | `CWvsContext::AskWhetherUsePamsSong` | — | — | 待核 | PamsSong 询问 |
| 221 | `CUIQuestInfoDetail::OnButtonClicked` | — | — | 待核 | 任务详情按钮 |
| 222 | `CUserLocal::SendRepeatEffectRemoveRequest` | — | — | 待核 | 重复特效移除 |

### 3.8 怪物 / NPC / 反应堆 / Field

| v95 | v95 发送函数（首个） | s9 RecvOpcode | 值 | 置信 | 备注 |
|---|---|---|---|---|---|
| 227 | `CMob::GenerateMovePath` | `MOVE_LIFE` | 188 | 证据 | 怪物移动路径 |
| 228 | `CMob::ApplyControl` | `AUTO_AGGRO` | 189 | 待核 | 控制权接受 |
| 229 | `CMob::SendDropPickUpRequest` | — | — | 待核 | 怪物拾取 |
| 230 | `CMob::Update` | — | — | 待核 | 怪物状态同步 A（CMob::Update） |
| 231 | `CMob::Update` | — | — | 待核 | 怪物状态同步 B（CMob::Update） |
| 232 | `CMob::TryFirstSelfDestruction` | `MONSTER_BOMB` | 193 | 证据 | 自爆 |
| 233 | `CMob::SetDamagedByMob` | `MOB_DAMAGE_MOB` | 194 | 证据 | 怪打怪 |
| 234 | `CMob::Update` | — | — | 待核 | 怪物状态同步 C（CMob::Update） |
| 235 | `CMob::UpdateTimeBomb` | `MONSTER_BOMB` | 193 | 证据 | 倒计时炸弹 |
| 236 | `CMob::SendCollisionEscort` | — | — | 待核 | 护卫碰撞 |
| 237 | `CMob::SendRequestEscortPath` | — | — | 待核 | 护卫路径请求 |
| 238 | `CMob::SendEscortStopEndRequest` | — | — | 待核 | 护卫停止 |
| 241 | `CNpc::GenerateMovePath` | `NPC_ACTION` | 197 | 证据 | NPC 移动 |
| 242 | `CNpc::RequestSpecialAction` | `NPC_ACTION` | 197 | 待核 | NPC 特殊动作 |
| 246 | `CWvsContext::SendDropPickUpRequest` | `ITEM_PICKUP` | 202 | 证据 | 拾取 |
| 249 | `CReactorPool::FindHitReactor`（共 2 个函数） | `DAMAGE_REACTOR` | 205 | 证据 | 反应堆受击 |
| 250 | `CReactorPool::FindTouchReactorAroundLocalUser` | `TOUCHING_REACTOR` | 206 | 证据 | 反应堆触碰 |
| 251 | `CUserLocal::ResetNLCPQ` | `MONSTER_CARNIVAL` | 218 | 待核 | NLC PQ 重置 |
| 255 | `CField_SnowBall::BasicActionAttack` | `SNOWBALL` | 211 | 证据 | 雪球 |
| 256 | `CField_SnowBall::Update` | `SNOWBALL` | 211 | 待核 | 雪球状态 |
| 257 | `CField_Coconut::BasicActionAttack` | `COCONUT` | 213 | 证据 | 椰子 |
| 258 | `CField::SendChatMsgSlash` | `ADMIN_CHAT` | 118 | 待核 | 斜杠命令 |
| 259 | `CField_GuildBoss::BasicActionAttack` | — | — | 无对应 | 公会 Boss（s9 收包无） |
| 262 | `CUIMonsterCarnival::RequestSend` | `MONSTER_CARNIVAL` | 218 | 证据 | 怪物狂欢 |
| 264 | `CField_ContiMove::Init` | — | — | 待核 | 连续移动场（ContiMove） |
| 266 | `CWvsContext::SendPartyWanted` | `PARTY_SEARCH_REGISTER` | 220 | 待核 | 组队招聘 |
| 267 | `CWvsContext::SendCancelPartyWanted` | `PARTY_SEARCH_UPDATE` | 223 | 待核 | 取消招聘 |
| 269 | `CStage::OnSetField` | — | — | 待核 | 阶段切换 |
| 270 | `CField::OnRequestFootHoldInfo` | — | — | 待核 | FootHold 信息请求 |

### 3.9 仅 s9 存在（v95 无对应，移植时为本项目自定义）

以下 s9 收包在 v95 发送表中找不到同义项。协议 v2 里它们要么保留 s9 原值，要么按 v95 风格合并：

`GUEST_LOGIN(2)`、`SERVERLIST_REREQUEST(4)`、`CHARLIST_REQUEST(5)`、`PLAYER_DC(12)`、`PLAYER_LOGGEDIN(20)`、
`CLIENT_START_ERROR(25)`、`CLIENT_ERROR(26)`、`STRANGE_DATA(27)`、`RELOG(28)`、`REGISTER_PIC(29)`、
`TOUCH_MONSTER_ATTACK(47)`、`TAKE_DAMAGE(48)`、`CLOSE_CHALKBOARD(50)`、`MONSTER_BOOK_COVER(57)`、
`USE_TELEPORT_ROCK(84)`（v95 用发送 91）、`REPORT(106)`、`ADMIN_LOG(129)`、`USE_SOLOMON_ITEM(157)`、
`CLICK_GUIDE(162)`、`BEHOLDER(178)`、`FIELD_DAMAGE_MOB(191)`、`MOB_DAMAGE_MOB_FRIENDLY(192)`、
`PLAYER_MAP_TRANSFER(207)`、`LEFT_KNOCKBACK(212)`、`MATCH_TABLE(214)`、`USE_MAPLELIFE(256)`、`USE_HAMMER(260)`、
`SET_HPMPALERT(4096)`、`CUSTOM_PACKET(14099)`、`MAPLETV(65534)`

## 4. v95 接收 → s9 发包（`SendOpcode`）

### 4.1 `CWvsContext` 段（28–140，全量）

| v95 | 处理函数 | s9 SendOpcode | 值 | 置信 | 备注 |
|---|---|---|---|---|---|
| 28 | `CWvsContext::OnInventoryOperation` | `INVENTORY_OPERATION` | 29 | 证据 |  |
| 29 | `CWvsContext::OnInventoryGrow` | `INVENTORY_GROW` | 30 | 证据 |  |
| 30 | `CWvsContext::OnStatChanged` | `STAT_CHANGED` | 31 | 证据 |  |
| 31 | `CWvsContext::OnTemporaryStatSet` | `GIVE_BUFF` | 32 | 证据 |  |
| 32 | `CWvsContext::OnTemporaryStatReset` | `CANCEL_BUFF` | 33 | 证据 |  |
| 33 | `CWvsContext::OnForcedStatSet` | `FORCED_STAT_SET` | 34 | 证据 |  |
| 34 | `CWvsContext::OnForcedStatReset` | `FORCED_STAT_RESET` | 35 | 证据 |  |
| 35 | `CWvsContext::OnChangeSkillRecordResult` | `UPDATE_SKILLS` | 36 | 证据 |  |
| 36 | `CWvsContext::OnSkillUseResult` | `SKILL_USE_RESULT` | 37 | 证据 |  |
| 37 | `CWvsContext::OnGivePopularityResult` | `FAME_RESPONSE` | 38 | 证据 |  |
| 38 | `CWvsContext::OnMessage` | `SHOW_STATUS_INFO` | 39 | 证据 |  |
| 39 | `CWvsContext::OnOpenFullClientDownloadLink` | `OPEN_FULL_CLIENT_DOWNLOAD_LINK` | 40 | 证据 |  |
| 40 | `CWvsContext::OnMemoResult` | `MEMO_RESULT` | 41 | 证据 |  |
| 41 | `CWvsContext::OnMapTransferResult` | `MAP_TRANSFER_RESULT` | 42 | 证据 |  |
| 42 | `CWvsContext::OnAntiMacroResult` | — | — | 无对应 | 反外挂结果，s9 无独立包 |
| 44 | `CWvsContext::OnClaimResult` | `CLAIM_RESULT` | 45 | 证据 |  |
| 45 | `CWvsContext::OnSetClaimSvrAvailableTime` | `CLAIM_AVAILABLE_TIME` | 46 | 证据 |  |
| 46 | `CWvsContext::OnClaimSvrStatusChanged` | `CLAIM_STATUS_CHANGED` | 47 | 证据 |  |
| 47 | `CWvsContext::OnSetTamingMobInfo` | `SET_TAMING_MOB_INFO` | 48 | 证据 |  |
| 48 | `CWvsContext::OnQuestClear` | `QUEST_CLEAR` | 49 | 证据 | 注意不是 SET_QUEST_CLEAR(150) |
| 49 | `CWvsContext::OnEntrustedShopCheckResult` | `ENTRUSTED_SHOP_CHECK_RESULT` | 50 | 证据 |  |
| 50 | `CWvsContext::OnSkillLearnItemResult` | `SKILL_LEARN_ITEM_RESULT` | 51 | 证据 |  |
| 51 | `CWvsContext::OnSkillResetItemResult` | — | — | 待核 | 技能重置结果 |
| 52 | `CWvsContext::OnGatherItemResult` | `GATHER_ITEM_RESULT` | 52 | 证据 |  |
| 53 | `CWvsContext::OnSortItemResult` | `SORT_ITEM_RESULT` | 53 | 证据 |  |
| 55 | `CWvsContext::OnSueCharacterResult` | `SUE_CHARACTER_RESULT` | 55 | 证据 |  |
| 57 | `CWvsContext::OnTradeMoneyLimit` | `TRADE_MONEY_LIMIT` | 57 | 证据 |  |
| 58 | `CWvsContext::OnSetGender` | `SET_GENDER` | 58 | 证据 |  |
| 59 | `CWvsContext::OnGuildBBSPacket` | `GUILD_BBS_PACKET` | 59 | 证据 |  |
| 61 | `CWvsContext::OnCharacterInfo` | `CHAR_INFO` | 61 | 证据 |  |
| 62 | `CWvsContext::OnPartyResult` | `PARTY_OPERATION` | 62 | 证据 |  |
| 64 | `CWvsContext::OnExpedtionResult` | — | — | 无对应 | 远征队结果，s9 无 |
| 65 | `CWvsContext::OnFriendResult` | `BUDDYLIST` | 63 | 证据 |  |
| 67 | `CWvsContext::OnGuildResult` | `GUILD_OPERATION` | 65 | 证据 |  |
| 68 | `CWvsContext::OnAllianceResult` | `ALLIANCE_OPERATION` | 66 | 证据 |  |
| 69 | `CWvsContext::OnTownPortal` | `SPAWN_PORTAL` | 67 | 证据 |  |
| 70 | `CWvsContext::OnOpenGate` | — | — | 待核 | 开洞传送门 |
| 71 | `CWvsContext::OnBroadcastMsg` | `SERVERMESSAGE` | 68 | 待核 | 系统广播 |
| 72 | `CWvsContext::OnIncubatorResult` | `INCUBATOR_RESULT` | 69 | 证据 |  |
| 73 | `CWvsContext::OnShopScannerResult` | `SHOP_SCANNER_RESULT` | 70 | 证据 |  |
| 74 | `CWvsContext::OnShopLinkResult` | `SHOP_LINK_RESULT` | 71 | 证据 |  |
| 75 | `CWvsContext::OnMarriageRequest` | `MARRIAGE_REQUEST` | 72 | 证据 |  |
| 76 | `CWvsContext::OnMarriageResult` | `MARRIAGE_RESULT` | 73 | 证据 |  |
| 77 | `CWvsContext::OnWeddingGiftResult` | `WEDDING_GIFT_RESULT` | 74 | 证据 |  |
| 78 | `CWvsContext::OnNotifyMarriedPartnerMapTransfer` | `NOTIFY_MARRIED_PARTNER_MAP_TRANSFER` | 75 | 证据 |  |
| 79 | `CWvsContext::OnCashPetFoodResult` | `CASH_PET_FOOD_RESULT` | 76 | 证据 |  |
| 80 | `CWvsContext::OnSetWeekEventMessage` | `SET_WEEK_EVENT_MESSAGE` | 77 | 证据 |  |
| 81 | `CWvsContext::OnSetPotionDiscountRate` | `SET_POTION_DISCOUNT_RATE` | 78 | 证据 |  |
| 82 | `CWvsContext::OnBridleMobCatchFail` | `BRIDLE_MOB_CATCH_FAIL` | 79 | 证据 |  |
| 83 | `CWvsContext::OnImitatedNPCResult` | `IMITATED_NPC_RESULT` | 80 | 证据 |  |
| 84 | `CNpcPool::OnNpcImitateData` | `IMITATED_NPC_DATA` | 81 | 证据 |  |
| 85 | `CNpcPool::OnUpdateLimitedDisableInfo` | `LIMITED_NPC_DISABLE_INFO` | 82 | 证据 |  |
| 86 | `CWvsContext::OnMonsterBookSetCard` | `MONSTER_BOOK_SET_CARD` | 83 | 证据 |  |
| 87 | `CWvsContext::OnMonsterBookSetCover` | `MONSTER_BOOK_SET_COVER` | 84 | 证据 |  |
| 88 | `CWvsContext::OnHourChanged` | `HOUR_CHANGED` | 85 | 证据 |  |
| 89 | `CWvsContext::OnMiniMapOnOff` | `MINIMAP_ON_OFF` | 86 | 证据 |  |
| 90 | `CWvsContext::OnConsultAuthkeyUpdate` | `CONSULT_AUTHKEY_UPDATE` | 87 | 证据 |  |
| 91 | `CWvsContext::OnClassCompetitionAuthkeyUpdate` | `CLASS_COMPETITION_AUTHKEY_UPDATE` | 88 | 证据 |  |
| 92 | `CWvsContext::OnWebBoardAuthkeyUpdate` | `WEB_BOARD_AUTHKEY_UPDATE` | 89 | 证据 |  |
| 93 | `CWvsContext::OnSessionValue` | `SESSION_VALUE` | 90 | 证据 |  |
| 94 | `CWvsContext::OnPartyValue` | `PARTY_VALUE` | 91 | 证据 |  |
| 95 | `CWvsContext::OnFieldSetVariable` | `FIELD_SET_VARIABLE` | 92 | 证据 |  |
| 96 | `CWvsContext::OnBonusExpRateChanged` | `BONUS_EXP_CHANGED` | 93 | 证据 |  |
| 97 | `CWvsContext::OnPotionDiscountRateChanged` | — | — | 待核 | 药水折扣变更 |
| 98 | `CWvsContext::OnFamilyChartResult` | `FAMILY_CHART_RESULT` | 94 | 证据 |  |
| 99 | `CWvsContext::OnFamilyInfoResult` | `FAMILY_INFO_RESULT` | 95 | 证据 |  |
| 100 | `CWvsContext::OnFamilyResult` | `FAMILY_RESULT` | 96 | 证据 |  |
| 101 | `CWvsContext::OnFamilyJoinRequest` | `FAMILY_JOIN_REQUEST` | 97 | 证据 |  |
| 102 | `CWvsContext::OnFamilyJoinRequestResult` | `FAMILY_JOIN_REQUEST_RESULT` | 98 | 证据 |  |
| 103 | `CWvsContext::OnFamilyJoinAccepted` | `FAMILY_JOIN_ACCEPTED` | 99 | 证据 |  |
| 104 | `CWvsContext::OnFamilyPrivilegeList` | `FAMILY_PRIVILEGE_LIST` | 100 | 证据 |  |
| 105 | `CWvsContext::OnFamilyFamousPointIncResult` | `FAMILY_REP_GAIN` | 101 | 证据 |  |
| 106 | `CWvsContext::OnFamilyNotifyLoginOrLogout` | `FAMILY_NOTIFY_LOGIN_OR_LOGOUT` | 102 | 证据 |  |
| 107 | `CWvsContext::OnFamilySetPrivilege` | `FAMILY_SET_PRIVILEGE` | 103 | 证据 |  |
| 108 | `CWvsContext::OnFamilySummonRequest` | `FAMILY_SUMMON_REQUEST` | 104 | 证据 |  |
| 109 | `CWvsContext::OnNotifyLevelUp` | `NOTIFY_LEVELUP` | 105 | 证据 |  |
| 110 | `CWvsContext::OnNotifyWedding` | `NOTIFY_MARRIAGE` | 106 | 证据 |  |
| 111 | `CWvsContext::OnNotifyJobChange` | `NOTIFY_JOB_CHANGE` | 107 | 证据 |  |
| 113 | `CWvsContext::OnMapleTVUseRes` | `MAPLE_TV_USE_RES` | 109 | 证据 |  |
| 114 | `CWvsContext::OnAvatarMegaphoneRes` | `AVATAR_MEGAPHONE_RESULT` | 110 | 证据 |  |
| 115 | `CWvsContext::OnSetAvatarMegaphone` | `SET_AVATAR_MEGAPHONE` | 111 | 证据 |  |
| 116 | `CWvsContext::OnClearAvatarMegaphone` | `CLEAR_AVATAR_MEGAPHONE` | 112 | 证据 |  |
| 117 | `CWvsContext::OnCancelNameChangeResult` | `CANCEL_NAME_CHANGE_RESULT` | 113 | 证据 |  |
| 118 | `CWvsContext::OnCancelTransferWorldResult` | `CANCEL_TRANSFER_WORLD_RESULT` | 114 | 证据 |  |
| 119 | `CWvsContext::OnDestroyShopResult` | `DESTROY_SHOP_RESULT` | 115 | 证据 |  |
| 120 | `CWvsContext::OnFakeGMNotice` | `FAKE_GM_NOTICE` | 116 | 证据 |  |
| 121 | `CWvsContext::OnSuccessInUsegachaponBox` | `SUCCESS_IN_USE_GACHAPON_BOX` | 117 | 证据 |  |
| 122 | `CWvsContext::OnNewYearCardRes` | `NEW_YEAR_CARD_RES` | 118 | 证据 |  |
| 123 | `CWvsContext::OnRandomMorphRes` | `RANDOM_MORPH_RES` | 119 | 证据 |  |
| 124 | `CWvsContext::OnCancelNameChangebyOther` | `CANCEL_NAME_CHANGE_BY_OTHER` | 120 | 证据 |  |
| 125 | `CWvsContext::OnSetBuyEquipExt` | `SET_EXTRA_PENDANT_SLOT` | 121 | 待核 | 额外吊坠槽 |
| 126 | `CWvsContext::OnSetPassenserRequest` | — | — | 待核 | 同乘请求 |
| 127 | `CWvsContext::OnScriptProgressMessage` | `SCRIPT_PROGRESS_MESSAGE` | 122 | 证据 |  |
| 128 | `CWvsContext::OnDataCRCCheckFailed` | `DATA_CRC_CHECK_FAILED` | 123 | 证据 |  |
| 129 | `CWvsContext::OnCakePieEventResult` | — | — | 待核 | CakePie 活动结果 |
| 130 | `CWvsContext::OnUpdateGMBoard` | — | — | 待核 | GM 公告板更新 |
| 131 | `CWvsContext::OnShowSlotMessage` | — | — | 待核 | 槽位消息 |
| 132 | `CWvsContext::OnWildHunterInfo` | — | — | 待核 | 狂豹猎人信息 |
| 133 | `CWvsContext::OnAccountMoreInfo` | — | — | 待核 | 账号附加信息 |
| 134 | `CWvsContext::OnFindFirend` | — | — | 待核 | 好友搜索 |
| 135 | `CWvsContext::OnStageChange` | — | — | 待核 | 阶段切换 |
| 136 | `CWvsContext::OnDragonBallBox` | — | — | 待核 | 龙珠盒 |
| 137 | `CWvsContext::OnAskWhetherUsePamsSong` | — | — | 待核 | PamsSong 询问 |
| 138 | `CWvsContext::OnTransferChannel` | `CHANGE_CHANNEL` | 16 | 待核 | 换频道结果 |
| 139 | `CWvsContext::OnDisallowedDeliveryQuestList` | — | — | 待核 | 禁止投递任务列表 |
| 140 | `CWvsContext::OnMacroSysDataInit` | `MACRO_SYS_DATA_INIT` | 124 | 证据 |  |

### 4.2 `CField` / 池 / 对话框段（141–432，锚点）

未列出的 opcode 为 v95 无处理或尚未取证的子码（完整数值见 `notes/opcode-v95.md` §3–§4）。

#### 4.2.1 地图加载 / 场景 / 时钟（144–177）

| v95 | 处理函数 | s9 SendOpcode | 值 | 置信 | 备注 |
|---|---|---|---|---|---|
| 144 | `CMapLoadable::OnSetBackEffect` | `SET_BACK_EFFECT` | 128 | 证据 | CMapLoadable |
| 145 | `CMapLoadable::OnSetMapObjectVisible` | `SET_MAP_OBJECT_VISIBLE` | 129 | 证据 | CMapLoadable |
| 146 | `CMapLoadable::OnClearBackEffect` | `CLEAR_BACK_EFFECT` | 130 | 证据 | CMapLoadable |
| 147 | `CField::OnTransferFieldReqIgnored` | `BLOCKED_MAP` | 131 | 待核 | 转移请求被忽略 |
| 148 | `CField::OnTransferChannelReqIgnored` | `BLOCKED_SERVER` | 132 | 待核 | 换线请求被忽略 |
| 149 | `CField::OnFieldSpecificData` | — | — | 待核 | 地图私有数据 |
| 150 | `CField::OnGroupMessage` | `MULTICHAT` | 134 | 证据 | 组消息 |
| 151 | `CField::OnWhisper` | `WHISPER` | 135 | 证据 |  |
| 152 | `CField::OnCoupleMessage` | `SPOUSE_CHAT` | 136 | 证据 |  |
| 153 | `CField::OnSummonItemInavailable` | `SUMMON_ITEM_INAVAILABLE` | 137 | 证据 |  |
| 154 | `CField::OnFieldEffect` | `FIELD_EFFECT` | 138 | 证据 |  |
| 155 | `CField::OnFieldObstacleOnOff` | `FIELD_OBSTACLE_ONOFF` | 139 | 证据 |  |
| 156 | `CField::OnFieldObstacleOnOffStatus` | `FIELD_OBSTACLE_ONOFF_LIST` | 140 | 证据 |  |
| 157 | `CField::OnFieldObstacleAllReset` | `FIELD_OBSTACLE_ALL_RESET` | 141 | 证据 |  |
| 158 | `CField::OnBlowWeather` | `BLOW_WEATHER` | 142 | 证据 |  |
| 159 | `CField::OnPlayJukeBox` | `PLAY_JUKEBOX` | 143 | 证据 |  |
| 160 | `CField::OnAdminResult` | `ADMIN_RESULT` | 144 | 证据 |  |
| 161 | `CField::OnQuiz` | `OX_QUIZ` | 145 | 证据 |  |
| 162 | `CField::OnDesc` | `GMEVENT_INSTRUCTIONS` | 146 | 证据 | 警告信息 |
| 163 | `→ vtable 虚调用（本次确认为 CField::OnClock）` | `CLOCK` | 147 | 证据 | 子码 0-3 |
| 166 | `CField::OnSetQuestClear` | `SET_QUEST_CLEAR` | 150 | 证据 |  |
| 167 | `CField::OnSetQuestTime` | `SET_QUEST_TIME` | 151 | 证据 |  |
| 168 | `CField::OnWarnMessage` | `ARIANT_RESULT` | 152 | 待核 | 警告信息 |
| 169 | `CField::OnSetObjectState` | `SET_OBJECT_STATE` | 153 | 证据 |  |
| 170 | `CField::OnDestroyClock` | `STOP_CLOCK` | 154 | 证据 |  |
| 172 | `CField::OnStalkResult` | — | — | 待核 | StalkResult |
| 175 | `CQuickslotKeyMappedMan::OnInit` | `QUICKSLOT_INIT` | 159 | 证据 |  |
| 176 | `CField::OnFootHoldInfo` | — | — | 待核 | FootHoldInfo |
| 177 | `CField::OnRequestFootHoldInfo` | — | — | 待核 | RequestFootHoldInfo |

#### 4.2.2 角色进入 / 普通动作（179–233）

| v95 | 处理函数 | s9 SendOpcode | 值 | 置信 | 备注 |
|---|---|---|---|---|---|
| 179 | `CUserPool::OnUserEnterField` | `SPAWN_PLAYER` | 160 | 证据 |  |
| 180 | `CUserPool::OnUserLeaveField` | `REMOVE_PLAYER_FROM_MAP` | 161 | 证据 |  |
| 181 | `CUser::OnChat(bIsFromOutsideOfMap=0)` | `CHATTEXT` | 162 | 证据 |  |
| 182 | `CUser::OnChat(bIsFromOutsideOfMap=1)` | `CHATTEXT` | 162 | 待核 | 地图外聊天 |
| 183 | `CUser::OnADBoard` | `CHALKBOARD` | 164 | 证据 |  |
| 184 | `CUser::OnMiniRoomBalloon` | `UPDATE_CHAR_BOX` | 165 | 证据 |  |
| 185 | `CUser::SetConsumeItemEffect` | `SHOW_CONSUME_EFFECT` | 166 | 证据 |  |
| 186 | `CUser::ShowItemUpgradeEffect` | `SHOW_SCROLL_EFFECT` | 167 | 证据 |  |
| 187 | `CUser::ShowItemHyperUpgradeEffect` | — | — | 待核 | 超级强化特效 |
| 188 | `CUser::ShowItemOptionUpgradeEffect` | — | — | 待核 | 潜能强化特效 |
| 189 | `CUser::ShowItemReleaseEffect` | — | — | 待核 | 解封特效 |
| 190 | `CUser::ShowItemUnreleaseEffect` | — | — | 待核 | 解除解封特效 |
| 191 | `CUser::OnHitByUser` | — | — | 待核 | 被玩家攻击 |
| 192 | `CUser::OnTeslaTriangle` | — | — | 待核 | 特斯拉三角 |
| 193 | `CUser::OnFollowCharacter` | — | — | 待核 | 跟随角色 |
| 194 | `CUser::OnShowPQReward` | — | — | 待核 | PQ 奖励展示 |
| 195 | `CUser::OnSetPhase` | — | — | 待核 | 阶段设置 |
| 197 | `CUser::ShowRecoverUpgradeCountEffect` | — | — | 待核 | 恢复强化次数特效 |
| 198 | `CUserPool::OnUserCommonPacket` | `SPAWN_PET` | 168 | 证据 |  |
| 199 | `CUserPool::OnUserCommonPacket` | `MOVE_PET` | 170 | 证据 |  |
| 200 | `CUserPool::OnUserCommonPacket` | `PET_CHAT` | 171 | 证据 |  |
| 201 | `CUserPool::OnUserCommonPacket` | `PET_NAMECHANGE` | 172 | 证据 |  |
| 202 | `CUserPool::OnUserCommonPacket` | `PET_EXCEPTION_LIST` | 173 | 证据 |  |
| 203 | `CUserPool::OnUserCommonPacket` | `PET_COMMAND` | 174 | 证据 |  |
| 204 | `CUserPool::OnUserCommonPacket` | — | — | 待核 | 宠物包 7 |
| 205 | `CUserPool::OnUserCommonPacket` | — | — | 待核 | 宠物包 8 |
| 206 | `CUserPool::OnUserCommonPacket` | `SPAWN_DRAGON` | 181 | 待核 | CUser::OnDragonPacket |
| 207 | `CUserPool::OnUserCommonPacket` | `MOVE_DRAGON` | 182 | 待核 |  |
| 208 | `CUserPool::OnUserCommonPacket` | `REMOVE_DRAGON` | 183 | 待核 |  |
| 210 | `CUserRemote::OnMove` | `MOVE_PLAYER` | 185 | 证据 |  |
| 214 | `CUserRemote::OnAttack` | `CLOSE_RANGE_ATTACK / RANGED_ATTACK / MAGIC_ATTACK` | — | 证据 | 一个 OnAttack(nType) 覆盖 186/187/188 |
| 215 | `CUserRemote::OnSkillPrepare` | — | — | 待核 | 技能预备 |
| 216 | `CUserRemote::OnMovingShootAttackPrepare` | — | — | 待核 | 移动射击预备 |
| 217 | `CUserRemote::OnSkillCancel` | `CANCEL_SKILL_EFFECT` | 191 | 证据 |  |
| 218 | `CUserRemote::OnHit` | `DAMAGE_PLAYER` | 192 | 证据 |  |
| 219 | `CUser::OnEmotion` | `FACIAL_EXPRESSION` | 193 | 证据 |  |
| 220 | `CUserRemote::OnSetActiveEffectItem` | `SHOW_ITEM_EFFECT` | 194 | 证据 |  |
| 221 | `CUserRemote::OnShowUpgradeTombEffect` | — | — | 待核 | 墓碑升级特效 |
| 222 | `CUserRemote::OnSetActivePortableChair` | `SHOW_CHAIR` | 196 | 证据 |  |
| 223 | `CUserRemote::OnAvatarModified` | `UPDATE_CHAR_LOOK` | 197 | 证据 |  |
| 224 | `CUser::OnEffect` | `SHOW_FOREIGN_EFFECT` | 198 | 证据 |  |
| 225 | `CUserRemote::OnSetTemporaryStat` | `GIVE_FOREIGN_BUFF` | 199 | 证据 |  |
| 226 | `CUserRemote::OnResetTemporaryStat` | `CANCEL_FOREIGN_BUFF` | 200 | 证据 |  |
| 227 | `CUserRemote::OnReceiveHP` | `UPDATE_PARTYMEMBER_HP` | 201 | 证据 |  |
| 228 | `CUserRemote::OnGuildNameChanged` | `GUILD_NAME_CHANGED` | 202 | 证据 |  |
| 229 | `CUserRemote::OnGuildMarkChanged` | `GUILD_MARK_CHANGED` | 203 | 证据 |  |
| 230 | `CUserRemote::OnThrowGrenade` | `THROW_GRENADE` | 204 | 证据 |  |
| 231 | `CUserLocal::OnSitResult` | `SHOW_CHAIR / CANCEL_CHAIR` | — | 待核 | 坐下结果 |
| 232 | `CUser::OnEmotion` | `FACIAL_EXPRESSION` | 193 | 证据 |  |
| 233 | `CUser::OnEffect` | `SHOW_FOREIGN_EFFECT` | 198 | 证据 |  |

#### 4.2.3 本地角色 / 任务 / UI（234–276）

| v95 | 处理函数 | s9 SendOpcode | 值 | 置信 | 备注 |
|---|---|---|---|---|---|
| 234 | `CUserLocal::OnTeleport` | — | — | 待核 | 瞬移 |
| 236 | `CUserLocal::OnMesoGive_Succeeded` | `MESO_BAG_MESSAGE` | 210 | 待核 | 给金币成功 |
| 237 | `CUserLocal::OnMesoGive_Failed` | `MESO_BAG_MESSAGE` | 210 | 待核 | 给金币失败 |
| 240 | `CUserLocal::OnFieldFadeInOut` | — | — | 待核 | 渐隐 |
| 241 | `CUserLocal::OnFieldFadeOutForce` | — | — | 待核 | 强制渐隐 |
| 242 | `CUserLocal::OnQuestResult` | `UPDATE_QUEST_INFO` | 211 | 证据 | 任务结果 |
| 243 | `CUserLocal::OnNotifyHPDecByField` | `ON_NOTIFY_HP_DEC_BY_FIELD` | 212 | 证据 |  |
| 245 | `CUserLocal::OnBalloonMsg` | `PLAYER_HINT` | 214 | 待核 | 气泡提示 |
| 248 | `CUserLocal::OnMakerResult` | `MAKER_RESULT` | 217 | 证据 | 制造结果 |
| 251 | `CUserLocal::OnOpenUI` | `OPEN_UI` | 220 | 证据 |  |
| 252 | `CUserLocal::OnOpenUIWithOption` | `OPEN_UI` | 220 | 待核 | 带选项开 UI |
| 253 | `CUserLocal::OnSetDirectionMode` | `LOCK_UI` | 221 | 待核 | 方向模式 |
| 254 | `CUserLocal::OnSetStandAloneMode` | `LOCK_UI` | 221 | 待核 | 独立模式 |
| 255 | `CUserLocal::OnHireTutor` | `SPAWN_GUIDE` | 223 | 待核 | 雇佣导师 |
| 256 | `CUserLocal::OnTutorMsg` | `TALK_GUIDE` | 224 | 待核 | 导师消息 |
| 257 | `CUserLocal::OnIncComboResponse` | `SHOW_COMBO` | 225 | 证据 |  |
| 263 | `CUserLocal::OnNoticeMsg` | — | — | 待核 | 公告消息 |
| 264 | `CUserLocal::OnChatMsg` | — | — | 待核 | 聊天消息 |
| 267 | `CUserLocal::OnDamageMeter` | — | — | 待核 | 伤害计量 |
| 276 | `CUserLocal::OnSkillCooltimeSet` | `COOLDOWN` | 234 | 证据 |  |

#### 4.2.4 怪物 / NPC（284–318）

| v95 | 处理函数 | s9 SendOpcode | 值 | 置信 | 备注 |
|---|---|---|---|---|---|
| 284 | `CMobPool::OnMobEnterField` | `SPAWN_MONSTER` | 236 | 证据 |  |
| 285 | `CMobPool::OnMobLeaveField` | `KILL_MONSTER` | 237 | 证据 |  |
| 286 | `CMobPool::OnMobChangeController` | `SPAWN_MONSTER_CONTROL` | 238 | 证据 |  |
| 287 | `CMob::OnMove` | `MOVE_MONSTER` | 239 | 证据 |  |
| 288 | `CMob::OnCtrlAck` | `MOVE_MONSTER_RESPONSE` | 240 | 证据 |  |
| 290 | `CMob::OnStatSet` | `APPLY_MONSTER_STATUS` | 242 | 证据 |  |
| 291 | `CMob::OnStatReset` | `CANCEL_MONSTER_STATUS` | 243 | 证据 |  |
| 292 | `CMob::OnSuspendReset` | `RESET_MONSTER_ANIMATION` | 244 | 证据 |  |
| 294 | `CMob::OnDamaged` | `DAMAGE_MONSTER` | 246 | 证据 |  |
| 298 | `CMob::OnHPIndicator` | `SHOW_MONSTER_HP` | 250 | 证据 |  |
| 304 | `CMob::OnEscortFullPath` | `NPC_TALK` | 304 | 证据 |  |
| 305 | `CMob::OnEscortStopEndPermmision` | `OPEN_NPC_SHOP` | 305 | 证据 |  |
| 306 | `CMob::OnEscortStopSay` | `CONFIRM_SHOP_TRANSACTION` | 306 | 证据 |  |
| 308 | `CMob::OnNextAttack` | `ADMIN_SHOP` | 308 | 证据 |  |
| 309 | `CMob::OnMobAttackedByMob` | `STORAGE` | 309 | 证据 |  |
| 311 | `CNpcPool::OnNpcEnterField` | `SPAWN_NPC` | 257 | 证据 |  |
| 312 | `CNpcPool::OnNpcLeaveField` | `REMOVE_NPC` | 258 | 证据 |  |
| 313 | `CNpcPool::OnNpcChangeController` | `SPAWN_NPC_REQUEST_CONTROLLER` | 259 | 证据 |  |
| 314 | `CNpc::OnMove` | `NPC_ACTION` | 260 | 证据 |  |

#### 4.2.5 雇佣商店 / 掉落 / 反应堆（319–337）

| v95 | 处理函数 | s9 SendOpcode | 值 | 置信 | 备注 |
|---|---|---|---|---|---|
| 319 | `CEmployeePool::OnEmployeeEnterField` | `SPAWN_HIRED_MERCHANT` | 265 | 证据 |  |
| 320 | `CEmployeePool::OnEmployeeLeaveField` | `DESTROY_HIRED_MERCHANT` | 266 | 证据 |  |
| 321 | `CEmployeePool::OnEmployeeMiniRoomBalloon` | `UPDATE_HIRED_MERCHANT` | 267 | 证据 |  |
| 322 | `CDropPool::OnDropEnterField` | `DROP_ITEM_FROM_MAPOBJECT` | 268 | 证据 |  |
| 323 | `CDropPool::OnPacket` | `REMOVE_ITEM_FROM_MAP` | 269 | 证据 |  |
| 325 | `CMessageBoxPool::OnCreateFailed` | `SPAWN_MIST` | 273 | 待核 | 消息框创建失败 |
| 326 | `CMessageBoxPool::OnMessageBoxEnterField` | `SPAWN_MIST` | 273 | 待核 | 消息框进入 |
| 327 | `CMessageBoxPool::OnMessageBoxLeaveField` | `REMOVE_MIST` | 274 | 待核 | 消息框离开 |
| 334 | `CReactorPool::OnReactorChangeState` | `REACTOR_HIT` | 277 | 待核 | 反应堆状态变更 |
| 335 | `CReactorPool::OnReactorMove` | — | — | 待核 | 反应堆移动 |
| 336 | `CReactorPool::OnReactorEnterField` | `REACTOR_SPAWN` | 279 | 证据 |  |
| 337 | `CReactorPool::OnReactorLeaveField` | `REACTOR_DESTROY` | 280 | 证据 |  |

#### 4.2.6 Boss 计时 / 对话框（359–373）

| v95 | 处理函数 | s9 SendOpcode | 值 | 置信 | 备注 |
|---|---|---|---|---|---|
| 359 | `CField::OnHontaleTimer` | — | — | 待核 | 龙之巢计时 |
| 360 | `CField::OnChaosZakumTimer` | — | — | 待核 | 混沌扎昆计时 |
| 361 | `CField::OnHontailTimer` | — | — | 待核 | 龙之巢计时 |
| 362 | `CField::OnZakumTimer` | — | — | 待核 | 扎昆计时 |
| 363 | `CScriptMan::OnScriptMessage` | `NPC_TALK` | 304 | 证据 | 脚本消息 |
| 364 | `CShopDlg::OnPacket` | `OPEN_NPC_SHOP` | 305 | 证据 | 商店打开 |
| 365 | `CShopDlg::OnPacket` | `CONFIRM_SHOP_TRANSACTION` | 306 | 证据 | 商店子操作 |
| 366 | `CAdminShopDlg::OnPacket` | `ADMIN_SHOP_MESSAGE` | 307 | 待核 | 管理商店打开 |
| 367 | `CAdminShopDlg::OnPacket` | `ADMIN_SHOP` | 308 | 待核 | 管理商店子操作 |
| 368 | `CTrunkDlg::OnPacket` | `STORAGE` | 309 | 证据 | 仓库子操作 |
| 369 | `CStoreBankDlg::OnPacket` | `FREDRICK_MESSAGE` | 310 | 待核 | 商店银行打开 |
| 370 | `CStoreBankDlg::OnPacket` | `FREDRICK` | 311 | 待核 | 商店银行子操作 |
| 371 | `CRPSGameDlg::OnPacket` | `RPS_GAME` | 312 | 证据 |  |
| 372 | `CUIMessenger::OnPacket` | `MESSENGER` | 313 | 证据 |  |
| 373 | `CMiniRoomBaseDlg::OnPacketBase` | `PLAYER_INTERACTION` | 314 | 证据 | 迷你房基类 |

#### 4.2.7 包裹 / 商城 / TV（381–432）

| v95 | 处理函数 | s9 SendOpcode | 值 | 置信 | 备注 |
|---|---|---|---|---|---|
| 381 | `CParcelDlg::OnPacket` | `PARCEL` | 322 | 证据 |  |
| 382 | `CCashShop::OnChargeParamResult` | `CHARGE_PARAM_RESULT` | 323 | 证据 |  |
| 383 | `CCashShop::OnQueryCashResult` | `QUERY_CASH_RESULT` | 324 | 证据 |  |
| 384 | `CCashShop::OnCashItemResult` | `CASHSHOP_OPERATION` | 325 | 证据 |  |
| 385 | `CCashShop::OnPurchaseExpChanged` | `CASHSHOP_PURCHASE_EXP_CHANGED` | 326 | 待核 |  |
| 386 | `CCashShop::OnGiftMateInfoResult` | `CASHSHOP_GIFT_INFO_RESULT` | 327 | 待核 |  |
| 387 | `CCashShop::OnCheckDuplicatedIDResult` | `CASHSHOP_CHECK_NAME_CHANGE_POSSIBLE_RESULT` | 329 | 待核 |  |
| 388 | `CCashShop::OnCheckNameChangePossibleResult` | `CASHSHOP_CHECK_NAME_CHANGE_POSSIBLE_RESULT` | 329 | 证据 |  |
| 390 | `CCashShop::OnCheckTransferWorldPossibleResult` | `CASHSHOP_CHECK_TRANSFER_WORLD_POSSIBLE_RESULT` | 331 | 证据 |  |
| 391 | `CCashShop::OnCashShopGachaponStampResult` | `CASHSHOP_GACHAPON_STAMP_RESULT` | 332 | 待核 |  |
| 392 | `—` | `CASHSHOP_CASH_ITEM_GACHAPON_RESULT` | 333 | 待核 |  |
| 393 | `CCashShop::OnCashItemGachaponResult` | `CASHSHOP_CASH_ITEM_GACHAPON_RESULT` | 333 | 证据 |  |
| 395 | `CCashShop::OnOneADay` | — | — | 待核 | OneADay |
| 396 | `CCashShop::OnNoticeFreeCashItem` | — | — | 待核 | 免费现金道具通知 |
| 398 | `CFuncKeyMappedMan::OnInit` | `KEYMAP` | 335 | 证据 |  |
| 399 | `CFuncKeyMappedMan::OnPetConsumeItemInit` | `AUTO_HP_POT` | 336 | 证据 |  |
| 400 | `CFuncKeyMappedMan::OnPetConsumeMPItemInit` | `AUTO_MP_POT` | 337 | 证据 |  |
| 405 | `CMapleTVMan::OnSetMessage` | `SEND_TV` | 341 | 待核 |  |
| 406 | `CMapleTVMan::OnClearMessage` | `REMOVE_TV` | 342 | 待核 |  |
| 407 | `CMapleTVMan::OnSendMessageResult` | `ENABLE_TV` | 343 | 待核 |  |
| 421 | `CBattleRecordMan::OnDotDamageInfo` | — | — | 待核 | DOT 伤害信息 |
| 422 | `CBattleRecordMan::OnServerOnCalcRequestResult` | — | — | 待核 | 计算请求结果 |
| 432 | `CWvsContext::OnLogoutGift` | — | — | 待核 | 登出礼物 |

## 5. 本轮补齐与修正

### 5.1 新增的 12 个发送 opcode

旧表漏掉、本轮由 `insn_query` 逐点回推补齐：

| opcode | 构造函数 | 证据（紧邻 ctor 的 push） | 语义 |
|---|---|---|---|
| 20 | `CClientSocket::OnConnect` (0x4aef10) | `push 14h @0x4af458` | 连接建立（`CClientSocket::OnConnect`） |
| 32 | `CClientSocket::OnConnect` (0x4aef10) | `push 20h @0x4af3e3` | 连接建立第二包 |
| 52 | `CUserLocal::SetDamaged` (0x9343c0) | `push 34h` | 受击/延迟上报（`CUserLocal::SetDamaged` / `Update`） |
| 126 | `CField::SendChatMsgSlash` (0x5408e0) | `push 7Eh` | 斜杠聊天（`CField::SendChatMsgSlash`） |
| 139 | `CField::SendChatMsgSlash` (0x5408e0) | `push 8Bh` | 斜杠聊天变体 |
| 152 | `CField::SendChatMsgSlash` (0x5408e0) | `push 98h` | 斜杠聊天变体 |
| 158 | `CField::SendChatMsgSlash` (0x5408e0) | `push 9Eh` | 斜杠聊天变体 |
| 219 | `CUserLocal::TryDoingMagicAttack` (0x92a240) | `push 0DBh` | 魔法攻击（`CUserLocal::TryDoingMagicAttack`） |
| 230 | `CMob::Update` (0x654300) | `push 0E6h` | 怪物状态同步 A（`CMob::Update`） |
| 231 | `CMob::Update` (0x654300) | `push 0E7h` | 怪物状态同步 B（`CMob::Update`） |
| 234 | `CMob::Update` (0x654300) | `push 0EAh` | 怪物状态同步 C（`CMob::Update`） |
| 258 | `CField::SendChatMsgSlash` (0x5408e0) | `push 102h` | 斜杠聊天变体 |

### 5.2 语义修正

| 旧认识 | 修正 | 依据 |
|---|---|---|
| v95 44/45/46 = 近战/远程/魔法攻击 | 44 = 单键动作、45 = 起身、46 = 坐下 | `CUserLocal::OnKey` / `HandleXKeyDown` + `CWvsContext::SendGetUpFromChairRequest` / `SendSitOnPortableChairRequest` |
| v95 103 = 某个主动技能 | 103 = **统一技能入口**（主动技能/召唤/召唤怪/城镇传送/吞噬/磁铁…） | 15 个函数共用：`DoActiveSkill_*` ×10 + `SendSkillUseRequest` / `TryDoingMonsterMagnet` / `TryDoingSwallowAbsorb` / `CGrenade::SendTimeBombInfo` |
| v95 144 = 交易 | 144 = **统一交互**（交易房/个人商店/委托商店/记忆/五子棋/迷你房） | 57 个 Dlg 方法共用（`CTradingRoomDlg`/`CPersonalShopDlg`/`CEntrustedShopDlg`/`COmokDlg`/`CMemoryGameDlg`/`CMiniRoomBaseDlg` …） |
| 219 = UI_Menu | 218 = UI_Menu、**219 = 魔法攻击** | `push 0DBh` @0x92dc52 → ctor @0x92dc5d；该块在 `CUserLocal::TryDoingMagicAttack`（0x92a240，size 0x3c26）内 |
| 259 = 可映射 s9 | v95 GuildBoss 在 s9 收包侧**无对应** | s9 `RecvOpcode` 无 guild boss 项 |

### 5.3 缺口：攻击发送 opcode 未能静态取证

- `CUserLocal::TryDoingCloseRangeAttack` / `TryDoingShootAttack` / 普攻 / 体术的包体构造**内联**在 `0x9237f8` 起始的块里，该块无 COutPacket ctor 调用、无 xref 进入（`0x923600–0x9237f7` 为原始数据），静态无法证明 opcode。
- 已核 `0x91e780`（近战）、`0x925a00`（远程）、`0x9123c0`（普攻）、`0x930710`（体术）四个函数：调用 `Encode*`/`SendPacket` 但**不调用任何 COutPacket 构造**，opcode 来自上层传入或已构造对象。
- 处理：不臆造数值。移植阶段用**动态抓包**（`SendPacket` 断点 / 明文代理）在 095 客户端上实测，再回填本表。
- 连带影响：v95 接收侧攻击是单包 214（`OnAttack(nType)`），s9 侧拆 186/187/188/189；协议 v2 若采用 v95 风格，需要新增子类型字段而不是新 opcode。

## 6. 覆盖度统计

| 方向 | 行数 | 证据 | 待核 | 无对应 |
|---|---|---|---|---|
| v95 发送 → s9 收包 | 238 | 133 | 90 | 15 |
| v95 接收 → s9 发包 | 274 | 188 | 84 | 2 |

v95 发送侧 238 个 opcode 全部覆盖；接收侧覆盖 274/375（其余为 v95 无处理或仅子码，见 `notes/opcode-v95.md` §3–§4）。

## 7. 自动匹配附录

`build_map2.py` 用 Dice 系数对函数名与 s9 枚举名做词元相似度。**仅作交叉检查**：v95 函数名是 mangled 且粒度更粗，机器匹配会给出误配（例：v95 48 `OnQuestClear` → 150 `SET_QUEST_CLEAR` 应为 49 `QUEST_CLEAR`；v95 282 `CSummonedPool::OnSkill` → 37 `SKILL_USE_RESULT` 应为 180 `SUMMON_SKILL`）。

### 7.1 接收侧词元全等（90 条）

| v95 recv | v95 处理函数 | s9 SendOpcode | 值 |
|---|---|---|---|
| 28 | `CWvsContext::OnInventoryOperation` | `INVENTORY_OPERATION` | 29 |
| 29 | `CWvsContext::OnInventoryGrow` | `INVENTORY_GROW` | 30 |
| 30 | `CWvsContext::OnStatChanged` | `STAT_CHANGED` | 31 |
| 33 | `CWvsContext::OnForcedStatSet` | `FORCED_STAT_SET` | 34 |
| 34 | `CWvsContext::OnForcedStatReset` | `FORCED_STAT_RESET` | 35 |
| 36 | `CWvsContext::OnSkillUseResult` | `SKILL_USE_RESULT` | 37 |
| 39 | `CWvsContext::OnOpenFullClientDownloadLink` | `OPEN_FULL_CLIENT_DOWNLOAD_LINK` | 40 |
| 40 | `CWvsContext::OnMemoResult` | `MEMO_RESULT` | 41 |
| 41 | `CWvsContext::OnMapTransferResult` | `MAP_TRANSFER_RESULT` | 42 |
| 44 | `CWvsContext::OnClaimResult` | `CLAIM_RESULT` | 45 |
| 47 | `CWvsContext::OnSetTamingMobInfo` | `SET_TAMING_MOB_INFO` | 48 |
| 48 | `CWvsContext::OnQuestClear` | `SET_QUEST_CLEAR` | 150 |
| 49 | `CWvsContext::OnEntrustedShopCheckResult` | `ENTRUSTED_SHOP_CHECK_RESULT` | 50 |
| 50 | `CWvsContext::OnSkillLearnItemResult` | `SKILL_LEARN_ITEM_RESULT` | 51 |
| 52 | `CWvsContext::OnGatherItemResult` | `GATHER_ITEM_RESULT` | 52 |
| 53 | `CWvsContext::OnSortItemResult` | `SORT_ITEM_RESULT` | 53 |
| 55 | `CWvsContext::OnSueCharacterResult` | `SUE_CHARACTER_RESULT` | 55 |
| 57 | `CWvsContext::OnTradeMoneyLimit` | `TRADE_MONEY_LIMIT` | 57 |
| 58 | `CWvsContext::OnSetGender` | `SET_GENDER` | 58 |
| 59 | `CWvsContext::OnGuildBBSPacket` | `GUILD_BBS_PACKET` | 59 |
| 72 | `CWvsContext::OnIncubatorResult` | `INCUBATOR_RESULT` | 69 |
| 73 | `CWvsContext::OnShopScannerResult` | `SHOP_SCANNER_RESULT` | 70 |
| 74 | `CWvsContext::OnShopLinkResult` | `SHOP_LINK_RESULT` | 71 |
| 75 | `CWvsContext::OnMarriageRequest` | `MARRIAGE_RESULT` | 73 |
| 76 | `CWvsContext::OnMarriageResult` | `MARRIAGE_RESULT` | 73 |
| 77 | `CWvsContext::OnWeddingGiftResult` | `WEDDING_GIFT_RESULT` | 74 |
| 78 | `CWvsContext::OnNotifyMarriedPartnerMapTransfer` | `NOTIFY_MARRIED_PARTNER_MAP_TRANSFER` | 75 |
| 79 | `CWvsContext::OnCashPetFoodResult` | `CASH_PET_FOOD_RESULT` | 76 |
| 80 | `CWvsContext::OnSetWeekEventMessage` | `SET_WEEK_EVENT_MESSAGE` | 77 |
| 81 | `CWvsContext::OnSetPotionDiscountRate` | `SET_POTION_DISCOUNT_RATE` | 78 |
| 82 | `CWvsContext::OnBridleMobCatchFail` | `BRIDLE_MOB_CATCH_FAIL` | 79 |
| 83 | `CWvsContext::OnImitatedNPCResult` | `IMITATED_NPC_RESULT` | 80 |
| 86 | `CWvsContext::OnMonsterBookSetCard` | `MONSTER_BOOK_SET_CARD` | 83 |
| 87 | `CWvsContext::OnMonsterBookSetCover` | `MONSTER_BOOK_SET_COVER` | 84 |
| 88 | `CWvsContext::OnHourChanged` | `HOUR_CHANGED` | 85 |
| 90 | `CWvsContext::OnConsultAuthkeyUpdate` | `CONSULT_AUTHKEY_UPDATE` | 87 |
| 91 | `CWvsContext::OnClassCompetitionAuthkeyUpdate` | `CLASS_COMPETITION_AUTHKEY_UPDATE` | 88 |
| 92 | `CWvsContext::OnWebBoardAuthkeyUpdate` | `WEB_BOARD_AUTHKEY_UPDATE` | 89 |
| 93 | `CWvsContext::OnSessionValue` | `SESSION_VALUE` | 90 |
| 94 | `CWvsContext::OnPartyValue` | `PARTY_VALUE` | 91 |
| 95 | `CWvsContext::OnFieldSetVariable` | `FIELD_SET_VARIABLE` | 92 |
| 98 | `CWvsContext::OnFamilyChartResult` | `FAMILY_CHART_RESULT` | 94 |
| 99 | `CWvsContext::OnFamilyInfoResult` | `FAMILY_INFO_RESULT` | 95 |
| 100 | `CWvsContext::OnFamilyResult` | `FAMILY_RESULT` | 96 |
| 101 | `CWvsContext::OnFamilyJoinRequest` | `FAMILY_JOIN_REQUEST_RESULT` | 98 |
| 102 | `CWvsContext::OnFamilyJoinRequestResult` | `FAMILY_JOIN_REQUEST_RESULT` | 98 |
| 103 | `CWvsContext::OnFamilyJoinAccepted` | `FAMILY_JOIN_ACCEPTED` | 99 |
| 104 | `CWvsContext::OnFamilyPrivilegeList` | `FAMILY_PRIVILEGE_LIST` | 100 |
| 106 | `CWvsContext::OnFamilyNotifyLoginOrLogout` | `FAMILY_NOTIFY_LOGIN_OR_LOGOUT` | 102 |
| 107 | `CWvsContext::OnFamilySetPrivilege` | `FAMILY_SET_PRIVILEGE` | 103 |
| 108 | `CWvsContext::OnFamilySummonRequest` | `FAMILY_SUMMON_REQUEST` | 104 |
| 111 | `CWvsContext::OnNotifyJobChange` | `NOTIFY_JOB_CHANGE` | 107 |
| 113 | `CWvsContext::OnMapleTVUseRes` | `MAPLE_TV_USE_RES` | 109 |
| 114 | `CWvsContext::OnAvatarMegaphoneRes` | `SET_AVATAR_MEGAPHONE` | 111 |
| 115 | `CWvsContext::OnSetAvatarMegaphone` | `SET_AVATAR_MEGAPHONE` | 111 |
| 116 | `CWvsContext::OnClearAvatarMegaphone` | `CLEAR_AVATAR_MEGAPHONE` | 112 |
| 117 | `CWvsContext::OnCancelNameChangeResult` | `CANCEL_NAME_CHANGE_RESULT` | 113 |
| 118 | `CWvsContext::OnCancelTransferWorldResult` | `CANCEL_TRANSFER_WORLD_RESULT` | 114 |
| 119 | `CWvsContext::OnDestroyShopResult` | `DESTROY_SHOP_RESULT` | 115 |
| 120 | `CWvsContext::OnFakeGMNotice` | `FAKE_GM_NOTICE` | 116 |
| 122 | `CWvsContext::OnNewYearCardRes` | `NEW_YEAR_CARD_RES` | 118 |
| 123 | `CWvsContext::OnRandomMorphRes` | `RANDOM_MORPH_RES` | 119 |
| 127 | `CWvsContext::OnScriptProgressMessage` | `SCRIPT_PROGRESS_MESSAGE` | 122 |
| 128 | `CWvsContext::OnDataCRCCheckFailed` | `DATA_CRC_CHECK_FAILED` | 123 |
| 140 | `CWvsContext::OnMacroSysDataInit` | `MACRO_SYS_DATA_INIT` | 124 |
| 144 | `CMapLoadable::OnSetBackEffect` | `SET_BACK_EFFECT` | 128 |
| 145 | `CMapLoadable::OnSetMapObjectVisible` | `SET_MAP_OBJECT_VISIBLE` | 129 |
| 146 | `CMapLoadable::OnClearBackEffect` | `CLEAR_BACK_EFFECT` | 130 |
| 151 | `CField::OnWhisper` | `WHISPER` | 135 |
| 153 | `CField::OnSummonItemInavailable` | `SUMMON_ITEM_INAVAILABLE` | 137 |
| 154 | `CField::OnFieldEffect` | `FIELD_EFFECT` | 138 |
| 157 | `CField::OnFieldObstacleAllReset` | `FIELD_OBSTACLE_ALL_RESET` | 141 |
| 158 | `CField::OnBlowWeather` | `BLOW_WEATHER` | 142 |
| 160 | `CField::OnAdminResult` | `ADMIN_RESULT` | 144 |
| 166 | `CField::OnSetQuestClear` | `SET_QUEST_CLEAR` | 150 |
| 167 | `CField::OnSetQuestTime` | `SET_QUEST_TIME` | 151 |
| 169 | `CField::OnSetObjectState` | `SET_OBJECT_STATE` | 153 |
| 185 | `CUser::SetConsumeItemEffect` | `SHOW_CONSUME_EFFECT` | 166 |
| 224 | `CUser::OnEffect` | `SHOW_ITEM_EFFECT` | 194 |
| 228 | `CUserRemote::OnGuildNameChanged` | `GUILD_NAME_CHANGED` | 202 |
| 229 | `CUserRemote::OnGuildMarkChanged` | `GUILD_MARK_CHANGED` | 203 |
| 230 | `CUserRemote::OnThrowGrenade` | `THROW_GRENADE` | 204 |
| 233 | `CUser::OnEffect` | `SHOW_ITEM_EFFECT` | 194 |
| 243 | `CUserLocal::OnNotifyHPDecByField` | `ON_NOTIFY_HP_DEC_BY_FIELD` | 212 |
| 248 | `CUserLocal::OnMakerResult` | `MAKER_RESULT` | 217 |
| 251 | `CUserLocal::OnOpenUI` | `OPEN_UI` | 220 |
| 282 | `CSummonedPool::OnSkill` | `SKILL_USE_RESULT` | 37 |
| 300 | `CMob::OnEffectByItem` | `SHOW_ITEM_EFFECT` | 194 |
| 382 | `CCashShop::OnChargeParamResult` | `CHARGE_PARAM_RESULT` | 323 |
| 383 | `CCashShop::OnQueryCashResult` | `QUERY_CASH_RESULT` | 324 |

### 7.2 发送侧词元全等（7 条，均正确）

| v95 send | s9 RecvOpcode | 值 | handler |
|---|---|---|---|
| 13 | `VIEW_ALL_CHAR` | 13 | `ViewAllCharHandler` |
| 24 | `DELETE_CHAR` | 23 | `DeleteCharHandler` |
| 71 | `SKILL_EFFECT` | 93 | `SkillEffectHandler` |
| 82 | `PET_FOOD` | 76 | `PetFoodHandler` |
| 128 | `USE_REMOTE` | 116 | `RemoteGachaponHandler` |
| 129 | `WATER_OF_LIFE` | 117 | `UseWaterOfLifeHandler` |
| 137 | `USE_ITEM_REWARD` | 112 | `ItemRewardHandler` |

## 8. 复现

```bash
# 1) v95 发送站点（IDB 13338，headless）
python E:/project/_ms_probe/batch_send.py            # -> dec_send/*.json
python E:/project/_ms_probe/resolve_dyn.py          # -> ctor_sites.json
python E:/project/_ms_probe/extract_trunc_send.py    # -> trunc_ctor_sites.json / trunc_ctor_resolved.json
python E:/project/_ms_probe/extract_trunc2.py        # -> trunc2_ctor_resolved.json
# 2) 合并 + 去重
python E:/project/_ms_probe/merge_send.py            # -> send_sites_all.json / send_ops.json
# 3) s9 枚举快照
python E:/project/_ms_probe/extract_s9.py            # -> s9_opcodes.json
# 4) 机器候选 + 本文
python E:/project/_ms_probe/build_map2.py            # -> map_candidates2.json
python E:/project/_ms_probe/gen_opcode_map.py        # -> notes/opcode-map-v95-vs-s9.md
```

## 9. 备注

- 本表是**移植设计输入**，不是协议规范。协议 v2 的最终 opcode 值由 s9 侧决定（保留/重排），本表只用于确定“某个 v95 行为在 s9 归哪个包”。
- 095 中文加壳端（`开启游戏.vmp.exe`）无法静态确认自定义 opcode；所有“待核”项在抓包前都不得写入服务端。
- 与已有功能（联盟 143 / 制造 113 / 怪物图鉴 83/84）相关的行已用“证据”标注，可直接复核现有 handler。
- 原始数据与复现脚本归档在 [evidence/opcode-v95/](../evidence/opcode-v95/)（`send_sites_all.json` / `map_candidates2.json` / `s9_opcodes.json` + `scripts/`，校验和见其 `hashes.txt`）。
