// Angel 083 `OnHit_CMob` @ 0x668b83
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
void __userpurge CMob::OnHit(
        CMob *this@<ecx>,
        unsigned int a2,
        int *a3,
        int a4,
        int a5,
        int a6,
        int a7,
        int a8,
        int a9,
        int a10,
        int a11,
        int a12,
        int a13,
        int a14,
        int a15)
{
  int *v15; // esi
  const struct CharacterData *v16; // ebx
  CSkillInfo *v17; // edi
  int v18; // eax
  int SkillLevel; // edi
  int weapon_type; // eax
  CMob *v21; // edi
  int v22; // edx
  int v23; // ecx
  BOOL v24; // esi
  int v25; // eax
  int *v26; // eax
  int v27; // eax
  int *v28; // esi
  _DWORD *v29; // eax
  wchar_t *v30; // eax
  int v31; // eax
  int v32; // eax
  wchar_t *v33; // eax
  int v34; // ecx
  int v35; // eax
  int *v36; // eax
  int v37; // eax
  int v38; // eax
  VARIANTARG *v39; // eax
  _DWORD *Instance; // eax
  wchar_t *v41; // eax
  int v42; // eax
  int v43; // eax
  wchar_t *v44; // eax
  int v45; // ecx
  int v46; // eax
  int *v47; // eax
  int v48; // eax
  int v49; // eax
  int v50; // eax
  int v51; // eax
  char v52; // al
  int v53; // [esp-Ch] [ebp-70h] BYREF
  VARIANTARG *p_pvarg; // [esp-8h] [ebp-6Ch]
  VARIANTARG *v55; // [esp-4h] [ebp-68h]
  int v56; // [esp+0h] [ebp-64h]
  int v57; // [esp+4h] [ebp-60h]
  VARIANTARG v58; // [esp+Ch] [ebp-58h] BYREF
  VARIANTARG v59; // [esp+1Ch] [ebp-48h] BYREF
  VARIANTARG pvarg; // [esp+2Ch] [ebp-38h] BYREF
  VARIANTARG pvargDest; // [esp+3Ch] [ebp-28h] BYREF
  const struct SKILLENTRY *v62; // [esp+4Ch] [ebp-18h] BYREF
  CMob *v63; // [esp+50h] [ebp-14h]
  wchar_t *v64; // [esp+54h] [ebp-10h] BYREF
  int v65; // [esp+60h] [ebp-4h]

  v15 = TSingleton<CUserLocal>::ms_pInstance;
  v63 = this;
  v16 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &pvargDest.lVal) + 4);
  sub_4289B7(this: &pvargDest.lVal);
  v62 = 0;
  v17 = dword_BE78DC;
  v55 = &v62;
  v18 = (*(*v15 + 64))(a1: v15);
  SkillLevel = CSkillInfo::GetSkillLevel(this: v17, a2: v16, a3: v18 != 2000 ? 21000000 : 20000017, a4: &v62);
  weapon_type = get_weapon_type(nItemID: v15[315]);
  if ( a2 == v15[1130] && SkillLevel > 0 && weapon_type == 44 && a6 > 0 )
    sub_9602F3(a1: v15);
  v21 = v63;
  v63 = ((a10 == 10 || *(v63 + 205) != 0 && *(*(v63 + 70) != 0 ? *(v63 + 70) - 12 + 0x110 : 272) == 0)
      && CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v16, a3: 21110003, a4: 0) != 0
      && sub_66918F(a1: v21, a2: v22) == 0
      && (sub_6691A4(a1: v21) == 1 || sub_6691A4(a1: v21) == 2)
      && *(get_field() + 344) == 0);
  if ( sub_6691B9(a1: v21) != 0 )
  {
    if ( sub_76662D(a1: a3) != 0 )
      CMob::ShowHitEffect(this: v21, a2: a6);
  }
  else if ( a2 == v15[1130]
         && (_ZtlSecureFuse<long>(a1: v21 + 304, a2: *(v21 + 78)) == -2
          || _ZtlSecureFuse<long>(a1: v21 + 304, a2: *(v21 + 78)) > 0)
         && a10 != 0
         && (*(v21 + 205) == 0 && *(v21 + 329) == 0
          || *(*(v21 + 70) != 0 ? *(v21 + 70) - 12 + 0x110 : 272) != 0
          || v63 != 0)
         && (_ZtlSecureFuse<long>(a1: v21 + 972, a2: *(v21 + 245)) == 0
          || a3 == (&loc_4E23ED + 2)
          || a3 == &byte_E6935C
          || a3 == &word_D72A0E
          || (v23 = (*(*v15 + 64))(a1: v15)) / 100 == 21
          || v23 == 2000) )
  {
    CMob::GenerateMovePath(a1: v21, a2: a4, a3: a5 == 0, a4: 0, a5: a9, a6: a10, a7: a12, a8: a13, a9: a14, a10: v63);
  }
  if ( a3 != 1121001 && a3 != 1221001 && a3 != 1321001 )
  {
    if ( a3 == 3221007 )
      a7 = 1;
    if ( a3 != 1221011 && a3 != (&loc_4FAA8F + 2) && a3 != 21120006 && a6 >= 0 )
      CMob::ShowDamage(this: v21, nDamage: a6, a3: a8, a4: a7, bHalfHeight: a3 == (&loc_4E23EA + 2), a6: v56, a7: v57);
    goto LABEL_45;
  }
  v24 = a10 == 3;
  sub_66926E(a1: v24);
  if ( v24 )
