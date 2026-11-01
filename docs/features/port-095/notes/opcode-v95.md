# v95 Opcode 表（GMS v95 客户端静态导出）

> 证据源：`E:\MXD\Maplestory95.i64`（未加壳 GMS v95 客户端 + 符号），IDA Pro 9.4 / ida-pro-mcp（13338）。
> 生成脚本：`E:\project\_ms_probe\gen_opcode_doc.py`；原始反编译见 `docs/features/port-095/evidence/opcode-v95/`。
> 本表用于**语义参照**（字段顺序、二级分发、系统归属），不作为 095 中文客户端已确认协议——中文端为加壳二进制，需抓包校准（见 §7）。

## 1. 顶层：`CClientSocket::ProcessPacket` @ `0x4b00f0`

opcode 由 `CInPacket::Decode2` 读出（**uint16**），随后分流：

| opcode | 处理函数 | 说明 |
|---|---|---|
| 0–15 | stage → `CLogin::OnPacket` | 登录阶段 |
| 16 | `CClientSocket::OnMigrateCommand` | 连接层特例 |
| 17 | `CClientSocket::OnAliveReq` | 连接层特例 |
| 18 | `CClientSocket::OnAuthenCodeChanged` | 连接层特例 |
| 19 | `CClientSocket::OnAuthenMessage` | 连接层特例 |
| 20 | `CSecurityClient::OnPacket` | 连接层特例 |
| 23 | `CClientSocket::OnCheckCrcResult` | 连接层特例 |
| 21, 22, 24–27 | stage → `CLogin::OnPacket` | 登录阶段（CLogin 二级分发） |
| 28–140 | `CWvsContext::OnPacket` | 全局上下文（106 个 opcode） |
| 141–432 | stage → `CField::OnPacket` / `CCashShop::OnPacket` | g_pStage 虚调用；地图内为 CField |
| 其他 | g_pStage->OnPacket（无处理则丢弃） | — |

关键证据：`dec_processpacket.c`；`(unsigned int)(v4 - 28) > 0x70` 即 28..140 归 CWvsContext，其余交 `g_pStage`。

## 2. `CWvsContext::OnPacket`（28–140，106 项）

| opcode | 处理函数 |
|---|---|
| 28 | CWvsContext::OnInventoryOperation |
| 29 | CWvsContext::OnInventoryGrow |
| 30 | CWvsContext::OnStatChanged |
| 31 | CWvsContext::OnTemporaryStatSet |
| 32 | CWvsContext::OnTemporaryStatReset |
| 33 | CWvsContext::OnForcedStatSet |
| 34 | CWvsContext::OnForcedStatReset |
| 35 | CWvsContext::OnChangeSkillRecordResult |
| 36 | CWvsContext::OnSkillUseResult |
| 37 | CWvsContext::OnGivePopularityResult |
| 38 | CWvsContext::OnMessage |
| 39 | CWvsContext::OnOpenFullClientDownloadLink |
| 40 | CWvsContext::OnMemoResult |
| 41 | CWvsContext::OnMapTransferResult |
| 42 | CWvsContext::OnAntiMacroResult |
| 44 | CWvsContext::OnClaimResult |
| 45 | CWvsContext::OnSetClaimSvrAvailableTime |
| 46 | CWvsContext::OnClaimSvrStatusChanged |
| 47 | CWvsContext::OnSetTamingMobInfo |
| 48 | CWvsContext::OnQuestClear |
| 49 | CWvsContext::OnEntrustedShopCheckResult |
| 50 | CWvsContext::OnSkillLearnItemResult |
| 51 | CWvsContext::OnSkillResetItemResult |
| 52 | CWvsContext::OnGatherItemResult |
| 53 | CWvsContext::OnSortItemResult |
| 55 | CWvsContext::OnSueCharacterResult |
| 57 | CWvsContext::OnTradeMoneyLimit |
| 58 | CWvsContext::OnSetGender |
| 59 | CWvsContext::OnGuildBBSPacket |
| 61 | CWvsContext::OnCharacterInfo |
| 62 | CWvsContext::OnPartyResult |
| 64 | CWvsContext::OnExpedtionResult |
| 65 | CWvsContext::OnFriendResult |
| 67 | CWvsContext::OnGuildResult |
| 68 | CWvsContext::OnAllianceResult |
| 69 | CWvsContext::OnTownPortal |
| 70 | CWvsContext::OnOpenGate |
| 71 | CWvsContext::OnBroadcastMsg |
| 72 | CWvsContext::OnIncubatorResult |
| 73 | CWvsContext::OnShopScannerResult |
| 74 | CWvsContext::OnShopLinkResult |
| 75 | CWvsContext::OnMarriageRequest |
| 76 | CWvsContext::OnMarriageResult |
| 77 | CWvsContext::OnWeddingGiftResult |
| 78 | CWvsContext::OnNotifyMarriedPartnerMapTransfer |
| 79 | CWvsContext::OnCashPetFoodResult |
| 80 | CWvsContext::OnSetWeekEventMessage |
| 81 | CWvsContext::OnSetPotionDiscountRate |
| 82 | CWvsContext::OnBridleMobCatchFail |
| 83 | CWvsContext::OnImitatedNPCResult |
| 84 | CWvsContext::OnImitatedNPCData |
| 85 | CWvsContext::OnLimitedNPCDisableInfo |
| 86 | CWvsContext::OnMonsterBookSetCard |
| 87 | CWvsContext::OnMonsterBookSetCover |
| 88 | CWvsContext::OnHourChanged |
| 89 | CWvsContext::OnMiniMapOnOff |
| 90 | CWvsContext::OnConsultAuthkeyUpdate |
| 91 | CWvsContext::OnClassCompetitionAuthkeyUpdate |
| 92 | CWvsContext::OnWebBoardAuthkeyUpdate |
| 93 | CWvsContext::OnSessionValue |
| 94 | CWvsContext::OnPartyValue |
| 95 | CWvsContext::OnFieldSetVariable |
| 96 | CWvsContext::OnBonusExpRateChanged |
| 97 | CWvsContext::OnPotionDiscountRateChanged |
| 98 | CWvsContext::OnFamilyChartResult |
| 99 | CWvsContext::OnFamilyInfoResult |
| 100 | CWvsContext::OnFamilyResult |
| 101 | CWvsContext::OnFamilyJoinRequest |
| 102 | CWvsContext::OnFamilyJoinRequestResult |
| 103 | CWvsContext::OnFamilyJoinAccepted |
| 104 | CWvsContext::OnFamilyPrivilegeList |
| 105 | CWvsContext::OnFamilyFamousPointIncResult |
| 106 | CWvsContext::OnFamilyNotifyLoginOrLogout |
| 107 | CWvsContext::OnFamilySetPrivilege |
| 108 | CWvsContext::OnFamilySummonRequest |
| 109 | CWvsContext::OnNotifyLevelUp |
| 110 | CWvsContext::OnNotifyWedding |
| 111 | CWvsContext::OnNotifyJobChange |
| 113 | CWvsContext::OnMapleTVUseRes |
| 114 | CWvsContext::OnAvatarMegaphoneRes |
| 115 | CWvsContext::OnSetAvatarMegaphone |
| 116 | CWvsContext::OnClearAvatarMegaphone |
| 117 | CWvsContext::OnCancelNameChangeResult |
| 118 | CWvsContext::OnCancelTransferWorldResult |
| 119 | CWvsContext::OnDestroyShopResult |
| 120 | CWvsContext::OnFakeGMNotice |
| 121 | CWvsContext::OnSuccessInUsegachaponBox |
| 122 | CWvsContext::OnNewYearCardRes |
| 123 | CWvsContext::OnRandomMorphRes |
| 124 | CWvsContext::OnCancelNameChangebyOther |
| 125 | CWvsContext::OnSetBuyEquipExt |
| 126 | CWvsContext::OnSetPassenserRequest |
| 127 | CWvsContext::OnScriptProgressMessage |
| 128 | CWvsContext::OnDataCRCCheckFailed |
| 129 | CWvsContext::OnCakePieEventResult |
| 130 | CWvsContext::OnUpdateGMBoard |
| 131 | CWvsContext::OnShowSlotMessage |
| 132 | CWvsContext::OnWildHunterInfo |
| 133 | CWvsContext::OnAccountMoreInfo |
| 134 | CWvsContext::OnFindFirend |
| 135 | CWvsContext::OnStageChange |
| 136 | CWvsContext::OnDragonBallBox |
| 137 | CWvsContext::OnAskWhetherUsePamsSong |
| 138 | CWvsContext::OnTransferChannel |
| 139 | CWvsContext::OnDisallowedDeliveryQuestList |
| 140 | CWvsContext::OnMacroSysDataInit |

