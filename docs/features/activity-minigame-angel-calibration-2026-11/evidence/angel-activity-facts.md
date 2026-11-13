# Angel 083 活动小游戏面反编译语义抽取（观察层）

来源：`evidence/angel-activity-decompile-full.json`（逐字 Hex-Rays 伪代码，超大函数从 `/output/` 端点补全并归一化）。
本文件由 `tools/ida_angel_activity_facts.py` 自动生成，只做字符串抽取，不含推断。

## 1. StringPool 资源节点全集（238 个）

- `SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK` ← CField_Coconut, CField_MassacreResult, CMonsterBookMan, CNoticeQuestProgress, CQuest, CShip…
- `SP_1000_BLAZE` ← CField_Dojang, CMonsterBookMan
- `SP_10_EQP` ← CUIQuestAlarm
- `SP_11_SETUP` ← CUIItemMaker
- `SP_1231_TAB` ← CUIMonsterCarnival
- `SP_1246_ENCHANTSUCCESS` ← CUIEnchantDlg
- `SP_1247_ENCHANTFAILURE` ← CUIEnchantDlg
- `SP_1248_WHISTLE` ← CShip
- `SP_1252_SOUND_BGM04_ARABPIRATE` ← CShip
- `SP_1262_UI_BASICIMG_BTMAX` ← CUIMonsterCarnival
- `SP_12_BEGINNER` ← CUIMedalQuestInfo, CUIMonsterBook
- `SP_1300_UI_UIWINDOWIMG_FLOATNOTICE_D` ← CFloatNotice
- `SP_1345_UI_LOGINIMG_TITLE_BTLOGIN` ← CUITitle
- `SP_1347_UI_LOGINIMG_TITLE_BTLOGINIDSAVE` ← CUITitle
- `SP_1348_UI_LOGINIMG_TITLE_BTLOGINIDLOST` ← CUITitle
- `SP_1349_UI_LOGINIMG_TITLE_BTPASSWDLOST` ← CUITitle
- `SP_1350_UI_LOGINIMG_TITLE_BTNEW` ← CUITitle
- `SP_1351_UI_LOGINIMG_TITLE_BTHOMEPAGE` ← CUITitle
- `SP_1352_UI_LOGINIMG_TITLE_BTQUIT` ← CUITitle
- `SP_1353_UI_LOGINIMG_TITLE_CHECK_0` ← CUITitle
- `SP_1354_UI_LOGINIMG_TITLE_CHECK_1` ← CUITitle
- `SP_1388_UI_CHATBALLOONIMG_` ← CChatBalloon
- `SP_1389_NPC` ← CChatBalloon, CQuest
- `SP_1390_ADBOARD` ← CChatBalloon
- `SP_1410_CANVASFONT` ← CUIItemMaker, CUIMedalQuestInfo, CUIMonsterCarnival, CUIQuestInfo, CUIQuestInfoDetail
- `SP_1411_BA` ← CUIMedalQuestInfo, CUIQuestInfo, CUIQuestInfoDetail
- `SP_1414_UI_CHATBALLOONIMG_MINIROOM` ← CChatBalloon
- `SP_1416_OMOK` ← CChatBalloon
- `SP_1421_CNUM` ← CChatBalloon
- `SP_1422_MNUM` ← CChatBalloon
- `SP_1425_PROGRESS` ← CChatBalloon
- `SP_1589_ICONRAW` ← CMonsterBookMan
- `SP_1593_MOB` ← CChatBalloon, CMonsterBookMan, CQuest
- `SP_1604_NAME` ← CNoticeQuestProgress, CQuestMan, CUIQuestAlarm
- `SP_1612_MAXHP` ← CMonsterBookMan
- `SP_1613_MAXMP` ← CMonsterBookMan
- `SP_1662_` ← CUIMedalQuestInfo
- `SP_170_YOU_HAVE_EITHER_ENTERED_A_WRONG_NPC_NAME_OR` ← CUIQuestAlarm
- `SP_1729_STREETNAME` ← CUIMonsterBook
- `SP_1730_MAPNAME` ← CUIMonsterBook
- `SP_1735_AREA` ← CQuestMan, CUIMedalQuestInfo, CUIQuestInfo
- `SP_1752_HEIGHT` ← CTutor
- `SP_1883_UI_UIWINDOWIMG_ENERGYBAR` ← CUIEnergyBar
- `SP_1886_DISPLAYS_THE_ENERGY_LEVEL_THATS_BEEN_CHARGED_NUSE_THE_MOUSE_TO_CHANGE_POSITION` ← CUIEnergyBar
- `SP_1891_UI_UIWINDOWIMG_QUEST_BACKGRND2` ← CUIQuestInfoDetail
- `SP_1896_UI_UIWINDOWIMG_QUEST_LINE` ← CUIQuestInfoDetail
- `SP_1897_UI_UIWINDOWIMG_QUEST_ICON0` ← CUIQuestInfo
- `SP_1898_UI_UIWINDOWIMG_QUEST_ICON1` ← CUIQuestInfo
- `SP_1899_UI_UIWINDOWIMG_QUEST_ICON2` ← CUIQuestInfo
- `SP_1901_UI_UIWINDOWIMG_QUEST_ICON4` ← CUIQuestInfo
- `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID` ← CUIMedalQuestInfo, CUIMonsterCarnival, CUIQuestAlarm, CUIQuestInfo, CUITitle
- `SP_2239_STRING_MONSTERBOOKIMG` ← CMonsterBookMan
- `SP_2240_STRING_MOBIMG` ← CNoticeQuestProgress, CUIQuestAlarm
- `SP_2242_EPISODE` ← CMonsterBookMan
- `SP_2243_REWARD` ← CMonsterBookMan
- `SP_2356_ICON` ← CMonsterBookMan
- `SP_2366_ID` ← CQuest
- `SP_2501_DOJANG_TIMEOVER` ← CField_Dojang
- `SP_2558_DOJANG_TIMEOVER` ← CField_Dojang
- `SP_2561_UI_BASICIMG_BTHIDE` ← CUIQuestInfoDetail
- `SP_2584_UI_UIWINDOWIMG_MONSTERBOOK` ← CMonsterBookMan
- `SP_25_HERO` ← CUIQuestAlarm
- `SP_2664_UI_UIWINDOWIMG_QUEST_TAB_ENABLED` ← CUIQuestInfo
- `SP_2665_UI_UIWINDOWIMG_QUEST_TAB_DISABLED` ← CUIQuestInfo
- `SP_2774_03D` ← CField_Coconut
- `SP_2776_COCONUT` ← CField_Coconut
- `SP_2777_COUNTFALLING` ← CField_Coconut
- `SP_2778_COUNTBOMBING` ← CField_Coconut
- `SP_2779_COUNTSTOPPED` ← CField_Coconut
- `SP_2780_MAP_OBJ_ETCIMG_COCONUT_BACKGRND` ← CField_Coconut
- `SP_2781_MAP_OBJ_ETCIMG_COCONUT_FONTSCORE` ← CField_Coconut
- `SP_2782_MAP_OBJ_ETCIMG_COCONUT_FONTTIME` ← CField_Coconut
- `SP_2784_02D02D` ← CField_Coconut
- `SP_2973_QUEST` ← CQuest
- `SP_2_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTPASSWORD` ← CUITitle
- `SP_3199_QUEST_ACTIMG_D_D_ITEM` ← CQuest
- `SP_3212_QUEST_SAYIMG` ← CQuest
- `SP_3214_ASK` ← CQuest
- `SP_3216_ORDER` ← CQuestMan
- `SP_3217_ITEM` ← CQuest
- `SP_3218_QUEST_SAYIMG_D_1_LOST` ← CQuest
- `SP_3219_QUEST_SAYIMG_D_1_STOP_S` ← CQuest
- `SP_3220_QUEST_CHECKIMG` ← CQuestMan
- `SP_3232_QUEST_QUESTINFOIMG` ← CQuestMan, CUIMedalQuestInfo, CUIQuestInfo, CUIQuestInfoDetail
- `SP_3236_PARENT` ← CQuestMan, CUIMedalQuestInfo, CUIQuestInfo
- `SP_3438_TO_COMPLETE_THE_QUEST_YOULL_NEED_TO_UNSEAL_THE_QUEST_ITEM` ← CQuest
- `SP_3452_ITEM_CASH_0503IMG` ← CChatBalloon
- `SP_3458_SIGNBOARD` ← CChatBalloon
- `SP_3468_THE_OWNER_OF_THE_STORE_IS_CURRENTLY_R_NUNDERGOING_STORE_MAINTENANCE_R_NPLEASE_TR` ← CMiniRoomBaseDlg
- `SP_3475_YOU_MAY_NOT_OPEN_THE_STORE_NEAR_A_PORTAL` ← CMiniRoomBaseDlg
- `SP_3506_UNABLE_TO_COMPLETE_THE_QUEST_R_NDUE_TO_A_LACK_OF_PET_R_NPLEASE_TRY_AGAIN_AFTER_S` ← CQuest
- `SP_3507_UNABLE_TO_COMPLETE_THE_QUEST_R_NWITH_THE_CURRENT_PET_YOU_HAVE_SUMMONED_R_NPLEASE` ← CQuest
- `SP_3620_QUEST_HELPER_D_5` ← CUIQuestAlarm
- `SP_3621_UI_BASICIMG_BTCLOSE2` ← CUIQuestAlarm
- `SP_3654_EFFECT_MAPEFFIMG_VIEWRANGE_1` ← CField_LimitedView
- `SP_366_UNABLE_TO_FIND_THE_CHARACTER` ← CMiniRoomBaseDlg
- `SP_3675_EBROCK_PAPER_SCISSORS_NOTICENK_R_NTHE_PARTICIPATION_FEE_FOR_THIS_ROCK_PAPER_SCIS` ← CRPSGameDlg
- `SP_37_ARCH_MAGEFIREPOISON` ← CField_Coconut
- `SP_3830_ITEM_CONSUME_0238IMG` ← CMonsterBookMan
- `SP_387_THE_SCROLL_LIGHTS_UP_AND_THEN_ITS_MYSTERIOUS_POWER_HAS_BEEN_TRANSFERRED_TO_THE_I` ← CUIEnchantDlg
- `SP_388_THE_SCROLL_LIGHTS_UP_BUT_THE_ITEM_WINDS_UP_AS_IF_NOTHING_HAPPENED` ← CUIEnchantDlg
- `SP_389_THE_ITEM_IS_DESTROYED_DUE_TO_THE_OVERWHELMING_POWER_OF_THE_SCROLL` ← CUIEnchantDlg
- `SP_394_THE_ROOM_IS_ALREADY_CLOSED` ← CMiniRoomBaseDlg
- `SP_395_YOU_CANT_ENTER_THE_ROOM_DUE_TO_FULL_CAPACITY` ← CMiniRoomBaseDlg
- `SP_3965_THE_ITEM_UPGRADE_FAILED_BUT_SINCE_THE_WHITE_SCROLL_WAS_USED_THE_NUMBER_OF_ITEM_U` ← CUIEnchantDlg
- `SP_3966__THE_ITEM_WAS_SUCCESSFULLY_UPGRADED_AND_THE_WHITE_SCROLL_WAS_USED_IN_THE_PROCESS` ← CUIEnchantDlg
- `SP_396_OTHER_REQUESTS_ARE_BEING_FULFILLED_THIS_MINUTE` ← CMiniRoomBaseDlg
- `SP_397_YOU_CANT_DO_IT_WHILE_YOURE_DEAD` ← CMiniRoomBaseDlg
- `SP_398_YOU_CANT_DO_IT_IN_THE_MIDDLE_OF_THE_EVENT` ← CMiniRoomBaseDlg
- `SP_399_THIS_CHARACTER_IS_UNABLE_TO_DO_IT` ← CMiniRoomBaseDlg
- `SP_3_HTTP__MAPLESTORYNEXONNET_WZASPXPART_DOWNLOADS_GAMEPATCHES` ← CShip
- `SP_400_YOU_ARE_NOT_ALLOWED_TO_TRADE_OTHER_ITEMS_AT_THIS_POINT` ← CMiniRoomBaseDlg
- `SP_401_YOU_CANT_ESTABLISH_A_MINIROOM_RIGHT_HERE` ← CMiniRoomBaseDlg
- `SP_402_YOU_CAN_ONLY_MAKE_TRADE_R_NIF_YOU_TWO_ARE_IN_THE_SAME_MAP` ← CMiniRoomBaseDlg
- `SP_403_S_IS_DOING_SOMETHING_ELSE_RIGHT_NOW` ← CMiniRoomBaseDlg
- `SP_404_S_HAVE_DENIED_INVITATION` ← CMiniRoomBaseDlg
- `SP_405_S_IS_CURRENTLY_NOT_ACCEPTING_ANY_INVITATION` ← CMiniRoomBaseDlg
- `SP_4063_AUTOSTART` ← CQuestMan
- `SP_4066_ONESHOT` ← CQuestMan
- `SP_4067_TIMELIMIT` ← CQuestMan
- `SP_4078_MAPLE_RED` ← CField_MonsterCarnival
- `SP_4079_MAPLE_BLUE` ← CField_MonsterCarnival
- `SP_4080__S__HAS_BECOME_UNABLE_TO_FIGHT_AND__S_TEAM_HAS_LOST_D_CP` ← CField_MonsterCarnival
- `SP_4081__S__HAS_BECOME_UNABLE_TO_FIGHT_BUT__S__HAS_NO_CP_SO__S__TEAM_DID_NOT_LOSE_ANY_CP` ← CField_MonsterCarnival
- `SP_4082_YOU_DONT_HAVE_ENOUGH_CP_TO_CONTINUE` ← CField_MonsterCarnival
- `SP_4083_YOU_CAN_NO_LONGER_SUMMON_THE_MONSTER` ← CField_MonsterCarnival
- `SP_4084_YOU_CAN_NO_LONGER_SUMMON_THE_BEING` ← CField_MonsterCarnival
- `SP_4085_THIS_BEING_IS_ALREADY_SUMMONED` ← CField_MonsterCarnival
- `SP_4086_THIS_REQUEST_HAS_FAILED_DUE_TO_AN_UNKNOWN_ERROR` ← CField_MonsterCarnival
- `SP_4087_YOU_HAVE_WON_THE_MONSTER_CARNIVAL_PLEASE_WAIT_AS_YOULL_BE_TRANSPORTED_OUT_OF_HER` ← CField_MonsterCarnival
- `SP_4088_UNFORTUNATELY_YOU_HAVE_LOST_THE_MONSTER_CARNIVAL_PLEASE_WAIT_AS_YOULL_BE_TRANSPO` ← CField_MonsterCarnival
- `SP_4089_DESPITE_THE_OVERTIME_THE_CARNIVAL_ENDED_IN_A_DRAW_PLEASE_WAIT_AS_YOULL_BE_TRANSP` ← CField_MonsterCarnival
- `SP_4090_MONSTER_CARNIVAL_HAS_ENDED_ABRUPTLY_DUE_TO_THE_OPPOSING_TEAM_LEAVING_THE_GAME_TO` ← CField_MonsterCarnival
- `SP_4096__S__OF_TEAM__S__HAS_QUIT_THE_MONSTER_CARNIVAL` ← CField_MonsterCarnival
- `SP_4097_SINCE_THE_LEADER_OF_THE_TEAM__S__QUIT_THE_MONSTER_CARNIVAL__S__HAS_BEEN_APPOINTE` ← CField_MonsterCarnival
- `SP_4098_UI_UIWINDOWIMG_MONSTERCARNIVAL_ICON0` ← CUIMonsterCarnival
- `SP_4099_UI_UIWINDOWIMG_MONSTERCARNIVAL_ICON1` ← CUIMonsterCarnival
- `SP_4102_UI_UIWINDOWIMG_MONSTERCARNIVAL_BACKGRND3_TOP_0` ← CUIMonsterCarnival
- `SP_4103_UI_UIWINDOWIMG_MONSTERCARNIVAL_BACKGRND3_MIDDLE1_0` ← CUIMonsterCarnival
- `SP_4104_UI_UIWINDOWIMG_MONSTERCARNIVAL_BACKGRND3_BOTTOM_0` ← CUIMonsterCarnival
- `SP_4105_UI_UIWINDOWIMG_MONSTERCARNIVAL_BACKGRND3_MIDDLE0_0` ← CUIMonsterCarnival
- `SP_4106_UI_UIWINDOWIMG_MONSTERCARNIVAL_BTSIDE` ← CUIMonsterCarnival
- `SP_4107_UI_UIWINDOWIMG_MONSTERCARNIVAL_TAB_ENABLED` ← CUIMonsterCarnival
- `SP_4108_UI_UIWINDOWIMG_MONSTERCARNIVAL_TAB_DISABLED` ← CUIMonsterCarnival
- `SP_4109_UI_BASICIMG_BTDECIDE` ← CUIMonsterCarnival
- `SP_4112_UI_UIWINDOWIMG_MONSTERCARNIVAL_ICON2` ← CUIMonsterCarnival
- `SP_4113_UI_UIWINDOWIMG_MONSTERCARNIVAL_ICON3` ← CUIMonsterCarnival
- `SP_4128_CP_EARNED_THAT_CAN_BE_USED` ← CUIMonsterCarnival
- `SP_4129_CP_REMAINING_D___CP_EARNED_D` ← CUIMonsterCarnival
- `SP_4130_THE_GAME_WILL_BE_DECIDED_BASED_ON_THE_TOTAL_CP_EARNED_BETWEEN_TWO_TEAMS` ← CUIMonsterCarnival
- `SP_4132_COMPARE_THE_TOTAL_CP_EARNED_BETWEEN_TWO_TEAMS_TO_FIND_THE_WINNER` ← CUIMonsterCarnival
- `SP_4166_AUTOPRECOMPLETE` ← CQuestMan
- `SP_4176_AUTOMATIC_LOCK_DISABLED` ← CUIMonsterCarnival
- `SP_4177_AUTOMATIC_LOCK_ENABLED` ← CUIMonsterCarnival
- `SP_428_YOU_MAY_NOT_ENTER_THIS_STORE` ← CMiniRoomBaseDlg
- `SP_433_YOU_DONT_HAVE_ENOUGH_MESOS_TO_ENTER` ← CMiniRoomBaseDlg
- `SP_434_YOU_CANT_START_THE_GAME_HERE` ← CMiniRoomBaseDlg
- `SP_435_THE_PERSONAL_STORE_CAN_ONLY_BE_BUILT_R_NAT_THE_FREE_MARKET_AVAILABLE_AT_MAIN_TOW` ← CMiniRoomBaseDlg
- `SP_436_YOU_MAY_ONLY_USE_IT_AT_ROOM_722_LOCATED_AT_THE_2ND_FLOOR_AND_ABOVE_OF_THE_FREE_M` ← CMiniRoomBaseDlg
- `SP_4505_UI_UIWINDOWIMG_SLIDEMENU_D` ← CSlideMenuDlg
- `SP_4506_UI_UIWINDOWIMG_SLIDEMENU_D_BACKGRD` ← CSlideMenuDlg
- `SP_4507_UI_UIWINDOWIMG_SLIDEMENU_D_BTMAIN_D` ← CSlideMenuDlg
- `SP_4512_UI_UIWINDOWIMG_SLIDEMENU_D_CHOICE` ← CSlideMenuDlg
- `SP_4513_UI_UIWINDOWIMG_SLIDEMENU_D_COVER` ← CSlideMenuDlg
- `SP_4530_SORTKEY` ← CQuestMan
- `SP_4552_UI_UIWINDOWIMG_MURUENGRAID_NUMBER_BAR` ← CField_Dojang
- `SP_472_YOU_ARE_UNABLE_TO_ENTER_THE_TOURNAMENT_ROOM` ← CMiniRoomBaseDlg
- `SP_5066_UI_UIWINDOWIMG_CASHGACHAPON_BTOPEN` ← CUICashItemGachapon
- `SP_5067_UI_UIWINDOWIMG_CASHGACHAPON_BTCLOSE` ← CUICashItemGachapon
- `SP_5105_VIEWMEDALITEM` ← CUIMedalQuestInfo
- `SP_5106_MEDALCATEGORY` ← CUIMedalQuestInfo
- `SP_5111_AUTOACCEPT` ← CQuestMan
- `SP_5138_UI_TUTORIALIMG_D` ← CTutor
- `SP_5162_UI_UIWINDOWIMG_TITLE_BACKGRND2` ← CUIMedalQuestInfo
- `SP_5164_UI_UIWINDOWIMG_TITLE_BACKGRND4` ← CUIMedalQuestInfo
- `SP_5168_UI_UIWINDOWIMG_TITLE_ICON1` ← CUIMedalQuestInfo
- `SP_5169_UI_UIWINDOWIMG_TITLE_ICON2` ← CUIMedalQuestInfo
- `SP_5170_UI_UIWINDOWIMG_TITLE_TAB_DISABLED` ← CUIMedalQuestInfo
- `SP_5171_UI_UIWINDOWIMG_TITLE_TAB_ENABLED` ← CUIMedalQuestInfo
- `SP_5256_YOU_CANNOT_OPEN_A_STORE_IN_THIS_CHANNEL_PLEASE_USE_A_DIFFERENT_CHANNEL` ← CMiniRoomBaseDlg
- `SP_5300_NUMBER_OF_MEDALS_OBTAINED_D` ← CUIMedalQuestInfo
- `SP_5301_OBTAINABLE_TITLE` ← CUIMedalQuestInfo
- `SP_5302_TITLE_BEING_CHALLENGED` ← CUIMedalQuestInfo
- `SP_5319_THIS_ITEM_HAS_EXPIRED` ← CMiniRoomBaseDlg
- `SP_5354_SHOWLAYERTAG` ← CQuestMan
- `SP_5371_UI_UIWINDOWIMG_MONSTERKILLING_COUNT_BACKGRD0` ← CField_Massacre
- `SP_5372_UI_UIWINDOWIMG_MONSTERKILLING_COUNT_BACKGRD1` ← CField_Massacre
- `SP_5376_UI_UIWINDOWIMG_MONSTERKILLING_COUNT_NUMBER` ← CField_Massacre
- `SP_5377_UI_UIWINDOWIMG_MONSTERKILLING_COUNT_NUMBER2` ← CField_Massacre
- `SP_5378_UI_UIWINDOWIMG_MONSTERKILLING_GAUGE_BACKGRD` ← CField_Massacre
- `SP_5379_UI_UIWINDOWIMG_MONSTERKILLING_GAUGE_BACKGRDD` ← CField_Massacre
- `SP_5380_UI_UIWINDOWIMG_MONSTERKILLING_GAUGE_DANGER` ← CField_Massacre
- `SP_5381_UI_UIWINDOWIMG_MONSTERKILLING_GAUGE_ICOND` ← CField_Massacre
- `SP_5382_UI_UIWINDOWIMG_MONSTERKILLING_GAUGE_PIXEL` ← CField_Massacre
- `SP_5383_UI_UIWINDOWIMG_MONSTERKILLING_GAUGE_TEXT` ← CField_Massacre
- `SP_5384_UI_UIWINDOWIMG_MONSTERKILLING_GAUGE_TEXTD` ← CField_Massacre
- `SP_5385_UI_UIWINDOWIMG_MONSTERKILLING_RESULT_BACKGRD` ← CField_MassacreResult
- `SP_5386_UI_UIWINDOWIMG_MONSTERKILLING_RESULT_BACKGRD2` ← CField_MassacreResult
- `SP_5389_UI_UIWINDOWIMG_MONSTERKILLING_RESULT_RANK_A` ← CField_MassacreResult
- `SP_5390_UI_UIWINDOWIMG_MONSTERKILLING_RESULT_RANK_B` ← CField_MassacreResult
- `SP_5391_UI_UIWINDOWIMG_MONSTERKILLING_RESULT_RANK_C` ← CField_MassacreResult
- `SP_5392_UI_UIWINDOWIMG_MONSTERKILLING_RESULT_RANK_D` ← CField_MassacreResult
- `SP_5393_UI_UIWINDOWIMG_MONSTERKILLING_RESULT_RANK_S` ← CField_MassacreResult
- `SP_5515_D` ← CChatBalloon
- `SP_5527_ARIAL` ← CUIItemMaker, CUIMedalQuestInfo, CUIMonsterCarnival, CUIQuestInfo, CUIQuestInfoDetail
- `SP_5533_CATEGORY` ← CUIItemMaker
- `SP_5538_ETC` ← CUIItemMaker
- `SP_5550_MESO` ← CUIQuestAlarm
- `SP_5567_THE_PASSWORD_IS_INCORRECT` ← CMiniRoomBaseDlg
- `SP_5576_TITLE` ← CUIMedalQuestInfo
- `SP_5596_USE` ← CUIItemMaker
- `SP_5599_YOU_DO_NOT_HAVE_ENOUGH_MESOS` ← CUIItemMaker
- `SP_5612_A0` ← CUIMonsterBook
- `SP_5613_A1` ← CUIMonsterBook
- `SP_5619_BACKGRND` ← CChatBalloon
- `SP_5621_COUNT` ← CQuest
- `SP_5623_DEAD` ← CChatBalloon
- `SP_5624_DELAY` ← CChatBalloon, CUIMonsterBook
- `SP_5633_HTTP__MAPLESTORYNEXONNET` ← CUITitle
- `SP_5643_PET` ← CChatBalloon
- `SP_598_STRENGTHENING_GEM` ← CUIItemMaker
- `SP_599_MONSTER_CRYSTAL` ← CUIItemMaker
- `SP_600_DISASSEMBLE_EQUIPMENT` ← CUIItemMaker
- `SP_753_PRODUCTION_FAILED` ← CUIItemMaker
- `SP_756__WOULD_YOU_LIKE_TO_MAKE_THIS` ← CUIItemMaker
- `SP_757_WOULD_YOU_LIKE_TO_MAKE_A_MONSTER_CRYSTAL` ← CUIItemMaker
- `SP_758_WOULD_YOU_LIKE_TO_DISASSEMBLE` ← CUIItemMaker
- `SP_759_WOULD_YOU_LIKE_TO_MAKE_` ← CUIItemMaker
- `SP_760_APPLY_THE_DROP_ITEMS_HERE_R_N_TO_MAKE_THE_MONSTER_CRYSTAL` ← CUIItemMaker
- `SP_761_APPLY_THE_EQUIPMENT_TO_BE_DISASSEMBLED_HERE_R_N_TO_ACQUIRE_THE_MONSTER_CRYSTAL` ← CUIItemMaker
- `SP_83_THERE_IS_CURRENTLY_NOT_ENOUGH_AVAILABLE_MEMORY_TO_RUN_MAPLESTORY_R_NPLEASE_VISIT` ← CField_Coconut
- `SP_950_MAP` ← CMonsterBookMan
- `SP_954_MOB_07DIMG` ← CMonsterBookMan, CUIMonsterBook
- `SP_956_CANVAS` ← CField_Coconut, CField_Dojang, CField_LimitedView, CSlideNotice, CUIEnergyBar, CUIMonsterCarnival
- `SP_960_DEFAULT` ← CQuest, CUIMonsterBook
- `SP_961_INFO` ← CMonsterBookMan, CQuest, CUIMonsterBook
- `SP_977_X` ← CField_GuildBoss
- `SP_978_Y` ← CField_GuildBoss, CFloatNotice

