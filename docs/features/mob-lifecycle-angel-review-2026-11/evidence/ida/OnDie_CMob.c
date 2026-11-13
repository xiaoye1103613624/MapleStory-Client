// Angel 083 `OnDie_CMob` @ 0x663995
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
char *__thiscall CMob::OnDie(CMob *this)
{
  int v2; // eax
  int *v3; // eax
  int v4; // eax
  int v5; // eax
  int v6; // esi
  char *v7; // edx
  int v8; // ecx
  bool v9; // zf
  int *v10; // eax
  int v11; // edx
  int v12; // eax
  int v13; // eax
  int v14; // esi
  int v15; // eax
  _DWORD *v16; // eax
  _DWORD *v17; // esi
  int v18; // ecx
  _DWORD *Instance; // eax
  int v20; // eax
  int **Alpha; // eax
  int v22; // ecx
  int v23; // eax
  int v24; // eax
  int v25; // eax
  int v26; // ecx
  int v27; // eax
  int v28; // eax
  int v29; // eax
  char *result; // eax
  int v31; // [esp+14h] [ebp-64h]
  VARIANTARG pvargDest; // [esp+24h] [ebp-54h] BYREF
  VARIANTARG v33; // [esp+34h] [ebp-44h] BYREF
  VARIANTARG pvarg; // [esp+44h] [ebp-34h] BYREF
  _DWORD *v35; // [esp+54h] [ebp-24h]
  char v36[4]; // [esp+58h] [ebp-20h] BYREF
  int *v37; // [esp+5Ch] [ebp-1Ch]
  int v38; // [esp+60h] [ebp-18h] BYREF
  char v39[4]; // [esp+64h] [ebp-14h]
  _DWORD *v40; // [esp+68h] [ebp-10h] BYREF
  int v41; // [esp+74h] [ebp-4h]

  if ( *(this + 169) == 0 || (v2 = *(this + 99)) == 0 )
    v2 = *(this + 98);
  *v39 = _ZtlSecureFuse<unsigned long>(a1: v2 + 12, a2: *(v2 + 20));
  v31 = *((*(*(this + 1) + 16))(a1: this + 4) + 4);
  v3 = (*(*(this + 1) + 16))(a1: this + 4);
  v4 = sub_9894F3(a1: *v3, a2: v31);
  sub_989BA1(a1: v39[0], a2: 1, a3: v4);
  if ( *(this + 169) == 0 || (v5 = *(this + 99)) == 0 )
    v5 = *(this + 98);
  v6 = *(v5 + 548);
  if ( v6 > 0 )
  {
    if ( _ZtlSecureFuse<long>(a1: this + 1092, a2: *(this + 275)) == 3 )
    {
      v7 = this + 960;
      v8 = 21;
    }
    else
    {
      v8 = rand() % v6 + 9;
      v7 = this + 960;
    }
    *(this + 242) = _ZtlSecureTear<long>(a1: v8, a2: v7);
    CMob::PrepareActionLayer(this);
    sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);
    v9 = *(this + 70) == 0;
    v41 = 0;
    if ( v9 )
      _com_issue_error(a1: -2147467261);
    v10 = *(this + 70);
    v11 = *v10;
    *v39 = v10;
    v12 = (*(v11 + 140))(
            a1: v10,
            a2: this + 1296,
            a3: this + 1300,
            a4: 0,
            a5: 0,
            a6: 0,
            a7: 0,
            a8: 0,
            a9: 0,
            a10: *&pvarg.vt,
            a11: pvarg.decVal.Hi32,
            a12: pvarg.lVal,
            a13: pvarg.cyVal.Hi);
    if ( v12 < 0 )
      _com_issue_errorex(a1: v12, a2: *v39, a3: dword_BD8348);
    v41 = -1;
    v13 = ZComAPI::ZComVariantCopy(&pvarg);
    if ( v13 < 0 )
      _com_issue_error(a1: v13);
    if ( *(this + 70) == 0 )
      _com_issue_error(a1: -2147467261);
    v14 = *(this + 70);
    v15 = (*(*v14 + 64))(a1: v14, a2: *(this + 324), a3: *(this + 325));
    if ( v15 < 0 )
      _com_issue_errorex(a1: v15, a2: v14, a3: dword_BD8328);
    *(this + 278) = _ZtlSecureTear<int>(t: 2, at: this + 276);
    CMob::TrySpeaking(this, a2: -1, a3: -1);
  }
  ZList<_com_ptr_t<_com_IIID<IWzGr2DLayer,&__s_GUID const _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf>>>::RemoveAll(a1: this + 236);
  ZList<_com_ptr_t<_com_IIID<IWzGr2DLayer,&__s_GUID const _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf>>>::RemoveAll(a1: this + 256);
  ZList<CMob::AFFECTEDSKILLENTRY>::RemoveAll(a1: this + 176);
  *(this + 279) = 0x7FFFFFFF;
  *(this + 280) = 0x7FFFFFFF;
  *(this + 281) = 0x7FFFFFFF;
  *(this + 282) = 0x7FFFFFFF;
  v16 = *(this + 52);
  if ( v16 != 0 )
  {
    while ( 1 )
    {
      v17 = v16;
      v18 = *(v16 != 0 ? v16 - 3 : 4);
      v9 = v16[7] == 0;
      v35 = v16;
      *v39 = v18 != 0 ? v18 + 16 : 0;
      if ( !v9 )
      {
        v40 = 0;
        v41 = 1;
        Instance = StringPool::GetInstance();
        v20 = *StringPool::GetStringW(this: Instance, a2: &v38, a3: SP_958_SHAPE2DVECTOR2D);
        LOBYTE(v41) = 2;
        PcCreateObject::IWzVector2D(a1: v20, a2: &v40, a3: 0);
        LOBYTE(v41) = 1;
        if ( v38 != 0 )
          ZXString<unsigned short>::_Release(lpAddend: (v38 - 12));
        sub_410FDF(this: &pvarg, a2: v40, a3: 1);
        v9 = v17[7] == 0;
        LOBYTE(v41) = 3;
        if ( v9 )
          _com_issue_error(a1: -2147467261);
        Alpha = IWzGr2DLayer::GetAlpha(a1: v36);
        v9 = *Alpha == 0;
        LOBYTE(v41) = 4;
        if ( v9 )
          _com_issue_error(a1: -2147467261);
        v22 = **Alpha;
        v37 = *Alpha;
        v23 = (*(v22 + 100))(a1: v37, a2: *&pvarg.vt, a3: pvarg.decVal.Hi32, a4: pvarg.lVal, a5: pvarg.cyVal.Hi);
        if ( v23 < 0 )
          _com_issue_errorex(a1: v23, a2: v37, a3: dword_BD8348);
        sub_41569E(a1: v36);
        LOBYTE(v41) = 1;
        v24 = ZComAPI::ZComVariantCopy(&pvarg);
        if ( v24 < 0 )
          _com_issue_error(a1: v24);
        sub_402F85(&pvargDest, pvargSrc: &pvargSrc);
        v9 = v35[7] == 0;
        LOBYTE(v41) = 5;
        if ( v9 )
          _com_issue_error(a1: -2147467261);
        v25 = sub_437476(a1: v35[7]);
        sub_402FAB(a1: v25 + 200, a2: 3);
        LOBYTE(v41) = 6;
        if ( v40 == 0 )
          _com_issue_error(a1: -2147467261);
        v26 = *v40;
        v35 = v40;
        v27 = (*(v26 + 144))(
                a1: v40,
                a2: -255,
                a3: 0,
                a4: *&v33.vt,
                a5: v33.decVal.Hi32,
                a6: v33.lVal,
                a7: v33.cyVal.Hi,
                a8: *&pvargDest.vt,
                a9: pvargDest.decVal.Hi32,
                a10: pvargDest.lVal,
                a11: pvargDest.cyVal.Hi);
        if ( v27 < 0 )
          _com_issue_errorex(a1: v27, a2: v35, a3: dword_BD8348);
        LOBYTE(v41) = 5;
        v28 = ZComAPI::ZComVariantCopy(pvarg: &v33);
        if ( v28 < 0 )
          _com_issue_error(a1: v28);
        LOBYTE(v41) = 1;
        v29 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest);
        if ( v29 < 0 )
          _com_issue_error(a1: v29);
        v41 = -1;
        sub_41569E(a1: &v40);
      }
      if ( *v39 == 0 )
        break;
      v16 = *v39;
    }
  }
  if ( _ZtlSecureFuse<unsigned long>(a1: *(this + 98) + 12, a2: *(*(this + 98) + 20)) == (&loc_8646FE + 4) )
    CAnimationDisplayer::Effect_Tremble(a1: dbl_AF46E0, a2: 0, a3: 0, a4: 0, a5: 0);
  if ( _ZtlSecureFuse<unsigned long>(a1: *(this + 98) + 12, a2: *(*(this + 98) + 20)) == &loc_866E2A )
    CAnimationDisplayer::Effect_Tremble(a1: dbl_AF82A0, a2: 0, a3: 0, a4: 4000, a5: 1);
  if ( _ZtlSecureFuse<unsigned long>(a1: *(this + 98) + 12, a2: *(*(this + 98) + 20)) == 8810018 )
    CAnimationDisplayer::Effect_Tremble(a1: dbl_AF8298, a2: 0, a3: 0, a4: 3000, a5: 1);
  result = _ZtlSecureFuse<unsigned long>(a1: *(this + 98) + 12, a2: *(*(this + 98) + 20));
  if ( result == &byte_86BC33 )
    return CAnimationDisplayer::Effect_Tremble(a1: dbl_AF82A0, a2: 0, a3: 0, a4: 3000, a5: 0);
  return result;
}