void __thiscall CNpcPool::OnPacket(CNpcPool *this, int nType, CInPacket *iPacket)
{
  switch ( nType ) /*0x679789*/
  {
    case 84: /*0x679789*/
      CNpcPool::OnNpcImitateData(this, iPacket); /*0x6797c6*/
      break; /*0x6797cd*/
    case 85: /*0x679789*/
      CNpcPool::OnUpdateLimitedDisableInfo(this, iPacket); /*0x6797d5*/
      break; /*0x6797dc*/
    case 311: /*0x679789*/
      CNpcPool::OnNpcEnterField(this, iPacket); /*0x679797*/
      break; /*0x67979e*/
    case 312: /*0x679789*/
      CNpcPool::OnNpcLeaveField(this, iPacket); /*0x6797a6*/
      break; /*0x6797ad*/
    case 313: /*0x679789*/
      CNpcPool::OnNpcChangeController(this, iPacket); /*0x6797b5*/
      break; /*0x6797bc*/
    default:
      if ( (unsigned int)(nType - 314) <= 2 ) /*0x6797ed*/
        CNpcPool::OnNpcPacket(this, nType, iPacket); /*0x6797f1*/
      if ( nType == 317 ) /*0x6797fe*/
        CNpcPool::OnNpcTemplatePacket(this, nType: 317, iPacket); /*0x679804*/
      break; /*0x679804*/
  }
}