## 2. 活动小游戏类逐函数关键行


### CMiniRoomBaseDlg（15 函数）


#### CMiniRoomBaseDlg::CheckAndSendChat  (895 B)

  - `void __thiscall CMiniRoomBaseDlg::CheckAndSendChat(void **this, char a2)`
  - `if ( CChatHelper::TryChat(this: this + 35, &a2) != 0 )`
  - `COutPacket::COutPacket(a1: 123);`
  - `LOBYTE(v6) = 1;`
  - `COutPacket::Encode1(this: &v3, a2: 6);`
  - `ZXString<char>::operator=(a1: &a2);`
  - `COutPacket::EncodeStr(a1: v2);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v3);`
  - `LOBYTE(v6) = 0;`
  - `ZArray<unsigned char>::RemoveAll(a1: v4);`

#### CMiniRoomBaseDlg::DecodeAvatar  (2161 B)

  - `int __thiscall CMiniRoomBaseDlg::DecodeAvatar(CMiniRoomBaseDlg *this, int a2, struct CInPacket *a3)`
  - `(*(v4 + 60))(a1: this, a2, a3: &v27, a4: &v25);`
  - `ZRef<CAvatar>::_Alloc(a1: v6);`
  - `CWnd::GetLayer(a1: &v16);`
  - `Layer = CWnd::GetLayer(a1: &a2);`
  - `LOBYTE(v28) = 1;`
  - `sub_428EC4(a1: Layer);`
  - `LOBYTE(v28) = 2;`
  - `v10 = sub_65FD9D(a1: a3);`
  - `LOBYTE(v28) = 5;`
  - `v11 = sub_4285F2(a1: v6);`
  - `CAvatar::Init(`
  - `if ( a2 != 0 )`
  - `return (*(*a2 + 8))(a1: a2);`

#### CMiniRoomBaseDlg::Draw  (783 B)

  - `int __thiscall CMiniRoomBaseDlg::Draw(CMiniRoomBaseDlg *this, const struct tagRECT *a2)`
  - `CWnd::Draw(this, a2);`
  - `result = sub_65FAB9(a1: this);`
  - `if ( *(this + 51) > 0 )`
  - `v6 = (this + 224);`
  - `if ( *v6 != 0 )`
  - `CMiniRoomBaseDlg::GetUserID(this, a2: &v7, a3: v5);`
  - `result = (*(*this + 84))(a1: this, a2: v5, a3: v7);`
  - `while ( v5 < *(this + 51) );`

#### CMiniRoomBaseDlg::GetUserID  (199 B)

  - `_DWORD *__thiscall CMiniRoomBaseDlg::GetUserID(char *this, _DWORD *a2, int a3)`
  - `ZXString<char>::operator=(a1: this + 4 * a3 + 284);`

#### CMiniRoomBaseDlg::MiniRoomFactory  (1626 B)

- 分支 case：`1`, `2`, `3`, `4`, `5`, `6`
  - `static CEntrustedShopDlg *__cdecl CMiniRoomBaseDlg::MiniRoomFactory(int a1)`
  - `switch ( a1 )`
  - `v7 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 2844);`
  - `if ( v7 != 0 )`
  - `return COmokDlg::COmokDlg(this: v7);`
  - `v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 2096);`
  - `if ( v6 != 0 )`
  - `return CMemoryGameDlg::CMemoryGameDlg(this: v6);`
  - `v5 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1772);`
  - `if ( v5 != 0 )`
  - `return CTradingRoomDlg::CTradingRoomDlg(this: v5);`
  - `v4 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1916);`
  - `if ( v4 != 0 )`
  - `return CPersonalShopDlg::CPersonalShopDlg(this: v4);`
  - `v3 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1960);`
  - `if ( v3 != 0 )`
  - …（截断 16 行）

#### CMiniRoomBaseDlg::OnAvatar  (387 B)

  - `void __thiscall CMiniRoomBaseDlg::OnAvatar(CMiniRoomBaseDlg *this, int a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `if ( *(this + 2 * v3 + 56) == 0 )`
  - `_CxxThrowException(a1: &a2, a2: &_TI3_AVCDisconnectException__);`
  - `CMiniRoomBaseDlg::DecodeAvatar(this, a2: v3, a3: a2);`

#### CMiniRoomBaseDlg::OnCheckSSN2Static  (3892 B)

  - `static signed int __cdecl CMiniRoomBaseDlg::OnCheckSSN2Static(struct CInPacket *a1)`
  - `v2 = CInPacket::Decode1(this: a1);`
  - `v3 = CInPacket::Decode1(this: a1);`
  - `result = CInPacket::Decode1(this: a1);`
  - `if ( result != 0 )`
  - `if ( v2 != 0 )`
  - `if ( v2 != 4 )`
  - `v10 = CInPacket::Decode4(this: a1);`
  - `v11 = CInPacket::Decode1(this: a1);`
  - `COutPacket::COutPacket(a1: 123);`
  - `COutPacket::Encode1(this: &v14, a2: 4);`
  - `COutPacket::Encode4(this: &v14, a2: v10);`
  - `COutPacket::Encode1(this: &v14, a2: 0);`
  - `COutPacket::Encode1(this: &v14, a2: v11);`
  - `if ( v11 != 0 )`
  - `v12 = CInPacket::Decode2(this: a1);`
  - …（截断 16 行）

#### CMiniRoomBaseDlg::OnEnterBase  (749 B)

  - `int __thiscall CMiniRoomBaseDlg::OnEnterBase(CMiniRoomBaseDlg *this, int a2)`
  - `v4 = CInPacket::Decode1(this: a2);`
  - `if ( *(this + 2 * v4 + 56) != 0 )`
  - `_CxxThrowException(a1: &a2, a2: &_TI3_AVCDisconnectException__);`
  - `CMiniRoomBaseDlg::DecodeAvatar(this, a2: v4, a3: v2);`
  - `v5 = CInPacket::DecodeStr(a1: &a2);`
  - `ZXString<char>::operator=(a1: v5);`
  - `++*(this + 52);`
  - `return (*(v6 + 68))(a1: this, a2: v4, a3: v2);`

#### CMiniRoomBaseDlg::OnEnterResultBase  (1417 B)

  - `int __thiscall CMiniRoomBaseDlg::OnEnterResultBase(CMiniRoomBaseDlg *this, struct CInPacket *a2)`
  - `*(this + 51) = CInPacket::Decode1(this: a2);`
  - `*(this + 50) = CInPacket::Decode1(this: a2);`
  - `while ( 1 )`
  - `v4 = CInPacket::Decode1(this: v2);`
  - `if ( v4 < 0 )`
  - `if ( (*(*this + 92))(a1: this) == 0 || v4 != 0 )`
  - `CMiniRoomBaseDlg::DecodeAvatar(this, a2: v4, a3: v2);`
  - `v5 = CInPacket::Decode4(this: v2);`
  - `(*(v10 + 96))(a1: this, a2: v5);`
  - `v6 = CInPacket::DecodeStr(a1: v9);`
  - `ZXString<char>::operator=(a1: v6);`
  - `++*(this + 52);`
  - `if ( dword_BE7910 != 0 && (*(*(dword_BE7910 + 4) + 72))(a1: dword_BE7910 + 4, a2: &dword_BEDB00) != 0 )`
  - `return (*(*v7 + 72))(a1: v7, a2: v2);`

#### CMiniRoomBaseDlg::OnEnterResultStatic  (13517 B)

- 分支 case：`1`, `2`, `3`, `4`, `5`, `6`, `7`, `9`, `10`, `11`, `12`, `13`, `14`, `15`, `16`, `17`, `18`, `19`, `20`, `21`, `22`, `24`
- SP_ 节点：`SP_3468_THE_OWNER_OF_THE_STORE_IS_CURRENTLY_R_NUNDERGOING_STORE_MAINTENANCE_R_NPLEASE_TR`, `SP_3475_YOU_MAY_NOT_OPEN_THE_STORE_NEAR_A_PORTAL`, `SP_394_THE_ROOM_IS_ALREADY_CLOSED`, `SP_395_YOU_CANT_ENTER_THE_ROOM_DUE_TO_FULL_CAPACITY`, `SP_396_OTHER_REQUESTS_ARE_BEING_FULFILLED_THIS_MINUTE`, `SP_397_YOU_CANT_DO_IT_WHILE_YOURE_DEAD`, `SP_398_YOU_CANT_DO_IT_IN_THE_MIDDLE_OF_THE_EVENT`, `SP_399_THIS_CHARACTER_IS_UNABLE_TO_DO_IT`, `SP_400_YOU_ARE_NOT_ALLOWED_TO_TRADE_OTHER_ITEMS_AT_THIS_POINT`, `SP_401_YOU_CANT_ESTABLISH_A_MINIROOM_RIGHT_HERE`, `SP_402_YOU_CAN_ONLY_MAKE_TRADE_R_NIF_YOU_TWO_ARE_IN_THE_SAME_MAP`, `SP_428_YOU_MAY_NOT_ENTER_THIS_STORE`, `SP_433_YOU_DONT_HAVE_ENOUGH_MESOS_TO_ENTER`, `SP_434_YOU_CANT_START_THE_GAME_HERE`, `SP_435_THE_PERSONAL_STORE_CAN_ONLY_BE_BUILT_R_NAT_THE_FREE_MARKET_AVAILABLE_AT_MAIN_TOW`, `SP_436_YOU_MAY_ONLY_USE_IT_AT_ROOM_722_LOCATED_AT_THE_2ND_FLOOR_AND_ABOVE_OF_THE_FREE_M`, `SP_472_YOU_ARE_UNABLE_TO_ENTER_THE_TOURNAMENT_ROOM`, `SP_5256_YOU_CANNOT_OPEN_A_STORE_IN_THIS_CHANNEL_PLEASE_USE_A_DIFFERENT_CHANNEL`, `SP_5319_THIS_ITEM_HAS_EXPIRED`, `SP_5567_THE_PASSWORD_IS_INCORRECT`
  - `static void __cdecl CMiniRoomBaseDlg::OnEnterResultStatic(int a1)`
  - `if ( dword_BE7910 != 0 )`
  - `_CxxThrowException(&a1, a2: &_TI3_AVCDisconnectException__);`
  - `v1 = CInPacket::Decode1(this: a1);`
  - `if ( v1 != 0 )`
  - `v50 = CMiniRoomBaseDlg::MiniRoomFactory(a1: v1);`
  - `if ( v50 == 0 )`
  - `v56 = *unknown_libname_3(a1: 553648131);`
  - `_CxxThrowException(a1: &v56, a2: &_TI3_AVCDisconnectException__);`
  - `*(v50 + 54) = v2;`
  - `CMiniRoomBaseDlg::OnEnterResultBase(this: v50, a2: v55);`
  - `v3 = CInPacket::Decode1(this: a1);`
  - `switch ( v3 )`
  - `LOBYTE(v78) = 2;`
  - `ZXString<char>::operator=(a1: String);`
  - `LOBYTE(v78) = 1;`
  - …（截断 16 行）

#### CMiniRoomBaseDlg::OnInviteResultStatic  (3761 B)

- SP_ 节点：`SP_366_UNABLE_TO_FIND_THE_CHARACTER`, `SP_403_S_IS_DOING_SOMETHING_ELSE_RIGHT_NOW`, `SP_404_S_HAVE_DENIED_INVITATION`, `SP_405_S_IS_CURRENTLY_NOT_ACCEPTING_ANY_INVITATION`
  - `static int __cdecl CMiniRoomBaseDlg::OnInviteResultStatic(volatile LONG *a1)`
  - `result = CInPacket::Decode1(this: a1);`
  - `if ( result != 0 )`
  - `if ( v2 != 0 )`
  - `if ( v3 != 0 )`
  - `if ( v4 == 0 )`
  - `CInPacket::DecodeStr(a1: v19);`
  - `LOBYTE(v20) = 4;`
  - `LOBYTE(v20) = 5;`
  - `ZXString<char>::Format(&a1, a2: v13, a3: v19[0]);`
  - `if ( v17 != 0 )`
  - `LOBYTE(v20) = 0;`
  - `if ( v4 == 1 )`
  - `LOBYTE(v20) = 6;`
  - `LOBYTE(v20) = 7;`
  - `ZXString<char>::Format(&a1, a2: v12, a3: v19[0]);`
  - …（截断 16 行）

#### CMiniRoomBaseDlg::OnInviteStatic  (2067 B)

  - `static void __cdecl CMiniRoomBaseDlg::OnInviteStatic(struct CInPacket *a1)`
  - `v2 = CInPacket::Decode1(this: a1);`
  - `CInPacket::DecodeStr(a1: v10);`
  - `v12 = CInPacket::Decode4(this: a1);`
  - `if ( (TSecType<unsigned char>::GetData(this: v1 + 2060) & 1) != 0 && sub_4F38B4(this: v1) )`
  - `CMiniRoomBaseDlg::SendInviteResult(a1: v12, a2: 4);`
  - `LOBYTE(v11) = 1;`
  - `if ( v2 == 3 || v2 == 6 )`
  - `if ( *(TSingleton<CConfig>::ms_pInstance + 11) == 0`
  - `|| (v8 = &v7,`
  - `ZXString<char>::operator=(a1: v10),`
  - `LOBYTE(v11) = 1,`
  - `sub_538C98(),`
  - `CConfig::IsInBlackList(a1: v7) != 0) )`
  - `v4 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 280);`
  - `LOBYTE(v11) = 3;`
  - …（截断 16 行）

#### CMiniRoomBaseDlg::OnPacketBase  (1601 B)

- 分支 case：`3`, `4`, `6`, `9`, `10`
  - `int __cdecl CMiniRoomBaseDlg::OnPacketBase(struct CInPacket *a1)`
  - `v1 = CInPacket::Decode1(this: a1);`
  - `if ( dword_BE7910 == 0 )`
  - `if ( v3 == 0 )`
  - `return CMiniRoomBaseDlg::OnInviteStatic(a1);`
  - `if ( v4 != 0 )`
  - `if ( v5 == 0 )`
  - `return CMiniRoomBaseDlg::OnEnterResultStatic(a1);`
  - `if ( v5 == 9 )`
  - `return CMiniRoomBaseDlg::OnCheckSSN2Static(a1);`
  - `return CMiniRoomBaseDlg::OnInviteResultStatic(a1);`
  - `result = (*(*(dword_BE7910 + 4) + 72))(a1: dword_BE7910 + 4, a2: &dword_BEDB00);`
  - `if ( v6 == 0 )`
  - `switch ( v1 )`
  - `return CMiniRoomBaseDlg::OnEnterBase(a1);`
  - `return (*(*v6 + 80))(a1: v6, a2: a1);`
  - …（截断 16 行）

#### CMiniRoomBaseDlg::SendCashInviteResult  (1789 B)

  - `static int __cdecl CMiniRoomBaseDlg::SendCashInviteResult(int a1, int a2)`
  - `v2 = sub_9ACF95();`
  - `if ( v2 >= 0 )`
  - `COutPacket::COutPacket(a1: 123);`
  - `if ( a2 != 0 )`
  - `COutPacket::Encode1(this: &v5, a2: 3);`
  - `COutPacket::Encode4(this: &v5, a2: a1);`
  - `COutPacket::Encode1(this: &v5, a2);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v5);`
  - `COutPacket::Encode1(this: &v7, a2: 14);`
  - `COutPacket::Encode1(this: &v7, a2: 4);`
  - `COutPacket::Encode1(this: &v7, a2: 6);`
  - `COutPacket::Encode4(this: &v7, a2: v2);`
  - `COutPacket::Encode4(this: &v7, a2: a1);`
  - `COutPacket::Encode1(this: &v7, a2: 0);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v7);`
  - …（截断 16 行）

#### CMiniRoomBaseDlg::SendInviteResult  (1139 B)

  - `static int __cdecl CMiniRoomBaseDlg::SendInviteResult(int a1, int a2)`
  - `COutPacket::COutPacket(a1: 123);`
  - `if ( a2 != 0 )`
  - `COutPacket::Encode1(this: &v4, a2: 3);`
  - `COutPacket::Encode4(this: &v4, a2: a1);`
  - `COutPacket::Encode1(this: &v4, a2);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v4);`
  - `COutPacket::Encode1(this: &v6, a2: 4);`
  - `COutPacket::Encode4(this: &v6, a2: a1);`
  - `COutPacket::Encode1(this: &v6, a2: 0);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v6);`
  - `return ZArray<unsigned char>::RemoveAll(a1: v2);`

### COmokDlg（1 函数）


#### COmokDlg::OnKey  (1232 B)

  - `void __thiscall COmokDlg::OnKey(COmokDlg *this, unsigned int a2, signed int a3)`
  - `if ( a3 >= 0 && a2 == 13 )`
  - `v4 = *(this + 114);`
  - `if ( v4 != 0 )`
  - `v5 = *CCtrlEdit::GetText(this: v4, a2: &a3);`
  - `if ( !v6 )`
  - `CCtrlEdit::GetText(this: *(this + 114), &a2);`
  - `v7 = *(this + 114);`
  - `CCtrlEdit::SetText(this: v7, a2: 0);`
  - `ZXString<char>::operator=(a1: &a2);`
  - `CMiniRoomBaseDlg::CheckAndSendChat(this: this - 1, a2: v8[0]);`
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: *(this + 114) != 0 ? (*(this + 114) + 4) : 0);`
  - `else if ( a2 != 27 )`
  - `CDialog::OnKey(this, a2, a3);`

### CMemoryGameDlg（1 函数）


