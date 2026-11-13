# Angel 083 NPC 面反编译语义抽取（观察层）

来源：`evidence/angel-npc-decompile.json`（全量函数逐字 Hex-Rays 伪代码，来自 Angel 13339 IDB）。
本文件由 `tools/ida_angel_npc_facts.py` 自动生成，只做字符串抽取与归类，不含推断。

## 1. 逐函数关键信号（opcode / Encode / Decode / 分支 / SP_ 节点 / 数值调用）


### <free>::0CNpc  @ 0x6d045a  (2606 B)

  - `CNpc *__userpurge CNpc::CNpc@<eax>(CNpc *this@<ecx>, struct CNpcTemplate *a2, unsigned int a3)`
  - `CLife::CLife(this);`
  - `*(this + 35) = 0;`
  - `*(this + 44) = -1;`
  - `*(this + 42) = a2;`
  - `v4 = *(a2 + 24);`
  - `LOBYTE(v15) = 3;`
  - `if ( v4 != 0 )`
  - `v5 = *(v4 - 4);`
  - `sub_6D602C(this: this + 47, a2: v5 + 2, a3: &v14);`
  - `v6 = *(a2 + 24);`
  - `if ( v6 != 0 )`
  - `v7 = *(v6 - 4);`
  - `sub_6D602C(this: this + 78, a2: v7 + 2, a3: &v14);`
  - …（截断 14 行）

### <free>::1CNpc  @ 0x6d067f  (2667 B)

  - `int __thiscall CNpc::~CNpc(CNpc *this)`
  - `*(this - 2) = &off_AFC9D8;`
  - `*(this - 1) = &off_AFC9B4;`
  - `sub_6725B6(a1: this + 320);`
  - `sub_6D5FEE(a1: this + 304);`
  - `if ( *(this + 69) != 0 )`
  - `sub_428C15(a1: 0);`
  - `*(this + 69) = 0;`
  - `v2 = *(this + 66);`
  - `if ( v2 != 0 )`
  - `(*(*v2 + 8))(a1: v2);`
  - `v3 = *(this + 65);`
  - `if ( v3 != 0 )`
  - `(*(*v3 + 8))(a1: v3);`
  - …（截断 14 行）

### <free>::CreateNpc  @ 0x6d041e  (310 B)

  - `CNpc *__cdecl CreateNpc(struct CNpcTemplate *a1)`
  - `v1 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 384);`
  - `if ( v1 != 0 )`
  - `return CNpc::CNpc(this: v1, a2: a1, a3: v1);`

### <free>::_GCNpc  @ 0x6d065b  (243 B)

  - `int __thiscall CNpc::`scalar deleting destructor'(CNpc *this, char a2)`
  - `CNpc::~CNpc(this);`
  - `if ( (a2 & 1) != 0 )`
  - `ZAllocEx<ZAllocAnonSelector>::Free(a1: this - 8);`

### ?$ZList::AddTail  @ 0x6d9c1a  (511 B)

  - `int __thiscall ZList<CNpcPool::NPCENTRY>::AddTail(_DWORD *this)`
  - `v2 = *(this + 4);`
  - `result = sub_6DC980(a1: 0);`
  - `*(result + 4) = 0;`
  - `*(result + 8) = v2;`
  - `++*(this + 2);`
  - `v4 = *(this + 4);`
  - `if ( v4 != 0 )`
  - `*(v4 + 4) = result;`
  - `*(this + 3) = result;`

### ?$ZList::RemoveAt  @ 0x6d9c48  (792 B)

  - `int __thiscall ZList<CNpcPool::NPCENTRY>::RemoveAt(_DWORD *this, int a2)`
  - `v2 = *(a2 + 8);`
  - `if ( v2 != 0 )`
  - `v4 = *(a2 + 4);`
  - `if ( v4 != 0 )`
  - `*(v2 + 4) = v4;`
  - `*(*(a2 + 4) + 8) = *(a2 + 8);`
  - `*(this + 4) = v2;`
  - `*(*(a2 + 8) + 4) = 0;`
  - `v3 = *(a2 + 4);`
  - `if ( v3 != 0 )`
  - `*(this + 3) = v3;`
  - `*(this + 4) = 0;`
  - `--*(this + 2);`
  - …（截断 14 行）

### CActionMan::LoadNpcAction  @ 0x40db31  (10146 B)

- SP_ 节点：`SP_1147_ZIGZAG`, `SP_5624_DELAY`
  - `int __thiscall CActionMan::LoadNpcAction(char *this, int a2, int a3, int a4, StringPoolStrings *a5)`
  - `v7 = a3 | (*a2 << 8);`
  - `if ( sub_4187FD(a1: v54, a2: v63) != 0 )`
  - `v9 = dword_BF060C(a1: v46);`
  - `*(v8 + 40) = v9;`
  - `sub_416BEA(a1: v10);`
  - `sub_41577E(a1: v11 + 20);`
  - `return sub_415BAB(a1: v63);`
  - `v12 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 52);`
  - `LOBYTE(v69) = 1;`
  - `if ( v12 != 0 )`
  - `v13 = sub_40E0A3(a1: v12);`
  - `sub_415B99(a1: v67);`
  - `*(v68 + 12) = v5;`
  - …（截断 14 行）

### CAdminShopDlg::GetCurNPCItem  @ 0x426504  (239 B)

  - `_DWORD *__thiscall CAdminShopDlg::GetCurNPCItem(_DWORD *this)`
  - `if ( *(*(this + 39) + 60) == 0 )`

