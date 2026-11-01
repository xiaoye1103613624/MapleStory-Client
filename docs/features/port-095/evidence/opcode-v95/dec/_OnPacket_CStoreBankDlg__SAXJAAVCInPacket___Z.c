void __cdecl CStoreBankDlg::OnPacket(ZXString<char> *nType, CInPacket *iPacket)
{
  CInPacket *v2; // edi
  char *v3; // ecx
  CStoreBankDlg *v4; // eax
  CStoreBankDlg *v5; // eax
  int v6; // ebp
  unsigned int v7; // esi
  unsigned __int8 v8; // al
  const char *ChannelName; // eax
  StringPool *v10; // eax
  char *v11; // ecx
  char *v12; // eax
  StringPool *v13; // eax
  CUtilDlgEx *v14; // eax
  char *v15; // ecx
  int v16; // eax
  int v17; // eax
  CUtilDlgEx *v18; // esi
  StringPool *v19; // eax
  CStoreBankDlg *v20; // esi
  int v21; // ebx
  int v22; // eax
  CStoreBankDlg *v23; // esi
  CInPacket *v24; // ecx
  char v25; // al
  StringPool *Instance; // eax
  ZXString<char> *String; // eax
  char *m_pStr; // eax
  StringPool *v29; // eax
  ZXString<char> *v30; // eax
  StringPool *v31; // eax
  ZXString<char> *v32; // eax
  StringPool *v33; // eax
  ZXString<char> *v34; // eax
  StringPool *v35; // eax
  ZXString<char> *v36; // eax
  ZXString<char> v37; // [esp-14h] [ebp-7Ch] BYREF
  ZXString<char> v38; // [esp-10h] [ebp-78h] BYREF
  ZXString<char> v39; // [esp-Ch] [ebp-74h] BYREF
  ZXString<char> *p_sChannelName; // [esp-8h] [ebp-70h]
  StringPoolStrings v41; // [esp-4h] [ebp-6Ch]
  ZXString<char> sChannelName; // [esp+14h] [ebp-54h] BYREF
  ZXString<char> sWorld; // [esp+18h] [ebp-50h] BYREF
  ZRef<CUtilDlgEx> sAll; // [esp+1Ch] [ebp-4Ch] BYREF
  CMSException v45; // [esp+24h] [ebp-44h] BYREF
  HRESULT pExceptionObject; // [esp+28h] [ebp-40h] BYREF
  ZXString<char> result; // [esp+2Ch] [ebp-3Ch] BYREF
  ZXString<char> v48; // [esp+30h] [ebp-38h] BYREF
  CMSException v49; // [esp+34h] [ebp-34h] BYREF
  HRESULT v50; // [esp+38h] [ebp-30h] BYREF
  CMSException v51; // [esp+3Ch] [ebp-2Ch] BYREF
  HRESULT m_hr; // [esp+40h] [ebp-28h] BYREF
  ZXString<char> v53; // [esp+44h] [ebp-24h] BYREF
  ZXString<char> v54; // [esp+48h] [ebp-20h] BYREF
  ZXString<char> v55; // [esp+4Ch] [ebp-1Ch] BYREF
  ZXString<char> v56; // [esp+50h] [ebp-18h] BYREF
  ZXString<char> v57; // [esp+54h] [ebp-14h] BYREF
  ZXString<char> *v58; // [esp+58h] [ebp-10h]
  int v59; // [esp+64h] [ebp-4h]

  if ( nType == (ZXString<char> *)369 ) /*0x745c90*/
  {
    if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr /*0x74601c*/
      && TSingleton<CUniqueModeless>::ms_pInstance->CDialog::CWnd::IUIMsgHandler::vfptr->IsKindOf(
           this: &TSingleton<CUniqueModeless>::ms_pInstance->IUIMsgHandler,
           a2: &CStoreBankDlg::ms_RTTI_CStoreBankDlg) != 0 )
    {
      v23 = (CStoreBankDlg *)TSingleton<CUniqueModeless>::ms_pInstance; /*0x746022*/
      if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr ) /*0x74602a*/
      {
        if ( TSingleton<CUniqueModeless>::ms_pInstance[1].m_rcInvalidated.left == 0 ) /*0x746036*/
        {
          CMSException::CMSException(this: &v51, hr: 553648131); /*0x746041*/
          m_hr = v51.m_hr; /*0x746054*/
          _CxxThrowException(pExceptionObject: &m_hr, pThrowInfo: &_TI3_AVCDisconnectException__); /*0x746058*/
        }
        v24 = iPacket; /*0x74605d*/
        TSingleton<CUniqueModeless>::ms_pInstance[1].m_rcInvalidated.left = 0; /*0x746061*/
        v25 = CInPacket::Decode1(this: v24); /*0x746067*/
        nType = nullptr; /*0x74606f*/
        v59 = 9; /*0x74607b*/
        switch ( v25 ) /*0x746088*/
        {
          case 30: /*0x746088*/
            CStoreBankDlg::RemoveAll(this: v23); /*0x746091*/
            CWnd::InvalidateRect(this: v23, pRect: nullptr); /*0x746099*/
            v23->m_pBtGet.p->vfptr->SetEnable(this: &v23->m_pBtGet.p->IUIMsgHandler, a2: 0); /*0x7460ae*/
            Instance = StringPool::GetInstance(); /*0x7460ba*/
            String = StringPool::GetString( /*0x7460c1*/
                       this: Instance,
                       result: &v53,
                       nIdx: SP_3526_YOU_HAVE_RETRIEVED_YOUR_ITEMS_AND_MESOS);
            LOBYTE(v59) = 10; /*0x7460cb*/
            ZXString<char>::operator=(this: (ZXString<char> *)&nType, s: String); /*0x7460d0*/
            m_pStr = v53._m_pStr; /*0x7460d5*/
            goto LABEL_47; /*0x7460d9*/
          case 31: /*0x746088*/
            v29 = StringPool::GetInstance(); /*0x7460e8*/
            v30 = StringPool::GetString( /*0x7460ef*/
                    this: v29,
                    result: &v54,
                    nIdx: SP_3527_UNABLE_TO_RETRIEVE_MESOS_AND_ITEMS_DUE_TO_R_NTOO_MUCH_MONEY_STORED_R_NAT_THE_STO);
            LOBYTE(v59) = 11; /*0x7460f9*/
            ZXString<char>::operator=(this: (ZXString<char> *)&nType, s: v30); /*0x7460fe*/
            m_pStr = v54._m_pStr; /*0x746103*/
            goto LABEL_47; /*0x746107*/
          case 32: /*0x746088*/
            v31 = StringPool::GetInstance(); /*0x746113*/
            v32 = StringPool::GetString( /*0x74611a*/
                    this: v31,
                    result: &v55,
                    nIdx: SP_3528_UNABLE_TO_RETRIEVE_MESOS_AND_ITEMS_DUE_TO_R_NONE_OF_THE_ITEMS_R_NTHAT_CAN_ONLY_B);
            LOBYTE(v59) = 12; /*0x746124*/
            ZXString<char>::operator=(this: (ZXString<char> *)&nType, s: v32); /*0x746129*/
            m_pStr = v55._m_pStr; /*0x74612e*/
            goto LABEL_47; /*0x746132*/
          case 33: /*0x746088*/
            v33 = StringPool::GetInstance(); /*0x74613e*/
            v34 = StringPool::GetString( /*0x746145*/
                    this: v33,
                    result: &v56,
                    nIdx: SP_3529_DUE_TO_THE_LACK_OF_SERVICE_FEE_YOU_WERE_UNABLE_TO__R_NRETRIEVE_MESOS_OR_ITEMS);
            LOBYTE(v59) = 13; /*0x74614f*/
            ZXString<char>::operator=(this: (ZXString<char> *)&nType, s: v34); /*0x746154*/
            m_pStr = v56._m_pStr; /*0x746159*/
            goto LABEL_47; /*0x74615d*/
          case 34: /*0x746088*/
            v35 = StringPool::GetInstance(); /*0x746169*/
            v36 = StringPool::GetString( /*0x746170*/
                    this: v35,
                    result: &v57,
                    nIdx: SP_3530_UNABLE_TO_RETRIEVE_MESOS_AND_ITEMS_R_NDUE_TO_FULL_INVENTORY);
            LOBYTE(v59) = 14; /*0x74617a*/
            ZXString<char>::operator=(this: (ZXString<char> *)&nType, s: v36); /*0x74617f*/
            m_pStr = v57._m_pStr; /*0x746184*/
LABEL_47:
            LOBYTE(v59) = 9; /*0x746188*/
            if ( m_pStr != nullptr ) /*0x74618e*/
              ZXString<char>::_Release(pData: (ZXString<char>::_ZXStringData *)m_pStr - 1); /*0x746194*/
            break; /*0x746194*/
          default:
            break;
        }
        v41 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x74619c*/
        p_sChannelName = nullptr; /*0x74619d*/
        v39._m_pStr = nullptr; /*0x74619e*/
        v38._m_pStr = nullptr; /*0x74619f*/
        v58 = &v37; /*0x7461aa*/
        v37._m_pStr = nullptr; /*0x7461af*/
        ZXString<char>::operator=(this: &v37, s: (ZXString<char> *)&nType); /*0x7461b1*/
        CUtilDlg::Notice( /*0x7461b6*/
          sMsg: v37,
          sSndName: (const wchar_t *)v38._m_pStr,
          ppDialog: (ZRef<CDialog> *)v39._m_pStr,
          bAutoSeparated: (int)p_sChannelName,
          bIsTightLine: v41);
LABEL_50:
        v59 = -1; /*0x7461be*/
        if ( nType != nullptr ) /*0x7461cc*/
          ZXString<char>::_Release(pData: (ZXString<char>::_ZXStringData *)&nType[-3]); /*0x7461d2*/
      }
    }
  }
  else if ( nType == (ZXString<char> *)370 ) /*0x745c99*/
  {
    v2 = iPacket; /*0x745c9f*/
    switch ( CInPacket::Decode1(this: iPacket) ) /*0x745cb9*/
    {
      case '#': /*0x745cb9*/
        if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr ) /*0x745cc7*/
        {
          CMSException::CMSException(this: &v45, hr: 553648131); /*0x745cd2*/
          pExceptionObject = v45.m_hr; /*0x745ce5*/
          _CxxThrowException(&pExceptionObject, pThrowInfo: &_TI3_AVCDisconnectException__); /*0x745ce9*/
        }
        v4 = (CStoreBankDlg *)ZAllocEx<ZAllocAnonSelector>::Alloc( /*0x745cf8*/
                                this: &ZAllocEx<ZAllocAnonSelector>::_s_alloc,
                                uSize: 0xE0u);
        nType = (ZXString<char> *)v4; /*0x745cfd*/
        v59 = 0; /*0x745d01*/
        if ( v4 != nullptr ) /*0x745d0b*/
          CStoreBankDlg::CStoreBankDlg(this: v4); /*0x745d0f*/
        else
          v5 = nullptr; /*0x745d16*/
        v59 = -1; /*0x745d1b*/
        CStoreBankDlg::SetStoreBankDlg(this: v5, iPacket: v2); /*0x745d23*/
        break; /*0x745d3b*/
      case '$': /*0x745cb9*/
        if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr /*0x745f8d*/
          && TSingleton<CUniqueModeless>::ms_pInstance->CDialog::CWnd::IUIMsgHandler::vfptr->IsKindOf(
               this: &TSingleton<CUniqueModeless>::ms_pInstance->IUIMsgHandler,
               a2: &CStoreBankDlg::ms_RTTI_CStoreBankDlg) != 0 )
        {
          v20 = (CStoreBankDlg *)TSingleton<CUniqueModeless>::ms_pInstance; /*0x745f93*/
          if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr ) /*0x745f9b*/
          {
            if ( TSingleton<CUniqueModeless>::ms_pInstance[1].m_rcInvalidated.left == 0 ) /*0x745fa8*/
            {
              CMSException::CMSException(this: &v49, hr: 553648131); /*0x745fb3*/
              v50 = v49.m_hr; /*0x745fc6*/
              _CxxThrowException(pExceptionObject: &v50, pThrowInfo: &_TI3_AVCDisconnectException__); /*0x745fca*/
            }
            v21 = CInPacket::Decode4(this: v2); /*0x745fd8*/
            v22 = CInPacket::Decode4(this: v2); /*0x745fda*/
            CStoreBankDlg::SendGetAllRequest(this: v20, nPassingDay: v21, nFee: v22); /*0x745fe3*/
          }
        }
        break; /*0x745ffb*/
      case '%': /*0x745cb9*/
        v6 = CInPacket::Decode4(this: v2); /*0x745d45*/
        v7 = CInPacket::Decode4(this: v2); /*0x745d4e*/
        v8 = CInPacket::Decode1(this: v2); /*0x745d50*/
        nType = nullptr; /*0x745d57*/
        v59 = 1; /*0x745d5b*/
        if ( v7 == 999999999 || v8 >= 0xFEu ) /*0x745d71*/
        {
          v13 = StringPool::GetInstance(); /*0x745e5e*/
          v41 = (StringPoolStrings)StringPool::GetString( /*0x745e6c*/
                                     this: v13,
                                     result: &v48,
                                     nIdx: SP_3521_I_DONT_THINK_YOU_HAVE_ANY_ITEMS_OR_MONEY_TO_RETRIEVE_HERE_R_NTHIS_IS_WHERE_YOU_R)->_m_pStr;
          LOBYTE(v59) = 2; /*0x745e72*/
          ZXString<char>::Format(this: (ZXString<char> *)&nType, sFormat: (const char *)v41); /*0x745e77*/
          v12 = v48._m_pStr; /*0x745e7c*/
        }
        else
        {
          ChannelName = CWvsContext::GetChannelName( /*0x745d83*/
                          this: (CWvsContext *)TSingleton<CWvsContext>::ms_pInstance._m_pStr,
                          nIdx: v8);
          ZXString<char>::ZXString<char>(this: (ZXString<char> *)&sAll, t: ChannelName, n: -1); /*0x745d8d*/
          sChannelName._m_pStr = nullptr; /*0x745d92*/
          sWorld._m_pStr = nullptr; /*0x745d96*/
          v41 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x745d9a*/
          p_sChannelName = &sChannelName; /*0x745d9f*/
          v39._m_pStr = (char *)&sWorld; /*0x745da4*/
          v38._m_pStr = (char *)&sWorld; /*0x745da5*/
          v58 = &v38; /*0x745dac*/
          LOBYTE(v59) = 5; /*0x745db3*/
          ZXString<char>::ZXString<char>(this: &v38, s: (ZXString<char> *)&sAll); /*0x745db7*/
          separate_channelName( /*0x745dbc*/
            sAll: v38,
            sWorld: (ZXString<char> *)v39._m_pStr,
            sChannel: p_sChannelName,
            bAttachCH: v41);
          v41 = SP_3522_YOUR_PERSONAL_STORE_IS_OPEN_BIN_CHANNEL_S_FREE_MARKET_DK_R_NIF_YOU_NEED_ME_THEN_; /*0x745dc8*/
          p_sChannelName = &result; /*0x745dcd*/
          v10 = StringPool::GetInstance(); /*0x745dce*/
          v11 = StringPool::GetString(this: v10, result: p_sChannelName, nIdx: v41)->_m_pStr; /*0x745dda*/
          LOBYTE(v59) = 6; /*0x745df7*/
          ZXString<char>::Format(this: (ZXString<char> *)&nType, sFormat: v11, sChannelName._m_pStr, v7 % 0x64); /*0x745dfc*/
          LOBYTE(v59) = 5; /*0x745e08*/
          if ( result._m_pStr != nullptr ) /*0x745e0e*/
            ZXString<char>::_Release(pData: (ZXString<char>::_ZXStringData *)result._m_pStr - 1); /*0x745e14*/
          LOBYTE(v59) = 4; /*0x745e20*/
          if ( sWorld._m_pStr != nullptr ) /*0x745e27*/
            ZXString<char>::_Release(pData: (ZXString<char>::_ZXStringData *)sWorld._m_pStr - 1); /*0x745e2d*/
          LOBYTE(v59) = 3; /*0x745e39*/
          if ( sChannelName._m_pStr != nullptr ) /*0x745e40*/
            ZXString<char>::_Release(pData: (ZXString<char>::_ZXStringData *)sChannelName._m_pStr - 1); /*0x745e46*/
          v12 = *(char **)sAll.gap0; /*0x745e4e*/
        }
        LOBYTE(v59) = 1; /*0x745e83*/
        if ( v12 != nullptr ) /*0x745e8a*/
          ZXString<char>::_Release(pData: (ZXString<char>::_ZXStringData *)v12 - 1); /*0x745e90*/
        v14 = (CUtilDlgEx *)ZAllocEx<ZAllocAnonSelector>::Alloc( /*0x745ea2*/
                              this: &ZAllocEx<ZAllocAnonSelector>::_s_alloc,
                              uSize: 0xC50u);
        v58 = (ZXString<char> *)v14; /*0x745ea7*/
        LOBYTE(v59) = 7; /*0x745eab*/
        if ( v14 != nullptr && (CUtilDlgEx::CUtilDlgEx(this: v14), v16 != 0) && (v17 = v16 + 8) != 0 ) /*0x745ec4*/
        {
          v18 = (CUtilDlgEx *)(v17 - 8); /*0x745ec6*/
          sAll.p = (CUtilDlgEx *)(v17 - 8); /*0x745ec9*/
          if ( v17 != 8 ) /*0x745ecf*/
            InterlockedIncrement(lpAddend: &v18->_m_nRef); /*0x745ed5*/
        }
        else
        {
          v18 = nullptr; /*0x745f26*/
          sAll.p = nullptr; /*0x745f28*/
        }
        v41 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x745edb*/
        p_sChannelName = nullptr; /*0x745edc*/
        v39._m_pStr = v15; /*0x745edd*/
        v58 = &v39; /*0x745ee4*/
        LOBYTE(v59) = 8; /*0x745ee9*/
        ZXString<char>::ZXString<char>(this: &v39, s: (ZXString<char> *)&nType); /*0x745eee*/
        CUtilDlgEx::SetUtilDlgEx( /*0x745ef7*/
          this: v18,
          dlgType: 0,
          nTemplateID: v6,
          sText: v39,
          bNoNpc: (int)p_sChannelName,
          bQuest: v41);
        CUtilDlgEx::SetUtilDlgEx_TEXT(this: v18, bPrev: 0, bNext: 0); /*0x745f00*/
        CUtilDlgEx::CreateUtilDlgEx(this: v18); /*0x745f07*/
        CDialog::DoModal(this: v18); /*0x745f0e*/
        LOBYTE(v59) = 1; /*0x745f17*/
        ZRef<CUtilDlgEx>::~ZRef<CUtilDlgEx>(this: &sAll); /*0x745f1c*/
        goto LABEL_50; /*0x745f21*/
      case '&': /*0x745cb9*/
        v41 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x745f2e*/
        p_sChannelName = nullptr; /*0x745f30*/
        v39._m_pStr = nullptr; /*0x745f32*/
        v38._m_pStr = nullptr; /*0x745f34*/
        v37._m_pStr = v3; /*0x745f36*/
        nType = &v37; /*0x745f39*/
        v19 = StringPool::GetInstance(); /*0x745f46*/
        StringPool::GetString( /*0x745f4d*/
          this: v19,
          result: &v37,
          nIdx: SP_3523_THIS_IS_CURRENTLY_UNAVAILABLE_R_NPLEASE_TRY_AGAIN_LATER);
        CUtilDlg::Notice( /*0x745f52*/
          sMsg: v37,
          sSndName: (const wchar_t *)v38._m_pStr,
          ppDialog: (ZRef<CDialog> *)v39._m_pStr,
          bAutoSeparated: (int)p_sChannelName,
          bIsTightLine: v41);
        break; /*0x745f6d*/
      default:
        return;
    }
  }
}