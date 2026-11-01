void __thiscall CTownPortalPool::OnPacket(CTownPortalPool *this, int nType, CInPacket *iPacket)
{
  if ( nType == 330 ) /*0x7636b9*/
  {
    CTownPortalPool::OnTownPortalCreated(this, iPacket); /*0x7636d2*/
  }
  else if ( nType == 331 ) /*0x7636be*/
  {
    CTownPortalPool::OnTownPortalRemoved(this, iPacket); /*0x7636c5*/
  }
}