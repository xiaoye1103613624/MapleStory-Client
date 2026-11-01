void __cdecl CRPSGameDlg::OnPacket(CInPacket *iPacket)
{
  int v1; // edi
  unsigned int v2; // edi
  StringPool *Instance; // eax
  CUtilDlgEx *v4; // eax
  int v5; // eax
  int v6; // eax
  CUtilDlgEx *v7; // esi
  CRPSGameDlg *v8; // eax
  ZXString<char> v9; // [esp-Ch] [ebp-54h] BYREF
  int v10; // [esp-8h] [ebp-50h]
  int v11; // [esp-4h] [ebp-4Ch]
  ZXString<char> asMsg; // [esp+Ch] [ebp-3Ch] BYREF
  CMSException v13; // [esp+10h] [ebp-38h] BYREF
  HRESULT pExceptionObject; // [esp+14h] [ebp-34h] BYREF
  CMSException v15; // [esp+18h] [ebp-30h] BYREF
  HRESULT v16; // [esp+1Ch] [ebp-2Ch] BYREF
  CMSException v17; // [esp+20h] [ebp-28h] BYREF
  HRESULT m_hr; // [esp+24h] [ebp-24h] BYREF
  CMSException v19; // [esp+28h] [ebp-20h] BYREF
  HRESULT v20; // [esp+2Ch] [ebp-1Ch] BYREF
  ZXString<char> *v21; // [esp+30h] [ebp-18h]
  ZRef<CUtilDlgEx> pQDlg; // [esp+34h] [ebp-14h] BYREF
  int v23; // [esp+44h] [ebp-4h]

  v1 = (unsigned __int8)CInPacket::Decode1(this: iPacket); /*0x6d9e30*/
  switch ( v1 ) /*0x6d9e46*/
  {
    case 6: /*0x6d9e46*/
    case 7: /*0x6d9e46*/
    case 9: /*0x6d9e46*/
    case 10: /*0x6d9e46*/
    case 11: /*0x6d9e46*/
    case 12: /*0x6d9e46*/
    case 14: /*0x6d9e46*/
      if ( TSingleton<CUniqueModeless>::ms_pInstance == nullptr ) /*0x6da027*/
      {
        CMSException::CMSException(this: &v17, hr: 553648131); /*0x6da032*/
        m_hr = v17.m_hr; /*0x6da045*/
        _CxxThrowException(pExceptionObject: &m_hr, pThrowInfo: &_TI3_AVCDisconnectException__); /*0x6da049*/
      }
      if ( TSingleton<CUniqueModeless>::ms_pInstance->CDialog::CWnd::IUIMsgHandler::vfptr->IsKindOf( /*0x6da069*/
             this: &TSingleton<CUniqueModeless>::ms_pInstance->IUIMsgHandler,
             a2: &CRPSGameDlg::ms_RTTI_CRPSGameDlg) == 0
        || TSingleton<CUniqueModeless>::ms_pInstance == nullptr )
      {
        CMSException::CMSException(this: &v19, hr: 553648131); /*0x6da074*/
        v20 = v19.m_hr; /*0x6da087*/
        _CxxThrowException(pExceptionObject: &v20, pThrowInfo: &_TI3_AVCDisconnectException__); /*0x6da08b*/
      }
      CRPSGameDlg::ProcessPacket(this: (CRPSGameDlg *)TSingleton<CUniqueModeless>::ms_pInstance, nType: v1, iPacket); /*0x6da092*/
      break; /*0x6da092*/
    case 8: /*0x6d9e46*/
      if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr ) /*0x6d9e54*/
      {
        CMSException::CMSException(this: &v13, hr: 553648131); /*0x6d9e5f*/
        pExceptionObject = v13.m_hr; /*0x6d9e72*/
        _CxxThrowException(&pExceptionObject, pThrowInfo: &_TI3_AVCDisconnectException__); /*0x6d9e76*/
      }
      v2 = CInPacket::Decode4(this: iPacket); /*0x6d9e82*/
      Instance = StringPool::GetInstance(); /*0x6d9e8e*/
      StringPool::GetString( /*0x6d9e95*/
        this: Instance,
        result: &asMsg,
        nIdx: SP_3715_EBROCK_PAPER_SCISSORS_NOTICENK_R_NTHE_PARTICIPATION_FEE_FOR_THIS_ROCK_PAPER_SCIS);
      v23 = 0; /*0x6d9ea4*/
      v4 = (CUtilDlgEx *)ZAllocEx<ZAllocAnonSelector>::Alloc( /*0x6d9eac*/
                           this: &ZAllocEx<ZAllocAnonSelector>::_s_alloc,
                           uSize: 0xC50u);
      v21 = (ZXString<char> *)v4; /*0x6d9eb1*/
      LOBYTE(v23) = 1; /*0x6d9eb5*/
      if ( v4 != nullptr && (CUtilDlgEx::CUtilDlgEx(this: v4), v5 != 0) && (v6 = v5 + 8) != 0 ) /*0x6d9ed4*/
      {
        v7 = (CUtilDlgEx *)(v6 - 8); /*0x6d9eda*/
        pQDlg.p = (CUtilDlgEx *)(v6 - 8); /*0x6d9edd*/
        if ( v6 != 8 ) /*0x6d9ee3*/
          InterlockedIncrement(lpAddend: &v7->_m_nRef); /*0x6d9ee9*/
      }
      else
      {
        v7 = nullptr; /*0x6d9f96*/
        pQDlg.p = nullptr; /*0x6d9f98*/
      }
      v11 = 0; /*0x6d9eef*/
      v10 = 0; /*0x6d9ef1*/
      v21 = &v9; /*0x6d9efa*/
      LOBYTE(v23) = 2; /*0x6d9eff*/
      v9._m_pStr = nullptr; /*0x6d9f04*/
      ZXString<char>::operator=(this: &v9, s: &asMsg); /*0x6d9f0a*/
      CUtilDlgEx::SetUtilDlgEx(this: v7, dlgType: 0, nTemplateID: v2, sText: v9, bNoNpc: v10, bQuest: v11); /*0x6d9f14*/
      CUtilDlgEx::SetUtilDlgEx_TEXT(this: v7, bPrev: 0, bNext: 0); /*0x6d9f1f*/
      CUtilDlgEx::CreateUtilDlgEx(this: v7); /*0x6d9f26*/
      CDialog::DoModal(this: v7); /*0x6d9f2d*/
      v8 = (CRPSGameDlg *)ZAllocEx<ZAllocAnonSelector>::Alloc( /*0x6d9f3c*/
                            this: &ZAllocEx<ZAllocAnonSelector>::_s_alloc,
                            uSize: 0x13Cu);
      v21 = (ZXString<char> *)v8; /*0x6d9f41*/
      LOBYTE(v23) = 3; /*0x6d9f45*/
      if ( v8 != nullptr ) /*0x6d9f4c*/
        CRPSGameDlg::CRPSGameDlg(this: v8, dwNpcTemplateID: v2); /*0x6d9f51*/
      LOBYTE(v23) = 0; /*0x6d9f5a*/
      ZRef<CUtilDlgEx>::~ZRef<CUtilDlgEx>(this: &pQDlg); /*0x6d9f5f*/
      v23 = -1; /*0x6d9f68*/
      if ( asMsg._m_pStr != nullptr ) /*0x6d9f72*/
        ZXString<char>::_Release(pData: (ZXString<char>::_ZXStringData *)asMsg._m_pStr - 1); /*0x6d9f7c*/
      break; /*0x6d9f95*/
    case 13: /*0x6d9e46*/
      if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr ) /*0x6d9fa8*/
      {
        if ( TSingleton<CUniqueModeless>::ms_pInstance->CDialog::CWnd::IUIMsgHandler::vfptr->IsKindOf( /*0x6d9fc9*/
               this: &TSingleton<CUniqueModeless>::ms_pInstance->IUIMsgHandler,
               a2: &CRPSGameDlg::ms_RTTI_CRPSGameDlg) == 0
          || TSingleton<CUniqueModeless>::ms_pInstance == nullptr )
        {
          CMSException::CMSException(this: &v15, hr: 553648131); /*0x6d9fd4*/
          v16 = v15.m_hr; /*0x6d9fe7*/
          _CxxThrowException(pExceptionObject: &v16, pThrowInfo: &_TI3_AVCDisconnectException__); /*0x6d9feb*/
        }
        CWnd::Destroy(this: TSingleton<CUniqueModeless>::ms_pInstance); /*0x6d9ff0*/
        if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr ) /*0x6d9ffc*/
          TSingleton<CUniqueModeless>::ms_pInstance->CDialog::CWnd::ZRefCounted::vfptr->__vecDelDtor( /*0x6da00c*/
            this: &TSingleton<CUniqueModeless>::ms_pInstance->ZRefCounted,
            a2: 1);
      }
      break; /*0x6da01f*/
    default:
      return;
  }
}