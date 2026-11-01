void __thiscall COpenGatePool::OnPacket(COpenGatePool *this, int nType, CInPacket *iPacket)
{
  if ( nType == 332 ) /*0x68c8b9*/
  {
    COpenGatePool::OnOpenGateCreated(this, iPacket); /*0x68c8d2*/
  }
  else if ( nType == 333 ) /*0x68c8be*/
  {
    COpenGatePool::OnOpenGateRemoved(this, iPacket); /*0x68c8c5*/
  }
}