// Angel 083 `ShowHitEffect_CMob` @ 0x66fb4d
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
int __thiscall CMob::ShowHitEffect(CMob *this, int update_time)
{
  char *v3; // ebx
  int result; // eax
  int v5; // eax
  int v6; // edx
  unsigned int v7; // edi
  int v8; // edi
  int v9; // eax
  int v10; // eax
  int v11; // eax

  v3 = this + 960;
  result = _ZtlSecureFuse<long>(a1: this + 960, a2: *(this + 242));
  if ( result < 12 || result > 20 )
  {
    result = _ZtlSecureFuse<long>(a1: v3, a2: *(this + 242));
    if ( result < 21 || result > 37 )
    {
      result = _ZtlSecureFuse<long>(a1: v3, a2: *(this + 242));
      if ( result < 6 || result > 8 )
      {
        if ( *(this + 169) == 0 || (v5 = *(this + 99)) == 0 )
          v5 = *(this + 98);
        result = _ZtlSecureFuse<long>(a1: v5 + 268, a2: *(v5 + 276));
        if ( update_time >= result )
        {
          if ( sub_66918F(a1: this, a2: v6) == 0
            || (update_time = get_update_time(),
                result = _ZtlSecureFuse<long>(a1: this + 984, a2: *(this + 248)) + 10000,
                update_time > result) )
          {
            v7 = sub_668AAD(a1: this);
            *(this + 242) = _ZtlSecureTear<long>(a1: v7, a2: v3);
            CMob::PrepareActionLayer(this);
            v8 = update_time + CMob::GetActionDelay(this, a2: v7);
            v9 = _ZtlSecureTear<long>(a1: v8, a2: this + 972);
            *(this + 245) = v9;
            if ( v8 == 0 )
            {
              v10 = _ZtlSecureFuse<long>(a1: this + 972, a2: v9);
              *(this + 245) = _ZtlSecureTear<long>(a1: v10 + 1, a2: this + 972);
            }
            v11 = _ZtlSecureFuse<long>(a1: this + 972, a2: *(this + 245));
            result = _ZtlSecureTear<long>(a1: v11, a2: this + 984);
            *(this + 248) = result;
          }
        }
      }
    }
  }
  return result;
}