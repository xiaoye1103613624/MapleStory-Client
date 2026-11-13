// Angel 083 `TryDoingSkill_CMob` @ 0x66e7af
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
int __thiscall CMob::TryDoingSkill(CMob *this, int *a2, int *a3, struct CMob::TARGETINFO *a4)
{
  int v5; // edx
  int v6; // edi
  int v7; // eax
  unsigned int i; // ebx
  int v9; // ecx
  _DWORD *v10; // ebx
  int v11; // eax
  int v12; // eax
  int v13; // eax
  int v14; // ecx
  int *v15; // ecx

  v5 = 0;
  if ( *(this + 169) != 0 )
    return 0;
  if ( *(this + 151) != 0 )
    return 0;
  if ( *(this + 141) != 0 )
    return 0;
  if ( *(this + 144) != 0 )
    return 0;
  if ( *(this + 172) != 0 )
    return 0;
  v6 = *(this + 81);
  if ( v6 == 0 || *(this + 227) != 0 )
    return 0;
  v7 = *(this + 98);
  for ( i = 0; ; ++i )
  {
    v9 = *(v7 + 636);
    if ( v9 == 0 || i >= *(v9 - 4) )
      break;
    if ( v6 == *(v5 + v9 + 12) )
    {
      v10 = (*(v7 + 636) + 28 * i);
      goto LABEL_15;
    }
    v5 += 28;
  }
  v10 = 0;
LABEL_15:
  if ( v10 == 0 )
    return 0;
  *a2 = v10[5] + 21;
  v11 = sub_663FE2(a1: this);
  *a3 = v11;
  BYTE1(v11) = *(this + 328);
  LOBYTE(v11) = *(this + 324);
  *a4 = v11;
  v12 = sub_7632F2(a1: v10[3]);
  MOBSKILLLEVELDATA::GetEffectUOL(a1: *(v12 + 4) + 92 * *(this + 82) - 92, &a2);
  if ( a2 != 0 && *a2 != 0 )
    v13 = *(*a2 - 4) >> 1;
  else
    v13 = 0;
  if ( v13 != 0 )
  {
    sub_402C9B(a1: &a2);
    *(this + 42) = *a3;
    v14 = get_update_time() + v10[6];
    *(this + 41) = v14;
    if ( v14 == 0 )
      *(this + 41) = 1;
  }
  v15 = a2;
  *(this + 81) = 0;
  if ( v15 != 0 )
    _bstr_t::Data_t::Release(a1: v15);
  return 1;
}