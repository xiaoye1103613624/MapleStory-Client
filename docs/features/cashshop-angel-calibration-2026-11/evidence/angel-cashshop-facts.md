# Angel 083 商城现金交易面反编译语义抽取（观察层）

来源：`evidence/angel-cashshop-decompile-full.json`（逐字 Hex-Rays 伪代码，超大函数从 `/output/` 端点补全并归一化）。
本文件由 `tools/ida_angel_cashshop_facts.py` 自动生成，只做字符串抽取，不含推断。

## 1. StringPool 资源节点全集（266 个）

- `SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK` ← CCSWnd_Best, CCSWnd_Char, CCSWnd_List, CCSWnd_Tab, CCashShop, CCashTradingRoomDlg…
- `SP_100_MAPLESTORY_ERROR__LOADING_REACTOR_FAILED` ← CCashTradingRoomDlg
- `SP_104_MAPLESTORY_ERROR__SPRITE_INIT_FAILEDUNKNOWN` ← CCSWnd_List, CCSWnd_Tab
- `SP_10_EQP` ← CCSWnd_Inventory
- `SP_116_MAPLESTORY_ERROR__MOB_ERROR` ← CUIRecommendWorld
- `SP_11_SETUP` ← CCSWnd_Char, CCSWnd_Inventory
- `SP_1206_UI_CASHSHOPIMG_CSCHAR_BTBUYAVATAR` ← CCSWnd_Char
- `SP_1207_UI_CASHSHOPIMG_CSCHAR_BTDEFAULTAVATAR` ← CCSWnd_Char
- `SP_1210_UI_CASHSHOPIMG_CSINVENTORY_BTEXEQUIP` ← CCSWnd_Inventory
- `SP_1211_UI_CASHSHOPIMG_CSINVENTORY_BTEXCONSUME` ← CCSWnd_Inventory
- `SP_1212_UI_CASHSHOPIMG_CSINVENTORY_BTEXINSTALL` ← CCSWnd_Inventory
- `SP_1213_UI_CASHSHOPIMG_CSINVENTORY_BTEXETC` ← CCSWnd_Inventory
- `SP_1215_UI_CASHSHOPIMG_CSINVENTORY_BTEXTRUNK` ← CCSWnd_Inventory
- `SP_1216_UI_CASHSHOPIMG_CSTAB_TAB_D` ← CCSWnd_Tab
- `SP_1217_UI_CASHSHOPIMG_CSLIST_BASE` ← CCSWnd_List
- `SP_1218_UI_CASHSHOPIMG_CSLIST_ITEMICON` ← CCSWnd_List
- `SP_1219_UI_CASHSHOPIMG_PICTUREPLATE_HOWTO` ← CCSWnd_List
- `SP_1220_UI_CASHSHOPIMG_PICTUREPLATE_HOWTOGIFT` ← CCSWnd_List
- `SP_1221_UI_CASHSHOPIMG_PICTUREPLATE_NOITEM` ← CCSWnd_List
- `SP_1223_UI_CASHSHOPIMG_CSLIST_BTBUY` ← CCSWnd_List
- `SP_1224_UI_CASHSHOPIMG_CSLIST_BTGIFT` ← CCSWnd_List
- `SP_1225_UI_CASHSHOPIMG_CSLIST_BTRESERVE` ← CCSWnd_List
- `SP_1226_UI_CASHSHOPIMG_CSLIST_BTREMOVE` ← CCSWnd_List
- `SP_1227_UI_CASHSHOPIMG_CSSTATUS_BTCHARGE` ← CCSWnd_Status
- `SP_1228_UI_CASHSHOPIMG_CSSTATUS_BTCHECK` ← CCSWnd_Status
- `SP_1229_UI_CASHSHOPIMG_CSSTATUS_BTCOUPON` ← CCSWnd_Status
- `SP_1230_UI_CASHSHOPIMG_CSSTATUS_BTEXIT` ← CCSWnd_Status
- `SP_1231_TAB` ← CCSWnd_Tab
- `SP_1232_DLGNOTICE` ← CCouponUseSelectDlg
- `SP_1233_BUYSHOPITEM` ← CCashShop
- `SP_1249_SOUND_BGMUIIMG_SHOPBGM` ← CCashShop
- `SP_1253_UI_BASICIMG_ITEMNO` ← CCSWnd_Inventory, CCSWnd_Locker
- `SP_1257_UI_BASICIMG_BTOK2` ← CConfirmPurchaseDlg
- `SP_1258_UI_BASICIMG_BTCANCEL2` ← CConfirmPurchaseDlg
- `SP_1273_STRING_TOOLTIPHELPIMG_SHOP_BUTTON_BUYAVATAR` ← CCSWnd_Char
- `SP_1274_STRING_TOOLTIPHELPIMG_SHOP_BUTTON_DEFAULTAVATAR` ← CCSWnd_Char
- `SP_1280_STRING_TOOLTIPHELPIMG_SHOP_BUTTON_CHARGE` ← CCSWnd_Status
- `SP_1281_STRING_TOOLTIPHELPIMG_SHOP_BUTTON_CHECK` ← CCSWnd_Status
- `SP_1282_STRING_TOOLTIPHELPIMG_SHOP_BUTTON_COUPON` ← CCSWnd_Status
- `SP_1283_UI_UIWINDOWIMG_COUPON_BACKGRND` ← CCouponUseSelectDlg
- `SP_1300_UI_UIWINDOWIMG_FLOATNOTICE_D` ← CCSWnd_Char
- `SP_1301_UI_CASHSHOPIMG_BASE_BACKGRND` ← CCashShop
- `SP_1302_UI_CASHSHOPIMG_BASE_BACKGRND1` ← CCashShop
- `SP_1315_UI_LOGINIMG_CHARSELECT_PAGEL_` ← CUIAvatar
- `SP_1316_UI_LOGINIMG_CHARSELECT_PAGER_` ← CUIAvatar
- `SP_1317_UI_LOGINIMG_CHARSELECT_CHARACTER_0` ← CUIAvatar
- `SP_1318_UI_LOGINIMG_CHARSELECT_CHARACTER_1` ← CUIAvatar
- `SP_1323_UI_LOGINIMG_CHARSELECT_NAMETAG` ← CUIAvatar, CUIAvatarVAC
- `SP_1324_UI_LOGINIMG_CHARSELECT_KNIGHT` ← CUIAvatar
- `SP_1366_UI_LOGINIMG_WORLDSELECT_WORLD_D` ← CUIRecommendWorld
- `SP_1385_ETC_CATEGORYIMG` ← CCashShop
- `SP_1397_E` ← CCSWnd_List
- `SP_1398_W` ← CCSWnd_List
- `SP_1410_CANVASFONT` ← CCSWnd_Char, CUIAvatar, CUIAvatarVAC
- `SP_1411_BA` ← CCSWnd_Char
- `SP_1486_FOOTHOLD` ← CCSWnd_Char
- `SP_1487_LADDERROPE` ← CCSWnd_Char
- `SP_1515_TYPE` ← CCSWnd_Char
- `SP_153_S_IS_AT_S` ← CUIUserInfoWishList
- `SP_1587_MSG` ← CCSWnd_Char
- `SP_1614_SPEED` ← CCSWnd_Char
- `SP_1644_EFFECT` ← CCSWnd_Char
- `SP_1802_FLOATTYPE` ← CCSWnd_Char
- `SP_1803_PATH` ← CCSWnd_Char
- `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID` ← CCSWnd_Char
- `SP_2302_MENUUP` ← CUIAdminShopWishList
- `SP_2422_CHATBALLOON` ← CCSWnd_Char
- `SP_2423_NAMETAG` ← CCSWnd_Char
- `SP_2505_ITEMS_ARE_NOT_AVAILABLE_FOR_PURCHASE_R_N_AT_THIS_HOUR` ← CCashShop
- `SP_2506_THE_ITEM_IS_OUT_OF_STOCK_AND_THEREFORE_R_NNOT_AVAILABLE_FOR_SALE` ← CCashShop
- `SP_2508_UI_CASHSHOPIMG_GUIDEWORDS_D` ← CCSWnd_List
- `SP_2695_D_NX` ← CConfirmPurchaseDlg
- `SP_2697_UI_CASHSHOPIMG_CSNOTICE_0` ← CConfirmPurchaseDlg
- `SP_2727_SAMPLE` ← CUserPreview
- `SP_2853_UI_CASHSHOPIMG_CSICON_D` ← CCSWnd_List
- `SP_2864_THE_OPTION_IS_NOT_AVAILABLE_FOR_THAT_PACKAGE` ← CCashShop
- `SP_2879_UI_CASHSHOPIMG_CSDISCOUNT` ← CCSWnd_List
- `SP_2902_UI_CASHSHOPIMG_CSCHAR_BTTAKEOFFAVATAR` ← CCSWnd_Char
- `SP_2903_STRING_TOOLTIPHELPIMG_SHOP_BUTTON_TAKEOFFAVATAR` ← CCSWnd_Char
- `SP_2964_YOU_MAY_NOT_PURCHASE_THE_ITEM_R_NSINCE_YOU_ALREADY_HAVE_IT` ← CCashShop
- `SP_3119_UI_CASHSHOPIMG_CSEFFECT_EFFECT_0` ← CCSWnd_List
- `SP_3120_UI_CASHSHOPIMG_CSEFFECT_EFFECT_1` ← CCSWnd_List
- `SP_3129_UI_CASHSHOPIMG_BASE_TAB_ENABLE` ← CCSWnd_Char
- `SP_3130_UI_CASHSHOPIMG_BASE_TAB_DISABLE` ← CCSWnd_Char
- `SP_3131_UI_CASHSHOPPREVIEWIMG` ← CCSWnd_Char
- `SP_3133_UI_CASHSHOPIMG_BASE_PREVIEWONOFF_ON` ← CCSWnd_Char
- `SP_3134_UI_CASHSHOPIMG_BASE_PREVIEWONOFF_OFF` ← CCSWnd_Char
- `SP_3174_UI_CASHSHOPIMG_BASE_TAB2_ENABLE` ← CCSWnd_Inventory
- `SP_3175_UI_CASHSHOPIMG_BASE_TAB2_DISABLE` ← CCSWnd_Inventory
- `SP_3274_THIS_IS_THE_NX_COUPON_NUMBER_R_NREGISTER_YOUR_COUPON_AT_WWWNEXONNET` ← CCashShop
- `SP_3275_THIS_COUPON_IS_ONLY_AVAILABLE_TO_THE_USERS_BUYING_CASH_ITEM_FOR_THE_FIRST_TIME` ← CCashShop
- `SP_3441_PURCHASE_SUCCESSFUL_R_ND_YOU_HAVE_RECEIVED_MAPLEPOINTS` ← CCashShop
- `SP_3468_THE_OWNER_OF_THE_STORE_IS_CURRENTLY_R_NUNDERGOING_STORE_MAINTENANCE_R_NPLEASE_TR` ← CUIShopScanResult
- `SP_3512_OO` ← CCSWnd_Char
- `SP_3513_INFO` ← CCSWnd_Char
- `SP_3524_UI_CASHSHOPIMG_PICTUREPLATE_SHORTCUTHELP` ← CCSWnd_List
- `SP_3558_YOU_HAVE_ALREADY_APPLIED_FOR_THIS` ← CCashShop
- `SP_3559_____R_N10___R_N33333_____R_N___R_N____R_N_` ← CCSWnd_Best
- `SP_3560_33333_____R_N______R_N__R_N___D_` ← CCSWnd_Best
- `SP_3628__S__R_NWOULD_YOU_LIKE_TO_REGISTER` ← CUIAdminShopWishListSearchResult
- `SP_3640_THIS_STORE_IS_OPEN_AT_ROOM_D_ON_CHANNEL_S` ← CUIShopScanResult
- `SP_3641_THIS_CAN_ONLY_BE_USED_INSIDE_THE_FREE_MARKET` ← CUIShopScanResult
- `SP_3650__ITEM_PER_BUNDLE` ← CUIShopScanResult
- `SP_3651_PRICE_PER_BUNDLE` ← CUIShopScanResult
- `SP_3652_TOTAL__SOLD` ← CUIShopScanResult
- `SP_3745_THIS_MEGAPHONE_IS_ONLY_AVAILABLE_FOR__R_NCHARACTERS_THAT_ARE_OVER_LEVEL_10` ← CCSWnd_Best
- `SP_3746_HIRED_MERCHANTS_SHOP_IS_ONLY_AVAILABLE_FOR__R_NCHARACTERS_THAT_ARE_OVER_LEVEL_15` ← CCSWnd_Best, CCashShop
- `SP_3748_THIS_TELEPORT_ROCK_IS_ONLY_AVAILABLE_FOR__R_NCHARACTERS_THAT_ARE_OVER_LEVEL_6` ← CCSWnd_Best
- `SP_3749_THIS_ITEM_CAN_ONLY_BE_USED_15_DAYS__R_NAFTER_THE_ACCOUNTS_REGISTRATION` ← CCSWnd_Best, CCashShop
- `SP_3781_DIRECTION` ← CCSWnd_Char
- `SP_3899_SUCCESSFULLY_DELETED` ← CParcelDlg
- `SP_3900_SUCCESSFULLY_CLAIMED` ← CParcelDlg
- `SP_3902_A_NEW_PACKAGE_HAS_BEEN_SENT` ← CParcelDlg
- `SP_394_THE_ROOM_IS_ALREADY_CLOSED` ← CUIShopScanResult
- `SP_395_YOU_CANT_ENTER_THE_ROOM_DUE_TO_FULL_CAPACITY` ← CUIShopScanResult
- `SP_396_OTHER_REQUESTS_ARE_BEING_FULFILLED_THIS_MINUTE` ← CUIShopScanResult
- `SP_3974_UI_UIWINDOWIMG_AVATARMEGAPHONE` ← CUIAvatarMegaphone
- `SP_397_YOU_CANT_DO_IT_WHILE_YOURE_DEAD` ← CUIShopScanResult
- `SP_3983_UI_LOGINIMG_VIEWALLCHAR_BTENTER` ← CUIAvatarVAC
- `SP_3986_UI_LOGINIMG_VIEWALLCHAR_WORLDICONS_D` ← CUIAvatarVAC
- `SP_3987_UI_LOGINIMG_VIEWALLCHAR_BACKGRND` ← CUIAvatarVAC
- `SP_3991_WOULD_YOU_LIKE_TO_GIFT_THIS_ITEM_TO_SOMEONE_R_N__S__` ← CCashShop
- `SP_3992_YOU_DO_NOT_HAVE_ENOUGH_GIFT_TOKENS__R_NIN_YOUR_ACCOUNT_PLEASE_CHARGE_YOUR_ACCOUN` ← CCashShop
- `SP_3993_DUE_TO_TECHNICAL_DIFFICULTIES_R_NTHIS_ITEM_CANNOT_BE_SENT_AT_THIS_TIME_R_NPLEASE` ← CCashShop
- `SP_3994_YOU_MAY_NOT_GIFT_ITEMS_FOR__R_NIT_HAS_BEEN_LESS_THAN_TWO_WEEKS__R_NSINCE_YOU_FIR` ← CCashShop
- `SP_3995_USERS_WITH_HISTORY_OF_ILLEGAL_ACTIVITIES_R_N_MAY_NOT_GIFT_ITEMS_TO_OTHERS_PLEASE` ← CCashShop
- `SP_3996_DUE_TO_LIMITATIONS__R_NYOU_MAY_NOT_GIFT_THIS_ITEM_AS_THIS_TIME__R_NPLEASE_TRY_AG` ← CCashShop
- `SP_3997_YOU_HAVE_EXCEEDED_THE_AMOUNT_OF_TIME__R_NYOU_CAN_GIFT_ITEMS_TO_OTHER_CHARACTERS` ← CCashShop
- `SP_3998_THIS_ITEM_CANNOT_BE_GIFTED__R_NDUE_TO_TECHNICAL_DIFFICULTIES__R_NPLEASE_TRY_AGAI` ← CCashShop
- `SP_399_THIS_CHARACTER_IS_UNABLE_TO_DO_IT` ← CUIShopScanResult
- `SP_4002_YOU_CANNOT_TRANSFER_A_CHARACTER_UNDER_LEVEL_20` ← CCashShop
- `SP_4005_YOU_CANNOT_TRANSFER_A_CHARACTER__R_NTO_THE_SAME_WORLD_IT_IS_CURRENTLY_IN` ← CCashShop
- `SP_4006_YOU_CANNOT_TRANSFER_A_CHARACTER__R_NINTO_THE_NEW_SERVER_WORLD` ← CCashShop
- `SP_4007_YOU_MAY_NOT_TRANSFER_OUT_OF_THIS__R_NWORLD_AT_THIS_TIME` ← CCashShop
- `SP_4008_YOU_CANNOT_TRANSFER_A_CHARACTER_INTO__R_NA_WORLD_THAT_HAS_NO_EMPTY_CHARACTER_SLO` ← CCashShop
- `SP_400_YOU_ARE_NOT_ALLOWED_TO_TRADE_OTHER_ITEMS_AT_THIS_POINT` ← CUIShopScanResult
- `SP_4020_THE_EVENT_HAS_EITHER_ENDED_OR_R_NTHIS_ITEM_IS_NOT_AVAILABLE_FOR_FREE_TESTING` ← CCashShop
- `SP_406_TRADE_CANCELLED_R_NBY_THE_OTHER_CHARACTER` ← CCashTradingRoomDlg
- `SP_407_TRADE_SUCCESSFUL_R_NPLEASE_CHECK_THE_RESULTS` ← CCashTradingRoomDlg
- `SP_408_TRADE_SUCCESSFUL_R_NRECEIVED_D_MESOS_AFTER_FEES_R_NPLEASE_CHECK_THE_RESULTS` ← CCashTradingRoomDlg
- `SP_409_TRADE_UNSUCCESSFUL` ← CCashTradingRoomDlg
- `SP_40_ARCH_MAGEICELIGHTNING` ← CCashShop
- `SP_410_YOU_CANNOT_MAKE_THE_TRADE_BECAUSE_R_NTHERE_ARE_SOME_ITEMS_WHICH_R_NYOU_CANNOT_CA` ← CCashTradingRoomDlg
- `SP_411_YOU_CANNOT_MAKE_THE_TRADE_BECAUSE_R_NTHE_OTHER_PERSONS_ON_A_DIFFERENT_MAP` ← CCashTradingRoomDlg
- `SP_412_HOW_MUCH_MONEY_WILL_YOU_PUT_UP` ← CCashTradingRoomDlg
- `SP_413_ARE_YOU_SURE_YOU_WANT_TO_TRADE` ← CCashTradingRoomDlg
- `SP_428_YOU_MAY_NOT_ENTER_THIS_STORE` ← CUIShopScanResult
- `SP_4421_USE_THE_TAB_KEY_TO_TOGGLE_THE_MENU` ← CUIAvatar
- `SP_4457_YOU_HAVE_EXCEEDED_THE_MAXIMUM_NUMBER_R_NOF_USAGE_PER_ACCOUNT_FOR_THIS_ACCOUNT_R_` ← CCashShop
- `SP_4537_YOU_CANNOT_SEND_A_GIFT_TO_YOUR_OWN_ACCOUNT_R_NPLEASE_PURCHASE_IT_AFTER_LOGGING_R` ← CCashShop
- `SP_4538_PLEASE_CONFIRM_WHETHER_R_NTHE_CHARACTERS_NAME_IS_CORRECT` ← CCashShop
- `SP_4539_THIS_ITEM_HAS_A_GENDER_RESTRICTION_R_NPLEASE_CONFIRM_THE_GENDER_OF_THE_RECIPIENT` ← CCashShop
- `SP_4540_THE_GIFT_CANNOT_BE_SENT_BECAUSE_R_NTHE_RECIPIENTS_INVENTORY_IS_FULL` ← CCashShop
- `SP_4666_YOU_CURRENTLY_DO_NOT_HAVE_ANY_ITEMS_THAT_R_NCAN_USE_THE_SCISSORS_OF_KARMA_R_NWOU` ← CCashShop
- `SP_4765_THE_BUY_NOW_PRICE_IS_LOWER_R_NTHAN_THE_STARTING_BID` ← CRegisterAuctionEntryDlg
- `SP_4801_PLEASE_ENTER_MORE_THAN_1_CHARACTER` ← CRegisterWishEntrySearchDlg
- `SP_4802_THE_SEARCH_YIELDED_0_RESULT` ← CRegisterWishEntrySearchDlg
- `SP_4842_THIS_IS_YOUR_SEARCH_RESULT` ← CCSWnd_Tab
- `SP_494_YOUR_JOB_LIMIT_DOES_NOT_ALLOW_R_NYOU_TO_USE_THIS_ITEM__R_NARE_YOU_SURE_YOU_WANT_` ← CCashShop
- `SP_495_YOUR_LEVEL_LIMIT_DOES_NOT_ALLOW_R_NYOU_TO_USE_THIS_ITEM_R_NARE_YOU_SURE_YOU_WANT` ← CCashShop
- `SP_496_YOUR_FAME_LIMIT_DOES_NOT_ALLOW_R_NYOU_TO_USE_THIS_ITEM_R_NARE_YOU_SURE_YOU_WANT_` ← CCashShop
- `SP_498_YOU_ALREADY_HAVE_THE_ITEMS_SELECTED_ARE_R_NYOU_SURE_YOU_WANT_TO_BUY_THE_ITEMS_AG` ← CCashShop
- `SP_499_YOU_MAY_NOT_USE_THIS_ITEM_WITH_R_NTHE_WEAPON_YOU_ARE_EQUIPPED_WITH_R_NWILL_YOU_C` ← CCashShop
- `SP_5017_YOU_HAVE_TO_QUIT_FAMILY__R_NTO_MOVE_TO_ANOTHER_WORLD` ← CCashShop
- `SP_501__R_NTHE_CURRENT_PET_YOU_HAVE_R_NCANNOT_WEAR_THIS_ITEM_R_NARE_YOU_SURE_YOU_WANT_T` ← CCashShop
- `SP_502_ONLY_PETS_ARE_ALLOWED_TO_WEAR_THIS_ITEM_R_NARE_YOU_SURE_YOU_WANT_TO_PURCHASE_THI` ← CCashShop
- `SP_503_THIS_ITEM_CANNOT_BE_USED_WITHOUT_A_PET_AND_R_NTHIS_ITEM_CANNOT_BE_USED_ON_A_DECE` ← CCashShop
- `SP_5046_YOU_CAN_NOT_USE_THIS_NAME_R_NPLEASE_CHECK_AGAIN` ← CUICharacterSaleDlg
- `SP_504__R_NTHIS_ITEM_CANNOT_BE_USED_BY_THE_R_NPET_YOURE_CURRENTLY_RAISING_R_NARE_YOU_SU` ← CCashShop
- `SP_505_THIS_IS_AN_ITEM_THAT_CANNOT_BE_USED_R_NIF_YOU_DO_NOT_HAVE_THE_MONSTER_R_NRIDING_` ← CCashShop
- `SP_5063_THIS_ITEM_CANNOT_BE_PURCHASED__R_NWITH_MAPLEPOINTS` ← CCashShop
- `SP_5064_SORRY_FOR_INCONVINENCE__R_NPLEASE_TRY_AGAIN` ← CCashShop
- `SP_5066_UI_UIWINDOWIMG_CASHGACHAPON_BTOPEN` ← CUICashItemGachapon
- `SP_5067_UI_UIWINDOWIMG_CASHGACHAPON_BTCLOSE` ← CUICashItemGachapon
- `SP_506_ARE_YOU_SURE_YOU_WANT_TO_BUY_THIS_ITEM` ← CCashShop
- `SP_507_TOTAL_D_NX` ← CCashShop
- `SP_508_MAKE_PURCHASE` ← CCashShop
- `SP_509_THE_PURCHASE_HAS_FAILED` ← CCashShop
- `SP_510_YOU_HAVE_PURCHASED_THE_FOLLOWING_ITEMS_R_NSOME_ITEMS_WERENT_PURCHASED` ← CCashShop
- `SP_5121_UI_LOGINIMG_CHARSELECT_ARAN` ← CUIAvatar
- `SP_5122_UI_LOGINIMG_CHARSELECT_ADVENTURE` ← CUIAvatar
- `SP_514_CHECK_YOUR_BIRTHDAY_CODE_AND_R_NPLEASE_TRY_AGAIN` ← CCashShop
- `SP_516_YOU_CANNOT_USE_THIS_NAME__R_NPLEASE_TRY_AGAIN` ← CCashShop
- `SP_5207_SOME_ITEMS_CANNOT_BE_TRADED_WITH_OTHER_USERS_AND_R_N` ← CCashShop
- `SP_5214_TIME_LIMIT_EXPIRED` ← CCashTradingRoomDlg
- `SP_5215_HAVE_YOU_CHECKED_THE_R_NTIME_LIMIT_ON_THE_ITEM` ← CCashTradingRoomDlg
- `SP_5216_DDAYS_DHOURS_AND_DMINUTES_LEFT` ← CCashTradingRoomDlg
- `SP_5277_CASH_ITEMS_BECOME_UNTRADABLE_R_NONCE_THEY_HAVE_BEEN_TRADED_AND__R_NMAPLE_POINTS_` ← CCashTradingRoomDlg
- `SP_5278_A_CASH_ITEM_TRADE_WAS_CANCELED_R_NDUE_TO_A_SERVER_CHECK` ← CCashTradingRoomDlg
- `SP_5279_TRADE_WAS_CANCELED_BECAUSE_THERE_WAS_AN_ITEM_R_NTHAT_DISAPPEARED_BECAUSE_IT_EXPI` ← CCashTradingRoomDlg
- `SP_527_YOU_ALREADY_HAVE_THIS_ITEM_ON_YOUR_WISH_LIST` ← CCashShop
- `SP_528_YOUR_WISH_LIST_IS_FULL` ← CCashShop
- `SP_529_YOU_HAVE_SUCCESSFULLY_MADE_THE_PURCHASE` ← CCashShop
- `SP_534_REQUEST_TIMED_OUT_R_NPLEASE_TRY_AGAIN` ← CCashShop
- `SP_535_YOU_DONT_HAVE_ENOUGH_CASH` ← CCashShop
- `SP_5361_UI_CASHSHOPIMG_BASE_BACKGRND2` ← CCashShop
- `SP_5362_UI_LOGINIMG_WORLDSELECT_ALERT_BACKGRD` ← CUIRecommendWorld
- `SP_5363_UI_LOGINIMG_WORLDSELECT_ALERT_BTARROWL` ← CUIRecommendWorld
- `SP_5364_UI_LOGINIMG_WORLDSELECT_ALERT_BTARROWR` ← CUIRecommendWorld
- `SP_5365_UI_LOGINIMG_WORLDSELECT_ALERT_BTCHOICE` ← CUIRecommendWorld
- `SP_536_YOU_CANT_BUY_SOMEONE_A_CASH_ITEM_GIFT_IF_YOURE_UNDER_14` ← CCashShop
- `SP_537_YOU_HAVE_EXCEEDED_THE_ALLOTTED_LIMIT_OF_PRICE_R_NFOR_GIFTS` ← CCashShop
- `SP_538_PLEASE_CHECK_AND_SEE_IF_YOU_HAVE_EXCEEDED_R_NTHE_NUMBER_OF_CASH_ITEMS_YOU_CAN_HA` ← CCashShop
- `SP_539_PLEASE_CHECK_AND_SEE_R_NIF_THE_NAME_OF_THE_CHARACTER_IS_WRONG_R_NOR_IF_THE_ITEM_` ← CCashShop
- `SP_540_YOU_HAVE_REACHED_THE_DAILY_MAXIMUM__R_NPURCHASE_LIMIT_FOR_THE_CASH_SHOP` ← CCashShop
- `SP_541_DUE_TO_GENDER_RESTRICTIONS_THE_COUPON__R_NIS_UNAVAILABLE_FOR_USE` ← CCashShop
- `SP_542_THIS_COUPON_WAS_ALREADY_USED` ← CCashShop
- `SP_5439_WITH_THE_WEAPON_THAT_S_CURRENTLY_HAS_EQUIPPED_R_NTHIS_ITEM_CANT_BE_USED_R_NDO_YO` ← CCashShop
- `SP_543_THIS_COUPON_HAS_EXPIRED` ← CCashShop
- `SP_544_THIS_COUPON_CAN_ONLY_BE_USED_AT_R_NNEXON_AFFILIATED_INTERNET_CAFES_R_NPLEASE_USE` ← CCashShop
- `SP_545_THIS_COUPON_IS_A_NEXON_AFFILIATED_INTERNET_CAFE_ONLY_COUPON_R_NAND_IT_HAD_ALREAD` ← CCashShop
- `SP_546_THIS_COUPON_IS_A_NEXON_AFFILIATED_INTERNET_CAFE_ONLY_COUPON_R_NAND_IT_HAD_ALREAD` ← CCashShop
- `SP_5471_THIS_CONTAINS_AN_ITEM_THAT_WILL_DISAPPEAR_ONCE__R_NTHE_EXPIRATION_DATE_HAS_PASSE` ← CCashShop
- `SP_5472_S_DOES_NOT_HAVE_AN_ITEM_THAT__R_NTHE_SCISSORS_OF_KARMA_CAN_BE_USED_ON_R_NDO_YOU_` ← CCashShop
- `SP_547_PLEASE_CHECK_AND_SEE_IF__R_NTHE_COUPON_NUMBER_IS_RIGHT` ← CCashShop
- `SP_548_YOU_ARE_DISCONNECTED__R_NFROM_CASH_SHOP__R_NBECAUSE_YOU_PUT_WRONG__R_NCOUPON_NUM` ← CCashShop
- `SP_5515_D` ← CCSWnd_List
- `SP_5517_S` ← CUIShopScanResult
- `SP_551_THIS_COUPON_IS_ONLY_FOR_REGULAR_ITEMS_AND__R_NITS_UNAVAILABLE_TO_GIVE_AWAY_AS_A_` ← CCashShop
- `SP_5527_ARIAL` ← CCSWnd_Char, CUIAvatar, CUIAvatarMegaphone, CUIAvatarVAC
- `SP_552_THIS_COUPON_IS_ONLY_FOR_MAPLESTORY_AND_R_NIT_CANNOT_BE_GIFTED_TO_OTHERS` ← CCashShop
- `SP_5536_DUE_TO_JOB_LIMITATIONS_S_CANT_R_NUSE_THIS_ITEM_R_NDO_YOU_STILL_WISH_TO_GIVE_IT_A` ← CCashShop
- `SP_5538_ETC` ← CCSWnd_Inventory
- `SP_553_THIS_ITEM_IS_ONLY_AVAILABLE_FOR_PURCHASE_BY_A_USER_AT_THE_PREMIUM_SERVICE_INTERN` ← CCashShop
- `SP_554_YOU_ARE_SENDING_A_GIFT_TO_AN_INVALID_RECIPIENT_R_NPLEASE_CHECK_THE_CHARACTER_NAM` ← CCashShop
- `SP_555_YOU_HAVE_EXCEEDED_THE_SPENDING_LIMIT_OF_NX` ← CCashShop
- `SP_5560_PLEASE_ENTER_THE_PRICE_R_NOF_OVER_110_NX` ← CRegisterAuctionEntryDlg
- `SP_5566_THE_GAME_FILE_HAS_BEEN_DAMAGED_R_NSO_YOU_CANNOT_PARTICIPATE_IN_AN_ITEM_TRADE_PLE` ← CCashTradingRoomDlg
- `SP_556_THE_CASH_SHOP_IS_UNAVAILABLE_R_NDURING_THE_BETA_TEST_PHASE_R_NWE_APOLOGIZE_FOR_Y` ← CCashShop
- `SP_5573_THIS_ITEM_CANNOT_BE_R_NPURCHASED_BY_ANYONE_UNDER_7` ← CCashShop
- `SP_5574_THIS_ITEM_CANNOT_BE_R_NRECEIVED_BY_ANYONE_UNDER_7` ← CCashShop
- `SP_557_DUE_TO_AN_UNKNOWN_ERROR_R_NTHE_REQUEST_FOR_CASH_SHOP_HAS_FAILED` ← CCashShop
- `SP_5596_USE` ← CCSWnd_Inventory
- `SP_5599_YOU_DO_NOT_HAVE_ENOUGH_MESOS` ← CCashShop
- `SP_5601_YOU_HAVE_GIVEN_A_GIFT_TO` ← CCashShop
- `SP_5603_YOUR_INVENTORY_IS_FULL` ← CCSWnd_Locker
- `SP_5611__R_NUSING_THE_COUPON` ← CCashShop
- `SP_5617_AND__R_N` ← CCashShop
- `SP_561_ALL_THE_GIFTS_HAVE_BEEN_SENT_R_ND_NX__R_NWAS_SPENT_IN_THE_PROCESS` ← CCashShop
- `SP_5624_DELAY` ← CUserPreview
- `SP_562_THE_GIFTS_COULD_NOT_BE_SENT` ← CCashShop
- `SP_5639_MESO` ← CCSWnd_Best
- `SP_563___S___R_NHAS_BEEN_SENT_TO_THE_CHARACTER_ABOVE_R_NIT_COULD_NOT_BE_SENT_TO_SEVERAL` ← CCashShop
- `SP_568_THIS_IS_YOUR_WISH_LIST` ← CCSWnd_Tab
- `SP_570_THE_ITEM_IS_BEING_PREPARED_WERE_SORRY_FOR_THE_INCONVENIENCE` ← CCSWnd_Tab
- `SP_571__NX` ← CCSWnd_Best
- `SP_572_D` ← CCSWnd_List, CUIUserInfoWishList
- `SP_574__PET_S` ← CCSWnd_List
- `SP_575_ADDING_SLOTS_AT_THE_STORAGE` ← CCSWnd_Inventory
- `SP_576_ADD_D_SLOTS_FOR_THE_STORAGE_CURRENT_D_SLOTS_R_NMAXIMUM_OF_48_SLOTS_AVAILABLED_NX` ← CCSWnd_Inventory
- `SP_578_ADD_S_SLOTS` ← CCSWnd_Inventory
- `SP_579_ADD_D_SLOTS_FOR_S_INVENTORY_CURRENT_D_SLOTS_R_NMAXIMUM_OF_D_SLOTS_AVAILABLED_NX` ← CCSWnd_Inventory
- `SP_583_PLEASE_CHECK_THE_NAME_OF_THE_RECEIVER` ← CCashShop
- `SP_585_YOU_HAVE_RECEIVED` ← CCashShop
- `SP_586__R_ND_S` ← CCashShop
- `SP_587_D_MAPLEPOINTS` ← CCashShop
- `SP_588_D_MESOS` ← CCashShop, CConfirmPurchaseDlg
- `SP_589_TO_S` ← CCashShop
- `SP_591_THE_ITEM_IS_NOT_ON_SALE_NOW` ← CCashShop
- `SP_592_ARE_YOU_SURE_YOU_WANT_TO_LEAVE_THE_CASH_SHOP` ← CCSWnd_Status
- `SP_593_YOU_DONT_HAVE_ENOUGH_NX_PREPAID` ← CCashShop
- `SP_853_PLEASE_CHECK_IF_YOUR_INVENTORY_IS_FULL_OR_NOT` ← CCashShop
- `SP_956_CANVAS` ← CConfirmPurchaseDlg
- `SP_958_SHAPE2DVECTOR2D` ← CCSWnd_Char, CUserPreview
- `SP_966_SS` ← CUIAvatarMegaphone
- `SP_978_Y` ← CCSWnd_Char

