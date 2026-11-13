// Angel 083 `FindBodyAttackMob_CMobPool` @ 0x678c77
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
_DWORD *__thiscall CMobPool::FindBodyAttackMob(CMobPool *this, const struct tagRECT *a2, int a3)
{
  int v3; // eax
  unsigned int v4; // ebx
  _DWORD *v5; // esi
  int v6; // edx
  int v7; // edi
  int v8; // eax
  int v9; // edi
  int v10; // ecx
  int v11; // edx
  int v12; // ebx
  int v13; // eax
  int v14; // eax
  int v15; // ecx
  _DWORD v17[4]; // [esp+Ch] [ebp-38h] BYREF
  int v18; // [esp+1Ch] [ebp-28h] BYREF
  int v19; // [esp+20h] [ebp-24h]
  int v20; // [esp+24h] [ebp-20h]
  int v21; // [esp+28h] [ebp-1Ch]
  BOOL v22; // [esp+2Ch] [ebp-18h]
  int v23; // [esp+30h] [ebp-14h]
  int v24; // [esp+34h] [ebp-10h] BYREF
  int v25; // [esp+40h] [ebp-4h]

  v3 = *(this + 10);
  v4 = 0;
  if ( v3 == 0 )
    return 0;
  while ( 1 )
  {
    v5 = *(v3 + 4);
    v6 = *(v3 != 0 ? v3 - 16 + 4 : 4);
    v23 = v6 != 0 ? v6 + 16 : 0;
    v22 = (*(get_field() + 324) & 2) != 0;
    if ( (v22 || a3 == 0 || _ZtlSecureFuse<unsigned long>(a1: v5 + 95, a2: v5[97]) == 9400594)
      && timeGetTime() != 0
      && _ZtlSecureFuse<long>(a1: v5 + 270, a2: v5[272]) == 0
      && timeGetTime() == 0
      && sub_667569(a1: v5) == 0
      && timeGetTime() != 0
      && *(v5[98] + 520) == 0
      && sub_670C06(a1: v5) == 0
      && v5[202] == 0
      && (v5 == -416 || v5[217] <= 0) )
    {
      break;
    }
LABEL_34:
    if ( v23 == 0 )
      return 0;
    v3 = v23;
  }
  v24 = 0;
  v25 = 0;
  sub_664699(this: v5, a2: &v24, a3: 1);
  v7 = 0;
  while ( v24 != 0 && v4 < *(v24 - 4) )
  {
    if ( dword_BF04AC(a1: v7 + v24) == 0 && dword_BF04A8(a1: v17, a2: v7 + v24, a3: a2) != 0 )
      goto LABEL_36;
    ++v4;
    v7 += 16;
  }
  sub_664559(this: v5, a2: &v18, a3: 1);
  if ( dword_BF04AC(a1: &v18) == 0 )
  {
    v8 = (v21 + v19) / 2;
    v9 = v21 - 10;
    v10 = (v20 + v18) / 2;
    if ( v8 + 1 >= v21 - 10 )
      v9 = v8 + 1;
    v11 = v10 + 1;
    if ( v10 + 1 < v20 - 25 )
      v11 = v20 - 25;
    v12 = v19 + 30;
    v13 = v8 - 1;
    if ( v19 + 30 >= v13 )
      v12 = v13;
    v14 = v18 + 25;
    v15 = v10 - 1;
    if ( v18 + 25 >= v15 )
      v14 = v15;
    dword_BF040C(a1: &v18, a2: v14, a3: v12, a4: v11, a5: v9);
    if ( dword_BF04A8(a1: v17, a2: &v18, a3: a2) != 0 )
    {
LABEL_36:
      v25 = -1;
      sub_53942D(a1: &v24);
      return v5;
    }
  }
  v25 = -1;
  sub_53942D(a1: &v24);
  v4 = 0;
  goto LABEL_34;
}