#### CMemoryGameDlg::OnKey  (1244 B)

  - `void __thiscall CMemoryGameDlg::OnKey(CMemoryGameDlg *this, unsigned int a2, signed int a3)`
  - `if ( a3 >= 0 && a2 == 13 )`
  - `v4 = *(this + 114);`
  - `if ( v4 != 0 )`
  - `v5 = *CCtrlEdit::GetText(this: v4, a2: &a3);`
  - `if ( !v6 )`
  - `CCtrlEdit::GetText(this: *(this + 114), &a2);`
  - `v7 = *(this + 114);`
  - `CCtrlEdit::SetText(this: v7, a2: 0);`
  - `ZXString<char>::operator=(a1: &a2);`
  - `CMiniRoomBaseDlg::CheckAndSendChat(this: this - 1, a2: v8[0]);`
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: *(this + 114) != 0 ? (*(this + 114) + 4) : 0);`
  - `else if ( a2 != 27 )`
  - `CDialog::OnKey(this, a2, a3);`

### CRPSGameDlg（1 函数）


#### CRPSGameDlg::OnPacket  (4229 B)

- SP_ 节点：`SP_3675_EBROCK_PAPER_SCISSORS_NOTICENK_R_NTHE_PARTICIPATION_FEE_FOR_THIS_ROCK_PAPER_SCIS`
  - `void __cdecl CRPSGameDlg::OnPacket(int a1)`
  - `v1 = CInPacket::Decode1(this: a1);`
  - `if ( v1 < 6u )`
  - `if ( v1 <= 7u )`
  - `if ( v1 == 8 )`
  - `if ( dword_BE7910 != 0 )`
  - `_CxxThrowException(a1: &v19, a2: &_TI3_AVCDisconnectException__);`
  - `v3 = CInPacket::Decode4(this: a1);`
  - `v5 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v5 != 0 )`
  - `v6 = CUtilDlgEx::CUtilDlgEx(this: v5);`
  - `v15 = v7 != 0 ? (v7 - 8) : 0;`
  - `LOBYTE(v21) = 1;`
  - `sub_4289CD(a1: v14);`
  - `LOBYTE(v21) = 3;`
  - `sub_428211(&a1);`
  - …（截断 16 行）

### CField_Battlefield（8 函数）


#### CField_Battlefield::DecodeFieldSpecificData  (248 B)

  - `void __thiscall CField_Battlefield::DecodeFieldSpecificData(_DWORD *this, struct CUser *a2, int a3)`
  - `v4 = CInPacket::Decode1(this: a3);`
  - `CField_Battlefield::SetUserTeam(this, a2, a3: v4);`

#### CField_Battlefield::DrawScore  (338 B)

  - `int __thiscall CField_Battlefield::DrawScore(_DWORD *this)`
  - `result = *(this + 509);`
  - `if ( result != 0 )`
  - `v2 = *(this + 506);`
  - `*(result + 216) = *(this + 507);`
  - `*(result + 220) = v2;`

#### CField_Battlefield::Init  (162 B)

  - `int __thiscall CField_Battlefield::Init(_BYTE *this, int a2)`
  - `CField::Init(this, a2);`
  - `return CField_Battlefield::DrawScore(this);`

#### CField_Battlefield::OnClock  (1203 B)

  - `int __thiscall CField_Battlefield::OnClock(CClock **this, int a2)`
  - `result = CInPacket::Decode1(this: a2);`
  - `if ( result == 2 )`
  - `v4 = *(this + 115);`
  - `if ( v4 != 0 )`
  - `CWnd::Destroy(a1: v4);`
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 224) != 0 )`
  - `v5 = CScoreboard_Battlefield::CScoreboard_Battlefield();`
  - `v6 = ZRef<CScoreboard_Battlefield>::operator=(a1: v5);`
  - `ZRef<CClock>::operator=(a1: *(v6 + 4));`
  - `CWnd::CreateWnd(`
  - `this: *(this + 509),`
  - `v7 = *(this + 509);`
  - `v8 = CInPacket::Decode4(this: a2);`
  - `CClock::SetTimer(this: v7, a2: v8);`
  - `CClock::Start(this: *(this + 509));`
  - …（截断 16 行）

#### CField_Battlefield::OnPacket  (357 B)

  - `void __thiscall CField_Battlefield::OnPacket(char *this, signed int a2, wchar_t *a3)`
  - `if ( a2 == 299 )`
  - `CField_Battlefield::OnScoreUpdate(a1: a3);`
  - `else if ( a2 == 300 )`
  - `CField_Battlefield::OnTeamChanged(a1: a3);`
  - `CField::OnPacket(this, a2, a3);`

