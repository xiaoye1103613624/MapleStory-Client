# Angel v83 CUIUserInfo decompiled (extracted from corewindow calibration)

## CUIUserInfo::OnCreate

`c
int __thiscall CUIUserInfo::OnCreate(CUIUserInfo *this, void *a2)
{
  int v3; // ecx
  _DWORD *Instance; // eax
  int v5; // eax
  int *v6; // eax
  int v7; // eax
  int v8; // eax
  int v9; // eax
  int v10; // eax
  int **v11; // eax
  int **v12; // ebx
  int v13; // eax
  int v14; // eax
  int *Layer; // eax
  bool v16; // zf
  int v17; // ecx
  int v18; // eax
  int v19; // eax
  int *v20; // eax
  int v21; // ecx
  int v22; // eax
  int v23; // eax
  int v24; // eax
  int v25; // edi
  int *v26; // esi
  int v27; // eax
  int *v28; // eax
  int v29; // ecx
  int v30; // eax
  int v31; // eax
  int *v32; // esi
  int v33; // eax
  int *v34; // esi
  int v35; // eax
  int v36; // eax
  int v37; // eax
  int v38; // eax
  int v39; // eax
  int v40; // eax
  int v41; // eax
  CUIUserInfo *v42; // esi
  __int64 v43; // rax
  _DWORD *v44; // eax
  __int64 v45; // rax
  _DWORD *v46; // eax
  CUIUserInfo **v47; // eax
  int result; // eax
  int v49; // [esp-14h] [ebp-D0h] BYREF
  __int64 v50; // [esp-10h] [ebp-CCh]
  int v51; // [esp-8h] [ebp-C4h]
  int v52; // [esp-4h] [ebp-C0h] BYREF
  VARIANTARG v53; // [esp+Ch] [ebp-B0h] BYREF
  VARIANTARG pvarg; // [esp+1Ch] [ebp-A0h] BYREF
  VARIANTARG v55; // [esp+2Ch] [ebp-90h] BYREF
  VARIANTARG v56; // [esp+3Ch] [ebp-80h] BYREF
  VARIANTARG v57; // [esp+4Ch] [ebp-70h] BYREF
  VARIANTARG v58; // [esp+5Ch] [ebp-60h] BYREF
  VARIANTARG pvargDest; // [esp+6Ch] [ebp-50h] BYREF
  int Unknown; // [esp+7Ch] [ebp-40h] BYREF
  VARIANTARG v61; // [esp+80h] [ebp-3Ch] BYREF
  VARIANTARG v62; // [esp+90h] [ebp-2Ch] BYREF
  CUIUserInfo *v63; // [esp+A0h] [ebp-1Ch]
  int v64; // [esp+A4h] [ebp-18h]
  int *v65; // [esp+A8h] [ebp-14h] BYREF
  int *v66; // [esp+ACh] [ebp-10h] BYREF
  int v67; // [esp+B8h] [ebp-4h]
  _DWORD *v68; // [esp+C4h] [ebp+8h]
  _DWORD *v69; // [esp+C4h] [ebp+8h]

  v63 = this; /*0x8feccf*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x8fecd2*/
  v67 = 0; /*0x8fece0*/
  sub_402F85(pvargDest: &v55, pvargSrc: &pvargSrc); /*0x8fece3*/
  v52 = 0; /*0x8fece8*/
  v51 = 0; /*0x8fece9*/
  HIDWORD(v50) = &pvargDest; /*0x8feced*/
  LODWORD(v50) = &v55; /*0x8fecf4*/
  v49 = v3; /*0x8fecf5*/
  v66 = &v49; /*0x8fecf8*/
  LOBYTE(v67) = 1; /*0x8fed01*/
  Instance = StringPool::GetInstance(); /*0x8fed05*/
  StringPool::GetBSTR(this: Instance, a2: &v49, a3: SP_4682_UI_UIWINDOWIMG_USERINFO_BOSSPETCROWN); /*0x8fed0c*/
  LOBYTE(v67) = 1; /*0x8fed18*/
  v5 = sub_406472(); /*0x8fed1c*/
  v6 = sub_402680(a1: v5); /*0x8fed23*/
  IWzResMan::GetObjectA(this: v6, pvargDest: &pvarg, sUOL: v49, vParam: v50, vAux: SHIDWORD(v50)); /*0x8fed2a*/
  LOBYTE(v67) = 3; /*0x8fed31*/
  Unknown = Ztl_variant_t::GetUnknown(a1: v51, a2: v52); /*0x8fed3c*/
  v64 = 0; /*0x8fed3f*/
  if ( Unknown != 0 ) /*0x8fed42*/
  {
    v7 = sub_41E527(a1: &Unknown); /*0x8fed4b*/
    if ( v7 < 0 && v7 != -2147467262 ) /*0x8fed59*/
      _com_issue_error(a1: v7); /*0x8fed5c*/
  }
  LOBYTE(v67) = 7; /*0x8fed68*/
  v8 = ZComAPI::ZComVariantCopy(&pvarg); /*0x8fed6c*/
  if ( v8 < 0 ) /*0x8fed74*/
    _com_issue_error(a1: v8); /*0x8fed77*/
  LOBYTE(v67) = 6; /*0x8fed83*/
  v9 = ZComAPI::ZComVariantCopy(pvarg: &v55); /*0x8fed87*/
  if ( v9 < 0 ) /*0x8fed8f*/
    _com_issue_error(a1: v9); /*0x8fed92*/
  LOBYTE(v67) = 5; /*0x8fed9b*/
  v10 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x8fed9f*/
  if ( v10 < 0 ) /*0x8feda7*/
    _com_issue_error(a1: v10); /*0x8fedaa*/
  sub_402F85(pvargDest: &v62, pvargSrc: &pvargSrc); /*0x8fedb3*/
  LOBYTE(v67) = 8; /*0x8fedbe*/
  sub_402FAB(a1: 0, a2: 3); /*0x8fedc2*/
  LOBYTE(v67) = 9; /*0x8fedcd*/
  if ( dword_BF14EC == 0 ) /*0x8fedd1*/
    _com_issue_error(a1: -2147467261); /*0x8fedd8*/
  v11 = IWzGr2D::CreateLayer( /*0x8fedf4*/
          this: dword_BF14EC,
          a2: &v65,
          nLeft: 0,
          nTop: 0,
          uWidth: 0,
          uHeight: 0,
          z: 0,
          vCanvas: &v61,
          dwFilter: &v62);
  v12 = (this + 1780); /*0x8fedfb*/
  sub_428712(a1: *v11); /*0x8fee03*/
  sub_4156C5(a1: &v65); /*0x8fee0b*/
  LOBYTE(v67) = 8; /*0x8fee14*/
  v13 = ZComAPI::ZComVariantCopy(pvarg: &v61); /*0x8fee18*/
  if ( v13 < 0 ) /*0x8fee20*/
    _com_issue_error(a1: v13); /*0x8fee23*/
  LOBYTE(v67) = 5; /*0x8fee2c*/
  v14 = ZComAPI::ZComVariantCopy(pvarg: &v62); /*0x8fee30*/
  if ( v14 < 0 ) /*0x8fee38*/
    _com_issue_error(a1: v14); /*0x8fee3b*/
  Layer = CWnd::GetLayer(a1: &v66); /*0x8fee46*/
  sub_410FDF(this: &v62, a2: *Layer, a3: 1); /*0x8fee53*/
  v16 = *v12 == 0; /*0x8fee58*/
  LOBYTE(v67) = 11; /*0x8fee5a*/
  if ( v16 ) /*0x8fee5e*/
    _com_issue_error(a1: -2147467261); /*0x8fee65*/
  v17 = **v12; /*0x8fee6c*/
  v65 = *v12; /*0x8fee7a*/
  v18 = (*(v17 + 100))(a1: v65, a2: *&v62.vt, a3: v62.decVal.Hi32, a4: v62.lVal, a5: v62.cyVal.Hi); /*0x8fee7e*/
  if ( v18 < 0 ) /*0x8fee83*/
    _com_issue_errorex(a1: v18, a2: v65, a3: dword_BD8348); /*0x8fee8e*/
  LOBYTE(v67) = 10; /*0x8fee97*/
  v19 = ZComAPI::ZComVariantCopy(pvarg: &v62); /*0x8fee9b*/
  if ( v19 < 0 ) /*0x8feea3*/
    _com_issue_error(a1: v19); /*0x8feea6*/
  LOBYTE(v67) = 5; /*0x8feeae*/
  sub_4156C5(a1: &v66); /*0x8feeb2*/
  sub_402F85(pvargDest: &v61, pvargSrc: &pvargSrc); /*0x8feec0*/
  LOBYTE(v67) = 12; /*0x8feec9*/
  sub_402F85(pvargDest: &v62, pvargSrc: &pvargSrc); /*0x8feecd*/
  v16 = *v12 == 0; /*0x8feed2*/
  LOBYTE(v67) = 13; /*0x8feed5*/
  if ( v16 ) /*0x8feed9*/
    _com_issue_error(a1: -2147467261); /*0x8feee0*/
  v20 = *v12; /*0x8feeea*/
  v50 = *&v61.vt; /*0x8feeef*/
  v21 = *v20; /*0x8feef1*/
  v66 = v20; /*0x8fef08*/
  v22 = (*(v21 + 144))( /*0x8fef0c*/
          a1: v20,
          a2: 44,
          a3: 267,
          a4: *&v62.vt,
          a5: v62.decVal.Hi32,
          a6: v62.lVal,
          a7: v62.cyVal.Hi,
          a8: *&v61.vt,
          a9: v61.decVal.Hi32,
          a10: v61.lVal,
          a11: v61.cyVal.Hi);
  if ( v22 < 0 ) /*0x8fef14*/
    _com_issue_errorex(a1: v22, a2: v66, a3: dword_BD8348); /*0x8fef1f*/
  LOBYTE(v67) = 12; /*0x8fef28*/
  v23 = ZComAPI::ZComVariantCopy(pvarg: &v62); /*0x8fef2c*/
  if ( v23 < 0 ) /*0x8fef34*/
    _com_issue_error(a1: v23); /*0x8fef37*/
  LOBYTE(v67) = 5; /*0x8fef40*/
  v24 = ZComAPI::ZComVariantCopy(pvarg: &v61); /*0x8fef44*/
  if ( v24 < 0 ) /*0x8fef4c*/
    _com_issue_error(a1: v24); /*0x8fef4f*/
  if ( *(v63 + 408) != 1 || *(v63 + 417) != 0 ) /*0x8fef67*/
    v25 = 0xFFFFFF; /*0x8fef6e*/
  else
    v25 = -1; /*0x8fef69*/
  if ( *v12 == 0 ) /*0x8fef76*/
    _com_issue_error(a1: -2147467261); /*0x8fef7d*/
  v26 = *v12; /*0x8fef82*/
  v27 = (*(**v12 + 224))(a1: *v12, a2: v25); /*0x8fef88*/
  if ( v27 < 0 ) /*0x8fef90*/
    _com_issue_errorex(a1: v27, a2: v26, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x8fef99*/
  v28 = CWnd::GetLayer(a1: &v65); /*0x8fefa5*/
  sub_410FDF(this: &v62, a2: *v28, a3: 1); /*0x8fefb2*/
  v16 = *v12 == 0; /*0x8fefb7*/
  LOBYTE(v67) = 15; /*0x8fefba*/
  if ( v16 ) /*0x8fefbe*/
    _com_issue_error(a1: -2147467261); /*0x8fefc5*/
  v29 = **v12; /*0x8fefcc*/
  v66 = *v12; /*0x8fefda*/
  v30 = (*(v29 + 252))(a1: v66, a2: *&v62.vt, a3: v62.decVal.Hi32, a4: v62.lVal, a5: v62.cyVal.Hi); /*0x8fefde*/
  if ( v30 < 0 ) /*0x8fefeb*/
    _com_issue_errorex(a1: v30, a2: v66, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x8feff2*/
  LOBYTE(v67) = 14; /*0x8feffb*/
  v31 = ZComAPI::ZComVariantCopy(pvarg: &v62); /*0x8fefff*/
  if ( v31 < 0 ) /*0x8ff007*/
    _com_issue_error(a1: v31); /*0x8ff00a*/
  LOBYTE(v67) = 5; /*0x8ff012*/
  sub_4156C5(a1: &v65); /*0x8ff016*/
  if ( *v12 == 0 ) /*0x8ff01e*/
    _com_issue_error(a1: -2147467261); /*0x8ff025*/
  v32 = *v12; /*0x8ff02a*/
  v33 = (*(**v12 + 180))(a1: *v12, a2: 1); /*0x8ff031*/
  if ( v33 < 0 ) /*0x8ff039*/
    _com_issue_errorex(a1: v33, a2: v32, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x8ff03e*/
  if ( *v12 == 0 ) /*0x8ff046*/
    _com_issue_error(a1: -2147467261); /*0x8ff04d*/
  v34 = *v12; /*0x8ff052*/
  v35 = (*(**v12 + 216))(a1: *v12, a2: 1); /*0x8ff059*/
  if ( v35 < 0 ) /*0x8ff061*/
    _com_issue_errorex(a1: v35, a2: v34, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x8ff066*/
  sub_402F85(pvargDest: &v57, pvargSrc: &pvargSrc); /*0x8ff074*/
  LOBYTE(v67) = 16; /*0x8ff07d*/
  sub_402F85(pvargDest: &v58, pvargSrc: &pvargSrc); /*0x8ff081*/
  LOBYTE(v67) = 17; /*0x8ff08a*/
  sub_402F85(pvargDest: &v56, pvargSrc: &pvargSrc); /*0x8ff08e*/
  LOBYTE(v67) = 18; /*0x8ff097*/
  sub_402F85(pvargDest: &v61, pvargSrc: &pvargSrc); /*0x8ff09b*/
  LOBYTE(v67) = 19; /*0x8ff0a4*/
  sub_402F85(pvargDest: &v62, pvargSrc: &pvargSrc); /*0x8ff0a8*/
  v16 = *v12 == 0; /*0x8ff0af*/
  LOBYTE(v67) = 20; /*0x8ff0b1*/
  if ( v16 ) /*0x8ff0b5*/
    _com_issue_error(a1: -2147467261); /*0x8ff0bc*/
  IWzGr2DLayer::Animate(pvargDest: &v53, a2: v64, a3: &v62, a4: &v61, a5: &v56, a6: &v58, a7: &v57); /*0x8ff0e3*/
  v36 = ZComAPI::ZComVariantCopy(pvarg: &v53); /*0x8ff0ef*/
  if ( v36 < 0 ) /*0x8ff0f7*/
    _com_issue_error(a1: v36); /*0x8ff0fa*/
  LOBYTE(v67) = 19; /*0x8ff103*/
  v37 = ZComAPI::ZComVariantCopy(pvarg: &v62); /*0x8ff107*/
  if ( v37 < 0 ) /*0x8ff10f*/
    _com_issue_error(a1: v37); /*0x8ff112*/
  LOBYTE(v67) = 18; /*0x8ff11b*/
  v38 = ZComAPI::ZComVariantCopy(pvarg: &v61); /*0x8ff11f*/
  if ( v38 < 0 ) /*0x8ff127*/
    _com_issue_error(a1: v38); /*0x8ff12a*/
  LOBYTE(v67) = 17; /*0x8ff133*/
  v39 = ZComAPI::ZComVariantCopy(pvarg: &v56); /*0x8ff137*/
  if ( v39 < 0 ) /*0x8ff13f*/
    _com_issue_error(a1: v39); /*0x8ff142*/
  LOBYTE(v67) = 16; /*0x8ff14b*/
  v40 = ZComAPI::ZComVariantCopy(pvarg: &v58); /*0x8ff14f*/
  if ( v40 < 0 ) /*0x8ff157*/
    _com_issue_error(a1: v40); /*0x8ff15a*/
  LOBYTE(v67) = 5; /*0x8ff163*/
  v41 = ZComAPI::ZComVariantCopy(pvarg: &v57); /*0x8ff167*/
  if ( v41 < 0 ) /*0x8ff16f*/
    _com_issue_error(a1: v41); /*0x8ff172*/
  v42 = v63; /*0x8ff177*/
  CUIUserInfo::SetLayer(this: v63); /*0x8ff17c*/
  sub_8FFE3B(this: v42); /*0x8ff183*/
  v66 = &v52; /*0x8ff18b*/
  v52 = 0; /*0x8ff195*/
  sub_41C06C(a1: &word_AF0E08, a2: -1); /*0x8ff197*/
  CUIWnd::OnCreate(a1: a2, a2: v52); /*0x8ff1a1*/
  if ( *(v42 + 361) != 0 ) /*0x8ff1ae*/
  {
    if ( (*(v42 + 361) & 3) == 1 ) /*0x8ff1b8*/
    {
      if ( *(v42 + 395) == 0 ) /*0x8ff234*/
      {
        v69 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 124); /*0x8ff242*/
        LOBYTE(v67) = 21; /*0x8ff247*/
        if ( v69 != 0 ) /*0x8ff24b*/
        {
          v52 = (*(*(v42 + 1) + 48))(a1: v42 + 4); /*0x8ff258*/
          v45 = (*(*(v42 + 1) + 44))(a1: v42 + 4); /*0x8ff25d*/
          v46 = CUIUserInfoDetail::OnCreate(a1: v69, a2: SHIDWORD(v45), a3: v45 + 270, a4: v52); /*0x8ff269*/
        }
        else
        {
          v46 = 0; /*0x8ff270*/
        }
        LOBYTE(v67) = 5; /*0x8ff279*/
        sub_904E73(a1: v46); /*0x8ff27d*/
      }
      *(v42 + 361) = 1; /*0x8ff285*/
      *(v42 + 451) = 1; /*0x8ff28b*/
    }
    else if ( (*(v42 + 361) & 3) == 2 ) /*0x8ff1bb*/
    {
      if ( *(v42 + 397) == 0 ) /*0x8ff1c7*/
      {
        v68 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1448); /*0x8ff1d8*/
        LOBYTE(v67) = 22; /*0x8ff1dd*/
        if ( v68 != 0 ) /*0x8ff1e1*/
        {
          v52 = (*(*(v42 + 1) + 48))(a1: v42 + 4); /*0x8ff1ee*/
          v43 = (*(*(v42 + 1) + 44))(a1: v42 + 4); /*0x8ff1f3*/
          v44 = CUIUserInfoWishList::OnCreate(a1: v68, a2: SHIDWORD(v43), a3: v43 + 270, a4: v52); /*0x8ff1ff*/
        }
        else
        {
          v44 = 0; /*0x8ff206*/
        }
        LOBYTE(v67) = 5; /*0x8ff20f*/
        sub_904EC1(a1: v44); /*0x8ff213*/
      }
      *(v42 + 361) = 2; /*0x8ff218*/
      *(v42 + 452) = 1; /*0x8ff222*/
    }
  }
  v47 = ZArray<CUIWnd *>::InsertBefore(a1: -1); /*0x8ff29e*/
  v67 = -1; /*0x8ff2a3*/
  *v47 = v42; /*0x8ff2a7*/
  result = v64; /*0x8ff2a9*/
  if ( v64 != 0 ) /*0x8ff2ae*/
    return (*(*v64 + 8))(a1: v64); /*0x8ff2b3*/
  return result; /*0x8ff2b6*/
}
`

## CUIUserInfo::OnButtonClicked

`c
void __thiscall CUIUserInfo::OnButtonClicked(int this, unsigned int a2)
{
  _DWORD *v3; // edi
  int v4; // eax
  const char *v5; // eax
  const char **v6; // esi
  const char *v7; // ecx
  int v8; // ecx
  char v9; // [esp-4h] [ebp-Ch]
  char v10; // [esp-4h] [ebp-Ch]
  int v11; // [esp-4h] [ebp-Ch]

  v3 = *dword_BE7918; /*0x8ff5c1*/
  if ( a2 <= 0x7D7 ) /*0x8ff5c7*/
  {
    if ( a2 == 2007 ) /*0x8ff5cd*/
    {
      if ( *(this + 3116) != 0 ) /*0x8ff6d6*/
        sub_9009BF(this); /*0x8ff6da*/
LABEL_29:
      v11 = 0; /*0x8ff6df*/
    }
    else
    {
      if ( a2 <= 0x7D3 ) /*0x8ff5d8*/
      {
        if ( a2 == 2003 ) /*0x8ff5de*/
        {
          if ( get_field() != 0 ) /*0x8ff67e*/
          {
            get_field(); /*0x8ff68d*/
            CField::SendInviteTradingRoomMsg(a1: this + 1640, a2: 0); /*0x8ff694*/
          }
          return; /*0x8ff699*/
        }
        if ( a2 == 1000 ) /*0x8ff5ed*/
        {
          CWvsContext::UI_Close(this: *dword_BE7918, a2: 10); /*0x8ff66d*/
          return; /*0x8ff672*/
        }
        if ( a2 == 2000 ) /*0x8ff5f1*/
        {
          if ( *(this + 1580) == 0 ) /*0x8ff655*/
          {
            v9 = 1; /*0x8ff665*/
            goto LABEL_17; /*0x8ff667*/
          }
        }
        else
        {
          if ( a2 != 2001 ) /*0x8ff5f4*/
          {
            if ( a2 == 2002 ) /*0x8ff5f7*/
            {
              if ( *(*dword_BE7918 + 11988) != 0 /*0x8ff623*/
                && *(*dword_BE7918 + 12166) == *(*dword_BE7918 + 8352)
                && get_field() != 0 )
              {
                get_field(); /*0x8ff630*/
                CField::SendJoinPartyMsg(a1: this + 1640); /*0x8ff637*/
              }
            }
            else
            {
LABEL_49:
              CUIWnd::OnButtonClicked(a1: a2); /*0x8ff7a2*/
            }
            return; /*0x8ff63c*/
          }
          if ( *(this + 1588) == 0 ) /*0x8ff648*/
          {
            v9 = 2; /*0x8ff64a*/
LABEL_17:
            sub_900BD9(this, a2: v9); /*0x8ff659*/
            return; /*0x8ff660*/
          }
        }
        v9 = 0; /*0x8ff657*/
        goto LABEL_17; /*0x8ff657*/
      }
      if ( a2 == 2004 ) /*0x8ff6a6*/
      {
        v10 = 1; /*0x8ff6cb*/
        goto LABEL_25; /*0x8ff6cd*/
      }
      if ( a2 == 2005 ) /*0x8ff6a9*/
      {
        v10 = 0; /*0x8ff6b7*/
LABEL_25:
        CWvsContext::SendGivePopularityRequest(this: *dword_BE7918, sName: *(this + 1640), bInc: v10); /*0x8ff6b9*/
        return; /*0x8ff6c6*/
      }
LABEL_47:
      v11 = 1; /*0x8ff797*/
    }
LABEL_48:
    CUIUserInfo::SetState(this, a2: v11); /*0x8ff799*/
    return; /*0x8ff7a0*/
  }
  switch ( a2 ) /*0x8ff6f5*/
  {
    case 2008u: /*0x8ff6f5*/
      v11 = 2; /*0x8ff762*/
      goto LABEL_48; /*0x8ff764*/
    case 2009u: /*0x8ff6f5*/
    case 2011u: /*0x8ff6f5*/
      goto LABEL_29;
    case 2010u: /*0x8ff6f5*/
      v11 = 3; /*0x8ff766*/
      goto LABEL_48; /*0x8ff768*/
    case 2012u: /*0x8ff6f5*/
      sub_9009BF(this); /*0x8ff76c*/
      break; /*0x8ff771*/
    case 2013u: /*0x8ff6f5*/
    case 2014u: /*0x8ff6f5*/
    case 2015u: /*0x8ff6f5*/
      *(this + 1668) = a2 - 2013; /*0x8ff778*/
      v8 = *(this + 1596); /*0x8ff77e*/
      if ( v8 != 0 && *(v8 + 164) != a2 - 2013 ) /*0x8ff78e*/
        sub_9009BF(this); /*0x8ff792*/
      goto LABEL_47; /*0x8ff792*/
    case 2016u: /*0x8ff6f5*/
      v4 = *(*dword_BE7918 + 8376); /*0x8ff6fc*/
      if ( v4 != 0 ) /*0x8ff704*/
        v5 = (v4 + 4); /*0x8ff706*/
      else
        v5 = 0; /*0x8ff70b*/
      v6 = (this + 1640); /*0x8ff713*/
      if ( v5 == 0 ) /*0x8ff71b*/
        v5 = dword_BF6B44; /*0x8ff71d*/
      v7 = *v6; /*0x8ff71f*/
      if ( *v6 == 0 ) /*0x8ff723*/
        v7 = dword_BF6B44; /*0x8ff725*/
      if ( strcmp(a1: v7, a2: v5) == 0 ) /*0x8ff734*/
      {
        CWvsContext::UI_Toggle(this: v3, a2: 27, a3: -1); /*0x8ff73a*/
      }
      else
      {
        CWvsContext::UI_Open(this: v3, a2: 28, a3: -1); /*0x8ff745*/
        ZXString<char>::operator=(a1: v6); /*0x8ff754*/
        sub_A34040(a1: 0); /*0x8ff75b*/
      }
      break; /*0x8ff73f*/
    default:
      goto LABEL_49;
  }
}
`

## CUIUserInfo::SetState

`c
void __thiscall CUIUserInfo::SetState(CUIUserInfo *this, int a2)
{
  int v3; // eax
  int v4; // [esp-8h] [ebp-10h]

  *(this + 408) = a2; /*0x8ff802*/
  CUIUserInfo::SetLayer(this); /*0x8ff808*/
  sub_8FFE3B(this); /*0x8ff80f*/
  if ( *(this + 399) != 0 )
  {
    v4 = (*(this + 408) != 0 ? 196 : 34) + (*(*(this + 1) + 48))(a1: this + 4);
    v3 = (*(*(this + 1) + 44))(a1: this + 4); /*0x8ff844*/
    sub_49254B(a1: v3 + 270, a2: v4); /*0x8ff84f*/
  }
  CWnd::InvalidateRect(this, a2: 0); /*0x8ff859*/
}
`

## CUIUserInfo::SetLayer

