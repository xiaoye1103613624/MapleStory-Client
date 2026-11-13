// Angel 083 `OnPacket_CMobPool` @ 0x67930e
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
void __thiscall CMobPool::OnPacket(CMobPool *this, signed int a2, struct CInPacket *a3)
{
  switch ( a2 )
  {
    case 236:
      CMobPool::OnMobEnterField(a1: a3);
      break;
    case 237:
      CMobPool::OnMobLeaveField(a1: a3);
      break;
    case 238:
      CMobPool::OnMobChangeController(a1: a3);
      break;
    case 249:
      CMobPool::OnMobCrcKeyChanged(a1: a3);
      break;
    default:
      if ( a2 >= 239 && a2 <= 255 )
        CMobPool::OnMobPacket(this, a2, a3);
      break;
  }
}