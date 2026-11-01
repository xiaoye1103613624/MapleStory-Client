void __thiscall CDropPool::OnPacket(CDropPool *this, int nType, CInPacket *iPacket)
{
  if ( nType == 322 ) /*0x517419*/
  {
    CDropPool::OnDropEnterField(this, iPacket); /*0x517432*/
  }
  else if ( nType == 324 ) /*0x51741e*/
  {
    CDropPool::OnDropLeaveField(this, iPacket); /*0x517425*/
  }
}