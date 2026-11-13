# Angel 083 QUEST 面反编译语义抽取（观察层）

来源：`evidence/angel-quest-decompile.json`（全量函数逐字 Hex-Rays 伪代码，来自 Angel 13339 IDB）。
本文件由 `tools/ida_angel_quest_facts.py` 自动生成，只做字符串抽取与归类，不含推断。

## 1. 逐函数关键信号（opcode / Encode / Decode / 分支 / SP_ 节点 / 数值调用）


### <free>::0CQuest  @ 0x7163a6  (1564 B)

  - `CQuest *__thiscall CQuest::CQuest(CQuest *this, int a2, ulong a3)`
  - `memset(a1: this, a2: 0, a3: 0xCu);`
  - `*(this + 6) = a2;`
  - `*(this + 4) = a3;`
  - `*(this + 6) = 0;`
  - `LOBYTE(v11) = 5;`
  - `v4 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v7) + 4);`
  - `if ( v8 != 0 )`
  - `CurFieldID = CWvsContext::GetCurFieldID(a1: *dword_BE7918);`
  - `if ( sub_500EA5(a1: &v10) != 0 )`
  - `else if ( sub_719F5F(a1: &a2) == 0`
  - `|| CQuestMan::CheckStartDemand(`
  - `a7: *(*dword_BE7918 + 14272),`
  - `sub_7166B6(a1: this);`
  - …（截断 14 行）

### <free>::0CUIMedalQuestInfoDetail  @ 0x83d255  (1215 B)

  - `CWnd *__fastcall CUIMedalQuestInfoDetail::CUIMedalQuestInfoDetail(CWnd *a1, int a2, char a3, int a4, int a5, int a6)`
  - `CWnd::CWnd(this: a1);`
  - `dword_BF0F3C = (a1 + 108) != 0 ? a1 : 0;`
  - `*(a1 + 28) = a4;`
  - `sub_845659();`
  - `*(a1 + 30) = 0;`
  - `CUIToolTip::CUIToolTip(this: a1 + 49);`
  - `*(a1 + 1) = &off_B3994C;`
  - `*(a1 + 2) = &off_B39948;`
  - `CWnd::CreateWnd(`
  - `sub_845676(a1: &a3);`

### <free>::0CUIQuestInfo  @ 0x87e8f3  (2498 B)

  - `CUIQuestInfo *__fastcall CUIQuestInfo::CUIQuestInfo(CUIQuestInfo *this, int a2, int a3)`
  - `CUIWnd::CUIWnd(this, a2: 6, a3: 3, a4: 227, a5: 6, a6: 1, a7: 0, a8: 0);`
  - `dword_BF0FF0 = (this + 1456) != 0 ? this : 0;`
  - `*(this + 365) = 0;`
  - ``eh vector constructor iterator'(`
  - ``eh vector constructor iterator'(a1: this + 1516, a2: 4u, a3: 4, a4: sub_4738A3, a5: sub_428498);`
  - ``eh vector constructor iterator'(a1: this + 1540, a2: 4u, a3: 3, a4: sub_888A99, a5: sub_888A9F);`
  - `LOBYTE(v8) = 11;`
  - `*(this + 396) = &off_AF2074;`
  - `*(this + 369) = -1;`
  - `*(this + 368) = a3;`
  - `v5 = *(this + 354);`
  - `*(this + 1) = &off_B3A9A0;`
  - `*(this + 2) = &off_B3A99C;`
  - …（截断 14 行）

### <free>::1CUIQuestAlarm  @ 0x884d77  (4136 B)

  - `int __thiscall CUIQuestAlarm::~CUIQuestAlarm(CUIQuestAlarm *this)`
  - `*(this - 2) = &off_B3AA74;`
  - `*(this - 1) = &off_B3AA28;`
  - `v2 = *(this + 399);`
  - `v3 = (this + 1592);`
  - `if ( v2 != 0 )`
  - `v5 = &v2[*(this + 400)];`
  - `while ( v2 < v5 )`
  - `if ( *v2 != 0 )`
  - `for ( i = v6; i != 0; v4 = 0 )`
  - `v20 = *sub_888FA9(a1: &i, a2: v18);`
  - `if ( isAllConditionsSatisfied(a1: v20) == 0 )`
  - `while ( 1 )`
  - `v7 = *(this + 362);`
  - …（截断 14 行）

### <free>::AddQuestList  @ 0x724bac  (11821 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1604_NAME`, `SP_3195_DLD_SLK_R_N`, `SP_3232_QUEST_QUESTINFOIMG`, `SP_4297_IN_PROGRESS`, `SP_4299_PRE_COMPLETION_ENABLED`, `SP_5514_04D`
  - `int __cdecl AddQuestList(_DWORD *a1, int a2, _DWORD *a3, int a4)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v56, pvargSrc: &pvargSrc);`
  - `LOBYTE(v74) = 1;`
  - `v6 = sub_406472();`
  - `v7 = sub_402680(a1: v6);`
  - `IWzResMan::GetObjectA(this: v7, pvargDest: &pvarg, sUOL: v46, vParam: v47, vAux: p_pvargDest);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v49, a2: v50);`
  - `if ( Unknown != 0 )`
  - `v8 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v8 < 0 && v8 != -2147467262 )`
  - `if ( v9 < 0 )`
  - `if ( v10 < 0 )`
  - `if ( v11 < 0 )`
  - …（截断 14 行）

### <free>::_GCUIQuestAlarm  @ 0x884d53  (281 B)

  - `char *__thiscall CUIQuestAlarm::`scalar deleting destructor'(CUIQuestAlarm *this, char a2)`
  - `CUIQuestAlarm::~CUIQuestAlarm(this);`
  - `if ( (a2 & 1) != 0 )`
  - `ZAllocEx<ZAllocAnonSelector>::Free(a1: this - 8);`

### CAdminShopDlg::SendTradeRequest  @ 0x427a29  (12225 B)

- **c2s opcode `COutPacket(68)`**
- c2s 编码：`Encode1`, `Encode4`, `Encode2`
- SP_ 节点：`SP_3416_SEALED_ITEMS_CANNOT_BE_R_NSOLD_TRADED_OR_DROPPED`, `SP_4656_YOU_DO_NOT_HAVE_THE_ITEM`, `SP_5599_YOU_DO_NOT_HAVE_ENOUGH_MESOS`, `SP_848_ARE_YOU_SURE_YOU_WANT_TO_BUY_IT`, `SP_849_HOW_MANY_ARE_YOU_WILLING_TO_BUY`, `SP_850_ARE_YOU_SURE_YOU_WANT_TO_SELL_IT`, `SP_851_HOW_MANY_ARE_YOU_WILLING_TO_SELL`
  - `void *__thiscall CAdminShopDlg::SendTradeRequest(CAdminShopDlg *this)`
  - `if ( *(this + 389) == 0 )`
  - `v3 = *(this + 385);`
  - `if ( v3 != 0 )`
  - `v4 = *(this + 387);`
  - `if ( v4 < 0 )`
  - `v10 = *(this + 52);`
  - `v11 = v10 != 0 ? *(v10 - 4) : 0;`
  - `if ( v4 >= v11 )`
  - `v5 = *(this + 39);`
  - `if ( *(v5 + 60) == 0 )`
  - `v8 = v7 != 0 ? *(v7 - 4) : 0;`
  - `if ( v4 >= v8 )`
  - `sub_428A28(a1: v12);`
  - …（截断 14 行）

### CConfig::LoadQuestAlarm  @ 0x49e536  (1869 B)

- SP_ 节点：`SP_2482_QUESTALARMD`, `SP_2483_QUESTALARMCNT`
  - `void __thiscall CConfig::LoadQuestAlarm(CConfig *this)`
  - `LOBYTE(v12) = 1;`
  - `Opt_Int = CConfig::GetOpt_Int(nType: 2, sKey: v5, nDefaultValue: 0, nLowBound: 0x80000000, nHighBound: 0x7FFFFFFF);`
  - `if ( Opt_Int >= 0 )`
  - `ZArray<long>::RemoveAll(a1: v11 + 188);`
  - `if ( Opt_Int > 0 )`
  - `ZXString<char>::Format(a1: &v9, a2: v3, a3: v10[0]);`
  - `v6 = CConfig::GetOpt_Int(nType: 2, sKey: v9, nDefaultValue: 0, nLowBound: 0x80000000, nHighBound: 0x7FFFFFFF);`
  - `v4 = ZArray<long>::InsertBefore(a1: -1);`
  - `while ( *v10 < Opt_Int );`

### CDragon::UpdateQuestInfo  @ 0x4ff794  (8062 B)

- SP_ 节点：`SP_3190_UI_UIWINDOWIMG_QUESTICON_D`
  - `int __thiscall CDragon::UpdateQuestInfo(CDragon *this)`
  - `result = (*(**(this + 62) + 12))(a1: *(this + 62));`
  - `if ( result != 0 )`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v35) + 4);`
  - `v38 = *(*dword_BE7918 + 14272);`
  - `CurFieldID = CWvsContext::GetCurFieldID(a1: *dword_BE7918);`
  - `for ( i = 0; ; ++i )`
  - `v4 = *(this + 63);`
  - `if ( v4 == 0 || i >= *(v4 - 4) )`
  - `LOWORD(v4) = *(v4 + 2 * i);`
  - `if ( v4 < 0x4B0u || v4 >= 0x578u )`
  - `if ( sub_500EA5(a1: &v36) != 0 )`
  - `if ( CQuestMan::CheckCompleteDemand(this: v41, a2: v44, i: 0xF7508u, a4: v3, a5: v42) == 0 )`
  - `*(this + 64) = 2;`
  - …（截断 14 行）

### CField::SendTransferChannelRequest  @ 0x5304af  (2318 B)

- **c2s opcode `COutPacket(39)`**
- c2s 编码：`Encode1`, `Encode4`
- SP_ 节点：`SP_266_YOU_CANT_DO_IT_RIGHT_NOW_R_NCATCH_YOUR_BREATH_AND_THEN_TRY_AGAIN`, `SP_277_YOU_CANT_DO_IT_HERE_IN_THIS_MAP`
- 节流 `CanSendExclRequest(500)`
  - `int __thiscall CField::SendTransferChannelRequest(CField *this, char a2)`
  - `result = CWvsContext::CanSendExclRequest(a1: 500, a2: 0);`
  - `if ( result != 0 && TSingleton<CUserLocal>::ms_pInstance != 0 )`
  - `if ( sub_4F38D3(this: v2) == 0 && *(TSingleton<CUserLocal>::ms_pInstance + 1388) > 0 )`
  - `return CUtilDlg::Notice(sMsg: v11, sSndName: v12, ppDialog: v13, bAutoSeparated: v14, bIsTightLine: v15);`
  - `field = get_field();`
  - `if ( field != 0 && (*(field + 324) & 0x10) != 0 )`
  - `COutPacket::COutPacket(a1: 39);`
  - `COutPacket::Encode1(this: &v16, a2);`
  - `update_time = get_update_time();`
  - `COutPacket::Encode4(this: &v16, a2: update_time);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v16);`
  - `*(v2 + 8356) = 1;`
  - `v9 = get_update_time();`
  - …（截断 14 行）

### CField::SendTransferFieldRequest  @ 0x53035d  (2067 B)

- **c2s opcode `COutPacket(38)`**
- c2s 编码：`Encode1`, `Encode4`, `EncodeStr`, `Encode2`
  - `int __stdcall CField::SendTransferFieldRequest(int a1, char *a2, char a3, int a4, int a5)`
  - `if ( *(*dword_BE7918 + 8356) != 0 || get_update_time() - *(v7 + 8360) < 500 )`
  - `COutPacket::COutPacket(a1: 38);`
  - `if ( get_field() != 0 )`
  - `v9 = *(get_field() + 308);`
  - `COutPacket::Encode1(this: &v14, a2: v9);`
  - `COutPacket::Encode4(this: &v14, a2: a1);`
  - `if ( a2 == 0 )`
  - `ZXString<char>::GetBuffer(a1: v10, a2: 0xFFFFFFFF);`
  - `COutPacket::EncodeStr(this: &v14, a2: 0);`
  - `if ( a2 != 0 )`
  - `v11 = (*(*(TSingleton<CUserLocal>::ms_pInstance + 4) + 16))(a1: TSingleton<CUserLocal>::ms_pInstance + 4);`
  - `COutPacket::Encode2(this: &v14, a2: *v11);`
  - `v12 = (*(*(TSingleton<CUserLocal>::ms_pInstance + 4) + 16))(a1: TSingleton<CUserLocal>::ms_pInstance + 4);`
  - …（截断 14 行）

### CField::UpdateQuestLayer  @ 0x537cdc  (2179 B)

  - `void __thiscall CField::UpdateQuestLayer(CField *this)`
  - `CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v14);`
  - `if ( v15 != 0 )`
  - `v2 = *(this + 47);`
  - `if ( v2 != 0 )`
  - `v4 = &v2[*(v3 + 2)];`
  - `while ( v2 < v4 )`
  - `if ( *v2 != 0 )`
  - `while ( v17 != 0 )`
  - `v6 = sub_5394BE(a1: &v17, a2: v12);`
  - `ZXString<char>::operator=(a1: v6);`
  - `LOBYTE(v19) = 1;`
  - `ZXString<char>::operator=(a1: &v18);`
  - `sub_538C86();`
  - …（截断 14 行）

### CField_MonsterCarnival::OnRequestResult  @ 0x56557d  (5250 B)

- s2c 解码：`Decode1`, `DecodeStr`
- SP_ 节点：`SP_4082_YOU_DONT_HAVE_ENOUGH_CP_TO_CONTINUE`, `SP_4083_YOU_CAN_NO_LONGER_SUMMON_THE_MONSTER`, `SP_4084_YOU_CAN_NO_LONGER_SUMMON_THE_BEING`, `SP_4085_THIS_BEING_IS_ALREADY_SUMMONED`, `SP_4086_THIS_REQUEST_HAS_FAILED_DUE_TO_AN_UNKNOWN_ERROR`
  - `void __thiscall CField_MonsterCarnival::OnRequestResult(CField_MonsterCarnival *this, int a2, char **a3)`
  - `if ( a2 == 0 )`
  - `v8 = CInPacket::Decode1(this: a3);`
  - `if ( v9 != 0 )`
  - `if ( v10 != 0 )`
  - `if ( v11 != 0 )`
  - `if ( v12 != 0 )`
  - `*(*dword_BE7918 + 8356) = 0;`
  - `update_time = get_update_time();`
  - `*(v19 + 8360) = update_time;`
  - `LOBYTE(v29) = 8;`
  - `ZXString<char>::Format(a1: &a3, a2: v21, a3: v22);`
  - `if ( a3 != 0 && *a3 != 0 )`
  - `CHATLOG_ADD(sString: &a3, nType: 7u);`
  - …（截断 14 行）

### CItemInfo::IsPartyQuestItem  @ 0x5d516e  (1379 B)

- SP_ 节点：`SP_2974_PQUEST`
  - `int __thiscall CItemInfo::IsPartyQuestItem(CItemInfo *this, int a2)`
  - `CItemInfo::GetItemInfo(a1: &a2, a2);`
  - `if ( a2 == 0 )`
  - `LOBYTE(v12) = 0;`
  - `sub_40263B(a1: &a2);`
  - `Item = IWzProperty::GetItem(pvargDest: &pvarg, a2: v8);`
  - `int32 = get_int32(pvarSrc: Item, a2: v9);`
  - `if ( v6 < 0 )`
  - `if ( int32 == 0 )`
  - `if ( a2 != 0 )`
  - `(*(*a2 + 8))(a1: a2);`

### CItemInfo::IsQuestItem  @ 0x5d50b8  (1373 B)

- SP_ 节点：`SP_2973_QUEST`
  - `int __thiscall CItemInfo::IsQuestItem(CItemInfo *this, int a2)`
  - `CItemInfo::GetItemInfo(a1: &a2, a2);`
  - `if ( a2 == 0 )`
  - `LOBYTE(v12) = 0;`
  - `sub_40263B(a1: &a2);`
  - `Item = IWzProperty::GetItem(pvargDest: &pvarg, a2: v8);`
  - `int32 = get_int32(pvarSrc: Item, a2: v9);`
  - `if ( v6 < 0 )`
  - `if ( int32 == 0 )`
  - `if ( a2 != 0 )`
  - `(*(*a2 + 8))(a1: a2);`

### CItemSpeakerDlg::_SendConsumeCashItemUseRequest  @ 0x5ec4b7  (3978 B)

- **c2s opcode `COutPacket(79)`**
- c2s 编码：`Encode2`, `Encode4`, `EncodeStr`, `Encode1`
- SP_ 节点：`SP_297_OTHER_REQUESTS_ARE_IN_PROCESS_R_NPLEASE_TRY_AGAIN_IN_A_LITTLE_BIT`, `SP_4434_YOU_HAVE_NOT_SELECTED_AN_ITEM_R_NARE_YOU_SURE_YOU_STILL_WANT_TO_USE_THE_ITEM`
- 节流 `CanSendExclRequest(500)`
  - `int __thiscall CItemSpeakerDlg::_SendConsumeCashItemUseRequest(CItemSpeakerDlg *this)`
  - `CCtrlEdit::GetText(this: *(this + 366), a2: &v24);`
  - `ZXString<char>::TrimRight(a1: 0);`
  - `v2 = *ZXString<char>::TrimLeft(a1: 0);`
  - `if ( v24 != 0 )`
  - `if ( v4 )`
  - `if ( *(this + 37) == 0 )`
  - `if ( CUtilDlg::YesNo(a1: v16, a2: v17, a3: v18, a4: v19, a5: v20[0]) == 7 )`
  - `if ( *dword_BE7918 == 0 )`
  - `if ( CWvsContext::CanSendExclRequest(a1: 500, a2: 0) == 0 )`
  - `CUtilDlg::Notice(sMsg: v16, sSndName: v17, ppDialog: v18, bAutoSeparated: v19, bIsTightLine: v20[0]);`
  - `COutPacket::COutPacket(a1: 79);`
  - `v10 = *(this + 64);`
  - `COutPacket::Encode2(a1: v10);`
  - …（截断 14 行）

### CLogin::SendRequest  @ 0x5f6932  (175 B)

  - `void __thiscall CLogin::SendRequest(CLogin *this, struct COutPacket *a2)`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2);`
  - `*(this + 92) = 1;`

### CNoticeQuestProgress::NoticeProgressChange  @ 0x6ce306  (19574 B)

  - `int *__thiscall CNoticeQuestProgress::NoticeProgressChange(_DWORD *this)`
  - `if ( *(v2 - 1) != 0 )`
  - `sub_402F85(pvargDest: &v90, pvargSrc: &pvargSrc);`
  - `LOBYTE(v111) = 1;`
  - `sub_402F85(pvargDest: &v92, pvargSrc: &pvargSrc);`
  - `if ( dword_BF14EC == 0 )`
  - `v4 = IWzGr2D::CreateLayer(`
  - `sub_428712(a1: *v4);`
  - `sub_4156C5(a1: &v96);`
  - `if ( v5 < 0 )`
  - `if ( v6 < 0 )`
  - `sub_410FDF(this: &v74, a2: *(v2 - 9), a3: 1);`
  - `if ( v110 == 0 )`
  - `v8 = (*(v7 + 100))(a1: v110, a2: *&v74.vt, a3: v74.decVal.Hi32, a4: v74.lVal, a5: v74.cyVal.Hi);`
  - …（截断 14 行）

