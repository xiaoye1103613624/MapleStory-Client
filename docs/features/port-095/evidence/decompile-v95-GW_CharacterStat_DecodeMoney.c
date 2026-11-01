void __thiscall GW_CharacterStat::DecodeMoney(GW_CharacterStat *this, CInPacket *iPacket)
{
  int v3; // eax

  v3 = CInPacket::Decode4(this: iPacket); /*0x4f52e7*/
  this->_ZtlSecureTear_nMoney_CS = _ZtlSecureTear<long>(t: v3, at: this->_ZtlSecureTear_nMoney); /*0x4f52f9*/
}