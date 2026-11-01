void __thiscall CNpcPool::OnNpcPacket(CNpcPool *this, int nType, CInPacket *iPacket)
{
  unsigned int v4; // eax
  CNpc *Npc; // ecx

  v4 = CInPacket::Decode4(this: iPacket); /*0x67926a*/
  Npc = CNpcPool::GetNpc(this, dwNpcId: v4); /*0x679277*/
  if ( Npc != nullptr ) /*0x67927b*/
  {
    switch ( nType ) /*0x679286*/
    {
      case 314: /*0x679286*/
        CNpc::OnMove(this: Npc, iPacket); /*0x6792a9*/
        break;
      case 315: /*0x679286*/
        CNpc::OnUpdateLimitedInfo(this: Npc, iPacket); /*0x67929e*/
        break;
      case 316: /*0x679286*/
        CNpc::OnSetSpecialAction(this: Npc, iPacket); /*0x679293*/
        break;
      default:
        break; /*0x679286*/
    }
  }
}