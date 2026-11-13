// Angel 083 `OnDamaged_CMob` @ 0x66c6c2
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
_DWORD *__thiscall CMob::OnDamaged(CMob *this, struct CInPacket *a2)
{
  int v3; // ebx
  _DWORD *result; // eax
  int v5; // ebx
  int v6; // eax
  int v7; // [esp+0h] [ebp-Ch]
  int v8; // [esp+4h] [ebp-8h]

  CInPacket::Decode1(this: a2);
  v3 = CInPacket::Decode4(this: a2);
  CMob::ShowDamage(this, nDamage: v3, a3: 0, a4: 0, bHalfHeight: 0, a6: v7, a7: v8);
  result = *(this + 98);
  if ( result[130] != 0 )
  {
    CMob::ShowHitEffect(this, a2: v3);
    v5 = CInPacket::Decode4(this: a2);
    v6 = CInPacket::Decode4(this: a2);
    CMob::CreateHPIndicator(this, a2: 100 * v5 / v6, a3: -65536);
    return CMob::ShowHPIndicator(this);
  }
  return result;
}