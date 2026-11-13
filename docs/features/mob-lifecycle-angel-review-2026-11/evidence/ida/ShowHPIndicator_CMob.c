// Angel 083 `ShowHPIndicator_CMob` @ 0x6635ed
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
void *__thiscall CMob::ShowHPIndicator(CMob *this)
{
  void *result; // eax
  int v3; // eax
  int v4; // eax
  bool v5; // zf
  int v6; // esi
  int v7; // eax
  int v8; // eax
  int v9; // eax
  int v10; // eax
  int v11; // eax
  VARIANTARG v12; // [esp+4h] [ebp-80h] BYREF
  VARIANTARG pvargDest; // [esp+14h] [ebp-70h] BYREF
  VARIANTARG v14; // [esp+24h] [ebp-60h] BYREF
  VARIANTARG v15; // [esp+34h] [ebp-50h] BYREF
  VARIANTARG v16; // [esp+44h] [ebp-40h] BYREF
  VARIANTARG v17; // [esp+54h] [ebp-30h] BYREF
  VARIANTARG pvarg; // [esp+64h] [ebp-20h] BYREF
  int v19; // [esp+74h] [ebp-10h] BYREF
  int v20; // [esp+80h] [ebp-4h]

  result = &loc_AA1390;
  if ( *(this + 306) != 0 )
  {
    sub_402FAB(a1: -2, a2: 3);
    v20 = 0;
    if ( *(this + 306) == 0 )
      _com_issue_error(a1: -2147467261);
    IWzGr2DLayer::RemoveCanvas(this: *(this + 306), a2: &v19, a3: &v17);
    if ( v19 != 0 )
      (*(*v19 + 8))(a1: v19);
    v20 = -1;
    v3 = ZComAPI::ZComVariantCopy(pvarg: &v17);
    if ( v3 < 0 )
      _com_issue_error(a1: v3);
    VariantInit(&pvarg);
    v4 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);
    if ( v4 < 0 )
      _com_issue_error(a1: v4);
    v20 = 1;
    sub_402F85(&pvargDest, pvargSrc: &pvargSrc);
    LOBYTE(v20) = 2;
    sub_402F85(pvargDest: &v14, pvargSrc: &pvargSrc);
    LOBYTE(v20) = 3;
    sub_402F85(pvargDest: &v15, pvargSrc: &pvargSrc);
    LOBYTE(v20) = 4;
    sub_402F85(pvargDest: &v16, pvargSrc: &pvargSrc);
    v5 = *(this + 306) == 0;
    v6 = *(this + 312);
    LOBYTE(v20) = 5;
    if ( v5 )
      _com_issue_error(a1: -2147467261);
    IWzGr2DLayer::Animate(pvargDest: &v12, a2: v6, a3: &v16, a4: &v15, a5: &v14, a6: &pvargDest, a7: &pvarg);
    v7 = ZComAPI::ZComVariantCopy(pvarg: &v12);
    if ( v7 < 0 )
      _com_issue_error(a1: v7);
    LOBYTE(v20) = 4;
    v8 = ZComAPI::ZComVariantCopy(pvarg: &v16);
    if ( v8 < 0 )
      _com_issue_error(a1: v8);
    LOBYTE(v20) = 3;
    v9 = ZComAPI::ZComVariantCopy(pvarg: &v15);
    if ( v9 < 0 )
      _com_issue_error(a1: v9);
    LOBYTE(v20) = 2;
    v10 = ZComAPI::ZComVariantCopy(pvarg: &v14);
    if ( v10 < 0 )
      _com_issue_error(a1: v10);
    LOBYTE(v20) = 1;
    v11 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest);
    if ( v11 < 0 )
      _com_issue_error(a1: v11);
    v20 = -1;
    result = ZComAPI::ZComVariantCopy(&pvarg);
    if ( result < 0 )
      _com_issue_error(a1: result);
  }
  return result;
}