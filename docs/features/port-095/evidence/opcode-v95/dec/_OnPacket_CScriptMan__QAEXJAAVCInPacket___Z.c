void __thiscall CScriptMan::OnPacket(CScriptMan *this, int nType, CInPacket *iPacket)
{
  if ( nType == 363 ) /*0x6de368*/
    CScriptMan::OnScriptMessage(this, iPacket); /*0x6de36f*/
}