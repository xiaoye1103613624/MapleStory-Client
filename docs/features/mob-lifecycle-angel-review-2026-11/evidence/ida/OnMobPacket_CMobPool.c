// Angel 083 `OnMobPacket_CMobPool` @ 0x67936d
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
void __thiscall CMobPool::OnMobPacket(CMobPool *this, int a2, struct CInPacket *a3)
{
  unsigned int v4; // eax
  CMob *Mob; // ecx

  v4 = CInPacket::Decode4(this: a3);
  Mob = CMobPool::GetMob(this, a2: v4);
  if ( Mob != 0 )
  {
    switch ( a2 )
    {
      case 239:
        CMob::OnMove(this: Mob, a2: a3);
        break;
      case 240:
        CMob::OnCtrlAck(this: Mob, a2: a3);
        break;
      case 242:
        CMob::OnStatSet(this: Mob, a2: a3);
        break;
      case 243:
        CMob::OnStatReset(this: Mob, a2: a3);
        break;
      case 244:
        sub_66C500(a1: a3);
        break;
      case 245:
        sub_66C675(a1: a3);
        break;
      case 246:
        CMob::OnDamaged(this: Mob, a2: a3);
        break;
      case 247:
        sub_66D8E7(a1: a3);
        break;
      case 250:
        sub_66D639(a1: a3);
        break;
      case 251:
        sub_66D6B9(a1: a3);
        break;
      case 252:
        CMob::OnEffectByItem(this: Mob, a2: a3);
        break;
      case 253:
        CMob::OnMobSpeaking(this: Mob, a2: a3);
        break;
      case 254:
        sub_6710FC(a1: a3);
        break;
      case 255:
        sub_670F41(a1: a3);
        break;
      default:
        return;
    }
  }
}