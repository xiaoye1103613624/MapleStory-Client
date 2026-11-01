void __thiscall CMobPool::OnMobPacket(CMobPool *this, int nType, CInPacket *iPacket)
{
  unsigned int v4; // eax
  CMob *Mob; // ecx

  v4 = CInPacket::Decode4(this: iPacket); /*0x6570ba*/
  Mob = CMobPool::GetMob(this, dwMobID: v4); /*0x6570c7*/
  if ( Mob != nullptr ) /*0x6570cb*/
  {
    if ( nType == 305 ) /*0x6570da*/
    {
      CMob::OnEscortStopEndPermmision(this: Mob); /*0x6570dc*/
    }
    else
    {
      switch ( nType ) /*0x6570f4*/
      {
        case 287: /*0x6570f4*/
          CMob::OnMove(this: Mob, iPacket); /*0x6570fc*/
          break; /*0x657103*/
        case 288: /*0x6570f4*/
          CMob::OnCtrlAck(this: Mob, iPacket); /*0x657107*/
          break; /*0x65710e*/
        case 290: /*0x6570f4*/
          CMob::OnStatSet(this: Mob, iPacket); /*0x657112*/
          break; /*0x657119*/
        case 291: /*0x6570f4*/
          CMob::OnStatReset(this: Mob, iPacket); /*0x65711d*/
          break; /*0x657124*/
        case 292: /*0x6570f4*/
          CMob::OnSuspendReset(this: Mob, iPacket); /*0x65716a*/
          break; /*0x657171*/
        case 293: /*0x6570f4*/
          CMob::OnAffected(this: Mob, iPacket); /*0x657128*/
          break; /*0x65712f*/
        case 294: /*0x6570f4*/
          CMob::OnDamaged(this: Mob, iPacket); /*0x657133*/
          break; /*0x65713a*/
        case 295: /*0x6570f4*/
          CMob::OnSpecialEffectBySkill(this: Mob, iPacket); /*0x65715f*/
          break; /*0x657166*/
        case 298: /*0x6570f4*/
          CMob::OnHPIndicator(this: Mob, iPacket); /*0x65713e*/
          break; /*0x657145*/
        case 299: /*0x6570f4*/
          CMob::OnCatchEffect(this: Mob, iPacket); /*0x657149*/
          break; /*0x657150*/
        case 300: /*0x6570f4*/
          CMob::OnEffectByItem(this: Mob, iPacket); /*0x657154*/
          break; /*0x65715b*/
        case 301: /*0x6570f4*/
          CMob::OnMobSpeaking(this: Mob, iPacket); /*0x657180*/
          break; /*0x657187*/
        case 302: /*0x6570f4*/
          CMob::OnIncMobChargeCount(this: Mob, iPacket); /*0x6571c2*/
          break; /*0x6571c2*/
        case 303: /*0x6570f4*/
          CMob::OnMobSkillDelay(this: Mob, iPacket); /*0x65718b*/
          break; /*0x657192*/
        case 304: /*0x6570f4*/
          CMob::OnEscortFullPath(this: Mob, iPacket); /*0x657196*/
          break; /*0x65719d*/
        case 306: /*0x6570f4*/
          CMob::OnEscortStopSay(this: Mob, iPacket); /*0x6571a1*/
          break; /*0x6571a8*/
        case 307: /*0x6570f4*/
          CMob::OnEscortReturnBefore(this: Mob, iPaket: iPacket); /*0x6571ac*/
          break; /*0x6571b3*/
        case 308: /*0x6570f4*/
          CMob::OnNextAttack(this: Mob, iPacket); /*0x6571b7*/
          break; /*0x6571be*/
        case 309: /*0x6570f4*/
          CMob::OnMobAttackedByMob(this: Mob, iPacket); /*0x657175*/
          break; /*0x65717c*/
        default:
          return;
      }
    }
  }
}