## 2. 商城现金交易类逐函数关键行


### CCashShop（66 函数）


#### CCashShop::BlowWeather  (133 B)

  - `void __thiscall CCashShop::BlowWeather(CCashShop *this, int a2)`
  - `CCSWnd_Char::BlowWeather(this: (this + 1364), a2);`

#### CCashShop::ChangeLimitGoodsState  (1215 B)

  - `int __thiscall CCashShop::ChangeLimitGoodsState(CCashShop *this, int a2, int a3)`
  - `v3 = *(this + 19);`
  - `if ( v3 == 0 || *(v3 - 4) == 0 )`
  - `v5 = (v3 + 4);`
  - `v6 = *(this + 19);`
  - `if ( v6 == 0 || v4 >= *(v6 - 4) )`
  - `while ( 1 )`
  - `if ( *v7 == 0 )`
  - `if ( *v7 == a2 )`
  - `if ( v9 >= 10 )`
  - `if ( a3 == 191 )`
  - `*(104 * v4 + v3 + 44) = 2;`
  - `if ( a3 != 192 )`
  - `*(104 * v4 + v3 + 44) = -1;`
  - `*(104 * v4 + v3 + 44) = 1;`

#### CCashShop::CheckTransferWorldPossible  (1374 B)

- SP_ 节点：`SP_5017_YOU_HAVE_TO_QUIT_FAMILY__R_NTO_MOVE_TO_ANOTHER_WORLD`
  - `int __stdcall CCashShop::CheckTransferWorldPossible(int a1)`
  - `if ( *dword_BE7918 == 0 )`
  - `ZXString<char>::Format(a1, a2: aUnknownErrorTy, a3: v5);`
  - `if ( *(*dword_BE7918 + 12360) != 0 && **(*dword_BE7918 + 12372) == *(*dword_BE7918 + 8352) )`
  - `ZXString<char>::Format(a1, a2: aGuildMasterCan, a3: v5);`
  - `if ( (TSecType<unsigned char>::GetData(this: (*dword_BE7918 + 8240)) & 1) != 0 )`
  - `ZXString<char>::Format(a1, a2: aGmCanNotTransf, a3: v5);`
  - `if ( *(v1 + 14376) != 0 || *(v1 + 14384) != 0 )`
  - `ZXString<char>::Format(a1, a2: v4, a3: v5);`

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

#### CCashShop::FindWishList  (1418 B)

  - `int __thiscall CCashShop::FindWishList(CCashShop *this, int a2)`
  - `UsableCommodityIndexBySN = CCashShop::GetUsableCommodityIndexBySN(this, &a2);`
  - `if ( UsableCommodityIndexBySN < 0 )`
  - `v3 = (*dword_BE7918 + 14048);`
  - `for ( i = (this + 1180); ; ++i )`
  - `if ( *i != 0 )`
  - `v4 = CCashShop::GetUsableCommodityIndexBySN(this, a2: i);`
  - `if ( v4 >= 0 )`
  - `v5 = *(*v3 + 8 * UsableCommodityIndexBySN + 4);`
  - `Data = TSecType<long>::GetData(a1: (*(8 * v4 + *v3 + 4) + 16), a2: v3);`
  - `if ( TSecType<long>::GetData(a1: (v5 + 16), a2: v3) == Data )`
  - `v7 = *(v6 + *v3 + 4);`
  - `v8 = *(*v3 + 8 * UsableCommodityIndexBySN + 4);`
  - `if ( *(v8 + 28) == *(v7 + 28) && *(v8 + 44) == *(v7 + 44) )`
  - `if ( ++v13 >= 10 )`

#### CCashShop::GetCategorySubIndex  (583 B)

  - `unsigned int __thiscall CCashShop::GetCategorySubIndex(CCashShop *this, int a2, int a3)`
  - `for ( i = 0; ; i += 28 )`
  - `v5 = *(this + 17);`
  - `if ( v5 == 0 || result >= *(v5 - 4) )`
  - `v6 = *(v5 + i);`
  - `if ( v6 == a2 && *(v7 + 4) == a3 )`

#### CCashShop::GetUsableCommodityIndexBySN  (2203 B)

  - `int __thiscall CCashShop::GetUsableCommodityIndexBySN(CCashShop *this, int *a2)`
  - `v2 = (*dword_BE7918 + 14048);`
  - `v3 = *(*dword_BE7918 + 14048);`
  - `if ( v3 != 0 )`
  - `v15 = *(v3 - 4);`
  - `if ( v15 > 0 )`
  - `if ( *(*v6 + 12) == *a2 )`
  - `while ( v4 < v15 );`
  - `if ( v5 )`
  - `v8 = *(v3 + 8 * v4 + 4);`
  - `if ( *(v8 + 68) != 0 )`
  - `v9 = *(v3 + 8 * v4 + 4);`
  - `if ( v8 != 0 )`
  - `InterlockedIncrement(lpAddend: (v8 + 4));`
  - `while ( 1 )`
  - `v10 = *(*v2 + 8 * v4 + 4);`
  - …（截断 16 行）

#### CCashShop::GiftWishItem  (11364 B)

- SP_ 节点：`SP_535_YOU_DONT_HAVE_ENOUGH_CASH`, `SP_591_THE_ITEM_IS_NOT_ON_SALE_NOW`
  - `void __thiscall CCashShop::GiftWishItem(CCashShop *this)`
  - `v2 = *(this + 16) == 0;`
  - `if ( v2 )`
  - `CCashShop::NoticeFailReason(this, a2: 195);`
  - `if ( *(this + 6) == 0 )`
  - `v3 = (*dword_BE7918 + 14048);`
  - `nCommSN = *(*dword_BE7918 + 14060);`
  - `v43 = (*dword_BE7918 + 14048);`
  - `ZXString<char>::operator=(a1: *dword_BE7918 + 14064);`
  - `UsableCommodityIndexBySN = CCashShop::GetUsableCommodityIndexBySN(this: v1, a2: &nCommSN);`
  - `LOBYTE(v55) = 1;`
  - `if ( UsableCommodityIndexBySN >= 0 )`
  - `sub_473CB6(a1: v6);`
  - `ZRef<CS_COMMODITY>::~ZRef<CS_COMMODITY>(this: &v39);`
  - `v7 = *(v6 + 4);`
  - `if ( *(v7 + 68) != 0 )`
  - …（截断 16 行）

#### CCashShop::GoTo  (333 B)

  - `void __thiscall CCashShop::GoTo(CCashShop *this, int a2, int a3, int a4)`
  - `CCashShop::OnChangedCategory(this, a2);`
  - `CCashShop::OnChangedCategorySub(this, a2: a3);`
  - `CCashShop::OnChangedPage(this, a2: a4 / 10);`
  - `CCSWnd_List::SetCursorToPlate(this: (this + 5564), a2: a4 % 10);`

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

#### CCashShop::IsControlingUserPreview  (107 B)

  - `int __thiscall CCashShop::IsControlingUserPreview(CCashShop *this)`
  - `return *(this + 334);`

#### CCashShop::IsLimitGoodsRemain  (1464 B)

  - `signed int __thiscall CCashShop::IsLimitGoodsRemain(_DWORD *this, int a2)`
  - `v3 = *(this + 19);`
  - `if ( v3 == 0 || v11 >= *(v3 - 4) )`
  - `v4 = (v3 + v2);`
  - `v6 = (v3 + v2 + 4);`
  - `while ( 1 )`
  - `if ( *v6 == 0 )`
  - `if ( *v6 == a2 )`
  - `if ( v5 >= 10 )`
  - `dword_BF03B0(a1: v10);`
  - `v8 = v10[3] + 100 * (v10[1] + 100 * v10[0]);`
  - `if ( (v7 & 2) != 0 && (v4[15] > v8 || v8 > v4[16]) )`
  - `if ( (v7 & 4) != 0 && v4[v10[2] + 19] == 0 )`
  - `if ( (v7 & 8) != 0 && (v4[17] > v8 || v8 > v4[18] - 1) )`
  - `if ( (v7 & 1) != 0 && v4[13] <= 0 )`

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

#### CCashShop::LoadData  (11392 B)

- SP_ 节点：`SP_1385_ETC_CATEGORYIMG`
  - `int __fastcall CCashShop::LoadData(CCashShop *this, int a2, int a3)`
  - `v4 = CInPacket::Decode1(this: a3);`
  - `*(this + 16) = v4;`
  - `if ( v4 != 0 )`
  - `v5 = CInPacket::DecodeStr(this: a3, a2: &Unknown);`
  - `v6 = (this + 1172);`
  - `LOBYTE(v66) = 2;`
  - `ZXString<char>::operator=(a1: v5);`
  - `LOBYTE(v66) = 1;`
  - `if ( Unknown != 0 )`
  - `if ( *v6 != 0 )`
  - `i = *(*v6 - 4);`
  - `v7 = ZXString<char>::Mid(a1: &Unknown, a2: 0, a3: 2u);`
  - `LOBYTE(v66) = 3;`
  - `ZXString<char>::operator=(a1: v7);`
  - `if ( i > 2 )`
  - …（截断 16 行）

#### CCashShop::NoticeFailReason  (16627 B)

- 分支 case：`163`, `165`, `166`, `167`, `168`, `169`, `170`, `171`, `172`, `173`, `176`, `178`, `179`, `180`, `181`, `182`, `183`, `184`, `185`, `186`, `187`, `188`, `189`, `190`, `191`, `192`, `193`, `194`, `195`, `196`, `199`, `200`, `205`, `208`, `210`, `211`, `212`, `213`, `214`, `215`, `216`, `217`, `218`, `219`, `220`, `221`, `222`, `223`, `224`, `230`, `231`, `232`, `233`
- SP_ 节点：`SP_2505_ITEMS_ARE_NOT_AVAILABLE_FOR_PURCHASE_R_N_AT_THIS_HOUR`, `SP_2506_THE_ITEM_IS_OUT_OF_STOCK_AND_THEREFORE_R_NNOT_AVAILABLE_FOR_SALE`, `SP_3274_THIS_IS_THE_NX_COUPON_NUMBER_R_NREGISTER_YOUR_COUPON_AT_WWWNEXONNET`, `SP_3275_THIS_COUPON_IS_ONLY_AVAILABLE_TO_THE_USERS_BUYING_CASH_ITEM_FOR_THE_FIRST_TIME`, `SP_3558_YOU_HAVE_ALREADY_APPLIED_FOR_THIS`, `SP_3749_THIS_ITEM_CAN_ONLY_BE_USED_15_DAYS__R_NAFTER_THE_ACCOUNTS_REGISTRATION`, `SP_3992_YOU_DO_NOT_HAVE_ENOUGH_GIFT_TOKENS__R_NIN_YOUR_ACCOUNT_PLEASE_CHARGE_YOUR_ACCOUN`, `SP_3993_DUE_TO_TECHNICAL_DIFFICULTIES_R_NTHIS_ITEM_CANNOT_BE_SENT_AT_THIS_TIME_R_NPLEASE`, `SP_3994_YOU_MAY_NOT_GIFT_ITEMS_FOR__R_NIT_HAS_BEEN_LESS_THAN_TWO_WEEKS__R_NSINCE_YOU_FIR`, `SP_3995_USERS_WITH_HISTORY_OF_ILLEGAL_ACTIVITIES_R_N_MAY_NOT_GIFT_ITEMS_TO_OTHERS_PLEASE`, `SP_3996_DUE_TO_LIMITATIONS__R_NYOU_MAY_NOT_GIFT_THIS_ITEM_AS_THIS_TIME__R_NPLEASE_TRY_AG`, `SP_3997_YOU_HAVE_EXCEEDED_THE_AMOUNT_OF_TIME__R_NYOU_CAN_GIFT_ITEMS_TO_OTHER_CHARACTERS`, `SP_3998_THIS_ITEM_CANNOT_BE_GIFTED__R_NDUE_TO_TECHNICAL_DIFFICULTIES__R_NPLEASE_TRY_AGAI`, `SP_4002_YOU_CANNOT_TRANSFER_A_CHARACTER_UNDER_LEVEL_20`, `SP_4005_YOU_CANNOT_TRANSFER_A_CHARACTER__R_NTO_THE_SAME_WORLD_IT_IS_CURRENTLY_IN`, `SP_4006_YOU_CANNOT_TRANSFER_A_CHARACTER__R_NINTO_THE_NEW_SERVER_WORLD`, `SP_4007_YOU_MAY_NOT_TRANSFER_OUT_OF_THIS__R_NWORLD_AT_THIS_TIME`, `SP_4008_YOU_CANNOT_TRANSFER_A_CHARACTER_INTO__R_NA_WORLD_THAT_HAS_NO_EMPTY_CHARACTER_SLO`, `SP_4020_THE_EVENT_HAS_EITHER_ENDED_OR_R_NTHIS_ITEM_IS_NOT_AVAILABLE_FOR_FREE_TESTING`, `SP_4457_YOU_HAVE_EXCEEDED_THE_MAXIMUM_NUMBER_R_NOF_USAGE_PER_ACCOUNT_FOR_THIS_ACCOUNT_R_`, `SP_4537_YOU_CANNOT_SEND_A_GIFT_TO_YOUR_OWN_ACCOUNT_R_NPLEASE_PURCHASE_IT_AFTER_LOGGING_R`, `SP_4538_PLEASE_CONFIRM_WHETHER_R_NTHE_CHARACTERS_NAME_IS_CORRECT`, `SP_4539_THIS_ITEM_HAS_A_GENDER_RESTRICTION_R_NPLEASE_CONFIRM_THE_GENDER_OF_THE_RECIPIENT`, `SP_4540_THE_GIFT_CANNOT_BE_SENT_BECAUSE_R_NTHE_RECIPIENTS_INVENTORY_IS_FULL`, `SP_5063_THIS_ITEM_CANNOT_BE_PURCHASED__R_NWITH_MAPLEPOINTS`, `SP_5064_SORRY_FOR_INCONVINENCE__R_NPLEASE_TRY_AGAIN`, `SP_514_CHECK_YOUR_BIRTHDAY_CODE_AND_R_NPLEASE_TRY_AGAIN`, `SP_534_REQUEST_TIMED_OUT_R_NPLEASE_TRY_AGAIN`, `SP_535_YOU_DONT_HAVE_ENOUGH_CASH`, `SP_536_YOU_CANT_BUY_SOMEONE_A_CASH_ITEM_GIFT_IF_YOURE_UNDER_14`, `SP_537_YOU_HAVE_EXCEEDED_THE_ALLOTTED_LIMIT_OF_PRICE_R_NFOR_GIFTS`, `SP_538_PLEASE_CHECK_AND_SEE_IF_YOU_HAVE_EXCEEDED_R_NTHE_NUMBER_OF_CASH_ITEMS_YOU_CAN_HA`, `SP_539_PLEASE_CHECK_AND_SEE_R_NIF_THE_NAME_OF_THE_CHARACTER_IS_WRONG_R_NOR_IF_THE_ITEM_`, `SP_540_YOU_HAVE_REACHED_THE_DAILY_MAXIMUM__R_NPURCHASE_LIMIT_FOR_THE_CASH_SHOP`, `SP_541_DUE_TO_GENDER_RESTRICTIONS_THE_COUPON__R_NIS_UNAVAILABLE_FOR_USE`, `SP_542_THIS_COUPON_WAS_ALREADY_USED`, `SP_543_THIS_COUPON_HAS_EXPIRED`, `SP_544_THIS_COUPON_CAN_ONLY_BE_USED_AT_R_NNEXON_AFFILIATED_INTERNET_CAFES_R_NPLEASE_USE`, `SP_545_THIS_COUPON_IS_A_NEXON_AFFILIATED_INTERNET_CAFE_ONLY_COUPON_R_NAND_IT_HAD_ALREAD`, `SP_546_THIS_COUPON_IS_A_NEXON_AFFILIATED_INTERNET_CAFE_ONLY_COUPON_R_NAND_IT_HAD_ALREAD`, `SP_547_PLEASE_CHECK_AND_SEE_IF__R_NTHE_COUPON_NUMBER_IS_RIGHT`, `SP_551_THIS_COUPON_IS_ONLY_FOR_REGULAR_ITEMS_AND__R_NITS_UNAVAILABLE_TO_GIVE_AWAY_AS_A_`, `SP_552_THIS_COUPON_IS_ONLY_FOR_MAPLESTORY_AND_R_NIT_CANNOT_BE_GIFTED_TO_OTHERS`, `SP_553_THIS_ITEM_IS_ONLY_AVAILABLE_FOR_PURCHASE_BY_A_USER_AT_THE_PREMIUM_SERVICE_INTERN`, `SP_554_YOU_ARE_SENDING_A_GIFT_TO_AN_INVALID_RECIPIENT_R_NPLEASE_CHECK_THE_CHARACTER_NAM`, `SP_555_YOU_HAVE_EXCEEDED_THE_SPENDING_LIMIT_OF_NX`, `SP_556_THE_CASH_SHOP_IS_UNAVAILABLE_R_NDURING_THE_BETA_TEST_PHASE_R_NWE_APOLOGIZE_FOR_Y`, `SP_5573_THIS_ITEM_CANNOT_BE_R_NPURCHASED_BY_ANYONE_UNDER_7`, `SP_5574_THIS_ITEM_CANNOT_BE_R_NRECEIVED_BY_ANYONE_UNDER_7`, `SP_557_DUE_TO_AN_UNKNOWN_ERROR_R_NTHE_REQUEST_FOR_CASH_SHOP_HAS_FAILED`, `SP_5599_YOU_DO_NOT_HAVE_ENOUGH_MESOS`, `SP_583_PLEASE_CHECK_THE_NAME_OF_THE_RECEIVER`, `SP_853_PLEASE_CHECK_IF_YOUR_INVENTORY_IS_FULL_OR_NOT`
  - `int __thiscall CCashShop::NoticeFailReason(CCashShop *this, int a2)`
  - `switch ( a2 )`
  - `ZXString<char>::ZXString<char>(this: &v5, a2: aTheCouponSyste, a3: 0xFFFFFFFF);`
  - `return CUtilDlg::Notice(sMsg: v5, sSndName: v6, ppDialog: v7, bAutoSeparated: v8, bIsTightLine: v9);`

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

#### CCashShop::OnBuyTransferWorldItem  (4035 B)

- SP_ 节点：`SP_593_YOU_DONT_HAVE_ENOUGH_NX_PREPAID`
  - `void __thiscall CCashShop::OnBuyTransferWorldItem(CCashShop *this, int *a2)`
  - `if ( *(this + 16) == 0 )`
  - `CCashShop::NoticeFailReason(this, a2: 195);`
  - `if ( *(this + 6) == 0 )`
  - `ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF);`
  - `if ( CCashShop::CheckTransferWorldPossible(a1: &v23) == 0 )`
  - `v4 = sub_428226(a1: &v23);`
  - `ZXString<char>::ZXString<char>(this: &v12, a2: v4, a3: 0xFFFFFFFF);`
  - `CUtilDlg::Notice(sMsg: v12, sSndName: v13, ppDialog: v14, bAutoSeparated: v15, bIsTightLine: v16);`
  - `v20 = sub_9ACF95();`
  - `if ( v20 < 0 )`
  - `if ( v23 != 0 )`
  - `LOBYTE(v24) = 1;`
  - `CWvsContext::GetCommodityBySN(a1: v17, a2);`
  - `LOBYTE(v24) = 2;`
  - `Data = TSecType<long>::GetData(a1: (v18 + 16), a2: 0);`
  - …（截断 16 行）

#### CCashShop::OnCashItemGachaponResult  (2162 B)

- SP_ 节点：`SP_538_PLEASE_CHECK_AND_SEE_IF_YOU_HAVE_EXCEEDED_R_NTHE_NUMBER_OF_CASH_ITEMS_YOU_CAN_HA`
  - `int __thiscall CCashShop::OnCashItemGachaponResult(char *this, wchar_t *a2)`
  - `result = CInPacket::Decode1(this: a2);`
  - `if ( result == 229 )`
  - `CInPacket::DecodeBuffer(a1: v18, a2: 8u);`
  - `v5 = CInPacket::Decode4(this: a2);`
  - `for ( i = 0; ; i += 55 )`
  - `if ( *v7 == 0 || v6 >= *(v8 - 4) )`
  - `v9 = (i + v8);`
  - `if ( *v9 == v18[0] && v9[1] == v18[1] )`
  - `*(*v7 + 55 * v6 + 24) = v5;`
  - `if ( v5 == 0 )`
  - `sub_47CDE8(a1: (55 * v6 + *v7));`
  - `v10 = ZArray<GW_CashItemInfo>::InsertBefore(a1: -1);`
  - `result = CInPacket::DecodeBuffer(a1: v10, a2: 0x37u);`
  - `v11 = *(this + 330);`
  - `if ( v11 != 0 )`
  - …（截断 16 行）

#### CCashShop::OnCashItemResBuyFailed  (930 B)

  - `void __thiscall CCashShop::OnCashItemResBuyFailed(CCashShop *this, struct CInPacket *a2)`
  - `*(this + 6) = 0;`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `if ( v3 == 192 || v3 == 191 )`
  - `v5 = CInPacket::Decode4(this: a2);`
  - `CCashShop::UpdateStock(this, a2: v5, a3: v4);`
  - `CCashShop::ChangeLimitGoodsState(this, a2: v5, a3: v4);`
  - `CCSWnd_List::ChangePage(this: (this + 5564));`
  - `if ( v4 == 162 || v4 == 164 || v4 == 163 )`
  - `CCashShop::NoticeFailReason(this, a2: v4);`
  - `CCashShop::SendTransferFieldPacket();`
  - `else if ( *(this + 310) != 0 )`
  - `CCashShop::SendBuyAvatarPacket(this);`

#### CCashShop::OnCashItemResBuyPackageDone  (3234 B)

