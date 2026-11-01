void __thiscall CMapleTVMan::OnPacket(CMapleTVMan *this, int nType, CInPacket *iPacket)
{
  switch ( nType ) /*0x60fe19*/
  {
    case 405: /*0x60fe19*/
      CMapleTVMan::OnSetMessage(this, iPacket); /*0x60fe44*/
      break;
    case 406: /*0x60fe19*/
      CMapleTVMan::OnClearMessage(this, iPacket); /*0x60fe37*/
      break;
    case 407: /*0x60fe19*/
      CMapleTVMan::OnSendMessageResult(this, iPacket); /*0x60fe2a*/
      break;
    default:
      break; /*0x60fe19*/
  }
}