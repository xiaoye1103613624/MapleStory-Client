void __thiscall CUserPool::OnPacket(CUserPool *this, int nType, CInPacket *iPacket)
{
  if ( nType == 179 ) /*0x94ddfc*/
  {
    CUserPool::OnUserEnterField(this, iPacket); /*0x94de51*/
  }
  else if ( nType == 180 ) /*0x94de01*/
  {
    CUserPool::OnUserLeaveField(this, iPacket); /*0x94de44*/
  }
  else if ( (unsigned int)(nType - 181) > 0x1C ) /*0x94de0c*/
  {
    if ( (unsigned int)(nType - 210) > 0x14 ) /*0x94de20*/
    {
      if ( (unsigned int)(nType - 231) <= 0x2D ) /*0x94de34*/
        CUserPool::OnUserLocalPacket(this, nType, iPacket); /*0x94de3a*/
    }
    else
    {
      CUserPool::OnUserRemotePacket(this, nType, iPacket); /*0x94de26*/
    }
  }
  else
  {
    CUserPool::OnUserCommonPacket(this, nType, iPacket); /*0x94de12*/
  }
}