- SP_ 节点：`SP_1233_BUYSHOPITEM`, `SP_3441_PURCHASE_SUCCESSFUL_R_ND_YOU_HAVE_RECEIVED_MAPLEPOINTS`, `SP_529_YOU_HAVE_SUCCESSFULLY_MADE_THE_PURCHASE`
  - `int __thiscall CCashShop::OnCashItemResBuyPackageDone(CCashShop *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `ZArray<GW_CashItemInfo>::_Alloc(a1: v3, a2: &v26);`
  - `if ( v3 > 0 )`
  - `CInPacket::DecodeBuffer(this: a2, a2: v24, a3: 55 * v3);`
  - `v23 = (this + 2468);`
  - `for ( i = v3; i != 0; --i )`
  - `v5 = ZArray<GW_CashItemInfo>::InsertBefore(a1: -1);`
  - `qmemcpy(v5, v24 + v4, 0x34u);`
  - `*(v5 + 52) = *v6;`
  - `*(v5 + 54) = v6[2];`
  - `CWnd::InvalidateRect(this: v7, a2: v19);`
  - `CCSWnd_Locker::SetSelectedNo(this: v23, a2: 0);`
  - `v8 = CInPacket::Decode2(this: a2);`
  - `LOBYTE(v27) = 1;`
  - `if ( v8 != 0 )`
  - …（截断 16 行）

#### CCashShop::OnCashItemResGiftCouponDone  (8511 B)

- SP_ 节点：`SP_1233_BUYSHOPITEM`, `SP_5601_YOU_HAVE_GIVEN_A_GIFT_TO`, `SP_5617_AND__R_N`, `SP_585_YOU_HAVE_RECEIVED`, `SP_586__R_ND_S`, `SP_587_D_MAPLEPOINTS`, `SP_589_TO_S`
  - `void __thiscall CCashShop::OnCashItemResGiftCouponDone(CCashShop *this, volatile LONG *a2)`
  - `CInPacket::DecodeStr(this: a2, a2: v37);`
  - `v2 = CInPacket::Decode1(this: a2);`
  - `LOBYTE(v48) = 7;`
  - `LOBYTE(v48) = 8;`
  - `ZXString<char>::Format(a1: &v41, a2: v32, a3: v37[0]);`
  - `if ( i != 0 )`
  - `ZArray<GW_CashItemInfo>::_Alloc(a1: v2, a2: &v47);`
  - `LOBYTE(v48) = 9;`
  - `if ( v2 > 0 )`
  - `CInPacket::DecodeBuffer(this: a2, a2: v45, a3: 55 * v2);`
  - `for ( i = v2; i != 0; --i )`
  - `ItemName = CItemInfo::GetItemName(a1: &v36, a2: *(v45 + v5 + 16));`
  - `LOBYTE(v48) = 10;`
  - `ZXString<char>::operator=(a1: ItemName);`
  - `if ( v36 != 0 )`
  - …（截断 16 行）

#### CCashShop::OnCashItemResLimitGoodsCountChanged  (1915 B)

  - `void __thiscall CCashShop::OnCashItemResLimitGoodsCountChanged(CCashShop *this, struct CInPacket *a2)`
  - `v10 = CInPacket::Decode4(this: a2);`
  - `v14 = CInPacket::Decode4(this: a2);`
  - `v12 = CInPacket::Decode4(this: a2);`
  - `v3 = (*dword_BE7918 + 14052);`
  - `for ( i = 0; ; i += 104 )`
  - `if ( *v3 == 0 || v15 >= *(v4 - 4) )`
  - `v5 = (i + v4);`
  - `if ( v5 != 0 && v10 == *v5 )`
  - `while ( *v7 != 0 )`
  - `if ( *v7 == v14 )`
  - `if ( v5[13] <= 0 )`
  - `if ( v8 )`
  - `CCashShop::ChangeLimitGoodsState(this, a2: v14, a3: v9);`
  - `CCSWnd_List::ChangePage(this: (this + 5564));`
  - `else if ( v12 == 0 )`
  - …（截断 16 行）

#### CCashShop::OnCashItemResLoadGiftDone  (607 B)

  - `int __thiscall CCashShop::OnCashItemResLoadGiftDone(CCashShop *this, struct CInPacket *a2)`
  - `v2 = CInPacket::Decode2(this: a2);`
  - `ZArray<GW_GiftList>::_Alloc(a1: v2, a2: &v6);`
  - `if ( v2 != 0 )`
  - `CInPacket::DecodeBuffer(this: a2, a2: v5, a3: 98 * v2);`
  - `return sub_47CE5E(a1: &v5);`

#### CCashShop::OnCashItemResLoadGiftFailed  (293 B)

  - `int __thiscall CCashShop::OnCashItemResLoadGiftFailed(CCashShop *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `CCashShop::NoticeFailReason(this, a2: v3);`
  - `return CCashShop::SendTransferFieldPacket();`

#### CCashShop::OnCashItemResLoadLockerDone  (705 B)

  - `void __thiscall CCashShop::OnCashItemResLoadLockerDone(void **this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode2(this: a2);`
  - `ZArray<GW_CashItemInfo>::_Alloc(a1: v3, a2: &v4);`
  - `if ( v3 != 0 )`
  - `CInPacket::DecodeBuffer(this: a2, a2: *(this + 290), a3: 55 * v3);`
  - `*(this + 291) = CInPacket::Decode2(this: a2);`
  - `*(this + 292) = CInPacket::Decode2(this: a2);`
  - `CCSWnd_List::ResetSlotIncComm(this: (this + 1391));`
  - `CCSWnd_Inventory::EnableExTrunkButton(this: (this + 976));`
  - `CWnd::InvalidateRect(this: (this + 617), a2: 0);`

#### CCashShop::OnCashItemResLoadLockerFailed  (295 B)

  - `int __thiscall CCashShop::OnCashItemResLoadLockerFailed(CCashShop *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `CCashShop::NoticeFailReason(this, a2: v3);`
  - `return CCashShop::SendTransferFieldPacket();`

#### CCashShop::OnCashItemResLoadWishDone  (488 B)

  - `void __thiscall CCashShop::OnCashItemResLoadWishDone(CCashShop *this, struct CInPacket *a2)`
  - `v3 = (this + 1180);`
  - `CInPacket::DecodeBuffer(this: a2, a2: this + 1180, a3: 0x28u);`
  - `for ( i = 10; i != 0; --i )`
  - `if ( *v3 != 0 )`
  - `CCashShop::GetUsableCommodityIndexBySN(this, a2: v3);`
  - `CWnd::InvalidateRect(this: (this + 5564), a2: 0);`

#### CCashShop::OnCashItemResLoadWishFailed  (293 B)

  - `int __thiscall CCashShop::OnCashItemResLoadWishFailed(CCashShop *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `CCashShop::NoticeFailReason(this, a2: v3);`
  - `return CCashShop::SendTransferFieldPacket();`

#### CCashShop::OnCashItemResUseCouponDone  (11764 B)

- SP_ 节点：`SP_1233_BUYSHOPITEM`, `SP_5611__R_NUSING_THE_COUPON`, `SP_5617_AND__R_N`, `SP_585_YOU_HAVE_RECEIVED`, `SP_586__R_ND_S`, `SP_587_D_MAPLEPOINTS`, `SP_588_D_MESOS`
  - `void __fastcall CCashShop::OnCashItemResUseCouponDone(CCashShop *this, int a2, struct CInPacket *a3)`
  - `v4 = CInPacket::Decode1(this: a3);`
  - `ZArray<GW_CashItemInfo>::_Alloc(a1: v4, a2: &v75);`
  - `LOBYTE(v76) = 8;`
  - `if ( v4 > 0 )`
  - `CInPacket::DecodeBuffer(this: a3, a2: v72, a3: 55 * v4);`
  - `v5 = ZArray<GW_CashItemInfo>::InsertBefore(a1: -1);`
  - `qmemcpy(v7, v72 + *v68, 0x34u);`
  - `v9 = (v8 + 2468);`
  - `CWnd::InvalidateRect(this: (v8 + 2468), a2: 0);`
  - `CCSWnd_Locker::SetSelectedNo(this: v9, a2: 0);`
  - `ItemName = CItemInfo::GetItemName(a1: &v60, a2: *(v72 + *v68 + 16));`
  - `LOBYTE(v76) = 9;`
  - `ZXString<char>::operator=(a1: ItemName);`
  - `if ( v60 != 0 )`
  - `v12 = *(v72 + v10 + 24);`
  - …（截断 16 行）

#### CCashShop::OnCashItemResUseCouponFailed  (1279 B)

- SP_ 节点：`SP_548_YOU_ARE_DISCONNECTED__R_NFROM_CASH_SHOP__R_NBECAUSE_YOU_PUT_WRONG__R_NCOUPON_NUM`
  - `void __thiscall CCashShop::OnCashItemResUseCouponFailed(CCashShop *this, struct CInPacket *a2)`
  - `*(this + 6) = 0;`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `if ( v3 == 162 || v3 == 164 )`
  - `CCashShop::NoticeFailReason(this, a2: v3);`
  - `CCashShop::SendTransferFieldPacket();`
  - `if ( v4 == 177 )`
  - `CUtilDlg::Notice(sMsg: v7, sSndName: v8, ppDialog: v9, bAutoSeparated: v10, bIsTightLine: v11);`
  - `CCashShop::OnStatusExit(this);`

#### CCashShop::OnCashItemResult  (4949 B)

- 分支 case：`71`, `75`, `76`, `77`, `78`, `79`, `80`, `85`, `86`, `87`, `88`, `89`, `91`, `92`, `94`, `95`, `96`, `97`, `98`, `99`, `100`, `101`, `102`, `103`, `104`, `105`, `106`, `107`, `108`, `109`, `110`
  - `void __thiscall CCashShop::OnCashItemResult(CCashShop *this, struct CInPacket *a2)`
  - `switch ( CInPacket::Decode1(this: a2) )`
  - `CCashShop::OnCashItemResLimitGoodsCountChanged(this, a2);`
  - `CCashShop::OnCashItemResLoadLockerDone(this, a2);`
  - `CCashShop::OnCashItemResLoadLockerFailed(this, a2);`
  - `CCashShop::OnCashItemResLoadGiftDone(this, a2);`
  - `CCashShop::OnCashItemResLoadGiftFailed(this, a2);`
  - `CCashShop::OnCashItemResLoadWishDone(this, a2);`
  - `CCashShop::OnCashItemResLoadWishFailed(this, a2);`
  - `sub_479844(a1: a2);`
  - `sub_4798A7(a1: a2);`
  - `sub_4798C6(a1: a2);`
  - `CCashShop::OnCashItemResBuyFailed(this, a2);`
  - `CCashShop::OnCashItemResUseCouponDone(this, a2);`
  - `CCashShop::OnCashItemResGiftCouponDone(this, a2);`
  - `CCashShop::OnCashItemResUseCouponFailed(this, a2);`
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

#### CCashShop::OnChangedSortType  (6052 B)

  - `void __thiscall CCashShop::OnChangedSortType(CCashShop *this, int a2)`
  - `*(this + 316) = a2;`
  - `v2 = (*dword_BE7918 + 14048);`
  - `while ( 1 )`
  - `v3 = *(this + 17);`
  - `if ( v3 == 0 || v34 >= *(v3 - 4) )`
  - `v5 = *(v3 + v33 + 16);`
  - `if ( v5 != -1 )`
  - `v6 = v5 + *(v4 + 20);`
  - `if ( v5 < v6 - 1 )`
  - `if ( *(*(v7 + *v2 + 4) + 68) != 0 )`
  - `if ( v35 < v6 )`
  - `v10 = *(8 * v36 + *v2 + 4);`
  - `if ( v10[17] != 0 )`
  - `v11 = *(v32 + 316);`
  - `if ( v11 != 0 )`
  - …（截断 16 行）

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

#### CCashShop::OnGiftPackage  (9479 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_3991_WOULD_YOU_LIKE_TO_GIFT_THIS_ITEM_TO_SOMEONE_R_N__S__`, `SP_557_DUE_TO_AN_UNKNOWN_ERROR_R_NTHE_REQUEST_FOR_CASH_SHOP_HAS_FAILED`, `SP_593_YOU_DONT_HAVE_ENOUGH_NX_PREPAID`
  - `void __thiscall CCashShop::OnGiftPackage(StringPoolStrings *this, int a2)`
  - `if ( *(this + 16) == 0 )`
  - `CCashShop::NoticeFailReason(this, a2: 195);`
  - `if ( *(this + 6) == 0 )`
  - `CWvsContext::GetCommodityBySN(a1: &v28, a2);`
  - `LOBYTE(v40) = 1;`
  - `if ( v29 != 0 )`
  - `if ( TSecType<long>::GetData(a1: (v29 + 16), a2: 0) / 10000 == 910 )`
  - `if ( CUtilDlg::Notice(sMsg: v19, sSndName: v20, ppDialog: v21, bAutoSeparated: v22, bIsTightLine: v23) == 0 )`
  - `TSecType<long>::GetData(a1: (v29 + 16), a2: 0);`
  - `*v35 = *(v29 + 32);`
  - `if ( *v35 != 0 )`
  - `Data = TSecType<long>::GetData(a1: (v29 + 16), a2: 0);`
  - `sub_5D7787(a1: v38, a2: Data);`
  - `v23 = *(this + 15);`
  - `LOBYTE(v40) = 2;`
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

#### CCashShop::OnPacket  (1189 B)

- 分支 case：`323`, `324`, `325`, `326`, `327`, `328`, `329`, `331`, `332`, `333`
  - `void __thiscall CCashShop::OnPacket(int **this, int a2, volatile LONG *a3)`
  - `switch ( a2 )`
  - `sub_478EF6(a1: a3);`
  - `CCashShop::OnQueryCashResult(a1: a3);`
  - `CCashShop::OnCashItemResult(a1: a3);`
  - `sub_479070(a1: a3);`
  - `CCashShop::OnGiftMateInfoResult(this: this - 2, a2: a3);`
  - `sub_47BAEA(a1: a3);`
  - `sub_47BBB6(a1: a3);`
  - `sub_47BD9B(a1: a3);`
  - `sub_47C83C(a1: this - 2, a2: a3);`
  - `CCashShop::OnCashItemGachaponResult(this: this - 8, a2: a3);`

#### CCashShop::OnQueryCashResult  (1451 B)

  - `void __thiscall CCashShop::OnQueryCashResult(CCashShop *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode4(this: a2);`
  - `v4 = CInPacket::Decode4(this: a2);`
  - `v5 = CInPacket::Decode4(this: a2);`
  - `if ( v3 < 0 || v4 < 0 || v5 < 0 )`
  - `CCashShop::NoticeFailReason(this, a2: 164);`
  - `CCashShop::SendTransferFieldPacket();`
  - `*(this + 9) = _ZtlSecureTear<long>(a1: v3, a2: this + 28);`
  - `*(this + 12) = _ZtlSecureTear<long>(a1: v4, a2: this + 40);`
  - `*(this + 15) = _ZtlSecureTear<long>(a1: v6, a2: this + 52);`
  - `CWnd::InvalidateRect(this: (this + 7648), a2: 0);`
  - `v7 = *(this + 310) == 0;`
  - `*(this + 6) = 0;`
  - `if ( !v7 )`
  - `CCashShop::SendBuyAvatarPacket(this);`
  - `if ( *(this + 322) != 0 )`
  - …（截断 16 行）

#### CCashShop::OnRemoveWish  (1598 B)

  - `void __thiscall CCashShop::OnRemoveWish(CCashShop *this, int a2)`
  - `if ( *(this + 16) != 0 )`
  - `if ( *(this + 6) == 0 && a2 != 0 && CCashShop::GetUsableCommodityIndexBySN(this, &a2) >= 0 )`
  - `memcpy(a1: v6, a2: this + 1180, a3: sizeof(v6));`
  - `WishList = CCashShop::FindWishList(this, a2);`
  - `if ( WishList != -1 )`
  - `if ( WishList < 9 )`
  - `while ( v4 != 0 );`
  - `COutPacket::COutPacket(a1: 229);`
  - `COutPacket::Encode1(this: &v7, a2: 5);`
  - `COutPacket::EncodeBuffer(a1: v6, a2: 0x28u);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v7);`
  - `*(this + 6) = 1;`
  - `*(this + 306) = 1;`
  - `ZArray<unsigned char>::RemoveAll(a1: v8);`
  - `CCashShop::NoticeFailReason(this, a2: 195);`
  - …（截断 16 行）

#### CCashShop::OnSetWish  (3291 B)

- SP_ 节点：`SP_527_YOU_ALREADY_HAVE_THIS_ITEM_ON_YOUR_WISH_LIST`, `SP_528_YOUR_WISH_LIST_IS_FULL`
  - `void __thiscall CCashShop::OnSetWish(CCashShop *this, int a2)`
  - `if ( *(this + 16) == 0 )`
  - `CCashShop::NoticeFailReason(this, a2: 195);`
  - `if ( *(this + 6) == 0 && a2 != 0 )`
  - `UsableCommodityIndexBySN = CCashShop::GetUsableCommodityIndexBySN(this, &a2);`
  - `if ( UsableCommodityIndexBySN >= 0 )`
  - `v5 = (this + 1180);`
  - `if ( *v5 == 0 )`
  - `while ( v20 < 10 );`
  - `if ( v20 == 10 )`
  - `CUtilDlg::Notice(sMsg: v11, sSndName: v12, ppDialog: v13, bAutoSeparated: v14, bIsTightLine: v15);`
  - `if ( CCashShop::FindWishList(this, a2) >= 0 )`
  - `v8 = (*dword_BE7918 + 14048);`
  - `if ( *(*(*(*dword_BE7918 + 14048) + v9 + 4) + 32) != 0 )`
  - `COutPacket::COutPacket(a1: 229);`
  - `COutPacket::Encode1(this: &v16, a2: 5);`
  - …（截断 16 行）

#### CCashShop::OnStatusCharge  (801 B)

  - `int __thiscall CCashShop::OnStatusCharge(CCashShop **this)`
  - `if ( *(this + 6) == 0 )`
  - `ZXString<char>::ZXString<char>(this: &v2, a2: aPleaseVisitThe, a3: 0xFFFFFFFF);`
  - `return CUtilDlg::Notice(sMsg: v2, sSndName: v3, ppDialog: v4, bAutoSeparated: v5, bIsTightLine: v6);`

#### CCashShop::OnStatusCheck  (204 B)

  - `void __thiscall CCashShop::OnStatusCheck(CCashShop *this)`
  - `if ( *(this + 16) != 0 )`
  - `sub_472958(this);`
  - `CCashShop::NoticeFailReason(this, a2: 195);`

#### CCashShop::OnStatusCoupon  (2952 B)

  - `void __thiscall CCashShop::OnStatusCoupon(CCashShop *this)`
  - `if ( *(this + 16) != 0 )`
  - `if ( *(this + 6) == 0 )`
  - `if ( CCouponUseSelectDlg::Confirm(a1: &v14) == 1 )`
  - `ZXString<char>::TrimRight(a1: 0);`
  - `ZXString<char>::TrimLeft(a1: 0);`
  - `if ( v14 != 0 )`
  - `v3 = *(v14 - 4);`
  - `if ( v3 != 0 )`
  - `COutPacket::COutPacket(a1: 230);`
  - `LOBYTE(v15) = 3;`
  - `ZXString<char>::operator=(a1: &v13);`
  - `COutPacket::EncodeStr(this: &v9, a2: v8[0]);`
  - `ZXString<char>::operator=(a1: &v14);`
  - `if ( v13 != 0 && *v13 != 0 )`
  - `ZXString<char>::operator=(a1: &v12);`
  - …（截断 16 行）

#### CCashShop::OnStatusExit  (134 B)

  - `void __thiscall CCashShop::OnStatusExit(CCashShop *this)`
  - `CCashShop::SendTransferFieldPacket();`

#### CCashShop::OnTakeOff  (94 B)

  - `int __thiscall CCashShop::OnTakeOff(CCashShop *this)`
  - `return sub_4AC6D6(a1: this + 1364);`

#### CCashShop::OnWear  (147 B)

  - `void __thiscall CCashShop::OnWear(CCashShop *this, int a2, int a3, int a4)`
  - `CCSWnd_Char::OnWear(this: (this + 1364), a2, a3, a4);`

#### CCashShop::ProcessBuy  (1319 B)

  - `void __thiscall CCashShop::ProcessBuy(CCashShop *this, void ***a2, int *a3)`
  - `if ( a2 / 100 != 11120 || a2 == 1112000 )`
  - `if ( a2 / 10000 == 910 )`
  - `CCashShop::OnBuyPackage(this, a2: a3);`
  - `else if ( a3 < 80000000 || a3 >= 90000000 )`
  - `if ( v4 == 11128 && a2 % 10 <= 2 )`
  - `sub_470A5A(this, a2: a3);`
  - `else if ( a2 / 1000 == 5430 )`
  - `sub_46C6F8(a1: a3);`
  - `else if ( v5 == 555 )`
  - `sub_46C8E1(a1: a3);`
  - `else if ( v5 == 911 )`
  - `sub_470653(this, a2: a3);`
  - `else if ( a2 == (&loc_5265BC + 4) )`
  - `sub_47031E(this, a2: a3);`
  - `else if ( a2 == &off_5269A8 )`
  - …（截断 16 行）

#### CCashShop::RestoreFocus  (547 B)

  - `CWndMan *__thiscall CCashShop::RestoreFocus(CCashShop *this)`
  - `if ( TSingleton<CWndMan>::ms_pInstance != 0 )`
  - `IsControlingUserPreview = CCashShop::IsControlingUserPreview(this);`
  - `return CWndMan::SetFocus(this: v3, a2: IsControlingUserPreview == 0 ? (this != 0 ? (this + 4) : 0) : 0);`

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

#### CCashShop::SendCheckDuplicateIDPacket  (1758 B)

- SP_ 节点：`SP_516_YOU_CANNOT_USE_THIS_NAME__R_NPLEASE_TRY_AGAIN`
  - `void __thiscall CCashShop::SendCheckDuplicateIDPacket(_DWORD *this, int a2)`
  - `Data = TSecType<unsigned char>::GetData(this: (*dword_BE7918 + 8240));`
  - `valid = is_valid_character_name(sCharName: &a2, bWordCheck: (Data & 1) == 0);`
  - `if ( valid )`
  - `COutPacket::COutPacket(a1: 21);`
  - `LOBYTE(v15) = 1;`
  - `ZXString<char>::operator=(a1: &a2);`
  - `COutPacket::EncodeStr(this: &v12, a2: v11);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v12);`
  - `LOBYTE(v15) = 0;`
  - `*(this + 6) = 1;`
  - `ZArray<unsigned char>::RemoveAll(a1: v13);`
  - `CUtilDlg::Notice(sMsg: v7, sSndName: v8, ppDialog: v9, bAutoSeparated: v10, bIsTightLine: v11);`

#### CCashShop::SendGiftsPacket  (6226 B)

- SP_ 节点：`SP_561_ALL_THE_GIFTS_HAVE_BEEN_SENT_R_ND_NX__R_NWAS_SPENT_IN_THE_PROCESS`, `SP_562_THE_GIFTS_COULD_NOT_BE_SENT`, `SP_563___S___R_NHAS_BEEN_SENT_TO_THE_CHARACTER_ABOVE_R_NIT_COULD_NOT_BE_SENT_TO_SEVERAL`
  - `void __thiscall CCashShop::SendGiftsPacket(CCashShop *this)`
  - `v2 = *(this + 320);`
  - `v3 = (this + 1280);`
  - `if ( v2 != 0 )`
  - `v4 = *(v2 - 4);`
  - `v5 = *(this + 321);`
  - `if ( v5 == v4 )`
  - `v6 = *(this + 325);`
  - `v7 = (this + 1300);`
  - `if ( v6 != 0 )`
  - `v8 = *(v6 - 4);`
  - `if ( v9 != 0 )`
  - `v10 = *(v9 - 4);`
  - `if ( v8 == v10 )`
  - `v29[0] = *(this + 323);`
  - `LOBYTE(v39) = 1;`
  - …（截断 16 行）

#### CCashShop::SendTransferFieldPacket  (790 B)

  - `int CCashShop::SendTransferFieldPacket()`
  - `COutPacket::COutPacket(a1: 38);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: v3);`
  - `v0 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 24);`
  - `LOBYTE(v6) = 1;`
  - `if ( v0 != 0 )`
  - `v1 = CInterStage::CInterStage(this: v0);`
  - `LOBYTE(v6) = 0;`
  - `set_stage(a1: v1, a2: 0);`
  - `return ZArray<unsigned char>::RemoveAll(a1: v4);`

#### CCashShop::SetActiveEffectItem  (132 B)

  - `int __thiscall CCashShop::SetActiveEffectItem(CCashShop *this, int a2, int a3)`
  - `return sub_4AD168(a1: a2, a2: a3);`

#### CCashShop::SetEmotion  (131 B)

  - `void __thiscall CCashShop::SetEmotion(CCashShop *this, int a2)`
  - `CCSWnd_Char::SetEmotion(this: (this + 1364), a2);`

#### CCashShop::SetPet  (148 B)

  - `void __thiscall CCashShop::SetPet(CCashShop *this, int a2, int a3, int a4)`
  - `CITCWnd_Char::SetPet(this: (this + 1364), a2, a3, a4);`

#### CCashShop::SetUserPreviewControl  (267 B)

  - `int __thiscall CCashShop::SetUserPreviewControl(CCashShop *this, _DWORD *a2)`
  - `*(this + 334) = a2;`
  - `sub_4AD51E(this: this + 341, a2);`
  - `sub_4B9638(this: this + 1391, a2);`
  - `return CCashShop::RestoreFocus(this);`

#### CCashShop::ShowBullet  (131 B)

  - `void __thiscall CCashShop::ShowBullet(CCashShop *this, int a2)`
  - `CCSWnd_Char::ShowBullet(this: (this + 1364), a2);`

#### CCashShop::ShowEntrustedShop  (130 B)

  - `int __thiscall CCashShop::ShowEntrustedShop(CCashShop *this, int a2, int a3)`
  - `return sub_4AFE80(a1: a2, a2: a3);`

#### CCashShop::ShowMessageBox  (127 B)

  - `int __thiscall CCashShop::ShowMessageBox(CCashShop *this, int a2, int a3)`
  - `return sub_4AFE45(a1: a2, a2: a3);`

#### CCashShop::ShowPersonalShop  (129 B)

  - `int __thiscall CCashShop::ShowPersonalShop(CCashShop *this, int a2, int a3)`
  - `return sub_4AFE5B(a1: a2, a2: a3);`

#### CCashShop::UpdateStock  (811 B)

  - `int __thiscall CCashShop::UpdateStock(CCashShop *this, int a2, int a3)`
  - `v3 = *(this + 18);`
  - `if ( v3 == 0 )`
  - `if ( *(v3 - 4) == 0 )`
  - `v4 = *(v3 - 4) - 1;`
  - `if ( v4 < 0 )`
  - `for ( i = (v3 + 8 * v4); *i != a2; i -= 2 )`
  - `if ( --v4 < 0 )`
  - `if ( a3 != 191 )`
  - `if ( a3 == 192 )`
  - `*(v3 + 8 * v4 + 4) = 1;`
  - `*(v3 + 8 * v4 + 4) = 2;`

### CCSWnd_Char（18 函数）


#### CCSWnd_Char::BlowWeather  (6891 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1515_TYPE`, `SP_1587_MSG`, `SP_1614_SPEED`, `SP_1802_FLOATTYPE`, `SP_1803_PATH`, `SP_3512_OO`, `SP_3513_INFO`, `SP_3781_DIRECTION`
  - `int __thiscall CCSWnd_Char::BlowWeather(CCSWnd_Char *this, StringPoolStrings a2)`
  - `*(this + 272) = 1;`
  - `CItemInfo::GetItemInfo(a1: &v46, a2: v34);`
  - `if ( v46 == 0 )`
  - `return sub_4027E9(a1: &v46);`
  - `LOBYTE(v47) = 0;`
  - `sub_40263B(a1: &v46);`
  - `Item = IWzProperty::GetItem(pvargDest: &pvarg, a2: v33);`
  - `LOBYTE(v47) = 2;`
  - `int32 = get_int32(pvarSrc: Item, a2: v34);`
  - `if ( v6 < 0 )`
  - `v9 = IWzProperty::GetItem(pvargDest: &pvarg, a2: v33);`
  - `LOBYTE(v47) = 4;`
  - `v39 = get_int32(pvarSrc: v9, a2: v34);`
  - `if ( v10 < 0 )`
  - `v13 = IWzProperty::GetItem(pvargDest: &pvarg, a2: v33);`
  - …（截断 16 行）

#### CCSWnd_Char::CmpAvatar  (1217 B)

- 分支 case：`12`, `13`, `15`, `16`, `14`, `11`
  - `int __thiscall CCSWnd_Char::CmpAvatar(CCSWnd_Char *this, int a2)`
  - `if ( a2 != 14 && is_sticker_bodypart(a1: a2) == 0 )`
  - `switch ( a2 )`
  - `while ( 1 )`
  - `v7 = *(this + 4 * *v6 + 173);`
  - `if ( v7 != 0 && *(this + 2 * a2 + 151) == v7 )`
  - `if ( ++v6 >= dword_BD85A0 )`
  - `return *(this + 2 * a2 + 152);`
  - `if ( *(this + 255) == 0 )`
  - `v4 = *(this + 169);`
  - `v5 = *(this + 2 * a2 + 151);`
  - `if ( v5 == 0 || v4 == v5 )`
  - `v4 = *(this + 4 * a2 + 173);`

#### CCSWnd_Char::CmpPet  (1787 B)

  - `int __thiscall CCSWnd_Char::CmpPet(CCSWnd_Char *this)`
  - `v2 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v13) + 4);`
  - `if ( v14 != 0 )`
  - `sub_428C44(this: &v13, a2: 0);`
  - `CashItemSlotPosition = CharacterData::FindCashItemSlotPosition(this: v2, a2: 5, a3: *(v2 + 27));`
  - `if ( CashItemSlotPosition != 0 )`
  - `Item = CharacterData::GetItem(this: v2, a2: v12, a3: 5, a4: CashItemSlotPosition);`
  - `v5 = *(Item + 4);`
  - `v6 = (v5 + 12);`
  - `v6 = TSecType<long>::TSecType<long>(this: &v10, a2: 0);`
  - `Data = TSecType<long>::GetData(a1: v6, a2: this);`
  - `if ( (v15 & 2) != 0 )`
  - `if ( v11 != 0 )`
  - `ZAllocEx<ZAllocAnonSelector>::Free(a1: v11);`
  - `if ( (v15 & 1) != 0 && v12[1] != 0 )`
  - `ZRef<GW_ItemSlotBase>::~ZRef<GW_ItemSlotBase>(this: v12, a2: 0);`
  - …（截断 16 行）

#### CCSWnd_Char::CreateFloatNotice  (18764 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_11_SETUP`, `SP_1300_UI_UIWINDOWIMG_FLOATNOTICE_D`, `SP_1410_CANVASFONT`, `SP_1411_BA`, `SP_5527_ARIAL`, `SP_978_Y`
  - `int __thiscall CCSWnd_Char::CreateFloatNotice(CCSWnd_Char *this, const char *a2, int *a3)`
  - `LOBYTE(v116) = 1;`
  - `PcCreateObject::IWzFont(sUOL: v4, pObj: &v108, pUnkOuter: 0);`
  - `LOBYTE(v116) = 0;`
  - `if ( i != 0 )`
  - `LOBYTE(v116) = 2;`
  - `sub_4626B3(this: &pvarg, a2: BSTR);`
  - `LOBYTE(v116) = 3;`
  - `sub_4284A8(a1: &v108);`
  - `sub_46341A(a1: v89, a2: p_pvargDest, a3: v91, a4: p_pvarg);`
  - `if ( v9 < 0 )`
  - `sub_402D9A(this: &i);`
  - `LOBYTE(v116) = 5;`
  - `LOBYTE(v116) = 6;`
  - `sub_416821(a1: &v102, a2: v11, a3);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - …（截断 16 行）

#### CCSWnd_Char::Draw  (5799 B)

  - `_DWORD *__thiscall CCSWnd_Char::Draw(_DWORD *this, VARIANTARG *a2)`
  - `CWnd::Draw(this, a2);`
  - `CWnd::GetCanvas(a1: &v38);`
  - `get_basic_font(a1: &a2, fType: 1);`
  - `LOBYTE(v39) = 2;`
  - `get_basic_font(a1: &v35, fType: 25);`
  - `LOBYTE(v39) = 3;`
  - `CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v32);`
  - `sub_4289B7(this: &v32);`
  - `v37 = *(*(this + 275) + 64);`
  - `sub_402F85(pvargDest: &v29, pvargSrc: &pvargSrc);`
  - `LOBYTE(v39) = 4;`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v39) = 5;`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v39) = 6;`
  - …（截断 16 行）

#### CCSWnd_Char::EndChat  (242 B)

  - `CWndMan *__thiscall CCSWnd_Char::EndChat(CCSWnd_Char *this)`
  - `CCtrlEdit::SetText(this: *(this + 34), a2: 0);`
  - `CWnd::SetFocusChild(this, a2: 0);`
  - `return CCashShop::RestoreFocus(this: *(this + 275));`

#### CCSWnd_Char::GetWeaponItemID  (1529 B)

  - `int __thiscall CCSWnd_Char::GetWeaponItemID(CCSWnd_Char *this, int a2, int *a3)`
  - `v4 = *(this + 173);`
  - `if ( v4 / 100000 != 17 )`
  - `EquipItem = CItemInfo::GetEquipItem(this: dword_BE78D8, a2: v4);`
  - `if ( EquipItem == 0 )`
  - `return *(this + 217);`
  - `while ( 1 )`
  - `if ( (*(EquipItem + 464) & (1 << v7)) != 0 )`
  - `while ( ((1 << v9) & (1 << v7)) == 0 )`
  - `if ( ++v9 > 17 )`
  - `if ( *v8 == a2 )`
  - `if ( v8 > dword_BD8658 )`
  - `CharacterImgEntry = CActionMan::GetCharacterImgEntry(this: dword_BE78D4, a2: v13, a3: v5);`
  - `*a3 = *(*(CharacterImgEntry + 4) + 44);`
  - `if ( !v11 )`
  - `sub_41603F(a1: 0);`
  - …（截断 16 行）

#### CCSWnd_Char::IsTwoHandWeapon  (2347 B)

  - `BOOL __stdcall CCSWnd_Char::IsTwoHandWeapon(signed int a1)`
  - `if ( a1 != 0 )`
  - `if ( a1 / 100000 != 17 )`
  - `CItemInfo::GetItemProp(&a1, a2: a1);`
  - `if ( a1 == 0 )`
  - `while ( 1 )`
  - `v3 = alloca(32);`
  - `v4 = _Int2StrW(a1: v2 + 30, a2: &v10);`
  - `_bstr_t::_bstr_t(a1: v4);`
  - `LOBYTE(v15) = 0;`
  - `sub_40263B(&a1);`
  - `IWzProperty::GetItem(pvargDest: &pvarg, a2: v7);`
  - `LOBYTE(v15) = 2;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v8, a2: v9);`
  - `if ( Unknown != 0 )`
  - `v5 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - …（截断 16 行）

