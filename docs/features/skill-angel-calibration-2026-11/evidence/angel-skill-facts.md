# Angel 083 技能面反编译语义抽取（观察层）

来源：`evidence/angel-skill-decompile.json`（106 函数逐字 Hex-Rays 伪代码，来自 Angel 13339 IDB）。
本文件由 `tools/ida_angel_skill_facts.py` 自动生成，只做字符串抽取与归类，不含推断。

## 1. 逐函数关键信号（opcode / Encode / Decode / 分支 / SP_ 节点 / 数值调用）


### <free>::0CUISkill  @ 0x8aa5ad  (5084 B)

- SP_ 节点：`SP_1410_CANVASFONT`, `SP_8_TAHOMA`
  - `_DWORD *__fastcall CUISkill::CUISkill(_DWORD *a1, int a2, int a3, int a4)`
  - `CUIWnd::CUIWnd(this: a1, a2: 3, a3: 3, a4: 155, a5: 6, a6: 1, a7: 0, a8: 0);`
  - ``eh vector constructor iterator'(`
  - ``eh vector constructor iterator'(a1: a1 + 393, a2: 4u, a3: 3, a4: sub_4B250B, a5: sub_4145C6);`
  - `LOBYTE(v29) = 18;`
  - `basic_font = get_basic_font(a1: v21, fType: p_pvarg);`
  - `if ( v6 != v7 )`
  - `sub_428902(a1: a1 + 387);`
  - `if ( v28 != 0 )`
  - `(*(*v28 + 8))(a1: v28);`
  - `if ( v27 != 0 )`
  - `(*(*v27 + 8))(a1: v27);`
  - `v8 = *get_basic_font(a1: &v27, fType: 6);`
  - `if ( v9 != v8 )`
  - …（截断 14 行）

### <free>::0CWndSkillGuide  @ 0x8aa243  (4550 B)

  - `CWndSkillGuide *__thiscall CWndSkillGuide::CWndSkillGuide(CWndSkillGuide *this, int Unknown)`
  - `CWnd::CWnd(this);`
  - `*(this + 1) = &off_B3B4F8;`
  - `*(this + 2) = &off_B3B4F4;`
  - `CWnd::CreateWnd(`
  - `LOBYTE(v33) = 1;`
  - `sub_416821(a1: &v31, a2: str_Ui_20, a3: Unknown - 1);`
  - `VariantInit(&pvarg);`
  - `v3 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v3 < 0 )`
  - `sub_402F85(pvargDest: &v25, pvargSrc: &pvargSrc);`
  - `v4 = sub_41468C(a1: &v31);`
  - `_bstr_t::_bstr_t(a1: v4);`
  - `v6 = sub_406472();`
  - …（截断 14 行）

### <free>::get_novice_skill_point  @ 0x765e9e  (2085 B)

  - `int __cdecl get_novice_skill_point(int cd)`
  - `if ( ZMap<long,long,long>::GetAt(a1: &key, a2: &nValue) != 0 )`
  - `if ( ZMap<long,long,long>::GetAt(a1: &cd, a2: &nValue) != 0 )`
  - `v3 = _ZtlSecureFuse<unsigned char>(a1: v1 + 51, a2: *(v1 + 53));`
  - `if ( v3 >= 7u )`

### <free>::get_random_magic_attack_action  @ 0x7662f5  (997 B)

  - `int __cdecl get_random_magic_attack_action(int a1, int a2, char **a3, int a4, unsigned int a5)`
  - `if ( a1 < 1 || a1 > 9 )`
  - `if ( a3 != 0 )`
  - `if ( !SKILLENTRY::IsCorrectWeaponType(this: a3, a2, a3: v10) )`
  - `if ( SKILLENTRY::IsActionAppointed(this: a3, a2: v5, a3: a4) )`
  - `if ( a3[3] != 3 )`
  - `return SKILLENTRY::GetRandomAppointedAction(this: a3, a2: v6, a3: a4, a4: a5);`
  - `if ( a3[3] == 3 )`
  - `if ( v9 > 0 )`

### <free>::get_random_melee_attack_action  @ 0x766031  (997 B)

  - `int __cdecl get_random_melee_attack_action(int a1, int a2, char **a3, int a4, unsigned int a5)`
  - `if ( a1 < 1 || a1 > 9 )`
  - `if ( a3 != 0 )`
  - `if ( !SKILLENTRY::IsCorrectWeaponType(this: a3, a2, a3: v10) )`
  - `if ( SKILLENTRY::IsActionAppointed(this: a3, a2: v5, a3: a4) )`
  - `if ( a3[3] != 3 )`
  - `return SKILLENTRY::GetRandomAppointedAction(this: a3, a2: v6, a3: a4, a4: a5);`
  - `if ( a3[3] == 3 )`
  - `if ( v9 > 0 )`

### <free>::get_random_shoot_attack_action  @ 0x766272  (997 B)

  - `int __cdecl get_random_shoot_attack_action(int a1, int a2, char **a3, int a4, unsigned int a5)`
  - `if ( a1 < 1 || a1 > 9 )`
  - `if ( a3 != 0 )`
  - `if ( !SKILLENTRY::IsCorrectWeaponType(this: a3, a2, a3: v10) )`
  - `if ( SKILLENTRY::IsActionAppointed(this: a3, a2: v5, a3: a4) )`
  - `if ( a3[3] != 3 )`
  - `return SKILLENTRY::GetRandomAppointedAction(this: a3, a2: v6, a3: a4, a4: a5);`
  - `if ( a3[3] == 3 )`
  - `if ( v9 > 0 )`

### <free>::get_skill_root_from_job  @ 0x4a8c4f  (963 B)

  - `int __cdecl get_skill_root_from_job(int a1)`
  - `result = get_job_name(a1);`
  - `if ( result != 0 )`
  - `v2 = 100 * (result + 10 * (a1 / 1000));`
  - `*ZArray<long>::InsertBefore(a1: -1) = v2;`
  - `*ZArray<long>::InsertBefore(a1: -1) = v3;`
  - `for ( i = 1; i <= 8; ++i )`
  - `if ( a1 % 10 < i )`
  - `result = ZArray<long>::InsertBefore(a1: -1);`

### <free>::is_able_to_map_macrosys_skill  @ 0x4fb292  (520 B)

  - `BOOL __cdecl is_able_to_map_macrosys_skill(UnwindMapEntry *nSkillID)`
  - `&& !is_keydown_skill(a1: nSkillID)`
  - `&& nSkillID != (&stru_B8A588.toState + 3)`
  - `&& nSkillID != (&loc_A9634B + 2)`

### <free>::is_antirepeat_buff_skill  @ 0x96d6ca  (2304 B)

  - `BOOL __cdecl is_antirepeat_buff_skill(char *a1)`
  - `if ( a1 > 2321005 )`
  - `if ( a1 > &byte_4E23F1 )`
  - `if ( a1 <= &byte_D72A0B )`
  - `|| a1 > (&stru_B8A580.action + 3) && a1 <= (&stru_B8A588.toState + 1);`
  - `if ( a1 == &byte_E6935D || a1 == 21121000 || a1 == 22171000 )`
  - `if ( a1 == &byte_4E23F1 )`
  - `if ( a1 == 4121000 || a1 == &loc_401A2A + 1 || a1 == &loc_406846 + 2 )`
  - `if ( a1 == 4111001 || a1 == 3121000 || a1 == 3121002 || a1 == 3221000 )`
  - `if ( a1 == 2321005 )`
  - `if ( a1 == 2301004 || a1 == 2311001 || a1 == 2311003 )`
  - `if ( a1 <= 1211009 )`
  - `if ( a1 == 1211009 || a1 == 1001003 || a1 == 1101006 || a1 == 1111007 || a1 == 1121000 )`
  - `if ( a1 != 1221000 )`
  - …（截断 14 行）

### <free>::is_correct_job_for_skill_root  @ 0x7669dd  (208 B)

  - `BOOL __cdecl is_correct_job_for_skill_root(int a1, int a2)`
  - `if ( a2 % 100 == 0 )`

### <free>::is_correct_magic_attack  @ 0x766525  (1188 B)

  - `BOOL __cdecl is_correct_magic_attack(int a1, int a2, int a3, char **a4, int a5)`
  - `if ( a1 < 1 || a1 > 9 )`
  - `if ( a4 != 0 )`
  - `if ( !SKILLENTRY::IsCorrectWeaponType(this: a4, a2, a3: v13) )`
  - `if ( SKILLENTRY::IsActionAppointed(this: a4, a2: v5, a3: a5) )`
  - `if ( a4[3] != 3 )`
  - `return SKILLENTRY::IsCorrectAppointedAction(this: a4, a2: v6, a3: a5, a4: a3);`
  - `if ( a4[3] == 3 )`
  - `if ( v10 <= 0 )`
  - `for ( i = &dword_BDB2AC[v9]; *i != a3; ++i )`
  - `if ( ++v11 >= v10 )`

### <free>::is_correct_melee_attack  @ 0x766379  (1254 B)

  - `BOOL __cdecl is_correct_melee_attack(int a1, int a2, int a3, char **a4, int a5)`
  - `if ( a1 < 1 || a1 > 9 )`
  - `if ( a3 == 32 )`
  - `if ( a4 != 0 )`
  - `if ( !SKILLENTRY::IsCorrectWeaponType(this: a4, a2, a3: v13) )`
  - `if ( SKILLENTRY::IsActionAppointed(this: a4, a2: v6, a3: a5) )`
  - `if ( a4[3] != 3 )`
  - `return SKILLENTRY::IsCorrectAppointedAction(this: a4, a2: v7, a3: a5, a4: a3);`
  - `if ( a4[3] == 3 )`
  - `if ( v10 <= 0 )`
  - `for ( i = &dword_BDAFDC[v9]; *i != a3; ++i )`
  - `if ( ++v11 >= v10 )`

### <free>::is_correct_normal_attack  @ 0x7665b1  (355 B)

  - `BOOL __cdecl is_correct_normal_attack(int a1, int a2, int a3, char **a4, int a5)`
  - `if ( is_correct_melee_attack(a1, a2, a3, a4, a5) )`
  - `result = is_correct_shoot_attack(a1, a2, a3, a4, a5);`
  - `if ( result )`

### <free>::is_correct_shoot_attack  @ 0x76649c  (1188 B)

  - `BOOL __cdecl is_correct_shoot_attack(int a1, int a2, int a3, char **a4, int a5)`
  - `if ( a1 < 1 || a1 > 9 )`
  - `if ( a4 != 0 )`
  - `if ( !SKILLENTRY::IsCorrectWeaponType(this: a4, a2, a3: v13) )`
  - `if ( SKILLENTRY::IsActionAppointed(this: a4, a2: v5, a3: a5) )`
  - `if ( a4[3] != 3 )`
  - `return SKILLENTRY::IsCorrectAppointedAction(this: a4, a2: v6, a3: a5, a4: a3);`
  - `if ( a4[3] == 3 )`
  - `if ( v10 <= 0 )`
  - `for ( i = &dword_BDB18C[v9]; *i != a3; ++i )`
  - `if ( ++v11 >= v10 )`

### <free>::is_heros_will_skill  @ 0x765e2d  (552 B)

  - `int __cdecl is_heros_will_skill(int a1)`
  - `if ( a1 > 3221008 )`
  - `if ( a1 == 4121009 || a1 == (&loc_40684F + 1) || a1 == &byte_4E23F0 || a1 == &loc_4FAA92 || a1 == 21121008 )`
  - `if ( a1 == 3221008`