`c
int __thiscall CUIUserInfo::SetLayer(CUIUserInfo *this)
{
  int v2; // esi
  int v3; // eax
  _DWORD **Layer; // eax
  bool v5; // zf
  int v6; // eax
  ULONG Hi; // ecx
  int v8; // eax
  int v9; // eax
  int v10; // eax
  _DWORD *Instance; // eax
  CAvatar *v12; // ecx
  int v13; // esi
  int v14; // eax
  _DWORD *v15; // eax
  CAvatar *v16; // ecx
  int *v17; // edi
  int v18; // esi
  int v19; // eax
  _DWORD *v20; // eax
  int *v21; // esi
  int v22; // eax
  int v23; // eax
  int *v24; // esi
  int v25; // eax
  int v26; // eax
  CAvatar *v27; // ecx
  _DWORD *v28; // eax
  int *v29; // edi
  int v30; // esi
  int v31; // eax
  _DWORD *v32; // eax
  int v33; // eax
  int v34; // ecx
  int v35; // edx
  int v36; // eax
  int v37; // eax
  int v38; // eax
  int v39; // edi
  int v40; // eax
  int v41; // edi
  int v42; // eax
  int v43; // eax
  int v44; // eax
  int v45; // eax
  int v46; // eax
  int v47; // eax
  int v48; // eax
  int result; // eax
  int v50; // [esp-28h] [ebp-C0h]
  ULONG Hi32; // [esp-Ch] [ebp-A4h] BYREF
  CY cyVal; // [esp-8h] [ebp-A0h]
  VARIANTARG v53; // [esp+Ch] [ebp-8Ch] BYREF
  VARIANTARG v54; // [esp+1Ch] [ebp-7Ch] BYREF
  VARIANTARG v55; // [esp+2Ch] [ebp-6Ch] BYREF
  VARIANTARG v56; // [esp+3Ch] [ebp-5Ch] BYREF
  VARIANTARG v57; // [esp+4Ch] [ebp-4Ch] BYREF
  VARIANTARG pvargDest; // [esp+5Ch] [ebp-3Ch] BYREF
  VARIANTARG pvarg; // [esp+6Ch] [ebp-2Ch] BYREF
  int v60; // [esp+7Ch] [ebp-1Ch] BYREF
  int i; // [esp+80h] [ebp-18h] BYREF
  int v62; // [esp+84h] [ebp-14h] BYREF
  int v63; // [esp+88h] [ebp-10h] BYREF
  int v64; // [esp+94h] [ebp-4h]

  if ( *(this + 408) != 1 || *(this + 417) != 0 ) /*0x8ff889*/
    i = 0xFFFFFF; /*0x8ff893*/
  else
    i = -1; /*0x8ff88b*/
  if ( *(this + 445) == 0 ) /*0x8ff8a0*/
    _com_issue_error(a1: -2147467261); /*0x8ff8a7*/
  v2 = *(this + 445); /*0x8ff8ac*/
  v3 = (*(*v2 + 224))(a1: v2, a2: i); /*0x8ff8b8*/
  if ( v3 < 0 ) /*0x8ff8c0*/
    _com_issue_errorex(a1: v3, a2: v2, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x8ff8c9*/
  sub_402FAB(a1: -2, a2: 3); /*0x8ff8d5*/
  v64 = 0; /*0x8ff8e0*/
  Layer = CWnd::GetLayer(a1: &v62); /*0x8ff8e3*/
  v5 = *Layer == 0; /*0x8ff8ea*/
  LOBYTE(v64) = 1; /*0x8ff8ec*/
  if ( v5 ) /*0x8ff8f0*/
    _com_issue_error(a1: -2147467261); /*0x8ff8f7*/
  IWzGr2DLayer::RemoveCanvas(this: *Layer, a2: &i, a3: &pvarg); /*0x8ff908*/
  sub_4156AB(a1: &i); /*0x8ff910*/
  sub_4156C5(a1: &v62); /*0x8ff918*/
  v64 = -1; /*0x8ff91d*/
  v6 = ZComAPI::ZComVariantCopy(&pvarg); /*0x8ff925*/
  Hi = cyVal.Hi; /*0x8ff92c*/
  if ( v6 < 0 ) /*0x8ff92d*/
    _com_issue_error(a1: v6); /*0x8ff930*/
  v8 = *(this + 408); /*0x8ff935*/
  if ( v8 != 0 )
  {
    v9 = v8 - 1; /*0x8ff943*/
    if ( v9 != 0 ) /*0x8ff944*/
    {
      v10 = v9 - 1; /*0x8ff94a*/
      if ( v10 != 0 ) /*0x8ff94b*/
      {
        if ( v10 == 1 ) /*0x8ff952*/
        {
          cyVal.int64 = 0; /*0x8ff959*/
          Hi32 = Hi; /*0x8ff95a*/
          i = &Hi32; /*0x8ff95d*/
          *(this + 9) = 275; /*0x8ff966*/
          *(this + 10) = 404; /*0x8ff96d*/
          Instance = StringPool::GetInstance(); /*0x8ff974*/
          StringPool::GetBSTR(this: Instance, a2: &Hi32, a3: SP_2057_UI_UIWINDOWIMG_USERINFO_BACKGRND7); /*0x8ff97b*/
          CWnd::SetBackgrnd(a1: Hi32, a2: cyVal.Lo, a3: cyVal.Hi); /*0x8ff982*/
          v12 = *(this + 407); /*0x8ff987*/
          if ( v12 != 0 ) /*0x8ff98f*/
            CAvatar::SetVisibleTamingMob(this: v12, a2: 0); /*0x8ff992*/
          v62 = this + 1784; /*0x8ff99d*/
          for ( i = 3; i != 0; --i ) /*0x8ff9a0*/
          {
            v13 = *v62; /*0x8ff9aa*/
            if ( *v62 != 0 ) /*0x8ff9ae*/
            {
              v14 = (*(*v13 + 224))(a1: v13, a2: 0xFFFFFF); /*0x8ff9b8*/
              if ( v14 < 0 ) /*0x8ff9c0*/
                _com_issue_errorex(a1: v14, a2: v13, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x8ff9c9*/
            }
            v62 += 4; /*0x8ff9ce*/
          }
        }
      }
      else
      {
        cyVal.int64 = 0; /*0x8ff9dd*/
        Hi32 = Hi; /*0x8ff9de*/
        i = &Hi32; /*0x8ff9e1*/
        *(this + 9) = 275; /*0x8ff9ea*/
        *(this + 10) = 362; /*0x8ff9f1*/
        v15 = StringPool::GetInstance(); /*0x8ff9f8*/
        StringPool::GetBSTR(this: v15, a2: &Hi32, a3: SP_2055_UI_UIWINDOWIMG_USERINFO_BACKGRND5); /*0x8ff9ff*/
        CWnd::SetBackgrnd(a1: Hi32, a2: cyVal.Lo, a3: cyVal.Hi); /*0x8ffa06*/
        v16 = *(this + 407); /*0x8ffa0b*/
        if ( v16 != 0 ) /*0x8ffa13*/
          CAvatar::SetVisibleTamingMob(this: v16, a2: 1); /*0x8ffa17*/
        v17 = (this + 1784); /*0x8ffa1c*/
        for ( i = 3; i != 0; --i ) /*0x8ffa22*/
        {
          v18 = *v17; /*0x8ffa29*/
          if ( *v17 != 0 ) /*0x8ffa2d*/
          {
            v19 = (*(*v18 + 224))(a1: v18, a2: 0xFFFFFF); /*0x8ffa37*/
            if ( v19 < 0 ) /*0x8ffa3f*/
              _com_issue_errorex(a1: v19, a2: v18, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x8ffa48*/
          }
          ++v17; /*0x8ffa4d*/
        }
      }
      goto LABEL_59; /*0x8ff9d5*/
    }
    cyVal.int64 = 0; /*0x8ffa5b*/
    Hi32 = Hi; /*0x8ffa5c*/
    i = &Hi32; /*0x8ffa5f*/
    *(this + 9) = 275; /*0x8ffa68*/
    *(this + 10) = 400; /*0x8ffa6f*/
    v20 = StringPool::GetInstance(); /*0x8ffa76*/
    StringPool::GetBSTR(this: v20, a2: &Hi32, a3: SP_2052_UI_UIWINDOWIMG_USERINFO_BACKGRND2); /*0x8ffa7d*/
    CWnd::SetBackgrnd(a1: Hi32, a2: cyVal.Lo, a3: cyVal.Hi); /*0x8ffa84*/
    v62 = 0; /*0x8ffa8f*/
    i = this + 1784; /*0x8ffa92*/
    do
    {
      v21 = *i; /*0x8ffa98*/
      if ( *i != 0 )
      {
        v22 = *v21; /*0x8ffaa7*/
        cyVal.Hi = v62 == *(this + 417) ? -1 : 0xFFFFFF;
        v23 = (*(v22 + 224))(a1: v21, a2: cyVal.Hi); /*0x8ffab5*/
        if ( v23 < 0 ) /*0x8ffabd*/
          _com_issue_errorex(a1: v23, a2: v21, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x8ffac6*/
      }
      ++v62; /*0x8ffacb*/
      i += 4; /*0x8fface*/
    }
    while ( v62 < 3 );
    if ( *(this + 417) != 0 ) /*0x8ffade*/
    {
      if ( *(this + 445) == 0 ) /*0x8ffb04*/
        _com_issue_error(a1: -2147467261); /*0x8ffb0b*/
      v24 = *(this + 445); /*0x8ffb10*/
      v25 = *v24; /*0x8ffb16*/
      cyVal.Hi = 0xFFFFFF; /*0x8ffb18*/
    }
    else
    {
      if ( *(this + 445) == 0 ) /*0x8ffae6*/
        _com_issue_error(a1: -2147467261); /*0x8ffaed*/
      v24 = *(this + 445); /*0x8ffaf2*/
      v25 = *v24; /*0x8ffaf8*/
      cyVal.Hi = -1; /*0x8ffafa*/
    }
    v26 = (*(v25 + 224))(a1: v24, a2: cyVal.Hi); /*0x8ffb1e*/
    if ( v26 < 0 ) /*0x8ffb26*/
      _com_issue_errorex(a1: v26, a2: v24, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x8ffb2f*/
    v27 = *(this + 407); /*0x8ffb34*/
    if ( v27 != 0 ) /*0x8ffb3c*/
LABEL_58:
      CAvatar::SetVisibleTamingMob(this: v27, a2: 0); /*0x8ffbbc*/
  }
  else
  {
    cyVal.int64 = 0; /*0x8ffb4b*/
    Hi32 = Hi; /*0x8ffb4c*/
    i = &Hi32; /*0x8ffb4f*/
    *(this + 9) = 275; /*0x8ffb58*/
    *(this + 10) = 199; /*0x8ffb5f*/
    v28 = StringPool::GetInstance(); /*0x8ffb66*/
    StringPool::GetBSTR(this: v28, a2: &Hi32, a3: SP_2051_UI_UIWINDOWIMG_USERINFO_BACKGRND); /*0x8ffb6d*/
    CWnd::SetBackgrnd(a1: Hi32, a2: cyVal.Lo, a3: cyVal.Hi); /*0x8ffb74*/
    v29 = (this + 1784); /*0x8ffb79*/
    for ( i = 3; i != 0; --i ) /*0x8ffb7f*/
    {
      v30 = *v29; /*0x8ffb86*/
      if ( *v29 != 0 ) /*0x8ffb8a*/
      {
        v31 = (*(*v30 + 224))(a1: v30, a2: 0xFFFFFF); /*0x8ffb94*/
        if ( v31 < 0 ) /*0x8ffb9c*/
          _com_issue_errorex(a1: v31, a2: v30, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x8ffba5*/
      }
      ++v29; /*0x8ffbaa*/
    }
    v27 = *(this + 407); /*0x8ffbb2*/
    if ( v27 != 0 ) /*0x8ffbba*/
      goto LABEL_58; /*0x8ffbba*/
  }
LABEL_59:
  v63 = 0; /*0x8ffbc5*/
  cyVal.Hi = SP_956_CANVAS; /*0x8ffbcb*/
  cyVal.Lo = &i; /*0x8ffbd0*/
  v64 = 2; /*0x8ffbd1*/
  v32 = StringPool::GetInstance(); /*0x8ffbd8*/
  v33 = *StringPool::GetStringW(this: v32, a2: cyVal.Lo, a3: cyVal.Hi); /*0x8ffbe4*/
  LOBYTE(v64) = 3; /*0x8ffbec*/
  PcCreateObject::IWzCanvas(a1: v33, a2: &v63, a3: 0); /*0x8ffbf0*/
  LOBYTE(v64) = 2; /*0x8ffbfb*/
  ZXString<unsigned short>::_Release(a1: &i); /*0x8ffbff*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x8ffc0d*/
  LOBYTE(v64) = 4; /*0x8ffc16*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x8ffc1a*/
  LOBYTE(v64) = 5; /*0x8ffc24*/
  if ( v63 == 0 ) /*0x8ffc28*/
    _com_issue_error(a1: -2147467261); /*0x8ffc2f*/
  v34 = *(this + 10); /*0x8ffc3a*/
  Hi32 = pvargDest.decVal.Hi32; /*0x8ffc42*/
  v35 = *v63; /*0x8ffc44*/
  cyVal = pvargDest.cyVal; /*0x8ffc46*/
  v50 = *(this + 9); /*0x8ffc53*/
  i = v63; /*0x8ffc58*/
  v36 = (*(v35 + 44))( /*0x8ffc5c*/
          a1: v63,
          a2: v50,
          a3: v34,
          a4: *&pvarg.vt,
          a5: pvarg.decVal.Hi32,
          a6: pvarg.lVal,
          a7: pvarg.cyVal.Hi,
          a8: *&pvargDest.vt,
          a9: pvargDest.decVal.Hi32,
          a10: pvargDest.lVal,
          a11: pvargDest.cyVal.Hi);
  if ( v36 < 0 ) /*0x8ffc66*/
    _com_issue_errorex(a1: v36, a2: i, a3: _GUID_7600dc6c_9328_4bff_9624_5b0f5c01179e); /*0x8ffc6d*/
  LOBYTE(v64) = 4; /*0x8ffc76*/
  v37 = ZComAPI::ZComVariantCopy(&pvarg); /*0x8ffc7a*/
  if ( v37 < 0 ) /*0x8ffc82*/
    _com_issue_error(a1: v37); /*0x8ffc85*/
  LOBYTE(v64) = 2; /*0x8ffc8e*/
  v38 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x8ffc92*/
  if ( v38 < 0 ) /*0x8ffc9a*/
    _com_issue_error(a1: v38); /*0x8ffc9d*/
  if ( v63 == 0 ) /*0x8ffca6*/
    _com_issue_error(a1: -2147467261); /*0x8ffcad*/
  v39 = v63; /*0x8ffcb2*/
  v40 = (*(*v63 + 112))(a1: v63, a2: 0); /*0x8ffcba*/
  if ( v40 < 0 ) /*0x8ffcbf*/
    _com_issue_errorex(a1: v40, a2: v39, a3: _GUID_7600dc6c_9328_4bff_9624_5b0f5c01179e); /*0x8ffcc4*/
  if ( v63 == 0 ) /*0x8ffccd*/
    _com_issue_error(a1: -2147467261); /*0x8ffcd4*/
  v41 = v63; /*0x8ffcd9*/
  v42 = (*(*v63 + 120))(a1: v63, a2: 0); /*0x8ffce1*/
  if ( v42 < 0 ) /*0x8ffce6*/
    _com_issue_errorex(a1: v42, a2: v41, a3: _GUID_7600dc6c_9328_4bff_9624_5b0f5c01179e); /*0x8ffceb*/
  sub_402F85(pvargDest: &v54, pvargSrc: &pvargSrc); /*0x8ffcf9*/
  LOBYTE(v64) = 6; /*0x8ffd02*/
  sub_402F85(pvargDest: &v55, pvargSrc: &pvargSrc); /*0x8ffd06*/
  LOBYTE(v64) = 7; /*0x8ffd0f*/
  sub_402F85(pvargDest: &v56, pvargSrc: &pvargSrc); /*0x8ffd13*/
  LOBYTE(v64) = 8; /*0x8ffd1c*/
  sub_402F85(pvargDest: &v57, pvargSrc: &pvargSrc); /*0x8ffd20*/
  LOBYTE(v64) = 9; /*0x8ffd29*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x8ffd2d*/
  LOBYTE(v64) = 10; /*0x8ffd38*/
  v5 = *CWnd::GetLayer(a1: &v60) == 0; /*0x8ffd43*/
  LOBYTE(v64) = 11; /*0x8ffd46*/
  if ( v5 ) /*0x8ffd4a*/
    _com_issue_error(a1: -2147467261); /*0x8ffd51*/
  IWzGr2DLayer::Animate(pvargDest: &v53, a2: v63, a3: &pvargDest, a4: &v57, a5: &v56, a6: &v55, a7: &v54); /*0x8ffd76*/
  v43 = ZComAPI::ZComVariantCopy(pvarg: &v53); /*0x8ffd82*/
  if ( v43 < 0 ) /*0x8ffd8a*/
    _com_issue_error(a1: v43); /*0x8ffd8d*/
  LOBYTE(v64) = 10; /*0x8ffd97*/
  if ( v60 != 0 ) /*0x8ffd9b*/
    (*(*v60 + 8))(a1: v60); /*0x8ffda0*/
  LOBYTE(v64) = 9; /*0x8ffda7*/
  v44 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x8ffdab*/
  if ( v44 < 0 ) /*0x8ffdb3*/
    _com_issue_error(a1: v44); /*0x8ffdb6*/
  LOBYTE(v64) = 8; /*0x8ffdbf*/
  v45 = ZComAPI::ZComVariantCopy(pvarg: &v57); /*0x8ffdc3*/
  if ( v45 < 0 ) /*0x8ffdcb*/
    _com_issue_error(a1: v45); /*0x8ffdce*/
  LOBYTE(v64) = 7; /*0x8ffdd7*/
  v46 = ZComAPI::ZComVariantCopy(pvarg: &v56); /*0x8ffddb*/
  if ( v46 < 0 ) /*0x8ffde3*/
    _com_issue_error(a1: v46); /*0x8ffde6*/
  LOBYTE(v64) = 6; /*0x8ffdef*/
  v47 = ZComAPI::ZComVariantCopy(pvarg: &v55); /*0x8ffdf3*/
  if ( v47 < 0 ) /*0x8ffdfb*/
    _com_issue_error(a1: v47); /*0x8ffdfe*/
  LOBYTE(v64) = 2; /*0x8ffe07*/
  v48 = ZComAPI::ZComVariantCopy(pvarg: &v54); /*0x8ffe0b*/
  if ( v48 < 0 ) /*0x8ffe13*/
    _com_issue_error(a1: v48); /*0x8ffe16*/
  result = v63; /*0x8ffe1b*/
  v64 = -1; /*0x8ffe1e*/
  if ( v63 != 0 ) /*0x8ffe24*/
    return (*(*v63 + 8))(a1: v63); /*0x8ffe29*/
  return result; /*0x8ffe2c*/
}
`

## CUIUserInfo::Update

`c
int __thiscall CUIUserInfo::Update(CUIUserInfo *this)
{
  int v2; // edx
  CAvatar *v3; // ecx
  CAvatar *v4; // ecx
  int result; // eax
  bool v6; // sf
  int v7; // ecx
  int v8; // ecx
  int v9; // ecx

  CWnd::Update(this); /*0x9010bd*/
  v3 = *(this + 405); /*0x9010c2*/
  if ( v3 != 0 ) /*0x9010ca*/
    CAvatar::Update(this: v3, a2: v2); /*0x9010cc*/
  v4 = *(this + 407); /*0x9010d1*/
  if ( v4 != 0 ) /*0x9010d9*/
    CAvatar::Update(this: v4, a2: v2); /*0x9010db*/
  result = *(this + 361); /*0x9010e0*/
  if ( result == 0 && *(this + 451) == 0 && *(this + 452) == 0 ) /*0x9010f8*/
  {
    v6 = *(this + 453) - 30 < 0; /*0x9010fa*/
    *(this + 453) -= 30; /*0x9010fa*/
    if ( v6 ) /*0x901101*/
    {
      v7 = *(this + 395); /*0x901103*/
      if ( v7 != 0 ) /*0x90110b*/
      {
        CWnd::Destroy(a1: v7); /*0x90110d*/
        result = sub_904F81(a1: this + 1576); /*0x901118*/
      }
      goto LABEL_19; /*0x90111d*/
    }
  }
  if ( result == 1 || *(this + 451) != 0 || (v6 = *(this + 453) - 30 < 0, *(this + 453) -= 30, !v6) ) /*0x901134*/
  {
    if ( result != 2 && *(this + 452) == 0 ) /*0x90115e*/
    {
      v6 = *(this + 453) - 30 < 0; /*0x901160*/
      *(this + 453) -= 30; /*0x901160*/
      if ( v6 ) /*0x901167*/
      {
LABEL_19:
        v9 = *(this + 397); /*0x901169*/
        if ( v9 != 0 ) /*0x901171*/
        {
          CWnd::Destroy(a1: v9); /*0x901173*/
          return sub_904F97(a1: this + 1584); /*0x90117e*/
        }
      }
    }
  }
  else
  {
    v8 = *(this + 395); /*0x901136*/
    if ( v8 != 0 ) /*0x90113e*/
    {
      CWnd::Destroy(a1: v8); /*0x901140*/
      return sub_904F81(a1: this + 1576); /*0x90114b*/
    }
  }
  return result; /*0x901150*/
}
`

## CUIUserInfo::Draw

