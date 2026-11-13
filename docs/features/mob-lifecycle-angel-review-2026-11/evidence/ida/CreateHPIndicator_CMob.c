// Angel 083 `CreateHPIndicator_CMob` @ 0x66fded
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
int __thiscall CMob::CreateHPIndicator(CMob *this, int a2, int a3)
{
  CMob *v3; // edi
  int **v4; // ebx
  bool v5; // zf
  int **v6; // eax
  int v7; // eax
  int v8; // eax
  int v9; // ecx
  int v10; // eax
  int v11; // eax
  int v12; // ecx
  int v13; // eax
  int v14; // eax
  int *v15; // esi
  int v16; // eax
  int result; // eax
  void **v18; // ebx
  int v19; // ecx
  int v20; // eax
  int v21; // eax
  int v22; // eax
  void *v23; // eax
  void *v24; // eax
  void *v25; // eax
  VARIANTARG v26; // [esp+Ch] [ebp-54h] BYREF
  VARIANTARG v27; // [esp+1Ch] [ebp-44h] BYREF
  VARIANTARG pvargDest; // [esp+2Ch] [ebp-34h] BYREF
  VARIANTARG pvarg; // [esp+3Ch] [ebp-24h] BYREF
  int *v30; // [esp+4Ch] [ebp-14h] BYREF
  CMob *v31; // [esp+50h] [ebp-10h]
  int v32; // [esp+5Ch] [ebp-4h]
  int v33; // [esp+68h] [ebp+8h]

  v3 = this;
  v4 = (this + 1224);
  v5 = *(this + 306) == 0;
  v31 = this;
  if ( !v5 )
    goto LABEL_25;
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc);
  v32 = 0;
  sub_402FAB(a1: 0, a2: 3);
  LOBYTE(v32) = 1;
  if ( dword_BF14EC == 0 )
    _com_issue_error(a1: -2147467261);
  v6 = IWzGr2D::CreateLayer(
         this: dword_BF14EC,
         a2: &v30,
         nLeft: 0,
         nTop: 0,
         uWidth: 52,
         uHeight: 10,
         z: 0,
         vCanvas: &pvarg,
         dwFilter: &pvargDest);
  sub_428712(a1: *v6);
  sub_4156C5(a1: &v30);
  LOBYTE(v32) = 0;
  v7 = ZComAPI::ZComVariantCopy(&pvarg);
  if ( v7 < 0 )
    _com_issue_error(a1: v7);
  v32 = -1;
  v8 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest);
  if ( v8 < 0 )
    _com_issue_error(a1: v8);
  sub_410FDF(this: &pvargDest, a2: *(v3 + 304), a3: 1);
  v5 = *v4 == 0;
  v32 = 2;
  if ( v5 )
    _com_issue_error(a1: -2147467261);
  v9 = **v4;
  v30 = *v4;
  v10 = (*(v9 + 252))(
          a1: v30,
          a2: *&pvargDest.vt,
          a3: pvargDest.decVal.Hi32,
          a4: pvargDest.lVal,
          a5: pvargDest.cyVal.Hi);
  if ( v10 < 0 )
    _com_issue_errorex(a1: v10, a2: v30, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf);
  v32 = -1;
  v11 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest);
  if ( v11 < 0 )
    _com_issue_error(a1: v11);
  sub_410FDF(this: &pvargDest, a2: *(v31 + 70), a3: 1);
  v5 = *v4 == 0;
  v32 = 3;
  if ( v5 )
    _com_issue_error(a1: -2147467261);
  v12 = **v4;
  v30 = *v4;
  v13 = (*(v12 + 100))(
          a1: v30,
          a2: *&pvargDest.vt,
          a3: pvargDest.decVal.Hi32,
          a4: pvargDest.lVal,
          a5: pvargDest.cyVal.Hi);
  if ( v13 < 0 )
    _com_issue_errorex(a1: v13, a2: v30, a3: dword_BD8348);
  v32 = -1;
  v14 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest);
  if ( v14 < 0 )
    _com_issue_error(a1: v14);
  if ( *v4 == 0 )
    _com_issue_error(a1: -2147467261);
  v15 = *v4;
  v16 = (*(**v4 + 224))(a1: *v4, a2: -1);
  if ( v16 < 0 )
    _com_issue_errorex(a1: v16, a2: v15, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf);
  result = sub_66FC9F(a1: v31);
  v3 = v31;
  if ( *v4 != 0 )
  {
LABEL_25:
    v33 = 46 * a2 / 100;
    sub_402F85(pvargDest: &v26, pvargSrc: &pvargSrc);
    v32 = 4;
    sub_402F85(pvargDest: &v27, pvargSrc: &pvargSrc);
    v5 = *(v3 + 312) == 0;
    v18 = (v3 + 1248);
    LOBYTE(v32) = 5;
    if ( v5 )
      _com_issue_error(a1: -2147467261);
    v19 = **v18;
    v30 = *v18;
    v20 = (*(v19 + 44))(
            a1: v30,
            a2: 52,
            a3: 10,
            a4: *&v27.vt,
            a5: v27.decVal.Hi32,
            a6: v27.lVal,
            a7: v27.cyVal.Hi,
            a8: *&v26.vt,
            a9: v26.decVal.Hi32,
            a10: v26.lVal,
            a11: v26.cyVal.Hi);
    if ( v20 < 0 )
      _com_issue_errorex(a1: v20, a2: v30, a3: _GUID_7600dc6c_9328_4bff_9624_5b0f5c01179e);
    LOBYTE(v32) = 4;
    v21 = ZComAPI::ZComVariantCopy(pvarg: &v27);
    if ( v21 < 0 )
      _com_issue_error(a1: v21);
    v32 = -1;
    v22 = ZComAPI::ZComVariantCopy(pvarg: &v26);
    if ( v22 < 0 )
      _com_issue_error(a1: v22);
    if ( *v18 == 0 )
      _com_issue_error(a1: -2147467261);
    sub_55BDA5(this: *v18, a2: 0, a3: 0, a4: 52, a5: 10, a6: -16777216);
    if ( *v18 == 0 )
      _com_issue_error(a1: -2147467261);
    sub_55BDA5(this: *v18, a2: 1, a3: 1, a4: 50, a5: 8, a6: -1);
    v23 = sub_414576(a1: v18);
    sub_55BDA5(this: v23, a2: 2, a3: 2, a4: 48, a5: 6, a6: -16777216);
    v24 = sub_414576(a1: v18);
    sub_55BDA5(this: v24, a2: 3, a3: 3, a4: v33, a5: 4, a6: a3);
    v25 = sub_414576(a1: v18);
    return sub_55BDA5(this: v25, a2: 3, a3: 6, a4: v33, a5: 1, a6: -6290688);
  }
  return result;
}