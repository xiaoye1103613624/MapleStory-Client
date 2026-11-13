// Angel 083 `Update_CMob` @ 0x6675a8
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
int __fastcall CMob::Update(CMob *this, int a2)
{
  int v3; // esi
  int v4; // edx
  int AttackInfo; // eax
  int v6; // eax
  bool v7; // zf
  int v8; // esi
  int v9; // eax
  int v10; // eax
  int v11; // eax
  int v12; // esi
  int v13; // ecx
  int v14; // eax
  int v15; // eax
  int **Alpha; // eax
  int *v17; // ecx
  int v18; // edx
  int v19; // eax
  int v20; // eax
  int v21; // eax
  int v22; // eax
  int v23; // esi
  int v24; // eax
  int v25; // esi
  CVecCtrl *v26; // esi
  int v27; // eax
  int v28; // eax
  int BodyAttackMob; // eax
  int v30; // esi
  _DWORD *v31; // eax
  int v32; // esi
  int v33; // esi
  int v34; // esi
  _DWORD *v35; // edx
  int v36; // eax
  int v37; // eax
  int v38; // eax
  _DWORD *Instance; // eax
  wchar_t **StringW; // eax
  wchar_t *v41; // ecx
  int v42; // ecx
  int v43; // edx
  int v44; // ecx
  char *v45; // eax
  int v46; // eax
  int v47; // eax
  int v48; // eax
  _DWORD *v49; // eax
  int v50; // edx
  int v51; // eax
  int v52; // eax
  int v53; // eax
  CMob *v54; // esi
  int v55; // eax
  _DWORD *v56; // eax
  int v57; // eax
  int v58; // eax
  _DWORD *v59; // eax
  int v60; // eax
  int v61; // ecx
  int v62; // eax
  int v63; // esi
  int v64; // ecx
  int v65; // eax
  DWORD Time; // eax
  int v67; // eax
  int v68; // eax
  int v69; // eax
  int v70; // eax
  DWORD v71; // eax
  int v72; // eax
  DWORD v73; // eax
  int v74; // eax
  DWORD v75; // eax
  int v76; // eax
  int v77; // eax
  unsigned int v78; // eax
  int v79; // eax
  int v80; // eax
  _DWORD *v81; // esi
  int v82; // ecx
  wchar_t **v83; // ecx
  char *v84; // eax
  wchar_t *v85; // eax
  void *v86; // esp
  wchar_t **v87; // ecx
  wchar_t *v88; // ecx
  wchar_t *v89; // eax
  wchar_t **v90; // ecx
  int v91; // eax
  int v92; // eax
  int v93; // eax
  int v94; // esi
  int v95; // eax
  int updated; // esi
  int v97; // eax
  int v98; // eax
  int **v99; // eax
  int *v100; // edx
  int v101; // eax
  int v102; // eax
  int v103; // eax
  _DWORD *v104; // esi
  int v105; // eax
  int v106; // eax
  int v107; // esi
  int v108; // eax
  int *v109; // eax
  int *v110; // ecx
  int v111; // eax
  int v112; // edi
  int *v113; // eax
  unsigned int v114; // eax
  int v115; // eax
  signed int v116; // esi
  int v117; // edi
  char v118; // al
  _DWORD *v119; // eax
  _DWORD *v120; // ecx
  int v121; // ecx
  int v122; // eax
  int v123; // eax
  int v124; // eax
  signed int v125; // esi
  char v126; // di
  char v127; // al
  int result; // eax
  _DWORD *v129; // [esp-88h] [ebp-158h] BYREF
  int v130; // [esp-84h] [ebp-154h]
  _DWORD *v131; // [esp-80h] [ebp-150h] BYREF
  _DWORD *v132; // [esp-7Ch] [ebp-14Ch]
  int v133; // [esp-78h] [ebp-148h]
  wchar_t **v134; // [esp-74h] [ebp-144h] BYREF
  int v135; // [esp-70h] [ebp-140h]
  int **v136; // [esp-6Ch] [ebp-13Ch]
  int **v137; // [esp-68h] [ebp-138h]
  int v138; // [esp-64h] [ebp-134h]
  BOOL v139; // [esp-60h] [ebp-130h] BYREF
  int v140; // [esp-5Ch] [ebp-12Ch]
  unsigned int v141; // [esp-58h] [ebp-128h]
  wchar_t **v142; // [esp-54h] [ebp-124h] BYREF
  int v143; // [esp-50h] [ebp-120h]
  int **v144; // [esp-4Ch] [ebp-11Ch]
  wchar_t *v145; // [esp-48h] [ebp-118h]
  int v146; // [esp-44h] [ebp-114h]
  int v147; // [esp-40h] [ebp-110h] BYREF
  int *v148; // [esp-3Ch] [ebp-10Ch]
  int *v149; // [esp-38h] [ebp-108h]
  wchar_t *v150; // [esp-34h] [ebp-104h] BYREF
  int v151; // [esp-30h] [ebp-100h]
  __int64 v152; // [esp-10h] [ebp-E0h]
  VARIANTARG *v153; // [esp-4h] [ebp-D4h]
  int v154; // [esp+0h] [ebp-D0h]
  int v155; // [esp+4h] [ebp-CCh]
  int v156[4]; // [esp+Ch] [ebp-C4h] BYREF
  _DWORD v157[4]; // [esp+1Ch] [ebp-B4h] BYREF
  _DWORD v158[4]; // [esp+2Ch] [ebp-A4h] BYREF
  VARIANTARG pvargDest; // [esp+3Ch] [ebp-94h] BYREF
  DWORD v160; // [esp+4Ch] [ebp-84h]
  DWORD v161; // [esp+50h] [ebp-80h]
  DWORD v162; // [esp+54h] [ebp-7Ch]
  int v163; // [esp+58h] [ebp-78h] BYREF
  int v164; // [esp+5Ch] [ebp-74h]
  int v165; // [esp+60h] [ebp-70h]
  int v166; // [esp+64h] [ebp-6Ch] BYREF
  int v167; // [esp+68h] [ebp-68h]
  int v168; // [esp+6Ch] [ebp-64h]
  int v169; // [esp+70h] [ebp-60h]
  int v170; // [esp+74h] [ebp-5Ch]
  int v171; // [esp+78h] [ebp-58h] BYREF
  int v172; // [esp+7Ch] [ebp-54h]
  wchar_t ***v173; // [esp+80h] [ebp-50h]
  int v174; // [esp+84h] [ebp-4Ch] BYREF
  int v175; // [esp+88h] [ebp-48h]
  int v176; // [esp+8Ch] [ebp-44h] BYREF
  CVecCtrl *v177; // [esp+90h] [ebp-40h]
  VARIANTARG pvarg; // [esp+94h] [ebp-3Ch] BYREF
  int v179; // [esp+A4h] [ebp-2Ch] BYREF
  int v180; // [esp+A8h] [ebp-28h] BYREF
  int v181; // [esp+ACh] [ebp-24h] BYREF
  wchar_t ***v182; // [esp+B0h] [ebp-20h] BYREF
  _DWORD *v183; // [esp+B4h] [ebp-1Ch] BYREF
  bool v184; // [esp+BBh] [ebp-15h]
  int update_time; // [esp+BCh] [ebp-14h]
  int i; // [esp+C0h] [ebp-10h] BYREF
  int v187; // [esp+CCh] [ebp-4h]

  update_time = get_update_time();
  CChatBalloon::CheckTimeOut(this: (this + 20), a2: 0);
  v3 = *(this + 57);
  while ( v3 != 0 )
  {
    v153 = *(v3 + 28);
    v4 = *(v3 != 0 ? v3 - 16 + 4 : 4);
    v3 = v4 != 0 ? v4 + 16 : 0;
    AttackInfo = CMobTemplate::GetAttackInfo(a1: v153);
    if ( *(AttackInfo + 160) != 0 )
    {
      *(this + 39) = *(AttackInfo + 140);
      sub_670A3F(a1: this);
    }
  }
  v6 = _ZtlSecureFuse<long>(a1: *(this + 98) + 432, a2: *(*(this + 98) + 440));
  v7 = *(this + 147) == 0;
  v8 = update_time;
  i = v6;
  if ( !v7 && update_time - *(this + 279) >= 1000 )
  {
    if ( sub_68146C(this: *(this + 98), a2: *(this + 148)) != 0 )
    {
      v9 = i;
      if ( i <= 0 )
        v9 = *(this + 147);
    }
    else
    {
      v9 = 0;
    }
    CMob::ShowDamage(this, nDamage: v9, a3: 0, a4: 0, bHalfHeight: 0, a6: v154, a7: v155);
    *(this + 279) = update_time;
  }
  if ( *(this + 192) != 0 && v8 - *(this + 280) >= 1000 )
  {
    if ( sub_68146C(this: *(this + 98), a2: *(this + 193)) != 0 )
    {
      v10 = i;
      if ( i <= 0 )
        v10 = *(this + 192);
    }
    else
    {
      v10 = 0;
    }
    CMob::ShowDamage(this, nDamage: v10, a3: 0, a4: 0, bHalfHeight: 0, a6: v154, a7: v155);
    *(this + 280) = v8;
  }
  if ( *(this + 188) != 0 && v8 - *(this + 281) >= 1000 )
  {
    if ( sub_68146C(this: *(this + 98), a2: *(this + 189)) != 0 )
    {
      v11 = i;
      if ( i <= 0 )
        v11 = *(this + 188);
    }
    else
    {
      v11 = 0;
    }
    CMob::ShowDamage(this, nDamage: v11, a3: 0, a4: 0, bHalfHeight: 0, a6: v154, a7: v155);
    *(this + 281) = v8;
  }
  v180 = *(this + 231);
  while ( v180 != 0 )
  {
    v12 = v180;
    v13 = *(v180 != 0 ? v180 - 16 + 4 : 4);
    v7 = *(v180 + 4) == 0;
    v180 = v13 != 0 ? v13 + 16 : 0;
    if ( !v7 && update_time - *(v12 + 12) >= 1000 )
    {
      v14 = sub_78C7FD(a1: v12);
      if ( sub_68146C(this: *(this + 98), a2: v14) != 0 )
      {
        v15 = i;
        if ( i <= 0 )
          v15 = *(v12 + 4);
      }
      else
      {
        v15 = 0;
      }
      CMob::ShowDamage(this, nDamage: v15, a3: 0, a4: 0, bHalfHeight: 0, a6: v154, a7: v155);
      *(v12 + 12) = update_time;
    }
  }
  if ( _ZtlSecureFuse<long>(a1: this + 1080, a2: *(this + 272)) != 0
    && update_time - _ZtlSecureFuse<long>(a1: this + 1080, a2: *(this + 272)) > 0 )
  {
    v153 = &pvargSrc;
    *(this + 272) = _ZtlSecureTear<long>(a1: 0, a2: this + 1080);
    sub_402F85(&pvargDest, pvargSrc: v153);
    v187 = 0;
    sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);
    v7 = *(this + 304) == 0;
    LOBYTE(v187) = 1;
    if ( v7 )
      _com_issue_error(a1: -2147467261);
    Alpha = IWzGr2DLayer::GetAlpha(a1: &v183);
    v7 = *Alpha == 0;
    LOBYTE(v187) = 2;
    if ( v7 )
      _com_issue_error(a1: -2147467261);
    v17 = *Alpha;
    v152 = *&pvargDest.vt;
    v18 = *v17;
    v179 = v17;
    v19 = (*(v18 + 144))(
            a1: v17,
            a2: 255,
            a3: 255,
            a4: *&pvarg.vt,
            a5: pvarg.decVal.Hi32,
            a6: pvarg.lVal,
            a7: pvarg.cyVal.Hi,
            a8: *&pvargDest.vt,
            a9: pvargDest.decVal.Hi32,
            a10: pvargDest.lVal,
            a11: pvargDest.cyVal.Hi);
    if ( v19 < 0 )
      _com_issue_errorex(a1: v19, a2: v179, a3: dword_BD8348);
    sub_41569E(a1: &v183);
    LOBYTE(v187) = 0;
    v20 = ZComAPI::ZComVariantCopy(&pvarg);
    if ( v20 < 0 )
      _com_issue_error(a1: v20);
    v187 = -1;
    v21 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest);
    if ( v21 < 0 )
      _com_issue_error(a1: v21);
    *(this + 278) = _ZtlSecureTear<int>(t: 0, at: this + 276);
  }
  if ( sub_670C06(a1: this) != 0 )
  {
    if ( _ZtlSecureFuse<long>(a1: this + 1080, a2: *(this + 272)) == 0
      || _ZtlSecureFuse<long>(a1: this + 1080, a2: *(this + 272)) != 0
      && (v22 = _ZtlSecureFuse<long>(a1: this + 1080, a2: *(this + 272)), update_time > v22 - 500) )
    {
      if ( *(this + 304) == 0 )
        _com_issue_error(a1: -2147467261);
      v23 = *(this + 304);
      v24 = (*(*v23 + 224))(a1: v23, a2: -2130706433);
      if ( v24 < 0 )
        _com_issue_errorex(a1: v24, a2: v23, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf);
    }
  }
  v25 = *(this + 70);
  v151 = *(this + 245);
  v26 = v25 != 0 ? (v25 - 12) : 0;
  v181 = *(this + 71) != 0 ? *(this + 71) - 12 : 0;
  v177 = v26;
  if ( _ZtlSecureFuse<long>(a1: this + 972, a2: v151) != 0 )
  {
    v27 = _ZtlSecureFuse<long>(a1: this + 972, a2: *(this + 245));
    if ( update_time - v27 >= 0 )
      *(this + 245) = _ZtlSecureTear<long>(a1: 0, a2: this + 972);
  }
  sub_66E146(a1: update_time);
  v151 = *(this + 278);
  v150 = (this + 1104);
  if ( timeGetTime() == 0 )
  {
    sub_66E2B1(a1: update_time);
    if ( sub_667569(a1: this) == 0 && *(this + 227) == 0 )
    {
      sub_66E32D(a1: this);
      sub_66E636(this);
    }
  }
  if ( _ZtlSecureFuse<long>(a1: this + 960, a2: *(this + 242)) <= -1 )
  {
    if ( *(this + 169) == 0 || (v28 = *(this + 99)) == 0 )
      v28 = *(this + 98);
    if ( (_ZtlSecureFuse<long>(a1: v28 + 64, a2: *(v28 + 72)) == 3 || *(v26 + 68) != 0) && *(v181 + 24) != 0 )
    {
      if ( sub_670C54(a1: this) != 0 )
      {
        sub_664559(this, a2: &pvargDest, a3: 1);
        if ( update_time - *(dword_BEBFA4 + 108) >= 2000 )
        {
          BodyAttackMob = CMobPool::FindBodyAttackMob(a1: &pvargDest, a2: 0);
          i = BodyAttackMob;
          if ( BodyAttackMob != 0 )
          {
            v30 = BodyAttackMob + 4;
            v183 = (*(*(BodyAttackMob + 4) + 20))(a1: BodyAttackMob + 4);
            v31 = (*(*v30 + 16))(a1: v30);
            v32 = *v31 - *v183;
            v33 = v32 - *(*(*(this + 1) + 16))(a1: this + 4);
            v34 = *(*(*(this + 1) + 20))(a1: this + 4) + v33;
            if ( _ZtlSecureFuse<CLadderOrRope *>(a1: v177 + 73, a2: *(v177 + 75)) != 0 )
            {
              v35 = (*(*(this + 1) + 20))(a1: this + 4);
              v36 = *(this + 1);
              v183 = v35;
              v37 = (*(v36 + 16))(a1: this + 4);
              if ( *(v37 + 4) - v183[1] < 0 )
                v34 = 0x7FFFFFFF;
            }
            sub_670C63(a1: 0, a2: 0, a3: 0, a4: i, a5: -1, a6: v34);
            sub_670C63(a1: 0, a2: 0, a3: 0, a4: this, a5: -1, a6: -v34);
            *(dword_BEBFA4 + 108) = update_time;
          }
        }
      }
      if ( _ZtlSecureFuse<long>(a1: this + 304, a2: *(this + 78)) == 4
        && (*(this + 205) == 0 && *(this + 329) == 0 || *(*(this + 70) != 0 ? *(this + 70) - 12 + 0x110 : 272) != 0) )
      {
        i = 0;
        if ( CMob::TryDoingSkill(this, a2: &v183, a3: &v180, a4: &i) != 0
          || *(v181 + 592) != 0 && CMob::IsTargetInAttackRange(this, a2: 3, a3: &v183, a4: &v180, a5: &i) != 0 )
        {
          v151 = 0;
          v150 = 0;
          v149 = 0;
          v148 = 0;
          v147 = 0;
          v146 = 1;
          v145 = i;
          v144 = v180;
          *(this + 93) = 0;
          CMob::GenerateMovePath(
            a1: this,
            a2: v183,
            a3: v144,
            a4: v145,
            a5: v146,
            a6: v147,
            a7: v148,
            a8: v149,
            a9: v150,
            a10: v151);
        }
      }
    }
  }
  v38 = *(this + 84);
  if ( v38 != 0 && v38 - update_time < 0 )
  {
    i = 0;
    v187 = 3;
    Instance = StringPool::GetInstance();
    StringW = StringPool::GetStringW(this: Instance, a2: &v183, a3: SP_1590_EFFECT_SUMMONIMG_D);
    v151 = *(this + 83);
    v150 = *StringW;
    LOBYTE(v187) = 4;
    sub_416821(a1: &i, a2: v150, a3: v151);
    LOBYTE(v187) = 3;
    ZXString<unsigned short>::_Release(a1: &v183);
    v151 = -1073343324;
    v150 = v41;
    v182 = &v150;
    sub_432C10(a1: 0);
    v149 = *(this + 327);
    LOBYTE(v187) = 5;
    v148 = *(this + 326);
    v147 = v42;
    pvarg.cyVal.Hi = &v147;
    sub_432BDD(a1: 0);
    LOBYTE(v187) = 3;
    v145 = sub_41468C(a1: &i);
    v45 = sub_432CBC(a1: v44, a2: v43);
    CAnimationDisplayer::Effect_General(this: v45, a2: v145, a3: 0, a4: v147, a5: v148, a6: v149, a7: v150, a8: v151);
    v46 = sub_9894F3(a1: *(this + 326), a2: *(this + 327));
    sub_98A2D4(a1: *(this + 83), a2: v46);
    v187 = -1;
    *(this + 84) = 0;
    ZXString<unsigned short>::_Release(a1: &i);
  }
  sub_664559(this, a2: v157, a3: 1);
  v47 = update_time - *(this + 282);
  v174 = 0;
  v166 = 0;
  v163 = 0;
  if ( v47 >= 1000 )
  {
    v151 = 1;
    v150 = &v166;
    v149 = &v171;
    v148 = &v163;
    v147 = &v174;
    get_field();
    if ( sub_642710(a1: v157, a2: v147, a3: v148, a4: v149, a5: v150, a6: v151) != 0 && v174 > 0 )
    {
      COutPacket::COutPacket(a1: 191);
      v151 = *(this + 97);
      v187 = 7;
      v48 = _ZtlSecureFuse<unsigned long>(a1: this + 380, a2: v151);
      COutPacket::Encode4(this: &pvarg, a2: v48);
      COutPacket::Encode4(this: &pvarg, a2: v174);
      CClientSocket::SendPacket(this: dword_BE7914, a2: &pvarg);
      if ( v171 == 0 && v172 == 0 )
      {
        v179 = (*(*(this + 1) + 20))(a1: this + 4);
        v49 = (*(*(this + 1) + 16))(a1: this + 4);
        v171 = *v49 - *v179;
        v50 = (*(*(this + 1) + 20))(a1: this + 4);
        v51 = *(this + 1);
        v179 = v50;
        v52 = (*(v51 + 16))(a1: this + 4);
        v172 = *(v52 + 4) - *(v179 + 4);
      }
      *(this + 282) = update_time;
      if ( *(*(this + 71) != 0 ? *(this + 71) - 12 + 0x18 : 24) != 0
        && (*(this + 205) == 0 && *(this + 329) == 0 || *(*(this + 70) != 0 ? *(this + 70) - 12 + 0x110 : 272) != 0) )
      {
        v151 = 0;
        v150 = 0;
        v149 = 0;
        v148 = 0;
        v147 = 1;
        v144 = (v171 < 0);
        v53 = sub_668AAD(a1: this);
        CMob::GenerateMovePath(
          a1: this,
          a2: v53,
          a3: v144,
          a4: 0,
          a5: 0,
          a6: v147,
          a7: v148,
          a8: v149,
          a9: v150,
          a10: v151);
      }
      v187 = -1;
      ZArray<unsigned char>::RemoveAll(a1: &pvarg.decVal.Hi32);
    }
  }
  v54 = *(dword_BEBFA4 + 92);
  if ( v54 != 0 )
  {
    v55 = _ZtlSecureFuse<unsigned long>(a1: *(this + 98) + 12, a2: *(*(this + 98) + 20));
    if ( sub_670BC0(a1: v55) != 0 )
    {
      i = *(dword_BEBFA4 + 96);
      if ( v54 != this )
      {
        if ( CMob::IsActive(this) != 0 )
        {
          v56 = sub_668ADD(this, a2: &v183);
          v184 = *(*v56 != 0 ? *v56 - 12 + 0x250 : 592) == 0;
          if ( v183 != 0 )
            (*(*v183 + 8))(a1: v183);
          if ( v184 )
            sub_66B562(a1: 1, a2: v54 + 4, a3: 0);
        }
        if ( (*(*dword_BE7918 + 11988) != 0 && *(*dword_BE7918 + 12166) == *(*dword_BE7918 + 8352)
           || *(dword_BEBFA8 + 44) == 0)
          && update_time - i >= 1500 )
        {
          sub_664559(this: v54, a2: v158, a3: 0);
          sub_664559(this, a2: &pvargDest, a3: 0);
          if ( dword_BF04A8(a1: v156, a2: v158, a3: &pvargDest) != 0 )
          {
            COutPacket::COutPacket(a1: 192);
            v147 = *(this + 97);
            v187 = 8;
            v57 = _ZtlSecureFuse<unsigned long>(a1: this + 380, a2: v147);
            COutPacket::Encode4(this: &pvarg, a2: v57);
            COutPacket::Encode4(this: &pvarg, a2: *(*dword_BE7918 + 8352));
            v58 = _ZtlSecureFuse<unsigned long>(a1: v54 + 380, a2: *(v54 + 97));
            COutPacket::Encode4(this: &pvarg, a2: v58);
            CClientSocket::SendPacket(this: dword_BE7914, a2: &pvarg);
            i = update_time;
            v59 = sub_668ADD(this: v54, a2: &v179);
            v184 = *(*v59 != 0 ? *v59 - 12 + 0x18 : 24) != 0
                && (*(v54 + 205) == 0 && *(v54 + 329) == 0 || *(*(v54 + 70) != 0 ? *(v54 + 70) - 12 + 0x110 : 272) != 0
                  ? (v60 = 0)
                  : (v60 = 1),
                    v60 == 0);
            if ( v179 != 0 )
            {
              v61 = *v179;
              v147 = v179;
              (*(v61 + 8))();
            }
            if ( v184 )
            {
              v139 = v171 < 0;
              v62 = sub_668AAD(a1: v54);
              CMob::GenerateMovePath(a1: v54, a2: v62, a3: v139, a4: 0, a5: 0, a6: 0, a7: 0, a8: 0, a9: 0, a10: 0);
            }
            v187 = -1;
            ZArray<unsigned char>::RemoveAll(a1: &pvarg.decVal.Hi32);
          }
        }
      }
      *(dword_BEBFA4 + 96) = i;
    }
  }
  v180 = *(this + 292);
  while ( v180 != 0 )
  {
    v63 = v180;
    v146 = *(v180 + 8);
    v64 = *(v180 != 0 ? v180 - 16 + 4 : 4);
    v145 = v180;
    v182 = v180;
    v180 = v64 != 0 ? v64 + 16 : 0;
    v65 = _ZtlSecureFuse<long>(a1: v145, a2: v146);
    if ( update_time - v65 >= 0 )
    {
      CMob::GetCurrentAction(this, a2: &v181);
      v146 = *(v63 + 164);
      v145 = (v63 + 156);
      Time = timeGetTime();
      v144 = *(v63 + 152);
      i = Time;
      v67 = _ZtlSecureFuse<long>(a1: v63 + 144, a2: v144);
      v142 = *(v63 + 140);
      v169 = v67;
      v68 = _ZtlSecureFuse<long>(a1: v63 + 132, a2: v142);
      v140 = *(v63 + 128);
      v168 = v68;
      v69 = _ZtlSecureFuse<long>(a1: v63 + 120, a2: v140);
      v138 = *(v63 + 116);
      v170 = v69;
      v70 = _ZtlSecureFuse<long>(a1: v63 + 108, a2: v138);
      v136 = *(v63 + 104);
      v164 = v70;
      v135 = v63 + 96;
      v71 = timeGetTime();
      v134 = *(v63 + 92);
      v160 = v71;
      v72 = _ZtlSecureFuse<long>(a1: v63 + 84, a2: v134);
      v132 = *(v63 + 80);
      v167 = v72;
      v131 = (v63 + 72);
      v73 = timeGetTime();
      v146 = *(v63 + 68);
      v161 = v73;
      v74 = _ZtlSecureFuse<long>(a1: v63 + 60, a2: v146);
      v144 = *(v63 + 56);
      v165 = v74;
      v143 = v63 + 48;
      v75 = timeGetTime();
      v142 = *(v63 + 44);
      v162 = v75;
      v76 = _ZtlSecureFuse<long>(a1: v63 + 36, a2: v142);
      v140 = *(v63 + 32);
      v173 = v76;
      pvarg.cyVal.Hi = v63 + 24;
      v77 = _ZtlSecureFuse<long>(a1: v63 + 24, a2: v140);
      v138 = *(v63 + 20);
      v175 = v77;
      v78 = _ZtlSecureFuse<unsigned long>(a1: v63 + 12, a2: v138);
      CMob::OnHit(
        this,
        a2: v78,
        a3: v175,
        a4: v173,
        a5: v162,
        a6: v165,
        a7: v161,
        a8: v167,
        a9: v160,
        a10: v164,
        a11: v170,
        a12: v168,
        a13: v169,
        a14: i,
        a15: v147);
      CMob::GetCurrentAction(this, a2: &v183);
      if ( _ZtlSecureFuse<long>(a1: pvarg.cyVal.Hi, a2: *(v63 + 32)) == 2201004 && v181 != v183 )
      {
        v79 = _ZtlSecureFuse<long>(a1: this + 948, a2: *(this + 239));
        *(this + 239) = _ZtlSecureTear<long>(a1: v79 ^ (v183 ^ (v79 ^ (v181 != v183))) & 1, a2: this + 948);
      }
      sub_673100(a1: v182);
    }
  }
  v80 = *(this + 297);
  while ( v80 != 0 )
  {
    v81 = v80;
    v175 = v80;
    v82 = *(v80 != 0 ? v80 - 16 + 4 : 4);
    v80 = v82 != 0 ? v82 + 16 : 0;
    v83 = (update_time - *v81);
    v180 = v80;
    if ( v83 >= 0 )
    {
      v84 = v81[1];
      if ( v84 == &loc_4E23ED + 2 || v84 == &byte_E6935C )
      {
        v181 = 0;
        v187 = 9;
        for ( i = 0; i < 3; ++i )
        {
          if ( i != 0 )
          {
            v86 = alloca(32);
            LOBYTE(v85) = _itow(a1: i - 1, a2: &v139, a3: 10);
          }
          else
          {
            v85 = &word_AF0E08;
          }
          v87 = v81[4];
          if ( v87 != 0 )
            v88 = *v87;
          else
            v88 = 0;
          sub_416821(a1: &v181, a2: v88, a3: v85);
          v138 = i == 2;
          v137 = v81[6];
          v136 = v81[5];
          v135 = v81[2];
          v89 = sub_41468C(a1: &v181);
          v134 = v90;
          v173 = &v134;
          _bstr_t::_bstr_t(a1: v89);
          sub_66936E(this, a2: &pvarg.cyVal.Hi, a3: v134, a4: v135, a5: v136, a6: v137, a7: v138);
          if ( pvarg.cyVal.Hi != 0 )
            (*(*pvarg.cyVal.Hi + 8))(a1: pvarg.cyVal.Hi);
        }
        v187 = -1;
        ZXString<unsigned short>::_Release(a1: &v181);
      }
      else
      {
        v146 = 0;
        v145 = v81[6];
        v144 = v81[5];
        v143 = v81[2];
        v142 = v83;
        v173 = &v142;
        sub_405210(a1: v81 + 4);
        sub_66936E(this, a2: &v182, a3: v142, a4: v143, a5: v144, a6: v145, a7: v146);
        if ( v182 != 0 )
          ((*v182)[2])(a1: v182);
      }
      if ( v81[3] != 0 )
        play_skill_sound(a1: v81[1], a2: 0x32);
      sub_67328A(a1: v175);
      v80 = v180;
    }
  }
  while ( *(this + 330) == 0 && *(this + 334) != 0 )
  {
    pvarg.cyVal.Hi = *(*(this + 335) + 4);
    sub_672644(a1: &pvarg.decVal.Lo32);
    v7 = *pvarg.cyVal.Hi == 0;
    v187 = 10;
    v146 = pvarg.cyVal.Hi + 20;
    v182 = &v142;
    v141 = 128;
    if ( v7 )
    {
      v92 = sub_672360(a1: &pvarg.decVal.Lo32);
      UINT128::UINT128(this: &v142, a2: (v92 + 4), a3: v141);
      CMob::ProcessStatReset(a1: this, a2: v142, a3: v143, a4: v144, a5: v145, a6: v146);
    }
    else
    {
      v91 = sub_672360(a1: &pvarg.decVal.Lo32);
      UINT128::UINT128(this: &v142, a2: (v91 + 4), a3: v141);
      CMob::ProcessStatSet(a1: this, a2: v142, a3: v143, a4: v144, a5: v145, a6: v146);
    }
    sub_67348C(a1: *(this + 335));
    v187 = -1;
    if ( pvarg.cyVal.Hi != 0 )
    {
      ZRef<CMob::ReservedPacket>::~ZRef<CMob::ReservedPacket>(a1: 0);
      pvarg.cyVal.Hi = 0;
    }
  }
  v182 = sub_66E9F7(a1: update_time);
  if ( v182 != 0 )
  {
    v146 = *(this + 278);
    v145 = (this + 1104);
    if ( timeGetTime() == 0 )
    {
      if ( *(this + 169) == 0 || (v93 = *(this + 99)) == 0 )
        v93 = *(this + 98);
      v94 = *(v93 + 544);
      if ( v94 > 0 )
      {
        v95 = rand();
        *(this + 242) = _ZtlSecureTear<long>(a1: v95 % v94 + 6, a2: this + 960);
        CMob::PrepareActionLayer(this);
      }
    }
  }
  v146 = *(this + 278);
  v145 = (this + 1104);
  if ( timeGetTime() != 0 )
    updated = -1;
  else
    updated = CVecCtrl::UpdatePassive(this: v177);
  CMob::SetMoveAction(this, a2: *(v177 + 82), a3: 0);
  if ( updated == -1 && (_ZtlSecureFuse<long>(a1: this + 960, a2: *(this + 242)) <= -1 || v182 != 0) )
  {
    v146 = *(this + 278);
    v145 = (this + 1104);
    if ( timeGetTime() == 0
      && (*(this + 205) == 0 && *(this + 329) == 0 || *(*(this + 70) != 0 ? *(this + 70) - 12 + 0x110 : 272) != 0)
      && (_ZtlSecureFuse<long>(a1: this + 304, a2: *(this + 78)) == 3
       || _ZtlSecureFuse<long>(a1: this + 304, a2: *(this + 78)) == 4) )
    {
      CMob::GenerateMovePath(a1: this, a2: -1, a3: 0, a4: 0, a5: 0, a6: 0, a7: 0, a8: 0, a9: 0, a10: 0);
    }
  }
  sub_66F710(a1: 0);
  CMob::ProcessAction(this, a2: update_time);
  sub_666362(a1: update_time);
  v97 = *(this + 325);
  *(this + 326) = *(this + 324);
  *(this + 327) = v97;
  VariantInit(&pvarg);
  v98 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);
  if ( v98 < 0 )
    _com_issue_error(a1: v98);
  v7 = *(this + 70) == 0;
  v187 = 11;
  if ( v7 )
    _com_issue_error(a1: -2147467261);
  v99 = *(this + 70);
  v100 = *v99;
  v182 = v99;
  v101 = (v100[35])(
           a1: v99,
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
  if ( v101 < 0 )
    _com_issue_errorex(a1: v101, a2: v182, a3: dword_BD8348);
  v187 = -1;
  v102 = ZComAPI::ZComVariantCopy(&pvarg);
  if ( v102 < 0 )
    _com_issue_error(a1: v102);
  v103 = *(this + 85);
  if ( v103 != 0 && v103 - update_time < 0 )
  {
    if ( *(this + 304) == 0 )
      _com_issue_error(a1: -2147467261);
    v104 = *(this + 304);
    v105 = *v104;
    v133 = -1;
    v132 = v104;
    v106 = (*(v105 + 224))();
    if ( v106 < 0 )
      _com_issue_errorex(a1: v106, a2: v104, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf);
    *(this + 85) = 0;
  }
  if ( *(*(this + 98) + 520) != 0 && _ZtlSecureFuse<long>(a1: this + 972, a2: *(this + 245)) != 0 )
  {
    ++*(this + 94);
    if ( (*(this + 376) & 3u) < 2 )
    {
      if ( *(this + 304) == 0 )
        _com_issue_error(a1: -2147467261);
      v107 = *(this + 304);
      v108 = (*(*v107 + 224))(a1: v107, a2: -8355712);
      if ( v108 < 0 )
        _com_issue_errorex(a1: v108, a2: v107, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf);
    }
  }
  if ( *(*(this + 98) + 520) == 0 )
  {
    v183 = 0;
    v177 = 0;
    i = get_field();
    v109 = *(this + 314);
    if ( v109 != 0 )
    {
      v110 = &v109[*(this + 315)];
      while ( v109 < v110 )
      {
        if ( *v109 != 0 )
        {
          v111 = *v109;
          goto LABEL_213;
        }
        ++v109;
      }
    }
    v111 = 0;
LABEL_213:
    v176 = v111;
    while ( v176 != 0 )
    {
      v112 = v176;
      v113 = sub_4E87BE(a1: &v176, a2: &v179);
      v177 = (v177 + 1);
      v181 = *v113;
      if ( v177 != 1 )
      {
        v114 = dword_BF060C(a1: v132);
        if ( v181 >= v114 )
          continue;
      }
      ZMap<long,long,long>::RemoveKey(a1: v112 + 8);
      if ( v183 < v181 )
      {
        v115 = *(this + 98);
        v131 = *(v115 + 488);
        v130 = v115 + 480;
        if ( timeGetTime() != 0 )
        {
          if ( i != 0 )
          {
            v116 = _ZtlSecureFuse<long>(a1: *(this + 98) + 124, a2: *(*(this + 98) + 132));
            v117 = _ZtlSecureFuse<long>(a1: *(this + 98) + 124, a2: *(*(this + 98) + 132));
            v182 = _ZtlSecureFuse<long>(a1: *(this + 98) + 504, a2: *(*(this + 98) + 512));
            pvarg.cyVal.Hi = _ZtlSecureFuse<long>(a1: *(this + 98) + 492, a2: *(*(this + 98) + 500));
            v118 = _ZtlSecureFuse<unsigned long>(a1: *(this + 98) + 12, a2: *(*(this + 98) + 20));
            CField::ShowMobHPTag(
              this: i,
              arg0: v118,
              a3: SBYTE4(pvarg.decVal.Lo64),
              a4: v182,
              a5: v179 * v117 / 100,
              a6: v116);
          }
        }
        else
        {
          CMob::CreateHPIndicator(this, a2: v179, a3: -16711936);
          v119 = *(this + 70);
          v120 = &v131;
          v182 = &v131;
          v131 = v119;
          if ( v119 != 0 )
          {
            v121 = *v119;
            v130 = v119;
            (*(v121 + 4))();
          }
          v129 = v120;
          pvarg.cyVal.Hi = &v129;
          sub_4145AB(a1: this + 1216);
          v187 = -1;
          v122 = sub_668AA6(a1: this);
          v123 = sub_428226(a1: v122 + 48);
          sub_66C734(a1: v123, a2: v129, a3: v130);
        }
        v183 = v181;
      }
    }
    v124 = *(this + 98);
    v130 = *(v124 + 488);
    v129 = (v124 + 480);
    if ( timeGetTime() != 0 && *(this + 311) != 0 && i != 0 && *(this + 311) + 20000 < dword_BF060C(a1: v131) )
    {
      v125 = _ZtlSecureFuse<long>(a1: *(this + 98) + 124, a2: *(*(this + 98) + 132));
      v126 = _ZtlSecureFuse<long>(a1: *(this + 98) + 504, a2: *(*(this + 98) + 512));
      v182 = _ZtlSecureFuse<long>(a1: *(this + 98) + 492, a2: *(*(this + 98) + 500));
      v127 = _ZtlSecureFuse<unsigned long>(a1: *(this + 98) + 12, a2: *(*(this + 98) + 20));
      CField::ShowMobHPTag(this: i, arg0: v127, a3: v182, a4: v126, a5: -1, a6: v125);
    }
    if ( *(this + 306) != 0 )
    {
      if ( *(this + 311) + 5000 >= dword_BF060C(a1: v131) )
      {
        if ( *(*dword_BE7918 + 14300) == 0 || *(*(this + 98) + 552) != 0 )
          sub_668B02(a1: this);
        else
          CMob::ShowHPIndicator(this);
        if ( *(*dword_BE7918 + 14296) != 0 && *(*(this + 98) + 556) == 0 )
        {
          v130 = 1;
          goto LABEL_234;
        }
      }
      else
      {
        sub_668B02(a1: this);
        sub_428712(a1: 0);
      }
      v130 = 0;
LABEL_234:
      CLife::ShowNameTag(this, bShow: v130);
    }
    sub_66FC9F(a1: this);
  }
  result = _ZtlSecureFuse<long>(a1: *(this + 98) + 456, a2: *(*(this + 98) + 464));
  if ( result != 0 )
  {
    sub_670B96(a1: *(this + 34));
    return sub_6707E1(a1: this, a2: v131);
  }
  return result;
}