`c
void __thiscall CUIUserInfo::Draw(CUIUserInfoDetail *this, const struct tagRECT *a2)
{
  CUIUserInfoDetail *v2; // esi
  int *basic_font; // eax
  int v4; // eax
  int v5; // ecx
  int v6; // eax
  _DWORD *v7; // ecx
  int *v8; // eax
  _DWORD *v9; // eax
  int v10; // eax
  int v11; // eax
  int v12; // eax
  _DWORD *Instance; // eax
  char *v14; // eax
  int *v15; // eax
  int v16; // eax
  int v17; // ecx
  _DWORD *v18; // eax
  int v19; // eax
  int v20; // eax
  char *job_name; // eax
  int *v22; // eax
  int v23; // eax
  int v24; // ecx
  _DWORD *v25; // eax
  int v26; // eax
  int v27; // eax
  _DWORD *v28; // eax
  char *v29; // eax
  int *v30; // eax
  int v31; // eax
  int v32; // ecx
  _DWORD *v33; // eax
  int v34; // eax
  int v35; // eax
  unsigned int v36; // ecx
  int *v37; // eax
  int v38; // eax
  int v39; // ecx
  _DWORD *v40; // eax
  int v41; // eax
  int v42; // eax
  int v43; // eax
  int v44; // ecx
  _DWORD *v45; // eax
  int v46; // eax
  int *v47; // eax
  int v48; // eax
  int v49; // eax
  int v50; // eax
  int v51; // eax
  int *v52; // eax
  int v53; // eax
  int v54; // ecx
  int v55; // ecx
  int v56; // eax
  _DWORD *v57; // ecx
  int *v58; // eax
  _DWORD *v59; // eax
  int v60; // eax
  int v61; // eax
  int v62; // eax
  int *v63; // eax
  int v64; // eax
  int v65; // ecx
  _DWORD *v66; // eax
  int v67; // eax
  int v68; // eax
  _DWORD *v69; // eax
  char *v70; // eax
  int *v71; // eax
  int v72; // eax
  int v73; // ecx
  _DWORD *v74; // eax
  int v75; // eax
  int v76; // eax
  _DWORD *v77; // eax
  char *v78; // eax
  int *v79; // eax
  int v80; // eax
  int v81; // ecx
  _DWORD *v82; // eax
  int v83; // eax
  int v84; // eax
  _DWORD *v85; // eax
  char *v86; // eax
  int *v87; // eax
  int v88; // eax
  int v89; // ecx
  _DWORD *v90; // eax
  int v91; // eax
  int v92; // eax
  int v93; // edx
  int v94; // eax
  int v95; // ecx
  int v96; // eax
  int v97; // eax
  int v98; // eax
  int v99; // ecx
  int ItemName; // eax
  unsigned int v101; // ecx
  int *v102; // eax
  int v103; // eax
  int v104; // ecx
  int v105; // edx
  int v106; // eax
  int v107; // eax
  _DWORD *v108; // eax
  int v109; // eax
  int v110; // eax
  int v111; // ecx
  int v112; // eax
  int v113; // eax
  CUIUserInfoDetail *v114; // esi
  unsigned int v115; // ecx
  _DWORD *v116; // eax
  VARIANTARG *Item; // edi
  _DWORD *v118; // eax
  char *v119; // eax
  int v120; // eax
  int *v121; // eax
  int v122; // eax
  int v123; // ecx
  int v124; // edx
  int v125; // eax
  int v126; // eax
  _DWORD *v127; // eax
  int v128; // eax
  int v129; // eax
  int v130; // ecx
  _DWORD *v131; // eax
  int v132; // eax
  int *v133; // eax
  int v134; // eax
  int v135; // eax
  int v136; // eax
  int v137; // eax
  int *v138; // eax
  int v139; // eax
  int v140; // ecx
  int v141; // eax
  _DWORD *v142; // ecx
  int *v143; // eax
  _DWORD *v144; // eax
  int v145; // eax
  int v146; // eax
  int v147; // eax
  int *v148; // eax
  int v149; // eax
  int v150; // ecx
  _DWORD *v151; // eax
  int v152; // eax
  int v153; // eax
  _DWORD *v154; // eax
  _DWORD *v155; // eax
  int *v156; // eax
  int v157; // eax
  int v158; // ecx
  _DWORD *v159; // eax
  int v160; // eax
  int v161; // eax
  _DWORD *v162; // eax
  _DWORD *v163; // eax
  int *v164; // eax
  int v165; // eax
  int v166; // ecx
  _DWORD *v167; // eax
  int v168; // eax
  int v169; // eax
  _DWORD *v170; // eax
  _DWORD *v171; // eax
  int *v172; // eax
  int v173; // eax
  int v174; // ecx
  _DWORD *v175; // eax
  int v176; // eax
  int v177; // eax
  int v178; // edx
  int v179; // eax
  int v180; // ecx
  int v181; // eax
  int v182; // eax
  int v183; // eax
  int v184; // ecx
  int v185; // eax
  unsigned int v186; // ecx
  int v187; // eax
  int v188; // eax
  int *v189; // eax
  int v190; // eax
  int v191; // ecx
  int v192; // edx
  int v193; // eax
  int v194; // eax
  _DWORD *v195; // eax
  int v196; // eax
  int v197; // eax
  int v198; // eax
  int v199; // ecx
  int v200; // eax
  int v201; // eax
  unsigned int v202; // ecx
  _DWORD *v203; // eax
  VARIANTARG *v204; // esi
  _DWORD *v205; // eax
  _DWORD *v206; // edi
  char *v207; // eax
  int v208; // eax
  int v209; // eax
  int v210; // eax
  int *v211; // eax
  int v212; // eax
  int v213; // ecx
  int v214; // edx
  int v215; // eax
  int v216; // eax
  _DWORD *v217; // eax
  int v218; // eax
  int v219; // eax
  int v220; // ecx
  int v221; // esi
  _DWORD *v222; // eax
  _DWORD *v223; // eax
  int v224; // eax
  int v225; // eax
  int *v226; // eax
  int v227; // eax
  int v228; // ecx
  _DWORD *v229; // eax
  int v230; // eax
  int v231; // eax
  LONG left; // ecx
  int v233; // eax
  int v234; // eax
  CUIUserInfoDetail *v235; // edi
  _BYTE *v236; // eax
  void (__stdcall *v237)(VARIANTARG *); // esi
  int v238; // eax
  int v239; // eax
  int *v240; // eax
  int v241; // eax
  int v242; // eax
  int v243; // ecx
  _DWORD *v244; // eax
  int v245; // eax
  int v246; // eax
  _DWORD *v247; // eax
  _DWORD *v248; // eax
  int v249; // eax
  int v250; // eax
  int *v251; // eax
  int v252; // eax
  int v253; // ecx
  _DWORD *v254; // eax
  int v255; // eax
  int v256; // eax
  _DWORD *v257; // eax
  _DWORD *v258; // eax
  int v259; // eax
  int v260; // eax
  int *v261; // eax
  int v262; // eax
  int v263; // ecx
  _DWORD *v264; // eax
  int v265; // eax
  int v266; // eax
  _DWORD *v267; // eax
  _DWORD *v268; // eax
  int v269; // eax
  int v270; // eax
  int *v271; // eax
  int v272; // eax
  int v273; // ecx
  _DWORD *v274; // eax
  int v275; // eax
  int v276; // eax
  int v277; // eax
  unsigned int v278; // edi
  void (__stdcall *v279)(VARIANTARG *); // esi
  int v280; // eax
  int v281; // eax
  int *v282; // eax
  int v283; // eax
  int v284; // eax
  int v285; // ecx
  _DWORD *v286; // eax
  int v287; // eax
  int v288; // eax
  int v289; // eax
  int v290; // edi
  _DWORD *v291; // eax
  char *v292; // eax
  int v293; // eax
  int v294; // eax
  int v295; // eax
  int *v296; // eax
  int v297; // eax
  int v298; // ecx
  _DWORD *v299; // eax
  int v300; // eax
  int v301; // eax
  CUIUserInfoDetail *v302; // esi
  int v303; // edi
  int v304; // eax
  int QuestDisplayMedal; // eax
  _DWORD *v306; // eax
  _DWORD *v307; // eax
  char *v308; // eax
  int v309; // eax
  int *v310; // eax
  int v311; // eax
  int v312; // ecx
  _DWORD *v313; // eax
  bool v314; // zf
  int v315; // ecx
  _DWORD *v316; // eax
  int v317; // eax
  int *v318; // eax
  int v319; // ecx
  _DWORD *v320; // eax
  int v321; // eax
  int *v322; // eax
  int v323; // [esp-28h] [ebp-10Ch] BYREF
  unsigned int v324; // [esp-24h] [ebp-108h]
  int v325; // [esp-20h] [ebp-104h]
  int v326; // [esp-1Ch] [ebp-100h] BYREF
  StringPoolStrings p_pvarg; // [esp-18h] [ebp-FCh]
  int v328; // [esp-14h] [ebp-F8h] BYREF
  unsigned __int64 v329; // [esp-10h] [ebp-F4h] BYREF
  _QWORD v330[2]; // [esp-8h] [ebp-ECh] BYREF
  VARIANTARG v331; // [esp+Ch] [ebp-D8h] BYREF
  VARIANTARG v332; // [esp+1Ch] [ebp-C8h] BYREF
  VARIANTARG v333; // [esp+2Ch] [ebp-B8h] BYREF
  VARIANTARG v334; // [esp+3Ch] [ebp-A8h] BYREF
  unsigned __int64 *v335; // [esp+4Ch] [ebp-98h] BYREF
  _BYTE v336[4]; // [esp+50h] [ebp-94h] BYREF
  VARIANTARG v337; // [esp+54h] [ebp-90h] BYREF
  VARIANTARG v338; // [esp+64h] [ebp-80h] BYREF
  VARIANTARG pvargDest; // [esp+74h] [ebp-70h] BYREF
  int *v340; // [esp+84h] [ebp-60h] BYREF
  int Unknown; // [esp+88h] [ebp-5Ch] BYREF
  int v342; // [esp+8Ch] [ebp-58h] BYREF
  int v343; // [esp+90h] [ebp-54h] BYREF
  int v344; // [esp+94h] [ebp-50h] BYREF
  unsigned __int64 *v345; // [esp+98h] [ebp-4Ch] BYREF
  CUIUserInfoDetail *v346; // [esp+9Ch] [ebp-48h]
  VARIANTARG pvarg; // [esp+A0h] [ebp-44h] BYREF
  VARIANTARG v348; // [esp+B0h] [ebp-34h] BYREF
  unsigned __int64 *String; // [esp+C0h] [ebp-24h] BYREF
  int v350; // [esp+C4h] [ebp-20h] BYREF
  int v351; // [esp+C8h] [ebp-1Ch] BYREF
  int v352; // [esp+CCh] [ebp-18h] BYREF
  int v353; // [esp+D0h] [ebp-14h] BYREF
  int *v354; // [esp+D4h] [ebp-10h] BYREF
  int v355; // [esp+E0h] [ebp-4h]

  v2 = this; /*0x90119b*/
  v346 = this; /*0x90119d*/
  CWnd::Draw(this, a2); /*0x9011a0*/
  v344 = 0; /*0x9011a7*/
  v355 = 0; /*0x9011b0*/
  CWnd::GetCanvas(a1: &v342); /*0x9011b3*/
  LOBYTE(v355) = 1; /*0x9011c1*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x9011c5*/
  LOBYTE(v355) = 2; /*0x9011ce*/
  sub_402F85(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x9011d2*/
  LOBYTE(v355) = 3; /*0x9011db*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x9011df*/
  HIDWORD(v330[0]) = &pvargDest; /*0x9011e7*/
  LODWORD(v330[0]) = &v348; /*0x9011eb*/
  LOBYTE(v355) = 4; /*0x9011f1*/
  basic_font = get_basic_font(a1: &Unknown, fType: 0); /*0x9011f5*/
  LOBYTE(v355) = 5; /*0x9011fe*/
  HIDWORD(v329) = sub_4284A5(this: basic_font); /*0x901207*/
  v4 = sub_428226(a1: v2 + 1640); /*0x90120e*/
  LODWORD(v329) = v5; /*0x901213*/
  v354 = &v329; /*0x901216*/
  _bstr_t::_bstr_t(a1: v4); /*0x90121a*/
  v328 = 144; /*0x90121f*/
  p_pvarg = &pvarg; /*0x901227*/
  LOBYTE(v355) = 6; /*0x90122e*/
  v6 = sub_428226(a1: v2 + 1640); /*0x901232*/
  v326 = v7; /*0x901237*/
  v353 = &v326; /*0x90123a*/
  _bstr_t::_bstr_t(a1: v6); /*0x90123e*/
  LOBYTE(v355) = 7; /*0x901248*/
  v8 = get_basic_font(a1: &a2, fType: 0); /*0x90124c*/
  LOBYTE(v355) = 9; /*0x901255*/
  sub_4284A8(a1: v8); /*0x901259*/
  p_pvarg = 52 - (sub_42782E(a1: v326, a2: p_pvarg) >> 1); /*0x90126c*/
  LOBYTE(v355) = 10; /*0x901270*/
  v9 = sub_414576(a1: &v342); /*0x901274*/
  IWzCanvas::DrawTextA( /*0x90127b*/
    this: v9,
    nLeft: p_pvarg,
    nTop: v328,
    sText: v329,
    pFont: SHIDWORD(v329),
    vAlpha: v330[0],
    vTabOrg: HIDWORD(v330[0]));
  sub_45BAEE(a1: &a2); /*0x901283*/
  sub_45BAEE(a1: &Unknown); /*0x90128b*/
  LOBYTE(v355) = 3; /*0x901294*/
  v10 = ZComAPI::ZComVariantCopy(&pvarg); /*0x901298*/
  if ( v10 < 0 ) /*0x9012a0*/
    _com_issue_error(a1: v10); /*0x9012a3*/
  LOBYTE(v355) = 2; /*0x9012ac*/
  v11 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x9012b0*/
  if ( v11 < 0 ) /*0x9012b8*/
    _com_issue_error(a1: v11); /*0x9012bb*/
  LOBYTE(v355) = 1; /*0x9012c4*/
  v12 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x9012c8*/
  if ( v12 < 0 ) /*0x9012d0*/
    _com_issue_error(a1: v12); /*0x9012d3*/
  HIDWORD(v330[SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK]) = SP_5515_D; /*0x9012db*/
  LODWORD(v330[0]) = &a2; /*0x9012e0*/
  Instance = StringPool::GetInstance(); /*0x9012e1*/
  v14 = *StringPool::GetString(this: Instance, result: v330[0], nIdx: SHIDWORD(v330[0])); /*0x9012f4*/
  BYTE4(v330[0]) = *(v2 + 1644); /*0x9012f6*/
  LOBYTE(v355) = 11; /*0x9012fc*/
  ZXString<char>::Format(a1: &v344, a2: v14, a3: SBYTE4(v330[0])); /*0x901300*/
  LOBYTE(v355) = 1; /*0x90130d*/
  if ( a2 != 0 ) /*0x901311*/
    ZXString<char>::_Release(lpAddend: &a2[-1].top); /*0x901317*/
  sub_402F85(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x901321*/
  LOBYTE(v355) = 12; /*0x90132a*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x90132e*/
  HIDWORD(v330[0]) = &v348; /*0x901336*/
  LODWORD(v330[0]) = &pvarg; /*0x90133a*/
  LOBYTE(v355) = 13; /*0x901341*/
  v15 = get_basic_font(a1: &a2, fType: 1); /*0x901345*/
  LOBYTE(v355) = 14; /*0x90134e*/
  HIDWORD(v329) = sub_4284A5(this: v15); /*0x901357*/
  v16 = sub_428226(a1: &v344); /*0x90135b*/
  LODWORD(v329) = v17; /*0x901360*/
  v354 = &v329; /*0x901363*/
  _bstr_t::_bstr_t(a1: v16); /*0x901367*/
  v328 = 77; /*0x90136c*/
  p_pvarg = SP_160_YOU_HAVE_NO_BUDDY_AVAILABLE_FOR_CHAT; /*0x90136e*/
  LOBYTE(v355) = 14; /*0x901376*/
  v18 = sub_414576(a1: &v342); /*0x90137a*/
  IWzCanvas::DrawTextA( /*0x901381*/
    this: v18,
    nLeft: p_pvarg,
    nTop: v328,
    sText: v329,
    pFont: SHIDWORD(v329),
    vAlpha: v330[0],
    vTabOrg: HIDWORD(v330[0]));
  sub_45BAEE(a1: &a2); /*0x901389*/
  LOBYTE(v355) = 12; /*0x901392*/
  v19 = ZComAPI::ZComVariantCopy(&pvarg); /*0x901396*/
  if ( v19 < 0 ) /*0x90139e*/
    _com_issue_error(a1: v19); /*0x9013a1*/
  LOBYTE(v355) = 1; /*0x9013aa*/
  v20 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x9013ae*/
  if ( v20 < 0 ) /*0x9013b6*/
    _com_issue_error(a1: v20); /*0x9013b9*/
  job_name = get_job_name(a1: *(v2 + 823)); /*0x9013c6*/
  ZXString<char>::GetBuffer(a1: job_name, a2: 0xFFFFFFFF); /*0x9013d2*/
  sub_538AAC(a1: &v344); /*0x9013da*/
  sub_402F85(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x9013e3*/
  LOBYTE(v355) = 16; /*0x9013ec*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x9013f0*/
  HIDWORD(v330[0]) = &v348; /*0x9013f8*/
  LODWORD(v330[0]) = &pvarg; /*0x9013fc*/
  LOBYTE(v355) = 17; /*0x901403*/
  v22 = get_basic_font(a1: &a2, fType: 44); /*0x901407*/
  LOBYTE(v355) = 18; /*0x901410*/
  HIDWORD(v329) = sub_4284A5(this: v22); /*0x901419*/
  v23 = sub_428226(a1: &v344); /*0x90141d*/
  LODWORD(v329) = v24; /*0x901422*/
  v354 = &v329; /*0x901425*/
  _bstr_t::_bstr_t(a1: v23); /*0x901429*/
  v328 = 96; /*0x90142e*/
  p_pvarg = SP_160_YOU_HAVE_NO_BUDDY_AVAILABLE_FOR_CHAT; /*0x901430*/
  LOBYTE(v355) = 18; /*0x901438*/
  v25 = sub_414576(a1: &v342); /*0x90143c*/
  IWzCanvas::DrawTextA( /*0x901443*/
    this: v25,
    nLeft: p_pvarg,
    nTop: v328,
    sText: v329,
    pFont: SHIDWORD(v329),
    vAlpha: v330[0],
    vTabOrg: HIDWORD(v330[0]));
  sub_45BAEE(a1: &a2); /*0x90144b*/
  LOBYTE(v355) = 16; /*0x901454*/
  v26 = ZComAPI::ZComVariantCopy(&pvarg); /*0x901458*/
  if ( v26 < 0 ) /*0x901460*/
    _com_issue_error(a1: v26); /*0x901463*/
  LOBYTE(v355) = 1; /*0x90146c*/
  v27 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x901470*/
  if ( v27 < 0 ) /*0x901478*/
    _com_issue_error(a1: v27); /*0x90147b*/
  HIDWORD(v330[SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK]) = SP_5515_D; /*0x901483*/
  LODWORD(v330[0]) = &a2; /*0x901488*/
  v28 = StringPool::GetInstance(); /*0x901489*/
  v29 = *StringPool::GetString(this: v28, result: v330[0], nIdx: SHIDWORD(v330[0])); /*0x90149c*/
  WORD2(v330[0]) = *(v2 + 824); /*0x90149e*/
  LOBYTE(v355) = 20; /*0x9014a4*/
  ZXString<char>::Format(a1: &v344, a2: v29, a3: SBYTE4(v330[0])); /*0x9014a8*/
  LOBYTE(v355) = 1; /*0x9014b5*/
  if ( a2 != 0 ) /*0x9014b9*/
    ZXString<char>::_Release(lpAddend: &a2[-1].top); /*0x9014bf*/
  sub_402F85(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x9014c9*/
  LOBYTE(v355) = 21; /*0x9014d2*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x9014d6*/
  HIDWORD(v330[0]) = &v348; /*0x9014de*/
  LODWORD(v330[0]) = &pvarg; /*0x9014e2*/
  LOBYTE(v355) = 22; /*0x9014e9*/
  v30 = get_basic_font(a1: &a2, fType: 1); /*0x9014ed*/
  LOBYTE(v355) = 23; /*0x9014f6*/
  HIDWORD(v329) = sub_4284A5(this: v30); /*0x9014ff*/
  v31 = sub_428226(a1: &v344); /*0x901503*/
  LODWORD(v329) = v32; /*0x901508*/
  v354 = &v329; /*0x90150b*/
  _bstr_t::_bstr_t(a1: v31); /*0x90150f*/
  v328 = 113; /*0x901514*/
  p_pvarg = SP_160_YOU_HAVE_NO_BUDDY_AVAILABLE_FOR_CHAT; /*0x901516*/
  LOBYTE(v355) = 23; /*0x90151e*/
  v33 = sub_414576(a1: &v342); /*0x901522*/
  IWzCanvas::DrawTextA( /*0x901529*/
    this: v33,
    nLeft: p_pvarg,
    nTop: v328,
    sText: v329,
    pFont: SHIDWORD(v329),
    vAlpha: v330[0],
    vTabOrg: HIDWORD(v330[0]));
  sub_45BAEE(a1: &a2); /*0x901531*/
  LOBYTE(v355) = 21; /*0x90153a*/
  v34 = ZComAPI::ZComVariantCopy(&pvarg); /*0x90153e*/
  if ( v34 < 0 ) /*0x901546*/
    _com_issue_error(a1: v34); /*0x901549*/
  LOBYTE(v355) = 1; /*0x901552*/
  v35 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x901556*/
  if ( v35 < 0 ) /*0x90155e*/
    _com_issue_error(a1: v35); /*0x901561*/
  ZXString<char>::operator=(a1: v2 + 1652); /*0x901570*/
  v330[0] = v36 | 0x5400000000LL; /*0x901577*/
  a2 = v330; /*0x90157a*/
  get_basic_font(a1: v330, fType: 1); /*0x901580*/
  format_string(sFormat: &v344, pFont: v330[0], nWidth: HIDWORD(v330[0])); /*0x90158b*/
  sub_402F85(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x901597*/
  LOBYTE(v355) = 25; /*0x9015a0*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x9015a4*/
  HIDWORD(v330[0]) = &v348; /*0x9015ac*/
  LODWORD(v330[0]) = &pvarg; /*0x9015b0*/
  LOBYTE(v355) = 26; /*0x9015b7*/
  v37 = get_basic_font(a1: &a2, fType: 1); /*0x9015bb*/
  LOBYTE(v355) = 27; /*0x9015c4*/
  HIDWORD(v329) = sub_4284A5(this: v37); /*0x9015cd*/
  v38 = sub_428226(a1: &v344); /*0x9015d1*/
  LODWORD(v329) = v39; /*0x9015d6*/
  v354 = &v329; /*0x9015d9*/
  _bstr_t::_bstr_t(a1: v38); /*0x9015dd*/
  v328 = 131; /*0x9015e2*/
  p_pvarg = SP_160_YOU_HAVE_NO_BUDDY_AVAILABLE_FOR_CHAT; /*0x9015e7*/
  LOBYTE(v355) = 27; /*0x9015ef*/
  v40 = sub_414576(a1: &v342); /*0x9015f3*/
  IWzCanvas::DrawTextA( /*0x9015fa*/
    this: v40,
    nLeft: p_pvarg,
    nTop: v328,
    sText: v329,
    pFont: SHIDWORD(v329),
    vAlpha: v330[0],
    vTabOrg: HIDWORD(v330[0]));
  sub_45BAEE(a1: &a2); /*0x901602*/
  LOBYTE(v355) = 25; /*0x90160b*/
  v41 = ZComAPI::ZComVariantCopy(&pvarg); /*0x90160f*/
  if ( v41 < 0 ) /*0x901617*/
    _com_issue_error(a1: v41); /*0x90161a*/
  LOBYTE(v355) = 1; /*0x901623*/
  v42 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x901627*/
  if ( v42 < 0 ) /*0x90162f*/
    _com_issue_error(a1: v42); /*0x901632*/
  v43 = *(v2 + 408); /*0x901637*/
  if ( v43 == 1 ) /*0x901640*/
  {
    a2 = 0; /*0x901646*/
    LOBYTE(v355) = 29; /*0x90164f*/
    CWnd::GetCanvas(a1: &v351); /*0x901653*/
    LOBYTE(v355) = 30; /*0x90165c*/
    sub_402F85(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x901660*/
    LOBYTE(v355) = 31; /*0x901669*/
    sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x90166d*/
    v330[0] = 0; /*0x901673*/
    HIDWORD(v329) = &v348; /*0x901677*/
    LODWORD(v329) = &pvarg; /*0x90167b*/
    v328 = v44; /*0x90167c*/
    v354 = &v328; /*0x90167f*/
    p_pvarg = SP_2564_UI_UIWINDOWIMG_USERINFO_REQLV; /*0x901682*/
    v326 = &v328; /*0x901687*/
    LOBYTE(v355) = 32; /*0x901688*/
    v45 = StringPool::GetInstance(); /*0x90168c*/
    StringPool::GetBSTR(this: v45, a2: v326, a3: p_pvarg); /*0x901693*/
    p_pvarg = &v331; /*0x90169e*/
    LOBYTE(v355) = 32; /*0x90169f*/
    v46 = sub_406472(); /*0x9016a3*/
    v47 = sub_402680(a1: v46); /*0x9016aa*/
    IWzResMan::GetObjectA(this: v47, pvargDest: p_pvarg, sUOL: v328, vParam: v329, vAux: SHIDWORD(v329)); /*0x9016b1*/
    LOBYTE(v355) = 34; /*0x9016b8*/
    Unknown = Ztl_variant_t::GetUnknown(a1: v330[0], a2: SBYTE4(v330[0])); /*0x9016c3*/
    v343 = 0; /*0x9016c6*/
    if ( Unknown != 0 ) /*0x9016c9*/
    {
      v48 = sub_41E527(a1: &Unknown); /*0x9016d2*/
      if ( v48 < 0 && v48 != -2147467262 ) /*0x9016e0*/
        _com_issue_error(a1: v48); /*0x9016e3*/
    }
    LOBYTE(v355) = 38; /*0x9016ef*/
    v49 = ZComAPI::ZComVariantCopy(pvarg: &v331); /*0x9016f3*/
    if ( v49 < 0 ) /*0x9016fb*/
      _com_issue_error(a1: v49); /*0x9016fe*/
    LOBYTE(v355) = 37; /*0x901707*/
    v50 = ZComAPI::ZComVariantCopy(&pvarg); /*0x90170b*/
    if ( v50 < 0 ) /*0x901713*/
      _com_issue_error(a1: v50); /*0x901716*/
    LOBYTE(v355) = 36; /*0x90171f*/
    v51 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x901723*/
    if ( v51 < 0 ) /*0x90172b*/
      _com_issue_error(a1: v51); /*0x90172e*/
    sub_402F85(pvargDest: &v333, pvargSrc: &pvargSrc); /*0x90173a*/
    LOBYTE(v355) = 39; /*0x901743*/
    sub_402F85(pvargDest: &v338, pvargSrc: &pvargSrc); /*0x901747*/
    LOBYTE(v355) = 40; /*0x901750*/
    sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x901754*/
    HIDWORD(v330[0]) = &v333; /*0x90175f*/
    LODWORD(v330[0]) = &v338; /*0x901763*/
    LOBYTE(v355) = 41; /*0x901769*/
    v52 = get_basic_font(a1: &v352, fType: 0); /*0x90176d*/
    LOBYTE(v355) = 42; /*0x901776*/
    HIDWORD(v329) = sub_4284A5(this: v52); /*0x90177f*/
    v53 = sub_428226(a1: v2 + 4 * *(v2 + 417) + 1684); /*0x90178d*/
    LODWORD(v329) = v54; /*0x901792*/
    v354 = &v329; /*0x901795*/
    _bstr_t::_bstr_t(a1: v53); /*0x901799*/
    v328 = 306; /*0x90179e*/
    p_pvarg = &pvargDest; /*0x9017a6*/
    v55 = v2 + 4 * *(v2 + 417) + 1684; /*0x9017ad*/
    LOBYTE(v355) = 43; /*0x9017b4*/
    v56 = sub_428226(a1: v55); /*0x9017b8*/
    v326 = v57; /*0x9017bd*/
    v353 = &v326; /*0x9017c0*/
    _bstr_t::_bstr_t(a1: v56); /*0x9017c4*/
    LOBYTE(v355) = 44; /*0x9017ce*/
    v58 = get_basic_font(a1: &v350, fType: 0); /*0x9017d2*/
    LOBYTE(v355) = 46; /*0x9017db*/
    sub_4284A8(a1: v58); /*0x9017df*/
    p_pvarg = 52 - (sub_42782E(a1: v326, a2: p_pvarg) >> 1); /*0x9017f2*/
    LOBYTE(v355) = 47; /*0x9017f6*/
    v59 = sub_414576(a1: &v351); /*0x9017fa*/
    IWzCanvas::DrawTextA( /*0x901801*/
      this: v59,
      nLeft: p_pvarg,
      nTop: v328,
      sText: v329,
      pFont: SHIDWORD(v329),
      vAlpha: v330[0],
      vTabOrg: HIDWORD(v330[0]));
    sub_45BAEE(a1: &v350); /*0x901809*/
    sub_45BAEE(a1: &v352); /*0x901811*/
    LOBYTE(v355) = 40; /*0x90181a*/
    v60 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x90181e*/
    if ( v60 < 0 ) /*0x901826*/
      _com_issue_error(a1: v60); /*0x901829*/
    LOBYTE(v355) = 39; /*0x901832*/
    v61 = ZComAPI::ZComVariantCopy(pvarg: &v338); /*0x901836*/
    if ( v61 < 0 ) /*0x90183e*/
      _com_issue_error(a1: v61); /*0x901841*/
    LOBYTE(v355) = 36; /*0x90184d*/
    v62 = ZComAPI::ZComVariantCopy(pvarg: &v333); /*0x901851*/
    if ( v62 < 0 ) /*0x901859*/
      _com_issue_error(a1: v62); /*0x90185c*/
    ZXString<char>::operator=(a1: *(v2 + *(v2 + 417) + 418) + 4); /*0x901875*/
    sub_538AAC(a1: &a2); /*0x90187d*/
    sub_402F85(pvargDest: &v338, pvargSrc: &pvargSrc); /*0x901886*/
    LOBYTE(v355) = 48; /*0x90188f*/
    sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x901893*/
    HIDWORD(v330[0]) = &v338; /*0x90189b*/
    LODWORD(v330[0]) = &pvargDest; /*0x90189f*/
    LOBYTE(v355) = 49; /*0x9018a6*/
    v63 = get_basic_font(a1: &v350, fType: 1); /*0x9018aa*/
    LOBYTE(v355) = 50; /*0x9018b3*/
    HIDWORD(v329) = sub_4284A5(this: v63); /*0x9018bc*/
    v64 = sub_428226(a1: *(v2 + *(v2 + 417) + 418) + 4); /*0x9018cd*/
    LODWORD(v329) = v65; /*0x9018d2*/
    v354 = &v329; /*0x9018d5*/
    _bstr_t::_bstr_t(a1: v64); /*0x9018d9*/
    v328 = 331; /*0x9018de*/
    p_pvarg = SP_58_BRAWLER; /*0x9018e3*/
    LOBYTE(v355) = 50; /*0x9018e8*/
    v66 = sub_414576(a1: &v351); /*0x9018ec*/
    IWzCanvas::DrawTextA( /*0x9018f3*/
      this: v66,
      nLeft: p_pvarg,
      nTop: v328,
      sText: v329,
      pFont: SHIDWORD(v329),
      vAlpha: v330[0],
      vTabOrg: HIDWORD(v330[0]));
    sub_45BAEE(a1: &v350); /*0x9018fb*/
    LOBYTE(v355) = 48; /*0x901904*/
    v67 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x901908*/
    if ( v67 < 0 ) /*0x901910*/
      _com_issue_error(a1: v67); /*0x901913*/
    LOBYTE(v355) = 36; /*0x90191c*/
    v68 = ZComAPI::ZComVariantCopy(pvarg: &v338); /*0x901920*/
    if ( v68 < 0 ) /*0x901928*/
      _com_issue_error(a1: v68); /*0x90192b*/
    HIDWORD(v330[SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK]) = SP_5515_D; /*0x901933*/
    LODWORD(v330[0]) = &v350; /*0x901938*/
    v69 = StringPool::GetInstance(); /*0x901939*/
    v70 = *StringPool::GetString(this: v69, result: v330[0], nIdx: SHIDWORD(v330[0])); /*0x901953*/
    BYTE4(v330[0]) = *(v2 + *(v2 + 417) + 1696); /*0x901955*/
    LOBYTE(v355) = 52; /*0x90195b*/
    ZXString<char>::Format(a1: &a2, a2: v70, a3: SBYTE4(v330[0])); /*0x90195f*/
    LOBYTE(v355) = 36; /*0x90196c*/
    if ( v350 != 0 ) /*0x901970*/
      ZXString<char>::_Release(lpAddend: (v350 - 12)); /*0x901976*/
    sub_402F85(pvargDest: &v338, pvargSrc: &pvargSrc); /*0x901980*/
    LOBYTE(v355) = 53; /*0x901989*/
    sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x90198d*/
    HIDWORD(v330[0]) = &v338; /*0x901995*/
    LODWORD(v330[0]) = &pvargDest; /*0x901999*/
    LOBYTE(v355) = 54; /*0x9019a0*/
    v71 = get_basic_font(a1: &v350, fType: 1); /*0x9019a4*/
    LOBYTE(v355) = 55; /*0x9019ad*/
    HIDWORD(v329) = sub_4284A5(this: v71); /*0x9019b6*/
    v72 = sub_428226(a1: &a2); /*0x9019ba*/
    LODWORD(v329) = v73; /*0x9019bf*/
    v354 = &v329; /*0x9019c2*/
    _bstr_t::_bstr_t(a1: v72); /*0x9019c6*/
    v328 = 348; /*0x9019cb*/
    p_pvarg = SP_58_BRAWLER; /*0x9019d0*/
    LOBYTE(v355) = 55; /*0x9019d5*/
    v74 = sub_414576(a1: &v351); /*0x9019d9*/
    IWzCanvas::DrawTextA( /*0x9019e0*/
      this: v74,
      nLeft: p_pvarg,
      nTop: v328,
      sText: v329,
      pFont: SHIDWORD(v329),
      vAlpha: v330[0],
      vTabOrg: HIDWORD(v330[0]));
    sub_45BAEE(a1: &v350); /*0x9019e8*/
    LOBYTE(v355) = 53; /*0x9019f1*/
    v75 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x9019f5*/
    if ( v75 < 0 ) /*0x9019fd*/
      _com_issue_error(a1: v75); /*0x901a00*/
    LOBYTE(v355) = 36; /*0x901a09*/
    v76 = ZComAPI::ZComVariantCopy(pvarg: &v338); /*0x901a0d*/
    if ( v76 < 0 ) /*0x901a15*/
      _com_issue_error(a1: v76); /*0x901a18*/
    HIDWORD(v330[SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK]) = SP_5515_D; /*0x901a20*/
    LODWORD(v330[0]) = &v350; /*0x901a25*/
    v77 = StringPool::GetInstance(); /*0x901a26*/
    v78 = *StringPool::GetString(this: v77, result: v330[0], nIdx: SHIDWORD(v330[0])); /*0x901a40*/
    WORD2(v330[0]) = *(v2 + *(v2 + 417) + 850); /*0x901a42*/
    LOBYTE(v355) = 57; /*0x901a48*/
    ZXString<char>::Format(a1: &a2, a2: v78, a3: SBYTE4(v330[0])); /*0x901a4c*/
    LOBYTE(v355) = 36; /*0x901a59*/
    if ( v350 != 0 ) /*0x901a5d*/
      ZXString<char>::_Release(lpAddend: (v350 - 12)); /*0x901a63*/
    sub_402F85(pvargDest: &v338, pvargSrc: &pvargSrc); /*0x901a6d*/
    LOBYTE(v355) = 58; /*0x901a76*/
    sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x901a7a*/
    HIDWORD(v330[0]) = &v338; /*0x901a82*/
    LODWORD(v330[0]) = &pvargDest; /*0x901a86*/
    LOBYTE(v355) = 59; /*0x901a8d*/
    v79 = get_basic_font(a1: &v350, fType: 1); /*0x901a91*/
    LOBYTE(v355) = 60; /*0x901a9a*/
    HIDWORD(v329) = sub_4284A5(this: v79); /*0x901aa3*/
    v80 = sub_428226(a1: &a2); /*0x901aa7*/
    LODWORD(v329) = v81; /*0x901aac*/
    v354 = &v329; /*0x901aaf*/
    _bstr_t::_bstr_t(a1: v80); /*0x901ab3*/
    v328 = 330; /*0x901ab8*/
    p_pvarg = SP_189__GIVEPRIZE; /*0x901abd*/
    LOBYTE(v355) = 60; /*0x901ac5*/
    v82 = sub_414576(a1: &v351); /*0x901ac9*/
    IWzCanvas::DrawTextA( /*0x901ad0*/
      this: v82,
      nLeft: p_pvarg,
      nTop: v328,
      sText: v329,
      pFont: SHIDWORD(v329),
      vAlpha: v330[0],
      vTabOrg: HIDWORD(v330[0]));
    sub_45BAEE(a1: &v350); /*0x901ad8*/
    LOBYTE(v355) = 58; /*0x901ae1*/
    v83 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x901ae5*/
    if ( v83 < 0 ) /*0x901aed*/
      _com_issue_error(a1: v83); /*0x901af0*/
    LOBYTE(v355) = 36; /*0x901af9*/
    v84 = ZComAPI::ZComVariantCopy(pvarg: &v338); /*0x901afd*/
    if ( v84 < 0 ) /*0x901b05*/
      _com_issue_error(a1: v84); /*0x901b08*/
    HIDWORD(v330[SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK]) = SP_5515_D; /*0x901b10*/
    LODWORD(v330[0]) = &v350; /*0x901b15*/
    v85 = StringPool::GetInstance(); /*0x901b16*/
    v86 = *StringPool::GetString(this: v85, result: v330[0], nIdx: SHIDWORD(v330[0])); /*0x901b30*/
    BYTE4(v330[0]) = *(v2 + *(v2 + 417) + 1706); /*0x901b32*/
    LOBYTE(v355) = 62; /*0x901b38*/
    ZXString<char>::Format(a1: &a2, a2: v86, a3: SBYTE4(v330[0])); /*0x901b3c*/
    LOBYTE(v355) = 36; /*0x901b49*/
    if ( v350 != 0 ) /*0x901b4d*/
      ZXString<char>::_Release(lpAddend: (v350 - 12)); /*0x901b53*/
    sub_402F85(pvargDest: &v338, pvargSrc: &pvargSrc); /*0x901b5d*/
    LOBYTE(v355) = 63; /*0x901b66*/
    sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x901b6a*/
    HIDWORD(v330[0]) = &v338; /*0x901b72*/
    LODWORD(v330[0]) = &pvargDest; /*0x901b76*/
    LOBYTE(v355) = 64; /*0x901b7d*/
    v87 = get_basic_font(a1: &v350, fType: 1); /*0x901b81*/
    LOBYTE(v355) = 65; /*0x901b8a*/
    HIDWORD(v329) = sub_4284A5(this: v87); /*0x901b93*/
    v88 = sub_428226(a1: &a2); /*0x901b97*/
    LODWORD(v329) = v89; /*0x901b9c*/
    v354 = &v329; /*0x901b9f*/
    _bstr_t::_bstr_t(a1: v88); /*0x901ba3*/
    v328 = 348; /*0x901ba8*/
    p_pvarg = SP_189__GIVEPRIZE; /*0x901bad*/
    LOBYTE(v355) = 65; /*0x901bb5*/
    v90 = sub_414576(a1: &v351); /*0x901bb9*/
    IWzCanvas::DrawTextA( /*0x901bc0*/
      this: v90,
      nLeft: p_pvarg,
      nTop: v328,
      sText: v329,
      pFont: SHIDWORD(v329),
      vAlpha: v330[0],
      vTabOrg: HIDWORD(v330[0]));
    sub_45BAEE(a1: &v350); /*0x901bc8*/
    LOBYTE(v355) = 63; /*0x901bd1*/
    v91 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x901bd5*/
    if ( v91 < 0 ) /*0x901bdd*/
      _com_issue_error(a1: v91); /*0x901be0*/
    LOBYTE(v355) = 36; /*0x901be9*/
    v92 = ZComAPI::ZComVariantCopy(pvarg: &v338); /*0x901bed*/
    if ( v92 < 0 ) /*0x901bf5*/
      _com_issue_error(a1: v92); /*0x901bf8*/
    v94 = *(v2 + *(v2 + 417) + 400); /*0x901c0a*/
    if ( v94 != 0 && *(v94 - 4) != 0 ) /*0x901c17*/
    {
      v95 = *(*(v2 + 393) + 56); /*0x901c23*/
      v352 = v95; /*0x901c26*/
      while ( 1 ) /*0x901c36*/
      {
        v96 = *(v2 + *(v2 + 417) + 400); /*0x901c36*/
        if ( v96 == 0 || v95 >= *(v96 - 4) || v95 >= *(*(v2 + 393) + 56) + 3 ) /*0x901c57*/
          break; /*0x901c57*/
        v330[0] = 0; /*0x901c5e*/
        v329 = 0; /*0x901c60*/
        v328 = 0; /*0x901c61*/
        p_pvarg = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x901c62*/
        v97 = sub_64BCCC(a1: v2 + 1568, a2: v93); /*0x901c69*/
        v98 = sub_64B1A1(a1: v97); /*0x901c70*/
        v326 = 38 * (v352 - v98) + 248; /*0x901c89*/
        v325 = 106; /*0x901c8a*/
        v324 = *sub_5ADCAA(a1: v352); /*0x901c9b*/
        v323 = v99; /*0x901ca0*/
        String = &v323; /*0x901ca3*/
        sub_4281DD(a1: &v351); /*0x901ca7*/
        LOBYTE(v355) = 36; /*0x901cac*/
        sub_4283F8(); /*0x901cb0*/
        sub_5D6458( /*0x901cb7*/
          a1: v323,
          a2: v324,
          a3: v325,
          a4: v326,
          a5: p_pvarg,
          a6: v328,
          a7: v329,
          a8: HIDWORD(v329),
          a9: v330[0],
          a10: HIDWORD(v330[0]));
        ItemName = CItemInfo::GetItemName(a1: &v335, a2: *(*(v2 + *(v2 + 417) + 400) + 4 * v352)); /*0x901cdc*/
        LOBYTE(v355) = 68; /*0x901ce5*/
        ZXString<char>::operator=(a1: ItemName); /*0x901ce9*/
        LOBYTE(v355) = 36; /*0x901cf6*/
        if ( v335 != 0 ) /*0x901cfa*/
        {
          ZXString<char>::_Release(lpAddend: v335 - 3); /*0x901d00*/
          v101 = HIDWORD(v330[0]); /*0x901d05*/
        }
        v330[0] = v101 | 0x6400000000LL; /*0x901d08*/
        String = v330; /*0x901d0b*/
        get_basic_font(a1: v330, fType: 1); /*0x901d11*/
        format_string(sFormat: &a2, pFont: v330[0], nWidth: HIDWORD(v330[0])); /*0x901d1c*/
        sub_402F85(pvargDest: &v338, pvargSrc: &pvargSrc); /*0x901d28*/
        LOBYTE(v355) = 69; /*0x901d31*/
        sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x901d35*/
        HIDWORD(v330[0]) = &v338; /*0x901d3d*/
        LODWORD(v330[0]) = &pvargDest; /*0x901d41*/
        LOBYTE(v355) = 70; /*0x901d48*/
        v102 = get_basic_font(a1: &v340, fType: 1); /*0x901d4c*/
        LOBYTE(v355) = 71; /*0x901d55*/
        HIDWORD(v329) = sub_4284A5(this: v102); /*0x901d5e*/
        v103 = sub_428226(a1: &a2); /*0x901d62*/
        LODWORD(v329) = v104; /*0x901d67*/
        String = &v329; /*0x901d6a*/
        _bstr_t::_bstr_t(a1: v103); /*0x901d6e*/
        LOBYTE(v355) = 71; /*0x901d79*/
        v106 = sub_64BCCC(a1: v2 + 1568, a2: v105); /*0x901d7d*/
        v107 = sub_64B1A1(a1: v106); /*0x901d84*/
        v328 = 38 * (v352 - v107) + 217; /*0x901d97*/
        p_pvarg = SP_147_PLEASE_TRY_AGAIN; /*0x901d98*/
        v108 = sub_414576(a1: &v351); /*0x901da0*/
        IWzCanvas::DrawTextA( /*0x901da7*/
          this: v108,
          nLeft: p_pvarg,
          nTop: v328,
          sText: v329,
          pFont: SHIDWORD(v329),
          vAlpha: v330[0],
          vTabOrg: HIDWORD(v330[0]));
        sub_45BAEE(a1: &v340); /*0x901daf*/
        LOBYTE(v355) = 69; /*0x901db8*/
        v109 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x901dbc*/
        if ( v109 < 0 ) /*0x901dc4*/
          _com_issue_error(a1: v109); /*0x901dc7*/
        LOBYTE(v355) = 36; /*0x901dd0*/
        v110 = ZComAPI::ZComVariantCopy(pvarg: &v338); /*0x901dd4*/
        if ( v110 < 0 ) /*0x901ddc*/
          _com_issue_error(a1: v110); /*0x901ddf*/
        sub_402F85(pvargDest: &v333, pvargSrc: &pvargSrc); /*0x901deb*/
        v345 = *(*(v2 + 393) + 56); /*0x901df9*/
        LOBYTE(v355) = 73; /*0x901e01*/
        if ( v351 == 0 ) /*0x901e05*/
          _com_issue_error(a1: -2147467261); /*0x901e0c*/
        v111 = *v351; /*0x901e1a*/
        v328 = v343; /*0x901e24*/
        v329 = *&v333.vt; /*0x901e2d*/
        p_pvarg = 38 * (v352 - v345) + 236; /*0x901e35*/
        v330[0] = v333.cyVal.int64; /*0x901e36*/
        v326 = 147; /*0x901e37*/
        v325 = v351; /*0x901e3c*/
        String = v351; /*0x901e3d*/
        v112 = (*(v111 + 128))( /*0x901e41*/
                 a1: v351,
                 a2: 147,
                 a3: p_pvarg,
                 a4: v343,
                 a5: *&v333.vt,
                 a6: v333.decVal.Hi32,
                 a7: v333.lVal,
                 a8: v333.cyVal.Hi);
        if ( v112 < 0 ) /*0x901e49*/
          _com_issue_errorex(a1: v112, a2: String, a3: _GUID_7600dc6c_9328_4bff_9624_5b0f5c01179e); /*0x901e54*/
        LOBYTE(v355) = 36; /*0x901e60*/
        v113 = ZComAPI::ZComVariantCopy(pvarg: &v333); /*0x901e64*/
        if ( v113 < 0 ) /*0x901e6c*/
          _com_issue_error(a1: v113); /*0x901e6f*/
        v114 = v346; /*0x901e74*/
        CItemInfo::GetItemInfo(a1: &v350, a2: *(*(v346 + *(v346 + 417) + 400) + 4 * v352)); /*0x901e94*/
        LOBYTE(v355) = 74; /*0x901e9d*/
        if ( v350 != 0 ) /*0x901ea1*/
        {
          v330[0] = v115; /*0x901ea9*/
          String = v330; /*0x901eac*/
          HIDWORD(v329) = SP_1887_REQLEVEL; /*0x901eaf*/
          LODWORD(v329) = v330; /*0x901eb4*/
          v116 = StringPool::GetInstance(); /*0x901eb5*/
          StringPool::GetBSTR(this: v116, a2: v329, a3: SHIDWORD(v329)); /*0x901ebc*/
          HIDWORD(v329) = &v332; /*0x901ec7*/
          LOBYTE(v355) = 74; /*0x901ecb*/
          sub_40263B(a1: &v350); /*0x901ecf*/
          Item = IWzProperty::GetItem(pvargDest: HIDWORD(v329), a2: v330[0]); /*0x901edb*/
          LODWORD(v330[0]) = SP_5515_D; /*0x901ee0*/
          HIDWORD(v329) = &v353; /*0x901ee5*/
          LOBYTE(v355) = 76; /*0x901ee6*/
          v118 = StringPool::GetInstance(); /*0x901eea*/
          String = StringPool::GetString(this: v118, result: HIDWORD(v329), nIdx: v330[0]); /*0x901ef6*/
          LOBYTE(v355) = 77; /*0x901efa*/
          HIDWORD(v330[0]) = get_int32(pvarSrc: Item, a2: SHIDWORD(v330[0])); /*0x901f08*/
          v119 = sub_428226(a1: String); /*0x901f09*/
          ZXString<char>::Format(a1: &a2, a2: v119, a3: SBYTE4(v330[0])); /*0x901f13*/
          LOBYTE(v355) = 76; /*0x901f20*/
          if ( v353 != 0 ) /*0x901f24*/
            ZXString<char>::_Release(lpAddend: (v353 - 12)); /*0x901f2a*/
          LOBYTE(v355) = 74; /*0x901f37*/
          v120 = ZComAPI::ZComVariantCopy(pvarg: &v332); /*0x901f3b*/
          if ( v120 < 0 ) /*0x901f43*/
            _com_issue_error(a1: v120); /*0x901f46*/
          sub_402F85(pvargDest: &v337, pvargSrc: &pvargSrc); /*0x901f52*/
          LOBYTE(v355) = 78; /*0x901f5e*/
          sub_402F85(pvargDest: &v334, pvargSrc: &pvargSrc); /*0x901f62*/
          HIDWORD(v330[0]) = &v337; /*0x901f6d*/
          LODWORD(v330[0]) = &v334; /*0x901f74*/
          LOBYTE(v355) = 79; /*0x901f7b*/
          v121 = get_basic_font(a1: &v354, fType: 1); /*0x901f7f*/
          LOBYTE(v355) = 80; /*0x901f88*/
          HIDWORD(v329) = sub_4284A5(this: v121); /*0x901f91*/
          v122 = sub_428226(a1: &a2); /*0x901f95*/
          LODWORD(v329) = v123; /*0x901f9a*/
          String = &v329; /*0x901f9d*/
          _bstr_t::_bstr_t(a1: v122); /*0x901fa1*/
          LOBYTE(v355) = 80; /*0x901fac*/
          v125 = sub_64BCCC(a1: v114 + 1568, a2: v124); /*0x901fb0*/
          v126 = sub_64B1A1(a1: v125); /*0x901fb7*/
          v328 = 38 * (v352 - v126) + 235; /*0x901fca*/
          p_pvarg = SP_183__RESET; /*0x901fcb*/
          v127 = sub_414576(a1: &v351); /*0x901fd3*/
          IWzCanvas::DrawTextA( /*0x901fda*/
            this: v127,
            nLeft: p_pvarg,
            nTop: v328,
            sText: v329,
            pFont: SHIDWORD(v329),
            vAlpha: v330[0],
            vTabOrg: HIDWORD(v330[0]));
          sub_45BAEE(a1: &v354); /*0x901fe2*/
          LOBYTE(v355) = 78; /*0x901fee*/
          v128 = ZComAPI::ZComVariantCopy(pvarg: &v334); /*0x901ff2*/
          if ( v128 < 0 ) /*0x901ffa*/
            _com_issue_error(a1: v128); /*0x901ffd*/
          LOBYTE(v355) = 74; /*0x902009*/
          v129 = ZComAPI::ZComVariantCopy(pvarg: &v337); /*0x90200d*/
          if ( v129 < 0 ) /*0x902015*/
            _com_issue_error(a1: v129); /*0x902018*/
        }
        LOBYTE(v355) = 36; /*0x902020*/
        sub_4027E9(a1: &v350); /*0x902024*/
        v95 = ++v352; /*0x90202c*/
        v2 = v346; /*0x90202f*/
      }
    }
    sub_4156AB(a1: &v343); /*0x90203c*/
    sub_4156AB(a1: &v351); /*0x902044*/
    LOBYTE(v355) = 1; /*0x902049*/
LABEL_95:
    ZXString<char>::_Release(a1: &a2); /*0x902050*/
LABEL_294:
    _com_ptr_t<_com_IIID<IWzGr2DLayer,&__s_GUID const _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf>>::_com_ptr_t<_com_IIID<IWzGr2DLayer,&__s_GUID const _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf>>(a1: v336); /*0x903552*/
    v314 = *(v2 + 416) == 0; /*0x90355d*/
    LOBYTE(v355) = -71; /*0x903563*/
    HIDWORD(v330[0]) = &pvargSrc; /*0x903567*/
    if ( v314 ) /*0x903568*/
    {
      sub_426B85(pvargDest: &v334, pvargSrc: HIDWORD(v330[0])); /*0x903623*/
      LOBYTE(v355) = -66; /*0x90362f*/
      sub_426B85(pvargDest: &v337, pvargSrc: &pvargSrc); /*0x903633*/
      v330[0] = 0; /*0x903639*/
      HIDWORD(v329) = &v334; /*0x903640*/
      LODWORD(v329) = &v337; /*0x903647*/
      v328 = v319; /*0x903648*/
      v354 = &v328; /*0x90364b*/
      p_pvarg = SP_2578_UI_UIWINDOWIMG_USERINFO_MARRY_UNMARRIED_0; /*0x90364e*/
      v326 = &v328; /*0x903653*/
      LOBYTE(v355) = -65; /*0x903654*/
      v320 = StringPool::GetInstance(); /*0x903658*/
      StringPool::GetBSTR(this: v320, a2: v326, a3: p_pvarg); /*0x90365f*/
      p_pvarg = &v332; /*0x90366a*/
      LOBYTE(v355) = -65; /*0x90366b*/
      v321 = sub_406472(); /*0x90366f*/
      v322 = sub_402680(a1: v321); /*0x903676*/
      IWzResMan::GetObjectA(this: v322, pvargDest: p_pvarg, sUOL: v328, vParam: v329, vAux: SHIDWORD(v329)); /*0x90367d*/
      LOBYTE(v355) = -63; /*0x903684*/
      a2 = Ztl_variant_t::GetUnknown(a1: v330[0], a2: SBYTE4(v330[0])); /*0x90368d*/
      sub_539A6F(a1: &a2); /*0x90369a*/
      LOBYTE(v355) = -65; /*0x9036a5*/
      sub_403923(pvarg: &v332); /*0x9036a9*/
      LOBYTE(v355) = -66; /*0x9036b4*/
    }
    else
    {
      sub_426B85(pvargDest: &v334, pvargSrc: HIDWORD(v330[0])); /*0x903574*/
      LOBYTE(v355) = -70; /*0x903580*/
      sub_426B85(pvargDest: &v337, pvargSrc: &pvargSrc); /*0x903584*/
      v330[0] = 0; /*0x90358a*/
      HIDWORD(v329) = &v334; /*0x903591*/
      LODWORD(v329) = &v337; /*0x903598*/
      v328 = v315; /*0x903599*/
      v354 = &v328; /*0x90359c*/
      p_pvarg = SP_2577_UI_UIWINDOWIMG_USERINFO_MARRY_MARRIED_0; /*0x90359f*/
      v326 = &v328; /*0x9035a4*/
      LOBYTE(v355) = -69; /*0x9035a5*/
      v316 = StringPool::GetInstance(); /*0x9035a9*/
      StringPool::GetBSTR(this: v316, a2: v326, a3: p_pvarg); /*0x9035b0*/
      p_pvarg = &v332; /*0x9035bb*/
      LOBYTE(v355) = -69; /*0x9035bc*/
      v317 = sub_406472(); /*0x9035c0*/
      v318 = sub_402680(a1: v317); /*0x9035c7*/
      IWzResMan::GetObjectA(this: v318, pvargDest: p_pvarg, sUOL: v328, vParam: v329, vAux: SHIDWORD(v329)); /*0x9035ce*/
      LOBYTE(v355) = -67; /*0x9035d5*/
      a2 = Ztl_variant_t::GetUnknown(a1: v330[0], a2: SBYTE4(v330[0])); /*0x9035de*/
      sub_539A6F(a1: &a2); /*0x9035eb*/
      LOBYTE(v355) = -69; /*0x9035f6*/
      sub_403923(pvarg: &v332); /*0x9035fa*/
      LOBYTE(v355) = -70; /*0x903605*/
    }
    sub_403923(pvarg: &v337); /*0x903609*/
    LOBYTE(v355) = -71; /*0x90360e*/
    sub_403923(pvarg: &v334); /*0x9036c7*/
    sub_424B91(a1: 255, a2: 3); /*0x9036d9*/
    HIDWORD(v330[0]) = &v337; /*0x9036e4*/
    LOBYTE(v355) = -62; /*0x9036eb*/
    LODWORD(v330[0]) = sub_4281F8(a1: v336); /*0x9036f4*/
    v329 = 0x1E0000000ALL; /*0x9036f7*/
    sub_414576(a1: &v342); /*0x9036fc*/
    sub_424BA7(a1: v329, a2: HIDWORD(v329), a3: v330[0], a4: HIDWORD(v330[0])); /*0x903703*/
    LOBYTE(v355) = -71; /*0x90370e*/
    sub_403923(pvarg: &v337); /*0x903712*/
    _com_ptr_t<_com_IIID<IWzGr2DLayer,&__s_GUID const _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf>>::~_com_ptr_t<_com_IIID<IWzGr2DLayer,&__s_GUID const _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf>>(a1: v336); /*0x90371d*/
    _com_ptr_t<_com_IIID<IWzGr2DLayer,&__s_GUID const _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf>>::~_com_ptr_t<_com_IIID<IWzGr2DLayer,&__s_GUID const _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf>>(a1: &v342); /*0x903725*/
    goto LABEL_298; /*0x903725*/
  }
  if ( v43 != 2 ) /*0x90205d*/
  {
    if ( v43 == 3 ) /*0x902ade*/
    {
      v352 = 0; /*0x902ae4*/
      LOBYTE(v355) = -121; /*0x902aed*/
      CWnd::GetCanvas(a1: &a2); /*0x902af1*/
      v221 = *(v2 + 442); /*0x902af6*/
      LOBYTE(v355) = -120; /*0x902afe*/
      if ( v221 != 0 ) /*0x902b02*/
      {
        HIDWORD(v330[0]) = 5515; /*0x902b0b*/
        LODWORD(v330[0]) = &v354; /*0x902b10*/
        v222 = StringPool::GetInstance(); /*0x902b11*/
        v223 = StringPool::GetString(this: v222, result: v330[0], nIdx: SHIDWORD(v330[0])); /*0x902b18*/
        HIDWORD(v330[0]) = *(v221 + 12); /*0x902b1d*/
        LODWORD(v330[0]) = *v223; /*0x902b22*/
        LOBYTE(v355) = -119; /*0x902b27*/
        ZXString<char>::Format(a1: &v352, a2: v330[0], a3: SBYTE4(v330[0])); /*0x902b2b*/
        LOBYTE(v355) = -120; /*0x902b36*/
        ZXString<char>::_Release(a1: &v354); /*0x902b3a*/
        VariantInit(pvarg: &v348); /*0x902b49*/
        v224 = ZtlTaskMemFreeImp(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x902b50*/
        if ( v224 < 0 ) /*0x902b59*/
          _com_issue_error(a1: v224); /*0x902b5c*/
        LOBYTE(v355) = -118; /*0x902b65*/
        VariantInit(&pvarg); /*0x902b69*/
        v225 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x902b70*/
        if ( v225 < 0 ) /*0x902b79*/
          _com_issue_error(a1: v225); /*0x902b7c*/
        HIDWORD(v330[0]) = &v348; /*0x902b84*/
        LODWORD(v330[0]) = &pvarg; /*0x902b88*/
        LOBYTE(v355) = -117; /*0x902b8f*/
        v226 = get_basic_font(a1: &v354, fType: 1); /*0x902b93*/
        LOBYTE(v355) = -116; /*0x902b9c*/
        HIDWORD(v329) = sub_4284A5(this: v226); /*0x902ba5*/
        v227 = sub_428226(a1: &v352); /*0x902ba9*/
        LODWORD(v329) = v228; /*0x902bae*/
        v353 = &v329; /*0x902bb1*/
        _bstr_t::_bstr_t(a1: v227); /*0x902bb5*/
        v328 = 181; /*0x902bba*/
        p_pvarg = SP_101_MAPLESTORY_ERROR__LOADING_TAMINGMOB_FAILED; /*0x902bbf*/
        LOBYTE(v355) = -116; /*0x902bc4*/
        v229 = sub_414576(a1: &a2); /*0x902bc8*/
        IWzCanvas::DrawTextA( /*0x902bcf*/
          this: v229,
          nLeft: p_pvarg,
          nTop: v328,
          sText: v329,
          pFont: SHIDWORD(v329),
          vAlpha: v330[0],
          vTabOrg: HIDWORD(v330[0]));
        LOBYTE(v355) = -117; /*0x902bd9*/
        if ( v354 != 0 ) /*0x902bdd*/
          (*(*v354 + 8))(a1: v354); /*0x902be2*/
        LOBYTE(v355) = -118; /*0x902be9*/
        v230 = ZComAPI::ZComVariantCopy(&pvarg); /*0x902bed*/
        if ( v230 < 0 ) /*0x902bf5*/
          _com_issue_error(a1: v230); /*0x902bf8*/
        LOBYTE(v355) = -120; /*0x902c01*/
        v231 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x902c05*/
        if ( v231 < 0 ) /*0x902c0d*/
          _com_issue_error(a1: v231); /*0x902c10*/
        if ( dword_BED610 != 0 ) /*0x902c1c*/
        {
          sub_402FAB(a1: 255, a2: 3); /*0x902c2f*/
          HIDWORD(v330[0]) = *(*(v346 + 442) + 12); /*0x902c3d*/
          LOBYTE(v355) = -114; /*0x902c4a*/
          v354 = *sub_685C0F(a1: &String, a2: HIDWORD(v330[0])); /*0x902c55*/
          LOBYTE(v355) = -113; /*0x902c5d*/
          if ( a2 == 0 ) /*0x902c61*/
            _com_issue_error(a1: -2147467261); /*0x902c68*/
          left = a2->left; /*0x902c70*/
          v328 = v354; /*0x902c77*/
          v329 = *&v337.vt; /*0x902c80*/
          p_pvarg = SP_182__START; /*0x902c82*/
          v330[0] = v337.cyVal.int64; /*0x902c87*/
          v326 = 14; /*0x902c88*/
          v353 = a2; /*0x902c8b*/
          v233 = (*(left + 128))( /*0x902c8f*/
                   a1: a2,
                   a2: 14,
                   a3: 182,
                   a4: v354,
                   a5: *&v337.vt,
                   a6: v337.decVal.Hi32,
                   a7: v337.lVal,
                   a8: v337.cyVal.Hi);
          if ( v233 < 0 ) /*0x902c97*/
            _com_issue_errorex(a1: v233, a2: v353, a3: _GUID_7600dc6c_9328_4bff_9624_5b0f5c01179e); /*0x902ca2*/
          LOBYTE(v355) = -114; /*0x902cac*/
          if ( String != 0 ) /*0x902cb0*/
            (*(*String + 8))(a1: String); /*0x902cb5*/
          LOBYTE(v355) = -120; /*0x902cbf*/
          v234 = ZComAPI::ZComVariantCopy(pvarg: &v337); /*0x902cc3*/
          if ( v234 < 0 ) /*0x902ccb*/
            _com_issue_error(a1: v234); /*0x902cce*/
        }
        v235 = v346; /*0x902cd3*/
        v236 = *(*(v346 + 442) + 32); /*0x902cdc*/
        if ( v236 != 0 && *v236 != 0 ) /*0x902cea*/
        {
          v237 = VariantInit; /*0x902cf0*/
          VariantInit(pvarg: &v348); /*0x902cfa*/
          v238 = ZtlTaskMemFreeImp(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x902d01*/
          if ( v238 < 0 ) /*0x902d0a*/
            _com_issue_error(a1: v238); /*0x902d0d*/
          LOBYTE(v355) = -112; /*0x902d16*/
          VariantInit(&pvarg); /*0x902d1a*/
          v239 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x902d21*/
          if ( v239 < 0 ) /*0x902d2a*/
            _com_issue_error(a1: v239); /*0x902d2d*/
          HIDWORD(v330[0]) = &v348; /*0x902d35*/
          LODWORD(v330[0]) = &pvarg; /*0x902d39*/
          LOBYTE(v355) = -111; /*0x902d40*/
          v240 = get_basic_font(a1: &v354, fType: 1); /*0x902d44*/
          LOBYTE(v355) = -110; /*0x902d4d*/
          HIDWORD(v329) = sub_4284A5(this: v240); /*0x902d56*/
          v241 = sub_867D38(a1: v235 + 1764); /*0x902d5d*/
          v242 = sub_428226(a1: v241 + 32); /*0x902d67*/
          LODWORD(v329) = v243; /*0x902d6c*/
          v353 = &v329; /*0x902d6f*/
          _bstr_t::_bstr_t(a1: v242); /*0x902d73*/
          v328 = 199; /*0x902d78*/
          p_pvarg = SP_101_MAPLESTORY_ERROR__LOADING_TAMINGMOB_FAILED; /*0x902d7d*/
          LOBYTE(v355) = -110; /*0x902d82*/
          v244 = sub_414576(a1: &a2); /*0x902d86*/
          IWzCanvas::DrawTextA( /*0x902d8d*/
            this: v244,
            nLeft: p_pvarg,
            nTop: v328,
            sText: v329,
            pFont: SHIDWORD(v329),
            vAlpha: v330[0],
            vTabOrg: HIDWORD(v330[0]));
          LOBYTE(v355) = -111; /*0x902d97*/
          if ( v354 != 0 ) /*0x902d9b*/
            (*(*v354 + 8))(a1: v354); /*0x902da0*/
          LOBYTE(v355) = -112; /*0x902da7*/
          v245 = ZComAPI::ZComVariantCopy(&pvarg); /*0x902dab*/
          if ( v245 < 0 ) /*0x902db3*/
            _com_issue_error(a1: v245); /*0x902db6*/
          LOBYTE(v355) = -120; /*0x902dbf*/
          v246 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x902dc3*/
          if ( v246 < 0 ) /*0x902dcb*/
            _com_issue_error(a1: v246); /*0x902dce*/
        }
        else
        {
          v237 = VariantInit; /*0x902dd5*/
        }
        v354 = *(v346 + 442); /*0x902de4*/
        HIDWORD(v330[0]) = 5515; /*0x902def*/
        LODWORD(v330[0]) = &v353; /*0x902df0*/
        v247 = StringPool::GetInstance(); /*0x902df1*/
        v248 = StringPool::GetString(this: v247, result: v330[0], nIdx: SHIDWORD(v330[0])); /*0x902df8*/
        HIDWORD(v330[0]) = v354[6]; /*0x902e00*/
        LODWORD(v330[0]) = *v248; /*0x902e05*/
        LOBYTE(v355) = -108; /*0x902e0a*/
        ZXString<char>::Format(a1: &v352, a2: v330[0], a3: SBYTE4(v330[0])); /*0x902e0e*/
        LOBYTE(v355) = -120; /*0x902e19*/
        ZXString<char>::_Release(a1: &v353); /*0x902e1d*/
        v237(pvarg: &v348); /*0x902e26*/
        v249 = ZtlTaskMemFreeImp(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x902e2d*/
        if ( v249 < 0 ) /*0x902e36*/
          _com_issue_error(a1: v249); /*0x902e39*/
        LOBYTE(v355) = -107; /*0x902e42*/
        v237(&pvarg); /*0x902e46*/
        v250 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x902e4d*/
        if ( v250 < 0 ) /*0x902e56*/
          _com_issue_error(a1: v250); /*0x902e59*/
        HIDWORD(v330[0]) = &v348; /*0x902e61*/
        LODWORD(v330[0]) = &pvarg; /*0x902e65*/
        LOBYTE(v355) = -106; /*0x902e6c*/
        v251 = get_basic_font(a1: &v354, fType: 1); /*0x902e70*/
        LOBYTE(v355) = -105; /*0x902e79*/
        HIDWORD(v329) = sub_4284A5(this: v251); /*0x902e82*/
        v252 = sub_428226(a1: &v352); /*0x902e86*/
        LODWORD(v329) = v253; /*0x902e8b*/
        v353 = &v329; /*0x902e8e*/
        _bstr_t::_bstr_t(a1: v252); /*0x902e92*/
        v328 = 217; /*0x902e97*/
        p_pvarg = SP_101_MAPLESTORY_ERROR__LOADING_TAMINGMOB_FAILED; /*0x902e9c*/
        LOBYTE(v355) = -105; /*0x902ea1*/
        v254 = sub_414576(a1: &a2); /*0x902ea5*/
        IWzCanvas::DrawTextA( /*0x902eac*/
          this: v254,
          nLeft: p_pvarg,
          nTop: v328,
          sText: v329,
          pFont: SHIDWORD(v329),
          vAlpha: v330[0],
          vTabOrg: HIDWORD(v330[0]));
        LOBYTE(v355) = -106; /*0x902eb6*/
        if ( v354 != 0 ) /*0x902eba*/
          (*(*v354 + 8))(a1: v354); /*0x902ebf*/
        LOBYTE(v355) = -107; /*0x902ec6*/
        v255 = ZComAPI::ZComVariantCopy(&pvarg); /*0x902eca*/
        if ( v255 < 0 ) /*0x902ed2*/
          _com_issue_error(a1: v255); /*0x902ed5*/
        LOBYTE(v355) = -120; /*0x902ede*/
        v256 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x902ee2*/
        if ( v256 < 0 ) /*0x902eea*/
          _com_issue_error(a1: v256); /*0x902eed*/
        v354 = *(v346 + 442); /*0x902efb*/
        HIDWORD(v330[0]) = 5515; /*0x902f01*/
        LODWORD(v330[0]) = &v353; /*0x902f02*/
        v257 = StringPool::GetInstance(); /*0x902f03*/
        v258 = StringPool::GetString(this: v257, result: v330[0], nIdx: SHIDWORD(v330[0])); /*0x902f0a*/
        HIDWORD(v330[0]) = v354[4]; /*0x902f12*/
        LODWORD(v330[0]) = *v258; /*0x902f17*/
        LOBYTE(v355) = -103; /*0x902f1c*/
        ZXString<char>::Format(a1: &v352, a2: v330[0], a3: SBYTE4(v330[0])); /*0x902f20*/
        LOBYTE(v355) = -120; /*0x902f2b*/
        ZXString<char>::_Release(a1: &v353); /*0x902f2f*/
        v237(pvarg: &v348); /*0x902f38*/
        v259 = ZtlTaskMemFreeImp(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x902f3f*/
        if ( v259 < 0 ) /*0x902f48*/
          _com_issue_error(a1: v259); /*0x902f4b*/
        LOBYTE(v355) = -102; /*0x902f54*/
        v237(&pvarg); /*0x902f58*/
        v260 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x902f5f*/
        if ( v260 < 0 ) /*0x902f68*/
          _com_issue_error(a1: v260); /*0x902f6b*/
        HIDWORD(v330[0]) = &v348; /*0x902f73*/
        LODWORD(v330[0]) = &pvarg; /*0x902f77*/
        LOBYTE(v355) = -101; /*0x902f7e*/
        v261 = get_basic_font(a1: &v354, fType: 1); /*0x902f82*/
        LOBYTE(v355) = -100; /*0x902f8b*/
        HIDWORD(v329) = sub_4284A5(this: v261); /*0x902f94*/
        v262 = sub_428226(a1: &v352); /*0x902f98*/
        LODWORD(v329) = v263; /*0x902f9d*/
        v353 = &v329; /*0x902fa0*/
        _bstr_t::_bstr_t(a1: v262); /*0x902fa4*/
        v328 = 217; /*0x902fa9*/
        p_pvarg = SP_168_YOU_HAVE_ENTERED_AN_INVALID_CHARACTER_NAME; /*0x902fae*/
        LOBYTE(v355) = -100; /*0x902fb6*/
        v264 = sub_414576(a1: &a2); /*0x902fba*/
        IWzCanvas::DrawTextA( /*0x902fc1*/
          this: v264,
          nLeft: p_pvarg,
          nTop: v328,
          sText: v329,
          pFont: SHIDWORD(v329),
          vAlpha: v330[0],
          vTabOrg: HIDWORD(v330[0]));
        LOBYTE(v355) = -101; /*0x902fcb*/
        if ( v354 != 0 ) /*0x902fcf*/
          (*(*v354 + 8))(a1: v354); /*0x902fd4*/
        LOBYTE(v355) = -102; /*0x902fdb*/
        v265 = ZComAPI::ZComVariantCopy(&pvarg); /*0x902fdf*/
        if ( v265 < 0 ) /*0x902fe7*/
          _com_issue_error(a1: v265); /*0x902fea*/
        LOBYTE(v355) = -120; /*0x902ff3*/
        v266 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x902ff7*/
        if ( v266 < 0 ) /*0x902fff*/
          _com_issue_error(a1: v266); /*0x903002*/
        v354 = *(v346 + 442); /*0x903010*/
        HIDWORD(v330[0]) = 5515; /*0x903016*/
        LODWORD(v330[0]) = &v353; /*0x903017*/
        v267 = StringPool::GetInstance(); /*0x903018*/
        v268 = StringPool::GetString(this: v267, result: v330[0], nIdx: SHIDWORD(v330[0])); /*0x90301f*/
        HIDWORD(v330[0]) = v354[5]; /*0x903027*/
        LODWORD(v330[0]) = *v268; /*0x90302c*/
        LOBYTE(v355) = -98; /*0x903031*/
        ZXString<char>::Format(a1: &v352, a2: v330[0], a3: SBYTE4(v330[0])); /*0x903035*/
        LOBYTE(v355) = -120; /*0x903040*/
        ZXString<char>::_Release(a1: &v353); /*0x903044*/
        v237(pvarg: &v348); /*0x90304d*/
        v269 = ZtlTaskMemFreeImp(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x903054*/
        if ( v269 < 0 ) /*0x90305d*/
          _com_issue_error(a1: v269); /*0x903060*/
        LOBYTE(v355) = -97; /*0x903069*/
        v237(&pvarg); /*0x90306d*/
        v270 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x903074*/
        if ( v270 < 0 ) /*0x90307d*/
          _com_issue_error(a1: v270); /*0x903080*/
        HIDWORD(v330[0]) = &v348; /*0x903088*/
        LODWORD(v330[0]) = &pvarg; /*0x90308c*/
        LOBYTE(v355) = -96; /*0x903093*/
        v271 = get_basic_font(a1: &v354, fType: 1); /*0x903097*/
        LOBYTE(v355) = -95; /*0x9030a0*/
        HIDWORD(v329) = sub_4284A5(this: v271); /*0x9030a9*/
        v272 = sub_428226(a1: &v352); /*0x9030ad*/
        LODWORD(v329) = v273; /*0x9030b2*/
        v353 = &v329; /*0x9030b5*/
        _bstr_t::_bstr_t(a1: v272); /*0x9030b9*/
        v328 = 217; /*0x9030be*/
        p_pvarg = SP_236__POINTER03; /*0x9030c3*/
        LOBYTE(v355) = -95; /*0x9030cb*/
        v274 = sub_414576(a1: &a2); /*0x9030cf*/
        IWzCanvas::DrawTextA( /*0x9030d6*/
          this: v274,
          nLeft: p_pvarg,
          nTop: v328,
          sText: v329,
          pFont: SHIDWORD(v329),
          vAlpha: v330[0],
          vTabOrg: HIDWORD(v330[0]));
        LOBYTE(v355) = -96; /*0x9030e0*/
        if ( v354 != 0 ) /*0x9030e4*/
          (*(*v354 + 8))(a1: v354); /*0x9030e9*/
        LOBYTE(v355) = -97; /*0x9030f0*/
        v275 = ZComAPI::ZComVariantCopy(&pvarg); /*0x9030f4*/
        if ( v275 < 0 ) /*0x9030fc*/
          _com_issue_error(a1: v275); /*0x9030ff*/
        LOBYTE(v355) = -120; /*0x903108*/
        v276 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x90310c*/
        v220 = HIDWORD(v330[0]); /*0x903113*/
        if ( v276 < 0 ) /*0x903114*/
          _com_issue_error(a1: v276); /*0x903117*/
      }
      v277 = *(v346 + 444); /*0x90311f*/
      if ( v277 != 0 ) /*0x903127*/
      {
        v278 = *(v277 + 12); /*0x90312d*/
        if ( v278 != 0 ) /*0x903132*/
        {
          v330[0] = 0; /*0x90313b*/
          v329 = 0; /*0x90313d*/
          v328 = 0; /*0x90313e*/
          p_pvarg = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x90313f*/
          v326 = 278; /*0x903140*/
          v325 = 8; /*0x903145*/
          v324 = v278; /*0x903147*/
          v323 = v220; /*0x903148*/
          v354 = &v323; /*0x90314e*/
          sub_4281DD(a1: &a2); /*0x903152*/
          LOBYTE(v355) = -120; /*0x903157*/
          sub_4283F8(); /*0x90315b*/
          sub_5D6458( /*0x903162*/
            a1: v323,
            a2: v324,
            a3: v325,
            a4: v326,
            a5: p_pvarg,
            a6: v328,
            a7: v329,
            a8: HIDWORD(v329),
            a9: v330[0],
            a10: HIDWORD(v330[0]));
          v279 = VariantInit; /*0x903167*/
          VariantInit(pvarg: &v348); /*0x903171*/
          v280 = ZtlTaskMemFreeImp(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x903178*/
          if ( v280 < 0 ) /*0x903181*/
            _com_issue_error(a1: v280); /*0x903184*/
          LOBYTE(v355) = -92; /*0x90318d*/
          VariantInit(&pvarg); /*0x903191*/
          v281 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x903198*/
          if ( v281 < 0 ) /*0x9031a1*/
            _com_issue_error(a1: v281); /*0x9031a4*/
          HIDWORD(v330[0]) = &v348; /*0x9031ac*/
          LODWORD(v330[0]) = &pvarg; /*0x9031b0*/
          LOBYTE(v355) = -91; /*0x9031b7*/
          v282 = get_basic_font(a1: &v353, fType: 1); /*0x9031bb*/
          LOBYTE(v355) = -90; /*0x9031c4*/
          v329 = __PAIR64__(sub_4284A5(this: v282), v278); /*0x9031d1*/
          v328 = &v354; /*0x9031d2*/
          sub_4283F8(); /*0x9031d3*/
          v283 = CItemInfo::GetItemName(a1: v328, a2: v329); /*0x9031da*/
          LOBYTE(v355) = -89; /*0x9031e1*/
          v284 = sub_428226(a1: v283); /*0x9031e5*/
          LODWORD(v329) = v285; /*0x9031ea*/
          String = &v329; /*0x9031ed*/
          _bstr_t::_bstr_t(a1: v284); /*0x9031f1*/
          v328 = 247; /*0x9031f6*/
          p_pvarg = SP_101_MAPLESTORY_ERROR__LOADING_TAMINGMOB_FAILED; /*0x9031fb*/
          LOBYTE(v355) = -89; /*0x903200*/
          v286 = sub_414576(a1: &a2); /*0x903204*/
          IWzCanvas::DrawTextA( /*0x90320b*/
            this: v286,
            nLeft: p_pvarg,
            nTop: v328,
            sText: v329,
            pFont: SHIDWORD(v329),
            vAlpha: v330[0],
            vTabOrg: HIDWORD(v330[0]));
          LOBYTE(v355) = -90; /*0x903213*/
          ZXString<char>::_Release(a1: &v354); /*0x903217*/
          LOBYTE(v355) = -91; /*0x903221*/
          if ( v353 != 0 ) /*0x903225*/
            (*(*v353 + 8))(a1: v353); /*0x90322a*/
          LOBYTE(v355) = -92; /*0x903231*/
          v287 = ZComAPI::ZComVariantCopy(&pvarg); /*0x903235*/
          if ( v287 < 0 ) /*0x90323d*/
            _com_issue_error(a1: v287); /*0x903240*/
          LOBYTE(v355) = -120; /*0x903249*/
          v288 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x90324d*/
          if ( v288 < 0 ) /*0x903255*/
            _com_issue_error(a1: v288); /*0x903258*/
        }
        else
        {
          v279 = VariantInit; /*0x90325f*/
        }
        String = *(v346 + 444); /*0x90326e*/
        v289 = *(String + 4); /*0x903271*/
        if ( v289 != 0 ) /*0x903276*/
          v290 = *(v289 - 4); /*0x90327c*/
        else
          LOBYTE(v290) = 0; /*0x903278*/
        HIDWORD(v330[0]) = 5515; /*0x903282*/
        LODWORD(v330[0]) = &v354; /*0x903287*/
        v291 = StringPool::GetInstance(); /*0x903288*/
        v292 = *StringPool::GetString(this: v291, result: v330[0], nIdx: SHIDWORD(v330[0])); /*0x903294*/
        Unknown = 0; /*0x903296*/
        LOBYTE(v355) = -86; /*0x9032a0*/
        v293 = ZXString<char>::Format(a1: &Unknown, a2: v292, a3: v290); /*0x9032a4*/
        v350 = 0; /*0x9032a9*/
        ZXString<char>::operator=(a1: v293); /*0x9032b4*/
        LOBYTE(v355) = -83; /*0x9032bc*/
        ZXString<char>::_Release(a1: &Unknown); /*0x9032c0*/
        LOBYTE(v355) = -84; /*0x9032c8*/
        ZXString<char>::_Release(a1: &v354); /*0x9032cc*/
        v279(pvarg: &v348); /*0x9032d5*/
        v294 = ZtlTaskMemFreeImp(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x9032dc*/
        if ( v294 < 0 ) /*0x9032e5*/
          _com_issue_error(a1: v294); /*0x9032e8*/
        LOBYTE(v355) = -82; /*0x9032f1*/
        v279(&pvarg); /*0x9032f5*/
        v295 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x9032fc*/
        if ( v295 < 0 ) /*0x903305*/
          _com_issue_error(a1: v295); /*0x903308*/
        HIDWORD(v330[0]) = &v348; /*0x903310*/
        LODWORD(v330[0]) = &pvarg; /*0x903314*/
        LOBYTE(v355) = -81; /*0x90331b*/
        v296 = get_basic_font(a1: &v353, fType: 1); /*0x90331f*/
        LOBYTE(v355) = -80; /*0x903328*/
        HIDWORD(v329) = sub_4284A5(this: v296); /*0x903331*/
        v297 = sub_428226(a1: &v350); /*0x903335*/
        LODWORD(v329) = v298; /*0x90333a*/
        v345 = &v329; /*0x90333d*/
        _bstr_t::_bstr_t(a1: v297); /*0x903341*/
        v328 = 265; /*0x903346*/
        p_pvarg = SP_101_MAPLESTORY_ERROR__LOADING_TAMINGMOB_FAILED; /*0x90334b*/
        LOBYTE(v355) = -80; /*0x903350*/
        v299 = sub_414576(a1: &a2); /*0x903354*/
        IWzCanvas::DrawTextA( /*0x90335b*/
          this: v299,
          nLeft: p_pvarg,
          nTop: v328,
          sText: v329,
          pFont: SHIDWORD(v329),
          vAlpha: v330[0],
          vTabOrg: HIDWORD(v330[0]));
        LOBYTE(v355) = -81; /*0x903365*/
        if ( v353 != 0 ) /*0x903369*/
          (*(*v353 + 8))(a1: v353); /*0x90336e*/
        LOBYTE(v355) = -82; /*0x903375*/
        v300 = ZComAPI::ZComVariantCopy(&pvarg); /*0x903379*/
        if ( v300 < 0 ) /*0x903381*/
          _com_issue_error(a1: v300); /*0x903384*/
        LOBYTE(v355) = -84; /*0x90338d*/
        v301 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x903391*/
        if ( v301 < 0 ) /*0x903399*/
          _com_issue_error(a1: v301); /*0x90339c*/
        v302 = v346; /*0x9033a1*/
        v303 = *(*(v346 + 393) + 56); /*0x9033aa*/
        v343 = 301; /*0x9033ad*/
        while ( 1 ) /*0x9033ba*/
        {
          v304 = *(*(v302 + 444) + 16); /*0x9033ba*/
          if ( v304 == 0 || v303 >= *(v304 - 4) || v303 >= *(*(v302 + 393) + 56) + 4 ) /*0x9033dc*/
            break; /*0x9033dc*/
          QuestDisplayMedal = CQuestMan::GetQuestDisplayMedal(this: dword_BED614, a2: *(*(String + 4) + 2 * v303)); /*0x9033f3*/
          if ( QuestDisplayMedal != 0 ) /*0x9033fa*/
          {
            CItemInfo::GetItemName(a1: &v351, a2: QuestDisplayMedal); /*0x90340b*/
            HIDWORD(v330[0]) = 4542; /*0x903413*/
            LODWORD(v330[0]) = &v345; /*0x903418*/
            LOBYTE(v355) = -78; /*0x903419*/
            v306 = StringPool::GetInstance(); /*0x90341d*/
            v307 = StringPool::GetString(this: v306, result: v330[0], nIdx: SHIDWORD(v330[0])); /*0x903424*/
            HIDWORD(v330[0]) = &sStrDefault; /*0x903429*/
            LOBYTE(v355) = -77; /*0x903430*/
            v308 = sub_428226(a1: v307); /*0x903434*/
            v309 = sub_5DB9D7(a1: &v353, a2: v308, a3: HIDWORD(v330[0])); /*0x903441*/
            LOBYTE(v355) = -76; /*0x90344a*/
            ZXString<char>::operator=(a1: v309); /*0x90344e*/
            LOBYTE(v355) = -77; /*0x903456*/
            ZXString<char>::_Release(a1: &v353); /*0x90345a*/
            LOBYTE(v355) = -78; /*0x903462*/
            ZXString<char>::_Release(a1: &v345); /*0x903466*/
            if ( sub_538A83(a1: &v351) == 0 ) /*0x903475*/
            {
              sub_426B85(pvargDest: &v334, pvargSrc: &pvargSrc); /*0x903482*/
              LOBYTE(v355) = -75; /*0x90348e*/
              sub_426B85(pvargDest: &v337, pvargSrc: &pvargSrc); /*0x903492*/
              HIDWORD(v330[0]) = &v334; /*0x90349d*/
              LODWORD(v330[0]) = &v337; /*0x9034a4*/
              LOBYTE(v355) = -74; /*0x9034ab*/
              v310 = get_basic_font(a1: &v340, fType: 1); /*0x9034af*/
              LOBYTE(v355) = -73; /*0x9034b8*/
              HIDWORD(v329) = sub_4284A5(this: v310); /*0x9034c1*/
              v311 = sub_428226(a1: &v351); /*0x9034c5*/
              LODWORD(v329) = v312; /*0x9034ca*/
              v335 = &v329; /*0x9034cd*/
              _bstr_t::_bstr_t(a1: v311); /*0x9034d4*/
              v328 = v343; /*0x9034d9*/
              p_pvarg = SP_52_ASSASSIN; /*0x9034df*/
              LOBYTE(v355) = -73; /*0x9034e1*/
              v313 = sub_414576(a1: &a2); /*0x9034e5*/
              IWzCanvas::DrawTextA( /*0x9034ec*/
                this: v313,
                nLeft: p_pvarg,
                nTop: v328,
                sText: v329,
                pFont: SHIDWORD(v329),
                vAlpha: v330[0],
                vTabOrg: HIDWORD(v330[0]));
              sub_428498(a1: &v340); /*0x9034f4*/
              LOBYTE(v355) = -75; /*0x9034ff*/
              sub_403923(pvarg: &v337); /*0x903503*/
              LOBYTE(v355) = -78; /*0x90350e*/
              sub_403923(pvarg: &v334); /*0x903512*/
              v343 += 18; /*0x903517*/
            }
            LOBYTE(v355) = -84; /*0x90351e*/
            ZXString<char>::_Release(a1: &v351); /*0x903522*/
          }
          ++v303; /*0x903527*/
        }
        LOBYTE(v355) = -120; /*0x903530*/
        ZXString<char>::_Release(a1: &v350); /*0x903534*/
      }
      _com_ptr_t<_com_IIID<IWzGr2DLayer,&__s_GUID const _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf>>::~_com_ptr_t<_com_IIID<IWzGr2DLayer,&__s_GUID const _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf>>(a1: &a2); /*0x90353c*/
      LOBYTE(v355) = 1; /*0x903544*/
      ZXString<char>::_Release(a1: &v352); /*0x903548*/
      v2 = v346; /*0x90354d*/
    }
    goto LABEL_294; /*0x90354d*/
  }
  a2 = 0; /*0x902063*/
  LOBYTE(v355) = 82; /*0x90206c*/
  CWnd::GetCanvas(a1: &v351); /*0x902070*/
  LOBYTE(v355) = 83; /*0x90207c*/
  sub_402F85(pvargDest: &v334, pvargSrc: &pvargSrc); /*0x902080*/
  LOBYTE(v355) = 84; /*0x90208c*/
  sub_402F85(pvargDest: &v337, pvargSrc: &pvargSrc); /*0x902090*/
  v330[0] = 0; /*0x902096*/
  HIDWORD(v329) = &v334; /*0x90209d*/
  LODWORD(v329) = &v337; /*0x9020a4*/
  v328 = v130; /*0x9020a5*/
  v353 = &v328; /*0x9020a8*/
  p_pvarg = SP_2564_UI_UIWINDOWIMG_USERINFO_REQLV; /*0x9020ab*/
  v326 = &v328; /*0x9020b0*/
  LOBYTE(v355) = 85; /*0x9020b1*/
  v131 = StringPool::GetInstance(); /*0x9020b5*/
  StringPool::GetBSTR(this: v131, a2: v326, a3: p_pvarg); /*0x9020bc*/
  p_pvarg = &v332; /*0x9020c7*/
  LOBYTE(v355) = 85; /*0x9020c8*/
  v132 = sub_406472(); /*0x9020cc*/
  v133 = sub_402680(a1: v132); /*0x9020d3*/
  IWzResMan::GetObjectA(this: v133, pvargDest: p_pvarg, sUOL: v328, vParam: v329, vAux: SHIDWORD(v329)); /*0x9020da*/
  LOBYTE(v355) = 87; /*0x9020e1*/
  v354 = Ztl_variant_t::GetUnknown(a1: v330[0], a2: SBYTE4(v330[0])); /*0x9020ec*/
  v350 = 0; /*0x9020ef*/
  if ( v354 != 0 ) /*0x9020f2*/
  {
    v134 = sub_41E527(a1: &v354); /*0x9020fb*/
    if ( v134 < 0 && v134 != -2147467262 ) /*0x902109*/
      _com_issue_error(a1: v134); /*0x90210c*/
  }
  LOBYTE(v355) = 91; /*0x902118*/
  v135 = ZComAPI::ZComVariantCopy(pvarg: &v332); /*0x90211c*/
  if ( v135 < 0 ) /*0x902124*/
    _com_issue_error(a1: v135); /*0x902127*/
  LOBYTE(v355) = 90; /*0x902133*/
  v136 = ZComAPI::ZComVariantCopy(pvarg: &v337); /*0x902137*/
  if ( v136 < 0 ) /*0x90213f*/
    _com_issue_error(a1: v136); /*0x902142*/
  LOBYTE(v355) = 89; /*0x90214e*/
  v137 = ZComAPI::ZComVariantCopy(pvarg: &v334); /*0x902152*/
  if ( v137 < 0 ) /*0x90215a*/
    _com_issue_error(a1: v137); /*0x90215d*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x902166*/
  LOBYTE(v355) = 92; /*0x90216f*/
  sub_402F85(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x902173*/
  LOBYTE(v355) = 93; /*0x90217c*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x902180*/
  HIDWORD(v330[0]) = &pvargDest; /*0x902188*/
  LODWORD(v330[0]) = &v348; /*0x90218c*/
  LOBYTE(v355) = 94; /*0x902192*/
  v138 = get_basic_font(a1: &String, fType: 0); /*0x902196*/
  LOBYTE(v355) = 95; /*0x90219f*/
  HIDWORD(v329) = sub_4284A5(this: v138); /*0x9021a8*/
  v139 = sub_428226(a1: v2 + 1732); /*0x9021af*/
  LODWORD(v329) = v140; /*0x9021b4*/
  v345 = &v329; /*0x9021b7*/
  _bstr_t::_bstr_t(a1: v139); /*0x9021bb*/
  v328 = 269; /*0x9021c0*/
  p_pvarg = &pvarg; /*0x9021c8*/
  LOBYTE(v355) = 96; /*0x9021cf*/
  v141 = sub_428226(a1: v2 + 1732); /*0x9021d3*/
  v326 = v142; /*0x9021d8*/
  v340 = &v326; /*0x9021db*/
  _bstr_t::_bstr_t(a1: v141); /*0x9021df*/
  LOBYTE(v355) = 97; /*0x9021e9*/
  v143 = get_basic_font(a1: &v353, fType: 0); /*0x9021ed*/
  LOBYTE(v355) = 99; /*0x9021f6*/
  sub_4284A8(a1: v143); /*0x9021fa*/
  p_pvarg = 52 - (sub_42782E(a1: v326, a2: p_pvarg) >> 1); /*0x90220d*/
  LOBYTE(v355) = 100; /*0x902211*/
  v144 = sub_414576(a1: &v351); /*0x902215*/
  IWzCanvas::DrawTextA( /*0x90221c*/
    this: v144,
    nLeft: p_pvarg,
    nTop: v328,
    sText: v329,
    pFont: SHIDWORD(v329),
    vAlpha: v330[0],
    vTabOrg: HIDWORD(v330[0]));
  LOBYTE(v355) = 95; /*0x902226*/
  if ( v353 != 0 ) /*0x90222a*/
    (*(*v353 + 8))(a1: v353); /*0x90222f*/
  LOBYTE(v355) = 94; /*0x902237*/
  if ( String != 0 ) /*0x90223b*/
    (*(*String + 8))(a1: String); /*0x902240*/
  LOBYTE(v355) = 93; /*0x902247*/
  v145 = ZComAPI::ZComVariantCopy(&pvarg); /*0x90224b*/
  if ( v145 < 0 ) /*0x902253*/
    _com_issue_error(a1: v145); /*0x902256*/
  LOBYTE(v355) = 92; /*0x90225f*/
  v146 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x902263*/
  if ( v146 < 0 ) /*0x90226b*/
    _com_issue_error(a1: v146); /*0x90226e*/
  LOBYTE(v355) = 89; /*0x902277*/
  v147 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x90227b*/
  if ( v147 < 0 ) /*0x902283*/
    _com_issue_error(a1: v147); /*0x902286*/
  sub_402F85(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x90228f*/
  LOBYTE(v355) = 101; /*0x902298*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x90229c*/
  HIDWORD(v330[0]) = &v348; /*0x9022a4*/
  LODWORD(v330[0]) = &pvarg; /*0x9022a8*/
  LOBYTE(v355) = 102; /*0x9022af*/
  v148 = get_basic_font(a1: &v353, fType: 1); /*0x9022b3*/
  LOBYTE(v355) = 103; /*0x9022bc*/
  HIDWORD(v329) = sub_4284A5(this: v148); /*0x9022c5*/
  v149 = sub_428226(a1: v2 + 1732); /*0x9022cc*/
  LODWORD(v329) = v150; /*0x9022d1*/
  String = &v329; /*0x9022d4*/
  _bstr_t::_bstr_t(a1: v149); /*0x9022d8*/
  v328 = 293; /*0x9022dd*/
  p_pvarg = SP_58_BRAWLER; /*0x9022e2*/
  LOBYTE(v355) = 103; /*0x9022e7*/
  v151 = sub_414576(a1: &v351); /*0x9022eb*/
  IWzCanvas::DrawTextA( /*0x9022f2*/
    this: v151,
    nLeft: p_pvarg,
    nTop: v328,
    sText: v329,
    pFont: SHIDWORD(v329),
    vAlpha: v330[0],
    vTabOrg: HIDWORD(v330[0]));
  LOBYTE(v355) = 102; /*0x9022fc*/
  if ( v353 != 0 ) /*0x902300*/
    (*(*v353 + 8))(a1: v353); /*0x902305*/
  LOBYTE(v355) = 101; /*0x90230c*/
  v152 = ZComAPI::ZComVariantCopy(&pvarg); /*0x902310*/
  if ( v152 < 0 ) /*0x902318*/
    _com_issue_error(a1: v152); /*0x90231b*/
  LOBYTE(v355) = 89; /*0x902324*/
  v153 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x902328*/
  if ( v153 < 0 ) /*0x902330*/
    _com_issue_error(a1: v153); /*0x902333*/
  HIDWORD(v330[0]) = 5515; /*0x90233b*/
  LODWORD(v330[0]) = &v353; /*0x902340*/
  v154 = StringPool::GetInstance(); /*0x902341*/
  v155 = StringPool::GetString(this: v154, result: v330[0], nIdx: SHIDWORD(v330[0])); /*0x902348*/
  HIDWORD(v330[0]) = *(v2 + 438); /*0x90234d*/
  LODWORD(v330[0]) = *v155; /*0x902355*/
  LOBYTE(v355) = 105; /*0x90235a*/
  ZXString<char>::Format(a1: &a2, a2: v330[0], a3: SBYTE4(v330[0])); /*0x90235e*/
  LOBYTE(v355) = 89; /*0x902369*/
  ZXString<char>::_Release(a1: &v353); /*0x90236d*/
  sub_402F85(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x902376*/
  LOBYTE(v355) = 106; /*0x90237f*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x902383*/
  HIDWORD(v330[0]) = &v348; /*0x90238b*/
  LODWORD(v330[0]) = &pvarg; /*0x90238f*/
  LOBYTE(v355) = 107; /*0x902396*/
  v156 = get_basic_font(a1: &v353, fType: 1); /*0x90239a*/
  LOBYTE(v355) = 108; /*0x9023a3*/
  HIDWORD(v329) = sub_4284A5(this: v156); /*0x9023ac*/
  v157 = sub_428226(a1: &a2); /*0x9023b0*/
  LODWORD(v329) = v158; /*0x9023b5*/
  String = &v329; /*0x9023b8*/
  _bstr_t::_bstr_t(a1: v157); /*0x9023bc*/
  v328 = 311; /*0x9023c1*/
  p_pvarg = SP_58_BRAWLER; /*0x9023c6*/
  LOBYTE(v355) = 108; /*0x9023cb*/
  v159 = sub_414576(a1: &v351); /*0x9023cf*/
  IWzCanvas::DrawTextA( /*0x9023d6*/
    this: v159,
    nLeft: p_pvarg,
    nTop: v328,
    sText: v329,
    pFont: SHIDWORD(v329),
    vAlpha: v330[0],
    vTabOrg: HIDWORD(v330[0]));
  LOBYTE(v355) = 107; /*0x9023e0*/
  if ( v353 != 0 ) /*0x9023e4*/
    (*(*v353 + 8))(a1: v353); /*0x9023e9*/
  LOBYTE(v355) = 106; /*0x9023f0*/
  v160 = ZComAPI::ZComVariantCopy(&pvarg); /*0x9023f4*/
  if ( v160 < 0 ) /*0x9023fc*/
    _com_issue_error(a1: v160); /*0x9023ff*/
  LOBYTE(v355) = 89; /*0x902408*/
  v161 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x90240c*/
  if ( v161 < 0 ) /*0x902414*/
    _com_issue_error(a1: v161); /*0x902417*/
  HIDWORD(v330[0]) = 5515; /*0x90241f*/
  LODWORD(v330[0]) = &v353; /*0x902424*/
  v162 = StringPool::GetInstance(); /*0x902425*/
  v163 = StringPool::GetString(this: v162, result: v330[0], nIdx: SHIDWORD(v330[0])); /*0x90242c*/
  HIDWORD(v330[0]) = *(v2 + 439); /*0x902431*/
  LODWORD(v330[0]) = *v163; /*0x902439*/
  LOBYTE(v355) = 110; /*0x90243e*/
  ZXString<char>::Format(a1: &a2, a2: v330[0], a3: SBYTE4(v330[0])); /*0x902442*/
  LOBYTE(v355) = 89; /*0x90244d*/
  ZXString<char>::_Release(a1: &v353); /*0x902451*/
  sub_402F85(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x90245a*/
  LOBYTE(v355) = 111; /*0x902463*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x902467*/
  HIDWORD(v330[0]) = &v348; /*0x90246f*/
  LODWORD(v330[0]) = &pvarg; /*0x902473*/
  LOBYTE(v355) = 112; /*0x90247a*/
  v164 = get_basic_font(a1: &v353, fType: 1); /*0x90247e*/
  LOBYTE(v355) = 113; /*0x902487*/
  HIDWORD(v329) = sub_4284A5(this: v164); /*0x902490*/
  v165 = sub_428226(a1: &a2); /*0x902494*/
  LODWORD(v329) = v166; /*0x902499*/
  String = &v329; /*0x90249c*/
  _bstr_t::_bstr_t(a1: v165); /*0x9024a0*/
  v328 = 293; /*0x9024a5*/
  p_pvarg = SP_189__GIVEPRIZE; /*0x9024aa*/
  LOBYTE(v355) = 113; /*0x9024b2*/
  v167 = sub_414576(a1: &v351); /*0x9024b6*/
  IWzCanvas::DrawTextA( /*0x9024bd*/
    this: v167,
    nLeft: p_pvarg,
    nTop: v328,
    sText: v329,
    pFont: SHIDWORD(v329),
    vAlpha: v330[0],
    vTabOrg: HIDWORD(v330[0]));
  LOBYTE(v355) = 112; /*0x9024c7*/
  if ( v353 != 0 ) /*0x9024cb*/
    (*(*v353 + 8))(a1: v353); /*0x9024d0*/
  LOBYTE(v355) = 111; /*0x9024d7*/
  v168 = ZComAPI::ZComVariantCopy(&pvarg); /*0x9024db*/
  if ( v168 < 0 ) /*0x9024e3*/
    _com_issue_error(a1: v168); /*0x9024e6*/
  LOBYTE(v355) = 89; /*0x9024ef*/
  v169 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x9024f3*/
  if ( v169 < 0 ) /*0x9024fb*/
    _com_issue_error(a1: v169); /*0x9024fe*/
  HIDWORD(v330[0]) = 5515; /*0x902506*/
  LODWORD(v330[0]) = &v353; /*0x90250b*/
  v170 = StringPool::GetInstance(); /*0x90250c*/
  v171 = StringPool::GetString(this: v170, result: v330[0], nIdx: SHIDWORD(v330[0])); /*0x902513*/
  HIDWORD(v330[0]) = *(v2 + 440); /*0x902518*/
  LODWORD(v330[0]) = *v171; /*0x902520*/
  LOBYTE(v355) = 115; /*0x902525*/
  ZXString<char>::Format(a1: &a2, a2: v330[0], a3: SBYTE4(v330[0])); /*0x902529*/
  LOBYTE(v355) = 89; /*0x902534*/
  ZXString<char>::_Release(a1: &v353); /*0x902538*/
  sub_402F85(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x902541*/
  LOBYTE(v355) = 116; /*0x90254a*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x90254e*/
  HIDWORD(v330[0]) = &v348; /*0x902556*/
  LODWORD(v330[0]) = &pvarg; /*0x90255a*/
  LOBYTE(v355) = 117; /*0x902561*/
  v172 = get_basic_font(a1: &v353, fType: 1); /*0x902565*/
  LOBYTE(v355) = 118; /*0x90256e*/
  HIDWORD(v329) = sub_4284A5(this: v172); /*0x902577*/
  v173 = sub_428226(a1: &a2); /*0x90257b*/
  LODWORD(v329) = v174; /*0x902580*/
  String = &v329; /*0x902583*/
  _bstr_t::_bstr_t(a1: v173); /*0x902587*/
  v328 = 311; /*0x90258c*/
  p_pvarg = SP_189__GIVEPRIZE; /*0x902591*/
  LOBYTE(v355) = 118; /*0x902599*/
  v175 = sub_414576(a1: &v351); /*0x90259d*/
  IWzCanvas::DrawTextA( /*0x9025a4*/
    this: v175,
    nLeft: p_pvarg,
    nTop: v328,
    sText: v329,
    pFont: SHIDWORD(v329),
    vAlpha: v330[0],
    vTabOrg: HIDWORD(v330[0]));
  LOBYTE(v355) = 117; /*0x9025ae*/
  if ( v353 != 0 ) /*0x9025b2*/
    (*(*v353 + 8))(a1: v353); /*0x9025b7*/
  LOBYTE(v355) = 116; /*0x9025be*/
  v176 = ZComAPI::ZComVariantCopy(&pvarg); /*0x9025c2*/
  if ( v176 < 0 ) /*0x9025ca*/
    _com_issue_error(a1: v176); /*0x9025cd*/
  LOBYTE(v355) = 89; /*0x9025d6*/
  v177 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x9025da*/
  if ( v177 < 0 ) /*0x9025e2*/
    _com_issue_error(a1: v177); /*0x9025e5*/
  v179 = *(v2 + 403); /*0x9025ea*/
  if ( v179 != 0 && *(v179 - 4) != 0 ) /*0x9025f7*/
  {
    v180 = *(*(v2 + 393) + 56); /*0x902647*/
    v352 = v180; /*0x90264a*/
    while ( 1 ) /*0x90264d*/
    {
      v181 = *(v2 + 403); /*0x90264d*/
      if ( v181 == 0 || v180 >= *(v181 - 4) || v180 >= *(*(v2 + 393) + 56) + 3 ) /*0x902672*/
        break; /*0x902672*/
      v330[0] = 0; /*0x902679*/
      v329 = 0; /*0x90267b*/
      v328 = 0; /*0x90267c*/
      p_pvarg = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x90267d*/
      v182 = sub_64BCCC(a1: v2 + 1568, a2: v178); /*0x902684*/
      v183 = sub_64B1A1(a1: v182); /*0x90268b*/
      v326 = 38 * (v352 - v183) + 212; /*0x90269e*/
      v325 = 106; /*0x90269f*/
      v324 = *sub_5ADCAA(a1: v352); /*0x9026af*/
      v323 = v184; /*0x9026b4*/
      v345 = &v323; /*0x9026b7*/
      sub_4281DD(a1: &v351); /*0x9026bb*/
      LOBYTE(v355) = 89; /*0x9026c0*/
      sub_4283F8(); /*0x9026c4*/
      sub_5D6458( /*0x9026cb*/
        a1: v323,
        a2: v324,
        a3: v325,
        a4: v326,
        a5: p_pvarg,
        a6: v328,
        a7: v329,
        a8: HIDWORD(v329),
        a9: v330[0],
        a10: HIDWORD(v330[0]));
      v185 = CItemInfo::GetItemName(a1: &v353, a2: *(*(v2 + 403) + 4 * v352)); /*0x9026e6*/
      LOBYTE(v355) = 121; /*0x9026ef*/
      ZXString<char>::operator=(a1: v185); /*0x9026f3*/
      LOBYTE(v355) = 89; /*0x9026fb*/
      ZXString<char>::_Release(a1: &v353); /*0x9026ff*/
      v330[0] = v186 | 0x6400000000LL; /*0x902706*/
      v345 = v330; /*0x902709*/
      get_basic_font(a1: v330, fType: 1); /*0x90270f*/
      format_string(sFormat: &a2, pFont: v330[0], nWidth: HIDWORD(v330[0])); /*0x90271a*/
      VariantInit(pvarg: &v338); /*0x902726*/
      v187 = ZtlTaskMemFreeImp(pvargDest: &v338, pvargSrc: &pvargSrc); /*0x902731*/
      if ( v187 < 0 ) /*0x90273a*/
        _com_issue_error(a1: v187); /*0x90273d*/
      LOBYTE(v355) = 122; /*0x902749*/
      VariantInit(pvarg: &v333); /*0x90274d*/
      v188 = ZtlTaskMemFreeImp(pvargDest: &v333, pvargSrc: &pvargSrc); /*0x90275b*/
      if ( v188 < 0 ) /*0x902764*/
        _com_issue_error(a1: v188); /*0x902767*/
      HIDWORD(v330[0]) = &v338; /*0x90276f*/
      LODWORD(v330[0]) = &v333; /*0x902776*/
      LOBYTE(v355) = 123; /*0x90277d*/
      v189 = get_basic_font(a1: &String, fType: 1); /*0x902781*/
      LOBYTE(v355) = 124; /*0x90278a*/
      HIDWORD(v329) = sub_4284A5(this: v189); /*0x902793*/
      v190 = sub_428226(a1: &a2); /*0x902797*/
      LODWORD(v329) = v191; /*0x90279c*/
      v345 = &v329; /*0x90279f*/
      _bstr_t::_bstr_t(a1: v190); /*0x9027a3*/
      LOBYTE(v355) = 124; /*0x9027ae*/
      v193 = sub_64BCCC(a1: v2 + 1568, a2: v192); /*0x9027b2*/
      v194 = sub_64B1A1(a1: v193); /*0x9027b9*/
      v328 = 38 * (v352 - v194) + 179; /*0x9027cc*/
      p_pvarg = SP_147_PLEASE_TRY_AGAIN; /*0x9027cd*/
      v195 = sub_414576(a1: &v351); /*0x9027d5*/
      IWzCanvas::DrawTextA( /*0x9027dc*/
        this: v195,
        nLeft: p_pvarg,
        nTop: v328,
        sText: v329,
        pFont: SHIDWORD(v329),
        vAlpha: v330[0],
        vTabOrg: HIDWORD(v330[0]));
      LOBYTE(v355) = 123; /*0x9027e6*/
      if ( String != 0 ) /*0x9027ea*/
        (*(*String + 8))(a1: String); /*0x9027ef*/
      LOBYTE(v355) = 122; /*0x9027f9*/
      v196 = ZComAPI::ZComVariantCopy(pvarg: &v333); /*0x9027fd*/
      if ( v196 < 0 ) /*0x902805*/
        _com_issue_error(a1: v196); /*0x902808*/
      LOBYTE(v355) = 89; /*0x902811*/
      v197 = ZComAPI::ZComVariantCopy(pvarg: &v338); /*0x902815*/
      if ( v197 < 0 ) /*0x90281d*/
        _com_issue_error(a1: v197); /*0x902820*/
      VariantInit(pvarg: &pvargDest); /*0x902829*/
      v198 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc); /*0x902834*/
      if ( v198 < 0 ) /*0x90283d*/
        _com_issue_error(a1: v198); /*0x902840*/
      v345 = *(*(v2 + 393) + 56); /*0x90284e*/
      LOBYTE(v355) = 126; /*0x902856*/
      if ( v351 == 0 ) /*0x90285a*/
        _com_issue_error(a1: -2147467261); /*0x902861*/
      v199 = *v351; /*0x90286f*/
      v328 = v350; /*0x902879*/
      v329 = *&pvargDest.vt; /*0x90287f*/
      p_pvarg = 38 * (v352 - v345) + 200; /*0x902887*/
      v330[0] = pvargDest.cyVal.int64; /*0x902888*/
      v326 = 147; /*0x902889*/
      v325 = v351; /*0x90288e*/
      v340 = v351; /*0x90288f*/
      v200 = (*(v199 + 128))( /*0x902893*/
               a1: v351,
               a2: 147,
               a3: p_pvarg,
               a4: v350,
               a5: *&pvargDest.vt,
               a6: pvargDest.decVal.Hi32,
               a7: pvargDest.lVal,
               a8: pvargDest.cyVal.Hi);
      if ( v200 < 0 ) /*0x90289b*/
        _com_issue_errorex(a1: v200, a2: v340, a3: _GUID_7600dc6c_9328_4bff_9624_5b0f5c01179e); /*0x9028a6*/
      LOBYTE(v355) = 89; /*0x9028af*/
      v201 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x9028b3*/
      if ( v201 < 0 ) /*0x9028bb*/
        _com_issue_error(a1: v201); /*0x9028be*/
      CItemInfo::GetItemInfo(a1: &v343, a2: *(*(v346 + 403) + 4 * v352)); /*0x9028dd*/
      LOBYTE(v355) = 127; /*0x9028e6*/
      if ( v343 != 0 ) /*0x9028ea*/
      {
        v330[0] = v202; /*0x9028f7*/
        v345 = v330; /*0x9028fa*/
        HIDWORD(v329) = 1887; /*0x9028fd*/
        LODWORD(v329) = v330; /*0x902902*/
        v203 = StringPool::GetInstance(); /*0x902903*/
        StringPool::GetBSTR(this: v203, a2: v329, a3: SHIDWORD(v329)); /*0x90290a*/
        HIDWORD(v329) = &v331; /*0x902915*/
        LOBYTE(v355) = 127; /*0x902919*/
        sub_40263B(a1: &v343); /*0x90291d*/
        v204 = IWzProperty::GetItem(pvargDest: HIDWORD(v329), a2: v330[0]); /*0x902929*/
        LODWORD(v330[0]) = 5515; /*0x902931*/
        HIDWORD(v329) = &v335; /*0x902936*/
        LOBYTE(v355) = -127; /*0x902937*/
        v205 = StringPool::GetInstance(); /*0x90293b*/
        v206 = StringPool::GetString(this: v205, result: HIDWORD(v329), nIdx: v330[0]); /*0x902947*/
        LOBYTE(v355) = -126; /*0x90294a*/
        HIDWORD(v330[0]) = get_int32(pvarSrc: v204, a2: SHIDWORD(v330[0])); /*0x902955*/
        v207 = sub_428226(a1: v206); /*0x902958*/
        ZXString<char>::Format(a1: &a2, a2: v207, a3: SBYTE4(v330[0])); /*0x902962*/
        LOBYTE(v355) = -127; /*0x902970*/
        ZXString<char>::_Release(a1: &v335); /*0x902974*/
        LOBYTE(v355) = 127; /*0x902980*/
        v208 = ZComAPI::ZComVariantCopy(pvarg: &v331); /*0x902984*/
        if ( v208 < 0 ) /*0x90298c*/
          _com_issue_error(a1: v208); /*0x90298f*/
        VariantInit(&pvarg); /*0x90299e*/
        v209 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x9029a5*/
        if ( v209 < 0 ) /*0x9029ae*/
          _com_issue_error(a1: v209); /*0x9029b1*/
        LOBYTE(v355) = -125; /*0x9029ba*/
        VariantInit(pvarg: &v348); /*0x9029be*/
        v210 = ZtlTaskMemFreeImp(pvargDest: &v348, pvargSrc: &pvargSrc); /*0x9029c5*/
        if ( v210 < 0 ) /*0x9029ce*/
          _com_issue_error(a1: v210); /*0x9029d1*/
        HIDWORD(v330[0]) = &pvarg; /*0x9029d9*/
        LODWORD(v330[0]) = &v348; /*0x9029dd*/
        LOBYTE(v355) = -124; /*0x9029e4*/
        v211 = get_basic_font(a1: &Unknown, fType: 1); /*0x9029e8*/
        LOBYTE(v355) = -123; /*0x9029f1*/
        HIDWORD(v329) = sub_4284A5(this: v211); /*0x9029fa*/
        v212 = sub_428226(a1: &a2); /*0x9029fe*/
        LODWORD(v329) = v213; /*0x902a03*/
        v345 = &v329; /*0x902a06*/
        _bstr_t::_bstr_t(a1: v212); /*0x902a0a*/
        LOBYTE(v355) = -123; /*0x902a18*/
        v215 = sub_64BCCC(a1: v346 + 1568, a2: v214); /*0x902a1c*/
        v216 = sub_64B1A1(a1: v215); /*0x902a23*/
        v328 = 38 * (v352 - v216) + 197; /*0x902a36*/
        p_pvarg = SP_183__RESET; /*0x902a37*/
        v217 = sub_414576(a1: &v351); /*0x902a3f*/
        IWzCanvas::DrawTextA( /*0x902a46*/
          this: v217,
          nLeft: p_pvarg,
          nTop: v328,
          sText: v329,
          pFont: SHIDWORD(v329),
          vAlpha: v330[0],
          vTabOrg: HIDWORD(v330[0]));
        LOBYTE(v355) = -124; /*0x902a50*/
        if ( Unknown != 0 ) /*0x902a54*/
          (*(*Unknown + 8))(a1: Unknown); /*0x902a59*/
        LOBYTE(v355) = -125; /*0x902a60*/
        v218 = ZComAPI::ZComVariantCopy(pvarg: &v348); /*0x902a64*/
        if ( v218 < 0 ) /*0x902a6c*/
          _com_issue_error(a1: v218); /*0x902a6f*/
        LOBYTE(v355) = 127; /*0x902a78*/
        v219 = ZComAPI::ZComVariantCopy(&pvarg); /*0x902a7c*/
        if ( v219 < 0 ) /*0x902a84*/
          _com_issue_error(a1: v219); /*0x902a87*/
        LOBYTE(v355) = 89; /*0x902a91*/
        if ( v343 != 0 ) /*0x902a95*/
          (*(*v343 + 8))(a1: v343); /*0x902a9a*/
      }
      else
      {
        LOBYTE(v355) = 89; /*0x9028ec*/
      }
      v180 = ++v352; /*0x902aa0*/
      v2 = v346; /*0x902aa3*/
    }
    LOBYTE(v355) = 83; /*0x902ab2*/
    if ( v350 != 0 ) /*0x902ab6*/
      (*(*v350 + 8))(a1: v350); /*0x902abb*/
    LOBYTE(v355) = 82; /*0x902ac3*/
    if ( v351 != 0 ) /*0x902ac7*/
      (*(*v351 + 8))(a1: v351); /*0x902acc*/
    LOBYTE(v355) = 1; /*0x902acf*/
    goto LABEL_95; /*0x902ad6*/
  }
  LOBYTE(v355) = 83; /*0x9025fe*/
  if ( v350 != 0 ) /*0x902602*/
    (*(*v350 + 8))(a1: v350); /*0x902607*/
  LOBYTE(v355) = 82; /*0x90260f*/
  if ( v351 != 0 ) /*0x902613*/
    (*(*v351 + 8))(a1: v351); /*0x902618*/
  LOBYTE(v355) = 1; /*0x90261e*/
  ZXString<char>::_Release(a1: &a2); /*0x902622*/
  LOBYTE(v355) = 0; /*0x90262a*/
  if ( v342 != 0 ) /*0x902630*/
    (*(*v342 + 8))(a1: v342); /*0x902639*/
LABEL_298:
  v355 = -1; /*0x90372a*/
  ZXString<char>::_Release(a1: &v344); /*0x903731*/
}
`