### CNoticeQuestProgress::OnQuestProgressUpdated_Item  @ 0x6cdb4e  (3484 B)

  - `void __thiscall CNoticeQuestProgress::OnQuestProgressUpdated_Item(`
  - `if ( nItemID != 0 )`
  - `CompleteDemand = CQuestMan::GetCompleteDemand(a1: usQuestID);`
  - `if ( CompleteDemand != 0 )`
  - `v5 = *(CompleteDemand + 92);`
  - `if ( v5 != 0 && *(v5 - 4) != 0 )`
  - `for ( i = 0; ; ++i )`
  - `v7 = *(v4 + 92);`
  - `if ( v7 == 0 || i >= *(v7 - 4) )`
  - `if ( nItemID == *(v7 + 8 * i) )`
  - `ItemCount = CWvsContext::GetItemCount(a1: *(*(v4 + 92) + 8 * i));`
  - `v9 = *(*(v4 + 92) + v8 + 4);`
  - `ItemName = CItemInfo::GetItemName(a1: v16, a2: nItemID);`
  - `LOBYTE(v19) = 1;`
  - …（截断 14 行）

### CNoticeQuestProgress::OnQuestProgressUpdated_Mob  @ 0x6cdd82  (10802 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1604_NAME`, `SP_2240_STRING_MOBIMG`
  - `void __thiscall CNoticeQuestProgress::OnQuestProgressUpdated_Mob(char *this, int a2, int a3)`
  - `CompleteDemand = CQuestMan::GetCompleteDemand(a1: a2);`
  - `if ( CompleteDemand == 0 || (v65 = 0, (v7 = *(CompleteDemand + 96)) == 0) || *(v7 - 4) == 0 )`
  - `if ( !v6 )`
  - `LOBYTE(v68) = 2;`
  - `v8 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v49) + 4);`
  - `if ( sub_4E901B(this: (v8 + 1535), a2: &i, a3: &v66) == 0 )`
  - `if ( v66 != 0 )`
  - `if ( v65 != 0 )`
  - `while ( 1 )`
  - `v10 = *(v4 + 96);`
  - `if ( v10 == 0 || v57 >= *(v10 - 4) )`
  - `v11 = *(v66 - 4);`
  - `if ( v11 >= v64 )`
  - …（截断 14 行）

### CNoticeQuestProgress::OnQuestProgressUpdated_Script  @ 0x6ce251  (986 B)

  - `void __fastcall CNoticeQuestProgress::OnQuestProgressUpdated_Script(int a1, int a2, _BYTE *sMsg)`
  - `if ( sMsg != 0 && *sMsg != 0 )`
  - `v5 = (a1 + 128);`
  - `if ( *v5 + 3000 < get_update_time() )`
  - `while ( v4 < 5 );`
  - `if ( v4 < 5 )`
  - `*(v6 + 116) = 0;`
  - `sub_414663(a1: a1 + 44 * (v4 + 3));`
  - `*(v6 + 128) = get_update_time();`
  - `ZXString<char>::operator=(a1: &sMsg);`

### CNoticeQuestProgress::Update  @ 0x6cec47  (169 B)

  - `int *__thiscall CNoticeQuestProgress::Update(_DWORD *this)`
  - `CWnd::Update(this);`
  - `return CNoticeQuestProgress::NoticeProgressChange(this);`

### CNpc::GetQuestDCRange  @ 0x6d4029  (834 B)

  - `_DWORD *__thiscall CNpc::GetQuestDCRange(CNpc *this)`
  - `v2 = (this + 368);`
  - `dword_BF0410(a1: this + 368);`
  - `if ( *(this + 59) != 0 )`
  - `*v2 = *(this + 84) + sub_425AEF(a1: *(this + 59));`
  - `if ( *(this + 59) == 0 )`
  - `*(this + 93) = *(this + 85) + sub_425B16(a1: *(this + 59));`
  - `*(this + 94) = *v2 + IWzGr2DLayer::GetWidth(a1: *(this + 59));`
  - `*(this + 95) = *(this + 93) + IWzGr2DLayer::GetHeight(a1: *(this + 59));`

### CNpc::SetClientActionByQuest  @ 0x6d1b11  (10252 B)

  - `void __thiscall CNpc::SetClientActionByQuest(CNpc *this)`
  - `v2 = *(this + 79);`
  - `v3 = *(this + 42);`
  - `*(this + 80) = v2;`
  - `v4 = sub_6DEF51(a1: v3);`
  - `v5 = v4 == *(this + 80);`
  - `*(this + 79) = v4;`
  - `if ( !v5 )`
  - `CNpc::PrepareActionLayer(this);`
  - `if ( *(sub_41A450(a1: *(this + 79)) + 28) != 0 && *(this + 79) >= 0 )`
  - `*(this + 81) = 1;`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `v5 = *(this + 57) == 0;`
  - …（截断 14 行）

### CNpc::SetQuestList  @ 0x6d1779  (7251 B)

- SP_ 节点：`SP_3190_UI_UIWINDOWIMG_QUESTICON_D`
  - `void __thiscall CNpc::SetQuestList(CNpc *this)`
  - `ZArray<unsigned short>::RemoveAll(a1: this + 204);`
  - `v2 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v36) + 4);`
  - `v38 = *(*dword_BE7918 + 14272);`
  - `CurFieldID = CWvsContext::GetCurFieldID(a1: *dword_BE7918);`
  - `for ( i = 0; ; ++i )`
  - `v3 = *(this + 49);`
  - `if ( v3 == 0 || i >= *(v3 - 4) )`
  - `LOWORD(v3) = *(v3 + 2 * i);`
  - `if ( v3 < 0x4B0u || v3 >= 0x578u )`
  - `if ( sub_500EA5(a1: &v37) != 0 )`
  - `CQuestMan::CheckCompleteDemand(this: v40, a2: v43, i: **(this + 42), a4: v2, a5: v41);`
  - `v4 = sub_500E4C(a1: &v43, a2: 0);`
  - `v7 = *(this + 42);`
  - …（截断 14 行）

### CNpc::ShowQuestList  @ 0x6d3392  (27614 B)

- **c2s opcode `COutPacket(58)`**
- c2s 编码：`Encode4`, `Encode2`
- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1735_AREA`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_3191__R_N_R_NFUI_UIWINDOWIMG_UTILDLGEX_LIST1_R_N`, `SP_3192__R_N_R_NFUI_UIWINDOWIMG_UTILDLGEX_LIST0_R_N`, `SP_3193__R_N_R_NFUI_UIWINDOWIMG_UTILDLGEX_LIST3_R_N`, `SP_3194__R_N_R_NFUI_UIWINDOWIMG_UTILDLGEX_LIST2_R_N`, `SP_3195_DLD_SLK_R_N`, `SP_3196_ETC_SCRIPTINFOIMG`, `SP_3232_QUEST_QUESTINFOIMG`, `SP_4418_NO_TARGET_AVAILABLE`, `SP_5514_04D`
  - `void __thiscall CNpc::ShowQuestList(CNpc *this)`
  - `v2 = *(this + 77) == 0;`
  - `if ( !v2 && *(this + 81) == 0 )`
  - `v101 = *(this + 42) + 144;`
  - `ZXString<char>::operator=(a1: v101);`
  - `v3 = *(v1 + 50);`
  - `if ( v3 != 0 )`
  - `v4 = *(v3 - 4);`
  - `v5 = *(v1 + 49);`
  - `if ( v5 != 0 )`
  - `v6 = *(v5 - 4);`
  - `if ( v4 == v6 )`
  - `v7 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `LOBYTE(v127) = 2;`
  - …（截断 14 行）

### CQuest::OnCompleteQuestFailed  @ 0x718b58  (44961 B)

- **c2s opcode `COutPacket(107)`**
- c2s 编码：`Encode1`, `Encode2`, `Encode4`, `EncodeBuffer`
- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1389_NPC`, `SP_1593_MOB`, `SP_2366_ID`, `SP_2973_QUEST`, `SP_3199_QUEST_ACTIMG_D_D_ITEM`, `SP_3217_ITEM`, `SP_3218_QUEST_SAYIMG_D_1_LOST`, `SP_3219_QUEST_SAYIMG_D_1_STOP_S`, `SP_3438_TO_COMPLETE_THE_QUEST_YOULL_NEED_TO_UNSEAL_THE_QUEST_ITEM`, `SP_3506_UNABLE_TO_COMPLETE_THE_QUEST_R_NDUE_TO_A_LACK_OF_PET_R_NPLEASE_TRY_AGAIN_AFTER_S`, `SP_3507_UNABLE_TO_COMPLETE_THE_QUEST_R_NWITH_THE_CURRENT_PET_YOU_HAVE_SUMMONED_R_NPLEASE`, `SP_5621_COUNT`, `SP_960_DEFAULT`…
  - `void __thiscall CQuest::OnCompleteQuestFailed(unsigned __int16 *this, VARIANTARG **p_p_i)`
  - `v219 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v183.lVal) + 4);`
  - `switch ( p_p_i )`
  - `LOBYTE(v223) = 3;`
  - `sub_416821(a1: &v220, a2: v7, a3: v6);`
  - `if ( v191 != 0 )`
  - `sub_402F85(pvargDest: &v173, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v175, pvargSrc: &pvargSrc);`
  - `v8 = sub_41468C(a1: &v220);`
  - `_bstr_t::_bstr_t(a1: v8);`
  - `v10 = sub_406472();`
  - `v11 = sub_402680(a1: v10);`
  - `IWzResMan::GetObjectA(this: v11, pvargDest: &v172, sUOL: p_i, vParam: v162, vAux: p_pvarg);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: p_p_p_i, a2: v165);`
  - …（截断 14 行）

### CQuest::SayNormal  @ 0x717740  (4413 B)

  - `signed int __thiscall CQuest::SayNormal(char *this, _DWORD *a2)`
  - `sub_4281C2(a1: this + 24);`
  - `sub_716541(a1: v26, a2: v27);`
  - `if ( *a2 == 0 )`
  - `v4 = *(*a2 - 4);`
  - `if ( v34 >= v4 )`
  - `if ( v34 == v4 - 1 )`
  - `if ( *(this + 5) == 1 )`
  - `v14 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v14 != 0 )`
  - `v15 = CUtilDlgEx::CUtilDlgEx(this: v14);`
  - `v29 = v16 != 0 ? (v16 - 8) : 0;`
  - `sub_4289CD(a1: v28);`
  - `v17 = sub_490E87(a1: v34);`
  - …（截断 14 行）

### CQuest::StartQuest  @ 0x716fe1  (10701 B)

- **c2s opcode `COutPacket(107)`**
- c2s 编码：`Encode1`, `Encode2`, `Encode4`
- SP_ 节点：`SP_3212_QUEST_SAYIMG`, `SP_3214_ASK`
  - `void __thiscall CQuest::StartQuest(unsigned __int16 *this, int a2)`
  - `v3 = (*(*(TSingleton<CUserLocal>::ms_pInstance + 4) + 16))(a1: TSingleton<CUserLocal>::ms_pInstance + 4);`
  - `v5 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v54) + 4);`
  - `v6 = CQuestMan::CheckCompleteDemand(`
  - `a2: *(this + 6),`
  - `i: *(this + 4),`
  - `a5: (*dword_BE7918 + 8500));`
  - `if ( v6 != 0 && (*(this + 5) == 1 || v6 == 11) )`
  - `CQuest::OnCompleteQuestFailed(this, a2: v6);`
  - `if ( *(this + 5) == 0 && CQuestMan::IsStartScriptLinkedQuest(a1: *(this + 6)) != 0 )`
  - `COutPacket::COutPacket(a1: 107);`
  - `COutPacket::Encode1(this: &v52, a2: 4);`
  - `COutPacket::Encode2(a1: *(this + 6));`
  - `COutPacket::Encode4(this: &v52, a2: *(this + 4));`
  - …（截断 14 行）

### CQuestMan::CheckCompleteDemand  @ 0x721d2c  (20060 B)

  - `signed int __thiscall CQuestMan::CheckCompleteDemand(`
  - `if ( a2 >= 0x4B0u && a2 < 0x578u )`
  - `if ( a2 == 0 )`
  - `if ( sub_6CECD6(a1: &a2, a2: v85) == 0 )`
  - `LOBYTE(v94) = 1;`
  - `if ( sub_4E901B(this: (a4 + 1535), a2: &v90, a3: &v93) == 0 )`
  - `if ( v93 != 0 )`
  - `if ( v8 != 0 && v8 != i )`
  - `while ( 1 )`
  - `if ( v9 == 0 || i >= *(v9 - 4) )`
  - `v10 = (v9 + 8 * i);`
  - `v88 = (v6 + 4 * v11 + 1095);`
  - `v12 = *v88 != 0 ? *(*v88 - 4) : 0;`
  - `if ( v90 > v12 - 1 )`
  - …（截断 14 行）

### CQuestMan::CheckStartDemand  @ 0x721163  (20890 B)

  - `int __fastcall CQuestMan::CheckStartDemand(int a1, int a2, int a3, char *Data, int a5, int a6, int a7, int a8)`
  - `if ( a3 == 0 )`
  - `if ( sub_6CECD6(a1: &a3, a2: v102) == 0 )`
  - `if ( sub_500EA5(a1: &v104) != 0 )`
  - `if ( v10 >= 0 && v10 < *(a1 + 76) )`
  - `if ( *v103 >= 0 && *v103 > *(a1 + 76) )`
  - `if ( v11 != 0 && v11 != Data )`
  - `if ( sub_724B43(a1: a3, a2: v9) == 0 )`
  - `sub_721C65(a1: a6);`
  - `dword_BF03B0(a1: &v92);`
  - `v98 = sub_7B8F35();`
  - `if ( sub_500E4C(a1: &a3, a2: &FileTime) != 0 )`
  - `if ( v103[12] < 0 )`
  - `if ( v103[37] == 0 )`
  - …（截断 14 行）

### CQuestMan::GetCompleteDemand  @ 0x6cdd06  (851 B)

  - `int __thiscall CQuestMan::GetCompleteDemand(CQuestMan *this, unsigned __int16 a2)`
  - `if ( sub_6CECD6(a1: &a2, a2: v5) != 0 )`
  - `v3 = *(v2 + 1);`
  - `if ( (v9 & 1) != 0 && v8 != 0 )`
  - `sub_6CEC6E(a1: 0);`
  - `if ( v6 != 0 )`

### CQuestMan::GetPartyQuestRankCount  @ 0x724722  (753 B)

  - `int __stdcall CQuestMan::GetPartyQuestRankCount(int a1, char *a2)`
  - `for ( i = 1200; i < 0x578u; ++i )`
  - `ZXString<char>::GetBuffer(a1: aRank, a2: 0xFFFFFFFF);`
  - `sub_4E704B(this: a2, a2: &v5, a3: i, a4: &v6);`
  - `LOBYTE(v7) = 2;`
  - `if ( ZXString<char>::operator==(a1) != 0 )`

### CQuestMan::GetQuestByNpc  @ 0x71ddec  (219 B)

  - `BOOL __stdcall CQuestMan::GetQuestByNpc(char a1, int a2)`
  - `ZArray<unsigned short>::RemoveAll(a1: a2);`
  - `return ZMap<unsigned long,ZArray<unsigned short>,unsigned long>::GetAt(&a1, a2) != 0;`

### CQuestMan::GetQuestDisplayMedal  @ 0x71df41  (4831 B)

- SP_ 节点：`SP_3232_QUEST_QUESTINFOIMG`
  - `int __thiscall CQuestMan::GetQuestDisplayMedal(CQuestMan *this, int a2)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v32, pvargSrc: &pvargSrc);`
  - `LOBYTE(v37) = 1;`
  - `v4 = sub_406472();`
  - `v5 = sub_402680(a1: v4);`
  - `IWzResMan::GetObjectA(this: v5, pvargDest: &pvarg, sUOL: v23, vParam: v24, vAux: p_pvargDest);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v26, a2: v27);`
  - `if ( Unknown != 0 )`
  - `v6 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v6 < 0 && v6 != -2147467262 )`
  - `if ( v7 < 0 )`
  - `if ( v8 < 0 )`
  - `if ( v9 < 0 )`
  - …（截断 14 行）

### CQuestMan::IsAutoAutoAcceptQuest  @ 0x720fab  (295 B)

  - `BOOL __thiscall CQuestMan::IsAutoAutoAcceptQuest(CQuestMan *this, unsigned __int16 usQuestID)`
  - `return ZMap<unsigned short,int,unsigned short>::GetAt(a1: &usQuestID, a2: &bAutoAccept) != 0;`

### CQuestMan::IsAutoCompleteQuest  @ 0x720fcc  (230 B)

  - `BOOL __thiscall CQuestMan::IsAutoCompleteQuest(void *this, char a2)`
  - `return ZMap<unsigned short,int,unsigned short>::GetAt(a1: &a2, a2: &v3) != 0;`

### CQuestMan::IsAutoStartQuest  @ 0x720f8a  (227 B)

  - `BOOL __thiscall CQuestMan::IsAutoStartQuest(void *this, char a2)`
  - `return ZMap<unsigned short,int,unsigned short>::GetAt(a1: &a2, a2: &v3) != 0;`

### CQuestMan::IsCompleteScriptLinkedQuest  @ 0x720f09  (851 B)

  - `int __thiscall CQuestMan::IsCompleteScriptLinkedQuest(CQuestMan *this, unsigned __int16 a2)`
  - `if ( a2 == 0 )`
  - `if ( sub_6CECD6(a1: &a2, a2: v5) == 0 )`
  - `v3 = *(v6 + 132);`
  - `if ( v3 == 0 || *v3 == 0 )`
  - `if ( !v2 )`
  - `sub_6CEC6E(a1: 0);`
  - `if ( v6 != 0 )`

### CQuestMan::IsStartScriptLinkedQuest  @ 0x720e88  (848 B)

  - `int __thiscall CQuestMan::IsStartScriptLinkedQuest(CQuestMan *this, unsigned __int16 a2)`
  - `if ( a2 == 0 )`
  - `if ( sub_6CECD6(a1: &a2, a2: v5) == 0 )`
  - `v3 = *(v6 + 128);`
  - `if ( v3 == 0 || *v3 == 0 )`
  - `if ( !v2 )`
  - `sub_6CEC6E(a1: 0);`
  - `if ( v6 != 0 )`

### CQuestMan::LoadDemand  @ 0x71d8df  (10886 B)