#### CField_Battlefield::OnScoreUpdate  (375 B)

  - `int __thiscall CField_Battlefield::OnScoreUpdate(CField_Battlefield *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `v4 = CInPacket::Decode1(this: a2);`
  - `*(this + 506) = v3;`
  - `*(this + 507) = v4;`
  - `return CField_Battlefield::DrawScore(this);`

#### CField_Battlefield::OnTeamChanged  (471 B)

  - `void __thiscall CField_Battlefield::OnTeamChanged(CField_Battlefield *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode4(this: a2);`
  - `User = CUserPool::GetUser(this: dword_BEBFA8, a2: v3);`
  - `if ( User != 0 )`
  - `v5 = CInPacket::Decode1(this: a2);`
  - `CField_Battlefield::SetUserTeam(this, a2: User, a3: v5);`

#### CField_Battlefield::SetUserTeam  (426 B)

  - `void __thiscall CField_Battlefield::SetUserTeam(_DWORD *this, struct CUser *a2, int a3)`
  - `if ( a2 != 0 )`
  - `if ( (*(*a2 + 12))(a1: a2) != 0 && (a3 == 0 || a3 == 2) )`
  - `CUIMiniMap::SetShowMiniMap(a1: 0);`
  - `CUIMiniMap::ToggleMap(a1: dword_BED788, a2: this);`
  - `*(a2 + 2742) = a3;`
  - `CField::ApplyUserLook(this, a2);`

### CField_Coconut（6 函数）


#### CField_Coconut::BasicActionAttack  (2297 B)

  - `signed int __thiscall CField_Coconut::BasicActionAttack(_DWORD *this)`
  - `if ( TSingleton<CUserLocal>::ms_pInstance == 0 )`
  - `v8 = *(this + 511);`
  - `sub_549F70(a1: v8, a2: &v18);`
  - `v3 = *(this + 511) <= 0;`
  - `if ( !v3 )`
  - `v9 = *CField_Coconut::GetObjectName(a1: v14, a2: v16);`
  - `v4 = (v15 + v17);`
  - `LOBYTE(v19) = 1;`
  - `ObjectRect = CMapLoadable::GetObjectRect(a1: v10, a2: v9);`
  - `LOBYTE(v19) = 0;`
  - `while ( v16 < *(this + 511) );`
  - `v6 = CUserLocal::TryDoingNormalAttack(`
  - `if ( v6 >= 0 )`
  - `COutPacket::COutPacket(a1: 213);`
  - `LOBYTE(v19) = 2;`
  - …（截断 16 行）

#### CField_Coconut::DrawBoard  (10450 B)

- SP_ 节点：`SP_2774_03D`, `SP_2781_MAP_OBJ_ETCIMG_COCONUT_FONTSCORE`, `SP_2782_MAP_OBJ_ETCIMG_COCONUT_FONTTIME`, `SP_2784_02D02D`, `SP_37_ARCH_MAGEFIREPOISON`, `SP_83_THERE_IS_CURRENTLY_NOT_ENOUGH_AVAILABLE_MEMORY_TO_RUN_MAPLESTORY_R_NPLEASE_VISIT`
  - `_DWORD *__thiscall CField_Coconut::DrawBoard(_DWORD *this)`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `v2 = *(this + 725) == 0;`
  - `if ( v2 )`
  - `IWzGr2DLayer::GetCanvas(a1: &v67, a2: &v56);`
  - `LOBYTE(v70) = 2;`
  - `if ( v3 < 0 )`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `v69 = *(this + 724);`
  - `LOBYTE(v70) = 3;`
  - `if ( v67 == 0 )`
  - `v5 = (*(v4 + 128))(`
  - `if ( v5 < 0 )`
  - `if ( v6 < 0 )`
  - `LOBYTE(v70) = 4;`
  - `sub_402F85(pvargDest: &v57, pvargSrc: &pvargSrc);`
  - …（截断 16 行）

#### CField_Coconut::GetObjectName  (874 B)

- SP_ 节点：`SP_2774_03D`
  - `_DWORD *__cdecl CField_Coconut::GetObjectName(_DWORD *a1, char a2)`
  - `LOBYTE(v9) = 2;`
  - `v4 = ZXString<char>::Format(a1: &v8, a2: v3, a3: a2);`
  - `ZXString<char>::operator=(a1: v4);`
  - `LOBYTE(v9) = 1;`
  - `LOBYTE(v9) = 0;`

#### CField_Coconut::Init  (23907 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_2776_COCONUT`, `SP_2777_COUNTFALLING`, `SP_2778_COUNTBOMBING`, `SP_2779_COUNTSTOPPED`, `SP_2780_MAP_OBJ_ETCIMG_COCONUT_BACKGRND`, `SP_956_CANVAS`
  - `int __thiscall CField_Coconut::Init(_BYTE *this, VARIANTARG **pParam)`
  - `CField::Init(this, a2: pParam);`
  - `sub_40263B(a1: this + 48);`
  - `IWzProperty::GetItem(pvargDest: &v115, a2: v110);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v111, a2: v112);`
  - `if ( Unknown != 0 )`
  - `v5 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v5 < 0 && v5 != -2147467262 )`
  - `LOBYTE(v133) = 3;`
  - `if ( v6 < 0 )`
  - `sub_40263B(a1: &v129);`
  - `pvarSrc = IWzProperty::GetItem(pvargDest: v110, a2: v111);`
  - `LOBYTE(v133) = 5;`
  - `Item = IWzProperty::GetItem(pvargDest: &v122, a2: v110);`
  - `LOBYTE(v133) = 7;`
  - `v14 = IWzProperty::GetItem(pvargDest: &v125, a2: v109);`
  - …（截断 16 行）

#### CField_Coconut::OnCoconutScore  (259 B)

  - `_DWORD *__thiscall CField_Coconut::OnCoconutScore(_DWORD *this, CInPacket *a2)`
  - `*(this + 512) = CInPacket::Decode2(this: a2);`
  - `*(this + 513) = CInPacket::Decode2(this: a2);`
  - `return CField_Coconut::DrawBoard(this);`

#### CField_Coconut::Update  (1292 B)

  - `_DWORD *__thiscall CField_Coconut::Update(int *this)`
  - `CField::Update(this);`
  - `result = get_update_time();`
  - `v3 = result - *(this + 515);`
  - `if ( v3 > 150 )`
  - `*(this + 515) = result;`
  - `result = CField_Coconut::DrawBoard(this);`
  - `v4 = *(this + 509);`
  - `while ( v4 != 0 )`
  - `v6 = *(v4 != 0 ? v4 - 16 + 4 : 4);`
  - `result = (*(v4 + 8) - v11);`
  - `if ( result <= 0 )`
  - `ObjectName = CField_Coconut::GetObjectName(a1: v10, a2: *v5);`
  - `CMapLoadable::SetObjectState(a1: v8, a2: v9);`
  - `result = sub_549FEB(a1: v5);`

### CField_ContiMove（4 函数）


#### CField_ContiMove::OnContiMove  (1228 B)

  - `char __thiscall CField_ContiMove::OnContiMove(CField_ContiMove *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode1(this: a2) - 7;`
  - `if ( v3 != 0 )`
  - `if ( v4 != 0 )`
  - `if ( v5 != 0 )`
  - `if ( v6 != 0 )`
  - `if ( v7 != 0 )`
  - `if ( v8 == 0 )`
  - `LOBYTE(v8) = CField_ContiMove::OnEndShipMoveField(this, a2);`
  - `LOBYTE(v8) = nullsub_7(a1: a2);`
  - `LOBYTE(v8) = sub_54DDB5(a1: a2);`
  - `LOBYTE(v8) = nullsub_6(a1: a2);`
  - `LOBYTE(v8) = sub_54DD90(a1: a2);`
  - `LOBYTE(v8) = nullsub_5(a1: a2);`

#### CField_ContiMove::OnContiState  (896 B)

  - `int __thiscall CField_ContiMove::OnContiState(_DWORD *this, int a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `result = CInPacket::Decode1(this: a2);`
  - `if ( v3 <= 1 )`
  - `if ( *(this + 509) == 0 )`
  - `return CShip::EnterShipMove(this: this + 506);`
  - `if ( v3 != 2 )`
  - `if ( v3 <= 4 )`
  - `if ( *(this + 509) == 1 && result == 1 )`
  - `result = CShip::AppearShip(a1: this + 506);`
  - `else if ( v3 != 5 )`
  - `if ( v3 != 6 )`
  - `return CShip::LeaveShipMove(a1: this + 506);`

#### CField_ContiMove::OnEndShipMoveField  (282 B)

  - `char __thiscall CField_ContiMove::OnEndShipMoveField(_DWORD *this, int a2)`
  - `result = CInPacket::Decode1(this: a2);`
  - `if ( result == 6 )`
  - `return CShip::EnterShipMove(this: this + 506);`

#### CField_ContiMove::OnPacket  (364 B)

  - `void __thiscall CField_ContiMove::OnPacket(char *this, signed int a2, wchar_t *a3)`
  - `if ( a2 == 148 )`
  - `CField_ContiMove::OnContiMove(a1: a3);`
  - `else if ( a2 == 149 )`
  - `CField_ContiMove::OnContiState(this: this - 2, a2: a3);`
  - `CField::OnPacket(this, a2, a3);`

### CField_Dojang（4 函数）


#### CField_Dojang::DrawDigit  (3304 B)

  - `int *__thiscall CField_Dojang::DrawDigit(CField_Dojang *this, int a2, int a3, unsigned int a4)`
  - `if ( *(this + 518) != 0 )`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `v6 = *(this + 518) == 0;`
  - `if ( v6 )`
  - `IWzGr2DLayer::GetCanvas(a1: &v19, a2: &v15);`
  - `LOBYTE(v20) = 2;`
  - `if ( result < 0 )`
  - `if ( v19 != 0 )`
  - `VariantInit(&pvarg);`
  - `v7 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v7 < 0 )`
  - `LOBYTE(v20) = 3;`
  - `v18 = *(this + a4 / 0xA % 0xA + 519);`
  - `if ( v19 == 0 )`
  - `v9 = (*(v8 + 128))(`
  - …（截断 16 行）

#### CField_Dojang::OnClock  (8822 B)

- SP_ 节点：`SP_4552_UI_UIWINDOWIMG_MURUENGRAID_NUMBER_BAR`, `SP_956_CANVAS`
  - `int __thiscall CField_Dojang::OnClock(CField_Dojang *this, struct CInPacket *a2)`
  - `result = CInPacket::Decode1(this: a2);`
  - `if ( result == 2 )`
  - `*(this + 530) = CInPacket::Decode4(this: a2);`
  - `*(this + 529) = 1000 * *(this + 530) + timeGetTime();`
  - `LOBYTE(v43) = 1;`
  - `PcCreateObject::IWzCanvas(a1: v5, &a2, a3: 0);`
  - `LOBYTE(v43) = 0;`
  - `if ( Unknown != 0 )`
  - `if ( a2 != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v43) = 2;`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v43) = 3;`
  - `if ( a2 == 0 )`
  - `v7 = (*(v6 + 44))(`
  - …（截断 16 行）

#### CField_Dojang::Update  (19875 B)

  - `void __thiscall CField_Dojang::Update(CField_Dojang *this)`
  - `CField::Update(this);`
  - `v73 = sub_679025(a1: dword_BEBFA4);`
  - `if ( v73 != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `v2 = *(this + 512) == 0;`
  - `LOBYTE(v77) = 1;`
  - `if ( v2 )`
  - `Alpha = IWzGr2DLayer::GetAlpha(a1: &v75);`
  - `LOBYTE(v77) = 2;`
  - `v5 = (*(v4 + 144))(`
  - `if ( v5 < 0 )`
  - `sub_41569E(a1: &v75);`
  - `LOBYTE(v77) = 0;`
  - `if ( v6 < 0 )`
  - …（截断 16 行）

#### CField_Dojang::UpdateTimer  (1421 B)

- SP_ 节点：`SP_1000_BLAZE`, `SP_2501_DOJANG_TIMEOVER`, `SP_2558_DOJANG_TIMEOVER`
  - `void __thiscall CField_Dojang::UpdateTimer(CField_Dojang *this)`
  - `if ( *(this + 529) != 0 )`
  - `v2 = (*(this + 529) - timeGetTime()) / 1000;`
  - `if ( v2 <= 0 )`
  - `*(this + 529) = 0;`
  - `CField::ShowScreenEffect(a1: v6[0]);`
  - `play_field_sound(a1: v5, a2: 0x64u);`
  - `if ( *(this + 530) != v2 )`
  - `*(this + 530) = v2;`
  - `CField_Dojang::DrawDigit(this, a2: 0, a3: 0, a4: v2 / 60);`
  - `CField_Dojang::DrawDigit(this, a2: 68, a3: 0, a4: *(this + 530) % 60);`

### CField_Massacre（1 函数）


#### CField_Massacre::Init  (33771 B)

- SP_ 节点：`SP_5371_UI_UIWINDOWIMG_MONSTERKILLING_COUNT_BACKGRD0`, `SP_5372_UI_UIWINDOWIMG_MONSTERKILLING_COUNT_BACKGRD1`, `SP_5376_UI_UIWINDOWIMG_MONSTERKILLING_COUNT_NUMBER`, `SP_5377_UI_UIWINDOWIMG_MONSTERKILLING_COUNT_NUMBER2`, `SP_5378_UI_UIWINDOWIMG_MONSTERKILLING_GAUGE_BACKGRD`, `SP_5379_UI_UIWINDOWIMG_MONSTERKILLING_GAUGE_BACKGRDD`, `SP_5380_UI_UIWINDOWIMG_MONSTERKILLING_GAUGE_DANGER`, `SP_5381_UI_UIWINDOWIMG_MONSTERKILLING_GAUGE_ICOND`, `SP_5382_UI_UIWINDOWIMG_MONSTERKILLING_GAUGE_PIXEL`, `SP_5383_UI_UIWINDOWIMG_MONSTERKILLING_GAUGE_TEXT`, `SP_5384_UI_UIWINDOWIMG_MONSTERKILLING_GAUGE_TEXTD`
  - `_DWORD *__thiscall CField_Massacre::Init(_BYTE *this, VARIANTARG *pParam)`
  - `CField::Init(this, a2: pParam);`
  - `HIDWORD(v149) = v3;`
  - `_bstr_t::_bstr_t(a1: str_Mo);`
  - `sub_40263B(a1: this + 48);`
  - `IWzProperty::GetItem(pvargDest: &v153, a2: SHIDWORD(v149));`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v150, a2: v151);`
  - `if ( Unknown != 0 )`
  - `v4 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v4 < 0 && v4 != -2147467262 )`
  - `LOBYTE(v168) = 3;`
  - `if ( v5 < 0 )`
  - `_bstr_t::_bstr_t(a1: str_Di);`
  - `HIDWORD(v149) = &v159;`
  - `sub_40263B(a1: &v160);`
  - `Item = IWzProperty::GetItem(pvargDest: HIDWORD(v149), a2: v150);`
  - …（截断 16 行）

### CField_MassacreResult（2 函数）


#### CField_MassacreResult::OnMassacreResult  (18115 B)

- 分支 case：`1`, `2`, `3`, `4`
- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_5385_UI_UIWINDOWIMG_MONSTERKILLING_RESULT_BACKGRD`, `SP_5386_UI_UIWINDOWIMG_MONSTERKILLING_RESULT_BACKGRD2`, `SP_5389_UI_UIWINDOWIMG_MONSTERKILLING_RESULT_RANK_A`, `SP_5390_UI_UIWINDOWIMG_MONSTERKILLING_RESULT_RANK_B`, `SP_5391_UI_UIWINDOWIMG_MONSTERKILLING_RESULT_RANK_C`, `SP_5392_UI_UIWINDOWIMG_MONSTERKILLING_RESULT_RANK_D`, `SP_5393_UI_UIWINDOWIMG_MONSTERKILLING_RESULT_RANK_S`
  - `void __thiscall CField_MassacreResult::OnMassacreResult(char *this, char **iPacket)`
  - `v88 = CInPacket::Decode1(this: iPacket);`
  - `v87 = CInPacket::Decode4(this: iPacket);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `HIDWORD(v73[0]) = &pvargDest;`
  - `LODWORD(v73[0]) = &pvarg;`
  - `HIDWORD(v72) = v2;`
  - `LOBYTE(v93) = 1;`
  - `v4 = sub_406472();`
  - `v5 = sub_402680(a1: v4);`
  - `ObjectA = IWzResMan::GetObjectA(`
  - `sUOL: SHIDWORD(v72),`
  - `vAux: SHIDWORD(v73[0]));`
  - `LOBYTE(v93) = 3;`
  - `unknown = get_unknown(a1: &v86, a2: ObjectA);`
  - …（截断 16 行）

#### CField_MassacreResult::OnPacket  (287 B)

  - `void __thiscall CField_MassacreResult::OnPacket(char *this, signed int nType, char **iPacket)`
  - `if ( nType == 158 )`
  - `CField_MassacreResult::OnMassacreResult(this: this - 8, iPacket);`
  - `CField::OnPacket(this, a2: nType, a3: iPacket);`

### CField_MonsterCarnival（9 函数）


#### CField_MonsterCarnival::CreateUIWindow  (2869 B)

  - `void __thiscall CField_MonsterCarnival::CreateUIWindow(CField_MonsterCarnival *this)`
  - `if ( dword_BED7DC == 0 )`
  - `TSingleton<CUIMonsterCarnival>::CreateInstance();`
  - `v3 = (this + 2024);`
  - `while ( *v3 != 0 && v1 < *(*v3 - 4) )`
  - `v4 = sub_565F4D(a1: v1);`
  - `ZXString<char>::operator=(a1: v4 + 12);`
  - `v18 = *(sub_565F4D(a1: v1) + 8);`
  - `v5 = sub_565F4D(a1: v1);`
  - `sub_428211(a1: v5 + 4);`
  - `sub_565E7C();`
  - `CUIMonsterCarnival::SetUIData(a1: 0, a2: v1, a3: v17, a4: v18, a5: v19[0]);`
  - `v8 = (v22 + 2028);`
  - `while ( *v8 != 0 && v7 < *(*v8 - 4) )`
  - `v9 = sub_565F4D(a1: v7);`
  - `ZXString<char>::operator=(a1: v9 + 12);`
  - …（截断 16 行）

#### CField_MonsterCarnival::OnEnter  (2483 B)

  - `int __thiscall CField_MonsterCarnival::OnEnter(CField_MonsterCarnival *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `i = CInPacket::Decode2(this: v2);`
  - `v14 = CInPacket::Decode2(this: v2);`
  - `v13 = CInPacket::Decode2(this: v2);`
  - `v17 = CInPacket::Decode2(this: v2);`
  - `v11 = CInPacket::Decode2(this: v2);`
  - `v12 = CInPacket::Decode2(this: v2);`
  - `CField_MonsterCarnival::CreateUIWindow(this: v15);`
  - `sub_870889(a1: v3);`
  - `if ( TSingleton<CUserLocal>::ms_pInstance != 0 && dword_BED7DC != 0 )`
  - `*(TSingleton<CUserLocal>::ms_pInstance + 7828) = v3;`
  - `CUserLocal::RedrawGuildNameTag(this: v5);`
  - `sub_87083E(a1: i, a2: v14);`
  - `sub_87085C(a1: v3, a2: v13, a3: v17);`
  - `sub_87085C(a1: v3 == 0, a2: v11, a3: v12);`
  - …（截断 16 行）

#### CField_MonsterCarnival::OnPacket  (1239 B)

- 分支 case：`289`, `290`, `291`, `292`, `293`, `294`, `295`, `296`
  - `void __thiscall CField_MonsterCarnival::OnPacket(char *this, signed int a2, wchar_t *a3)`
  - `switch ( a2 )`
  - `CField_MonsterCarnival::OnEnter(a1: a3);`
  - `CField_MonsterCarnival::OnPersonalCP(a1: a3);`
  - `CField_MonsterCarnival::OnTeamCP(a1: a3);`
  - `CField_MonsterCarnival::OnRequestResult(a1: this - 8, a2, a3: 1, a4: a3);`
  - `CField_MonsterCarnival::OnRequestResult(a1: this - 8, a2, a3: 0, a4: a3);`
  - `CField_MonsterCarnival::OnProcessForDeath(a1: a3);`
  - `CField_MonsterCarnival::OnShowMemberOutMsg(a1: a3);`
  - `CField_MonsterCarnival::OnShowGameResult(a1: this - 8, a2, a3);`
  - `CField::OnPacket(this, a2, a3);`

#### CField_MonsterCarnival::OnPersonalCP  (376 B)

  - `int __thiscall CField_MonsterCarnival::OnPersonalCP(CField_MonsterCarnival *this, struct CInPacket *a2)`
  - `v2 = CInPacket::Decode2(this: a2);`
  - `result = CInPacket::Decode2(this: a2);`
  - `if ( dword_BED7DC != 0 )`
  - `return sub_87083E(a1: v2, a2: result);`

#### CField_MonsterCarnival::OnProcessForDeath  (2888 B)

- SP_ 节点：`SP_4078_MAPLE_RED`, `SP_4079_MAPLE_BLUE`, `SP_4080__S__HAS_BECOME_UNABLE_TO_FIGHT_AND__S_TEAM_HAS_LOST_D_CP`, `SP_4081__S__HAS_BECOME_UNABLE_TO_FIGHT_BUT__S__HAS_NO_CP_SO__S__TEAM_DID_NOT_LOSE_ANY_CP`
  - `void __thiscall CField_MonsterCarnival::OnProcessForDeath(CField_MonsterCarnival *this, struct CInPacket *a2)`
  - `v2 = CInPacket::Decode1(this: a2);`
  - `CInPacket::DecodeStr(this: a2, a2: v14);`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `if ( v2 != 0 )`
  - `LOBYTE(v18) = 1;`
  - `ZXString<char>::operator=(a1: String);`
  - `if ( (v17 & 2) != 0 )`
  - `if ( (v17 & 1) != 0 )`
  - `LOBYTE(v18) = 6;`
  - `if ( v3 <= 0 )`
  - `LOBYTE(v18) = 8;`
  - `ZXString<char>::Format(a1: &v15, a2: v11, a3: v14[0]);`
  - `LOBYTE(v18) = 7;`
  - `ZXString<char>::Format(a1: &v15, a2: v8, a3: v14[0]);`
  - `CHATLOG_ADD(sString: &v15, nType: 7u);`
  - …（截断 16 行）

#### CField_MonsterCarnival::OnRequestResult  (5250 B)

- SP_ 节点：`SP_4082_YOU_DONT_HAVE_ENOUGH_CP_TO_CONTINUE`, `SP_4083_YOU_CAN_NO_LONGER_SUMMON_THE_MONSTER`, `SP_4084_YOU_CAN_NO_LONGER_SUMMON_THE_BEING`, `SP_4085_THIS_BEING_IS_ALREADY_SUMMONED`, `SP_4086_THIS_REQUEST_HAS_FAILED_DUE_TO_AN_UNKNOWN_ERROR`
  - `void __thiscall CField_MonsterCarnival::OnRequestResult(CField_MonsterCarnival *this, int a2, char **a3)`
  - `if ( a2 == 0 )`
  - `v8 = CInPacket::Decode1(this: a3);`
  - `if ( v9 != 0 )`
  - `if ( v10 != 0 )`
  - `if ( v11 != 0 )`
  - `if ( v12 != 0 )`
  - `if ( v12 != 1 )`
  - `*(*dword_BE7918 + 8356) = 0;`
  - `update_time = get_update_time();`
  - `*(v19 + 8360) = update_time;`
  - `LOBYTE(v29) = 8;`
  - `ZXString<char>::Format(a1: &a3, a2: v21, a3: v22);`
  - `LOBYTE(v29) = 3;`
  - `if ( a3 != 0 && *a3 != 0 )`
  - `CHATLOG_ADD(sString: &a3, nType: 7u);`
  - …（截断 16 行）

#### CField_MonsterCarnival::OnShowGameResult  (3138 B)

- SP_ 节点：`SP_4087_YOU_HAVE_WON_THE_MONSTER_CARNIVAL_PLEASE_WAIT_AS_YOULL_BE_TRANSPORTED_OUT_OF_HER`, `SP_4088_UNFORTUNATELY_YOU_HAVE_LOST_THE_MONSTER_CARNIVAL_PLEASE_WAIT_AS_YOULL_BE_TRANSPO`, `SP_4089_DESPITE_THE_OVERTIME_THE_CARNIVAL_ENDED_IN_A_DRAW_PLEASE_WAIT_AS_YOULL_BE_TRANSP`, `SP_4090_MONSTER_CARNIVAL_HAS_ENDED_ABRUPTLY_DUE_TO_THE_OPPOSING_TEAM_LEAVING_THE_GAME_TO`
  - `void __thiscall CField_MonsterCarnival::OnShowGameResult(CField_MonsterCarnival *this, struct CInPacket *a2)`
  - `v2 = CInPacket::Decode1(this: a2);`
  - `if ( v3 != 0 )`
  - `if ( v4 != 0 )`
  - `if ( v5 != 0 )`
  - `if ( v5 != 1 )`
  - `LOBYTE(v20) = 4;`
  - `ZXString<char>::Format(a1: &a2, a2: v11, a3: v15);`
  - `LOBYTE(v20) = 0;`
  - `LOBYTE(v20) = 3;`
  - `ZXString<char>::Format(a1: &a2, a2: v12, a3: v15);`
  - `LOBYTE(v20) = 2;`
  - `ZXString<char>::Format(a1: &a2, a2: v13, a3: v15);`
  - `LOBYTE(v20) = 1;`
  - `ZXString<char>::Format(a1: &a2, a2: v14, a3: v15);`
  - `if ( a2 != 0 && *a2 != 0 )`
  - …（截断 16 行）

#### CField_MonsterCarnival::OnShowMemberOutMsg  (2477 B)

- SP_ 节点：`SP_4078_MAPLE_RED`, `SP_4079_MAPLE_BLUE`, `SP_4096__S__OF_TEAM__S__HAS_QUIT_THE_MONSTER_CARNIVAL`, `SP_4097_SINCE_THE_LEADER_OF_THE_TEAM__S__QUIT_THE_MONSTER_CARNIVAL__S__HAS_BEEN_APPOINTE`
  - `void __thiscall CField_MonsterCarnival::OnShowMemberOutMsg(CField_MonsterCarnival *this, struct CInPacket *a2)`
  - `v2 = CInPacket::Decode1(this: a2) == 6;`
  - `if ( CInPacket::Decode1(this: a2) != 0 )`
  - `ZXString<char>::operator=(a1: String);`
  - `if ( (v13 & 2) != 0 )`
  - `if ( (v13 & 1) != 0 )`
  - `CInPacket::DecodeStr(this: a2, a2: &v10);`
  - `LOBYTE(v14) = 6;`
  - `if ( v2 )`
  - `LOBYTE(v14) = 7;`
  - `LOBYTE(v14) = 8;`
  - `ZXString<char>::Format(a1: &v12, a2: v7, a3: v11[0]);`
  - `CHATLOG_ADD(sString: &v12, nType: 0xCu);`
  - `LOBYTE(v14) = 5;`
  - `LOBYTE(v14) = 3;`

#### CField_MonsterCarnival::OnTeamCP  (447 B)

  - `int __thiscall CField_MonsterCarnival::OnTeamCP(CField_MonsterCarnival *this, struct CInPacket *a2)`
  - `v2 = CInPacket::Decode1(this: a2);`
  - `v3 = CInPacket::Decode2(this: a2);`
  - `result = CInPacket::Decode2(this: a2);`
  - `if ( dword_BED7DC != 0 )`
  - `return sub_87085C(a1: v2, a2: v3, a3: result);`

### CField_SnowBall（1 函数）


#### CField_SnowBall::Update  (4600 B)

  - `int __thiscall CField_SnowBall::Update(CField_SnowBall *this)`
  - `if ( TSingleton<CUserLocal>::ms_pInstance != 0 )`
  - `v3 = (*(*(TSingleton<CUserLocal>::ms_pInstance + 4) + 16))(a1: TSingleton<CUserLocal>::ms_pInstance + 4);`
  - `while ( dword_BF0484(a1: v4, a2: *v3, a3: v3[1]) == 0 )`
  - `if ( v28 > 1 )`
  - `if ( *(this + 506) != v28 + 2 && CUserLocal::IsStun(this: v2) == 0 )`
  - `COutPacket::COutPacket(a1: 212);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: v19);`
  - `ZArray<unsigned char>::RemoveAll(a1: v20);`
  - `while ( dword_BF0484(a1: v5, a2: *v27, a3: v27[1]) == 0 )`
  - `v7 = CRand32::Random(a1: dword_BEDD38);`
  - `(*(v6 + 72))(a1: v2, a2: v7 % 0xB + 30, a3: 270, a4: -270, a5: 0, a6: 0, a7: 0, a8: 0, a9: 0, a10: 1, a11: 1);`
  - `update_time = get_update_time();`
  - `v8 = *(this + 561);`
  - `if ( v8 != 0 )`
  - `while ( 1 )`
  - …（截断 16 行）

### CField_SpaceGAGA（1 函数）


#### CField_SpaceGAGA::OnClock  (1138 B)

  - `void __thiscall CField_SpaceGAGA::OnClock(void *this, int a2)`
  - `if ( CInPacket::Decode1(this: a2) == 2 )`
  - `v3 = *(this + 115);`
  - `if ( v3 != 0 )`
  - `CWnd::Destroy(a1: v3);`
  - `v4 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 208);`
  - `if ( v4 != 0 )`
  - `v5 = CTimerboard_SpaceGAGA::CTimerboard_SpaceGAGA(this: v4);`
  - `v6 = ZRef<CTimerboard_SpaceGAGA>::operator=(a1: v5);`
  - `ZRef<CClock>::operator=(a1: *(v6 + 4));`
  - `CWnd::CreateWnd(`
  - `this: *(this + 507),`
  - `v7 = *(this + 507);`
  - `v8 = CInPacket::Decode4(this: a2);`
  - `CClock::SetTimer(this: v7, a2: v8);`
  - `CClock::Start(this: *(this + 507));`
  - …（截断 16 行）

### CField_Witchtower（1 函数）


#### CField_Witchtower::Close  (234 B)

  - `int __thiscall CField_Witchtower::Close(CField_Witchtower *this)`
  - `v2 = *(this + 507);`
  - `if ( v2 != 0 )`
  - `CWnd::Destroy(a1: v2);`
  - `return CField::Close(this);`

### CField_LimitedView（3 函数）


#### CField_LimitedView::DrawViewRange  (8291 B)

  - `int *__thiscall CField_LimitedView::DrawViewRange(CField_LimitedView *this)`
  - `result = *(dword_BEBFA8 + 8);`
  - `if ( result != 0 )`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `v3 = *(v2 + 506) == 0;`
  - `if ( v3 )`
  - `IWzGr2DLayer::GetCanvas(a1: &v52, a2: &pvarg);`
  - `LOBYTE(v55) = 2;`
  - `if ( v4 < 0 )`
  - `v5 = *(v2 + 513);`
  - `if ( v5 != 0 )`
  - `while ( 1 )`
  - `v6 = *(v5 != 0 ? v5 - 3 : 4);`
  - `v51 = v6 != 0 ? (v6 + 16) : 0;`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `v49 = *(v2 + 508);`
  - …（截断 16 行）

#### CField_LimitedView::Init  (11966 B)

- SP_ 节点：`SP_3654_EFFECT_MAPEFFIMG_VIEWRANGE_1`, `SP_956_CANVAS`
  - `void __thiscall CField_LimitedView::Init(CField_LimitedView *this, volatile LONG *a2)`
  - `CField::Init(this, a2);`
  - `v3 = (this + 2028);`
  - `sub_4051E5(a1: 0);`
  - `PcCreateObject::IWzCanvas(a1: v5, a2: v3, a3: 0);`
  - `if ( a2 != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v68) = 2;`
  - `if ( v6 )`
  - `v9 = (*(v8 + 44))(`
  - `if ( v9 < 0 )`
  - `LOBYTE(v68) = 1;`
  - `if ( v10 < 0 )`
  - `if ( v11 < 0 )`
  - `v6 = *(v67 + 507) == 0;`
  - …（截断 16 行）

#### CField_LimitedView::Update  (165 B)

  - `void __thiscall CField_LimitedView::Update(CField_LimitedView *this)`
  - `CField::Update(this);`
  - `CField_LimitedView::DrawViewRange(this);`

### CField_PartyRaid（1 函数）


#### CField_PartyRaid::Init  (16985 B)

  - `void __thiscall CField_PartyRaid::Init(CField_PartyRaid *this, int a2)`
  - `CField::Init(this, a2);`
  - `v89 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 72);`
  - `if ( v89 != 0 )`
  - `sub_402F85(pvargDest: &v83, pvargSrc: &pvargSrc);`
  - `LOBYTE(v93) = 1;`
  - `sub_402F85(pvargDest: &v84, pvargSrc: &pvargSrc);`
  - `HIDWORD(v79) = &v83;`
  - `LODWORD(v79) = &v84;`
  - `_bstr_t::_bstr_t(a1: aUiUiwindowImgP);`
  - `v3 = sub_406472();`
  - `v4 = sub_402680(a1: v3);`
  - `IWzResMan::GetObjectA(this: v4, pvargDest: &v82, sUOL: v78, vParam: v79, vAux: SHIDWORD(v79));`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v80, a2: v81);`
  - `HIDWORD(v79) = v6;`
  - `sub_433363(a1: &v90);`
  - …（截断 16 行）

### CField_PartyRaidBoss（1 函数）


#### CField_PartyRaidBoss::Init  (16350 B)

  - `int __thiscall CField_PartyRaidBoss::Init(CField_PartyRaidBoss *this, int *Unknown)`
  - `CField::Init(this, a2: Unknown);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v86) = 1;`
  - `_bstr_t::_bstr_t(a1: aUiUiwindowImgD);`
  - `v3 = sub_406472();`
  - `v4 = sub_402680(a1: v3);`
  - `IWzResMan::GetObjectA(this: v4, pvargDest: &v78, sUOL: v73, vParam: p_pvargDest, vAux: p_pvarg);`
  - `LOBYTE(v86) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v76, a2: v77);`
  - `v5 = sub_41E527(a1: &Unknown);`
  - `if ( v5 < 0 && v5 != -2147467262 )`
  - `if ( v6 < 0 )`
  - `LOBYTE(v86) = 0;`
  - `if ( v7 < 0 )`
  - …（截断 16 行）

### CField_PartyRaidResult（1 函数）


#### CField_PartyRaidResult::Init  (5612 B)

  - `int __thiscall CField_PartyRaidResult::Init(CField_PartyRaidResult *this, int a2)`
  - `CField::Init(this, a2);`
  - `v32[0] = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 72);`
  - `if ( v32[0] != 0 )`
  - `sub_402F85(pvargDest: &v27, pvargSrc: &pvargSrc);`
  - `LOBYTE(v34) = 1;`
  - `sub_402F85(pvargDest: &v28, pvargSrc: &pvargSrc);`
  - `_bstr_t::_bstr_t(a1: aUiUiwindowImgP_2);`
  - `v3 = sub_406472();`
  - `v4 = sub_402680(a1: v3);`
  - `IWzResMan::GetObjectA(this: v4, pvargDest: &v26, sUOL: v20, vParam: p_pvargDest, vAux: p_pvarg);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v23, a2: v24);`
  - `sub_433363(a1: &a2);`
  - `v7 = sub_550D75(a1: p_pvarg, a2: v23, a3: v24);`
  - `sub_551479(a1: v7);`
  - `if ( (v33 & 4) != 0 )`
  - …（截断 16 行）

### CField_GuildBoss（2 函数）


#### CField_GuildBoss::BasicActionAttack  (1647 B)

  - `signed int __thiscall CField_GuildBoss::BasicActionAttack(_DWORD *this)`
  - `if ( TSingleton<CUserLocal>::ms_pInstance != 0 )`
  - `v3 = ZArray<tagRECT>::InsertBefore(a1: -1);`
  - `*v3 = *(this + 517);`
  - `v3[1] = *(this + 518);`
  - `v3[2] = *(this + 519);`
  - `v3[3] = *(this + 520);`
  - `if ( CReactorPool::FindHitReactor(a1: dword_BED774, a2: v4) == 0 )`
  - `if ( CUserLocal::TryDoingNormalAttack(`
  - `sub_5586B1(this: this + 513);`
  - `if ( *(this + 507) == 0 )`
  - `COutPacket::COutPacket(a1: 215);`
  - `LOBYTE(v10) = 1;`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: v6);`
  - `LOBYTE(v10) = 0;`
  - `ZArray<unsigned char>::RemoveAll(a1: v7);`
  - …（截断 16 行）

#### CField_GuildBoss::Init  (7688 B)

- SP_ 节点：`SP_977_X`, `SP_978_Y`
  - `_DWORD *__thiscall CField_GuildBoss::Init(CField_GuildBoss *this, int a2)`
  - `CField::Init(this, a2);`
  - `_bstr_t::_bstr_t(a1: aHealer);`
  - `sub_40263B(a1: this + 48);`
  - `IWzProperty::GetItem(pvargDest: &pvarg, a2: p_pvargDest);`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v36, a2: v37[0]);`
  - `if ( Unknown != 0 )`
  - `v4 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v4 < 0 && v4 != -2147467262 )`
  - `LOBYTE(v47) = 3;`
  - `if ( v5 < 0 )`
  - `sub_40263B(a1: &v46);`
  - `Item = IWzProperty::GetItem(pvargDest: p_pvargDest, a2: v36);`
  - `LOBYTE(v47) = 5;`
  - `int32 = get_int32(pvarSrc: Item, a2: v37[0]);`
  - `if ( v9 < 0 )`
  - …（截断 16 行）

### VCScoreboard_Battlefield（1 函数）


#### VCScoreboard_Battlefield::4?$ZRef  (432 B)

  - `int *__thiscall ZRef<CScoreboard_Battlefield>::operator=(int *this, int a2)`
  - `sub_545E69(a1: v6);`
  - `v4 = *(this + 1);`
  - `*(this + 1) = v7;`
  - `sub_545E53(a1: v6);`

### VCTimerboard_SpaceGAGA（1 函数）


#### VCTimerboard_SpaceGAGA::4?$ZRef  (430 B)

  - `int *__thiscall ZRef<CTimerboard_SpaceGAGA>::operator=(int *this, int a2)`
  - `sub_578BF5(a1: v6);`
  - `v4 = *(this + 1);`
  - `*(this + 1) = v7;`
  - `sub_578BDF(a1: v6);`

### BattlefieldTeamCond（1 函数）


#### BattlefieldTeamCond::Parse  (1910 B)

  - `int __cdecl Field::BattlefieldTeamCond::Parse(int a1)`
  - `if ( a1 == 0 )`
  - `_bstr_t::_bstr_t(a1: aBattlefieldtea);`
  - `LOBYTE(v11) = 0;`
  - `sub_40263B(&a1);`
  - `Item = IWzProperty::GetItem(pvargDest: &pvarg, a2: v6);`
  - `LOBYTE(v11) = 2;`
  - `int32 = get_int32(pvarSrc: Item, a2: v7);`
  - `if ( v4 < 0 )`
  - `if ( int32 == -1 )`
  - `if ( int32 > 2 )`
  - `if ( a1 != 0 )`
  - `(*(*a1 + 8))(a1);`
  - `sub_588FE7(a1: v9);`
  - `if ( v10 != 0 )`
  - `InterlockedIncrement(lpAddend: v10 + 1);`
  - …（截断 16 行）

### CUIMonsterCarnival（16 函数）


#### CUIMonsterCarnival::DrawCPTooltip  (5977 B)

- SP_ 节点：`SP_4128_CP_EARNED_THAT_CAN_BE_USED`, `SP_4129_CP_REMAINING_D___CP_EARNED_D`, `SP_4130_THE_GAME_WILL_BE_DECIDED_BASED_ON_THE_TOTAL_CP_EARNED_BETWEEN_TWO_TEAMS`, `SP_4132_COMPARE_THE_TOTAL_CP_EARNED_BETWEEN_TWO_TEAMS_TO_FIND_THE_WINNER`
  - `int __thiscall CUIMonsterCarnival::DrawCPTooltip(CUIMonsterCarnival *this, int a2, int a3, struct tagPOINT a4)`
  - `dword_BF040C(a1: v32, a2: 9, a3: 35, a4: 139, a5: 45);`
  - `dword_BF040C(a1: v31, a2: 9, a3: 55, a4: 139, a5: 65);`
  - `dword_BF040C(a1: v30, a2: 9, a3: 75, a4: 139, a5: 85);`
  - `if ( dword_BF0484(a1: v32, a2: a4.x, a3: a4.y) != 0 )`
  - `LOBYTE(v37) = 2;`
  - `ZXString<char>::Format(a1: &v35, a2: v28, a3: v29);`
  - `LOBYTE(v37) = 1;`
  - `v28 = *(this + 731);`
  - `v27 = *(this + 730);`
  - `LOBYTE(v37) = 3;`
  - `ZXString<char>::Format(a1: &v36, a2: v8, a3: v27);`
  - `if ( dword_BF0484(a1: v31, a2: a4.x, a3: a4.y) != 0 )`
  - `LOBYTE(v37) = 4;`
  - `v12 = *(this + 736);`
  - `v28 = *(this + v12 + 734);`
  - …（截断 16 行）

#### CUIMonsterCarnival::DrawListTooltip  (748 B)

  - `void __thiscall CUIMonsterCarnival::DrawListTooltip(CUIMonsterCarnival *this, int a2, int a3, struct tagPOINT a4)`
  - `sub_86D35D(a1: &a4.y, a2: a4.x, a3: a4.y);`
  - `if ( a4.y != 0 && *a4.y != 0 )`
  - `v6 = ((*(*(this + 1) + 48))(a1: this + 4) + a3 + 20);`
  - `v5 = (*(*(this + 1) + 44))(a1: this + 4);`
  - `CUIToolTip::SetToolTip_String(this: (this + 1608), a2: v5 + a2 + 20, a3: v6, a4: y);`
  - `*(this + 729) = 8000;`
  - `CUIToolTip::ClearToolTip(this: (this + 1608));`

#### CUIMonsterCarnival::DrawLockTooltip  (1729 B)

- SP_ 节点：`SP_4176_AUTOMATIC_LOCK_DISABLED`, `SP_4177_AUTOMATIC_LOCK_ENABLED`
  - `int __userpurge CUIMonsterCarnival::DrawLockTooltip@<eax>(`
  - `if ( dword_BF0484(a1: this + 1484, a2: a5.x, a3: a5.y) != 0 )`
  - `v6 = *(this + 375) == 0;`
  - `if ( v6 )`
  - `LOBYTE(v15) = 2;`
  - `LOBYTE(v15) = 1;`
  - `ZXString<char>::Format(a1: &a5.y, a2: v12, a3: a2);`
  - `LOBYTE(v15) = 0;`
  - `v11 = ((*(*(this + 1) + 48))(a1: this + 4) + a4 + 20);`
  - `v9 = (*(*(this + 1) + 44))(a1: this + 4);`
  - `CUIToolTip::SetToolTip_String(this: (this + 1608), a2: v9 + a3 + 20, a3: v11, a4: y);`
  - `CUIToolTip::ClearToolTip(this: (this + 1608));`

#### CUIMonsterCarnival::FuncKeyProcess  (443 B)

  - `int __thiscall CUIMonsterCarnival::FuncKeyProcess(CTabList **this, unsigned int a2)`
  - `result = CTabList::GetSelectItemIdx(this: *(this + *(this + 376) + 386), a2);`
  - `if ( result != 0 )`
  - `*(this + 727) = result;`
  - `CUIMonsterCarnival::OnTabAutoOpen(this, a2: 0);`
  - `return CUIMonsterCarnival::RequestSend(this);`

#### CUIMonsterCarnival::OnButtonClicked  (1208 B)

  - `int __thiscall CUIMonsterCarnival::OnButtonClicked(CUIMonsterCarnival *this, unsigned int a2)`
  - `switch ( a2 )`
  - `*(this + 381) = 1;`
  - `*(this + 729) = 8000;`
  - `(*(*(*(this + 378) + 4) + 28))(a1: *(this + 378) + 4, a2: 0);`
  - `(*(*(*(this + 380) + 4) + 28))(a1: *(this + 380) + 4, a2: *(this + 381));`
  - `*(this + 381) = 0;`
  - `(*(*(*(this + 378) + 4) + 28))(a1: *(this + 378) + 4, a2: 1);`
  - `CUIToolTip::ClearToolTip(this: (this + 1608));`
  - `CUIMonsterCarnival::ResetUI(this);`
  - `return CUIWnd::OnButtonClicked(a1: a2);`
  - `(*(*this + 56))(a1: this, a2: *(this + 384));`
  - `CUIMonsterCarnival::RequestSend(this);`

#### CUIMonsterCarnival::OnCreate  (30882 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1262_UI_BASICIMG_BTMAX`, `SP_1410_CANVASFONT`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_4098_UI_UIWINDOWIMG_MONSTERCARNIVAL_ICON0`, `SP_4099_UI_UIWINDOWIMG_MONSTERCARNIVAL_ICON1`, `SP_4102_UI_UIWINDOWIMG_MONSTERCARNIVAL_BACKGRND3_TOP_0`, `SP_4103_UI_UIWINDOWIMG_MONSTERCARNIVAL_BACKGRND3_MIDDLE1_0`, `SP_4104_UI_UIWINDOWIMG_MONSTERCARNIVAL_BACKGRND3_BOTTOM_0`, `SP_4105_UI_UIWINDOWIMG_MONSTERCARNIVAL_BACKGRND3_MIDDLE0_0`, `SP_4106_UI_UIWINDOWIMG_MONSTERCARNIVAL_BTSIDE`, `SP_4107_UI_UIWINDOWIMG_MONSTERCARNIVAL_TAB_ENABLED`, `SP_4108_UI_UIWINDOWIMG_MONSTERCARNIVAL_TAB_DISABLED`, `SP_4112_UI_UIWINDOWIMG_MONSTERCARNIVAL_ICON2`, `SP_4113_UI_UIWINDOWIMG_MONSTERCARNIVAL_ICON3`, `SP_5527_ARIAL`
  - `void __thiscall CUIMonsterCarnival::OnCreate(CCtrlTab **this, int j)`
  - `for ( i = 3; i != 0; --i )`
  - `v4 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 12);`
  - `if ( v4 != 0 )`
  - `CUIMonsterCarnival::SetLayer(this);`
  - `sub_402F85(pvargDest: &v173, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v174, pvargSrc: &pvargSrc);`
  - `LOBYTE(v177) = 1;`
  - `v7 = sub_406472();`
  - `v8 = sub_402680(a1: v7);`
  - `IWzResMan::GetObjectA(this: v8, pvargDest: &pvarg, sUOL: v154, vParam: v155, vAux: v156);`
  - `LOBYTE(v177) = 3;`
  - `i = Ztl_variant_t::GetUnknown(a1: p_i, a2: v158);`
  - `v9 = sub_41E527(a1: &i);`
  - `if ( v9 < 0 && v9 != -2147467262 )`
  - `if ( v10 < 0 )`
  - …（截断 16 行）

#### CUIMonsterCarnival::OnMouseButton  (1755 B)

  - `void __thiscall CUIMonsterCarnival::OnMouseButton(`
  - `if ( a2 == 513 || a2 == 515 )`
  - `if ( dword_BF0484(a1: this + 1480, a2: a4 - 6, a3: a5 - 128) != 0 )`
  - `*(this + 374) = *(this + 374) == 0;`
  - `*(this + 728) = 8000;`
  - `CUIToolTip::ClearToolTip(this: (this + 1604));`
  - `v8 = (this - 4);`
  - `CUIMonsterCarnival::SetButton(this: v8);`
  - `CWnd::InvalidateRect(this: v8, a2: 0);`
  - `v12 = *(this + 726);`
  - `SelectItemIdx = CTabList::GetSelectItemIdx(this: *(this + *(this + 375) + 385), a2: __PAIR64__(v7, v5));`
  - `*(this + 726) = SelectItemIdx;`
  - `v11 = *(*(this + 369) + 4);`
  - `if ( v10 )`
  - `(*(v11 + 28))(a1: 0);`
  - `(*(v11 + 28))(a1: 1);`
  - …（截断 16 行）

#### CUIMonsterCarnival::OnMouseMove  (743 B)

  - `int __thiscall CUIMonsterCarnival::OnMouseMove(CUIMonsterCarnival *this, struct tagPOINT a2)`
  - `v2 = (this - 4);`
  - `if ( CUIMonsterCarnival::DrawCPTooltip(this: (this - 4), a2: a2.x, a3: a2.y, a4: a2) == 0 )`
  - `if ( CUIMonsterCarnival::DrawLockTooltip(this: v2, a2: a2.x, a3: a2.y, a4: v4) == 0 )`
  - `CUIMonsterCarnival::DrawListTooltip(this: v2, a2: a2.x, a3: a2.y, a4: v5);`

#### CUIMonsterCarnival::OnTabAutoOpen  (609 B)

- SP_ 节点：`SP_1231_TAB`
  - `void __thiscall CUIMonsterCarnival::OnTabAutoOpen(CUIMonsterCarnival *this, int a2)`
  - `if ( *(this + 381) != 0 && a2 != 0 )`
  - `v3 = (*(this + 376) + 1) % 3;`
  - `v3 = *(this + 376);`
  - `play_ui_sound(a1: *StringW);`
  - `CCtrlTab::SetTab(this: *(this + 366), a2: v3);`

#### CUIMonsterCarnival::RequestSend  (1002 B)

  - `int __thiscall CUIMonsterCarnival::RequestSend(CUIMonsterCarnival *this)`
  - `result = CWvsContext::CanSendExclRequest(a1: 500, a2: 0);`
  - `if ( result != 0 )`
  - `COutPacket::COutPacket(a1: 218);`
  - `v3 = *(this + 1504);`
  - `COutPacket::Encode1(this: &v6, a2: v3);`
  - `COutPacket::Encode4(this: &v6, a2: *(this + 727) - 1);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v6);`
  - `*(*dword_BE7918 + 8356) = 1;`
  - `update_time = get_update_time();`
  - `*(v4 + 8360) = update_time;`
  - `return ZArray<unsigned char>::RemoveAll(a1: v7);`

#### CUIMonsterCarnival::ResetUI  (281 B)

  - `void __thiscall CUIMonsterCarnival::ResetUI(CUIMonsterCarnival *this)`
  - `CUIMonsterCarnival::SetLayer(this);`
  - `CUIMonsterCarnival::SetScrollBar(this);`
  - `CUIMonsterCarnival::SetButton(this);`
  - `CWnd::InvalidateRect(this, a2: 0);`

#### CUIMonsterCarnival::SetButton  (2063 B)

- SP_ 节点：`SP_4109_UI_BASICIMG_BTDECIDE`
  - `void __thiscall CUIMonsterCarnival::SetButton(CUIMonsterCarnival *this)`
  - `v2 = *(this + 370);`
  - `if ( v2 != 0 )`
  - `(*(*v2 + 12))(a1: v2);`
  - `v3 = *(*(this + *(this + 376) + 386) + 8);`
  - `if ( v3 != 0 )`
  - `v13 = *(v3 - 4);`
  - `if ( v13 > 12 )`
  - `memset(v10, 0, sizeof(v10));`
  - `LOBYTE(v14) = 1;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `LOBYTE(v14) = 0;`
  - `v7 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v14) = 2;`
  - `if ( v7 != 0 )`
  - `v8 = CCtrlButton::CCtrlButton(this: v7);`
  - …（截断 16 行）

#### CUIMonsterCarnival::SetLayer  (590 B)

  - `void __thiscall CUIMonsterCarnival::SetLayer(CUIMonsterCarnival *this)`
  - `v1 = *(this + 381);`
  - `if ( v1 != 0 )`
  - `v2 = *(*(this + *(this + 376) + 386) + 8);`
  - `if ( v2 != 0 )`
  - `v2 = *(v2 - 4);`
  - `if ( v2 > 12 )`
  - `v3 = 13 * (v2 + 14);`
  - `CUIMonsterCarnival::SetLayerSize(this, a2: 162, a3: v3);`

#### CUIMonsterCarnival::SetLayerSize  (5802 B)

- SP_ 节点：`SP_956_CANVAS`
  - `int __thiscall CUIMonsterCarnival::SetLayerSize(CUIMonsterCarnival *this, int a2, int a3)`
  - `v5 = *(this + 9) == a2;`
  - `if ( (!v5 || *(this + 10) != a3) && a2 != 0 && a3 != 0 )`
  - `sub_402FAB(a1: -2, a2: 3);`
  - `v5 = *(this + 6) == 0;`
  - `if ( v5 )`
  - `IWzGr2DLayer::RemoveCanvas(this: *(this + 6), a2: &v32, a3: &pvarg);`
  - `sub_4156AB(a1: &v32);`
  - `if ( v6 < 0 )`
  - `LOBYTE(v34) = 2;`
  - `PcCreateObject::IWzCanvas(a1: v8, a2: &v33, a3: 0);`
  - `LOBYTE(v34) = 1;`
  - `if ( v32 != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v34) = 3;`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - …（截断 16 行）

#### CUIMonsterCarnival::SetScrollBar  (1022 B)

  - `void __thiscall CUIMonsterCarnival::SetScrollBar(CUIMonsterCarnival *this)`
  - `v2 = *(*(this + *(this + 376) + 386) + 8);`
  - `if ( v2 == 0 )`
  - `v3 = *(v2 - 4);`
  - `if ( v3 <= 12 )`
  - `v5 = *(this + 368);`
  - `if ( v5 != 0 )`
  - `(*(*v5 + 12))(a1: v5);`
  - `if ( *(this + 381) != 0 )`
  - `(*(**(this + 368) + 32))(a1: *(this + 368), a2: this, a3: 1, a4: 1, a5: 3, a6: 142, a7: 128, a8: v6, a9: 0);`
  - `*(*(this + 368) + 52) = 100;`
  - `if ( v3 > 12 )`
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 368), a2: v7);`

#### CUIMonsterCarnival::SetUIData  (771 B)

  - `void __stdcall CUIMonsterCarnival::SetUIData(int a1, int a2, char a3, int *a4, char a5)`
  - `ZXString<char>::operator=(a1: &a5);`
  - `LOBYTE(v9) = 2;`
  - `sub_428211(a1: &a3);`
  - `LOBYTE(v9) = 1;`
  - `sub_86C9EC(a1: a2, a2: v6, a3: v7, a4: v8[0]);`
  - `LOBYTE(v9) = 0;`

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

### CUINewYearCardSenderDlg（10 函数）


#### CUINewYearCardSenderDlg::Draw  (1355 B)

  - `int __thiscall CUINewYearCardSenderDlg::Draw(CNewYearCardReceiverSearchResult **this, const struct tagRECT *a2)`
  - `CWnd::GetCanvas(a1: &v7);`
  - `if ( v7 == 0 )`
  - `v4 = (*(*v7 + 140))(a1: v7, a2: 0, a3: 0, a4: 518, a5: 188, a6: 0xFFFFFF);`
  - `if ( v4 < 0 )`
  - `if ( *(this + 196) != 0 )`
  - `CWnd::GetCanvas(a1: &v8);`
  - `LOBYTE(v9) = 1;`
  - `if ( v8 != 0 )`
  - `(*(*&v8->vt + 4))(a1: v8);`
  - `sub_A509F7(this: *(this + 48), a2: v6[0]);`
  - `LOBYTE(v9) = 0;`
  - `(*(*&v8->vt + 8))(a1: v8);`
  - `CWnd::Draw(this, a2);`
  - `if ( v7 != 0 )`
  - `return (*(*v7 + 8))(a1: v7);`
  - …（截断 16 行）

#### CUINewYearCardSenderDlg::GetRTTI  (120 B)

  - `int *__thiscall CUINewYearCardSenderDlg::GetRTTI(CUINewYearCardSenderDlg *this)`

#### CUINewYearCardSenderDlg::HitTest  (1146 B)

  - `int __thiscall CUINewYearCardSenderDlg::HitTest(`
  - `v5 = CWnd::HitTest(a1: a2, a2: a3, a3: a4);`
  - `if ( v5 != 0 )`
  - `if ( v6 != 0 )`
  - `if ( v6 == 1 )`
  - `if ( a4 != 0 && *a4 != 0 )`
  - `if ( (*(*(*a4 + 1) + 40))(a1: *a4 + 4) == 0 )`
  - `if ( a2 > 0x166 || a3 > 0xBC )`
  - `else if ( a2 <= 0x166 && a3 <= 0xBC )`
  - `else if ( *(this + 196) != 0 )`
  - `result = CNewYearCardReceiverSearchResult::HitTest(this: *(this + 48), a2, a3);`
  - `if ( result == 0 )`

#### CUINewYearCardSenderDlg::IsKindOf  (399 B)

  - `int __thiscall CUINewYearCardSenderDlg::IsKindOf(CUINewYearCardSenderDlg *this, const struct CRTTI *a2)`
  - `if ( &dword_BF55D4 == 0 )`
  - `while ( v2 != a2 )`
  - `if ( v2 == 0 )`

#### CUINewYearCardSenderDlg::OnButtonClicked  (562 B)

  - `int __thiscall CUINewYearCardSenderDlg::OnButtonClicked(CUINewYearCardSenderDlg *this, unsigned int a2)`
  - `if ( a2 != 0 )`
  - `if ( a2 == 1 )`
  - `sub_A51758(a1: this);`
  - `return sub_A518AB(a1: 1);`
  - `if ( a2 == 2 )`
  - `return sub_A518AB(a1: 2);`
  - `sub_A518D0(a1: 1);`
  - `return sub_A516A7(a1: this);`

#### CUINewYearCardSenderDlg::OnChildNotify  (425 B)

  - `int __thiscall CUINewYearCardSenderDlg::OnChildNotify(`
  - `if ( a2 == 5 && a3 >= 0x12C && a3 <= 0x130 )`
  - `return sub_A50D4E(a1: *(this + 48));`
  - `if ( a3 == 100 )`
  - `return (*(*this + 32))(a1: this, a2);`

#### CUINewYearCardSenderDlg::OnCreate  (5233 B)

  - `void __thiscall CUINewYearCardSenderDlg::OnCreate(CUINewYearCardSenderDlg *this, void *a2)`
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 24) != 0 )`
  - `v3 = sub_A506DD(a1: this);`
  - `*(this + 48) = v3;`
  - `memset(v36, 0, sizeof(v36));`
  - `sub_41C06C(a1: str_Ui_16, a2: -1);`
  - `v4 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `if ( v4 != 0 )`
  - `v5 = CCtrlButton::CCtrlButton(this: v4);`
  - `LOBYTE(v39) = 1;`
  - `v6 = sub_4284FF(a1: v5);`
  - `(*(**(v6 + 4) + 32))(a1: *(v6 + 4), a2: this, a3: 0, a4: 295, a5: 65, a6: 1, a7: v36);`
  - `sub_41C06C(a1: str_Ui_17, a2: -1);`
  - `v7 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v39) = 3;`
  - `if ( v7 != 0 )`
  - …（截断 16 行）

