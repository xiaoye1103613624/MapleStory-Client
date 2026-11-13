# Angel 083 核心窗口反编译语义抽取（观察层）

来源：`evidence/angel-corewindow-decompile-full.json`（296 函数，逐字 Hex-Rays 伪代码，7 个超大函数从 `/output/` 端点补全并归一化）。
本文件由 `tools/ida_angel_corewindow_facts.py` 自动生成，只做字符串抽取，不含推断。

## 1. StringPool 资源节点全集（401 个）

- `SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK` ← CCashShop, CDraggableItem, CNoticeQuestProgress, CQuest, CUIFadeYesNo, CUIItem…
- `SP_1001_SHINE` ← CUIToolTip
- `SP_100_MAPLESTORY_ERROR__LOADING_REACTOR_FAILED` ← CUIFadeYesNo
- `SP_101_MAPLESTORY_ERROR__LOADING_TAMINGMOB_FAILED` ← CUIUserInfo
- `SP_10_EQP` ← CUIQuestAlarm, CUIToolTip
- `SP_11_SETUP` ← CUIItemMaker, CUIToolTip
- `SP_1227_UI_CASHSHOPIMG_CSSTATUS_BTCHARGE` ← CCSWnd_Status
- `SP_1228_UI_CASHSHOPIMG_CSSTATUS_BTCHECK` ← CCSWnd_Status
- `SP_1229_UI_CASHSHOPIMG_CSSTATUS_BTCOUPON` ← CCSWnd_Status
- `SP_1230_UI_CASHSHOPIMG_CSSTATUS_BTEXIT` ← CCSWnd_Status
- `SP_1234_DRAGEND` ← CDraggableItem
- `SP_1235_INVITE` ← CUIFadeYesNo
- `SP_1240_WORLDMAPOPEN` ← CWorldMapDlg
- `SP_1241_WORLDMAPCLOSE` ← CWorldMapDlg
- `SP_1242_SELECTMAP` ← CWorldMapDlg
- `SP_1246_ENCHANTSUCCESS` ← CUIEnchantDlg
- `SP_1247_ENCHANTFAILURE` ← CUIEnchantDlg
- `SP_1249_SOUND_BGMUIIMG_SHOPBGM` ← CCashShop
- `SP_1253_UI_BASICIMG_ITEMNO` ← CUIItem
- `SP_1257_UI_BASICIMG_BTOK2` ← CItemSpeakerDlg, CUIMacroSys
- `SP_1258_UI_BASICIMG_BTCANCEL2` ← CItemSpeakerDlg
- `SP_1280_STRING_TOOLTIPHELPIMG_SHOP_BUTTON_CHARGE` ← CCSWnd_Status
- `SP_1281_STRING_TOOLTIPHELPIMG_SHOP_BUTTON_CHECK` ← CCSWnd_Status
- `SP_1282_STRING_TOOLTIPHELPIMG_SHOP_BUTTON_COUPON` ← CCSWnd_Status
- `SP_1284_UI_UIWINDOWIMG_FADEYESNO_BTOK` ← CUIFadeYesNo
- `SP_1285_UI_UIWINDOWIMG_FADEYESNO_BTCANCEL` ← CUIFadeYesNo
- `SP_1286_UI_UIWINDOWIMG_FADEYESNO_ICON0` ← CUIFadeYesNo
- `SP_1287_UI_UIWINDOWIMG_FADEYESNO_ICON1` ← CUIFadeYesNo
- `SP_1288_UI_UIWINDOWIMG_FADEYESNO_ICON2` ← CUIFadeYesNo
- `SP_1289_UI_UIWINDOWIMG_FADEYESNO_ICON3` ← CUIFadeYesNo
- `SP_1290_UI_UIWINDOWIMG_FADEYESNO_ICON4` ← CUIFadeYesNo
- `SP_1291_UI_UIWINDOWIMG_FADEYESNO_ICON5` ← CUIFadeYesNo
- `SP_1292_UI_UIWINDOWIMG_FADEYESNO_ICON6` ← CUIFadeYesNo
- `SP_1293_UI_UIWINDOWIMG_FADEYESNO_ICON7` ← CUIFadeYesNo
- `SP_1294_UI_UIWINDOWIMG_FADEYESNO_ICON8` ← CUIFadeYesNo
- `SP_1295_UI_UIWINDOWIMG_FADEYESNO_BACKGRND` ← CUIFadeYesNo
- `SP_1296_UI_UIWINDOWIMG_FADEYESNO_BACKGRND2` ← CUIFadeYesNo
- `SP_1298_UI_UIWINDOWIMG_FADEYESNO_BACKGRND4` ← CUIFadeYesNo
- `SP_12_BEGINNER` ← CUIMedalQuestInfo
- `SP_1301_UI_CASHSHOPIMG_BASE_BACKGRND` ← CCashShop
- `SP_1302_UI_CASHSHOPIMG_BASE_BACKGRND1` ← CCashShop
- `SP_1389_NPC` ← CQuest
- `SP_1410_CANVASFONT` ← CUIFadeYesNo, CUIItemMaker, CUIMedalQuestInfo, CUIQuestInfo, CUIQuestInfoDetail
- `SP_1411_BA` ← CUIMedalQuestInfo, CUIQuestInfo, CUIQuestInfoDetail
- `SP_1445_TITLE` ← CWorldMapDlg
- `SP_1447_DESC` ← CWorldMapDlg
- `SP_1467_DX` ← CItemEffectManager
- `SP_1468_DY` ← CItemEffectManager
- `SP_1469_DX1` ← CItemEffectManager
- `SP_1470_DY1` ← CItemEffectManager
- `SP_147_PLEASE_TRY_AGAIN` ← CUIUserInfo
- `SP_150_NOT_AVAILABLE` ← CDraggableItem
- `SP_1515_TYPE` ← CWorldMapDlg
- `SP_153_S_IS_AT_S` ← CUIUserInfoWishList
- `SP_1593_MOB` ← CQuest
- `SP_1604_NAME` ← CNoticeQuestProgress, CUIFadeYesNo, CUIQuestAlarm
- `SP_160_YOU_HAVE_NO_BUDDY_AVAILABLE_FOR_CHAT` ← CUIUserInfo
- `SP_1644_EFFECT` ← CItemEffectManager
- `SP_1662_` ← CUIMedalQuestInfo
- `SP_168_YOU_HAVE_ENTERED_AN_INVALID_CHARACTER_NAME` ← CUIUserInfo
- `SP_170_YOU_HAVE_EITHER_ENTERED_A_WRONG_NPC_NAME_OR` ← CUIQuestAlarm
- `SP_1729_STREETNAME` ← CWorldMapDlg
- `SP_1730_MAPNAME` ← CWorldMapDlg
- `SP_1735_AREA` ← CUIMedalQuestInfo, CUIQuestInfo
- `SP_1796_S__S` ← CWorldMapDlg
- `SP_1803_PATH` ← CItemEffectManager, CWorldMapDlg
- `SP_182__START` ← CUIUserInfo
- `SP_183__RESET` ← CUIUserInfo
- `SP_1847_DRAGSTART` ← CUIItem, CUIKeyConfig
- `SP_1848_UI_UIWINDOWIMG_ITEM_BTCOIN` ← CUIItem
- `SP_1849_UI_UIWINDOWIMG_ITEM_ACTIVEICON` ← CUIItem
- `SP_1851_UI_UIWINDOWIMG_ITEM_BTGATHER` ← CUIItem
- `SP_1852_UI_UIWINDOWIMG_ITEM_BTSORT` ← CUIItem
- `SP_1887_REQLEVEL` ← CUIUserInfo, CUIUserInfoDetail
- `SP_1891_UI_UIWINDOWIMG_QUEST_BACKGRND2` ← CUIQuestInfoDetail
- `SP_1896_UI_UIWINDOWIMG_QUEST_LINE` ← CUIQuestInfoDetail
- `SP_1897_UI_UIWINDOWIMG_QUEST_ICON0` ← CUIQuestInfo
- `SP_1898_UI_UIWINDOWIMG_QUEST_ICON1` ← CUIQuestInfo
- `SP_1899_UI_UIWINDOWIMG_QUEST_ICON2` ← CUIQuestInfo
- `SP_189__GIVEPRIZE` ← CUIUserInfo
- `SP_1901_UI_UIWINDOWIMG_QUEST_ICON4` ← CUIQuestInfo
- `SP_1923_UI_UIWINDOWIMG_SKILL_BTSPUP` ← CUISkill
- `SP_1924_UI_UIWINDOWIMG_SKILL_SKILL0` ← CUISkill
- `SP_1925_UI_UIWINDOWIMG_SKILL_SKILL1` ← CUISkill
- `SP_1926_UI_UIWINDOWIMG_SKILL_RECOMMEND_0` ← CUISkill
- `SP_1939_D_DD` ← CUIStat, CUIStatDetail
- `SP_1940_D__D` ← CUIStatDetail
- `SP_1941_D` ← CUIStatDetail
- `SP_1943_UI_UIWINDOWIMG_STAT_BTAPUP` ← CUIStat
- `SP_1944_UI_UIWINDOWIMG_STAT_BTAUTO` ← CUIStat
- `SP_1945_UI_UIWINDOWIMG_STAT_BTAUTO1` ← CUIStat
- `SP_1946_UI_UIWINDOWIMG_STAT_BTAUTO2` ← CUIStat
- `SP_1947_UI_UIWINDOWIMG_STAT_DISABLED_` ← CUIStat
- `SP_1948_STRING_TOOLTIPHELPIMG_GAME_BUTTON_STATAUTO` ← CUIStat
- `SP_1949_STRING_TOOLTIPHELPIMG_GAME_BUTTON_STATAUTO1` ← CUIStat
- `SP_1950_STRING_TOOLTIPHELPIMG_GAME_BUTTON_STATAUTO2` ← CUIStat
- `SP_1951_____YOUR_AP_WILL_BE_DISTRIBUTED_AS_FOLLOWS_R_N_R_N` ← CUIStat
- `SP_1952______S__D_R_N` ← CUIStat
- `SP_1953_STRING_TOOLTIPHELPIMG_GAME_UIWND_STAT` ← CUIStat
- `SP_1954_D_D` ← CUIStat
- `SP_1955_UI_UIWINDOWIMG_STAT_JOB` ← CUIStat
- `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID` ← CDraggableItem, CUIItem, CUIMedalQuestInfo, CUIQuestAlarm, CUIQuestInfo, CUIStat…
- `SP_2042_STR_` ← CUIToolTip
- `SP_2043_DEX_` ← CUIToolTip
- `SP_2044_INT_` ← CUIToolTip
- `SP_2045_LUK_` ← CUIToolTip
- `SP_2046_HP_` ← CUIToolTip
- `SP_2047_MP_` ← CUIToolTip
- `SP_2048__D` ← CUIToolTip
- `SP_2049___D` ← CUIToolTip
- `SP_2050_DESCD` ← CUIToolTip
- `SP_2051_UI_UIWINDOWIMG_USERINFO_BACKGRND` ← CUIUserInfo
- `SP_2052_UI_UIWINDOWIMG_USERINFO_BACKGRND2` ← CUIUserInfo
- `SP_2055_UI_UIWINDOWIMG_USERINFO_BACKGRND5` ← CUIUserInfo
- `SP_2057_UI_UIWINDOWIMG_USERINFO_BACKGRND7` ← CUIUserInfo
- `SP_2064_UI_UIWINDOWIMG_STAT_BTDETAIL` ← CUIStat
- `SP_2228_UI_UIWINDOWIMG_SHORTCUT_BTITEM` ← CUIShortCutMenu
- `SP_2229_UI_UIWINDOWIMG_SHORTCUT_BTEQUIP` ← CUIShortCutMenu
- `SP_2230_UI_UIWINDOWIMG_SHORTCUT_BTSTAT` ← CUIShortCutMenu
- `SP_2231_UI_UIWINDOWIMG_SHORTCUT_BTSKILL` ← CUIShortCutMenu
- `SP_2234_UI_UIWINDOWIMG_SHORTCUT_BTQUEST` ← CUIShortCutMenu
- `SP_2235_UI_UIWINDOWIMG_SHORTCUT_BTCOMM` ← CUIShortCutMenu
- `SP_2236_UI_UIWINDOWIMG_SHORTCUT_BTMOBBOOK` ← CUIShortCutMenu
- `SP_2237_UI_UIWINDOWIMG_SHORTCUT_BTMESSENGER` ← CUIShortCutMenu
- `SP_2240_STRING_MOBIMG` ← CNoticeQuestProgress, CUIQuestAlarm
- `SP_2241_STRING_SKILLIMG` ← SKILLENTRY
- `SP_2276_MAP_WORLDMAP_SIMG` ← CWorldMapDlg
- `SP_2277_MAP_MAPHELPERIMG_WORLDMAP_MAPIMAGE` ← CWorldMapDlg
- `SP_2281_UI_UIWINDOWIMG_WORLDMAP_BORDER_7` ← CWorldMapDlg
- `SP_2282_UI_UIWINDOWIMG_WORLDMAP_BORDER_6` ← CWorldMapDlg
- `SP_2283_UI_UIWINDOWIMG_WORLDMAP_BORDER_5` ← CWorldMapDlg
- `SP_2284_UI_UIWINDOWIMG_WORLDMAP_BORDER_4` ← CWorldMapDlg
- `SP_2285_UI_UIWINDOWIMG_WORLDMAP_BORDER_3` ← CWorldMapDlg
- `SP_2286_UI_UIWINDOWIMG_WORLDMAP_BORDER_2` ← CWorldMapDlg
- `SP_2287_UI_UIWINDOWIMG_WORLDMAP_BORDER_1` ← CWorldMapDlg
- `SP_2288_UI_UIWINDOWIMG_WORLDMAP_BORDER_0` ← CWorldMapDlg
- `SP_2289_UI_UIWINDOW_WORLDMAP_TITLE` ← CWorldMapDlg
- `SP_2290_BASEIMG_0` ← CWorldMapDlg
- `SP_2291_MAPLIST` ← CWorldMapDlg
- `SP_2292_MAPNO` ← CWorldMapDlg
- `SP_2293_MAPDESC` ← CWorldMapDlg
- `SP_2366_ID` ← CQuest
- `SP_236__POINTER03` ← CUIUserInfo
- `SP_2413_ICON_D` ← CUIKeyConfig
- `SP_2503_UI_UIWINDOWIMG_KEYCONFIG_NOTICE_D` ← CUIKeyConfig
- `SP_2561_UI_BASICIMG_BTHIDE` ← CUIQuestInfoDetail
- `SP_2564_UI_UIWINDOWIMG_USERINFO_REQLV` ← CUIUserInfo, CUIUserInfoDetail
- `SP_2577_UI_UIWINDOWIMG_USERINFO_MARRY_MARRIED_0` ← CUIUserInfo
- `SP_2578_UI_UIWINDOWIMG_USERINFO_MARRY_UNMARRIED_0` ← CUIUserInfo
- `SP_25_HERO` ← CUIQuestAlarm
- `SP_2660_UI_UIWINDOWIMG_SKILL_LINE` ← CUISkill
- `SP_2662_UI_UIWINDOWIMG_ITEM_TAB_ENABLED` ← CUIItem
- `SP_2663_UI_UIWINDOWIMG_ITEM_TAB_DISABLED` ← CUIItem
- `SP_2664_UI_UIWINDOWIMG_QUEST_TAB_ENABLED` ← CUIQuestInfo
- `SP_2665_UI_UIWINDOWIMG_QUEST_TAB_DISABLED` ← CUIQuestInfo
- `SP_266_YOU_CANT_DO_IT_RIGHT_NOW_R_NCATCH_YOUR_BREATH_AND_THEN_TRY_AGAIN` ← CDraggableItem
- `SP_2727_SAMPLE` ← CUIToolTip
- `SP_272_CURSING_IS_NOT_ALLOWED` ← CUIMacroSys
- `SP_2864_THE_OPTION_IS_NOT_AVAILABLE_FOR_THAT_PACKAGE` ← CCashShop
- `SP_2879_UI_CASHSHOPIMG_CSDISCOUNT` ← CUIToolTip
- `SP_2964_YOU_MAY_NOT_PURCHASE_THE_ITEM_R_NSINCE_YOU_ALREADY_HAVE_IT` ← CCashShop
- `SP_2973_QUEST` ← CQuest
- `SP_297_OTHER_REQUESTS_ARE_IN_PROCESS_R_NPLEASE_TRY_AGAIN_IN_A_LITTLE_BIT` ← CItemSpeakerDlg
- `SP_2982_NOFLIP` ← CItemEffectManager
- `SP_3199_QUEST_ACTIMG_D_D_ITEM` ← CQuest
- `SP_3212_QUEST_SAYIMG` ← CQuest
- `SP_3214_ASK` ← CQuest
- `SP_3217_ITEM` ← CQuest
- `SP_3218_QUEST_SAYIMG_D_1_LOST` ← CQuest
- `SP_3219_QUEST_SAYIMG_D_1_STOP_S` ← CQuest
- `SP_3232_QUEST_QUESTINFOIMG` ← CUIFadeYesNo, CUIMedalQuestInfo, CUIQuestInfo, CUIQuestInfoDetail
- `SP_3236_PARENT` ← CUIMedalQuestInfo, CUIQuestInfo
- `SP_3296_IT_IS_SAVED` ← CUIMacroSys, CUIMacroSysEx
- `SP_3416_SEALED_ITEMS_CANNOT_BE_R_NSOLD_TRADED_OR_DROPPED` ← CDraggableItem
- `SP_3438_TO_COMPLETE_THE_QUEST_YOULL_NEED_TO_UNSEAL_THE_QUEST_ITEM` ← CQuest
- `SP_3506_UNABLE_TO_COMPLETE_THE_QUEST_R_NDUE_TO_A_LACK_OF_PET_R_NPLEASE_TRY_AGAIN_AFTER_S` ← CQuest
- `SP_3507_UNABLE_TO_COMPLETE_THE_QUEST_R_NWITH_THE_CURRENT_PET_YOU_HAVE_SUMMONED_R_NPLEASE` ← CQuest
- `SP_3509_YOURE_LACKING_LEVEL_1_OR_2_SKILLS` ← CUISkill
- `SP_3510_YOURE_LACKING_LEVEL_1_SKILLS` ← CUISkill
- `SP_3511_YOU_ARE_LACKING_IN_EITHER_1ST_2ND_OR_3RD_LEVEL_SKILLS` ← CUISkill
- `SP_3550_GUILD_QUEST` ← CTemporaryStatView
- `SP_3588_FOLLOW` ← CItemEffectManager
- `SP_3620_QUEST_HELPER_D_5` ← CUIQuestAlarm
- `SP_3621_UI_BASICIMG_BTCLOSE2` ← CUIQuestAlarm
- `SP_3746_HIRED_MERCHANTS_SHOP_IS_ONLY_AVAILABLE_FOR__R_NCHARACTERS_THAT_ARE_OVER_LEVEL_15` ← CCashShop
- `SP_3749_THIS_ITEM_CAN_ONLY_BE_USED_15_DAYS__R_NAFTER_THE_ACCOUNTS_REGISTRATION` ← CCashShop
- `SP_3818_YOU_CAN_RIDE_THE_TAMED_CASH_MONSTER_WHEN_IT_IS_EQUIPPED_WITH_A_CASH_SADDLE` ← CDraggableItem
- `SP_3820_LEFT` ← CItemEffectManager
- `SP_3821_RIGHT` ← CItemEffectManager
- `SP_3822_TOP` ← CItemEffectManager
- `SP_3823_BOTTOM` ← CItemEffectManager
- `SP_386_PLEASE_SELECT_THE_PET_YOU_WOULD_LIKE_TO_PUT_THE_PET_EQUIPMENT_ON` ← CDraggableItem
- `SP_3873_QUICK_DELIVEY_PACKAGE` ← CUIFadeYesNo
- `SP_3874_ARRIVED` ← CUIFadeYesNo
- `SP_3875_PACKAGE_R_NRECEIVED` ← CUIFadeYesNo
- `SP_3876_THE_QUICK_DELIVERY_HAS_ARRIVED` ← CUIFadeYesNo
- `SP_3877__S_` ← CUIFadeYesNo
- `SP_387_THE_SCROLL_LIGHTS_UP_AND_THEN_ITS_MYSTERIOUS_POWER_HAS_BEEN_TRANSFERRED_TO_THE_I` ← CUIEnchantDlg
- `SP_388_THE_SCROLL_LIGHTS_UP_BUT_THE_ITEM_WINDS_UP_AS_IF_NOTHING_HAPPENED` ← CUIEnchantDlg
- `SP_389_THE_ITEM_IS_DESTROYED_DUE_TO_THE_OVERWHELMING_POWER_OF_THE_SCROLL` ← CUIEnchantDlg
- `SP_3912_UI_BASICIMG_ICON_DELIVERY` ← CUIFadeYesNo
- `SP_3965_THE_ITEM_UPGRADE_FAILED_BUT_SINCE_THE_WHITE_SCROLL_WAS_USED_THE_NUMBER_OF_ITEM_U` ← CUIEnchantDlg
- `SP_3966__THE_ITEM_WAS_SUCCESSFULLY_UPGRADED_AND_THE_WHITE_SCROLL_WAS_USED_IN_THE_PROCESS` ← CUIEnchantDlg
- `SP_4038_SPOT` ← CWorldMapDlg
- `SP_40_ARCH_MAGEICELIGHTNING` ← CCashShop
- `SP_4110_PARTY_MEMBER__S` ← CUIToolTip
- `SP_4154_UI_UIWINDOWIMG_SKILLMACRO_BACKGRND` ← CUIMacroSys
- `SP_4161_UI_UIWINDOWIMG_SKILLMACRO_MACROICON_D_ICON` ← CUIKeyConfig, CUIMacroSysEx, CUIToolTip
- `SP_4163_UI_UIWINDOWIMG_SKILLMACROEX_MACROSLOT2` ← CUIMacroSysEx
- `SP_4164_UI_UIWINDOWIMG_SKILLMACROEX_MACROICON_D_ICON` ← CUIMacroSys, CUIMacroSysEx
- `SP_4200_YOU_ARE_MARRIED_WITH__S` ← CUIToolTip
- `SP_4259_DROPPING_THIS_ITEM_WILL_BREAK_YOUR_ENGAGEMENT_R_NDO_YOU_STILL_WISH_TO_DROP_THIS_` ← CDraggableItem
- `SP_4260_YOU_WILL_NOT_BE_ABLE_TO_RECEIVE_THE_WEDDING_RING_R_NIS_THAT_OKAY` ← CDraggableItem
- `SP_4266_YOU_MAY_ONLY_EQUIP_ONE_WEDDING_RING` ← CDraggableItem
- `SP_4267__SIGNIFICANT_OTHER_` ← CWorldMapDlg
- `SP_4336_REGISTER_THE_FREQUENTLY_USED_SKILLS_FOR_A_QUICK_ACCESS_WITH_A_SINGLE_BUTTON` ← CUIToolTip
- `SP_4341_YOU_MAY_OPEN_CLOSE_YOUR_OWN_SKILL_WINDOW` ← CUISkill, CUISkillEx
- `SP_4342_DRAG_THE_SKILL_ICON_FROM_THE_SKILL_INVENTORY_R_NON_THE_LEFT_INTO_THE_SLOTS` ← CUIMacroSys, CUIMacroSysEx
- `SP_4343_SAVING_THE_SKILL_NAME` ← CUIMacroSys
- `SP_4344_UI_BASICIMG_BTMACRO` ← CUISkill
- `SP_4345_UI_UIWINDOWIMG_SKILLMACROEX_BTMACROIN` ← CUISkillEx
- `SP_4346_UI_UIWINDOWIMG_SKILLMACROEX_BTMACROOUT` ← CUISkillEx
- `SP_4378_TO_HATCH_PIGMY_EGG_YOU_NEED_TO_R_NPURCHASE_THE_INCUBATOR_FROM_THE_CASH_SHOP` ← CDraggableItem
- `SP_4434_YOU_HAVE_NOT_SELECTED_AN_ITEM_R_NARE_YOU_SURE_YOU_STILL_WANT_TO_USE_THE_ITEM` ← CItemSpeakerDlg
- `SP_4435_EMISSION` ← CItemEffectManager
- `SP_4436_THETA` ← CItemEffectManager
- `SP_4437_GENONMOVE` ← CItemEffectManager
- `SP_4438_GENPOINT` ← CItemEffectManager
- `SP_4496_PARTY_QUEST` ← CUIFadeYesNo
- `SP_4498_UI_UIWINDOWIMG_ITEM_FULLBACKGRND` ← CUIItem
- `SP_4499_UI_UIWINDOWIMG_ITEM_BACKGRND` ← CUIItem
- `SP_4500_UI_UIWINDOWIMG_ITEM_BTCASHSHOP` ← CUIItem
- `SP_4501_EXPAND_INVENTORY` ← CUIItem
- `SP_4502_COLLAPSE_INVENTORY` ← CUIItem
- `SP_4504_YOU_CAN_EXPAND_THE_INVENTORY_SLOT_WITH_CASH_ITEMS` ← CUIItem
- `SP_4529_UI_UIWINDOWIMG_ITEM_DISABLED` ← CUIItem
- `SP_4538_PLEASE_CONFIRM_WHETHER_R_NTHE_CHARACTERS_NAME_IS_CORRECT` ← CCashShop
- `SP_4613_S_WOULD_LIKE_TO_R_NBE_YOUR_SENIOR_WOULD_YOU_LIKE_TO_BE_R_NSS_JUNIOR` ← CUIFadeYesNo
- `SP_4658_ONCE_DROPPED_THE_ITEM_CANNOT_BE_TRADED_R_NWOULD_YOU_STILL_LIKE_TO_DROP_IT` ← CDraggableItem
- `SP_4665_CYOU_CAN_ENABLE_1_TIME_OF_TRADING_WHEN_YOU_USE_THE_SCISSORS_OF_KARMA` ← CUIToolTip
- `SP_4666_YOU_CURRENTLY_DO_NOT_HAVE_ANY_ITEMS_THAT_R_NCAN_USE_THE_SCISSORS_OF_KARMA_R_NWOU` ← CCashShop
- `SP_4667__R_N_CITEMS_THAT_CAN_BE_USED_` ← CUIToolTip
- `SP_4682_UI_UIWINDOWIMG_USERINFO_BOSSPETCROWN` ← CUIUserInfo
- `SP_4875_WOULD_YOU_LIKE_TO_THROW_R_NAWAY_THIS_CARD` ← CDraggableItem
- `SP_494_YOUR_JOB_LIMIT_DOES_NOT_ALLOW_R_NYOU_TO_USE_THIS_ITEM__R_NARE_YOU_SURE_YOU_WANT_` ← CCashShop
- `SP_495_YOUR_LEVEL_LIMIT_DOES_NOT_ALLOW_R_NYOU_TO_USE_THIS_ITEM_R_NARE_YOU_SURE_YOU_WANT` ← CCashShop
- `SP_496_YOUR_FAME_LIMIT_DOES_NOT_ALLOW_R_NYOU_TO_USE_THIS_ITEM_R_NARE_YOU_SURE_YOU_WANT_` ← CCashShop
- `SP_498_YOU_ALREADY_HAVE_THE_ITEMS_SELECTED_ARE_R_NYOU_SURE_YOU_WANT_TO_BUY_THE_ITEMS_AG` ← CCashShop
- `SP_499_YOU_MAY_NOT_USE_THIS_ITEM_WITH_R_NTHE_WEAPON_YOU_ARE_EQUIPPED_WITH_R_NWILL_YOU_C` ← CCashShop
- `SP_501__R_NTHE_CURRENT_PET_YOU_HAVE_R_NCANNOT_WEAR_THIS_ITEM_R_NARE_YOU_SURE_YOU_WANT_T` ← CCashShop
- `SP_502_ONLY_PETS_ARE_ALLOWED_TO_WEAR_THIS_ITEM_R_NARE_YOU_SURE_YOU_WANT_TO_PURCHASE_THI` ← CCashShop
- `SP_503_THIS_ITEM_CANNOT_BE_USED_WITHOUT_A_PET_AND_R_NTHIS_ITEM_CANNOT_BE_USED_ON_A_DECE` ← CCashShop
- `SP_504__R_NTHIS_ITEM_CANNOT_BE_USED_BY_THE_R_NPET_YOURE_CURRENTLY_RAISING_R_NARE_YOU_SU` ← CCashShop
- `SP_505_THIS_IS_AN_ITEM_THAT_CANNOT_BE_USED_R_NIF_YOU_DO_NOT_HAVE_THE_MONSTER_R_NRIDING_` ← CCashShop
- `SP_5066_UI_UIWINDOWIMG_CASHGACHAPON_BTOPEN` ← CUICashItemGachapon
- `SP_5067_UI_UIWINDOWIMG_CASHGACHAPON_BTCLOSE` ← CUICashItemGachapon
- `SP_506_ARE_YOU_SURE_YOU_WANT_TO_BUY_THIS_ITEM` ← CCashShop
- `SP_507_TOTAL_D_NX` ← CCashShop
- `SP_508_MAKE_PURCHASE` ← CCashShop
- `SP_509_THE_PURCHASE_HAS_FAILED` ← CCashShop
- `SP_5105_VIEWMEDALITEM` ← CUIMedalQuestInfo
- `SP_5106_MEDALCATEGORY` ← CUIMedalQuestInfo
- `SP_510_YOU_HAVE_PURCHASED_THE_FOLLOWING_ITEMS_R_NSOME_ITEMS_WERENT_PURCHASED` ← CCashShop
- `SP_5153_UI_UIWINDOWIMG_STAT_BASICSTAT` ← CUIStat
- `SP_5160_UI_UIWINDOWIMG_FADEYESNO_BACKGRND6` ← CUIFadeYesNo
- `SP_5161_UI_UIWINDOWIMG_FADEYESNO_ICON9` ← CUIFadeYesNo
- `SP_5162_UI_UIWINDOWIMG_TITLE_BACKGRND2` ← CUIMedalQuestInfo
- `SP_5164_UI_UIWINDOWIMG_TITLE_BACKGRND4` ← CUIMedalQuestInfo
- `SP_5168_UI_UIWINDOWIMG_TITLE_ICON1` ← CUIMedalQuestInfo
- `SP_5169_UI_UIWINDOWIMG_TITLE_ICON2` ← CUIMedalQuestInfo
- `SP_5170_UI_UIWINDOWIMG_TITLE_TAB_DISABLED` ← CUIMedalQuestInfo
- `SP_5171_UI_UIWINDOWIMG_TITLE_TAB_ENABLED` ← CUIMedalQuestInfo
- `SP_5197_MOVEMENT_IS_POSSIBLE_ONLY_WITHIN_THE_ACCOUNT` ← CUIToolTip
- `SP_5207_SOME_ITEMS_CANNOT_BE_TRADED_WITH_OTHER_USERS_AND_R_N` ← CCashShop
- `SP_5211_IF_YOU_PURCHASE_THIS_ITEM_YOU_WILL_BE_ABLE_TO_EQUIP_R_NADDITIONAL_EQUIPMENT_ITEM` ← CUIToolTip
- `SP_5212_AVAILABLE_UNTIL_DMONTH_DDATE_04DYEAR_AT_02DHOUR02DMINUTE` ← CUIToolTip
- `SP_5213_IF_YOU_USE_THIS_ITEM_YOU_WILL_NOT_BE_ABLE_R_NTO_TRADE_THE_ITEM_WITH_OTHER_USERS_` ← CDraggableItem
- `SP_5223_YOU_CAN_ONLY_BE_EQUIPPED_WITH_1_UNIQUE_EQUIPPED_ITEM` ← CDraggableItem
- `SP_5225_THIS_IS_A_CASH_TRADE_INVITE` ← CUIFadeYesNo
- `SP_5258_THIS_ITEM_CANNOT_BE_RECOVERED_ONCE_DROPPED_HOWEVER_IT_IS_POSSIBLE_TO_TRANSFER_IT` ← CDraggableItem
- `SP_5274_TIME_LIMIT_EXTENSION_IS_NOT_POSSIBLE` ← CUIToolTip
- `SP_5275_UNIQUE_EQUIPPED_ITEM` ← CUIToolTip
- `SP_5280_HOW_MANY_WILL_YOU_DROP_A_PORTION_OF_THE_AMOUNT_MAY_BE_LOST_IF_PICKED_UP_BY_ANOTH` ← CUIItem
- `SP_529_YOU_HAVE_SUCCESSFULLY_MADE_THE_PURCHASE` ← CCashShop
- `SP_52_ASSASSIN` ← CUIUserInfo
- `SP_5300_NUMBER_OF_MEDALS_OBTAINED_D` ← CUIMedalQuestInfo
- `SP_5301_OBTAINABLE_TITLE` ← CUIMedalQuestInfo
- `SP_5302_TITLE_BEING_CHALLENGED` ← CUIMedalQuestInfo
- `SP_5321_YOU_GOT_A_TITLE` ← CUIFadeYesNo
- `SP_5329_UI_UIWINDOWIMG_ITEM_NEW_TAB1` ← CUIItem
- `SP_5330_UI_UIWINDOWIMG_ITEM_NEW_TAB0` ← CUIItem
- `SP_5331_UI_UIWINDOWIMG_ITEM_NEW_INVENTORY` ← CUIItem
- `SP_535_YOU_DONT_HAVE_ENOUGH_CASH` ← CCashShop
- `SP_5361_UI_CASHSHOPIMG_BASE_BACKGRND2` ← CCashShop
- `SP_5439_WITH_THE_WEAPON_THAT_S_CURRENTLY_HAS_EQUIPPED_R_NTHIS_ITEM_CANT_BE_USED_R_NDO_YO` ← CCashShop
- `SP_5454_THIS_ITEM_CAN_ONLY_BE_EQUIPPED_ON_EVAN` ← CDraggableItem
- `SP_5471_THIS_CONTAINS_AN_ITEM_THAT_WILL_DISAPPEAR_ONCE__R_NTHE_EXPIRATION_DATE_HAS_PASSE` ← CCashShop
- `SP_5472_S_DOES_NOT_HAVE_AN_ITEM_THAT__R_NTHE_SCISSORS_OF_KARMA_CAN_BE_USED_ON_R_NDO_YOU_` ← CCashShop
- `SP_5514_04D` ← CUIFadeYesNo
- `SP_5515_D` ← CUIStat, CUIUserInfo, CUIUserInfoDetail
- `SP_5516_D___D` ← CUIStat
- `SP_5527_ARIAL` ← CUIFadeYesNo, CUIItemMaker, CUIMacroSys, CUIMedalQuestInfo, CUIQuestInfo, CUIQuestInfoDetail…
- `SP_5530_CATEGORY_` ← CUIToolTip
- `SP_5533_CATEGORY` ← CUIItemMaker
- `SP_5536_DUE_TO_JOB_LIMITATIONS_S_CANT_R_NUSE_THIS_ITEM_R_NDO_YOU_STILL_WISH_TO_GIVE_IT_A` ← CCashShop
- `SP_5538_ETC` ← CUIItemMaker, CUIToolTip
- `SP_5550_MESO` ← CUIQuestAlarm
- `SP_5571_THIS_ITEM_CANT_BE_TAKEN_BACK_ONCE__R_NTHROWN_AWAY_WILL_YOU_STILL_DROP_IT` ← CDraggableItem
- `SP_5576_TITLE` ← CUIMedalQuestInfo
- `SP_5577_UI_BASICIMG_BTCLOSE` ← CWorldMapDlg
- `SP_557_DUE_TO_AN_UNKNOWN_ERROR_R_NTHE_REQUEST_FOR_CASH_SHOP_HAS_FAILED` ← CCashShop
- `SP_5580_UI_UIWINDOWIMG_ITEM_BTFULL` ← CUIItem
- `SP_5581_UI_UIWINDOWIMG_ITEM_BTSMALL` ← CUIItem
- `SP_5596_USE` ← CUIItemMaker, CUIToolTip
- `SP_5599_YOU_DO_NOT_HAVE_ENOUGH_MESOS` ← CUIItemMaker
- `SP_5621_COUNT` ← CQuest
- `SP_5624_DELAY` ← CItemEffectManager
- `SP_5634_INTERVAL` ← CItemEffectManager
- `SP_5641_NONE` ← CUIToolTip
- `SP_572_D` ← CUIUserInfoWishList
- `SP_577_ADD_D_SLOTS_FOR_THE_STORAGE_CURRENT_D_R_NMAXIMUM_OF_48_SLOTS_AVAILABLE` ← CUIToolTip
- `SP_580_ADD_D_SLOTS_FOR_THE_S_INVENTORY_CURRENT_D_SLOTS_R_NMAXIMUM_OF_D_SLOTS_AVAILABLE` ← CUIToolTip
- `SP_581_ADD_A_CHARACTER_SLOT_CURRENT__D_SLOTS_R_NYOU_MAY_HAVE_UP_TO_D_SLOTS` ← CUIToolTip
- `SP_58_BRAWLER` ← CUIUserInfo
- `SP_592_ARE_YOU_SURE_YOU_WANT_TO_LEAVE_THE_CASH_SHOP` ← CCSWnd_Status
- `SP_598_STRENGTHENING_GEM` ← CUIItemMaker
- `SP_599_MONSTER_CRYSTAL` ← CUIItemMaker
- `SP_600_DISASSEMBLE_EQUIPMENT` ← CUIItemMaker
- `SP_61_GUNSLINGER` ← CUIStat
- `SP_651_ITS_A_GIFT_FROM_S` ← CUIToolTip
- `SP_656_ATTACK_SPEED_` ← CUIToolTip
- `SP_657_WEAPON_ATTACK_` ← CUIToolTip
- `SP_658_MAGIC_ATTACK_` ← CUIToolTip
- `SP_659_WEAPON_DEF_` ← CUIToolTip
- `SP_660_MAGIC_DEF_` ← CUIToolTip
- `SP_661_ACCURACY_` ← CUIToolTip
- `SP_662_AVOIDABILITY_` ← CUIToolTip
- `SP_663_HANDS_` ← CUIToolTip
- `SP_664_SPEED_` ← CUIToolTip
- `SP_665_JUMP_` ← CUIToolTip
- `SP_667_THE_RATE_OF_KNOCK_BACK_` ← CUIToolTip
- `SP_668_NUMBER_OF_UPGRADES_AVAILABLE_` ← CUIToolTip
- `SP_669_ADD_PREVENT_SLIPPING` ← CUIToolTip
- `SP_66_YOU_CANT_DO_IT_ONCE_YOURE_DEAD` ← CDraggableItem, CUIItem
- `SP_670_ADD_PREVENT_COLDNESS` ← CUIToolTip
- `SP_671_A_GIFT_FROM_S` ← CUIToolTip
- `SP_672_A_COUPLE_RING_WITH__S` ← CUIToolTip
- `SP_673_THIS_IS_YOUR_FRIENDSHIP_RING_WITH_S` ← CUIToolTip
- `SP_692_ONE_OF_A_KIND_ITEM` ← CUIToolTip
- `SP_693_QUEST_ITEM` ← CUIToolTip
- `SP_694_PARTY_QUEST_ITEM` ← CUIToolTip
- `SP_695_UNTRADEABLE` ← CUIToolTip
- `SP_696_CAN_TRADE_1_TIME_TRADE_DISABLED_AFTER_TRANSACTION` ← CUIToolTip
- `SP_697_SLOT_MERGER` ← CUIItem
- `SP_698_ITEM_SORT` ← CUIItem
- `SP_699_TRADE_DISABLED_WHEN_EQUIPPED` ← CUIToolTip
- `SP_714_S_IS` ← CUIFadeYesNo
- `SP_715_LOGGED_IN` ← CUIFadeYesNo
- `SP_716_YOURE_ON_THE_SAME_CH` ← CUIFadeYesNo
- `SP_717_WITH_S` ← CUIFadeYesNo
- `SP_753_PRODUCTION_FAILED` ← CUIItemMaker
- `SP_756__WOULD_YOU_LIKE_TO_MAKE_THIS` ← CUIItemMaker
- `SP_757_WOULD_YOU_LIKE_TO_MAKE_A_MONSTER_CRYSTAL` ← CUIItemMaker
- `SP_758_WOULD_YOU_LIKE_TO_DISASSEMBLE` ← CUIItemMaker
- `SP_759_WOULD_YOU_LIKE_TO_MAKE_` ← CUIItemMaker
- `SP_760_APPLY_THE_DROP_ITEMS_HERE_R_N_TO_MAKE_THE_MONSTER_CRYSTAL` ← CUIItemMaker
- `SP_761_APPLY_THE_EQUIPMENT_TO_BE_DISASSEMBLED_HERE_R_N_TO_ACQUIRE_THE_MONSTER_CRYSTAL` ← CUIItemMaker
- `SP_763_FROM_S` ← CUIFadeYesNo
- `SP_764_AN_INVITATION` ← CUIFadeYesNo
- `SP_765_BUDDY_REQUEST` ← CUIFadeYesNo
- `SP_766_TRADE_REQUEST` ← CUIFadeYesNo
- `SP_767_PARTY_INVITE` ← CUIFadeYesNo
- `SP_768_GUILD_INVITE` ← CUIFadeYesNo
- `SP_769_ITS_A_FAMILY_INVITATION` ← CUIFadeYesNo
- `SP_770_NEW_QUEST` ← CUIFadeYesNo
- `SP_771_QUEST_COMPLETE` ← CUIFadeYesNo
- `SP_775_THE_ADMIN_ACCOUNT_CANT_THROW_AWAY_THE_MONEY` ← CUIItem
- `SP_80_WE_ARE_UNABLE_TO_CONNECT_TO_THE_LOGIN_SERVER_DUE_TO_A_SERVER_CHECK_A_FIREWALL_BL` ← CUIFadeYesNo
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
- `SP_956_CANVAS` ← CItemSpeakerDlg, CUIUserInfo, CWorldMapDlg
- `SP_959_ORIGIN` ← CWorldMapDlg
- `SP_960_DEFAULT` ← CQuest
- `SP_961_INFO` ← CItemEffectManager, CQuest
- `SP_979_Z` ← CItemEffectManager

## 2. 核心窗口类逐函数关键行


### CUISkill（5 函数）


#### CUISkill::CloseSkillGuide  (295 B)

  - `int __thiscall CUISkill::CloseSkillGuide(CUISkill *this)`
  - `v2 = *(this + 411);`
  - `if ( v2 != 0 )`
  - `CWnd::Destroy(a1: v2);`
  - `return sub_8ADF92(a1: this + 1640);`

#### CUISkill::GetSkillRootVisible  (594 B)

  - `char *__thiscall CUISkill::GetSkillRootVisible(CUISkill *this, int a2)`
  - `if ( a2 != 0 )`
  - `v3 = *(*(this + 364) + 4 * *(*(this + 366) + 60));`
  - `v4 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v6) + 4);`
  - `if ( v6[1] != 0 )`
  - `sub_428C44(this: v6, a2: 0);`
  - `CSkillInfo::GetSkillRootVisible(this: dword_BE78DC, a2: v3, a3: v4, a4: (this + 1524));`

#### CUISkill::OnCreate  (15602 B)

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
  - `LOBYTE(v84) = 2;`
  - `v7 = *CWnd::GetLayer(a1: &v83) == 0;`
  - …（截断 16 行）

#### CUISkill::OnSkillLevelUpButton  (7860 B)

- 分支 case：`0`, `1`, `2`
- SP_ 节点：`SP_3509_YOURE_LACKING_LEVEL_1_OR_2_SKILLS`, `SP_3510_YOURE_LACKING_LEVEL_1_SKILLS`, `SP_3511_YOU_ARE_LACKING_IN_EITHER_1ST_2ND_OR_3RD_LEVEL_SKILLS`
  - `void __thiscall CUISkill::OnSkillLevelUpButton(CUISkill *this, int *a2)`
  - `if ( CWvsContext::CanSendExclRequest(a1: 500, a2: 0) != 0 )`
  - `v4 = *(CWvsContext::GetCharacterData(this: v2, a2: &v35) + 4);`
  - `if ( v36 != 0 )`
  - `sub_428C44(this: &v35, a2: 0);`
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
  - `switch ( v9 % 10 )`
  - …（截断 16 行）

#### CUISkill::OpenCurSkillGuide  (673 B)

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

### CUISkillEx（3 函数）


#### CUISkillEx::OnButtonClicked  (827 B)

  - `void __thiscall CUISkillEx::OnButtonClicked(CUISkillEx *this, unsigned int a2)`
  - `if ( a2 >= 0x7DA && a2 < 0x7DE )`
  - `CUISkillEx::OnSkillLevelUpButton(this, a2: a2 - 2010);`
  - `if ( a2 == 2023 )`
  - `CUISkillEx::ShiftMacroUIState(this, a2: 0);`
  - `CUIWnd::OnButtonClicked(a1: a2);`
  - `if ( a2 != 2024 )`
  - `COutPacket::COutPacket(a1: 165);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: v2);`
  - `ZArray<unsigned char>::RemoveAll(a1: v3);`

#### CUISkillEx::OnSkillLevelUpButton  (1382 B)

  - `void __thiscall CUISkillEx::OnSkillLevelUpButton(CUISkillEx *this, int a2)`
  - `if ( CWvsContext::CanSendExclRequest(a1: 500, a2: 0) != 0 )`
  - `v4 = *(CWvsContext::GetCharacterData(this: v2, a2: v9) + 4);`
  - `if ( v9[1] != 0 )`
  - `sub_428C44(this: v9, a2: 0);`
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

#### CUISkillEx::ShiftMacroUIState  (9527 B)

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
  - `*(this + 401) = 0;`
  - `LOBYTE(v45) = 2;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `LOBYTE(v45) = 0;`
  - `if ( v43 != 0 )`
  - `v8 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - …（截断 16 行）

### CUISkillChangeConfirm（1 函数）


#### CUISkillChangeConfirm::GetResult  (252 B)

  - `int __thiscall CUISkillChangeConfirm::GetResult(CUISkillChangeConfirm *this, int *a2, int *a3)`
  - `*a2 = *(this + 39);`
  - `result = *(this + 38);`

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

### CWndSkillGuide（1 函数）


#### CWndSkillGuide::OnKey  (280 B)

  - `int __thiscall CWndSkillGuide::OnKey(CWndSkillGuide *this, unsigned int a2, signed int a3)`
  - `if ( a3 >= 0 && a2 == 27 && dword_BF1080 != 0 )`
  - `return CUISkill::CloseSkillGuide(this: dword_BF1080);`

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

### CUIMacroSys（8 函数）


#### CUIMacroSys::GetIndexByPos  (983 B)

  - `int __thiscall CUIMacroSys::GetIndexByPos(CUIMacroSys *this, int a2, int a3, int a4, int *a5, int *a6)`
  - `while ( v7 - 32 > a3 || a3 > v7 )`
  - `if ( v7 >= 260 )`
  - `if ( v6 != 5 )`
  - `while ( v9 - 32 > a2 || a2 > v9 )`
  - `if ( v9 >= 155 )`
  - `if ( v8 != 3 )`
  - `if ( a4 != 0 && a2 >= 145 && a2 <= 177 )`

#### CUIMacroSys::OnButtonClicked  (2028 B)

- SP_ 节点：`SP_272_CURSING_IS_NOT_ALLOWED`, `SP_3296_IT_IS_SAVED`
  - `void __thiscall CUIMacroSys::OnButtonClicked(_DWORD *this, int a2)`
  - `if ( a2 == 3000 && *(this + 361) != -1 )`
  - `CCtrlEdit::GetText(this: *(this + 34), &a2);`
  - `Buffer = ZXString<char>::GetBuffer(a1: 1024, a2: 1);`
  - `v14 = CCurseProcess::ProcessString(a1: Buffer, a2: 0, a3: 1);`
  - `ZXString<char>::ReleaseBuffer(a1: -1);`
  - `if ( v14 != 0 )`
  - `v13 = *(*(this + 32) + 72);`
  - `sub_428211(a1: &a2);`
  - `v11 = *(this + 361);`
  - `LOBYTE(v15) = 0;`
  - `v5 = sub_8ADF76();`
  - `CMacroSysMan::SetMacroInfo(this: v5, nMacroIdx: v11, sName: v12, bShout: v13);`
  - `CUtilDlg::Notice(sMsg: v9, sSndName: v10, ppDialog: v11, bAutoSeparated: v12, bIsTightLine: v13);`

#### CUIMacroSys::OnCreate  (7403 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1257_UI_BASICIMG_BTOK2`, `SP_4154_UI_UIWINDOWIMG_SKILLMACRO_BACKGRND`, `SP_4343_SAVING_THE_SKILL_NAME`, `SP_5527_ARIAL`
  - `void __thiscall CUIMacroSys::OnCreate(CUIMacroSys *this, void *a2)`
  - `CWnd::SetBackgrnd(this, a2: v29, a3: v30, a4: v31);`
  - `*(this + 361) = -1;`
  - `v4 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 116);`
  - `if ( v4 != 0 )`
  - `v5 = CCtrlScrollBar::CCtrlScrollBar(this: v4);`
  - `sub_428597(a1: v5);`
  - `(*(**(this + 30) + 32))(a1: *(this + 30), a2: this, a3: 3010, a4: 1, a5: 3, a6: 185, a7: 54, a8: 125, a9: 0);`
  - `*(*(this + 30) + 52) = 100;`
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 30), a2: 3);`
  - `memset(v39, 0, sizeof(v39));`
  - `LOBYTE(v44) = 2;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `LOBYTE(v44) = 1;`
  - `if ( v42 != 0 )`
  - `v8 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - …（截断 16 行）

#### CUIMacroSys::OnKey  (230 B)

  - `int __stdcall CUIMacroSys::OnKey(unsigned int a1, signed int a2)`
  - `if ( a2 >= 0 && a1 == 27 )`
  - `return CWvsContext::ProcessBasicUIKey(a1: 27, a2);`

#### CUIMacroSys::OnMouseButton  (13390 B)

- SP_ 节点：`SP_4164_UI_UIWINDOWIMG_SKILLMACROEX_MACROICON_D_ICON`
  - `void __fastcall CUIMacroSys::OnMouseButton(CUIMacroSys *this, int a2, CUIMacroSys *a3, unsigned int a4, int a5, int a1)`
  - `if ( a3 == 513 )`
  - `v6 = (this - 4);`
  - `v62 = (this - 4);`
  - `CUIMacroSys::GetIndexByPos(this: (this - 4), a2: a5, a3: a1, a4: 1, a5: v67, a6: &v63);`
  - `if ( *v67 != -1 )`
  - `CUIMacroSys::OnSelected(this: v6, a2: *v67);`
  - `if ( v63 != -1 )`
  - `if ( v63 == 3 )`
  - `LOBYTE(v69) = 1;`
  - `LOBYTE(v69) = 2;`
  - `ZXString<char>::Format(a1: &v65, a2: v50.pcVal, a3: v67[0]);`
  - `if ( Unknown != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v69) = 3;`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - …（截断 16 行）

#### CUIMacroSys::OnMouseMove  (4165 B)

- SP_ 节点：`SP_4342_DRAG_THE_SKILL_ICON_FROM_THE_SKILL_INVENTORY_R_NON_THE_LEFT_INTO_THE_SLOTS`
  - `int __thiscall CUIMacroSys::OnMouseMove(CUIMacroSys *this, int a2, int a3)`
  - `v31 = (this - 4);`
  - `v4 = *CWnd::GetLayer(a1: &v29) == 0;`
  - `if ( v4 )`
  - `Alpha = IWzGr2DLayer::GetAlpha(a1: &v30);`
  - `LOBYTE(v34) = 1;`
  - `v6 = IWzShape2D::Getx(a1: *Alpha);`
  - `LOBYTE(v34) = 0;`
  - `if ( v30 != 0 )`
  - `(*(*v30 + 8))(a1: v30);`
  - `if ( v29 != 0 )`
  - `(*(*v29 + 8))(a1: v29);`
  - `if ( v6 != 0 && CUIMacroSys::GetIndexByPos(this: v31, a2, a3, a4: 1, a5: &v33, a6: &v32) != 0 )`
  - `if ( v32 == 3 )`
  - `Skill = (*(*this + 48))(a1: this) + a3 + 20;`
  - `v7 = (*(*this + 44))(a1: this);`
  - …（截断 16 行）

#### CUIMacroSys::OnSelected  (856 B)

  - `void __thiscall CUIMacroSys::OnSelected(CUIMacroSys *this, unsigned int a2)`
  - `*(this + 359) = a2;`
  - `MacroName = CMacroSysMan::GetMacroName(this: dword_BEC1F4, &a2, a3: v7);`
  - `CCtrlEdit::SetText(this: *(this + 32), a2: *MacroName);`
  - `v5 = *(this + 30);`
  - `IsShoutMacro = CMacroSysMan::IsShoutMacro(this: dword_BEC1F4, nMacroIdx: v2);`
  - `CCtrlCheckBox::SetChecked(this: v5, a2: IsShoutMacro);`
  - `(*(*(*(this + 28) + 4) + 28))(a1: *(this + 28) + 4, a2: *(this + 359) >= 0);`
  - `CUIMacroSys::ResetInfo(this);`

#### CUIMacroSys::ResetInfo  (273 B)

  - `void __thiscall CUIMacroSys::ResetInfo(CUIMacroSys *this)`
  - `(*(*(*(this + 30) + 4) + 36))(a1: *(this + 30) + 4, a2: *(dword_BF10A4 + 1604) == 2);`
  - `CUIToolTip::ClearToolTip(this: (this + 132));`
  - `CWnd::InvalidateRect(this, a2: 0);`

### CUIMacroSysEx（8 函数）


#### CUIMacroSysEx::Draw  (14139 B)

- SP_ 节点：`SP_4163_UI_UIWINDOWIMG_SKILLMACROEX_MACROSLOT2`, `SP_4164_UI_UIWINDOWIMG_SKILLMACROEX_MACROICON_D_ICON`
  - `const struct tagRECT *__thiscall CUIMacroSysEx::Draw(CUIMacroSysEx *this, const struct tagRECT *arg0)`
  - `CWnd::Draw(this, a2: arg0);`
  - `CWnd::GetCanvas(a1: &arg0);`
  - `v3 = *(this + 359);`
  - `if ( v3 <= 4 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v71) = 1;`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v71) = 2;`
  - `v6 = sub_406472();`
  - `v7 = sub_402680(a1: v6);`
  - `IWzResMan::GetObjectA(this: v7, pvargDest: &v56, sUOL: v52, vParam: *&v53.vt, vAux: v53.decVal.Hi32);`
  - `LOBYTE(v71) = 4;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v53.cVal, a2: SBYTE4(v53.decVal.Lo64));`
  - `if ( Unknown != 0 )`
  - `v8 = sub_41E527(a1: &Unknown);`
  - …（截断 16 行）

#### CUIMacroSysEx::GetIndexByPos  (1063 B)

  - `signed int __thiscall CUIMacroSysEx::GetIndexByPos(CUIMacroSysEx *this, int a2, int a3, int a4, int *a5, int *a6)`
  - `v7 = *(*(this + 30) + 56);`
  - `while ( v8 - 32 > a3 || a3 > v8 )`
  - `if ( v8 >= 223 )`
  - `if ( v6 != 3 )`
  - `while ( v10 - 32 > a2 || a2 > v10 )`
  - `if ( v10 >= 163 )`
  - `if ( v9 != 3 )`
  - `if ( a4 != 0 && a2 >= 146 && a2 <= 178 )`

#### CUIMacroSysEx::OnButtonClicked  (1400 B)

- SP_ 节点：`SP_3296_IT_IS_SAVED`
  - `void __thiscall CUIMacroSysEx::OnButtonClicked(_DWORD *this, int a2)`
  - `if ( a2 == 3000 && *(this + 359) != -1 )`
  - `CCtrlEdit::GetText(this: *(this + 32), &a2);`
  - `v11 = *(*(this + 30) + 72);`
  - `sub_428211(a1: &a2);`
  - `v9 = *(this + 359);`
  - `LOBYTE(v13) = 0;`
  - `v4 = sub_8ADF76();`
  - `CMacroSysMan::SetMacroInfo(this: v4, nMacroIdx: v9, sName: v10, bShout: v11);`
  - `CUtilDlg::Notice(sMsg: v7, sSndName: v8, ppDialog: v9, bAutoSeparated: v10, bIsTightLine: v11);`

#### CUIMacroSysEx::OnKey  (258 B)

  - `int __thiscall CUIMacroSysEx::OnKey(CUIMacroSysEx *this, unsigned int a2, signed int a3)`
  - `if ( a3 >= 0 && a2 == 27 )`
  - `return CWvsContext::ProcessBasicUIKey(a1: 27, a2: a3);`

#### CUIMacroSysEx::OnMouseButton  (13465 B)

- SP_ 节点：`SP_4161_UI_UIWINDOWIMG_SKILLMACRO_MACROICON_D_ICON`
  - `void __fastcall CUIMacroSysEx::OnMouseButton(`
  - `if ( a3 == 513 )`
  - `v6 = (this - 4);`
  - `v62 = (this - 4);`
  - `CUIMacroSysEx::GetIndexByPos(this: (this - 4), a2: a5, a3: a1, a4: 1, a5: v67, a6: &v63);`
  - `if ( *v67 != -1 )`
  - `CUIMacroSysEx::OnSelected(this: v6, a2: *v67);`
  - `if ( v63 != -1 )`
  - `if ( v63 == 3 )`
  - `LOBYTE(v69) = 1;`
  - `LOBYTE(v69) = 2;`
  - `ZXString<char>::Format(a1: &v65, a2: v50.pcVal, a3: v67[0]);`
  - `if ( Unknown != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v69) = 3;`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - …（截断 16 行）

#### CUIMacroSysEx::OnMouseMove  (4178 B)

- SP_ 节点：`SP_4342_DRAG_THE_SKILL_ICON_FROM_THE_SKILL_INVENTORY_R_NON_THE_LEFT_INTO_THE_SLOTS`
  - `signed int __thiscall CUIMacroSysEx::OnMouseMove(CUIMacroSysEx *this, int a2, int a3)`
  - `v31 = (this - 4);`
  - `v4 = *CWnd::GetLayer(a1: &v29) == 0;`
  - `if ( v4 )`
  - `Alpha = IWzGr2DLayer::GetAlpha(a1: &v30);`
  - `LOBYTE(v34) = 1;`
  - `v6 = IWzShape2D::Getx(a1: *Alpha);`
  - `LOBYTE(v34) = 0;`
  - `if ( v30 != 0 )`
  - `(*(*v30 + 8))(a1: v30);`
  - `if ( v29 != 0 )`
  - `(*(*v29 + 8))(a1: v29);`
  - `if ( v6 != 0 && CUIMacroSysEx::GetIndexByPos(this: v31, a2, a3, a4: 1, a5: &v33, a6: &v32) != 0 )`
  - `if ( v32 == 3 )`
  - `Skill = (*(*this + 48))(a1: this) + a3 + 20;`
  - `v7 = (*(*this + 44))(a1: this);`
  - …（截断 16 行）

#### CUIMacroSysEx::OnSelected  (862 B)

  - `void __thiscall CUIMacroSysEx::OnSelected(CUIMacroSysEx *this, unsigned int a2)`
  - `*(this + 361) = a2;`
  - `MacroName = CMacroSysMan::GetMacroName(this: dword_BEC1F4, &a2, a3: v7);`
  - `CCtrlEdit::SetText(this: *(this + 34), a2: *MacroName);`
  - `v5 = *(this + 32);`
  - `IsShoutMacro = CMacroSysMan::IsShoutMacro(this: dword_BEC1F4, nMacroIdx: v2);`
  - `CCtrlCheckBox::SetChecked(this: v5, a2: IsShoutMacro);`
  - `(*(*(*(this + 28) + 4) + 28))(a1: *(this + 28) + 4, a2: *(this + 361) >= 0);`
  - `CUIMacroSysEx::ResetInfo(this);`

#### CUIMacroSysEx::ResetInfo  (277 B)

  - `void __thiscall CUIMacroSysEx::ResetInfo(CUIMacroSysEx *this)`
  - `(*(*(*(this + 32) + 4) + 36))(a1: *(this + 32) + 4, a2: *(dword_BF1080 + 1584) == 2);`
  - `CUIToolTip::ClearToolTip(this: (this + 140));`
  - `CWnd::InvalidateRect(this, a2: 0);`

### CMacroSysMan（8 函数）


#### CMacroSysMan::FlushToSvr  (418 B)

  - `int CMacroSysMan::FlushToSvr()`
  - `COutPacket::COutPacket(a1: 110);`
  - `sub_4E776B(a1: v1);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: v1);`
  - `return ZArray<unsigned char>::RemoveAll(a1: v2);`

#### CMacroSysMan::GetMacro  (285 B)

  - `int __thiscall CMacroSysMan::GetMacro(CMacroSysMan *this, int a2, unsigned int a3)`
  - `if ( a2 >= 0 && (v3 = *(this + 11)) != 0 && a2 < *(v3 - 4) && a3 <= 2 )`
  - `return *(29 * a2 + 4 * a3 + *(this + 11));`

#### CMacroSysMan::GetMacroName  (783 B)

  - `_DWORD *__thiscall CMacroSysMan::GetMacroName(_DWORD *this, _DWORD *a2, int a3)`
  - `if ( a3 >= 0 && (v4 = *(this + 11)) != 0 && a3 < *(v4 - 4) )`
  - `v6 = (v4 + 29 * a3 + 12);`
  - `ZXString<char>::GetBuffer(a1: v6, a2: 0xFFFFFFFF);`
  - `v7 = *(this + 11);`
  - `ZXString<char>::GetBuffer(a1: (v7 + v5 + 12), a2: 0xFFFFFFFF);`
  - `ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF);`

#### CMacroSysMan::IsShoutMacro  (226 B)

  - `BOOL __thiscall CMacroSysMan::IsShoutMacro(CMacroSysMan *this, int nMacroIdx)`
  - `return nMacroIdx < 0 || (v2 = *(this + 11)) == 0 || nMacroIdx >= *(v2 - 4) || *(29 * nMacroIdx + v2 + 25) == 0;`

#### CMacroSysMan::Map  (217 B)

  - `int __thiscall CMacroSysMan::Map(const void **this, int a2, int a3, int a4)`
  - `CMacroSysMan::Realloc(this, a2: a2 + 1);`
  - `*(*(this + 11) + 29 * a2 + 4 * a3) = a4;`

#### CMacroSysMan::Realloc  (862 B)

  - `_DWORD *__thiscall CMacroSysMan::Realloc(const void **this, signed int a2)`
  - `result = *(this + 11);`
  - `if ( result != 0 )`
  - `result = *(result - 1);`
  - `if ( a2 > v7 )`
  - `sub_4E9391(this: this + 11, a2, a3: 0, a4: &a2 + 3);`
  - `result = memset(a1: *v3 + v5, a2: 0, a3: 0x1Du);`
  - `while ( v6 != 0 );`

#### CMacroSysMan::SetMacroInfo  (836 B)

  - `void __thiscall CMacroSysMan::SetMacroInfo(const void **this, signed int nMacroIdx, int sName, int bShout)`
  - `if ( nMacroIdx <= 5 )`
  - `if ( sName != 0 )`
  - `v5 = *(sName - 4);`
  - `if ( v5 <= 12 )`
  - `CMacroSysMan::Realloc(this, a2: nMacroIdx + 1);`
  - `if ( lstrcmp(a1: *(this + 11) + 29 * nMacroIdx + 12, a2: sName) != 0 || *(*(this + 11) + v6 + 25) != (bShout == 0) )`
  - `lstrcpy(a1: *(this + 11) + v6 + 12, a2: sName);`
  - `*(*(this + 11) + v6 + 25) = bShout == 0;`
  - `CMacroSysMan::FlushToSvr();`

#### CMacroSysMan::Update  (1840 B)

  - `char *__thiscall CMacroSysMan::Update(CMacroSysMan *this)`
  - `if ( *(this + 3) != 0 )`
  - `v3 = *(this + 4);`
  - `v8 = *(v3 + 4);`
  - `result = CUser::IsDarkSight(this: TSingleton<CUserLocal>::ms_pInstance);`
  - `if ( (result == 0 || v6 == &loc_406849 || (result = sub_96004B(a1: byte_D72625)) > 0)`
  - `&& ((result = CUserLocal::DoActiveSkill(this: v4, nSkillID: v6, nScanCode: 0, pnConsumeCheck: 0)) != 0`
  - `|| *(v4 + 2775) != 0) )`
  - `if ( v8 != 0 && *(29 * v7 + *(this + 11) + 25) == 0 && get_field() != 0 && *(*dword_BE7918 + 8292) == 0 )`
  - `v5 = (29 * v7 + *(this + 11) + 12);`
  - `ZXString<char>::GetBuffer(a1: v5, a2: 0xFFFFFFFF);`
  - `get_field();`
  - `CField::SendChatMsg(a1: &v9, a2: 1);`
  - `*(this + 6) = 0;`
  - `sub_631F62(this: this + 1, a2: *(this + 4));`
  - `return sub_41CE16(a1: this + 28);`
  - …（截断 16 行）

### CUIEnchantDlg（2 函数）


#### CUIEnchantDlg::MakeClone  (5084 B)

  - `int __stdcall CUIEnchantDlg::MakeClone(int a1, char a2, int a3)`
  - `if ( a3 != 0 )`
  - `sub_4E34E1(a1: v31, a2: 1);`
  - `LOBYTE(v34) = 2;`
  - `Data = TSecType<long>::GetData(a1: (v3 + 12), a2: v32 + 12);`
  - `TSecType<long>::SetData(a1: Data);`
  - `v7 = _ZtlSecureFuse<unsigned char>(a1: v3 + 40, a2: *(v3 + 42));`
  - `*(v5 + 42) = sub_4E807A(a1: v7, a2: (v5 + 40));`
  - `v8 = _ZtlSecureFuse<unsigned char>(a1: v3 + 46, a2: *(v3 + 48));`
  - `*(v5 + 48) = sub_4E807A(a1: v8, a2: (v5 + 46));`
  - `v9 = _ZtlSecureFuse<short>(a1: v3 + 52, a2: *(v3 + 56));`
  - `*(v5 + 56) = sub_4E80EB(a1: v9, a2: v5 + 52);`
  - `v10 = _ZtlSecureFuse<short>(a1: v3 + 60, a2: *(v3 + 64));`
  - `*(v5 + 64) = sub_4E80EB(a1: v10, a2: v5 + 60);`
  - `v11 = _ZtlSecureFuse<short>(a1: v3 + 68, a2: *(v3 + 72));`
  - `*(v5 + 72) = sub_4E80EB(a1: v11, a2: v5 + 68);`
  - …（截断 16 行）

#### CUIEnchantDlg::RefreshToolTip  (8172 B)

- SP_ 节点：`SP_1246_ENCHANTSUCCESS`, `SP_1247_ENCHANTFAILURE`, `SP_387_THE_SCROLL_LIGHTS_UP_AND_THEN_ITS_MYSTERIOUS_POWER_HAS_BEEN_TRANSFERRED_TO_THE_I`, `SP_388_THE_SCROLL_LIGHTS_UP_BUT_THE_ITEM_WINDS_UP_AS_IF_NOTHING_HAPPENED`, `SP_389_THE_ITEM_IS_DESTROYED_DUE_TO_THE_OVERWHELMING_POWER_OF_THE_SCROLL`, `SP_3965_THE_ITEM_UPGRADE_FAILED_BUT_SINCE_THE_WHITE_SCROLL_WAS_USED_THE_NUMBER_OF_ITEM_U`, `SP_3966__THE_ITEM_WAS_SUCCESSFULLY_UPGRADED_AND_THE_WHITE_SCROLL_WAS_USED_IN_THE_PROCESS`
  - `void __thiscall CUIEnchantDlg::RefreshToolTip(CUIEnchantDlg *this)`
  - `v2 = *(this + 365) == 0;`
  - `if ( v2 )`
  - `if ( *(this + 366) != 0 )`
  - `LOBYTE(v37) = 3;`
  - `ZXString<char>::operator=(a1: String);`
  - `LOBYTE(v37) = 0;`
  - `if ( sub_7F63E8(a1: this) == 2 )`
  - `LOBYTE(v37) = 4;`
  - `ZXString<char>::operator=(a1: v10);`
  - `if ( (v36 & 8) != 0 )`
  - `if ( v33 != 0 )`
  - `if ( (v36 & 4) == 0 )`
  - `if ( v6 != 0 )`
  - `LOBYTE(v37) = 1;`
  - `ZXString<char>::operator=(a1: v4);`
  - …（截断 16 行）

### CUIQuestInfo（15 函数）


#### CUIQuestInfo::GetQuestInfo  (404 B)

  - `int __thiscall CUIQuestInfo::GetQuestInfo(int *this, int a2)`
  - `v2 = *(this + 368);`
  - `if ( v2 < 0 || (v3 = *(this + 369)) < 0 )`
  - `*(a2 + 4) = 0;`
  - `*(a2 + 4) = *(*(this + v2 + 385) + 8 * v3 + 4);`
  - `sub_888D34(a1: a2);`

#### CUIQuestInfo::LoadData  (30409 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1735_AREA`, `SP_3236_PARENT`
  - `int __thiscall CUIQuestInfo::LoadData(CUIQuestInfo *this)`
  - `v138 = (this + 1540);`
  - `sub_88900C(a1: v138);`
  - `for ( i = 0; ; ++i )`
  - `v3 = *(dword_BED614 + 128);`
  - `v4 = v3 != 0 ? *(v3 - 4) : 0;`
  - `if ( i >= v4 )`
  - `sub_884901(a1: m, a2: i);`
  - `while ( m < 3 );`
  - `sub_4747C2(a1: this + 1584);`
  - `v119 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v120) + 4);`
  - `sub_4289B7(this: &v120);`
  - `CurFieldID = CWvsContext::GetCurFieldID(a1: *dword_BE7918);`
  - `sub_6718F1(a1: &v115);`
  - ``eh vector constructor iterator'(a1: v109, a2: 4u, a3: 3, a4: ZArray<int>::ZArray<int>, a5: ZArray<int>::~ZArray<int>);`
  - `LOBYTE(v144) = 1;`
  - …（截断 16 行）

#### CUIQuestInfo::MarkQuestAsRead  (328 B)

  - `void __thiscall CUIQuestInfo::MarkQuestAsRead(CUIQuestInfo *this, unsigned __int16 a2)`
  - `if ( *(this + 368) == 0 && ZList<unsigned short>::Find(a1: &a2, a2: 0) == 0 )`
  - `v2 = sub_7264B5(a1: &dword_BF0FD8);`
  - `sub_A1F8E3(a1: 0);`

#### CUIQuestInfo::OnChildNotify  (501 B)

  - `void __thiscall CUIQuestInfo::OnChildNotify(CUIQuestInfo *this, unsigned int a2, unsigned int a3, unsigned int a4)`
  - `if ( a2 == 2000 )`
  - `if ( a3 == 500 )`
  - `CUIQuestInfo::OnTabChanged(this, a2: a4);`
  - `else if ( a2 == 2001 && a3 >= 0x12C && a3 <= 0x130 )`
  - `CWnd::InvalidateRect(this, a2: 0);`
  - `if ( a3 == 100 )`
  - `(*(*this + 32))(a1: this, a2);`

#### CUIQuestInfo::OnCreate  (31685 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1410_CANVASFONT`, `SP_1411_BA`, `SP_1897_UI_UIWINDOWIMG_QUEST_ICON0`, `SP_1898_UI_UIWINDOWIMG_QUEST_ICON1`, `SP_1899_UI_UIWINDOWIMG_QUEST_ICON2`, `SP_1901_UI_UIWINDOWIMG_QUEST_ICON4`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_2664_UI_UIWINDOWIMG_QUEST_TAB_ENABLED`, `SP_2665_UI_UIWINDOWIMG_QUEST_TAB_DISABLED`, `SP_3232_QUEST_QUESTINFOIMG`, `SP_5527_ARIAL`
  - `int __thiscall CUIQuestInfo::OnCreate(CUIQuestInfo *this, int Unknown)`
  - `sub_41C06C(a1: &word_AF0E08, a2: -1);`
  - `CUIWnd::OnCreate(a1: Unknown, a2: v161);`
  - `for ( i = 3; i != 0; --i )`
  - `v4 = *(dword_BED614 + 128);`
  - `if ( v4 != 0 )`
  - `v5 = *(v4 - 4);`
  - `sub_7576C5(a1: v5, a2: &Unknown + 3);`
  - `v6 = *(dword_BED614 + 128);`
  - `if ( v6 != 0 )`
  - `v7 = *(v6 - 4);`
  - `sub_7576C5(a1: v7, a2: &v172);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v179) = 1;`
  - `v10 = sub_406472();`
  - …（截断 16 行）

#### CUIQuestInfo::OnTabChanged  (801 B)

  - `void __thiscall CUIQuestInfo::OnTabChanged(CUIQuestInfo *this, int a2)`
  - `*(this + 369) = -1;`
  - `*(this + 368) = a2;`
  - `CUIQuestInfo::LoadData(this);`
  - `CUIQuestInfo::SetScrollBar(this);`
  - `if ( a2 != 0 )`
  - `if ( a2 == 1 && CUIQuestInfo::TryShowRecentlyUpdatedQuest(this) == 0 )`
  - `CUIQuestInfo::TryShowRecentlyViewedQuest(this);`
  - `else if ( CUIQuestInfo::TryShowSuitableLevelQuest(this, a2: 1) == 0 )`
  - `CUIQuestInfo::TryShowSuitableLevelQuest(this, a2: 0);`
  - `if ( *(this + 369) == -1 && CUIQuestInfo::TryShowTopQuest(this) == 0 )`
  - `CUIQuestInfo::ToggleDetail(this, a2: 0);`
  - `CUIQuestInfo::SyncScrollBarWithSelected(this);`

#### CUIQuestInfo::OnToggleDetail  (5810 B)

  - `void *__thiscall CUIQuestInfo::OnToggleDetail(CUIQuestInfo *this)`
  - `if ( *(this + 389) != 0 )`
  - `CWnd::GetLayer(a1: &v35);`
  - `if ( v35 == 0 )`
  - `LT = IWzGr2DLayer::GetLT(a1: v33);`
  - `LOBYTE(v36) = 1;`
  - `if ( v4 )`
  - `v5 = sub_425AEF(a1: *LT);`
  - `LOBYTE(v36) = 0;`
  - `sub_41569E(a1: v33);`
  - `v6 = IWzGr2DLayer::GetLT(a1: v33);`
  - `LOBYTE(v36) = 2;`
  - `v7 = sub_425B16(a1: *v6);`
  - `v4 = *IWzGr2DLayer::GetLT(a1: v33) == 0;`
  - `LOBYTE(v36) = 3;`
  - `IWzVector2D::GetOrigin(pvargDest: &v27);`
  - …（截断 16 行）

#### CUIQuestInfo::ResetInfo  (2360 B)

  - `void __thiscall CUIQuestInfo::ResetInfo(CUIQuestInfo *this, int a2)`
  - `v3 = *(this + 369) < 0;`
  - `if ( v3 )`
  - `QuestInfo = CUIQuestInfo::GetQuestInfo(this, a2: v7);`
  - `v12 = *(QuestInfo + 1);`
  - `sub_888D34(a1: v11);`
  - `if ( (v14 & 2) != 0 )`
  - `if ( v10 != 0 )`
  - `sub_888DE3(a1: 0);`
  - `if ( (v14 & 1) != 0 && v8 != 0 )`
  - `if ( a2 != 0 )`
  - `CUIQuestInfo::LoadData(this);`
  - `if ( v12 == 0 || ((v5 = *(this + *(this + 368) + 385)) != 0 ? (v6 = *(v5 - 4)) : (v6 = 0), *(this + 369) >= v6) )`
  - `*(this + 369) = -1;`
  - `CUIQuestInfo::ToggleDetail(this, a2: 0);`
  - `if ( *(this + 369) >= 0 && *(this + 372) != 0 )`
  - …（截断 16 行）

#### CUIQuestInfo::SetScrollBar  (279 B)

  - `void __thiscall CUIQuestInfo::SetScrollBar(CUIQuestInfo *this)`
  - `v1 = *(this + *(this + 368) + 385);`
  - `if ( v1 != 0 )`
  - `v1 = *(v1 - 4);`
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 367), a2: v1 - 13);`

#### CUIQuestInfo::SyncScrollBarWithSelected  (283 B)

  - `void __thiscall CUIQuestInfo::SyncScrollBarWithSelected(CUIQuestInfo *this)`
  - `if ( *(this + 369) <= 8 )`
  - `CCtrlScrollBar::SetCurPos(this: *(this + 367), a2: 0);`
  - `CCtrlScrollBar::SetCurPos(this: *(this + 367), a2: *(this + 369));`

#### CUIQuestInfo::ToggleDetail  (1852 B)

  - `void __thiscall CUIQuestInfo::ToggleDetail(CUIQuestInfo *this, int a2)`
  - `v3 = *(this + 368);`
  - `if ( v3 > 2`
  - `|| ((v4 = *(this + 369)) & 0x80000000) != 0`
  - `|| ((v5 = *(this + v3 + 385)) != 0 ? (v6 = *(v5 - 4)) : (v6 = 0), v4 >= v6) )`
  - `if ( a2 != 0 )`
  - `if ( *(this + 389) == 0 )`
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1556) != 0 )`
  - `v13 = (*(*(this + 1) + 48))(a1: this + 4);`
  - `v12 = (*(*(this + 1) + 44))(a1: this + 4);`
  - `sub_888A16(a1: this, a2: 1);`
  - `v8 = sub_87B155(a1: v10, a2: v11, a3: v12, a4: v13);`
  - `sub_888AA9(a1: v8);`
  - `*(this + 371) = 1;`
  - `CUIQuestInfo::OnToggleDetail(this, a2: v14);`
  - `v9 = **(CUIQuestInfo::GetQuestInfo(this, a2: v15) + 4);`
  - …（截断 16 行）

#### CUIQuestInfo::TryShowRecentlyUpdatedQuest  (471 B)

  - `int __thiscall CUIQuestInfo::TryShowRecentlyUpdatedQuest(CUIQuestInfo *this)`
  - `while ( 1 )`
  - `v1 = sub_71DF05(a1: dword_BED614);`
  - `if ( v1 == 0 )`
  - `if ( sub_882B2E(a1: 1, a2: v1) != 0 )`
  - `CQuestMan::RemoveRecentlyUpdatedQuest(a1: v2);`

#### CUIQuestInfo::TryShowRecentlyViewedQuest  (139 B)

  - `int __thiscall CUIQuestInfo::TryShowRecentlyViewedQuest(CUIQuestInfo *this)`
  - `return sub_882B2E(a1: 1, a2: *(this + 722));`

#### CUIQuestInfo::TryShowSuitableLevelQuest  (1375 B)

  - `int __thiscall CUIQuestInfo::TryShowSuitableLevelQuest(CCtrlTab **this, int a2)`
  - `if ( *(this + 368) != 0 )`
  - `CCtrlTab::SetTab(this: *(this + 365), a2: 0);`
  - `for ( i = 0; ; ++i )`
  - `v4 = *(this + 385);`
  - `if ( v4 == 0 || i >= *(v4 - 4) )`
  - `v12 = *(v4 + 8 * i + 4);`
  - `sub_888D34(a1: v11);`
  - `sub_888D34(a1: &v9);`
  - `if ( sub_882E46(a1: v9, a2: v10) == 0 && sub_882E99(a1: *v12) == 0 && (a2 == 0 || sub_882F70(a1: *v12) == 0) )`
  - `if ( v12 != 0 )`
  - `sub_888DE3(a1: 0);`
  - `v6 = sub_882B2E(a1: 0, a2: *v12);`

#### CUIQuestInfo::TryShowTopQuest  (1210 B)

  - `int __thiscall CUIQuestInfo::TryShowTopQuest(CUIQuestInfo *this)`
  - `for ( i = 0; ; ++i )`
  - `v3 = *(this + *(this + 368) + 385);`
  - `if ( v3 == 0 || i >= *(v3 - 4) )`
  - `v11 = *(v3 + 8 * i + 4);`
  - `sub_888D34(a1: v10);`
  - `sub_888D34(a1: &v8);`
  - `if ( sub_882E46(a1: v8, a2: v9) == 0 )`
  - `if ( v11 != 0 )`
  - `sub_888DE3(a1: 0);`
  - `v5 = sub_882B2E(a1: *(this + 368), a2: *v11);`

### CUIQuestInfoDetail（4 函数）


#### CUIQuestInfoDetail::Draw  (17787 B)

  - `VARIANTARG **__thiscall CUIQuestInfoDetail::Draw(_DWORD *this, VARIANTARG **a2)`
  - `CWnd::Draw(this, a2);`
  - `CWnd::GetCanvas(a1: &a2);`
  - `v3 = *(this + 30);`
  - `ZXString<char>::operator=(a1: v3 + 12);`
  - `sub_42847D(a1: this + 50);`
  - `format_string(sFormat: &v97, pFont: p_pvarg, nWidth: p_pvargDest);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v103) = 2;`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v103) = 3;`
  - `v79 = sub_4284A5(this: this + 50);`
  - `v5 = sub_428226(a1: &v97);`
  - `_bstr_t::_bstr_t(a1: v5);`
  - `v7 = sub_414576(a1: &a2);`
  - `IWzCanvas::DrawTextA(`
  - …（截断 16 行）

#### CUIQuestInfoDetail::OnButtonClicked  (1788 B)

  - `void __fastcall CUIQuestInfoDetail::OnButtonClicked(CUIQuestInfo **this, int a2, unsigned int a3)`
  - `switch ( a3 )`
  - `CUIQuestInfo::ToggleDetail(this: *(this + 28), a2: 0);`
  - `CUIQuestInfo::GetQuestInfo(this: *(this + 28), a2: v6);`
  - `if ( v7 != 0 )`
  - `CWvsContext::ResignQuest(this: *dword_BE7918, a2: *v7);`
  - `CWvsContext::TryRegisterAutoStartQuest(this: *dword_BE7918, a2: v5, a3: 1);`
  - `if ( v5 == 10394 )`
  - `sub_888DE3(a1: 0);`
  - `sub_87E769(a1: this);`
  - `if ( dword_BF0FF4 == 0 )`
  - `sub_888AF6(a1: this, a2);`
  - `v4 = *(CUIQuestInfo::GetQuestInfo(this: *(this + 28), a2: v8) + 4);`
  - `LOWORD(v4) = *v4;`
  - `sub_88599C(a1: dword_BF0FF4, a2: v4, a3: 0);`
  - `if ( v9 != 0 )`
  - …（截断 16 行）

#### CUIQuestInfoDetail::OnCreate  (10932 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1410_CANVASFONT`, `SP_1411_BA`, `SP_1891_UI_UIWINDOWIMG_QUEST_BACKGRND2`, `SP_1896_UI_UIWINDOWIMG_QUEST_LINE`, `SP_2561_UI_BASICIMG_BTHIDE`, `SP_3232_QUEST_QUESTINFOIMG`, `SP_5527_ARIAL`
  - `void __thiscall CUIQuestInfoDetail::OnCreate(CUIQuestInfoDetail *this, void *a2)`
  - `CWnd::SetBackgrnd(this, a2: p_pvargDest, a3: p_Unknown, a4: v50);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v53, pvargSrc: &pvargSrc);`
  - `LOBYTE(v60) = 1;`
  - `v6 = sub_406472();`
  - `v7 = sub_402680(a1: v6);`
  - `IWzResMan::GetObjectA(this: v7, pvargDest: &pvarg, sUOL: v46, vParam: v47, vAux: p_pvargDest);`
  - `LOBYTE(v60) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: p_Unknown, a2: v50);`
  - `v8 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v8 < 0 && v8 != -2147467262 )`
  - `if ( v9 < 0 )`
  - `LOBYTE(v60) = 0;`
  - `if ( v10 < 0 )`
  - `if ( v11 < 0 )`
  - …（截断 16 行）

#### CUIQuestInfoDetail::ResetInfo  (1258 B)

  - `void __thiscall CUIQuestInfoDetail::ResetInfo(CUIQuestInfoDetail *this)`
  - `QuestInfo = CUIQuestInfo::GetQuestInfo(this: *(this + 28), a2: v6);`
  - `sub_888A62(a1: QuestInfo);`
  - `if ( v7 != 0 )`
  - `sub_888DE3(a1: 0);`
  - `sub_87C388(this);`
  - `v3 = *(this + 36);`
  - `if ( v3 != 0 )`
  - `(*(*v3 + 12))(a1: v3);`
  - `v4 = *(this + 34);`
  - `if ( v4 != 0 )`
  - `(*(*v4 + 12))(a1: v4);`
  - `v5 = *(this + 38);`
  - `if ( v5 != 0 )`
  - `(*(*v5 + 12))(a1: v5);`
  - `sub_87C5E1(a1: this);`
  - …（截断 16 行）

### CUIQuestAlarm（6 函数）


#### CUIQuestAlarm::Create  (594 B)

  - `int __thiscall CUIQuestAlarm::Create(int *this)`
  - `CUIWnd::CreateUIWndPosSaved(`
  - `a2: *(this + 354),`
  - `v2 = sub_88811A(a1: this);`
  - `CWnd::CreateWnd(`
  - `*(this + 399) = 1;`
  - `return sub_49F589(a1: 1);`

#### CUIQuestAlarm::DeleteQuest  (438 B)

  - `void __thiscall CUIQuestAlarm::DeleteQuest(CUIQuestAlarm *this, unsigned __int16 a2)`
  - `for ( i = 0; ; ++i )`
  - `v3 = *(this + 364);`
  - `if ( v3 == 0 || i >= *(v3 - 4) )`
  - `if ( *(v3 + 4 * i) == a2 )`
  - `CUIQuestAlarm::DeleteQuestByIndex(this, a2: i);`

#### CUIQuestAlarm::DeleteQuestByIndex  (340 B)

  - `int __thiscall CUIQuestAlarm::DeleteQuestByIndex(CUIQuestAlarm *this, int a2)`
  - `CUIQuestAlarm::RemoveQuestByIndex(this, a2);`
  - `if ( dword_BF0FEC != 0 )`
  - `CUIQuestInfoDetail::ResetInfo(this: dword_BF0FEC);`
  - `CWnd::Destroy(a1: this);`
  - `return CUIQuestAlarm::Create(this);`

#### CUIQuestAlarm::Draw  (62225 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_10_EQP`, `SP_1604_NAME`, `SP_170_YOU_HAVE_EITHER_ENTERED_A_WRONG_NPC_NAME_OR`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_2240_STRING_MOBIMG`, `SP_25_HERO`, `SP_3620_QUEST_HELPER_D_5`, `SP_3621_UI_BASICIMG_BTCLOSE2`, `SP_5550_MESO`
  - `int __thiscall CUIQuestAlarm::Draw(CUIQuestAlarm *this, StringPoolStrings arg0)`
  - `void (__stdcall *v129)(VARIANTARG *); // edi`
  - `CWnd::Draw(this, a2: arg0);`
  - `CWnd::GetCanvas(a1: &v339);`
  - `v3 = *(v2 + 364);`
  - `LOBYTE(v342) = 1;`
  - `if ( v3 != 0 )`
  - `v4 = *(v3 - 4);`
  - `LOBYTE(v4) = 0;`
  - `LOBYTE(v342) = 2;`
  - `ZXString<char>::Format(a1: &v318, a2: v6, a3: v4);`
  - `if ( arg0 != SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v342) = 3;`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v342) = 4;`
  - …（截断 16 行）

#### CUIQuestAlarm::RemoveOldQuest  (641 B)

  - `int __thiscall CUIQuestAlarm::RemoveOldQuest(CUIQuestAlarm *this)`
  - `v6 = sub_888F88(a1: this + 1600);`
  - `while ( v6 != 0 )`
  - `v7 = *sub_888FA9(a1: &v6, a2: &v5);`
  - `v2 = dword_BF060C(a1: v4);`
  - `if ( (v2 - v5) > 0x927C0 && isAllConditionsSatisfied(a1: v7) == 0 )`
  - `sub_888F1A(a1: &v7);`
  - `CUIQuestAlarm::DeleteQuest(this, a2: v7);`

#### CUIQuestAlarm::RemoveQuestByIndex  (667 B)

  - `int __thiscall CUIQuestAlarm::RemoveQuestByIndex(CUIQuestAlarm *this, int a2)`
  - `result = *(this + 364);`
  - `v4 = (this + 1456);`
  - `if ( result != 0 && *(result - 4) != 0 )`
  - `sub_888F1A(a1: *v4 + 4 * a2);`
  - `sub_49ED9D(a1: a2);`
  - `result = sub_496E6B(a1: (4 * a2 + *v4));`
  - `v5 = *(this + 2 * a2 + 378);`
  - `if ( v5 != 0 )`
  - `(*(*v5 + 12))(a1: v5);`

### CUIQuestDelivery（1 函数）


#### CUIQuestDelivery::OnChildNotify  (375 B)

  - `void __thiscall CUIQuestDelivery::OnChildNotify(`
  - `if ( a2 == 2000 && a3 >= 0x12C && a3 <= 0x130 )`
  - `CWnd::InvalidateRect(this, a2: 0);`
  - `else if ( a3 == 100 )`
  - `(*(*this + 32))(a1: this, a2);`

### CUIMedalQuestInfo（10 函数）


#### CUIMedalQuestInfo::Draw  (40571 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_5300_NUMBER_OF_MEDALS_OBTAINED_D`, `SP_5301_OBTAINABLE_TITLE`, `SP_5302_TITLE_BEING_CHALLENGED`
  - `int *__thiscall CUIMedalQuestInfo::Draw(int *this, int a2)`
  - `CWnd::Draw(this, a2);`
  - `CWnd::GetCanvas(a1: &v183);`
  - `ZArray<_com_ptr_t<_com_IIID<IWzGr2DLayer,&__s_GUID const _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf>>>::RemoveAll(a1: this + 384);`
  - `v3 = *(*(this + 367) + 56);`
  - `v4 = *(this + 374) == v3;`
  - `if ( !v4 )`
  - `*(this + 374) = v3;`
  - `for ( i = 0; i < 2; ++i )`
  - `sub_8454F6(a1: *(this + 369), a2: i);`
  - `get_basic_font(a1: &v157, fType: 1);`
  - `LOBYTE(v184) = 1;`
  - `get_basic_font(a1: &v159, fType: 36);`
  - `v6 = *(this + 369);`
  - `LOBYTE(v184) = 2;`
  - `if ( v6 >= 0 )`
  - …（截断 16 行）

#### CUIMedalQuestInfo::GetSelect  (1468 B)

  - `int __thiscall CUIMedalQuestInfo::GetSelect(CUIMedalQuestInfo *this, int a2)`
  - `v3 = *(this + *(this + 369) + 390);`
  - `if ( v3 != 0 )`
  - `v4 = *(v3 - 4);`
  - `for ( i = *(*(this + 367) + 56); ; ++i )`
  - `if ( i >= v4 )`
  - `v14 = *(*(this + *(this + 369) + 390) + 8 * i + 4);`
  - `ZRef<MedalQuestInfo>::ZRef<MedalQuestInfo>(a1: v13);`
  - `ZRef<MedalQuestInfo>::ZRef<MedalQuestInfo>(a1: &v11);`
  - `v7 = CUIMedalQuestInfo::IsStateButton(a1: v11, a2: v12);`
  - `v8 = *(v14 + 56);`
  - `if ( v8 < a2 && a2 < v9 + v8 )`
  - `if ( v14 != 0 )`
  - `sub_84576E(a1: 0);`

#### CUIMedalQuestInfo::IsStateButton  (308 B)

  - `_WORD *__stdcall CUIMedalQuestInfo::IsStateButton(char a1, _WORD *a2)`
  - `if ( a2 != 0 )`
  - `sub_84576E(a1: 0);`

#### CUIMedalQuestInfo::LoadData  (38911 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_12_BEGINNER`, `SP_1735_AREA`, `SP_3236_PARENT`, `SP_5105_VIEWMEDALITEM`, `SP_5106_MEDALCATEGORY`, `SP_5576_TITLE`
  - `int __fastcall CUIMedalQuestInfo::LoadData(CUIMedalQuestInfo *this)`
  - `v170 = (this + 1560);`
  - `sub_8458AB(a1: v170);`
  - `for ( i = 0; i < 2; ++i )`
  - `sub_8454F6(a1: j, a2: i);`
  - `while ( j < 4 );`
  - `sub_4747C2(a1: v176 + 1620);`
  - `sub_8458AB(a1: v176 + 1576);`
  - `v149 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &pvargDest.lVal) + 4);`
  - `sub_4289B7(this: &pvargDest.lVal);`
  - `CurFieldID = CWvsContext::GetCurFieldID(a1: *dword_BE7918);`
  - `sub_6718F1(a1: &v150);`
  - ``eh vector constructor iterator'(a1: v135, a2: 4u, a3: 4, a4: ZArray<int>::ZArray<int>, a5: ZArray<int>::~ZArray<int>);`
  - `LOBYTE(v179) = 1;`
  - `v4 = ZArray<int>::InsertBefore(a1: -1);`
  - `while ( !v5 );`
  - …（截断 16 行）

#### CUIMedalQuestInfo::OnCreate  (35918 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1410_CANVASFONT`, `SP_1411_BA`, `SP_1662_`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_3232_QUEST_QUESTINFOIMG`, `SP_5162_UI_UIWINDOWIMG_TITLE_BACKGRND2`, `SP_5164_UI_UIWINDOWIMG_TITLE_BACKGRND4`, `SP_5168_UI_UIWINDOWIMG_TITLE_ICON1`, `SP_5169_UI_UIWINDOWIMG_TITLE_ICON2`, `SP_5170_UI_UIWINDOWIMG_TITLE_TAB_DISABLED`, `SP_5171_UI_UIWINDOWIMG_TITLE_TAB_ENABLED`, `SP_5527_ARIAL`
  - `int __thiscall CUIMedalQuestInfo::OnCreate(CUIMedalQuestInfo *this, int Unknown)`
  - `sub_41C06C(a1: &word_AF0E08, a2: -1);`
  - `CUIWnd::OnCreate(a1: Unknown, a2: v177);`
  - `for ( i = 4; i != 0; --i )`
  - `sub_7576C5(a1: 2, a2: &Unknown + 3);`
  - `sub_7576C5(a1: 2, a2: &v185);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v189, pvargSrc: &pvargSrc);`
  - `LOBYTE(v194) = 1;`
  - `v6 = sub_406472();`
  - `v7 = sub_402680(a1: v6);`
  - `IWzResMan::GetObjectA(this: v7, pvargDest: &v184, sUOL: v173, vParam: v174, vAux: p_pvargDest);`
  - `LOBYTE(v194) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v176, a2: v177);`
  - `v8 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v8 < 0 && v8 != -2147467262 )`
  - …（截断 16 行）

#### CUIMedalQuestInfo::OnMouseButton  (2894 B)

  - `void __thiscall CUIMedalQuestInfo::OnMouseButton(int *this, int msg, int a3, int a4, signed int a5)`
  - `if ( a5 >= 40 && *(this + 368) < 4 )`
  - `v6 = (this - 1);`
  - `Select = CUIMedalQuestInfo::GetSelect(a1: a5);`
  - `if ( Select == -1 )`
  - `if ( *v9 >= 0 )`
  - `CUIMedalQuestInfo::ToggleDetail(this: v6, a2: 0);`
  - `CWnd::InvalidateRect(this: v6, a2: 0);`
  - `v18 = *(*(this + *(this + 368) + 389) + 8 * Select + 4);`
  - `ZRef<MedalQuestInfo>::ZRef<MedalQuestInfo>(a1: v17);`
  - `ZRef<MedalQuestInfo>::ZRef<MedalQuestInfo>(a1: &v15);`
  - `if ( CUIMedalQuestInfo::IsStateButton(a1: v15, a2: v16) != 0 )`
  - `if ( msg == 513 )`
  - `v12 = *(this + 368);`
  - `if ( *(*(this + v12 + 396) + 8 * v11 + 4) != 0 )`
  - `(*(v13 + 32))(a1: v6, a2: v14);`
  - …（截断 16 行）

#### CUIMedalQuestInfo::ResetInfo  (3157 B)

  - `void __thiscall CUIMedalQuestInfo::ResetInfo(CUIMedalQuestInfo *this, int a2)`
  - `v3 = *(this + 370) < 0;`
  - `if ( v3 )`
  - `v4 = sub_83D9E7(a1: v9);`
  - `v14 = *(v4 + 1);`
  - `ZRef<MedalQuestInfo>::ZRef<MedalQuestInfo>(a1: v13);`
  - `if ( (v16 & 2) != 0 )`
  - `if ( v12 != 0 )`
  - `sub_84576E(a1: 0);`
  - `if ( (v16 & 1) != 0 && v10 != 0 )`
  - `if ( a2 != 0 )`
  - `CUIMedalQuestInfo::LoadData(this);`
  - `if ( v14 != 0 )`
  - `if ( *(this + 373) == 0 )`
  - `v5 = *(this + 369);`
  - `if ( v5 != 100 )`
  - …（截断 16 行）

#### CUIMedalQuestInfo::SetScrollBar  (862 B)

  - `void __thiscall CUIMedalQuestInfo::SetScrollBar(CUIMedalQuestInfo *this)`
  - `v1 = *(this + 369);`
  - `if ( v1 == 100 )`
  - `v2 = *(this + 394);`
  - `if ( v2 != 0 )`
  - `v3 = *(v2 - 4);`
  - `v4 = (this + 1468);`
  - `sub_843B0E(a1: 239, a2: 93);`
  - `sub_4D82FB(a1: 245);`
  - `v5 = *(this + v1 + 390);`
  - `if ( v5 != 0 )`
  - `v3 = *(v5 - 4);`
  - `sub_843B0E(a1: 239, a2: 48);`
  - `sub_4D82FB(a1: 290);`
  - `CCtrlScrollBar::SetScrollRange(this: *v4, a2: v3 - 4);`

#### CUIMedalQuestInfo::ToggleDetail  (1427 B)

  - `unsigned int __thiscall CUIMedalQuestInfo::ToggleDetail(CUIMedalQuestInfo *this, int a2)`
  - `result = *(this + 369);`
  - `if ( result >= 4`
  - `|| ((v4 = *(this + 370)) & 0x80000000) != 0`
  - `|| ((v5 = *(this + result + 390)) != 0 ? (result = *(v5 - 4)) : (result = 0), v4 >= result) )`
  - `if ( a2 != 0 )`
  - `if ( *(this + 396) != 0 )`
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1528) != 0 )`
  - `v11 = (*(*(this + 1) + 48))(a1: this + 4);`
  - `v10 = (*(*(this + 1) + 44))(a1: this + 4);`
  - `sub_845570(a1: this, a2: 1);`
  - `v7 = CUIMedalQuestInfoDetail::CUIMedalQuestInfoDetail(a1: v8, a2: v9, a3: v10, a4: v11);`
  - `sub_845603(a1: v7);`
  - `*(this + 372) = 1;`
  - `return sub_844F59(a1: this);`

#### CUIMedalQuestInfo::Update  (851 B)

  - `void __thiscall CUIMedalQuestInfo::Update(CWnd *this)`
  - `CWnd::Update(this);`
  - `if ( *(this + 396) == 0 )`
  - `if ( *(this + 373) == 0 )`
  - `v2 = *(this + 371) - 30 < 0;`
  - `*(this + 371) -= 30;`
  - `if ( v2 )`
  - `v3 = *(this + 396);`
  - `if ( v3 != 0 )`
  - `CWnd::Destroy(a1: v3);`
  - `sub_8456F2(a1: this + 1580);`
  - `*(this + 370) = -1;`
  - `*(this + 372) = 0;`
  - `CWnd::InvalidateRect(this, a2: 0);`
  - `if ( *(this + 370) >= 0 )`
  - `CUIMedalQuestInfo::ToggleDetail(this, a2: 1);`
  - …（截断 16 行）

### CUIMedalQuestInfoDetail（2 函数）


#### CUIMedalQuestInfoDetail::OnButtonClicked  (1266 B)

  - `void __thiscall CUIMedalQuestInfoDetail::OnButtonClicked(CUIMedalQuestInfo **this, unsigned int a2)`
  - `switch ( a2 )`
  - `CUIMedalQuestInfo::ToggleDetail(this: *(this + 28), a2: 0);`
  - `if ( dword_BED614 != 0 )`
  - `v3 = **(sub_83D9E7(a1: v5) + 4);`
  - `if ( v6 != 0 )`
  - `sub_84576E(a1: 0);`
  - `v4 = sub_83DB26(a1: v3);`
  - `CWvsContext::StartQuest(usQuestID: v3, dwNpcTemplateID: *(v4 + 24), bAutoStart: 1);`
  - `v2 = **(sub_83D9E7(a1: v7) + 4);`
  - `if ( v8 != 0 )`
  - `CWvsContext::ResignQuest(this: *dword_BE7918, a2: v2);`

#### CUIMedalQuestInfoDetail::ResetInfo  (986 B)

  - `void __thiscall CUIMedalQuestInfoDetail::ResetInfo(CWnd *this)`
  - `v2 = sub_83D9E7(a1: v5);`
  - `sub_8455BC(a1: v2);`
  - `if ( v6 != 0 )`
  - `sub_84576E(a1: 0);`
  - `sub_83E05E(a1: this);`
  - `v3 = *(this + 34);`
  - `if ( v3 != 0 )`
  - `(*(*v3 + 12))(a1: v3);`
  - `v4 = *(this + 36);`
  - `if ( v4 != 0 )`
  - `(*(*v4 + 12))(a1: v4);`
  - `sub_83E1C1(a1: this);`
  - `sub_83EA05(a1: this);`
  - `sub_83F751(a1: this);`
  - `sub_83F512(this);`
  - …（截断 16 行）

### CQuest（3 函数）


#### CQuest::OnCompleteQuestFailed  (44961 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1389_NPC`, `SP_1593_MOB`, `SP_2366_ID`, `SP_2973_QUEST`, `SP_3199_QUEST_ACTIMG_D_D_ITEM`, `SP_3217_ITEM`, `SP_3218_QUEST_SAYIMG_D_1_LOST`, `SP_3219_QUEST_SAYIMG_D_1_STOP_S`, `SP_3438_TO_COMPLETE_THE_QUEST_YOULL_NEED_TO_UNSEAL_THE_QUEST_ITEM`, `SP_3506_UNABLE_TO_COMPLETE_THE_QUEST_R_NDUE_TO_A_LACK_OF_PET_R_NPLEASE_TRY_AGAIN_AFTER_S`, `SP_3507_UNABLE_TO_COMPLETE_THE_QUEST_R_NWITH_THE_CURRENT_PET_YOU_HAVE_SUMMONED_R_NPLEASE`, `SP_5621_COUNT`, `SP_960_DEFAULT`, `SP_961_INFO`
  - `void __thiscall CQuest::OnCompleteQuestFailed(unsigned __int16 *this, VARIANTARG **p_p_i)`
  - `v219 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v183.lVal) + 4);`
  - `sub_4289B7(this: &v183.lVal);`
  - `switch ( p_p_i )`
  - `LOBYTE(v223) = 3;`
  - `sub_416821(a1: &v220, a2: v7, a3: v6);`
  - `LOBYTE(v223) = 2;`
  - `if ( v191 != 0 )`
  - `sub_402F85(pvargDest: &v173, pvargSrc: &pvargSrc);`
  - `LOBYTE(v223) = 4;`
  - `sub_402F85(pvargDest: &v175, pvargSrc: &pvargSrc);`
  - `LOBYTE(v223) = 5;`
  - `v8 = sub_41468C(a1: &v220);`
  - `_bstr_t::_bstr_t(a1: v8);`
  - `v10 = sub_406472();`
  - `v11 = sub_402680(a1: v10);`
  - …（截断 16 行）

#### CQuest::SayNormal  (4413 B)

  - `signed int __thiscall CQuest::SayNormal(char *this, _DWORD *a2)`
  - `sub_4281C2(a1: this + 24);`
  - `sub_716541(a1: v26, a2: v27);`
  - `if ( *a2 == 0 )`
  - `v4 = *(*a2 - 4);`
  - `if ( v34 >= v4 )`
  - `if ( v34 == v4 - 1 )`
  - `if ( *(this + 5) == 1 )`
  - `v14 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v14 != 0 )`
  - `v15 = CUtilDlgEx::CUtilDlgEx(this: v14);`
  - `v29 = v16 != 0 ? (v16 - 8) : 0;`
  - `sub_4289CD(a1: v28);`
  - `v17 = sub_490E87(a1: v34);`
  - `sub_428211(a1: v17);`
  - `v24 = *(this + 4);`
  - …（截断 16 行）

#### CQuest::StartQuest  (10701 B)

- SP_ 节点：`SP_3212_QUEST_SAYIMG`, `SP_3214_ASK`
  - `void __thiscall CQuest::StartQuest(unsigned __int16 *this, int a2)`
  - `v3 = (*(*(TSingleton<CUserLocal>::ms_pInstance + 4) + 16))(a1: TSingleton<CUserLocal>::ms_pInstance + 4);`
  - `v5 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v54) + 4);`
  - `sub_4289B7(this: v54);`
  - `v6 = CQuestMan::CheckCompleteDemand(`
  - `a2: *(this + 6),`
  - `i: *(this + 4),`
  - `a5: (*dword_BE7918 + 8500));`
  - `if ( v6 != 0 && (*(this + 5) == 1 || v6 == 11) )`
  - `CQuest::OnCompleteQuestFailed(this, a2: v6);`
  - `if ( *(this + 5) == 0 && CQuestMan::IsStartScriptLinkedQuest(a1: *(this + 6)) != 0 )`
  - `COutPacket::COutPacket(a1: 107);`
  - `COutPacket::Encode1(this: &v52, a2: 4);`
  - `COutPacket::Encode2(a1: *(this + 6));`
  - `COutPacket::Encode4(this: &v52, a2: *(this + 4));`
  - `COutPacket::Encode2(a1: v50);`
  - …（截断 16 行）

### CNoticeQuestProgress（5 函数）


#### CNoticeQuestProgress::NoticeProgressChange  (19574 B)

  - `int *__thiscall CNoticeQuestProgress::NoticeProgressChange(_DWORD *this)`
  - `if ( *(v2 - 1) != 0 )`
  - `sub_402F85(pvargDest: &v90, pvargSrc: &pvargSrc);`
  - `LOBYTE(v111) = 1;`
  - `sub_402F85(pvargDest: &v92, pvargSrc: &pvargSrc);`
  - `LOBYTE(v111) = 2;`
  - `if ( dword_BF14EC == 0 )`
  - `v4 = IWzGr2D::CreateLayer(`
  - `sub_428712(a1: *v4);`
  - `sub_4156C5(a1: &v96);`
  - `if ( v5 < 0 )`
  - `LOBYTE(v111) = 0;`
  - `if ( v6 < 0 )`
  - `sub_410FDF(this: &v74, a2: *(v2 - 9), a3: 1);`
  - `LOBYTE(v111) = 3;`
  - `if ( v110 == 0 )`
  - …（截断 16 行）

#### CNoticeQuestProgress::OnQuestProgressUpdated_Item  (3484 B)

  - `void __thiscall CNoticeQuestProgress::OnQuestProgressUpdated_Item(`
  - `if ( nItemID != 0 )`
  - `CompleteDemand = CQuestMan::GetCompleteDemand(a1: usQuestID);`
  - `if ( CompleteDemand != 0 )`
  - `v5 = *(CompleteDemand + 92);`
  - `if ( v5 != 0 && *(v5 - 4) != 0 )`
  - `for ( i = 0; ; ++i )`
  - `v7 = *(v4 + 92);`
  - `if ( v7 == 0 || i >= *(v7 - 4) )`
  - `if ( nItemID == *(v7 + 8 * i) )`
  - `ItemCount = CWvsContext::GetItemCount(a1: *(*(v4 + 92) + 8 * i));`
  - `v9 = *(*(v4 + 92) + v8 + 4);`
  - `ItemName = CItemInfo::GetItemName(a1: v16, a2: nItemID);`
  - `LOBYTE(v19) = 1;`
  - `ZXString<char>::operator=(a1: ItemName);`
  - `LOBYTE(v19) = 0;`
  - …（截断 16 行）

#### CNoticeQuestProgress::OnQuestProgressUpdated_Mob  (10802 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1604_NAME`, `SP_2240_STRING_MOBIMG`
  - `void __thiscall CNoticeQuestProgress::OnQuestProgressUpdated_Mob(char *this, int a2, int a3)`
  - `CompleteDemand = CQuestMan::GetCompleteDemand(a1: a2);`
  - `if ( CompleteDemand == 0 || (v65 = 0, (v7 = *(CompleteDemand + 96)) == 0) || *(v7 - 4) == 0 )`
  - `if ( !v6 )`
  - `LOBYTE(v68) = 2;`
  - `v8 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v49) + 4);`
  - `sub_4289B7(this: &v49);`
  - `if ( sub_4E901B(this: (v8 + 1535), a2: &i, a3: &v66) == 0 )`
  - `LOBYTE(v68) = 1;`
  - `if ( v66 != 0 )`
  - `LOBYTE(v68) = 0;`
  - `if ( v65 != 0 )`
  - `LOBYTE(v68) = 6;`
  - `while ( 1 )`
  - `v10 = *(v4 + 96);`
  - `if ( v10 == 0 || v57 >= *(v10 - 4) )`
  - …（截断 16 行）

#### CNoticeQuestProgress::OnQuestProgressUpdated_Script  (986 B)

  - `void __fastcall CNoticeQuestProgress::OnQuestProgressUpdated_Script(int a1, int a2, _BYTE *sMsg)`
  - `if ( sMsg != 0 && *sMsg != 0 )`
  - `v5 = (a1 + 128);`
  - `if ( *v5 + 3000 < get_update_time() )`
  - `while ( v4 < 5 );`
  - `if ( v4 < 5 )`
  - `*(v6 + 116) = 0;`
  - `*(v6 + 120) = 0;`
  - `*(v6 + 124) = 0;`
  - `sub_414663(a1: a1 + 44 * (v4 + 3));`
  - `*(v6 + 136) = 0;`
  - `*(v6 + 140) = 0;`
  - `*(v6 + 128) = get_update_time();`
  - `*(v6 + 144) = 1;`
  - `ZXString<char>::operator=(a1: &sMsg);`

#### CNoticeQuestProgress::Update  (169 B)

  - `int *__thiscall CNoticeQuestProgress::Update(_DWORD *this)`
  - `CWnd::Update(this);`
  - `return CNoticeQuestProgress::NoticeProgressChange(this);`

### CUIToolTip（26 函数）


#### CUIToolTip::0ItemToolTipParam  (469 B)

  - `CUIToolTip::ItemToolTipParam *__thiscall CUIToolTip::ItemToolTipParam::ItemToolTipParam(`
  - `*(this + 1) = 0;`
  - `*(this + 2) = 0;`
  - `*(this + 3) = 0;`
  - `*(this + 4) = 0;`
  - `*(this + 5) = 0;`
  - `*(this + 6) = dword_AF2378;`
  - `*(this + 7) = dword_AF237C;`
  - `*(this + 10) = 0;`

#### CUIToolTip::1ItemToolTipParam  (329 B)

  - `int __thiscall CUIToolTip::ItemToolTipParam::~ItemToolTipParam(CUIToolTip::ItemToolTipParam *this)`
  - `if ( *(this + 10) != 0 )`
  - `result = sub_4868C3(a1: 0);`
  - `*(v1 + 1) = 0;`

#### CUIToolTip::AddInfoEx  (3335 B)

  - `void __thiscall CUIToolTip::AddInfoEx(_DWORD *this, int a2, int *a3, char a4, char a5, int a6, int a7)`
  - `CUIToolTip::GetFontByType(this, &a2, a3: a2);`
  - `CUIToolTip::GetFontByType(this, a2: &v26, a3);`
  - `v9 = this + 8 * *(this + 7) + 8;`
  - `LOBYTE(v27) = 3;`
  - `ZXString<char>::operator=(a1: p_pvargDest);`
  - `ZXString<char>::operator=(a1: &a5);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v27) = 4;`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v27) = 5;`
  - `v10 = sub_428226(a1: &a4);`
  - `_bstr_t::_bstr_t(a1: v10);`
  - `sub_4284A8(a1: &a2);`
  - `v12 = sub_42782E(a1: v22, a2: p_pvargDest);`
  - `v13 = sub_428226(a1: &a5);`
  - …（截断 16 行）

#### CUIToolTip::ClearToolTip  (479 B)

  - `_DWORD *__thiscall CUIToolTip::ClearToolTip(CUIToolTip *this)`
  - `*(this + 1) = 0;`
  - `*(this + 2) = 0;`
  - `*(this + 3) = 0;`
  - `sub_428712(a1: 0);`
  - `*(this + 7) = 0;`
  - `result = (this + 32);`
  - `for ( i = 32; i != 0; --i )`

#### CUIToolTip::DrawItemIcon  (2502 B)

  - `int __thiscall CUIToolTip::DrawItemIcon(CUIToolTip *this, int a2, int a3, int a4, int a5, int a6)`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `v7 = *(this + 4) == 0;`
  - `if ( v7 )`
  - `Canvas = IWzGr2DLayer::GetCanvas(a1: &v27, a2: &pvarg);`
  - `LOBYTE(v28) = 1;`
  - `v11 = (*(**Canvas + 140))(a1: *Canvas, a2, a3, a4: 68, a5: 68, a6: -1593835521);`
  - `if ( v11 < 0 )`
  - `LOBYTE(v28) = 0;`
  - `if ( v27 != 0 )`
  - `(*(*v27 + 8))(a1: v27);`
  - `if ( v12 < 0 )`
  - `sub_4281FB(a1: v8);`
  - `IWzGr2DLayer::GetCanvas(a1: &v15, a2: &v25);`
  - `LOBYTE(v28) = 2;`
  - `sub_4283F8();`
  - …（截断 16 行）

#### CUIToolTip::DrawToolTip_Equip  (12009 B)

- SP_ 节点：`SP_5641_NONE`
  - `int __thiscall CUIToolTip::DrawToolTip_Equip(CUIToolTip *this, int a2, struct GW_ItemSlotEquip *a3)`
  - `Data = TSecType<long>::GetData(a1: a3 + 12, a2: this);`
  - `EquipItem = CItemInfo::GetEquipItem(this: v3, a2: Data);`
  - `v71 = TSecType<long>::GetData(a1: a3 + 12, a2: this);`
  - `v10 = (*(v9 + 4))(a1: a3);`
  - `CUIToolTip::DrawItemIcon(this, a2: 10, a3: a2 + 32, a4: v71, a5: v10, a6: 0);`
  - `v11 = *(CWvsContext::GetCharacterData(this: v7, a2: &v68) + 4);`
  - `v12 = _ZtlSecureFuse<long>(a1: v7 + 2098, a2: cyVal.Hi);`
  - `v73 = _ZtlSecureFuse<long>(a1: v8 + 24, a2: Hi32);`
  - `v13 = sub_4E758C(a1: v71);`
  - `sub_8F5310(a1: v73, a2: v72, a3: v13, a4: 0, a5: a2, a6: 0, a7: 0);`
  - `sub_4289B7(this: &v68);`
  - `v14 = _ZtlSecureFuse<long>(a1: v7 + 2104, a2: v7[2106]);`
  - `v15 = _ZtlSecureFuse<long>(a1: v8 + 6, a2: Hi32);`
  - `sub_8F5056(a1: v15, a2: v71, a3: 1, a4: a2, a5: 0, a6: 0);`
  - `v16 = _ZtlSecureFuse<long>(a1: v7 + 2107, a2: v7[2109]);`
  - …（截断 16 行）

#### CUIToolTip::GetFontByType  (2377 B)

- 分支 case：`1`, `2`, `3`, `4`, `5`, `6`, `7`, `8`, `9`, `10`, `11`, `12`, `13`, `14`, `15`, `16`, `17`, `18`
  - `_DWORD *__thiscall CUIToolTip::GetFontByType(_DWORD *this, _DWORD *a2, int a3)`
  - `switch ( a3 )`
  - `v3 = *(this + 265);`
  - `v3 = *(this + 266);`
  - `v3 = *(this + 267);`
  - `v3 = *(this + 268);`
  - `v3 = *(this + 270);`
  - `v3 = *(this + 271);`
  - `v3 = *(this + 272);`
  - `v3 = *(this + 273);`
  - `v3 = *(this + 274);`
  - `if ( v3 != 0 )`
  - `(*(*v3 + 4))(a1: v3);`
  - `v5 = *(this + 275);`
  - `v5 = *(this + 276);`
  - `v5 = *(this + 278);`
  - …（截断 16 行）

#### CUIToolTip::MakeLayer  (9791 B)

  - `int __thiscall CUIToolTip::MakeLayer(int *this, int a2, int *a3, int *a4, int a5, _DWORD *a6, int *a7, int a8)`
  - `*(this + 5) = a3;`
  - `*(this + 6) = v9;`
  - `if ( v10 )`
  - `if ( v8 < 0 )`
  - `if ( v9 < 0 )`
  - `v20 = *(this + 3);`
  - `if ( v8 + v20 > 799 )`
  - `a3 = (799 - v20);`
  - `v21 = *(this + 2);`
  - `if ( v9 + v21 > 599 )`
  - `a4 = (599 - v21);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v59) = 5;`
  - `if ( dword_BF14EC == 0 )`
  - …（截断 16 行）

#### CUIToolTip::PrintValue  (2599 B)

- SP_ 节点：`SP_2048__D`, `SP_2049___D`
  - `void __thiscall CUIToolTip::PrintValue(_DWORD *this, int a2, int a3, char a4, int a5)`
  - `if ( a5 != 0 || a3 > 0 )`
  - `LOBYTE(v18) = 1;`
  - `if ( a2 != 0 )`
  - `if ( a2 == 1 )`
  - `if ( a2 != 2 )`
  - `LOBYTE(v18) = 0;`
  - `ZXString<char>::Format(a1: &a5, a2: v14, a3: v15);`
  - `if ( a3 <= 0 )`
  - `LOBYTE(v18) = 2;`
  - `ZXString<char>::Format(a1: &a5, a2: *String, a3: v6);`
  - `if ( (v17 & 2) != 0 )`
  - `if ( (v17 & 1) != 0 )`
  - `sub_428211(a1: &a5);`
  - `LOBYTE(v18) = 4;`
  - `sub_428211(a1: &a4);`
  - …（截断 16 行）

#### CUIToolTip::SetBasicInfo  (300 B)

  - `int __thiscall CUIToolTip::SetBasicInfo(CUIToolTip *this, int a2, int a3, int a4, int a5)`
  - `CUIToolTip::ClearToolTip(this);`
  - `*(this + 1) = a2;`
  - `*(this + 3) = a3;`
  - `*(this + 2) = a4;`
  - `*(this + 264) = a5;`

#### CUIToolTip::SetToolTip_Bundle  (40117 B)

- SP_ 节点：`SP_4665_CYOU_CAN_ENABLE_1_TIME_OF_TRADING_WHEN_YOU_USE_THE_SCISSORS_OF_KARMA`, `SP_4667__R_N_CITEMS_THAT_CAN_BE_USED_`, `SP_671_A_GIFT_FROM_S`, `SP_692_ONE_OF_A_KIND_ITEM`, `SP_693_QUEST_ITEM`, `SP_694_PARTY_QUEST_ITEM`, `SP_695_UNTRADEABLE`, `SP_696_CAN_TRADE_1_TIME_TRADE_DISABLED_AFTER_TRANSACTION`
  - `void __thiscall CUIToolTip::SetToolTip_Bundle(`
  - `if ( sub_444F15(a1: 0) == 0 && *(this + 5) == a2 && *(this + 6) == a3 )`
  - `sub_5A31B2(a1: &a20);`
  - `LOBYTE(v202) = 2;`
  - `if ( CItemInfo::IsOnlyItem(a1: a4) != 0 )`
  - `LOBYTE(v202) = 3;`
  - `ZXString<char>::operator=(a1: String);`
  - `if ( result != 0 )`
  - `if ( CItemInfo::IsQuestItem(this: dword_BE78D8, a2: a4) != 0 )`
  - `LOBYTE(v202) = 4;`
  - `v27 = *ZXString<char>::operator+(a1: &result, a2: v26);`
  - `LOBYTE(v202) = 7;`
  - `if ( v27 != 0 )`
  - `v28 = *(v27 - 1);`
  - `ZXString<char>::_Cat(a1: v27, a2: v28);`
  - `LOBYTE(v202) = 6;`
  - …（截断 16 行）

#### CUIToolTip::SetToolTip_Equip  (57033 B)

- SP_ 节点：`SP_4200_YOU_ARE_MARRIED_WITH__S`, `SP_5197_MOVEMENT_IS_POSSIBLE_ONLY_WITHIN_THE_ACCOUNT`, `SP_5274_TIME_LIMIT_EXTENSION_IS_NOT_POSSIBLE`, `SP_5275_UNIQUE_EQUIPPED_ITEM`, `SP_651_ITS_A_GIFT_FROM_S`, `SP_672_A_COUPLE_RING_WITH__S`, `SP_673_THIS_IS_YOUR_FRIENDSHIP_RING_WITH_S`, `SP_692_ONE_OF_A_KIND_ITEM`, `SP_693_QUEST_ITEM`, `SP_694_PARTY_QUEST_ITEM`, `SP_695_UNTRADEABLE`, `SP_696_CAN_TRADE_1_TIME_TRADE_DISABLED_AFTER_TRANSACTION`, `SP_699_TRADE_DISABLED_WHEN_EQUIPPED`
  - `void __userpurge CUIToolTip::SetToolTip_Equip(`
  - `if ( sub_444F15(a1: 0) == 0 && *(ecx0 + 5) == nTop && *(ecx0 + 6) == p_Mid32 )`
  - `sub_5A31B2(a1: &a18);`
  - `LOBYTE(v270) = 2;`
  - `Data = TSecType<long>::GetData(a1: p_Hi32 + 12, a2: ebx0);`
  - `if ( CItemInfo::IsOnlyItem(a1: Data) != 0 )`
  - `LOBYTE(v270) = 3;`
  - `ZXString<char>::operator=(a1: String);`
  - `if ( result != 0 )`
  - `v25 = TSecType<long>::GetData(a1: p_Hi32 + 12, a2: ebx0);`
  - `if ( CItemInfo::IsQuestItem(this: v24, a2: v25) != 0 )`
  - `LOBYTE(v270) = 4;`
  - `v28 = *ZXString<char>::operator+(a1: &v262, a2: v27);`
  - `LOBYTE(v270) = 7;`
  - `if ( v28 != 0 )`
  - `v29 = *(v28 - 1);`
  - …（截断 16 行）

#### CUIToolTip::SetToolTip_Equip2  (7802 B)

  - `void __thiscall CUIToolTip::SetToolTip_Equip2(`
  - `v8 = (this + 16);`
  - `v36 = (this + 16);`
  - `if ( sub_444F15(a1: 0) != 0 || *(this + 5) != a2 || *(this + 6) != a3 )`
  - `sub_7B49F6(a1: 0);`
  - `CUIToolTip::SetToolTip_Equip(`
  - `a20: HIDWORD(int64));`
  - `sub_4156C5(a1: &v37);`
  - `if ( a6 != 0 )`
  - `if ( *v8 == 0 )`
  - `Width = IWzGr2DLayer::GetWidth(a1: *v8);`
  - `Height = IWzGr2DLayer::GetHeight(a1: *v8);`
  - `if ( v37 == 0 )`
  - `v40 = IWzGr2DLayer::GetWidth(a1: v37);`
  - `v13 = IWzGr2DLayer::GetWidth(a1: v12);`
  - `if ( v40 < v13 )`
  - …（截断 16 行）

#### CUIToolTip::SetToolTip_EquipExt  (7075 B)

- SP_ 节点：`SP_5211_IF_YOU_PURCHASE_THIS_ITEM_YOU_WILL_BE_ABLE_TO_EQUIP_R_NADDITIONAL_EQUIPMENT_ITEM`, `SP_5212_AVAILABLE_UNTIL_DMONTH_DDATE_04DYEAR_AT_02DHOUR02DMINUTE`
  - `void __thiscall CUIToolTip::SetToolTip_EquipExt(CUIToolTip *this, int *a2, signed int *a3, char a4, int a5, int a6)`
  - `v6 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &FileTime2) + 4);`
  - `if ( FileTime2.dwHighDateTime != 0 )`
  - `sub_428C44(this: &FileTime2, a2: 0);`
  - `LOBYTE(v45) = 2;`
  - `Data = TSecType<long>::GetData(a1: (a5 + 16), a2: 0);`
  - `ItemName = CItemInfo::GetItemName(a1: &v40, a2: Data);`
  - `LOBYTE(v45) = 3;`
  - `ZXString<char>::operator=(a1: ItemName);`
  - `dword_BF03B8(a1: v6 + 8 * a6 + 1119, a2: v37);`
  - `FileTime2 = sub_7B8F35();`
  - `if ( v9 != 0 || CompareFileTime(lpFileTime1: (v6 + 1119), lpFileTime2: &FileTime2) >= 0 )`
  - `LOBYTE(v45) = 5;`
  - `ZXString<char>::Format(a1: &v43, a2: v13, a3: v37[0]);`
  - `LOBYTE(v45) = 4;`
  - `ZXString<char>::operator=(a1: String);`
  - …（截断 16 行）

#### CUIToolTip::SetToolTip_Equip_Basic  (14966 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1001_SHINE`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_2042_STR_`, `SP_2043_DEX_`, `SP_2044_INT_`, `SP_2045_LUK_`, `SP_2046_HP_`, `SP_2047_MP_`, `SP_5530_CATEGORY_`, `SP_656_ATTACK_SPEED_`, `SP_657_WEAPON_ATTACK_`, `SP_658_MAGIC_ATTACK_`, `SP_659_WEAPON_DEF_`, `SP_660_MAGIC_DEF_`, `SP_661_ACCURACY_`, `SP_662_AVOIDABILITY_`, `SP_663_HANDS_`, `SP_664_SPEED_`, `SP_665_JUMP_`, `SP_667_THE_RATE_OF_KNOCK_BACK_`, `SP_668_NUMBER_OF_UPGRADES_AVAILABLE_`, `SP_669_ADD_PREVENT_SLIPPING`, `SP_670_ADD_PREVENT_COLDNESS`
  - `void __thiscall CUIToolTip::SetToolTip_Equip_Basic(CUIToolTip *this, struct GW_ItemSlotEquip *a2)`
  - `Data = TSecType<long>::GetData(a1: a2 + 12, a2: dword_BE78D8);`
  - `EquipItem = CItemInfo::GetEquipItem(this: v2, a2: Data);`
  - `if ( EquipItem == 0 )`
  - `v6 = TSecType<long>::GetData(a1: v3 + 12, a2: 0);`
  - `get_weapon_category_name(a1: &v82, nItemID: v6);`
  - `LOBYTE(v85) = 1;`
  - `v7 = TSecType<long>::GetData(a1: v3 + 12, a2: 0);`
  - `get_weapon_attack_speed(a1: &v83, nItemID: v7);`
  - `LOBYTE(v85) = 2;`
  - `v8 = TSecType<long>::GetData(a1: v3 + 12, a2: 0);`
  - `get_item_category_name(a1: &v84, a2: v8);`
  - `LOBYTE(v85) = 3;`
  - `if ( v82 != 0 )`
  - `sub_428211(a1: &v82);`
  - `LOBYTE(v85) = 4;`
  - …（截断 16 行）

#### CUIToolTip::SetToolTip_MacroSysSkill  (7388 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_4161_UI_UIWINDOWIMG_SKILLMACRO_MACROICON_D_ICON`, `SP_4336_REGISTER_THE_FREQUENTLY_USED_SKILLS_FOR_A_QUICK_ACCESS_WITH_A_SINGLE_BUTTON`
  - `void __thiscall CUIToolTip::SetToolTip_MacroSysSkill(`
  - `if ( *(this + 4) == 0 || *(this + 5) != a2 || *(this + 6) != Unknown )`
  - `CUIToolTip::SetBasicInfo(this, a2: 11, a3: 290, a4: 142, a5: 125);`
  - `CUIToolTip::MakeLayer(this, a2: &v41, a3: a2, a4: Unknown, a5: 1, a6: 0, a7: 0, a8: -1610612672);`
  - `LOBYTE(v44) = 1;`
  - `CUIToolTip::GetFontByType(this, a2: &v35, a3: 1);`
  - `LOBYTE(v44) = 2;`
  - `v6 = sub_8ADF76();`
  - `MacroName = CMacroSysMan::GetMacroName(this: v6, a2: &Unknown, a3: v34);`
  - `LOBYTE(v44) = 4;`
  - `v8 = sub_428226(a1: MacroName);`
  - `sub_8F466D(a1: 10, a2: v8, a3: v35);`
  - `CUIToolTip::GetFontByType(this, a2: &p_pvargDest, a3: 7);`
  - `LOBYTE(v44) = 5;`
  - `LOBYTE(v44) = 7;`
  - `v12 = sub_428226(a1: String);`
  - …（截断 16 行）

#### CUIToolTip::SetToolTip_Package  (62186 B)

- SP_ 节点：`SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_2879_UI_CASHSHOPIMG_CSDISCOUNT`, `SP_5527_ARIAL`
  - `int __thiscall CUIToolTip::SetToolTip_Package(`
  - `if ( sub_444F15(a1: 0) == 0 && *(this + 5) == arg0 && *(this + 6) == a3 )`
  - `return ZRef<CS_COMMODITY>::~ZRef<CS_COMMODITY>(a1: &a4);`
  - `sub_402F85(pvargDest: &v253, pvargSrc: &pvargSrc);`
  - `LOBYTE(v325) = 1;`
  - `sub_402F85(pvargDest: &v252, pvargSrc: &pvargSrc);`
  - `LOBYTE(v325) = 2;`
  - `v11 = sub_406472();`
  - `v12 = sub_402680(a1: v11);`
  - `IWzResMan::GetObjectA(this: v12, pvargDest: &v245, sUOL: v236, vParam: *&v237.vt, vAux: v237.decVal.Hi32);`
  - `LOBYTE(v325) = 4;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v237.cVal, a2: SBYTE4(v237.decVal.Lo64));`
  - `if ( Unknown != 0 )`
  - `v13 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v13 < 0 && v13 != -2147467262 )`
  - `LOBYTE(v325) = 8;`
  - …（截断 16 行）

#### CUIToolTip::SetToolTip_Pet  (13982 B)

- SP_ 节点：`SP_2050_DESCD`, `SP_671_A_GIFT_FROM_S`
  - `void __thiscall CUIToolTip::SetToolTip_Pet(`
  - `if ( sub_444F15(a1: 0) != 0 || *(v12 + 5) != a2 || *(v12 + 6) != a3 )`
  - `sub_8EBFDE(a1: v12, a2: a4, a3: a5, a4: &v80);`
  - `if ( a7 != 0 && strlen(a1: a7) != 0 )`
  - `LOBYTE(v84) = 2;`
  - `ZXString<char>::Format(a1: &v78, a2: v15, a3: a7);`
  - `LOBYTE(v84) = 1;`
  - `LOBYTE(v84) = 3;`
  - `a7 = (a4 + 3);`
  - `Data = TSecType<long>::GetData(a1: a4 + 12, a2: 0);`
  - `CItemInfo::GetItemName(a1: &v82, a2: Data);`
  - `LOBYTE(v84) = 4;`
  - `if ( a5 != 0 || sub_4E4003(a1: a4) == 0 )`
  - `v21 = TSecType<long>::GetData(a1: a7, a2: 0);`
  - `ItemDesc = CItemInfo::GetItemDesc(a1: &a5, a2: v21);`
  - `LOBYTE(v84) = 7;`
  - …（截断 16 行）

#### CUIToolTip::SetToolTip_Ring  (17535 B)

- SP_ 节点：`SP_2727_SAMPLE`, `SP_4200_YOU_ARE_MARRIED_WITH__S`, `SP_672_A_COUPLE_RING_WITH__S`, `SP_673_THIS_IS_YOUR_FRIENDSHIP_RING_WITH_S`
  - `int __thiscall CUIToolTip::SetToolTip_Ring(`
  - `if ( sub_444F15(a1: 0) != 0 || *(this + 5) != x || (result = *(this + 6)) != y )`
  - `v13 = *(pe + 3) == 0;`
  - `LOBYTE(v101) = 2;`
  - `v15 = (*(v14 + 4))(a1: pe);`
  - `sub_8EBE04(a1: pe + 32, a2: a5, a3: &v89, a4: v96, a5: v15, a6: 0, a7: 0);`
  - `if ( v89 != 0 && *v89 != 0 )`
  - `sub_47CB2D(a1: -1);`
  - `ZXString<char>::operator=(a1: &v89);`
  - `Data = TSecType<long>::GetData(a1: pe + 12, a2: 0);`
  - `CItemInfo::GetItemInfo(a1: &v87, a2: Data);`
  - `LOBYTE(v101) = 3;`
  - `sub_40263B(a1: &v87);`
  - `Item = IWzProperty::GetItem(pvargDest: &v80, a2: v79.cyVal.Hi);`
  - `LOBYTE(v101) = 5;`
  - `unknown = get_unknown(a1: v85, a2: Item);`
  - …（截断 16 行）

#### CUIToolTip::SetToolTip_Skill  (12475 B)

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
  - `LOBYTE(v65) = 0;`
  - `if ( v9 < 0 )`
  - `if ( v63 == 0 )`
  - `v11 = sub_485A45(a1: v63);`
  - `v12 = *v64 * (v11 + 4) + 128;`
  - …（截断 16 行）

#### CUIToolTip::SetToolTip_SlotInc  (9210 B)

- SP_ 节点：`SP_10_EQP`, `SP_11_SETUP`, `SP_5538_ETC`, `SP_5596_USE`, `SP_577_ADD_D_SLOTS_FOR_THE_STORAGE_CURRENT_D_R_NMAXIMUM_OF_48_SLOTS_AVAILABLE`, `SP_580_ADD_D_SLOTS_FOR_THE_S_INVENTORY_CURRENT_D_SLOTS_R_NMAXIMUM_OF_D_SLOTS_AVAILABLE`, `SP_581_ADD_A_CHARACTER_SLOT_CURRENT__D_SLOTS_R_NYOU_MAY_HAVE_UP_TO_D_SLOTS`
  - `void __thiscall CUIToolTip::SetToolTip_SlotInc(`
  - `if ( (byte_BF111C & 1) == 0 )`
  - `ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF);`
  - `atexit(a1: loc_8F1E96);`
  - `v49 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &pvarg.lVal) + 4);`
  - `sub_4289B7(this: &pvarg.lVal);`
  - `LOBYTE(v56) = 2;`
  - `v50 = TSecType<long>::GetData(a1: (a5 + 16), a2: 0) / 1000 % 10;`
  - `Data = TSecType<long>::GetData(a1: (a5 + 16), a2: 0);`
  - `*v52 = sub_5D27AF(a1: Data);`
  - `v11 = TSecType<long>::GetData(a1: (a5 + 16), a2: 0);`
  - `v12 = sub_5D7787(a1: &v51, a2: v11);`
  - `LOBYTE(v56) = 3;`
  - `ZXString<char>::operator=(a1: v12);`
  - `if ( TSecType<long>::GetData(a1: (a5 + 16), a2: 0) / 1000 == 5430 )`
  - `v13 = TSecType<long>::GetData(a1: (a5 + 16), a2: 0);`
  - …（截断 16 行）

#### CUIToolTip::SetToolTip_String  (3422 B)

  - `_DWORD *__thiscall CUIToolTip::SetToolTip_String(CUIToolTip *this, int a2, int *a3, const char *a4)`
  - `v4 = *(this + 272);`
  - `if ( v4 != 0 )`
  - `(*(*v4 + 4))(a1: v4);`
  - `VariantInit(&pvarg);`
  - `v5 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v5 < 0 )`
  - `LOBYTE(v27) = 1;`
  - `v6 = sub_4284A8(a1: &v26);`
  - `v19 = (sub_485A45(a1: v6) + 6);`
  - `_bstr_t::_bstr_t(a1: a4);`
  - `sub_4284A8(a1: &v26);`
  - `v8 = sub_42782E(a1: v17, a2: p_pvarg);`
  - `CUIToolTip::SetBasicInfo(this: v25, a2: 1, a3: v8 + 8, a4: v19, a5: p_pvargDest);`
  - `LOBYTE(v27) = 0;`
  - `if ( v9 < 0 )`
  - …（截断 16 行）

#### CUIToolTip::SetToolTip_String2  (5205 B)

  - `void __thiscall CUIToolTip::SetToolTip_String2(`
  - `v11 = *(this + 4) == 0;`
  - `if ( (v11 || *(this + 5) != a2 || *(this + 6) != a3) && (a4 != 0 && *a4 != 0 || a5 != 0 && *a5 != 0) )`
  - `CUIToolTip::GetFontByType(this, a2: &v38, a3: a8 != 0 ? 7 : 1);`
  - `LOBYTE(v39) = 2;`
  - `CUIToolTip::GetFontByType(this: v36, a2: &v35, a3: v14);`
  - `LOBYTE(v39) = 3;`
  - `if ( a8 != 0 )`
  - `if ( a9 == 0 )`
  - `if ( a4 != 0 && *a4 != 0 )`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v39) = 4;`
  - `v16 = sub_428226(a1: &a4);`
  - `_bstr_t::_bstr_t(a1: v16);`
  - `sub_4284A8(a1: &v38);`
  - `v18 = sub_42782E(a1: v31, a2: p_pvarg) + v13;`
  - …（截断 16 行）

#### CUIToolTip::SetToolTip_String_MultiLine  (2156 B)

  - `void __thiscall CUIToolTip::SetToolTip_String_MultiLine(CUIToolTip *this, int *a2, int *a3, char a4, int a5, int **a6)`
  - `v7 = *(this + 272);`
  - `if ( v7 != 0 )`
  - `(*(*v7 + 4))(a1: v7);`
  - `LOBYTE(v21) = 1;`
  - `if ( a5 == 0 )`
  - `CUIToolTip::GetFontByType(this, a2: &v14, a3: 7);`
  - `v9 = sub_428226(a1: &a4);`
  - `v10 = sub_8F4535(a1: 5, a2: a5, a3: 10, a4: v9, a5: v14, a6: v15, a7: v16, a8: v17) + 10;`
  - `if ( v8 != 0 )`
  - `CUIToolTip::SetBasicInfo(this, a2: 1, a3: v11, a4: v10, a5: -1);`
  - `CUIToolTip::MakeLayer(this, a2: &a5, a3: a2, a4: a3, a5: 0, a6: 0, a7: 0, a8: -1610612672);`
  - `if ( a5 != 0 )`
  - `(*(*a5 + 8))(a1: a5);`
  - `if ( v8 == 0 )`
  - `v13 = sub_428226(a1: &a4);`
  - …（截断 16 行）

#### CUIToolTip::SetToolTip_WorldMap  (8259 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_4110_PARTY_MEMBER__S`
  - `void __thiscall CUIToolTip::SetToolTip_WorldMap(`
  - `if ( sub_444F15(a1: 0) == 0 && *(this + 5) == a2 && *(this + 6) == a3 )`
  - `LOBYTE(v41) = 1;`
  - `if ( a4 != 0 )`
  - `LOBYTE(v41) = 0;`
  - `if ( a5 != 0 )`
  - `if ( a6 != 0 )`
  - `if ( a4 != 0 && *a4 != 0 || a5 != 0 && *a5 != 0 )`
  - `LOBYTE(v41) = 4;`
  - `if ( a5 != 0 && *a5 != 0 )`
  - `FontByType = CUIToolTip::GetFontByType(this, a2: &v38, a3: 1);`
  - `sub_4288D7(a1: *FontByType);`
  - `if ( v38 != 0 )`
  - `(*(*v38 + 8))(a1: v38);`
  - `VariantInit(&pvarg);`
  - `v14 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - …（截断 16 行）

#### CUIToolTip::ShowItemToolTip  (6463 B)

  - `void __thiscall CUIToolTip::ShowItemToolTip(`
  - `if ( a5 != 0 && (v10 = *(a5 + 4)) != 0 )`
  - `v53 = *(v10 + 44);`
  - `if ( a5 != 0 )`
  - `*v57 = *(a5 + 8);`
  - `if ( a5 != 0 && (v11 = *(a5 + 4)) != 0 )`
  - `v58 = *(v11 + 32);`
  - `if ( a5 != 0 && (v12 = *(a5 + 4)) != 0 )`
  - `v61 = *(v12 + 12);`
  - `if ( a4 == 0 )`
  - `CUIToolTip::ClearToolTip(this);`
  - `if ( a9 != 0 )`
  - `v54 = *(a5 + 20);`
  - `v15 = (a5 + 24);`
  - `if ( v14 != 0 )`
  - `v60 = *(a5 + 32);`
  - …（截断 16 行）

### CUIItem（12 函数）


#### CUIItem::ChangeTab  (121 B)

  - `void __thiscall CUIItem::ChangeTab(CCtrlTab **this, int a2)`
  - `CCtrlTab::SetTab(this: *(this + 365), a2);`

#### CUIItem::Draw  (13325 B)

- SP_ 节点：`SP_1849_UI_UIWINDOWIMG_ITEM_ACTIVEICON`
  - `int *__thiscall CUIItem::Draw(signed int *this, IWzCanvas *i)`
  - `CWnd::Draw(this, a2: i);`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v61.lVal) + 4);`
  - `sub_4289B7(this: &v61.lVal);`
  - `CWnd::GetCanvas(a1: &v66);`
  - `HIDWORD(v56[0]) = *(v3 + 173);`
  - `LOBYTE(v67) = 1;`
  - `v4 = _ZtlSecureFuse<long>(a1: v3 + 165, a2: HIDWORD(v56[0]));`
  - `format_integer(a1: &v64, a2: v4, a3: 1);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `HIDWORD(v56[0]) = &pvargDest;`
  - `LOBYTE(v67) = 2;`
  - `v5 = sub_428226(a1: &v64);`
  - `LODWORD(v56[0]) = v6;`
  - `_bstr_t::_bstr_t(a1: v5);`
  - `LOBYTE(v67) = 3;`
  - …（截断 16 行）

#### CUIItem::GetItemSlotRect  (1546 B)

  - `signed int __thiscall CUIItem::GetItemSlotRect(int *this, int a2, int a3)`
  - `v4 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v15) + 4);`
  - `if ( v16 != 0 )`
  - `sub_428C44(this: &v15, a2: 0);`
  - `v5 = *(v4 + 4 * *(this + 377) + 1095);`
  - `if ( v5 != 0 )`
  - `v5 = *(v5 - 4);`
  - `v6 = *(this + 385);`
  - `if ( v6 != 0 )`
  - `v7 = *(this + 376);`
  - `if ( a2 < v7 )`
  - `if ( v8 >= v5 )`
  - `if ( a2 < v8 )`
  - `v10 = a2 - *(this + 376);`
  - `if ( v6 != 0 && v11 >= 6 )`
  - `dword_BF040C(a1: a3, a2: v13 + 36 * v12 + 8, a3: 34 * v11 + 50, a4: v13 + 36 * v12 + 40, a5: 34 * v11 + 82);`
  - …（截断 16 行）

#### CUIItem::GetSlotPositionFromPoint  (1082 B)

  - `int __thiscall CUIItem::GetSlotPositionFromPoint(int *this, int rx, int ry)`
  - `v4 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v11) + 4);`
  - `if ( v12 != 0 )`
  - `sub_428C44(this: &v11, a2: 0);`
  - `v5 = *(v4 + 4 * *(this + 377) + 1095);`
  - `if ( v5 != 0 )`
  - `v6 = *(v5 - 4);`
  - `if ( *(this + 385) != 0 )`
  - `v7 = *(this + 376) + 24;`
  - `if ( v7 >= v6 )`
  - `for ( i = *(this + 376); i < v7; ++i )`
  - `CUIItem::GetItemSlotRect(this, a2: i, a3: rcSlot);`
  - `if ( dword_BF0484(a1: rcSlot, a2: rx, a3: ry) != 0 )`

#### CUIItem::OnChildNotify  (3881 B)

- SP_ 节点：`SP_4501_EXPAND_INVENTORY`, `SP_4502_COLLAPSE_INVENTORY`, `SP_4504_YOU_CAN_EXPAND_THE_INVENTORY_SLOT_WITH_CASH_ITEMS`, `SP_697_SLOT_MERGER`, `SP_698_ITEM_SORT`
  - `void __thiscall CUIItem::OnChildNotify(CUIItem *this, unsigned int a2, unsigned int a3, int a4)`
  - `if ( a2 == 2000 )`
  - `if ( a3 == 500 )`
  - `CUIItem::OnTabChanged(this, a2: a4);`
  - `else if ( a2 == 2001 && a3 >= 0x12C && a3 <= 0x130 )`
  - `CUIToolTip::ClearToolTip(this: (this + 116));`
  - `dword_BF0EB4[*(this + 377)] = v4;`
  - `*(this + 376) = 4 * v4 + 1;`
  - `CWnd::InvalidateRect(this, a2: 0);`
  - `CUIItem::SetLastestGetItemEffect(a1: this);`
  - `if ( a3 == 101 )`
  - `if ( a2 != 2003 )`
  - `switch ( a2 )`
  - `if ( v4 != 0 )`
  - `sub_81C2D1(a1: v15);`
  - `sub_81C2D1(a1: v14);`
  - …（截断 16 行）

#### CUIItem::OnCreate  (18077 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1253_UI_BASICIMG_ITEMNO`, `SP_1848_UI_UIWINDOWIMG_ITEM_BTCOIN`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_2662_UI_UIWINDOWIMG_ITEM_TAB_ENABLED`, `SP_2663_UI_UIWINDOWIMG_ITEM_TAB_DISABLED`, `SP_4498_UI_UIWINDOWIMG_ITEM_FULLBACKGRND`, `SP_4499_UI_UIWINDOWIMG_ITEM_BACKGRND`, `SP_4500_UI_UIWINDOWIMG_ITEM_BTCASHSHOP`, `SP_4529_UI_UIWINDOWIMG_ITEM_DISABLED`, `SP_5580_UI_UIWINDOWIMG_ITEM_BTFULL`, `SP_5581_UI_UIWINDOWIMG_ITEM_BTSMALL`
  - `int __thiscall CUIItem::OnCreate(_DWORD *this, CCtrlTab *Unknown)`
  - `v3 = *(this + 385) == 0;`
  - `if ( v3 )`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `CUIWnd::OnCreate(a1: Unknown, a2: v89);`
  - `if ( (v107 & 2) != 0 )`
  - `if ( v105 != 0 )`
  - `if ( (v107 & 1) != 0 )`
  - `if ( v102 != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v108) = 3;`
  - `v9 = sub_406472();`
  - `v10 = sub_402680(a1: v9);`
  - `IWzResMan::GetObjectA(this: v10, pvargDest: &v95, sUOL: v85, vParam: p_pvarg, vAux: p_pvargDest);`
  - `LOBYTE(v108) = 5;`
  - …（截断 16 行）

#### CUIItem::OnDropMoney  (3387 B)

- SP_ 节点：`SP_5280_HOW_MANY_WILL_YOU_DROP_A_PORTION_OF_THE_AMOUNT_MAY_BE_LOST_IF_PICKED_UP_BY_ANOTH`, `SP_66_YOU_CANT_DO_IT_ONCE_YOURE_DEAD`, `SP_775_THE_ADMIN_ACCOUNT_CANT_THROW_AWAY_THE_MONEY`
  - `int __thiscall CUIItem::OnDropMoney(CUIItem *this)`
  - `if ( (*(TSingleton<CUserLocal>::ms_pInstance + 1392) & 0xFFFFFFFE) == 0x12 )`
  - `return CUtilDlg::Notice(sMsg: v14, sSndName: v15, ppDialog: v16, bAutoSeparated: v17, bIsTightLine: v18);`
  - `if ( sub_4F38D3(this: *dword_BE7918) != 0 && sub_4F38B4(this: v1) )`
  - `v5 = *(CWvsContext::GetCharacterData(this: v1, a2: &v19) + 4);`
  - `if ( v20 != 0 )`
  - `sub_428C44(this: &v19, a2: 0);`
  - `v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v6 != 0 )`
  - `v7 = CUtilDlgEx::CUtilDlgEx(this: v6);`
  - `v22 = v8 != 0 ? (v8 - 8) : 0;`
  - `sub_4289CD(a1: v21);`
  - `LOBYTE(v25) = 1;`
  - `v11 = sub_428688(a1: v21);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v11, a2: 2, a3: 0, a4: v16, a5: v17, a6: v18);`
  - `v12 = _ZtlSecureFuse<long>(a1: v5 + 165, a2: *(v5 + 173));`
  - …（截断 16 行）

#### CUIItem::OnMouseButton  (9269 B)

- SP_ 节点：`SP_1847_DRAGSTART`
  - `_DWORD *__thiscall CUIItem::OnMouseButton(_DWORD *this, int *msg, int wParam, int rx, _DWORD *ry)`
  - `if ( msg == 513 )`
  - `result = CWvsContext::CanSendExclRequest(a1: 500, a2: 0);`
  - `if ( result != 0 )`
  - `v8 = *(CWvsContext::GetCharacterData(this: v7, a2: &v45) + 4);`
  - `sub_4289B7(this: &v45);`
  - `v48 = (v6 - 1);`
  - `SlotPositionFromPoint = CUIItem::GetSlotPositionFromPoint(this: v6 - 1, rx, ry);`
  - `v9 = *(CharacterData::GetItem(this: v8, a2: &pvarg.lVal, a3: v37, a4: SlotPositionFromPoint) + 4);`
  - `sub_4288C1(a1: &pvarg.decVal.Lo32);`
  - `if ( v9 != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `LOBYTE(v51) = 1;`
  - `if ( dword_BF14EC == 0 )`
  - `IWzGr2D::CreateLayer(`
  - …（截断 16 行）

#### CUIItem::OnMouseMove  (1692 B)

  - `int __thiscall CUIItem::OnMouseMove(char **this, int a2, int a3)`
  - `SlotPositionFromPoint = CUIItem::GetSlotPositionFromPoint(this: this - 1, rx: a2, ry: a3);`
  - `if ( SlotPositionFromPoint != 0 )`
  - `CInputSystem::SetCursorState(this: dword_BEC33C, a2: 5);`
  - `v4 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v11) + 4);`
  - `if ( v12 != 0 )`
  - `sub_428C44(this: &v11, a2: 0);`
  - `v5 = *(CharacterData::GetItem(this: v4, a2: &v11, a3: *(this + 376), a4: SlotPositionFromPoint) + 4);`
  - `ZRef<GW_ItemSlotBase>::~ZRef<GW_ItemSlotBase>(a1: 0);`
  - `if ( v5 != 0 )`
  - `CUIToolTip::ItemToolTipParam::ItemToolTipParam(this: v10);`
  - `v9 = ((*(v6 + 12))(a1: this) + a3 + 20);`
  - `v7 = (*(*this + 11))(a1: this);`
  - `CUIToolTip::ShowItemToolTip(this: this + 28, a2: (a2 + v7), a3: v9, a4: v5, a5: v10, a6: 0, a7: 0, a8: 0, a9: 0);`
  - `CUIToolTip::ItemToolTipParam::~ItemToolTipParam(this: v10);`
  - `CUIToolTip::ClearToolTip(this: (this + 28));`
  - …（截断 16 行）

#### CUIItem::OnTabChanged  (2272 B)

  - `int __thiscall CUIItem::OnTabChanged(CUIItem *this, int a2)`
  - `*(this + 361) = a2;`
  - `*(this + 377) = a2 + 1;`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v11) + 4);`
  - `if ( v12 != 0 )`
  - `sub_428C44(this: &v11, a2: 0);`
  - `v4 = *(this + 367);`
  - `if ( v4 != 0 )`
  - `v5 = *(v3 + 4 * *(this + 377) + 1095);`
  - `if ( v5 != 0 )`
  - `v6 = *(v5 - 4);`
  - `CCtrlScrollBar::SetScrollRange(this: v4, a2: (v6 - 22) / 4 + 1);`
  - `CCtrlScrollBar::SetCurPos(this: *(this + 367), a2: dword_BF0EB4[*(this + 377)]);`
  - `*(this + 376) = 4 * *(*(this + 367) + 56) + 1;`
  - `*(this + 376) = 1;`
  - `if ( *(this + 385) != 0 )`
  - …（截断 16 行）

#### CUIItem::SetArrangeButton  (2810 B)

- SP_ 节点：`SP_1851_UI_UIWINDOWIMG_ITEM_BTGATHER`, `SP_1852_UI_UIWINDOWIMG_ITEM_BTSORT`
  - `void __thiscall CUIItem::SetArrangeButton(CUIItem *this, int a2, int a3)`
  - `*(this + a2 + 378) = a3;`
  - `if ( v3 == *(this + 377) )`
  - `memset(v14, 0, sizeof(v14));`
  - `if ( v5 != 0 )`
  - `if ( v5 == 1 )`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `v8 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v17) = 4;`
  - `if ( v8 != 0 )`
  - `CCtrlButton::CCtrlButton(this: v8);`
  - `LOBYTE(v17) = 0;`
  - `sub_81F194(a1: v9);`
  - `(*(**(this + 371) + 32))(a1: this, a2: 2004, a3: *(this + 356) - 15, a4: 6, a5: 0, a6: v14);`
  - `ZXString<unsigned short>::operator=(a1: v11);`
  - `v12 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - …（截断 16 行）

#### CUIItem::SetLastestGetItemEffect  (30013 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_2663_UI_UIWINDOWIMG_ITEM_TAB_DISABLED`, `SP_5329_UI_UIWINDOWIMG_ITEM_NEW_TAB1`, `SP_5330_UI_UIWINDOWIMG_ITEM_NEW_TAB0`, `SP_5331_UI_UIWINDOWIMG_ITEM_NEW_INVENTORY`
  - `int __thiscall CUIItem::SetLastestGetItemEffect(CUIItem *this)`
  - `v117 = (this + 1548);`
  - `sub_428712(a1: 0);`
  - `v127 = (this + 1552);`
  - `v3 = *(*dword_BE7918 + 14532);`
  - `*(this + 390) = v3;`
  - `result = *(v2 + 14536);`
  - `*(this + 391) = result;`
  - `if ( v3 != 0 && result != 0 )`
  - `v113 = 34 * (v3 / 1000000);`
  - `v5 = *(*(this + 365) + 60);`
  - `LOBYTE(v132) = 1;`
  - `if ( v130 - 1 == v5 )`
  - `sub_402F85(&pvargDest, pvargSrc: cyVal.Hi);`
  - `LOBYTE(v132) = 2;`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - …（截断 16 行）

### CUIEquip（4 函数）


#### CUIEquip::OnButtonClicked  (592 B)

  - `void __fastcall CUIEquip::OnButtonClicked(CUIEquip *this, int a2, unsigned int a3)`
  - `switch ( a3 )`
  - `*(*dword_BE7918 + 14332) = 50200052;`
  - `CWvsContext::SendMigrateToShopRequest(this: *dword_BE7918, edx0: a2, a3: 0);`
  - `CUIEquip::ToggleDragonEquip(a1: this, a2);`
  - `CUIEquip::TogglePetEquip(this);`
  - `CUIWnd::OnButtonClicked(a1: a3);`

#### CUIEquip::OnMouseMove  (3617 B)

  - `int __thiscall CUIEquip::OnMouseMove(CUIEquip *this, int a2, int a3)`
  - `v26 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v23) + 4);`
  - `if ( v24 != 0 )`
  - `sub_428C44(this: &v23, a2: 0);`
  - `v24 = sub_7FEC32(a1: a2, a2: a3);`
  - `v27 = *(CharacterData::GetItem(this: v26, a2: v21, a3: 1, a4: -v24) + 4);`
  - `if ( v22 != 0 )`
  - `ZRef<GW_ItemSlotBase>::~ZRef<GW_ItemSlotBase>(a1: 0);`
  - `v29 = *(CharacterData::GetItem(this: v26, a2: v25, a3: 1, a4: -100 - v24) + 4);`
  - `if ( v26 != 0 )`
  - `if ( v27 == 0 )`
  - `if ( v29 == 0 )`
  - `CUIToolTip::ClearToolTip(this: (this + 112));`
  - `CInputSystem::SetCursorState(this: dword_BEC33C, a2: 0);`
  - `CInputSystem::SetCursorState(this: dword_BEC33C, a2: 5);`
  - `sub_7B49F6(a1: 0);`
  - …（截断 16 行）

#### CUIEquip::ToggleDragonEquip  (788 B)

  - `int __thiscall CUIEquip::ToggleDragonEquip(CUIEquip *this)`
  - `v2 = *(this + 373);`
  - `if ( v2 != 0 )`
  - `CWnd::Destroy(a1: v2);`
  - `return sub_801B68(a1: this + 1488);`
  - `v7 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1408);`
  - `if ( v7 != 0 )`
  - `v6 = (*(*(this + 1) + 48))(a1: this + 4) + 117;`
  - `v4 = (*(*(this + 1) + 44))(a1: this + 4);`
  - `v5 = sub_7FF968(a1: v7, a2: SHIDWORD(v4), a3: v4 - 142, a4: v6);`
  - `return sub_801AFD(a1: v5);`

#### CUIEquip::TogglePetEquip  (853 B)

  - `void *__thiscall CUIEquip::TogglePetEquip(CUIEquip *this)`
  - `if ( *(this + 365) != 0 )`
  - `result = sub_7FFB17(this);`
  - `*(this + 361) = 0;`
  - `v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1456);`
  - `if ( v6 != 0 )`
  - `v5 = (*(*(this + 1) + 48))(a1: this + 4) + 123;`
  - `v3 = (*(*(this + 1) + 44))(a1: this + 4);`
  - `v4 = sub_7FE299(a1: v6, a2: SHIDWORD(v3), a3: v3, a4: v5);`
  - `sub_801AB3(a1: v4);`
  - `*(this + 361) = 1;`

### CUIStat（8 函数）


#### CUIStat::AutoApUp  (4144 B)

- 分支 case：`0x80`, `0x100`, `0x200`
- SP_ 节点：`SP_1951_____YOUR_AP_WILL_BE_DISTRIBUTED_AS_FOLLOWS_R_N_R_N`, `SP_1952______S__D_R_N`
  - `int __thiscall CUIStat::AutoApUp(CUIStat *this, int a2)`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v25) + 4);`
  - `sub_4289B7(this: &v25);`
  - `if ( sub_7939BA(a1: v2, a2, a3: &v30) == 0 )`
  - `v4 = _ZtlSecureFuse<short>(a1: v3 + 129, a2: *(v3 + 133));`
  - `if ( v4 < 1 )`
  - `for ( i = 0; v5 != 0 && i < *(v5 - 1); ++i )`
  - `if ( v7 >= v4 )`
  - `if ( v4 > 0 )`
  - `if ( v5 != 0 )`
  - `v9 = *(v5 - 1);`
  - `while ( v5 != 0 && v10 < *(v5 - 1) )`
  - `if ( *v11 == 64 )`
  - `switch ( *v11 )`
  - `LOBYTE(v31) = 2;`
  - `for ( j = 0; j < 4; ++j )`
  - …（截断 16 行）

#### CUIStat::Draw  (39009 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1939_D_DD`, `SP_1954_D_D`, `SP_5153_UI_UIWINDOWIMG_STAT_BASICSTAT`, `SP_5515_D`, `SP_5516_D___D`, `SP_61_GUNSLINGER`
  - `VARIANTARG *__thiscall CUIStat::Draw(CUIStat *this, VARIANTARG **a2)`
  - `CWnd::Draw(this, a2);`
  - `CWnd::GetCanvas(a1: &v222);`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v213.lVal) + 4);`
  - `sub_4289B7(this: &v213.lVal);`
  - `v4 = (v2 + 8380);`
  - `LOBYTE(v224) = 1;`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v224) = 2;`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `HIDWORD(v210[0]) = &pvargDest;`
  - `LODWORD(v210[0]) = &pvarg;`
  - `LOBYTE(v224) = 3;`
  - `HIDWORD(v209) = sub_4284A5(this: v221 + 386);`
  - `LODWORD(v209) = v5;`
  - `_bstr_t::_bstr_t(a1: v3 + 4);`
  - …（截断 16 行）

#### CUIStat::EnableApUpButton  (2357 B)

  - `int __thiscall CUIStat::EnableApUpButton(CUIStat *this)`
  - `v2 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v15) + 4);`
  - `if ( v16 != 0 )`
  - `sub_428C44(this: &v15, a2: 0);`
  - `v3 = _ZtlSecureFuse<short>(a1: v2 + 129, a2: *(v2 + 133));`
  - `if ( *v5 != 0 )`
  - `(*(*(*v5 + 4) + 28))(a1: *v5 + 4, a2: v4);`
  - `while ( i != 0 );`
  - `if ( v4 )`
  - `v6 = _ZtlSecureFuse<unsigned char>(a1: v2 + 51, a2: *(v2 + 53));`
  - `_ZtlSecureFuse<short>(a1: v2 + 57, a2: *(v2 + 61));`
  - `if ( v6 < 0x14u )`
  - `for ( i = 2; i != 0; --i )`
  - `if ( *v7 != 0 )`
  - `(*(*(*v7 + 4) + 28))(a1: *v7 + 4, a2: 0);`
  - `for ( i = 3; i != 0; --i )`
  - …（截断 16 行）

#### CUIStat::OnButtonClicked  (1177 B)

- 分支 case：`2000`, `2001`, `2002`, `2003`, `2004`, `2005`, `2006`, `2007`, `2008`, `2009`
  - `void __thiscall CUIStat::OnButtonClicked(_DWORD *this, int a2)`
  - `switch ( a2 )`
  - `CWvsContext::SendAbilityUpRequest(this: *dword_BE7918, a2: v2);`
  - `CUIStat::ToggleDetail(this);`
  - `CUIStat::AutoApUp(a1: 1);`
  - `CUIStat::AutoApUp(a1: 0);`
  - `CUIWnd::OnButtonClicked(a1: a2);`

#### CUIStat::OnCreate  (8626 B)

- SP_ 节点：`SP_1947_UI_UIWINDOWIMG_STAT_DISABLED_`, `SP_1953_STRING_TOOLTIPHELPIMG_GAME_UIWND_STAT`, `SP_1955_UI_UIWINDOWIMG_STAT_JOB`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_2064_UI_UIWINDOWIMG_STAT_BTDETAIL`
  - `void __thiscall CUIStat::OnCreate(_DWORD *this, int *a1)`
  - `sub_41C06C(a1: &word_AF0E08, a2: -1);`
  - `CUIWnd::OnCreate(a1, a2: v37);`
  - `CUIStat::RestoreButtons(a1: this);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v44, pvargSrc: &pvargSrc);`
  - `LOBYTE(v54) = 1;`
  - `v5 = sub_406472();`
  - `v6 = sub_402680(a1: v5);`
  - `IWzResMan::GetObjectA(this: v6, pvargDest: &pvarg, sUOL: v33, vParam: v34, vAux: p_pvargDest);`
  - `LOBYTE(v54) = 3;`
  - `a1 = Ztl_variant_t::GetUnknown(a1: p_a2, a2: v37);`
  - `v7 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(&a1);`
  - `if ( v7 < 0 && v7 != -2147467262 )`
  - `if ( v8 < 0 )`
  - `LOBYTE(v54) = 0;`
  - …（截断 16 行）

#### CUIStat::ResetInfo  (340 B)

  - `void __thiscall CUIStat::ResetInfo(CWnd **this)`
  - `CUIStat::RestoreButtons(a1: this);`
  - `CUIStat::EnableApUpButton(this);`
  - `CWnd::InvalidateRect(this, a2: 0);`
  - `v2 = *(this + 385);`
  - `if ( v2 != 0 )`
  - `CWnd::InvalidateRect(this: v2, a2: 0);`

#### CUIStat::RestoreButtons  (9632 B)

- SP_ 节点：`SP_1943_UI_UIWINDOWIMG_STAT_BTAPUP`, `SP_1944_UI_UIWINDOWIMG_STAT_BTAUTO`, `SP_1945_UI_UIWINDOWIMG_STAT_BTAUTO1`, `SP_1946_UI_UIWINDOWIMG_STAT_BTAUTO2`, `SP_1948_STRING_TOOLTIPHELPIMG_GAME_BUTTON_STATAUTO`, `SP_1949_STRING_TOOLTIPHELPIMG_GAME_BUTTON_STATAUTO1`, `SP_1950_STRING_TOOLTIPHELPIMG_GAME_BUTTON_STATAUTO2`
  - `void __thiscall CUIStat::RestoreButtons(CUIStat *this)`
  - `v2 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v52) + 4);`
  - `sub_4289B7(this: v52);`
  - `v49 = *(v2 + 61);`
  - `v3 = _ZtlSecureFuse<short>(a1: v2 + 57, a2: v49);`
  - `if ( (v3 == 0 || v3 == 1000 || v3 == 2000 || v3 == 2001)`
  - `&& _ZtlSecureFuse<unsigned char>(a1: v2 + 51, a2: *(v2 + 53)) <= 0xAu )`
  - `for ( i = 6; i != 0; --i )`
  - `for ( j = 3; j != 0; --j )`
  - `LOBYTE(v54) = 1;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `LOBYTE(v54) = 0;`
  - `if ( *(this + 365) == 0 )`
  - `v10 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v54) = 2;`
  - `if ( v10 != 0 )`
  - …（截断 16 行）

#### CUIStat::ToggleDetail  (870 B)

  - `int __thiscall CUIStat::ToggleDetail(_DWORD *this)`
  - `if ( *(this + 385) != 0 )`
  - `result = sub_8C5531(a1: this);`
  - `*(this + 361) = 0;`
  - `v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1448);`
  - `if ( v6 != 0 )`
  - `v5 = (*(*(this + 1) + 48))(a1: this + 1) + 144;`
  - `v3 = (*(*(this + 1) + 44))(a1: this + 1);`
  - `v4 = CUIStatDetail::CUIStatDetail(a1: v6, a2: SHIDWORD(v3), a3: v3, a4: v5);`
  - `sub_8C88BC(a1: v4);`
  - `*(this + 361) = 1;`

### CUIStatDetail（2 函数）


#### CUIStatDetail::Draw  (54280 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1939_D_DD`, `SP_1940_D__D`, `SP_1941_D`
  - `_DWORD *__thiscall CUIStatDetail::Draw(CUIStatDetail *this, struct tagRECT *a2)`
  - `void (__stdcall *v87)(VARIANTARG *); // ebx`
  - `CWnd::Draw(this, a2);`
  - `CWnd::GetCanvas(a1: &v247);`
  - `get_basic_font(a1: &v248, fType: 25);`
  - `LOBYTE(v253) = 1;`
  - `get_basic_font(a1: &v240, fType: 27);`
  - `LOBYTE(v253) = 2;`
  - `get_basic_font(a1: &v245, fType: 30);`
  - `LOBYTE(v253) = 3;`
  - `get_basic_font(a1: &v246, fType: 31);`
  - `LOBYTE(v253) = 5;`
  - `v252 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v235.lVal) + 4);`
  - `sub_4289B7(this: &v235.lVal);`
  - `v3 = (v2 + 2125);`
  - `LOBYTE(v253) = 6;`
  - …（截断 16 行）

#### CUIStatDetail::OnButtonClicked  (208 B)

  - `int __stdcall CUIStatDetail::OnButtonClicked(int a1)`
  - `if ( a1 == 1000 )`
  - `return CUIStat::ToggleDetail(this: dword_BF10C4);`

### CUIUserInfo（13 函数）


#### CUIUserInfo::CreatePetAvatar  (11566 B)

  - `int __thiscall CUIUserInfo::CreatePetAvatar(CUIUserInfo *this, int arg0)`
  - `v49 = *(this + arg0 + 428);`
  - `CActionMan::LoadPetAction(a1: dword_BE78D4, a2: *(this + arg0 + 418), a3: 1, a4: v49, a5: v54);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v65) = 1;`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `LOBYTE(v65) = 2;`
  - `if ( dword_BF14EC == 0 )`
  - `v50 = *IWzGr2D::CreateLayer(`
  - `v62 = (this + 4 * arg0 + 1784);`
  - `sub_428712(a1: v50);`
  - `sub_4156C5(a1: &a2);`
  - `if ( v3 < 0 )`
  - `LOBYTE(v65) = 0;`
  - `if ( v4 < 0 )`
  - `Layer = CWnd::GetLayer(a1: &v63);`
  - …（截断 16 行）

#### CUIUserInfo::Draw  (86126 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_101_MAPLESTORY_ERROR__LOADING_TAMINGMOB_FAILED`, `SP_147_PLEASE_TRY_AGAIN`, `SP_160_YOU_HAVE_NO_BUDDY_AVAILABLE_FOR_CHAT`, `SP_168_YOU_HAVE_ENTERED_AN_INVALID_CHARACTER_NAME`, `SP_182__START`, `SP_183__RESET`, `SP_1887_REQLEVEL`, `SP_189__GIVEPRIZE`, `SP_236__POINTER03`, `SP_2564_UI_UIWINDOWIMG_USERINFO_REQLV`, `SP_2577_UI_UIWINDOWIMG_USERINFO_MARRY_MARRIED_0`, `SP_2578_UI_UIWINDOWIMG_USERINFO_MARRY_UNMARRIED_0`, `SP_52_ASSASSIN`, `SP_5515_D`, `SP_58_BRAWLER`
  - `void __thiscall CUIUserInfo::Draw(CUIUserInfoDetail *this, const struct tagRECT *a2)`
  - `void (__stdcall *v237)(VARIANTARG *); // esi`
  - `void (__stdcall *v279)(VARIANTARG *); // esi`
  - `CWnd::Draw(this, a2);`
  - `CWnd::GetCanvas(a1: &v342);`
  - `LOBYTE(v355) = 1;`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v355) = 2;`
  - `sub_402F85(pvargDest: &v348, pvargSrc: &pvargSrc);`
  - `LOBYTE(v355) = 3;`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `HIDWORD(v330[0]) = &pvargDest;`
  - `LODWORD(v330[0]) = &v348;`
  - `LOBYTE(v355) = 4;`
  - `basic_font = get_basic_font(a1: &Unknown, fType: 0);`
  - `LOBYTE(v355) = 5;`
  - …（截断 16 行）

#### CUIUserInfo::OnButtonClicked  (4161 B)

  - `void __thiscall CUIUserInfo::OnButtonClicked(int this, unsigned int a2)`
  - `if ( a2 <= 0x7D7 )`
  - `if ( a2 == 2007 )`
  - `if ( *(this + 3116) != 0 )`
  - `sub_9009BF(this);`
  - `if ( a2 == 2003 )`
  - `if ( get_field() != 0 )`
  - `get_field();`
  - `CField::SendInviteTradingRoomMsg(a1: this + 1640, a2: 0);`
  - `if ( a2 == 1000 )`
  - `CWvsContext::UI_Close(this: *dword_BE7918, a2: 10);`
  - `if ( a2 == 2000 )`
  - `if ( *(this + 1580) == 0 )`
  - `if ( a2 != 2001 )`
  - `if ( a2 == 2002 )`
  - `if ( *(*dword_BE7918 + 11988) != 0`
  - …（截断 16 行）

#### CUIUserInfo::OnCreate  (11846 B)

- SP_ 节点：`SP_4682_UI_UIWINDOWIMG_USERINFO_BOSSPETCROWN`
  - `int __thiscall CUIUserInfo::OnCreate(CUIUserInfo *this, void *a2)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v55, pvargSrc: &pvargSrc);`
  - `HIDWORD(v50) = &pvargDest;`
  - `LODWORD(v50) = &v55;`
  - `LOBYTE(v67) = 1;`
  - `v5 = sub_406472();`
  - `v6 = sub_402680(a1: v5);`
  - `IWzResMan::GetObjectA(this: v6, pvargDest: &pvarg, sUOL: v49, vParam: v50, vAux: SHIDWORD(v50));`
  - `LOBYTE(v67) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v51, a2: v52);`
  - `if ( Unknown != 0 )`
  - `v7 = sub_41E527(a1: &Unknown);`
  - `if ( v7 < 0 && v7 != -2147467262 )`
  - `LOBYTE(v67) = 7;`
  - `if ( v8 < 0 )`
  - …（截断 16 行）

#### CUIUserInfo::ResetInfo_Pet  (2065 B)

  - `void __thiscall CUIUserInfo::ResetInfo_Pet(CUIUserInfo *this)`
  - `if ( *(TSingleton<CUserLocal>::ms_pInstance + 4520) == *(this + 409) )`
  - `for ( i = 3; i != 0; --i )`
  - `sub_581FAA(a1: 0);`
  - `while ( 1 )`
  - `v5 = *(*(v1 + 7900) + 8 * v4 + 4);`
  - `if ( v5 == 0 )`
  - `ItemSlot = CPet::GetItemSlot(this: v5);`
  - `Data = TSecType<long>::GetData(a1: (ItemSlot + 12), a2: this);`
  - `PetTemplate = CPetTemplate::GetPetTemplate(a1: Data);`
  - `if ( PetTemplate == 0 )`
  - `*(this + v4 + 418) = PetTemplate;`
  - `ZXString<char>::GetBuffer(a1: (ItemSlot + 40), a2: 0xFFFFFFFF);`
  - `*(this + v4 + 1696) = _ZtlSecureFuse<unsigned char>(a1: ItemSlot + 53, a2: *(ItemSlot + 55));`
  - `*(this + v4 + 850) = _ZtlSecureFuse<short>(a1: ItemSlot + 59, a2: *(ItemSlot + 63));`
  - `*(this + v4 + 1706) = _ZtlSecureFuse<unsigned char>(a1: ItemSlot + 67, a2: *(ItemSlot + 69));`
  - …（截断 16 行）

#### CUIUserInfo::SetAvatarInfo  (7383 B)

- 分支 case：`18`, `19`, `20`, `1902015`, `1902016`, `1902017`, `1902018`
  - `int __thiscall CUIUserInfo::SetAvatarInfo(CUIUserInfo *this, const struct AvatarLook *a2)`
  - `ZArray<TSecType<long>>::RemoveAll(a1: this + 1796);`
  - `for ( i = 0; i < 52; ++i )`
  - `if ( i != 0 && i != 14 )`
  - `switch ( i )`
  - `*(this + 435) = *(a2 + 97);`
  - `*(this + 436) = *(a2 + 101);`
  - `*(this + 437) = *(a2 + 105);`
  - `if ( i == 11 && *(a2 + 21) / 100000 == 17 )`
  - `TSecType<long>::TSecType<long>(this: v36, a2: *(a2 + 21));`
  - `Data = TSecType<long>::GetData(a1: v36, a2: v3);`
  - `ZArray<TSecType<long>>::Insert(a1: -1);`
  - `TSecType<long>::SetData(a1: Data);`
  - `if ( v37 != 0 )`
  - `ZAllocEx<ZAllocAnonSelector>::Free(a1: v37);`
  - `v39 = (a2 + 4 * i);`
  - …（截断 16 行）

#### CUIUserInfo::SetLayer  (12119 B)

- SP_ 节点：`SP_2051_UI_UIWINDOWIMG_USERINFO_BACKGRND`, `SP_2052_UI_UIWINDOWIMG_USERINFO_BACKGRND2`, `SP_2055_UI_UIWINDOWIMG_USERINFO_BACKGRND5`, `SP_2057_UI_UIWINDOWIMG_USERINFO_BACKGRND7`, `SP_956_CANVAS`
  - `int __thiscall CUIUserInfo::SetLayer(CUIUserInfo *this)`
  - `if ( *(this + 408) != 1 || *(this + 417) != 0 )`
  - `if ( *(this + 445) == 0 )`
  - `v2 = *(this + 445);`
  - `v3 = (*(*v2 + 224))(a1: v2, a2: i);`
  - `if ( v3 < 0 )`
  - `sub_402FAB(a1: -2, a2: 3);`
  - `Layer = CWnd::GetLayer(a1: &v62);`
  - `LOBYTE(v64) = 1;`
  - `if ( v5 )`
  - `IWzGr2DLayer::RemoveCanvas(this: *Layer, a2: &i, a3: &pvarg);`
  - `sub_4156AB(a1: &i);`
  - `sub_4156C5(a1: &v62);`
  - `if ( v6 < 0 )`
  - `v8 = *(this + 408);`
  - `if ( v8 != 0 )`
  - …（截断 16 行）

#### CUIUserInfo::SetMultiPetInfo  (1926 B)

  - `void __stdcall CUIUserInfo::SetMultiPetInfo(int a1, struct CInPacket *a2)`
  - `if ( a1 != 0 )`
  - `v4 = CInPacket::Decode4(this: v3);`
  - `CInPacket::DecodeStr(&a1);`
  - `LOBYTE(v17) = CInPacket::Decode1(this: v3);`
  - `LOWORD(v5) = CInPacket::Decode2(this: v3);`
  - `LOBYTE(v19) = CInPacket::Decode1(this: v3);`
  - `LOWORD(v6) = CInPacket::Decode2(this: v3);`
  - `v16 = CInPacket::Decode4(this: v3);`
  - `sub_428211(&a1);`
  - `LOBYTE(v20) = 0;`
  - `v8 = sub_904DDC();`
  - `CUIUserInfo::SetPetInfo(this: v8, a2: v2++, a3: v4, a4: v11, a5: v12, a6: v13, a7: v14, a8: v15, a9: v16);`
  - `v9 = CInPacket::Decode1(this: v3);`
  - `while ( v10 != 0 );`
  - `CUIUserInfo::SetState(this: dword_BEDCCC, a2: 1);`
  - …（截断 16 行）

#### CUIUserInfo::SetPetInfo  (705 B)

  - `void __thiscall CUIUserInfo::SetPetInfo(`
  - `*(this + a2 + 418) = CPetTemplate::GetPetTemplate(a1: a3);`
  - `ZXString<char>::operator=(a1: &a4);`
  - `*(this + a2 + 1696) = a5;`
  - `*(this + a2 + 850) = a6;`
  - `*(this + a2 + 1706) = a7;`
  - `*(this + a2 + 862) = a8;`
  - `*(this + a2 + 428) = a9;`
  - `CUIUserInfo::CreatePetAvatar(this, a2);`
  - `CUIUserInfo::SetPetItemList(this, a2);`

#### CUIUserInfo::SetPetItemList  (2187 B)

  - `int __thiscall CUIUserInfo::SetPetItemList(CUIUserInfo *this, int a2)`
  - `ZArray<long>::RemoveAll(a1: this + 4 * a2 + 1600);`
  - `if ( *(this + a2 + 428) != 0 )`
  - `*ZArray<long>::InsertBefore(a1: -1) = *(this + a2 + 428);`
  - `result = AvatarLook::`vftable'(a1: v8);`
  - `v4 = *(this + 405);`
  - `if ( v4 != 0 )`
  - `result = AvatarLook::operator=(a1: v4 + 4);`
  - `if ( *v5 > 0 )`
  - `if ( v6 / 10000 == 181 || v6 == 1822000 || v6 == 1832000 )`
  - `if ( v6 == 1812003 || v6 == 1812002 )`
  - `result = ZArray<long>::InsertBefore(a1: -1);`
  - `if ( v10 > 45 )`
  - `if ( v10 == 46 )`
  - `if ( v10 == 47 )`
  - `if ( v10 != 48 )`
  - …（截断 16 行）

#### CUIUserInfo::SetState  (506 B)

  - `void __thiscall CUIUserInfo::SetState(CUIUserInfo *this, int a2)`
  - `*(this + 408) = a2;`
  - `CUIUserInfo::SetLayer(this);`
  - `sub_8FFE3B(this);`
  - `if ( *(this + 399) != 0 )`
  - `v4 = (*(this + 408) != 0 ? 196 : 34) + (*(*(this + 1) + 48))(a1: this + 4);`
  - `v3 = (*(*(this + 1) + 44))(a1: this + 4);`
  - `sub_49254B(a1: v3 + 270, a2: v4);`
  - `CWnd::InvalidateRect(this, a2: 0);`

#### CUIUserInfo::SetWishItemInfo  (1222 B)

  - `int __thiscall CUIUserInfo::SetWishItemInfo(char *this, int *a2)`
  - `ZArray<TSecType<long>>::RemoveAll(a1: this + 1800);`
  - `if ( *a2 != 0 )`
  - `result = *(result - 4);`
  - `for ( i = result - 1; i >= 0; --i )`
  - `result = CWvsContext::GetCommodityBySN(a1: v7, a2: *(*a2 + 4 * i));`
  - `if ( v8 != 0 )`
  - `v5 = *(*a2 + 4 * i);`
  - `ZArray<TSecType<long>>::Insert(a1: -1);`
  - `result = TSecType<long>::SetData(a1: v5);`
  - `result = sub_473F9E(a1: 0);`
  - `v6 = *(v2 + 397);`
  - `if ( v6 != 0 )`
  - `return sub_8FE3FB(this: v6);`

#### CUIUserInfo::Update  (1712 B)

  - `int __thiscall CUIUserInfo::Update(CUIUserInfo *this)`
  - `CWnd::Update(this);`
  - `v3 = *(this + 405);`
  - `if ( v3 != 0 )`
  - `CAvatar::Update(this: v3, a2: v2);`
  - `v4 = *(this + 407);`
  - `if ( v4 != 0 )`
  - `CAvatar::Update(this: v4, a2: v2);`
  - `result = *(this + 361);`
  - `if ( result == 0 && *(this + 451) == 0 && *(this + 452) == 0 )`
  - `v6 = *(this + 453) - 30 < 0;`
  - `*(this + 453) -= 30;`
  - `if ( v6 )`
  - `v7 = *(this + 395);`
  - `if ( v7 != 0 )`
  - `CWnd::Destroy(a1: v7);`
  - …（截断 16 行）

### CUIUserInfoDetail（2 函数）


#### CUIUserInfoDetail::Draw  (12606 B)

- SP_ 节点：`SP_1887_REQLEVEL`, `SP_2564_UI_UIWINDOWIMG_USERINFO_REQLV`, `SP_5515_D`
  - `__int64 *__thiscall CUIUserInfoDetail::Draw(_DWORD *this, int arg0)`
  - `CWnd::Draw(this, a2: arg0);`
  - `CWnd::GetCanvas(a1: &v81);`
  - `sub_402F85(pvargDest: &v66, pvargSrc: &pvargSrc);`
  - `LOBYTE(v84) = 1;`
  - `sub_402F85(pvargDest: &v64, pvargSrc: &pvargSrc);`
  - `HIDWORD(v59) = &v66;`
  - `LODWORD(v59) = &v64;`
  - `LOBYTE(v84) = 2;`
  - `v5 = sub_406472();`
  - `v6 = sub_402680(a1: v5);`
  - `IWzResMan::GetObjectA(this: v6, pvargDest: &v62, sUOL: v58, vParam: v59, vAux: SHIDWORD(v59));`
  - `LOBYTE(v84) = 4;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v60[0], a2: SBYTE4(v60[0]));`
  - `if ( Unknown != 0 )`
  - `v7 = sub_41E527(a1: &Unknown);`
  - …（截断 16 行）

#### CUIUserInfoDetail::OnCreate  (539 B)

  - `CWnd *__fastcall CUIUserInfoDetail::OnCreate(CWnd *a1, int a2, int a3, int a4)`
  - `CWnd::CWnd(this: a1);`
  - `dword_BF1174 = (a1 + 108) != 0 ? a1 : 0;`
  - `*(a1 + 28) = 0;`
  - `*(a1 + 30) = 0;`
  - `*(a1 + 1) = &off_B3BEA0;`
  - `*(a1 + 2) = &off_B3BE9C;`
  - `CWnd::CreateWnd(`

### CUIUserInfoWishList（2 函数）


#### CUIUserInfoWishList::Draw  (11657 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_153_S_IS_AT_S`, `SP_572_D`
  - `_DWORD *__thiscall CUIUserInfoWishList::Draw(_DWORD *this, int arg0)`
  - `CWnd::Draw(this, a2: arg0);`
  - `CWnd::GetCanvas(a1: &v77);`
  - `v4 = *(v2[30] + 56);`
  - `LOBYTE(v79) = 1;`
  - `if ( v4 < v4 + 3 )`
  - `while ( 1 )`
  - `v6 = *(v3 + 1800);`
  - `if ( v6 == 0 || v4 >= *(v6 - 4) )`
  - `Data = TSecType<long>::GetData(a1: (v6 + v5), a2: 0);`
  - `CWvsContext::GetCommodityBySN(a1: v64, a2: Data);`
  - `LOBYTE(v79) = 2;`
  - `if ( v65 != 0 && TSecType<long>::GetData(a1: (v65 + 16), a2: 0) != 0 )`
  - `v8 = TSecType<long>::GetData(a1: (v65 + 16), a2: 0);`
  - `if ( v8 / 100000 == 91 )`
  - `v11 = sub_64BCCC(a1: v10, a2: v9);`
  - …（截断 16 行）

#### CUIUserInfoWishList::OnCreate  (709 B)

  - `_DWORD *__fastcall CUIUserInfoWishList::OnCreate(_DWORD *a1, int a2, int a3, int a4)`
  - `CWnd::CWnd(this: a1);`
  - `CUIToolTip::CUIToolTip(this: a1 + 37);`
  - `CWnd::CreateWnd(`

### CUIKeyConfig（7 函数）


#### CUIKeyConfig::0CNoticeDlg  (2704 B)

- SP_ 节点：`SP_2503_UI_UIWINDOWIMG_KEYCONFIG_NOTICE_D`
  - `CUIKeyConfig::CNoticeDlg *__thiscall CUIKeyConfig::CNoticeDlg::CNoticeDlg(`
  - `CDialog::CDialog(this);`
  - `*(this + 33) = 0;`
  - `*(this + 35) = 0;`
  - `CUIToolTip::CUIToolTip(this: this + 36);`
  - `*(this + 1) = &off_B39820;`
  - `*(this + 2) = &off_B3981C;`
  - `LOBYTE(v23) = 4;`
  - `LOBYTE(v23) = 5;`
  - `sub_416821(a1: &v22, a2: v5, a3);`
  - `Layer = CWnd::GetLayer(a1: &v18);`
  - `LOBYTE(v23) = 6;`
  - `if ( v7 )`
  - `v8 = CWnd::GetLayer(a1: &v19);`
  - `LOBYTE(v23) = 7;`
  - `v10 = IWzGr2DLayer::GetHeight(a1: v21) / 2;`
  - …（截断 16 行）

#### CUIKeyConfig::GetShortCutIndexByPos  (1041 B)

  - `int __thiscall CUIKeyConfig::GetShortCutIndexByPos(CUIKeyConfig *this, int a2, int a3)`
  - `while ( 1 )`
  - `v7[0] = *(v4 - 1);`
  - `if ( dword_BF0484(a1: v7, a2, a3) != 0 )`
  - `if ( v4 >= dword_BE2C6C )`
  - `if ( dword_BE27E0[2 * v8] == 0 && dword_BE27E4[2 * v8] == 0 )`
  - `switch ( v8 )`

#### CUIKeyConfig::OnButtonClicked  (1894 B)

  - `void __thiscall CUIKeyConfig::OnButtonClicked(CWnd *this, int a2)`
  - `CUIWnd::OnButtonClicked(a1: a2);`
  - `if ( a2 == 1 )`
  - `CFuncKeyMappedMan::SaveFuncKeyMap(this: dword_BED5A0);`
  - `CQuickslotKeyMappedMan::SaveQuickslotKeyMap(this: dword_BED658);`
  - `if ( a2 == 2 )`
  - `CWnd::InvalidateRect(this: TSingleton<CUIStatusBar>::ms_pInstance, a2: 0);`
  - `CWvsContext::UI_Close(this: *dword_BE7918, a2: 5);`
  - `if ( a2 == 2000 )`
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444) != 0 )`
  - `v4 = CUIKeyConfig::CNoticeDlg::CNoticeDlg(a1: this, a2: 0);`
  - `if ( CDialog::DoModal(a1: v4) == 1 )`
  - `CFuncKeyMappedMan::DefaultFuncKeyMap(this: dword_BED5A0);`
  - `CQuickslotKeyMappedMan::DefaultQuickslotKeyMap(this: dword_BED658);`
  - `CUIStatusBar::ReDrawQuickslot(this: TSingleton<CUIStatusBar>::ms_pInstance);`
  - `if ( a2 != 2001 )`
  - …（截断 16 行）

#### CUIKeyConfig::OnDestroy  (1536 B)

  - `int __thiscall CUIKeyConfig::OnDestroy(_DWORD *this)`
  - `CWvsContext::RemoveFromStackForTab(a1: this);`
  - `if ( (memcmp(a1: (dword_BED5A0 + 449), a2: (dword_BED5A0 + 4), a3: 0x1BDu) != 0`
  - `|| memcmp(a1: (v2 + 36), a2: (v2 + 4), a3: 0x20u) != 0)`
  - `v8 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `if ( v8 != 0 )`
  - `v3 = CUIKeyConfig::CNoticeDlg::CNoticeDlg(a1: v9, a2: 2);`
  - `sub_836DCA(a1: v6);`
  - `if ( CDialog::DoModal(a1: v7) == 1 )`
  - `if ( memcmp(a1: (v1 + 449), a2: (v1 + 4), a3: 0x1BDu) != 0 )`
  - `CFuncKeyMappedMan::SaveFuncKeyMap(this: v1);`
  - `if ( memcmp(a1: (v2 + 36), a2: (v2 + 4), a3: 0x20u) != 0 )`
  - `CQuickslotKeyMappedMan::SaveQuickslotKeyMap(this: v2);`
  - `return CUIWnd::OnDestroy(a1: v9);`

#### CUIKeyConfig::OnMouseButton  (245 B)

  - `void __thiscall CUIKeyConfig::OnMouseButton(CUIKeyConfig *this, unsigned int a2, unsigned int a3, int a4, int a5)`
  - `if ( a2 == 513 )`
  - `CUIKeyConfig::TryBeginDragFuncKeyMappedIcon(this: (this - 4), a2: a4, a3: a5);`

#### CUIKeyConfig::OnMouseMove  (3082 B)

  - `int __thiscall CUIKeyConfig::OnMouseMove(CUIKeyConfig *this, int a2, int a3)`
  - `ShortCutIndexByPos = CUIKeyConfig::GetShortCutIndexByPos(this: (this - 4), a2, a3);`
  - `if ( ShortCutIndexByPos > 90 || ShortCutIndexByPos == -1 )`
  - `if ( *(this + 29) != 1 )`
  - `CUIToolTip::ClearToolTip(this: (this + 112));`
  - `CInputSystem::SetCursorState(this: dword_BEC33C, a2: 5);`
  - `if ( *v5 == 2 || v6 == 7 )`
  - `CItemInfo::GetItemSlot(a1: v21, a2: *(v5 + 1));`
  - `sub_7B49F6(a1: 0);`
  - `LOBYTE(v24) = 1;`
  - `v10 = sub_428244(a1: v21);`
  - `LOBYTE(v24) = 0;`
  - `v17 = (*(v11 + 4))(a1: v10);`
  - `v16 = sub_428244(a1: v21) + 32;`
  - `v15 = *(v5 + 1);`
  - `v14 = (*(*this + 48))(a1: this) + a3 + 20;`
  - …（截断 16 行）

#### CUIKeyConfig::TryBeginDragFuncKeyMappedIcon  (30835 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1847_DRAGSTART`, `SP_2413_ICON_D`, `SP_4161_UI_UIWINDOWIMG_SKILLMACRO_MACROICON_D_ICON`
  - `wchar_t *__thiscall CUIKeyConfig::TryBeginDragFuncKeyMappedIcon(CUIKeyConfig *this, int arg0, int a3)`
  - `result = CUIKeyConfig::GetShortCutIndexByPos(this, a2: arg0, a3);`
  - `if ( result == -1 )`
  - `v157 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v151) + 4);`
  - `sub_4289B7(this: &v151);`
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
  - `LOBYTE(v165) = 4;`
  - …（截断 16 行）

### CWorldMapDlg（18 函数）


#### CWorldMapDlg::CheckLinkInfo  (4826 B)

- SP_ 节点：`SP_959_ORIGIN`
  - `int __thiscall CWorldMapDlg::CheckLinkInfo(CWorldMapDlg *this, int *a2, int a3)`
  - `v5 = sub_414576(a1: this + 1452);`
  - `Property = IWzCanvas::GetProperty(this: v5, a2: &v34);`
  - `LOBYTE(v41) = 2;`
  - `sub_40263B(a1: Property);`
  - `Item = IWzProperty::GetItem(pvargDest: &pvarg, a2: v26[0]);`
  - `LOBYTE(v41) = 3;`
  - `unknown = get_unknown(a1: v35, a2: Item);`
  - `LOBYTE(v41) = 4;`
  - `sub_41A4AF(a1: unknown);`
  - `sub_4027B1(a1: v35);`
  - `LOBYTE(v41) = 7;`
  - `if ( v9 < 0 )`
  - `LOBYTE(v41) = 6;`
  - `sub_4027E9(a1: &v34);`
  - `for ( i = 0; ; i += 12 )`
  - …（截断 16 行）

#### CWorldMapDlg::CheckSpotInfo  (5067 B)

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
  - `ZXString<char>::Format(a1: &v39, a2: v15, a3: v30);`
  - `LOBYTE(v42) = 3;`
  - …（截断 16 行）

#### CWorldMapDlg::ClearToolTip  (124 B)

  - `_DWORD *__thiscall CWorldMapDlg::ClearToolTip(CWorldMapDlg *this)`
  - `return CUIToolTip::ClearToolTip(this: (this + 132));`

#### CWorldMapDlg::CreateWorldMapDlg  (7786 B)

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
  - `LOBYTE(v57) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v40, a2: v41);`
  - `if ( Unknown != 0 )`
  - `v9 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v9 < 0 && v9 != -2147467262 )`
  - `LOBYTE(v57) = 7;`
  - `if ( v10 < 0 )`
  - …（截断 16 行）

#### CWorldMapDlg::Draw  (10777 B)

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
  - `LOBYTE(v69) = 0;`
  - `if ( v5 < 0 )`
  - `v64 = *(v2 + 364);`
  - `LOBYTE(v69) = 2;`
  - `v7 = (*(v6 + 128))(`
  - `if ( v7 < 0 )`
  - `if ( v8 < 0 )`
  - …（截断 16 行）

#### CWorldMapDlg::HitTest  (390 B)

  - `int __thiscall CWorldMapDlg::HitTest(CWorldMapDlg *this, int a2, unsigned int a3, struct CCtrlWnd **a4)`
  - `result = CWnd::HitTest(a1: a2, a2: a3, a3: a4);`
  - `if ( result != 0 && (a4 == 0 || *a4 == 0) )`
  - `if ( a3 > 0x18 )`

#### CWorldMapDlg::LoadMapList  (21758 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1445_TITLE`, `SP_1447_DESC`, `SP_1515_TYPE`, `SP_1729_STREETNAME`, `SP_1730_MAPNAME`, `SP_1803_PATH`, `SP_2291_MAPLIST`, `SP_2292_MAPNO`, `SP_2293_MAPDESC`, `SP_4038_SPOT`, `SP_959_ORIGIN`
  - `int __thiscall CWorldMapDlg::LoadMapList(_DWORD *this, int a2)`
  - `for ( i = 0; ; i += 68 )`
  - `if ( *v3 == 0 || v2 >= *(v4 - 4) )`
  - `sub_9EDD3E(a1: v4 + i);`
  - `sub_9EE882(a1: v3);`
  - `LOBYTE(v128) = 0;`
  - `sub_40263B(a1: &a2);`
  - `IWzProperty::GetItem(pvargDest: &pvarg, a2: v86);`
  - `LOBYTE(v128) = 2;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: p_pvargDest, a2: v88[0]);`
  - `if ( Unknown != 0 )`
  - `v8 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v8 < 0 && v8 != -2147467262 )`
  - `LOBYTE(v128) = 4;`
  - `if ( v9 < 0 )`
  - `if ( v125 != 0 && IWzProperty::Getcount(a1: v125) != 0 )`
  - …（截断 16 行）

#### CWorldMapDlg::MakeBorder  (26557 B)

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
  - `LOBYTE(v138) = 1;`
  - `if ( v11 < 0 )`
  - `if ( v12 < 0 )`
  - `v14 = (v137 + 26);`
  - `if ( v137[26] == 0 )`
  - `v17 = (*(v16 + 140))(a1: v15, a2: 0, a3: 0, a4: v128.decVal.Hi32, a5: Unknown, a6: 0xFFFFFF);`
  - `if ( v17 < 0 )`
  - …（截断 16 行）

#### CWorldMapDlg::OnButtonClicked  (249 B)

  - `void __thiscall CWorldMapDlg::OnButtonClicked(CWorldMapDlg *this, unsigned int a2)`
  - `if ( a2 == 1 )`
  - `(*(*this + 52))(a1: 1);`
  - `else if ( a2 == 2 )`
  - `(*(*this + 52))(a1: 2);`

#### CWorldMapDlg::OnCreate  (4054 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1240_WORLDMAPOPEN`, `SP_2289_UI_UIWINDOW_WORLDMAP_TITLE`, `SP_5577_UI_BASICIMG_BTCLOSE`
  - `void __thiscall CWorldMapDlg::OnCreate(CWorldMapDlg *this, void *a2)`
  - `CWorldMapDlg::MakeBorder(this);`
  - `sub_9ECC4A(a1: 12, a2: 34, a3: 642, a4: 472);`
  - `LOBYTE(v28) = 1;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `LOBYTE(v28) = 0;`
  - `v5 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v28) = 2;`
  - `if ( v5 != 0 )`
  - `v6 = CCtrlButton::CCtrlButton(this: v5);`
  - `sub_4284FF(a1: v6);`
  - `(*(**(this + 33) + 32))(a1: *(this + 33), a2: this, a3: 2, a4: *(this + 9) - 20, a5: 6, a6: 0, a7: v25);`
  - `VariantInit(&pvarg);`
  - `v7 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v7 < 0 )`
  - `LOBYTE(v28) = 3;`
  - …（截断 16 行）

#### CWorldMapDlg::OnDestroy  (528 B)

- SP_ 节点：`SP_1241_WORLDMAPCLOSE`
  - `void __thiscall CWorldMapDlg::OnDestroy(CWorldMapDlg *this)`
  - `play_ui_sound(a1: v2);`

#### CWorldMapDlg::OnKey  (731 B)

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

#### CWorldMapDlg::OnMouseButton  (419 B)

  - `void __thiscall CWorldMapDlg::OnMouseButton(CWorldMapDlg *this, unsigned int a2, unsigned int a3, int a4, int a5)`
  - `if ( a2 == 514 )`
  - `CWorldMapDlg::SetWorldMapDeeper(this: (this - 4));`
  - `else if ( a2 == 517 )`
  - `CWorldMapDlg::SetWorldMapShallower(this: (this - 4));`
  - `CUIToolTip::ClearToolTip(this: (this + 132));`

#### CWorldMapDlg::OnMouseEnter  (285 B)

  - `_DWORD *__thiscall CWorldMapDlg::OnMouseEnter(CWorldMapDlg *this, int a2)`
  - `result = CWnd::OnMouseEnter(a1: a2);`
  - `if ( a2 == 0 )`
  - `return CUIToolTip::ClearToolTip(this: (this + 132));`

#### CWorldMapDlg::OnMouseMove  (385 B)

  - `int __thiscall CWorldMapDlg::OnMouseMove(CWorldMapDlg *this, int a2, int a3)`
  - `v3 = (this - 4);`
  - `v4 = CWorldMapDlg::CheckSpotInfo(this: (this - 4), a2, a3);`
  - `if ( (v4 | CWorldMapDlg::CheckLinkInfo(this: v3, a2, a3)) != 0 )`
  - `CWnd::InvalidateRect(this: v3, a2: 0);`

#### CWorldMapDlg::SetWorldMap  (25867 B)

- SP_ 节点：`SP_2290_BASEIMG_0`
  - `int __fastcall CWorldMapDlg::SetWorldMap(int a1, int a2, int a3)`
  - `HIDWORD(v90) = a1;`
  - `LOBYTE(v128) = 0;`
  - `sub_40263B(a1: &a3);`
  - `IWzProperty::GetItem(pvargDest: &v103, a2: SHIDWORD(v90));`
  - `LOBYTE(v128) = 2;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v91[0], a2: SBYTE4(v91[0]));`
  - `v5 = sub_41E527(a1: &Unknown);`
  - `if ( v5 < 0 && v5 != -2147467262 )`
  - `if ( v6 < 0 )`
  - `if ( *(v3 + 1452) != 0 )`
  - `HIDWORD(v91[0]) = a3;`
  - `sub_415691(a1: v91 + 4);`
  - `if ( sub_9ECE30(a1: HIDWORD(v91[0])) != 0 )`
  - `if ( CWorldMapDlg::LoadMapList(a1: HIDWORD(v91[0])) != 0 )`
  - `if ( sub_9ECF85(a1: HIDWORD(v91[0])) != 0 )`
  - …（截断 16 行）

#### CWorldMapDlg::SetWorldMapDeeper  (4526 B)

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
  - `LOBYTE(v33) = 1;`
  - `VariantInit(&pvarg);`
  - `v7 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v7 < 0 )`
  - `LOBYTE(v33) = 3;`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v33) = 4;`
  - …（截断 16 行）

#### CWorldMapDlg::SetWorldMapShallower  (4230 B)

- SP_ 节点：`SP_1242_SELECTMAP`, `SP_2276_MAP_WORLDMAP_SIMG`
  - `void __thiscall CWorldMapDlg::SetWorldMapShallower(CWorldMapDlg *this)`
  - `v2 = *(this + 31);`
  - `if ( v2 != 0 && *v2 != 0 )`
  - `v20 = *(this + 31);`
  - `LOBYTE(v31) = 2;`
  - `sub_416821(a1: &v28, a2: v19, a3: v20);`
  - `LOBYTE(v31) = 1;`
  - `VariantInit(&pvarg);`
  - `v5 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v5 < 0 )`
  - `LOBYTE(v31) = 3;`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v31) = 4;`
  - `v6 = sub_41468C(a1: &v28);`
  - `_bstr_t::_bstr_t(a1: v6);`
  - `LOBYTE(v31) = 5;`
  - …（截断 16 行）

### CUIItemMaker（23 函数）


#### CUIItemMaker::ClearItemSlot  (998 B)

  - `int __thiscall CUIItemMaker::ClearItemSlot(CUIItemMaker *this)`
  - `*(this + 364) = 0;`
  - `sub_4051E5(a1: 0);`
  - `*(this + 371) = 0;`
  - `if ( *(this + 373) != 0 )`
  - `sub_5E8CCB(a1: 0);`
  - `*(this + 373) = 0;`
  - `for ( i = 6; i != 0; --i )`
  - `sub_82497F(a1: v2);`
  - `*(this + 449) = 1;`
  - `*(this + 450) = 0;`
  - `*(this + 451) = 0;`
  - `for ( j = 4; j != 0; --j )`
  - `sub_8249B1(a1: v4);`
  - `*(this + 484) = 0;`
  - `*(this + 485) = 0;`
  - …（截断 16 行）

#### CUIItemMaker::ConfirmMake  (3599 B)

- SP_ 节点：`SP_756__WOULD_YOU_LIKE_TO_MAKE_THIS`, `SP_757_WOULD_YOU_LIKE_TO_MAKE_A_MONSTER_CRYSTAL`, `SP_758_WOULD_YOU_LIKE_TO_DISASSEMBLE`, `SP_759_WOULD_YOU_LIKE_TO_MAKE_`
  - `int __thiscall CUIItemMaker::ConfirmMake(CUIItemMaker *this)`
  - `v2 = *(this + 449) - 1;`
  - `if ( v2 != 0 )`
  - `if ( v3 != 0 )`
  - `if ( v4 != 0 )`
  - `if ( v4 != 1 )`
  - `LOBYTE(v30) = 5;`
  - `ZXString<char>::operator=(a1: String);`
  - `LOBYTE(v30) = 0;`
  - `LOBYTE(v30) = 4;`
  - `ZXString<char>::operator=(a1: v9);`
  - `LOBYTE(v30) = 3;`
  - `ZXString<char>::operator=(a1: v11);`
  - `ItemName = CItemInfo::GetItemName(a1: v25, a2: *(this + 371));`
  - `LOBYTE(v30) = 1;`
  - `ZXString<char>::operator=(a1: ItemName);`
  - …（截断 16 行）

#### CUIItemMaker::DoesSatisfyPreCondition  (2799 B)

  - `signed int __stdcall CUIItemMaker::DoesSatisfyPreCondition(char a1)`
  - `CItemMakerInfo::GetItemMakeInfo(a1: v17, a2: a1);`
  - `if ( v18 == 0`
  - `|| ((v1 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v16) + 4),`
  - `sub_4289B7(this: v16),`
  - `(v2 = _ZtlSecureFuse<short>(a1: v1 + 57, a2: *(v1 + 61))) / 100 != 22)`
  - `? (v3 = 1000 * (v2 / 1000))`
  - `: (v3 = 2001),`
  - `(SkillLevel = CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v1, a3: 10000 * v3 + 1007, a4: 0),`
  - `(v6 = v18[3]) != 0)`
  - `sub_5E8CB5(a1: v17);`
  - `if ( v18[2] != 0 )`
  - `v7 = _ZtlSecureFuse<unsigned char>(a1: v1 + 51, a2: *(v1 + 53));`
  - `if ( v7 < v18[2] )`
  - `if ( v9 != 0 )`
  - `if ( CWvsContext::GetItemCount(a1: v9) <= 0 )`
  - …（截断 16 行）

#### CUIItemMaker::Draw  (1639 B)

  - `VARIANTARG *__thiscall CUIItemMaker::Draw(_DWORD *this, VARIANTARG *pRect)`
  - `CWnd::Draw(this, a2: pRect);`
  - `CWnd::GetCanvas(a1: &pRect);`
  - `if ( pRect != 0 )`
  - `(*(*&pRect->vt + 4))(a1: pRect);`
  - `sub_824D81(this, a2: v4[0]);`
  - `CUIItemMaker::DrawRecipe(this, a2: v4[0]);`
  - `CUIItemMaker::DrawGem(this, a2: v4[0]);`
  - `sub_8259A2(this, a2: v4[0]);`
  - `CUIItemMaker::DrawMeso(this, a2: v4[0]);`
  - `sub_825B50(this, a2: v4[0]);`
  - `return (*(*&pRect->vt + 8))(a1: pRect);`

#### CUIItemMaker::DrawGem  (3517 B)

  - `int __thiscall CUIItemMaker::DrawGem(_DWORD *this, int a2)`
  - `v3 = *(this + 484) <= 0;`
  - `if ( !v3 )`
  - `if ( *v4 != 0 && *(v4 - 5) != 0 )`
  - `v5 = sub_40B920(a1: *v4);`
  - `v16 = (37 - v5) / 2;`
  - `if ( v6 )`
  - `v17 = (37 - sub_40B947(a1: *v4)) / 2;`
  - `sub_402FAB(a1: 255, a2: 3);`
  - `LOBYTE(v22) = 1;`
  - `if ( a2 == 0 )`
  - `v8 = (*(*a2 + 128))(`
  - `if ( v8 < 0 )`
  - `LOBYTE(v22) = 0;`
  - `if ( v9 < 0 )`
  - `while ( v19 < v2[484] );`
  - …（截断 16 行）

#### CUIItemMaker::DrawMeso  (3822 B)

  - `VARIANTARG *__thiscall CUIItemMaker::DrawMeso(_DWORD *this, VARIANTARG *a2)`
  - `v3 = *(this + 449);`
  - `if ( v3 != 1 && v3 != 4 && v3 != 3 )`
  - `LOBYTE(TotalCostToDisassemble) = 0;`
  - `if ( v3 == 1 )`
  - `v5 = *(this + 373);`
  - `if ( v5 != 0 )`
  - `TotalCostToDisassemble = CUIItemMaker::GetTotalCostToMake(this, a2: *(v5 + 32));`
  - `if ( v3 == 4 )`
  - `TotalCostToDisassemble = CUIItemMaker::GetTotalCostToDisassemble(this);`
  - `if ( TotalCostToDisassemble < 0 )`
  - `LOBYTE(v28) = 1;`
  - `format_integer(a1: &v27, a2: TotalCostToDisassemble, a3: 1);`
  - `VariantInit(&pvarg);`
  - `v6 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v6 < 0 )`
  - …（截断 16 行）

#### CUIItemMaker::DrawRecipe  (10460 B)

  - `VARIANTARG *__thiscall CUIItemMaker::DrawRecipe(_DWORD *this, VARIANTARG *a2)`
  - `v3 = *(this + 450) <= 0;`
  - `if ( !v3 )`
  - `v4 = (this + 389);`
  - `v61 = (this + 389);`
  - `if ( *v4 != 0 && *(v4 - 5) != 0 )`
  - `v5 = sub_40B920(a1: *v4);`
  - `v59 = (37 - v5) / 2;`
  - `if ( v6 )`
  - `v55 = (37 - sub_40B947(a1: *v4)) / 2;`
  - `sub_402FAB(a1: 255, a2: 3);`
  - `LOBYTE(v64) = 1;`
  - `if ( a2 == 0 )`
  - `v8 = 93 * (i % 3);`
  - `v9 = (*(v7 + 128))(`
  - `if ( v9 < 0 )`
  - …（截断 16 行）

#### CUIItemMaker::GetTotalCostToDisassemble  (1771 B)

  - `int __thiscall CUIItemMaker::GetTotalCostToDisassemble(CUIItemMaker *this)`
  - `v13 = *(this + 384);`
  - `if ( v13 != 0 )`
  - `InterlockedIncrement(lpAddend: (v13 + 4));`
  - `if ( v1 == 0 )`
  - `if ( !v2 )`
  - `ZRef<GW_ItemSlotBase>::~ZRef<GW_ItemSlotBase>(a1: 0);`
  - `Data = TSecType<long>::GetData(a1: (v1 + 12), a2: 0);`
  - `CItemMakerInfo::GetItemMakeInfo(a1: v10, a2: Data);`
  - `LOBYTE(v15) = 1;`
  - `if ( v11 == 0 )`
  - `v9 = *(v11 + 32);`
  - `sub_4E7ECC(a1: v12);`
  - `sub_4283F8();`
  - `v6 = sub_5CA659(a1: v7, a2: v8, a3: v9);`
  - `LOBYTE(v15) = 0;`
  - …（截断 16 行）

#### CUIItemMaker::GetTotalCostToMake  (1541 B)

  - `int __thiscall CUIItemMaker::GetTotalCostToMake(CUIItemMaker *this, int a2)`
  - `v3 = *(this + 494);`
  - `v5 = *(this + 484) <= 0;`
  - `if ( !v5 )`
  - `if ( *v14 != 0 )`
  - `v4 += TSecType<long>::GetData(a1: (*v14 + 12), a2: v4) % 10 + 1;`
  - `while ( v13 < *(this + 484) );`
  - `v6 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v12) + 4);`
  - `if ( v13 != 0 )`
  - `sub_428C44(this: &v12, a2: 0);`
  - `v13 = _ZtlSecureFuse<short>(a1: v6 + 57, a2: *(v6 + 61));`
  - `if ( v13 / 100 == 22 || v13 == 2001 )`
  - `v7 = 1000 * (v13 / 1000);`
  - `CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v6, a3: 10000 * v7 + 1007, a4: 0);`
  - `v8 = a2 / 10 * ((v3 != 0 ? 5 : 0) + 3 * v4 + 1);`
  - `if ( v8 <= 0 )`
  - …（截断 16 行）

#### CUIItemMaker::IsAbleToMake  (1025 B)

- 分支 case：`2`, `3`, `4`
  - `BOOL __thiscall CUIItemMaker::IsAbleToMake(_DWORD *this)`
  - `v2 = *(this + 371);`
  - `if ( v2 != 0 )`
  - `if ( *(this + 449) != 1 )`
  - `switch ( *(this + 449) )`
  - `return *(this + 451) > 0;`
  - `return *(this + 392) >= 100;`
  - `return *(this + 373) != 0;`
  - `if ( CUIItemMaker::DoesSatisfyPreCondition(a1: v2) != 0 )`
  - `v4 = *(this + 450);`
  - `if ( v4 <= 0 )`
  - `for ( i = this + 393; *i != 0; i += 11 )`
  - `if ( ++v5 >= v4 )`

#### CUIItemMaker::IsEnoughMeso  (2467 B)

- SP_ 节点：`SP_5599_YOU_DO_NOT_HAVE_ENOUGH_MESOS`, `SP_753_PRODUCTION_FAILED`
  - `signed int __thiscall CUIItemMaker::IsEnoughMeso(CUIItemMaker *this)`
  - `if ( *(this + 449) == 3 )`
  - `v2 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v18) + 4);`
  - `if ( v19 != 0 )`
  - `sub_428C44(this: &v18, a2: 0);`
  - `v3 = *(this + 373);`
  - `if ( v3 == 0 )`
  - `v4 = *(this + 449);`
  - `v5 = *(v3 + 32);`
  - `if ( v4 > 0 )`
  - `if ( v4 <= 2 )`
  - `TotalCostToDisassemble = CUIItemMaker::GetTotalCostToMake(this, a2: *(v3 + 32));`
  - `if ( v4 != 4 )`
  - `TotalCostToDisassemble = CUIItemMaker::GetTotalCostToDisassemble(this);`
  - `v7 = _ZtlSecureFuse<long>(a1: v2 + 165, a2: *(v2 + 173));`
  - `if ( v7 < v5 )`
  - …（截断 16 行）

#### CUIItemMaker::IsExistEmptySlot  (2102 B)

- 分支 case：`1`, `2`, `3`, `4`
  - `signed int __thiscall CUIItemMaker::IsExistEmptySlot(CUIItemMaker *this)`
  - `v2 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v11) + 4);`
  - `if ( v12 != 0 )`
  - `sub_428C44(this: &v11, a2: 0);`
  - `v3 = *(this + 449);`
  - `switch ( v3 )`
  - `v6 = *(this + 373);`
  - `if ( v6 == 0 )`
  - `if ( *(v6 + 84) != 0 )`
  - `v7 = *(v6 + 88);`
  - `if ( v7 != 0 )`
  - `while ( 1 )`
  - `v8 = *(v7 != 0 ? v7 - 3 : 4);`
  - `v7 = v8 != 0 ? (v8 + 16) : 0;`
  - `if ( sub_4F1176(this: v14, a2: v10 / 1000000) <= 0 )`
  - `if ( v7 == 0 )`
  - …（截断 16 行）

#### CUIItemMaker::IsExistMakableItem  (2171 B)

- SP_ 节点：`SP_753_PRODUCTION_FAILED`
  - `signed int __thiscall CUIItemMaker::IsExistMakableItem(_DWORD *this)`
  - `if ( *(this + 449) != 2 )`
  - `v2 = *(this + 381);`
  - `if ( v2 == 0 )`
  - `CUtilDlg::Notice(sMsg: v8, sSndName: v9, ppDialog: v10, bAutoSeparated: v11, bIsTightLine: v12);`
  - `while ( 1 )`
  - `v4 = *(v2 != 0 ? v2 - 3 : 4);`
  - `v2 = v4 != 0 ? (v4 + 16) : 0;`
  - `CItemMakerInfo::GetItemMakeInfo(a1: v15, a2: v3);`
  - `if ( CUIItemMaker::IsMatched_WithCurrentSetting(this, a2: v3) != 0 )`
  - `if ( v16 != 0 )`
  - `sub_5E8CCB(a1: 0);`
  - `LOBYTE(v12) = v3;`
  - `*(this + 371) = v3;`
  - `Info = CItemMakerInfo::GetItemMakeInfo(a1: v13, a2: v12);`
  - `LOBYTE(v18) = 1;`
  - …（截断 16 行）

#### CUIItemMaker::IsMatched_WithCurrentSetting  (3058 B)

  - `signed int __thiscall CUIItemMaker::IsMatched_WithCurrentSetting(_DWORD *this, int a2)`
  - `CItemMakerInfo::GetItemMakeInfo(a1: v20, a2);`
  - `if ( v21 == 0 || CUIItemMaker::DoesSatisfyPreCondition(a1: a2) == 0 )`
  - `sub_5E8CB5(a1: v20);`
  - `while ( v3 != 0 )`
  - `v4 = *(v3 != 0 ? v3 - 3 : 4);`
  - `v3 = v4 != 0 ? (v4 + 16) : 0;`
  - `if ( sub_822FA3(a1: v16, a2: v17) == 0 )`
  - `if ( *(this + 494) != 0 )`
  - `v6 = *(this + 487);`
  - `if ( v6 != 0 )`
  - `if ( v7[7] != TSecType<long>::GetData(a1: (v6 + 12), a2: 0) )`
  - `v8 = *(this + 485);`
  - `if ( v8 <= v21[6] )`
  - `if ( v8 > 0 )`
  - `v10 = *get_weapon_category_name(a1: &a2, nItemID: a2) == 0;`
  - …（截断 16 行）

#### CUIItemMaker::OnButtonClicked  (1205 B)

  - `void __thiscall CUIItemMaker::OnButtonClicked(signed int *this, int a2)`
  - `if ( a2 == 20000 )`
  - `if ( *(this + 364) == 1 && CUIItemMaker::ConfirmMake(a1: this) != 0 )`
  - `CUIItemMaker::StartItemMake(this);`
  - `CUIWnd::OnButtonClicked(a1: a2);`
  - `LOBYTE(v7) = 0;`
  - `if ( a2 != 20001 )`
  - `v3 = *(this + 850);`
  - `if ( v3 != 0 )`
  - `sub_989654(a1: v3);`
  - `CWvsContext::UI_Close(this: *dword_BE7918, a2: *(this + 354));`

#### CUIItemMaker::OnCreate  (12136 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1410_CANVASFONT`, `SP_5527_ARIAL`
  - `void __thiscall CUIItemMaker::OnCreate(CUIItemMaker *this, VARIANTARG **Unknown)`
  - `sub_41C06C(a1: &word_AF0E08, a2: -1);`
  - `CUIWnd::OnCreate(a1: Unknown, a2: v55);`
  - `sub_4B620C(a1: v56);`
  - `v3 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 268);`
  - `LOBYTE(v65) = 1;`
  - `if ( v3 != 0 )`
  - `v4 = sub_4C4259(a1: v3);`
  - `LOBYTE(v65) = 0;`
  - `v5 = sub_4BCDBE(this: this + 836, a2: v4);`
  - `(*(*v5[1] + 32))(a1: v5[1], a2: this, a3: 30000, a4: 3, a5: 77, a6: 71, a7: 78, a8: 18, a9: v56);`
  - `v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 268);`
  - `LOBYTE(v65) = 2;`
  - `if ( v6 != 0 )`
  - `v7 = sub_4C4259(a1: v6);`
  - `v8 = sub_4BCDBE(this: this + 838, a2: v7);`
  - …（截断 16 行）

#### CUIItemMaker::OnMouseMove  (277 B)

  - `int __thiscall CUIItemMaker::OnMouseMove(CUIItemMaker *this, LONG a2, LONG a3)`
  - `CUIItemMaker::SetToolTip(this: (this - 4), a2: &v4);`

#### CUIItemMaker::RequestItemMake  (3386 B)

- SP_ 节点：`SP_753_PRODUCTION_FAILED`
  - `signed int __thiscall CUIItemMaker::RequestItemMake(_DWORD *this)`
  - `if ( *(this + 364) != 3`
  - `|| *(this + 449) == 2 && CUIItemMaker::IsExistMakableItem(this) == 0`
  - `|| CUIItemMaker::IsExistEmptySlot(this) == 0`
  - `|| CUIItemMaker::IsEnoughMeso(this) == 0 )`
  - `if ( !CUIItemMaker::IsAbleToMake(this) )`
  - `CUtilDlg::Notice(sMsg: v10, sSndName: v11, ppDialog: v12, bAutoSeparated: v13, bIsTightLine: v14);`
  - `if ( *(this + 449) == 3 && *(this + 384) == 0 )`
  - `COutPacket::COutPacket(a1: 113);`
  - `v5 = *(this + 449);`
  - `if ( v5 > 0 )`
  - `if ( v5 <= 2 )`
  - `COutPacket::Encode4(this: &v15, a2: v5);`
  - `COutPacket::Encode4(this: &v15, a2: *(this + 371));`
  - `COutPacket::Encode1(this: &v15, a2: *(this + 1976));`
  - `COutPacket::Encode4(this: &v15, a2: *(this + 485));`
  - …（截断 16 行）

#### CUIItemMaker::SetComboBox_Category  (6960 B)

- SP_ 节点：`SP_11_SETUP`, `SP_5533_CATEGORY`, `SP_5538_ETC`, `SP_5596_USE`, `SP_598_STRENGTHENING_GEM`, `SP_599_MONSTER_CRYSTAL`, `SP_600_DISASSEMBLE_EQUIPMENT`
  - `int __thiscall CUIItemMaker::SetComboBox_Category(char **this)`
  - `sub_8232BF(a1: v43);`
  - `v4 = *(this + 837);`
  - `LOBYTE(v51) = 1;`
  - `CCtrlComboBox::AddItem(this: v4, a2: v35, a3: 0);`
  - `LOBYTE(v51) = 0;`
  - `if ( v48 != 0 )`
  - `v7 = *(this + 837);`
  - `LOBYTE(v51) = 2;`
  - `CCtrlComboBox::AddItem(this: v7, a2: v36, a3: 998);`
  - `v10 = *(this + 837);`
  - `LOBYTE(v51) = 3;`
  - `CCtrlComboBox::AddItem(this: v10, a2: v37, a3: 426);`
  - `v13 = *(this + 837);`
  - `LOBYTE(v51) = 4;`
  - `CCtrlComboBox::AddItem(this: v13, a2: v38, a3: 425);`
  - …（截断 16 行）

#### CUIItemMaker::SetItemSlotRect  (1108 B)

  - `int __thiscall CUIItemMaker::SetItemSlotRect(CUIItemMaker *this)`
  - `dword_BF040C(a1: this + 1496, a2: 15, a3: 39, a4: 67, a5: 91);`
  - `dword_BF040C(`
  - `a2: 93 * (v2 % 3) + 19,`
  - `a4: 93 * (v2 % 3) + 56,`
  - `while ( v2 < 6 );`
  - `dword_BF040C(a1: v6, a2: v4, a3: 269, a4: v4 + 37, a5: 306);`
  - `while ( v4 < 308 );`
  - `dword_BF040C(a1: this + 1952, a2: 19, a3: 269, a4: 56, a5: 306);`
  - `dword_BF040C(a1: this + 3288, a2: 11, a3: 124, a4: 300, a5: 220);`
  - `dword_BF040C(a1: this + 3304, a2: 11, a3: 250, a4: 80, a5: 313);`
  - `return dword_BF040C(a1: this + 3320, a2: 80, a3: 250, a4: 304, a5: 313);`

#### CUIItemMaker::SetToolTip  (3976 B)

- SP_ 节点：`SP_760_APPLY_THE_DROP_ITEMS_HERE_R_N_TO_MAKE_THE_MONSTER_CRYSTAL`, `SP_761_APPLY_THE_EQUIPMENT_TO_BE_DISASSEMBLED_HERE_R_N_TO_ACQUIRE_THE_MONSTER_CRYSTAL`
  - `void __thiscall CUIItemMaker::SetToolTip(CUIItemMaker *this, const struct tagPOINT *a2)`
  - `v13 = (*(*(this + 1) + 44))(a1: this + 4) + a2->x + 20;`
  - `v14 = (*(*v3 + 48))(a1: v3) + a2->y + 20;`
  - `v20 = (this + 1988);`
  - `CUIToolTip::ClearToolTip(this: (this + 1988));`
  - `CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v15);`
  - `if ( v16 != 0 )`
  - `sub_428C44(this: &v15, a2: 0);`
  - `if ( dword_BF0484(a1: this + 1496, a2: a2->x, a3: a2->y) == 0 || *(this + 371) == 0 || *(this + 449) != 1 )`
  - `if ( dword_BF0484(a1: v18 + 4, a2: a2->x, a3: a2->y) != 0 && *v18 != 0 )`
  - `CUIToolTip::ShowItemToolTip(`
  - `a4: *(this + 11 * v16 + 384),`
  - `while ( v16 < 6 );`
  - `if ( dword_BF0484(a1: v16 + 4, a2: a2->x, a3: a2->y) != 0 && *v16 != 0 )`
  - `a4: *(this + 8 * v18 + 453),`
  - `while ( v18 < 4 );`
  - …（截断 16 行）

#### CUIItemMaker::StartItemMake  (918 B)

  - `int __thiscall CUIItemMaker::StartItemMake(CUIItemMaker *this)`
  - `if ( *(this + 364) != 1 )`
  - `v2 = *(this + 449);`
  - `if ( v2 == 1 )`
  - `if ( v2 != 2 )`
  - `if ( v2 <= 2 || v2 > 4 )`
  - `if ( CUIItemMaker::IsAbleToMake(this)`
  - `&& CUIItemMaker::IsExistEmptySlot(this) != 0`
  - `&& CUIItemMaker::IsEnoughMeso(this) != 0 )`
  - `if ( *(this + 451) <= 0 )`
  - `(*(*(*(this + 841) + 4) + 28))(a1: *(this + 841) + 4, a2: 0);`
  - `*(this + 834) = get_update_time();`
  - `*(this + 364) = 2;`
  - `*(this + 850) = sub_9895EB(a1: str_It);`

#### CUIItemMaker::Update  (944 B)

  - `void __thiscall CUIItemMaker::Update(CWnd *this)`
  - `CWnd::Update(this);`
  - `v2 = *(this + 364);`
  - `if ( v2 >= 0 )`
  - `if ( v2 <= 1 )`
  - `v3 = !CUIItemMaker::IsAbleToMake(this);`
  - `v4 = *(*(this + 841) + 4);`
  - `if ( v3 )`
  - `(*(v4 + 28))(a1: 0);`
  - `*(this + 364) = 0;`
  - `(*(v4 + 28))(a1: 1);`
  - `*(this + 364) = 1;`
  - `else if ( v2 == 2 && get_update_time() - *(this + 834) >= 5100 )`
  - `*(this + 364) = 3;`
  - `if ( CUIItemMaker::RequestItemMake(this) == 0 )`
  - `sub_824A90(a1: this);`
  - …（截断 16 行）

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

### CFadeWnd（4 函数）


#### CFadeWnd::Close  (151 B)

  - `int __thiscall CFadeWnd::Close(CFadeWnd *this, int a2)`
  - `*(this + 46) = 1;`
  - `*(this + 48) = a2;`

#### CFadeWnd::CreateFadeWnd  (2904 B)

  - `int __thiscall CFadeWnd::CreateFadeWnd(int *this, int a2, int a3, wchar_t *a4, int a5, int a6, VARIANTARG *a7)`
  - `if ( a4 != 0 )`
  - `_bstr_t::_bstr_t(a1: a4);`
  - `CWnd::SetBackgrnd(this, a2: v17, a3: v18, a4: v19);`
  - `CWnd::CreateWnd(`
  - `nLeft: *(this + 37),`
  - `nTop: *(this + 38),`
  - `VariantInit(&pvarg);`
  - `v8 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v8 < 0 )`
  - `VariantInit(pvarg: &pvargDest);`
  - `v9 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v9 < 0 )`
  - `LOBYTE(v24) = 1;`
  - `v10 = *CWnd::GetLayer(a1: &a7) == 0;`
  - `LOBYTE(v24) = 2;`
  - …（截断 16 行）

#### CFadeWnd::SendCloseMessage  (4281 B)

- 分支 case：`5`, `6`, `8`
  - `void __thiscall CFadeWnd::SendCloseMessage(CFadeWnd *this)`
  - `v2 = *(this + 49);`
  - `if ( v2 != 0 )`
  - `if ( v2 == 1 )`
  - `v11[0] = *(this + 52);`
  - `get_field();`
  - `sub_5311C1(a1: v11[0]);`
  - `if ( v2 <= 1 )`
  - `if ( v2 <= 3 )`
  - `CMiniRoomBaseDlg::SendInviteResult(a1: *(this + 51), a2: 3);`
  - `switch ( v2 )`
  - `COutPacket::COutPacket(a1: 125);`
  - `COutPacket::Encode1(this: &v14, a2: 23);`
  - `ZXString<char>::operator=(a1: this + 200);`
  - `COutPacket::EncodeStr(a1: v11[0]);`
  - `v7 = sub_42861D();`
  - …（截断 16 行）

#### CFadeWnd::SetOption  (581 B)

  - `int __thiscall CFadeWnd::SetOption(`
  - `*(this + 31) = a2;`
  - `*(this + 32) = a3;`
  - `*(this + 33) = a4;`
  - `*(this + 148) = a5;`
  - `*(this + 156) = a6;`
  - `*(this + 164) = a7;`
  - `*(this + 34) = a8;`
  - `*(this + 35) = a9;`
  - `*(this + 36) = a10;`

### CUIFadeYesNo（19 函数）


#### CUIFadeYesNo::CreateAllianceInvite  (3926 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1235_INVITE`, `SP_1291_UI_UIWINDOWIMG_FADEYESNO_ICON5`, `SP_1295_UI_UIWINDOWIMG_FADEYESNO_BACKGRND`
  - `void __thiscall CUIFadeYesNo::CreateAllianceInvite(CFadeWnd *this, char a2, int Unknown)`
  - `*(this + 49) = 6;`
  - `ZXString<char>::operator=(a1: v20);`
  - `*(this + 55) = Unknown;`
  - `VariantInit(&pvarg);`
  - `v4 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v4 < 0 )`
  - `LOBYTE(v25) = 1;`
  - `VariantInit(pvarg: &pvargDest);`
  - `v5 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v5 < 0 )`
  - `LOBYTE(v25) = 2;`
  - `v8 = sub_406472();`
  - `v9 = sub_402680(a1: v8);`
  - `IWzResMan::GetObjectA(this: v9, pvargDest: &v21, sUOL: v17.x, vParam: v17.y, vAux: p_pvarg);`
  - `LOBYTE(v25) = 4;`
  - …（截断 16 行）

#### CUIFadeYesNo::CreateFamilyInvite  (3925 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1235_INVITE`, `SP_1294_UI_UIWINDOWIMG_FADEYESNO_ICON8`, `SP_1295_UI_UIWINDOWIMG_FADEYESNO_BACKGRND`
  - `void __thiscall CUIFadeYesNo::CreateFamilyInvite(CFadeWnd *this, char a2, int Unknown)`
  - `*(this + 49) = 12;`
  - `ZXString<char>::operator=(a1: v20);`
  - `*(this + 55) = Unknown;`
  - `VariantInit(&pvarg);`
  - `v4 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v4 < 0 )`
  - `LOBYTE(v25) = 1;`
  - `VariantInit(pvarg: &pvargDest);`
  - `v5 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v5 < 0 )`
  - `LOBYTE(v25) = 2;`
  - `v8 = sub_406472();`
  - `v9 = sub_402680(a1: v8);`
  - `IWzResMan::GetObjectA(this: v9, pvargDest: &v21, sUOL: v17.x, vParam: v17.y, vAux: p_pvarg);`
  - `LOBYTE(v25) = 4;`
  - …（截断 16 行）

#### CUIFadeYesNo::CreateFriendReg  (3921 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1235_INVITE`, `SP_1287_UI_UIWINDOWIMG_FADEYESNO_ICON1`, `SP_1295_UI_UIWINDOWIMG_FADEYESNO_BACKGRND`
  - `void __thiscall CUIFadeYesNo::CreateFriendReg(CFadeWnd *this, char a2, int Unknown)`
  - `*(this + 49) = 1;`
  - `ZXString<char>::operator=(a1: v20);`
  - `*(this + 52) = Unknown;`
  - `VariantInit(&pvarg);`
  - `v4 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v4 < 0 )`
  - `LOBYTE(v25) = 1;`
  - `VariantInit(pvarg: &pvargDest);`
  - `v5 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v5 < 0 )`
  - `LOBYTE(v25) = 2;`
  - `v8 = sub_406472();`
  - `v9 = sub_402680(a1: v8);`
  - `IWzResMan::GetObjectA(this: v9, pvargDest: &v21, sUOL: v17.x, vParam: v17.y, vAux: p_pvarg);`
  - `LOBYTE(v25) = 4;`
  - …（截断 16 行）

#### CUIFadeYesNo::CreateGuildInvite  (3933 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1235_INVITE`, `SP_1291_UI_UIWINDOWIMG_FADEYESNO_ICON5`, `SP_1295_UI_UIWINDOWIMG_FADEYESNO_BACKGRND`
  - `void __thiscall CUIFadeYesNo::CreateGuildInvite(CFadeWnd *this, char a2, int Unknown, struct tagPOINT *a4)`
  - `*(this + 49) = 8;`
  - `ZXString<char>::operator=(a1: v21);`
  - `*(this + 55) = Unknown;`
  - `*(this + 57) = a4;`
  - `VariantInit(&pvarg);`
  - `v5 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v5 < 0 )`
  - `LOBYTE(v25) = 1;`
  - `VariantInit(pvarg: &pvargDest);`
  - `v6 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v6 < 0 )`
  - `LOBYTE(v25) = 2;`
  - `v9 = sub_406472();`
  - `v10 = sub_402680(a1: v9);`
  - `IWzResMan::GetObjectA(this: v10, pvargDest: &v22, sUOL: v18.x, vParam: v18.y, vAux: p_pvarg);`
  - …（截断 16 行）

#### CUIFadeYesNo::CreateMSMInvite  (3869 B)

- SP_ 节点：`SP_1235_INVITE`, `SP_1286_UI_UIWINDOWIMG_FADEYESNO_ICON0`, `SP_1295_UI_UIWINDOWIMG_FADEYESNO_BACKGRND`
  - `void __thiscall CUIFadeYesNo::CreateMSMInvite(CFadeWnd *this, char a2, int Unknown, struct tagPOINT *a4)`
  - `*(this + 49) = 0;`
  - `ZXString<char>::operator=(a1: v21);`
  - `*(this + 53) = Unknown;`
  - `*(this + 51) = a4;`
  - `VariantInit(&pvarg);`
  - `v5 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v5 < 0 )`
  - `LOBYTE(v25) = 1;`
  - `VariantInit(pvarg: &pvargDest);`
  - `v6 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v6 < 0 )`
  - `LOBYTE(v25) = 2;`
  - `v9 = sub_406472();`
  - `v10 = sub_402680(a1: v9);`
  - `IWzResMan::GetObjectA(this: v10, pvargDest: &v22, sUOL: v18.x, vParam: v18.y, vAux: p_pvarg);`
  - …（截断 16 行）

#### CUIFadeYesNo::CreateNewYearCardArrived  (3932 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1235_INVITE`, `SP_1293_UI_UIWINDOWIMG_FADEYESNO_ICON7`, `SP_1296_UI_UIWINDOWIMG_FADEYESNO_BACKGRND2`
  - `void __thiscall CUIFadeYesNo::CreateNewYearCardArrived(CFadeWnd *this, char a2, int Unknown)`
  - `*(this + 49) = 13;`
  - `ZXString<char>::operator=(a1: v20);`
  - `*(this + 51) = Unknown;`
  - `VariantInit(&pvarg);`
  - `v4 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v4 < 0 )`
  - `LOBYTE(v25) = 1;`
  - `VariantInit(pvarg: &pvargDest);`
  - `v5 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v5 < 0 )`
  - `LOBYTE(v25) = 2;`
  - `v8 = sub_406472();`
  - `v9 = sub_402680(a1: v8);`
  - `IWzResMan::GetObjectA(this: v9, pvargDest: &v21, sUOL: v17.x, vParam: v17.y, vAux: p_pvarg);`
  - `LOBYTE(v25) = 4;`
  - …（截断 16 行）

#### CUIFadeYesNo::CreateParcelAlarm  (4097 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1235_INVITE`, `SP_1298_UI_UIWINDOWIMG_FADEYESNO_BACKGRND4`, `SP_3912_UI_BASICIMG_ICON_DELIVERY`
  - `void __thiscall CUIFadeYesNo::CreateParcelAlarm(CFadeWnd *this, char a2, int Unknown)`
  - `*(this + 49) = 10;`
  - `ZXString<char>::operator=(a1: p_pvarg);`
  - `*(this + 62) = Unknown;`
  - `VariantInit(pvarg: p_pvarg);`
  - `v5 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v5 < 0 )`
  - `LOBYTE(v26) = 1;`
  - `VariantInit(pvarg: &pvargDest);`
  - `v6 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v6 < 0 )`
  - `LOBYTE(v26) = 2;`
  - `v9 = sub_406472();`
  - `v10 = sub_402680(a1: v9);`
  - `IWzResMan::GetObjectA(this: v10, pvargDest: &v22, sUOL: v18.x, vParam: v18.y, vAux: v19);`
  - `LOBYTE(v26) = 4;`
  - …（截断 16 行）

#### CUIFadeYesNo::CreatePartyInvite  (3891 B)

- SP_ 节点：`SP_1235_INVITE`, `SP_1291_UI_UIWINDOWIMG_FADEYESNO_ICON5`, `SP_1295_UI_UIWINDOWIMG_FADEYESNO_BACKGRND`
  - `void __thiscall CUIFadeYesNo::CreatePartyInvite(CFadeWnd *this, char sInviter, int a3, struct tagPOINT *a4)`
  - `*(this + 49) = 5;`
  - `ZXString<char>::operator=(a1: p_sInviter);`
  - `*(this + 55) = a3;`
  - `*(this + 56) = a4;`
  - `VariantInit(&pvarg);`
  - `v5 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v5 < 0 )`
  - `LOBYTE(v25) = 1;`
  - `VariantInit(pvarg: &pvargDest);`
  - `v6 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v6 < 0 )`
  - `LOBYTE(v25) = 2;`
  - `v9 = sub_406472();`
  - `v10 = sub_402680(a1: v9);`
  - `IWzResMan::GetObjectA(this: v10, pvargDest: &v22, sUOL: v18.x, vParam: v18.y, vAux: p_pvarg);`
  - …（截断 16 行）

#### CUIFadeYesNo::CreateQuestClear  (4699 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1235_INVITE`, `SP_1292_UI_UIWINDOWIMG_FADEYESNO_ICON6`, `SP_1293_UI_UIWINDOWIMG_FADEYESNO_ICON7`, `SP_1298_UI_UIWINDOWIMG_FADEYESNO_BACKGRND4`
  - `void __thiscall CUIFadeYesNo::CreateQuestClear(int this, int usQuestID)`
  - `if ( TSingleton<CUIStatusBar>::ms_pInstance != 0 )`
  - `*(this + 196) = 7;`
  - `*(this + 232) = v3;`
  - `sub_402F85(&pvargDest, pvargSrc: v20);`
  - `sub_402F85(pvargDest: &v23, pvargSrc: &pvargSrc);`
  - `v4 = *(this + 232) == 0;`
  - `LOBYTE(v28) = 1;`
  - `if ( v4 )`
  - `LOBYTE(v28) = 2;`
  - `sub_405210(a1: BSTR);`
  - `v8 = sub_406472();`
  - `v9 = sub_402680(a1: v8);`
  - `IWzResMan::GetObjectA(this: v9, pvargDest: &pvarg, sUOL: v17.x, vParam: v17.y, vAux: p_pvargDest);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: p_usQuestID, a2: v20);`
  - `v10 = sub_41E527(a1: &Unknown);`
  - …（截断 16 行）

#### CUIFadeYesNo::CreateTradeInvite  (4032 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1235_INVITE`, `SP_1288_UI_UIWINDOWIMG_FADEYESNO_ICON2`, `SP_1295_UI_UIWINDOWIMG_FADEYESNO_BACKGRND`, `SP_5160_UI_UIWINDOWIMG_FADEYESNO_BACKGRND6`, `SP_5161_UI_UIWINDOWIMG_FADEYESNO_ICON9`
  - `void __thiscall CUIFadeYesNo::CreateTradeInvite(CFadeWnd *this, char a2, struct tagPOINT *a3, int Unknown)`
  - `*(this + 49) = (Unknown != 0) + 2;`
  - `ZXString<char>::operator=(a1: &a2);`
  - `*(this + 51) = a3;`
  - `VariantInit(&pvarg);`
  - `v6 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v6 < 0 )`
  - `LOBYTE(v26) = 1;`
  - `VariantInit(pvarg: &pvargDest);`
  - `v7 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v7 < 0 )`
  - `LOBYTE(v26) = 2;`
  - `a3: (v5 != 0 ? SP_5161_UI_UIWINDOWIMG_FADEYESNO_ICON9 : SP_1288_UI_UIWINDOWIMG_FADEYESNO_ICON2));`
  - `v10 = sub_406472();`
  - `v11 = sub_402680(a1: v10);`
  - `IWzResMan::GetObjectA(this: v11, pvargDest: &v23, sUOL: v19.x, vParam: v19.y, vAux: p_pvarg);`
  - …（截断 16 行）

#### CUIFadeYesNo::CreateUserAlarm  (4459 B)

- SP_ 节点：`SP_1235_INVITE`, `SP_1289_UI_UIWINDOWIMG_FADEYESNO_ICON3`, `SP_1290_UI_UIWINDOWIMG_FADEYESNO_ICON4`, `SP_1296_UI_UIWINDOWIMG_FADEYESNO_BACKGRND2`
  - `void __thiscall CUIFadeYesNo::CreateUserAlarm(CFadeWnd *this, char sUser, VARIANTARG **nType)`
  - `*(this + 49) = 9;`
  - `ZXString<char>::operator=(a1: p_sUser);`
  - `*(this + 59) = nType;`
  - `sub_402F85(&pvargDest, pvargSrc: p_sUser);`
  - `LOBYTE(v27) = 1;`
  - `sub_402F85(pvargDest: &v22, pvargSrc: &pvargSrc);`
  - `LOBYTE(v27) = 2;`
  - `if ( nType != 0 )`
  - `sub_405210(a1: BSTR);`
  - `v7 = sub_406472();`
  - `v8 = sub_402680(a1: v7);`
  - `IWzResMan::GetObjectA(this: v8, pvargDest: &pvarg, sUOL: v16.x, vParam: v16.y, vAux: p_pvargDest);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: p_nType, a2: p_sUser);`
  - `v9 = sub_41E527(a1: &Unknown);`
  - `if ( v9 < 0 && v9 != -2147467262 )`
  - …（截断 16 行）

#### CUIFadeYesNo::Delete  (1009 B)

  - `void __thiscall CUIFadeYesNo::Delete(_DWORD *this)`
  - `if ( *(this + 49) == 13 )`
  - `v7 = *(this + 51);`
  - `ZXString<char>::ZXString<char>(this: &v6, a2: &sStrDefault, a3: 0xFFFFFFFF);`
  - `v5 = *(this + 49);`
  - `v2 = sub_42861D();`
  - `CWvsContext::DeleteFadeWnd(this: v2, a2: v5, a3: 0, a4: 0, a5: v6, a6: v7);`
  - `v7 = v3 != 0 ? (v3 - 2) : 0;`
  - `sub_524634(a1: &v6);`
  - `v4 = sub_42861D();`
  - `CWvsContext::DeleteFadeWnd(this: v4, a2: v6, a3: v7);`

#### CUIFadeYesNo::Draw  (70857 B)

- 分支 case：`0`, `1`, `2`, `3`, `4`, `5`, `6`, `7`, `8`, `9`, `10`, `11`, `12`, `13`
- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_100_MAPLESTORY_ERROR__LOADING_REACTOR_FAILED`, `SP_1604_NAME`, `SP_3232_QUEST_QUESTINFOIMG`, `SP_3873_QUICK_DELIVEY_PACKAGE`, `SP_3874_ARRIVED`, `SP_3875_PACKAGE_R_NRECEIVED`, `SP_3876_THE_QUICK_DELIVERY_HAS_ARRIVED`, `SP_3877__S_`, `SP_4496_PARTY_QUEST`, `SP_5225_THIS_IS_A_CASH_TRADE_INVITE`, `SP_5321_YOU_GOT_A_TITLE`, `SP_5514_04D`, `SP_714_S_IS`, `SP_715_LOGGED_IN`, `SP_716_YOURE_ON_THE_SAME_CH`, `SP_717_WITH_S`, `SP_763_FROM_S`, `SP_764_AN_INVITATION`, `SP_765_BUDDY_REQUEST`, `SP_766_TRADE_REQUEST`, `SP_767_PARTY_INVITE`, `SP_768_GUILD_INVITE`, `SP_769_ITS_A_FAMILY_INVITATION`, `SP_770_NEW_QUEST`, `SP_771_QUEST_COMPLETE`, `SP_80_WE_ARE_UNABLE_TO_CONNECT_TO_THE_LOGIN_SERVER_DUE_TO_A_SERVER_CHECK_A_FIREWALL_BL`
  - `const struct tagRECT *__thiscall CUIFadeYesNo::Draw(CUIFadeYesNo *this, const struct tagRECT *a2)`
  - `CWnd::Draw(this, a2);`
  - `CWnd::GetCanvas(a1: &a2);`
  - `v3 = *(this + 49);`
  - `LOBYTE(v363) = 1;`
  - `switch ( v3 )`
  - `p_pvarg = *(this + 50);`
  - `LOBYTE(v363) = 2;`
  - `ZXString<char>::Format(a1: &v362, a2: p_pvargDest, a3: p_pvarg);`
  - `if ( v349 != 0 )`
  - `sub_402F85(pvargDest: &v303, pvargSrc: &pvargSrc);`
  - `LOBYTE(v363) = 3;`
  - `sub_402F85(pvargDest: &v284, pvargSrc: &pvargSrc);`
  - `LOBYTE(v363) = 4;`
  - `v264 = sub_4284A5(this: v6);`
  - `v9 = sub_414576(a1: &a2);`
  - …（截断 16 行）

#### CUIFadeYesNo::MoveWndPosition  (252 B)

  - `int __thiscall CUIFadeYesNo::MoveWndPosition(CUIFadeYesNo *this)`
  - `v1 = *(this + 38);`
  - `*(this + 40) += 5;`
  - `*(this + 42) += 5;`
  - `return sub_49254B(a1: *(this + 37), a2: v1 + 5);`

#### CUIFadeYesNo::OnButtonClicked  (6932 B)

- 分支 case：`0`, `1`, `2`, `3`, `4`, `5`, `6`, `7`, `8`, `10`, `11`, `12`, `13`
- SP_ 节点：`SP_4613_S_WOULD_LIKE_TO_R_NBE_YOUR_SENIOR_WOULD_YOU_LIKE_TO_BE_R_NSS_JUNIOR`
  - `void __thiscall CUIFadeYesNo::OnButtonClicked(CUIFadeYesNo *this, int a2)`
  - `v3 = (this + 276);`
  - `if ( *(this + 69) == 0 )`
  - `v4 = *(this + 49);`
  - `if ( v4 != 4 )`
  - `if ( a2 == 2000 )`
  - `switch ( v4 )`
  - `CUIMessenger::TryNew(a1: *(this + 51));`
  - `v15[0] = *(this + 52);`
  - `field = get_field();`
  - `CField::SendAcceptFriendMsg(this: field, a2: v15[0]);`
  - `CMiniRoomBaseDlg::SendInviteResult(a1: *(this + 51), a2: 0);`
  - `CMiniRoomBaseDlg::SendCashInviteResult(a1: *(this + 51), a2: 0);`
  - `CUIFadeYesNo::TryShowMemoListDlg(this, a2: 470, a3: 92);`
  - `if ( *(this + 56) != 0 )`
  - `COutPacket::COutPacket(a1: 124);`
  - …（截断 16 行）

#### CUIFadeYesNo::OnCreate  (4949 B)

- SP_ 节点：`SP_1284_UI_UIWINDOWIMG_FADEYESNO_BTOK`, `SP_1285_UI_UIWINDOWIMG_FADEYESNO_BTCANCEL`, `SP_1410_CANVASFONT`, `SP_5527_ARIAL`
  - `void __thiscall CUIFadeYesNo::OnCreate(CUIFadeYesNo *this, void *a2)`
  - `v3 = *(this + 49);`
  - `memset(v29, 0, sizeof(v29));`
  - `if ( v3 <= 2 || v3 == 5 || v3 == 6 || v3 == 8 || v3 == 12 )`
  - `LOBYTE(v33) = 1;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `LOBYTE(v33) = 0;`
  - `v6 = *(this + 49);`
  - `if ( v6 != 9 && (v6 != 10 || *(this + 62) != 0) )`
  - `v7 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v33) = 2;`
  - `if ( v7 != 0 )`
  - `v8 = CCtrlButton::CCtrlButton(this: v7);`
  - `v9 = sub_4284FF(a1: v8);`
  - `(*(**(v9 + 4) + 32))(a1: *(v9 + 4), a2: this, a3: 2000, a4: v32 + 136, a5: 7, a6: 1, a7: v29);`
  - `LOBYTE(v33) = 3;`
  - …（截断 16 行）

#### CUIFadeYesNo::OnMouseButton  (1243 B)

  - `void *__thiscall CUIFadeYesNo::OnMouseButton(_DWORD *this, int a2, int a3, int a4, int a5)`
  - `if ( a2 == 514 )`
  - `if ( *(this + 48) == 7 )`
  - `v12 = v7 != 0 ? (v7 - 2) : 0;`
  - `sub_524634(a1: &v11);`
  - `v8 = sub_42861D();`
  - `if ( CWvsContext::IsTopFadeWnd(this: v8, a2: v11, a3: v12) )`
  - `if ( CWvsContext::ShowQuestInfoDetail(a1: *(this + 114)) == 1 )`
  - `sub_A26FAA(a1: 1, a2: *(this + 114));`
  - `CFadeWnd::Close(this: (this - 1), a2: 1);`
  - `v12 = v9 != 0 ? (v9 - 2) : 0;`
  - `v10 = sub_42861D();`
  - `return CWvsContext::SetTopFadeWnd(this: v10, a2: v11, a3: v12);`

#### CUIFadeYesNo::SendCloseMessage  (694 B)

  - `void __thiscall CUIFadeYesNo::SendCloseMessage(CUIFadeYesNo **this)`
  - `if ( *(this + 49) == 12 )`
  - `v2 = *(this + 55);`
  - `sub_428211(a1: this + 50);`
  - `sub_42861D();`
  - `CWvsContext::SendFamilyInviteResult(a1: v1, a2: v2, a3: v3);`
  - `CFadeWnd::SendCloseMessage(this);`

#### CUIFadeYesNo::TryShowMemoListDlg  (1833 B)

  - `int __thiscall CUIFadeYesNo::TryShowMemoListDlg(CUIFadeYesNo *this, int a2, int a3)`
  - `result = dword_BF0484(a1: v11, a2, a3);`
  - `if ( result != 0 )`
  - `sub_5247C1(a1: v7);`
  - `sub_524667(a1: v4);`
  - `sub_5247C1(a1: v4);`
  - `if ( v8 != 0 )`
  - `CWnd::InvalidateRect(this: v14, a2: 0);`
  - `v16 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 192);`
  - `LOBYTE(v15) = 1;`
  - `if ( v16 != 0 )`
  - `v5 = sub_649D82(a1: v7);`
  - `LOBYTE(v15) = 0;`
  - `sub_5246B0(a1: v12);`
  - `LOBYTE(v15) = 2;`
  - `CDialog::DoModal(a1: v13);`
  - …（截断 16 行）

### CUIShortCutMenu（2 函数）


#### CUIShortCutMenu::OnCreate  (11628 B)

- SP_ 节点：`SP_2228_UI_UIWINDOWIMG_SHORTCUT_BTITEM`, `SP_2229_UI_UIWINDOWIMG_SHORTCUT_BTEQUIP`, `SP_2230_UI_UIWINDOWIMG_SHORTCUT_BTSTAT`, `SP_2231_UI_UIWINDOWIMG_SHORTCUT_BTSKILL`, `SP_2234_UI_UIWINDOWIMG_SHORTCUT_BTQUEST`, `SP_2235_UI_UIWINDOWIMG_SHORTCUT_BTCOMM`, `SP_2236_UI_UIWINDOWIMG_SHORTCUT_BTMOBBOOK`, `SP_2237_UI_UIWINDOWIMG_SHORTCUT_BTMESSENGER`
  - `int __thiscall CUIShortCutMenu::OnCreate(CUIShortCutMenu *this, void *a2)`
  - `LOBYTE(v57) = 1;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `LOBYTE(v57) = 0;`
  - `if ( v56 != 0 )`
  - `v5 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v57) = 2;`
  - `if ( v5 != 0 )`
  - `v6 = CCtrlButton::CCtrlButton(this: v5);`
  - `sub_4284FF(a1: v6);`
  - `(*(**(this + 54) + 32))(a1: *(this + 54), a2: this, a3: 1000, a4: 6, a5: 24, a6: 0, a7: v52);`
  - `LOBYTE(v57) = 3;`
  - `ZXString<unsigned short>::operator=(a1: v8);`
  - `v9 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v57) = 4;`
  - `if ( v9 != 0 )`
  - …（截断 16 行）

#### CUIShortCutMenu::ResetButtonState  (435 B)

  - `void __thiscall CUIShortCutMenu::ResetButtonState(CUIShortCutMenu *this)`
  - `v3 = (this + 216);`
  - `v4 = *(this + 69) != 0 && *(this + 70) == v2;`
  - `CCtrlButton::MouseEnter(this: *v3, a2: v4, a3: 1);`
  - `while ( v2 < 8 );`

### CUIAntiMacroCharacterName（1 函数）


#### CUIAntiMacroCharacterName::GetResult  (484 B)

  - `void __thiscall CUIAntiMacroCharacterName::GetResult(void *this, int a2)`
  - `CCtrlEdit::GetText(a1: &v3);`
  - `ZXString<char>::TrimRight(a1: 0);`
  - `v2 = ZXString<char>::TrimLeft(a1: 0);`
  - `ZXString<char>::operator=(a1: v2);`

### CItemEffectManager（2 函数）


#### CItemEffectManager::LoadItemEffect  (39317 B)

- SP_ 节点：`SP_1467_DX`, `SP_1468_DY`, `SP_1469_DX1`, `SP_1470_DY1`, `SP_1644_EFFECT`, `SP_1803_PATH`, `SP_2982_NOFLIP`, `SP_3588_FOLLOW`, `SP_3820_LEFT`, `SP_3821_RIGHT`, `SP_3822_TOP`, `SP_3823_BOTTOM`, `SP_4435_EMISSION`, `SP_4436_THETA`, `SP_4437_GENONMOVE`, `SP_4438_GENPOINT`, `SP_5624_DELAY`, `SP_5634_INTERVAL`, `SP_961_INFO`, `SP_979_Z`
  - `int __thiscall CItemEffectManager::LoadItemEffect(_DWORD *this, int a2, int a3, signed int a4)`
  - `if ( a3 == 0 )`
  - `*(a2 + 4) = 0;`
  - `get_equip_data_path(a1: &v194, a2: a3);`
  - `if ( v194 != 0 && *v194 != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v200) = 2;`
  - `sub_402F85(pvargDest: &v148, pvargSrc: &pvargSrc);`
  - `LOBYTE(v200) = 3;`
  - `v7 = sub_41468C(a1: &v194);`
  - `_bstr_t::_bstr_t(a1: v7);`
  - `v9 = sub_406472();`
  - `v10 = sub_402680(a1: v9);`
  - `IWzResMan::GetObjectA(this: v10, pvargDest: &pvarg, sUOL: v136, vParam: v137, vAux: p_pvargDest);`
  - `LOBYTE(v200) = 5;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v139, a2: v140);`
  - …（截断 16 行）

#### CItemEffectManager::UpdateEffects  (1000 B)

  - `int __thiscall CItemEffectManager::UpdateEffects(CItemEffectManager *this, int *a2)`
  - `for ( i = 0; ; ++i )`
  - `if ( *v9 == 0 || i >= *(result - 4) )`
  - `v5 = *(8 * i + result + 4);`
  - `if ( v5 == 0 || (*(*v5 + 12))(a1: v5) != *a2 )`
  - `ItemEffect = CItemEffectManager::LoadItemEffect(this: v9, a2: v7, a3: *a2, a4: i);`
  - `sub_5C7C70(a1: ItemEffect);`
  - `if ( v8 != 0 )`
  - `sub_5C7CA9(a1: 0);`

### CItemSpeakerDlg（4 函数）


#### CItemSpeakerDlg::OnButtonClicked  (360 B)

  - `void __thiscall CItemSpeakerDlg::OnButtonClicked(CItemSpeakerDlg *this, unsigned int a2)`
  - `if ( a2 == 1 )`
  - `if ( CItemSpeakerDlg::_SendConsumeCashItemUseRequest(this) != 0 )`
  - `(*(*this + 52))(a1: this, a2: 1);`
  - `else if ( a2 == 2 )`
  - `(*(*this + 52))(a1: this, a2: 2);`

#### CItemSpeakerDlg::OnCreate  (11186 B)

- SP_ 节点：`SP_1257_UI_BASICIMG_BTOK2`, `SP_1258_UI_BASICIMG_BTCANCEL2`
  - `void __thiscall CItemSpeakerDlg::OnCreate(CItemSpeakerDlg *this, void *a2)`
  - `CCtrlEdit::CREATEPARAM::CREATEPARAM(this: v49);`
  - `v3 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 184);`
  - `LOBYTE(v61) = 1;`
  - `if ( v3 != 0 )`
  - `v4 = CCtrlEdit::CCtrlEdit(this: v3);`
  - `LOBYTE(v61) = 0;`
  - `sub_473956(a1: v4);`
  - `v5 = *(this + 366);`
  - `v47 = *(this + 10) - 86;`
  - `(*(*v5 + 8))(a1: v5, a2: this, a3: 1001, a4: 19, a5: v47, a6: 202, a7: 16, a8: v49);`
  - `CWnd::SetFocusChild(this, a2: *(this + 366));`
  - `LOBYTE(v61) = 2;`
  - `LOBYTE(v61) = 3;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `if ( v59 != 0 )`
  - …（截断 16 行）

#### CItemSpeakerDlg::PutItem  (15097 B)

- SP_ 节点：`SP_956_CANVAS`
  - `signed int __thiscall CItemSpeakerDlg::PutItem(`
  - `*(this + 17) = a2;`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &pvarg.lVal) + 4);`
  - `sub_4289B7(this: &pvarg.lVal);`
  - `Item = CharacterData::GetItem(this: v3, a2: &pvarg.lVal, a3: *(this + 34), a4: *(this + 35));`
  - `sub_4288AF(a1: Item);`
  - `sub_4288C1(a1: this + 18);`
  - `v5 = *(Item + 4);`
  - `*(this + 37) = v5;`
  - `sub_4288C1(a1: &pvarg.decVal.Lo32);`
  - `sub_402FAB(a1: -2, a2: 3);`
  - `v6 = *(this + 373) == 0;`
  - `if ( v6 )`
  - `IWzGr2DLayer::RemoveCanvas(this: *(this + 373), a2: &Data, a3: &pvarg);`
  - `sub_4156AB(a1: &Data);`
  - `if ( v7 < 0 )`
  - …（截断 16 行）

#### CItemSpeakerDlg::_SendConsumeCashItemUseRequest  (3978 B)

- SP_ 节点：`SP_297_OTHER_REQUESTS_ARE_IN_PROCESS_R_NPLEASE_TRY_AGAIN_IN_A_LITTLE_BIT`, `SP_4434_YOU_HAVE_NOT_SELECTED_AN_ITEM_R_NARE_YOU_SURE_YOU_STILL_WANT_TO_USE_THE_ITEM`
  - `int __thiscall CItemSpeakerDlg::_SendConsumeCashItemUseRequest(CItemSpeakerDlg *this)`
  - `CCtrlEdit::GetText(this: *(this + 366), a2: &v24);`
  - `ZXString<char>::TrimRight(a1: 0);`
  - `v2 = *ZXString<char>::TrimLeft(a1: 0);`
  - `if ( v24 != 0 )`
  - `if ( v4 )`
  - `if ( *(this + 37) == 0 )`
  - `if ( CUtilDlg::YesNo(a1: v16, a2: v17, a3: v18, a4: v19, a5: v20[0]) == 7 )`
  - `if ( *dword_BE7918 == 0 )`
  - `if ( CWvsContext::CanSendExclRequest(a1: 500, a2: 0) == 0 )`
  - `CUtilDlg::Notice(sMsg: v16, sSndName: v17, ppDialog: v18, bAutoSeparated: v19, bIsTightLine: v20[0]);`
  - `COutPacket::COutPacket(a1: 79);`
  - `v10 = *(this + 64);`
  - `COutPacket::Encode2(a1: v10);`
  - `COutPacket::Encode4(this: &v21, a2: *(this + 33));`
  - `v12 = sub_4B1F6F(this: this + 365);`
  - …（截断 16 行）

### CCSWnd_Status（4 函数）


#### CCSWnd_Status::Draw  (10582 B)

  - `struct tagRECT *__thiscall CCSWnd_Status::Draw(CCSWnd_Status *this, struct tagRECT *a2)`
  - `CWnd::Draw(this, a2);`
  - `CWnd::GetCanvas(a1: &a2);`
  - `v3 = *(this + 35);`
  - `p_pvarg = *(v3 + 36);`
  - `LOBYTE(v66) = 2;`
  - `v4 = _ZtlSecureFuse<long>(a1: v3 + 28, a2: p_pvarg);`
  - `format_integer(a1: &v63, a2: v4, a3: 1);`
  - `v5 = _ZtlSecureFuse<long>(a1: *(this + 35) + 40, a2: *(*(this + 35) + 48));`
  - `format_integer(a1: &v64, a2: v5, a3: 1);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v66) = 3;`
  - `v6 = sub_428226(a1: &v63);`
  - `_bstr_t::_bstr_t(a1: v6);`
  - `LOBYTE(v66) = 4;`
  - `basic_font = get_basic_font(a1: &v62, fType: 25);`
  - …（截断 16 行）

#### CCSWnd_Status::OnButtonClicked  (594 B)

  - `void __thiscall CCSWnd_Status::OnButtonClicked(CCashShop **this, unsigned int a2)`
  - `switch ( a2 )`
  - `CCashShop::OnStatusCharge(this: *(this + 35));`
  - `CCashShop::OnStatusCheck(this: *(this + 35));`
  - `CCashShop::OnStatusCoupon(this: *(this + 35));`
  - `CCashShop::OnStatusExit(this: *(this + 35));`

#### CCSWnd_Status::OnCreate  (5204 B)

- SP_ 节点：`SP_1227_UI_CASHSHOPIMG_CSSTATUS_BTCHARGE`, `SP_1228_UI_CASHSHOPIMG_CSSTATUS_BTCHECK`, `SP_1229_UI_CASHSHOPIMG_CSSTATUS_BTCOUPON`, `SP_1230_UI_CASHSHOPIMG_CSSTATUS_BTEXIT`, `SP_1280_STRING_TOOLTIPHELPIMG_SHOP_BUTTON_CHARGE`, `SP_1281_STRING_TOOLTIPHELPIMG_SHOP_BUTTON_CHECK`, `SP_1282_STRING_TOOLTIPHELPIMG_SHOP_BUTTON_COUPON`
  - `void __userpurge CCSWnd_Status::OnCreate(CCSWnd_Status *this@<ecx>, int a2@<edi>, volatile LONG *a3)`
  - `*(this + 35) = a3;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `if ( a3 != 0 )`
  - `v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v28) = 2;`
  - `if ( v6 != 0 )`
  - `v7 = CCtrlButton::CCtrlButton(this: v6);`
  - `LOBYTE(v28) = 0;`
  - `sub_4284FF(a1: v7);`
  - `(*(**(this + 28) + 32))(a1: *(this + 28), a2: this, a3: 1000, a4: 248, a5: 13, a6: 0, a7: v26, a8: a2);`
  - `LOBYTE(v28) = 3;`
  - `sub_4C10F7(a1: v24, a2: 1, a3: 1);`
  - `LOBYTE(v28) = 4;`
  - `ZXString<unsigned short>::operator=(a1: v10);`
  - `v11 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - …（截断 16 行）

#### CCSWnd_Status::OnKeyRet  (2425 B)

- SP_ 节点：`SP_592_ARE_YOU_SURE_YOU_WANT_TO_LEAVE_THE_CASH_SHOP`
  - `int __thiscall CCSWnd_Status::OnKeyRet(CCSWnd_Status *this, unsigned int a2, int *a3)`
  - `if ( SHIBYTE(a3) >= 0 )`
  - `switch ( a2 )`
  - `if ( (*(*(*(this + 34) + 4) + 32))(a1: *(this + 34) + 4) != 0 )`
  - `sub_428211(a1: String);`
  - `HIBYTE(a3) = CUtilDlg::YesNo(a1: v9, a2: v10, a3: v11, a4: v12, a5: v13) == 6;`
  - `if ( HIBYTE(a3) == 0 )`
  - `if ( (*(*(*(this + 28) + 4) + 32))(a1: *(this + 28) + 4) == 0 )`
  - `if ( (*(*(*(this + 30) + 4) + 32))(a1: *(this + 30) + 4) == 0 )`
  - `if ( (*(*(*(this + 32) + 4) + 32))(a1: *(this + 32) + 4) == 0 )`
  - `(*(v4 + 32))(a1: this, a2: v13);`

### CCashShop（14 函数）


#### CCashShop::Close  (577 B)

  - `int __thiscall CCashShop::Close(CCashShop *this)`
  - `CSoundMan::PlayBGM(a1: 0, a2: 0, a3: 600, a4: 1000, a5: 0);`
  - `CStage::FadeIn();`
  - `sub_428712(a1: 0);`
  - `CWnd::Destroy(a1: this + 1364);`
  - `CWnd::Destroy(a1: this + 2468);`
  - `CWnd::Destroy(a1: this + 3904);`
  - `CWnd::Destroy(a1: this + 5388);`
  - `CWnd::Destroy(a1: this + 5564);`
  - `CWnd::Destroy(a1: this + 7516);`
  - `CWnd::Destroy(a1: this + 7648);`
  - `return CWnd::Destroy(a1: this + 7792);`

#### CCashShop::Init  (12279 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1249_SOUND_BGMUIIMG_SHOPBGM`, `SP_1301_UI_CASHSHOPIMG_BASE_BACKGRND`, `SP_1302_UI_CASHSHOPIMG_BASE_BACKGRND1`, `SP_40_ARCH_MAGEICELIGHTNING`, `SP_5361_UI_CASHSHOPIMG_BASE_BACKGRND2`
  - `void __thiscall CCashShop::Init(VARIANTARG *this, _DWORD *a2)`
  - `CWvsContext::ClearFieldUI(this: *dword_BE7918);`
  - `*(*dword_BE7918 + 12488) = 0;`
  - `*(this + 9) = _ZtlSecureTear<long>(a1: 0, a2: this + 28);`
  - `*(this + 12) = _ZtlSecureTear<long>(a1: 0, a2: this + 40);`
  - `*(this + 15) = _ZtlSecureTear<long>(a1: 0, a2: this + 52);`
  - `p_pvargDest = (this + 1180);`
  - `*(this + 331) = 1;`
  - `*(this + 332) = 0;`
  - `*(this + 333) = 0;`
  - `*(this + 334) = 1;`
  - `memset(a1: p_pvargDest, a2: p_Unknown, a3: v48);`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v52) + 4);`
  - `v48 = *(v3 + 61);`
  - `LOBYTE(v56) = 1;`
  - `v4 = _ZtlSecureFuse<short>(a1: v3 + 57, a2: v48);`
  - …（截断 16 行）

#### CCashShop::IsUsableItem  (15531 B)

- SP_ 节点：`SP_4666_YOU_CURRENTLY_DO_NOT_HAVE_ANY_ITEMS_THAT_R_NCAN_USE_THE_SCISSORS_OF_KARMA_R_NWOU`, `SP_494_YOUR_JOB_LIMIT_DOES_NOT_ALLOW_R_NYOU_TO_USE_THIS_ITEM__R_NARE_YOU_SURE_YOU_WANT_`, `SP_495_YOUR_LEVEL_LIMIT_DOES_NOT_ALLOW_R_NYOU_TO_USE_THIS_ITEM_R_NARE_YOU_SURE_YOU_WANT`, `SP_496_YOUR_FAME_LIMIT_DOES_NOT_ALLOW_R_NYOU_TO_USE_THIS_ITEM_R_NARE_YOU_SURE_YOU_WANT_`, `SP_499_YOU_MAY_NOT_USE_THIS_ITEM_WITH_R_NTHE_WEAPON_YOU_ARE_EQUIPPED_WITH_R_NWILL_YOU_C`, `SP_501__R_NTHE_CURRENT_PET_YOU_HAVE_R_NCANNOT_WEAR_THIS_ITEM_R_NARE_YOU_SURE_YOU_WANT_T`, `SP_502_ONLY_PETS_ARE_ALLOWED_TO_WEAR_THIS_ITEM_R_NARE_YOU_SURE_YOU_WANT_TO_PURCHASE_THI`, `SP_503_THIS_ITEM_CANNOT_BE_USED_WITHOUT_A_PET_AND_R_NTHIS_ITEM_CANNOT_BE_USED_ON_A_DECE`, `SP_504__R_NTHIS_ITEM_CANNOT_BE_USED_BY_THE_R_NPET_YOURE_CURRENTLY_RAISING_R_NARE_YOU_SU`, `SP_505_THIS_IS_AN_ITEM_THAT_CANNOT_BE_USED_R_NIF_YOU_DO_NOT_HAVE_THE_MONSTER_R_NRIDING_`, `SP_557_DUE_TO_AN_UNKNOWN_ERROR_R_NTHE_REQUEST_FOR_CASH_SHOP_HAS_FAILED`
  - `int __stdcall CCashShop::IsUsableItem(int a1, int a2)`
  - `v2 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v67) + 4);`
  - `sub_4289B7(this: v67);`
  - `if ( a1 / 10000 == 519 || a1 / 10000 == 524 )`
  - `CItemInfo::GetItemInfo(a1: &v74, a2: a1);`
  - `if ( v74 == 0 )`
  - `LOBYTE(v76) = 1;`
  - `_CxxThrowException(a1: &a2, a2: &_TI3_AVCTerminateException__);`
  - `if ( CharacterData::GetActiveItemSlotPet(this: v2, a2: 0) == 0 )`
  - `LOBYTE(v76) = 2;`
  - `ZXString<char>::operator=(a1: String);`
  - `LOBYTE(v76) = 0;`
  - `if ( !v6 )`
  - `sub_4027E9(a1: &v74);`
  - `ActiveItemSlotPet = CharacterData::GetActiveItemSlotPet(this: v2, a2: v73);`
  - `if ( ActiveItemSlotPet == 0 )`
  - …（截断 16 行）

#### CCashShop::IsUsableItemGift  (10049 B)

- SP_ 节点：`SP_503_THIS_ITEM_CANNOT_BE_USED_WITHOUT_A_PET_AND_R_NTHIS_ITEM_CANNOT_BE_USED_ON_A_DECE`, `SP_505_THIS_IS_AN_ITEM_THAT_CANNOT_BE_USED_R_NIF_YOU_DO_NOT_HAVE_THE_MONSTER_R_NRIDING_`, `SP_5439_WITH_THE_WEAPON_THAT_S_CURRENTLY_HAS_EQUIPPED_R_NTHIS_ITEM_CANT_BE_USED_R_NDO_YO`, `SP_5472_S_DOES_NOT_HAVE_AN_ITEM_THAT__R_NTHE_SCISSORS_OF_KARMA_CAN_BE_USED_ON_R_NDO_YOU_`, `SP_5536_DUE_TO_JOB_LIMITATIONS_S_CANT_R_NUSE_THIS_ITEM_R_NDO_YOU_STILL_WISH_TO_GIVE_IT_A`, `SP_557_DUE_TO_AN_UNKNOWN_ERROR_R_NTHE_REQUEST_FOR_CASH_SHOP_HAS_FAILED`
  - `int __stdcall CCashShop::IsUsableItemGift(int a1, int a2, int a3, int a4, int a5)`
  - `if ( a1 / 10000 == 519 || v5 == 524 )`
  - `CItemInfo::GetItemInfo(a1: &v50, a2: a1);`
  - `LOBYTE(v52) = 1;`
  - `if ( v50 == 0 )`
  - `LOBYTE(v52) = 2;`
  - `_CxxThrowException(a1: &a2, a2: &_TI3_AVCTerminateException__);`
  - `if ( CharacterData::GetActiveItemSlotPet(this: a4, a2: 0) == 0 )`
  - `LOBYTE(v52) = 3;`
  - `ZXString<char>::operator=(a1: String);`
  - `if ( a4 != 0 )`
  - `sub_4027E9(a1: &v50);`
  - `if ( a3 != 0 )`
  - `LOBYTE(v52) = 0;`
  - `if ( v5 == 552 )`
  - `v50 = (a4 + 1099);`
  - …（截断 16 行）

#### CCashShop::OnBuyAvatar  (16568 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_2864_THE_OPTION_IS_NOT_AVAILABLE_FOR_THAT_PACKAGE`, `SP_2964_YOU_MAY_NOT_PURCHASE_THE_ITEM_R_NSINCE_YOU_ALREADY_HAVE_IT`, `SP_3746_HIRED_MERCHANTS_SHOP_IS_ONLY_AVAILABLE_FOR__R_NCHARACTERS_THAT_ARE_OVER_LEVEL_15`, `SP_3749_THIS_ITEM_CAN_ONLY_BE_USED_15_DAYS__R_NAFTER_THE_ACCOUNTS_REGISTRATION`, `SP_498_YOU_ALREADY_HAVE_THE_ITEMS_SELECTED_ARE_R_NYOU_SURE_YOU_WANT_TO_BUY_THE_ITEMS_AG`, `SP_507_TOTAL_D_NX`, `SP_508_MAKE_PURCHASE`, `SP_535_YOU_DONT_HAVE_ENOUGH_CASH`, `SP_557_DUE_TO_AN_UNKNOWN_ERROR_R_NTHE_REQUEST_FOR_CASH_SHOP_HAS_FAILED`
  - `void __thiscall CCashShop::OnBuyAvatar(CCashShop *this)`
  - `if ( *(this + 16) == 0 )`
  - `CCashShop::NoticeFailReason(this, a2: 195);`
  - `if ( *(this + 6) == 0 )`
  - `if ( *(this + 311) == 0 )`
  - `if ( CUtilDlg::Notice(sMsg: v42, sSndName: v43, ppDialog: v44, bAutoSeparated: p_Data, bIsTightLine: v46) == 0 )`
  - `ZArray<long>::RemoveAll(a1: this + 1228);`
  - `for ( i = 0; i < 53; ++i )`
  - `v3 = CCSWnd_Char::CmpAvatar(this: (this + 1364), a2: i);`
  - `if ( v3 != 0 )`
  - `*ZArray<long>::InsertBefore(a1: -1) = v3;`
  - `v4 = CCSWnd_Char::CmpPet(this: (this + 1364));`
  - `if ( v4 != 0 )`
  - `*ZArray<long>::InsertBefore(a1: -1) = v4;`
  - `v5 = sub_4ACFEF(a1: this + 1364);`
  - `if ( v5 != 0 )`
  - …（截断 16 行）

#### CCashShop::OnBuyPackage  (10918 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_498_YOU_ALREADY_HAVE_THE_ITEMS_SELECTED_ARE_R_NYOU_SURE_YOU_WANT_TO_BUY_THE_ITEMS_AG`, `SP_506_ARE_YOU_SURE_YOU_WANT_TO_BUY_THIS_ITEM`, `SP_5207_SOME_ITEMS_CANNOT_BE_TRADED_WITH_OTHER_USERS_AND_R_N`, `SP_535_YOU_DONT_HAVE_ENOUGH_CASH`, `SP_557_DUE_TO_AN_UNKNOWN_ERROR_R_NTHE_REQUEST_FOR_CASH_SHOP_HAS_FAILED`
  - `void __thiscall CCashShop::OnBuyPackage(CCashShop *this, int a2)`
  - `if ( *(this + 6) != 0 )`
  - `CWvsContext::GetCommodityBySN(a1: v39, a2);`
  - `LOBYTE(v51) = 2;`
  - `if ( v40 == 0 )`
  - `if ( TSecType<long>::GetData(a1: (v40 + 16), a2: 0) / 10000 != 910 )`
  - `if ( CUtilDlg::Notice(sMsg: v30, sSndName: v31, ppDialog: v32, bAutoSeparated: v33, bIsTightLine: v34) != 0 )`
  - `TSecType<long>::GetData(a1: (v40 + 16), a2: 0);`
  - `v5 = *(v40 + 32);`
  - `if ( v5 != 0 )`
  - `if ( _ZtlSecureFuse<long>(a1: this + 28, a2: *(this + 9)) >= v5 )`
  - `if ( _ZtlSecureFuse<long>(a1: this + 40, a2: *(this + 12)) >= v5 )`
  - `v6 = _ZtlSecureFuse<long>(a1: this + 52, a2: *(this + 15));`
  - `if ( v6 >= v5 )`
  - `if ( v49 == 0 )`
  - `CUtilDlg::Notice(sMsg: v30, sSndName: v31, ppDialog: v32, bAutoSeparated: v33, bIsTightLine: v34);`
  - …（截断 16 行）

#### CCashShop::OnChangedCategory  (296 B)

  - `void __thiscall CCashShop::OnChangedCategory(CCashShop *this, int a2)`
  - `*(this + 331) = a2;`
  - `CCSWnd_Tab::ChangeCategory(this: (this + 5388));`
  - `CCSWnd_Best::ChangeCategory(this: this + 1879);`
  - `CCashShop::OnChangedCategorySub(this, a2: 0);`

#### CCashShop::OnChangedCategorySub  (277 B)

  - `void __thiscall CCashShop::OnChangedCategorySub(CCashShop *this, int a2)`
  - `*(this + 332) = a2;`
  - `sub_4B6B26(this: this + 1347);`
  - `CCSWnd_List::ChangeCategorySub(this: this + 1391);`
  - `CCashShop::OnChangedPage(this, a2: 0);`

#### CCashShop::OnChangedPage  (165 B)

  - `void __thiscall CCashShop::OnChangedPage(CCashShop *this, int a2)`
  - `*(this + 333) = a2;`
  - `CCSWnd_List::ChangePage(this: (this + 5564));`

#### CCashShop::OnDefaultAvatar  (130 B)

  - `void __thiscall CCashShop::OnDefaultAvatar(CCashShop *this)`
  - `CITCWnd_Char::OnDefaultAvatar(this: (this + 1364));`

#### CCashShop::OnGiftMateInfoResult  (14369 B)

- SP_ 节点：`SP_4538_PLEASE_CONFIRM_WHETHER_R_NTHE_CHARACTERS_NAME_IS_CORRECT`, `SP_535_YOU_DONT_HAVE_ENOUGH_CASH`, `SP_5471_THIS_CONTAINS_AN_ITEM_THAT_WILL_DISAPPEAR_ONCE__R_NTHE_EXPIRATION_DATE_HAS_PASSE`, `SP_557_DUE_TO_AN_UNKNOWN_ERROR_R_NTHE_REQUEST_FOR_CASH_SHOP_HAS_FAILED`
  - `void __thiscall CCashShop::OnGiftMateInfoResult(int **this, volatile LONG *a2)`
  - `if ( CInPacket::Decode1(this: a2) == 0 )`
  - `CUtilDlg::Notice(sMsg: v41, sSndName: v42, ppDialog: v43, bAutoSeparated: v44, bIsTightLine: v45[0]);`
  - `v52 = CInPacket::Decode4(this: v2);`
  - `v59 = CInPacket::Decode4(this: v2);`
  - `CInPacket::DecodeStr(a1: &v67);`
  - `CInPacket::DecodeStr(a1: &v65);`
  - `LOBYTE(v68) = 1;`
  - `CharacterData::CharacterData(this: v46);`
  - `LOBYTE(v68) = 2;`
  - `BasicStat::Clear(this: v51);`
  - `ForcedStat::Clear(this: v50);`
  - `v6 = CharacterData::Decode(this: v46, a2: v2, a3: 0);`
  - `if ( v52 == 0 || v59 == 0 || v67 == 0 || v65 == 0 || (HIDWORD(v6) & v6) != 0xFFFFFFFF )`
  - `sub_77EC9F(a1: v46, a2: v50, a3: &v47, a4: v48, a5: v49, a6: 0, a7: 0, a8: 0);`
  - `CWvsContext::GetCommodityBySN(a1: v62, a2: v59);`
  - …（截断 16 行）

#### CCashShop::OnKey  (935 B)

  - `signed int __thiscall CCashShop::OnKey(CCashShop *this, unsigned int a2, int *a3)`
  - `if ( a3 >= 0 && a2 == 20 )`
  - `IsControlingUserPreview = CCashShop::IsControlingUserPreview(this: (this - 4));`
  - `CCashShop::SetUserPreviewControl(this: (this - 4), a2: IsControlingUserPreview == 0);`
  - `if ( CCashShop::IsControlingUserPreview(this: (this - 4)) == 0`
  - `|| (result = CCSWnd_Char::OnKeyRet(this: (this + 1360), a2, a3)) == 0 )`
  - `result = CCSWnd_Tab::OnKeyRet(this: (this + 5384), a2, a3);`
  - `if ( result == 0 )`
  - `result = CCSWnd_List::OnKeyRet(this: (this + 5560), wParam: a2, lParam: a3);`
  - `return CCSWnd_Status::OnKeyRet(this: (this + 7644), a2, a3);`

#### CCashShop::OnTakeOff  (94 B)

  - `int __thiscall CCashShop::OnTakeOff(CCashShop *this)`
  - `return sub_4AC6D6(a1: this + 1364);`

#### CCashShop::SendBuyAvatarPacket  (5829 B)

- SP_ 节点：`SP_509_THE_PURCHASE_HAS_FAILED`, `SP_510_YOU_HAVE_PURCHASED_THE_FOLLOWING_ITEMS_R_NSOME_ITEMS_WERENT_PURCHASED`, `SP_529_YOU_HAVE_SUCCESSFULLY_MADE_THE_PURCHASE`
  - `void __thiscall CCashShop::SendBuyAvatarPacket(CCashShop *this)`
  - `v2 = *(this + 309);`
  - `if ( v2 <= 0 )`
  - `v3 = *(this + 308);`
  - `v4 = (this + 1232);`
  - `if ( v3 != 0 )`
  - `v5 = *(v3 - 4);`
  - `v6 = *(this + 307);`
  - `if ( v6 != 0 )`
  - `v7 = *(v6 - 4);`
  - `if ( v5 == v7 )`
  - `LOBYTE(v38) = 1;`
  - `ZXString<char>::Format(a1: &v37, a2: v27, a3: v28);`
  - `LOBYTE(v38) = 0;`
  - `if ( *v4 != 0 )`
  - `String = *(*v4 - 4);`
  - …（截断 16 行）

### CUICashItemGachapon（3 函数）


#### CUICashItemGachapon::OnCashItemGachaponResult  (1623 B)

  - `int __thiscall CUICashItemGachapon::OnCashItemGachaponResult(CUICashItemGachapon *this, wchar_t *a2)`
  - `v16 = CInPacket::Decode4(this: a2);`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `v4 = CInPacket::Decode1(this: a2);`
  - `*(this + 41) = v16;`
  - `*(this + 42) = v3;`
  - `*(this + 54) = v4;`
  - `update_time = get_update_time();`
  - `*(this + 53) = update_time + 2500;`
  - `*(this + 45) = 1;`
  - `CWnd::GetLayer(a1: v12);`
  - `Layer = CWnd::GetLayer(a1: &a2);`
  - `LOBYTE(v17) = 1;`
  - `sub_428EC4(a1: Layer);`
  - `LOBYTE(v17) = 3;`
  - `sub_432CBC(a1: v10, a2: v9);`
  - …（截断 16 行）

#### CUICashItemGachapon::OnCreate  (8416 B)

- SP_ 节点：`SP_5066_UI_UIWINDOWIMG_CASHGACHAPON_BTOPEN`, `SP_5067_UI_UIWINDOWIMG_CASHGACHAPON_BTCLOSE`
  - `void __thiscall CUICashItemGachapon::OnCreate(CUICashItemGachapon *this, void *a2)`
  - `LOBYTE(v48) = 1;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `LOBYTE(v48) = 0;`
  - `if ( Unknown != 0 )`
  - `v5 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v48) = 2;`
  - `if ( v5 != 0 )`
  - `v6 = CCtrlButton::CCtrlButton(this: v5);`
  - `v7 = sub_4284FF(a1: v6);`
  - `(*(**(v7 + 4) + 32))(a1: *(v7 + 4), a2: this, a3: 2000, a4: 68, a5: 149, a6: 1, a7: v42);`
  - `LOBYTE(v48) = 3;`
  - `ZXString<unsigned short>::operator=(a1: v9);`
  - `v10 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v48) = 4;`
  - `if ( v10 != 0 )`
  - …（截断 16 行）

#### CUICashItemGachapon::Update  (13716 B)

  - `void *__thiscall CUICashItemGachapon::Update(CUICashItemGachapon *this)`
  - `if ( *(this + 41) != 0 )`
  - `result = get_update_time();`
  - `if ( result > *(this + 53) && *(this + 44) == 0 )`
  - `if ( *(this + 45) != 0 )`
  - `(*(*(*(this + 47) + 4) + 28))(a1: *(this + 47) + 4, a2: 1);`
  - `*(this + 45) = 0;`
  - `CWnd::GetCanvas(a1: &v53);`
  - `ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF);`
  - `LOBYTE(v54) = 1;`
  - `v41.cyVal.Hi = *(this + 41);`
  - `LOBYTE(v54) = 2;`
  - `CItemInfo::GetItemName(a1: v47, a2: SBYTE4(v41.decVal.Lo64));`
  - `v3 = *(this + 42);`
  - `LOBYTE(v54) = 3;`
  - `if ( v3 <= 1 )`
  - …（截断 16 行）

## 3. 坐标 / 尺寸 / 按钮 id / 封包字段候选行（核心窗口全函数）


### CCashShop::IsUsableItem
  - `if ( CharacterData::GetActiveItemSlotPet(this: v2, a2: 0) == 0 )`
  - `v45 = *(CharacterData::GetItem(this: v75, a2: v72, a3: 5, a4: CashItemSlotPosition) + 4);`
  - `if ( CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v55, a3: 10000 * v57 + 1004, a4: 0) <= 0 )`

### CCashShop::IsUsableItemGift
  - `if ( CharacterData::GetActiveItemSlotPet(this: a4, a2: 0) == 0 )`
  - `&& CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: a4, a3: 1004, a4: 0) <= 0`
  - `&& CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: a4, a3: &loc_989A68 + 4, a4: 0) <= 0 )`

### CCashShop::OnBuyAvatar
  - `Data = TSecType<long>::GetData(a1: (v18 + 16), a2: 0);`

### CCashShop::OnBuyPackage
  - `if ( TSecType<long>::GetData(a1: (v40 + 16), a2: 0) / 10000 != 910 )`
  - `TSecType<long>::GetData(a1: (v40 + 16), a2: 0);`
  - `Data = TSecType<long>::GetData(a1: v42 + 16, a2: 0);`
  - `v12 = TSecType<long>::GetData(a1: v42 + 16, a2: 0);`

### CCashShop::OnGiftMateInfoResult
  - `Data = TSecType<long>::GetData(a1: (v63 + 16), a2: 0);`

### CCashShop::OnKey
  - `CCashShop::SetUserPreviewControl(this: (this - 4), a2: IsControlingUserPreview == 0);`
  - `|| (result = CCSWnd_Char::OnKeyRet(this: (this + 1360), a2, a3)) == 0 )`

### CCashShop::SendBuyAvatarPacket
  - `CWvsContext::GetCommodityBySN(a1: v31, a2: *(*(this + 307) + 4 * *(*(this + 308) + 4 * i)));`
  - `Data = TSecType<long>::GetData(a1: (v32 + 16), a2: 0);`
  - `COutPacket::Encode1(this: &v29, a2: *(this + 310) == 2);`

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

### CItemEffectManager::LoadItemEffect
  - `if ( v196 != 0 && IWzProperty::Getcount(a1: v196) != 0 )`
  - `if ( v197 == 0 || IWzProperty::Getcount(a1: v197) == 0 )`

### CItemSpeakerDlg::OnCreate
  - `CCtrlCheckBox::SetChecked(this: *(this + 364), a2: 1);`

### CItemSpeakerDlg::PutItem
  - `Item = CharacterData::GetItem(this: v3, a2: &pvarg.lVal, a3: *(this + 34), a4: *(this + 35));`
  - `PcCreateObject::IWzCanvas(a1: v12, a2: &a2 + 1, a3: 0);`
  - `v29 = *StringPool::GetStringW(this: v28, a2: v56[0], a3: SHIDWORD(v56[0]));`

### CItemSpeakerDlg::_SendConsumeCashItemUseRequest
  - `COutPacket::Encode1(this: &v21, a2: *(*(this + 364) + 72));`
  - `COutPacket::Encode1(this: &v21, a2: *(this + 37) != 0);`

### CMacroSysMan::Update
  - `if ( v8 != 0 && *(29 * v7 + *(this + 11) + 25) == 0 && get_field() != 0 && *(*dword_BE7918 + 8292) == 0 )`

### CNoticeQuestProgress::OnQuestProgressUpdated_Item
  - `ItemCount = CWvsContext::GetItemCount(a1: *(*(v4 + 92) + 8 * i));`
  - `if ( *(v12 + 3) + 3000 >= get_update_time() && usQuestID == *v12 && nItemID == *(v12 + 1) )`

### CNoticeQuestProgress::OnQuestProgressUpdated_Mob
  - `if ( *(v36 + 3) + 3000 >= get_update_time() && a2 == *v36 )`

### CQuest::OnCompleteQuestFailed
  - `Data = TSecType<long>::GetData(a1: v32 + 12, a2: 0);`
  - `v165 = (CNpcTemplate::GetNpcTemplate(a1: *(v216 + 4)) + 148);`
  - `CUtilDlgEx::SetUtilDlgEx_TEXT(this: v214, a2: v116 > 0, a3: v116 < (v123 - 1));`
  - `CUtilDlgEx::SetUtilDlgEx_TEXT(this: v214, a2: 0, a3: 0);`

### CQuest::SayNormal
  - `CUtilDlgEx::SetUtilDlgEx_TEXT(this: v31, a2: v34 > 0, a3: v34 < (v11 - 1));`

### CSkillInfo::GetSkillLevel
  - `if ( ZMap<long,long,long>::GetAt(a1: &a3, a2: &a4) == 0 || a4 == 0 )`

### CUICashItemGachapon::Update
  - `ItemIcon = CItemInfo::GetItemIcon(a1: &p_lVal, a2: v41.decVal.Hi32, bPetDead: 0, bWithShadow: 0);`

### CUIEnchantDlg::MakeClone
  - `Data = TSecType<long>::GetData(a1: (v3 + 12), a2: v32 + 12);`

### CUIEnchantDlg::RefreshToolTip
  - `CharacterData::GetItem(this: v21, a2: v29, a3: *(this + 36), a4: *(this + 37));`

### CUIEquip::OnMouseMove
  - `v27 = *(CharacterData::GetItem(this: v26, a2: v21, a3: 1, a4: -v24) + 4);`
  - `v29 = *(CharacterData::GetItem(this: v26, a2: v25, a3: 1, a4: -100 - v24) + 4);`
  - `CUIToolTip::SetToolTip_Equip2(this: (this + 112), a2: (a2 + v10), a3: v16, a4: v17, a5: v29, a6: 1, a7: 0);`

### CUIFadeYesNo::CreateAllianceInvite
  - `CFadeWnd::CreateFadeWnd(this, a2: 196, a3: 44, a4: v15, a5: 10, a6: 1, a7: 0);`

### CUIFadeYesNo::CreateFamilyInvite
  - `CFadeWnd::CreateFadeWnd(this, a2: 196, a3: 44, a4: v15, a5: 10, a6: 1, a7: 0);`

### CUIFadeYesNo::CreateFriendReg
  - `CFadeWnd::CreateFadeWnd(this, a2: 196, a3: 44, a4: v15, a5: 10, a6: 1, a7: 0);`

### CUIFadeYesNo::CreateGuildInvite
  - `CFadeWnd::CreateFadeWnd(this, a2: 196, a3: 44, a4: v16, a5: 10, a6: 1, a7: 0);`

### CUIFadeYesNo::CreateMSMInvite
  - `CFadeWnd::CreateFadeWnd(this, a2: 196, a3: 44, a4: v16, a5: 10, a6: 1, a7: 0);`

### CUIFadeYesNo::CreateNewYearCardArrived
  - `CFadeWnd::CreateFadeWnd(this, a2: 155, a3: 44, a4: v15, a5: 10, a6: 1, a7: 0);`

### CUIFadeYesNo::CreateParcelAlarm
  - `CFadeWnd::CreateFadeWnd(this, a2: 155, a3: 44, a4: v16, a5: 10, a6: 1, a7: 0);`

### CUIFadeYesNo::CreatePartyInvite
  - `CFadeWnd::CreateFadeWnd(this, a2: 196, a3: 44, a4: v16, a5: 10, a6: 1, a7: 0);`

### CUIFadeYesNo::CreateQuestClear
  - `CFadeWnd::CreateFadeWnd(this, a2: 155, a3: 44, a4: v15, a5: 10, a6: 1, a7: 0);`

### CUIFadeYesNo::CreateTradeInvite
  - `CFadeWnd::CreateFadeWnd(this, a2: v5 != 0 ? 160 : 196, a3: 44, a4: v17, a5: 10, a6: 1, a7: 0);`

### CUIFadeYesNo::CreateUserAlarm
  - `CFadeWnd::CreateFadeWnd(this, a2: 155, a3: 44, a4: v14, a5: 10, a6: 1, a7: 0);`

### CUIFadeYesNo::Delete
  - `CWvsContext::DeleteFadeWnd(this: v2, a2: v5, a3: 0, a4: 0, a5: v6, a6: v7);`

### CUIFadeYesNo::OnCreate
  - `PcCreateObject::IWzFont(sUOL: v16, pObj: this + 68, pUnkOuter: 0);`

### CUIFadeYesNo::OnMouseButton
  - `CFadeWnd::Close(this: (this - 1), a2: 1);`

### CUIItem::Draw
  - `v19 = *(CharacterData::GetItem(this: v3, a2: &v61.lVal, a3: *(this + 377), a4: i) + 4);`
  - `if ( ProperBulletPosition != 0 && CUIItem::GetItemSlotRect(this, a2: ProperBulletPosition, a3: &pvarg) != 0 )`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v56[0], a2: SBYTE4(v56[0]));`

### CUIItem::OnCreate
  - `CUIItem::SetArrangeButton(a1: *(this + 377), a2: *(this + *(this + 377) + 378));`

### CUIItem::OnDropMoney
  - `CUtilDlgEx::SetUtilDlgEx(this: v11, a2: 2, a3: 0, a4: v16, a5: v17, a6: v18);`
  - `CUtilDlgEx::SetUtilDlgEx_INPUT_NO(this: v22, a2: 10, a3: 10, a4: v12, a5: 0, a6: 10, a7: 0);`

### CUIItem::OnMouseButton
  - `CItemInfo::GetItemIcon(a1: &v50, a2: Data, bPetDead: 0, bWithShadow: 0);`
  - `CWndMan::BeginDragDrop(a1: v33 != 4 ? v33 : 0, a2: v32);`

### CUIItem::OnMouseMove
  - `v5 = *(CharacterData::GetItem(this: v4, a2: &v11, a3: *(this + 376), a4: SlotPositionFromPoint) + 4);`

### CUIItem::OnTabChanged
  - `CCtrlScrollBar::SetScrollRange(this: v4, a2: (v6 - 22) / 4 + 1);`
  - `CCtrlScrollBar::SetCurPos(this: *(this + 367), a2: dword_BF0EB4[*(this + 377)]);`
  - `CUIItem::SetArrangeButton(a1: *(this + 377), a2: *(this + *(this + 377) + 378));`

### CUIItem::SetLastestGetItemEffect
  - `ZXString<unsigned short>::_Release(lpAddend: (v131[0] - 12));`
  - `if ( v128 != 0 && IWzProperty::Getcount(a1: v128) != 0 )`
  - `if ( CUIItem::GetItemSlotRect(this: v52, a2: v52[391], a3: &v107) == 0 )`
  - `if ( v123 != 0 && TSecType<long>::GetData(a1: v123 + 12, a2: 0) == v52[390] )`

### CUIItemMaker::DoesSatisfyPreCondition
  - `(SkillLevel = CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v1, a3: 10000 * v3 + 1007, a4: 0),`

### CUIItemMaker::DrawMeso
  - `IWzCanvas::DrawTextA(this: v15, nLeft: v9 + 207, nTop: 226, sText: v19, pFont: v20, vAlpha: v21, vTabOrg: p_pvarg);`

### CUIItemMaker::GetTotalCostToDisassemble
  - `Data = TSecType<long>::GetData(a1: (v1 + 12), a2: 0);`

### CUIItemMaker::GetTotalCostToMake
  - `v4 += TSecType<long>::GetData(a1: (*v14 + 12), a2: v4) % 10 + 1;`
  - `CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v6, a3: 10000 * v7 + 1007, a4: 0);`

### CUIItemMaker::IsMatched_WithCurrentSetting
  - `if ( v21 == 0 || CUIItemMaker::DoesSatisfyPreCondition(a1: a2) == 0 )`
  - `if ( v7[7] != TSecType<long>::GetData(a1: (v6 + 12), a2: 0) )`
  - `while ( *v12 == 0 || (TSecType<long>::GetData(a1: (*v12 + 12), a2: v11) % 10000 / 100) >= 2 )`

### CUIItemMaker::OnButtonClicked
  - `if ( *(this + 364) == 1 && CUIItemMaker::ConfirmMake(a1: this) != 0 )`

### CUIItemMaker::OnCreate
  - `PcCreateObject::IWzFont(sUOL: v40, pObj: this + 848, pUnkOuter: 0);`

### CUIItemMaker::RequestItemMake
  - `|| *(this + 449) == 2 && CUIItemMaker::IsExistMakableItem(this) == 0`
  - `v14 = TSecType<long>::GetData(a1: (*(this + 384) + 12), a2: 3);`

### CUIItemMaker::SetComboBox_Category
  - `CCtrlComboBox::AddItem(this: *(this + 837), a2: dword_B37B68, a3: 999);`

### CUIItemMaker::Update
  - `else if ( v2 == 2 && get_update_time() - *(this + 834) >= 5100 )`

### CUIKeyConfig::0CNoticeDlg
  - `CDialog::CreateDlg(this, a2: v11 + v12 - 133, a3: v16, a4: 266, a5: 116, a6: 10, a7: 1, a8: 0);`
  - `CWnd::SetBackgrnd(a1: v17, a2: 0, a3: 0);`

### CUIKeyConfig::OnButtonClicked
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444) != 0 )`
  - `memset(a1: (dword_BED5A0 + 4), a2: 0, a3: 0x1BDu);`

### CUIKeyConfig::OnMouseMove
  - `if ( ShortCutIndexByPos > 90 || ShortCutIndexByPos == -1 )`
  - `CUIToolTip::SetToolTip_Skill(this: this + 112, arg0: (a2 + v7), a3: v18, a4: Skill, a5: 1);`

### CUIKeyConfig::TryBeginDragFuncKeyMappedIcon
  - `if ( CSkillInfo::GetSkillLevel(a1: v157, a2: *&v160[1], a3: &v162) <= 0 )`
  - `CWndMan::BeginDragDrop(a1: v68 != 0 ? v68 + 4 : 0, a2: v64);`
  - `ItemIcon = CItemInfo::GetItemIcon(&a1, a2: *&v160[1], bPetDead: 0, bWithShadow: 0);`

### CUIMacroSys::OnButtonClicked
  - `Buffer = ZXString<char>::GetBuffer(a1: 1024, a2: 1);`

### CUIMacroSys::OnCreate
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 30), a2: 3);`
  - `CCtrlCheckBox::SetChecked(this: *(this + 32), a2: 0);`

### CUIMacroSys::OnMouseButton
  - `CUIMacroSys::GetIndexByPos(this: (this - 4), a2: a5, a3: a1, a4: 1, a5: v67, a6: &v63);`
  - `if ( Macro == 0 || (Skill = CSkillInfo::GetSkill(this: dword_BE78DC, a2: Macro)) == 0 )`
  - `CWndMan::BeginDragDrop(a1: v46 != 4 ? v46 : 0, a2: v43);`

### CUIMacroSys::OnMouseMove
  - `if ( v6 != 0 && CUIMacroSys::GetIndexByPos(this: v31, a2, a3, a4: 1, a5: &v33, a6: &v32) != 0 )`
  - `CUIToolTip::SetToolTip_Skill(this: this + 128, arg0: (a2 + v10), a3: v25, a4: Skill, a5: 1);`

### CUIMacroSysEx::OnMouseButton
  - `CUIMacroSysEx::GetIndexByPos(this: (this - 4), a2: a5, a3: a1, a4: 1, a5: v67, a6: &v63);`
  - `if ( Macro == 0 || (Skill = CSkillInfo::GetSkill(this: dword_BE78DC, a2: Macro)) == 0 )`
  - `CWndMan::BeginDragDrop(a1: v46 != 4 ? v46 : 0, a2: v43);`

### CUIMacroSysEx::OnMouseMove
  - `if ( v6 != 0 && CUIMacroSysEx::GetIndexByPos(this: v31, a2, a3, a4: 1, a5: &v33, a6: &v32) != 0 )`
  - `CUIToolTip::SetToolTip_Skill(this: this + 136, arg0: (a2 + v10), a3: v25, a4: Skill, a5: 1);`

### CUIMedalQuestInfo::Draw
  - `StringW = StringPool::GetStringW(this: v29, a2: v126[0], a3: SHIDWORD(v126[0]));`
  - `IWzCanvas::SetClipRect(pvargDest: &v127, a2: 7, a3: 48, a4: 230, a5: 291, a6: &v137);`
  - `String = StringPool::GetString(this: v64, result: v126[0], nIdx: SHIDWORD(v126[0]));`
  - `v62 = StringPool::GetString(this: v61, result: v126[0], nIdx: SHIDWORD(v126[0]));`
  - `v84 = StringPool::GetStringW(this: v83, a2: v126[0], a3: SHIDWORD(v126[0]));`

### CUIMedalQuestInfo::OnCreate
  - `PcCreateObject::IWzFont(sUOL: v110, pObj: this + 379, pUnkOuter: 0);`
  - `CCtrlTab::SetTab(this: *(this + 365), a2: 100);`
  - `CCtrlTab::SetTab(this: *(this + 365), a2: *(this + 368));`

### CUIMedalQuestInfo::OnMouseButton
  - `CWnd::InvalidateRect(this: (this - 1), a2: 0);`

### CUIMedalQuestInfo::ToggleDetail
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1528) != 0 )`

### CUIQuestAlarm::Draw
  - `v247[0] = *StringPool::GetString(this: v59, result: v246, nIdx: v247[0]);`
  - `ZXString<char>::_Release(lpAddend: (a2[0] - 12));`

### CUIQuestAlarm::RemoveQuestByIndex
  - `return ZRef<CCtrlButton>::_ReleaseRaw(a1: this + 8 * a2 + 1508);`

### CUIQuestInfo::OnCreate
  - `PcCreateObject::IWzFont(sUOL: v114, pObj: this + 379, pUnkOuter: 0);`
  - `CCtrlTab::SetTab(this: *(this + 365), a2: 1);`
  - `CCtrlTab::SetTab(this: *(this + 365), a2: *(this + 368));`

### CUIQuestInfo::SetScrollBar
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 367), a2: v1 - 13);`

### CUIQuestInfo::SyncScrollBarWithSelected
  - `CCtrlScrollBar::SetCurPos(this: *(this + 367), a2: 0);`
  - `CCtrlScrollBar::SetCurPos(this: *(this + 367), a2: *(this + 369));`

### CUIQuestInfo::ToggleDetail
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1556) != 0 )`

### CUIQuestInfo::TryShowSuitableLevelQuest
  - `CCtrlTab::SetTab(this: *(this + 365), a2: 0);`

### CUIQuestInfoDetail::Draw
  - `IWzCanvas::SetClipRect(pvargDest: &v82, a2: 18, a3: 123, a4: 263, a5: v17 - 2, a6: &v85);`
  - `IWzCanvas::SetClipRect(pvargDest: &v82, a2: 18, a3: 249, a4: 263, a5: 108, a6: &v84);`

### CUIQuestInfoDetail::OnButtonClicked
  - `v4 = *(CUIQuestInfo::GetQuestInfo(this: *(this + 28), a2: v8) + 4);`

### CUIQuestInfoDetail::OnCreate
  - `PcCreateObject::IWzFont(sUOL: v21, pObj: this + 50, pUnkOuter: 0);`

### CUISkill::OnCreate
  - `CCtrlTab::SetTab(this: *(this + 366), a2: *(this + 361));`
  - `CCtrlTab::SetTab(this: *(this + 366), a2: v70 - 1);`

### CUISkill::OnSkillLevelUpButton
  - `&& (*(*(this + 366) + 60) != 0 || get_novice_skill_point(cd: v4) > 0) )`
  - `if ( CWvsContext::GetSkillLevelUpState(this: v38, a2: v8) == 1 && *(v8 + 11) == 0 )`

### CUISkillEx::OnSkillLevelUpButton
  - `&& (*(*(this + 366) + 60) != 0 || get_novice_skill_point(cd: v4) > 0) )`
  - `if ( CWvsContext::GetSkillLevelUpState(this: v10, a2: v8) == 1 && *(v8 + 11) == 0 )`

### CUIStat::Draw
  - `v18 = *StringPool::GetString(this: Instance, result: v210[0], nIdx: SHIDWORD(v210[0]));`
  - `String = StringPool::GetString(this: v33, result: v210[0], nIdx: SHIDWORD(v210[0]));`
  - `v44 = StringPool::GetString(this: v43, result: v210[0], nIdx: SHIDWORD(v210[0]));`
  - `v61 = *StringPool::GetString(this: v60, result: v210[0], nIdx: SHIDWORD(v210[0]));`
  - `*v215 = Ztl_variant_t::GetUnknown(a1: v210[0], a2: SBYTE4(v210[0]));`
  - `IWzCanvas::DrawTextA(this: v120, nLeft: 61, nTop: 262, sText: v194, pFont: v195, vAlpha: p_Hi, vTabOrg: v197);`
  - `IWzCanvas::DrawTextA(this: v137, nLeft: 61, nTop: 280, sText: v187, pFont: v188, vAlpha: v189, vTabOrg: pvarVal);`
  - `IWzCanvas::DrawTextA(this: v155, nLeft: 61, nTop: 298, sText: v180, pFont: v181, vAlpha: v182, vTabOrg: v183);`

### CUIStatDetail::Draw
  - `Data = TSecType<long>::GetData(a1: (v237 + 12), a2: 0);`
  - `v8 = *(CharacterData::GetItem(this: v252, a2: &pvargDest.lVal, a3: 2, a4: weapon_type) + 4);`
  - `&& TSecType<long>::GetData(a1: v8 + 12, a2: v8) / 1000 != 2331`
  - `&& TSecType<long>::GetData(a1: v8 + 12, a2: v8) / 1000 != 2332 )`
  - `v11 = TSecType<long>::GetData(a1: (v237 + 12), a2: 0);`
  - `v189 = TSecType<long>::GetData(a1: (*(v252 + 387) + 12), a2: v183);`

### CUIToolTip::DrawItemIcon
  - `v11 = (*(**Canvas + 140))(a1: *Canvas, a2, a3, a4: 68, a5: 68, a6: -1593835521);`

### CUIToolTip::DrawToolTip_Equip
  - `CUIToolTip::DrawItemIcon(this, a2: 10, a3: a2 + 32, a4: v71, a5: v10, a6: 0);`
  - `draw_number_by_image(pCanvas: v58, nLeft: 148, nTop: v61, nNo: Hi32, pBase: cyVal.Lo, nHorzSpace: 1);`
  - `draw_number_by_image(pCanvas: v59, nLeft: 148, nTop: v62, nNo: v78, pBase: cyVal.Lo, nHorzSpace: 1);`

### CUIToolTip::SetToolTip_Bundle
  - `CUIToolTip::MakeLayer(this: v103, a2: &a7, a3: a2, a4: a3, a5: 1, a6: 0, a7: 0, a8: -1610612672);`
  - `CUIToolTip::GetFontByType(a1: v164 + 4, a2: 1);`
  - `v126 = *StringPool::GetString(this: v125, result: v164[0], nIdx: SHIDWORD(v164[0]));`
  - `CUIToolTip::DrawItemIcon(this: v193, a2: 10, a3: a5 + 32, a4, a5: a6, a6: 0);`

### CUIToolTip::SetToolTip_Equip
  - `v31 = TSecType<long>::GetData(a1: p_Hi32 + 12, a2: 4);`
  - `v48 = TSecType<long>::GetData(a1: p_Hi32 + 12, a2: (p_Hi32 + 3));`
  - `v72 = TSecType<long>::GetData(a1: v64 + 12, a2: 0);`
  - `CUIToolTip::SetBasicInfo(this: v264, a2: 4, a3: 236, a4: result + v259 + 16 * v101 + v263 + 166, a5: 152);`
  - `CUIToolTip::AddInfoEx(a1: 14, a2: 16, a3: v242.vt, a4: v242.decVal.Hi32, a5: v242.lVal, a6: v242.cyVal.Hi);`
  - `Layer = CUIToolTip::MakeLayer(this: v151, a2: &a1, a3: nTop, a4: p_Mid32, a5: 1, a6: 0, a7: 0, a8: -1606418432);`
  - `&& (TSecType<long>::GetData(a1: a2, a2: 0) == 1822000 || TSecType<long>::GetData(a1: a2, a2: 0) == 1832000) )`
  - `if ( TSecType<long>::GetData(a1: a2, a2: 0) == 1822000 && v184 != 0 && (v185 = *(v184 + 136))[4] != 0 )`
  - `else if ( TSecType<long>::GetData(a1: a2, a2: 0) == 1832000 && v184 != 0 && *(*(v184 + 136) + 20) != 0 )`
  - `CUIToolTip::GetFontByType(a1: &v242.decVal.Lo64 + 4, a2: 11);`

### CUIToolTip::SetToolTip_EquipExt
  - `Data = TSecType<long>::GetData(a1: (a5 + 16), a2: 0);`
  - `CUIToolTip::SetBasicInfo(this: v44, a2: 10, a3: v41, a4: v25 + v26, a5: -1);`
  - `CUIToolTip::MakeLayer(this: v44, a2: &a3, a3: a2, a4: a3, a5: 1, a6: 0, a7: 0, a8: -1610612672);`

### CUIToolTip::SetToolTip_Equip_Basic
  - `v6 = TSecType<long>::GetData(a1: v3 + 12, a2: 0);`

### CUIToolTip::SetToolTip_MacroSysSkill
  - `CUIToolTip::SetBasicInfo(this, a2: 11, a3: 290, a4: 142, a5: 125);`
  - `CUIToolTip::MakeLayer(this, a2: &v41, a3: a2, a4: Unknown, a5: 1, a6: 0, a7: 0, a8: -1610612672);`

### CUIToolTip::SetToolTip_Package
  - `CUIToolTip::SetBasicInfo(this: v310, a2: 9, a3: 260, a4: 53 * v42 - v43 + 65, a5: -1);`
  - `CWvsContext::GetCommodityBySN(a1: v318, a2: *(*(a5 + 96) + 4 * v47));`
  - `Data = TSecType<long>::GetData(a1: (a5 + 16), a2: 0);`
  - `if ( TSecType<long>::GetData(a1: (v319 + 16), a2: 0) / 10000 == 190 )`
  - `v64 = TSecType<long>::GetData(a1: (v319 + 16), a2: 0);`
  - `CUIToolTip::MakeLayer(this: v56, a2: &v324, a3: arg0, a4: &a3->lVal, a5: 1, a6: 0, a7: 0, a8: -1610612672);`
  - `v68 = TSecType<long>::GetData(a1: (a5 + 16), a2: 0);`

### CUIToolTip::SetToolTip_Pet
  - `Data = TSecType<long>::GetData(a1: a4 + 12, a2: 0);`
  - `CUIToolTip::SetBasicInfo(this: v12, a2: 6, a3: 290, a4: v32 + v33 + v83 + 116, a5: -1);`
  - `CUIToolTip::MakeLayer(this: v12, a2: &a5, a3: a2, a4: a3, a5: 1, a6: 0, a7: 0, a8: -1610612672);`
  - `CUIToolTip::DrawItemIcon(this: v12, a2: 10, a3: v57, a4: v58, a5: p_pvargDest, a6: v68[0]);`

### CUIToolTip::SetToolTip_Ring
  - `Data = TSecType<long>::GetData(a1: pe + 12, a2: 0);`
  - `CUIToolTip::SetBasicInfo(this: v99, a2: 8, a3: 236, a4: v23 + 16 * v24 + v22 + 166, a5: 152);`
  - `v25 = TSecType<long>::GetData(a1: pe + 12, a2: 0);`
  - `CUIToolTip::MakeLayer(this: v42, a2: &v94, a3: x, a4: y, a5: 1, a6: 0, a7: 0, a8: -1610612672);`

### CUIToolTip::SetToolTip_Skill
  - `CUIToolTip::SetBasicInfo(this, a2: 7, a3: 320, a4: v12 + v13, a5: *v64 * (v14 + 4) + 114);`
  - `CUIToolTip::MakeLayer(this, a2: &a3, a3: arg0, a4: a3, a5: 1, a6: 0, a7: 0, a8: -1610612672);`

### CUIToolTip::SetToolTip_SlotInc
  - `v50 = TSecType<long>::GetData(a1: (a5 + 16), a2: 0) / 1000 % 10;`
  - `Data = TSecType<long>::GetData(a1: (a5 + 16), a2: 0);`
  - `v11 = TSecType<long>::GetData(a1: (a5 + 16), a2: 0);`
  - `if ( TSecType<long>::GetData(a1: (a5 + 16), a2: 0) / 1000 == 5430 )`
  - `CUIToolTip::SetBasicInfo(this: v55, a2: 10, a3: *v52, a4: v37 + v38, a5: -1);`
  - `CUIToolTip::MakeLayer(this: v55, a2: &a3, a3: a2, a4: a3, a5: 1, a6: 0, a7: 0, a8: -1610612672);`

### CUIToolTip::SetToolTip_String
  - `CUIToolTip::SetBasicInfo(this: v25, a2: 1, a3: v8 + 8, a4: v19, a5: p_pvargDest);`
  - `CUIToolTip::MakeLayer(this: v25, a2: &v24, a3: a2, a4: a3, a5: 0, a6: 0, a7: 0, a8: -1610612672);`
  - `IWzCanvas::DrawTextA(this: v13, nLeft: 4, nTop: 1, sText: v17, pFont: p_pvarg, vAlpha: v19, vTabOrg: p_pvargDest);`

### CUIToolTip::SetToolTip_String2
  - `CUIToolTip::GetFontByType(this, a2: &v38, a3: a8 != 0 ? 7 : 1);`
  - `CUIToolTip::SetBasicInfo(this: v36, a2: 2, a3: v37, a4: v23, a5: -1);`

### CUIToolTip::SetToolTip_String_MultiLine
  - `CUIToolTip::SetBasicInfo(this, a2: 1, a3: v11, a4: v10, a5: -1);`
  - `CUIToolTip::MakeLayer(this, a2: &a5, a3: a2, a4: a3, a5: 0, a6: 0, a7: 0, a8: -1610612672);`

### CUIToolTip::SetToolTip_WorldMap
  - `CUIToolTip::SetBasicInfo(this: v39, a2: 3, a3: v10, a4: v12, a5: -1);`
  - `CUIToolTip::MakeLayer(this: v39, a2: &a3, a3: a2, a4: a3, a5: 1, a6: 0, a7: 0, a8: -1610612672);`

### CUIToolTip::ShowItemToolTip
  - `Data = TSecType<long>::GetData(a1: a4 + 12, a2: (a4 + 12));`

### CUIUserInfo::Draw
  - `v14 = *StringPool::GetString(this: Instance, result: v330[0], nIdx: SHIDWORD(v330[0]));`
  - `v29 = *StringPool::GetString(this: v28, result: v330[0], nIdx: SHIDWORD(v330[0]));`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v330[0], a2: SBYTE4(v330[0]));`
  - `ItemName = CItemInfo::GetItemName(a1: &v335, a2: *(*(v2 + *(v2 + 417) + 400) + 4 * v352));`
  - `CItemInfo::GetItemInfo(a1: &v350, a2: *(*(v346 + *(v346 + 417) + 400) + 4 * v352));`
  - `HIDWORD(v330[0]) = get_int32(pvarSrc: Item, a2: SHIDWORD(v330[0]));`
  - `a2 = Ztl_variant_t::GetUnknown(a1: v330[0], a2: SBYTE4(v330[0]));`
  - `v223 = StringPool::GetString(this: v222, result: v330[0], nIdx: SHIDWORD(v330[0]));`
  - `QuestDisplayMedal = CQuestMan::GetQuestDisplayMedal(this: dword_BED614, a2: *(*(String + 4) + 2 * v303));`
  - `v354 = Ztl_variant_t::GetUnknown(a1: v330[0], a2: SBYTE4(v330[0]));`
  - `v185 = CItemInfo::GetItemName(a1: &v353, a2: *(*(v2 + 403) + 4 * v352));`
  - `CItemInfo::GetItemInfo(a1: &v343, a2: *(*(v346 + 403) + 4 * v352));`
  - `HIDWORD(v330[0]) = get_int32(pvarSrc: v204, a2: SHIDWORD(v330[0]));`

### CUIUserInfo::SetAvatarInfo
  - `CAvatar::SetRidingVehicle(this: *(this + 407), a2: *(this + 435), a3: v31);`
  - `CAvatar::SetVisibleMan(this: *(this + 407), a2: 0);`

### CUIUserInfo::SetPetItemList
  - `ZArray<long>::RemoveAll(a1: this + 4 * a2 + 1600);`

### CUIUserInfoDetail::Draw
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v60[0], a2: SBYTE4(v60[0]));`
  - `HIDWORD(v60[0]) = get_int32(pvarSrc: Item, a2: SHIDWORD(v60[0]));`

### CUIUserInfoWishList::Draw
  - `if ( v65 != 0 && TSecType<long>::GetData(a1: (v65 + 16), a2: 0) != 0 )`
  - `v8 = TSecType<long>::GetData(a1: (v65 + 16), a2: 0);`

### CUIUserInfoWishList::OnCreate
  - ``eh vector constructor iterator'(a1: a1 + 31, a2: 8u, a3: 3, a4: sub_47391D, a5: ZRef<CCtrlButton>::_ReleaseRaw);`

### CWorldMapDlg::CheckSpotInfo
  - `CUIToolTip::SetToolTip_WorldMap(this: this + 34, a2: (v26 + a2 + 20), a3: v28, a4: v29, a5: v30, a6: v31);`

### CWorldMapDlg::CreateWorldMapDlg
  - `CDialog::CreateDlg(this, a2: 67, a3: 0, a4: 666, a5: 524, a6: 10, a7: 1, a8: 0);`

### CWorldMapDlg::Draw
  - `v35 = v33[1] - IWzCanvas::Getcy(this: v67) + 35;`

### CWorldMapDlg::LoadMapList
  - `if ( v125 != 0 && IWzProperty::Getcount(a1: v125) != 0 )`
  - `if ( v124 == 0 || IWzProperty::Getcount(a1: v124) == 0 )`
  - `if ( v117[368] != 0 && v23[8] == 0 && CNpcTemplate::GetNpcTemplate(a1: v117[368]) != 0 )`
  - `v63 = CField::GetFieldProp(a1: &v99, a2: **v56)[1] == 0;`

### CWorldMapDlg::SetWorldMap
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v91[0], a2: SBYTE4(v91[0]));`
