// Angel 083 `Init_CMob` @ 0x662884
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
int __thiscall CMob::Init(CMob *this, _DWORD *a2, struct CInPacket *a3)
{
  CInPacket *v4; // esi
  unsigned __int8 v5; // al
  struct CInPacket *v6; // eax
  int v7; // eax
  int v8; // eax
  int v9; // eax
  int v10; // eax
  int v11; // eax
  int v12; // eax
  int v13; // eax
  int FineMoveDirAction; // eax
  int v15; // eax
  _DWORD *v16; // eax
  int v17; // eax
  int v18; // eax
  int v19; // eax
  int **v20; // eax
  int v21; // eax
  int v22; // eax
  struct CInPacket *v23; // eax
  int v24; // ecx
  int v25; // eax
  int v26; // eax
  _DWORD *v27; // esi
  int v28; // edi
  int v29; // eax
  char v30; // di
  _DWORD *v31; // eax
  wchar_t *v32; // eax
  wchar_t *v33; // eax
  int v34; // ecx
  int v35; // eax
  int *v36; // eax
  int v37; // eax
  int v38; // eax
  int v39; // eax
  int v40; // eax
  int v41; // eax
  bool v42; // zf
  _DWORD *v43; // eax
  VARIANTARG *Item; // eax
  int int32; // eax
  unsigned int v46; // edi
  int v47; // eax
  struct CInPacket **v48; // eax
  struct CInPacket *v49; // eax
  int v50; // ecx
  int v51; // eax
  int v52; // eax
  VARIANTARG *v53; // eax
  int v54; // eax
  struct CInPacket **v55; // eax
  struct CInPacket *v56; // eax
  int v57; // ecx
  int v58; // eax
  int v59; // eax
  int v60; // eax
  int v61; // eax
  struct CInPacket **v62; // eax
  struct CInPacket *v63; // ecx
  int v64; // edx
  int v65; // eax
  int v66; // eax
  struct CMob *Mob; // edi
  int v68; // esi
  int v69; // eax
  struct CInPacket **Alpha; // eax
  struct CInPacket *v71; // eax
  int v72; // ecx
  int v73; // eax
  int v74; // eax
  int v75; // eax
  int v76; // eax
  char v77; // di
  int *v78; // eax
  int v79; // eax
  int v80; // esi
  int v81; // ecx
  _DWORD *v82; // eax
  int **v83; // eax
  int v84; // eax
  _DWORD *v85; // eax
  _DWORD *v86; // eax
  int v87; // ecx
  int v88; // eax
  int v89; // eax
  int v90; // eax
  int result; // eax
  double v92; // [esp+0h] [ebp-148h] BYREF
  __int64 v93; // [esp+8h] [ebp-140h]
  _QWORD v94[2]; // [esp+10h] [ebp-138h] BYREF
  VARIANTARG v95; // [esp+24h] [ebp-124h] BYREF
  VARIANTARG v96; // [esp+34h] [ebp-114h] BYREF
  VARIANTARG v97; // [esp+44h] [ebp-104h] BYREF
  VARIANTARG v98; // [esp+54h] [ebp-F4h] BYREF
  VARIANTARG v99; // [esp+64h] [ebp-E4h] BYREF
  VARIANTARG v100; // [esp+74h] [ebp-D4h] BYREF
  VARIANTARG v101; // [esp+84h] [ebp-C4h] BYREF
  VARIANTARG v102; // [esp+94h] [ebp-B4h] BYREF
  VARIANTARG v103; // [esp+A4h] [ebp-A4h] BYREF
  VARIANTARG v104; // [esp+B4h] [ebp-94h] BYREF
  VARIANTARG v105; // [esp+C4h] [ebp-84h] BYREF
  VARIANTARG v106; // [esp+D4h] [ebp-74h] BYREF
  VARIANTARG pvarg; // [esp+E4h] [ebp-64h] BYREF
  int Unknown; // [esp+F4h] [ebp-54h] BYREF
  _BYTE v109[4]; // [esp+F8h] [ebp-50h] BYREF
  _BYTE v110[4]; // [esp+FCh] [ebp-4Ch] BYREF
  _BYTE v111[4]; // [esp+100h] [ebp-48h] BYREF
  char *v112; // [esp+104h] [ebp-44h]
  VARIANTARG pvargDest; // [esp+108h] [ebp-40h] BYREF
  int v114; // [esp+118h] [ebp-30h]
  int v115; // [esp+11Ch] [ebp-2Ch] BYREF
  int v116; // [esp+120h] [ebp-28h] BYREF
  int update_time; // [esp+124h] [ebp-24h]
  unsigned int v118; // [esp+128h] [ebp-20h]
  int *Instance; // [esp+12Ch] [ebp-1Ch] BYREF
  char v120[4]; // [esp+130h] [ebp-18h] BYREF
  int Foothold; // [esp+134h] [ebp-14h] BYREF
  int v122; // [esp+138h] [ebp-10h] BYREF
  int v123; // [esp+144h] [ebp-4h]

  v115 = 0;
  update_time = get_update_time();
  *(this + 97) = sub_4E81A0(a1: a2, a2: this + 380);
  CMob::SetShoeAttr(this);
  v4 = a3;
  *(this + 326) = CInPacket::Decode2(this: a3);
  *(this + 327) = CInPacket::Decode2(this: v4);
  *(this + 324) = *(this + 326);
  *(this + 325) = *(this + 327);
  v5 = CInPacket::Decode1(this: v4);
  *(this + 239) = _ZtlSecureTear<long>(a1: v5, a2: this + 948);
  LOWORD(v6) = CInPacket::Decode2(this: v4);
  a3 = v6;
  LOWORD(v7) = CInPacket::Decode2(this: v4);
  v122 = v7;
  v8 = CInPacket::Decode1(this: v4);
  *v120 = v8;
  if ( v8 == -3 || v8 >= 0 )
    v118 = CInPacket::Decode4(this: v4);
  else
    v118 = 0;
  *(this + 236) = CInPacket::Decode1(this: v4);
  v114 = CInPacket::Decode4(this: v4);
  Foothold = CWvsPhysicalSpace2D::GetFoothold(dwSN: a3);
  v9 = CWvsPhysicalSpace2D::GetFoothold(dwSN: v122);
  if ( v9 != 0 )
    a3 = *(v9 + 32);
  else
    a3 = 0;
  Instance = CVecCtrlMob::CreateInstance();
  v10 = sub_675A13(a1: &Instance);
  if ( v10 < 0 && v10 != -2147467262 )
    _com_issue_error(a1: v10);
  if ( *(this + 169) == 0 || (v11 = *(this + 99)) == 0 )
    v11 = *(this + 98);
  v12 = _ZtlSecureFuse<long>(a1: v11 + 64, a2: *(v11 + 72));
  sub_9BBD19(a1: this != 0 ? this + 4 : 0, a2: v12, a3);
  v13 = _ZtlSecureFuse<long>(a1: this + 948, a2: *(this + 239));
  (*(*Instance + 4))(a1: Instance, a2: 0, a3: *(this + 324), a4: *(this + 325), a5: 0, a6: 0, a7: v13, a8: Foothold);
  FineMoveDirAction = CMob::GetFineMoveDirAction(this, nMoveDirAction: Instance[82]);
  *(this + 239) = _ZtlSecureTear<long>(a1: FineMoveDirAction, a2: this + 948);
  v116 = CVecCtrlMob::CreateInstance();
  v15 = sub_675A13(a1: &v116);
  if ( v15 < 0 && v15 != -2147467262 )
    _com_issue_error(a1: v15);
  v16 = StringPool::GetInstance();
  v17 = *StringPool::GetStringW(this: v16, a2: &Foothold, a3: SP_958_SHAPE2DVECTOR2D);
  v123 = 0;
  PcCreateObject::IWzVector2D(a1: v17, a2: this + 288, a3: 0);
  v123 = -1;
  if ( Foothold != 0 )
    ZXString<unsigned short>::_Release(lpAddend: (Foothold - 12));
  if ( *(this + 169) == 0 || (v18 = *(this + 99)) == 0 )
    v18 = *(this + 98);
  v19 = _ZtlSecureFuse<long>(a1: v18 + 64, a2: *(v18 + 72));
  sub_9BBD19(a1: this != 0 ? this + 4 : 0, a2: v19, a3);
  (*(*v116 + 4))(a1: v116, a2: 0, a3: 0, a4: 0, a5: 0, a6: 0, a7: 0, a8: 0);
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc);
  v123 = 1;
  sub_402FAB(a1: 0, a2: 3);
  LOBYTE(v123) = 2;
  if ( dword_BF14EC == 0 )
    _com_issue_error(a1: -2147467261);
  v20 = IWzGr2D::CreateLayer(
          this: dword_BF14EC,
          a2: &a3,
          nLeft: 0,
          nTop: 0,
          uWidth: 0,
          uHeight: 0,
          z: 0,
          vCanvas: &pvarg,
          dwFilter: &pvargDest);
  sub_428712(a1: *v20);
  sub_4156C5(a1: &a3);
  LOBYTE(v123) = 1;
  v21 = ZComAPI::ZComVariantCopy(&pvarg);
  if ( v21 < 0 )
    _com_issue_error(a1: v21);
  v123 = -1;
  v22 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest);
  if ( v22 < 0 )
    _com_issue_error(a1: v22);
  sub_410FDF(this: &pvargDest, a2: *(this + 70), a3: 1);
  v42 = *(this + 304) == 0;
  v123 = 3;
  if ( v42 )
    _com_issue_error(a1: -2147467261);
  v23 = *(this + 304);
  v24 = *v23;
  a3 = v23;
  v25 = (*(v24 + 100))(
          a1: v23,
          a2: *&pvargDest.vt,
          a3: pvargDest.decVal.Hi32,
          a4: pvargDest.lVal,
          a5: pvargDest.cyVal.Hi);
  if ( v25 < 0 )
    _com_issue_errorex(a1: v25, a2: a3, a3: dword_BD8348);
  v123 = -1;
  v26 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest);
  if ( v26 < 0 )
    _com_issue_error(a1: v26);
  v27 = (this + 1216);
  if ( *(this + 304) == 0 )
    _com_issue_error(a1: -2147467261);
  v28 = *v27;
  v29 = (*(**v27 + 224))(a1: *v27, a2: 0xFFFFFF);
  if ( v29 < 0 )
    _com_issue_errorex(a1: v29, a2: v28, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf);
  v30 = v120[0];
  if ( *v120 == -4 )
  {
    v69 = *(this + 98);
    HIDWORD(v94[0]) = *(v69 + 84);
    LODWORD(v94[0]) = v69 + 76;
    if ( timeGetTime() != 0 )
    {
      HIDWORD(v94[0]) = &pvargSrc;
      *(this + 242) = _ZtlSecureTear<long>(a1: 4, a2: this + 960);
      sub_402F85(pvargDest: &v103, pvargSrc: HIDWORD(v94[0]));
      v123 = 13;
      sub_402F85(pvargDest: &v105, pvargSrc: &pvargSrc);
      v42 = *v27 == 0;
      LOBYTE(v123) = 14;
      if ( v42 )
        _com_issue_error(a1: -2147467261);
      Alpha = IWzGr2DLayer::GetAlpha(a1: &v115);
      v42 = *Alpha == 0;
      LOBYTE(v123) = 15;
      if ( v42 )
        _com_issue_error(a1: -2147467261);
      v71 = *Alpha;
      v93 = *&v103.vt;
      v72 = *v71;
      a3 = v71;
      v73 = (*(v72 + 144))(
              a1: v71,
              a2: 255,
              a3: 0,
              a4: *&v105.vt,
              a5: v105.decVal.Hi32,
              a6: v105.lVal,
              a7: v105.cyVal.Hi,
              a8: *&v103.vt,
              a9: v103.decVal.Hi32,
              a10: v103.lVal,
              a11: v103.cyVal.Hi);
      if ( v73 < 0 )
        _com_issue_errorex(a1: v73, a2: a3, a3: dword_BD8348);
      sub_41569E(a1: &v115);
      LOBYTE(v123) = 13;
      v74 = ZComAPI::ZComVariantCopy(pvarg: &v105);
      if ( v74 < 0 )
        _com_issue_error(a1: v74);
      v123 = -1;
      v75 = ZComAPI::ZComVariantCopy(pvarg: &v103);
      if ( v75 < 0 )
        _com_issue_error(a1: v75);
      if ( *(this + 169) == 0 || (v76 = *(this + 99)) == 0 )
        v76 = *(this + 98);
      v77 = _ZtlSecureFuse<unsigned long>(a1: v76 + 12, a2: *(v76 + 20));
      HIDWORD(v94[0]) = *((*(*(this + 1) + 16))(a1: this + 4) + 4);
      v78 = (*(*(this + 1) + 16))(a1: this + 4);
      v79 = sub_9894F3(a1: *v78, a2: SHIDWORD(v94[0]));
      sub_989BA1(a1: v77, a2: 0x26, a3: v79);
    }
    *(this + 272) = _ZtlSecureTear<long>(a1: 0, a2: this + 1080);
    *(this + 278) = _ZtlSecureTear<int>(t: 1, at: this + 276);
    if ( _ZtlSecureFuse<unsigned long>(a1: *(this + 98) + 12, a2: *(*(this + 98) + 20)) == (&loc_8646FE + 2) )
      CAnimationDisplayer::Effect_Tremble(a1: dbl_AF46E0, a2: 0, a3: 0, a4: 0, a5: 0);
  }
  else if ( *v120 == -3 )
  {
    Mob = CMobPool::GetMob(this: dword_BEBFA4, a2: v118);
    if ( Mob != 0 )
    {
      v68 = _ZtlSecureFuse<unsigned long>(a1: this + 380, a2: *(this + 97));
      *sub_5DD293(a1: Mob + 1136) = v68;
      *(this + 278) = _ZtlSecureTear<int>(t: 1, at: this + 276);
    }
  }
  else
  {
    if ( *v120 == -2 )
    {
      v54 = *(this + 98);
      HIDWORD(v94[0]) = *(v54 + 84);
      LODWORD(v94[0]) = v54 + 76;
      if ( timeGetTime() != 0 )
      {
        HIDWORD(v94[0]) = &pvargSrc;
        *(this + 242) = _ZtlSecureTear<long>(a1: 4, a2: this + 960);
        sub_402F85(pvargDest: &v106, pvargSrc: HIDWORD(v94[0]));
        v123 = 7;
        sub_402F85(pvargDest: &v97, pvargSrc: &pvargSrc);
        v42 = *v27 == 0;
        LOBYTE(v123) = 8;
        if ( v42 )
          _com_issue_error(a1: -2147467261);
        v55 = IWzGr2DLayer::GetAlpha(a1: v109);
        v42 = *v55 == 0;
        LOBYTE(v123) = 9;
        if ( v42 )
          _com_issue_error(a1: -2147467261);
        v56 = *v55;
        v93 = *&v106.vt;
        v57 = *v56;
        a3 = v56;
        v58 = (*(v57 + 144))(
                a1: v56,
                a2: 255,
                a3: 0,
                a4: *&v97.vt,
                a5: v97.decVal.Hi32,
                a6: v97.lVal,
                a7: v97.cyVal.Hi,
                a8: *&v106.vt,
                a9: v106.decVal.Hi32,
                a10: v106.lVal,
                a11: v106.cyVal.Hi);
        if ( v58 < 0 )
          _com_issue_errorex(a1: v58, a2: a3, a3: dword_BD8348);
        sub_41569E(a1: v109);
        LOBYTE(v123) = 7;
        v59 = ZComAPI::ZComVariantCopy(pvarg: &v97);
        if ( v59 < 0 )
          _com_issue_error(a1: v59);
        v123 = -1;
        v53 = &v106;
      }
      else
      {
        HIDWORD(v94[0]) = _ZtlSecureTear<long>(a1: update_time + 800, a2: this + 1080);
        LODWORD(v94[0]) = this + 1080;
        *(this + 272) = HIDWORD(v94[0]);
        if ( _ZtlSecureFuse<long>(a1: v94[0], a2: HIDWORD(v94[0])) == 0 )
          *(this + 272) = _ZtlSecureTear<long>(a1: 1, a2: this + 1080);
        sub_402F85(pvargDest: &v100, pvargSrc: &pvargSrc);
        HIDWORD(v94[0]) = *(this + 272);
        v123 = 10;
        v61 = _ZtlSecureFuse<long>(a1: this + 1080, a2: HIDWORD(v94[0]));
        sub_402FAB(a1: v61, a2: 3);
        v42 = *v27 == 0;
        LOBYTE(v123) = 11;
        if ( v42 )
          _com_issue_error(a1: -2147467261);
        v62 = IWzGr2DLayer::GetAlpha(a1: v110);
        v42 = *v62 == 0;
        LOBYTE(v123) = 12;
        if ( v42 )
          _com_issue_error(a1: -2147467261);
        v63 = *v62;
        v93 = *&v100.vt;
        v64 = *v63;
        a3 = v63;
        v65 = (*(v64 + 144))(
                a1: v63,
                a2: 255,
                a3: 255,
                a4: *&v98.vt,
                a5: v98.decVal.Hi32,
                a6: v98.lVal,
                a7: v98.cyVal.Hi,
                a8: *&v100.vt,
                a9: v100.decVal.Hi32,
                a10: v100.lVal,
                a11: v100.cyVal.Hi);
        if ( v65 < 0 )
          _com_issue_errorex(a1: v65, a2: a3, a3: dword_BD8348);
        sub_41569E(a1: v110);
        LOBYTE(v123) = 10;
        v66 = ZComAPI::ZComVariantCopy(pvarg: &v98);
        if ( v66 < 0 )
          _com_issue_error(a1: v66);
        v123 = -1;
        v53 = &v100;
      }
    }
    else
    {
      if ( *v120 != -1 )
      {
        v122 = 0;
        v123 = 16;
        v31 = StringPool::GetInstance();
        v32 = *StringPool::GetStringW(this: v31, a2: &Foothold, a3: SP_1590_EFFECT_SUMMONIMG_D);
        LOBYTE(v123) = 17;
        sub_416821(a1: &v122, a2: v32, a3: v30);
        LOBYTE(v123) = 16;
        if ( Foothold != 0 )
          ZXString<unsigned short>::_Release(lpAddend: (Foothold - 12));
        sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);
        LOBYTE(v123) = 18;
        sub_402F85(&pvargDest, pvargSrc: &pvargSrc);
        v94[0] = 0;
        HIDWORD(v93) = &pvarg;
        LODWORD(v93) = &pvargDest;
        LOBYTE(v123) = 19;
        v33 = sub_41468C(a1: &v122);
        HIDWORD(v92) = v34;
        v112 = &v92 + 4;
        _bstr_t::_bstr_t(a1: v33);
        LOBYTE(v123) = 19;
        v35 = sub_406472();
        v36 = sub_402680(a1: v35);
        IWzResMan::GetObjectA(this: v36, pvargDest: &v96, sUOL: SHIDWORD(v92), vParam: v93, vAux: SHIDWORD(v93));
        LOBYTE(v123) = 21;
        Unknown = Ztl_variant_t::GetUnknown(a1: v94[0], a2: SBYTE4(v94[0]));
        a3 = 0;
        if ( Unknown != 0 )
        {
          v37 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);
          if ( v37 < 0 && v37 != -2147467262 )
            _com_issue_error(a1: v37);
        }
        LOBYTE(v123) = 25;
        v38 = ZComAPI::ZComVariantCopy(pvarg: &v96);
        if ( v38 < 0 )
          _com_issue_error(a1: v38);
        LOBYTE(v123) = 24;
        v39 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest);
        if ( v39 < 0 )
          _com_issue_error(a1: v39);
        LOBYTE(v123) = 23;
        v40 = ZComAPI::ZComVariantCopy(&pvarg);
        if ( v40 < 0 )
          _com_issue_error(a1: v40);
        if ( a3 != 0 )
        {
          v94[0] = HIDWORD(v94[0]);
          v112 = v94;
          HIDWORD(v93) = 5624;
          LODWORD(v93) = v94;
          v43 = StringPool::GetInstance();
          StringPool::GetBSTR(this: v43, a2: v93, a3: SHIDWORD(v93));
          HIDWORD(v93) = &v95;
          LOBYTE(v123) = 23;
          sub_40263B(a1: &a3);
          Item = IWzProperty::GetItem(pvargDest: HIDWORD(v93), a2: v94[0]);
          LOBYTE(v123) = 27;
          int32 = get_int32(pvarSrc: Item, a2: SHIDWORD(v94[0]));
          v46 = v118;
          *(this + 272) = _ZtlSecureTear<long>(a1: update_time + v118 + int32, a2: this + 1080);
          LOBYTE(v123) = 23;
          v47 = ZComAPI::ZComVariantCopy(pvarg: &v95);
          if ( v47 < 0 )
            _com_issue_error(a1: v47);
          if ( _ZtlSecureFuse<long>(a1: this + 1080, a2: *(this + 272)) == 0 )
            *(this + 272) = _ZtlSecureTear<long>(a1: 1, a2: this + 1080);
          v42 = v46 + update_time == 0;
          *(this + 84) = v46 + update_time;
          if ( v42 )
            *(this + 84) = 1;
          *(this + 83) = *v120;
          *(this + 278) = _ZtlSecureTear<int>(t: 1, at: this + 276);
          sub_4027E9(a1: &a3);
          v41 = v122;
          v123 = -1;
          v42 = v122 == 0;
        }
        else
        {
          sub_4027E9(a1: &a3);
          v41 = v122;
          v123 = -1;
          v42 = v122 == 0;
        }
        if ( !v42 )
          ZXString<unsigned short>::_Release(lpAddend: (v41 - 12));
        goto LABEL_116;
      }
      sub_402F85(pvargDest: &v104, pvargSrc: &pvargSrc);
      v123 = 4;
      sub_402F85(pvargDest: &v102, pvargSrc: &pvargSrc);
      v42 = *v27 == 0;
      LOBYTE(v123) = 5;
      if ( v42 )
        _com_issue_error(a1: -2147467261);
      v48 = IWzGr2DLayer::GetAlpha(a1: v111);
      v42 = *v48 == 0;
      LOBYTE(v123) = 6;
      if ( v42 )
        _com_issue_error(a1: -2147467261);
      v49 = *v48;
      v93 = *&v104.vt;
      v50 = *v49;
      a3 = v49;
      v51 = (*(v50 + 144))(
              a1: v49,
              a2: 255,
              a3: 0,
              a4: *&v102.vt,
              a5: v102.decVal.Hi32,
              a6: v102.lVal,
              a7: v102.cyVal.Hi,
              a8: *&v104.vt,
              a9: v104.decVal.Hi32,
              a10: v104.lVal,
              a11: v104.cyVal.Hi);
      if ( v51 < 0 )
        _com_issue_errorex(a1: v51, a2: a3, a3: dword_BD8348);
      sub_41569E(a1: v111);
      LOBYTE(v123) = 4;
      v52 = ZComAPI::ZComVariantCopy(pvarg: &v102);
      if ( v52 < 0 )
        _com_issue_error(a1: v52);
      v123 = -1;
      v53 = &v104;
    }
    v60 = ZComAPI::ZComVariantCopy(pvarg: v53);
    if ( v60 < 0 )
      _com_issue_error(a1: v60);
  }
