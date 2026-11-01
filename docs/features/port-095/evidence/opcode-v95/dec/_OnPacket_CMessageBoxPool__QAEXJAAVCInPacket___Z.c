void __thiscall CMessageBoxPool::OnPacket(CMessageBoxPool *this, int nType, CInPacket *iPacket)
{
  switch ( nType ) /*0x637159*/
  {
    case 325: /*0x637159*/
      CMessageBoxPool::OnCreateFailed(this, iPacket); /*0x637184*/
      break;
    case 326: /*0x637159*/
      CMessageBoxPool::OnMessageBoxEnterField(this, iPacket); /*0x637177*/
      break;
    case 327: /*0x637159*/
      CMessageBoxPool::OnMessageBoxLeaveField(this, iPacket); /*0x63716a*/
      break;
    default:
      break; /*0x637159*/
  }
}