- SP_ 节点：`SP_3220_QUEST_CHECKIMG`
  - `signed int __thiscall CQuestMan::LoadDemand(_DWORD *this)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v55, pvargSrc: &pvargSrc);`
  - `LOBYTE(v69) = 1;`
  - `_bstr_t::_bstr_t(a1: str_Et);`
  - `v2 = sub_406472();`
  - `v3 = sub_402680(a1: v2);`
  - `IWzResMan::GetObjectA(this: v3, pvargDest: &pvarg, sUOL: v45, vParam: v46, vAux: p_pvargDest);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v48, a2: v49);`
  - `if ( Unknown != 0 )`
  - `v4 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v4 < 0 && v4 != -2147467262 )`
  - `if ( v5 < 0 )`
  - `if ( v6 < 0 )`
  - …（截断 14 行）

### CQuestMan::LoadPartyQuestInfo  @ 0x723341  (12658 B)

  - `int __thiscall CQuestMan::LoadPartyQuestInfo(CQuestMan *this)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v46, pvargSrc: &pvargSrc);`
  - `LOBYTE(v70) = 1;`
  - `_bstr_t::_bstr_t(a1: str_Qu);`
  - `v2 = sub_406472();`
  - `v3 = sub_402680(a1: v2);`
  - `IWzResMan::GetObjectA(this: v3, pvargDest: &pvarg, sUOL: v34, vParam: v35, vAux: p_pvargDest);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v37, a2: v38[0]);`
  - `if ( Unknown != 0 )`
  - `v4 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v4 < 0 && v4 != -2147467262 )`
  - `if ( v5 < 0 )`
  - `if ( v6 < 0 )`
  - …（截断 14 行）

### CQuestMan::LoadPartyQuestRank  @ 0x7238dd  (18533 B)

  - `int __stdcall CQuestMan::LoadPartyQuestRank(char a1, int a2)`
  - `sub_72538C(&a1, a2: 0);`
  - `v2 = sub_726752(a1: -1);`
  - `_bstr_t::_bstr_t(a1: str_Mi);`
  - `LOBYTE(v106) = 0;`
  - `sub_40263B(a1: &a2);`
  - `Item = IWzProperty::GetItem(&pvargDest, a2: v62);`
  - `*v4 = get_int32(pvarSrc: Item, a2: v63);`
  - `if ( v6 < 0 )`
  - `_bstr_t::_bstr_t(a1: str_Ma_0);`
  - `v8 = IWzProperty::GetItem(&pvargDest, a2: v62);`
  - `v4[1] = get_int32(pvarSrc: v8, a2: v63);`
  - `if ( v9 < 0 )`
  - `if ( a2 == 0 )`
  - …（截断 14 行）

### CQuestMan::LoadQuestInfo  @ 0x722823  (22479 B)

- SP_ 节点：`SP_1604_NAME`, `SP_1735_AREA`, `SP_3216_ORDER`, `SP_3232_QUEST_QUESTINFOIMG`, `SP_3236_PARENT`, `SP_4063_AUTOSTART`, `SP_4066_ONESHOT`, `SP_4067_TIMELIMIT`, `SP_4166_AUTOPRECOMPLETE`, `SP_4530_SORTKEY`, `SP_5111_AUTOACCEPT`, `SP_5354_SHOWLAYERTAG`
  - `_DWORD *__thiscall CQuestMan::LoadQuestInfo(_DWORD *this)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v119, pvargSrc: &pvargSrc);`
  - `LOBYTE(v144) = 1;`
  - `v4 = sub_406472();`
  - `v5 = sub_402680(a1: v4);`
  - `IWzResMan::GetObjectA(this: v5, pvargDest: &v103, sUOL: v98, vParam: v99, vAux: p_pvargDest);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v101, a2: v102);`
  - `if ( Unknown != 0 )`
  - `v6 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v6 < 0 && v6 != -2147467262 )`
  - `if ( v7 < 0 )`
  - `if ( v8 < 0 )`
  - `if ( v9 < 0 )`
  - …（截断 14 行）

### CQuestMan::RemoveRecentlyUpdatedQuest  @ 0x71df1c  (296 B)

  - `int __stdcall CQuestMan::RemoveRecentlyUpdatedQuest(unsigned __int16 a1)`
  - `result = ZList<unsigned short>::Find(&a1, a2: 0);`
  - `if ( result != 0 )`
  - `return ZList<unsigned short>::RemoveAt(a1: result);`

### CQuestMan::SetRecentlyUpdatedQuest  @ 0x71dee4  (334 B)

  - `unsigned __int16 *__thiscall CQuestMan::SetRecentlyUpdatedQuest(CQuestMan *this, unsigned __int16 a2)`
  - `CQuestMan::RemoveRecentlyUpdatedQuest(a1: a2);`
  - `result = ZList<unsigned short>::AddHead(a1: this + 624);`

### CSecurityClient::OnCheckClientIntegrityRequest  @ 0xa4bf22  (1323 B)

- **c2s opcode `COutPacket(26)`**
- s2c 解码：`Decode2`, `DecodeBuffer`
- c2s 编码：`Encode1`, `Encode2`, `EncodeBuffer`
  - `int __thiscall CSecurityClient::OnCheckClientIntegrityRequest(CSecurityClient *this, struct CInPacket *a2)`
  - `memset(a1: &v4[101], a2: 0, a3: 0x192u);`
  - `LOWORD(v4[101]) = CInPacket::Decode2(this: a2);`
  - `CInPacket::DecodeBuffer(this: a2, a2: &v4[101] + 2, a3: LOWORD(v4[101]));`
  - `memset(a1: v4, a2: 0, a3: 0x192u);`
  - `Response = _AhnHS_MakeResponse(a1: &v4[101] + 2, a2: LOWORD(v4[101]), a3: v4);`
  - `if ( Response != 0 )`
  - `_CxxThrowException(a1: v5, a2: &_TI2_AVCSecurityThreatDetected__);`
  - `COutPacket::COutPacket(a1: 26);`
  - `COutPacket::Encode1(this: &v6, a2: 4);`
  - `COutPacket::Encode2(this: &v6, a2: v4[0]);`
  - `COutPacket::EncodeBuffer(this: &v6, a2: v4 + 2, a3: LOWORD(v4[0]));`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v6);`

### CTrunkDlg::SendGetItemRequest  @ 0x7c83af  (4544 B)

- **c2s opcode `COutPacket(62)`**
- c2s 编码：`Encode1`
- SP_ 节点：`SP_4661_ONCE_FOUND_THE_ITEM_CANNOT_BE_TRADED_R_NWOULD_YOU_STILL_LIKE_TO_FIND_IT`, `SP_858_ARE_YOU_SURE_YOU_WANT_TO__R_NRECOVER_THE_ITEMS_FROM_STORAGE`, `SP_859_RECOVERING_THE_ITEMS_WILL_COST_D_MESOS_R_NARE_YOU_SURE_YOU_WANT_TO_RECOVER_THIS`
  - `void __thiscall CTrunkDlg::SendGetItemRequest(int *this)`
  - `v2 = *(this + 60);`
  - `if ( v2 >= 0 )`
  - `v3 = *(this + 53);`
  - `v4 = v3 != 0 ? *(v3 - 4) : 0;`
  - `if ( v2 < v4 && *(this + 65) == 0 )`
  - `v5 = *(this + 53) + 28 * v2;`
  - `v25 = *(v5 + 24);`
  - `if ( v25 != 0 )`
  - `InterlockedIncrement(lpAddend: (v25 + 4));`
  - `if ( (*(v8 + 20))(a1: v7) == 0 )`
  - `if ( CUtilDlg::YesNo(a1: v18, a2: v19, a3: v20, a4: v21, a5: v22) == 6 )`
  - `v11 = *(this + 51) <= 0;`
  - `LOBYTE(v30) = 1;`
  - …（截断 14 行）

### CTrunkDlg::SendGetMoneyRequest  @ 0x7c8887  (1220 B)

- **c2s opcode `COutPacket(62)`**
- c2s 编码：`Encode1`, `Encode4`
- SP_ 节点：`SP_863_HOW_MUCH_WILL_YOU_TAKE_OUT`
  - `void __thiscall CTrunkDlg::SendGetMoneyRequest(int *this)`
  - `if ( *(this + 65) == 0 )`
  - `v4 = *(this + 63);`
  - `v5 = sub_7C8FA5(a1: v3, a2: v4, a3: v4);`
  - `if ( v6 > 0 && v6 <= *(this + 63) )`
  - `COutPacket::COutPacket(a1: 62);`
  - `COutPacket::Encode1(this: &v7, a2: 7);`
  - `COutPacket::Encode4(this: &v7, a2: v6);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v7);`
  - `*(this + 65) = 1;`

### CTrunkDlg::SendPutItemRequest  @ 0x7c858c  (6136 B)

- **c2s opcode `COutPacket(62)`**
- c2s 编码：`Encode1`, `Encode2`, `Encode4`
- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_4660_ONCE_STORED_IN_THE_INVENTORY_R_NTHE_ITEM_CANNOT_BE_TRADED_R_NBY_THE_CHARACTER_WH`, `SP_860_ARE_YOU_SURE_YOU_WANT_TO_STORE_THE_ITEMS`, `SP_861_STORING_THIS_WILL_COST_D_MESOS_R_NARE_YOU_SURE_YOU_WANT_TO_STORE_THIS`, `SP_862_HOW_MANY_WILL_YOU_STORE`
  - `void __thiscall CTrunkDlg::SendPutItemRequest(_DWORD *this)`
  - `v2 = *(this + 61);`
  - `if ( v2 >= 0 )`
  - `v3 = *(this + 54);`
  - `v4 = v3 != 0 ? *(v3 - 4) : 0;`
  - `if ( v2 < v4 && *(this + 65) == 0 )`
  - `v5 = (*(this + 54) + 28 * v2);`
  - `sub_4288AF(a1: v26);`
  - `if ( (*(v27->dwLowDateTime + 20))(a1: v27, a2: v28 % 1000000) != 0 )`
  - `if ( CUtilDlg::YesNo(a1: v19, a2: v20, a3: v21, a4: v22, a5: v23) != 6 )`
  - `if ( sub_4F3144(ebx0: 0, a2: v27) )`
  - `LOBYTE(v37) = 1;`
  - `v34 = sub_7C8FA5(a1: v10, a2: v6, a3: v6);`
  - `if ( v32 != 0 )`
  - …（截断 14 行）

### CTrunkDlg::SendPutMoneyRequest  @ 0x7c893e  (1832 B)

- **c2s opcode `COutPacket(62)`**
- c2s 编码：`Encode1`, `Encode4`
- SP_ 节点：`SP_864_HOW_MUCH_WILL_YOU_SAVE`
  - `void __thiscall CTrunkDlg::SendPutMoneyRequest(_DWORD *this)`
  - `v1 = *(this + 65) == 0;`
  - `if ( v1 )`
  - `v2 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v10) + 4);`
  - `if ( v11 != 0 )`
  - `v3 = _ZtlSecureFuse<long>(a1: v2 + 165, a2: *(v2 + 173));`
  - `v13 = _ZtlSecureFuse<long>(a1: v2 + 165, a2: *(v2 + 173));`
  - `v6 = sub_7C8FA5(a1: v5, a2: v13, a3: v3);`
  - `if ( v7 > 0 && v7 <= _ZtlSecureFuse<long>(a1: v2 + 165, a2: *(v2 + 173)) )`
  - `COutPacket::COutPacket(a1: 62);`
  - `COutPacket::Encode1(this: &v8, a2: 7);`
  - `COutPacket::Encode4(this: &v8, a2: -v7);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v8);`

### CTrunkDlg::SendSortItemRequest  @ 0x7c8824  (648 B)

- **c2s opcode `COutPacket(62)`**
- c2s 编码：`Encode1`
  - `void *__thiscall CTrunkDlg::SendSortItemRequest(_DWORD *this)`
  - `if ( *(this + 65) == 0 )`
  - `COutPacket::COutPacket(a1: 62);`
  - `COutPacket::Encode1(this: &v3, a2: 6);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v3);`
  - `*(this + 65) = 1;`

### CUIAvatarVAC::IsRequestValid  @ 0x60afd5  (180 B)

  - `BOOL __thiscall CUIAvatarVAC::IsRequestValid(CUIAvatarVAC *this)`
  - `v1 = *(this + 27);`
  - `return *(v1 + 368) == 0 && *(v1 + 360) == 5;`

### CUICharSelect::IsRequestValid  @ 0x601861  (182 B)

  - `BOOL __thiscall CUICharSelect::IsRequestValid(CUICharSelect *this)`
  - `v1 = *(this + 28);`
  - `return *(v1 + 368) == 0 && *(v1 + 360) == 2;`

### CUIFadeYesNo::CreateQuestClear  @ 0x523408  (4699 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1235_INVITE`, `SP_1292_UI_UIWINDOWIMG_FADEYESNO_ICON6`, `SP_1293_UI_UIWINDOWIMG_FADEYESNO_ICON7`, `SP_1298_UI_UIWINDOWIMG_FADEYESNO_BACKGRND4`
  - `void __thiscall CUIFadeYesNo::CreateQuestClear(int this, int usQuestID)`
  - `if ( TSingleton<CUIStatusBar>::ms_pInstance != 0 )`
  - `*(this + 196) = 7;`
  - `*(this + 232) = v3;`
  - `sub_402F85(&pvargDest, pvargSrc: v20);`
  - `sub_402F85(pvargDest: &v23, pvargSrc: &pvargSrc);`
  - `v4 = *(this + 232) == 0;`
  - `LOBYTE(v28) = 1;`
  - `if ( v4 )`
  - `sub_405210(a1: BSTR);`
  - `v8 = sub_406472();`
  - `v9 = sub_402680(a1: v8);`
  - `IWzResMan::GetObjectA(this: v9, pvargDest: &pvarg, sUOL: v17.x, vParam: v17.y, vAux: p_pvargDest);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: p_usQuestID, a2: v20);`
  - …（截断 14 行）

### CUIGuildBBS::SendLoadListRequest  @ 0x816b69  (527 B)

- **c2s opcode `COutPacket(155)`**
- c2s 编码：`Encode1`, `Encode4`
  - `int __thiscall CUIGuildBBS::SendLoadListRequest(int *this)`
  - `COutPacket::COutPacket(a1: 155);`
  - `COutPacket::Encode1(this: &v3, a2: 2);`
  - `COutPacket::Encode4(this: &v3, a2: *(this + 368));`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v3);`

### CUIGuildBBS::SendViewEntryRequest  @ 0x816bca  (583 B)

- **c2s opcode `COutPacket(155)`**
- c2s 编码：`Encode1`, `Encode4`
  - `int __thiscall CUIGuildBBS::SendViewEntryRequest(int *this)`
  - `COutPacket::COutPacket(a1: 155);`
  - `COutPacket::Encode1(this: &v3, a2: 3);`
  - `COutPacket::Encode4(this: &v3, a2: *(this + 365));`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v3);`
  - `CUIGuildBBS::SendLoadListRequest(this);`

### CUIItemMaker::RequestItemMake  @ 0x827096  (3386 B)

- **c2s opcode `COutPacket(113)`**
- c2s 编码：`Encode4`, `Encode1`
- SP_ 节点：`SP_753_PRODUCTION_FAILED`
  - `signed int __thiscall CUIItemMaker::RequestItemMake(_DWORD *this)`
  - `if ( *(this + 364) != 3`
  - `|| *(this + 449) == 2 && CUIItemMaker::IsExistMakableItem(this) == 0`
  - `|| CUIItemMaker::IsExistEmptySlot(this) == 0`
  - `|| CUIItemMaker::IsEnoughMeso(this) == 0 )`
  - `if ( !CUIItemMaker::IsAbleToMake(this) )`
  - `CUtilDlg::Notice(sMsg: v10, sSndName: v11, ppDialog: v12, bAutoSeparated: v13, bIsTightLine: v14);`
  - `if ( *(this + 449) == 3 && *(this + 384) == 0 )`
  - `COutPacket::COutPacket(a1: 113);`
  - `v5 = *(this + 449);`
  - `if ( v5 > 0 )`
  - `if ( v5 <= 2 )`
  - `COutPacket::Encode4(this: &v15, a2: v5);`
  - `COutPacket::Encode4(this: &v15, a2: *(this + 371));`
  - …（截断 14 行）

### CUIMedalQuestInfo::Draw  @ 0x843bf6  (40571 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_5300_NUMBER_OF_MEDALS_OBTAINED_D`, `SP_5301_OBTAINABLE_TITLE`, `SP_5302_TITLE_BEING_CHALLENGED`
  - `int *__thiscall CUIMedalQuestInfo::Draw(int *this, int a2)`
  - `CWnd::Draw(this, a2);`
  - `CWnd::GetCanvas(a1: &v183);`
  - `ZArray<_com_ptr_t<_com_IIID<IWzGr2DLayer,&__s_GUID const _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf>>>::RemoveAll(a1: this + 384);`
  - `v3 = *(*(this + 367) + 56);`
  - `v4 = *(this + 374) == v3;`
  - `if ( !v4 )`
  - `*(this + 374) = v3;`
  - `for ( i = 0; i < 2; ++i )`
  - `sub_8454F6(a1: *(this + 369), a2: i);`
  - `get_basic_font(a1: &v157, fType: 1);`
  - `LOBYTE(v184) = 1;`
  - `get_basic_font(a1: &v159, fType: 36);`
  - `v6 = *(this + 369);`
  - …（截断 14 行）

### CUIMedalQuestInfo::GetSelect  @ 0x841976  (1468 B)

  - `int __thiscall CUIMedalQuestInfo::GetSelect(CUIMedalQuestInfo *this, int a2)`
  - `v3 = *(this + *(this + 369) + 390);`
  - `if ( v3 != 0 )`
  - `v4 = *(v3 - 4);`
  - `for ( i = *(*(this + 367) + 56); ; ++i )`
  - `if ( i >= v4 )`
  - `v14 = *(*(this + *(this + 369) + 390) + 8 * i + 4);`
  - `v7 = CUIMedalQuestInfo::IsStateButton(a1: v11, a2: v12);`
  - `v8 = *(v14 + 56);`
  - `if ( v8 < a2 && a2 < v9 + v8 )`
  - `if ( v14 != 0 )`
  - `sub_84576E(a1: 0);`

### CUIMedalQuestInfo::IsStateButton  @ 0x8438a6  (308 B)

  - `_WORD *__stdcall CUIMedalQuestInfo::IsStateButton(char a1, _WORD *a2)`
  - `if ( a2 != 0 )`
  - `sub_84576E(a1: 0);`

### CUIMedalQuestInfo::LoadData  @ 0x8423e2  (38911 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_12_BEGINNER`, `SP_1735_AREA`, `SP_3236_PARENT`, `SP_5105_VIEWMEDALITEM`, `SP_5106_MEDALCATEGORY`, `SP_5576_TITLE`
  - `int __fastcall CUIMedalQuestInfo::LoadData(CUIMedalQuestInfo *this)`
  - `v170 = (this + 1560);`
  - `sub_8458AB(a1: v170);`
  - `for ( i = 0; i < 2; ++i )`
  - `sub_8454F6(a1: j, a2: i);`
  - `while ( j < 4 );`
  - `sub_4747C2(a1: v176 + 1620);`
  - `sub_8458AB(a1: v176 + 1576);`
  - `v149 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &pvargDest.lVal) + 4);`
  - `CurFieldID = CWvsContext::GetCurFieldID(a1: *dword_BE7918);`
  - `sub_6718F1(a1: &v150);`
  - ``eh vector constructor iterator'(a1: v135, a2: 4u, a3: 4, a4: ZArray<int>::ZArray<int>, a5: ZArray<int>::~ZArray<int>);`
  - `LOBYTE(v179) = 1;`
  - `v4 = ZArray<int>::InsertBefore(a1: -1);`
  - …（截断 14 行）