## CUIUserInfo::SetPetInfo

`c
void __thiscall CUIUserInfo::SetPetInfo(
        CUIUserInfo *this,
        int a2,
        unsigned int a3,
        char a4,
        char a5,
        __int16 a6,
        char a7,
        __int16 a8,
        int a9)
{
  *(this + a2 + 418) = CPetTemplate::GetPetTemplate(a1: a3); /*0x903c6e*/
  ZXString<char>::operator=(a1: &a4); /*0x903c80*/
  *(this + a2 + 1696) = a5; /*0x903c88*/
  *(this + a2 + 850) = a6; /*0x903c93*/
  *(this + a2 + 1706) = a7; /*0x903c9e*/
  *(this + a2 + 862) = a8; /*0x903ca9*/
  *(this + a2 + 428) = a9; /*0x903cb7*/
  CUIUserInfo::CreatePetAvatar(this, a2); /*0x903cbe*/
  CUIUserInfo::SetPetItemList(this, a2); /*0x903cc6*/
  ZXString<char>::_Release(a1: &a4); /*0x903cd2*/
}
`

## CUIUserInfo::SetMultiPetInfo

`c
void __stdcall CUIUserInfo::SetMultiPetInfo(int a1, struct CInPacket *a2)
{
  int v2; // ebx
  CInPacket *v3; // esi
  unsigned int v4; // edi
  int v5; // eax
  struct CInPacket *v6; // eax
  int v7; // ecx
  CUIUserInfo *v8; // eax
  unsigned __int8 v9; // al
  int v10; // edi
  int v11; // [esp-18h] [ebp-3Ch] BYREF
  int v12; // [esp-14h] [ebp-38h]
  int v13; // [esp-10h] [ebp-34h]
  int v14; // [esp-Ch] [ebp-30h]
  struct CInPacket *v15; // [esp-8h] [ebp-2Ch]
  int v16; // [esp-4h] [ebp-28h]
  int v17; // [esp+Ch] [ebp-18h]
  int v18; // [esp+10h] [ebp-14h]
  int v19; // [esp+14h] [ebp-10h]
  int v20; // [esp+20h] [ebp-4h]

  v2 = 0; /*0x903cf5*/
  if ( a1 != 0 ) /*0x903cfc*/
  {
    v3 = a2; /*0x903d02*/
    do /*0x903d93*/
    {
      v4 = CInPacket::Decode4(this: v3); /*0x903d0c*/
      CInPacket::DecodeStr(&a1); /*0x903d14*/
      v20 = 0; /*0x903d19*/
      LOBYTE(v17) = CInPacket::Decode1(this: v3); /*0x903d26*/
      LOWORD(v5) = CInPacket::Decode2(this: v3); /*0x903d29*/
      v18 = v5; /*0x903d30*/
      LOBYTE(v19) = CInPacket::Decode1(this: v3); /*0x903d3a*/
      LOWORD(v6) = CInPacket::Decode2(this: v3); /*0x903d3d*/
      a2 = v6; /*0x903d44*/
      v16 = CInPacket::Decode4(this: v3); /*0x903d4c*/
      v15 = a2; /*0x903d4d*/
      v14 = v19; /*0x903d53*/
      v13 = v18; /*0x903d56*/
      v12 = v17; /*0x903d59*/
      v11 = v7; /*0x903d5c*/
      a2 = &v11; /*0x903d5f*/
      sub_428211(&a1); /*0x903d63*/
      LOBYTE(v20) = 0; /*0x903d68*/
      v8 = sub_904DDC(); /*0x903d6e*/
      CUIUserInfo::SetPetInfo(this: v8, a2: v2++, a3: v4, a4: v11, a5: v12, a6: v13, a7: v14, a8: v15, a9: v16); /*0x903d75*/
      v9 = CInPacket::Decode1(this: v3); /*0x903d7d*/
      v20 = -1; /*0x903d82*/
      v10 = v9; /*0x903d89*/
      ZXString<char>::_Release(&a1); /*0x903d8c*/
    }
    while ( v10 != 0 ); /*0x903d93*/
    CUIUserInfo::SetState(this: dword_BEDCCC, a2: 1); /*0x903da1*/
  }
}
`

