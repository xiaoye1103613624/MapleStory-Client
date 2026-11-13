# Angel 083 宠物召唤龙面反编译语义抽取（观察层）

来源：`evidence/angel-petsummon-decompile-full.json`（逐字 Hex-Rays 伪代码，超大函数从 `/output/` 端点补全并归一化）。
本文件由 `tools/ida_angel_petsummon_facts.py` 自动生成，只做字符串抽取，不含推断。

## 1. StringPool 资源节点全集（185 个）

- `SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK` ← CActionFrame, CActionMan, CChatBalloon, CDraggableItem, CPet, CStage…
- `SP_1035_STAND` ← CActionMan
- `SP_1076_FACE` ← CActionFrame, CActionMan
- `SP_1079_FRAME` ← CActionMan
- `SP_1140_ISLOT` ← CActionMan
- `SP_1141_VSLOT` ← CActionMan
- `SP_1142_SFX` ← CActionMan
- `SP_1143_AFTERIMAGE` ← CActionMan
- `SP_1144_WALK` ← CActionMan
- `SP_1145_ATTACK` ← CActionMan
- `SP_1146_ATTACKSPEED` ← CActionMan
- `SP_1147_ZIGZAG` ← CActionMan
- `SP_1149_H4H5` ← CActionMan
- `SP_1150_WEEKLY` ← CActionMan
- `SP_1234_DRAGEND` ← CDraggableItem
- `SP_1245_PICKUPITEM` ← CDropPool
- `SP_1315_UI_LOGINIMG_CHARSELECT_PAGEL_` ← CUIAvatar
- `SP_1316_UI_LOGINIMG_CHARSELECT_PAGER_` ← CUIAvatar
- `SP_1317_UI_LOGINIMG_CHARSELECT_CHARACTER_0` ← CUIAvatar
- `SP_1318_UI_LOGINIMG_CHARSELECT_CHARACTER_1` ← CUIAvatar
- `SP_1323_UI_LOGINIMG_CHARSELECT_NAMETAG` ← CUIAvatar, CUIAvatarVAC
- `SP_1324_UI_LOGINIMG_CHARSELECT_KNIGHT` ← CUIAvatar
- `SP_1388_UI_CHATBALLOONIMG_` ← CChatBalloon
- `SP_1389_NPC` ← CChatBalloon
- `SP_1390_ADBOARD` ← CChatBalloon
- `SP_1394_NW` ← CChatBalloon
- `SP_1395_N` ← CChatBalloon
- `SP_1396_NE` ← CChatBalloon
- `SP_1397_E` ← CChatBalloon
- `SP_1398_W` ← CChatBalloon
- `SP_1399_C` ← CChatBalloon
- `SP_1400_SW` ← CChatBalloon
- `SP_1401_S` ← CChatBalloon
- `SP_1402_SE` ← CChatBalloon
- `SP_1403_ARROW` ← CChatBalloon
- `SP_1410_CANVASFONT` ← CChatBalloon, CUIAvatar, CUIAvatarVAC
- `SP_1411_BA` ← CChatBalloon
- `SP_1414_UI_CHATBALLOONIMG_MINIROOM` ← CChatBalloon
- `SP_1416_OMOK` ← CChatBalloon
- `SP_1421_CNUM` ← CChatBalloon
- `SP_1422_MNUM` ← CChatBalloon
- `SP_1425_PROGRESS` ← CChatBalloon
- `SP_148_NOT_AVAILABLE_WITH_AN_ADMIN_ACCOUNT` ← CDropPool
- `SP_150_NOT_AVAILABLE` ← CDraggableItem
- `SP_1528_VRLEFT` ← CWvsPhysicalSpace2D
- `SP_1529_VRRIGHT` ← CWvsPhysicalSpace2D
- `SP_1530_VRTOP` ← CWvsPhysicalSpace2D
- `SP_1531_VRBOTTOM` ← CWvsPhysicalSpace2D
- `SP_1532_VRLIMIT` ← CWvsPhysicalSpace2D
- `SP_1587_MSG` ← CStage
- `SP_1588_ICONREWARD` ← CDropPool
- `SP_1589_ICONRAW` ← CDropPool
- `SP_1590_EFFECT_SUMMONIMG_D` ← CMob
- `SP_1591_SOUND_SKILLIMG_07D_HIT` ← CMob
- `SP_1593_MOB` ← CChatBalloon
- `SP_1617_PADAMAGE` ← CSummonedBase
- `SP_1619_MADAMAGE` ← CSummonedBase
- `SP_1636_RANGE` ← CSummonedBase
- `SP_1637_MOBCOUNT` ← CSummonedBase
- `SP_1642_SP` ← CSummonedBase
- `SP_1643_R` ← CSummonedBase
- `SP_1644_EFFECT` ← CAnimationDisplayer
- `SP_1654_ATTACKAFTER` ← CSummonedBase
- `SP_1655_EFFECTAFTER` ← CSummonedBase
- `SP_1717_08D` ← CDropPool
- `SP_1748_PAGE` ← CWvsPhysicalSpace2D
- `SP_1796_S__S` ← CMob
- `SP_1911_PREV` ← CWvsPhysicalSpace2D
- `SP_1912_NEXT` ← CWvsPhysicalSpace2D
- `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID` ← CDraggableItem, CDropPool, CStage
- `SP_2241_STRING_SKILLIMG` ← SKILLENTRY
- `SP_2320_X1` ← CWvsPhysicalSpace2D
- `SP_2321_X2` ← CWvsPhysicalSpace2D
- `SP_2322_Y1` ← CWvsPhysicalSpace2D
- `SP_2323_Y2` ← CWvsPhysicalSpace2D
- `SP_2324_L` ← CWvsPhysicalSpace2D
- `SP_2325_UF` ← CWvsPhysicalSpace2D
- `SP_2435_EFFECT_ITEMEFFIMG_D` ← CAnimationDisplayer
- `SP_2499_REPEAT` ← CPet
- `SP_266_YOU_CANT_DO_IT_RIGHT_NOW_R_NCATCH_YOUR_BREATH_AND_THEN_TRY_AGAIN` ← CDraggableItem
- `SP_2698_POS` ← CPet
- `SP_2714_SKILL_231IMG_SKILL_2311002_MDOOR` ← CTownPortalPool
- `SP_2715_SKILL_231IMG_SKILL_2311002_FRAME` ← CTownPortalPool
- `SP_2716_SKILL_231IMG_SKILL_2311002_CDOOR` ← CTownPortalPool
- `SP_272_CURSING_IS_NOT_ALLOWED` ← CChatHelper
- `SP_2819_BULLETSPEED` ← CSummonedBase
- `SP_2866_RECT` ← CActionMan
- `SP_2870_EFFECT_BASICEFFIMG_SUMMONED` ← CDragon, CSummoned
- `SP_2899_EFFECTDISTANCE` ← CAnimationDisplayer
- `SP_2971_DRAG` ← CWvsPhysicalSpace2D
- `SP_2972_FORCE` ← CWvsPhysicalSpace2D
- `SP_3190_UI_UIWINDOWIMG_QUESTICON_D` ← CDragon
- `SP_3306_SOUND_CASHEFFECTIMG_08D_HIT` ← CMob
- `SP_3393_YOUR_CHATTING_WILL_BE_ENABLED_AFTER_04D_D_D` ← CStage
- `SP_3400_ETC_CHATBLOCKREASONIMG` ← CStage
- `SP_3416_SEALED_ITEMS_CANNOT_BE_R_NSOLD_TRADED_OR_DROPPED` ← CDraggableItem
- `SP_3452_ITEM_CASH_0503IMG` ← CChatBalloon
- `SP_3458_SIGNBOARD` ← CChatBalloon
- `SP_3804_CHARACTERSTART` ← CActionFrame
- `SP_3805_CHARACTEREND` ← CActionFrame
- `SP_380_THE_CLOSENESS_INCREASED_D` ← CPet
- `SP_3818_YOU_CAN_RIDE_THE_TAMED_CASH_MONSTER_WHEN_IT_IS_EQUIPPED_WITH_A_CASH_SADDLE` ← CDraggableItem
- `SP_381_THE_CLOSENESS_DECREASED_D` ← CPet
- `SP_386_PLEASE_SELECT_THE_PET_YOU_WOULD_LIKE_TO_PUT_THE_PET_EQUIPMENT_ON` ← CDraggableItem
- `SP_3974_UI_UIWINDOWIMG_AVATARMEGAPHONE` ← CUIAvatarMegaphone
- `SP_3983_UI_LOGINIMG_VIEWALLCHAR_BTENTER` ← CUIAvatarVAC
- `SP_3986_UI_LOGINIMG_VIEWALLCHAR_WORLDICONS_D` ← CUIAvatarVAC
- `SP_3987_UI_LOGINIMG_VIEWALLCHAR_BACKGRND` ← CUIAvatarVAC
- `SP_3_HTTP__MAPLESTORYNEXONNET_WZASPXPART_DOWNLOADS_GAMEPATCHES` ← CPet
- `SP_4151_FLIP` ← CActionMan
- `SP_4152_ROTATE` ← CActionMan
- `SP_4259_DROPPING_THIS_ITEM_WILL_BREAK_YOUR_ENGAGEMENT_R_NDO_YOU_STILL_WISH_TO_DROP_THIS_` ← CDraggableItem
- `SP_4260_YOU_WILL_NOT_BE_ABLE_TO_RECEIVE_THE_WEDDING_RING_R_NIS_THAT_OKAY` ← CDraggableItem
- `SP_4266_YOU_MAY_ONLY_EQUIP_ONE_WEDDING_RING` ← CDraggableItem
- `SP_4378_TO_HATCH_PIGMY_EGG_YOU_NEED_TO_R_NPURCHASE_THE_INCUBATOR_FROM_THE_CASH_SHOP` ← CDraggableItem
- `SP_4417_FORBIDFALLDOWN` ← CWvsPhysicalSpace2D
- `SP_4421_USE_THE_TAB_KEY_TO_TOGGLE_THE_MENU` ← CUIAvatar
- `SP_4560_MAP_EFFECTIMG_MIRO_RR1_D` ← CAnimationDisplayer
- `SP_4561_MAP_EFFECTIMG_MIRO_RR2_D` ← CAnimationDisplayer
- `SP_4562_MAP_EFFECTIMG_MIRO_RR3_D` ← CAnimationDisplayer
- `SP_4658_ONCE_DROPPED_THE_ITEM_CANNOT_BE_TRADED_R_NWOULD_YOU_STILL_LIKE_TO_DROP_IT` ← CDraggableItem
- `SP_4817_REPEATSTARTINDEX` ← CPet
- `SP_4818_EFFECT_SETEFFIMG_D_PETEFFECT` ← CPet
- `SP_4819_EFFECT_SETEFFIMG_D_PETBACKGRND_D` ← CPet
- `SP_4843_ITEM_CASH_0528IMG_08D_TILE` ← CAffectedAreaPool
- `SP_4875_WOULD_YOU_LIKE_TO_THROW_R_NAWAY_THIS_CARD` ← CDraggableItem
- `SP_5121_UI_LOGINIMG_CHARSELECT_ARAN` ← CUIAvatar
- `SP_5122_UI_LOGINIMG_CHARSELECT_ADVENTURE` ← CUIAvatar
- `SP_5213_IF_YOU_USE_THIS_ITEM_YOU_WILL_NOT_BE_ABLE_R_NTO_TRADE_THE_ITEM_WITH_OTHER_USERS_` ← CDraggableItem
- `SP_5223_YOU_CAN_ONLY_BE_EQUIPPED_WITH_1_UNIQUE_EQUIPPED_ITEM` ← CDraggableItem
- `SP_5258_THIS_ITEM_CANNOT_BE_RECOVERED_ONCE_DROPPED_HOWEVER_IT_IS_POSSIBLE_TO_TRANSFER_IT` ← CDraggableItem
- `SP_5327_EFFECT_BASICEFFIMG_COOLHIT_COOL` ← CAnimationDisplayer
- `SP_5454_THIS_ITEM_CAN_ONLY_BE_EQUIPPED_ON_EVAN` ← CDraggableItem
- `SP_5508_EFFECT_BASICEFFIMG_DRAGONEYES` ← CDragon
- `SP_5510__MESOS` ← CPet
- `SP_5515_D` ← CAnimationDisplayer, CChatBalloon
- `SP_5527_ARIAL` ← CChatBalloon, CUIAvatar, CUIAvatarMegaphone, CUIAvatarVAC
- `SP_5571_THIS_ITEM_CANT_BE_TAKEN_BACK_ONCE__R_NTHROWN_AWAY_WILL_YOU_STILL_DROP_IT` ← CDraggableItem
- `SP_5612_A0` ← CActionMan
- `SP_5613_A1` ← CActionMan
- `SP_5614_ACTION` ← CActionMan
- `SP_5619_BACKGRND` ← CChatBalloon
- `SP_5623_DEAD` ← CChatBalloon
- `SP_5624_DELAY` ← CActionMan, CAvatar, CChatBalloon
- `SP_5628_HEAD` ← CActionMan, CChatBalloon
- `SP_5636_LT` ← CActionMan, CSummonedBase
- `SP_5640_MOVE` ← CActionMan
- `SP_5643_PET` ← CChatBalloon
- `SP_5647_RB` ← CActionMan, CSummonedBase
- `SP_66_YOU_CANT_DO_IT_ONCE_YOURE_DEAD` ← CDraggableItem
- `SP_867_YOU_CANNOT_TRADE_THIS_ITEM_AFTER_EQUIPPING_R_N_DO_YOU_STILL_WISH_TO_EQUIP` ← CDraggableItem
- `SP_868_YOUR_STATS_ARE_TOO_LOW_TO_EQUIP_THIS_ITEM` ← CDraggableItem
- `SP_869_ONLY_ONE_COUPLE_RING_CAN_BE_EQUIPPED` ← CDraggableItem
- `SP_870_YOU_MAY_ONLY_EQUIP_R_NONE_FRIENDSHIP_RING_AT_A_TIME` ← CDraggableItem
- `SP_871_YOU_NEED_TO_EQUIP_YOURSELF_WITH_AN_ITEM_R_NTO_USE_IT` ← CDraggableItem
- `SP_872_YOU_ARE_ABLE_TO_USE_THIS_ONLY_R_NWHEN_YOU_ARE_WEARING_A_SHIELD` ← CDraggableItem
- `SP_873_YOU_MUST_HAVE_A_TAMED_MONSTER_R_NIN_ORDER_TO_EQUIP_THIS` ← CDraggableItem
- `SP_874_YOU_MUST_BE_EQUIPPED_WITH_A_SADDLE_R_NTO_EQUIP_THIS` ← CDraggableItem
- `SP_875_THIS_ITEM_IS_NOT_AVAILABLE_WITH_THE_WEAPON_R_NYOURE_CURRENLY_USING` ← CDraggableItem
- `SP_876_YOU_MAY_NOT_USE_THE_ITEM_WITH_THE_R_NCASH_ITEM_YOURE_CURRENTLY_EQUIPPED_WITH_R_N` ← CDraggableItem
- `SP_877_THIS_ITEM_CANNOT_BE_EQUIPPED_WITH_YOUR_CURRENT_PET` ← CDraggableItem
- `SP_878_YOU_CANNOT_CHANGE_YOUR_EQUIPMENT_ON_THIS_MAP` ← CDraggableItem
- `SP_879_YOU_DO_NOT_CURRENTLY_HAVE_A_PET_THAT_IS_ALIVE` ← CDraggableItem
- `SP_880_THE_PET_IS_UNABLE_TO_WEAR_THE_EQUIPMENT` ← CDraggableItem
- `SP_881_THE_ADMIN_CANT_DROP_ITEMS` ← CDraggableItem
- `SP_882_YOU_CANT_DROP_THIS_ITEM` ← CDraggableItem
- `SP_885_HOW_MANY_WILL_YOU_DROP` ← CDraggableItem
- `SP_886_THIS_ITEM_IS_UNAVAILABLE_TO_TRADE_WITH_OTHERS` ← CDraggableItem
- `SP_891_REPEATING_THE_SAME_LINE_OVER_AND_OVER_R_NCAN_NEGATIVELY_AFFECT_OTHER_USERS` ← CChatHelper
- `SP_892_TOO_MUCH_CHATTING_CAN_DISRUPT_R_NOTHER_PLAYERS_ABILITY_TO_PLAY_THE_GAME` ← CChatHelper
- `SP_893_YOU_ARE_CURRENTLY_BLOCKED_FROM_CHATTING` ← CChatHelper, CPet
- `SP_921_EBASIC_CONFIGURATION_FOR_MAPLESTORYN_R_N_R_NFUI_UIWINDOWIMG_UTILDLGEX_NOTICE` ← CStage
- `SP_950_MAP` ← CActionMan
- `SP_954_MOB_07DIMG` ← CMobTemplate
- `SP_956_CANVAS` ← CActionFrame, CAnimationDisplayer, CChatBalloon
- `SP_957_PROPERTY` ← CActionMan
- `SP_958_SHAPE2DVECTOR2D` ← CAnimationDisplayer, CDropPool, CMob, CReactorPool
- `SP_959_ORIGIN` ← CActionMan
- `SP_960_DEFAULT` ← CActionMan
- `SP_961_INFO` ← CActionMan, CSummonedBase
- `SP_966_SS` ← CAnimationDisplayer, CUIAvatarMegaphone
- `SP_977_X` ← CWvsPhysicalSpace2D
- `SP_982_FIXED` ← CPet
- `SP_984_MISS` ← CAnimationDisplayer
- `SP_986_ETC_EMOTIONEFFECT_S` ← CAvatar

## 2. 宠物召唤龙类逐函数关键行


### CPet（28 函数）


#### CPet::AutoSpeakingByEvent  (1712 B)

  - `void __thiscall CPet::AutoSpeakingByEvent(CPet *this, int a2)`
  - `if ( (CPet::GetUpgradePetSkill(this) & 0x100) != 0 )`
  - `CPetTemplate::GetAutoSpeakingByEvent(a1: &v18, a2);`
  - `if ( v18 != 0 && *v18 != 0 )`
  - `v4 = *(this + 34);`
  - `v13 = *(v4 + 20);`
  - `CAnimationDisplayer::LoadLayer(a1: this + 144);`
  - `sub_4145AB(a1: this + 292);`
  - `LOBYTE(v19) = 2;`
  - `v6 = sub_428226(a1: &v18);`
  - `_bstr_t::_bstr_t(a1: v6);`
  - `LOBYTE(v19) = 0;`
  - `CChatBalloon::MakeBalloon(`
  - `this: (this + 20),`

#### CPet::ChatCommand  (590 B)

  - `signed int __thiscall CPet::ChatCommand(CPet *this, int a2)`
  - `CPet::UpdatePetAbility(a1: this);`
  - `result = CPet::GetLevel(this);`
  - `if ( result >= 15 )`
  - `Time = timeGetTime();`
  - `v4 = sub_428226(a1: a2);`
  - `_bstr_t::_bstr_t(a1: v4);`
  - `return CPet::DoAction(a1: 2, a2: 0, a3: v6, a4: 1, a5: Time);`

#### CPet::CursedChatCommand  (4033 B)

- SP_ 节点：`SP_893_YOU_ARE_CURRENTLY_BLOCKED_FROM_CHATTING`
  - `void __thiscall CPet::CursedChatCommand(CPet *this)`
  - `Level = CPet::GetLevel(this);`
  - `if ( *(this + 62) <= -1 && Level >= 15 )`
  - `if ( *(*dword_BE7918 + 8292) != 0 )`
  - `CHATLOG_ADD(sString: String, nType: 8u);`
  - `if ( v23 != 0 )`
  - `v5 = *(this + 34);`
  - `for ( i = 0; ; i += 12 )`
  - `v8 = *(v5 + 76);`
  - `if ( v8 != 0 )`
  - `v8 = *(v8 - 4);`
  - `if ( v6 >= v8 )`
  - `v9 = (i + *(v5 + 76));`
  - `if ( Level >= *v9 && v9[1] >= Level )`
  - `v11 = *(v5 + 76);`
  - `if ( v11 != 0 )`
  - …（截断 16 行）

#### CPet::DoAction  (4659 B)

  - `void *__thiscall CPet::DoAction(int this, int a2, int a3, _DWORD *a4, int a5, int a6)`
  - `if ( a3 >= 0 )`
  - `v11 = (this + 144);`
  - `if ( CVecCtrl::IsOnLadder(this: *(this + 144) != 0 ? (*(this + 144) - 12) : 0) != 0`
  - `|| CVecCtrl::IsOnRope(this: *v11 != 0 ? (*v11 - 12) : 0) != 0 )`
  - `if ( v8 >= 9 )`
  - `*(this + 248) = v8;`
  - `CPet::PrepareActionLayer(this);`
  - `if ( a2 == 0 )`
  - `v13 = *sub_70D6E8(a1: &v34, a2: v8 - 9);`
  - `LOBYTE(v36) = 1;`
  - `if ( v13 != 0 )`
  - `v14 = **(this + 136);`
  - `v30 = *((*(*(this + 4) + 16))(a1: this + 4) + 4);`
  - `v15 = (*(*(this + 4) + 16))(a1: this + 4);`
  - `v16 = sub_9894F3(a1: *v15, a2: v30);`
  - …（截断 16 行）

#### CPet::GetAdditionalLayer  (222 B)

  - `char *__thiscall CPet::GetAdditionalLayer(CPet *this, int a2)`
  - `if ( a2 == 0 )`
  - `if ( a2 == 1 )`

#### CPet::GetBodyRect  (800 B)

  - `int __thiscall CPet::GetBodyRect(CPet *this, struct tagRECT *a2)`
  - `sub_703C36(a1: 0);`
  - `v3 = *(*(this + 63) + 4) + 16;`
  - `if ( sub_703CA5(a1: v3) != 0 )`
  - `return dword_BF0410(a1: a2);`
  - `v5 = *(this + 109);`
  - `v6 = *(this + 60);`
  - `v7 = *(this + 108);`
  - `v8 = sub_4144D4(a1: a2, a2: v3);`
  - `if ( (v6 & 1) == 0 )`
  - `sub_45730D(a1: v8, a2: v8 + 2);`
  - `return dword_BF0454(a1: v9, a2: v7, a3: v10);`

#### CPet::GetItemSlot  (882 B)

  - `int __thiscall CPet::GetItemSlot(CPet *this)`
  - `result = (*(**(this + 37) + 12))(a1: *(this + 37));`
  - `if ( result != 0 )`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v6) + 4);`
  - `if ( v7 != 0 )`
  - `sub_428C44(this: &v6, a2: 0);`
  - `CashItemSlotPosition = CharacterData::FindCashItemSlotPosition(this: v3, a2: 5, a3: *(this + 20));`
  - `v5 = *(CharacterData::GetItem(this: v3, a2: &v6, a3: 5, a4: CashItemSlotPosition) + 4);`
  - `ZRef<GW_ItemSlotBase>::~ZRef<GW_ItemSlotBase>(a1: 0);`

#### CPet::GetLevel  (294 B)

  - `signed int __thiscall CPet::GetLevel(CPet *this)`
  - `ItemSlot = CPet::GetItemSlot(this);`
  - `if ( ItemSlot != 0 )`
  - `return _ZtlSecureFuse<unsigned char>(a1: ItemSlot + 53, a2: *(ItemSlot + 55));`

#### CPet::GetUpgradePetSkill  (534 B)

  - `__int16 __thiscall CPet::GetUpgradePetSkill(CPet *this)`
  - `ItemSlot = CPet::GetItemSlot(this);`
  - `if ( ItemSlot != 0 )`
  - `InterlockedIncrement(lpAddend: (ItemSlot + 4));`
  - `if ( v4 != 0 )`
  - `v2 = sub_4E81DB(a1: v4 + 81, a2: *(v4 + 85));`
  - `sub_4F5818(a1: 0);`

#### CPet::Init  (6308 B)

  - `void __thiscall CPet::Init(CPet *this, struct CUser *a2, VARIANTARG **a3)`
  - `*(this + 37) = a2;`
  - `v6 = CInPacket::Decode4(this: v5);`
  - `*(this + 34) = CPetTemplate::GetPetTemplate(a1: v6);`
  - `v7 = CInPacket::DecodeStr(a1: &v49);`
  - `ZXString<char>::operator=(a1: v7);`
  - `if ( v49 != 0 )`
  - `v8 = sub_7078AF(a1: &v49, a2: dword_AFD5EC, a3: asc_AF1E78);`
  - `ZXString<char>::operator=(a1: v8);`
  - `CInPacket::DecodeBuffer(a1: this + 160, a2: 8u);`
  - `ItemSlot = CPet::GetItemSlot(this);`
  - `if ( ItemSlot != 0 )`
  - `*(this + 42) = _ZtlSecureFuse<short>(a1: ItemSlot + 59, a2: *(ItemSlot + 63));`
  - `*(this + 43) = _ZtlSecureFuse<unsigned char>(a1: v10 + 67, a2: *(v10 + 69));`
  - `*(this + 44) = _ZtlSecureFuse<short>(a1: v10 + 73, a2: *(v10 + 77));`
  - `v11 = *(this + 34);`
  - …（截断 16 行）

#### CPet::LoadLayer  (10470 B)

- SP_ 节点：`SP_2499_REPEAT`, `SP_2698_POS`, `SP_3_HTTP__MAPLESTORYNEXONNET_WZASPXPART_DOWNLOADS_GAMEPATCHES`, `SP_4817_REPEATSTARTINDEX`, `SP_982_FIXED`
  - `signed int __thiscall CPet::LoadLayer(CPet *this, _DWORD *a2, signed int *a3, signed int *a4, _DWORD *a5)`
  - `if ( a2 != 0 && *a2 != 0 )`
  - `v6 = *(*a2 - 4) >> 1;`
  - `if ( v6 == 0 )`
  - `sub_402D9A(this: &a2);`
  - `sub_402F85(pvargDest: &v61, pvargSrc: &pvargSrc);`
  - `LOBYTE(v71) = 1;`
  - `sub_402F85(pvargDest: &v62, pvargSrc: &pvargSrc);`
  - `sub_405210(a1: &a2);`
  - `LOBYTE(v71) = 2;`
  - `v8 = sub_406472();`
  - `v9 = sub_402680(a1: v8);`
  - `IWzResMan::GetObjectA(this: v9, pvargDest: &v60, sUOL: v55, vParam: v56, vAux: p_pvarg);`
  - `LOBYTE(v71) = 4;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v58, a2: v59);`
  - `if ( Unknown != 0 )`
  - …（截断 16 行）

#### CPet::OnAction  (1062 B)

  - `void __thiscall CPet::OnAction(_DWORD *this, int a2)`
  - `v2 = *(this + 37);`
  - `if ( v2 == 0 || *(v2 + 1384) == 0 )`
  - `v4 = CInPacket::Decode1(this: a2);`
  - `v5 = CInPacket::Decode1(this: v3);`
  - `CInPacket::DecodeStr(a1: &a2);`
  - `v10 = CInPacket::Decode1(this: v3);`
  - `v6 = sub_428226(a1: &a2);`
  - `_bstr_t::_bstr_t(a1: v6);`
  - `CPet::DoAction(a1: v4, a2: v5, a3: v8, a4: v9, a5: v10);`

#### CPet::OnActionCommand  (3949 B)

  - `unsigned int __thiscall CPet::OnActionCommand(CPet *this, int *a2)`
  - `result = *(this + 37);`
  - `if ( result != 0 && *(result + 1384) != 0 )`
  - `Level = CPet::GetLevel(this);`
  - `v5 = CInPacket::Decode1(this: a2);`
  - `if ( v5 == 0 )`
  - `*(this + 45) = 0;`
  - `result = 28 * CInPacket::Decode1(this: a2);`
  - `v6 = (result + *(*(this + 34) + 68));`
  - `if ( Level < *v6 || v6[1] < Level )`
  - `if ( CInPacket::Decode1(this: a2) != 0 )`
  - `if ( v5 != 1 )`
  - `if ( *v7 != 0 )`
  - `result = *(result - 4);`
  - `if ( result != 0 )`
  - `v12 = CRand32::Random(a1: dword_BEDD38) % v26;`
  - …（截断 16 行）

#### CPet::OnAvatarModified  (220 B)

  - `int __thiscall CPet::OnAvatarModified(CPet *this)`
  - `if ( *(this + 62) > -1 )`
  - `*(this + 62) = -1;`
  - `sub_703CF4(a1: this);`
  - `return CPet::PrepareActionLayer(this);`

#### CPet::OnLoadExceptionList  (3913 B)

- SP_ 节点：`SP_5510__MESOS`
  - `char __thiscall CPet::OnLoadExceptionList(_DWORD *this, const char *a2)`
  - `CInPacket::DecodeBuffer(a1: v19, a2: 8u);`
  - `LOBYTE(v3) = this - 96;`
  - `if ( v19[0] == *(this + 40) && v19[1] == *(this + 41) )`
  - `ZArray<long>::RemoveAll(a1: v4);`
  - `LOBYTE(v3) = CInPacket::Decode1(this: a2);`
  - `if ( v3 != 0 )`
  - `v6 = CInPacket::Decode4(this: a2);`
  - `v3 = ZArray<long>::InsertBefore(a1: -1);`
  - `while ( v5 != 0 );`
  - `if ( v23 > 0 )`
  - `while ( v3 < v23 )`
  - `CItemInfo::GetItemName(a1: &a2, a2: *(v7 + *v4));`
  - `v8 = *(v7 + *v4) == 0x7FFFFFFF;`
  - `if ( v8 )`
  - `LOBYTE(v26) = 1;`
  - …（截断 16 行）

#### CPet::OnMove  (156 B)

  - `void __thiscall CPet::OnMove(CPet *this, struct CInPacket *a2)`
  - `CMovePath::OnMovePacket(this: (*(this + 36) != 0 ? *(this + 36) - 12 + 428 : 428), a2);`

#### CPet::OnNameChanged  (1391 B)

  - `int __thiscall CPet::OnNameChanged(_DWORD *this, int a2)`
  - `v3 = CInPacket::DecodeStr(a1: v18);`
  - `ZXString<char>::operator=(a1: v3);`
  - `if ( CInPacket::Decode1(this: a2) != 0 )`
  - `v5 = *(*(this + 34) + 16);`
  - `CAnimationDisplayer::LoadLayer(a1: this + 36);`
  - `sub_4145AB(a1: this + 73);`
  - `v7 = sub_428226(a1: this + 38);`
  - `return CLife::MakeNameTag(this, a2: v7, a3: v9, a4: v10, a5: v11, a6: v12, a7: v13, a8: v14, a9: v15, i: v16);`

#### CPet::OnResolveMoveAction  (1714 B)

  - `int __thiscall CPet::OnResolveMoveAction(CPet **this, int nInputX, int nInputY, int nCurMoveAction, CVecCtrl *pvc)`
  - `if ( _ZtlSecureFuse<CLadderOrRope *>(a1: pvc + 73, a2: *(pvc + 75)) != 0 )`
  - `VecCtrl = CUser::GetVecCtrl(this: *(this + 36), a2: &nCurMoveAction);`
  - `if ( nCurMoveAction != 0 )`
  - `(*(*nCurMoveAction + 8))(a1: nCurMoveAction);`
  - `if ( _ZtlSecureFuse<CLadderOrRope *>(a1: (v9 + 292), a2: *(v9 + 300)) != 0 )`
  - `return *(*(this + 36) + 348) & 1 | 0x1E;`
  - `if ( *(v5 + 68) != 0 )`
  - `if ( nInputX != 0 )`
  - `return (nInputX < 0) | 2;`
  - `if ( *(this + 42) <= 50 )`
  - `v11 = *(this + 60) - 1;`
  - `if ( v11 != 0 )`
  - `if ( v12 == 0 )`
  - `if ( v13 == 0 )`
  - `if ( v13 == 1 )`
  - …（截断 16 行）

#### CPet::OnValidateStat  (2326 B)

- SP_ 节点：`SP_380_THE_CLOSENESS_INCREASED_D`, `SP_381_THE_CLOSENESS_DECREASED_D`
  - `void __thiscall CPet::OnValidateStat(CPet *this)`
  - `if ( (*(**(this + 37) + 12))(a1: *(this + 37)) != 0 )`
  - `ItemSlot = CPet::GetItemSlot(this);`
  - `v3 = *(this + 42);`
  - `*(this + 42) = _ZtlSecureFuse<short>(a1: ItemSlot + 59, a2: *(ItemSlot + 63));`
  - `*(this + 43) = _ZtlSecureFuse<unsigned char>(a1: v4 + 67, a2: *(v4 + 69));`
  - `*(this + 44) = _ZtlSecureFuse<short>(a1: v4 + 73, a2: *(v4 + 77));`
  - `CPet::SetShoeAttr(this);`
  - `CPet::UpdatePetAbility(a1: this);`
  - `v5 = *(this + 42);`
  - `if ( v3 >= v5 )`
  - `if ( v3 <= v5 )`
  - `v13 = *(this + 42) - v3;`
  - `LOBYTE(v16) = 2;`
  - `ZXString<char>::Format(a1: &v15, a2: v11, a3: v13);`
  - `LOBYTE(v16) = 0;`
  - …（截断 16 行）

#### CPet::ParseCommand  (5031 B)

  - `signed int __thiscall CPet::ParseCommand(int this, volatile LONG *a2)`
  - `v3 = *(this + 248);`
  - `if ( v3 > -1`
  - `|| *(this + 180) != 0`
  - `|| *(this + 184) < 500`
  - `|| (v5 = this + 148, (*(**(this + 148) + 12))(a1: *(this + 148)) == 0)`
  - `|| *(*v5 + 1388) > 0 )`
  - `if ( a2 != 0 )`
  - `ZXString<char>::TrimRight(a1: 0);`
  - `v6 = ZXString<char>::TrimLeft(a1: 0);`
  - `ZXString<char>::operator=(a1: v6);`
  - `v7 = (this + 152);`
  - `if ( ZXString<char>::operator==(a1: *(this + 136) + 4) == 0 )`
  - `v8 = a2 != 0 ? *(a2 - 1) : 0;`
  - `v9 = *v7 != 0 ? *(*v7 - 4) : 0;`
  - `if ( v8 > v9 )`
  - …（截断 16 行）

#### CPet::PrepareActionLayer  (5907 B)

  - `int __thiscall CPet::PrepareActionLayer(CPet *this)`
  - `result = *(this + 37);`
  - `if ( result == 0 || *(result + 1384) == 0 )`
  - `v4 = sub_703C36(a1: &v31);`
  - `if ( v4 == 8 && *(this + 39) == 0 && *(*(*(this + 37) + 7900) + 12) != 0 )`
  - `if ( *(this + 68) == 0 )`
  - `CActionMan::LoadPetAction(a1: dword_BE78D4, a2: 0, a3: 8, a4: 0, a5: this + 264);`
  - `v34 = (*(this + 65) + 20 * v4);`
  - `if ( *(v5 + 2) == 0 )`
  - `v6 = *(this + 39);`
  - `v7 = *(this + 37);`
  - `if ( v6 != 0 )`
  - `if ( v6 == 1 )`
  - `v8 = *(v7 + 285);`
  - `else if ( v6 == 2 )`
  - `v8 = *(v7 + 317);`
  - …（截断 16 行）

#### CPet::SetMoveAction  (301 B)

  - `void __thiscall CPet::SetMoveAction(CPet *this, int a2, int a3)`
  - `if ( a3 != 0 || a2 != *(this + 60) )`
  - `v3 = *(this + 62) <= -1;`
  - `*(this + 60) = a2;`
  - `if ( v3 )`
  - `CPet::PrepareActionLayer(this);`

#### CPet::SetPositionContext  (105 B)

  - `int __thiscall CPet::SetPositionContext(CPet *this, int a2)`
  - `return sub_9C50C1(a1: a2);`

#### CPet::SetSetItemBackground  (13895 B)

- SP_ 节点：`SP_4819_EFFECT_SETEFFIMG_D_PETBACKGRND_D`
  - `int __thiscall CPet::SetSetItemBackground(_DWORD *this, int *a2, signed int *a3)`
  - `result = CPet::GetAdditionalLayer(this, a2: 1);`
  - `if ( *(result + 20) != 0 )`
  - `update_time = get_update_time();`
  - `if ( (*(*(this + 37) + 1392) & 0xFFFFFFFE) != 4 || a3 != 0 )`
  - `if ( *v5 == 0 )`
  - `sub_432D38(a1: &a2);`
  - `if ( a2 == 0 )`
  - `Alpha = IWzGr2DLayer::GetAlpha(a1: &v62);`
  - `LOBYTE(v68) = 17;`
  - `sub_4281FB(a1: &a2);`
  - `a3 = IWzGr2DLayer::GetAlpha(a1: &Flip);`
  - `LOBYTE(v68) = 18;`
  - `v51 = sub_432BFA(a1: Alpha);`
  - `v56 = IWzShape2D::Getx(a1: v51);`
  - `sub_432BFA(a1: a3);`
  - …（截断 16 行）

#### CPet::SetSetItemEffect  (9815 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_4818_EFFECT_SETEFFIMG_D_PETEFFECT`, `SP_4819_EFFECT_SETEFFIMG_D_PETBACKGRND_D`
  - `void __thiscall CPet::SetSetItemEffect(CPet *this, int a2, _DWORD *a3)`
  - `if ( a2 < 0 || *(*(this + 37) + 1320) != 0 )`
  - `sub_706540(a1: 0);`
  - `sub_706540(a1: 1);`
  - `AdditionalLayer = CPet::GetAdditionalLayer(this, a2: 0);`
  - `if ( *AdditionalLayer != a2 )`
  - `LOBYTE(v48) = 1;`
  - `sub_416821(a1: &v47, a2: v6, a3: a2);`
  - `LOBYTE(v48) = 0;`
  - `if ( v44 != 0 )`
  - `v7 = *(v3 + 37);`
  - `p_Layer = (AdditionalLayer + 12);`
  - `LayerUnderFace = CAvatar::GetLayerUnderFace(this: (v7 + 136), a2: &v44);`
  - `LOBYTE(v48) = 2;`
  - `v9 = sub_4281FB(a1: LayerUnderFace);`
  - `HIDWORD(v38) = IWzGr2DLayer::GetFlip(this: v9);`
  - …（截断 16 行）

#### CPet::SetShoeAttr  (2765 B)

  - `int __thiscall CPet::SetShoeAttr(CPet *this)`
  - `result = CPet::GetItemSlot(this);`
  - `v3 = *(this + 37);`
  - `if ( v3 != 0 && result != 0 )`
  - `result = (*(*v3 + 12))(a1: v3);`
  - `if ( result != 0 )`
  - `result = sub_7050AC(a1: v4);`
  - `(*(*(*(this + 37) + 4) + 12))(a1: *(this + 37) + 4, a2: v16);`
  - `Data = TSecType<double>::GetData(a1: v17 + 12);`
  - `sub_500D61(a1: SLOBYTE(Data), a2: HIDWORD(Data));`
  - `v6 = TSecType<double>::GetData(a1: v17 + 24);`
  - `sub_500D61(a1: SLOBYTE(v6), a2: HIDWORD(v6));`
  - `v7 = TSecType<double>::GetData(a1: v17 + 36);`
  - `sub_500D61(a1: SLOBYTE(v7), a2: HIDWORD(v7));`
  - `v8 = TSecType<double>::GetData(a1: v17 + 48);`
  - `sub_500D61(a1: SLOBYTE(v8), a2: HIDWORD(v8));`
  - …（截断 16 行）

#### CPet::Update  (6725 B)

  - `CVecCtrl *__thiscall CPet::Update(CPet *this)`
  - `if ( *(this + 116) == 0 )`
  - `CChatBalloon::CheckTimeOut(this: (this + 20), a2: 0);`
  - `VecCtrl = CUser::GetVecCtrl(this: *(this + 37), a2: &v32);`
  - `if ( v32 != 0 )`
  - `(*(*v32 + 8))(a1: v32);`
  - `v28 = *(v3 + 300);`
  - `v31 = *(this + 36) != 0 ? (*(this + 36) - 12) : 0;`
  - `v5 = _ZtlSecureFuse<CLadderOrRope *>(a1: (v3 + 292), a2: v28) == 0;`
  - `v6 = *(v31 + 6);`
  - `if ( v6 != 0 )`
  - `if ( *(this + 62) <= -1 || v32 != 0 )`
  - `CVecCtrl::UpdateActive(this: v31);`
  - `updated = CVecCtrl::UpdatePassive(this: v31);`
  - `if ( updated == -1 )`
  - `if ( (updated & 0x10) != 0 )`
  - …（截断 16 行）

#### CPet::UpdatePetAbility  (5262 B)

  - `int *__thiscall CPet::UpdatePetAbility(CPet *this)`
  - `if ( *(this + 37) == 0 )`
  - `v4 = *(v32 + 8 * *v33 + 4);`
  - `if ( v4 != 0 )`
  - `Data = TSecType<long>::GetData(a1: (v4 + 12), a2: v3);`
  - `EquipItem = CItemInfo::GetEquipItem(this: v5, a2: Data);`
  - `if ( EquipItem != 0 )`
  - `if ( (v8 = *v33, *v33 == 24)`
  - `|| (v9 = *(this + 39)) == 0 && (v8 >= 21 && v8 <= 29 || v8 == 46)`
  - `|| v9 == 1 && (v8 >= 31 && v8 <= 37 || v8 == 47)`
  - `|| v9 == 2 && (v8 >= 39 && v8 <= 45 || v8 == 48) )`
  - `v3 |= *(EquipItem + 496);`
  - `while ( v33 < dword_BD85F0 );`
  - `v27 = *(this + 88);`
  - `Time = timeGetTime();`
  - `v25 = *(this + 82);`
  - …（截断 16 行）

### CVecCtrlPet（6 函数）


#### CVecCtrlPet::CollisionDetectWalk  (3085 B)

  - `int __userpurge CVecCtrlPet::CollisionDetectWalk@<eax>(`
  - `v18 = _ZtlSecureFuse<double>(a1: this + 224, a2: *(this + 60));`
  - `v19 = v18 - _ZtlSecureFuse<double>(a1: a2, a2: *(a2 + 4));`
  - `if ( v19 == dbl_AF0DE8 )`
  - `return CVecCtrl::CollisionDetectWalk(this, a2, a3);`
  - `v6 = *(this + 5);`
  - `if ( v6 != 0 )`
  - `v8 = *(v7 + 148);`
  - `v9 = *(*(this + 68) + 32);`
  - `v11 = *(v10 + 8 * v9);`
  - `v12 = *(v10 + 8 * v9 + 4);`
  - `if ( v12 - v11 > 40 )`
  - `_ZtlSecureFuse<double>(a1: this + 224, a2: *(this + 60));`
  - `if ( v19 >= dbl_AF0DE8 || _ZtlSecureFuse<double>(a1: this + 224, a2: *(this + 60)) >= dbl_AF0DE8 )`
  - `v13 = *(*(this + 68) + 76);`
  - `if ( v13 != 0 )`
  - …（截断 16 行）

#### CVecCtrlPet::CreateInstance  (310 B)

  - `int CVecCtrlPet::CreateInstance()`
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 632) != 0 )`
  - `v0 = sub_9C4020();`
  - `if ( v0 == 0 )`
  - `_com_raise_error(a1: -2147024882, a2: 0);`

#### CVecCtrlPet::EndUpdateActive  (970 B)

  - `int __thiscall CVecCtrlPet::EndUpdateActive(CVecCtrlPet *this)`
  - `v2 = (this + 428);`
  - `result = CMovePath::IsTimeForFlush(this: (this + 428), a2: 0);`
  - `if ( result != 0 )`
  - `COutPacket::COutPacket(a1: 167);`
  - `v4 = *(this + 5);`
  - `if ( v4 != 0 )`
  - `COutPacket::EncodeBuffer(a1: (v5 + 160), a2: 8u);`
  - `if ( CMovePath::Flush(this: v2, a2: v6, a3: 0, a4: 0) == 0 )`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: v6);`
  - `return ZArray<unsigned char>::RemoveAll(a1: v7);`

#### CVecCtrlPet::Init  (259 B)

  - `int __thiscall CVecCtrlPet::Init(CVecCtrlPet *this, struct IVecCtrlOwner *a2, int a3)`
  - `CVecCtrl::Init(this, a2);`
  - `*(this + 153) = 0;`
  - `*(this + 154) = 0;`
  - `*(this + 146) = a3;`

#### CVecCtrlPet::SetActive  (342 B)

  - `void *__thiscall CVecCtrlPet::SetActive(`
  - `CVecCtrl::SetActive(this, a2, a3, a4: a4 - 1, a5, a6, a7, a8);`
  - `return memset(a1: this + 620, a2: 0, a3: 8u);`

#### CVecCtrlPet::WorkUpdateActive  (16790 B)

  - `signed int __thiscall CVecCtrlPet::WorkUpdateActive(CVecCtrlPet *this, int tElapse)`
  - `v3 = *(this + 5);`
  - `if ( v3 != 0 )`
  - `nPetX = *(v4 + 148);`
  - `VecCtrl = CUser::GetVecCtrl(this: nPetX, a2: &v92);`
  - `sub_41569E(a1: &v92);`
  - `if ( _ZtlSecureFuse<CLadderOrRope *>(a1: v6 + 73, a2: v76) == 0`
  - `|| _ZtlSecureFuse<CLadderOrRope *>(a1: this + 73, a2: *(this + 75)) == 0 )`
  - `if ( _ZtlSecureFuse<CLadderOrRope *>(a1: v6 + 73, a2: v6[75]) != 0 )`
  - `v11 = _ZtlSecureFuse<double>(a1: this + 32, a2: *(this + 12));`
  - `v66 = *(this + 18);`
  - `nPetX = _ZtlSecureFuse<double>(a1: this + 56, a2: v66);`
  - `qmemcpy(this + 32, (v98 + 32), 0x60u);`
  - `v12 = _ZtlSecureFuse<double>(a1: this + 32, a2: *(this + 12));`
  - `v65 = *(this + 18);`
  - `v13 = _ZtlSecureFuse<double>(a1: this + 56, a2: v65);`
  - …（截断 16 行）

### CPetTemplate（2 函数）


#### CPetTemplate::GetAutoSpeakingByEvent  (568 B)

  - `_DWORD *__thiscall CPetTemplate::GetAutoSpeakingByEvent(_DWORD *this, _DWORD *a2, unsigned int a3)`
  - `v3 = *(this + 21);`
  - `if ( v3 != 0 && *(v3 - 4) > a3 )`
  - `v5 = *(this + 21);`
  - `ZXString<char>::operator=(a1: v5 + 4 * a3);`
  - `ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF);`

#### CPetTemplate::GetPetTemplate  (419 B)

  - `static int __cdecl CPetTemplate::GetPetTemplate(unsigned int a1)`
  - `sub_70DC0C(&a1, a2: v3);`
  - `if ( v4 != 0 )`
  - `sub_70D92F(a1: 0);`

### CUIPetEquip（2 函数）


#### CUIPetEquip::OnButtonClicked  (345 B)

  - `void __thiscall CUIPetEquip::OnButtonClicked(CUIPetEquip *this, unsigned int a2)`
  - `if ( a2 == 2000 )`
  - `CUIEquip::TogglePetEquip(this: dword_BED64C);`
  - `else if ( a2 > 0x7D0 && a2 <= 0x7D3 )`
  - `*(this + 360) = a2 - 2001;`
  - `CWnd::InvalidateRect(this, a2: 0);`

#### CUIPetEquip::OnMouseMove  (4826 B)

  - `int __thiscall CUIPetEquip::OnMouseMove(char **this, int a2, int a3)`
  - `v33 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v34) + 4);`
  - `if ( v35 != 0 )`
  - `sub_428C44(this: &v34, a2: 0);`
  - `v5 = sub_8011FA(a1: a2, a2: a3);`
  - `v35 = *(CharacterData::GetItem(this: v33, a2: v29, a3: 1, a4: -v5) + 4);`
  - `if ( v30 != 0 )`
  - `ZRef<GW_ItemSlotBase>::~ZRef<GW_ItemSlotBase>(a1: 0);`
  - `v6 = sub_8011FA(a1: a2, a2: a3);`
  - `v37 = *(CharacterData::GetItem(this: v33, a2: v31, a3: 1, a4: -100 - v6) + 4);`
  - `if ( v32 != 0 )`
  - `CInputSystem::SetCursorState(this: dword_BEC33C, a2: 5);`
  - `if ( v37 != 0 )`
  - `v24 = (*(v10 + 12))(a1: this) + a3 + 20;`
  - `v11 = (*(*this + 11))(a1: this);`
  - `CUIToolTip::SetToolTip_Equip2(this: (this + 26), a2: (a2 + v11), a3: v24, a4: v25, a5: v37, a6: 1, a7: 0);`
  - …（截断 16 行）

### CSummoned（16 函数）


#### CSummoned::GetBodyRect  (1232 B)

  - `signed int __thiscall CSummoned::GetBodyRect(CSummoned *this, struct tagRECT *a2, int a3)`
  - `v4 = (this + 288);`
  - `if ( dword_BF04AC(a1: this + 288) != 0 )`
  - `dword_BF0410(a1: a2);`
  - `v11 = sub_7A3980(a1: this);`
  - `if ( v11 == 0 )`
  - `v4 = (this + 304);`
  - `dword_BF0454(a1: a2, a2: *(this + 86), a3: *(this + 87));`
  - `if ( a3 != 0 )`
  - `v9 = *(this + 89);`
  - `v8 = *(this + 88);`
  - `dword_BF0454(a1: &v10, a2: v8, a3: v9);`
  - `dword_BF0408(a1: a2, a2, a3: &v10);`

#### CSummoned::GetCurrentAction  (787 B)

  - `int __thiscall CSummoned::GetCurrentAction(CSummoned *this, int *a2)`
  - `v3 = _ZtlSecureFuse<long>(a1: this + 220, a2: *(this + 57));`
  - `if ( a2 != 0 )`
  - `v5 = (v3 >> 1) - 1;`
  - `if ( v5 != 0 )`
  - `if ( v5 == 5 )`
  - `v9 = *(this + 60);`
  - `v6 = (this + 240);`
  - `if ( _ZtlSecureFuse<long>(a1: v7, a2: v9) <= -1 )`
  - `return _ZtlSecureFuse<long>(a1: v7, a2: *v6);`

#### CSummoned::GetHeight  (945 B)

  - `int __thiscall CSummoned::GetHeight(CSummoned *this)`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `if ( *(this + 54) == 0 )`
  - `IWzGr2DLayer::GetCanvas(a1: &v7, a2: &pvarg);`
  - `LOBYTE(v8) = 2;`
  - `if ( v2 < 0 )`
  - `if ( v7 == 0 )`
  - `v3 = IWzCanvas::Getcy(this: v7);`
  - `if ( v3 <= 0 )`
  - `if ( v7 != 0 )`
  - `(*(*v7 + 8))(a1: v7);`

#### CSummoned::Init  (1116 B)

  - `int __thiscall CSummoned::Init(CSummoned *this, struct CInPacket *a2, CInPacket *a3)`
  - `v5 = CInPacket::Decode2(this: a3);`
  - `v9 = CInPacket::Decode2(this: a3);`
  - `v10 = CInPacket::Decode1(this: a3);`
  - `LOWORD(v6) = CInPacket::Decode2(this: a3);`
  - `v11 = CInPacket::Decode1(this: a3);`
  - `v12 = CInPacket::Decode1(this: a3);`
  - `Skill = CSkillInfo::GetSkill(this: dword_BE78DC, a2: *(this + 45));`
  - `CWvsPhysicalSpace2D::GetFoothold(dwSN: v14);`
  - `if ( Skill == 0 )`
  - `v8 = CInPacket::Decode1(this: a3);`
  - `return CSummoned::Init(this, a2, a3: v5, a4: v9, a5: v10, Foothold: v14, a7: v11, a8: v12, a9: v8);`

#### CSummoned::MakeIncDecHPEffect  (835 B)

  - `void __thiscall CSummoned::MakeIncDecHPEffect(CSummoned *this, int a2)`
  - `v5 = (*(*(this + 1) + 16))(a1: this + 4);`
  - `if ( a2 <= 0 )`
  - `v9 = *(v5 + 4) - CSummoned::GetHeight(this);`
  - `v7 = (*(*v4 + 16))(a1: v4);`
  - `CAnimationDisplayer::Effect_Miss(this: v2, a2: *v7, a3: v9, a4: 2);`
  - `v8 = *(v5 + 4) - CSummoned::GetHeight(this);`
  - `v6 = (*(*v4 + 16))(a1: v4);`
  - `CAnimationDisplayer::Effect_HP(this: v2, lCenterLeft: *v6, lCenterTop: v8, idx: a2, a5: 2, lY: 0);`

#### CSummoned::OnResolveMoveAction  (786 B)

  - `int __thiscall CSummoned::OnResolveMoveAction(int *this, int nInputX, int nInputY, bool nCurMoveAction, int pvc)`
  - `v5 = *(this + 48);`
  - `if ( v5 != 0 )`
  - `if ( v5 == 1 )`
  - `if ( nInputX != 0 )`
  - `return (nInputX < 0) | 2;`
  - `if ( v5 <= 2 || v5 > 4 )`
  - `return (nInputX < 0) | 0xC;`

#### CSummoned::PrepareActionLayer  (5193 B)

  - `int __thiscall CSummoned::PrepareActionLayer(CSummoned *this)`
  - `CurrentAction = CSummoned::GetCurrentAction(this, a2: &v33);`
  - `v3 = *(this + 61);`
  - `v5 = *(v4 + 8);`
  - `if ( v5 == 0 )`
  - `CActionMan::LoadSummonedAction(`
  - `a2: *(this + 45),`
  - `a3: *(this + 47),`
  - `if ( *(v4 + 8) == 0 )`
  - `v6 = *(this + 49);`
  - `if ( v6 == 3 || v6 == 4 )`
  - `CActionMan::LoadSummonedAction(a1: dword_BE78D4, a2: *(this + 45), a3: *(this + 47), a4: v7, a5: v4);`
  - `_CxxThrowException(a1: &v35, a2: &_TI3_AVCTerminateException__);`
  - `sub_402FAB(a1: -2, a2: 3);`
  - `v8 = *(this + 54) == 0;`
  - `if ( v8 )`
  - …（截断 16 行）

#### CSummoned::ProcessAction  (3401 B)

  - `int __thiscall CSummoned::ProcessAction(CSummoned *this, int a2)`
  - `CurrentAction = CSummoned::GetCurrentAction(this, a2: 0);`
  - `*(this + 63) -= 30;`
  - `if ( *(this + 63) <= 0 )`
  - `v4 = *(this + 61) + 20 * CurrentAction;`
  - `v5 = *(*(this + 62) != 0 ? *(this + 62) - 16 + 4 : 4);`
  - `*(this + 62) = v5 != 0 ? v5 + 16 : 0;`
  - `v17 = *(this + 60);`
  - `if ( _ZtlSecureFuse<long>(a1: this + 232, a2: v17) <= -1 )`
  - `if ( *(this + 62) == 0 )`
  - `*(this + 62) = *(v4 + 12);`
  - `else if ( *(this + 62) == 0 )`
  - `v6 = _ZtlSecureTear<long>(a1: -1, a2: this + 232);`
  - `*(this + 82) = 0;`
  - `*(this + 60) = v6;`
  - `CSummoned::PrepareActionLayer(this);`
  - …（截断 16 行）

#### CSummoned::ProcessAttack  (23230 B)

  - `int __thiscall CSummoned::ProcessAttack(CSummoned *this, int a2)`
  - `result = *(this + 35);`
  - `if ( result != 0 && a2 - result >= 0 )`
  - `HIDWORD(v74[0]) = 1;`
  - `LODWORD(v74[0]) = a2 - result;`
  - `sub_4145AB(a1: this + 216);`
  - `HIDWORD(dblVal) = v4;`
  - `CAnimationDisplayer::LoadLayer(a1: this + 332);`
  - `LODWORD(dblVal) = *(this + 36) == 0;`
  - `HIDWORD(v71) = sub_40B31F(a1: this + 148);`
  - `v7 = sub_432CBC(a1: v6, a2: v5);`
  - `CAnimationDisplayer::Effect_General(`
  - `a2: HIDWORD(v71),`
  - `a3: SLODWORD(dblVal),`
  - `a4: SHIDWORD(dblVal),`
  - `a5: SLODWORD(v73),`
  - …（截断 16 行）

#### CSummoned::SetDamaged  (5708 B)

  - `int __thiscall CSummoned::SetDamaged(CSummoned *this, int a2, int a3, int a4, struct CMob *a5, int a6, int a7)`
  - `result = _ZtlSecureFuse<long>(a1: this + 260, a2: *(this + 67));`
  - `if ( result == 0 && *(this + 53) == 0 )`
  - `if ( a5 != 0 )`
  - `result = sub_670C06(a1: a5);`
  - `if ( result != 0 || *(a5 + 202) != 0 )`
  - `v39 = *(a5 + 98);`
  - `AttackInfo = CMobTemplate::GetAttackInfo(a1: a6);`
  - `if ( AttackInfo != 0 && AttackInfo[7] != 0 )`
  - `v11 = sub_793735(a1: a5 + 416, a2: *(this + 47));`
  - `v11 = sub_793687(a1: a5 + 416, a2: *(this + 47));`
  - `if ( v41 != 0 )`
  - `CSummoned::GetBodyRect(this, a2: &v36, a3: 1);`
  - `if ( v36.right == v36.left )`
  - `left = CRand32::Random(a1: dword_BEDD38);`
  - `v13 = CRand32::Random(a1: dword_BEDD38);`
  - …（截断 16 行）

#### CSummoned::SetMoveAction  (499 B)

  - `void __thiscall CSummoned::SetMoveAction(CSummoned *this, int a2, int a3)`
  - `if ( a3 != 0 || a2 != _ZtlSecureFuse<long>(a1: this + 220, a2: *(this + 57)) )`
  - `v4 = _ZtlSecureTear<long>(a1: a2, a2: this + 220);`
  - `v5 = *(this + 60);`
  - `*(this + 57) = v4;`
  - `if ( _ZtlSecureFuse<long>(a1: this + 232, a2: v5) <= -1 )`
  - `CSummoned::PrepareActionLayer(this);`

#### CSummoned::ShowEffect  (1590 B)

- SP_ 节点：`SP_2870_EFFECT_BASICEFFIMG_SUMMONED`
  - `void __thiscall CSummoned::ShowEffect(CSummoned *this, int a2)`
  - `if ( a2 == 0 )`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `LOBYTE(v18) = 0;`
  - `sub_432C10(a1: 0);`
  - `CAnimationDisplayer::LoadLayer(a1: this + 332);`
  - `v9 = sub_41468C(a1: &v17);`
  - `v8 = sub_432CBC(a1: v7, a2: v6);`
  - `CAnimationDisplayer::Effect_General(this: v8, a2: v9, a3: 0, a4: v10, a5: v11, a6: v12, a7: v13, a8: v14);`

#### CSummoned::TryDoingAttack  (6885 B)

  - `void *__userpurge CSummoned::TryDoingAttack@<eax>(CSummoned *this@<ecx>, int a2, int a3)`
  - `v4 = *(this + 50) == 2;`
  - `if ( v4 )`
  - `v6 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v47) + 4);`
  - `sub_4289B7(this: v47);`
  - `v51 = (v5 + 8500);`
  - `result = CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v6, a3: 1320009, a4: &dword_BF0D40);`
  - `if ( result > 0 )`
  - `LevelData = SKILLENTRY::GetLevelData(this: dword_BF0D40, a2: v7, a3: result);`
  - `v9 = _ZtlSecureFuse<long>(a1: LevelData + 7, a2: LevelData[9]);`
  - `v10 = _ZtlSecureFuse<long>(a1: LevelData + 22, a2: v39);`
  - `v11 = _ZtlSecureFuse<long>(a1: LevelData + 19, a2: v38);`
  - `v12 = _ZtlSecureFuse<long>(a1: LevelData + 16, a2: v37);`
  - `v13 = _ZtlSecureFuse<long>(a1: LevelData + 10, a2: v36);`
  - `v52 = (v49 != 0) + (v50 != 0) + (i != 0) + (v52 != 0) + (v13 != 0);`
  - `v14 = _ZtlSecureFuse<long>(a1: LevelData + 7, a2: v35);`
  - …（截断 16 行）

#### CSummoned::TryDoingAttackManual  (26839 B)

  - `void __thiscall CSummoned::TryDoingAttackManual(_DWORD *this, int a2)`
  - `v3 = *(this + 50) == 1;`
  - `if ( v3 )`
  - `v128 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v132) + 4);`
  - `if ( v133 != 0 )`
  - `sub_428C44(this: &v132, a2: 0);`
  - `v3 = *(this + 45) == &off_4FA6A2;`
  - `v134 = ((CRand32::Random(a1: dword_BEDD38) & 1) == 0) | 4;`
  - `if ( sub_766612(a1: *(this + 45)) != 0`
  - `|| *(this + 45) == (&loc_4F8376 + 4)`
  - `|| CUser::IsOnLadderOrRope(this: TSingleton<CUserLocal>::ms_pInstance) == 0`
  - `&& (v6 = *(TSingleton<CUserLocal>::ms_pInstance + 1348) / 10000) != 190`
  - `&& _ZtlSecureFuse<long>(a1: v5 + 468, a2: *(v5 + 476)) == 0`
  - `&& _ZtlSecureFuse<long>(a1: v5 + 2644, a2: *(v5 + 2652)) == 0 )`
  - `v111[0] = *(this + 45);`
  - `v7 = *(CSummonedBase::LoadAttackInfo(a1: v121, a2: v111[0], a3: v134 - 4) + 4);`
  - …（截断 16 行）

#### CSummoned::TryDoingHeal  (3165 B)

  - `int __thiscall CSummoned::TryDoingHeal(CSummoned *this, int a2)`
  - `v2 = *(this + 50) == 2;`
  - `if ( !v2 )`
  - `v4 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v23) + 4);`
  - `if ( v24 != 0 )`
  - `sub_428C44(this: &v23, a2: 0);`
  - `SkillLevel = CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v4, a3: 1320008, a4: &dword_BF0D38);`
  - `if ( SkillLevel <= 0 )`
  - `v19 = *(v3 + 8484);`
  - `v5 = _ZtlSecureFuse<long>(a1: v3 + 8476, a2: v19);`
  - `v18 = *(v4 + 101);`
  - `if ( v5 <= _ZtlSecureFuse<short>(a1: v4 + 97, a2: v18) )`
  - `LevelData = SKILLENTRY::GetLevelData(this: dword_BF0D38, a2: v6, a3: SkillLevel);`
  - `SkillLevel = CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v4, a3: 1320006, a4: &dword_BF0D3C);`
  - `if ( SkillLevel > 0 && *(v3 + 14124) != 0 )`
  - `v8 = _ZtlSecureFuse<short>(a1: v27, a2: *(v4 + 101));`
  - …（截断 16 行）

#### CSummoned::Update  (4423 B)

  - `int __fastcall CSummoned::Update(CSummoned *a1, int a2)`
  - `update_time = get_update_time();`
  - `v4 = *(a1 + 83) != 0 ? (*(a1 + 83) - 12) : 0;`
  - `if ( v5 != 0 )`
  - `CVecCtrl::UpdateActive(this: v4);`
  - `if ( _ZtlSecureFuse<long>(a1: a1 + 260, a2: *(a1 + 67)) == 0 )`
  - `CSummoned::GetBodyRect(this: a1, a2: &v23, a3: 1);`
  - `BodyAttackMob = CMobPool::FindBodyAttackMob(a1: &v23, a2: 0);`
  - `if ( BodyAttackMob != 0 )`
  - `v27 = (*(BodyAttackMob[1] + 20))(a1: BodyAttackMob + 1);`
  - `v8 = (*(*v7 + 16))(a1: v7);`
  - `v10 = v9 - *(*(*(a1 + 1) + 16))(a1: a1 + 1);`
  - `v11 = *(*(*(a1 + 1) + 20))(a1: a1 + 1) + v10;`
  - `if ( _ZtlSecureFuse<CLadderOrRope *>(a1: v4 + 73, a2: v4[75]) != 0 )`
  - `v12 = (*(*(a1 + 1) + 20))(a1: a1 + 1);`
  - `v13 = *(a1 + 1);`
  - …（截断 16 行）

### CSummonedBase（1 函数）


#### CSummonedBase::LoadAttackInfo  (23498 B)

- SP_ 节点：`SP_1617_PADAMAGE`, `SP_1619_MADAMAGE`, `SP_1636_RANGE`, `SP_1637_MOBCOUNT`, `SP_1642_SP`, `SP_1643_R`, `SP_1654_ATTACKAFTER`, `SP_1655_EFFECTAFTER`, `SP_2819_BULLETSPEED`, `SP_5636_LT`, `SP_5647_RB`, `SP_961_INFO`
  - `int __stdcall CSummonedBase::LoadAttackInfo(int a1, UnwindMapEntry *a2, int a3)`
  - `v101 = a3 | (2 * a2);`
  - `if ( sub_7AD6D7(a1: &v101, a2: v106) != 0 )`
  - `*(a1 + 4) = v107;`
  - `if ( !v86 )`
  - `InterlockedIncrement(lpAddend: (v88 + 4));`
  - `LOBYTE(v120) = 0;`
  - `if ( v88 != 0 )`
  - `sub_7A961F(a1: 0);`
  - `CActionMan::GetSummonedProp(a1: &v110, a2, a3: 0);`
  - `if ( v110 == 0 )`
  - `*(a1 + 4) = 0;`
  - `sub_4027E9(a1: &v110);`
  - `sub_7AD6C1(a1: v106);`
  - `sub_409E70(a1: 4 * v3 + 12501964);`
  - `LOBYTE(v120) = 3;`
  - …（截断 16 行）

### CVecCtrlSummoned（6 函数）


#### CVecCtrlSummoned::CollisionDetectFloat  (2915 B)

  - `signed int __thiscall CVecCtrlSummoned::CollisionDetectFloat(CVecCtrlSummoned *this, __int64 a2, int a3)`
  - `v4 = *(this + 150);`
  - `if ( v4 != 3 && v4 != 4 )`
  - `return CVecCtrl::CollisionDetectFloat(this, a2, a3: HIDWORD(a2), a4: a3);`
  - `v7 = _ZtlSecureFuse<double>(a1: this + 32, a2: *(this + 12));`
  - `if ( v7 >= v20 )`
  - `v9 = _ZtlSecureFuse<double>(a1: this + 32, a2: *(this + 12));`
  - `v21 = (v5[11] - 1);`
  - `if ( v9 <= v21 )`
  - `*(this + 12) = _ZtlSecureTear<double>(a1: v16, a2: HIDWORD(v8));`
  - `v10 = _ZtlSecureFuse<double>(a1: this + 56, a2: *(this + 18));`
  - `if ( v10 < v22 )`
  - `*(this + 18) = _ZtlSecureTear<double>(a1: v17, a2: HIDWORD(v11));`
  - `v12 = _ZtlSecureFuse<double>(a1: this + 56, a2: *(this + 18));`
  - `v23 = (v5[12] - 1);`
  - `if ( v12 > v23 )`
  - …（截断 16 行）

#### CVecCtrlSummoned::CreateInstance  (503 B)

  - `CVecCtrl *CVecCtrlSummoned::CreateInstance()`
  - `v0 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 648);`
  - `if ( v0 != 0 )`
  - `CVecCtrl::CVecCtrl(this: v0);`
  - `*(v1 + 3) = &off_B3E8F8;`
  - `if ( v1 == 0 )`
  - `_com_raise_error(a1: -2147024882, a2: 0);`

#### CVecCtrlSummoned::CtrlUpdateActiveFly  (5893 B)

  - `unsigned int __thiscall CVecCtrlSummoned::CtrlUpdateActiveFly(CVecCtrlSummoned *this)`
  - `if ( *(this + 150) == 4 )`
  - `if ( *(this + 156) != 0 )`
  - `v22 = *(this + 12);`
  - `v3 = *(this + 154) - _ZtlSecureFuse<double>(a1: this + 32, a2: v22);`
  - `if ( abs(a1: v3) >= 10 )`
  - `if ( *(this + 154) >= _ZtlSecureFuse<double>(a1: this + 32, a2: *(this + 12)) )`
  - `v4 = *(this + 155) - _ZtlSecureFuse<double>(a1: this + 56, a2: *(this + 18));`
  - `if ( abs(a1: v4) >= 10 )`
  - `if ( *(this + 155) < _ZtlSecureFuse<double>(a1: this + 56, a2: *(this + 18)) )`
  - `CVecCtrl::Jump(this);`
  - `--*(this + 158);`
  - `CVecCtrl::SetInput(this, a2: v34, a3: v23);`
  - `v5 = _ZtlSecureFuse<double>(a1: this + 32, a2: *(this + 12));`
  - `v18 = *(this + 18);`
  - `v6 = _ZtlSecureFuse<double>(a1: this + 56, a2: v18);`
  - …（截断 16 行）

#### CVecCtrlSummoned::Init  (919 B)

  - `int *__thiscall CVecCtrlSummoned::Init(`
  - `CVecCtrl::Init(this, a2: pOwner);`
  - `*(this + 146) = dwSummonedID;`
  - `*(this + 147) = nSkillID;`
  - `*(this + 150) = nMoveAbility;`
  - `*(this + 151) = 0;`
  - `*(this + 152) = 0;`
  - `*(this + 153) = 0;`
  - `if ( nMoveAbility == 1 )`
  - `if ( nSkillID == dword_E4E5AC`
  - `|| nSkillID == (&loc_A7DCA9 + 3)`
  - `|| nSkillID == (sub_C6612A + 2)`
  - `*(this + 153) = 42;`
  - `*(this + 153) = 70;`

#### CVecCtrlSummoned::SetActive  (485 B)

  - `void *__thiscall CVecCtrlSummoned::SetActive(`
  - `CVecCtrl::SetActive(this, a2, a3, a4, a5, a6, a7, a8);`
  - `*(this + 159) = a3;`
  - `*(this + 160) = a4;`
  - `*(this + 149) = 0;`
  - `*(this + 148) = 0;`
  - `return memset(a1: this + 616, a2: 0, a3: 0x14u);`

#### CVecCtrlSummoned::WorkUpdateActive  (10627 B)

  - `signed int __thiscall CVecCtrlSummoned::WorkUpdateActive(CVecCtrlSummoned *this, int tElapse)`
  - `if ( *(this + 150) == 0 )`
  - `CAvatar::GetFaceOrigin(this: (TSingleton<CUserLocal>::ms_pInstance + 136), a2: &v65);`
  - `v5 = *(this + 5);`
  - `if ( v5 != 0 && (*(*v5 + 32))(a1: v5, a2: &dword_BF0D30) != 0 && (v6 = *(this + 5)) != 0 )`
  - `if ( *(this + 150) != 4 )`
  - `v57 = *(this + 18);`
  - `v63 = _ZtlSecureFuse<double>(a1: this + 56, a2: v57);`
  - `v7 = _ZtlSecureFuse<double>(a1: this + 32, a2: *(this + 12));`
  - `dword_BF0454(a1: v62, a2: v7, a3: v63);`
  - `v9 = (*(*(v4 + 4) + 16))(a1: v4 + 4, a2: v44, a3: v46, a4: v58);`
  - `if ( dword_BF0484(a1: v62, a2: *v9, a3: v9[1]) == 0 )`
  - `if ( *(this + 150) == 1 )`
  - `if ( v65 == 0 )`
  - `v67 = sub_425B16(a1: v65);`
  - `v10 = (*(*v8 + 16))(a1: v4 + 4);`
  - …（截断 16 行）

### CDragon（10 函数）


#### CDragon::CreateEffect  (4927 B)

- SP_ 节点：`SP_2870_EFFECT_BASICEFFIMG_SUMMONED`, `SP_5508_EFFECT_BASICEFFIMG_DRAGONEYES`
  - `void __fastcall CDragon::CreateEffect(CDragon *this, int a2, int a3)`
  - `if ( a3 == 0 )`
  - `LOBYTE(v38) = 1;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `LOBYTE(v38) = 0;`
  - `if ( v34 != 0 )`
  - `if ( a3 != 1 )`
  - `sub_432C10(a1: 0);`
  - `CAnimationDisplayer::LoadLayer(a1: v3 + 220);`
  - `v23 = sub_41468C(a1: &v37);`
  - `v21 = sub_432CBC(a1: v20, a2: v19);`
  - `CAnimationDisplayer::Effect_General(this: v21, a2: v23, a3: 0, a4: v25, a5: v26, a6: v27, a7: v28, a8: v29);`
  - `sub_4145AB(a1: this + 136);`
  - `LOBYTE(v38) = 3;`
  - `LOBYTE(v38) = 5;`
  - `v6 = sub_4281FB(a1: v3 + 136);`
  - …（截断 16 行）

#### CDragon::GetCurrentAction  (658 B)

  - `int __thiscall CDragon::GetCurrentAction(CDragon *this, int *a2)`
  - `v3 = _ZtlSecureFuse<long>(a1: this + 140, a2: *(this + 37));`
  - `if ( a2 != 0 )`
  - `if ( v3 >> 1 != 1 )`
  - `v8 = *(this + 40);`
  - `v5 = (this + 160);`
  - `if ( _ZtlSecureFuse<long>(a1: v6, a2: v8) <= -1 )`
  - `return _ZtlSecureFuse<long>(a1: v6, a2: *v5);`

#### CDragon::NeedToTalk  (2412 B)

  - `int __thiscall CDragon::NeedToTalk(CDragon *this, int a2, int a3)`
  - `v4 = (this + 248);`
  - `if ( (*(**(this + 62) + 12))(a1: *(this + 62)) != 0 )`
  - `if ( v5 != 0 )`
  - `CUserLocal::GetDCRect(this: v5, a2: &v14);`
  - `if ( dword_BF04AC(a1: &v14) == 0 && dword_BF0484(a1: &v14, a2, a3) != 0 )`
  - `CAvatar::GetBodyRect(this: v5 + 34, a2: &v14, a3: 1);`
  - `sub_4FF6D2(this, a2: &v14, a3: 0);`
  - `if ( *(this + 66) != 0 )`
  - `v7 = sub_425AEF(a1: *(this + 66));`
  - `v8 = *(this + 66) == 0;`
  - `v14.left = *(this + 58) + v7;`
  - `if ( v8 )`
  - `v9 = sub_425B16(a1: *(this + 66));`
  - `v14.top = *(this + 59) + v9;`
  - `Width = IWzGr2DLayer::GetWidth(a1: *(this + 66));`
  - …（截断 16 行）

#### CDragon::PrepareActionLayer  (5213 B)

  - `int __thiscall CDragon::PrepareActionLayer(CDragon *this)`
  - `CurrentAction = CDragon::GetCurrentAction(this, a2: &v36);`
  - `v3 = (*(**(this + 62) + 64))(a1: *(this + 62));`
  - `v4 = 4 * sub_4FEEC5(a1: v3);`
  - `v5 = *(v4 + *(this + 68));`
  - `if ( v5 == 0 || *(v5 - 4) == 0 )`
  - `sub_500F93(a1: 21, a2: &v39);`
  - `v6 = *(v4 + *(this + 68));`
  - `v7 = *(v6 + 20 * CurrentAction + 8) == 0;`
  - `if ( v7 )`
  - `v9 = *(this + 62);`
  - `v10 = (*(*v9 + 64))(a1: v9);`
  - `sub_41127F(a1: v10, a2: CurrentAction, a3: v28);`
  - `if ( *(v8 + 8) == 0 )`
  - `_CxxThrowException(a1: &v38, a2: &_TI3_AVCTerminateException__);`
  - `sub_402FAB(a1: -2, a2: 3);`
  - …（截断 16 行）

#### CDragon::ProcessAction  (4369 B)

  - `int __thiscall CDragon::ProcessAction(CDragon *this, int a2)`
  - `v3 = (*(**(this + 62) + 64))(a1: *(this + 62));`
  - `result = sub_4FEEC5(a1: v3);`
  - `if ( result < 0 )`
  - `result = CDragon::GetCurrentAction(this, a2: 0);`
  - `*(this + 42) -= 30;`
  - `if ( *(this + 42) > 0 )`
  - `v6 = *(*(this + 68) + 4 * v5) + 20 * result;`
  - `v7 = *(*(this + 41) != 0 ? *(this + 41) - 16 + 4 : 4);`
  - `*(this + 41) = v8;`
  - `if ( v8 == 0 && result >= 7 && result <= 8 )`
  - `*(this + 41) = *(v6 + 12);`
  - `if ( _ZtlSecureFuse<long>(a1: this + 152, a2: *(this + 40)) <= -1 )`
  - `if ( *(this + 41) == 0 )`
  - `else if ( *(this + 41) == 0 )`
  - `*(this + 40) = _ZtlSecureTear<long>(a1: -1, a2: this + 152);`
  - …（截断 16 行）

#### CDragon::SetFrameInfo  (534 B)

  - `int __thiscall CDragon::SetFrameInfo(_DWORD *this, int a2, int a3)`
  - `v3 = *(a3 + 4);`
  - `if ( _ZtlSecureFuse<long>(a1: this + 38, a2: *(this + 40)) <= -1 )`
  - `v5 = *(v3 + 64);`
  - `v5 = *(v3 + 64) * (*(this + 43) + 10) / 16;`
  - `*(this + 42) = v5;`
  - `sub_4144D4(a1: this + 44, a2: v3 + 16);`
  - `return dword_BF040C(a1: this + 48, a2: -*(this + 46), a3: *(this + 45), a4: -*(this + 44), a5: *(this + 47));`

#### CDragon::SetMoveAction  (700 B)

  - `int __thiscall CDragon::SetMoveAction(CDragon *this, int a2, int a3)`
  - `if ( a3 != 0 || (result = _ZtlSecureFuse<long>(a1: this + 140, a2: *(this + 37)), a2 != result) )`
  - `v5 = _ZtlSecureTear<long>(a1: a2, a2: this + 140);`
  - `v6 = *(this + 40);`
  - `*(this + 37) = v5;`
  - `if ( _ZtlSecureFuse<long>(a1: this + 152, a2: v6) <= -1 )`
  - `CDragon::PrepareActionLayer(this);`
  - `result = _ZtlSecureFuse<long>(a1: this + 140, a2: *(this + 37));`
  - `*(*(this + 55) != 0 ? *(this + 55) - 12 + 0x148 : 328) = result;`

#### CDragon::SetOneTimeAction  (366 B)

  - `int __thiscall CDragon::SetOneTimeAction(CDragon *this, int a2)`
  - `v4 = (this + 160);`
  - `*(this + 40) = _ZtlSecureTear<long>(a1: -1, a2: this + 152);`
  - `*v4 = _ZtlSecureTear<long>(a1: a2, a2: v3);`
  - `return CDragon::PrepareActionLayer(this);`

#### CDragon::Update  (5927 B)

  - `int __fastcall CDragon::Update(int a1, int a2)`
  - `update_time = get_update_time();`
  - `v4 = *(a1 + 220);`
  - `v5 = v4 != 0 ? (v4 - 12) : 0;`
  - `if ( *(v5 + 6) != 0 )`
  - `CVecCtrl::UpdateActive(this: v5);`
  - `updated = CVecCtrl::UpdatePassive(this: v5);`
  - `if ( updated > 0 && (updated & 0x10) != 0 )`
  - `CDragon::CreateEffect(a1: 0);`
  - `CDragon::SetMoveAction(this: a1, a2: *(v5 + 82), a3: 0);`
  - `CDragon::ProcessAction(a1: v28);`
  - `CDragon::UpdateQuestInfo(a1);`
  - `v7 = (a1 + 268);`
  - `if ( *(a1 + 268) != 0 )`
  - `v8 = *(a1 + 136) == 0;`
  - `if ( v8 )`
  - …（截断 16 行）

#### CDragon::UpdateQuestInfo  (8062 B)

- SP_ 节点：`SP_3190_UI_UIWINDOWIMG_QUESTICON_D`
  - `int __thiscall CDragon::UpdateQuestInfo(CDragon *this)`
  - `result = (*(**(this + 62) + 12))(a1: *(this + 62));`
  - `if ( result != 0 )`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v35) + 4);`
  - `sub_4289B7(this: &v35);`
  - `v38 = *(*dword_BE7918 + 14272);`
  - `CurFieldID = CWvsContext::GetCurFieldID(a1: *dword_BE7918);`
  - `for ( i = 0; ; ++i )`
  - `v4 = *(this + 63);`
  - `if ( v4 == 0 || i >= *(v4 - 4) )`
  - `LOWORD(v4) = *(v4 + 2 * i);`
  - `if ( v4 < 0x4B0u || v4 >= 0x578u )`
  - `if ( sub_500EA5(a1: &v36) != 0 )`
  - `if ( CQuestMan::CheckCompleteDemand(this: v41, a2: v44, i: 0xF7508u, a4: v3, a5: v42) == 0 )`
  - `*(this + 64) = 2;`
  - `sub_500E4C(a1: &v44, a2: 0);`
  - …（截断 16 行）

### CVecCtrlDragon（2 函数）


#### CVecCtrlDragon::WorkUpdateActive  (9111 B)

  - `signed int __thiscall CVecCtrlDragon::WorkUpdateActive(CVecCtrl *this, int a2)`
  - `User = CUserPool::GetUser(this: dword_BEBFA8, a2: *(this + 147));`
  - `v4 = *(this + 5);`
  - `if ( v4 != 0 )`
  - `(*(*v4 + 32))(a1: v4, a2: &dword_BED67C);`
  - `v56 = *(this + 18);`
  - `v62 = _ZtlSecureFuse<double>(a1: this + 56, a2: v56);`
  - `v6 = _ZtlSecureFuse<double>(a1: this + 32, a2: *(this + 12));`
  - `dword_BF0454(a1: v61, a2: v6, a3: v62);`
  - `v7 = *(v5 + 4);`
  - `v9 = (*(v7 + 16))(a1: v8, a2: v47, a3: v49, a4: v57);`
  - `if ( dword_BF0484(a1: v61, a2: *v9, a3: v9[1]) == 0 )`
  - `v65 = *((*(*v8 + 16))(a1: v8) + 4);`
  - `v10 = (*(*v8 + 16))(a1: v8);`
  - `v11 = (*(*(this + 3) + 64))(a1: this + 12, a2: *v10, a3: v65);`
  - `if ( v11 < 0 )`
  - …（截断 16 行）

#### CVecCtrlDragon::WorkUpdatePassive  (1380 B)

  - `signed int __thiscall CVecCtrlDragon::WorkUpdatePassive(CVecCtrl *this, int a2)`
  - `updated = CVecCtrl::WorkUpdatePassive(this, a2, a3: v10, a4: v11);`
  - `v12 = _ZtlSecureFuse<double>(a1: this + 56, a2: *(this + 18));`
  - `v5 = _ZtlSecureFuse<double>(a1: this + 32, a2: *(this + 12));`
  - `FootholdUnderneath = CWvsPhysicalSpace2D::GetFootholdUnderneath(`
  - `if ( FootholdUnderneath != 0 )`
  - `if ( *(this + 76) != v7 || *(this + 77) != FootholdUnderneath[8] )`
  - `*(this + 76) = v7;`
  - `v8 = *(this + 5);`
  - `*(this + 77) = FootholdUnderneath[8];`
  - `(*(*v8 + 8))(a1: v8, a2: this);`

### CMob（51 函数）


#### CMob::ClearActionLayer  (229 B)

  - `int __thiscall CMob::ClearActionLayer(CMob *this)`
  - `for ( i = 0; i < 800; i += 20 )`
  - `result = sub_416B72(a1: i + *(this + 269));`

#### CMob::CreateHPIndicator  (5781 B)

  - `int __thiscall CMob::CreateHPIndicator(CMob *this, int a2, int a3)`
  - `v4 = (this + 1224);`
  - `v5 = *(this + 306) == 0;`
  - `if ( !v5 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `LOBYTE(v32) = 1;`
  - `if ( dword_BF14EC == 0 )`
  - `v6 = IWzGr2D::CreateLayer(`
  - `sub_428712(a1: *v6);`
  - `sub_4156C5(a1: &v30);`
  - `LOBYTE(v32) = 0;`
  - `if ( v7 < 0 )`
  - `if ( v8 < 0 )`
  - `sub_410FDF(this: &pvargDest, a2: *(v3 + 304), a3: 1);`
  - `if ( v5 )`
  - …（截断 16 行）

#### CMob::DoAttack  (13193 B)

  - `void __fastcall CMob::DoAttack(_DWORD *a1, int a2, int a3, int a4, unsigned int a5)`
  - `int (*v18)(); // eax`
  - `HIDWORD(v78) = 0;`
  - `if ( !v6 )`
  - `if ( a1[141] != 0 )`
  - `if ( a1[144] != 0 )`
  - `if ( a1[172] != 0 )`
  - `if ( a1[151] != 0 )`
  - `update_time = get_update_time();`
  - `AttackInfo = CMobTemplate::GetAttackInfo(a1: a3 - 12);`
  - `if ( AttackInfo == 0 )`
  - `if ( v9 != 0 && (v10 = *v9) != 0 )`
  - `v11 = *(v10 - 4) >> 1;`
  - `if ( v11 != 0 )`
  - `if ( v12 != 0 )`
  - `InterlockedIncrement(lpAddend: (v12 + 8));`
  - …（截断 16 行）

#### CMob::GenerateMovePath  (10946 B)

- 分支 case：`2`, `3`, `4`, `5`, `6`, `7`, `8`, `9`
  - `int __thiscall CMob::GenerateMovePath(`
  - `update_time = get_update_time();`
  - `v12 = *(this + 71);`
  - `v60 = *(this + 70) != 0 ? (*(this + 70) - 12) : 0;`
  - `v13 = v12 != 0 ? (v12 - 12) : 0;`
  - `v14 = *(this + 169) == 0;`
  - `if ( v14 || (v15 = *(this + 99)) == 0 )`
  - `v15 = *(this + 98);`
  - `v61 = _ZtlSecureFuse<long>(a1: v15 + 64, a2: *(v15 + 72)) == 3;`
  - `if ( a2 < 0 )`
  - `ActionDelay = CMob::GetActionDelay(this, a2);`
  - `if ( a2 >= 0 )`
  - `v16 = _ZtlSecureFuse<long>(a1: this + 960, a2: *(this + 242));`
  - `if ( (v16 < 12 || v16 > 20) && ((v17 = _ZtlSecureFuse<long>(a1: this + 960, a2: *(this + 242))) < 21 || v17 > 37)`
  - `|| (v18 = a2, a2 < 6)`
  - `v19 = _ZtlSecureTear<long>(a1: a2, a2: this + 960);`
  - …（截断 16 行）

#### CMob::GetActionDelay  (925 B)

  - `int __thiscall CMob::GetActionDelay(CMob *this, unsigned int a2)`
  - `if ( a2 >= 0x28 )`
  - `v2 = *(this + 269) + 20 * a2;`
  - `if ( *(v2 + 8) == 0 )`
  - `if ( *(this + 169) == 0 || (v3 = *(this + 99)) == 0 )`
  - `v3 = *(this + 98);`
  - `v4 = _ZtlSecureFuse<unsigned long>(a1: v3 + 12, a2: *(v3 + 20));`
  - `CActionMan::LoadMobAction(a1: v4, a2, a3: v2);`
  - `v5 = *(v2 + 12);`
  - `while ( v5 != 0 )`
  - `v7 = *(v5 + 4);`
  - `v8 = *(v5 != 0 ? v5 - 16 + 4 : 4);`
  - `result += *(v7 + 76);`

#### CMob::GetCurrentAction  (715 B)

  - `int __thiscall CMob::GetCurrentAction(CMob *this, int *a2)`
  - `v3 = _ZtlSecureFuse<long>(a1: this + 948, a2: *(this + 239));`
  - `v4 = CMob::MoveAction2RawAction(a1: v3, a2);`
  - `FineAction = CMob::GetFineAction(this, nAction: v4);`
  - `v10 = *(this + 242);`
  - `v6 = (this + 968);`
  - `if ( _ZtlSecureFuse<long>(a1: v7, a2: v10) <= -1 )`
  - `return _ZtlSecureFuse<long>(a1: v7, a2: *v6);`

#### CMob::GetFineAction  (1926 B)

  - `int __thiscall CMob::GetFineAction(CMob *this, int nAction)`
  - `if ( (byte_BEDB28 & 1) == 0 )`
  - `atexit(a1: CMob::GetFineAction_::_2_::_dynamic_atexit_destructor_for__sMobActionErrata__);`
  - `if ( *(this + 169) == 0 || (v3 = *(this + 99)) == 0 )`
  - `v3 = *(this + 98);`
  - `v4 = _ZtlSecureFuse<unsigned long>(a1: v3 + 12, a2: *(v3 + 20));`
  - `if ( !CMob::LoadMobAction(this, a2: nAction) )`
  - `ZMap<long,ZMap<long,long,long>,long>::Insert(this: &dword_BEDB10, a2: &v7, a3: 0);`
  - `if ( ZMap<long,long,long>::GetAt(a1: &nAction, a2: v8) != 0 )`
  - `CMob::LoadMobAction(this, a2: v8[0]);`
  - `if ( CMob::LoadMobAction(this, a2: 1) )`
  - `if ( !CMob::LoadMobAction(this, a2: 3) )`
  - `_CxxThrowException(a1: &v6, a2: &_TI3_AVCTerminateException__);`
  - `ZMap<long,ZMap<long,long,long>,long>::Insert(this: &dword_BEDB10, a2: &v6, a3: 0);`
  - `ZMap<long,long,long>::Insert(a1: &nAction, a2: v8);`

#### CMob::GetFineMoveDirAction  (1169 B)

  - `int __thiscall CMob::GetFineMoveDirAction(CMob *this, int nMoveDirAction)`
  - `v3 = CMob::MoveAction2RawAction(this, a2: nMoveDirAction, a3: &nMoveDirAction);`
  - `FineAction = CMob::GetFineAction(this, nAction: v3);`
  - `if ( FineAction == 0 )`
  - `if ( v5 == 0 )`
  - `if ( v6 == 0 )`
  - `if ( v7 == 0 )`
  - `if ( v7 == 35 )`
  - `return nMoveDirAction & 1 | (2 * v8);`
  - `v8 = _ZtlSecureFuse<long>(a1: *(this + 98) + 64, a2: *(*(this + 98) + 72)) != 3 ? 2 : 6;`

#### CMob::GetHeight  (1560 B)

  - `int __thiscall CMob::GetHeight(CMob *this)`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `v2 = *(this + 304) == 0;`
  - `LOBYTE(v11) = 1;`
  - `if ( v2 )`
  - `v3 = *IWzGr2DLayer::GetCanvas(a1: &v8, a2: &pvarg);`
  - `LOBYTE(v11) = 2;`
  - `if ( v10 != v3 )`
  - `sub_4156B8(this: &v10);`
  - `if ( v4 != 0 )`
  - `(*(*v4 + 8))(a1: v4);`
  - `if ( v8 != 0 )`
  - `(*(*v8 + 8))(a1: v8);`
  - `LOBYTE(v11) = 0;`
  - `if ( v5 < 0 )`
  - `if ( v10 == 0 )`
  - …（截断 16 行）

#### CMob::GetHitPoint  (1050 B)

  - `LONG __thiscall CMob::GetHitPoint(CMob *this, struct tagRECT a2, int a3)`
  - `if ( sub_664559(this, a2: &v6, a3: 0) != 0 )`
  - `if ( a2.top < v8 )`
  - `if ( a2.bottom <= v6 )`
  - `if ( a2.right < v9 )`
  - `if ( a3 <= v7 )`
  - `dword_BF04A8(a1: v5, a2: &a2.top, a3: &v6);`
  - `v4 = (v5[3] + v5[1]) / 2;`
  - `*a2.left = (v5[2] + v5[0]) / 2;`
  - `*(a2.left + 4) = v4;`
  - `*(a2.left + 4) = 0;`

#### CMob::GetPos  (80 B)

  - `char *__thiscall CMob::GetPos(char *this)`

#### CMob::GetPosPrev  (84 B)

  - `char *__thiscall CMob::GetPosPrev(char *this)`

#### CMob::GetRTTI  (61 B)

  - `int *CMob::GetRTTI()`

#### CMob::GetRemainDamageInfoDelay  (758 B)

  - `int __thiscall CMob::GetRemainDamageInfoDelay(CMob *this)`
  - `if ( *(this + 291) == 0 )`
  - `update_time = get_update_time();`
  - `v4 = *(this + 292);`
  - `while ( v4 != 0 )`
  - `v10 = *(v4 + 8);`
  - `v7 = *(v4 != 0 ? v4 - 16 + 4 : 4);`
  - `v8 = _ZtlSecureFuse<long>(a1: v9, a2: v10) - v5;`
  - `if ( v8 >= v6 )`

#### CMob::GetType  (55 B)

  - `signed int CMob::GetType()`

#### CMob::GetZMass  (112 B)

  - `int __thiscall CMob::GetZMass(_DWORD *this)`
  - `return *(*(this + 69) != 0 ? *(this + 69) - 12 + 0x134 : 308);`

#### CMob::Init  (24710 B)

- SP_ 节点：`SP_1590_EFFECT_SUMMONIMG_D`, `SP_958_SHAPE2DVECTOR2D`
  - `int __thiscall CMob::Init(CMob *this, _DWORD *a2, struct CInPacket *a3)`
  - `update_time = get_update_time();`
  - `*(this + 97) = sub_4E81A0(a1: a2, a2: this + 380);`
  - `CMob::SetShoeAttr(this);`
  - `*(this + 326) = CInPacket::Decode2(this: a3);`
  - `*(this + 327) = CInPacket::Decode2(this: v4);`
  - `*(this + 324) = *(this + 326);`
  - `*(this + 325) = *(this + 327);`
  - `v5 = CInPacket::Decode1(this: v4);`
  - `*(this + 239) = _ZtlSecureTear<long>(a1: v5, a2: this + 948);`
  - `LOWORD(v6) = CInPacket::Decode2(this: v4);`
  - `LOWORD(v7) = CInPacket::Decode2(this: v4);`
  - `v8 = CInPacket::Decode1(this: v4);`
  - `if ( v8 == -3 || v8 >= 0 )`
  - `v118 = CInPacket::Decode4(this: v4);`
  - `*(this + 236) = CInPacket::Decode1(this: v4);`
  - …（截断 16 行）

#### CMob::IsActive  (108 B)

  - `int __thiscall CMob::IsActive(CMob *this)`
  - `return *(*(this + 71) != 0 ? *(this + 71) - 12 + 0x18 : 24);`

#### CMob::IsKindOf  (325 B)

  - `signed int __stdcall CMob::IsKindOf(int *a1)`
  - `if ( &dword_BEDB54 == 0 )`
  - `while ( v1 != a1 )`
  - `if ( v1 == 0 )`

#### CMob::IsTargetInAttackRange  (14710 B)

- 分支 case：`0`, `1`, `2`, `3`
  - `signed int __thiscall CMob::IsTargetInAttackRange(CMob *this, int a2, int *a3, int *a4, struct CMob::TARGETINFO *a5)`
  - `v6 = *(this + 169) == 0;`
  - `if ( v6 || (v7 = *(this + 99)) == 0 )`
  - `v7 = *(this + 98);`
  - `if ( *(v7 + 536) <= 0`
  - `|| sub_667569(a1: this) != 0`
  - `|| timeGetTime() != 0`
  - `|| *(v5 + 141) != 0`
  - `|| *(v5 + 144) != 0`
  - `|| *(v5 + 172) != 0`
  - `|| *(v5 + 151) != 0`
  - `|| *(v5 + 227) != 0 )`
  - `memset(v68, 0, sizeof(v68));`
  - `v8 = sub_663FE2(a1: v5);`
  - `switch ( a2 )`
  - `CAvatar::GetBodyRect(this: (TSingleton<CUserLocal>::ms_pInstance + 136), a2: &v91, a3: 0);`
  - …（截断 16 行）

#### CMob::LoadMobAction  (531 B)

  - `BOOL __thiscall CMob::LoadMobAction(CMob *this, int a2)`
  - `if ( *(20 * a2 + *(this + 269) + 8) == 0 )`
  - `if ( *(this + 169) == 0 || (v4 = *(this + 99)) == 0 )`
  - `v4 = *(this + 98);`
  - `v5 = _ZtlSecureFuse<unsigned long>(a1: v4 + 12, a2: *(v4 + 20));`
  - `CActionMan::LoadMobAction(a1: v5, a2, a3: *(this + 269) + v3);`
  - `return *(v3 + *(this + 269) + 8) != 0;`

#### CMob::MoveAction2RawAction  (576 B)

- 分支 case：`1`, `2`, `3`, `6`
  - `int __thiscall CMob::MoveAction2RawAction(CMob *this, int a2, int *a3)`
  - `if ( a3 != 0 )`
  - `switch ( a2 >> 1 )`
  - `if ( a2 >> 1 != 16 )`
  - `return _ZtlSecureFuse<long>(a1: *(this + 98) + 64, a2: *(*(this + 98) + 72)) != 3 ? 1 : 3;`

#### CMob::OnCtrlAck  (1085 B)

  - `int __thiscall CMob::OnCtrlAck(CMob *this, struct CInPacket *a2)`
  - `if ( CMob::IsActive(this) == 0 )`
  - `CMob::SetActive(this, a2: 1);`
  - `v10 = CInPacket::Decode2(this: a2);`
  - `v4 = CInPacket::Decode1(this: a2);`
  - `v5 = CInPacket::Decode2(this: a2);`
  - `*(this + 102) = _ZtlSecureTear<long>(a1: v5, a2: this + 400);`
  - `v6 = CInPacket::Decode1(this: a2);`
  - `v7 = CInPacket::Decode1(this: a2);`
  - `if ( *(this + 81) == 0 || v6 != 0 )`
  - `*(this + 81) = v6;`
  - `*(this + 82) = v7;`
  - `if ( v10 - _ZtlSecureFuse<short>(a1: this + 316, a2: *(this + 80)) >= 0 )`
  - `v8 = (v4 != 0) + 3;`
  - `result = _ZtlSecureTear<long>(a1: v8, a2: this + 304);`
  - `*(this + 78) = result;`
  - …（截断 16 行）

#### CMob::OnDamaged  (807 B)

  - `_DWORD *__thiscall CMob::OnDamaged(CMob *this, struct CInPacket *a2)`
  - `CInPacket::Decode1(this: a2);`
  - `v3 = CInPacket::Decode4(this: a2);`
  - `CMob::ShowDamage(this, nDamage: v3, a3: 0, a4: 0, bHalfHeight: 0, a6: v7, a7: v8);`
  - `result = *(this + 98);`
  - `if ( result[130] != 0 )`
  - `CMob::ShowHitEffect(this, a2: v3);`
  - `v5 = CInPacket::Decode4(this: a2);`
  - `v6 = CInPacket::Decode4(this: a2);`
  - `CMob::CreateHPIndicator(this, a2: 100 * v5 / v6, a3: -65536);`
  - `return CMob::ShowHPIndicator(this);`

#### CMob::OnDie  (8301 B)

- SP_ 节点：`SP_958_SHAPE2DVECTOR2D`
  - `char *__thiscall CMob::OnDie(CMob *this)`
  - `if ( *(this + 169) == 0 || (v2 = *(this + 99)) == 0 )`
  - `v2 = *(this + 98);`
  - `*v39 = _ZtlSecureFuse<unsigned long>(a1: v2 + 12, a2: *(v2 + 20));`
  - `v31 = *((*(*(this + 1) + 16))(a1: this + 4) + 4);`
  - `v3 = (*(*(this + 1) + 16))(a1: this + 4);`
  - `v4 = sub_9894F3(a1: *v3, a2: v31);`
  - `sub_989BA1(a1: v39[0], a2: 1, a3: v4);`
  - `if ( *(this + 169) == 0 || (v5 = *(this + 99)) == 0 )`
  - `v5 = *(this + 98);`
  - `v6 = *(v5 + 548);`
  - `if ( v6 > 0 )`
  - `if ( _ZtlSecureFuse<long>(a1: this + 1092, a2: *(this + 275)) == 3 )`
  - `v8 = rand() % v6 + 9;`
  - `*(this + 242) = _ZtlSecureTear<long>(a1: v8, a2: v7);`
  - `CMob::PrepareActionLayer(this);`
  - …（截断 16 行）

#### CMob::OnDoomed  (2453 B)

  - `void __thiscall CMob::OnDoomed(CMob *this, int a2)`
  - `if ( a2 == 0 )`
  - `if ( *(this + 99) == 0 )`
  - `*(this + 99) = 0;`
  - `if ( *(this + 330) != 0 )`
  - `CMovePath::SetStatChangedPoint(this: (*(this + 71) != 0 ? *(this + 71) - 12 + 428 : 428), a2: *(this + 1324));`
  - `*(this + 330) = 0;`
  - `MobTemplate = CMobTemplate::GetMobTemplate(a1: 5);`
  - `MobStat::SetFromWhenDoom(this: (this + 416), a2: MobTemplate);`
  - `*(this + 99) = CMobTemplate::GetMobTemplate(a1: 5);`
  - `if ( *(this + 304) == 0 )`
  - `v4 = *(this + 304);`
  - `v5 = (*(*v4 + 224))(a1: v4, a2: 0xFFFFFF);`
  - `if ( v5 < 0 )`
  - `*(this + 85) = get_update_time() + 1200;`
  - `v6 = *(this + 70) != 0 ? (*(this + 70) - 12) : 0;`
  - …（截断 16 行）

#### CMob::OnEffectByItem  (289 B)

  - `void **__thiscall CMob::OnEffectByItem(_DWORD *this, int a2)`
  - `v3 = CInPacket::Decode4(this: a2);`
  - `v4 = CInPacket::Decode1(this: a2);`
  - `return CMob::ShowEffectByItem(this, nItemID: v3, bSuccess: v4);`

#### CMob::OnHit  (9298 B)

- SP_ 节点：`SP_1591_SOUND_SKILLIMG_07D_HIT`, `SP_3306_SOUND_CASHEFFECTIMG_08D_HIT`
  - `void __userpurge CMob::OnHit(`
  - `v16 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &pvargDest.lVal) + 4);`
  - `sub_4289B7(this: &pvargDest.lVal);`
  - `v18 = (*(*v15 + 64))(a1: v15);`
  - `SkillLevel = CSkillInfo::GetSkillLevel(this: v17, a2: v16, a3: v18 != 2000 ? 21000000 : 20000017, a4: &v62);`
  - `weapon_type = get_weapon_type(nItemID: v15[315]);`
  - `if ( a2 == v15[1130] && SkillLevel > 0 && weapon_type == 44 && a6 > 0 )`
  - `sub_9602F3(a1: v15);`
  - `v63 = ((a10 == 10 || *(v63 + 205) != 0 && *(*(v63 + 70) != 0 ? *(v63 + 70) - 12 + 0x110 : 272) == 0)`
  - `&& CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v16, a3: 21110003, a4: 0) != 0`
  - `&& sub_66918F(a1: v21, a2: v22) == 0`
  - `&& (sub_6691A4(a1: v21) == 1 || sub_6691A4(a1: v21) == 2)`
  - `&& *(get_field() + 344) == 0);`
  - `if ( sub_6691B9(a1: v21) != 0 )`
  - `if ( sub_76662D(a1: a3) != 0 )`
  - `CMob::ShowHitEffect(this: v21, a2: a6);`
  - …（截断 16 行）

#### CMob::OnMobSpeaking  (264 B)

  - `void __thiscall CMob::OnMobSpeaking(CMob *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode4(this: a2);`
  - `v4 = CInPacket::Decode4(this: a2);`
  - `CMob::TrySpeaking(this, a2: v3, a3: v4);`

#### CMob::OnMove  (6570 B)

  - `int __thiscall CMob::OnMove(CMob *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `v4 = CInPacket::Decode1(this: a2);`
  - `v5 = CInPacket::Decode1(this: a2);`
  - `v35 = *(this + 78);`
  - `if ( _ZtlSecureFuse<long>(a1: this + 304, a2: v35) != 1 || v4 != 0 || (result = v45 >> 1) >= 0xC && v45 >> 1 <= 0x14u )`
  - `update_time = get_update_time();`
  - `v8 = *(this + 70);`
  - `if ( CMob::IsActive(this) != 0 )`
  - `v10 = *(this + 71) != 0 ? *(this + 71) - 12 : 0;`
  - `(*(*v10 + 4))(a1: v10, a2: 0, a3: 0, a4: 0, a5: 0, a6: 0, a7: 0, a8: 0);`
  - `if ( *(this + 169) == 0 || (v11 = *(this + 99)) == 0 )`
  - `v11 = *(this + 98);`
  - `v12 = _ZtlSecureFuse<long>(a1: v11 + 64, a2: *(v11 + 72));`
  - `CMovePath::DiscardByInterrupt(this: (v9 + 428), a2: v12, a3: v9, a4: v3);`
  - `v42 = CInPacket::Decode4(this: a2);`
  - …（截断 16 行）

#### CMob::OnRevive  (2111 B)

  - `int __thiscall CMob::OnRevive(int *this)`
  - `*(this + 278) = _ZtlSecureTear<int>(t: 0, at: this + 276);`
  - `VariantInit(&pvarg);`
  - `v2 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v2 < 0 )`
  - `VariantInit(pvarg: &pvargDest);`
  - `v3 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v3 < 0 )`
  - `v4 = *(this + 304) == 0;`
  - `LOBYTE(v13) = 1;`
  - `if ( v4 )`
  - `Alpha = IWzGr2DLayer::GetAlpha(a1: &v12);`
  - `LOBYTE(v13) = 2;`
  - `v7 = (*(*v6 + 144))(`
  - `if ( v7 < 0 )`
  - `if ( v12 != 0 )`
  - …（截断 16 行）

#### CMob::OnStatReset  (1487 B)

  - `int __thiscall CMob::OnStatReset(CMob *this, struct CInPacket *a2)`
  - `UINT128::UINT128(this: v7);`
  - `CInPacket::DecodeBuffer(a1: v7, a2: 0x10u);`
  - `UINT128::UINT128(this: v5, a2: v7, a3: 0x80u);`
  - `if ( MobStat::IsMovementAffectingStat() != 0 && *(this + 330) != 0 )`
  - `ZRef<CMob::ReservedPacket>::_Alloc(a1: v8);`
  - `UINT128::setValue(this: (v9 + 1), a2: v7);`
  - `CInPacket::operator=(a1: a2);`
  - `ZList<ZRef<CMob::ReservedPacket>>::AddTail(a1: this + 1328);`
  - `result = ZRef<CMob::ReservedPacket>::operator=(a1: v8);`
  - `if ( v9 != 0 )`
  - `return ZRef<CMob::ReservedPacket>::~ZRef<CMob::ReservedPacket>(a1: 0);`
  - `UINT128::UINT128(this: &v4, a2: v7, a3: 0x80u);`
  - `return CMob::ProcessStatReset(a1: this, a2: v4, a3: v5[0], a4: v5[1], a5: v5[2], a6: v6);`

#### CMob::OnStatSet  (1483 B)

  - `int __thiscall CMob::OnStatSet(CMob *this, struct CInPacket *a2)`
  - `UINT128::UINT128(this: v7);`
  - `CInPacket::DecodeBuffer(a1: v7, a2: 0x10u);`
  - `UINT128::UINT128(this: v5, a2: v7, a3: 0x80u);`
  - `if ( MobStat::IsMovementAffectingStat() != 0 && *(this + 330) != 0 )`
  - `ZRef<CMob::ReservedPacket>::_Alloc(a1: v8);`
  - `UINT128::setValue(this: (v9 + 1), a2: v7);`
  - `CInPacket::operator=(a1: a2);`
  - `ZList<ZRef<CMob::ReservedPacket>>::AddTail(a1: this + 1328);`
  - `result = ZRef<CMob::ReservedPacket>::operator=(a1: v8);`
  - `if ( v9 != 0 )`
  - `return ZRef<CMob::ReservedPacket>::~ZRef<CMob::ReservedPacket>(a1: 0);`
  - `UINT128::UINT128(this: &v4, a2: v7, a3: 0x80u);`
  - `return CMob::ProcessStatSet(a1: this, a2: v4, a3: v5[0], a4: v5[1], a5: v5[2], a6: v6);`

#### CMob::PrepareActionLayer  (5762 B)

  - `int __thiscall CMob::PrepareActionLayer(CMob *this)`
  - `CurrentAction = CMob::GetCurrentAction(a1: &v36);`
  - `FineAction = CMob::GetFineAction(this, nAction: CurrentAction);`
  - `v4 = *(this + 269) + 20 * FineAction;`
  - `if ( CMob::LoadMobAction(a1: FineAction) == 0 )`
  - `_CxxThrowException(a1: &v37, a2: &_TI3_AVCTerminateException__);`
  - `sub_402FAB(a1: -2, a2: 3);`
  - `v5 = *(this + 304) == 0;`
  - `if ( v5 )`
  - `IWzGr2DLayer::RemoveCanvas(this: *(this + 304), a2: &v38, a3: &pvarg);`
  - `sub_4156AB(a1: &v38);`
  - `if ( v6 < 0 )`
  - `*(this + 258) = _ZtlSecureTear<long>(a1: 0, a2: this + 1024);`
  - `v7 = *(v4 + 12);`
  - `if ( v7 != 0 )`
  - `while ( 1 )`
  - …（截断 16 行）

#### CMob::ProcessAction  (2117 B)

  - `int *__thiscall CMob::ProcessAction(int **this, int a2)`
  - `CurrentAction = CMob::GetCurrentAction(this, a2: 0);`
  - `v15 = (_ZtlSecureFuse<long>(a1: this + 250, a2: *(this + 252)) - 30);`
  - `result = _ZtlSecureTear<long>(a1: v15, a2: this + 250);`
  - `*(this + 252) = result;`
  - `if ( v5 )`
  - `v6 = &(*(this + 269))[5 * CurrentAction];`
  - `v7 = *(*(this + 249) != 0 ? *(this + 249) - 3 : 4);`
  - `*(this + 249) = v7 != 0 ? (v7 + 16) : 0;`
  - `v12 = *(this + 242);`
  - `if ( _ZtlSecureFuse<long>(a1: this + 240, a2: v12) <= -1 )`
  - `if ( *(this + 249) == 0 )`
  - `*(this + 249) = v6[3];`
  - `else if ( *(this + 249) == 0 )`
  - `*(this + 242) = _ZtlSecureTear<long>(a1: -1, a2: this + 240);`
  - `return CMob::PrepareActionLayer(this);`
  - …（截断 16 行）

#### CMob::ProcessStatReset  (2191 B)

  - `void __thiscall CMob::ProcessStatReset(int *this, char a2, int a3, int a4, int a5, int a6)`
  - `UINT128::UINT128(this: &v15, &a2, a3: 0x80u);`
  - `sub_78C70A(a1: v15, a2: v16[0], a3: v16[1], a4: v16[2], a5: v17);`
  - `v7 = UINT128::operator&(a1: v18, a2: dword_BEFA20);`
  - `if ( UINT128::operator bool(a1: v7) != 0 )`
  - `if ( *(this + 169) == 0 || (v8 = *(this + 99)) == 0 )`
  - `v8 = *(this + 98);`
  - `sub_78C7E8(a1: (v8 + 280));`
  - `v9 = UINT128::operator&(a1: v18, a2: dword_BEF9B0);`
  - `if ( UINT128::operator bool(a1: v9) != 0 )`
  - `MobStat::SetFrom(a1: this + 104, a2: *(this + 98));`
  - `CMob::OnDoomed(this, a2: 0);`
  - `v10 = UINT128::operator&(a1: v18, a2: dword_BEF8F0);`
  - `if ( UINT128::operator bool(a1: v10) != 0 )`
  - `sub_66B562(a1: 0, a2: 0, a3: 0);`
  - `if ( sub_670C54(a1: this) != 0 )`
  - …（截断 16 行）

#### CMob::ProcessStatSet  (10404 B)

  - `void __thiscall CMob::ProcessStatSet(__int64 ***this, char a2, int a3, int a4, int a5, CInPacket *a6)`
  - `update_time = get_update_time();`
  - `UINT128::UINT128(this: v47, &a2, a3: 0x80u);`
  - `MobStat::DecodeTemporary(a1: this + 104, a2: v47[0], a3: v47[1], a4: v48, a5: HIDWORD(v48), a6: v49, a7: v50);`
  - `v8 = UINT128::operator&(a1: &pvargDest, a2: dword_BEFA20);`
  - `if ( UINT128::operator bool(a1: v8) != 0 )`
  - `v9 = *(this + 148);`
  - `*(this + 279) = update_time;`
  - `if ( v9 == 2121003 || v9 == 2221003 )`
  - `MobStat::AdjustDamagedElemAttr(this: (this + 104), a2: v9, a3: v51);`
  - `if ( *(this + 169) == 0 || (v10 = *(this + 99)) == 0 )`
  - `v10 = *(this + 98);`
  - `sub_78C7E8(a1: (v10 + 280));`
  - `v11 = UINT128::operator&(a1: &pvargDest, a2: dword_BEF930);`
  - `if ( UINT128::operator bool(a1: v11) != 0 )`
  - `*(this + 280) = update_time;`
  - …（截断 16 行）

#### CMob::SetActive  (2154 B)

  - `int __thiscall CMob::SetActive(CMob *this, int a2)`
  - `v3 = *(this + 70) != 0 ? *(this + 70) - 12 : 0;`
  - `v4 = *(this + 71) != 0 ? *(this + 71) - 12 : 0;`
  - `if ( a2 == 0 )`
  - `if ( *(*(this + 71) != 0 ? *(this + 71) - 12 + 0x18 : 24) == 0 )`
  - `v8 = _ZtlSecureFuse<long>(a1: this + 304, a2: *(this + 78)) - 3;`
  - `if ( v8 != 0 )`
  - `if ( v8 != 1 )`
  - `v10 = _ZtlSecureTear<long>(a1: -1, a2: v9);`
  - `v11 = *(this + 169) == 0;`
  - `*(this + 78) = v10;`
  - `if ( v11 || *(this + 99) == 0 )`
  - `v12 = *(this + 98);`
  - `v12 = *(this + 99);`
  - `v13 = _ZtlSecureFuse<long>(a1: v12 + 64, a2: *(v12 + 72));`
  - `CMovePath::DiscardByInterrupt(this: (v3 + 428), a2: v13, a3: v3, a4: 0);`
  - …（截断 16 行）

#### CMob::SetFrameInfo  (9439 B)

  - `int __thiscall CMob::SetFrameInfo(int this, _DWORD *a2, _DWORD *a3)`
  - `if ( !v4 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `v6 = *(this + 1216) == 0;`
  - `LOBYTE(v50) = 1;`
  - `if ( v6 )`
  - `Alpha = IWzGr2DLayer::GetAlpha(a1: &v47);`
  - `LOBYTE(v50) = 2;`
  - `v11 = (*(v10 + 144))(`
  - `if ( v11 < 0 )`
  - `sub_41569E(a1: &v47);`
  - `LOBYTE(v50) = 0;`
  - `if ( v12 < 0 )`
  - `if ( v13 < 0 )`
  - `if ( v3[21] > -1 )`
  - …（截断 16 行）

#### CMob::SetGuided  (497 B)

  - `int __thiscall CMob::SetGuided(CMob *this, int a2, int a3)`
  - `update_time = get_update_time();`
  - `v5 = sub_672B51(a1: this + 176);`
  - `if ( update_time + a3 == 0 )`
  - `result = *(this + 48);`
  - `*(this + 69) = result;`

#### CMob::SetMoveAction  (748 B)

  - `int __thiscall CMob::SetMoveAction(CMob *this, int a2, int a3)`
  - `FineMoveDirAction = CMob::GetFineMoveDirAction(this, nMoveDirAction: a2);`
  - `if ( a3 != 0 || (result = _ZtlSecureFuse<long>(a1: this + 948, a2: *(this + 239)), FineMoveDirAction != result) )`
  - `v6 = _ZtlSecureTear<long>(a1: FineMoveDirAction, a2: this + 948);`
  - `v7 = *(this + 242);`
  - `*(this + 239) = v6;`
  - `result = _ZtlSecureFuse<long>(a1: this + 960, a2: v7);`
  - `if ( result <= -1 )`
  - `return CMob::PrepareActionLayer(this);`

#### CMob::SetShoeAttr  (3211 B)

  - `int __thiscall CMob::SetShoeAttr(CMob *this)`
  - `v2 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 144);`
  - `if ( v2 != 0 )`
  - `v3 = sub_4FE802(a1: v2);`
  - `sub_500D1A(a1: v3);`
  - `v4 = *(this + 137);`
  - `if ( *(this + 169) == 0 || (v5 = *(this + 99)) == 0 )`
  - `v5 = *(this + 98);`
  - `if ( (*(this + 71) != 0 ? *(this + 71) - 12 : 0) != 0`
  - `&& *(*(this + 71) != 0 ? *(this + 71) - 12 + 0x250 : 592) != 0`
  - `&& *(v5 + 516) != 0 )`
  - `v4 = _ZtlSecureFuse<long>(a1: v5 + 172, a2: *(v5 + 180));`
  - `if ( *(this + 169) == 0 || (v6 = *(this + 99)) == 0 )`
  - `v6 = *(this + 98);`
  - `if ( _ZtlSecureFuse<long>(a1: v6 + 64, a2: *(v6 + 72)) != 0 )`
  - `v8 = v4 + *(this + 138);`
  - …（截断 16 行）

#### CMob::SetTemporaryStat  (2439 B)

  - `int __thiscall CMob::SetTemporaryStat(CMob *this, struct CInPacket *a2, int a3)`
  - `v4 = UINT128::UINT128(this: v19, a2: 0);`
  - `UINT128::UINT128(this: v11, a2: v4, a3: 0x80u);`
  - `v5 = operator~(a1: v18, a2: v11[0], a3: v11[1], a4: v11[2], a5: v12);`
  - `UINT128::UINT128(this: v14, a2: v5, a3: v13);`
  - `MobStat::Reset(a1: this + 416, a2: v14[0], a3: v14[1], a4: v15, a5: update_time);`
  - `MobStat::SetFrom(a1: this + 416, a2: *(this + 98));`
  - `UINT128::UINT128(this: v20);`
  - `CInPacket::DecodeBuffer(a1: v20, a2: 0x10u);`
  - `update_time = get_update_time();`
  - `UINT128::UINT128(this: &v12, a2: v20, a3: 0x80u);`
  - `MobStat::DecodeTemporary(a1: this + 416, a2: v12, a3: v13, a4: v14[0], a5: v14[1], a6: v15, a7: update_time);`
  - `v6 = UINT128::operator&(a1: v18, a2: dword_BEF9B0);`
  - `if ( UINT128::operator bool(a1: v6) != 0 )`
  - `MobTemplate = CMobTemplate::GetMobTemplate(a1: 5);`
  - `MobStat::SetFromWhenDoom(this: (this + 416), a2: MobTemplate);`
  - …（截断 16 行）

#### CMob::ShowDamage  (942 B)

  - `void __userpurge CMob::ShowDamage(CMob *this@<ecx>, int nDamage, int a3, int *a4, int bHalfHeight, int a6, int a7)`
  - `if ( *(this + 72) == 0 )`
  - `v8 = IWzShape2D::Getx(a1: *(this + 72));`
  - `v9 = -15 - a3 * (bHalfHeight != 0 ? 15 : 30) + IWzShape2D::Gety(a1: *(this + 72));`
  - `if ( nDamage <= 0 )`
  - `if ( nDamage != 0 )`
  - `CAnimationDisplayer::Effect_HP(this: dword_BEBF6C, lCenterLeft: v8, lCenterTop: v9, idx: -nDamage, a5: 1, lY: 0);`
  - `CAnimationDisplayer::Effect_Miss(this: dword_BEBF6C, a2: v8, a3: v9, a4: 0);`
  - `CAnimationDisplayer::Effect_HP(this: dword_BEBF6C, lCenterLeft: v8, lCenterTop: v9, idx: nDamage, a5: 0, lY: a4);`

#### CMob::ShowEffectByItem  (855 B)

  - `void **__thiscall CMob::ShowEffectByItem(_DWORD *this, int nItemID, VARIANTARG **bSuccess)`
  - `if ( *(this + 70) == 0 )`
  - `v4 = IWzShape2D::Getx(a1: *(this + 70));`
  - `v5 = IWzShape2D::Gety(a1: *(this + 70));`
  - `CAnimationDisplayer::Effect_ByItem(this: dword_BEBF6C, a2: v4, a3: v5 - 2, a4: nItemID, bSuccess);`
  - `result = get_item_sound(nItemID, bSuccess);`
  - `if ( result != 0 )`
  - `item_sound = get_item_sound(nItemID, bSuccess);`
  - `return play_game_sound(a1: item_sound, a2: 100);`

#### CMob::ShowHPIndicator  (3158 B)

  - `void *__thiscall CMob::ShowHPIndicator(CMob *this)`
  - `if ( *(this + 306) != 0 )`
  - `sub_402FAB(a1: -2, a2: 3);`
  - `if ( *(this + 306) == 0 )`
  - `IWzGr2DLayer::RemoveCanvas(this: *(this + 306), a2: &v19, a3: &v17);`
  - `if ( v19 != 0 )`
  - `(*(*v19 + 8))(a1: v19);`
  - `if ( v3 < 0 )`
  - `VariantInit(&pvarg);`
  - `v4 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v4 < 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v20) = 2;`
  - `sub_402F85(pvargDest: &v14, pvargSrc: &pvargSrc);`
  - `LOBYTE(v20) = 3;`
  - `sub_402F85(pvargDest: &v15, pvargSrc: &pvargSrc);`
  - …（截断 16 行）

#### CMob::ShowHitEffect  (2103 B)

  - `int __thiscall CMob::ShowHitEffect(CMob *this, int update_time)`
  - `result = _ZtlSecureFuse<long>(a1: this + 960, a2: *(this + 242));`
  - `if ( result < 12 || result > 20 )`
  - `result = _ZtlSecureFuse<long>(a1: v3, a2: *(this + 242));`
  - `if ( result < 21 || result > 37 )`
  - `if ( result < 6 || result > 8 )`
  - `if ( *(this + 169) == 0 || (v5 = *(this + 99)) == 0 )`
  - `v5 = *(this + 98);`
  - `result = _ZtlSecureFuse<long>(a1: v5 + 268, a2: *(v5 + 276));`
  - `if ( update_time >= result )`
  - `if ( sub_66918F(a1: this, a2: v6) == 0`
  - `|| (update_time = get_update_time(),`
  - `result = _ZtlSecureFuse<long>(a1: this + 984, a2: *(this + 248)) + 10000,`
  - `v7 = sub_668AAD(a1: this);`
  - `*(this + 242) = _ZtlSecureTear<long>(a1: v7, a2: v3);`
  - `CMob::PrepareActionLayer(this);`
  - …（截断 16 行）

#### CMob::TryDoingSkill  (2137 B)

  - `int __thiscall CMob::TryDoingSkill(CMob *this, int *a2, int *a3, struct CMob::TARGETINFO *a4)`
  - `if ( *(this + 169) != 0 )`
  - `if ( *(this + 151) != 0 )`
  - `if ( *(this + 141) != 0 )`
  - `if ( *(this + 144) != 0 )`
  - `if ( *(this + 172) != 0 )`
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
  - …（截断 16 行）

#### CMob::TrySpeaking  (11573 B)

- SP_ 节点：`SP_1796_S__S`
  - `void __thiscall CMob::TrySpeaking(CMob *this, int j, unsigned int i)`
  - `v4 = *(this + 98);`
  - `v5 = *(v4 + 640);`
  - `v6 = (v4 + 640);`
  - `if ( v5 == 0 )`
  - `v7 = *(v5 - 4);`
  - `if ( v7 == 0 )`
  - `if ( j >= v7 )`
  - `CurrentAction = CMob::GetCurrentAction(this, a2: 0);`
  - `if ( CurrentAction < 9 || CurrentAction > 11 )`
  - `if ( v8 == -1 )`
  - `else if ( v8 == -1 )`
  - `if ( i == -1 )`
  - `for ( i = 0; ; ++i )`
  - `if ( *v6 == 0 || i >= *(v11 - 4) )`
  - `v60 = *(v11 + 8 * i + 4);`
  - …（截断 16 行）

#### CMob::Update  (35594 B)

- SP_ 节点：`SP_1590_EFFECT_SUMMONIMG_D`
  - `int __fastcall CMob::Update(CMob *this, int a2)`
  - `update_time = get_update_time();`
  - `CChatBalloon::CheckTimeOut(this: (this + 20), a2: 0);`
  - `v3 = *(this + 57);`
  - `while ( v3 != 0 )`
  - `v153 = *(v3 + 28);`
  - `v4 = *(v3 != 0 ? v3 - 16 + 4 : 4);`
  - `AttackInfo = CMobTemplate::GetAttackInfo(a1: v153);`
  - `if ( *(AttackInfo + 160) != 0 )`
  - `*(this + 39) = *(AttackInfo + 140);`
  - `sub_670A3F(a1: this);`
  - `v6 = _ZtlSecureFuse<long>(a1: *(this + 98) + 432, a2: *(*(this + 98) + 440));`
  - `v7 = *(this + 147) == 0;`
  - `if ( !v7 && update_time - *(this + 279) >= 1000 )`
  - `if ( sub_68146C(this: *(this + 98), a2: *(this + 148)) != 0 )`
  - `if ( i <= 0 )`
  - …（截断 16 行）

#### CMob::UpdateAffectedSkillList  (5747 B)

  - `int __thiscall CMob::UpdateAffectedSkillList(CMob *this, int a2)`
  - `v3 = *(this + 114) == 0;`
  - `if ( !v3 )`
  - `sub_47CCB7(a1: this + 460, a2: &v12);`
  - `if ( *(this + 118) != 0 )`
  - `sub_47CCB7(a1: this + 476, a2: &v12);`
  - `if ( *(this + 122) != 0 )`
  - `sub_47CCB7(a1: this + 492, a2: &v12);`
  - `if ( *(this + 126) != 0 )`
  - `sub_47CCB7(a1: this + 508, a2: &v12);`
  - `if ( *(this + 130) != 0 )`
  - `sub_47CCB7(a1: this + 524, a2: &v12);`
  - `if ( *(this + 134) != 0 )`
  - `sub_47CCB7(a1: this + 540, a2: &v12);`
  - `if ( *(this + 138) != 0 )`
  - `sub_47CCB7(a1: this + 556, a2: &v12);`
  - …（截断 16 行）

### CMobPool（13 函数）


#### CMobPool::CancelChaseTarget  (712 B)

  - `void __thiscall CMobPool::CancelChaseTarget(CMobPool *this, struct CMob *a2)`
  - `v2 = *(this + 10);`
  - `while ( v2 != 0 )`
  - `v3 = *(v2 + 4);`
  - `v4 = *(v2 != 0 ? v2 - 16 + 4 : 4);`
  - `if ( CMob::IsActive(this: v3) != 0 )`
  - `v5 = sub_668ADD(this: v3, a2: &v7);`
  - `if ( v7 != 0 )`
  - `(*(*v7 + 8))(a1: v7);`
  - `if ( v6 != 0 && *(v6 + 592) == (a2 != 0 ? a2 + 4 : 0) )`
  - `sub_66B562(a1: 0, a2: 0, a3: 0);`

#### CMobPool::FindBodyAttackMob  (2953 B)

  - `_DWORD *__thiscall CMobPool::FindBodyAttackMob(CMobPool *this, const struct tagRECT *a2, int a3)`
  - `v3 = *(this + 10);`
  - `if ( v3 == 0 )`
  - `while ( 1 )`
  - `v5 = *(v3 + 4);`
  - `v6 = *(v3 != 0 ? v3 - 16 + 4 : 4);`
  - `v22 = (*(get_field() + 324) & 2) != 0;`
  - `if ( (v22 || a3 == 0 || _ZtlSecureFuse<unsigned long>(a1: v5 + 95, a2: v5[97]) == 9400594)`
  - `&& timeGetTime() != 0`
  - `&& _ZtlSecureFuse<long>(a1: v5 + 270, a2: v5[272]) == 0`
  - `&& timeGetTime() == 0`
  - `&& sub_667569(a1: v5) == 0`
  - `&& *(v5[98] + 520) == 0`
  - `&& sub_670C06(a1: v5) == 0`
  - `&& (v5 == -416 || v5[217] <= 0) )`
  - `if ( v23 == 0 )`
  - …（截断 16 行）

#### CMobPool::FindHitMobInRect  (1973 B)

  - `int __userpurge CMobPool::FindHitMobInRect@<eax>(`
  - `v10 = *(this + 10);`
  - `while ( v10 != 0 )`
  - `if ( v19 >= a4 )`
  - `v11 = *(v10 + 4);`
  - `v12 = *(v10 != 0 ? v10 - 16 + 4 : 4);`
  - `if ( timeGetTime() != 0`
  - `&& timeGetTime() == 0`
  - `&& (a5 == 0`
  - `|| (v13 = _ZtlSecureFuse<unsigned long>(a1: v11 + 380, a2: *(v11 + 97))) != _ZtlSecureFuse<unsigned long>(`
  - `a2: *(a5 + 97)))`
  - `&& (a6 == 0 || _ZtlSecureFuse<unsigned long>(a1: v11 + 380, a2: *(v11 + 97)) == a6)`
  - `&& (a8 == 0 || a8 == _ZtlSecureFuse<unsigned long>(a1: *(v11 + 98) + 12, a2: *(*(v11 + 98) + 20)))`
  - `&& *(*(v11 + 98) + 520) == 0`
  - `&& sub_670C06(a1: v11) == 0`
  - `&& (a9 != 0 || *(v11 + 202) == 0) )`
  - …（截断 16 行）

#### CMobPool::GetMob  (628 B)

  - `int __thiscall CMobPool::GetMob(CMobPool *this, unsigned int a2)`
  - `if ( ZMap<unsigned long,__POSITION *,unsigned long>::GetAt(a1: &a2, a2: &v7) != 0 )`
  - `v3 = *(v2 + 1);`
  - `if ( (v8 & 1) != 0 )`
  - `sub_445A87(a1: v5);`

#### CMobPool::OnMobChangeController  (532 B)

  - `DWORD __thiscall CMobPool::OnMobChangeController(CMobPool *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `v4 = CInPacket::Decode4(this: a2);`
  - `if ( v3 == 0 )`
  - `return CMobPool::SetRemoteMob(this, a2: v4);`
  - `v6 = CInPacket::Decode1(this: a2);`
  - `return CMobPool::SetLocalMob(this, a2: v3, a3: v5, a4: v6, a5: a2);`

#### CMobPool::OnMobCrcKeyChanged  (705 B)

  - `int __thiscall CMobPool::OnMobCrcKeyChanged(CMobPool *this, struct CInPacket *a2)`
  - `*(this + 30) = CInPacket::Decode4(this: a2);`
  - `for ( i = *(this + 10); i != 0; *(v4 + 1236) = 1 )`
  - `v4 = *(i + 4);`
  - `v5 = *(i != 0 ? i - 16 + 4 : 4);`
  - `COutPacket::COutPacket(a1: 164);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: v7);`
  - `return ZArray<unsigned char>::RemoveAll(a1: v8);`

#### CMobPool::OnMobEnterField  (1992 B)

  - `int __thiscall CMobPool::OnMobEnterField(CMobPool *this, struct CInPacket *a2)`
  - `v17 = CInPacket::Decode4(this: a2);`
  - `v16 = CInPacket::Decode1(this: a2);`
  - `v15 = CInPacket::Decode4(this: a2);`
  - `Mob = CMobPool::GetMob(this, a2: v17);`
  - `if ( Mob != 0 )`
  - `*(Mob + 300) = _ZtlSecureTear<int>(t: 1, at: (Mob + 292));`
  - `CMob::SetTemporaryStat(this: v4, a2, a3: v10);`
  - `MobTemplate = CMobTemplate::GetMobTemplate(a1: v15);`
  - `v4 = CreateMob(a1: MobTemplate);`
  - `v6 = v4 != 0 ? (v4 + 2) : 0;`
  - `sub_6726BC(a1: v12);`
  - `v7 = sub_6799C6(a1: this + 28);`
  - `sub_6799A3(a1: v12);`
  - `ZMap<unsigned long,__POSITION *,unsigned long>::Insert(a1: &v17, a2: &v14);`
  - `sub_445A87(a1: v12);`
  - …（截断 16 行）

#### CMobPool::OnMobLeaveField  (2042 B)

  - `int __thiscall CMobPool::OnMobLeaveField(CMobPool *this, struct CInPacket *a2)`
  - `v10 = CInPacket::Decode4(this: a2);`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `result = ZMap<unsigned long,__POSITION *,unsigned long>::GetAt(a1: &v10, a2: &v11);`
  - `if ( result != 0 )`
  - `v9 = *(v11 + 4);`
  - `sub_6726BC(a1: v8);`
  - `if ( v3 != 0 )`
  - `*(v6 + 275) = _ZtlSecureTear<long>(a1: v3, a2: v9 + 1092);`
  - `sub_679950(a1: v8);`
  - `if ( timeGetTime() != 0 )`
  - `*(v5 + 75) = _ZtlSecureTear<int>(t: 0, at: v9 + 73);`
  - `if ( CMob::IsActive(this: v9) != 0 )`
  - `CMobPool::CancelChaseTarget(this, a2: v9);`
  - `if ( *(*(v9 + 98) + 520) == 0 )`
  - `if ( sub_670C54(a1: v9) == 0 )`
  - …（截断 16 行）

#### CMobPool::OnMobPacket  (1842 B)

- 分支 case：`239`, `240`, `242`, `243`, `244`, `245`, `246`, `247`, `250`, `251`, `252`, `253`, `254`, `255`
  - `void __thiscall CMobPool::OnMobPacket(CMobPool *this, int a2, struct CInPacket *a3)`
  - `v4 = CInPacket::Decode4(this: a3);`
  - `Mob = CMobPool::GetMob(this, a2: v4);`
  - `if ( Mob != 0 )`
  - `switch ( a2 )`
  - `CMob::OnMove(this: Mob, a2: a3);`
  - `CMob::OnCtrlAck(this: Mob, a2: a3);`
  - `CMob::OnStatSet(this: Mob, a2: a3);`
  - `CMob::OnStatReset(this: Mob, a2: a3);`
  - `sub_66C500(a1: a3);`
  - `sub_66C675(a1: a3);`
  - `CMob::OnDamaged(this: Mob, a2: a3);`
  - `sub_66D8E7(a1: a3);`
  - `sub_66D639(a1: a3);`
  - `sub_66D6B9(a1: a3);`
  - `CMob::OnEffectByItem(this: Mob, a2: a3);`
  - …（截断 16 行）

#### CMobPool::OnPacket  (646 B)

- 分支 case：`236`, `237`, `238`, `249`
  - `void __thiscall CMobPool::OnPacket(CMobPool *this, signed int a2, struct CInPacket *a3)`
  - `switch ( a2 )`
  - `CMobPool::OnMobEnterField(a1: a3);`
  - `CMobPool::OnMobLeaveField(a1: a3);`
  - `CMobPool::OnMobChangeController(a1: a3);`
  - `CMobPool::OnMobCrcKeyChanged(a1: a3);`
  - `if ( a2 >= 239 && a2 <= 255 )`
  - `CMobPool::OnMobPacket(this, a2, a3);`

#### CMobPool::SetLocalMob  (1530 B)

  - `int __thiscall CMobPool::SetLocalMob(CMobPool *this, int a2, _DWORD *a3, int a4, struct CInPacket *a5)`
  - `Mob = CMobPool::GetMob(this, a2: a3);`
  - `v8 = CInPacket::Decode4(this: a5);`
  - `if ( v7 != 0 )`
  - `CMob::SetTemporaryStat(this: v7, a2: v6, a3: a4);`
  - `MobTemplate = CMobTemplate::GetMobTemplate(a1: v8);`
  - `v7 = CreateMob(a1: MobTemplate);`
  - `sub_6726BC(a1: v12);`
  - `a5 = sub_679950(a1: v12);`
  - `ZMap<unsigned long,__POSITION *,unsigned long>::Insert(a1: &a3, a2: &a5);`
  - `sub_445A87(a1: v12);`
  - `CMob::Init(this: v7, a2: a3, a3: v6);`
  - `result = CMob::SetActive(this: v7, a2: 1);`
  - `if ( a2 > 1 )`
  - `return sub_66B562(`

#### CMobPool::SetRemoteMob  (761 B)

  - `DWORD __thiscall CMobPool::SetRemoteMob(CMobPool *this, unsigned int a2)`
  - `result = ZMap<unsigned long,__POSITION *,unsigned long>::GetAt(a1: &a2, a2: &v5);`
  - `if ( result != 0 )`
  - `v4 = *(v5 + 4);`
  - `result = CMob::IsActive(this: v4);`
  - `CMob::SetActive(this: v4, a2: 0);`
  - `result = timeGetTime();`
  - `if ( result == 0 )`
  - `sub_679A04(this: this + 7, a2: v5);`
  - `return ZMap<unsigned long,__POSITION *,unsigned long>::RemoveKey(a1: &a2);`

#### CMobPool::Update  (2799 B)

  - `void __thiscall CMobPool::Update(CMobPool *this)`
  - `void (__thiscall ***v4)(_DWORD); // ecx`
  - `void (__thiscall **v10)(CMob *); // eax`
  - `void (__thiscall **v15)(int); // eax`
  - `void (__thiscall ***v23)(CMob *); // [esp+18h] [ebp-10h]`
  - `field = get_field();`
  - `CMapLoadable::UpdateObstacleInfo(this: field);`
  - `v3 = *(this + 10);`
  - `while ( v3 != 0 )`
  - `v4 = *(v3 + 4);`
  - `v5 = *(v3 != 0 ? v3 - 16 + 4 : 4);`
  - `(**v4)(a1: v4);`
  - `v6 = *(this + 15);`
  - `while ( v6 != 0 )`
  - `v8 = *(v6 != 0 ? v6 - 16 + 4 : 4);`
  - `v9 = *(v6 + 4);`
  - …（截断 16 行）

### CMobTemplate（2 函数）


#### CMobTemplate::GetAttackInfo  (608 B)

  - `int __thiscall CMobTemplate::GetAttackInfo(CMobTemplate *this, int a2)`
  - `if ( a2 < 0 || a2 >= *(this + 134) )`
  - `if ( *v2 == 0 )`
  - `v3 = sub_67FD81(a1: v5, a2);`
  - `sub_538B3E(a1: v3);`
  - `if ( v6 != 0 )`
  - `sub_5390CB(a1: 0);`

#### CMobTemplate::GetMobTemplate  (3910 B)

- SP_ 节点：`SP_954_MOB_07DIMG`
  - `static int __cdecl CMobTemplate::GetMobTemplate(int *a1)`
  - `sub_68223D(&a1, a2: v22);`
  - `if ( v23 == 0 )`
  - `LOBYTE(v27) = 1;`
  - `LOBYTE(v27) = 2;`
  - `sub_416821(a1: &v26, a2: v3, a3: v1);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v27) = 3;`
  - `sub_402F85(pvargDest: &v21, pvargSrc: &pvargSrc);`
  - `LOBYTE(v27) = 4;`
  - `v4 = sub_41468C(a1: &v26);`
  - `_bstr_t::_bstr_t(a1: v4);`
  - `v6 = sub_406472();`
  - `v7 = sub_402680(a1: v6);`
  - `IWzResMan::GetObjectA(this: v7, pvargDest: &pvarg, sUOL: v14, vParam: v15, vAux: p_pvargDest);`
  - `LOBYTE(v27) = 6;`
  - …（截断 16 行）

### MobStat（6 函数）


#### MobStat::AdjustDamagedElemAttr  (611 B)

  - `int __userpurge MobStat::AdjustDamagedElemAttr@<eax>(MobStat *this@<ecx>, int a2, const int *const a3)`
  - `if ( a2 == 2121003 )`
  - `*(this + 2) = 3;`
  - `*(this + 3) = 0;`
  - `if ( a2 != 2221003 )`
  - `*(this + 2) = 0;`
  - `*(this + 3) = 3;`
  - `*(this + 4) = 0;`
  - `*(this + 5) = 0;`
  - `*(this + 6) = 0;`
  - `*(this + 7) = 0;`

#### MobStat::DecodeTemporary  (12794 B)

  - `int __thiscall MobStat::DecodeTemporary(_DWORD *this, int a2, int a3, int a4, int a5, CInPacket *a6, int a7)`
  - `v8 = UINT128::operator&(a1: v49, a2: dword_BEFAB0);`
  - `if ( UINT128::operator bool(a1: v8) != 0 )`
  - `*(this + 10) = CInPacket::Decode2(this: a6);`
  - `*(this + 11) = CInPacket::Decode4(this: a6);`
  - `*(this + 12) = a7 + 500 * CInPacket::Decode2(this: a6);`
  - `v11 = UINT128::operator&(a1: v49, a2: dword_BEFAA0);`
  - `if ( UINT128::operator bool(a1: v11) != 0 )`
  - `*(this + 14) = CInPacket::Decode2(this: a6);`
  - `*(this + 15) = CInPacket::Decode4(this: a6);`
  - `*(this + 16) = a7 + 500 * CInPacket::Decode2(this: a6);`
  - `v12 = UINT128::operator&(a1: v49, a2: dword_BEFA90);`
  - `if ( UINT128::operator bool(a1: v12) != 0 )`
  - `*(this + 18) = CInPacket::Decode2(this: a6);`
  - `*(this + 19) = CInPacket::Decode4(this: a6);`
  - `*(this + 20) = a7 + 500 * CInPacket::Decode2(this: a6);`
  - …（截断 16 行）

#### MobStat::IsMovementAffectingStat  (825 B)

  - `BOOL MobStat::IsMovementAffectingStat()`
  - `if ( (byte_BEF888 & 1) == 0 )`
  - `sub_873F63(a1: v3, a2: dword_BEFA40);`
  - `sub_873F63(a1: v4, a2: dword_BEFA30);`
  - `sub_873F63(a1: v5, a2: dword_BEF9B0);`
  - `v0 = sub_873F63(a1: v6, a2: dword_BEF8B0);`
  - `UINT128::UINT128(this: dword_BEF878, a2: v0, a3: 0x80u);`
  - `atexit(a1: nullsub_13);`
  - `v1 = UINT128::operator&(a1: v3, a2: dword_BEF878);`
  - `return UINT128::operator!(a1: v1) == 0;`

#### MobStat::Reset  (8312 B)

  - `int __thiscall MobStat::Reset(_DWORD *this, int a2, int a3, int a4, int a5)`
  - `v6 = UINT128::operator&(a1: v38, a2: dword_BEFAB0);`
  - `if ( UINT128::operator bool(a1: v6) != 0 )`
  - `*(this + 12) = 0;`
  - `*(this + 11) = 0;`
  - `*(this + 10) = 0;`
  - `v7 = UINT128::operator&(a1: v38, a2: dword_BEFAA0);`
  - `if ( UINT128::operator bool(a1: v7) != 0 )`
  - `*(this + 16) = 0;`
  - `*(this + 15) = 0;`
  - `*(this + 14) = 0;`
  - `v8 = UINT128::operator&(a1: v38, a2: dword_BEFA90);`
  - `if ( UINT128::operator bool(a1: v8) != 0 )`
  - `*(this + 20) = 0;`
  - `*(this + 19) = 0;`
  - `*(this + 18) = 0;`
  - …（截断 16 行）

#### MobStat::SetFrom  (1252 B)

  - `DWORD __thiscall MobStat::SetFrom(char *this, int a2)`
  - `memset(a1: this, a2: 0, a3: 0x208u);`
  - `*this = _ZtlSecureFuse<long>(a1: a2 + 112, a2: *(a2 + 120));`
  - `memcpy(a1: this + 4, a2: (a2 + 280), a3: 0x20u);`
  - `*(this + 9) = _ZtlSecureFuse<long>(a1: a2 + 184, a2: *(a2 + 192));`
  - `*(this + 13) = _ZtlSecureFuse<long>(a1: a2 + 196, a2: *(a2 + 204));`
  - `*(this + 17) = _ZtlSecureFuse<long>(a1: a2 + 208, a2: *(a2 + 216));`
  - `*(this + 21) = _ZtlSecureFuse<long>(a1: a2 + 220, a2: *(a2 + 228));`
  - `*(this + 25) = _ZtlSecureFuse<long>(a1: a2 + 232, a2: *(a2 + 240));`
  - `*(this + 29) = _ZtlSecureFuse<long>(a1: a2 + 244, a2: *(a2 + 252));`
  - `if ( _ZtlSecureFuse<long>(a1: a2 + 64, a2: *(a2 + 72)) == 3 )`
  - `v3 = _ZtlSecureFuse<long>(a1: a2 + 160, a2: *(a2 + 168));`
  - `v3 = _ZtlSecureFuse<long>(a1: a2 + 148, a2: *(a2 + 156));`
  - `*(this + 33) = v3;`
  - `*(this + 60) = *(a2 + 336);`
  - `*(this + 122) = timeGetTime();`
  - …（截断 16 行）

#### MobStat::SetFromWhenDoom  (562 B)

  - `int __thiscall MobStat::SetFromWhenDoom(MobStat *this, const struct CMobTemplate *a2)`
  - `memcpy(a1: this + 4, a2: a2 + 280, a3: 0x20u);`
  - `*(this + 9) = _ZtlSecureFuse<long>(a1: a2 + 184, a2: *(a2 + 48));`
  - `*(this + 17) = _ZtlSecureFuse<long>(a1: a2 + 208, a2: *(a2 + 54));`
  - `*(this + 25) = _ZtlSecureFuse<long>(a1: a2 + 232, a2: *(a2 + 60));`
  - `result = _ZtlSecureFuse<long>(a1: a2 + 148, a2: *(a2 + 39));`
  - `*(this + 33) = result;`

### MOBSKILLLEVELDATA（1 函数）


#### MOBSKILLLEVELDATA::GetEffectUOL  (272 B)

  - `_DWORD *__thiscall MOBSKILLLEVELDATA::GetEffectUOL(_DWORD *this, _DWORD *a2)`
  - `v2 = *(this + 17);`
  - `if ( v2 != 0 )`
  - `InterlockedIncrement(lpAddend: (v2 + 8));`

### MOBINFO（1 函数）


#### MOBINFO::Clear  (439 B)

  - `int __thiscall CUIMonsterBook::MOBINFO::Clear(CUIMonsterBook::MOBINFO *this)`
  - `sub_414663(a1: this + 4);`
  - `sub_414663(a1: this + 8);`
  - `sub_414663(a1: this + 12);`
  - `sub_868368(a1: this + 16);`
  - `sub_8684FC(a1: this + 24);`
  - `sub_868432(a1: this + 20);`
  - `sub_428712(a1: 0);`
  - `return sub_416B72(a1: this + 32);`

### CVecCtrlMob（11 函数）


#### CVecCtrlMob::BeginUpdateActive  (144 B)

  - `int __thiscall CVecCtrlMob::BeginUpdateActive(CVecCtrlMob *this)`
  - `CVecCtrl::BeginUpdateActive(this);`

#### CVecCtrlMob::ChaseTargetImp  (536 B)

  - `void __thiscall CVecCtrlMob::ChaseTargetImp(CVecCtrlMob *this, int a2, struct IVecCtrlOwner *a3, int a4)`
  - `if ( a2 != 0 )`
  - `*(this + 150) = CRand32::Random(a1: this + 608) % 0xEA60u + 180000;`
  - `if ( *(this + 148) != a3 && *(this + 149) == 0 )`
  - `*(this + 148) = a3;`
  - `*(this + 149) = a4;`
  - `memset(a1: this + 1252, a2: 0, a3: 0x14u);`
  - `*(this + 148) = 0;`
  - `*(this + 149) = 0;`

#### CVecCtrlMob::CollisionDetectFloat  (4748 B)

  - `signed int __thiscall CVecCtrlMob::CollisionDetectFloat(CVecCtrlMob *this, const struct AbsPos *a2, __int64 a3)`
  - `if ( *(this + 146) == 2 )`
  - `v5 = *(this + 69);`
  - `if ( v5 != 0 )`
  - `v7 = *(v5 + 32);`
  - `v6 = *(v8 + 8 * v7);`
  - `v34 = *(v8 + 8 * v7 + 4);`
  - `v9 = *(this + 147);`
  - `v11 = *(v10 + 8 * v9);`
  - `v12 = *(v10 + 8 * v9 + 4);`
  - `if ( v6 >= v11 )`
  - `if ( v12 >= v34 )`
  - `if ( v34 - v36 > 40 )`
  - `if ( _ZtlSecureFuse<double>(a1: this + 32, a2: *(this + 12)) >= v35 )`
  - `if ( _ZtlSecureFuse<double>(a1: this + 32, a2: *(this + 12)) > v29 )`
  - `*(this + 12) = _ZtlSecureTear<double>(a1: SLOBYTE(v24), a2: HIDWORD(v29));`
  - …（截断 16 行）

#### CVecCtrlMob::CollisionDetectWalk  (6070 B)

  - `int __userpurge CVecCtrlMob::CollisionDetectWalk@<eax>(CVecCtrlMob *this@<ecx>, __int64 a2, int a3)`
  - `v5 = (this + 224);`
  - `v33 = _ZtlSecureFuse<double>(a1: this + 224, a2: *(this + 60));`
  - `v34 = v33 - _ZtlSecureFuse<double>(a1: a2, a2: *(a2 + 16));`
  - `if ( v34 == dbl_AF0DE8 )`
  - `return CVecCtrl::CollisionDetectWalk(this, a2, a3: HIDWORD(a2));`
  - `v6 = *(this + 146);`
  - `if ( v6 != 1 )`
  - `if ( v6 != 2 )`
  - `v12 = *(this + 68);`
  - `v13 = *(v12 + 32);`
  - `v38 = *(v11 + 8 * v13);`
  - `v14 = *(this + 147);`
  - `v39 = *(v11 + 8 * v13 + 4);`
  - `v15 = *(v11 + 8 * v14);`
  - `v16 = *(v11 + 8 * v14 + 4);`
  - …（截断 16 行）

#### CVecCtrlMob::CreateInstance  (408 B)

  - `static CVecCtrlMob *CVecCtrlMob::CreateInstance()`
  - `v0 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1272);`
  - `if ( v0 != 0 )`
  - `v1 = CVecCtrlMob::CVecCtrlMob(this: v0);`
  - `if ( v1 == 0 )`
  - `_com_raise_error(a1: -2147024882, a2: 0);`

#### CVecCtrlMob::CtrlUpdateActiveMove  (3408 B)

  - `void __thiscall CVecCtrlMob::CtrlUpdateActiveMove(CVecCtrlMob *this)`
  - `if ( *(this + 72) != 0 )`
  - `CVecCtrl::SetInput(this, a2: 0, a3: 0);`
  - `if ( *(this + 314) <= 0 )`
  - `if ( *(this + 148) != 0 )`
  - `v2 = CRand32::Random(a1: this + 608);`
  - `v3 = *(this + 148);`
  - `*(this + 314) = (v2 % 0x190 + 800) / 0x5A;`
  - `if ( (*(*v3 + 32))(a1: v3, a2: (v2 % 0x190 + 800) % 0x5A, a3: &dword_BF1270) == 0`
  - `&& (*(**(this + 148) + 32))(a1: *(this + 148), a2: &dword_BEDB54) == 0`
  - `&& (*(**(this + 148) + 32))(a1: *(this + 148), a2: &dword_BF0D30) == 0 )`
  - `v4 = (*(**(this + 148) + 32))(a1: *(this + 148), a2: &dword_BF1270);`
  - `(*(**(this + 148) + 32))(a1: *(this + 148), a2: &dword_BEDB54);`
  - `(*(**(this + 148) + 32))(a1: *(this + 148), a2: &dword_BF0D30);`
  - `(*(**(this + 148) + 32))(a1: *(this + 148), a2: &dword_BF1578);`
  - `ZXString<char>::Format(a1: &v11, a2: aR6025DDDD, a3: v4);`
  - …（截断 16 行）

#### CVecCtrlMob::InspectUpdateActive  (2008 B)

  - `void __thiscall CVecCtrlMob::InspectUpdateActive(CVecCtrlMob *this)`
  - `v2 = *(this + 5);`
  - `if ( v2 != 0 && (*(*v2 + 32))(a1: v2, a2: &dword_BEDB54) != 0 && (v3 = *(this + 5)) != 0 )`
  - `v4 = (v3 - 4);`
  - `if ( v4 != 0 && (v4[141] != 0 || v4[144] != 0 || v4[172] != 0 || timeGetTime() != 0) )`
  - `v5 = *(this + 146);`
  - `if ( v5 < 0 )`
  - `if ( v5 <= 2 )`
  - `*(this + 299) = 1;`
  - `*(this + 300) = 0;`
  - `if ( v5 != 3 )`
  - `*(this + 299) = 0;`
  - `v6 = *(this + 313);`
  - `v13 = *(this + 314);`
  - `v12 = _ZtlSecureFuse<double>(a1: this + 32, a2: *(this + 12));`
  - `v11 = _ZtlSecureFuse<double>(a1: this + 56, a2: *(this + 18));`
  - …（截断 16 行）

#### CVecCtrlMob::SaveFloatStateAfterCollision  (373 B)

  - `int __thiscall CVecCtrlMob::SaveFloatStateAfterCollision(CVecCtrlMob *this)`
  - `v1 = *(this + 286) == 0;`
  - `*(this + 280) = *(this + 313);`
  - `result = *(this + 314);`
  - `*(this + 281) = result;`
  - `if ( v1 )`
  - `*(this + 287) = 0;`

#### CVecCtrlMob::SaveFloatStateBeforeCollision  (671 B)

  - `void __thiscall CVecCtrlMob::SaveFloatStateBeforeCollision(CVecCtrlMob *this)`
  - `qmemcpy(this + 1024, this + 32, 0x60u);`
  - `*(this + 253) = *(this + 313);`
  - `*(this + 254) = *(this + 314);`
  - `v2 = _ZtlSecureFuse<double>(a1: this + 1024, a2: *(this + 260));`
  - `*(this + 282) = v2 != _ZtlSecureFuse<double>(a1: this + 32, a2: *(this + 12))`
  - `|| (v3 = _ZtlSecureFuse<double>(a1: this + 1048, a2: *(this + 266)),`
  - `_ZtlSecureFuse<double>(a1: this + 56, a2: *(this + 18)) != v3);`
  - `*(this + 286) = 1;`

#### CVecCtrlMob::SetActive  (437 B)

  - `void *__thiscall CVecCtrlMob::SetActive(`
  - `CVecCtrl::SetActive(this, a2, a3, a4, a5, a6, a7, a8);`
  - `*(this + 148) = 0;`
  - `*(this + 149) = 0;`
  - `*(this + 151) = 0;`
  - `return memset(a1: this + 1252, a2: 0, a3: 0x14u);`

#### CVecCtrlMob::WorkUpdateActive  (1545 B)

  - `signed int __thiscall CVecCtrlMob::WorkUpdateActive(CVecCtrlMob *this, signed int a2)`
  - `v3 = *(this + 5);`
  - `if ( v3 != 0 && (*(*v3 + 32))(a1: v3, a2: &dword_BEDB54) != 0 && (v4 = *(this + 5)) != 0 )`
  - `v5 = (v4 - 4);`
  - `if ( v5 != 0 && (v5[141] != 0 || v5[144] != 0 || v5[172] != 0 || timeGetTime() != 0) )`
  - `CVecCtrl::SetInput(this, a2: 0, a3: 0);`
  - `v6 = *(this + 146);`
  - `if ( v6 != 0 )`
  - `if ( v7 != 0 )`
  - `if ( v8 != 0 )`
  - `if ( v8 == 1 )`
  - `sub_9BD487(a1: this);`
  - `sub_9BD799(this);`
  - `sub_9BCF48(a1: this);`
  - `CVecCtrlMob::CtrlUpdateActiveMove(this);`
  - `sub_9BCB02(this);`
  - …（截断 16 行）

### CActionMan（17 函数）


#### CActionMan::CreateAfterimageLayer  (3741 B)

  - `int __thiscall CActionMan::CreateAfterimageLayer(char *this, int a2, int **a3, _DWORD *a4)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v25) = 2;`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v25) = 3;`
  - `if ( a4 == 0 )`
  - `Alpha = IWzGr2DLayer::GetAlpha(a1: &v24);`
  - `LOBYTE(v25) = 4;`
  - `if ( v5 )`
  - `v7 = (*(*v6 + 144))(`
  - `if ( v7 < 0 )`
  - `sub_41569E(a1: &v24);`
  - `if ( v8 < 0 )`
  - `LOBYTE(v25) = 1;`
  - `if ( v9 < 0 )`
  - `sub_402FAB(a1: -2, a2: 3);`
  - …（截断 16 行）

#### CActionMan::GetCharacterImgEntry  (22176 B)

- SP_ 节点：`SP_1035_STAND`, `SP_1140_ISLOT`, `SP_1141_VSLOT`, `SP_1142_SFX`, `SP_1143_AFTERIMAGE`, `SP_1144_WALK`, `SP_1145_ATTACK`, `SP_1146_ATTACKSPEED`, `SP_1150_WEEKLY`, `SP_961_INFO`
  - `int __thiscall CActionMan::GetCharacterImgEntry(_WORD *this, int a2, int a3)`
  - `if ( a3 < 0 )`
  - `*(a2 + 4) = 0;`
  - `v4 = sub_416F8E(a1: &a3, a2: 0);`
  - `if ( v4 != 0 )`
  - `v80 = *(v4 + 4);`
  - `*(v80 + 48) = dword_BF060C(a1: v100);`
  - `if ( *(*(v5 + 1) + 60) != 0 )`
  - `Instance = TSimpleSingleton<CDayOfWeek>::GetInstance();`
  - `CurrentDay = CDayOfWeek::GetCurrentDay(this: Instance);`
  - `get_weekly_data_path(a1: &v113, a2: a3, a3: CurrentDay);`
  - `if ( v113 != 0 && *v113 != 0 )`
  - `VariantInit(pvarg: &pvargDest);`
  - `v83 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v83 < 0 )`
  - `LOBYTE(v119) = 42;`
  - …（截断 16 行）

#### CActionMan::GetMeleeAttackRange  (1046 B)

  - `int __thiscall CActionMan::GetMeleeAttackRange(char *this, int sAfterimageUOL, int nAction, int rc)`
  - `if ( nAction == 55 )`
  - `result = dword_BF040C(a1: rc, a2: -88, a3: -62, a4: -18, a5: -6);`
  - `if ( sAfterimageUOL == 0 )`
  - `return _bstr_t::Data_t::Release(a1: v6);`
  - `if ( sAfterimageUOL != 0 )`
  - `InterlockedIncrement(lpAddend: (sAfterimageUOL + 8));`
  - `WeaponAfterImage = CActionMan::GetWeaponAfterImage(this, a2: sAfterimageUOL);`
  - `if ( WeaponAfterImage != 0 )`
  - `result = sub_4144D4(a1: rc, a2: *(WeaponAfterImage + 16) + 48 * nAction);`
  - `result = dword_BF0410(a1: rc);`

#### CActionMan::GetSummonedProp  (2758 B)

  - `_DWORD *__stdcall CActionMan::GetSummonedProp(_DWORD *a1, int a2, int a3)`
  - `if ( CSkillInfo::GetSkill(this: dword_BE78DC, a2) != 0 )`
  - `sub_41081F(a1: &a2);`
  - `VariantInit(&pvarg);`
  - `v4 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v4 < 0 )`
  - `LOBYTE(v23) = 2;`
  - `VariantInit(pvarg: &pvargDest);`
  - `v5 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v5 < 0 )`
  - `sub_405210(a1: &a2);`
  - `LOBYTE(v23) = 3;`
  - `v7 = sub_406472();`
  - `v8 = sub_402680(a1: v7);`
  - `ObjectA = IWzResMan::GetObjectA(this: v8, pvargDest: &v17, sUOL: v14, vParam: p_pvargDest, vAux: p_pvarg);`
  - `LOBYTE(v23) = 5;`
  - …（截断 16 行）

#### CActionMan::GetWeaponAfterImage  (14475 B)

- SP_ 节点：`SP_5636_LT`, `SP_5647_RB`
  - `int __thiscall CActionMan::GetWeaponAfterImage(char *this, int a2)`
  - `if ( a2 != 0 )`
  - `v77 = sub_40B32B();`
  - `if ( sub_417FB8(a1: &v77, a2: v75) == 0 )`
  - `sub_415AD7(a1: v75);`
  - `sub_416864(a1: 162, a2: &v82);`
  - `sub_416942(a1: 162, a2: &v82);`
  - `memset(a1: *(v76 + 16), a2: 0, a3: 2592u);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v83) = 2;`
  - `sub_402F85(pvargDest: &v57, pvargSrc: &pvargSrc);`
  - `LOBYTE(v83) = 3;`
  - `v3 = sub_40B31F(a1: &a2);`
  - `_bstr_t::_bstr_t(a1: v3);`
  - `v5 = sub_406472();`
  - `v6 = sub_402680(a1: v5);`
  - …（截断 16 行）

#### CActionMan::Init  (22679 B)

- SP_ 节点：`SP_1076_FACE`, `SP_1079_FRAME`, `SP_4151_FLIP`, `SP_4152_ROTATE`, `SP_5614_ACTION`, `SP_5624_DELAY`, `SP_5640_MOVE`
  - `int __thiscall CActionMan::Init(_WORD *this)`
  - `CActionFrame::LoadMappers();`
  - `v106 = *(CActionMan::GetCharacterImgEntry(this, a2: v110, a3: 2000) + 4);`
  - `result = sub_4158EE(a1: v110);`
  - `if ( v4 != 40 )`
  - `get_action_name_from_code(a1: &v84, a2: v4);`
  - `sub_40263B(a1: v106 + 12);`
  - `IWzProperty::GetItem(pvargDest: p_pvargDest, a2: v84);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v85, a2: v86);`
  - `if ( Unknown != 0 )`
  - `v5 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v5 < 0 && v5 != -2147467262 )`
  - `LOBYTE(v123) = 3;`
  - `if ( v6 < 0 )`
  - `if ( v4 >= 101 && v4 <= 108 )`
  - `v7 = alloca(32);`
  - …（截断 16 行）

#### CActionMan::LoadCharacterAction  (4056 B)

  - `int __thiscall CActionMan::LoadCharacterAction(`
  - `if ( nAction == 38 )`
  - `memset(a1: v19, a2: 0, a3: sizeof(v19));`
  - `memcpy(a1: v19, a2: aAvatarHairEquip, a3: sizeof(v19));`
  - `while ( v12 < dword_BD85A0 );`
  - `if ( v19[5] / 10000 == 105 && v19[6] != 0 )`
  - `if ( CItemInfo::IsCashItem(a1: v19[5]) == 0 && CItemInfo::IsCashItem(a1: v19[6]) != 0 )`
  - `if ( v14 == 0 )`
  - `v14 = (nGender != 0 ? 1041046 : 1040036);`
  - `if ( v19[6] == 0 && v14 / 10000 != 105 )`
  - `v19[6] = (nGender != 0 ? 1061039 : 1060026);`
  - `if ( v19[1] == 1002186 )`
  - `if ( v19[4] == 1032024 )`
  - `if ( v19[3] == 1022079 )`
  - `if ( v19[7] == 1072153 )`
  - `if ( v19[8] == 1082102 )`
  - …（截断 16 行）

#### CActionMan::LoadFaceLook  (80263 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1076_FACE`, `SP_5624_DELAY`, `SP_950_MAP`, `SP_959_ORIGIN`, `SP_960_DEFAULT`
  - `LONG __thiscall CActionMan::LoadFaceLook(`
  - `v8 = sub_417227(a1: v292, a2: 0);`
  - `if ( v8 != 0 )`
  - `v311 = *(v8 + 4);`
  - `sub_41593D(a1: v310);`
  - `v10 = dword_BF060C(a1: v269);`
  - `*(v9 + 9) = v10;`
  - `sub_4169BE(a1: v11);`
  - `sub_4156F1(a1: v12 + 12);`
  - `v13 = *(v311 + 8);`
  - `sub_41594F(a1: v310);`
  - `CActionMan::GetCharacterImgEntry(this: v306, a2: v299, a3: Unknown);`
  - `if ( v6 != 0 )`
  - `CharacterImgEntry = CActionMan::GetCharacterImgEntry(this: v306, a2: v275, a3: v6);`
  - `LOBYTE(v355) = 2;`
  - `v324 = *(CharacterImgEntry + 1);`
  - …（截断 16 行）

#### CActionMan::LoadMobAction  (25046 B)

- SP_ 节点：`SP_1147_ZIGZAG`, `SP_2866_RECT`, `SP_5612_A0`, `SP_5613_A1`, `SP_5624_DELAY`, `SP_5628_HEAD`, `SP_5636_LT`, `SP_5647_RB`
  - `int __thiscall CActionMan::LoadMobAction(char *this, void *a2, int a3, int a4)`
  - `v128 = a3 | (a2 << 8);`
  - `if ( sub_417D97(a1: &v128, a2: v132) != 0 )`
  - `v7 = dword_BF060C(a1: v98);`
  - `*(v6 + 40) = v7;`
  - `sub_416B72(a1: v8);`
  - `sub_415738(a1: v9 + 20);`
  - `return sub_415AC1(a1: v132);`
  - `v10 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 52);`
  - `LOBYTE(v148) = 1;`
  - `if ( v10 != 0 )`
  - `v11 = sub_40D420(a1: v10);`
  - `LOBYTE(v148) = 0;`
  - `sub_415AAF(a1: v144);`
  - `*(v145 + 12) = v5;`
  - `*(v145 + 16) = v4;`
  - …（截断 16 行）

#### CActionMan::LoadNpcAction  (10146 B)

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
  - `LOBYTE(v69) = 0;`
  - `sub_415B99(a1: v67);`
  - `*(v68 + 12) = v5;`
  - `*(v68 + 16) = v6;`
  - …（截断 16 行）

#### CActionMan::LoadPetAction  (28324 B)

- SP_ 节点：`SP_1147_ZIGZAG`, `SP_5624_DELAY`, `SP_5636_LT`, `SP_5647_RB`
  - `int __thiscall CActionMan::LoadPetAction(StringPoolStrings *this, IWzCanvas *a2, int a3, IWzCanvas *a4, int a5)`
  - `if ( a2 != 0 )`
  - `if ( a3 >= 9 )`
  - `v7 = sub_70D6E8(a1: &v135, a2: a3 - 9);`
  - `sub_409E70(a1: v7);`
  - `if ( (v161 & 1) != 0 )`
  - `sub_402D9A(this: &v135);`
  - `v143 = v6 | (v155 << 8);`
  - `LOBYTE(v163) = 1;`
  - `if ( sub_418F8B(a1: &v143, a2: v157) != 0 )`
  - `v8[11] = dword_BF060C(a1: v130);`
  - `v9 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 56);`
  - `LOBYTE(v163) = 2;`
  - `if ( v9 != 0 )`
  - `v10 = sub_40F0E9(a1: v9);`
  - `sub_414CF8(a1: v10);`
  - …（截断 16 行）

#### CActionMan::LoadShadowPartnerAction  (15742 B)

- SP_ 节点：`SP_5612_A0`, `SP_5613_A1`, `SP_957_PROPERTY`
  - `int __thiscall CActionMan::LoadShadowPartnerAction(char *this, int a2, int Unknown, void *a4)`
  - `v83 = Unknown | (a2 << 8);`
  - `if ( sub_419E84(a1: &v83, a2: v89) != 0 )`
  - `v7 = dword_BF060C(a1: v75);`
  - `*(v6 + 44) = v7;`
  - `sub_416DCA(a1: v8);`
  - `sub_415896(a1: v9 + 24);`
  - `return sub_415DA9(a1: v89);`
  - `v10 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 56);`
  - `LOBYTE(v96) = 1;`
  - `if ( v10 != 0 )`
  - `v11 = sub_411001(a1: v10);`
  - `LOBYTE(v96) = 0;`
  - `sub_415D97(a1: v93);`
  - `*(v94 + 12) = v5;`
  - `*(v94 + 16) = v4;`
  - …（截断 16 行）

#### CActionMan::LoadSummonedAction  (14911 B)

- SP_ 节点：`SP_1147_ZIGZAG`, `SP_5612_A0`, `SP_5613_A1`, `SP_5624_DELAY`, `SP_5636_LT`, `SP_5647_RB`
  - `int __thiscall CActionMan::LoadSummonedAction(char *this, StringPoolStrings *a2, int a3, int a4, int a5)`
  - `if ( sub_419A8C(a1: v73, a2: &v88) != 0 )`
  - `v9 = dword_BF060C(a1: v64);`
  - `*(v8 + 48) = v9;`
  - `sub_416D52(a1: v10);`
  - `sub_415850(a1: v11 + 28);`
  - `return sub_415D48(a1: &v88);`
  - `v12 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 60);`
  - `LOBYTE(v96) = 1;`
  - `if ( v12 != 0 )`
  - `v13 = sub_4105CA(a1: v12);`
  - `LOBYTE(v96) = 0;`
  - `sub_415D36(a1: &v93);`
  - `LOBYTE(v96) = 2;`
  - `CActionMan::GetSummonedProp(a1: &v91, a2: v7, a3: v63);`
  - `LOBYTE(v96) = 3;`
  - …（截断 16 行）

#### CActionMan::LoadTamingMobAction  (3220 B)

  - `signed int __stdcall CActionMan::LoadTamingMobAction(`
  - `if ( nAction == 4 )`
  - `if ( v5 >= 4 && v5 != 40 && v5 != 35 && v5 != 33 )`
  - `if ( nVehicleID / 10000 != 190 )`
  - `if ( v7 != 193 )`
  - `if ( v6 )`
  - `if ( v5 > 96 )`
  - `if ( v5 < 109 )`
  - `if ( v5 <= 110 )`
  - `if ( v5 != 121 )`
  - `if ( nVehicleID == 1932001 )`
  - `if ( v5 >= 95 )`
  - `if ( v5 < 36 )`
  - `if ( v5 > 37 )`
  - `if ( v5 <= 40 || v5 > 42 )`
  - `if ( nVehicleID == 1932003`
  - …（截断 16 行）

#### CActionMan::MergeCharacterSprite  (2056 B)

  - `int __stdcall CActionMan::MergeCharacterSprite(int a1, _DWORD *a2, int *a3)`
  - `if ( *a2 != 0 )`
  - `v3 = *(*a2 - 4);`
  - `if ( dword_BEC624[6 * *(a1 + 208)] != 0 )`
  - `result = ZArray<ZRef<CActionMan::CHARACTERACTIONFRAMEENTRY>>::_Realloc(a1: v13, a2: 1, a3: &a1 + 3);`
  - `if ( v3 > 0 )`
  - `v7 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 44);`
  - `if ( v7 != 0 )`
  - `v8 = sub_4124FC(a1: v7);`
  - `if ( v8 != 0 )`
  - `InterlockedIncrement(lpAddend: (v8 + 4));`
  - `v9 = (a1 + *a2);`
  - `CActionFrame::Draw(this: v9, a2: (v8 + 12), a3: (v8 + 16), a4: (v8 + 28), a5: (v8 + 20), a6: (v8 + 36), a7: 0);`
  - `result = sub_41504D(a1: v11);`
  - `if ( v12 != 0 )`
  - `result = sub_416461(a1: 0);`
  - …（截断 16 行）

#### CActionMan::MergeTamingMobSprite  (2113 B)

  - `int __stdcall CActionMan::MergeTamingMobSprite(int a1, _DWORD *i, int a3)`
  - `if ( *i != 0 )`
  - `v16 = *(*i - 4);`
  - `result = ZArray<ZRef<CActionMan::TAMINGMOBACTIONFRAMEENTRY>>::_Realloc(a1: v16, a2: 1, a3: &i + 3);`
  - `if ( v16 > 0 )`
  - `for ( i = 0; ; v4 = i )`
  - `v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 56);`
  - `if ( v6 != 0 )`
  - `v7 = sub_41269B(a1: v6);`
  - `if ( v7 != 0 )`
  - `InterlockedIncrement(lpAddend: (v7 + 4));`
  - `CActionFrame::Draw(this: (v4 + *v3), a2: (v7 + 12), a3: (v7 + 16), a4: (v7 + 40), a5: &v11, a6: (v7 + 48), a7: 1);`
  - `*(v13 + 36) = *(v4 + *v3 + 80);`
  - `v8 = (v4 + *v3 + 84);`
  - `v9 = (v13 + 20);`
  - `*(v13 + 20) = *v8++;`
  - …（截断 16 行）

#### CActionMan::load_character_action  (3180 B)

- SP_ 节点：`SP_1149_H4H5`
  - `signed int __stdcall CActionMan::load_character_action(`
  - `v20 = *(a3 + 4) != 0;`
  - `load_item_action(a1, a2, a3: a2 + 2000, a4, a5: 0, a6, a7: v20);`
  - `switch ( a1 )`
  - `if ( v8 == 38 )`
  - `load_item_action(a1: v10, a2, a3: a2 + 12000, a4, a5: 0, a6, a7: v20);`
  - `if ( v11 == 11 )`
  - `if ( v8 == 65 || v8 == 73 || v8 == 89 || v8 == 115 )`
  - `else if ( v11 == 18 || v11 == 19 || v11 == 20 )`
  - `v12 = *(a3 + 4 * v11);`
  - `if ( v12 != 0 )`
  - `load_item_action(a1: v13, a2, a3: v12, a4, a5: v11 == 11 ? v9 : 0, a6, a7: v20);`
  - `while ( v11 <= 51 );`
  - `if ( *(a3 + 4) == 0 )`
  - `if ( (byte_BE78B0 & 1) == 0 )`
  - `atexit(a1: sub_4138B4);`
  - …（截断 16 行）

### CActionFrame（9 函数）


#### CActionFrame::Draw  (12953 B)

- SP_ 节点：`SP_956_CANVAS`
  - `_DWORD *__thiscall CActionFrame::Draw(`
  - `CActionFrame::UpdateMBR(this);`
  - `CActionFrame::UpdateVisiblity(this);`
  - `v65 = *(this + 8) - *(this + 6);`
  - `v64 = (*(this + 9) - *(this + 7));`
  - `PcCreateObject::IWzCanvas(a1: v9, a2, a3: 0);`
  - `if ( v63 != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v66) = 2;`
  - `if ( v10 )`
  - `v12 = (*(v11 + 44))(`
  - `if ( v12 < 0 )`
  - `LOBYTE(v66) = 1;`
  - `if ( v13 < 0 )`
  - `if ( v14 < 0 )`
  - …（截断 16 行）

#### CActionFrame::ExtractMap  (5131 B)

  - `int __cdecl CActionFrame::ExtractMap(int a1, void *Unknown)`
  - `ZRef<ZList<CActionFrame::MAPINFO>>::_Alloc(a1: v21);`
  - `IWzCanvas::GetProperty(this: Unknown, a2: &v30);`
  - `LOBYTE(v31) = 2;`
  - `if ( v30 == 0 )`
  - `*(a1 + 4) = v22;`
  - `sub_4028DD(a1);`
  - `sub_4027E9(a1: &v30);`
  - `LOBYTE(v31) = 0;`
  - `sub_4028FA(a1: v21);`
  - `sub_405210(a1: &dword_BE7878);`
  - `sub_40263B(a1: &v30);`
  - `IWzProperty::GetItem(&pvargDest, a2: v16);`
  - `LOBYTE(v31) = 4;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v17, a2: v18);`
  - `v4 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - …（截断 16 行）

#### CActionFrame::GetSMapper  (376 B)

  - `_DWORD *__cdecl CActionFrame::GetSMapper(_DWORD *a1)`
  - `if ( dword_BE78A0 == 0 )`
  - `CActionFrame::LoadMappers();`
  - `if ( !v2 )`
  - `(*(*v1 + 4))(a1: v1);`

#### CActionFrame::GetZMapper  (376 B)

  - `_DWORD *__cdecl CActionFrame::GetZMapper(_DWORD *a1)`
  - `if ( dword_BE78A4 == 0 )`
  - `CActionFrame::LoadMappers();`
  - `if ( !v2 )`
  - `(*(*v1 + 4))(a1: v1);`

#### CActionFrame::LoadMappers  (9917 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1076_FACE`, `SP_3804_CHARACTERSTART`, `SP_3805_CHARACTEREND`
  - `_DWORD *CActionFrame::LoadMappers()`
  - `sub_402F85(pvargDest: &v41, pvargSrc: &pvargSrc);`
  - `LOBYTE(v51) = 3;`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v51) = 4;`
  - `sub_405210(a1: &dword_BE788C);`
  - `LOBYTE(v51) = 5;`
  - `v1 = sub_406472();`
  - `v2 = sub_402680(a1: v1);`
  - `IWzResMan::GetObjectA(this: v2, pvargDest: &v40, sUOL: v34, vParam: p_pvargDest, vAux: v36);`
  - `LOBYTE(v51) = 6;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v37, a2: NewEnum);`
  - `v3 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v3 < 0 && v3 != -2147467262 )`
  - `if ( v4 < 0 )`
  - `if ( v5 < 0 )`
  - …（截断 16 行）

#### CActionFrame::Merge  (2880 B)

  - `int __thiscall CActionFrame::Merge(int *this, void *a2, void *a3, struct IWzCanvas *a4)`
  - `sub_40282B(a1: v15);`
  - `sub_402C40(a1: a3, a2: 1);`
  - `LOBYTE(v18) = 1;`
  - `sub_402C40(a1: a2, a2: 1);`
  - `LOBYTE(v18) = 2;`
  - `sub_402803(a1: v16 + 7);`
  - `CSpriteSource::Init(this: v5[1], a2: &a3, a3: &v17, a4);`
  - `sub_402D9A(this: &a3);`
  - `LOBYTE(v18) = 0;`
  - `sub_402D9A(this: &v17);`
  - `v16[3] = -*(v16[8] + 40);`
  - `v16[4] = -*(v16[8] + 44);`
  - `v7 = *(result + 36);`
  - `if ( v7 == dword_BE789C )`
  - `if ( v16 != 0 )`
  - …（截断 16 行）

#### CActionFrame::MergeGroup  (2970 B)

  - `int __thiscall CActionFrame::MergeGroup(_DWORD *this, int a2, int a3)`
  - `v33 = *(a2 + 4);`
  - `v3 = *(a3 + 4);`
  - `v4 = *(v3 + 12);`
  - `while ( v4 != 0 )`
  - `v6 = *(v4 != 0 ? v4 - 16 + 4 : 4);`
  - `v7 = *(v33 + 12);`
  - `if ( v7 != 0 )`
  - `while ( 1 )`
  - `v8 = *(v7 != 0 ? v7 - 16 + 4 : 4);`
  - `if ( sub_402CCC(a1: v7) != 0 )`
  - `if ( v32 == 0 )`
  - `v26 += *(v7 + 4);`
  - `v27 += *(v7 + 8);`
  - `v28 += *(v5 + 4);`
  - `v29 += *(v5 + 8);`
  - …（截断 16 行）

#### CActionFrame::UpdateMBR  (707 B)

  - `int __thiscall CActionFrame::UpdateMBR(CActionFrame *this)`
  - `if ( *(this + 10) == 0 )`
  - `*(this + 10) = 1;`
  - `result = dword_BF0410(a1: this + 24);`
  - `for ( i = *(this + 4); i != 0; result = dword_BF0408(a1: v2, a2: v2, a3: v7) )`
  - `v5 = *(i != 0 ? i - 16 + 4 : 4);`
  - `v6 = *(i + 4);`
  - `dword_BF040C(a1: v7, a2: v6[3], a3: v6[4], a4: v6[3] + *(v6[8] + 28), a5: v6[4] + *(v6[8] + 32));`

#### CActionFrame::UpdateVisiblity  (3024 B)

  - `int __thiscall CActionFrame::UpdateVisiblity(CActionFrame *this)`
  - `v2 = *(this + 14);`
  - `if ( v2 != 0 && (v3 = *v2) != 0 )`
  - `v4 = *(v3 - 4) >> 1;`
  - `if ( v4 != 0 )`
  - `v5 = *(this + 14);`
  - `if ( v8 != 0 )`
  - `v25 = v9 | (v8 << 16);`
  - `sub_4036FA(a1: &v25, a2: &v24);`
  - `while ( *v7 != 0 );`
  - `v10 = *(this + 4);`
  - `if ( v10 != 0 )`
  - `v11 = *(v10 + 4);`
  - `v12 = *(v10 != 0 ? v10 - 16 + 4 : 4);`
  - `*(v11 + 20) = 1;`
  - `v13 = *(*(v11 + 32) + 20);`
  - …（截断 16 行）

### VCActionFrame（1 函数）


#### VCActionFrame::$construct  (196 B)

  - `int __cdecl construct<CActionFrame>(int a1)`
  - `if ( a1 != 0 )`
  - `return sub_41EAE2(a1);`

### CAnimationDisplayer（19 函数）


#### CAnimationDisplayer::Effect_BasicFloat  (8914 B)

  - `int __thiscall CAnimationDisplayer::Effect_BasicFloat(char *this, int lCenterLeft, int lCenterTop, int pCanvas)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v45) = 1;`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `LOBYTE(v45) = 2;`
  - `if ( dword_BF14EC == 0 )`
  - `IWzGr2D::CreateLayer(`
  - `LOBYTE(v45) = 5;`
  - `if ( v4 < 0 )`
  - `LOBYTE(v45) = 4;`
  - `if ( v5 < 0 )`
  - `if ( v44 == 0 )`
  - `v7 = (*(*v44 + 180))(a1: v44, a2: -1073343324);`
  - `if ( v7 < 0 )`
  - `v9 = (*(v8 + 224))(a1: v44, a2: -1);`
  - `if ( v9 < 0 )`
  - …（截断 16 行）

#### CAnimationDisplayer::Effect_ByItem  (8000 B)

- SP_ 节点：`SP_2435_EFFECT_ITEMEFFIMG_D`, `SP_958_SHAPE2DVECTOR2D`
  - `void __thiscall CAnimationDisplayer::Effect_ByItem(CAnimationDisplayer *this, CY a2, int a4, VARIANTARG **bSuccess)`
  - `LOBYTE(v43) = 1;`
  - `if ( v5 != 0 )`
  - `sub_416821(a1: &v39, a2: v6, a3: a4);`
  - `LOBYTE(v43) = 0;`
  - `sub_402D9A(this: &p_Hi);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v43) = 2;`
  - `sub_402F85(pvargDest: &v35, pvargSrc: &pvargSrc);`
  - `HIDWORD(v30) = &pvargDest;`
  - `LODWORD(v30) = &v35;`
  - `LOBYTE(v43) = 3;`
  - `v7 = sub_41468C(a1: &v39);`
  - `_bstr_t::_bstr_t(a1: v7);`
  - `v9 = sub_406472();`
  - `v10 = sub_402680(a1: v9);`
  - …（截断 16 行）

#### CAnimationDisplayer::Effect_CashItemGachapon  (3846 B)

  - `int __thiscall CAnimationDisplayer::Effect_CashItemGachapon(char *this, int a2, int a3, int a4, int a5)`
  - `if ( a5 == 0 )`
  - `sub_41C06C(a1: v5, a2: -1);`
  - `sub_4145AB(a1: &a4);`
  - `CAnimationDisplayer::LoadLayer(a1: &a3);`
  - `LOBYTE(v26) = 2;`
  - `v8 = sub_41468C(a1: &v25);`
  - `sub_43E86F(`
  - `a8: SHIDWORD(v18),`
  - `LOBYTE(v26) = 5;`
  - `if ( a5 != 0 )`
  - `VariantInit(&pvarg);`
  - `v9 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v9 < 0 )`
  - `LOBYTE(v26) = 6;`
  - `VariantInit(pvarg: &pvargDest);`
  - …（截断 16 行）

#### CAnimationDisplayer::Effect_Cool  (3138 B)

- SP_ 节点：`SP_5327_EFFECT_BASICEFFIMG_COOLHIT_COOL`
  - `int __thiscall CAnimationDisplayer::Effect_Cool(CAnimationDisplayer *this, int a2, int a3)`
  - `VariantInit(&pvarg);`
  - `v3 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v3 < 0 )`
  - `VariantInit(pvarg: &pvargDest);`
  - `v4 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v4 < 0 )`
  - `LOBYTE(v27) = 1;`
  - `v7 = sub_406472();`
  - `v8 = sub_402680(a1: v7);`
  - `IWzResMan::GetObjectA(this: v8, pvargDest: &v20, sUOL: v15, vParam: p_pvargDest, vAux: p_pvarg);`
  - `LOBYTE(v27) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v18, a2: v19);`
  - `if ( Unknown != 0 )`
  - `v10 = sub_41E527(a1: &v25);`
  - `if ( v10 < 0 && v10 != -2147467262 )`
  - …（截断 16 行）

#### CAnimationDisplayer::Effect_General  (3559 B)

  - `int *__thiscall CAnimationDisplayer::Effect_General(`
  - `sub_4145AB(a1: &a7);`
  - `CAnimationDisplayer::LoadLayer(a1: &a4);`
  - `sub_43E86F(a1: &a8, a2, a3, a4: v17, a5: v18, a6: v19, a7: v20, a8: v21, a9: v22, a10: v23);`
  - `LOBYTE(v27) = 2;`
  - `if ( a8 != 0 )`
  - `VariantInit(&pvarg);`
  - `v11 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v11 < 0 )`
  - `LOBYTE(v27) = 3;`
  - `VariantInit(pvarg: &pvargDest);`
  - `v12 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v12 < 0 )`
  - `LOBYTE(v27) = 4;`
  - `if ( a8 == 0 )`
  - `v14 = (*(*a8 + 68))(`
  - …（截断 16 行）

#### CAnimationDisplayer::Effect_HP  (24778 B)

- SP_ 节点：`SP_1644_EFFECT`, `SP_5515_D`, `SP_956_CANVAS`
  - `int __thiscall CAnimationDisplayer::Effect_HP(`
  - `if ( a5 != 0 )`
  - `if ( a5 == 1 )`
  - `sub_4027BE(a1: *(this + 95));`
  - `cyVal.Lo = *(this + 94);`
  - `if ( a5 != 2 )`
  - `sub_4027E9(a1: &v126);`
  - `return sub_4027E9(a1: &v128);`
  - `sub_4027BE(a1: *(this + 97));`
  - `cyVal.Lo = *(this + 96);`
  - `else if ( lY != 0 )`
  - `sub_4027BE(a1: *(this + 99));`
  - `cyVal.Lo = *(this + 98);`
  - `sub_4027BE(a1: *(this + 93));`
  - `cyVal.Lo = *(this + 92);`
  - `sub_4027BE(a1: cyVal.Lo);`
  - …（截断 16 行）

#### CAnimationDisplayer::Effect_Miss  (1904 B)

- SP_ 节点：`SP_984_MISS`
  - `void __thiscall CAnimationDisplayer::Effect_Miss(CAnimationDisplayer *this, int a2, int a3, int a4)`
  - `if ( a4 != 0 )`
  - `if ( a4 != 2 )`
  - `v5 = *(this + 96);`
  - `v5 = *(this + 92);`
  - `if ( v5 != 0 )`
  - `sub_415691(a1: &v15);`
  - `LOBYTE(v16) = 0;`
  - `sub_40263B(a1: &v15);`
  - `IWzProperty::GetItem(pvargDest: &pvarg, a2: v9);`
  - `LOBYTE(v16) = 2;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v10, a2: v11);`
  - `if ( Unknown != 0 )`
  - `v7 = sub_41E527(a1: &Unknown);`
  - `if ( v7 < 0 && v7 != -2147467262 )`
  - `LOBYTE(v16) = 4;`
  - …（截断 16 行）

#### CAnimationDisplayer::Effect_RewardRullet  (14142 B)

- SP_ 节点：`SP_4560_MAP_EFFECTIMG_MIRO_RR1_D`, `SP_4561_MAP_EFFECTIMG_MIRO_RR2_D`, `SP_4562_MAP_EFFECTIMG_MIRO_RR3_D`, `SP_966_SS`
  - `int __thiscall CAnimationDisplayer::Effect_RewardRullet(char *this, int a2, int a3, int a4, int a5)`
  - `LOBYTE(v102) = 3;`
  - `v7 = sub_416821(a1: &v94, a2: v6, a3: a2);`
  - `ZXString<unsigned short>::operator=(a1: v7);`
  - `LOBYTE(v102) = 6;`
  - `if ( v94 != 0 )`
  - `LOBYTE(v102) = 5;`
  - `if ( v85 != 0 )`
  - `LOBYTE(v102) = 8;`
  - `v10 = sub_416821(a1: &v93, a2: v9, a3);`
  - `ZXString<unsigned short>::operator=(a1: v10);`
  - `LOBYTE(v102) = 11;`
  - `if ( v93 != 0 )`
  - `LOBYTE(v102) = 10;`
  - `if ( v87 != 0 )`
  - `LOBYTE(v102) = 13;`
  - …（截断 16 行）

#### CAnimationDisplayer::Effect_SkillUse  (4737 B)

- SP_ 节点：`SP_966_SS`
  - `int __thiscall CAnimationDisplayer::Effect_SkillUse(`
  - `for ( i = -1; i <= a7; ++i )`
  - `if ( i < 0 )`
  - `v8 = alloca(32);`
  - `v9 = _itow(a1: i, a2: &v23, a3: 10);`
  - `HIDWORD(v25) = v32;`
  - `LOBYTE(v36) = 3;`
  - `sub_416821(a1: &v33, a2: v11, a3: a2);`
  - `LOBYTE(v36) = 2;`
  - `sub_4145AB(a1: &a5);`
  - `CAnimationDisplayer::LoadLayer(a1: &a4);`
  - `v14 = sub_41468C(a1: &v33);`
  - `sub_43E86F(`
  - `a9: SHIDWORD(v25),`
  - `LOBYTE(v36) = 6;`
  - `if ( v35 == 0 )`
  - …（截断 16 行）

#### CAnimationDisplayer::Effect_Tremble  (871 B)

  - `Concurrency::details::SchedulerProxy *__thiscall CAnimationDisplayer::Effect_Tremble(`
  - `if ( *(TSingleton<CConfig>::ms_pInstance + 34) != 0 || a6 != 0 )`
  - `*(this + 400) = a2;`
  - `result = (a4 + get_update_time());`
  - `*(this + 408) = result;`
  - `*(this + 412) = result + (a3 != 0 ? 1500 : 2000) + a5;`
  - `if ( a5 != 0 )`
  - `else if ( a3 != 0 )`
  - `*(this + 416) = v8;`

#### CAnimationDisplayer::LoadLayer  (281 B)

  - `int *__thiscall CAnimationDisplayer::LoadLayer(int *this, int *a2)`
  - `if ( !v3 )`
  - `(*(*v2 + 4))(a1: v2);`

#### CAnimationDisplayer::LoadSingleLayer  (8420 B)

  - `_DWORD *__cdecl CAnimationDisplayer::LoadSingleLayer(`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v52) = 2;`
  - `sub_402F85(pvargDest: &v47, pvargSrc: &pvargSrc);`
  - `HIDWORD(v42[0]) = &pvargDest;`
  - `LODWORD(v42[0]) = &v47;`
  - `LOBYTE(v52) = 3;`
  - `_bstr_t::_bstr_t(a1: a2);`
  - `v11 = sub_406472();`
  - `v12 = sub_402680(a1: v11);`
  - `ObjectA = IWzResMan::GetObjectA(this: v12, pvargDest: &pvarg, sUOL: Hi32, vParam: v42[0], vAux: SHIDWORD(v42[0]));`
  - `LOBYTE(v52) = 5;`
  - `unknown = get_unknown(a1: &v49, a2: ObjectA);`
  - `LOBYTE(v52) = 6;`
  - `sub_41A852(a1: unknown);`
  - `sub_4027B1(a1: &v49);`
  - …（截断 16 行）

#### CAnimationDisplayer::MakeLayer_FootHold  (8730 B)

  - `int __stdcall CAnimationDisplayer::MakeLayer_FootHold(wchar_t *a1, int *a2, int a3, int a4, int a5)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v55, pvargSrc: &pvargSrc);`
  - `LOBYTE(v65) = 1;`
  - `_bstr_t::_bstr_t(a1);`
  - `v6 = sub_406472();`
  - `v7 = sub_402680(a1: v6);`
  - `IWzResMan::GetObjectA(this: v7, pvargDest: &pvarg, sUOL: v44, vParam: v45, vAux: p_pvargDest);`
  - `LOBYTE(v65) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v47, a2: v48);`
  - `if ( Unknown != 0 )`
  - `v8 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v8 < 0 && v8 != -2147467262 )`
  - `LOBYTE(v65) = 7;`
  - `if ( v9 < 0 )`
  - `LOBYTE(v65) = 6;`
  - …（截断 16 行）

#### CAnimationDisplayer::NonFieldUpdate  (7234 B)

  - `int __fastcall CAnimationDisplayer::NonFieldUpdate(int a1, int a2)`
  - `update_time = get_update_time();`
  - `v4 = *(a1 + 400);`
  - `if ( v4 > dbl_AF0DE8 && update_time - *(a1 + 408) > 0 )`
  - `if ( dword_BF14EC == 0 )`
  - `IWzGr2D::GetCenter(this: dword_BF14EC, a2: &v38);`
  - `if ( v5 - *(a1 + 412) < 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v39) = 1;`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `v6 = *(a1 + 400) + *(a1 + 400);`
  - `LOBYTE(v39) = 2;`
  - `v35 = -*(a1 + 400);`
  - `if ( v7 != 0 )`
  - `v8 = CRand32::Random(a1: dword_BEDD38);`
  - `v36 = CRand32::Random(a1: dword_BEDD38);`
  - …（截断 16 行）

#### CAnimationDisplayer::RegisterFadeInOutAnimation  (6855 B)

- SP_ 节点：`SP_956_CANVAS`
  - `int __thiscall CAnimationDisplayer::RegisterFadeInOutAnimation(`
  - `LOBYTE(v41) = 1;`
  - `PcCreateObject::IWzCanvas(a1: v7, a2: &v40, a3: 0);`
  - `LOBYTE(v41) = 0;`
  - `if ( v38 != 0 )`
  - `if ( v40 != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v41) = 2;`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v41) = 3;`
  - `if ( v40 == 0 )`
  - `v9 = (*(v8 + 44))(`
  - `if ( v9 < 0 )`
  - `if ( v10 < 0 )`
  - `if ( v11 < 0 )`
  - `v13 = (*(v12 + 112))(a1: v40, a2: 0);`
  - …（截断 16 行）

#### CAnimationDisplayer::RegisterFireCrackerAnimation  (3944 B)

  - `int __thiscall CAnimationDisplayer::RegisterFireCrackerAnimation(`
  - `VariantInit(&pvarg);`
  - `v8 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v8 < 0 )`
  - `VariantInit(pvarg: &pvargDest);`
  - `v9 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v9 < 0 )`
  - `LOBYTE(v35) = 1;`
  - `_bstr_t::_bstr_t(a1: a2);`
  - `v11 = sub_406472();`
  - `v12 = sub_402680(a1: v11);`
  - `IWzResMan::GetObjectA(this: v12, pvargDest: &v29, sUOL: v24, vParam: p_pvargDest, vAux: p_pvarg);`
  - `LOBYTE(v35) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v27, a2: v28);`
  - `if ( Unknown != 0 )`
  - `v13 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - …（截断 16 行）

#### CAnimationDisplayer::RegisterFootHoldAnimation  (4419 B)

- SP_ 节点：`SP_2899_EFFECTDISTANCE`
  - `int __thiscall CAnimationDisplayer::RegisterFootHoldAnimation(`
  - `VariantInit(&pvarg);`
  - `v8 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v8 < 0 )`
  - `VariantInit(pvarg: &pvargDest);`
  - `v9 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v9 < 0 )`
  - `LOBYTE(v36) = 1;`
  - `_bstr_t::_bstr_t(a1: a2);`
  - `v11 = sub_406472();`
  - `v12 = sub_402680(a1: v11);`
  - `IWzResMan::GetObjectA(this: v12, pvargDest: &v29, sUOL: v23, vParam: p_pvargDest, vAux: p_pvarg);`
  - `LOBYTE(v36) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v26, a2: v27);`
  - `if ( Unknown != 0 )`
  - `v13 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - …（截断 16 行）

#### CAnimationDisplayer::RemoveAll  (753 B)

  - `void __thiscall CAnimationDisplayer::RemoveAll(CAnimationDisplayer *this)`
  - `sub_4467A0(a1: this + 48);`
  - `sub_44692A(a1: this + 68);`
  - `sub_446AB4(a1: this + 88);`
  - `sub_446C3E(a1: this + 108);`
  - `sub_446DC8(a1: this + 128);`
  - `sub_446F52(a1: this + 148);`
  - `sub_4470DC(a1: this + 168);`
  - `sub_447266(a1: this + 188);`
  - `sub_4473F0(a1: this + 208);`
  - `sub_44757A(a1: this + 228);`
  - `sub_447704(a1: this + 248);`
  - `sub_44788E(a1: this + 268);`
  - `sub_447A18(a1: this + 288);`
  - `sub_447BA2(a1: this + 308);`
  - `CAnimationDisplayer::ResetLocalFadeLayer(this: (this + 328));`
  - …（截断 16 行）

#### CAnimationDisplayer::ResetLocalFadeLayer  (462 B)

  - `int __thiscall CAnimationDisplayer::ResetLocalFadeLayer(CAnimationDisplayer *this)`
  - `for ( result = *(this + 3); result != 0; result = v4 != 0 ? v4 + 16 : 0 )`
  - `v4 = *(v3 + 4);`
  - `sub_402D2E(a1: v3);`
  - `*(this + 4) = 0;`
  - `*(this + 3) = 0;`
  - `*(this + 2) = 0;`

### CAvatar（28 函数）


#### CAvatar::0ACTIONINFO  (1175 B)

  - `void __thiscall CAvatar::ACTIONINFO::ACTIONINFO(_DWORD *this)`
  - `*(this + 1) = 0;`
  - `*(this + 2) = 0;`
  - `*(this + 3) = 0;`
  - ``eh vector constructor iterator'(`
  - `*(this + 374) = 0;`

#### CAvatar::1ACTIONINFO  (781 B)

  - `void __thiscall CAvatar::ACTIONINFO::~ACTIONINFO(char *this)`
  - `sub_4573B7(a1: this + 1496);`
  - ``eh vector destructor iterator'(`
  - `ZArray<long>::RemoveAll(a1: this + 12);`

#### CAvatar::ClearActionLayer  (1027 B)

  - `const void *__thiscall CAvatar::ClearActionLayer(CAvatar *this, int a2)`
  - `for ( i = 162; i != 0; --i )`
  - `ZArray<ZRef<CActionMan::CHARACTERACTIONFRAMEENTRY>>::_Realloc(a1: 0, a2: 1, a3: &a2 + 3);`
  - `ZArray<ZRef<CActionMan::TAMINGMOBACTIONFRAMEENTRY>>::_Realloc(a1: 0, a2: 1, a3: &a2 + 3);`
  - `while ( v3 != 0 );`
  - `v6 = (v2 + 1312);`
  - `for ( j = 46; j != 0; --j )`
  - `result = ZArray<ZRef<CActionMan::MORPHACTIONFRAMEENTRY>>::_Realloc(this: v6++, a2: 0, a3: 1, a4: &a2 + 3);`

#### CAvatar::ForcingAppearance  (474 B)

  - `void __thiscall CAvatar::ForcingAppearance(_DWORD *this, int a2, void *a3)`
  - `if ( a3 != 0 )`
  - `*(this + 303) = 0;`
  - `*(this + 280) = a2;`
  - `if ( a2 != 0 )`
  - `memcpy(a1: this + 228, a2: a3, a3: 0xD0u);`
  - `CAvatar::ClearActionLayer(this, a2: 0);`
  - `if ( CAvatar::GetOneTimeAction(this) <= -1 )`
  - `(*(*this + 16))(a1: this, a2: *(this + 314), a3: 1);`

#### CAvatar::GetBodyRect  (2228 B)

  - `int __thiscall CAvatar::GetBodyRect(_DWORD *this, int *a2, int a3)`
  - `v4 = (*(this + 314) & 0xFFFFFFFE) == 10;`
  - `v5 = *(this + 296) == 0;`
  - `if ( v5 )`
  - `if ( !v4 )`
  - `if ( sub_451987(this) )`
  - `a2[3] += *(this + 310);`
  - `a2[1] += *(this + 310);`
  - `*a2 += *(this + 309);`
  - `a2[2] += *(this + 309);`
  - `dword_BF0408(a1: a2, a2, a3: this + 304);`
  - `*a2 = *(this + 297);`
  - `a2[1] = *(this + 298);`
  - `a2[2] = *(this + 299);`
  - `a2[3] = *(this + 300);`
  - `if ( (*(this + 314) & 1) == 0 )`
  - …（截断 16 行）

#### CAvatar::GetFaceOrigin  (236 B)

  - `int *__thiscall CAvatar::GetFaceOrigin(int *this, int *a2)`
  - `v2 = *(this + 1069);`
  - `if ( v2 != 0 )`
  - `(*(*v2 + 4))(a1: v2);`

#### CAvatar::GetHeight  (2148 B)

  - `int __thiscall CAvatar::GetHeight(CAvatar *this)`
  - `v2 = *(this + 296) == 0;`
  - `if ( v2 )`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `v2 = *(this + 1073) == 0;`
  - `LOBYTE(v13) = 1;`
  - `Canvas = IWzGr2DLayer::GetCanvas(a1: v10, a2: &pvarg);`
  - `sub_4051E5(a1: *Canvas);`
  - `sub_4156AB(a1: v10);`
  - `LOBYTE(v13) = 0;`
  - `if ( v4 < 0 )`
  - `if ( v12 != 0 && IWzCanvas::Getcy(this: v12) > 0 )`
  - `if ( v12 == 0 )`
  - `v11 = IWzCanvas::Getcy(this: v12);`
  - `v2 = *(this + 1074) == 0;`
  - `LOBYTE(v13) = 2;`
  - …（截断 16 行）

#### CAvatar::GetLayerUnderFace  (240 B)

  - `int *__thiscall CAvatar::GetLayerUnderFace(int *this, int *a2)`
  - `v2 = *(this + 1074);`
  - `if ( v2 != 0 )`
  - `(*(*v2 + 4))(a1: v2);`

#### CAvatar::GetLayerZ  (192 B)

  - `int __thiscall CAvatar::GetLayerZ(void **this)`
  - `if ( *(this + 1074) != 0 )`
  - `return IWzGr2DLayer::GetZ(this: *(this + 1074));`

#### CAvatar::GetOneTimeAction  (4193 B)

- 分支 case：`27`, `32`, `41`
  - `signed int __thiscall CAvatar::GetOneTimeAction(CAvatar *this)`
  - `if ( CMorphTemplate::IsSuperMan() != 0 )`
  - `v2 = *(this + 315);`
  - `if ( v2 > 81 )`
  - `if ( v2 > 98 )`
  - `if ( v15 == 0 )`
  - `if ( v16 == 0 )`
  - `if ( v17 == 0 )`
  - `if ( v18 == 0 )`
  - `if ( v19 == 0 )`
  - `if ( v20 == 0 )`
  - `if ( v20 == 1 )`
  - `if ( v2 == 98 )`
  - `if ( v2 > 86 )`
  - `switch ( v2 )`
  - `if ( v2 == 86 )`
  - …（截断 16 行）

#### CAvatar::Init  (1488 B)

  - `int *__thiscall CAvatar::Init(`
  - `*(this + 1085) = nScale;`
  - `sub_4145AB(a1: &pOverlay);`
  - `CAnimationDisplayer::LoadLayer(this: &v13, a2: &pOrigin);`
  - `CAvatar::Init(this, a2: v13, i: v14, a4: v15, a5: v16, a6: v17);`
  - `AvatarLook::operator=(a1: a2);`
  - `*(this + 314) = nMoveAction;`
  - `*(this + 1066) = nDefaultEmotion;`
  - `CAvatar::NotifyAvatarModified(a1: v17);`
  - `LOBYTE(v18) = 0;`
  - `if ( pOrigin != 0 )`
  - `(*(*pOrigin + 8))(a1: pOrigin);`
  - `if ( pOverlay != 0 )`
  - `return (*(*pOverlay + 8))(a1: pOverlay);`

#### CAvatar::IsHideMorphed  (155 B)

  - `BOOL __thiscall CAvatar::IsHideMorphed(_DWORD *this)`
  - `return *(this + 296) != 0 && CMorphTemplate::IsHideMorphed(a1: *(this + 296)) != 0;`

#### CAvatar::IsMonsterMorphed  (244 B)

  - `BOOL __thiscall CAvatar::IsMonsterMorphed(CAvatar *this)`
  - `v1 = (this + 1184);`
  - `return *(this + 296) != 0 && sub_6887FB(a1: *(this + 296)) == 0 && CMorphTemplate::IsHideMorphed(a1: *v1) == 0;`

#### CAvatar::MoveAction2RawAction  (3290 B)

- 分支 case：`2`, `4`, `10`, `3`, `5`, `6`, `7`, `8`, `9`, `1`
  - `int __thiscall CAvatar::MoveAction2RawAction(CAvatar *this, int a2, int *a3)`
  - `if ( a3 != 0 )`
  - `if ( *(this + 296) != 0 )`
  - `if ( CAvatar::IsMonsterMorphed(a1: this) != 0 || CAvatar::IsHideMorphed(this) )`
  - `switch ( v4 )`
  - `if ( CMorphTemplate::IsSuperMan() != 0 )`
  - `else if ( *(this + 301) != 0 )`
  - `result = *(this + 282) != 1;`
  - `result = (*(this + 283) != 1) + 2;`

#### CAvatar::NotifyAvatarModified  (3862 B)

  - `int __thiscall CAvatar::NotifyAvatarModified(CAvatar *this, int a2)`
  - `if ( a2 == 0 )`
  - `result = memcmp(a1: this + 457, a2: this + 4, a3: 0x1C5u);`
  - `if ( result == 0 )`
  - `AvatarLook::operator=(a1: this + 4);`
  - `if ( (*(*this + 8))(a1: this) != 0`
  - `&& *(this + 61) == 0`
  - `&& CWvsContext::ShowQuestInfoDetail(a1: 22012) == 2 )`
  - `*(this + 61) = 1082262;`
  - `v4 = *(this + 73);`
  - `if ( v4 != 0`
  - `&& (v5 = CActionMan::GetCharacterImgEntry(this: dword_BE78D4, a2: v12, a3: v4),`
  - `LOBYTE(v15) = 1,`
  - `sub_41475D(a1: v5),`
  - `HIBYTE(a2) = v14 != 0,`
  - `LOBYTE(v15) = 0,`
  - …（截断 16 行）

#### CAvatar::PrepareActionLayer  (73931 B)

  - `void __fastcall CAvatar::PrepareActionLayer(CAvatar *this, int a2, int i, int a4, _DWORD *a5)`
  - `v6 = *(this + 312) == 0;`
  - `if ( !v6 )`
  - `CAvatar::ClearActionLayer(this, a2: 0);`
  - `v7 = sub_451E4C(this, a2: &v242);`
  - `v8 = *(v5 + 1184);`
  - `if ( v8 == HIDWORD(v5) )`
  - `if ( v7 >= 162 )`
  - `else if ( v7 >= 46 )`
  - `OneTimeAction = CAvatar::GetOneTimeAction(this: v5);`
  - `v6 = *(v5 + 1184) == HIDWORD(v5);`
  - `v257 = (1500 * (OneTimeAction > -1) + v5 + 1264);`
  - `CAvatar::ClearActionLayer(this: v5, a2: SHIDWORD(v5));`
  - `if ( *v259 == HIDWORD(v5) || *(*v259 - 4) == HIDWORD(v5) )`
  - `sub_40B9E7(a1: *(v5 + 1184), a2: v7, a3: v259);`
  - `if ( *v259 != HIDWORD(v5) )`
  - …（截断 16 行）

#### CAvatar::PrepareFaceLayer  (7557 B)

- SP_ 节点：`SP_5624_DELAY`
  - `void *__thiscall CAvatar::PrepareFaceLayer(_DWORD *this, LONG a2)`
  - `if ( *(this + 312) == 0 )`
  - `v4 = *(this + 280) == 0;`
  - `v5 = *(this + 37);`
  - `if ( !v4 && *(this + 230) != -1 )`
  - `v5 = *(this + 230);`
  - `v6 = _ZtlSecureFuse<long>(a1: this + 292, a2: *(this + 294));`
  - `FaceLook = CActionMan::LoadFaceLook(`
  - `arg0: *(this + 17),`
  - `Unknown: *(this + 21),`
  - `if ( a2 < 0 )`
  - `sub_402FAB(a1: -2, a2: 3);`
  - `v4 = *(this + 1072) == 0;`
  - `LOBYTE(v47) = 1;`
  - `if ( v4 )`
  - `IWzGr2DLayer::RemoveCanvas(this: *(this + 1072), a2: &v46, a3: &pvarg);`
  - …（截断 16 行）

#### CAvatar::RegisterNextBlink  (292 B)

  - `int __thiscall CAvatar::RegisterNextBlink(CAvatar *this)`
  - `*(this + 289) = 0;`
  - `v2 = rand() % 3000;`
  - `result = v2 + get_update_time() + 2000;`
  - `*(this + 290) = result;`

#### CAvatar::SetAvatarLook  (191 B)

  - `int __thiscall CAvatar::SetAvatarLook(CAvatar *this, const struct AvatarLook *a2)`
  - `AvatarLook::operator=(a1: a2);`
  - `return CAvatar::NotifyAvatarModified(a1: 0);`

#### CAvatar::SetEmotion  (3087 B)

- SP_ 节点：`SP_986_ETC_EMOTIONEFFECT_S`
  - `void __thiscall CAvatar::SetEmotion(CAvatar *this, unsigned int nEmotion, LONG tDuration)`
  - `v4 = (this + 1184);`
  - `if ( *(this + 296) == 0 )`
  - `if ( ((v5 = sub_451E4C(this, a2: 0), v6 = *v4, v7 = *v4 == 0, v8 = nEmotion, v7) || v5 != 8 && v5 != 20)`
  - `&& (v6 != 0 || v5 != 33 && v5 != 32)`
  - `if ( nEmotion <= 0x16 )`
  - `v9 = _ZtlSecureTear<long>(a1: nEmotion, a2: this + 1168);`
  - `v7 = *(this + 312) == 0;`
  - `*(this + 294) = v9;`
  - `*(this + 295) = 0;`
  - `if ( v7 )`
  - `CAvatar::PrepareFaceLayer(this, a2: tDuration);`
  - `if ( v10 != 0 )`
  - `LOBYTE(v11) = 0;`
  - `sub_416821(a1: &nEmotion, a2: v13, a3: v11);`
  - `LOBYTE(v28) = 0;`
  - …（截断 16 行）

#### CAvatar::SetGhostState  (226 B)

  - `int __thiscall CAvatar::SetGhostState(CAvatar *this, int a2)`
  - `*(this + 301) = a2;`
  - `CAvatar::ClearActionLayer(this, a2: 0);`
  - `return (*(*this + 20))(a1: this, a2: 6, a3: 100, a4: 0);`

#### CAvatar::SetLayerColor  (1137 B)

  - `int __thiscall CAvatar::SetLayerColor(CAvatar *this, int a2)`
  - `if ( *(this + 1072) == 0 )`
  - `v3 = *(this + 1072);`
  - `v4 = (*(*v3 + 224))(a1: v3, a2);`
  - `if ( v4 < 0 )`
  - `if ( *(this + 1073) == 0 )`
  - `v5 = *(this + 1073);`
  - `v6 = (*(*v5 + 224))(a1: v5, a2);`
  - `if ( v6 < 0 )`
  - `if ( *(this + 1074) == 0 )`
  - `v7 = *(this + 1074);`
  - `result = (*(*v7 + 224))(a1: v7, a2);`
  - `if ( result < 0 )`

#### CAvatar::SetMorphed  (11599 B)

  - `int __thiscall CAvatar::SetMorphed(CAvatar *this, unsigned int a2)`
  - `if ( *(this + 296) == a2 )`
  - `*(this + 296) = a2;`
  - `CAvatar::ClearActionLayer(this, a2: 0);`
  - `*(this + 315) = -1;`
  - `CAvatar::ClearActionLayer(this, a2: 1);`
  - `(*(*this + 20))(a1: this, a2: 6, a3: 100, a4: 0);`
  - `if ( *(this + 296) == 0 )`
  - `CAvatar::SetEmotion(this, nEmotion: 0, tDuration: -1);`
  - `if ( a2 == 1002 )`
  - `if ( *(this + 1072) == 0 )`
  - `v62 = sub_432CC2(a1: *(this + 1072));`
  - `v4 = *(this + 1072);`
  - `v5 = (*(*v4 + 224))(a1: v4, a2: 0xFFFFFF);`
  - `if ( v5 < 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - …（截断 16 行）

#### CAvatar::SetMoveAction  (937 B)

  - `void __thiscall CAvatar::SetMoveAction(CAvatar *this, int a2, int a3)`
  - `if ( a3 != 0 || a2 != *(this + 314) )`
  - `*(this + 314) = a2;`
  - `if ( (a2 & 0xFFFFFFFE) == 0x12 )`
  - `*(this + 315) = -1;`
  - `CAvatar::ClearActionLayer(this, a2: 1);`
  - `if ( CAvatar::GetOneTimeAction(this) <= -1 )`
  - `(*(*this + 20))(a1: this, a2: 6, a3: 100, a4: 0);`
  - `v4 = sub_451E4C(this, a2: 0);`
  - `if ( *(this + 296) != 0 )`
  - `if ( v4 != 8 && v4 != 20 )`
  - `if ( _ZtlSecureFuse<long>(a1: this + 1168, a2: *(this + 294)) == 8 )`
  - `CAvatar::SetEmotion(this, nEmotion: 0, tDuration: -1);`
  - `if ( v4 == 33 || v4 == 32 )`

#### CAvatar::SetRidingVehicle  (357 B)

  - `int __userpurge CAvatar::SetRidingVehicle@<eax>(CAvatar *this@<ecx>, int a2, int a3)`
  - `if ( *(this + 280) == 0 )`
  - `*(this + 303) = a2;`
  - `CAvatar::ClearActionLayer(this, a2: 0);`
  - `return (*(*this + 20))(a1: this, a2: 6, a3: 100, a4: 0);`

#### CAvatar::SetVisibleMan  (1206 B)

  - `int __thiscall CAvatar::SetVisibleMan(CAvatar *this, int a2)`
  - `if ( *(this + 1074) == 0 )`
  - `v4 = *(this + 1074);`
  - `v5 = (*(*v4 + 224))(a1: v4, a2: a2 != 0 ? -1 : 0xFFFFFF);`
  - `if ( v5 < 0 )`
  - `if ( *(this + 1073) == 0 )`
  - `v6 = *(this + 1073);`
  - `v7 = (*(*v6 + 224))(a1: v6, a2: v3);`
  - `if ( v7 < 0 )`
  - `if ( *(this + 1072) == 0 )`
  - `v8 = *(this + 1072);`
  - `result = (*(*v8 + 224))(a1: v8, a2: v3);`
  - `if ( result < 0 )`

#### CAvatar::SetVisibleTamingMob  (904 B)

  - `BOOL __thiscall CAvatar::SetVisibleTamingMob(CAvatar *this, int a2)`
  - `if ( *(this + 1076) == 0 )`
  - `v3 = *(this + 1076);`
  - `v4 = (*(*v3 + 224))(a1: v3, a2: a2 != 0 ? -1 : 0xFFFFFF);`
  - `if ( v4 < 0 )`
  - `if ( *(this + 1077) == 0 )`
  - `v5 = *(this + 1077);`
  - `v6 = (*(*v5 + 224))(a1: v5, a2: a2 != 0 ? -1 : 0xFFFFFF);`
  - `if ( v6 < 0 )`
  - `*(this + 302) = result;`

#### CAvatar::Update  (38033 B)

  - `__int16 __fastcall CAvatar::Update(CAvatar *this, int a2)`
  - `v4 = *(this + 312) == 0;`
  - `if ( !v4 )`
  - `update_time = get_update_time();`
  - `v170 = sub_451E4C(this, a2: 0);`
  - `v2 = *(this + 296);`
  - `if ( v2 != 0 )`
  - `if ( v170 >= 46 )`
  - `else if ( v170 >= 162 )`
  - `v5 = *(this + 313);`
  - `if ( v5 > 0 )`
  - `*(this + 313) = v6;`
  - `if ( v6 < 0 )`
  - `v7 = *(this + 314);`
  - `*(this + 313) = 0;`
  - `if ( (v7 & 0xFFFFFFFE) == 8 )`
  - …（截断 16 行）

### CAvatarMegaphone（8 函数）


#### CAvatarMegaphone::Draw  (7943 B)

  - `const struct tagRECT *__thiscall CAvatarMegaphone::Draw(CAvatarMegaphone *this, const struct tagRECT *a2)`
  - `CWnd::Draw(this, a2);`
  - `CWnd::GetCanvas(a1: &a2);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v52) = 1;`
  - `sub_402F85(pvargDest: &v47, pvargSrc: &pvargSrc);`
  - `LOBYTE(v52) = 2;`
  - `v44 = sub_4284A5(this: this + 612);`
  - `v3 = sub_428226(a1: this + 596);`
  - `_bstr_t::_bstr_t(a1: v3);`
  - `v5 = sub_414576(a1: &a2);`
  - `IWzCanvas::DrawTextA(this: v5, nLeft: 124, nTop: 14, sText: v43, pFont: v44, vAlpha: p_pvargDest, vTabOrg: p_pvarg);`
  - `if ( v6 < 0 )`
  - `LOBYTE(v52) = 0;`
  - `if ( v7 < 0 )`
  - `LOBYTE(v52) = 4;`
  - …（截断 16 行）

#### CAvatarMegaphone::HelloAvatarMegaphone  (9636 B)

  - `void __thiscall CAvatarMegaphone::HelloAvatarMegaphone(CAvatarMegaphone *this)`
  - `v53 = abs(a1: 225);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v57) = 1;`
  - `Layer = CWnd::GetLayer(a1: &v52);`
  - `LOBYTE(v57) = 2;`
  - `if ( v2 )`
  - `v5 = (*(v4 + 144))(`
  - `if ( v5 < 0 )`
  - `sub_4156C5(a1: &v52);`
  - `LOBYTE(v57) = 0;`
  - `if ( v6 < 0 )`
  - `if ( v7 < 0 )`
  - `v8 = CWnd::GetLayer(a1: &v51);`
  - `LOBYTE(v57) = 4;`
  - …（截断 16 行）

#### CAvatarMegaphone::OnCreate  (13892 B)

  - `_DWORD *__thiscall CAvatarMegaphone::OnCreate(CAvatarMegaphone *this, void *a2)`
  - `CItemInfo::GetItemInfo(a1: &v79, a2: *(this + 27));`
  - `_bstr_t::_bstr_t(a1: str_Pa_4);`
  - `LOBYTE(v80) = 0;`
  - `sub_40263B(a1: &v79);`
  - `Item = IWzProperty::GetItem(pvargDest: &pvarg, a2: p_pvarg);`
  - `LOBYTE(v80) = 2;`
  - `if ( v5 )`
  - `v6 = *(Item + 8);`
  - `sub_41C06C(a1: v6, a2: -1);`
  - `if ( v7 < 0 )`
  - `_bstr_t::_bstr_t(a1: str_Em);`
  - `v9 = IWzProperty::GetItem(pvargDest: &pvarg, a2: v70);`
  - `LOBYTE(v80) = 4;`
  - `int32 = get_int32(pvarSrc: v9, a2: p_pvarg);`
  - `*(this + 146) = int32;`
  - …（截断 16 行）

#### CAvatarMegaphone::OnDestroy  (88 B)

  - `void __thiscall CAvatarMegaphone::OnDestroy(CAvatarMegaphone *this)`

#### CAvatarMegaphone::OnKey  (75 B)

  - `void __stdcall CAvatarMegaphone::OnKey(int a1, int a2)`

#### CAvatarMegaphone::OnMouseButton  (782 B)

  - `void __stdcall CAvatarMegaphone::OnMouseButton(int a1, int a2, int a3, int a4)`
  - `if ( a1 == 517 )`
  - `if ( *dword_BE7918 != 0 )`
  - `CWndMan::GetCursorPos(this: TSingleton<CWndMan>::ms_pInstance, a2: &v5, a3: 0);`
  - `CTemporaryStatView::FindIcon(this: (v4 + 11944), a2: &v5, a3: &v6, a4: &a1);`
  - `if ( v6 == 1 )`
  - `CWvsContext::SendStatChangeItemCancelRequest(this: v4, a2: -a1);`
  - `else if ( v6 == 2 && a1 != 1311006 )`
  - `CUserLocal::SendSkillCancelRequest(a1);`

#### CAvatarMegaphone::OnMouseMove  (337 B)

  - `int __stdcall CAvatarMegaphone::OnMouseMove(int a1, int a2)`
  - `CWndMan::GetCursorPos(this: TSingleton<CWndMan>::ms_pInstance, a2: &v3, a3: 0);`
  - `CUserLocal::OnMouseMove(this: TSingleton<CUserLocal>::ms_pInstance, a2: v3.x, a3: v3.y);`

#### CAvatarMegaphone::Update  (114 B)

  - `void __thiscall CAvatarMegaphone::Update(_DWORD *this)`
  - `CWnd::Update(this);`

### CUIAvatar（5 函数）


#### CUIAvatar::DrawNameTag  (7181 B)

  - `_DWORD *__thiscall CUIAvatar::DrawNameTag(CUIAvatar *this, int a2, int a3)`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `v4 = *(this + a2 + 49) == 0;`
  - `v5 = (this + 4 * a2 + 196);`
  - `if ( v4 )`
  - `IWzGr2DLayer::GetCanvas(a1: &v43, a2: &pvarg);`
  - `LOBYTE(v44) = 2;`
  - `if ( v6 < 0 )`
  - `if ( *v5 == 0 )`
  - `Width = IWzGr2DLayer::GetWidth(a1: *v5);`
  - `Height = IWzGr2DLayer::GetHeight(a1: *v5);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `HIDWORD(int64) = &pvargDest;`
  - `LOBYTE(v44) = 3;`
  - `v9 = *(this + sub_605F09(a1: a2) + 28);`
  - `LODWORD(int64) = v10;`
  - …（截断 16 行）

#### CUIAvatar::OnCreate  (25020 B)

- SP_ 节点：`SP_1315_UI_LOGINIMG_CHARSELECT_PAGEL_`, `SP_1316_UI_LOGINIMG_CHARSELECT_PAGER_`, `SP_1323_UI_LOGINIMG_CHARSELECT_NAMETAG`, `SP_1410_CANVASFONT`, `SP_4421_USE_THE_TAB_KEY_TO_TOGGLE_THE_MENU`, `SP_5527_ARIAL`
  - `_DWORD *__thiscall CUIAvatar::OnCreate(CUIAvatar *this, int **a2)`
  - `sub_402F85(pvargDest: &v129, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v128, pvargSrc: &pvargSrc);`
  - `LOBYTE(v143) = 1;`
  - `v5 = sub_406472();`
  - `v6 = sub_402680(a1: v5);`
  - `IWzResMan::GetObjectA(this: v6, pvargDest: &v127, sUOL: v122, vParam: v123, vAux: v124);`
  - `LOBYTE(v143) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v125, a2: v126);`
  - `if ( Unknown != 0 )`
  - `v7 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v7 < 0 && v7 != -2147467262 )`
  - `LOBYTE(v143) = 7;`
  - `if ( v8 < 0 )`
  - `LOBYTE(v143) = 6;`
  - `if ( v9 < 0 )`
  - …（截断 16 行）

#### CUIAvatar::Refresh  (324 B)

  - `int __thiscall CUIAvatar::Refresh(CUIAvatar *this)`
  - `for ( i = 0; i < 3; ++i )`
  - `result = CUIAvatar::ResetCharacter(this, a2: i, a3: *(this + 2 * *(this + 76) + i + *(this + 76) + 28), a4: v4);`

#### CUIAvatar::ResetCharacter  (17301 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1317_UI_LOGINIMG_CHARSELECT_CHARACTER_0`, `SP_1318_UI_LOGINIMG_CHARSELECT_CHARACTER_1`, `SP_1324_UI_LOGINIMG_CHARSELECT_KNIGHT`, `SP_5121_UI_LOGINIMG_CHARSELECT_ARAN`, `SP_5122_UI_LOGINIMG_CHARSELECT_ADVENTURE`
  - `int __userpurge CUIAvatar::ResetCharacter@<eax>(CUIAvatar *this@<ecx>, int a2, struct AvatarData *a3, int a4)`
  - `if ( *a3 != 0 )`
  - `ZRef<CAvatar>::_Alloc(a1: v31);`
  - `v93 = *(v31 + 1);`
  - `HIDWORD(v83) = 80;`
  - `LODWORD(v83) = 125 * a2 + 170;`
  - `CWnd::GetLayer(a1: &v81);`
  - `Layer = CWnd::GetLayer(a1: &v94);`
  - `LOBYTE(v99) = 15;`
  - `sub_428EC4(a1: Layer);`
  - `LOBYTE(v99) = 17;`
  - `v35 = sub_426626(a1: 2, a2: 0);`
  - `CAvatar::Init(`
  - `y: HIDWORD(v83),`
  - `sub_4156C5(a1: &v94);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - …（截断 16 行）

#### CUIAvatar::SelectCharacter  (1090 B)

  - `void __thiscall CUIAvatar::SelectCharacter(CUIAvatar *this, int a2)`
  - `if ( a2 <= 0xE && **(this + a2 + 28) != 0 )`
  - `*(*(this + 27) + 400) = a2;`
  - `CLogin::OnSelectedCharChanged(this: *(this + 27), a2);`
  - `*(*(this + 27) + 400) = -1;`
  - `CLogin::OnSelectedCharChanged(this: *(this + 27), a2: -1);`
  - `if ( *(this + 76) != a2 / 3 )`
  - `*(this + 76) = a2 / 3;`
  - `CUIAvatar::Refresh(this);`
  - `if ( a2 >= 0 )`
  - `v4 = (this + 176);`
  - `if ( *v4 != 0 )`
  - `(*(**v4 + 16))(a1: *v4, a2: 2 * ((v5 != v2) + 1), a3: 0);`
  - `CUIAvatar::DrawNameTag(this, a2: v5, a3: v5 == v2);`
  - `while ( v5 < 3 );`
  - `CWnd::InvalidateRect(this, a2: 0);`
  - …（截断 16 行）

### VCAvatar（1 函数）


#### VCAvatar::0?$ZRefCountedDummy  (374 B)

  - `_DWORD *__thiscall ZRefCountedDummy<CAvatar>::ZRefCountedDummy<CAvatar>(_DWORD *this)`
  - `memset(a1: this, a2: 0, a3: 0xCu);`
  - `*(this + 3) = &off_AF0C68;`
  - `CAvatar::CAvatar(this: (this + 4));`
  - `*(this + 3) = &off_AF0C60;`

### CUIAvatarVAC（13 函数）


#### CUIAvatarVAC::DrawWorldIcon  (4073 B)

  - `int __thiscall CUIAvatarVAC::DrawWorldIcon(CUIAvatarVAC *this, int a2)`
  - `result = sub_60A485(a1: a2);`
  - `if ( result != 0 )`
  - `v4 = a2 - 3 * CUIAvatarVAC::GetScrollBarIdx(this);`
  - `v5 = 117 * (v4 % 3) + 74;`
  - `v20 = 117 * (v4 / 3) + 150;`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `v6 = *(this + a2 + 3740) == 0;`
  - `v21 = (this + 4 * a2 + 14960);`
  - `if ( v6 )`
  - `IWzGr2DLayer::GetCanvas(a1: &v23, a2: &v18);`
  - `LOBYTE(v24) = 2;`
  - `if ( v7 < 0 )`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `v25 = *(this + *(*(*(this + 27) + 328) + 4 * a2) + 3808);`
  - `LOBYTE(v24) = 3;`
  - …（截断 16 行）

#### CUIAvatarVAC::GetScrollBarIdx  (112 B)

  - `int __thiscall CUIAvatarVAC::GetScrollBarIdx(CUIAvatarVAC *this)`
  - `return *(*(this + 33) + 56);`

#### CUIAvatarVAC::GetSelectedIdx  (1733 B)

  - `int __thiscall CUIAvatarVAC::GetSelectedIdx(CUIAvatarVAC *this, struct tagPOINT a2)`
  - `v14 = (this + 14240);`
  - `while ( 1 )`
  - `if ( *v14 != 0 )`
  - `Layer = CWnd::GetLayer(a1: v12);`
  - `if ( v3 )`
  - `v4 = IWzShape2D::Getx(a1: *Layer);`
  - `if ( v12[0] != 0 )`
  - `(*(*v12[0] + 8))(a1: v12[0]);`
  - `v6 = CWnd::GetLayer(a1: &v11);`
  - `v7 = IWzShape2D::Gety(a1: *v6);`
  - `if ( v11 != 0 )`
  - `(*(*v11 + 8))(a1: v11);`
  - `CAvatar::GetBodyRect(this: *v14, a2: v10, a3: 0);`
  - `if ( dword_BF0484(a1: v10, a2: v5, a3: v8) != 0 )`
  - `if ( v13 >= 60 )`
  - …（截断 16 行）

#### CUIAvatarVAC::IsRequestValid  (180 B)

  - `BOOL __thiscall CUIAvatarVAC::IsRequestValid(CUIAvatarVAC *this)`
  - `v1 = *(this + 27);`
  - `return *(v1 + 368) == 0 && *(v1 + 360) == 5;`

#### CUIAvatarVAC::MakeCharacter  (7891 B)

  - `int __thiscall CUIAvatarVAC::MakeCharacter(CUIAvatarVAC *this, int a2, struct AvatarData *a3)`
  - `sub_5F9FCA(a1: a3);`
  - `v48 = (117 * (a2 / 3) + 150);`
  - `ZRef<CAvatar>::_Alloc(a1: this + 8 * a2 + 14236);`
  - `v38 = 117 * (a2 % 3) + 74;`
  - `v4 = *(this + 2 * a2 + 3560);`
  - `CWnd::GetLayer(a1: &v36);`
  - `Layer = CWnd::GetLayer(a1: &v49);`
  - `LOBYTE(v51) = 1;`
  - `sub_428EC4(a1: Layer);`
  - `LOBYTE(v51) = 2;`
  - `v8 = sub_426626(a1: 2, a2: 0);`
  - `CAvatar::Init(`
  - `sub_4156C5(a1: &v49);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `_bstr_t::_bstr_t(a1: a3 + 4);`
  - …（截断 16 行）

#### CUIAvatarVAC::MakeSelectedCharacterNameTag  (4840 B)

  - `int __thiscall CUIAvatarVAC::MakeSelectedCharacterNameTag(CUIAvatarVAC *this)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v29) = 1;`
  - `if ( dword_BF14EC == 0 )`
  - `v2 = IWzGr2D::CreateLayer(`
  - `v3 = (this + 14956);`
  - `sub_428712(a1: *v2);`
  - `sub_4156C5(a1: &v27);`
  - `LOBYTE(v29) = 0;`
  - `if ( v4 < 0 )`
  - `if ( v5 < 0 )`
  - `if ( *v3 == 0 )`
  - `v7 = (*(**v3 + 224))(a1: *v3, a2: -1);`
  - `if ( v7 < 0 )`
  - `v20 = *CWnd::GetLayer(a1: &v26);`
  - …（截断 16 行）

#### CUIAvatarVAC::MakeWorldIcon  (4982 B)

  - `void __thiscall CUIAvatarVAC::MakeWorldIcon(CUIAvatarVAC *this, int a2)`
  - `v3 = *(*(*(this + 27) + 328) + 4 * a2);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v30) = 1;`
  - `if ( dword_BF14EC == 0 )`
  - `v4 = IWzGr2D::CreateLayer(`
  - `z: *(this + v3 + 3808) != 0,`
  - `v5 = (this + 4 * a2 + 14960);`
  - `sub_428712(a1: *v4);`
  - `sub_4156C5(a1: &v28);`
  - `LOBYTE(v30) = 0;`
  - `if ( v6 < 0 )`
  - `if ( v7 < 0 )`
  - `if ( *v5 == 0 )`
  - `v9 = (*(**v5 + 224))(a1: *v5, a2: -1);`
  - …（截断 16 行）

#### CUIAvatarVAC::OnButtonClicked  (319 B)

  - `int __thiscall CUIAvatarVAC::OnButtonClicked(CLogin **this, unsigned int a2)`
  - `result = CUIAvatarVAC::IsRequestValid(this);`
  - `if ( result != 0 && a2 == 1001 )`
  - `return CLogin::SendSelectCharPacketByVAC(this: *(this + 27));`

#### CUIAvatarVAC::OnChildNotify  (892 B)

  - `void __thiscall CUIAvatarVAC::OnChildNotify(CUIAvatarVAC *this, unsigned int a2, unsigned int a3, unsigned int a4)`
  - `if ( a2 == 1002 && a3 >= 0x12C && a3 <= 0x130 )`
  - `v5 = *(*(this + 27) + 400);`
  - `v6 = v5 - 3 * CUIAvatarVAC::GetScrollBarIdx(this);`
  - `if ( v6 <= 8 )`
  - `if ( v6 >= 0 )`
  - `CUIAvatarVAC::SetSelection(this, a2: v5);`
  - `ScrollBarIdx = CUIAvatarVAC::GetScrollBarIdx(this);`
  - `ScrollBarIdx = CUIAvatarVAC::GetScrollBarIdx(this) + 2;`
  - `CUIAvatarVAC::SetSelection(this, a2: v5 % 3 + 3 * ScrollBarIdx);`
  - `else if ( a3 == 100 )`
  - `(*(*this + 32))(a1: this, a2);`

#### CUIAvatarVAC::OnCreate  (15603 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1323_UI_LOGINIMG_CHARSELECT_NAMETAG`, `SP_1410_CANVASFONT`, `SP_3983_UI_LOGINIMG_VIEWALLCHAR_BTENTER`, `SP_3986_UI_LOGINIMG_VIEWALLCHAR_WORLDICONS_D`, `SP_3987_UI_LOGINIMG_VIEWALLCHAR_BACKGRND`, `SP_5527_ARIAL`
  - `void __thiscall CUIAvatarVAC::OnCreate(CUIAvatarVAC *this, struct AvatarData *a2)`
  - `_bstr_t::_bstr_t(this: &v95, a2: dword_AF7074);`
  - `LOBYTE(v97) = 1;`
  - `v4 = sub_44427B(a1: v94, a2: &v95);`
  - `LOBYTE(v97) = 2;`
  - `sub_48EC62(a1: v4);`
  - `CWnd::SetBackgrnd(a1: v78, a2: v79, a3: v80);`
  - `sub_402D9A(this: v94);`
  - `sub_402D9A(this: &v96);`
  - `sub_402D9A(this: &v95);`
  - `LOBYTE(v97) = 4;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `LOBYTE(v97) = 3;`
  - `v8 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v97) = 5;`
  - `if ( v8 != 0 )`
  - …（截断 16 行）

#### CUIAvatarVAC::OnMouseButton  (646 B)

  - `void __thiscall CUIAvatarVAC::OnMouseButton(CUIAvatarVAC *this, unsigned int a2, unsigned int a3, struct tagPOINT a4)`
  - `if ( a2 == 513 )`
  - `SelectedIdx = CUIAvatarVAC::GetSelectedIdx(this: (this - 4), a2: a4);`
  - `if ( SelectedIdx >= 0 )`
  - `*(*(this + 26) + 400) = SelectedIdx;`
  - `CUIAvatarVAC::SetSelection(this: (this - 4), a2: SelectedIdx);`
  - `else if ( a2 == 515 && CUIAvatarVAC::GetSelectedIdx(this: (this - 4), a2: a4) >= 0 )`
  - `CLogin::SendSelectCharPacketByVAC(this: *(this + 26));`

#### CUIAvatarVAC::ResetAvatarPosition  (533 B)

  - `int __thiscall CUIAvatarVAC::ResetAvatarPosition(CUIAvatarVAC *this)`
  - `result = *(this + 27);`
  - `if ( *(result + 300) > 0 )`
  - `v4 = (this + 136);`
  - `sub_60A020(this, a2: v3, a3: *v4);`
  - `while ( v3 < *(result + 300) );`

#### CUIAvatarVAC::SetSelection  (1146 B)

  - `void __thiscall CUIAvatarVAC::SetSelection(CUIAvatarVAC *this, int a2)`
  - `if ( a2 >= 0 && (v4 = *(this + 27), a2 < *(v4 + 300)) && *(this + 231 * a2 + 376) != 0 )`
  - `*(v4 + 400) = a2;`
  - `v6 = a2 - 3 * CUIAvatarVAC::GetScrollBarIdx(this);`
  - `CLogin::OnSelectedCharChanged(this: *(this + 27), a2: v6);`
  - `*(*(this + 27) + 400) = -1;`
  - `CLogin::OnSelectedCharChanged(this: *(this + 27), a2: -1);`
  - `if ( *(*(this + 27) + 300) > 0 )`
  - `v5 = (this + 14240);`
  - `if ( *v5 != 0 )`
  - `(*(**v5 + 16))(a1: *v5, a2: 2 * ((v7 != a2) + 1), a3: 0);`
  - `while ( v7 < *(*(this + 27) + 300) );`
  - `CUIAvatarVAC::ResetAvatarPosition(this);`
  - `CUIAvatarVAC::MakeSelectedCharacterNameTag(this);`
  - `CWnd::InvalidateRect(this, a2: 0);`

### CUIAvatarMegaphone（6 函数）


#### CUIAvatarMegaphone::Draw  (141 B)

  - `void *__thiscall CUIAvatarMegaphone::Draw(CUIAvatarMegaphone *this, const struct tagRECT *a2)`
  - `return CWnd::Draw(this, a2);`

#### CUIAvatarMegaphone::GetText  (2901 B)

  - `void __thiscall CUIAvatarMegaphone::GetText(char **this, int a2, int a3, int a4, int a5)`
  - `memset(a1: v14, a2: 0, a3: sizeof(v14));`
  - `memset(a1: v15, a2: 0, a3: sizeof(v15));`
  - `v6 = *(this + 40);`
  - `v7 = *(v6 + 68);`
  - `if ( v7 != 0 )`
  - `v18 = *(v7 - 4);`
  - `if ( v18 > 5 )`
  - `for ( i = 0; i < v18; ++i )`
  - `if ( i != 0 )`
  - `v14[i] = *(*(v6 + 68) + 4 * i);`
  - `if ( v9 )`
  - `v15[i] = *(*(v6 + 68) + 4 * i + 4);`
  - `if ( v18 == 1 )`
  - `v15[0] = *(*(v6 + 68) + 4);`
  - `CCtrlMLEdit::GetText(this: v6, a2: &v17);`
  - …（截断 16 行）

#### CUIAvatarMegaphone::HitTest  (438 B)

  - `int __thiscall CUIAvatarMegaphone::HitTest(CUIAvatarMegaphone *this, int a2, int a3, struct CCtrlWnd **a4)`
  - `result = CWnd::HitTest(a1: a2, a2: a3, a3: a4);`
  - `if ( result != 0 )`
  - `if ( (a4 == 0 || *a4 == 0) && *(this + 15) != 0 )`
  - `if ( a4 == 0 || *a4 == 0 )`

#### CUIAvatarMegaphone::IsCheckWhisper  (231 B)

  - `int __thiscall CUIAvatarMegaphone::IsCheckWhisper(CUIAvatarMegaphone *this)`
  - `v1 = *(this + 42);`
  - `if ( v1 != 0 )`
  - `return *(v1 + 72);`

#### CUIAvatarMegaphone::OnChildNotify  (556 B)

  - `int __thiscall CUIAvatarMegaphone::OnChildNotify(`
  - `CWnd::GetCanvas(a1: &v6);`
  - `if ( a3 == 100 )`
  - `(*(*this + 32))(a1: this, a2);`
  - `if ( v6 != 0 )`
  - `return (*(*v6 + 8))(a1: v6);`

#### CUIAvatarMegaphone::OnCreate  (4642 B)

- SP_ 节点：`SP_3974_UI_UIWINDOWIMG_AVATARMEGAPHONE`, `SP_5527_ARIAL`, `SP_966_SS`
  - `void __thiscall CUIAvatarMegaphone::OnCreate(CUIAvatarMegaphone *this, void *a2)`
  - `LOBYTE(v25) = 2;`
  - `sub_416821(a1: &v21, a2: v5, a3: v24[0]);`
  - `LOBYTE(v25) = 1;`
  - `v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v25) = 3;`
  - `if ( v6 != 0 )`
  - `v7 = CCtrlButton::CCtrlButton(this: v6);`
  - `sub_4284FF(a1: v7);`
  - `(*(**(this + 33) + 32))(a1: *(this + 33), a2: this, a3: 1, a4: 52, a5: 170, a6: 0, a7: v20);`
  - `LOBYTE(v25) = 4;`
  - `sub_416821(a1: &v21, a2: v9, a3: v24[0]);`
  - `v10 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v25) = 5;`
  - `if ( v10 != 0 )`
  - `v11 = CCtrlButton::CCtrlButton(this: v10);`
  - …（截断 16 行）

### CQuickslotKeyMappedMan（3 函数）


#### CQuickslotKeyMappedMan::DefaultQuickslotKeyMap  (170 B)

  - `void *__thiscall CQuickslotKeyMappedMan::DefaultQuickslotKeyMap(CQuickslotKeyMappedMan *this)`
  - `return memcpy(a1: this + 4, a2: dword_BDAFAC, a3: 0x20u);`

#### CQuickslotKeyMappedMan::OnInit  (574 B)

  - `int __thiscall CQuickslotKeyMappedMan::OnInit(char *this, int a2)`
  - `if ( CInPacket::Decode1(this: a2) != 0 )`
  - `CInPacket::DecodeBuffer(a1: this + 4, a2: 0x20u);`
  - `CQuickslotKeyMappedMan::DefaultQuickslotKeyMap(a1: this);`
  - `if ( TSingleton<CUIStatusBar>::ms_pInstance != 0 )`
  - `CUIStatusBar::ReDrawQuickslot(this: TSingleton<CUIStatusBar>::ms_pInstance);`
  - `memcpy(a1: this + 36, a2: this + 4, a3: 0x20u);`
  - `return CQuickslotKeyMappedMan::SaveQuickslotKeyMap(a1: this);`

#### CQuickslotKeyMappedMan::SaveQuickslotKeyMap  (570 B)

  - `int __thiscall CQuickslotKeyMappedMan::SaveQuickslotKeyMap(CQuickslotKeyMappedMan *this)`
  - `COutPacket::COutPacket(a1: 183);`
  - `COutPacket::EncodeBuffer(a1: this + 4, a2: 0x20u);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: v3);`
  - `memcpy(a1: this + 36, a2: this + 4, a3: 0x20u);`
  - `return ZArray<unsigned char>::RemoveAll(a1: v4);`

### CDraggableItem（11 函数）


#### CDraggableItem::CanPerformAction  (1328 B)

- SP_ 节点：`SP_266_YOU_CANT_DO_IT_RIGHT_NOW_R_NCATCH_YOUR_BREATH_AND_THEN_TRY_AGAIN`, `SP_66_YOU_CANT_DO_IT_ONCE_YOURE_DEAD`
  - `int __stdcall CDraggableItem::CanPerformAction(int a1)`
  - `if ( TSingleton<CUserLocal>::ms_pInstance == 0 )`
  - `if ( a1 != 0 && *(TSingleton<CUserLocal>::ms_pInstance + 1388) > 0 )`
  - `CUtilDlg::Notice(sMsg: v4, sSndName: v5, ppDialog: v6, bAutoSeparated: v7, bIsTightLine: v8);`
  - `if ( (*(TSingleton<CUserLocal>::ms_pInstance + 1392) & 0xFFFFFFFE) == 0x12 )`

#### CDraggableItem::GetOffEquipItem  (15177 B)

- SP_ 节点：`SP_4266_YOU_MAY_ONLY_EQUIP_ONE_WEDDING_RING`, `SP_869_ONLY_ONE_COUPLE_RING_CAN_BE_EQUIPPED`, `SP_870_YOU_MAY_ONLY_EQUIP_R_NONE_FRIENDSHIP_RING_AT_A_TIME`, `SP_878_YOU_CANNOT_CHANGE_YOUR_EQUIPMENT_ON_THIS_MAP`
  - `signed int __thiscall CDraggableItem::GetOffEquipItem(int *this, int a2, int a3, FILETIME *a4, int a5)`
  - `void (__thiscall *v60)(void *); // [esp-4h] [ebp-4DCh]`
  - `void (__thiscall *v69)(void *); // [esp+4A4h] [ebp-34h]`
  - `if ( dword_BE7910 != 0 )`
  - `if ( CDraggableItem::CanPerformAction(a1: 1) == 0 )`
  - `v6 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v67) + 4);`
  - `sub_4289B7(this: v67);`
  - `if ( CWvsContext::CanSendExclRequest(a1: 500, a2: 0) == 0 || *(this + 6) != 1 )`
  - `if ( *(a3 + 1508) != 1 )`
  - `CUIItem::ChangeTab(this: a3, a2: 0);`
  - `CWndMan::BeginDragDrop(a1: *(this + 9), a2: this);`
  - `SlotPositionFromPoint = CUIItem::GetSlotPositionFromPoint(this: a3, rx: a4, ry: a5);`
  - `if ( SlotPositionFromPoint == 0 )`
  - `v7 = *(CharacterData::GetItem(this: v6, a2: v68, a3: 1, a4: *(this + 7)) + 4);`
  - `sub_4288C1(a1: v68);`
  - `v80 = *(CharacterData::GetItem(this: v6, a2: v68, a3: 1, a4: SlotPositionFromPoint) + 4);`
  - …（截断 16 行）

#### CDraggableItem::MapFuncKey  (5010 B)

  - `signed int __thiscall CDraggableItem::MapFuncKey(CDraggableItem *this, int a2, int a3, int a4)`
  - `v5 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v25) + 4);`
  - `if ( v26 != 0 )`
  - `sub_428C44(this: &v25, a2: 0);`
  - `v6 = *(CharacterData::GetItem(this: v5, a2: &v25, a3: *(v4 + 6), a4: *(v4 + 7)) + 4);`
  - `ZRef<GW_ItemSlotBase>::~ZRef<GW_ItemSlotBase>(a1: 0);`
  - `if ( v6 == 0 )`
  - `Data = TSecType<long>::GetData(a1: (v6 + 12), a2: 0);`
  - `v8 = *(v4 + 6);`
  - `if ( v8 == 2 )`
  - `if ( v9 == 200`
  - `if ( v8 == 5 )`
  - `if ( v31 / 10000 == 524 || v31 / 10000 == 530 )`
  - `if ( get_etc_cash_item_type(a1: v31) == 6 )`
  - `v10 = *(v4 + 6);`
  - `if ( v10 == 5 && v31 / 10000 == 501 || v10 == 4 && v31 / 10000 == 429 )`
  - …（截断 16 行）

#### CDraggableItem::MoveItemSlot  (1700 B)

  - `signed int __thiscall CDraggableItem::MoveItemSlot(int *this, int *a2, int a3, int a4)`
  - `if ( dword_BE7910 != 0 )`
  - `if ( CDraggableItem::CanPerformAction(a1: 1) == 0 )`
  - `if ( CWvsContext::CanSendExclRequest(a1: 500, a2: 0) == 0 )`
  - `SlotPositionFromPoint = CUIItem::GetSlotPositionFromPoint(this: a2, rx: a3, ry: a4);`
  - `if ( SlotPositionFromPoint == 0 )`
  - `v6 = *(CWvsContext::GetCharacterData(this: v5, a2: &v10) + 4);`
  - `if ( v11 != 0 )`
  - `sub_428C44(this: &v10, a2: 0);`
  - `v7 = *(this + 6);`
  - `if ( a2[377] != v7 || *(this + 7) == SlotPositionFromPoint )`
  - `v8 = *(CharacterData::GetItem(this: v6, a2: &v10, a3: v7, a4: *(this + 7)) + 4);`
  - `ZRef<GW_ItemSlotBase>::~ZRef<GW_ItemSlotBase>(a1: 0);`
  - `if ( v8 == 0 || !sub_4F3144(ebx0: 0, a2: v8) && (*(v8->dwLowDateTime + 32))(a1: v8) <= 0 )`
  - `CWvsContext::SendChangeSlotPositionRequest(`
  - `a2: *(this + 6),`
  - …（截断 16 行）

#### CDraggableItem::MoveItemToInventory  (887 B)

  - `int __thiscall CDraggableItem::MoveItemToInventory(int *this, _DWORD *pPersonalShopDlg, int pUIItem, int x, int y)`
  - `if ( CDraggableItem::CanPerformAction(a1: 1) == 0 )`
  - `v6 = *(this + 9);`
  - `if ( v6 != 0 && (*(*v6 + 72))(a1: v6, a2: &dword_BEDCA0) != 0 && (v7 = *(this + 9)) != 0 )`
  - `if ( v8 == 0 )`
  - `v9 = *(this + 6);`
  - `if ( *(pUIItem + 1508) != v9 )`
  - `CUIItem::ChangeTab(this: pUIItem, a2: v9 - 1);`
  - `CWndMan::BeginDragDrop(a1: *(this + 9), a2: this);`
  - `return CPersonalShopDlg::MoveItemToInventory(this: pPersonalShopDlg, a2: *(this + 8));`

#### CDraggableItem::OnDoubleClicked  (31472 B)

- 分支 case：`2`, `3`, `4`
- SP_ 节点：`SP_1234_DRAGEND`, `SP_150_NOT_AVAILABLE`, `SP_3416_SEALED_ITEMS_CANNOT_BE_R_NSOLD_TRADED_OR_DROPPED`, `SP_386_PLEASE_SELECT_THE_PET_YOU_WOULD_LIKE_TO_PUT_THE_PET_EQUIPMENT_ON`, `SP_4378_TO_HATCH_PIGMY_EGG_YOU_NEED_TO_R_NPURCHASE_THE_INCUBATOR_FROM_THE_CASH_SHOP`, `SP_5213_IF_YOU_USE_THIS_ITEM_YOU_WILL_NOT_BE_ABLE_R_NTO_TRADE_THE_ITEM_WITH_OTHER_USERS_`
  - `int __thiscall CDraggableItem::OnDoubleClicked(_DWORD *this)`
  - `if ( CDraggableItem::CanPerformAction(a1: 0) == 0 )`
  - `v3 = *(this + 9);`
  - `if ( v3 != 0 && (*(*v3 + 72))(a1: v3, a2: &dword_BF0EB0) != 0 && (v4 = *(this + 9)) != 0 )`
  - `if ( v5 == 0 )`
  - `v90 = *(this + 9);`
  - `if ( v90 != 0 && (*(*v90 + 72))(a1: v90, a2: &dword_BF0E48) != 0 && (v91 = *(this + 9)) != 0 )`
  - `if ( v92 != 0 )`
  - `if ( *(this + 7) == -18 )`
  - `v93 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v121) + 4);`
  - `if ( v122 != 0 )`
  - `sub_428C44(this: &v121, a2: 0);`
  - `v94 = *(CharacterData::GetItem(this: v93, a2: &v121, a3: 1, a4: *(this + 7)) + 4);`
  - `ZRef<GW_ItemSlotBase>::~ZRef<GW_ItemSlotBase>(a1: 0);`
  - `if ( v94 != 0 )`
  - `Data = TSecType<long>::GetData(a1: (v94 + 12), a2: 0);`
  - …（截断 16 行）

#### CDraggableItem::OnDropped  (16474 B)

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
  - `if ( v5 != 0 && (*(*v5 + 72))(a1: v5, a2: &dword_BED9E0) != 0 )`
  - `if ( a2 != (v13 != 0 ? v13 + 4 : 0) && v13 != 0 )`
  - …（截断 16 行）

#### CDraggableItem::PutItem  (3602 B)

- SP_ 节点：`SP_3416_SEALED_ITEMS_CANNOT_BE_R_NSOLD_TRADED_OR_DROPPED`, `SP_886_THIS_ITEM_IS_UNAVAILABLE_TO_TRADE_WITH_OTHERS`
  - `int __thiscall CDraggableItem::PutItem(CDraggableItem *this, struct CPersonalShopDlg *a2, int a3, int a4)`
  - `v5 = *(this + 9);`
  - `if ( v5 != 0 && (*(*v5 + 72))(a1: v5, a2: &dword_BF0EB0) != 0 && (v6 = *(this + 9)) != 0 )`
  - `if ( v7 != 0 )`
  - `v8 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v24) + 4);`
  - `if ( v25 != 0 )`
  - `sub_428C44(this: &v24, a2: 0);`
  - `v9 = *(CharacterData::GetItem(this: v8, a2: &v24, a3: *(this + 6), a4: *(this + 7)) + 4);`
  - `ZRef<GW_ItemSlotBase>::~ZRef<GW_ItemSlotBase>(a1: 0);`
  - `if ( v9 != 0 )`
  - `if ( (*(*v9 + 4))(a1: v9) != 0 )`
  - `Data = TSecType<long>::GetData(a1: v9 + 12, a2: v9 + 12);`
  - `if ( CItemInfo::IsCashItem(a1: Data) == 0 && *(v9 + 3) == 0 || (*(*v9 + 20))(a1: v9) != 0 )`
  - `v14 = TSecType<long>::GetData(a1: v9 + 12, a2: v11);`
  - `if ( CItemInfo::IsPartyQuestItem(this: v26, a2: v14) == 0 )`
  - `v15 = TSecType<long>::GetData(a1: v9 + 12, a2: v11);`
  - …（截断 16 行）

#### CDraggableItem::ThrowItem  (14065 B)

- 分支 case：`1112803`, `1112806`, `1112807`, `1112809`, `4031481`, `4031376`, `4031480`, `4031375`
- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_3416_SEALED_ITEMS_CANNOT_BE_R_NSOLD_TRADED_OR_DROPPED`, `SP_4259_DROPPING_THIS_ITEM_WILL_BREAK_YOUR_ENGAGEMENT_R_NDO_YOU_STILL_WISH_TO_DROP_THIS_`, `SP_4260_YOU_WILL_NOT_BE_ABLE_TO_RECEIVE_THE_WEDDING_RING_R_NIS_THAT_OKAY`, `SP_4658_ONCE_DROPPED_THE_ITEM_CANNOT_BE_TRADED_R_NWOULD_YOU_STILL_LIKE_TO_DROP_IT`, `SP_4875_WOULD_YOU_LIKE_TO_THROW_R_NAWAY_THIS_CARD`, `SP_5258_THIS_ITEM_CANNOT_BE_RECOVERED_ONCE_DROPPED_HOWEVER_IT_IS_POSSIBLE_TO_TRANSFER_IT`, `SP_5571_THIS_ITEM_CANT_BE_TAKEN_BACK_ONCE__R_NTHROWN_AWAY_WILL_YOU_STILL_DROP_IT`, `SP_881_THE_ADMIN_CANT_DROP_ITEMS`, `SP_882_YOU_CANT_DROP_THIS_ITEM`, `SP_885_HOW_MANY_WILL_YOU_DROP`
  - `signed int __thiscall CDraggableItem::ThrowItem(signed int *this, struct GW_ItemSlotBase *pUIEquip, int pUIItem)`
  - `if ( dword_BE7910 != 0 )`
  - `if ( CDraggableItem::CanPerformAction(a1: 1) == 0 )`
  - `if ( CWvsContext::CanSendExclRequest(a1: 500, a2: 0) == 0 )`
  - `if ( sub_4F38D3(this: v5) != 0 && sub_4F38B4(this: v5) )`
  - `CUtilDlg::Notice(sMsg: v47, sSndName: v48, ppDialog: v49, bAutoSeparated: v50, bIsTightLine: v51);`
  - `get_field();`
  - `if ( get_field() != 0 && pUIEquip != 0 && *(get_field() + 356) != 0 )`
  - `v7 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v54) + 4);`
  - `sub_4289B7(this: &v54);`
  - `v8 = *(CharacterData::GetItem(this: v7, a2: v53, a3: *(this + 6), a4: *(this + 7)) + 4);`
  - `sub_4288C1(a1: v53);`
  - `if ( v8 == 0 )`
  - `if ( *(TSingleton<CUserLocal>::ms_pInstance + 1348) / 10000 == 190`
  - `&& (TSecType<long>::GetData(a1: pUIEquip + 12, a2: 0) / 10000 == 190`
  - `|| TSecType<long>::GetData(a1: pUIEquip + 12, a2: 0) / 10000 == 191) )`
  - …（截断 16 行）

#### CDraggableItem::UnmapFuncKey  (2859 B)

  - `signed int __thiscall CDraggableItem::UnmapFuncKey(_DWORD *this, int a2)`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v16) + 4);`
  - `if ( v17 != 0 )`
  - `sub_428C44(this: &v16, a2: 0);`
  - `v4 = *(CharacterData::GetItem(this: v3, a2: &v16, a3: v2[6], a4: v2[7]) + 4);`
  - `ZRef<GW_ItemSlotBase>::~ZRef<GW_ItemSlotBase>(a1: 0);`
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
  - …（截断 16 行）

#### CDraggableItem::WearEquipItem  (34317 B)

- 分支 case：`14`, `30`, `38`, `21`, `22`, `23`, `46`, `26`, `27`, `28`, `29`, `31`, `32`, `33`, `34`, `47`, `35`, `36`, `37`, `39`, `40`, `41`, `42`, `48`, `43`, `44`, `45`
- SP_ 节点：`SP_3818_YOU_CAN_RIDE_THE_TAMED_CASH_MONSTER_WHEN_IT_IS_EQUIPPED_WITH_A_CASH_SADDLE`, `SP_4266_YOU_MAY_ONLY_EQUIP_ONE_WEDDING_RING`, `SP_5223_YOU_CAN_ONLY_BE_EQUIPPED_WITH_1_UNIQUE_EQUIPPED_ITEM`, `SP_5454_THIS_ITEM_CAN_ONLY_BE_EQUIPPED_ON_EVAN`, `SP_867_YOU_CANNOT_TRADE_THIS_ITEM_AFTER_EQUIPPING_R_N_DO_YOU_STILL_WISH_TO_EQUIP`, `SP_868_YOUR_STATS_ARE_TOO_LOW_TO_EQUIP_THIS_ITEM`, `SP_869_ONLY_ONE_COUPLE_RING_CAN_BE_EQUIPPED`, `SP_870_YOU_MAY_ONLY_EQUIP_R_NONE_FRIENDSHIP_RING_AT_A_TIME`, `SP_871_YOU_NEED_TO_EQUIP_YOURSELF_WITH_AN_ITEM_R_NTO_USE_IT`, `SP_872_YOU_ARE_ABLE_TO_USE_THIS_ONLY_R_NWHEN_YOU_ARE_WEARING_A_SHIELD`, `SP_873_YOU_MUST_HAVE_A_TAMED_MONSTER_R_NIN_ORDER_TO_EQUIP_THIS`, `SP_874_YOU_MUST_BE_EQUIPPED_WITH_A_SADDLE_R_NTO_EQUIP_THIS`, `SP_875_THIS_ITEM_IS_NOT_AVAILABLE_WITH_THE_WEAPON_R_NYOURE_CURRENLY_USING`, `SP_876_YOU_MAY_NOT_USE_THE_ITEM_WITH_THE_R_NCASH_ITEM_YOURE_CURRENTLY_EQUIPPED_WITH_R_N`, `SP_877_THIS_ITEM_CANNOT_BE_EQUIPPED_WITH_YOUR_CURRENT_PET`, `SP_878_YOU_CANNOT_CHANGE_YOUR_EQUIPMENT_ON_THIS_MAP`, `SP_879_YOU_DO_NOT_CURRENTLY_HAVE_A_PET_THAT_IS_ALIVE`, `SP_880_THE_PET_IS_UNABLE_TO_WEAR_THE_EQUIPMENT`
  - `signed int __thiscall CDraggableItem::WearEquipItem(_DWORD *this, int a2, signed int a3)`
  - `void (__thiscall *v113)(void *); // [esp-4h] [ebp-4F8h]`
  - `if ( dword_BE7910 != 0 )`
  - `if ( CDraggableItem::CanPerformAction(a1: 1) == 0 )`
  - `if ( *(this + 6) != 1 )`
  - `v5 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v118) + 4);`
  - `sub_4289B7(this: v118);`
  - `FileTime2 = sub_7B8F35();`
  - `if ( a3 == 51 && CompareFileTime(lpFileTime1: (v5 + 1119), lpFileTime2: &FileTime2) < 0 )`
  - `v6 = *(CharacterData::GetItem(this: *v134, a2: v119, a3: 1, a4: a2) + 4);`
  - `sub_4288C1(a1: v119);`
  - `if ( v6 == 0 )`
  - `Data = TSecType<long>::GetData(a1: (v6 + 12), a2: v6);`
  - `if ( a3 < 1000 || a3 >= 1004 || (v7 = (*(*v4 + 64))(a1: v4)) / 100 == 22 || v7 == 2001 )`
  - `if ( CItemInfo::IsOnlyEquipItem(a1: Data) != 0 && CharacterData::IsEquiped(a1: Data) != 0 )`
  - `if ( *(TSingleton<CUserLocal>::ms_pInstance + 1348) / 10000 == 190`
  - …（截断 16 行）

### CDraggableSkill（4 函数）


#### CDraggableSkill::MapFuncKey  (2917 B)

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
  - `else if ( v5 != 54 )`
  - `*(v4 + 4) = *v11;`
  - …（截断 16 行）

#### CDraggableSkill::MapMacro  (2785 B)

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
  - `CMacroSysMan::FlushToSvr();`
  - `if ( v11 != 0 )`
  - …（截断 16 行）

#### CDraggableSkill::OnDoubleClicked  (1185 B)

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

#### CDraggableSkill::OnDropped  (8169 B)

  - `signed int __thiscall CDraggableSkill::OnDropped(`
  - `v6 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v50) + 4);`
  - `if ( v50[1] != 0 )`
  - `sub_428C44(this: v50, a2: 0);`
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
  - `if ( v13 != 0 )`
  - …（截断 16 行）

### CDropPool（7 函数）


#### CDropPool::BoobyTrapCheckPickupItem  (1336 B)

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
  - `CWvsContext::SendBoobyTrapAlert(this: v2, a2: 2);`
  - `*(this + 26) = 0;`
  - `result = ZList<tagPOINT>::RemoveAll(a1: this + 84);`
  - `*(v2 + 3578) = 0;`

#### CDropPool::GetMoneyIcon  (4873 B)

- SP_ 节点：`SP_1589_ICONRAW`, `SP_1717_08D`
  - `_DWORD *__thiscall CDropPool::GetMoneyIcon(char *this, _DWORD *a2, int *a3, int *a4)`
  - `LOBYTE(v39) = 2;`
  - `v8 = sub_506BFE(a1: a3);`
  - `ZXString<char>::Format(a1: &v35, a2: v7, a3: &loc_89543E + v8 + 2);`
  - `LOBYTE(v39) = 1;`
  - `v9 = sub_428226(a1: &v35);`
  - `_bstr_t::_bstr_t(a1: v9);`
  - `sub_40263B(a1: this + 76);`
  - `IWzProperty::GetItem(pvargDest: &pvarg, a2: v27);`
  - `LOBYTE(v39) = 4;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v28, a2: v29);`
  - `if ( Unknown != 0 )`
  - `v11 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v11 < 0 && v11 != -2147467262 )`
  - `LOBYTE(v39) = 6;`
  - `if ( v12 < 0 )`
  - …（截断 16 行）

#### CDropPool::MakeMoneyAnimation  (36338 B)

  - `_DWORD *__thiscall CDropPool::MakeMoneyAnimation(char *this, char a2, int a3)`
  - `v117 = *(a3 + 52);`
  - `v3 = sub_506BFE(a1: v117);`
  - `v209 = *(a3 + 56);`
  - `sub_432D38(a1: &v209);`
  - `LOBYTE(v210) = 1;`
  - `sub_402FAB(a1: -2, a2: 3);`
  - `LOBYTE(v210) = 2;`
  - `if ( v209 == 0 )`
  - `IWzGr2DLayer::RemoveCanvas(this: v209, a2: &v207, a3: &v195);`
  - `sub_4156AB(a1: &v207);`
  - `if ( v4 < 0 )`
  - `if ( v3 >= 0 )`
  - `if ( v3 <= 1 )`
  - `sub_402F85(pvargDest: &v176, pvargSrc: &pvargSrc);`
  - `LOBYTE(v210) = 3;`
  - …（截断 16 行）

#### CDropPool::OnDropEnterField  (21483 B)

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
  - `*(v10 + 10) = CInPacket::Decode4(this: a2);`
  - `v107 = (v93 - 1);`
  - …（截断 16 行）

#### CDropPool::OnDropLeaveField  (12038 B)

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
  - `v77 = *(v79 + 60);`
  - `sub_4458F9(a1: &v77);`
  - …（截断 16 行）

#### CDropPool::OnPacket  (339 B)

  - `int __stdcall CDropPool::OnPacket(int a1, struct CInPacket *a2)`
  - `if ( a1 == 268 )`
  - `return CDropPool::OnDropEnterField(a1: a2);`
  - `if ( a1 == 269 )`
  - `return CDropPool::OnDropLeaveField(a1: a2);`

#### CDropPool::TryPickUpDrop  (4076 B)

- SP_ 节点：`SP_148_NOT_AVAILABLE_WITH_AN_ADMIN_ACCOUNT`
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
  - `if ( (v22 - result[8].x >= 15000`
  - `|| ((v12 = result[5].y) != 0 || result[4].y == *(v23 + 8352)) && (v12 != 1 || result[4].y == *(v23 + 11988)))`
  - …（截断 16 行）

### CReactorPool（4 函数）


#### CReactorPool::FindHitReactor  (8201 B)

  - `signed int __fastcall CReactorPool::FindHitReactor(int a1, int a2)`
  - `update_time = get_update_time();`
  - `v4 = *(a1 + 36);`
  - `if ( v4 != 0 )`
  - `while ( 1 )`
  - `v5 = *(v4 != 0 ? v4 - 16 + 4 : 4);`
  - `v6 = *(v4 + 4);`
  - `sub_7361DD(a1: v31);`
  - `LOBYTE(v44) = 1;`
  - `v42 = ZArray<tagRECT>::InsertBefore(a1: -1);`
  - `SetRectEmpty(lprc: v42);`
  - `if ( v32[12] == 0 && update_time - v32[11] >= 0 )`
  - `sub_73A82C(a1: &v29, a2: v32[4]);`
  - `LOBYTE(v44) = 2;`
  - `if ( v30 != 0 )`
  - `for ( i = 0; ; ++i )`
  - …（截断 16 行）

#### CReactorPool::OnPacket  (431 B)

  - `int __stdcall CReactorPool::OnPacket(int a1, struct CInPacket *a2)`
  - `if ( a1 == 277 )`
  - `return CReactorPool::OnReactorChangeState(a1: a2);`
  - `if ( a1 == 279 )`
  - `return CReactorPool::OnReactorEnterField(a1: a2);`
  - `if ( a1 == 280 )`
  - `return sub_73551F(a1: a2);`

#### CReactorPool::OnReactorChangeState  (1727 B)

  - `int __thiscall CReactorPool::OnReactorChangeState(CReactorPool *this, struct CInPacket *a2)`
  - `update_time = get_update_time();`
  - `a2 = CInPacket::Decode4(this: a2);`
  - `result = sub_736284(a1: &a2, a2: v12);`
  - `if ( result != 0 )`
  - `v5[4] = CInPacket::Decode1(this: v3);`
  - `v6[5] = CInPacket::Decode2(this: v3);`
  - `v7[6] = CInPacket::Decode2(this: v3);`
  - `v8 = CInPacket::Decode2(this: v3);`
  - `if ( v13[12] == 0 )`
  - `if ( v13[10] == 0 )`
  - `v9[9] = CInPacket::Decode1(this: v3);`
  - `v11 = CInPacket::Decode1(this: v3);`
  - `if ( v13 != 0 )`
  - `return sub_736226(a1: 0);`

#### CReactorPool::OnReactorEnterField  (6603 B)

- SP_ 节点：`SP_958_SHAPE2DVECTOR2D`
  - `int __thiscall CReactorPool::OnReactorEnterField(CReactorPool *this, volatile LONG *p_p_pvargDest)`
  - `v35[0] = get_update_time();`
  - `sub_7361AA(a1: v33);`
  - `*v3 = CInPacket::Decode4(this: p_p_pvargDest);`
  - `v4[1] = CInPacket::Decode4(this: v2);`
  - `v5 = CInPacket::Decode1(this: v2);`
  - `v6[5] = CInPacket::Decode2(this: v2);`
  - `v7[6] = CInPacket::Decode2(this: v2);`
  - `v8[16] = CInPacket::Decode1(this: v2);`
  - `v9 = CInPacket::DecodeStr(a1: &p_p_pvargDest);`
  - `LOBYTE(v37) = 1;`
  - `ZXString<char>::operator=(a1: v9);`
  - `LOBYTE(v37) = 0;`
  - `if ( p_p_pvargDest != 0 )`
  - `LOBYTE(v37) = 2;`
  - `LOBYTE(v37) = 3;`
  - …（截断 16 行）

### CAffectedAreaPool（4 函数）


#### CAffectedAreaPool::IsSmokeAreaByPoint  (1895 B)

  - `int __thiscall CAffectedAreaPool::IsSmokeAreaByPoint(int *this, int a2, _DWORD *a3, int a4, int a5)`
  - `update_time = get_update_time();`
  - `v7 = *(this + 4);`
  - `while ( 1 )`
  - `if ( v7 == 0 )`
  - `v8 = *(v7 != 0 ? v7 - 16 + 4 : 4);`
  - `v9 = *(v7 + 4);`
  - `sub_432DCB(a1: v16);`
  - `if ( !v14 || v18 - v17[5] < 0 )`
  - `for ( i = *a3; ; ++i )`
  - `if ( v10 == 0 )`
  - `if ( v11 >= *(v10 - 1) || v17[2] == *i )`
  - `v13 = *(v10 - 1);`
  - `if ( v11 == v13 && v17[2] != a2 )`
  - `if ( dword_BF0484(a1: v17 + 8, a2: a4, a3: a5) != 0 )`
  - `if ( !v14 )`
  - …（截断 16 行）

#### CAffectedAreaPool::OnAffectedAreaCreated  (16152 B)

- SP_ 节点：`SP_4843_ITEM_CASH_0528IMG_08D_TILE`
  - `int __thiscall CAffectedAreaPool::OnAffectedAreaCreated(void *this, CInPacket *iPacket)`
  - `update_time = get_update_time();`
  - `v90 = CInPacket::Decode4(this: iPacket);`
  - `v93[0] = CInPacket::Decode4(this: v3);`
  - `v4 = CInPacket::Decode4(this: v3);`
  - `iPacket = CInPacket::Decode4(this: v3);`
  - `v96 = CInPacket::Decode1(this: v3);`
  - `v86 = CInPacket::Decode2(this: v3);`
  - `CInPacket::DecodeBuffer(a1: v84, a2: 0x10u);`
  - `v88 = CInPacket::Decode4(this: v3);`
  - `sub_43315A(a1: &v90, a2: 0);`
  - `sub_432D98(a1: v97);`
  - `if ( v98[5] == 0 )`
  - `if ( v93[0] == 3 )`
  - `AreaBuffItem = CItemInfo::GetAreaBuffItem(a1: iPacket);`
  - `LOBYTE(v99) = 2;`
  - …（截断 16 行）

#### CAffectedAreaPool::OnAffectedAreaRemoved  (7991 B)

  - `int *__thiscall CAffectedAreaPool::OnAffectedAreaRemoved(CAffectedAreaPool *this, struct CInPacket *a2)`
  - `v39 = CInPacket::Decode4(this: a2);`
  - `if ( sub_43315A(a1: &v39, a2: v46) != 0 )`
  - `sub_432DCB(a1: &v27);`
  - `sub_432729(a1: v27, a2: update_time);`
  - `result = *(v47 + 60);`
  - `if ( result != 0 )`
  - `result = *(result - 1);`
  - `if ( v43 > 0 )`
  - `v5 = (*(v47 + 60) + 4 * v49);`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `LOBYTE(v48) = 1;`
  - `if ( v6 )`
  - `IWzGr2DLayer::GetCanvas(a1: &v42, a2: &pvarg);`
  - `LOBYTE(v48) = 3;`
  - `if ( v7 < 0 )`
  - …（截断 16 行）

#### CAffectedAreaPool::OnPacket  (414 B)

  - `int __thiscall CAffectedAreaPool::OnPacket(void *this, int nType, CInPacket *iPacket)`
  - `if ( nType == 273 )`
  - `return CAffectedAreaPool::OnAffectedAreaCreated(this, iPacket);`
  - `if ( nType == 274 )`
  - `return CAffectedAreaPool::OnAffectedAreaRemoved(this, a2: iPacket);`

### CTownPortalPool（3 函数）


#### CTownPortalPool::OnPacket  (363 B)

  - `int __stdcall CTownPortalPool::OnPacket(int a1, struct CInPacket *a2)`
  - `if ( a1 == 275 )`
  - `return CTownPortalPool::OnTownPortalCreated(a1: a2);`
  - `if ( a1 == 276 )`
  - `return CTownPortalPool::OnTownPortalRemoved(a1: a2);`

#### CTownPortalPool::OnTownPortalCreated  (21720 B)

- SP_ 节点：`SP_2714_SKILL_231IMG_SKILL_2311002_MDOOR`, `SP_2715_SKILL_231IMG_SKILL_2311002_FRAME`, `SP_2716_SKILL_231IMG_SKILL_2311002_CDOOR`
  - `int __thiscall CTownPortalPool::OnTownPortalCreated(`
  - `CUser::GetVecCtrl(this: *(dword_BEBFA8 + 8), a2: &a1);`
  - `sub_41569E(&a1);`
  - `v3 = CInPacket::Decode1(this: FootholdClosest);`
  - `v99 = CInPacket::Decode4(this: v2);`
  - `if ( sub_7BE739(a1: &v99, a2: v92) != 0 )`
  - `FootholdUnderneath = CWvsPhysicalSpace2D::GetFootholdUnderneath(`
  - `if ( FootholdUnderneath == 0 )`
  - `FootholdUnderneath = CWvsPhysicalSpace2D::GetFootholdClosest(this: dword_BEBFA0, x: v93[1].Mid32, y: v93[2].Lo32);`
  - `if ( v6 )`
  - `v88.decVal.Hi32 = 30000 * sub_5375E9(a1: FootholdUnderneath) - 1073721823;`
  - `sub_432C10(a1: 0);`
  - `LOBYTE(v101) = 1;`
  - `v87 = *(sub_7BE638(a1: v92) + 16);`
  - `v86 = *(sub_7BE638(a1: v92) + 12);`
  - `sub_432BDD(a1: 0);`
  - …（截断 16 行）

#### CTownPortalPool::OnTownPortalRemoved  (10088 B)

  - `int __thiscall CTownPortalPool::OnTownPortalRemoved(CTownPortalPool *this, struct CInPacket *a2)`
  - `v2 = CInPacket::Decode1(this: a2);`
  - `v40 = CInPacket::Decode4(this: a2);`
  - `if ( sub_7BE739(a1: &v40, a2: v43) != 0 )`
  - `v35[0] = __PAIR64__(v44, v3);`
  - `sub_7BE6D7();`
  - `sub_7BE5D1(a1: v35[0], a2: HIDWORD(v35[0]));`
  - `if ( v2 == 0 )`
  - `a2 = *(v44 + 20);`
  - `sub_432D38(a1: &a2);`
  - `v48 = *(v44 + 24);`
  - `sub_432D38(a1: &v48);`
  - `LOBYTE(v49) = 2;`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `LOBYTE(v49) = 3;`
  - `if ( a2 == 0 )`
  - …（截断 16 行）

### CPortalList（3 函数）


#### CPortalList::FindPortal  (927 B)

  - `_DWORD *__fastcall CPortalList::FindPortal(CPortalList *this, int a2, int a3, int a4, int a5)`
  - `v5 = *(this + 1);`
  - `if ( v5 != 0 )`
  - `v5 = *(v5 - 4);`
  - `if ( v5 - 1 < 0 )`
  - `while ( 1 )`
  - `v7 = *(*(this + 1) + 8 * v6 + 4);`
  - `if ( v7[2] != 0 )`
  - `if ( --v6 < 0 )`
  - `dword_BF040C(a1: v9, a2: v7[3] - a5, a3: v7[4] - 50, a4: v7[3] + a5, a5: v7[4] + 50);`
  - `if ( dword_BF0484(a1: v9, a2: a3, a3: a4) == 0 )`

#### CPortalList::FindPortal_Collision  (890 B)

  - `_DWORD *__fastcall CPortalList::FindPortal_Collision(CPortalList *this, int a2, int a3, int a4)`
  - `v4 = *(this + 2);`
  - `if ( v4 != 0 )`
  - `v4 = *(v4 - 4);`
  - `if ( v4 - 1 < 0 )`
  - `while ( 1 )`
  - `v5 = *(*(this + 2) + 8 * v9 + 4);`
  - `dword_BF040C(a1: v7, a2: v5[3] - v5[5] / 2, a3: v5[4] - v5[6] / 2, a4: v5[3] + v5[5] / 2, a5: v5[4] + v5[6] / 2);`
  - `if ( dword_BF0484(a1: v7, a2: a3, a3: a4) != 0 )`
  - `if ( --v9 < 0 )`

#### CPortalList::RestorePortal  (28384 B)

  - `int __thiscall CPortalList::RestorePortal(_DWORD *this, int a2, int a3)`
  - `if ( a3 == 0 )`
  - `return sub_4027E9(a1: &a3);`
  - `Data = TSecType<unsigned long>::GetData(a1: a2 + 312);`
  - `Crc32 = CCrc32::GetCrc32(a1: &Data, a2: 4, a3: 0, a4: 0, a5: 0);`
  - `if ( v6 )`
  - `NewEnum = IWzProperty::Get_NewEnum(a1: v135);`
  - `LOBYTE(v153) = 1;`
  - `sub_403606(a1: NewEnum);`
  - `sub_4027B1(a1: v135);`
  - `VariantInit(&pvarg);`
  - `LOBYTE(v153) = 4;`
  - `while ( 1 )`
  - `if ( v139 == 0 )`
  - `if ( (*(*v139 + 12))(a1: v139, a2: 1, a3: &pvarg, a4: &v134) != 0 )`
  - `sub_406478(a1: &v148, pvarSrc: &pvarg);`
  - …（截断 16 行）

### CWvsPhysicalSpace2D（10 函数）


#### CWvsPhysicalSpace2D::CanGoThrough  (6019 B)

  - `signed int __thiscall CWvsPhysicalSpace2D::CanGoThrough(`
  - `if ( v25 && v8 == 0 )`
  - `CWvsPhysicalSpace2D::GetCrossCandidate(this: this + 17, a2: xm1, a3: ym1, a4: v6, a5: v7, a6: v37);`
  - `if ( v38 == 0 )`
  - `v11 = *(v10 + 4);`
  - `v12 = *(v11 + 48);`
  - `v13 = *(v10 != 0 ? v10 - 16 + 4 : 4);`
  - `if ( v12 > dbl_AF0DE8 || (v14 = *(v11 + 32)) == 0 || v14 == nZMass )`
  - `v16 = *(v11 + 12);`
  - `v17 = *(v11 + 16);`
  - `v18 = *(v11 + 24);`
  - `v52 = *(v11 + 20);`
  - `v19 = v52 * (ym1 - v17) - (v18 - v17) * (xm1 - v16);`
  - `v20 = v52 * (*ym2 - v17) - (v18 - v17) * (v49 - v16);`
  - `if ( v19 <= 0 && v20 >= 0 && (v19 != 0 || v20 != 0) )`
  - `v23 = v22 * (v47 - ym1) - v21 * (v43 - xm1);`
  - …（截断 16 行）

#### CWvsPhysicalSpace2D::GetCrossCandidate  (690 B)

  - `int __thiscall CWvsPhysicalSpace2D::GetCrossCandidate(_DWORD *this, int a2, int a3, int a4, int a5, int a6)`
  - `if ( a2 >= a4 )`
  - `if ( a3 >= a5 )`
  - `return sub_50892D(a1: *(this + 7), a2: &v7, a3: a6);`

#### CWvsPhysicalSpace2D::GetFoothold  (969 B)

  - `int __stdcall CWvsPhysicalSpace2D::GetFoothold(unsigned int dwSN)`
  - `if ( dwSN != 0 && ZMap<unsigned long,ZRef<CStaticFoothold>,unsigned long>::GetAt(a1: &dwSN, a2: v4) != 0 )`
  - `v2 = *(v1 + 1);`
  - `if ( (v8 & 1) != 0 && v7 != 0 )`
  - `ZRef<CStaticFoothold>::~ZRef<CStaticFoothold>(a1: 0);`
  - `if ( v5 != 0 )`

#### CWvsPhysicalSpace2D::GetFootholdAbove  (1754 B)

  - `_DWORD *__thiscall CWvsPhysicalSpace2D::GetFootholdAbove(CWvsPhysicalSpace2D *this, int x, int y, int *pcy, int yMax)`
  - `if ( yMax > y )`
  - `CWvsPhysicalSpace2D::GetCrossCandidate(this: this + 17, a2: x - 1, a3: yMax, a4: x + 1, a5: y + 1, a6: v14);`
  - `if ( v15 != 0 )`
  - `v8 = -(v7 != 0);`
  - `v10 = *(v7 + 4);`
  - `v17 = *((v9 & v8) + 4) != 0 ? *((v9 & v8) + 4) + 16 : 0;`
  - `if ( v12 < v11 && v12 <= x && v11 >= x )`
  - `v13 = v10[4] + (x - v12) * (v10[6] - v10[4]) / (v11 - v12);`
  - `if ( v13 <= y && v13 > yMax )`
  - `yMax = v10[4] + (x - v12) * (v10[6] - v10[4]) / (v11 - v12);`
  - `while ( v17 != 0 );`
  - `ZList<ZRef<CStaticFoothold>>::RemoveAll(a1: v14);`

#### CWvsPhysicalSpace2D::GetFootholdClosest  (908 B)

  - `_DWORD *__thiscall CWvsPhysicalSpace2D::GetFootholdClosest(CWvsPhysicalSpace2D *this, int x, int y)`
  - `v3 = *(this + 34);`
  - `while ( v3 != 0 )`
  - `v5 = *(v3 + 4);`
  - `v6 = *(v3 != 0 ? v3 - 16 + 4 : 4);`
  - `if ( v7 + 8 <= v8 )`
  - `v9 = (v8 + v7) / 2 - x;`
  - `if ( v9 * v9 + (v10 / 2 - y) * (v10 / 2 - y) < v4 )`
  - `v4 = v9 * v9 + (v10 / 2 - y) * (v10 / 2 - y);`

#### CWvsPhysicalSpace2D::GetFootholdRange  (1377 B)

  - `int __thiscall CWvsPhysicalSpace2D::GetFootholdRange(_DWORD *this, int x, int y1, int y2, int lPosition)`
  - `CWvsPhysicalSpace2D::GetCrossCandidate(this: this + 17, a2: x - 1, a3: y1 - 1, a4: x + 1, a5: y2 + 1, a6: v13);`
  - `while ( v6 != 0 )`
  - `v7 = *(v6 + 4);`
  - `v9 = *(v6 != 0 ? v6 - 16 + 4 : 4);`
  - `if ( v10 < v8 && v10 <= x && v8 >= x )`
  - `v11 = v7[4] + (x - v10) * (v7[6] - v7[4]) / (v8 - v10);`
  - `if ( v11 >= y1 && y2 >= v11 )`
  - `*ZList<long>::AddTail(a1: lPosition) = v11;`
  - `return ZList<ZRef<CStaticFoothold>>::RemoveAll(a1: v13);`

#### CWvsPhysicalSpace2D::GetFootholdUnderneath  (1848 B)

  - `_DWORD *__thiscall CWvsPhysicalSpace2D::GetFootholdUnderneath(`
  - `if ( yMin < y )`
  - `CWvsPhysicalSpace2D::GetCrossCandidate(`
  - `for ( i = v16; i != 0; i = nRangeXa )`
  - `v9 = -(i != 0);`
  - `v11 = *(i + 4);`
  - `nRangeXa = *((v10 & v9) + 4) != 0 ? *((v10 & v9) + 4) + 16 : 0;`
  - `if ( v13 < v12 && v13 <= x && v12 >= x )`
  - `v14 = v11[4] + (x - v13) * (v11[6] - v11[4]) / (v12 - v13);`
  - `if ( v14 >= y && v14 < yMin )`
  - `yMin = v11[4] + (x - v13) * (v11[6] - v11[4]) / (v12 - v13);`
  - `if ( pcy != 0 )`
  - `ZList<ZRef<CStaticFoothold>>::RemoveAll(a1: lCrossCandidate);`

#### CWvsPhysicalSpace2D::GetLadderOrRope  (1228 B)

  - `_DWORD *__fastcall CWvsPhysicalSpace2D::GetLadderOrRope(`
  - `if ( a3 >= a5 )`
  - `if ( a4 >= a6 )`
  - `for ( i = 28; ; i += 28 )`
  - `v10 = *(this + 42);`
  - `if ( v10 == 0 || v8 >= *(v10 - 4) )`
  - `result = (*(this + 42) + i);`
  - `if ( v14 <= v12 && v12 <= v15 && v7 <= result[5] && result[4] <= a4 )`

#### CWvsPhysicalSpace2D::Load  (42547 B)

- SP_ 节点：`SP_1528_VRLEFT`, `SP_1529_VRRIGHT`, `SP_1530_VRTOP`, `SP_1531_VRBOTTOM`, `SP_1532_VRLIMIT`, `SP_1748_PAGE`, `SP_1911_PREV`, `SP_1912_NEXT`, `SP_2320_X1`, `SP_2321_X2`, `SP_2322_Y1`, `SP_2323_Y2`, `SP_2324_L`, `SP_2325_UF`, `SP_2971_DRAG`, `SP_2972_FORCE`, `SP_4417_FORBIDFALLDOWN`, `SP_977_X`
  - `int __thiscall CWvsPhysicalSpace2D::Load(const void **this, VARIANTARG *pPropFoothold, int pLadderRope, int pInfo)`
  - `HIDWORD(v6) = 0;`
  - `Crc32 = CCrc32::GetCrc32(a1: &v198, a2: 4, a3: 0, a4: 0, a5: 0);`
  - `v5 = CCrc32::GetCrc32(a1: &Crc32, a2: 4, a3: 0, a4: 0, a5: 0);`
  - `*(this + 44) = v5;`
  - `if ( v7 )`
  - `NewEnum = IWzProperty::Get_NewEnum(a1: v160);`
  - `LOBYTE(v205) = 3;`
  - `sub_403606(a1: NewEnum);`
  - `sub_4027B1(a1: v160);`
  - `VariantInit(&pvarg);`
  - `LOBYTE(v205) = 6;`
  - `while ( 1 )`
  - `if ( v170 == 0 )`
  - `if ( (*(*v170 + 12))(a1: v170, a2: 1, a3: &pvarg, a4: &v158) != 0 )`
  - `sub_406478(a1: &v178, pvarSrc: &pvarg);`
  - …（截断 16 行）

#### CWvsPhysicalSpace2D::SetFieldAttr  (1194 B)

  - `void __thiscall CWvsPhysicalSpace2D::SetFieldAttr(CWvsPhysicalSpace2D *this)`
  - `v2 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 84);`
  - `if ( v2 != 0 )`
  - `v3 = CAttrField::CAttrField(this: v2);`
  - `*(this + 43) = v3;`
  - `field = get_field();`
  - `if ( field != 0 )`
  - `v7 = sub_A45CD6(this: field);`
  - `sub_500D61(a1: SLOBYTE(v7), a2: HIDWORD(v7));`
  - `v8 = sub_A45CD6(this: v5);`
  - `sub_500D61(a1: SLOBYTE(v8), a2: HIDWORD(v8));`
  - `if ( v5[86] != 0 )`
  - `TSecType<long>::SetData(a1: 1);`
  - `else if ( v5[87] != 0 )`
  - `TSecType<long>::SetData(a1: 2);`
  - `if ( v6 != 0 )`
  - …（截断 16 行）

### VCWvsPhysicalSpace2D（1 函数）


#### VCWvsPhysicalSpace2D::0?$ZRefCountedDummy  (490 B)

  - `ZRefCountedDummy<CWvsPhysicalSpace2D> *__thiscall ZRefCountedDummy<CWvsPhysicalSpace2D>::ZRefCountedDummy<CWvsPhysicalSpace2D>(`
  - `memset(a1: this, a2: 0, a3: 0xCu);`
  - `*(this + 3) = &off_AF283C;`
  - `CWvsPhysicalSpace2D::CWvsPhysicalSpace2D(a1: this + 16);`
  - `*(this + 3) = &off_AF2834;`

### CStage（5 函数）


#### CStage::FadeIn  (8421 B)

  - `int CStage::FadeIn()`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v42) = 1;`
  - `if ( dword_BF14EC == 0 )`
  - `v0 = sub_776E01(this: dword_BF14EC, a2: &v41);`
  - `LOBYTE(v42) = 2;`
  - `if ( v1 )`
  - `v3 = (*(v2 + 144))(`
  - `if ( v3 < 0 )`
  - `sub_41569E(a1: &v41);`
  - `LOBYTE(v42) = 0;`
  - `if ( v4 < 0 )`
  - `if ( v5 < 0 )`
  - `LOBYTE(v42) = 4;`
  - `v6 = sub_776E33(this: dword_BF14EC, a2: &v41);`
  - …（截断 16 行）

#### CStage::OnPacket  (447 B)

  - `void __thiscall CStage::OnPacket(char *this, int nType, int **iPacket)`
  - `switch ( nType )`
  - `CStage::OnSetField(this: (this - 8), a2: iPacket);`
  - `CStage::OnSetITC(a1: iPacket);`
  - `CStage::OnSetCashShop(iPacket);`

#### CStage::OnSetCashShop  (2066 B)

  - `void __stdcall CStage::OnSetCashShop(struct CInPacket *iPacket)`
  - `CWvsContext::OnSetLogoutGiftConfig(this: &v11, a2: v10);`
  - `CharacterData::Decode(this: v12, a2: iPacket, a3: 0);`
  - `sub_42899A(this: &v8);`
  - `LOBYTE(v15) = 0;`
  - `v3 = sub_42861D();`
  - `CWvsContext::SetCharacterData(this: v3, a2: v8, a3: v9);`
  - `if ( (*(*(dword_BEDED4 + 4) + 72))(a1: dword_BEDED4 + 4, a2: &dword_BED874) == 0 )`
  - `v4 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 24);`
  - `LOBYTE(v15) = 2;`
  - `if ( v4 != 0 )`
  - `v5 = CInterStage::CInterStage(this: v4);`
  - `set_stage(a1: v5, a2: 0);`
  - `v14 = *(*dword_BE7918 + 14056);`
  - `v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 7948);`
  - `LOBYTE(v15) = 3;`
  - …（截断 16 行）

#### CStage::OnSetField  (20486 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1587_MSG`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_3393_YOUR_CHATTING_WILL_BE_ENABLED_AFTER_04D_D_D`, `SP_3400_ETC_CHATBLOCKREASONIMG`, `SP_921_EBASIC_CONFIGURATION_FOR_MAPLESTORYN_R_N_R_NFUI_UIWINDOWIMG_UTILDLGEX_NOTICE`
  - `void __thiscall CStage::OnSetField(CStage *this, unsigned int **a2)`
  - `v3 = *(*dword_BE7918 + 8280);`
  - `v5 = (*dword_BE7918 + 8280);`
  - `v6 = CInPacket::Decode4(this: a2);`
  - `if ( !v7 )`
  - `*(v2 + 12332) = 1;`
  - `v105 = CInPacket::Decode1(this: v4);`
  - `v102 = CInPacket::Decode1(this: v4);`
  - `v103 = CInPacket::Decode2(this: v4);`
  - `ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF);`
  - `if ( v103 != 0 )`
  - `v8 = CInPacket::DecodeStr(a1: &v110);`
  - `LOBYTE(v111) = 2;`
  - `ZXString<char>::operator=(a1: v8);`
  - `LOBYTE(v111) = 1;`
  - `if ( v110 != 0 )`
  - …（截断 16 行）

#### CStage::OnSetITC  (1817 B)

  - `void __thiscall CStage::OnSetITC(CStage *this, struct CInPacket *a2)`
  - `CWvsContext::OnSetLogoutGiftConfig(this: &v10, a2: v9);`
  - `CharacterData::Decode(this: v11, a2, a3: 0);`
  - `sub_42899A(this: &v7);`
  - `LOBYTE(v13) = 0;`
  - `v3 = sub_42861D();`
  - `CWvsContext::SetCharacterData(this: v3, a2: v7, a3: v8);`
  - `if ( (*(*(dword_BEDED4 + 4) + 72))(a1: dword_BEDED4 + 4, a2: &dword_BED874) == 0 )`
  - `v4 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 24);`
  - `LOBYTE(v13) = 2;`
  - `if ( v4 != 0 )`
  - `v5 = CInterStage::CInterStage(this: v4);`
  - `set_stage(a1: v5, a2: 0);`
  - `v12 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 7832);`
  - `LOBYTE(v13) = 3;`
  - `if ( v12 != 0 )`
  - …（截断 16 行）

### CUserPool（21 函数）


#### CUserPool::CreateLocalUser  (1132 B)

  - `LONG __thiscall CUserPool::CreateLocalUser(_DWORD *this, int a2, int *a3)`
  - `sub_42899A(this: &v9);`
  - `LOBYTE(v12) = 0;`
  - `v4 = sub_42861D();`
  - `CWvsContext::SetCharacterData(this: v4, a2: v9, a3: v10);`
  - `v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 12928);`
  - `LOBYTE(v12) = 2;`
  - `if ( v6 != 0 )`
  - `v7 = CUserLocal::CUserLocal(a1: v6, a2: v5, a3: *a3);`
  - `sub_9732FE(a1: v7);`
  - `result = CUserLocal::Init(this: *(this + 2));`
  - `if ( a3 != 0 )`
  - `return sub_428C44(this: &a2, a2: 0);`

#### CUserPool::FindBalloon  (1447 B)

  - `int __thiscall CUserPool::FindBalloon(int *this, _DWORD *a2)`
  - `v2 = *(this + 12);`
  - `if ( v2 == 0 )`
  - `while ( 1 )`
  - `v3 = *(v2 != 0 ? v2 - 16 + 4 : 4);`
  - `v4 = *(v2 + 4);`
  - `if ( v4 != 0 )`
  - `InterlockedIncrement(lpAddend: (v4 + 4));`
  - `v5 = *(v4 + 16);`
  - `v6 = *(v5 + 7832) == 0;`
  - `if ( !v6 )`
  - `sub_53F907(a1: 0);`
  - `CChatBalloon::GetMiniRoomBalloonRect(this: (v5 + 20), a2: &v9);`
  - `if ( dword_BF0484(a1: &v9, a2: *a2, a3: a2[1]) == 0 )`
  - `v8 = *(v10 + 16);`
  - `if ( v10 != 0 )`
  - …（截断 16 行）

#### CUserPool::FindPet  (1919 B)

  - `int __thiscall CUserPool::FindPet(int *this, _DWORD *a2)`
  - `if ( *(this + 2) != 0 )`
  - `for ( i = 0; i < 24; i += 8 )`
  - `v4 = *(*(this + 2) + 7900);`
  - `if ( *(v4 + i + 4) == 0 )`
  - `CPet::GetBodyRect(this: *(v4 + i + 4), a2: &v13);`
  - `if ( dword_BF0484(a1: &v13, a2: *a2, a3: a2[1]) != 0 )`
  - `return *(this + 2);`
  - `v5 = *(this + 12);`
  - `while ( v5 != 0 )`
  - `v6 = *(v5 != 0 ? v5 - 16 + 4 : 4);`
  - `v7 = *(v5 + 4);`
  - `if ( v7 != 0 )`
  - `InterlockedIncrement(lpAddend: (v7 + 4));`
  - `v8 = *(*(v7 + 16) + 1384) == 0;`
  - `if ( v8 )`
  - …（截断 16 行）

#### CUserPool::FindRemoteUserByName  (859 B)

  - `int __thiscall CUserPool::FindRemoteUserByName(int *this, int sName)`
  - `pos = *(this + 12);`
  - `while ( 1 )`
  - `if ( pos == 0 )`
  - `v3 = *(pos != 0 ? pos - 16 + 4 : 4);`
  - `v4 = *(pos + 4);`
  - `if ( v4 != 0 )`
  - `InterlockedIncrement(lpAddend: (v4 + 4));`
  - `if ( dword_BF03C4(a1: sName, a2: *(*(v4 + 16) + 4524)) == 0 )`
  - `if ( v7 != 0 )`
  - `sub_53F907(a1: 0);`
  - `v5 = *(v7 + 16);`

#### CUserPool::GetRemoteUser  (577 B)

  - `int __stdcall CUserPool::GetRemoteUser(char a1)`
  - `if ( sub_973741(&a1, a2: v3) != 0 )`
  - `v2 = *(v4 + 16);`
  - `if ( v4 != 0 )`
  - `sub_53F907(a1: 0);`

#### CUserPool::GetUser  (278 B)

  - `int __thiscall CUserPool::GetUser(CUserPool *this, unsigned int a2)`
  - `result = *(this + 2);`
  - `if ( result == 0 || *(result + 4520) != a2 )`
  - `return CUserPool::GetRemoteUser(a1: a2);`

#### CUserPool::OnCoupleChairRecordAdd  (373 B)

  - `_DWORD *__thiscall CUserPool::OnCoupleChairRecordAdd(char *this, int a2, int a3)`
  - `CUserPool::OnCoupleChairRecordRemove(a1: a2);`
  - `result = sub_974513(a1: this + 188);`

#### CUserPool::OnCoupleChairRecordRemove  (1665 B)

  - `int __thiscall CUserPool::OnCoupleChairRecordRemove(CUserPool *this, unsigned int a2)`
  - `result = *(this + 50);`
  - `while ( result != 0 )`
  - `v5 = *(result != 0 ? result - 16 + 4 : 4);`
  - `if ( *v4 == a2 )`
  - `if ( v4[3] != 0 && v4[1] != 0 )`
  - `User = CUserPool::GetUser(this, a2: *v4);`
  - `v6 = CUserPool::GetUser(this, a2: v4[1]);`
  - `if ( User != 0 )`
  - `CUser::SetCoupleChairEffect(a1: 0, a2: v6, a3: 0);`
  - `if ( v7 != 0 )`
  - `CUser::SetCoupleChairEffect(a1: 0, a2: 0, a3: 0);`
  - `v8 = *(this + 50);`
  - `if ( v8 != 0 )`
  - `while ( 1 )`
  - `v11 = *(v8 != 0 ? v8 - 16 + 4 : 4);`
  - …（截断 16 行）

#### CUserPool::OnCoupleRecordAdd  (1669 B)

  - `_DWORD *__thiscall CUserPool::OnCoupleRecordAdd(int *this, _DWORD *a2, int a3, int a4)`
  - `if ( *a2 != 0 )`
  - `v7 = *(this + 2);`
  - `if ( result == *(v7 + 7960) && v6 == *(v7 + 7964) )`
  - `if ( v7 != 0 )`
  - `if ( *(a3 + 4520) > *(v7 + 4520) )`
  - `sub_9746CF(a1: &v14, &a2);`
  - `result = sub_973B06(a1: this + 14);`
  - `*result = *(v14 + 4520);`
  - `result[2] = *(v14 + 7952);`
  - `result[3] = *(v11 + 4);`
  - `v8 = *(this + 12);`
  - `while ( v8 != 0 )`
  - `v8 = *(v8 - 12) != 0 ? *(v8 - 12) + 16 : 0;`
  - `v10 = *(*(v9 + 4) + 16);`
  - `if ( result == *(v10 + 7960) && v6 == *(v10 + 7964) )`
  - …（截断 16 行）

#### CUserPool::OnCoupleRecordRemove  (991 B)

  - `unsigned int *__thiscall CUserPool::OnCoupleRecordRemove(CUserPool *this, int *a2)`
  - `result = (v2 | *a2);`
  - `if ( *a2 != 0 )`
  - `result = *(this + 17);`
  - `while ( result != 0 )`
  - `v7 = *(result != 0 ? result - 3 : 4);`
  - `result = v7 != 0 ? (v7 + 16) : 0;`
  - `if ( v6[2] == v4 && v6[3] == v2 || v6[4] == v4 && v6[5] == v2 )`
  - `CUserPool::GetUser(this, a2: *v6);`
  - `User = CUserPool::GetUser(this, a2: v6[1]);`
  - `CUser::SetCoupleItemEffect(a1: 0, a2: User, a3: v6[6]);`
  - `return ZList<CUserPool::COUPLEENTRY>::RemoveAt(a1: v9);`

#### CUserPool::OnFriendRecordRemove  (1008 B)

  - `unsigned int *__thiscall CUserPool::OnFriendRecordRemove(CUserPool *this, int *a2)`
  - `result = (v2 | *a2);`
  - `if ( *a2 != 0 )`
  - `result = *(this + 29);`
  - `while ( result != 0 )`
  - `v7 = *(result != 0 ? result - 3 : 4);`
  - `result = v7 != 0 ? (v7 + 16) : 0;`
  - `if ( v6[2] == v4 && v6[3] == v2 || v6[4] == v4 && v6[5] == v2 )`
  - `User = CUserPool::GetUser(this, a2: *v6);`
  - `v9 = CUserPool::GetUser(this, a2: v6[1]);`
  - `CUser::SetFriendShipItemEffect(this: User, a2: 0, a3: v9, a4: v6[6]);`
  - `return sub_973E58(a1: v10);`

#### CUserPool::OnMarriageRecordRemove  (911 B)

  - `void __thiscall CUserPool::OnMarriageRecordRemove(CUserPool *this, int a2)`
  - `if ( a2 != 0 )`
  - `v3 = *(this + 45);`
  - `while ( v3 != 0 )`
  - `v5 = *(v3 != 0 ? v3 - 3 : 4);`
  - `v3 = v5 != 0 ? (v5 + 16) : 0;`
  - `if ( v6 == a2 || v4[1] == a2 )`
  - `User = CUserPool::GetUser(this, a2: v6);`
  - `v7 = CUserPool::GetUser(this, a2: v4[1]);`
  - `if ( User != 0 && v7 != 0 )`
  - `sub_93FCBE(a1: 0, a2: v7, a3: 0);`
  - `sub_9743C7(a1: v8);`

#### CUserPool::OnNewYearCardRecordAdd  (1551 B)

  - `void __thiscall CUserPool::OnNewYearCardRecordAdd(CUserPool *this, struct CUser *i, unsigned int a3)`
  - `if ( i != 0 )`
  - `v16 = *(i + 1130);`
  - `v5 = sub_97412E(a1: &v16, a2: 0);`
  - `v6 = sub_5DD293(a1: v5 + 12);`
  - `v8 = sub_9742F6(a1: this + 144);`
  - `for ( i = v8; i != 0; v8 = i )`
  - `v9 = sub_974317(a1: &i, a2: 0);`
  - `if ( *v9 != *(v3 + 1130) )`
  - `v11 = *(a3 + 12);`
  - `while ( v11 != 0 )`
  - `v13 = *(v11 != 0 ? v11 - 16 + 4 : 4);`
  - `if ( v7 == *v12 )`
  - `v14 = sub_973FA4(a1: v17 + 124);`

#### CUserPool::OnNewYearCardRecordRemoveAll  (1157 B)

  - `unsigned int __thiscall CUserPool::OnNewYearCardRecordRemoveAll(CUserPool *this, unsigned int a2)`
  - `result = sub_974237(a1: &a2);`
  - `v4 = *(v2 + 34);`
  - `while ( v4 != 0 )`
  - `v6 = *(v4 != 0 ? v4 - 3 : 4);`
  - `v4 = v6 != 0 ? (v6 + 16) : 0;`
  - `if ( result == a2 || v5[1] == a2 )`
  - `User = CUserPool::GetUser(this: v2, a2: result);`
  - `v8 = CUserPool::GetUser(this, a2: v5[1]);`
  - `if ( User != 0 )`
  - `sub_93F83C(a1: 0, a2: v8, a3: v5[2], a4: 224);`
  - `if ( v10 != 0 )`
  - `sub_93F83C(a1: 0, a2: User, a3: v5[2], a4: 224);`
  - `result = sub_973FE2(a1: v9);`

#### CUserPool::OnPacket  (709 B)

  - `void __thiscall CUserPool::OnPacket(CUserPool *this, signed int a2, wchar_t *a3)`
  - `if ( a2 == 160 )`
  - `CUserPool::OnUserEnterField(this, a2: a3);`
  - `else if ( a2 == 161 )`
  - `CUserPool::OnUserLeaveField(a1: a3);`
  - `else if ( a2 < 162 || a2 > 184 )`
  - `if ( a2 < 185 || a2 > 204 )`
  - `if ( a2 >= 205 && a2 <= 234 )`
  - `CUserPool::OnUserLocalPacket(a1: this, a2: a2 - 161, a3: a2, a4: a3);`
  - `CUserPool::OnUserRemotePacket(a1: a2, a2: a3);`
  - `CUserPool::OnUserCommonPacket(this, a2, iPacket: a3);`

#### CUserPool::OnUserCommonPacket  (1547 B)

- 分支 case：`162`, `163`, `164`, `165`, `166`, `167`
  - `void __thiscall CUserPool::OnUserCommonPacket(CUserPool *this, int a2, char *iPacket)`
  - `v4 = CInPacket::Decode4(this: iPacket);`
  - `User = CUserPool::GetUser(this, a2: v4);`
  - `if ( a2 == 163 )`
  - `if ( User != 0 )`
  - `v7 = *(this + 2);`
  - `if ( v7 != 0 )`
  - `switch ( v6 )`
  - `CUser::OnChat(this: v7, a2: iPacket, a3: 0);`
  - `CUser::OnChat(this: v7, a2: iPacket, a3: 1);`
  - `CUser::OnADBoard(this: v7, a2: iPacket);`
  - `sub_938BA5(a1: iPacket);`
  - `CUser::SetConsumeItemEffect(this: v7, iPacket);`
  - `sub_93354D(a1: iPacket);`
  - `if ( v6 >= 168 && v6 <= 174 )`
  - `CUser::OnPetPacket(this: v7, a2: v6, iPacket);`
  - …（截断 16 行）

#### CUserPool::OnUserEnterField  (2665 B)

  - `_DWORD *__thiscall CUserPool::OnUserEnterField(CUserPool *this, struct CInPacket *a2)`
  - `result = CInPacket::Decode4(this: a2);`
  - `if ( result != *(*dword_BE7918 + 8352) )`
  - `result = sub_973741(a1: &v19, a2: 0);`
  - `if ( result == 0 && *(v4 + 12468) == 0 )`
  - `v5 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 28);`
  - `if ( v5 != 0 )`
  - `v6 = sub_972285(a1: v5);`
  - `sub_85BCDB(a1: v16);`
  - `v18 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 14304);`
  - `LOBYTE(v20) = 2;`
  - `if ( v18 != 0 )`
  - `v7 = CUserRemote::CUserRemote(a1: v19);`
  - `LOBYTE(v20) = 1;`
  - `sub_9732B1(a1: v7);`
  - `v8 = sub_97397C(a1: this + 36);`
  - …（截断 16 行）

#### CUserPool::OnUserLeaveField  (1220 B)

  - `_DWORD *__thiscall CUserPool::OnUserLeaveField(CUserPool *this, struct CInPacket *a2)`
  - `a2 = CInPacket::Decode4(this: a2);`
  - `result = sub_97393E(a1: &a2);`
  - `if ( result != 0 )`
  - `sub_97352F(a1: result + 3);`
  - `CUserPool::OnCoupleRecordRemove(this, a2: (MEMORY[0x10] + 7952));`
  - `CUserPool::OnFriendRecordRemove(this, a2: (MEMORY[0x10] + 8072));`
  - `CUserPool::OnMarriageRecordRemove(this, a2: *(MEMORY[0x10] + 4520));`
  - `CUserPool::OnNewYearCardRecordRemoveAll(this, a2: *(MEMORY[0x10] + 4520));`
  - `CUserPool::OnCoupleChairRecordRemove(a1: a2);`
  - `sub_9739BA(a1: MEMORY[0x14]);`
  - `sub_97387F(a1: v4 + 2);`
  - `if ( MEMORY[0x18] != 0 )`
  - `sub_973CCE(a1: MEMORY[0x18]);`
  - `if ( dword_BF11B4 != 0 )`
  - `CWnd::Destroy(a1: dword_BF11B4);`
  - …（截断 16 行）

#### CUserPool::OnUserLocalPacket  (280 B)

  - `int __fastcall CUserPool::OnUserLocalPacket(int a1, int a2, int a3, wchar_t *a4)`
  - `v4 = *(a1 + 8);`
  - `if ( v4 != 0 )`
  - `return CUserLocal::OnPacket(a1: v4, a2, a3, a4);`

#### CUserPool::OnUserRemotePacket  (2721 B)

- 分支 case：`186`, `187`, `188`, `189`, `190`, `191`, `192`, `193`, `194`, `195`, `198`, `204`, `185`, `196`, `197`, `199`, `200`, `201`, `202`, `203`
  - `void __userpurge CUserPool::OnUserRemotePacket(int a1@<ebx>, int a2, wchar_t *a3)`
  - `v3 = CInPacket::Decode4(this: a3);`
  - `RemoteUser = CUserPool::GetRemoteUser(a1: v3);`
  - `if ( RemoteUser != 0 && *(*dword_BE7918 + 12468) == 0 )`
  - `if ( RemoteUser[346] == 0 )`
  - `switch ( a2 )`
  - `CUserRemote::OnAttack(a1: a2, a2: a3);`
  - `sub_980A81(a1: a3);`
  - `sub_980BF5(a1: a3);`
  - `CUserRemote::OnHit(a1: a3);`
  - `v5 = CInPacket::Decode4(this: a3);`
  - `CAvatar::SetEmotion(this: (RemoteUser + 34), nEmotion: v5, tDuration: -1);`
  - `v6 = CInPacket::Decode4(this: a3);`
  - `CUser::SetActiveEffectItem(this: RemoteUser, a2: v6);`
  - `sub_983E40(a1: a3);`
  - `CUser::OnEffect(this: RemoteUser, arg0: a3);`
  - …（截断 16 行）

#### CUserPool::Update  (17074 B)

  - `int __thiscall CUserPool::Update(CUserRemote *this)`
  - `void (__thiscall ***v2)(_DWORD); // ecx`
  - `void (__thiscall ***v5)(_DWORD); // ecx`
  - `v2 = *(this + 2);`
  - `if ( v2 != 0 )`
  - `(**v2)(a1: v2);`
  - `v3 = *(v1 + 12);`
  - `while ( v3 != 0 )`
  - `v4 = *(v3 != 0 ? v3 - 16 + 4 : 4);`
  - `v5 = *(*(v3 + 4) + 16);`
  - `(**v5)(a1: v5);`
  - `result = *(v1 + 17);`
  - `if ( result != 0 )`
  - `while ( 1 )`
  - `v8 = *(result != 0 ? result - 16 + 4 : 4);`
  - `v89 = v8 != 0 ? (v8 + 16) : 0;`
  - …（截断 16 行）

### EQUIPITEM（3 函数）


#### EQUIPITEM::IsAbleToStickWithWeapon  (245 B)

  - `BOOL __thiscall CItemInfo::EQUIPITEM::IsAbleToStickWithWeapon(CItemInfo::EQUIPITEM *this, int a2)`
  - `return _ZtlSecureFuse<long>(a1: this, a2: *(this + 2)) / 100000 == 17`
  - `&& ((1 << sub_46D3DB(a1: a2)) & *(this + 116)) != 0;`

#### EQUIPITEM::IsItemSuitedForPet  (718 B)

  - `BOOL __thiscall CItemInfo::EQUIPITEM::IsItemSuitedForPet(CItemInfo::EQUIPITEM *this, int a2)`
  - `if ( _ZtlSecureFuse<long>(a1: this, a2: *(this + 2)) / 10000 == 180 && a2 / 10000 == 500 )`
  - `v2 = UINT128::UINT128(this: v7);`
  - `v3 = UINT128::setBitNumber(this: v2, a2: a2 % 1000, a3: 1u);`
  - `v4 = UINT128::operator&(a1: v6, a2: v3);`
  - `if ( UINT128::operator!(a1: v4) == 0 )`

#### EQUIPITEM::IsItemSuitedForTamingMob  (266 B)

  - `BOOL __thiscall CItemInfo::EQUIPITEM::IsItemSuitedForTamingMob(CItemInfo::EQUIPITEM *this, int a2)`
  - `return _ZtlSecureFuse<long>(a1: this, a2: *(this + 2)) / 10000 == 191`
  - `&& (*(this + 72) & (1LL << (a2 % 100))) != 0;`

### UITEM（2 函数）


#### UITEM::0?$ZArray  (120 B)

  - `void __thiscall ZArray<CTradingRoomDlg::ITEM>::ZArray<CTradingRoomDlg::ITEM>(_DWORD *this)`

#### UITEM::1?$ZArray  (151 B)

  - `void __thiscall ZArray<CTradingRoomDlg::ITEM>::~ZArray<CTradingRoomDlg::ITEM>(void *this)`
  - `sub_7C3C3E(a1: this);`

### UEQUIPITEM（1 函数）


#### UEQUIPITEM::1?$ZRef  (426 B)

  - `LONG __thiscall ZRef<CItemInfo::EQUIPITEM>::~ZRef<CItemInfo::EQUIPITEM>(_DWORD *this, int a2)`
  - `v2 = *(this + 1) != 0 ? *(this + 1) - 16 : 0;`
  - `result = InterlockedDecrement(lpAddend: (v2 + 4));`
  - `if ( result == 0 )`
  - `InterlockedIncrement(lpAddend: (v2 + 4));`
  - `return sub_402D2E(a1: v2);`

### ABSORBITEM（1 函数）


#### ABSORBITEM::Update  (6720 B)

  - `signed int __thiscall CAnimationDisplayer::ABSORBITEM::Update(unsigned int *this, int Unknown)`
  - `v41 = *(this + 2);`
  - `v40 = *(this + 3);`
  - `if ( v5 )`
  - `Mob = CMobPool::GetMob(this: dword_BEBFA4, a2: *(this + 1));`
  - `if ( Mob != 0 )`
  - `v45 = *(*(*(Mob + 4) + 16))(a1: Mob + 4);`
  - `v17 = (*(*(v16 + 1) + 16))(a1: v16 + 1);`
  - `v12 = *(v17 + 4) - CMob::GetHeight(this: v16) / 2;`
  - `User = CUserPool::GetUser(this: dword_BEBFA8, a2: v4);`
  - `if ( User != 0 && *(User + 1384) == 0 )`
  - `v8 = *(this + 6);`
  - `if ( v8 > 2 )`
  - `v45 = *(*(v7[1] + 16))(a1: v7 + 1);`
  - `v14 = CAvatar::GetHeight(this: (v7 + 34)) / 2;`
  - `v12 = *((*(*v13 + 16))(a1: v13) + 4) - v14;`
  - …（截断 16 行）

### UGW_ItemSlotBase（1 函数）


#### UGW_ItemSlotBase::1?$ZRef  (378 B)

  - `LONG __thiscall ZRef<GW_ItemSlotBase>::~ZRef<GW_ItemSlotBase>(_DWORD *this, int a2)`
  - `result = InterlockedDecrement(lpAddend: (*(this + 1) + 4));`
  - `if ( result == 0 )`
  - `InterlockedIncrement(lpAddend: (*(this + 1) + 4));`
  - `return sub_402D2E(a1: *(this + 1));`

### GW_ItemSlotBase（2 函数）


#### GW_ItemSlotBase::Decode  (964 B)

  - `int __cdecl GW_ItemSlotBase::Decode(int a1, int a2)`
  - `v2 = CInPacket::Decode1(this: a2);`
  - `sub_4E34E1(a1: v6, a2: v2);`
  - `if ( v7 != 0 )`
  - `(*(*v7 + 112))(a1: v7, a2);`
  - `*(a1 + 4) = v7;`
  - `if ( !v5 )`
  - `InterlockedIncrement(lpAddend: (v4 + 4));`
  - `LOBYTE(v9) = 0;`
  - `if ( v4 != 0 )`
  - `ZRef<GW_ItemSlotBase>::~ZRef<GW_ItemSlotBase>(a1: 0);`
  - `*(a1 + 4) = 0;`

#### GW_ItemSlotBase::Encode  (261 B)

  - `int __thiscall GW_ItemSlotBase::Encode(GW_ItemSlotBase *this, struct COutPacket *a2)`
  - `v3 = (*(*this + 24))(a1: this);`
  - `COutPacket::Encode1(this: a2, a2: v3);`
  - `return (*(*this + 116))(a1: this, a2);`

### AvatarLook（2 函数）


#### AvatarLook::Decode  (1779 B)

  - `int __thiscall AvatarLook::Decode(AvatarLook *this, struct CInPacket *a2)`
  - `*(this + 12) = CInPacket::Decode1(this: a2);`
  - `*(this + 13) = CInPacket::Decode1(this: a2);`
  - `*(this + 17) = CInPacket::Decode4(this: a2);`
  - `memset(a1: this + 25, a2: 0, a3: 0xD0u);`
  - `memset(a1: this + 233, a2: 0, a3: 0xD0u);`
  - `CInPacket::Decode1(this: a2);`
  - `*(this + 25) = CInPacket::Decode4(this: a2);`
  - `while ( 1 )`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `if ( v3 == 0xFF )`
  - `v5 = CInPacket::Decode4(this: a2);`
  - `if ( v3 <= 0x33u && v3 != 0 )`
  - `LOBYTE(v7) = is_correct_bodypart(nItemID: v5, nBodyPart: v3, nGender: 2);`
  - `if ( v7 != 0 )`
  - `*(this + 4 * v3 + 25) = v6;`
  - …（截断 16 行）

#### AvatarLook::Load  (157 B)

  - `int __thiscall AvatarLook::Load(void *this, const struct CharacterData *a2)`
  - `return AvatarLook::Load(this, a2, a3: a2 + 231, a4: a2 + 647);`

### UserLook（1 函数）


#### UserLook::ForceLook  (388 B)

  - `struct CUser *__thiscall CField::UserLook::ForceLook(CField::UserLook *this, struct CUser *a2)`
  - `if ( a2 != 0 )`
  - `CAvatar::ForcingAppearance(this: a2 + 34, a2: 1, a3: this + 57);`
  - `return CAvatar::NotifyAvatarModified(this: (a2 + 136), a2: 1);`

### CChatBalloon（3 函数）


#### CChatBalloon::CreateCanvas  (48398 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1394_NW`, `SP_1395_N`, `SP_1396_NE`, `SP_1397_E`, `SP_1398_W`, `SP_1399_C`, `SP_1400_SW`, `SP_1401_S`, `SP_1402_SE`, `SP_1403_ARROW`, `SP_1410_CANVASFONT`, `SP_1411_BA`, `SP_5527_ARIAL`, `SP_5628_HEAD`, `SP_956_CANVAS`
  - `int __thiscall CChatBalloon::CreateCanvas(_DWORD *this, int a2, _DWORD *a3, int **p_p_Unknown, char a5, char *a6)`
  - `LOBYTE(v263) = 2;`
  - `sub_40263B(a1: &a5);`
  - `IWzProperty::GetItem(pvargDest: &pvarg, a2: v221);`
  - `LOBYTE(v263) = 4;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: p_Unknown, a2: p_pvargDest);`
  - `if ( Unknown != 0 )`
  - `v7 = sub_41E527(a1: &Unknown);`
  - `if ( v7 < 0 && v7 != -2147467262 )`
  - `LOBYTE(v263) = 6;`
  - `if ( v8 < 0 )`
  - `IWzProperty::GetItem(pvargDest: &v235, a2: v221);`
  - `LOBYTE(v263) = 8;`
  - `v258 = Ztl_variant_t::GetUnknown(a1: p_Unknown, a2: p_pvargDest);`
  - `if ( v258 != 0 )`
  - `v11 = sub_41E527(a1: &v258);`
  - …（截断 16 行）

#### CChatBalloon::MakeBalloon  (24237 B)

- SP_ 节点：`SP_1388_UI_CHATBALLOONIMG_`, `SP_1389_NPC`, `SP_1390_ADBOARD`, `SP_1593_MOB`, `SP_5623_DEAD`, `SP_5624_DELAY`, `SP_5643_PET`
  - `int __thiscall CChatBalloon::MakeBalloon(`
  - `v9 = *(this + 21) == 0;`
  - `LOBYTE(v141) = 3;`
  - `if ( !v9 )`
  - `LOBYTE(v141) = 4;`
  - `if ( int32 != 0 )`
  - `LOBYTE(v141) = 5;`
  - `if ( v12 != 0 )`
  - `v13 = *(v12 - 1) >> 1;`
  - `sub_48EF51(a1: v12, a2: v13);`
  - `if ( a6 == 1000 )`
  - `v33 = alloca(32);`
  - `v34 = _itow(a1: a7, a2: v113 + 2, a3: 10);`
  - `v35 = wcslen(a1: v34);`
  - `sub_48EF51(a1: v34, a2: v35);`
  - `if ( a6 == 1001 )`
  - …（截断 16 行）

#### CChatBalloon::MakeMiniRoomBalloon  (74259 B)

- 分支 case：`1`, `2`, `4`, `5`
- SP_ 节点：`SP_1414_UI_CHATBALLOONIMG_MINIROOM`, `SP_1416_OMOK`, `SP_1421_CNUM`, `SP_1422_MNUM`, `SP_1425_PROGRESS`, `SP_3452_ITEM_CASH_0503IMG`, `SP_3458_SIGNBOARD`, `SP_5515_D`, `SP_5619_BACKGRND`, `SP_5624_DELAY`
  - `int __thiscall CChatBalloon::MakeMiniRoomBalloon(`
  - `LOBYTE(v347) = 16;`
  - `ZXString<char>::Format(a1: &v325, a2: v11, a3: a9);`
  - `LOBYTE(v347) = 15;`
  - `if ( Unknown != 0 )`
  - `LOBYTE(v347) = 17;`
  - `ZXString<char>::Format(a1: &v322, a2: v13, a3: i);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v347) = 18;`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v347) = 19;`
  - `v16 = sub_406472();`
  - `v17 = sub_402680(a1: v16);`
  - `IWzResMan::GetObjectA(this: v17, pvargDest: &v311, sUOL: v292, vParam: p_pvarg, vAux: p_pvargDest);`
  - `LOBYTE(v347) = 21;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: p_a2, a2: v296);`
  - …（截断 16 行）

### CChatHelper（4 函数）


#### CChatHelper::HistoryAdd  (1403 B)

  - `void __thiscall CChatHelper::HistoryAdd(void **this, char a2)`
  - `v3 = *(this + 2);`
  - `if ( v3 != 0 )`
  - `v3 = *(v3 - 4);`
  - `if ( v3 - 1 < 0 || ZXString<char>::operator==(a1: &a2) == 0 )`
  - `sub_47CB2D(a1: -1);`
  - `ZXString<char>::operator=(a1: &a2);`
  - `v5 = *(this + 9);`
  - `if ( v5 < 0 )`
  - `if ( *v4 != 0 )`
  - `v6 = *(v6 - 1);`
  - `if ( v5 < v6 && ZXString<char>::operator==(a1: &a2) != 0 )`
  - `*(this + 10) = 1;`
  - `v7 = *(v7 - 1);`
  - `*(this + 10) = 0;`
  - `*(this + 9) = v7;`
  - …（截断 16 行）

#### CChatHelper::HistoryDown  (985 B)

  - `_DWORD *__thiscall CChatHelper::HistoryDown(int *this, _DWORD *a2)`
  - `v2 = *(this + 2);`
  - `if ( v2 != 0 )`
  - `v2 = *(v2 - 4);`
  - `v4 = *(this + 9);`
  - `if ( v4 < 0 )`
  - `if ( v4 < v3 )`
  - `*(this + 9) = ++v4;`
  - `ZXString<char>::operator=(a1: *(this + 2) + 4 * v4);`
  - `if ( v5 && v3 >= 0 )`
  - `ZXString<char>::operator=(a1: &v7);`
  - `LOBYTE(v8) = 0;`

#### CChatHelper::HistoryUp  (930 B)

  - `_DWORD *__thiscall CChatHelper::HistoryUp(int *this, _DWORD *a2)`
  - `if ( *(this + 10) != 0 )`
  - `++*(this + 9);`
  - `*(this + 10) = 0;`
  - `if ( --*(this + 9) < 0 )`
  - `*(this + 9) = 0;`
  - `v3 = *(this + 9);`
  - `if ( v3 >= 0 )`
  - `v4 = *(this + 2);`
  - `if ( v4 != 0 )`
  - `v5 = *(v4 - 4);`
  - `if ( v3 < v5 )`
  - `ZXString<char>::operator=(a1: *(this + 2) + 4 * v3);`
  - `ZXString<char>::operator=(a1: &v7);`
  - `LOBYTE(v8) = 0;`

#### CChatHelper::TryChat  (4570 B)

- SP_ 节点：`SP_272_CURSING_IS_NOT_ALLOWED`, `SP_891_REPEATING_THE_SAME_LINE_OVER_AND_OVER_R_NCAN_NEGATIVELY_AFFECT_OTHER_USERS`, `SP_892_TOO_MUCH_CHATTING_CAN_DISRUPT_R_NOTHER_PLAYERS_ABILITY_TO_PLAY_THE_GAME`, `SP_893_YOU_ARE_CURRENTLY_BLOCKED_FROM_CHATTING`
  - `signed int __thiscall CChatHelper::TryChat(void **this, int a2)`
  - `ZXString<char>::operator=(a1: a2);`
  - `v25 = dword_BF060C(a1: v24);`
  - `if ( (v25 - *(this + 1)) < 2800 )`
  - `if ( *(*dword_BE7918 + 8292) == 0 )`
  - `LOBYTE(v28) = 2;`
  - `Buffer = ZXString<char>::GetBuffer(a1: 1024, a2: 1);`
  - `v7 = CCurseProcess::ProcessString(a1: Buffer, a2: 0, a3: 1);`
  - `ZXString<char>::ReleaseBuffer(a1: -1);`
  - `if ( v7 != 0 )`
  - `if ( (v25 - *this) > 0x7530 )`
  - `ZArray<ZXString<char>>::RemoveAll(a1: this + 3);`
  - `sub_47CB2D(a1: -1);`
  - `ZXString<char>::operator=(a1: v23);`
  - `while ( *v10 != 0 && *(*v10 - 1) > 4u )`
  - `ZArray<ZXString<char>>::RemoveAt(a1: *v10);`
  - …（截断 16 行）

### SKILLENTRY（8 函数）


#### SKILLENTRY::GetEffectUOL  (450 B)

  - `_DWORD *__thiscall SKILLENTRY::GetEffectUOL(int *this, _DWORD *a2, unsigned __int8 a3)`
  - `v4 = sub_761332(a1: a3);`
  - `if ( v4 != 0 )`
  - `v5 = (v4 + 4);`
  - `if ( v6 != 0 )`
  - `InterlockedIncrement(lpAddend: (v6 + 8));`

#### SKILLENTRY::GetLevelData  (5700 B)

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
  - `v10 = sub_402680(a1: v9);`
  - `IWzResMan::GetObjectA(this: v10, pvargDest: &pvarg, sUOL: v32, vParam: v33, vAux: p_pvargDest);`
  - …（截断 16 行）

#### SKILLENTRY::GetRandomAppointedAction  (664 B)

  - `int __fastcall SKILLENTRY::GetRandomAppointedAction(SKILLENTRY *this, int a2, int a3, unsigned int a4)`
  - `LevelData = SKILLENTRY::GetLevelData(this, a2, a3);`
  - `v6 = (LevelData + 360);`
  - `if ( _ZtlSecureFuse<long>(a1: LevelData + 352, a2: *(LevelData + 360)) >= 0 )`
  - `return _ZtlSecureFuse<long>(a1: v7, a2: *v6);`
  - `v9 = *(this + 6);`
  - `if ( v9 != 0 && *(v9 - 4) != 0 )`
  - `return *(*(this + 6) + 4 * (a4 % *(v9 - 4)));`

#### SKILLENTRY::GetScreenEffectUOL  (271 B)

  - `_DWORD *__thiscall SKILLENTRY::GetScreenEffectUOL(_DWORD *this, _DWORD *a2)`
  - `v2 = *(this + 28);`
  - `if ( v2 != 0 )`
  - `InterlockedIncrement(lpAddend: (v2 + 8));`

#### SKILLENTRY::GetSpecialUOL  (266 B)

  - `_DWORD *__thiscall SKILLENTRY::GetSpecialUOL(_DWORD *this, _DWORD *a2)`
  - `v2 = *(this + 39);`
  - `if ( v2 != 0 )`
  - `InterlockedIncrement(lpAddend: (v2 + 8));`

#### SKILLENTRY::IsActionAppointed  (485 B)

  - `BOOL __fastcall SKILLENTRY::IsActionAppointed(SKILLENTRY *this, int a2, int a3)`
  - `LevelData = SKILLENTRY::GetLevelData(this, a2, a3);`
  - `if ( _ZtlSecureFuse<long>(a1: LevelData + 352, a2: *(LevelData + 360)) < 0 )`
  - `v5 = *(this + 6);`
  - `if ( v5 == 0 || *(v5 - 4) == 0 )`

#### SKILLENTRY::IsCorrectAppointedAction  (1096 B)

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

#### SKILLENTRY::IsCorrectWeaponType  (2510 B)

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
  - `if ( v3 >= (&loc_4E23E7 + 2) )`
  - `if ( v3 != &loc_4C4F29 )`
  - …（截断 16 行）

### CSkillInfo（4 函数）


#### CSkillInfo::CheckConsumeForActiveSkill  (7443 B)

  - `signed int __thiscall CSkillInfo::CheckConsumeForActiveSkill(`
  - `if ( (v5 == 0 || v5 == 9)`
  - `&& sub_764721(a1: nSkillID) == 0 )`
  - `cda = CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: cd, a3: nSkillID, a4: &nIncMPCon);`
  - `if ( nSkillID == 21000002 )`
  - `SkillLevel = CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: cd, a3: 21120002, a4: 0);`
  - `if ( SkillLevel != 0 )`
  - `CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: cd, a3: 21120009, a4: &v37);`
  - `SkillLevel = CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: cd, a3: 21110002, a4: 0);`
  - `CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: cd, a3: 21110007, a4: &v37);`
  - `else if ( nSkillID == 21100001 )`
  - `CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: cd, a3: 21120010, a4: &v37);`
  - `CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: cd, a3: 21110008, a4: &v37);`
  - `if ( cda <= 0 )`
  - `if ( nSkillID == 1121010 && _ZtlSecureFuse<long>(a1: a4 + 219, a2: a4[221]) < 10 )`
  - `LevelData = SKILLENTRY::GetLevelData(a1: cda);`
  - …（截断 16 行）

#### CSkillInfo::GetSkill  (514 B)

  - `int __thiscall CSkillInfo::GetSkill(CSkillInfo *this, int a2)`
  - `if ( a2 / 10000 / 100 == 22 || a2 / 10000 == 2001 )`
  - `sub_76716A(a1: &a2, a2: v4);`
  - `if ( v5 != 0 )`
  - `sub_766D3C(a1: 0);`

#### CSkillInfo::GetSkillLevel  (2217 B)

  - `int __thiscall CSkillInfo::GetSkillLevel(`
  - `if ( a3 / 10000 / 100 == 22 || a3 / 10000 == 2001 )`
  - `if ( a3 > 20001011 )`
  - `if ( a3 == 20001020 )`
  - `if ( a3 > 20011008 )`
  - `if ( a3 > 20011011 )`
  - `if ( !v4 )`
  - `if ( a4 != 0 )`
  - `*v8 = CSkillInfo::GetSkill(this: v9, a2: a3);`
  - `if ( a3 >= 20001009 )`
  - `if ( a3 >= 1009 )`
  - `if ( a3 <= 1011 || a3 == 1020 )`
  - `if ( a3 > &loc_989A70 )`
  - `if ( a3 <= &loc_989A70 + 3 )`
  - `if ( Skill == 0 )`
  - `Skill = CSkillInfo::GetSkill(this: v9, a2: a3);`
  - …（截断 16 行）

#### CSkillInfo::GetSkillRootVisible  (1015 B)

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

### SkillInfo（2 函数）


#### SkillInfo::Parse  (5385 B)

  - `int __thiscall Field::SkillInfo::Parse(char *this, int a2)`
  - `if ( a2 != 0 )`
  - `_bstr_t::_bstr_t(a1: aClass);`
  - `LOBYTE(v40) = 0;`
  - `sub_40263B(a1: &a2);`
  - `IWzProperty::GetItem(pvargDest: &pvarg, a2: v27);`
  - `LOBYTE(v40) = 2;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v28, a2: v29);`
  - `if ( Unknown != 0 )`
  - `v3 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v3 < 0 && v3 != -2147467262 )`
  - `LOBYTE(v40) = 4;`
  - `if ( v4 < 0 )`
  - `if ( v37 != 0 )`
  - `v39 = IWzProperty::Getcount(a1: v37);`
  - `for ( i = 0; i < v39; ++i )`
  - …（截断 16 行）

#### SkillInfo::SetFieldType  (141 B)

  - `int __thiscall Field::SkillInfo::SetFieldType(Field::SkillInfo *this, int a2)`
  - `*(this + 10) = a2;`

### CInPacket（9 函数）


#### CInPacket::AppendBuffer  (1965 B)

  - `int __thiscall CInPacket::AppendBuffer(int this, _DWORD *a2, size_t a3, int a4)`
  - `if ( a3 != 0 )`
  - `*a3 = *(this + 4);`
  - `v7 = *(v6 + 16);`
  - `v8 = *(v6 + 12);`
  - `if ( *(this + 4) == 0 )`
  - `v9 = *(this + 12);`
  - `a3 = *(v6 + 12);`
  - `if ( v8 >= 4 - v9 )`
  - `CInPacket::RawAppendBuffer(a1: v7, a2: a3);`
  - `if ( *(this + 12) >= 4u )`
  - `*(this + 4) = 1;`
  - `v10 = CInPacket::Decode2(this);`
  - `*(this + 14) = v10;`
  - `if ( v11 )`
  - `v12 = CInPacket::Decode2(this);`
  - …（截断 16 行）

#### CInPacket::Decode1  (418 B)

  - `char __thiscall CInPacket::Decode1(int this)`
  - `v1 = *(this + 20);`
  - `if ( *(this + 12) == v1 )`
  - `_CxxThrowException(a1: &v3, a2: &_TI1_AVZException__);`
  - `result = *(v1 + *(this + 8));`
  - `*(this + 20) = v1 + 1;`

#### CInPacket::Decode2  (431 B)

  - `__int16 __thiscall CInPacket::Decode2(CInPacket *this)`
  - `v1 = *(this + 5);`
  - `if ( *(this + 6) - v1 < 2 )`
  - `_CxxThrowException(a1: &v3, a2: &_TI1_AVZException__);`
  - `result = *(v1 + *(this + 2));`
  - `*(this + 5) = v1 + 2;`

#### CInPacket::Decode4  (420 B)

  - `int __thiscall CInPacket::Decode4(int this)`
  - `v1 = *(this + 20);`
  - `if ( *(this + 12) - v1 < 4 )`
  - `_CxxThrowException(a1: &v3, a2: &_TI1_AVZException__);`
  - `result = *(v1 + *(this + 8));`
  - `*(this + 20) = v1 + 4;`

#### CInPacket::DecodeBuffer  (519 B)

  - `void *__thiscall CInPacket::DecodeBuffer(int this, void *a2, size_t a3)`
  - `v4 = *(this + 12);`
  - `v5 = *(this + 20);`
  - `if ( v4 - v5 < a3 )`
  - `_CxxThrowException(a1: &a3, a2: &_TI1_AVZException__);`
  - `result = memcpy(a1: a2, a2: (v5 + *(this + 8)), a3);`
  - `*(this + 20) += v6;`

#### CInPacket::DecodeStr  (658 B)

  - `_DWORD *__thiscall CInPacket::DecodeStr(int this, _DWORD *a2)`
  - `v2 = *(this + 20);`
  - `v5 = *(this + 12) - v2;`
  - `v4 = (v2 + *(this + 8));`
  - `*(this + 20) += CIOBufferManipulator::DecodeStr(a1: &v6, a2: v4, a3: v5);`
  - `ZXString<char>::operator=(a1: &v6);`
  - `LOBYTE(v7) = 0;`

#### CInPacket::DecryptData  (1990 B)

  - `signed int __thiscall CInPacket::DecryptData(CInPacket *this, unsigned int a2)`
  - `v3 = *(this + 8);`
  - `if ( v3 >= 0x5B0 )`
  - `if ( *(this + 8) == 0 )`
  - `v5 = *(this + 2) + 4;`
  - `CAESCipher::Decrypt(a1: v5, a2: v5, a3: v18, a4: &a2);`
  - `while ( 1 )`
  - `if ( v3 == 0 )`
  - `if ( v3 < 0x5B4 )`
  - `CAESCipher::Decrypt(a1: v5, a2: v5, a3: 1460, a4: &a2);`
  - `v17 = *(this + 2) + 4;`
  - `v19 = *(this + 8);`
  - `v6 = (v19 + v17);`
  - `v9 = __ROL1__(*--v6, 3) ^ 0x13;`
  - `v10 = __ROR1__((v11 ^ v9) - v13, 4);`
  - `while ( v13 != 0 );`
  - …（截断 16 行）

#### CInPacket::RawAppendBuffer  (824 B)

  - `void *__thiscall CInPacket::RawAppendBuffer(int this, void *a2, size_t a3)`
  - `v4 = *(this + 8);`
  - `v5 = (this + 8);`
  - `if ( v4 != 0 )`
  - `v4 = *(v4 - 4);`
  - `v7 = a3 + *(this + 12);`
  - `if ( v7 > v4 )`
  - `if ( *v5 != 0 )`
  - `v8 = *(v8 - 4);`
  - `while ( v7 > v8 );`
  - `ZArray<unsigned char>::_Realloc(a1: v8, a2: 0, a3: &a3 + 3);`
  - `result = memcpy(a1: (*v5 + *(this + 12)), a2, a3: v6);`
  - `*(this + 12) += v6;`

#### CInPacket::WriteClientLog  (2096 B)

  - `void __thiscall CInPacket::WriteClientLog(CInPacket *this)`
  - `ExceptionFileName = CWvsApp::GetExceptionFileName(a1: this);`
  - `v2 = dword_BF0354(a1: ExceptionFileName, a2: 0x40000000, a3: 0, a4: 0, a5: 4, a6: 0x80000000, a7: 0);`
  - `if ( v2 != -1 )`
  - `dword_BF0368(a1: v2, a2: 0, a3: 0, a4: 2);`
  - `ZXString<char>::GetBuffer(a1: aInvalidDecodin, a2: 0xFFFFFFFF);`
  - `v4 = sub_6EC7B1(a1: v10);`
  - `LOBYTE(v15) = 1;`
  - `sub_47C957(a1: &v11, a2: aPacketdump);`
  - `LOBYTE(v15) = 2;`
  - `ZXString<char>::operator+(a1: v12, a2: v4);`
  - `LOBYTE(v15) = 3;`
  - `v5 = sub_47C957(a1: &v13, a2: asc_AF1E28);`
  - `LOBYTE(v15) = 4;`
  - `ZXString<char>::operator=(a1: v5);`
  - `LOBYTE(v15) = 0;`
  - …（截断 16 行）

### COutPacket（8 函数）


#### COutPacket::Encode1  (239 B)

  - `int __thiscall COutPacket::Encode1(_DWORD *this, char a2)`
  - `sub_406567(this, a2: 1);`
  - `result = *(this + 1);`
  - `*(result + (*(this + 2))++) = a2;`

#### COutPacket::Encode2  (284 B)

  - `int __thiscall COutPacket::Encode2(COutPacket *this, unsigned __int16 a2)`
  - `sub_406567(this, a2: 2);`
  - `result = *(this + 1);`
  - `*(result + *(this + 2)) = a2;`
  - `*(this + 2) += 2;`

#### COutPacket::Encode4  (267 B)

  - `int __thiscall COutPacket::Encode4(_DWORD *this, int a2)`
  - `sub_406567(this, a2: 4);`
  - `result = *(this + 1);`
  - `*(result + *(this + 2)) = a2;`
  - `*(this + 2) += 4;`

#### COutPacket::EncodeBuffer  (282 B)

  - `void *__thiscall COutPacket::EncodeBuffer(_DWORD *this, void *a2, size_t a3)`
  - `sub_406567(this, a2: a3);`
  - `result = memcpy(a1: (*(this + 2) + *(this + 1)), a2, a3);`
  - `*(this + 2) += a3;`

#### COutPacket::EncodeStr  (666 B)

  - `void __thiscall COutPacket::EncodeStr(_DWORD *this, int a2)`
  - `if ( a2 != 0 )`
  - `v3 = *(a2 - 4);`
  - `sub_406567(this, a2: v3 + 2);`
  - `v6 = *(this + 2) + *(this + 1);`
  - `sub_428211(a1: &a2);`
  - `*(this + 2) += sub_46F435(a1: v5, a2: v6);`

#### COutPacket::Init  (362 B)

  - `void __thiscall COutPacket::Init(COutPacket *this, int a2, int a3, int a4)`
  - `*(this + 2) = 0;`
  - `if ( a2 != 0x7FFFFFFF )`
  - `if ( a4 != 0 )`
  - `COutPacket::InitByte(this, a2, a3);`
  - `COutPacket::Encode2(this, a2);`
  - `*(this + 3) = 0;`

#### COutPacket::InitByte  (306 B)

  - `int __thiscall COutPacket::InitByte(COutPacket *this, int a2, int a3)`
  - `*(this + 2) = 0;`
  - `if ( a2 != 0x7FFFFFFF )`
  - `return COutPacket::Encode1(this, a2);`

#### COutPacket::MakeBufferList  (3696 B)

  - `int __userpurge COutPacket::MakeBufferList@<eax>(`
  - `*(a2 - 6) = v9 + 4;`
  - `*(a2 - 5) = a1[1];`
  - `if ( !v7 && a1[3] == 0 )`
  - `*(a2 - 7) = v9;`
  - `v12 = *(a2 - 7);`
  - `v14 = *(a2 - 5);`
  - `v15 ^= v12 + __ROL1__(*v14, 3);`
  - `*v14++ = 71 - __ROR1__(v15, v12--);`
  - `while ( v12 != 0 );`
  - `v10 = *(a2 - 7);`
  - `v13 ^= v10 + __ROL1__(*--v14, 4);`
  - `*v14 = __ROR1__(v13 ^ 0x13, 3);`
  - `while ( v10 != 0 );`
  - `while ( v30 != 1 );`
  - `if ( *(a2 - 6) < 0x5B4u )`
  - …（截断 16 行）

## 3. 坐标 / 尺寸 / 按钮 id / 封包字段候选行（宠物召唤龙全函数）


### ABSORBITEM::Update
  - `v12 = *(v17 + 4) - CMob::GetHeight(this: v16) / 2;`
  - `v14 = CAvatar::GetHeight(this: (v7 + 34)) / 2;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: 0, a2: 0);`

### AvatarLook::Decode
  - `memset(a1: this + 25, a2: 0, a3: 0xD0u);`

### CActionFrame::ExtractMap
  - `*(v11 + 4) = (v12 + IWzShape2D::Getx(a1: Unknown)) / 2;`
  - `*(v11 + 8) = (v14 + IWzShape2D::Gety(a1: Unknown)) / 2;`

### CActionMan::GetWeaponAfterImage
  - `memset(a1: *(v76 + 16), a2: 0, a3: 2592u);`

### CActionMan::Init
  - `v106 = *(CActionMan::GetCharacterImgEntry(this, a2: v110, a3: 2000) + 4);`
  - `v53[4] = get_int32(pvarSrc: v59, a2: v81) != 0;`

### CActionMan::LoadFaceLook
  - `IWzProperty::GetItem(pvargDest: *&v259[12], a2: *&v259[16]);`
  - `Item = IWzProperty::GetItem(pvargDest: *&v259[12], a2: *&v259[16]);`
  - `StringPool::GetBSTR(this: Instance, a2: *&v259[8], a3: *&v259[12]);`
  - `v42 = IWzProperty::GetItem(pvargDest: *&v259[12], a2: *&v259[16]);`
  - `StringPool::GetBSTR(this: v45, a2: *&v259[4], a3: *&v259[8]);`
  - `v320 = Ztl_variant_t::GetUnknown(a1: v259[12], a2: v259[16]);`

### CActionMan::MergeCharacterSprite
  - `CActionFrame::Draw(this: v9, a2: (v8 + 12), a3: (v8 + 16), a4: (v8 + 28), a5: (v8 + 20), a6: (v8 + 36), a7: 0);`

### CActionMan::MergeTamingMobSprite
  - `CActionFrame::Draw(this: (v4 + *v3), a2: (v7 + 12), a3: (v7 + 16), a4: (v7 + 40), a5: &v11, a6: (v7 + 48), a7: 1);`

### CAffectedAreaPool::OnAffectedAreaCreated
  - `CAnimationDisplayer::MakeLayer_FootHold(a1: v69, a2: v84, a3: (v67 + 15), a4: 20, a5: 0);`

### CAnimationDisplayer::LoadSingleLayer
  - `ObjectA = IWzResMan::GetObjectA(this: v12, pvargDest: &pvarg, sUOL: Hi32, vParam: v42[0], vAux: SHIDWORD(v42[0]));`

### CAvatar::GetHeight
  - `if ( v12 != 0 && IWzCanvas::Getcy(this: v12) > 0 )`

### CAvatar::NotifyAvatarModified
  - `CAvatar::SetEmotion(this, nEmotion: *(this + 1066), tDuration: -1);`

### CAvatar::SetMorphed
  - `CAvatar::SetEmotion(this, nEmotion: 0, tDuration: -1);`

### CAvatar::SetMoveAction
  - `CAvatar::SetEmotion(this, nEmotion: 0, tDuration: -1);`

### CAvatar::Update
  - `v37 = 1500 * (CAvatar::GetOneTimeAction(this) > -1);`
  - `CAvatar::SetEmotion(this, nEmotion: 0, tDuration: -1);`
  - `if ( IWzGr2DLayer::GetFlip(this: *(this + 1077)) != 0 )`
  - `if ( IWzGr2DLayer::GetFlip(this: *(this + 1072)) != 0 && *(*(v171 + 20) + v170 + 20) != 0 )`
  - `v9 = (this + 1500 * (CAvatar::GetOneTimeAction(this) > -1) + 1264);`

### CAvatarMegaphone::Draw
  - `IWzCanvas::DrawTextA(this: v5, nLeft: 124, nTop: 14, sText: v43, pFont: v44, vAlpha: p_pvargDest, vTabOrg: p_pvarg);`

### CAvatarMegaphone::HelloAvatarMegaphone
  - `Buffer = ZXString<char>::GetBuffer(a1: 1024, a2: 1);`

### CChatBalloon::MakeMiniRoomBalloon
  - `a9 = Ztl_variant_t::GetUnknown(a1: v285[8], a2: v285[12]);`

### CChatHelper::TryChat
  - `Buffer = ZXString<char>::GetBuffer(a1: 1024, a2: 1);`

### CDraggableItem::GetOffEquipItem
  - `v7 = *(CharacterData::GetItem(this: v6, a2: v68, a3: 1, a4: *(this + 7)) + 4);`
  - `v80 = *(CharacterData::GetItem(this: v6, a2: v68, a3: 1, a4: SlotPositionFromPoint) + 4);`
  - `&& (TSecType<long>::GetData(a1: (v7 + 12), a2: 10000) / 10000 == 190`
  - `|| TSecType<long>::GetData(a1: (v7 + 12), a2: 10000) / 10000 == 191) )`
  - `v17 = TSecType<long>::GetData(a1: (v16 + 12), a2: 100);`
  - `if ( ExclusiveWeaponShieldBodyPart != 0 && CharacterData::FindEmptySlotPosition(a1: 1) == 0 )`
  - `CWvsContext::SendChangeSlotPositionRequest(this: v72, a2: 1, a3: v71[7], a4: SlotPositionFromPoint, a5: -1);`

### CDraggableItem::MapFuncKey
  - `v6 = *(CharacterData::GetItem(this: v5, a2: &v25, a3: *(v4 + 6), a4: *(v4 + 7)) + 4);`
  - `Data = TSecType<long>::GetData(a1: (v6 + 12), a2: 0);`

### CDraggableItem::MoveItemSlot
  - `if ( a2[377] != v7 || *(this + 7) == SlotPositionFromPoint )`
  - `v8 = *(CharacterData::GetItem(this: v6, a2: &v10, a3: v7, a4: *(this + 7)) + 4);`

### CDraggableItem::OnDoubleClicked
  - `v94 = *(CharacterData::GetItem(this: v93, a2: &v121, a3: 1, a4: *(this + 7)) + 4);`
  - `Data = TSecType<long>::GetData(a1: (v94 + 12), a2: 0);`
  - `v8 = *(CharacterData::GetItem(this: v7, a2: &v121, a3: *(this + 6), a4: *(this + 7)) + 4);`
  - `v46 = TSecType<long>::GetData(a1: (v8 + 12), a2: v8 + 12);`
  - `if ( TSecType<long>::GetData(a1: (v8 + 12), a2: 10000) / 10000 == 219 )`
  - `&& (v128 = (v8 + 12), TSecType<long>::GetData(a1: (v8 + 12), a2: 10000) / 10000 == 224) )`
  - `&& ((v128 = (v8 + 12), (v63 = TSecType<long>::GetData(a1: (v8 + 12), a2: 10000) / 10000) == 545)`
  - `&& TSecType<long>::GetData(a1: (v8 + 12), a2: 10000) / 10000 == 408 )`
  - `else if ( TSecType<long>::GetData(a1: v128, a2: 10000) / 1000 == 4220 )`
  - `bodypart_from_item = get_bodypart_from_item(a1: v69, a2: *(v126 + 17), a3: v112, a4: 1);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v85, a2: 6, a3: sub_897B50, a4: v109, a5: v110, a6: v111[0]);`
  - `v111[0] = *StringPool::GetStringW(this: v89, a2: v110, a3: v111[0]);`

### CDraggableItem::OnDropped
  - `if ( v21 != 0 && v5 == (TSingleton<CWndMan>::ms_pInstance != 0 ? TSingleton<CWndMan>::ms_pInstance + 4 : 0) )`
  - `if ( v33 != 0 && a3 == (TSingleton<CWndMan>::ms_pInstance != 0 ? TSingleton<CWndMan>::ms_pInstance + 4 : 0) )`
  - `if ( a3 == (TSingleton<CWndMan>::ms_pInstance != 0 ? TSingleton<CWndMan>::ms_pInstance + 4 : 0) )`

### CDraggableItem::PutItem
  - `v9 = *(CharacterData::GetItem(this: v8, a2: &v24, a3: *(this + 6), a4: *(this + 7)) + 4);`
  - `Data = TSecType<long>::GetData(a1: v9 + 12, a2: v9 + 12);`

### CDraggableItem::ThrowItem
  - `if ( get_field() != 0 && pUIEquip != 0 && *(get_field() + 356) != 0 )`
  - `v8 = *(CharacterData::GetItem(this: v7, a2: v53, a3: *(this + 6), a4: *(this + 7)) + 4);`
  - `&& (TSecType<long>::GetData(a1: pUIEquip + 12, a2: 0) / 10000 == 190`
  - `|| TSecType<long>::GetData(a1: pUIEquip + 12, a2: 0) / 10000 == 191) )`
  - `Data = TSecType<long>::GetData(a1: pUIEquip + 12, a2: 0);`
  - `|| TSecType<long>::GetData(a1: v11, a2: 0) / 10000 == 224`
  - `if ( TSecType<long>::GetData(a1: v11, a2: 0) / 1000 == 4220 )`
  - `if ( (TSecType<unsigned char>::GetData(this: v60 + 2060) & 1) == 0 || sub_4F38B4(this: v25) )`
  - `if ( get_field() != 0 && *(get_field() + 356) != 0 )`
  - `if ( TSecType<long>::GetData(a1: v58, a2: 0) / 10000 != 430`
  - `&& ((TSecType<unsigned char>::GetData(this: v60 + 2060) & 1) == 0 || sub_4F38B4(this: v60)) )`
  - `&& (TSecType<unsigned char>::GetData(this: v60 + 2060) & 1) == 0 )`
  - `CWvsContext::SendChangeSlotPositionRequest(this: v60, a2: v43[6], a3: v43[7], a4: 0, a5: v39);`

### CDraggableItem::UnmapFuncKey
  - `v4 = *(CharacterData::GetItem(this: v3, a2: &v16, a3: v2[6], a4: v2[7]) + 4);`
  - `Data = TSecType<long>::GetData(a1: (v4 + 12), a2: 0);`
  - `if ( v6 == 5 && get_etc_cash_item_type(a1: v19) != 6 )`

### CDraggableItem::WearEquipItem
  - `v6 = *(CharacterData::GetItem(this: *v134, a2: v119, a3: 1, a4: a2) + 4);`
  - `&& (TSecType<long>::GetData(a1: (v128 + 12), a2: 1000) / 10000 == 190`
  - `|| TSecType<long>::GetData(a1: (v128 + 12), a2: 1000) / 10000 == 191) )`
  - `v131 = *(CharacterData::GetItem(this: *v134, a2: v126, a3: 1, a4: v57) + 4);`
  - `if ( v131 != 0 && CharacterData::FindEmptySlotPosition(a1: 1) == 0 )`
  - `v68 = TSecType<long>::GetData(a1: (v136 + 12), a2: 8);`
  - `CWvsContext::SendChangeSlotPositionRequest(this: v123, a2: 1, a3: a2, a4: v130, a5: -1);`

### CDraggableSkill::OnDropped
  - `if ( *(this + 7) == 0 && CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v6, a3: *(this + 6), a4: 0) <= 0 )`
  - `if ( !v33 && a3 == (TSingleton<CWndMan>::ms_pInstance != 0 ? TSingleton<CWndMan>::ms_pInstance + 4 : 0) && v51[7] == 0 )`

### CDragon::NeedToTalk
  - `CAvatar::GetBodyRect(this: v5 + 34, a2: &v14, a3: 1);`

### CDragon::Update
  - `CDragon::SetMoveAction(this: a1, a2: *(v5 + 82), a3: 0);`

### CDropPool::OnDropEnterField
  - `Data = TSecType<long>::GetData(a1: v115 + 12, a2: 0);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: SLOBYTE(v89[0]), a2: SBYTE4(v89[0]));`
  - `ItemIcon = CItemInfo::GetItemIcon(a1: &v113, a2: *(v115 + 13), bPetDead: 0, bWithShadow: SHIDWORD(v89[0]));`
  - `MoneyIcon = CDropPool::GetMoneyIcon(a1: &v112, a2: *(v115 + 13), a3: 0);`
  - `v49 = TSecType<long>::GetData(a1: v47 + 104, a2: 0);`
  - `CDropPool::MakeMoneyAnimation(this: v105, a2: SLOBYTE(v89[0]), a3: SHIDWORD(v89[0]));`
  - `CInPacket::DecodeBuffer(a1: v115 + 128, a2: HIDWORD(v89[0]));`
  - `memset(a1: v115 + 128, a2: 0, a3: HIDWORD(v89[0]));`

### CDropPool::TryPickUpDrop
  - `Data = TSecType<long>::GetData(a1: &result[14].y, a2: 0);`

### CInPacket::WriteClientLog
  - `Buffer = ZXString<char>::GetBuffer(a1: 0, a2: 1);`

### CMob::DoAttack
  - `CWvsPhysicalSpace2D::GetFootholdRange(this: v82, x: v46, y1: *(v48 + 4) - 100, y2: v73[0], lPosition: v73[1]);`
  - `CAvatar::GetBodyRect(this: (TSingleton<CUserLocal>::ms_pInstance + 136), a2: &v81, a3: 0);`

### CMob::GenerateMovePath
  - `if ( CMovePath::IsTimeForFlush(this: v72, a2: v41 == 3) != 0 )`
  - `COutPacket::Encode1(this: &v56, a2: v71 | (8 * (a10 | (2 * v70))));`
  - `COutPacket::Encode1(this: &v56, a2: (v44 == 0) | (16 * (v45 != 0)));`
  - `CMovePath::Flush(this: v72, a2: &v56, a3: v49 == 3, a4: (v60 + 428));`

### CMob::Init
  - `*(this + 239) = _ZtlSecureTear<long>(a1: FineMoveDirAction, a2: this + 948);`
  - `PcCreateObject::IWzVector2D(a1: v17, a2: this + 288, a3: 0);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v94[0], a2: SBYTE4(v94[0]));`

### CMob::IsTargetInAttackRange
  - `CAvatar::GetBodyRect(this: (TSingleton<CUserLocal>::ms_pInstance + 136), a2: &v91, a3: 0);`
  - `CAvatar::GetBodyRect(this: (v9 + 136), a2: &v91, a3: 0);`
  - `CWvsPhysicalSpace2D::GetFootholdRange(this: v81, x: v34, y1: *(v36 + 4) - 100, y2: v62, lPosition: v70);`

### CMob::OnDoomed
  - `CMovePath::SetStatChangedPoint(this: (*(this + 71) != 0 ? *(this + 71) - 12 + 428 : 428), a2: *(this + 1324));`
  - `*(this + 99) = CMobTemplate::GetMobTemplate(a1: 5);`
  - `*(this + 85) = get_update_time() + 1200;`
  - `CMob::SetMoveAction(this, a2: v6[82], a3: 1);`
  - `CMovePath::DiscardByInterrupt(this: (v6 + 107), a2: v11, a3: v6, a4: 0);`

### CMob::OnHit
  - `&& CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v16, a3: 21110003, a4: 0) != 0`
  - `&& *(get_field() + 344) == 0);`
  - `CMob::GenerateMovePath(a1: v21, a2: a4, a3: a5 == 0, a4: 0, a5: a9, a6: a10, a7: a12, a8: a13, a9: a14, a10: v63);`

### CMob::OnMove
  - `MOBSKILLLEVELDATA::GetEffectUOL(a1: 92 * v27 + *(v31 + 4) - 92, a2: &v44);`

### CMob::OnStatReset
  - `if ( MobStat::IsMovementAffectingStat() != 0 && *(this + 330) != 0 )`
  - `return CMob::ProcessStatReset(a1: this, a2: v4, a3: v5[0], a4: v5[1], a5: v5[2], a6: v6);`

### CMob::OnStatSet
  - `if ( MobStat::IsMovementAffectingStat() != 0 && *(this + 330) != 0 )`
  - `return CMob::ProcessStatSet(a1: this, a2: v4, a3: v5[0], a4: v5[1], a5: v5[2], a6: v6);`

### CMob::ProcessStatReset
  - `MobStat::SetFrom(a1: this + 104, a2: *(this + 98));`
  - `CMovePath::SetStatChangedPoint(this: (v14 != 0 ? v14 - 12 + 428 : 428), a2: v13);`

### CMob::ProcessStatSet
  - `MobStat::DecodeTemporary(a1: this + 104, a2: v47[0], a3: v47[1], a4: v48, a5: HIDWORD(v48), a6: v49, a7: v50);`
  - `CMovePath::SetStatChangedPoint(this: *(this + 71) != 0 ? (*(this + 71) + 104) : 428, a2: a6);`

### CMob::SetActive
  - `CMovePath::DiscardByInterrupt(this: (v3 + 428), a2: v13, a3: v3, a4: 0);`

### CMob::SetFrameInfo
  - `if ( IWzGr2DLayer::GetFlip(this: *(this + 1216)) != 0 )`

### CMob::SetMoveAction
  - `if ( a3 != 0 || (result = _ZtlSecureFuse<long>(a1: this + 948, a2: *(this + 239)), FineMoveDirAction != result) )`

### CMob::SetTemporaryStat
  - `MobStat::Reset(a1: this + 416, a2: v14[0], a3: v14[1], a4: v15, a5: update_time);`
  - `MobStat::SetFrom(a1: this + 416, a2: *(this + 98));`
  - `MobStat::DecodeTemporary(a1: this + 416, a2: v12, a3: v13, a4: v14[0], a5: v14[1], a6: v15, a7: update_time);`
  - `MobStat::AdjustDamagedElemAttr(this: (this + 416), a2: *(this + 148), a3: v17);`

### CMob::ShowDamage
  - `v9 = -15 - a3 * (bHalfHeight != 0 ? 15 : 30) + IWzShape2D::Gety(a1: *(this + 72));`

### CMob::TryDoingSkill
  - `MOBSKILLLEVELDATA::GetEffectUOL(a1: *(v12 + 4) + 92 * *(this + 82) - 92, &a2);`

### CMob::Update
  - `if ( _ZtlSecureFuse<CLadderOrRope *>(a1: v177 + 73, a2: *(v177 + 75)) != 0 )`
  - `|| *(v181 + 592) != 0 && CMob::IsTargetInAttackRange(this, a2: 3, a3: &v183, a4: &v180, a5: &i) != 0 )`
  - `CMob::GenerateMovePath(a1: v54, a2: v62, a3: v139, a4: 0, a5: 0, a6: 0, a7: 0, a8: 0, a9: 0, a10: 0);`
  - `CMob::SetMoveAction(this, a2: *(v177 + 82), a3: 0);`
  - `CMob::GenerateMovePath(a1: this, a2: -1, a3: 0, a4: 0, a5: 0, a6: 0, a7: 0, a8: 0, a9: 0, a10: 0);`
  - `if ( timeGetTime() != 0 && *(this + 311) != 0 && i != 0 && *(this + 311) + 20000 < dword_BF060C(a1: v131) )`

### CMobPool::FindBodyAttackMob
  - `v22 = (*(get_field() + 324) & 2) != 0;`

### CPet::CursedChatCommand
  - `if ( CVecCtrl::IsOnLadder(this: *(this + 36) != 0 ? (*(this + 36) - 12) : 0) != 0`
  - `|| CVecCtrl::IsOnRope(this: *(this + 36) != 0 ? (*(this + 36) - 12) : 0) != 0 )`

### CPet::DoAction
  - `if ( CVecCtrl::IsOnLadder(this: *(this + 144) != 0 ? (*(this + 144) - 12) : 0) != 0`
  - `|| CVecCtrl::IsOnRope(this: *v11 != 0 ? (*v11 - 12) : 0) != 0 )`

### CPet::GetItemSlot
  - `CashItemSlotPosition = CharacterData::FindCashItemSlotPosition(this: v3, a2: 5, a3: *(this + 20));`
  - `v5 = *(CharacterData::GetItem(this: v3, a2: &v6, a3: 5, a4: CashItemSlotPosition) + 4);`

### CPet::Init
  - `CVecCtrlPet::Init(this: Instance, a2: this != 0 ? (this + 4) : 0, a3: *(v17 + 8));`
  - `v36 = _ZtlSecureFuse<CLadderOrRope *>(a1: (v15 + 292), a2: *(v15 + 300));`

### CPet::LoadLayer
  - `CAvatar::GetLayerUnderFace(this: (*(v69 + 37) + 136), a2: &v56);`

### CPet::OnMove
  - `CMovePath::OnMovePacket(this: (*(this + 36) != 0 ? *(this + 36) - 12 + 428 : 428), a2);`

### CPet::OnResolveMoveAction
  - `if ( _ZtlSecureFuse<CLadderOrRope *>(a1: pvc + 73, a2: *(pvc + 75)) != 0 )`
  - `if ( _ZtlSecureFuse<CLadderOrRope *>(a1: (v9 + 292), a2: *(v9 + 300)) != 0 )`
  - `return v7 & 1 | (2 * (CVecCtrl::IsSwimming(a1: v5) != 0 ? 6 : 3));`

### CPet::SetSetItemBackground
  - `LayerUnderFace = CAvatar::GetLayerUnderFace(this: (v67[37] + 136), a2: &v62);`
  - `v34 = CAvatar::GetLayerUnderFace(this: (v67[37] + 136), &a2);`

### CPet::SetSetItemEffect
  - `v12 = *(AdditionalLayer + 5) == 0;`

### CPet::Update
  - `v5 = _ZtlSecureFuse<CLadderOrRope *>(a1: (v3 + 292), a2: v28) == 0;`
  - `CPet::SetMoveAction(this, a2: *(v31 + 82), a3: 0);`

### CPet::UpdatePetAbility
  - `|| (v27 = *(this + 82), v26 = this + 320, result = timeGetTime(), v31 != result)`
  - `|| (v27 = *(this + 85), v26 = this + 332, result = timeGetTime(), v30 != result) )`

### CPortalList::RestorePortal
  - `Crc32 = CCrc32::GetCrc32(a1: &Data, a2: 4, a3: 0, a4: 0, a5: 0);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v109[0], a2: SBYTE4(v109[0]));`
  - `Item = IWzProperty::GetItem(pvargDest: v109[0], a2: SHIDWORD(v109[0]));`
  - `v19 = IWzProperty::GetItem(pvargDest: v109[0], a2: SHIDWORD(v109[0]));`
  - `v146 = CCrc32::GetCrc32(a1: &v138, a2: 4, a3: 0, a4: 0, a5: 0);`
  - `p_Hi32 = CCrc32::GetCrc32(a1: &v147, a2: 4, a3: 0, a4: 0, a5: 0);`
  - `v146 = CCrc32::GetCrc32(a1: &p_Hi32, a2: 4, a3: 0, a4: 0, a5: 0);`
  - `Buffer = ZXString<char>::GetBuffer(a1: 0, a2: 1);`
  - `v3[16] ^= CCrc32::GetCrc32(a1: Buffer, a2: v78, a3: 0, a4: 0, a5: 0);`
  - `String = StringPool::GetString(this: v101, result: v109[0], nIdx: SHIDWORD(v109[0]));`

### CSkillInfo::GetSkillLevel
  - `if ( ZMap<long,long,long>::GetAt(a1: &a3, a2: &a4) == 0 || a4 == 0 )`

### CStage::OnSetField
  - `if ( TSecType<unsigned char>::GetData(this: (v2 + 8252)) < 0 )`
  - `CUtilDlgEx::SetUtilDlgEx_TEXT(this: v104.cyVal.Hi, a2: 0, a3: 0);`
  - `if ( TSecType<unsigned long>::GetData(a1: field + 312) != 0 )`
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: v68 != 0 ? v95 : 0);`
  - `&& TSecType<unsigned long>::GetData(a1: v99 + 312) == 0`

### CSummoned::ProcessAttack
  - `v12 = *StringPool::GetStringW(this: Instance, a2: SLODWORD(v74[0]), a3: SHIDWORD(v74[0]));`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: SLOBYTE(v74[0]), a2: SBYTE4(v74[0]));`
  - `if ( v103 != 0 && IWzProperty::Getcount(a1: v103) != 0 )`
  - `v104 = Ztl_variant_t::GetUnknown(a1: 0, a2: 0) == 0;`
  - `v58 = *StringPool::GetStringW(this: v57, a2: SLODWORD(v74[0]), a3: SHIDWORD(v74[0]));`

### CSummoned::TryDoingAttackManual
  - `if ( v16 != (&loc_4F8376 + 3) && v16 != &off_4FA6A2 || HitMobInRect == 0 )`
  - `HitPoint = CMob::GetHitPoint(a1: &v131.right, a2: v117[0], a3: v117[1], a4: v117[2], a5: v117[3]);`
  - `if ( HitMobInRect != 0 || v28 != 0 )`
  - `sub_75A4D3(a1: v139, a2: i, a3: v143, a4: 0, a5: v134, a6: HitMobInRect, a7: v112, a8: 0);`
  - `if ( v143 == 0 || (Data = TSecType<long>::GetData(a1: (*(v127 + 323) + 12), a2: v47), v143 == Data) )`
  - `COutPacket::Encode1(this: &v135, a2: v83[8] & 0x7F | (v88 << 7));`

### CSummoned::TryDoingHeal
  - `COutPacket::Encode1(this: &v21, a2: (v15 << 7) | 6);`

### CSummoned::Update
  - `if ( _ZtlSecureFuse<CLadderOrRope *>(a1: v4 + 73, a2: v4[75]) != 0 )`
  - `CSummoned::SetDamaged(a1: 0, a2: 0, a3: 0, a4: v26, a5: -1, a6: v11);`
  - `if ( update_time - *(a1 + 69) > get_summoned_attack_delay(a1: *(a1 + 45)) )`
  - `CSummoned::SetMoveAction(this: a1, a2: v4[82], a3: 0);`

### CSummonedBase::LoadAttackInfo
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v90[0], a2: SBYTE4(v90[0]));`
  - `v97 = Ztl_variant_t::GetUnknown(a1: v90[0], a2: SBYTE4(v90[0]));`
  - `v60 = *IWzProperty::GetItem(pvargDest: v90[0], a2: SHIDWORD(v90[0]));`
  - `v65 = *StringPool::GetStringW(this: v64, a2: v90[0], a3: SHIDWORD(v90[0]));`

### CTownPortalPool::OnTownPortalCreated
  - `FootholdUnderneath = CWvsPhysicalSpace2D::GetFootholdClosest(this: dword_BEBFA0, x: v93[1].Mid32, y: v93[2].Lo32);`
  - `FootholdClosest = CWvsPhysicalSpace2D::GetFootholdClosest(this: dword_BEBFA0, x: v93[1].Mid32, y: v93[2].Lo32);`

### CUIAvatar::OnCreate
  - `PcCreateObject::IWzFont(sUOL: v21, pObj: this + 67, pUnkOuter: 0);`
  - `IWzCanvas::DrawTextA(this: v79, nLeft: 10, nTop: 15, sText: v83, pFont: v84, vAlpha: v85, vTabOrg: p_pvargDest);`

### CUIAvatar::Refresh
  - `result = CUIAvatar::ResetCharacter(this, a2: i, a3: *(this + 2 * *(this + 76) + i + *(this + 76) + 28), a4: v4);`

### CUIAvatarMegaphone::OnCreate
  - `CCtrlCheckBox::SetChecked(this: *(this + 38), a2: 1);`

### CUIAvatarVAC::OnChildNotify
  - `CUIAvatarVAC::SetSelection(this, a2: v5 % 3 + 3 * ScrollBarIdx);`

### CUIAvatarVAC::OnCreate
  - `PcCreateObject::IWzFont(sUOL: v44, pObj: this + 3800, pUnkOuter: 0);`

### CUIAvatarVAC::OnMouseButton
  - `else if ( a2 == 515 && CUIAvatarVAC::GetSelectedIdx(this: (this - 4), a2: a4) >= 0 )`

### CUIPetEquip::OnMouseMove
  - `v35 = *(CharacterData::GetItem(this: v33, a2: v29, a3: 1, a4: -v5) + 4);`
  - `v37 = *(CharacterData::GetItem(this: v33, a2: v31, a3: 1, a4: -100 - v6) + 4);`
  - `CUIToolTip::SetToolTip_Equip2(this: (this + 26), a2: (a2 + v11), a3: v24, a4: v25, a5: v37, a6: 1, a7: 0);`

### CUserPool::FindRemoteUserByName
  - `v3 = *(pos != 0 ? pos - 16 + 4 : 4);`

### CUserPool::OnCoupleChairRecordRemove
  - `CUser::SetCoupleChairEffect(a1: 0, a2: v6, a3: 0);`
  - `CUser::SetCoupleChairEffect(a1: 0, a2: 0, a3: 0);`

### CUserPool::OnCoupleRecordRemove
  - `CUser::SetCoupleItemEffect(a1: 0, a2: User, a3: v6[6]);`

### CUserPool::OnFriendRecordRemove
  - `CUser::SetFriendShipItemEffect(this: User, a2: 0, a3: v9, a4: v6[6]);`

### CUserPool::OnUserRemotePacket
  - `CAvatar::SetEmotion(this: (RemoteUser + 34), nEmotion: v5, tDuration: -1);`

### CVecCtrlMob::ChaseTargetImp
  - `memset(a1: this + 1252, a2: 0, a3: 0x14u);`

### CVecCtrlMob::CollisionDetectFloat
  - `CVecCtrl::SetInput(this, a2: -1, a3: 0);`
  - `CVecCtrl::SetInput(this, a2: 1, a3: 0);`

### CVecCtrlMob::CtrlUpdateActiveMove
  - `CVecCtrl::SetInput(this, a2: 0, a3: 0);`
  - `CVecCtrlMob::ChaseTargetImp(this, a2: 0, a3: 0, a4: 0);`
  - `CVecCtrl::SetInput(this, a2: *(this + 313), a3: 0);`

### CVecCtrlMob::InspectUpdateActive
  - `if ( v4 != 0 && (v4[141] != 0 || v4[144] != 0 || v4[172] != 0 || timeGetTime() != 0) )`

### CVecCtrlMob::SetActive
  - `return memset(a1: this + 1252, a2: 0, a3: 0x14u);`

### CVecCtrlMob::WorkUpdateActive
  - `if ( v5 != 0 && (v5[141] != 0 || v5[144] != 0 || v5[172] != 0 || timeGetTime() != 0) )`
  - `CVecCtrl::SetInput(this, a2: 0, a3: 0);`
  - `CVecCtrlMob::ChaseTargetImp(this, a2: 0, a3: 0, a4: 0);`

### CVecCtrlPet::CreateInstance
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 632) != 0 )`

### CVecCtrlPet::EndUpdateActive
  - `result = CMovePath::IsTimeForFlush(this: (this + 428), a2: 0);`
  - `if ( CMovePath::Flush(this: v2, a2: v6, a3: 0, a4: 0) == 0 )`

### CVecCtrlPet::SetActive
  - `return memset(a1: this + 620, a2: 0, a3: 8u);`

### CVecCtrlPet::WorkUpdateActive
  - `if ( _ZtlSecureFuse<CLadderOrRope *>(a1: v6 + 73, a2: v76) == 0`
  - `|| _ZtlSecureFuse<CLadderOrRope *>(a1: this + 73, a2: *(this + 75)) == 0 )`
  - `if ( _ZtlSecureFuse<CLadderOrRope *>(a1: v6 + 73, a2: v6[75]) != 0 )`
  - `v15 = _ZtlSecureFuse<CLadderOrRope *>(a1: this + 73, a2: *(this + 75));`
  - `if ( _ZtlSecureFuse<CLadderOrRope *>(a1: this + 73, a2: *(this + 75)) != 0 )`
  - `CVecCtrl::SetInput(this, a2: nPetX[2721], a3: 0);`
  - `if ( *(this + 68) == 0 && CVecCtrl::IsSwimming(a1: this) == 0 )`
  - `if ( *(this + 153) != 0 && timeGetTime() > *(this + 153) + 700 )`
  - `&& (timeGetTime() != 0 || timeGetTime() != 0) )`
  - `CVecCtrl::SetInput(this, a2: *(this + 155), a3: 0);`
  - `CVecCtrl::SetInput(this, a2: (nPetX[348] & 1) != 0 ? -1 : 1, a3: 0);`
  - `memset(a1: this + 620, a2: 0, a3: 8u);`

### CVecCtrlSummoned::SetActive
  - `return memset(a1: this + 616, a2: 0, a3: 0x14u);`

### CWvsPhysicalSpace2D::GetFoothold
  - `if ( dwSN != 0 && ZMap<unsigned long,ZRef<CStaticFoothold>,unsigned long>::GetAt(a1: &dwSN, a2: v4) != 0 )`

### CWvsPhysicalSpace2D::GetFootholdAbove
  - `CWvsPhysicalSpace2D::GetCrossCandidate(this: this + 17, a2: x - 1, a3: yMax, a4: x + 1, a5: y + 1, a6: v14);`

### CWvsPhysicalSpace2D::GetFootholdRange
  - `CWvsPhysicalSpace2D::GetCrossCandidate(this: this + 17, a2: x - 1, a3: y1 - 1, a4: x + 1, a5: y2 + 1, a6: v13);`

### CWvsPhysicalSpace2D::Load
  - `Crc32 = CCrc32::GetCrc32(a1: &v198, a2: 4, a3: 0, a4: 0, a5: 0);`
  - `v5 = CCrc32::GetCrc32(a1: &Crc32, a2: 4, a3: 0, a4: 0, a5: 0);`
  - `v190 = CCrc32::GetCrc32(a1: &v174, a2: 4, a3: 0, a4: 0, a5: 0);`
  - `v196 = CCrc32::GetCrc32(a1: &v203, a2: 4, a3: 0, a4: LODWORD(v135), a5: HIDWORD(v135));`
  - `v111[1] = get_int32(pvarSrc: v113, a2: SHIDWORD(v135)) != 0;`
  - `*(this + 44) = (CCrc32::GetCrc32(a1: &v203, a2: 4, a3: 0, a4: 0, a5: 0) ^ *(this + 44));`

### CWvsPhysicalSpace2D::SetFieldAttr
  - `Geometry::InclusionChecker::AddToArea(this: (*(this + 43) + 60), a2: v5[470], a3: v6);`

### MobStat::DecodeTemporary
  - `*(this + 12) = a7 + 500 * CInPacket::Decode2(this: a6);`

### UGW_ItemSlotBase::1?$ZRef
  - `result = InterlockedDecrement(lpAddend: (*(this + 1) + 4));`
  - `InterlockedIncrement(lpAddend: (*(this + 1) + 4));`