### CUIMedalQuestInfo::OnCreate  @ 0x84056b  (35918 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1410_CANVASFONT`, `SP_1411_BA`, `SP_1662_`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_3232_QUEST_QUESTINFOIMG`, `SP_5162_UI_UIWINDOWIMG_TITLE_BACKGRND2`, `SP_5164_UI_UIWINDOWIMG_TITLE_BACKGRND4`, `SP_5168_UI_UIWINDOWIMG_TITLE_ICON1`, `SP_5169_UI_UIWINDOWIMG_TITLE_ICON2`, `SP_5170_UI_UIWINDOWIMG_TITLE_TAB_DISABLED`, `SP_5171_UI_UIWINDOWIMG_TITLE_TAB_ENABLED`, `SP_5527_ARIAL`
  - `int __thiscall CUIMedalQuestInfo::OnCreate(CUIMedalQuestInfo *this, int Unknown)`
  - `sub_41C06C(a1: &word_AF0E08, a2: -1);`
  - `CUIWnd::OnCreate(a1: Unknown, a2: v177);`
  - `for ( i = 4; i != 0; --i )`
  - `sub_7576C5(a1: 2, a2: &Unknown + 3);`
  - `sub_7576C5(a1: 2, a2: &v185);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v189, pvargSrc: &pvargSrc);`
  - `LOBYTE(v194) = 1;`
  - `v6 = sub_406472();`
  - `v7 = sub_402680(a1: v6);`
  - `IWzResMan::GetObjectA(this: v7, pvargDest: &v184, sUOL: v173, vParam: v174, vAux: p_pvargDest);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v176, a2: v177);`
  - `v8 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - …（截断 14 行）

### CUIMedalQuestInfo::OnMouseButton  @ 0x841678  (2894 B)

  - `void __thiscall CUIMedalQuestInfo::OnMouseButton(int *this, int msg, int a3, int a4, signed int a5)`
  - `if ( a5 >= 40 && *(this + 368) < 4 )`
  - `v6 = (this - 1);`
  - `Select = CUIMedalQuestInfo::GetSelect(a1: a5);`
  - `if ( Select == -1 )`
  - `if ( *v9 >= 0 )`
  - `CUIMedalQuestInfo::ToggleDetail(this: v6, a2: 0);`
  - `CWnd::InvalidateRect(this: v6, a2: 0);`
  - `v18 = *(*(this + *(this + 368) + 389) + 8 * Select + 4);`
  - `if ( CUIMedalQuestInfo::IsStateButton(a1: v15, a2: v16) != 0 )`
  - `if ( msg == 513 )`
  - `v12 = *(this + 368);`
  - `if ( *(*(this + v12 + 396) + 8 * v11 + 4) != 0 )`
  - `(*(v13 + 32))(a1: v6, a2: v14);`
  - …（截断 14 行）

### CUIMedalQuestInfo::ResetInfo  @ 0x8438df  (3157 B)

  - `void __thiscall CUIMedalQuestInfo::ResetInfo(CUIMedalQuestInfo *this, int a2)`
  - `v3 = *(this + 370) < 0;`
  - `if ( v3 )`
  - `v4 = sub_83D9E7(a1: v9);`
  - `v14 = *(v4 + 1);`
  - `if ( (v16 & 2) != 0 )`
  - `if ( v12 != 0 )`
  - `sub_84576E(a1: 0);`
  - `if ( (v16 & 1) != 0 && v10 != 0 )`
  - `if ( a2 != 0 )`
  - `CUIMedalQuestInfo::LoadData(this);`
  - `if ( v14 != 0 )`
  - `if ( *(this + 373) == 0 )`
  - `v5 = *(this + 369);`
  - …（截断 14 行）

### CUIMedalQuestInfo::SetScrollBar  @ 0x843a93  (862 B)

  - `void __thiscall CUIMedalQuestInfo::SetScrollBar(CUIMedalQuestInfo *this)`
  - `v1 = *(this + 369);`
  - `if ( v1 == 100 )`
  - `v2 = *(this + 394);`
  - `if ( v2 != 0 )`
  - `v3 = *(v2 - 4);`
  - `v4 = (this + 1468);`
  - `sub_843B0E(a1: 239, a2: 93);`
  - `sub_4D82FB(a1: 245);`
  - `v5 = *(this + v1 + 390);`
  - `if ( v5 != 0 )`
  - `v3 = *(v5 - 4);`
  - `CCtrlScrollBar::SetScrollRange(this: *v4, a2: v3 - 4);`

### CUIMedalQuestInfo::ToggleDetail  @ 0x844e1f  (1427 B)

  - `unsigned int __thiscall CUIMedalQuestInfo::ToggleDetail(CUIMedalQuestInfo *this, int a2)`
  - `result = *(this + 369);`
  - `if ( result >= 4`
  - `|| ((v4 = *(this + 370)) & 0x80000000) != 0`
  - `|| ((v5 = *(this + result + 390)) != 0 ? (result = *(v5 - 4)) : (result = 0), v4 >= result) )`
  - `if ( a2 != 0 )`
  - `if ( *(this + 396) != 0 )`
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1528) != 0 )`
  - `v11 = (*(*(this + 1) + 48))(a1: this + 4);`
  - `v10 = (*(*(this + 1) + 44))(a1: this + 4);`
  - `sub_845570(a1: this, a2: 1);`
  - `v7 = CUIMedalQuestInfoDetail::CUIMedalQuestInfoDetail(a1: v8, a2: v9, a3: v10, a4: v11);`
  - `sub_845603(a1: v7);`
  - `*(this + 372) = 1;`
  - …（截断 14 行）

### CUIMedalQuestInfo::Update  @ 0x844eea  (851 B)

  - `void __thiscall CUIMedalQuestInfo::Update(CWnd *this)`
  - `CWnd::Update(this);`
  - `if ( *(this + 396) == 0 )`
  - `v2 = *(this + 371) - 30 < 0;`
  - `*(this + 371) -= 30;`
  - `if ( v2 )`
  - `v3 = *(this + 396);`
  - `if ( v3 != 0 )`
  - `CWnd::Destroy(a1: v3);`
  - `sub_8456F2(a1: this + 1580);`
  - `*(this + 370) = -1;`
  - `*(this + 372) = 0;`
  - `CWnd::InvalidateRect(this, a2: 0);`
  - `if ( *(this + 370) >= 0 )`
  - …（截断 14 行）

### CUIMedalQuestInfoDetail::OnButtonClicked  @ 0x83da76  (1266 B)

  - `void __thiscall CUIMedalQuestInfoDetail::OnButtonClicked(CUIMedalQuestInfo **this, unsigned int a2)`
  - `switch ( a2 )`
  - `CUIMedalQuestInfo::ToggleDetail(this: *(this + 28), a2: 0);`
  - `if ( dword_BED614 != 0 )`
  - `v3 = **(sub_83D9E7(a1: v5) + 4);`
  - `if ( v6 != 0 )`
  - `sub_84576E(a1: 0);`
  - `v4 = sub_83DB26(a1: v3);`
  - `CWvsContext::StartQuest(usQuestID: v3, dwNpcTemplateID: *(v4 + 24), bAutoStart: 1);`
  - `v2 = **(sub_83D9E7(a1: v7) + 4);`
  - `if ( v8 != 0 )`
  - `CWvsContext::ResignQuest(this: *dword_BE7918, a2: v2);`

### CUIMedalQuestInfoDetail::ResetInfo  @ 0x83e118  (986 B)

  - `void __thiscall CUIMedalQuestInfoDetail::ResetInfo(CWnd *this)`
  - `v2 = sub_83D9E7(a1: v5);`
  - `sub_8455BC(a1: v2);`
  - `if ( v6 != 0 )`
  - `sub_84576E(a1: 0);`
  - `sub_83E05E(a1: this);`
  - `v3 = *(this + 34);`
  - `if ( v3 != 0 )`
  - `(*(*v3 + 12))(a1: v3);`
  - `v4 = *(this + 36);`
  - `if ( v4 != 0 )`
  - `(*(*v4 + 12))(a1: v4);`
  - `sub_83E1C1(a1: this);`
  - `sub_83EA05(a1: this);`
  - …（截断 14 行）

### CUIMonsterCarnival::RequestSend  @ 0x8706d3  (1002 B)

- **c2s opcode `COutPacket(218)`**
- c2s 编码：`Encode1`, `Encode4`
- 节流 `CanSendExclRequest(500)`
  - `int __thiscall CUIMonsterCarnival::RequestSend(CUIMonsterCarnival *this)`
  - `result = CWvsContext::CanSendExclRequest(a1: 500, a2: 0);`
  - `if ( result != 0 )`
  - `COutPacket::COutPacket(a1: 218);`
  - `v3 = *(this + 1504);`
  - `COutPacket::Encode1(this: &v6, a2: v3);`
  - `COutPacket::Encode4(this: &v6, a2: *(this + 727) - 1);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v6);`
  - `*(*dword_BE7918 + 8356) = 1;`
  - `update_time = get_update_time();`
  - `*(v4 + 8360) = update_time;`

### CUINewCharNameSelect::IsRequestValid  @ 0x61770e  (196 B)

  - `BOOL __thiscall CUINewCharNameSelect::IsRequestValid(CUINewCharNameSelect *this)`
  - `v1 = *(this + 27);`
  - `return *(v1 + 368) == 0 && *(v1 + 360) == 4;`

### CUIQuestAlarm::Create  @ 0x8881cb  (594 B)

  - `int __thiscall CUIQuestAlarm::Create(int *this)`
  - `CUIWnd::CreateUIWndPosSaved(`
  - `a2: *(this + 354),`
  - `v2 = sub_88811A(a1: this);`
  - `CWnd::CreateWnd(`
  - `*(this + 399) = 1;`
  - `return sub_49F589(a1: 1);`

### CUIQuestAlarm::DeleteQuest  @ 0x885df1  (438 B)

  - `void __thiscall CUIQuestAlarm::DeleteQuest(CUIQuestAlarm *this, unsigned __int16 a2)`
  - `for ( i = 0; ; ++i )`
  - `v3 = *(this + 364);`
  - `if ( v3 == 0 || i >= *(v3 - 4) )`
  - `if ( *(v3 + 4 * i) == a2 )`
  - `CUIQuestAlarm::DeleteQuestByIndex(this, a2: i);`

### CUIQuestAlarm::DeleteQuestByIndex  @ 0x885e8b  (340 B)

  - `int __thiscall CUIQuestAlarm::DeleteQuestByIndex(CUIQuestAlarm *this, int a2)`
  - `CUIQuestAlarm::RemoveQuestByIndex(this, a2);`
  - `if ( dword_BF0FEC != 0 )`
  - `CUIQuestInfoDetail::ResetInfo(this: dword_BF0FEC);`
  - `CWnd::Destroy(a1: this);`
  - `return CUIQuestAlarm::Create(this);`