## CUIUserInfo::SetAvatarInfo

`c
int __thiscall CUIUserInfo::SetAvatarInfo(CUIUserInfo *this, const struct AvatarLook *a2)
{
  char *v3; // ebx
  char Data; // di
  char v5; // di
  int v6; // ecx
  char v7; // di
  int *v8; // ecx
  int Layer; // eax
  int *v10; // ecx
  _DWORD *v11; // eax
  CItemInfo::EQUIPITEM *EquipItem; // eax
  _DWORD *v13; // eax
  int v14; // ecx
  int v15; // edx
  int v16; // eax
  int v17; // eax
  int *v18; // ecx
  _DWORD *v19; // eax
  int v20; // ecx
  int v22; // [esp-20h] [ebp-6Ch]
  int v23; // [esp-20h] [ebp-6Ch]
  int *v24; // [esp-1Ch] [ebp-68h] BYREF
  int *v25; // [esp-18h] [ebp-64h] BYREF
  unsigned int v26; // [esp-14h] [ebp-60h]
  signed int v27; // [esp-10h] [ebp-5Ch]
  int v28; // [esp-Ch] [ebp-58h]
  int v29; // [esp-8h] [ebp-54h]
  int v30; // [esp-4h] [ebp-50h]
  int v31; // [esp+0h] [ebp-4Ch]
  _DWORD v32[2]; // [esp+Ch] [ebp-40h] BYREF
  int v33; // [esp+14h] [ebp-38h]
  _DWORD v34[2]; // [esp+18h] [ebp-34h] BYREF
  int v35; // [esp+20h] [ebp-2Ch]
  _DWORD v36[2]; // [esp+24h] [ebp-28h] BYREF
  int v37; // [esp+2Ch] [ebp-20h]
  int **v38; // [esp+30h] [ebp-1Ch]
  int **v39; // [esp+34h] [ebp-18h] BYREF
  int v40; // [esp+38h] [ebp-14h]
  int i; // [esp+3Ch] [ebp-10h] BYREF
  int v42; // [esp+48h] [ebp-4h]

  v3 = this + 1796; /*0x903e35*/
  v40 = 1; /*0x903e3e*/
  ZArray<TSecType<long>>::RemoveAll(a1: this + 1796); /*0x903e45*/
  for ( i = 0; i < 52; ++i ) /*0x903e4a*/
  {
    if ( i != 0 && i != 14 ) /*0x903e5c*/
    {
      switch ( i ) /*0x903e66*/
      {
        case 18: /*0x903e66*/
          *(this + 435) = *(a2 + 97); /*0x903e6e*/
          break;
        case 19: /*0x903e66*/
          *(this + 436) = *(a2 + 101); /*0x903e85*/
          break;
        case 20: /*0x903e66*/
          *(this + 437) = *(a2 + 105); /*0x903e9c*/
          break;
        default:
          if ( i == 11 && *(a2 + 21) / 100000 == 17 ) /*0x903ec0*/
          {
            TSecType<long>::TSecType<long>(this: v36, a2: *(a2 + 21)); /*0x903ec6*/
            v42 = 0; /*0x903ecb*/
            Data = TSecType<long>::GetData(a1: v36, a2: v3); /*0x903edb*/
            ZArray<TSecType<long>>::Insert(a1: -1); /*0x903edd*/
            TSecType<long>::SetData(a1: Data); /*0x903ee5*/
            v42 = -1; /*0x903eea*/
            if ( v37 != 0 ) /*0x903ef2*/
              ZAllocEx<ZAllocAnonSelector>::Free(a1: v37); /*0x903efc*/
          }
          v39 = (a2 + 4 * i); /*0x903f0a*/
          if ( *(v39 + 233) > 0 ) /*0x903f15*/
          {
            TSecType<long>::TSecType<long>(this: v34, a2: *(v39 + 233)); /*0x903f1d*/
            v42 = 1; /*0x903f25*/
            v5 = TSecType<long>::GetData(a1: v34, a2: v3); /*0x903f35*/
            ZArray<TSecType<long>>::Insert(a1: -1); /*0x903f37*/
            TSecType<long>::SetData(a1: v5); /*0x903f3f*/
            v42 = -1; /*0x903f44*/
            if ( v35 != 0 ) /*0x903f4c*/
              ZAllocEx<ZAllocAnonSelector>::Free(a1: v35); /*0x903f56*/
          }
          v6 = *(v39 + 25); /*0x903f5e*/
          if ( v6 > 0 && v6 / 10000 != 181 ) /*0x903f76*/
          {
            TSecType<long>::TSecType<long>(this: v32, a2: *(v39 + 25)); /*0x903f7c*/
            v42 = 2; /*0x903f84*/
            v7 = TSecType<long>::GetData(a1: v32, a2: v3); /*0x903f94*/
            ZArray<TSecType<long>>::Insert(a1: -1); /*0x903f96*/
            TSecType<long>::SetData(a1: v7); /*0x903f9e*/
            v42 = -1; /*0x903fa3*/
            if ( v33 != 0 ) /*0x903fab*/
              ZAllocEx<ZAllocAnonSelector>::Free(a1: v33); /*0x903fb5*/
          }
          break;
      }
    }
  }
  ZRef<CAvatar>::_Alloc(a1: this + 1616); /*0x903fcf*/
  v30 = 0; /*0x903fd6*/
  v29 = 100; /*0x903fd7*/
  v28 = 130; /*0x903fd9*/
  v27 = 55; /*0x903fde*/
  v26 = 1; /*0x903fe0*/
  v25 = v8; /*0x903fe2*/
  v38 = &v25; /*0x903fe5*/
  CWnd::GetLayer(a1: &v25); /*0x903feb*/
  v42 = 3; /*0x903ff6*/
  Layer = CWnd::GetLayer(a1: &v39); /*0x903ffd*/
  v24 = v10; /*0x904002*/
  i = &v24; /*0x904005*/
  LOBYTE(v42) = 4; /*0x904009*/
  sub_428EC4(a1: Layer); /*0x90400d*/
  LOBYTE(v42) = 6; /*0x904016*/
  v22 = sub_426626(a1: 2, a2: 1); /*0x904021*/
  v11 = sub_4285F2(a1: this + 1616); /*0x904027*/
  CAvatar::Init( /*0x90402e*/
    this: v11,
    a2,
    nMoveAction: v22,
    pOrigin: v24,
    pOverlay: v25,
    z: v26,
    x: v27,
    y: v28,
    nScale: v29,
    nDefaultEmotion: v30);
  v42 = -1; /*0x904036*/
  if ( v39 != 0 ) /*0x90403c*/
    ((*v39)[2])(a1: v39); /*0x904041*/
  if ( *(this + 435) != 0 && *(this + 436) != 0 ) /*0x904058*/
  {
    EquipItem = CItemInfo::GetEquipItem(this: dword_BE78D8, a2: *(this + 436)); /*0x904065*/
    if ( EquipItem == 0 || !CItemInfo::EQUIPITEM::IsItemSuitedForTamingMob(this: EquipItem, a2: *(this + 435)) ) /*0x904081*/
    {
      v40 = 0; /*0x9041a2*/
      goto LABEL_45; /*0x9041a2*/
    }
    ZRef<CAvatar>::_Alloc(a1: this + 1624); /*0x90408f*/
    if ( *(this + 435) == 1902002 ) /*0x9040a1*/
    {
      v15 = 70; /*0x9040fe*/
      v16 = -25; /*0x904101*/
      v14 = -25; /*0x904102*/
    }
    else
    {
      switch ( *(this + 435) ) /*0x9040a6*/
      {
        case 1902015: /*0x9040a6*/
          v15 = 80; /*0x9040f5*/
          v14 = 0; /*0x9040f6*/
          v30 = -12; /*0x9040f8*/
          break;
        case 1902016: /*0x9040a6*/
        case 1902017: /*0x9040a6*/
          v15 = 80; /*0x9040eb*/
          v14 = -5; /*0x9040ee*/
          v30 = -15; /*0x9040ef*/
          break;
        case 1902018: /*0x9040a6*/
          v15 = 75; /*0x9040e0*/
          v14 = -5; /*0x9040e3*/
          v30 = -20; /*0x9040e4*/
          break;
        default:
          v13 = CItemInfo::GetEquipItem(this: dword_BE78D8, a2: *(this + 435)); /*0x9040b8*/
          if ( v13 != 0 ) /*0x9040bf*/
          {
            v15 = v13[117]; /*0x9040ca*/
            v14 = v13[118]; /*0x9040d0*/
            v16 = v13[119]; /*0x9040d6*/
          }
          else
          {
            v14 = 0; /*0x9040c3*/
            v15 = 100; /*0x9040c5*/
            v16 = 0; /*0x9040c6*/
          }
          goto LABEL_41; /*0x9040c8*/
      }
      v16 = v30; /*0x9040e6*/
    }
LABEL_41:
    v30 = 0; /*0x904104*/
    v29 = v15; /*0x904105*/
    v28 = v16 + 261; /*0x90410b*/
    v27 = v14 + 53; /*0x90410f*/
    v26 = 1; /*0x904110*/
    v25 = (v14 + 53); /*0x904112*/
    v38 = &v25; /*0x904115*/
    CWnd::GetLayer(a1: &v25); /*0x90411b*/
    v42 = 7; /*0x904126*/
    v17 = CWnd::GetLayer(a1: &i); /*0x90412d*/
    v24 = v18; /*0x904132*/
    v39 = &v24; /*0x904135*/
    LOBYTE(v42) = 8; /*0x904139*/
    sub_428EC4(a1: v17); /*0x90413d*/
    LOBYTE(v42) = 10; /*0x904146*/
    v23 = sub_426626(a1: 2, a2: 1); /*0x904151*/
    v19 = sub_4285F2(a1: this + 1624); /*0x904157*/
    CAvatar::Init( /*0x90415e*/
      this: v19,
      a2,
      nMoveAction: v23,
      pOrigin: v24,
      pOverlay: v25,
      z: v26,
      x: v27,
      y: v28,
      nScale: v29,
      nDefaultEmotion: v30);
    v42 = -1; /*0x904166*/
    if ( i != 0 ) /*0x90416c*/
      (*(*i + 8))(a1: i); /*0x904171*/
    CAvatar::SetRidingVehicle(this: *(this + 407), a2: *(this + 435), a3: v31); /*0x904186*/
    CAvatar::SetVisibleMan(this: *(this + 407), a2: 0); /*0x90418e*/
    CUIUserInfo::SetState(this, a2: *(this + 408)); /*0x90419b*/
  }
LABEL_45:
  v20 = *(this + 395); /*0x9041a5*/
  if ( v20 != 0 ) /*0x9041ad*/
    sub_8FDAEF(a1: v20); /*0x9041af*/
  return v40; /*0x9041b4*/
}
`

