// Angel 083 `OnStatSet_CMob` @ 0x66c301
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
int __thiscall CMob::OnStatSet(CMob *this, struct CInPacket *a2)
{
  int result; // eax
  int v4; // [esp-14h] [ebp-48h] BYREF
  _DWORD v5[3]; // [esp-10h] [ebp-44h] BYREF
  struct CInPacket *v6; // [esp-4h] [ebp-38h]
  _BYTE v7[16]; // [esp+Ch] [ebp-28h] BYREF
  _BYTE v8[4]; // [esp+1Ch] [ebp-18h] BYREF
  _DWORD *v9; // [esp+20h] [ebp-14h]
  _DWORD *v10; // [esp+24h] [ebp-10h]
  int v11; // [esp+30h] [ebp-4h]

  UINT128::UINT128(this: v7);
  CInPacket::DecodeBuffer(a1: v7, a2: 0x10u);
  v10 = v5;
  UINT128::UINT128(this: v5, a2: v7, a3: 0x80u);
  if ( MobStat::IsMovementAffectingStat() != 0 && *(this + 330) != 0 )
  {
    v9 = 0;
    ZRef<CMob::ReservedPacket>::_Alloc(a1: v8);
    *v9 = 1;
    v11 = 0;
    UINT128::setValue(this: (v9 + 1), a2: v7);
    CInPacket::operator=(a1: a2);
    ZList<ZRef<CMob::ReservedPacket>>::AddTail(a1: this + 1328);
    result = ZRef<CMob::ReservedPacket>::operator=(a1: v8);
    v11 = -1;
    if ( v9 != 0 )
      return ZRef<CMob::ReservedPacket>::~ZRef<CMob::ReservedPacket>(a1: 0);
  }
  else
  {
    v6 = a2;
    UINT128::UINT128(this: &v4, a2: v7, a3: 0x80u);
    return CMob::ProcessStatSet(a1: this, a2: v4, a3: v5[0], a4: v5[1], a5: v5[2], a6: v6);
  }
  return result;
}