### <free>::is_keydown_skill  @ 0x4fb08f  (823 B)

  - `BOOL __cdecl is_keydown_skill(__int16 *a1)`
  - `if ( a1 > (&loc_4DD5CB + 1) )`
  - `if ( a1 == (&loc_4F5C69 + 1)`
  - `|| a1 == (&loc_4FAA8B + 1)`
  - `if ( a1 == (&loc_4DD5CB + 1) )`
  - `if ( a1 > 2221001 )`
  - `if ( a1 == 2321001 || a1 == 3121004 )`
  - `if ( a1 == 2221001 || a1 == 1121001 || a1 == 1221001 || a1 == 1321001 )`

### <free>::is_skill_need_master_level  @ 0x4e8f04  (383 B)

  - `BOOL __cdecl is_skill_need_master_level(int a1)`
  - `if ( a1 / 10000 / 100 == 22 || v1 == 2001 )`
  - `return get_job_level(nJob: a1 / 10000) == 9 || get_job_level(nJob: v1) == 10;`
  - `if ( v1 % 100 == 0 )`

### <free>::play_skill_sound  @ 0x989729  (5386 B)

- SP_ 节点：`SP_2194_USE2`, `SP_2196_SOUND_SKILLIMG_07D_S`, `SP_2820_SUMMONED`, `SP_2827_DELAYEDHIT`, `SP_3712_LOOP`, `SP_3856_USE3`, `SP_3857_USE4`, `SP_3858_USE5`, `SP_5543_HIT`, `SP_5596_USE`
  - `int __cdecl play_skill_sound(char a1, wchar_t *a2)`
  - `switch ( a2 )`
  - `LOBYTE(v36) = 1;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `ZXString<unsigned short>::operator=(a1: v7);`
  - `ZXString<unsigned short>::operator=(a1: v9);`
  - `ZXString<unsigned short>::operator=(a1: v11);`
  - `ZXString<unsigned short>::operator=(a1: v13);`
  - `ZXString<unsigned short>::operator=(a1: v15);`
  - `ZXString<unsigned short>::operator=(a1: v17);`
  - `ZXString<unsigned short>::operator=(a1: v19);`
  - `ZXString<unsigned short>::operator=(a1: v21);`
  - `if ( v5 != 0 )`
  - `sub_416821(a1: &a2, a2: v23, a3: a1);`
  - …（截断 14 行）

### <free>::stop_skill_sound  @ 0x98999f  (101 B)

  - `int __cdecl stop_skill_sound(unsigned int a1)`
  - `return CSoundMan::StopSE(a1, a2: 0);`

### ?$ZList::RemoveAll  @ 0x672ca1  (448 B)

  - `int __thiscall ZList<CMob::AFFECTEDSKILLENTRY>::RemoveAll(int *this)`
  - `for ( result = *(this + 3); result != 0; result = v4 != 0 ? v4 + 16 : 0 )`
  - `v4 = *(v3 + 4);`
  - `sub_402D2E(a1: v3);`
  - `*(this + 4) = 0;`

### CAnimationDisplayer::Effect_SkillUse  @ 0x43d72a  (4737 B)

- SP_ 节点：`SP_966_SS`
  - `int __thiscall CAnimationDisplayer::Effect_SkillUse(`
  - `for ( i = -1; i <= a7; ++i )`
  - `if ( i < 0 )`
  - `v8 = alloca(32);`
  - `v9 = _itow(a1: i, a2: &v23, a3: 10);`
  - `HIDWORD(v25) = v32;`
  - `LOBYTE(v36) = 3;`
  - `sub_416821(a1: &v33, a2: v11, a3: a2);`
  - `sub_4145AB(a1: &a5);`
  - `CAnimationDisplayer::LoadLayer(a1: &a4);`
  - `v14 = sub_41468C(a1: &v33);`
  - `sub_43E86F(`
  - `a9: SHIDWORD(v25),`
  - `if ( v35 == 0 )`
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

### CDraggableSkill::OnDoubleClicked  @ 0x4fb001  (1185 B)

  - `signed int __thiscall CDraggableSkill::OnDoubleClicked(__int16 **this)`
  - `v2 = *(this + 10);`
  - `if ( v2 != 0 && (*(*v2 + 72))(a1: v2, a2: &dword_BF1078) != 0 && (v3 = *(this + 10)) != 0 )`
  - `if ( v4 == 0 )`
  - `v5 = *(this + 10);`
  - `if ( v5 != 0 && (*(*v5 + 72))(a1: v5, a2: &dword_BF109C) != 0 && (v6 = *(this + 10)) != 0 )`
  - `if ( v7 == 0 )`
  - `if ( TSingleton<CUserLocal>::ms_pInstance == 0 )`
  - `v9 = *(this + 6);`
  - `if ( v9 < 0 || dword_BE7910 != 0 )`
  - `if ( is_keydown_skill(a1: v9) == 0 )`
  - `CUserLocal::DoActiveSkill(this: v8, nSkillID: *(this + 6), nScanCode: 0, pnConsumeCheck: 0);`

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

### CField::IsSkillForbiden  @ 0x537c7f  (112 B)

  - `BOOL __thiscall CField::IsSkillForbiden(CField *this, int a2)`
  - `return sub_587F5F(a1: a2) != 0;`

### CField::RestoreForbiddenSkill  @ 0x52b1d8  (1823 B)

  - `_DWORD *__thiscall CField::RestoreForbiddenSkill(char *this)`
  - `_bstr_t::_bstr_t(a1: aNoskill);`
  - `sub_40263B(a1: this + 48);`
  - `IWzProperty::GetItem(pvargDest: &pvarg, a2: v7);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v8, a2: v9);`
  - `if ( Unknown != 0 )`
  - `v3 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &v12);`
  - `if ( v3 < 0 && v3 != -2147467262 )`
  - `LOBYTE(v14) = 3;`
  - `if ( v4 < 0 )`
  - `v5 = (*(*this + 28))(a1: this);`
  - `Field::SkillInfo::SetFieldType(this: (this + 1904), a2: v5);`
  - `if ( v13 != 0 )`
  - `(*(*v13 + 4))(a1: v13);`
  - …（截断 14 行）

### CMob::TryDoingSkill  @ 0x66e7af  (2137 B)

  - `int __thiscall CMob::TryDoingSkill(CMob *this, int *a2, int *a3, struct CMob::TARGETINFO *a4)`
  - `if ( *(this + 169) != 0 )`
  - `v6 = *(this + 81);`
  - `if ( v6 == 0 || *(this + 227) != 0 )`
  - `v7 = *(this + 98);`
  - `for ( i = 0; ; ++i )`
  - `v9 = *(v7 + 636);`
  - `if ( v9 == 0 || i >= *(v9 - 4) )`
  - `if ( v6 == *(v5 + v9 + 12) )`
  - `v10 = (*(v7 + 636) + 28 * i);`
  - `if ( v10 == 0 )`
  - `v11 = sub_663FE2(a1: this);`
  - `BYTE1(v11) = *(this + 328);`
  - `LOBYTE(v11) = *(this + 324);`
  - …（截断 14 行）

### CMob::UpdateAffectedSkillList  @ 0x665cfd  (5747 B)

  - `int __thiscall CMob::UpdateAffectedSkillList(CMob *this, int a2)`
  - `v3 = *(this + 114) == 0;`
  - `if ( !v3 )`
  - `sub_47CCB7(a1: this + 460, a2: &v12);`
  - `if ( *(this + 118) != 0 )`
  - `sub_47CCB7(a1: &v13, a2: &v12);`
  - `sub_47CCB7(a1: this + 872, a2: &v11);`
  - `v4 = *(this + 47);`
  - `while ( v4 != 0 )`
  - `v6 = *(v4 != 0 ? v4 - 3 : 4);`
  - `if ( sub_474D3B(a1: v4, a2: 0) != 0 )`
  - `sub_6729B6(a1: v5);`
  - `else if ( sub_76662D(a1: *v5) == 0 )`
  - `if ( v5[2] != 0 )`
  - …（截断 14 行）

### CPet::GetUpgradePetSkill  @ 0x705db3  (534 B)

  - `__int16 __thiscall CPet::GetUpgradePetSkill(CPet *this)`
  - `ItemSlot = CPet::GetItemSlot(this);`
  - `if ( ItemSlot != 0 )`
  - `InterlockedIncrement(lpAddend: (ItemSlot + 4));`
  - `if ( v4 != 0 )`
  - `v2 = sub_4E81DB(a1: v4 + 81, a2: *(v4 + 85));`
  - `sub_4F5818(a1: 0);`

### CSkillInfo::CheckConsumeForActiveSkill  @ 0x764256  (7443 B)

  - `signed int __thiscall CSkillInfo::CheckConsumeForActiveSkill(`
  - `if ( (v5 == 0 || v5 == 9)`
  - `&& sub_764721(a1: nSkillID) == 0 )`
  - `cda = CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: cd, a3: nSkillID, a4: &nIncMPCon);`
  - `if ( nSkillID == 21000002 )`
  - `SkillLevel = CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: cd, a3: 21120002, a4: 0);`
  - `if ( SkillLevel != 0 )`
  - `CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: cd, a3: 21120009, a4: &v37);`
  - `else if ( nSkillID == 21100001 )`
  - `if ( cda <= 0 )`
  - `if ( nSkillID == 1121010 && _ZtlSecureFuse<long>(a1: a4 + 219, a2: a4[221]) < 10 )`
  - `LevelData = SKILLENTRY::GetLevelData(a1: cda);`
  - `get_amplification(cd, nSkillID, &pnIncMPCon);`
  - `v36 = _ZtlSecureFuse<long>(a1: LevelData + 37, a2: LevelData[39]);`
  - …（截断 14 行）

### CSkillInfo::GetSkill  @ 0x75c755  (514 B)

  - `int __thiscall CSkillInfo::GetSkill(CSkillInfo *this, int a2)`
  - `if ( a2 / 10000 / 100 == 22 || a2 / 10000 == 2001 )`
  - `sub_76716A(a1: &a2, a2: v4);`
  - `if ( v5 != 0 )`
  - `sub_766D3C(a1: 0);`

### CSkillInfo::GetSkillLevel  @ 0x7616f6  (2217 B)

  - `int __thiscall CSkillInfo::GetSkillLevel(`
  - `if ( a3 / 10000 / 100 == 22 || a3 / 10000 == 2001 )`
  - `if ( a3 > 20001011 )`
  - `if ( a3 == 20001020 )`
  - `if ( !v4 )`
  - `if ( a4 != 0 )`
  - `*v8 = CSkillInfo::GetSkill(this: v9, a2: a3);`
  - `if ( a3 >= 20001009 )`
  - `if ( a3 <= 1011 || a3 == 1020 )`
  - `if ( a3 > &loc_989A70 )`
  - `if ( a3 <= &loc_989A70 + 3 )`
  - `if ( Skill == 0 )`
  - `Skill = CSkillInfo::GetSkill(this: v9, a2: a3);`
  - `if ( v5 != 0 && *v5 == 0 )`
  - …（截断 14 行）

### CSkillInfo::GetSkillRootVisible  @ 0x75c7c5  (1015 B)

  - `int __thiscall CSkillInfo::GetSkillRootVisible(`
  - `v4 = sub_75C70A(a1: a2);`
  - `ZXString<char>::operator=(a1: v4 + 1);`
  - `sub_4051E5(a1: v6[3]);`
  - `a4 = (v5 + 8);`
  - `sub_766FFC();`
  - `for ( i = 0; ; ++i )`
  - `if ( result == 0 || i >= *(result - 4) )`
  - `a2 = *(result + 8 * i + 4);`
  - `if ( sub_7619B0(a1: a3, a2: *a2, a3: &a2) != 0 )`
  - `sub_767038(a1: -1);`
  - `sub_766B42(a1: v9);`

### CUISkill::CloseSkillGuide  @ 0x8aa4c5  (295 B)

  - `int __thiscall CUISkill::CloseSkillGuide(CUISkill *this)`
  - `v2 = *(this + 411);`
  - `if ( v2 != 0 )`
  - `CWnd::Destroy(a1: v2);`
  - `return sub_8ADF92(a1: this + 1640);`

### CUISkill::GetSkillRootVisible  @ 0x8bcdfa  (594 B)

  - `char *__thiscall CUISkill::GetSkillRootVisible(CUISkill *this, int a2)`
  - `if ( a2 != 0 )`
  - `v3 = *(*(this + 364) + 4 * *(*(this + 366) + 60));`
  - `v4 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v6) + 4);`
  - `if ( v6[1] != 0 )`
  - `CSkillInfo::GetSkillRootVisible(this: dword_BE78DC, a2: v3, a3: v4, a4: (this + 1524));`

### CUISkill::OnCreate  @ 0x8aaa88  (15602 B)

- SP_ 节点：`SP_1923_UI_UIWINDOWIMG_SKILL_BTSPUP`, `SP_1924_UI_UIWINDOWIMG_SKILL_SKILL0`, `SP_1925_UI_UIWINDOWIMG_SKILL_SKILL1`, `SP_1926_UI_UIWINDOWIMG_SKILL_RECOMMEND_0`, `SP_2660_UI_UIWINDOWIMG_SKILL_LINE`, `SP_4341_YOU_MAY_OPEN_CLOSE_YOUR_OWN_SKILL_WINDOW`, `SP_4344_UI_BASICIMG_BTMACRO`
  - `void __thiscall CUISkill::OnCreate(CUISkill *this, int Unknown)`
  - `sub_41C06C(a1: &word_AF0E08, a2: -1);`
  - `CUIWnd::OnCreate(a1: Unknown, a2: v73.cyVal.Hi);`
  - `p_Hi = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1452);`
  - `if ( p_Hi != 0 )`
  - `v73.cyVal.Hi = (*(*(this + 1) + 48))(a1: this + 4);`
  - `v3 = sub_8AB496(a1: *(this + 396));`
  - `v4 = *(this + 1);`
  - `v5 = (*(v4 + 44))(a1: this + 4);`
  - `v6 = CUIMacroSys::CUIMacroSys(a1: p_Hi, a2: SHIDWORD(v5), a3: Unknown + v5 - 2, a4: v73.cyVal.Hi);`
  - `sub_8ADEE7(a1: v6);`
  - `if ( *(this + 396) == 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - …（截断 14 行）

### CUISkill::OnSkillLevelUpButton  @ 0x8acf1a  (7860 B)

- 分支 case：`0`, `1`, `2`
- SP_ 节点：`SP_3509_YOURE_LACKING_LEVEL_1_OR_2_SKILLS`, `SP_3510_YOURE_LACKING_LEVEL_1_SKILLS`, `SP_3511_YOU_ARE_LACKING_IN_EITHER_1ST_2ND_OR_3RD_LEVEL_SKILLS`
- 节流 `CanSendExclRequest(500)`
  - `void __thiscall CUISkill::OnSkillLevelUpButton(CUISkill *this, int *a2)`
  - `if ( CWvsContext::CanSendExclRequest(a1: 500, a2: 0) != 0 )`
  - `v4 = *(CWvsContext::GetCharacterData(this: v2, a2: &v35) + 4);`
  - `if ( v36 != 0 )`
  - `if ( (*(*(this + 366) + 60) == 0 || _ZtlSecureFuse<short>(a1: v4 + 137, a2: *(v4 + 141)) > 0)`
  - `&& (*(*(this + 366) + 60) != 0 || get_novice_skill_point(cd: v4) > 0) )`
  - `v5 = a2 + *(*(this + 368) + 56);`
  - `v6 = sub_8ADA59(this, a2: 0);`
  - `if ( v6 != 0 && v5 >= 0 )`
  - `v7 = *(v6 + 8);`
  - `if ( v7 != 0 && v5 < *(v7 - 4) )`
  - `v8 = *(v7 + 8 * v5 + 4);`
  - `if ( CWvsContext::GetSkillLevelUpState(this: v38, a2: v8) == 1 && *(v8 + 11) == 0 )`
  - `if ( v9 % 100 != 0 )`
  - …（截断 14 行）

### CUISkill::OpenCurSkillGuide  @ 0x961063  (673 B)

  - `int __thiscall CUISkill::OpenCurSkillGuide(CUISkill *this)`
  - `result = *(this + 364);`
  - `if ( result != 0 && *(result - 4) >= 2u )`
  - `v3 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 108);`
  - `if ( v3 != 0 )`
  - `v4 = *(this + 364);`
  - `if ( v4 != 0 )`
  - `v4 = *(v4 - 4);`
  - `v5 = CWndSkillGuide::CWndSkillGuide(this: v3, a2: v4 - 1);`
  - `return sub_8ADF31(a1: v5);`

### CUISkillChangeConfirm::GetResult  @ 0xa0f8c8  (252 B)

  - `int __thiscall CUISkillChangeConfirm::GetResult(CUISkillChangeConfirm *this, int *a2, int *a3)`
  - `*a2 = *(this + 39);`
  - `result = *(this + 38);`

### CUISkillEx::OnButtonClicked  @ 0x8bb2bf  (827 B)

- **c2s opcode `COutPacket(165)`**
  - `void __thiscall CUISkillEx::OnButtonClicked(CUISkillEx *this, unsigned int a2)`
  - `if ( a2 >= 0x7DA && a2 < 0x7DE )`
  - `CUISkillEx::OnSkillLevelUpButton(this, a2: a2 - 2010);`
  - `if ( a2 == 2023 )`
  - `CUISkillEx::ShiftMacroUIState(this, a2: 0);`
  - `CUIWnd::OnButtonClicked(a1: a2);`
  - `if ( a2 != 2024 )`
  - `COutPacket::COutPacket(a1: 165);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: v2);`

### CUISkillEx::OnSkillLevelUpButton  @ 0x8bc62f  (1382 B)

- 节流 `CanSendExclRequest(500)`
  - `void __thiscall CUISkillEx::OnSkillLevelUpButton(CUISkillEx *this, int a2)`
  - `if ( CWvsContext::CanSendExclRequest(a1: 500, a2: 0) != 0 )`
  - `v4 = *(CWvsContext::GetCharacterData(this: v2, a2: v9) + 4);`
  - `if ( v9[1] != 0 )`
  - `if ( (*(*(this + 366) + 60) == 0 || sub_4E26B5(a1: *(*(this + 366) + 60)) != 0)`
  - `&& (*(*(this + 366) + 60) != 0 || get_novice_skill_point(cd: v4) > 0) )`
  - `v5 = a2 + 2 * *(*(this + 368) + 56);`
  - `SkillRootVisible = CUISkill::GetSkillRootVisible(this, a2: 0);`
  - `if ( SkillRootVisible != 0 && v5 >= 0 )`
  - `v7 = *(SkillRootVisible + 2);`
  - `if ( v7 != 0 && v5 < *(v7 - 4) )`
  - `v8 = *(v7 + 8 * v5 + 4);`
  - `if ( CWvsContext::GetSkillLevelUpState(this: v10, a2: v8) == 1 && *(v8 + 11) == 0 )`
  - `CWvsContext::SendSkillUpRequest(this: v10, a2: *v8);`
  - …（截断 14 行）

### CUISkillEx::ShiftMacroUIState  @ 0x8bad5a  (9527 B)

- SP_ 节点：`SP_4341_YOU_MAY_OPEN_CLOSE_YOUR_OWN_SKILL_WINDOW`, `SP_4345_UI_UIWINDOWIMG_SKILLMACROEX_BTMACROIN`, `SP_4346_UI_UIWINDOWIMG_SKILLMACROEX_BTMACROOUT`
  - `void __thiscall CUISkillEx::ShiftMacroUIState(CUISkillEx *this, int a2)`
  - `if ( *(this + 403) != 0 )`
  - `v3 = *(this + 380);`
  - `if ( v3 != 0 )`
  - `(*(*v3 + 12))(a1: v3);`
  - `v4 = *(this + 401) == 0;`
  - `memset(v38, 0, sizeof(v38));`
  - `if ( v4 )`
  - `*(this + 401) = 2;`
  - `LOBYTE(v45) = 1;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `if ( v43 != 0 )`
  - `v8 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `if ( v8 != 0 )`
  - …（截断 14 行）

### CUIToolTip::SetToolTip_MacroSysSkill  @ 0x8f2276  (7388 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_4161_UI_UIWINDOWIMG_SKILLMACRO_MACROICON_D_ICON`, `SP_4336_REGISTER_THE_FREQUENTLY_USED_SKILLS_FOR_A_QUICK_ACCESS_WITH_A_SINGLE_BUTTON`
  - `void __thiscall CUIToolTip::SetToolTip_MacroSysSkill(`
  - `if ( *(this + 4) == 0 || *(this + 5) != a2 || *(this + 6) != Unknown )`
  - `CUIToolTip::SetBasicInfo(this, a2: 11, a3: 290, a4: 142, a5: 125);`
  - `CUIToolTip::MakeLayer(this, a2: &v41, a3: a2, a4: Unknown, a5: 1, a6: 0, a7: 0, a8: -1610612672);`
  - `LOBYTE(v44) = 1;`
  - `CUIToolTip::GetFontByType(this, a2: &v35, a3: 1);`
  - `v6 = sub_8ADF76();`
  - `MacroName = CMacroSysMan::GetMacroName(this: v6, a2: &Unknown, a3: v34);`
  - `v8 = sub_428226(a1: MacroName);`
  - `sub_8F466D(a1: 10, a2: v8, a3: v35);`
  - `CUIToolTip::GetFontByType(this, a2: &p_pvargDest, a3: 7);`
  - `v12 = sub_428226(a1: String);`
  - `sub_8F4535(a1: 15, a2: *(this + 3) - 20, a3: 27, a4: v12, a5: p_pvargDest, a6: p_pvarg, a7: v34, a8: v35);`
  - `ZXString<char>::Format(a1: &v42, a2: v14, a3: p_p_pvargDest);`
  - …（截断 14 行）

### CUIToolTip::SetToolTip_Skill  @ 0x8f25d0  (12475 B)

  - `int *__thiscall CUIToolTip::SetToolTip_Skill(int this, int *arg0, int *a3, int *a4, int a5)`
  - `if ( *(this + 16) == 0 || *(this + 20) != arg0 || (result = a3, *(this + 24) != a3) )`
  - `p_wReserved1 = is_skill_need_master_level(a1: *a4);`
  - `CUIToolTip::GetFontByType(a1: &v63, a2: 7);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v65) = 1;`
  - `v7 = sub_428226(a1: a4 + 2);`
  - `_bstr_t::_bstr_t(a1: v7);`
  - `sub_4284A8(a1: &v63);`
  - `*v64 = sub_8EF623(a1: wReserved3, a2: p_a2, a3: v54[0]) - 4;`
  - `if ( *v64 <= 0 )`
  - `if ( v9 < 0 )`
  - `if ( v63 == 0 )`
  - `v11 = sub_485A45(a1: v63);`
  - …（截断 14 行）

### CUser::GetDashingSkill  @ 0x9433a2  (462 B)

  - `char *__thiscall CUser::GetDashingSkill(CUser *this)`
  - `(*(*this + 8))(a1: this);`
  - `v2 = (*(*this + 64))(a1: this);`
  - `v4 = (*(*this + 8))(a1: this, a2: v2 % 1000);`
  - `if ( SecondaryStat::IsEventVehicle(a1: v4, a2: SHIDWORD(v4)) )`
  - `return (v3 ? 10001015 : 1014);`
  - `return &loc_4C4F29 + (v3 ? &loc_98967D + 1 : 0) + 4;`

### CUser::SetAttackAction  @ 0x92edb2  (2864 B)

- SP_ 节点：`SP_4374_THIS_SKILL_CANNOT_BE_USED_WITH_THE_EQUIPPED_ITEM`
  - `signed int __thiscall CUser::SetAttackAction(CUser *this, int a2, int a3, char **dragon_action_code_from_name, int *a5)`
  - `*(this + 347) = 5000;`
  - `if ( CAvatar::GetOneTimeAction(this: (this + 136)) > -1 )`
  - `weapon_type = get_weapon_type(nItemID: *(this + 315));`
  - `if ( !is_correct_normal_attack(a1: *(this + 318), a2: weapon_type, a3: a2, a4: dragon_action_code_from_name, a5)`
  - `&& !is_correct_magic_attack(a1: *(this + 318), a2: weapon_type, a3: a2, a4: dragon_action_code_from_name, a5) )`
  - `if ( dragon_action_code_from_name != 0 )`
  - `if ( !SKILLENTRY::IsCorrectWeaponType(this: dragon_action_code_from_name, a2: weapon_type, a3: v18[1]) )`
  - `CHATLOG_ADD(sString: String, nType: 0xCu);`
  - `v7 = (*(*this + 64))(a1: this);`
  - `if ( v7 / 100 != 21 && v7 != 2000 )`
  - `if ( a2 < 122 || a2 > 124 )`
  - `v11 = *(this + 34);`
  - `*(this + 349) = a2;`
  - …（截断 14 行）

### CUser::ShowSkillEffect  @ 0x933990  (1793 B)


### CUser::ShowSkillPrepare  @ 0x93640b  (2300 B)

  - `void *__thiscall CUser::ShowSkillPrepare(CUser *this, const struct SKILLENTRY *a2, int a3, signed int *a4, int *a5)`
  - `if ( *(this + 346) == 0 )`
  - `a2 = *(a2 + 36);`
  - `sub_413878(a1: &a2);`
  - `if ( a4 != 0 )`
  - `sub_413878(a1: v7 + 148);`
  - `sub_402D9A(this: &a2);`
  - `a2 = *(v7 + 37);`
  - `if ( a5 != 0 )`
  - `v8 = (v7 + 152);`
  - `sub_413878(a1: v8);`
  - `if ( v9 != 0 && *v9 != 0 )`
  - `result = (*(*v9 - 4) >> 1);`
  - `if ( result != 0 )`
  - …（截断 14 行）

### CUser::ShowSkillSpecialEffect  @ 0x936528  (5494 B)

- SP_ 节点：`SP_958_SHAPE2DVECTOR2D`
  - `__int16 **__userpurge CUser::ShowSkillSpecialEffect@<eax>(`
  - `if ( a2 != 0 && *(this + 346) == 0 )`
  - `SKILLENTRY::GetSpecialUOL(this: a2, a2: &v41);`
  - `if ( v41 != 0 && *v41 != 0 )`
  - `result = (*(*v41 - 4) >> 1);`
  - `if ( result != 0 )`
  - `v11 = *v8 == (&loc_4F5C69 + 1);`
  - `LOBYTE(v42) = 1;`
  - `if ( v11 || v10 == &word_D7511E )`
  - `PcCreateObject::IWzVector2D(a1: v15, &a2, a3: 0);`
  - `if ( v39 != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `v16 = sub_936778(a1: v38);`
  - …（截断 14 行）

### CUserLocal::DoActiveSkill  @ 0x966f7a  (1878 B)


### CUserLocal::DoActiveSkill_BoundJump  @ 0x96be38  (5878 B)

- SP_ 节点：`SP_2728_EFFECT_BASICEFFIMG_FLYING`, `SP_2729_EFFECT_BASICEFFIMG_FLYING1`, `SP_5100_EFFECT_BASICEFFIMG_SOULRUSH`, `SP_884_YOU_CANNOT_USE_THAT_SKILL_ON_THIS_MAP`
  - `signed int __thiscall CUserLocal::DoActiveSkill_BoundJump(CUserLocal *this, char **a2, int a3)`
  - `if ( (TSecType<unsigned char>::GetData(this: (*dword_BE7918 + 8240)) & 1) == 0 )`
  - `v34 = (*(get_field() + 324) & 2) != 0;`
  - `if ( v34 != 0 )`
  - `CHATLOG_ADD(sString: String, nType: 0xCu);`
  - `if ( *(this + 2774) != 0`
  - `|| CUserLocal::IsImmovable(this)`
  - `|| CUserLocal::IsAttract(this)`
  - `|| CAvatar::GetOneTimeAction(this: (this + 136)) > -1 )`
  - `v7 = *(this + 1129) != 0 ? (*(this + 1129) - 12) : 0;`
  - `if ( v37 != 4111006 )`
  - `if ( v8 == &loc_A9634B + 2 )`
  - `if ( *(v7 + 68) == 0`
  - `|| CUserLocal::IsWeakened(a1: this) != 0`
  - …（截断 14 行）

### CUserLocal::DoActiveSkill_CombatStep  @ 0x96d95e  (2787 B)

  - `signed int __thiscall CUserLocal::DoActiveSkill_CombatStep(CUserLocal *this, char **a2, int a3)`
  - `if ( a3 == 0 || a2 == 0 || sub_958143(this: *dword_BE7918) == 0 && (*(get_field() + 324) & 2) != 0 )`
  - `if ( CUserLocal::IsImmovable(this) )`
  - `if ( CUserLocal::IsAttract(this) )`
  - `if ( *(*(this + 1129) != 0 ? *(this + 1129) - 12 + 0x110 : 272) == 0 )`
  - `v18 = *((*(*(this + 1) + 16))(a1: this + 4) + 4);`
  - `v5 = (*(*(this + 1) + 16))(a1: this + 4);`
  - `Portal_Collision = CPortalList::FindPortal_Collision(this: v4, a2: SHIDWORD(v5), a3: *v5, a4: v18);`
  - `if ( Portal_Collision != 0 )`
  - `if ( v7 == 12 || v7 == 13 )`
  - `update_time = get_update_time();`
  - `if ( ZMap<long,long,long>::GetAt(a1: a2, a2: &v21) != 0 && update_time < v21 )`
  - `if ( *(*(this + 1129) != 0 ? *(this + 1129) - 12 + 0x188 : 392) != 0`
  - `|| CUserLocal::SendSkillUseRequest(`
  - …（截断 14 行）

### CUserLocal::DoActiveSkill_Heal  @ 0x96a69d  (3202 B)

- **c2s opcode `COutPacket(91)`**
- c2s 编码：`Encode4`, `Encode1`, `Encode2`
- 节流 `CanSendExclRequest(300)`
  - `int __thiscall CUserLocal::DoActiveSkill_Heal(CUserLocal *this, const struct SKILLENTRY *a2, int a3)`
  - `if ( CWvsContext::CanSendExclRequest(a1: 300, a2: 0) == 0 )`
  - `HIDWORD(v5) = *(CWvsContext::GetCharacterData(this: v3, a2: &v19) + 4);`
  - `LODWORD(v5) = a2;`
  - `if ( CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: HIDWORD(v5), a3: *a2, a4: 0) <= 0 )`
  - `if ( v6 == 0 )`
  - `if ( CAvatar::GetOneTimeAction(this: (this + 136)) > -1 )`
  - `VecCtrl = CUser::GetVecCtrl(this, &a2);`
  - `HIDWORD(v5) = *VecCtrl != 0 ? *VecCtrl - 12 : 0;`
  - `if ( a2 != 0 )`
  - `(*(*a2 + 8))(a1: a2);`
  - `if ( CVecCtrl::IsOnLadder(this: HIDWORD(v5)) != 0 || CVecCtrl::IsOnRope(this: HIDWORD(v5)) != 0 )`
  - `COutPacket::COutPacket(a1: 91);`
  - `update_time = get_update_time();`
  - …（截断 14 行）

### CUserLocal::DoActiveSkill_MagicAttack  @ 0x969b10  (443 B)

  - `signed int __thiscall CUserLocal::DoActiveSkill_MagicAttack(IWzGr2DLayer **this, __int64 a2)`
  - `if ( CUserLocal::IsWeaponDisabled(this) == 0 )`
  - `if ( (*(this + 348) & 0xFFFFFFFE) != 0xA )`
  - `return CUserLocal::TryDoingMagicAttack(this, a2, a3: 0, a4: 0);`
  - `CUserLocal::TryDoingMeleeAttack(this, a2: 0, a3: 0, a4: 0, a5: 0, i: 0, a1: 0, StringW: 0);`

### CUserLocal::DoActiveSkill_MeleeAttack  @ 0x969465  (4250 B)

- SP_ 节点：`SP_3537_THIS_SKILL_IS_AVAILABLE_ONLY_WHEN_YOUR_ENERGY_IS_FULLY_CHARGED`, `SP_3538_THIS_SKILL_IS_AVAILABLE_ONLY_DURING_TRANSFORMATION`
  - `int __thiscall CUserLocal::DoActiveSkill_MeleeAttack(`
  - `if ( CUserLocal::IsWeaponDisabled(this) != 0 )`
  - `if ( (*(this + 348) & 0xFFFFFFFE) == 0xA && (a2 == 0 || *a2 != 21110006) )`
  - `if ( a2 == 0 )`
  - `v5 = (*(*this + 8))(a1: this);`
  - `get_weapon_type(nItemID: *(this + 315));`
  - `if ( *v4 > 5111006 )`
  - `if ( v6 < &loc_4E23EA + 2 )`
  - `if ( v6 <= &loc_4E23ED )`
  - `if ( CMorphTemplate::IsSuperMan() != 0`
  - `&& _ZtlSecureFuse<long>(a1: v5 + 1332, a2: *(v5 + 1340)) == (&loc_4E23EA + 1) )`
  - `CHATLOG_ADD(sString: String, nType: 0xCu);`
  - `if ( v6 <= &loc_A98A58 + 1 )`
  - `if ( v6 == &byte_E66C4D || v6 == &byte_E69359 )`
  - …（截断 14 行）

### CUserLocal::DoActiveSkill_Prepare  @ 0x96a86e  (14079 B)

- **c2s opcode `COutPacket(93)`**
- c2s 编码：`Encode4`, `Encode1`, `Encode2`
- 分支 case：`3`, `4`, `5`
- SP_ 节点：`SP_2843_YOU_DO_NOT_HAVE_ENOUGH_BULLETS_TO_USE_THIS_SKILL`, `SP_2844_YOU_CANNOT_USE_THIS_SKILL_WITH_THE_WEAPON_YOURE_EQUIPPED_WITH`, `SP_2849_YOU_HAVE_USED_UP_ALL_OF_YOUR_ARROWS`, `SP_2850_YOU_HAVE_USED_UP_ALL_OF_YOUR_THROWING_STARS`, `SP_4374_THIS_SKILL_CANNOT_BE_USED_WITH_THE_EQUIPPED_ITEM`
- 节流 `CanSendExclRequest(300)`
  - `signed int __fastcall CUserLocal::DoActiveSkill_Prepare(`
  - `update_time = get_update_time();`
  - `if ( CWvsContext::CanSendExclRequest(a1: 300, a2: 0) == 0 )`
  - `v8 = *(CWvsContext::GetCharacterData(this: v7, a2: &v62) + 4);`
  - `if ( v63 != 0 )`
  - `if ( CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v8, a3: v69, a4: 0) <= 0 )`
  - `if ( v9 == 0 || v9 == 9 )`
  - `VecCtrl = CUser::GetVecCtrl(this, a2: &v66);`
  - `v11 = *VecCtrl != 0 ? (*VecCtrl - 12) : 0;`
  - `if ( v66 != 0 )`
  - `(*(*v66 + 8))(a1: v66);`
  - `if ( v69 == (&loc_4F5C69 + 1) )`
  - `if ( v69 != &word_D7511E && v11[68] == 0 && CVecCtrl::IsSwimming(a1: v11) == 0 )`
  - `if ( v69 == (&loc_4F5C69 + 1) || v69 == &word_D7511E )`
  - …（截断 14 行）

### CUserLocal::DoActiveSkill_RecoveryAura  @ 0x96c917  (3840 B)

- **c2s opcode `COutPacket(91)`**
- c2s 编码：`Encode4`, `Encode1`, `Encode2`
- 节流 `CanSendExclRequest(1000)`
  - `int __thiscall CUserLocal::DoActiveSkill_RecoveryAura(CUserLocal *this, const struct SKILLENTRY *a2, signed int a3)`
  - `if ( CWvsContext::CanSendExclRequest(a1: 1000, a2: 0) == 0 )`
  - `dragon_action_code_from_name = *(CWvsContext::GetCharacterData(this: v3, a2: v21) + 4);`
  - `if ( CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: dragon_action_code_from_name, a3: *a2, a4: 0) <= 0 )`
  - `if ( v5 == 0 )`
  - `if ( CAvatar::GetOneTimeAction(this: (this + 136)) > -1 )`
  - `VecCtrl = CUser::GetVecCtrl(this, a2: &v22);`
  - `v7 = *VecCtrl != 0 ? (*VecCtrl - 12) : 0;`
  - `sub_41569E(a1: &v22);`
  - `if ( CVecCtrl::IsOnLadder(this: v7) != 0`
  - `|| CVecCtrl::IsOnRope(this: v7) != 0`
  - `|| *(v7 + 68) == 0 && CVecCtrl::IsSwimming(a1: v7) == 0 )`
  - `COutPacket::COutPacket(a1: 91);`
  - `update_time = get_update_time();`
  - …（截断 14 行）

### CUserLocal::DoActiveSkill_ShootAttack  @ 0x9696a2  (6493 B)

- 分支 case：`3`, `4`, `5`
- SP_ 节点：`SP_2843_YOU_DO_NOT_HAVE_ENOUGH_BULLETS_TO_USE_THIS_SKILL`, `SP_2844_YOU_CANNOT_USE_THIS_SKILL_WITH_THE_WEAPON_YOURE_EQUIPPED_WITH`, `SP_2849_YOU_HAVE_USED_UP_ALL_OF_YOUR_ARROWS`, `SP_2850_YOU_HAVE_USED_UP_ALL_OF_YOUR_THROWING_STARS`, `SP_3537_THIS_SKILL_IS_AVAILABLE_ONLY_WHEN_YOUR_ENERGY_IS_FULLY_CHARGED`, `SP_3538_THIS_SKILL_IS_AVAILABLE_ONLY_DURING_TRANSFORMATION`
  - `signed int __thiscall CUserLocal::DoActiveSkill_ShootAttack(CUserLocal *this, __int64 a2)`
  - `if ( CUserLocal::IsWeaponDisabled(this) != 0 )`
  - `v4 = (*(v3 + 8))(a1: this);`
  - `if ( v25 >= &loc_C80EDE`
  - `&& v25 <= (&loc_C80EDE + 1)`
  - `&& _ZtlSecureFuse<long>(a1: v4 + 1332, a2: *(v4 + 1340)) != &loc_C80EDD )`
  - `CHATLOG_ADD(sString: String, nType: 0xCu);`
  - `if ( v25 > &byte_E6935F )`
  - `if ( v25 != 21100004 && v25 != 21110004 )`
  - `if ( !v9 )`
  - `v24 = *(this + 315);`
  - `weapon_type = get_weapon_type(nItemID: v24);`
  - `if ( (v12 != 3 || weapon_type != 45 && weapon_type != 46)`
  - `&& (v12 != 4 || weapon_type != 47)`
  - …（截断 14 行）

### CUserLocal::DoActiveSkill_SmokeShell  @ 0x96c742  (2579 B)

- **c2s opcode `COutPacket(91)`**
- c2s 编码：`Encode4`, `Encode1`, `Encode2`
- 节流 `CanSendExclRequest(1000)`
  - `int __thiscall CUserLocal::DoActiveSkill_SmokeShell(CUserLocal *this, const struct SKILLENTRY *a2, signed int a3)`
  - `if ( CWvsContext::CanSendExclRequest(a1: 1000, a2: 0) == 0 )`
  - `v18 = *(CWvsContext::GetCharacterData(this: v3, a2: v16) + 4);`
  - `if ( CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v18, a3: *a2, a4: 0) <= 0 )`
  - `if ( v5 == 0 || v5 == 9 || CAvatar::GetOneTimeAction(this: (this + 136)) > -1 )`
  - `VecCtrl = CUser::GetVecCtrl(this, a2: &v17);`
  - `v7 = *VecCtrl != 0 ? (*VecCtrl - 12) : 0;`
  - `if ( v17 != 0 )`
  - `(*(*v17 + 8))(a1: v17);`
  - `if ( CVecCtrl::IsOnLadder(this: v7) != 0`
  - `|| CVecCtrl::IsOnRope(this: v7) != 0`
  - `|| *(v7 + 68) == 0 && CVecCtrl::IsSwimming(a1: v7) == 0 )`
  - `COutPacket::COutPacket(a1: 91);`
  - `update_time = get_update_time();`
  - …（截断 14 行）

### CUserLocal::DoActiveSkill_StatChange  @ 0x969e21  (7624 B)

- SP_ 节点：`SP_2842_YOU_DO_NOT_HAVE_ENOUGH_THROWING_STARS_TO_USE_THIS_SKILL`, `SP_884_YOU_CANNOT_USE_THAT_SKILL_ON_THIS_MAP`
  - `signed int __fastcall CUserLocal::DoActiveSkill_StatChange(`
  - `if ( v7`
  - `v9 = *CUser::GetVecCtrl(this, a2: &HitMobInRect);`
  - `if ( CVecCtrl::IsOnLadder(this: v9 != 0 ? (v9 - 12) : 0) != 0 )`
  - `VecCtrl = CUser::GetVecCtrl(this: v8, a2: &Party);`
  - `v11 = *VecCtrl != 0 ? (*VecCtrl - 12) : 0;`
  - `LOBYTE(v45) = 1;`
  - `v12 = CVecCtrl::IsOnRope(this: v11);`
  - `if ( Party != 0 )`
  - `(*(*Party + 8))(a1: Party);`
  - `if ( v13 != 0 )`
  - `if ( HitMobInRect != 0 )`
  - `(*(*HitMobInRect + 8))(a1: HitMobInRect);`
  - `if ( v14 != 0 )`
  - …（截断 14 行）

### CUserLocal::DoActiveSkill_TownPortal  @ 0x969bf9  (3733 B)

- **c2s opcode `COutPacket(91)`**
- c2s 编码：`Encode4`, `Encode1`, `Encode2`
- SP_ 节点：`SP_3181_YOU_CANNOT_USE_THE_MYSTIC_DOOR_SKILL_NEAR_THE_PORTAL`, `SP_884_YOU_CANNOT_USE_THAT_SKILL_ON_THIS_MAP`
- 节流 `CanSendExclRequest(200)`
  - `signed int __thiscall CUserLocal::DoActiveSkill_TownPortal(IWzGr2DLayer **this, SKILLENTRY *pSkill, signed int nSLV)`
  - `if ( CWvsContext::CanSendExclRequest(a1: 200, a2: 0) == 0 )`
  - `field = get_field();`
  - `if ( (*(field + 324) & 8) != 0 )`
  - `CHATLOG_ADD(sString: String, nType: 0xCu);`
  - `if ( nSLV != 0 )`
  - `if ( *(*(this + 1129) != 0 ? *(this + 1129) - 12 + 0x110 : 272) == 0`
  - `|| *(field + 340) != 0`
  - `|| TSecType<unsigned long>::GetData(a1: field + 312) / 1000000u % 100 == 9`
  - `|| (*(field + 324) & 8) != 0 )`
  - `v22 = *((*(*(this + 1) + 4))(a1: this + 1) + 4);`
  - `v10 = (*(*(this + 1) + 4))(a1: this + 1);`
  - `if ( CPortalList::FindPortal(this: v8, a2: SHIDWORD(v10), a3: *v10, a4: v22, a5: 50) != 0 )`
  - `CHATLOG_ADD(sString: v13, nType: 0xCu);`
  - …（截断 14 行）

### CUserLocal::DoActiveSkill_VehicleShootAttack  @ 0x969a57  (1120 B)

- SP_ 节点：`SP_3539_THIS_SKILL_IS_AVAILABLE_ONLY_WHEN_YOU_ARE_RIDING_THE_BATTLESHIP`
  - `signed int __thiscall CUserLocal::DoActiveSkill_VehicleShootAttack(CUserLocal *this, __int64 a2)`
  - `v3 = *(this + 1129) != 0 ? (*(this + 1129) - 12) : 0;`
  - `v4 = (*(*this + 8))(a1: this);`
  - `if ( !SecondaryStat::IsRidingSkillVehicle(this: v4) )`
  - `CHATLOG_ADD(sString: String, nType: 0xCu);`
  - `if ( *(v3 + 68) == 0 && CVecCtrl::IsSwimming(a1: v3) == 0`
  - `|| CVecCtrl::IsOnLadder(this: v3) != 0`
  - `|| CVecCtrl::IsOnRope(this: v3) != 0`
  - `|| CUserLocal::IsWeaponDisabled(this) != 0 )`
  - `return CUserLocal::TryDoingShootAttack(this, a2, k: 0, j: 0, a5: 0, a6: 0);`

### CUserLocal::DoAntiMacroSkill  @ 0x96d0d1  (2639 B)

- **c2s opcode `COutPacket(104)`**
- c2s 编码：`EncodeStr`
  - `signed int __stdcall CUserLocal::DoAntiMacroSkill(int a1, int a2)`
  - `v13 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 148);`
  - `if ( v13 != 0 )`
  - `v2 = CUIAntiMacroCharacterName::CUIAntiMacroCharacterName();`
  - `v12 = v3 != 0 ? (v3 - 8) : 0;`
  - `LOBYTE(v15) = 0;`
  - `sub_96DC98(a1: v11);`
  - `if ( CDialog::DoModal(a1: v12) == 1 )`
  - `CUIAntiMacroCharacterName::GetResult(this: v12, a2: &sTargetCharacterName);`
  - `RemoteUserByName = CUserPool::FindRemoteUserByName(this: dword_BEBFA8, sName: sTargetCharacterName);`
  - `if ( RemoteUserByName != 0 )`
  - `if ( *(RemoteUserByName + 1388) > 0`
  - `|| CAvatar::GetOneTimeAction(this: (RemoteUserByName + 136)) > -1`
  - `|| CUser::IsStand(a1: v5) == 0 && CUser::IsOnLadderOrRope(this: v5) == 0 )`
  - …（截断 14 行）

### CUserLocal::FindParty  @ 0x96db3f  (2453 B)

  - `int __userpurge CUserLocal::FindParty@<eax>(`
  - `v7 = *(*dword_BE7918 + 11988) == 0;`
  - `if ( v7 )`
  - `v8 = *(this + 348);`
  - `v10 = *((*(*(this + 1) + 16))(a1: this + 4, a2: a3, a3: a4, a4: a2) + 4);`
  - `v11 = (*(*v9 + 16))(a1: v9);`
  - `LevelData = SKILLENTRY::GetLevelData(this: a5, a2: SHIDWORD(v11), a3: a6);`
  - `v13 = sub_4144D4(a1: v24, a2: LevelData + 364);`
  - `if ( (v8 & 1) == 0 )`
  - `sub_45730D(a1: v13, a2: v13 + 2);`
  - `dword_BF0454(a1: v14, a2: v25, a3: v10);`
  - `v15 = (v26 + 11992);`
  - `for ( i = 6; i != 0; --i )`
  - `if ( v16 != 0 )`
  - …（截断 14 行）

### CUserLocal::GetProperBulletPosition  @ 0x949b5d  (7666 B)

  - `BOOL __thiscall CUserLocal::GetProperBulletPosition(int *this, void **a2, int a3, int *a4, signed int *a5, int *a6)`
  - `v8 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v40) + 4);`
  - `result = is_shooting_weapon(nItemID: *(this + 315));`
  - `if ( result )`
  - `if ( a2 == 0`
  - `|| (LevelData = SKILLENTRY::GetLevelData(this: a2, a2: v10, a3),`
  - `(v47 = _ZtlSecureFuse<long>(a1: LevelData + 280, a2: *(LevelData + 288))) == 0)`
  - `&& (v13 = SKILLENTRY::GetLevelData(this: a2, a2: v12, a3),`
  - `(v47 = _ZtlSecureFuse<long>(a1: v13 + 268, a2: *(v13 + 276))) == 0) )`
  - `if ( _ZtlSecureFuse<long>(a1: v6 + 9568, a2: *(v6 + 9576)) != 0 )`
  - `v39 = *(v6 + 10128);`
  - `v14 = (v6 + 10128);`
  - `if ( _ZtlSecureFuse<long>(a1: v15, a2: v39) <= 0 )`
  - `v22 = *(v8 + 1103);`
  - …（截断 14 行）

### CUserLocal::IsTeleportSkillAvailable  @ 0x96cb93  (3439 B)

  - `int __thiscall CUserLocal::IsTeleportSkillAvailable(`
  - `if ( !CUserLocal::IsImmovable(this)`
  - `&& !CUserLocal::IsAttract(this)`
  - `&& (CAvatar::GetOneTimeAction(this: (this + 136)) <= -1 || *(a2 + 3) == 3)`
  - `&& *(this + 2758) == 0 )`
  - `v5 = (*(*(this + 1) + 16))(a1: this + 4);`
  - `if ( *a2 == 5111006 || *a2 == &byte_E6935B )`
  - `while ( 1 )`
  - `v14 = (*(this + 1392) & 1) == 0;`
  - `if ( v14 )`
  - `FootholdUnderneath = CWvsPhysicalSpace2D::GetFootholdUnderneath(`
  - `FootholdAbove = CWvsPhysicalSpace2D::GetFootholdAbove(this: v21, x: v7, y: v8, pcy: &v19, yMax: v8 - 30);`
  - `if ( FootholdUnderneath == 0 && FootholdAbove == 0 )`
  - `if ( (*(this + 1392) & 1) != 0 )`
  - …（截断 14 行）

### CUserLocal::OnKeyDownSkillEnd  @ 0x95bedf  (4288 B)

  - `void *__thiscall CUserLocal::OnKeyDownSkillEnd(CUserLocal *this)`
  - `v2 = (this + 12628);`
  - `if ( *(this + 3157) != 0 )`
  - `stop_skill_sound(a1: *(this + 3157));`
  - `*(this + 3149) = 0;`
  - `sub_428712(a1: 0);`
  - `v14 = get_update_time() - *(this + 3147);`
  - `if ( v14 <= 30 )`
  - `v3 = (this + 10984);`
  - `v4 = sub_500971(a1: *(this + 2746));`
  - `if ( v14 >= v4 )`
  - `if ( CAvatar::GetOneTimeAction(this: (this + 136)) > -1 )`
  - `(*(*this + 68))(a1: this);`
  - `(*(*(this + 34) + 20))(a1: this + 136, a2: 6, a3: 100, a4: 0);`
  - …（截断 14 行）

### CUserLocal::OnOpenSkillGuide  @ 0x961042  (333 B)

  - `int __thiscall CUserLocal::OnOpenSkillGuide(CUserLocal *this, struct CInPacket *a2)`
  - `result = CWvsContext::UI_Open(this: *dword_BE7918, a2: 3, a3: 1);`
  - `if ( dword_BF1080 != 0 )`
  - `return CUISkill::OpenCurSkillGuide(this: dword_BF1080);`

### CUserLocal::OnSkillCooltimeSet  @ 0x95be66  (877 B)

- s2c 解码：`Decode4`, `Decode2`
  - `void __thiscall CUserLocal::OnSkillCooltimeSet(CUserLocal *this, struct CInPacket *a2)`
  - `v2 = CInPacket::Decode4(this: a2);`
  - `v3 = CInPacket::Decode2(this: a2);`
  - `CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v7);`
  - `if ( v7[1] != 0 )`
  - `if ( v5 != 0 )`
  - `v6 = 1000 * v5 + timeGetTime();`
  - `if ( v2 == 5221999 )`
  - `CWvsContext::SetSkillCooltimeOver(this: v4, a2: v2, a3: v6);`
  - `CWvsContext::RemoveSkillCooltimeOver(this: v4, a2: v2);`

### CUserLocal::SendSkillCancelRequest  @ 0x96d873  (1022 B)

- **c2s opcode `COutPacket(92)`**
- c2s 编码：`Encode4`
  - `int __thiscall CUserLocal::SendSkillCancelRequest(void **this, __int16 *a2)`
  - `if ( *(this + 2746) == a2 )`
  - `memset(a1: this + 2746, a2: 0, a3: 0xCu);`
  - `COutPacket::COutPacket(a1: 92);`
  - `COutPacket::Encode4(this: &v4, a2);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v4);`
  - `if ( a2 == 1004 || a2 == (&loc_4FAA8D + 1) || a2 == (&loc_989A68 + 4) || a2 == 20001004 || a2 == 20011004 )`
  - `if ( *(this + 3179) == 0 )`
  - `*(this + 3180) = get_update_time();`
  - `*(this + 3179) = *(this + 3179) + 1;`
  - `if ( is_keydown_skill(a1: a2) && a2 != 3121004 && a2 != (&loc_4FAA8B + 1) && a2 != &word_C80EDA )`
  - `sub_443D21(a1: *(this + 1130));`

### CUserLocal::SendSkillUseRequest  @ 0x96d399  (6184 B)

- **c2s opcode `COutPacket(91)`**
- c2s 编码：`Encode4`, `Encode1`, `Encode2`
- 节流 `CanSendExclRequest(300)`
  - `signed int __thiscall CUserLocal::SendSkillUseRequest(`
  - `if ( CWvsContext::CanSendExclRequest(a1: 300, a2: 0) == 0 )`
  - `v10 = *(CWvsContext::GetCharacterData(this: v8, a2: &v43) + 4);`
  - `if ( CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v10, a3: *pSkill, a4: 0) <= 0 )`
  - `if ( (v12 == 0 || v12 == 9) && sub_764721(a1: *pSkill) == 0 )`
  - `VecCtrl = CUser::GetVecCtrl(this, a2: &v42);`
  - `v15 = *VecCtrl != 0 ? (*VecCtrl - 12) : 0;`
  - `if ( v42 != 0 )`
  - `(*(*v42 + 8))(a1: v42);`
  - `if ( SKILLENTRY::IsActionAppointed(this: pSkill, a2: v14, a3: nSLV) )`
  - `if ( CAvatar::GetOneTimeAction(this: (this + 136)) <= -1 )`
  - `if ( CVecCtrl::IsOnLadder(this: v15) != 0 )`
  - `if ( CVecCtrl::IsOnRope(this: v15) == 0 )`
  - `v18 = rand();`
  - …（截断 14 行）

### CUserLocal::TryDoingMagicAttack  @ 0x95571f  (1886 B)


### CUserLocal::TryDoingMeleeAttack  @ 0x950921  (1787 B)

  - `{"addr":"0x950921","code":"int __thiscall CUserLocal::TryDoingMeleeAttack(\n        _DWORD *this,\n        const struct SKILLENTRY *a2,\n        const struct SKILLENTRY **a3,\n        int a4,\n       `

### CUserLocal::TryDoingPreparedSkill  @ 0x96cf1d  (2482 B)

  - `void __thiscall CUserLocal::TryDoingPreparedSkill(CUserLocal *this)`
  - `v2 = (this + 10984);`
  - `if ( *(this + 2746) != 0 && get_update_time() - *(this + 2747) >= 0 )`
  - `v13 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v11) + 4);`
  - `if ( v12 != 0 )`
  - `SkillLevel = CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v13, a3: *v2, a4: &v14);`
  - `if ( v14 != 0 && SkillLevel != 0 )`
  - `if ( *(this + 3149) != 0 )`
  - `if ( get_update_time() - *(this + 3147) == *(v14 + 8) )`
  - `v4 = *((*(*this + 8))(a1: this) + 3268);`
  - `v5 = (*(*this + 8))(a1: this);`
  - `v6 = _ZtlSecureFuse<long>(a1: v5 + 516, a2: *(v5 + 524));`
  - `v10 = *(this + 321);`
  - `v12 = _ZtlSecureFuse<long>(a1: this + 1276, a2: v10);`
  - …（截断 14 行）

### CUserLocal::TryDoingShootAttack  @ 0x9537d5  (1776 B)


### CUserLocal::TryDoingWings  @ 0x957a1f  (1922 B)

  - `int __thiscall CUserLocal::TryDoingWings(CUserLocal *this, const struct SKILLENTRY *a2, int a3)`
  - `if ( a2 == 0`
  - `|| (TSecType<unsigned char>::GetData(this: (*dword_BE7918 + 8240)) & 1) == 0 && (*(get_field() + 324) & 2) != 0 )`
  - `if ( CUserLocal::IsImmovable(this) )`
  - `if ( CUserLocal::IsAttract(this) )`
  - `if ( *(this + 2746) != 0 )`
  - `if ( CUserLocal::IsWeakened(a1: this) != 0 )`
  - `if ( CAvatar::GetOneTimeAction(this: (this + 136)) > -1 )`
  - `LevelData = SKILLENTRY::GetLevelData(this: a2, a2: v4, a3);`
  - `v6 = _ZtlSecureFuse<long>(a1: LevelData + 232, a2: *(LevelData + 240));`
  - `v7 = 1000 * v6 + get_update_time();`
  - `v8 = *(this + 1129);`
  - `*(this + 3148) = v7;`
  - `v9 = v8 != 0 ? (v8 - 12) : 0;`
  - …（截断 14 行）

### CUserLocal::TryRegisterTeleport  @ 0x957b74  (12596 B)

- **c2s opcode `COutPacket(101)`**
- c2s 编码：`Encode1`, `EncodeStr`, `Encode2`
- SP_ 节点：`SP_2180_EFFECT_BASICEFFIMG_TELEPORT`
  - `int __fastcall CUserLocal::TryRegisterTeleport(`
  - `if ( pSkill != 0 && nSLV > 0 )`
  - `if ( sub_958143(this: *dword_BE7918) == 0 )`
  - `v69 = (*(get_field() + 324) & 2) != 0;`
  - `if ( v69 )`
  - `if ( CUserLocal::IsImmovable(this: a1) || CUserLocal::IsAttract(this: a1) )`
  - `tCur_1 = get_update_time();`
  - `v9 = *(a1 + 2758) == 0;`
  - `if ( !v9 || *(a1 + 2762) != 0 && *(a1 + 2763) - tCur_1 > 0 )`
  - `v10 = *(a1 + 1129) != 0 ? (*(a1 + 1129) - 12) : 0;`
  - `if ( bForced == 0 && v10[68] == 0 )`
  - `if ( sTargetPortalName != 0 )`
  - `v11 = sub_712B24(a1: sTargetPortalName);`
  - `if ( v11 != 0 )`
  - …（截断 14 行）

### CUserLocal::ValidateItemSkill  @ 0x94eaa9  (5642 B)

  - `int __usercall CUserLocal::ValidateItemSkill@<eax>(CUserLocal *this@<ecx>, CItemInfo *a2@<ebx>)`
  - `sub_5E121C();`
  - `if ( *dword_BE7918 != 0 )`
  - `v36 = (*dword_BE7918 + 12496);`
  - `if ( v36 == 0 )`
  - `sub_4288AF(a1: v30);`
  - `if ( v31 != 0 )`
  - `Data = TSecType<long>::GetData(a1: (v31 + 12), a2);`
  - `if ( sub_5D2902(this: v3, a2: Data) )`
  - `v7 = TSecType<long>::GetData(a1: (v31 + 12), a2);`
  - `v26 = (*(*v5 + 36))(a1: v5);`
  - `v8 = (*(*v6 + 40))(a1: v6);`
  - `sub_5D29E5(this: a2, a2: v28, a3: v34, a4: v8, a5: v26);`
  - `LOBYTE(v39) = 1;`
  - …（截断 14 行）

### CUserLocal::ValidateSkillBonus  @ 0x94ed77  (10746 B)

  - `int __usercall CUserLocal::ValidateSkillBonus@<eax>(CItemInfo *a1@<ebx>)`
  - `sub_474813(a1: 100);`
  - `if ( *dword_BE7918 != 0 )`
  - `v57 = (v58 + 12);`
  - `if ( v58 == 0 )`
  - `sub_4288AF(a1: &v50);`
  - `LOBYTE(v62) = 1;`
  - `if ( v51 != 0 )`
  - `Data = TSecType<long>::GetData(a1: (v51 + 12), a2: a1);`
  - `if ( sub_5D2902(this: v2, a2: Data) )`
  - `v6 = TSecType<long>::GetData(a1: (v51 + 12), a2: a1);`
  - `v43 = (*(*v4 + 36))(a1: v4);`
  - `v7 = (*(*v5 + 40))(a1: v5);`
  - `sub_5D29E5(this: a1, a2: &v48, a3: v52, a4: v7, a5: v43);`
  - …（截断 14 行）

### CUserRemote::OnBodyAttack  @ 0x9830bb  (5464 B)

  - `void __thiscall CUserRemote::OnBodyAttack(int this, int a2, int *a3, int a4, int a5, int a6, int a7, int *a8)`
  - `update_time = get_update_time();`
  - `if ( a3 != 0 )`
  - `sub_405210(a1: this + 1288);`
  - `sub_936F34(a1: &v50, a2: a3, a3: a4, a4: v38.bottom, a5: a7);`
  - `if ( v50 != 0 && *v50 != 0 )`
  - `v11 = *(*v50 - 4) >> 1;`
  - `if ( v11 != 0 )`
  - `v38.bottom = sub_95355D(a1: this + 136, a2: 0);`
  - `sub_405210(a1: &v50);`
  - `CUser::RegisterAfterimage(a1: this, a2: update_time, a3: a6, a4: a5, a5: v38.right, a6: v38.bottom, a7: v39);`
  - `CAvatar::GetBodyRect(this: (this + 136), a2: &v42, a3: 1);`
  - `while ( 1 )`
  - `if ( *a8 == 0 || v55 >= *(v15 - 4) )`
  - …（截断 14 行）

### CUserRemote::OnMagicAttack  @ 0x98250d  (21471 B)

  - `void __thiscall CUserRemote::OnMagicAttack(`
  - `if ( a3 == 0 )`
  - `v10 = 1500 * (CAvatar::GetOneTimeAction(this: (this + 136)) > -1) + this + 136 + 1264;`
  - `v11 = *(v10 + 12);`
  - `if ( v11 == 0 || *(v11 - 4) == 0 )`
  - `v12 = *(v10 + 8) - *(*(v10 + 12) + 4 * *(v11 - 4) - 4);`
  - `update_time = get_update_time();`
  - `HIDWORD(v122) = a4;`
  - `sub_9556AB(a1: &v150, a2: a4);`
  - `sub_9370FB(a1: &v147);`
  - `LOBYTE(v154) = 1;`
  - `if ( v147 != 0 && *v147 != 0 )`
  - `v14 = *(*v147 - 4) >> 1;`
  - `if ( v14 != 0 )`
  - …（截断 14 行）

### CUserRemote::OnMeleeAttack  @ 0x980c90  (11268 B)

  - `void __fastcall CUserRemote::OnMeleeAttack(`
  - `update_time = get_update_time();`
  - `if ( a4 != 0 )`
  - `v10 = (*(*a1 + 8))(a1);`
  - `v66.bottom = _ZtlSecureFuse<long>(a1: v10 + 924, a2: *(v10 + 932)) != 0;`
  - `sub_405210(a1: a1 + 1288);`
  - `sub_936F34(a1: &v80, a2: a4, a3: a5, a4: v66.bottom, a5: a8);`
  - `if ( v80 != 0 && *v80 != 0 )`
  - `v13 = *(*v80 - 4) >> 1;`
  - `if ( v13 == 0 )`
  - `if ( a4 != 0`
  - `&& (v15 = SKILLENTRY::GetLevelData(this: a4, a2: v11, a3: a5),`
  - `v16 = sub_94BEBB(this: v71, a2: v15),`
  - `LOBYTE(v86) = 1,`
  - …（截断 14 行）

### CUserRemote::OnShootAttack  @ 0x981211  (34511 B)

  - `void __thiscall CUserRemote::OnShootAttack(`
  - `if ( (byte_BF1390 & 1) == 0 )`
  - `dword_BF138C = CSkillInfo::GetSkill(this: dword_BE78DC, a2: 3101004);`
  - `dword_BF1388 = CSkillInfo::GetSkill(this: dword_BE78DC, a2: &loc_C7E7C9 + 2);`
  - `v147 = (*(*v11 + 64))(a1: v11) / 1000 == 1;`
  - `if ( !v147 )`
  - `if ( a3 != 0 )`
  - `update_time = get_update_time();`
  - `if ( sub_76681D(a1: v222) == 0 && !is_shooting_weapon(nItemID: v11[315]) )`
  - `v14 = &v11[375 * (CAvatar::GetOneTimeAction(this: (v11 + 34)) > -1) + 350];`
  - `v15 = *(v14 + 12);`
  - `if ( v15 == 0 || *(v15 - 4) == 0 )`
  - `v16 = sub_9427C4(a1: v13, a2: *(v14 + 8) - *(*(v14 + 12) + 4 * *(v15 - 4) - 4));`
  - `v17 = (*(*v11 + 8))(a1: v11);`
  - …（截断 14 行）

### CWndSkillGuide::OnKey  @ 0x8aa4e4  (280 B)

  - `int __thiscall CWndSkillGuide::OnKey(CWndSkillGuide *this, unsigned int a2, signed int a3)`
  - `if ( a3 >= 0 && a2 == 27 && dword_BF1080 != 0 )`
  - `return CUISkill::CloseSkillGuide(this: dword_BF1080);`

### CWvsContext::GetSkillLevelUpState  @ 0xa08e05  (1302 B)

  - `signed int __thiscall CWvsContext::GetSkillLevelUpState(CWvsContext *this, const struct SKILLENTRY *a2)`
  - `v2 = *(CWvsContext::GetCharacterData(this, a2: &v9) + 4);`
  - `if ( v10 != 0 )`
  - `v3 = *(a2 + 20);`
  - `while ( v3 != 0 )`
  - `v5 = *(v3 != 0 ? v3 - 3 : 4);`
  - `v3 = v5 != 0 ? (v5 + 16) : 0;`
  - `if ( CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v2, a3: v8, a4: 0) < v4[1] )`
  - `if ( is_skill_need_master_level(a1: *a2) )`
  - `ZMap<long,long,long>::GetAt(a1: a2, a2: &v10);`
  - `ZMap<long,long,long>::GetAt(a1: a2, a2: &v11);`
  - `if ( v11 <= v10 )`
  - `SkillLevel = CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v2, a3: *a2, a4: &a2);`
  - `return SkillLevel >= sub_4FB30C(this: a2) ? -1 : 1;`
  - …（截断 14 行）

### CWvsContext::OnChangeSkillRecordResult  @ 0xa1e48c  (1911 B)

- s2c 解码：`Decode1`, `Decode2`, `Decode4`, `DecodeBuffer`
  - `_DWORD *__thiscall CWvsContext::OnChangeSkillRecordResult(FILETIME *this, struct CInPacket *a2)`
  - `if ( CInPacket::Decode1(this: a2) != 0 )`
  - `v3[1045].dwLowDateTime = get_update_time();`
  - `CWvsContext::GetCharacterData(this: v3, a2: &v10);`
  - `if ( v11 != 0 )`
  - `v4 = CInPacket::Decode2(this: v2);`
  - `if ( v4 != 0 )`
  - `for ( i = v4; i != 0; --i )`
  - `a2 = CInPacket::Decode4(this: v2);`
  - `v12 = CInPacket::Decode4(this: v2);`
  - `if ( v12 >= 0 )`
  - `ZMap<long,long,long>::Insert(a1: &a2, a2: &v12);`
  - `ZMap<long,long,long>::RemoveKey(a1: &a2);`
  - `v11 = CInPacket::Decode4(this: v2);`
  - …（截断 14 行）

### CWvsContext::OnSkillExpireMessage  @ 0xa219be  (1749 B)

- s2c 解码：`Decode1`, `Decode4`
- SP_ 节点：`SP_5240_S_HAS_DISAPPEARED_AS_THE_TIME_LIMIT_HAS_PASSED`
  - `void __thiscall CWvsContext::OnSkillExpireMessage(CWvsContext *this, struct CInPacket *a2)`
  - `v2 = CInPacket::Decode1(this: a2);`
  - `if ( v2 != 0 )`
  - `for ( i = v2; i != 0; --i )`
  - `v5 = CInPacket::Decode4(this: a2);`
  - `Skill = CSkillInfo::GetSkill(this: v4, a2: v5);`
  - `if ( Skill != 0 )`
  - `v8 = *(Skill + 4);`
  - `if ( v8 != 0 && *v8 != 0 )`
  - `v12 = *(v7 + 4);`
  - `LOBYTE(v15) = 1;`
  - `ZXString<char>::Format(a1: &v14, a2: v11, a3: v12);`
  - `CHATLOG_ADD(sString: &v14, nType: 0xCu);`

### CWvsContext::OnSkillUseResult  @ 0xa209b2  (228 B)

- s2c 解码：`Decode1`
  - `char __thiscall CWvsContext::OnSkillUseResult(CWvsContext *this, struct CInPacket *a2)`
  - `*(this + 2089) = 0;`
  - `*(this + 2090) = get_update_time();`
  - `return CInPacket::Decode1(this: a2);`

### CWvsContext::RemoveSkillCooltimeOver  @ 0xa1246c  (277 B)

  - `int __thiscall CWvsContext::RemoveSkillCooltimeOver(CWvsContext *this, int a2)`
  - `if ( a2 == 5221999 )`
  - `CTemporaryStatView::ResetTemporary(this: (this + 11944), a2: 2, a3: 5221006);`
  - `return ZMap<long,long,long>::RemoveKey(a1: &a2);`

### CWvsContext::SendSkillUpRequest  @ 0xa23cf3  (925 B)

- **c2s opcode `COutPacket(90)`**
- c2s 编码：`Encode4`
- 节流 `CanSendExclRequest(500)`
  - `int __thiscall CWvsContext::SendSkillUpRequest(CWvsContext *this, int a2)`
  - `result = CWvsContext::CanSendExclRequest(a1: 500, a2: 0);`
  - `if ( result != 0 )`
  - `COutPacket::COutPacket(a1: 90);`
  - `update_time = get_update_time();`
  - `COutPacket::Encode4(this: &v6, a2: update_time);`
  - `COutPacket::Encode4(this: &v6, a2);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v6);`
  - `*(this + 2089) = 1;`
  - `v5 = get_update_time();`
  - `*(this + 2090) = v5;`

### CWvsContext::SetSkillCooltimeOver  @ 0xa123a4  (1313 B)

  - `int __thiscall CWvsContext::SetSkillCooltimeOver(CWvsContext *this, int a2, int a3)`
  - `if ( a2 == 5221999 )`
  - `v4 = (*(*TSingleton<CUserLocal>::ms_pInstance + 8))(a1: TSingleton<CUserLocal>::ms_pInstance);`
  - `if ( SecondaryStat::IsRidingSkillVehicle(this: v4) )`
  - `CWvsContext::GetCharacterData(this, a2: &v9);`
  - `if ( v10 != 0 )`
  - `SkillLevel = CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v10, a3: 5221006, a4: 0);`
  - `v6 = _ZtlSecureFuse<unsigned char>(a1: v10 + 51, a2: *(v10 + 53));`
  - `v7 = sub_7665F1(a1: 5221006, a2: SkillLevel, a3: v6);`
  - `CTemporaryStatView::UpdatePassively(this: (this + 11944), a2: &loc_4FAA8D + 1, a3, a4: v7);`
  - `return ZMap<long,long,long>::Insert(a1: &a2, a2: &a3);`

### CalcDamage::MDamage  @ 0x791617  (15764 B)

  - `void __thiscall CalcDamage::MDamage(`
  - `for ( i = 7; i != 0; --i )`
  - `*v17++ = CRand32::Random(a1: this);`
  - `v19 = _ZtlSecureFuse<long>(a1: a4 + 24, a2: a4[26]);`
  - `v20 = _ZtlSecureFuse<long>(a1: a4 + 27, a2: a4[29]);`
  - `if ( v21 <= 0 )`
  - `if ( v21 >= 1999 )`
  - `v22 = _ZtlSecureFuse<long>(a1: a4 + 474, a2: a4[476]);`
  - `if ( v22 > 0 )`
  - `if ( v121 >= 1999 )`
  - `get_weapon_type(nItemID: nWeaponItemID);`
  - `v24 = _ZtlSecureFuse<long>(a1: a3 + 15, a2: a3[17]);`
  - `v126 = 5 * (v24 / 10 + _ZtlSecureFuse<long>(a1: a3 + 18, a2: a3[20]) / 10);`
  - `v25 = _ZtlSecureFuse<long>(a1: a3 + 3, a2: a3[5]);`
  - …（截断 14 行）

### CalcDamage::PDamage  @ 0x78df87  (1799 B)

  - `{"addr":"0x78df87","code":"int __thiscall CalcDamage::PDamage(\n        CalcDamage *this,\n        CharacterData *a2,\n        _DWORD *a3,\n        _DWORD *a4,\n        int a5,\n        _DWORD *a6,\n `

### MOBSKILLLEVELDATA::GetEffectUOL  @ 0x66c215  (272 B)

  - `_DWORD *__thiscall MOBSKILLLEVELDATA::GetEffectUOL(_DWORD *this, _DWORD *a2)`
  - `v2 = *(this + 17);`
  - `if ( v2 != 0 )`
  - `InterlockedIncrement(lpAddend: (v2 + 8));`

### SKILLENTRY::GetEffectUOL  @ 0x932d40  (450 B)

  - `_DWORD *__thiscall SKILLENTRY::GetEffectUOL(int *this, _DWORD *a2, unsigned __int8 a3)`
  - `v4 = sub_761332(a1: a3);`
  - `if ( v4 != 0 )`
  - `v5 = (v4 + 4);`
  - `if ( v6 != 0 )`
  - `InterlockedIncrement(lpAddend: (v6 + 8));`

### SKILLENTRY::GetLevelData  @ 0x760f23  (5700 B)

- SP_ 节点：`SP_2241_STRING_SKILLIMG`
  - `int __fastcall SKILLENTRY::GetLevelData(SKILLENTRY *this, int a2, int a3)`
  - `v3 = (this + 92);`
  - `v4 = *(this + 23) == 0;`
  - `if ( !v4 )`
  - `v45 = (this + 88);`
  - `v5 = *(this + 22);`
  - `if ( v5 != 0 )`
  - `v6 = *(v5 - 4);`
  - `if ( *v3 == 0 )`
  - `if ( v6 != IWzProperty::Getcount(a1: *v3) )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v40, pvargSrc: &pvargSrc);`
  - `LOBYTE(v47) = 1;`
  - `v9 = sub_406472();`
  - …（截断 14 行）

### SKILLENTRY::GetRandomAppointedAction  @ 0x7660b6  (664 B)

  - `int __fastcall SKILLENTRY::GetRandomAppointedAction(SKILLENTRY *this, int a2, int a3, unsigned int a4)`
  - `LevelData = SKILLENTRY::GetLevelData(this, a2, a3);`
  - `v6 = (LevelData + 360);`
  - `if ( _ZtlSecureFuse<long>(a1: LevelData + 352, a2: *(LevelData + 360)) >= 0 )`
  - `return _ZtlSecureFuse<long>(a1: v7, a2: *v6);`
  - `v9 = *(this + 6);`
  - `if ( v9 != 0 && *(v9 - 4) != 0 )`
  - `return *(*(this + 6) + 4 * (a4 % *(v9 - 4)));`

### SKILLENTRY::GetScreenEffectUOL  @ 0x936304  (271 B)

  - `_DWORD *__thiscall SKILLENTRY::GetScreenEffectUOL(_DWORD *this, _DWORD *a2)`
  - `v2 = *(this + 28);`
  - `if ( v2 != 0 )`
  - `InterlockedIncrement(lpAddend: (v2 + 8));`

### SKILLENTRY::GetSpecialUOL  @ 0x411256  (266 B)

  - `_DWORD *__thiscall SKILLENTRY::GetSpecialUOL(_DWORD *this, _DWORD *a2)`
  - `v2 = *(this + 39);`
  - `if ( v2 != 0 )`
  - `InterlockedIncrement(lpAddend: (v2 + 8));`

### SKILLENTRY::IsActionAppointed  @ 0x76611c  (485 B)

  - `BOOL __fastcall SKILLENTRY::IsActionAppointed(SKILLENTRY *this, int a2, int a3)`
  - `LevelData = SKILLENTRY::GetLevelData(this, a2, a3);`
  - `if ( _ZtlSecureFuse<long>(a1: LevelData + 352, a2: *(LevelData + 360)) < 0 )`
  - `v5 = *(this + 6);`
  - `if ( v5 == 0 || *(v5 - 4) == 0 )`

### SKILLENTRY::IsCorrectAppointedAction  @ 0x766418  (1096 B)

  - `BOOL __fastcall SKILLENTRY::IsCorrectAppointedAction(SKILLENTRY *this, int a2, int a3, int a4)`
  - `LevelData = SKILLENTRY::GetLevelData(this, a2, a3);`
  - `v6 = (LevelData + 360);`
  - `if ( _ZtlSecureFuse<long>(a1: LevelData + 352, a2: *(LevelData + 360)) >= 0 )`
  - `return a4 == _ZtlSecureFuse<long>(a1: v7, a2: *v6);`
  - `for ( i = 0; ; ++i )`
  - `v10 = *(this + 6);`
  - `if ( v10 == 0 || i >= *(v10 - 4) )`
  - `if ( *(v10 + 4 * i) == a4 )`
  - `if ( a4 == 60 )`
  - `for ( j = 0; ; ++j )`
  - `v12 = *(this + 6);`
  - `if ( v12 == 0 || j >= *(v12 - 4) )`
  - `if ( *(v12 + 4 * j) == 59 )`
  - …（截断 14 行）

### SKILLENTRY::IsCorrectWeaponType  @ 0x766157  (2510 B)

  - `BOOL __userpurge SKILLENTRY::IsCorrectWeaponType@<eax>(char **this@<ecx>, int a2, int a3)`
  - `if ( *this > (&stru_B8A588.action + 2) )`
  - `if ( v3 > &byte_E6935C )`
  - `if ( v6 != 0 )`
  - `if ( v7 != 0 && v7 != 3 )`
  - `if ( v3 >= &byte_E6935B )`
  - `if ( v3 < (&stru_B8CC98.action + 1) )`
  - `if ( v3 > (&stru_B8CC98.action + 2) )`
  - `if ( v3 != &byte_E4E5A9 )`
  - `if ( v3 == &byte_E66C4B )`
  - `if ( a2 == 39 || a2 == 48 )`
  - `if ( a2 == 37 )`
  - `if ( *this == &stru_B8A588.action + 2 )`
  - `if ( v3 <= &loc_4E23EA )`
  - …（截断 14 行）

### SecondaryStat::IsRidingSkillVehicle  @ 0x8c46ab  (225 B)

  - `BOOL __thiscall SecondaryStat::IsRidingSkillVehicle(SecondaryStat *this)`
  - `v1 = *(this + 815);`
  - `return (*(*v1 + 4))(a1: v1) != 0 && *ZFatalSection::Lock(a1: v1) / 10000 == 193;`

### SkillInfo::Parse  @ 0x5880bf  (5385 B)

  - `int __thiscall Field::SkillInfo::Parse(char *this, int a2)`
  - `if ( a2 != 0 )`
  - `_bstr_t::_bstr_t(a1: aClass);`
  - `LOBYTE(v40) = 0;`
  - `sub_40263B(a1: &a2);`
  - `IWzProperty::GetItem(pvargDest: &pvarg, a2: v27);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v28, a2: v29);`
  - `if ( Unknown != 0 )`
  - `v3 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v3 < 0 && v3 != -2147467262 )`
  - `if ( v4 < 0 )`
  - `if ( v37 != 0 )`
  - `v39 = IWzProperty::Getcount(a1: v37);`
  - `for ( i = 0; i < v39; ++i )`
  - …（截断 14 行）

### SkillInfo::SetFieldType  @ 0x587f55  (141 B)

  - `int __thiscall Field::SkillInfo::SetFieldType(Field::SkillInfo *this, int a2)`
  - `*(this + 10) = a2;`

## 2. 反编译中出现的 c2s opcode 常量汇总（COutPacket 构造参数）

| opcode (dec) | hex | 来源函数 |
| --- | --- | --- |
| 90 | 0x5A | CWvsContext::SendSkillUpRequest |
| 91 | 0x5B | CUserLocal::DoActiveSkill_Heal, CUserLocal::DoActiveSkill_RecoveryAura, CUserLocal::DoActiveSkill_SmokeShell, CUserLocal::DoActiveSkill_TownPortal… |
| 92 | 0x5C | CUserLocal::SendSkillCancelRequest |
| 93 | 0x5D | CUserLocal::DoActiveSkill_Prepare |
| 101 | 0x65 | CUserLocal::TryRegisterTeleport |
| 104 | 0x68 | CUserLocal::DoAntiMacroSkill |
| 165 | 0xA5 | CUISkillEx::OnButtonClicked |
