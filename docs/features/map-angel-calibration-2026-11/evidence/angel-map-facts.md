# Angel 083 MAP 面反编译语义抽取（观察层）

来源：`evidence/angel-map-decompile.json`（全量函数逐字 Hex-Rays 伪代码，来自 Angel 13339 IDB）。
本文件由 `tools/ida_angel_map_facts.py` 自动生成，只做字符串抽取与归类，不含推断。

## 1. 逐函数关键信号（opcode / Encode / Decode / 分支 / SP_ 节点 / 数值调用）


### <free>::0CUIMiniMap  @ 0x854b8c  (28854 B)

- SP_ 节点：`SP_1389_NPC`, `SP_1473_PORTAL`, `SP_1712_MAP_MAPHELPERIMG`, `SP_4397_MINIMAP`, `SP_4398_USER`, `SP_4399_ANOTHER`, `SP_4400_FRIEND`, `SP_4401_GUILD`, `SP_4402_GUILDMASTER`, `SP_4403_MATCH`, `SP_4404_PARTYMASTER`, `SP_4405_STARTNPC`, `SP_4406_ENDNPC`, `SP_4407_USERTRADER`…
  - `_DWORD *__fastcall CUIMiniMap::CUIMiniMap(_DWORD *a1, int a2, int a3)`
  - `CUIWnd::CUIWnd(this: a1, a2: 4, a3: 0, a4: -1, a5: -1, a6: 0, a7: 0, a8: 0);`
  - `sub_5E29FB(a1: p_pvarg);`
  - `sub_50B028(a1: p_pvarg);`
  - `sub_85BF66(a1: p_pvarg);`
  - `LOBYTE(v121) = 47;`
  - `VariantInit(pvarg: p_pvarg);`
  - `v4 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v4 < 0 )`
  - `VariantInit(pvarg: &pvargDest);`
  - `v5 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v5 < 0 )`
  - `v8 = sub_406472();`
  - `v9 = sub_402680(a1: v8);`
  - …（截断 14 行）

### <free>::0CWorldMapDlg  @ 0x9e9823  (700 B)

  - `CWorldMapDlg *__thiscall CWorldMapDlg::CWorldMapDlg(CWorldMapDlg *this)`
  - `CDialog::CDialog(this);`
  - `*(this + 31) = 0;`
  - `CUIToolTip::CUIToolTip(this: this + 34);`
  - `*(this + 361) = -1;`
  - `*(this + 1) = &off_B3F19C;`
  - `*(this + 2) = &off_B3F198;`

### <free>::1CWorldMapDlg  @ 0x9e98e6  (835 B)

  - `_DWORD *__thiscall CWorldMapDlg::~CWorldMapDlg(CWorldMapDlg *this)`
  - `*(this - 2) = &off_B3F1E8;`
  - `*(this - 1) = &off_B3F19C;`
  - `sub_9EE90F(a1: this + 1460);`
  - `sub_9EE882(a1: this + 1456);`
  - `sub_41E4EB(a1: this + 1452);`
  - `v2 = *(this + 362);`
  - `if ( v2 != 0 )`
  - `(*(*v2 + 8))(a1: v2);`
  - `v3 = *(this + 361);`
  - `if ( v3 != 0 )`
  - `(*(*v3 + 8))(a1: v3);`
  - `sub_9EE7C0(a1: this + 1432);`
  - `CUIToolTip::~CUIToolTip(a1: this + 128);`
  - …（截断 14 行）

### <free>::CatchIt  @ 0xa64fdd  (1009 B)

  - `void __cdecl CatchIt(`
  - `if ( a7 != 0 )`
  - `BuildCatchObject(a1: ExceptionRecord, a2, a3: a6, a4: a7);`
  - `if ( TargetFrame != 0 )`
  - `_UnwindNestedFrames(TargetFrame, ExceptionRecord);`
  - `_UnwindNestedFrames(TargetFrame: a2, ExceptionRecord);`
  - `__FrameUnwindToState(a1: a2, a2: a4, a3: a5, a4: a8->tryLow);`
  - `v10 = CallCatchBlock(a1: ExceptionRecord, a2, a3, a4: a5, a5: a6->addressOfHandler, a6: a9, a7: 0x100u);`
  - `if ( v10 != 0 )`
  - `_JumpToContinuation(a1: v10, a2);`

### <free>::_GCWorldMapDlg  @ 0x9e98c2  (281 B)

  - `char *__thiscall CWorldMapDlg::`scalar deleting destructor'(CWorldMapDlg *this, char a2)`
  - `CWorldMapDlg::~CWorldMapDlg(a1: this);`
  - `if ( (a2 & 1) != 0 )`
  - `ZAllocEx<ZAllocAnonSelector>::Free(a1: this - 8);`

### <free>::_GetRangeOfTrysToCheck  @ 0xa60a2a  (1255 B)

  - `TryBlockMapEntry *__cdecl _GetRangeOfTrysToCheck(`
  - `if ( a2 >= 0 )`
  - `if ( nTryBlocks == -1 )`
  - `sub_A6564D();`
  - `if ( pTryBlockMap[--nTryBlocks].tryHigh < a3 && a3 <= pTryBlockMap[nTryBlocks].catchHigh || nTryBlocks == -1 )`
  - `while ( a2 >= 0 );`
  - `if ( v8 > a1->nTryBlocks || v9 > v8 )`

### <free>::_g_apfnPCOMAPIs  @ 0x40387e  (330 B)

  - `_DWORD *__thiscall _g_apfnPCOMAPIs(_DWORD *this, int a2, int a3, char a4)`
  - `*(this + 3) = 0;`
  - `*(this + 1) = a2;`
  - `*(this + 2) = a3;`
  - `if ( a3 != 0 && a4 != 0 )`
  - `(*(*a3 + 4))(a1: a3);`

### <free>::is_able_to_map_macrosys_skill  @ 0x4fb292  (520 B)

  - `BOOL __cdecl is_able_to_map_macrosys_skill(UnwindMapEntry *nSkillID)`
  - `&& !is_keydown_skill(a1: nSkillID)`
  - `&& nSkillID != (&stru_B8A588.toState + 3)`
  - `&& nSkillID != (&loc_A9634B + 2)`

### ?$ZMap::GetAt  @ 0x447ef0  (572 B)

  - `_DWORD *__thiscall ZMap<unsigned long,__POSITION *,unsigned long>::GetAt(_DWORD *this, unsigned int *a2, _DWORD *a3)`
  - `if ( *(this + 1) == 0 )`
  - `v3 = *(*(this + 1) + 4 * (_lrotr(a1: *a2, a2: 5) % *(this + 2)));`
  - `if ( v3 == 0 )`
  - `while ( v3[2] != *a2 )`
  - `if ( a3 != 0 )`

### ?$ZMap::GetAt  @ 0x4a9ce4  (580 B)

  - `int __thiscall ZMap<long,ZXString<char>,long>::GetAt(_DWORD *this, unsigned int *a2, int a3)`
  - `if ( *(this + 1) == 0 )`
  - `v3 = *(*(this + 1) + 4 * (_lrotr(a1: *a2, a2: 5) % *(this + 2)));`
  - `if ( v3 == 0 )`
  - `while ( *(v3 + 8) != *a2 )`
  - `v3 = *(v3 + 4);`
  - `if ( a3 != 0 )`
  - `ZXString<char>::operator=(a1: v3 + 12);`

### ?$ZMap::GetAt  @ 0x4e86a0  (546 B)

  - `_DWORD *__thiscall ZMap<long,long,long>::GetAt(_DWORD *this, unsigned int *a2, _DWORD *a3)`
  - `if ( *(this + 1) == 0 )`
  - `v3 = *(*(this + 1) + 4 * (_lrotr(a1: *a2, a2: 5) % *(this + 2)));`
  - `if ( v3 == 0 )`
  - `while ( v3[2] != *a2 )`
  - `if ( a3 != 0 )`

### ?$ZMap::GetAt  @ 0x50dea0  (590 B)

  - `if ( *(this + 1) == 0 )`
  - `v3 = *(*(this + 1) + 4 * (_lrotr(a1: *a2, a2: 5) % *(this + 2)));`
  - `if ( v3 == 0 )`
  - `while ( *(v3 + 8) != *a2 )`
  - `v3 = *(v3 + 4);`
  - `if ( a3 != 0 )`
  - `sub_508A2E(a1: v3 + 12);`

### ?$ZMap::GetAt  @ 0x5dd4ae  (577 B)

  - `if ( *(this + 1) == 0 )`
  - `v3 = *(*(this + 1) + 4 * (_lrotr(a1: *a2, a2: 5) % *(this + 2)));`
  - `if ( v3 == 0 )`
  - `while ( *(v3 + 8) != *a2 )`
  - `v3 = *(v3 + 4);`
  - `if ( a3 != 0 )`
  - `sub_5DBC65(a1: v3 + 12);`

### ?$ZMap::GetAt  @ 0x6864e4  (571 B)

  - `if ( *(this + 1) == 0 )`
  - `v3 = *(*(this + 1) + 4 * (_lrotr(a1: *a2, a2: 5) % *(this + 2)));`
  - `if ( v3 == 0 )`
  - `while ( *(v3 + 8) != *a2 )`
  - `v3 = *(v3 + 4);`
  - `if ( a3 != 0 )`
  - `sub_687779(a1: v3 + 12);`

### ?$ZMap::GetAt  @ 0x725a4e  (591 B)

  - `int __thiscall ZMap<unsigned long,ZArray<unsigned short>,unsigned long>::GetAt(_DWORD *this, unsigned int *a2, int a3)`
  - `if ( *(this + 1) == 0 )`
  - `v3 = *(*(this + 1) + 4 * (_lrotr(a1: *a2, a2: 5) % *(this + 2)));`
  - `if ( v3 == 0 )`
  - `while ( *(v3 + 8) != *a2 )`
  - `v3 = *(v3 + 4);`
  - `if ( a3 != 0 )`
  - `sub_67364B(a1: v3 + 12);`

### ?$ZMap::GetAt  @ 0x72631a  (564 B)

  - `int __thiscall ZMap<unsigned short,int,unsigned short>::GetAt(_DWORD *this, _WORD *a2, _DWORD *a3)`
  - `if ( *(this + 1) == 0 )`
  - `v3 = *(*(this + 1) + 4 * (_lrotr(a1: *a2, a2: 5) % *(this + 2)));`
  - `if ( v3 == 0 )`
  - `while ( *(v3 + 8) != *a2 )`
  - `v3 = *(v3 + 4);`
  - `if ( a3 != 0 )`
  - `*a3 = *(v3 + 12);`

### ?$ZMap::Insert  @ 0x4a9ccb  (147 B)

  - `int __stdcall ZMap<long,ZXString<char>,long>::Insert(int a1, int a2)`
  - `return ZMap<long,ZXString<char>,long>::Insert(a1, a2) + 12;`

### ?$ZMap::Insert  @ 0x4a9d38  (1566 B)

  - `_DWORD *__thiscall ZMap<long,ZXString<char>,long>::Insert(int *this, unsigned int *a2, int a3)`
  - `if ( *(this + 1) != 0 )`
  - `if ( *(this + 3) <= *(this + 5) )`
  - `v4 = 2 * *(this + 2) + 1;`
  - `v4 = *(this + 2);`
  - `if ( v4 != 0 )`
  - `sub_4AAA76(a1: v4, a2: 0);`
  - `v5 = _lrotr(a1: *a2, a2: 5) % *(this + 2);`
  - `v6 = *(this + 1);`
  - `v7 = *(v6 + 4 * v5);`
  - `v8 = (v6 + 4 * v5);`
  - `if ( v7 != 0 )`
  - `while ( *(v7 + 8) != *a2 )`
  - `v7 = *(v7 + 4);`
  - …（截断 14 行）

### ?$ZMap::Insert  @ 0x4e86f0  (1396 B)

  - `_DWORD *__thiscall ZMap<long,long,long>::Insert(int *this, unsigned int *a2, _DWORD *a3)`
  - `if ( *(this + 1) != 0 )`
  - `if ( *(this + 3) <= *(this + 5) )`
  - `v4 = 2 * *(this + 2) + 1;`
  - `v4 = *(this + 2);`
  - `if ( v4 != 0 )`
  - `sub_4EBB43(a1: v4, a2: 0);`
  - `v5 = (*(this + 1) + 4 * (_lrotr(a1: *a2, a2: 5) % *(this + 2)));`
  - `if ( *v5 != 0 )`
  - `while ( result[2] != *a2 )`
  - `if ( result == 0 )`
  - `if ( a3 != 0 )`
  - `++*(this + 3);`
  - `result = sub_4EC563(a1: dword_BF6C58);`
  - …（截断 14 行）

### ?$ZMap::Insert  @ 0x511138  (1421 B)

  - `_DWORD *__thiscall ZMap<unsigned long,__POSITION *,unsigned long>::Insert(int *this, unsigned int *a2, _DWORD *a3)`
  - `if ( *(this + 1) != 0 )`
  - `if ( *(this + 3) <= *(this + 5) )`
  - `v4 = 2 * *(this + 2) + 1;`
  - `v4 = *(this + 2);`
  - `if ( v4 != 0 )`
  - `sub_512437(a1: v4, a2: 0);`
  - `v5 = (*(this + 1) + 4 * (_lrotr(a1: *a2, a2: 5) % *(this + 2)));`
  - `if ( *v5 != 0 )`
  - `while ( result[2] != *a2 )`
  - `if ( result == 0 )`
  - `if ( a3 != 0 )`
  - `++*(this + 3);`
  - `result = sub_512506(a1: dword_BF6970);`
  - …（截断 14 行）