## CUIUserInfo::SetWishItemInfo

`c
int __thiscall CUIUserInfo::SetWishItemInfo(char *this, int *a2)
{
  char *v2; // edi
  int result; // eax
  int i; // ebx
  int v5; // edi
  CCtrlScrollBar **v6; // edi
  _BYTE v7[4]; // [esp+Ch] [ebp-18h] BYREF
  int v8; // [esp+10h] [ebp-14h]
  char *v9; // [esp+14h] [ebp-10h]
  int v10; // [esp+20h] [ebp-4h]

  v2 = this; /*0x9041d8*/
  v9 = this; /*0x9041e0*/
  ZArray<TSecType<long>>::RemoveAll(a1: this + 1800); /*0x9041e3*/
  result = *a2; /*0x9041eb*/
  if ( *a2 != 0 ) /*0x9041ef*/
    result = *(result - 4); /*0x9041f1*/
  for ( i = result - 1; i >= 0; --i ) /*0x9041f9*/
  {
    result = CWvsContext::GetCommodityBySN(a1: v7, a2: *(*a2 + 4 * i)); /*0x90420a*/
    v10 = 0; /*0x90420f*/
    if ( v8 != 0 ) /*0x904217*/
    {
      v5 = *(*a2 + 4 * i); /*0x90421b*/
      ZArray<TSecType<long>>::Insert(a1: -1); /*0x904229*/
      result = TSecType<long>::SetData(a1: v5); /*0x904231*/
      v2 = v9; /*0x904236*/
    }
    v10 = -1; /*0x904239*/
    if ( v8 != 0 ) /*0x904241*/
    {
      result = sub_473F9E(a1: 0); /*0x904248*/
      v8 = 0; /*0x90424d*/
    }
  }
  v6 = *(v2 + 397); /*0x904254*/
  if ( v6 != 0 ) /*0x90425c*/
    return sub_8FE3FB(this: v6); /*0x904260*/
  return result; /*0x904265*/
}
`

## CUIUserInfo::CreatePetAvatar

