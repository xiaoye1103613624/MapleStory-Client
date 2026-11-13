// Angel 083 `OnCtrlAck_CMob` @ 0x66c23b
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
int __thiscall CMob::OnCtrlAck(CMob *this, struct CInPacket *a2)
{
  int v4; // ebp
  unsigned __int16 v5; // cx
  int v6; // ebx
  unsigned __int8 v7; // al
  int v8; // ecx
  int result; // eax
  __int16 v10; // [esp+14h] [ebp+4h]

  if ( CMob::IsActive(this) == 0 )
    CMob::SetActive(this, a2: 1);
  v10 = CInPacket::Decode2(this: a2);
  v4 = CInPacket::Decode1(this: a2);
  v5 = CInPacket::Decode2(this: a2);
  *(this + 102) = _ZtlSecureTear<long>(a1: v5, a2: this + 400);
  v6 = CInPacket::Decode1(this: a2);
  v7 = CInPacket::Decode1(this: a2);
  if ( *(this + 81) == 0 || v6 != 0 )
  {
    *(this + 81) = v6;
    *(this + 82) = v7;
  }
  if ( v10 - _ZtlSecureFuse<short>(a1: this + 316, a2: *(this + 80)) >= 0 )
    v8 = (v4 != 0) + 3;
  else
    v8 = 2;
  result = _ZtlSecureTear<long>(a1: v8, a2: this + 304);
  *(this + 78) = result;
  return result;
}