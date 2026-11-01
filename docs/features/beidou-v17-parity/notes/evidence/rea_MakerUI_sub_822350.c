void __thiscall sub_822350(_DWORD *this, VARIANTARG **p_p_pvarg)
{
  int v3; // eax
  int v4; // eax
  int v5; // eax
  int v6; // eax
  VARIANTARG **v7; // ecx
  int v8; // eax
  int v9; // eax
  VARIANTARG **v10; // ecx
  int v11; // eax
  int v12; // eax
  int v13; // ecx
  int v14; // eax
  int v15; // eax
  int v16; // eax
  int v17; // eax
  int v18; // eax
  int v19; // ecx
  int v20; // eax
  int v21; // eax
  int v22; // eax
  int v23; // eax
  int v24; // eax
  int v25; // eax
  int v26; // eax
  int v27; // ecx
  int v28; // eax
  int v29; // eax
  int v30; // eax
  int v31; // eax
  int v32; // eax
  int v33; // eax
  int v34; // eax
  int v35; // eax
  VARIANTARG *v36; // ecx
  int v37; // eax
  int v38; // eax
  int v39; // eax
  VARIANTARG *v40; // ecx
  int v41; // eax
  int v42; // [esp-14h] [ebp-C0h] BYREF
  VARIANTARG *p_pvarg; // [esp-10h] [ebp-BCh] BYREF
  int p_pvargDest; // [esp-Ch] [ebp-B8h]
  int p_p_p_pvarg; // [esp-8h] [ebp-B4h]
  int v46; // [esp-4h] [ebp-B0h] BYREF
  _DWORD v47[9]; // [esp+Ch] [ebp-A0h] BYREF
  int v48; // [esp+30h] [ebp-7Ch]
  int v49; // [esp+54h] [ebp-58h]
  VARIANTARG v50; // [esp+5Ch] [ebp-50h] BYREF
  _DWORD v51[3]; // [esp+6Ch] [ebp-40h] BYREF
  int v52; // [esp+78h] [ebp-34h] BYREF
  VARIANTARG pvargDest; // [esp+7Ch] [ebp-30h] BYREF
  void *v54; // [esp+8Ch] [ebp-20h]
  VARIANTARG pvarg; // [esp+90h] [ebp-1Ch] BYREF
  int v56; // [esp+A8h] [ebp-4h]

  v54 = &v46; /*0x822368*/
  v46 = 0; /*0x822374*/
  sub_41C06C(Src: (void *)&word_AF0E08, a2: -1); /*0x822376*/
  sub_92C2E8(a1: p_p_pvarg, a2: v46); /*0x822380*/
  sub_4B620C(a1: v47); /*0x82238b*/
  v56 = 0; /*0x82239c*/
  v47[0] = -1118482; /*0x82239f*/
  v47[1] = -5922408; /*0x8223a9*/
  v47[2] = -6710887; /*0x8223b3*/
  v47[7] = 10; /*0x8223bd*/
  v48 = 30001; /*0x8223c7*/
  p_p_pvarg = (VARIANTARG **)sub_403065(a1: 268); /*0x8223d5*/
  LOBYTE(v56) = 1; /*0x8223da*/
  if ( p_p_pvarg != nullptr ) /*0x8223de*/
    v3 = sub_4C4259(); /*0x8223e0*/
  else
    v3 = 0; /*0x8223e7*/
  LOBYTE(v56) = 0; /*0x8223f0*/
  v4 = sub_4BCDBE(a1: v3); /*0x8223f3*/
  (*(void (__thiscall **)(_DWORD, _DWORD *, int, int, int, int, int, int, _DWORD *))(**(_DWORD **)(v4 + 4) + 32))( /*0x822416*/
    a1: *(_DWORD *)(v4 + 4),
    a2: this,
    a3: 30000,
    a4: 3,
    a5: 77,
    a6: 71,
    a7: 78,
    a8: 18,
    a9: v47);
  v48 = 40001; /*0x822420*/
  p_p_pvarg = (VARIANTARG **)sub_403065(a1: 268); /*0x82242e*/
  LOBYTE(v56) = 2; /*0x822433*/
  if ( p_p_pvarg != nullptr ) /*0x822437*/
    v5 = sub_4C4259(); /*0x822439*/
  else
    v5 = 0; /*0x822440*/
  LOBYTE(v56) = 0; /*0x822449*/
  v6 = sub_4BCDBE(a1: v5); /*0x82244c*/
  (*(void (__thiscall **)(_DWORD, _DWORD *, int, int, int, int, int, int, _DWORD *))(**(_DWORD **)(v6 + 4) + 32))( /*0x822475*/
    a1: *(_DWORD *)(v6 + 4),
    a2: this,
    a3: 40000,
    a4: 3,
    a5: 157,
    a6: 71,
    a7: 143,
    a8: 18,
    a9: v47);
  v52 = 0; /*0x822478*/
  LOBYTE(v56) = 3; /*0x82248b*/
  memset(v51, 0, sizeof(v51)); /*0x82248f*/
  sub_41C06C(Src: &off_B3930C, a2: -1); /*0x822492*/
  v7 = (VARIANTARG **)sub_403065(a1: 1444); /*0x8224a3*/
  p_p_pvarg = v7; /*0x8224a5*/
  LOBYTE(v56) = 4; /*0x8224aa*/
  if ( v7 != nullptr ) /*0x8224ae*/
    v8 = sub_4258E4(a1: v7); /*0x8224b0*/
  else
    v8 = 0; /*0x8224b7*/
  LOBYTE(v56) = 3; /*0x8224c0*/
  v9 = sub_4284FF(a1: v8); /*0x8224c4*/
  (*(void (__thiscall **)(_DWORD, _DWORD *, int, int, int, int, _DWORD *))(**(_DWORD **)(v9 + 4) + 32))( /*0x8224e3*/
    a1: *(_DWORD *)(v9 + 4),
    a2: this,
    a3: 20000,
    a4: 82,
    a5: 356,
    a6: 1,
    a7: v51);
  (*(void (__thiscall **)(int, _DWORD))(*(_DWORD *)(*(this + 841) + 4) + 28))(a1: *(this + 841) + 4, a2: 0); /*0x8224f2*/
  sub_41C06C(Src: &off_B392CC, a2: -1); /*0x8224ff*/
  v10 = (VARIANTARG **)sub_403065(a1: 1444); /*0x822510*/
  p_p_pvarg = v10; /*0x822512*/
  LOBYTE(v56) = 5; /*0x822517*/
  if ( v10 != nullptr ) /*0x82251b*/
    v11 = sub_4258E4(a1: v10); /*0x82251d*/
  else
    v11 = 0; /*0x822524*/
  LOBYTE(v56) = 3; /*0x82252d*/
  v12 = sub_4284FF(a1: v11); /*0x822531*/
  (*(void (__thiscall **)(_DWORD, _DWORD *, int, int, int, int, _DWORD *))(**(_DWORD **)(v12 + 4) + 32))( /*0x822553*/
    a1: *(_DWORD *)(v12 + 4),
    a2: this,
    a3: 20001,
    a4: 163,
    a5: 356,
    a6: 1,
    a7: v51);
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x82255f*/
  LOBYTE(v56) = 6; /*0x822568*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x82256c*/
  v46 = 0; /*0x822571*/
  p_p_p_pvarg = 0; /*0x822572*/
  p_pvargDest = (int)&pvargDest; /*0x822576*/
  p_pvarg = &pvarg; /*0x82257a*/
  v42 = v13; /*0x82257b*/
  v54 = &v42; /*0x82257e*/
  LOBYTE(v56) = 7; /*0x822586*/
  sub_425ADD(a1: "UI/UIWindow.img/Maker/notUse"); /*0x82258a*/
  LOBYTE(v56) = 7; /*0x822593*/
  v14 = sub_406472(); /*0x822597*/
  sub_402680(a1: v14); /*0x82259e*/
  sub_403A93(pvargDest: &v50, a2: v42, a3: (int)p_pvarg, a4: p_pvargDest); /*0x8225a5*/
  LOBYTE(v56) = 9; /*0x8225ac*/
  p_p_pvarg = (VARIANTARG **)sub_4032B2(a1: p_p_p_pvarg, a2: v46); /*0x8225b5*/
  v15 = sub_41E527(a1: &p_p_pvarg); /*0x8225c2*/
  if ( v15 < 0 && v15 != -2147467262 ) /*0x8225d0*/
    sub_A5FDE4(a1: v15); /*0x8225d3*/
  LOBYTE(v56) = 7; /*0x8225dc*/
  v16 = sub_40291D(pvarg: &v50); /*0x8225e0*/
  if ( v16 < 0 ) /*0x8225e8*/
    sub_A5FDE4(a1: v16); /*0x8225eb*/
  LOBYTE(v56) = 6; /*0x8225f4*/
  v17 = sub_40291D(&pvarg); /*0x8225f8*/
  if ( v17 < 0 ) /*0x822600*/
    sub_A5FDE4(a1: v17); /*0x822603*/
  LOBYTE(v56) = 3; /*0x82260c*/
  v18 = sub_40291D(pvarg: &pvargDest); /*0x822610*/
  if ( v18 < 0 ) /*0x822618*/
    sub_A5FDE4(a1: v18); /*0x82261b*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x822624*/
  LOBYTE(v56) = 10; /*0x82262d*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x822631*/
  v46 = 0; /*0x822636*/
  p_p_p_pvarg = 0; /*0x822637*/
  p_pvargDest = (int)&pvargDest; /*0x82263b*/
  p_pvarg = &pvarg; /*0x82263f*/
  v42 = v19; /*0x822640*/
  v54 = &v42; /*0x822643*/
  LOBYTE(v56) = 11; /*0x82264b*/
  sub_425ADD(a1: "UI/UIWindow.img/Maker/GaugeBar/bar"); /*0x82264f*/
  LOBYTE(v56) = 11; /*0x822658*/
  v20 = sub_406472(); /*0x82265c*/
  sub_402680(a1: v20); /*0x822663*/
  sub_403A93(pvargDest: &v50, a2: v42, a3: (int)p_pvarg, a4: p_pvargDest); /*0x82266a*/
  LOBYTE(v56) = 13; /*0x822671*/
  p_p_pvarg = (VARIANTARG **)sub_4032B2(a1: p_p_p_pvarg, a2: v46); /*0x82267a*/
  v21 = sub_41E527(a1: &p_p_pvarg); /*0x822687*/
  if ( v21 < 0 && v21 != -2147467262 ) /*0x822695*/
    sub_A5FDE4(a1: v21); /*0x822698*/
  LOBYTE(v56) = 11; /*0x8226a1*/
  v22 = sub_40291D(pvarg: &v50); /*0x8226a5*/
  if ( v22 < 0 ) /*0x8226ad*/
    sub_A5FDE4(a1: v22); /*0x8226b0*/
  LOBYTE(v56) = 10; /*0x8226b9*/
  v23 = sub_40291D(&pvarg); /*0x8226bd*/
  if ( v23 < 0 ) /*0x8226c5*/
    sub_A5FDE4(a1: v23); /*0x8226c8*/
  LOBYTE(v56) = 3; /*0x8226d1*/
  v24 = sub_40291D(pvarg: &pvargDest); /*0x8226d5*/
  if ( v24 < 0 ) /*0x8226dd*/
    sub_A5FDE4(a1: v24); /*0x8226e0*/
  VariantInit(&pvarg); /*0x8226e9*/
  v25 = sub_402B8C(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x8226f4*/
  if ( v25 < 0 ) /*0x8226fd*/
    sub_A5FDE4(a1: v25); /*0x822700*/
  LOBYTE(v56) = 14; /*0x822709*/
  VariantInit(pvarg: &pvargDest); /*0x82270d*/
  v26 = sub_402B8C(&pvargDest, pvargSrc: &pvargSrc); /*0x822718*/
  v27 = v46; /*0x822720*/
  if ( v26 < 0 ) /*0x822721*/
    sub_A5FDE4(a1: v26); /*0x822724*/
  v46 = 0; /*0x822729*/
  p_p_p_pvarg = 0; /*0x82272a*/
  p_pvargDest = (int)&pvarg; /*0x82272e*/
  p_pvarg = &pvargDest; /*0x822732*/
  v42 = v27; /*0x822733*/
  v54 = &v42; /*0x822736*/
  LOBYTE(v56) = 15; /*0x82273e*/
  sub_425ADD(a1: "UI/UIWindow.img/Maker/GaugeBar/gauge"); /*0x822742*/
  LOBYTE(v56) = 15; /*0x82274b*/
  v28 = sub_406472(); /*0x82274f*/
  sub_402680(a1: v28); /*0x822756*/
  sub_403A93(pvargDest: &v50, a2: v42, a3: (int)p_pvarg, a4: p_pvargDest); /*0x82275d*/
  LOBYTE(v56) = 17; /*0x822764*/
  p_p_pvarg = (VARIANTARG **)sub_4032B2(a1: p_p_p_pvarg, a2: v46); /*0x82276d*/
  v29 = sub_41E527(a1: &p_p_pvarg); /*0x82277a*/
  if ( v29 < 0 && v29 != -2147467262 ) /*0x822788*/
    sub_A5FDE4(a1: v29); /*0x82278b*/
  LOBYTE(v56) = 15; /*0x822794*/
  v30 = sub_40291D(pvarg: &v50); /*0x822798*/
  if ( v30 < 0 ) /*0x8227a0*/
    sub_A5FDE4(a1: v30); /*0x8227a3*/
  LOBYTE(v56) = 14; /*0x8227ac*/
  v31 = sub_40291D(pvarg: &pvargDest); /*0x8227b0*/
  if ( v31 < 0 ) /*0x8227b8*/
    sub_A5FDE4(a1: v31); /*0x8227bb*/
  LOBYTE(v56) = 3; /*0x8227c4*/
  v32 = sub_40291D(&pvarg); /*0x8227c8*/
  if ( v32 < 0 ) /*0x8227d0*/
    sub_A5FDE4(a1: v32); /*0x8227d3*/
  if ( *(this + 846) == 0 ) /*0x8227de*/
    sub_A5FDE4(a1: -2147467261); /*0x8227e5*/
  v33 = sub_40B920(a1: *(this + 846)); /*0x8227f0*/
  v46 = 1410; /*0x8227ff*/
  *(this + 835) = (v33 - 4) / 5u; /*0x822804*/
  p_p_p_pvarg = (int)&p_p_pvarg; /*0x82280d*/
  sub_79E805(); /*0x82280e*/
  v34 = *(_DWORD *)sub_406276(a1: p_p_p_pvarg, a2: v46); /*0x82281a*/
  LOBYTE(v56) = 18; /*0x822825*/
  sub_463670(a1: v34, a2: this + 848, a3: 0); /*0x822829*/
  LOBYTE(v56) = 3; /*0x822834*/
  sub_40265E(a1: &p_p_pvarg); /*0x822838*/
  VariantInit(&pvarg); /*0x822841*/
  v35 = sub_402B8C(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x82284c*/
  v36 = (VARIANTARG *)v46; /*0x822854*/
  if ( v35 < 0 ) /*0x822855*/
    sub_A5FDE4(a1: v35); /*0x822858*/
  v46 = (int)&pvarg; /*0x822860*/
  p_p_p_pvarg = -7296829; /*0x822861*/
  p_pvargDest = 11; /*0x822866*/
  p_pvarg = v36; /*0x822868*/
  p_p_pvarg = &p_pvarg; /*0x82286b*/
  v42 = 5527; /*0x82286e*/
  LOBYTE(v56) = 19; /*0x822874*/
  sub_79E805(); /*0x822878*/
  sub_406292(a1: &p_pvarg, a2: v42); /*0x82287f*/
  LOBYTE(v56) = 19; /*0x82288a*/
  sub_4284A8(a1: this + 848); /*0x82288e*/
  sub_46341A(a1: p_pvarg, a2: p_pvargDest, a3: p_p_p_pvarg, a4: v46); /*0x822895*/
  LOBYTE(v56) = 3; /*0x82289e*/
  v37 = sub_40291D(&pvarg); /*0x8228a2*/
  if ( v37 < 0 ) /*0x8228aa*/
    sub_A5FDE4(a1: v37); /*0x8228ad*/
  v46 = 1410; /*0x8228b5*/
  p_p_p_pvarg = (int)&p_p_pvarg; /*0x8228ba*/
  sub_79E805(); /*0x8228bb*/
  v38 = *(_DWORD *)sub_406276(a1: p_p_p_pvarg, a2: v46); /*0x8228c7*/
  LOBYTE(v56) = 21; /*0x8228d2*/
  sub_463670(a1: v38, a2: this + 849, a3: 0); /*0x8228d6*/
  LOBYTE(v56) = 3; /*0x8228e1*/
  sub_40265E(a1: &p_p_pvarg); /*0x8228e5*/
  VariantInit(&pvarg); /*0x8228ee*/
  v39 = sub_402B8C(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x8228f9*/
  v40 = (VARIANTARG *)v46; /*0x822901*/
  if ( v39 < 0 ) /*0x822902*/
    sub_A5FDE4(a1: v39); /*0x822905*/
  v46 = (int)&pvarg; /*0x82290d*/
  p_p_p_pvarg = -14069382; /*0x82290e*/
  p_pvargDest = 11; /*0x822913*/
  p_pvarg = v40; /*0x822915*/
  p_p_pvarg = &p_pvarg; /*0x822918*/
  v42 = 5527; /*0x82291b*/
  LOBYTE(v56) = 22; /*0x822921*/
  sub_79E805(); /*0x822925*/
  sub_406292(a1: &p_pvarg, a2: v42); /*0x82292c*/
  LOBYTE(v56) = 22; /*0x822937*/
  sub_4284A8(a1: this + 849); /*0x82293b*/
  sub_46341A(a1: p_pvarg, a2: p_pvargDest, a3: p_p_p_pvarg, a4: v46); /*0x822942*/
  LOBYTE(v56) = 3; /*0x82294b*/
  v41 = sub_40291D(&pvarg); /*0x82294f*/
  if ( v41 < 0 ) /*0x822957*/
    sub_A5FDE4(a1: v41); /*0x82295a*/
  sub_8248DD(a1: this); /*0x822961*/
  sub_8229B8(a1: this); /*0x822968*/
  sub_8234A3(a1: this); /*0x82296f*/
  sub_9E04C9(a1: 0); /*0x822977*/
  *(_DWORD *)sub_801D41(a1: -1) = this; /*0x822991*/
  LOBYTE(v56) = 0; /*0x822993*/
  sub_40265E(a1: &v52); /*0x822996*/
  if ( v49 != 0 ) /*0x8229a0*/
    sub_402EA5(); /*0x8229a2*/
}