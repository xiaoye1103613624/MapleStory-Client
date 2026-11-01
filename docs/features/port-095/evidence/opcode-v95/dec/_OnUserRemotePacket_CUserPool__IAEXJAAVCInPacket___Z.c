void __userpurge CUserPool::OnUserRemotePacket(CUserPool *this@<ecx>, int a2@<ebx>, int nType, CInPacket *iPacket)
{
  unsigned int v5; // eax
  CUserRemote *RemoteUser; // esi

  v5 = CInPacket::Decode4(this: iPacket); /*0x94b39a*/
  RemoteUser = CUserPool::GetRemoteUser(this, dwCharacterID: v5); /*0x94b3a7*/
  if ( RemoteUser != nullptr && *((_DWORD *)TSingleton<CWvsContext>::ms_pInstance._m_pStr + 3605) == 0 ) /*0x94b3bd*/
  {
    if ( RemoteUser->m_bDelayedLoad == 0 ) /*0x94b3cf*/
    {
      switch ( nType ) /*0x94b3dc*/
      {
        case 211: /*0x94b3dc*/
        case 212: /*0x94b3dc*/
        case 213: /*0x94b3dc*/
        case 214: /*0x94b3dc*/
          CUserRemote::OnAttack(this: RemoteUser, nType, iPacket); /*0x94b3e7*/
          break; /*0x94b3ec*/
        case 215: /*0x94b3dc*/
          CUserRemote::OnSkillPrepare(this: RemoteUser, iPacket); /*0x94b3f1*/
          break; /*0x94b3f6*/
        case 216: /*0x94b3dc*/
          CUserRemote::OnMovingShootAttackPrepare(this: RemoteUser, iPacket); /*0x94b3fb*/
          break; /*0x94b400*/
        case 217: /*0x94b3dc*/
          CUserRemote::OnSkillCancel(this: RemoteUser, iPacket); /*0x94b405*/
          break; /*0x94b40a*/
        case 218: /*0x94b3dc*/
          CUserRemote::OnHit(this: RemoteUser, iPacket); /*0x94b40f*/
          break; /*0x94b414*/
        case 219: /*0x94b3dc*/
          CUser::OnEmotion(this: RemoteUser, iPacket); /*0x94b419*/
          break; /*0x94b41e*/
        case 220: /*0x94b3dc*/
          CUserRemote::OnSetActiveEffectItem(this: RemoteUser, iPacket); /*0x94b423*/
          break; /*0x94b428*/
        case 221: /*0x94b3dc*/
          CUserRemote::OnShowUpgradeTombEffect(this: RemoteUser, iPacket); /*0x94b42d*/
          break; /*0x94b432*/
        case 224: /*0x94b3dc*/
          CUser::OnEffect(this: RemoteUser, iPacket); /*0x94b437*/
          break; /*0x94b43c*/
        case 230: /*0x94b3dc*/
          CUserRemote::OnThrowGrenade(this: RemoteUser, iPacket); /*0x94b441*/
          break; /*0x94b441*/
        default:
          break;
      }
    }
    switch ( a2 ) /*0x94b459*/
    {
      case 210: /*0x94b459*/
        CUserRemote::OnMove(this: RemoteUser, iPacket); /*0x94b463*/
        break; /*0x94b46a*/
      case 222: /*0x94b459*/
        CUserRemote::OnSetActivePortableChair(this: RemoteUser, iPacket); /*0x94b470*/
        break; /*0x94b477*/
      case 223: /*0x94b459*/
        CUserRemote::OnAvatarModified(this: RemoteUser, iPacket); /*0x94b47d*/
        break; /*0x94b484*/
      case 225: /*0x94b459*/
        CUserRemote::OnSetTemporaryStat(this: RemoteUser, iPacket); /*0x94b48a*/
        break; /*0x94b491*/
      case 226: /*0x94b459*/
        CUserRemote::OnResetTemporaryStat(this: RemoteUser, iPacket); /*0x94b497*/
        break; /*0x94b49e*/
      case 227: /*0x94b459*/
        CUserRemote::OnReceiveHP(this: RemoteUser, iPacket); /*0x94b4be*/
        break; /*0x94b4be*/
      case 228: /*0x94b459*/
        CUserRemote::OnGuildNameChanged(this: RemoteUser, iPacket); /*0x94b4a4*/
        break; /*0x94b4ab*/
      case 229: /*0x94b459*/
        CUserRemote::OnGuildMarkChanged(this: RemoteUser, iPacket); /*0x94b4b1*/
        break; /*0x94b4b8*/
      default:
        return;
    }
  }
}