#### CUINewYearCardSenderDlg::OnKey  (213 B)

  - `void __thiscall CUINewYearCardSenderDlg::OnKey(CUINewYearCardSenderDlg *this, unsigned int a2, signed int a3)`
  - `if ( a3 < 0 || a2 != 13 && a2 != 27 )`
  - `CDialog::OnKey(this, a2, a3);`

#### CUINewYearCardSenderDlg::OnMouseButton  (324 B)

  - `int __thiscall CUINewYearCardSenderDlg::OnMouseButton(`
  - `if ( *(this + 192) != 0 )`
  - `return sub_A50C39(a1: a4, a2: a5);`

#### CUINewYearCardSenderDlg::OnMouseMove  (213 B)

  - `int __thiscall CUINewYearCardSenderDlg::OnMouseMove(CUINewYearCardSenderDlg *this, int a2, int a3)`
  - `if ( *(this + 192) != 0 )`
  - `sub_A50CC6(a1: a2, a2: a3);`

### CNewYearCardReceiverSearchResult（4 函数）


#### CNewYearCardReceiverSearchResult::HitTest  (634 B)

  - `int __thiscall CNewYearCardReceiverSearchResult::HitTest(CNewYearCardReceiverSearchResult *this, int a2, int a3)`
  - `if ( a2 < 368 || a2 > 488 || a3 < 24 || a3 > 99 )`
  - `CNewYearCardReceiverSearchResult::_ClearHighlight(this);`
  - `Selection = CNewYearCardReceiverSearchResult::_GetSelection(this, a2, a3);`
  - `if ( Selection - CNewYearCardReceiverSearchResult::_GetTopItemIndex(this) < 0 )`

#### CNewYearCardReceiverSearchResult::_ClearHighlight  (172 B)

  - `int __thiscall CNewYearCardReceiverSearchResult::_ClearHighlight(CNewYearCardReceiverSearchResult *this)`
  - `*(this + 5) = -1;`
  - `return sub_A50D4E(a1: this);`

#### CNewYearCardReceiverSearchResult::_GetSelection  (829 B)

  - `unsigned int __thiscall CNewYearCardReceiverSearchResult::_GetSelection(`
  - `if ( a2 >= 368 && a2 <= 488 && a3 >= 24 && a3 <= 99 )`
  - `if ( *this != 0 )`
  - `v3 = *(*this - 4);`
  - `result = CNewYearCardReceiverSearchResult::_GetTopItemIndex(this);`
  - `while ( result < v3 )`
  - `if ( a3 >= v5 && a3 <= (v5 + 13) )`
  - `if ( v5 >= 0x64 )`

#### CNewYearCardReceiverSearchResult::_GetTopItemIndex  (449 B)

  - `int __thiscall CNewYearCardReceiverSearchResult::_GetTopItemIndex(CNewYearCardReceiverSearchResult *this)`
  - `result = *(*(*(this + 1) + 180) + 56);`
  - `if ( result < 0 )`
  - `if ( v2 != 0 )`
  - `v2 = *(v2 - 4);`
  - `if ( result > (v2 - 5) )`

### GW_NewYearCardRecord（1 函数）


#### GW_NewYearCardRecord::Decode  (1324 B)

  - `volatile LONG *__thiscall GW_NewYearCardRecord::Decode(GW_NewYearCardRecord *this, volatile LONG *a2)`
  - `*this = CInPacket::Decode4(this: a2);`
  - `*(this + 1) = CInPacket::Decode4(this: v2);`
  - `v4 = CInPacket::DecodeStr(this: v2, &a2);`
  - `lstrcpy(a1: this + 8, a2: *v4);`
  - `*(this + 21) = CInPacket::Decode1(this: v2);`
  - `CInPacket::DecodeBuffer(this: v2, a2: this + 25, a3: 8u);`
  - `*(this + 33) = CInPacket::Decode4(this: v2);`
  - `v5 = CInPacket::DecodeStr(this: v2, &a2);`
  - `lstrcpy(a1: this + 37, a2: *v5);`
  - `*(this + 50) = CInPacket::Decode1(this: v2);`
  - `*(this + 54) = CInPacket::Decode1(this: v2);`
  - `CInPacket::DecodeBuffer(this: v2, a2: this + 58, a3: 8u);`
  - `v6 = CInPacket::DecodeStr(this: v2, &a2);`
  - `lstrcpy(a1: this + 66, a2: *v6);`
  - `if ( a2 != 0 )`
  - …（截断 16 行）

### CUIEventAlarm（1 函数）


#### CUIEventAlarm::CreateEventAlarm  (565 B)

  - `int __thiscall CUIEventAlarm::CreateEventAlarm(CUIEventAlarm *this)`
  - `v1 = *(this + 36);`
  - `if ( v2 < 0 )`
  - `if ( v2 > 800 - v1 )`
  - `v4 = *(this + 37);`
  - `if ( 600 - v4 < 0 )`
  - `return CDialog::CreateDlg(this, a2: v2, a3: v3, a4: v1, a5: v4, a6: 10, a7: 1, a8: 0);`

### CUIEnergyBar（2 函数）


#### CUIEnergyBar::OnMouseMove  (981 B)

- SP_ 节点：`SP_1886_DISPLAYS_THE_ENERGY_LEVEL_THATS_BEEN_CHARGED_NUSE_THE_MOUSE_TO_CHANGE_POSITION`
  - `int __thiscall CUIEnergyBar::OnMouseMove(CUIEnergyBar *this, int a2, int a3)`
  - `v8 = (*(v5 + 48))(a1: this) + a3 + 20;`
  - `v6 = (*(*this + 44))(a1: this);`
  - `CUIToolTip::SetToolTip_String_MultiLine(a1: v6 + a2 + 20, a2: v8, a3: v9, a4: v10, a5: v11);`

