void __thiscall CNpcPool::OnNpcTemplatePacket(CNpcPool *this, int nType, CInPacket *iPacket)
{
  if ( nType == 317 ) /*0x678618*/
    CNpcTemplate::OnSetNpcScript(iPacket); /*0x67861f*/
}