`c
int __thiscall CUIUserInfo::CreatePetAvatar(CUIUserInfo *this, int arg0)
{
  int v3; // eax
  int v4; // eax
  int *Layer; // eax
  bool v6; // zf
  int v7; // ecx
  int v8; // eax
  int v9; // eax
  int v10; // ecx
  int v11; // eax
  int v12; // eax
  int v13; // eax
  int *v14; // esi
  int *v15; // edi
  int v16; // eax
  int *v17; // eax
  int v18; // ecx
  int v19; // eax
  int v20; // eax
  int *v21; // edi
  int *v22; // esi
  int v23; // eax
  int v24; // esi
  int v25; // eax
  int **v26; // ecx
  int *v27; // esi
  int v28; // edx
  int v29; // eax
  int v30; // eax
  int v31; // eax
  int v32; // eax
  int v33; // eax
  int v34; // eax
  int v35; // eax
  int v36; // eax
  unsigned int v37; // eax
  int v38; // ecx
  int v39; // eax
  int v40; // eax
  int v41; // eax
  CUIUserInfo *v42; // esi
  char *v43; // edi
  int v44; // eax
  int v45; // eax
  int v46; // eax
  IWzGr2DLayer *v47; // eax
  int v49; // [esp-8h] [ebp-A4h]
  int v50; // [esp-4h] [ebp-A0h]
  unsigned int v51; // [esp-4h] [ebp-A0h]
  VARIANTARG v52; // [esp+Ch] [ebp-90h] BYREF
  VARIANTARG v53; // [esp+1Ch] [ebp-80h] BYREF
  _DWORD v54[3]; // [esp+2Ch] [ebp-70h] BYREF
  int **v55; // [esp+38h] [ebp-64h]
  int v56; // [esp+3Ch] [ebp-60h]
  VARIANTARG pvarg; // [esp+40h] [ebp-5Ch] BYREF
  VARIANTARG v58; // [esp+50h] [ebp-4Ch] BYREF
  VARIANTARG v59; // [esp+60h] [ebp-3Ch] BYREF
  VARIANTARG pvargDest; // [esp+70h] [ebp-2Ch] BYREF
  CUIUserInfo *v61; // [esp+80h] [ebp-1Ch]
  int **v62; // [esp+84h] [ebp-18h]
  int v63; // [esp+88h] [ebp-14h] BYREF
  int *a2; // [esp+8Ch] [ebp-10h] BYREF
  int v65; // [esp+98h] [ebp-4h]

  v61 = this; /*0x904351*/
  v54[2] = 0; /*0x904354*/
  v55 = 0; /*0x904357*/
  v56 = 0; /*0x90435a*/
  v54[0] = &off_AF06C8; /*0x90435d*/
  v49 = *(this + arg0 + 428); /*0x904371*/
  v65 = 0; /*0x904378*/
  CActionMan::LoadPetAction(a1: dword_BE78D4, a2: *(this + arg0 + 418), a3: 1, a4: v49, a5: v54); /*0x904384*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x904392*/
  LOBYTE(v65) = 1; /*0x90439d*/
  sub_402FAB(a1: 0, a2: 3); /*0x9043a1*/
  LOBYTE(v65) = 2; /*0x9043ac*/
  if ( dword_BF14EC == 0 ) /*0x9043b0*/
    _com_issue_error(a1: -2147467261); /*0x9043b7*/
  v50 = *IWzGr2D::CreateLayer( /*0x9043db*/
           this: dword_BF14EC,
           &a2,
           nLeft: 0,
           nTop: 0,
           uWidth: 0,
           uHeight: 0,
           z: 0,
           vCanvas: &pvarg,
           dwFilter: &pvargDest);
  v62 = (this + 4 * arg0 + 1784); /*0x9043e4*/
  sub_428712(a1: v50); /*0x9043e7*/
  sub_4156C5(a1: &a2); /*0x9043ef*/
  LOBYTE(v65) = 1; /*0x9043f8*/
  v3 = ZComAPI::ZComVariantCopy(&pvarg); /*0x9043fc*/
  if ( v3 < 0 ) /*0x904404*/
    _com_issue_error(a1: v3); /*0x904407*/
  LOBYTE(v65) = 0; /*0x90440c*/
  v4 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x904414*/
  if ( v4 < 0 ) /*0x90441c*/
    _com_issue_error(a1: v4); /*0x90441f*/
  Layer = CWnd::GetLayer(a1: &v63); /*0x90442a*/
  sub_410FDF(this: &pvargDest, a2: *Layer, a3: 1); /*0x904437*/
  v6 = *v62 == 0; /*0x90443f*/
  LOBYTE(v65) = 4; /*0x904441*/
  if ( v6 ) /*0x904445*/
    _com_issue_error(a1: -2147467261); /*0x90444c*/
  v7 = **v62; /*0x904453*/
  a2 = *v62; /*0x904461*/
  v8 = (*(v7 + 100))(a1: a2, a2: *&pvargDest.vt, a3: pvargDest.decVal.Hi32, a4: pvargDest.lVal, a5: pvargDest.cyVal.Hi); /*0x904465*/
  if ( v8 < 0 ) /*0x90446a*/
    _com_issue_errorex(a1: v8, a2, a3: dword_BD8348); /*0x904475*/
  LOBYTE(v65) = 3; /*0x90447e*/
  v9 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x904482*/
  if ( v9 < 0 ) /*0x90448a*/
    _com_issue_error(a1: v9); /*0x90448d*/
  LOBYTE(v65) = 0; /*0x904492*/
  sub_4156C5(a1: &v63); /*0x904499*/
  sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x9044a2*/
  LOBYTE(v65) = 5; /*0x9044ab*/
  sub_402F85(&pvargDest, pvargSrc: &pvargSrc); /*0x9044af*/
  v6 = *v62 == 0; /*0x9044b7*/
  LOBYTE(v65) = 6; /*0x9044ba*/
  if ( v6 ) /*0x9044be*/
    _com_issue_error(a1: -2147467261); /*0x9044c5*/
  v10 = **v62; /*0x9044d6*/
  v63 = *v62; /*0x9044ed*/
  v11 = (*(v10 + 144))( /*0x9044f1*/
          a1: v63,
          a2: 53,
          a3: 299,
          a4: *&pvargDest.vt,
          a5: pvargDest.decVal.Hi32,
          a6: pvargDest.lVal,
          a7: pvargDest.cyVal.Hi,
          a8: *&pvarg.vt,
          a9: pvarg.decVal.Hi32,
          a10: pvarg.lVal,
          a11: pvarg.cyVal.Hi);
  if ( v11 < 0 ) /*0x9044f9*/
    _com_issue_errorex(a1: v11, a2: v63, a3: dword_BD8348); /*0x904504*/
  LOBYTE(v65) = 5; /*0x90450d*/
  v12 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x904511*/
  if ( v12 < 0 ) /*0x904519*/
    _com_issue_error(a1: v12); /*0x90451c*/
  LOBYTE(v65) = 0; /*0x904521*/
  v13 = ZComAPI::ZComVariantCopy(&pvarg); /*0x904529*/
  if ( v13 < 0 ) /*0x904531*/
    _com_issue_error(a1: v13); /*0x904534*/
  if ( *(v61 + 408) == 1 && arg0 == *(v61 + 417) ) /*0x90454e*/
    a2 = -1; /*0x904550*/
  else
    a2 = 0xFFFFFF; /*0x904556*/
  v14 = v62; /*0x90455d*/
  if ( *v62 == 0 ) /*0x904563*/
    _com_issue_error(a1: -2147467261); /*0x90456a*/
  v15 = *v62; /*0x90456f*/
  v16 = (*(*v15 + 224))(a1: v15, a2); /*0x904577*/
  if ( v16 < 0 ) /*0x90457f*/
    _com_issue_errorex(a1: v16, a2: v15, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x904588*/
  v17 = CWnd::GetLayer(a1: &a2); /*0x904594*/
  sub_410FDF(this: &pvargDest, a2: *v17, a3: 1); /*0x9045a1*/
  v6 = *v14 == 0; /*0x9045a6*/
  LOBYTE(v65) = 8; /*0x9045a9*/
  if ( v6 ) /*0x9045ad*/
    _com_issue_error(a1: -2147467261); /*0x9045b4*/
  v18 = **v14; /*0x9045bb*/
  v63 = *v14; /*0x9045c9*/
  v19 = (*(v18 + 252))( /*0x9045cd*/
          a1: v63,
          a2: *&pvargDest.vt,
          a3: pvargDest.decVal.Hi32,
          a4: pvargDest.lVal,
          a5: pvargDest.cyVal.Hi);
  if ( v19 < 0 ) /*0x9045d5*/
    _com_issue_errorex(a1: v19, a2: v63, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x9045e0*/
  LOBYTE(v65) = 7; /*0x9045e9*/
  v20 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x9045ed*/
  if ( v20 < 0 ) /*0x9045f5*/
    _com_issue_error(a1: v20); /*0x9045f8*/
  LOBYTE(v65) = 0; /*0x9045fd*/
  sub_4156C5(a1: &a2); /*0x904604*/
  v21 = v62; /*0x904609*/
  if ( *v62 == 0 ) /*0x90460f*/
    _com_issue_error(a1: -2147467261); /*0x904616*/
  v22 = *v62; /*0x90461b*/
  v23 = (*(**v62 + 180))(a1: *v62, a2: 1); /*0x904622*/
  if ( v23 < 0 ) /*0x90462a*/
    _com_issue_errorex(a1: v23, a2: v22, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x904633*/
  if ( *v21 == 0 ) /*0x90463b*/
    _com_issue_error(a1: -2147467261); /*0x904642*/
  v24 = *v21; /*0x904647*/
  v25 = (*(**v21 + 216))(a1: *v21, a2: 1); /*0x90464e*/
  if ( v25 < 0 ) /*0x904656*/
    _com_issue_errorex(a1: v25, a2: v24, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x90465f*/
  v26 = v55; /*0x904664*/
  a2 = 0; /*0x904667*/
  if ( v55 != 0 )
  {
    while ( 1 )
    {
      v27 = v26[1]; /*0x904678*/
      v28 = *(v26 != 0 ? v26 - 3 : 4);
      v62 = v28 != 0 ? (v28 + 16) : 0;
      sub_402F85(pvargDest: &v58, pvargSrc: &pvargSrc); /*0x90469b*/
      LOBYTE(v65) = 9; /*0x9046a4*/
      sub_402F85(pvargDest: &v59, pvargSrc: &pvargSrc); /*0x9046a8*/
      LOBYTE(v65) = 10; /*0x9046b1*/
      sub_402F85(pvargDest: &v53, pvargSrc: &pvargSrc); /*0x9046b5*/
      LOBYTE(v65) = 11; /*0x9046be*/
      sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x9046c2*/
      v29 = v27[16]; /*0x9046c7*/
      LOBYTE(v65) = 12; /*0x9046d0*/
      sub_402FAB(a1: v29, a2: 3); /*0x9046d4*/
      v6 = *v21 == 0; /*0x9046d9*/
      v30 = v27[3]; /*0x9046dc*/
      LOBYTE(v65) = 13; /*0x9046df*/
      v63 = v30; /*0x9046e3*/
      if ( v6 ) /*0x9046e6*/
        _com_issue_error(a1: -2147467261); /*0x9046ed*/
      IWzGr2DLayer::Animate(pvargDest: &v52, a2: v63, a3: &pvargDest, a4: &pvarg, a5: &v53, a6: &v59, a7: &v58); /*0x904712*/
      v31 = ZComAPI::ZComVariantCopy(pvarg: &v52); /*0x90471e*/
      if ( v31 < 0 ) /*0x904726*/
        _com_issue_error(a1: v31); /*0x904729*/
      LOBYTE(v65) = 12; /*0x904732*/
      v32 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x904736*/
      if ( v32 < 0 ) /*0x90473e*/
        _com_issue_error(a1: v32); /*0x904741*/
      LOBYTE(v65) = 11; /*0x90474a*/
      v33 = ZComAPI::ZComVariantCopy(&pvarg); /*0x90474e*/
      if ( v33 < 0 ) /*0x904756*/
        _com_issue_error(a1: v33); /*0x904759*/
      LOBYTE(v65) = 10; /*0x904762*/
      v34 = ZComAPI::ZComVariantCopy(pvarg: &v53); /*0x904766*/
      if ( v34 < 0 ) /*0x90476e*/
        _com_issue_error(a1: v34); /*0x904771*/
      LOBYTE(v65) = 9; /*0x90477a*/
      v35 = ZComAPI::ZComVariantCopy(pvarg: &v59); /*0x90477e*/
      if ( v35 < 0 ) /*0x904786*/
        _com_issue_error(a1: v35); /*0x904789*/
      LOBYTE(v65) = 0; /*0x90478e*/
      v36 = ZComAPI::ZComVariantCopy(pvarg: &v58); /*0x904796*/
      if ( v36 < 0 ) /*0x90479e*/
        _com_issue_error(a1: v36); /*0x9047a1*/
      if ( v27[3] == 0 ) /*0x9047aa*/
        _com_issue_error(a1: -2147467261); /*0x9047b1*/
      v37 = sub_40B947(a1: v27[3]); /*0x9047b9*/
      if ( v37 > a2 ) /*0x9047c1*/
      {
        if ( v27[3] == 0 ) /*0x9047c7*/
          _com_issue_error(a1: -2147467261); /*0x9047ce*/
        a2 = sub_40B947(a1: v27[3]); /*0x9047db*/
      }
      if ( v62 == 0 ) /*0x9047e2*/
        break; /*0x9047e2*/
      v26 = v62; /*0x904675*/
    }
  }
  sub_402F85(pvargDest: &v59, pvargSrc: &pvargSrc); /*0x9047ec*/
  LOBYTE(v65) = 14; /*0x9047f5*/
  sub_402F85(pvargDest: &v58, pvargSrc: &pvargSrc); /*0x9047f9*/
  v6 = *v21 == 0; /*0x9047fe*/
  LOBYTE(v65) = 15; /*0x904801*/
  if ( v6 ) /*0x904805*/
    _com_issue_error(a1: -2147467261); /*0x90480c*/
  v38 = **v21; /*0x90481d*/
  v63 = *v21; /*0x90482f*/
  v39 = (*(v38 + 272))( /*0x904833*/
          a1: v63,
          a2: 32,
          a3: *&v58.vt,
          a4: v58.decVal.Hi32,
          a5: v58.lVal,
          a6: v58.cyVal.Hi,
          a7: *&v59.vt,
          a8: v59.decVal.Hi32,
          a9: v59.lVal,
          a10: v59.cyVal.Hi);
  if ( v39 < 0 ) /*0x90483b*/
    _com_issue_errorex(a1: v39, a2: v63, a3: _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf); /*0x904846*/
  LOBYTE(v65) = 14; /*0x90484f*/
  v40 = ZComAPI::ZComVariantCopy(pvarg: &v58); /*0x904853*/
  if ( v40 < 0 ) /*0x90485b*/
    _com_issue_error(a1: v40); /*0x90485e*/
  LOBYTE(v65) = 0; /*0x904863*/
  v41 = ZComAPI::ZComVariantCopy(pvarg: &v59); /*0x90486b*/
  if ( v41 < 0 ) /*0x904873*/
    _com_issue_error(a1: v41); /*0x904876*/
  if ( arg0 == 0 ) /*0x90487f*/
  {
    sub_402F85(pvargDest: &v59, pvargSrc: &pvargSrc); /*0x904889*/
    LOBYTE(v65) = 16; /*0x904892*/
    sub_426B85(pvargDest: &v58, pvargSrc: &pvargSrc); /*0x904896*/
    v42 = v61; /*0x90489b*/
    v43 = v61 + 1780; /*0x9048ab*/
    LOBYTE(v65) = 17; /*0x9048b5*/
    sub_4281FB(a1: v61 + 1780); /*0x9048b9*/
    sub_432C2D(a1: 44, a2: 209, a3: &v58, a4: &v59); /*0x9048c0*/
    LOBYTE(v65) = 16; /*0x9048c9*/
    v44 = ZComAPI::ZComVariantCopy(pvarg: &v58); /*0x9048cd*/
    if ( v44 < 0 ) /*0x9048d5*/
      _com_issue_error(a1: v44); /*0x9048d8*/
    LOBYTE(v65) = 0; /*0x9048dd*/
    v45 = ZComAPI::ZComVariantCopy(pvarg: &v59); /*0x9048e5*/
    if ( v45 < 0 ) /*0x9048ed*/
      _com_issue_error(a1: v45); /*0x9048f0*/
    if ( *(v42 + 408) != 1 || *(v42 + 417) != 0 ) /*0x904905*/
      v46 = 0xFFFFFF; /*0x90490c*/
    else
      v46 = -1; /*0x904907*/
    v51 = v46; /*0x904911*/
    v47 = sub_4281FB(a1: v43); /*0x904914*/
    IWzGr2DLayer::Putcolor(this: v47, a2: v51); /*0x90491b*/
  }
  v65 = -1; /*0x904920*/
  v54[0] = &off_AF06C8; /*0x904927*/
  return sub_416C62(a1: v54); /*0x904933*/
}
`

## CUIUserInfo::ResetInfo_Pet

`c
void __thiscall CUIUserInfo::ResetInfo_Pet(CUIUserInfo *this)
{
  int v1; // esi
  char *v3; // edi
  int v4; // edi
  CPet *v5; // ecx
  int ItemSlot; // esi
  unsigned int Data; // eax
  struct CPetTemplate *PetTemplate; // eax
  int v9; // eax
  int i; // [esp+8h] [ebp-8h]
  int v11; // [esp+Ch] [ebp-4h]

  v1 = TSingleton<CUserLocal>::ms_pInstance; /*0x90496e*/
  v11 = TSingleton<CUserLocal>::ms_pInstance; /*0x904982*/
  if ( *(TSingleton<CUserLocal>::ms_pInstance + 4520) == *(this + 409) )
  {
    v3 = this + 1784; /*0x90498c*/
    for ( i = 3; i != 0; --i ) /*0x904992*/
    {
      sub_581FAA(a1: 0); /*0x90499d*/
      v3 += 4; /*0x9049a2*/
    }
    v4 = 0; /*0x9049aa*/
    while ( 1 )
    {
      v5 = *(*(v1 + 7900) + 8 * v4 + 4); /*0x9049b7*/
      if ( v5 == 0 ) /*0x9049bd*/
        break; /*0x9049bd*/
      ItemSlot = CPet::GetItemSlot(this: v5); /*0x9049c8*/
      Data = TSecType<long>::GetData(a1: (ItemSlot + 12), a2: this); /*0x9049cd*/
      PetTemplate = CPetTemplate::GetPetTemplate(a1: Data); /*0x9049d3*/
      if ( PetTemplate == 0 ) /*0x9049db*/
        break; /*0x9049db*/
      *(this + v4 + 418) = PetTemplate; /*0x9049e1*/
      ZXString<char>::GetBuffer(a1: (ItemSlot + 40), a2: 0xFFFFFFFF); /*0x9049f5*/
      *(this + v4 + 1696) = _ZtlSecureFuse<unsigned char>(a1: ItemSlot + 53, a2: *(ItemSlot + 55)); /*0x904a06*/
      *(this + v4 + 850) = _ZtlSecureFuse<short>(a1: ItemSlot + 59, a2: *(ItemSlot + 63)); /*0x904a19*/
      *(this + v4 + 1706) = _ZtlSecureFuse<unsigned char>(a1: ItemSlot + 67, a2: *(ItemSlot + 69)); /*0x904a32*/
      if ( v4 != 0 )
      {
        if ( v4 == 1 )
          v9 = *(v11 + 285); /*0x904a4e*/
        else
          v9 = v4 == 2 ? *(v11 + 317) : 0;
      }
      else
      {
        v9 = *(v11 + 221); /*0x904a3e*/
      }
      *(this + v4 + 428) = v9; /*0x904a6b*/
      CUIUserInfo::CreatePetAvatar(this, arg0: v4++); /*0x904a72*/
      if ( v4 >= 3 ) /*0x904a7b*/
        break; /*0x904a7b*/
      v1 = v11; /*0x9049ae*/
    }
    CWnd::InvalidateRect(this, a2: 0); /*0x904a85*/
  }
}
`

## CUIUserInfo::SetPetItemList

