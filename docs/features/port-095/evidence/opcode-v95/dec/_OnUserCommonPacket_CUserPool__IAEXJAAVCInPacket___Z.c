void __thiscall CUserPool::OnUserCommonPacket(CUserPool *this, int nType, CInPacket *iPacket)
{
  unsigned int v4; // eax
  CUser *User; // eax
  int v6; // ebx
  CUserLocal *p; // esi
  int v8; // [esp+0h] [ebp-10h]
  int v9; // [esp+4h] [ebp-Ch]

  v4 = CInPacket::Decode4(this: iPacket); /*0x94cdbc*/
  User = CUserPool::GetUser(this, dwCharacterID: v4); /*0x94cdc4*/
  v6 = nType; /*0x94cdc9*/
  p = (CUserLocal *)User; /*0x94cdcd*/
  if ( nType == 182 ) /*0x94cdd5*/
  {
    if ( User != nullptr ) /*0x94cdd9*/
      v6 = 181; /*0x94cde0*/
    else
      p = this->m_pUserLocal.p; /*0x94cddb*/
  }
  if ( p != nullptr ) /*0x94cde7*/
  {
    switch ( v6 ) /*0x94cdfc*/
    {
      case 181: /*0x94cdfc*/
        CUser::OnChat(this: p, iPacket, bIsFromOutsideOfMap: 0); /*0x94ce08*/
        break; /*0x94ce11*/
      case 182: /*0x94cdfc*/
        CUser::OnChat(this: p, iPacket, bIsFromOutsideOfMap: 1); /*0x94ce19*/
        break; /*0x94ce22*/
      case 183: /*0x94cdfc*/
        CUser::OnADBoard(this: p, iPacket); /*0x94ce28*/
        break; /*0x94ce31*/
      case 184: /*0x94cdfc*/
        CUser::OnMiniRoomBalloon(this: p, iPacket); /*0x94ce37*/
        break; /*0x94ce40*/
      case 185: /*0x94cdfc*/
        CUser::SetConsumeItemEffect(this: p, iPacket); /*0x94ce46*/
        break; /*0x94ce4f*/
      case 186: /*0x94cdfc*/
        CUser::ShowItemUpgradeEffect(this: p, iPacket); /*0x94ce55*/
        break; /*0x94ce5e*/
      case 187: /*0x94cdfc*/
        CUser::ShowItemHyperUpgradeEffect(this: p, iPacket); /*0x94ce64*/
        break; /*0x94ce6d*/
      case 188: /*0x94cdfc*/
        CUser::ShowItemOptionUpgradeEffect(this: p, iPacket); /*0x94ce82*/
        break; /*0x94ce8b*/
      case 189: /*0x94cdfc*/
        CUser::ShowItemReleaseEffect(this: p, iPacket); /*0x94ce73*/
        break; /*0x94ce7c*/
      case 190: /*0x94cdfc*/
        CUser::ShowItemUnreleaseEffect(this: p, iPacket); /*0x94ce91*/
        break; /*0x94ce9a*/
      case 191: /*0x94cdfc*/
        CUser::OnHitByUser(this: p, iPacket); /*0x94cea0*/
        break; /*0x94cea9*/
      case 192: /*0x94cdfc*/
        CUser::OnTeslaTriangle(this: p, iPacket); /*0x94ceaf*/
        break; /*0x94ceb8*/
      case 193: /*0x94cdfc*/
        CUser::OnFollowCharacter(this: p, iPacket, a3: v8, a4: v9); /*0x94cebe*/
        break; /*0x94cec7*/
      case 194: /*0x94cdfc*/
        CUser::OnShowPQReward(this: p, iPacket); /*0x94cecd*/
        break; /*0x94ced6*/
      case 195: /*0x94cdfc*/
        CUser::OnSetPhase(this: p, iPacket); /*0x94cedc*/
        break; /*0x94cee5*/
      case 197: /*0x94cdfc*/
        CUser::ShowRecoverUpgradeCountEffect(this: p, iPacket); /*0x94ceeb*/
        break; /*0x94cef4*/
      default:
        if ( (unsigned int)(v6 - 198) <= 7 ) /*0x94cf00*/
          CUser::OnPetPacket(this: p, nType: v6, iPacket); /*0x94cf06*/
        if ( (unsigned int)(v6 - 206) <= 2 ) /*0x94cf14*/
          CUser::OnDragonPacket(this: p, nType: v6, iPacket); /*0x94cf1a*/
        break; /*0x94cf1a*/
    }
  }
}