#### CCSWnd_Char::OnButtonClicked  (528 B)

- 分支 case：`1000`, `1001`, `1002`
  - `void __thiscall CCSWnd_Char::OnButtonClicked(CCashShop **this, int a2)`
  - `CCashShop::RestoreFocus(this: *(this + 275));`
  - `switch ( a2 )`
  - `CCashShop::OnBuyAvatar(this: *(this + 275));`
  - `CCashShop::OnDefaultAvatar(this: *(this + 275));`
  - `CCashShop::OnTakeOff(this: *(this + 275));`

#### CCSWnd_Char::OnCreate  (26803 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1206_UI_CASHSHOPIMG_CSCHAR_BTBUYAVATAR`, `SP_1207_UI_CASHSHOPIMG_CSCHAR_BTDEFAULTAVATAR`, `SP_1273_STRING_TOOLTIPHELPIMG_SHOP_BUTTON_BUYAVATAR`, `SP_1274_STRING_TOOLTIPHELPIMG_SHOP_BUTTON_DEFAULTAVATAR`, `SP_1486_FOOTHOLD`, `SP_1487_LADDERROPE`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_2902_UI_CASHSHOPIMG_CSCHAR_BTTAKEOFFAVATAR`, `SP_2903_STRING_TOOLTIPHELPIMG_SHOP_BUTTON_TAKEOFFAVATAR`, `SP_3129_UI_CASHSHOPIMG_BASE_TAB_ENABLE`, `SP_3130_UI_CASHSHOPIMG_BASE_TAB_DISABLE`, `SP_3131_UI_CASHSHOPPREVIEWIMG`, `SP_3133_UI_CASHSHOPIMG_BASE_PREVIEWONOFF_ON`, `SP_3134_UI_CASHSHOPIMG_BASE_PREVIEWONOFF_OFF`, `SP_5527_ARIAL`
  - `_DWORD *__thiscall CCSWnd_Char::OnCreate(CCSWnd_Char *this, int Unknown)`
  - `*(this + 275) = Unknown;`
  - `sub_402F85(&pvargDest, pvargSrc: v130);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v159) = 1;`
  - `if ( dword_BF14EC == 0 )`
  - `v3 = IWzGr2D::CreateLayer(`
  - `v4 = (this + 1056);`
  - `sub_428712(a1: *v3);`
  - `sub_4156C5(a1: &Unknown);`
  - `LOBYTE(v159) = 0;`
  - `if ( v5 < 0 )`
  - `if ( v6 < 0 )`
  - `if ( *v4 == 0 )`
  - `v8 = (*(**v4 + 180))(a1: *v4, a2: -1073741824);`
  - `if ( v8 < 0 )`
  - …（截断 16 行）

#### CCSWnd_Char::OnKeyRet  (3275 B)

  - `signed int __thiscall CCSWnd_Char::OnKeyRet(CCSWnd_Char *this, unsigned int a2, _DWORD *a3)`
  - `if ( *(*(this + 260) + 11020) != 0 || a3 < 0 )`
  - `v4 = BYTE2(a3);`
  - `if ( BYTE2(a3) == 54 )`
  - `v6 = *(this + 34);`
  - `if ( *(TSingleton<CWndMan>::ms_pInstance + 136) == (*(this + 34) != 0 ? (v6 + 4) : 0) )`
  - `if ( *v5 == 5 && *(v4 + dword_BED5A0 + 4 * v4 + 5) == 53 )`
  - `CCSWnd_Char::EndChat(this);`
  - `if ( a2 == 13 )`
  - `v7 = *CCtrlEdit::GetText(this: *(this + 34), &a2);`
  - `if ( !v8 )`
  - `v10 = sub_4B1F6F(this: this + 33);`
  - `CCtrlEdit::GetText(this: v10, a2: v20);`
  - `sub_4AD07A(a1: v20[0]);`
  - `if ( a2 == 27 )`
  - `else if ( a2 > 0x24 && a2 <= 0x28 )`
  - …（截断 16 行）

#### CCSWnd_Char::OnMouseButton  (642 B)

  - `void __thiscall CCSWnd_Char::OnMouseButton(CCashShop **this, int msg, int wParam, int rx, int ry)`
  - `if ( msg == 513`
  - `&& (dword_BF0484(a1: dword_BD8A88, a2: rx, a3: ry) != 0`
  - `|| dword_BF0484(a1: dword_BD8A98, a2: rx, a3: ry) != 0`
  - `|| dword_BF0484(a1: dword_BD8AA8, a2: rx, a3: ry) != 0) )`
  - `IsControlingUserPreview = CCashShop::IsControlingUserPreview(this: *(this + 274));`
  - `CCashShop::SetUserPreviewControl(this: *(this + 274), a2: (IsControlingUserPreview == 0));`
  - `CCSWnd_Char::EndChat(this: (this - 1));`

#### CCSWnd_Char::OnWear  (4762 B)

  - `signed int __thiscall CCSWnd_Char::OnWear(CCSWnd_Char *this, int a2, int a3, int a4)`
  - `result = get_bodypart_from_item(a1: a2, a2: *(this + 160), a3: v23, a4: 0);`
  - `if ( result != 0 )`
  - `if ( v23[0] != 14 )`
  - `result = is_sticker_bodypart(a1: v23[0]);`
  - `if ( result == 0 )`
  - `if ( a2 / 100000 != 16 )`
  - `if ( *(*(this + 275) + 1244) != 0 )`
  - `(*(*(*(this + 28) + 4) + 28))(a1: *(this + 28) + 4, a2: 0);`
  - `if ( a2 / 10000 == 105 && (result = this + 652, *(this + 163) != 0) )`
  - `*(this + 164) = 0;`
  - `result = *(this + 161) / 10000;`
  - `if ( result == 105 && v6 == 6 )`
  - `*(this + 161) = 0;`
  - `*(this + 162) = 0;`
  - `if ( v6 == 12 )`
  - …（截断 16 行）

#### CCSWnd_Char::SetEmotion  (174 B)

  - `void __thiscall CCSWnd_Char::SetEmotion(CCSWnd_Char *this, int a2)`
  - `CAvatar::SetEmotion(this: (*(this + 260) + 136), nEmotion: a2 % 100 + 8, tDuration: -1);`

#### CCSWnd_Char::SetRiding  (1750 B)

  - `int __thiscall CCSWnd_Char::SetRiding(CCSWnd_Char *this, int a2, int a3, int a4)`
  - `if ( a2 == 0 )`
  - `CItemInfo::GetItemInfo(a1: &v9, a2);`
  - `if ( v9 == 0 )`
  - `if ( *(this + 187) != 0 && *(this + 189) != 0 )`
  - `*(this + 188) = 0;`
  - `*(this + 187) = 0;`
  - `*(this + 190) = 0;`
  - `*(this + 189) = 0;`
  - `*(*(this + 260) + 1348) = 0;`
  - `*(this + 2 * a4 + 151) = a2;`
  - `*(this + 2 * a4 + 152) = a3;`
  - `if ( *(this + 187) == 0 || *(this + 189) == 0 )`
  - `if ( v9 != 0 )`
  - `(*(*v9 + 8))(a1: v9);`
  - `EquipItem = CItemInfo::GetEquipItem(this: dword_BE78D8, a2: *(this + 189));`
  - …（截断 16 行）

#### CCSWnd_Char::SetRing  (5843 B)

- SP_ 节点：`SP_1644_EFFECT`, `SP_2422_CHATBALLOON`, `SP_2423_NAMETAG`
  - `void *__thiscall CCSWnd_Char::SetRing(CCSWnd_Char *this, int a2, int a3, int a4)`
  - `if ( a2 != 0 )`
  - `CItemInfo::GetItemInfo(a1: &v28, a2);`
  - `if ( v28 != 0 )`
  - `if ( *(this + 175) == a2 && a4 == 0 )`
  - `*(this + 176) = 0;`
  - `*(this + 175) = 0;`
  - `return (*(*result + 8))(a1: result);`
  - `if ( *(this + 177) == a2 && a4 == 0 )`
  - `*(this + 178) = 0;`
  - `*(this + 177) = 0;`
  - `LOBYTE(v29) = 0;`
  - `sub_40263B(a1: &v28);`
  - `Item = IWzProperty::GetItem(pvargDest: &pvarg, a2: v23);`
  - `LOBYTE(v29) = 2;`
  - `int32 = get_int32(pvarSrc: Item, a2: v24);`
  - …（截断 16 行）

#### CCSWnd_Char::ShowBullet  (684 B)

  - `void __thiscall CCSWnd_Char::ShowBullet(CUserPreview **this, int a2)`
  - `WeaponItemID = CCSWnd_Char::GetWeaponItemID(this, a2: 47, a3: &v5);`
  - `if ( get_weapon_type(nItemID: WeaponItemID) == 47 )`
  - `v4 = (*(this + 260) + 140);`
  - `if ( *(*(this + 260) + 209) != WeaponItemID )`
  - `*(*(this + 260) + 209) = WeaponItemID;`
  - `CAvatar::SetAvatarLook(this: (*(this + 260) + 136), a2: v4);`
  - `CUserPreview::DoShootAttack(this: *(this + 260), a2: WeaponItemID, a3: a2);`

#### CCSWnd_Char::TransientLayer_Weather  (48771 B)

- 分支 case：`3`, `4`, `5`, `6`
- SP_ 节点：`SP_958_SHAPE2DVECTOR2D`
  - `int __thiscall CCSWnd_Char::TransientLayer_Weather(_DWORD *this, int a2, int a3, signed int a4, int a5)`
  - `*(this + 273) = 1;`
  - `ZList<_com_ptr_t<_com_IIID<IWzGr2DLayer,&__s_GUID const _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf>>>::RemoveAll(a1: this + 266);`
  - `sub_41E4EB(a1: &v234);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v249) = 3;`
  - `sub_402F85(pvargDest: &v235, pvargSrc: &pvargSrc);`
  - `HIDWORD(v173) = &pvargDest;`
  - `LODWORD(v173) = &v235;`
  - `LOBYTE(v249) = 4;`
  - `v5 = sub_41468C(a1: &a2);`
  - `HIDWORD(v172) = v6;`
  - `_bstr_t::_bstr_t(a1: v5);`
  - `v7 = sub_406472();`
  - `v8 = sub_402680(a1: v7);`
  - `IWzResMan::GetObjectA(this: v8, pvargDest: &pvarg, sUOL: SHIDWORD(v172), vParam: v173, vAux: SHIDWORD(v173));`
  - …（截断 16 行）

### CCSWnd_List（18 函数）


#### CCSWnd_List::ChangeCategorySub  (3939 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_5515_D`
  - `void __thiscall CCSWnd_List::ChangeCategorySub(CCashShop **this)`
  - `v2 = *(this + 144);`
  - `v3 = *(v2 + 331);`
  - `if ( v3 == 10 )`
  - `*(this + 485) = 1;`
  - `v4 = *(v2 + 338) / 10u + 1;`
  - `CCtrlSelector::SetSelectorStart(this: *(this + 28), a2: v4, a3: 10, a4: 1, a5: 1);`
  - `if ( v4 > 0 )`
  - `LOBYTE(v22) = 1;`
  - `ZXString<char>::Format(a1: &v21, a2: v6, a3: v20 + 1);`
  - `LOBYTE(v22) = 0;`
  - `ZXString<char>::operator=(a1: &v21);`
  - `sub_4BCE03(a1: this + 27);`
  - `sub_4D9521(a1: v14, a2: v15[0]);`
  - `while ( v18 < v4 );`
  - `CategorySubIndex = CCashShop::GetCategorySubIndex(this: v2, a2: v3, a3: *(v2 + 332));`
  - …（截断 16 行）

#### CCSWnd_List::ChangePage  (12863 B)

- 分支 case：`10`, `9`, `8`
- SP_ 节点：`SP_1219_UI_CASHSHOPIMG_PICTUREPLATE_HOWTO`, `SP_1220_UI_CASHSHOPIMG_PICTUREPLATE_HOWTOGIFT`, `SP_1221_UI_CASHSHOPIMG_PICTUREPLATE_NOITEM`
  - `void __thiscall CCSWnd_List::ChangePage(CCSWnd_List *this)`
  - `sub_4B9F4D(this);`
  - `v3 = (this + 416);`
  - `*(v3 - 10) = -1;`
  - `while ( v2 != 0 );`
  - `*(this + 93) = -1;`
  - `*(this + 92) = 0;`
  - `CCSWnd_List::SetButtonKeyFocus(this, nPlateIndx: v52, nBtnIndx: v53);`
  - `(*(*(this + 1) + 52))(a1: this + 4);`
  - `v4 = *(this + 144);`
  - `v5 = *(v4 + 331);`
  - `switch ( v5 )`
  - `v53 = 10 * *(v4 + 333);`
  - `v6 = sub_4BD0E6(a1: v53);`
  - `if ( v6 != 0 )`
  - `v7 = (this + 376);`
  - …（截断 16 行）

#### CCSWnd_List::Draw  (70638 B)

- 分支 case：`0`, `1`, `2`, `3`
- SP_ 节点：`SP_1397_E`, `SP_1398_W`, `SP_2853_UI_CASHSHOPIMG_CSICON_D`, `SP_2879_UI_CASHSHOPIMG_CSDISCOUNT`, `SP_3119_UI_CASHSHOPIMG_CSEFFECT_EFFECT_0`, `SP_3120_UI_CASHSHOPIMG_CSEFFECT_EFFECT_1`, `SP_572_D`, `SP_574__PET_S`
  - `tagVARIANT::$::$2E851DBA257FA738680F86C475CAC6EB::$E3ADF3533BFFE4E09553D0C58E41D45E *__thiscall CCSWnd_List::Draw(`
  - `CWnd::Draw(this, a2);`
  - `CWnd::GetCanvas(a1: &a2);`
  - `if ( a2 == 0 )`
  - `v4 = (*(a2->lVal + 140))(a1: a2, a2: 0, a3: 0, a4: *(this + 9), a5: 410, a6: 0xFFFFFF);`
  - `if ( v4 < 0 )`
  - `if ( *(this + 487) != 0 || (v262 = 0, !sub_4B9E9D(this)) )`
  - `*(this + 487) = 0;`
  - `if ( !v5 )`
  - `ZArray<_com_ptr_t<_com_IIID<IWzGr2DLayer,&__s_GUID const _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf>>>::RemoveAll(a1: this + 604);`
  - `ZArray<_com_ptr_t<_com_IIID<IWzGr2DLayer,&__s_GUID const _GUID_6dc8c7ce_8e81_4420_b4f6_4b60b7d5fcdf>>>::RemoveAll(a1: this + 608);`
  - `if ( *(this + 91) == 0 )`
  - `v6 = *(this + 478);`
  - `if ( v6 != 0 && *(v6 - 4) != 0 )`
  - `for ( i = 0; ; i += 12 )`
  - `v7 = *(this + 478);`
  - …（截断 16 行）

#### CCSWnd_List::GetCommodityGender  (2765 B)

  - `signed int __stdcall CCSWnd_List::GetCommodityGender(int a1, int a2)`
  - `v2 = *(a2 + 64);`
  - `if ( v2 != 2 )`
  - `Data = TSecType<long>::GetData(a1: (a2 + 16), a2: 0);`
  - `EquipItem = CItemInfo::GetEquipItem(this: v4, a2: Data);`
  - `v7 = (a2 + 16);`
  - `if ( EquipItem != 0 )`
  - `v8 = TSecType<long>::GetData(a1: v7, a2: 0);`
  - `gender_from_id = get_gender_from_id(nItemID: v8);`
  - `ZRef<CS_COMMODITY>::~ZRef<CS_COMMODITY>(this: &a1);`
  - `if ( TSecType<long>::GetData(a1: v7, a2: 0) / 10000 == 910 )`
  - `for ( i = 0; ; ++i )`
  - `v11 = *(a2 + 96);`
  - `if ( v11 == 0 || i >= *(v11 - 4) )`
  - `CWvsContext::GetCommodityBySN(a1: v16, a2: *(v11 + 4 * i));`
  - `LOBYTE(v18) = 1;`
  - …（截断 16 行）

#### CCSWnd_List::GetPlateRect  (399 B)

  - `struct tagRECT *__stdcall CCSWnd_List::GetPlateRect(struct tagRECT *tagRECT, int nPlate)`
  - `v3 = 206 * (nPlate % 10 % 2);`
  - `tagRECT->top = 81 * (nPlate % 10 / 2) + 2;`
  - `tagRECT->bottom = 81 * (nPlate % 10 / 2) + 82;`

#### CCSWnd_List::OnButtonClicked  (2745 B)

- SP_ 节点：`SP_3524_UI_CASHSHOPIMG_PICTUREPLATE_SHORTCUTHELP`
  - `void __thiscall CCSWnd_List::OnButtonClicked(CCSWnd_List *this, unsigned int a2)`
  - `if ( a2 == 4000 )`
  - `v3 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 132);`
  - `if ( v3 != 0 )`
  - `v4 = sub_4B7940(this: v3);`
  - `v5 = v4 != 0 ? (v4 + 2) : 0;`
  - `sub_4BCEA2(a1: v14);`
  - `LOBYTE(v18) = 2;`
  - `CDialog::CreateDlg(a1: v12, a2: 1, a3: 0);`
  - `LOBYTE(v18) = 1;`
  - `if ( a2 != 0 )`
  - `CDialog::DoModal(a1: v15);`
  - `sub_4BCEBF(a1: v14);`
  - `v7 = *(this + (a2 - 2000) / 3 + 94);`
  - `a2 = (a2 - 2000) % 3;`
  - `if ( v7 < 0 )`
  - …（截断 16 行）

#### CCSWnd_List::OnCreate  (8161 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_104_MAPLESTORY_ERROR__SPRITE_INIT_FAILEDUNKNOWN`, `SP_1217_UI_CASHSHOPIMG_CSLIST_BASE`, `SP_1218_UI_CASHSHOPIMG_CSLIST_ITEMICON`, `SP_2508_UI_CASHSHOPIMG_GUIDEWORDS_D`
  - `void __thiscall CCSWnd_List::OnCreate(CCSWnd_List *this, int Unknown)`
  - `*(this + 144) = Unknown;`
  - `v3 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: v38);`
  - `if ( v3 != 0 )`
  - `v4 = CCtrlSelector::CCtrlSelector(this: v3);`
  - `sub_4BCE07(a1: v4);`
  - `(*(**(this + 28) + 32))(a1: *(this + 28), a2: this, a3: 1000, a4: 4, a5: 412, a6: 406, a7: 0);`
  - `CCtrlSelector::SetSelectorAttr(this: *(this + 28), a2: -12949590, a3: -1, a4: -16777216);`
  - `*(this + 92) = -1;`
  - `*(this + 93) = -1;`
  - `*(this + 481) = -1;`
  - `*(this + 482) = -1;`
  - `*(this + 91) = 0;`
  - `*(this + 480) = 0;`
  - `*(this + 484) = 0;`
  - `sub_402F85(pvargDest: &v41, pvargSrc: v38);`
  - …（截断 16 行）

#### CCSWnd_List::OnKey  (175 B)

  - `signed int __thiscall CCSWnd_List::OnKey(char *this, unsigned int wParam, signed int lParam)`
  - `return CCSWnd_List::OnKeyRet(this: (this - 4), wParam, lParam);`

#### CCSWnd_List::OnKeyRet  (7192 B)

  - `signed int __thiscall CCSWnd_List::OnKeyRet(CCSWnd_List *this, unsigned int wParam, signed int lParam)`
  - `if ( lParam < 0 )`
  - `*(this + 483) = (dword_BF060C)() - dword_BD8ABC - 1;`
  - `v4 = (this + 1932);`
  - `if ( ((dword_BF060C)() - dword_BD8ABC) < *(this + 483) )`
  - `*v4 = dword_BF060C(a1: v28);`
  - `if ( wParam != 33 )`
  - `if ( wParam == 34 )`
  - `if ( CCashShop::IsControlingUserPreview(this: *(this + 144)) == 0 )`
  - `if ( (lParam & 0x80000000) == 0 )`
  - `v10 = *(this + 93);`
  - `if ( v10 != -1 )`
  - `if ( wParam == 13 )`
  - `v24 = *(this + 92);`
  - `if ( v24 >= 0 && v24 < *(this + 91) && v10 <= 2 )`
  - `v25 = *(this + 4 * v24 + 2 * v10 + 2 * v24 + 30) + 4;`
  - …（截断 16 行）

#### CCSWnd_List::OnMouseMove  (1958 B)

  - `int __thiscall CCSWnd_List::OnMouseMove(int *this, int rx, int ry)`
  - `while ( v16 == 0 )`
  - `CCSWnd_List::GetPlateRect(tagRECT: &v11, nPlate: v4);`
  - `if ( dword_BF0484(a1: v12, a2: rx, a3: ry) != 0 && *v15 >= 0 )`
  - `if ( *(this + 485) == v4 )`
  - `if ( v16 == 0 )`
  - `*(this + 485) = -1;`
  - `CWvsCommodityEx::GetCommodityExByIndex(this: *dword_BE7918, a2: v13, a3: *v15);`
  - `sub_4738BF(a1: v13);`
  - `v16 = CCSWnd_List::ShowItemToolTip(this: this - 1, ebx0: v4, a2: v7, pComm: v8, rx: v9, ry: v10);`
  - `if ( v16 != 0 )`
  - `*(this + 485) = v4;`
  - `if ( v14 != 0 )`
  - `sub_473F9E(a1: 0);`
  - `if ( ++v4 >= 10 )`

#### CCSWnd_List::ResetSlotIncComm  (4251 B)

  - `void __thiscall CCSWnd_List::ResetSlotIncComm(CCSWnd_List *this)`
  - `v2 = *(this + 91) <= 0;`
  - `if ( !v2 )`
  - `v3 = (this + 536);`
  - `v20 = (this + 536);`
  - `if ( *(v3 - 40) >= 0 )`
  - `CWvsCommodityEx::GetCommodityExByIndex(this: *dword_BE7918, a2: &v17, a3: *(v3 - 40));`
  - `if ( v18 != 0 )`
  - `Data = TSecType<long>::GetData(a1: (v18 + 16), a2: v1);`
  - `if ( Data / 10000 == 911 || (v5 = Data % 1000, Data / 1000 == 5430) )`
  - `(*(*(*v6 + 4) + 28))(a1: *v6 + 4, a2: v5, a3: 1);`
  - `v7 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v15) + 4);`
  - `if ( v16 != 0 )`
  - `sub_428C44(this: &v15, a2: 0);`
  - `v8 = TSecType<long>::GetData(a1: (v18 + 16), a2: v1) / 1000 % 10;`
  - `if ( v8 <= 5 )`
  - …（截断 16 行）

#### CCSWnd_List::SetButtonKeyFocus  (753 B)

  - `void __thiscall CCSWnd_List::SetButtonKeyFocus(CCtrlButton **this, signed int nPlateIndx, unsigned int nBtnIndx)`
  - `for ( i = 10; i != 0; --i )`
  - `for ( j = 3; j != 0; --j )`
  - `if ( *v4 != 0 )`
  - `CCtrlButton::SetKeyFocus(this: *v4, a2: 0);`
  - `if ( nPlateIndx >= 0 && nPlateIndx < *(this + 91) && nBtnIndx <= 2 )`
  - `v6 = *(this + 4 * nPlateIndx + 2 * nBtnIndx + 2 * nPlateIndx + 30);`
  - `if ( v6 != 0 )`
  - `CCtrlButton::SetKeyFocus(this: v6, a2: 1);`

#### CCSWnd_List::SetCursorToPlate  (664 B)

  - `void __thiscall CCSWnd_List::SetCursorToPlate(CCSWnd_List *this, int a2)`
  - `CCSWnd_List::GetPlateRect(tagRECT: &v8, nPlate: a2);`
  - `v3 = *(this + 1);`
  - `this = (this + 4);`
  - `v4 = (*(v3 + 44))(a1: this);`
  - `v6 = (*(*this + 48))(a1: this);`
  - `CWndMan::SetCursorPos(this: TSingleton<CWndMan>::ms_pInstance, a2: v7, a3: 0);`

#### CCSWnd_List::SetPlateNo  (16287 B)

