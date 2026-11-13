# Angel 083 Drop 面反编译语义抽取（观察层）

来源：`evidence/angel-drop-decompile.json`（全量函数逐字 Hex-Rays 伪代码，来自 Angel 13339 IDB）。
本文件由 `tools/ida_angel_drop_facts.py` 自动生成，只做字符串抽取与归类，不含推断。

## 1. 逐函数关键信号（opcode / Encode / Decode / 分支 / SP_ 节点 / 数值调用）


### CDraggableItem::OnDropped  @ 0x4ef140  (16474 B)

- SP_ 节点：`SP_1234_DRAGEND`
  - `int __thiscall CDraggableItem::OnDropped(CDraggableItem *this, int a2, int a3, FILETIME *a4, int a5)`
  - `if ( a3 != 0 && (*(*a3 + 72))(a1: a3, a2: &dword_BF0DA8) != 0 )`
  - `v7 = (v5 - 4);`
  - `if ( a2 != (v7 != 0 ? v7 + 4 : 0) && v7 != 0 )`
  - `return CDraggableItem::PutItem(this, a2: v7, a3: a4, a4: a5);`
  - `if ( v5 != 0 && (*(*v5 + 72))(a1: v5, a2: &dword_BEC370) != 0 )`
  - `if ( a2 != (v10 != 0 ? v10 + 4 : 0) && v10 != 0 )`
  - `return sub_4F407C(this, a2: v10, a3: a4, a4: a5);`
  - `if ( v5 != 0 && (*(*v5 + 72))(a1: v5, a2: &dword_BEDCA0) != 0 )`
  - `if ( a2 != (v11 != 0 ? v11 + 4 : 0) && v11 != 0 )`
  - `return CDraggableItem::PutItem(a1: v11, a2: a4, a3: a5);`
  - `if ( v5 != 0 && (*(*v5 + 72))(a1: v5, a2: &dword_BEDC94) != 0 )`
  - `if ( a2 != (v12 != 0 ? v12 + 4 : 0) && v12 != 0 )`
  - `return sub_4F483E(this, a2: v12);`
  - …（截断 14 行）

### CDraggableMenu::OnDropped  @ 0x4f91e4  (2556 B)

  - `int __thiscall CDraggableMenu::OnDropped(`
  - `if ( a2 != 0 && (*(*a2 + 72))(a1: a2, a2: &dword_BF0F08) != 0 )`
  - `if ( v5 != 0 )`
  - `v6 = a3 != 0 && (*(*a3 + 72))(a1: a3, a2: &dword_BF0F08) != 0 ? a3 - 4 : 0;`
  - `if ( v6 != 0 )`
  - `return CDraggableMenu::MapFuncKey(this, a2: a4, a3: a5, a4: 0);`
  - `if ( a2 != 0 && (*(*a2 + 72))(a1: a2, a2: &dword_BF10FC) != 0 )`
  - `if ( v7 == 0 || (a3 == 0 || (*(*a3 + 72))(a1: a3, a2: &dword_BF10FC) == 0 ? (v8 = 0) : (v8 = a3 - 4), v8 == 0) )`
  - `if ( v10 != 0 )`
  - `v11 = a3 != 0 && (*(*a3 + 72))(a1: a3, a2: &dword_BF10FC) != 0 ? a3 - 4 : 0;`
  - `if ( v11 != 0 )`
  - `if ( v12 == 0 || (a3 == 0 || (*(*a3 + 72))(a1: a3, a2: &dword_BF0F08) == 0 ? (v13 = 0) : (v13 = a3 - 4), v13 == 0) )`
  - `if ( v14 == 0 )`
  - `v15 = a3 != 0 && (*(*a3 + 72))(a1: a3, a2: &dword_BF10FC) != 0 ? a3 - 4 : 0;`
  - …（截断 14 行）

