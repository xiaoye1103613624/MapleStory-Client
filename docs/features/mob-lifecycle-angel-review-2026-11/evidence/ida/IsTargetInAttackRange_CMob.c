// Angel 083 `IsTargetInAttackRange_CMob` @ 0x66a517
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
signed int __thiscall CMob::IsTargetInAttackRange(CMob *this, int a2, int *a3, int *a4, struct CMob::TARGETINFO *a5)
{
  CMob *v5; // ebx
  bool v6; // zf
  int v7; // eax
  int v8; // eax
  int v9; // edi
  int v10; // esi
  _DWORD *v11; // ecx
  _DWORD *v12; // eax
  _DWORD *v13; // eax
  int v14; // eax
  _DWORD *AttackInfo; // eax
  _DWORD *v16; // esi
  int v17; // eax
  int v18; // eax
  char *v19; // edi
  int v20; // eax
  _DWORD *v21; // ebx
  int v22; // eax
  int v23; // eax
  int v24; // edx
  unsigned int v25; // ecx
  BOOL v26; // eax
  CMob *v27; // edi
  char *v28; // ebx
  int v29; // eax
  int v30; // eax
  int v31; // eax
  int v32; // eax
  int v33; // edi
  int v34; // edi
  int v35; // eax
  int v36; // eax
  _DWORD *v37; // edi
  int v38; // esi
  CMob *v39; // ecx
  unsigned int v40; // edi
  int v41; // eax
  int v42; // eax
  int v43; // eax
  int v44; // eax
  int v45; // eax
  _DWORD *v46; // eax
  int v47; // ebx
  int v48; // eax
  int v49; // edi
  int v50; // eax
  __int16 v51; // cx
  bool v52; // cc
  __int16 v53; // si
  int v54; // eax
  unsigned int v55; // ecx
  unsigned int v56; // eax
  int v57; // eax
  int v58; // ecx
  int v59; // eax
  int v61; // [esp-14h] [ebp-158h]
  int v62; // [esp-14h] [ebp-158h]
  int v63; // [esp-10h] [ebp-154h]
  int v64; // [esp+0h] [ebp-144h]
  int v65[22]; // [esp+Ch] [ebp-138h] BYREF
  _DWORD v66[4]; // [esp+64h] [ebp-E0h] BYREF
  int v67[4]; // [esp+74h] [ebp-D0h] BYREF
  int v68[9]; // [esp+84h] [ebp-C0h] BYREF
  _DWORD v69[4]; // [esp+A8h] [ebp-9Ch] BYREF
  _DWORD v70[2]; // [esp+B8h] [ebp-8Ch] BYREF
  int v71; // [esp+C0h] [ebp-84h]
  int v72; // [esp+C4h] [ebp-80h]
  int v73; // [esp+C8h] [ebp-7Ch]
  _DWORD *v74; // [esp+CCh] [ebp-78h]
  int v75; // [esp+D0h] [ebp-74h] BYREF
  int v76; // [esp+D4h] [ebp-70h]
  int v77; // [esp+D8h] [ebp-6Ch]
  int v78; // [esp+DCh] [ebp-68h]
  int v79; // [esp+E0h] [ebp-64h]
  int v80; // [esp+E4h] [ebp-60h]
  _DWORD *v81; // [esp+E8h] [ebp-5Ch]
  int v82; // [esp+ECh] [ebp-58h]
  int v83; // [esp+F0h] [ebp-54h]
  int v84; // [esp+F4h] [ebp-50h]
  int v85; // [esp+F8h] [ebp-4Ch]
  int v86; // [esp+FCh] [ebp-48h]
  int v87; // [esp+100h] [ebp-44h]
  CMob *v88; // [esp+104h] [ebp-40h]
  int v89; // [esp+108h] [ebp-3Ch] BYREF
  _DWORD *v90; // [esp+10Ch] [ebp-38h]
  struct tagRECT v91; // [esp+110h] [ebp-34h] BYREF
  CSummoned *v92; // [esp+120h] [ebp-24h]
  _DWORD *v93; // [esp+124h] [ebp-20h]
  int v94; // [esp+128h] [ebp-1Ch]
  unsigned int i; // [esp+12Ch] [ebp-18h]
  unsigned int v96; // [esp+130h] [ebp-14h]
  int v97; // [esp+134h] [ebp-10h]
  int v98; // [esp+140h] [ebp-4h]

  v5 = this;
  v6 = *(this + 169) == 0;
  v88 = this;
  if ( v6 || (v7 = *(this + 99)) == 0 )
    v7 = *(this + 98);
  if ( *(v7 + 536) <= 0
    || sub_667569(a1: this) != 0
    || timeGetTime() != 0
    || *(v5 + 141) != 0
    || *(v5 + 144) != 0
    || *(v5 + 172) != 0
    || *(v5 + 151) != 0
    || *(v5 + 227) != 0 )
  {
    return 0;
  }
  memset(v68, 0, sizeof(v68));
  v96 = 0;
  v8 = sub_663FE2(a1: v5);
  v9 = TSingleton<CUserLocal>::ms_pInstance;
  v82 = v8;
  v90 = TSingleton<CUserLocal>::ms_pInstance;
  v93 = 0;
  v92 = 0;
  switch ( a2 )
  {
    case 0:
      CAvatar::GetBodyRect(this: (TSingleton<CUserLocal>::ms_pInstance + 136), a2: &v91, a3: 0);
      a2 = 0;
      break;
    case 1:
      v12 = (*(*(v5 + 1) + 16))(a1: v5 + 4);
      v13 = sub_678ECC(a1: *v12, a2: v12[1]);
      v93 = v13;
      if ( v13 == 0 )
        return 0;
      sub_664559(this: v13, a2: &v91, a3: 0);
LABEL_31:
      a2 = 2;
      break;
    case 2:
      v93 = *(dword_BEBFA4 + 104);
      if ( v93 == 0 )
        return 0;
      sub_664559(this: v93, a2: &v91, a3: 0);
      goto LABEL_31;
    case 3:
      v10 = *(*(v5 + 71) != 0 ? *(v5 + 71) - 12 + 0x250 : 592);
      if ( v10 == 0 )
        return 0;
      if ( (*(*v10 + 32))(a1: v10, a2: &dword_BF0D30) != 0 )
        v92 = (v10 - 4);
      else
        v92 = 0;
      if ( (*(*v10 + 32))(a1: v10, a2: &dword_BEDB54) != 0 )
        v11 = (v10 - 4);
      else
        v11 = 0;
      v93 = v11;
      if ( (v9 != 0 ? v9 + 4 : 0) == v10 )
      {
        CAvatar::GetBodyRect(this: (v9 + 136), a2: &v91, a3: 0);
        a2 = 0;
      }
      else if ( v10 == (v92 != 0 ? v92 + 4 : 0) )
      {
        CSummoned::GetBodyRect(this: v92, a2: &v91, a3: 0);
        a2 = 1;
      }
      else
      {
        if ( v10 != (v11 != 0 ? (v11 + 1) : 0) )
          return 0;
        sub_664559(this: v11, a2: &v91, a3: 0);
        a2 = 2;
      }
      break;
    default:
      break;
  }
  if ( *(v5 + 169) != 0 || (v6 = *(*(v5 + 98) + 416) == 0, v87 = 1, v6) )
    v87 = 0;
  v94 = 0;
  while ( 1 )
  {
    if ( *(v5 + 169) == 0 || (v14 = *(v5 + 99)) == 0 )
      v14 = *(v5 + 98);
    if ( v94 >= *(v14 + 536) )
      break;
    AttackInfo = CMobTemplate::GetAttackInfo(a1: v94);
    v16 = AttackInfo;
    if ( AttackInfo == 0
      || AttackInfo[36] == 0 && *(v5 + 93) != 0
      || AttackInfo[4] != 0
      || AttackInfo[5] > _ZtlSecureFuse<long>(a1: v5 + 400, a2: *(v5 + 102))
      || a2 == 0 && CAvatar::IsHideMorphed(this: v90 + 34)
      || v16[40] != 0 && *(v5 + 35) == 0
      || a2 == 0
      && v16[7] == 0
      && _ZtlSecureFuse<unsigned long>(a1: v5 + 380, a2: *(v5 + 97)) != (&loc_8F7111 + 1)
      && (CUser::IsDarkSight(this: v90) || CUser::IsSneak(this: v90)) )
    {
      goto LABEL_124;
    }
    if ( timeGetTime() != 0 )
    {
      if ( *(v5 + 169) == 0 || (v17 = *(v5 + 99)) == 0 )
        v17 = *(v5 + 98);
      if ( v94 == *(v17 + 536) - 1 )
        break;
    }
    v18 = v16[3];
    if ( v18 == 0 )
    {
      v19 = v5 + 4;
      v97 = *((*(*(v5 + 1) + 16))(a1: v5 + 4, a2: v64) + 4);
      i = *(*(*(v5 + 1) + 16))(a1: v5 + 4);
      v20 = sub_4144D4(a1: v67, a2: v16 + 10);
      dword_BF0454(a1: v20, a2: i, a3: v97);
      v97 = *((*(*v19 + 16))(a1: v19) + 4);
      i = *(*(*v19 + 16))(a1: v19);
      v21 = sub_4144D4(a1: v66, a2: v16 + 10);
      v21[2] = -v21[2];
      *v21 = -*v21;
      sub_45730D(a1: v21, a2: v21 + 2);
      dword_BF0454(a1: v21, a2: i, a3: v97);
      if ( v82 != 0 )
      {
        if ( dword_BF04A8(a1: v69, a2: v67, a3: &v91) == 0 )
        {
          if ( v87 != 0 )
            goto LABEL_124;
          v22 = dword_BF04A8(a1: v69, a2: v66, a3: &v91);
          goto LABEL_67;
        }
        goto LABEL_84;
      }
      if ( dword_BF04A8(a1: v69, a2: v66, a3: &v91) != 0 )
        goto LABEL_78;
      v23 = dword_BF04A8(a1: v69, a2: v67, a3: &v91);
      goto LABEL_83;
    }
    if ( v18 != 3 && v18 != 4 )
    {
      v19 = v5 + 4;
      v83 = v16[10] + *(*(*(v5 + 1) + 16))(a1: v5 + 4);
      v84 = v16[11] + *((*(*(v5 + 1) + 16))(a1: v5 + 4) + 4);
      v85 = *(*(*(v5 + 1) + 16))(a1: v5 + 4) - v16[10];
      v86 = v16[11] + *((*(*(v5 + 1) + 16))(a1: v5 + 4) + 4);
      if ( v82 != 0 )
      {
        if ( sub_66AFB7(a1: v83, a2: v83, a3: v83 - v16[12], a4: v84, a5: 4, a6: &v91) == 0 )
        {
          if ( v87 != 0 )
            goto LABEL_124;
          v22 = sub_66AFB7(a1: v85, a2: v85, a3: v85 + v16[12], a4: v86, a5: 4, a6: &v91);
LABEL_67:
          if ( v22 == 0 )
            goto LABEL_124;
LABEL_78:
          v24 = 0;
LABEL_79:
          v25 = v96;
          v6 = v87 == 0;
          v65[v96 + 4] = v94;
          v26 = !v6 || v24 != 0;
          v65[v25 + 13] = v26;
          v42 = v16[3];
          if ( v42 == 1 )
          {
            if ( a2 >= 0 )
            {
              if ( a2 <= 1 )
              {
                if ( v92 != 0 )
                  v44 = sub_765C29(a1: *(v92 + 45));
                else
                  v44 = 0;
                v68[v96] = v44 | (4 * v90[1130]);
              }
              else if ( a2 == 2 )
              {
                v43 = _ZtlSecureFuse<unsigned long>(a1: v93 + 95, a2: v93[97]);
                v68[v96] = (4 * v43) | 3;
              }
            }
          }
          else if ( v42 == 2 )
          {
            v45 = v16[10];
            if ( v24 == 0 )
              v45 = -v45;
            v97 = v45;
            v46 = (*(*v19 + 16))(a1: v19);
            v47 = v97 + *v46;
            v48 = v16[11] + *((*(*v19 + 16))(a1: v19) + 4);
            v80 = v47;
            v49 = v48;
            v50 = (v91.right + v91.left) / 2;
            if ( v82 != 0 )
            {
              v51 = LOWORD(v91.right) - 10;
              v52 = v50 < v91.right - 10;
            }
            else
            {
              v51 = LOWORD(v91.left) + 10;
              v52 = v91.left + 10 < v50;
            }
            v53 = v51;
            if ( !v52 )
              v53 = (v91.right + v91.left) / 2;
            LOWORD(v54) = LOWORD(v91.top) + 10;
            if ( v49 >= v91.top + 10 )
            {
              LOWORD(v54) = LOWORD(v91.bottom) - 10;
              if ( v49 <= v91.bottom - 10 )
                v54 = (v91.bottom + v91.top) / 2;
            }
            v55 = v96;
            LOWORD(v68[v96]) = v53;
            HIWORD(v68[v55]) = v54;
          }
          else
          {
            v68[v25] = 0;
          }
          ++v96;
          goto LABEL_124;
        }
LABEL_84:
        v24 = 1;
        goto LABEL_79;
      }
      if ( sub_66AFB7(a1: v85, a2: v85, a3: v85 + v16[12], a4: v86, a5: 4, a6: &v91) != 0 )
        goto LABEL_78;
      v23 = sub_66AFB7(a1: v83, a2: v83, a3: v83 - v16[12], a4: v84, a5: 4, a6: &v91);
LABEL_83:
      if ( v23 == 0 )
        goto LABEL_124;
      goto LABEL_84;
    }
    if ( get_update_time() - *(v5 + 92) < 2500 )
      goto LABEL_124;
    sub_4144D4(a1: &v75, a2: v16 + 14);
    v27 = v88;
    v28 = v88 + 4;
    v29 = (*(*(v88 + 1) + 16))(a1: v88 + 4);
    v76 += *(v29 + 4);
    v30 = (*(*v28 + 16))(a1: v28);
    v78 += *(v30 + 4);
    v97 = (*(*v28 + 16))(a1: v28);
    v31 = sub_663FE2(a1: v27);
    v75 = *v97 + v16[10] * v16[13] * (v31 != 0 ? 1 : -1);
    v97 = (*(*v28 + 16))(a1: v28);
    v32 = sub_663FE2(a1: v27);
    v77 = *v97 + v16[13] * (v16[11] + v16[10]) * (v32 != 0 ? 1 : -1);
    if ( dword_BF04A8(a1: v65, a2: &v75, a3: &v91) == 0 )
      goto LABEL_124;
    v89 = 0;
    v63 = v16[12];
    v61 = v16[11];
    v98 = 0;
    get_random_unique_array(a1: &v89, a2: v16[10], a3: v61, a4: v63);
    v97 = 0;
    for ( i = 0; v89 != 0 && i < *(v89 - 4); ++i )
    {
      v74 = (v89 + 4 * i);
      v81 = (*(*v28 + 16))(a1: v28);
      v79 = v16[13];
      v33 = sub_663FE2(a1: v88);
      v34 = *v81 + (v79 / 2 + v79 * *v74) * (v33 != 0 ? 1 : -1);
      v71 = 0;
      v72 = 0;
      v73 = 0;
      v70[0] = &off_AF1218;
      v35 = v16[3];
      LOBYTE(v98) = 1;
      if ( v35 == 3 )
      {
        v81 = dword_BEBFA0;
        v62 = *((*(*v28 + 16))(a1: v28) + 4) + 100;
        v36 = (*(*v28 + 16))(a1: v28);
        CWvsPhysicalSpace2D::GetFootholdRange(this: v81, x: v34, y1: *(v36 + 4) - 100, y2: v62, lPosition: v70);
      }
      else
      {
        if ( v35 != 4 )
          goto LABEL_96;
        v37 = ((*(*v28 + 16))(a1: v28) + 4);
        *ZList<long>::AddTail(a1: v70) = *v37;
      }
      if ( v71 != 0 )
        v97 |= 1 << (*(v89 + 4 * i) - *(v16 + 40));
LABEL_96:
      LOBYTE(v98) = 0;
      v70[0] = &off_AF1218;
      ZList<long>::RemoveAll(a1: v70);
    }
    v38 = v97;
    if ( v97 != 0 )
    {
      v39 = v88;
      v40 = v96;
      v65[v96 + 4] = v94;
      v41 = sub_663FE2(a1: v39);
      ++v96;
      v65[v40 + 13] = v41;
      v68[v40] = v38;
    }
    v98 = -1;
    ZArray<long>::RemoveAll(a1: &v89);
LABEL_124:
    ++v94;
    v5 = v88;
  }
  if ( v96 == 0 )
    return 0;
  if ( a3 != 0 && a4 != 0 && a5 != 0 )
  {
    v56 = CRand32::Random(a1: dword_BEDD38);
    v57 = v56 % v96;
    *a3 = v65[v57 + 4] + 12;
    v58 = v65[v57 + 13];
    v59 = v68[v57];
    *a4 = v58;
    *a5 = v59;
  }
  return 1;
}