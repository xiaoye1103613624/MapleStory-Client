// Angel 083 `OnMobChangeController_CMobPool` @ 0x679777
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
DWORD __thiscall CMobPool::OnMobChangeController(CMobPool *this, struct CInPacket *a2)
{
  int v3; // edi
  unsigned int v4; // eax
  _DWORD *v5; // ebp
  unsigned __int8 v6; // al

  v3 = CInPacket::Decode1(this: a2);
  v4 = CInPacket::Decode4(this: a2);
  v5 = v4;
  if ( v3 == 0 )
    return CMobPool::SetRemoteMob(this, a2: v4);
  v6 = CInPacket::Decode1(this: a2);
  return CMobPool::SetLocalMob(this, a2: v3, a3: v5, a4: v6, a5: a2);
}