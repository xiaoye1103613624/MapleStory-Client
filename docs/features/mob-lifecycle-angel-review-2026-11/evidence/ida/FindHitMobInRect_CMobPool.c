// Angel 083 `FindHitMobInRect_CMobPool` @ 0x678476
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
int __userpurge CMobPool::FindHitMobInRect@<eax>(
        CMobPool *this@<ecx>,
        const struct tagRECT *a2,
        struct CMob **const a3,
        int a4,
        struct CMob *a5,
        int a6,
        int a7,
        unsigned int a8,
        int a9,
        int a10)
{
  int v10; // ebx
  struct CMob *v11; // esi
  int v12; // edx
  int v13; // edi
  struct CMob **v14; // eax
  _DWORD v16[4]; // [esp+Ch] [ebp-28h] BYREF
  _DWORD v17[4]; // [esp+1Ch] [ebp-18h] BYREF
  int v18; // [esp+2Ch] [ebp-8h]
  int v19; // [esp+30h] [ebp-4h]

  v10 = *(this + 10);
  v19 = 0;
  v18 = 0;
  while ( v10 != 0 )
  {
    if ( v19 >= a4 )
      break;
    v11 = *(v10 + 4);
    v12 = *(v10 != 0 ? v10 - 16 + 4 : 4);
    v10 = v12 != 0 ? v12 + 16 : 0;
    if ( timeGetTime() != 0
      && timeGetTime() == 0
      && (a5 == 0
       || (v13 = _ZtlSecureFuse<unsigned long>(a1: v11 + 380, a2: *(v11 + 97))) != _ZtlSecureFuse<unsigned long>(
                                                                                     a1: a5 + 380,
                                                                                     a2: *(a5 + 97)))
      && (a6 == 0 || _ZtlSecureFuse<unsigned long>(a1: v11 + 380, a2: *(v11 + 97)) == a6)
      && (a8 == 0 || a8 == _ZtlSecureFuse<unsigned long>(a1: *(v11 + 98) + 12, a2: *(*(v11 + 98) + 20)))
      && *(*(v11 + 98) + 520) == 0
      && sub_670C06(a1: v11) == 0
      && (a9 != 0 || *(v11 + 202) == 0) )
    {
      sub_664559(this: v11, a2: v17, a3: 1);
      if ( dword_BF04AC(a1: v17) == 0 && dword_BF04A8(a1: v16, a2: v17, a3: a2) != 0 )
      {
        v14 = a3;
        ++v19;
        ++a3;
        *v14 = v11;
        if ( a7 != 0 && *(v11 + 148) == a7 )
          ++v18;
      }
    }
  }
  return v19 | (v18 << 16);
}