### ?$ZMap::Insert  @ 0x6736a8  (1356 B)

  - `int __thiscall ZMap<long,ZMap<long,long,long>,long>::Insert(int *this, unsigned int *a2, int a3)`
  - `if ( *(this + 1) != 0 )`
  - `if ( *(this + 3) <= *(this + 5) )`
  - `v4 = 2 * *(this + 2) + 1;`
  - `v4 = *(this + 2);`
  - `if ( v4 != 0 )`
  - `ZMap<long,ZMap<long,long,long>,long>::ResizeHashTable(a1: v4, a2: 0);`
  - `v5 = _lrotr(a1: *a2, a2: 5) % *(this + 2);`
  - `v6 = *(this + 1);`
  - `v7 = *(v6 + 4 * v5);`
  - `v8 = (v6 + 4 * v5);`
  - `if ( v7 != 0 )`
  - `while ( *(v7 + 8) != *a2 )`
  - `v7 = *(v7 + 4);`
  - …（截断 14 行）

### ?$ZMap::Insert  @ 0x6d9d15  (1412 B)

  - `_DWORD *__thiscall ZMap<unsigned long,int,unsigned long>::Insert(int *this, unsigned int *a2, _DWORD *a3)`
  - `if ( *(this + 1) != 0 )`
  - `if ( *(this + 3) <= *(this + 5) )`
  - `v4 = 2 * *(this + 2) + 1;`
  - `v4 = *(this + 2);`
  - `if ( v4 != 0 )`
  - `sub_6DB2F7(a1: v4, a2: 0);`
  - `v5 = (*(this + 1) + 4 * (_lrotr(a1: *a2, a2: 5) % *(this + 2)));`
  - `if ( *v5 != 0 )`
  - `while ( result[2] != *a2 )`
  - `if ( result == 0 )`
  - `if ( a3 != 0 )`
  - `++*(this + 3);`
  - `result = sub_6DB3C6(a1: dword_BF68F8);`
  - …（截断 14 行）

### ?$ZMap::Insert  @ 0x72636d  (1418 B)

  - `int __thiscall ZMap<unsigned short,int,unsigned short>::Insert(int *this, _WORD *a2, _DWORD *a3)`
  - `if ( *(this + 1) != 0 )`
  - `if ( *(this + 3) <= *(this + 5) )`
  - `v4 = 2 * *(this + 2) + 1;`
  - `v4 = *(this + 2);`
  - `if ( v4 != 0 )`
  - `sub_7293D8(a1: v4, a2: 0);`
  - `v5 = (*(this + 1) + 4 * (_lrotr(a1: *a2, a2: 5) % *(this + 2)));`
  - `if ( *v5 != 0 )`
  - `while ( *(result + 8) != *a2 )`
  - `result = *(result + 4);`
  - `if ( result == 0 )`
  - `if ( a3 != 0 )`
  - `*(result + 12) = *a3;`
  - …（截断 14 行）

### ?$ZMap::RemoveAll  @ 0x6d9de3  (828 B)

  - `int __thiscall ZMap<unsigned long,int,unsigned long>::RemoveAll(int **this)`
  - `v2 = *(this + 1);`
  - `if ( v2 != 0 )`
  - `v3 = &v2[*(this + 2)];`
  - `while ( v2 < v3 )`
  - `if ( v4 != 0 )`
  - `v5 = *(v4 + 4);`
  - `(**v4)(a1: v4, a2: 1);`
  - `while ( v5 != 0 );`
  - `result = ZAllocEx<ZAllocAnonSelector>::Free(a1: *(this + 1));`
  - `*(this + 1) = 0;`

### ?$ZMap::RemoveAll  @ 0x726440  (830 B)

  - `int __thiscall ZMap<unsigned short,int,unsigned short>::RemoveAll(int **this)`
  - `v2 = *(this + 1);`
  - `if ( v2 != 0 )`
  - `v3 = &v2[*(this + 2)];`
  - `while ( v2 < v3 )`
  - `if ( v4 != 0 )`
  - `v5 = *(v4 + 4);`
  - `(**v4)(a1: v4, a2: 1);`
  - `while ( v5 != 0 );`
  - `result = ZAllocEx<ZAllocAnonSelector>::Free(a1: *(this + 1));`
  - `*(this + 1) = 0;`

### ?$ZMap::RemoveKey  @ 0x511206  (966 B)

  - `int __thiscall ZMap<unsigned long,__POSITION *,unsigned long>::RemoveKey(_DWORD *this, unsigned int *a2)`
  - `if ( *(this + 1) == 0 )`
  - `v3 = _lrotr(a1: *a2, a2: 5) % *(this + 2);`
  - `v4 = *(this + 1);`
  - `v5 = *(v4 + 4 * v3);`
  - `v6 = (v4 + 4 * v3);`
  - `if ( v5 == 0 )`
  - `if ( *(v5 + 8) != *a2 )`
  - `for ( i = *(v5 + 4); i != 0; i = *(i + 4) )`
  - `if ( *(i + 8) == *a2 )`
  - `*(v5 + 4) = *(i + 4);`
  - `*v6 = *(v5 + 4);`
  - `(**v5)(a1: v5, a2: 1);`
  - `--*(this + 3);`
  - …（截断 14 行）

### ?$ZMap::RemoveKey  @ 0x672948  (940 B)

  - `int __thiscall ZMap<long,long,long>::RemoveKey(_DWORD *this, unsigned int *a2)`
  - `if ( *(this + 1) == 0 )`
  - `v3 = _lrotr(a1: *a2, a2: 5) % *(this + 2);`
  - `v4 = *(this + 1);`
  - `v5 = *(v4 + 4 * v3);`
  - `v6 = (v4 + 4 * v3);`
  - `if ( v5 == 0 )`
  - `if ( *(v5 + 8) != *a2 )`
  - `for ( i = *(v5 + 4); i != 0; i = *(i + 4) )`
  - `if ( *(i + 8) == *a2 )`
  - `*(v5 + 4) = *(i + 4);`
  - `*v6 = *(v5 + 4);`
  - `(**v5)(a1: v5, a2: 1);`
  - `--*(this + 3);`
  - …（截断 14 行）

### ?$ZMap::ResizeHashTable  @ 0x675a77  (1448 B)

  - `unsigned int __thiscall ZMap<long,ZMap<long,long,long>,long>::ResizeHashTable(`
  - `result = *(this + 2);`
  - `if ( result != a2 || *(this + 1) == 0 )`
  - `v5 = *(this + 1);`
  - `*(this + 2) = a2;`
  - `sub_675B22(a1: a3);`
  - `v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 4 * a2);`
  - `*(this + 1) = v6;`
  - `result = memset(a1: v6, a2: 0, a3: 4 * a2);`
  - `if ( v5 != 0 )`
  - `while ( v5 < v12 )`
  - `if ( v7 != 0 )`
  - `v8 = _lrotr(a1: *(v7 + 8), a2: 5);`
  - `v9 = *(v7 + 4);`
  - …（截断 14 行）

### ?$ZRef::_Alloc  @ 0x4028aa  (442 B)

  - `if ( *(this + 1) != 0 )`
  - `sub_402D6B(a1: 0);`
  - `*(this + 1) = 0;`
  - `v2 = sub_403C72(a1: 0);`
  - `*(v2 + 4) = 1;`
  - `*(this + 1) = v3 != 0 ? result : 0;`

### CActionFrame::ExtractMap  @ 0x4016a5  (5131 B)

  - `int __cdecl CActionFrame::ExtractMap(int a1, void *Unknown)`
  - `IWzCanvas::GetProperty(this: Unknown, a2: &v30);`
  - `LOBYTE(v31) = 2;`
  - `if ( v30 == 0 )`
  - `*(a1 + 4) = v22;`
  - `sub_4028DD(a1);`
  - `sub_4027E9(a1: &v30);`
  - `sub_4028FA(a1: v21);`
  - `sub_405210(a1: &dword_BE7878);`
  - `sub_40263B(a1: &v30);`
  - `IWzProperty::GetItem(&pvargDest, a2: v16);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v17, a2: v18);`
  - `v4 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v4 < 0 && v4 != -2147467262 )`
  - …（截断 14 行）

### CActionFrame::GetSMapper  @ 0x774b81  (376 B)

  - `_DWORD *__cdecl CActionFrame::GetSMapper(_DWORD *a1)`
  - `if ( dword_BE78A0 == 0 )`
  - `CActionFrame::LoadMappers();`
  - `if ( !v2 )`
  - `(*(*v1 + 4))(a1: v1);`

### CActionFrame::GetZMapper  @ 0x774b51  (376 B)

  - `_DWORD *__cdecl CActionFrame::GetZMapper(_DWORD *a1)`
  - `if ( dword_BE78A4 == 0 )`
  - `CActionFrame::LoadMappers();`
  - `if ( !v2 )`
  - `(*(*v1 + 4))(a1: v1);`

