void __thiscall CAffectedAreaPool::OnPacket(CAffectedAreaPool *this, int nType, CInPacket *iPacket)
{
  if ( nType == 328 ) /*0x438339*/
  {
    CAffectedAreaPool::OnAffectedAreaCreated(this, iPacket); /*0x438352*/
  }
  else if ( nType == 329 ) /*0x43833e*/
  {
    CAffectedAreaPool::OnAffectedAreaRemoved(this, iPacket); /*0x438345*/
  }
}