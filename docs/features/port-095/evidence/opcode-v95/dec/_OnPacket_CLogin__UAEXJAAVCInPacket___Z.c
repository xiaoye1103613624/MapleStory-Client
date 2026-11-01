void __thiscall CLogin::OnPacket(CLogin *this, int nType, CInPacket *iPacket)
{
  switch ( nType ) /*0x5df94d*/
  {
    case 0: /*0x5df94d*/
      CLogin::OnCheckPasswordResult(this: (CLogin *)((char *)this - 8), iPacket); /*0x5df95c*/
      break; /*0x5df95c*/
    case 1: /*0x5df94d*/
      CLogin::OnGuestIDLoginResult(this: (CLogin *)((char *)this - 8), iPacket); /*0x5df96c*/
      break; /*0x5df971*/
    case 2: /*0x5df94d*/
      CLogin::OnAccountInfoResult(this: (CLogin *)((char *)this - 8), iPacket); /*0x5df97c*/
      break; /*0x5df981*/
    case 3: /*0x5df94d*/
      CLogin::OnCheckUserLimitResult(this: (CLogin *)((char *)this - 8), iPacket); /*0x5df98c*/
      break; /*0x5df991*/
    case 4: /*0x5df94d*/
      CLogin::OnSetAccountResult(this: (CLogin *)((char *)this - 8), iPacket); /*0x5df99c*/
      break; /*0x5df9a1*/
    case 5: /*0x5df94d*/
      CLogin::OnConfirmEULAResult(this: (CLogin *)((char *)this - 8), iPacket); /*0x5df9ac*/
      break; /*0x5df9b1*/
    case 6: /*0x5df94d*/
      CLogin::OnCheckPinCodeResult(this: (CLogin *)((char *)this - 8), iPacket); /*0x5df9bc*/
      break; /*0x5df9c1*/
    case 7: /*0x5df94d*/
      CLogin::OnUpdatePinCodeResult(this: (CLogin *)((char *)this - 8), iPacket); /*0x5df9cc*/
      break; /*0x5df9d1*/
    case 8: /*0x5df94d*/
      CLogin::OnViewAllCharResult(this: (CLogin *)((char *)this - 8), iPacket); /*0x5df9dc*/
      break; /*0x5df9e1*/
    case 9: /*0x5df94d*/
      CLogin::OnSelectCharacterByVACResult(this: (CLogin *)((char *)this - 8), iPacket); /*0x5df9ec*/
      break; /*0x5df9f1*/
    case 10: /*0x5df94d*/
      CLogin::OnWorldInformation(this: (CLogin *)((char *)this - 8), iPacket); /*0x5df9fc*/
      break; /*0x5dfa01*/
    case 11: /*0x5df94d*/
      CLogin::OnSelectWorldResult(this: (CLogin *)((char *)this - 8), iPacket); /*0x5dfa0c*/
      break; /*0x5dfa11*/
    case 12: /*0x5df94d*/
      CLogin::OnSelectCharacterResult(this: (CLogin *)((char *)this - 8), iPacket); /*0x5dfa4c*/
      break; /*0x5dfa51*/
    case 13: /*0x5df94d*/
      CLogin::OnCheckDuplicatedIDResult(this: (CLogin *)((char *)this - 8), iPacket); /*0x5dfa1c*/
      break; /*0x5dfa21*/
    case 14: /*0x5df94d*/
      CLogin::OnCreateNewCharacterResult(this: (CLogin *)((char *)this - 8), iPacket); /*0x5dfa3c*/
      break; /*0x5dfa41*/
    case 15: /*0x5df94d*/
      CLogin::OnDeleteCharacterResult(this: (CLogin *)((char *)this - 8), iPacket); /*0x5dfa2c*/
      break; /*0x5dfa31*/
    case 21: /*0x5df94d*/
      CLogin::OnEnableSPWResult(this: (CLogin *)((char *)this - 8), iPacket); /*0x5dfa5c*/
      break; /*0x5dfa61*/
    case 24: /*0x5df94d*/
      CLogin::OnLatestConnectedWorld(this: (CLogin *)((char *)this - 8), iPacket); /*0x5dfa7c*/
      break; /*0x5dfa81*/
    case 25: /*0x5df94d*/
      CLogin::OnRecommendWorldMessage(this: (CLogin *)((char *)this - 8), iPacket); /*0x5dfa8c*/
      break; /*0x5dfa91*/
    case 26: /*0x5df94d*/
      CLogin::OnExtraCharInfoResult(this: (CLogin *)((char *)this - 8), iPacket); /*0x5dfa9c*/
      break; /*0x5dfaa1*/
    case 27: /*0x5df94d*/
      CLogin::OnCheckSPWResult(this: (CLogin *)((char *)this - 8), iPacket); /*0x5dfa6c*/
      break; /*0x5dfa71*/
    default:
      if ( (unsigned int)(nType - 141) > 2 ) /*0x5dfaad*/
      {
        if ( (unsigned int)(nType - 144) <= 2 ) /*0x5dfac1*/
          CMapLoadable::OnPacket(this, nType, iPacket); /*0x5dfacb*/
      }
      else
      {
        CStage::OnPacket(this, nType, iPacket); /*0x5dfab3*/
      }
      break; /*0x5dfab3*/
  }
}