### CDraggableSkill::OnDropped  @ 0x4faa22  (8169 B)

  - `signed int __thiscall CDraggableSkill::OnDropped(`
  - `v6 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v50) + 4);`
  - `if ( v50[1] != 0 )`
  - `if ( *(this + 7) == 0 && CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v6, a3: *(this + 6), a4: 0) <= 0 )`
  - `if ( a2 != 0 && (*(*a2 + 72))(a1: a2, a2: &dword_BF1078) != 0 )`
  - `if ( v7 != 0 )`
  - `v8 = a3 != 0 && (*(*a3 + 72))(a1: a3, a2: &dword_BF10FC) != 0 ? a3 - 4 : 0;`
  - `if ( v8 != 0 )`
  - `v10 = a2 != 0 && (*(*a2 + 72))(a1: a2, a2: &dword_BF109C) != 0 ? a2 - 4 : 0;`
  - `if ( v10 != 0 )`
  - `v11 = a3 != 0 && (*(*a3 + 72))(a1: a3, a2: &dword_BF10FC) != 0 ? a3 - 4 : 0;`
  - `if ( v11 != 0 )`
  - `if ( v12 != 0 )`
  - `v13 = a3 != 0 && (*(*a3 + 72))(a1: a3, a2: &dword_BF0F08) != 0 ? a3 - 4 : 0;`
  - …（截断 14 行）

### CDropPool::BoobyTrapCheckPickupItem  @ 0x5081b0  (1336 B)

  - `int __thiscall CDropPool::BoobyTrapCheckPickupItem(CDropPool *this, const struct tagPOINT *a2)`
  - `if ( *(this + 23) != 0 && *(this + 26) == 0 )`
  - `CWvsContext::SendBoobyTrapAlert(this: *dword_BE7918, a2: 1);`
  - `if ( *(v2 + 3578) != 0 && *(this + 23) == 0 )`
  - `CWvsContext::SendBoobyTrapAlert(this: v2, a2: 1);`
  - `v6 = *(this + 24);`
  - `while ( v6 != 0 )`
  - `v7 = *(v6 != 0 ? v6 - 3 : 4);`
  - `v6 = v7 != 0 ? (v7 + 16) : 0;`
  - `if ( dword_BF0484(a1: v11, a2: v9, a3: v10) == 0 )`
  - `*(this + 26) = 0;`
  - `result = ZList<tagPOINT>::RemoveAll(a1: this + 84);`
  - `*(v2 + 3578) = 0;`

### CDropPool::GetMoneyIcon  @ 0x506c26  (4873 B)

- SP_ 节点：`SP_1589_ICONRAW`, `SP_1717_08D`
  - `_DWORD *__thiscall CDropPool::GetMoneyIcon(char *this, _DWORD *a2, int *a3, int *a4)`
  - `LOBYTE(v39) = 2;`
  - `v8 = sub_506BFE(a1: a3);`
  - `ZXString<char>::Format(a1: &v35, a2: v7, a3: &loc_89543E + v8 + 2);`
  - `v9 = sub_428226(a1: &v35);`
  - `_bstr_t::_bstr_t(a1: v9);`
  - `sub_40263B(a1: this + 76);`
  - `IWzProperty::GetItem(pvargDest: &pvarg, a2: v27);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v28, a2: v29);`
  - `if ( Unknown != 0 )`
  - `v11 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v11 < 0 && v11 != -2147467262 )`
  - `if ( v12 < 0 )`
  - `sub_40263B(a1: &v36);`
  - …（截断 14 行）

### CDropPool::MakeMoneyAnimation  @ 0x506e62  (36338 B)

  - `_DWORD *__thiscall CDropPool::MakeMoneyAnimation(char *this, char a2, int a3)`
  - `v117 = *(a3 + 52);`
  - `v3 = sub_506BFE(a1: v117);`
  - `v209 = *(a3 + 56);`
  - `sub_432D38(a1: &v209);`
  - `LOBYTE(v210) = 1;`
  - `sub_402FAB(a1: -2, a2: 3);`
  - `if ( v209 == 0 )`
  - `IWzGr2DLayer::RemoveCanvas(this: v209, a2: &v207, a3: &v195);`
  - `sub_4156AB(a1: &v207);`
  - `if ( v4 < 0 )`
  - `if ( v3 >= 0 )`
  - `if ( v3 <= 1 )`
  - `sub_402F85(pvargDest: &v176, pvargSrc: &pvargSrc);`
  - …（截断 14 行）