未使用/无处理：43、54、56、60、63、66、112（default 直接 return）。

## 3. `CField::OnPacket`（141–432）

### 3.1 区间路由（转交各子系统池/对话框）

| opcode 区间 | 转交 |
|---|---|
| 141–143 | `CStage::OnPacket` |
| 144–146 | `CMapLoadable::OnPacket` |
| 179–195 | `CUserPool::OnPacket` |
| 197–277 | `CUserPool::OnPacket` |
| 278–283 | `CSummonedPool::OnPacket` |
| 284–310 | `CMobPool::OnPacket` |
| 311–318 | `CNpcPool::OnPacket` |
| 319–321 | `CEmployeePool::OnPacket` |
| 322–324 | `CDropPool::OnPacket` |
| 325–327 | `CMessageBoxPool::OnPacket` |
| 328–329 | `CAffectedAreaPool::OnPacket` |
| 330–331 | `CTownPortalPool::OnPacket` |
| 332–333 | `COpenGatePool::OnPacket` |
| 334–337 | `CReactorPool::OnPacket` |
| 363 | `CScriptMan::OnPacket` |
| 364–365 | `CShopDlg::OnPacket` |
| 366–367 | `CAdminShopDlg::OnPacket` |
| 369–370 | `CStoreBankDlg::OnPacket` |
| 398–400 | `CFuncKeyMappedMan::OnPacket` |
| 404–409 | `CMapleTVMan::OnPacket` |
| 413–416 | `CField::OnCharacterSale` |
| 420–423 | `CBattleRecordMan::OnPacket` |
| 424–427 | `CField::OnItemUpgrade` |
| 428–431 | `CField::OnVega` |

### 3.2 直接 case（CField 自身处理）

| opcode | 处理函数 |
|---|---|
| 147 | CField::OnTransferFieldReqIgnored |
| 148 | CField::OnTransferChannelReqIgnored |
| 149 | CField::OnFieldSpecificData |
| 150 | CField::OnGroupMessage |
| 151 | CField::OnWhisper |
| 152 | CField::OnCoupleMessage |
| 153 | CField::OnSummonItemInavailable |
| 154 | CField::OnFieldEffect |
| 155 | CField::OnFieldObstacleOnOff |
| 156 | CField::OnFieldObstacleOnOffStatus |
| 157 | CField::OnFieldObstacleAllReset |
| 158 | CField::OnBlowWeather |
| 159 | CField::OnPlayJukeBox |
| 160 | CField::OnAdminResult |
| 161 | CField::OnQuiz |
| 162 | CField::OnDesc |
| 163 | → vtable 虚调用（本次确认为 CField::OnClock） |
| 166 | CField::OnSetQuestClear |
| 167 | CField::OnSetQuestTime |
| 168 | CField::OnWarnMessage |
| 169 | CField::OnSetObjectState |
| 170 | CField::OnDestroyClock |
| 172 | CField::OnStalkResult |
| 175 | CQuickslotKeyMappedMan::OnInit |
| 176 | CField::OnFootHoldInfo |
| 177 | CField::OnRequestFootHoldInfo |
| 196 | 无处理 |
| 359 | CField::OnHontaleTimer |
| 360 | CField::OnChaosZakumTimer |
| 361 | CField::OnHontailTimer |
| 362 | CField::OnZakumTimer |
| 368 | CTrunkDlg::OnPacket |
| 371 | CRPSGameDlg::OnPacket |
| 372 | CUIMessenger::OnPacket |
| 373 | CMiniRoomBaseDlg::OnPacketBase |
| 381 | CParcelDlg::OnPacket |
| 432 | CWvsContext::OnLogoutGift |

### 3.3 141–432 中无处理/未使用

164、165、171、173、174、178、338、339、340、341、342、343、344、345、346、347、348、349、350、351、352、353、354、355、356、357、358、374、375、376、377、378、379、380、382、383、384、385、386、387、388、389、390、391、392、393、394、395、396、397、401、402、403、410、411、412、417、418、419

其中 382–396 在**商城阶段**由 `CCashShop::OnPacket` 处理（见 §4.20），CField 本身忽略；196 命中后直接 `return`；163 为 CField 虚函数 `OnClock`（子操作码 0=事件倒计时 / 1=设置时刻 / 2=创建时钟 / 3=停止，见 `dec_clock.json`）。

注：179–277 除 196 外整体交 `CUserPool`。

## 4. 二级分发


### 4.1 CUserPool::OnPacket（179–276）

| 子 opcode | 处理函数 |
|---|---|
| 179 | `CUserPool::OnUserEnterField` |
| 180 | `CUserPool::OnUserLeaveField` |
| 181–209 | `CUserPool::OnUserCommonPacket` |
| 210–230 | `CUserPool::OnUserRemotePacket` |
| 231–276 | `CUserPool::OnUserLocalPacket` |

### 4.2 CUserRemote::OnPacket（210–230）

两次 switch：211–214 共用 `OnAttack(nType)`；210/222/223/225–229 在“延迟加载/切换中”分支里仍会处理（第二条 switch 用同一 nType）。

| 子 opcode | 处理函数 |
|---|---|
| 210 | `CUserRemote::OnMove` |
| 214 | `CUserRemote::OnAttack` |
| 215 | `CUserRemote::OnSkillPrepare` |
| 216 | `CUserRemote::OnMovingShootAttackPrepare` |
| 217 | `CUserRemote::OnSkillCancel` |
| 218 | `CUserRemote::OnHit` |
| 219 | `CUser::OnEmotion` |
| 220 | `CUserRemote::OnSetActiveEffectItem` |
| 221 | `CUserRemote::OnShowUpgradeTombEffect` |
| 222 | `CUserRemote::OnSetActivePortableChair` |
| 223 | `CUserRemote::OnAvatarModified` |
| 224 | `CUser::OnEffect` |
| 225 | `CUserRemote::OnSetTemporaryStat` |
| 226 | `CUserRemote::OnResetTemporaryStat` |
| 227 | `CUserRemote::OnReceiveHP` |
| 228 | `CUserRemote::OnGuildNameChanged` |
| 229 | `CUserRemote::OnGuildMarkChanged` |
| 230 | `CUserRemote::OnThrowGrenade` |

### 4.3 CUserCommon::OnPacket（181–209）

先 `Decode4` 取角色 ID 查 `CUserPool::GetUser`；nType=182 且角色存在时按 181 处理（自己发的聊天）。

| 子 opcode | 处理函数 |
|---|---|
| 181 | `CUser::OnChat(bIsFromOutsideOfMap=0)` |
| 182 | `CUser::OnChat(bIsFromOutsideOfMap=1)` |
| 183 | `CUser::OnADBoard` |
| 184 | `CUser::OnMiniRoomBalloon` |
| 185 | `CUser::SetConsumeItemEffect` |
| 186 | `CUser::ShowItemUpgradeEffect` |
| 187 | `CUser::ShowItemHyperUpgradeEffect` |
| 188 | `CUser::ShowItemOptionUpgradeEffect` |
| 189 | `CUser::ShowItemReleaseEffect` |
| 190 | `CUser::ShowItemUnreleaseEffect` |
| 191 | `CUser::OnHitByUser` |
| 192 | `CUser::OnTeslaTriangle` |
| 193 | `CUser::OnFollowCharacter` |
| 194 | `CUser::OnShowPQReward` |
| 195 | `CUser::OnSetPhase` |
| 197 | `CUser::ShowRecoverUpgradeCountEffect` |
| 198–205 | `CUser::OnPetPacket` |
| 206–208 | `CUser::OnDragonPacket` |
| 196, 209 | 无处理 |
### 4.4 CUserLocal::OnPacket（231–276）

CUserPool 在 231–276 直接转 `CUserLocal::OnPacket`（单参数版本），下表为该函数内部 switch。

