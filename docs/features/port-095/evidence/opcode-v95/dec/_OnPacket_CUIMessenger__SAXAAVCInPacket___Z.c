void __cdecl CUIMessenger::OnPacket(CInPacket *iPacket)
{
  char v1; // al

  v1 = CInPacket::Decode1(this: iPacket); /*0x7f5e4c*/
  if ( v1 == 3 ) /*0x7f5e52*/
  {
    CUIMessenger::OnInvite(iPacket); /*0x7f5e55*/
  }
  else if ( TSingleton<CUIMessenger>::ms_pInstance != nullptr ) /*0x7f5e67*/
  {
    switch ( v1 ) /*0x7f5e6e*/
    {
      case 0: /*0x7f5e6e*/
        CUIMessenger::OnEnter(this: TSingleton<CUIMessenger>::ms_pInstance, iPacket); /*0x7f5e7e*/
        break; /*0x7f5e84*/
      case 1: /*0x7f5e6e*/
        CUIMessenger::OnSelfEnterResult(this: TSingleton<CUIMessenger>::ms_pInstance, iPacket); /*0x7f5e76*/
        break; /*0x7f5e7c*/
      case 2: /*0x7f5e6e*/
        CUIMessenger::OnLeave(this: TSingleton<CUIMessenger>::ms_pInstance, iPacket); /*0x7f5e86*/
        break; /*0x7f5e8c*/
      case 4: /*0x7f5e6e*/
        CUIMessenger::OnInviteResult(this: TSingleton<CUIMessenger>::ms_pInstance, iPacket); /*0x7f5e8e*/
        break; /*0x7f5e94*/
      case 5: /*0x7f5e6e*/
        CUIMessenger::OnBlocked(this: TSingleton<CUIMessenger>::ms_pInstance, iPacket); /*0x7f5e96*/
        break; /*0x7f5e9c*/
      case 6: /*0x7f5e6e*/
        CUIMessenger::OnChat(this: TSingleton<CUIMessenger>::ms_pInstance, iPacket); /*0x7f5e9e*/
        break; /*0x7f5ea4*/
      case 7: /*0x7f5e6e*/
        CUIMessenger::OnAvatar(this: TSingleton<CUIMessenger>::ms_pInstance, iPacket); /*0x7f5ea6*/
        break; /*0x7f5eac*/
      case 8: /*0x7f5e6e*/
        CUIMessenger::OnMigrated(this: TSingleton<CUIMessenger>::ms_pInstance, iPacket); /*0x7f5eae*/
        break; /*0x7f5eae*/
      default:
        return;
    }
  }
}