### CUIQuestAlarm::Draw  @ 0x885eb8  (62225 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_10_EQP`, `SP_1604_NAME`, `SP_170_YOU_HAVE_EITHER_ENTERED_A_WRONG_NPC_NAME_OR`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_2240_STRING_MOBIMG`, `SP_25_HERO`, `SP_3620_QUEST_HELPER_D_5`, `SP_3621_UI_BASICIMG_BTCLOSE2`, `SP_5550_MESO`
  - `int __thiscall CUIQuestAlarm::Draw(CUIQuestAlarm *this, StringPoolStrings arg0)`
  - `void (__stdcall *v129)(VARIANTARG *); // edi`
  - `CWnd::Draw(this, a2: arg0);`
  - `CWnd::GetCanvas(a1: &v339);`
  - `v3 = *(v2 + 364);`
  - `LOBYTE(v342) = 1;`
  - `if ( v3 != 0 )`
  - `v4 = *(v3 - 4);`
  - `LOBYTE(v4) = 0;`
  - `ZXString<char>::Format(a1: &v318, a2: v6, a3: v4);`
  - `if ( arg0 != SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `v253.decVal.Hi32 = sub_4284A5(this: v2 + 1464);`
  - …（截断 14 行）

### CUIQuestAlarm::RemoveOldQuest  @ 0x887b5c  (641 B)

  - `int __thiscall CUIQuestAlarm::RemoveOldQuest(CUIQuestAlarm *this)`
  - `v6 = sub_888F88(a1: this + 1600);`
  - `while ( v6 != 0 )`
  - `v7 = *sub_888FA9(a1: &v6, a2: &v5);`
  - `v2 = dword_BF060C(a1: v4);`
  - `if ( (v2 - v5) > 0x927C0 && isAllConditionsSatisfied(a1: v7) == 0 )`
  - `sub_888F1A(a1: &v7);`
  - `CUIQuestAlarm::DeleteQuest(this, a2: v7);`

### CUIQuestAlarm::RemoveQuestByIndex  @ 0x885e1a  (667 B)

  - `int __thiscall CUIQuestAlarm::RemoveQuestByIndex(CUIQuestAlarm *this, int a2)`
  - `result = *(this + 364);`
  - `v4 = (this + 1456);`
  - `if ( result != 0 && *(result - 4) != 0 )`
  - `sub_888F1A(a1: *v4 + 4 * a2);`
  - `sub_49ED9D(a1: a2);`
  - `result = sub_496E6B(a1: (4 * a2 + *v4));`
  - `v5 = *(this + 2 * a2 + 378);`
  - `if ( v5 != 0 )`
  - `(*(*v5 + 12))(a1: v5);`

### CUIQuestDelivery::OnChildNotify  @ 0x6ff0d4  (375 B)

  - `void __thiscall CUIQuestDelivery::OnChildNotify(`
  - `if ( a2 == 2000 && a3 >= 0x12C && a3 <= 0x130 )`
  - `CWnd::InvalidateRect(this, a2: 0);`
  - `else if ( a3 == 100 )`
  - `(*(*this + 32))(a1: this, a2);`

### CUIQuestInfo::GetQuestInfo  @ 0x87b921  (404 B)

  - `int __thiscall CUIQuestInfo::GetQuestInfo(int *this, int a2)`
  - `v2 = *(this + 368);`
  - `if ( v2 < 0 || (v3 = *(this + 369)) < 0 )`
  - `*(a2 + 4) = 0;`
  - `*(a2 + 4) = *(*(this + v2 + 385) + 8 * v3 + 4);`
  - `sub_888D34(a1: a2);`

### CUIQuestInfo::LoadData  @ 0x881b18  (30409 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1735_AREA`, `SP_3236_PARENT`
  - `int __thiscall CUIQuestInfo::LoadData(CUIQuestInfo *this)`
  - `v138 = (this + 1540);`
  - `sub_88900C(a1: v138);`
  - `for ( i = 0; ; ++i )`
  - `v3 = *(dword_BED614 + 128);`
  - `v4 = v3 != 0 ? *(v3 - 4) : 0;`
  - `if ( i >= v4 )`
  - `sub_884901(a1: m, a2: i);`
  - `while ( m < 3 );`
  - `sub_4747C2(a1: this + 1584);`
  - `v119 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v120) + 4);`
  - `CurFieldID = CWvsContext::GetCurFieldID(a1: *dword_BE7918);`
  - `sub_6718F1(a1: &v115);`
  - ``eh vector constructor iterator'(a1: v109, a2: 4u, a3: 3, a4: ZArray<int>::ZArray<int>, a5: ZArray<int>::~ZArray<int>);`
  - …（截断 14 行）

### CUIQuestInfo::MarkQuestAsRead  @ 0x882f2f  (328 B)

  - `void __thiscall CUIQuestInfo::MarkQuestAsRead(CUIQuestInfo *this, unsigned __int16 a2)`
  - `if ( *(this + 368) == 0 && ZList<unsigned short>::Find(a1: &a2, a2: 0) == 0 )`
  - `v2 = sub_7264B5(a1: &dword_BF0FD8);`
  - `sub_A1F8E3(a1: 0);`

### CUIQuestInfo::OnChildNotify  @ 0x87fb2f  (501 B)

  - `void __thiscall CUIQuestInfo::OnChildNotify(CUIQuestInfo *this, unsigned int a2, unsigned int a3, unsigned int a4)`
  - `if ( a2 == 2000 )`
  - `if ( a3 == 500 )`
  - `CUIQuestInfo::OnTabChanged(this, a2: a4);`
  - `else if ( a2 == 2001 && a3 >= 0x12C && a3 <= 0x130 )`
  - `CWnd::InvalidateRect(this, a2: 0);`
  - `(*(*this + 32))(a1: this, a2);`

### CUIQuestInfo::OnCreate  @ 0x87ec3f  (31685 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1410_CANVASFONT`, `SP_1411_BA`, `SP_1897_UI_UIWINDOWIMG_QUEST_ICON0`, `SP_1898_UI_UIWINDOWIMG_QUEST_ICON1`, `SP_1899_UI_UIWINDOWIMG_QUEST_ICON2`, `SP_1901_UI_UIWINDOWIMG_QUEST_ICON4`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_2664_UI_UIWINDOWIMG_QUEST_TAB_ENABLED`, `SP_2665_UI_UIWINDOWIMG_QUEST_TAB_DISABLED`, `SP_3232_QUEST_QUESTINFOIMG`, `SP_5527_ARIAL`
  - `int __thiscall CUIQuestInfo::OnCreate(CUIQuestInfo *this, int Unknown)`
  - `sub_41C06C(a1: &word_AF0E08, a2: -1);`
  - `CUIWnd::OnCreate(a1: Unknown, a2: v161);`
  - `for ( i = 3; i != 0; --i )`
  - `v4 = *(dword_BED614 + 128);`
  - `if ( v4 != 0 )`
  - `v5 = *(v4 - 4);`
  - `sub_7576C5(a1: v5, a2: &Unknown + 3);`
  - `v6 = *(dword_BED614 + 128);`
  - `if ( v6 != 0 )`
  - `v7 = *(v6 - 4);`
  - `sub_7576C5(a1: v7, a2: &v172);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - …（截断 14 行）

### CUIQuestInfo::OnTabChanged  @ 0x87fd4f  (801 B)

  - `void __thiscall CUIQuestInfo::OnTabChanged(CUIQuestInfo *this, int a2)`
  - `*(this + 369) = -1;`
  - `*(this + 368) = a2;`
  - `CUIQuestInfo::LoadData(this);`
  - `CUIQuestInfo::SetScrollBar(this);`
  - `if ( a2 != 0 )`
  - `if ( a2 == 1 && CUIQuestInfo::TryShowRecentlyUpdatedQuest(this) == 0 )`
  - `CUIQuestInfo::TryShowRecentlyViewedQuest(this);`
  - `else if ( CUIQuestInfo::TryShowSuitableLevelQuest(this, a2: 1) == 0 )`
  - `CUIQuestInfo::TryShowSuitableLevelQuest(this, a2: 0);`
  - `if ( *(this + 369) == -1 && CUIQuestInfo::TryShowTopQuest(this) == 0 )`
  - `CUIQuestInfo::ToggleDetail(this, a2: 0);`
  - `CUIQuestInfo::SyncScrollBarWithSelected(this);`

### CUIQuestInfo::OnToggleDetail  @ 0x8843a4  (5810 B)

  - `void *__thiscall CUIQuestInfo::OnToggleDetail(CUIQuestInfo *this)`
  - `if ( *(this + 389) != 0 )`
  - `CWnd::GetLayer(a1: &v35);`
  - `if ( v35 == 0 )`
  - `LT = IWzGr2DLayer::GetLT(a1: v33);`
  - `LOBYTE(v36) = 1;`
  - `if ( v4 )`
  - `v5 = sub_425AEF(a1: *LT);`
  - `sub_41569E(a1: v33);`
  - `v6 = IWzGr2DLayer::GetLT(a1: v33);`
  - `v7 = sub_425B16(a1: *v6);`
  - `v4 = *IWzGr2DLayer::GetLT(a1: v33) == 0;`
  - `IWzVector2D::GetOrigin(pvargDest: &v27);`
  - `v8 = IWzGr2DLayer::GetLT(a1: v31);`
  - …（截断 14 行）

### CUIQuestInfo::ResetInfo  @ 0x882f88  (2360 B)

  - `void __thiscall CUIQuestInfo::ResetInfo(CUIQuestInfo *this, int a2)`
  - `v3 = *(this + 369) < 0;`
  - `if ( v3 )`
  - `QuestInfo = CUIQuestInfo::GetQuestInfo(this, a2: v7);`
  - `v12 = *(QuestInfo + 1);`
  - `sub_888D34(a1: v11);`
  - `if ( (v14 & 2) != 0 )`
  - `if ( v10 != 0 )`
  - `sub_888DE3(a1: 0);`
  - `if ( (v14 & 1) != 0 && v8 != 0 )`
  - `if ( a2 != 0 )`
  - `CUIQuestInfo::LoadData(this);`
  - `if ( v12 == 0 || ((v5 = *(this + *(this + 368) + 385)) != 0 ? (v6 = *(v5 - 4)) : (v6 = 0), *(this + 369) >= v6) )`
  - `*(this + 369) = -1;`
  - …（截断 14 行）

### CUIQuestInfo::SetScrollBar  @ 0x883100  (279 B)

  - `void __thiscall CUIQuestInfo::SetScrollBar(CUIQuestInfo *this)`
  - `v1 = *(this + *(this + 368) + 385);`
  - `if ( v1 != 0 )`
  - `v1 = *(v1 - 4);`
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 367), a2: v1 - 13);`

### CUIQuestInfo::SyncScrollBarWithSelected  @ 0x882c8a  (283 B)

  - `void __thiscall CUIQuestInfo::SyncScrollBarWithSelected(CUIQuestInfo *this)`
  - `if ( *(this + 369) <= 8 )`
  - `CCtrlScrollBar::SetCurPos(this: *(this + 367), a2: 0);`
  - `CCtrlScrollBar::SetCurPos(this: *(this + 367), a2: *(this + 369));`

### CUIQuestInfo::ToggleDetail  @ 0x88422c  (1852 B)

  - `void __thiscall CUIQuestInfo::ToggleDetail(CUIQuestInfo *this, int a2)`
  - `v3 = *(this + 368);`
  - `if ( v3 > 2`
  - `|| ((v4 = *(this + 369)) & 0x80000000) != 0`
  - `|| ((v5 = *(this + v3 + 385)) != 0 ? (v6 = *(v5 - 4)) : (v6 = 0), v4 >= v6) )`
  - `if ( a2 != 0 )`
  - `if ( *(this + 389) == 0 )`
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1556) != 0 )`
  - `v13 = (*(*(this + 1) + 48))(a1: this + 4);`
  - `v12 = (*(*(this + 1) + 44))(a1: this + 4);`
  - `sub_888A16(a1: this, a2: 1);`
  - `v8 = sub_87B155(a1: v10, a2: v11, a3: v12, a4: v13);`
  - `sub_888AA9(a1: v8);`
  - `*(this + 371) = 1;`
  - …（截断 14 行）

### CUIQuestInfo::TryShowRecentlyUpdatedQuest  @ 0x882932  (471 B)

  - `int __thiscall CUIQuestInfo::TryShowRecentlyUpdatedQuest(CUIQuestInfo *this)`
  - `while ( 1 )`
  - `v1 = sub_71DF05(a1: dword_BED614);`
  - `if ( v1 == 0 )`
  - `if ( sub_882B2E(a1: 1, a2: v1) != 0 )`
  - `CQuestMan::RemoveRecentlyUpdatedQuest(a1: v2);`

### CUIQuestInfo::TryShowRecentlyViewedQuest  @ 0x88296e  (139 B)

  - `int __thiscall CUIQuestInfo::TryShowRecentlyViewedQuest(CUIQuestInfo *this)`
  - `return sub_882B2E(a1: 1, a2: *(this + 722));`

### CUIQuestInfo::TryShowSuitableLevelQuest  @ 0x88297e  (1375 B)

  - `int __thiscall CUIQuestInfo::TryShowSuitableLevelQuest(CCtrlTab **this, int a2)`
  - `if ( *(this + 368) != 0 )`
  - `CCtrlTab::SetTab(this: *(this + 365), a2: 0);`
  - `for ( i = 0; ; ++i )`
  - `v4 = *(this + 385);`
  - `if ( v4 == 0 || i >= *(v4 - 4) )`
  - `v12 = *(v4 + 8 * i + 4);`
  - `sub_888D34(a1: v11);`
  - `sub_888D34(a1: &v9);`
  - `if ( sub_882E46(a1: v9, a2: v10) == 0 && sub_882E99(a1: *v12) == 0 && (a2 == 0 || sub_882F70(a1: *v12) == 0) )`
  - `if ( v12 != 0 )`
  - `sub_888DE3(a1: 0);`
  - `v6 = sub_882B2E(a1: 0, a2: *v12);`

### CUIQuestInfo::TryShowTopQuest  @ 0x882a71  (1210 B)

  - `int __thiscall CUIQuestInfo::TryShowTopQuest(CUIQuestInfo *this)`
  - `for ( i = 0; ; ++i )`
  - `v3 = *(this + *(this + 368) + 385);`
  - `if ( v3 == 0 || i >= *(v3 - 4) )`
  - `v11 = *(v3 + 8 * i + 4);`
  - `sub_888D34(a1: v10);`
  - `sub_888D34(a1: &v8);`
  - `if ( sub_882E46(a1: v8, a2: v9) == 0 )`
  - `if ( v11 != 0 )`
  - `sub_888DE3(a1: 0);`
  - `v5 = sub_882B2E(a1: *(this + 368), a2: *v11);`

### CUIQuestInfoDetail::Draw  @ 0x87bae7  (17787 B)

  - `VARIANTARG **__thiscall CUIQuestInfoDetail::Draw(_DWORD *this, VARIANTARG **a2)`
  - `CWnd::Draw(this, a2);`
  - `CWnd::GetCanvas(a1: &a2);`
  - `v3 = *(this + 30);`
  - `ZXString<char>::operator=(a1: v3 + 12);`
  - `sub_42847D(a1: this + 50);`
  - `format_string(sFormat: &v97, pFont: p_pvarg, nWidth: p_pvargDest);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v103) = 2;`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `v79 = sub_4284A5(this: this + 50);`
  - `v5 = sub_428226(a1: &v97);`
  - `_bstr_t::_bstr_t(a1: v5);`
  - `v7 = sub_414576(a1: &a2);`
  - …（截断 14 行）

### CUIQuestInfoDetail::OnButtonClicked  @ 0x87b9cf  (1788 B)

  - `void __fastcall CUIQuestInfoDetail::OnButtonClicked(CUIQuestInfo **this, int a2, unsigned int a3)`
  - `switch ( a3 )`
  - `CUIQuestInfo::ToggleDetail(this: *(this + 28), a2: 0);`
  - `CUIQuestInfo::GetQuestInfo(this: *(this + 28), a2: v6);`
  - `if ( v7 != 0 )`
  - `CWvsContext::ResignQuest(this: *dword_BE7918, a2: *v7);`
  - `CWvsContext::TryRegisterAutoStartQuest(this: *dword_BE7918, a2: v5, a3: 1);`
  - `if ( v5 == 10394 )`
  - `sub_888DE3(a1: 0);`
  - `sub_87E769(a1: this);`
  - `if ( dword_BF0FF4 == 0 )`
  - `sub_888AF6(a1: this, a2);`
  - `v4 = *(CUIQuestInfo::GetQuestInfo(this: *(this + 28), a2: v8) + 4);`
  - `LOWORD(v4) = *v4;`
  - …（截断 14 行）

### CUIQuestInfoDetail::OnCreate  @ 0x87b3ef  (10932 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1410_CANVASFONT`, `SP_1411_BA`, `SP_1891_UI_UIWINDOWIMG_QUEST_BACKGRND2`, `SP_1896_UI_UIWINDOWIMG_QUEST_LINE`, `SP_2561_UI_BASICIMG_BTHIDE`, `SP_3232_QUEST_QUESTINFOIMG`, `SP_5527_ARIAL`
  - `void __thiscall CUIQuestInfoDetail::OnCreate(CUIQuestInfoDetail *this, void *a2)`
  - `CWnd::SetBackgrnd(this, a2: p_pvargDest, a3: p_Unknown, a4: v50);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v53, pvargSrc: &pvargSrc);`
  - `LOBYTE(v60) = 1;`
  - `v6 = sub_406472();`
  - `v7 = sub_402680(a1: v6);`
  - `IWzResMan::GetObjectA(this: v7, pvargDest: &pvarg, sUOL: v46, vParam: v47, vAux: p_pvargDest);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: p_Unknown, a2: v50);`
  - `v8 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v8 < 0 && v8 != -2147467262 )`
  - `if ( v9 < 0 )`
  - `if ( v10 < 0 )`
  - `if ( v11 < 0 )`
  - …（截断 14 行）

### CUIQuestInfoDetail::ResetInfo  @ 0x87c515  (1258 B)

  - `void __thiscall CUIQuestInfoDetail::ResetInfo(CUIQuestInfoDetail *this)`
  - `QuestInfo = CUIQuestInfo::GetQuestInfo(this: *(this + 28), a2: v6);`
  - `sub_888A62(a1: QuestInfo);`
  - `if ( v7 != 0 )`
  - `sub_888DE3(a1: 0);`
  - `sub_87C388(this);`
  - `v3 = *(this + 36);`
  - `if ( v3 != 0 )`
  - `(*(*v3 + 12))(a1: v3);`
  - `v4 = *(this + 34);`
  - `if ( v4 != 0 )`
  - `(*(*v4 + 12))(a1: v4);`
  - `v5 = *(this + 38);`
  - `if ( v5 != 0 )`
  - …（截断 14 行）

### CUIRecommendWorld::IsRequestValid  @ 0x62b94c  (190 B)

  - `BOOL __thiscall CUIRecommendWorld::IsRequestValid(CUIRecommendWorld *this)`
  - `v1 = *(this + 32);`
  - `return *(v1 + 368) == 0 && *(v1 + 360) == 1;`

### CUITitle::IsRequestValid  @ 0x621156  (172 B)

  - `BOOL __thiscall CUITitle::IsRequestValid(CUITitle *this)`
  - `v1 = *(this + 32);`
  - `return *(v1 + 368) == 0 && *(v1 + 360) == 0;`

### CUIWorldSelect::IsRequestValid  @ 0x62540a  (184 B)

  - `BOOL __thiscall CUIWorldSelect::IsRequestValid(CUIWorldSelect *this)`
  - `v1 = *(this + 32);`
  - `return *(v1 + 368) == 0 && *(v1 + 360) == 1;`

### CUserLocal::CheckBoobyTrapPickUpRequest  @ 0x95d7b7  (1169 B)

  - `void __thiscall CUserLocal::CheckBoobyTrapPickUpRequest(CUserLocal *this, unsigned int a2)`
  - `v3 = BYTE2(a2);`
  - `if ( BYTE2(a2) == 82`
  - `&& (a2 & 0x1000000) == 0`
  - `&& !CUser::IsDarkSight(this)`
  - `&& !CUser::IsSneak(this)`
  - `&& (*(this + 348) & 0xFFFFFFFE) != 0x12`
  - `&& CUserLocal::IsStun(this) == 0 )`
  - `CWvsContext::CheckBoobyTrapPickUpRequest(this: v2, a2: (this + 12580));`
  - `if ( BYTE2(a2) == 54 )`
  - `v6 = *(v3 + v5 + 4);`
  - `if ( v6 != 0 )`
  - `v8 = *(v7 + 1);`
  - `if ( v8 != 0`
  - …（截断 14 行）

### CUserLocal::OnQuestResult  @ 0x959a73  (35939 B)

- s2c 解码：`Decode1`, `Decode2`, `Decode4`
- 分支 case：`6`, `7`, `8`, `9`, `10`, `1`, `2`, `3`, `4`, `11`, `13`, `14`, `15`, `16`
- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_10_EQP`, `SP_11_SETUP`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_2366_ID`, `SP_2_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTPASSWORD`, `SP_3198_QUEST_ACTIMG_D_D`, `SP_3199_QUEST_ACTIMG_D_D_ITEM`, `SP_3201_JOB`, `SP_3230_GENDER`, `SP_3256_QUEST_SAYIMG_D_D_YES`, `SP_3257_THE_QUEST_HAS_ENDED_R_NDUE_TO_AN_UNKNOWN_ERROR`, `SP_3259_S_ITEM_INVENTORY_IS_FULL`, `SP_3260__OR`…
  - `int __stdcall CUserLocal::OnQuestResult(int i)`
  - `switch ( CInPacket::Decode1(this: i) )`
  - `v114 = CInPacket::Decode2(this: v1);`
  - `if ( v114 != 0 )`
  - `for ( i = v114; i != 0; --i )`
  - `LOWORD(v115) = CInPacket::Decode2(this: v1);`
  - `v117 = CInPacket::Decode4(this: v1);`
  - `sub_A24D5D(a1: v116, a2: v117);`
  - `return sub_A1F8E3(a1: 0);`
  - `v118 = CInPacket::Decode2(this: v1);`
  - `if ( v118 != 0 )`
  - `for ( j = v118; j != 0; --j )`
  - `v120 = CInPacket::Decode2(this: v1);`
  - `sub_A24E33(a1: v120);`
  - …（截断 14 行）

### CUserLocal::OnResignQuestReturn  @ 0x961120  (372 B)

- s2c 解码：`Decode2`
  - `void __thiscall CUserLocal::OnResignQuestReturn(CUserLocal *this, struct CInPacket *a2)`
  - `v2 = CInPacket::Decode2(this: a2);`
  - `CWvsContext::TryRegisterAutoStartQuest(this: *dword_BE7918, a2: v2, a3: 1);`
  - `if ( sub_96115A(a1: v2) != 0 )`
  - `CUIQuestAlarm::DeleteQuest(this: dword_BF0FF4, a2: v2);`

### CUserLocal::SendSkillCancelRequest  @ 0x96d873  (1022 B)

- **c2s opcode `COutPacket(92)`**
- c2s 编码：`Encode4`
  - `int __thiscall CUserLocal::SendSkillCancelRequest(void **this, __int16 *a2)`
  - `if ( *(this + 2746) == a2 )`
  - `memset(a1: this + 2746, a2: 0, a3: 0xCu);`
  - `COutPacket::COutPacket(a1: 92);`
  - `COutPacket::Encode4(this: &v4, a2);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v4);`
  - `if ( a2 == 1004 || a2 == (&loc_4FAA8D + 1) || a2 == (&loc_989A68 + 4) || a2 == 20001004 || a2 == 20011004 )`
  - `if ( *(this + 3179) == 0 )`
  - `*(this + 3180) = get_update_time();`
  - `*(this + 3179) = *(this + 3179) + 1;`
  - `if ( is_keydown_skill(a1: a2) && a2 != 3121004 && a2 != (&loc_4FAA8B + 1) && a2 != &word_C80EDA )`
  - `sub_443D21(a1: *(this + 1130));`

### CUserLocal::SendSkillUseRequest  @ 0x96d399  (6184 B)

- **c2s opcode `COutPacket(91)`**
- c2s 编码：`Encode4`, `Encode1`, `Encode2`
- 节流 `CanSendExclRequest(300)`
  - `signed int __thiscall CUserLocal::SendSkillUseRequest(`
  - `if ( CWvsContext::CanSendExclRequest(a1: 300, a2: 0) == 0 )`
  - `v10 = *(CWvsContext::GetCharacterData(this: v8, a2: &v43) + 4);`
  - `if ( CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v10, a3: *pSkill, a4: 0) <= 0 )`
  - `if ( (v12 == 0 || v12 == 9) && sub_764721(a1: *pSkill) == 0 )`
  - `VecCtrl = CUser::GetVecCtrl(this, a2: &v42);`
  - `v15 = *VecCtrl != 0 ? (*VecCtrl - 12) : 0;`
  - `if ( v42 != 0 )`
  - `(*(*v42 + 8))(a1: v42);`
  - `if ( SKILLENTRY::IsActionAppointed(this: pSkill, a2: v14, a3: nSLV) )`
  - `if ( CAvatar::GetOneTimeAction(this: (this + 136)) <= -1 )`
  - `if ( CVecCtrl::IsOnLadder(this: v15) != 0 )`
  - `if ( CVecCtrl::IsOnRope(this: v15) == 0 )`
  - `v18 = rand();`
  - …（截断 14 行）

### CUserLocal::ShowAutoStartQuestList  @ 0x95d260  (7082 B)

- SP_ 节点：`SP_3191__R_N_R_NFUI_UIWINDOWIMG_UTILDLGEX_LIST1_R_N`, `SP_3192__R_N_R_NFUI_UIWINDOWIMG_UTILDLGEX_LIST0_R_N`, `SP_3193__R_N_R_NFUI_UIWINDOWIMG_UTILDLGEX_LIST3_R_N`, `SP_4171_SOMEONE_IN_MAPLESTORY_WOULD_LIKE_TO_SEND_YOU_A_MESSAGE`
  - `void __thiscall CUserLocal::ShowAutoStartQuestList(CUserLocal *this)`
  - `if ( (byte_BF12AC & 1) == 0 )`
  - `dword_BF12A8 = CNpcTemplate::GetNpcTemplate(a1: sub_897B50);`
  - `v3 = *(*dword_BE7918 + 14256);`
  - `LOBYTE(v48) = 4;`
  - `if ( v3 != 0 )`
  - `if ( v5 != 0 )`
  - `v6 = *(v5 - 1);`
  - `ZXString<char>::_Cat(a1: v5, a2: v6);`
  - `if ( i != 0 )`
  - `v2 = *(v2 + 12);`
  - `while ( v2 != 0 )`
  - `v8 = *(v2 != 0 ? v2 - 16 + 4 : 4);`
  - `*sub_6D5F84(a1: -1) = v7;`
  - …（截断 14 行）