### CActionFrame::LoadMappers  @ 0x4011c6  (9917 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1076_FACE`, `SP_3804_CHARACTERSTART`, `SP_3805_CHARACTEREND`
  - `_DWORD *CActionFrame::LoadMappers()`
  - `sub_402F85(pvargDest: &v41, pvargSrc: &pvargSrc);`
  - `LOBYTE(v51) = 3;`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_405210(a1: &dword_BE788C);`
  - `v1 = sub_406472();`
  - `v2 = sub_402680(a1: v1);`
  - `IWzResMan::GetObjectA(this: v2, pvargDest: &v40, sUOL: v34, vParam: p_pvargDest, vAux: v36);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v37, a2: NewEnum);`
  - `v3 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v3 < 0 && v3 != -2147467262 )`
  - `if ( v4 < 0 )`
  - `if ( v5 < 0 )`
  - `if ( v6 < 0 )`
  - …（截断 14 行）

### CConfig::LoadFuncKeyMapped  @ 0x49df79  (220 B)

  - `int __thiscall CConfig::LoadFuncKeyMapped(_DWORD *this, void *a2)`
  - `if ( *(this + 20) == 0 )`
  - `memcpy(a1: a2, a2: this + 154, a3: 0x1BDu);`

### CDraggableItem::MapFuncKey  @ 0x4f38f4  (5010 B)

  - `signed int __thiscall CDraggableItem::MapFuncKey(CDraggableItem *this, int a2, int a3, int a4)`
  - `v5 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v25) + 4);`
  - `if ( v26 != 0 )`
  - `v6 = *(CharacterData::GetItem(this: v5, a2: &v25, a3: *(v4 + 6), a4: *(v4 + 7)) + 4);`
  - `if ( v6 == 0 )`
  - `Data = TSecType<long>::GetData(a1: (v6 + 12), a2: 0);`
  - `v8 = *(v4 + 6);`
  - `if ( v8 == 2 )`
  - `if ( v9 == 200`
  - `if ( v31 / 10000 == 524 || v31 / 10000 == 530 )`
  - `if ( get_etc_cash_item_type(a1: v31) == 6 )`
  - `v10 = *(v4 + 6);`
  - `if ( v10 == 5 && v31 / 10000 == 501 || v10 == 4 && v31 / 10000 == 429 )`
  - `if ( v10 != 3 || v31 / 10000 != 301 )`
  - …（截断 14 行）

### CDraggableItem::UnmapFuncKey  @ 0x4f3bf7  (2859 B)

  - `signed int __thiscall CDraggableItem::UnmapFuncKey(_DWORD *this, int a2)`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v16) + 4);`
  - `if ( v17 != 0 )`
  - `v4 = *(CharacterData::GetItem(this: v3, a2: &v16, a3: v2[6], a4: v2[7]) + 4);`
  - `if ( v4 == 0 )`
  - `Data = TSecType<long>::GetData(a1: (v4 + 12), a2: 0);`
  - `if ( v6 == 2 )`
  - `if ( v7 != 200`
  - `if ( v6 == 5 && get_etc_cash_item_type(a1: v19) != 6 )`
  - `cashslot_item_type = get_cashslot_item_type(a1: v19);`
  - `if ( cashslot_item_type < 8 || cashslot_item_type > 11 && cashslot_item_type != 38 )`
  - `if ( cashslot_item_type != 9 )`
  - `if ( a2 != 0 )`
  - `v11 = *(v17 + 4 * v18 + 4);`
  - …（截断 14 行）

### CDraggableMenu::MapFuncKey  @ 0x4f9386  (2933 B)

  - `int __thiscall CDraggableMenu::MapFuncKey(CDraggableMenu *this, int a2, int a3, int a4)`
  - `if ( a4 != 0 && *(TSingleton<CUIStatusBar>::ms_pInstance + 3344) == 0 )`
  - `if ( dword_BED650 != 0 )`
  - `if ( a4 != 0 )`
  - `ShortCutIndexByPos = CUIStatusBar::GetShortCutIndexByPos(a1: a2, a2: a3);`
  - `ShortCutIndexByPos = CUIKeyConfig::GetShortCutIndexByPos(this: v16, a2, a3);`
  - `if ( ShortCutIndexByPos >= 8 )`
  - `else if ( ShortCutIndexByPos < 0 || ShortCutIndexByPos > 90 && *(v5 + 8) != 0 )`
  - `for ( i = 0; i < 89; ++i )`
  - `if ( i == 54 )`
  - `if ( *v10 == *(v5 + 6) && *(v10 + 1) == *(v5 + 7) )`
  - `if ( ShortCutIndexByPos <= 90 )`
  - `v11 = *(v4 + 4 * ShortCutIndexByPos + 4);`
  - `if ( v11 != 54 )`
  - …（截断 14 行）

### CDraggableSkill::MapFuncKey  @ 0x4fb32c  (2917 B)

  - `signed int __thiscall CDraggableSkill::MapFuncKey(CDraggableSkill *this, int rx, int ry, int bOnStatusBar)`
  - `if ( bOnStatusBar == 0 || *(TSingleton<CUIStatusBar>::ms_pInstance + 3344) != 0 )`
  - `if ( dword_BED650 != 0 )`
  - `? CUIStatusBar::GetShortCutIndexByPos(a1: rx, a2: ry)`
  - `: CUIKeyConfig::GetShortCutIndexByPos(this: v17, a2: rx, a3: ry);`
  - `if ( v5 != -1 )`
  - `for ( i = 0; i < 89; ++i )`
  - `if ( i == 54 )`
  - `if ( *v9 == 1 && *(v9 + 1) == *(v4 + 6) && *(v4 + 7) == 0`
  - `|| *v9 == 8 && *(v9 + 1) == *(v4 + 6) && *(v4 + 7) != 0 )`
  - `if ( v5 < 89 )`
  - `if ( bOnStatusBar != 0 )`
  - `v10 = *(v14 + 4 * v5 + 4);`
  - `if ( v10 != 54 )`
  - …（截断 14 行）

### CDraggableSkill::MapMacro  @ 0x4fb112  (2785 B)

  - `signed int __thiscall CDraggableSkill::MapMacro(UnwindMapEntry **this, int a2, int a3, int a4, int a5)`
  - `if ( !is_able_to_map_macrosys_skill(nSkillID: *(this + 6)) )`
  - `Skill = CSkillInfo::GetSkill(this: dword_BE78DC, a2: *(this + 6));`
  - `if ( Skill != 0 )`
  - `if ( sub_4FB30C(this: Skill) >= 1 )`
  - `LevelData = SKILLENTRY::GetLevelData(this: v7, a2: v9, a3: v8);`
  - `if ( _ZtlSecureFuse<long>(a1: LevelData + 444, a2: *(LevelData + 452)) > 0 )`
  - `while ( ++v8 <= sub_4FB30C(this: v7) );`
  - `if ( dword_BED668 != 0 )`
  - `if ( dword_BED664 == 0 )`
  - `if ( a5 != 0 )`
  - `v12 = *(this + 8);`
  - `if ( v12 != -1 && *(this + 9) != -1 )`
  - `sub_631A99(this: dword_BEC1F4, a2: v12, a3: *(this + 9));`
  - …（截断 14 行）

### CField::LoadMap  @ 0x529bb4  (4782 B)

- SP_ 节点：`SP_1473_PORTAL`, `SP_191__U`
  - `int __thiscall CField::LoadMap(CMapLoadable *this, signed int i)`
  - `CMapLoadable::LoadMap(this);`
  - `CField::RestoreOption(a1: this);`
  - `CField::RestoreHelpMsg(a1: i);`
  - `CField::RestorePeculiarInfo(a1: this);`
  - `CField::RestoreSeat(a1: this);`
  - `CField::RestoreClock(a1: this);`
  - `CField::RestoreToolTip(this);`
  - `CField::RestoreWeatherMsg(a1: this);`
  - `if ( *(*dword_BE7918 + 12464) == 0 )`
  - `CMapLoadable::PlayBGMFromMapInfo(a1: this);`
  - `sub_4281C2(a1: this + 48);`
  - `v5 = sub_538C92();`
  - `CUIMiniMap::LoadMiniMap(this: v5, a2: v18, a3: p_i, p_p_pvarg: v20);`
  - …（截断 14 行）

### CFuncKeyMappedMan::AdaptVirtualKey  @ 0x58deac  (1750 B)

- 分支 case：`1`, `2`, `3`
  - `void __thiscall CFuncKeyMappedMan::AdaptVirtualKey(char *this)`
  - `if ( TSingleton<CUIStatusBar>::ms_pInstance != 0 )`
  - `while ( 1 )`
  - `if ( *(v3 - 1) == 4 && v1 != 0 )`
  - `if ( *v3 != 0 )`
  - `switch ( *v3 )`
  - `v6 = MapVirtualKeyA(uCode: v2, uMapType: 1u);`
  - `CUIStatusBar::SetShortcutTooltip(this: v1, a2: 1, a3: v6);`
  - `v5 = MapVirtualKeyA(uCode: v2, uMapType: 1u);`
  - `CUIStatusBar::SetShortcutTooltip(this: v1, a2: 2, a3: v5);`
  - `v4 = MapVirtualKeyA(uCode: v2, uMapType: 1u);`
  - `CUIStatusBar::SetShortcutTooltip(this: v1, a2: 3, a3: v4);`
  - `v7 = MapVirtualKeyA(uCode: v2, uMapType: 1u);`
  - `CUIStatusBar::SetShortcutTooltip(this: v1, a2: 0, a3: v7);`
  - …（截断 14 行）

### CFuncKeyMappedMan::DefaultFuncKeyMap  @ 0x58e114  (156 B)

  - `void *__thiscall CFuncKeyMappedMan::DefaultFuncKeyMap(CFuncKeyMappedMan *this)`
  - `return memcpy(a1: this + 4, a2: dword_BD8BCC, a3: 0x1BDu);`

### CFuncKeyMappedMan::FuncKeyMapped  @ 0x58e12b  (973 B)

- 分支 case：`0`, `1`, `2`, `3`, `4`, `5`, `6`, `7`
  - `char *__thiscall CFuncKeyMappedMan::FuncKeyMapped(char *this, int nIdx)`
  - `switch ( nIdx )`

### CFuncKeyMappedMan::OnInit  @ 0x58ddb4  (845 B)

- s2c 解码：`Decode1`
  - `int __thiscall CFuncKeyMappedMan::OnInit(char *this, int a2)`
  - `if ( CInPacket::Decode1(this: a2) != 0 )`
  - `if ( CConfig::LoadFuncKeyMapped(this: TSingleton<CConfig>::ms_pInstance, a2: this + 4) == 0 )`
  - `CFuncKeyMappedMan::DefaultFuncKeyMap(this);`
  - `CFuncKeyMappedMan::SaveFuncKeyMap(this);`
  - `for ( i = 89; i != 0; --i )`
  - `FUNCKEY_MAPPED::Decode(a1: v4, a2);`
  - `CFuncKeyMappedMan::AdaptVirtualKey(this);`
  - `memcpy(a1: this + 449, a2: v3, a3: 0x1BDu);`
  - `return CFuncKeyMappedMan::SaveFuncKeyMap(this);`

### CFuncKeyMappedMan::SaveFuncKeyMap  @ 0x58df2f  (1657 B)

- **c2s opcode `COutPacket(135)`**
- c2s 编码：`Encode4`
  - `int __thiscall CFuncKeyMappedMan::SaveFuncKeyMap(CFuncKeyMappedMan *this)`
  - `COutPacket::COutPacket(a1: 135);`
  - `COutPacket::Encode4(this: &v7, a2: 0);`
  - `LOBYTE(v10) = 1;`
  - `if ( *v3 != v3[445] || *(v3 + 1) != *(v3 + 446) )`
  - `*ZArray<long>::InsertBefore(a1: -1) = v2;`
  - `while ( v2 < 89 );`
  - `if ( dword_BEC1F8 != 0 )`
  - `CSequencedKeyMan::Restore(this: dword_BEC1F8);`
  - `if ( v9 != 0 )`
  - `v4 = *(v9 - 4);`
  - `if ( v4 != 0 )`
  - `COutPacket::Encode4(this: &v7, a2: *(v9 - 4));`
  - `for ( i = 0; i < v4; ++i )`
  - …（截断 14 行）

### CItemInfo::FindMapByName  @ 0x5cfaf3  (5786 B)

- SP_ 节点：`SP_1730_MAPNAME`
  - `int __thiscall CItemInfo::FindMapByName(char *this, int a2, char *a3)`
  - `memset(&v17[2], 0, 12);`
  - `ZXString<char>::TrimRight(a1: 0);`
  - `v3 = ZXString<char>::TrimLeft(a1: 0);`
  - `ZXString<char>::operator=(a1: v3);`
  - `sub_538AAC(a1: &a3);`
  - `for ( i = ZXString<char>::Find(a1: asc_AF1E78, a2: 0, a3: 0);`
  - `i = ZXString<char>::Find(a1: asc_AF1E78, a2: 0, a3: 0) )`
  - `v5 = sub_5DB9D7(a1: &v26, a2: asc_AF1E78, a3: &sStrDefault);`
  - `LOBYTE(v30) = 3;`
  - `ZXString<char>::operator=(a1: v5);`
  - `if ( v26 != 0 )`
  - `v6 = *(v27 + 28);`
  - `v26 = (v27 + 108);`
  - …（截断 14 行）

### CItemInfo::GetMapString  @ 0x5cf792  (6302 B)

- SP_ 节点：`SP_2498_MAP_MAP_MAPD_09DIMG`, `SP_961_INFO`, `SP_962_LINK`
  - `_DWORD *__thiscall CItemInfo::GetMapString(void *this, _DWORD *a2, unsigned int a3, char a4)`
  - `LOBYTE(v43) = 2;`
  - `if ( v5 != 0 )`
  - `sub_416821(a1: &v39, a2: v6, a3: a3 / 100000000);`
  - `sub_402D9A(this: &Unknown);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v33, pvargSrc: &pvargSrc);`
  - `v7 = sub_41468C(a1: &v39);`
  - `_bstr_t::_bstr_t(a1: v7);`
  - `v9 = sub_406472();`
  - `v10 = sub_402680(a1: v9);`
  - `IWzResMan::GetObjectA(this: v10, pvargDest: &pvarg, sUOL: v25, vParam: p_pvarg, vAux: p_pvargDest);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v28, a2: v29);`
  - `v11 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - …（截断 14 行）

### CItemInfo::IterateMapString  @ 0x5d0467  (6228 B)

  - `void __thiscall CItemInfo::IterateMapString(void *this, int a2)`
  - `if ( a2 == 0 )`
  - `NewEnum = IWzProperty::Get_NewEnum(a1: v28);`
  - `LOBYTE(v37) = 1;`
  - `sub_403606(a1: NewEnum);`
  - `sub_4027B1(a1: v28);`
  - `VariantInit(&pvarg);`
  - `while ( 1 )`
  - `if ( v31 == 0 )`
  - `if ( (*(*v31 + 12))(a1: v31, a2: 1, a3: &pvarg, a4: &v27) != 0 )`
  - `sub_406478(a1: &v32, pvarSrc: &pvarg);`
  - `if ( v32 != 0 )`
  - `v24 = sub_A62D95(a1: v3, a2: &v25, a3: 10);`
  - `sub_405210(a1: &v32);`
  - …（截断 14 行）

### CMacroSysMan::Map  @ 0x631b26  (217 B)

  - `int __thiscall CMacroSysMan::Map(const void **this, int a2, int a3, int a4)`
  - `CMacroSysMan::Realloc(this, a2: a2 + 1);`
  - `*(*(this + 11) + 29 * a2 + 4 * a3) = a4;`

### CMapLoadable::Close  @ 0x63988a  (2607 B)

  - `int __thiscall CMapLoadable::Close(CMapLoadable *this)`
  - `CStage::FadeIn();`
  - `if ( dword_BF14EC == 0 )`
  - `Center = IWzGr2D::GetCenter(this: dword_BF14EC, a2: &v15);`
  - `if ( v3 )`
  - `v6 = (*(v5 + 100))(a1: v4, a2: *&pvarg.vt, a3: Hi32, a4: lVal, a5: Hi);`
  - `if ( v6 < 0 )`
  - `if ( v15 != 0 )`
  - `(*(*v15 + 8))(a1: v15);`
  - `v7 = IWzGr2D::GetCenter(this: dword_BF14EC, a2: &v16);`
  - `v9 = (*(**v7 + 64))(a1: *v7, a2: 0, a3: 0);`
  - `if ( v9 < 0 )`
  - `if ( v16 != 0 )`
  - `(*(*v16 + 8))(a1: v16);`
  - …（截断 14 行）

### CMapLoadable::GetObjectRect  @ 0x642e87  (6758 B)

- SP_ 节点：`SP_5636_LT`, `SP_5647_RB`
  - `int *__stdcall CMapLoadable::GetObjectRect(int *a1, char a2)`
  - `dword_BF0410(a1: &v31);`
  - `v2 = sub_64527E(a1: &a2, a2: 0);`
  - `if ( v2 != 0 )`
  - `v42 = *(16 * *v2 + v2[1] + 8);`
  - `sub_432D38(a1: &v42);`
  - `if ( v42 == 0 )`
  - `v43 = *IWzGr2DLayer::GetAlpha(a1: &Unknown) == 0;`
  - `LOBYTE(v44) = 0;`
  - `sub_41569E(a1: &Unknown);`
  - `if ( v43 )`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `IWzGr2DLayer::GetCanvas(a1: &v39, a2: &pvarg);`
  - `if ( v4 < 0 )`
  - …（截断 14 行）

### CMapLoadable::Init  @ 0x639708  (2288 B)

  - `void __thiscall CMapLoadable::Init(CMapLoadable *this, void *a2)`
  - `v2 = *(TSingleton<CConfig>::ms_pInstance + 25);`
  - `if ( v2 != 0 )`
  - `if ( v3 != 0 )`
  - `*(this + 58) = 0;`
  - `*(this + 59) = v3 == 1;`
  - `if ( dword_BF14EC == 0 )`
  - `sub_5FC251(a1: dword_BF14EC);`
  - `Center = IWzGr2D::GetCenter(this: dword_BF14EC, a2: &v13);`
  - `if ( v5 )`
  - `v7 = (*(**Center + 100))(a1: *Center, a2: *&pvarg.vt, a3: pvarg.decVal.Hi32, a4: pvarg.lVal, a5: pvarg.cyVal.Hi);`
  - `if ( v7 < 0 )`
  - `if ( v13 != 0 )`
  - `(*(*v13 + 8))(a1: v13);`
  - …（截断 14 行）

### CMapLoadable::LoadMap  @ 0x639b3d  (13062 B)

- SP_ 节点：`SP_1486_FOOTHOLD`, `SP_1487_LADDERROPE`, `SP_1515_TYPE`, `SP_1877_LIFE`, `SP_2366_ID`, `SP_2980_REACTOR`
  - `_DWORD *__thiscall CMapLoadable::LoadMap(VARIANTARG *this)`
  - `v3 = (this + 3);`
  - `v73 = (this + 3);`
  - `sub_40263B(a1: this + 3);`
  - `IWzProperty::GetItem(pvargDest: &pvarg, a2: v61);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v62, a2: v63[0]);`
  - `if ( Unknown != 0 )`
  - `v4 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v4 < 0 && v4 != -2147467262 )`
  - `LOBYTE(v83) = 3;`
  - `if ( v5 < 0 )`
  - `IWzProperty::GetItem(&pvargDest, a2: v61);`
  - `v70 = Ztl_variant_t::GetUnknown(a1: v62, a2: v63[0]);`
  - `if ( v70 != 0 )`
  - …（截断 14 行）

### CMapLoadable::MakeBack  @ 0x63cd4e  (44508 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1464_F`, `SP_1492_NO`, `SP_1500_BLEND`, `SP_1507_BS`, `SP_1508_MAP_BACK_`, `SP_1509_RX`, `SP_1510_RY`, `SP_1511_CX`, `SP_1512_CY`, `SP_1513_ANI`, `SP_1514_A`, `SP_1515_TYPE`, `SP_1516_FRONT`…
  - `_DWORD *__thiscall CMapLoadable::MakeBack(_DWORD *this, int *a2, _DWORD *a3)`
  - `LODWORD(v204[0]) = this;`
  - `LOBYTE(v235) = 0;`
  - `sub_40263B(a1: &a3);`
  - `Item = IWzProperty::GetItem(pvargDest: &pvarg, a2: v204[0]);`
  - `sub_406478(a1: &v227, pvarSrc: Item);`
  - `if ( v5 < 0 )`
  - `if ( v227 != 0 && *v227 != 0 )`
  - `v6 = *(*v227 - 4) >> 1;`
  - `if ( v6 == 0 )`
  - `LODWORD(v204[0]) = SP_1508_MAP_BACK_;`
  - `if ( v227 != 0 )`
  - `v9 = wcslen(a1: v8);`
  - `sub_48EF51(a1: v8, a2: v9);`
  - …（截断 14 行）