- SP_ 节点：`SP_1223_UI_CASHSHOPIMG_CSLIST_BTBUY`, `SP_1224_UI_CASHSHOPIMG_CSLIST_BTGIFT`, `SP_1225_UI_CASHSHOPIMG_CSLIST_BTRESERVE`, `SP_1226_UI_CASHSHOPIMG_CSLIST_BTREMOVE`
  - `void __thiscall CCSWnd_List::SetPlateNo(CCSWnd_List *this, int a2)`
  - `*(this + 91) = a2;`
  - `v3 = *(this + 90);`
  - `if ( v3 != 0 )`
  - `(*(*v3 + 12))(a1: v3);`
  - `v4 = (v2 + 120);`
  - `if ( *v66 != 0 )`
  - `(*(**v66 + 12))(a1: *v66);`
  - `while ( v5 < 3 );`
  - `while ( v61 < 30 );`
  - `v6 = *(v2 + 91) <= 0;`
  - `if ( !v6 )`
  - `CCSWnd_List::GetPlateRect(tagRECT: &v42, nPlate: v62);`
  - `LOBYTE(v65) = 1;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `LOBYTE(v65) = 0;`
  - …（截断 16 行）

#### CCSWnd_List::ShowItemToolTip  (10116 B)

  - `int __userpurge CCSWnd_List::ShowItemToolTip@<eax>(`
  - `if ( pComm == 0 || *dword_BE7918 == 0 || dword_BE78D8 == 0 )`
  - `ZRef<CS_COMMODITY>::~ZRef<CS_COMMODITY>(this: &a2);`
  - `Data = TSecType<long>::GetData(a1: pComm + 16, a2: ebx0);`
  - `if ( Data / 10000 == 911 || Data / 1000 == 5430 )`
  - `if ( v9 < 80000000 || v9 >= 90000000 )`
  - `if ( TSecType<long>::GetData(a1: pComm + 16, a2: 10000) / 10000 != 910 )`
  - `v10 = TSecType<long>::GetData(a1: pComm + 16, a2: dword_BE78D8);`
  - `if ( CItemInfo::IsCashItem(a1: v10) == 0 )`
  - `if ( pComm != 0 )`
  - `sub_473F9E(a1: 0);`
  - `if ( !v11 )`
  - `for ( i = 0; ; i += 104 )`
  - `if ( *v55 == 0 || v56 >= *(v12 - 4) )`
  - `v13 = (i + v12);`
  - `if ( v13 != 0 )`
  - …（截断 16 行）

#### CCSWnd_List::Update  (2165 B)

  - `void __thiscall CCSWnd_List::Update(int *this)`
  - `v2 = dword_BF060C(a1: v12);`
  - `if ( *(this + 484) != 0 && v2 - *(this + 483) > dword_BD8AC0 )`
  - `v3 = *(this + 92);`
  - `if ( v3 >= 0 && v3 < *(this + 91) )`
  - `v4 = *(this + v3 + 94) < 0;`
  - `if ( !v4 )`
  - `if ( *dword_BE7918 != 0 )`
  - `Commodity = CWvsCommodityEx::GetCommodityExByIndex(this: *dword_BE7918, a2: v14, a3: *v5);`
  - `LOBYTE(v19) = 1;`
  - `ZRef<CS_COMMODITY>::operator=(this: &v16, a2: Commodity);`
  - `LOBYTE(v19) = 0;`
  - `if ( v15 != 0 )`
  - `sub_473F9E(a1: 0);`
  - `CCSWnd_List::GetPlateRect(tagRECT: &v13, nPlate: *(this + 92));`
  - `sub_4738BF(a1: &v16);`
  - …（截断 16 行）

#### CCSWnd_List::WearCommodity  (2311 B)

  - `signed int __thiscall CCSWnd_List::WearCommodity(_DWORD *this, char a2, int a3)`
  - `if ( TSecType<long>::GetData(a1: (a3 + 16), a2: this) / 10000 == 910 )`
  - `if ( a3 != 0 )`
  - `InterlockedIncrement(lpAddend: (a3 + 4));`
  - `result = CCSWnd_List::GetCommodityGender(a1: v11, a2: v12);`
  - `if ( result != 2 && result != *(v4 + 8236) )`
  - `while ( 1 )`
  - `v8 = *(a3 + 96);`
  - `if ( v8 == 0 || v5 >= *(v8 - 4) )`
  - `CWvsContext::GetCommodityBySN(a1: v13, a2: *(v8 + 4 * v5));`
  - `LOBYTE(v16) = 1;`
  - `sub_4738BF(a1: v13);`
  - `if ( CCSWnd_List::WearItem(a1: this, a2: this, a3: v10, a4: v11, a5: v12) != 0 )`
  - `*(*(this + 144) + 1244) = 1;`
  - `LOBYTE(v16) = 0;`
  - `if ( v14 != 0 )`
  - …（截断 16 行）

#### CCSWnd_List::WearItem  (4101 B)

  - `signed int __userpurge CCSWnd_List::WearItem@<eax>(int a1@<ecx>, int a2@<ebx>, char a3, int a4, int a5)`
  - `if ( TSecType<long>::GetData(a1: (a4 + 16), a2) / 1000000 == 1 )`
  - `Data = TSecType<long>::GetData(a1: (a4 + 16), a2: 1000000);`
  - `CCashShop::OnWear(this: *(a1 + 576), a2: Data, a3: *(a4 + 12), a4: a5);`
  - `v6 = TSecType<long>::GetData(a1: (a4 + 16), a2: 500);`
  - `if ( v6 / 1000000 == 5 && v6 / 10000 == 500 )`
  - `v7 = TSecType<long>::GetData(a1: (a4 + 16), a2: 500);`
  - `CCashShop::SetPet(this: *(a1 + 576), a2: v7, a3: *(a4 + 12), a4: a5);`
  - `if ( a4 != 0 )`
  - `sub_473F9E(a1: 0);`
  - `v9 = TSecType<long>::GetData(a1: (a4 + 16), a2: 500);`
  - `cashslot_item_type = get_cashslot_item_type(a1: v9);`
  - `v11 = (a4 + 16);`
  - `if ( cashslot_item_type == 6 )`
  - `v12 = TSecType<long>::GetData(a1: v11, a2: 500);`
  - `CCashShop::SetEmotion(this: *(a1 + 576), a2: v12);`
  - …（截断 16 行）

### CCSWnd_Inventory（9 函数）


#### CCSWnd_Inventory::Draw  (7941 B)

  - `int *__thiscall CCSWnd_Inventory::Draw(CCSWnd_Inventory *this, const struct tagRECT *a2)`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v45) + 4);`
  - `if ( v46 != 0 )`
  - `sub_428C44(this: &v45, a2: 0);`
  - `CWnd::GetCanvas(a1: &v52);`
  - `v4 = *(v2 + 369);`
  - `*(v2 + 369) = -1;`
  - `if ( v4 >= 0 )`
  - `v5 = *(v2 + 368);`
  - `v6 = *(v3 + 4 * v5 + 1095);`
  - `if ( v6 != 0 )`
  - `v6 = *(v6 - 4);`
  - `if ( v4 < v6 - 1 )`
  - `v7 = *(CharacterData::GetItem(this: v3, a2: &v48, a3: v5, a4: v4) + 4);`
  - `if ( v49 != 0 )`
  - `ZRef<GW_ItemSlotBase>::~ZRef<GW_ItemSlotBase>(this: &v48, a2: 0);`
  - …（截断 16 行）

#### CCSWnd_Inventory::EnableExTrunkButton  (194 B)

  - `int __thiscall CCSWnd_Inventory::EnableExTrunkButton(CCSWnd_Inventory *this)`
  - `return (*(*(*(this + 40) + 4) + 28))(a1: *(this + 40) + 4, a2: *(*(this + 370) + 1164) + 4 <= 48);`

#### CCSWnd_Inventory::GetItemSlotRect  (1177 B)

  - `signed int __thiscall CCSWnd_Inventory::GetItemSlotRect(`
  - `v4 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v8) + 4);`
  - `if ( v9 != 0 )`
  - `sub_428C44(this: &v8, a2: 0);`
  - `v5 = *(v4 + 4 * *(this + 368) + 1095);`
  - `if ( v5 != 0 )`
  - `v5 = *(v5 - 4);`
  - `v6 = *(this + 367);`
  - `if ( nSlotPosition < v6 )`
  - `if ( v6 + 12 < v5 )`
  - `if ( nSlotPosition >= v5 )`
  - `dword_BF0410(a1: tagRECT);`
  - `dword_BF040C(`
  - `a2: 35 * ((nSlotPosition - v6) % 4) + 22,`
  - `a3: 35 * ((nSlotPosition - v6) / 4) + 55,`
  - `a4: 35 * ((nSlotPosition - v6) % 4) + 54,`
  - …（截断 16 行）

#### CCSWnd_Inventory::GetSlotPositionFromPoint  (1117 B)

  - `int __thiscall CCSWnd_Inventory::GetSlotPositionFromPoint(CCSWnd_Inventory *this, int a2, int a3)`
  - `v4 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v11.right) + 4);`
  - `if ( v11.bottom != 0 )`
  - `sub_428C44(this: &v11.right, a2: 0);`
  - `v5 = *(v4 + 4 * *(this + 368) + 1095);`
  - `if ( v5 != 0 )`
  - `v6 = *(v5 - 4);`
  - `v7 = *(this + 367);`
  - `while ( 1 )`
  - `v9 = *(this + 367) + 12;`
  - `if ( v9 >= v8 )`
  - `if ( v7 >= v9 )`
  - `CCSWnd_Inventory::GetItemSlotRect(this, nSlotPosition: v7, tagRECT: &v11);`
  - `if ( dword_BF0484(a1: &v11, a2, a3) != 0 )`

#### CCSWnd_Inventory::OnChildNotify  (4965 B)

- SP_ 节点：`SP_10_EQP`, `SP_11_SETUP`, `SP_5538_ETC`, `SP_5596_USE`, `SP_575_ADDING_SLOTS_AT_THE_STORAGE`, `SP_576_ADD_D_SLOTS_FOR_THE_STORAGE_CURRENT_D_SLOTS_R_NMAXIMUM_OF_48_SLOTS_AVAILABLED_NX`, `SP_578_ADD_S_SLOTS`, `SP_579_ADD_D_SLOTS_FOR_S_INVENTORY_CURRENT_D_SLOTS_R_NMAXIMUM_OF_D_SLOTS_AVAILABLED_NX`
  - `void __thiscall CCSWnd_Inventory::OnChildNotify(_DWORD *this, unsigned int a2, unsigned int a3, char *a4)`
  - `if ( a2 == 1000 )`
  - `if ( a3 == 500 )`
  - `CCSWnd_Inventory::OnTabChanged(this, a2: a4);`
  - `if ( a2 == 1001 )`
  - `if ( a3 < 300 || a3 > 304 )`
  - `CUIToolTip::ClearToolTip(this: (this + 41));`
  - `dword_BED588[*(this + 368)] = a4;`
  - `*(this + 367) = 4 * v5 + 1;`
  - `CWnd::InvalidateRect(this, a2: 0);`
  - `if ( a2 >= 1002 && a2 <= 1005 && a3 == 101 )`
  - `if ( (byte_BED568 & 1) == 0 )`
  - `ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF);`
  - `atexit(a1: loc_4B14C9);`
  - `v22 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v21) + 4);`
  - `sub_4289B7(this: v21);`
  - …（截断 16 行）

#### CCSWnd_Inventory::OnCreate  (15514 B)

- SP_ 节点：`SP_1210_UI_CASHSHOPIMG_CSINVENTORY_BTEXEQUIP`, `SP_1211_UI_CASHSHOPIMG_CSINVENTORY_BTEXCONSUME`, `SP_1212_UI_CASHSHOPIMG_CSINVENTORY_BTEXINSTALL`, `SP_1213_UI_CASHSHOPIMG_CSINVENTORY_BTEXETC`, `SP_1215_UI_CASHSHOPIMG_CSINVENTORY_BTEXTRUNK`, `SP_1253_UI_BASICIMG_ITEMNO`, `SP_3174_UI_CASHSHOPIMG_BASE_TAB2_ENABLE`, `SP_3175_UI_CASHSHOPIMG_BASE_TAB2_DISABLE`
  - `_DWORD *__thiscall CCSWnd_Inventory::OnCreate(CCSWnd_Inventory *this, int i)`
  - `*(this + 370) = i;`
  - `v3 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: v83);`
  - `if ( v3 != 0 )`
  - `v4 = CCtrlTab::CCtrlTab(this: v3);`
  - `sub_42854D(a1: v4);`
  - `(*(**(this + 28) + 8))(a1: *(this + 28), a2: this, a3: 1000, a4: 4, a5: 17, a6: 28, a7: 156, a8: 0);`
  - `v5 = *(this + 28);`
  - `*(v5 + 52) = 1;`
  - `*(v5 + 56) = 0;`
  - `sub_402F85(pvargDest: &v87, pvargSrc: v82);`
  - `sub_402F85(pvargDest: &v88, pvargSrc: &pvargSrc);`
  - `LOBYTE(v103) = 2;`
  - `v8 = sub_406472();`
  - `v9 = sub_402680(a1: v8);`
  - `IWzResMan::GetObjectA(this: v9, pvargDest: &v86, sUOL: v78, vParam: v79, vAux: v80);`
  - …（截断 16 行）

#### CCSWnd_Inventory::OnMouseButton  (1877 B)

  - `void __thiscall CCSWnd_Inventory::OnMouseButton(`
  - `CUIToolTip::ClearToolTip(this: (this + 160));`
  - `CCSWnd_Char::EndChat(this: (*(this + 369) + 1364));`
  - `v6 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v11) + 4);`
  - `if ( v12 != 0 )`
  - `sub_428C44(this: &v11, a2: 0);`
  - `if ( a2 == 513 )`
  - `v7 = (this - 4);`
  - `SlotPositionFromPoint = CCSWnd_Inventory::GetSlotPositionFromPoint(this: (this - 4), a2: a4, a3: a5);`
  - `v8 = *(CharacterData::GetItem(this: v6, a2: &v11, a3: *(this + 367), a4: SlotPositionFromPoint) + 4);`
  - `ZRef<GW_ItemSlotBase>::~ZRef<GW_ItemSlotBase>(this: &v11, a2: 0);`
  - `if ( v8 != 0 && *(v8 + 24) != 0 )`
  - `sub_4B1D81(this: v7, a2: SlotPositionFromPoint);`
  - `else if ( a2 == 515 )`
  - `v9 = CCSWnd_Inventory::GetSlotPositionFromPoint(this: (this - 4), a2: a4, a3: a5);`
  - `v10 = *(CharacterData::GetItem(this: v6, a2: &v11, a3: *(this + 367), a4: v9) + 4);`
  - …（截断 16 行）

#### CCSWnd_Inventory::OnMouseMove  (1453 B)

  - `int __thiscall CCSWnd_Inventory::OnMouseMove(CCSWnd_Inventory *this, int a2, int a3)`
  - `SlotPositionFromPoint = CCSWnd_Inventory::GetSlotPositionFromPoint(this: (this - 4), a2, a3);`
  - `if ( SlotPositionFromPoint == 0 )`
  - `v4 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v10) + 4);`
  - `if ( v11 != 0 )`
  - `sub_428C44(this: &v10, a2: 0);`
  - `v5 = *(CharacterData::GetItem(this: v4, a2: &v10, a3: *(this + 367), a4: SlotPositionFromPoint) + 4);`
  - `ZRef<GW_ItemSlotBase>::~ZRef<GW_ItemSlotBase>(this: &v10, a2: 0);`
  - `if ( v5 != 0`
  - `&& (SlotPositionFromPoint = dword_BE78D8,`
  - `Data = TSecType<long>::GetData(a1: v5 + 12, a2: v5),`
  - `CItemInfo::IsCashItem(a1: Data) != 0) )`
  - `v9 = ((*(*this + 48))(a1: this) + a3 + 20);`
  - `v7 = (*(*this + 44))(a1: this);`
  - `CUIToolTip::ShowItemToolTip(this: this + 40, a2: (a2 + v7), a3: v9, a4: v5, a5: 0, a6: 0, a7: 0, a8: 0, a9: 0);`
  - `CUIToolTip::ClearToolTip(this: (this + 160));`
  - …（截断 16 行）

#### CCSWnd_Inventory::OnTabChanged  (885 B)

  - `void __thiscall CCSWnd_Inventory::OnTabChanged(_DWORD *this, int a2)`
  - `*(this + 368) = a2 + 1;`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v5) + 4);`
  - `if ( v6 != 0 )`
  - `sub_428C44(this: &v5, a2: 0);`
  - `v4 = *(v3 + 4 * *(this + 368) + 1095);`
  - `if ( v4 != 0 )`
  - `v4 = *(v4 - 4);`
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 30), a2: (v4 - 10) / 4 + 1);`
  - `CCtrlScrollBar::SetCurPos(this: *(this + 30), a2: dword_BED588[*(this + 368)]);`
  - `*(this + 367) = 4 * *(*(this + 30) + 56) + 1;`
  - `CUIToolTip::ClearToolTip(this: (this + 41));`
  - `CWnd::InvalidateRect(this, a2: 0);`

### CCSWnd_Locker（8 函数）


#### CCSWnd_Locker::Draw  (7973 B)

  - `signed int *__thiscall CCSWnd_Locker::Draw(CCSWnd_Locker *this, const struct tagRECT *a2)`
  - `CWnd::GetCanvas(a1: &v49);`
  - `v3 = (*(this + 358) + 1160);`
  - `if ( v49 == 0 )`
  - `v4 = *(this + 10);`
  - `v6 = (*(v5 + 140))(a1: v49, a2: 0, a3: 0, a4: 229, a5: v4, a6: 0xFFFFFF);`
  - `if ( v6 < 0 )`
  - `v7 = *(this + 357);`
  - `if ( v7 < 0 || (*v3 != 0 ? (v8 = *(*v3 - 4)) : (v8 = 0), v7 >= v8) )`
  - `*(this + 357) = -1;`
  - `if ( v9 != 0 && *(v9 - 4) != 0 )`
  - `CCSWnd_Locker::ResetScrollBar(this);`
  - `v11 = 2 * (3 * *(*(this + 30) + 56) + 6);`
  - `v48 = 6 * *(*(this + 30) + 56);`
  - `if ( v48 < v11 )`
  - `while ( 1 )`
  - …（截断 16 行）

#### CCSWnd_Locker::GetLockerIndex  (561 B)

  - `int __thiscall CCSWnd_Locker::GetLockerIndex(CCSWnd_Locker *this, int a2, int a3)`
  - `v4 = *(*(this + 30) + 56);`
  - `for ( i = 3 * v4 + 6; v5 < 2 * i; i = 3 * (*(*(this + 30) + 56) + 2) )`
  - `CCSWnd_Locker::GetLockerRect(this, retstr: &v8, a3: v5);`
  - `if ( dword_BF0484(a1: &v8, a2, a3) != 0 )`

#### CCSWnd_Locker::GetLockerRect  (765 B)

  - `struct tagRECT *__thiscall CCSWnd_Locker::GetLockerRect(CCSWnd_Locker *this, struct tagRECT *retstr, int a3)`
  - `memset(&v6, 0, sizeof(v6));`
  - `if ( a3 >= 0 )`
  - `v3 = *(*(this + 358) + 1160);`
  - `if ( v3 != 0 && a3 < *(v3 - 4) )`
  - `v4 = -6 * *(*(this + 30) + 56) + a3;`
  - `if ( v4 < 0xC )`
  - `v6.left = 35 * (v4 % 6) + 21;`
  - `v6.right = 35 * (v4 % 6) + 53;`
  - `v6.top = 35 * (v4 / 6) + 30;`
  - `v6.bottom = 35 * (v4 / 6) + 62;`

#### CCSWnd_Locker::OnCreate  (4421 B)

