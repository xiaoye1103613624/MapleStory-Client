// Angel 083 `ProcessAction_CMob` @ 0x666205
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
int *__thiscall CMob::ProcessAction(int **this, int a2)
{
  int CurrentAction; // esi
  int *result; // eax
  bool v5; // cc
  int *v6; // edi
  int v7; // ecx
  int **v8; // esi
  int v9; // ecx
  int v10; // eax
  int v11; // eax
  int v12; // [esp-4h] [ebp-34h]
  VARIANTARG pvarg; // [esp+Ch] [ebp-24h] BYREF
  int *v14; // [esp+1Ch] [ebp-14h]
  int *v15; // [esp+20h] [ebp-10h]
  int v16; // [esp+2Ch] [ebp-4h]

  CurrentAction = CMob::GetCurrentAction(this, a2: 0);
  v15 = (_ZtlSecureFuse<long>(a1: this + 250, a2: *(this + 252)) - 30);
  result = _ZtlSecureTear<long>(a1: v15, a2: this + 250);
  v5 = v15 <= 0;
  *(this + 252) = result;
  if ( v5 )
  {
    v6 = &(*(this + 269))[5 * CurrentAction];
    v7 = *(*(this + 249) != 0 ? *(this + 249) - 3 : 4);
    *(this + 249) = v7 != 0 ? (v7 + 16) : 0;
    v12 = *(this + 242);
    v14 = v6;
    if ( _ZtlSecureFuse<long>(a1: this + 240, a2: v12) <= -1 )
    {
      if ( *(this + 249) == 0 )
        *(this + 249) = v6[3];
    }
    else if ( *(this + 249) == 0 )
    {
      *(this + 242) = _ZtlSecureTear<long>(a1: -1, a2: this + 240);
      return CMob::PrepareActionLayer(this);
    }
    sub_402FAB(a1: 1, a2: 3);
    v16 = 0;
    v8 = this + 304;
    if ( *(this + 304) == 0 )
      _com_issue_error(a1: -2147467261);
    v9 = **v8;
    v15 = *v8;
    v10 = (*(v9 + 268))(a1: v15, a2: *&pvarg.vt, a3: pvarg.decVal.Hi32, a4: pvarg.lVal, a5: pvarg.cyVal.Hi);
    if ( v10 < 0 )
      _com_issue_errorex(a1: v10, a2: v15, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf);
    v16 = -1;
    v11 = ZComAPI::ZComVariantCopy(&pvarg);
    if ( v11 < 0 )
      _com_issue_error(a1: v11);
    return CMob::SetFrameInfo(this, a2: v14, a3: *(this + 249));
  }
  return result;
}