| 子 opcode | 处理函数 |
|---|---|
| 231 | `CUserLocal::OnSitResult` |
| 232 | `CUser::OnEmotion` |
| 233 | `CUser::OnEffect` |
| 234 | `CUserLocal::OnTeleport` |
| 236 | `CUserLocal::OnMesoGive_Succeeded` |
| 237 | `CUserLocal::OnMesoGive_Failed` |
| 238 | `CUserLocal::OnRandomMesobag_Succeeded` |
| 239 | `CUserLocal::OnRandomMesobag_Failed` |
| 240 | `CUserLocal::OnFieldFadeInOut` |
| 241 | `CUserLocal::OnFieldFadeOutForce` |
| 242 | `CUserLocal::OnQuestResult` |
| 243 | `CUserLocal::OnNotifyHPDecByField` |
| 245 | `CUserLocal::OnBalloonMsg` |
| 246 | `CUserLocal::OnPlayEventSound` |
| 247 | `CUserLocal::OnPlayMinigameSound` |
| 248 | `CUserLocal::OnMakerResult` |
| 250 | `CUserLocal::OnOpenClassCompetitionPage` |
| 251 | `CUserLocal::OnOpenUI` |
| 252 | `CUserLocal::OnOpenUIWithOption` |
| 253 | `CUserLocal::OnSetDirectionMode` |
| 254 | `CUserLocal::OnSetStandAloneMode` |
| 255 | `CUserLocal::OnHireTutor` |
| 256 | `CUserLocal::OnTutorMsg` |
| 257 | `CUserLocal::OnIncComboResponse` |
| 258 | `CUser::OnRandomEmotion` |
| 259 | `CUserLocal::OnResignQuestReturn` |
| 260 | `CUserLocal::OnPassMateName` |
| 261 | `CUserLocal::OnRadioSchedule` |
| 262 | `CUserLocal::OnOpenSkillGuide` |
| 263 | `CUserLocal::OnNoticeMsg` |
| 264 | `CUserLocal::OnChatMsg` |
| 265 | `CUserLocal::OnBuffzoneEffect` |
| 266 | `CUserLocal::OnGoToCommoditySN` |
| 267 | `CUserLocal::OnDamageMeter` |
| 268 | `CUserLocal::OnTimeBombAttack` |
| 269 | `CUser::OnPassiveMove` |
| 270 | `CUserLocal::OnFollowCharacterFailed` |
| 271 | `CUserLocal::OnVengeanceSkillApply` |
| 272 | `CUserLocal::OnExJablinApply` |
| 273 | `CUserLocal::OnAskAPSPEvent` |
| 274 | `CUserLocal::OnQuestGuideResult` |
| 275 | `CUserLocal::OnDeliveryQuest` |
| 276 | `CUserLocal::OnSkillCooltimeSet` |

### 4.5 CMobPool::OnPacket（284–310）

| 子 opcode | 处理函数 |
|---|---|
| 284 | `CMobPool::OnMobEnterField` |
| 285 | `CMobPool::OnMobLeaveField` |
| 286 | `CMobPool::OnMobChangeController` |
| 287–296 | `CMobPool::OnMobPacket` |
| 297 | `CMobPool::OnMobCrcKeyChanged` |
| 298–309 | `CMobPool::OnMobPacket` |

### 4.6 CMob::OnPacket（287–309）

| 子 opcode | 处理函数 |
|---|---|
| 287 | `CMob::OnMove` |
| 288 | `CMob::OnCtrlAck` |
| 290 | `CMob::OnStatSet` |
| 291 | `CMob::OnStatReset` |
| 292 | `CMob::OnSuspendReset` |
| 293 | `CMob::OnAffected` |
| 294 | `CMob::OnDamaged` |
| 295 | `CMob::OnSpecialEffectBySkill` |
| 298 | `CMob::OnHPIndicator` |
| 299 | `CMob::OnCatchEffect` |
| 300 | `CMob::OnEffectByItem` |
| 301 | `CMob::OnMobSpeaking` |
| 302 | `CMob::OnIncMobChargeCount` |
| 303 | `CMob::OnMobSkillDelay` |
| 304 | `CMob::OnEscortFullPath` |
| 305 | `CMob::OnEscortStopEndPermmision` |
| 306 | `CMob::OnEscortStopSay` |
| 307 | `CMob::OnEscortReturnBefore` |
| 308 | `CMob::OnNextAttack` |
| 309 | `CMob::OnMobAttackedByMob` |

### 4.7 CNpcPool::OnPacket（311–318）

| 子 opcode | 处理函数 |
|---|---|
| 84 | `CNpcPool::OnNpcImitateData` |
| 85 | `CNpcPool::OnUpdateLimitedDisableInfo` |
| 311 | `CNpcPool::OnNpcEnterField` |
| 312 | `CNpcPool::OnNpcLeaveField` |
| 313 | `CNpcPool::OnNpcChangeController` |
| 314–316 | `CNpcPool::OnNpcPacket` |
| 317 | `CNpcPool::OnNpcTemplatePacket` |

### 4.8 CNpc::OnPacket（314–316）

| 子 opcode | 处理函数 |
|---|---|
| 314 | `CNpc::OnMove` |
| 315 | `CNpc::OnUpdateLimitedInfo` |
| 316 | `CNpc::OnSetSpecialAction` |

### 4.9 CNpcTemplate::OnPacket（317）

| 子 opcode | 处理函数 |
|---|---|
| 317 | `CNpcTemplate::OnSetNpcScript` |

### 4.10 CSummonedPool（278–283）

| 子 opcode | 处理函数 |
|---|---|
| 278 | `CSummonedPool::OnCreated` |
| 279 | `CSummonedPool::OnRemoved` |
| 280 | `CSummonedPool::OnMove` |
| 281 | `CSummonedPool::OnAttack` |
| 282 | `CSummonedPool::OnSkill` |
| 283 | `CSummonedPool::OnHit` |

### 4.11 CEmployeePool（319–321）

| 子 opcode | 处理函数 |
|---|---|
| 319 | `CEmployeePool::OnEmployeeEnterField` |
| 320 | `CEmployeePool::OnEmployeeLeaveField` |
| 321 | `CEmployeePool::OnEmployeeMiniRoomBalloon` |

### 4.12 CDropPool（322–324）

| 子 opcode | 处理函数 |
|---|---|
| 322 | `CDropPool::OnDropEnterField` |
| 324 | `CDropPool::OnDropLeaveField` |

### 4.13 CMessageBoxPool（325–327）

| 子 opcode | 处理函数 |
|---|---|
| 325 | `CMessageBoxPool::OnCreateFailed` |
| 326 | `CMessageBoxPool::OnMessageBoxEnterField` |
| 327 | `CMessageBoxPool::OnMessageBoxLeaveField` |

### 4.14 CAffectedAreaPool（328–329）

| 子 opcode | 处理函数 |
|---|---|
| 328 | `CAffectedAreaPool::OnAffectedAreaCreated` |
| 329 | `CAffectedAreaPool::OnAffectedAreaRemoved` |

### 4.15 CTownPortalPool（330–331）

| 子 opcode | 处理函数 |
|---|---|
| 330 | `CTownPortalPool::OnTownPortalCreated` |
| 331 | `CTownPortalPool::OnTownPortalRemoved` |

### 4.16 COpenGatePool（332–333）

| 子 opcode | 处理函数 |
|---|---|
| 332 | `COpenGatePool::OnOpenGateCreated` |
| 333 | `COpenGatePool::OnOpenGateRemoved` |

### 4.17 CReactorPool（334–337）

| 子 opcode | 处理函数 |
|---|---|
| 334 | `CReactorPool::OnReactorChangeState` |
| 335 | `CReactorPool::OnReactorMove` |
| 336 | `CReactorPool::OnReactorEnterField` |
| 337 | `CReactorPool::OnReactorLeaveField` |

### 4.18 CMapLoadable（144–146）

| 子 opcode | 处理函数 |
|---|---|
| 144 | `CMapLoadable::OnSetBackEffect` |
| 145 | `CMapLoadable::OnSetMapObjectVisible` |
| 146 | `CMapLoadable::OnClearBackEffect` |

### 4.19 CLogin（0–27，含 141–146 转发）

| 子 opcode | 处理函数 |
|---|---|
| 0 | `CLogin::OnCheckPasswordResult` |
| 1 | `CLogin::OnGuestIDLoginResult` |
| 2 | `CLogin::OnAccountInfoResult` |
| 3 | `CLogin::OnCheckUserLimitResult` |
| 4 | `CLogin::OnSetAccountResult` |
| 5 | `CLogin::OnConfirmEULAResult` |
| 6 | `CLogin::OnCheckPinCodeResult` |
| 7 | `CLogin::OnUpdatePinCodeResult` |
| 8 | `CLogin::OnViewAllCharResult` |
| 9 | `CLogin::OnSelectCharacterByVACResult` |
| 10 | `CLogin::OnWorldInformation` |
| 11 | `CLogin::OnSelectWorldResult` |
| 12 | `CLogin::OnSelectCharacterResult` |
| 13 | `CLogin::OnCheckDuplicatedIDResult` |
| 14 | `CLogin::OnCreateNewCharacterResult` |
| 15 | `CLogin::OnDeleteCharacterResult` |
| 21 | `CLogin::OnEnableSPWResult` |
| 24 | `CLogin::OnLatestConnectedWorld` |
| 25 | `CLogin::OnRecommendWorldMessage` |
| 26 | `CLogin::OnExtraCharInfoResult` |
| 27 | `CLogin::OnCheckSPWResult` |
| 141–143 | `CStage::OnPacket` |
| 144–146 | `CMapLoadable::OnPacket` |