LABEL_116:
  sub_664E35(a1: this);
  CMob::PrepareActionLayer(this);
  CMob::UpdateAffectedSkillList(this, a2: 0);
  *(this + 78) = _ZtlSecureTear<long>(a1: -1, a2: this + 304);
  *(this + 102) = _ZtlSecureTear<long>(a1: 0, a2: this + 400);
  *(this + 41) = 0;
  *(this + 248) = _ZtlSecureTear<long>(a1: 0, a2: this + 984);
  *(this + 245) = _ZtlSecureTear<long>(a1: 0, a2: this + 972);
  if ( *(*(this + 98) + 520) != 0 )
  {
    CMob::CreateHPIndicator(this, a2: 100, a3: -65536);
    CMob::ShowHPIndicator(this);
  }
  v80 = (*(*TSingleton<CUserLocal>::ms_pInstance + 8))(a1: TSingleton<CUserLocal>::ms_pInstance);
  if ( (*(**(v80 + 3276) + 4))(a1: *(v80 + 3276)) != 0 && *(v80 + 3276) != 0 && sub_6635CD() == a2 )
  {
    v81 = *(v80 + 3276);
    HIDWORD(v94[0]) = 0;
    v82 = sub_672293(a1: v81);
    CMob::SetGuided(this, a2: *v82, a3: HIDWORD(v94[0]));
  }
  if ( v114 != 0 )
  {
    v83 = sub_66A004(this, &a2, a3: v114);
    sub_428712(a1: *v83);
    sub_4156C5(a1: &a2);
  }
  v84 = *(this + 98);
  HIDWORD(v94[0]) = *(v84 + 412);
  LODWORD(v94[0]) = v84 + 404;
  if ( timeGetTime() != 0 )
  {
    sub_402F85(pvargDest: &v99, pvargSrc: &pvargSrc);
    v123 = 28;
    sub_402F85(pvargDest: &v101, pvargSrc: &pvargSrc);
    v42 = *(this + 304) == 0;
    LOBYTE(v123) = 29;
    if ( v42 )
      _com_issue_error(a1: -2147467261);
    v85 = IWzGr2DLayer::GetAlpha(a1: v120);
    v42 = *v85 == 0;
    LOBYTE(v123) = 30;
    if ( v42 )
      _com_issue_error(a1: -2147467261);
    v86 = *v85;
    v93 = *&v99.vt;
    v87 = *v86;
    a2 = v86;
    v88 = (*(v87 + 144))(
            a1: v86,
            a2: 0,
            a3: 0,
            a4: *&v101.vt,
            a5: v101.decVal.Hi32,
            a6: v101.lVal,
            a7: v101.cyVal.Hi,
            a8: *&v99.vt,
            a9: v99.decVal.Hi32,
            a10: v99.lVal,
            a11: v99.cyVal.Hi);
    if ( v88 < 0 )
      _com_issue_errorex(a1: v88, a2, a3: dword_BD8348);
    sub_41569E(a1: v120);
    LOBYTE(v123) = 28;
    v89 = ZComAPI::ZComVariantCopy(pvarg: &v101);
    if ( v89 < 0 )
      _com_issue_error(a1: v89);
    v123 = -1;
    v90 = ZComAPI::ZComVariantCopy(pvarg: &v99);
    if ( v90 < 0 )
      _com_issue_error(a1: v90);
  }
  result = _ZtlSecureFuse<long>(a1: *(this + 98) + 456, a2: *(*(this + 98) + 464));
  if ( result != 0 )
  {
    sub_67010A(a1: this);
    return sub_67060B(a1: this);
  }
  return result;
}