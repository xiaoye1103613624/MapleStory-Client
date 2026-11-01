void __cdecl CParcelDlg::OnPacket(CInPacket *iPacket)
{
  int v1; // edi
  char *v2; // eax
  BOOL v3; // edi
  char *v4; // eax
  CParcelDlg *v5; // eax
  int v6; // edi
  int v7; // esi
  CParcelDlg *v8; // ebp
  StringPool *Instance; // eax
  ZXString<char> *String; // eax
  char *v11; // ecx
  int v12; // ebx
  StringPool *v13; // eax
  CParcelDlg *v14; // edi
  char *v15; // ecx
  StringPool *v16; // eax
  BOOL v17; // edi
  CUIFadeYesNo *v18; // eax
  CUIFadeYesNo *v19; // eax
  CUIFadeYesNo *v20; // esi
  BOOL v21; // esi
  char *v22; // eax
  CUIFadeYesNo *v23; // eax
  CUIFadeYesNo *v24; // edi
  CParcelDlg *v25; // esi
  int m_nMode; // eax
  ZXString<char> v27; // [esp-14h] [ebp-6Ch] BYREF
  const wchar_t *v28; // [esp-10h] [ebp-68h]
  ZRef<CDialog> *v29; // [esp-Ch] [ebp-64h]
  ZRef<PARCEL> v30[3]; // [esp-8h] [ebp-60h] BYREF
  ZXString<char> *v31; // [esp+14h] [ebp-44h]
  ZXString<char> sSender; // [esp+18h] [ebp-40h] BYREF
  CMSException v33; // [esp+1Ch] [ebp-3Ch] BYREF
  HRESULT pExceptionObject; // [esp+20h] [ebp-38h] BYREF
  CMSException v35; // [esp+24h] [ebp-34h] BYREF
  HRESULT m_hr; // [esp+28h] [ebp-30h] BYREF
  CMSException v37; // [esp+2Ch] [ebp-2Ch] BYREF
  HRESULT v38; // [esp+30h] [ebp-28h] BYREF
  ZXString<char> v39; // [esp+34h] [ebp-24h] BYREF
  ZXString<char> result; // [esp+38h] [ebp-20h] BYREF
  CMSException v41; // [esp+3Ch] [ebp-1Ch] BYREF
  HRESULT v42; // [esp+40h] [ebp-18h] BYREF
  ZRef<PARCEL> p; // [esp+44h] [ebp-14h] BYREF
  int v44; // [esp+54h] [ebp-4h]

  v31 = nullptr; /*0x69299f*/
  v1 = (unsigned __int8)CInPacket::Decode1(this: iPacket); /*0x6929a8*/
  switch ( v1 )
  {
    case 8:
      if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr ) /*0x692a34*/
      {
        CMSException::CMSException(this: &v35, hr: 553648131); /*0x692a3f*/
        m_hr = v35.m_hr; /*0x692a52*/
        _CxxThrowException(pExceptionObject: &m_hr, pThrowInfo: &_TI3_AVCDisconnectException__); /*0x692a56*/
      }
      v3 = CInPacket::Decode1(this: iPacket) != 0; /*0x692a73*/
      v4 = (char *)ZAllocEx<ZAllocAnonSelector>::Alloc(this: &ZAllocEx<ZAllocAnonSelector>::_s_alloc, uSize: 0x158u); /*0x692a75*/
      sSender._m_pStr = v4; /*0x692a7a*/
      v44 = 1; /*0x692a7e*/
      if ( v4 != nullptr )
        CParcelDlg::CParcelDlg(this: (CParcelDlg *)v4, nMode: v3 ? 2 : 0);
      else
        v5 = nullptr; /*0x692a9b*/
      v44 = -1; /*0x692aa0*/
      CParcelDlg::SetParcelDlg(this: v5, iPacket); /*0x692aa8*/
      break; /*0x692ac0*/
    case 23:
      v6 = CInPacket::Decode4(this: iPacket); /*0x692aca*/
      v7 = (unsigned __int8)CInPacket::Decode1(this: iPacket); /*0x692ad1*/
      if ( TSingleton<CUniqueModeless>::ms_pInstance == nullptr /*0x692af8*/
        || TSingleton<CUniqueModeless>::ms_pInstance->CDialog::CWnd::IUIMsgHandler::vfptr->IsKindOf(
             this: &TSingleton<CUniqueModeless>::ms_pInstance->IUIMsgHandler,
             a2: &CParcelDlg::ms_RTTI_CParcelDlg) == 0
        || (v8 = (CParcelDlg *)TSingleton<CUniqueModeless>::ms_pInstance,
            TSingleton<CUniqueModeless>::ms_pInstance == nullptr) )
      {
        CMSException::CMSException(this: &v37, hr: 553648131); /*0x692b03*/
        v38 = v37.m_hr; /*0x692b16*/
        _CxxThrowException(pExceptionObject: &v38, pThrowInfo: &_TI3_AVCDisconnectException__); /*0x692b1a*/
      }
      CParcelDlg::RemoveParcel(this: (CParcelDlg *)TSingleton<CUniqueModeless>::ms_pInstance, nParcelSN: v6); /*0x692b22*/
      if ( v7 == 3 ) /*0x692b2a*/
      {
        Instance = StringPool::GetInstance(); /*0x692b36*/
        String = StringPool::GetString(this: Instance, &result, nIdx: SP_3940_SUCCESSFULLY_DELETED); /*0x692b3d*/
        v44 = 2; /*0x692b42*/
        v12 = 1; /*0x692b4a*/
      }
      else
      {
        v13 = StringPool::GetInstance(); /*0x692b59*/
        String = StringPool::GetString(this: v13, result: &v39, nIdx: SP_3941_SUCCESSFULLY_CLAIMED); /*0x692b60*/
        v44 = 3; /*0x692b65*/
        v12 = 2; /*0x692b6d*/
      }
      v30[0].p = nullptr; /*0x692b72*/
      *(_DWORD *)v30[0].gap0 = 0; /*0x692b74*/
      v29 = nullptr; /*0x692b76*/
      v28 = nullptr; /*0x692b78*/
      v27._m_pStr = v11; /*0x692b7a*/
      sSender._m_pStr = (char *)&v27; /*0x692b7d*/
      v31 = (ZXString<char> *)v12; /*0x692b82*/
      ZXString<char>::ZXString<char>(this: &v27, s: String); /*0x692b86*/
      CUtilDlg::Notice( /*0x692b8b*/
        sMsg: v27,
        sSndName: v28,
        ppDialog: v29,
        bAutoSeparated: *(int *)v30[0].gap0,
        bIsTightLine: (int)v30[0].p);
      v44 = 2; /*0x692b93*/
      if ( (v12 & 2) != 0 ) /*0x692b9e*/
      {
        v12 &= ~2u; /*0x692ba4*/
        v31 = (ZXString<char> *)v12; /*0x692ba7*/
        if ( v39._m_pStr != nullptr ) /*0x692bad*/
          ZXString<char>::_Release(pData: (ZXString<char>::_ZXStringData *)v39._m_pStr - 1); /*0x692bb3*/
      }
      v44 = -1; /*0x692bbb*/
      if ( (v12 & 1) != 0 && result._m_pStr != nullptr ) /*0x692bce*/
        ZXString<char>::_Release(pData: (ZXString<char>::_ZXStringData *)result._m_pStr - 1); /*0x692bd4*/
      CParcelDlg::SetCtrlEnabled(this: v8, bEnable: 1); /*0x692be0*/
      break; /*0x692bf8*/
    case 24:
      if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr /*0x692c18*/
        && TSingleton<CUniqueModeless>::ms_pInstance->CDialog::CWnd::IUIMsgHandler::vfptr->IsKindOf(
             this: &TSingleton<CUniqueModeless>::ms_pInstance->IUIMsgHandler,
             a2: &CParcelDlg::ms_RTTI_CParcelDlg) != 0 )
      {
        v14 = (CParcelDlg *)TSingleton<CUniqueModeless>::ms_pInstance; /*0x692c1e*/
        if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr ) /*0x692c26*/
        {
          p.p = nullptr; /*0x692c30*/
          ZRef<PARCEL>::_Alloc(this: &p); /*0x692c34*/
          v44 = 4; /*0x692c3e*/
          PARCEL::Decode(this: p.p, iPacket); /*0x692c46*/
          v31 = (ZXString<char> *)v30; /*0x692c54*/
          ZRef<PARCEL>::ZRef<PARCEL>(this: v30, r: &p); /*0x692c59*/
          CParcelDlg::AddNewParcel(this: v14, p: v30[0]); /*0x692c60*/
          v30[0].p = nullptr; /*0x692c65*/
          *(_DWORD *)v30[0].gap0 = 0; /*0x692c66*/
          v29 = nullptr; /*0x692c67*/
          v28 = nullptr; /*0x692c68*/
          v27._m_pStr = v15; /*0x692c69*/
          v31 = &v27; /*0x692c6c*/
          v16 = StringPool::GetInstance(); /*0x692c76*/
          StringPool::GetString(this: v16, result: &v27, nIdx: SP_3943_A_NEW_PACKAGE_HAS_BEEN_SENT); /*0x692c7d*/
          CUtilDlg::Notice( /*0x692c82*/
            sMsg: v27,
            sSndName: v28,
            ppDialog: v29,
            bAutoSeparated: *(int *)v30[0].gap0,
            bIsTightLine: (int)v30[0].p);
          v44 = -1; /*0x692c8e*/
          ZRef<PARCEL>::~ZRef<PARCEL>(this: &p); /*0x692c96*/
        }
      }
      break; /*0x692cae*/
    case 25:
      CInPacket::DecodeStr(this: iPacket, result: &sSender); /*0x692cb6*/
      v44 = 5; /*0x692cc2*/
      v17 = CInPacket::Decode1(this: iPacket) != 0; /*0x692cd7*/
      if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr /*0x692cf8*/
        && TSingleton<CUniqueModeless>::ms_pInstance->CDialog::CWnd::IUIMsgHandler::vfptr->IsKindOf(
             this: &TSingleton<CUniqueModeless>::ms_pInstance->IUIMsgHandler,
             a2: &CParcelDlg::ms_RTTI_CParcelDlg) != 0
        && TSingleton<CUniqueModeless>::ms_pInstance != nullptr )
      {
        CWnd::InvalidateRect(this: TSingleton<CUniqueModeless>::ms_pInstance, pRect: nullptr); /*0x692cfb*/
      }
      else
      {
        v18 = (CUIFadeYesNo *)ZAllocEx<ZAllocAnonSelector>::Alloc( /*0x692d0c*/
                                this: &ZAllocEx<ZAllocAnonSelector>::_s_alloc,
                                uSize: 0x144u);
        v31 = (ZXString<char> *)v18; /*0x692d11*/
        LOBYTE(v44) = 6; /*0x692d15*/
        if ( v18 != nullptr ) /*0x692d1c*/
        {
          CUIFadeYesNo::CUIFadeYesNo(this: v18); /*0x692d20*/
          v20 = v19; /*0x692d25*/
        }
        else
        {
          v20 = nullptr; /*0x692d29*/
        }
        v30[0].p = (PARCEL *)v17; /*0x692d2b*/
        v31 = (ZXString<char> *)v30; /*0x692d33*/
        LOBYTE(v44) = 5; /*0x692d38*/
        *(_DWORD *)v30[0].gap0 = 0; /*0x692d3c*/
        ZXString<char>::operator=(this: (ZXString<char> *)v30, s: &sSender); /*0x692d3e*/
        CUIFadeYesNo::CreateParcelAlarm( /*0x692d45*/
          this: v20,
          sSender: *(ZXString<char> *)v30[0].gap0,
          bQuickDelivery: (int)v30[0].p);
        CWvsContext::SetNewFadeWnd(this: (CWvsContext *)TSingleton<CWvsContext>::ms_pInstance._m_pStr, pWnd: v20); /*0x692d51*/
      }
      v44 = -1; /*0x692d5a*/
      if ( sSender._m_pStr != nullptr ) /*0x692d64*/
        ZXString<char>::_Release(pData: (ZXString<char>::_ZXStringData *)sSender._m_pStr - 1); /*0x692d6e*/
      break; /*0x692d89*/
    case 26:
      if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr ) /*0x6929cb*/
      {
        CMSException::CMSException(this: &v33, hr: 553648131); /*0x6929d6*/
        pExceptionObject = v33.m_hr; /*0x6929e9*/
        _CxxThrowException(&pExceptionObject, pThrowInfo: &_TI3_AVCDisconnectException__); /*0x6929ed*/
      }
      v2 = (char *)ZAllocEx<ZAllocAnonSelector>::Alloc(this: &ZAllocEx<ZAllocAnonSelector>::_s_alloc, uSize: 0x158u); /*0x6929fc*/
      sSender._m_pStr = v2; /*0x692a01*/
      v44 = 0; /*0x692a05*/
      if ( v2 != nullptr ) /*0x692a0b*/
        CParcelDlg::CParcelDlg(this: (CParcelDlg *)v2, nMode: 1); /*0x692a15*/
      break; /*0x692a2d*/
    case 27:
      if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr /*0x692daf*/
        && TSingleton<CUniqueModeless>::ms_pInstance->CDialog::CWnd::IUIMsgHandler::vfptr->IsKindOf(
             this: &TSingleton<CUniqueModeless>::ms_pInstance->IUIMsgHandler,
             a2: &CParcelDlg::ms_RTTI_CParcelDlg) != 0
        && TSingleton<CUniqueModeless>::ms_pInstance != nullptr )
      {
        CWnd::InvalidateRect(this: TSingleton<CUniqueModeless>::ms_pInstance, pRect: nullptr); /*0x692db2*/
      }
      else
      {
        v21 = CInPacket::Decode1(this: iPacket) != 0; /*0x692dde*/
        v22 = (char *)ZAllocEx<ZAllocAnonSelector>::Alloc(this: &ZAllocEx<ZAllocAnonSelector>::_s_alloc, uSize: 0x144u); /*0x692de5*/
        sSender._m_pStr = v22; /*0x692dea*/
        v44 = 7; /*0x692dee*/
        if ( v22 != nullptr ) /*0x692df8*/
        {
          CUIFadeYesNo::CUIFadeYesNo(this: (CUIFadeYesNo *)v22); /*0x692dfc*/
          v24 = v23; /*0x692e01*/
        }
        else
        {
          v24 = nullptr; /*0x692e05*/
        }
        v30[0].p = (PARCEL *)v21; /*0x692e07*/
        v31 = (ZXString<char> *)v30; /*0x692e0b*/
        v44 = -1; /*0x692e19*/
        *(_DWORD *)v30[0].gap0 = 0; /*0x692e21*/
        ZXString<char>::GetBuffer(this: (ZXString<char> *)v30, nMinLength: 0, bRetain: 0); /*0x692e23*/
        ZXString<char>::ReleaseBuffer(this: (ZXString<char> *)v30, nLength: 0); /*0x692e34*/
        CUIFadeYesNo::CreateParcelAlarm( /*0x692e3b*/
          this: v24,
          sSender: *(ZXString<char> *)v30[0].gap0,
          bQuickDelivery: (int)v30[0].p);
        CWvsContext::SetNewFadeWnd(this: (CWvsContext *)TSingleton<CWvsContext>::ms_pInstance._m_pStr, pWnd: v24); /*0x692e47*/
      }
      break; /*0x692dca*/
    default:
      if ( TSingleton<CUniqueModeless>::ms_pInstance == nullptr /*0x692e85*/
        || TSingleton<CUniqueModeless>::ms_pInstance->CDialog::CWnd::IUIMsgHandler::vfptr->IsKindOf(
             this: &TSingleton<CUniqueModeless>::ms_pInstance->IUIMsgHandler,
             a2: &CParcelDlg::ms_RTTI_CParcelDlg) == 0
        || (v25 = (CParcelDlg *)TSingleton<CUniqueModeless>::ms_pInstance,
            TSingleton<CUniqueModeless>::ms_pInstance == nullptr) )
      {
        CMSException::CMSException(this: &v41, hr: 553648131); /*0x692e90*/
        v42 = v41.m_hr; /*0x692ea3*/
        _CxxThrowException(pExceptionObject: &v42, pThrowInfo: &_TI3_AVCDisconnectException__); /*0x692ea7*/
      }
      CParcelDlg::NoticeResult(nResult: v1); /*0x692ead*/
      CParcelDlg::SetCtrlEnabled(this: v25, bEnable: 1); /*0x692eb9*/
      if ( v1 == 18 ) /*0x692ec1*/
      {
        m_nMode = v25->m_nMode; /*0x692ec3*/
        if ( m_nMode != 0 ) /*0x692ecb*/
        {
          if ( m_nMode == 1 ) /*0x692ed0*/
            CParcelDlg::CloseParcelDlg(this: v25); /*0x692ed4*/
        }
        else
        {
          CParcelDlg::ResetSendInfo(this: v25); /*0x692eef*/
        }
      }
      break; /*0x692eec*/
  }
}