### CDropPool::OnDropEnterField  @ 0x505900  (21483 B)

- s2c 解码：`Decode1`, `Decode4`, `Decode2`, `DecodeBuffer`
- SP_ 节点：`SP_1588_ICONREWARD`, `SP_958_SHAPE2DVECTOR2D`
  - `int __thiscall CDropPool::OnDropEnterField(CDropPool *this, struct CInPacket *a2)`
  - `update_time = get_update_time();`
  - `v108 = CInPacket::Decode1(this: a2);`
  - `v95 = CInPacket::Decode4(this: a2);`
  - `if ( sub_508AE3(a1: &v95, a2: v114) != 0 )`
  - `if ( (v108 == 1 || v108 == 2) && v115[29] == 0 )`
  - `sub_5088C7(a1: v114);`
  - `*(v115 + 8) = v95;`
  - `*(v5 + 12) = CInPacket::Decode1(this: a2);`
  - `*(v6 + 13) = CInPacket::Decode4(this: a2);`
  - `*(v7 + 9) = CInPacket::Decode4(this: a2);`
  - `*(v8 + 11) = CInPacket::Decode1(this: a2);`
  - `v93 = CInPacket::Decode2(this: a2);`
  - `v9 = CInPacket::Decode2(this: a2);`
  - …（截断 14 行）

### CDropPool::OnDropLeaveField  @ 0x506590  (12038 B)

