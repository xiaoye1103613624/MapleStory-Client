void __cdecl CShopDlg::OnPacket(ZXString<char> *nType, CInPacket *iPacket)
{
  CShopDlg *v2; // esi
  CInPacket *v3; // edi
  CInPacket *v4; // ecx
  char *v5; // ecx
  int m_nSnapshotTI; // eax
  int tab_from_item_typeindex; // eax
  CCtrlTab *p; // ecx
  int m_nCurPos; // ebx
  int v10; // eax
  int v11; // edi
  char *v12; // ecx
  unsigned int v13; // esi
  StringPool *Instance; // eax
  ZXString<char> *String; // eax
  const char *m_pStr; // eax
  ZXString<char> *v17; // eax
  char *v18; // esi
  StringPool *v19; // eax
  ZXString<char> *v20; // eax
  const char *v21; // eax
  ZXString<char> *v22; // eax
  StringPool *v23; // eax
  CShopDlg *v24; // eax
  CShopDlg *v25; // eax
  StringPoolStrings v26; // [esp-18h] [ebp-58h]
  ZXString<char> v27; // [esp-14h] [ebp-54h] BYREF
  const wchar_t *v28; // [esp-10h] [ebp-50h]
  ZRef<CDialog> *v29; // [esp-Ch] [ebp-4Ch]
  int v30; // [esp-8h] [ebp-48h]
  int v31; // [esp-4h] [ebp-44h]
  ZXString<char> v32; // [esp+10h] [ebp-30h] BYREF
  ZXString<char> v33; // [esp+14h] [ebp-2Ch] BYREF
  CMSException v34; // [esp+18h] [ebp-28h] BYREF
  HRESULT pExceptionObject; // [esp+1Ch] [ebp-24h] BYREF
  ZXString<char> result; // [esp+20h] [ebp-20h] BYREF
  ZXString<char> v37; // [esp+24h] [ebp-1Ch] BYREF
  CMSException v38; // [esp+28h] [ebp-18h] BYREF
  HRESULT m_hr; // [esp+2Ch] [ebp-14h] BYREF
  ZXString<char> *v40; // [esp+30h] [ebp-10h]
  int v41; // [esp+3Ch] [ebp-4h]

  if ( nType == (ZXString<char> *)364 ) /*0x6eb7ff*/
  {
    if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr ) /*0x6ebbf7*/
    {
      CMSException::CMSException(this: &v38, hr: 553648131); /*0x6ebc02*/
      m_hr = v38.m_hr; /*0x6ebc15*/
      _CxxThrowException(pExceptionObject: &m_hr, pThrowInfo: &_TI3_AVCDisconnectException__); /*0x6ebc19*/
    }
    v24 = (CShopDlg *)ZAllocEx<ZAllocAnonSelector>::Alloc(this: &ZAllocEx<ZAllocAnonSelector>::_s_alloc, uSize: 0xB88u); /*0x6ebc28*/
    nType = (ZXString<char> *)v24; /*0x6ebc2d*/
    v41 = 0; /*0x6ebc31*/
    if ( v24 != nullptr ) /*0x6ebc3b*/
      CShopDlg::CShopDlg(this: v24); /*0x6ebc3f*/
    else
      v25 = nullptr; /*0x6ebc46*/
    CShopDlg::SetShopDlg(this: v25, iPacket); /*0x6ebc57*/
  }
  else if ( nType == (ZXString<char> *)365 /*0x6eb82c*/
         && TSingleton<CUniqueModeless>::ms_pInstance != nullptr
         && TSingleton<CUniqueModeless>::ms_pInstance->CDialog::CWnd::IUIMsgHandler::vfptr->IsKindOf(
              this: &TSingleton<CUniqueModeless>::ms_pInstance->IUIMsgHandler,
              a2: &CShopDlg::ms_RTTI_CShopDlg) != 0 )
  {
    v2 = (CShopDlg *)TSingleton<CUniqueModeless>::ms_pInstance; /*0x6eb832*/
    if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr ) /*0x6eb83a*/
    {
      if ( TSingleton<CUniqueModeless>::ms_pInstance[1].m_pBackgrnd.m_pInterface == nullptr ) /*0x6eb847*/
      {
        CMSException::CMSException(this: &v34, hr: 553648131); /*0x6eb852*/
        pExceptionObject = v34.m_hr; /*0x6eb865*/
        _CxxThrowException(&pExceptionObject, pThrowInfo: &_TI3_AVCDisconnectException__); /*0x6eb869*/
      }
      v3 = iPacket; /*0x6eb86e*/
      v4 = iPacket; /*0x6eb872*/
      TSingleton<CUniqueModeless>::ms_pInstance[1].m_pBackgrnd.m_pInterface = nullptr; /*0x6eb874*/
      switch ( CInPacket::Decode1(this: v4) ) /*0x6eb88f*/
      {
        case 0: /*0x6eb88f*/
          m_nSnapshotTI = v2->m_nSnapshotTI; /*0x6eb896*/
          if ( m_nSnapshotTI > 0 && m_nSnapshotTI <= 5 ) /*0x6eb8a7*/
          {
            tab_from_item_typeindex = get_tab_from_item_typeindex(nTI: v2->m_nSnapshotTI); /*0x6eb8ae*/
            p = v2->m_pTabSell.p; /*0x6eb8b3*/
            if ( p->m_nCurTab != tab_from_item_typeindex ) /*0x6eb8bf*/
              CCtrlTab::SetTab(this: p, nTab: tab_from_item_typeindex); /*0x6eb8c2*/
            m_nCurPos = v2->m_pSBSell.p->m_nCurPos; /*0x6eb8cd*/
            v10 = CShopDlg::CmpSellItem(this: v2); /*0x6eb8d2*/
            v11 = v10; /*0x6eb8d7*/
            if ( v10 < m_nCurPos ) /*0x6eb8db*/
              CCtrlScrollBar::SetCurPos(this: v2->m_pSBSell.p, nPos: v10); /*0x6eb8e4*/
            if ( v11 > m_nCurPos + 4 ) /*0x6eb8ee*/
              CCtrlScrollBar::SetCurPos(this: v2->m_pSBSell.p, nPos: v11 - 4); /*0x6eb8fa*/
            v2->m_nSellSelected = v11; /*0x6eb8ff*/
          }
          return; /*0x6eb917*/
        case 1: /*0x6eb88f*/
        case 5: /*0x6eb88f*/
        case 9: /*0x6eb88f*/
          v31 = 0; /*0x6eb918*/
          v30 = 0; /*0x6eb91a*/
          v29 = nullptr; /*0x6eb91c*/
          v28 = nullptr; /*0x6eb91e*/
          v27._m_pStr = v5; /*0x6eb920*/
          v40 = &v27; /*0x6eb923*/
          v26 = SP_869_YOU_DONT_HAVE_ENOUGH_IN_STOCK; /*0x6eb927*/
          goto LABEL_38; /*0x6eb92c*/
        case 2: /*0x6eb88f*/
        case 10: /*0x6eb88f*/
          v31 = 0; /*0x6eb931*/
          v30 = 0; /*0x6eb933*/
          v29 = nullptr; /*0x6eb935*/
          v28 = nullptr; /*0x6eb937*/
          v27._m_pStr = v5; /*0x6eb939*/
          v40 = &v27; /*0x6eb93c*/
          v26 = SP_6795_YOU_DO_NOT_HAVE_ENOUGH_MESOS; /*0x6eb940*/
          goto LABEL_38; /*0x6eb946*/
        case 3: /*0x6eb88f*/
          v31 = 0; /*0x6eb94b*/
          v30 = 0; /*0x6eb94d*/
          v29 = nullptr; /*0x6eb94f*/
          v28 = nullptr; /*0x6eb951*/
          v27._m_pStr = v5; /*0x6eb953*/
          v40 = &v27; /*0x6eb956*/
          v26 = SP_870_PLEASE_CHECK_IF_YOUR_INVENTORY_IS_FULL_OR_NOT; /*0x6eb95a*/
          goto LABEL_38; /*0x6eb960*/
        case 4: /*0x6eb88f*/
        case 8: /*0x6eb88f*/
          return;
        case 13: /*0x6eb88f*/
          v31 = 0; /*0x6eba28*/
          v30 = 0; /*0x6eba2a*/
          v29 = nullptr; /*0x6eba2c*/
          v28 = nullptr; /*0x6eba2e*/
          v27._m_pStr = v5; /*0x6eba30*/
          v40 = &v27; /*0x6eba33*/
          v26 = SP_5439_YOU_NEED_MORE_ITEMS; /*0x6eba37*/
          goto LABEL_38; /*0x6eba3c*/
        case 14: /*0x6eb88f*/
          v31 = 0; /*0x6eba48*/
          v30 = 0; /*0x6eba4a*/
          v29 = nullptr; /*0x6eba4c*/
          v28 = nullptr; /*0x6eba4e*/
          v13 = CInPacket::Decode4(this: v3); /*0x6eba5a*/
          Instance = StringPool::GetInstance(); /*0x6eba5c*/
          String = StringPool::GetString( /*0x6eba63*/
                     this: Instance,
                     &result,
                     nIdx: SP_5455_YOU_MUST_BE_UNDER_LVD_TO_PURCHASE_THIS_ITEM);
          v41 = 2; /*0x6eba6d*/
          v32._m_pStr = nullptr; /*0x6eba71*/
          m_pStr = String->_m_pStr; /*0x6eba79*/
          v27._m_pStr = (char *)-v13; /*0x6eba7d*/
          LOBYTE(v41) = 3; /*0x6eba84*/
          v17 = ZXString<char>::Format(this: &v32, sFormat: m_pStr); /*0x6eba89*/
          nType = &v27; /*0x6eba93*/
          ZXString<char>::ZXString<char>(this: &v27, s: v17); /*0x6eba98*/
          CUtilDlg::Notice(sMsg: v27, sSndName: v28, ppDialog: v29, bAutoSeparated: v30, bIsTightLine: v31); /*0x6eba9d*/
          LOBYTE(v41) = 2; /*0x6ebaa9*/
          if ( v32._m_pStr != nullptr ) /*0x6ebaaf*/
            ZXString<char>::_Release(pData: (ZXString<char>::_ZXStringData *)v32._m_pStr - 1); /*0x6ebab5*/
          v41 = -1; /*0x6ebac1*/
          if ( result._m_pStr != nullptr ) /*0x6ebacb*/
            ZXString<char>::_Release(pData: (ZXString<char>::_ZXStringData *)result._m_pStr - 1); /*0x6ebad5*/
          return; /*0x6ebaef*/
        case 15: /*0x6eb88f*/
          v31 = 0; /*0x6ebaf7*/
          v30 = 0; /*0x6ebaf9*/
          v29 = nullptr; /*0x6ebafb*/
          v28 = nullptr; /*0x6ebafd*/
          v18 = (char *)CInPacket::Decode4(this: v3); /*0x6ebb09*/
          v19 = StringPool::GetInstance(); /*0x6ebb0b*/
          v20 = StringPool::GetString(this: v19, result: &v37, nIdx: SP_5454_YOU_MUST_BE_OVER_LVD_TO_PURCHASE_THIS_ITEM); /*0x6ebb12*/
          v41 = 4; /*0x6ebb1c*/
          v33._m_pStr = nullptr; /*0x6ebb20*/
          v21 = v20->_m_pStr; /*0x6ebb28*/
          v27._m_pStr = v18; /*0x6ebb2a*/
          LOBYTE(v41) = 5; /*0x6ebb31*/
          v22 = ZXString<char>::Format(this: &v33, sFormat: v21); /*0x6ebb36*/
          nType = &v27; /*0x6ebb40*/
          ZXString<char>::ZXString<char>(this: &v27, s: v22); /*0x6ebb45*/
          CUtilDlg::Notice(sMsg: v27, sSndName: v28, ppDialog: v29, bAutoSeparated: v30, bIsTightLine: v31); /*0x6ebb4a*/
          LOBYTE(v41) = 4; /*0x6ebb56*/
          if ( v33._m_pStr != nullptr ) /*0x6ebb5c*/
            ZXString<char>::_Release(pData: (ZXString<char>::_ZXStringData *)v33._m_pStr - 1); /*0x6ebb62*/
          v41 = -1; /*0x6ebb6e*/
          if ( v37._m_pStr != nullptr ) /*0x6ebb78*/
            ZXString<char>::_Release(pData: (ZXString<char>::_ZXStringData *)v37._m_pStr - 1); /*0x6ebb82*/
          return; /*0x6ebb9c*/
        case 16: /*0x6eb88f*/
          v31 = 0; /*0x6eb965*/
          v30 = 0; /*0x6eb967*/
          v29 = nullptr; /*0x6eb969*/
          v28 = nullptr; /*0x6eb96b*/
          v27._m_pStr = v5; /*0x6eb96d*/
          v40 = &v27; /*0x6eb970*/
          v26 = SP_872_YOU_CAN_NO_LONGER_PURCHASE_THIS_ITEM; /*0x6eb974*/
          goto LABEL_38; /*0x6eb979*/
        case 17: /*0x6eb88f*/
          v31 = 0; /*0x6ebb9d*/
          v30 = 0; /*0x6ebb9f*/
          v29 = nullptr; /*0x6ebba1*/
          v28 = nullptr; /*0x6ebba3*/
          v27._m_pStr = v5; /*0x6ebba5*/
          v40 = &v27; /*0x6ebba8*/
          v26 = SP_5869_ITEMS_OR_MESOS_CANNOT_BE_MOVED_R_NPLEASE_CONTACT_CUSTOMER_SUPPORT; /*0x6ebbac*/
          goto LABEL_38; /*0x6ebbb2*/
        case 18: /*0x6eb88f*/
          v31 = 0; /*0x6eb97e*/
          v30 = 0; /*0x6eb980*/
          v29 = nullptr; /*0x6eb982*/
          v28 = nullptr; /*0x6eb984*/
          v27._m_pStr = v5; /*0x6eb986*/
          v40 = &v27; /*0x6eb989*/
          v26 = SP_4018_PLAYERS_THAT_ARE_LEVEL_15_AND_BELOW__R_NMAY_ONLY_TRADE_1_MILLION_MESOS_PER_DAY__; /*0x6eb98d*/
          goto LABEL_38; /*0x6eb993*/
        case 19: /*0x6eb88f*/
          if ( CInPacket::Decode1(this: v3) == 0 ) /*0x6eb9a4*/
            goto LABEL_37; /*0x6eb9a4*/
          CInPacket::DecodeStr(this: v3, result: (ZXString<char> *)&nType); /*0x6eb9ad*/
          v31 = 0; /*0x6eb9b2*/
          v30 = 1; /*0x6eb9b4*/
          v29 = nullptr; /*0x6eb9b6*/
          v28 = nullptr; /*0x6eb9b8*/
          v27._m_pStr = v12; /*0x6eb9ba*/
          v40 = &v27; /*0x6eb9c1*/
          v41 = 1; /*0x6eb9c6*/
          ZXString<char>::ZXString<char>(this: &v27, s: (ZXString<char> *)&nType); /*0x6eb9ce*/
          CUtilDlg::Notice(sMsg: v27, sSndName: v28, ppDialog: v29, bAutoSeparated: v30, bIsTightLine: v31); /*0x6eb9d3*/
          v41 = -1; /*0x6eb9df*/
          if ( nType != nullptr ) /*0x6eb9e9*/
            ZXString<char>::_Release(pData: (ZXString<char>::_ZXStringData *)&nType[-3]); /*0x6eb9f3*/
          break; /*0x6eba0d*/
        default:
LABEL_37:
          v31 = 0; /*0x6ebbb4*/
          v30 = 0; /*0x6ebbb6*/
          v29 = nullptr; /*0x6ebbb8*/
          v28 = nullptr; /*0x6ebbba*/
          v27._m_pStr = v5; /*0x6ebbbc*/
          v40 = &v27; /*0x6ebbbf*/
          v26 = SP_873_DUE_TO_AN_ERROR_THE_TRADE_DID_NOT_HAPPEN; /*0x6ebbc3*/
LABEL_38:
          v23 = StringPool::GetInstance(); /*0x6ebbc9*/
          StringPool::GetString(this: v23, result: &v27, nIdx: v26); /*0x6ebbd0*/
          CUtilDlg::Notice(sMsg: v27, sSndName: v28, ppDialog: v29, bAutoSeparated: v30, bIsTightLine: v31); /*0x6ebbd5*/
          break; /*0x6ebbef*/
      }
    }
  }
}