void __thiscall CFuncKeyMappedMan::OnPacket(CFuncKeyMappedMan *this, int nType, CInPacket *iPacket)
{
  switch ( nType ) /*0x568ce9*/
  {
    case 398: /*0x568ce9*/
      CFuncKeyMappedMan::OnInit(this, iPacket); /*0x568d14*/
      break;
    case 399: /*0x568ce9*/
      CFuncKeyMappedMan::OnPetConsumeItemInit(this, iPacket); /*0x568d07*/
      break;
    case 400: /*0x568ce9*/
      CFuncKeyMappedMan::OnPetConsumeMPItemInit(this, iPacket); /*0x568cfa*/
      break;
    default:
      break; /*0x568ce9*/
  }
}