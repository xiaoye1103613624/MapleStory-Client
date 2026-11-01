void __thiscall CReactorPool::OnPacket(CReactorPool *this, int nType, CInPacket *iPacket)
{
  switch ( nType ) /*0x6cf9be*/
  {
    case 334: /*0x6cf9be*/
      CReactorPool::OnReactorChangeState(this, iPacket); /*0x6cf9ca*/
      break; /*0x6cf9cf*/
    case 335: /*0x6cf9be*/
      CReactorPool::OnReactorMove(this, iPacket); /*0x6cf9f1*/
      break; /*0x6cf9f1*/
    case 336: /*0x6cf9be*/
      CReactorPool::OnReactorEnterField(this, iPacket); /*0x6cf9d7*/
      break; /*0x6cf9dc*/
    case 337: /*0x6cf9be*/
      CReactorPool::OnReactorLeaveField(this, iPacket); /*0x6cf9e4*/
      break; /*0x6cf9e9*/
    default:
      return;
  }
}