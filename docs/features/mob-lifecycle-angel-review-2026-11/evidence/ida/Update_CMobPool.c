// Angel 083 `Update_CMobPool` @ 0x679138
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
void __thiscall CMobPool::Update(CMobPool *this)
{
  CMapLoadable *field; // eax
  int v3; // esi
  void (__thiscall ***v4)(_DWORD); // ecx
  int v5; // edx
  int v6; // esi
  int v7; // edi
  int v8; // edx
  CMob *v9; // eax
  void (__thiscall **v10)(CMob *); // eax
  int v11; // esi
  int v12; // edi
  int v13; // edx
  int v14; // eax
  void (__thiscall **v15)(int); // eax
  unsigned int *v16; // edi
  int v17; // edx
  int *Mob; // eax
  unsigned int v19; // [esp-4h] [ebp-2Ch]
  char v20[4]; // [esp+Ch] [ebp-1Ch] BYREF
  int v21; // [esp+10h] [ebp-18h]
  char v22[4]; // [esp+14h] [ebp-14h] BYREF
  void (__thiscall ***v23)(CMob *); // [esp+18h] [ebp-10h]
  int v24; // [esp+24h] [ebp-4h]

  v23 = 0;
  field = get_field();
  CMapLoadable::UpdateObstacleInfo(this: field);
  v3 = *(this + 10);
  while ( v3 != 0 )
  {
    v4 = *(v3 + 4);
    v5 = *(v3 != 0 ? v3 - 16 + 4 : 4);
    v3 = v5 != 0 ? v5 + 16 : 0;
    (**v4)(a1: v4);
  }
  v6 = *(this + 15);
  while ( v6 != 0 )
  {
    v7 = v6;
    v8 = *(v6 != 0 ? v6 - 16 + 4 : 4);
    v9 = *(v6 + 4);
    v6 = v8 != 0 ? v8 + 16 : 0;
    v23 = v9;
    sub_6726BC(a1: v22);
    v10 = *v23;
    v24 = 0;
    (*v10)(a1: v23);
    if ( CMob::GetRemainDamageInfoDelay(this: v23) <= 0 )
    {
      sub_679A04(this: this + 12, a2: v7);
      if ( _ZtlSecureFuse<long>(a1: v23 + 273, a2: v23[275]) <= 1
        || _ZtlSecureFuse<long>(a1: v23 + 273, a2: v23[275]) == 3 )
      {
        CMob::OnDie(this: v23);
      }
      else
      {
        sub_663E5B(this: v23);
      }
      sub_679950(a1: v22);
    }
    v24 = -1;
    sub_445A87(a1: v22);
  }
  v11 = *(this + 20);
  while ( v11 != 0 )
  {
    v12 = v11;
    v13 = *(v11 != 0 ? v11 - 16 + 4 : 4);
    v14 = *(v11 + 4);
    v11 = v13 != 0 ? v13 + 16 : 0;
    v21 = v14;
    sub_6726BC(a1: v20);
    v15 = *v21;
    v24 = 1;
    (*v15)(a1: v21);
    if ( _ZtlSecureFuse<long>(a1: v21 + 960, a2: *(v21 + 968)) <= -1 )
    {
      sub_679A04(this: this + 17, a2: v12);
      v16 = *(v21 + 1148);
      while ( v16 != 0 )
      {
        v19 = *v16;
        v17 = *(v16 != 0 ? v16 - 3 : 4);
        v16 = v17 != 0 ? (v17 + 16) : 0;
        Mob = CMobPool::GetMob(this, a2: v19);
        if ( Mob != 0 )
          CMob::OnRevive(this: Mob);
      }
    }
    v24 = -1;
    sub_445A87(a1: v20);
  }
}