- s2c 解码：`Decode1`, `Decode4`, `Decode2`
- SP_ 节点：`SP_1245_PICKUPITEM`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`
  - `int __thiscall CDropPool::OnDropLeaveField(CDropPool *this, struct CInPacket *a2)`
  - `v2 = CInPacket::Decode1(this: a2);`
  - `v71 = CInPacket::Decode4(this: a2);`
  - `if ( sub_508AE3(a1: &v71, a2: v78) == 0 )`
  - `if ( v2 == 2 || v2 == 3 || v2 == 5 )`
  - `*(v7 + 88) = CInPacket::Decode4(this: a2);`
  - `else if ( v2 == 4 )`
  - `v5 = CInPacket::Decode2(this: a2);`
  - `*(v4 + 68) = get_update_time() + v5;`
  - `*(v79 + 72) = 4;`
  - `return sub_508917(a1: v78);`
  - `sub_5088FA(a1: &v53);`
  - `result = sub_5056AF(a1: v53, a2: v54[0]);`
  - `if ( v2 == 0 )`
  - …（截断 14 行）

### CDropPool::OnPacket  @ 0x5058db  (339 B)

  - `int __stdcall CDropPool::OnPacket(int a1, struct CInPacket *a2)`
  - `if ( a1 == 268 )`
  - `return CDropPool::OnDropEnterField(a1: a2);`
  - `return CDropPool::OnDropLeaveField(a1: a2);`

### CDropPool::TryPickUpDrop  @ 0x50463c  (4076 B)

- SP_ 节点：`SP_148_NOT_AVAILABLE_WITH_AN_ADMIN_ACCOUNT`
- 节流 `CanSendExclRequest(30)`
  - `const struct tagPOINT *__thiscall CDropPool::TryPickUpDrop(CDropPool *this, const struct tagPOINT *a2)`
  - `v3 = *(*dword_BE7918 + 8340);`
  - `if ( v3 == 0 )`
  - `result = CWvsContext::CanSendExclRequest(a1: 30, a2: 0);`
  - `if ( result == 0 )`
  - `result = get_update_time();`
  - `v9 = *(this + 11);`
  - `while ( 1 )`
  - `if ( v9 == 0 )`
  - `v10 = *(v9 != 0 ? v9 - 16 + 4 : 4);`
  - `v11 = *(v9 + 4);`
  - `sub_5088FA(a1: v20);`
  - `v15 = *(v21 + 48) == 0;`
  - `if ( v15 )`
  - …（截断 14 行）

### CUIItem::OnDropMoney  @ 0x81d996  (3387 B)

- SP_ 节点：`SP_5280_HOW_MANY_WILL_YOU_DROP_A_PORTION_OF_THE_AMOUNT_MAY_BE_LOST_IF_PICKED_UP_BY_ANOTH`, `SP_66_YOU_CANT_DO_IT_ONCE_YOURE_DEAD`, `SP_775_THE_ADMIN_ACCOUNT_CANT_THROW_AWAY_THE_MONEY`
  - `int __thiscall CUIItem::OnDropMoney(CUIItem *this)`
  - `if ( (*(TSingleton<CUserLocal>::ms_pInstance + 1392) & 0xFFFFFFFE) == 0x12 )`
  - `return CUtilDlg::Notice(sMsg: v14, sSndName: v15, ppDialog: v16, bAutoSeparated: v17, bIsTightLine: v18);`
  - `if ( sub_4F38D3(this: *dword_BE7918) != 0 && sub_4F38B4(this: v1) )`
  - `v5 = *(CWvsContext::GetCharacterData(this: v1, a2: &v19) + 4);`
  - `if ( v20 != 0 )`
  - `v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v6 != 0 )`
  - `v7 = CUtilDlgEx::CUtilDlgEx(this: v6);`
  - `v22 = v8 != 0 ? (v8 - 8) : 0;`
  - `sub_4289CD(a1: v21);`
  - `LOBYTE(v25) = 1;`
  - `v11 = sub_428688(a1: v21);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v11, a2: 2, a3: 0, a4: v16, a5: v17, a6: v18);`
  - …（截断 14 行）

### CUserLocal::CheckBoobyTrapPickUpRequest  @ 0x95d7b7  (1169 B)

  - `void __thiscall CUserLocal::CheckBoobyTrapPickUpRequest(CUserLocal *this, unsigned int a2)`
  - `v3 = BYTE2(a2);`
  - `if ( BYTE2(a2) == 82`
  - `&& (a2 & 0x1000000) == 0`
  - `&& !CUser::IsDarkSight(this)`
  - `&& !CUser::IsSneak(this)`
  - `&& (*(this + 348) & 0xFFFFFFFE) != 0x12`
  - `&& CUserLocal::IsStun(this) == 0 )`
  - `CWvsContext::CheckBoobyTrapPickUpRequest(this: v2, a2: (this + 12580));`
  - `if ( BYTE2(a2) == 54 )`
  - `v6 = *(v3 + v5 + 4);`
  - `if ( v6 != 0 )`
  - `v8 = *(v7 + 1);`
  - `if ( v8 != 0`
  - …（截断 14 行）

### CWnd::OnDragDrop  @ 0x4243fe  (103 B)

  - `int __stdcall CWnd::OnDragDrop(int a1, struct DRAGCTX *a2, int a3, int a4)`

### CWndMan::BeginDragDrop  @ 0x9e353d  (4356 B)

  - `int __userpurge CWndMan::BeginDragDrop@<eax>(`
  - `if ( *(this + 33) != 0 )`
  - `*(this + 33) = 0;`
  - `CInputSystem::SetCursorState(this: dword_BEC33C, a2: 11);`
  - `*(this + 36) = a2;`
  - `sub_9E49F9(a1: a3);`
  - `if ( *(a3 + 3) == 0 )`
  - `if ( *(this + 55) == 0 )`
  - `v5 = *(a3 + 3);`
  - `v28 = *(this + 55);`
  - `v6 = IWzShape2D::Getx(a1: v5);`
  - `v7 = IWzShape2D::Getx(a1: v28);`
  - `v8 = *(a3 + 3) == 0;`
  - `if ( v8 )`
  - …（截断 14 行）

### CWndMan::EndDragDrop  @ 0x9e37c2  (5881 B)

  - `int __thiscall CWndMan::EndDragDrop(CWndMan *this, int a2, int a3, int a4)`
  - `result = *(this + 38);`
  - `if ( result != 0 )`
  - `v6 = *(this + 35);`
  - `if ( v6 != 0 )`
  - `(*(*v6 + 24))(a1: v6, a2: 0, a3: result, a4: a2, a5: a3);`
  - `v28 = *(this + 36);`
  - `v27 = *(this + 38);`
  - `sub_9E4A95(a1: v26);`
  - `sub_432D38(a1: &v32);`
  - `LOBYTE(v33) = 1;`
  - `if ( sub_444F15(a1: 0) == 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - …（截断 14 行）

### CWvsContext::CheckBoobyTrapPickUpRequest  @ 0xa32914  (188 B)

  - `int __thiscall CWvsContext::CheckBoobyTrapPickUpRequest(CWvsContext *this, const struct tagPOINT *a2)`
  - `return CDropPool::BoobyTrapCheckPickupItem(this: dword_BED6AC, a2);`

### CWvsContext::OnDropPickUpMessage  @ 0xa20ad9  (8585 B)

- s2c 解码：`Decode1`, `Decode4`, `Decode2`
- 分支 case：`2`, `-2`, `-3`
- SP_ 节点：`SP_120_MAPLESTORY_ERROR__ACTIONMAN_ERRORTYPE1`, `SP_290_YOU_HAVE_GAINED_MESOS_D`, `SP_291_INTERNET_CAFE_MESO_BONUS_D`, `SP_295_YOU_CANT_GET_ANYMORE_ITEMS`, `SP_2983_THIS_ITEM_IS_UNAVAILABLE_FOR_THE_PICK_UP`, `SP_5239_A_PORTION_WAS_NOT_FOUND_AFTER_FALLING_ON_THE_GROUND`, `SP_5311_YOU_CANNOT_ACQUIRE_ANY_ITEMS_BECAUSE_THE_GAME_FILE_HAS_BEEN_DAMAGED_PLEASE_TRY_A`, `SP_5317_YOU_CANNOT_ACQUIRE_ANY_ITEMS`, `SP_5437_YOU_HAVE_GAINED_ITEMS_IN_THE_S_TAB_S_D`, `SP_5438_YOU_HAVE_GAINED_AN_ITEM_IN_THE_S_TAB_S`, `SP_96_MAPLESTORY_ERROR__LOADING_MOB_FAILED`
  - `void __thiscall CWvsContext::OnDropPickUpMessage(VARIANTARG **this, StringPoolStrings *a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `if ( v3 == 1 )`
  - `v4 = CInPacket::Decode1(this: v2);`
  - `a2 = CInPacket::Decode4(this: v2);`
  - `v5 = CInPacket::Decode2(this: v2);`
  - `LOBYTE(v38) = 1;`
  - `ZXString<char>::Format(a1: &v37, a2: v7, a3: a2);`
  - `if ( *v36 != 0 )`
  - `if ( dword_BEDCD0 != 0 )`
  - `sub_89B159(a1: v37, a2: 0, a3: 1);`
  - `if ( v4 != 0 )`
  - `CHATLOG_ADD(sString: String, nType: 8u);`
  - `if ( v5 > 0 )`
  - …（截断 14 行）

### CWvsContext::SendDropMoneyRequest  @ 0xa23de5  (1339 B)

- **c2s opcode `COutPacket(94)`**
- c2s 编码：`Encode4`
- SP_ 节点：`SP_4558_YOU_CANNOT_DROP_THE_ITEM_HERE`
  - `void __thiscall CWvsContext::SendDropMoneyRequest(CWvsContext *this, int a2)`
  - `if ( CWvsContext::CanSendExclRequest(this, a2: 200, a3: 0) )`
  - `if ( (*(get_field() + 324) & 0x400000) != 0 )`
  - `CHATLOG_ADD(sString: String, nType: 0xCu);`
  - `COutPacket::COutPacket(a1: 94);`
  - `update_time = get_update_time();`
  - `COutPacket::Encode4(this: &v7, a2: update_time);`
  - `COutPacket::Encode4(this: &v7, a2);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v7);`
  - `*(this + 2089) = 1;`
  - `v6 = get_update_time();`
  - `*(this + 2090) = v6;`

## 2. 反编译中出现的 c2s opcode 常量汇总（COutPacket 构造参数）

| opcode (dec) | hex | 来源函数 |
| --- | --- | --- |
| 94 | 0x5E | CWvsContext::SendDropMoneyRequest |