LABEL_45:
    sub_66B562(
      a1: a9,
      a2: TSingleton<CUserLocal>::ms_pInstance != 0 ? TSingleton<CUserLocal>::ms_pInstance + 4 : 0,
      a3: 0);
  v64 = 0;
  v25 = *(v21 + 1);
  v65 = 0;
  v55 = *((*(v25 + 16))(a1: v21 + 1) + 4);
  v26 = (*(*(v21 + 1) + 16))(a1: v21 + 1);
  v27 = sub_9894F3(a1: *v26, a2: v55);
  v28 = a3;
  a10 = v27;
  if ( a11 <= 0 || a3 == 4111005 || a3 == &word_D7511A )
  {
    if ( a3 == 0 )
      goto LABEL_87;
    Instance = StringPool::GetInstance();
    v41 = *StringPool::GetStringW(this: Instance, a2: &a3, a3: SP_1591_SOUND_SKILLIMG_07D_HIT);
    LOBYTE(v65) = 8;
    sub_416821(a1: &v64, a2: v41, a3: v28);
    LOBYTE(v65) = 0;
    ZXString<unsigned short>::_Release(a1: &a3);
    if ( v28 == 3101005 )
    {
      if ( a8 <= 0 && a6 != 0 )
        goto LABEL_74;
    }
    else if ( v28 != (&loc_4E23ED + 2) && v28 != (&loc_4E23EA + 2) && v28 != &byte_E6935C && v28 != 22171002 || a8 <= 0 )
    {
      goto LABEL_74;
    }
    ZXString<unsigned short>::Assign<char>(a1: &sStrDefault, a2: -1);
LABEL_74:
    VariantInit(pvarg: &v59);
    v42 = ZtlTaskMemFreeImp(pvargDest: &v59, pvargSrc: &pvargSrc);
    if ( v42 < 0 )
      _com_issue_error(a1: v42);
    LOBYTE(v65) = 9;
    VariantInit(&pvarg);
    v43 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);
    if ( v43 < 0 )
      _com_issue_error(a1: v43);
    v55 = &v59;
    p_pvarg = &pvarg;
    LOBYTE(v65) = 10;
    v44 = sub_41468C(a1: &v64);
    v53 = v45;
    a3 = &v53;
    _bstr_t::_bstr_t(a1: v44);
    LOBYTE(v65) = 10;
    v46 = sub_406472();
    v47 = sub_402680(a1: v46);
    IWzResMan::GetObjectA(this: v47, pvargDest: &v58, sUOL: v53, vParam: p_pvarg, vAux: v55);
    LOBYTE(v65) = 14;
    v48 = ZComAPI::ZComVariantCopy(&pvarg);
    if ( v48 < 0 )
      _com_issue_error(a1: v48);
    LOBYTE(v65) = 13;
    v49 = ZComAPI::ZComVariantCopy(pvarg: &v59);
    if ( v49 < 0 )
      _com_issue_error(a1: v49);
    if ( v58.vt == 0 )
      ZXString<unsigned short>::Assign<char>(a1: &sStrDefault, a2: -1);
    LOBYTE(v65) = 0;
    v39 = &v58;
    goto LABEL_85;
  }
  v29 = StringPool::GetInstance();
  v30 = *StringPool::GetStringW(this: v29, a2: &a3, a3: SP_3306_SOUND_CASHEFFECTIMG_08D_HIT);
  LOBYTE(v65) = 1;
  sub_416821(a1: &v64, a2: v30, a3: a11);
  LOBYTE(v65) = 0;
  ZXString<unsigned short>::_Release(a1: &a3);
  if ( a8 > 0 || a6 == 0 )
    ZXString<unsigned short>::Assign<char>(a1: &sStrDefault, a2: -1);
  VariantInit(&pvarg);
  v31 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);
  if ( v31 < 0 )
    _com_issue_error(a1: v31);
  LOBYTE(v65) = 2;
  VariantInit(pvarg: &pvargDest);
  v32 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);
  if ( v32 < 0 )
    _com_issue_error(a1: v32);
  v55 = &pvarg;
  p_pvarg = &pvargDest;
  LOBYTE(v65) = 3;
  v33 = sub_41468C(a1: &v64);
  v53 = v34;
  a3 = &v53;
  _bstr_t::_bstr_t(a1: v33);
  LOBYTE(v65) = 3;
  v35 = sub_406472();
  v36 = sub_402680(a1: v35);
  IWzResMan::GetObjectA(this: v36, pvargDest: &v59, sUOL: v53, vParam: p_pvarg, vAux: v55);
  LOBYTE(v65) = 7;
  v37 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest);
  if ( v37 < 0 )
    _com_issue_error(a1: v37);
  LOBYTE(v65) = 6;
  v38 = ZComAPI::ZComVariantCopy(&pvarg);
  if ( v38 < 0 )
    _com_issue_error(a1: v38);
  if ( v59.vt == 0 )
    ZXString<unsigned short>::Assign<char>(a1: &sStrDefault, a2: -1);
  LOBYTE(v65) = 0;
  v39 = &v59;
LABEL_85:
  v50 = ZComAPI::ZComVariantCopy(pvarg: v39);
  if ( v50 < 0 )
    _com_issue_error(a1: v50);
LABEL_87:
  if ( v64 != 0 && *v64 != 0 )
  {
    CSoundMan::PlaySE(a1: v64, a2: a10, a3: 0);
  }
  else
  {
    if ( *(v21 + 169) == 0 || (v51 = *(v21 + 99)) == 0 )
      v51 = *(v21 + 98);
    v52 = _ZtlSecureFuse<unsigned long>(a1: v51 + 12, a2: *(v51 + 20));
    sub_989BA1(a1: v52, a2: 0, a3: a10);
  }
  v65 = -1;
  ZXString<unsigned short>::_Release(a1: &v64);
}