### CWvsContext::CanSendExclRequest  @ 0x485bf7  (281 B)

  - `BOOL __thiscall CWvsContext::CanSendExclRequest(CWvsContext *this, int a2, int a3)`
  - `return *(this + 2089) == 0`
  - `&& (a3 != 0 || _ZtlSecureFuse<short>(a1: *(this + 2094) + 97, a2: *(*(this + 2094) + 101)) > 0)`
  - `&& get_update_time() - *(this + 2090) >= a2;`

### CWvsContext::CheckBoobyTrapPickUpRequest  @ 0xa32914  (188 B)

  - `int __thiscall CWvsContext::CheckBoobyTrapPickUpRequest(CWvsContext *this, const struct tagPOINT *a2)`
  - `return CDropPool::BoobyTrapCheckPickupItem(this: dword_BED6AC, a2);`

### CWvsContext::CheckEquipOnAutoStartQuest  @ 0xa08896  (686 B)

  - `int __thiscall CWvsContext::CheckEquipOnAutoStartQuest(CWvsContext *this, int a2, int a3)`
  - `if ( a2 <= 0 || sub_720E40(a1: a2) == 0 || *(v3 + 492) == 0 )`
  - `sub_725C01(a1: &a2, a2: &v6);`
  - `if ( v6 != 0 && *(v6 - 4) != 0 )`
  - `sub_A08AB7(a1: &v6, a2: a3);`
  - `ZArray<unsigned short>::RemoveAll(a1: &v6);`

### CWvsContext::CheckFieldOnAutoStartQuest  @ 0xa0898f  (753 B)

  - `int __thiscall CWvsContext::CheckFieldOnAutoStartQuest(CWvsContext *this, unsigned int a2)`
  - `if ( a2 == 0 || sub_720E61(a1: a2) == 0 || *(v2 + 516) == 0 )`
  - `ZMap<unsigned long,ZArray<unsigned short>,unsigned long>::GetAt(this: (v2 + 504), &a2, a3: &v5);`
  - `if ( v5 != 0 && *(v5 - 4) != 0 )`
  - `sub_A08AB7(a1: &v5, a2: 1);`
  - `ZArray<unsigned short>::RemoveAll(a1: &v5);`

### CWvsContext::CheckNormalAutoStartQuest  @ 0xa08a19  (728 B)

  - `int __thiscall CWvsContext::CheckNormalAutoStartQuest(CWvsContext *this, int a2)`
  - `v3 = sub_A08A93(a1: v5);`
  - `LOBYTE(v7) = 1;`
  - `sub_67364B(a1: v3);`
  - `ZArray<unsigned short>::RemoveAll(a1: v5);`
  - `if ( v6 != 0 && *(v6 - 4) != 0 )`
  - `sub_A08AB7(a1: &v6, a2);`
  - `ZArray<unsigned short>::RemoveAll(a1: &v6);`

### CWvsContext::CheckQuestCompleteByMeso  @ 0xa1f685  (3649 B)

  - `int __thiscall CWvsContext::CheckQuestCompleteByMeso(CWvsContext *this, int a2)`
  - `HIWORD(v2) = 0;`
  - `if ( sub_71DE6A(a1: &v22) == 0 )`
  - `v4 = *(CWvsContext::GetCharacterData(this: v23, a2: &v15) + 4);`
  - `if ( v16 != 0 )`
  - `while ( v22 != 0 && v25 < *(v22 - 4) )`
  - `LOWORD(v2) = *(v22 + 2 * v25);`
  - `if ( sub_500EA5(a1: &v21) == 0 )`
  - `CompleteDemand = CQuestMan::GetCompleteDemand(this: v3, a2: v2);`
  - `v13 = *(v4 + 173);`
  - `v18 = *(CompleteDemand + 52);`
  - `v21 = _ZtlSecureFuse<long>(a1: v4 + 165, a2: v13);`
  - `v14 = (v23 + 2125);`
  - `v6 = CQuestMan::GetCompleteDemand(this: v3, a2: v2);`
  - …（截断 14 行）

### CWvsContext::OnFamilyJoinRequest  @ 0xa349ab  (1418 B)

- s2c 解码：`Decode4`, `DecodeStr`
  - `void __thiscall CWvsContext::OnFamilyJoinRequest(CWvsContext *this, struct CInPacket *a2)`
  - `v7 = CInPacket::Decode4(this: a2);`
  - `CInPacket::DecodeStr(this: a2, a2: &v8);`
  - `ZXString<char>::operator=(a1: &v8);`
  - `LOBYTE(v9) = 0;`
  - `sub_538C98();`
  - `if ( CConfig::IsInBlackList(a1: 0) != 0`
  - `|| *(TSingleton<CConfig>::ms_pInstance + 19) == 0`
  - `|| CWvsContext::IsFadeWndExist(a1: 12) != 0 )`
  - `sub_428211(a1: &v8);`
  - `CWvsContext::SendFamilyInviteResult(a1: v5, a2: v7, a3: 0);`
  - `v3 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 280);`
  - `if ( v3 != 0 )`
  - `v4 = CUIFadeYesNo::CUIFadeYesNo(a1: v3);`
  - …（截断 14 行）

### CWvsContext::OnFamilyJoinRequestResult  @ 0xa34a97  (2156 B)

- s2c 解码：`Decode1`, `DecodeStr`
- SP_ 节点：`SP_4614_S_HAS_DECLINED_YOUR_JUNIOR_REQUEST`, `SP_4615_YOU_HAVE_ADDED_S_AS_A_JUNIOR_PLEASE_DO_YOUR_BEST_TO_GUIDE_THE_JUNIOR_IN_HIS_HER_`
  - `void __thiscall CWvsContext::OnFamilyJoinRequestResult(CWvsContext *this, struct CInPacket *a2)`
  - `v2 = CInPacket::Decode1(this: a2);`
  - `CInPacket::DecodeStr(this: a2, a2: v13);`
  - `if ( v2 != 0 )`
  - `LOBYTE(v15) = 2;`
  - `ZXString<char>::Format(a1: &v14, a2: v10, a3: v13[0]);`
  - `CHATLOG_ADD(sString: &v14, nType: 0xCu);`
  - `sub_428211(a1: &v14);`
  - `CUtilDlg::Notice(sMsg: v7, sSndName: v8, ppDialog: v9, bAutoSeparated: v10, bIsTightLine: v11);`

### CWvsContext::OnFamilySummonRequest  @ 0xa350c8  (2299 B)

- **c2s opcode `COutPacket(153)`**
- s2c 解码：`DecodeStr`
- c2s 编码：`EncodeStr`, `Encode1`
- SP_ 节点：`SP_4624_FROM_S_S_HAS_REQUESTED_A_SUMMONS_WOULD_YOU_LIKE_TO_TRANSPORT`
  - `void __thiscall CWvsContext::OnFamilySummonRequest(CWvsContext *this, struct CInPacket *a2)`
  - `CInPacket::DecodeStr(this: a2, a2: &v16);`
  - `CInPacket::DecodeStr(this: a2, a2: v15);`
  - `LOBYTE(v18) = 2;`
  - `ZXString<char>::Format(a1: &v17, a2: v4, a3: v15[0]);`
  - `COutPacket::COutPacket(a1: 153);`
  - `ZXString<char>::operator=(a1: &v16);`
  - `COutPacket::EncodeStr(this: &v12, a2: v11);`
  - `sub_428211(a1: &v17);`
  - `v6 = CUtilDlg::YesNo(a1: v7, a2: v8, a3: v9, a4: v10, a5: v11);`
  - `COutPacket::Encode1(this: &v12, a2: v6 == 6);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v12);`

### CWvsContext::OnMarriageRequest  @ 0xa29886  (2292 B)