### 4.20 CCashShop（382–396）

392/393 共用 `OnCashItemGachaponResult`；389、394 无处理。

| 子 opcode | 处理函数 |
|---|---|
| 382 | `CCashShop::OnChargeParamResult` |
| 383 | `CCashShop::OnQueryCashResult` |
| 384 | `CCashShop::OnCashItemResult` |
| 385 | `CCashShop::OnPurchaseExpChanged` |
| 386 | `CCashShop::OnGiftMateInfoResult` |
| 387 | `CCashShop::OnCheckDuplicatedIDResult` |
| 388 | `CCashShop::OnCheckNameChangePossibleResult` |
| 390 | `CCashShop::OnCheckTransferWorldPossibleResult` |
| 391 | `CCashShop::OnCashShopGachaponStampResult` |
| 393 | `CCashShop::OnCashItemGachaponResult` |
| 395 | `CCashShop::OnOneADay` |
| 396 | `CCashShop::OnNoticeFreeCashItem` |

### 4.21 CScriptMan（363）

| 子 opcode | 处理函数 |
|---|---|
| 363 | `CScriptMan::OnScriptMessage` |

### 4.22 CUIMessenger（372）

子码 3 = `OnInvite`（在 switch 之前单独判断）。

| 子 opcode | 处理函数 |
|---|---|
| 0 | `CUIMessenger::OnEnter` |
| 1 | `CUIMessenger::OnSelfEnterResult` |
| 2 | `CUIMessenger::OnLeave` |
| 4 | `CUIMessenger::OnInviteResult` |
| 5 | `CUIMessenger::OnBlocked` |
| 6 | `CUIMessenger::OnChat` |
| 7 | `CUIMessenger::OnAvatar` |
| 8 | `CUIMessenger::OnMigrated` |

### 4.23 CMiniRoomBaseDlg::OnPacketBase（373）

无对话框实例时只处理 2/3/5/14（静态邀请/进入/SSN）；有实例时 3/4/6/9/10，其余交虚函数。

| 子 opcode | 处理函数 |
|---|---|
| 2 | `CMiniRoomBaseDlg::OnInviteStatic` |
| 3 | `CMiniRoomBaseDlg::OnInviteResultStatic` |
| 4 | `CMiniRoomBaseDlg::OnEnterBase` |
| 5 | `CMiniRoomBaseDlg::OnEnterResultStatic` |
| 9 | `CMiniRoomBaseDlg::OnAvatar` |
| 10 | `CMiniRoomBaseDlg::OnLeaveBase` |
| 14 | `CMiniRoomBaseDlg::OnCheckSSN2Static` |

### 4.24 CRPSGameDlg（371）

| 子 opcode | 行为 |
|---|---|
| 8 | 打开对话框（`CDialog::DoModal`） |
| 6, 7, 9–12, 14 | `CRPSGameDlg::ProcessPacket` |
| 13 | `CWnd::Destroy` |

### 4.25 CParcelDlg（381）

| 子 opcode | 行为 |
|---|---|
| 8 | `SetParcelDlg`（打开包裹对话框，Decode1 决定 nMode=2/0） |
| 23 | `RemoveParcel(SN)` + 结果提示（3=删除成功，其他=领取成功） |
| 24 | `PARCEL::Decode` + `AddNewParcel` + 新包裹提示 |
| 25 | 快速投递提醒：`DecodeStr(sender)` + `Decode1` + `CUIFadeYesNo::CreateParcelAlarm` |
| 26 | 新建空白对话框 `CParcelDlg(nMode=1)` |
| 27 | `Decode1` + `CUIFadeYesNo::CreateParcelAlarm` |
| 其他(默认) | `NoticeResult(nResult)` + `SetCtrlEnabled`；nResult=18 时按 nMode 关闭或 `ResetSendInfo` |

### 4.26 CTrunkDlg（368）

| 子 opcode | 行为 |
|---|---|
| 9 | `CmpPutItem` 刷新“放入”列表并定位滚动条 |
| 13, 15, 19 | `SetGetItems` + `InvalidateRect`（取回列表） |
| 22 | 打开仓库对话框 `SetTrunkDlg` |
| 10 | 提示：背包已满 |
| 11, 16 | 提示：金币不足 |
| 12 | 提示：该物品无法取回 |
| 17 | 提示：仓库已满 |
| 23 | 提示：物品/金币无法移动，请联系客服 |
| 24 | `Decode1`；非 0 则 `DecodeStr` 后提示，否则“交易失败” |
| 其他(默认) | 提示：由于错误，交易未发生 |
### 4.27 CFuncKeyMappedMan（398–400）

| 子 opcode | 处理函数 |
|---|---|
| 398 | `CFuncKeyMappedMan::OnInit` |
| 399 | `CFuncKeyMappedMan::OnPetConsumeItemInit` |
| 400 | `CFuncKeyMappedMan::OnPetConsumeMPItemInit` |

### 4.28 CMapleTVMan（404–409）

| 子 opcode | 处理函数 |
|---|---|
| 405 | `CMapleTVMan::OnSetMessage` |
| 406 | `CMapleTVMan::OnClearMessage` |
| 407 | `CMapleTVMan::OnSendMessageResult` |

### 4.29 CBattleRecordMan（420–423）

CField 把 420–423 交给本池，其中 420、423 无处理。

| 子 opcode | 处理函数 |
|---|---|
| 421 | `CBattleRecordMan::OnDotDamageInfo` |
| 422 | `CBattleRecordMan::OnServerOnCalcRequestResult` |

### 4.30 CShopDlg（364 打开 / 365 子操作码）

子操作码：0、1、2、3、4、5、8、9、10、13、14、15、16、17、18、19。365 的子码：0（出售列表刷新，`CmpSellItem`）、1–5、8–10、13–19（购买/回退/结果提示，含 `CUtilDlg::Notice`）。原始反编译：`evidence/opcode-v95/dec/_OnPacket_CShopDlg__SAXJAAVCInPacket___Z.c`。

### 4.31 CAdminShopDlg（366 打开 / 367 子操作码）

子操作码：1、2、3、4、5、6、7、8、9、10、11。367 的子码 1–11 经 `v18`/`v20` 两次 Decode1 展开，10/11 会弹 `CUtilDlgEx`（见反编译正文）。原始反编译：`evidence/opcode-v95/dec/_OnPacket_CAdminShopDlg__SAXJAAVCInPacket___Z.c`。

### 4.32 CStoreBankDlg（369 打开 / 370 子操作码）

子操作码：30、31、32、33、34。370 的子码为 30–34 与 35('#'/取钱) 36('$') 37('%') 38('&')（见反编译正文）。原始反编译：`evidence/opcode-v95/dec/_OnPacket_CStoreBankDlg__SAXJAAVCInPacket___Z.c`。

## 5. 发送（客户端 → 服务端）opcode

方法：对 `??0COutPacket@@QAE@J@Z`（`0x68d090`）取全部 **582** 处 xref（462 个函数）。

- 502 处来自 `dec_send/` 完整反编译，抽取 `COutPacket::COutPacket(this: &p, nType: N)` 常量（含 `(COutPacket *)&p` 形式与变量回溯，见 `resolve_dyn.py`）；
- 80 处位于 11 个**反编译被截断或漏采**的函数（`0x5408e0`/`0x654300`/`0x6b1fc0`/`0x92a240`/`0x9343c0`/`0x937330`/`0x9eb3e0`/`0xa0d3b0` + `0x5dbef0`/`0x627e20`/`0x9a6dd0`），用 `insn_query` 回查紧邻 ctor 的 `push <imm>` 补齐；跳转表分支共用同一 `push` 的（`0x6282da` → `0x6282e5`/`0x628304`）按共享处理。

共解析出 **238** 个不同发送 opcode。旧版 217 的两处错误：15 个“动态 opcode”实为 `this: (COutPacket *)&pkt` 的强制转换被正则漏匹配；80 个构造点因函数未采集/反编译截断而丢失。

相对旧表的 12 个新增 opcode：**20、32、52、126、139、152、158、219、230、231、234、258**（明细与语义见 [opcode-map-v95-vs-s9.md §5.1](opcode-map-v95-vs-s9.md)）。

每个 opcode 最多列 4 个发送函数；完整数据：

- `evidence/opcode-v95/send_sites_all.json`：逐个构造点（`fn` / `name` / `op` / `how` / `src`）
- `evidence/opcode-v95/send_ops.json`：`{opcode: [函数地址]}`