#### CUIEnergyBar::SetEnergy  (32544 B)

- SP_ 节点：`SP_1883_UI_UIWINDOWIMG_ENERGYBAR`, `SP_956_CANVAS`
  - `int __thiscall CUIEnergyBar::SetEnergy(ULONG **this, ULONG *arg0, int a2)`
  - `v4 = arg0 == *(this + 364);`
  - `if ( !v4 || a2 != *(this + 365) )`
  - `*(this + 364) = arg0;`
  - `*(this + 365) = v5;`
  - `LOBYTE(v141) = 1;`
  - `PcCreateObject::IWzCanvas(a1: v7, a2: &v140, a3: 0);`
  - `LOBYTE(v141) = 0;`
  - `if ( v138 != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v141) = 2;`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v141) = 3;`
  - `if ( v140 == 0 )`
  - `v8 = *(v139 + 10);`
  - `v9 = *(v139 + 9);`
  - …（截断 16 行）

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

### UMedalQuestInfo（1 函数）


#### UMedalQuestInfo::0?$ZRef  (270 B)

  - `LONG __thiscall ZRef<MedalQuestInfo>::ZRef<MedalQuestInfo>(LONG *this)`
  - `result = *(this + 1);`
  - `if ( result != 0 )`
  - `return InterlockedIncrement(lpAddend: (*(this + 1) != 0 ? result - 16 + 4 : 4));`

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

### CQuestMan（17 函数）


#### CQuestMan::CheckCompleteDemand  (20060 B)

  - `signed int __thiscall CQuestMan::CheckCompleteDemand(`
  - `if ( a2 >= 0x4B0u && a2 < 0x578u )`
  - `if ( a2 == 0 )`
  - `if ( sub_6CECD6(a1: &a2, a2: v85) == 0 )`
  - `LOBYTE(v94) = 1;`
  - `if ( sub_4E901B(this: (a4 + 1535), a2: &v90, a3: &v93) == 0 )`
  - `LOBYTE(v94) = 0;`
  - `if ( v93 != 0 )`
  - `if ( v8 != 0 && v8 != i )`
  - `while ( 1 )`
  - `if ( v9 == 0 || i >= *(v9 - 4) )`
  - `v10 = (v9 + 8 * i);`
  - `v88 = (v6 + 4 * v11 + 1095);`
  - `v12 = *v88 != 0 ? *(*v88 - 4) : 0;`
  - `if ( v90 > v12 - 1 )`
  - `v13 = *(CharacterData::GetItem(this: a4, a2: v80, a3: v87, a4: v90) + 4);`
  - …（截断 16 行）

#### CQuestMan::CheckStartDemand  (20890 B)

  - `int __fastcall CQuestMan::CheckStartDemand(int a1, int a2, int a3, char *Data, int a5, int a6, int a7, int a8)`
  - `if ( a3 == 0 )`
  - `if ( sub_6CECD6(a1: &a3, a2: v102) == 0 )`
  - `if ( sub_500EA5(a1: &v104) != 0 )`
  - `if ( v10 >= 0 && v10 < *(a1 + 76) )`
  - `if ( *v103 >= 0 && *v103 > *(a1 + 76) )`
  - `if ( v11 != 0 && v11 != Data )`
  - `if ( sub_724B43(a1: a3, a2: v9) == 0 )`
  - `sub_721C65(a1: a6);`
  - `dword_BF03B0(a1: &v92);`
  - `v98 = sub_7B8F35();`
  - `if ( sub_500E4C(a1: &a3, a2: &FileTime) != 0 )`
  - `if ( v103[12] < 0 )`
  - `if ( v103[37] == 0 )`
  - `FileTimeToSystemTime(lpFileTime: &FileTime, lpSystemTime: &SystemTime);`
  - `FileTimeToSystemTime(lpFileTime: &v98, lpSystemTime: &v95);`
  - …（截断 16 行）

#### CQuestMan::GetCompleteDemand  (851 B)

  - `int __thiscall CQuestMan::GetCompleteDemand(CQuestMan *this, unsigned __int16 a2)`
  - `if ( sub_6CECD6(a1: &a2, a2: v5) != 0 )`
  - `v3 = *(v2 + 1);`
  - `if ( (v9 & 1) != 0 && v8 != 0 )`
  - `sub_6CEC6E(a1: 0);`
  - `if ( v6 != 0 )`

#### CQuestMan::GetPartyQuestRankCount  (753 B)

  - `int __stdcall CQuestMan::GetPartyQuestRankCount(int a1, char *a2)`
  - `for ( i = 1200; i < 0x578u; ++i )`
  - `ZXString<char>::GetBuffer(a1: aRank, a2: 0xFFFFFFFF);`
  - `sub_4E704B(this: a2, a2: &v5, a3: i, a4: &v6);`
  - `LOBYTE(v7) = 2;`
  - `if ( ZXString<char>::operator==(a1) != 0 )`

#### CQuestMan::GetQuestByNpc  (219 B)

  - `BOOL __stdcall CQuestMan::GetQuestByNpc(char a1, int a2)`
  - `ZArray<unsigned short>::RemoveAll(a1: a2);`
  - `return ZMap<unsigned long,ZArray<unsigned short>,unsigned long>::GetAt(&a1, a2) != 0;`

#### CQuestMan::GetQuestDisplayMedal  (4831 B)

- SP_ 节点：`SP_3232_QUEST_QUESTINFOIMG`
  - `int __thiscall CQuestMan::GetQuestDisplayMedal(CQuestMan *this, int a2)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v32, pvargSrc: &pvargSrc);`
  - `LOBYTE(v37) = 1;`
  - `v4 = sub_406472();`
  - `v5 = sub_402680(a1: v4);`
  - `IWzResMan::GetObjectA(this: v5, pvargDest: &pvarg, sUOL: v23, vParam: v24, vAux: p_pvargDest);`
  - `LOBYTE(v37) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v26, a2: v27);`
  - `if ( Unknown != 0 )`
  - `v6 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v6 < 0 && v6 != -2147467262 )`
  - `LOBYTE(v37) = 7;`
  - `if ( v7 < 0 )`
  - `LOBYTE(v37) = 6;`
  - `if ( v8 < 0 )`
  - …（截断 16 行）

#### CQuestMan::IsAutoAutoAcceptQuest  (295 B)

  - `BOOL __thiscall CQuestMan::IsAutoAutoAcceptQuest(CQuestMan *this, unsigned __int16 usQuestID)`
  - `return ZMap<unsigned short,int,unsigned short>::GetAt(a1: &usQuestID, a2: &bAutoAccept) != 0;`

#### CQuestMan::IsAutoCompleteQuest  (230 B)

  - `BOOL __thiscall CQuestMan::IsAutoCompleteQuest(void *this, char a2)`
  - `return ZMap<unsigned short,int,unsigned short>::GetAt(a1: &a2, a2: &v3) != 0;`

#### CQuestMan::IsAutoStartQuest  (227 B)

  - `BOOL __thiscall CQuestMan::IsAutoStartQuest(void *this, char a2)`
  - `return ZMap<unsigned short,int,unsigned short>::GetAt(a1: &a2, a2: &v3) != 0;`

#### CQuestMan::IsCompleteScriptLinkedQuest  (851 B)

  - `int __thiscall CQuestMan::IsCompleteScriptLinkedQuest(CQuestMan *this, unsigned __int16 a2)`
  - `if ( a2 == 0 )`
  - `if ( sub_6CECD6(a1: &a2, a2: v5) == 0 )`
  - `v3 = *(v6 + 132);`
  - `if ( v3 == 0 || *v3 == 0 )`
  - `if ( !v2 )`
  - `sub_6CEC6E(a1: 0);`
  - `if ( v6 != 0 )`

#### CQuestMan::IsStartScriptLinkedQuest  (848 B)

  - `int __thiscall CQuestMan::IsStartScriptLinkedQuest(CQuestMan *this, unsigned __int16 a2)`
  - `if ( a2 == 0 )`
  - `if ( sub_6CECD6(a1: &a2, a2: v5) == 0 )`
  - `v3 = *(v6 + 128);`
  - `if ( v3 == 0 || *v3 == 0 )`
  - `if ( !v2 )`
  - `sub_6CEC6E(a1: 0);`
  - `if ( v6 != 0 )`

#### CQuestMan::LoadDemand  (10886 B)

- SP_ 节点：`SP_3220_QUEST_CHECKIMG`
  - `signed int __thiscall CQuestMan::LoadDemand(_DWORD *this)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v55, pvargSrc: &pvargSrc);`
  - `LOBYTE(v69) = 1;`
  - `_bstr_t::_bstr_t(a1: str_Et);`
  - `v2 = sub_406472();`
  - `v3 = sub_402680(a1: v2);`
  - `IWzResMan::GetObjectA(this: v3, pvargDest: &pvarg, sUOL: v45, vParam: v46, vAux: p_pvargDest);`
  - `LOBYTE(v69) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v48, a2: v49);`
  - `if ( Unknown != 0 )`
  - `v4 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v4 < 0 && v4 != -2147467262 )`
  - `LOBYTE(v69) = 7;`
  - `if ( v5 < 0 )`
  - `LOBYTE(v69) = 6;`
  - …（截断 16 行）

#### CQuestMan::LoadPartyQuestInfo  (12658 B)

  - `int __thiscall CQuestMan::LoadPartyQuestInfo(CQuestMan *this)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v46, pvargSrc: &pvargSrc);`
  - `LOBYTE(v70) = 1;`
  - `_bstr_t::_bstr_t(a1: str_Qu);`
  - `v2 = sub_406472();`
  - `v3 = sub_402680(a1: v2);`
  - `IWzResMan::GetObjectA(this: v3, pvargDest: &pvarg, sUOL: v34, vParam: v35, vAux: p_pvargDest);`
  - `LOBYTE(v70) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v37, a2: v38[0]);`
  - `if ( Unknown != 0 )`
  - `v4 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v4 < 0 && v4 != -2147467262 )`
  - `LOBYTE(v70) = 7;`
  - `if ( v5 < 0 )`
  - `LOBYTE(v70) = 6;`
  - …（截断 16 行）

#### CQuestMan::LoadPartyQuestRank  (18533 B)

  - `int __stdcall CQuestMan::LoadPartyQuestRank(char a1, int a2)`
  - `sub_72538C(&a1, a2: 0);`
  - `v2 = sub_726752(a1: -1);`
  - `_bstr_t::_bstr_t(a1: str_Mi);`
  - `LOBYTE(v106) = 0;`
  - `sub_40263B(a1: &a2);`
  - `Item = IWzProperty::GetItem(&pvargDest, a2: v62);`
  - `LOBYTE(v106) = 2;`
  - `*v4 = get_int32(pvarSrc: Item, a2: v63);`
  - `if ( v6 < 0 )`
  - `_bstr_t::_bstr_t(a1: str_Ma_0);`
  - `v8 = IWzProperty::GetItem(&pvargDest, a2: v62);`
  - `LOBYTE(v106) = 4;`
  - `v4[1] = get_int32(pvarSrc: v8, a2: v63);`
  - `if ( v9 < 0 )`
  - `if ( a2 == 0 )`
  - …（截断 16 行）

#### CQuestMan::LoadQuestInfo  (22479 B)

- SP_ 节点：`SP_1604_NAME`, `SP_1735_AREA`, `SP_3216_ORDER`, `SP_3232_QUEST_QUESTINFOIMG`, `SP_3236_PARENT`, `SP_4063_AUTOSTART`, `SP_4066_ONESHOT`, `SP_4067_TIMELIMIT`, `SP_4166_AUTOPRECOMPLETE`, `SP_4530_SORTKEY`, `SP_5111_AUTOACCEPT`, `SP_5354_SHOWLAYERTAG`
  - `_DWORD *__thiscall CQuestMan::LoadQuestInfo(_DWORD *this)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v119, pvargSrc: &pvargSrc);`
  - `LOBYTE(v144) = 1;`
  - `v4 = sub_406472();`
  - `v5 = sub_402680(a1: v4);`
  - `IWzResMan::GetObjectA(this: v5, pvargDest: &v103, sUOL: v98, vParam: v99, vAux: p_pvargDest);`
  - `LOBYTE(v144) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v101, a2: v102);`
  - `if ( Unknown != 0 )`
  - `v6 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v6 < 0 && v6 != -2147467262 )`
  - `LOBYTE(v144) = 7;`
  - `if ( v7 < 0 )`
  - `LOBYTE(v144) = 6;`
  - `if ( v8 < 0 )`
  - …（截断 16 行）

#### CQuestMan::RemoveRecentlyUpdatedQuest  (296 B)

  - `int __stdcall CQuestMan::RemoveRecentlyUpdatedQuest(unsigned __int16 a1)`
  - `result = ZList<unsigned short>::Find(&a1, a2: 0);`
  - `if ( result != 0 )`
  - `return ZList<unsigned short>::RemoveAt(a1: result);`

#### CQuestMan::SetRecentlyUpdatedQuest  (334 B)

  - `unsigned __int16 *__thiscall CQuestMan::SetRecentlyUpdatedQuest(CQuestMan *this, unsigned __int16 a2)`
  - `CQuestMan::RemoveRecentlyUpdatedQuest(a1: a2);`
  - `result = ZList<unsigned short>::AddHead(a1: this + 624);`

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

### CUIMonsterBook（5 函数）


#### CUIMonsterBook::GetCard  (190 B)

  - `int __thiscall CUIMonsterBook::GetCard(_DWORD *this, int a2)`
  - `sub_867A5B(a1: a2, a2: *(*(this + 694) + 52), a3: *(this + 364), a4: *(this + 366));`

#### CUIMonsterBook::LoadMobAction  (2175 B)

  - `int __stdcall CUIMonsterBook::LoadMobAction(void *a1, signed int a2, int a3)`
  - `if ( a2 > 0 )`
  - `*ZList<long>::AddTail(a1: v15) = 1;`
  - `*ZList<long>::AddTail(a1: v15) = 3;`
  - `if ( a2 > 1 )`
  - `*ZList<long>::AddTail(a1: v15) = 0;`
  - `*ZList<long>::AddTail(a1: v15) = 2;`
  - `if ( a2 > 2 )`
  - `for ( i = 12; i <= 20; ++i )`
  - `*ZList<long>::AddTail(a1: v15) = i;`
  - `for ( j = 21; j <= 37; ++j )`
  - `*ZList<long>::AddTail(a1: v15) = j;`
  - `if ( a2 > 3 )`
  - `for ( k = 6; k <= 8; ++k )`
  - `*ZList<long>::AddTail(a1: v15) = k;`
  - `if ( a2 > 4 )`
  - …（截断 16 行）

#### CUIMonsterBook::LoadMobInfo  (23747 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_12_BEGINNER`, `SP_1729_STREETNAME`, `SP_1730_MAPNAME`, `SP_5612_A0`, `SP_5613_A1`, `SP_5624_DELAY`, `SP_954_MOB_07DIMG`, `SP_960_DEFAULT`, `SP_961_INFO`
  - `void __thiscall CUIMonsterBook::LoadMobInfo(_DWORD *this, int a2, _DWORD *a3)`
  - `CUIMonsterBook::MOBINFO::Clear(this: (this + 904));`
  - `if ( a3 == 0 || *(*(v3 + 2776) + 52) == 9 )`
  - `sub_685F54(a1: &a2);`
  - `*v4 = sub_95FC65(a1: *a3);`
  - `MobTemplate = CMobTemplate::GetMobTemplate(a1: a3[1]);`
  - `sub_867DD0(a1: v101);`
  - `LOBYTE(v119) = 1;`
  - `if ( MobTemplate != 0 )`
  - `ZXString<char>::operator=(a1: MobTemplate + 48);`
  - `LOBYTE(v119) = 2;`
  - `LOBYTE(v119) = 3;`
  - `ZXString<char>::Format(a1: &v103, a2: p_p_p_pvargDest, a3: v85);`
  - `if ( v113 != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v119) = 4;`
  - …（截断 16 行）

#### CUIMonsterBook::UpdateCheckList  (455 B)

  - `int __thiscall CUIMonsterBook::UpdateCheckList(CUIMonsterBook *this)`
  - `for ( i = 0; i < 9; ++i )`
  - `v3 = sub_95FDB4(a1: i) == 0;`
  - `v4 = **(this + 694);`
  - `if ( v3 )`
  - `result = (*(v4 + 36))(a1: i, a2: 0);`
  - `result = (*(v4 + 36))(a1: i, a2: 1);`

#### CUIMonsterBook::UpdateUI  (659 B)

  - `void __thiscall CUIMonsterBook::UpdateUI(CUIMonsterBook *this)`
  - `v2 = (this + 2836);`
  - `for ( i = 3; i != 0; --i )`
  - `if ( *(*(this + 694) + 52) != 9 )`
  - `CUIMonsterBook::GetCard(this, a2: &v4);`
  - `CUIMonsterBook::LoadMobInfo(this, a2: v4, a3: v5);`
  - `CUIMonsterBook::UpdateCheckList(this);`

### CMonsterBookMan（6 函数）


#### CMonsterBookMan::GetCard  (529 B)

  - `int __stdcall CMonsterBookMan::GetCard(int a1, char a2)`
  - `sub_6860FE(a1: &a2, a2: &v3);`
  - `*(a1 + 4) = v4;`
  - `sub_685F37(a1);`
  - `LOBYTE(v6) = 0;`
  - `if ( v4 != 0 )`
  - `sub_685FE2(this: &v3, a2: 0);`

#### CMonsterBookMan::GetString  (904 B)

  - `int __stdcall CMonsterBookMan::GetString(int a1, char nCardID)`
  - `CMonsterBookMan::GetCard(a1: &v5, a2: nCardID);`
  - `LOBYTE(v8) = 2;`
  - `if ( v6 != 0 )`
  - `ZMap<long,ZRef<MonsterBookStr>,long>::GetAt(a1: v6 + 4, a2: v3);`
  - `*(a1 + 4) = v4;`
  - `sub_685F9D(a1);`
  - `LOBYTE(v8) = 1;`
  - `sub_685FE2(this: &v5, a2: 0);`
  - `LOBYTE(v8) = 0;`
  - `if ( v4 != 0 )`
  - `sub_686011(a1: 0);`

#### CMonsterBookMan::LoadBook  (215 B)

  - `BOOL __thiscall CMonsterBookMan::LoadBook(int *this)`
  - `return CMonsterBookMan::LoadCard(this) != 0`
  - `&& CMonsterBookMan::LoadStringA(this) != 0`
  - `&& CMonsterBookMan::LoadBookIcon(this) != 0;`

#### CMonsterBookMan::LoadBookIcon  (5609 B)

- SP_ 节点：`SP_2356_ICON`, `SP_2584_UI_UIWINDOWIMG_MONSTERBOOK`
  - `signed int __thiscall CMonsterBookMan::LoadBookIcon(int *this)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v33, pvargSrc: &pvargSrc);`
  - `LOBYTE(v41) = 1;`
  - `v4 = sub_406472();`
  - `v5 = sub_402680(a1: v4);`
  - `IWzResMan::GetObjectA(this: v5, pvargDest: &pvarg, sUOL: v24, vParam: v25, vAux: p_pvargDest);`
  - `LOBYTE(v41) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v27, a2: v28);`
  - `if ( Unknown != 0 )`
  - `v6 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v6 < 0 && v6 != -2147467262 )`
  - `LOBYTE(v41) = 7;`
  - `if ( v7 < 0 )`
  - `LOBYTE(v41) = 6;`
  - `if ( v8 < 0 )`
  - …（截断 16 行）

#### CMonsterBookMan::LoadCard  (11641 B)

- SP_ 节点：`SP_1000_BLAZE`, `SP_1589_ICONRAW`, `SP_1593_MOB`, `SP_3830_ITEM_CONSUME_0238IMG`, `SP_961_INFO`
  - `signed int __thiscall CMonsterBookMan::LoadCard(_DWORD *this)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v45, pvargSrc: &pvargSrc);`
  - `LOBYTE(v67) = 1;`
  - `v3 = sub_406472();`
  - `v4 = sub_402680(a1: v3);`
  - `IWzResMan::GetObjectA(this: v4, pvargDest: &v40, sUOL: v34, vParam: v35, vAux: p_pvargDest);`
  - `LOBYTE(v67) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: i, a2: v38);`
  - `if ( Unknown != 0 )`
  - `v5 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v5 < 0 && v5 != -2147467262 )`
  - `LOBYTE(v67) = 7;`
  - `if ( v6 < 0 )`
  - `LOBYTE(v67) = 6;`
  - `if ( v7 < 0 )`
  - …（截断 16 行）

#### CMonsterBookMan::LoadStringA  (21328 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1612_MAXHP`, `SP_1613_MAXMP`, `SP_2239_STRING_MONSTERBOOKIMG`, `SP_2242_EPISODE`, `SP_2243_REWARD`, `SP_950_MAP`, `SP_954_MOB_07DIMG`, `SP_961_INFO`
  - `signed int __thiscall CMonsterBookMan::LoadStringA(CMonsterBookMan *this)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v100, pvargSrc: &pvargSrc);`
  - `LOBYTE(v127) = 1;`
  - `v3 = sub_406472();`
  - `v4 = sub_402680(a1: v3);`
  - `IWzResMan::GetObjectA(this: v4, pvargDest: &v94, sUOL: v82, vParam: v83, vAux: p_pvargDest);`
  - `LOBYTE(v127) = 3;`
  - `v112[0] = Ztl_variant_t::GetUnknown(a1: v85, a2: v86);`
  - `if ( v112[0] != 0 )`
  - `v5 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: v112);`
  - `if ( v5 < 0 && v5 != -2147467262 )`
  - `LOBYTE(v127) = 7;`
  - `if ( v6 < 0 )`
  - `LOBYTE(v127) = 6;`
  - `if ( v7 < 0 )`
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

### CItemMakerInfo（1 函数）


#### CItemMakerInfo::GetItemMakeInfo  (529 B)

  - `int __stdcall CItemMakerInfo::GetItemMakeInfo(int a1, char a2)`
  - `sub_5E8DB4(a1: &a2, a2: v3);`
  - `*(a1 + 4) = v4;`
  - `sub_5E8C98(a1);`
  - `LOBYTE(v6) = 0;`
  - `if ( v4 != 0 )`
  - `sub_5E8CCB(a1: 0);`

### CUIGameMenu（1 函数）


#### CUIGameMenu::ResetButtonState  (427 B)

  - `void __thiscall CUIGameMenu::ResetButtonState(CUIGameMenu *this)`
  - `v3 = (this + 216);`
  - `v4 = *(this + 61) != 0 && *(this + 62) == v2;`
  - `CCtrlButton::MouseEnter(this: *v3, a2: v4, a3: 1);`
  - `while ( v2 < 4 );`

### CUIRevive（3 函数）