- SP_ 节点：`SP_1253_UI_BASICIMG_ITEMNO`
  - `void __thiscall CCSWnd_Locker::OnCreate(CCSWnd_Locker *this, void *Unknown)`
  - `*(this + 358) = Unknown;`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v34) + 4);`
  - `v26 = *(v3 + 61);`
  - `v4 = _ZtlSecureFuse<short>(a1: v3 + 57, a2: v26);`
  - `sub_4289B7(this: &v34);`
  - `v6 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v34) + 4);`
  - `v26 = *(v6 + 61);`
  - `v7 = _ZtlSecureFuse<short>(a1: v6 + 57, a2: v26);`
  - `if ( v35 != 0 )`
  - `sub_428C44(this: &v34, a2: 0);`
  - `v9 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 116);`
  - `LOBYTE(v36) = 3;`
  - `if ( v9 != 0 )`
  - `v10 = CCtrlScrollBar::CCtrlScrollBar(this: v9);`
  - `LOBYTE(v36) = 2;`
  - …（截断 16 行）

#### CCSWnd_Locker::OnMouseButton  (3847 B)

- SP_ 节点：`SP_5603_YOUR_INVENTORY_IS_FULL`
  - `void __thiscall CCSWnd_Locker::OnMouseButton(`
  - `CUIToolTip::ClearToolTip(this: (this + 120));`
  - `CCSWnd_Char::EndChat(this: (*(this + 357) + 1364));`
  - `v27 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v25) + 4);`
  - `if ( v26 != 0 )`
  - `sub_428C44(this: &v25, a2: 0);`
  - `if ( a2 == 513 )`
  - `v6 = (this - 4);`
  - `v7 = (*(this + 357) + 1160);`
  - `LockerIndex = CCSWnd_Locker::GetLockerIndex(this: (this - 4), a2: a4, a3: a5);`
  - `if ( LockerIndex >= 0 )`
  - `if ( v9 != 0 && LockerIndex < *(v9 - 4) )`
  - `v10 = (this + 1424);`
  - `if ( *v10 != LockerIndex )`
  - `CWnd::InvalidateRect(this: v6, a2: v24);`
  - `else if ( a2 == 515 )`
  - …（截断 16 行）

#### CCSWnd_Locker::OnMouseMove  (1905 B)

  - `int __thiscall CCSWnd_Locker::OnMouseMove(CCSWnd_Locker *this, int a2, int a3)`
  - `v4 = (*(this + 357) + 1160);`
  - `LockerIndex = CCSWnd_Locker::GetLockerIndex(this: (this - 4), a2, a3);`
  - `if ( LockerIndex >= 0 && (v6 = *v4, *v4 != 0) && LockerIndex < *(v6 - 4) )`
  - `CItemInfo::GetItemSlot(a1: &v16, a2: *(55 * LockerIndex + v6 + 16));`
  - `*(v17 + 32) = *(v8 + v7 + 39);`
  - `*(v17 + 24) = *(*v4 + v7);`
  - `*(v17 + 28) = *(v9 + v7 + 4);`
  - `CUIToolTip::ItemToolTipParam::ItemToolTipParam(this: v15);`
  - `LOBYTE(v18) = 1;`
  - `v13 = ((*(v10 + 48))(a1: this) + a3 + 20);`
  - `v11 = (*(*this + 44))(a1: this);`
  - `CUIToolTip::ShowItemToolTip(`
  - `a2: (v11 + a2 + 20),`
  - `LOBYTE(v18) = 0;`
  - `CUIToolTip::ItemToolTipParam::~ItemToolTipParam(this: v15);`
  - …（截断 16 行）

#### CCSWnd_Locker::ResetScrollBar  (286 B)

  - `void __thiscall CCSWnd_Locker::ResetScrollBar(CCSWnd_Locker *this)`
  - `v1 = *(*(this + 358) + 1160);`
  - `if ( v1 != 0 )`
  - `v1 = *(v1 - 4);`
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 30), a2: (v1 + 5) / 6u - 1);`

#### CCSWnd_Locker::SetSelectedNo  (658 B)

  - `void __thiscall CCSWnd_Locker::SetSelectedNo(CCtrlScrollBar **this, int a2)`
  - `CCSWnd_Locker::ResetScrollBar(this);`
  - `if ( a2 == 0 && (v3 = *(*(this + 358) + 290)) != 0 && *(v3 - 4) != 0 )`
  - `*(this + 357) = (*(v3 - 4) - 1);`
  - `*(this + 357) = -1;`
  - `v4 = *(this + 30);`
  - `v5 = *(v4 + 15);`
  - `if ( v5 > 1 )`
  - `CCtrlScrollBar::SetCurPos(this: v4, a2: v5 - 1);`
  - `CUIToolTip::ClearToolTip(this: (this + 31));`
  - `CWnd::InvalidateRect(this, a2: 0);`

### CCSWnd_Tab（7 函数）


#### CCSWnd_Tab::ChangeCategory  (3466 B)

- SP_ 节点：`SP_4842_THIS_IS_YOUR_SEARCH_RESULT`, `SP_568_THIS_IS_YOUR_WISH_LIST`, `SP_570_THE_ITEM_IS_BEING_PREPARED_WERE_SORRY_FOR_THE_INCONVENIENCE`
  - `void __thiscall CCSWnd_Tab::ChangeCategory(CCSWnd_Tab *this)`
  - `v2 = *(this + 42);`
  - `*(this + 41) = v2[331];`
  - `sub_471B77(this: v2, a2: v2[331], a3: &v17, a4: &v18);`
  - `v3 = *(*(this + 42) + 1324);`
  - `v4 = *(this + 28);`
  - `if ( v3 == 10 )`
  - `CCtrlSelector::SetSelectorAttr(this: v4, a2: -12346660, a3: -1, a4: -1);`
  - `CCtrlSelector::SetSelectorAttr(`
  - `CCtrlSelector::SetSelectorStart(this: *(this + 28), a2: 0, a3: 20, a4: 0, a5: 1);`
  - `CCtrlSelector::SetSelectorEnd(a1: *(this + 28));`
  - `v5 = *(*(this + 42) + 1324);`
  - `if ( v5 == 10 )`
  - `CCtrlSelector::SetSelectorStart(this: *(this + 28), a2: 1, a3: 20, a4: 0, a5: 1);`
  - `sub_4BCE03(a1: this + 108);`
  - `sub_4D9521(a1: 0, a2: v15[0]);`
  - …（截断 16 行）

#### CCSWnd_Tab::Draw  (2081 B)

  - `const struct tagRECT *__thiscall CCSWnd_Tab::Draw(CCSWnd_Tab *this, const struct tagRECT *pRect)`
  - `CWnd::Draw(this, a2: pRect);`
  - `CWnd::GetCanvas(a1: &pRect);`
  - `VariantInit(&pvarg);`
  - `v3 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v3 < 0 )`
  - `v4 = *(this + 41);`
  - `LOBYTE(v14) = 1;`
  - `if ( v4 != 8 )`
  - `if ( v4 == 9 )`
  - `if ( v4 != 10 )`
  - `v6 = *(this + v5 + 31);`
  - `if ( pRect == 0 )`
  - `v8 = (*(left + 128))(`
  - `if ( v8 < 0 )`
  - `LOBYTE(v14) = 0;`
  - …（截断 16 行）

#### CCSWnd_Tab::GetTabIndex  (791 B)

  - `signed int __thiscall CCSWnd_Tab::GetTabIndex(signed int *this, int a2, signed int a3)`
  - `if ( a3 >= 22 && a3 <= 52 )`
  - `v3 = *(this + 41);`
  - `while ( result >= v3 )`
  - `if ( result > v3 )`
  - `if ( v6 <= 0x32 )`
  - `if ( v5 > nullsub_55 )`
  - `v6 = a2 - *(v5 - 1);`

#### CCSWnd_Tab::OnChildNotify  (533 B)

  - `void __thiscall CCSWnd_Tab::OnChildNotify(CCashShop **this, int nId, int param1, int a4)`
  - `if ( nId == 1000 )`
  - `if ( param1 == 600 )`
  - `CCashShop::OnChangedSortType(this: *(this + 42), a2: a4);`
  - `if ( param1 == 800 )`
  - `CCashShop::OnChangedCategorySub(this: *(this + 42), a2: a4);`
  - `if ( param1 == 100 )`
  - `(*(*this + 8))(a1: this, a2: nId);`

#### CCSWnd_Tab::OnCreate  (4059 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_104_MAPLESTORY_ERROR__SPRITE_INIT_FAILEDUNKNOWN`, `SP_1216_UI_CASHSHOPIMG_CSTAB_TAB_D`
  - `void __thiscall CCSWnd_Tab::OnCreate(CCSWnd_Tab *this, void *a2)`
  - `*(this + 42) = a2;`
  - `v3 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: v21);`
  - `if ( v3 != 0 )`
  - `v4 = CCtrlSelector::CCtrlSelector(this: v3);`
  - `sub_4BCE07(a1: v4);`
  - `(*(**(this + 28) + 32))(a1: *(this + 28), a2: this, a3: 1000, a4: 8, a5: 59, a6: 485, a7: 0);`
  - `LOBYTE(v30) = 2;`
  - `sub_416821(a1: &v28, a2: v6, a3: v31);`
  - `LOBYTE(v30) = 1;`
  - `VariantInit(&pvarg);`
  - `v7 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v7 < 0 )`
  - `LOBYTE(v30) = 3;`
  - `VariantInit(pvarg: &pvargDest);`
  - `v8 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - …（截断 16 行）

#### CCSWnd_Tab::OnKeyRet  (2584 B)

- SP_ 节点：`SP_1231_TAB`
  - `signed int __thiscall CCSWnd_Tab::OnKeyRet(CCSWnd_Tab *this, unsigned int a2, int a3)`
  - `if ( a3 < 0 )`
  - `*(this + 43) = (dword_BF060C)() - dword_BD8AB8 - 1;`
  - `v4 = (this + 172);`
  - `if ( ((dword_BF060C)() - dword_BD8AB8) >= *(this + 43) )`
  - `*v4 = dword_BF060C(a1: v13);`
  - `if ( a2 < 0x31 || a2 > 0x39 )`
  - `switch ( a2 )`
  - `if ( *(*(this + 42) + 1324) != v5 )`
  - `play_ui_sound(a1: *StringW);`
  - `CCashShop::OnChangedCategory(this: *(this + 42), a2: v5);`
  - `CWnd::InvalidateRect(this, a2: 0);`
  - `if ( *(this + 41) == 9 )`
  - `if ( CInputSystem::IsKeyPressed(this: dword_BEC33C, a2: 160) != 0`
  - `|| CInputSystem::IsKeyPressed(this: dword_BEC33C, a2: 161) != 0 )`
  - `v5 = (v6 + *(this + 41) + 7) % 8 + 1;`
  - …（截断 16 行）

#### CCSWnd_Tab::OnMouseButton  (890 B)

- SP_ 节点：`SP_1231_TAB`
  - `void __thiscall CCSWnd_Tab::OnMouseButton(_DWORD *this, int msg, int wParam, int rx, signed int ry)`
  - `CCSWnd_Char::EndChat(this: (*(this + 41) + 1364));`
  - `if ( msg == 513 )`
  - `TabIndex = CCSWnd_Tab::GetTabIndex(this: this - 1, a2: rx, a3: ry);`
  - `if ( TabIndex != 0 )`
  - `play_ui_sound(a1: *StringW);`
  - `*(this + 40) = TabIndex;`
  - `CCashShop::OnChangedCategory(this: *(this + 41), a2: TabIndex);`
  - `CWnd::InvalidateRect(this: (this - 1), a2: 0);`

### CCSWnd_Best（5 函数）


#### CCSWnd_Best::ChangeCategory  (597 B)

  - `void __thiscall CCSWnd_Best::ChangeCategory(CWnd *this)`
  - `v2 = *(*(this + 32) + 1324);`
  - `if ( v2 == 9 || v2 == 10 )`
  - `v3 = 60 * (*(*dword_BE7918 + 8236) + 2 * v2);`
  - `v4 = (this + 108);`
  - `for ( i = 5; i != 0; --i )`
  - `*v4 = CCashShop::GetUsableCommodityIndexBySN(this: *(this + 32), a2: (v3 + *(this + 32) + 88));`
  - `CWnd::InvalidateRect(this, a2: 0);`

#### CCSWnd_Best::Draw  (13762 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_5639_MESO`, `SP_571__NX`
  - `struct tagRECT *__thiscall CCSWnd_Best::Draw(CCSWnd_Best *this, struct tagRECT *a2)`
  - `CWnd::Draw(this, a2);`
  - `CWnd::GetCanvas(a1: &a2);`
  - `if ( a2 == 0 )`
  - `v4 = (*(a2->left + 140))(a1: a2, a2: 0, a3: 0, a4: *(v2 + 9), a5: *(v2 + 10), a6: 0xFFFFFF);`
  - `if ( v4 < 0 )`
  - `sub_402F85(pvargDest: &v58, pvargSrc: &pvargSrc);`
  - `LOBYTE(v81) = 1;`
  - `sub_402F85(pvargDest: &v60, pvargSrc: &pvargSrc);`
  - `HIDWORD(v55) = &v58;`
  - `LODWORD(v55) = &v60;`
  - `LOBYTE(v81) = 2;`
  - `_bstr_t::_bstr_t(a1: str_Ui_19);`
  - `v6 = sub_406472();`
  - `v7 = sub_402680(a1: v6);`
  - `IWzResMan::GetObjectA(this: v7, pvargDest: &v57, sUOL: v54, vParam: v55, vAux: SHIDWORD(v55));`
  - …（截断 16 行）

#### CCSWnd_Best::GetBestRect  (277 B)

  - `struct tagRECT *__stdcall CCSWnd_Best::GetBestRect(struct tagRECT *retstr, int nBest)`

#### CCSWnd_Best::OnCreate  (153 B)

  - `int __thiscall CCSWnd_Best::OnCreate(_DWORD *this, int a2)`
  - `*(this + 32) = a2;`
  - `return CCSWnd_Best::ChangeCategory(this);`

#### CCSWnd_Best::OnMouseButton  (8338 B)

- SP_ 节点：`SP_3559_____R_N10___R_N33333_____R_N___R_N____R_N_`, `SP_3560_33333_____R_N______R_N__R_N___D_`, `SP_3745_THIS_MEGAPHONE_IS_ONLY_AVAILABLE_FOR__R_NCHARACTERS_THAT_ARE_OVER_LEVEL_10`, `SP_3746_HIRED_MERCHANTS_SHOP_IS_ONLY_AVAILABLE_FOR__R_NCHARACTERS_THAT_ARE_OVER_LEVEL_15`, `SP_3748_THIS_TELEPORT_ROCK_IS_ONLY_AVAILABLE_FOR__R_NCHARACTERS_THAT_ARE_OVER_LEVEL_6`, `SP_3749_THIS_ITEM_CAN_ONLY_BE_USED_15_DAYS__R_NAFTER_THE_ACCOUNTS_REGISTRATION`
  - `void __thiscall CCSWnd_Best::OnMouseButton(_DWORD *this, int a2, int a3, int a4, int a5)`
  - `v6 = (*(this + 31) + 1364);`
  - `CCSWnd_Char::EndChat(this: v6);`
  - `while ( 1 )`
  - `CCSWnd_Best::GetBestRect(retstr: &v41, nBest: v47);`
  - `if ( dword_BF0484(a1: &v41, a2: a4, a3: a5) != 0 )`
  - `if ( ++v47 >= 5 )`
  - `if ( v47 == -1 )`
  - `if ( v47 == 4 )`
  - `v8 = *(this + 31);`
  - `if ( *(v8 + 317) != 0 )`
  - `if ( a2 != 513 )`
  - `if ( *(v8 + 331) != 9 )`
  - `CCashShop::OnChangedCategory(this: v8, a2: 9);`
  - `for ( i = 1180; i < 1220; i += 4 )`
  - `if ( *(i + *(this + 31)) == 0 )`
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

### CUserPreview（7 函数）


#### CUserPreview::DoMeleeAttack  (2870 B)

  - `int __thiscall CUserPreview::DoMeleeAttack(CUserPreview *this, int a2, int a3)`
  - `v18 = CRand32::Random(a1: *dword_BE7918 + 13620);`
  - `if ( CAvatar::GetOneTimeAction(this: (this + 136)) > -1 || CUser::IsOnLadderOrRope(this) != 0 )`
  - `return sub_95369A(a1: v10);`
  - `if ( a2 != 0 && a3 == 0 )`
  - `v5 = *(this + 318);`
  - `if ( (*(this + 348) & 0xFFFFFFFE) == 0xA )`
  - `weapon_type = get_weapon_type(nItemID: a2);`
  - `random_melee_attack_action = get_random_melee_attack_action(a1: v5, a2: weapon_type, a3: 0, a4: 0, a5: v18);`
  - `if ( random_melee_attack_action < 0 )`
  - `CUser::SetAttackAction(this, a2: random_melee_attack_action, a3: 6, a4: 0, a5: 0);`
  - `sub_405210(a1: this + 1288);`
  - `sub_936F34(a1: &v19, a2: 0, a3: 0, a4: v16, a5: 0);`
  - `LOBYTE(v20) = 1;`
  - `if ( v19 != 0 && *v19 != 0 )`
  - `v9 = *(*v19 - 4) >> 1;`
  - …（截断 16 行）

#### CUserPreview::DoShootAttack  (11108 B)

  - `int __thiscall CUserPreview::DoShootAttack(CUserPreview *this, unsigned __int64 arg0)`
  - `a2 = CRand32::Random(a1: *dword_BE7918 + 13620);`
  - `if ( CAvatar::GetOneTimeAction(this: (this + 136)) > -1 || CUser::IsOnLadderOrRope(this) != 0 || *(this + 2746) != 0 )`
  - `if ( (*(this + 348) & 0xFFFFFFFE) == 0xA )`
  - `CUserPreview::DoMeleeAttack(this, a2: arg0, a3: *(this + 318));`
  - `return sub_95369A(a1: v4);`
  - `update_time = get_update_time();`
  - `weapon_type = get_weapon_type(nItemID: arg0);`
  - `random_shoot_attack_action = get_random_shoot_attack_action(a1: *(this + 318), a2: weapon_type, a3: 0, a4: 0, a5: a2);`
  - `if ( random_shoot_attack_action < 0 )`
  - `CUser::SetAttackAction(this, a2: random_shoot_attack_action, a3: 6, a4: 0, a5: 0);`
  - `sub_405210(a1: this + 1288);`
  - `sub_936F34(a1: &v75, a2: 0, a3: 0, a4: Lo64, a5: HIDWORD(Lo64));`
  - `LOBYTE(v77) = 1;`
  - `if ( v75 != 0 && *v75 != 0 )`
  - `v10 = *(*v75 - 4) >> 1;`
  - …（截断 16 行）

#### CUserPreview::Init  (2099 B)

  - `LONG __thiscall CUserPreview::Init(CUserPreview *this)`
  - `v3 = *(*dword_BE7918 + 8352);`
  - `*(this + 1130) = v3;`
  - `v4 = *(v1 + 8376);`
  - `if ( v4 != 0 )`
  - `v5 = (v4 + 4);`
  - `ZXString<char>::GetBuffer(a1: v5, a2: 0xFFFFFFFF);`
  - `*(this + 1971) = _ZtlSecureTear<long>(a1: 0, a2: this + 7876);`
  - `*(this + 1974) = _ZtlSecureTear<long>(a1: 0, a2: this + 7888);`
  - `*(this + 348) = 6;`
  - `Instance = CVecCtrlUserPreview::CreateInstance();`
  - `v6 = _com_ptr_t<_com_IIID<IWzVector2D,&__s_GUID const _GUID_f28bd1ed_3deb_4f92_9eec_10ef5a1c3fb4>>::_QueryInterface<CVecCtrlUserPreview *>(a1: &Instance);`
  - `if ( v6 < 0 && v6 != -2147467262 )`
  - `CVecCtrl::Init(this: Instance, a2: this != 0 ? (this + 4) : 0);`
  - `(*(*Instance + 4))(a1: Instance, a2: 1, a3: -350, a4: -250, a5: 0, a6: 0, a7: *(this + 348), a8: 0);`
  - `*(this + 2746) = 0;`
  - …（截断 16 行）

#### CUserPreview::InitObjects  (1039 B)

- SP_ 节点：`SP_958_SHAPE2DVECTOR2D`
  - `int __thiscall CUserPreview::InitObjects(CUserPreview *this)`
  - `*(this + 2748) = 0;`
  - `sub_428712(a1: 0);`
  - `PcCreateObject::IWzVector2D(a1: v4, a2: this + 11000, a3: 0);`
  - `*(this + 2753) = 0;`
  - `sub_581FAA(a1: 0);`
  - `*(this + 2755) = 0;`
  - `*(this + 2756) = 0;`
  - `result = sub_428712(a1: 0);`
  - `*(this + 2768) = 0;`

#### CUserPreview::ShowADBoard  (1574 B)

- SP_ 节点：`SP_2727_SAMPLE`
  - `void __thiscall CUserPreview::ShowADBoard(_DWORD *this, int a2, int a3)`
  - `if ( *(this + 2768) != 0 )`
  - `sub_581FAA(a1: 0);`
  - `*(this + 2768) = 0;`
  - `CAnimationDisplayer::LoadLayer(this: &v11, a2: this + 1104);`
  - `sub_4145AB(a1: this + 1108);`
  - `LOBYTE(v17) = 1;`
  - `LOBYTE(v17) = 2;`
  - `v7 = sub_428226(a1: String);`
  - `_bstr_t::_bstr_t(a1: v7);`
  - `LOBYTE(v17) = 3;`
  - `CChatBalloon::MakeADBoardBalloon(this: (this + 5), a2: v9, a3: v10, a4: v11, a5: v12);`
  - `*(this + 2768) = 1;`

#### CUserPreview::ShowFireCrack  (16835 B)

- SP_ 节点：`SP_5624_DELAY`
  - `int __thiscall CUserPreview::ShowFireCrack(CUserPreview *this, int a2)`
  - `v96 = *(this + 1129) != 0 ? *(this + 1129) - 12 : 0;`
  - `v107 = (this + 11048);`
  - `ZXString<char>::Format(a1: &v102, a2: aMapMaphelperIm, a3: v101[0]);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v110) = 1;`
  - `sub_402F85(pvargDest: &v78, pvargSrc: &pvargSrc);`
  - `LOBYTE(v110) = 2;`
  - `v2 = alloca(2 * dword_BF03D0(a1: 0, a2: 0, a3: v102, a4: -1, a5: 0, a6: 0));`
  - `v3 = sub_428226(a1: &v102);`
  - `v4 = sub_46ABB2(a1: v3, a2: p_pvarg, a3: v63);`
  - `HIDWORD(v103) = &v63;`
  - `_bstr_t::_bstr_t(a1: v4);`
  - `v5 = sub_406472();`
  - `v6 = sub_402680(a1: v5);`
  - `IWzResMan::GetObjectA(this: v6, pvargDest: p_pvarg, sUOL: v63, vParam: v64, vAux: p_pvargDest);`
  - …（截断 16 行）

#### CUserPreview::Update  (3312 B)

  - `int __thiscall CUserPreview::Update(_DWORD *this)`
  - `v2 = *(this + 1129) != 0 ? (*(this + 1129) - 12) : 0;`
  - `CVecCtrl::UpdateActive(this: v2);`
  - `(*(*(this + 34) + 16))(a1: this + 34, a2: *(v2 + 82), a3: 0);`
  - `if ( get_update_time() - *(this + 2744) > *(this + 2745) )`
  - `if ( *(this + 2747) != 0 )`
  - `*(this + 2747) = 0;`
  - `*(this + 2746) = 0;`
  - `else if ( *(this + 2746) != 0 )`
  - `sub_97931C(a1: this);`
  - `if ( *(this + 2749) != 0 )`
  - `VariantInit(&pvarg);`
  - `v3 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v3 < 0 )`
  - `VariantInit(pvarg: &pvargDest);`
  - `v4 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - …（截断 16 行）

### CVecCtrlUserPreview（7 函数）


#### CVecCtrlUserPreview::BeginUpdateActive  (138 B)

  - `int __thiscall CVecCtrlUserPreview::BeginUpdateActive(CVecCtrl *this)`
  - `return CVecCtrl::BeginUpdateActive(this);`

#### CVecCtrlUserPreview::CreateInstance  (325 B)

  - `static int CVecCtrlUserPreview::CreateInstance()`
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 584) != 0 )`
  - `v0 = sub_9CF54F();`
  - `if ( v0 == 0 )`
  - `_com_raise_error(a1: -2147024882, a2: 0);`

#### CVecCtrlUserPreview::EndUpdateActive  (2233 B)

  - `void __thiscall CVecCtrlUserPreview::EndUpdateActive(CVecCtrlUserPreview *this)`
  - `v2 = *(this + 115);`
  - `v4 = _ZtlSecureFuse<double>(a1: this + 32, a2: *(this + 12));`
  - `if ( v4 < dbl_AF0DE8 )`
  - `*(v3 + 4) = _ZtlSecureTear<double>(a1: SLOBYTE(v12), a2: HIDWORD(v16));`
  - `v6 = _ZtlSecureFuse<double>(a1: (v3 + 24), a2: *(v3 + 10));`
  - `if ( v6 < dbl_AF0DE8 )`
  - `*(v3 + 10) = _ZtlSecureTear<double>(a1: SLOBYTE(v13), a2: HIDWORD(v17));`
  - `v8 = _ZtlSecureFuse<double>(a1: (v3 + 48), a2: *(v3 + 16));`
  - `if ( v8 < dbl_AF0DE8 )`
  - `*(v3 + 16) = _ZtlSecureTear<double>(a1: SLOBYTE(v14), a2: HIDWORD(v18));`
  - `v10 = _ZtlSecureFuse<double>(a1: (v3 + 72), a2: *(v3 + 22));`
  - `if ( v10 < dbl_AF0DE8 )`
  - `*(v3 + 22) = _ZtlSecureTear<double>(a1: SLOBYTE(v15), a2: HIDWORD(v19));`
  - `if ( *(this + 68) != 0 )`
  - `RelPos::SetFromAbsPos(this: (this + 224), a2: v3, a3: *(this + 68));`
  - …（截断 16 行）

#### CVecCtrlUserPreview::IsAbleToClimbLadderOrRope  (176 B)

  - `BOOL __thiscall CVecCtrlUserPreview::IsAbleToClimbLadderOrRope(CVecCtrlUserPreview *this)`
  - `return CAvatar::GetOneTimeAction(this: (dword_BED5A4 + 136)) <= -1;`

#### CVecCtrlUserPreview::IsStun  (120 B)

  - `int __thiscall CVecCtrlUserPreview::IsStun(CVecCtrlUserPreview *this)`
  - `return *(dword_BED5A4 + 11020);`

#### CVecCtrlUserPreview::WorkUpdateActive  (10176 B)

  - `signed int __thiscall CVecCtrlUserPreview::WorkUpdateActive(CVecCtrlUserPreview *this, signed int a2)`
  - `if ( CVecCtrlUserPreview::IsStun(this) == 0 )`
  - `v59 = *(TSingleton<CWvsApp>::ms_pInstance + 4);`
  - `if ( dword_BF04E0() == v59 && CWnd::IsFocused(this: TSingleton<CWndMan>::ms_pInstance) && dword_BE7910 == 0 )`
  - `if ( dword_BEDED4 != 0`
  - `&& (v5 = dword_BEDED4 + 4, (*(*(dword_BEDED4 + 4) + 72))(a1: dword_BEDED4 + 4, a2: &dword_BEC310) != 0) )`
  - `v6 = (*(*v5 + 72))(a1: v5, a2: &dword_BEC310) != 0 ? v60 : 0;`
  - `v7 = (dword_BED5A4 + 136);`
  - `if ( CAvatar::GetOneTimeAction(this: (dword_BED5A4 + 136)) > -1`
  - `|| v6 != 0 && CCashShop::IsControlingUserPreview(this: v6) == 0 )`
  - `v4 = CAvatar::GetOneTimeAction(this: v7) == 32;`
  - `v9 = -(CInputSystem::IsKeyPressed(this: dword_BEC33C, a2: 37) != 0);`
  - `v3 = (CInputSystem::IsKeyPressed(this: v8, a2: 39) != 0) + v9;`
  - `v10 = -(CInputSystem::IsKeyPressed(this: dword_BEC33C, a2: 38) != 0);`
  - `v4 = (CInputSystem::IsKeyPressed(this: v61, a2: 40) != 0) + v10;`
  - `CVecCtrl::SetInput(this, a2: v3, a3: v4);`
  - …（截断 16 行）

#### CVecCtrlUserPreview::WorkUpdateActiveLadderOrRope  (1984 B)

  - `void __thiscall CVecCtrlUserPreview::WorkUpdateActiveLadderOrRope(CVecCtrlUserPreview *this)`
  - `if ( *(this + 90) != 0 )`
  - `CVecCtrl::Jump(this);`
  - `*(dword_BED5A4 + 10884) = _ZtlSecureFuse<long>(a1: this + 336, a2: *(this + 86));`
  - `if ( *(this + 98) != 0 )`
  - `CVecCtrl::Impact(this);`
  - `v2 = _ZtlSecureFuse<CLadderOrRope *>(a1: this + 73, a2: *(this + 75));`
  - `if ( v2 != 0 )`
  - `v9 = _ZtlSecureFuse<double>(a1: this + 56, a2: *(this + 18));`
  - `v6 = _ZtlSecureFuse<long>(a1: this + 348, a2: *(this + 89)) * dbl_AF3728 + v9;`
  - `*(this + 18) = _ZtlSecureTear<double>(a1: SLOBYTE(v6), a2: HIDWORD(v6));`
  - `if ( _ZtlSecureFuse<long>(a1: this + 348, a2: *(this + 89)) < 0`
  - `&& _ZtlSecureFuse<double>(a1: this + 56, a2: *(this + 18)) < v2[4] )`
  - `if ( v2[2] == 0 )`
  - `v7 = *(_ZtlSecureFuse<CLadderOrRope *>(a1: this + 73, a2: *(this + 75)) + 16);`
  - `*(this + 18) = _ZtlSecureTear<double>(a1: SLOBYTE(v7), a2: HIDWORD(v7));`
  - …（截断 16 行）

### PAVCVecCtrlUserPreview（1 函数）


#### PAVCVecCtrlUserPreview::$_QueryInterface  (878 B)

  - `int __thiscall _com_ptr_t<_com_IIID<IWzVector2D,&__s_GUID const _GUID_f28bd1ed_3deb_4f92_9eec_10ef5a1c3fb4>>::_QueryInterface<CVecCtrlUserPreview *>(`
  - `if ( *a2 != 0 )`
  - `v3 = (**(*a2 + 12))(a1: *a2 + 12, a2: dword_BD8348, a3: &a2);`
  - `if ( v3 >= 0 )`
  - `if ( *this != 0 )`
  - `(*(**this + 8))(a1: *this);`
  - `_com_ptr_t<_com_IIID<IWzVector2D,&__s_GUID const _GUID_f28bd1ed_3deb_4f92_9eec_10ef5a1c3fb4>>::operator=(a1: 0);`

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

### CAvatar（4 函数）


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

#### CAvatar::SetRidingVehicle  (357 B)

  - `int __userpurge CAvatar::SetRidingVehicle@<eax>(CAvatar *this@<ecx>, int a2, int a3)`
  - `if ( *(this + 280) == 0 )`
  - `*(this + 303) = a2;`
  - `CAvatar::ClearActionLayer(this, a2: 0);`
  - `return (*(*this + 20))(a1: this, a2: 6, a3: 100, a4: 0);`

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

### VCAvatar（1 函数）


#### VCAvatar::0?$ZRefCountedDummy  (374 B)

  - `_DWORD *__thiscall ZRefCountedDummy<CAvatar>::ZRefCountedDummy<CAvatar>(_DWORD *this)`
  - `memset(a1: this, a2: 0, a3: 0xCu);`
  - `*(this + 3) = &off_AF0C68;`
  - `CAvatar::CAvatar(this: (this + 4));`
  - `*(this + 3) = &off_AF0C60;`

### CUIAdminShopWishList（7 函数）


#### CUIAdminShopWishList::CanAddList  (771 B)

  - `int __thiscall CUIAdminShopWishList::CanAddList(int this, int a2, char a3)`
  - `if ( *(this + 112) != 0 )`
  - `if ( (a2 >= *(this + 120) && *(this + 124) >= a2 || a2 >= *(this + 128) && *(this + 132) >= a2)`
  - `&& CItemInfo::IsScannableItem(this: dword_BE78D8, a2, a3: 0) != 0 )`
  - `if ( ZXString<char>::Find(a1: *(this + 136), a2: 0, a3: 1) < 0`
  - `|| CItemInfo::IsScannableItem(this: dword_BE78D8, a2, a3: 0) == 0 )`

#### CUIAdminShopWishList::CloseAdminShopWishList  (770 B)

- SP_ 节点：`SP_2302_MENUUP`
  - `void __thiscall CUIAdminShopWishList::CloseAdminShopWishList(CUIAdminShopWishList *this)`
  - `play_ui_sound(a1: v2);`
  - `if ( dword_BE791C != 0 )`
  - `CWnd::Destroy(a1: dword_BE791C);`
  - `(**(dword_BE791C + 8))(a1: dword_BE791C + 8, a2: 1);`

#### CUIAdminShopWishList::OnButtonClicked  (1016 B)

  - `void __thiscall CUIAdminShopWishList::OnButtonClicked(CUIAdminShopWishList *this, unsigned int a2)`
  - `switch ( a2 )`
  - `CUIAdminShopWishList::ToggleAddOn(this, a2: *(this + 44) == 0);`
  - `v2 = sub_4B1F6F(this: this + 41);`
  - `CCtrlEdit::GetText(this: v2, &a2);`
  - `v3 = sub_538A95(a1: 0);`
  - `ZXString<char>::operator=(a1: v3);`
  - `CUIAdminShopWishList::SearchItemName(a1: v4[0]);`
  - `CUIAdminShopWishList::CloseAdminShopWishList(this);`

#### CUIAdminShopWishList::OnKey  (866 B)

  - `void __thiscall CUIAdminShopWishList::OnKey(CUIAdminShopWishList *this, unsigned int a2, int *a3)`
  - `if ( SHIBYTE(a3) >= 0 )`
  - `if ( a2 == 13 )`
  - `v3 = sub_4B1F6F(this: this + 40);`
  - `CCtrlEdit::GetText(this: v3, &a2);`
  - `v4 = sub_538A95(a1: 0);`
  - `ZXString<char>::operator=(a1: v4);`
  - `CUIAdminShopWishList::SearchItemName(a1: v5);`
  - `else if ( a2 == 27 )`
  - `CUIAdminShopWishList::CloseAdminShopWishList(this: (this - 4));`

#### CUIAdminShopWishList::SearchItemName  (1277 B)

  - `void __thiscall CUIAdminShopWishList::SearchItemName(int this, _BYTE *a2)`
  - `if ( a2 != 0 && *a2 != 0 )`
  - `ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF);`
  - `LOBYTE(v8) = 1;`
  - `for ( i = 0; ; ++i )`
  - `v5 = v3 != 0 ? *(v3 - 1) : 0;`
  - `if ( i >= v5 )`
  - `if ( v3[i] != 32 )`
  - `ZXString<char>::_Cat(a1: &v7, a2: 1u);`
  - `ZXString<char>::operator=(a1: &v6);`
  - `if ( *(this + 208) == 0 )`
  - `CUIAdminShopWishList::ToggleAddOn(this, a2: 2);`
  - `*(this + 112) = 0;`
  - `CUIAdminShopWishListSearchResult::Reset(this: *(this + 184));`
  - `CWnd::InvalidateRect(this: *(this + 184), a2: 0);`
  - `LOBYTE(v8) = 0;`
  - …（截断 16 行）

#### CUIAdminShopWishList::SendRegisterPacket  (519 B)

  - `int __stdcall CUIAdminShopWishList::SendRegisterPacket(int a1)`
  - `COutPacket::COutPacket(a1: 68);`
  - `COutPacket::Encode1(this: &v2, a2: 3);`
  - `COutPacket::Encode4(this: &v2, a2: a1);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v2);`
  - `return ZArray<unsigned char>::RemoveAll(a1: v3);`

#### CUIAdminShopWishList::ToggleAddOn  (3106 B)

  - `char *__thiscall CUIAdminShopWishList::ToggleAddOn(CUIAdminShopWishList *this, char a2)`
  - `if ( (a2 & 3) != 0 )`
  - `if ( (a2 & 3) != 1 )`
  - `result = ((a2 & 3) - 2);`
  - `if ( (a2 & 3) != 2 )`
  - `if ( *(this + 51) != 0 && *(this + 44) != 0 )`
  - `sub_7D0312(a1: this + 172);`
  - `CWnd::GetLayer(a1: &v9);`
  - `sub_7CFC83(a1: v9, a2: v10);`
  - `if ( *(this + 46) == 0 )`
  - `v12 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1548);`
  - `if ( v12 != 0 )`
  - `v10 = (*(*(this + 1) + 48))(a1: this + 4);`
  - `v4 = (*(*(this + 1) + 44))(a1: this + 4);`
  - `v5 = sub_7CF95E(a1: v4, a2: v10);`
  - `sub_7D0364(a1: v5);`
  - …（截断 16 行）

### CUIAdminShopWishListSearchResult（5 函数）


#### CUIAdminShopWishListSearchResult::GetIndexFromPoint  (832 B)

  - `int __thiscall CUIAdminShopWishListSearchResult::GetIndexFromPoint(`
  - `for ( i = 63; ; i += 20 )`
  - `v5 = *(v8 + 45);`
  - `if ( v5 >= 10 )`
  - `if ( v9 >= v5 )`
  - `if ( dword_BF0484(a1: v7, a2, a3) != 0 )`

#### CUIAdminShopWishListSearchResult::OnButtonClicked  (6432 B)

- 分支 case：`1000`, `1001`, `1002`
- SP_ 节点：`SP_3628__S__R_NWOULD_YOU_LIKE_TO_REGISTER`
  - `void __thiscall CUIAdminShopWishListSearchResult::OnButtonClicked(CUIAdminShopWishListSearchResult *this, int a2)`
  - `switch ( a2 )`
  - `CUIAdminShopWishList::ToggleAddOn(this: dword_BE791C, a2: 0);`
  - `ZList<long>::RemoveAll(a1: this + 192);`
  - `sub_5DC1C5(a1: this + 172);`
  - `ZList<long>::RemoveAll(a1: this + 172);`
  - `sub_5DC1C5(a1: this + 152);`
  - `*(this + 56) = *(this + 55);`
  - `*(this + 55) = *(this + 54);`
  - `*(this + 54) = *(this + 53);`
  - `ZList<long>::RemoveAll(a1: this + 152);`
  - `v3 = *(*(this + 54) != 0 ? *(this + 54) - 16 + 8 : 8);`
  - `while ( v4 != 0 )`
  - `if ( a2 >= 10 )`
  - `v5 = *(v4 != 0 ? v4 - 16 + 8 : 8);`
  - `sub_5DCF53();`
  - …（截断 16 行）

#### CUIAdminShopWishListSearchResult::OnMouseButton  (3195 B)

- SP_ 节点：`SP_3628__S__R_NWOULD_YOU_LIKE_TO_REGISTER`
  - `void __thiscall CUIAdminShopWishListSearchResult::OnMouseButton(`
  - `if ( a2 != 514 && a2 != 515 )`
  - `v21 = (this - 4);`
  - `IndexFromPoint = CUIAdminShopWishListSearchResult::GetIndexFromPoint(this: (this - 4), a2: a4, a3: a5);`
  - `if ( v5 == 514 && *(this + 35) != IndexFromPoint )`
  - `*(this + 35) = IndexFromPoint;`
  - `v13 = *(this + 33);`
  - `if ( IndexFromPoint > 9 )`
  - `v14 = *(v13 + 4);`
  - `(*(v14 + 28))(a1: v15, a2: v20);`
  - `CWnd::InvalidateRect(this: v21, a2: 0);`
  - `if ( v5 != 515 || IndexFromPoint < 0 )`
  - `v8 = *ZList<long>::FindIndex(a1: IndexFromPoint);`
  - `CItemInfo::GetItemName(a1: &a5, a2: v8);`
  - `LOBYTE(v23) = 2;`
  - `v11 = ZXString<char>::Format(a1: &a4, a2: v10, a3: a5);`
  - …（截断 16 行）

#### CUIAdminShopWishListSearchResult::OnMouseMove  (1659 B)

  - `int __thiscall CUIAdminShopWishListSearchResult::OnMouseMove(char **this, int a2, int a3)`
  - `v14 = (this - 1);`
  - `IndexFromPoint = CUIAdminShopWishListSearchResult::GetIndexFromPoint(this: (this - 1), a2, a3);`
  - `if ( IndexFromPoint >= 0 )`
  - `Index = ZList<long>::FindIndex(a1: IndexFromPoint);`
  - `CItemInfo::GetItemSlot(a1: v12, a2: *Index);`
  - `if ( v13 != 0 )`
  - `v10 = ((*(*this + 12))(a1: this) + a3 + 20);`
  - `v7 = (*(*this + 11))(a1: this);`
  - `CUIToolTip::ShowItemToolTip(`
  - `a2: (v7 + a2 + 20),`
  - `ZRef<GW_ItemSlotBase>::~ZRef<GW_ItemSlotBase>(a1: 0);`
  - `CUIToolTip::ClearToolTip(this: (this + 56));`
  - `if ( *(this + 36) != v5 )`
  - `*(this + 36) = v5;`
  - `CWnd::InvalidateRect(this: v8, a2: 0);`
  - …（截断 16 行）

#### CUIAdminShopWishListSearchResult::Reset  (2105 B)

  - `int __thiscall CUIAdminShopWishListSearchResult::Reset(CUIAdminShopWishListSearchResult *this)`
  - `(*(*(*(this + 30) + 4) + 28))(a1: *(this + 30) + 4, a2: 0);`
  - `*(this + 53) = 0;`
  - `*(this + 54) = *(this + 385);`
  - `ZList<long>::RemoveAll(a1: this + 172);`
  - `ZList<long>::RemoveAll(a1: this + 192);`
  - `v2 = *(this + 54);`
  - `while ( v2 != 0 && v15 < 20 )`
  - `v3 = *(v2 != 0 ? v2 - 16 + 4 : 4);`
  - `sub_5DCE88(a1: v10);`
  - `v4 = sub_7D03A9(a1: v11);`
  - `ZXString<char>::operator=(a1: v4);`
  - `LOBYTE(v16) = 0;`
  - `v9 = *(sub_7D03A9(a1: v11) + 4);`
  - `v5 = sub_7D0307();`
  - `if ( CUIAdminShopWishList::CanAddList(this: v5, a2: v9, a3: v10) != 0 )`
  - …（截断 16 行）

### CRegisterWishEntryDlg（1 函数）


#### CRegisterWishEntryDlg::Confirm  (1543 B)

  - `int __cdecl CRegisterWishEntryDlg::Confirm(int a1, int a2, int *a3, int *a4, int a5, _BYTE *a6)`
  - `v17 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 188);`
  - `if ( v17 != 0 )`
  - `v6 = sub_5AB7DB(a1, a2);`
  - `v16 = v7 != 0 ? (v7 - 8) : 0;`
  - `sub_5ADB54(a1: v15);`
  - `v8 = CDialog::DoModal(a1: v16);`
  - `if ( v8 == 1 )`
  - `Text = CCtrlEdit::GetText(this: v16[40], &a2);`
  - `v10 = atoi(a1: *Text);`
  - `v11 = CCtrlEdit::GetText(this: v16[42], &a2);`
  - `v12 = atoi(a1: *v11);`
  - `v13 = CCtrlEdit::GetText(this: v16[44], &a2);`
  - `LOBYTE(v18) = 2;`
  - `ZXString<char>::operator=(a1: v13);`
  - `LOBYTE(v18) = 1;`
  - …（截断 16 行）

### CRegisterWishEntrySearchDlg（2 函数）


#### CRegisterWishEntrySearchDlg::OnButtonClicked  (2783 B)

- SP_ 节点：`SP_4801_PLEASE_ENTER_MORE_THAN_1_CHARACTER`, `SP_4802_THE_SEARCH_YIELDED_0_RESULT`
  - `void __thiscall CRegisterWishEntrySearchDlg::OnButtonClicked(int this, int a2)`
  - `CUIToolTip::ClearToolTip(this: (this + 180));`
  - `if ( a2 == 1001 )`
  - `CCtrlEdit::GetText(this: *(this + 164), a2: &v15);`
  - `sub_4BCD89(a1: &v15);`
  - `if ( v15 != 0 )`
  - `v4 = *(v15 - 4);`
  - `if ( v4 < 1 )`
  - `CUtilDlg::Notice(sMsg: v9, sSndName: v10, ppDialog: v11, bAutoSeparated: v12, bIsTightLine: v13);`
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: *(this + 164) != 0 ? (*(this + 164) + 4) : 0);`
  - `if ( !CItemInfo::GetItemIDArrayByName(`
  - `nTI: *(*(this + 172) + 104),`
  - `anResult: (this + 176)) )`
  - `*(this + 128) = -1;`
  - `*(this + 132) = -1;`
  - `sub_5AA737(this);`
  - …（截断 16 行）

#### CRegisterWishEntrySearchDlg::OnMouseButton  (2006 B)

  - `void __thiscall CRegisterWishEntrySearchDlg::OnMouseButton(int *this, int a2, int a3, int a4, int a5)`
  - `v6 = (this + 44);`
  - `CUIToolTip::ClearToolTip(this: (this + 44));`
  - `if ( a2 == 514 )`
  - `v7 = *(this + 32);`
  - `*(this + 31) = v7;`
  - `CWnd::InvalidateRect(this: (this - 1), a2: v14[0]);`
  - `CUIToolTip::ClearToolTip(this: v6);`
  - `if ( a2 != 515 )`
  - `v8 = *(this + 32);`
  - `if ( v8 >= 0 )`
  - `if ( CRegisterWishEntryDlg::Confirm(`
  - `a1: *(*(this + 43) + 4 * v8),`
  - `a2: *(this + 30),`
  - `ZXString<char>::operator=(a1: &a2);`
  - `LOBYTE(v19) = 0;`
  - …（截断 16 行）

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

### CRegisterAuctionEntryDlg（2 函数）


#### CRegisterAuctionEntryDlg::Confirm  (3655 B)

- SP_ 节点：`SP_4765_THE_BUY_NOW_PRICE_IS_LOWER_R_NTHAN_THE_STARTING_BID`, `SP_5560_PLEASE_ENTER_THE_PRICE_R_NOF_OVER_110_NX`
  - `static int __cdecl CRegisterAuctionEntryDlg::Confirm(`
  - `v35 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 216);`
  - `if ( v35 != 0 )`
  - `v9 = sub_5AC65A(a1, a2, a3);`
  - `sub_5ADB87(a1: v33);`
  - `v37 = CDialog::DoModal(a1: v34);`
  - `if ( v37 == 1 )`
  - `Text = CCtrlEdit::GetText(this: *(v34 + 196), a2: &a3);`
  - `v12 = atoi(a1: *Text);`
  - `if ( a3 != 0 )`
  - `v13 = CCtrlEdit::GetText(this: *(v34 + 172), a2: &a3);`
  - `v14 = atoi(a1: *v13);`
  - `v16 = CCtrlEdit::GetText(this: *(v34 + 180), a2: &a3);`
  - `v17 = atoi(a1: *v16);`
  - `v19 = CCtrlEdit::GetText(this: *(v34 + 188), a2: &a3);`
  - `v20 = atoi(a1: *v19);`
  - …（截断 16 行）

#### CRegisterAuctionEntryDlg::OnKey  (1589 B)

  - `void __thiscall CRegisterAuctionEntryDlg::OnKey(CRegisterAuctionEntryDlg *this, unsigned int a2, unsigned int a3)`
  - `InterlockedIncrement(lpAddend: this + 41);`
  - `if ( CheckNumber(a1: v9, a2: v10, a3: v11, a4: v12) )`
  - `Text = CCtrlEdit::GetText(this: *(this + 42), a2: &v13);`
  - `v6 = atoi(a1: *Text);`
  - `*(this + 31) = v6 + *(this + 34) + v6 * *(this + 33) / 100;`
  - `InterlockedIncrement(lpAddend: this + 43);`
  - `v7 = CCtrlEdit::GetText(this: *(this + 44), a2: &v13);`
  - `v8 = atoi(a1: *v7);`
  - `*(this + 32) = v8 + *(this + 34) + v8 * *(this + 33) / 100;`
  - `CWnd::InvalidateRect(this: (this - 4), a2: 0);`

### CRegisterSaleEntryDlg（1 函数）


#### CRegisterSaleEntryDlg::Confirm  (1498 B)

  - `int __cdecl CRegisterSaleEntryDlg::Confirm(int a1, int a2, int a3, int *a4, int *a5, _BYTE *a6, _BYTE *a7)`
  - `v7 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 180);`
  - `if ( v7 != 0 )`
  - `v8 = CRegisterSaleEntryDlg::CRegisterSaleEntryDlg(this: v7, a2: a1, a3: a2, a4: a3);`
  - `v9 = v8 != 0 ? (v8 + 2) : 0;`
  - `sub_5ADB21(a1: v15);`
  - `v19 = CDialog::DoModal(a1: v16);`
  - `if ( v19 == 1 )`
  - `Text = CCtrlEdit::GetText(this: *(v16 + 168), a2: &a3);`
  - `v11 = atoi(a1: *Text);`
  - `v12 = CCtrlEdit::GetText(this: *(v16 + 160), a2: &a3);`
  - `v13 = atoi(a1: *v12);`

### CUICharacterSaleDlg（4 函数）


#### CUICharacterSaleDlg::ClearCurrentStep  (685 B)

- 分支 case：`1`, `2`, `3`, `4`, `5`, `6`
  - `int __thiscall CUICharacterSaleDlg::ClearCurrentStep(CUICharacterSaleDlg *this, int a2)`
  - `sub_7D4F97(a1: this);`
  - `switch ( a2 )`
  - `sub_7D501F(a1: 0);`
  - `sub_7D5111(a1: 0);`
  - `sub_7D5242(a1: 0);`
  - `sub_7D532D(a1: 0);`
  - `sub_7D544D(a1: 0);`
  - `sub_7D5547(a1: 0);`

#### CUICharacterSaleDlg::OnButtonClicked  (3597 B)

  - `void __thiscall CUICharacterSaleDlg::OnButtonClicked(CUICharacterSaleDlg *this, unsigned int a2)`
  - `CUICharacterSaleDlg::ClearCurrentStep(this, a2: *(this + 111));`
  - `if ( a2 >= 0x3F0 && a2 < 0x402 )`
  - `if ( a2 >= 0x3F9 )`
  - `if ( v5 == 4 )`
  - `if ( *(this + 504) == 0 )`
  - `if ( v5 > 4 )`
  - `CUICharacterSaleDlg::ShowWindow(this);`
  - `sub_7D59F9(a1: v5, a2: v4 != 0 ? 1 : -1);`
  - `CWnd::InvalidateRect(this, a2: 0);`
  - `if ( a2 != 1000 )`
  - `switch ( a2 )`
  - `if ( *(this + 111) == 6 && *(this + 110) >= 2u )`
  - `*(this + 111) = 5;`
  - `if ( --*(this + 111) == 3 )`
  - `*(this + 111) = 2;`
  - …（截断 16 行）

#### CUICharacterSaleDlg::SendCheckDuplicateIDPacket  (1789 B)

- SP_ 节点：`SP_5046_YOU_CAN_NOT_USE_THIS_NAME_R_NPLEASE_CHECK_AGAIN`
  - `void *__thiscall CUICharacterSaleDlg::SendCheckDuplicateIDPacket(_DWORD *this, int *a2)`
  - `if ( *(this + 180) == 0 )`
  - `v4 = sub_4F38D3(this: *dword_BE7918);`
  - `valid = is_valid_character_name(sCharName: a2, bWordCheck: v4 == 0);`
  - `if ( valid )`
  - `COutPacket::COutPacket(a1: 256);`
  - `ZXString<char>::operator=(a1: a2);`
  - `COutPacket::EncodeStr(this: &v13, a2: v12);`
  - `sub_7D766A(a1: &v13);`
  - `return ZArray<unsigned char>::RemoveAll(a1: v14);`
  - `CUtilDlg::Notice(sMsg: v8, sSndName: v9, ppDialog: v10, bAutoSeparated: v11, bIsTightLine: v12);`
  - `return (*(*this + 32))(a1: this, a2: 1001);`

#### CUICharacterSaleDlg::ShowWindow  (645 B)

- 分支 case：`1`, `2`, `3`, `4`, `5`, `6`
  - `int __thiscall CUICharacterSaleDlg::ShowWindow(CUICharacterSaleDlg *this)`
  - `switch ( *(this + 111) )`
  - `sub_7D501F(a1: 1);`
  - `sub_7D5111(a1: 1);`
  - `sub_7D5242(a1: 1);`
  - `sub_7D532D(a1: 1);`
  - `sub_7D544D(a1: 1);`
  - `sub_7D5547(a1: 1);`

### CConfirmPurchaseDlg（4 函数）


#### CConfirmPurchaseDlg::Draw  (8868 B)

- SP_ 节点：`SP_2695_D_NX`, `SP_588_D_MESOS`
  - `_DWORD *__thiscall CConfirmPurchaseDlg::Draw(_DWORD *this, int a2)`
  - `CWnd::Draw(this, a2);`
  - `CConfirmPurchaseDlg::DrawBackgrnd(this);`
  - `CWnd::GetCanvas(a1: &v57);`
  - `v3 = *(this + 32);`
  - `if ( v3 < 0 )`
  - `if ( v3 <= 2 )`
  - `v6 = *(this + 34) <= 0;`
  - `if ( !v6 )`
  - `pvarg.cyVal.Hi = *(*(this + 36) + 8 * v59 + 4);`
  - `sub_473CB6(a1: &pvarg.decVal.Lo32);`
  - `v7 = *(pvarg.cyVal.Hi + 12);`
  - `LOBYTE(v61) = 4;`
  - `if ( v7 < 80000000 || v7 >= 90000000 )`
  - `LOBYTE(v61) = 5;`
  - `ZXString<char>::Format(a1: v54, a2: *String, a3: *(pvarg.cyVal.Hi + 32));`
  - …（截断 16 行）

#### CConfirmPurchaseDlg::DrawBackgrnd  (20428 B)

- SP_ 节点：`SP_2697_UI_CASHSHOPIMG_CSNOTICE_0`
  - `VARIANTARG **__thiscall CConfirmPurchaseDlg::DrawBackgrnd(_DWORD *this)`
  - `CWnd::GetCanvas(a1: &v91);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v93) = 1;`
  - `sub_402F85(pvargDest: &v68, pvargSrc: &pvargSrc);`
  - `LOBYTE(v93) = 2;`
  - `v3 = sub_406472();`
  - `v4 = sub_402680(a1: v3);`
  - `IWzResMan::GetObjectA(this: v4, pvargDest: &v62, sUOL: v60, vParam: *&v61.vt, vAux: v61.decVal.Hi32);`
  - `LOBYTE(v93) = 4;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v61.cVal, a2: SBYTE4(v61.decVal.Lo64));`
  - `if ( Unknown != 0 )`
  - `v5 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v5 < 0 && v5 != -2147467262 )`
  - `LOBYTE(v93) = 8;`
  - `if ( v6 < 0 )`
  - …（截断 16 行）

#### CConfirmPurchaseDlg::OnCreate  (4840 B)

- SP_ 节点：`SP_1257_UI_BASICIMG_BTOK2`, `SP_1258_UI_BASICIMG_BTCANCEL2`
  - `void __thiscall CConfirmPurchaseDlg::OnCreate(_DWORD *this, int a2)`
  - `CConfirmPurchaseDlg::SetLayer(this);`
  - `CCtrlCheckBox::CREATEPARAM::CREATEPARAM(a1: v19);`
  - `v3 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 108);`
  - `LOBYTE(v26) = 1;`
  - `if ( v3 != 0 )`
  - `v4 = CCtrlCheckBox::CCtrlCheckBox(this: v3);`
  - `LOBYTE(v26) = 0;`
  - `sub_4739BA(a1: v4);`
  - `(*(**(this + 42) + 32))(a1: *(this + 42), a2: this, a3: 1000, a4: 25, a5: *(this + 10) - 95, a6: v19);`
  - `CCtrlCheckBox::SetChecked(this: *(this + 42), a2: *(this + 33) == 2);`
  - `if ( (*(this + 33) & 2) == 0 )`
  - `(*(*(*(this + 42) + 4) + 28))(a1: *(this + 42) + 4, a2: 0);`
  - `v5 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 108);`
  - `LOBYTE(v26) = 2;`
  - `if ( v5 != 0 )`
  - …（截断 16 行）

#### CConfirmPurchaseDlg::SetLayer  (6782 B)

- SP_ 节点：`SP_956_CANVAS`
  - `int __thiscall CConfirmPurchaseDlg::SetLayer(_DWORD *this)`
  - `LOBYTE(v38) = 1;`
  - `PcCreateObject::IWzCanvas(a1: v3, a2: &v37, a3: 0);`
  - `LOBYTE(v38) = 0;`
  - `if ( v36 != 0 )`
  - `v4 = *(this + 32);`
  - `*(this + 9) = 286;`
  - `if ( v4 == 0 )`
  - `v7 = 40 * *(this + 34) + 156;`
  - `if ( v5 == 0 )`
  - `v7 = 40 * *(this + 34) + 175;`
  - `if ( v6 == 0 )`
  - `v7 = 40 * *(this + 34) + 194;`
  - `*(this + 10) = v7;`
  - `if ( v6 == 1 )`
  - `*(this + 10) = 170;`
  - …（截断 16 行）

### CCouponUseSelectDlg（2 函数）


#### CCouponUseSelectDlg::Confirm  (1850 B)

- SP_ 节点：`SP_1232_DLGNOTICE`, `SP_1283_UI_UIWINDOWIMG_COUPON_BACKGRND`
  - `int CCouponUseSelectDlg::Confirm()`
  - `v0 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 180);`
  - `if ( v0 != 0 )`
  - `v1 = sub_471374(this: v0);`
  - `v2 = v1 != 0 ? (v1 + 2) : 0;`
  - `sub_473ED6(a1: v10);`
  - `LOBYTE(v13) = 2;`
  - `play_ui_sound(a1: v9);`
  - `LOBYTE(v13) = 1;`
  - `LOBYTE(v13) = 3;`
  - `CDialog::CreateDlg(a1: v8, a2: 1, a3: 0);`
  - `v5 = CDialog::DoModal(a1: v11);`
  - `if ( v5 == 1 )`
  - `Text = CCtrlEdit::GetText(this: *(v11 + 128), a2: &v12);`
  - `LOBYTE(v13) = 4;`
  - `ZXString<char>::operator=(a1: Text);`
  - …（截断 16 行）

#### CCouponUseSelectDlg::Update  (2354 B)

  - `void __thiscall CCouponUseSelectDlg::Update(char **this)`
  - `if ( CCtrlWnd::IsFocused(this: *(this + 32)) != 0 )`
  - `v2 = *CCtrlEdit::GetText(this: *(this + 32), a2: &v18);`
  - `if ( v4 )`
  - `CCtrlEdit::GetText(this: *(this + 32), a2: &v17);`
  - `v5 = sub_4737BB(a1: &v18, a2: 1);`
  - `if ( asc_AF21E0 == 0 )`
  - `if ( v7 == 0 )`
  - `v8 = strcmp(a1: v7, a2: v6) == 0;`
  - `if ( v8 )`
  - `v9 = *CCtrlEdit::GetText(this: *(this + 32), a2: &v14);`
  - `if ( v9 != 0 )`
  - `v10 = *(v9 - 4);`
  - `CCtrlEdit::GetText(this: *(this + 32), a2: &v15);`
  - `LOBYTE(v19) = 2;`
  - `v11 = sub_47379F(a1: &v16, a2: v10 - 1);`
  - …（截断 16 行）

### CUIShopScanResult（2 函数）


#### CUIShopScanResult::OnMouseMove  (13275 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_3640_THIS_STORE_IS_OPEN_AT_ROOM_D_ON_CHANNEL_S`, `SP_3650__ITEM_PER_BUNDLE`, `SP_3651_PRICE_PER_BUNDLE`, `SP_3652_TOTAL__SOLD`, `SP_5517_S`
  - `signed int __thiscall CUIShopScanResult::OnMouseMove(char **this, signed int a2, signed int a3)`
  - `if ( a3 >= 176 )`
  - `v16 = (a3 - 174) % 22;`
  - `a3 = (a3 - 174) / 22;`
  - `if ( a3 < 0 )`
  - `v17 = *(this + 58);`
  - `if ( v17 == 0 || a3 >= *(v17 - 4) )`
  - `if ( a2 >= 304 && a2 <= 361 )`
  - `v18 = *(this + 386) % 1000000;`
  - `if ( *(this + 386) / 1000000 == 1 )`
  - `v19 = *(*&(*(this + 58))[8 * a3 + 4] + 40);`
  - `v54 = ((*(*this + 12))(a1: this, a2: v18) + v3 + 20);`
  - `v20 = (*(*this + 11))(a1: this);`
  - `CUIToolTip::ShowItemToolTip(`
  - `a2: (v20 + a2 + 20),`
  - `CItemInfo::GetItemSlot(a1: &pvarg.decVal.Lo32, a2: *(this + 386));`
  - …（截断 16 行）

