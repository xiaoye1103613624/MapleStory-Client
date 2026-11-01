void __thiscall CMobPool::OnPacket(CMobPool *this, int nType, CInPacket *iPacket)
{
  switch ( nType ) /*0x658e16*/
  {
    case 284: /*0x658e16*/
      CMobPool::OnMobEnterField(this, iPacket); /*0x658e22*/
      break; /*0x658e22*/
    case 285: /*0x658e16*/
      CMobPool::OnMobLeaveField(this, iPacket); /*0x658e2f*/
      break; /*0x658e34*/
    case 286: /*0x658e16*/
      CMobPool::OnMobChangeController(this, iPacket); /*0x658e3c*/
      break; /*0x658e41*/
    case 297: /*0x658e16*/
      CMobPool::OnMobCrcKeyChanged(this, iPacket); /*0x658e49*/
      break; /*0x658e4e*/
    default:
      if ( (unsigned int)(nType - 287) <= 0x16 ) /*0x658e5a*/
        CMobPool::OnMobPacket(this, nType, iPacket); /*0x658e60*/
      break; /*0x658e60*/
  }
}