#### CUIRevive::OnButtonClicked  (412 B)

  - `int __thiscall CUIRevive::OnButtonClicked(CUIRevive *this, int a2)`
  - `if ( a2 == 1 )`
  - `return CUIRevive::Revive(this, a2: 0);`
  - `if ( a2 == 6 )`
  - `return CUIRevive::Revive(this, a2: 1);`
  - `if ( a2 == 7 )`

#### CUIRevive::Revive  (450 B)

  - `void __thiscall CUIRevive::Revive(CUIRevive *this, int a2)`
  - `field = get_field();`
  - `if ( field != 0 )`
  - `CField::SendTransferFieldRequest(a1: field, a2: v3, a3: 0, a4: 0, a5: a2, a6: 0, a7: 0);`
  - `if ( *dword_BE7918 != 0 )`
  - `CWvsContext::OnRevive(a1: *dword_BE7918);`
  - `CWvsContext::UI_CloseRevive(this: *dword_BE7918);`

#### CUIRevive::Update  (272 B)

  - `unsigned int __thiscall CUIRevive::Update(CUIRevive *this)`
  - `result = timeGetTime() - *(this + 34);`
  - `if ( result > 0x927C0 )`
  - `return CUIRevive::Revive(this, a2: 0);`

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

### CGrenade（3 函数）


#### CGrenade::BombAndAttackMob  (781 B)

  - `int __thiscall CGrenade::BombAndAttackMob(int *this)`
  - `v2 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v6) + 4);`
  - `if ( v6[1] != 0 )`
  - `sub_428C44(this: v6, a2: 0);`
  - `v5 = *(this + 17);`
  - `SkillLevel = CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: v2, a3: v5, a4: &v7);`
  - `return CUserLocal::TryDoingMeleeAttack(`
  - `this: *(this + 4),`

#### CGrenade::JustBomb  (873 B)

  - `// Hex-Rays decompile failed at 0x58fc17; DISASM fallback (13339, 12 instructions):`
  - `// void __thiscall CGrenade::JustBomb(CGrenade *this)`
  - `//   SKILLENTRY *skill = CSkillInfo::GetSkill(dword_BE78DC, *(this + 0x44));  // 0x58fbfc..0x58fc08`
  - `//   (*(this + 0x10) CUser*)->ShowSkillSpecialEffect(skill, 0x320, this, ...); // 0x58fc0d..0x58fc17`

#### CGrenade::Update  (1748 B)

  - `signed int __thiscall CGrenade::Update(int *this)`
  - `v2 = *(this + 5);`
  - `if ( v2 == 0 )`
  - `v9 = v2 != 0 ? (v2 - 12) : 0;`
  - `CVecCtrl::UpdateActive(this: v9);`
  - `*(this + 11) = *(this + 9);`
  - `*(this + 12) = *(this + 10);`
  - `VariantInit(&pvarg);`
  - `v3 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v3 < 0 )`
  - `if ( *(this + 5) == 0 )`
  - `v4 = **(this + 5);`
  - `v10 = *(this + 5);`
  - `v5 = (*(v4 + 140))(`
  - `if ( v5 < 0 )`
  - `if ( v6 < 0 )`
  - …（截断 16 行）

### CVecCtrlGrenade（1 函数）


#### CVecCtrlGrenade::WorkUpdateActive  (620 B)

  - `int __thiscall CVecCtrlGrenade::WorkUpdateActive(CVecCtrlGrenade *this, int a2)`
  - `if ( *(this + 98) != 0 )`
  - `CVecCtrl::Impact(this);`
  - `if ( *(this + 68) != 0 )`
  - `*(this + 146) = 1;`
  - `qmemcpy(v4, this + 32, sizeof(v4));`
  - `CVecCtrl::CalcFloat(this, a2);`
  - `if ( CVecCtrl::CollisionDetectFloat(this, a2: v4, a3: &a2, a4: 1) != 0 && a2 > 0 )`
  - `(*(*this + 24))(a1: this, a2);`

### CShip（3 函数）


#### CShip::AppearShip  (7197 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1252_SOUND_BGM04_ARABPIRATE`, `SP_3_HTTP__MAPLESTORYNEXONNET_WZASPXPART_DOWNLOADS_GAMEPATCHES`
  - `int *__thiscall CShip::AppearShip(CShip *this)`
  - `if ( *(this + 17) != 0 && *(this + 3) == 1 )`
  - `if ( *(this + 17) == 0 )`
  - `v3 = *(this + 17);`
  - `v4 = (*(*v3 + 64))(a1: v3, a2: *(this + 4), a3: *(this + 6));`
  - `if ( v4 < 0 )`
  - `sub_432C10(a1: 0);`
  - `CAnimationDisplayer::LoadLayer(this: &v32, a2: this + 17);`
  - `v31 = *(this + 9) != 0;`
  - `v7 = sub_40B31F(a1: this + 60);`
  - `v8 = sub_43E86F(a1: &v43, a2: v7, a3: v31, a4: v32, a5: v33, a6: v34, a7: v35, a8: SHIDWORD(v35), a9: v36, a10: v37);`
  - `v9 = (this + 64);`
  - `sub_428712(a1: *v8);`
  - `sub_4156C5(a1: &v43);`
  - `if ( *(this + 16) == 0 )`
  - `Alpha = IWzGr2DLayer::GetAlpha(a1: &v42);`
  - …（截断 16 行）

#### CShip::EnterShipMove  (2664 B)

- SP_ 节点：`SP_1248_WHISTLE`
  - `void *__thiscall CShip::EnterShipMove(_DWORD *this)`
  - `if ( *(this + 16) != 0 && *(this + 3) == 0 )`
  - `sub_989A79(a1: v4, a2: 100);`
  - `if ( *(this + 17) == 0 )`
  - `v5 = *(this + 17);`
  - `v6 = (*(*v5 + 64))(a1: v5, a2: *(this + 5), a3: *(this + 6));`
  - `if ( v6 < 0 )`
  - `VariantInit(&pvarg);`
  - `v7 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v7 < 0 )`
  - `v8 = *(this + 16) == 0;`
  - `if ( v8 )`
  - `v9 = sub_437476(a1: *(this + 16));`
  - `sub_402FAB(a1: 1000 * *(this + 8) + v9, a2: 3);`
  - `v8 = *(this + 17) == 0;`
  - `LOBYTE(v16) = 2;`
  - …（截断 16 行）

#### CShip::LeaveShipMove  (2663 B)

- SP_ 节点：`SP_1248_WHISTLE`
  - `void *__thiscall CShip::LeaveShipMove(CShip *this)`
  - `if ( *(this + 16) != 0 && *(this + 3) == 0 )`
  - `sub_989A79(a1: v4, a2: 100);`
  - `if ( *(this + 17) == 0 )`
  - `v5 = *(this + 17);`
  - `v6 = (*(*v5 + 64))(a1: v5, a2: *(this + 4), a3: *(this + 6));`
  - `if ( v6 < 0 )`
  - `VariantInit(&pvarg);`
  - `v7 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v7 < 0 )`
  - `v8 = *(this + 16) == 0;`
  - `if ( v8 )`
  - `v9 = sub_437476(a1: *(this + 16));`
  - `sub_402FAB(a1: 1000 * *(this + 8) + v9, a2: 3);`
  - `v8 = *(this + 17) == 0;`
  - `LOBYTE(v16) = 2;`
  - …（截断 16 行）

### CTutor（1 函数）


#### CTutor::OnMessage  (6682 B)

- SP_ 节点：`SP_1752_HEIGHT`, `SP_5138_UI_TUTORIALIMG_D`
  - `int __thiscall CTutor::OnMessage(LONG *this, char a2, int a3)`
  - `sub_428712(a1: 0);`
  - `*(this + 94) = dword_BF060C(a1: v37);`
  - `v36.lVal = *(this + 45);`
  - `*(this + 95) = a3;`
  - `CActionMan::GetSummonedProp(a1: &v47, a2: v36.lVal, a3: v36.cyVal.Hi);`
  - `LOBYTE(v48) = 0;`
  - `sub_40263B(a1: &v47);`
  - `Item = IWzProperty::GetItem(pvargDest: &pvarg, a2: v36.lVal);`
  - `LOBYTE(v48) = 2;`
  - `int32 = get_int32(pvarSrc: Item, a2: v36.cyVal.Hi);`
  - `if ( v9 < 0 )`
  - `sub_4145AB(a1: this + 54);`
  - `CAnimationDisplayer::LoadLayer(this: &v33, a2: this + 83);`
  - `LOBYTE(v48) = 4;`
  - `LOBYTE(v48) = 5;`
  - …（截断 16 行）

### VCTutor（1 函数）


#### VCTutor::4?$ZRef  (415 B)

  - `int *__thiscall ZRef<CTutor>::operator=(int *this, int a2)`
  - `sub_9615FA(a1: v6);`
  - `v4 = *(this + 1);`
  - `*(this + 1) = v7;`
  - `sub_96159B(a1: v6);`

### CSlideNotice（2 函数）


#### CSlideNotice::OnCreate  (801 B)

  - `int __thiscall CSlideNotice::OnCreate(CSlideNotice *this, void *a2)`
  - `Canvas = CWnd::GetCanvas(a1: &v6);`
  - `if ( *Canvas == 0 )`
  - `v4 = (*(**Canvas + 140))(a1: *Canvas, a2: 0, a3: 0, a4: 800, a5: 600, a6: 0x80000000);`
  - `if ( v4 < 0 )`
  - `if ( v6 != 0 )`
  - `return (*(*v6 + 8))(a1: v6);`

#### CSlideNotice::SetMsg  (14819 B)

- SP_ 节点：`SP_956_CANVAS`
  - `int __thiscall CSlideNotice::SetMsg(_DWORD **this, int a2)`
  - `sub_428712(a1: 0);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v74) = 1;`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `LOBYTE(v74) = 2;`
  - `if ( dword_BF14EC == 0 )`
  - `v3 = IWzGr2D::CreateLayer(`
  - `sub_428712(a1: *v3);`
  - `sub_4156C5(a1: &v70);`
  - `if ( v4 < 0 )`
  - `LOBYTE(v74) = 0;`
  - `if ( v5 < 0 )`
  - `if ( *v2 == 0 )`
  - `v7 = (*(**v2 + 224))(a1: *v2, a2: -1);`
  - `if ( v7 < 0 )`
  - …（截断 16 行）

### CFloatNotice（2 函数）


#### CFloatNotice::CreateFloatNotice  (15162 B)

- SP_ 节点：`SP_1300_UI_UIWINDOWIMG_FLOATNOTICE_D`, `SP_978_Y`
  - `void __thiscall CFloatNotice::CreateFloatNotice(CFloatNotice *this, const char *a2, int *a3, IWzGr2DLayer **a4, int *i)`
  - `LOBYTE(v97) = 1;`
  - `sub_416821(a1: &v88, a2: v7, a3);`
  - `LOBYTE(v97) = 0;`
  - `if ( Unknown != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v97) = 2;`
  - `sub_402F85(pvargDest: &v78, pvargSrc: &pvargSrc);`
  - `LOBYTE(v97) = 3;`
  - `v8 = sub_41468C(a1: &v88);`
  - `_bstr_t::_bstr_t(a1: v8);`
  - `v10 = sub_406472();`
  - `v11 = sub_402680(a1: v10);`
  - `IWzResMan::GetObjectA(this: v11, pvargDest: &pvarg, sUOL: v72, vParam: v73, vAux: p_pvargDest);`
  - `LOBYTE(v97) = 5;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v75, a2: v76);`
  - …（截断 16 行）

#### CFloatNotice::DeleteFloatNotice  (111 B)

  - `int __thiscall CFloatNotice::DeleteFloatNotice(CFloatNotice *this)`
  - `return sub_428712(a1: 0);`

### CSlideMenuDlg（3 函数）


#### CSlideMenuDlg::OnCreate  (10480 B)

- SP_ 节点：`SP_4505_UI_UIWINDOWIMG_SLIDEMENU_D`, `SP_4506_UI_UIWINDOWIMG_SLIDEMENU_D_BACKGRD`, `SP_4512_UI_UIWINDOWIMG_SLIDEMENU_D_CHOICE`, `SP_4513_UI_UIWINDOWIMG_SLIDEMENU_D_COVER`
  - `void __thiscall CSlideMenuDlg::OnCreate(_DWORD *this, int pData)`
  - `v47 = *(this + 33);`
  - `LOBYTE(v69) = 1;`
  - `v5 = sub_416821(a1: &v60, a2: v4, a3: v47);`
  - `ZXString<unsigned short>::operator=(a1: v5);`
  - `LOBYTE(v69) = 4;`
  - `if ( v60 != 0 )`
  - `LOBYTE(v69) = 3;`
  - `if ( v53 != 0 )`
  - `LOBYTE(v69) = 6;`
  - `v8 = sub_416821(a1: &v59, a2: v7, a3: v47);`
  - `ZXString<unsigned short>::operator=(a1: v8);`
  - `LOBYTE(v69) = 9;`
  - `if ( v59 != 0 )`
  - `LOBYTE(v69) = 8;`
  - `if ( v58 != 0 )`
  - …（截断 16 行）

#### CSlideMenuDlg::SetButtonInfo  (5663 B)

- SP_ 节点：`SP_4507_UI_UIWINDOWIMG_SLIDEMENU_D_BTMAIN_D`
  - `void __thiscall CSlideMenuDlg::SetButtonInfo(char *this, int sInfo)`
  - `ZXString<unsigned short>::Assign<char>(a1: sInfo, a2: -1);`
  - `ZXString<unsigned short>::Assign<char>(a1: dword_AF7B94, a2: -1);`
  - `sub_48EF51(a1: 0, a2: 0);`
  - `ZArray<CSlideMenuDlg::MAINBUTTONINFO>::InsertBefore(a1: -1);`
  - `sub_76C721(a1: &v23);`
  - `while ( 1 )`
  - `v6 = v5 != 0 ? *(v5 - 4) >> 1 : 0;`
  - `if ( v4 >= v6 - 1 )`
  - `v7 = *(v5 + 2 * v4) == 35;`
  - `LOBYTE(v33) = 5;`
  - `if ( v7 )`
  - `for ( i = v4 + 1; *(v5 + 2 * i) != 35; ++i )`
  - `v32 = *(v5 + 2 * i);`
  - `ZXString<char>::_Cat(a1: &v32, a2: 1u);`
  - `for ( j = (v5 + 2 * v4); *j != 35; ++j )`
  - …（截断 16 行）

#### CSlideMenuDlg::SetSlideMenuDlg  (1366 B)

  - `void __thiscall CSlideMenuDlg::SetSlideMenuDlg(char *this, int iPacket)`
  - `*(this + 33) = CInPacket::Decode4(this: iPacket);`
  - `CInPacket::DecodeStr(this: iPacket, a2: &v8);`
  - `ZXString<char>::operator=(a1: &v8);`
  - `CSlideMenuDlg::SetButtonInfo(this, sInfo: 0);`
  - `v3 = *(this + 31);`
  - `if ( *(v3 + 20) == -1 && (v3 == 0 || *(v3 - 4) != 2) )`
  - `sub_76C781(a1: v4);`
  - `LOBYTE(v9) = 1;`
  - `ZArray<CSlideMenuDlg::MAINBUTTONINFO>::InsertBefore(a1: -1);`
  - `sub_76C721(a1: &v5);`
  - `LOBYTE(v9) = 2;`
  - `LOBYTE(v9) = 0;`

### CUITitle（5 函数）


#### CUITitle::EnableLoginCtrl  (291 B)

  - `int __thiscall CUITitle::EnableLoginCtrl(CUITitle *this, int a2)`
  - `(*(*(*(this + 51) + 4) + 28))(a1: *(this + 51) + 4, a2);`
  - `(*(*(*(this + 53) + 4) + 28))(a1: *(this + 53) + 4, a2);`
  - `return (*(*(*(this + 37) + 4) + 28))(a1: *(this + 37) + 4, a2);`

#### CUITitle::IsRequestValid  (172 B)

  - `BOOL __thiscall CUITitle::IsRequestValid(CUITitle *this)`
  - `v1 = *(this + 32);`
  - `return *(v1 + 368) == 0 && *(v1 + 360) == 0;`

#### CUITitle::OnButtonClicked  (3453 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_2_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTPASSWORD`, `SP_5633_HTTP__MAPLESTORYNEXONNET`
  - `void __thiscall CUITitle::OnButtonClicked(CUITitle *this, unsigned int a2)`
  - `if ( a2 == 1000 )`
  - `if ( CUITitle::IsRequestValid(this) == 0 )`
  - `(*(*this + 52))(a1: this, a2: 1);`
  - `if ( a2 != 1001 )`
  - `switch ( a2 )`
  - `open_web_site(a1: v8, lpWindowName: 0);`
  - `open_web_site(a1: v11, lpWindowName: 0);`
  - `open_web_site(a1: v10, lpWindowName: 0);`
  - `open_web_site(a1: v9, lpWindowName: 0);`
  - `dword_BF041C(a1: 0);`
  - `if ( a2 == 1 )`
  - `else if ( a2 == 2 )`
  - `(*(*this + 52))(a1: this, a2: 2);`
  - `if ( CUITitle::IsRequestValid(this) != 0 )`
  - `sub_62113B(a1: this);`
  - …（截断 16 行）

#### CUITitle::OnCreate  (14026 B)