| opcode | 发送函数（调用点所在函数 0x 地址） |
|---|---|
| 1 | `CLogin::SendCheckPasswordPacket` (0x5db9d0) |
| 4 | `CLogin::ChangeStepImmediate` (0x5d9730)<br>`CLogin::Init` (0x5d8010) |
| 6 | `CLogin::SendCheckUserLimitPacket` (0x5d43d0) |
| 7 | `CLogin::OnAcceptLicense` (0x5d4540)<br>`CLogin::OnDenyLicense` (0x5d45d0) |
| 8 | `CLogin::SendCancelGenderPacket` (0x5d46e0)<br>`CLogin::SendSetGenderPacket` (0x5d4650) |
| 9 | `CLogin::OnCheckPasswordResult` (0x5dc600)<br>`CLogin::OnCheckPinCodeResult` (0x5db000)<br>`CLogin::OnSelectWorldResult` (0x5dda00)<br>`CLogin::OnSetAccountResult` (0x5d5e80) |
| 10 | `CLogin::OnCheckPinCodeResult` (0x5db000) |
| 11 | `CLicenseDlg::OnButtonClicked` (0x5ff870)<br>`CLogin::OnCheckPasswordResult` (0x5dc600)<br>`CLogin::OnCheckPinCodeResult` (0x5db000) |
| 12 | `CLogin::GotoWorldSelect` (0x5daf20)<br>`CLogin::SendViewAllCharPacket` (0x5dfb40) |
| 13 | `CLogin::SendViewAllCharPacket` (0x5dfb40) |
| 14 | `CLogin::SendSelectCharPacketByVAC` (0x5d7550) |
| 15 | `CLogin::MakeVACDlg` (0x5d44a0)<br>`CLogin::ResetVAC` (0x5d7dc0) |
| 16 | `CCashShop::SendCheckNameChangePossiblePacket` (0x488190) |
| 18 | `CCashShop::SendCheckTransferWorldPossiblePacket` (0x4884c0) |
| 19 | `CLogin::SendSelectCharPacket` (0x5da2a0) |
| 20 | `CClientSocket::OnConnect` (0x4aef10) |
| 21 | `CCashShop::SendCheckDuplicateIDPacket` (0x488070)<br>`CLogin::SendCheckDuplicateIDPacket` (0x5d5690) |
| 22 | `CLogin::SendNewCharPacket` (0x5d7bd0) |
| 23 | `CLogin::SendNewCharPacket` (0x5d7bd0) |
| 24 | `CLogin::SendDeleteCharPacket` (0x5d53a0) |
| 25 | `CClientSocket::OnAliveReq` (0x4afc90) |
| 27 | `CSecurityClient::OnCheckClientIntegrityRequest` (0xa1ac10) |
| 28 | `CLogin::SendSelectCharPacket` (0x5da2a0) |
| 29 | `CLogin::SendSelectCharPacket` (0x5da2a0) |
| 30 | `CLogin::SendSelectCharPacketByVAC` (0x5d7550) |
| 31 | `CLogin::SendSelectCharPacketByVAC` (0x5d7550) |
| 32 | `CClientSocket::OnConnect` (0x4aef10) |
| 34 | `CLogin::Init` (0x5d8010) |
| 35 | `CLogin::SendCheckPasswordPacket` (0x5db9d0)<br>`CLogin::SendLoginPacket` (0x5dbef0)<br>`CLogin::SendViewAllCharPacket` (0x5dfb40) |
| 36 | `CWvsApp::SendBackupPacket` (0x9c7a80)<br>`CWvsApp::SendClearStackLog` (0x9c7b60) |
| 41 | `CCashShop::SendTransferFieldPacket` (0x494a20)<br>`CField::SendTransferFieldRequest` (0x5345c0)<br>`CITC::SendTransferFieldPacket` (0x572b70) |
| 42 | `CField::SendTransferChannelRequest` (0x52efa0) |
| 43 | `CWvsContext::SendMigrateToShopRequest` (0x9dc280) |
| 44 | `CUserLocal::OnKey` (0x9369f0) |
| 45 | `CUserLocal::HandleXKeyDown` (0x90f6d0)<br>`CWvsContext::SendGetUpFromChairRequest` (0x9d6740) |
| 46 | `CWvsContext::SendSitOnPortableChairRequest` (0x9da100) |
| 51 | `CUserLocal::TryDoingSmoothingMovingShootAttackPrepare` (0x912f90) |
| 52 | `CUserLocal::SetDamaged` (0x9343c0)<br>`CUserLocal::Update` (0x937330) |
| 54 | `CField::SendChatMsg` (0x534000)<br>`CField::SendChatMsgSlash` (0x5408e0) |
| 55 | `CUserLocal::HandleLButtonClk` (0x933920) |
| 56 | `CUserLocal::Update` (0x937330)<br>`CWvsContext::SendEmotionChange` (0x9f9320) |
| 57 | `CWvsContext::SendActiveEffectItemChange` (0x9f9420) |
| 58 | `CUserLocal::RequestUpgradeTombEffect` (0x908320) |
| 61 | `CUserLocal::SendBanMapByMobRequest` (0x908d50) |
| 63 | `CNpc::ShowQuestList` (0x672b50)<br>`CUserLocal::TalkToNpc` (0x9321f0) |
| 64 | `CWvsContext::SendRemoteShopOpenRequest` (0x9f30d0) |
| 65 | `CScriptMan::OnAskAvatar` (0x6dcff0)<br>`CScriptMan::OnAskBoxText` (0x6dc9c0)<br>`CScriptMan::OnAskMembershopAvatar` (0x6dd340)<br>`CScriptMan::OnAskMenu` (0x6dce00) …（共 14 个函数） |
| 66 | `CShopDlg::SendBuyRequest` (0x6e9bb0)<br>`CShopDlg::SendRechargeRequest` (0x6e4e90)<br>`CShopDlg::SendSellRequest` (0x6e7260)<br>`CShopDlg::SetRet` (0x6e4b80) |
| 67 | `CTrunkDlg::SendGetItemRequest` (0x769e00)<br>`CTrunkDlg::SendGetMoneyRequest` (0x7688e0)<br>`CTrunkDlg::SendPutItemRequest` (0x768570)<br>`CTrunkDlg::SendPutMoneyRequest` (0x7689e0) …（共 6 个函数） |
| 68 | `CWvsContext::SendEntrustedShopCheckRequest` (0x9fab90) |
| 69 | `CStoreBankDlg::SendCalculateFeeRequest` (0x743f70)<br>`CStoreBankDlg::SendGetAllRequest` (0x7449f0)<br>`CStoreBankDlg::SetRet` (0x743eb0) |
| 70 | `CParcelDlg::CloseParcelDlg` (0x68ef40)<br>`CTabQuickSend::SendQuickDelivery` (0x68f7d0)<br>`CTabReceive::DiscardParcel` (0x68f5e0)<br>`CTabReceive::ReceiveParcel` (0x68f470) …（共 6 个函数） |
| 71 | `CUserLocal::SendSkillEffectRequest` (0x93aeb0) |
| 72 | `CUIShopScanner::OnCreate` (0x848b90) |
| 73 | `CUIShopScanResult::OnButtonClicked` (0x848e80) |
| 74 | `CAdminShopDlg::OnPacket` (0x4310f0)<br>`CAdminShopDlg::SendTradeRequest` (0x430220)<br>`CAdminShopDlg::SetRet` (0x42ca60)<br>`CUIAdminShopWishList::SendRegisterPacket` (0x76e200) |
| 75 | `CWvsContext::SendGatherItemRequest` (0x9d5b70) |
| 76 | `CWvsContext::SendSortItemRequest` (0x9d5c60) |
| 77 | `CWvsContext::SendChangeSlotPositionRequest` (0x9d9c10) |
| 78 | `CWvsContext::SendStatChangeItemUseRequest` (0x9ddfe0) |
| 79 | `CWvsContext::SendStatChangeItemCancelRequest` (0x9d9dd0) |
| 80 | `CWvsContext::TryRecovery` (0x9d4020) |
| 81 | `CWvsContext::SendMobSummonItemUseRequest` (0x9de580) |
| 82 | `CWvsContext::SendPetFoodItemUseRequest` (0x9d9f20) |
| 83 | `CWvsContext::SendTamingMobFoodItemUseRequest` (0x9d63a0) |
| 84 | `CWvsContext::SendScriptRunItemRequest` (0x9de7a0) |
| 85 | `CItemSpeakerDlg::_SendConsumeCashItemUseRequest` (0x5c9e70)<br>`CUICharacterSaleDlg::SendCreateNewCharacter` (0x77a240)<br>`CUIIncubator::OnButtonClicked` (0x7ca660)<br>`CUIKarmaDlg::_SendConsumeCashItemUseRequest` (0x7d7ef0) …（共 6 个函数） |
| 86 | `CWvsContext::SendActivatePetRequest` (0x9f6980) |
| 87 | `CWvsContext::SendBridleItemUseRequest` (0x9e08c0) |
| 88 | `CWvsContext::SendSkillLearnItemUseRequest` (0x9d65e0) |
| 89 | `CWvsContext::SendSkillResetItemUseRequest` (0x9de8c0) |
| 90 | `CWvsContext::SendShopScannerItemUseRequest` (0x9e10e0) |
| 91 | `CWvsContext::SendMapTransferItemUseRequest` (0x9e6020) |
| 92 | `CWvsContext::SendPortalScrollUseRequest` (0x9fca70) |
| 93 | `CWvsContext::SendUpgradeItemUseRequest` (0x9d6260) |
| 94 | `CWvsContext::SendHyperUpgradeItemUseRequest` (0x9d6130) |
| 95 | `CWvsContext::SendItemOptionUpgradeItemUseRequest` (0x9d6000) |
| 96 | `CWvsContext::SendUIOpenItemRequest` (0x9d64d0) |
| 97 | `CWvsContext::SendItemReleaseRequest` (0x9d5f10) |
| 98 | `CWvsContext::SendAbilityUpRequest` (0x9f61c0) |
| 99 | `CWvsContext::SendAbilityUpRequest` (0x9f63b0) |
| 100 | `CWvsContext::SendStatChangeRequest` (0x9f2a00) |
| 101 | `CWvsContext::SendStatChangeRequestByItemOption` (0x9f2950) |
| 102 | `CWvsContext::SendSkillUpRequest` (0x9f2e90) |
| 103 | `CGrenade::SendTimeBombInfo` (0x568f20)<br>`CUserLocal::DoActiveSkill_DamageMeter` (0x93d0d0)<br>`CUserLocal::DoActiveSkill_Flying` (0x93cec0)<br>`CUserLocal::DoActiveSkill_Heal` (0x93a830) …（共 15 个函数） |
| 104 | `CUserLocal::SendSkillCancelRequest` (0x93d730) |
| 105 | `CUserLocal::DoActiveSkill_Prepare` (0x941710) |
| 106 | `CWvsContext::SendDropMoneyRequest` (0x9f6650) |
| 107 | `CWvsContext::SendGivePopularityRequest` (0x9f67e0) |
| 109 | `CWvsContext::SendCharacterInfoRequest` (0x9f2f70) |
| 110 | `CWvsContext::SendActivatePetRequest` (0x9f6980) |
| 111 | `CWvsContext::CheckTemporaryStatDuration` (0x9f2d30) |
| 112 | `CUserLocal::CheckPortal_Collision` (0x919a10)<br>`CUserLocal::HandleUpKeyDown` (0x919e50) |
| 113 | `CUserLocal::TryRegisterTeleport` (0x913690) |
| 114 | `CWvsContext::SendMapTransferRequest` (0x9f3b90) |
| 115 | `CWvsContext::SendAntiMacroItemUseRequest` (0x9ff270) |
| 116 | `CUserLocal::DoAntiMacroSkill` (0x940f20) |
| 117 | `CUIAdminAntiMacro::SetRet` (0x78dd80)<br>`CUIAntiMacro::SetRet` (0x78c940) |
| 118 | `CWvsContext::SendClaimRequest` (0xa05fb0) |
| 119 | `CQuest::OnCompleteQuestFailed` (0x6b1fc0)<br>`CQuest::StartQuest` (0x6b40a0)<br>`CWvsContext::ResignQuest` (0x9f3cf0) |
| 120 | `CWvsContext::CheckDarkForce` (0x9f4070)<br>`CWvsContext::CheckDragonFury` (0x9f42c0)<br>`CWvsContext::OnTemporaryStatReset` (0x9f2ab0)<br>`CWvsContext::OnTemporaryStatSet` (0xa02fc0) |
| 121 | `CUserLocal::ThrowGrenade` (0x90df50) |
| 122 | `CMacroSysMan::FlushToSvr` (0x60ea20) |
| 123 | `CWvsContext::SendSelectNpcItemUseRequest` (0x9da430) |
| 124 | `CWvsContext::SendLotteryItemUseRequest` (0x9d6c50) |
| 125 | `CUIItemMaker::RequestItemMake` (0x7d58d0) |
| 126 | `CField::SendChatMsgSlash` (0x5408e0) |
| 127 | `CWvsContext::SendUseBoxGachaponItemRequest` (0x9d6bc0) |
| 128 | `CWvsContext::SendUseGachaponRemoteRequest` (0x9d7340) |
| 129 | `CWvsContext::SendWaterOfLife` (0x9f28e0) |
| 130 | `CRepairDurabilityDlg::SendRepairDurabilityAll` (0x6d37b0) |
| 131 | `CRepairDurabilityDlg::SendRepairDurability` (0x6d3980) |
| 132 | `CDamageMeter::SaveDamageInfo` (0x4f10d0) |
| 133 | `CUserLocal::UpdateClientTimer` (0x90e6d0) |
| 134 | `CWvsContext::SendFollowCharacterRequest` (0x9f9530) |
| 136 | `CUIPQReward::SelectReward` (0x81cbb0) |
| 137 | `CUIPQReward::RequestReward` (0x81cad0) |
| 138 | `CWvsContext::OnSetPassenserRequest` (0x9fb090)<br>`CWvsContext::SendFollowRequestApply` (0x9f4690) |
| 139 | `CField::SendChatMsgSlash` (0x5408e0) |
| 140 | `CUIStatusBar::SendGroupMessage` (0x87f7f0) |
| 141 | `CField::OnWhisper` (0x5448a0)<br>`CField::SendChatMsgWhisper` (0x53d3b0)<br>`CField::SendLocationWhisper` (0x534150) |
| 142 | `CUIStatusBar::SendCoupleMessage` (0x87b3e0) |
| 143 | `CFadeWnd::SendCloseMessage` (0x524180)<br>`CUIMessenger::OnCreate` (0x7f59d0)<br>`CUIMessenger::OnDestroy` (0x7f03f0)<br>`CUIMessenger::OnInvite` (0x7f2cb0) …（共 7 个函数） |
| 144 | `??_ECMemoryGameDlg@@W7AEPAXI@Z` (0x627e20)<br>`CCashTradingRoomDlg::OnTrade` (0x49cfd0)<br>`CCashTradingRoomDlg::PutItem` (0x49d210)<br>`CCashTradingRoomDlg::PutMoney` (0x49d480) …（共 57 个函数） |
| 145 | `CField::SendChangePartyBossMsg` (0x530370)<br>`CField::SendCreateNewPartyMsg` (0x52ebc0)<br>`CField::SendJoinPartyMsg` (0x534310)<br>`CField::SendKickPartyMsg` (0x530140) …（共 6 个函数） |
| 146 | `CFadeWnd::SendCloseMessage` (0x524180)<br>`CUIFadeYesNo::OnButtonClicked` (0x529c60)<br>`CWvsContext::OnPartyResult` (0xa10ab0) |
| 147 | `ExpeditionIntermediary::OnPacketExpNoti_Invite` (0x522220)<br>`ExpeditionIntermediary::SendExpChangeBossPacket` (0x521530)<br>`ExpeditionIntermediary::SendExpChangeMasterPacket` (0x5214a0)<br>`ExpeditionIntermediary::SendExpCreatePacket` (0x5216f0) …（共 9 个函数） |
| 148 | `CUIFadeYesNo::OnButtonClicked` (0x529c60)<br>`CUIFadeYesNo::SendCloseMessage` (0x526b30)<br>`CWvsContext::OnPartyResult` (0xa10ab0)<br>`TabPartyAdver::RequestAdverRemoveFromNotiList` (0x812ca0) …（共 8 个函数） |
| 149 | `CField::InputGuildName` (0x5347d0)<br>`CField::SendCreateGuildAgreeMsg` (0x52d780)<br>`CField::SendInviteGuildMsg` (0x5348e0)<br>`CField::SendKickGuildMsg` (0x534cb0) …（共 11 个函数） |
| 150 | `CFadeWnd::SendCloseMessage` (0x524180)<br>`CWvsContext::OnGuildResult` (0xa0d3b0) |
| 151 | `CField::SendChatMsgSlash` (0x5408e0)<br>`CStage::OnSetField` (0x71a0a0)<br>`CUserLocal::OnKey` (0x9369f0) |
| 152 | `CField::SendChatMsgSlash` (0x5408e0) |
| 153 | `CField::SendAcceptFriendMsg` (0x52f290)<br>`CField::SendDeleteFriendMsg` (0x52f170)<br>`CField::SendSetFriendMsg` (0x535240)<br>`CWvsContext::LoadFriend` (0xa10240) |
| 154 | `CCashShop::OnCashItemResLoadGiftDone` (0x496520)<br>`CMemoListDlg::SetRet` (0x624280)<br>`CWvsContext::OnMemoNotify_Receive` (0x9f3830) |
| 156 | `CField::TryEnterTownPortal` (0x52f970)<br>`CTownPortalPool::TryEnterTownPortal` (0x7624b0) |
| 157 | `COpenGatePool::TryEnterOpenGate` (0x689860) |
| 158 | `CField::SendChatMsgSlash` (0x5408e0) |
| 159 | `CFuncKeyMappedMan::ChangePetConsumeItemID` (0x568920)<br>`CFuncKeyMappedMan::ChangePetConsumeMPItemID` (0x5689c0)<br>`CFuncKeyMappedMan::SaveFuncKeyMap` (0x568a60) |
| 160 | `CRPSGameDlg::OnBtContinue` (0x6d6900)<br>`CRPSGameDlg::OnBtExit` (0x6d6a40)<br>`CRPSGameDlg::OnBtRetry` (0x6d69a0)<br>`CRPSGameDlg::OnBtStart` (0x6d6860) …（共 6 个函数） |
| 161 | `CEngageDlg::SetRet` (0x51a680)<br>`CWvsContext::OnMarriageRequest` (0xa00bb0)<br>`CWvsContext::SendEngagementRequest` (0x9e1410)<br>`CWvsContext::SendInvitationQuery` (0x9da630) …（共 7 个函数） |
| 162 | `??_ECWishListInputDlg@@W7AEPAXI@Z` (0x9a6dd0)<br>`CWishListGiveDlg::SendPutItemRequest` (0x9a7140)<br>`CWishListRecvDlg::SendGetItemRequest` (0x9aba50) |
| 163 | `CField_Wedding::OnWeddingProgress` (0x5640f0) |
| 164 | `CField_Wedding::OnWeddingProgress` (0x5640f0) |
| 165 | `CWvsContext::SendBoobyTrapAlert` (0xa09680) |
| 166 | `CUIMiniMap::OnMouseButton` (0x7f8410) |
| 167 | `CTabGuildAlliance::OnChangeMaster` (0x8ba670)<br>`CTabGuildAlliance::OnGradeChange` (0x8ba5b0)<br>`CTabGuildAlliance::OnInvite` (0x8cc9c0)<br>`CTabGuildAlliance::OnKick` (0x8bf710) …（共 10 个函数） |
| 168 | `CFadeWnd::SendCloseMessage` (0x524180)<br>`CWvsContext::OnAllianceResult` (0xa0ef60) |
| 169 | `CWvsContext::SendFamilyChartRequest` (0xa09d20) |
| 170 | `CWvsContext::SendFamilyInfoRequest` (0xa09860) |
| 171 | `CWvsContext::SendRegisterJunior` (0xa09dd0) |
| 172 | `CWvsContext::SendUnregisterJunior` (0xa099e0) |
| 173 | `CWvsContext::SendUnregisterParent` (0xa098d0) |
| 174 | `CWvsContext::SendFamilyInviteResult` (0xa09c50) |
| 175 | `CWvsContext::SendUseFamilyPrivilege` (0xa09ee0) |
| 176 | `CWvsContext::SendSetFamilyPrecept` (0xa0a0d0) |
| 177 | `CWvsContext::OnFamilySummonRequest` (0xa0b0a0) |
| 179 | `CUIGuildBBS::OnComment` (0x7c4530)<br>`CUIGuildBBS::OnCommentDelete` (0x7c3b70)<br>`CUIGuildBBS::OnDelete` (0x7c6520)<br>`CUIGuildBBS::OnRegister` (0x7c4250) …（共 6 个函数） |
| 180 | `CWvsContext::SendMigrateToITCRequest` (0x9def50) |
| 181 | `CWvsContext::SendExpUpItemUseRequest` (0x9db1c0) |
| 182 | `CWvsContext::SendTempExpUseRequest` (0x9db430) |
| 183 | `CUIFadeYesNo::OnButtonClicked` (0x529c60)<br>`CUINewYearCardSenderDlg::_SendNewYearCard` (0xa1d2a0) |
| 184 | `CUIRandomMorphDlg::_SendMorphRequest` (0xa1ec70) |
| 185 | `CUICashItemGachapon::OnButtonClicked` (0x96aa40) |
| 186 | `CUICashGachapon::OnButtonClicked` (0x97a330) |
| 187 | `CCashShop::SendChangeMaplePoint` (0x488580) |
| 188 | `CUserLocal::HandleLButtonDblClk` (0x933b90) |
| 189 | `CUserLocal::RequestIncCombo` (0x909070) |
| 190 | `CMobPool::OnMobCrcKeyChanged` (0x657230) |
| 191 | `CWvsContext::SendRequestSessionValue` (0x9e1a90) |
| 192 | `CWvsContext::CheckOpBoardHasNew` (0x9d4870) |
| 193 | `CUIAccountMoreInfo::SendLoadAccountMoreInfoRequest` (0x76b190)<br>`CUIAccountMoreInfo::SendSaveAccountMoreInfoRequest` (0x76b210) |
| 194 | `CUIFindFriend::SendMyInfoRequest` (0x7b8000)<br>`CUIFindFriend::SendSearchRequest` (0x7bb4c0)<br>`CUIFindFriendDetail::SetDetailInfo` (0x7b8a80) |
| 195 | `CUserLocal::OnAskAPSPEvent` (0x90f0e0) |
| 196 | `CWvsContext::SendDragonBallBoxRequest` (0x9d73d0) |
| 197 | `CWvsContext::SendDragonBallSummonRequest` (0x9d7440) |
| 199 | `CVecCtrlPet::EndUpdateActive` (0x99f5a0) |
| 200 | `CPet::DoAction` (0x6a2340) |
| 201 | `CPet::ParseCommand` (0x6a3cc0) |
| 202 | `CPet::SendDropPickUpRequest` (0x6a0820) |
| 203 | `CWvsContext::SendStatChangeItemUseRequestByPetQ` (0x9de400) |
| 204 | `CPet::SendUpdateExceptionListRequest` (0x6a0dd0) |
| 207 | `CVecCtrlSummoned::EndUpdateActive` (0x9a0700) |
| 209 | `CSummoned::SetDamaged` (0x74b730) |
| 210 | `CSummoned::TryDoingGiveBuff` (0x74af50)<br>`CSummoned::TryDoingHeal` (0x74ad90)<br>`CSummoned::TryDoingHealingRobot` (0x74b3e0)<br>`CSummoned::TryDoingSummon` (0x74b640) |
| 211 | `CSummoned::SendRemove` (0x74c170) |
| 214 | `CVecCtrlDragon::EndUpdateActive` (0x996570) |
| 216 | `CQuickslotKeyMappedMan::SaveQuickslotKeyMap` (0x6c60a0) |
| 217 | `CUserLocal::UpdatePassiveSkillData` (0x918c20) |
| 218 | `CWvsContext::UI_Menu` (0x9e8120) |
| 219 | `CUserLocal::TryDoingMagicAttack` (0x92a240) |
| 220 | `CWvsContext::AskWhetherUsePamsSong` (0x9d74f0) |
| 221 | `CUIQuestInfoDetail::OnButtonClicked` (0x834c90) |
| 222 | `CUserLocal::SendRepeatEffectRemoveRequest` (0x909d70) |
| 227 | `CMob::GenerateMovePath` (0x651100) |
| 228 | `CMob::ApplyControl` (0x640d20) |
| 229 | `CMob::SendDropPickUpRequest` (0x644450) |
| 230 | `CMob::Update` (0x654300) |
| 231 | `CMob::Update` (0x654300) |
| 232 | `CMob::TryFirstSelfDestruction` (0x640ee0) |
| 233 | `CMob::SetDamagedByMob` (0x64b260) |
| 234 | `CMob::Update` (0x654300) |
| 235 | `CMob::UpdateTimeBomb` (0x643c30) |
| 236 | `CMob::SendCollisionEscort` (0x641150) |
| 237 | `CMob::SendRequestEscortPath` (0x6411f0) |
| 238 | `CMob::SendEscortStopEndRequest` (0x641290) |
| 241 | `CNpc::GenerateMovePath` (0x671590) |
| 242 | `CNpc::RequestSpecialAction` (0x673bc0) |
| 246 | `CWvsContext::SendDropPickUpRequest` (0x9d5d50) |
| 249 | `CReactorPool::FindHitReactor` (0x6cd4e0)<br>`CReactorPool::FindSkillReactor` (0x6cdab0) |
| 250 | `CReactorPool::FindTouchReactorAroundLocalUser` (0x6cded0) |
| 251 | `CUserLocal::ResetNLCPQ` (0x52d630) |
| 255 | `CField_SnowBall::BasicActionAttack` (0x5617b0) |
| 256 | `CField_SnowBall::Update` (0x5612d0) |
| 257 | `CField_Coconut::BasicActionAttack` (0x54a5e0) |
| 258 | `CField::SendChatMsgSlash` (0x5408e0) |
| 259 | `CField_GuildBoss::BasicActionAttack` (0x5517d0) |
| 262 | `CUIMonsterCarnival::RequestSend` (0x80b4a0) |
| 264 | `CField_ContiMove::Init` (0x54d4c0) |
| 266 | `CWvsContext::SendPartyWanted` (0xa10100) |
| 267 | `CWvsContext::SendCancelPartyWanted` (0xa0ffd0) |
| 269 | `CStage::OnSetField` (0x71a0a0) |
| 270 | `CField::OnRequestFootHoldInfo` (0x52ddd0) |
| 274 | `CCashShop::TrySendQueryCashRequest` (0x481bc0) |
| 275 | `CCashShop::ApplyWishListEvent` (0x482ea0)<br>`CCashShop::GiftWishItem` (0x491fa0)<br>`CCashShop::OnBuy` (0x48e530)<br>`CCashShop::OnBuyCouple` (0x490d80) …（共 25 个函数） |
| 276 | `CCashShop::OnStatusCoupon` (0x487ee0) |
| 283 | `CUIRaiseWndBase::OnCreate` (0x83acc0) |
| 284 | `COutPacket::COutPacket` (0x839740)<br>`CUIRaiseWndBase::CUIRaiseWndBase` (0x83b7c0) |
| 285 | `CUIRaiseWnd::SendPutItem` (0x838cf0) |
| 286 | `CUIRaisePieceWnd::SendPutItem` (0x838d90) |
| 291 | `CClassCompetition::SendRequestAuthKey` (0x4abf50) |
| 292 | `CUIWebEvent::SendRequestAuthKey` (0x8dcb10) |
| 296 | `CUIItemUpgrade::Update` (0x7bef50) |
| 299 | `CBattleRecordMan::RequestOnCalc` (0x470bd0) |
| 306 | `CITC::OnStatusCharge` (0x572a50) |
| 307 | `CITC::TrySendQueryCashRequest` (0x572ad0) |
| 308 | `CITC::OnBuy` (0x573270)<br>`CITC::OnBuyAuctionImm` (0x573310)<br>`CITC::OnBuyWish` (0x573660)<br>`CITC::OnBuyZzim` (0x573450) …（共 18 个函数） |
| 311 | `CUICharacterSaleDlg::SendCheckDuplicateIDPacket` (0x777d20) |
| 313 | `CUILogoutGift::OnButtonClicked` (0x7de690) |