`c
int __thiscall CUIUserInfo::SetPetItemList(CUIUserInfo *this, int a2)
{
  int result; // eax
  int v4; // edi
  int *v5; // esi
  int v6; // ecx
  int v7; // [esp-4h] [ebp-1E8h]
  _BYTE v8[25]; // [esp+Ch] [ebp-1D8h] BYREF
  char v9; // [esp+25h] [ebp-1BFh] BYREF
  int v10; // [esp+1D4h] [ebp-10h]
  int v11; // [esp+1E0h] [ebp-4h]

  ZArray<long>::RemoveAll(a1: this + 4 * a2 + 1600); /*0x904b0b*/
  if ( *(this + a2 + 428) != 0 ) /*0x904b1f*/
    *ZArray<long>::InsertBefore(a1: -1) = *(this + a2 + 428); /*0x904b2c*/
  result = AvatarLook::`vftable'(a1: v8); /*0x904b34*/
  v4 = *(this + 405); /*0x904b39*/
  v11 = 0; /*0x904b3f*/
  if ( v4 != 0 ) /*0x904b45*/
    result = AvatarLook::operator=(a1: v4 + 4); /*0x904b51*/
  v10 = 0; /*0x904b56*/
  v5 = &v9; /*0x904b5a*/
  do /*0x904be2*/
  {
    v6 = *v5; /*0x904b60*/
    if ( *v5 > 0 ) /*0x904b64*/
    {
      result = v6 / 10000; /*0x904b70*/
      if ( v6 / 10000 == 181 || v6 == 1822000 || v6 == 1832000 ) /*0x904b87*/
      {
        if ( v6 == 1812003 || v6 == 1812002 ) /*0x904b97*/
        {
LABEL_18:
          result = ZArray<long>::InsertBefore(a1: -1); /*0x904bcb*/
          *result = *v5; /*0x904bd6*/
          goto LABEL_19; /*0x904bd6*/
        }
        if ( v10 > 45 ) /*0x904b9f*/
        {
          if ( v10 == 46 ) /*0x904bfc*/
            goto LABEL_16; /*0x904bfc*/
          if ( v10 == 47 ) /*0x904bff*/
          {
LABEL_26:
            v7 = 1; /*0x904c09*/
            goto LABEL_25; /*0x904c0b*/
          }
          if ( v10 != 48 ) /*0x904c02*/
          {
LABEL_16:
            result = 0; /*0x904bc4*/
            goto LABEL_17; /*0x904bc4*/
          }
        }
        else if ( v10 < 39 ) /*0x904ba4*/
        {
          if ( v10 <= 30 || v10 > 37 ) /*0x904bc2*/
            goto LABEL_16; /*0x904bc2*/
          goto LABEL_26; /*0x904bc2*/
        }
        v7 = 2; /*0x904c04*/
LABEL_25:
        result = v7; /*0x904c06*/
LABEL_17:
        if ( result != a2 ) /*0x904bc9*/
          goto LABEL_19; /*0x904bc9*/
        goto LABEL_18; /*0x904bc9*/
      }
    }
LABEL_19:
    ++v10; /*0x904bd8*/
    ++v5; /*0x904bdb*/
  }
  while ( v10 < 52 ); /*0x904be2*/
  return result; /*0x904be8*/
}
`

## CUIUserInfoDetail::Draw

`c
__int64 *__thiscall CUIUserInfoDetail::Draw(_DWORD *this, int arg0)
{
  int v3; // ecx
  _DWORD *Instance; // eax
  int v5; // eax
  int *v6; // eax
  int v7; // eax
  int v8; // eax
  int v9; // eax
  int v10; // eax
  int v11; // edx
  unsigned int v12; // edi
  int v13; // eax
  int v14; // eax
  int v15; // ecx
  _BYTE *v16; // ecx
  char Data; // al
  int ItemName; // eax
  unsigned int v19; // ecx
  int *basic_font; // eax
  int v21; // eax
  int v22; // ecx
  int v23; // edx
  int v24; // eax
  _DWORD *v25; // eax
  int v26; // eax
  int v27; // eax
  int v28; // eax
  int v29; // eax
  int v30; // edx
  int v31; // eax
  int v32; // eax
  int v33; // eax
  unsigned int v34; // ecx
  _DWORD *v35; // eax
  VARIANTARG *Item; // esi
  _DWORD *v37; // eax
  _DWORD *String; // edi
  char *v39; // eax
  int v40; // eax
  int v41; // eax
  int v42; // eax
  int *v43; // eax
  int v44; // eax
  int v45; // ecx
  int v46; // edx
  int v47; // eax
  _DWORD *v48; // eax
  int v49; // eax
  int v50; // eax
  int v51; // eax
  __int64 *result; // eax
  int v53; // [esp-28h] [ebp-108h] BYREF
  int v54; // [esp-24h] [ebp-104h]
  int v55; // [esp-20h] [ebp-100h]
  int v56; // [esp-1Ch] [ebp-FCh]
  int v57; // [esp-18h] [ebp-F8h]
  int v58; // [esp-14h] [ebp-F4h] BYREF
  __int64 v59; // [esp-10h] [ebp-F0h] BYREF
  _QWORD v60[2]; // [esp-8h] [ebp-E8h] BYREF
  VARIANTARG v61; // [esp+Ch] [ebp-D4h] BYREF
  VARIANTARG v62; // [esp+1Ch] [ebp-C4h] BYREF
  VARIANTARG v63; // [esp+2Ch] [ebp-B4h] BYREF
  VARIANTARG v64; // [esp+3Ch] [ebp-A4h] BYREF
  VARIANTARG v65; // [esp+4Ch] [ebp-94h] BYREF
  VARIANTARG v66; // [esp+5Ch] [ebp-84h] BYREF
  VARIANTARG pvarg; // [esp+6Ch] [ebp-74h] BYREF
  VARIANTARG v68; // [esp+7Ch] [ebp-64h] BYREF
  VARIANTARG pvargDest; // [esp+8Ch] [ebp-54h] BYREF
  int v70; // [esp+9Ch] [ebp-44h] BYREF
  int v71; // [esp+A0h] [ebp-40h] BYREF
  _BYTE v72[4]; // [esp+A4h] [ebp-3Ch] BYREF
  int Unknown; // [esp+A8h] [ebp-38h] BYREF
  int a2; // [esp+ACh] [ebp-34h] BYREF
  int v75; // [esp+B0h] [ebp-30h] BYREF
  int v76; // [esp+B4h] [ebp-2Ch]
  int v77; // [esp+B8h] [ebp-28h]
  int v78; // [esp+BCh] [ebp-24h]
  _DWORD *v79; // [esp+C0h] [ebp-20h]
  _DWORD *v80; // [esp+C4h] [ebp-1Ch]
  __int64 *v81; // [esp+C8h] [ebp-18h] BYREF
  int v82; // [esp+CCh] [ebp-14h] BYREF
  __int64 *v83; // [esp+D0h] [ebp-10h]
  int v84; // [esp+DCh] [ebp-4h]
  int v85; // [esp+E8h] [ebp+8h]

  v80 = this; /*0x8fd4d8*/
  CWnd::Draw(this, a2: arg0); /*0x8fd4db*/
  CWnd::GetCanvas(a1: &v81); /*0x8fd4e6*/
  v84 = 0; /*0x8fd4f9*/
  sub_402F85(pvargDest: &v66, pvargSrc: &pvargSrc); /*0x8fd4fc*/
  LOBYTE(v84) = 1; /*0x8fd508*/
  sub_402F85(pvargDest: &v64, pvargSrc: &pvargSrc); /*0x8fd50c*/
  v60[0] = 0; /*0x8fd512*/
  HIDWORD(v59) = &v66; /*0x8fd519*/
  LODWORD(v59) = &v64; /*0x8fd520*/
  v58 = v3; /*0x8fd521*/
  LOBYTE(v84) = 2; /*0x8fd52d*/
  Instance = StringPool::GetInstance(); /*0x8fd531*/
  StringPool::GetBSTR(this: Instance, a2: &v58, a3: SP_2564_UI_UIWINDOWIMG_USERINFO_REQLV); /*0x8fd538*/
  LOBYTE(v84) = 2; /*0x8fd544*/
  v5 = sub_406472(); /*0x8fd548*/
  v6 = sub_402680(a1: v5); /*0x8fd54f*/
  IWzResMan::GetObjectA(this: v6, pvargDest: &v62, sUOL: v58, vParam: v59, vAux: SHIDWORD(v59)); /*0x8fd556*/
  LOBYTE(v84) = 4; /*0x8fd55d*/
  Unknown = Ztl_variant_t::GetUnknown(a1: v60[0], a2: SBYTE4(v60[0])); /*0x8fd568*/
  v77 = 0; /*0x8fd56b*/
  if ( Unknown != 0 ) /*0x8fd56e*/
  {
    v7 = sub_41E527(a1: &Unknown); /*0x8fd577*/
    if ( v7 < 0 && v7 != -2147467262 ) /*0x8fd585*/
      _com_issue_error(a1: v7); /*0x8fd588*/
  }
  LOBYTE(v84) = 8; /*0x8fd594*/
  v8 = ZComAPI::ZComVariantCopy(pvarg: &v62); /*0x8fd598*/
  if ( v8 < 0 ) /*0x8fd5a0*/
    _com_issue_error(a1: v8); /*0x8fd5a3*/
  LOBYTE(v84) = 7; /*0x8fd5af*/
  v9 = ZComAPI::ZComVariantCopy(pvarg: &v64); /*0x8fd5b3*/
  if ( v9 < 0 ) /*0x8fd5bb*/
    _com_issue_error(a1: v9); /*0x8fd5be*/
  LOBYTE(v84) = 6; /*0x8fd5ca*/
  v10 = ZComAPI::ZComVariantCopy(pvarg: &v66); /*0x8fd5ce*/
  if ( v10 < 0 ) /*0x8fd5d6*/
    _com_issue_error(a1: v10); /*0x8fd5d9*/
  v82 = 0; /*0x8fd5e4*/
  v12 = *(*(this + 30) + 56); /*0x8fd5ea*/
  LOBYTE(v84) = 9; /*0x8fd5f4*/
  v85 = v12; /*0x8fd5f8*/
  if ( !__OFSUB__(v12, v12 + 3) ) /*0x8fd5fb*/
  {
    v79 = (dword_BEDCCC + 1796); /*0x8fd60d*/
    v78 = 12 * v12; /*0x8fd610*/
    while ( *v79 != 0 && v12 < *(*v79 - 4) ) /*0x8fd61d*/
    {
      v60[0] = 0; /*0x8fd634*/
      v59 = 0; /*0x8fd636*/
      v58 = 0; /*0x8fd637*/
      v57 = 0; /*0x8fd638*/
      v13 = sub_64BCCC(a1: v80 + 29, a2: v11); /*0x8fd63c*/
      v56 = 40 * (v12 - sub_64B1A1(a1: v13)) + 64; /*0x8fd659*/
      v55 = 17; /*0x8fd65a*/
      v14 = sub_9052D6(a1: v12); /*0x8fd65d*/
      v54 = TSecType<long>::TSecType<long>(a1: v14); /*0x8fd669*/
      v53 = v15; /*0x8fd66a*/
      v83 = &v53; /*0x8fd670*/
      sub_4281DD(a1: &v81); /*0x8fd674*/
      LOBYTE(v84) = 9; /*0x8fd679*/
      sub_4283F8(); /*0x8fd67d*/
      sub_5D6458( /*0x8fd684*/
        a1: v53,
        a2: v54,
        a3: v55,
        a4: v56,
        a5: v57,
        a6: v58,
        a7: v59,
        a8: HIDWORD(v59),
        a9: v60[0],
        a10: HIDWORD(v60[0]));
      v16 = (*v79 + v78); /*0x8fd68f*/
      v76 = dword_BE78D8; /*0x8fd696*/
      Data = TSecType<long>::GetData(a1: v16, a2: &pvargSrc); /*0x8fd699*/
      ItemName = CItemInfo::GetItemName(a1: v72, a2: Data); /*0x8fd6a6*/
      LOBYTE(v84) = 11; /*0x8fd6af*/
      ZXString<char>::operator=(a1: ItemName); /*0x8fd6b3*/
      LOBYTE(v84) = 9; /*0x8fd6bb*/
      ZXString<char>::_Release(a1: v72); /*0x8fd6bf*/
      v60[0] = v19 | 0x9900000000LL; /*0x8fd6c9*/
      v83 = v60; /*0x8fd6cc*/
      get_basic_font(a1: v60, fType: 1); /*0x8fd6d2*/
      format_string(sFormat: &v82, pFont: v60[0], nWidth: HIDWORD(v60[0])); /*0x8fd6dd*/
      sub_402F85(pvargDest: &v65, pvargSrc: &pvargSrc); /*0x8fd6ec*/
      LOBYTE(v84) = 12; /*0x8fd6f8*/
      sub_402F85(pvargDest: &v63, pvargSrc: &pvargSrc); /*0x8fd6fc*/
      HIDWORD(v60[0]) = &v65; /*0x8fd707*/
      LODWORD(v60[0]) = &v63; /*0x8fd70e*/
      LOBYTE(v84) = 13; /*0x8fd70f*/
      basic_font = get_basic_font(a1: &v71, fType: 1); /*0x8fd719*/
      LOBYTE(v84) = 14; /*0x8fd722*/
      HIDWORD(v59) = sub_4284A5(this: basic_font); /*0x8fd72b*/
      v21 = sub_428226(a1: &v82); /*0x8fd72f*/
      LODWORD(v59) = v22; /*0x8fd734*/
      v83 = &v59; /*0x8fd737*/
      _bstr_t::_bstr_t(a1: v21); /*0x8fd73b*/
      LOBYTE(v84) = 14; /*0x8fd746*/
      v24 = sub_64BCCC(a1: v80 + 29, a2: v23); /*0x8fd74a*/
      v58 = 40 * (v12 - sub_64B1A1(a1: v24)) + 32; /*0x8fd764*/
      v57 = 57; /*0x8fd765*/
      v25 = sub_414576(a1: &v81); /*0x8fd76a*/
      IWzCanvas::DrawTextA( /*0x8fd771*/
        this: v25,
        nLeft: v57,
        nTop: v58,
        sText: v59,
        pFont: SHIDWORD(v59),
        vAlpha: v60[0],
        vTabOrg: HIDWORD(v60[0]));
      LOBYTE(v84) = 13; /*0x8fd77b*/
      if ( v71 != 0 ) /*0x8fd77f*/
        (*(*v71 + 8))(a1: v71); /*0x8fd784*/
      LOBYTE(v84) = 12; /*0x8fd78e*/
      v26 = ZComAPI::ZComVariantCopy(pvarg: &v63); /*0x8fd792*/
      if ( v26 < 0 ) /*0x8fd79a*/
        _com_issue_error(a1: v26); /*0x8fd79d*/
      LOBYTE(v84) = 9; /*0x8fd7a9*/
      v27 = ZComAPI::ZComVariantCopy(pvarg: &v65); /*0x8fd7ad*/
      if ( v27 < 0 ) /*0x8fd7b5*/
        _com_issue_error(a1: v27); /*0x8fd7b8*/
      VariantInit(&pvarg); /*0x8fd7c1*/
      v28 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x8fd7cc*/
      if ( v28 < 0 ) /*0x8fd7d5*/
        _com_issue_error(a1: v28); /*0x8fd7d8*/
      v29 = *(v80[30] + 56); /*0x8fd7e8*/
      LOBYTE(v84) = 16; /*0x8fd7eb*/
      v76 = v29; /*0x8fd7ef*/
      if ( v81 == 0 ) /*0x8fd7f2*/
        _com_issue_error(a1: -2147467261); /*0x8fd7f9*/
      v30 = *v81; /*0x8fd807*/
      v58 = v77; /*0x8fd80e*/
      v59 = *&pvarg.vt; /*0x8fd814*/
      v57 = 40 * (v85 - v76) + 53; /*0x8fd820*/
      v60[0] = pvarg.cyVal.int64; /*0x8fd821*/
      v56 = 57; /*0x8fd822*/
      v55 = v81; /*0x8fd824*/
      v83 = v81; /*0x8fd825*/
      v31 = (*(v30 + 128))( /*0x8fd829*/
              a1: v81,
              a2: 57,
              a3: v57,
              a4: v77,
              a5: *&pvarg.vt,
              a6: pvarg.decVal.Hi32,
              a7: pvarg.lVal,
              a8: pvarg.cyVal.Hi);
      if ( v31 < 0 ) /*0x8fd831*/
        _com_issue_errorex(a1: v31, a2: v83, a3: _GUID_7600dc6c_9328_4bff_9624_5b0f5c01179e); /*0x8fd83c*/
      LOBYTE(v84) = 9; /*0x8fd845*/
      v32 = ZComAPI::ZComVariantCopy(&pvarg); /*0x8fd849*/
      if ( v32 < 0 ) /*0x8fd851*/
        _com_issue_error(a1: v32); /*0x8fd854*/
      v33 = TSecType<long>::GetData(a1: (*v79 + v78), a2: &pvargSrc); /*0x8fd867*/
      CItemInfo::GetItemInfo(a1: &v75, a2: v33); /*0x8fd873*/
      v60[0] = v34; /*0x8fd87a*/
      v83 = v60; /*0x8fd87d*/
      HIDWORD(v59) = SP_1887_REQLEVEL; /*0x8fd880*/
      LODWORD(v59) = v60; /*0x8fd885*/
      LOBYTE(v84) = 17; /*0x8fd886*/
      v35 = StringPool::GetInstance(); /*0x8fd88a*/
      StringPool::GetBSTR(this: v35, a2: v59, a3: SHIDWORD(v59)); /*0x8fd891*/
      HIDWORD(v59) = &v61; /*0x8fd89c*/
      LOBYTE(v84) = 17; /*0x8fd8a0*/
      sub_40263B(a1: &v75); /*0x8fd8a4*/
      Item = IWzProperty::GetItem(pvargDest: HIDWORD(v59), a2: v60[0]); /*0x8fd8b0*/
      LODWORD(v60[0]) = SP_5515_D; /*0x8fd8b5*/
      HIDWORD(v59) = &a2; /*0x8fd8ba*/
      LOBYTE(v84) = 19; /*0x8fd8bb*/
      v37 = StringPool::GetInstance(); /*0x8fd8bf*/
      String = StringPool::GetString(this: v37, result: HIDWORD(v59), nIdx: v60[0]); /*0x8fd8cb*/
      LOBYTE(v84) = 20; /*0x8fd8ce*/
      HIDWORD(v60[0]) = get_int32(pvarSrc: Item, a2: SHIDWORD(v60[0])); /*0x8fd8d9*/
      v39 = sub_428226(a1: String); /*0x8fd8dc*/
      ZXString<char>::Format(a1: &v82, a2: v39, a3: SBYTE4(v60[0])); /*0x8fd8e6*/
      LOBYTE(v84) = 19; /*0x8fd8f1*/
      ZXString<char>::_Release(a1: &a2); /*0x8fd8f5*/
      LOBYTE(v84) = 17; /*0x8fd901*/
      v40 = ZComAPI::ZComVariantCopy(pvarg: &v61); /*0x8fd905*/
      if ( v40 < 0 ) /*0x8fd90d*/
        _com_issue_error(a1: v40); /*0x8fd910*/
      VariantInit(pvarg: &pvargDest); /*0x8fd91f*/
      v41 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc); /*0x8fd926*/
      if ( v41 < 0 ) /*0x8fd92f*/
        _com_issue_error(a1: v41); /*0x8fd932*/
      LOBYTE(v84) = 21; /*0x8fd93b*/
      VariantInit(pvarg: &v68); /*0x8fd93f*/
      v42 = ZtlTaskMemFreeImp(pvargDest: &v68, pvargSrc: &pvargSrc); /*0x8fd946*/
      if ( v42 < 0 ) /*0x8fd94f*/
        _com_issue_error(a1: v42); /*0x8fd952*/
      HIDWORD(v60[0]) = &pvargDest; /*0x8fd95a*/
      LODWORD(v60[0]) = &v68; /*0x8fd95e*/
      LOBYTE(v84) = 22; /*0x8fd965*/
      v43 = get_basic_font(a1: &v70, fType: 1); /*0x8fd969*/
      LOBYTE(v84) = 23; /*0x8fd972*/
      HIDWORD(v59) = sub_4284A5(this: v43); /*0x8fd97b*/
      v44 = sub_428226(a1: &v82); /*0x8fd97f*/
      LODWORD(v59) = v45; /*0x8fd984*/
      v83 = &v59; /*0x8fd987*/
      _bstr_t::_bstr_t(a1: v44); /*0x8fd98b*/
      LOBYTE(v84) = 23; /*0x8fd996*/
      v47 = sub_64BCCC(a1: v80 + 29, a2: v46); /*0x8fd99a*/
      v58 = 40 * (v85 - sub_64B1A1(a1: v47)) + 50; /*0x8fd9b5*/
      v57 = 93; /*0x8fd9b6*/
      v48 = sub_414576(a1: &v81); /*0x8fd9bb*/
      IWzCanvas::DrawTextA( /*0x8fd9c2*/
        this: v48,
        nLeft: v57,
        nTop: v58,
        sText: v59,
        pFont: SHIDWORD(v59),
        vAlpha: v60[0],
        vTabOrg: HIDWORD(v60[0]));
      LOBYTE(v84) = 22; /*0x8fd9cc*/
      if ( v70 != 0 ) /*0x8fd9d0*/
        (*(*v70 + 8))(a1: v70); /*0x8fd9d5*/
      LOBYTE(v84) = 21; /*0x8fd9dc*/
      v49 = ZComAPI::ZComVariantCopy(pvarg: &v68); /*0x8fd9e0*/
      if ( v49 < 0 ) /*0x8fd9e8*/
        _com_issue_error(a1: v49); /*0x8fd9eb*/
      LOBYTE(v84) = 17; /*0x8fd9f4*/
      v50 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x8fd9f8*/
      if ( v50 < 0 ) /*0x8fda00*/
        _com_issue_error(a1: v50); /*0x8fda03*/
      LOBYTE(v84) = 9; /*0x8fda0d*/
      if ( v75 != 0 ) /*0x8fda11*/
        (*(*v75 + 8))(a1: v75); /*0x8fda16*/
      v51 = *(v80[30] + 56); /*0x8fda1f*/
      ++v85; /*0x8fda22*/
      v78 += 12; /*0x8fda25*/
      if ( v85 >= v51 + 3 ) /*0x8fda2f*/
        break; /*0x8fda2f*/
      v12 = v85; /*0x8fd615*/
    }
  }
  LOBYTE(v84) = 6; /*0x8fda3a*/
  ZXString<char>::_Release(a1: &v82); /*0x8fda3e*/
  LOBYTE(v84) = 0; /*0x8fda46*/
  if ( v77 != 0 ) /*0x8fda4c*/
    (*(*v77 + 8))(a1: v77); /*0x8fda51*/
  result = v81; /*0x8fda54*/
  v84 = -1; /*0x8fda57*/
  if ( v81 != 0 ) /*0x8fda5d*/
    return (*(*v81 + 8))(a1: v81); /*0x8fda62*/
  return result; /*0x8fda65*/
}
`

## CUIUserInfoDetail::OnCreate

`c
CWnd *__fastcall CUIUserInfoDetail::OnCreate(CWnd *a1, int a2, int a3, int a4)
{
  CWnd::CWnd(this: a1); /*0x8ff2d9*/
  dword_BF1174 = (a1 + 108) != 0 ? a1 : 0;
  *(a1 + 28) = 0; /*0x8ff2f7*/
  *(a1 + 30) = 0; /*0x8ff2fa*/
  *a1 = &off_B3BEEC; /*0x8ff31a*/
  *(a1 + 1) = &off_B3BEA0; /*0x8ff320*/
  *(a1 + 2) = &off_B3BE9C; /*0x8ff327*/
  CWnd::CreateWnd( /*0x8ff32e*/
    this: a1,
    nLeft: a3,
    nTop: a4,
    nWidth: 240,
    nHeight: 162,
    z: 10,
    bScreenCoord: 1,
    pData: 0,
    bSetFocus: 1);
  return a1; /*0x8ff333*/
}
`

## CUIUserInfoWishList::Draw

`c
_DWORD *__thiscall CUIUserInfoWishList::Draw(_DWORD *this, int arg0)
{
  _DWORD *v2; // esi
  int v3; // edx
  signed int v4; // edi
  int v5; // ecx
  int v6; // eax
  int Data; // eax
  int v8; // eax
  int v9; // edx
  _DWORD *v10; // esi
  int v11; // eax
  int v12; // eax
  int v13; // ecx
  int v14; // eax
  int v15; // eax
  _BYTE *v16; // ecx
  int v17; // eax
  int v18; // eax
  int v19; // ecx
  char v20; // al
  int ItemName; // eax
  VARIANTARG *v22; // ecx
  int v23; // eax
  int v24; // eax
  int *basic_font; // eax
  int v26; // eax
  int *v27; // ecx
  int v28; // edx
  int v29; // eax
  int v30; // eax
  _DWORD *v31; // eax
  int v32; // eax
  int v33; // eax
  _DWORD *Instance; // eax
  VARIANTARG **String; // eax
  size_t v36; // ecx
  int v37; // eax
  int v38; // eax
  int *v39; // eax
  int v40; // eax
  int *v41; // ecx
  int v42; // edx
  int v43; // eax
  int v44; // eax
  _DWORD *v45; // eax
  int v46; // eax
  int v47; // eax
  int v48; // eax
  _DWORD *result; // eax
  int v50; // [esp-28h] [ebp-BCh] BYREF
  int v51; // [esp-24h] [ebp-B8h]
  int v52; // [esp-20h] [ebp-B4h]
  int v53; // [esp-1Ch] [ebp-B0h]
  int v54; // [esp-18h] [ebp-ACh] BYREF
  int v55; // [esp-14h] [ebp-A8h]
  int v56; // [esp-10h] [ebp-A4h] BYREF
  int v57; // [esp-Ch] [ebp-A0h]
  VARIANTARG *p_pvargDest; // [esp-8h] [ebp-9Ch] BYREF
  StringPoolStrings p_pvarg; // [esp-4h] [ebp-98h]
  VARIANTARG v60; // [esp+Ch] [ebp-88h] BYREF
  VARIANTARG v61; // [esp+1Ch] [ebp-78h] BYREF
  VARIANTARG pvarg; // [esp+2Ch] [ebp-68h] BYREF
  VARIANTARG pvargDest; // [esp+3Ch] [ebp-58h] BYREF
  _BYTE v64[4]; // [esp+4Ch] [ebp-48h] BYREF
  int v65; // [esp+50h] [ebp-44h]
  int v66; // [esp+54h] [ebp-40h] BYREF
  int a2; // [esp+58h] [ebp-3Ch] BYREF
  int *v68; // [esp+5Ch] [ebp-38h]
  int v69; // [esp+60h] [ebp-34h] BYREF
  _BYTE v70[4]; // [esp+64h] [ebp-30h] BYREF
  _BYTE v71[4]; // [esp+68h] [ebp-2Ch] BYREF
  int v72; // [esp+6Ch] [ebp-28h]
  int v73; // [esp+70h] [ebp-24h]
  _DWORD *v74; // [esp+74h] [ebp-20h]
  int v75; // [esp+78h] [ebp-1Ch]
  void *v76; // [esp+7Ch] [ebp-18h] BYREF
  _DWORD *v77; // [esp+80h] [ebp-14h] BYREF
  int v78; // [esp+84h] [ebp-10h]
  int v79; // [esp+90h] [ebp-4h]

  v2 = this; /*0x8fde1b*/
  v74 = this; /*0x8fde1d*/
  CWnd::Draw(this, a2: arg0); /*0x8fde20*/
  CWnd::GetCanvas(a1: &v77); /*0x8fde2b*/
  v3 = dword_BEDCCC; /*0x8fde30*/
  v79 = 0; /*0x8fde3d*/
  v73 = dword_BEDCCC; /*0x8fde40*/
  v72 = *dword_BE7918; /*0x8fde43*/
  arg0 = 0; /*0x8fde46*/
  v4 = *(v2[30] + 56); /*0x8fde4c*/
  LOBYTE(v79) = 1; /*0x8fde56*/
  v78 = v4; /*0x8fde5a*/
  if ( v4 < v4 + 3 ) /*0x8fde5d*/
  {
    v5 = 12 * v4; /*0x8fde66*/
    v75 = 12 * v4; /*0x8fde69*/
    while ( 1 ) /*0x8fde74*/
    {
      v6 = *(v3 + 1800); /*0x8fde74*/
      if ( v6 == 0 || v4 >= *(v6 - 4) ) /*0x8fde85*/
        break; /*0x8fde85*/
      Data = TSecType<long>::GetData(a1: (v6 + v5), a2: 0); /*0x8fde8d*/
      CWvsContext::GetCommodityBySN(a1: v64, a2: Data); /*0x8fde9a*/
      LOBYTE(v79) = 2; /*0x8fdea4*/
      if ( v65 != 0 && TSecType<long>::GetData(a1: (v65 + 16), a2: 0) != 0 ) /*0x8fdeb8*/
      {
        v8 = TSecType<long>::GetData(a1: (v65 + 16), a2: 0); /*0x8fdec4*/
        v9 = v8 % 100000; /*0x8fdecf*/
        p_pvarg = SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK; /*0x8fded1*/
        p_pvargDest = 0; /*0x8fded2*/
        if ( v8 / 100000 == 91 ) /*0x8fded6*/
        {
          v10 = v2 + 29; /*0x8fdedc*/
          v11 = sub_64BCCC(a1: v10, a2: v9); /*0x8fdee1*/
          v57 = 40 * (v4 - sub_64B1A1(a1: v11)) + 64; /*0x8fdefb*/
          v56 = 17; /*0x8fdefc*/
          v12 = sub_4738E0(a1: v64); /*0x8fdf01*/
          v55 = TSecType<long>::TSecType<long>(a1: v12 + 16); /*0x8fdf10*/
          v54 = v13; /*0x8fdf11*/
          v68 = &v54; /*0x8fdf17*/
          sub_4281DD(a1: &v77); /*0x8fdf1b*/
          LOBYTE(v79) = 2; /*0x8fdf20*/
          sub_4283F8(); /*0x8fdf24*/
          sub_5D79EE(a1: v54, a2: v55, a3: v56, a4: v57, a5: p_pvargDest, a6: p_pvarg); /*0x8fdf2b*/
          v14 = TSecType<long>::GetData(a1: (v65 + 16), a2: 0); /*0x8fdf3c*/
          v15 = sub_5D7787(a1: v71, a2: v14); /*0x8fdf48*/
          LOBYTE(v79) = 4; /*0x8fdf51*/
          ZXString<char>::operator=(a1: v15); /*0x8fdf55*/
          LOBYTE(v79) = 2; /*0x8fdf5a*/
          v16 = v71; /*0x8fdf5e*/
        }
        else
        {
          v57 = 0; /*0x8fdf66*/
          v56 = 0; /*0x8fdf67*/
          v55 = 0; /*0x8fdf68*/
          v10 = v2 + 29; /*0x8fdf69*/
          v54 = 0; /*0x8fdf6c*/
          v17 = sub_64BCCC(a1: v10, a2: v9); /*0x8fdf6f*/
          v53 = 40 * (v4 - sub_64B1A1(a1: v17)) + 64; /*0x8fdf89*/
          v52 = 17; /*0x8fdf8a*/
          v18 = sub_4738E0(a1: v64); /*0x8fdf8f*/
          v51 = TSecType<long>::TSecType<long>(a1: v18 + 16); /*0x8fdf9e*/
          v50 = v19; /*0x8fdf9f*/
          v68 = &v50; /*0x8fdfa5*/
          sub_4281DD(a1: &v77); /*0x8fdfa9*/
          LOBYTE(v79) = 2; /*0x8fdfae*/
          sub_4283F8(); /*0x8fdfb2*/
          sub_5D6458( /*0x8fdfb9*/
            a1: v50,
            a2: v51,
            a3: v52,
            a4: v53,
            a5: v54,
            a6: v55,
            a7: v56,
            a8: v57,
            a9: p_pvargDest,
            a10: p_pvarg);
          v20 = TSecType<long>::GetData(a1: (v65 + 16), a2: 0); /*0x8fdfca*/
          ItemName = CItemInfo::GetItemName(a1: v70, a2: v20); /*0x8fdfd6*/
          LOBYTE(v79) = 6; /*0x8fdfdf*/
          ZXString<char>::operator=(a1: ItemName); /*0x8fdfe3*/
          LOBYTE(v79) = 2; /*0x8fdfe8*/
          v16 = v70; /*0x8fdfec*/
        }
        ZXString<char>::_Release(a1: v16); /*0x8fdfef*/
        p_pvarg = SP_153_S_IS_AT_S; /*0x8fdff4*/
        p_pvargDest = v22; /*0x8fdff9*/
        v68 = &p_pvargDest; /*0x8fdffc*/
        get_basic_font(a1: &p_pvargDest, fType: 1); /*0x8fe002*/
        format_string(sFormat: &arg0, pFont: p_pvargDest, nWidth: p_pvarg); /*0x8fe00d*/
        VariantInit(&pvarg); /*0x8fe01f*/
        v23 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc); /*0x8fe02a*/
        if ( v23 < 0 ) /*0x8fe033*/
          _com_issue_error(a1: v23); /*0x8fe036*/
        LOBYTE(v79) = 7; /*0x8fe03f*/
        VariantInit(pvarg: &pvargDest); /*0x8fe043*/
        v24 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc); /*0x8fe04e*/
        if ( v24 < 0 ) /*0x8fe057*/
          _com_issue_error(a1: v24); /*0x8fe05a*/
        p_pvarg = &pvarg; /*0x8fe062*/
        p_pvargDest = &pvargDest; /*0x8fe066*/
        LOBYTE(v79) = 8; /*0x8fe06d*/
        basic_font = get_basic_font(a1: &v69, fType: 1); /*0x8fe071*/
        LOBYTE(v79) = 9; /*0x8fe07a*/
        v57 = sub_4284A5(this: basic_font); /*0x8fe083*/
        v26 = sub_428226(a1: &arg0); /*0x8fe087*/
        v56 = v27; /*0x8fe08c*/
        v68 = &v56; /*0x8fe08f*/
        _bstr_t::_bstr_t(a1: v26); /*0x8fe093*/
        LOBYTE(v79) = 9; /*0x8fe09a*/
        v29 = sub_64BCCC(a1: v10, a2: v28); /*0x8fe09e*/
        v30 = sub_64B1A1(a1: v29); /*0x8fe0a5*/
        v55 = 40 * (v78 - v30) + 32; /*0x8fe0b9*/
        v54 = 57; /*0x8fe0ba*/
        v31 = sub_414576(a1: &v77); /*0x8fe0bf*/
        IWzCanvas::DrawTextA( /*0x8fe0c6*/
          this: v31,
          nLeft: v54,
          nTop: v55,
          sText: v56,
          pFont: v57,
          vAlpha: p_pvargDest,
          vTabOrg: p_pvarg);
        LOBYTE(v79) = 8; /*0x8fe0d0*/
        if ( v69 != 0 ) /*0x8fe0d4*/
          (*(*v69 + 8))(a1: v69); /*0x8fe0d9*/
        LOBYTE(v79) = 7; /*0x8fe0e0*/
        v32 = ZComAPI::ZComVariantCopy(pvarg: &pvargDest); /*0x8fe0e4*/
        if ( v32 < 0 ) /*0x8fe0ec*/
          _com_issue_error(a1: v32); /*0x8fe0ef*/
        LOBYTE(v79) = 2; /*0x8fe0f8*/
        v33 = ZComAPI::ZComVariantCopy(&pvarg); /*0x8fe0fc*/
        if ( v33 < 0 ) /*0x8fe104*/
          _com_issue_error(a1: v33); /*0x8fe107*/
        if ( *(v65 + 28) > 1 ) /*0x8fe113*/
        {
          v76 = 0; /*0x8fe115*/
          v68 = v65; /*0x8fe118*/
          p_pvarg = SP_572_D; /*0x8fe11e*/
          p_pvargDest = &a2; /*0x8fe123*/
          LOBYTE(v79) = 11; /*0x8fe124*/
          Instance = StringPool::GetInstance(); /*0x8fe128*/
          String = StringPool::GetString(this: Instance, result: p_pvargDest, nIdx: p_pvarg); /*0x8fe12f*/
          p_pvarg = v68[7]; /*0x8fe137*/
          p_pvargDest = *String; /*0x8fe13c*/
          LOBYTE(v79) = 12; /*0x8fe141*/
          ZXString<char>::Format(a1: &v76, a2: p_pvargDest, a3: p_pvarg); /*0x8fe145*/
          LOBYTE(v79) = 11; /*0x8fe150*/
          ZXString<char>::_Release(a1: &a2); /*0x8fe154*/
          if ( v76 != 0 ) /*0x8fe15e*/
            v36 = *(v76 - 1); /*0x8fe160*/
          else
            v36 = 0; /*0x8fe165*/
          ZXString<char>::_Cat(a1: v76, a2: v36); /*0x8fe16c*/
          LOBYTE(v79) = 2; /*0x8fe174*/
          ZXString<char>::_Release(a1: &v76); /*0x8fe178*/
        }
        VariantInit(pvarg: &v60); /*0x8fe184*/
        v37 = ZtlTaskMemFreeImp(pvargDest: &v60, pvargSrc: &pvargSrc); /*0x8fe192*/
        if ( v37 < 0 ) /*0x8fe19b*/
          _com_issue_error(a1: v37); /*0x8fe19e*/
        LOBYTE(v79) = 13; /*0x8fe1a7*/
        VariantInit(pvarg: &v61); /*0x8fe1ab*/
        v38 = ZtlTaskMemFreeImp(pvargDest: &v61, pvargSrc: &pvargSrc); /*0x8fe1b6*/
        if ( v38 < 0 ) /*0x8fe1bf*/
          _com_issue_error(a1: v38); /*0x8fe1c2*/
        p_pvarg = &v60; /*0x8fe1cd*/
        p_pvargDest = &v61; /*0x8fe1d1*/
        LOBYTE(v79) = 14; /*0x8fe1d8*/
        v39 = get_basic_font(a1: &v66, fType: 1); /*0x8fe1dc*/
        LOBYTE(v79) = 15; /*0x8fe1e5*/
        v57 = sub_4284A5(this: v39); /*0x8fe1ee*/
        v40 = sub_428226(a1: &arg0); /*0x8fe1f2*/
        v56 = v41; /*0x8fe1f7*/
        v68 = &v56; /*0x8fe1fa*/
        _bstr_t::_bstr_t(a1: v40); /*0x8fe1fe*/
        LOBYTE(v79) = 15; /*0x8fe205*/
        v43 = sub_64BCCC(a1: v10, a2: v42); /*0x8fe209*/
        v44 = sub_64B1A1(a1: v43); /*0x8fe210*/
        v55 = 40 * (v78 - v44) + 32; /*0x8fe224*/
        v54 = 57; /*0x8fe225*/
        v45 = sub_414576(a1: &v77); /*0x8fe22a*/
        IWzCanvas::DrawTextA( /*0x8fe231*/
          this: v45,
          nLeft: v54,
          nTop: v55,
          sText: v56,
          pFont: v57,
          vAlpha: p_pvargDest,
          vTabOrg: p_pvarg);
        LOBYTE(v79) = 14; /*0x8fe23b*/
        if ( v66 != 0 ) /*0x8fe23f*/
          (*(*v66 + 8))(a1: v66); /*0x8fe244*/
        LOBYTE(v79) = 13; /*0x8fe24b*/
        v46 = ZComAPI::ZComVariantCopy(pvarg: &v61); /*0x8fe24f*/
        if ( v46 < 0 ) /*0x8fe257*/
          _com_issue_error(a1: v46); /*0x8fe25a*/
        LOBYTE(v79) = 2; /*0x8fe266*/
        v47 = ZComAPI::ZComVariantCopy(pvarg: &v60); /*0x8fe26a*/
        if ( v47 < 0 ) /*0x8fe272*/
          _com_issue_error(a1: v47); /*0x8fe275*/
        LOBYTE(v79) = 1; /*0x8fe27d*/
        if ( v65 != 0 ) /*0x8fe281*/
        {
          sub_473F9E(a1: 0); /*0x8fe287*/
          v65 = 0; /*0x8fe28c*/
        }
        v4 = v78; /*0x8fe28f*/
        v2 = v74; /*0x8fe292*/
      }
      else
      {
        LOBYTE(v79) = 1; /*0x8fe2de*/
        ZRef<CS_COMMODITY>::~ZRef<CS_COMMODITY>(a1: v64); /*0x8fe2e2*/
      }
      v48 = *(v2[30] + 56); /*0x8fe298*/
      v75 += 12; /*0x8fe29b*/
      v78 = ++v4; /*0x8fe2a5*/
      if ( v4 >= v48 + 3 ) /*0x8fe2a8*/
        break; /*0x8fe2a8*/
      v5 = v75; /*0x8fde6e*/
      v3 = v73; /*0x8fde71*/
    }
  }
  LOBYTE(v79) = 0; /*0x8fe2b1*/
  ZXString<char>::_Release(a1: &arg0); /*0x8fe2b4*/
  result = v77; /*0x8fe2b9*/
  v79 = -1; /*0x8fe2bc*/
  if ( v77 != 0 ) /*0x8fe2c2*/
    return (*(*v77 + 8))(a1: v77); /*0x8fe2c7*/
  return result; /*0x8fe2ca*/
}
`

## CUIUserInfoWishList::OnCreate

`c
_DWORD *__fastcall CUIUserInfoWishList::OnCreate(_DWORD *a1, int a2, int a3, int a4)
{
  CWnd::CWnd(this: a1); /*0x8ff3df*/
  dword_BF1170 = a1 + 27 != 0 ? a1 : 0;
  a1[28] = 0; /*0x8ff3fc*/
  a1[30] = 0; /*0x8ff400*/
  `eh vector constructor iterator'(a1: a1 + 31, a2: 8u, a3: 3, a4: sub_47391D, a5: ZRef<CCtrlButton>::_ReleaseRaw); /*0x8ff41a*/
  CUIToolTip::CUIToolTip(this: a1 + 37); /*0x8ff429*/
  *a1 = &off_B3BF70; /*0x8ff44c*/
  a1[1] = &off_B3BF24; /*0x8ff452*/
  a1[2] = &off_B3BF20; /*0x8ff459*/
  CWnd::CreateWnd( /*0x8ff460*/
    this: a1,
    nLeft: a3,
    nTop: a4,
    nWidth: 240,
    nHeight: 162,
    z: 10,
    bScreenCoord: 1,
    pData: 0,
    bSetFocus: 1);
  return a1; /*0x8ff465*/
}
`

