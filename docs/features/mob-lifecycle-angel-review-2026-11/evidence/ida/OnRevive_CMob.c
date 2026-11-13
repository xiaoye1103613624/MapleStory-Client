// Angel 083 `OnRevive_CMob` @ 0x664182
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
int __thiscall CMob::OnRevive(int *this)
{
  int v2; // eax
  int v3; // eax
  bool v4; // zf
  int *Alpha; // eax
  int v6; // ebx
  int v7; // eax
  int v8; // eax
  int result; // eax
  VARIANTARG pvarg; // [esp+Ch] [ebp-30h] BYREF
  VARIANTARG pvargDest; // [esp+1Ch] [ebp-20h] BYREF
  int v12; // [esp+2Ch] [ebp-10h] BYREF
  int v13; // [esp+38h] [ebp-4h]

  *(this + 278) = _ZtlSecureTear<int>(t: 0, at: this + 276);
  VariantInit(&pvarg);
  v2 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);
  if ( v2 < 0 )
    _com_issue_error(a1: v2);
  v13 = 0;
  VariantInit(pvarg: &pvargDest);
  v3 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);
  if ( v3 < 0 )
    _com_issue_error(a1: v3);
  v4 = *(this + 304) == 0;
  LOBYTE(v13) = 1;
  if ( v4 )
    _com_issue_error(a1: -2147467261);
  Alpha = IWzGr2DLayer::GetAlpha(a1: &v12);
  v4 = *Alpha == 0;
  LOBYTE(v13) = 2;
  if ( v4 )
    _com_issue_error(a1: -2147467261);
  v6 = *Alpha;
  v7 = (*(*v6 + 144))(
         a1: v6,
         a2: 255,
         a3: 255,
         a4: *&pvargDest.vt,
         a5: pvargDest.decVal.Hi32,
         a6: pvargDest.lVal,
         a7: pvargDest.cyVal.Hi,
         a8: *&pvarg.vt,
         a9: pvarg.decVal.Hi32,
         a10: pvarg.lVal,
         a11: pvarg.cyVal.Hi);
  if ( v7 < 0 )
    _com_issue_errorex(a1: v7, a2: v6, a3: dword_BD8348);
  LOBYTE(v13) = 1;
  if ( v12 != 0 )
    (*(*v12 + 8))(a1: v12);
  LOBYTE(v13) = 0;
  v8 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest);
  if ( v8 < 0 )
    _com_issue_error(a1: v8);
  v13 = -1;
  result = ZComAPI::ZComVariantCopy(&pvarg);
  if ( result < 0 )
    _com_issue_error(a1: result);
  return result;
}