### CMapLoadable::MakeCloud  @ 0x63f717  (14434 B)

- SP_ 节点：`SP_1526_MAP_MAPHELPERIMG_WEATHER_CLOUD`
  - `int __thiscall CMapLoadable::MakeCloud(_DWORD *this)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v66, pvargSrc: &pvargSrc);`
  - `LOBYTE(v98) = 1;`
  - `v4 = sub_406472();`
  - `v5 = sub_402680(a1: v4);`
  - `IWzResMan::GetObjectA(this: v5, pvargDest: &pvarg, sUOL: v56, vParam: v57, vAux: p_pvargDest);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v59, a2: v60);`
  - `if ( Unknown != 0 )`
  - `v6 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v6 < 0 && v6 != -2147467262 )`
  - `if ( v7 < 0 )`
  - `if ( v8 < 0 )`
  - `if ( v9 < 0 )`
  - …（截断 14 行）

### CMapLoadable::MakeObj  @ 0x63ad16  (46921 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1142_SFX`, `SP_1464_F`, `SP_1467_DX`, `SP_1468_DY`, `SP_1495_MAP_OBJ_SIMG_S_S_S`, `SP_1496_OS`, `SP_1497_L0`, `SP_1498_L1`, `SP_1499_L2`, `SP_1502_OBSTACLE`, `SP_1509_RX`, `SP_1510_RY`, `SP_1511_CX`…
  - `int *__thiscall CMapLoadable::MakeObj(char *this, int **a2, int *a3)`
  - `LOBYTE(v220) = 1;`
  - `sub_40263B(a1: &a3);`
  - `Item = IWzProperty::GetItem(pvargDest: &v191, a2: v180[0]);`
  - `if ( v6 )`
  - `v7 = *(Item + 8);`
  - `v9 = IWzProperty::GetItem(pvargDest: &v188, a2: p_pvargDest);`
  - `v10 = sub_440D94(a1: v9, a2: p_pvarg);`
  - `v13 = IWzProperty::GetItem(pvargDest: &v192, a2: p_a1);`
  - `v14 = sub_440D94(a1: v13, a2: p_pvargDest);`
  - `v17 = IWzProperty::GetItem(pvargDest: &pvarg, a2: v176);`
  - `p_a1 = sub_440D94(a1: v17, a2: p_a1);`
  - `v20 = sub_41468C(a1: StringW);`
  - `sub_416821(a1: &v199, a2: v20, a3: p_a1);`
  - …（截断 14 行）

### CMapLoadable::MakeObjLayer  @ 0x63c212  (11127 B)

- SP_ 节点：`SP_958_SHAPE2DVECTOR2D`
  - `_DWORD *__thiscall CMapLoadable::MakeObjLayer(`
  - `LOBYTE(v67) = 3;`
  - `PcCreateObject::IWzVector2D(a1: v15, a2: &v66, a3: 0);`
  - `if ( v64[0] != 0 )`
  - `HIDWORD(v60[0]) = p_Hi32;`
  - `LODWORD(v60[0]) = 255;`
  - `HIDWORD(v59) = 30000 * a4 + a7 - 1073739824;`
  - `LODWORD(v59) = a7;`
  - `sub_432C10(a1: 0);`
  - `CAnimationDisplayer::LoadLayer(this: &v58.decVal.Hi32, a2: &v66);`
  - `sub_4281C2(a1: &a3);`
  - `sub_43EA3E(`
  - `a8: HIDWORD(v59),`
  - `a10: HIDWORD(v60[0]));`
  - …（截断 14 行）

### CMapLoadable::MakeObstacles  @ 0x63c741  (7830 B)

- SP_ 节点：`SP_1502_OBSTACLE`, `SP_1503_DAMAGE`, `SP_1504_MOBDAMAGE`, `SP_1505_DIR`, `SP_1611_LEVEL`, `SP_2863_DISEASE`, `SP_4890_FORCEHP`, `SP_5116_TARGETFIELD`
  - `int __thiscall CMapLoadable::MakeObstacles(char *this, int a2, int a3, int *a4, int a5)`
  - `LOBYTE(v53) = 1;`
  - `sub_40263B(a1: &a2);`
  - `Item = IWzProperty::GetItem(pvargDest: &pvarg, a2: v47);`
  - `v8 = get_int32(pvarSrc: Item, a2: v48) != 0;`
  - `if ( v9 < 0 )`
  - `if ( v8 )`
  - `v11 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 52);`
  - `if ( v11 != 0 )`
  - `v12 = sub_63CB2E(a1: v11);`
  - `sub_645057(a1: v50);`
  - `sub_428712(a1: a3);`
  - `*(v51 + 16) = a4;`
  - `v14 = IWzProperty::GetItem(pvargDest: &pvarg, a2: v47);`
  - …（截断 14 行）

### CMapLoadable::MakeTile  @ 0x63a5c4  (8719 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1491_U`, `SP_1492_NO`, `SP_1493_ZM`, `SP_5520_S_D`, `SP_977_X`, `SP_978_Y`, `SP_979_Z`
  - `int __thiscall CMapLoadable::MakeTile(void *this, int a2, int a3, int a4)`
  - `LOBYTE(v56) = 2;`
  - `sub_40263B(a1: &a4);`
  - `Item = IWzProperty::GetItem(&pvargDest, a2: v44[0]);`
  - `v44[0] = sub_403007(a1: Item);`
  - `v8 = IWzProperty::GetItem(pvargDest: &pvarg, a2: p_pvarg);`
  - `p_pvarg = sub_63AA0C(a1: v8);`
  - `v11 = sub_41468C(a1: StringW);`
  - `sub_416821(a1: &v53, a2: v11, a3: p_pvarg);`
  - `if ( Unknown != 0 )`
  - `if ( v12 < 0 )`
  - `if ( v13 < 0 )`
  - `v14 = sub_41468C(a1: &v53);`
  - `_bstr_t::_bstr_t(a1: v14);`
  - …（截断 14 行）

### CMapLoadable::MakeVectorAnimate  @ 0x63ef34  (16347 B)

