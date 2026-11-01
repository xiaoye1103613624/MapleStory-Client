void __thiscall CEmployeePool::OnPacket(CEmployeePool *this, int nType, CInPacket *iPacket)
{
  switch ( nType ) /*0x519029*/
  {
    case 319: /*0x519029*/
      CEmployeePool::OnEmployeeEnterField(this, iPacket); /*0x519054*/
      break;
    case 320: /*0x519029*/
      CEmployeePool::OnEmployeeLeaveField(this, iPacket); /*0x519047*/
      break;
    case 321: /*0x519029*/
      CEmployeePool::OnEmployeeMiniRoomBalloon(this, iPacket); /*0x51903a*/
      break;
    default:
      break; /*0x519029*/
  }
}