#### CUIShopScanResult::OnShopLinkResult  (6520 B)

- SP_ 节点：`SP_3468_THE_OWNER_OF_THE_STORE_IS_CURRENTLY_R_NUNDERGOING_STORE_MAINTENANCE_R_NPLEASE_TR`, `SP_3641_THIS_CAN_ONLY_BE_USED_INSIDE_THE_FREE_MARKET`, `SP_394_THE_ROOM_IS_ALREADY_CLOSED`, `SP_395_YOU_CANT_ENTER_THE_ROOM_DUE_TO_FULL_CAPACITY`, `SP_396_OTHER_REQUESTS_ARE_BEING_FULFILLED_THIS_MINUTE`, `SP_397_YOU_CANT_DO_IT_WHILE_YOURE_DEAD`, `SP_399_THIS_CHARACTER_IS_UNABLE_TO_DO_IT`, `SP_400_YOU_ARE_NOT_ALLOWED_TO_TRADE_OTHER_ITEMS_AT_THIS_POINT`, `SP_428_YOU_MAY_NOT_ENTER_THIS_STORE`
  - `int __thiscall CUIShopScanResult::OnShopLinkResult(CUIShopScanResult *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `if ( v3 > 4u )`
  - `if ( v17 != 0 )`
  - `if ( v18 != 0 )`
  - `if ( v19 != 0 )`
  - `if ( v19 != 5 )`
  - `LOBYTE(v46) = 9;`
  - `ZXString<char>::operator=(a1: String);`
  - `LOBYTE(v46) = 0;`
  - `LOBYTE(v46) = 8;`
  - `ZXString<char>::operator=(a1: v23);`
  - `LOBYTE(v46) = 7;`
  - `ZXString<char>::operator=(a1: v25);`
  - `LOBYTE(v46) = 6;`
  - `ZXString<char>::operator=(a1: v27);`
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

### CParcelDlg（1 函数）


#### CParcelDlg::OnPacket  (7990 B)

- 分支 case：`8`, `23`, `24`, `25`, `26`, `27`
- SP_ 节点：`SP_3899_SUCCESSFULLY_DELETED`, `SP_3900_SUCCESSFULLY_CLAIMED`, `SP_3902_A_NEW_PACKAGE_HAS_BEEN_SENT`
  - `void __cdecl CParcelDlg::OnPacket(int a1)`
  - `a1 = CInPacket::Decode1(this: a1);`
  - `switch ( a1 )`
  - `if ( dword_BE7910 != 0 )`
  - `_CxxThrowException(a1: &v29, a2: &_TI3_AVCDisconnectException__);`
  - `v21 = CInPacket::Decode1(this: v1) != 0;`
  - `a1 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 284);`
  - `if ( a1 != 0 )`
  - `sub_6F3E94(a1: v21 ? 2 : 0);`
  - `sub_6F51EA(a1: v1);`
  - `v15 = CInPacket::Decode4(this: v1);`
  - `v16 = CInPacket::Decode1(this: v1);`
  - `if ( dword_BE7910 != 0 && (*(*(dword_BE7910 + 4) + 72))(a1: dword_BE7910 + 4, a2: &dword_BEDC94) != 0 )`
  - `if ( a1 == 0 )`
  - `_CxxThrowException(a1: &v32, a2: &_TI3_AVCDisconnectException__);`
  - `sub_6F51FC(a1: v15);`
  - …（截断 16 行）

### CUIQuestDelivery（1 函数）


#### CUIQuestDelivery::OnChildNotify  (375 B)

  - `void __thiscall CUIQuestDelivery::OnChildNotify(`
  - `if ( a2 == 2000 && a3 >= 0x12C && a3 <= 0x130 )`
  - `CWnd::InvalidateRect(this, a2: 0);`
  - `else if ( a3 == 100 )`
  - `(*(*this + 32))(a1: this, a2);`

### CUIRecommendWorld（7 函数）


#### CUIRecommendWorld::IsRequestValid  (190 B)

  - `BOOL __thiscall CUIRecommendWorld::IsRequestValid(CUIRecommendWorld *this)`
  - `v1 = *(this + 32);`
  - `return *(v1 + 368) == 0 && *(v1 + 360) == 1;`

#### CUIRecommendWorld::MoveWorld  (403 B)

  - `void __thiscall CUIRecommendWorld::MoveWorld(CUIRecommendWorld *this, int nMovement)`
  - `v2 = *(*(this + 32) + 528);`
  - `if ( v2 != 0 )`
  - `v3 = *(v2 - 4);`
  - `*(this + 33) = (nMovement + v3 + *(this + 33)) % v3;`
  - `CUIRecommendWorld::ResetWorldAndMessage(this, bRedraw: 1);`

#### CUIRecommendWorld::OnButtonClicked  (591 B)

- 分支 case：`1000`, `1001`, `1002`
  - `void __thiscall CUIRecommendWorld::OnButtonClicked(CUIRecommendWorld *this, int nId)`
  - `if ( CUIRecommendWorld::IsRequestValid(a1: this) != 0 )`
  - `switch ( nId )`
  - `CUIRecommendWorld::MoveWorld(this, nMovement: -1);`
  - `CUIRecommendWorld::MoveWorld(this, nMovement: 1);`
  - `CUIRecommendWorld::SelectWorld(this);`

#### CUIRecommendWorld::OnCreate  (5274 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_116_MAPLESTORY_ERROR__MOB_ERROR`, `SP_5362_UI_LOGINIMG_WORLDSELECT_ALERT_BACKGRD`, `SP_5363_UI_LOGINIMG_WORLDSELECT_ALERT_BTARROWL`, `SP_5364_UI_LOGINIMG_WORLDSELECT_ALERT_BTARROWR`, `SP_5365_UI_LOGINIMG_WORLDSELECT_ALERT_BTCHOICE`
  - `void __thiscall CUIRecommendWorld::OnCreate(CUIRecommendWorld *this, CUIRecommendWorld **pData)`
  - `*(this + 32) = pData;`
  - `*(this + 33) = 0;`
  - `if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 )`
  - `(*(*(TSingleton<CUIWorldSelect>::ms_pInstance + 4) + 28))(a1: TSingleton<CUIWorldSelect>::ms_pInstance + 4, a2: 0);`
  - `CWnd::SetBackgrnd(a1: v20, a2: p_pData, a3: v22);`
  - `basic_font = get_basic_font(a1: &pData, fType: 1);`
  - `v5 = (this + 160);`
  - `sub_4288D7(a1: *basic_font);`
  - `if ( pData != 0 )`
  - `(*(*pData + 2))(a1: pData);`
  - `if ( *v5 == 0 )`
  - `(*(**v5 + 20))(a1: *v5, a2: &v26);`
  - `*(this + 45) = 85 / v26;`
  - `v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: v22);`
  - `if ( v6 != 0 )`
  - …（截断 16 行）

#### CUIRecommendWorld::OnMouseButton  (741 B)

  - `int __thiscall CUIRecommendWorld::OnMouseButton(char *this, int msg, int wParam, int rx, int ry)`
  - `result = CUIRecommendWorld::IsRequestValid(this: (this - 4));`
  - `if ( result != 0 )`
  - `if ( msg == 513 )`
  - `result = dword_BF0484(a1: v7, a2: rx, a3: ry);`
  - `return CUIRecommendWorld::SelectWorld(this: v5);`

#### CUIRecommendWorld::ResetWorldAndMessage  (7929 B)

- SP_ 节点：`SP_1366_UI_LOGINIMG_WORLDSELECT_WORLD_D`
  - `void __thiscall CUIRecommendWorld::ResetWorldAndMessage(CUIRecommendWorld *this, int bRedraw)`
  - `v3 = *(this + 32);`
  - `v4 = *(v3 + 528);`
  - `v5 = (v3 + 528);`
  - `if ( v4 != 0 && *(v4 - 4) > 1u )`
  - `(*(*(*(this + 35) + 4) + 28))(a1: *(this + 35) + 4, a2: 1, a3: v43);`
  - `v6 = *(this + 37);`
  - `v7 = *(v6 + 4);`
  - `(*(*(*(this + 35) + 4) + 28))(a1: *(this + 35) + 4, a2: 0, a3: v43);`
  - `v9 = *(this + 37);`
  - `v7 = *(v9 + 4);`
  - `(*(v7 + 28))(a1: v8, a2: v43);`
  - `v42 = *(*v5 + 8 * *(this + 33));`
  - `LOBYTE(v54) = 1;`
  - `sub_416821(a1: &v51, a2: v41, a3: v42);`
  - `LOBYTE(v54) = 0;`
  - …（截断 16 行）

#### CUIRecommendWorld::SelectWorld  (351 B)

  - `int __thiscall CUIRecommendWorld::SelectWorld(_DWORD *this)`
  - `if ( TSingleton<CUIWorldSelect>::ms_pInstance != 0 )`
  - `CUIWorldSelect::SetFocusWorld(`
  - `nWorldID: *(*(*(this + 32) + 528) + 8 * *(this + 33)));`
  - `return (*(*this + 52))(a1: this, a2: 1);`

### UCS_COMMODITY（2 函数）


#### UCS_COMMODITY::1?$ZRef  (250 B)

  - `int __thiscall ZRef<CS_COMMODITY>::~ZRef<CS_COMMODITY>(_DWORD *this)`
  - `if ( *(this + 1) != 0 )`
  - `result = sub_473F9E(a1: 0);`
  - `*(this + 1) = 0;`

#### UCS_COMMODITY::4?$ZRef  (397 B)

  - `_DWORD *__thiscall ZRef<CS_COMMODITY>::operator=(_DWORD *this, int a2)`
  - `v2 = *(a2 + 4);`
  - `if ( v2 != 0 )`
  - `InterlockedIncrement(lpAddend: (v2 + 4));`
  - `if ( *(this + 1) != 0 )`
  - `sub_473F9E(a1: 0);`
  - `*(this + 1) = 0;`
  - `*(this + 1) = *(a2 + 4);`

### CWvsCommodityEx（1 函数）


#### CWvsCommodityEx::GetCommodityExByIndex  (303 B)

  - `int __thiscall CWvsCommodityEx::GetCommodityExByIndex(_DWORD *this, int a2, int a3)`
  - `v3 = *(*(this + 3512) + 8 * a3 + 4);`
  - `*(a2 + 4) = v3;`
  - `if ( v3 != 0 )`
  - `InterlockedIncrement(lpAddend: (v3 + 4));`

### CCashTradingRoomDlg（5 函数）


#### CCashTradingRoomDlg::OnButtonClicked  (1732 B)

  - `void __thiscall CCashTradingRoomDlg::OnButtonClicked(char **this, unsigned int a2)`
  - `switch ( a2 )`
  - `CCashTradingRoomDlg::Trade(this);`
  - `CCashTradingRoomDlg::PutMoney(this);`
  - `v4 = *(this + 107);`
  - `if ( v4 != 0 )`
  - `v5 = *CCtrlEdit::GetText(this: v4, a2: &v9);`
  - `if ( !v6 )`
  - `CCtrlEdit::GetText(this: *(this + 107), &a2);`
  - `v7 = *(this + 107);`
  - `CCtrlEdit::SetText(this: v7, a2: 0);`
  - `ZXString<char>::operator=(a1: &a2);`
  - `CMiniRoomBaseDlg::CheckAndSendChat(this, a2: v8[0]);`
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: *(this + 107) != 0 ? (*(this + 107) + 4) : 0);`
  - `(*(v3 + 13))(a1: this, a2: v8[0]);`

#### CCashTradingRoomDlg::OnKey  (1163 B)

  - `void __thiscall CCashTradingRoomDlg::OnKey(CCashTradingRoomDlg *this, unsigned int a2, signed int a3)`
  - `if ( a3 >= 0 && a2 == 13 )`
  - `v4 = *(this + 106);`
  - `if ( v4 != 0 )`
  - `v5 = *CCtrlEdit::GetText(this: v4, &a2);`
  - `if ( !v6 )`
  - `CCtrlEdit::GetText(this: *(this + 106), a2: &a3);`
  - `v7 = *(this + 106);`
  - `CCtrlEdit::SetText(this: v7, a2: 0);`
  - `ZXString<char>::operator=(a1: &a3);`
  - `CMiniRoomBaseDlg::CheckAndSendChat(this: this - 1, a2: v8[0]);`
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: *(this + 106) != 0 ? (*(this + 106) + 4) : 0);`

