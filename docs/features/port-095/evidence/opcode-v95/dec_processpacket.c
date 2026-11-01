void __thiscall CClientSocket::ProcessPacket(CClientSocket *this, CInPacket *iPacket)
{
  CStage *p; // ebp
  int v4; // edi
  ZRef<CStage> pStage; // [esp+14h] [ebp-14h] BYREF
  int v6; // [esp+24h] [ebp-4h]

  p = g_pStage.p; /*0x4b0119*/
  pStage.p = g_pStage.p; /*0x4b011f*/
  if ( g_pStage.p != nullptr ) /*0x4b0125*/
    InterlockedIncrement(lpAddend: &g_pStage.p->_m_nRef); /*0x4b012b*/
  v6 = 0; /*0x4b0137*/
  v4 = (unsigned __int16)CInPacket::Decode2(this: iPacket); /*0x4b0144*/
  switch ( v4 ) /*0x4b014f*/
  {
    case 16: /*0x4b014f*/
      CClientSocket::OnMigrateCommand(this, iPacket); /*0x4b0163*/
      break; /*0x4b0168*/
    case 17: /*0x4b014f*/
      CClientSocket::OnAliveReq(this, iPacket); /*0x4b0159*/
      break; /*0x4b015e*/
    case 18: /*0x4b014f*/
      CClientSocket::OnAuthenCodeChanged(iPacket); /*0x4b016d*/
      break; /*0x4b0172*/
    case 19: /*0x4b014f*/
      CClientSocket::OnAuthenMessage(iPacket); /*0x4b0177*/
      break; /*0x4b017c*/
    case 20: /*0x4b014f*/
      if ( TSingleton<CSecurityClient>::ms_pInstance != nullptr ) /*0x4b0186*/
        CSecurityClient::OnPacket(this: TSingleton<CSecurityClient>::ms_pInstance, iPacket); /*0x4b0189*/
      break; /*0x4b018e*/
    case 23: /*0x4b014f*/
      CClientSocket::OnCheckCrcResult(this, iPacket); /*0x4b0193*/
      break; /*0x4b0198*/
    default:
      CWvsApp::SetBackupPacket(this: TSingleton<CWvsApp>::ms_pInstance, iPacket); /*0x4b01a1*/
      if ( (unsigned int)(v4 - 28) > 0x70 ) /*0x4b01ac*/
      {
        if ( p != nullptr ) /*0x4b01bf*/
          p->INetMsgHandler::vfptr->OnPacket(this: &p->INetMsgHandler, a2: v4, a3: iPacket); /*0x4b01cb*/
      }
      else
      {
        CWvsContext::OnPacket(this: (CWvsContext *)TSingleton<CWvsContext>::ms_pInstance._m_pStr, nType: v4, iPacket); /*0x4b01b6*/
      }
      break; /*0x4b01bb*/
  }
  v6 = -1; /*0x4b01cd*/
  if ( p != nullptr ) /*0x4b01d7*/
    ZRef<CStage>::_ReleaseRaw(this: &pStage, __formal: nullptr); /*0x4b01df*/
}