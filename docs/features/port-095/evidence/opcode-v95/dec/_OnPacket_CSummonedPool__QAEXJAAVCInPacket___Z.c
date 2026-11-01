void __thiscall CSummonedPool::OnPacket(CSummonedPool *this, int nType, CInPacket *iPacket)
{
  unsigned int v4; // eax

  v4 = CInPacket::Decode4(this: iPacket); /*0x75ac7a*/
  switch ( nType ) /*0x75ac8e*/
  {
    case 278: /*0x75ac8e*/
      CSummonedPool::OnCreated(this, dwCharacterID: v4, iPacket); /*0x75ac99*/
      break; /*0x75aca0*/
    case 279: /*0x75ac8e*/
      CSummonedPool::OnRemoved(this, dwCharacterID: v4, iPacket); /*0x75aca7*/
      break; /*0x75acae*/
    case 280: /*0x75ac8e*/
      CSummonedPool::OnMove(this, dwCharacterID: v4, iPacket); /*0x75acb5*/
      break; /*0x75acbc*/
    case 281: /*0x75ac8e*/
      CSummonedPool::OnAttack(this, dwCharacterID: v4, iPacket); /*0x75acc3*/
      break; /*0x75acca*/
    case 282: /*0x75ac8e*/
      CSummonedPool::OnSkill(this, dwCharacterID: v4, iPacket); /*0x75acd1*/
      break; /*0x75acd8*/
    case 283: /*0x75ac8e*/
      CSummonedPool::OnHit(this, dwCharacterID: v4, iPacket); /*0x75acdf*/
      break; /*0x75acdf*/
    default:
      return;
  }
}