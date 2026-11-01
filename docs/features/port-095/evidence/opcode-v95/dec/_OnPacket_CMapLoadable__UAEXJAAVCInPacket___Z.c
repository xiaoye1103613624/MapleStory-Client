void __thiscall CMapLoadable::OnPacket(CMapLoadable *this, int nType, CInPacket *iPacket)
{
  switch ( nType ) /*0x61fd89*/
  {
    case 144: /*0x61fd89*/
      CMapLoadable::OnSetBackEffect(this: (CMapLoadable *)((char *)this - 8), iPacket); /*0x61fdbd*/
      break;
    case 145: /*0x61fd89*/
      CMapLoadable::OnSetMapObjectVisible(this: (CMapLoadable *)((char *)this - 8), iPacket); /*0x61fdad*/
      break;
    case 146: /*0x61fd89*/
      CMapLoadable::OnClearBackEffect(this: (CMapLoadable *)((char *)this - 8), iPacket); /*0x61fd9d*/
      break;
    default:
      break; /*0x61fd89*/
  }
}