- **c2s opcode `COutPacket(137)`**
- s2c 解码：`Decode1`, `DecodeStr`, `Decode4`
- c2s 编码：`Encode1`, `EncodeStr`, `Encode4`
- SP_ 节点：`SP_4233_S_HAS_REQUESTED_ENGAGEMENT_R_NWILL_YOU_ACCEPT_THIS_PROPOSAL`
  - `void __thiscall CWvsContext::OnMarriageRequest(CWvsContext *this, struct CInPacket *a2)`
  - `v2 = CInPacket::Decode1(this: a2);`
  - `if ( v2 != 0 )`
  - `if ( v2 == 9 && dword_BE7910 == 0 )`
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 232) != 0 )`
  - `sub_9D7822();`
  - `CInPacket::DecodeStr(this: a2, a2: v12);`
  - `v3 = CInPacket::Decode4(this: a2);`
  - `LOBYTE(v14) = 1;`
  - `ZXString<char>::Format(a1: &v13, a2: v7, a3: v12[0]);`
  - `COutPacket::COutPacket(a1: 137);`
  - `COutPacket::Encode1(this: &v9, a2: 2);`
  - `sub_428211(a1: &v13);`
  - `v8 = CUtilDlg::YesNo(a1: v6, a2: 0, a3: 0, a4: 0, a5: 0) == 6;`
  - …（截断 14 行）

### CWvsContext::OnQuestProgressUpdated  @ 0xa26db3  (651 B)

  - `void __thiscall CWvsContext::OnQuestProgressUpdated(_DWORD *this, int usQuestID, int nItemID, char sOldInfo)`
  - `CQuestMan::SetRecentlyUpdatedQuest(this: dword_BED614, a2: usQuestID);`
  - `if ( nItemID != 0 )`
  - `CNoticeQuestProgress::OnQuestProgressUpdated_Item(this: *(this + 3489), usQuestID, nItemID);`
  - `ZXString<char>::operator=(a1: &sOldInfo);`
  - `v5 = sub_A1499D(a1: this + 3488);`
  - `CNoticeQuestProgress::OnQuestProgressUpdated_Mob(this: v5, a2: usQuestID, a3: 0);`

### CWvsContext::OnQuestRecordExMessage  @ 0xa2160b  (1096 B)

- s2c 解码：`Decode2`, `DecodeStr`
  - `void __thiscall CWvsContext::OnQuestRecordExMessage(CWvsContext *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode2(this: a2);`
  - `CInPacket::DecodeStr(this: a2, a2: &v9);`
  - `v4 = *(CWvsContext::GetCharacterData(this, a2: &v7) + 4);`
  - `LOBYTE(v10) = 1;`
  - `sub_4E71FA(this: v4, a2: v3, a3: &v9);`
  - `if ( v8 != 0 )`
  - `v5 = *(this + 3452);`
  - `if ( v5 != 0 )`
  - `CUIQuestInfo::ResetInfo(this: v5, a2: 1);`
  - `v6 = *(this + 3454);`
  - `if ( v6 != 0 )`
  - `CUIMedalQuestInfo::ResetInfo(this: v6, a2: 1);`

### CWvsContext::OnQuestRecordMessage  @ 0xa20f4c  (12650 B)

- s2c 解码：`Decode2`, `Decode1`, `DecodeBuffer`, `DecodeStr`
  - `void __thiscall CWvsContext::OnQuestRecordMessage(FILETIME *this, struct CInPacket *a2)`
  - `LOWORD(v4) = CInPacket::Decode2(this: a2);`
  - `v5 = CInPacket::Decode1(this: v2);`
  - `v71 = *(CWvsContext::GetCharacterData(this, a2: &v68) + 4);`
  - `if ( v5 == 0 )`
  - `CWvsContext::GetCharacterData(this, a2: &v59);`
  - `v37 = sub_500EA5(a1: &v67);`
  - `if ( v60 != 0 )`
  - `if ( v38 )`
  - `CWvsContext::GetCharacterData(this, a2: &v57);`
  - `sub_4E694B(a1: a2);`
  - `if ( v58 != 0 )`
  - `CWvsContext::GetCharacterData(this, a2: &v55);`
  - `v39 = sub_500E4C(a1: &a2, a2: 0);`
  - …（截断 14 行）

### CWvsContext::ResetAutoQuest  @ 0xa086d5  (3540 B)

  - `void __thiscall CWvsContext::ResetAutoQuest(int **this)`
  - `if ( TSingleton<CUserLocal>::ms_pInstance != 0 )`
  - `ZMap<unsigned short,int,unsigned short>::RemoveAll(this: this + 3541);`
  - `for ( i = 0; i <= 102; i += 2 )`
  - `v3 = *(&(*(this + 2094))[i + 58] + 3);`
  - `if ( v3 != 0 )`
  - `Data = TSecType<long>::GetData(a1: (v3 + 12), a2: 0);`
  - `CWvsContext::CheckEquipOnAutoStartQuest(a1: Data, a2: 0);`
  - `v5 = *(&(*(this + 2094))[i + 162] + 3);`
  - `if ( v5 != 0 )`
  - `v6 = TSecType<long>::GetData(a1: (v5 + 12), a2: 0);`
  - `CWvsContext::CheckEquipOnAutoStartQuest(a1: v6, a2: 0);`
  - `for ( j = 0; j < 8; j += 2 )`
  - `v8 = *(&(*(this + 2094))[j + 266] + 3);`
  - …（截断 14 行）

### CWvsContext::ResignQuest  @ 0xa26ea7  (2215 B)

- **c2s opcode `COutPacket(107)`**
- c2s 编码：`Encode1`, `Encode2`
- SP_ 节点：`SP_3258_ARE_YOU_SURE_YOU_WANT_TO_QUIT`
  - `char __thiscall CWvsContext::ResignQuest(CWvsContext *this, int a2)`
  - `CWvsContext::GetCharacterData(this, a2: &v15);`
  - `v2 = sub_500EA5(a1: &v17);`
  - `if ( v2 == 0 )`
  - `LOBYTE(v2) = CUtilDlg::YesNo(a1: v8, a2: v9, a3: v10, a4: v11, a5: v12) != 6;`
  - `if ( v2 != 0 )`
  - `if ( v16 != 0 )`
  - `if ( v19 == 0 )`
  - `for ( i = 0; ; ++i )`
  - `v6 = *(dword_BF0FF4 + 1456);`
  - `if ( v6 == 0 || i >= *(v6 - 4) )`
  - `if ( *(v6 + 4 * i) == a2 )`
  - `CUIQuestAlarm::DeleteQuest(this: dword_BF0FF4, a2);`
  - `COutPacket::COutPacket(a1: 107);`
  - …（截断 14 行）

### CWvsContext::SendAbilityUpRequest  @ 0xa239b7  (3892 B)

- **c2s opcode `COutPacket(87)`**
- c2s 编码：`Encode4`
- SP_ 节点：`SP_5259_IF_YOU_INVEST_YOUR_AP_IN_HP_OR_MP_YOUR_CHARACTER_MAY_HAVE_R_NINSUFFICIENT_STATS_`, `SP_883_YOU_CANNOT_CHANGE_YOUR_STATS_ON_THIS_MAP`
  - `int __thiscall CWvsContext::SendAbilityUpRequest(CWvsContext *this, int a2)`
  - `result = CWvsContext::CanSendExclRequest(this, a2: 500, a3: 0);`
  - `if ( result != 0 )`
  - `if ( ForcedStat::IsForced(this: this + 2947) != 0 )`
  - `return CUtilDlg::Notice(sMsg: v14, sSndName: v15, ppDialog: v16, bAutoSeparated: v17, bIsTightLine: v18);`
  - `v6 = *(CWvsContext::GetCharacterData(this, a2: &v21) + 4);`
  - `if ( v22 != 0 )`
  - `result = _ZtlSecureFuse<short>(a1: v6 + 129, a2: *(v6 + 133));`
  - `if ( result > 0 )`
  - `v7 = _ZtlSecureFuse<unsigned char>(a1: v6 + 51, a2: *(v6 + 53));`
  - `v16 = *(v6 + 61);`
  - `result = _ZtlSecureFuse<short>(a1: v6 + 57, a2: v16);`
  - `if ( v25 >= 0x14u || (a2 & 0x2800) == 0 )`
  - `v8 = _ZtlSecureFuse<unsigned char>(a1: v6 + 51, a2: *(v6 + 53));`
  - …（截断 14 行）

### CWvsContext::SendActivatePetRequest  @ 0xa240a2  (9377 B)

- **c2s opcode `COutPacket(80)`**
- **c2s opcode `COutPacket(98)`**
- c2s 编码：`Encode4`, `EncodeBuffer`, `Encode2`, `Encode1`
- SP_ 节点：`SP_266_YOU_CANT_DO_IT_RIGHT_NOW_R_NCATCH_YOUR_BREATH_AND_THEN_TRY_AGAIN`, `SP_378_THE_TIME_HAS_RUN_OUT_SO_IT_CANT_MOVE`, `SP_379_YOU_CANNOT_REVIVE_THIS_PET_R_NYOU_CAN_PURCHASE_A_NEW_PET_R_NIN_THE_CASH_SHOP_WOU`, `SP_384_YOU_HAVE_EXCEEDED_THE_MAXIMUM_NUMBER_R_N_OF_PETS_THAT_CAN_ACCOMPANY_YOU_R_NPLEAS`, `SP_385_WOULD_YOU_LIKE_TO_CHANGE_YOUR_BOSS_PET_R_N_THE_ITEM_IGNORE_SKILL_AS_WELL_AS__R_N`
  - `LONG __thiscall CWvsContext::SendActivatePetRequest(CWvsContext *this, int a2)`
  - `result = CWvsContext::CanSendExclRequest(this, a2: 200, a3: 0);`
  - `if ( result != 0 )`
  - `result = CUser::IsOnLadderOrRope(this: TSingleton<CUserLocal>::ms_pInstance);`
  - `if ( result == 0 )`
  - `if ( *(TSingleton<CUserLocal>::ms_pInstance + 1388) > 0 )`
  - `return CUtilDlg::Notice(sMsg: v31, sSndName: v32, ppDialog: v33, bAutoSeparated: v34, bIsTightLine: v35);`
  - `CharacterData = CWvsContext::GetCharacterData(this, a2: v40);`
  - `v6 = *(CharacterData + 4);`
  - `v7 = *(CharacterData::GetItem(this: v6, a2: v41, a3: 5, a4: a2) + 4);`
  - `sub_4288C1(a1: v41);`
  - `if ( v7 != 0 )`
  - `result = (*(*v7 + 24))(a1: v7);`
  - `if ( sub_4E4003(a1: v7) != 0 )`
  - …（截断 14 行）

### CWvsContext::SendAntiMacroItemUseRequest  @ 0xa262d9  (4668 B)

- **c2s opcode `COutPacket(103)`**
- c2s 编码：`EncodeStr`, `Encode2`, `Encode4`
- SP_ 节点：`SP_150_NOT_AVAILABLE`, `SP_277_YOU_CANT_DO_IT_HERE_IN_THIS_MAP`, `SP_3173_YOU_MAY_NOT_USE_THE_LIE_DETECTOR_ON_THIS_AREA`
  - `void __thiscall CWvsContext::SendAntiMacroItemUseRequest(CWvsContext *this, int *a2, int a3)`
  - `if ( dword_BE7910 != 0 )`
  - `CHATLOG_ADD(sString: String, nType: 0xCu);`
  - `if ( CUserLocal::CheckRidingVehicle(this: TSingleton<CUserLocal>::ms_pInstance) != 0 )`
  - `if ( *(get_field() + 340) != 0 )`
  - `CUtilDlg::Notice(sMsg: v14, sSndName: v15, ppDialog: v16, bAutoSeparated: v17, bIsTightLine: v18);`
  - `v23 = ((*(get_field() + 324) & 0x10000) != 0);`
  - `if ( v23 != 0 )`
  - `CHATLOG_ADD(sString: v9, nType: 0xCu);`
  - `v23 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 148);`
  - `v10 = CUIAntiMacroCharacterName::CUIAntiMacroCharacterName();`
  - `v22 = v11 != 0 ? (v11 - 8) : 0;`
  - `LOBYTE(v26) = 2;`
  - `sub_96DC98(a1: v21);`
  - …（截断 14 行）

### CWvsContext::SendCashSlotItemUseRequest  @ 0xa1dd28  (1152 B)

  - `void __thiscall CWvsContext::SendCashSlotItemUseRequest(CWvsContext *this, int a2, int a3, union _LARGE_INTEGER a4)`
  - `cashslot_item_type = get_cashslot_item_type(a1: a3);`
  - `if ( cashslot_item_type < 8 || cashslot_item_type > 11 && cashslot_item_type != 38 )`
  - `if ( cashslot_item_type != 0 )`
  - `if ( v6 != 0 )`
  - `if ( v7 != 0 )`
  - `if ( v8 != 0 )`
  - `if ( v8 == 27 )`
  - `CWvsContext::SendRemoteShopOpenRequest(this, a2);`
  - `CWvsContext::SendEntrustedShopCheckRequest(this, a2, a3, a4);`
  - `CWvsContext::SendActiveEffectItemChange(a1: a3);`
  - `CWvsContext::SendActivatePetRequest(this, a2);`

### CWvsContext::SendChangeSlotPositionRequest  @ 0xa0900a  (1799 B)

- **c2s opcode `COutPacket(71)`**
- c2s 编码：`Encode4`, `Encode1`, `Encode2`
- SP_ 节点：`SP_4558_YOU_CANNOT_DROP_THE_ITEM_HERE`
  - `void __thiscall CWvsContext::SendChangeSlotPositionRequest(`
  - `if ( dword_BEC1F4 != 0 && *(dword_BEC1F4 + 12) != 0 )`
  - `sub_631A80(this: dword_BEC1F4);`
  - `if ( CWvsContext::CanSendExclRequest(this, a2: 500, a3: 0) )`
  - `if ( a4 != 0 || !(v12 = (*(get_field() + 324) & 0x400000) != 0) )`
  - `COutPacket::COutPacket(a1: 71);`
  - `update_time = get_update_time();`
  - `COutPacket::Encode4(this: &v10, a2: update_time);`
  - `COutPacket::Encode1(this: &v10, a2);`
  - `COutPacket::Encode2(this: &v10, a2: a3);`
  - `COutPacket::Encode2(this: &v10, a2: a4);`
  - `COutPacket::Encode2(this: &v10, a2: a5);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v10);`
  - `*(this + 2089) = 1;`
  - …（截断 14 行）

### CWvsContext::SendCharacterInfoRequest  @ 0xa23fba  (1723 B)

- **c2s opcode `COutPacket(97)`**
- c2s 编码：`Encode4`, `Encode1`
  - `int __thiscall CWvsContext::SendCharacterInfoRequest(CWvsContext *this, int a2, int a3, char a4)`
  - `field = get_field();`
  - `if ( field == 0 || (result = (*(*field + 28))(a1: field)) != 22 )`
  - `result = CWvsContext::CanSendExclRequest(this, a2: 200, a3: 0);`
  - `if ( result != 0 && TSingleton<CUIStatusBar>::ms_pInstance != 0 )`
  - `result = CUserPool::GetRemoteUser(a1: a2);`
  - `if ( a3 != 0 )`
  - `if ( result != 0 )`
  - `COutPacket::COutPacket(a1: 97);`
  - `update_time = get_update_time();`
  - `COutPacket::Encode4(this: &v10, a2: update_time);`
  - `COutPacket::Encode4(this: &v10, a2: v7);`
  - `COutPacket::Encode1(this: &v10, a2: a4);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v10);`
  - …（截断 14 行）

### CWvsContext::SendClaimRequest  @ 0xa2719c  (13417 B)

- **c2s opcode `COutPacket(106)`**
- c2s 编码：`Encode1`, `EncodeStr`
- SP_ 节点：`SP_3385_THE_REPORTING_SYSTEM_IS_CURRENTLY_UNDERGOING_MAINTENENCE_CHECK`, `SP_3391_THERES_NO_CHARACTER_CAN_BE_REPORTED_IN_THIS_MAP`, `SP_3395_YOU_MAY_ONLY_REPORT_FROM_D_TO_D`, `SP_3396_THIS_CHARACTER_IS_UNABLE_TO_REPORT_OTHERS`
  - `void __fastcall CWvsContext::SendClaimRequest(int a1, int edx0, _DWORD *a2, volatile LONG *a4)`
  - `v5 = *(a1 + 12480) == 0;`
  - `if ( v5 )`
  - `CUtilDlg::Notice(sMsg: v37, sSndName: v38, ppDialog: v39, bAutoSeparated: v40, bIsTightLine: v41[0]);`
  - `if ( a4 != 0 )`
  - `dword_BF03B0(a1: &v42);`
  - `v6 = *(v4 + 12476);`
  - `LOBYTE(a1) = *(v4 + 12477);`
  - `if ( (v6 >= a1 || v6 > v43 || v43 >= a1) && (v6 <= a1 || v6 > v43 && v43 >= a1) && (v6 != 0 || a1 != 0) )`
  - `if ( v6 != a1 )`
  - `LOBYTE(v65) = 1;`
  - `v41[0] = *(v4 + 12477);`
  - `LOBYTE(v40) = *(v4 + 12476);`
  - `ZXString<char>::Format(a1: &v60, a2: v35, a3: v40);`
  - …（截断 14 行）

### CWvsContext::SendConsumeCashItemUseRequest  @ 0xa0a63f  (148278 B)

- **c2s opcode `COutPacket(79)`**
- c2s 编码：`Encode2`, `Encode4`, `EncodeStr`, `Encode1`
- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1391_ENTER_A_NOTE`, `SP_1445_TITLE`, `SP_150_NOT_AVAILABLE`, `SP_1803_PATH`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_271_ENTER_YOUR_COMMENTS_THROUGH_THE_MEGAPHONE`, `SP_272_CURSING_IS_NOT_ALLOWED`, `SP_273_USE_THE_MEGAPHONE_TO_TEXT_UP_TO_60_CHARACTERS`, `SP_274_THE_EFFECT_IS_TAKING_PLACE_RIGHT_NOW_R_NPLEASE_TRY_IT_LATER`, `SP_276_YOU_CANT_USE_IT_HERE_IN_THIS_MAP`, `SP_297_OTHER_REQUESTS_ARE_IN_PROCESS_R_NPLEASE_TRY_AGAIN_IN_A_LITTLE_BIT`, `SP_3642_NOT_AVAILABLE_IN_THE_FREE_MARKET`, `SP_3721_PLEASE_ENTER_THE_TEXT`…
  - `void __thiscall CWvsContext::SendConsumeCashItemUseRequest(`
  - `field = get_field();`
  - `if ( field == 0 )`
  - `if ( !CWvsContext::CanSendExclRequest(this: v504, a2: 500, a3: 0) )`
  - `CUtilDlg::Notice(sMsg: v410, sSndName: v411, ppDialog: v412, bAutoSeparated: v413, bIsTightLine: v414[0]);`
  - `COutPacket::COutPacket(a1: 79);`
  - `LOBYTE(v520) = 1;`
  - `COutPacket::Encode2(this: &v511, a2);`
  - `COutPacket::Encode4(this: &v511, a2: a3);`
  - `consume_cash_item_type = get_consume_cash_item_type(a1: v7);`
  - `switch ( consume_cash_item_type )`
  - `if ( consume_cash_item_type == 13 )`
  - `v21 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 164);`
  - `if ( v21 != 0 )`
  - …（截断 14 行）

### CWvsContext::SendCreateMiniGameRequest  @ 0xa1d4d8  (6838 B)

- **c2s opcode `COutPacket(123)`**
- c2s 编码：`Encode1`, `EncodeStr`
- SP_ 节点：`SP_150_NOT_AVAILABLE`, `SP_272_CURSING_IS_NOT_ALLOWED`, `SP_434_YOU_CANT_START_THE_GAME_HERE`
  - `void __thiscall CWvsContext::SendCreateMiniGameRequest(void *this, int a2, int a3)`
  - `if ( CUserLocal::CheckRidingVehicle(this: TSingleton<CUserLocal>::ms_pInstance) == 0 )`
  - `if ( *(TSingleton<CUserLocal>::ms_pInstance + 1320) != 0 || *(TSingleton<CUserLocal>::ms_pInstance + 1340) != 0 )`
  - `CWvsContext::AddChatMorphedMsg(a1: this);`
  - `if ( dword_BE7910 != 0 )`
  - `CHATLOG_ADD(sString: String, nType: 0xCu);`
  - `if ( v6 != 0 )`
  - `if ( CUser::IsStand(a1: TSingleton<CUserLocal>::ms_pInstance) == 0 )`
  - `v31 = (*(get_field() + 324) & 0x80) != 0;`
  - `if ( v31 != 0 )`
  - `CUtilDlg::Notice(sMsg: v21, sSndName: v22, ppDialog: v23, bAutoSeparated: v24, bIsTightLine: v25[0]);`
  - `if ( v9 != 0 )`
  - `v14 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 204);`
  - `LOBYTE(v36) = 5;`
  - …（截断 14 行）

### CWvsContext::SendDropMoneyRequest  @ 0xa23de5  (1339 B)

- **c2s opcode `COutPacket(94)`**
- c2s 编码：`Encode4`
- SP_ 节点：`SP_4558_YOU_CANNOT_DROP_THE_ITEM_HERE`
  - `void __thiscall CWvsContext::SendDropMoneyRequest(CWvsContext *this, int a2)`
  - `if ( CWvsContext::CanSendExclRequest(this, a2: 200, a3: 0) )`
  - `if ( (*(get_field() + 324) & 0x400000) != 0 )`
  - `CHATLOG_ADD(sString: String, nType: 0xCu);`
  - `COutPacket::COutPacket(a1: 94);`
  - `update_time = get_update_time();`
  - `COutPacket::Encode4(this: &v7, a2: update_time);`
  - `COutPacket::Encode4(this: &v7, a2);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v7);`
  - `*(this + 2089) = 1;`
  - `v6 = get_update_time();`
  - `*(this + 2090) = v6;`

### CWvsContext::SendEntrustedShopCheckRequest  @ 0xa27b7d  (4782 B)

- **c2s opcode `COutPacket(63)`**
- c2s 编码：`Encode1`, `EncodeBuffer`
- SP_ 节点：`SP_3475_YOU_MAY_NOT_OPEN_THE_STORE_NEAR_A_PORTAL`, `SP_3746_HIRED_MERCHANTS_SHOP_IS_ONLY_AVAILABLE_FOR__R_NCHARACTERS_THAT_ARE_OVER_LEVEL_15`, `SP_4301_YOU_MAY_NOT_OPEN_THE_STORE_WHILE_JUMPING`, `SP_435_THE_PERSONAL_STORE_CAN_ONLY_BE_BUILT_R_NAT_THE_FREE_MARKET_AVAILABLE_AT_MAIN_TOW`
  - `void __thiscall CWvsContext::SendEntrustedShopCheckRequest(CWvsContext *this, int *a2, int a3, union _LARGE_INTEGER a4)`
  - `if ( CUserLocal::CheckRidingVehicle(this: TSingleton<CUserLocal>::ms_pInstance) == 0 )`
  - `if ( *(TSingleton<CUserLocal>::ms_pInstance + 1320) != 0 || *(TSingleton<CUserLocal>::ms_pInstance + 1340) != 0 )`
  - `CWvsContext::AddChatMorphedMsg(a1: v31);`
  - `if ( *(get_field() + 352) == 0 )`
  - `CUtilDlg::Notice(sMsg: v21, sSndName: v22, ppDialog: v23, bAutoSeparated: v24, bIsTightLine: v25);`
  - `VecCtrl = CUser::GetVecCtrl(this: TSingleton<CUserLocal>::ms_pInstance, a2: &v30);`
  - `v6 = *(*VecCtrl != 0 ? *VecCtrl - 12 + 0x110 : 272) == 0;`
  - `if ( v30 != 0 )`
  - `(*(*v30 + 8))(a1: v30);`
  - `if ( v6 )`
  - `CHATLOG_ADD(sString: String, nType: 0xCu);`
  - `v9 = *(dword_BEBFA8 + 8);`
  - `if ( v9 != 0 )`
  - …（截断 14 行）

### CWvsContext::SendEtcCashItemUseRequest  @ 0xa1dc5b  (1206 B)

  - `char *__thiscall CWvsContext::SendEtcCashItemUseRequest(_DWORD *this, int nPOS, int nItemID)`
  - `result = get_etc_cash_item_type(a1: nItemID);`
  - `if ( result != 0 )`
  - `if ( v5 != 0 )`
  - `if ( v6 != 0 )`
  - `if ( v7 != 0 )`
  - `if ( result == 0 )`
  - `result = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 220);`
  - `return CUIGachaponRemote::CUIGachaponRemote(a1: nItemID);`
  - `return CWvsContext::SendEmotionChange(this, a2: nItemID % 100 + 8);`
  - `return CWvsContext::SendWaterOfLife(a1: this);`
  - `return CWvsContext::SendOpenShopRequest(a1: nPOS, a2: nItemID, a3: 0);`

### CWvsContext::SendExpUpItemUseRequest  @ 0xa12685  (3649 B)

- **c2s opcode `COutPacket(157)`**
- c2s 编码：`Encode4`, `Encode2`
- SP_ 节点：`SP_3177_YOUR_LEVEL_IS_TOO_HIGH_TO_USE_THE_SELECTED_ITEM`, `SP_3815_NOT_AVAILABLE_WHILE_RIDING_ON_THE_TAMED_MONSTER`, `SP_4860_YOULL_NEED_TO_USE_ALL_YOUR_REMAINING_EXP_IN_ORDER_TO_USE_A_NEW_WRIT_OF_SOLOMON`
  - `int *__thiscall CWvsContext::SendExpUpItemUseRequest(CWvsContext *this, int *a2, int *a3)`
  - `if ( *(TSingleton<CUserLocal>::ms_pInstance + 1348) / 10000 == 190 )`
  - `CHATLOG_ADD(sString: String, nType: 0xCu);`
  - `result = timeGetTime();`
  - `if ( result == 0 && dword_BE7910 == 0 )`
  - `result = CWvsContext::IsAbleToConsume(a1: a3, a2: 0);`
  - `if ( result != 0 )`
  - `v8 = _ZtlSecureFuse<unsigned char>(a1: *(this + 2094) + 51, a2: *(*(this + 2094) + 53));`
  - `if ( v8 > CItemInfo::GetMaxLEV(a1: v7) )`
  - `CHATLOG_ADD(sString: v10, nType: 0xCu);`
  - `if ( _ZtlSecureFuse<long>(a1: *(this + 2094) + 177, a2: *(*(this + 2094) + 185)) != 0 )`
  - `CHATLOG_ADD(sString: v12, nType: 0xCu);`
  - `result = CUserLocal::IsAttract(this: TSingleton<CUserLocal>::ms_pInstance);`
  - `if ( result == 0 )`
  - …（截断 14 行）

### CWvsContext::SendFamilyChartRequest  @ 0xa34489  (193 B)

  - `int __stdcall CWvsContext::SendFamilyChartRequest(int a1)`
  - `if ( dword_BF0E6C != 0 )`
  - `return sub_80A5F7(a1);`

### CWvsContext::SendGetUpFromChairRequest  @ 0xa0fbf4  (830 B)

- **c2s opcode `COutPacket(42)`**
- c2s 编码：`Encode2`
  - `int __thiscall CWvsContext::SendGetUpFromChairRequest(CWvsContext *this, int a2)`
  - `result = CWvsContext::CanSendExclRequest(this, a2, a3: 0);`
  - `if ( result != 0 )`
  - `COutPacket::COutPacket(a1: 42);`
  - `COutPacket::Encode2(this: v5, a2: 0xFFFFu);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: v5);`
  - `*(this + 2089) = 1;`
  - `update_time = get_update_time();`
  - `*(this + 2090) = update_time;`

### CWvsContext::SendGivePopularityRequest  @ 0xa23eb5  (1924 B)

- **c2s opcode `COutPacket(95)`**
- c2s 编码：`Encode4`, `Encode1`
- SP_ 节点：`SP_307_PLEASE_TRY_IT_AGAIN_LATER`, `SP_366_UNABLE_TO_FIND_THE_CHARACTER`
  - `void __thiscall CWvsContext::SendGivePopularityRequest(_DWORD *this, const char *sName, char bInc)`
  - `if ( TSingleton<CUIStatusBar>::ms_pInstance != 0 )`
  - `v4 = (dword_BF060C)();`
  - `RemoteUserByName = CUserPool::FindRemoteUserByName(this: dword_BEBFA8, sName);`
  - `if ( RemoteUserByName == 0 )`
  - `CHATLOG_ADD(sString: String, nType: 0xCu);`
  - `if ( v4 - *(this + 3401) < 300000 )`
  - `CHATLOG_ADD(sString: v10, nType: 0xCu);`
  - `*(this + 3401) = v4;`
  - `COutPacket::COutPacket(a1: 95);`
  - `v11 = *(RemoteUserByName + 4520);`
  - `COutPacket::Encode4(this: &v12, a2: v11);`
  - `COutPacket::Encode1(this: &v12, a2: bInc);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v12);`
  - …（截断 14 行）

### CWvsContext::SendMigrateToShopRequest  @ 0xa04dca  (3653 B)

- c2s 编码：`Encode4`
- SP_ 节点：`SP_266_YOU_CANT_DO_IT_RIGHT_NOW_R_NCATCH_YOUR_BREATH_AND_THEN_TRY_AGAIN`, `SP_277_YOU_CANT_DO_IT_HERE_IN_THIS_MAP`, `SP_3172_YOU_CANNOT_USE_IT_IN_THE_MIDDLE_OF_THE_LIE_DETECTOR_TEST`, `SP_3922_YOU_CANT_DO_THIS_WHILE_TAKING_THE_QUIZ`
  - `void __fastcall CWvsContext::SendMigrateToShopRequest(_DWORD *this, int edx0, _DWORD **a3)`
  - `if ( *(this + 2084) != 0 )`
  - `ZXString<char>::Format(a1: &a3, a2: aTheCashShopIsN, a3: v19);`
  - `CHATLOG_ADD(sString: &a3, nType: 0xCu);`
  - `if ( TSingleton<CUserLocal>::ms_pInstance == 0 )`
  - `if ( *(TSingleton<CUserLocal>::ms_pInstance + 1388) > 0 )`
  - `CUtilDlg::Notice(sMsg: v14, sSndName: v15, ppDialog: v16, bAutoSeparated: v17, bIsTightLine: v18);`
  - `if ( *(this + 2089) == 0 && get_update_time() - *(this + 2090) >= 500 )`
  - `CHATLOG_ADD(sString: String, nType: 0xCu);`
  - `CHATLOG_ADD(sString: v8, nType: 0xCu);`
  - `v22 = (*(get_field() + 324) & 0x10) != 0;`
  - `if ( v22 )`
  - `*(this + 3514) = a3;`
  - `COutPacket::COutPacket(a1: v18);`
  - …（截断 14 行）

### CWvsContext::SendMobSummonItemUseRequest  @ 0xa0977b  (3483 B)

- **c2s opcode `COutPacket(75)`**
- c2s 编码：`Encode4`, `Encode2`
- SP_ 节点：`SP_276_YOU_CANT_USE_IT_HERE_IN_THIS_MAP`, `SP_279_TOO_MUCH_INTEREST_CAN_HINDER_YOUR_PROCESS_OF_LEVELING_UP_R_N_R_NWILL_YOU_REALLY_`
  - `signed int __thiscall CWvsContext::SendMobSummonItemUseRequest(CWvsContext *this, unsigned __int16 a2, int a3)`
  - `result = CWvsContext::IsAbleToConsume(a1: a3, a2: 1);`
  - `if ( result != 0 )`
  - `result = CUserLocal::CheckRidingVehicle(this: TSingleton<CUserLocal>::ms_pInstance);`
  - `if ( result == 0 )`
  - `field = get_field();`
  - `TSecType<unsigned long>::GetData(a1: field + 312);`
  - `if ( (TSecType<unsigned char>::GetData(this: this + 2060) & 1) != 0`
  - `|| !(v19 = (*(get_field() + 324) & 4) != 0)`
  - `|| (v19 = a3, get_field(), sub_5DCF70(a1: &v19, a2: 0) != 0) )`
  - `if ( (TSecType<unsigned char>::GetData(this: this + 2060) & 1) != 0 )`
  - `if ( a3 / 1000 == 2109 )`
  - `if ( a3 == 2100067 )`
  - `result = CUtilDlg::YesNo(a1: v11, a2: v12, a3: v13, a4: v14, a5: v15);`
  - …（截断 14 行）

### CWvsContext::SendOpenShopRequest  @ 0xa1dd99  (10548 B)

- **c2s opcode `COutPacket(123)`**
- c2s 编码：`Encode1`, `EncodeStr`, `Encode2`, `Encode4`
- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_148_NOT_AVAILABLE_WITH_AN_ADMIN_ACCOUNT`, `SP_150_NOT_AVAILABLE`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_2305_USESHOPITEM`, `SP_272_CURSING_IS_NOT_ALLOWED`, `SP_3498_HELLO_IM_FREDRICK_THE_UNION_CHIEF_OF_THE_HIRED_MERCHANT_UNION_THE_WAY_TO_USE_HIR`, `SP_396_OTHER_REQUESTS_ARE_BEING_FULFILLED_THIS_MINUTE`, `SP_414_PLEASE_ENTER_THE_NAME_OF_THE_STORE_R_N_R_NIF_YOU_MOVE_BEFORE_YOU_OPEN_THE_STORE_`, `SP_435_THE_PERSONAL_STORE_CAN_ONLY_BE_BUILT_R_NAT_THE_FREE_MARKET_AVAILABLE_AT_MAIN_TOW`
  - `signed int __thiscall CWvsContext::SendOpenShopRequest(`
  - `result = CUserLocal::CheckRidingVehicle(this: TSingleton<CUserLocal>::ms_pInstance);`
  - `if ( result == 0 )`
  - `if ( *(TSingleton<CUserLocal>::ms_pInstance + 1320) != 0 || *(TSingleton<CUserLocal>::ms_pInstance + 1340) != 0 )`
  - `return CWvsContext::AddChatMorphedMsg(a1: this);`
  - `if ( (TSecType<unsigned char>::GetData(this: this + 2060) & 1) != 0 || *(this + 2085) != 0 )`
  - `CHATLOG_ADD(sString: String, nType: 0xCu);`
  - `if ( result != 0 )`
  - `if ( dword_BE7910 != 0 )`
  - `CHATLOG_ADD(sString: v7, nType: 0xCu);`
  - `if ( *(get_field() + 352) != 0 )`
  - `result = CUser::IsStand(a1: TSingleton<CUserLocal>::ms_pInstance);`
  - `v10 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v10 != 0 )`
  - …（截断 14 行）

### CWvsContext::SendPetFoodItemUseRequest  @ 0xa09905  (2733 B)

- **c2s opcode `COutPacket(76)`**
- c2s 编码：`Encode4`, `Encode2`
- SP_ 节点：`SP_2304_EAT`
  - `int __thiscall CWvsContext::SendPetFoodItemUseRequest(CWvsContext *this, signed int a2, int a3)`
  - `result = CWvsContext::CanSendExclRequest(this, a2: 200, a3: 0);`
  - `if ( result != 0 )`
  - `COutPacket::COutPacket(a1: 76);`
  - `update_time = get_update_time();`
  - `COutPacket::Encode4(this: &v18, a2: update_time);`
  - `COutPacket::Encode2(this: &v18, a2);`
  - `COutPacket::Encode4(this: &v18, a2: a3);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v18);`
  - `*(this + 2089) = 1;`
  - `*(this + 2090) = get_update_time();`
  - `v8 = (TSingleton<CUserLocal>::ms_pInstance + 7900);`
  - `v9 = *(*(TSingleton<CUserLocal>::ms_pInstance + 7900) + 4);`
  - `if ( v9 != 0 )`
  - …（截断 14 行）

### CWvsContext::SendRemoteShopOpenRequest  @ 0xa24606  (647 B)

- **c2s opcode `COutPacket(59)`**
- c2s 编码：`Encode2`
  - `void *__thiscall CWvsContext::SendRemoteShopOpenRequest(CWvsContext *this, unsigned __int16 a2)`
  - `if ( dword_BE7910 == 0 )`
  - `COutPacket::COutPacket(a1: 59);`
  - `COutPacket::Encode2(this: v3, a2);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: v3);`

### CWvsContext::SendShopScannerItemUseRequest  @ 0xa0a25e  (839 B)

- **c2s opcode `COutPacket(83)`**
- c2s 编码：`Encode2`, `Encode4`
  - `int __thiscall CWvsContext::SendShopScannerItemUseRequest(CWvsContext *this, unsigned __int16 a2, int a3)`
  - `if ( a3 / 10000 == 231 )`
  - `result = CWvsContext::CanSendExclRequest(this, a2: 200, a3: 0);`
  - `if ( result != 0 )`
  - `COutPacket::COutPacket(a1: 83);`
  - `COutPacket::Encode2(this: &v5, a2);`
  - `COutPacket::Encode4(this: &v5, a2: a3);`
  - `CWvsContext::RunShopScanner(this, a2: &v5);`

### CWvsContext::SendSitOnPortableChairRequest  @ 0xa0f9e2  (2759 B)

- **c2s opcode `COutPacket(43)`**
- c2s 编码：`Encode4`
- SP_ 节点：`SP_4269_IT_IS_NOT_APPROPRIATE_TO_USE_IT_HERE`
  - `void __thiscall CWvsContext::SendSitOnPortableChairRequest(CWvsContext *this, int a2, int *a3)`
  - `if ( CWvsContext::CanSendExclRequest(this, a2: 200, a3: 0) )`
  - `if ( timeGetTime() == 0 && v4[2746] == 0 && v4[347] <= 0 && CUser::IsStand(a1: v4) != 0 )`
  - `if ( CAvatar::IsMonsterMorphed(this: (v4 + 34)) || CAvatar::IsHideMorphed(this: v4 + 34) )`
  - `CWvsContext::AddChatMorphedMsg(a1: this);`
  - `else if ( CUserLocal::CheckRidingVehicle(this: v4) == 0 )`
  - `field = get_field();`
  - `if ( field == 0 || (*(*field + 28))(a1: field) == 61 )`
  - `CHATLOG_ADD(sString: String, nType: 0xCu);`
  - `v6 = _ZtlSecureFuse<unsigned char>(a1: *(this + 2094) + 51, a2: *(*(this + 2094) + 53));`
  - `if ( CItemInfo::GetRequiredLEV(a1: a3) <= v6 )`
  - `COutPacket::COutPacket(a1: 43);`
  - `COutPacket::Encode4(this: &v13, a2: a3);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v13);`
  - …（截断 14 行）

### CWvsContext::SendSkillUpRequest  @ 0xa23cf3  (925 B)

- **c2s opcode `COutPacket(90)`**
- c2s 编码：`Encode4`
- 节流 `CanSendExclRequest(500)`
  - `int __thiscall CWvsContext::SendSkillUpRequest(CWvsContext *this, int a2)`
  - `result = CWvsContext::CanSendExclRequest(a1: 500, a2: 0);`
  - `if ( result != 0 )`
  - `COutPacket::COutPacket(a1: 90);`
  - `update_time = get_update_time();`
  - `COutPacket::Encode4(this: &v6, a2: update_time);`
  - `COutPacket::Encode4(this: &v6, a2);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v6);`
  - `*(this + 2089) = 1;`
  - `v5 = get_update_time();`
  - `*(this + 2090) = v5;`

### CWvsContext::SendStatChangeItemCancelRequest  @ 0xa096af  (1180 B)

- **c2s opcode `COutPacket(73)`**
- c2s 编码：`Encode4`
- SP_ 节点：`SP_4348_YOU_MAY_NOT_CANCEL_THE_TRANSFORMATION`
  - `void __thiscall CWvsContext::SendStatChangeItemCancelRequest(CWvsContext *this, int a2)`
  - `if ( CWvsContext::CanSendExclRequest(this, a2: 200, a3: 0) )`
  - `if ( a2 / -10000 == 221`
  - `&& *(TSingleton<CUserLocal>::ms_pInstance + 1320) != 0`
  - `&& CItemInfo::IsNoCancelMouse(this: dword_BE78D8, a2: -a2) != 0 )`
  - `CHATLOG_ADD(sString: String, nType: 0xCu);`
  - `COutPacket::COutPacket(a1: 73);`
  - `COutPacket::Encode4(this: &v4, a2);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v4);`

### CWvsContext::SendStatChangeItemUseRequest  @ 0xa092fb  (4176 B)

- **c2s opcode `COutPacket(72)`**
- c2s 编码：`Encode4`, `Encode2`
- SP_ 节点：`SP_3815_NOT_AVAILABLE_WHILE_RIDING_ON_THE_TAMED_MONSTER`, `SP_4077_THIS_ITEM_CANNOT_BE_USED_HERE`
  - `int __thiscall CWvsContext::SendStatChangeItemUseRequest(CWvsContext *this, int a2, int a3)`
  - `if ( dword_BEC1F4 != 0 && *(dword_BEC1F4 + 12) != 0 )`
  - `sub_631A80(this: dword_BEC1F4);`
  - `v18 = (*(get_field() + 324) & 0x400) != 0;`
  - `if ( v18 != 0 )`
  - `get_field();`
  - `if ( sub_5DCF70(a1: &v18, a2: 0) == 0 )`
  - `CHATLOG_ADD(sString: String, nType: 0xCu);`
  - `v18 = *(TSingleton<CUserLocal>::ms_pInstance + 1348) / 10000;`
  - `if ( v18 == 190 )`
  - `CHATLOG_ADD(sString: v9, nType: 0xCu);`
  - `if ( !v6 )`
  - `if ( a3 / 10000 == 221 || result == 236 )`
  - `result = timeGetTime();`
  - …（截断 14 行）

### CWvsContext::ShowQuestInfoDetail  @ 0xa2a0be  (1305 B)

  - `int __userpurge CWvsContext::ShowQuestInfoDetail@<eax>(CWvsContext *this@<ecx>, int a2, unsigned __int16 a3)`
  - `v5 = *(CWvsContext::GetCharacterData(this, a2: &v13) + 4);`
  - `if ( v14 != 0 )`
  - `CurFieldID = CWvsContext::GetCurFieldID(a1: this);`
  - `if ( v3 == 0 || v5 == 0 || CurFieldID == -1 )`
  - `if ( sub_500EA5(a1: &v15) != 0 )`
  - `if ( sub_500E4C(a1: &a2, a2: 0) != 0 )`
  - `if ( sub_83DB26(a1: a2) != 0`
  - `&& (v12 = v14,`
  - `v10 = *(*dword_BE7918 + 14272),`
  - `v8 = sub_83DB26(a1: a2),`
  - `CQuestMan::CheckStartDemand(a1: v3, a2: v9, a3: a2, Data: *(v8 + 24), a5: v5, a6: this + 8500, a7: v10, a8: v12) != 0) )`

### CWvsContext::StartQuest  @ 0xa26e31  (445 B)

  - `void __stdcall CWvsContext::StartQuest(int usQuestID, char *dwNpcTemplateID, int bAutoStart)`
  - `v3 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 44);`
  - `if ( v3 != 0 )`
  - `v4 = CQuest::CQuest(this: v3, a2: usQuestID, a3: dwNpcTemplateID);`
  - `sub_A2B77B(a1: v4);`
  - `CQuest::StartQuest(this: 0, a2: bAutoStart);`

### CWvsContext::TryRegisterAutoStartQuest  @ 0xa08af6  (1965 B)

  - `void __thiscall CWvsContext::TryRegisterAutoStartQuest(CWvsContext *this, int a2, int a3)`
  - `if ( dword_BED614 != 0 )`
  - `v7 = *(CWvsContext::GetCharacterData(this, a2: &v8) + 4);`
  - `if ( CurFieldID != 0 )`
  - `CurFieldID = CWvsContext::GetCurFieldID(a1: this);`
  - `v5 = *(sub_83DB26(a1: a2) + 24);`
  - `if ( CQuestMan::CheckStartDemand(`
  - `a7: *(this + 3568),`
  - `if ( !CQuestMan::IsAutoAutoAcceptQuest(this: v10, usQuestID: a2)`
  - `|| ZMap<unsigned short,int,unsigned short>::GetAt(this: this + 3547, &a2, a3: 0) != 0 )`
  - `if ( !CQuestMan::IsAutoAutoAcceptQuest(this: v10, usQuestID: a2) )`
  - `if ( a3 != 0 && ZMap<unsigned short,int,unsigned short>::GetAt(this: this + 3541, &a2, a3: 0) == 0 )`
  - `*(this + 3559) = 1;`
  - `ZMap<unsigned short,int,unsigned short>::Insert(this: this + 3541, &a2, &a3);`
  - …（截断 14 行）

### CWvsContext::UpdateAutoQuestAlertIcon  @ 0xa08d30  (653 B)

  - `CWvsContext *__thiscall CWvsContext::UpdateAutoQuestAlertIcon(CWvsContext *this)`
  - `if ( TSingleton<CUserLocal>::ms_pInstance != 0 )`
  - `if ( *(this + 3544) != 0 || *(this + 3556) != 0 || *(this + 3564) != 0 )`
  - `return sub_95CED9();`
  - `*(TSingleton<CUserLocal>::ms_pInstance + 12668) = 0;`
  - `sub_428712(a1: 0);`
  - `*(*dword_BE7918 + 14236) = 0;`
  - `*(result + 3567) = 0;`

### UMedalQuestInfo::0?$ZRef  @ 0x8456bf  (270 B)

  - `result = *(this + 1);`
  - `if ( result != 0 )`
  - `return InterlockedIncrement(lpAddend: (*(this + 1) != 0 ? result - 16 + 4 : 4));`

## 2. 反编译中出现的 c2s opcode 常量汇总（COutPacket 构造参数）

| opcode (dec) | hex | 来源函数 |
| --- | --- | --- |
| 26 | 0x1A | CSecurityClient::OnCheckClientIntegrityRequest |
| 38 | 0x26 | CField::SendTransferFieldRequest |
| 39 | 0x27 | CField::SendTransferChannelRequest |
| 42 | 0x2A | CWvsContext::SendGetUpFromChairRequest |
| 43 | 0x2B | CWvsContext::SendSitOnPortableChairRequest |
| 58 | 0x3A | CNpc::ShowQuestList |
| 59 | 0x3B | CWvsContext::SendRemoteShopOpenRequest |
| 62 | 0x3E | CTrunkDlg::SendGetItemRequest, CTrunkDlg::SendGetMoneyRequest, CTrunkDlg::SendPutItemRequest, CTrunkDlg::SendPutMoneyRequest… |
| 63 | 0x3F | CWvsContext::SendEntrustedShopCheckRequest |
| 68 | 0x44 | CAdminShopDlg::SendTradeRequest |
| 71 | 0x47 | CWvsContext::SendChangeSlotPositionRequest |
| 72 | 0x48 | CWvsContext::SendStatChangeItemUseRequest |
| 73 | 0x49 | CWvsContext::SendStatChangeItemCancelRequest |
| 75 | 0x4B | CWvsContext::SendMobSummonItemUseRequest |
| 76 | 0x4C | CWvsContext::SendPetFoodItemUseRequest |
| 79 | 0x4F | CItemSpeakerDlg::_SendConsumeCashItemUseRequest, CWvsContext::SendConsumeCashItemUseRequest |
| 80 | 0x50 | CWvsContext::SendActivatePetRequest |
| 83 | 0x53 | CWvsContext::SendShopScannerItemUseRequest |
| 87 | 0x57 | CWvsContext::SendAbilityUpRequest |
| 90 | 0x5A | CWvsContext::SendSkillUpRequest |
| 91 | 0x5B | CUserLocal::SendSkillUseRequest |
| 92 | 0x5C | CUserLocal::SendSkillCancelRequest |
| 94 | 0x5E | CWvsContext::SendDropMoneyRequest |
| 95 | 0x5F | CWvsContext::SendGivePopularityRequest |
| 97 | 0x61 | CWvsContext::SendCharacterInfoRequest |
| 98 | 0x62 | CWvsContext::SendActivatePetRequest |
| 103 | 0x67 | CWvsContext::SendAntiMacroItemUseRequest |
| 106 | 0x6A | CWvsContext::SendClaimRequest |
| 107 | 0x6B | CQuest::OnCompleteQuestFailed, CQuest::StartQuest, CWvsContext::ResignQuest |
| 113 | 0x71 | CUIItemMaker::RequestItemMake |
| 123 | 0x7B | CWvsContext::SendCreateMiniGameRequest, CWvsContext::SendOpenShopRequest |
| 137 | 0x89 | CWvsContext::OnMarriageRequest |
| 153 | 0x99 | CWvsContext::OnFamilySummonRequest |
| 155 | 0x9B | CUIGuildBBS::SendLoadListRequest, CUIGuildBBS::SendViewEntryRequest |
| 157 | 0x9D | CWvsContext::SendExpUpItemUseRequest |
| 218 | 0xDA | CUIMonsterCarnival::RequestSend |
