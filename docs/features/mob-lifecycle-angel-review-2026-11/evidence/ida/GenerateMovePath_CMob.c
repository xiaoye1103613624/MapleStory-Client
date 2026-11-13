// Angel 083 `GenerateMovePath_CMob` @ 0x66b6fc
// source: E:\MXD\反编译IDA\v83\`Angel.i64 (IDA MCP 13339), read-only
int __thiscall CMob::GenerateMovePath(
        CMob *this,
        int a2,
        int a3,
        unsigned int a4,
        int a5,
        int a6,
        int a7,
        int a8,
        int a9,
        int a10)
{
  int update_time; // eax
  int v12; // esi
  CVecCtrlMob *v13; // esi
  bool v14; // zf
  int v15; // eax
  int v16; // eax
  int v17; // eax
  int v18; // ecx
  int v19; // eax
  int v20; // eax
  int v21; // eax
  int v22; // eax
  int v23; // edx
  int *v24; // eax
  int v25; // eax
  int v26; // eax
  int v27; // eax
  int v28; // edi
  int v29; // esi
  int Foothold; // eax
  int v31; // eax
  int v32; // eax
  int v33; // esi
  int v34; // eax
  int v35; // ecx
  int v36; // eax
  struct IVecCtrlOwner *v37; // eax
  int v38; // eax
  int v39; // eax
  int v40; // eax
  int v41; // eax
  BOOL v42; // ecx
  int v43; // eax
  int v44; // edi
  signed int v45; // eax
  int v46; // eax
  int v47; // esi
  int v48; // eax
  int v49; // eax
  int v50; // eax
  int v52; // [esp+4h] [ebp-68h]
  int v53; // [esp+4h] [ebp-68h]
  int v54; // [esp+4h] [ebp-68h]
  _WORD v55[12]; // [esp+14h] [ebp-58h] BYREF
  int v56; // [esp+2Ch] [ebp-40h] BYREF
  _BYTE v57[12]; // [esp+30h] [ebp-3Ch] BYREF
  int v58; // [esp+3Ch] [ebp-30h]
  struct IVecCtrlOwner *v59; // [esp+40h] [ebp-2Ch]
  struct CVecCtrl *v60; // [esp+44h] [ebp-28h]
  BOOL v61; // [esp+48h] [ebp-24h]
  char v62[4]; // [esp+4Ch] [ebp-20h]
  int v63; // [esp+50h] [ebp-1Ch]
  int v64; // [esp+54h] [ebp-18h]
  CVecCtrlMob *v65; // [esp+58h] [ebp-14h]
  int ActionDelay; // [esp+5Ch] [ebp-10h]
  int v67; // [esp+68h] [ebp-4h]
  __int16 v68; // [esp+78h] [ebp+Ch]
  int v69; // [esp+84h] [ebp+18h]
  bool v70; // [esp+84h] [ebp+18h]
  bool v71; // [esp+88h] [ebp+1Ch]
  CMovePath *v72; // [esp+90h] [ebp+24h]

  v58 = 0;
  update_time = get_update_time();
  v12 = *(this + 71);
  v63 = update_time;
  v60 = *(this + 70) != 0 ? (*(this + 70) - 12) : 0;
  v13 = v12 != 0 ? (v12 - 12) : 0;
  v14 = *(this + 169) == 0;
  v65 = v13;
  if ( v14 || (v15 = *(this + 99)) == 0 )
    v15 = *(this + 98);
  v61 = _ZtlSecureFuse<long>(a1: v15 + 64, a2: *(v15 + 72)) == 3;
  if ( a2 < 0 )
    ActionDelay = 0;
  else
    ActionDelay = CMob::GetActionDelay(this, a2);
  if ( a2 >= 0 )
  {
    v16 = _ZtlSecureFuse<long>(a1: this + 960, a2: *(this + 242));
    if ( (v16 < 12 || v16 > 20) && ((v17 = _ZtlSecureFuse<long>(a1: this + 960, a2: *(this + 242))) < 21 || v17 > 37)
      || (v18 = a2, a2 < 6)
      || a2 > 8
      || a10 != 0 )
    {
      v19 = _ZtlSecureTear<long>(a1: a2, a2: this + 960);
      v52 = *(this + 239);
      *(this + 242) = v19;
      v20 = _ZtlSecureFuse<long>(a1: this + 948, a2: v52);
      *(this + 239) = _ZtlSecureTear<long>(a1: v20 ^ (a3 ^ v20) & 1, a2: this + 948);
      CMob::PrepareActionLayer(this);
      v18 = a2;
    }
    v21 = 1000;
    if ( v18 < 12 || v18 > 20 )
    {
      if ( v18 < 6 || v18 > 8 )
      {
        if ( v18 >= 21 && v18 <= 37 )
        {
          *v62 = _ZtlSecureFuse<unsigned long>(a1: *(this + 98) + 12, a2: *(*(this + 98) + 20));
          v53 = *((*(*(this + 1) + 16))(a1: this + 1) + 4);
          v24 = (*(*(this + 1) + 16))(a1: this + 1);
          v25 = sub_9894F3(a1: *v24, a2: v53);
          sub_989BA1(a1: v62[0], a2: (a2 - 1), a3: v25);
          HIWORD(a4) = ActionDelay;
        }
      }
      else
      {
        if ( ActionDelay > 1000 )
          v21 = ActionDelay;
        *(this + 245) = _ZtlSecureTear<long>(a1: v21 + v63, a2: this + 972);
      }
    }
    else
    {
      v22 = _ZtlSecureFuse<long>(a1: this + 960, a2: *(this + 242));
      CMob::DoAttack(a1: this, a2: v23, a3: v22, a4: a3, a5: a4);
    }
    v59 = *(v13 + 148);
    v58 = *(v13 + 149);
    if ( sub_6691B9(a1: this) != 0 )
      goto LABEL_60;
    if ( *(this + 169) == 0 || (v26 = *(this + 99)) == 0 )
      v26 = *(this + 98);
    v27 = _ZtlSecureFuse<long>(a1: v26 + 64, a2: *(v26 + 72));
    CMovePath::DiscardByInterrupt(this: (v60 + 428), a2: v27, a3: v60, a4: a10);
    ActionDelay -= sub_66BE2D(a1: v60 + 428);
    qmemcpy(v55, v60 + 496, sizeof(v55));
    v28 = *v65;
    v29 = v55[1];
    Foothold = CWvsPhysicalSpace2D::GetFoothold(dwSN: v55[6]);
    (*(v28 + 4))(a1: v65, a2: 1, a3: v29, a4: v55[2], a5: v55[3], a6: v55[4], a7: LOBYTE(v55[5]), a8: Foothold);
    if ( a2 >= 6 && a2 <= 8 )
    {
      *v62 = v63 + ActionDelay;
      v31 = _ZtlSecureTear<long>(a1: v63 + ActionDelay, a2: this + 972);
      v14 = *v62 == 0;
      *(this + 245) = v31;
      if ( v14 )
      {
        v32 = _ZtlSecureFuse<long>(a1: this + 972, a2: v31);
        *(this + 245) = _ZtlSecureTear<long>(a1: v32 + 1, a2: this + 972);
      }
    }
    if ( ActionDelay < 90 )
      goto LABEL_60;
    if ( a2 < 6 || a2 > 8 )
    {
      sub_9BBDFD(a1: ActionDelay, a2: 0, a3: 0, a4: 0, a5: 0, a6: 0, a7: 0, a8: 0);
LABEL_60:
      v37 = v59;
      if ( v59 == 0 )
        v37 = TSingleton<CUserLocal>::ms_pInstance != 0 ? (TSingleton<CUserLocal>::ms_pInstance + 4) : 0;
      CVecCtrlMob::ChaseTargetImp(this: v65, a2: a5, a3: v37, a4: v58);
      v13 = v65;
      goto LABEL_63;
    }
    if ( a6 == 10 )
    {
      sub_9BECC1(a1: dbl_AF82B8);
      *(this + 329) = 1;
      goto LABEL_60;
    }
    if ( a10 != 0 )
    {
      sub_9BECC1(a1: dbl_AF82B0);
      goto LABEL_60;
    }
    v64 = 0;
    v63 = 0;
    *v62 = a3 == 0;
    switch ( a6 )
    {
      case 2:
        v33 = 300;
        v63 = 20000;
        goto LABEL_57;
      case 3:
        v64 = sub_66486E(a1: this) + 45;
        v34 = abs(a1: v29 - a7);
        v35 = v34 >= v64 ? -1000 : 0;
        goto LABEL_49;
      case 4:
        v35 = abs(a1: v29 - a7) >= 100 ? 1000 : 300;
        goto LABEL_49;
      case 5:
        v35 = abs(a1: v29 - a7) >= 100 ? 700 : 300;
        goto LABEL_49;
      case 6:
        *v62 = a9;
        v36 = *(this + 1);
        v33 = 300;
        if ( a9 != 0 )
        {
          if ( *(*(v36 + 16))() >= a7 )
            goto LABEL_58;
        }
        else if ( *(*(v36 + 16))() <= a7 )
        {
          goto LABEL_59;
        }
LABEL_55:
        v33 = 0;
LABEL_57:
        if ( *v62 == 0 )
        {
LABEL_59:
          sub_9BBDFD(a1: ActionDelay, a2: v33, a3: 0, a4: v63, a5: a6, a6: a7, a7: a8, a8: v64);
          goto LABEL_60;
        }
LABEL_58:
        v33 = -v33;
        goto LABEL_59;
      case 7:
        v35 = abs(a1: v29 - a7) >= 100 ? -700 : -300;
LABEL_49:
        v33 = v35;
        goto LABEL_57;
      case 8:
        abs(a1: v29 - a7);
        v33 = 700;
        goto LABEL_57;
      case 9:
        goto LABEL_55;
      default:
        v33 = 130;
        v63 = 40000;
        goto LABEL_57;
    }
  }
LABEL_63:
  v38 = sub_66379D(a1: this);
  v39 = v38 - ActionDelay;
  if ( v39 > 90 )
    sub_9BBDFD(a1: v39, a2: 0, a3: 0, a4: 0, a5: 0, a6: 0, a7: 0, a8: 0);
  v69 = 0;
  v72 = (v13 + 428);
  do
  {
    if ( *(this + 169) == 0 || (v40 = *(this + 99)) == 0 )
      v40 = *(this + 98);
    v41 = _ZtlSecureFuse<long>(a1: v40 + 64, a2: *(v40 + 72));
    if ( CMovePath::IsTimeForFlush(this: v72, a2: v41 == 3) != 0 )
      break;
    CVecCtrl::UpdateActive(this: v13);
    ++v69;
  }
  while ( v69 < 55 );
  v42 = _ZtlSecureFuse<long>(a1: this + 304, a2: *(this + 78)) < 0;
  v70 = v42;
  v71 = *(v13 + 148) != 0 && (v42 || CRand32::Random(a1: dword_BEDD38) % 0x64u < 0x46);
  if ( a2 >= 0 )
    LOBYTE(a2) = (2 * a2) | a3 & 1;
  COutPacket::COutPacket(a1: 188);
  v54 = *(this + 97);
  v67 = 0;
  v43 = _ZtlSecureFuse<unsigned long>(a1: this + 380, a2: v54);
  COutPacket::Encode4(this: &v56, a2: v43);
  v68 = _ZtlSecureFuse<short>(a1: this + 316, a2: *(this + 80)) + 1;
  *(this + 80) = sub_4E80EB(a1: v68, a2: this + 316);
  COutPacket::Encode2(a1: v68);
  COutPacket::Encode1(this: &v56, a2: v71 | (8 * (a10 | (2 * v70))));
  COutPacket::Encode1(this: &v56, a2);
  COutPacket::Encode4(this: &v56, a2: a4);
  v44 = *(v13 + 6);
  v45 = sub_9BE403(this: v13);
  COutPacket::Encode1(this: &v56, a2: (v44 == 0) | (16 * (v45 != 0)));
  COutPacket::Encode4(this: &v56, a2: *(v13 + 288));
  if ( v61 )
    v46 = *(v13 + 313);
  else
    v46 = 16768460;
  COutPacket::Encode4(this: &v56, a2: v46);
  if ( v61 )
    v47 = *(v13 + 314);
  else
    v47 = 16768460;
  COutPacket::Encode4(this: &v56, a2: v47);
  if ( *(this + 169) == 0 || (v48 = *(this + 99)) == 0 )
    v48 = *(this + 98);
  v49 = _ZtlSecureFuse<long>(a1: v48 + 64, a2: *(v48 + 72));
  CMovePath::Flush(this: v72, a2: &v56, a3: v49 == 3, a4: (v60 + 428));
  CClientSocket::SendPacket(this: dword_BE7914, a2: &v56);
  if ( _ZtlSecureFuse<long>(a1: this + 304, a2: *(this + 78)) == -2
    || _ZtlSecureFuse<long>(a1: this + 304, a2: *(this + 78)) == 1 )
  {
    v50 = _ZtlSecureTear<long>(a1: 1, a2: this + 304);
  }
  else
  {
    v50 = _ZtlSecureTear<long>(a1: 2, a2: this + 304);
  }
  v67 = -1;
  *(this + 78) = v50;
  return ZArray<unsigned char>::RemoveAll(a1: v57);
}