### CAdminShopDlg::SetNPC  @ 0x426636  (9851 B)

  - `int __thiscall CAdminShopDlg::SetNPC(char *this)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `LOBYTE(v57) = 1;`
  - `if ( dword_BF14EC == 0 )`
  - `v2 = IWzGr2D::CreateLayer(`
  - `v3 = (this + 196);`
  - `sub_428712(a1: *v2);`
  - `sub_4156C5(a1: &v55);`
  - `if ( v4 < 0 )`
  - `if ( v5 < 0 )`
  - `v6 = *CWnd::GetLayer(a1: &v54);`
  - `sub_410FDF(this: &pvargDest, a2: v6, a3: 1);`
  - `if ( v7 )`
  - …（截断 14 行）

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

### CNpc::GenerateMovePath  @ 0x6d3273  (1649 B)

- **c2s opcode `COutPacket(197)`**
- c2s 编码：`Encode4`, `Encode1`
  - `int __thiscall CNpc::GenerateMovePath(int *this, int a2, char a3)`
  - `v4 = *(this + 37) != 0 ? *(this + 37) - 12 : 0;`
  - `if ( *(*(this + 42) + 12) != 0 )`
  - `if ( a2 != -1 )`
  - `v6 = *(CNpc::GetActionFrameList(this, a2) + 12);`
  - `while ( v6 != 0 )`
  - `v7 = *(v6 + 4);`
  - `v8 = *(v6 != 0 ? v6 - 16 + 4 : 4);`
  - `v5 += *(v7 + 16);`
  - `CVecCtrlNpc::UpdateActiveInterrupted(this: v4, a2: v5, a3: 0);`
  - `for ( i = 0; i < 55; ++i )`
  - `if ( CMovePath::IsTimeForFlush(this: (v4 + 428), a2: 0) != 0 )`
  - `CVecCtrl::UpdateActive(this: v4);`
  - `COutPacket::COutPacket(a1: 197);`
  - …（截断 14 行）

### CNpc::GetActionFrameList  @ 0x6d5eeb  (711 B)

  - `int __thiscall CNpc::GetActionFrameList(int *this, signed int a2)`
  - `v2 = *(this + 79);`
  - `if ( v2 >= 0 )`
  - `v3 = *(this + 78);`
  - `v5 = v2 != *(this + 80) && v2 >= 0;`
  - `if ( *(v4 + 8) == 0 || v5 )`
  - `CActionMan::LoadNpcAction(this: dword_BE78D4, a2: *(this + 42), a3: a2, a4: v4, a5: *(this + 79));`
  - `if ( *(v4 + 8) == 0 )`
  - `_CxxThrowException(a1: &a2, a2: &_TI3_AVCTerminateException__);`

### CNpc::GetCurrentAction  @ 0x6d263e  (406 B)

  - `int __thiscall CNpc::GetCurrentAction(CNpc *this, int *a2)`
  - `v3 = *(this + 43);`
  - `if ( a2 != 0 )`
  - `*a2 = *(this + 43) & 1;`
  - `if ( v3 >> 1 != 2 )`
  - `if ( sub_6D21A6(a1: this) != 0 )`
  - `return *(this + 44);`

### CNpc::GetDCRange  @ 0x6d3fde  (411 B)

  - `char *__thiscall CNpc::GetDCRange(CNpc *this)`
  - `v1 = *(this + 42);`
  - `v2 = *(this + 84);`
  - `*(this + 88) = v2 + v1[8];`
  - `v3 = *(this + 85);`
  - `*(this + 89) = v3 + v1[9];`

### CNpc::GetPos  @ 0x6d060e  (79 B)

  - `char *__thiscall CNpc::GetPos(CNpc *this)`

### CNpc::GetPosPrev  @ 0x6d0615  (83 B)

  - `char *__thiscall CNpc::GetPosPrev(CNpc *this)`

### CNpc::GetQuestDCRange  @ 0x6d4029  (834 B)

  - `_DWORD *__thiscall CNpc::GetQuestDCRange(CNpc *this)`
  - `v2 = (this + 368);`
  - `dword_BF0410(a1: this + 368);`
  - `if ( *(this + 59) != 0 )`
  - `*v2 = *(this + 84) + sub_425AEF(a1: *(this + 59));`
  - `if ( *(this + 59) == 0 )`
  - `*(this + 93) = *(this + 85) + sub_425B16(a1: *(this + 59));`
  - `*(this + 94) = *v2 + IWzGr2DLayer::GetWidth(a1: *(this + 59));`
  - `*(this + 95) = *(this + 93) + IWzGr2DLayer::GetHeight(a1: *(this + 59));`

### CNpc::GetRTTI  @ 0x6d0634  (82 B)

  - `int *__thiscall CNpc::GetRTTI(CNpc *this)`

### CNpc::GetShoeAttr  @ 0x6d2ddd  (256 B)

  - `int __thiscall CNpc::GetShoeAttr(_DWORD *this, int a2)`
  - `v2 = *(this + 82);`
  - `*(a2 + 4) = v2;`
  - `if ( v2 != 0 )`
  - `InterlockedIncrement(lpAddend: (v2 + 4));`

### CNpc::GetType  @ 0x6d060a  (69 B)

  - `int __thiscall CNpc::GetType(CNpc *this)`

### CNpc::GetZMass  @ 0x6d061c  (110 B)

  - `int __thiscall CNpc::GetZMass(CNpc *this)`
  - `return *(*(this + 35) != 0 ? *(this + 35) - 12 + 0x134 : 308);`

### CNpc::Init  @ 0x6d089a  (29134 B)

- s2c 解码：`Decode2`, `Decode1`
- SP_ 节点：`SP_5527_ARIAL`, `SP_958_SHAPE2DVECTOR2D`
  - `void __thiscall CNpc::Init(CNpc *this, VARIANTARG *Foothold, struct CInPacket *a3)`
  - `*(this + 41) = Foothold;`
  - `CNpc::SetShoeAttr(this);`
  - `*(this + 86) = CInPacket::Decode2(this: a3);`
  - `*(this + 87) = CInPacket::Decode2(this: v4);`
  - `*(this + 84) = *(this + 86);`
  - `*(this + 43) = CInPacket::Decode1(this: v4);`
  - `v5 = CInPacket::Decode2(this: v4);`
  - `Foothold = CWvsPhysicalSpace2D::GetFoothold(dwSN: v5);`
  - `Instance = CVecCtrlNpc::CreateInstance();`
  - `v6 = _com_ptr_t<_com_IIID<IWzVector2D,&__s_GUID const _GUID_f28bd1ed_3deb_4f92_9eec_10ef5a1c3fb4>>::_QueryInterface<CVecCtrlNpc *>(a1: &Instance);`
  - `if ( v6 < 0 && v6 != -2147467262 )`
  - `v7 = this != 0 ? (this + 4) : 0;`
  - `v8 = *(*(this + 42) + 12) == 0;`
  - …（截断 14 行）

### CNpc::IsKindOf  @ 0x6d063a  (355 B)

  - `signed int __stdcall CNpc::IsKindOf(const struct CRTTI *a1)`
  - `if ( &dword_BEDC4C == 0 )`
  - `while ( v1 != a1 )`
  - `if ( v1 == 0 )`

### CNpc::OnLayerZChanged  @ 0x6d2dbb  (276 B)

  - `char *__thiscall CNpc::OnLayerZChanged(CNpc *this, const struct CVecCtrl *a2)`
  - `if ( (a2 != 0 ? a2 + 12 : 0) == *(this + 35) )`
  - `return CNpc::SetLayerZ(this: (this - 4));`

### CNpc::OnMove  @ 0x6d2e07  (9004 B)

- s2c 解码：`Decode1`
- SP_ 节点：`SP_1644_EFFECT`
  - `void __thiscall CNpc::OnMove(CNpc *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `v4 = CInPacket::Decode1(this: a2);`
  - `if ( v3 == -1 )`
  - `if ( v4 >= 0 )`
  - `v35 = *(*(this + 42) + 92);`
  - `if ( v35 != 0 && v5 < *(v35 - 4) )`
  - `v36 = *sub_6D608A(a1: v5);`
  - `if ( v36 != 0 )`
  - `InterlockedIncrement(lpAddend: (v36 + 8));`
  - `sub_6D271F(a1: v41[0]);`
  - `else if ( v3 >= 0 )`
  - `v6 = *(this + 42);`
  - `v7 = *(v6 + 96);`
  - …（截断 14 行）

### CNpc::OnResolveMoveAction  @ 0x6d2d72  (351 B)

  - `int __thiscall CNpc::OnResolveMoveAction(CNpc *this, int a2, int a3, char a4, const struct CVecCtrl *a5)`
  - `if ( *(*(this + 41) + 12) != 0 )`
  - `if ( a2 != 0 )`
  - `return (a2 < 0) | 2;`
  - `else if ( a2 != 0 )`

### CNpc::OnSetSpecialAction  @ 0x6d590d  (1016 B)

- s2c 解码：`DecodeStr`
  - `void __thiscall CNpc::OnSetSpecialAction(CNpc *this, struct CInPacket *a2)`
  - `if ( *(this + 48) == 0 )`
  - `CInPacket::DecodeStr(this: a2, &a2);`
  - `for ( i = 0; ; ++i )`
  - `v4 = *(*(this + 42) + 96);`
  - `if ( v4 == 0 || i >= *(v4 - 4) )`
  - `if ( *(v4 + 8 * i) != 0 )`
  - `v5 = sub_40B32B(a1: *(v4 + 8 * i));`
  - `if ( v5 == 0 )`
  - `if ( a2 != 0 )`
  - `if ( _strcmpi(a1: v6, a2: v5) == 0 )`
  - `if ( i >= 0 )`
  - `CNpc::GenerateMovePath(this, a2: i + 2, a3: 0);`

### CNpc::OnUpdateLimitedInfo  @ 0x6d59b0  (6410 B)

- s2c 解码：`Decode1`
- SP_ 节点：`SP_4419_EFFECT_MAPEFFIMG_NPCSUMMON`, `SP_4420_EFFECT_MAPEFFIMG_NPCRETURN`
  - `int __thiscall CNpc::OnUpdateLimitedInfo(CNpc *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `*(this + 55) = -1;`
  - `*(this + 77) = v3;`
  - `CNpc::PrepareActionLayer(this);`
  - `sub_581FAA(a1: 0);`
  - `if ( *(this + 77) != 0 )`
  - `CNpc::RestoreLayers(this);`
  - `sub_432C10(a1: 0);`
  - `CAnimationDisplayer::LoadLayer(this: &v29, a2: this + 36);`
  - `LOBYTE(v42) = 7;`
  - `v21 = sub_41468C(a1: StringW);`
  - `v34 = *sub_43E86F(a1: &a2, a2: v21, a3: 0, a4: v29, a5: v30, a6: v31, a7: v32, a8: SHIDWORD(v32), a9: v33, a10: v34);`
  - `v40 = (this + 240);`
  - …（截断 14 行）

### CNpc::PrepareActionLayer  @ 0x6d232a  (5370 B)

  - `int __thiscall CNpc::PrepareActionLayer(_DWORD *this)`
  - `v2 = *(this + 42);`
  - `if ( v2 != 0 && *(v2 + 196) != 0 )`
  - `v3 = *(this + 35);`
  - `if ( v3 != 0 )`
  - `return (*(*v3 + 20))(a1: v3, a2: 6, a3: 100, a4: 0);`
  - `sub_402FAB(a1: -2, a2: 3);`
  - `if ( v5 )`
  - `if ( *(this + 77) == 0 )`
  - `IWzGr2DLayer::RemoveCanvas(this: *v4, a2: &v31, a3: &pvarg);`
  - `sub_4156AB(a1: &v31);`
  - `if ( result < 0 )`
  - `CurrentAction = CNpc::GetCurrentAction(a1: &v29);`
  - `ActionFrameList = CNpc::GetActionFrameList(this, a2: CurrentAction);`
  - …（截断 14 行）

### CNpc::RestoreLayers  @ 0x6d5c9a  (5399 B)

- SP_ 节点：`SP_1665_UI_BASICIMG_DCMARK`
  - `int __thiscall CNpc::RestoreLayers(CNpc *this)`
  - `CNpc::SetQuestList(this);`
  - `if ( *(*(this + 42) + 28) != 0 )`
  - `sub_4145AB(a1: this + 228);`
  - `v3 = sub_4281FB(a1: this + 228);`
  - `v28 = -IWzGr2DLayer::GetHeight(a1: v3);`
  - `CAnimationDisplayer::LoadLayer(this: &v26, a2: this + 36);`
  - `LOBYTE(v38) = 1;`
  - `v7 = sub_41468C(a1: StringW);`
  - `v8 = sub_43E86F(a1: &v37, a2: v7, a3: 0, a4: v26, a5: v27, a6: v28, a7: v29, a8: SHIDWORD(v29), a9: v30, a10: v31);`
  - `sub_428712(a1: *v8);`
  - `if ( v37 != 0 )`
  - `(*(*v37 + 8))(a1: v37);`
  - `VariantInit(&pvarg);`
  - …（截断 14 行）

### CNpc::SetActive  @ 0x6d2024  (759 B)

  - `int __thiscall CNpc::SetActive(CNpc *this, int a2)`
  - `v3 = *(this + 37) != 0 ? *(this + 37) - 12 : 0;`
  - `if ( a2 == 0 )`
  - `return (*(*v3 + 4))(a1: v3, a2: 0, a3: 0, a4: 0, a5: 0, a6: 0, a7: 0, a8: 0);`
  - `v5 = *(this + 36) != 0 ? *(this + 36) - 12 + 496 : 496;`
  - `Foothold = CWvsPhysicalSpace2D::GetFoothold(dwSN: *(*(this + 36) != 0 ? *(this + 36) - 12 + 0x1FC : 508));`
  - `(*(v4 + 4))(a1: v3, a2: 1, a3: *(v5 + 2), a4: *(v5 + 4), a5: *(v5 + 6), a6: *(v5 + 8), a7: *(v5 + 10), a8: Foothold);`
  - `result = sub_6D20B1(a1: this);`
  - `*(this + 38) = 0;`

### CNpc::SetClientActionByQuest  @ 0x6d1b11  (10252 B)

  - `void __thiscall CNpc::SetClientActionByQuest(CNpc *this)`
  - `v2 = *(this + 79);`
  - `v3 = *(this + 42);`
  - `*(this + 80) = v2;`
  - `v4 = sub_6DEF51(a1: v3);`
  - `v5 = v4 == *(this + 80);`
  - `*(this + 79) = v4;`
  - `if ( !v5 )`
  - `CNpc::PrepareActionLayer(this);`
  - `if ( *(sub_41A450(a1: *(this + 79)) + 28) != 0 && *(this + 79) >= 0 )`
  - `*(this + 81) = 1;`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `v5 = *(this + 57) == 0;`
  - …（截断 14 行）

### CNpc::SetImitatedLook  @ 0x6d21cd  (3147 B)

  - `void __thiscall CNpc::SetImitatedLook(CNpc *this, struct AvatarLook *a2)`
  - `sub_432C10(a1: 0);`
  - `CAnimationDisplayer::LoadLayer(this: &v17, a2: this + 36);`
  - `LOBYTE(v30) = 2;`
  - `v28 = sub_426626(a1: 2, a2: 1);`
  - `v5 = sub_4285F2(a1: this + 136);`
  - `CAvatar::Init(`
  - `CNpc::PrepareActionLayer(this);`
  - `v6 = *(this + 42);`
  - `v16 = (this + 144);`
  - `*(v6 + 24) = 0;`
  - `CAnimationDisplayer::LoadLayer(this: &v17, a2: v16);`
  - `sub_4145AB(a1: this + 228);`
  - `v9 = *(this + 42);`
  - …（截断 14 行）

### CNpc::SetLayerZ  @ 0x6d267d  (886 B)

  - `int __thiscall CNpc::SetLayerZ(CNpc *this)`
  - `if ( *(this + 57) == 0 )`
  - `v2 = *(this + 57);`
  - `result = (*(*v2 + 180))(`
  - `* (3000 * *(*(this + 36) != 0 ? *(this + 36) - 12 + 0x130 : 304)`
  - `- *(*(this + 36) != 0 ? *(this + 36) - 12 + 0x134 : 308))`
  - `if ( result < 0 )`
  - `if ( *(this + 35) != 0 )`
  - `v4 = IWzGr2DLayer::GetZ(this: *(this + 57));`
  - `return sub_452195(a1: v4);`

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

### CNpc::SetMoveAction  @ 0x6d215e  (394 B)

  - `void __thiscall CNpc::SetMoveAction(CNpc *this, int a2, int a3)`
  - `v4 = *(this + 35);`
  - `if ( v4 != 0 )`
  - `(*(*v4 + 16))(a1: v4, a2, a3);`
  - `if ( a3 != 0 || a2 != *(this + 43) )`
  - `*(this + 43) = a2;`
  - `if ( sub_6D21A6(a1: this) == 0 )`
  - `CNpc::PrepareActionLayer(this);`

### CNpc::SetQuestList  @ 0x6d1779  (7251 B)

- SP_ 节点：`SP_3190_UI_UIWINDOWIMG_QUESTICON_D`
  - `void __thiscall CNpc::SetQuestList(CNpc *this)`
  - `ZArray<unsigned short>::RemoveAll(a1: this + 204);`
  - `v2 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v36) + 4);`
  - `v38 = *(*dword_BE7918 + 14272);`
  - `CurFieldID = CWvsContext::GetCurFieldID(a1: *dword_BE7918);`
  - `for ( i = 0; ; ++i )`
  - `v3 = *(this + 49);`
  - `if ( v3 == 0 || i >= *(v3 - 4) )`
  - `LOWORD(v3) = *(v3 + 2 * i);`
  - `if ( v3 < 0x4B0u || v3 >= 0x578u )`
  - `if ( sub_500EA5(a1: &v37) != 0 )`
  - `CQuestMan::CheckCompleteDemand(this: v40, a2: v43, i: **(this + 42), a4: v2, a5: v41);`
  - `v4 = sub_500E4C(a1: &v43, a2: 0);`
  - `v7 = *(this + 42);`
  - …（截断 14 行）

### CNpc::SetShoeAttr  @ 0x6d20d5  (650 B)

  - `int __thiscall CNpc::SetShoeAttr(CNpc *this)`
  - `v2 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 144);`
  - `if ( v2 != 0 )`
  - `v3 = sub_4FE802(a1: v2);`
  - `sub_500D1A(a1: v3);`
  - `v4 = *(*(this + 42) + 20);`
  - `if ( v4 <= 10 )`
  - `if ( v4 >= 140 )`
  - `return sub_500D61(a1: SLOBYTE(v6), a2: HIDWORD(v6));`

### CNpc::ShowQuestList  @ 0x6d3392  (27614 B)

- **c2s opcode `COutPacket(58)`**
- c2s 编码：`Encode4`, `Encode2`
- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1735_AREA`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_3191__R_N_R_NFUI_UIWINDOWIMG_UTILDLGEX_LIST1_R_N`, `SP_3192__R_N_R_NFUI_UIWINDOWIMG_UTILDLGEX_LIST0_R_N`, `SP_3193__R_N_R_NFUI_UIWINDOWIMG_UTILDLGEX_LIST3_R_N`, `SP_3194__R_N_R_NFUI_UIWINDOWIMG_UTILDLGEX_LIST2_R_N`, `SP_3195_DLD_SLK_R_N`, `SP_3196_ETC_SCRIPTINFOIMG`, `SP_3232_QUEST_QUESTINFOIMG`, `SP_4418_NO_TARGET_AVAILABLE`, `SP_5514_04D`
  - `void __thiscall CNpc::ShowQuestList(CNpc *this)`
  - `v2 = *(this + 77) == 0;`
  - `if ( !v2 && *(this + 81) == 0 )`
  - `v101 = *(this + 42) + 144;`
  - `ZXString<char>::operator=(a1: v101);`
  - `v3 = *(v1 + 50);`
  - `if ( v3 != 0 )`
  - `v4 = *(v3 - 4);`
  - `v5 = *(v1 + 49);`
  - `if ( v5 != 0 )`
  - `v6 = *(v5 - 4);`
  - `if ( v4 == v6 )`
  - `v7 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `LOBYTE(v127) = 2;`
  - …（截断 14 行）

### CNpc::Update  @ 0x6d2918  (6111 B)

  - `_DWORD *__fastcall CNpc::Update(CNpc *this, int a2)`
  - `if ( *(this + 60) != 0 )`
  - `result = sub_443E36(a1: *(this + 60));`
  - `if ( result == 0 )`
  - `result = sub_428712(a1: 0);`
  - `result = *(this + 42);`
  - `if ( result[49] != 0 )`
  - `v4 = *(this + 35);`
  - `if ( v4 == 0 )`
  - `CAvatar::Update(this: v4, a2);`
  - `v5 = *(this + 36) != 0 ? (*(this + 36) - 12) : 0;`
  - `v6 = *(this + 37) != 0 ? *(this + 37) - 12 : 0;`
  - `updated = CVecCtrl::UpdatePassive(this: v5);`
  - `CNpc::SetMoveAction(this, a2: v5[82], a3: 0);`
  - …（截断 14 行）

### CNpc::UpdateScript  @ 0x6d40dc  (149 B)

  - `void __thiscall CNpc::UpdateScript(CNpcTemplate **this, struct _SYSTEMTIME a2)`
  - `CNpcTemplate::UpdateScript(this: *(this + 42), a2);`

### CNpcPool::FindNpc  @ 0x6d9390  (3031 B)

  - `int __thiscall CNpcPool::FindNpc(CNpcPool *this, const struct tagPOINT *a2, int a3)`
  - `v27 = (v3 / dbl_AFC9F0);`
  - `v29 = ((dbl_AFC9F0 - dbl_AF0DE0) * v3);`
  - `v4 = *(this + 10);`
  - `if ( v4 != 0 )`
  - `while ( 1 )`
  - `v31 = *(v4 + 4);`
  - `v18 = **(*(v4 + 16) + 168);`
  - `NpcTemplate = CNpcTemplate::GetNpcTemplate(a1: v18);`
  - `if ( NpcTemplate != 0 && *(NpcTemplate + 17) == 0 && (*(NpcTemplate + 49) == 0 || *(*(v6 + 16) + 140) != 0) )`
  - `v8 = *(v6 + 16);`
  - `if ( v8 == 0 || *(v8 + 77) != 0 && *(v8 + 81) == 0 )`
  - `DCRange = CNpc::GetDCRange(this: v8);`
  - `v23 = *(DCRange + 1);`
  - …（截断 14 行）

### CNpcPool::GetNpc  @ 0x6d9927  (643 B)

  - `int __thiscall CNpcPool::GetNpc(CNpcPool *this, unsigned int a2)`
  - `if ( ZMap<unsigned long,__POSITION *,unsigned long>::GetAt(a1: &a2, a2: &v7) != 0 && v7 != 0 )`
  - `v2 = (v7 + 12);`
  - `v3 = *(v2 + 1);`
  - `if ( (v8 & 1) != 0 )`
  - `sub_6D9B97(a1: v5);`

### CNpcPool::OnNpcChangeController  @ 0x6d9a83  (400 B)

- s2c 解码：`Decode1`, `Decode4`
  - `char __thiscall CNpcPool::OnNpcChangeController(CNpcPool *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `v4 = CInPacket::Decode4(this: a2);`
  - `if ( v3 != 0 )`
  - `return CNpcPool::SetLocalNpc(this, a2: v4, a3: a2);`
  - `return CNpcPool::SetRemoteNpc(this, a2: v4);`

### CNpcPool::OnNpcEnterField  @ 0x6d9993  (1104 B)

- s2c 解码：`Decode4`
  - `void __thiscall CNpcPool::OnNpcEnterField(CNpcPool *this, struct CInPacket *a2)`
  - `v9 = CInPacket::Decode4(this: a2);`
  - `if ( ZMap<unsigned long,__POSITION *,unsigned long>::GetAt(a1: &v8, a2: &v7) != 0 && v7 != 0 )`
  - `*(v7 + 24) |= 1u;`
  - `v3 = ZList<CNpcPool::NPCENTRY>::AddTail(this: this + 7);`
  - `v4 = CInPacket::Decode4(this: a2);`
  - `NpcTemplate = CNpcTemplate::GetNpcTemplate(a1: v4);`
  - `Npc = CreateNpc(a1: NpcTemplate);`
  - `*(v3 + 24) = 1;`
  - `*(v3 + 20) = *(this + 11);`
  - `ZMap<unsigned long,__POSITION *,unsigned long>::Insert(a1: &v9, a2: v3 + 20);`
  - `CNpc::Init(this: *(v3 + 16), Foothold: v9, a3: a2);`

### CNpcPool::OnNpcImitateData  @ 0x6d97c6  (1360 B)

- s2c 解码：`Decode1`, `Decode4`, `DecodeStr`
  - `void __thiscall CNpcPool::OnNpcImitateData(CNpcPool *this, struct CInPacket *a2)`
  - `v2 = CInPacket::Decode1(this: a2);`
  - `if ( v2 != 0 )`
  - `for ( i = v2; i != 0; --i )`
  - `v3 = CInPacket::Decode4(this: a2);`
  - `CInPacket::DecodeStr(this: a2, a2: &v8);`
  - `AvatarLook::`vftable'(a1: v6);`
  - `LOBYTE(v10) = 1;`
  - `AvatarLook::Decode(this: v6, a2);`
  - `v4 = *(v7 + 10);`
  - `while ( v4 != 0 )`
  - `v5 = *(v4 + 16);`
  - `v4 = *(v4 + 4);`
  - `if ( v5 != 0 && **(v5 + 42) == v3 )`
  - …（截断 14 行）

### CNpcPool::OnNpcLeaveField  @ 0x6d9a25  (871 B)

- s2c 解码：`Decode4`
  - `_BYTE *__thiscall CNpcPool::OnNpcLeaveField(CNpcPool *this, struct CInPacket *a2)`
  - `v5 = CInPacket::Decode4(this: a2);`
  - `result = ZMap<unsigned long,__POSITION *,unsigned long>::GetAt(a1: &a2, a2: &v6);`
  - `if ( result != 0 )`
  - `if ( v6 != 0 )`
  - `if ( *result == 0 )`
  - `ZList<CNpcPool::NPCENTRY>::RemoveAt(this: this + 7, a2: *(v4 + 5));`
  - `return ZMap<unsigned long,__POSITION *,unsigned long>::RemoveKey(a1: &v5);`

### CNpcPool::OnNpcPacket  @ 0x6d98de  (681 B)

- s2c 解码：`Decode4`
- 分支 case：`260`, `261`, `262`
  - `void __thiscall CNpcPool::OnNpcPacket(CNpcPool *this, int a2, struct CInPacket *a3)`
  - `v4 = CInPacket::Decode4(this: a3);`
  - `Npc = CNpcPool::GetNpc(this, a2: v4);`
  - `if ( Npc != 0 )`
  - `switch ( a2 )`
  - `CNpc::OnMove(this: Npc, a2: a3);`
  - `CNpc::OnUpdateLimitedInfo(this: Npc, a2: a3);`
  - `CNpc::OnSetSpecialAction(this: Npc, a2: a3);`

### CNpcPool::OnNpcTemplatePacket  @ 0x6d997c  (182 B)

  - `void __thiscall CNpcPool::OnNpcTemplatePacket(CNpcPool *this, int a2, struct CInPacket *a3)`
  - `if ( a2 == 263 )`
  - `CNpcTemplate::OnSetNpcScript(a1: a3);`

### CNpcPool::OnPacket  @ 0x6d9734  (893 B)

- 分支 case：`81`, `82`, `257`, `258`, `259`
  - `signed int __stdcall CNpcPool::OnPacket(signed int a1, struct CInPacket *a2)`
  - `switch ( a1 )`
  - `return CNpcPool::OnNpcImitateData(a1: a2);`
  - `return CNpcPool::OnUpdateLimitedDisableInfo(a1: a2);`
  - `return CNpcPool::OnNpcEnterField(a1: a2);`
  - `return CNpcPool::OnNpcLeaveField(a1: a2);`
  - `return CNpcPool::OnNpcChangeController(a1: a2);`
  - `if ( a1 >= 260 && a1 <= 262 )`
  - `CNpcPool::OnNpcPacket(a1, a2);`
  - `if ( a1 == 263 )`
  - `return CNpcPool::OnNpcTemplatePacket(a1: 263, a2);`

### CNpcPool::OnUpdateLimitedDisableInfo  @ 0x6d989c  (613 B)

- s2c 解码：`Decode1`, `Decode4`
  - `int __thiscall CNpcPool::OnUpdateLimitedDisableInfo(CNpcPool *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `result = ZMap<unsigned long,int,unsigned long>::RemoveAll(a1: this + 56);`
  - `if ( v3 > 0 )`
  - `v5 = CInPacket::Decode4(this: a2);`
  - `result = ZMap<unsigned long,int,unsigned long>::Insert(a1: &v5, a2: 0);`
  - `while ( v3 != 0 );`

### CNpcPool::SetLocalNpc  @ 0x6d9549  (1339 B)

- s2c 解码：`Decode4`
  - `char __thiscall CNpcPool::SetLocalNpc(CNpcPool *this, VARIANTARG *a2, struct CInPacket *a3)`
  - `if ( ZMap<unsigned long,__POSITION *,unsigned long>::GetAt(a1: &v11, a2: &v10) != 0 && (v4 = v10, v10 != 0) )`
  - `result = *(v10 + 24);`
  - `if ( (result & 2) == 0 )`
  - `*(v10 + 24) = result | 2;`
  - `return CNpc::SetActive(this: *(v4 + 16), a2: 1);`
  - `v6 = ZList<CNpcPool::NPCENTRY>::AddTail(this: this + 7);`
  - `v7 = CInPacket::Decode4(this: a3);`
  - `NpcTemplate = CNpcTemplate::GetNpcTemplate(a1: v7);`
  - `Npc = CreateNpc(a1: NpcTemplate);`
  - `*(v6 + 24) = 2;`
  - `*(v6 + 20) = *(this + 11);`
  - `ZMap<unsigned long,__POSITION *,unsigned long>::Insert(a1: &a2, a2: v6 + 20);`
  - `CNpc::Init(this: *(v6 + 16), Foothold: a2, a3);`
  - …（截断 14 行）

### CNpcPool::SetRemoteNpc  @ 0x6d95ee  (968 B)

  - `char __thiscall CNpcPool::SetRemoteNpc(CNpcPool *this, unsigned int a2)`
  - `v3 = ZMap<unsigned long,__POSITION *,unsigned long>::GetAt(a1: &v8, a2: &v7);`
  - `if ( v3 != 0 )`
  - `if ( v7 != 0 )`
  - `LOBYTE(v3) = *(v7 + 24);`
  - `v5 = (v7 + 24);`
  - `if ( (v3 & 2) != 0 )`
  - `LOBYTE(v3) = CNpc::SetActive(this: *(v4 + 16), a2: 0);`
  - `if ( *v5 == 0 )`
  - `ZList<CNpcPool::NPCENTRY>::RemoveAt(this: this + 7, a2: *(v4 + 20));`
  - `LOBYTE(v3) = ZMap<unsigned long,__POSITION *,unsigned long>::RemoveKey(a1: &a2);`

### CNpcPool::Update  @ 0x6d966d  (1044 B)

  - `void __thiscall CNpcPool::Update(CNpcPool *this)`
  - `void (__thiscall ***v6)(_DWORD); // ecx`
  - `v2 = (*(this + 12) + 1) % 33;`
  - `*(this + 12) = v2;`
  - `if ( v2 == 0 )`
  - `field = get_field();`
  - `v8 = *CField::GetCorrectTime(this: field, retstr: &v7);`
  - `if ( v8.wDay != *(this + 26) )`
  - `*(this + 26) = v8.wDay;`
  - `v4 = *(this + 10);`
  - `while ( v4 != 0 )`
  - `v6 = *(v4 + 16);`
  - `v4 = *(v4 + 4);`
  - `(**v6)(a1: v6);`
  - …（截断 14 行）

### CNpcTemplate::GetNpcTemplate  @ 0x6dcb16  (6414 B)

- SP_ 节点：`SP_1666_STRING_NPCIMG`, `SP_955_NPC_07DIMG`
  - `static int __cdecl CNpcTemplate::GetNpcTemplate(int a1)`
  - `sub_6DF2EE(a1: &v40, a2: v35);`
  - `if ( v1 != 0 && v36 == 0 )`
  - `LOBYTE(v43) = 1;`
  - `sub_416821(a1: &v40, a2: v3, a3: v1);`
  - `if ( Unknown != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v34, pvargSrc: &pvargSrc);`
  - `v6 = sub_406472();`
  - `v7 = sub_402680(a1: v6);`
  - `IWzResMan::GetObjectA(this: v7, pvargDest: &pvarg, sUOL: v24, vParam: v25, vAux: p_pvargDest);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v27, a2: v28);`
  - `v8 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v8 < 0 && v8 != -2147467262 )`
  - …（截断 14 行）

### CNpcTemplate::OnSetNpcScript  @ 0x6deeb7  (1106 B)

- s2c 解码：`Decode1`
  - `static void __cdecl CNpcTemplate::OnSetNpcScript(struct CInPacket *a1)`
  - `v1 = CInPacket::Decode1(this: a1);`
  - `if ( v1 != 0 )`
  - `for ( i = v1; i != 0; --i )`
  - `MODSCRIPT::Decode(this: &v5, a2: a1);`
  - `NpcTemplate = CNpcTemplate::GetNpcTemplate(a1: v5);`
  - `if ( NpcTemplate != 0 )`
  - `ZList<SCRIPTINFO>::AddTail(a1: v6);`
  - `field = get_field();`
  - `v9 = *CField::GetCorrectTime(this: field, retstr: &v4);`
  - `CNpcTemplate::UpdateScript(this: NpcTemplate, a2: v9);`

### CNpcTemplate::RegisterNpc  @ 0x6dce02  (2000 B)


### CNpcTemplate::UpdateScript  @ 0x6dedf7  (1111 B)

  - `void __thiscall CNpcTemplate::UpdateScript(CNpcTemplate *this, struct _SYSTEMTIME a2)`
  - `v3 = a2.wDay + 100 * (a2.wMonth + 100 * a2.wYear);`
  - `ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF);`
  - `v4 = *(this + 35);`
  - `while ( v4 != 0 )`
  - `v6 = *(v4 != 0 ? v4 - 16 + 8 : 8);`
  - `ZXString<char>::operator=(a1: v5);`
  - `v9 = *(v5 + 4);`
  - `v7 = *(v5 + 8);`
  - `if ( v9 <= v3 && v3 <= v7 )`
  - `ZXString<char>::operator=(a1: &v8);`

### CQuestMan::GetQuestByNpc  @ 0x71ddec  (219 B)

  - `BOOL __stdcall CQuestMan::GetQuestByNpc(char a1, int a2)`
  - `ZArray<unsigned short>::RemoveAll(a1: a2);`
  - `return ZMap<unsigned long,ZArray<unsigned short>,unsigned long>::GetAt(&a1, a2) != 0;`

### CTrunkDlg::SetNPC  @ 0x7c71af  (10041 B)

  - `int __thiscall CTrunkDlg::SetNPC(CTrunkDlg *this)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `LOBYTE(v57) = 1;`
  - `if ( dword_BF14EC == 0 )`
  - `v2 = IWzGr2D::CreateLayer(`
  - `v3 = (this + 272);`
  - `sub_428712(a1: *v2);`
  - `sub_4156C5(a1: &v56);`
  - `if ( v4 < 0 )`
  - `if ( v5 < 0 )`
  - `v6 = *CWnd::GetLayer(a1: &v55);`
  - `sub_410FDF(this: &pvargDest, a2: v6, a3: 1);`
  - `if ( v7 )`
  - …（截断 14 行）

### CUserLocal::TalkToNpc  @ 0x95fe9e  (1297 B)

- **c2s opcode `COutPacket(58)`**
- c2s 编码：`Encode4`, `Encode2`
  - `int __thiscall CUserLocal::TalkToNpc(CUserLocal *this, char a2, CNpc *a3)`
  - `if ( *(this + 3149) != 0 )`
  - `CUserLocal::OnKeyDownSkillEnd(this);`
  - `v4 = *(a3 + 49);`
  - `if ( v4 != 0 && *(v4 - 4) != 0 )`
  - `CNpc::ShowQuestList(this: a3);`
  - `COutPacket::COutPacket(a1: 58);`
  - `v10 = *(a3 + 41);`
  - `LOBYTE(v13) = 1;`
  - `COutPacket::Encode4(this: &v11, a2: v10);`
  - `v5 = *(this + 1);`
  - `v6 = (this + 4);`
  - `v7 = (*(v5 + 16))(a1: v6);`
  - `COutPacket::Encode2(this: &v11, a2: *v7);`
  - …（截断 14 行）

### CUtilDlgEx::SetNPC  @ 0x9a64fc  (1915 B)


### CVecCtrlNpc::CreateInstance  @ 0x9c1ce6  (317 B)

  - `static int CVecCtrlNpc::CreateInstance()`
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 600) != 0 )`
  - `v0 = sub_9C1C9B();`
  - `if ( v0 == 0 )`
  - `_com_raise_error(a1: -2147024882, a2: 0);`

### CVecCtrlNpc::Init  @ 0x9c1d3b  (348 B)

  - `int __thiscall CVecCtrlNpc::Init(CVecCtrlNpc *this, struct IVecCtrlOwner *a2, int a3, struct RANGE *a4)`
  - `CVecCtrl::Init(this, a2);`
  - `*(this + 146) = a3 == 1;`
  - `*(this + 78) = *a4;`
  - `result = *(a4 + 1);`
  - `*(this + 80) = result;`

### CVecCtrlNpc::SetActive  @ 0x9c1d70  (424 B)

  - `void *__thiscall CVecCtrlNpc::SetActive(`
  - `CVecCtrl::SetActive(this, a2, a3, a4, a5, a6, a7, a8);`
  - `result = memset(a1: this + 588, a2: 0, a3: 8u);`
  - `*(this + 149) = 0;`

### CVecCtrlNpc::UpdateActiveInterrupted  @ 0x9c1daf  (6070 B)

  - `int __thiscall CVecCtrlNpc::UpdateActiveInterrupted(CVecCtrlNpc *this, signed int a2, signed int a3)`
  - `v4 = *((*(**(this + 5) + 12))(a1: *(this + 5), a2: &v31) + 4);`
  - `result = sub_6725B6(a1: &v31);`
  - `if ( *(this + 68) != 0 )`
  - `*(this + 66) = _ZtlSecureTear<double>(a1: SLOBYTE(v20), a2: HIDWORD(v20));`
  - `sub_9B1646(this: this + 8, a2: this + 224, a3: *(this + 68));`
  - `result = CVecCtrl::SetMovePathAttribute(this, a2: 2);`
  - `if ( a2 > 0 )`
  - `while ( 1 )`
  - `if ( a2 <= 30 )`
  - `v29 = *(this + 66);`
  - `qmemcpy(v30, this + 224, sizeof(v30));`
  - `v34 = _ZtlSecureFuse<double>(a1: this + 248, a2: v29);`
  - `Data = TSecType<double>::GetData(a1: v35);`
  - …（截断 14 行）

### CVecCtrlNpc::WorkUpdateActive  @ 0x9c211e  (1048 B)

  - `int __thiscall CVecCtrlNpc::WorkUpdateActive(CVecCtrlNpc *this, signed int a2)`
  - `if ( *(this + 148) <= 0 )`
  - `if ( rand() % 5 != 0 )`
  - `v3 = rand();`
  - `*(this + 147) = 0;`
  - `*(this + 148) = (v3 % 2000 + 1000) / 90;`
  - `*(this + 148) = (rand() % 1000 + 500) / 90;`
  - `*(this + 147) = rand() % 2 != 0 ? -1 : 1;`
  - `CVecCtrl::SetInput(this, a2: *(this + 147), a3: 0);`
  - `--*(this + 148);`
  - `CVecCtrl::WorkUpdateActive(this, a2);`
  - `if ( *(this + 147) != 0 )`
  - `v6 = _ZtlSecureFuse<double>(a1: this + 32, a2: *(this + 12));`
  - `if ( _ZtlSecureFuse<double>(a1: this + 128, a2: *(this + 36)) == v6 )`
  - …（截断 14 行）

### CWvsContext::GuildNPCSay  @ 0xa393d3  (2683 B)

  - `int __stdcall CWvsContext::GuildNPCSay(int *a1, int a2)`
  - `if ( (byte_BF55B0 & 1) == 0 )`
  - `atexit(a1: loc_A39564);`
  - `if ( dword_BF55AC != 0 )`
  - `(*(*dword_BF55AC + 52))(a1: dword_BF55AC, a2: 1);`
  - `if ( *a1 == 0 || v14 >= *(result - 4) )`
  - `v4 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v4 != 0 )`
  - `v5 = CUtilDlgEx::CUtilDlgEx(this: v4);`
  - `v13 = v6 != 0 ? (v6 - 8) : 0;`
  - `sub_4289CD(a1: v12);`
  - `v7 = sub_490E87(a1: v14);`
  - `sub_428211(a1: v7);`
  - `LOBYTE(v15) = 1;`
  - …（截断 14 行）

### PAVCVecCtrlNpc::$_QueryInterface  @ 0x6d78db  (870 B)

  - `int __thiscall _com_ptr_t<_com_IIID<IWzVector2D,&__s_GUID const _GUID_f28bd1ed_3deb_4f92_9eec_10ef5a1c3fb4>>::_QueryInterface<CVecCtrlNpc *>(`
  - `if ( *a2 != 0 )`
  - `v3 = (**(*a2 + 12))(a1: *a2 + 12, a2: dword_BD8348, a3: &a2);`
  - `if ( v3 >= 0 )`
  - `if ( *this != 0 )`
  - `(*(**this + 8))(a1: *this);`
  - `_com_ptr_t<_com_IIID<IWzVector2D,&__s_GUID const _GUID_f28bd1ed_3deb_4f92_9eec_10ef5a1c3fb4>>::operator=(a1: 0);`

### VCNpc::4?$ZRef  @ 0x6d9ac2  (413 B)

  - `sub_6D9BAD(a1: v6);`
  - `v4 = *(this + 1);`
  - `*(this + 1) = v7;`
  - `sub_6D9B97(a1: v6);`

## 2. 反编译中出现的 c2s opcode 常量汇总（COutPacket 构造参数）

| opcode (dec) | hex | 来源函数 |
| --- | --- | --- |
| 58 | 0x3A | CNpc::ShowQuestList, CUserLocal::TalkToNpc |
| 197 | 0xC5 | CNpc::GenerateMovePath |
