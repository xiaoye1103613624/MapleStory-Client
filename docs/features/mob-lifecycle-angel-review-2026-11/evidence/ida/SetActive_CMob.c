// Angel 083 `SetActive_CMob` @ 0x6637ec
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
int __thiscall CMob::SetActive(CMob *this, int a2)
{
  int v3; // esi
  int v4; // ebx
  int result; // eax
  int v6; // edi
  int Foothold; // eax
  int v8; // eax
  char *v9; // edx
  int v10; // eax
  bool v11; // zf
  int v12; // edi
  int v13; // eax
  int v14; // eax

  v3 = *(this + 70) != 0 ? *(this + 70) - 12 : 0;
  v4 = *(this + 71) != 0 ? *(this + 71) - 12 : 0;
  result = 0;
  if ( a2 == 0 )
  {
    if ( *(*(this + 71) != 0 ? *(this + 71) - 12 + 0x18 : 24) == 0 )
      return result;
    v8 = _ZtlSecureFuse<long>(a1: this + 304, a2: *(this + 78)) - 3;
    if ( v8 != 0 )
    {
      v9 = this + 304;
      if ( v8 != 1 )
      {
        v10 = _ZtlSecureTear<long>(a1: -1, a2: v9);
        v11 = *(this + 169) == 0;
        *(this + 78) = v10;
        if ( v11 || *(this + 99) == 0 )
          v12 = *(this + 98);
        else
          v12 = *(this + 99);
        v13 = _ZtlSecureFuse<long>(a1: v12 + 64, a2: *(v12 + 72));
        CMovePath::DiscardByInterrupt(this: (v3 + 428), a2: v13, a3: v3, a4: 0);
        return (*(*v4 + 4))(a1: v4, a2: 0, a3: 0, a4: 0, a5: 0, a6: 0, a7: 0, a8: 0);
      }
      v14 = _ZtlSecureTear<long>(a1: -3, a2: v9);
    }
    else
    {
      v14 = _ZtlSecureTear<long>(a1: -2, a2: this + 304);
    }
    *(this + 78) = v14;
    return (*(*v4 + 4))(a1: v4, a2: 0, a3: 0, a4: 0, a5: 0, a6: 0, a7: 0, a8: 0);
  }
  if ( *(*(this + 71) != 0 ? *(this + 71) - 12 + 0x18 : 24) == 0 )
  {
    *(this + 78) = _ZtlSecureTear<long>(a1: 3, a2: this + 304);
    v6 = *v4;
    Foothold = CWvsPhysicalSpace2D::GetFoothold(dwSN: *(v3 + 508));
    return (*(v6 + 4))(
             a1: v4,
             a2: 1,
             a3: *(v3 + 498),
             a4: *(v3 + 500),
             a5: *(v3 + 502),
             a6: *(v3 + 504),
             a7: *(v3 + 506),
             a8: Foothold);
  }
  return result;
}