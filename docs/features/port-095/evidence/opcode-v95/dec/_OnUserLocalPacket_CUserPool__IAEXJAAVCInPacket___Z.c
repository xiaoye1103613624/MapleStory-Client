void __thiscall CUserPool::OnUserLocalPacket(CUserPool *this, int nType, CInPacket *iPacket)
{
  CUserLocal *p; // ecx

  p = this->m_pUserLocal.p; /*0x948aa0*/
  if ( p != nullptr ) /*0x948aa5*/
    CUserLocal::OnPacket(this: p, nType, iPacket); /*0x948aa7*/
}