原始数据：`evidence/opcode-v95/send_ops.json`（opcode → 函数地址集合）。

## 6. 与当前 s9（v83）协议的关系

- s9 收包枚举：`E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\net\opcodes\RecvOpcode.java`（对应本文 §5 发送表）
- s9 发包枚举：`...\SendOpcode.java`（对应本文 §1–§4 接收表）

抽样对照（同一系统在两端的编号完全不同）：

| 系统 | v95（本文） | s9 当前 |
|---|---|---|
| 联盟操作 | send 167（`CTabGuildAlliance::OnInvite/OnKick/OnWithdraw…`） | RecvOpcode.ALLIANCE_OPERATION = 0x8F (143) |
| 制造 | send 125（`CUIItemMaker::RequestItemMake`） | RecvOpcode.MAKER_SKILL = 0x71 (113) |
| 信使 | send 143（`CUIMessenger::SendInviteMsg/ProcessChat/OnDestroy`） | RecvOpcode.MESSENGER = 0x7A (122) |
| 怪物图鉴 | recv 86/87（`CWvsContext::OnMonsterBookSetCard/SetCover`） | SendOpcode.SET_CARD = 0x53 (83) / SET_COVER = 0x54 (84) |
| 制造结果 | recv 248（`CUserLocal::OnMakerResult`） | SendOpcode.MAKER_RESULT = 0xD9 (217) |

