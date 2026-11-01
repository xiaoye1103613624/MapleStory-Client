void __thiscall CWvsContext::OnPacket(CWvsContext *this, int nType, CInPacket *iPacket)
{
  switch ( nType ) /*0x9e5840*/
  {
    case 28: /*0x9e5840*/
      CWvsContext::OnInventoryOperation(this, iPacket); /*0x9e584c*/
      break; /*0x9e5851*/
    case 29: /*0x9e5840*/
      CWvsContext::OnInventoryGrow(this, iPacket); /*0x9e5859*/
      break; /*0x9e585e*/
    case 30: /*0x9e5840*/
      CWvsContext::OnStatChanged(this, iPacket); /*0x9e5866*/
      break; /*0x9e586b*/
    case 31: /*0x9e5840*/
      CWvsContext::OnTemporaryStatSet(this, iPacket); /*0x9e5873*/
      break; /*0x9e5878*/
    case 32: /*0x9e5840*/
      CWvsContext::OnTemporaryStatReset(this, iPacket); /*0x9e5880*/
      break; /*0x9e5885*/
    case 33: /*0x9e5840*/
      CWvsContext::OnForcedStatSet(this, iPacket); /*0x9e588d*/
      break; /*0x9e5892*/
    case 34: /*0x9e5840*/
      CWvsContext::OnForcedStatReset(this, iPacket); /*0x9e589a*/
      break; /*0x9e589f*/
    case 35: /*0x9e5840*/
      CWvsContext::OnChangeSkillRecordResult(this, iPacket); /*0x9e58a7*/
      break; /*0x9e58ac*/
    case 36: /*0x9e5840*/
      CWvsContext::OnSkillUseResult(this, iPacket); /*0x9e58b4*/
      break; /*0x9e58b9*/
    case 37: /*0x9e5840*/
      CWvsContext::OnGivePopularityResult(this, iPacket); /*0x9e58c1*/
      break; /*0x9e58c6*/
    case 38: /*0x9e5840*/
      CWvsContext::OnMessage(this, iPacket); /*0x9e5943*/
      break; /*0x9e5948*/
    case 39: /*0x9e5840*/
      CWvsContext::OnOpenFullClientDownloadLink(this, iPacket); /*0x9e5950*/
      break; /*0x9e5955*/
    case 40: /*0x9e5840*/
      CWvsContext::OnMemoResult(this, iPacket); /*0x9e595d*/
      break; /*0x9e5962*/
    case 41: /*0x9e5840*/
      CWvsContext::OnMapTransferResult(this, iPacket); /*0x9e596a*/
      break; /*0x9e596f*/
    case 42: /*0x9e5840*/
      CWvsContext::OnAntiMacroResult(this, iPacket); /*0x9e5977*/
      break; /*0x9e597c*/
    case 44: /*0x9e5840*/
      CWvsContext::OnClaimResult(this, iPacket); /*0x9e599e*/
      break; /*0x9e59a3*/
    case 45: /*0x9e5840*/
      CWvsContext::OnSetClaimSvrAvailableTime(this, iPacket); /*0x9e59ab*/
      break; /*0x9e59b0*/
    case 46: /*0x9e5840*/
      CWvsContext::OnClaimSvrStatusChanged(this, iPacket); /*0x9e59b8*/
      break; /*0x9e59bd*/
    case 47: /*0x9e5840*/
      CWvsContext::OnSetTamingMobInfo(this, iPacket); /*0x9e5a06*/
      break; /*0x9e5a0b*/
    case 48: /*0x9e5840*/
      CWvsContext::OnQuestClear(this, iPacket); /*0x9e5984*/
      break; /*0x9e5989*/
    case 49: /*0x9e5840*/
      CWvsContext::OnEntrustedShopCheckResult(this, iPacket); /*0x9e59c5*/
      break; /*0x9e59ca*/
    case 50: /*0x9e5840*/
      CWvsContext::OnSkillLearnItemResult(this, iPacket); /*0x9e5a13*/
      break; /*0x9e5a18*/
    case 51: /*0x9e5840*/
      CWvsContext::OnSkillResetItemResult(this, iPacket); /*0x9e5a20*/
      break; /*0x9e5a25*/
    case 52: /*0x9e5840*/
      CWvsContext::OnGatherItemResult(this, iPacket); /*0x9e5a2d*/
      break; /*0x9e5a32*/
    case 53: /*0x9e5840*/
      CWvsContext::OnSortItemResult(this, iPacket); /*0x9e5a3a*/
      break; /*0x9e5a3f*/
    case 55: /*0x9e5840*/
      CWvsContext::OnSueCharacterResult(this, iPacket); /*0x9e5991*/
      break; /*0x9e5996*/
    case 57: /*0x9e5840*/
      CWvsContext::OnTradeMoneyLimit(this, iPacket); /*0x9e5a47*/
      break; /*0x9e5a4c*/
    case 58: /*0x9e5840*/
      CWvsContext::OnSetGender(this, iPacket); /*0x9e5a54*/
      break; /*0x9e5a59*/
    case 59: /*0x9e5840*/
      CWvsContext::OnGuildBBSPacket(this, iPacket); /*0x9e5b0a*/
      break; /*0x9e5b0f*/
    case 61: /*0x9e5840*/
      CWvsContext::OnCharacterInfo(this, iPacket); /*0x9e58ce*/
      break; /*0x9e58d3*/
    case 62: /*0x9e5840*/
      CWvsContext::OnPartyResult(this, iPacket); /*0x9e58db*/
      break; /*0x9e58e0*/
    case 64: /*0x9e5840*/
      CWvsContext::OnExpedtionResult(this, iPacket); /*0x9e58e8*/
      break; /*0x9e58ed*/
    case 65: /*0x9e5840*/
      CWvsContext::OnFriendResult(this, iPacket); /*0x9e590f*/
      break; /*0x9e5914*/
    case 67: /*0x9e5840*/
      CWvsContext::OnGuildResult(this, iPacket); /*0x9e58f5*/
      break; /*0x9e58fa*/
    case 68: /*0x9e5840*/
      CWvsContext::OnAllianceResult(this, iPacket); /*0x9e5902*/
      break; /*0x9e5907*/
    case 69: /*0x9e5840*/
      CWvsContext::OnTownPortal(this, iPacket); /*0x9e591c*/
      break; /*0x9e5921*/
    case 70: /*0x9e5840*/
      CWvsContext::OnOpenGate(this, iPacket); /*0x9e5929*/
      break; /*0x9e592e*/
    case 71: /*0x9e5840*/
      CWvsContext::OnBroadcastMsg(this, iPacket); /*0x9e5936*/
      break; /*0x9e593b*/
    case 72: /*0x9e5840*/
      CWvsContext::OnIncubatorResult(this, iPacket); /*0x9e59d2*/
      break; /*0x9e59d7*/
    case 73: /*0x9e5840*/
      CWvsContext::OnShopScannerResult(this, iPacket); /*0x9e59df*/
      break; /*0x9e59e4*/
    case 74: /*0x9e5840*/
      CWvsContext::OnShopLinkResult(iPacket); /*0x9e59ec*/
      break; /*0x9e59f1*/
    case 75: /*0x9e5840*/
      CWvsContext::OnMarriageRequest(this, iPacket); /*0x9e5aa2*/
      break; /*0x9e5aa7*/
    case 76: /*0x9e5840*/
      CWvsContext::OnMarriageResult(this, iPacket); /*0x9e5aaf*/
      break; /*0x9e5ab4*/
    case 77: /*0x9e5840*/
      CWvsContext::OnWeddingGiftResult(this, iPacket); /*0x9e5abc*/
      break; /*0x9e5ac1*/
    case 78: /*0x9e5840*/
      CWvsContext::OnNotifyMarriedPartnerMapTransfer(this, iPacket); /*0x9e5ac9*/
      break; /*0x9e5ace*/
    case 79: /*0x9e5840*/
      CWvsContext::OnCashPetFoodResult(this, iPacket); /*0x9e59f9*/
      break; /*0x9e59fe*/
    case 80: /*0x9e5840*/
      CWvsContext::OnSetWeekEventMessage(this, iPacket); /*0x9e5a61*/
      break; /*0x9e5a66*/
    case 81: /*0x9e5840*/
      CWvsContext::OnSetPotionDiscountRate(this, iPacket); /*0x9e5a6e*/
      break; /*0x9e5a73*/
    case 82: /*0x9e5840*/
      CWvsContext::OnBridleMobCatchFail(this, iPacket); /*0x9e5ae3*/
      break; /*0x9e5ae8*/
    case 83: /*0x9e5840*/
      CWvsContext::OnImitatedNPCResult(this, iPacket); /*0x9e5a7b*/
      break; /*0x9e5a80*/
    case 84: /*0x9e5840*/
      CWvsContext::OnImitatedNPCData(this, iPacket); /*0x9e5a88*/
      break; /*0x9e5a8d*/
    case 85: /*0x9e5840*/
      CWvsContext::OnLimitedNPCDisableInfo(this, iPacket); /*0x9e5a95*/
      break; /*0x9e5a9a*/
    case 86: /*0x9e5840*/
      CWvsContext::OnMonsterBookSetCard(iPacket); /*0x9e5af0*/
      break; /*0x9e5af5*/
    case 87: /*0x9e5840*/
      CWvsContext::OnMonsterBookSetCover(iPacket); /*0x9e5afd*/
      break; /*0x9e5b02*/
    case 88: /*0x9e5840*/
      CWvsContext::OnHourChanged(this, iPacket); /*0x9e5bb3*/
      break; /*0x9e5bb8*/
    case 89: /*0x9e5840*/
      CWvsContext::OnMiniMapOnOff(this, iPacket); /*0x9e5bc0*/
      break; /*0x9e5bc5*/
    case 90: /*0x9e5840*/
      CWvsContext::OnConsultAuthkeyUpdate(this, iPacket); /*0x9e5bcd*/
      break; /*0x9e5bd2*/
    case 91: /*0x9e5840*/
      CWvsContext::OnClassCompetitionAuthkeyUpdate(this, iPacket); /*0x9e5bda*/
      break; /*0x9e5bdf*/
    case 92: /*0x9e5840*/
      CWvsContext::OnWebBoardAuthkeyUpdate(this, iPacket); /*0x9e5be7*/
      break; /*0x9e5bec*/
    case 93: /*0x9e5840*/
      CWvsContext::OnSessionValue(this, iPacket); /*0x9e5bf4*/
      break; /*0x9e5bf9*/
    case 94: /*0x9e5840*/
      CWvsContext::OnPartyValue(this, iPacket); /*0x9e5c01*/
      break; /*0x9e5c06*/
    case 95: /*0x9e5840*/
      CWvsContext::OnFieldSetVariable(this, iPacket); /*0x9e5c0e*/
      break; /*0x9e5c13*/
    case 96: /*0x9e5840*/
      CWvsContext::OnBonusExpRateChanged(this, iPacket); /*0x9e5c1b*/
      break; /*0x9e5c20*/
    case 97: /*0x9e5840*/
      CWvsContext::OnPotionDiscountRateChanged(this, iPacket); /*0x9e5c28*/
      break; /*0x9e5c2d*/
    case 98: /*0x9e5840*/
      CWvsContext::OnFamilyChartResult(this, iPacket); /*0x9e5c35*/
      break; /*0x9e5c3a*/
    case 99: /*0x9e5840*/
      CWvsContext::OnFamilyInfoResult(this, iPacket); /*0x9e5c42*/
      break; /*0x9e5c47*/
    case 100: /*0x9e5840*/
      CWvsContext::OnFamilyResult(this, iPacket); /*0x9e5c4f*/
      break; /*0x9e5c54*/
    case 101: /*0x9e5840*/
      CWvsContext::OnFamilyJoinRequest(this, iPacket); /*0x9e5c5c*/
      break; /*0x9e5c61*/
    case 102: /*0x9e5840*/
      CWvsContext::OnFamilyJoinRequestResult(this, iPacket); /*0x9e5c69*/
      break; /*0x9e5c6e*/
    case 103: /*0x9e5840*/
      CWvsContext::OnFamilyJoinAccepted(this, iPacket); /*0x9e5c76*/
      break; /*0x9e5c7b*/
    case 104: /*0x9e5840*/
      CWvsContext::OnFamilyPrivilegeList(this, iPacket); /*0x9e5c83*/
      break; /*0x9e5c88*/
    case 105: /*0x9e5840*/
      CWvsContext::OnFamilyFamousPointIncResult(iPacket); /*0x9e5c90*/
      break; /*0x9e5c95*/
    case 106: /*0x9e5840*/
      CWvsContext::OnFamilyNotifyLoginOrLogout(this, iPacket); /*0x9e5c9d*/
      break; /*0x9e5ca2*/
    case 107: /*0x9e5840*/
      CWvsContext::OnFamilySetPrivilege(this, iPacket); /*0x9e5caa*/
      break; /*0x9e5caf*/
    case 108: /*0x9e5840*/
      CWvsContext::OnFamilySummonRequest(this, iPacket); /*0x9e5cb7*/
      break; /*0x9e5cbc*/
    case 109: /*0x9e5840*/
      CWvsContext::OnNotifyLevelUp(this, iPacket); /*0x9e5cc4*/
      break; /*0x9e5cc9*/
    case 110: /*0x9e5840*/
      CWvsContext::OnNotifyWedding(this, iPacket); /*0x9e5cd1*/
      break; /*0x9e5cd6*/
    case 111: /*0x9e5840*/
      CWvsContext::OnNotifyJobChange(this, iPacket); /*0x9e5cde*/
      break; /*0x9e5ce3*/
    case 113: /*0x9e5840*/
      CWvsContext::OnMapleTVUseRes(this, iPacket); /*0x9e5b17*/
      break; /*0x9e5b1c*/
    case 114: /*0x9e5840*/
      CWvsContext::OnAvatarMegaphoneRes(this, iPacket); /*0x9e5b24*/
      break; /*0x9e5b29*/
    case 115: /*0x9e5840*/
      CWvsContext::OnSetAvatarMegaphone(this, iPacket); /*0x9e5b31*/
      break; /*0x9e5b36*/
    case 116: /*0x9e5840*/
      CWvsContext::OnClearAvatarMegaphone(this, iPacket); /*0x9e5b3e*/
      break; /*0x9e5b43*/
    case 117: /*0x9e5840*/
      CWvsContext::OnCancelNameChangeResult(this, iPacket); /*0x9e5b4b*/
      break; /*0x9e5b50*/
    case 118: /*0x9e5840*/
      CWvsContext::OnCancelTransferWorldResult(this, iPacket); /*0x9e5b58*/
      break; /*0x9e5b5d*/
    case 119: /*0x9e5840*/
      CWvsContext::OnDestroyShopResult(this, iPacket); /*0x9e5b65*/
      break; /*0x9e5b6a*/
    case 120: /*0x9e5840*/
      CWvsContext::OnFakeGMNotice(this, iPacket); /*0x9e5b72*/
      break; /*0x9e5b77*/
    case 121: /*0x9e5840*/
      CWvsContext::OnSuccessInUsegachaponBox(this, iPacket); /*0x9e5b7f*/
      break; /*0x9e5b84*/
    case 122: /*0x9e5840*/
      CWvsContext::OnNewYearCardRes(this, iPacket); /*0x9e5b8c*/
      break; /*0x9e5b91*/
    case 123: /*0x9e5840*/
      CWvsContext::OnRandomMorphRes(this, iPacket); /*0x9e5b99*/
      break; /*0x9e5b9e*/
    case 124: /*0x9e5840*/
      CWvsContext::OnCancelNameChangebyOther(this, iPacket); /*0x9e5ba6*/
      break; /*0x9e5bab*/
    case 125: /*0x9e5840*/
      CWvsContext::OnSetBuyEquipExt(this, iPacket); /*0x9e5ceb*/
      break; /*0x9e5cf0*/
    case 126: /*0x9e5840*/
      CWvsContext::OnSetPassenserRequest(this, iPacket); /*0x9e5d1f*/
      break; /*0x9e5d24*/
    case 127: /*0x9e5840*/
      CWvsContext::OnScriptProgressMessage(this, iPacket); /*0x9e5cf8*/
      break; /*0x9e5cfd*/
    case 128: /*0x9e5840*/
      CWvsContext::OnDataCRCCheckFailed(this, iPacket); /*0x9e5d05*/
      break; /*0x9e5d0a*/
    case 129: /*0x9e5840*/
      CWvsContext::OnCakePieEventResult(this, iPacket); /*0x9e5d39*/
      break; /*0x9e5d3e*/
    case 130: /*0x9e5840*/
      CWvsContext::OnUpdateGMBoard(this, iPacket); /*0x9e5d46*/
      break; /*0x9e5d4b*/
    case 131: /*0x9e5840*/
      CWvsContext::OnShowSlotMessage(this, iPacket); /*0x9e5d12*/
      break; /*0x9e5d17*/
    case 132: /*0x9e5840*/
      CWvsContext::OnWildHunterInfo(this, iPacket); /*0x9e5d2c*/
      break; /*0x9e5d31*/
    case 133: /*0x9e5840*/
      CWvsContext::OnAccountMoreInfo(this, iPacket); /*0x9e5d53*/
      break; /*0x9e5d58*/
    case 134: /*0x9e5840*/
      CWvsContext::OnFindFirend(this, iPacket); /*0x9e5d60*/
      break; /*0x9e5d65*/
    case 135: /*0x9e5840*/
      CWvsContext::OnStageChange(this, iPacket); /*0x9e5d6d*/
      break; /*0x9e5d72*/
    case 136: /*0x9e5840*/
      CWvsContext::OnDragonBallBox(this, iPacket); /*0x9e5d94*/
      break; /*0x9e5d99*/
    case 137: /*0x9e5840*/
      CWvsContext::OnAskWhetherUsePamsSong(this, iPacket); /*0x9e5da1*/
      break; /*0x9e5da1*/
    case 138: /*0x9e5840*/
      CWvsContext::OnTransferChannel(iPacket); /*0x9e5d7a*/
      break; /*0x9e5d7f*/
    case 139: /*0x9e5840*/
      CWvsContext::OnDisallowedDeliveryQuestList(this, iPacket); /*0x9e5d87*/
      break; /*0x9e5d8c*/
    case 140: /*0x9e5840*/
      CWvsContext::OnMacroSysDataInit(iPacket); /*0x9e5ad6*/
      break; /*0x9e5adb*/
    default:
      return;
  }
}