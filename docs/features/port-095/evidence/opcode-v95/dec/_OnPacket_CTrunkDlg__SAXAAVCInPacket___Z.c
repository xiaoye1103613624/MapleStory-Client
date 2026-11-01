void __cdecl CTrunkDlg::OnPacket(CInPacket *iPacket)
{
  CInPacket *v1; // edi
  int v2; // ebx
  CUniqueModeless *v3; // eax
  int v4; // esi
  CTrunkDlg *v5; // esi
  CTrunkDlg *v6; // eax
  CTrunkDlg *v7; // eax
  int m_nSnapshotTI; // eax
  unsigned int tab_from_item_typeindex; // eax
  CCtrlTab *p; // ecx
  CCtrlScrollBar *v11; // ebx
  int m_nCurPos; // ebp
  int v13; // eax
  int v14; // edi
  char *v15; // ecx
  char *v16; // ecx
  StringPool *Instance; // eax
  StringPoolStrings v18; // [esp-18h] [ebp-4Ch]
  ZXString<char> v19; // [esp-14h] [ebp-48h] BYREF
  const wchar_t *v20; // [esp-10h] [ebp-44h]
  ZRef<CDialog> *v21; // [esp-Ch] [ebp-40h]
  int v22; // [esp-8h] [ebp-3Ch]
  int v23; // [esp-4h] [ebp-38h]
  ZXString<char> sMsg; // [esp+14h] [ebp-20h] BYREF
  CMSException v25; // [esp+18h] [ebp-1Ch] BYREF
  ZXString<char> *pExceptionObject; // [esp+1Ch] [ebp-18h] BYREF
  CMSException v27; // [esp+20h] [ebp-14h] BYREF
  ZXString<char> *m_hr; // [esp+24h] [ebp-10h] BYREF
  int v29; // [esp+30h] [ebp-4h]

  v1 = iPacket; /*0x76a9b7*/
  v2 = (unsigned __int8)CInPacket::Decode1(this: iPacket); /*0x76a9c2*/
  v3 = TSingleton<CUniqueModeless>::ms_pInstance; /*0x76a9c5*/
  if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr )
  {
    v4 = TSingleton<CUniqueModeless>::ms_pInstance->CDialog::CWnd::IUIMsgHandler::vfptr->IsKindOf( /*0x76a9df*/
           this: &TSingleton<CUniqueModeless>::ms_pInstance->IUIMsgHandler,
           a2: &CTrunkDlg::ms_RTTI_CTrunkDlg);
    v3 = TSingleton<CUniqueModeless>::ms_pInstance; /*0x76a9e1*/
    v5 = v4 != 0 ? (CTrunkDlg *)TSingleton<CUniqueModeless>::ms_pInstance : nullptr;
  }
  else
  {
    v5 = nullptr; /*0x76a9ee*/
  }
  if ( v2 != 21 && v2 != 22 ) /*0x76a9f8*/
  {
    if ( v5 == nullptr ) /*0x76a9fc*/
      return; /*0x76a9fc*/
    if ( v5->m_bTrunkRequestSent == 0 ) /*0x76aa08*/
    {
      CMSException::CMSException(this: &v25, hr: 553648131); /*0x76aa13*/
      pExceptionObject = (ZXString<char> *)v25.m_hr; /*0x76aa26*/
      _CxxThrowException(&pExceptionObject, pThrowInfo: &_TI3_AVCDisconnectException__); /*0x76aa2a*/
    }
    v5->m_bTrunkRequestSent = 0; /*0x76aa2f*/
    v3 = TSingleton<CUniqueModeless>::ms_pInstance; /*0x76aa35*/
  }
  switch ( v2 ) /*0x76aa46*/
  {
    case 9: /*0x76aa46*/
      m_nSnapshotTI = v5->m_nSnapshotTI; /*0x76aac8*/
      if ( m_nSnapshotTI > 0 && m_nSnapshotTI <= 5 ) /*0x76aad9*/
      {
        tab_from_item_typeindex = get_tab_from_item_typeindex(nTI: v5->m_nSnapshotTI); /*0x76aae0*/
        if ( tab_from_item_typeindex <= 4 ) /*0x76aaeb*/
        {
          p = v5->m_pTab.p; /*0x76aaed*/
          if ( p->m_nCurTab != tab_from_item_typeindex ) /*0x76aaf6*/
            CCtrlTab::SetTab(this: p, nTab: tab_from_item_typeindex); /*0x76aaf9*/
        }
        v11 = v5->m_pSBPut.p; /*0x76aafe*/
        m_nCurPos = v11->m_nCurPos; /*0x76ab04*/
        v13 = CTrunkDlg::CmpPutItem(this: v5); /*0x76ab09*/
        v14 = v13; /*0x76ab0e*/
        if ( v13 < m_nCurPos ) /*0x76ab12*/
          CCtrlScrollBar::SetCurPos(this: v11, nPos: v13); /*0x76ab17*/
        if ( v14 > m_nCurPos + 4 ) /*0x76ab21*/
          CCtrlScrollBar::SetCurPos(this: v5->m_pSBPut.p, nPos: v14 - 4); /*0x76ab2d*/
        v5->m_nPutSelected = v14; /*0x76ab32*/
        v1 = iPacket; /*0x76ab38*/
        goto $LN10_76; /*0x76ab38*/
      }
      return; /*0x76aad9*/
    case 10: /*0x76aa46*/
      v23 = 0; /*0x76ab61*/
      v22 = 0; /*0x76ab62*/
      v21 = nullptr; /*0x76ab63*/
      v20 = nullptr; /*0x76ab64*/
      v19._m_pStr = (char *)(v2 - 9); /*0x76ab65*/
      m_hr = &v19; /*0x76ab68*/
      v18 = SP_870_PLEASE_CHECK_IF_YOUR_INVENTORY_IS_FULL_OR_NOT; /*0x76ab6c*/
      goto LABEL_39; /*0x76ab72*/
    case 11: /*0x76aa46*/
    case 16: /*0x76aa46*/
      v23 = 0; /*0x76ab8d*/
      v22 = 0; /*0x76ab8e*/
      v21 = nullptr; /*0x76ab8f*/
      v20 = nullptr; /*0x76ab90*/
      v19._m_pStr = (char *)(v2 - 9); /*0x76ab91*/
      m_hr = &v19; /*0x76ab94*/
      v18 = SP_6795_YOU_DO_NOT_HAVE_ENOUGH_MESOS; /*0x76ab98*/
      goto LABEL_39; /*0x76ab9d*/
    case 12: /*0x76aa46*/
      v23 = 0; /*0x76ab77*/
      v22 = 0; /*0x76ab78*/
      v21 = nullptr; /*0x76ab79*/
      v20 = nullptr; /*0x76ab7a*/
      v19._m_pStr = (char *)(v2 - 9); /*0x76ab7b*/
      m_hr = &v19; /*0x76ab7e*/
      v18 = SP_884_ITEM_COULD_NOT_BE_RETRIEVED_R_NBECAUSE_THERE_WAS_AN_ITEM_THAT_R_NCOULD_ONLY_BE_A; /*0x76ab82*/
      goto LABEL_39; /*0x76ab88*/
    case 13: /*0x76aa46*/
    case 15: /*0x76aa46*/
    case 19: /*0x76aa46*/
$LN10_76: /*0x76ab3c*/
      CTrunkDlg::SetGetItems(this: v5, iPacket: v1); /*0x76ab3c*/
      CWnd::InvalidateRect(this: v5, pRect: nullptr); /*0x76ab48*/
      return; /*0x76ab60*/
    case 17: /*0x76aa46*/
      v23 = 0; /*0x76aba2*/
      v22 = 0; /*0x76aba3*/
      v21 = nullptr; /*0x76aba4*/
      v20 = nullptr; /*0x76aba5*/
      v19._m_pStr = (char *)(v2 - 9); /*0x76aba6*/
      m_hr = &v19; /*0x76aba9*/
      v18 = SP_883_THE_STORAGE_IS_FULL; /*0x76abad*/
      goto LABEL_39; /*0x76abb3*/
    case 22: /*0x76aa46*/
      if ( v5 != nullptr ) /*0x76aa4f*/
      {
        CMSException::CMSException(this: &v27, hr: 553648131); /*0x76aa5a*/
        m_hr = (ZXString<char> *)v27.m_hr; /*0x76aa6d*/
        _CxxThrowException(pExceptionObject: &m_hr, pThrowInfo: &_TI3_AVCDisconnectException__); /*0x76aa71*/
      }
      if ( v3 == nullptr ) /*0x76aa78*/
      {
        v6 = (CTrunkDlg *)ZAllocEx<ZAllocAnonSelector>::Alloc( /*0x76aa88*/
                            this: &ZAllocEx<ZAllocAnonSelector>::_s_alloc,
                            uSize: 0xB54u);
        pExceptionObject = (ZXString<char> *)v6; /*0x76aa8d*/
        v29 = 0; /*0x76aa91*/
        if ( v6 != nullptr ) /*0x76aa97*/
          CTrunkDlg::CTrunkDlg(this: v6); /*0x76aa9b*/
        else
          v7 = nullptr; /*0x76aaa2*/
        v29 = -1; /*0x76aaa7*/
        CTrunkDlg::SetTrunkDlg(this: v7, iPacket); /*0x76aaaf*/
      }
      return; /*0x76aac7*/
    case 23: /*0x76aa46*/
      v23 = 0; /*0x76abb8*/
      v22 = 0; /*0x76abb9*/
      v21 = nullptr; /*0x76abba*/
      v20 = nullptr; /*0x76abbb*/
      v19._m_pStr = (char *)(v2 - 9); /*0x76abbc*/
      m_hr = &v19; /*0x76abbf*/
      v18 = SP_5869_ITEMS_OR_MESOS_CANNOT_BE_MOVED_R_NPLEASE_CONTACT_CUSTOMER_SUPPORT; /*0x76abc3*/
      goto LABEL_39; /*0x76abc9*/
    case 24: /*0x76aa46*/
      if ( CInPacket::Decode1(this: iPacket) != 0 ) /*0x76abda*/
      {
        CInPacket::DecodeStr(this: iPacket, result: &sMsg); /*0x76abe3*/
        v23 = 0; /*0x76abe8*/
        v22 = 1; /*0x76abe9*/
        v21 = nullptr; /*0x76abeb*/
        v20 = nullptr; /*0x76abec*/
        v19._m_pStr = v16; /*0x76abed*/
        pExceptionObject = &v19; /*0x76abf4*/
        v29 = 1; /*0x76abf9*/
        ZXString<char>::ZXString<char>(this: &v19, s: &sMsg); /*0x76ac01*/
        CUtilDlg::Notice(sMsg: v19, sSndName: v20, ppDialog: v21, bAutoSeparated: v22, bIsTightLine: v23); /*0x76ac06*/
        v29 = -1; /*0x76ac12*/
        if ( sMsg._m_pStr != nullptr ) /*0x76ac1c*/
          ZXString<char>::_Release(pData: (ZXString<char>::_ZXStringData *)sMsg._m_pStr - 1); /*0x76ac22*/
      }
      else
      {
        v23 = 0; /*0x76ac3e*/
        v22 = 0; /*0x76ac3f*/
        v21 = nullptr; /*0x76ac40*/
        v20 = nullptr; /*0x76ac41*/
        v19._m_pStr = v15; /*0x76ac42*/
        pExceptionObject = &v19; /*0x76ac45*/
        v18 = SP_873_DUE_TO_AN_ERROR_THE_TRADE_DID_NOT_HAPPEN; /*0x76ac49*/
LABEL_39:
        Instance = StringPool::GetInstance(); /*0x76ac62*/
        StringPool::GetString(this: Instance, result: &v19, nIdx: v18); /*0x76ac69*/
        CUtilDlg::Notice(sMsg: v19, sSndName: v20, ppDialog: v21, bAutoSeparated: v22, bIsTightLine: v23); /*0x76ac6e*/
      }
      break; /*0x76ac3d*/
    default:
      v23 = 0; /*0x76ac51*/
      v22 = 0; /*0x76ac52*/
      v21 = nullptr; /*0x76ac53*/
      v20 = nullptr; /*0x76ac54*/
      v19._m_pStr = (char *)(v2 - 9); /*0x76ac55*/
      m_hr = &v19; /*0x76ac58*/
      v18 = SP_873_DUE_TO_AN_ERROR_THE_TRADE_DID_NOT_HAPPEN; /*0x76ac5c*/
      goto LABEL_39; /*0x76ac5c*/
  }
}