结论：两套编号**不可混用**；移植时按“系统语义 + 字段顺序”对齐，opcode 以 s9 侧协议 v2 表为准。

逐条（238 个发送 opcode + 274 个接收 opcode）的 v95 ↔ s9 对照见 **[`opcode-map-v95-vs-s9.md`](opcode-map-v95-vs-s9.md)**（含证据等级、语义修正与缺口清单）。

## 7. 缺口与限制

1. 本表来自 **GMS v95** 未加壳客户端；095 中文端（`开启游戏.vmp.exe` / PLUS2）为加壳，无法确认自定义 opcode（如 `ALLIANCE_OPERATION=143`、`MAKER_SKILL=113`、`MONSTER_BOOK 83/84` 等在 v95 上是否存在同号/同义项）。需按计划抓包校准。
2. ~~带变量 opcode 的构造点未展开（15 个函数）~~ **已收敛**：582 处 `COutPacket` 构造 xref 全部归因到 238 个 opcode（含共享 push 与截断函数二次取证），流程见 §8，结果见 `evidence/opcode-v95/send_sites_all.json`。
3. **仍缺**：普攻/近战/远程/体术四条攻击发送 opcode 无法静态取证——构造内联在 `0x9237f8`（无 `COutPacket` 构造调用、无 `push` 立即数），`0x91e780` / `0x925a00` / `0x9123c0` / `0x930710` 只调 `Encode*` / `SendPacket`。需抓包补齐（对应 s9 `SendOpcode.ATTACK_MELEE=0x2D` 等）。
4. 对话框子操作码只给出数值与处理函数名，未逐一记录包体字段；字段以各 `Decode` 实现为准（下一步 `notes/v95-decoders.md`）。
5. `CShopDlg` / `CAdminShopDlg` / `CStoreBankDlg` 的嵌套 switch 未自动展开。
6. 自动 token 匹配不可靠（仅 send 7/238、recv 90/375 精确命中，且有可证伪例：v95 recv 48 `OnQuestClear` 误配 150、282 `CSummonedPool::OnSkill` 误配 37、101 `FamilyJoinRequest` 误配 98、114 `AvatarMegaphoneRes` 误配 111），映射表以人工语义裁定为准，自动结果只作线索。