- 分支 case：`1`, `2`, `3`
- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_100_MAPLESTORY_ERROR__LOADING_REACTOR_FAILED`, `SP_1520_MOVETYPE`, `SP_1521_MOVER`, `SP_1522_MOVEW`, `SP_1523_MOVEH`, `SP_1524_MOVEP`, `SP_5000_UI_UIWINDOWIMG_CHARACTERCLONE_BTFINISH`, `SP_958_SHAPE2DVECTOR2D`
  - `int __stdcall CMapLoadable::MakeVectorAnimate(int a1, int a2)`
  - `if ( a1 == 0 )`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `LOBYTE(v84) = 2;`
  - `IWzGr2DLayer::GetCanvas(a1: &v82, a2: &pvarg);`
  - `if ( v2 < 0 )`
  - `if ( v82 == 0 )`
  - `sub_4156AB(a1: &v82);`
  - `sub_4156C5(&a1);`
  - `return sub_41569E(a1: &a2);`
  - `v5 = sub_414576(a1: &v82);`
  - `Property = IWzCanvas::GetProperty(this: v5, a2: &v77);`
  - `sub_40263B(a1: Property);`
  - `Item = IWzProperty::GetItem(&pvargDest, a2: v70);`
  - …（截断 14 行）

### CMapLoadable::OnClearBackEffect  @ 0x6449ca  (147 B)

  - `void __thiscall CMapLoadable::OnClearBackEffect(CMapLoadable *this, int a2)`
  - `CMapLoadable::ReloadBack(this);`

### CMapLoadable::OnLeaveDirectionMode  @ 0x644482  (170 B)

  - `int __thiscall CMapLoadable::OnLeaveDirectionMode(CMapLoadable *this)`
  - `if ( *(this + 6) == 0 )`
  - `*(this + 6) = -1;`
  - `return sub_641ECC();`

### CMapLoadable::OnPacket  @ 0x644446  (480 B)

- 分支 case：`128`, `129`, `130`
  - `void __thiscall CMapLoadable::OnPacket(char *this, int a2, struct CInPacket *a3)`
  - `switch ( a2 )`
  - `CMapLoadable::OnSetBackEffect(a1: a3);`
  - `CMapLoadable::OnSetMapObjectVisible(a1: a3);`
  - `CMapLoadable::OnClearBackEffect(this: (this - 8), a2: a3);`

### CMapLoadable::OnSetBackEffect  @ 0x6445c5  (6485 B)

  - `int __stdcall CMapLoadable::OnSetBackEffect(struct CInPacket *a1)`
  - `sub_64491F(a1: v21, a2: 0);`
  - `Field::BackEffect::Decode(this: v21, a2: a1);`
  - `if ( v22 == 0 )`
  - `LOBYTE(v36) = 1;`
  - `sub_6456D2(a1: &v23, a2: v29);`
  - `if ( v30 != 0 )`
  - `v11 = v24 + get_update_time();`
  - `v12 = *(v30 + 12);`
  - `if ( v12 != 0 )`
  - `while ( 1 )`
  - `v13 = *(v12 != 0 ? v12 - 3 : 4);`
  - `sub_432D38(&a1);`
  - `if ( a1 == 0 )`
  - …（截断 14 行）

### CMapLoadable::OnSetMapObjectVisible  @ 0x6449d2  (846 B)

- s2c 解码：`Decode1`, `DecodeStr`
  - `void __stdcall CMapLoadable::OnSetMapObjectVisible(struct CInPacket *a1)`
  - `v1 = CInPacket::Decode1(this: a1);`
  - `if ( v1 != 0 )`
  - `for ( i = v1; i != 0; --i )`
  - `CInPacket::DecodeStr(this: a1, a2: &v7);`
  - `v5 = CInPacket::Decode1(this: a1);`
  - `sub_428211(a1: &v7);`
  - `CMapLoadable::SetMapObjectVisible(a1: v4, a2: v5);`

### CMapLoadable::PlayBGMFromMapInfo  @ 0x64211e  (1775 B)

- SP_ 节点：`SP_1533_BGM`
  - `int __thiscall CMapLoadable::PlayBGMFromMapInfo(CMapLoadable *this)`
  - `v9 = *(this + 11);`
  - `if ( v9 != 0 )`
  - `(*(*v9 + 4))(a1: v9);`
  - `LOBYTE(v11) = 0;`
  - `sub_40263B(a1: &v9);`
  - `IWzProperty::GetItem(pvargDest: &pvarg, a2: v6[0]);`
  - `if ( pvarg.vt == 8 )`
  - `v3 = wcslen(a1: pvarg.bstrVal);`
  - `sub_48EF51(a1: pvarg.bstrVal, a2: v3);`
  - `CSoundMan::PlayBGM(a1: v10, a2: 1, a3: 600, a4: 0, a5: 0);`
  - `sub_41C06C(a1: &word_AF0E08, a2: -1);`
  - `if ( v4 < 0 )`
  - `return (*(*v9 + 8))(a1: v9);`
  - …（截断 14 行）

### CMapLoadable::PlayNextMusic  @ 0x641db2  (2035 B)

- SP_ 节点：`SP_1803_PATH`
  - `void __thiscall CMapLoadable::PlayNextMusic(CMapLoadable *this)`
  - `CSoundMan::PlayBGM(a1: 0, a2: 0, a3: 0, a4: 1000, a5: 0);`
  - `CConfig::ApplySysOpt(this: TSingleton<CConfig>::ms_pInstance, pSysOpt: 0, bApplyVideo: 0);`
  - `if ( *(this + 6) == -1 )`
  - `CMapLoadable::RestoreBGM(this);`
  - `*(this + 8) = 0;`
  - `CItemInfo::GetItemInfo(a1: &v11, a2: *(this + 6));`
  - `if ( v11 != 0 )`
  - `LOBYTE(v13) = 0;`
  - `sub_40263B(a1: &v11);`
  - `Item = IWzProperty::GetItem(pvargDest: &pvarg, a2: v7);`
  - `v5 = sub_440D94(a1: Item, a2: v8);`
  - `sub_41C06C(a1: v5, a2: -1);`
  - `if ( v6 < 0 )`
  - …（截断 14 行）

### CMapLoadable::ProcessReflection  @ 0x643f7a  (9265 B)

  - `int __thiscall CMapLoadable::ProcessReflection(CMapLoadable *this)`
  - `result = *(this + 38);`
  - `if ( result != 0 )`
  - `while ( 1 )`
  - `v2 = *(result != 0 ? result - 16 + 4 : 4);`
  - `v3 = *(result + 4);`
  - `sub_645024(a1: v49);`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `LOBYTE(v52) = 1;`
  - `if ( v4 )`
  - `IWzGr2DLayer::GetCanvas(a1: &v48, a2: &pvarg);`
  - `if ( v5 < 0 )`
  - `if ( v48 != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - …（截断 14 行）

### CMapLoadable::ReloadBack  @ 0x644491  (2342 B)

  - `int __thiscall CMapLoadable::ReloadBack(CMapLoadable *this)`
  - `sub_645810(a1: this + 208);`
  - `if ( dword_BF14EC == 0 )`
  - `Center = IWzGr2D::GetCenter(this: dword_BF14EC, a2: &v20);`
  - `if ( v2 )`
  - `v5 = (*(v4 + 100))(a1: v3, a2: *&pvarg.vt, a3: Hi32, a4: lVal, a5: Hi);`
  - `if ( v5 < 0 )`
  - `if ( v20 != 0 )`
  - `(*(*v20 + 8))(a1: v20);`
  - `v6 = IWzGr2D::GetCenter(this: dword_BF14EC, a2: &v19);`
  - `v8 = (*(**v6 + 64))(a1: *v6, a2: 0, a3: 0);`
  - `if ( v8 < 0 )`
  - `if ( v19 != 0 )`
  - `(*(*v19 + 8))(a1: v19);`
  - …（截断 14 行）

### CMapLoadable::RestoreBGM  @ 0x642214  (547 B)

  - `int __thiscall CMapLoadable::RestoreBGM(CMapLoadable *this)`
  - `if ( &word_AF0E08 == 0 )`
  - `v3 = *(this + 10);`
  - `if ( v3 == 0 )`
  - `if ( wcscmp(a1: v3, a2: v1) == 0 )`
  - `return CMapLoadable::PlayBGMFromMapInfo(this);`
  - `return CSoundMan::PlayBGM(a1: *(this + 10), a2: 1, a3: 1000, a4: 1000, a5: 1);`

### CMapLoadable::RestoreBack  @ 0x63cbba  (3711 B)

- SP_ 节点：`SP_1506_BACK`
  - `int __thiscall CMapLoadable::RestoreBack(CMapLoadable *this)`
  - `sub_40263B(a1: this + 48);`
  - `IWzProperty::GetItem(pvargDest: &pvarg, a2: v16);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v17, a2: v18);`
  - `if ( Unknown != 0 )`
  - `v3 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v3 < 0 && v3 != -2147467262 )`
  - `LOBYTE(v27) = 3;`
  - `if ( v4 < 0 )`
  - `if ( v26 == 0 )`
  - `v22 = IWzProperty::Getcount(a1: v26);`
  - `if ( v22 > 0 )`
  - `v6 = alloca(32);`
  - `v7 = _Int2StrW(a1: v5, a2: &v15);`
  - …（截断 14 行）

### CMapLoadable::RestoreObj  @ 0x63aa7e  (6603 B)

- SP_ 节点：`SP_1494_OBJ`, `SP_2980_REACTOR`
  - `int __thiscall CMapLoadable::RestoreObj(CMapLoadable *this)`
  - `for ( i = 0; i < 8; ++i )`
  - `v1 = alloca(32);`
  - `v2 = _Int2StrW(a1: i, a2: &v30);`
  - `_bstr_t::_bstr_t(a1: v2);`
  - `sub_40263B(a1: v40 + 12);`
  - `IWzProperty::GetItem(pvargDest: p_pvarg, a2: v27);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v28, a2: v29);`
  - `if ( Unknown != 0 )`
  - `v3 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v3 < 0 && v3 != -2147467262 )`
  - `LOBYTE(v45) = 3;`
  - `if ( v4 < 0 )`
  - `sub_40263B(a1: &v41);`
  - …（截断 14 行）

### CMapLoadable::RestoreTile  @ 0x63a100  (12051 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1488_TILE`, `SP_1489_MAP_TILE_`, `SP_1490_TS`, `SP_961_INFO`
  - `int __thiscall CMapLoadable::RestoreTile(CMapLoadable *this)`
  - `for ( i = 0; i < 8; ++i )`
  - `v1 = alloca(32);`
  - `v2 = _Int2StrW(a1: i, a2: &v50);`
  - `_bstr_t::_bstr_t(a1: v2);`
  - `sub_40263B(a1: v64 + 48);`
  - `IWzProperty::GetItem(pvargDest: p_pvargDest, a2: p_pvarg);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v48, a2: v49);`
  - `if ( Unknown != 0 )`
  - `v3 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v3 < 0 && v3 != -2147467262 )`
  - `LOBYTE(v74) = 3;`
  - `if ( v4 < 0 )`
  - `sub_40263B(a1: &v69);`
  - …（截断 14 行）

### CMapLoadable::RestoreViewRange  @ 0x641ef1  (4200 B)

- SP_ 节点：`SP_1528_VRLEFT`, `SP_1529_VRRIGHT`, `SP_1530_VRTOP`, `SP_1531_VRBOTTOM`
  - `int *__thiscall CMapLoadable::RestoreViewRange(int **this)`
  - `v32 = *(this + 11);`
  - `if ( v32 != 0 )`
  - `(*(*v32 + 4))(a1: v32);`
  - `LOBYTE(v33) = 0;`
  - `sub_40263B(a1: &v32);`
  - `Item = IWzProperty::GetItem(pvargDest: &pvarg, a2: v28);`
  - `int32 = get_int32(pvarSrc: Item, a2: v29);`
  - `*(this + 60) = (int32 + 400);`
  - `if ( v7 < 0 )`
  - `v10 = IWzProperty::GetItem(pvargDest: &pvarg, a2: v28);`
  - `v11 = get_int32(pvarSrc: v10, a2: v29);`
  - `*(this + 61) = (v11 + 300);`
  - `if ( v12 < 0 )`
  - …（截断 14 行）

### CMapLoadable::RestoreWeather  @ 0x63f671  (1419 B)

- SP_ 节点：`SP_1525_CLOUD`
  - `int __thiscall CMapLoadable::RestoreWeather(_DWORD *this)`
  - `v12 = *(this + 11);`
  - `if ( v12 != 0 )`
  - `(*(*v12 + 4))(a1: v12);`
  - `LOBYTE(v13) = 0;`
  - `sub_40263B(a1: &v12);`
  - `Item = IWzProperty::GetItem(pvargDest: &pvarg, a2: v8);`
  - `int32 = get_int32(pvarSrc: Item, a2: v9);`
  - `if ( v6 < 0 )`
  - `if ( v5 != 0 )`
  - `CMapLoadable::MakeCloud(this);`
  - `return (*(*v12 + 8))(a1: v12);`

### CMapLoadable::SetFieldMagLevel  @ 0x642890  (4408 B)

  - `int *__thiscall CMapLoadable::SetFieldMagLevel(CMapLoadable *this)`
  - `v4 = *(this + 12) == 0;`
  - `if ( !v4 )`
  - `v5 = *(TSingleton<CConfig>::ms_pInstance + 25);`
  - `if ( v5 != 0 )`
  - `if ( v6 != 0 )`
  - `if ( *(this + 58) != v7 )`
  - `*(this + 58) = v7;`
  - `ZList<_com_ptr_t<_com_IIID<IWzGr2DLayer,&__s_GUID const _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf>>>::RemoveAll(a1: this + 80);`
  - `if ( dword_BF14E8 == 0 )`
  - `v9 = (*(*dword_BF14E8 + 36))(a1: dword_BF14E8, a2: 0);`
  - `if ( v9 < 0 )`
  - `*(this + 65) = 1;`
  - `CMapLoadable::RestoreObj(this);`
  - …（截断 14 行）

### CMapLoadable::SetLayerListVisible  @ 0x644ac5  (969 B)

  - `int __cdecl CMapLoadable::SetLayerListVisible(char a1, int a2, int a3)`
  - `if ( a2 != 0 )`
  - `for ( i = *(a2 + 12); i != 0; result = (*(*v6 + 8))(a1: v6) )`
  - `v7 = *(i != 0 ? i - 3 : 4);`
  - `i = v7 != 0 ? (v7 + 16) : 0;`
  - `if ( !v4 )`
  - `(*(*v6 + 4))(a1: v6);`
  - `if ( v6 == 0 )`
  - `v8 = (*(*v6 + 284))(a1: v6, a2: a3);`
  - `if ( v8 < 0 )`
  - `return sub_53909C(a1: 0);`

### CMapLoadable::SetMapObjectVisible  @ 0x644a44  (886 B)

  - `void __stdcall CMapLoadable::SetMapObjectVisible(int a1, int *a2)`
  - `if ( sub_645492(a1: &v8, a2: v6) != 0 )`
  - `sub_538B00(a1: v6);`
  - `CMapLoadable::SetLayerListVisible(a1: v3, a2: v4, a3: v5);`
  - `LOBYTE(v9) = 0;`
  - `if ( v7 != 0 )`
  - `sub_53909C(a1: 0);`

### CMapLoadable::SetObjectState  @ 0x642aca  (7178 B)

  - `int __thiscall CMapLoadable::SetObjectState(CMapLoadable *this, const char *a2, int *a3)`
  - `v3 = sub_64527E(a1: &a2, a2: 0);`
  - `if ( v3 == 0 )`
  - `if ( a3 != -1 )`
  - `if ( v5 == 0 || *(v5 - 4) <= a3 )`
  - `v7 = (16 * *v4 + v4[1] + 8);`
  - `if ( *v7 == 0 )`
  - `if ( sub_443E36(a1: *v7) != 0 )`
  - `sub_402FAB(a1: -1, a2: 3);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `v10 = *(v9 + v8 + 8) == 0;`
  - `LOBYTE(v47) = 1;`
  - `if ( v10 )`
  - `v11 = *(v9 + v8 + 8);`
  - …（截断 14 行）

### CMapLoadable::TransientLayer_FireCracker  @ 0x6419e7  (1776 B)

- SP_ 节点：`SP_3061_SD`
  - `void __thiscall CMapLoadable::TransientLayer_FireCracker(CMapLoadable *this, char a2)`
  - `CMapLoadable::WeatherLayer_RemoveAll(this);`
  - `LOBYTE(v10) = 1;`
  - `sub_416821(a1: &v8, a2: v4, a3: a2);`
  - `CAnimationDisplayer::RegisterFireCrackerAnimation(`
  - `while ( v9 < 5 );`

### CMapLoadable::TransientLayer_NewYear  @ 0x641b13  (2338 B)

- SP_ 节点：`SP_4683_S_SOUND1`, `SP_5520_S_D`
  - `void __thiscall CMapLoadable::TransientLayer_NewYear(CMapLoadable *this, int a2)`
  - `CMapLoadable::WeatherLayer_RemoveAll(this);`
  - `LOBYTE(v20) = 2;`
  - `sub_416821(a1: &v18, a2: v3, a3: a2);`
  - `sub_416821(a1: &v19, a2: v14, a3: a2);`
  - `sub_444FA1(a1: &v19);`
  - `v9 = sub_41468C(a1: &v18);`
  - `sub_432CBC(a1: v8, a2: v7);`
  - `sub_436B1F(a1: v9, a2: v10, a3: v11, a4: v12, a5: v13, a6: v14, a7: v15);`

### CMapLoadable::TransientLayer_Weather  @ 0x63fe57  (54667 B)

- 分支 case：`3`, `4`, `5`, `6`
- SP_ 节点：`SP_958_SHAPE2DVECTOR2D`
  - `void __thiscall CMapLoadable::TransientLayer_Weather(CMapLoadable *this, int a2, int a3, signed int a4, int *a5)`
  - `HIDWORD(v8) = 0;`
  - `CMapLoadable::WeatherLayer_RemoveAll(this);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v266) = 3;`
  - `sub_402F85(pvargDest: &v254, pvargSrc: &pvargSrc);`
  - `HIDWORD(v186) = &pvargDest;`
  - `LODWORD(v186) = &v254;`
  - `v5 = sub_41468C(a1: &a2);`
  - `HIDWORD(v185) = LODWORD(v8);`
  - `_bstr_t::_bstr_t(a1: v5);`
  - `v6 = sub_406472();`
  - `v7 = sub_402680(a1: v6);`
  - `IWzResMan::GetObjectA(this: v7, pvargDest: &pvarg, sUOL: SHIDWORD(v185), vParam: v186, vAux: SHIDWORD(v186));`
  - …（截断 14 行）

### CMapLoadable::Update  @ 0x6399ef  (1895 B)

  - `int __thiscall CMapLoadable::Update(CMapLoadable *this)`
  - `if ( ++dword_BEDACC % 4 == 0 )`
  - `v2 = *(this + 28);`
  - `while ( v2 != 0 )`
  - `v4 = *(v2 != 0 ? v2 - 3 : 4);`
  - `v2 = v4 != 0 ? (v4 + 16) : 0;`
  - `if ( v3 != 0 )`
  - `(*(*v3 + 4))(a1: v3);`
  - `if ( v3 == 0 )`
  - `Alpha = IWzGr2DLayer::GetAlpha(a1: &v13);`
  - `LOBYTE(v14) = 1;`
  - `if ( v6 )`
  - `v7 = IWzShape2D::Getx(a1: *Alpha);`
  - `if ( v13 != 0 )`
  - …（截断 14 行）

### CMapLoadable::UpdateObstacleInfo  @ 0x642264  (10035 B)

- SP_ 节点：`SP_1505_DIR`, `SP_5636_LT`, `SP_5647_RB`
  - `int __thiscall CMapLoadable::UpdateObstacleInfo(CMapLoadable *this)`
  - `result = sub_645861();`
  - `v3 = *(this + 33);`
  - `if ( v3 != 0 )`
  - `while ( 1 )`
  - `v4 = -(v3 != 0);`
  - `v6 = *(v3 + 4);`
  - `v7 = *((v5 & v4) + 4) != 0 ? *((v5 & v4) + 4) + 16 : 0;`
  - `v8 = *(v6 + 12);`
  - `sub_432D38(a1: &v64);`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `LOBYTE(v67) = 1;`
  - `if ( v64 == 0 )`
  - `IWzGr2DLayer::GetCanvas(a1: &v63, a2: &pvarg);`
  - …（截断 14 行）

### CMapLoadable::WeatherLayer_RemoveAll  @ 0x641aeb  (316 B)

  - `int __thiscall CMapLoadable::WeatherLayer_RemoveAll(CMapLoadable *this)`
  - `ZList<_com_ptr_t<_com_IIID<IWzGr2DLayer,&__s_GUID const _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf>>>::RemoveAll(a1: this + 100);`
  - `sub_447704(a1: dword_BEBF6C + 248);`
  - `return sub_44788E(a1: dword_BEBF6C + 268);`

### CMapleTVMan::GetReceivername  @ 0x6d5835  (183 B)

  - `_DWORD *__thiscall CMapleTVMan::GetReceivername(char *this, _DWORD *a2)`
  - `ZXString<char>::operator=(a1: this + 928);`

### CMapleTVMan::GetSenderName  @ 0x6d5811  (181 B)

  - `_DWORD *__thiscall CMapleTVMan::GetSenderName(char *this, _DWORD *a2)`
  - `ZXString<char>::operator=(a1: this + 924);`

### CMapleTVMan::IsLoadFlash  @ 0x63764c  (189 B)

  - `int __thiscall CMapleTVMan::IsLoadFlash(CMapleTVMan *this)`
  - `if ( *(this + 246) == 0 )`
  - `*(this + 246) = 0;`

### CNpc::DrawMapleTVMessage  @ 0x6d4ad4  (28336 B)

- SP_ 节点：`SP_3955_UI_MAPLETVIMG_TVCHAT1`, `SP_3956_UI_MAPLETVIMG_TVCHAT2`
  - `void __thiscall CNpc::DrawMapleTVMessage(CNpc *this)`
  - `sub_6D5859(a1: v137);`
  - `LOBYTE(v148) = 1;`
  - `sub_6D587D(a1: v138);`
  - `sub_6D58A1(a1: v134);`
  - `sub_6D58C5(a1: v136);`
  - `sub_6D58E9(a1: v135);`
  - `Buffer = ZXString<char>::GetBuffer(a1: 1024, a2: 1);`
  - `CCurseProcess::ProcessString(a1: Buffer, a2: 0, a3: 1);`
  - `ZXString<char>::ReleaseBuffer(a1: -1);`
  - `v4 = ZXString<char>::GetBuffer(a1: 1024, a2: 1);`
  - `CCurseProcess::ProcessString(a1: v4, a2: 0, a3: 1);`
  - `v5 = ZXString<char>::GetBuffer(a1: 1024, a2: 1);`
  - `CCurseProcess::ProcessString(a1: v5, a2: 0, a3: 1);`
  - …（截断 14 行）

### CNpc::SetMapleTVMessage  @ 0x6d417e  (17313 B)

- SP_ 节点：`SP_3942_UI_MAPLETVIMG_TVON`, `SP_3943_UI_MAPLETVIMG_TVOFF`
  - `void __thiscall CNpc::SetMapleTVMessage(CNpc *this)`
  - `if ( !v2 )`
  - `v3 = *(dword_BED76C + 4);`
  - `v70 = *(dword_BED76C + 12);`
  - `if ( v3 != 0 )`
  - `if ( *(this + 75) != 0 )`
  - `sub_402FAB(a1: -2, a2: 3);`
  - `v2 = *(this + 65) == 0;`
  - `if ( v2 )`
  - `IWzGr2DLayer::RemoveCanvas(this: *(this + 65), a2: &v71, a3: &v68);`
  - `sub_4156AB(a1: &v71);`
  - `if ( v4 < 0 )`
  - `*(this + 75) = 0;`
  - `if ( v5 < 0 )`
  - …（截断 14 行）

### CQuickslotKeyMappedMan::DefaultQuickslotKeyMap  @ 0x72b8e6  (170 B)

  - `void *__thiscall CQuickslotKeyMappedMan::DefaultQuickslotKeyMap(CQuickslotKeyMappedMan *this)`
  - `return memcpy(a1: this + 4, a2: dword_BDAFAC, a3: 0x20u);`

### CQuickslotKeyMappedMan::OnInit  @ 0x72b82a  (574 B)

- s2c 解码：`Decode1`, `DecodeBuffer`
  - `int __thiscall CQuickslotKeyMappedMan::OnInit(char *this, int a2)`
  - `if ( CInPacket::Decode1(this: a2) != 0 )`
  - `CInPacket::DecodeBuffer(a1: this + 4, a2: 0x20u);`
  - `CQuickslotKeyMappedMan::DefaultQuickslotKeyMap(a1: this);`
  - `if ( TSingleton<CUIStatusBar>::ms_pInstance != 0 )`
  - `CUIStatusBar::ReDrawQuickslot(this: TSingleton<CUIStatusBar>::ms_pInstance);`
  - `memcpy(a1: this + 36, a2: this + 4, a3: 0x20u);`
  - `return CQuickslotKeyMappedMan::SaveQuickslotKeyMap(a1: this);`

### CQuickslotKeyMappedMan::SaveQuickslotKeyMap  @ 0x72b87e  (570 B)

- **c2s opcode `COutPacket(183)`**
- c2s 编码：`EncodeBuffer`
  - `int __thiscall CQuickslotKeyMappedMan::SaveQuickslotKeyMap(CQuickslotKeyMappedMan *this)`
  - `COutPacket::COutPacket(a1: 183);`
  - `COutPacket::EncodeBuffer(a1: this + 4, a2: 0x20u);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: v3);`
  - `memcpy(a1: this + 36, a2: this + 4, a3: 0x20u);`

### CUIKeyConfig::TryBeginDragFuncKeyMappedIcon  @ 0x83550d  (30835 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1847_DRAGSTART`, `SP_2413_ICON_D`, `SP_4161_UI_UIWINDOWIMG_SKILLMACRO_MACROICON_D_ICON`
  - `wchar_t *__thiscall CUIKeyConfig::TryBeginDragFuncKeyMappedIcon(CUIKeyConfig *this, int arg0, int a3)`
  - `result = CUIKeyConfig::GetShortCutIndexByPos(this, a2: arg0, a3);`
  - `if ( result == -1 )`
  - `v157 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v151) + 4);`
  - `if ( v5 > 90 )`
  - `result = (3 * v5 + 360);`
  - `if ( *(this + result) != 0 )`
  - `if ( v5 == 54 )`
  - `v6 = (v5 + dword_BED5A0 + 4 * v5 + 4);`
  - `sub_402F85(pvargDest: &v144, pvargSrc: &pvargSrc);`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `LOBYTE(v165) = 1;`
  - `if ( dword_BF14EC == 0 )`
  - `IWzGr2D::CreateLayer(`
  - …（截断 14 行）