#### CCashTradingRoomDlg::OnLeave  (5347 B)

- SP_ 节点：`SP_406_TRADE_CANCELLED_R_NBY_THE_OTHER_CHARACTER`, `SP_407_TRADE_SUCCESSFUL_R_NPLEASE_CHECK_THE_RESULTS`, `SP_408_TRADE_SUCCESSFUL_R_NRECEIVED_D_MESOS_AFTER_FEES_R_NPLEASE_CHECK_THE_RESULTS`, `SP_409_TRADE_UNSUCCESSFUL`, `SP_410_YOU_CANNOT_MAKE_THE_TRADE_BECAUSE_R_NTHERE_ARE_SOME_ITEMS_WHICH_R_NYOU_CANNOT_CA`, `SP_411_YOU_CANNOT_MAKE_THE_TRADE_BECAUSE_R_NTHE_OTHER_PERSONS_ON_A_DIFFERENT_MAP`, `SP_5278_A_CASH_ITEM_TRADE_WAS_CANCELED_R_NDUE_TO_A_SERVER_CHECK`, `SP_5279_TRADE_WAS_CANCELED_BECAUSE_THERE_WAS_AN_ITEM_R_NTHAT_DISAPPEARED_BECAUSE_IT_EXPI`, `SP_5566_THE_GAME_FILE_HAS_BEEN_DAMAGED_R_NSO_YOU_CANNOT_PARTICIPATE_IN_AN_ITEM_TRADE_PLE`
  - `void __thiscall CCashTradingRoomDlg::OnLeave(CCashTradingRoomDlg *this, int a2, struct CInPacket *a3)`
  - `v4 = CInPacket::Decode1(this: a3);`
  - `v5 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v24) + 4);`
  - `v23 = *(v5 + 173);`
  - `v6 = _ZtlSecureFuse<long>(a1: v5 + 165, a2: v23);`
  - `if ( v24[1] != 0 )`
  - `sub_428C44(this: v24, a2: 0);`
  - `if ( v4 == 7 )`
  - `v7 = *(this + 122);`
  - `if ( v26 > v7 && v26 - v7 < *(this + 120) - *(this + 119) )`
  - `(*(*this + 52))(a1: this, a2: 8);`
  - `if ( v4 > 10 )`
  - `if ( v15 != 0 )`
  - `if ( v16 != 0 )`
  - `if ( v16 != 1 )`
  - `else if ( v4 == 10 )`
  - …（截断 16 行）

#### CCashTradingRoomDlg::PutMoney  (3136 B)

- SP_ 节点：`SP_412_HOW_MUCH_MONEY_WILL_YOU_PUT_UP`
  - `void *__thiscall CCashTradingRoomDlg::PutMoney(CCashTradingRoomDlg *this)`
  - `if ( *(this + 115) == 0 && *(this + 52) > 1 )`
  - `result = CWvsContext::CanSendExclRequest(a1: 500, a2: 0);`
  - `if ( result != 0 )`
  - `v3 = *(CWvsContext::GetCharacterData(this: v2, a2: &v17) + 4);`
  - `if ( v18 != 0 )`
  - `sub_428C44(this: &v17, a2: 0);`
  - `v4 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v4 != 0 )`
  - `v5 = CUtilDlgEx::CUtilDlgEx(this: v4);`
  - `v20 = v6 != 0 ? (v6 - 8) : 0;`
  - `sub_4289CD(a1: v19);`
  - `LOBYTE(v23) = 1;`
  - `v9 = sub_428688(a1: v19);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v9, a2: 2, a3: 0, a4: v12, a5: v13, a6: v14);`
  - `v10 = _ZtlSecureFuse<long>(a1: v3 + 165, a2: *(v3 + 173));`
  - …（截断 16 行）

#### CCashTradingRoomDlg::Trade  (8783 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_100_MAPLESTORY_ERROR__LOADING_REACTOR_FAILED`, `SP_413_ARE_YOU_SURE_YOU_WANT_TO_TRADE`, `SP_5214_TIME_LIMIT_EXPIRED`, `SP_5215_HAVE_YOU_CHECKED_THE_R_NTIME_LIMIT_ON_THE_ITEM`, `SP_5216_DDAYS_DHOURS_AND_DMINUTES_LEFT`, `SP_5277_CASH_ITEMS_BECOME_UNTRADABLE_R_NONCE_THEY_HAVE_BEEN_TRADED_AND__R_NMAPLE_POINTS_`
  - `void __thiscall CCashTradingRoomDlg::Trade(CCashTradingRoomDlg *this)`
  - `v38 = sub_7B8F35();`
  - `ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF);`
  - `for ( i = 0; ; i += 16 )`
  - `v4 = *(this + 118);`
  - `if ( v4 == 0 )`
  - `if ( v48 >= *(v4 - 4) )`
  - `v40 = *(*(this + 118) + i + 4);`
  - `sub_4288AF(a1: v39);`
  - `LOBYTE(v54) = 1;`
  - `if ( v40 == 0 || (*(*v40 + 20))(a1: v40) == 0 )`
  - `if ( (*(v5 + 24))(a1: v40) == 3 )`
  - `if ( CompareFileTime(lpFileTime1: v40 + 4, lpFileTime2: &stru_AF2380) < 0 || v6 != 0 )`
  - `LOBYTE(v54) = 2;`
  - `if ( dword_BF03B4(a1: v40 + 8, a2: &v38) <= 0 )`
  - `if ( v6 != 0 )`
  - …（截断 16 行）

### VCSoldItemDlg（1 函数）


#### VCSoldItemDlg::4?$ZRef  (439 B)

  - `int *__thiscall ZRef<CPersonalShopDlg::CSoldItemDlg>::operator=(int *this, int a2)`
  - `sub_6FFEBC(a1: v6);`
  - `v4 = *(this + 1);`
  - `*(this + 1) = v7;`
  - `sub_6FFE90(a1: v6);`

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

## 3. 坐标 / 尺寸 / 按钮 id / 封包字段候选行（商城现金交易全函数）


### AvatarLook::Decode
  - `memset(a1: this + 25, a2: 0, a3: 0xD0u);`

### CAvatarMegaphone::Draw
  - `IWzCanvas::DrawTextA(this: v5, nLeft: 124, nTop: 14, sText: v43, pFont: v44, vAlpha: p_pvargDest, vTabOrg: p_pvarg);`

### CAvatarMegaphone::HelloAvatarMegaphone
  - `Buffer = ZXString<char>::GetBuffer(a1: 1024, a2: 1);`

### CCSWnd_Best::ChangeCategory
  - `*v4 = CCashShop::GetUsableCommodityIndexBySN(this: *(this + 32), a2: (v3 + *(this + 32) + 88));`

### CCSWnd_Best::Draw
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v56[0], a2: SBYTE4(v56[0]));`
  - `Data = TSecType<long>::GetData(a1: (*(8 * v15 + *v72 + 4) + 16), a2: 0);`
  - `String = StringPool::GetString(this: Instance, result: v56[0], nIdx: SHIDWORD(v56[0]));`
  - `String = StringPool::GetString(this: v35, result: v56[0], nIdx: SHIDWORD(v56[0]));`

### CCSWnd_Best::OnMouseButton
  - `if ( TSecType<long>::GetData(a1: (v39 + 16), a2: v17) / 10000 == 507`
  - `|| (v18 = TSecType<long>::GetData(a1: (v39 + 16), a2: 514) / 10000) == 514`
  - `|| TSecType<long>::GetData(a1: (v39 + 16), a2: 514) / 10000 == 504 )`
  - `v45 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v36) + 4) == 0;`
  - `if ( TSecType<long>::GetData(a1: (v39 + 16), a2: 514) / 10000 == 507 && v46 <= 10 )`
  - `v20 = TSecType<long>::GetData(a1: (v39 + 16), a2: 514) / 10000;`
  - `Data = TSecType<long>::GetData(a1: (v39 + 16), a2: 514);`
  - `if ( TSecType<long>::GetData(a1: (v39 + 16), a2: 514) / 10000 == 504 && v46 < 7 )`
  - `v29 = TSecType<long>::GetData(a1: (v39 + 16), a2: 514);`

### CCSWnd_Char::CmpPet
  - `CashItemSlotPosition = CharacterData::FindCashItemSlotPosition(this: v2, a2: 5, a3: *(v2 + 27));`

### CCSWnd_Char::EndChat
  - `CCtrlEdit::SetText(this: *(this + 34), a2: 0);`

### CCSWnd_Char::OnCreate
  - `CCtrlTab::SetTab(this: *(this + 36), a2: 0);`

### CCSWnd_Char::OnKeyRet
  - `if ( *(TSingleton<CWndMan>::ms_pInstance + 136) == (*(this + 34) != 0 ? (v6 + 4) : 0) )`
  - `if ( get_weapon_type(nItemID: WeaponItemID) == 45 || get_weapon_type(nItemID: v16) == 46 )`
  - `*(*VecCtrl != 0 ? *VecCtrl - 12 + 0x168 : 360) = 1;`

### CCSWnd_Char::OnMouseButton
  - `CCashShop::SetUserPreviewControl(this: *(this + 274), a2: (IsControlingUserPreview == 0));`

### CCSWnd_Char::OnWear
  - `result = get_bodypart_from_item(a1: a2, a2: *(this + 160), a3: v23, a4: 0);`
  - `if ( v23[0] == 10 && CCSWnd_Char::IsTwoHandWeapon(a1: *(this + 173)) )`
  - `if ( CCSWnd_Char::IsTwoHandWeapon(a1: a2) && *(this + 171) != 0 )`
  - `return CAvatar::SetAvatarLook(this: (*(this + 260) + 136), a2: v19);`

### CCSWnd_Char::SetEmotion
  - `CAvatar::SetEmotion(this: (*(this + 260) + 136), nEmotion: a2 % 100 + 8, tDuration: -1);`

### CCSWnd_Char::ShowBullet
  - `CAvatar::SetAvatarLook(this: (*(this + 260) + 136), a2: v4);`

### CCSWnd_Char::TransientLayer_Weather
  - `Unknown = Ztl_variant_t::GetUnknown(a1: SLOBYTE(v174[0]), a2: SBYTE4(v174[0]));`
  - `v15 = get_int32(pvarSrc: Item, a2: SHIDWORD(v174[0])) != 0;`

### CCSWnd_Inventory::Draw
  - `v12 = *(CharacterData::GetItem(this: v47, a2: &v43, a3: *(v2 + 368), a4: v50) + 4);`

### CCSWnd_Inventory::GetItemSlotRect
  - `a2: 35 * ((nSlotPosition - v6) % 4) + 22,`
  - `a3: 35 * ((nSlotPosition - v6) / 4) + 55,`
  - `a5: 35 * ((nSlotPosition - v6) / 4) + 87);`

### CCSWnd_Inventory::OnCreate
  - `CCtrlTab::SetTab(this: *(this + 28), a2: 0);`

### CCSWnd_Inventory::OnMouseButton
  - `CCSWnd_Char::EndChat(this: (*(this + 369) + 1364));`
  - `v8 = *(CharacterData::GetItem(this: v6, a2: &v11, a3: *(this + 367), a4: SlotPositionFromPoint) + 4);`
  - `v10 = *(CharacterData::GetItem(this: v6, a2: &v11, a3: *(this + 367), a4: v9) + 4);`

### CCSWnd_Inventory::OnMouseMove
  - `v5 = *(CharacterData::GetItem(this: v4, a2: &v10, a3: *(this + 367), a4: SlotPositionFromPoint) + 4);`

### CCSWnd_Inventory::OnTabChanged
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 30), a2: (v4 - 10) / 4 + 1);`
  - `CCtrlScrollBar::SetCurPos(this: *(this + 30), a2: dword_BED588[*(this + 368)]);`

### CCSWnd_List::ChangeCategorySub
  - `CCtrlSelector::SetSelectorStart(this: *(this + 28), a2: v4, a3: 10, a4: 1, a5: 1);`
  - `CCtrlSelector::SetSelectorStart(this: v10, a2: v9, a3: 10, a4: 1, a5: 1);`
  - `CCtrlSelector::SetSelectorStart(this: *(this + 28), a2: 0, a3: 10, a4: 1, a5: 1);`

### CCSWnd_List::ChangePage
  - `UsableCommodityIndexBySN = CCashShop::GetUsableCommodityIndexBySN(this: *(this + 144), a2: v9 + 3);`

### CCSWnd_List::Draw
  - `if ( TSecType<long>::GetData(a1: (v274 + 16), a2: 0) / 100000 == 91 )`
  - `Data = TSecType<long>::GetData(a1: (v274 + 16), a2: 0);`
  - `v39 = TSecType<long>::GetData(a1: (v274 + 16), a2: 0);`
  - `if ( TSecType<long>::GetData(a1: (v274 + 16), a2: 0) == v56 )`
  - `if ( TSecType<long>::GetData(a1: (v274 + 16), a2: 0) == *v70 )`

### CCSWnd_List::GetCommodityGender
  - `Data = TSecType<long>::GetData(a1: (a2 + 16), a2: 0);`
  - `if ( TSecType<long>::GetData(a1: v7, a2: 0) / 10000 == 910 )`
  - `if ( TSecType<long>::GetData(a1: (v17 + 16), a2: 0) != 0 )`
  - `v13 = TSecType<long>::GetData(a1: (v17 + 16), a2: 0);`

### CCSWnd_List::GetPlateRect
  - `tagRECT->top = 81 * (nPlate % 10 / 2) + 2;`
  - `tagRECT->bottom = 81 * (nPlate % 10 / 2) + 82;`

### CCSWnd_List::OnButtonClicked
  - `CDialog::CreateDlg(a1: v12, a2: 1, a3: 0);`

### CCSWnd_List::OnCreate
  - `CCtrlSelector::SetSelectorAttr(this: *(this + 28), a2: -12949590, a3: -1, a4: -16777216);`

### CCSWnd_List::OnKeyRet
  - `CCSWnd_List::SetButtonKeyFocus(this, nPlateIndx: -1, nBtnIndx: -1);`
  - `CCSWnd_List::SetButtonKeyFocus(this, nPlateIndx: *(this + 92), nBtnIndx: *(this + 93));`
  - `CWvsCommodityEx::GetCommodityExByIndex(this: *dword_BE7918, a2: v29, a3: *(this + *(this + 92) + 94));`

### CCSWnd_List::ResetSlotIncComm
  - `v8 = TSecType<long>::GetData(a1: (v18 + 16), a2: v1) / 1000 % 10;`

### CCSWnd_List::SetPlateNo
  - `ZRef<CCtrlButton>::_ReleaseRaw(a1: v2 + 8 * v5 + 8 * v61 + 116);`
  - `Data = TSecType<long>::GetData(a1: (v56 + 16), a2: 0);`
  - `|| (v69 = TSecType<long>::GetData(a1: (v56 + 16), a2: 0)) / 100 == 11128 && v69 % 10 <= 2 )`
  - `if ( TSecType<long>::GetData(a1: (v56 + 16), a2: 0) == (&loc_5265BC + 4)`
  - `|| TSecType<long>::GetData(a1: (v56 + 16), a2: 0) == &off_5269A8 )`
  - `if ( TSecType<long>::GetData(a1: (v56 + 16), a2: 0) == &loc_4FAE70 )`
  - `if ( TSecType<long>::GetData(a1: (v24 + 16), a2: 0) / 10000 == 507 )`
  - `v27 = TSecType<long>::GetData(a1: (v56 + 16), a2: 0) / 10000;`
  - `v70 = TSecType<long>::GetData(a1: (v56 + 16), a2: 0);`

### CCSWnd_List::ShowItemToolTip
  - `if ( TSecType<long>::GetData(a1: pComm + 16, a2: 10000) / 10000 != 910 )`
  - `if ( TSecType<long>::GetData(a1: v8 + 16, a2: 1) == *v13 )`
  - `if ( TSecType<long>::GetData(a1: v8 + 16, a2: 1) / 10000 == 910 )`
  - `if ( TSecType<long>::GetData(a1: pComm + 16, a2: 1) / 1000 == 5430 )`
  - `v20 = TSecType<long>::GetData(a1: pComm + 16, a2: 1);`
  - `if ( get_bodypart_from_item(a1: v33, a2: 2, a3: &nBodyPart, a4: 0) != 0 && nBodyPart == 12 )`
  - `CUIToolTip::SetToolTip_SlotInc(this: (this + 153), a2: (v24 + rx + 20), a3: v48, a4: v49, a5: v50, a6: v51);`

### CCSWnd_List::WearCommodity
  - `if ( TSecType<long>::GetData(a1: (a3 + 16), a2: this) / 10000 == 910 )`

### CCSWnd_List::WearItem
  - `if ( TSecType<long>::GetData(a1: (a4 + 16), a2) / 1000000 == 1 )`
  - `v6 = TSecType<long>::GetData(a1: (a4 + 16), a2: 500);`
  - `CCashShop::SetPet(this: *(a1 + 576), a2: v7, a3: *(a4 + 12), a4: a5);`
  - `CCashShop::SetActiveEffectItem(this: *(a1 + 576), a2: v14, a3: *(a4 + 12));`

### CCSWnd_Locker::Draw
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 30), a2: 0);`

### CCSWnd_Locker::OnMouseButton
  - `CCSWnd_Char::EndChat(this: (*(this + 357) + 1364));`
  - `sub_472632(this: *(this + 357), a2: *(*v11 + v14), a3: *(*v11 + v14 + 4), a4: v30, a5: EmptySlotPosition);`
  - `CCtrlTab::SetTab(this: *(dword_BEC338 + 112), a2: v30 - 1);`

### CCSWnd_Locker::OnMouseMove
  - `CItemInfo::GetItemSlot(a1: &v16, a2: *(55 * LockerIndex + v6 + 16));`

### CCSWnd_Locker::ResetScrollBar
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 30), a2: (v1 + 5) / 6u - 1);`

### CCSWnd_Tab::ChangeCategory
  - `CCtrlSelector::SetSelectorAttr(this: v4, a2: -12346660, a3: -1, a4: -1);`
  - `CCtrlSelector::SetSelectorStart(this: *(this + 28), a2: 0, a3: 20, a4: 0, a5: 1);`
  - `CCtrlSelector::SetSelectorStart(this: *(this + 28), a2: v18, a3: 20, a4: 0, a5: 1);`

### CCSWnd_Tab::OnMouseButton
  - `CCSWnd_Char::EndChat(this: (*(this + 41) + 1364));`
  - `CWnd::InvalidateRect(this: (this - 1), a2: 0);`

### CCashShop::CheckTransferWorldPossible
  - `if ( (TSecType<unsigned char>::GetData(this: (*dword_BE7918 + 8240)) & 1) != 0 )`

### CCashShop::FindWishList
  - `Data = TSecType<long>::GetData(a1: (*(8 * v4 + *v3 + 4) + 16), a2: v3);`

### CCashShop::GiftWishItem
  - `Data = TSecType<long>::GetData(a1: (v7 + 16), a2: 0);`
  - `v13 = TSecType<long>::GetData(a1: (v40 + 16), a2: 0);`

### CCashShop::GoTo
  - `CCSWnd_List::SetCursorToPlate(this: (this + 5564), a2: a4 % 10);`

### CCashShop::IsUsableItem
  - `if ( CharacterData::GetActiveItemSlotPet(this: v2, a2: 0) == 0 )`
  - `v45 = *(CharacterData::GetItem(this: v75, a2: v72, a3: 5, a4: CashItemSlotPosition) + 4);`
  - `if ( CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v55, a3: 10000 * v57 + 1004, a4: 0) <= 0 )`

### CCashShop::IsUsableItemGift
  - `if ( CharacterData::GetActiveItemSlotPet(this: a4, a2: 0) == 0 )`
  - `&& CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: a4, a3: 1004, a4: 0) <= 0`
  - `&& CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: a4, a3: &loc_989A68 + 4, a4: 0) <= 0 )`

### CCashShop::LoadData
  - `if ( Unknown + v47 != 0 && TSecType<long>::GetData(a1: (*(*v24 + 8 * i + 4) + 16), a2: v24) == *v48 )`

### CCashShop::OnBuyAvatar
  - `Data = TSecType<long>::GetData(a1: (v18 + 16), a2: 0);`

### CCashShop::OnBuyPackage
  - `if ( TSecType<long>::GetData(a1: (v40 + 16), a2: 0) / 10000 != 910 )`
  - `TSecType<long>::GetData(a1: (v40 + 16), a2: 0);`
  - `Data = TSecType<long>::GetData(a1: v42 + 16, a2: 0);`
  - `v12 = TSecType<long>::GetData(a1: v42 + 16, a2: 0);`

### CCashShop::OnBuyTransferWorldItem
  - `Data = TSecType<long>::GetData(a1: (v18 + 16), a2: 0);`

### CCashShop::OnCashItemResLoadLockerDone
  - `CInPacket::DecodeBuffer(this: a2, a2: *(this + 290), a3: 55 * v3);`
  - `CWnd::InvalidateRect(this: (this + 617), a2: 0);`

### CCashShop::OnCashItemResLoadWishDone
  - `CWnd::InvalidateRect(this: (this + 5564), a2: 0);`

### CCashShop::OnCashItemResUseCouponDone
  - `CWnd::InvalidateRect(this: (v8 + 2468), a2: 0);`
  - `CCtrlTab::SetTab(this: *(dword_BEC338 + 112), a2: v21 - 1);`

### CCashShop::OnGiftMateInfoResult
  - `Data = TSecType<long>::GetData(a1: (v63 + 16), a2: 0);`

### CCashShop::OnGiftPackage
  - `if ( TSecType<long>::GetData(a1: (v29 + 16), a2: 0) / 10000 == 910 )`
  - `TSecType<long>::GetData(a1: (v29 + 16), a2: 0);`
  - `Data = TSecType<long>::GetData(a1: (v29 + 16), a2: 0);`

### CCashShop::OnKey
  - `CCashShop::SetUserPreviewControl(this: (this - 4), a2: IsControlingUserPreview == 0);`
  - `|| (result = CCSWnd_Char::OnKeyRet(this: (this + 1360), a2, a3)) == 0 )`

### CCashShop::OnQueryCashResult
  - `CWnd::InvalidateRect(this: (this + 7648), a2: 0);`

### CCashShop::OnRemoveWish
  - `if ( *(this + 6) == 0 && a2 != 0 && CCashShop::GetUsableCommodityIndexBySN(this, &a2) >= 0 )`

### CCashShop::RestoreFocus
  - `return CWndMan::SetFocus(this: v3, a2: IsControlingUserPreview == 0 ? (this != 0 ? (this + 4) : 0) : 0);`

### CCashShop::SendBuyAvatarPacket
  - `CWvsContext::GetCommodityBySN(a1: v31, a2: *(*(this + 307) + 4 * *(*(this + 308) + 4 * i)));`
  - `Data = TSecType<long>::GetData(a1: (v32 + 16), a2: 0);`
  - `COutPacket::Encode1(this: &v29, a2: *(this + 310) == 2);`

### CCashTradingRoomDlg::OnButtonClicked
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: *(this + 107) != 0 ? (*(this + 107) + 4) : 0);`

### CCashTradingRoomDlg::OnKey
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: *(this + 106) != 0 ? (*(this + 106) + 4) : 0);`

### CCashTradingRoomDlg::PutMoney
  - `CUtilDlgEx::SetUtilDlgEx(this: v9, a2: 2, a3: 0, a4: v12, a5: v13, a6: v14);`
  - `CUtilDlgEx::SetUtilDlgEx_INPUT_NO(this: v20, a2: 1, a3: 1, a4: v10, a5: 0, a6: 10, a7: 0);`

### CCashTradingRoomDlg::Trade
  - `Data = TSecType<long>::GetData(a1: v40 + 12, a2: 0);`
  - `v26 = TSecType<long>::GetData(a1: (v25 + 12), a2: 0);`
  - `COutPacket::Encode4(this: &v36, a2: *(v51 + 8 * k + 4));`

### CConfirmPurchaseDlg::Draw
  - `if ( TSecType<long>::GetData(a1: (pvarg.cyVal.Hi + 16), a2: 0) / 100000 == 91 )`
  - `Data = TSecType<long>::GetData(a1: (pvarg.cyVal.Hi + 16), a2: 0);`
  - `v13 = TSecType<long>::GetData(a1: (pvarg.cyVal.Hi + 16), a2: 0);`

### CConfirmPurchaseDlg::OnCreate
  - `CCtrlCheckBox::SetChecked(this: *(this + 42), a2: *(this + 33) == 2);`

### CCouponUseSelectDlg::Confirm
  - `CDialog::CreateDlg(a1: v8, a2: 1, a3: 0);`

### CCouponUseSelectDlg::Update
  - `if ( CCtrlWnd::IsFocused(this: *(this + 32)) != 0 )`

### CInPacket::WriteClientLog
  - `Buffer = ZXString<char>::GetBuffer(a1: 0, a2: 1);`

### CRegisterAuctionEntryDlg::OnKey
  - `CWnd::InvalidateRect(this: (this - 4), a2: 0);`

### CRegisterWishEntrySearchDlg::OnButtonClicked
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: *(this + 164) != 0 ? (*(this + 164) + 4) : 0);`

### CRegisterWishEntrySearchDlg::OnMouseButton
  - `CWnd::InvalidateRect(this: (this - 1), a2: v14[0]);`

### CUIAdminShopWishList::OnButtonClicked
  - `CUIAdminShopWishList::ToggleAddOn(this, a2: *(this + 44) == 0);`

### CUIAdminShopWishList::SearchItemName
  - `CWnd::InvalidateRect(this: *(this + 184), a2: 0);`

### CUIAdminShopWishListSearchResult::OnButtonClicked
  - `if ( CUIAdminShopWishList::CanAddList(this: *v33, a2: *(v7 + 4), a3: v28[0]) != 0 )`

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

### CUICashItemGachapon::Update
  - `ItemIcon = CItemInfo::GetItemIcon(a1: &p_lVal, a2: v41.decVal.Hi32, bPetDead: 0, bWithShadow: 0);`

### CUIRecommendWorld::MoveWorld
  - `*(this + 33) = (nMovement + v3 + *(this + 33)) % v3;`

### CUIRecommendWorld::ResetWorldAndMessage
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 44), a2: v37 - *(this + 45) + 1);`

### CUIShopScanResult::OnMouseMove
  - `CUIToolTip::SetToolTip_String(this: (this + 61), a2: v25 + a2 + 20, a3: v59, a4: ChannelName);`

### CUIUserInfoWishList::Draw
  - `if ( v65 != 0 && TSecType<long>::GetData(a1: (v65 + 16), a2: 0) != 0 )`
  - `v8 = TSecType<long>::GetData(a1: (v65 + 16), a2: 0);`

### CUIUserInfoWishList::OnCreate
  - ``eh vector constructor iterator'(a1: a1 + 31, a2: 8u, a3: 3, a4: sub_47391D, a5: ZRef<CCtrlButton>::_ReleaseRaw);`

### CUserPreview::DoMeleeAttack
  - `if ( CAvatar::GetOneTimeAction(this: (this + 136)) > -1 || CUser::IsOnLadderOrRope(this) != 0 )`
  - `random_melee_attack_action = get_random_melee_attack_action(a1: v5, a2: weapon_type, a3: 0, a4: 0, a5: v18);`
  - `CUser::SetAttackAction(this, a2: random_melee_attack_action, a3: 6, a4: 0, a5: 0);`
  - `v15 = 1500 * (CAvatar::GetOneTimeAction(this: (this + 136)) > -1);`

### CUserPreview::DoShootAttack
  - `if ( CAvatar::GetOneTimeAction(this: (this + 136)) > -1 || CUser::IsOnLadderOrRope(this) != 0 || *(this + 2746) != 0 )`
  - `random_shoot_attack_action = get_random_shoot_attack_action(a1: *(this + 318), a2: weapon_type, a3: 0, a4: 0, a5: a2);`
  - `CUser::SetAttackAction(this, a2: random_shoot_attack_action, a3: 6, a4: 0, a5: 0);`

### CUserPreview::Init
  - `CVecCtrl::Init(this: Instance, a2: this != 0 ? (this + 4) : 0);`

### CUserPreview::Update
  - `if ( *(this + 2760) != 0 && get_update_time() - *(this + 2767) > 5000 )`

### CVecCtrlUserPreview::CreateInstance
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 584) != 0 )`

### CVecCtrlUserPreview::EndUpdateActive
  - `RelPos::SetFromAbsPos(this: (this + 224), a2: v3, a3: *(this + 68));`

### CVecCtrlUserPreview::IsAbleToClimbLadderOrRope
  - `return CAvatar::GetOneTimeAction(this: (dword_BED5A4 + 136)) <= -1;`

### CVecCtrlUserPreview::WorkUpdateActive
  - `if ( CAvatar::GetOneTimeAction(this: (dword_BED5A4 + 136)) > -1`
  - `if ( _ZtlSecureFuse<CLadderOrRope *>(a1: this + 73, a2: *(this + 75)) != 0 )`
  - `&& _ZtlSecureFuse<CLadderOrRope *>(a1: this + 73, a2: *(this + 75)) == 0`
  - `(*(*this + 20))(a1: this, a2: 0, a3: LadderOrRope, a4: 0);`
  - `if ( _ZtlSecureFuse<CLadderOrRope *>(a1: this + 73, a2: *(this + 75)) == 0`
  - `if ( LadderOrRope[4] <= (v46 + 10) )`

### CVecCtrlUserPreview::WorkUpdateActiveLadderOrRope
  - `v2 = _ZtlSecureFuse<CLadderOrRope *>(a1: this + 73, a2: *(this + 75));`
  - `v7 = *(_ZtlSecureFuse<CLadderOrRope *>(a1: this + 73, a2: *(this + 75)) + 16);`