## 8. 复现

```bash
# ---- 接收侧 ----
# 顶层/上下文/地图分发（已归档）
cat evidence/opcode-v95/dec_processpacket.c
cat evidence/opcode-v95/dec_cwvscontext_onpacket.c
cat evidence/opcode-v95/dec_cfield_onpacket.c
# 二级分发 dump
python E:/project/_ms_probe/extract_cases.py    # -> dec_maps.json / extract_out*.txt
python E:/project/_ms_probe/gen_opcode_doc.py   # -> 本文 §1–§4

# ---- 发送侧（xref -> 逐函数反编译 -> 常量/变量解析 -> 截断补采 -> 合并）----
python E:/project/_ms_probe/batch_send.py          # 582 处 ??0COutPacket@@QAE@J@Z xref -> dec_send/*.json
python E:/project/_ms_probe/resolve_dyn.py         # 常量/赋值/LOBYTE 解析 -> ctor_sites.json
python E:/project/_ms_probe/extract_trunc_send.py  # 首轮反编译被截断的 8 个函数 -> trunc_ctor_sites.json
python E:/project/_ms_probe/extract_trunc2.py      # 第二轮补采 3 个函数 -> trunc2_ctor_sites.json
python E:/project/_ms_probe/merge_send.py          # 合并 + 同函数共享 push 回溯 -> send_sites_all.json / send_ops.json
python E:/project/_ms_probe/gen_send_section.py    # -> 本文 §5

# ---- 映射表 ----
python E:/project/_ms_probe/extract_s9.py          # s9 枚举 -> s9_opcodes.json / s9_list.txt
python E:/project/_ms_probe/build_map2.py          # 语义候选打分 -> map_candidates2.json
python E:/project/_ms_probe/gen_opcode_map.py      # -> notes/opcode-map-v95-vs-s9.md
```

中间产物均在 `E:/project/_ms_probe/`，归档副本在 `docs/features/port-095/evidence/opcode-v95/`（含 `scripts/` 与 `hashes.txt`）。
