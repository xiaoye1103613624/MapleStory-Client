# Angel v83 (13339) 登录链路反编译逐字证据

来源：`ida_angel_login_decompile.py` 批量 `decompile`（Hex-Rays，带 `/*0xNNNN*/` 地址标记）。
本文件为只读取证产物，非结论文档；结论见 `../README.md` 与 `../notes/`。

## CLogin::Init @ 0x5f42cd

```c
_DWORD *__thiscall CLogin::Init(VARIANTARG *this, volatile LONG *a2)
{
  _DWORD *v3; // ecx
  _DWORD *Instance; // eax
  int v5; // eax
  int *v6; // eax
  int v7; // eax
  int v8; // eax
  int v9; // eax
  int v10; // eax
  StringPoolStrings v11; // ecx
  _DWORD *v12; // eax
  int v13; // eax
  int v14; // eax
  int v15; // ecx
  int v16; // eax
  _DWORD *v17; // ecx
  _DWORD *v18; // eax
  int v19; // eax
  int *v20; // eax
  int v21; // eax
  int v22; // eax
  int v23; // eax
  int v24; // eax
  _DWORD *v25; // eax
  char *v26; // eax
  int v27; // eax
  VARIANTARG *v28; // ecx
  int *basic_font; // eax
  int v30; // eax
  int *v31; // eax
  int v32; // eax
  int v33; // ecx
  _DWORD *v34; // eax
  int v35; // eax
  int v36; // eax
  int **v37; // eax
  int v38; // eax
  int v39; // eax
  int *Center; // eax
  bool v41; // zf
  volatile LONG *v42; // eax
  volatile LONG v43; // ecx
  int v44; // eax
  int v45; // eax
  volatile LONG *v46; // eax
  volatile LONG v47; // ecx
  int v48; // eax
  int v49; // eax
  int v50; // eax
  int v51; // esi
  int v52; // eax
  int v53; // esi
  char *v54; // edi
  int CmdLine; // eax
  int v56; // esi
  char *v57; // edi
  int v58; // eax
  int v59; // eax
  int v60; // eax
  volatile LONG *v61; // eax
  int v62; // edx
  int v63; // eax
  int v64; // eax
  volatile LONG **v65; // eax
  volatile LONG *v66; // esi
  int StepHeight; // eax
  int v68; // eax
  int v69; // eax
  int v70; // eax
  _DWORD *result; // eax
  int v72; // [esp-18h] [ebp-A4h]
  int v73; // [esp-14h] [ebp-A0h] BYREF
  __int64 v74; // [esp-10h] [ebp-9Ch] BYREF
  VARIANTARG *v75; // [esp-8h] [ebp-94h] BYREF
  StringPoolStrings p_pvarg; // [esp-4h] [ebp-90h]
  VARIANTARG v77; // [esp+Ch] [ebp-80h] BYREF
  _BYTE v78[4]; // [esp+1Ch] [ebp-70h] BYREF
  _BYTE v79[12]; // [esp+20h] [ebp-6Ch] BYREF
  VARIANTARG v80; // [esp+2Ch] [ebp-60h] BYREF
  VARIANTARG v81; // [esp+3Ch] [ebp-50h] BYREF
  VARIANTARG pvarg; // [esp+4Ch] [ebp-40h] BYREF
  VARIANTARG pvargDest; // [esp+5Ch] [ebp-30h] BYREF
  __int64 *v84; // [esp+6Ch] [ebp-20h]
  VARIANTARG **v85; // [esp+70h] [ebp-1Ch] BYREF
  char *Unknown; // [esp+74h] [ebp-18h] BYREF
  _DWORD *v87; // [esp+78h] [ebp-14h] BYREF
  int v88; // [esp+7Ch] [ebp-10h] BYREF
  int v89; // [esp+88h] [ebp-4h]

  CMapLoadable::Init(a1: a2); /*0x5f42e2*/
  p_pvarg = &pvargSrc; /*0x5f42ee*/
  *(this + 58) = 0; /*0x5f42f2*/
  *(this + 59) = 0; /*0x5f42f8*/
  sub_402F85(pvargDest: &v80, pvargSrc: p_pvarg); /*0x5f42fe*/
  v89 = 0; /*0x5f4307*/
  sub_402F85(pvargDest: &v81, pvargSrc: &pvargSrc); /*0x5f430a*/
  p_pvarg = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f430f*/
  v75 = 0; /*0x5f4310*/
  HIDWORD(v74) = &v80; /*0x5f4314*/
  LODWORD(v74) = &v81; /*0x5f4318*/
  v73 = v3; /*0x5f4319*/
  v85 = &v73; /*0x5f431c*/
  LOBYTE(v89) = 1; /*0x5f4325*/
  Instance = StringPool::GetInstance(); /*0x5f4329*/
  StringPool::GetBSTR(this: Instance, a2: &v73, a3: SP_1306_UI_MAPLOGINIMG); /*0x5f4330*/
  LOBYTE(v89) = 1; /*0x5f4339*/
  v5 = sub_406472(); /*0x5f433d*/
  v6 = sub_402680(a1: v5); /*0x5f4344*/
  IWzResMan::GetObjectA(this: v6, pvargDest: &v77, sUOL: v73, vParam: v74, vAux: SHIDWORD(v74)); /*0x5f434b*/
  LOBYTE(v89) = 3; /*0x5f4352*/
  a2 = Ztl_variant_t::GetUnknown(a1: v75, a2: p_pvarg); /*0x5f435b*/
  v7 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &a2); /*0x5f4365*/
  if ( v7 < 0 && v7 != -2147467262 ) /*0x5f4373*/
    _com_issue_error(a1: v7); /*0x5f4376*/
  LOBYTE(v89) = 1; /*0x5f437f*/
  v8 = ZComAPI::ZComVariantCopy(pvarg: &v77); /*0x5f4383*/
  if ( v8 < 0 ) /*0x5f438b*/
    _com_issue_error(a1: v8); /*0x5f438e*/
  LOBYTE(v89) = 0; /*0x5f4393*/
  v9 = ZComAPI::ZComVariantCopy(pvarg: &v81); /*0x5f439b*/
  if ( v9 < 0 ) /*0x5f43a3*/
    _com_issue_error(a1: v9); /*0x5f43a6*/
  v89 = -1; /*0x5f43ab*/
  v10 = ZComAPI::ZComVariantCopy(pvarg: &v80); /*0x5f43b3*/
  v11 = p_pvarg; /*0x5f43ba*/
  if ( v10 < 0 ) /*0x5f43bb*/
    _com_issue_error(a1: v10); /*0x5f43be*/
  p_pvarg = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f43c3*/
  v75 = 0; /*0x5f43c4*/
  HIDWORD(v74) = v11; /*0x5f43c5*/
  Unknown = &v74 + 4; /*0x5f43c8*/
  LODWORD(v74) = SP_961_INFO; /*0x5f43cb*/
  v73 = &v74 + 4; /*0x5f43d0*/
  v12 = StringPool::GetInstance(); /*0x5f43d1*/
  StringPool::GetBSTR(this: v12, a2: v73, a3: v74); /*0x5f43d8*/
  v89 = -1; /*0x5f43dd*/
  LODWORD(v74) = &v77; /*0x5f43e4*/
  sub_40263B(a1: this + 3); /*0x5f43e8*/
  IWzProperty::GetItem(pvargDest: v74, a2: SHIDWORD(v74)); /*0x5f43ef*/
  v89 = 5; /*0x5f43f6*/
  a2 = Ztl_variant_t::GetUnknown(a1: v75, a2: p_pvarg); /*0x5f4402*/
  v13 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &a2); /*0x5f440c*/
  if ( v13 < 0 && v13 != -2147467262 ) /*0x5f441a*/
    _com_issue_error(a1: v13); /*0x5f441d*/
  v89 = -1; /*0x5f4422*/
  v14 = ZComAPI::ZComVariantCopy(pvarg: &v77); /*0x5f442a*/
  if ( v14 < 0 ) /*0x5f4432*/
    _com_issue_error(a1: v14); /*0x5f4435*/
  (*(*&this->vt + 12))(a1: this); /*0x5f443e*/
  if ( dword_BF14E8 == 0 ) /*0x5f4447*/
    _com_issue_error(a1: -2147467261); /*0x5f444e*/
  v15 = *dword_BF14E8; /*0x5f4458*/
  p_pvarg = SP_5632_HOT|SP_2336_CHARACTER_GLOVE_08DIMG|0x2A000; /*0x5f445a*/
  v75 = dword_BF14E8; /*0x5f445f*/
  a2 = dword_BF14E8; /*0x5f4460*/
  v16 = (*(v15 + 36))(a1: dword_BF14E8, a2: 180000); /*0x5f4463*/
  if ( v16 < 0 ) /*0x5f4468*/
    _com_issue_errorex(a1: v16, a2, a3: byte_BD8318); /*0x5f4473*/
  sub_402F85(pvargDest: &v81, pvargSrc: &pvargSrc); /*0x5f447c*/
  v89 = 6; /*0x5f4485*/
  sub_402F85(pvargDest: &v80, pvargSrc: &pvargSrc); /*0x5f448c*/
  p_pvarg = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f4491*/
  v75 = 0; /*0x5f4492*/
  HIDWORD(v74) = &v81; /*0x5f4496*/
  LODWORD(v74) = &v80; /*0x5f449a*/
  v73 = v17; /*0x5f449b*/
  a2 = &v73; /*0x5f449e*/
  LOBYTE(v89) = 7; /*0x5f44a7*/
  v18 = StringPool::GetInstance(); /*0x5f44ab*/
  StringPool::GetBSTR(this: v18, a2: &v73, a3: SP_1307_UI_LOGINIMG_COMMON_FRAME); /*0x5f44b2*/
  LOBYTE(v89) = 7; /*0x5f44bb*/
  v19 = sub_406472(); /*0x5f44bf*/
  v20 = sub_402680(a1: v19); /*0x5f44c6*/
  IWzResMan::GetObjectA(this: v20, pvargDest: &v77, sUOL: v73, vParam: v74, vAux: SHIDWORD(v74)); /*0x5f44cd*/
  LOBYTE(v89) = 9; /*0x5f44d4*/
  Unknown = Ztl_variant_t::GetUnknown(a1: v75, a2: p_pvarg); /*0x5f44df*/
  v87 = 0; /*0x5f44e2*/
  if ( Unknown != 0 ) /*0x5f44e5*/
  {
    v21 = sub_41E527(a1: &Unknown); /*0x5f44ee*/
    if ( v21 < 0 && v21 != -2147467262 ) /*0x5f44fc*/
      _com_issue_error(a1: v21); /*0x5f44ff*/
  }
  LOBYTE(v89) = 13; /*0x5f4508*/
  v22 = ZComAPI::ZComVariantCopy(pvarg: &v77); /*0x5f450c*/
  if ( v22 < 0 ) /*0x5f4514*/
    _com_issue_error(a1: v22); /*0x5f4517*/
  LOBYTE(v89) = 12; /*0x5f4520*/
  v23 = ZComAPI::ZComVariantCopy(pvarg: &v80); /*0x5f4524*/
  if ( v23 < 0 ) /*0x5f452c*/
    _com_issue_error(a1: v23); /*0x5f452f*/
  LOBYTE(v89) = 11; /*0x5f4538*/
  v24 = ZComAPI::ZComVariantCopy(pvarg: &v81); /*0x5f453c*/
  if ( v24 < 0 ) /*0x5f4544*/
    _com_issue_error(a1: v24); /*0x5f4547*/
  v88 = 0; /*0x5f454c*/
  p_pvarg = SP_2895_VER_DD; /*0x5f4552*/
  v75 = &a2; /*0x5f4557*/
  LOBYTE(v89) = 14; /*0x5f4558*/
  v25 = StringPool::GetInstance(); /*0x5f455c*/
  v26 = *StringPool::GetString(this: v25, result: v75, nIdx: p_pvarg); /*0x5f4568*/
  p_pvarg = SP_83_THERE_IS_CURRENTLY_NOT_ENOUGH_AVAILABLE_MEMORY_TO_RUN_MAPLESTORY_R_NPLEASE_VISIT; /*0x5f456a*/
  LOBYTE(v89) = 15; /*0x5f4572*/
  ZXString<char>::Format(a1: &v88, a2: v26, a3: 0); /*0x5f4576*/
  LOBYTE(v89) = 14; /*0x5f4583*/
  if ( a2 != 0 ) /*0x5f4587*/
    ZXString<char>::_Release(lpAddend: a2 - 3); /*0x5f458d*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x5f4597*/
  p_pvarg = &pvarg; /*0x5f459f*/
  LOBYTE(v89) = 16; /*0x5f45a3*/
  v27 = sub_428226(a1: &v88); /*0x5f45a7*/
  v75 = v28; /*0x5f45ac*/
  v85 = &v75; /*0x5f45af*/
  _bstr_t::_bstr_t(a1: v27); /*0x5f45b3*/
  LOBYTE(v89) = 17; /*0x5f45be*/
  basic_font = get_basic_font(a1: &a2, fType: 37); /*0x5f45c2*/
  LOBYTE(v89) = 19; /*0x5f45cb*/
  sub_4284A8(a1: basic_font); /*0x5f45cf*/
  v85 = sub_42782E(a1: v75, a2: p_pvarg); /*0x5f45de*/
  sub_45BAEE(a1: &a2); /*0x5f45e1*/
  LOBYTE(v89) = 14; /*0x5f45ea*/
  v30 = ZComAPI::ZComVariantCopy(&pvarg); /*0x5f45ee*/
  if ( v30 < 0 ) /*0x5f45f6*/
    _com_issue_error(a1: v30); /*0x5f45f9*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x5f4602*/
  LOBYTE(v89) = 20; /*0x5f460b*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x5f460f*/
  p_pvarg = &pvargDest; /*0x5f4617*/
  v75 = &pvarg; /*0x5f461b*/
  LOBYTE(v89) = 21; /*0x5f4622*/
  v31 = get_basic_font(a1: &a2, fType: 37); /*0x5f4626*/
  LOBYTE(v89) = 22; /*0x5f462f*/
  HIDWORD(v74) = sub_4284A5(this: v31); /*0x5f4638*/
  v32 = sub_428226(a1: &v88); /*0x5f463c*/
  LODWORD(v74) = v33; /*0x5f4641*/
  v84 = &v74; /*0x5f4644*/
  _bstr_t::_bstr_t(a1: v32); /*0x5f4648*/
  v73 = 10; /*0x5f4655*/
  v72 = 645 - v85; /*0x5f4657*/
  LOBYTE(v89) = 22; /*0x5f465b*/
  v34 = sub_414576(a1: &v87); /*0x5f465f*/
  IWzCanvas::DrawTextA( /*0x5f4666*/
    this: v34,
    nLeft: v72,
    nTop: v73,
    sText: v74,
    pFont: SHIDWORD(v74),
    vAlpha: v75,
    vTabOrg: p_pvarg);
  sub_45BAEE(a1: &a2); /*0x5f466e*/
  LOBYTE(v89) = 20; /*0x5f4677*/
  v35 = ZComAPI::ZComVariantCopy(&pvarg); /*0x5f467b*/
  if ( v35 < 0 ) /*0x5f4683*/
    _com_issue_error(a1: v35); /*0x5f4686*/
  LOBYTE(v89) = 14; /*0x5f468f*/
  v36 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x5f4693*/
  if ( v36 < 0 ) /*0x5f469b*/
    _com_issue_error(a1: v36); /*0x5f469e*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x5f46a7*/
  sub_410FDF(this: &pvarg, a2: v87, a3: 1); /*0x5f46b4*/
  LOBYTE(v89) = 25; /*0x5f46bf*/
  if ( dword_BF14EC == 0 ) /*0x5f46c3*/
    _com_issue_error(a1: -2147467261); /*0x5f46ca*/
  v37 = IWzGr2D::CreateLayer( /*0x5f46e7*/
          this: dword_BF14EC,
          &a2,
          nLeft: 0,
          nTop: 0,
          uWidth: 0,
          uHeight: 0,
          z: 110,
          vCanvas: &pvarg,
          dwFilter: &pvargDest);
  sub_428712(a1: *v37); /*0x5f46f6*/
  if ( a2 != 0 ) /*0x5f4700*/
    (*(*a2 + 8))(a1: a2); /*0x5f4705*/
  LOBYTE(v89) = 24; /*0x5f470c*/
  v38 = ZComAPI::ZComVariantCopy(&pvarg); /*0x5f4710*/
  if ( v38 < 0 ) /*0x5f4718*/
    _com_issue_error(a1: v38); /*0x5f471b*/
  LOBYTE(v89) = 14; /*0x5f4724*/
  v39 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x5f4728*/
  if ( v39 < 0 ) /*0x5f4730*/
    _com_issue_error(a1: v39); /*0x5f4733*/
  if ( dword_BF14EC == 0 ) /*0x5f473e*/
    _com_issue_error(a1: -2147467261); /*0x5f4745*/
  Center = IWzGr2D::GetCenter(this: dword_BF14EC, a2: &v85); /*0x5f4754*/
  sub_410FDF(this: &pvarg, a2: *Center, a3: 1); /*0x5f4761*/
  v41 = *(this + 89) == 0; /*0x5f4766*/
  LOBYTE(v89) = 27; /*0x5f4768*/
  if ( v41 ) /*0x5f476c*/
    _com_issue_error(a1: -2147467261); /*0x5f4773*/
  v42 = *(this + 89); /*0x5f4778*/
  v43 = *v42; /*0x5f477e*/
  a2 = v42; /*0x5f478c*/
  v44 = (*(v43 + 100))(a1: v42, a2: *&pvarg.vt, a3: pvarg.decVal.Hi32, a4: pvarg.lVal, a5: pvarg.cyVal.Hi); /*0x5f4790*/
  if ( v44 < 0 ) /*0x5f4795*/
    _com_issue_errorex(a1: v44, a2, a3: dword_BD8348); /*0x5f47a0*/
  LOBYTE(v89) = 26; /*0x5f47a9*/
  v45 = ZComAPI::ZComVariantCopy(&pvarg); /*0x5f47ad*/
  if ( v45 < 0 ) /*0x5f47b5*/
    _com_issue_error(a1: v45); /*0x5f47b8*/
  LOBYTE(v89) = 14; /*0x5f47c2*/
  if ( v85 != 0 ) /*0x5f47c6*/
    ((*v85)->lVal)(a1: v85); /*0x5f47cb*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x5f47d7*/
  LOBYTE(v89) = 28; /*0x5f47e0*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x5f47e4*/
  v41 = *(this + 89) == 0; /*0x5f47e9*/
  LOBYTE(v89) = 29; /*0x5f47f0*/
  if ( v41 ) /*0x5f47f4*/
    _com_issue_error(a1: -2147467261); /*0x5f47fb*/
  v46 = *(this + 89); /*0x5f4805*/
  v74 = *&pvargDest.vt; /*0x5f480e*/
  v47 = *v46; /*0x5f4810*/
  a2 = v46; /*0x5f482a*/
  v48 = (*(v47 + 144))( /*0x5f482e*/
          a1: v46,
          a2: -400,
          a3: -300,
          a4: *&pvarg.vt,
          a5: pvarg.decVal.Hi32,
          a6: pvarg.lVal,
          a7: pvarg.cyVal.Hi,
          a8: *&pvargDest.vt,
          a9: pvargDest.decVal.Hi32,
          a10: pvargDest.lVal,
          a11: pvargDest.cyVal.Hi);
  if ( v48 < 0 ) /*0x5f4836*/
    _com_issue_errorex(a1: v48, a2, a3: dword_BD8348); /*0x5f4841*/
  LOBYTE(v89) = 28; /*0x5f484a*/
  v49 = ZComAPI::ZComVariantCopy(&pvarg); /*0x5f484e*/
  if ( v49 < 0 ) /*0x5f4856*/
    _com_issue_error(a1: v49); /*0x5f4859*/
  LOBYTE(v89) = 14; /*0x5f4862*/
  v50 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x5f4866*/
  if ( v50 < 0 ) /*0x5f486e*/
    _com_issue_error(a1: v50); /*0x5f4871*/
  if ( *(this + 89) == 0 ) /*0x5f4883*/
    _com_issue_error(a1: -2147467261); /*0x5f488a*/
  v51 = *(this + 89); /*0x5f488f*/
  v52 = (*(*v51 + 224))(a1: v51, a2: -1); /*0x5f4896*/
  if ( v52 < 0 ) /*0x5f489e*/
    _com_issue_errorex(a1: v52, a2: v51, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x5f48a7*/
  if ( *(TSingleton<CWvsApp>::ms_pInstance + 36) == 1 ) /*0x5f48b5*/
  {
    v53 = 0; /*0x5f48b7*/
    v54 = this + 536; /*0x5f48b9*/
    do /*0x5f48f1*/
    {
      CmdLine = CWvsApp::GetCmdLine(a1: &a2, a2: v53 + 4); /*0x5f48cd*/
      LOBYTE(v89) = 30; /*0x5f48d5*/
      ZXString<char>::operator=(a1: CmdLine); /*0x5f48d9*/
      LOBYTE(v89) = 14; /*0x5f48e1*/
      ZXString<char>::_Release(a1: &a2); /*0x5f48e5*/
      ++v53; /*0x5f48ea*/
      v54 += 4; /*0x5f48eb*/
    }
    while ( v53 < 5 ); /*0x5f48f1*/
  }
  else
  {
    v56 = 0; /*0x5f48f5*/
    v57 = this + 536; /*0x5f48f7*/
    do /*0x5f492f*/
    {
      v58 = CWvsApp::GetCmdLine(a1: &a2, a2: v56 + 2); /*0x5f490b*/
      LOBYTE(v89) = 31; /*0x5f4913*/
      ZXString<char>::operator=(a1: v58); /*0x5f4917*/
      LOBYTE(v89) = 14; /*0x5f491f*/
      ZXString<char>::_Release(a1: &a2); /*0x5f4923*/
      ++v56; /*0x5f4928*/
      v57 += 4; /*0x5f4929*/
    }
    while ( v56 < 5 ); /*0x5f492f*/
  }
  a2 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 256); /*0x5f4942*/
  LOBYTE(v89) = 32; /*0x5f4947*/
  if ( a2 != 0 ) /*0x5f494b*/
    v59 = sub_60E157(a1: this); /*0x5f4950*/
  else
    v59 = 0; /*0x5f4957*/
  LOBYTE(v89) = 14; /*0x5f4960*/
  sub_5FD7DC(a1: v59); /*0x5f4964*/
  v60 = 1; /*0x5f4976*/
  if ( *(*dword_BE7918 + 8228) == 1 )
  {
    *(this + 90) = 1; /*0x5f497f*/
    *(this + 96) = 1; /*0x5f4985*/
    if ( *(TSingleton<CWvsApp>::ms_pInstance + 24) != -800 ) /*0x5f499c*/
      v60 = *(TSingleton<CWvsApp>::ms_pInstance + 24) + 800; /*0x5f499e*/
    *(this + 100) = -1; /*0x5f49a0*/
    *(this + 91) = v60; /*0x5f49a7*/
    sub_5FDD2D(a1: 15, a2: &a2 + 3); /*0x5f49b9*/
    sub_5FDDA6(a1: 15, a2: &a2 + 3); /*0x5f49ca*/
    sub_5FDC2A(a1: 15, a2: &a2 + 3); /*0x5f49db*/
    sub_5FDD2D(a1: 60, a2: &a2 + 3); /*0x5f49ec*/
    sub_5FDDA6(a1: 60, a2: &a2 + 3); /*0x5f49fd*/
    sub_5FC313(a1: 0); /*0x5f4a06*/
    a2 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 400); /*0x5f4a17*/
    LOBYTE(v89) = 33; /*0x5f4a1c*/
    if ( a2 != 0 ) /*0x5f4a20*/
      sub_6246CE(a1: this); /*0x5f4a25*/
    LOBYTE(v89) = 14; /*0x5f4a2f*/
    COutPacket::COutPacket(a1: 4); /*0x5f4a33*/
    LOBYTE(v89) = 34; /*0x5f4a42*/
    CClientSocket::SendPacket(this: dword_BE7914, a2: &pvarg); /*0x5f4a46*/
    sub_60E632(a1: (*(this + 372) != 2 ? 0 : 4) | 2);
    LOBYTE(v89) = 14; /*0x5f4a6f*/
    ZArray<unsigned char>::RemoveAll(a1: &pvarg.decVal.Hi32); /*0x5f4a73*/
  }
  else
  {
    v61 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1516); /*0x5f4a81*/
    a2 = v61; /*0x5f4a86*/
    LOBYTE(v89) = 35; /*0x5f4a8b*/
    if ( v61 != 0 ) /*0x5f4a8f*/
      CLicenseDlg::Draw(a1: v61, a2: v62, a3: this); /*0x5f4a94*/
    LOBYTE(v89) = 14; /*0x5f4a99*/
  }
  VariantInit(&pvarg); /*0x5f4aa7*/
  v63 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x5f4ab3*/
  if ( v63 < 0 ) /*0x5f4abc*/
    _com_issue_error(a1: v63); /*0x5f4abf*/
  LOBYTE(v89) = 36; /*0x5f4ac8*/
  VariantInit(pvarg: &pvargDest); /*0x5f4acc*/
  v64 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc); /*0x5f4ad3*/
  if ( v64 < 0 ) /*0x5f4adc*/
    _com_issue_error(a1: v64); /*0x5f4adf*/
  LOBYTE(v89) = 37; /*0x5f4aeb*/
  if ( dword_BF14EC == 0 ) /*0x5f4aef*/
    _com_issue_error(a1: -2147467261); /*0x5f4af6*/
  v65 = IWzGr2D::GetCenter(this: dword_BF14EC, a2: &v85); /*0x5f4b05*/
  v41 = *v65 == 0; /*0x5f4b0c*/
  LOBYTE(v89) = 38; /*0x5f4b0f*/
  if ( v41 ) /*0x5f4b13*/
    _com_issue_error(a1: -2147467261); /*0x5f4b1a*/
  v66 = *v65; /*0x5f4b1f*/
  p_pvarg = *(this + 90); /*0x5f4b21*/
  a2 = v66; /*0x5f4b29*/
  StepHeight = CLogin::GetStepHeight(this, a2: p_pvarg); /*0x5f4b2c*/
  v68 = (*(*v66 + 144))( /*0x5f4b51*/
          a1: a2,
          a2: 28,
          a3: StepHeight,
          a4: *&pvargDest.vt,
          a5: pvargDest.decVal.Hi32,
          a6: pvargDest.lVal,
          a7: pvargDest.cyVal.Hi,
          a8: *&pvarg.vt,
          a9: pvarg.decVal.Hi32,
          a10: pvarg.lVal,
          a11: pvarg.cyVal.Hi);
  if ( v68 < 0 ) /*0x5f4b59*/
    _com_issue_errorex(a1: v68, a2, a3: dword_BD8348); /*0x5f4b64*/
  LOBYTE(v89) = 37; /*0x5f4b6e*/
  if ( v85 != 0 ) /*0x5f4b72*/
    ((*v85)->lVal)(a1: v85); /*0x5f4b77*/
  LOBYTE(v89) = 36; /*0x5f4b7e*/
  v69 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x5f4b82*/
  if ( v69 < 0 ) /*0x5f4b8a*/
    _com_issue_error(a1: v69); /*0x5f4b8d*/
  LOBYTE(v89) = 14; /*0x5f4b96*/
  v70 = ZComAPI::ZComVariantCopy(&pvarg); /*0x5f4b9a*/
  if ( v70 < 0 ) /*0x5f4ba2*/
    _com_issue_error(a1: v70); /*0x5f4ba5*/
  sub_5FBA85(a1: this); /*0x5f4bac*/
  CMapLoadable::PlayBGMFromMapInfo(a1: this); /*0x5f4bb3*/
  sub_776B64(); /*0x5f4bba*/
  COutPacket::COutPacket(a1: 35); /*0x5f4bc4*/
  LOBYTE(v89) = 39; /*0x5f4bd3*/
  CClientSocket::SendPacket(this: dword_BE7914, a2: v78); /*0x5f4bd7*/
  LOBYTE(v89) = 14; /*0x5f4bdf*/
  ZArray<unsigned char>::RemoveAll(a1: v79); /*0x5f4be3*/
  LOBYTE(v89) = 11; /*0x5f4beb*/
  ZXString<char>::_Release(a1: &v88); /*0x5f4bef*/
  result = v87; /*0x5f4bf4*/
  v89 = -1; /*0x5f4bf7*/
  if ( v87 != 0 ) /*0x5f4bfd*/
    return (*(*v87 + 8))(a1: v87); /*0x5f4c02*/
  return result; /*0x5f4c05*/
}
```

## CLogin::Update @ 0x5f4c16

```c
void __thiscall CLogin::Update(CLogin *this)
{
  int v2; // edi
  int v3; // eax
  int v4; // eax
  CWnd *v5; // ecx
  int v6; // eax
  int v7; // eax
  int v8; // edx
  _DWORD *v9; // ecx
  int v11; // eax
  int v12; // eax
  int v13; // eax
  int v14; // edx
  _DWORD *v15; // ecx
  int v16; // edx
  _DWORD *v17; // ecx
  int v18; // edx
  _DWORD *v19; // ecx
  int v20; // eax
  int v21; // eax
  int v22; // edx
  _DWORD *v23; // ecx
  int v24; // edx
  _DWORD *v25; // ecx
  int v26; // edx
  _DWORD *v27; // ecx
  int v28; // eax
  int v29; // eax
  CFadeWnd *v30; // ecx
  int v31; // edx
  _DWORD *v32; // ecx
  int v33; // eax
  CFadeWnd *v34; // ecx
  int v35; // edx
  _DWORD *v36; // ecx
  int v37; // eax
  int v38; // eax
  _BYTE *v39; // eax
  _BYTE *v40; // eax
  _BYTE *v41; // eax
  const char *v42; // edi
  int v43; // eax
  _BYTE *v44; // eax
  _BYTE *v45; // eax
  unsigned int v46; // [esp-4h] [ebp-24h]

  if ( *(this + 97) != 0 ) /*0x5f4c2f*/
    dword_BF041C(a1: 0); /*0x5f4c32*/
  v2 = *(TSingleton<CWvsApp>::ms_pInstance + 24); /*0x5f4c3d*/
  v3 = *(this + 91); /*0x5f4c40*/
  if ( v3 != 0 && v2 - v3 > 0 ) /*0x5f4c50*/
  {
    CLogin::OnStepChanged(this); /*0x5f4c54*/
    *(this + 91) = 0; /*0x5f4c59*/
  }
  v4 = *(this + 90); /*0x5f4c5f*/
  if ( v4 == 1 ) /*0x5f4c68*/
  {
    if ( *(TSingleton<CWvsApp>::ms_pInstance + 36) == 1 ) /*0x5f4c73*/
    {
      CLogin::EnableLoginStartCtrl(this, a2: 2); /*0x5f4c77*/
      goto LABEL_14; /*0x5f4c77*/
    }
    if ( *(this + 77) == 0 ) /*0x5f4c7f*/
    {
      CLogin::EnableLoginStartCtrl(this, a2: 17); /*0x5f4c83*/
      goto LABEL_14; /*0x5f4c83*/
    }
    goto LABEL_12; /*0x5f4c7f*/
  }
  if ( v4 != 0 ) /*0x5f4c87*/
  {
LABEL_12:
    CLogin::EnableLoginStartCtrl(this, a2: 1); /*0x5f4c89*/
    goto LABEL_14; /*0x5f4c8b*/
  }
  CLogin::EnableLoginStartCtrl(this, a2: 0); /*0x5f4c90*/
LABEL_14:
  v5 = *(this + 120); /*0x5f4c95*/
  if ( v5 != 0 ) /*0x5f4c9d*/
    CWnd::InvalidateRect(this: v5, a2: 0); /*0x5f4ca0*/
  if ( *(this + 71) != 0 ) /*0x5f4cab*/
  {
    v6 = *(this + 73); /*0x5f4cb1*/
    if ( v6 != 0 && v2 - v6 > 10000 ) /*0x5f4cc7*/
    {
      v7 = *(this + 76); /*0x5f4ccd*/
      *(this + 71) = 0; /*0x5f4cd5*/
      *(this + 75) = v7; /*0x5f4cdb*/
      if ( v7 == 0 ) /*0x5f4ce1*/
      {
        if ( *(this + 70) != 0 ) /*0x5f4ce9*/
          CLogin::RemoveNoticeConnecting(a1: this); /*0x5f4ced*/
        CLogin::ResetVAC(a1: this); /*0x5f4cf4*/
        CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5f4cfd*/
        return; /*0x5f4d02*/
      }
      CLogin::MakeVACDlg(a1: this); /*0x5f4d09*/
      CLogin::ChangeStep(this, a2: 5); /*0x5f4d12*/
      if ( dword_BEDA38 != 0 ) /*0x5f4d1d*/
      {
        CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5f4d6b*/
      }
      else
      {
        ZSynchronizedHelper<ZFatalSection>::ZSynchronizedHelper<ZFatalSection>(a1: this + 332); /*0x5f4d2a*/
        v9 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 15352); /*0x5f4d41*/
        if ( v9 != 0 ) /*0x5f4d4c*/
          CUIAvatarVAC::CUIAvatarVAC(a1: v9, a2: v8, a3: this, a4: *(this + 78)); /*0x5f4d55*/
        if ( (*(this + 84))-- == 1 ) /*0x5f4d5e*/
          *(this + 83) = 0; /*0x5f4d63*/
      }
    }
  }
  if ( *(this + 143) != 0 ) /*0x5f4d76*/
  {
    v11 = *(this + 142); /*0x5f4d7c*/
    if ( v11 != 0 ) /*0x5f4d84*/
    {
      if ( v11 == 1 ) /*0x5f4d8b*/
      {
        if ( dword_BEDA6C != 0 ) /*0x5f4d99*/
        {
          CWnd::Destroy(a1: dword_BEDA6C); /*0x5f4d9b*/
          if ( dword_BEDA6C != 0 ) /*0x5f4da7*/
            (**(dword_BEDA6C + 8))(a1: dword_BEDA6C + 8, a2: 1); /*0x5f4db0*/
        }
        if ( dword_BEDA48 != 0 ) /*0x5f4dba*/
        {
          CWnd::Destroy(a1: dword_BEDA48); /*0x5f4dbc*/
          if ( dword_BEDA48 != 0 ) /*0x5f4dc8*/
            (**(dword_BEDA48 + 8))(a1: dword_BEDA48 + 8, a2: 1); /*0x5f4dd1*/
        }
        if ( dword_BEDA44 != 0 ) /*0x5f4ddb*/
        {
          CWnd::Destroy(a1: dword_BEDA44); /*0x5f4ddd*/
          if ( dword_BEDA44 != 0 ) /*0x5f4de9*/
            (**(dword_BEDA44 + 8))(a1: dword_BEDA44 + 8, a2: 1); /*0x5f4df2*/
        }
        if ( dword_BEDA40 != 0 ) /*0x5f4dfc*/
        {
          CWnd::Destroy(a1: dword_BEDA40); /*0x5f4dfe*/
          if ( dword_BEDA40 != 0 ) /*0x5f4e0a*/
            (**(dword_BEDA40 + 8))(a1: dword_BEDA40 + 8, a2: 1); /*0x5f4e13*/
        }
        v12 = *(this + 133); /*0x5f4e15*/
        if ( v12 != 0 ) /*0x5f4e1d*/
        {
          v13 = v12 - 1; /*0x5f4e23*/
          if ( v13 != 0 ) /*0x5f4e24*/
          {
            if ( v13 == 1 ) /*0x5f4e27*/
            {
              if ( dword_BEDA34 != 0 ) /*0x5f4e33*/
                sub_5FD8FF(); /*0x5f4e35*/
              v15 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 312); /*0x5f4e49*/
              if ( v15 != 0 ) /*0x5f4e57*/
                sub_619A66(a1: v15, a2: v14, a3: this); /*0x5f4e5e*/
            }
          }
          else
          {
            if ( dword_BEDA34 != 0 ) /*0x5f4e6e*/
              sub_5FD8FF(); /*0x5f4e70*/
            v17 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 312); /*0x5f4e84*/
            if ( v17 != 0 ) /*0x5f4e92*/
              sub_619119(a1: v17, a2: v16, a3: this); /*0x5f4e99*/
          }
        }
        else
        {
          if ( dword_BEDA34 != 0 ) /*0x5f4ea9*/
            sub_5FD8FF(); /*0x5f4eab*/
          v19 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 312); /*0x5f4ebf*/
          if ( v19 != 0 ) /*0x5f4ecd*/
            sub_6183D1(this: v19, edx0: v18, a3: this); /*0x5f4ed4*/
        }
      }
    }
    else
    {
      if ( dword_BEDA6C != 0 ) /*0x5f4ee6*/
      {
        CWnd::Destroy(a1: dword_BEDA6C); /*0x5f4ee8*/
        if ( dword_BEDA6C != 0 ) /*0x5f4ef4*/
          (**(dword_BEDA6C + 8))(a1: dword_BEDA6C + 8, a2: 1); /*0x5f4eff*/
      }
      if ( dword_BEDA34 != 0 ) /*0x5f4f0e*/
      {
        CWnd::Destroy(a1: dword_BEDA34); /*0x5f4f10*/
        if ( dword_BEDA34 != 0 ) /*0x5f4f1c*/
          (**(dword_BEDA34 + 8))(a1: dword_BEDA34 + 8, a2: 1); /*0x5f4f24*/
      }
      v20 = *(this + 133); /*0x5f4f26*/
      if ( v20 != 0 ) /*0x5f4f2e*/
      {
        v21 = v20 - 1; /*0x5f4f34*/
        if ( v21 != 0 ) /*0x5f4f35*/
        {
          if ( v21 == 1 ) /*0x5f4f3c*/
          {
            if ( dword_BEDA48 != 0 ) /*0x5f4f4a*/
            {
              CWnd::Destroy(a1: dword_BEDA48); /*0x5f4f4c*/
              if ( dword_BEDA48 != 0 ) /*0x5f4f58*/
                (**(dword_BEDA48 + 8))(a1: dword_BEDA48 + 8, a2: 1); /*0x5f4f60*/
            }
            if ( dword_BEDA44 != 0 ) /*0x5f4f6a*/
            {
              CWnd::Destroy(a1: dword_BEDA44); /*0x5f4f6c*/
              if ( dword_BEDA44 != 0 ) /*0x5f4f78*/
                (**(dword_BEDA44 + 8))(a1: dword_BEDA44 + 8, a2: 1); /*0x5f4f80*/
            }
            if ( dword_BEDA40 != 0 ) /*0x5f4f8a*/
            {
              CWnd::Destroy(a1: dword_BEDA40); /*0x5f4f8c*/
              if ( dword_BEDA40 != 0 ) /*0x5f4f98*/
                (**(dword_BEDA40 + 8))(a1: dword_BEDA40 + 8, a2: 1); /*0x5f4fa0*/
            }
            v23 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 140); /*0x5f4fb1*/
            if ( v23 != 0 ) /*0x5f4fbf*/
              CUINewCharNameSelectAran::CUINewCharNameSelectAran(a1: v23, a2: v22, a3: this); /*0x5f4fc6*/
          }
        }
        else
        {
          if ( dword_BEDA48 != 0 ) /*0x5f4fd8*/
          {
            CWnd::Destroy(a1: dword_BEDA48); /*0x5f4fda*/
            if ( dword_BEDA48 != 0 ) /*0x5f4fe6*/
              (**(dword_BEDA48 + 8))(a1: dword_BEDA48 + 8, a2: 1); /*0x5f4fee*/
          }
          if ( dword_BEDA44 != 0 ) /*0x5f4ff8*/
          {
            CWnd::Destroy(a1: dword_BEDA44); /*0x5f4ffa*/
            if ( dword_BEDA44 != 0 ) /*0x5f5006*/
              (**(dword_BEDA44 + 8))(a1: dword_BEDA44 + 8, a2: 1); /*0x5f500e*/
          }
          if ( dword_BEDA40 != 0 ) /*0x5f5018*/
          {
            CWnd::Destroy(a1: dword_BEDA40); /*0x5f501a*/
            if ( dword_BEDA40 != 0 ) /*0x5f5026*/
              (**(dword_BEDA40 + 8))(a1: dword_BEDA40 + 8, a2: 1); /*0x5f502e*/
          }
          v25 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 140); /*0x5f503f*/
          if ( v25 != 0 ) /*0x5f504d*/
            CUINewCharNameSelectNormal::CUINewCharNameSelectNormal(a1: v25, a2: v24, a3: this); /*0x5f5054*/
        }
      }
      else
      {
        if ( dword_BEDA48 != 0 ) /*0x5f5066*/
        {
          CWnd::Destroy(a1: dword_BEDA48); /*0x5f5068*/
          if ( dword_BEDA48 != 0 ) /*0x5f5074*/
            (**(dword_BEDA48 + 8))(a1: dword_BEDA48 + 8, a2: 1); /*0x5f507c*/
        }
        if ( dword_BEDA44 != 0 ) /*0x5f5086*/
        {
          CWnd::Destroy(a1: dword_BEDA44); /*0x5f5088*/
          if ( dword_BEDA44 != 0 ) /*0x5f5094*/
            (**(dword_BEDA44 + 8))(a1: dword_BEDA44 + 8, a2: 1); /*0x5f509c*/
        }
        if ( dword_BEDA40 != 0 ) /*0x5f50a6*/
        {
          CWnd::Destroy(a1: dword_BEDA40); /*0x5f50a8*/
          if ( dword_BEDA40 != 0 ) /*0x5f50b4*/
            (**(dword_BEDA40 + 8))(a1: dword_BEDA40 + 8, a2: 1); /*0x5f50bc*/
        }
        v27 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 140); /*0x5f50cd*/
        if ( v27 != 0 ) /*0x5f50db*/
          CUINewCharNameSelectCygnus::CUINewCharNameSelectCygnus(a1: v27, a2: v26, a3: this); /*0x5f50de*/
      }
    }
    *(this + 143) = 0; /*0x5f50e7*/
  }
  v28 = *(this + 90); /*0x5f50ed*/
  if ( v28 != 2 && v28 != 5 ) /*0x5f50fb*/
  {
    sub_581FAA(a1: 0); /*0x5f5113*/
    sub_581FAA(a1: 0); /*0x5f511f*/
  }
  if ( *(this + 118) != 0 && *(this + 90) != 4 && *(this + 118) != 0 ) /*0x5f5141*/
  {
    sub_428C15(a1: 0); /*0x5f5146*/
    *(this + 118) = 0; /*0x5f514b*/
  }
  if ( *(this + 90) != 2 || (v29 = *(this + 100)) < 0 || *(this + 126) != 0 || v29 >= 15 ) /*0x5f516c*/
  {
LABEL_117:
    v30 = dword_BEDA50; /*0x5f51a7*/
    if ( dword_BEDA50 == 0 ) /*0x5f51af*/
      goto LABEL_121; /*0x5f51af*/
    goto LABEL_118; /*0x5f51af*/
  }
  v30 = dword_BEDA50; /*0x5f516e*/
  if ( dword_BEDA50 == 0 ) /*0x5f5176*/
  {
    v32 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 240); /*0x5f5187*/
    if ( v32 != 0 ) /*0x5f5195*/
      CUICharDetail::CUICharDetail(a1: v32, a2: v31, a3: this, a4: *(this + 100)); /*0x5f519e*/
    goto LABEL_117; /*0x5f519e*/
  }
LABEL_118:
  if ( *(this + 126) != 0 || *(this + 100) != *(v30 + 54) ) /*0x5f51c5*/
    CFadeWnd::Close(this: v30, a2: 0); /*0x5f51c8*/
LABEL_121:
  if ( *(this + 90) != 5 ) /*0x5f51d4*/
    goto LABEL_129; /*0x5f51d4*/
  v33 = *(this + 100); /*0x5f51d6*/
  if ( v33 < 0 || *(this + 126) != 0 || v33 >= 60 ) /*0x5f51eb*/
    goto LABEL_129; /*0x5f51eb*/
  v34 = dword_BEDA3C; /*0x5f51ed*/
  if ( dword_BEDA3C != 0 ) /*0x5f51f5*/
  {
LABEL_130:
    if ( *(this + 126) != 0 || *(this + 100) != *(v34 + 54) ) /*0x5f524c*/
      CFadeWnd::Close(this: v34, a2: 0); /*0x5f524f*/
    goto LABEL_133; /*0x5f524f*/
  }
  if ( *(this + 76) != 0 ) /*0x5f51fd*/
  {
    v36 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 236); /*0x5f520e*/
    if ( v36 != 0 ) /*0x5f521c*/
      CUICharDetailVAC::CUICharDetailVAC(a1: v36, a2: v35, a3: this, a4: *(this + 100)); /*0x5f5225*/
LABEL_129:
    v34 = dword_BEDA3C; /*0x5f522e*/
    if ( dword_BEDA3C != 0 ) /*0x5f5236*/
      goto LABEL_130; /*0x5f5236*/
  }
LABEL_133:
  v37 = *(this + 90); /*0x5f5254*/
  if ( v37 != 0 ) /*0x5f525c*/
  {
    v38 = v37 - 1; /*0x5f5262*/
    if ( v38 != 0 ) /*0x5f5263*/
    {
      if ( v38 == 1 ) /*0x5f5266*/
      {
        v39 = *(this + 138); /*0x5f526c*/
        if ( v39 != 0 && *v39 != 0 && *(TSingleton<CWvsApp>::ms_pInstance + 40) != 0 && *(this + 91) == 0 ) /*0x5f5296*/
        {
          *(TSingleton<CWvsApp>::ms_pInstance + 40) = 0; /*0x5f529c*/
          *(this + 100) = atoi(a1: *(this + 138)); /*0x5f52ad*/
          CLogin::SendSelectCharPacket(this); /*0x5f52b3*/
        }
      }
    }
    else
    {
      v40 = *(this + 136); /*0x5f52bd*/
      if ( v40 != 0 && *v40 != 0 ) /*0x5f52cd*/
      {
        v41 = *(this + 137); /*0x5f52d3*/
        if ( v41 != 0 && *v41 != 0 && *(TSingleton<CWvsApp>::ms_pInstance + 40) != 0 && *(this + 91) == 0 ) /*0x5f52f1*/
        {
          v42 = *(this + 136); /*0x5f52f9*/
          v46 = atoi(a1: *(this + 137)); /*0x5f5306*/
          v43 = atoi(a1: v42); /*0x5f5308*/
          CLogin::SendViewAllCharPacket(this, a2: v43, a3: v46); /*0x5f5311*/
        }
      }
    }
  }
  else
  {
    v44 = *(this + 134); /*0x5f5318*/
    if ( v44 != 0 && *v44 != 0 ) /*0x5f5324*/
    {
      v45 = *(this + 135); /*0x5f5326*/
      if ( v45 != 0 && *v45 != 0 && *(TSingleton<CWvsApp>::ms_pInstance + 40) != 0 && *dword_BEDA60 != 0 ) /*0x5f5344*/
        CLogin::SendCheckPasswordPacket(a1: *(this + 134), a2: *(this + 135)); /*0x5f5354*/
    }
  }
}
```

## CLogin::ChangeStep @ 0x5f53c0

```c
void __thiscall CLogin::ChangeStep(CLogin *this, wchar_t *a2)
{
  int v2; // edx
  int v4; // eax
  bool v5; // zf
  int v6; // eax
  bool v7; // cc
  int v8; // eax
  CFadeWnd *v9; // ecx
  int v10; // eax
  int v11; // eax
  int v12; // eax
  int v13; // eax
  int v14; // eax
  int v15; // eax
  int *v16; // ecx
  _DWORD *v17; // eax
  CFadeWnd *v18; // ecx
  int v19; // eax
  int v20; // edx
  int **v21; // ecx
  int v22; // eax
  CFadeWnd *v23; // ecx
  CFadeWnd *v24; // ecx
  int Instance; // eax
  wchar_t *StepHeight; // eax
  int v27; // eax
  _DWORD *v28; // eax
  int v29; // [esp-20h] [ebp-28Ch]
  int *v30; // [esp-1Ch] [ebp-288h] BYREF
  int *v31; // [esp-18h] [ebp-284h] BYREF
  unsigned int v32; // [esp-14h] [ebp-280h]
  signed int v33; // [esp-10h] [ebp-27Ch]
  int *v34; // [esp-Ch] [ebp-278h]
  int p_a1; // [esp-8h] [ebp-274h]
  StringPoolStrings v36; // [esp-4h] [ebp-270h]
  int v37; // [esp+0h] [ebp-26Ch]
  int v38; // [esp+4h] [ebp-268h]
  _BYTE v39[13]; // [esp+Ch] [ebp-260h] BYREF
  int v40; // [esp+19h] [ebp-253h]
  int v41; // [esp+1D4h] [ebp-98h] BYREF
  double v42; // [esp+1DCh] [ebp-90h]
  double v43; // [esp+1E4h] [ebp-88h]
  double v44; // [esp+1ECh] [ebp-80h]
  double v45; // [esp+1F4h] [ebp-78h]
  int v46; // [esp+1FCh] [ebp-70h]
  int v47; // [esp+204h] [ebp-68h]
  double v48; // [esp+20Ch] [ebp-60h]
  double v49; // [esp+214h] [ebp-58h]
  double v50; // [esp+21Ch] [ebp-50h]
  double v51; // [esp+224h] [ebp-48h]
  int v52; // [esp+22Ch] [ebp-40h]
  _BYTE v53[4]; // [esp+234h] [ebp-38h] BYREF
  _BYTE v54[12]; // [esp+238h] [ebp-34h] BYREF
  int **v55; // [esp+244h] [ebp-28h]
  int **v56; // [esp+248h] [ebp-24h]
  char v57; // [esp+24Ch] [ebp-20h] BYREF
  char v58; // [esp+24Dh] [ebp-1Fh] BYREF
  char v59; // [esp+24Eh] [ebp-1Eh] BYREF
  char v60; // [esp+24Fh] [ebp-1Dh] BYREF
  int a1; // [esp+250h] [ebp-1Ch] BYREF
  int v62; // [esp+254h] [ebp-18h]
  int v63; // [esp+258h] [ebp-14h] BYREF
  int v64; // [esp+25Ch] [ebp-10h]
  int v65; // [esp+268h] [ebp-4h]

  v2 = a2; /*0x5f53d0*/
  v4 = *(this + 90); /*0x5f53d7*/
  v64 = v4; /*0x5f53e2*/
  if ( a2 < 0 ) /*0x5f53e5*/
    v2 = (v4 + 1) % 6; /*0x5f53ec*/
  v5 = v64 == v2; /*0x5f53ee*/
  *(this + 90) = v2; /*0x5f53f1*/
  if ( !v5 )
  {
    a1 = *(TSingleton<CWvsApp>::ms_pInstance + 24); /*0x5f540b*/
    v62 = 300 * abs(a1: v64 - v2) + 500; /*0x5f541f*/
    v6 = a1 + v62; /*0x5f5422*/
    if ( a1 + v62 == 0 ) /*0x5f5429*/
      v6 = 1; /*0x5f542b*/
    v7 = *(this + 90) < 3; /*0x5f542d*/
    *(this + 91) = v6; /*0x5f5434*/
    if ( !v7 ) /*0x5f543a*/
      sub_60E632(a1: 1); /*0x5f5443*/
    v8 = *(this + 90); /*0x5f5448*/
    if ( v8 == 1 )
    {
      if ( *(TSingleton<CWvsApp>::ms_pInstance + 36) == 1 ) /*0x5f545a*/
      {
        v36 = SP_2_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTPASSWORD; /*0x5f5462*/
        goto LABEL_16; /*0x5f5464*/
      }
      if ( *(this + 77) == 0 ) /*0x5f5472*/
      {
        v36 = SP_17_; /*0x5f5474*/
        goto LABEL_16; /*0x5f5476*/
      }
    }
    else if ( v8 == 0 )
    {
      v36 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f5485*/
LABEL_16:
      sub_60E632(a1: v36); /*0x5f5486*/
      if ( *(this + 90) == 1 ) /*0x5f5497*/
        sub_60E903(a1: 1); /*0x5f549a*/
      else
        sub_60E903(a1: 4); /*0x5f54a8*/
      CWnd::InvalidateRect(this: *(this + 120), a2: 0); /*0x5f54b4*/
      v9 = *(this + 122); /*0x5f54b9*/
      if ( v9 != 0 && (v64 != 3 || *(this + 90) != 4) && (v64 != 4 || *(this + 90) != 3) ) /*0x5f54dd*/
        CFadeWnd::Close(this: v9, a2: 0); /*0x5f54e0*/
      v10 = *(this + 90); /*0x5f54e5*/
      if ( v10 == 0 ) /*0x5f54ed*/
      {
        *(this + 92) = 0; /*0x5f57f3*/
        v24 = *(this + 124); /*0x5f57f9*/
        if ( v24 != 0 ) /*0x5f5801*/
          CFadeWnd::Close(this: v24, a2: 0); /*0x5f5804*/
        *(this + 133) = 0; /*0x5f5809*/
        if ( *(TSingleton<CWvsApp>::ms_pInstance + 36) == 2 ) /*0x5f5818*/
        {
          Instance = CNMCOClientObject::GetInstance(a1: v37, a2: v38); /*0x5f581a*/
          CNMCOClientObject::LogoutAuth(a1: Instance); /*0x5f5821*/
        }
        goto LABEL_69; /*0x5f5821*/
      }
      v11 = v10 - 1; /*0x5f54f3*/
      if ( v11 != 0 )
      {
        v12 = v11 - 1; /*0x5f54fa*/
        if ( v12 != 0 )
        {
          v13 = v12 - 1; /*0x5f5501*/
          if ( v13 != 0 )
          {
            v14 = v13 - 1; /*0x5f5508*/
            if ( v14 != 0 )
            {
              if ( v14 == 1 )
                *(*(this + 120) + 252) = 0; /*0x5f5530*/
              else
                CWndMan::SetFocus(a1: this != 0 ? this + 4 : 0);
            }
            else
            {
              v63 = 0; /*0x5f5545*/
              ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF); /*0x5f5548*/
              v65 = 3; /*0x5f5557*/
              ZXString<char>::operator=(a1: &v63); /*0x5f555e*/
              v65 = -1; /*0x5f5563*/
              ZXString<char>::_Release(a1: &v63); /*0x5f556a*/
              sub_5FCF8A(a1: this); /*0x5f5571*/
              AvatarLook::`vftable'(a1: v39); /*0x5f557c*/
              v39[12] = *(*dword_BE7918 + 8236); /*0x5f558c*/
              v15 = *(this + 133); /*0x5f5592*/
              v65 = 4; /*0x5f559a*/
              if ( v15 != 0 ) /*0x5f559d*/
              {
                if ( v15 == 2 ) /*0x5f55ae*/
                  v40 = 11; /*0x5f55b0*/
              }
              else
              {
                v40 = 10; /*0x5f559f*/
              }
              CLogin::GetSelectedAL(this, a2: v39); /*0x5f55c3*/
              ZRef<CAvatar>::_Alloc(a1: this + 468); /*0x5f55d0*/
              v36 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f55d5*/
              p_a1 = 100; /*0x5f55d6*/
              v34 = (-2369 - 600 * sub_5F595B(a1: this)); /*0x5f55ec*/
              v33 = 22; /*0x5f55ed*/
              v32 = 0; /*0x5f55ef*/
              v31 = v34; /*0x5f55f0*/
              v55 = &v31; /*0x5f55f3*/
              sub_432C10(a1: 0); /*0x5f55f7*/
              v30 = v16; /*0x5f55fc*/
              v56 = &v30; /*0x5f55ff*/
              LOBYTE(v65) = 5; /*0x5f5603*/
              sub_432BDD(a1: 0); /*0x5f5607*/
              LOBYTE(v65) = 4; /*0x5f560f*/
              v29 = sub_426626(a1: 2, a2: 0); /*0x5f561a*/
              v17 = sub_4285F2(a1: this + 468); /*0x5f5624*/
              CAvatar::Init( /*0x5f562b*/
                this: v17,
                a2: v39,
                nMoveAction: v29,
                pOrigin: v30,
                pOverlay: v31,
                z: v32,
                x: v33,
                y: v34,
                nScale: p_a1,
                nDefaultEmotion: v36);
              v65 = -1; /*0x5f5630*/
            }
          }
          else
          {
            v18 = *(this + 124); /*0x5f5639*/
            if ( v18 != 0 && v64 <= 2 ) /*0x5f5647*/
              CFadeWnd::Close(this: v18, a2: 0); /*0x5f564a*/
            v19 = dword_BEDA6C; /*0x5f564f*/
            if ( dword_BEDA6C == 0 ) /*0x5f5656*/
            {
              v21 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 188); /*0x5f5667*/
              v56 = v21; /*0x5f5669*/
              v65 = 2; /*0x5f566e*/
              if ( v21 != 0 ) /*0x5f5675*/
                CUINewCharRaceSelect::CUINewCharRaceSelect(a1: v21, a2: v20, a3: this); /*0x5f5678*/
              v65 = -1; /*0x5f567d*/
              v19 = dword_BEDA6C; /*0x5f5681*/
            }
            CWndMan::SetFocus(a1: v19 != 0 ? v19 + 4 : 0);
            *(this + 133) = 1; /*0x5f569d*/
          }
          goto LABEL_69; /*0x5f5525*/
        }
        if ( TSingleton<CWndMan>::ms_pInstance == 0 ) /*0x5f56b4*/
          goto LABEL_50; /*0x5f56b4*/
        v22 = dword_BEDA4C; /*0x5f56b6*/
      }
      else
      {
        *(this + 100) = -1; /*0x5f56db*/
        sub_5FDD2D(a1: 15, a2: &a2 + 3); /*0x5f56ee*/
        sub_5FDDA6(a1: 15, a2: &v60); /*0x5f56ff*/
        sub_5FDC2A(a1: 15, a2: &v58); /*0x5f5710*/
        sub_5FDD2D(a1: 60, a2: &v59); /*0x5f5721*/
        sub_5FDDA6(a1: 60, a2: &v57); /*0x5f5732*/
        v23 = *(this + 124); /*0x5f5737*/
        if ( v23 != 0 ) /*0x5f573f*/
          CFadeWnd::Close(this: v23, a2: 0); /*0x5f5742*/
        if ( *(this + 96) == 1 ) /*0x5f574e*/
        {
          if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 ) /*0x5f5758*/
          {
            CWnd::Destroy(a1: TSingleton<CUIWorldSelect>::ms_pInstance); /*0x5f575a*/
            if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 ) /*0x5f5766*/
              (**(TSingleton<CUIWorldSelect>::ms_pInstance + 8))( /*0x5f576f*/
                a1: TSingleton<CUIWorldSelect>::ms_pInstance + 8,
                a2: 1);
          }
          v56 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 400); /*0x5f5782*/
          v65 = 0; /*0x5f5787*/
          if ( v56 != 0 ) /*0x5f578a*/
            sub_6246CE(a1: this); /*0x5f578d*/
          v65 = -1; /*0x5f5792*/
          COutPacket::COutPacket(a1: 4); /*0x5f579a*/
          v65 = 1; /*0x5f57a9*/
          CClientSocket::SendPacket(this: dword_BE7914, a2: v53); /*0x5f57b0*/
          v65 = -1; /*0x5f57b5*/
          ZArray<unsigned char>::RemoveAll(a1: v54); /*0x5f57bc*/
        }
        if ( v64 == 5 ) /*0x5f57c5*/
          CLogin::ResetVAC(a1: this); /*0x5f57c9*/
        if ( TSingleton<CWndMan>::ms_pInstance == 0 ) /*0x5f57d6*/
          goto LABEL_50; /*0x5f57d6*/
        v22 = dword_BEDA68; /*0x5f57dc*/
        if ( dword_BEDA68 != 0 ) /*0x5f57e3*/
          goto LABEL_49; /*0x5f57e3*/
        v22 = TSingleton<CUIWorldSelect>::ms_pInstance; /*0x5f57e9*/
      }
      if ( v22 != 0 )
LABEL_49:
        CWndMan::SetFocus(a1: v22 != 0 ? v22 + 4 : 0);
LABEL_50:
      *(this + 133) = 0; /*0x5f56d0*/
LABEL_69:
      StepHeight = CLogin::GetStepHeight(this, a2: v64); /*0x5f5826*/
      v36 = *(this + 90); /*0x5f5830*/
      a2 = StepHeight; /*0x5f5838*/
      v63 = CLogin::GetStepHeight(this, a2: v36); /*0x5f5844*/
      if ( v64 == 5 ) /*0x5f5847*/
        a2 = CLogin::GetStepHeight(this, a2: 1); /*0x5f5852*/
      if ( *(this + 90) == 5 ) /*0x5f585c*/
        v63 = CLogin::GetStepHeight(this, a2: 1); /*0x5f5867*/
      v42 = dbl_AF6B38; /*0x5f5876*/
      v41 = 0; /*0x5f587f*/
      v43 = a2; /*0x5f588b*/
      v44 = 0.0; /*0x5f5893*/
      v47 = 0; /*0x5f589a*/
      v62 = 7000 * (v63 - a2) / v62 / 2; /*0x5f58a8*/
      v46 = a1; /*0x5f58b1*/
      v52 = *(this + 91); /*0x5f58ba*/
      v45 = v62; /*0x5f58bd*/
      v48 = dbl_AF6B38; /*0x5f58c0*/
      v49 = v63; /*0x5f58c6*/
      v50 = 0.0; /*0x5f58cb*/
      v51 = 0.0; /*0x5f58d0*/
      if ( dword_BF14EC == 0 ) /*0x5f58d3*/
        _com_issue_error(a1: -2147467261); /*0x5f58da*/
      v27 = *IWzGr2D::GetCenter(this: dword_BF14EC, &a2); /*0x5f58ee*/
      v65 = 7; /*0x5f58fa*/
      sub_43E7C7(a1: v27, a2: 2, a3: &v41); /*0x5f5901*/
      v65 = -1; /*0x5f5909*/
      if ( a2 != 0 ) /*0x5f5912*/
        (*(*a2 + 8))(a1: a2); /*0x5f5917*/
      v36 = SP_1236_SCROLLUP; /*0x5f591d*/
      p_a1 = &a1; /*0x5f5922*/
      v28 = StringPool::GetInstance(); /*0x5f5923*/
      v36 = *StringPool::GetStringW(this: v28, a2: p_a1, a3: v36); /*0x5f592f*/
      v65 = 8; /*0x5f5931*/
      play_ui_sound(a1: v36); /*0x5f5938*/
      v65 = -1; /*0x5f593d*/
      ZXString<unsigned short>::_Release(&a1); /*0x5f5945*/
      return; /*0x5f5945*/
    }
    v36 = SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID; /*0x5f5482*/
    goto LABEL_16; /*0x5f5483*/
  }
}
```

## CLogin::OnStepChanged @ 0x5f5962

```c
_DWORD *__thiscall CLogin::OnStepChanged(_DWORD *this)
{
  int v2; // eax
  int v3; // eax
  int v4; // eax
  int v5; // eax
  int v6; // eax
  _DWORD *result; // eax
  CFadeWnd *v8; // eax
  CFadeWnd *v9; // eax
  _DWORD *v10; // esi
  CFadeWnd *v11; // eax
  CFadeWnd *v12; // eax
  CFadeWnd *v13; // eax
  CFadeWnd *v14; // eax
  CFadeWnd *v15; // ecx
  _DWORD *v16; // ebx
  CFadeWnd *v17; // eax
  CFadeWnd *v18; // eax
  CFadeWnd *v19; // eax
  CFadeWnd *v20; // eax
  CFadeWnd *v21; // eax
  CFadeWnd *v22; // eax
  CFadeWnd *v23; // eax
  CFadeWnd *v24; // eax

  v2 = *(this + 90); /*0x5f5972*/
  if ( v2 != 0 ) /*0x5f5980*/
  {
    v3 = v2 - 1; /*0x5f5986*/
    if ( v3 == 0 ) /*0x5f5987*/
    {
      if ( *dword_BEDA60 != 0 ) /*0x5f60d7*/
      {
        CWnd::Destroy(a1: *dword_BEDA60); /*0x5f60d9*/
        if ( *dword_BEDA60 != 0 ) /*0x5f60e5*/
          (**(*dword_BEDA60 + 8))(a1: *dword_BEDA60 + 8, a2: 1); /*0x5f60ed*/
      }
      if ( dword_BEDA54 != 0 ) /*0x5f60f7*/
      {
        CWnd::Destroy(a1: dword_BEDA54); /*0x5f60f9*/
        if ( dword_BEDA54 != 0 ) /*0x5f6105*/
          (**(dword_BEDA54 + 8))(a1: dword_BEDA54 + 8, a2: 1); /*0x5f610d*/
      }
      if ( dword_BEDA4C != 0 ) /*0x5f6117*/
      {
        CWnd::Destroy(a1: dword_BEDA4C); /*0x5f6119*/
        if ( dword_BEDA4C != 0 ) /*0x5f6125*/
          (**(dword_BEDA4C + 8))(a1: dword_BEDA4C + 8, a2: 1); /*0x5f612d*/
      }
      if ( dword_BEDA50 != 0 ) /*0x5f6137*/
      {
        CWnd::Destroy(a1: dword_BEDA50); /*0x5f6139*/
        if ( dword_BEDA50 != 0 ) /*0x5f6145*/
          (**(dword_BEDA50 + 8))(a1: dword_BEDA50 + 8, a2: 1); /*0x5f614d*/
      }
      if ( dword_BEDA48 != 0 ) /*0x5f6157*/
      {
        CWnd::Destroy(a1: dword_BEDA48); /*0x5f6159*/
        if ( dword_BEDA48 != 0 ) /*0x5f6165*/
          (**(dword_BEDA48 + 8))(a1: dword_BEDA48 + 8, a2: 1); /*0x5f616d*/
      }
      if ( dword_BEDA44 != 0 ) /*0x5f6177*/
      {
        CWnd::Destroy(a1: dword_BEDA44); /*0x5f6179*/
        if ( dword_BEDA44 != 0 ) /*0x5f6185*/
          (**(dword_BEDA44 + 8))(a1: dword_BEDA44 + 8, a2: 1); /*0x5f618d*/
      }
      if ( dword_BEDA40 != 0 ) /*0x5f6197*/
      {
        CWnd::Destroy(a1: dword_BEDA40); /*0x5f6199*/
        if ( dword_BEDA40 != 0 ) /*0x5f61a5*/
          (**(dword_BEDA40 + 8))(a1: dword_BEDA40 + 8, a2: 1); /*0x5f61ad*/
      }
      if ( dword_BEDA38 != 0 ) /*0x5f61b7*/
      {
        CWnd::Destroy(a1: dword_BEDA38); /*0x5f61b9*/
        if ( dword_BEDA38 != 0 ) /*0x5f61c5*/
          (**(dword_BEDA38 + 8))(a1: dword_BEDA38 + 8, a2: 1); /*0x5f61cd*/
      }
      if ( dword_BEDA3C != 0 ) /*0x5f61d7*/
      {
        CWnd::Destroy(a1: dword_BEDA3C); /*0x5f61d9*/
        if ( dword_BEDA3C != 0 ) /*0x5f61e5*/
          (**(dword_BEDA3C + 8))(a1: dword_BEDA3C + 8, a2: 1); /*0x5f61ed*/
      }
      if ( dword_BEDA34 != 0 ) /*0x5f61f7*/
      {
        CWnd::Destroy(a1: dword_BEDA34); /*0x5f61f9*/
        if ( dword_BEDA34 != 0 ) /*0x5f6205*/
          (**(dword_BEDA34 + 8))(a1: dword_BEDA34 + 8, a2: 1); /*0x5f620d*/
      }
      if ( dword_BEDA6C != 0 ) /*0x5f6217*/
      {
        CWnd::Destroy(a1: dword_BEDA6C); /*0x5f6219*/
        if ( dword_BEDA6C != 0 ) /*0x5f6225*/
          (**(dword_BEDA6C + 8))(a1: dword_BEDA6C + 8, a2: 1); /*0x5f622d*/
      }
      sub_5FC313(a1: 0); /*0x5f6232*/
      v24 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 232); /*0x5f6241*/
      if ( v24 != 0 ) /*0x5f624e*/
      {
        v9 = sub_60D7F8(this: v24, a2: this, a3: 1); /*0x5f6255*/
        goto LABEL_31; /*0x5f6255*/
      }
      goto LABEL_30; /*0x5f624e*/
    }
    v4 = v3 - 1; /*0x5f598d*/
    if ( v4 == 0 ) /*0x5f598e*/
    {
      if ( dword_BEDA68 != 0 ) /*0x5f5ed4*/
      {
        CWnd::Destroy(a1: dword_BEDA68); /*0x5f5ed6*/
        if ( dword_BEDA68 != 0 ) /*0x5f5ee2*/
          (**(dword_BEDA68 + 8))(a1: dword_BEDA68 + 8, a2: 1); /*0x5f5eea*/
      }
      if ( *dword_BEDA60 != 0 ) /*0x5f5ef4*/
      {
        CWnd::Destroy(a1: *dword_BEDA60); /*0x5f5ef6*/
        if ( *dword_BEDA60 != 0 ) /*0x5f5f02*/
          (**(*dword_BEDA60 + 8))(a1: *dword_BEDA60 + 8, a2: 1); /*0x5f5f0a*/
      }
      if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 ) /*0x5f5f14*/
      {
        CWnd::Destroy(a1: TSingleton<CUIWorldSelect>::ms_pInstance); /*0x5f5f16*/
        if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 ) /*0x5f5f22*/
          (**(TSingleton<CUIWorldSelect>::ms_pInstance + 8))(a1: TSingleton<CUIWorldSelect>::ms_pInstance + 8, a2: 1); /*0x5f5f2a*/
      }
      if ( TSingleton<CUIChannelSelect>::ms_pInstance != 0 ) /*0x5f5f34*/
      {
        CWnd::Destroy(a1: TSingleton<CUIChannelSelect>::ms_pInstance); /*0x5f5f36*/
        if ( TSingleton<CUIChannelSelect>::ms_pInstance != 0 ) /*0x5f5f42*/
          (**(TSingleton<CUIChannelSelect>::ms_pInstance + 8))( /*0x5f5f4a*/
            a1: TSingleton<CUIChannelSelect>::ms_pInstance + 8,
            a2: 1);
      }
      if ( dword_BEDA48 != 0 ) /*0x5f5f54*/
      {
        CWnd::Destroy(a1: dword_BEDA48); /*0x5f5f56*/
        if ( dword_BEDA48 != 0 ) /*0x5f5f62*/
          (**(dword_BEDA48 + 8))(a1: dword_BEDA48 + 8, a2: 1); /*0x5f5f6a*/
      }
      if ( dword_BEDA44 != 0 ) /*0x5f5f74*/
      {
        CWnd::Destroy(a1: dword_BEDA44); /*0x5f5f76*/
        if ( dword_BEDA44 != 0 ) /*0x5f5f82*/
          (**(dword_BEDA44 + 8))(a1: dword_BEDA44 + 8, a2: 1); /*0x5f5f8a*/
      }
      if ( dword_BEDA40 != 0 ) /*0x5f5f94*/
      {
        CWnd::Destroy(a1: dword_BEDA40); /*0x5f5f96*/
        if ( dword_BEDA40 != 0 ) /*0x5f5fa2*/
          (**(dword_BEDA40 + 8))(a1: dword_BEDA40 + 8, a2: 1); /*0x5f5faa*/
      }
      if ( dword_BEDA34 != 0 ) /*0x5f5fb4*/
      {
        CWnd::Destroy(a1: dword_BEDA34); /*0x5f5fb6*/
        if ( dword_BEDA34 != 0 ) /*0x5f5fc2*/
          (**(dword_BEDA34 + 8))(a1: dword_BEDA34 + 8, a2: 1); /*0x5f5fca*/
      }
      if ( dword_BEDA6C != 0 ) /*0x5f5fd4*/
      {
        CWnd::Destroy(a1: dword_BEDA6C); /*0x5f5fd6*/
        if ( dword_BEDA6C != 0 ) /*0x5f5fe2*/
          (**(dword_BEDA6C + 8))(a1: dword_BEDA6C + 8, a2: 1); /*0x5f5fea*/
      }
      if ( dword_BEDA38 != 0 ) /*0x5f5ff4*/
      {
        CWnd::Destroy(a1: dword_BEDA38); /*0x5f5ff6*/
        if ( dword_BEDA38 != 0 ) /*0x5f6002*/
          (**(dword_BEDA38 + 8))(a1: dword_BEDA38 + 8, a2: 1); /*0x5f600a*/
      }
      if ( dword_BEDA3C != 0 ) /*0x5f6014*/
      {
        CWnd::Destroy(a1: dword_BEDA3C); /*0x5f6016*/
        if ( dword_BEDA3C != 0 ) /*0x5f6022*/
          (**(dword_BEDA3C + 8))(a1: dword_BEDA3C + 8, a2: 1); /*0x5f602a*/
      }
      sub_5FC313(a1: *(this + 122)); /*0x5f6035*/
      v20 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 232); /*0x5f6047*/
      if ( v20 != 0 ) /*0x5f6057*/
        v21 = sub_60D7F8(this: v20, a2: this, a3: 2); /*0x5f605f*/
      else
        v21 = 0; /*0x5f6066*/
      sub_5FD826(a1: v21); /*0x5f6076*/
      if ( *(this + 124) == 0 ) /*0x5f6085*/
      {
        v22 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 232); /*0x5f608a*/
        if ( v22 != 0 ) /*0x5f6097*/
          v23 = sub_60D7F8(this: v22, a2: this, a3: -1); /*0x5f60a0*/
        else
          v23 = 0; /*0x5f60a7*/
        sub_5FD826(a1: v23); /*0x5f60b7*/
      }
      result = this; /*0x5f60bc*/
      v10 = this + 117; /*0x5f60bf*/
      goto LABEL_32; /*0x5f60c7*/
    }
    v5 = v4 - 1; /*0x5f5994*/
    if ( v5 != 0 ) /*0x5f5995*/
    {
      v6 = v5 - 1; /*0x5f599b*/
      if ( v6 != 0 ) /*0x5f599c*/
      {
        result = (v6 - 1); /*0x5f59a2*/
        if ( result != 0 ) /*0x5f59a3*/
          return result; /*0x5f59a3*/
        if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 ) /*0x5f59b4*/
        {
          CWnd::Destroy(a1: TSingleton<CUIWorldSelect>::ms_pInstance); /*0x5f59b6*/
          if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 ) /*0x5f59c2*/
            (**(TSingleton<CUIWorldSelect>::ms_pInstance + 8))(a1: TSingleton<CUIWorldSelect>::ms_pInstance + 8, a2: 1); /*0x5f59ca*/
        }
        if ( TSingleton<CUIChannelSelect>::ms_pInstance != 0 ) /*0x5f59d4*/
        {
          CWnd::Destroy(a1: TSingleton<CUIChannelSelect>::ms_pInstance); /*0x5f59d6*/
          if ( TSingleton<CUIChannelSelect>::ms_pInstance != 0 ) /*0x5f59e2*/
            (**(TSingleton<CUIChannelSelect>::ms_pInstance + 8))( /*0x5f59ea*/
              a1: TSingleton<CUIChannelSelect>::ms_pInstance + 8,
              a2: 1);
        }
        if ( dword_BEDA54 != 0 ) /*0x5f59f4*/
        {
          CWnd::Destroy(a1: dword_BEDA54); /*0x5f59f6*/
          if ( dword_BEDA54 != 0 ) /*0x5f5a02*/
            (**(dword_BEDA54 + 8))(a1: dword_BEDA54 + 8, a2: 1); /*0x5f5a0a*/
        }
        if ( *dword_BEDA60 != 0 ) /*0x5f5a14*/
        {
          CWnd::Destroy(a1: *dword_BEDA60); /*0x5f5a16*/
          if ( *dword_BEDA60 != 0 ) /*0x5f5a22*/
            (**(*dword_BEDA60 + 8))(a1: *dword_BEDA60 + 8, a2: 1); /*0x5f5a2a*/
        }
        if ( dword_BEDA4C != 0 ) /*0x5f5a34*/
        {
          CWnd::Destroy(a1: dword_BEDA4C); /*0x5f5a36*/
          if ( dword_BEDA4C != 0 ) /*0x5f5a42*/
            (**(dword_BEDA4C + 8))(a1: dword_BEDA4C + 8, a2: 1); /*0x5f5a4a*/
        }
        if ( dword_BEDA50 != 0 ) /*0x5f5a54*/
        {
          CWnd::Destroy(a1: dword_BEDA50); /*0x5f5a56*/
          if ( dword_BEDA50 != 0 ) /*0x5f5a62*/
            (**(dword_BEDA50 + 8))(a1: dword_BEDA50 + 8, a2: 1); /*0x5f5a6a*/
        }
        if ( dword_BEDA34 != 0 ) /*0x5f5a74*/
        {
          CWnd::Destroy(a1: dword_BEDA34); /*0x5f5a76*/
          if ( dword_BEDA34 != 0 ) /*0x5f5a82*/
            (**(dword_BEDA34 + 8))(a1: dword_BEDA34 + 8, a2: 1); /*0x5f5a8a*/
        }
        sub_5FC313(a1: 0); /*0x5f5a8f*/
        v8 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 232); /*0x5f5a9e*/
        if ( v8 != 0 ) /*0x5f5aaf*/
        {
          v9 = sub_60D7F8(this: v8, a2: this, a3: 5); /*0x5f5ab6*/
LABEL_31:
          result = sub_5FD826(a1: v9); /*0x5f5abf*/
          v10 = this + 117; /*0x5f5acf*/
LABEL_32:
          if ( v10[1] != 0 ) /*0x5f5ad8*/
          {
            result = sub_428C15(a1: 0); /*0x5f5ae1*/
            v10[1] = 0; /*0x5f5ae6*/
          }
          return result; /*0x5f5ae9*/
        }
LABEL_30:
        v9 = 0; /*0x5f5abd*/
        goto LABEL_31; /*0x5f5abd*/
      }
      if ( dword_BEDA68 != 0 ) /*0x5f5af9*/
      {
        CWnd::Destroy(a1: dword_BEDA68); /*0x5f5afb*/
        if ( dword_BEDA68 != 0 ) /*0x5f5b07*/
          (**(dword_BEDA68 + 8))(a1: dword_BEDA68 + 8, a2: 1); /*0x5f5b0f*/
      }
      if ( *dword_BEDA60 != 0 ) /*0x5f5b19*/
      {
        CWnd::Destroy(a1: *dword_BEDA60); /*0x5f5b1b*/
        if ( *dword_BEDA60 != 0 ) /*0x5f5b27*/
          (**(*dword_BEDA60 + 8))(a1: *dword_BEDA60 + 8, a2: 1); /*0x5f5b2f*/
      }
      if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 ) /*0x5f5b39*/
      {
        CWnd::Destroy(a1: TSingleton<CUIWorldSelect>::ms_pInstance); /*0x5f5b3b*/
        if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 ) /*0x5f5b47*/
          (**(TSingleton<CUIWorldSelect>::ms_pInstance + 8))(a1: TSingleton<CUIWorldSelect>::ms_pInstance + 8, a2: 1); /*0x5f5b4f*/
      }
      if ( TSingleton<CUIChannelSelect>::ms_pInstance != 0 ) /*0x5f5b59*/
      {
        CWnd::Destroy(a1: TSingleton<CUIChannelSelect>::ms_pInstance); /*0x5f5b5b*/
        if ( TSingleton<CUIChannelSelect>::ms_pInstance != 0 ) /*0x5f5b67*/
          (**(TSingleton<CUIChannelSelect>::ms_pInstance + 8))( /*0x5f5b6f*/
            a1: TSingleton<CUIChannelSelect>::ms_pInstance + 8,
            a2: 1);
      }
      if ( dword_BEDA54 != 0 ) /*0x5f5b79*/
      {
        CWnd::Destroy(a1: dword_BEDA54); /*0x5f5b7b*/
        if ( dword_BEDA54 != 0 ) /*0x5f5b87*/
          (**(dword_BEDA54 + 8))(a1: dword_BEDA54 + 8, a2: 1); /*0x5f5b8f*/
      }
      if ( dword_BEDA4C != 0 ) /*0x5f5b99*/
      {
        CWnd::Destroy(a1: dword_BEDA4C); /*0x5f5b9b*/
        if ( dword_BEDA4C != 0 ) /*0x5f5ba7*/
          (**(dword_BEDA4C + 8))(a1: dword_BEDA4C + 8, a2: 1); /*0x5f5baf*/
      }
      if ( dword_BEDA50 != 0 ) /*0x5f5bb9*/
      {
        CWnd::Destroy(a1: dword_BEDA50); /*0x5f5bbb*/
        if ( dword_BEDA50 != 0 ) /*0x5f5bc7*/
          (**(dword_BEDA50 + 8))(a1: dword_BEDA50 + 8, a2: 1); /*0x5f5bcf*/
      }
      if ( dword_BEDA6C != 0 ) /*0x5f5bd9*/
      {
        CWnd::Destroy(a1: dword_BEDA6C); /*0x5f5bdb*/
        if ( dword_BEDA6C != 0 ) /*0x5f5be7*/
          (**(dword_BEDA6C + 8))(a1: dword_BEDA6C + 8, a2: 1); /*0x5f5bef*/
      }
      result = this; /*0x5f5bf1*/
      if ( *(this + 122) == 0 ) /*0x5f5c06*/
      {
        v11 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 232); /*0x5f5c0b*/
        if ( v11 != 0 ) /*0x5f5c1c*/
          v12 = sub_60D7F8(this: v11, a2: this, a3: 3); /*0x5f5c25*/
        else
          v12 = 0; /*0x5f5c2c*/
        sub_5FD826(a1: v12); /*0x5f5c3c*/
        result = this; /*0x5f5c41*/
      }
      if ( result[124] == 0 ) /*0x5f5c4a*/
      {
        v13 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 232); /*0x5f5c4f*/
        if ( v13 != 0 ) /*0x5f5c60*/
          v14 = sub_60D7F8(this: v13, a2: this, a3: -1); /*0x5f5c69*/
        else
          v14 = 0; /*0x5f5c70*/
        sub_5FD826(a1: v14); /*0x5f5c80*/
        result = this; /*0x5f5c85*/
      }
      result[142] = 0; /*0x5f5c88*/
      result[143] = 1; /*0x5f5c8e*/
    }
    else
    {
      if ( dword_BEDA68 != 0 ) /*0x5f5ca8*/
      {
        CWnd::Destroy(a1: dword_BEDA68); /*0x5f5caa*/
        if ( dword_BEDA68 != 0 ) /*0x5f5cb6*/
          (**(dword_BEDA68 + 8))(a1: dword_BEDA68 + 8, a2: 1); /*0x5f5cbe*/
      }
      if ( *dword_BEDA60 != 0 ) /*0x5f5cc8*/
      {
        CWnd::Destroy(a1: *dword_BEDA60); /*0x5f5cca*/
        if ( *dword_BEDA60 != 0 ) /*0x5f5cd6*/
          (**(*dword_BEDA60 + 8))(a1: *dword_BEDA60 + 8, a2: 1); /*0x5f5cde*/
      }
      if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 ) /*0x5f5ce8*/
      {
        CWnd::Destroy(a1: TSingleton<CUIWorldSelect>::ms_pInstance); /*0x5f5cea*/
        if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 ) /*0x5f5cf6*/
          (**(TSingleton<CUIWorldSelect>::ms_pInstance + 8))(a1: TSingleton<CUIWorldSelect>::ms_pInstance + 8, a2: 1); /*0x5f5cfe*/
      }
      if ( TSingleton<CUIChannelSelect>::ms_pInstance != 0 ) /*0x5f5d08*/
      {
        CWnd::Destroy(a1: TSingleton<CUIChannelSelect>::ms_pInstance); /*0x5f5d0a*/
        if ( TSingleton<CUIChannelSelect>::ms_pInstance != 0 ) /*0x5f5d16*/
          (**(TSingleton<CUIChannelSelect>::ms_pInstance + 8))( /*0x5f5d1e*/
            a1: TSingleton<CUIChannelSelect>::ms_pInstance + 8,
            a2: 1);
      }
      if ( dword_BEDA54 != 0 ) /*0x5f5d28*/
      {
        CWnd::Destroy(a1: dword_BEDA54); /*0x5f5d2a*/
        if ( dword_BEDA54 != 0 ) /*0x5f5d36*/
          (**(dword_BEDA54 + 8))(a1: dword_BEDA54 + 8, a2: 1); /*0x5f5d3e*/
      }
      if ( dword_BEDA4C != 0 ) /*0x5f5d48*/
      {
        CWnd::Destroy(a1: dword_BEDA4C); /*0x5f5d4a*/
        if ( dword_BEDA4C != 0 ) /*0x5f5d56*/
          (**(dword_BEDA4C + 8))(a1: dword_BEDA4C + 8, a2: 1); /*0x5f5d5e*/
      }
      if ( dword_BEDA50 != 0 ) /*0x5f5d68*/
      {
        CWnd::Destroy(a1: dword_BEDA50); /*0x5f5d6a*/
        if ( dword_BEDA50 != 0 ) /*0x5f5d76*/
          (**(dword_BEDA50 + 8))(a1: dword_BEDA50 + 8, a2: 1); /*0x5f5d7e*/
      }
      if ( dword_BEDA38 != 0 ) /*0x5f5d88*/
      {
        CWnd::Destroy(a1: dword_BEDA38); /*0x5f5d8a*/
        if ( dword_BEDA38 != 0 ) /*0x5f5d96*/
          (**(dword_BEDA38 + 8))(a1: dword_BEDA38 + 8, a2: 1); /*0x5f5d9e*/
      }
      if ( dword_BEDA3C != 0 ) /*0x5f5da8*/
      {
        CWnd::Destroy(a1: dword_BEDA3C); /*0x5f5daa*/
        if ( dword_BEDA3C != 0 ) /*0x5f5db6*/
          (**(dword_BEDA3C + 8))(a1: dword_BEDA3C + 8, a2: 1); /*0x5f5dbe*/
      }
      result = sub_5FC313(a1: *(this + 122)); /*0x5f5dcf*/
      if ( *(this + 122) != 0 ) /*0x5f5de1*/
      {
        v16 = this; /*0x5f5e1d*/
      }
      else
      {
        v15 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 232); /*0x5f5deb*/
        v16 = this; /*0x5f5dfa*/
        if ( v15 != 0 ) /*0x5f5df8*/
          v17 = sub_60D7F8(this: v15, a2: this, a3: 3); /*0x5f5dff*/
        else
          v17 = 0; /*0x5f5e09*/
        result = sub_5FD826(a1: v17); /*0x5f5e16*/
      }
      if ( dword_BEDA48 != 0 ) /*0x5f5e28*/
      {
        CWnd::Destroy(a1: dword_BEDA48); /*0x5f5e2a*/
        result = dword_BEDA48; /*0x5f5e2f*/
        if ( dword_BEDA48 != 0 ) /*0x5f5e36*/
          result = (**(dword_BEDA48 + 8))(a1: dword_BEDA48 + 8, a2: 1); /*0x5f5e3f*/
      }
      if ( dword_BEDA44 != 0 ) /*0x5f5e49*/
      {
        CWnd::Destroy(a1: dword_BEDA44); /*0x5f5e4b*/
        result = dword_BEDA44; /*0x5f5e50*/
        if ( dword_BEDA44 != 0 ) /*0x5f5e57*/
          result = (**(dword_BEDA44 + 8))(a1: dword_BEDA44 + 8, a2: 1); /*0x5f5e60*/
      }
      if ( dword_BEDA40 != 0 ) /*0x5f5e6a*/
      {
        CWnd::Destroy(a1: dword_BEDA40); /*0x5f5e6c*/
        result = dword_BEDA40; /*0x5f5e71*/
        if ( dword_BEDA40 != 0 ) /*0x5f5e78*/
          result = (**(dword_BEDA40 + 8))(a1: dword_BEDA40 + 8, a2: 1); /*0x5f5e81*/
      }
      if ( v16[124] == 0 ) /*0x5f5e8a*/
      {
        v18 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 232); /*0x5f5e93*/
        if ( v18 != 0 ) /*0x5f5ea4*/
          v19 = sub_60D7F8(this: v18, a2: v16, a3: -1); /*0x5f5eab*/
        else
          v19 = 0; /*0x5f5eb2*/
        return sub_5FD826(a1: v19); /*0x5f5ebf*/
      }
    }
  }
  else
  {
    if ( dword_BEDA68 != 0 ) /*0x5f6265*/
    {
      CWnd::Destroy(a1: dword_BEDA68); /*0x5f6267*/
      if ( dword_BEDA68 != 0 ) /*0x5f6273*/
        (**(dword_BEDA68 + 8))(a1: dword_BEDA68 + 8, a2: 1); /*0x5f627b*/
    }
    if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 ) /*0x5f6285*/
    {
      CWnd::Destroy(a1: TSingleton<CUIWorldSelect>::ms_pInstance); /*0x5f6287*/
      if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 ) /*0x5f6293*/
        (**(TSingleton<CUIWorldSelect>::ms_pInstance + 8))(a1: TSingleton<CUIWorldSelect>::ms_pInstance + 8, a2: 1); /*0x5f629b*/
    }
    if ( TSingleton<CUIChannelSelect>::ms_pInstance != 0 ) /*0x5f62a5*/
    {
      CWnd::Destroy(a1: TSingleton<CUIChannelSelect>::ms_pInstance); /*0x5f62a7*/
      if ( TSingleton<CUIChannelSelect>::ms_pInstance != 0 ) /*0x5f62b3*/
        (**(TSingleton<CUIChannelSelect>::ms_pInstance + 8))(a1: TSingleton<CUIChannelSelect>::ms_pInstance + 8, a2: 1); /*0x5f62bb*/
    }
    if ( dword_BEDA54 != 0 ) /*0x5f62c5*/
    {
      CWnd::Destroy(a1: dword_BEDA54); /*0x5f62c7*/
      if ( dword_BEDA54 != 0 ) /*0x5f62d3*/
        (**(dword_BEDA54 + 8))(a1: dword_BEDA54 + 8, a2: 1); /*0x5f62db*/
    }
    if ( dword_BEDA4C != 0 ) /*0x5f62e5*/
    {
      CWnd::Destroy(a1: dword_BEDA4C); /*0x5f62e7*/
      if ( dword_BEDA4C != 0 ) /*0x5f62f3*/
        (**(dword_BEDA4C + 8))(a1: dword_BEDA4C + 8, a2: 1); /*0x5f62fb*/
    }
    if ( dword_BEDA50 != 0 ) /*0x5f6305*/
    {
      CWnd::Destroy(a1: dword_BEDA50); /*0x5f6307*/
      if ( dword_BEDA50 != 0 ) /*0x5f6313*/
        (**(dword_BEDA50 + 8))(a1: dword_BEDA50 + 8, a2: 1); /*0x5f631b*/
    }
    if ( dword_BEDA48 != 0 ) /*0x5f6325*/
    {
      CWnd::Destroy(a1: dword_BEDA48); /*0x5f6327*/
      if ( dword_BEDA48 != 0 ) /*0x5f6333*/
        (**(dword_BEDA48 + 8))(a1: dword_BEDA48 + 8, a2: 1); /*0x5f633b*/
    }
    if ( dword_BEDA44 != 0 ) /*0x5f6345*/
    {
      CWnd::Destroy(a1: dword_BEDA44); /*0x5f6347*/
      if ( dword_BEDA44 != 0 ) /*0x5f6353*/
        (**(dword_BEDA44 + 8))(a1: dword_BEDA44 + 8, a2: 1); /*0x5f635b*/
    }
    if ( dword_BEDA40 != 0 ) /*0x5f6365*/
    {
      CWnd::Destroy(a1: dword_BEDA40); /*0x5f6367*/
      if ( dword_BEDA40 != 0 ) /*0x5f6373*/
        (**(dword_BEDA40 + 8))(a1: dword_BEDA40 + 8, a2: 1); /*0x5f637b*/
    }
    if ( dword_BEDA38 != 0 ) /*0x5f6385*/
    {
      CWnd::Destroy(a1: dword_BEDA38); /*0x5f6387*/
      if ( dword_BEDA38 != 0 ) /*0x5f6393*/
        (**(dword_BEDA38 + 8))(a1: dword_BEDA38 + 8, a2: 1); /*0x5f639b*/
    }
    if ( dword_BEDA3C != 0 ) /*0x5f63a5*/
    {
      CWnd::Destroy(a1: dword_BEDA3C); /*0x5f63a7*/
      if ( dword_BEDA3C != 0 ) /*0x5f63b3*/
        (**(dword_BEDA3C + 8))(a1: dword_BEDA3C + 8, a2: 1); /*0x5f63bb*/
    }
    if ( dword_BEDA34 != 0 ) /*0x5f63c5*/
    {
      CWnd::Destroy(a1: dword_BEDA34); /*0x5f63c7*/
      if ( dword_BEDA34 != 0 ) /*0x5f63d3*/
        (**(dword_BEDA34 + 8))(a1: dword_BEDA34 + 8, a2: 1); /*0x5f63db*/
    }
    if ( dword_BEDA6C != 0 ) /*0x5f63e5*/
    {
      CWnd::Destroy(a1: dword_BEDA6C); /*0x5f63e7*/
      if ( dword_BEDA6C != 0 ) /*0x5f63f3*/
        (**(dword_BEDA6C + 8))(a1: dword_BEDA6C + 8, a2: 1); /*0x5f63fb*/
    }
    sub_5FC313(a1: 0); /*0x5f6400*/
    result = ZRef<CAvatar>::Clear(a1: this + 117); /*0x5f640b*/
    *(this + 133) = 0; /*0x5f6410*/
  }
  return result; /*0x5f6416*/
}
```

## CLogin::OnNewCharStep @ 0x5f6425

```c
void __thiscall CLogin::OnNewCharStep(CLogin *this)
{
  if ( *(32 * *(*dword_BE7918 + 8276) + *(this + 99) + 24) != 0 ) /*0x5f6440*/
  {
    CLoginUtilDlg::Notice(a1: 36, a2: 0); /*0x5f6445*/
  }
  else if ( *(684 * *(this + 95) + *(this + 101) - 684) != 0 ) /*0x5f6465*/
  {
    CLoginUtilDlg::Error(a1: 9, a2: this + 500); /*0x5f6470*/
  }
  else
  {
    CLogin::ChangeStep(this, a2: 0xFFFFFFFF); /*0x5f647b*/
  }
}
```

## CLogin::OnSelectedCharChanged @ 0x5f6482

```c
void __thiscall CLogin::OnSelectedCharChanged(unsigned int this, int a2)
{
  int v2; // edi
  int v4; // eax
  void *v5; // eax
  _DWORD *v6; // eax
  int v7; // eax
  wchar_t *v8; // eax
  _DWORD *v9; // eax
  int **v10; // esi
  bool v11; // zf
  int *v12; // eax
  int v13; // ecx
  int v14; // eax
  int v15; // eax
  int v16; // eax
  StringPoolStrings v17; // ecx
  int v18; // ecx
  int v19; // eax
  void *v20; // eax
  _DWORD *v21; // eax
  int v22; // eax
  wchar_t *v23; // eax
  _DWORD *v24; // eax
  int *v25; // ebx
  int v26; // ebx
  int v27; // eax
  int v28; // eax
  VARIANTARG *p_pvarg; // eax
  int v30; // esi
  int v31; // edi
  unsigned int v32; // ecx
  int v33; // ecx
  int v34; // eax
  void *v35; // eax
  _DWORD *Instance; // eax
  int StringW; // eax
  wchar_t *v38; // eax
  int v39; // eax
  int v40; // esi
  int *v41; // ebx
  int v42; // eax
  int v43; // eax
  int v44; // ebx
  int v45; // eax
  int v46; // eax
  int v47; // eax
  _DWORD *v48; // eax
  int v49; // [esp-1Ch] [ebp-64h] BYREF
  int v50; // [esp-18h] [ebp-60h]
  int v51; // [esp-14h] [ebp-5Ch]
  unsigned __int64 v52; // [esp-10h] [ebp-58h] BYREF
  int v53; // [esp-8h] [ebp-50h]
  StringPoolStrings v54; // [esp-4h] [ebp-4Ch]
  VARIANTARG pvarg; // [esp+Ch] [ebp-3Ch] BYREF
  VARIANTARG pvargDest; // [esp+1Ch] [ebp-2Ch] BYREF
  unsigned __int64 *v57; // [esp+2Ch] [ebp-1Ch]
  int *v58; // [esp+30h] [ebp-18h] BYREF
  int v59; // [esp+34h] [ebp-14h] BYREF
  int v60; // [esp+38h] [ebp-10h] BYREF
  int v61; // [esp+44h] [ebp-4h]

  v2 = a2; /*0x5f6492*/
  if ( a2 >= 0 ) /*0x5f649b*/
  {
    if ( *(this + 360) == 5 ) /*0x5f64c1*/
    {
      v30 = 117 * (a2 % 3) - 199; /*0x5f674d*/
      v31 = 117 * (a2 / 3) - 140; /*0x5f6758*/
      sub_428712(a1: 0); /*0x5f675e*/
      v54 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f6763*/
      v53 = 255; /*0x5f6765*/
      v52 = v32 | 0xA00000000LL; /*0x5f676c*/
      v57 = &v52; /*0x5f676f*/
      sub_432C10(a1: 0); /*0x5f6774*/
      v51 = v31; /*0x5f6779*/
      v50 = v30; /*0x5f677a*/
      v49 = v33; /*0x5f677b*/
      v58 = &v49; /*0x5f677e*/
      v61 = 12; /*0x5f6782*/
      v34 = sub_5FD76D(); /*0x5f6789*/
      v35 = sub_444FB6(a1: v34); /*0x5f6790*/
      IWzGr2D::GetCenter(this: v35, a2: &v49); /*0x5f6797*/
      LOBYTE(v61) = 13; /*0x5f67a5*/
      Instance = StringPool::GetInstance(); /*0x5f67a9*/
      StringW = StringPool::GetStringW(this: Instance, a2: &v59, a3: SP_3982_UI_LOGINIMG_VIEWALLCHAR_SELECT); /*0x5f67b0*/
      LOBYTE(v61) = 15; /*0x5f67b9*/
      v38 = sub_41468C(a1: StringW); /*0x5f67bd*/
      v39 = *sub_43E86F( /*0x5f67cf*/
               a1: &a2,
               a2: v38,
               a3: 0,
               a4: v49,
               a5: v50,
               a6: v51,
               a7: v52,
               a8: SHIDWORD(v52),
               a9: v53,
               a10: v54);
      v40 = *(this + 460); /*0x5f67d1*/
      v41 = (this + 460); /*0x5f67d7*/
      LOBYTE(v61) = 16; /*0x5f67df*/
      if ( v40 != v39 ) /*0x5f67e3*/
      {
        *v41 = v39; /*0x5f67e7*/
        sub_432D38(a1: v41); /*0x5f67e9*/
        if ( v40 != 0 ) /*0x5f67f0*/
          (*(*v40 + 8))(a1: v40); /*0x5f67f5*/
      }
      LOBYTE(v61) = 15; /*0x5f67fd*/
      if ( a2 != 0 ) /*0x5f6801*/
        (*(*a2 + 8))(a1: a2); /*0x5f6806*/
      v61 = -1; /*0x5f6809*/
      ZXString<unsigned short>::_Release(a1: &v59); /*0x5f6810*/
      VariantInit(&pvarg); /*0x5f681f*/
      v42 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x5f682b*/
      if ( v42 < 0 ) /*0x5f6834*/
        _com_issue_error(a1: v42); /*0x5f6837*/
      v61 = 17; /*0x5f6840*/
      VariantInit(pvarg: &pvargDest); /*0x5f6847*/
      v43 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc); /*0x5f684e*/
      if ( v43 < 0 ) /*0x5f6857*/
        _com_issue_error(a1: v43); /*0x5f685a*/
      v11 = *v41 == 0; /*0x5f685f*/
      LOBYTE(v61) = 18; /*0x5f6862*/
      if ( v11 ) /*0x5f6866*/
        _com_issue_error(a1: -2147467261); /*0x5f686d*/
      v44 = *v41; /*0x5f6877*/
      v45 = (*(*v44 + 272))( /*0x5f6891*/
              a1: v44,
              a2: 32,
              a3: *&pvargDest.vt,
              a4: pvargDest.decVal.Hi32,
              a5: pvargDest.lVal,
              a6: pvargDest.cyVal.Hi,
              a7: *&pvarg.vt,
              a8: pvarg.decVal.Hi32,
              a9: pvarg.lVal,
              a10: pvarg.cyVal.Hi);
      if ( v45 < 0 ) /*0x5f6899*/
        _com_issue_errorex(a1: v45, a2: v44, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x5f68a2*/
      LOBYTE(v61) = 17; /*0x5f68ab*/
      v46 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x5f68af*/
      if ( v46 < 0 ) /*0x5f68b7*/
        _com_issue_error(a1: v46); /*0x5f68ba*/
      v61 = -1; /*0x5f68bf*/
      p_pvarg = &pvarg; /*0x5f68c3*/
    }
    else
    {
      v54 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f64c7*/
      v53 = 255; /*0x5f64c8*/
      v52 = this | 0xC00614A400000000uLL; /*0x5f64d2*/
      v57 = &v52; /*0x5f64d5*/
      sub_432C10(a1: 0); /*0x5f64d9*/
      v51 = -300; /*0x5f64de*/
      v61 = 0; /*0x5f64eb*/
      v50 = 125 * (v2 % 3) - 140; /*0x5f64f7*/
      v49 = 3; /*0x5f64f8*/
      v59 = v50; /*0x5f64f9*/
      v58 = &v49; /*0x5f64fe*/
      v4 = sub_5FD76D(); /*0x5f6502*/
      v5 = sub_444FB6(a1: v4); /*0x5f6509*/
      IWzGr2D::GetCenter(this: v5, a2: &v49); /*0x5f6510*/
      LOBYTE(v61) = 1; /*0x5f651e*/
      v6 = StringPool::GetInstance(); /*0x5f6522*/
      v7 = StringPool::GetStringW(this: v6, a2: &v60, a3: SP_1313_UI_LOGINIMG_CHARSELECT_EFFECT_0); /*0x5f6529*/
      LOBYTE(v61) = 3; /*0x5f6531*/
      v8 = sub_41468C(a1: v7); /*0x5f6535*/
      v9 = sub_43E86F(a1: &a2, a2: v8, a3: 0, a4: v49, a5: v50, a6: v51, a7: v52, a8: SHIDWORD(v52), a9: v53, a10: v54); /*0x5f653f*/
      v10 = (this + 464); /*0x5f6549*/
      sub_428712(a1: *v9); /*0x5f6551*/
      sub_4156C5(a1: &a2); /*0x5f6559*/
      v61 = -1; /*0x5f6561*/
      if ( v60 != 0 ) /*0x5f6567*/
        ZXString<unsigned short>::_Release(lpAddend: (v60 - 12)); /*0x5f656d*/
      sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x5f657c*/
      v61 = 4; /*0x5f6585*/
      sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x5f658c*/
      v11 = *v10 == 0; /*0x5f6591*/
      LOBYTE(v61) = 5; /*0x5f6594*/
      if ( v11 ) /*0x5f6598*/
        _com_issue_error(a1: -2147467261); /*0x5f659f*/
      v12 = *v10; /*0x5f65a4*/
      v52 = *&pvargDest.vt; /*0x5f65ae*/
      v13 = *v12; /*0x5f65b0*/
      a2 = v12; /*0x5f65c2*/
      v14 = (*(v13 + 272))( /*0x5f65c6*/
              a1: v12,
              a2: 32,
              a3: *&pvarg.vt,
              a4: pvarg.decVal.Hi32,
              a5: pvarg.lVal,
              a6: pvarg.cyVal.Hi,
              a7: *&pvargDest.vt,
              a8: pvargDest.decVal.Hi32,
              a9: pvargDest.lVal,
              a10: pvargDest.cyVal.Hi);
      if ( v14 < 0 ) /*0x5f65d0*/
        _com_issue_errorex(a1: v14, a2, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x5f65db*/
      LOBYTE(v61) = 4; /*0x5f65e4*/
      v15 = ZComAPI::ZComVariantCopy(&pvarg); /*0x5f65e8*/
      if ( v15 < 0 ) /*0x5f65f0*/
        _com_issue_error(a1: v15); /*0x5f65f3*/
      v61 = -1; /*0x5f65f8*/
      v16 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x5f6600*/
      v17 = v54; /*0x5f6607*/
      if ( v16 < 0 ) /*0x5f6608*/
        _com_issue_error(a1: v16); /*0x5f660b*/
      v54 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f6610*/
      v53 = 255; /*0x5f6611*/
      v52 = v17 | 0xC00614A400000000uLL; /*0x5f661b*/
      v57 = &v52; /*0x5f661e*/
      sub_432C10(a1: 0); /*0x5f6622*/
      v51 = -300; /*0x5f6627*/
      v50 = v59; /*0x5f662c*/
      v61 = 6; /*0x5f662f*/
      v49 = v18; /*0x5f6636*/
      v58 = &v49; /*0x5f6639*/
      v19 = sub_5FD76D(); /*0x5f663d*/
      v20 = sub_444FB6(a1: v19); /*0x5f6644*/
      IWzGr2D::GetCenter(this: v20, a2: &v49); /*0x5f664b*/
      LOBYTE(v61) = 7; /*0x5f6659*/
      v21 = StringPool::GetInstance(); /*0x5f665d*/
      v22 = StringPool::GetStringW(this: v21, a2: &v60, a3: SP_1314_UI_LOGINIMG_CHARSELECT_EFFECT_1); /*0x5f6664*/
      LOBYTE(v61) = 9; /*0x5f666c*/
      v23 = sub_41468C(a1: v22); /*0x5f6670*/
      v24 = sub_43E86F( /*0x5f667a*/
              a1: &a2,
              a2: v23,
              a3: 0,
              a4: v49,
              a5: v50,
              a6: v51,
              a7: v52,
              a8: SHIDWORD(v52),
              a9: v53,
              a10: v54);
      v25 = (this + 460); /*0x5f6684*/
      sub_428712(a1: *v24); /*0x5f668c*/
      if ( a2 != 0 ) /*0x5f6696*/
        (*(*a2 + 8))(a1: a2); /*0x5f669b*/
      v61 = -1; /*0x5f669e*/
      ZXString<unsigned short>::_Release(a1: &v60); /*0x5f66a5*/
      sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x5f66b3*/
      v61 = 10; /*0x5f66bc*/
      sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x5f66c3*/
      v11 = *v25 == 0; /*0x5f66c8*/
      LOBYTE(v61) = 11; /*0x5f66ca*/
      if ( v11 ) /*0x5f66ce*/
        _com_issue_error(a1: -2147467261); /*0x5f66d5*/
      v26 = *v25; /*0x5f66df*/
      v27 = (*(*v26 + 272))( /*0x5f66f9*/
              a1: v26,
              a2: 0,
              a3: *&pvarg.vt,
              a4: pvarg.decVal.Hi32,
              a5: pvarg.lVal,
              a6: pvarg.cyVal.Hi,
              a7: *&pvargDest.vt,
              a8: pvargDest.decVal.Hi32,
              a9: pvargDest.lVal,
              a10: pvargDest.cyVal.Hi);
      if ( v27 < 0 ) /*0x5f6701*/
        _com_issue_errorex(a1: v27, a2: v26, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x5f670a*/
      LOBYTE(v61) = 10; /*0x5f6713*/
      v28 = ZComAPI::ZComVariantCopy(&pvarg); /*0x5f6717*/
      if ( v28 < 0 ) /*0x5f671f*/
        _com_issue_error(a1: v28); /*0x5f6722*/
      v61 = -1; /*0x5f6727*/
      p_pvarg = &pvargDest; /*0x5f672b*/
    }
    v47 = ZComAPI::ZComVariantCopy(pvarg: p_pvarg); /*0x5f68c7*/
    if ( v47 < 0 ) /*0x5f68cf*/
      _com_issue_error(a1: v47); /*0x5f68d2*/
    v54 = SP_1237_CHARSELECT; /*0x5f68da*/
    v53 = &v58; /*0x5f68df*/
    v48 = StringPool::GetInstance(); /*0x5f68e0*/
    v54 = *StringPool::GetStringW(this: v48, a2: v53, a3: v54); /*0x5f68ec*/
    v61 = 19; /*0x5f68ee*/
    play_ui_sound(a1: v54); /*0x5f68f5*/
    v61 = -1; /*0x5f68fa*/
    ZXString<unsigned short>::_Release(a1: &v58); /*0x5f6902*/
  }
  else
  {
    sub_428712(a1: 0); /*0x5f64a4*/
    sub_428712(a1: 0); /*0x5f64b0*/
  }
}
```

## CLogin::SendCheckPasswordPacket @ 0x5f6952

```c
int __thiscall CLogin::SendCheckPasswordPacket(_DWORD *this, char *a2, char *a3)
{
  char *MachineId; // eax
  int GameRoomClient; // eax
  int PartnerCode; // eax
  size_t v8[4]; // [esp-4h] [ebp-45Ch] BYREF
  _BYTE v9[36]; // [esp+40Ch] [ebp-4Ch] BYREF
  size_t *v10; // [esp+430h] [ebp-28h]
  int v11; // [esp+43Ch] [ebp-1Ch] BYREF
  _BYTE v12[12]; // [esp+440h] [ebp-18h] BYREF
  size_t *v13; // [esp+44Ch] [ebp-Ch]
  int v14; // [esp+454h] [ebp-4h]

  if ( *(this + 92) != 0 ) /*0x5f696f*/
    return 0; /*0x5f6b53*/
  *(this + 92) = 1; /*0x5f697e*/
  sub_5FDDE3(a1: this + 99); /*0x5f6984*/
  sub_5FDF26(a1: this + 129); /*0x5f698f*/
  CSystemInfo::CSystemInfo(this: v9); /*0x5f6b60*/
  v14 = 2; /*0x5f6b68*/
  CSystemInfo::Init(this: v9); /*0x5f6b6f*/
  COutPacket::COutPacket(a1: 1); /*0x5f6b78*/
  v10 = v8; /*0x5f6b80*/
  LOBYTE(v14) = 3; /*0x5f6b8a*/
  v8[0] = 0; /*0x5f6b8e*/
  ZXString<char>::GetBuffer(a1: a2, a2: 0xFFFFFFFF); /*0x5f6b90*/
  COutPacket::EncodeStr(a1: v8[0]); /*0x5f6b98*/
  v13 = v8; /*0x5f6ba0*/
  v8[0] = 0; /*0x5f6bab*/
  ZXString<char>::GetBuffer(a1: a3, a2: 0xFFFFFFFF); /*0x5f6bad*/
  COutPacket::EncodeStr(a1: v8[0]); /*0x5f6bb5*/
  v8[0] = 16; /*0x5f6bba*/
  MachineId = CSystemInfo::GetMachineId(this: v9); /*0x5f6bbf*/
  COutPacket::EncodeBuffer(a1: MachineId, a2: v8[0]); /*0x5f6bc8*/
  GameRoomClient = CSystemInfo::GetGameRoomClient(); /*0x5f6bd0*/
  COutPacket::Encode4(this: &v11, a2: GameRoomClient); /*0x5f6bd9*/
  COutPacket::Encode1(this: &v11, a2: *(TSingleton<CWvsApp>::ms_pInstance + 36)); /*0x5f6be9*/
  COutPacket::Encode1(this: &v11, a2: 0); /*0x5f6bf2*/
  COutPacket::Encode1(this: &v11, a2: 0); /*0x5f6bfb*/
  PartnerCode = CConfig::GetPartnerCode(this: TSingleton<CConfig>::ms_pInstance); /*0x5f6c06*/
  COutPacket::Encode4(this: &v11, a2: PartnerCode); /*0x5f6c0f*/
  CClientSocket::SendPacket(this: dword_BE7914, a2: &v11); /*0x5f6c1e*/
  ZXString<char>::GetBuffer(a1: a2, a2: 0xFFFFFFFF); /*0x5f6c32*/
  if ( *dword_BEDA60 != 0 ) /*0x5f6c3e*/
    (*(*(*dword_BEDA60 + 4) + 52))(a1: *dword_BEDA60 + 4); /*0x5f6c45*/
  LOBYTE(v14) = 2; /*0x5f6c4b*/
  ZArray<unsigned char>::RemoveAll(a1: v12); /*0x5f6c4f*/
  v14 = -1; /*0x5f6c57*/
  sub_A54BC0(a1: v9); /*0x5f6c5a*/
  return 1; /*0x5f6c61*/
}
```

## CLogin::SendViewAllCharPacket @ 0x5f6d6a

```c
signed int __thiscall CLogin::SendViewAllCharPacket(CLogin *this, int a2, int a3)
{
  unsigned int v3; // edi
  bool v4; // zf
  int v5; // eax
  int i; // esi
  int v7; // edx
  int v8; // edx
  char *v9; // esi
  int v10; // eax
  int v11; // eax
  int v12; // eax
  int v13; // eax
  int j; // ebx
  int v15; // eax
  int v16; // edi
  int Instance; // eax
  int v18; // eax
  char *MachineId; // eax
  int GameRoomClient; // eax
  int v21; // edi
  int v23; // esi
  _DWORD *v24; // eax
  char *v25; // eax
  unsigned __int8 ***v26; // ecx
  _DWORD *v27; // eax
  char *v28; // eax
  int v29; // [esp-8h] [ebp-480h]
  unsigned __int8 *v30[4]; // [esp-4h] [ebp-47Ch] BYREF
  char v31[1024]; // [esp+Ch] [ebp-46Ch] BYREF
  struct sockaddr name; // [esp+40Ch] [ebp-6Ch] BYREF
  _BYTE v33[36]; // [esp+41Ch] [ebp-5Ch] BYREF
  int namelen; // [esp+440h] [ebp-38h] BYREF
  int v35; // [esp+444h] [ebp-34h] BYREF
  _BYTE v36[12]; // [esp+448h] [ebp-30h] BYREF
  unsigned __int8 **v37; // [esp+454h] [ebp-24h] BYREF
  CLogin *v38; // [esp+458h] [ebp-20h]
  char v39; // [esp+45Fh] [ebp-19h] BYREF
  int v40; // [esp+460h] [ebp-18h] BYREF
  int v41; // [esp+464h] [ebp-14h] BYREF
  int v42; // [esp+468h] [ebp-10h] BYREF
  int v43; // [esp+474h] [ebp-4h]

  v3 = 0; /*0x5f6d7d*/
  v4 = *(this + 92) == 0; /*0x5f6d7f*/
  v38 = this; /*0x5f6d85*/
  if ( v4 ) /*0x5f6d88*/
  {
    v5 = 0; /*0x5f6d8e*/
    for ( i = 0; ; i += 32 ) /*0x5f6d90*/
    {
      v7 = *(this + 99); /*0x5f6d92*/
      if ( v7 == 0 || v5 >= *(v7 - 4) ) /*0x5f6da3*/
        return 0; /*0x5f6da3*/
      if ( *(i + v7) == a2 ) /*0x5f6daf*/
        break; /*0x5f6daf*/
      ++v5; /*0x5f6db1*/
    }
    if ( v5 < 0 ) /*0x5f6db9*/
      return 0; /*0x5f6db9*/
    v8 = *(this + 99); /*0x5f6dbf*/
    if ( v8 == 0 ) /*0x5f6dc7*/
      return 0; /*0x5f6dc7*/
    if ( v5 >= *(v8 - 4) ) /*0x5f6dd0*/
      return 0; /*0x5f6dd0*/
    if ( a3 < 0 ) /*0x5f6ddb*/
      return 0; /*0x5f6ddb*/
    v9 = (32 * v5 + *(this + 99)); /*0x5f6dea*/
    v10 = *(v9 + 7); /*0x5f6ded*/
    if ( v10 == 0 || a3 >= *(v10 - 4) ) /*0x5f6dfb*/
      return 0; /*0x5f6dfb*/
    v11 = *(v10 - 4); /*0x5f6e09*/
    v42 = 0; /*0x5f6e14*/
    ZArray<ZXString<char>>::_Alloc(a1: v11, a2: &a2 + 3); /*0x5f6e17*/
    v12 = *(v9 + 7); /*0x5f6e1c*/
    v43 = 0; /*0x5f6e21*/
    if ( v12 != 0 ) /*0x5f6e24*/
      v13 = *(v12 - 4); /*0x5f6e2a*/
    else
      v13 = 0; /*0x5f6e26*/
    v41 = 0; /*0x5f6e35*/
    sub_5FDC2A(a1: v13, a2: &v39); /*0x5f6e38*/
    LOBYTE(v43) = 1; /*0x5f6e3d*/
    for ( j = 0; ; j += 20 ) /*0x5f6e41*/
    {
      v15 = *(v9 + 7); /*0x5f6e43*/
      if ( v15 == 0 || v3 >= *(v15 - 4) ) /*0x5f6e4d*/
        break; /*0x5f6e4d*/
      ZXString<char>::operator=(a1: *(v9 + 7) + j); /*0x5f6e5b*/
      *(v41 + 4 * v3++) = *(j + *(v9 + 7) + 16); /*0x5f6e6a*/
    }
    sub_A02DDE(a1: *v9, a2: a3, a3: &v42, a4: &v41); /*0x5f6e86*/
    v16 = *(TSingleton<CWvsApp>::ms_pInstance + 36); /*0x5f6e91*/
    if ( v16 != 1 ) /*0x5f6e99*/
    {
LABEL_23:
      COutPacket::COutPacket(a1: 5); /*0x5f6ee5*/
      LOBYTE(v43) = 5; /*0x5f6ef3*/
      COutPacket::Encode1(this: &v35, a2: v16); /*0x5f6ef7*/
      if ( v16 == 1 ) /*0x5f6efe*/
      {
        CSystemInfo::CSystemInfo(this: v33); /*0x5f6f03*/
        LOBYTE(v43) = 6; /*0x5f6f0b*/
        CSystemInfo::Init(this: v33); /*0x5f6f0f*/
        v30[0] = 0; /*0x5f6f17*/
        v37 = v30; /*0x5f6f1a*/
        ZXString<char>::GetBuffer(a1: v31, a2: 0xFFFFFFFF); /*0x5f6f26*/
        COutPacket::EncodeStr(a1: v30[0]); /*0x5f6f2e*/
        v30[0] = 16; /*0x5f6f33*/
        MachineId = CSystemInfo::GetMachineId(this: v33); /*0x5f6f38*/
        COutPacket::EncodeBuffer(a1: MachineId, a2: v30[0]); /*0x5f6f41*/
        GameRoomClient = CSystemInfo::GetGameRoomClient(); /*0x5f6f49*/
        COutPacket::Encode4(this: &v35, a2: GameRoomClient); /*0x5f6f52*/
        COutPacket::Encode1(this: &v35, a2: *(TSingleton<CWvsApp>::ms_pInstance + 36)); /*0x5f6f62*/
        LOBYTE(v43) = 5; /*0x5f6f6a*/
        sub_A54BC0(a1: v33); /*0x5f6f6e*/
      }
      COutPacket::Encode1(this: &v35, a2: *v9); /*0x5f6f7b*/
      COutPacket::Encode1(this: &v35, a2: a3); /*0x5f6f86*/
      namelen = 16; /*0x5f6f97*/
      v21 = 0; /*0x5f6fa3*/
      if ( getsockname(s: *(dword_BE7914 + 8), &name, &namelen) == 0 ) /*0x5f6fad*/
        v21 = *&name.sa_data[2]; /*0x5f6faf*/
      COutPacket::Encode4(this: &v35, a2: v21); /*0x5f6fb6*/
      CClientSocket::SendPacket(this: dword_BE7914, a2: &v35); /*0x5f6fc5*/
      *(v38 + 92) = 1; /*0x5f6fcd*/
      ZXString<char>::operator=(a1: v9 + 4); /*0x5f6fdf*/
      CConfig::SaveGlobal(this: TSingleton<CConfig>::ms_pInstance); /*0x5f6fea*/
      LOBYTE(v43) = 1; /*0x5f6ff2*/
      ZArray<unsigned char>::RemoveAll(a1: v36); /*0x5f6ff6*/
      LOBYTE(v43) = 0; /*0x5f6ffb*/
      ZArray<int>::RemoveAll(a1: &v41); /*0x5f7002*/
      v43 = -1; /*0x5f7007*/
      ZArray<ZXString<char>>::RemoveAll(a1: &v42); /*0x5f700e*/
      return 1; /*0x5f7015*/
    }
    CWvsApp::GetCmdLine(a1: &v40, a2: 1); /*0x5f6ea0*/
    LOBYTE(v43) = 2; /*0x5f6ead*/
    Instance = CNMCOClientObject::GetInstance(a1: v40, a2: 201); /*0x5f6eb1*/
    v18 = CNMCOClientObject::AttachAuth(a1: Instance, a2: v29, a3: v30[0]); /*0x5f6eb8*/
    if ( v18 == 0 ) /*0x5f6ec1*/
    {
      CNMCOClientObject::GetInstance(a1: v31, a2: v30[1]); /*0x5f6ece*/
      CNMCOClientObject::GetNexonPassport(a1: v30[0]); /*0x5f6ed5*/
      LOBYTE(v43) = 1; /*0x5f6edd*/
      ZXString<char>::_Release(a1: &v40); /*0x5f6ee0*/
      goto LABEL_23; /*0x5f6ee0*/
    }
    if ( v18 <= 20025 ) /*0x5f7021*/
    {
      if ( v18 != 20025 ) /*0x5f7023*/
      {
        switch ( v18 ) /*0x5f7036*/
        {
          case 20000: /*0x5f7036*/
          case 20021: /*0x5f7036*/
            v30[0] = 12; /*0x5f703d*/
            goto LABEL_50; /*0x5f703f*/
          case 20002: /*0x5f7036*/
            goto LABEL_43;
          case 20003: /*0x5f7036*/
            v30[0] = 17; /*0x5f7041*/
            goto LABEL_50; /*0x5f7043*/
          case 20006: /*0x5f7036*/
            v30[0] = 14; /*0x5f7045*/
            goto LABEL_50; /*0x5f7047*/
          case 20007: /*0x5f7036*/
            v30[0] = 2; /*0x5f7049*/
            goto LABEL_50; /*0x5f704b*/
          case 20008: /*0x5f7036*/
          case 20009: /*0x5f7036*/
          case 20010: /*0x5f7036*/
            goto LABEL_42;
          case 20015: /*0x5f7036*/
            goto LABEL_44;
          default:
            goto LABEL_47;
        }
      }
      v30[0] = 15; /*0x5f704d*/
      goto LABEL_50; /*0x5f704f*/
    }
    if ( v18 > 20033 ) /*0x5f7056*/
    {
      if ( v18 != 20036 ) /*0x5f7090*/
      {
        if ( v18 == 20037 ) /*0x5f7097*/
        {
          v30[0] = 7; /*0x5f709d*/
          goto LABEL_50; /*0x5f709f*/
        }
        goto LABEL_47; /*0x5f7097*/
      }
    }
    else
    {
      if ( v18 >= 20032 ) /*0x5f705d*/
      {
LABEL_47:
        v30[0] = 9; /*0x5f7099*/
        goto LABEL_50; /*0x5f709b*/
      }
      if ( v18 == 20026 ) /*0x5f7066*/
      {
LABEL_44:
        v30[0] = 4; /*0x5f7087*/
        goto LABEL_50; /*0x5f7089*/
      }
      if ( v18 > 20028 ) /*0x5f706f*/
      {
        if ( v18 != 20030 ) /*0x5f7076*/
        {
          if ( v18 == 20031 ) /*0x5f707d*/
          {
LABEL_42:
            v23 = 1; /*0x5f707f*/
LABEL_51:
            switch ( v23 ) /*0x5f70b7*/
            {
              case 2: /*0x5f70b7*/
                CLoginUtilDlg::Error(a1: 16, a2: 0); /*0x5f70c9*/
                break; /*0x5f70c9*/
              case 4: /*0x5f70b7*/
                CLoginUtilDlg::Error(a1: 3, a2: 0); /*0x5f70e1*/
                break; /*0x5f70e1*/
              case 7: /*0x5f70b7*/
                CLogin::GotoTitle(this: v38, a2: 0, a3: 0); /*0x5f70eb*/
                CLoginUtilDlg::Error(a1: 17, a2: 0); /*0x5f70f4*/
                break; /*0x5f70f4*/
              case 8: /*0x5f70b7*/
              case 9: /*0x5f70b7*/
              case 12: /*0x5f70b7*/
                CLoginUtilDlg::Error(a1: 15, a2: 0); /*0x5f70c1*/
                break; /*0x5f70c1*/
              case 14: /*0x5f70b7*/
                if ( CLoginUtilDlg::YesNo2(a1: 27) != 0 ) /*0x5f7113*/
                {
                  v24 = StringPool::GetInstance(); /*0x5f7122*/
                  v25 = *StringPool::GetString(this: v24, result: &a3, nIdx: SP_2871_HTTP__WWWNEXONNET); /*0x5f712e*/
                  LOBYTE(v43) = 3; /*0x5f7133*/
                  open_web_site(a1: v25, lpWindowName: 0); /*0x5f7137*/
                  LOBYTE(v43) = 2; /*0x5f713e*/
                  v26 = &a3; /*0x5f7142*/
                  goto LABEL_60; /*0x5f7145*/
                }
                break; /*0x5f7145*/
              case 15: /*0x5f70b7*/
                if ( CLoginUtilDlg::YesNo2(a1: 26) != 0 ) /*0x5f7151*/
                {
                  v27 = StringPool::GetInstance(); /*0x5f715c*/
                  v28 = *StringPool::GetString(this: v27, result: &v37, nIdx: SP_2871_HTTP__WWWNEXONNET); /*0x5f7168*/
                  LOBYTE(v43) = 4; /*0x5f716d*/
                  open_web_site(a1: v28, lpWindowName: 0); /*0x5f7171*/
                  LOBYTE(v43) = 2; /*0x5f7178*/
                  v26 = &v37; /*0x5f717c*/
LABEL_60:
                  ZXString<char>::_Release(a1: v26); /*0x5f717f*/
                }
                break; /*0x5f7184*/
              case 16: /*0x5f70b7*/
                CLoginUtilDlg::Error(a1: 33, a2: 0); /*0x5f7189*/
                break; /*0x5f7189*/
              case 17: /*0x5f70b7*/
                CLoginUtilDlg::Error(a1: 27, a2: 0); /*0x5f718e*/
                break; /*0x5f718e*/
              default:
                break;
            }
            LOBYTE(v43) = 1; /*0x5f71ad*/
            ZXString<char>::_Release(a1: &v40); /*0x5f71b4*/
            LOBYTE(v43) = 0; /*0x5f71b9*/
            ZArray<int>::RemoveAll(a1: &v41); /*0x5f71c0*/
            v43 = -1; /*0x5f71c5*/
            ZArray<ZXString<char>>::RemoveAll(a1: &v42); /*0x5f71cc*/
            return 0; /*0x5f71cc*/
          }
          goto LABEL_47; /*0x5f707d*/
        }
LABEL_43:
        v30[0] = 8; /*0x5f7083*/
LABEL_50:
        v23 = v30[0]; /*0x5f70a3*/
        goto LABEL_51; /*0x5f70a3*/
      }
    }
    v30[0] = 16; /*0x5f70a1*/
    goto LABEL_50; /*0x5f70a1*/
  }
  return 0; /*0x5f71d3*/
}
```

## CLogin::SendSelectCharPacket @ 0x5f726d

```c
void __thiscall CLogin::SendSelectCharPacket(CLogin *this)
{
  unsigned int v2; // eax
  int v3; // eax
  unsigned __int8 v4; // al
  _DWORD *v5; // ecx
  _DWORD *v6; // eax
  int v7; // ecx
  _DWORD v8[4]; // [esp-4h] [ebp-68h] BYREF
  int v9; // [esp+Ch] [ebp-58h] BYREF
  _BYTE v10[12]; // [esp+10h] [ebp-54h] BYREF
  int v11; // [esp+1Ch] [ebp-48h] BYREF
  _BYTE v12[12]; // [esp+20h] [ebp-44h] BYREF
  int v13; // [esp+2Ch] [ebp-38h] BYREF
  _BYTE v14[4]; // [esp+30h] [ebp-34h] BYREF
  _BYTE v15[4]; // [esp+34h] [ebp-30h] BYREF
  int v16; // [esp+38h] [ebp-2Ch]
  _DWORD *v17; // [esp+3Ch] [ebp-28h]
  _DWORD *v18; // [esp+40h] [ebp-24h] BYREF
  int v19; // [esp+44h] [ebp-20h]
  int v20; // [esp+48h] [ebp-1Ch] BYREF
  int v21; // [esp+4Ch] [ebp-18h] BYREF
  _DWORD *v22; // [esp+50h] [ebp-14h] BYREF
  int v23; // [esp+54h] [ebp-10h] BYREF
  int v24; // [esp+60h] [ebp-4h]

  if ( *(this + 92) == 0 && *(this + 126) == 0 )
  {
    v2 = *(this + 100); /*0x5f7299*/
    if ( v2 <= 0xE && *(this + 91) == 0 )
    {
      v3 = *(684 * v2 + *(this + 101)); /*0x5f72c8*/
      v19 = 0; /*0x5f72cd*/
      if ( v3 != 0 ) /*0x5f72d0*/
        v19 = v3; /*0x5f72d2*/
      v4 = *(this + 372); /*0x5f72d5*/
      if ( v4 != 0 )
      {
        if ( v4 == 1 )
        {
          v5 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1848); /*0x5f73cd*/
          v22 = v5; /*0x5f73cf*/
          v24 = 4; /*0x5f73d4*/
          if ( v5 != 0 ) /*0x5f73db*/
            v6 = CSoftKeyboardDlg::CSoftKeyboardDlg(this: v5, a2: 6, a3: 16); /*0x5f73e1*/
          else
            v6 = 0; /*0x5f73e8*/
          v24 = -1; /*0x5f73ea*/
          v7 = v6 != 0 ? (v6 + 2) : 0;
          v16 = v7 != 0 ? v7 - 8 : 0;
          sub_5FDA24(a1: v15); /*0x5f7408*/
          v24 = 5; /*0x5f7418*/
          ZRef<CDialog>::operator=(a1: v16); /*0x5f741f*/
          if ( CDialog::DoModal(a1: v16) != 1 ) /*0x5f742f*/
          {
            sub_42890F(a1: this + 500); /*0x5f7433*/
            v24 = -1; /*0x5f7438*/
            sub_5FDA41(a1: v15); /*0x5f743f*/
            return; /*0x5f7444*/
          }
          CSoftKeyboardDlg::GetResult(a1: &v18); /*0x5f7450*/
          LOBYTE(v24) = 6; /*0x5f745a*/
          COutPacket::COutPacket(a1: 30); /*0x5f745e*/
          v17 = v8; /*0x5f7469*/
          LOBYTE(v24) = 7; /*0x5f746d*/
          v8[0] = 0; /*0x5f7471*/
          ZXString<char>::operator=(a1: &v18); /*0x5f7473*/
          COutPacket::EncodeStr(a1: v8[0]); /*0x5f747b*/
          COutPacket::Encode4(this: &v11, a2: v19); /*0x5f7486*/
          v20 = 0; /*0x5f7496*/
          ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF); /*0x5f7499*/
          LOBYTE(v24) = 8; /*0x5f74a4*/
          CLogin::GetLocalMacAddress(a1: &v20); /*0x5f74a8*/
          v17 = v8; /*0x5f74b3*/
          v8[0] = 0; /*0x5f74b7*/
          ZXString<char>::operator=(a1: &v20); /*0x5f74b9*/
          COutPacket::EncodeStr(a1: v8[0]); /*0x5f74c1*/
          v21 = 0; /*0x5f74cc*/
          ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF); /*0x5f74cf*/
          LOBYTE(v24) = 9; /*0x5f74da*/
          CLogin::GetLocalMacAddressWithHDDSerialNo(a1: &v21); /*0x5f74de*/
          v17 = v8; /*0x5f74e9*/
          v8[0] = 0; /*0x5f74ed*/
          ZXString<char>::operator=(a1: &v21); /*0x5f74ef*/
          COutPacket::EncodeStr(a1: v8[0]); /*0x5f74f7*/
          CClientSocket::SendPacket(this: dword_BE7914, a2: &v11); /*0x5f7506*/
          sub_42890F(a1: this + 500); /*0x5f7511*/
          LOBYTE(v24) = 8; /*0x5f7519*/
          ZXString<char>::_Release(a1: &v21); /*0x5f751d*/
          LOBYTE(v24) = 7; /*0x5f7525*/
          ZXString<char>::_Release(a1: &v20); /*0x5f7529*/
          LOBYTE(v24) = 6; /*0x5f7531*/
          ZArray<unsigned char>::RemoveAll(a1: v12); /*0x5f7535*/
          LOBYTE(v24) = 5; /*0x5f753a*/
          ZXString<char>::_Release(a1: &v18); /*0x5f7541*/
          v24 = -1; /*0x5f7546*/
          sub_5FDA41(a1: v15); /*0x5f754d*/
        }
        else if ( v4 <= 3u ) /*0x5f72f3*/
        {
          COutPacket::COutPacket(a1: 19); /*0x5f72fe*/
          v24 = 10; /*0x5f7309*/
          COutPacket::Encode4(this: &v9, a2: v19); /*0x5f7310*/
          v22 = 0; /*0x5f7320*/
          ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF); /*0x5f7323*/
          LOBYTE(v24) = 11; /*0x5f732e*/
          CLogin::GetLocalMacAddress(a1: &v22); /*0x5f7332*/
          v18 = v8; /*0x5f733d*/
          v8[0] = 0; /*0x5f7341*/
          ZXString<char>::operator=(a1: &v22); /*0x5f7343*/
          COutPacket::EncodeStr(a1: v8[0]); /*0x5f734b*/
          v23 = 0; /*0x5f7356*/
          ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF); /*0x5f7359*/
          LOBYTE(v24) = 12; /*0x5f7364*/
          CLogin::GetLocalMacAddressWithHDDSerialNo(a1: &v23); /*0x5f7368*/
          v18 = v8; /*0x5f7373*/
          v8[0] = 0; /*0x5f7377*/
          ZXString<char>::operator=(a1: &v23); /*0x5f7379*/
          COutPacket::EncodeStr(a1: v8[0]); /*0x5f7381*/
          CClientSocket::SendPacket(this: dword_BE7914, a2: &v9); /*0x5f7390*/
          LOBYTE(v24) = 11; /*0x5f7398*/
          ZXString<char>::_Release(a1: &v23); /*0x5f739c*/
          LOBYTE(v24) = 10; /*0x5f73a4*/
          ZXString<char>::_Release(a1: &v22); /*0x5f73a8*/
          v24 = -1; /*0x5f73ad*/
          ZArray<unsigned char>::RemoveAll(a1: v10); /*0x5f73b4*/
        }
      }
      else
      {
        CLoginUtilDlg::Notice(a1: 95, a2: this + 500); /*0x5f7560*/
        v18 = 0; /*0x5f7565*/
        v24 = 0; /*0x5f756d*/
        if ( CSoftKeyboardDlg::InitializeSecondaryPassword(a1: &v18, a2: this + 500) == 0 ) /*0x5f757a*/
        {
          v24 = -1; /*0x5f757c*/
          ZXString<char>::_Release(a1: &v18); /*0x5f7583*/
          return; /*0x5f7588*/
        }
        COutPacket::COutPacket(a1: 29); /*0x5f7592*/
        LOBYTE(v24) = 1; /*0x5f759c*/
        COutPacket::Encode1(this: &v13, a2: 1); /*0x5f75a0*/
        COutPacket::Encode4(this: &v13, a2: v19); /*0x5f75ab*/
        v21 = 0; /*0x5f75bb*/
        ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF); /*0x5f75be*/
        LOBYTE(v24) = 2; /*0x5f75c9*/
        CLogin::GetLocalMacAddress(a1: &v21); /*0x5f75cd*/
        v17 = v8; /*0x5f75d8*/
        v8[0] = 0; /*0x5f75dc*/
        ZXString<char>::operator=(a1: &v21); /*0x5f75de*/
        COutPacket::EncodeStr(a1: v8[0]); /*0x5f75e6*/
        v20 = 0; /*0x5f75f1*/
        ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF); /*0x5f75f4*/
        LOBYTE(v24) = 3; /*0x5f75ff*/
        CLogin::GetLocalMacAddressWithHDDSerialNo(a1: &v20); /*0x5f7603*/
        v17 = v8; /*0x5f760e*/
        v8[0] = 0; /*0x5f7612*/
        ZXString<char>::operator=(a1: &v20); /*0x5f7614*/
        COutPacket::EncodeStr(a1: v8[0]); /*0x5f761c*/
        v17 = v8; /*0x5f7627*/
        v8[0] = 0; /*0x5f762b*/
        ZXString<char>::operator=(a1: &v18); /*0x5f762d*/
        COutPacket::EncodeStr(a1: v8[0]); /*0x5f7635*/
        CClientSocket::SendPacket(this: dword_BE7914, a2: &v13); /*0x5f7644*/
        LOBYTE(v24) = 2; /*0x5f764c*/
        ZXString<char>::_Release(a1: &v20); /*0x5f7650*/
        LOBYTE(v24) = 1; /*0x5f7658*/
        ZXString<char>::_Release(a1: &v21); /*0x5f765c*/
        LOBYTE(v24) = 0; /*0x5f7664*/
        ZArray<unsigned char>::RemoveAll(a1: v14); /*0x5f7667*/
        v24 = -1; /*0x5f766c*/
        ZXString<char>::_Release(a1: &v18); /*0x5f7673*/
      }
      *(this + 92) = 1; /*0x5f7678*/
    }
  }
}
```

## CLogin::SendDeleteCharPacket @ 0x5f7c4a

```c
unsigned int __thiscall CLogin::SendDeleteCharPacket(CLogin *this)
{
  unsigned int result; // eax
  int *v3; // eax
  _DWORD *v4; // eax
  int v5; // ecx
  int v6; // eax
  int v7; // eax
  int v8; // eax
  int v9; // [esp-4h] [ebp-3Ch] BYREF
  int v10; // [esp+Ch] [ebp-2Ch] BYREF
  _BYTE v11[12]; // [esp+10h] [ebp-28h] BYREF
  _BYTE v12[4]; // [esp+1Ch] [ebp-1Ch] BYREF
  int v13; // [esp+20h] [ebp-18h]
  int *v14; // [esp+24h] [ebp-14h]
  _BYTE v15[4]; // [esp+28h] [ebp-10h] BYREF
  int v16; // [esp+34h] [ebp-4h]

  result = &loc_A97A06; /*0x5f7c4a*/
  if ( *(this + 92) == 0 )
  {
    result = *(this + 100); /*0x5f7c6a*/
    if ( result <= 0xE )
    {
      result *= 684; /*0x5f7c87*/
      if ( *(result + *(this + 101)) != 0 )
      {
        if ( *(this + 372) == 1 )
        {
          v3 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1848); /*0x5f7cc7*/
          v14 = v3; /*0x5f7ccc*/
          v16 = 0; /*0x5f7cd1*/
          if ( v3 != 0 ) /*0x5f7cd4*/
            v4 = CSoftKeyboardDlg::CSoftKeyboardDlg(this: v3, a2: 6, a3: 16); /*0x5f7cdc*/
          else
            v4 = 0; /*0x5f7ce3*/
          v16 = -1; /*0x5f7ce5*/
          v5 = v4 != 0 ? (v4 + 2) : 0;
          v13 = v5 != 0 ? v5 - 8 : 0;
          sub_5FDA24(a1: v12); /*0x5f7d03*/
          v16 = 1; /*0x5f7d13*/
          ZRef<CDialog>::operator=(a1: v13); /*0x5f7d16*/
          if ( CDialog::DoModal(a1: v13) == 1 )
          {
            CSoftKeyboardDlg::GetResult(a1: v15); /*0x5f7d3a*/
            LOBYTE(v16) = 2; /*0x5f7d41*/
            sub_42890F(a1: this + 500); /*0x5f7d45*/
            v6 = *(this + 101) + 684 * *(this + 100); /*0x5f7d56*/
            v7 = _ZtlSecureFuse<short>(a1: v6 + 57, a2: *(v6 + 61)) / 1000; /*0x5f7d73*/
            if ( v7 == 1 || v7 == 2 )
              v8 = *(*(this + 103) + 4 * *(this + 100)) != 0 ? 58 : 55;
            else
              v8 = *(*(this + 103) + 4 * *(this + 100)) != 0 ? 57 : 13;
            if ( CLoginUtilDlg::YesNo(a1: v8, a2: this + 500) != 0 ) /*0x5f7dbd*/
            {
              COutPacket::COutPacket(a1: 23); /*0x5f7dc8*/
              v14 = &v9; /*0x5f7dd3*/
              LOBYTE(v16) = 3; /*0x5f7dd9*/
              v9 = 0; /*0x5f7ddd*/
              ZXString<char>::operator=(a1: v15); /*0x5f7ddf*/
              COutPacket::EncodeStr(a1: v9); /*0x5f7de7*/
              COutPacket::Encode4(this: &v10, a2: *(684 * *(this + 100) + *(this + 101))); /*0x5f7e04*/
              CLogin::SendRequest(this, a2: &v10); /*0x5f7e0f*/
              if ( dword_BEDA4C != 0 ) /*0x5f7e1a*/
              {
                *(this + 100) = -1; /*0x5f7e1c*/
                CUIAvatar::SelectCharacter(this: dword_BEDA4C, a2: -1); /*0x5f7e2b*/
              }
              sub_428712(a1: 0); /*0x5f7e37*/
              sub_428712(a1: 0); /*0x5f7e43*/
              LOBYTE(v16) = 2; /*0x5f7e4b*/
              ZArray<unsigned char>::RemoveAll(a1: v11); /*0x5f7e4f*/
            }
            LOBYTE(v16) = 1; /*0x5f7e57*/
            ZXString<char>::_Release(a1: v15); /*0x5f7e5a*/
          }
          else
          {
            sub_42890F(a1: this + 500); /*0x5f7d29*/
          }
          v16 = -1; /*0x5f7e5f*/
          return sub_5FDA41(a1: v12); /*0x5f7e66*/
        }
        else
        {
          return CLoginUtilDlg::Notice(a1: 94, a2: this + 500); /*0x5f7cb1*/
        }
      }
    }
  }
  return result; /*0x5f7e6b*/
}
```

## CLogin::SendCheckDuplicateIDPacket @ 0x5f7fe7

```c
void *__thiscall CLogin::SendCheckDuplicateIDPacket(CLogin *this, int *a2)
{
  void *result; // eax
  signed int v4; // eax
  int v5; // [esp-4h] [ebp-28h] BYREF
  _BYTE v6[4]; // [esp+4h] [ebp-20h] BYREF
  _BYTE v7[12]; // [esp+8h] [ebp-1Ch] BYREF
  int *v8; // [esp+14h] [ebp-10h]
  int v9; // [esp+20h] [ebp-4h]

  result = &loc_A97A40; /*0x5f7fe7*/
  if ( *(this + 92) == 0 ) /*0x5f7ffe*/
  {
    v4 = sub_4F38D3(this: *dword_BE7918); /*0x5f8006*/
    if ( is_valid_character_name(sCharName: a2, bWordCheck: v4 == 0) ) /*0x5f8014*/
    {
      COutPacket::COutPacket(a1: 21); /*0x5f8030*/
      v9 = 0; /*0x5f8035*/
      v5 = 0; /*0x5f803c*/
      v8 = &v5; /*0x5f803f*/
      ZXString<char>::operator=(a1: a2); /*0x5f8045*/
      COutPacket::EncodeStr(a1: v5); /*0x5f804d*/
      CLogin::SendRequest(this, a2: v6); /*0x5f8058*/
      v9 = -1; /*0x5f805d*/
      return ZArray<unsigned char>::RemoveAll(a1: v7); /*0x5f8064*/
    }
    else
    {
      return CLoginUtilDlg::Error(a1: 10, a2: 0); /*0x5f8022*/
    }
  }
  return result; /*0x5f8069*/
}
```

## CLogin::OnPacket @ 0x5f80ff

```c
void __thiscall CLogin::OnPacket(char *this, signed int a2, struct CInPacket *a3)
{
  switch ( a2 ) /*0x5f810e*/
  {
    case 0: /*0x5f810e*/
      CLogin::OnCheckPasswordResult(a1: a3); /*0x5f811b*/
      break; /*0x5f8120*/
    case 1: /*0x5f810e*/
      CLogin::OnGuestIDLoginResult(a1: a3); /*0x5f812b*/
      break; /*0x5f8130*/
    case 2: /*0x5f810e*/
      CLogin::OnAccountInfoResult(a1: a3); /*0x5f813b*/
      break; /*0x5f8140*/
    case 3: /*0x5f810e*/
      CLogin::OnCheckUserLimitResult(a1: a3); /*0x5f814b*/
      break; /*0x5f8150*/
    case 4: /*0x5f810e*/
      CLogin::OnSetAccountResult(a1: a3); /*0x5f815b*/
      break; /*0x5f8160*/
    case 5: /*0x5f810e*/
      CLogin::OnConfirmEULAResult(a1: a3); /*0x5f816b*/
      break; /*0x5f8170*/
    case 6: /*0x5f810e*/
      CLogin::OnCheckPinCodeResult(a1: a3); /*0x5f817b*/
      break; /*0x5f8180*/
    case 7: /*0x5f810e*/
      CLogin::OnUpdatePinCodeResult(a1: a3); /*0x5f818b*/
      break; /*0x5f8190*/
    case 8: /*0x5f810e*/
      CLogin::OnViewAllCharResult(a1: a3); /*0x5f819b*/
      break; /*0x5f81a0*/
    case 9: /*0x5f810e*/
      CLogin::OnSelectCharacterByVACResult(a1: a3); /*0x5f81ab*/
      break; /*0x5f81b0*/
    case 10: /*0x5f810e*/
      CLogin::OnWorldInformation(a1: a3); /*0x5f81bb*/
      break; /*0x5f81c0*/
    case 11: /*0x5f810e*/
      CLogin::OnSelectWorldResult(a1: a3); /*0x5f81cb*/
      break; /*0x5f81d0*/
    case 12: /*0x5f810e*/
      CLogin::OnSelectCharacterResult(a1: a3); /*0x5f8208*/
      break; /*0x5f820d*/
    case 13: /*0x5f810e*/
      CLogin::OnCheckDuplicatedIDResult(a1: a3); /*0x5f81db*/
      break; /*0x5f81e0*/
    case 14: /*0x5f810e*/
      CLogin::OnCreateNewCharacterResult(a1: a3); /*0x5f81fb*/
      break; /*0x5f8200*/
    case 15: /*0x5f810e*/
      CLogin::OnDeleteCharacterResult(a1: a3); /*0x5f81eb*/
      break; /*0x5f81f0*/
    case 22: /*0x5f810e*/
      sub_5FB83D(a1: a3); /*0x5f8215*/
      break; /*0x5f821a*/
    case 23: /*0x5f810e*/
      CLogin::OnEnableSPWResult(a1: a3); /*0x5f8222*/
      break; /*0x5f8227*/
    case 26: /*0x5f810e*/
      CLogin::OnLatestConnectedWorld(a1: a3); /*0x5f823c*/
      break; /*0x5f8241*/
    case 27: /*0x5f810e*/
      CLogin::OnRecommendWorldMessage(a1: a3); /*0x5f8249*/
      break; /*0x5f824e*/
    case 28: /*0x5f810e*/
      CLogin::OnCheckSPWResult(a1: a3); /*0x5f822f*/
      break; /*0x5f8234*/
    default:
      if ( a2 < 125 || a2 > 127 ) /*0x5f8258*/
      {
        if ( a2 >= 128 && a2 <= 130 ) /*0x5f8271*/
          CMapLoadable::OnPacket(a1: a2, a2: a3); /*0x5f8277*/
      }
      else
      {
        CStage::OnPacket(this, nType: a2, iPacket: a3); /*0x5f825e*/
      }
      break; /*0x5f8263*/
  }
}
```

## CLogin::OnCheckPasswordResult @ 0x5f83ee

```c
// positive sp value has been detected, the output may be wrong!
void __thiscall CLogin::OnCheckPasswordResult(CLogin *this, int result)
{
  struct CInPacket *v2; // esi
  struct CInPacket *v4; // ecx
  int v5; // ecx
  int v6; // edi
  int v7; // esi
  _DWORD *v8; // eax
  char *v9; // ecx
  _DWORD *Instance; // eax
  int v11; // ecx
  char *p_Unknown; // ecx
  int v13; // ecx
  _DWORD *v14; // eax
  int v15; // eax
  int *v16; // eax
  int v17; // eax
  int v18; // eax
  int v19; // eax
  int v20; // eax
  void *v21; // esp
  wchar_t *v22; // eax
  int v23; // eax
  int v24; // eax
  _DWORD *v25; // eax
  int Item; // eax
  bool v27; // zf
  wchar_t *v28; // eax
  int v29; // eax
  wchar_t *v30; // ecx
  _DWORD *v31; // eax
  int v32; // ecx
  _DWORD *v33; // eax
  char *v34; // eax
  _DWORD *v35; // eax
  _DWORD *v36; // eax
  char *v37; // eax
  _DWORD *v38; // eax
  char *v39; // eax
  int v40; // edi
  CLogin *v41; // esi
  unsigned __int8 v42; // al
  _DWORD *v43; // eax
  char *v44; // eax
  struct CInPacket *v45; // edi
  int *v46; // esi
  VARIANTARG *v47; // edi
  struct CInPacket *v48; // eax
  int v49; // edx
  wchar_t *v50; // ecx
  int v51; // ecx
  int v52; // esi
  char v53; // al
  ULONG *p_Hi32; // ecx
  int v55; // [esp-54h] [ebp-140h]
  int v56; // [esp-50h] [ebp-13Ch]
  int *v57; // [esp-50h] [ebp-13Ch]
  int v58; // [esp-4Ch] [ebp-138h]
  int v59; // [esp-48h] [ebp-134h]
  int v60; // [esp-48h] [ebp-134h]
  int v61; // [esp-44h] [ebp-130h] BYREF
  wchar_t *v62; // [esp-40h] [ebp-12Ch] BYREF
  wchar_t *v63; // [esp-3Ch] [ebp-128h] BYREF
  wchar_t *v64; // [esp-38h] [ebp-124h]
  int *v65; // [esp-34h] [ebp-120h] BYREF
  StringPoolStrings v66; // [esp-30h] [ebp-11Ch]
  int v67; // [esp-2Ch] [ebp-118h] BYREF
  int v68; // [esp-28h] [ebp-114h] BYREF
  int v69; // [esp-24h] [ebp-110h]
  int v70; // [esp-20h] [ebp-10Ch]
  int v71; // [esp-1Ch] [ebp-108h] BYREF
  VARIANTARG *v72; // [esp-18h] [ebp-104h]
  VARIANTARG *p_pvargDest; // [esp-14h] [ebp-100h]
  int p_result; // [esp-10h] [ebp-FCh] BYREF
  StringPoolStrings v75; // [esp-Ch] [ebp-F8h] BYREF
  char v76; // [esp-8h] [ebp-F4h]
  VARIANTARG v77; // [esp+Ch] [ebp-E0h] BYREF
  int v78[4]; // [esp+1Ch] [ebp-D0h] BYREF
  VARIANTARG v79; // [esp+2Ch] [ebp-C0h] BYREF
  VARIANTARG pvarg; // [esp+3Ch] [ebp-B0h] BYREF
  _DWORD v81[4]; // [esp+4Ch] [ebp-A0h] BYREF
  _DWORD v82[4]; // [esp+5Ch] [ebp-90h] BYREF
  _DWORD v83[2]; // [esp+6Ch] [ebp-80h] BYREF
  _DWORD v84[2]; // [esp+74h] [ebp-78h] BYREF
  __int64 v85; // [esp+7Ch] [ebp-70h] BYREF
  VARIANTARG pvargDest; // [esp+84h] [ebp-68h] BYREF
  _DWORD v87[2]; // [esp+94h] [ebp-58h] BYREF
  VARIANTARG v88; // [esp+9Ch] [ebp-50h] BYREF
  int v89; // [esp+ACh] [ebp-40h] BYREF
  char v90; // [esp+B0h] [ebp-3Ch] BYREF
  unsigned __int16 v91; // [esp+B2h] [ebp-3Ah]
  unsigned __int16 v92; // [esp+B4h] [ebp-38h]
  unsigned __int16 v93; // [esp+B6h] [ebp-36h]
  char v94[4]; // [esp+BCh] [ebp-30h] BYREF
  int v95; // [esp+C0h] [ebp-2Ch] BYREF
  int Unknown; // [esp+C4h] [ebp-28h] BYREF
  wchar_t *v97; // [esp+C8h] [ebp-24h] BYREF
  int *v98; // [esp+CCh] [ebp-20h]
  int v99; // [esp+D0h] [ebp-1Ch]
  CLogin *v100; // [esp+D4h] [ebp-18h] BYREF
  char v101[4]; // [esp+D8h] [ebp-14h] BYREF
  int a2; // [esp+DCh] [ebp-10h] BYREF
  int v103; // [esp+E8h] [ebp-4h]

  v2 = result; /*0x5f8400*/
  v4 = result; /*0x5f8408*/
  v100 = this; /*0x5f840a*/
  *(this + 92) = 0; /*0x5f840d*/
  v99 = CInPacket::Decode1(this: v4); /*0x5f841d*/
  *(this + 456) = CInPacket::Decode1(this: v2); /*0x5f8427*/
  CInPacket::Decode4(this: v2); /*0x5f842d*/
  v5 = *dword_BEDA60; /*0x5f8432*/
  if ( *dword_BEDA60 != 0 ) /*0x5f843a*/
    CUITitle::EnableLoginCtrl(this: *dword_BEDA60, a2: 1); /*0x5f843e*/
  if ( v99 == 2 )
  {
    v6 = CInPacket::Decode1(this: v2); /*0x5f8454*/
    CInPacket::DecodeBuffer(a1: v87, a2: 8u); /*0x5f845f*/
    if ( v6 == 0 ) /*0x5f8466*/
      goto LABEL_51; /*0x5f8466*/
    if ( v6 > 20 && v6 < 41 ) /*0x5f8474*/
      v6 = 12; /*0x5f8478*/
    dword_BF03B8(a1: v87, a2: &v89); /*0x5f8481*/
    dword_BF03B0(a1: v78); /*0x5f848e*/
    v88.decVal.8 = sub_7B8F35(); /*0x5f8499*/
    v85 = (sub_7B8E69)(a1: &v88.decVal.Lo32, a2: 1080); /*0x5f84ae*/
    dword_BF03B4(a1: &v85, a2: v87); /*0x5f84bd*/
    *v101 = 0; /*0x5f84c3*/
    v103 = 0; /*0x5f84d6*/
    if ( v89 - LOWORD(v78[0]) >= 3 ) /*0x5f84d9*/
    {
      Instance = StringPool::GetInstance(); /*0x5f8562*/
      v75 = *StringPool::GetString( /*0x5f856e*/
               this: Instance,
               result: &a2,
               nIdx: SP_2875_THE_ID_HAS_BEEN_PERMANENTLY_BLOCKED_R_NSO_YOU_WONT_BE_ABLE_TO_USE_THIS_ACCOUNT);
      LOBYTE(v103) = 2; /*0x5f8574*/
      ZXString<char>::Format(a1: v101, a2: v75, a3: v76); /*0x5f8578*/
      LOBYTE(v103) = 0; /*0x5f857f*/
    }
    else
    {
      if ( v92 <= 0xCu ) /*0x5f84e0*/
        v7 = v92; /*0x5f84f0*/
      else
        v7 = v92 % 12; /*0x5f84ec*/
      v8 = StringPool::GetInstance(); /*0x5f84fd*/
      v9 = *StringPool::GetString(this: v8, result: &a2, nIdx: SP_2837_YOU_CAN_LOGIN_AFTER_R_NS_2D_4D_2D02D_S); /*0x5f8509*/
      LOBYTE(v103) = 1; /*0x5f8513*/
      v75 = dword_BED9F4[v92 > 0xCu]; /*0x5f851b*/
      p_result = v93; /*0x5f8526*/
      p_pvargDest = v7; /*0x5f852b*/
      v72 = v89; /*0x5f852c*/
      v71 = v91; /*0x5f8531*/
      ZXString<char>::Format(a1: v101, a2: v9, a3: *&dword_BED9FC[4 * ((HIWORD(v89) - 1) % 12)]); /*0x5f8549*/
      LOBYTE(v103) = 0; /*0x5f8551*/
    }
    ZXString<char>::_Release(a1: &a2); /*0x5f8585*/
    if ( v6 == 99 ) /*0x5f858d*/
    {
      result = 0; /*0x5f858f*/
      LOBYTE(v103) = 3; /*0x5f859e*/
      ZXString<char>::Format(a1: &result, a2: aYouHaveBeenBlo, a3: v101[0]); /*0x5f85a2*/
      v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f85aa*/
      p_result = 1; /*0x5f85ab*/
      p_pvargDest = 0; /*0x5f85ad*/
      v72 = 0; /*0x5f85ae*/
      v71 = v11; /*0x5f85af*/
      v98 = &v71; /*0x5f85b5*/
      sub_428211(a1: &result); /*0x5f85b9*/
      CUtilDlg::Notice( /*0x5f85be*/
        sMsg: v71,
        sSndName: &v72->vt,
        ppDialog: p_pvargDest,
        bAutoSeparated: p_result,
        bIsTightLine: v75);
      LOBYTE(v103) = 0; /*0x5f85c9*/
      ZXString<char>::_Release(a1: &result); /*0x5f85cc*/
LABEL_16:
      v103 = -1; /*0x5f85d1*/
      p_Unknown = v101; /*0x5f85d5*/
LABEL_73:
      ZXString<char>::_Release(a1: p_Unknown); /*0x5f8a05*/
      return; /*0x5f8a0a*/
    }
    sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x5f85e6*/
    LOBYTE(v103) = 4; /*0x5f85ef*/
    sub_402F85(pvargDest: &v88, pvargSrc: &pvargSrc); /*0x5f85f3*/
    v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f85f8*/
    p_result = 0; /*0x5f85f9*/
    p_pvargDest = &pvargDest; /*0x5f85fd*/
    v72 = &v88; /*0x5f8601*/
    v71 = v13; /*0x5f8602*/
    v98 = &v71; /*0x5f8605*/
    LOBYTE(v103) = 5; /*0x5f860e*/
    v14 = StringPool::GetInstance(); /*0x5f8612*/
    StringPool::GetBSTR(this: v14, a2: &v71, a3: SP_2836_ETC_BLOCKREASONIMG); /*0x5f8619*/
    LOBYTE(v103) = 5; /*0x5f8625*/
    v15 = sub_406472(); /*0x5f8629*/
    v16 = sub_402680(a1: v15); /*0x5f8630*/
    IWzResMan::GetObjectA(this: v16, pvargDest: &pvarg, sUOL: v71, vParam: v72, vAux: p_pvargDest); /*0x5f8637*/
    LOBYTE(v103) = 7; /*0x5f863e*/
    a2 = Ztl_variant_t::GetUnknown(a1: p_result, a2: v75); /*0x5f8649*/
    v97 = 0; /*0x5f864c*/
    if ( a2 != 0 ) /*0x5f8654*/
    {
      v17 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &a2); /*0x5f865d*/
      if ( v17 < 0 && v17 != -2147467262 ) /*0x5f8668*/
        _com_issue_error(a1: v17); /*0x5f866b*/
    }
    LOBYTE(v103) = 11; /*0x5f8677*/
    v18 = ZComAPI::ZComVariantCopy(&pvarg); /*0x5f867b*/
    if ( v18 < 0 ) /*0x5f8683*/
      _com_issue_error(a1: v18); /*0x5f8686*/
    LOBYTE(v103) = 10; /*0x5f868f*/
    v19 = ZComAPI::ZComVariantCopy(pvarg: &v88); /*0x5f8693*/
    if ( v19 < 0 ) /*0x5f869b*/
      _com_issue_error(a1: v19); /*0x5f869e*/
    LOBYTE(v103) = 9; /*0x5f86a7*/
    v20 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x5f86ab*/
    if ( v20 < 0 ) /*0x5f86b3*/
      _com_issue_error(a1: v20); /*0x5f86b6*/
    if ( v97 != 0 )
    {
      v21 = alloca(32); /*0x5f86c7*/
      v67 = 0; /*0x5f86ce*/
      v66 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f86cf*/
      v22 = _Int2StrW(a1: v6, a2: &v68); /*0x5f86d2*/
      v98 = &v65; /*0x5f86da*/
      _bstr_t::_bstr_t(a1: v22); /*0x5f86de*/
      LOBYTE(v103) = 9; /*0x5f86ed*/
      sub_40263B(a1: &v97); /*0x5f86f1*/
      IWzProperty::GetItem(pvargDest: &v79, a2: v65); /*0x5f86f8*/
      LOBYTE(v103) = 13; /*0x5f86ff*/
      Unknown = Ztl_variant_t::GetUnknown(a1: v66, a2: v67); /*0x5f870a*/
      result = 0; /*0x5f870d*/
      if ( Unknown != 0 ) /*0x5f8710*/
      {
        v23 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown); /*0x5f8719*/
        if ( v23 < 0 && v23 != -2147467262 ) /*0x5f8724*/
          _com_issue_error(a1: v23); /*0x5f8727*/
      }
      LOBYTE(v103) = 15; /*0x5f8733*/
      v24 = ZComAPI::ZComVariantCopy(pvarg: &v79); /*0x5f8737*/
      if ( v24 < 0 ) /*0x5f873f*/
        _com_issue_error(a1: v24); /*0x5f8742*/
      v100 = 0; /*0x5f8747*/
      LOBYTE(v103) = 16; /*0x5f874d*/
      if ( result != 0 )
      {
        v98 = &v67; /*0x5f8756*/
        v66 = SP_1587_MSG; /*0x5f8759*/
        v65 = &v67; /*0x5f875e*/
        v25 = StringPool::GetInstance(); /*0x5f875f*/
        StringPool::GetBSTR(this: v25, a2: v65, a3: v66); /*0x5f8766*/
        v66 = &v77; /*0x5f8771*/
        LOBYTE(v103) = 16; /*0x5f8775*/
        sub_40263B(a1: &result); /*0x5f8779*/
        Item = IWzProperty::GetItem(pvargDest: v66, a2: v67); /*0x5f8780*/
        v27 = *Item == 8; /*0x5f8785*/
        LOBYTE(v103) = 18; /*0x5f8789*/
        v28 = v27 ? *(Item + 8) : &word_AF0E08;
        ZXString<char>::Assign<unsigned short>(this: &v100, a2: v28, a3: -1); /*0x5f879f*/
        LOBYTE(v103) = 16; /*0x5f87ab*/
        v29 = ZComAPI::ZComVariantCopy(pvarg: &v77); /*0x5f87af*/
        if ( v29 < 0 ) /*0x5f87b7*/
          _com_issue_error(a1: v29); /*0x5f87ba*/
      }
      v67 = 0; /*0x5f87bf*/
      v66 = SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID; /*0x5f87c0*/
      v65 = 0; /*0x5f87c2*/
      v64 = 0; /*0x5f87c3*/
      sub_47C957(a1: &v95, a2: asc_AF1E28); /*0x5f87d0*/
      v63 = v30; /*0x5f87d5*/
      v98 = &v63; /*0x5f87d8*/
      LOBYTE(v103) = 19; /*0x5f87e2*/
      ZXString<char>::operator+(a1: &v63, a2: v101); /*0x5f87e6*/
      CUtilDlg::Notice(sMsg: v63, sSndName: v64, ppDialog: v65, bAutoSeparated: v66, bIsTightLine: v67); /*0x5f87eb*/
      LOBYTE(v103) = 16; /*0x5f87f6*/
      ZXString<char>::_Release(a1: &v95); /*0x5f87fa*/
      LOBYTE(v103) = 15; /*0x5f8802*/
      ZXString<char>::_Release(a1: &v100); /*0x5f8806*/
      LOBYTE(v103) = 9; /*0x5f8810*/
      if ( result != 0 ) /*0x5f8814*/
        (*(*result + 8))(a1: result); /*0x5f8819*/
      LOBYTE(v103) = 0; /*0x5f8821*/
      if ( v97 != 0 ) /*0x5f8824*/
        (*(*v97 + 8))(a1: v97); /*0x5f882d*/
      goto LABEL_16; /*0x5f8830*/
    }
    v103 = -1; /*0x5f8835*/
    ZXString<char>::_Release(a1: v101); /*0x5f883c*/
  }
  if ( v99 == 27 ) /*0x5f8847*/
  {
    v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f884d*/
    p_result = 0; /*0x5f884e*/
    p_pvargDest = 0; /*0x5f884f*/
    v72 = 0; /*0x5f8850*/
    v71 = v5; /*0x5f8851*/
    result = &v71; /*0x5f8854*/
    v31 = StringPool::GetInstance(); /*0x5f885d*/
    StringPool::GetString( /*0x5f8864*/
      this: v31,
      result: &v71,
      nIdx: SP_4836_PLEASE_DOWNLOAD_THE_FULL_CLIENT_TO_EXPERIENCE__R_NTHE_WORLD_OF_MAPLESTORY__R_NWO);
    if ( CUtilDlg::YesNo(a1: v71, a2: &v72->vt, a3: p_pvargDest, a4: p_result, a5: v75) == 6 ) /*0x5f8874*/
    {
      v75 = SP_3_HTTP__MAPLESTORYNEXONNET_WZASPXPART_DOWNLOADS_GAMEPATCHES; /*0x5f8879*/
      p_result = &result; /*0x5f887b*/
      v33 = StringPool::GetInstance(); /*0x5f887c*/
      v34 = *StringPool::GetString(this: v33, result: p_result, nIdx: v75); /*0x5f8888*/
      v103 = 20; /*0x5f888c*/
      open_web_site(a1: v34, lpWindowName: 0); /*0x5f8893*/
      v103 = -1; /*0x5f8898*/
      ZXString<char>::_Release(a1: &result); /*0x5f88a1*/
      v98 = 570425354; /*0x5f88ab*/
      result = 570425354; /*0x5f88ae*/
      v103 = 21; /*0x5f88ba*/
      _CxxThrowException(a1: &result, a2: &_TI3_AVCTerminateException__); /*0x5f88c1*/
    }
    v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f88c6*/
    p_result = 0; /*0x5f88c7*/
    p_pvargDest = 0; /*0x5f88c8*/
    v72 = 0; /*0x5f88c9*/
    v71 = v32; /*0x5f88ca*/
    result = &v71; /*0x5f88cd*/
    v35 = StringPool::GetInstance(); /*0x5f88d6*/
    StringPool::GetString( /*0x5f88dd*/
      this: v35,
      result: &v71,
      nIdx: SP_4822_IF_YOU_WISH_TO_DOWNLOAD_THE_FULL_CLIENT_LATER__R_NYOU_CAN_ALWAYS_VISIT_MAPLESTOR);
    CUtilDlg::Notice(sMsg: v71, sSndName: &v72->vt, ppDialog: p_pvargDest, bAutoSeparated: p_result, bIsTightLine: v75); /*0x5f88e2*/
    return; /*0x5f88ea*/
  }
  if ( v99 > 11 ) /*0x5f88f2*/
  {
    switch ( v99 ) /*0x5f894f*/
    {
      case 13: /*0x5f894f*/
        v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f8a0f*/
        p_result = 21; /*0x5f8a10*/
LABEL_75:
        CLoginUtilDlg::Error(a1: p_result, a2: v75); /*0x5f8a12*/
        goto LABEL_76; /*0x5f8a12*/
      case 14: /*0x5f894f*/
        if ( CLoginUtilDlg::YesNo2(a1: 27) == 0 ) /*0x5f89d1*/
          return; /*0x5f89d1*/
        v75 = SP_2871_HTTP__WWWNEXONNET; /*0x5f89da*/
        p_result = &Unknown; /*0x5f89df*/
        v38 = StringPool::GetInstance(); /*0x5f89e0*/
        v39 = *StringPool::GetString(this: v38, result: p_result, nIdx: v75); /*0x5f89ec*/
        v103 = 22; /*0x5f89f0*/
        open_web_site(a1: v39, lpWindowName: 0); /*0x5f89f7*/
        v103 = -1; /*0x5f89fc*/
        p_Unknown = &Unknown; /*0x5f8a02*/
        goto LABEL_73; /*0x5f8a02*/
      case 15: /*0x5f894f*/
        if ( CLoginUtilDlg::YesNo2(a1: 26) == 0 ) /*0x5f8991*/
          return; /*0x5f8991*/
        v75 = SP_2871_HTTP__WWWNEXONNET; /*0x5f899a*/
        p_result = &a2; /*0x5f899f*/
        v36 = StringPool::GetInstance(); /*0x5f89a0*/
        v37 = *StringPool::GetString(this: v36, result: p_result, nIdx: v75); /*0x5f89ac*/
        v103 = 23; /*0x5f89b0*/
        open_web_site(a1: v37, lpWindowName: 0); /*0x5f89b7*/
        v103 = -1; /*0x5f89bc*/
        p_Unknown = &a2; /*0x5f89c2*/
        goto LABEL_73; /*0x5f89c5*/
      default:
        break; /*0x5f895c*/
    }
    if ( v99 != 16 ) /*0x5f895c*/
    {
      if ( v99 == 17 ) /*0x5f895f*/
      {
        v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f8977*/
        p_result = 27; /*0x5f8978*/
        goto LABEL_107; /*0x5f897a*/
      }
      if ( v99 != 21 ) /*0x5f8964*/
      {
        if ( v99 != 25 ) /*0x5f8969*/
          goto LABEL_76; /*0x5f8969*/
        v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f896f*/
        p_result = 40; /*0x5f8970*/
LABEL_107:
        CLoginUtilDlg::Error(a1: p_result, a2: v75); /*0x5f8d78*/
        return; /*0x5f8d78*/
      }
    }
    v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f897f*/
    p_result = 33; /*0x5f8980*/
    goto LABEL_107; /*0x5f8982*/
  }
  if ( v99 == 11 ) /*0x5f88f4*/
  {
    v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f8944*/
    p_result = 14; /*0x5f8945*/
    goto LABEL_107; /*0x5f8947*/
  }
LABEL_51:
  switch ( v99 ) /*0x5f8903*/
  {
    case -1: /*0x5f8903*/
    case 6: /*0x5f8903*/
    case 8: /*0x5f8903*/
    case 9: /*0x5f8903*/
      v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f890a*/
      p_result = 15; /*0x5f890b*/
      goto LABEL_75; /*0x5f890d*/
    case 2: /*0x5f8903*/
    case 3: /*0x5f8903*/
      v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f8912*/
      p_result = 16; /*0x5f8913*/
      goto LABEL_75; /*0x5f8915*/
    case 4: /*0x5f8903*/
      v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f8922*/
      p_result = 3; /*0x5f8923*/
      goto LABEL_75; /*0x5f8925*/
    case 5: /*0x5f8903*/
      v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f891a*/
      p_result = 20; /*0x5f891b*/
      goto LABEL_75; /*0x5f891d*/
    case 7: /*0x5f8903*/
      CLogin::GotoTitle(this: v100, a2: 0, a3: 0); /*0x5f892f*/
      v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f8934*/
      p_result = 17; /*0x5f8935*/
      goto LABEL_75; /*0x5f8937*/
    case 10: /*0x5f8903*/
      v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f893c*/
      p_result = 19; /*0x5f893d*/
      goto LABEL_75; /*0x5f893f*/
    default:
      break;
  }
LABEL_76:
  v40 = v99; /*0x5f8a19*/
  if ( v99 != 0 && v99 != 12 && v99 != 23 ) /*0x5f8a28*/
    return; /*0x5f8a28*/
  v41 = v100; /*0x5f8a2e*/
  v42 = *(v100 + 456); /*0x5f8a31*/
  if ( v42 <= 1u ) /*0x5f8a39*/
  {
    if ( *dword_BEDA60 != 0 ) /*0x5f8a99*/
      CUITitle::EnableLoginCtrl(this: *dword_BEDA60, a2: 0); /*0x5f8a9c*/
    if ( v40 == 23 ) /*0x5f8aa4*/
    {
      result = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 172); /*0x5f8ab5*/
      v103 = 25; /*0x5f8aba*/
      if ( result != 0 ) /*0x5f8ac1*/
        sub_6211B6(a1: v41); /*0x5f8aca*/
      return; /*0x5f8acf*/
    }
    v45 = result; /*0x5f8ad4*/
    a2 = CInPacket::Decode4(this: result); /*0x5f8ae0*/
    v46 = CInPacket::Decode1(this: v45); /*0x5f8ae8*/
    v98 = v46; /*0x5f8aed*/
    LOBYTE(Unknown) = CInPacket::Decode1(this: v45); /*0x5f8af7*/
    LOBYTE(v99) = CInPacket::Decode1(this: v45); /*0x5f8b01*/
    LOBYTE(v95) = CInPacket::Decode1(this: v45); /*0x5f8b09*/
    CInPacket::DecodeStr(a1: v94); /*0x5f8b12*/
    v103 = 26; /*0x5f8b19*/
    v101[0] = CInPacket::Decode1(this: v45); /*0x5f8b27*/
    LOBYTE(v97) = CInPacket::Decode1(this: v45); /*0x5f8b2f*/
    CInPacket::DecodeBuffer(a1: v83, a2: 8u); /*0x5f8b3a*/
    dword_BF03B8(a1: v83, a2: v81); /*0x5f8b4a*/
    CInPacket::DecodeBuffer(a1: v84, a2: 8u); /*0x5f8b58*/
    dword_BF03B8(a1: v84, a2: v82); /*0x5f8b68*/
    v47 = CInPacket::Decode4(this: v45); /*0x5f8b78*/
    if ( v46 == 10 ) /*0x5f8b7a*/
    {
      if ( *dword_BEDA60 != 0 ) /*0x5f8b88*/
        CUITitle::EnableLoginCtrl(this: *dword_BEDA60, a2: 1); /*0x5f8b8c*/
      if ( *dword_BEDA70 == 0 ) /*0x5f8b97*/
      {
        v48 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 356); /*0x5f8ba3*/
        result = v48; /*0x5f8ba8*/
        LOBYTE(v103) = 27; /*0x5f8bad*/
        if ( v48 != 0 ) /*0x5f8bb1*/
          sub_61301F(a1: v48, a2: v49, a3: v100); /*0x5f8bb8*/
        LOBYTE(v103) = 26; /*0x5f8bbd*/
      }
      result = &v75; /*0x5f8bc4*/
      v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f8bce*/
      ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF); /*0x5f8bd0*/
      p_result = v47; /*0x5f8bd5*/
      v69 = v99; /*0x5f8bdb*/
      v70 = v82[0]; /*0x5f8be4*/
      v71 = v82[1]; /*0x5f8be5*/
      v72 = v82[2]; /*0x5f8be6*/
      p_pvargDest = v82[3]; /*0x5f8be7*/
      v64 = v97; /*0x5f8bed*/
      v63 = *v101; /*0x5f8bf6*/
      v65 = v81[0]; /*0x5f8bf9*/
      v66 = v81[1]; /*0x5f8bfa*/
      v62 = v50; /*0x5f8bfb*/
      v67 = v81[2]; /*0x5f8bfc*/
      v98 = &v62; /*0x5f8c02*/
      LOBYTE(v103) = 28; /*0x5f8c06*/
      v68 = v81[3]; /*0x5f8c0a*/
      sub_428211(a1: v94); /*0x5f8c0b*/
      v61 = v95; /*0x5f8c10*/
      LOBYTE(v103) = 26; /*0x5f8c13*/
      v59 = Unknown; /*0x5f8c17*/
      v56 = a2; /*0x5f8c1c*/
      sub_42861D(); /*0x5f8c1f*/
      CWvsContext::SetAccountInfo( /*0x5f8c26*/
        a1: v56,
        a2: 10,
        a3: v59,
        a4: v61,
        a5: v62,
        a6: v63,
        a7: v64,
        a8: v65,
        a9: v66,
        a10: v67,
        a11: v68,
        a12: v69,
        a13: v70,
        a14: v71,
        a15: v72,
        a16: p_pvargDest,
        a17: p_result,
        a18: v75);
      goto LABEL_97; /*0x5f8c26*/
    }
    if ( *dword_BEDA70 != 0 ) /*0x5f8c3f*/
    {
      CWnd::Destroy(a1: *dword_BEDA70); /*0x5f8c41*/
      if ( *dword_BEDA70 != 0 ) /*0x5f8c4d*/
        (**(*dword_BEDA70 + 8))(a1: *dword_BEDA70 + 8, a2: 1); /*0x5f8c56*/
    }
    v88.cyVal.Hi = &p_result; /*0x5f8c5b*/
    p_result = 0; /*0x5f8c65*/
    ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF); /*0x5f8c67*/
    p_pvargDest = v47; /*0x5f8c6c*/
    v68 = v99; /*0x5f8c72*/
    v69 = v82[0]; /*0x5f8c7b*/
    v70 = v82[1]; /*0x5f8c7c*/
    v71 = v82[2]; /*0x5f8c7d*/
    v72 = v82[3]; /*0x5f8c7e*/
    v63 = v97; /*0x5f8c84*/
    v62 = *v101; /*0x5f8c8d*/
    v64 = v81[0]; /*0x5f8c90*/
    v65 = v81[1]; /*0x5f8c91*/
    v61 = v51; /*0x5f8c92*/
    v66 = v81[2]; /*0x5f8c93*/
    v99 = &v61; /*0x5f8c99*/
    LOBYTE(v103) = 30; /*0x5f8c9d*/
    v67 = v81[3]; /*0x5f8ca1*/
    sub_428211(a1: v94); /*0x5f8ca2*/
    v60 = v95; /*0x5f8ca7*/
    LOBYTE(v103) = 26; /*0x5f8caa*/
    v58 = Unknown; /*0x5f8cae*/
    v57 = v98; /*0x5f8cb1*/
    v55 = a2; /*0x5f8cb4*/
    sub_42861D(); /*0x5f8cb7*/
    CWvsContext::SetAccountInfo( /*0x5f8cbe*/
      a1: v55,
      a2: v57,
      a3: v58,
      a4: v60,
      a5: v61,
      a6: v62,
      a7: v63,
      a8: v64,
      a9: v65,
      a10: v66,
      a11: v67,
      a12: v68,
      a13: v69,
      a14: v70,
      a15: v71,
      a16: v72,
      a17: p_pvargDest,
      a18: p_result);
    v52 = CInPacket::Decode1(this: result); /*0x5f8cce*/
    v53 = CInPacket::Decode1(this: result); /*0x5f8cd1*/
    *(v100 + 372) = v53; /*0x5f8cdb*/
    if ( v52 != 0 ) /*0x5f8ce1*/
    {
      COutPacket::COutPacket(a1: 11); /*0x5f8d4d*/
      LOBYTE(v103) = 33; /*0x5f8d5c*/
      CClientSocket::SendPacket(this: dword_BE7914, a2: &pvargDest); /*0x5f8d60*/
      LOBYTE(v103) = 26; /*0x5f8d65*/
      p_Hi32 = &pvargDest.decVal.Hi32; /*0x5f8d69*/
    }
    else
    {
      if ( *(TSingleton<CWvsApp>::ms_pInstance + 36) == 1 ) /*0x5f8cec*/
      {
LABEL_97:
        v103 = -1; /*0x5f8c2b*/
        p_Unknown = v94; /*0x5f8c2f*/
        goto LABEL_73; /*0x5f8c32*/
      }
      COutPacket::COutPacket(a1: 9); /*0x5f8cf7*/
      LOBYTE(v103) = 32; /*0x5f8d01*/
      COutPacket::Encode1(this: &v89, a2: 1); /*0x5f8d05*/
      COutPacket::Encode1(this: &v89, a2: 1); /*0x5f8d0f*/
      result = &p_result; /*0x5f8d17*/
      p_result = 0; /*0x5f8d21*/
      ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF); /*0x5f8d23*/
      COutPacket::EncodeStr(a1: p_result); /*0x5f8d2b*/
      CClientSocket::SendPacket(this: dword_BE7914, a2: &v89); /*0x5f8d3a*/
      LOBYTE(v103) = 26; /*0x5f8d3f*/
      p_Hi32 = &v90; /*0x5f8d43*/
    }
    ZArray<unsigned char>::RemoveAll(a1: p_Hi32); /*0x5f8d6c*/
    goto LABEL_97; /*0x5f8d71*/
  }
  v75 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x5f8a3d*/
  if ( v42 > 3u ) /*0x5f8a3e*/
  {
    p_result = 15; /*0x5f8d76*/
    goto LABEL_107; /*0x5f8d76*/
  }
  if ( CLoginUtilDlg::YesNo(a1: 31, a2: v75) != 0 && *(v41 + 90) == 0 ) /*0x5f8a5b*/
  {
    v75 = SP_7_HTTP__PASSPORTNEXONNET_PART_MYMAPLE_VERIFYCODE; /*0x5f8a64*/
    p_result = &result; /*0x5f8a66*/
    v43 = StringPool::GetInstance(); /*0x5f8a67*/
    v44 = *StringPool::GetString(this: v43, result: p_result, nIdx: v75); /*0x5f8a73*/
    v103 = 24; /*0x5f8a77*/
    open_web_site(a1: v44, lpWindowName: 0); /*0x5f8a7e*/
    v103 = -1; /*0x5f8a83*/
    p_Unknown = &result; /*0x5f8a89*/
    goto LABEL_73; /*0x5f8a8c*/
  }
}
```

## CLogin::OnWorldInformation @ 0x5f95b7

```c
void __thiscall CLogin::OnWorldInformation(CLogin *this, int a2)
{
  CInPacket *v2; // edi
  int *v4; // esi
  int v5; // eax
  int v6; // eax
  unsigned __int8 v7; // al
  _DWORD *v8; // esi
  int v9; // eax
  unsigned __int8 v10; // al
  bool v11; // zf
  int v12; // eax
  int v13; // esi
  int v14; // eax
  _DWORD v15[2]; // [esp+Ch] [ebp-20h] BYREF
  int v16; // [esp+14h] [ebp-18h] BYREF
  int v17; // [esp+18h] [ebp-14h] BYREF
  _DWORD *v18; // [esp+1Ch] [ebp-10h] BYREF
  int v19; // [esp+28h] [ebp-4h]

  v2 = a2; /*0x5f95c7*/
  a2 = CInPacket::Decode1(this: a2); /*0x5f95d8*/
  if ( a2 >= 0 ) /*0x5f95db*/
  {
    v4 = sub_5FDE1F(a1: -1); /*0x5f962f*/
    *v4 = a2; /*0x5f9634*/
    v5 = CInPacket::DecodeStr(a1: &a2); /*0x5f963c*/
    ZXString<char>::operator=(a1: v5); /*0x5f964c*/
    v19 = -1; /*0x5f9654*/
    if ( a2 != 0 ) /*0x5f965a*/
      ZXString<char>::_Release(lpAddend: (a2 - 12)); /*0x5f9660*/
    v4[2] = CInPacket::Decode1(this: v2); /*0x5f9670*/
    v6 = CInPacket::DecodeStr(a1: &a2); /*0x5f9679*/
    ZXString<char>::operator=(a1: v6); /*0x5f9689*/
    v19 = -1; /*0x5f9691*/
    if ( a2 != 0 ) /*0x5f9697*/
      ZXString<char>::_Release(lpAddend: (a2 - 12)); /*0x5f969d*/
    v4[4] = CInPacket::Decode2(this: v2); /*0x5f96af*/
    v4[5] = CInPacket::Decode2(this: v2); /*0x5f96bc*/
    v4[6] = CInPacket::Decode1(this: v2); /*0x5f96c9*/
    v7 = CInPacket::Decode1(this: v2); /*0x5f96cc*/
    if ( v7 != 0 ) /*0x5f96d6*/
    {
      v18 = v4 + 7; /*0x5f96db*/
      a2 = v7; /*0x5f96de*/
      do /*0x5f9750*/
      {
        v8 = sub_5FDC66(a1: -1); /*0x5f96eb*/
        v9 = CInPacket::DecodeStr(a1: &v17); /*0x5f96f3*/
        v19 = 3; /*0x5f96fb*/
        ZXString<char>::operator=(a1: v9); /*0x5f9702*/
        v19 = -1; /*0x5f970a*/
        if ( v17 != 0 ) /*0x5f9710*/
          ZXString<char>::_Release(lpAddend: (v17 - 12)); /*0x5f9716*/
        v8[1] = CInPacket::Decode4(this: v2); /*0x5f9725*/
        v8[2] = CInPacket::Decode1(this: v2); /*0x5f9732*/
        v8[3] = CInPacket::Decode1(this: v2); /*0x5f973f*/
        v10 = CInPacket::Decode1(this: v2); /*0x5f9742*/
        v11 = a2-- == 1; /*0x5f9747*/
        v8[4] = v10; /*0x5f974d*/
      }
      while ( !v11 ); /*0x5f9750*/
    }
    *(this + 128) = CInPacket::Decode2(this: v2); /*0x5f9762*/
    sub_5FDF26(a1: this + 516); /*0x5f9768*/
    v12 = *(this + 128); /*0x5f976d*/
    if ( v12 != 0 ) /*0x5f9775*/
    {
      v13 = 0; /*0x5f9777*/
      if ( v12 > 0 ) /*0x5f977b*/
      {
        do /*0x5f97ea*/
        {
          v16 = 0; /*0x5f977d*/
          v19 = 4; /*0x5f9783*/
          v15[0] = CInPacket::Decode2(this: v2); /*0x5f9794*/
          v15[1] = CInPacket::Decode2(this: v2); /*0x5f979f*/
          v14 = CInPacket::DecodeStr(a1: &v18); /*0x5f97a8*/
          LOBYTE(v19) = 5; /*0x5f97b1*/
          ZXString<char>::operator=(a1: v14); /*0x5f97b5*/
          LOBYTE(v19) = 4; /*0x5f97bd*/
          ZXString<char>::_Release(a1: &v18); /*0x5f97c1*/
          sub_5FD8B5(a1: v15, a2: -1); /*0x5f97d2*/
          v19 = -1; /*0x5f97d7*/
          ZXString<char>::_Release(a1: &v16); /*0x5f97de*/
          ++v13; /*0x5f97e3*/
        }
        while ( v13 < *(this + 128) ); /*0x5f97ea*/
      }
    }
  }
  else
  {
    if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 ) /*0x5f95e5*/
    {
      CUIWorldSelect::DrawWorldItems(this: TSingleton<CUIWorldSelect>::ms_pInstance); /*0x5f960f*/
    }
    else
    {
      a2 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 400); /*0x5f95f6*/
      v19 = 0; /*0x5f95f9*/
      if ( a2 != 0 ) /*0x5f95ff*/
        sub_6246CE(a1: this); /*0x5f9604*/
      v19 = -1; /*0x5f9609*/
    }
    CLogin::ChangeStep(this, a2: 1); /*0x5f9618*/
  }
}
```

## CLogin::OnSelectWorldResult @ 0x5f9891

```c
void __thiscall CLogin::OnSelectWorldResult(CLogin *this, struct CInPacket *a2)
{
  unsigned __int8 v3; // al
  int v4; // ebx
  _DWORD *v5; // eax
  char *v6; // eax
  int *v7; // ecx
  _DWORD *Instance; // eax
  char *v9; // eax
  int v10; // eax
  _DWORD *v11; // ecx
  int v12; // ebx
  int v13; // edi
  _DWORD *v14; // edi
  int v15; // eax
  _DWORD *v16; // eax
  int v17; // edx
  _DWORD *v18; // eax
  int v19; // edx
  int v20; // [esp+Ch] [ebp-2Ch] BYREF
  _BYTE v21[12]; // [esp+10h] [ebp-28h] BYREF
  int v22; // [esp+1Ch] [ebp-1Ch]
  int v23; // [esp+20h] [ebp-18h] BYREF
  int v24; // [esp+24h] [ebp-14h] BYREF
  int v25; // [esp+28h] [ebp-10h]
  int v26; // [esp+34h] [ebp-4h]

  *(this + 92) = 0; /*0x5f98a5*/
  if ( dword_BEDA78 != 0 ) /*0x5f98b3*/
  {
    CWnd::Destroy(a1: dword_BEDA78); /*0x5f98b5*/
    if ( dword_BEDA78 != 0 ) /*0x5f98c1*/
      (**(dword_BEDA78 + 8))(a1: dword_BEDA78 + 8, a2: 1); /*0x5f98ca*/
  }
  if ( dword_BEDA74 != 0 ) /*0x5f98d2*/
    (*(**(TSingleton<CUIChannelSelect>::ms_pInstance + 268) + 52))( /*0x5f98e3*/
      a1: *(TSingleton<CUIChannelSelect>::ms_pInstance + 268),
      a2: 3);
  v3 = CInPacket::Decode1(this: a2); /*0x5f98e9*/
  v4 = v3; /*0x5f98f0*/
  if ( v3 > 0xBu ) /*0x5f98f5*/
  {
    if ( v3 != 13 ) /*0x5f994f*/
    {
      switch ( v3 ) /*0x5f9956*/
      {
        case 0xEu: /*0x5f9956*/
          if ( CLoginUtilDlg::YesNo2(a1: 27) == 0 ) /*0x5f99d2*/
            return; /*0x5f99d2*/
          Instance = StringPool::GetInstance(); /*0x5f99e1*/
          v9 = *StringPool::GetString(this: Instance, result: &v24, nIdx: SP_2871_HTTP__WWWNEXONNET); /*0x5f99ed*/
          v26 = 0; /*0x5f99f1*/
          open_web_site(a1: v9, lpWindowName: 0); /*0x5f99f4*/
          v26 = -1; /*0x5f99f9*/
          v7 = &v24; /*0x5f99ff*/
          break;
        case 0xFu: /*0x5f9956*/
          if ( CLoginUtilDlg::YesNo2(a1: 26) == 0 ) /*0x5f9992*/
            return; /*0x5f9992*/
          v5 = StringPool::GetInstance(); /*0x5f99a1*/
          v6 = *StringPool::GetString(this: v5, result: &v23, nIdx: SP_2871_HTTP__WWWNEXONNET); /*0x5f99ad*/
          v26 = 1; /*0x5f99b1*/
          open_web_site(a1: v6, lpWindowName: 0); /*0x5f99b8*/
          v26 = -1; /*0x5f99bd*/
          v7 = &v23; /*0x5f99c3*/
          break;
        case 0x10u: /*0x5f9956*/
          CLoginUtilDlg::Error(a1: 33, a2: 0); /*0x5f997c*/
          return; /*0x5f997c*/
        case 0x11u: /*0x5f9956*/
          CLoginUtilDlg::Error(a1: 27, a2: 0); /*0x5f9977*/
          return; /*0x5f9977*/
        case 0x15u: /*0x5f9956*/
          break;
        case 0x19u: /*0x5f9956*/
          CLoginUtilDlg::Error(a1: 40, a2: 0); /*0x5f9972*/
          return; /*0x5f9972*/
        default:
          goto LABEL_33; /*0x5f9969*/
      }
      ZXString<char>::_Release(a1: v7); /*0x5f9a02*/
      return; /*0x5f9a07*/
    }
    CLoginUtilDlg::Error(a1: 21, a2: 0); /*0x5f9a0f*/
  }
  else
  {
    if ( v3 == 11 ) /*0x5f98f7*/
    {
      CLoginUtilDlg::Error(a1: 14, a2: 0); /*0x5f9948*/
      return; /*0x5f9948*/
    }
    switch ( v3 ) /*0x5f9905*/
    {
      case 0xFF: /*0x5f9905*/
      case 6u: /*0x5f9905*/
      case 8u: /*0x5f9905*/
      case 9u: /*0x5f9905*/
        CLoginUtilDlg::Error(a1: 15, a2: 0); /*0x5f990f*/
        break; /*0x5f990f*/
      case 2u: /*0x5f9905*/
      case 3u: /*0x5f9905*/
        CLoginUtilDlg::Error(a1: 16, a2: 0); /*0x5f9917*/
        break; /*0x5f9917*/
      case 4u: /*0x5f9905*/
        CLoginUtilDlg::Error(a1: 3, a2: 0); /*0x5f9927*/
        break; /*0x5f9927*/
      case 5u: /*0x5f9905*/
        CLoginUtilDlg::Error(a1: 20, a2: 0); /*0x5f991f*/
        break; /*0x5f991f*/
      case 7u: /*0x5f9905*/
        CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5f9930*/
        CLoginUtilDlg::Error(a1: 17, a2: 0); /*0x5f9938*/
        break; /*0x5f9938*/
      case 0xAu: /*0x5f9905*/
        CLoginUtilDlg::Error(a1: 19, a2: 0); /*0x5f9940*/
        break; /*0x5f9940*/
      default:
        break;
    }
  }
LABEL_33:
  if ( v4 == 0 || v4 == 12 || v4 == 23 ) /*0x5f9a22*/
  {
    v10 = *dword_BE7918; /*0x5f9a28*/
    v11 = (*dword_BE7918 + 8276); /*0x5f9a33*/
    *(this + 130) = *(*dword_BE7918 + 8276); /*0x5f9a39*/
    v22 = sub_49CF38(a1: *v11, a2: *(v10 + 8232)); /*0x5f9a57*/
    v12 = CInPacket::Decode1(this: a2); /*0x5f9a61*/
    v25 = 0; /*0x5f9a68*/
    v24 = 0; /*0x5f9a6b*/
    v23 = 0; /*0x5f9a6e*/
    *(*dword_BE7918 + 8344) = v12; /*0x5f9a71*/
    do /*0x5f9b23*/
    {
      if ( v25 >= v12 ) /*0x5f9a7a*/
      {
        *(*(this + 101) + v24) = 0; /*0x5f9c3a*/
      }
      else
      {
        v13 = *(this + 101) + v24; /*0x5f9a83*/
        GW_CharacterStat::Decode(this: v13, a2, a3: 0); /*0x5f9a90*/
        AvatarLook::Decode(this: (v13 + 231), a2); /*0x5f9a9e*/
        v14 = (*(this + 103) + 4 * v25); /*0x5f9aac*/
        *v14 = CInPacket::Decode1(this: a2); /*0x5f9abd*/
        if ( CInPacket::Decode1(this: a2) != 0 ) /*0x5f9acb*/
          CInPacket::DecodeBuffer(a1: (v23 + *(this + 102)), a2: 0x10u); /*0x5f9ada*/
        else
          memset(a1: (v23 + *(this + 102)), a2: 0, a3: 0x10u); /*0x5f9aed*/
        if ( *(v24 + *(this + 101)) == v22 ) /*0x5f9b04*/
          *(this + 100) = v25; /*0x5f9b09*/
      }
      ++v25; /*0x5f9b11*/
      v23 += 16; /*0x5f9b14*/
      v24 += 684; /*0x5f9b18*/
    }
    while ( v25 < 15 ); /*0x5f9b23*/
    *(this + 372) = CInPacket::Decode1(this: a2); /*0x5f9b34*/
    v15 = CInPacket::Decode4(this: a2); /*0x5f9b3a*/
    *(this + 95) = v15; /*0x5f9b3f*/
    *(*dword_BE7918 + 8348) = v15; /*0x5f9b4b*/
    v16 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 144); /*0x5f9b5d*/
    v26 = 2; /*0x5f9b67*/
    if ( v16 != 0 ) /*0x5f9b6e*/
      CUICharSelect::CUICharSelect(a1: v16, a2: v17, a3: this); /*0x5f9b73*/
    v18 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 308); /*0x5f9b83*/
    v26 = 3; /*0x5f9b8d*/
    if ( v18 != 0 ) /*0x5f9b94*/
      sub_603FF0(a1: v18, a2: v19, a3: this, a4: *(this + 101)); /*0x5f9b9f*/
    v26 = -1; /*0x5f9baa*/
    CLogin::ChangeStep(this, a2: 0xFFFFFFFF); /*0x5f9bad*/
    if ( *(TSingleton<CWvsApp>::ms_pInstance + 36) == 1 && *(this + 372) == 0 ) /*0x5f9bc4*/
    {
      COutPacket::COutPacket(a1: 9); /*0x5f9bcb*/
      v26 = 4; /*0x5f9bd5*/
      COutPacket::Encode1(this: &v20, a2: 1); /*0x5f9bdc*/
      COutPacket::Encode1(this: &v20, a2: 1); /*0x5f9be6*/
      ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF); /*0x5f9bf9*/
      COutPacket::EncodeStr(a1: 0); /*0x5f9c01*/
      CClientSocket::SendPacket(this: dword_BE7914, a2: &v20); /*0x5f9c10*/
      v26 = -1; /*0x5f9c18*/
      ZArray<unsigned char>::RemoveAll(a1: v21); /*0x5f9c1b*/
    }
  }
}
```

## CLogin::OnCheckDuplicatedIDResult @ 0x5f9c72

```c
void __thiscall CLogin::OnCheckDuplicatedIDResult(CLogin *this, struct CInPacket *a2)
{
  char v3; // al
  int v4; // [esp+Ch] [ebp-14h] BYREF
  _BYTE v5[4]; // [esp+10h] [ebp-10h] BYREF
  int v6; // [esp+1Ch] [ebp-4h]

  *(this + 92) = 0; /*0x5f9c8c*/
  CInPacket::DecodeStr(a1: v5); /*0x5f9c92*/
  v6 = 0; /*0x5f9c9a*/
  v3 = CInPacket::Decode1(this: a2); /*0x5f9c9d*/
  v4 = 0; /*0x5f9ca5*/
  LOBYTE(v6) = 1; /*0x5f9cad*/
  if ( v3 == 1 ) /*0x5f9cb1*/
  {
    CLoginUtilDlg::Error(a1: 5, a2: 0); /*0x5f9cb6*/
  }
  else if ( v3 == 2 ) /*0x5f9cbb*/
  {
    CLoginUtilDlg::Error(a1: 10, a2: 0); /*0x5f9cc0*/
  }
  else if ( v3 != 0 ) /*0x5f9cc4*/
  {
    CLoginUtilDlg::Error(a1: 18, a2: 0); /*0x5f9ce6*/
  }
  else
  {
    ZXString<char>::operator=(a1: v5); /*0x5f9cd0*/
    *(this + 142) = 1; /*0x5f9cd5*/
    *(this + 143) = 1; /*0x5f9cdb*/
  }
  LOBYTE(v6) = 0; /*0x5f9cf0*/
  ZXString<char>::_Release(a1: &v4); /*0x5f9cf3*/
  v6 = -1; /*0x5f9cf8*/
  ZXString<char>::_Release(a1: v5); /*0x5f9cff*/
}
```

## CLogin::OnDeleteCharacterResult @ 0x5f9d15

```c
void __thiscall CLogin::OnDeleteCharacterResult(CLogin *this, int a2)
{
  struct CInPacket *v3; // ecx
  char v4; // al
  int v5; // ecx
  int v6; // edx
  _DWORD *Instance; // eax
  _DWORD *v8; // eax
  int v9; // ebx
  int v10; // ecx
  int v11; // eax
  int v12; // ecx
  int v13; // edi
  int v14; // ebx
  int v15; // [esp-14h] [ebp-30h] BYREF
  wchar_t *v16; // [esp-10h] [ebp-2Ch]
  int v17; // [esp-Ch] [ebp-28h]
  int v18; // [esp-8h] [ebp-24h]
  int v19; // [esp-4h] [ebp-20h]
  int v20; // [esp+Ch] [ebp-10h]
  int v21; // [esp+18h] [ebp-4h]

  v3 = a2; /*0x5f9d25*/
  *(this + 92) = 0; /*0x5f9d2a*/
  v20 = CInPacket::Decode4(this: v3); /*0x5f9d38*/
  v4 = CInPacket::Decode1(this: a2); /*0x5f9d3b*/
  v6 = v4; /*0x5f9d40*/
  switch ( v4 ) /*0x5f9d46*/
  {
    case 10: /*0x5f9d46*/
      v19 = 0; /*0x5f9d48*/
      v18 = 19; /*0x5f9d49*/
LABEL_20:
      CLoginUtilDlg::Error(a1: v18, a2: v19); /*0x5f9df8*/
      return; /*0x5f9dff*/
    case 22: /*0x5f9d46*/
      v19 = this + 500; /*0x5f9d5b*/
      v18 = 34; /*0x5f9d5c*/
      goto LABEL_20; /*0x5f9d5e*/
    case 29: /*0x5f9d46*/
      v19 = this + 500; /*0x5f9d6e*/
      v18 = 54; /*0x5f9d6f*/
      goto LABEL_20; /*0x5f9d71*/
    case 18: /*0x5f9d46*/
      v19 = 0; /*0x5f9d7b*/
      v18 = 11; /*0x5f9d7c*/
      goto LABEL_20; /*0x5f9d7e*/
    case 20: /*0x5f9d46*/
      v19 = this + 500; /*0x5f9d8b*/
      v18 = 93; /*0x5f9d8c*/
      goto LABEL_20; /*0x5f9d8e*/
    case 24: /*0x5f9d46*/
      v19 = this + 500; /*0x5f9d9b*/
      v18 = 39; /*0x5f9d9c*/
      goto LABEL_20; /*0x5f9d9e*/
    case 9: /*0x5f9d46*/
      v19 = this + 500; /*0x5f9dab*/
      v18 = 18; /*0x5f9dac*/
      goto LABEL_20; /*0x5f9dae*/
    case 26: /*0x5f9d46*/
      v19 = 0; /*0x5f9db5*/
      v18 = 0; /*0x5f9db6*/
      v17 = 0; /*0x5f9db7*/
      v16 = 0; /*0x5f9db8*/
      v15 = v5; /*0x5f9db9*/
      v20 = &v15; /*0x5f9dbc*/
      Instance = StringPool::GetInstance(); /*0x5f9dc5*/
      StringPool::GetString( /*0x5f9dcc*/
        this: Instance,
        result: &v15,
        nIdx: SP_4011_YOU_CANNOT_DELETE_A_CHARACTER_THAT_R_N_IS_CURRENTLY_GOING_THROUGH_THE_TRANSFER);
      CUtilDlg::Notice(sMsg: v15, sSndName: v16, ppDialog: v17, bAutoSeparated: v18, bIsTightLine: v19); /*0x5f9dd1*/
      return; /*0x5f9dd1*/
    case 6: /*0x5f9d46*/
      v19 = this + 500; /*0x5f9df5*/
      v18 = 15; /*0x5f9df6*/
      goto LABEL_20; /*0x5f9df6*/
    default:
      break; /*0x5f9e01*/
  }
  v8 = *(this + 101); /*0x5f9e01*/
  v9 = -1; /*0x5f9e07*/
  a2 = -1; /*0x5f9e0a*/
  v10 = 0; /*0x5f9e0d*/
  while ( v20 != *v8 ) /*0x5f9e14*/
  {
    ++v10; /*0x5f9e16*/
    v8 += 171; /*0x5f9e17*/
    if ( v10 >= 15 ) /*0x5f9e1f*/
      goto LABEL_26; /*0x5f9e1f*/
  }
  a2 = v10; /*0x5f9e23*/
  v9 = v10; /*0x5f9e26*/
LABEL_26:
  --*(*dword_BE7918 + 8344); /*0x5f9e28*/
  if ( v6 != 0 || v9 < 0 ) /*0x5f9e45*/
  {
    v20 = 570425352; /*0x5f9f79*/
    v21 = 0; /*0x5f9f7c*/
    a2 = 570425352; /*0x5f9f80*/
    _CxxThrowException(a1: &a2, a2: &_TI3_AVCTerminateException__); /*0x5f9f8c*/
  }
  if ( v9 < 14 ) /*0x5f9e4e*/
  {
    v11 = 684 * v9; /*0x5f9e52*/
    do /*0x5f9e8e*/
    {
      v12 = *(this + 101); /*0x5f9e58*/
      v13 = v11 + 684; /*0x5f9e5e*/
      v14 = v11 + 684 + v12; /*0x5f9e64*/
      v20 = v11 + v12; /*0x5f9e6a*/
      sub_5F9FCA(a1: v14); /*0x5f9e6d*/
      AvatarLook::operator=(a1: v14 + 231); /*0x5f9e82*/
      v11 = v13; /*0x5f9e87*/
    }
    while ( v13 < 9576 ); /*0x5f9e8e*/
    v9 = a2; /*0x5f9e90*/
  }
  *(*(this + 101) + 9576) = 0; /*0x5f9e99*/
  memcpy_0(a1: (16 * v9 + *(this + 102)), a2: (*(this + 102) + 16 * (v9 + 1)), a3: 224 - 16 * v9); /*0x5f9ec1*/
  memset(a1: (*(this + 102) + 224), a2: 0, a3: 0x10u); /*0x5f9ed3*/
  if ( dword_BEDA4C != 0 ) /*0x5f9ee5*/
  {
    CUIAvatar::Refresh(this: dword_BEDA4C); /*0x5f9ee7*/
    if ( *(684 * v9 + *(this + 101)) == 0 ) /*0x5f9efd*/
    {
      if ( v9 <= 0 ) /*0x5f9f01*/
      {
        v19 = -1; /*0x5f9f07*/
        goto LABEL_38; /*0x5f9f07*/
      }
      --v9; /*0x5f9f03*/
    }
    v19 = v9; /*0x5f9f04*/
LABEL_38:
    CUIAvatar::SelectCharacter(this: dword_BEDA4C, a2: v19); /*0x5f9f09*/
  }
  if ( *(this + 100) == -1 ) /*0x5f9f1b*/
  {
    sub_428712(a1: 0); /*0x5f9f24*/
    sub_428712(a1: 0); /*0x5f9f30*/
  }
  if ( dword_BEDA50 != 0 ) /*0x5f9f3d*/
  {
    CWnd::Destroy(a1: dword_BEDA50); /*0x5f9f3f*/
    if ( dword_BEDA50 != 0 ) /*0x5f9f4b*/
      (**(dword_BEDA50 + 8))(a1: dword_BEDA50 + 8, a2: 1); /*0x5f9f54*/
  }
  if ( TSingleton<CWndMan>::ms_pInstance != 0 ) /*0x5f9f5e*/
    CWndMan::SetFocus(a1: *(this + 98)); /*0x5f9f6a*/
}
```

## CLogin::OnCreateNewCharacterResult @ 0x5fa26c

```c
void __thiscall CLogin::OnCreateNewCharacterResult(CLogin *this, struct CInPacket *a2)
{
  char v3; // al
  int v4; // ecx
  int v5; // edx
  _DWORD *Instance; // eax
  _DWORD *v7; // eax
  int v8; // ecx
  int v9; // ebx
  int v10; // edi
  int v11; // eax
  _BYTE *v12; // eax
  const char **v13; // edi
  const char *v14; // ebx
  const char *v15; // eax
  _DWORD *v16; // eax
  int v17; // edx
  _DWORD *v18; // eax
  int v19; // edx
  int v20; // [esp-14h] [ebp-30h] BYREF
  wchar_t *v21; // [esp-10h] [ebp-2Ch]
  int v22; // [esp-Ch] [ebp-28h]
  int v23; // [esp-8h] [ebp-24h]
  int v24; // [esp-4h] [ebp-20h]
  int v25; // [esp+Ch] [ebp-10h]
  int v26; // [esp+18h] [ebp-4h]

  *(this + 92) = 0; /*0x5fa281*/
  v3 = CInPacket::Decode1(this: a2); /*0x5fa287*/
  v5 = v3; /*0x5fa28c*/
  switch ( v3 ) /*0x5fa292*/
  {
    case 10: /*0x5fa292*/
      v24 = 0; /*0x5fa294*/
      v23 = 19; /*0x5fa295*/
      goto LABEL_32; /*0x5fa297*/
    case 26: /*0x5fa292*/
      v24 = 0; /*0x5fa2a1*/
      v23 = 0; /*0x5fa2a2*/
      v22 = 0; /*0x5fa2a3*/
      v21 = 0; /*0x5fa2a4*/
      v20 = v4; /*0x5fa2a5*/
      Instance = StringPool::GetInstance(); /*0x5fa2b1*/
      StringPool::GetString( /*0x5fa2b8*/
        this: Instance,
        result: &v20,
        nIdx: SP_4016_YOU_CANNOT_CREATE_A_NEW_CHARACTER__R_NUNDER_THE_ACCOUNT_THAT__R_NHAS_REQUESTED_F);
      CUtilDlg::Notice(sMsg: v20, sSndName: v21, ppDialog: v22, bAutoSeparated: v23, bIsTightLine: v24); /*0x5fa2bd*/
      return; /*0x5fa2c5*/
    case 30: /*0x5fa292*/
      CLoginUtilDlg::Error(a1: 10, a2: 0); /*0x5fa2d2*/
      return; /*0x5fa2d9*/
    default:
      break; /*0x5fa2de*/
  }
  v25 = -1; /*0x5fa2de*/
  v7 = *(this + 101); /*0x5fa2e2*/
  v8 = 0; /*0x5fa2e8*/
  while ( *v7 != 0 ) /*0x5fa2ec*/
  {
    ++v8; /*0x5fa2ee*/
    v7 += 171; /*0x5fa2ef*/
    if ( v8 >= 15 ) /*0x5fa2f7*/
      goto LABEL_12; /*0x5fa2f7*/
  }
  v25 = v8; /*0x5fa2fb*/
LABEL_12:
  ++*(*dword_BE7918 + 8344); /*0x5fa2fe*/
  if ( v5 != 0 || v25 < 0 ) /*0x5fa31e*/
  {
    v24 = 0; /*0x5fa447*/
    v23 = 18; /*0x5fa448*/
LABEL_32:
    CLoginUtilDlg::Error(a1: v23, a2: v24); /*0x5fa44a*/
    if ( TSingleton<CWndMan>::ms_pInstance != 0 ) /*0x5fa459*/
      CWndMan::SetFocus(a1: *(this + 98)); /*0x5fa461*/
    return; /*0x5fa461*/
  }
  v9 = 684 * v25; /*0x5fa324*/
  v10 = *(this + 101) + 684 * v25; /*0x5fa32c*/
  GW_CharacterStat::Decode(this: v10, a2, a3: 0); /*0x5fa339*/
  AvatarLook::Decode(this: (v10 + 231), a2); /*0x5fa347*/
  v11 = v25; /*0x5fa352*/
  *(*(this + 103) + 4 * v25) = 0; /*0x5fa355*/
  memset(a1: (16 * v11 + *(this + 102)), a2: 0, a3: 0x10u); /*0x5fa369*/
  v12 = *(this + 127); /*0x5fa36e*/
  v13 = (this + 508); /*0x5fa374*/
  if ( v12 != 0 && *v12 != 0 ) /*0x5fa384*/
  {
    v14 = (v9 + *(this + 101) + 4); /*0x5fa392*/
    if ( v14 == 0 ) /*0x5fa398*/
      v14 = dword_BF6B44; /*0x5fa39a*/
    v15 = *v13; /*0x5fa39c*/
    if ( *v13 == 0 ) /*0x5fa3a0*/
      v15 = dword_BF6B44; /*0x5fa3a2*/
    if ( _strcmpi(a1: v15, a2: v14) == 0 ) /*0x5fa3af*/
      sub_414663(a1: this + 508); /*0x5fa3b3*/
  }
  if ( dword_BEDA4C == 0 ) /*0x5fa3c4*/
  {
    v16 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 308); /*0x5fa3cd*/
    v26 = 0; /*0x5fa3d5*/
    if ( v16 != 0 ) /*0x5fa3db*/
      sub_603FF0(a1: v16, a2: v17, a3: this, a4: *(this + 101)); /*0x5fa3e6*/
    v26 = -1; /*0x5fa3eb*/
  }
  CUIAvatar::Refresh(this: dword_BEDA4C); /*0x5fa3f5*/
  CUIAvatar::SelectCharacter(this: dword_BEDA4C, a2: v25); /*0x5fa403*/
  if ( *dword_BE7918 != 0 ) /*0x5fa412*/
    *(*dword_BE7918 + 8332) = 1; /*0x5fa414*/
  v18 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 144); /*0x5fa421*/
  v26 = 1; /*0x5fa42b*/
  if ( v18 != 0 ) /*0x5fa42e*/
    CUICharSelect::CUICharSelect(a1: v18, a2: v19, a3: this); /*0x5fa433*/
  v26 = -1; /*0x5fa438*/
  CLogin::ChangeStep(this, a2: 2); /*0x5fa440*/
}
```

## CLogin::OnViewAllCharResult @ 0x5facca

```c
void __thiscall CLogin::OnViewAllCharResult(CLogin *this, struct CInPacket *a2)
{
  struct CInPacket *v3; // ebx
  int v4; // eax
  struct CInPacket *v5; // ecx
  bool v6; // zf
  int v7; // ecx
  int v8; // ecx
  int v9; // ecx
  _DWORD *Instance; // eax
  char *v11; // ebx
  int v12; // ecx
  _DWORD *v13; // eax
  char *v14; // eax
  int v15; // edx
  int v16; // ebx
  int v17; // edi
  int v18; // ebx
  struct CInPacket *v19; // eax
  int v20; // ecx
  StringPoolStrings v21; // [esp-18h] [ebp-40h]
  int v22; // [esp-14h] [ebp-3Ch] BYREF
  wchar_t *v23; // [esp-10h] [ebp-38h]
  int v24; // [esp-Ch] [ebp-34h]
  int v25; // [esp-8h] [ebp-30h]
  size_t v26; // [esp-4h] [ebp-2Ch]
  char *v27; // [esp+Ch] [ebp-1Ch]
  char *v28; // [esp+10h] [ebp-18h]
  int i; // [esp+14h] [ebp-14h]
  char v30; // [esp+19h] [ebp-Fh] BYREF
  char v31; // [esp+1Ah] [ebp-Eh] BYREF
  char v32; // [esp+1Bh] [ebp-Dh] BYREF
  int v33; // [esp+24h] [ebp-4h]

  if ( *(this + 71) != 0 ) /*0x5face4*/
  {
    v3 = a2; /*0x5facea*/
    switch ( CInPacket::Decode1(this: a2) ) /*0x5fad00*/
    {
      case 0: /*0x5fad00*/
        v28 = CInPacket::Decode1(this: v3); /*0x5faf1e*/
        v16 = CInPacket::Decode1(this: v3); /*0x5faf2e*/
        v27 = this + 332; /*0x5faf30*/
        ZSynchronizedHelper<ZFatalSection>::ZSynchronizedHelper<ZFatalSection>(a1: this + 332); /*0x5faf33*/
        v33 = 4; /*0x5faf3a*/
        if ( v16 > 0 ) /*0x5faf41*/
        {
          for ( i = v16; i != 0; --i ) /*0x5faf47*/
          {
            v17 = *(this + 76); /*0x5faf4a*/
            v18 = *(this + 78) + 684 * v17; /*0x5faf58*/
            v26 = 0; /*0x5faf5e*/
            v25 = a2; /*0x5faf60*/
            *(this + 76) = v17 + 1; /*0x5faf68*/
            GW_CharacterStat::Decode(this: v18, a2: v25, a3: v26); /*0x5faf6e*/
            AvatarLook::Decode(this: (v18 + 231), a2); /*0x5faf7c*/
            *(*(this + 82) + 4 * v17) = v28; /*0x5faf8a*/
            v26 = 16; /*0x5faf9a*/
            if ( CInPacket::Decode1(this: a2) != 0 ) /*0x5faf9c*/
              CInPacket::DecodeBuffer(a1: (*(this + 79) + 16 * v17), a2: v26); /*0x5fafab*/
            else
              memset(a1: (*(this + 79) + 16 * v17), a2: 0, a3: v26); /*0x5fafbe*/
          }
        }
        v33 = -1; /*0x5fafd1*/
        v6 = (*(this + 84))-- == 1; /*0x5fafd5*/
        v11 = this + 332; /*0x5fafdb*/
        if ( v6 ) /*0x5fafe1*/
          *v11 = 0; /*0x5fafe3*/
        a2 = (this + 340); /*0x5fafeb*/
        ZSynchronizedHelper<ZFatalSection>::ZSynchronizedHelper<ZFatalSection>(a1: this + 340); /*0x5fafee*/
        --*(this + 74); /*0x5faff3*/
        v19 = a2; /*0x5fb005*/
        if ( *(this + 74) <= 0 ) /*0x5fb008*/
        {
          v6 = (*(a2 + 1))-- == 1; /*0x5fb01a*/
          if ( v6 ) /*0x5fb01d*/
            *v19 = 0; /*0x5fb01f*/
          CLogin::MakeVACDlg(a1: this); /*0x5fb023*/
          CLogin::ChangeStep(this, a2: 5); /*0x5fb02c*/
          if ( dword_BEDA38 == 0 ) /*0x5fb037*/
          {
            a2 = (this + 332); /*0x5fb03b*/
            ZSynchronizedHelper<ZFatalSection>::ZSynchronizedHelper<ZFatalSection>(a1: this + 332); /*0x5fb03e*/
            v33 = 6; /*0x5fb04d*/
            v14 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 15352); /*0x5fb054*/
            v27 = v14; /*0x5fb059*/
            goto LABEL_38; /*0x5fb059*/
          }
          goto LABEL_44; /*0x5fb037*/
        }
        v6 = (*(a2 + 1))-- == 1; /*0x5fb00a*/
        if ( v6 ) /*0x5fb00d*/
          *v19 = 0; /*0x5fb013*/
        return; /*0x5fb015*/
      case 1: /*0x5fad00*/
        v28 = this + 340; /*0x5fad0f*/
        ZSynchronizedHelper<ZFatalSection>::ZSynchronizedHelper<ZFatalSection>(a1: this + 340); /*0x5fad12*/
        v4 = CInPacket::Decode4(this: a2); /*0x5fad1d*/
        v5 = a2; /*0x5fad22*/
        *(this + 74) = v4; /*0x5fad25*/
        v33 = -1; /*0x5fad30*/
        *(this + 75) = CInPacket::Decode4(this: v5); /*0x5fad34*/
        v6 = (*(this + 86))-- == 1; /*0x5fad3a*/
        if ( v6 ) /*0x5fad3d*/
          *(this + 85) = 0; /*0x5fad3f*/
        if ( *(this + 74) == 0 || *(this + 75) == 0 ) /*0x5fad4f*/
          goto LABEL_18; /*0x5fad4f*/
        sub_5FDD2D(a1: 60, a2: &v32); /*0x5fad5d*/
        sub_5FDDA6(a1: 60, a2: &v31); /*0x5fad6e*/
        sub_5FDBEE(a1: 60, a2: &v30); /*0x5fad7f*/
        return; /*0x5fad84*/
      case 2: /*0x5fad00*/
        CLogin::RemoveNoticeConnecting(a1: this); /*0x5fada8*/
        CLogin::ResetVAC(a1: this); /*0x5fadaf*/
        v26 = 0; /*0x5fadb4*/
        v25 = 0; /*0x5fadb5*/
        v24 = 0; /*0x5fadb6*/
        v23 = 0; /*0x5fadb7*/
        v22 = v7; /*0x5fadb8*/
        a2 = &v22; /*0x5fadbb*/
        v21 = SP_3989_YOU_ARE_ALREADY_CONNECTED_TO_SERVER; /*0x5fadbe*/
        goto LABEL_13; /*0x5fadc3*/
      case 3: /*0x5fad00*/
      case 6: /*0x5fad00*/
      case 7: /*0x5fad00*/
        CLogin::RemoveNoticeConnecting(a1: this); /*0x5fadc7*/
        CLogin::ResetVAC(a1: this); /*0x5fadce*/
        if ( CInPacket::Decode1(this: v3) != 0 ) /*0x5faddc*/
        {
          CInPacket::DecodeStr(a1: &a2); /*0x5fade4*/
          v26 = 0; /*0x5fadec*/
          v25 = 1; /*0x5faded*/
          v24 = 0; /*0x5fadee*/
          v23 = 0; /*0x5fadef*/
          v22 = v9; /*0x5fadf0*/
          v33 = 1; /*0x5fadf1*/
          v28 = &v22; /*0x5fadf9*/
          sub_428211(a1: &a2); /*0x5fadfd*/
          CUtilDlg::Notice(sMsg: v22, sSndName: v23, ppDialog: v24, bAutoSeparated: v25, bIsTightLine: v26); /*0x5fae02*/
          v33 = -1; /*0x5fae07*/
          ZXString<char>::_Release(a1: &a2); /*0x5fae11*/
        }
        else
        {
          v26 = 0; /*0x5fae1b*/
          v25 = 0; /*0x5fae1c*/
          v24 = 0; /*0x5fae1d*/
          v23 = 0; /*0x5fae1e*/
          v22 = v8; /*0x5fae1f*/
          a2 = &v22; /*0x5fae22*/
          v21 = SP_3990_UNKNOWN_ERROR__VIEW_ALL_CHARACTERS; /*0x5fae25*/
LABEL_13:
          Instance = StringPool::GetInstance(); /*0x5fae2a*/
          StringPool::GetString(this: Instance, result: &v22, nIdx: v21); /*0x5fae32*/
LABEL_43:
          CUtilDlg::Notice(sMsg: v22, sSndName: v23, ppDialog: v24, bAutoSeparated: v25, bIsTightLine: v26); /*0x5fb09f*/
        }
LABEL_44:
        CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5fb0a7*/
        break; /*0x5fb0ab*/
      case 4: /*0x5fad00*/
      case 5: /*0x5fad00*/
        v11 = this + 340; /*0x5fae3c*/
        ZSynchronizedHelper<ZFatalSection>::ZSynchronizedHelper<ZFatalSection>(a1: this + 340); /*0x5fae44*/
        if ( --*(this + 74) > 0 ) /*0x5fae5b*/
          goto LABEL_40; /*0x5fae5b*/
        v6 = (*(this + 86))-- == 1; /*0x5fae61*/
        if ( v6 ) /*0x5fae64*/
          *v11 = 0; /*0x5fae66*/
        if ( *(this + 76) != 0 ) /*0x5fae6e*/
        {
          CLogin::ChangeStep(this, a2: 5); /*0x5faeb4*/
          if ( dword_BEDA38 != 0 ) /*0x5faebf*/
          {
            if ( *(this + 70) != 0 ) /*0x5faf00*/
              CLogin::RemoveNoticeConnecting(a1: this); /*0x5faf08*/
          }
          else
          {
            CLogin::MakeVACDlg(a1: this); /*0x5faec3*/
            v11 = this + 332; /*0x5faec8*/
            a2 = (this + 332); /*0x5faed0*/
            ZSynchronizedHelper<ZFatalSection>::ZSynchronizedHelper<ZFatalSection>(a1: this + 332); /*0x5faed3*/
            v33 = 3; /*0x5faee2*/
            v14 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 15352); /*0x5faee9*/
            v28 = v14; /*0x5faeee*/
LABEL_38:
            if ( v14 != 0 ) /*0x5fb062*/
              CUIAvatarVAC::CUIAvatarVAC(a1: v14, a2: v15, a3: this, a4: *(this + 78)); /*0x5fb06d*/
LABEL_40:
            v6 = (*(v11 + 1))-- == 1; /*0x5fb072*/
            if ( v6 ) /*0x5fb075*/
              *v11 = 0; /*0x5fb077*/
          }
        }
        else
        {
LABEL_18:
          CLogin::RemoveNoticeConnecting(a1: this); /*0x5fae70*/
          CLogin::ResetVAC(a1: this); /*0x5fae79*/
          v26 = 0; /*0x5fae7e*/
          v25 = 0; /*0x5fae7f*/
          v24 = 0; /*0x5fae80*/
          v23 = 0; /*0x5fae81*/
          v22 = v12; /*0x5fae82*/
          a2 = &v22; /*0x5fae85*/
          v13 = StringPool::GetInstance(); /*0x5fae8e*/
          StringPool::GetString(this: v13, result: &v22, nIdx: SP_3988_CANNOT_FIND_ANY_CHARACTERS); /*0x5fae95*/
          CUtilDlg::Notice(sMsg: v22, sSndName: v23, ppDialog: v24, bAutoSeparated: v25, bIsTightLine: v26); /*0x5fae9a*/
          CLogin::GotoWorldSelect(this, a2: 1); /*0x5faea6*/
        }
        break; /*0x5faeab*/
      default:
        CLogin::RemoveNoticeConnecting(a1: this); /*0x5fb07d*/
        CLogin::ResetVAC(a1: this); /*0x5fb084*/
        v26 = 0; /*0x5fb089*/
        v25 = 0; /*0x5fb08a*/
        v24 = 0; /*0x5fb08b*/
        v23 = 0; /*0x5fb08c*/
        v22 = v20; /*0x5fb08d*/
        a2 = &v22; /*0x5fb090*/
        ZXString<char>::ZXString<char>(this: &v22, a2: aError, a3: 0xFFFFFFFF); /*0x5fb09a*/
        goto LABEL_43; /*0x5fb09a*/
    }
  }
}
```

## CLogin::OnSelectCharacterResult @ 0x5fb541

```c
void __thiscall CLogin::OnSelectCharacterResult(CLogin *this, struct CInPacket *a2)
{
  struct CInPacket *v3; // ecx
  int v4; // esi
  _DWORD *v5; // eax
  int *v6; // ecx
  _DWORD *Instance; // eax
  CInPacket *v8; // ebx
  _DWORD *v9; // esi
  int v10; // eax
  _DWORD *v11; // eax
  char *v12; // [esp-8h] [ebp-3Ch]
  char *v13; // [esp-8h] [ebp-3Ch]
  wchar_t *v14; // [esp-8h] [ebp-3Ch]
  _BYTE v15[4]; // [esp+Ch] [ebp-28h] BYREF
  int v16; // [esp+10h] [ebp-24h]
  int v17; // [esp+1Ch] [ebp-18h]
  int v18; // [esp+20h] [ebp-14h] BYREF
  int v19; // [esp+24h] [ebp-10h] BYREF
  int v20; // [esp+30h] [ebp-4h]

  v3 = a2; /*0x5fb553*/
  *(this + 92) = 0; /*0x5fb558*/
  v4 = CInPacket::Decode1(this: v3); /*0x5fb566*/
  v17 = CInPacket::Decode1(this: a2); /*0x5fb574*/
  if ( v4 <= 11 ) /*0x5fb577*/
  {
    if ( v4 == 11 ) /*0x5fb579*/
    {
      CLoginUtilDlg::Error(a1: 14, a2: 0); /*0x5fb5ca*/
      return; /*0x5fb5ca*/
    }
    switch ( v4 ) /*0x5fb587*/
    {
      case 2: /*0x5fb587*/
      case 3: /*0x5fb587*/
        CLoginUtilDlg::Error(a1: 16, a2: 0); /*0x5fb599*/
        break; /*0x5fb599*/
      case 4: /*0x5fb587*/
        CLoginUtilDlg::Error(a1: 3, a2: 0); /*0x5fb5a9*/
        break; /*0x5fb5a9*/
      case 5: /*0x5fb587*/
        CLoginUtilDlg::Error(a1: 20, a2: 0); /*0x5fb5a1*/
        break; /*0x5fb5a1*/
      case 6: /*0x5fb587*/
      case 8: /*0x5fb587*/
      case 9: /*0x5fb587*/
        CLoginUtilDlg::Error(a1: 15, a2: 0); /*0x5fb591*/
        break; /*0x5fb591*/
      case 7: /*0x5fb587*/
        CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5fb5b2*/
        CLoginUtilDlg::Error(a1: 17, a2: 0); /*0x5fb5ba*/
        break; /*0x5fb5ba*/
      case 10: /*0x5fb587*/
        CLoginUtilDlg::Error(a1: 19, a2: 0); /*0x5fb5c2*/
        break; /*0x5fb5c2*/
      default:
        goto LABEL_27;
    }
    goto LABEL_27;
  }
  switch ( v4 ) /*0x5fb5d1*/
  {
    case 13: /*0x5fb5d1*/
      CLoginUtilDlg::Error(a1: 21, a2: 0); /*0x5fb691*/
      break; /*0x5fb691*/
    case 14: /*0x5fb5d1*/
      if ( CLoginUtilDlg::YesNo2(a1: 27) == 0 ) /*0x5fb654*/
        return; /*0x5fb654*/
      Instance = StringPool::GetInstance(); /*0x5fb663*/
      v13 = *StringPool::GetString(this: Instance, result: &v18, nIdx: SP_2871_HTTP__WWWNEXONNET); /*0x5fb672*/
      v20 = 0; /*0x5fb673*/
      open_web_site(a1: v13, lpWindowName: 0); /*0x5fb676*/
      v20 = -1; /*0x5fb67b*/
      v6 = &v18; /*0x5fb681*/
      goto LABEL_25; /*0x5fb681*/
    case 15: /*0x5fb5d1*/
      if ( CLoginUtilDlg::YesNo2(a1: 26) == 0 ) /*0x5fb614*/
        return; /*0x5fb614*/
      v5 = StringPool::GetInstance(); /*0x5fb623*/
      v12 = *StringPool::GetString(this: v5, result: &v19, nIdx: SP_2871_HTTP__WWWNEXONNET); /*0x5fb632*/
      v20 = 1; /*0x5fb633*/
      open_web_site(a1: v12, lpWindowName: 0); /*0x5fb63a*/
      v20 = -1; /*0x5fb63f*/
      v6 = &v19; /*0x5fb645*/
LABEL_25:
      ZXString<char>::_Release(a1: v6); /*0x5fb684*/
      return; /*0x5fb689*/
    case 16: /*0x5fb5d1*/
      goto LABEL_20; /*0x5fb5de*/
    case 17: /*0x5fb5d1*/
      CLoginUtilDlg::Error(a1: 27, a2: 0); /*0x5fb5f9*/
      return; /*0x5fb5f9*/
    case 21: /*0x5fb5d1*/
LABEL_20:
      CLoginUtilDlg::Error(a1: 33, a2: 0); /*0x5fb5fe*/
      return; /*0x5fb605*/
    case 25: /*0x5fb5d1*/
      CLoginUtilDlg::Error(a1: 40, a2: 0); /*0x5fb5f4*/
      return; /*0x5fb5f4*/
    default:
      break; /*0x5fb69a*/
  }
LABEL_27:
  if ( v4 != 0 ) /*0x5fb69a*/
  {
    if ( v4 == 12 ) /*0x5fb69f*/
    {
      if ( v17 > 13 ) /*0x5fb6b9*/
      {
        switch ( v17 ) /*0x5fb707*/
        {
          case 19: /*0x5fb707*/
            CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5fb740*/
            CLoginUtilDlg::Error(a1: 25, a2: 0); /*0x5fb748*/
            return; /*0x5fb748*/
          case 25: /*0x5fb707*/
            CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5fb72f*/
            CLoginUtilDlg::Error(a1: 31, a2: 0); /*0x5fb737*/
            return; /*0x5fb737*/
          case 27: /*0x5fb707*/
            CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5fb71e*/
            CLoginUtilDlg::Error(a1: 56, a2: 0); /*0x5fb726*/
            return; /*0x5fb726*/
          default:
            break; /*0x5fb707*/
        }
      }
      else
      {
        switch ( v17 ) /*0x5fb6bb*/
        {
          case 13: /*0x5fb6bb*/
            goto LABEL_48; /*0x5fb6bb*/
          case 1: /*0x5fb6bb*/
            CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5fb6f7*/
            CLoginUtilDlg::Error(a1: 28, a2: 0); /*0x5fb6ff*/
            return; /*0x5fb6ff*/
          case 2: /*0x5fb6bb*/
            CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5fb6e6*/
            CLoginUtilDlg::Error(a1: 29, a2: 0); /*0x5fb6ee*/
            return; /*0x5fb6ee*/
          case 3: /*0x5fb6bb*/
            CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5fb6d5*/
            CLoginUtilDlg::Error(a1: 30, a2: 0); /*0x5fb6dd*/
            return; /*0x5fb6dd*/
          case 11: /*0x5fb6bb*/
            goto LABEL_48; /*0x5fb6cd*/
          default:
            break; /*0x5fb6bb*/
        }
      }
      CLoginUtilDlg::Error(a1: 15, a2: 0); /*0x5fb715*/
      return; /*0x5fb715*/
    }
    if ( v4 != 23 ) /*0x5fb6a4*/
      return; /*0x5fb6a4*/
  }
LABEL_48:
  v8 = a2; /*0x5fb74d*/
  v9 = *dword_BE7918; /*0x5fb750*/
  v18 = CInPacket::Decode4(this: a2); /*0x5fb75f*/
  LOWORD(v10) = CInPacket::Decode2(this: v8); /*0x5fb762*/
  v17 = v10; /*0x5fb769*/
  v19 = CInPacket::Decode4(this: v8); /*0x5fb773*/
  LOBYTE(v8) = CInPacket::Decode1(this: v8); /*0x5fb77e*/
  a2 = CInPacket::Decode4(this: a2); /*0x5fb785*/
  sub_49481B(a1: v15, hostshort: v17, hostlong: 0); /*0x5fb791*/
  v16 = v18; /*0x5fb799*/
  v9[2088] = v19; /*0x5fb79f*/
  v9[2072] = a2; /*0x5fb7a8*/
  v20 = 2; /*0x5fb7b9*/
  v9[2071] = (v8 & 2) != 0; /*0x5fb7c0*/
  CWvsContext::IssueConnect(a1: v15); /*0x5fb7c6*/
  v11 = StringPool::GetInstance(); /*0x5fb7d4*/
  v14 = *StringPool::GetStringW(this: v11, &a2, a3: SP_1243_GAMEIN); /*0x5fb7e4*/
  LOBYTE(v20) = 3; /*0x5fb7e5*/
  play_game_sound(a1: v14, a2: 100); /*0x5fb7e9*/
  LOBYTE(v20) = 2; /*0x5fb7f3*/
  ZXString<unsigned short>::_Release(a1: &a2); /*0x5fb7f7*/
}
```

## CLogin::GetStepHeight @ 0x5fc0e4

```c
int __thiscall CLogin::GetStepHeight(CLogin *this, int a2)
{
  int v2; // eax

  if ( *(this + 90) == 4 ) /*0x5fc0eb*/
    v2 = *(this + 133); /*0x5fc0ed*/
  else
    v2 = 0; /*0x5fc0f5*/
  return -8 - 600 * (v2 + a2); /*0x5fc108*/
}
```

## CLogin::GotoTitle @ 0x5fc142

```c
void __thiscall CLogin::GotoTitle(CLogin *this, int a2, struct IUIMsgHandler *a3)
{
  int v4; // eax
  int v5; // edx
  _DWORD *v6; // ecx
  int v7; // eax
  wchar_t *v8; // [esp-4h] [ebp-18h]

  if ( *(this + 90) > *(this + 96) && *(this + 91) == 0 ) /*0x5fc16a*/
  {
    if ( a2 != 0 ) /*0x5fc173*/
      v4 = CLoginUtilDlg::YesNo(a1: 7, a2: this + 500); /*0x5fc17e*/
    else
      v4 = 1; /*0x5fc189*/
    if ( v4 != 0 ) /*0x5fc18c*/
    {
      if ( dword_BF14EC == 0 ) /*0x5fc198*/
        _com_issue_error(a1: -2147467261); /*0x5fc19f*/
      sub_5FC251(a1: dword_BF14EC); /*0x5fc1aa*/
      sub_9F6F27(); /*0x5fc1b5*/
      if ( *dword_BEDA60 != 0 || *(this + 96) != 0 ) /*0x5fc1c8*/
      {
        if ( *(this + 96) == 1 || (v7 = *(this + 99)) != 0 && *(v7 - 4) != 0 ) /*0x5fc207*/
          sub_5FDDE3(a1: this + 396); /*0x5fc20f*/
      }
      else
      {
        v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1516); /*0x5fc1d9*/
        if ( v6 != 0 ) /*0x5fc1e3*/
          CLicenseDlg::Draw(a1: v6, a2: v5, a3: this); /*0x5fc1e6*/
      }
      v8 = *(this + 96); /*0x5fc214*/
      *(this + 372) = -1; /*0x5fc21a*/
      CLogin::ChangeStep(this, a2: v8); /*0x5fc223*/
    }
    else if ( a3 != 0 && TSingleton<CWndMan>::ms_pInstance != 0 ) /*0x5fc237*/
    {
      CWndMan::SetFocus(a1: a3); /*0x5fc23c*/
    }
  }
}
```

## CLogin::GotoWorldSelect @ 0x5fc36c

```c
void __thiscall CLogin::GotoWorldSelect(CLogin *this, int a2)
{
  _BYTE v3[4]; // [esp+4h] [ebp-1Ch] BYREF
  _BYTE v4[12]; // [esp+8h] [ebp-18h] BYREF
  int v5; // [esp+1Ch] [ebp-4h]

  if ( *(this + 90) > 1 ) /*0x5fc385*/
  {
    if ( dword_BF14EC == 0 ) /*0x5fc390*/
      _com_issue_error(a1: -2147467261); /*0x5fc397*/
    sub_5FC251(a1: dword_BF14EC); /*0x5fc3a2*/
    sub_5FDDE3(a1: this + 396); /*0x5fc3ad*/
    if ( a2 == 0 ) /*0x5fc3b5*/
    {
      COutPacket::COutPacket(a1: 12); /*0x5fc3bc*/
      v5 = 0; /*0x5fc3cb*/
      CClientSocket::SendPacket(this: dword_BE7914, a2: v3); /*0x5fc3ce*/
      v5 = -1; /*0x5fc3d3*/
      ZArray<unsigned char>::RemoveAll(a1: v4); /*0x5fc3da*/
    }
    *(this + 96) = 1; /*0x5fc3e9*/
    CLogin::ChangeStep(this, a2: 1); /*0x5fc3ef*/
    *(this + 96) = 0; /*0x5fc3f4*/
  }
}
```

## CLogin::OnConfirmEULAResult @ 0x5fc838

```c
int __thiscall CLogin::OnConfirmEULAResult(CLogin *this, struct CInPacket *a2)
{
  int v2; // ecx
  int v4; // [esp-14h] [ebp-14h] BYREF
  wchar_t *v5; // [esp-10h] [ebp-10h]
  int v6; // [esp-Ch] [ebp-Ch]
  int v7; // [esp-8h] [ebp-8h]
  int v8; // [esp-4h] [ebp-4h]

  if ( dword_BEDA78 != 0 ) /*0x5fc843*/
  {
    CWnd::Destroy(a1: dword_BEDA78); /*0x5fc845*/
    if ( dword_BEDA78 != 0 ) /*0x5fc851*/
      (**(dword_BEDA78 + 8))(a1: dword_BEDA78 + 8, a2: 1); /*0x5fc85a*/
  }
  if ( CInPacket::Decode1(this: a2) != 0 ) /*0x5fc866*/
    return (*(**dword_BEDA60 + 52))(a1: *dword_BEDA60, a2: 1); /*0x5fc896*/
  v8 = 0; /*0x5fc868*/
  v7 = 1; /*0x5fc86a*/
  v6 = 0; /*0x5fc86c*/
  v5 = 0; /*0x5fc86e*/
  v4 = v2; /*0x5fc870*/
  ZXString<char>::ZXString<char>(this: &v4, a2: aTryAgain, a3: 0xFFFFFFFF); /*0x5fc87d*/
  return CUtilDlg::Notice(sMsg: v4, sSndName: v5, ppDialog: v6, bAutoSeparated: v7, bIsTightLine: v8); /*0x5fc899*/
}
```

## CLogin::OnCheckPinCodeResult @ 0x5fc89d

```c
void __thiscall CLogin::OnCheckPinCodeResult(CLogin *this, struct CInPacket *a2)
{
  unsigned __int8 v3; // al
  int v4; // eax
  int v5; // eax
  int v6; // eax
  int v7; // eax
  int v8; // eax
  char *v9; // ebx
  int v10; // esi
  _DWORD *Instance; // eax
  _DWORD *String; // eax
  char *v13; // eax
  int v14; // eax
  int *v15; // ecx
  char *v16; // ebx
  int v17; // esi
  _DWORD *v18; // eax
  _DWORD *v19; // eax
  char *v20; // eax
  int v21; // eax
  int PinCode; // esi
  _DWORD *v23; // eax
  _DWORD *v24; // eax
  char *v25; // eax
  int v26; // eax
  _DWORD v27[4]; // [esp-4h] [ebp-54h] BYREF
  int v28; // [esp+Ch] [ebp-44h] BYREF
  _BYTE v29[12]; // [esp+10h] [ebp-40h] BYREF
  int v30; // [esp+1Ch] [ebp-34h] BYREF
  char v31[12]; // [esp+20h] [ebp-30h] BYREF
  int v32; // [esp+2Ch] [ebp-24h] BYREF
  int v33; // [esp+30h] [ebp-20h] BYREF
  char v34[4]; // [esp+34h] [ebp-1Ch] BYREF
  int v35; // [esp+38h] [ebp-18h] BYREF
  char v36[4]; // [esp+3Ch] [ebp-14h] BYREF
  int v37; // [esp+40h] [ebp-10h] BYREF
  int v38; // [esp+4Ch] [ebp-4h]

  v3 = CInPacket::Decode1(this: a2); /*0x5fc8b2*/
  if ( v3 != 0 ) /*0x5fc8be*/
  {
    v4 = v3 - 1; /*0x5fc8c4*/
    if ( v4 != 0 ) /*0x5fc8c5*/
    {
      v5 = v4 - 1; /*0x5fc8cb*/
      if ( v5 != 0 ) /*0x5fc8cc*/
      {
        v6 = v5 - 1; /*0x5fc8d2*/
        if ( v6 == 0 ) /*0x5fc8d3*/
        {
          CLoginUtilDlg::Error(a1: 15, a2: 0); /*0x5fc9e8*/
          return; /*0x5fc9e8*/
        }
        v7 = v6 - 1; /*0x5fc8d9*/
        if ( v7 != 0 ) /*0x5fc8da*/
        {
          if ( v7 == 3 ) /*0x5fc8df*/
          {
            CLogin::GotoTitle(this, a2: 0, a3: 0); /*0x5fc8e9*/
            CUITitle::EnableLoginCtrl(this: *dword_BEDA60, a2: 1); /*0x5fc8f6*/
            CLoginUtilDlg::Error(a1: 17, a2: 0); /*0x5fc8fe*/
          }
          return; /*0x5fc8fe*/
        }
        v8 = 4; /*0x5fc90c*/
        a2 = 0; /*0x5fc90d*/
        v38 = 7; /*0x5fc910*/
        v9 = this + 500; /*0x5fc917*/
        do /*0x5fc933*/
        {
          v10 = CPinCodeDlg::EnterPinCode(a1: v9, a2: v8, a3: v36); /*0x5fc92b*/
          v8 = 5; /*0x5fc932*/
        }
        while ( v10 == -3 ); /*0x5fc933*/
        COutPacket::COutPacket(a1: 9); /*0x5fc93a*/
        LOBYTE(v38) = 8; /*0x5fc943*/
        COutPacket::Encode1(this: &v30, a2: v10); /*0x5fc947*/
        if ( v10 != 0 ) /*0x5fc94e*/
        {
          COutPacket::Encode1(this: &v30, a2: 0); /*0x5fc963*/
          Instance = StringPool::GetInstance(); /*0x5fc971*/
          String = StringPool::GetString(this: Instance, result: &v35, nIdx: SP_5514_04D); /*0x5fc978*/
          LOBYTE(v27[0]) = v36[0]; /*0x5fc97d*/
          LOBYTE(v38) = 9; /*0x5fc982*/
          v13 = sub_428226(a1: String); /*0x5fc986*/
          v14 = ZXString<char>::Format(a1: &a2, a2: v13, a3: v27[0]); /*0x5fc990*/
          *v34 = v27; /*0x5fc99a*/
          v27[0] = 0; /*0x5fc99e*/
          ZXString<char>::operator=(a1: v14); /*0x5fc9a0*/
          COutPacket::EncodeStr(a1: v27[0]); /*0x5fc9a8*/
          LOBYTE(v38) = 8; /*0x5fc9b0*/
          ZXString<char>::_Release(a1: &v35); /*0x5fc9b4*/
        }
        else
        {
          CUITitle::EnableLoginCtrl(this: *dword_BEDA60, a2: 1); /*0x5fc958*/
        }
        CClientSocket::SendPacket(this: dword_BE7914, a2: &v30); /*0x5fc9c3*/
        LOBYTE(v38) = 7; /*0x5fc9cb*/
        ZArray<unsigned char>::RemoveAll(a1: v31); /*0x5fc9cf*/
        v38 = -1; /*0x5fc9d4*/
        v15 = &a2; /*0x5fc9d8*/
      }
      else
      {
        v37 = 0; /*0x5fc9ed*/
        v38 = 4; /*0x5fc9f0*/
        v16 = this + 500; /*0x5fc9f7*/
        do /*0x5fca11*/
          v17 = CPinCodeDlg::EnterPinCode(a1: v16, a2: 5, a3: v34); /*0x5fca09*/
        while ( v17 == -3 ); /*0x5fca11*/
        COutPacket::COutPacket(a1: 9); /*0x5fca18*/
        LOBYTE(v38) = 5; /*0x5fca21*/
        COutPacket::Encode1(this: &v28, a2: v17); /*0x5fca25*/
        if ( v17 != 0 ) /*0x5fca2c*/
        {
          COutPacket::Encode1(this: &v28, a2: 0); /*0x5fca41*/
          v18 = StringPool::GetInstance(); /*0x5fca4f*/
          v19 = StringPool::GetString(this: v18, result: &v33, nIdx: SP_5514_04D); /*0x5fca56*/
          LOBYTE(v27[0]) = v34[0]; /*0x5fca5b*/
          LOBYTE(v38) = 6; /*0x5fca60*/
          v20 = sub_428226(a1: v19); /*0x5fca64*/
          v21 = ZXString<char>::Format(a1: &v37, a2: v20, a3: v27[0]); /*0x5fca6e*/
          a2 = v27; /*0x5fca78*/
          v27[0] = 0; /*0x5fca7c*/
          ZXString<char>::operator=(a1: v21); /*0x5fca7e*/
          COutPacket::EncodeStr(a1: v27[0]); /*0x5fca86*/
          LOBYTE(v38) = 5; /*0x5fca8e*/
          ZXString<char>::_Release(a1: &v33); /*0x5fca92*/
        }
        else
        {
          CUITitle::EnableLoginCtrl(this: *dword_BEDA60, a2: 1); /*0x5fca36*/
        }
        CClientSocket::SendPacket(this: dword_BE7914, a2: &v28); /*0x5fcaa1*/
        LOBYTE(v38) = 4; /*0x5fcaa9*/
        ZArray<unsigned char>::RemoveAll(a1: v29); /*0x5fcaad*/
        v38 = -1; /*0x5fcab2*/
        v15 = &v37; /*0x5fcab6*/
      }
    }
    else
    {
      PinCode = CPinCodeDlg::CreatePinCode(a1: this + 500); /*0x5fcacb*/
      v37 = 0; /*0x5fcacd*/
      v38 = 2; /*0x5fcad8*/
      COutPacket::COutPacket(a1: 10); /*0x5fcadb*/
      if ( PinCode >= 0 ) /*0x5fcae6*/
      {
        COutPacket::Encode1(this: &v28, a2: 1); /*0x5fcb03*/
        v23 = StringPool::GetInstance(); /*0x5fcb11*/
        v24 = StringPool::GetString(this: v23, result: &v32, nIdx: SP_5514_04D); /*0x5fcb18*/
        LOBYTE(v38) = 3; /*0x5fcb20*/
        v25 = sub_428226(a1: v24); /*0x5fcb24*/
        v26 = ZXString<char>::Format(a1: &v37, a2: v25, a3: PinCode); /*0x5fcb2e*/
        a2 = v27; /*0x5fcb38*/
        v27[0] = 0; /*0x5fcb3c*/
        ZXString<char>::operator=(a1: v26); /*0x5fcb3e*/
        COutPacket::EncodeStr(a1: v27[0]); /*0x5fcb46*/
        LOBYTE(v38) = 2; /*0x5fcb4e*/
        ZXString<char>::_Release(a1: &v32); /*0x5fcb52*/
      }
      else
      {
        COutPacket::Encode1(this: &v28, a2: 0); /*0x5fcaec*/
        CUITitle::EnableLoginCtrl(this: *dword_BEDA60, a2: 1); /*0x5fcaf8*/
      }
      CClientSocket::SendPacket(this: dword_BE7914, a2: &v28); /*0x5fcb61*/
      LOBYTE(v38) = 1; /*0x5fcb69*/
      ZArray<unsigned char>::RemoveAll(a1: v29); /*0x5fcb6c*/
      v38 = -1; /*0x5fcb71*/
      v15 = &v37; /*0x5fcb75*/
    }
    ZXString<char>::_Release(a1: v15); /*0x5fc9db*/
    return; /*0x5fc9e0*/
  }
  if ( *(TSingleton<CWvsApp>::ms_pInstance + 36) != 1 ) /*0x5fcb86*/
  {
    COutPacket::COutPacket(a1: 11); /*0x5fcb8d*/
    v38 = 0; /*0x5fcb9c*/
    CClientSocket::SendPacket(this: dword_BE7914, a2: &v28); /*0x5fcb9f*/
    v38 = -1; /*0x5fcba4*/
    ZArray<unsigned char>::RemoveAll(a1: v29); /*0x5fcbab*/
  }
}
```

## CLogin::ViewRecommendWorld @ 0x5fd364

```c
_DWORD *__thiscall CLogin::ViewRecommendWorld(CLogin *this, int a2)
{
  _DWORD *result; // eax
  int v4; // edx
  _DWORD *v5; // ecx
  int v6; // eax

  result = &loc_A98212; /*0x5fd364*/
  if ( a2 != 0 )
  {
    if ( *(this + 131) != 0 ) /*0x5fd37f*/
    {
      result = *(this + 132); /*0x5fd385*/
      if ( result != 0 && *(result - 1) != 0 ) /*0x5fd396*/
      {
        CLogin::CloseChannelSelect(); /*0x5fd39c*/
        v5 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 184); /*0x5fd3b0*/
        if ( v5 != 0 ) /*0x5fd3ba*/
          sub_62B2CF(a1: v5, a2: v4, a3: this); /*0x5fd3bd*/
        result = sub_60E903(a1: 0); /*0x5fd3cd*/
        *(this + 77) = 1; /*0x5fd3d2*/
      }
    }
  }
  else
  {
    if ( dword_BEDA68 != 0 ) /*0x5fd3e6*/
    {
      CWnd::Destroy(a1: dword_BEDA68); /*0x5fd3e8*/
      if ( dword_BEDA68 != 0 ) /*0x5fd3f4*/
        (**(dword_BEDA68 + 8))(a1: dword_BEDA68 + 8, a2: 1); /*0x5fd3fd*/
    }
    v6 = TSingleton<CUIChannelSelect>::ms_pInstance; /*0x5fd3ff*/
    if ( TSingleton<CUIChannelSelect>::ms_pInstance != 0
      || (v6 = TSingleton<CUIWorldSelect>::ms_pInstance, TSingleton<CUIWorldSelect>::ms_pInstance != 0) )
    {
      CWndMan::SetFocus(a1: v6 != 0 ? v6 + 4 : 0);
    }
    else
    {
      CWndMan::SetFocus(a1: this != 0 ? this + 4 : 0);
    }
    result = sub_60E903(a1: 1); /*0x5fd43e*/
    *(this + 77) = 0; /*0x5fd443*/
  }
  return result; /*0x5fd449*/
}
```

## CUILoginStart::OnButtonClicked @ 0x60e499

```c
void __thiscall CUILoginStart::OnButtonClicked(CLogin **this, unsigned int a2)
{
  CLogin *v3; // ecx
  int v4; // eax
  int v5; // esi
  int v6; // eax

  if ( *(this + 63) == 0 ) /*0x60e4a5*/
  {
    switch ( a2 ) /*0x60e4b4*/
    {
      case 1000u: /*0x60e4b4*/
        CLogin::ViewRecommendWorld(this: *(this + 31), a2: 0); /*0x60e509*/
        v3 = *(this + 31); /*0x60e50e*/
        v4 = *(v3 + 90); /*0x60e511*/
        if ( v4 == 2 ) /*0x60e51a*/
        {
          CLogin::GotoWorldSelect(this: v3, a2: 0); /*0x60e51d*/
        }
        else if ( v4 == 5 ) /*0x60e527*/
        {
          if ( dword_BEDA3C != 0 ) /*0x60e531*/
          {
            CWnd::Destroy(a1: dword_BEDA3C); /*0x60e533*/
            if ( dword_BEDA3C != 0 ) /*0x60e53f*/
              (**(dword_BEDA3C + 8))(a1: dword_BEDA3C + 8, a2: 1); /*0x60e548*/
          }
          if ( dword_BEDA38 != 0 ) /*0x60e552*/
          {
            CWnd::Destroy(a1: dword_BEDA38); /*0x60e554*/
            if ( dword_BEDA38 != 0 ) /*0x60e560*/
              (**(dword_BEDA38 + 8))(a1: dword_BEDA38 + 8, a2: 1); /*0x60e569*/
          }
          CLogin::GotoWorldSelect(this: *(this + 31), a2: 1); /*0x60e570*/
        }
        else
        {
          CLogin::GotoTitle(this: v3, a2: 1, a3: 0); /*0x60e575*/
        }
        break;
      case 1001u: /*0x60e4b4*/
        CLogin::ViewRecommendWorld(this: *(this + 31), a2: 0); /*0x60e4f7*/
        dword_BF041C(a1: 0); /*0x60e4fd*/
        break;
      case 1002u: /*0x60e4b4*/
        CLogin::ViewRecommendWorld(this: *(this + 31), a2: 0); /*0x60e4f1*/
        break;
      case 1003u: /*0x60e4b4*/
        CLogin::ViewRecommendWorld(this: *(this + 31), a2: 1); /*0x60e4e6*/
        break;
      default:
        if ( a2 == 1004 && sub_60EAD6(this) != 0 ) /*0x60e4cd*/
          CUILoginStart::EnableLoginStartCtrl(this, nEnable: 1); /*0x60e4d7*/
        break;
    }
    if ( TSingleton<CWndMan>::ms_pInstance != 0 ) /*0x60e582*/
    {
      v5 = *(this + 31); /*0x60e584*/
      v6 = *(v5 + 360); /*0x60e587*/
      if ( v6 > 0 && v6 <= 2 ) /*0x60e594*/
        CWndMan::SetFocus(a1: *(v5 + 392)); /*0x60e59c*/
    }
  }
}
```

## CUILoginStart::EnableLoginStartCtrl @ 0x60eb04

```c
int __thiscall CUILoginStart::EnableLoginStartCtrl(CUILoginStart *this, char nEnable)
{
  int v3; // eax
  int result; // eax
  int v5; // esi

  v3 = *(this + 53); /*0x60eb07*/
  if ( v3 != 0 ) /*0x60eb14*/
    (*(*(v3 + 4) + 28))(a1: v3 + 4, a2: nEnable & 1); /*0x60eb21*/
  result = *(this + 55); /*0x60eb24*/
  if ( result != 0 ) /*0x60eb2c*/
    result = (*(*(result + 4) + 28))(a1: result + 4, a2: nEnable & 2); /*0x60eb39*/
  v5 = *(this + 61); /*0x60eb3c*/
  if ( v5 != 0 ) /*0x60eb44*/
    return (*(*(v5 + 4) + 28))(a1: v5 + 4, a2: nEnable & 0x10); /*0x60eb50*/
  return result; /*0x60eb53*/
}
```

## CUITitle::OnCreate @ 0x62054a

```c
void __thiscall CUITitle::OnCreate(CUITitle *this, void *pData)
{
  Concurrency::details::SchedulerProxy *v2; // edi
  bool v4; // zf
  _BYTE *v5; // eax
  char *v6; // edi
  _DWORD *Instance; // eax
  int StringW; // eax
  CCtrlEdit *v9; // eax
  _DWORD *v10; // eax
  _DWORD *v11; // eax
  int v12; // eax
  CCtrlEdit *v13; // eax
  _DWORD *v14; // eax
  _DWORD *v15; // eax
  int v16; // eax
  CCtrlEdit *v17; // eax
  _DWORD *v18; // eax
  _DWORD *v19; // eax
  int v20; // eax
  CCtrlEdit *v21; // eax
  _DWORD *v22; // eax
  _DWORD *v23; // eax
  int v24; // eax
  CCtrlEdit *v25; // eax
  _DWORD *v26; // eax
  _DWORD *v27; // eax
  int v28; // eax
  CCtrlEdit *v29; // eax
  _DWORD *v30; // eax
  _DWORD *v31; // eax
  int v32; // eax
  CCtrlEdit *v33; // eax
  _DWORD *v34; // eax
  CCtrlEdit *v35; // eax
  int v36; // eax
  int v37; // ecx
  CCtrlEdit *v38; // eax
  int v39; // eax
  int v40; // ecx
  int v41; // eax
  int v42; // eax
  int v43; // ecx
  _DWORD *v44; // eax
  int v45; // eax
  int *v46; // eax
  int v47; // eax
  int v48; // eax
  int v49; // eax
  int v50; // eax
  int v51; // eax
  int v52; // eax
  int v53; // ecx
  _DWORD *v54; // eax
  int v55; // eax
  int *v56; // eax
  int v57; // eax
  int v58; // eax
  int v59; // eax
  int v60; // eax
  int v61; // [esp-14h] [ebp-B8h] BYREF
  VARIANTARG *p_pvargDest; // [esp-10h] [ebp-B4h]
  VARIANTARG *p_pvarg; // [esp-Ch] [ebp-B0h]
  int v64; // [esp-8h] [ebp-ACh]
  int v65; // [esp-4h] [ebp-A8h]
  VARIANTARG v66; // [esp+Ch] [ebp-98h] BYREF
  char v67[4]; // [esp+1Ch] [ebp-88h] BYREF
  int v68; // [esp+20h] [ebp-84h]
  int v69; // [esp+24h] [ebp-80h]
  int v70; // [esp+28h] [ebp-7Ch]
  int v71; // [esp+2Ch] [ebp-78h]
  int v72; // [esp+38h] [ebp-6Ch]
  int v73; // [esp+40h] [ebp-64h]
  int v74; // [esp+44h] [ebp-60h]
  int v75; // [esp+4Ch] [ebp-58h]
  VARIANTARG pvargDest; // [esp+54h] [ebp-50h] BYREF
  Concurrency::details::SchedulerProxy *v77; // [esp+64h] [ebp-40h]
  VARIANTARG pvarg; // [esp+68h] [ebp-3Ch] BYREF
  int *v79; // [esp+78h] [ebp-2Ch]
  int v80; // [esp+7Ch] [ebp-28h] BYREF
  _DWORD v81[3]; // [esp+80h] [ebp-24h] BYREF
  int v82; // [esp+8Ch] [ebp-18h] BYREF
  _BYTE *v83; // [esp+90h] [ebp-14h] BYREF
  CCtrlEdit *Unknown; // [esp+94h] [ebp-10h] BYREF
  int v85; // [esp+A0h] [ebp-4h]

  v83 = 0; /*0x62055a*/
  v2 = TSingleton<CConfig>::ms_pInstance; /*0x620561*/
  v77 = TSingleton<CConfig>::ms_pInstance; /*0x620574*/
  ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF); /*0x620577*/
  v85 = 0; /*0x62057c*/
  v80 = 0; /*0x620580*/
  ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF); /*0x62058a*/
  *(this + 33) = *(v2 + 1); /*0x620594*/
  v4 = *(v2 + 1) == 0; /*0x62059a*/
  LOBYTE(v85) = 1; /*0x62059d*/
  if ( !v4 ) /*0x6205a1*/
  {
    v5 = *(v2 + 2); /*0x6205a3*/
    v6 = v2 + 8; /*0x6205a6*/
    if ( v5 != 0 && *v5 != 0 ) /*0x6205af*/
      ZXString<char>::operator=(a1: v6); /*0x6205b5*/
  }
  v82 = 0; /*0x6205ba*/
  v81[0] = 1; /*0x6205bd*/
  v81[1] = 0; /*0x6205c4*/
  v81[2] = 0; /*0x6205c7*/
  LOBYTE(v85) = 2; /*0x6205d3*/
  Instance = StringPool::GetInstance(); /*0x6205d7*/
  StringW = StringPool::GetStringW(this: Instance, a2: &Unknown, a3: SP_1345_UI_LOGINIMG_TITLE_BTLOGIN); /*0x6205de*/
  LOBYTE(v85) = 3; /*0x6205e7*/
  ZXString<unsigned short>::operator=(a1: StringW); /*0x6205eb*/
  LOBYTE(v85) = 2; /*0x6205f3*/
  ZXString<unsigned short>::_Release(a1: &Unknown); /*0x6205f7*/
  v9 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x620609*/
  Unknown = v9; /*0x62060e*/
  LOBYTE(v85) = 4; /*0x620613*/
  if ( v9 != 0 ) /*0x620617*/
    v10 = CCtrlButton::CCtrlButton(this: v9); /*0x62061b*/
  else
    v10 = 0; /*0x620622*/
  LOBYTE(v85) = 2; /*0x62062b*/
  sub_4284FF(a1: v10); /*0x62062f*/
  (*(**(this + 37) + 32))(a1: *(this + 37), a2: this, a3: 1000, a4: 213, a5: -5, a6: 0, a7: v81); /*0x62064f*/
  v11 = StringPool::GetInstance(); /*0x62065b*/
  v12 = StringPool::GetStringW(this: v11, a2: &Unknown, a3: SP_1347_UI_LOGINIMG_TITLE_BTLOGINIDSAVE); /*0x620662*/
  LOBYTE(v85) = 5; /*0x62066b*/
  ZXString<unsigned short>::operator=(a1: v12); /*0x62066f*/
  LOBYTE(v85) = 2; /*0x620677*/
  ZXString<unsigned short>::_Release(a1: &Unknown); /*0x62067b*/
  v13 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x620683*/
  Unknown = v13; /*0x620688*/
  LOBYTE(v85) = 6; /*0x62068d*/
  if ( v13 != 0 ) /*0x620691*/
    v14 = CCtrlButton::CCtrlButton(this: v13); /*0x620695*/
  else
    v14 = 0; /*0x62069c*/
  LOBYTE(v85) = 2; /*0x6206a5*/
  sub_4284FF(a1: v14); /*0x6206a9*/
  (*(**(this + 39) + 32))(a1: *(this + 39), a2: this, a3: 1001, a4: 37, a5: 74, a6: 0, a7: v81); /*0x6206c6*/
  v15 = StringPool::GetInstance(); /*0x6206d2*/
  v16 = StringPool::GetStringW(this: v15, a2: &Unknown, a3: SP_1348_UI_LOGINIMG_TITLE_BTLOGINIDLOST); /*0x6206d9*/
  LOBYTE(v85) = 7; /*0x6206e2*/
  ZXString<unsigned short>::operator=(a1: v16); /*0x6206e6*/
  LOBYTE(v85) = 2; /*0x6206ee*/
  ZXString<unsigned short>::_Release(a1: &Unknown); /*0x6206f2*/
  v17 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x6206fa*/
  Unknown = v17; /*0x6206ff*/
  LOBYTE(v85) = 8; /*0x620704*/
  if ( v17 != 0 ) /*0x620708*/
    v18 = CCtrlButton::CCtrlButton(this: v17); /*0x62070c*/
  else
    v18 = 0; /*0x620713*/
  LOBYTE(v85) = 2; /*0x62071c*/
  sub_4284FF(a1: v18); /*0x620720*/
  (*(**(this + 41) + 32))(a1: *(this + 41), a2: this, a3: 1002, a4: 144, a5: 73, a6: 0, a7: v81); /*0x620740*/
  v19 = StringPool::GetInstance(); /*0x62074c*/
  v20 = StringPool::GetStringW(this: v19, a2: &Unknown, a3: SP_1349_UI_LOGINIMG_TITLE_BTPASSWDLOST); /*0x620753*/
  LOBYTE(v85) = 9; /*0x62075c*/
  ZXString<unsigned short>::operator=(a1: v20); /*0x620760*/
  LOBYTE(v85) = 2; /*0x620768*/
  ZXString<unsigned short>::_Release(a1: &Unknown); /*0x62076c*/
  v21 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x620774*/
  Unknown = v21; /*0x620779*/
  LOBYTE(v85) = 10; /*0x62077e*/
  if ( v21 != 0 ) /*0x620782*/
    v22 = CCtrlButton::CCtrlButton(this: v21); /*0x620786*/
  else
    v22 = 0; /*0x62078d*/
  LOBYTE(v85) = 2; /*0x620796*/
  sub_4284FF(a1: v22); /*0x62079a*/
  (*(**(this + 43) + 32))(a1: *(this + 43), a2: this, a3: 1003, a4: 226, a5: 74, a6: 0, a7: v81); /*0x6207ba*/
  v23 = StringPool::GetInstance(); /*0x6207c6*/
  v24 = StringPool::GetStringW(this: v23, a2: &Unknown, a3: SP_1350_UI_LOGINIMG_TITLE_BTNEW); /*0x6207cd*/
  LOBYTE(v85) = 11; /*0x6207d6*/
  ZXString<unsigned short>::operator=(a1: v24); /*0x6207da*/
  LOBYTE(v85) = 2; /*0x6207e2*/
  ZXString<unsigned short>::_Release(a1: &Unknown); /*0x6207e6*/
  v25 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x6207ee*/
  Unknown = v25; /*0x6207f3*/
  LOBYTE(v85) = 12; /*0x6207f8*/
  if ( v25 != 0 ) /*0x6207fc*/
    v26 = CCtrlButton::CCtrlButton(this: v25); /*0x620800*/
  else
    v26 = 0; /*0x620807*/
  LOBYTE(v85) = 2; /*0x620810*/
  sub_4284FF(a1: v26); /*0x620814*/
  (*(**(this + 45) + 32))(a1: *(this + 45), a2: this, a3: 1004, a4: 14, a5: 122, a6: 0, a7: v81); /*0x620831*/
  v27 = StringPool::GetInstance(); /*0x62083d*/
  v28 = StringPool::GetStringW(this: v27, a2: &Unknown, a3: SP_1351_UI_LOGINIMG_TITLE_BTHOMEPAGE); /*0x620844*/
  LOBYTE(v85) = 13; /*0x62084d*/
  ZXString<unsigned short>::operator=(a1: v28); /*0x620851*/
  LOBYTE(v85) = 2; /*0x620859*/
  ZXString<unsigned short>::_Release(a1: &Unknown); /*0x62085d*/
  v29 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x620865*/
  Unknown = v29; /*0x62086a*/
  LOBYTE(v85) = 14; /*0x62086f*/
  if ( v29 != 0 ) /*0x620873*/
    v30 = CCtrlButton::CCtrlButton(this: v29); /*0x620877*/
  else
    v30 = 0; /*0x62087e*/
  LOBYTE(v85) = 2; /*0x620887*/
  sub_4284FF(a1: v30); /*0x62088b*/
  (*(**(this + 47) + 32))(a1: *(this + 47), a2: this, a3: 1005, a4: 114, a5: 120, a6: 0, a7: v81); /*0x6208a8*/
  v31 = StringPool::GetInstance(); /*0x6208b4*/
  v32 = StringPool::GetStringW(this: v31, a2: &Unknown, a3: SP_1352_UI_LOGINIMG_TITLE_BTQUIT); /*0x6208bb*/
  LOBYTE(v85) = 15; /*0x6208c4*/
  ZXString<unsigned short>::operator=(a1: v32); /*0x6208c8*/
  LOBYTE(v85) = 2; /*0x6208d0*/
  ZXString<unsigned short>::_Release(a1: &Unknown); /*0x6208d4*/
  v33 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x6208dc*/
  Unknown = v33; /*0x6208e1*/
  LOBYTE(v85) = 16; /*0x6208e6*/
  if ( v33 != 0 ) /*0x6208ea*/
    v34 = CCtrlButton::CCtrlButton(this: v33); /*0x6208ee*/
  else
    v34 = 0; /*0x6208f5*/
  LOBYTE(v85) = 2; /*0x6208fe*/
  sub_4284FF(a1: v34); /*0x620902*/
  (*(**(this + 49) + 32))(a1: *(this + 49), a2: this, a3: 1006, a4: 214, a5: 120, a6: 0, a7: v81); /*0x620922*/
  CCtrlEdit::CREATEPARAM::CREATEPARAM(a1: v67); /*0x62092b*/
  v72 = -1; /*0x620930*/
  v71 = 2; /*0x62093b*/
  v69 = 0; /*0x620940*/
  LOBYTE(v85) = 17; /*0x62094b*/
  v73 = 0xFFFFFF; /*0x62094f*/
  v68 = 2; /*0x620956*/
  v70 = 2; /*0x62095c*/
  v35 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 184); /*0x62095f*/
  Unknown = v35; /*0x620964*/
  LOBYTE(v85) = 18; /*0x620969*/
  if ( v35 != 0 ) /*0x62096d*/
    v36 = CCtrlEdit::CCtrlEdit(this: v35); /*0x620971*/
  else
    v36 = 0; /*0x620978*/
  LOBYTE(v85) = 17; /*0x620981*/
  sub_473956(a1: v36); /*0x620985*/
  ZXString<char>::operator=(a1: &v83); /*0x620994*/
  v37 = *(this + 51); /*0x620999*/
  v75 = 12; /*0x6209b6*/
  (*(*v37 + 8))(a1: v37, a2: this, a3: 1007, a4: 67, a5: 12, a6: 132, a7: 15, a8: v67); /*0x6209c0*/
  v38 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 184); /*0x6209c6*/
  Unknown = v38; /*0x6209cb*/
  LOBYTE(v85) = 19; /*0x6209d0*/
  if ( v38 != 0 ) /*0x6209d4*/
    v39 = CCtrlEdit::CCtrlEdit(this: v38); /*0x6209d8*/
  else
    v39 = 0; /*0x6209df*/
  LOBYTE(v85) = 17; /*0x6209e8*/
  sub_473956(a1: v39); /*0x6209ec*/
  ZXString<char>::operator=(a1: &v80); /*0x6209fb*/
  v40 = *(this + 53); /*0x620a00*/
  v74 = 1; /*0x620a1a*/
  v75 = 12; /*0x620a21*/
  (*(*v40 + 8))(a1: v40, a2: this, a3: 1008, a4: 67, a5: 40, a6: 120, a7: 15, a8: v67); /*0x620a2b*/
  VariantInit(&pvarg); /*0x620a38*/
  v41 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x620a44*/
  if ( v41 < 0 ) /*0x620a4d*/
    _com_issue_error(a1: v41); /*0x620a50*/
  LOBYTE(v85) = 20; /*0x620a59*/
  VariantInit(pvarg: &pvargDest); /*0x620a5d*/
  v42 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc); /*0x620a64*/
  v43 = v65; /*0x620a6c*/
  if ( v42 < 0 ) /*0x620a6d*/
    _com_issue_error(a1: v42); /*0x620a70*/
  v65 = 0; /*0x620a75*/
  v64 = 0; /*0x620a77*/
  p_pvarg = &pvarg; /*0x620a7c*/
  p_pvargDest = &pvargDest; /*0x620a80*/
  v61 = v43; /*0x620a81*/
  v79 = &v61; /*0x620a84*/
  LOBYTE(v85) = 21; /*0x620a8d*/
  v44 = StringPool::GetInstance(); /*0x620a91*/
  StringPool::GetBSTR(this: v44, a2: &v61, a3: SP_1353_UI_LOGINIMG_TITLE_CHECK_0); /*0x620a98*/
  LOBYTE(v85) = 21; /*0x620aa4*/
  v45 = sub_406472(); /*0x620aa8*/
  v46 = sub_402680(a1: v45); /*0x620aaf*/
  IWzResMan::GetObjectA(this: v46, pvargDest: &v66, sUOL: v61, vParam: p_pvargDest, vAux: p_pvarg); /*0x620ab6*/
  LOBYTE(v85) = 23; /*0x620abd*/
  Unknown = Ztl_variant_t::GetUnknown(a1: v64, a2: v65); /*0x620ac6*/
  v47 = sub_41E527(a1: &Unknown); /*0x620ad3*/
  if ( v47 < 0 && v47 != -2147467262 ) /*0x620ae1*/
    _com_issue_error(a1: v47); /*0x620ae4*/
  LOBYTE(v85) = 21; /*0x620af0*/
  v48 = ZComAPI::ZComVariantCopy(pvarg: &v66); /*0x620af4*/
  if ( v48 < 0 ) /*0x620afc*/
    _com_issue_error(a1: v48); /*0x620aff*/
  LOBYTE(v85) = 20; /*0x620b08*/
  v49 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x620b0c*/
  if ( v49 < 0 ) /*0x620b14*/
    _com_issue_error(a1: v49); /*0x620b17*/
  LOBYTE(v85) = 17; /*0x620b20*/
  v50 = ZComAPI::ZComVariantCopy(&pvarg); /*0x620b24*/
  if ( v50 < 0 ) /*0x620b2c*/
    _com_issue_error(a1: v50); /*0x620b2f*/
  VariantInit(pvarg: &pvargDest); /*0x620b38*/
  v51 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc); /*0x620b3f*/
  if ( v51 < 0 ) /*0x620b48*/
    _com_issue_error(a1: v51); /*0x620b4b*/
  LOBYTE(v85) = 24; /*0x620b54*/
  VariantInit(&pvarg); /*0x620b58*/
  v52 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x620b5f*/
  v53 = v65; /*0x620b69*/
  if ( v52 < 0 ) /*0x620b6a*/
    _com_issue_error(a1: v52); /*0x620b6d*/
  v65 = 0; /*0x620b72*/
  v64 = 0; /*0x620b73*/
  p_pvarg = &pvargDest; /*0x620b77*/
  p_pvargDest = &pvarg; /*0x620b7b*/
  v61 = v53; /*0x620b7c*/
  v79 = &v61; /*0x620b7f*/
  LOBYTE(v85) = 25; /*0x620b88*/
  v54 = StringPool::GetInstance(); /*0x620b8c*/
  StringPool::GetBSTR(this: v54, a2: &v61, a3: SP_1354_UI_LOGINIMG_TITLE_CHECK_1); /*0x620b93*/
  LOBYTE(v85) = 25; /*0x620b9f*/
  v55 = sub_406472(); /*0x620ba3*/
  v56 = sub_402680(a1: v55); /*0x620baa*/
  IWzResMan::GetObjectA(this: v56, pvargDest: &v66, sUOL: v61, vParam: p_pvargDest, vAux: p_pvarg); /*0x620bb1*/
  LOBYTE(v85) = 27; /*0x620bb8*/
  Unknown = Ztl_variant_t::GetUnknown(a1: v64, a2: v65); /*0x620bc1*/
  v57 = sub_41E527(a1: &Unknown); /*0x620bce*/
  if ( v57 < 0 && v57 != -2147467262 ) /*0x620bdc*/
    _com_issue_error(a1: v57); /*0x620bdf*/
  LOBYTE(v85) = 25; /*0x620beb*/
  v58 = ZComAPI::ZComVariantCopy(pvarg: &v66); /*0x620bef*/
  if ( v58 < 0 ) /*0x620bf7*/
    _com_issue_error(a1: v58); /*0x620bfa*/
  LOBYTE(v85) = 24; /*0x620c03*/
  v59 = ZComAPI::ZComVariantCopy(&pvarg); /*0x620c07*/
  if ( v59 < 0 ) /*0x620c0f*/
    _com_issue_error(a1: v59); /*0x620c12*/
  LOBYTE(v85) = 17; /*0x620c1b*/
  v60 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x620c1f*/
  if ( v60 < 0 ) /*0x620c27*/
    _com_issue_error(a1: v60); /*0x620c2a*/
  if ( *(v77 + 1) != 0 && v83 != 0 && *v83 != 0 ) /*0x620c41*/
    v65 = *(this + 53); /*0x620c43*/
  else
    v65 = *(this + 51); /*0x620c4b*/
  CWnd::SetFocusChild(this, a2: v65); /*0x620c53*/
  *(this + 32) = pData; /*0x620c61*/
  LOBYTE(v85) = 2; /*0x620c67*/
  CCtrlEdit::CREATEPARAM::~CREATEPARAM(this: v67); /*0x620c6b*/
  LOBYTE(v85) = 1; /*0x620c73*/
  ZXString<unsigned short>::_Release(a1: &v82); /*0x620c77*/
  LOBYTE(v85) = 0; /*0x620c7c*/
  ZXString<char>::_Release(a1: &v80); /*0x620c83*/
  v85 = -1; /*0x620c88*/
  ZXString<char>::_Release(a1: &v83); /*0x620c8f*/
}
```

## CUITitle::OnButtonClicked @ 0x620ca5

```c
void __thiscall CUITitle::OnButtonClicked(CUITitle *this, unsigned int a2)
{
  _DWORD *v3; // eax
  int *v4; // ecx
  _DWORD *v5; // eax
  _DWORD *v6; // eax
  _DWORD *Instance; // eax
  char *v8; // eax
  char *v9; // [esp-8h] [ebp-28h]
  char *v10; // [esp-8h] [ebp-28h]
  char *v11; // [esp-8h] [ebp-28h]
  int v12; // [esp+4h] [ebp-1Ch] BYREF
  int v13; // [esp+8h] [ebp-18h] BYREF
  int v14; // [esp+Ch] [ebp-14h] BYREF
  int v15; // [esp+10h] [ebp-10h] BYREF
  int v16; // [esp+1Ch] [ebp-4h]

  if ( a2 == 1000 ) /*0x620cbd*/
  {
    if ( CUITitle::IsRequestValid(this) == 0 ) /*0x620e07*/
      return; /*0x620e07*/
    (*(*this + 52))(a1: this, a2: 1); /*0x620e0f*/
    goto LABEL_22; /*0x620e0f*/
  }
  if ( a2 != 1001 ) /*0x620cc4*/
  {
    switch ( a2 ) /*0x620ccb*/
    {
      case 0x3EAu: /*0x620ccb*/
        if ( CUITitle::IsRequestValid(this) == 0 ) /*0x620db8*/
          return; /*0x620db8*/
        Instance = StringPool::GetInstance(); /*0x620dc0*/
        v8 = *StringPool::GetString( /*0x620dcc*/
                this: Instance,
                result: &v12,
                nIdx: SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID);
        v16 = 0; /*0x620dce*/
        open_web_site(a1: v8, lpWindowName: 0); /*0x620dd5*/
        v16 = -1; /*0x620dda*/
        v4 = &v12; /*0x620de0*/
        break;
      case 0x3EBu: /*0x620ccb*/
        if ( CUITitle::IsRequestValid(this) == 0 ) /*0x620d7b*/
          return; /*0x620d7b*/
        v6 = StringPool::GetInstance(); /*0x620d87*/
        v11 = *StringPool::GetString( /*0x620d97*/
                 this: v6,
                 result: &v13,
                 nIdx: SP_2_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTPASSWORD);
        v16 = 1; /*0x620d98*/
        open_web_site(a1: v11, lpWindowName: 0); /*0x620d9f*/
        v16 = -1; /*0x620da4*/
        v4 = &v13; /*0x620daa*/
        break;
      case 0x3ECu: /*0x620ccb*/
        if ( CUITitle::IsRequestValid(this) == 0 ) /*0x620d3e*/
          return; /*0x620d3e*/
        v5 = StringPool::GetInstance(); /*0x620d4a*/
        v10 = *StringPool::GetString( /*0x620d5a*/
                 this: v5,
                 result: &v14,
                 nIdx: SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK);
        v16 = 2; /*0x620d5b*/
        open_web_site(a1: v10, lpWindowName: 0); /*0x620d62*/
        v16 = -1; /*0x620d67*/
        v4 = &v14; /*0x620d6d*/
        break;
      case 0x3EDu: /*0x620ccb*/
        if ( CUITitle::IsRequestValid(this) == 0 ) /*0x620cfb*/
          return; /*0x620cfb*/
        v3 = StringPool::GetInstance(); /*0x620d0a*/
        v9 = *StringPool::GetString(this: v3, result: &v15, nIdx: SP_5633_HTTP__MAPLESTORYNEXONNET); /*0x620d1a*/
        v16 = 3; /*0x620d1b*/
        open_web_site(a1: v9, lpWindowName: 0); /*0x620d22*/
        v16 = -1; /*0x620d27*/
        v4 = &v15; /*0x620d2d*/
        break;
      case 0x3EEu: /*0x620ccb*/
        dword_BF041C(a1: 0); /*0x620ce7*/
        return; /*0x620ced*/
      default:
LABEL_22:
        if ( a2 == 1 ) /*0x620e16*/
        {
          (*(*this + 52))(a1: this, a2: 1); /*0x620e1c*/
        }
        else if ( a2 == 2 ) /*0x620e22*/
        {
          (*(*this + 52))(a1: this, a2: 2); /*0x620e2a*/
        }
        return; /*0x620e1c*/
    }
    ZXString<char>::_Release(a1: v4); /*0x620de3*/
    return; /*0x620de8*/
  }
  if ( CUITitle::IsRequestValid(this) != 0 ) /*0x620df3*/
    sub_62113B(a1: this); /*0x620df7*/
}
```

## CUITitle::IsRequestValid @ 0x621156

```c
BOOL __thiscall CUITitle::IsRequestValid(CUITitle *this)
{
  int v1; // eax

  v1 = *(this + 32); /*0x621156*/
  return *(v1 + 368) == 0 && *(v1 + 360) == 0; /*0x621171*/
}
```

## CUIWorldSelect::OnCreate @ 0x624831

```c
void __thiscall CUIWorldSelect::OnCreate(CUIWorldSelect *this, int *a2)
{
  int **v3; // eax
  int **v4; // ebx
  int v5; // eax
  int v6; // eax
  int v7; // eax
  bool v8; // zf
  int v9; // ecx
  int v10; // eax
  int v11; // eax
  int v12; // ecx
  int v13; // eax
  int v14; // eax
  int v15; // eax
  int v16; // eax
  int v17; // ecx
  int v18; // eax
  int v19; // eax
  int *v20; // ebx
  int v21; // eax
  CUIWorldSelect *v22; // esi
  VARIANTARG v23; // [esp+Ch] [ebp-48h] BYREF
  VARIANTARG pvargDest; // [esp+1Ch] [ebp-38h] BYREF
  VARIANTARG pvarg; // [esp+2Ch] [ebp-28h] BYREF
  _BYTE v26[4]; // [esp+3Ch] [ebp-18h] BYREF
  _BYTE v27[4]; // [esp+40h] [ebp-14h] BYREF
  CUIWorldSelect *v28; // [esp+44h] [ebp-10h]
  int v29; // [esp+50h] [ebp-4h]

  v28 = this; /*0x62484f*/
  *(this + 32) = a2; /*0x624852*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x624858*/
  v29 = 0; /*0x624863*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x624866*/
  LOBYTE(v29) = 1; /*0x624871*/
  if ( dword_BF14EC == 0 ) /*0x624875*/
    _com_issue_error(a1: -2147467261); /*0x62487c*/
  v3 = IWzGr2D::CreateLayer( /*0x6248a1*/
         this: dword_BF14EC,
         &a2,
         nLeft: 0,
         nTop: 0,
         uWidth: 3150,
         uHeight: 189,
         z: 11,
         vCanvas: &pvarg,
         dwFilter: &pvargDest);
  v4 = (this + 380); /*0x6248a8*/
  sub_428712(a1: *v3); /*0x6248b0*/
  sub_4156C5(a1: &a2); /*0x6248b8*/
  LOBYTE(v29) = 0; /*0x6248bd*/
  v5 = ZComAPI::ZComVariantCopy(&pvarg); /*0x6248c5*/
  if ( v5 < 0 ) /*0x6248cd*/
    _com_issue_error(a1: v5); /*0x6248d0*/
  v29 = -1; /*0x6248d5*/
  v6 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x6248dd*/
  if ( v6 < 0 ) /*0x6248e5*/
    _com_issue_error(a1: v6); /*0x6248e8*/
  v7 = *CWnd::GetLayer(a1: v27); /*0x6248f8*/
  v29 = 2; /*0x624900*/
  sub_410FDF(this: &pvargDest, a2: v7, a3: 1); /*0x624907*/
  v8 = *v4 == 0; /*0x62490c*/
  LOBYTE(v29) = 3; /*0x62490e*/
  if ( v8 ) /*0x624912*/
    _com_issue_error(a1: -2147467261); /*0x624919*/
  v9 = **v4; /*0x624920*/
  a2 = *v4; /*0x62492e*/
  v10 = (*(v9 + 100))(a1: a2, a2: *&pvargDest.vt, a3: pvargDest.decVal.Hi32, a4: pvargDest.lVal, a5: pvargDest.cyVal.Hi); /*0x624932*/
  if ( v10 < 0 ) /*0x624937*/
    _com_issue_errorex(a1: v10, a2, a3: dword_BD8348); /*0x624942*/
  LOBYTE(v29) = 2; /*0x62494b*/
  v11 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x62494f*/
  if ( v11 < 0 ) /*0x624957*/
    _com_issue_error(a1: v11); /*0x62495a*/
  v29 = -1; /*0x62495f*/
  sub_4156C5(a1: v27); /*0x624966*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x624974*/
  v29 = 4; /*0x62497d*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x624984*/
  v8 = *v4 == 0; /*0x624989*/
  LOBYTE(v29) = 5; /*0x62498c*/
  if ( v8 ) /*0x624990*/
    _com_issue_error(a1: -2147467261); /*0x624997*/
  v12 = **v4; /*0x6249a8*/
  a2 = *v4; /*0x6249bc*/
  v13 = (*(v12 + 144))( /*0x6249c0*/
          a1: a2,
          a2: -10,
          a3: -10,
          a4: *&pvargDest.vt,
          a5: pvargDest.decVal.Hi32,
          a6: pvargDest.lVal,
          a7: pvargDest.cyVal.Hi,
          a8: *&pvarg.vt,
          a9: pvarg.decVal.Hi32,
          a10: pvarg.lVal,
          a11: pvarg.cyVal.Hi);
  if ( v13 < 0 ) /*0x6249c8*/
    _com_issue_errorex(a1: v13, a2, a3: dword_BD8348); /*0x6249d3*/
  LOBYTE(v29) = 4; /*0x6249dc*/
  v14 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x6249e0*/
  if ( v14 < 0 ) /*0x6249e8*/
    _com_issue_error(a1: v14); /*0x6249eb*/
  v29 = -1; /*0x6249f0*/
  v15 = ZComAPI::ZComVariantCopy(&pvarg); /*0x6249f8*/
  if ( v15 < 0 ) /*0x624a00*/
    _com_issue_error(a1: v15); /*0x624a03*/
  v16 = *CWnd::GetLayer(a1: v26); /*0x624a14*/
  v29 = 6; /*0x624a1c*/
  sub_410FDF(this: &v23, a2: v16, a3: 1); /*0x624a23*/
  v8 = *v4 == 0; /*0x624a28*/
  LOBYTE(v29) = 7; /*0x624a2b*/
  if ( v8 ) /*0x624a2f*/
    _com_issue_error(a1: -2147467261); /*0x624a36*/
  v17 = **v4; /*0x624a3d*/
  a2 = *v4; /*0x624a4b*/
  v18 = (*(v17 + 252))(a1: a2, a2: *&v23.vt, a3: v23.decVal.Hi32, a4: v23.lVal, a5: v23.cyVal.Hi); /*0x624a4f*/
  if ( v18 < 0 ) /*0x624a5c*/
    _com_issue_errorex(a1: v18, a2, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x624a63*/
  LOBYTE(v29) = 6; /*0x624a6c*/
  v19 = ZComAPI::ZComVariantCopy(pvarg: &v23); /*0x624a70*/
  if ( v19 < 0 ) /*0x624a78*/
    _com_issue_error(a1: v19); /*0x624a7b*/
  v29 = -1; /*0x624a80*/
  sub_4156C5(a1: v26); /*0x624a87*/
  if ( *v4 == 0 ) /*0x624a8f*/
    _com_issue_error(a1: -2147467261); /*0x624a96*/
  v20 = *v4; /*0x624a9b*/
  v21 = (*(*v20 + 224))(a1: v20, a2: -1); /*0x624aa2*/
  if ( v21 < 0 ) /*0x624aaa*/
    _com_issue_errorex(a1: v21, a2: v20, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x624aaf*/
  v22 = v28; /*0x624ab4*/
  *(v28 + 97) = 0; /*0x624ab7*/
  *(v22 + 34) = -1; /*0x624abe*/
  ZArray<_com_ptr_t<_com_IIID<IWzGr2DLayer,&__s_GUID const _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf>>>::RemoveAll(a1: v22 + 384); /*0x624acb*/
  CUIWorldSelect::DrawWorldItems(this: v22); /*0x624ad2*/
}
```

## CUIWorldSelect::OnButtonClicked @ 0x624ae8

```c
void __thiscall CUIWorldSelect::OnButtonClicked(CUIWorldSelect *this, unsigned int a2)
{
  int v3; // esi
  CLogin *v4; // ecx
  int v5; // eax

  if ( CUIWorldSelect::IsRequestValid(this) != 0 )
  {
    v3 = a2 - 1000; /*0x624af9*/
    if ( (a2 - 1000) >= 0 && v3 < *(this + 33) )
    {
      *(this + 34) = v3; /*0x624b0e*/
      CUIWorldSelect::EnableButtons(this, a2: a2 - 1000); /*0x624b14*/
      CUIWorldSelect::SetKeyFocus(this, a2: -1); /*0x624b1d*/
      v4 = *(this + 32); /*0x624b22*/
      *(this + 99) = v3; /*0x624b29*/
      CLogin::SendCheckUserLimitPacket(this: v4, a2: v3); /*0x624b2f*/
      if ( TSingleton<CWndMan>::ms_pInstance != 0 )
      {
        v5 = dword_BEDA68; /*0x624b3e*/
        if ( dword_BEDA68 != 0
          || (v5 = TSingleton<CUIChannelSelect>::ms_pInstance, TSingleton<CUIChannelSelect>::ms_pInstance != 0) )
        {
          CWndMan::SetFocus(a1: v5 != 0 ? v5 + 4 : 0);
        }
      }
    }
  }
}
```

## CUIWorldSelect::DrawWorldItems @ 0x624e13

```c
void __thiscall CUIWorldSelect::DrawWorldItems(CUIWorldSelect *this)
{
  int v2; // eax
  int v3; // edi
  _DWORD *v4; // eax
  int v5; // eax
  int v6; // eax
  VARIANTARG *v7; // ecx
  int v8; // eax
  int v9; // eax
  int v10; // ecx
  _DWORD *Instance; // eax
  int v12; // eax
  int *v13; // eax
  int v14; // eax
  int v15; // eax
  int v16; // eax
  int v17; // eax
  char *v18; // edi
  int v19; // esi
  _DWORD *v20; // eax
  wchar_t **StringW; // eax
  VARIANTARG **v22; // eax
  _DWORD *v23; // eax
  bool v24; // zf
  int v25; // eax
  void *v26; // esp
  wchar_t *v27; // eax
  int v28; // eax
  int v29; // eax
  int v30; // eax
  VARIANTARG *v31; // ecx
  int v32; // eax
  int v33; // eax
  _DWORD *v34; // eax
  VARIANTARG **v35; // eax
  _DWORD *v36; // eax
  VARIANTARG v37[3]; // [esp-44h] [ebp-FCh] BYREF
  int v38; // [esp-14h] [ebp-CCh] BYREF
  VARIANTARG *v39; // [esp-10h] [ebp-C8h]
  VARIANTARG *p_pvargDest; // [esp-Ch] [ebp-C4h] BYREF
  wchar_t *v41; // [esp-8h] [ebp-C0h]
  StringPoolStrings v42; // [esp-4h] [ebp-BCh]
  char v43; // [esp+0h] [ebp-B8h]
  VARIANTARG v44; // [esp+Ch] [ebp-ACh] BYREF
  VARIANTARG pvarg; // [esp+1Ch] [ebp-9Ch] BYREF
  VARIANTARG v46; // [esp+2Ch] [ebp-8Ch] BYREF
  VARIANTARG v47; // [esp+3Ch] [ebp-7Ch] BYREF
  VARIANTARG v48; // [esp+4Ch] [ebp-6Ch] BYREF
  VARIANTARG pvargDest; // [esp+5Ch] [ebp-5Ch] BYREF
  _BYTE v50[4]; // [esp+6Ch] [ebp-4Ch] BYREF
  int Unknown; // [esp+70h] [ebp-48h] BYREF
  int v52; // [esp+74h] [ebp-44h] BYREF
  _DWORD v53[3]; // [esp+78h] [ebp-40h] BYREF
  int v54; // [esp+84h] [ebp-34h] BYREF
  int *v55; // [esp+88h] [ebp-30h] BYREF
  int v56; // [esp+8Ch] [ebp-2Ch] BYREF
  int v57; // [esp+90h] [ebp-28h]
  VARIANTARG **v58; // [esp+94h] [ebp-24h]
  int v59; // [esp+98h] [ebp-20h]
  char *v60; // [esp+9Ch] [ebp-1Ch] BYREF
  VARIANTARG **v61; // [esp+A0h] [ebp-18h] BYREF
  int v62; // [esp+A4h] [ebp-14h] BYREF
  int v63; // [esp+A8h] [ebp-10h]
  int v64; // [esp+B4h] [ebp-4h]

  v2 = *(*(this + 32) + 512); /*0x624e2c*/
  *(this + 97) = v2; /*0x624e38*/
  if ( v2 != 0 ) /*0x624e3e*/
  {
    v3 = 0; /*0x624e44*/
    if ( v2 > 0 ) /*0x624e48*/
    {
      v63 = 0; /*0x624e4e*/
      do /*0x624ef0*/
      {
        v4 = sub_625348(a1: &v62); /*0x624e5b*/
        v42 = *(v63 + *v4 + 4); /*0x624e65*/
        v64 = 0; /*0x624e73*/
        sub_625348(a1: &v61); /*0x624e76*/
        LOBYTE(v64) = 1; /*0x624e7e*/
        v41 = *sub_62C145(a1: v3); /*0x624e87*/
        sub_625348(a1: &v60); /*0x624e93*/
        LOBYTE(v64) = 2; /*0x624e9b*/
        v5 = sub_62C145(a1: v3); /*0x624e9f*/
        v6 = sub_428226(a1: v5 + 8); /*0x624ea9*/
        p_pvargDest = v7; /*0x624eae*/
        v58 = &p_pvargDest; /*0x624eb1*/
        _bstr_t::_bstr_t(a1: v6); /*0x624eb5*/
        sub_625429(a1: p_pvargDest, a2: v41, a3: v42); /*0x624ebc*/
        LOBYTE(v64) = 1; /*0x624ec4*/
        sub_5FDF26(a1: &v60); /*0x624ec8*/
        LOBYTE(v64) = 0; /*0x624ecd*/
        sub_5FDF26(a1: &v61); /*0x624ed4*/
        v64 = -1; /*0x624ed9*/
        sub_5FDF26(a1: &v62); /*0x624ee0*/
        v63 += 12; /*0x624ee5*/
        ++v3; /*0x624ee9*/
      }
      while ( v3 < *(this + 97) ); /*0x624ef0*/
    }
  }
  v8 = *(*(this + 32) + 396); /*0x624f01*/
  if ( v8 != 0 ) /*0x624f05*/
    v9 = *(v8 - 4); /*0x624f0b*/
  else
    v9 = 0; /*0x624f07*/
  *(this + 33) = v9; /*0x624f10*/
  v54 = 0; /*0x624f17*/
  v53[0] = 1; /*0x624f1a*/
  v64 = 3; /*0x624f26*/
  v53[1] = 1; /*0x624f2d*/
  v53[2] = 1; /*0x624f30*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x624f33*/
  LOBYTE(v64) = 4; /*0x624f3c*/
  sub_402F85(pvargDest: &v47, pvargSrc: &pvargSrc); /*0x624f40*/
  v42 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x624f45*/
  v41 = 0; /*0x624f46*/
  p_pvargDest = &pvargDest; /*0x624f4a*/
  v39 = &v47; /*0x624f4e*/
  v38 = v10; /*0x624f4f*/
  v55 = &v38; /*0x624f52*/
  LOBYTE(v64) = 5; /*0x624f5b*/
  Instance = StringPool::GetInstance(); /*0x624f5f*/
  StringPool::GetBSTR(this: Instance, a2: &v38, a3: SP_3184_UI_LOGINIMG_WORLDNOTICE); /*0x624f66*/
  LOBYTE(v64) = 5; /*0x624f72*/
  v12 = sub_406472(); /*0x624f76*/
  v13 = sub_402680(a1: v12); /*0x624f7d*/
  IWzResMan::GetObjectA(this: v13, pvargDest: &pvarg, sUOL: v38, vParam: v39, vAux: p_pvargDest); /*0x624f84*/
  LOBYTE(v64) = 7; /*0x624f8b*/
  Unknown = Ztl_variant_t::GetUnknown(a1: v41, a2: v42); /*0x624f96*/
  v56 = 0; /*0x624f99*/
  if ( Unknown != 0 ) /*0x624f9c*/
  {
    v14 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown); /*0x624fa5*/
    if ( v14 < 0 && v14 != -2147467262 ) /*0x624fb3*/
      _com_issue_error(a1: v14); /*0x624fb6*/
  }
  LOBYTE(v64) = 11; /*0x624fc2*/
  v15 = ZComAPI::ZComVariantCopy(&pvarg); /*0x624fc6*/
  if ( v15 < 0 ) /*0x624fce*/
    _com_issue_error(a1: v15); /*0x624fd1*/
  LOBYTE(v64) = 10; /*0x624fda*/
  v16 = ZComAPI::ZComVariantCopy(pvarg: &v47); /*0x624fde*/
  if ( v16 < 0 ) /*0x624fe6*/
    _com_issue_error(a1: v16); /*0x624fe9*/
  LOBYTE(v64) = 9; /*0x624ff2*/
  v17 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x624ff6*/
  if ( v17 < 0 ) /*0x624ffe*/
    _com_issue_error(a1: v17); /*0x625001*/
  v18 = this + 144; /*0x625006*/
  v57 = 0; /*0x62500c*/
  v59 = 0; /*0x62500f*/
  v63 = 0; /*0x625012*/
  v60 = this + 144; /*0x625015*/
  do /*0x625261*/
  {
    if ( v57 >= *(this + 33) ) /*0x625021*/
    {
      v42 = SP_1357_UI_LOGINIMG_WORLDSELECT_BTWORLD_E; /*0x6252bc*/
      v41 = &v55; /*0x6252c1*/
      v34 = StringPool::GetInstance(); /*0x6252c2*/
      v42 = *StringPool::GetStringW(this: v34, a2: v41, a3: v42); /*0x6252ce*/
      LOBYTE(v64) = 22; /*0x6252d4*/
      sub_416821(a1: &v54, a2: v42, a3: v43); /*0x6252d8*/
      LOBYTE(v64) = 9; /*0x6252e2*/
      ZXString<unsigned short>::_Release(a1: &v55); /*0x6252e6*/
      v35 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x6252f5*/
      v58 = v35; /*0x6252fa*/
      LOBYTE(v64) = 23; /*0x6252ff*/
      if ( v35 != 0 ) /*0x625303*/
        v36 = CCtrlButton::CCtrlButton(this: v35); /*0x625307*/
      else
        v36 = 0; /*0x62530e*/
      LOBYTE(v64) = 9; /*0x625314*/
      sub_4284FF(a1: v36); /*0x625318*/
      (*(**v18 + 32))(a1: *v18, a2: this, a3: v57 + 1000, a4: v59, a5: 0, a6: 0, a7: v53); /*0x625335*/
      (*(*(*v18 + 4) + 28))(a1: *v18 + 4, a2: 0); /*0x625340*/
    }
    else
    {
      v19 = v63 + *(*(this + 32) + 396); /*0x625033*/
      v42 = SP_1355_UI_LOGINIMG_WORLDSELECT_BTWORLD_D; /*0x625039*/
      v41 = v50; /*0x62503e*/
      v20 = StringPool::GetInstance(); /*0x62503f*/
      StringW = StringPool::GetStringW(this: v20, a2: v41, a3: v42); /*0x625046*/
      v42 = *v19; /*0x62504b*/
      v41 = *StringW; /*0x62504f*/
      LOBYTE(v64) = 12; /*0x625054*/
      sub_416821(a1: &v54, a2: v41, a3: v42); /*0x625058*/
      LOBYTE(v64) = 9; /*0x625063*/
      ZXString<unsigned short>::_Release(a1: v50); /*0x625067*/
      v22 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x625076*/
      v58 = v22; /*0x62507b*/
      LOBYTE(v64) = 13; /*0x625080*/
      if ( v22 != 0 ) /*0x625084*/
        v23 = CCtrlButton::CCtrlButton(this: v22); /*0x625088*/
      else
        v23 = 0; /*0x62508f*/
      LOBYTE(v64) = 9; /*0x625095*/
      sub_4284FF(a1: v23); /*0x625099*/
      (*(**v18 + 32))(a1: *v18, a2: this, a3: v57 + 1000, a4: v59, a5: 0, a6: 0, a7: v53); /*0x6250b8*/
      if ( *(v19 + 8) != 0 ) /*0x6250bf*/
      {
        sub_402FAB(a1: 0, a2: 3); /*0x6250cf*/
        v24 = *(this + 95) == 0; /*0x6250d4*/
        LOBYTE(v64) = 14; /*0x6250db*/
        if ( v24 ) /*0x6250df*/
          _com_issue_error(a1: -2147467261); /*0x6250e6*/
        IWzGr2DLayer::GetCanvas(a1: &v61, a2: &v46); /*0x6250fc*/
        LOBYTE(v64) = 16; /*0x625108*/
        v25 = ZComAPI::ZComVariantCopy(pvarg: &v46); /*0x62510c*/
        if ( v25 < 0 ) /*0x625114*/
          _com_issue_error(a1: v25); /*0x625117*/
        v26 = alloca(32); /*0x62511f*/
        *&v37[0].decVal.Hi32 = 0; /*0x625128*/
        v27 = _Int2StrW(a1: *(v19 + 8), a2: &v37[0].decVal.Lo64 + 2); /*0x62512e*/
        v58 = v37; /*0x625136*/
        _bstr_t::_bstr_t(a1: v27); /*0x62513a*/
        LOBYTE(v64) = 16; /*0x625149*/
        sub_40263B(a1: &v56); /*0x62514d*/
        IWzProperty::GetItem(pvargDest: &v44, a2: *&v37[0].vt); /*0x625154*/
        LOBYTE(v64) = 18; /*0x62515b*/
        v28 = Ztl_variant_t::GetUnknown(a1: v37[0].decVal.Hi32, a2: v37[0].cVal); /*0x62515f*/
        v62 = 0; /*0x625164*/
        v52 = v28; /*0x62516a*/
        if ( v28 != 0 ) /*0x62516d*/
        {
          v29 = sub_41E527(a1: &v52); /*0x625176*/
          if ( v29 < 0 && v29 != -2147467262 ) /*0x625184*/
            _com_issue_error(a1: v29); /*0x625187*/
        }
        LOBYTE(v64) = 20; /*0x625193*/
        v30 = ZComAPI::ZComVariantCopy(pvarg: &v44); /*0x625197*/
        if ( v30 < 0 ) /*0x62519f*/
          _com_issue_error(a1: v30); /*0x6251a2*/
        if ( v62 != 0 ) /*0x6251ab*/
        {
          sub_402FAB(a1: 255, a2: 3); /*0x6251b7*/
          LOBYTE(v64) = 21; /*0x6251c1*/
          if ( v61 == 0 ) /*0x6251c5*/
            _com_issue_error(a1: -2147467261); /*0x6251cc*/
          v31 = *v61; /*0x6251d7*/
          v37[0] = v48; /*0x6251e4*/
          v58 = v61; /*0x6251ee*/
          v32 = (*&v31[8].vt)( /*0x6251f2*/
                  a1: v61,
                  a2: v59 + 5,
                  a3: 0,
                  a4: v62,
                  a5: *&v48.vt,
                  a6: v48.decVal.Hi32,
                  a7: v48.lVal,
                  a8: v48.cyVal.Hi);
          if ( v32 < 0 ) /*0x6251fa*/
            _com_issue_errorex(a1: v32, a2: v58, a3: _GUID_7600dc6c_9328_4bff_9624_5b0f5c01179e); /*0x625205*/
          LOBYTE(v64) = 20; /*0x62520e*/
          v33 = ZComAPI::ZComVariantCopy(pvarg: &v48); /*0x625212*/
          if ( v33 < 0 ) /*0x62521a*/
            _com_issue_error(a1: v33); /*0x62521d*/
          v18 = v60; /*0x625222*/
        }
        LOBYTE(v64) = 16; /*0x62522a*/
        if ( v62 != 0 ) /*0x62522e*/
          (*(*v62 + 8))(a1: v62); /*0x625233*/
        LOBYTE(v64) = 9; /*0x62523b*/
        if ( v61 != 0 ) /*0x62523f*/
          ((*v61)->lVal)(a1: v61); /*0x625244*/
      }
    }
    v63 += 32; /*0x625249*/
    ++v57; /*0x62524d*/
    v59 += 27; /*0x625250*/
    v18 += 8; /*0x625254*/
    v60 = v18; /*0x62525e*/
  }
  while ( v63 < 960 ); /*0x625261*/
  if ( *dword_BE7918 != 0 && *(*dword_BE7918 + 8328) == 0 ) /*0x625276*/
    CUIWorldSelect::MakeAdvice(this); /*0x62527a*/
  CWnd::InvalidateRect(this, a2: 0); /*0x625282*/
  LOBYTE(v64) = 3; /*0x62528c*/
  if ( v56 != 0 ) /*0x625290*/
    (*(*v56 + 8))(a1: v56); /*0x625295*/
  v64 = -1; /*0x625298*/
  ZXString<unsigned short>::_Release(a1: &v54); /*0x62529f*/
}
```

## CUIWorldSelect::EnableButtons @ 0x62536c

```c
int __thiscall CUIWorldSelect::EnableButtons(CUIWorldSelect *this, int a2)
{
  int v3; // ebx
  char *v4; // edi
  int result; // eax

  v3 = 0; /*0x625370*/
  if ( *(this + 33) > 0 ) /*0x625378*/
  {
    v4 = this + 144; /*0x62537b*/
    do /*0x625397*/
    {
      (*(*(*v4 + 4) + 28))(a1: *v4 + 4, a2: 1); /*0x62538a*/
      ++v3; /*0x62538d*/
      v4 += 8; /*0x62538e*/
    }
    while ( v3 < *(this + 33) ); /*0x625397*/
  }
  result = a2; /*0x62539a*/
  if ( a2 >= 0 ) /*0x6253a0*/
    return (*(*(*(this + 2 * a2 + 36) + 4) + 28))(a1: *(this + 2 * a2 + 36) + 4, a2: 0); /*0x6253b1*/
  return result; /*0x6253b4*/
}
```

## CUIWorldSelect::IsRequestValid @ 0x62540a

```c
BOOL __thiscall CUIWorldSelect::IsRequestValid(CUIWorldSelect *this)
{
  int v1; // eax

  v1 = *(this + 32); /*0x62540a*/
  return *(v1 + 368) == 0 && *(v1 + 360) == 1; /*0x625425*/
}
```

## CUIWorldSelect::SetFocusWorld @ 0x6272d0

```c
void __thiscall CUIWorldSelect::SetFocusWorld(CUIWorldSelect *this, int nWorldID)
{
  int v2; // edx
  int v3; // eax
  _DWORD *v4; // esi

  v2 = *(this + 33); /*0x6272d0*/
  v3 = 0; /*0x6272d7*/
  if ( v2 > 0 ) /*0x6272dc*/
  {
    v4 = *(*(this + 32) + 396); /*0x6272e4*/
    do /*0x6272f8*/
    {
      if ( *v4 == nWorldID ) /*0x6272f0*/
        break; /*0x6272f0*/
      ++v3; /*0x6272f2*/
      v4 += 8; /*0x6272f3*/
    }
    while ( v3 < v2 ); /*0x6272f8*/
  }
  if ( v3 != v2 ) /*0x6272fe*/
  {
    *(this + 34) = v3; /*0x627301*/
    CUIWorldSelect::SetKeyFocus(this, a2: v3); /*0x627307*/
  }
}
```

## CUIWorldSelect::MakeAdvice @ 0x6281b9

```c
int __thiscall CUIWorldSelect::MakeAdvice(CUIWorldSelect *this)
{
  _DWORD *v2; // eax
  int *v3; // ebx
  bool v4; // zf
  int v5; // eax
  int *basic_font; // eax
  int *v7; // ecx
  _DWORD *v8; // eax
  int v9; // eax
  int v10; // eax
  int v11; // eax
  int v12; // eax
  int v13; // ebx
  int v14; // eax
  int v15; // eax
  int v16; // eax
  int result; // eax
  int *v18; // [esp-10h] [ebp-64h] BYREF
  int v19; // [esp-Ch] [ebp-60h]
  VARIANTARG *p_pvarg; // [esp-8h] [ebp-5Ch]
  VARIANTARG *p_pvargDest; // [esp-4h] [ebp-58h]
  VARIANTARG v22; // [esp+Ch] [ebp-48h] BYREF
  VARIANTARG pvarg; // [esp+1Ch] [ebp-38h] BYREF
  VARIANTARG pvargDest; // [esp+2Ch] [ebp-28h] BYREF
  int **v25; // [esp+3Ch] [ebp-18h]
  int v26; // [esp+40h] [ebp-14h] BYREF
  int v27; // [esp+44h] [ebp-10h] BYREF
  int v28; // [esp+50h] [ebp-4h]

  v2 = sub_62730F(a1: &v26, a2: 300, a3: 34, a4: 0); /*0x6281d9*/
  v3 = (this + 392); /*0x6281e0*/
  sub_428712(a1: *v2); /*0x6281e8*/
  if ( v26 != 0 ) /*0x6281f2*/
    (*(*v26 + 8))(a1: v26); /*0x6281f7*/
  sub_402FAB(a1: 0, a2: 3); /*0x628200*/
  v4 = *v3 == 0; /*0x628205*/
  v28 = 0; /*0x628207*/
  if ( v4 ) /*0x62820a*/
    _com_issue_error(a1: -2147467261); /*0x628211*/
  IWzGr2DLayer::GetCanvas(a1: &v27, a2: &v22); /*0x628220*/
  LOBYTE(v28) = 2; /*0x628229*/
  v5 = ZComAPI::ZComVariantCopy(pvarg: &v22); /*0x62822d*/
  if ( v5 < 0 ) /*0x628235*/
    _com_issue_error(a1: v5); /*0x628238*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x628246*/
  LOBYTE(v28) = 3; /*0x62824f*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x628253*/
  p_pvargDest = &pvargDest; /*0x62825b*/
  p_pvarg = &pvarg; /*0x62825f*/
  LOBYTE(v28) = 4; /*0x628266*/
  basic_font = get_basic_font(a1: &v26, fType: 37); /*0x62826a*/
  LOBYTE(v28) = 5; /*0x628273*/
  v19 = sub_4284A5(this: basic_font); /*0x62827c*/
  v18 = v7; /*0x62827d*/
  v25 = &v18; /*0x628280*/
  _bstr_t::_bstr_t(a1: aPleaseSelectTh); /*0x628288*/
  LOBYTE(v28) = 5; /*0x628294*/
  v8 = sub_414576(a1: &v27); /*0x628298*/
  IWzCanvas::DrawTextA(this: v8, nLeft: 15, nTop: 10, sText: v18, pFont: v19, vAlpha: p_pvarg, vTabOrg: p_pvargDest); /*0x62829f*/
  LOBYTE(v28) = 4; /*0x6282a9*/
  if ( v26 != 0 ) /*0x6282ad*/
    (*(*v26 + 8))(a1: v26); /*0x6282b2*/
  LOBYTE(v28) = 3; /*0x6282b9*/
  v9 = ZComAPI::ZComVariantCopy(&pvarg); /*0x6282bd*/
  if ( v9 < 0 ) /*0x6282c5*/
    _com_issue_error(a1: v9); /*0x6282c8*/
  LOBYTE(v28) = 2; /*0x6282d1*/
  v10 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x6282d5*/
  if ( v10 < 0 ) /*0x6282dd*/
    _com_issue_error(a1: v10); /*0x6282e0*/
  VariantInit(&pvarg); /*0x6282ef*/
  v11 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x6282f6*/
  if ( v11 < 0 ) /*0x6282ff*/
    _com_issue_error(a1: v11); /*0x628302*/
  LOBYTE(v28) = 7; /*0x62830b*/
  VariantInit(pvarg: &pvargDest); /*0x62830f*/
  v12 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc); /*0x628316*/
  if ( v12 < 0 ) /*0x62831f*/
    _com_issue_error(a1: v12); /*0x628322*/
  v4 = *v3 == 0; /*0x628327*/
  LOBYTE(v28) = 8; /*0x62832a*/
  if ( v4 ) /*0x62832e*/
    _com_issue_error(a1: -2147467261); /*0x628335*/
  v13 = *v3; /*0x62833f*/
  v14 = (*(*v13 + 144))( /*0x62835b*/
          a1: v13,
          a2: 25,
          a3: -45,
          a4: *&pvargDest.vt,
          a5: pvargDest.decVal.Hi32,
          a6: pvargDest.lVal,
          a7: pvargDest.cyVal.Hi,
          a8: *&pvarg.vt,
          a9: pvarg.decVal.Hi32,
          a10: pvarg.lVal,
          a11: pvarg.cyVal.Hi);
  if ( v14 < 0 ) /*0x628363*/
    _com_issue_errorex(a1: v14, a2: v13, a3: dword_BD8348); /*0x62836c*/
  LOBYTE(v28) = 7; /*0x628375*/
  v15 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x628379*/
  if ( v15 < 0 ) /*0x628381*/
    _com_issue_error(a1: v15); /*0x628384*/
  LOBYTE(v28) = 2; /*0x62838d*/
  v16 = ZComAPI::ZComVariantCopy(&pvarg); /*0x628391*/
  if ( v16 < 0 ) /*0x628399*/
    _com_issue_error(a1: v16); /*0x62839c*/
  result = v27; /*0x6283a1*/
  v28 = -1; /*0x6283a4*/
  if ( v27 != 0 ) /*0x6283aa*/
    return (*(*v27 + 8))(a1: v27); /*0x6283af*/
  return result; /*0x6283b2*/
}
```

## CUIWorldSelect::UserLimitResult @ 0x6283c1

```c
CFadeWnd *__thiscall CUIWorldSelect::UserLimitResult(CUIWorldSelect *this, int a2, int a3)
{
  CFadeWnd *result; // eax
  _DWORD *v5; // eax
  int v6; // esi
  _DWORD *Instance; // eax
  int v8; // edx
  CFadeWnd *v9; // ecx
  _DWORD *v10; // eax
  CUIWorldSelect *v11; // [esp-14h] [ebp-40h] BYREF
  wchar_t *v12; // [esp-10h] [ebp-3Ch]
  int v13; // [esp-Ch] [ebp-38h]
  int v14; // [esp-8h] [ebp-34h]
  int v15; // [esp-4h] [ebp-30h]
  _DWORD v16[3]; // [esp+8h] [ebp-24h] BYREF
  _DWORD v17[3]; // [esp+14h] [ebp-18h] BYREF
  int v18; // [esp+28h] [ebp-4h]

  if ( a2 == 0 ) /*0x6283d9*/
  {
    if ( TSingleton<CUIChannelSelect>::ms_pInstance != 0 ) /*0x6284b3*/
      return sub_6297A0(a1: *(this + 99), a2: a3, a3: 1); /*0x6284b3*/
    v16[0] = *(this + 32); /*0x6284bb*/
    v16[1] = *(this + 99); /*0x6284c4*/
    v16[2] = a3; /*0x6284d4*/
    result = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 272); /*0x6284d7*/
    v9 = result; /*0x6284dc*/
    v18 = 0; /*0x6284e3*/
    if ( result == 0 ) /*0x6284e6*/
      return result; /*0x6284e6*/
    v10 = v16; /*0x6284e8*/
    return sub_628501(a1: v9, a2: v8, a3: v10); /*0x6284e8*/
  }
  if ( a2 == 1 ) /*0x6283e0*/
  {
    v15 = 0; /*0x62842b*/
    v14 = 1; /*0x62842c*/
    v13 = 0; /*0x62842e*/
    v12 = 0; /*0x62842f*/
    v11 = this; /*0x628430*/
    Instance = StringPool::GetInstance(); /*0x62843c*/
    StringPool::GetString( /*0x628443*/
      this: Instance,
      result: &v11,
      nIdx: SP_1369_SINCE_THERE_ARE_MANY_R_NCONCURRENT_USERS_IN_THIS_WORLD_R_NYOU_MAY_ENCOUNTER_SOME);
    CUtilDlg::Notice(sMsg: v11, sSndName: v12, ppDialog: v13, bAutoSeparated: v14, bIsTightLine: v15); /*0x628448*/
    if ( TSingleton<CUIChannelSelect>::ms_pInstance != 0 ) /*0x628458*/
      return sub_6297A0(a1: *(this + 99), a2: a3, a3: 1); /*0x62846a*/
    v17[0] = *(this + 32); /*0x628475*/
    v17[1] = *(this + 99); /*0x62847e*/
    v17[2] = a3; /*0x62848e*/
    result = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 272); /*0x628491*/
    v9 = result; /*0x628496*/
    v18 = 1; /*0x62849d*/
    if ( result == 0 ) /*0x6284a4*/
      return result; /*0x6284a4*/
    v10 = v17; /*0x6284a6*/
    return sub_628501(a1: v9, a2: v8, a3: v10); /*0x6284ec*/
  }
  result = (a2 - 2); /*0x6283e2*/
  if ( a2 == 2 ) /*0x6283e3*/
  {
    v15 = 0; /*0x6283e9*/
    v14 = 1; /*0x6283ea*/
    v13 = 0; /*0x6283ec*/
    v12 = 0; /*0x6283ed*/
    v11 = this; /*0x6283ee*/
    v5 = StringPool::GetInstance(); /*0x6283fa*/
    StringPool::GetString( /*0x628401*/
      this: v5,
      result: &v11,
      nIdx: SP_1370_THE_CONCURRENT_USERS_IN_THIS_WORLD_R_NHAVE_REACHED_THE_MAX_R_NPLEASE_TRY_AGAIN_L);
    CUtilDlg::Notice(sMsg: v11, sSndName: v12, ppDialog: v13, bAutoSeparated: v14, bIsTightLine: v15); /*0x628406*/
    v6 = *(this + 2 * *(this + 99) + 36); /*0x628411*/
    return (*(*(v6 + 4) + 28))(a1: v6 + 4, a2: 1); /*0x628423*/
  }
  return result; /*0x6284f1*/
}
```

## CUICharSelect::OnButtonClicked @ 0x601667

```c
void __thiscall CUICharSelect::OnButtonClicked(CLogin **this, unsigned int a2)
{
  if ( CUICharSelect::IsRequestValid(this) != 0 )
  {
    CUICharSelect::SetKeyFocus(this, a2: -1); /*0x601677*/
    *(this + 29) = (a2 - 1000); /*0x60168b*/
    switch ( a2 ) /*0x60168e*/
    {
      case 0x3E8u: /*0x60168e*/
        CLogin::SendSelectCharPacket(this: *(this + 28)); /*0x6016ad*/
        break;
      case 0x3E9u: /*0x60168e*/
        CLogin::OnNewCharStep(this: *(this + 28)); /*0x6016a3*/
        break;
      case 0x3EAu: /*0x60168e*/
        CLogin::SendDeleteCharPacket(this: *(this + 28)); /*0x601699*/
        break;
      default:
        break; /*0x60168e*/
    }
    if ( TSingleton<CWndMan>::ms_pInstance != 0 )
      CWndMan::SetFocus(a1: this != 0 ? this + 1 : 0);
  }
}
```

## CUICharSelect::IsRequestValid @ 0x601861

```c
BOOL __thiscall CUICharSelect::IsRequestValid(CUICharSelect *this)
{
  int v1; // eax

  v1 = *(this + 28); /*0x601861*/
  return *(v1 + 368) == 0 && *(v1 + 360) == 2; /*0x601879*/
}
```

## CUINewCharNameSelect::OnButtonClicked @ 0x6175ea

```c
void __thiscall CUINewCharNameSelect::OnButtonClicked(CLogin **this, unsigned int a2)
{
  int *Text; // eax

  if ( CUINewCharNameSelect::IsRequestValid(this) ) /*0x6175f7*/
  {
    if ( a2 == 1000 ) /*0x617608*/
    {
      Text = CCtrlEdit::GetText(a1: &a2); /*0x617621*/
      CLogin::SendCheckDuplicateIDPacket(this: *(this + 27), a2: Text); /*0x61762e*/
      ZXString<char>::_Release(a1: &a2); /*0x61763a*/
    }
    else if ( a2 == 1001 ) /*0x61760b*/
    {
      CLogin::OnNewCharCanceled(this: *(this + 27)); /*0x617610*/
    }
  }
}
```

## CUINewCharNameSelect::IsRequestValid @ 0x61770e

```c
BOOL __thiscall CUINewCharNameSelect::IsRequestValid(CUINewCharNameSelect *this)
{
  int v1; // eax

  v1 = *(this + 27); /*0x61770e*/
  return *(v1 + 368) == 0 && *(v1 + 360) == 4; /*0x617726*/
}
```

## CUIRecommendWorld::OnCreate @ 0x62b451

```c
void __thiscall CUIRecommendWorld::OnCreate(CUIRecommendWorld *this, CUIRecommendWorld **pData)
{
  _DWORD *Instance; // eax
  int *basic_font; // eax
  _DWORD *v5; // edi
  signed int *v6; // ecx
  int v7; // eax
  _DWORD *v8; // eax
  int StringW; // eax
  signed int *v10; // ecx
  _DWORD *v11; // eax
  _DWORD *v12; // eax
  int v13; // eax
  signed int *v14; // ecx
  _DWORD *v15; // eax
  _DWORD *v16; // eax
  int v17; // eax
  signed int *v18; // ecx
  _DWORD *v19; // eax
  CUIRecommendWorld *v20; // [esp-Ch] [ebp-3Ch] BYREF
  signed int **p_pData; // [esp-8h] [ebp-38h]
  StringPoolStrings v22; // [esp-4h] [ebp-34h]
  _DWORD v23[3]; // [esp+Ch] [ebp-24h] BYREF
  int v24; // [esp+18h] [ebp-18h] BYREF
  _BYTE v25[4]; // [esp+1Ch] [ebp-14h] BYREF
  int v26; // [esp+20h] [ebp-10h] BYREF
  int v27; // [esp+2Ch] [ebp-4h]

  *(this + 32) = pData; /*0x62b467*/
  *(this + 33) = 0; /*0x62b46d*/
  if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 ) /*0x62b47b*/
    (*(*(TSingleton<CUIWorldSelect>::ms_pInstance + 4) + 28))(a1: TSingleton<CUIWorldSelect>::ms_pInstance + 4, a2: 0); /*0x62b483*/
  v22 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x62b486*/
  p_pData = 0; /*0x62b487*/
  v20 = this; /*0x62b488*/
  pData = &v20; /*0x62b48b*/
  Instance = StringPool::GetInstance(); /*0x62b494*/
  StringPool::GetBSTR(this: Instance, a2: &v20, a3: SP_5362_UI_LOGINIMG_WORLDSELECT_ALERT_BACKGRD); /*0x62b49b*/
  CWnd::SetBackgrnd(a1: v20, a2: p_pData, a3: v22); /*0x62b4a2*/
  basic_font = get_basic_font(a1: &pData, fType: 1); /*0x62b4ad*/
  v5 = (this + 160); /*0x62b4b6*/
  sub_4288D7(a1: *basic_font); /*0x62b4be*/
  if ( pData != 0 ) /*0x62b4c8*/
    (*(*pData + 2))(a1: pData); /*0x62b4cd*/
  if ( *v5 == 0 ) /*0x62b4d2*/
    _com_issue_error(a1: -2147467261); /*0x62b4d9*/
  (*(**v5 + 20))(a1: *v5, a2: &v26); /*0x62b4e7*/
  v22 = SP_116_MAPLESTORY_ERROR__MOB_ERROR; /*0x62b4f6*/
  *(this + 45) = 85 / v26; /*0x62b4fa*/
  v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: v22); /*0x62b505*/
  pData = v6; /*0x62b507*/
  v27 = 0; /*0x62b50c*/
  if ( v6 != 0 ) /*0x62b50f*/
    v7 = CCtrlScrollBar::CCtrlScrollBar(this: v6); /*0x62b511*/
  else
    v7 = 0; /*0x62b518*/
  v27 = -1; /*0x62b51a*/
  sub_428597(a1: v7); /*0x62b525*/
  (*(**(this + 44) + 32))(a1: *(this + 44), a2: this, a3: 1003, a4: 1, a5: 7, a6: 502, a7: 58, a8: 85, a9: 0); /*0x62b546*/
  *(*(this + 44) + 52) = *(this + 45); /*0x62b555*/
  v24 = 0; /*0x62b558*/
  v22 = SP_5363_UI_LOGINIMG_WORLDSELECT_ALERT_BTARROWL; /*0x62b564*/
  p_pData = &pData; /*0x62b569*/
  v27 = 1; /*0x62b56a*/
  memset(v23, 0, sizeof(v23)); /*0x62b571*/
  v8 = StringPool::GetInstance(); /*0x62b574*/
  StringW = StringPool::GetStringW(this: v8, a2: p_pData, a3: v22); /*0x62b57b*/
  LOBYTE(v27) = 2; /*0x62b584*/
  ZXString<unsigned short>::operator=(a1: StringW); /*0x62b588*/
  LOBYTE(v27) = 1; /*0x62b590*/
  ZXString<unsigned short>::_Release(a1: &pData); /*0x62b594*/
  v10 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x62b5a5*/
  pData = v10; /*0x62b5a7*/
  LOBYTE(v27) = 3; /*0x62b5ac*/
  if ( v10 != 0 ) /*0x62b5b0*/
    v11 = CCtrlButton::CCtrlButton(this: v10); /*0x62b5b2*/
  else
    v11 = 0; /*0x62b5b9*/
  LOBYTE(v27) = 1; /*0x62b5c2*/
  sub_4284FF(a1: v11); /*0x62b5c6*/
  (*(**(this + 35) + 32))(a1: *(this + 35), a2: this, a3: 1000, a4: 45, a5: 70, a6: 0, a7: v23); /*0x62b5e2*/
  v22 = SP_5364_UI_LOGINIMG_WORLDSELECT_ALERT_BTARROWR; /*0x62b5e8*/
  p_pData = &pData; /*0x62b5ed*/
  v23[0] = 0; /*0x62b5ee*/
  v12 = StringPool::GetInstance(); /*0x62b5f1*/
  v13 = StringPool::GetStringW(this: v12, a2: p_pData, a3: v22); /*0x62b5f8*/
  LOBYTE(v27) = 4; /*0x62b601*/
  ZXString<unsigned short>::operator=(a1: v13); /*0x62b605*/
  LOBYTE(v27) = 1; /*0x62b60d*/
  ZXString<unsigned short>::_Release(a1: &pData); /*0x62b611*/
  v14 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x62b622*/
  pData = v14; /*0x62b624*/
  LOBYTE(v27) = 5; /*0x62b629*/
  if ( v14 != 0 ) /*0x62b62d*/
    v15 = CCtrlButton::CCtrlButton(this: v14); /*0x62b62f*/
  else
    v15 = 0; /*0x62b636*/
  LOBYTE(v27) = 1; /*0x62b63f*/
  sub_4284FF(a1: v15); /*0x62b643*/
  (*(**(this + 37) + 32))(a1: *(this + 37), a2: this, a3: 1001, a4: 215, a5: 70, a6: 0, a7: v23); /*0x62b662*/
  v22 = SP_5365_UI_LOGINIMG_WORLDSELECT_ALERT_BTCHOICE; /*0x62b668*/
  p_pData = v25; /*0x62b66d*/
  v23[0] = 0; /*0x62b66e*/
  v16 = StringPool::GetInstance(); /*0x62b671*/
  v17 = StringPool::GetStringW(this: v16, a2: p_pData, a3: v22); /*0x62b678*/
  LOBYTE(v27) = 6; /*0x62b681*/
  ZXString<unsigned short>::operator=(a1: v17); /*0x62b685*/
  LOBYTE(v27) = 1; /*0x62b68d*/
  ZXString<unsigned short>::_Release(a1: v25); /*0x62b691*/
  v18 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x62b6a2*/
  pData = v18; /*0x62b6a4*/
  LOBYTE(v27) = 7; /*0x62b6a9*/
  if ( v18 != 0 ) /*0x62b6ad*/
    v19 = CCtrlButton::CCtrlButton(this: v18); /*0x62b6af*/
  else
    v19 = 0; /*0x62b6b6*/
  LOBYTE(v27) = 1; /*0x62b6bf*/
  sub_4284FF(a1: v19); /*0x62b6c3*/
  (*(**(this + 39) + 32))(a1: *(this + 39), a2: this, a3: 1002, a4: 113, a5: 114, a6: 0, a7: v23); /*0x62b6df*/
  CUIRecommendWorld::ResetWorldAndMessage(this, bRedraw: 1); /*0x62b6e6*/
  v27 = -1; /*0x62b6eb*/
  ZXString<unsigned short>::_Release(a1: &v24); /*0x62b6f2*/
}
```

## CUIRecommendWorld::OnButtonClicked @ 0x62b708

```c
void __thiscall CUIRecommendWorld::OnButtonClicked(CUIRecommendWorld *this, int nId)
{
  if ( CUIRecommendWorld::IsRequestValid(a1: this) != 0 ) /*0x62b712*/
  {
    switch ( nId ) /*0x62b71d*/
    {
      case 1000: /*0x62b71d*/
        CUIRecommendWorld::MoveWorld(this, nMovement: -1); /*0x62b736*/
        break;
      case 1001: /*0x62b71d*/
        CUIRecommendWorld::MoveWorld(this, nMovement: 1); /*0x62b730*/
        break;
      case 1002: /*0x62b71d*/
        CUIRecommendWorld::SelectWorld(this); /*0x62b727*/
        break;
      default:
        break; /*0x62b71d*/
    }
  }
}
```

## CUIRecommendWorld::SelectWorld @ 0x62b96b

```c
int __thiscall CUIRecommendWorld::SelectWorld(_DWORD *this)
{
  if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 ) /*0x62b976*/
    CUIWorldSelect::SetFocusWorld( /*0x62b98d*/
      this: TSingleton<CUIWorldSelect>::ms_pInstance,
      nWorldID: *(*(*(this + 32) + 528) + 8 * *(this + 33)));
  return (*(*this + 52))(a1: this, a2: 1); /*0x62b99b*/
}
```

## CUIRecommendWorld::MoveWorld @ 0x62b99d

```c
void __thiscall CUIRecommendWorld::MoveWorld(CUIRecommendWorld *this, int nMovement)
{
  int v2; // eax
  int v3; // edi

  v2 = *(*(this + 32) + 528); /*0x62b9a8*/
  if ( v2 != 0 ) /*0x62b9ae*/
    v3 = *(v2 - 4); /*0x62b9b4*/
  else
    v3 = 0; /*0x62b9b0*/
  *(this + 33) = (nMovement + v3 + *(this + 33)) % v3; /*0x62b9ce*/
  CUIRecommendWorld::ResetWorldAndMessage(this, bRedraw: 1); /*0x62b9d0*/
}
```

## CUIAvatarVAC::OnCreate @ 0x608ca6

```c
void __thiscall CUIAvatarVAC::OnCreate(CUIAvatarVAC *this, struct AvatarData *a2)
{
  _DWORD *Instance; // eax
  int v4; // eax
  int v5; // ecx
  _DWORD *v6; // eax
  int StringW; // eax
  int *v8; // eax
  _DWORD *v9; // eax
  CCtrlScrollBar *v10; // eax
  int v11; // eax
  int v12; // eax
  int v13; // eax
  int v14; // ecx
  _DWORD *v15; // eax
  int v16; // eax
  int *v17; // eax
  int v18; // eax
  int v19; // eax
  int v20; // eax
  int v21; // eax
  void *v22; // esp
  wchar_t *v23; // eax
  int v24; // eax
  int v25; // eax
  int i; // ebx
  void *v27; // esp
  wchar_t *v28; // eax
  int v29; // eax
  int v30; // eax
  _DWORD *v31; // eax
  wchar_t *v32; // eax
  int v33; // eax
  int v34; // eax
  wchar_t *v35; // eax
  StringPoolStrings v36; // ecx
  int v37; // eax
  int *v38; // eax
  int v39; // eax
  int v40; // eax
  int v41; // eax
  int v42; // eax
  _DWORD *v43; // eax
  int v44; // eax
  int v45; // eax
  VARIANTARG *v46; // ecx
  _DWORD *v47; // eax
  int v48; // eax
  _DWORD *v49; // eax
  int v50; // eax
  int v51; // eax
  VARIANTARG *v52; // ecx
  _DWORD *v53; // eax
  int v54; // eax
  int v55; // ecx
  _DWORD *v56; // eax
  int v57; // ecx
  int *v58; // eax
  int v59; // edx
  bool v60; // cc
  StringPoolStrings v61; // [esp-90h] [ebp-138h] BYREF
  VARIANTARG *p_pvargDest; // [esp-8Ch] [ebp-134h] BYREF
  int v63; // [esp-88h] [ebp-130h]
  int v64; // [esp-84h] [ebp-12Ch] BYREF
  StringPoolStrings v65; // [esp-80h] [ebp-128h]
  int v66; // [esp-7Ch] [ebp-124h]
  int v67; // [esp-78h] [ebp-120h] BYREF
  VARIANTARG *p_pvarg; // [esp-68h] [ebp-110h]
  int v69; // [esp-64h] [ebp-10Ch] BYREF
  int v70; // [esp-60h] [ebp-108h]
  int v71; // [esp-5Ch] [ebp-104h]
  wchar_t v72; // [esp-58h] [ebp-100h] BYREF
  int v73; // [esp-4Ch] [ebp-F4h] BYREF
  VARIANTARG *v74; // [esp-48h] [ebp-F0h]
  VARIANTARG *v75; // [esp-44h] [ebp-ECh]
  int v76; // [esp-40h] [ebp-E8h]
  int v77; // [esp-3Ch] [ebp-E4h]
  int v78; // [esp-Ch] [ebp-B4h] BYREF
  int *v79; // [esp-8h] [ebp-B0h]
  StringPoolStrings v80; // [esp-4h] [ebp-ACh]
  VARIANTARG v81; // [esp+Ch] [ebp-9Ch] BYREF
  VARIANTARG v82; // [esp+1Ch] [ebp-8Ch] BYREF
  VARIANTARG v83; // [esp+2Ch] [ebp-7Ch] BYREF
  VARIANTARG v84; // [esp+3Ch] [ebp-6Ch] BYREF
  VARIANTARG pvargDest; // [esp+4Ch] [ebp-5Ch] BYREF
  _DWORD v86[3]; // [esp+5Ch] [ebp-4Ch] BYREF
  int v87; // [esp+68h] [ebp-40h] BYREF
  int Unknown; // [esp+6Ch] [ebp-3Ch] BYREF
  VARIANTARG pvarg; // [esp+70h] [ebp-38h] BYREF
  int v90; // [esp+80h] [ebp-28h] BYREF
  int v91; // [esp+84h] [ebp-24h] BYREF
  int *p_p_pvargDest; // [esp+88h] [ebp-20h]
  int *v93; // [esp+8Ch] [ebp-1Ch] BYREF
  char v94[4]; // [esp+90h] [ebp-18h] BYREF
  int v95; // [esp+94h] [ebp-14h] BYREF
  int v96; // [esp+98h] [ebp-10h] BYREF
  int v97; // [esp+A4h] [ebp-4h]

  _bstr_t::_bstr_t(this: &v95, a2: dword_AF7074); /*0x608cc3*/
  v80 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x608cca*/
  v79 = 0; /*0x608ccb*/
  v97 = 0; /*0x608cd5*/
  Instance = StringPool::GetInstance(); /*0x608cd8*/
  StringPool::GetBSTR(this: Instance, a2: &v96, a3: SP_3987_UI_LOGINIMG_VIEWALLCHAR_BACKGRND); /*0x608cdf*/
  LOBYTE(v97) = 1; /*0x608cee*/
  v4 = sub_44427B(a1: v94, a2: &v95); /*0x608cf2*/
  v78 = v5; /*0x608cf7*/
  v93 = &v78; /*0x608cfa*/
  LOBYTE(v97) = 2; /*0x608cfe*/
  sub_48EC62(a1: v4); /*0x608d02*/
  CWnd::SetBackgrnd(a1: v78, a2: v79, a3: v80); /*0x608d09*/
  sub_402D9A(this: v94); /*0x608d11*/
  sub_402D9A(this: &v96); /*0x608d19*/
  sub_402D9A(this: &v95); /*0x608d21*/
  v87 = 0; /*0x608d26*/
  v86[0] = 1; /*0x608d29*/
  v86[1] = 0; /*0x608d30*/
  v86[2] = 0; /*0x608d33*/
  v80 = SP_3983_UI_LOGINIMG_VIEWALLCHAR_BTENTER; /*0x608d39*/
  v79 = &v95; /*0x608d3e*/
  v97 = 3; /*0x608d3f*/
  v6 = StringPool::GetInstance(); /*0x608d46*/
  StringW = StringPool::GetStringW(this: v6, a2: v79, a3: v80); /*0x608d4d*/
  LOBYTE(v97) = 4; /*0x608d56*/
  ZXString<unsigned short>::operator=(a1: StringW); /*0x608d5a*/
  LOBYTE(v97) = 3; /*0x608d62*/
  ZXString<unsigned short>::_Release(a1: &v95); /*0x608d66*/
  v8 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x608d77*/
  v93 = v8; /*0x608d7c*/
  LOBYTE(v97) = 5; /*0x608d81*/
  if ( v8 != 0 ) /*0x608d85*/
    v9 = CCtrlButton::CCtrlButton(this: v8); /*0x608d89*/
  else
    v9 = 0; /*0x608d90*/
  LOBYTE(v97) = 3; /*0x608d96*/
  sub_4284FF(a1: v9); /*0x608d9a*/
  (*(**(this + 29) + 32))(a1: *(this + 29), a2: this, a3: 1001, a4: 415, a5: 377, a6: 0, a7: v86); /*0x608db9*/
  v10 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 116); /*0x608dc0*/
  v93 = v10; /*0x608dc5*/
  LOBYTE(v97) = 6; /*0x608dca*/
  if ( v10 != 0 ) /*0x608dce*/
    v11 = CCtrlScrollBar::CCtrlScrollBar(this: v10); /*0x608dd2*/
  else
    v11 = 0; /*0x608dd9*/
  LOBYTE(v97) = 3; /*0x608de2*/
  sub_428597(a1: v11); /*0x608de6*/
  (*(**(this + 33) + 32))(a1: *(this + 33), a2: this, a3: 1002, a4: 1, a5: 6, a6: 376, a7: 67, a8: 345, a9: 0); /*0x608e0a*/
  *(*(this + 33) + 52) = 350; /*0x608e13*/
  v12 = *(*(this + 27) + 300); /*0x608e1d*/
  if ( v12 <= 9 ) /*0x608e26*/
    v13 = 0; /*0x608e32*/
  else
    v13 = v12 / 3 - 2; /*0x608e2f*/
  CCtrlScrollBar::SetScrollRange(this: *(this + 33), a2: v13); /*0x608e3b*/
  sub_402F85(pvargDest: &v84, pvargSrc: &pvargSrc); /*0x608e49*/
  LOBYTE(v97) = 7; /*0x608e52*/
  sub_402F85(pvargDest: &v83, pvargSrc: &pvargSrc); /*0x608e56*/
  v77 = 0; /*0x608e5b*/
  v76 = 0; /*0x608e5c*/
  v75 = &v84; /*0x608e60*/
  v74 = &v83; /*0x608e64*/
  v73 = v14; /*0x608e65*/
  v93 = &v73; /*0x608e68*/
  LOBYTE(v97) = 8; /*0x608e71*/
  v15 = StringPool::GetInstance(); /*0x608e75*/
  StringPool::GetBSTR(this: v15, a2: &v73, a3: SP_1323_UI_LOGINIMG_CHARSELECT_NAMETAG); /*0x608e7c*/
  LOBYTE(v97) = 8; /*0x608e88*/
  v16 = sub_406472(); /*0x608e8c*/
  v17 = sub_402680(a1: v16); /*0x608e93*/
  IWzResMan::GetObjectA(this: v17, pvargDest: &v82, sUOL: v73, vParam: v74, vAux: v75); /*0x608e9a*/
  LOBYTE(v97) = 10; /*0x608ea1*/
  Unknown = Ztl_variant_t::GetUnknown(a1: v76, a2: v77); /*0x608eac*/
  v91 = 0; /*0x608eaf*/
  if ( Unknown != 0 ) /*0x608eb2*/
  {
    v18 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown); /*0x608ebb*/
    if ( v18 < 0 && v18 != -2147467262 ) /*0x608ec9*/
      _com_issue_error(a1: v18); /*0x608ecc*/
  }
  LOBYTE(v97) = 14; /*0x608ed8*/
  v19 = ZComAPI::ZComVariantCopy(pvarg: &v82); /*0x608edc*/
  if ( v19 < 0 ) /*0x608ee4*/
    _com_issue_error(a1: v19); /*0x608ee7*/
  LOBYTE(v97) = 13; /*0x608ef0*/
  v20 = ZComAPI::ZComVariantCopy(pvarg: &v83); /*0x608ef4*/
  if ( v20 < 0 ) /*0x608efc*/
    _com_issue_error(a1: v20); /*0x608eff*/
  LOBYTE(v97) = 12; /*0x608f08*/
  v21 = ZComAPI::ZComVariantCopy(pvarg: &v84); /*0x608f0c*/
  if ( v21 < 0 ) /*0x608f14*/
    _com_issue_error(a1: v21); /*0x608f17*/
  *v94 = 0; /*0x608f22*/
  v96 = this + 15208; /*0x608f25*/
  do /*0x609057*/
  {
    v22 = alloca(32); /*0x608f2b*/
    v71 = 0; /*0x608f32*/
    v70 = 0; /*0x608f33*/
    v23 = _Int2StrW(a1: *v94, a2: &v72); /*0x608f38*/
    p_p_pvargDest = &v69; /*0x608f40*/
    _bstr_t::_bstr_t(a1: v23); /*0x608f44*/
    p_pvarg = &pvarg; /*0x608f4c*/
    LOBYTE(v97) = 12; /*0x608f50*/
    sub_40263B(a1: &v91); /*0x608f54*/
    IWzProperty::GetItem(pvargDest: p_pvarg, a2: v69); /*0x608f5b*/
    LOBYTE(v97) = 16; /*0x608f62*/
    v90 = Ztl_variant_t::GetUnknown(a1: v70, a2: v71); /*0x608f6d*/
    v95 = 0; /*0x608f70*/
    if ( v90 != 0 ) /*0x608f73*/
    {
      v24 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &v90); /*0x608f7c*/
      if ( v24 < 0 && v24 != -2147467262 ) /*0x608f8a*/
        _com_issue_error(a1: v24); /*0x608f8d*/
    }
    LOBYTE(v97) = 18; /*0x608f96*/
    v25 = ZComAPI::ZComVariantCopy(&pvarg); /*0x608f9a*/
    if ( v25 < 0 ) /*0x608fa2*/
      _com_issue_error(a1: v25); /*0x608fa5*/
    for ( i = 0; i < 3; ++i ) /*0x608fad*/
    {
      v27 = alloca(32); /*0x608fb5*/
      v66 = 0; /*0x608fbc*/
      v65 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x608fbd*/
      v28 = _Int2StrW(a1: i, a2: &v67); /*0x608fc0*/
      p_p_pvargDest = &v64; /*0x608fc8*/
      _bstr_t::_bstr_t(a1: v28); /*0x608fcc*/
      LOBYTE(v97) = 18; /*0x608fd8*/
      sub_40263B(a1: &v95); /*0x608fdc*/
      IWzProperty::GetItem(&pvargDest, a2: v64); /*0x608fe3*/
      LOBYTE(v97) = 20; /*0x608fea*/
      v93 = Ztl_variant_t::GetUnknown(a1: v65, a2: v66); /*0x608ff6*/
      v29 = sub_41E527(a1: &v93); /*0x608ffd*/
      if ( v29 < 0 && v29 != -2147467262 ) /*0x60900b*/
        _com_issue_error(a1: v29); /*0x60900e*/
      LOBYTE(v97) = 18; /*0x609017*/
      v30 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x60901b*/
      if ( v30 < 0 ) /*0x609023*/
        _com_issue_error(a1: v30); /*0x609026*/
      v96 += 4; /*0x60902b*/
    }
    LOBYTE(v97) = 12; /*0x60903e*/
    if ( v95 != 0 ) /*0x609042*/
      (*(*v95 + 8))(a1: v95); /*0x609047*/
    ++*v94; /*0x60904a*/
  }
  while ( *v94 < 2 ); /*0x609057*/
  *v94 = 0; /*0x609069*/
  v96 = this + 15232; /*0x60906c*/
  do /*0x6091be*/
  {
    v95 = 0; /*0x60906f*/
    v65 = SP_3986_UI_LOGINIMG_VIEWALLCHAR_WORLDICONS_D; /*0x609075*/
    v64 = &v93; /*0x60907a*/
    LOBYTE(v97) = 21; /*0x60907b*/
    v31 = StringPool::GetInstance(); /*0x60907f*/
    v32 = *StringPool::GetStringW(this: v31, a2: v64, a3: v65); /*0x60908b*/
    LOBYTE(v97) = 22; /*0x609090*/
    sub_416821(a1: &v95, a2: v32, a3: v94[0]); /*0x609099*/
    LOBYTE(v97) = 21; /*0x6090a4*/
    ZXString<unsigned short>::_Release(a1: &v93); /*0x6090a8*/
    VariantInit(&pvarg); /*0x6090b1*/
    v33 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x6090bc*/
    if ( v33 < 0 ) /*0x6090c5*/
      _com_issue_error(a1: v33); /*0x6090c8*/
    LOBYTE(v97) = 23; /*0x6090d1*/
    VariantInit(pvarg: &pvargDest); /*0x6090d5*/
    v34 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc); /*0x6090e0*/
    if ( v34 < 0 ) /*0x6090e9*/
      _com_issue_error(a1: v34); /*0x6090ec*/
    v65 = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x6090f1*/
    v64 = 0; /*0x6090f2*/
    v63 = &pvarg; /*0x6090f6*/
    p_pvargDest = &pvargDest; /*0x6090fa*/
    LOBYTE(v97) = 24; /*0x6090fe*/
    v35 = sub_41468C(a1: &v95); /*0x609102*/
    v61 = v36; /*0x609107*/
    p_p_pvargDest = &v61; /*0x60910a*/
    _bstr_t::_bstr_t(a1: v35); /*0x60910e*/
    LOBYTE(v97) = 24; /*0x60911a*/
    v37 = sub_406472(); /*0x60911e*/
    v38 = sub_402680(a1: v37); /*0x609125*/
    IWzResMan::GetObjectA(this: v38, pvargDest: &v81, sUOL: v61, vParam: p_pvargDest, vAux: v63); /*0x60912c*/
    LOBYTE(v97) = 26; /*0x609133*/
    v90 = Ztl_variant_t::GetUnknown(a1: v64, a2: v65); /*0x60913f*/
    v39 = sub_41E527(a1: &v90); /*0x609146*/
    if ( v39 < 0 && v39 != -2147467262 ) /*0x609154*/
      _com_issue_error(a1: v39); /*0x609157*/
    LOBYTE(v97) = 24; /*0x609163*/
    v40 = ZComAPI::ZComVariantCopy(pvarg: &v81); /*0x609167*/
    if ( v40 < 0 ) /*0x60916f*/
      _com_issue_error(a1: v40); /*0x609172*/
    LOBYTE(v97) = 23; /*0x60917b*/
    v41 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x60917f*/
    if ( v41 < 0 ) /*0x609187*/
      _com_issue_error(a1: v41); /*0x60918a*/
    LOBYTE(v97) = 21; /*0x609193*/
    v42 = ZComAPI::ZComVariantCopy(&pvarg); /*0x609197*/
    if ( v42 < 0 ) /*0x60919f*/
      _com_issue_error(a1: v42); /*0x6091a2*/
    LOBYTE(v97) = 12; /*0x6091aa*/
    ZXString<unsigned short>::_Release(a1: &v95); /*0x6091ae*/
    ++*v94; /*0x6091b3*/
    v96 += 4; /*0x6091b6*/
  }
  while ( *v94 < 30 ); /*0x6091be*/
  v65 = SP_1410_CANVASFONT; /*0x6091c7*/
  v64 = &v93; /*0x6091cc*/
  v43 = StringPool::GetInstance(); /*0x6091cd*/
  v44 = *StringPool::GetStringW(this: v43, a2: v64, a3: v65); /*0x6091d9*/
  LOBYTE(v97) = 27; /*0x6091e4*/
  PcCreateObject::IWzFont(sUOL: v44, pObj: this + 3800, pUnkOuter: 0); /*0x6091e8*/
  LOBYTE(v97) = 12; /*0x6091f3*/
  ZXString<unsigned short>::_Release(a1: &v93); /*0x6091f7*/
  VariantInit(&pvarg); /*0x609200*/
  v45 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x60920b*/
  v46 = v65; /*0x609213*/
  if ( v45 < 0 ) /*0x609214*/
    _com_issue_error(a1: v45); /*0x609217*/
  v65 = &pvarg; /*0x60921f*/
  v64 = -6710887; /*0x609220*/
  v63 = 12; /*0x609225*/
  p_pvargDest = v46; /*0x609227*/
  p_p_pvargDest = &p_pvargDest; /*0x60922a*/
  v61 = SP_5527_ARIAL; /*0x60922d*/
  LOBYTE(v97) = 28; /*0x609233*/
  v47 = StringPool::GetInstance(); /*0x609237*/
  StringPool::GetBSTR(this: v47, a2: &p_pvargDest, a3: v61); /*0x60923e*/
  LOBYTE(v97) = 28; /*0x609249*/
  sub_4284A8(a1: this + 15200); /*0x60924d*/
  sub_46341A(a1: p_pvargDest, a2: v63, a3: v64, a4: v65); /*0x609254*/
  LOBYTE(v97) = 12; /*0x60925d*/
  v48 = ZComAPI::ZComVariantCopy(&pvarg); /*0x609261*/
  if ( v48 < 0 ) /*0x609269*/
    _com_issue_error(a1: v48); /*0x60926c*/
  v65 = SP_1410_CANVASFONT; /*0x609274*/
  v64 = &v93; /*0x609279*/
  v49 = StringPool::GetInstance(); /*0x60927a*/
  v50 = *StringPool::GetStringW(this: v49, a2: v64, a3: v65); /*0x609286*/
  LOBYTE(v97) = 30; /*0x609291*/
  PcCreateObject::IWzFont(sUOL: v50, pObj: this + 3801, pUnkOuter: 0); /*0x609295*/
  LOBYTE(v97) = 12; /*0x6092a0*/
  ZXString<unsigned short>::_Release(a1: &v93); /*0x6092a4*/
  VariantInit(&pvarg); /*0x6092ad*/
  v51 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x6092b8*/
  v52 = v65; /*0x6092c0*/
  if ( v51 < 0 ) /*0x6092c1*/
    _com_issue_error(a1: v51); /*0x6092c4*/
  v65 = &pvarg; /*0x6092cc*/
  v64 = -1; /*0x6092cd*/
  v63 = 12; /*0x6092cf*/
  p_pvargDest = v52; /*0x6092d1*/
  p_p_pvargDest = &p_pvargDest; /*0x6092d4*/
  v61 = SP_5527_ARIAL; /*0x6092d7*/
  LOBYTE(v97) = 31; /*0x6092dd*/
  v53 = StringPool::GetInstance(); /*0x6092e1*/
  StringPool::GetBSTR(this: v53, a2: &p_pvargDest, a3: v61); /*0x6092e8*/
  LOBYTE(v97) = 31; /*0x6092f3*/
  sub_4284A8(a1: this + 15204); /*0x6092f7*/
  sub_46341A(a1: p_pvargDest, a2: v63, a3: v64, a4: v65); /*0x6092fe*/
  LOBYTE(v97) = 12; /*0x609307*/
  v54 = ZComAPI::ZComVariantCopy(&pvarg); /*0x60930b*/
  if ( v54 < 0 ) /*0x609313*/
    _com_issue_error(a1: v54); /*0x609316*/
  v55 = 0; /*0x60931e*/
  if ( *(*(this + 27) + 300) > 0 ) /*0x609326*/
  {
    v56 = (this + 376); /*0x609328*/
    do /*0x60933f*/
    {
      *v56 = 0; /*0x60932e*/
      ++v55; /*0x609333*/
      v56 = (v56 + 231); /*0x609334*/
    }
    while ( v55 < *(*(this + 27) + 300) ); /*0x60933f*/
  }
  v57 = 0; /*0x60934a*/
  if ( *(*(this + 27) + 300) > 0 ) /*0x609353*/
  {
    v95 = a2; /*0x609358*/
    v58 = (this + 136); /*0x60935b*/
    do /*0x609376*/
    {
      v59 = v95; /*0x609361*/
      v95 += 684; /*0x609364*/
      *v58 = v59; /*0x609367*/
      ++v57; /*0x60936c*/
      ++v58; /*0x60936d*/
    }
    while ( v57 < *(*(this + 27) + 300) ); /*0x609376*/
  }
  v60 = *(*(this + 27) + 300) <= 0; /*0x60937b*/
  v96 = 0; /*0x609381*/
  if ( !v60 ) /*0x609384*/
  {
    do /*0x6093ba*/
    {
      if ( *a2 == 0 ) /*0x609391*/
        break; /*0x609391*/
      CUIAvatarVAC::MakeCharacter(this, a2: v96, a3: a2); /*0x609399*/
      CUIAvatarVAC::MakeWorldIcon(this, a2: v96++); /*0x6093a3*/
      a2 = (a2 + 684); /*0x6093b1*/
    }
    while ( v96 < *(*(this + 27) + 300) ); /*0x6093ba*/
  }
  CUIAvatarVAC::SetSelection(this, a2: 0); /*0x6093bf*/
  CUIAvatarVAC::MakeSelectedCharacterNameTag(this); /*0x6093c6*/
  LOBYTE(v97) = 3; /*0x6093d0*/
  if ( v91 != 0 ) /*0x6093d4*/
    (*(*v91 + 8))(a1: v91); /*0x6093d9*/
  v97 = -1; /*0x6093dc*/
  ZXString<unsigned short>::_Release(a1: &v87); /*0x6093e3*/
}
```

## CUIAvatar::OnCreate @ 0x6042dc

```c
_DWORD *__thiscall CUIAvatar::OnCreate(CUIAvatar *this, int **a2)
{
  int v3; // ecx
  _DWORD *Instance; // eax
  int v5; // eax
  int *v6; // eax
  int v7; // eax
  int v8; // eax
  int v9; // eax
  int v10; // eax
  void *v11; // esp
  wchar_t *v12; // eax
  int v13; // eax
  int v14; // eax
  int i; // edi
  void *v16; // esp
  wchar_t *v17; // eax
  int v18; // eax
  int v19; // eax
  _DWORD *v20; // eax
  int v21; // eax
  int v22; // ecx
  _DWORD *v23; // eax
  int v24; // eax
  _DWORD *v25; // eax
  int v26; // eax
  int v27; // ecx
  _DWORD *v28; // eax
  int v29; // eax
  int v30; // ecx
  int *v31; // eax
  int j; // edx
  int v33; // ecx
  int v34; // ecx
  _DWORD *v35; // eax
  int v36; // eax
  wchar_t *v37; // eax
  _DWORD *v38; // eax
  bool v39; // zf
  int v40; // ecx
  _DWORD *Alpha; // eax
  int v42; // ecx
  _DWORD *v43; // eax
  int v44; // eax
  wchar_t *v45; // eax
  _DWORD *v46; // eax
  _DWORD *v47; // eax
  int v48; // ecx
  int v49; // ecx
  _DWORD *v50; // eax
  int v51; // eax
  wchar_t *v52; // eax
  _DWORD *v53; // eax
  int v54; // ecx
  _DWORD *v55; // eax
  int v56; // ecx
  _DWORD *v57; // eax
  int v58; // eax
  wchar_t *v59; // eax
  _DWORD *v60; // eax
  _DWORD *v61; // eax
  CUIAvatar *v62; // esi
  int *v63; // eax
  int *v64; // edi
  int **v65; // esi
  int v66; // eax
  int v67; // eax
  int *v68; // eax
  int v69; // ecx
  int v70; // eax
  int v71; // eax
  int v72; // eax
  int v73; // eax
  int v74; // eax
  int v75; // eax
  int *basic_font; // eax
  int *v77; // ecx
  _DWORD *v78; // eax
  _DWORD *v79; // eax
  int v80; // eax
  int v81; // eax
  _DWORD *result; // eax
  int *v83; // [esp-204h] [ebp-2A4h] BYREF
  int v84; // [esp-200h] [ebp-2A0h]
  VARIANTARG *v85; // [esp-1FCh] [ebp-29Ch]
  VARIANTARG *p_pvargDest; // [esp-1F8h] [ebp-298h]
  __int64 v87; // [esp-1D8h] [ebp-278h]
  int v88; // [esp-17Ch] [ebp-21Ch] BYREF
  int v89; // [esp-178h] [ebp-218h]
  int v90; // [esp-174h] [ebp-214h]
  int v91; // [esp-170h] [ebp-210h] BYREF
  int v92; // [esp-16Ch] [ebp-20Ch]
  int v93; // [esp-168h] [ebp-208h]
  int v94; // [esp-164h] [ebp-204h]
  int v95; // [esp-11Ch] [ebp-1BCh] BYREF
  int v96; // [esp-118h] [ebp-1B8h]
  int v97; // [esp-114h] [ebp-1B4h]
  int v98; // [esp-110h] [ebp-1B0h] BYREF
  int v99; // [esp-10Ch] [ebp-1ACh]
  int v100; // [esp-108h] [ebp-1A8h]
  int v101; // [esp-104h] [ebp-1A4h]
  int v102; // [esp-BCh] [ebp-15Ch] BYREF
  int v103; // [esp-B8h] [ebp-158h]
  int v104; // [esp-B4h] [ebp-154h]
  int v105; // [esp-B0h] [ebp-150h] BYREF
  int v106; // [esp-ACh] [ebp-14Ch]
  int v107; // [esp-A8h] [ebp-148h]
  int v108; // [esp-A4h] [ebp-144h]
  int v109; // [esp-5Ch] [ebp-FCh] BYREF
  int v110; // [esp-58h] [ebp-F8h]
  int v111; // [esp-54h] [ebp-F4h]
  int v112; // [esp-50h] [ebp-F0h] BYREF
  int v113; // [esp-4Ch] [ebp-ECh] BYREF
  int v114; // [esp-48h] [ebp-E8h]
  VARIANTARG *p_pvarg; // [esp-44h] [ebp-E4h]
  int v116; // [esp-40h] [ebp-E0h] BYREF
  VARIANTARG *v117; // [esp-30h] [ebp-D0h]
  int v118; // [esp-2Ch] [ebp-CCh] BYREF
  int v119; // [esp-28h] [ebp-C8h]
  int v120; // [esp-24h] [ebp-C4h]
  wchar_t v121; // [esp-20h] [ebp-C0h] BYREF
  int v122; // [esp-14h] [ebp-B4h] BYREF
  VARIANTARG *v123; // [esp-10h] [ebp-B0h]
  VARIANTARG *v124; // [esp-Ch] [ebp-ACh]
  int v125; // [esp-8h] [ebp-A8h]
  int v126; // [esp-4h] [ebp-A4h]
  VARIANTARG v127; // [esp+Ch] [ebp-94h] BYREF
  VARIANTARG v128; // [esp+1Ch] [ebp-84h] BYREF
  VARIANTARG v129; // [esp+2Ch] [ebp-74h] BYREF
  int Unknown; // [esp+3Ch] [ebp-64h] BYREF
  VARIANTARG pvargDest; // [esp+40h] [ebp-60h] BYREF
  int v132; // [esp+50h] [ebp-50h] BYREF
  _DWORD *v133; // [esp+54h] [ebp-4Ch] BYREF
  int v134; // [esp+58h] [ebp-48h]
  VARIANTARG v135; // [esp+5Ch] [ebp-44h] BYREF
  VARIANTARG pvarg; // [esp+6Ch] [ebp-34h] BYREF
  char *v137; // [esp+7Ch] [ebp-24h]
  int *v138; // [esp+80h] [ebp-20h] BYREF
  int *v139; // [esp+84h] [ebp-1Ch]
  int v140; // [esp+88h] [ebp-18h] BYREF
  int v141; // [esp+8Ch] [ebp-14h] BYREF
  CUIAvatar *v142; // [esp+90h] [ebp-10h]
  int v143; // [esp+9Ch] [ebp-4h]

  v142 = this; /*0x6042fa*/
  sub_402F85(pvargDest: &v129, pvargSrc: &pvargSrc); /*0x6042fd*/
  v143 = 0; /*0x60430b*/
  sub_402F85(pvargDest: &v128, pvargSrc: &pvargSrc); /*0x60430e*/
  v126 = 0; /*0x604313*/
  v125 = 0; /*0x604314*/
  v124 = &v129; /*0x604318*/
  v123 = &v128; /*0x60431f*/
  v122 = v3; /*0x604320*/
  v141 = &v122; /*0x604323*/
  LOBYTE(v143) = 1; /*0x60432c*/
  Instance = StringPool::GetInstance(); /*0x604330*/
  StringPool::GetBSTR(this: Instance, a2: &v122, a3: SP_1323_UI_LOGINIMG_CHARSELECT_NAMETAG); /*0x604337*/
  LOBYTE(v143) = 1; /*0x604343*/
  v5 = sub_406472(); /*0x604347*/
  v6 = sub_402680(a1: v5); /*0x60434e*/
  IWzResMan::GetObjectA(this: v6, pvargDest: &v127, sUOL: v122, vParam: v123, vAux: v124); /*0x604355*/
  LOBYTE(v143) = 3; /*0x60435c*/
  Unknown = Ztl_variant_t::GetUnknown(a1: v125, a2: v126); /*0x604367*/
  v133 = 0; /*0x60436a*/
  if ( Unknown != 0 ) /*0x60436d*/
  {
    v7 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown); /*0x604376*/
    if ( v7 < 0 && v7 != -2147467262 ) /*0x604384*/
      _com_issue_error(a1: v7); /*0x604387*/
  }
  LOBYTE(v143) = 7; /*0x604393*/
  v8 = ZComAPI::ZComVariantCopy(pvarg: &v127); /*0x604397*/
  if ( v8 < 0 ) /*0x60439f*/
    _com_issue_error(a1: v8); /*0x6043a2*/
  LOBYTE(v143) = 6; /*0x6043ae*/
  v9 = ZComAPI::ZComVariantCopy(pvarg: &v128); /*0x6043b2*/
  if ( v9 < 0 ) /*0x6043ba*/
    _com_issue_error(a1: v9); /*0x6043bd*/
  LOBYTE(v143) = 5; /*0x6043c6*/
  v10 = ZComAPI::ZComVariantCopy(pvarg: &v129); /*0x6043ca*/
  if ( v10 < 0 ) /*0x6043d2*/
    _com_issue_error(a1: v10); /*0x6043d5*/
  v134 = 0; /*0x6043e0*/
  v137 = this + 276; /*0x6043e3*/
  do /*0x604510*/
  {
    v11 = alloca(32); /*0x6043e9*/
    v120 = 0; /*0x6043f0*/
    v119 = 0; /*0x6043f1*/
    v12 = _Int2StrW(a1: v134, a2: &v121); /*0x6043f6*/
    v139 = &v118; /*0x6043fe*/
    _bstr_t::_bstr_t(a1: v12); /*0x604402*/
    v117 = &v135; /*0x60440a*/
    LOBYTE(v143) = 5; /*0x60440e*/
    sub_40263B(a1: &v133); /*0x604412*/
    IWzProperty::GetItem(pvargDest: v117, a2: v118); /*0x604419*/
    LOBYTE(v143) = 9; /*0x604420*/
    v140 = Ztl_variant_t::GetUnknown(a1: v119, a2: v120); /*0x60442b*/
    v138 = 0; /*0x60442e*/
    if ( v140 != 0 ) /*0x604431*/
    {
      v13 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &v140); /*0x60443a*/
      if ( v13 < 0 && v13 != -2147467262 ) /*0x604448*/
        _com_issue_error(a1: v13); /*0x60444b*/
    }
    LOBYTE(v143) = 11; /*0x604454*/
    v14 = ZComAPI::ZComVariantCopy(pvarg: &v135); /*0x604458*/
    if ( v14 < 0 ) /*0x604460*/
      _com_issue_error(a1: v14); /*0x604463*/
    for ( i = 0; i < 3; ++i ) /*0x60446b*/
    {
      v16 = alloca(32); /*0x604473*/
      p_pvarg = 0; /*0x60447a*/
      v114 = 0; /*0x60447b*/
      v17 = _Int2StrW(a1: i, a2: &v116); /*0x60447e*/
      v139 = &v113; /*0x604486*/
      _bstr_t::_bstr_t(a1: v17); /*0x60448a*/
      LOBYTE(v143) = 11; /*0x604496*/
      sub_40263B(a1: &v138); /*0x60449a*/
      IWzProperty::GetItem(pvargDest: &pvarg, a2: v113); /*0x6044a1*/
      LOBYTE(v143) = 13; /*0x6044a8*/
      v141 = Ztl_variant_t::GetUnknown(a1: v114, a2: p_pvarg); /*0x6044b4*/
      v18 = sub_41E527(a1: &v141); /*0x6044bb*/
      if ( v18 < 0 && v18 != -2147467262 ) /*0x6044c9*/
        _com_issue_error(a1: v18); /*0x6044cc*/
      LOBYTE(v143) = 11; /*0x6044d5*/
      v19 = ZComAPI::ZComVariantCopy(&pvarg); /*0x6044d9*/
      if ( v19 < 0 ) /*0x6044e1*/
        _com_issue_error(a1: v19); /*0x6044e4*/
      v137 += 4; /*0x6044e9*/
    }
    LOBYTE(v143) = 5; /*0x6044fa*/
    sub_4027E9(a1: &v138); /*0x6044fe*/
    ++v134; /*0x604503*/
  }
  while ( v134 < 2 ); /*0x604510*/
  p_pvarg = SP_1410_CANVASFONT; /*0x604519*/
  v114 = &v141; /*0x60451e*/
  v20 = StringPool::GetInstance(); /*0x60451f*/
  v21 = *StringPool::GetStringW(this: v20, a2: v114, a3: p_pvarg); /*0x60452b*/
  LOBYTE(v143) = 14; /*0x604536*/
  PcCreateObject::IWzFont(sUOL: v21, pObj: this + 67, pUnkOuter: 0); /*0x60453a*/
  LOBYTE(v143) = 5; /*0x604545*/
  ZXString<unsigned short>::_Release(a1: &v141); /*0x604549*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x604556*/
  p_pvarg = &pvarg; /*0x60455e*/
  v114 = -6710887; /*0x60455f*/
  v113 = 12; /*0x604564*/
  v112 = v22; /*0x604566*/
  v139 = &v112; /*0x604569*/
  LOBYTE(v143) = 15; /*0x604573*/
  v23 = StringPool::GetInstance(); /*0x604577*/
  StringPool::GetBSTR(this: v23, a2: &v112, a3: SP_5527_ARIAL); /*0x60457e*/
  LOBYTE(v143) = 15; /*0x604589*/
  sub_4284A8(a1: this + 268); /*0x60458d*/
  sub_46341A(a1: v112, a2: v113, a3: v114, a4: p_pvarg); /*0x604594*/
  LOBYTE(v143) = 5; /*0x60459d*/
  v24 = ZComAPI::ZComVariantCopy(&pvarg); /*0x6045a1*/
  if ( v24 < 0 ) /*0x6045a9*/
    _com_issue_error(a1: v24); /*0x6045ac*/
  p_pvarg = SP_1410_CANVASFONT; /*0x6045b4*/
  v114 = &v141; /*0x6045b9*/
  v25 = StringPool::GetInstance(); /*0x6045ba*/
  v26 = *StringPool::GetStringW(this: v25, a2: v114, a3: p_pvarg); /*0x6045c6*/
  LOBYTE(v143) = 17; /*0x6045d1*/
  PcCreateObject::IWzFont(sUOL: v26, pObj: this + 68, pUnkOuter: 0); /*0x6045d5*/
  LOBYTE(v143) = 5; /*0x6045e0*/
  ZXString<unsigned short>::_Release(a1: &v141); /*0x6045e4*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x6045f1*/
  p_pvarg = &pvarg; /*0x6045f9*/
  v114 = -1; /*0x6045fa*/
  v113 = 12; /*0x6045fc*/
  v112 = v27; /*0x6045fe*/
  v139 = &v112; /*0x604601*/
  LOBYTE(v143) = 18; /*0x604606*/
  v28 = StringPool::GetInstance(); /*0x60460a*/
  StringPool::GetBSTR(this: v28, a2: &v112, a3: SP_5527_ARIAL); /*0x604611*/
  LOBYTE(v143) = 18; /*0x60461c*/
  sub_4284A8(a1: this + 272); /*0x604620*/
  sub_46341A(a1: v112, a2: v113, a3: v114, a4: p_pvarg); /*0x604627*/
  LOBYTE(v143) = 5; /*0x604630*/
  v29 = ZComAPI::ZComVariantCopy(&pvarg); /*0x604634*/
  if ( v29 < 0 ) /*0x60463c*/
    _com_issue_error(a1: v29); /*0x60463f*/
  v30 = a2; /*0x604644*/
  v31 = (this + 112); /*0x604649*/
  for ( j = 15; j != 0; --j ) /*0x60464c*/
  {
    *v31++ = v30; /*0x60464d*/
    v30 += 684; /*0x604652*/
  }
  CUIAvatar::Refresh(this); /*0x60465d*/
  CUIAvatar::SelectCharacter(this, a2: *(*(this + 27) + 400)); /*0x60466d*/
  p_pvarg = 0; /*0x604672*/
  v114 = 255; /*0x604673*/
  v113 = 0; /*0x604678*/
  v112 = v33; /*0x604679*/
  v139 = &v112; /*0x60467c*/
  sub_432C10(a1: 0); /*0x604680*/
  v111 = 0; /*0x604685*/
  v110 = 0; /*0x604686*/
  v109 = v34; /*0x604687*/
  v138 = &v109; /*0x60468a*/
  LOBYTE(v143) = 20; /*0x60468e*/
  sub_432BDD(a1: 0); /*0x604692*/
  LOBYTE(v143) = 21; /*0x6046a0*/
  v35 = StringPool::GetInstance(); /*0x6046a4*/
  StringPool::GetStringW(this: v35, a2: &v140, a3: SP_1315_UI_LOGINIMG_CHARSELECT_PAGEL_); /*0x6046ab*/
  LOBYTE(v143) = 22; /*0x6046bb*/
  v36 = sub_48EBCC(a1: &v141, a2: str_0); /*0x6046bf*/
  LOBYTE(v143) = 25; /*0x6046c7*/
  v37 = sub_41468C(a1: v36); /*0x6046cb*/
  v38 = sub_43E86F(a1: &a2, a2: v37, a3: 0, a4: v109, a5: v110, a6: v111, a7: v112, a8: v113, a9: v114, a10: p_pvarg); /*0x6046d5*/
  sub_428712(a1: *v38); /*0x6046e5*/
  if ( a2 != 0 ) /*0x6046ef*/
    ((*a2)[2])(a1: a2); /*0x6046f4*/
  LOBYTE(v143) = 24; /*0x6046fa*/
  ZXString<unsigned short>::_Release(a1: &v141); /*0x6046fe*/
  LOBYTE(v143) = 5; /*0x604706*/
  ZXString<unsigned short>::_Release(a1: &v140); /*0x60470a*/
  v135 = pvargSrc; /*0x60471a*/
  *&pvarg.vt = *&pvargSrc.vt; /*0x604726*/
  *&pvarg.decVal.Hi32 = *&pvargSrc.decVal.Hi32; /*0x604727*/
  v39 = *(v142 + 61) == 0; /*0x60472e*/
  pvarg.cyVal.Hi = pvargSrc.cyVal.Hi; /*0x604730*/
  if ( v39 ) /*0x604731*/
    _com_issue_error(a1: -2147467261); /*0x604738*/
  (*(**(v142 + 61) + 144))( /*0x60476c*/
    a1: *(v142 + 61),
    a2: -260,
    a3: -1215,
    a4: *&pvarg.vt,
    a5: pvarg.decVal.Hi32,
    a6: pvarg.lVal,
    a7: pvarg.cyVal.Hi,
    a8: *&v135.vt,
    a9: v135.decVal.Hi32,
    a10: v135.lVal,
    a11: v135.cyVal.Hi);
  v40 = 3; /*0x60477f*/
  if ( (*(v142 + 75) - 1) / 3 + 1 <= 1 ) /*0x604786*/
  {
    v135 = pvargSrc; /*0x604796*/
    *&pvarg.vt = *&pvargSrc.vt; /*0x6047a2*/
    *&pvarg.decVal.Hi32 = *&pvargSrc.decVal.Hi32; /*0x6047a3*/
    v39 = *(v142 + 61) == 0; /*0x6047aa*/
    pvarg.cyVal.Hi = pvargSrc.cyVal.Hi; /*0x6047ac*/
    if ( v39 ) /*0x6047ad*/
      _com_issue_error(a1: -2147467261); /*0x6047b4*/
    Alpha = IWzGr2DLayer::GetAlpha(a1: &a2); /*0x6047c9*/
    v39 = *Alpha == 0; /*0x6047d0*/
    LOBYTE(v143) = 26; /*0x6047d2*/
    if ( v39 ) /*0x6047d6*/
      _com_issue_error(a1: -2147467261); /*0x6047dd*/
    (*(**Alpha + 144))( /*0x604801*/
      a1: *Alpha,
      a2: 0,
      a3: 0,
      a4: *&pvarg.vt,
      a5: pvarg.decVal.Hi32,
      a6: pvarg.lVal,
      a7: pvarg.cyVal.Hi,
      a8: *&v135.vt,
      a9: v135.decVal.Hi32,
      a10: v135.lVal,
      a11: v135.cyVal.Hi);
    LOBYTE(v143) = 5; /*0x60480c*/
    if ( a2 != 0 ) /*0x604810*/
      ((*a2)[2])(a1: a2); /*0x604815*/
  }
  v108 = 0; /*0x604818*/
  v107 = 255; /*0x604819*/
  v106 = 0; /*0x60481e*/
  v105 = v40; /*0x60481f*/
  v139 = &v105; /*0x604822*/
  sub_432C10(a1: 0); /*0x604826*/
  v104 = 0; /*0x60482b*/
  v103 = 0; /*0x60482c*/
  v102 = v42; /*0x60482d*/
  v138 = &v102; /*0x604830*/
  LOBYTE(v143) = 27; /*0x604834*/
  sub_432BDD(a1: 0); /*0x604838*/
  LOBYTE(v143) = 28; /*0x604846*/
  v43 = StringPool::GetInstance(); /*0x60484a*/
  StringPool::GetStringW(this: v43, a2: &v140, a3: SP_1315_UI_LOGINIMG_CHARSELECT_PAGEL_); /*0x604851*/
  LOBYTE(v143) = 29; /*0x604861*/
  v44 = sub_48EBCC(a1: &v141, a2: str_1); /*0x604865*/
  LOBYTE(v143) = 32; /*0x60486d*/
  v45 = sub_41468C(a1: v44); /*0x604871*/
  v46 = sub_43E86F(a1: &a2, a2: v45, a3: 0, a4: v102, a5: v103, a6: v104, a7: v105, a8: v106, a9: v107, a10: v108); /*0x60487b*/
  sub_428712(a1: *v46); /*0x60488e*/
  if ( a2 != 0 ) /*0x604898*/
    ((*a2)[2])(a1: a2); /*0x60489d*/
  LOBYTE(v143) = 31; /*0x6048a3*/
  ZXString<unsigned short>::_Release(a1: &v141); /*0x6048a7*/
  LOBYTE(v143) = 5; /*0x6048af*/
  ZXString<unsigned short>::_Release(a1: &v140); /*0x6048b3*/
  v135 = pvargSrc; /*0x6048c3*/
  *&pvarg.vt = *&pvargSrc.vt; /*0x6048cf*/
  *&pvarg.decVal.Hi32 = *&pvargSrc.decVal.Hi32; /*0x6048d0*/
  v39 = *(v142 + 63) == 0; /*0x6048d7*/
  pvarg.cyVal.Hi = pvargSrc.cyVal.Hi; /*0x6048d9*/
  if ( v39 ) /*0x6048da*/
    _com_issue_error(a1: -2147467261); /*0x6048e1*/
  (*(**(v142 + 63) + 144))( /*0x604915*/
    a1: *(v142 + 63),
    a2: -260,
    a3: -1215,
    a4: *&pvarg.vt,
    a5: pvarg.decVal.Hi32,
    a6: pvarg.lVal,
    a7: pvarg.cyVal.Hi,
    a8: *&v135.vt,
    a9: v135.decVal.Hi32,
    a10: v135.lVal,
    a11: v135.cyVal.Hi);
  v135 = pvargSrc; /*0x604925*/
  *&pvarg.vt = *&pvargSrc.vt; /*0x604931*/
  *&pvarg.decVal.Hi32 = *&pvargSrc.decVal.Hi32; /*0x604932*/
  v39 = *(v142 + 63) == 0; /*0x604939*/
  pvarg.cyVal.Hi = pvargSrc.cyVal.Hi; /*0x60493b*/
  if ( v39 ) /*0x60493c*/
    _com_issue_error(a1: -2147467261); /*0x604943*/
  v47 = IWzGr2DLayer::GetAlpha(a1: &a2); /*0x604958*/
  v39 = *v47 == 0; /*0x60495f*/
  LOBYTE(v143) = 33; /*0x604961*/
  if ( v39 ) /*0x604965*/
    _com_issue_error(a1: -2147467261); /*0x60496c*/
  (*(**v47 + 144))( /*0x604990*/
    a1: *v47,
    a2: 0,
    a3: 0,
    a4: *&pvarg.vt,
    a5: pvarg.decVal.Hi32,
    a6: pvarg.lVal,
    a7: pvarg.cyVal.Hi,
    a8: *&v135.vt,
    a9: v135.decVal.Hi32,
    a10: v135.lVal,
    a11: v135.cyVal.Hi);
  LOBYTE(v143) = 5; /*0x60499b*/
  if ( a2 != 0 ) /*0x60499f*/
    ((*a2)[2])(a1: a2); /*0x6049a4*/
  v101 = 0; /*0x6049a7*/
  v100 = 255; /*0x6049a8*/
  v99 = 0; /*0x6049ad*/
  v98 = v48; /*0x6049ae*/
  v139 = &v98; /*0x6049b1*/
  sub_432C10(a1: 0); /*0x6049b5*/
  v97 = 0; /*0x6049ba*/
  v96 = 0; /*0x6049bb*/
  v95 = v49; /*0x6049bc*/
  v138 = &v95; /*0x6049bf*/
  LOBYTE(v143) = 34; /*0x6049c3*/
  sub_432BDD(a1: 0); /*0x6049c7*/
  LOBYTE(v143) = 35; /*0x6049d5*/
  v50 = StringPool::GetInstance(); /*0x6049d9*/
  StringPool::GetStringW(this: v50, a2: &v140, a3: SP_1316_UI_LOGINIMG_CHARSELECT_PAGER_); /*0x6049e0*/
  LOBYTE(v143) = 36; /*0x6049f0*/
  v51 = sub_48EBCC(a1: &v141, a2: str_0); /*0x6049f4*/
  LOBYTE(v143) = 39; /*0x6049fc*/
  v52 = sub_41468C(a1: v51); /*0x604a00*/
  v53 = sub_43E86F(a1: &a2, a2: v52, a3: 0, a4: v95, a5: v96, a6: v97, a7: v98, a8: v99, a9: v100, a10: v101); /*0x604a0a*/
  sub_428712(a1: *v53); /*0x604a1d*/
  if ( a2 != 0 ) /*0x604a27*/
    ((*a2)[2])(a1: a2); /*0x604a2c*/
  LOBYTE(v143) = 38; /*0x604a32*/
  ZXString<unsigned short>::_Release(a1: &v141); /*0x604a36*/
  LOBYTE(v143) = 5; /*0x604a3e*/
  ZXString<unsigned short>::_Release(a1: &v140); /*0x604a42*/
  v135 = pvargSrc; /*0x604a52*/
  *&pvarg.vt = *&pvargSrc.vt; /*0x604a5e*/
  *&pvarg.decVal.Hi32 = *&pvargSrc.decVal.Hi32; /*0x604a5f*/
  v39 = *(v142 + 62) == 0; /*0x604a66*/
  pvarg.cyVal.Hi = pvargSrc.cyVal.Hi; /*0x604a68*/
  if ( v39 ) /*0x604a69*/
    _com_issue_error(a1: -2147467261); /*0x604a70*/
  (*(**(v142 + 62) + 144))( /*0x604aa4*/
    a1: *(v142 + 62),
    a2: 188,
    a3: -1213,
    a4: *&pvarg.vt,
    a5: pvarg.decVal.Hi32,
    a6: pvarg.lVal,
    a7: pvarg.cyVal.Hi,
    a8: *&v135.vt,
    a9: v135.decVal.Hi32,
    a10: v135.lVal,
    a11: v135.cyVal.Hi);
  v54 = 3; /*0x604ab7*/
  if ( (*(v142 + 75) - 1) / 3 + 1 <= 1 ) /*0x604abe*/
  {
    v135 = pvargSrc; /*0x604ace*/
    *&pvarg.vt = *&pvargSrc.vt; /*0x604ada*/
    *&pvarg.decVal.Hi32 = *&pvargSrc.decVal.Hi32; /*0x604adb*/
    v39 = *(v142 + 62) == 0; /*0x604ae2*/
    pvarg.cyVal.Hi = pvargSrc.cyVal.Hi; /*0x604ae4*/
    if ( v39 ) /*0x604ae5*/
      _com_issue_error(a1: -2147467261); /*0x604aec*/
    v55 = IWzGr2DLayer::GetAlpha(a1: &a2); /*0x604b01*/
    v39 = *v55 == 0; /*0x604b08*/
    LOBYTE(v143) = 40; /*0x604b0a*/
    if ( v39 ) /*0x604b0e*/
      _com_issue_error(a1: -2147467261); /*0x604b15*/
    (*(**v55 + 144))( /*0x604b39*/
      a1: *v55,
      a2: 0,
      a3: 0,
      a4: *&pvarg.vt,
      a5: pvarg.decVal.Hi32,
      a6: pvarg.lVal,
      a7: pvarg.cyVal.Hi,
      a8: *&v135.vt,
      a9: v135.decVal.Hi32,
      a10: v135.lVal,
      a11: v135.cyVal.Hi);
    LOBYTE(v143) = 5; /*0x604b44*/
    if ( a2 != 0 ) /*0x604b48*/
      ((*a2)[2])(a1: a2); /*0x604b4d*/
  }
  v94 = 0; /*0x604b50*/
  v93 = 255; /*0x604b51*/
  v92 = 0; /*0x604b56*/
  v91 = v54; /*0x604b57*/
  v139 = &v91; /*0x604b5a*/
  sub_432C10(a1: 0); /*0x604b5e*/
  v90 = 0; /*0x604b63*/
  v89 = 0; /*0x604b64*/
  v88 = v56; /*0x604b65*/
  v138 = &v88; /*0x604b68*/
  LOBYTE(v143) = 41; /*0x604b6c*/
  sub_432BDD(a1: 0); /*0x604b70*/
  LOBYTE(v143) = 42; /*0x604b7e*/
  v57 = StringPool::GetInstance(); /*0x604b82*/
  StringPool::GetStringW(this: v57, a2: &v140, a3: SP_1316_UI_LOGINIMG_CHARSELECT_PAGER_); /*0x604b89*/
  LOBYTE(v143) = 43; /*0x604b99*/
  v58 = sub_48EBCC(a1: &v141, a2: str_1); /*0x604b9d*/
  LOBYTE(v143) = 46; /*0x604ba5*/
  v59 = sub_41468C(a1: v58); /*0x604ba9*/
  v60 = sub_43E86F(a1: &a2, a2: v59, a3: 0, a4: v88, a5: v89, a6: v90, a7: v91, a8: v92, a9: v93, a10: v94); /*0x604bb3*/
  sub_428712(a1: *v60); /*0x604bc6*/
  if ( a2 != 0 ) /*0x604bd0*/
    ((*a2)[2])(a1: a2); /*0x604bd5*/
  LOBYTE(v143) = 45; /*0x604bdb*/
  ZXString<unsigned short>::_Release(a1: &v141); /*0x604bdf*/
  LOBYTE(v143) = 5; /*0x604be7*/
  ZXString<unsigned short>::_Release(a1: &v140); /*0x604beb*/
  v135 = pvargSrc; /*0x604bfb*/
  *&pvarg.vt = *&pvargSrc.vt; /*0x604c07*/
  *&pvarg.decVal.Hi32 = *&pvargSrc.decVal.Hi32; /*0x604c08*/
  v39 = *(v142 + 64) == 0; /*0x604c0f*/
  pvarg.cyVal.Hi = pvargSrc.cyVal.Hi; /*0x604c11*/
  if ( v39 ) /*0x604c12*/
    _com_issue_error(a1: -2147467261); /*0x604c19*/
  (*(**(v142 + 64) + 144))( /*0x604c4d*/
    a1: *(v142 + 64),
    a2: 188,
    a3: -1214,
    a4: *&pvarg.vt,
    a5: pvarg.decVal.Hi32,
    a6: pvarg.lVal,
    a7: pvarg.cyVal.Hi,
    a8: *&v135.vt,
    a9: v135.decVal.Hi32,
    a10: v135.lVal,
    a11: v135.cyVal.Hi);
  v135 = pvargSrc; /*0x604c5d*/
  *&pvarg.vt = *&pvargSrc.vt; /*0x604c69*/
  *&pvarg.decVal.Hi32 = *&pvargSrc.decVal.Hi32; /*0x604c6a*/
  v39 = *(v142 + 64) == 0; /*0x604c71*/
  pvarg.cyVal.Hi = pvargSrc.cyVal.Hi; /*0x604c73*/
  if ( v39 ) /*0x604c74*/
    _com_issue_error(a1: -2147467261); /*0x604c7b*/
  v61 = IWzGr2DLayer::GetAlpha(a1: &a2); /*0x604c90*/
  v39 = *v61 == 0; /*0x604c97*/
  LOBYTE(v143) = 47; /*0x604c99*/
  if ( v39 ) /*0x604c9d*/
    _com_issue_error(a1: -2147467261); /*0x604ca4*/
  (*(**v61 + 144))( /*0x604cc8*/
    a1: *v61,
    a2: 0,
    a3: 0,
    a4: *&pvarg.vt,
    a5: pvarg.decVal.Hi32,
    a6: pvarg.lVal,
    a7: pvarg.cyVal.Hi,
    a8: *&v135.vt,
    a9: v135.decVal.Hi32,
    a10: v135.lVal,
    a11: v135.cyVal.Hi);
  LOBYTE(v143) = 5; /*0x604cd3*/
  if ( a2 != 0 ) /*0x604cd7*/
    ((*a2)[2])(a1: a2); /*0x604cdc*/
  v62 = v142; /*0x604cdf*/
  v63 = *sub_605F1B(a1: &v141, a2: 220, a3: 40); /*0x604cf4*/
  v64 = *(v62 + 65); /*0x604cf6*/
  v65 = (v62 + 260); /*0x604cfc*/
  LOBYTE(v143) = 48; /*0x604d04*/
  a2 = v65; /*0x604d08*/
  if ( v64 != v63 ) /*0x604d0b*/
  {
    *v65 = v63; /*0x604d0f*/
    sub_432D38(a1: v65); /*0x604d11*/
    if ( v64 != 0 ) /*0x604d18*/
      (*(*v64 + 8))(a1: v64); /*0x604d1d*/
  }
  LOBYTE(v143) = 5; /*0x604d25*/
  if ( v141 != 0 ) /*0x604d29*/
    (*(*v141 + 8))(a1: v141); /*0x604d2e*/
  VariantInit(&pvarg); /*0x604d3b*/
  v66 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x604d46*/
  if ( v66 < 0 ) /*0x604d4f*/
    _com_issue_error(a1: v66); /*0x604d52*/
  LOBYTE(v143) = 49; /*0x604d5b*/
  VariantInit(pvarg: &pvargDest); /*0x604d5f*/
  v67 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc); /*0x604d6a*/
  if ( v67 < 0 ) /*0x604d73*/
    _com_issue_error(a1: v67); /*0x604d76*/
  v39 = *v65 == 0; /*0x604d7b*/
  LOBYTE(v143) = 50; /*0x604d7d*/
  if ( v39 ) /*0x604d81*/
    _com_issue_error(a1: -2147467261); /*0x604d88*/
  v68 = *a2; /*0x604d95*/
  v87 = *&pvarg.vt; /*0x604d9a*/
  v69 = *v68; /*0x604d9c*/
  v141 = v68; /*0x604db6*/
  v70 = (*(v69 + 144))( /*0x604dba*/
          a1: v68,
          a2: 190,
          a3: -250,
          a4: *&pvargDest.vt,
          a5: pvargDest.decVal.Hi32,
          a6: pvargDest.lVal,
          a7: pvargDest.cyVal.Hi,
          a8: *&pvarg.vt,
          a9: pvarg.decVal.Hi32,
          a10: pvarg.lVal,
          a11: pvarg.cyVal.Hi);
  if ( v70 < 0 ) /*0x604dc2*/
    _com_issue_errorex(a1: v70, a2: v141, a3: dword_BD8348); /*0x604dcd*/
  LOBYTE(v143) = 49; /*0x604dd6*/
  v71 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x604dda*/
  if ( v71 < 0 ) /*0x604de2*/
    _com_issue_error(a1: v71); /*0x604de5*/
  LOBYTE(v143) = 5; /*0x604dee*/
  v72 = ZComAPI::ZComVariantCopy(&pvarg); /*0x604df2*/
  if ( v72 < 0 ) /*0x604dfa*/
    _com_issue_error(a1: v72); /*0x604dfd*/
  sub_402FAB(a1: 0, a2: 3); /*0x604e08*/
  v39 = *a2 == 0; /*0x604e10*/
  LOBYTE(v143) = 51; /*0x604e12*/
  if ( v39 ) /*0x604e16*/
    _com_issue_error(a1: -2147467261); /*0x604e1d*/
  IWzGr2DLayer::GetCanvas(a1: &v132, a2: &v135); /*0x604e2c*/
  LOBYTE(v143) = 53; /*0x604e35*/
  v73 = ZComAPI::ZComVariantCopy(pvarg: &v135); /*0x604e39*/
  if ( v73 < 0 ) /*0x604e41*/
    _com_issue_error(a1: v73); /*0x604e44*/
  VariantInit(pvarg: &pvargDest); /*0x604e53*/
  v74 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc); /*0x604e5f*/
  if ( v74 < 0 ) /*0x604e68*/
    _com_issue_error(a1: v74); /*0x604e6b*/
  LOBYTE(v143) = 54; /*0x604e74*/
  VariantInit(&pvarg); /*0x604e78*/
  v75 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x604e7f*/
  if ( v75 < 0 ) /*0x604e88*/
    _com_issue_error(a1: v75); /*0x604e8b*/
  p_pvargDest = &pvargDest; /*0x604e93*/
  v85 = &pvarg; /*0x604e97*/
  LOBYTE(v143) = 55; /*0x604e9e*/
  basic_font = get_basic_font(a1: &a2, fType: 1); /*0x604ea2*/
  LOBYTE(v143) = 56; /*0x604eab*/
  v84 = sub_4284A5(this: basic_font); /*0x604eb4*/
  v83 = v77; /*0x604eb5*/
  v139 = &v83; /*0x604eb8*/
  v78 = StringPool::GetInstance(); /*0x604ec1*/
  StringPool::GetBSTR(this: v78, a2: &v83, a3: SP_4421_USE_THE_TAB_KEY_TO_TOGGLE_THE_MENU); /*0x604ec8*/
  LOBYTE(v143) = 56; /*0x604ed4*/
  v79 = sub_414576(a1: &v132); /*0x604ed8*/
  IWzCanvas::DrawTextA(this: v79, nLeft: 10, nTop: 15, sText: v83, pFont: v84, vAlpha: v85, vTabOrg: p_pvargDest); /*0x604edf*/
  LOBYTE(v143) = 55; /*0x604ee9*/
  if ( a2 != 0 ) /*0x604eed*/
    ((*a2)[2])(a1: a2); /*0x604ef2*/
  LOBYTE(v143) = 54; /*0x604ef9*/
  v80 = ZComAPI::ZComVariantCopy(&pvarg); /*0x604efd*/
  if ( v80 < 0 ) /*0x604f05*/
    _com_issue_error(a1: v80); /*0x604f08*/
  LOBYTE(v143) = 53; /*0x604f11*/
  v81 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x604f15*/
  if ( v81 < 0 ) /*0x604f1d*/
    _com_issue_error(a1: v81); /*0x604f20*/
  LOBYTE(v143) = 5; /*0x604f2a*/
  if ( v132 != 0 ) /*0x604f2e*/
    (*(*v132 + 8))(a1: v132); /*0x604f33*/
  result = v133; /*0x604f36*/
  v143 = -1; /*0x604f39*/
  if ( v133 != 0 ) /*0x604f3f*/
    return (*(*v133 + 8))(a1: v133); /*0x604f44*/
  return result; /*0x604f50*/
}
```

## CUIAvatar::DrawNameTag @ 0x605b52

```c
_DWORD *__thiscall CUIAvatar::DrawNameTag(CUIAvatar *this, int a2, int a3)
{
  bool v4; // zf
  _DWORD *v5; // esi
  int v6; // eax
  int Width; // eax
  int Height; // esi
  int v9; // eax
  int v10; // ecx
  int v11; // eax
  int v12; // ecx
  int v13; // esi
  int v14; // eax
  int v15; // ecx
  int v16; // eax
  int v17; // eax
  int v18; // ecx
  int v19; // eax
  int v20; // eax
  int v21; // edx
  int v22; // eax
  int v23; // eax
  int v24; // eax
  int *v25; // ecx
  _DWORD *v26; // eax
  int v27; // eax
  int v28; // eax
  _DWORD *result; // eax
  int v30; // [esp-18h] [ebp-7Ch]
  int *v31; // [esp-10h] [ebp-74h]
  int v32; // [esp-Ch] [ebp-70h]
  __int64 int64; // [esp-8h] [ebp-6Ch] BYREF
  VARIANTARG v34; // [esp+Ch] [ebp-58h] BYREF
  VARIANTARG pvargDest; // [esp+1Ch] [ebp-48h] BYREF
  VARIANTARG pvarg; // [esp+2Ch] [ebp-38h] BYREF
  __int64 *v37; // [esp+3Ch] [ebp-28h]
  _DWORD *v38; // [esp+40h] [ebp-24h]
  _DWORD *v39; // [esp+44h] [ebp-20h]
  _DWORD *v40; // [esp+48h] [ebp-1Ch]
  int v41; // [esp+4Ch] [ebp-18h]
  _DWORD *v42; // [esp+50h] [ebp-14h]
  _DWORD *v43; // [esp+54h] [ebp-10h] BYREF
  int v44; // [esp+60h] [ebp-4h]
  BOOL v45; // [esp+70h] [ebp+Ch]

  sub_402FAB(a1: 0, a2: 3); /*0x605b6c*/
  v4 = *(this + a2 + 49) == 0; /*0x605b74*/
  v5 = (this + 4 * a2 + 196); /*0x605b7b*/
  v44 = 0; /*0x605b82*/
  if ( v4 ) /*0x605b85*/
    _com_issue_error(a1: -2147467261); /*0x605b8c*/
  IWzGr2DLayer::GetCanvas(a1: &v43, a2: &pvarg); /*0x605b9b*/
  LOBYTE(v44) = 2; /*0x605ba4*/
  v6 = ZComAPI::ZComVariantCopy(&pvarg); /*0x605ba8*/
  if ( v6 < 0 ) /*0x605bb0*/
    _com_issue_error(a1: v6); /*0x605bb3*/
  if ( *v5 == 0 ) /*0x605bba*/
    _com_issue_error(a1: -2147467261); /*0x605bc1*/
  Width = IWzGr2DLayer::GetWidth(a1: *v5); /*0x605bc8*/
  v4 = *v5 == 0; /*0x605bcd*/
  v41 = Width; /*0x605bcf*/
  if ( v4 ) /*0x605bd2*/
    _com_issue_error(a1: -2147467261); /*0x605bd9*/
  Height = IWzGr2DLayer::GetHeight(a1: *v5); /*0x605bed*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x605bef*/
  HIDWORD(int64) = &pvargDest; /*0x605bf7*/
  LOBYTE(v44) = 3; /*0x605bfd*/
  v9 = *(this + sub_605F09(a1: a2) + 28); /*0x605c06*/
  LODWORD(int64) = v10; /*0x605c0a*/
  v37 = &int64; /*0x605c10*/
  _bstr_t::_bstr_t(a1: v9 + 4); /*0x605c14*/
  LOBYTE(v44) = 3; /*0x605c1f*/
  sub_4284A8(a1: this + 268); /*0x605c23*/
  v37 = sub_42782E(a1: int64, a2: HIDWORD(int64)); /*0x605c2f*/
  LOBYTE(v44) = 2; /*0x605c36*/
  v11 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x605c3a*/
  if ( v11 < 0 ) /*0x605c42*/
    _com_issue_error(a1: v11); /*0x605c45*/
  if ( v43 == 0 ) /*0x605c4f*/
    _com_issue_error(a1: -2147467261); /*0x605c56*/
  v12 = *v43; /*0x605c5e*/
  int64 = Height | 0xFFFFFF00000000LL; /*0x605c65*/
  v13 = v41; /*0x605c66*/
  v42 = v43; /*0x605c6d*/
  v14 = (*(v12 + 140))(a1: v43, a2: 0, a3: 0, a4: v41, a5: int64, a6: 0xFFFFFF); /*0x605c70*/
  if ( v14 < 0 ) /*0x605c78*/
    _com_issue_errorex(a1: v14, a2: v42, a3: _GUID_7600dc6c_9328_4bff_9624_5b0f5c01179e); /*0x605c83*/
  v42 = 0; /*0x605c8d*/
  if ( v13 - 10 > 0 ) /*0x605c90*/
  {
    v40 = (this + 12 * (a3 != 0) + 280); /*0x605ca8*/
    do /*0x605d33*/
    {
      sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x605cb7*/
      v39 = *v40; /*0x605cc1*/
      LOBYTE(v44) = 5; /*0x605cc9*/
      if ( v43 == 0 ) /*0x605ccd*/
        _com_issue_error(a1: -2147467261); /*0x605cd4*/
      v15 = *v43; /*0x605cdc*/
      int64 = pvargDest.cyVal.int64; /*0x605cf0*/
      v38 = v43; /*0x605cf2*/
      v16 = (*(v15 + 128))( /*0x605cf6*/
              a1: v43,
              a2: v42,
              a3: 0,
              a4: v39,
              a5: *&pvargDest.vt,
              a6: pvargDest.decVal.Hi32,
              a7: pvargDest.lVal,
              a8: pvargDest.cyVal.Hi);
      if ( v16 < 0 ) /*0x605cfe*/
        _com_issue_errorex(a1: v16, a2: v38, a3: _GUID_7600dc6c_9328_4bff_9624_5b0f5c01179e); /*0x605d09*/
      LOBYTE(v44) = 2; /*0x605d12*/
      v17 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x605d16*/
      if ( v17 < 0 ) /*0x605d1e*/
        _com_issue_error(a1: v17); /*0x605d21*/
      v42 = (v42 + 9); /*0x605d29*/
    }
    while ( v42 < v41 - 10 ); /*0x605d33*/
  }
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x605d43*/
  LOBYTE(v44) = 6; /*0x605d4d*/
  v45 = a3 != 0; /*0x605d54*/
  v38 = *(this + 3 * v45 + 69); /*0x605d5e*/
  if ( v43 == 0 ) /*0x605d66*/
    _com_issue_error(a1: -2147467261); /*0x605d6d*/
  v18 = *v43; /*0x605d75*/
  int64 = pvargDest.cyVal.int64; /*0x605d86*/
  v39 = v43; /*0x605d8a*/
  v19 = (*(v18 + 128))( /*0x605d8e*/
          a1: v43,
          a2: 0,
          a3: 0,
          a4: v38,
          a5: *&pvargDest.vt,
          a6: pvargDest.decVal.Hi32,
          a7: pvargDest.lVal,
          a8: pvargDest.cyVal.Hi);
  if ( v19 < 0 ) /*0x605d96*/
    _com_issue_errorex(a1: v19, a2: v39, a3: _GUID_7600dc6c_9328_4bff_9624_5b0f5c01179e); /*0x605da1*/
  LOBYTE(v44) = 2; /*0x605daa*/
  v20 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x605dae*/
  if ( v20 < 0 ) /*0x605db6*/
    _com_issue_error(a1: v20); /*0x605db9*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x605dc6*/
  v38 = *(this + 3 * v45 + 71); /*0x605dd8*/
  LOBYTE(v44) = 7; /*0x605de0*/
  if ( v43 == 0 ) /*0x605de4*/
    _com_issue_error(a1: -2147467261); /*0x605deb*/
  v21 = *v43; /*0x605df6*/
  int64 = pvargDest.cyVal.int64; /*0x605e07*/
  v39 = v43; /*0x605e0d*/
  v22 = (*(v21 + 128))( /*0x605e11*/
          a1: v43,
          a2: v41 - 10,
          a3: 0,
          a4: v38,
          a5: *&pvargDest.vt,
          a6: pvargDest.decVal.Hi32,
          a7: pvargDest.lVal,
          a8: pvargDest.cyVal.Hi);
  if ( v22 < 0 ) /*0x605e19*/
    _com_issue_errorex(a1: v22, a2: v39, a3: _GUID_7600dc6c_9328_4bff_9624_5b0f5c01179e); /*0x605e24*/
  LOBYTE(v44) = 2; /*0x605e2d*/
  v23 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x605e31*/
  if ( v23 < 0 ) /*0x605e39*/
    _com_issue_error(a1: v23); /*0x605e3c*/
  sub_402F85(pvargDest: &v34, pvargSrc: &pvargSrc); /*0x605e4a*/
  LOBYTE(v44) = 8; /*0x605e53*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x605e57*/
  HIDWORD(int64) = &v34; /*0x605e5f*/
  LODWORD(int64) = &pvargDest; /*0x605e63*/
  LOBYTE(v44) = 9; /*0x605e6e*/
  v32 = sub_4284A5(this: this + 4 * v45 + 268); /*0x605e77*/
  v24 = *(this + sub_605F09(a1: a2) + 28); /*0x605e82*/
  v31 = v25; /*0x605e86*/
  _bstr_t::_bstr_t(a1: v24 + 4); /*0x605e90*/
  v30 = (v41 - v37) / 2 - 1; /*0x605ea3*/
  LOBYTE(v44) = 9; /*0x605ea7*/
  v26 = sub_414576(a1: &v43); /*0x605eab*/
  IWzCanvas::DrawTextA(this: v26, nLeft: v30, nTop: 2, sText: v31, pFont: v32, vAlpha: int64, vTabOrg: HIDWORD(int64)); /*0x605eb2*/
  LOBYTE(v44) = 8; /*0x605ebb*/
  v27 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x605ebf*/
  if ( v27 < 0 ) /*0x605ec7*/
    _com_issue_error(a1: v27); /*0x605eca*/
  LOBYTE(v44) = 2; /*0x605ed3*/
  v28 = ZComAPI::ZComVariantCopy(pvarg: &v34); /*0x605ed7*/
  if ( v28 < 0 ) /*0x605edf*/
    _com_issue_error(a1: v28); /*0x605ee2*/
  result = v43; /*0x605ee7*/
  v44 = -1; /*0x605eea*/
  if ( v43 != 0 ) /*0x605ef0*/
    return (*(*v43 + 8))(a1: v43); /*0x605ef5*/
  return result; /*0x605ef8*/
}
```

## CUIChannelSelect::OnKey @ 0x628a1c

```c
void __thiscall CUIChannelSelect::OnKey(int *this, unsigned int wParam, signed int lParam)
{
  int v4; // esi
  int v5; // eax
  unsigned int v6; // esi
  int v7; // eax
  int v8; // eax
  unsigned int v9; // ecx
  int v10; // edx
  int v11; // ecx
  unsigned int v12; // eax
  int v13; // eax
  int v14; // ecx
  int v15; // eax
  int v16; // eax

  v4 = *(this + 54); /*0x628a25*/
  if ( lParam >= 0 ) /*0x628a2b*/
  {
    if ( wParam == 9 ) /*0x628a38*/
    {
      if ( dword_BEC33C == 0 /*0x628bb5*/
        || CInputSystem::IsKeyPressed(this: dword_BEC33C, a2: 16) == 0
        && CInputSystem::IsKeyPressed(this: dword_BEC33C, a2: 161) == 0 )
      {
        v16 = *(*(this + 55) + 28); /*0x628bd0*/
        v6 = v4 + 1; /*0x628bd2*/
        if ( v16 == 0 || v6 >= *(v16 - 4) ) /*0x628bda*/
          v6 = 0; /*0x628bdc*/
        goto LABEL_58; /*0x628bdc*/
      }
      v6 = v4 - 1; /*0x628bb7*/
      if ( (v6 & 0x80000000) != 0 ) /*0x628bb8*/
      {
        v15 = *(*(this + 55) + 28); /*0x628bc3*/
        goto LABEL_43; /*0x628bc5*/
      }
LABEL_58:
      sub_629CBD(a1: v6); /*0x628bde*/
      return; /*0x628be2*/
    }
    if ( wParam != 13 ) /*0x628a43*/
    {
      if ( wParam == 27 ) /*0x628a4c*/
      {
        CLogin::CloseChannelSelect(); /*0x628b76*/
        return; /*0x628b7b*/
      }
      if ( wParam != 37 ) /*0x628a55*/
      {
        switch ( wParam ) /*0x628a5c*/
        {
          case '&': /*0x628a5c*/
            if ( v4 < 0 ) /*0x628aed*/
              v4 = 0; /*0x628aef*/
            if ( v4 >= 4 ) /*0x628af3*/
            {
              v6 = v4 - 4; /*0x628b33*/
            }
            else
            {
              v8 = *(this + 55); /*0x628af5*/
              v9 = *(v8 + 28); /*0x628afb*/
              v10 = v4; /*0x628b00*/
              if ( v9 != 0 ) /*0x628b02*/
                v9 = *(v9 - 4); /*0x628b04*/
              v6 = v4 + 4 * (v9 >> 2); /*0x628b0a*/
              v11 = *(v8 + 28); /*0x628b0d*/
              if ( v11 == 0 || v6 >= *(v11 - 4) ) /*0x628b17*/
              {
                v12 = *(v8 + 28); /*0x628b1d*/
                if ( v12 != 0 ) /*0x628b22*/
                  v12 = *(v12 - 4); /*0x628b24*/
                v6 = v10 + 4 * (v12 >> 2) - 4; /*0x628b2a*/
              }
            }
            break;
          case '\'': /*0x628a5c*/
            v7 = *(*(this + 55) + 28); /*0x628aba*/
            if ( v7 != 0 ) /*0x628abe*/
              v7 = *(v7 - 4); /*0x628ac0*/
            if ( v4 == v7 - 1 ) /*0x628ac9*/
            {
              v6 = 4 * (v4 / 4); /*0x628ad0*/
            }
            else if ( v4 % 4 == 3 ) /*0x628adc*/
            {
              v6 = v4 - 3; /*0x628ade*/
            }
            else
            {
              v6 = v4 + 1; /*0x628ae5*/
            }
            break;
          case '(': /*0x628a5c*/
            if ( v4 < 0 ) /*0x628a7e*/
              v4 = 0; /*0x628a80*/
            v5 = *(*(this + 55) + 28); /*0x628a8b*/
            if ( v5 != 0 ) /*0x628a8f*/
              v5 = *(v5 - 4); /*0x628a91*/
            if ( (v5 - v4) > 4 ) /*0x628a98*/
              v6 = v4 + 4; /*0x628aaa*/
            else
              v6 = v4 % -4; /*0x628aa2*/
            break;
          default:
            CDialog::OnKey(this, a2: wParam, a3: lParam); /*0x628a72*/
            return; /*0x628a77*/
        }
        goto LABEL_58; /*0x628aa5*/
      }
      if ( v4 < 0 ) /*0x628b3c*/
        v4 = 0; /*0x628b3e*/
      if ( v4 % 4 != 0 ) /*0x628b47*/
        v6 = v4 - 1; /*0x628b4e*/
      else
        v6 = v4 + 3; /*0x628b49*/
      v13 = *(this + 55); /*0x628b4f*/
      v14 = *(v13 + 28); /*0x628b55*/
      if ( v14 == 0 || v6 >= *(v14 - 4) ) /*0x628b5f*/
      {
        v15 = *(v13 + 28); /*0x628b61*/
LABEL_43:
        if ( v15 != 0 ) /*0x628b66*/
          v15 = *(v15 - 4); /*0x628b68*/
        v6 = v15 - 1; /*0x628b6b*/
        goto LABEL_58; /*0x628b6e*/
      }
      goto LABEL_58; /*0x628b5f*/
    }
    if ( v4 < 0 ) /*0x628b7f*/
      v4 = 0; /*0x628b81*/
    CUILoginStart::SendViewAllCharacterPacket(this: this - 1, a2: v4); /*0x628b87*/
  }
}
```

## CPinCodeDlg::OnCreate @ 0x61dbbb

```c
void __thiscall CPinCodeDlg::OnCreate(CPinCodeDlg *this, void *a2)
{
  _DWORD *Instance; // eax
  char **String; // eax
  int v5; // eax
  int v6; // ecx
  int v7; // eax
  int *v8; // eax
  int v9; // eax
  int v10; // eax
  int v11; // eax
  int v12; // eax
  int v13; // eax
  int v14; // eax
  int v15; // eax
  _DWORD *v16; // eax
  int v17; // eax
  CCtrlEdit *v18; // eax
  _DWORD *v19; // eax
  _DWORD *v20; // eax
  int v21; // eax
  CCtrlEdit *v22; // eax
  _DWORD *v23; // eax
  _DWORD *v24; // eax
  int v25; // eax
  CCtrlEdit *v26; // eax
  _DWORD *v27; // eax
  CCtrlEdit *v28; // eax
  int v29; // eax
  int Hi; // eax
  _DWORD *v31; // eax
  int v32; // eax
  CCtrlEdit *v33; // eax
  _DWORD *v34; // eax
  _DWORD *v35; // eax
  int v36; // eax
  CCtrlEdit *v37; // eax
  _DWORD *v38; // eax
  _DWORD *v39; // eax
  int v40; // eax
  CCtrlEdit *v41; // eax
  _DWORD *v42; // eax
  CCtrlEdit *v43; // eax
  int v44; // eax
  _DWORD *v45; // eax
  int StringW; // eax
  CCtrlEdit *v47; // eax
  _DWORD *v48; // eax
  _DWORD *v49; // eax
  int v50; // eax
  CCtrlEdit *v51; // eax
  _DWORD *v52; // eax
  int v53; // [esp-14h] [ebp-BCh] BYREF
  VARIANTARG *v54; // [esp-10h] [ebp-B8h]
  VARIANTARG *p_pvargDest; // [esp-Ch] [ebp-B4h]
  char *p_Unknown; // [esp-8h] [ebp-B0h]
  int v57; // [esp-4h] [ebp-ACh]
  VARIANTARG pvarg; // [esp+Ch] [ebp-9Ch] BYREF
  VARIANTARG pvargDest; // [esp+1Ch] [ebp-8Ch] BYREF
  _BYTE v60[28]; // [esp+2Ch] [ebp-7Ch] BYREF
  int v61; // [esp+48h] [ebp-60h]
  int v62; // [esp+50h] [ebp-58h]
  int v63; // [esp+54h] [ebp-54h]
  int v64; // [esp+5Ch] [ebp-4Ch]
  char v65[4]; // [esp+64h] [ebp-44h] BYREF
  char v66[4]; // [esp+68h] [ebp-40h] BYREF
  char v67[4]; // [esp+6Ch] [ebp-3Ch] BYREF
  char v68[4]; // [esp+70h] [ebp-38h] BYREF
  char v69[4]; // [esp+74h] [ebp-34h] BYREF
  char v70[4]; // [esp+78h] [ebp-30h] BYREF
  int *v71; // [esp+7Ch] [ebp-2Ch] BYREF
  int v72; // [esp+80h] [ebp-28h] BYREF
  int Unknown; // [esp+84h] [ebp-24h] BYREF
  VARIANTARG v74; // [esp+88h] [ebp-20h] BYREF
  CCtrlEdit *v75; // [esp+98h] [ebp-10h]
  int v76; // [esp+A4h] [ebp-4h]

  v72 = 0; /*0x61dbd2*/
  v76 = 0; /*0x61dbde*/
  Instance = StringPool::GetInstance(); /*0x61dbe1*/
  String = StringPool::GetString(this: Instance, result: &Unknown, nIdx: SP_3053_UI_LOGINIMG_PINCODE_TEXT_D); /*0x61dbe8*/
  v57 = *(this + 56); /*0x61dbed*/
  p_Unknown = *String; /*0x61dbf5*/
  LOBYTE(v76) = 1; /*0x61dbfa*/
  ZXString<char>::Format(a1: &v72, a2: p_Unknown, a3: v57); /*0x61dbfe*/
  LOBYTE(v76) = 0; /*0x61dc03*/
  ZXString<char>::_Release(a1: &Unknown); /*0x61dc0d*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x61dc1e*/
  LOBYTE(v76) = 2; /*0x61dc27*/
  sub_402F85(pvargDest: &v74, pvargSrc: &pvargSrc); /*0x61dc2b*/
  v57 = 0; /*0x61dc30*/
  p_Unknown = 0; /*0x61dc31*/
  p_pvargDest = &pvargDest; /*0x61dc38*/
  v54 = &v74; /*0x61dc3c*/
  LOBYTE(v76) = 3; /*0x61dc40*/
  v5 = sub_428226(a1: &v72); /*0x61dc44*/
  v53 = v6; /*0x61dc49*/
  v71 = &v53; /*0x61dc4c*/
  _bstr_t::_bstr_t(a1: v5); /*0x61dc50*/
  LOBYTE(v76) = 3; /*0x61dc5c*/
  v7 = sub_406472(); /*0x61dc60*/
  v8 = sub_402680(a1: v7); /*0x61dc67*/
  IWzResMan::GetObjectA(this: v8, pvargDest: &pvarg, sUOL: v53, vParam: v54, vAux: p_pvargDest); /*0x61dc6e*/
  LOBYTE(v76) = 5; /*0x61dc75*/
  Unknown = Ztl_variant_t::GetUnknown(a1: p_Unknown, a2: v57); /*0x61dc7e*/
  v9 = sub_41E527(a1: &Unknown); /*0x61dc8b*/
  if ( v9 < 0 && v9 != -2147467262 ) /*0x61dc99*/
    _com_issue_error(a1: v9); /*0x61dc9c*/
  LOBYTE(v76) = 3; /*0x61dca8*/
  v10 = ZComAPI::ZComVariantCopy(&pvarg); /*0x61dcac*/
  if ( v10 < 0 ) /*0x61dcb4*/
    _com_issue_error(a1: v10); /*0x61dcb7*/
  LOBYTE(v76) = 2; /*0x61dcc0*/
  v11 = ZComAPI::ZComVariantCopy(pvarg: &v74); /*0x61dcc4*/
  if ( v11 < 0 ) /*0x61dccc*/
    _com_issue_error(a1: v11); /*0x61dccf*/
  LOBYTE(v76) = 0; /*0x61dcd4*/
  v12 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x61dcdf*/
  if ( v12 < 0 ) /*0x61dce7*/
    _com_issue_error(a1: v12); /*0x61dcea*/
  v13 = *(this + 55); /*0x61dcef*/
  if ( v13 == 0 ) /*0x61dcf7*/
  {
    *&v74.vt = 1; /*0x61e198*/
    v74.cyVal.int64 = 0; /*0x61e1a2*/
    v57 = 3057; /*0x61e1a8*/
    p_Unknown = v67; /*0x61e1ad*/
    LOBYTE(v76) = 6; /*0x61e1ae*/
    v45 = StringPool::GetInstance(); /*0x61e1b2*/
    StringW = StringPool::GetStringW(this: v45, a2: p_Unknown, a3: v57); /*0x61e1b9*/
    LOBYTE(v76) = 7; /*0x61e1c2*/
    ZXString<unsigned short>::operator=(a1: StringW); /*0x61e1c6*/
    LOBYTE(v76) = 6; /*0x61e1ce*/
    ZXString<unsigned short>::_Release(a1: v67); /*0x61e1d2*/
    v47 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x61e1e3*/
    v75 = v47; /*0x61e1e8*/
    LOBYTE(v76) = 8; /*0x61e1ed*/
    if ( v47 != 0 ) /*0x61e1f1*/
      v48 = CCtrlButton::CCtrlButton(this: v47); /*0x61e1f5*/
    else
      v48 = 0; /*0x61e1fc*/
    LOBYTE(v76) = 6; /*0x61e205*/
    sub_4284FF(a1: v48); /*0x61e209*/
    (*(**(this + 59) + 32))(a1: *(this + 59), a2: this, a3: 1000, a4: 196, a5: 150, a6: 0, a7: &v74); /*0x61e22c*/
    v57 = 3058; /*0x61e232*/
    p_Unknown = &v71; /*0x61e237*/
    v49 = StringPool::GetInstance(); /*0x61e238*/
    v50 = StringPool::GetStringW(this: v49, a2: p_Unknown, a3: v57); /*0x61e23f*/
    LOBYTE(v76) = 9; /*0x61e248*/
    ZXString<unsigned short>::operator=(a1: v50); /*0x61e24c*/
    LOBYTE(v76) = 6; /*0x61e254*/
    ZXString<unsigned short>::_Release(a1: &v71); /*0x61e258*/
    v51 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x61e264*/
    v75 = v51; /*0x61e269*/
    LOBYTE(v76) = 10; /*0x61e26e*/
    if ( v51 != 0 ) /*0x61e272*/
      v52 = CCtrlButton::CCtrlButton(this: v51); /*0x61e276*/
    else
      v52 = 0; /*0x61e27d*/
    LOBYTE(v76) = 6; /*0x61e286*/
    sub_4284FF(a1: v52); /*0x61e28a*/
    (*(**(this + 61) + 32))(a1: *(this + 61), a2: this, a3: 1001, a4: 107, a5: 150, a6: 0, a7: &v74); /*0x61e2a6*/
    LOBYTE(v76) = 0; /*0x61e2a9*/
    Hi = v74.cyVal.Hi; /*0x61e2ad*/
    goto LABEL_49; /*0x61e2ad*/
  }
  v14 = v13 - 1; /*0x61dcfd*/
  if ( v14 == 0 ) /*0x61dcfe*/
  {
    *&v74.vt = 1; /*0x61dfe9*/
    v74.cyVal.int64 = 0; /*0x61dff3*/
    v57 = 3057; /*0x61dff9*/
    p_Unknown = v68; /*0x61dffe*/
    LOBYTE(v76) = 20; /*0x61dfff*/
    v35 = StringPool::GetInstance(); /*0x61e003*/
    v36 = StringPool::GetStringW(this: v35, a2: p_Unknown, a3: v57); /*0x61e00a*/
    LOBYTE(v76) = 21; /*0x61e013*/
    ZXString<unsigned short>::operator=(a1: v36); /*0x61e017*/
    LOBYTE(v76) = 20; /*0x61e01f*/
    ZXString<unsigned short>::_Release(a1: v68); /*0x61e023*/
    v37 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x61e034*/
    v75 = v37; /*0x61e039*/
    LOBYTE(v76) = 22; /*0x61e03e*/
    if ( v37 != 0 ) /*0x61e042*/
      v38 = CCtrlButton::CCtrlButton(this: v37); /*0x61e046*/
    else
      v38 = 0; /*0x61e04d*/
    LOBYTE(v76) = 20; /*0x61e056*/
    sub_4284FF(a1: v38); /*0x61e05a*/
    (*(**(this + 59) + 32))(a1: *(this + 59), a2: this, a3: 1000, a4: 107, a5: 150, a6: 0, a7: &v74); /*0x61e07a*/
    v57 = 3058; /*0x61e080*/
    p_Unknown = v70; /*0x61e085*/
    v39 = StringPool::GetInstance(); /*0x61e086*/
    v40 = StringPool::GetStringW(this: v39, a2: p_Unknown, a3: v57); /*0x61e08d*/
    LOBYTE(v76) = 23; /*0x61e096*/
    ZXString<unsigned short>::operator=(a1: v40); /*0x61e09a*/
    LOBYTE(v76) = 20; /*0x61e0a2*/
    ZXString<unsigned short>::_Release(a1: v70); /*0x61e0a6*/
    v41 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x61e0b2*/
    v75 = v41; /*0x61e0b7*/
    LOBYTE(v76) = 24; /*0x61e0bc*/
    if ( v41 != 0 ) /*0x61e0c0*/
      v42 = CCtrlButton::CCtrlButton(this: v41); /*0x61e0c4*/
    else
      v42 = 0; /*0x61e0cb*/
    LOBYTE(v76) = 20; /*0x61e0d4*/
    sub_4284FF(a1: v42); /*0x61e0d8*/
    (*(**(this + 61) + 32))(a1: *(this + 61), a2: this, a3: 1001, a4: 196, a5: 150, a6: 0, a7: &v74); /*0x61e0f7*/
    CCtrlEdit::CREATEPARAM::CREATEPARAM(a1: v60); /*0x61e0fd*/
    v62 = 0; /*0x61e102*/
    LOBYTE(v76) = 25; /*0x61e10d*/
    v61 = -16777216; /*0x61e111*/
    v64 = 8; /*0x61e118*/
    v63 = 1; /*0x61e11f*/
    v43 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 184); /*0x61e126*/
    v75 = v43; /*0x61e12b*/
    LOBYTE(v76) = 26; /*0x61e130*/
    if ( v43 != 0 ) /*0x61e134*/
      v44 = CCtrlEdit::CCtrlEdit(this: v43); /*0x61e138*/
    else
      v44 = 0; /*0x61e13f*/
    LOBYTE(v76) = 25; /*0x61e148*/
    sub_473956(a1: v44); /*0x61e14c*/
    (*(**(this + 65) + 8))(a1: *(this + 65), a2: this, a3: 1003, a4: 130, a5: 103, a6: 150, a7: 15, a8: v60); /*0x61e16d*/
    CWnd::SetFocusChild(this, a2: *(this + 65)); /*0x61e178*/
    LOBYTE(v76) = 20; /*0x61e180*/
    CCtrlEdit::CREATEPARAM::~CREATEPARAM(this: v60); /*0x61e184*/
    LOBYTE(v76) = 0; /*0x61e189*/
    goto LABEL_31; /*0x61e190*/
  }
  v15 = v14 - 1; /*0x61dd04*/
  if ( v15 == 0 ) /*0x61dd05*/
  {
    *&v74.vt = 1; /*0x61df41*/
    v74.cyVal.int64 = 0; /*0x61df4b*/
    v57 = 3057; /*0x61df51*/
    p_Unknown = v69; /*0x61df56*/
    LOBYTE(v76) = 27; /*0x61df57*/
    v31 = StringPool::GetInstance(); /*0x61df5b*/
    v32 = StringPool::GetStringW(this: v31, a2: p_Unknown, a3: v57); /*0x61df62*/
    LOBYTE(v76) = 28; /*0x61df6b*/
    ZXString<unsigned short>::operator=(a1: v32); /*0x61df6f*/
    LOBYTE(v76) = 27; /*0x61df77*/
    ZXString<unsigned short>::_Release(a1: v69); /*0x61df7b*/
    v33 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x61df8a*/
    v75 = v33; /*0x61df8f*/
    LOBYTE(v76) = 29; /*0x61df94*/
    if ( v33 != 0 ) /*0x61df98*/
      v34 = CCtrlButton::CCtrlButton(this: v33); /*0x61df9c*/
    else
      v34 = 0; /*0x61dfa3*/
    LOBYTE(v76) = 27; /*0x61dfac*/
    sub_4284FF(a1: v34); /*0x61dfb0*/
    (*(**(this + 59) + 32))(a1: *(this + 59), a2: this, a3: 1000, a4: 162, a5: 150, a6: 0, a7: &v74); /*0x61dfd2*/
    LOBYTE(v76) = 0; /*0x61dfd5*/
LABEL_31:
    ZXString<unsigned short>::_Release(a1: &v74.decVal.Lo64 + 4); /*0x61dfdc*/
    goto LABEL_51; /*0x61dfe1*/
  }
  if ( v15 != 1 ) /*0x61dd0c*/
    goto LABEL_51; /*0x61dd0c*/
  *&v74.vt = 1; /*0x61dd15*/
  v74.cyVal.int64 = 0; /*0x61dd1f*/
  v57 = 3060; /*0x61dd25*/
  p_Unknown = &Unknown; /*0x61dd2a*/
  LOBYTE(v76) = 11; /*0x61dd2b*/
  v16 = StringPool::GetInstance(); /*0x61dd2f*/
  v17 = StringPool::GetStringW(this: v16, a2: p_Unknown, a3: v57); /*0x61dd36*/
  LOBYTE(v76) = 12; /*0x61dd3f*/
  ZXString<unsigned short>::operator=(a1: v17); /*0x61dd43*/
  LOBYTE(v76) = 11; /*0x61dd4b*/
  ZXString<unsigned short>::_Release(a1: &Unknown); /*0x61dd4f*/
  v18 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x61dd60*/
  v75 = v18; /*0x61dd65*/
  LOBYTE(v76) = 13; /*0x61dd6a*/
  if ( v18 != 0 ) /*0x61dd6e*/
    v19 = CCtrlButton::CCtrlButton(this: v18); /*0x61dd72*/
  else
    v19 = 0; /*0x61dd79*/
  LOBYTE(v76) = 11; /*0x61dd82*/
  sub_4284FF(a1: v19); /*0x61dd86*/
  (*(**(this + 59) + 32))(a1: *(this + 59), a2: this, a3: 1000, a4: 132, a5: 150, a6: 0, a7: &v74); /*0x61dda9*/
  v57 = 3058; /*0x61ddaf*/
  p_Unknown = v65; /*0x61ddb4*/
  v20 = StringPool::GetInstance(); /*0x61ddb5*/
  v21 = StringPool::GetStringW(this: v20, a2: p_Unknown, a3: v57); /*0x61ddbc*/
  LOBYTE(v76) = 14; /*0x61ddc5*/
  ZXString<unsigned short>::operator=(a1: v21); /*0x61ddc9*/
  LOBYTE(v76) = 11; /*0x61ddd1*/
  ZXString<unsigned short>::_Release(a1: v65); /*0x61ddd5*/
  v22 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x61dde1*/
  v75 = v22; /*0x61dde6*/
  LOBYTE(v76) = 15; /*0x61ddeb*/
  if ( v22 != 0 ) /*0x61ddef*/
    v23 = CCtrlButton::CCtrlButton(this: v22); /*0x61ddf3*/
  else
    v23 = 0; /*0x61ddfa*/
  LOBYTE(v76) = 11; /*0x61de03*/
  sub_4284FF(a1: v23); /*0x61de07*/
  (*(**(this + 61) + 32))(a1: *(this + 61), a2: this, a3: 1001, a4: 212, a5: 150, a6: 0, a7: &v74); /*0x61de26*/
  v57 = 3059; /*0x61de2c*/
  p_Unknown = v66; /*0x61de31*/
  v24 = StringPool::GetInstance(); /*0x61de32*/
  v25 = StringPool::GetStringW(this: v24, a2: p_Unknown, a3: v57); /*0x61de39*/
  LOBYTE(v76) = 16; /*0x61de42*/
  ZXString<unsigned short>::operator=(a1: v25); /*0x61de46*/
  LOBYTE(v76) = 11; /*0x61de4e*/
  ZXString<unsigned short>::_Release(a1: v66); /*0x61de52*/
  v26 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444); /*0x61de5e*/
  v75 = v26; /*0x61de63*/
  LOBYTE(v76) = 17; /*0x61de68*/
  if ( v26 != 0 ) /*0x61de6c*/
    v27 = CCtrlButton::CCtrlButton(this: v26); /*0x61de70*/
  else
    v27 = 0; /*0x61de77*/
  LOBYTE(v76) = 11; /*0x61de80*/
  sub_4284FF(a1: v27); /*0x61de84*/
  (*(**(this + 61) + 32))(a1: *(this + 61), a2: this, a3: 1002, a4: 47, a5: 150, a6: 0, a7: &v74); /*0x61dea0*/
  CCtrlEdit::CREATEPARAM::CREATEPARAM(a1: v60); /*0x61dea6*/
  v62 = 0; /*0x61deab*/
  LOBYTE(v76) = 18; /*0x61deb6*/
  v61 = -16777216; /*0x61deba*/
  v64 = 8; /*0x61dec1*/
  v63 = 1; /*0x61dec8*/
  v28 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 184); /*0x61decf*/
  v75 = v28; /*0x61ded4*/
  LOBYTE(v76) = 19; /*0x61ded9*/
  if ( v28 != 0 ) /*0x61dedd*/
    v29 = CCtrlEdit::CCtrlEdit(this: v28); /*0x61dee1*/
  else
    v29 = 0; /*0x61dee8*/
  LOBYTE(v76) = 18; /*0x61def1*/
  sub_473956(a1: v29); /*0x61def5*/
  (*(**(this + 65) + 8))(a1: *(this + 65), a2: this, a3: 1003, a4: 130, a5: 103, a6: 150, a7: 15, a8: v60); /*0x61df16*/
  CWnd::SetFocusChild(this, a2: *(this + 65)); /*0x61df21*/
  LOBYTE(v76) = 11; /*0x61df29*/
  CCtrlEdit::CREATEPARAM::~CREATEPARAM(this: v60); /*0x61df2d*/
  LOBYTE(v76) = 0; /*0x61df32*/
  Hi = v74.cyVal.Hi; /*0x61df36*/
LABEL_49:
  if ( Hi != 0 ) /*0x61e2b2*/
    ZXString<unsigned short>::_Release(lpAddend: (Hi - 12)); /*0x61e2b8*/
LABEL_51:
  v76 = -1; /*0x61e2be*/
  ZXString<char>::_Release(a1: &v72); /*0x61e2c5*/
}
```

## CPinCodeDlg::OnButtonClicked @ 0x61e2db

```c
void __thiscall CPinCodeDlg::OnButtonClicked(CPinCodeDlg *this, unsigned int a2)
{
  const char **Text; // eax
  int v4; // edi
  int v5; // eax

  if ( a2 == 1000 ) /*0x61e2f0*/
  {
    if ( *(this + 55) == 1003 ) /*0x61e357*/
    {
      Text = CCtrlEdit::GetText(a1: &a2); /*0x61e364*/
      v4 = *this; /*0x61e36f*/
      v5 = atoi(a1: *Text); /*0x61e372*/
      (*(v4 + 52))(a1: this, a2: v5); /*0x61e37b*/
      ZXString<char>::_Release(a1: &a2); /*0x61e385*/
      return; /*0x61e385*/
    }
    goto LABEL_9; /*0x61e357*/
  }
  if ( a2 == 1001 ) /*0x61e2f3*/
  {
    if ( *(this + 55) != 1 || CPinCodeDlg::YesNo(a1: 7, a2: 0) != 0 ) /*0x61e31d*/
    {
      (*(*this + 52))(a1: this, a2: 0); /*0x61e34b*/
      return; /*0x61e34b*/
    }
    CCtrlEdit::SetText(a1: dword_AF78E0); /*0x61e32a*/
LABEL_9:
    (*(*this + 52))(a1: this, a2: 1); /*0x61e32f*/
    return; /*0x61e335*/
  }
  if ( a2 == 1002 && *(this + 55) == 3 ) /*0x61e2ff*/
    (*(*this + 52))(a1: this, a2: 2); /*0x61e305*/
}
```

## CLoginUtilDlg::DoModal @ 0x60f70b

```c
int __thiscall CLoginUtilDlg::DoModal(CLoginUtilDlg *this)
{
  int v2; // esi
  _BYTE v4[4]; // [esp+4h] [ebp-10h] BYREF
  int v5; // [esp+10h] [ebp-4h]

  CLoginUtilDlg::DoModal_::_2_::MODAL_OWNER::MODAL_OWNER(a1: this); /*0x60f71d*/
  v5 = 0; /*0x60f728*/
  CWvsApp::Run(a1: this + 54); /*0x60f733*/
  v2 = *(this + 53); /*0x60f738*/
  v5 = -1; /*0x60f73e*/
  CLoginUtilDlg::DoModal_::_2_::MODAL_OWNER::_MODAL_OWNER(a1: v4); /*0x60f745*/
  return v2; /*0x60f74a*/
}
```

## CLogo::Init @ 0x62edda

```c
void __thiscall CLogo::Init(CLogo *this, void *a2)
{
  int v2; // eax
  int CmdLine; // eax
  _BYTE v4[4]; // [esp+8h] [ebp-14h] BYREF
  _BYTE *v5; // [esp+Ch] [ebp-10h] BYREF
  int v6; // [esp+18h] [ebp-4h]

  CLogo::InitNXLogo(this); /*0x62edea*/
  CInputSystem::ShowCursor(this: dword_BEC33C, a2: 0); /*0x62edf8*/
  v5 = 0; /*0x62edfd*/
  v2 = *(TSingleton<CWvsApp>::ms_pInstance + 36); /*0x62ee06*/
  v6 = 0; /*0x62ee0c*/
  if ( v2 == 1 ) /*0x62ee0f*/
  {
    CmdLine = CWvsApp::GetCmdLine(a1: v4, a2: 5); /*0x62ee17*/
    LOBYTE(v6) = 1; /*0x62ee20*/
  }
  else
  {
    CmdLine = CWvsApp::GetCmdLine(a1: v4, a2: 3); /*0x62ee37*/
    LOBYTE(v6) = 2; /*0x62ee40*/
  }
  ZXString<char>::operator=(a1: CmdLine); /*0x62ee24*/
  LOBYTE(v6) = 0; /*0x62ee29*/
  ZXString<char>::_Release(a1: v4); /*0x62ee4f*/
  if ( v5 != 0 && *v5 != 0 && *(TSingleton<CWvsApp>::ms_pInstance + 40) != 0 ) /*0x62ee67*/
    CLogo::LogoEnd(); /*0x62ee6b*/
  v6 = -1; /*0x62ee70*/
  ZXString<char>::_Release(a1: &v5); /*0x62ee77*/
}
```