- SP_ 节点：`SP_1345_UI_LOGINIMG_TITLE_BTLOGIN`, `SP_1347_UI_LOGINIMG_TITLE_BTLOGINIDSAVE`, `SP_1348_UI_LOGINIMG_TITLE_BTLOGINIDLOST`, `SP_1349_UI_LOGINIMG_TITLE_BTPASSWDLOST`, `SP_1350_UI_LOGINIMG_TITLE_BTNEW`, `SP_1351_UI_LOGINIMG_TITLE_BTHOMEPAGE`, `SP_1352_UI_LOGINIMG_TITLE_BTQUIT`, `SP_1353_UI_LOGINIMG_TITLE_CHECK_0`, `SP_1354_UI_LOGINIMG_TITLE_CHECK_1`
  - `void __thiscall CUITitle::OnCreate(CUITitle *this, void *pData)`
  - `ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF);`
  - `*(this + 33) = *(v2 + 1);`
  - `v4 = *(v2 + 1) == 0;`
  - `LOBYTE(v85) = 1;`
  - `if ( !v4 )`
  - `v5 = *(v2 + 2);`
  - `if ( v5 != 0 && *v5 != 0 )`
  - `ZXString<char>::operator=(a1: v6);`
  - `LOBYTE(v85) = 2;`
  - `LOBYTE(v85) = 3;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `v9 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v85) = 4;`
  - `if ( v9 != 0 )`
  - `v10 = CCtrlButton::CCtrlButton(this: v9);`
  - …（截断 16 行）

#### CUITitle::SetRet  (4029 B)

  - `char *__thiscall CUITitle::SetRet(char **this, int a2)`
  - `result = CUITitle::IsRequestValid(this);`
  - `if ( result != 0 )`
  - `result = *(this + 32);`
  - `if ( *(result + 91) == 0 )`
  - `CCtrlEdit::GetText(a1: &v25);`
  - `LOBYTE(v26) = 1;`
  - `CCtrlEdit::GetText(a1: &v24);`
  - `LOBYTE(v26) = 2;`
  - `if ( a2 == 1 )`
  - `if ( v25 != 0 )`
  - `v5 = *(v25 - 1);`
  - `if ( v5 < 4`
  - `|| (v25 == 0 ? (v6 = 0) : (v6 = *(v25 - 1)),`
  - `&& ((v7 = v25, (v8 = strchr(a1: v25, a2: 64)) == 0) ? (v9 = -1) : (v9 = v8 - v7),`
  - `|| (v10 = v25, v25 != 0)`
  - …（截断 16 行）

### CUIAntiMacroCharacterName（1 函数）


#### CUIAntiMacroCharacterName::GetResult  (484 B)

  - `void __thiscall CUIAntiMacroCharacterName::GetResult(void *this, int a2)`
  - `CCtrlEdit::GetText(a1: &v3);`
  - `ZXString<char>::TrimRight(a1: 0);`
  - `v2 = ZXString<char>::TrimLeft(a1: 0);`
  - `ZXString<char>::operator=(a1: v2);`

### CAntiRepeat（1 函数）


#### CAntiRepeat::TryRepeat  (548 B)

  - `signed int __thiscall CAntiRepeat::TryRepeat(CAntiRepeat *this, int a2, int a3)`
  - `if ( v3 <= -6 || v3 >= 6 || (v4 = *(this + 1) - a3) <= -150 || v4 >= 150 )`
  - `*(this + 2) = 0;`
  - `*(this + 1) = a3;`
  - `v5 = *(this + 2);`
  - `if ( v5 >= *(this + 3) )`
  - `*(this + 2) = v5 + 1;`

### CScriptMan（17 函数）


#### CScriptMan::Init  (551 B)

  - `int __thiscall CScriptMan::Init(CScriptMan *this)`
  - `*(this + 1) = 0;`
  - `v2 = *(this + 3);`
  - `if ( v2 != 0 )`
  - `(*(*v2 + 52))(a1: v2, a2: 3);`
  - `v4 = *(this + 5);`
  - `if ( v4 != 0 )`
  - `(*(*v4 + 52))(a1: v4, a2: 3);`

#### CScriptMan::OnAskAvatar  (3589 B)

  - `void __thiscall CScriptMan::OnAskAvatar(CScriptMan *this, int a2, int a3, struct CInPacket *a4)`
  - `CInPacket::DecodeStr(this: a4, a2: &v26);`
  - `v5 = CInPacket::Decode1(this: a4);`
  - `if ( v5 != 0 )`
  - `v6 = CInPacket::Decode4(this: a4);`
  - `v7 = ZArray<long>::InsertBefore(a1: -1);`
  - `while ( !v8 );`
  - `v9 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `LOBYTE(v28) = 2;`
  - `if ( v9 != 0 )`
  - `v10 = CUtilDlgEx::CUtilDlgEx(this: v9);`
  - `LOBYTE(v28) = 1;`
  - `v23 = *(sub_582004(a1: v10) + 4);`
  - `sub_4289CD(a1: v22);`
  - `LOBYTE(v28) = 3;`
  - `sub_428211(a1: &v26);`
  - …（截断 16 行）

#### CScriptMan::OnAskBoxText  (3476 B)

  - `int __thiscall CScriptMan::OnAskBoxText(char *this, int a2, int a3, char *a4, int a5)`
  - `CInPacket::DecodeStr(this: a4, a2: &v29);`
  - `CInPacket::DecodeStr(this: v5, a2: &a4);`
  - `LOBYTE(v30) = 1;`
  - `v28 = CInPacket::Decode2(this: v5);`
  - `v7 = CInPacket::Decode2(this: v5);`
  - `v8 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `LOBYTE(v30) = 2;`
  - `if ( v8 != 0 )`
  - `v9 = CUtilDlgEx::CUtilDlgEx(this: v8);`
  - `v26 = *(sub_582004(a1: v9) + 4);`
  - `sub_4289CD(a1: v25);`
  - `LOBYTE(v30) = 3;`
  - `sub_428211(a1: &v29);`
  - `v12 = sub_428688(a1: v25);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v12, a2: 8, a3, a4: v20, a5: v21, a6: v22[0]);`
  - …（截断 16 行）

#### CScriptMan::OnAskMembershopAvatar  (3195 B)

  - `void __thiscall CScriptMan::OnAskMembershopAvatar(CScriptMan *this, int a2, int a3, struct CInPacket *a4)`
  - `CInPacket::DecodeStr(this: a4, a2: &v25);`
  - `v5 = CInPacket::Decode1(this: a4);`
  - `if ( v5 != 0 )`
  - `v6 = CInPacket::Decode4(this: a4);`
  - `v7 = ZArray<long>::InsertBefore(a1: -1);`
  - `while ( !v8 );`
  - `v9 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `LOBYTE(v28) = 2;`
  - `if ( v9 != 0 )`
  - `v10 = CUtilDlgEx::CUtilDlgEx(this: v9);`
  - `LOBYTE(v28) = 1;`
  - `v23 = *(sub_582004(a1: v10) + 4);`
  - `sub_4289CD(a1: v22);`
  - `LOBYTE(v28) = 3;`
  - `sub_428211(a1: &v25);`
  - …（截断 16 行）

#### CScriptMan::OnAskMenu  (2753 B)

  - `void __thiscall CScriptMan::OnAskMenu(CScriptMan *this, int a2, int a3, struct CInPacket *a4, int a5)`
  - `CInPacket::DecodeStr(this: a4, a2: &a4);`
  - `v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v6 != 0 )`
  - `v7 = CUtilDlgEx::CUtilDlgEx(this: v6);`
  - `LOBYTE(v24) = 0;`
  - `v22 = *(sub_582004(a1: v7) + 4);`
  - `sub_4289CD(a1: v21);`
  - `LOBYTE(v24) = 2;`
  - `sub_428211(a1: &a4);`
  - `v10 = sub_428688(a1: v21);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v10, a2: 4, a3, a4: v16, a5: v17, a6: v18);`
  - `v12 = (a5 & 6) == 0;`
  - `*(v22 + 440) = a5;`
  - `*(v11 + 441) = v13;`
  - `sub_9AC8E0(this: v22);`
  - …（截断 16 行）

#### CScriptMan::OnAskNumber  (3273 B)

  - `void __thiscall CScriptMan::OnAskNumber(CScriptMan *this, int a2, int a3, volatile LONG *a4, unsigned __int8 a5)`
  - `CInPacket::DecodeStr(this: a4, a2: &a4);`
  - `v26 = CInPacket::Decode4(this: v5);`
  - `v27 = CInPacket::Decode4(this: v5);`
  - `v7 = CInPacket::Decode4(this: v5);`
  - `v8 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v8 != 0 )`
  - `v9 = CUtilDlgEx::CUtilDlgEx(this: v8);`
  - `LOBYTE(v28) = 0;`
  - `v24 = *(sub_582004(a1: v9) + 4);`
  - `sub_4289CD(a1: v23);`
  - `LOBYTE(v28) = 2;`
  - `sub_428211(a1: &a4);`
  - `v12 = sub_428688(a1: v23);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v12, a2: 2, a3, a4: v18, a5: v19, a6: v20);`
  - `v14 = (a5 & 6) == 0;`
  - …（截断 16 行）

#### CScriptMan::OnAskPet  (4634 B)

  - `void __thiscall CScriptMan::OnAskPet(CScriptMan *this, int a2, int a3, struct CInPacket *a4)`
  - `CInPacket::DecodeStr(this: a4, a2: &v23);`
  - `v4 = CInPacket::Decode1(this: a4);`
  - `COutPacket::COutPacket(a1: 60);`
  - `LOBYTE(v25) = 1;`
  - `COutPacket::Encode1(this: &v14, a2: 9);`
  - `v5 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v17) + 4);`
  - `sub_4289B7(this: &v17);`
  - `LOBYTE(v25) = 2;`
  - `if ( v4 <= 0 )`
  - `COutPacket::Encode1(this: &v14, a2: 2);`
  - `for ( i = v4; i != 0; i = (i - 1) )`
  - `CInPacket::DecodeBuffer(this: a4, a2: &v17, a3: 8u);`
  - `CInPacket::Decode1(this: a4);`
  - `CashItemSlotPosition = CharacterData::FindCashItemSlotPosition(this: v5, a2: 5, a3: v17);`
  - `if ( CashItemSlotPosition > 0 )`
  - …（截断 16 行）

#### CScriptMan::OnAskPetAll  (4710 B)

  - `void __thiscall CScriptMan::OnAskPetAll(CScriptMan *this, int a2, int a3, struct CInPacket *a4)`
  - `CInPacket::DecodeStr(this: a4, a2: &v24);`
  - `v4 = CInPacket::Decode1(this: a4);`
  - `v21 = CInPacket::Decode1(this: a4);`
  - `COutPacket::COutPacket(a1: 60);`
  - `LOBYTE(v26) = 1;`
  - `COutPacket::Encode1(this: &v14, a2: 10);`
  - `v5 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v17) + 4);`
  - `sub_4289B7(this: &v17);`
  - `LOBYTE(v26) = 2;`
  - `if ( v4 > 0 )`
  - `for ( i = v4; i != 0; --i )`
  - `CInPacket::DecodeBuffer(this: a4, a2: &v17, a3: 8u);`
  - `CInPacket::Decode1(this: a4);`
  - `CashItemSlotPosition = CharacterData::FindCashItemSlotPosition(this: v5, a2: 5, a3: v17);`
  - `if ( CashItemSlotPosition > 0 )`
  - …（截断 16 行）

#### CScriptMan::OnAskQuiz  (138 B)

  - `int __thiscall CScriptMan::OnAskQuiz(CScriptMan *this, int a2, int a3, struct CInPacket *a4)`
  - `return sub_A26B09(a1: a4);`

#### CScriptMan::OnAskSlideMenu  (1657 B)

  - `int __thiscall CScriptMan::OnAskSlideMenu(CScriptMan *this, struct CInPacket *a2)`
  - `v3 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 204);`
  - `if ( v3 != 0 )`
  - `v4 = CSlideMenuDlg::CSlideMenuDlg(this: v3, iPacket: a2);`
  - `v11 = *(sub_747B46(a1: v4) + 4);`
  - `sub_747B9D(a1: v10);`
  - `v6 = CDialog::DoModal(a1: v11);`
  - `if ( v6 != 3 )`
  - `v14 = *(v11 + 136);`
  - `COutPacket::COutPacket(a1: 60);`
  - `LOBYTE(v13) = 2;`
  - `COutPacket::Encode1(this: &v8, a2: 14);`
  - `if ( v6 == 1 )`
  - `COutPacket::Encode1(this: &v8, a2: 1);`
  - `COutPacket::Encode4(this: &v8, a2: v14);`
  - `COutPacket::Encode1(this: &v8, a2: 0);`
  - …（截断 16 行）

#### CScriptMan::OnAskSpeedQuiz  (143 B)

  - `int __thiscall CScriptMan::OnAskSpeedQuiz(CScriptMan *this, int a2, int a3, struct CInPacket *a4)`
  - `return sub_A26C66(a1: a4);`

#### CScriptMan::OnAskText  (3591 B)

  - `int __thiscall CScriptMan::OnAskText(char *this, int a2, int a3, char *a4, int a5)`
  - `CInPacket::DecodeStr(this: a4, a2: &v30);`
  - `CInPacket::DecodeStr(this: v5, a2: &a4);`
  - `LOBYTE(v31) = 1;`
  - `v29 = CInPacket::Decode2(this: v5);`
  - `v7 = CInPacket::Decode2(this: v5);`
  - `v8 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `LOBYTE(v31) = 2;`
  - `if ( v8 != 0 )`
  - `v9 = CUtilDlgEx::CUtilDlgEx(this: v8);`
  - `v27 = *(sub_582004(a1: v9) + 4);`
  - `sub_4289CD(a1: v26);`
  - `LOBYTE(v31) = 3;`
  - `sub_428211(a1: &v30);`
  - `v12 = sub_428688(a1: v26);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v12, a2: 3, a3, a4: v21, a5: v22, a6: v23[0]);`
  - …（截断 16 行）

#### CScriptMan::OnAskYesNo  (2543 B)

  - `void __thiscall CScriptMan::OnAskYesNo(`
  - `CInPacket::DecodeStr(this: a4, a2: &a4);`
  - `v8 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v8 != 0 )`
  - `v9 = CUtilDlgEx::CUtilDlgEx(this: v8);`
  - `LOBYTE(v25) = 0;`
  - `v23 = *(sub_582004(a1: v9) + 4);`
  - `sub_4289CD(a1: v22);`
  - `LOBYTE(v25) = 2;`
  - `sub_428211(a1: &a4);`
  - `v12 = sub_428688(a1: v22);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v12, a2: 1, a3, a4: v17, a5: v18, a6: v19);`
  - `*(v23 + 440) = a5;`
  - `*(v14 + 441) = (v13 & 6) != 0;`
  - `CUtilDlgEx::SetUtilDlgEx_YESNO(a1: v23);`
  - `CUtilDlgEx::CreateUtilDlgEx(this: v23);`
  - …（截断 16 行）

#### CScriptMan::OnPacket  (180 B)

  - `void __thiscall CScriptMan::OnPacket(CScriptMan *this, int a2, struct CInPacket *a3)`
  - `if ( a2 == 304 )`
  - `CScriptMan::OnScriptMessage(this, a2: a3);`

#### CScriptMan::OnSay  (2870 B)

  - `void __thiscall CScriptMan::OnSay(CScriptMan *this, int a2, int a3, struct CInPacket *a4, unsigned __int8 a5)`
  - `if ( (a5 & 4) != 0 )`
  - `a3 = CInPacket::Decode4(this: a4);`
  - `CInPacket::DecodeStr(this: v5, a2: &a4);`
  - `v26 = CInPacket::Decode1(this: v5);`
  - `v7 = CInPacket::Decode1(this: v5);`
  - `v8 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v8 != 0 )`
  - `v9 = CUtilDlgEx::CUtilDlgEx(this: v8);`
  - `LOBYTE(v27) = 0;`
  - `v24 = *(sub_582004(a1: v9) + 4);`
  - `sub_4289CD(a1: v23);`
  - `LOBYTE(v27) = 2;`
  - `sub_428211(a1: &a4);`
  - `v12 = sub_428688(a1: v23);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v12, a2: 0, a3, a4: v18, a5: v19, a6: v20);`
  - …（截断 16 行）

#### CScriptMan::OnSayImage  (2218 B)

  - `int __stdcall CScriptMan::OnSayImage(int a1)`
  - `v1 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v1 != 0 )`
  - `v2 = CUtilDlgEx::CUtilDlgEx(this: v1);`
  - `v13 = v3 != 0 ? (v3 - 8) : 0;`
  - `sub_4289CD(a1: v12);`
  - `ZXString<char>::ZXString<char>(this: &v9, a2: &sStrDefault, a3: 0xFFFFFFFF);`
  - `LOBYTE(v17) = 1;`
  - `v5 = sub_428688(a1: v12);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v5, a2: 9, a3: 0, a4: v9, a5: v10, a6: v11);`
  - `v6 = CInPacket::Decode1(this: a1);`
  - `if ( v6 != 0 )`
  - `for ( i = v6; i != 0; i = (i - 1) )`
  - `CInPacket::DecodeStr(this: a1, a2: &v16);`
  - `LOBYTE(v17) = 3;`
  - `if ( v16 != 0 && *v16 != 0 )`
  - …（截断 16 行）

#### CScriptMan::OnScriptMessage  (2654 B)

- 分支 case：`0`, `1`, `2`, `3`, `4`, `5`, `6`, `7`, `8`, `9`, `10`, `12`, `13`, `14`
  - `void __thiscall CScriptMan::OnScriptMessage(CScriptMan *this, struct CInPacket *a2)`
  - `if ( *(this + 1) != 0 )`
  - `_CxxThrowException(a1: iPacket, a2: &_TI3_AVCDisconnectException__);`
  - `*(this + 1) = 1;`
  - `v4 = CInPacket::Decode1(this: a2);`
  - `v7 = CInPacket::Decode4(this: a2);`
  - `nMsgType = CInPacket::Decode1(this: a2);`
  - `LOBYTE(iPacket[0]) = CInPacket::Decode1(this: a2);`
  - `switch ( nMsgType )`
  - `CScriptMan::OnSay(this, a2: v4, a3: v7, a4: a2, a5: iPacket[0]);`
  - `CScriptMan::OnAskYesNo(this, a2: v4, a3: v7, a4: a2, a5: iPacket[0], a6: 0, a7: 0);`
  - `CScriptMan::OnAskText(this, a2: v4, a3: v7, a4: a2, a5: iPacket[0]);`
  - `CScriptMan::OnAskNumber(this, a2: v4, a3: v7, a4: a2, a5: iPacket[0]);`
  - `CScriptMan::OnAskMenu(this, a2: v4, a3: v7, a4: a2, a5: iPacket[0]);`
  - `CScriptMan::OnAskQuiz(this, a2: v4, a3: v7, a4: a2);`
  - `CScriptMan::OnAskSpeedQuiz(this, a2: v4, a3: v7, a4: a2);`
  - …（截断 16 行）

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

### CChatBalloon（3 函数）


#### CChatBalloon::GetMiniRoomBalloonRect  (2521 B)

  - `int __thiscall CChatBalloon::GetMiniRoomBalloonRect(CChatBalloon *this, struct tagRECT *a2)`
  - `if ( *(this + 1) == 0 )`
  - `return dword_BF0410(a1: a2);`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `v4 = *(this + 1) == 0;`
  - `LOBYTE(v21) = 1;`
  - `if ( v4 )`
  - `Canvas = IWzGr2DLayer::GetCanvas(a1: &v18, a2: &v16);`
  - `LOBYTE(v21) = 2;`
  - `v6 = IWzGr2DLayer::GetCanvas(a1: v19, a2: &pvarg);`
  - `LOBYTE(v21) = 3;`
  - `v14 = sub_40B947(a1: v20);`
  - `v8 = sub_40B920(a1: v7);`
  - `dword_BF040C(a1: a2, a2: 0, a3: 0, a4: v8, a5: v14);`
  - `sub_4156AB(a1: v19);`
  - `if ( v18 != 0 )`
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

## 3. 坐标 / 尺寸 / 按钮 id / 封包字段候选行（活动小游戏全函数）


### CChatBalloon::MakeMiniRoomBalloon
  - `a9 = Ztl_variant_t::GetUnknown(a1: v285[8], a2: v285[12]);`

### CField_Battlefield::OnClock
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 224) != 0 )`

### CField_Coconut::Init
  - `memset(a1: this + 2064, a2: 0, a3: 0x340u);`

### CField_Dojang::OnClock
  - `*(this + 529) = 1000 * *(this + 530) + timeGetTime();`

### CField_Dojang::Update
  - `&& (v76 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &pvargDest.lVal) + 4) != 0,`

### CField_Dojang::UpdateTimer
  - `v2 = (*(this + 529) - timeGetTime()) / 1000;`
  - `CField_Dojang::DrawDigit(this, a2: 0, a3: 0, a4: v2 / 60);`
  - `CField_Dojang::DrawDigit(this, a2: 68, a3: 0, a4: *(this + 530) % 60);`

### CField_LimitedView::Init
  - `PcCreateObject::IWzCanvas(a1: v40, a2: v67 + 508, a3: 0);`

### CField_Massacre::Init
  - `*(this + 524) = get_int32(pvarSrc: Item, a2: v151) != 0;`

### CField_MassacreResult::OnMassacreResult
  - `v60 = StringPool::GetStringW(this: v59, a2: v73[0], a3: SHIDWORD(v73[0]));`

### CField_MonsterCarnival::CreateUIWindow
  - `CUIMonsterCarnival::SetUIData(a1: 0, a2: v1, a3: v17, a4: v18, a5: v19[0]);`

### CField_PartyRaidResult::Init
  - `v32[0] = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 72);`

### CInPacket::WriteClientLog
  - `Buffer = ZXString<char>::GetBuffer(a1: 0, a2: 1);`

### CMemoryGameDlg::OnKey
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: *(this + 114) != 0 ? (*(this + 114) + 4) : 0);`

### CMiniRoomBaseDlg::OnInviteStatic
  - `if ( (TSecType<unsigned char>::GetData(this: v1 + 2060) & 1) != 0 && sub_4F38B4(this: v1) )`

### CNoticeQuestProgress::OnQuestProgressUpdated_Item
  - `ItemCount = CWvsContext::GetItemCount(a1: *(*(v4 + 92) + 8 * i));`
  - `if ( *(v12 + 3) + 3000 >= get_update_time() && usQuestID == *v12 && nItemID == *(v12 + 1) )`

### CNoticeQuestProgress::OnQuestProgressUpdated_Mob
  - `if ( *(v36 + 3) + 3000 >= get_update_time() && a2 == *v36 )`

### COmokDlg::OnKey
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: *(this + 114) != 0 ? (*(this + 114) + 4) : 0);`

### CQuest::OnCompleteQuestFailed
  - `Data = TSecType<long>::GetData(a1: v32 + 12, a2: 0);`
  - `v165 = (CNpcTemplate::GetNpcTemplate(a1: *(v216 + 4)) + 148);`
  - `CUtilDlgEx::SetUtilDlgEx_TEXT(this: v214, a2: v116 > 0, a3: v116 < (v123 - 1));`
  - `CUtilDlgEx::SetUtilDlgEx_TEXT(this: v214, a2: 0, a3: 0);`

### CQuest::SayNormal
  - `CUtilDlgEx::SetUtilDlgEx_TEXT(this: v31, a2: v34 > 0, a3: v34 < (v11 - 1));`

### CQuestMan::CheckCompleteDemand
  - `if ( v13 != 0 && TSecType<long>::GetData(a1: v13 + 12, a2: 0) == *v10 )`
  - `i = TSecType<long>::GetData(a1: (ActiveItemSlotPet + 12), a2: 0);`

### CQuestMan::CheckStartDemand
  - `if ( v30 != 0 && TSecType<long>::GetData(a1: v30 + 12, a2: a1) == *v27 )`

### CRPSGameDlg::OnPacket
  - `CUtilDlgEx::SetUtilDlgEx_TEXT(this: v15, a2: 0, a3: 0);`

### CScriptMan::OnAskBoxText
  - `CUtilDlgEx::SetUtilDlgEx(this: v12, a2: 8, a3, a4: v20, a5: v21, a6: v22[0]);`

### CScriptMan::OnAskPet
  - `v7 = *(CharacterData::GetItem(this: v5, a2: v16, a3: 5, a4: CashItemSlotPosition) + 4);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v11, a2: 6, a3, a4: v13, a5: 0, a6: 0);`

### CScriptMan::OnAskPetAll
  - `v7 = *(CharacterData::GetItem(this: v5, a2: v16, a3: 5, a4: CashItemSlotPosition) + 4);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v11, a2: 6, a3, a4: v13, a5: 0, a6: 0);`

### CScriptMan::OnAskText
  - `CUtilDlgEx::SetUtilDlgEx(this: v12, a2: 3, a3, a4: v21, a5: v22, a6: v23[0]);`

### CScriptMan::OnAskYesNo
  - `COutPacket::Encode1(this: &v20, a2: a7 == 0 ? 1 : 12);`

### CScriptMan::OnSayImage
  - `CUtilDlgEx::SetUtilDlgEx(this: v5, a2: 9, a3: 0, a4: v9, a5: v10, a6: v11);`
  - `CUtilDlgEx::SetUtilDlgEx_IMAGE(this: v13, a2: 0, a3: v7 > 1);`

### CSlideNotice::OnCreate
  - `v4 = (*(**Canvas + 140))(a1: *Canvas, a2: 0, a3: 0, a4: 800, a5: 600, a6: 0x80000000);`

### CUICashItemGachapon::Update
  - `ItemIcon = CItemInfo::GetItemIcon(a1: &p_lVal, a2: v41.decVal.Hi32, bPetDead: 0, bWithShadow: 0);`

### CUIEnchantDlg::MakeClone
  - `Data = TSecType<long>::GetData(a1: (v3 + 12), a2: v32 + 12);`

### CUIEnchantDlg::RefreshToolTip
  - `CharacterData::GetItem(this: v21, a2: v29, a3: *(this + 36), a4: *(this + 37));`

### CUIEventAlarm::CreateEventAlarm
  - `return CDialog::CreateDlg(this, a2: v2, a3: v3, a4: v1, a5: v4, a6: 10, a7: 1, a8: 0);`

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

### CUIMonsterBook::LoadMobInfo
  - `Data = TSecType<long>::GetData(a1: (v118 + 12), a2: 0);`

### CUIMonsterCarnival::DrawListTooltip
  - `CUIToolTip::SetToolTip_String(this: (this + 1608), a2: v5 + a2 + 20, a3: v6, a4: y);`

### CUIMonsterCarnival::DrawLockTooltip
  - `CUIToolTip::SetToolTip_String(this: (this + 1608), a2: v9 + a3 + 20, a3: v11, a4: y);`

### CUIMonsterCarnival::FuncKeyProcess
  - `result = CTabList::GetSelectItemIdx(this: *(this + *(this + 376) + 386), a2);`

### CUIMonsterCarnival::OnCreate
  - `PcCreateObject::IWzFont(sUOL: v121, pObj: this + 391, pUnkOuter: 0);`
  - `CCtrlTab::SetTab(this: *(this + 366), a2: *(this + 376));`

### CUIMonsterCarnival::OnMouseButton
  - `SelectItemIdx = CTabList::GetSelectItemIdx(this: *(this + *(this + 375) + 385), a2: __PAIR64__(v7, v5));`
  - `CWnd::InvalidateRect(this: (this - 4), a2: 0);`

### CUIMonsterCarnival::OnMouseMove
  - `if ( CUIMonsterCarnival::DrawCPTooltip(this: (this - 4), a2: a2.x, a3: a2.y, a4: a2) == 0 )`

### CUIMonsterCarnival::RequestSend
  - `COutPacket::Encode4(this: &v6, a2: *(this + 727) - 1);`

### CUINewYearCardSenderDlg::OnCreate
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 24) != 0 )`
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 45), a2: 100);`

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

### CVecCtrlGrenade::WorkUpdateActive
  - `if ( CVecCtrl::CollisionDetectFloat(this, a2: v4, a3: &a2, a4: 1) != 0 && a2 > 0 )`

### UMedalQuestInfo::0?$ZRef
  - `return InterlockedIncrement(lpAddend: (*(this + 1) != 0 ? result - 16 + 4 : 4));`
