void __cdecl CMiniRoomBaseDlg::OnPacketBase(CInPacket *iPacket)
{
  int v1; // edi

  v1 = CInPacket::Decode1(this: iPacket); /*0x639e1d*/
  if ( TSingleton<CUniqueModeless>::ms_pInstance != nullptr ) /*0x639e27*/
  {
    if ( TSingleton<CUniqueModeless>::ms_pInstance->CDialog::CWnd::IUIMsgHandler::vfptr->IsKindOf( /*0x639e83*/
           this: &TSingleton<CUniqueModeless>::ms_pInstance->IUIMsgHandler,
           a2: &CMiniRoomBaseDlg::ms_RTTI_CMiniRoomBaseDlg) != 0
      && TSingleton<CUniqueModeless>::ms_pInstance != nullptr )
    {
      switch ( v1 ) /*0x639e8d*/
      {
        case 3: /*0x639e8d*/
$LN6_82: /*0x639e94*/
          CMiniRoomBaseDlg::OnInviteResultStatic(iPacket); /*0x639e94*/
          break; /*0x639e9f*/
        case 4: /*0x639e8d*/
          CMiniRoomBaseDlg::OnEnterBase(this: (CMiniRoomBaseDlg *)TSingleton<CUniqueModeless>::ms_pInstance, iPacket); /*0x639ea1*/
          break; /*0x639ea8*/
        case 6: /*0x639e8d*/
          ((void (__stdcall *)(CInPacket *))TSingleton<CUniqueModeless>::ms_pInstance->CDialog::CWnd::IGObj::vfptr[20].Update)(a1: iPacket); /*0x639eaf*/
          break; /*0x639eb3*/
        case 9: /*0x639e8d*/
          CMiniRoomBaseDlg::OnAvatar(this: (CMiniRoomBaseDlg *)TSingleton<CUniqueModeless>::ms_pInstance, iPacket); /*0x639eb5*/
          break; /*0x639ebc*/
        case 10: /*0x639e8d*/
          CMiniRoomBaseDlg::OnLeaveBase(this: (CMiniRoomBaseDlg *)TSingleton<CUniqueModeless>::ms_pInstance, iPacket); /*0x639ebe*/
          break; /*0x639ec5*/
        default:
          ((void (__stdcall *)(int, CInPacket *))TSingleton<CUniqueModeless>::ms_pInstance->CDialog::CWnd::IGObj::vfptr[16].Update)( /*0x639ecd*/
            a1: v1,
            a2: iPacket);
          break; /*0x639ecd*/
      }
    }
  }
  else
  {
    switch ( v1 ) /*0x639e3c*/
    {
      case 2: /*0x639e3c*/
        CMiniRoomBaseDlg::OnInviteStatic(iPacket); /*0x639e50*/
        break; /*0x639e5a*/
      case 3: /*0x639e3c*/
        goto $LN6_82;
      case 5: /*0x639e3c*/
        CMiniRoomBaseDlg::OnEnterResultStatic(iPacket); /*0x639e44*/
        break; /*0x639e4e*/
      case 14: /*0x639e3c*/
        CMiniRoomBaseDlg::OnCheckSSN2Static(iPacket); /*0x639e5c*/
        break; /*0x639e66*/
      default:
        return;
    }
  }
}