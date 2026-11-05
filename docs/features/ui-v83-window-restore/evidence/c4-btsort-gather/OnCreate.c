/* CUIItem::OnCreate(void *) @ 0x7ce520 */

void __thiscall CUIItem::OnCreate(CUIItem *this, void *pData)
{
  void (__cdecl *v3)(VARIANTARG *); // edi
  HRESULT v4; // eax
  HRESULT v5; // eax
  StringPool *Instance; // eax
  Ztl_variant_t *ObjectA; // eax
  IUnknown *Unknown; // eax
  HRESULT v9; // eax
  void (__cdecl *v10)(VARIANTARG *); // ebx
  HRESULT v11; // eax
  HRESULT v12; // eax
  _bstr_t::Data_t *v13; // ecx
  Ztl_variant_t *v14; // eax
  IUnknown *v15; // eax
  HRESULT v16; // eax
  Ztl_bstr_t *v17; // eax
  int v18; // eax
  int v19; // eax
  CCtrlTab *v20; // edi
  CCtrlTab *p; // eax
  void (__cdecl *v22)(VARIANTARG *); // edi
  HRESULT v23; // eax
  HRESULT v24; // eax
  _bstr_t::Data_t *v25; // ecx
  Ztl_variant_t *v26; // eax
  IUnknown *v27; // eax
  HRESULT v28; // eax
  HRESULT v29; // eax
  HRESULT v30; // eax
  Ztl_variant_t *v31; // eax
  IUnknown *v32; // eax
  HRESULT v33; // eax
  IWzProperty *m_pInterface; // ebx
  void *v35; // esp
  void *v36; // esp
  char *v37; // eax
  Ztl_variant_t *v38; // eax
  IUnknown *v39; // eax
  HRESULT v40; // ebx
  char *v41; // eax
  Ztl_variant_t *v42; // eax
  int (***v43)(void); // eax
  int (**v44)(void); // ecx
  CCtrlTab *v45; // ecx
  CCtrlTab *v46; // eax
  int v47; // eax
  int v48; // eax
  CCtrlScrollBar *v49; // edi
  CCtrlScrollBar *v50; // eax
  ZRefCounted *v51; // eax
  ZRef<CCtrlOriginButton> *v52; // ebx
  CCtrlOriginButton *v53; // eax
  bool v54; // zf
  ZRef<CCtrlOriginButton> *v55; // ebx
  CCtrlOriginButton *v56; // eax
  ZRef<CCtrlOriginButton> *v57; // ebx
  CCtrlOriginButton *v58; // eax
  unsigned int (__stdcall *Release)(IUnknown *); // eax
  unsigned int (__stdcall *v60)(IUnknown *); // edx
  _com_ptr_t<_com_IIID<IWzCanvas,&_GUID_7600dc6c_9328_4bff_9624_5b0f5c01179e> > v61; // [esp-B8h] [ebp-114h]
  _com_ptr_t<_com_IIID<IWzCanvas,&_GUID_7600dc6c_9328_4bff_9624_5b0f5c01179e> > v62; // [esp-B4h] [ebp-110h]
  _bstr_t v63; // [esp-B0h] [ebp-10Ch] BYREF
  Ztl_bstr_t **v64; // [esp-ACh] [ebp-108h]
  Ztl_bstr_t *v65; // [esp-A8h] [ebp-104h] BYREF
  _bstr_t v66; // [esp-9Ch] [ebp-F8h] BYREF
  BOOL v67; // [esp-98h] [ebp-F4h]
  _DWORD v68[2]; // [esp-94h] [ebp-F0h] BYREF
  int v69; // [esp-8Ch] [ebp-E8h] BYREF
  char v70; // [esp-6Ch] [ebp-C8h] BYREF
  _bstr_t v71; // [esp-60h] [ebp-BCh] BYREF
  Ztl_variant_t *v72; // [esp-5Ch] [ebp-B8h]
  _bstr_t v73; // [esp-58h] [ebp-B4h] BYREF
  Ztl_variant_t *v74; // [esp-54h] [ebp-B0h]
  Ztl_variant_t *v75; // [esp-50h] [ebp-ACh]
  BOOL v76; // [esp-4Ch] [ebp-A8h]
  BOOL v77; // [esp-48h] [ebp-A4h]
  _bstr_t v78; // [esp-30h] [ebp-8Ch] BYREF
  Ztl_variant_t *v79; // [esp-2Ch] [ebp-88h]
  Ztl_variant_t *v80; // [esp-28h] [ebp-84h]
  BOOL v81; // [esp-24h] [ebp-80h]
  BOOL v82; // [esp-20h] [ebp-7Ch]
  Ztl_bstr_t v83; // [esp-1Ch] [ebp-78h] BYREF
  Ztl_variant_t *p_pvargDest; // [esp-18h] [ebp-74h]
  Ztl_variant_t *p_pvarg; // [esp-14h] [ebp-70h]
  BOOL v86; // [esp-10h] [ebp-6Ch]
  BOOL v87; // [esp-Ch] [ebp-68h]
  ZXString<unsigned short> v88; // [esp-8h] [ebp-64h] BYREF
  int v89; // [esp-4h] [ebp-60h]
  Ztl_variant_t Dst; // [esp+10h] [ebp-4Ch] BYREF
  Ztl_variant_t v91; // [esp+20h] [ebp-3Ch] BYREF
  Ztl_variant_t v92; // [esp+30h] [ebp-2Ch] BYREF
  Ztl_variant_t v93; // [esp+40h] [ebp-1Ch] BYREF
  int v94; // [esp+58h] [ebp-4h]
  _DWORD paramTab[2]; // [esp+5Ch] [ebp+0h] BYREF
  VARIANTARG v96; // [esp+64h] [ebp+8h] BYREF
  int v97; // [esp+74h] [ebp+18h] BYREF
  Ztl_bstr_t *v98; // [esp+78h] [ebp+1Ch]
  _com_ptr_t<_com_IIID<IWzProperty,&_GUID_986515d9_0a0b_4929_8b4f_718682177b92> > pPropNormal; // [esp+7Ch] [ebp+20h] BYREF
  Ztl_variant_t result; // [esp+80h] [ebp+24h] BYREF
  int i; // [esp+90h] [ebp+34h]
  _com_ptr_t<_com_IIID<IWzProperty,&_GUID_986515d9_0a0b_4929_8b4f_718682177b92> > pPropSelected; // [esp+94h] [ebp+38h] BYREF
  VARIANTARG pvarg; // [esp+98h] [ebp+3Ch] BYREF
  VARIANTARG pvargDest; // [esp+A8h] [ebp+4Ch] BYREF
  Ztl_bstr_t *v105; // [esp+B8h] [ebp+5Ch]
  Ztl_bstr_t *v106; // [esp+BCh] [ebp+60h] BYREF
  ZRef<CCtrlTab> v107; // [esp+C0h] [ebp+64h] BYREF

  v89 = 1;
  v88._m_pStr = (wchar_t *)this;
  v106 = (Ztl_bstr_t *)&v88;
  CUIItem::_GetBackgroundUOL(this, &v88);
  CUIWnd::OnCreate(this, pData, v88, v89);
  CLayoutMan::Init(&this->m_lm, this, 0, 0);
  v3 = *(void (__cdecl **)(VARIANTARG *))VariantInit;
  VariantInit(&pvarg);
  v4 = ZComAPI::ZComVariantCopy(&pvarg, &vtMissing);
  if ( v4 < 0 )
    _com_issue_error(v4);
  v94 = 0;
  v3(&pvargDest);
  v5 = ZComAPI::ZComVariantCopy(&pvargDest, &vtMissing);
  if ( v5 < 0 )
    _com_issue_error(v5);
  v87 = false;
  v86 = false;
  p_pvarg = (Ztl_variant_t *)&pvarg;
  p_pvargDest = (Ztl_variant_t *)&pvargDest;
  v83.m_Data = (_bstr_t::Data_t *)&pvargDest;
  v106 = &v83;
  LOBYTE(v94) = 1;
  Instance = StringPool::GetInstance();
  StringPool::GetBSTR(Instance, &v83, SP_1294_UI_BASICIMG_ITEMNO);
  LOBYTE(v94) = 2;
  if ( g_rm.m_pInterface == nullptr )
LABEL_6:
    _com_issue_error(-2147467261);
  LOBYTE(v94) = 1;
  ObjectA = IWzResMan::GetObjectA(g_rm.m_pInterface, &result, v83, p_pvargDest, p_pvarg);
  LOBYTE(v94) = 3;
  Unknown = Ztl_variant_t::GetUnknown(ObjectA, v86, v87);
  v9 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(
         &this->m_pImgFontNumber,
         Unknown);
  if ( v9 < 0 && v9 != -2147467262 )
    _com_issue_error(v9);
  v10 = *(void (__cdecl **)(VARIANTARG *))VariantClear;
  LOBYTE(v94) = 1;
  if ( result.vt == 8 )
  {
    result.vt = 0;
    if ( result.decVal.Lo32 != 0 )
      ZAPI.CoTaskMemFree((void *)(result.decVal.Lo32 - 4));
  }
  else
  {
    VariantClear(&result);
  }
  LOBYTE(v94) = 0;
  if ( pvargDest.vt == 8 )
  {
    pvargDest.vt = 0;
    if ( pvargDest.decVal.Lo32 != 0 )
      ZAPI.CoTaskMemFree((void *)(pvargDest.decVal.Lo32 - 4));
  }
  else
  {
    v10(&pvargDest);
  }
  v94 = -1;
  if ( pvarg.vt == 8 )
  {
    pvarg.vt = 0;
    if ( pvarg.decVal.Lo32 != 0 )
      ZAPI.CoTaskMemFree((void *)(pvarg.decVal.Lo32 - 4));
  }
  else
  {
    v10(&pvarg);
  }
  v3(&pvargDest);
  v11 = ZComAPI::ZComVariantCopy(&pvargDest, &vtMissing);
  if ( v11 < 0 )
    _com_issue_error(v11);
  v94 = 4;
  v3(&pvarg);
  v12 = ZComAPI::ZComVariantCopy(&pvarg, &vtMissing);
  if ( v12 < 0 )
    _com_issue_error(v12);
  v82 = false;
  v81 = false;
  v80 = (Ztl_variant_t *)&pvargDest;
  v79 = (Ztl_variant_t *)&pvarg;
  v78.m_Data = v13;
  v106 = (Ztl_bstr_t *)&v78;
  LOBYTE(v94) = 5;
  _bstr_t::_bstr_t(&v78, aUi_121);
  LOBYTE(v94) = 6;
  if ( g_rm.m_pInterface == nullptr )
    _com_issue_error(-2147467261);
  LOBYTE(v94) = 5;
  v14 = IWzResMan::GetObjectA(g_rm.m_pInterface, &result, (Ztl_bstr_t)v78.m_Data, v79, v80);
  LOBYTE(v94) = 7;
  v15 = Ztl_variant_t::GetUnknown(v14, v81, v82);
  v16 = _com_ptr_t<_com_IIID<IWzCanvas,&__s_GUID const _GUID_7600dc6c_9328_4bff_9624_5b0f5c01179e>>::_QueryInterface<IUnknown *>(
          &this->m_pCanvasDisabled,
          v15);
  if ( v16 < 0 && v16 != -2147467262 )
    _com_issue_error(v16);
  LOBYTE(v94) = 5;
  if ( result.vt == 8 )
  {
    result.vt = 0;
    if ( result.decVal.Lo32 != 0 )
      ZAPI.CoTaskMemFree((void *)(result.decVal.Lo32 - 4));
  }
  else
  {
    VariantClear(&result);
  }
  LOBYTE(v94) = 4;
  if ( pvarg.vt == 8 )
  {
    pvarg.vt = 0;
    if ( pvarg.decVal.Lo32 != 0 )
      ZAPI.CoTaskMemFree((void *)(pvarg.decVal.Lo32 - 4));
  }
  else
  {
    VariantClear(&pvarg);
  }
  v94 = -1;
  if ( pvargDest.vt == 8 )
  {
    pvargDest.vt = 0;
    if ( pvargDest.decVal.Lo32 != 0 )
      ZAPI.CoTaskMemFree((void *)(pvargDest.decVal.Lo32 - 4));
  }
  else
  {
    VariantClear(&pvargDest);
  }
  paramTab[0] = 0;
  paramTab[1] = 1;
  v17 = (Ztl_bstr_t *)ZAllocEx<ZAllocAnonSelector>::Alloc(&ZAllocEx<ZAllocAnonSelector>::_s_alloc, 0x70u);
  v106 = v17;
  v94 = 8;
  if ( v17 != nullptr )
    CCtrlTab::CCtrlTab((CCtrlTab *)v17);
  else
    v18 = 0;
  v94 = -1;
  if ( v18 != 0 && (v19 = v18 + 8) != 0 )
  {
    v20 = (CCtrlTab *)(v19 - 8);
    if ( v19 != 8 )
      InterlockedIncrement(&v20->_m_nRef);
  }
  else
  {
    v20 = nullptr;
  }
  p = this->m_pTab.p;
  this->m_pTab.p = v20;
  v107.p = p;
  if ( p != nullptr )
    ZRef<CCtrlTab>::_ReleaseRaw(&v107, nullptr);
  ((void (__thiscall *)(CCtrlTab *, CUIItem *, int, int, int, int, int, int, _DWORD *))this->m_pTab.p->vfptr[10].Update)(
    this->m_pTab.p,
    this,
    2000,
    8,
    9,
    26,
    157,
    19,
    paramTab);
  v22 = *(void (__cdecl **)(VARIANTARG *))VariantInit;
  VariantInit(&pvargDest);
  v23 = ZComAPI::ZComVariantCopy(&pvargDest, &vtMissing);
  if ( v23 < 0 )
    _com_issue_error(v23);
  v94 = 9;
  v22(&pvarg);
  v24 = ZComAPI::ZComVariantCopy(&pvarg, &vtMissing);
  if ( v24 < 0 )
    _com_issue_error(v24);
  v77 = false;
  v76 = false;
  v75 = (Ztl_variant_t *)&pvargDest;
  v74 = (Ztl_variant_t *)&pvarg;
  v73.m_Data = v25;
  v106 = (Ztl_bstr_t *)&v73;
  LOBYTE(v94) = 10;
  _bstr_t::_bstr_t(&v73, aUi_307);
  LOBYTE(v94) = 11;
  if ( g_rm.m_pInterface == nullptr )
    _com_issue_error(-2147467261);
  LOBYTE(v94) = 10;
  v26 = IWzResMan::GetObjectA(g_rm.m_pInterface, &v91, (Ztl_bstr_t)v73.m_Data, v74, v75);
  LOBYTE(v94) = 12;
  v27 = Ztl_variant_t::GetUnknown(v26, v76, v77);
  pPropNormal.m_pInterface = nullptr;
  v28 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(
          &pPropNormal,
          v27);
  if ( v28 < 0 && v28 != -2147467262 )
    _com_issue_error(v28);
  LOBYTE(v94) = 14;
  if ( v91.vt == 8 )
  {
    v91.vt = 0;
    if ( v91.decVal.Lo32 != 0 )
      ZAPI.CoTaskMemFree((void *)(v91.decVal.Lo32 - 4));
  }
  else
  {
    VariantClear(&v91);
  }
  LOBYTE(v94) = 15;
  if ( pvarg.vt == 8 )
  {
    pvarg.vt = 0;
    if ( pvarg.decVal.Lo32 != 0 )
      ZAPI.CoTaskMemFree((void *)(pvarg.decVal.Lo32 - 4));
  }
  else
  {
    VariantClear(&pvarg);
  }
  LOBYTE(v94) = 16;
  if ( pvargDest.vt == 8 )
  {
    pvargDest.vt = 0;
    if ( pvargDest.decVal.Lo32 != 0 )
      ZAPI.CoTaskMemFree((void *)(pvargDest.decVal.Lo32 - 4));
  }
  else
  {
    VariantClear(&pvargDest);
  }
  v22(&result);
  v29 = ZComAPI::ZComVariantCopy(&result, &vtMissing);
  if ( v29 < 0 )
    _com_issue_error(v29);
  LOBYTE(v94) = 17;
  v22(&v96);
  v30 = ZComAPI::ZComVariantCopy(&v96, &vtMissing);
  if ( v30 < 0 )
    _com_issue_error(v30);
  v75 = nullptr;
  v74 = nullptr;
  v73.m_Data = (_bstr_t::Data_t *)&result;
  v72 = (Ztl_variant_t *)&v96;
  v71.m_Data = (_bstr_t::Data_t *)&v96;
  v106 = (Ztl_bstr_t *)&v71;
  LOBYTE(v94) = 18;
  _bstr_t::_bstr_t(&v71, aUi_35);
  LOBYTE(v94) = 19;
  if ( g_rm.m_pInterface == nullptr )
    _com_issue_error(-2147467261);
  LOBYTE(v94) = 18;
  v31 = IWzResMan::GetObjectA(g_rm.m_pInterface, &v93, (Ztl_bstr_t)v71.m_Data, v72, (Ztl_variant_t *)v73.m_Data);
  LOBYTE(v94) = 20;
  v32 = Ztl_variant_t::GetUnknown(v31, (bool)v74, (bool)v75);
  pPropSelected.m_pInterface = nullptr;
  v33 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(
          &pPropSelected,
          v32);
  if ( v33 < 0 && v33 != -2147467262 )
    _com_issue_error(v33);
  LOBYTE(v94) = 22;
  if ( v93.vt == 8 )
  {
    v93.vt = 0;
    if ( v93.decVal.Lo32 != 0 )
      ZAPI.CoTaskMemFree((void *)(v93.decVal.Lo32 - 4));
  }
  else
  {
    VariantClear(&v93);
  }
  LOBYTE(v94) = 23;
  if ( v96.vt == 8 )
  {
    v96.vt = 0;
    if ( v96.decVal.Lo32 != 0 )
      ZAPI.CoTaskMemFree((void *)(v96.decVal.Lo32 - 4));
  }
  else
  {
    VariantClear(&v96);
  }
  LOBYTE(v94) = 24;
  if ( result.vt == 8 )
  {
    result.vt = 0;
    if ( result.decVal.Lo32 != 0 )
      ZAPI.CoTaskMemFree((void *)(result.decVal.Lo32 - 4));
  }
  else
  {
    VariantClear(&result);
  }
  for ( i = 0; i < 5; ++i )
  {
    m_pInterface = pPropSelected.m_pInterface;
    v35 = alloca(32);
    v36 = alloca(32);
    v105 = (Ztl_bstr_t *)&v69;
    v68[1] = 1;
    v68[0] = 0;
    v67 = false;
    v37 = itoa(i, &v70, 10);
    v98 = (Ztl_bstr_t *)&v66;
    _bstr_t::_bstr_t(&v66, v37);
    LOBYTE(v94) = 25;
    if ( m_pInterface == nullptr )
      goto LABEL_6;
    LOBYTE(v94) = 24;
    v38 = IWzProperty::Getitem(m_pInterface, &Dst, (Ztl_bstr_t)v66.m_Data);
    LOBYTE(v94) = 26;
    v39 = Ztl_variant_t::GetUnknown(v38, v67, v68[0]);
    v107.p = (CCtrlTab *)v68;
    v68[0] = 0;
    if ( v39 != nullptr )
    {
      v40 = ((int (__cdecl *)(IUnknown *, GUID *, int *))v39->vfptr->QueryInterface)(
              v39,
              &_GUID_7600dc6c_9328_4bff_9624_5b0f5c01179e,
              &v97);
      v98 = v40 < 0 ? nullptr : (Ztl_bstr_t *)v97;
      if ( v68[0] != 0 )
        (*(void (__cdecl **)(_DWORD))(*(_DWORD *)v68[0] + 8))(v68[0]);
      v68[0] = v98;
      if ( v40 < 0 && v40 != -2147467262 )
        goto LABEL_120;
    }
    v65 = nullptr;
    v64 = nullptr;
    LOBYTE(v94) = 27;
    v41 = itoa(i, (char *)v105, 10);
    v105 = (Ztl_bstr_t *)&v63;
    _bstr_t::_bstr_t(&v63, v41);
    LOBYTE(v94) = 28;
    if ( pPropNormal.m_pInterface == nullptr )
      goto LABEL_6;
    LOBYTE(v94) = 27;
    v42 = IWzProperty::Getitem(pPropNormal.m_pInterface, &v92, (Ztl_bstr_t)v63.m_Data);
    LOBYTE(v94) = 29;
    v43 = (int (***)(void))Ztl_variant_t::GetUnknown(v42, (bool)v64, (bool)v65);
    v105 = (Ztl_bstr_t *)&v65;
    v65 = nullptr;
    if ( v43 != nullptr )
    {
      v44 = *v43;
      v64 = &v106;
      v63.m_Data = (_bstr_t::Data_t *)&_GUID_7600dc6c_9328_4bff_9624_5b0f5c01179e;
      v62.m_pInterface = (IWzCanvas *)v43;
      v40 = (*v44)();
      v105 = v40 < 0 ? nullptr : v106;
      if ( v65 != nullptr )
      {
        v61.m_pInterface = (IWzCanvas *)v65;
        ((void (*)(void))v65->m_Data->m_RefCount)();
      }
      v65 = v105;
      if ( v40 < 0 && v40 != -2147467262 )
LABEL_120:
        _com_issue_error(v40);
    }
    v45 = this->m_pTab.p;
    LOBYTE(v94) = 30;
    CCtrlTab::AddItem_Canvas(v45, v61, v62, (int)v63.m_Data);
    LOBYTE(v94) = 26;
    if ( v92.vt == 8 )
    {
      v92.vt = 0;
      if ( v92.decVal.Lo32 != 0 )
        ZAPI.CoTaskMemFree((void *)(v92.decVal.Lo32 - 4));
    }
    else
    {
      VariantClear(&v92);
    }
    LOBYTE(v94) = 24;
    if ( Dst.vt == 8 )
    {
      Dst.vt = 0;
      if ( Dst.decVal.Lo32 != 0 )
        ZAPI.CoTaskMemFree((void *)(Dst.decVal.Lo32 - 4));
    }
    else
    {
      VariantClear(&Dst);
    }
  }
  if ( this->m_bExtended == 0 )
  {
    v46 = (CCtrlTab *)ZAllocEx<ZAllocAnonSelector>::Alloc(&ZAllocEx<ZAllocAnonSelector>::_s_alloc, 0x74u);
    v107.p = v46;
    LOBYTE(v94) = 31;
    if ( v46 != nullptr )
      CCtrlScrollBar::CCtrlScrollBar((CCtrlScrollBar *)v46);
    else
      v47 = 0;
    LOBYTE(v94) = 24;
    if ( v47 != 0 && (v48 = v47 + 8) != 0 )
    {
      v49 = (CCtrlScrollBar *)(v48 - 8);
      if ( v48 != 8 )
        InterlockedIncrement(&v49->_m_nRef);
    }
    else
    {
      v49 = nullptr;
    }
    v50 = this->m_pSBItem.p;
    this->m_pSBItem.p = v49;
    v107.p = (CCtrlTab *)v50;
    if ( v50 != nullptr )
      ZRef<CCtrlScrollBar>::_ReleaseRaw((ZRef<CCtrlScrollBar> *)&v107, nullptr);
    ((void (__thiscall *)(CCtrlScrollBar *, CUIItem *, int, int, int, int, int, int, _DWORD))this->m_pSBItem.p->vfptr[10].Update)(
      this->m_pSBItem.p,
      this,
      2001,
      1,
      8,
      152,
      51,
      207,
      0);
    this->m_pSBItem.p->m_nWheelRange = 142;
  }
  CLayoutMan::AddButton(&this->m_lm, (ZRef<CCtrlOriginButton> *)&v107, aUi_565, 0x7D2u, 0, 0, nullptr);
  if ( v107.p != nullptr && InterlockedDecrement(&v107.p->_m_nRef) == 0 )
  {
    v51 = v107.p != nullptr ? &v107.p->ZRefCounted : nullptr;
    InterlockedIncrement(&v51->_m_nRef);
    if ( v107.p != nullptr && v107.p != (CCtrlTab *)-8 )
      v107.p->vfptr->__vecDelDtor(&v107.p->ZRefCounted, 1);
  }
  if ( this->m_bExtended != 0 )
  {
    v52 = CLayoutMan::AddButton(&this->m_lm, (ZRef<CCtrlOriginButton> *)&v107, aUi_327, 0x7D6u, 0, 0, nullptr);
    v53 = v52->p;
    LOBYTE(v94) = 32;
    if ( v53 != nullptr )
      InterlockedIncrement(&v53->_m_nRef);
    if ( this->m_pBtExtend.p != nullptr )
    {
      ZRef<CCtrlOriginButton>::_ReleaseRaw(&this->m_pBtExtend, nullptr);
      this->m_pBtExtend.p = nullptr;
    }
    v54 = v107.p == nullptr;
    this->m_pBtExtend.p = v52->p;
    LOBYTE(v94) = 24;
    if ( !v54 )
      ZRef<CCtrlOriginButton>::_ReleaseRaw((ZRef<CCtrlOriginButton> *)&v107, nullptr);
    v55 = CLayoutMan::AddButton(&this->m_lm, (ZRef<CCtrlOriginButton> *)&v107, aUi_605, 0x7D7u, 0, 0, nullptr);
    v56 = v55->p;
    LOBYTE(v94) = 33;
    if ( v56 != nullptr )
      InterlockedIncrement(&v56->_m_nRef);
    if ( this->m_pBtCashShop.p != nullptr )
    {
      ZRef<CCtrlOriginButton>::_ReleaseRaw(&this->m_pBtCashShop, nullptr);
      this->m_pBtCashShop.p = nullptr;
    }
    v54 = v107.p == nullptr;
    this->m_pBtCashShop.p = v55->p;
    LOBYTE(v94) = 24;
    if ( !v54 )
      ZRef<CCtrlOriginButton>::_ReleaseRaw((ZRef<CCtrlOriginButton> *)&v107, nullptr);
  }
  else
  {
    v57 = CLayoutMan::AddButton(&this->m_lm, (ZRef<CCtrlOriginButton> *)&v107, aUi_346, 0x7D5u, 0, 0, nullptr);
    v58 = v57->p;
    LOBYTE(v94) = 34;
    if ( v58 != nullptr )
      InterlockedIncrement(&v58->_m_nRef);
    if ( this->m_pBtExtend.p != nullptr )
    {
      ZRef<CCtrlOriginButton>::_ReleaseRaw(&this->m_pBtExtend, nullptr);
      this->m_pBtExtend.p = nullptr;
    }
    v54 = v107.p == nullptr;
    this->m_pBtExtend.p = v57->p;
    LOBYTE(v94) = 24;
    if ( !v54 )
    {
      ZRef<CCtrlOriginButton>::_ReleaseRaw((ZRef<CCtrlOriginButton> *)&v107, nullptr);
      v107.p = nullptr;
    }
  }
  this->m_nFirstPosition = 1;
  CUIItem::SetTryToReleaseItem(this, 0, -1);
  CCtrlTab::SetTab(this->m_pTab.p, this->m_nOption);
  this->m_anSortBtState[0] = 0;
  this->m_anSortBtState[1] = 0;
  this->m_anSortBtState[2] = 0;
  this->m_anSortBtState[3] = 0;
  this->m_anSortBtState[4] = 0;
  this->m_anSortBtState[5] = 0;
  CUIItem::SetArrangeButton(this, this->m_nItemTI, this->m_anSortBtState[this->m_nItemTI]);
  *ZArray<CUIWnd *>::InsertBefore((ZArray<CUIWnd *> *)TSingleton<CWvsContext>::ms_pInstance._m_pStr + 4136, -1) = this;
  CUIItem::SetLastestGetItemEffect(this);
  Release = pPropSelected.m_pInterface->vfptr->Release;
  LOBYTE(v94) = 16;
  ((void (__cdecl *)(IWzProperty *))Release)(pPropSelected.m_pInterface);
  v60 = pPropNormal.m_pInterface->vfptr->Release;
  v94 = -1;
  ((void (__cdecl *)(IWzProperty *))v60)(pPropNormal.m_pInterface);
}

