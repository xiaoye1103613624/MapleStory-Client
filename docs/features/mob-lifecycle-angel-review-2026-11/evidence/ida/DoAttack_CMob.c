// Angel 083 `DoAttack_CMob` @ 0x66d9c0
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
void __fastcall CMob::DoAttack(_DWORD *a1, int a2, int a3, int a4, unsigned int a5)
{
  bool v6; // zf
  _DWORD *AttackInfo; // eax
  _DWORD *v8; // esi
  int *v9; // eax
  int v10; // eax
  int v11; // eax
  int v12; // eax
  int v13; // ecx
  int v14; // eax
  int v15; // ecx
  int v16; // eax
  int v17; // eax
  int (*v18)(); // eax
  CSummoned *v19; // edi
  struct CMob *Mob; // eax
  int v21; // eax
  _DWORD *v22; // eax
  LONG v23; // ecx
  LONG bottom; // edi
  int v25; // eax
  int v26; // edx
  bool v27; // cc
  int v28; // eax
  _DWORD *v29; // eax
  int *v30; // eax
  int v31; // eax
  int v32; // eax
  int v33; // eax
  int v34; // edi
  int v35; // ecx
  int v36; // edx
  double v37; // st7
  double v38; // st7
  double v39; // st7
  int v40; // edi
  int v41; // eax
  _DWORD *v42; // edi
  int *v43; // esi
  int v44; // ecx
  int v45; // edi
  int v46; // edi
  int v47; // eax
  int v48; // eax
  _DWORD *v49; // edi
  _DWORD *v50; // edi
  int *v51; // eax
  int *v52; // ecx
  int v53; // edx
  _DWORD *v54; // eax
  int v55; // edx
  int v56; // ecx
  int v57; // eax
  int v58; // eax
  int v59; // edx
  int v60; // ecx
  char *v61; // eax
  char v62; // al
  _DWORD *v63; // ebx
  char v64; // si
  int *v65; // eax
  int v66; // eax
  wchar_t *v67; // [esp-20h] [ebp-84h]
  int v68; // [esp-18h] [ebp-7Ch] BYREF
  int v69; // [esp-14h] [ebp-78h]
  int v70; // [esp-10h] [ebp-74h]
  int v71; // [esp-Ch] [ebp-70h] BYREF
  int v72; // [esp-8h] [ebp-6Ch]
  void *v73[5]; // [esp+0h] [ebp-64h]
  _DWORD v74[2]; // [esp+14h] [ebp-50h] BYREF
  int v75; // [esp+1Ch] [ebp-48h]
  LONG *v76; // [esp+20h] [ebp-44h]
  int v77; // [esp+24h] [ebp-40h]
  double v78; // [esp+28h] [ebp-3Ch] BYREF
  int v79; // [esp+30h] [ebp-34h] BYREF
  int *v80; // [esp+34h] [ebp-30h]
  struct tagRECT v81; // [esp+38h] [ebp-2Ch] BYREF
  unsigned int v82; // [esp+48h] [ebp-1Ch]
  char *v83; // [esp+4Ch] [ebp-18h]
  int v84; // [esp+50h] [ebp-14h]
  int update_time; // [esp+54h] [ebp-10h]
  int v86; // [esp+60h] [ebp-4h]
  int v87; // [esp+6Ch] [ebp+8h]
  int v88; // [esp+6Ch] [ebp+8h]
  int v89; // [esp+6Ch] [ebp+8h]
  int v90; // [esp+6Ch] [ebp+8h]
  int v91; // [esp+74h] [ebp+10h]
  int v92; // [esp+74h] [ebp+10h]
  int v93; // [esp+74h] [ebp+10h]

  v6 = a1[99] == 0;
  HIDWORD(v78) = 0;
  if ( !v6 )
    return;
  if ( a1[141] != 0 )
    return;
  if ( a1[144] != 0 )
    return;
  if ( a1[172] != 0 )
    return;
  if ( a1[151] != 0 )
    return;
  update_time = get_update_time();
  v84 = a3 - 12;
  AttackInfo = CMobTemplate::GetAttackInfo(a1: a3 - 12);
  v8 = AttackInfo;
  if ( AttackInfo == 0 )
    return;
  v9 = AttackInfo[29];
  if ( v9 != 0 && (v10 = *v9) != 0 )
    v11 = *(v10 - 4) >> 1;
  else
    v11 = 0;
  if ( v11 != 0 )
  {
    v12 = v8[29];
    if ( v12 != 0 )
      InterlockedIncrement(lpAddend: (v12 + 8));
    sub_402D9A(this: a1 + 43);
    v13 = update_time;
    a1[43] = v8[29];
    a1[42] = a4;
    v14 = v13 + v8[34];
    a1[41] = v14;
    if ( v14 == 0 )
      a1[41] = 1;
  }
  if ( v8[40] != 0 )
    a1[35] = 0;
  v15 = v8[3];
  if ( v15 != 1 )
  {
    if ( v15 == 2 )
    {
      v83 = (v8 + 31);
      v30 = v8[31];
      if ( v30 != 0 && (v31 = *v30) != 0 )
        v32 = *(v31 - 4) >> 1;
      else
        v32 = 0;
      if ( v32 != 0 )
      {
        v33 = v8[10];
        if ( a4 == 0 )
          v33 = -v33;
        LODWORD(v78) = v33 + *(*(a1[1] + 16))(a1: a1 + 1);
        v34 = SHIWORD(a5);
        HIDWORD(v78) = v8[11] + *((*(a1[1] + 16))(a1: a1 + 1) + 4);
        v79 = a5;
        if ( a4 != 0 )
        {
          v35 = LODWORD(v78) - a5;
          v91 = v35;
        }
        else
        {
          v91 = v79 - LODWORD(v78);
          v35 = v79 - LODWORD(v78);
        }
        v88 = v34 - HIDWORD(v78);
        if ( v35 <= 0 )
        {
          if ( a4 != 0 )
            v36 = LODWORD(v78) - 1;
          else
            v36 = LODWORD(v78) + 1;
          v34 = HIDWORD(v78);
          v88 = 0;
          v79 = v36;
          v91 = 1;
        }
        v82 = v88 / v91;
        if ( fabs(a1: (v88 / v91)) > dbl_AF82C8 )
        {
          if ( v88 <= 0 )
            v37 = -v91;
          else
            v37 = v91;
          v34 = (SHIDWORD(v78) + v37 * dbl_AF82C8);
          v88 = v34 - HIDWORD(v78);
        }
        v38 = sqrt(a1: (v91 * v91 + v88 * v88));
        *&v81.right = v8[12];
        v39 = *&v81.right / v38;
        v79 = ((v79 - LODWORD(v78)) * v39 + SLODWORD(v78));
        v80 = ((v34 - HIDWORD(v78)) * v39 + SHIDWORD(v78));
        v92 = (*&v81.right * dbl_AF82C0 / v8[26]);
        v40 = sub_672CDB(a1: a1 + 49, a2: (*&v81.right * dbl_AF82C0 / v8[26]) >> 32);
        v41 = update_time + v8[35];
        v73[1] = v83;
        *v40 = v41;
        *(v40 + 4) = update_time + v92 + v8[35];
        *(v40 + 8) = v78;
        *(v40 + 16) = v79;
        *(v40 + 20) = v80;
        *(v40 + 24) = a4;
        sub_402C9B(a1: v73[1]);
        *(v40 + 36) = v84;
        goto LABEL_87;
      }
    }
    if ( v15 == 0 )
    {
      v42 = sub_672E65(a1: a1 + 54);
      *v42 = update_time + v8[35];
      v42[1] = v8[3];
      v93 = *((*(a1[1] + 16))(a1: a1 + 1) + 4);
      v89 = *(*(a1[1] + 16))(a1: a1 + 1);
      v43 = sub_4144D4(a1: v42 + 2, a2: v8 + 10);
      if ( a4 == 0 )
      {
        v43[2] = -v43[2];
        v44 = *v43;
        v73[1] = v43 + 2;
        v73[0] = v43;
        *v43 = -v44;
        sub_45730D(a1: v73[0], a2: v73[1]);
      }
      dword_BF0454(a1: v43, a2: v89, a3: v93);
      v42[6] = a4;
      v42[7] = v84;
      goto LABEL_87;
    }
    if ( v15 != 3 && v15 != 4 || (v27 = v8[11] <= 0, v83 = 0, v90 = 0, v27) )
    {
LABEL_87:
      v62 = _ZtlSecureFuse<unsigned long>(a1: a1[98] + 12, a2: *(a1[98] + 20));
      v63 = a1 + 1;
      v64 = v62;
      v72 = *((*(*v63 + 16))(a1: v63) + 4);
      v65 = (*(*v63 + 16))(a1: v63);
      v66 = sub_9894F3(a1: *v65, a2: v72);
      sub_989BA1(a1: v64, a2: (v84 + 2), a3: v66);
      return;
    }
    while ( 1 )
    {
      if ( v83 >= v8[12] )
        goto LABEL_87;
      if ( ((1 << v90) & a5) != 0 )
        break;
LABEL_86:
      if ( ++v90 >= v8[11] )
        goto LABEL_87;
    }
    v45 = v8[13];
    v82 = (*(a1[1] + 16))(a1: a1 + 1);
    v46 = (a4 != 0 ? 1 : -1) * (v45 / 2 + v45 * (v90 + v8[10])) + *v82;
    v80 = v46;
    v75 = 0;
    v76 = 0;
    v77 = 0;
    v74[0] = &off_AF1218;
    v47 = v8[3];
    v86 = 1;
    if ( v47 == 3 )
    {
      v82 = dword_BEBFA0;
      v73[1] = v74;
      v73[0] = (*((*(a1[1] + 16))(a1: a1 + 1) + 4) + 100);
      v48 = (*(a1[1] + 16))(a1: a1 + 1);
      CWvsPhysicalSpace2D::GetFootholdRange(this: v82, x: v46, y1: *(v48 + 4) - 100, y2: v73[0], lPosition: v73[1]);
    }
    else
    {
      if ( v47 != 4 )
      {
LABEL_85:
        v86 = -1;
        v74[0] = &off_AF1218;
        ZList<long>::RemoveAll(a1: v74);
        goto LABEL_86;
      }
      v49 = ((*(a1[1] + 16))(a1: a1 + 1) + 4);
      *ZList<long>::AddTail(a1: v74) = *v49;
    }
    if ( v75 != 0 )
    {
      v50 = sub_672E65(a1: a1 + 54);
      *v50 = update_time + v8[35];
      v50[1] = v8[3];
      v81.bottom = *v76;
      HIDWORD(v78) = v50 + 2;
      v51 = sub_4144D4(a1: v50 + 2, a2: v8 + 14);
      v82 = v51;
      if ( a4 == 0 )
      {
        v52 = v51;
        v53 = v51[2];
        v54 = v51 + 2;
        *v54 = -v53;
        v55 = *v52;
        v73[1] = v54;
        v73[0] = v52;
        *v52 = -v55;
        sub_45730D(a1: v73[0], a2: v73[1]);
      }
      dword_BF0454(a1: v82, a2: v80, a3: v81.bottom);
      ++v83;
      v50[6] = a4;
      v72 = -1073343324;
      v71 = v56;
      v50[7] = v84;
      v80 = &v71;
      v70 = 0;
      a1[92] = update_time;
      sub_432C10(a1: v70);
      v57 = v8[13];
      v70 = v50[5];
      v58 = *HIDWORD(v78) + v57 / 2;
      LOBYTE(v86) = 2;
      v69 = v58;
      v68 = HIDWORD(v78);
      HIDWORD(v78) = &v68;
      sub_432BDD(a1: 0);
      LOBYTE(v86) = 1;
      v67 = sub_40B31F(a1: v8 + 32);
      v61 = sub_432CBC(a1: v60, a2: v59);
      CAnimationDisplayer::Effect_General(this: v61, a2: v67, a3: 0, a4: v68, a5: v69, a6: v70, a7: v71, a8: v72);
    }
    goto LABEL_85;
  }
  v83 = (a5 & 3);
  memset(&v81, 0, sizeof(v81));
  v16 = *(TSingleton<CUserLocal>::ms_pInstance + 4520) & 0x3FFFFFFF;
  v82 = a5 >> 2;
  v87 = 1;
  if ( a5 >> 2 == v16 )
  {
    if ( v83 == 0 )
    {
      CAvatar::GetBodyRect(this: (TSingleton<CUserLocal>::ms_pInstance + 136), a2: &v81, a3: 0);
      goto LABEL_29;
    }
    v17 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v78) + 4);
    v86 = 0;
    v18 = sub_765D49(a1: v17, a2: v83);
    v19 = *(sub_938EC9(a1: &v79, a2: v18) + 4);
    sub_6725CC(a1: &v79);
    v86 = -1;
    if ( HIDWORD(v78) != 0 )
      sub_428C44(this: &v78, a2: 0);
    if ( v19 != 0 )
    {
      CSummoned::GetBodyRect(this: v19, a2: &v81, a3: 0);
      v87 = 0;
    }
  }
  if ( v83 == 3 )
  {
    Mob = CMobPool::GetMob(this: dword_BEBFA4, a2: v82);
    if ( Mob == 0 )
      return;
    sub_664559(this: Mob, a2: &v81, a3: 0);
LABEL_29:
    v87 = 1;
  }
  v21 = v8[10];
  if ( a4 == 0 )
    v21 = -v21;
  v83 = v21;
  v22 = (*(a1[1] + 16))(a1: a1 + 1);
  LODWORD(v78) = &v83[*v22];
  v23 = v8[11] + *((*(a1[1] + 16))(a1: a1 + 1) + 4);
  v79 = LODWORD(v78);
  bottom = v23;
  v25 = (v81.right + v81.left) / 2;
  if ( a4 != 0 )
  {
    v26 = v81.right - 10;
    v27 = v25 < v81.right - 10;
  }
  else
  {
    v26 = v81.left + 10;
    v27 = v81.left + 10 < v25;
  }
  if ( v27 )
    v25 = v26;
  if ( v23 >= v81.top )
  {
    if ( v23 > v81.bottom )
      bottom = v81.bottom;
  }
  else
  {
    bottom = v81.top;
  }
  v82 = bottom - v23;
  v78 = sqrt(a1: (bottom - v23) * (bottom - v23) + (v25 - LODWORD(v78)) * (v25 - LODWORD(v78)));
  if ( v87 != 0 )
    v28 = abs(a1: v8[12]);
  else
    v28 = v8[12];
  if ( (v28 + 10) >= v78 )
  {
    v29 = sub_672E65(a1: a1 + 54);
    *v29 = update_time + v8[35];
    v29[1] = v8[3];
    v29[2] = a5;
    v29[6] = a4;
    v29[7] = v84;
    goto LABEL_87;
  }
}