### CUIMapTransfer::SetRet  @ 0x83a3c2  (6857 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1730_MAPNAME`, `SP_2951_YOU_CANNOT_TARGET_YOURSELF_R_NFOR_THE_TELEPORT`, `SP_2955_ITS_THE_MAP_YOURE_CURRENTLY_ON`, `SP_2959_WILL_YOU_TELEPORT_TO_THIS_MAP_R_N_S_`, `SP_807___THE_CHARACTER_NAME_IS_INVALID`
  - `void __thiscall CUIMapTransfer::SetRet(CUIMapTransfer *this, int a2)`
  - `if ( a2 == 1 )`
  - `v3 = *(this + 44);`
  - `if ( v3 >= 0 )`
  - `if ( v3 >= (*(this + 60) != 0 ? 10 : 5) )`
  - `field = get_field();`
  - `Data = TSecType<unsigned long>::GetData(a1: field + 312);`
  - `v19 = *(this + 44);`
  - `if ( *(this + v19 + 46) == Data )`
  - `if ( CUtilDlg::Notice(sMsg: v26, sSndName: v27, ppDialog: v28, bAutoSeparated: v29, bIsTightLine: v30) != 0 )`
  - `v29 = *(this + *(this + 44) + 46);`
  - `LOBYTE(v36) = 3;`
  - `CItemInfo::GetMapString(this: dword_BE78D8, a2: v33, a3: v29, a4: v30);`
  - `ZXString<char>::Format(a1: &v34, a2: v29, a3: v33[0]);`
  - …（截断 14 行）

### CUIMapleTV::GetText  @ 0x7f0dc9  (3739 B)

  - `void __thiscall CUIMapleTV::GetText(int this, int a2, int a3, int a4, int a5, int a6, int a7)`
  - `if ( *(this + 188) != 1 )`
  - `Text = CCtrlEdit::GetText(this: *(this + 176), a2: &i);`
  - `ZXString<char>::operator=(a1: Text);`
  - `if ( i != 0 )`
  - `memset(a1: v18, a2: 0, a3: sizeof(v18));`
  - `memset(a1: v19, a2: 0, a3: sizeof(v19));`
  - `v9 = *(this + 184);`
  - `v10 = *(v9 + 68);`
  - `if ( v10 != 0 )`
  - `a2 = *(v10 - 4);`
  - `if ( a2 > 5 )`
  - `for ( i = 0; i < a2; ++i )`
  - `if ( v11 != 0 )`
  - …（截断 14 行）

### CUIMiniMap::InsertStalkee  @ 0x85bbe4  (353 B)

  - `int __stdcall CUIMiniMap::InsertStalkee(char a1, int a2, int a3, int a4)`
  - `v4 = sub_508EC0(&a1, a2: 0);`
  - `*(v4 + 12) = a3;`
  - `*(v4 + 16) = a4;`
  - `sub_85BDBD(&a1, a2: 0);`
  - `return ZXString<char>::operator=(a1: v6);`

### CUIMiniMap::LoadMiniMap  @ 0x858603  (21963 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1395_N`, `SP_1515_TYPE`, `SP_1751_WIDTH`, `SP_1752_HEIGHT`, `SP_1870_MINIMAP`, `SP_1871_HIDEMINIMAP`, `SP_1872_MINIMAPONOFF`, `SP_1873_CANVAS`, `SP_1874_CENTERX`, `SP_1875_CENTERY`, `SP_1876_MAG`, `SP_1877_LIFE`, `SP_3095_HIDE`…
  - `signed int __thiscall CUIMiniMap::LoadMiniMap(int *this, _DWORD *a2, _DWORD *a3, VARIANTARG **p_p_pvarg)`
  - `v5 = *(this + 379) == 0;`
  - `if ( !v5 )`
  - `sub_4051E5(a1: 0);`
  - `sub_4750BF(a1: this + 380);`
  - `sub_85BD03(a1: this + 381);`
  - `sub_4027BE(a1: a2);`
  - `sub_4027BE(a1: a3);`
  - `*(this + 410) = p_p_pvarg;`
  - `*(this + 413) = 1;`
  - `if ( sub_4F38D3(this: *dword_BE7918) == 0 )`
  - `LOBYTE(v111) = 1;`
  - `sub_40263B(a1: &a2);`
  - `Item = IWzProperty::GetItem(pvargDest: &pvarg, a2: p_pvarg);`
  - …（截断 14 行）

### CUIMiniMap::OnButtonClicked  @ 0x85ace7  (2402 B)

- SP_ 节点：`SP_267_YOU_ARE_CURRENTLY_AT_A_PLACE_WHERE_R_NTHE_WORLD_MAP_IS_NOT_AVAILABLE`, `SP_269_THE_WORLD_MAP_IS_UNAVAILABLE_TO_GUEST_ID_PLAYERS_R_NPLEASE_DOWNLOAD_THE_FULL_CLI`
  - `void __userpurge CUIMiniMap::OnButtonClicked(CUIMiniMap *this@<ecx>, int a2@<ebx>, unsigned int a3)`
  - `switch ( a3 )`
  - `v8 = (this + 1444);`
  - `v9 = *(this + 361) + 1;`
  - `v9 = *(this + 361) - 1;`
  - `CUIMiniMap::ToggleMap(a1: this, a2);`
  - `field = get_field();`
  - `if ( field != 0 )`
  - `CWorldMapDlg::CWorldMapDlg(a1: v16);`
  - `Data = TSecType<unsigned long>::GetData(a1: field + 312);`
  - `if ( CWorldMapDlg::CreateWorldMapDlg(a1: Data, a2: 0) != 0 )`
  - `CDialog::DoModal(a1: v16);`
  - `v6 = *(*dword_BE7918 + 8336) == 0;`
  - `if ( v6 )`
  - …（截断 14 行）

### CUIMiniMap::OnMouseMove  @ 0x85ade2  (22396 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1730_MAPNAME`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_3771_STRING_TOOLTIPHELPIMG_PORTALTOOLTIP_D`
  - `signed int __thiscall CUIMiniMap::OnMouseMove(wchar_t **this, int arg0, int a3)`
  - `int (__thiscall **v10)(wchar_t **); // eax`
  - `int (__thiscall **v44)(wchar_t **); // eax`
  - `int (__thiscall **v70)(wchar_t **); // eax`
  - `sub_85ABBC(a1: this - 1);`
  - `v4 = *(this + 438);`
  - `if ( v4 != 0 )`
  - `v5 = *(v4 - 4);`
  - `if ( v5 != 0 )`
  - `if ( *(this + 427) == 0 )`
  - `if ( v115 == 0 )`
  - `v106 = sub_40B947(a1: v115);`
  - `v8 = sub_40B920(a1: v7);`
  - `dword_BF040C(a1: &pvarg, a2: 0, a3: 0, a4: v8, a5: v106);`
  - …（截断 14 行）

### CUIMiniMap::ReloadMiniMap  @ 0x8590a4  (545 B)

  - `int __thiscall CUIMiniMap::ReloadMiniMap(_DWORD *this)`
  - `v6 = *(this + 410);`
  - `sub_4281C2(a1: this + 406);`
  - `return CUIMiniMap::LoadMiniMap(this, a2: v4, a3: v5, p_p_pvarg: v6);`

### CUIMiniMap::SetCtrl  @ 0x858344  (4547 B)

