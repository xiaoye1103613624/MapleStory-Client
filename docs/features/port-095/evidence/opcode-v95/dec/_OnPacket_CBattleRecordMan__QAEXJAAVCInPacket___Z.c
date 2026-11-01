void __thiscall CBattleRecordMan::OnPacket(CBattleRecordMan *this, int nType, CInPacket *iPacket)
{
  if ( nType == 421 ) /*0x470ba9*/
  {
    CBattleRecordMan::OnDotDamageInfo(this, iPacket); /*0x470bc2*/
  }
  else if ( nType == 422 ) /*0x470bae*/
  {
    CBattleRecordMan::OnServerOnCalcRequestResult(this, iPacket); /*0x470bb5*/
  }
}