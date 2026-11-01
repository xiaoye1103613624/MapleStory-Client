void __thiscall GW_CharacterStat::Decode(GW_CharacterStat *this, CInPacket *iPacket, int bBackwardUpdate)
{
  char *sCharacterName; // eax
  char v5; // al
  __int16 v6; // ax
  __int16 v7; // ax
  __int16 v8; // ax
  __int16 v9; // ax
  __int16 v10; // ax
  int v11; // eax
  int v12; // eax
  int v13; // eax
  int v14; // eax
  __int16 v15; // ax
  unsigned int v16; // eax
  int v17; // ecx
  __int16 v18; // ax
  int v19; // eax
  __int16 v20; // ax
  int v21; // eax
  unsigned int v22; // eax
  unsigned int ZtlSecureTear_nJob_CS; // [esp-4h] [ebp-24h]
  char sTemp[13]; // [esp+Ch] [ebp-14h] BYREF

  if ( bBackwardUpdate != 0 ) /*0x4f9d5f*/
  {
    CInPacket::Decode4(this: iPacket); /*0x4f9d61*/
    sCharacterName = sTemp; /*0x4f9d66*/
  }
  else
  {
    this->dwCharacterID = CInPacket::Decode4(this: iPacket); /*0x4f9d71*/
    sCharacterName = this->sCharacterName; /*0x4f9d73*/
  }
  CInPacket::DecodeBuffer(this: iPacket, p: sCharacterName, uSize: 0xDu); /*0x4f9d7b*/
  if ( bBackwardUpdate != 0 ) /*0x4f9d84*/
  {
    CInPacket::Decode1(this: iPacket); /*0x4f9d86*/
    CInPacket::Decode1(this: iPacket); /*0x4f9d8d*/
    CInPacket::Decode4(this: iPacket); /*0x4f9d94*/
    CInPacket::Decode4(this: iPacket); /*0x4f9d9b*/
  }
  else
  {
    this->nGender = CInPacket::Decode1(this: iPacket); /*0x4f9da9*/
    this->nSkin = CInPacket::Decode1(this: iPacket); /*0x4f9db3*/
    this->nFace = CInPacket::Decode4(this: iPacket); /*0x4f9dbd*/
    this->nHair = CInPacket::Decode4(this: iPacket); /*0x4f9dc5*/
  }
  CInPacket::DecodeBuffer(this: iPacket, p: this->aliPetLockerSN, uSize: 0x18u); /*0x4f9dd0*/
  v5 = CInPacket::Decode1(this: iPacket); /*0x4f9dd7*/
  this->_ZtlSecureTear_nLevel_CS = _ZtlSecureTear<unsigned char>(t: v5, at: this->_ZtlSecureTear_nLevel); /*0x4f9de8*/
  v6 = CInPacket::Decode2(this: iPacket); /*0x4f9deb*/
  this->_ZtlSecureTear_nJob_CS = _ZtlSecureTear<short>(t: v6, at: this->_ZtlSecureTear_nJob); /*0x4f9dff*/
  v7 = CInPacket::Decode2(this: iPacket); /*0x4f9e02*/
  this->_ZtlSecureTear_nSTR_CS = _ZtlSecureTear<short>(t: v7, at: this->_ZtlSecureTear_nSTR); /*0x4f9e14*/
  v8 = CInPacket::Decode2(this: iPacket); /*0x4f9e17*/
  this->_ZtlSecureTear_nDEX_CS = _ZtlSecureTear<short>(t: v8, at: this->_ZtlSecureTear_nDEX); /*0x4f9e29*/
  v9 = CInPacket::Decode2(this: iPacket); /*0x4f9e2c*/
  this->_ZtlSecureTear_nINT_CS = _ZtlSecureTear<short>(t: v9, at: this->_ZtlSecureTear_nINT); /*0x4f9e3e*/
  v10 = CInPacket::Decode2(this: iPacket); /*0x4f9e41*/
  this->_ZtlSecureTear_nLUK_CS = _ZtlSecureTear<short>(t: v10, at: this->_ZtlSecureTear_nLUK); /*0x4f9e53*/
  v11 = CInPacket::Decode4(this: iPacket); /*0x4f9e56*/
  this->_ZtlSecureTear_nHP_CS = _ZtlSecureTear<long>(t: v11, at: this->_ZtlSecureTear_nHP); /*0x4f9e67*/
  v12 = CInPacket::Decode4(this: iPacket); /*0x4f9e6a*/
  this->_ZtlSecureTear_nMHP_CS = _ZtlSecureTear<long>(t: v12, at: this->_ZtlSecureTear_nMHP); /*0x4f9e7b*/
  v13 = CInPacket::Decode4(this: iPacket); /*0x4f9e7e*/
  this->_ZtlSecureTear_nMP_CS = _ZtlSecureTear<long>(t: v13, at: this->_ZtlSecureTear_nMP); /*0x4f9e8f*/
  v14 = CInPacket::Decode4(this: iPacket); /*0x4f9e95*/
  this->_ZtlSecureTear_nMMP_CS = _ZtlSecureTear<long>(t: v14, at: this->_ZtlSecureTear_nMMP); /*0x4f9ea9*/
  v15 = CInPacket::Decode2(this: iPacket); /*0x4f9eaf*/
  v16 = _ZtlSecureTear<short>(t: v15, at: this->_ZtlSecureTear_nAP); /*0x4f9ebd*/
  ZtlSecureTear_nJob_CS = this->_ZtlSecureTear_nJob_CS; /*0x4f9ec5*/
  this->_ZtlSecureTear_nAP_CS = v16; /*0x4f9ec7*/
  v17 = _ZtlSecureFuse<short>(at: this->_ZtlSecureTear_nJob, uCS: ZtlSecureTear_nJob_CS); /*0x4f9ed2*/
  if ( v17 / 1000 == 3 || v17 / 100 == 22 || v17 == 2001 ) /*0x4f9f0a*/
  {
    this->_ZtlSecureTear_nSP_CS = _ZtlSecureTear<short>(t: 0, at: this->_ZtlSecureTear_nSP); /*0x4f9f20*/
    ExtendSP::Decode(this: &this->extendSP, iPacket); /*0x4f9f26*/
  }
  else
  {
    v18 = CInPacket::Decode2(this: iPacket); /*0x4f9f2f*/
    this->_ZtlSecureTear_nSP_CS = _ZtlSecureTear<short>(t: v18, at: this->_ZtlSecureTear_nSP); /*0x4f9f48*/
    ZList<SPSet>::RemoveAll(this: &this->extendSP.lSPSet); /*0x4f9f4e*/
  }
  v19 = CInPacket::Decode4(this: iPacket); /*0x4f9f55*/
  this->_ZtlSecureTear_nEXP_CS = _ZtlSecureTear<long>(t: v19, at: this->_ZtlSecureTear_nEXP); /*0x4f9f69*/
  v20 = CInPacket::Decode2(this: iPacket); /*0x4f9f6f*/
  this->_ZtlSecureTear_nPOP_CS = _ZtlSecureTear<short>(t: v20, at: this->_ZtlSecureTear_nPOP); /*0x4f9f84*/
  v21 = CInPacket::Decode4(this: iPacket); /*0x4f9f8a*/
  this->_ZtlSecureTear_nTempEXP_CS = _ZtlSecureTear<long>(t: v21, at: this->_ZtlSecureTear_nTempEXP); /*0x4f9f9e*/
  v22 = CInPacket::Decode4(this: iPacket); /*0x4f9fa4*/
  this->_ZtlSecureTear_dwPosMap_CS = _ZtlSecureTear<unsigned long>(t: v22, at: this->_ZtlSecureTear_dwPosMap); /*0x4f9fb8*/
  this->nPortal = CInPacket::Decode1(this: iPacket); /*0x4f9fc5*/
  this->nPlaytime = CInPacket::Decode4(this: iPacket); /*0x4f9fd2*/
  this->nSubJob = CInPacket::Decode2(this: iPacket); /*0x4f9fe2*/
}