- SP_ 节点：`SP_1261_UI_BASICIMG_BTMIN`, `SP_1262_UI_BASICIMG_BTMAX`, `SP_1263_UI_UIWINDOWIMG_MINIMAP_BTMAP`
  - `void __usercall CUIMiniMap::SetCtrl(_DWORD *a1@<ecx>, int a2@<ebx>)`
  - `if ( a1[413] != 0 )`
  - `if ( v3 != 0 )`
  - `(*(*v3 + 12))(a1: v3);`
  - `if ( v4 != 0 )`
  - `(*(*v4 + 12))(a1: v4);`
  - `if ( v5 != 0 )`
  - `(*(*v5 + 12))(a1: v5);`
  - `if ( *(*dword_BE7918 + 13960) != 0 )`
  - `LOBYTE(v25) = 1;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `v8 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `if ( v8 != 0 )`
  - `v9 = CCtrlButton::CCtrlButton(this: v8);`
  - …（截断 14 行）

### CUIMiniMap::SetLayer  @ 0x855d02  (97842 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1394_NW`, `SP_1395_N`, `SP_1396_NE`, `SP_1397_E`, `SP_1398_W`, `SP_1399_C`, `SP_1400_SW`, `SP_1401_S`, `SP_1402_SE`, `SP_1410_CANVASFONT`, `SP_1411_BA`, `SP_1445_TITLE`, `SP_1448_MIN`…
  - `void __thiscall CUIMiniMap::SetLayer(int *this)`
  - `v2 = *(this + 413) == 0;`
  - `if ( v2 )`
  - `v346 = *(this + 361);`
  - `sub_402FAB(a1: -2, a2: 3);`
  - `Layer = CWnd::GetLayer(a1: &v347);`
  - `LOBYTE(v357) = 1;`
  - `IWzGr2DLayer::RemoveCanvas(this: *Layer, a2: &v356, a3: &pvarg);`
  - `sub_4156AB(a1: &v356);`
  - `sub_4156C5(a1: &v347);`
  - `if ( v4 < 0 )`
  - `if ( *(*dword_BE7918 + 13960) != 0 )`
  - `v334 = *(this + 405);`
  - `sub_415691(a1: &v334);`
  - …（截断 14 行）

### CUIMiniMap::SetShowMiniMap  @ 0x85bc51  (132 B)

  - `int __thiscall CUIMiniMap::SetShowMiniMap(CUIMiniMap *this, int a2)`
  - `*(this + 413) = a2;`

### CUIMiniMap::ToggleMap  @ 0x8590df  (215 B)

  - `void __usercall CUIMiniMap::ToggleMap(int *a1@<ecx>, int a2@<ebx>)`
  - `CUIMiniMap::SetLayer(this: a1);`
  - `CUIMiniMap::SetCtrl(a1, a2);`
  - `CWnd::InvalidateRect(this: a1, a2: 0);`

### CUIMiniMap::Update  @ 0x859122  (49195 B)

  - `void __thiscall CUIMiniMap::Update(int *this)`
  - `v2 = *(this + 5) == 0;`
  - `if ( !v2 && TSingleton<CUserLocal>::ms_pInstance != 0 )`
  - `if ( *(this + 361) == 2 || *(this + 413) == 0 )`
  - `CWnd::Update(this);`
  - `if ( ++dword_BF0F64 % 4 == 0 && *(*dword_BE7918 + 13960) != 0 )`
  - `v4 = *(TSingleton<CUserLocal>::ms_pInstance + 4);`
  - `v238 = (TSingleton<CUserLocal>::ms_pInstance + 4);`
  - `*(this + 373) = *(*(v4 + 16))(a1: TSingleton<CUserLocal>::ms_pInstance + 4);`
  - `*(this + 374) = *((*(*v3 + 16))(a1: v3) + 4);`
  - `sub_85ABBC(a1: this);`
  - `CWnd::GetCanvas(a1: &v250);`
  - `sub_402FAB(a1: 1, a2: 3);`
  - `LOBYTE(v254) = 1;`
  - …（截断 14 行）

### CUIToolTip::SetToolTip_WorldMap  @ 0x8e7434  (8259 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_4110_PARTY_MEMBER__S`
  - `void __thiscall CUIToolTip::SetToolTip_WorldMap(`
  - `if ( sub_444F15(a1: 0) == 0 && *(this + 5) == a2 && *(this + 6) == a3 )`
  - `LOBYTE(v41) = 1;`
  - `if ( a4 != 0 )`
  - `if ( a5 != 0 )`
  - `if ( a6 != 0 )`
  - `if ( a4 != 0 && *a4 != 0 || a5 != 0 && *a5 != 0 )`
  - `if ( a5 != 0 && *a5 != 0 )`
  - `FontByType = CUIToolTip::GetFontByType(this, a2: &v38, a3: 1);`
  - `sub_4288D7(a1: *FontByType);`
  - `if ( v38 != 0 )`
  - `(*(*v38 + 8))(a1: v38);`
  - `VariantInit(&pvarg);`
  - `v14 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - …（截断 14 行）

### CUser::SetMapSpecificEffect  @ 0x93cc16  (3395 B)

  - `void __thiscall CUser::SetMapSpecificEffect(CUser *this)`
  - `if ( *(this + 346) == 0 )`
  - `field = get_field();`
  - `if ( field != 0 )`
  - `sub_93CD98(a1: &v21);`
  - `if ( v21 != 0 && *v21 != 0 )`
  - `AdditionalLayer = CUser::GetAdditionalLayer(this, a2: 9);`
  - `*(AdditionalLayer + 2) = TSecType<unsigned long>::GetData(a1: field + 312);`
  - `v16 = (AdditionalLayer + 12);`
  - `v4 = sub_4281FB(a1: this + 4432);`
  - `Flip = IWzGr2DLayer::GetFlip(this: v4);`
  - `v5 = sub_41468C(a1: &v21);`
  - `_bstr_t::_bstr_t(a1: v5);`
  - `if ( CUser::LoadLayer(this, bsUOL: v14, bLeft: Flip, l: v16, pnRepeatStartIndex: v17) != 0 )`
  - …（截断 14 行）

### CUserLocal::UseFuncKeyMapped  @ 0x94f2ed  (11369 B)

- 分支 case：`3`, `5`, `6`, `7`, `212`, `226`, `524`, `530`, `301`, `227`
- 节流 `CanSendExclRequest(200)`
  - `signed int __thiscall CUserLocal::UseFuncKeyMapped(CUserLocal *this, unsigned int a2)`
  - `if ( BYTE2(a2) == 82 && (a2 & 0x1000000) == 0 )`
  - `if ( CUser::IsDarkSight(this) || CUser::IsSneak(this) || (*(this + 348) & 0xFFFFFFFE) == 0x12 )`
  - `v3 = CUserLocal::IsStun(this) == 0;`
  - `if ( BYTE2(a2) == 54 )`
  - `v4 = BYTE2(a2);`
  - `if ( *v5 == 0 || *v5 != 8 && *(v5 + 1) == 0 )`
  - `v43 = *(CWvsContext::GetCharacterData(this: v45, a2: &v35) + 4);`
  - `if ( v36 != 0 )`
  - `if ( *v5 != 8 && dword_BEC1F4 != 0 && *(dword_BEC1F4 + 12) != 0 )`
  - `sub_631A80(this: dword_BEC1F4);`
  - `if ( *v5 == 1 )`
  - `if ( *(dword_BEC1F8 + 60) == 0`
  - `&& !CUserLocal::IsImmovable(this)`
  - …（截断 14 行）

### CVecCtrl::BoundPosMapRange  @ 0x9b45c1  (3766 B)

  - `int __thiscall CVecCtrl::BoundPosMapRange(CVecCtrl *this, const struct AbsPos *a2, int a3)`
  - `v4 = (this + 32);`
  - `v26 = _ZtlSecureFuse<double>(a1: this + 32, a2: *(this + 12));`
  - `v34 = v26 - _ZtlSecureFuse<double>(a1: a2, a2: *(a2 + 4));`
  - `v27 = _ZtlSecureFuse<double>(a1: (v4 + 6), a2: v4[10]);`
  - `v28 = v27 - _ZtlSecureFuse<double>(a1: a2 + 24, a2: *(a2 + 10));`
  - `v6 = _ZtlSecureFuse<double>(a1: v4, a2: v4[4]);`
  - `v29 = *(this + 78);`
  - `if ( v6 < v29 )`
  - `v7 = _ZtlSecureTear<double>(a1: SLOBYTE(v20), a2: HIDWORD(COERCE_UNSIGNED_INT64(*(this + 78))));`
  - `v8 = _ZtlSecureTear<double>(a1: SLOBYTE(v21), a2: HIDWORD(COERCE_UNSIGNED_INT64(0.0)));`
  - `if ( v9 )`
  - `v10 = _ZtlSecureFuse<double>(a1: v4, a2: v4[4]);`
  - `v30 = *(this + 80);`
  - …（截断 14 行）

### CWorldMapDlg::CheckLinkInfo  @ 0x9ee00e  (4826 B)

- SP_ 节点：`SP_959_ORIGIN`
  - `int __thiscall CWorldMapDlg::CheckLinkInfo(CWorldMapDlg *this, int *a2, int a3)`
  - `v5 = sub_414576(a1: this + 1452);`
  - `Property = IWzCanvas::GetProperty(this: v5, a2: &v34);`
  - `LOBYTE(v41) = 2;`
  - `sub_40263B(a1: Property);`
  - `Item = IWzProperty::GetItem(pvargDest: &pvarg, a2: v26[0]);`
  - `unknown = get_unknown(a1: v35, a2: Item);`
  - `sub_41A4AF(a1: unknown);`
  - `sub_4027B1(a1: v35);`
  - `if ( v9 < 0 )`
  - `sub_4027E9(a1: &v34);`
  - `for ( i = 0; ; i += 12 )`
  - `v10 = *(this + 367);`
  - `if ( v10 == 0 || v40 >= *(v10 - 4) )`
  - …（截断 14 行）

### CWorldMapDlg::CheckSpotInfo  @ 0x9edd67  (5067 B)

- SP_ 节点：`SP_1796_S__S`
  - `int __thiscall CWorldMapDlg::CheckSpotInfo(CWorldMapDlg *this, int a2, int a3)`
  - `for ( i = 0; ; i += 17 )`
  - `v4 = *(this + 366);`
  - `if ( v4 == 0 || v40 >= *(v4 - 4) )`
  - `v5 = (i + v4);`
  - `v6 = abs(a1: *(i + v4) - a2 + 13);`
  - `v8 = abs(a1: v7 - (a3 - 35));`
  - `v9 = *(this + 365);`
  - `v10 = (v9 + 4 * v5[2]);`
  - `if ( *v10 == 0 )`
  - `v11 = sub_40B920(a1: *v10) / 3;`
  - `if ( v34 <= v11 && v33 <= v11 )`
  - `if ( v12 != 0 && *v12 != 0 )`
  - `LOBYTE(v42) = 4;`
  - …（截断 14 行）

### CWorldMapDlg::ClearToolTip  @ 0x9e98b7  (124 B)

  - `_DWORD *__thiscall CWorldMapDlg::ClearToolTip(CWorldMapDlg *this)`
  - `return CUIToolTip::ClearToolTip(this: (this + 132));`

### CWorldMapDlg::CreateWorldMapDlg  @ 0x9eb366  (7786 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_2277_MAP_MAPHELPERIMG_WORLDMAP_MAPIMAGE`, `SP_4267__SIGNIFICANT_OTHER_`
  - `int __userpurge CWorldMapDlg::CreateWorldMapDlg@<eax>(CWorldMapDlg *this@<ecx>, unsigned int a2, int i, int a4)`
  - `*(this + 359) = a2;`
  - `*(this + 368) = i;`
  - `sub_402F85(&pvargDest, pvargSrc: v41);`
  - `sub_402F85(pvargDest: &v45, pvargSrc: &pvargSrc);`
  - `LOBYTE(v57) = 1;`
  - `v7 = sub_406472();`
  - `v8 = sub_402680(a1: v7);`
  - `IWzResMan::GetObjectA(this: v8, pvargDest: &pvarg, sUOL: v37, vParam: v38, vAux: p_pvargDest);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v40, a2: v41);`
  - `if ( Unknown != 0 )`
  - `v9 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v9 < 0 && v9 != -2147467262 )`
  - `if ( v10 < 0 )`
  - …（截断 14 行）

### CWorldMapDlg::Draw  @ 0x9eb9c6  (10777 B)

- SP_ 节点：`SP_959_ORIGIN`
  - `int __thiscall CWorldMapDlg::Draw(CWorldMapDlg *this, struct tagRECT *a2)`
  - `CWnd::Draw(this, a2);`
  - `CWnd::GetCanvas(a1: &a2);`
  - `sub_402FAB(a1: 255, a2: 3);`
  - `v64 = *(v2 + 363);`
  - `LOBYTE(v69) = 1;`
  - `if ( a2 == 0 )`
  - `v4 = (*(left + 128))(`
  - `if ( v4 < 0 )`
  - `if ( v5 < 0 )`
  - `v7 = (*(v6 + 128))(`
  - `if ( v7 < 0 )`
  - `if ( v8 < 0 )`
  - `v10 = sub_414576(a1: v2 + 1452);`
  - …（截断 14 行）

### CWorldMapDlg::HitTest  @ 0x9eb98e  (390 B)

  - `int __thiscall CWorldMapDlg::HitTest(CWorldMapDlg *this, int a2, unsigned int a3, struct CCtrlWnd **a4)`
  - `result = CWnd::HitTest(a1: a2, a2: a3, a3: a4);`
  - `if ( result != 0 && (a4 == 0 || *a4 == 0) )`
  - `if ( a3 > 0x18 )`

### CWorldMapDlg::LoadMapList  @ 0x9ed38b  (21758 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1445_TITLE`, `SP_1447_DESC`, `SP_1515_TYPE`, `SP_1729_STREETNAME`, `SP_1730_MAPNAME`, `SP_1803_PATH`, `SP_2291_MAPLIST`, `SP_2292_MAPNO`, `SP_2293_MAPDESC`, `SP_4038_SPOT`, `SP_959_ORIGIN`
  - `int __thiscall CWorldMapDlg::LoadMapList(_DWORD *this, int a2)`
  - `for ( i = 0; ; i += 68 )`
  - `if ( *v3 == 0 || v2 >= *(v4 - 4) )`
  - `sub_9EDD3E(a1: v4 + i);`
  - `sub_9EE882(a1: v3);`
  - `LOBYTE(v128) = 0;`
  - `sub_40263B(a1: &a2);`
  - `IWzProperty::GetItem(pvargDest: &pvarg, a2: v86);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: p_pvargDest, a2: v88[0]);`
  - `if ( Unknown != 0 )`
  - `v8 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v8 < 0 && v8 != -2147467262 )`
  - `if ( v9 < 0 )`
  - `if ( v125 != 0 && IWzProperty::Getcount(a1: v125) != 0 )`
  - …（截断 14 行）

### CWorldMapDlg::MakeBorder  @ 0x9ebfc3  (26557 B)

- SP_ 节点：`SP_2281_UI_UIWINDOWIMG_WORLDMAP_BORDER_7`, `SP_2282_UI_UIWINDOWIMG_WORLDMAP_BORDER_6`, `SP_2283_UI_UIWINDOWIMG_WORLDMAP_BORDER_5`, `SP_2284_UI_UIWINDOWIMG_WORLDMAP_BORDER_4`, `SP_2285_UI_UIWINDOWIMG_WORLDMAP_BORDER_3`, `SP_2286_UI_UIWINDOWIMG_WORLDMAP_BORDER_2`, `SP_2287_UI_UIWINDOWIMG_WORLDMAP_BORDER_1`, `SP_2288_UI_UIWINDOWIMG_WORLDMAP_BORDER_0`, `SP_956_CANVAS`
  - `int __thiscall CWorldMapDlg::MakeBorder(ULONG *this)`
  - `PcCreateObject::IWzCanvas(a1: v3, a2: v4, a3: 0);`
  - `if ( v135 != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v138) = 2;`
  - `if ( v5 )`
  - `v10 = (*(v9 + 44))(`
  - `if ( v10 < 0 )`
  - `if ( v11 < 0 )`
  - `if ( v12 < 0 )`
  - `v14 = (v137 + 26);`
  - `if ( v137[26] == 0 )`
  - `v17 = (*(v16 + 140))(a1: v15, a2: 0, a3: 0, a4: v128.decVal.Hi32, a5: Unknown, a6: 0xFFFFFF);`
  - …（截断 14 行）

### CWorldMapDlg::OnButtonClicked  @ 0x424492  (249 B)

  - `void __thiscall CWorldMapDlg::OnButtonClicked(CWorldMapDlg *this, unsigned int a2)`
  - `if ( a2 == 1 )`
  - `(*(*this + 52))(a1: 1);`
  - `else if ( a2 == 2 )`

### CWorldMapDlg::OnCreate  @ 0x9eb75b  (4054 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1240_WORLDMAPOPEN`, `SP_2289_UI_UIWINDOW_WORLDMAP_TITLE`, `SP_5577_UI_BASICIMG_BTCLOSE`
  - `void __thiscall CWorldMapDlg::OnCreate(CWorldMapDlg *this, void *a2)`
  - `CWorldMapDlg::MakeBorder(this);`
  - `sub_9ECC4A(a1: 12, a2: 34, a3: 642, a4: 472);`
  - `LOBYTE(v28) = 1;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `v5 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `if ( v5 != 0 )`
  - `v6 = CCtrlButton::CCtrlButton(this: v5);`
  - `sub_4284FF(a1: v6);`
  - `(*(**(this + 33) + 32))(a1: *(this + 33), a2: this, a3: 2, a4: *(this + 9) - 20, a5: 6, a6: 0, a7: v25);`
  - `VariantInit(&pvarg);`
  - `v7 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v7 < 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - …（截断 14 行）

### CWorldMapDlg::OnDestroy  @ 0x9eb94a  (528 B)

- SP_ 节点：`SP_1241_WORLDMAPCLOSE`
  - `void __thiscall CWorldMapDlg::OnDestroy(CWorldMapDlg *this)`
  - `play_ui_sound(a1: v2);`

### CWorldMapDlg::OnKey  @ 0x9ee717  (731 B)

  - `void __thiscall CWorldMapDlg::OnKey(CWorldMapDlg *this, unsigned int a2, signed int a3)`
  - `if ( a3 >= 0 )`
  - `v3 = BYTE2(a3);`
  - `if ( BYTE2(a3) == 54 )`
  - `if ( *(v3 + dword_BED5A0 + 4 * v3 + 4 + 1) == 5 )`
  - `(*(*(this - 1) + 52))(a1: this - 4, a2: 1);`
  - `if ( a2 == 27 )`
  - `v4 = *(this + 30);`
  - `if ( v4 != 0 && *v4 != 0 )`
  - `CWorldMapDlg::SetWorldMapShallower(this: (this - 4));`
  - `CDialog::OnKey(this, a2, a3);`

### CWorldMapDlg::OnMouseButton  @ 0x9ee6e5  (419 B)

  - `void __thiscall CWorldMapDlg::OnMouseButton(CWorldMapDlg *this, unsigned int a2, unsigned int a3, int a4, int a5)`
  - `if ( a2 == 514 )`
  - `CWorldMapDlg::SetWorldMapDeeper(this: (this - 4));`
  - `else if ( a2 == 517 )`
  - `CWorldMapDlg::SetWorldMapShallower(this: (this - 4));`
  - `CUIToolTip::ClearToolTip(this: (this + 132));`

### CWorldMapDlg::OnMouseEnter  @ 0x9ee2ec  (285 B)

  - `_DWORD *__thiscall CWorldMapDlg::OnMouseEnter(CWorldMapDlg *this, int a2)`
  - `result = CWnd::OnMouseEnter(a1: a2);`
  - `if ( a2 == 0 )`
  - `return CUIToolTip::ClearToolTip(this: (this + 132));`

### CWorldMapDlg::OnMouseMove  @ 0x9ee2b3  (385 B)

  - `int __thiscall CWorldMapDlg::OnMouseMove(CWorldMapDlg *this, int a2, int a3)`
  - `v3 = (this - 4);`
  - `v4 = CWorldMapDlg::CheckSpotInfo(this: (this - 4), a2, a3);`
  - `if ( (v4 | CWorldMapDlg::CheckLinkInfo(this: v3, a2, a3)) != 0 )`
  - `CWnd::InvalidateRect(this: v3, a2: 0);`

### CWorldMapDlg::SetWorldMap  @ 0x9ea93b  (25867 B)

- SP_ 节点：`SP_2290_BASEIMG_0`
  - `int __fastcall CWorldMapDlg::SetWorldMap(int a1, int a2, int a3)`
  - `HIDWORD(v90) = a1;`
  - `LOBYTE(v128) = 0;`
  - `sub_40263B(a1: &a3);`
  - `IWzProperty::GetItem(pvargDest: &v103, a2: SHIDWORD(v90));`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v91[0], a2: SBYTE4(v91[0]));`
  - `v5 = sub_41E527(a1: &Unknown);`
  - `if ( v5 < 0 && v5 != -2147467262 )`
  - `if ( v6 < 0 )`
  - `if ( *(v3 + 1452) != 0 )`
  - `HIDWORD(v91[0]) = a3;`
  - `sub_415691(a1: v91 + 4);`
  - `if ( sub_9ECE30(a1: HIDWORD(v91[0])) != 0 )`
  - `if ( CWorldMapDlg::LoadMapList(a1: HIDWORD(v91[0])) != 0 )`
  - …（截断 14 行）

### CWorldMapDlg::SetWorldMapDeeper  @ 0x9ee30e  (4526 B)

- SP_ 节点：`SP_1242_SELECTMAP`, `SP_2276_MAP_WORLDMAP_SIMG`
  - `void __thiscall CWorldMapDlg::SetWorldMapDeeper(CWorldMapDlg *this)`
  - `v2 = *(this + 361);`
  - `if ( v2 != -1 )`
  - `v3 = *(this + 367) + 12 * v2;`
  - `v4 = *(v3 + 4);`
  - `if ( v4 != 0 && *v4 != 0 )`
  - `v22 = *(v3 + 4);`
  - `LOBYTE(v33) = 2;`
  - `sub_416821(a1: &v30, a2: v21, a3: v22);`
  - `VariantInit(&pvarg);`
  - `v7 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v7 < 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `v8 = sub_41468C(a1: &v30);`
  - …（截断 14 行）

### CWorldMapDlg::SetWorldMapShallower  @ 0x9ee507  (4230 B)

- SP_ 节点：`SP_1242_SELECTMAP`, `SP_2276_MAP_WORLDMAP_SIMG`
  - `void __thiscall CWorldMapDlg::SetWorldMapShallower(CWorldMapDlg *this)`
  - `v2 = *(this + 31);`
  - `if ( v2 != 0 && *v2 != 0 )`
  - `v20 = *(this + 31);`
  - `LOBYTE(v31) = 2;`
  - `sub_416821(a1: &v28, a2: v19, a3: v20);`
  - `VariantInit(&pvarg);`
  - `v5 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v5 < 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `v6 = sub_41468C(a1: &v28);`
  - `_bstr_t::_bstr_t(a1: v6);`
  - `v8 = sub_406472();`
  - `v9 = sub_402680(a1: v8);`
  - …（截断 14 行）

### CWvsContext::OnMapTransferResult  @ 0xa25268  (5190 B)

- s2c 解码：`Decode1`, `Decode4`
- 分支 case：`2`, `3`, `5`, `6`, `7`, `8`, `9`, `10`, `11`
- SP_ 节点：`SP_2949_S_IS_CURRENTLY_DIFFICULT_TO_LOCATE_SO_R_NTHE_TELEPORT_WILL_NOT_TAKE_PLACE`, `SP_2952_THIS_MAP_IS_NOT_AVAILABLE_TO_ENTER_FOR_THE_LIST`, `SP_2955_ITS_THE_MAP_YOURE_CURRENTLY_ON`, `SP_2956_USERS_BELOW_LEVEL_7_ARE_NOT_ALLOWED__R_NTO_GO_OUT_FROM_MAPLE_ISLAND`, `SP_2984_YOU_CANNOT_GO_TO_THAT_PLACE`
  - `void __thiscall CWvsContext::OnMapTransferResult(CWvsContext *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `v4 = CInPacket::Decode1(this: a2);`
  - `switch ( v3 )`
  - `v5 = *(v2 + 2094);`
  - `v6 = (v5 + 1399);`
  - `if ( v4 == 0 )`
  - `for ( i = v4 != 0 ? 10 : 5; i != 0; --i )`
  - `*v7++ = CInPacket::Decode4(this: a2);`
  - `if ( *dword_BF0F0C != 0 )`
  - `sub_83A6CE(a1: v6, a2: 1);`
  - `LOBYTE(v37) = 2;`
  - `ZXString<char>::Format(a1: &v36, a2: v22, a3: v27);`
  - `v24 = *(v2 + 3517);`
  - …（截断 14 行）

### CWvsContext::OnMiniMapOnOff  @ 0xa129ed  (218 B)

- s2c 解码：`Decode1`
  - `int __thiscall CWvsContext::OnMiniMapOnOff(CWvsContext *this, struct CInPacket *a2)`
  - `*(this + 3582) = CInPacket::Decode1(this: a2);`
  - `return CUIMiniMap::ReloadMiniMap(this: dword_BED788);`

### CWvsContext::OnNotifyMarriedPartnerMapTransfer  @ 0xa0831d  (314 B)

- s2c 解码：`Decode4`
  - `int __thiscall CWvsContext::OnNotifyMarriedPartnerMapTransfer(CWvsContext *this, struct CInPacket *a2)`
  - `*(this + 3075) = CInPacket::Decode4(this: a2);`
  - `result = CInPacket::Decode4(this: a2);`
  - `*(this + 3076) = result;`

### CWvsContext::UseFuncKeyMapped  @ 0xa0773d  (6745 B)

- 分支 case：`0`, `1`, `2`, `3`, `4`, `5`, `6`, `7`, `8`, `9`, `14`, `15`, `16`, `17`, `19`, `20`, `22`, `23`, `25`, `26`, `27`
- SP_ 节点：`SP_267_YOU_ARE_CURRENTLY_AT_A_PLACE_WHERE_R_NTHE_WORLD_MAP_IS_NOT_AVAILABLE`
  - `signed int __thiscall CWvsContext::UseFuncKeyMapped(_DWORD *this, signed int a2)`
  - `v2 = BYTE2(a2);`
  - `if ( BYTE2(a2) == 54 )`
  - `v4 = *(v2 + v3 + 5);`
  - `if ( a2 < 0 )`
  - `if ( v10 != 0 )`
  - `if ( v11 == 0 )`
  - `if ( TSingleton<CUIStatusBar>::ms_pInstance != 0 )`
  - `CWndMan::ClearComposition(a1: TSingleton<CWndMan>::ms_pInstance);`
  - `CUIStatusBar::ChangeWhisperTarget(this: TSingleton<CUIStatusBar>::ms_pInstance);`
  - `if ( v12 != 0 )`
  - `if ( v13 != 0 )`
  - `if ( v14 != 0 )`
  - `if ( v15 != 0 )`
  - …（截断 14 行）

### FUNCKEY_MAPPED::Decode  @ 0x4e4409  (138 B)

- s2c 解码：`DecodeBuffer`
  - `void *__thiscall FUNCKEY_MAPPED::Decode(void *this, int a2)`
  - `return CInPacket::DecodeBuffer(this: a2, a2: this, a3: 5u);`

## 2. 反编译中出现的 c2s opcode 常量汇总（COutPacket 构造参数）

| opcode (dec) | hex | 来源函数 |
| --- | --- | --- |
| 135 | 0x87 | CFuncKeyMappedMan::SaveFuncKeyMap |
| 183 | 0xB7 | CQuickslotKeyMappedMan::SaveQuickslotKeyMap |
