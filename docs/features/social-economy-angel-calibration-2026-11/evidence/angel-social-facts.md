# Angel 083 社交经济面反编译语义抽取（观察层）

来源：`evidence/angel-social-decompile-full.json`（逐字 Hex-Rays 伪代码，超大函数从 `/output/` 端点补全并归一化）。
本文件由 `tools/ida_angel_social_facts.py` 自动生成，只做字符串抽取，不含推断。

## 1. StringPool 资源节点全集（347 个）

- `SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK` ← CCashShop, CCashTradingRoomDlg, CChatBalloon, CPersonalShopDlg, CShopDlg, CTabFriend…
- `SP_100_MAPLESTORY_ERROR__LOADING_REACTOR_FAILED` ← CCashTradingRoomDlg, CPersonalShopDlg
- `SP_104_MAPLESTORY_ERROR__SPRITE_INIT_FAILEDUNKNOWN` ← CUIGuildBBS
- `SP_1253_UI_BASICIMG_ITEMNO` ← CAdminShopDlg, CShopDlg, CTradingRoomDlg, CTrunkDlg
- `SP_1260_UI_BASICIMG_BTCLAIM` ← CTradingRoomDlg, CUIMessenger
- `SP_1261_UI_BASICIMG_BTMIN` ← CUIMessenger
- `SP_1262_UI_BASICIMG_BTMAX` ← CUIMessenger
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
- `SP_1410_CANVASFONT` ← CChatBalloon, CUIMessenger, CUIUserList
- `SP_1411_BA` ← CChatBalloon
- `SP_1412_B` ← CUIUserList
- `SP_1414_UI_CHATBALLOONIMG_MINIROOM` ← CChatBalloon
- `SP_1416_OMOK` ← CChatBalloon
- `SP_1421_CNUM` ← CChatBalloon
- `SP_1422_MNUM` ← CChatBalloon
- `SP_1425_PROGRESS` ← CChatBalloon
- `SP_145__INVITE` ← CUIMessenger
- `SP_1542_UI_UIWINDOWIMG_SHOP_SELECT` ← CAdminShopDlg, CShopDlg
- `SP_1543_UI_UIWINDOWIMG_SHOP_MESO` ← CShopDlg
- `SP_1544_UI_UIWINDOWIMG_SHOP_TOKEN` ← CShopDlg
- `SP_1547_UI_UIWINDOWIMG_SHOP_BTBUY` ← CAdminShopDlg, CShopDlg
- `SP_1548_UI_UIWINDOWIMG_SHOP_BTSELL` ← CAdminShopDlg, CShopDlg
- `SP_1593_MOB` ← CChatBalloon
- `SP_1666_STRING_NPCIMG` ← CField_Wedding
- `SP_1818_UI_UIWINDOWIMG_TRADINGROOM_BTTRADE` ← CTradingRoomDlg
- `SP_1819_UI_UIWINDOWIMG_TRADINGROOM_BTCOIN` ← CTradingRoomDlg
- `SP_1820_UI_UIWINDOWIMG_TRADINGROOM_BTRESET` ← CTradingRoomDlg
- `SP_1822_UI_UIWINDOWIMG_TRUNK_EN` ← CTrunkDlg
- `SP_1823_UI_UIWINDOWIMG_TRUNK_BTGET` ← CTrunkDlg
- `SP_1824_UI_UIWINDOWIMG_TRUNK_BTPUT` ← CTrunkDlg
- `SP_1825_UI_UIWINDOWIMG_TRUNK_BTSORT` ← CTrunkDlg
- `SP_1847_DRAGSTART` ← CPersonalShopDlg
- `SP_1858_UI_UIWINDOWIMG_MESSENGER_BLINK` ← CUIMessenger
- `SP_1859__________________________` ← CUIMessenger
- `SP_1860_UI_UIWINDOWIMG_MESSENGER_CHATBALLOON` ← CUIMessenger
- `SP_1861_UI_UIWINDOWIMG_MESSENGER_BACKGRND` ← CUIMessenger
- `SP_1867_UI_UIWINDOW_MESSENGER_NAMEBAR_D` ← CUIMessenger
- `SP_1868__Q` ← CUIMessenger
- `SP_1869__Q` ← CUIMessenger
- `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID` ← CUIFamilyChart, CUIMessenger, CUIUserList
- `SP_2051_UI_UIWINDOWIMG_USERINFO_BACKGRND` ← CUIUserList
- `SP_2052_UI_UIWINDOWIMG_USERINFO_BACKGRND2` ← CUIUserList
- `SP_2053_UI_UIWINDOWIMG_USERINFO_BACKGRND3` ← CUIUserList
- `SP_2054_UI_UIWINDOWIMG_USERINFO_BACKGRND4` ← CUIUserList
- `SP_2055_UI_UIWINDOWIMG_USERINFO_BACKGRND5` ← CUIUserList
- `SP_2056_UI_UIWINDOWIMG_USERINFO_BACKGRND6` ← CUIUserList
- `SP_2057_UI_UIWINDOWIMG_USERINFO_BACKGRND7` ← CUIUserList
- `SP_2081_D_D` ← CTabBlackList, CTabFriend, CTabParty
- `SP_2082__D_D_` ← CTabGuild
- `SP_2090_UI_UIWINDOWIMG_USERLIST_BACKGRND` ← CUIUserList
- `SP_2097_UI_UIWINDOWIMG_USERLIST_FRIEND_BTMOD` ← CUIUserList
- `SP_2098_UI_UIWINDOWIMG_USERLIST_FRIEND_BTDELETE` ← CUIUserList
- `SP_2099_UI_UIWINDOWIMG_USERLIST_FRIEND_BTADDFRIEND` ← CUIUserList
- `SP_2100_UI_UIWINDOWIMG_USERLIST_FRIEND_BTADDGROUP` ← CUIUserList
- `SP_2101_UI_UIWINDOWIMG_USERLIST_FRIEND_BTWHISPER` ← CUIUserList
- `SP_2102_UI_UIWINDOWIMG_USERLIST_FRIEND_BTGROUPWHISPER` ← CUIUserList
- `SP_2103_UI_UIWINDOWIMG_USERLIST_FRIEND_BTBLOCK` ← CTabFriend, CUIUserList
- `SP_2104_UI_UIWINDOWIMG_USERLIST_FRIEND_BTUNBLOCK` ← CTabFriend
- `SP_2105_UI_UIWINDOWIMG_USERLIST_FRIEND_BTCHAT` ← CUIUserList
- `SP_2106_UI_UIWINDOWIMG_USERLIST_FRIEND_BTPARTY` ← CUIUserList
- `SP_2107_UI_UIWINDOWIMG_USERLIST_FRIEND_BTMESSAGE` ← CUIUserList
- `SP_2109_UI_UIWINDOWIMG_USERLIST_FRIEND_BTSHOWONLINE` ← CUIUserList
- `SP_2110_UI_UIWINDOWIMG_USERLIST_PARTY_BTCREATE` ← CUIUserList
- `SP_2111_UI_UIWINDOWIMG_USERLIST_PARTY_BTINVITE` ← CUIUserList
- `SP_2112_UI_UIWINDOWIMG_USERLIST_PARTY_BTKICK` ← CUIUserList
- `SP_2113_UI_UIWINDOWIMG_USERLIST_PARTY_BTWITHDRAW` ← CUIUserList
- `SP_2114_UI_UIWINDOWIMG_USERLIST_PARTY_BTWHISPER` ← CUIUserList
- `SP_2115_UI_UIWINDOWIMG_USERLIST_PARTY_BTCHAT` ← CUIUserList
- `SP_2116_UI_UIWINDOWIMG_USERLIST_PARTY_BTHP` ← CUIUserList
- `SP_2117_UI_UIWINDOWIMG_USERLIST_PARTY_BTSEARCH` ← CUIUserList
- `SP_2144_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTINVITE` ← CUIUserList
- `SP_2145_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTKICK` ← CUIUserList
- `SP_2146_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTWITHDRAW` ← CUIUserList
- `SP_2147_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTWHISPER` ← CUIUserList
- `SP_2148_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTCHAT` ← CUIUserList
- `SP_2149_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTINFO` ← CUIUserList
- `SP_2150_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTUP` ← CUIUserList
- `SP_2151_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTDOWN` ← CUIUserList
- `SP_2152_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTNOTICE` ← CUIUserList
- `SP_2153_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTPARTYINVITE` ← CUIUserList
- `SP_2165_UI_UIWINDOWIMG_USERLIST_BLACKLIST_BTADD` ← CUIUserList
- `SP_2166_UI_UIWINDOWIMG_USERLIST_BLACKLIST_BTDELETE` ← CUIUserList
- `SP_2302_MENUUP` ← CUIAdminShopWishList
- `SP_25_HERO` ← CTabParty
- `SP_2615_UI_UIWINDOWIMG_PERSONALSHOP_BTEXIT` ← CShopDlg
- `SP_2620_UI_UIWINDOWIMG_PERSONALSHOP_BACKGRND3` ← CPersonalShopDlg
- `SP_2670_UI_UIWINDOWIMG_USERLIST_TAB_ENABLED` ← CUIUserList
- `SP_2671_UI_UIWINDOWIMG_USERLIST_TAB_DISABLED` ← CUIUserList
- `SP_2676_UI_UIWINDOWIMG_SHOP_TABSELL_ENABLED` ← CAdminShopDlg, CShopDlg
- `SP_2677_UI_UIWINDOWIMG_SHOP_TABSELL_DISABLED` ← CAdminShopDlg, CShopDlg
- `SP_2678_UI_UIWINDOWIMG_SHOP_TABBUY_ENABLED` ← CAdminShopDlg, CShopDlg
- `SP_2679_UI_UIWINDOWIMG_SHOP_TABBUY_DISABLED` ← CAdminShopDlg, CShopDlg
- `SP_2722_UI_UIWINDOWIMG_MESSENGER_BTENTER` ← CTradingRoomDlg, CUIMessenger
- `SP_272_CURSING_IS_NOT_ALLOWED` ← CChatHelper
- `SP_2854_UI_UIWINDOWIMG_TRUNK_SELECT` ← CTrunkDlg
- `SP_2856_UI_UIWINDOW_TRUNK_BTOUTCOIN` ← CTrunkDlg
- `SP_2857_UI_UIWINDOW_TRUNK_BTINCOIN` ← CTrunkDlg
- `SP_2859_UI_UIWINDOWIMG_TRUNK_TAB_ENABLED` ← CTrunkDlg
- `SP_2860_UI_UIWINDOWIMG_TRUNK_TAB_DISABLED` ← CTrunkDlg
- `SP_2_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTPASSWORD` ← CUIFamilyChart
- `SP_323_YOU_ARE_NOT_THE_MASTER_OF_THE_PARTY` ← TabPartySearch
- `SP_3265_MEMBER` ← CWndAllianceGrade, CWndGuildGrade
- `SP_3296_IT_IS_SAVED` ← CWndAllianceGrade, CWndGuildGrade
- `SP_3299_PLEASE_PUT_A_POSITION_R_N_WHICH_YOU_WANT_TO_CHANGE_TO` ← CWndAllianceGrade, CWndGuildGrade
- `SP_3300_YOU_CANNOT_DELETE_THIS_POSITION_BECAUSE_R_NTHERE_IS_A_LOWER_RANK_THAN_THIS_POSIT` ← CWndAllianceGrade, CWndGuildGrade
- `SP_3301_YOU_CANNOT_DELETE_THIS_POSITION_R_NBECAUSE_THERE_ARE_STILL_MEMBERS_R_NWHO_ARE_IN` ← CWndGuildGrade
- `SP_3302_WOULD_LIKE_TO_SAVE_THE_CHANGE` ← CWndAllianceGrade, CWndGuildGrade
- `SP_3342_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTWHERE` ← CUIUserList
- `SP_3348__GUILD_NOTICE` ← CTabGuild
- `SP_3416_SEALED_ITEMS_CANNOT_BE_R_NSOLD_TRADED_OR_DROPPED` ← CAdminShopDlg
- `SP_3452_ITEM_CASH_0503IMG` ← CChatBalloon
- `SP_3458_SIGNBOARD` ← CChatBalloon
- `SP_3468_THE_OWNER_OF_THE_STORE_IS_CURRENTLY_R_NUNDERGOING_STORE_MAINTENANCE_R_NPLEASE_TR` ← CMiniRoomBaseDlg, CUIShopScanResult
- `SP_3475_YOU_MAY_NOT_OPEN_THE_STORE_NEAR_A_PORTAL` ← CMiniRoomBaseDlg
- `SP_3483_YOU_MAY_ONLY_SELL_ONE_OF_THIS_ITEM` ← CPersonalShopDlg
- `SP_3484_YOU_MAY_ONLY_POSSESS_ONE_OF_THIS_ITEM` ← CPersonalShopDlg
- `SP_350_YOU_ARE_NOT_IN_THE_GUILD` ← CTabGuild
- `SP_3580_WILL_YOU_EXIT_THIS_CHAT_ROOM` ← CUIMessenger
- `SP_3628__S__R_NWOULD_YOU_LIKE_TO_REGISTER` ← CUIAdminShopWishListSearchResult
- `SP_3640_THIS_STORE_IS_OPEN_AT_ROOM_D_ON_CHANNEL_S` ← CUIShopScanResult
- `SP_3641_THIS_CAN_ONLY_BE_USED_INSIDE_THE_FREE_MARKET` ← CUIShopScanResult
- `SP_3650__ITEM_PER_BUNDLE` ← CUIShopScanResult
- `SP_3651_PRICE_PER_BUNDLE` ← CUIShopScanResult
- `SP_3652_TOTAL__SOLD` ← CUIShopScanResult
- `SP_366_UNABLE_TO_FIND_THE_CHARACTER` ← CMiniRoomBaseDlg
- `SP_3722_ARE_YOU_SURE_YOU_WANT_TO_DELETE_THE_POST` ← CUIGuildBBS
- `SP_3723_PLEASE_DELETE_THE_CURRENT_NOTICE_R_NAND_THEN_TRY_AGAIN` ← CUIGuildBBS
- `SP_3724_UI_GUILDBBSIMG_GUILDBBS_BTQUIT` ← CUIGuildBBS
- `SP_3725_UI_GUILDBBSIMG_GUILDBBS_EMOTICON_BASIC_0` ← CUIGuildBBS
- `SP_3726_UI_GUILDBBSIMG_GUILDBBS_EMOTICON_BASIC_1` ← CUIGuildBBS
- `SP_3727_UI_GUILDBBSIMG_GUILDBBS_EMOTICON_BASIC_2` ← CUIGuildBBS
- `SP_3728_UI_GUILDBBSIMG_GUILDBBS_EMOTICON_SELECT` ← CUIGuildBBS
- `SP_3729_UI_GUILDBBSIMG_GUILDBBS_BTNOTICE` ← CUIGuildBBS
- `SP_3730_UI_GUILDBBSIMG_GUILDBBS_BTWRITE` ← CUIGuildBBS
- `SP_3731_UI_GUILDBBSIMG_GUILDBBS_BTRETOUCH` ← CUIGuildBBS
- `SP_3732_UI_GUILDBBSIMG_GUILDBBS_BTDELETE` ← CUIGuildBBS
- `SP_3733_UI_GUILDBBSIMG_GUILDBBS_BTREPLY` ← CUIGuildBBS
- `SP_3734_UI_GUILDBBSIMG_GUILDBBS_BTREPLYDELETE` ← CUIGuildBBS
- `SP_3735_UI_GUILDBBSIMG_GUILDBBS_BACKGRND2` ← CUIGuildBBS
- `SP_3736_UI_GUILDBBSIMG_GUILDBBS_BTREGISTER` ← CUIGuildBBS
- `SP_3737_UI_GUILDBBSIMG_GUILDBBS_BTCANCEL` ← CUIGuildBBS
- `SP_3738_UI_GUILDBBSIMG_GUILDBBS_MOVEEMOTICON_BTLEFT` ← CUIGuildBBS
- `SP_3739_UI_GUILDBBSIMG_GUILDBBS_MOVEEMOTICON_BTRIGHT` ← CUIGuildBBS
- `SP_3740_UI_GUILDBBSIMG_GUILDBBS_BACKGRND3` ← CUIGuildBBS
- `SP_3741_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTGUILDBBS` ← CUIUserList
- `SP_3743_THIS_MESSAGE_HAD_BEEN_DELETED_BY_ANOTHER_USER_R_NPLEASE_CHECK_AGAIN` ← CUIGuildBBS
- `SP_3744_UI_GUILDBBSIMG_GUILDBBS_EMOTICON_CASH_D` ← CUIGuildBBS
- `SP_3855_UNKNOWN` ← CTabGuild, CTabGuildAlliance
- `SP_3899_SUCCESSFULLY_DELETED` ← CParcelDlg
- `SP_3900_SUCCESSFULLY_CLAIMED` ← CParcelDlg
- `SP_3902_A_NEW_PACKAGE_HAS_BEEN_SENT` ← CParcelDlg
- `SP_3931_IT_HAS_EXCEEDED_THE_LIMIT_THAT_SYSTEM_ALLOWS` ← CPersonalShopDlg
- `SP_394_THE_ROOM_IS_ALREADY_CLOSED` ← CMiniRoomBaseDlg, CUIShopScanResult
- `SP_395_YOU_CANT_ENTER_THE_ROOM_DUE_TO_FULL_CAPACITY` ← CMiniRoomBaseDlg, CUIShopScanResult
- `SP_396_OTHER_REQUESTS_ARE_BEING_FULFILLED_THIS_MINUTE` ← CMiniRoomBaseDlg, CUIShopScanResult
- `SP_3977_PLAYERS_THAT_ARE_LEVEL_15_AND_BELOW__R_NMAY_ONLY_TRADE_1_MILLION_MESOS_PER_DAY__` ← CPersonalShopDlg, CShopDlg, CTradingRoomDlg
- `SP_397_YOU_CANT_DO_IT_WHILE_YOURE_DEAD` ← CMiniRoomBaseDlg, CUIShopScanResult
- `SP_398_YOU_CANT_DO_IT_IN_THE_MIDDLE_OF_THE_EVENT` ← CMiniRoomBaseDlg
- `SP_399_THIS_CHARACTER_IS_UNABLE_TO_DO_IT` ← CMiniRoomBaseDlg, CUIShopScanResult
- `SP_400_YOU_ARE_NOT_ALLOWED_TO_TRADE_OTHER_ITEMS_AT_THIS_POINT` ← CMiniRoomBaseDlg, CUIShopScanResult
- `SP_401_YOU_CANT_ESTABLISH_A_MINIROOM_RIGHT_HERE` ← CMiniRoomBaseDlg
- `SP_402_YOU_CAN_ONLY_MAKE_TRADE_R_NIF_YOU_TWO_ARE_IN_THE_SAME_MAP` ← CMiniRoomBaseDlg
- `SP_403_S_IS_DOING_SOMETHING_ELSE_RIGHT_NOW` ← CMiniRoomBaseDlg
- `SP_4044_UI_UIWINDOWIMG_USERLIST_PARTY_BTCHANGEBOSS` ← CUIUserList
- `SP_4045_DO_YOU_WISH_TO_PASS_THE_LEADER_POSITION_TO__R_NS` ← CTabParty
- `SP_404_S_HAVE_DENIED_INVITATION` ← CMiniRoomBaseDlg
- `SP_405_S_IS_CURRENTLY_NOT_ACCEPTING_ANY_INVITATION` ← CMiniRoomBaseDlg
- `SP_406_TRADE_CANCELLED_R_NBY_THE_OTHER_CHARACTER` ← CCashTradingRoomDlg, CTradingRoomDlg
- `SP_407_TRADE_SUCCESSFUL_R_NPLEASE_CHECK_THE_RESULTS` ← CCashTradingRoomDlg, CTradingRoomDlg
- `SP_408_TRADE_SUCCESSFUL_R_NRECEIVED_D_MESOS_AFTER_FEES_R_NPLEASE_CHECK_THE_RESULTS` ← CCashTradingRoomDlg, CTradingRoomDlg
- `SP_409_TRADE_UNSUCCESSFUL` ← CCashTradingRoomDlg, CTradingRoomDlg
- `SP_410_YOU_CANNOT_MAKE_THE_TRADE_BECAUSE_R_NTHERE_ARE_SOME_ITEMS_WHICH_R_NYOU_CANNOT_CA` ← CCashTradingRoomDlg, CTradingRoomDlg
- `SP_411_YOU_CANNOT_MAKE_THE_TRADE_BECAUSE_R_NTHE_OTHER_PERSONS_ON_A_DIFFERENT_MAP` ← CCashTradingRoomDlg, CTradingRoomDlg
- `SP_412_HOW_MUCH_MONEY_WILL_YOU_PUT_UP` ← CCashTradingRoomDlg, CTradingRoomDlg
- `SP_413_ARE_YOU_SURE_YOU_WANT_TO_TRADE` ← CCashTradingRoomDlg, CTradingRoomDlg
- `SP_4143_BUDDY_CHAT_WITH_S_HAS_BEEN_DISABLED` ← CTabFriend
- `SP_4144_BUDDY_CHAT_WITH_S_HAS_BEEN_ENABLED` ← CTabFriend
- `SP_419_THE_NUMBER_OF_ITEMS_SHOULD_BE_BASED_R_NON_THE_NUMBER_OF_ITEMS_IN_A_BUNDLE` ← CPersonalShopDlg
- `SP_4202_SOUND_BGMGLIMG_CHAPEL` ← CField_Wedding
- `SP_4203_SOUND_BGMGLIMG_CATHEDRAL` ← CField_Wedding
- `SP_4204_DO_YOU_WANT_TO_BLESS_THIS_COUPLE` ← CField_Wedding
- `SP_4205_EFFECT_BASICEFFIMG_WEDDING` ← CField_Wedding
- `SP_420_THE_TOTAL_PRICE_IS_D_MESOS_R_NWILL_YOU_MAKE_THE_PURCHASE` ← CPersonalShopDlg
- `SP_421_YOU_CANT_SELL_IT_ANYMORE` ← CPersonalShopDlg
- `SP_427_THE_ITEM_HAS_BEEN_SOLD_ALREADY` ← CPersonalShopDlg
- `SP_428_YOU_MAY_NOT_ENTER_THIS_STORE` ← CMiniRoomBaseDlg, CUIShopScanResult
- `SP_433_YOU_DONT_HAVE_ENOUGH_MESOS_TO_ENTER` ← CMiniRoomBaseDlg
- `SP_434_YOU_CANT_START_THE_GAME_HERE` ← CMiniRoomBaseDlg
- `SP_435_THE_PERSONAL_STORE_CAN_ONLY_BE_BUILT_R_NAT_THE_FREE_MARKET_AVAILABLE_AT_MAIN_TOW` ← CMiniRoomBaseDlg
- `SP_4363_THE_TOTAL_PRICE_FOR_THE_ITEMS_EXCEEDED_R_NTHE_MAXIMUM_AMOUNT_OF_MESOS_THAT_CAN_B` ← CPersonalShopDlg
- `SP_436_YOU_MAY_ONLY_USE_IT_AT_ROOM_722_LOCATED_AT_THE_2ND_FLOOR_AND_ABOVE_OF_THE_FREE_M` ← CMiniRoomBaseDlg
- `SP_4459_UNABLE_TO_DELETE_DUE_TO_A_MEMBER_R_NIN_THE_UNION_CURRENTLY_BEING_ON_THE_POSITION` ← CWndAllianceGrade
- `SP_4538_PLEASE_CONFIRM_WHETHER_R_NTHE_CHARACTERS_NAME_IS_CORRECT` ← CCashShop
- `SP_4571_UI_UIWINDOWIMG_FAMILY_BTOK` ← CUIFamily
- `SP_4572_UI_UIWINDOWIMG_FAMILY_BTTREE` ← CUIFamily
- `SP_4573_UI_UIWINDOWIMG_FAMILY_BTLEFT` ← CUIFamily
- `SP_4574_UI_UIWINDOWIMG_FAMILY_BTRIGHT` ← CUIFamily
- `SP_4575_UI_UIWINDOWIMG_FAMILY_BTFAMILYPRECEPT` ← CUIFamily
- `SP_4576_UI_UIWINDOWIMG_FAMILY_BTSPECIAL` ← CUIFamily
- `SP_4577_UI_UIWINDOWIMG_FAMILY_BTJUNIORENTRY` ← CUIFamily
- `SP_4579_UI_UIWINDOWIMG_FAMILYTREE_PLATELEADER_D` ← CUIFamilyChart
- `SP_4580_UI_UIWINDOWIMG_FAMILYTREE_PLATEOTHERS_D` ← CUIFamilyChart
- `SP_4581_UI_UIWINDOWIMG_FAMILYTREE_BTJUNIORENTRY` ← CUIFamilyChart
- `SP_4582_UI_UIWINDOWIMG_FAMILYTREE_BTBYE` ← CUIFamilyChart
- `SP_4583_UI_UIWINDOWIMG_FAMILYTREE_SELECTED` ← CUIFamilyChart
- `SP_4586_ENTER_THE_NAME_OF_THE_CHARACTER_YOU_WISH_TO_DELETE` ← CUIFamilyChart
- `SP_4587_ENTER_THE_FAMILY_PRECEPTS` ← CUIFamily
- `SP_4588_YOU_DO_NOT_HAVE_FAMILY_MEMBERS` ← CUIFamilyChart
- `SP_4591_YOU_DO_NOT_HAVE_A_FAMILY_YET` ← CUIFamily, CUIFamilyChart
- `SP_4592__PLEASE_ADD_A_JUNIOR_` ← CUIFamilyChart
- `SP_4593_S_FAMILY` ← CUIFamily, CUIFamilyChart
- `SP_4594_SENIORD_PPL` ← CUIFamilyChart
- `SP_4595_JUNIORD_PPL` ← CUIFamilyChart
- `SP_4632_THIS_COVERS_EVERYONE_IN_THE_FAMILY_R_NTHE_NUMBER_MAY_BE_HIGHER_THAN_THE_ONE_SHOW` ← CUIFamilyChart
- `SP_4635_C_YOU_HAVE_SURPASSED_YOUR_SENIORS_LEVEL_SO_YOUR_CUMULATIVE_REP_R_NCWILL_BE_REDUC` ← CUIFamilyChart
- `SP_4637_REPS_RECORDED_TO_YOUR_SENIOR__S_R_N` ← CUIFamilyChart
- `SP_4638_ENTITLEMENT_IN_USE__R_N` ← CUIFamilyChart
- `SP_4639_ENTITLEMENT_USED_TODAY__R_N` ← CUIFamilyChart
- `SP_4640_S___D_TIMES_R_N` ← CUIFamilyChart
- `SP_4645__R_N` ← CUIFamilyChart
- `SP_4646_ISNT_THERE_ANYTHING_ELSE_YOU_WANT_ILL_MAKE_A_NOTE_OF_IT` ← CAdminShopDlg
- `SP_4647_IM_NOT_DOING_BUSINESS_AT_THIS_TIME_DONT_I_DESERVE_SOME_REST_TOO` ← CAdminShopDlg
- `SP_4648_THATS_NOT_SOMETHING_I_BARTER_ILL_SHOW_YOU_THE_LIST_AGAIN_FOR_YOU_TO_CHOOSE_FROM` ← CAdminShopDlg
- `SP_4649_ARE_YOU_BEGGING` ← CAdminShopDlg
- `SP_4650_WHY_DO_YOU_CARRY_AROUND_SO_MUCH_MESOS_COME_BACK_AFTER_EMPTYING_YOUR_POCKETS` ← CAdminShopDlg
- `SP_4651_TAKING_YOUR_PRECIOUS_TIME_HUH_I_DONT_BARTER_THIS_ITEM_ANYMORE` ← CAdminShopDlg
- `SP_4652_IM_DONE_BARTERING_THIS_ITEM_FOR_THE_DAY_COME_BACK_TOMORROW` ← CAdminShopDlg
- `SP_4653_WHY_ARE_YOU_OBSESSING_OVER_THIS_ITEM_I_WONT_BARTER_ANYMORE` ← CAdminShopDlg
- `SP_4654_YOURE_GREEDY_HOW_ABOUT_YOU_LOWER_THE_QUANTITY` ← CAdminShopDlg
- `SP_4655_IM_A_LITTLE_PREOCCUPIED_RIGHT_NOW_SO_COME_BACK_LATER` ← CAdminShopDlg
- `SP_4656_YOU_DO_NOT_HAVE_THE_ITEM` ← CAdminShopDlg
- `SP_4659_ONCE_PURCHASED_THE_ITEM_CANNOT_BE_TRADED_R_NWOULD_YOU_STILL_LIKE_TO_PURCHASE_IT` ← CPersonalShopDlg
- `SP_4660_ONCE_STORED_IN_THE_INVENTORY_R_NTHE_ITEM_CANNOT_BE_TRADED_R_NBY_THE_CHARACTER_WH` ← CTrunkDlg
- `SP_4661_ONCE_FOUND_THE_ITEM_CANNOT_BE_TRADED_R_NWOULD_YOU_STILL_LIKE_TO_FIND_IT` ← CTrunkDlg
- `SP_4662_SOME_ITEMS_YOU_ARE_TRYING_TO_BARTER_R_NCANNOT_BE_TRADED_ONCE_RECEIVED_R_NWOULD_Y` ← CTradingRoomDlg
- `SP_472_YOU_ARE_UNABLE_TO_ENTER_THE_TOURNAMENT_ROOM` ← CMiniRoomBaseDlg
- `SP_498_YOU_ALREADY_HAVE_THE_ITEMS_SELECTED_ARE_R_NYOU_SURE_YOU_WANT_TO_BUY_THE_ITEMS_AG` ← CCashShop
- `SP_503_THIS_ITEM_CANNOT_BE_USED_WITHOUT_A_PET_AND_R_NTHIS_ITEM_CANNOT_BE_USED_ON_A_DECE` ← CCashShop
- `SP_505_THIS_IS_AN_ITEM_THAT_CANNOT_BE_USED_R_NIF_YOU_DO_NOT_HAVE_THE_MONSTER_R_NRIDING_` ← CCashShop
- `SP_506_ARE_YOU_SURE_YOU_WANT_TO_BUY_THIS_ITEM` ← CCashShop
- `SP_5207_SOME_ITEMS_CANNOT_BE_TRADED_WITH_OTHER_USERS_AND_R_N` ← CCashShop
- `SP_5214_TIME_LIMIT_EXPIRED` ← CCashTradingRoomDlg, CPersonalShopDlg
- `SP_5215_HAVE_YOU_CHECKED_THE_R_NTIME_LIMIT_ON_THE_ITEM` ← CCashTradingRoomDlg, CPersonalShopDlg
- `SP_5216_DDAYS_DHOURS_AND_DMINUTES_LEFT` ← CCashTradingRoomDlg, CPersonalShopDlg
- `SP_5256_YOU_CANNOT_OPEN_A_STORE_IN_THIS_CHANNEL_PLEASE_USE_A_DIFFERENT_CHANNEL` ← CMiniRoomBaseDlg
- `SP_5270_CASH_ITEMS_BECOME_UNTRADABLE_R_NONCE_THEY_HAVE_BEEN_TRADED_AND__R_NMAPLE_POINTS_` ← CPersonalShopDlg
- `SP_5277_CASH_ITEMS_BECOME_UNTRADABLE_R_NONCE_THEY_HAVE_BEEN_TRADED_AND__R_NMAPLE_POINTS_` ← CCashTradingRoomDlg
- `SP_5278_A_CASH_ITEM_TRADE_WAS_CANCELED_R_NDUE_TO_A_SERVER_CHECK` ← CCashTradingRoomDlg
- `SP_5279_TRADE_WAS_CANCELED_BECAUSE_THERE_WAS_AN_ITEM_R_NTHAT_DISAPPEARED_BECAUSE_IT_EXPI` ← CCashTradingRoomDlg
- `SP_527_YOU_ALREADY_HAVE_THIS_ITEM_ON_YOUR_WISH_LIST` ← CCashShop
- `SP_528_YOUR_WISH_LIST_IS_FULL` ← CCashShop
- `SP_5319_THIS_ITEM_HAS_EXPIRED` ← CMiniRoomBaseDlg
- `SP_535_YOU_DONT_HAVE_ENOUGH_CASH` ← CCashShop
- `SP_538_PLEASE_CHECK_AND_SEE_IF_YOU_HAVE_EXCEEDED_R_NTHE_NUMBER_OF_CASH_ITEMS_YOU_CAN_HA` ← CCashShop
- `SP_5434_YOU_NEED_MORE_ITEMS` ← CShopDlg
- `SP_5439_WITH_THE_WEAPON_THAT_S_CURRENTLY_HAS_EQUIPPED_R_NTHIS_ITEM_CANT_BE_USED_R_NDO_YO` ← CCashShop
- `SP_5449_YOU_MUST_BE_OVER_LVD_TO_PURCHASE_THIS_ITEM` ← CShopDlg
- `SP_5450_YOU_MUST_BE_UNDER_LVD_TO_PURCHASE_THIS_ITEM` ← CShopDlg
- `SP_5471_THIS_CONTAINS_AN_ITEM_THAT_WILL_DISAPPEAR_ONCE__R_NTHE_EXPIRATION_DATE_HAS_PASSE` ← CCashShop
- `SP_5472_S_DOES_NOT_HAVE_AN_ITEM_THAT__R_NTHE_SCISSORS_OF_KARMA_CAN_BE_USED_ON_R_NDO_YOU_` ← CCashShop
- `SP_5515_D` ← CChatBalloon
- `SP_5517_S` ← CUIShopScanResult
- `SP_5527_ARIAL` ← CChatBalloon, CTradingRoomDlg, CUIGuildBBS, CUIMessenger, CUIUserList
- `SP_5536_DUE_TO_JOB_LIMITATIONS_S_CANT_R_NUSE_THIS_ITEM_R_NDO_YOU_STILL_WISH_TO_GIVE_IT_A` ← CCashShop
- `SP_5554_ONLINE_PARTY_MEMBERS___TOTAL_REGISTERED_PARTY_MEMBERS` ← CTabParty
- `SP_5566_THE_GAME_FILE_HAS_BEEN_DAMAGED_R_NSO_YOU_CANNOT_PARTICIPATE_IN_AN_ITEM_TRADE_PLE` ← CCashTradingRoomDlg, CTradingRoomDlg
- `SP_5567_THE_PASSWORD_IS_INCORRECT` ← CMiniRoomBaseDlg
- `SP_5570_THE_TIME_LIMIT_HAS_EXPIRED` ← CPersonalShopDlg
- `SP_5577_UI_BASICIMG_BTCLOSE` ← CUIMessenger
- `SP_557_DUE_TO_AN_UNKNOWN_ERROR_R_NTHE_REQUEST_FOR_CASH_SHOP_HAS_FAILED` ← CCashShop
- `SP_5590_UI_UIWINDOWIMG_ITEMSEARCH_BTLEFT` ← CUIUserList
- `SP_5591_UI_UIWINDOWIMG_ITEMSEARCH_BTRIGHT` ← CUIUserList
- `SP_5599_YOU_DO_NOT_HAVE_ENOUGH_MESOS` ← CAdminShopDlg, CShopDlg, CTrunkDlg
- `SP_5619_BACKGRND` ← CChatBalloon
- `SP_561_ALL_THE_GIFTS_HAVE_BEEN_SENT_R_ND_NX__R_NWAS_SPENT_IN_THE_PROCESS` ← CCashShop
- `SP_5623_DEAD` ← CChatBalloon
- `SP_5624_DELAY` ← CChatBalloon
- `SP_5628_HEAD` ← CChatBalloon
- `SP_562_THE_GIFTS_COULD_NOT_BE_SENT` ← CCashShop
- `SP_5639_MESO` ← CShopDlg
- `SP_563___S___R_NHAS_BEEN_SENT_TO_THE_CHARACTER_ABOVE_R_NIT_COULD_NOT_BE_SENT_TO_SEVERAL` ← CCashShop
- `SP_5641_NONE` ← CTabBlackList, CTabFriend, CTabGuild, CTabParty
- `SP_5643_PET` ← CChatBalloon
- `SP_684_ONLINE_GUILD_MEMBERS___TOTAL_REGISTERED_GUILD_MEMBERS` ← CTabGuild, CTabGuildAlliance
- `SP_685_OFFLINE_GUILD_MEMBERS___TOTAL_REGISTERED_GUILD_MEMBERS` ← CTabGuild
- `SP_686_SORT_THRU_NAME` ← CTabGuild, CTabGuildAlliance
- `SP_687_SORT_THRU_JOB` ← CTabGuild, CTabGuildAlliance
- `SP_688_SORT_THRU_LEVEL` ← CTabGuild, CTabGuildAlliance
- `SP_689_SORT_THRU_GRADE` ← CTabGuild, CTabGuildAlliance
- `SP_691_THE_USER_IS_CURRENTLY_AT_ANOTHER_LOCATION` ← CTabParty
- `SP_712_PLEASE_ENTER_THE_NAME_OF_THE_CHARACTER_R_NYOULL_INVITE_TO_YOUR_PARTY` ← CTabParty
- `SP_718_CASH_SHOP` ← CTabParty
- `SP_725_PLEASE_ENTER_THE_NAME_OF_THE_CHARACTER_YOUD_LIKE_TO_ADD_TO_THE_BLACKLIST` ← CBlackListDlg, CTabBlackList
- `SP_726_YOU_MAY_NOT_ENTER_YOURSELF_IN_THE_BLACKLIST` ← CBlackListDlg, CTabBlackList
- `SP_727_THIS_CHARACTER_NAME_DOES_NOT_EXIST_R_NPLEASE_CHECK_AGAIN` ← CBlackListDlg, CTabBlackList
- `SP_728_THIS_NAME_HAS_ALREADY_BEEN_REGISTERED_R_NPLEASE_CHECK_AGAIN` ← CBlackListDlg, CTabBlackList
- `SP_729_YOU_CAN_USE_IT_AFTER_PURCHASING_THE_MEMO` ← CTabFriend
- `SP_730_YOU_MAY_ENTER_WITH_2_6_PARTY_MEMBERS` ← TabPartySearch
- `SP_731_YOUR_PARTY_ALREADY_HAS_D_OR_MORE_MEMBERS` ← TabPartySearch
- `SP_732_YOU_CAN_ONLY_SEARCH_FOR_PARTY_R_NMEMBERS_WITHIN_A_RANGE_OF_30_LEVELS` ← TabPartySearch
- `SP_733_CURRENTLY_THE_HIGHEST_LEVEL_IN_MAPLESTORY_R_NIS_AT_200` ← TabPartySearch
- `SP_734_THE_RANGE_OF_LEVEL_FOR_SEARCH_HAS_TO_INCLUDE_YOUR_OWN_LEVEL` ← TabPartySearch
- `SP_735_THE_MIN_VALUE_IS_HIGHER_THAN_THE_MAX` ← TabPartySearch
- `SP_736_PLEASE_ENTER_THE_JOB_CLASS_YOUD_LIKE_TO_PARTY_WITH` ← TabPartySearch
- `SP_800___MAPLE_MESSENGER_HELP__` ← CUIMessenger
- `SP_801_INVITE___INVITE_CHARACTER_NAME` ← CUIMessenger
- `SP_802_END___Q` ← CUIMessenger
- `SP_803___S_HAS_ENTERED` ← CUIMessenger
- `SP_804___YOU_HAVE_BEEN_UNABLE_TO_JOIN_THE_INVITED_CHAT_ROOM` ← CUIMessenger
- `SP_806___PLEASE_ENTER_THE_USERS_NAME` ← CUIMessenger
- `SP_824_IT_CANNOT_BE_USED_PLEASE_CHECK_IT_AGAIN` ← CWndAllianceGrade, CWndGuildGrade
- `SP_848_ARE_YOU_SURE_YOU_WANT_TO_BUY_IT` ← CAdminShopDlg, CPersonalShopDlg
- `SP_849_HOW_MANY_ARE_YOU_WILLING_TO_BUY` ← CAdminShopDlg, CPersonalShopDlg
- `SP_850_ARE_YOU_SURE_YOU_WANT_TO_SELL_IT` ← CAdminShopDlg
- `SP_851_HOW_MANY_ARE_YOU_WILLING_TO_SELL` ← CAdminShopDlg
- `SP_852_YOU_DONT_HAVE_ENOUGH_IN_STOCK` ← CShopDlg
- `SP_853_PLEASE_CHECK_IF_YOUR_INVENTORY_IS_FULL_OR_NOT` ← CShopDlg, CTrunkDlg
- `SP_855_DUE_TO_AN_ERROR_THE_TRADE_DID_NOT_HAPPEN` ← CShopDlg, CTrunkDlg
- `SP_858_ARE_YOU_SURE_YOU_WANT_TO__R_NRECOVER_THE_ITEMS_FROM_STORAGE` ← CTrunkDlg
- `SP_859_RECOVERING_THE_ITEMS_WILL_COST_D_MESOS_R_NARE_YOU_SURE_YOU_WANT_TO_RECOVER_THIS` ← CTrunkDlg
- `SP_860_ARE_YOU_SURE_YOU_WANT_TO_STORE_THE_ITEMS` ← CTrunkDlg
- `SP_861_STORING_THIS_WILL_COST_D_MESOS_R_NARE_YOU_SURE_YOU_WANT_TO_STORE_THIS` ← CTrunkDlg
- `SP_862_HOW_MANY_WILL_YOU_STORE` ← CTrunkDlg
- `SP_863_HOW_MUCH_WILL_YOU_TAKE_OUT` ← CTrunkDlg
- `SP_864_HOW_MUCH_WILL_YOU_SAVE` ← CTrunkDlg
- `SP_865_THE_STORAGE_IS_FULL` ← CTrunkDlg
- `SP_866_ITEM_COULD_NOT_BE_RETRIEVED_R_NBECAUSE_THERE_WAS_AN_ITEM_THAT_R_NCOULD_ONLY_BE_A` ← CTrunkDlg
- `SP_887_HOW_MANY_WILL_YOU_TRADE` ← CTradingRoomDlg
- `SP_891_REPEATING_THE_SAME_LINE_OVER_AND_OVER_R_NCAN_NEGATIVELY_AFFECT_OTHER_USERS` ← CChatHelper
- `SP_892_TOO_MUCH_CHATTING_CAN_DISRUPT_R_NOTHER_PLAYERS_ABILITY_TO_PLAY_THE_GAME` ← CChatHelper
- `SP_893_YOU_ARE_CURRENTLY_BLOCKED_FROM_CHATTING` ← CChatHelper
- `SP_956_CANVAS` ← CAdminShopDlg, CChatBalloon, CUIMessenger, CUIUserList
- `SP_958_SHAPE2DVECTOR2D` ← CField_Wedding
- `SP_977_X` ← CField_GuildBoss
- `SP_978_Y` ← CField_GuildBoss

## 2. 核心窗口类逐函数关键行


### CUIMessenger（26 函数）


#### CUIMessenger::0CHARACTER  (233 B)

  - `void __thiscall CUIMessenger::CHARACTER::CHARACTER(_DWORD *this)`
  - `*(this + 1) = 0;`
  - `*(this + 2) = 0;`
  - `*(this + 3) = 0;`
  - `*(this + 7) = 0;`
  - `sub_84DB40(a1: this);`

#### CUIMessenger::1CHARACTER  (607 B)

  - `void __thiscall CUIMessenger::CHARACTER::~CHARACTER(_DWORD *this)`
  - `v2 = *(this + 7);`
  - `if ( v2 != 0 )`
  - `_bstr_t::Data_t::Release(a1: v2);`
  - `*(this + 7) = 0;`
  - `v3 = *(this + 3);`
  - `if ( v3 != 0 )`
  - `(*(*v3 + 8))(a1: *(this + 3));`
  - `v4 = *(this + 2);`
  - `if ( v4 != 0 )`
  - `(*(*v4 + 8))(a1: v4);`
  - `if ( *(this + 1) != 0 )`
  - `sub_428C15(a1: 0);`
  - `*(this + 1) = 0;`

#### CUIMessenger::AddChatText  (5057 B)

  - `void __thiscall CUIMessenger::AddChatText(CWnd *this, _WORD *sText, wchar_t *nIdx)`
  - `v4 = *(this + 49);`
  - `&& ((v5 = *(this + 32), v6 = *(v4 + 56), v5 != 0) ? (v7 = *(v5 - 4)) : (v7 = 0), v6 >= v7 - *(this + 28));`
  - `if ( nIdx == -1 )`
  - `else if ( nIdx != 0 )`
  - `if ( nIdx == 1 )`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v39) = 1;`
  - `_bstr_t::_bstr_t(a1: asc_AF822C);`
  - `sub_4284A8(a1: this + 132);`
  - `v34 = sub_42782E(a1: v28, a2: p_pvarg);`
  - `LOBYTE(v39) = 0;`
  - `if ( v9 < 0 )`
  - `while ( 1 )`
  - `LOBYTE(v39) = 3;`
  - `v10 = sub_41468C(a1: &sText);`
  - …（截断 16 行）

#### CUIMessenger::Draw  (945 B)

  - `_DWORD *__thiscall CUIMessenger::Draw(wchar_t ***this, _DWORD *a2)`
  - `CWnd::Draw(this, a2);`
  - `CWnd::GetCanvas(a1: &a2);`
  - `v3 = *(this + 27);`
  - `if ( v3 <= 1 )`
  - `if ( a2 != 0 )`
  - `(*(*a2 + 4))(a1: a2);`
  - `CUIMessenger::DrawTextA(this, a2: v5[0]);`
  - `CUIMessenger::DrawStatusBar(this, a2: v5[0]);`
  - `return (*(*a2 + 8))(a1: a2);`

#### CUIMessenger::DrawStatusBar  (1016 B)

  - `_DWORD *__thiscall CUIMessenger::DrawStatusBar(wchar_t ***this, _DWORD *a2)`
  - `while ( 1 )`
  - `if ( *(v3 - 3) != 0 && *(v3 - 2) != 0 )`
  - `if ( *v3 != 0 )`
  - `v4 = wcslen(a1: v2);`
  - `sub_48EF51(a1: v2, a2: v4);`
  - `if ( --v6 == 0 )`
  - `LOBYTE(v8) = 0;`
  - `if ( a2 != 0 )`
  - `return (*(*a2 + 8))(a1: a2);`

#### CUIMessenger::DrawTextA  (3239 B)

  - `VARIANTARG *__thiscall CUIMessenger::DrawTextA(_DWORD *this, VARIANTARG *a2)`
  - `v3 = *(*(this + 49) + 56);`
  - `v4 = *(this + 28) <= 0;`
  - `if ( !v4 )`
  - `if ( *v5 == 0 || v28 >= *(*v5 - 4) )`
  - `VariantInit(&pvarg);`
  - `v6 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v6 < 0 )`
  - `LOBYTE(v29) = 1;`
  - `VariantInit(pvarg: &pvargDest);`
  - `v7 = ZtlTaskMemFreeImp(&pvargDest, pvargSrc: &pvargSrc);`
  - `if ( v7 < 0 )`
  - `LOBYTE(v29) = 2;`
  - `v19 = sub_4281F8(a1: &a2);`
  - `v8 = sub_85201E(this: this + 32, a2: v28);`
  - `v9 = sub_41468C(a1: v8);`
  - …（截断 16 行）

#### CUIMessenger::HandleSlashCommand  (2733 B)

- SP_ 节点：`SP_145__INVITE`, `SP_1868__Q`, `SP_1869__Q`
  - `signed int __thiscall CUIMessenger::HandleSlashCommand(CUIMessenger *this, char a2)`
  - `LOBYTE(v22) = 2;`
  - `sub_428211(a1: &a2);`
  - `if ( sub_4A8DEF(a1: v15, a2: v16) != 0 )`
  - `LOBYTE(v22) = 3;`
  - `v7 = sub_4A8DEF(a1: v15, a2: v16);`
  - `if ( v7 != 0 )`
  - `LOBYTE(v22) = 1;`
  - `if ( v21 != 0 )`
  - `CUIMessenger::TryDelete(this);`
  - `LOBYTE(v22) = 4;`
  - `v15 = sub_428226(a1: String);`
  - `v12 = slashcmd_check_with_subcmd(a1: v14, a2: v15);`
  - `if ( v12 != 0 )`
  - `ZXString<char>::operator=(a1: &v20);`
  - `CUIMessenger::SendInviteMsg(a1: v16);`
  - …（截断 16 行）

#### CUIMessenger::OnButtonClicked  (749 B)

  - `void __thiscall CUIMessenger::OnButtonClicked(CUIMessenger *this, unsigned int a2)`
  - `switch ( a2 )`
  - `CUIMessenger::TryDelete(this);`
  - `v2 = *(this + 27) + 1;`
  - `v2 = *(this + 27) - 1;`
  - `CUIMessenger::SetState(this, a2: v2 % 3);`
  - `CUIMessenger::ProcessChat(this);`
  - `CUIMessenger::SendClaim(this);`

#### CUIMessenger::OnChat  (6315 B)

  - `void __thiscall CUIMessenger::OnChat(CWnd *this, int a2)`
  - `CInPacket::DecodeStr(a1: &a2);`
  - `v3 = ZXString<char>::Find(a1: asc_AF39A4, a2: 0, a3: 0);`
  - `if ( v3 >= 0 )`
  - `v8 = sub_47379F(a1: &v33, a2: v3);`
  - `LOBYTE(v38) = 4;`
  - `ZXString<char>::operator=(a1: v8);`
  - `LOBYTE(v38) = 1;`
  - `if ( a2 != 0 )`
  - `v4 = *(a2 - 4);`
  - `v5 = sub_47379F(a1: &v33, a2: v4 - 1);`
  - `LOBYTE(v38) = 2;`
  - `ZXString<char>::operator=(a1: v5);`
  - `if ( v33 != 0 )`
  - `sub_4737BB(a1: &v33, a2: 1);`
  - `LOBYTE(v38) = 3;`
  - …（截断 16 行）

#### CUIMessenger::OnCreate  (11723 B)

- SP_ 节点：`SP_1410_CANVASFONT`, `SP_1858_UI_UIWINDOWIMG_MESSENGER_BLINK`, `SP_1859__________________________`, `SP_5527_ARIAL`, `SP_800___MAPLE_MESSENGER_HELP__`, `SP_801_INVITE___INVITE_CHARACTER_NAME`, `SP_802_END___Q`
  - `_DWORD *__thiscall CUIMessenger::OnCreate(CUIMessenger *this, int a2)`
  - `CUIMessenger::SetLayer(this);`
  - `CUIMessenger::SetCtrl(this);`
  - `basic_font = get_basic_font(a1: &Unknown, fType: 1);`
  - `sub_4288D7(a1: *basic_font);`
  - `sub_45BAEE(a1: &Unknown);`
  - `PcCreateObject::IWzFont(sUOL: v5, pObj: this + 34, pUnkOuter: 0);`
  - `if ( Unknown != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v68) = 1;`
  - `sub_4284A8(a1: this + 136);`
  - `sub_46341A(a1: v53, a2: HIDWORD(v53), a3: v54, a4: p_pvargDest);`
  - `if ( v8 < 0 )`
  - `sub_402F85(pvargDest: &v58, pvargSrc: &pvargSrc);`
  - `HIDWORD(v53) = &v58;`
  - `LODWORD(v53) = &pvargDest;`
  - …（截断 16 行）

#### CUIMessenger::OnEnter  (3260 B)

- SP_ 节点：`SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_803___S_HAS_ENTERED`
  - `void __thiscall CUIMessenger::OnEnter(`
  - `if ( a2 <= 2 )`
  - `v7 = (this + 32 * a2 + 200);`
  - `sub_84DB40(a1: v7);`
  - `_bstr_t::_bstr_t(this: &v22, a2: v20);`
  - `sub_402C9B(a1: &v22);`
  - `sub_402D9A(this: &v22);`
  - `if ( *(this + 27) == 2 )`
  - `sub_84EC32(a1: 1);`
  - `CUIMessenger::SetAvatar(this, a2, a3, a4: 0);`
  - `if ( a6 != 0 )`
  - `LOBYTE(v23) = 2;`
  - `ChannelName = CWvsContext::GetChannelName(a1: v20);`
  - `ZXString<char>::GetBuffer(a1: ChannelName, a2: 0xFFFFFFFF);`
  - `sub_428211(a1: &a2);`
  - `sub_988557(a1: v17, a2: v18, a3: v19, a4: v20);`
  - …（截断 16 行）

#### CUIMessenger::OnInvite  (2541 B)

  - `static void __cdecl CUIMessenger::OnInvite(struct CInPacket *a1)`
  - `CInPacket::DecodeStr(&a1);`
  - `v12 = CInPacket::Decode1(this: v1);`
  - `v13 = CInPacket::Decode4(this: v1);`
  - `if ( CInPacket::Decode1(this: v1) != 0`
  - `|| *(TSingleton<CConfig>::ms_pInstance + 10) != 0`
  - `&& (v11 = v8,`
  - `ZXString<char>::operator=(&a1),`
  - `LOBYTE(v14) = 0,`
  - `sub_538C98(),`
  - `CConfig::IsInBlackList(a1: v8[0]) == 0) )`
  - `v4 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 280);`
  - `LOBYTE(v14) = 3;`
  - `if ( v4 != 0 )`
  - `v5 = CUIFadeYesNo::CUIFadeYesNo(a1: v4);`
  - `LOBYTE(v14) = 0;`
  - …（截断 16 行）

#### CUIMessenger::OnKey  (4577 B)

- SP_ 节点：`SP_3580_WILL_YOU_EXIT_THIS_CHAT_ROOM`
  - `void __thiscall CUIMessenger::OnKey(CUIMessenger *this, int *a2, char *a3)`
  - `v4 = *(this + 46);`
  - `v5 = *(TSingleton<CWndMan>::ms_pInstance + 136) == (v4 != 0 ? v4 + 4 : 0);`
  - `if ( SHIBYTE(a3) >= 0 )`
  - `if ( v4 != 0 )`
  - `Text = CCtrlEdit::GetText(a1: &v17);`
  - `LOBYTE(v22) = 1;`
  - `ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF);`
  - `ZXString<char>::operator=(a1: Text);`
  - `if ( (v20 & 2) != 0 )`
  - `if ( v19 != 0 )`
  - `if ( (v20 & 1) != 0 && v17 != 0 )`
  - `if ( a2 != 13 )`
  - `if ( a2 == 27 )`
  - `if ( CUtilDlg::YesNo(a1: v12, a2: v13, a3: v14, a4: v15, a5: v16) == 6 )`
  - `CUIMessenger::TryDelete(this: (this - 4));`
  - …（截断 16 行）

#### CUIMessenger::OnPacket  (1212 B)

- 分支 case：`0`, `1`, `2`, `4`, `5`, `6`, `7`, `8`
  - `static void __cdecl CUIMessenger::OnPacket(struct CInPacket *a1)`
  - `v1 = CInPacket::Decode1(this: a1);`
  - `if ( v1 == 3 )`
  - `CUIMessenger::OnInvite(a1);`
  - `else if ( dword_BF0F54 != 0 )`
  - `switch ( v1 )`
  - `sub_84F01F(a1);`
  - `sub_84F264(this: dword_BF0F54, a2: a1);`
  - `sub_84F351(a1);`
  - `sub_851293(this: dword_BF0F54, a2: a1);`
  - `sub_851387(a1);`
  - `CUIMessenger::OnChat(this: dword_BF0F54, a2: a1);`
  - `sub_85194F(this: dword_BF0F54, a2: a1);`
  - `sub_8519F2(this: dword_BF0F54, a2: a1);`

#### CUIMessenger::OnSelfEnter  (1486 B)

- SP_ 节点：`SP_804___YOU_HAVE_BEEN_UNABLE_TO_JOIN_THE_INVITED_CHAT_ROOM`
  - `void __thiscall CUIMessenger::OnSelfEnter(CUIMessenger *this, int a2, wchar_t ***a3)`
  - `if ( a2 >= 0 )`
  - `*(this + 74) = a2;`
  - `*(this + 74) = 0;`
  - `CUIMessenger::AddChatText(this, sText: v8, nIdx: v9);`
  - `v13 = *(*dword_BE7918 + 8280);`
  - `v5 = *(*dword_BE7918 + 8376);`
  - `if ( v5 != 0 )`
  - `v6 = (v5 + 4);`
  - `v9 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v11) + 4);`
  - `v7 = AvatarLook::AvatarLook(a1: v9);`
  - `LOBYTE(v12) = 1;`
  - `CUIMessenger::OnEnter(this, a2: *(this + 74), a3: v7, a4: v6, a5: v13, a6: a3);`
  - `if ( v11[1] != 0 )`
  - `sub_428C44(this: v11, a2: 0);`

#### CUIMessenger::ProcessChat  (3173 B)

  - `void __thiscall CUIMessenger::ProcessChat(void **this)`
  - `CCtrlEdit::GetText(a1: &v16);`
  - `if ( v16 != 0 && *v16 != 0 && *(this + 74) >= 0 )`
  - `CCtrlEdit::SetText(a1: 0);`
  - `if ( *v16 == 47 )`
  - `ZXString<char>::operator=(a1: &v16);`
  - `if ( CUIMessenger::HandleSlashCommand(this, a2: v11[0]) != 0 )`
  - `*(this + 29) = 0;`
  - `if ( CChatHelper::TryChat(this: this + 75, a2: v11[0]) != 0 )`
  - `v2 = *(*dword_BE7918 + 8376);`
  - `if ( v2 != 0 )`
  - `v3 = (v2 + 4);`
  - `ZXString<char>::GetBuffer(a1: v3, a2: 0xFFFFFFFF);`
  - `LOBYTE(v17) = 1;`
  - `v4 = strlen(a1: asc_AF39A4);`
  - `ZXString<char>::_Cat(a1: asc_AF39A4, a2: v4);`
  - …（截断 16 行）

#### CUIMessenger::SendClaim  (1515 B)

  - `int __thiscall CUIMessenger::SendClaim(CUIMessenger *this)`
  - `for ( i = 0; ; ++i )`
  - `v2 = *(this + 32);`
  - `if ( v2 == 0 || i >= *(v2 - 4) )`
  - `v3 = (v2 + 8 * i);`
  - `if ( v4 == 11 || v4 == 6 || v4 == 13 )`
  - `v5 = alloca(dword_BF03D4(a1: 0, a2: 0, a3: *v3, a4: -1, a5: 0, a6: 0, a7: 0, a8: 0));`
  - `dword_BF03D4(a1: 0, a2: 0, a3: *(*(this + 32) + 8 * i), a4: -1, a5: v8, a6: 0x3FFFFFFF, a7: 0, a8: 0);`
  - `ZXString<char>::GetBuffer(a1: v8, a2: 0xFFFFFFFF);`
  - `LOBYTE(v12) = 1;`
  - `sub_47CB2D(a1: -1);`
  - `ZXString<char>::operator=(a1: &v10);`
  - `LOBYTE(v12) = 0;`
  - `sub_42861D();`
  - `CWvsContext::SendClaimRequest(a1: &v9, a2: v7);`
  - `return ZArray<ZXString<char>>::RemoveAll(a1: &v9);`
  - …（截断 16 行）

#### CUIMessenger::SendInviteMsg  (1078 B)

- SP_ 节点：`SP_806___PLEASE_ENTER_THE_USERS_NAME`
  - `void __thiscall CUIMessenger::SendInviteMsg(CWnd *this, int a2)`
  - `v3 = *(*dword_BE7918 + 8376);`
  - `if ( v3 != 0 )`
  - `v4 = (v3 + 4);`
  - `ZXString<char>::GetBuffer(a1: v4, a2: 0xFFFFFFFF);`
  - `LOBYTE(v11) = 1;`
  - `CUIMessenger::AddChatText(this, sText: v7, nIdx: v8);`
  - `LOBYTE(v11) = 0;`

#### CUIMessenger::SetAvatar  (25158 B)

- SP_ 节点：`SP_1860_UI_UIWINDOWIMG_MESSENGER_CHATBALLOON`, `SP_1867_UI_UIWINDOW_MESSENGER_NAMEBAR_D`, `SP_956_CANVAS`
  - `void __thiscall CUIMessenger::SetAvatar(`
  - `v4 = *(this + 8 * a2 + 51) == 0;`
  - `if ( (v4 || a4 != 0) && *(this + 8 * a2 + 54) != 0 )`
  - `ZRef<CAvatar>::_Alloc(a1: v126);`
  - `CWnd::GetLayer(a1: &v105);`
  - `Layer = CWnd::GetLayer(a1: &Unknown);`
  - `HIDWORD(v104) = v8;`
  - `LOBYTE(v127) = 1;`
  - `sub_428EC4(a1: Layer);`
  - `LOBYTE(v127) = 3;`
  - `LODWORD(v104) = sub_426626(a1: 2, a2: 1);`
  - `v9 = sub_4285F2(a1: v126);`
  - `CAvatar::Init(`
  - `pOrigin: HIDWORD(v104),`
  - `sub_4156C5(a1: &Unknown);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - …（截断 16 行）

#### CUIMessenger::SetCtrl  (13046 B)

- SP_ 节点：`SP_1260_UI_BASICIMG_BTCLAIM`, `SP_1261_UI_BASICIMG_BTMIN`, `SP_1262_UI_BASICIMG_BTMAX`, `SP_2722_UI_UIWINDOWIMG_MESSENGER_BTENTER`, `SP_5527_ARIAL`, `SP_5577_UI_BASICIMG_BTCLOSE`
  - `void __thiscall CUIMessenger::SetCtrl(CUIMessenger *this)`
  - `v2 = *(this + 27) != 2;`
  - `v3 = *(this + 37);`
  - `if ( v3 != 0 )`
  - `(*(*v3 + 12))(a1: v3);`
  - `LOBYTE(v68) = 1;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `LOBYTE(v68) = 0;`
  - `v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v68) = 2;`
  - `if ( v6 != 0 )`
  - `v7 = CCtrlButton::CCtrlButton(this: v6);`
  - `sub_4284FF(a1: v7);`
  - `(*(**(this + 37) + 32))(`
  - `a1: *(this + 37),`
  - `a4: *(this + 27) != 2 ? 276 : 166,`
  - …（截断 16 行）

#### CUIMessenger::SetLayer  (7603 B)

- SP_ 节点：`SP_1861_UI_UIWINDOWIMG_MESSENGER_BACKGRND`, `SP_956_CANVAS`
  - `int __thiscall CUIMessenger::SetLayer(ULONG **this)`
  - `sub_402FAB(a1: -2, a2: 3);`
  - `Layer = CWnd::GetLayer(a1: &p_Hi32);`
  - `LOBYTE(v44) = 1;`
  - `if ( v3 )`
  - `IWzGr2DLayer::RemoveCanvas(this: *Layer, a2: &v41, a3: &pvarg);`
  - `sub_4156AB(a1: &v41);`
  - `sub_4156C5(a1: &p_Hi32);`
  - `if ( v4 < 0 )`
  - `v6 = *(this + 27);`
  - `if ( v6 != 0 )`
  - `if ( v7 != 0 )`
  - `if ( v7 != 1 )`
  - `*(this + 9) = 184;`
  - `*(this + 10) = 22;`
  - `*(this + 9) = 392;`
  - …（截断 16 行）

#### CUIMessenger::SetShowState  (5337 B)

  - `CAvatar *__thiscall CUIMessenger::SetShowState(CUIMessenger *this, int a2)`
  - `result = *(this + 8 * a2 + 51);`
  - `if ( result != 0 )`
  - `v4 = *(this + 27);`
  - `if ( v4 != 0 )`
  - `if ( v5 != 0 )`
  - `if ( v5 != 1 )`
  - `CAvatar::SetLayerColor(this: result, a2: 0xFFFFFF);`
  - `if ( *(v2 + 2) == 0 )`
  - `v6 = *(v2 + 2);`
  - `v7 = (*(*v6 + 224))(a1: v6, a2: 0xFFFFFF);`
  - `if ( v7 < 0 )`
  - `if ( *(v2 + 3) == 0 )`
  - `v8 = *(v2 + 3);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - …（截断 16 行）

#### CUIMessenger::SetState  (300 B)

  - `void __thiscall CUIMessenger::SetState(CUIMessenger *this, int a2)`
  - `if ( a2 != *(this + 27) )`
  - `*(this + 27) = a2;`
  - `CUIMessenger::SetLayer(this);`
  - `CUIMessenger::SetCtrl(this);`
  - `CWnd::InvalidateRect(this, a2: 0);`

#### CUIMessenger::TryDelete  (375 B)

  - `int __thiscall CUIMessenger::TryDelete(CUIMessenger *this)`
  - `result = CWvsContext::CanSendExclRequest(a1: 0, a2: 0);`
  - `if ( result != 0 )`
  - `result = CWnd::Destroy(a1: this);`
  - `if ( this != 0 )`
  - `return (**(this + 2))(a1: this + 8, a2: 1);`

#### CUIMessenger::TryNew  (402 B)

  - `static int __cdecl CUIMessenger::TryNew(int a1)`
  - `if ( CWvsContext::CanSendExclRequest(a1: 0, a2: 0) == 0 || dword_BF0F54 != 0 )`
  - `v3 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1644);`
  - `if ( v3 != 0 )`
  - `CUIMessenger::CUIMessenger(a1: v3, a2: v2, a3: a1);`

#### CUIMessenger::Update  (4638 B)

  - `void __fastcall CUIMessenger::Update(CUIMessenger *this, int a2)`
  - `v3 = (this + 204);`
  - `for ( i = 3; i != 0; --i )`
  - `if ( *v3 != 0 )`
  - `CAvatar::Update(this: *v3, a2);`
  - `v4 = *(this + 30);`
  - `if ( v4 <= 0 )`
  - `if ( *(this + 47) != 0 )`
  - `if ( *(this + 29) == 0 )`
  - `v5 = *CCtrlEdit::GetText(a1: &i);`
  - `if ( v6 )`
  - `COutPacket::COutPacket(a1: 122);`
  - `COutPacket::Encode1(this: &v25, a2: 6);`
  - `v7 = ZXString<char>::ZXString<char>(this: &i, a2: a0, a3: 0xFFFFFFFF);`
  - `LOBYTE(v31) = 1;`
  - `v9 = sub_42861D();`
  - …（截断 16 行）

### CUIUserList（10 函数）


#### CUIUserList::CreateAllianceButton  (4633 B)

- SP_ 节点：`SP_5590_UI_UIWINDOWIMG_ITEMSEARCH_BTLEFT`, `SP_5591_UI_UIWINDOWIMG_ITEMSEARCH_BTRIGHT`
  - `int __thiscall CUIUserList::CreateAllianceButton(`
  - `result = dword_BF0484(a1: a4, a2: *(a3 + 1), a3: *(a3 + 2));`
  - `if ( result != 0 )`
  - `v8 = *(v5 + 2);`
  - `v30 = *(v5 + 1) + 71;`
  - `v10 = *(this + 2 * a2 + 471) == 0;`
  - `if ( v10 )`
  - `v27 = 1262 - (*v5 != 0);`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `v13 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v32) = 2;`
  - `if ( v13 != 0 )`
  - `v14 = CCtrlButton::CCtrlButton(this: v13);`
  - `LOBYTE(v32) = 0;`
  - `v15 = sub_4284FF(a1: v14);`
  - `(*(**(v15 + 4) + 32))(a1: *(v15 + 4), a2: this, a3: a2 + 2080, a4: v30, a5: v31, a6: 0, a7: v28);`
  - …（截断 16 行）

#### CUIUserList::DestroyAllianceButton  (748 B)

  - `int __thiscall CUIUserList::DestroyAllianceButton(CUIUserList *this, int a2)`
  - `if ( a2 >= 0 )`
  - `v6 = *(this + 2 * a2 + 471);`
  - `if ( v6 != 0 )`
  - `(*(*v6 + 12))(a1: v6);`
  - `v3 = (this + 1884);`
  - `for ( i = 15; i != 0; --i )`
  - `if ( *v3 != 0 )`
  - `(*(**v3 + 12))(a1: *v3);`

#### CUIUserList::DestroyGuildButton  (744 B)

  - `int __thiscall CUIUserList::DestroyGuildButton(CUIUserList *this, int a2)`
  - `if ( a2 >= 0 )`
  - `v6 = *(this + 2 * a2 + 435);`
  - `if ( v6 != 0 )`
  - `(*(*v6 + 12))(a1: v6);`
  - `v3 = (this + 1740);`
  - `for ( i = 6; i != 0; --i )`
  - `if ( *v3 != 0 )`
  - `(*(**v3 + 12))(a1: *v3);`

#### CUIUserList::Draw  (35089 B)

- SP_ 节点：`SP_956_CANVAS`
  - `_DWORD *__thiscall CUIUserList::Draw(CUIUserList *this, int i)`
  - `CWnd::Draw(this, a2: i);`
  - `Canvas = CWnd::GetCanvas(a1: &i);`
  - `sub_4051E5(a1: *Canvas);`
  - `sub_4156AB(a1: &i);`
  - `LOBYTE(v134) = 3;`
  - `PcCreateObject::IWzCanvas(a1: v5, a2: &v133, a3: 0);`
  - `LOBYTE(v134) = 2;`
  - `if ( i != 0 )`
  - `LOBYTE(v134) = 4;`
  - `PcCreateObject::IWzCanvas(a1: v7, a2: &v128, a3: 0);`
  - `v8 = *(this + 369);`
  - `if ( v8 != 0 )`
  - `if ( v9 != 0 )`
  - `if ( v10 != 0 )`
  - `if ( v11 != 0 )`
  - …（截断 16 行）

#### CUIUserList::OnCreate  (59169 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1410_CANVASFONT`, `SP_1412_B`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_2051_UI_UIWINDOWIMG_USERINFO_BACKGRND`, `SP_2052_UI_UIWINDOWIMG_USERINFO_BACKGRND2`, `SP_2053_UI_UIWINDOWIMG_USERINFO_BACKGRND3`, `SP_2054_UI_UIWINDOWIMG_USERINFO_BACKGRND4`, `SP_2055_UI_UIWINDOWIMG_USERINFO_BACKGRND5`, `SP_2056_UI_UIWINDOWIMG_USERINFO_BACKGRND6`, `SP_2057_UI_UIWINDOWIMG_USERINFO_BACKGRND7`, `SP_2090_UI_UIWINDOWIMG_USERLIST_BACKGRND`, `SP_2097_UI_UIWINDOWIMG_USERLIST_FRIEND_BTMOD`, `SP_2098_UI_UIWINDOWIMG_USERLIST_FRIEND_BTDELETE`, `SP_2099_UI_UIWINDOWIMG_USERLIST_FRIEND_BTADDFRIEND`, `SP_2100_UI_UIWINDOWIMG_USERLIST_FRIEND_BTADDGROUP`, `SP_2101_UI_UIWINDOWIMG_USERLIST_FRIEND_BTWHISPER`, `SP_2102_UI_UIWINDOWIMG_USERLIST_FRIEND_BTGROUPWHISPER`, `SP_2103_UI_UIWINDOWIMG_USERLIST_FRIEND_BTBLOCK`, `SP_2105_UI_UIWINDOWIMG_USERLIST_FRIEND_BTCHAT`, `SP_2106_UI_UIWINDOWIMG_USERLIST_FRIEND_BTPARTY`, `SP_2107_UI_UIWINDOWIMG_USERLIST_FRIEND_BTMESSAGE`, `SP_2109_UI_UIWINDOWIMG_USERLIST_FRIEND_BTSHOWONLINE`, `SP_2110_UI_UIWINDOWIMG_USERLIST_PARTY_BTCREATE`, `SP_2111_UI_UIWINDOWIMG_USERLIST_PARTY_BTINVITE`, `SP_2112_UI_UIWINDOWIMG_USERLIST_PARTY_BTKICK`, `SP_2113_UI_UIWINDOWIMG_USERLIST_PARTY_BTWITHDRAW`, `SP_2114_UI_UIWINDOWIMG_USERLIST_PARTY_BTWHISPER`, `SP_2115_UI_UIWINDOWIMG_USERLIST_PARTY_BTCHAT`, `SP_2116_UI_UIWINDOWIMG_USERLIST_PARTY_BTHP`, `SP_2117_UI_UIWINDOWIMG_USERLIST_PARTY_BTSEARCH`, `SP_2144_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTINVITE`, `SP_2145_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTKICK`, `SP_2146_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTWITHDRAW`, `SP_2147_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTWHISPER`, `SP_2148_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTCHAT`, `SP_2149_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTINFO`, `SP_2150_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTUP`, `SP_2151_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTDOWN`, `SP_2152_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTNOTICE`, `SP_2153_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTPARTYINVITE`, `SP_2165_UI_UIWINDOWIMG_USERLIST_BLACKLIST_BTADD`, `SP_2166_UI_UIWINDOWIMG_USERLIST_BLACKLIST_BTDELETE`, `SP_2670_UI_UIWINDOWIMG_USERLIST_TAB_ENABLED`, `SP_2671_UI_UIWINDOWIMG_USERLIST_TAB_DISABLED`, `SP_3342_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTWHERE`, `SP_3741_UI_UIWINDOWIMG_USERLIST_GUILD_GUILDINFO_BTGUILDBBS`, `SP_4044_UI_UIWINDOWIMG_USERLIST_PARTY_BTCHANGEBOSS`, `SP_5527_ARIAL`
  - `void __thiscall CUIUserList::OnCreate(CUIUserList *this, int Unknown)`
  - `sub_41C06C(a1: &word_AF0E08, a2: -1);`
  - `CUIWnd::OnCreate(a1: Unknown, a2: v303);`
  - `v3 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 88);`
  - `if ( v3 != 0 )`
  - `v4 = CInputSystem::CInputSystem(this: v3);`
  - `*(this + 545) = v4;`
  - `Unknown = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: v303);`
  - `if ( Unknown != 0 )`
  - `v5 = sub_91B9D4(a1: this);`
  - `*(this + 546) = v5;`
  - `v6 = sub_91BAB5(a1: this);`
  - `*(this + 547) = v6;`
  - `v7 = sub_91BC55(a1: this);`
  - `*(this + 548) = v7;`
  - `v8 = sub_91BDFE(a1: this);`
  - …（截断 16 行）

#### CUIUserList::OnMouseButton  (5323 B)

  - `_DWORD *__thiscall CUIUserList::OnMouseButton(CUIUserList *this, unsigned int a2, unsigned int a3, int a4, int a5)`
  - `if ( a2 == 513 )`
  - `v29 = *(this + 542);`
  - `v6 = *(this + 368);`
  - `if ( v6 == 0 )`
  - `*(this + 542) = 0;`
  - `ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF);`
  - `v28 = 33 * *(*(this + 367) + 56) + a5 - 113;`
  - `for ( i = 0; ; i += 32 )`
  - `v14 = *(this + 544);`
  - `v15 = *(v14 + 76);`
  - `if ( v15 == 0 )`
  - `if ( v31 >= *(v15 - 4) )`
  - `if ( *(this + 542) != 0 )`
  - `v16 = *(this + 543);`
  - `if ( v16 != 0 && *v16 != 0 )`
  - …（截断 16 行）

#### CUIUserList::OnMouseMove  (3037 B)

  - `signed int __thiscall CUIUserList::OnMouseMove(CTabParty **this, signed int a2, signed int a3)`
  - `v4 = *(this + 368);`
  - `if ( v4 != 0 )`
  - `if ( v5 != 0 )`
  - `if ( v6 != 0 )`
  - `if ( v7 != 0 )`
  - `if ( v7 != 1 )`
  - `v8 = sub_919677(a1: v22, a2, a3);`
  - `LOBYTE(v24) = 4;`
  - `ZXString<char>::operator=(a1: v8);`
  - `LOBYTE(v24) = 0;`
  - `ToolTipString = CTabGuildAlliance::GetToolTipString(a1: v21, a2, a3);`
  - `LOBYTE(v24) = 5;`
  - `ZXString<char>::operator=(a1: ToolTipString);`
  - `v11 = CTabGuild::GetToolTipString(this: *(this + 546), a2: &v20, a3: a2, a4: a3);`
  - `LOBYTE(v24) = 3;`
  - …（截断 16 行）

#### CUIUserList::ResetInfo  (1429 B)

  - `void __thiscall CUIUserList::ResetInfo(CUIUserList *this)`
  - `v2 = *(this + 369);`
  - `if ( v2 != 0 )`
  - `if ( v3 != 0 )`
  - `if ( v4 != 0 )`
  - `if ( v5 != 0 )`
  - `if ( v5 == 1 )`
  - `sub_9188D5(a1: *(this + 550));`
  - `CTabGuildAlliance::LoadData(this: *(this + 548));`
  - `if ( dword_BF11BC != 0 && *(*dword_BE7918 + 12398) != 0 )`
  - `sub_917B50(a1: dword_BF11BC);`
  - `sub_90EE97(this: *(this + 547));`
  - `if ( dword_BF11B8 != 0 && *(*dword_BE7918 + 12360) != 0 )`
  - `CWnd::InvalidateRect(this: *(this + 549), a2: 0);`
  - `sub_90B8D0(a1: *(this + 546));`
  - `sub_909029(a1: *(this + 545));`
  - …（截断 16 行）

#### CUIUserList::SetButton  (3494 B)

  - `int __thiscall CUIUserList::SetButton(CUIUserList *this)`
  - `for ( i = 11; i != 0; --i )`
  - `(*(*(*v2 + 4) + 36))(a1: *v2 + 4, a2: 0);`
  - `for ( j = 9; j != 0; --j )`
  - `(*(*(*v4 + 4) + 36))(a1: *v4 + 4, a2: 0);`
  - `for ( k = 12; k != 0; --k )`
  - `(*(*(*v6 + 4) + 36))(a1: *v6 + 4, a2: 0);`
  - `for ( m = 11; m != 0; --m )`
  - `(*(*(*v8 + 4) + 36))(a1: *v8 + 4, a2: 0);`
  - `for ( n = 2; n != 0; --n )`
  - `result = (*(*(*v11 + 4) + 36))(a1: *v11 + 4, a2: 0);`
  - `v14 = *(this + 369);`
  - `if ( v14 != 0 )`
  - `if ( v15 != 0 )`
  - `if ( v16 != 0 )`
  - `if ( v17 != 0 )`
  - …（截断 16 行）

#### CUIUserList::SetScrollBar  (1707 B)

  - `void __thiscall CUIUserList::SetScrollBar(CUIUserList *this, signed int a2)`
  - `v3 = *(this + 369);`
  - `if ( v3 == 0 )`
  - `if ( v4 == 0 )`
  - `if ( v5 == 0 )`
  - `if ( v6 == 0 )`
  - `if ( v6 == 1 )`
  - `if ( a2 != 0 )`
  - `(*(**(this + 368) + 12))(a1: *(this + 368));`
  - `(*(**(this + 368) + 32))(a1: *(this + 368), a2: this, a3: 2000, a4: 1, a5: 3, a6: 290, a7: v7, a8: v8, a9: 0);`
  - `sub_843B0E(a1: 290, a2: v7);`
  - `sub_4D82FB(a1: v8);`
  - `*(*(this + 368) + 52) = 165;`
  - `v9 = v10 + 18 * *(this + 505) - v8;`
  - `if ( v9 > 0 )`
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 368), a2: v9 / 33 + 2);`
  - …（截断 16 行）

### CUIFamily（3 函数）


#### CUIFamily::Draw  (10879 B)

- SP_ 节点：`SP_4591_YOU_DO_NOT_HAVE_A_FAMILY_YET`, `SP_4593_S_FAMILY`
  - `const struct tagRECT *__thiscall CUIFamily::Draw(CUIFamily *this, const struct tagRECT *a2)`
  - `CWnd::Draw(this, a2);`
  - `CWnd::GetCanvas(a1: &a2);`
  - `get_basic_font(a1: &v65, fType: 1);`
  - `v3 = *(*(this + 703) + 24);`
  - `LOBYTE(v66) = 2;`
  - `if ( v3 != 0 && *v3 != 0 )`
  - `v55 = *(*(this + 703) + 24);`
  - `LOBYTE(v66) = 3;`
  - `ZXString<char>::Format(a1: &v62, a2: v54, a3: v55);`
  - `LOBYTE(v66) = 4;`
  - `ZXString<char>::operator=(a1: v7);`
  - `v54 = sub_428226(a1: &v62);`
  - `get_basic_font(a1: &v50, fType: 0);`
  - `sub_4281DD(a1: &a2);`
  - `sub_808177(a1: v49, a2: v50, a3: v51, a4: v52, a5: v53, a6: v54, a7: v55);`
  - …（截断 16 行）

#### CUIFamily::OnButtonClicked  (4393 B)

- SP_ 节点：`SP_4587_ENTER_THE_FAMILY_PRECEPTS`
  - `void __thiscall CUIFamily::OnButtonClicked(CUIFamily *this, unsigned int a2)`
  - `switch ( a2 )`
  - `CWvsContext::UI_Open(this: *dword_BE7918, a2: 28, a3: -1);`
  - `v19 = sub_42861D();`
  - `CharacterName = CWvsContext::GetCharacterName(a1: v19);`
  - `ZXString<char>::GetBuffer(a1: CharacterName, a2: 0xFFFFFFFF);`
  - `sub_42861D();`
  - `sub_A34040(a1: 0);`
  - `v15 = *(this + 704);`
  - `if ( *v15 == 0 || *(v18 - 4) == 0 )`
  - `if ( v18 != 0 )`
  - `v18 = *(v18 - 4);`
  - `*(this + 705) += v18 - 1;`
  - `if ( *v15 == 0 || *(*v15 - 4) == 0 )`
  - `++*(this + 705);`
  - `v7 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - …（截断 16 行）

#### CUIFamily::OnCreate  (7361 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_4571_UI_UIWINDOWIMG_FAMILY_BTOK`, `SP_4572_UI_UIWINDOWIMG_FAMILY_BTTREE`, `SP_4573_UI_UIWINDOWIMG_FAMILY_BTLEFT`, `SP_4574_UI_UIWINDOWIMG_FAMILY_BTRIGHT`, `SP_4575_UI_UIWINDOWIMG_FAMILY_BTFAMILYPRECEPT`, `SP_4576_UI_UIWINDOWIMG_FAMILY_BTSPECIAL`, `SP_4577_UI_UIWINDOWIMG_FAMILY_BTJUNIORENTRY`
  - `void __thiscall CUIFamily::OnCreate(CWnd *this, _DWORD *a2)`
  - `sub_41C06C(a1: &word_AF0E08, a2: -1);`
  - `CUIWnd::OnCreate(a1: a2, a2: v38);`
  - `CWnd::InvalidateRect(this, a2: 0);`
  - `*ZArray<CUIWnd *>::InsertBefore(a1: -1) = this;`
  - `memset(v39, 0, sizeof(v39));`
  - `LOBYTE(v42) = 1;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `LOBYTE(v42) = 0;`
  - `v5 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v42) = 2;`
  - `if ( v5 != 0 )`
  - `v6 = CCtrlButton::CCtrlButton(this: v5);`
  - `v7 = sub_4284FF(a1: v6);`
  - `(*(**(v7 + 4) + 32))(a1: *(v7 + 4), a2: this, a3: 2005, a4: 95, a5: 77, a6: 1, a7: v39);`
  - `(*(*(*(this + 365) + 4) + 28))(a1: *(this + 365) + 4, a2: 0);`
  - …（截断 16 行）

### CUIFamilyChart（8 函数）


#### CUIFamilyChart::Draw  (8097 B)

- SP_ 节点：`SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_2_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTPASSWORD`, `SP_4591_YOU_DO_NOT_HAVE_A_FAMILY_YET`, `SP_4593_S_FAMILY`, `SP_4594_SENIORD_PPL`, `SP_4595_JUNIORD_PPL`
  - `int __thiscall CUIFamilyChart::Draw(CUIFamilyChart *this, const struct tagRECT *a2)`
  - `CWnd::Draw(this, a2);`
  - `CWnd::GetCanvas(a1: &v48);`
  - `v3 = *(this + 365);`
  - `if ( v3 != 0 )`
  - `sub_80C700(a1: v43, a2: &v44);`
  - `v4 = sub_9887FC(a1: v43, a2: v44);`
  - `LOBYTE(v49) = 1;`
  - `p_pvargDest = sub_428226(a1: v4);`
  - `get_basic_font(a1: &v33, fType: 1);`
  - `sub_4281DD(a1: &v48);`
  - `sub_808177(a1: v32, a2: v33, a3: v34, a4: v35, a5: v36, a6: p_pvargDest, a7: p_pvarg);`
  - `LOBYTE(v49) = 0;`
  - `v6 = *(this + 365);`
  - `LOBYTE(v49) = 2;`
  - `if ( v6 != 0 )`
  - …（截断 16 行）

#### CUIFamilyChart::OnCreate  (2761 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_4581_UI_UIWINDOWIMG_FAMILYTREE_BTJUNIORENTRY`, `SP_4582_UI_UIWINDOWIMG_FAMILYTREE_BTBYE`
  - `void __thiscall CUIFamilyChart::OnCreate(CWnd *this, _DWORD *a2)`
  - `sub_41C06C(a1: &word_AF0E08, a2: -1);`
  - `CUIWnd::OnCreate(a1: a2, a2: v13);`
  - `CWnd::InvalidateRect(this, a2: 0);`
  - `*ZArray<CUIWnd *>::InsertBefore(a1: -1) = this;`
  - `memset(v14, 0, sizeof(v14));`
  - `LOBYTE(v17) = 1;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `LOBYTE(v17) = 0;`
  - `v5 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v17) = 2;`
  - `if ( v5 != 0 )`
  - `v6 = CCtrlButton::CCtrlButton(this: v5);`
  - `v7 = sub_4284FF(a1: v6);`
  - `(*(**(v7 + 4) + 32))(a1: *(v7 + 4), a2: this, a3: 2000, a4: 21, a5: 57, a6: 1, a7: v14);`
  - `(*(*(*(this + 402) + 4) + 28))(a1: *(this + 402) + 4, a2: 0);`
  - …（截断 16 行）

#### CUIFamilyChart::OnMouseButton  (1453 B)

  - `void __userpurge CUIFamilyChart::OnMouseButton(int a1@<ecx>, _anonymous_namespace_ *a2@<ebx>, int a3, int a4, POINT pt)`
  - `if ( *(a1 + 1588) != 0 )`
  - `v10 = (a1 - 4);`
  - `ItemByPoint = CUIFamilyChart::_FindItemByPoint(pt);`
  - `if ( ItemByPoint >= 0 && (ItemByPoint == 2 || ItemByPoint == 5 || ItemByPoint == 6) )`
  - `v8 = *(a1 + 8 * ItemByPoint + 1456);`
  - `if ( v8 != 0 )`
  - `InterlockedIncrement(lpAddend: (v8 + 4));`
  - `if ( v9 != 0 )`
  - `*(a1 + 1592) = v7;`
  - `*(a1 + 1592) = -1;`
  - `if ( a3 == 513 )`
  - `if ( v7 != 2 && *(a1 + 8 * v7 + 1456) == 0 )`
  - ``anonymous namespace'::RegisterJunior_(this: a2);`
  - `else if ( a3 == 515 && *(a1 + 1592) >= 0 )`
  - `CUIFamilyChart::_UnregisterCharacter(this: v10);`
  - …（截断 16 行）

#### CUIFamilyChart::OnMouseMove  (2702 B)

- SP_ 节点：`SP_4632_THIS_COVERS_EVERYONE_IN_THE_FAMILY_R_NTHE_NUMBER_MAY_BE_HIGHER_THAN_THE_ONE_SHOW`
  - `int __thiscall CUIFamilyChart::OnMouseMove(_DWORD *this, POINT pt)`
  - `if ( *(this + 397) != 0 )`
  - `ItemByPoint = CUIFamilyChart::_FindItemByPoint(pt);`
  - `if ( ItemByPoint >= 0`
  - `&& (ItemByPoint == 2 || ItemByPoint == 5 || ItemByPoint == 6)`
  - `&& ((v5 = *(this + 2 * ItemByPoint + 364), v16 = *(this + 2 * v4 + 364), v5 != 0)`
  - `&& (InterlockedIncrement(lpAddend: (v5 + 4)), v5 = v16, v16 != 0)`
  - `if ( v5 != 0 )`
  - `ZRef<CUIFamilyChart::FamilyItem>::~ZRef<CUIFamilyChart::FamilyItem>(a1: 0);`
  - `if ( *(this + 399) != v6 )`
  - `*(this + 399) = v6;`
  - `CWnd::InvalidateRect(this: (this - 1), a2: v15);`
  - `if ( PtInRect(lprc: &stru_BE27D0, pt) )`
  - `if ( v4 < 0 || *(this + 2 * v4 + 364) == 0 )`
  - `CUIToolTip::ClearToolTip(this: (this + 404));`
  - `if ( v4 == 3 || v4 == 0 && (v7 = *(*(this + 364) + 12)) == *(*(this + 370) + 12) )`
  - …（截断 16 行）

#### CUIFamilyChart::_DrawChartItem  (13053 B)

- SP_ 节点：`SP_4579_UI_UIWINDOWIMG_FAMILYTREE_PLATELEADER_D`, `SP_4580_UI_UIWINDOWIMG_FAMILYTREE_PLATEOTHERS_D`, `SP_4583_UI_UIWINDOWIMG_FAMILYTREE_SELECTED`, `SP_4588_YOU_DO_NOT_HAVE_FAMILY_MEMBERS`, `SP_4592__PLEASE_ADD_A_JUNIOR_`
  - `int __thiscall CUIFamilyChart::_DrawChartItem(CUIFamilyChart *this, int a2)`
  - `v3 = *(this + 2 * a2 + 365);`
  - `sub_80C50A(a1: v60);`
  - `if ( a2 == 0 )`
  - `CWnd::GetCanvas(a1: &v71);`
  - `LOBYTE(v72) = 1;`
  - `if ( v61 != 0 )`
  - `if ( a2 != 3 )`
  - `LOBYTE(v72) = 4;`
  - `if ( a2 != 0 )`
  - `LOBYTE(v72) = 5;`
  - `sub_416821(a1: &v68, a2: *StringW, a3: *(v61 + 20) != 0);`
  - `if ( (v70 & 2) != 0 )`
  - `if ( (v70 & 1) != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v72) = 7;`
  - …（截断 16 行）

#### CUIFamilyChart::_FindItemByPoint  (731 B)

  - `int __stdcall CUIFamilyChart::_FindItemByPoint(POINT pt)`
  - `while ( 1 )`
  - `rc.left = *(v2 - 1);`
  - `if ( PtInRect(lprc: &rc, pt: __PAIR64__(pt.y, x)) )`
  - `if ( v2 >= &stru_BE27D0.top )`

#### CUIFamilyChart::_GetMineToolTip  (7479 B)

- SP_ 节点：`SP_4635_C_YOU_HAVE_SURPASSED_YOUR_SENIORS_LEVEL_SO_YOUR_CUMULATIVE_REP_R_NCWILL_BE_REDUC`, `SP_4637_REPS_RECORDED_TO_YOUR_SENIOR__S_R_N`, `SP_4638_ENTITLEMENT_IN_USE__R_N`, `SP_4639_ENTITLEMENT_USED_TODAY__R_N`, `SP_4640_S___D_TIMES_R_N`, `SP_4645__R_N`
  - `_DWORD *__thiscall CUIFamilyChart::_GetMineToolTip(int **this, _DWORD *a2)`
  - `v3 = *(this + 371);`
  - `sub_80C50A(a1: v36);`
  - `if ( v37 != 0 )`
  - `v34 = *(v37 + 32);`
  - `LOBYTE(v45) = 2;`
  - `v5 = sub_9887FC(a1: &v42, a2: v34);`
  - `LOBYTE(v45) = 3;`
  - `LOBYTE(v45) = 4;`
  - `ZXString<char>::Format(a1: &v44, a2: v33, a3: v35);`
  - `if ( v41 != 0 )`
  - `if ( v42 != 0 )`
  - `v8 = *(v37 + 56);`
  - `if ( v8 != 0 && *(v8 + 18) < *(v37 + 18) )`
  - `LOBYTE(v45) = 5;`
  - `v11 = v10 != 0 ? *(v10 - 1) : 0;`
  - …（截断 16 行）

#### CUIFamilyChart::_UnregisterCharacter  (8563 B)

- SP_ 节点：`SP_4586_ENTER_THE_NAME_OF_THE_CHARACTER_YOU_WISH_TO_DELETE`
  - `int __thiscall CUIFamilyChart::_UnregisterCharacter(CUIFamilyChart *this)`
  - `v2 = *(this + 399);`
  - `if ( v2 >= 0 )`
  - `v35 = *(this + 2 * v2 + 365);`
  - `sub_80C50A(a1: v34);`
  - `v18 = *(this + 371);`
  - `sub_80C50A(a1: v41);`
  - `if ( v35 != 0 && (v19 = v42, v42 != 0) )`
  - `v20 = *(v35 + 12);`
  - `if ( result != 0 )`
  - `InterlockedIncrement(lpAddend: (result + 4));`
  - `LOBYTE(v46) = 10;`
  - `if ( result != 0 && *(result + 12) == v20 )`
  - `if ( v36 != 0 )`
  - `InterlockedIncrement(lpAddend: (v36 + 4));`
  - `LOBYTE(v46) = 11;`
  - …（截断 16 行）

### CUIFriendGroup（2 函数）


#### CUIFriendGroup::OnOK  (2957 B)

  - `int __thiscall CUIFriendGroup::OnOK(char **this)`
  - `CCtrlEdit::GetText(this: *(this + 32), a2: &v17);`
  - `if ( v17 != 0 && *v17 != 0 )`
  - `v5 = (this + 43);`
  - `while ( 1 )`
  - `if ( *(*v5 + 72) != 0 )`
  - `if ( v4 >= 7 )`
  - `ZXString<char>::ZXString<char>(this: &v10, a2: aYouHaveNotSele, a3: 0xFFFFFFFF);`
  - `if ( *(this + 60) != 0 || ZXString<char>::operator==(a1: &v17) == 0 )`
  - `LOBYTE(v18) = 1;`
  - `sub_A3FC85(a1: &v16);`
  - `for ( i = 0; v16 != 0 && i < *(v16 - 1); ++i )`
  - `if ( ZXString<char>::operator==(a1: &v17) != 0 )`
  - `ZXString<char>::ZXString<char>(this: &v10, a2: aThisNameAlread, a3: 0xFFFFFFFF);`
  - `CUtilDlg::Notice(sMsg: v10, sSndName: v11, ppDialog: v12, bAutoSeparated: v13, bIsTightLine: v14);`
  - `LOBYTE(v18) = 0;`
  - …（截断 16 行）

#### CUIFriendGroup::SetRet  (465 B)

  - `int __thiscall CUIFriendGroup::SetRet(CUIFriendGroup *this, int a2)`
  - `if ( (a2 != 1 || (result = CUIFriendGroup::OnOK()) != 0) && *(this + 28) == 0 )`
  - `v4 = *(this + 5) == 0;`
  - `*(this + 27) = a2;`
  - `*(this + 28) = 1;`
  - `if ( !v4 )`
  - `return CWnd::Destroy(a1: this);`

### CTabFriend（12 函数）


#### CTabFriend::ChangeBlockOption  (442 B)

  - `void __thiscall CTabFriend::ChangeBlockOption(CTabFriend *this, unsigned int a2, __int16 a3, int a4)`
  - `if ( CConfig::ChangeBlockOption(this: TSingleton<CConfig>::ms_pInstance, a2, a3, a4) == 0`
  - `|| CWvsContext::ChangeBlockOption(this: *dword_BE7918, a2, a3, a4) == 0 )`
  - `CConfig::LoadBlockFriend(this: TSingleton<CConfig>::ms_pInstance);`
  - `CWvsContext::LoadFriend(this: *dword_BE7918);`

#### CTabFriend::Draw  (20858 B)

- SP_ 节点：`SP_2081_D_D`, `SP_5641_NONE`
  - `int __thiscall CTabFriend::Draw(_DWORD *this, int a2, int a3)`
  - `v5 = *(*(this + 18) + 2208);`
  - `sub_428902(a1: &v113);`
  - `v107 = *(v3[18] + 2204);`
  - `sub_428902(a1: &v107);`
  - `LOBYTE(v122) = 3;`
  - `while ( 1 )`
  - `if ( v7 == 0 || v116 >= *(v7 - 4) )`
  - `v6 += sub_90B2DD(a1: v4 + v7 + 28);`
  - `LOBYTE(v122) = 4;`
  - `LOBYTE(v122) = 5;`
  - `ZXString<char>::Format(a1: &v105, a2: v10, a3: v6);`
  - `if ( v119 != 0 )`
  - `sub_402F85(pvargDest: &v102, pvargSrc: &pvargSrc);`
  - `LOBYTE(v122) = 6;`
  - `v11 = sub_428226(a1: &v105);`
  - …（截断 16 行）

#### CTabFriend::GetFriendItem  (938 B)

  - `int __thiscall CTabFriend::GetFriendItem(_DWORD *this, int a2, void *a3)`
  - `memset(a1: a3, a2: 0, a3: 0x60u);`
  - `v5 = *(this + 19);`
  - `if ( v5 == 0 || v10 >= *(v5 - 4) )`
  - `v6 = *(v4 + v5 + 28);`
  - `v7 = (v4 + v5 + 28);`
  - `for ( i = v6; ; i += 24 )`
  - `if ( v6 == 0 || v8 >= *(v6 - 1) )`
  - `if ( *i == a2 )`
  - `sub_90B21A(a1: *v7 + 96 * v8);`
  - `return sub_908FF6(a1: a3);`

#### CTabFriend::IsGroupBlocked  (1002 B)

  - `int __thiscall CTabFriend::IsGroupBlocked(_DWORD *this, char a2)`
  - `for ( i = 0; ; i += 32 )`
  - `v4 = *(this + 19);`
  - `if ( v4 == 0 || v10 >= *(v4 - 4) )`
  - `if ( ZXString<char>::operator==(a1: &a2) != 0 )`
  - `v5 = *(i + *(this + 19) + 28);`
  - `v7 = (v5 + 80);`
  - `while ( v5 != 0 && v6 < *(v5 - 4) )`
  - `if ( v7[1] == 0 && *v7 == 0 )`

#### CTabFriend::OnBlockFriend  (4516 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_4143_BUDDY_CHAT_WITH_S_HAS_BEEN_DISABLED`, `SP_4144_BUDDY_CHAT_WITH_S_HAS_BEEN_ENABLED`
  - `void __thiscall CTabFriend::OnBlockFriend(CTabFriend *this)`
  - `v2 = *(this + 18);`
  - `v3 = *(v2 + 2172);`
  - `if ( v3 != 0 || (v4 = *(v2 + 2176)) != 0 && *v4 != 0 )`
  - `if ( v3 == 0 )`
  - `ZXString<char>::operator=(a1: v2 + 2176);`
  - `v27 = CTabFriend::IsGroupBlocked(a1: v20) == 0;`
  - `for ( i = 0; ; i += 32 )`
  - `v5 = *(this + 19);`
  - `if ( v5 == 0 || v24 >= *(v5 - 4) )`
  - `if ( v29 == 0 || ZXString<char>::operator==(a1: *(this + 18) + 2176) != 0 )`
  - `v23 = (i + *(this + 19) + 28);`
  - `for ( j = 0; ; j += 96 )`
  - `if ( *v23 == 0 || v26 >= *(v6 - 4) )`
  - `v7 = (j + v6);`
  - `if ( v29 != 0 )`
  - …（截断 16 行）

#### CTabFriend::OnButtonClicked  (7789 B)

  - `int __thiscall CTabFriend::OnButtonClicked(CTabFriend **this, unsigned int a2)`
  - `switch ( a2 )`
  - `ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF);`
  - `sub_909FCA(a1: 0);`
  - `ZXString<char>::ZXString<char>(this: &v4, a2: &sStrDefault, a3: 0xFFFFFFFF);`
  - `sub_90ADD3(a1: v4, a2: v5);`
  - `sub_90B0B6(a1: *(this + 545));`
  - `sub_90B0C1(a1: *(this + 545));`
  - `CTabFriend::OnModify(this: *(this + 545));`
  - `sub_90A303(a1: *(this + 545));`
  - `sub_90A8DC(a1: *(this + 545));`
  - `CTabFriend::OnWhisper(this: *(this + 545));`
  - `CTabFriend::OnSendMemo(this: *(this + 545));`
  - `CTabFriend::OnPartyInvite(this: *(this + 545));`
  - `CTabFriend::OnBlockFriend(this: *(this + 545));`
  - `sub_9111BE(a1: *(this + 547));`
  - …（截断 16 行）

#### CTabFriend::OnClick  (5465 B)

- SP_ 节点：`SP_2103_UI_UIWINDOWIMG_USERLIST_FRIEND_BTBLOCK`, `SP_2104_UI_UIWINDOWIMG_USERLIST_FRIEND_BTUNBLOCK`
  - `void __thiscall CTabFriend::OnClick(CTabFriend *this)`
  - `ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF);`
  - `v2 = *(this + 18);`
  - `if ( *(v2 + 2172) != 0 )`
  - `v31 = *(v2 + 2172);`
  - `if ( CTabFriend::GetFriendItem(this, a2: v31, a3: v34) >= 0 )`
  - `for ( i = 32; i < 88; i += 8 )`
  - `v19 = *(i + *(this + 18) + 1484) + 4;`
  - `(*(*v19 + 28))(a1: v19, a2: 1);`
  - `LOBYTE(v39) = 3;`
  - `v20 = is_offline(a1: v34);`
  - `if ( v20 )`
  - `v22 = ZArray<long>::InsertBefore(a1: -1);`
  - `v23 = ZArray<long>::InsertBefore(a1: v33[0]);`
  - `*ZArray<long>::InsertBefore(a1: v33[0]) = 9;`
  - `if ( v35[20] != 0 || v35[19] != 0 )`
  - …（截断 16 行）

#### CTabFriend::OnDblClick  (2307 B)

  - `void __thiscall CTabFriend::OnDblClick(CTabFriend *this)`
  - `v2 = *(this + 18);`
  - `if ( *(v2 + 2172) != 0 )`
  - `v8 = *(v2 + 2172);`
  - `if ( CTabFriend::GetFriendItem(this, a2: v8, a3: v10) < 0 )`
  - `sub_9094BE(a1: v10);`
  - `if ( v11[20] != 0 || v11[19] != 0 )`
  - `ZXString<char>::GetBuffer(a1: aYouCanUseItAft, a2: 0xFFFFFFFF);`
  - `LOBYTE(v14) = 1;`
  - `sub_428211(a1: &v13);`
  - `if ( CUtilDlg::YesNo(a1: v5, a2: v6, a3: v7, a4: v8, a5: v9) != 6 )`
  - `LOBYTE(v14) = 0;`
  - `CTabFriend::OnBlockFriend(this);`
  - `CTabFriend::GetFriendItem(this, a2: *(*(this + 18) + 2172), a3: v10);`
  - `if ( is_offline(a1: v10) )`
  - `CTabFriend::OnSendMemo(this);`
  - …（截断 16 行）

#### CTabFriend::OnModify  (2267 B)

  - `void __thiscall CTabFriend::OnModify(CTabFriend *this)`
  - `v2 = *(this + 18);`
  - `if ( *(v2 + 2172) != 0 )`
  - `v13 = *(v2 + 2172);`
  - `CTabFriend::GetFriendItem(a1: v13, a2: v15);`
  - `ZXString<char>::operator=(a1: v16);`
  - `sub_909FCA(a1: v14);`
  - `v3 = *(v2 + 2176);`
  - `if ( v3 != 0 && *v3 != 0 )`
  - `v4 = ZXString<char>::operator==(a1: &dword_BF11A4);`
  - `if ( v4 != 0 )`
  - `v6 = *(this + 18);`
  - `v7 = ZXString<char>::Format(a1: &v19, a2: aSCannotBeChang, a3: *(v6 + 2176));`
  - `sub_428211(a1: v7);`
  - `CUtilDlg::Notice(sMsg: v10, sSndName: v11, ppDialog: v12, bAutoSeparated: v13, bIsTightLine: v14);`
  - `v8 = *(this + 18);`
  - …（截断 16 行）

#### CTabFriend::OnPartyInvite  (952 B)

  - `void __thiscall CTabFriend::OnPartyInvite(CTabFriend *this)`
  - `v1 = *(this + 18);`
  - `v2 = *(v1 + 2172);`
  - `if ( v2 != 0 && v2 != *(*dword_BE7918 + 8352) )`
  - `FriendItem = CTabFriend::GetFriendItem(a1: *(v1 + 2172), a2: &v4);`
  - `if ( v4 != 0 && FriendItem <= 1 )`
  - `if ( get_field() != 0 )`
  - `CField::SendJoinPartyMsg(a1: v5);`

#### CTabFriend::OnSendMemo  (2357 B)

- SP_ 节点：`SP_729_YOU_CAN_USE_IT_AFTER_PURCHASING_THE_MEMO`
  - `void __thiscall CTabFriend::OnSendMemo(CTabFriend *this)`
  - `v1 = (*(this + 18) + 2172);`
  - `if ( *v1 != 0 )`
  - `CTabFriend::GetFriendItem(a1: v8, a2: v10);`
  - `CashItemSlotPosition = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v13) + 4);`
  - `if ( v14 != 0 )`
  - `sub_428C44(this: &v13, a2: 0);`
  - `CashItemSlotPosition = CharacterData::FindCashItemSlotPosition(this: CashItemSlotPosition, a2: 5, a3: &loc_4DAAD0);`
  - `if ( CashItemSlotPosition > 0 )`
  - `ZXString<char>::operator=(a1: v11);`
  - `CWvsContext::SendConsumeCashItemUseRequest(this: v2, a2: CashItemSlotPosition, a3: &loc_4DAAD0, a4: 0, a5: v9);`
  - `CUtilDlg::Notice(sMsg: v5, sSndName: v6, ppDialog: v7, bAutoSeparated: v8, bIsTightLine: v9);`

#### CTabFriend::OnWhisper  (3518 B)

  - `void __thiscall CTabFriend::OnWhisper(CTabFriend *this)`
  - `v1 = *(this + 18);`
  - `v2 = *(v1 + 2172);`
  - `if ( v2 != 0 && v2 != *(*dword_BE7918 + 8352) )`
  - `v14 = *(v1 + 2172);`
  - `FriendItem = CTabFriend::GetFriendItem(a1: v14, a2: &v18);`
  - `if ( v18 != 0 && FriendItem <= 1 )`
  - `if ( TSingleton<CUIStatusBar>::ms_pInstance != 0 )`
  - `LOBYTE(v25) = 2;`
  - `ZXString<char>::Format(a1: &v24, a2: aChangeHasBeenM, a3: v19[0]);`
  - `if ( sub_49F67D(a1: 0) != 0 )`
  - `v5 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 216);`
  - `LOBYTE(v25) = 3;`
  - `if ( v5 != 0 )`
  - `v6 = CUtilDlg::CUtilDlg(this: v5);`
  - `v7 = v6 != 0 ? (v6 + 2) : 0;`
  - …（截断 16 行）

### CTabParty（10 函数）


#### CTabParty::1PARTYITEM  (191 B)

  - `void __thiscall CTabParty::PARTYITEM::~PARTYITEM(CTabParty::PARTYITEM *this)`

#### CTabParty::Draw  (75299 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_2081_D_D`, `SP_25_HERO`, `SP_5641_NONE`
  - `int *__thiscall CTabParty::Draw(int *this, int *arg0)`
  - `v302 = *(*(this + 9) + 2208);`
  - `sub_428902(a1: &v302);`
  - `v313 = *(*(this + 9) + 2204);`
  - `sub_428902(a1: &v313);`
  - `v296 = *(*(this + 9) + 2212);`
  - `sub_428902(a1: &v296);`
  - `LOBYTE(v319) = 8;`
  - `sub_402F85(pvargDest: &v306, pvargSrc: &pvargSrc);`
  - `a2 = *(this + 1);`
  - `LOBYTE(v319) = 9;`
  - `if ( arg0 == 0 )`
  - `v4 = (*(v3 + 128))(`
  - `if ( v4 < 0 )`
  - `if ( v5 < 0 )`
  - `v6 = *(this + 10);`
  - …（截断 16 行）

#### CTabParty::GetMemberIDFromPoint  (1697 B)

  - `int __thiscall CTabParty::GetMemberIDFromPoint(CTabParty *this, int a2, int a3, int a4)`
  - `for ( i = 0; ; i += 80 )`
  - `v7 = *(this + 10);`
  - `if ( v7 == 0 || v5 >= *(v7 - 4) )`
  - `if ( dword_BF0484(a1: i + v7 + 28, a2: v17, a3: v18) != 0 )`
  - `v8 = *(this + 10);`
  - `return *(v9 + v8);`
  - `for ( j = 0; ; j += 80 )`
  - `v12 = *(this + 12);`
  - `if ( v12 == 0 || v10 >= *(v12 - 4) )`
  - `if ( dword_BF0484(a1: j + v12 + 28, a2: v17, a3: v18) != 0 )`
  - `v8 = *(this + 12);`
  - `for ( k = 0; ; k += 80 )`
  - `v15 = *(this + 11);`
  - `if ( v15 == 0 || v13 >= *(v15 - 4) )`
  - `if ( dword_BF0484(a1: k + v15 + 28, a2: v17, a3: v18) != 0 )`
  - …（截断 16 行）

#### CTabParty::GetPartyItem  (1381 B)

  - `int __thiscall CTabParty::GetPartyItem(_DWORD *this, int a2, void *a3)`
  - `memset(a1: a3, a2: 0, a3: 80u);`
  - `for ( i = 0; ; i += 80 )`
  - `v6 = *(this + 10);`
  - `if ( v6 == 0 || v4 >= *(v6 - 4) )`
  - `if ( *(i + v6) == a2 )`
  - `sub_90BD6F(a1: *(this + 10) + 80 * v4);`
  - `for ( j = 0; ; j += 80 )`
  - `v10 = *(this + 11);`
  - `if ( v10 == 0 || v8 >= *(v10 - 4) )`
  - `if ( *(j + v10) == a2 )`
  - `sub_90BD6F(a1: *(this + 11) + 80 * v8);`
  - `for ( k = 0; ; k += 80 )`
  - `v13 = *(this + 12);`
  - `if ( v13 == 0 || v11 >= *(v13 - 4) )`
  - `if ( *(v13 + k) == a2 )`
  - …（截断 16 行）

#### CTabParty::GetToolTipString  (4007 B)

- SP_ 节点：`SP_5554_ONLINE_PARTY_MEMBERS___TOTAL_REGISTERED_PARTY_MEMBERS`, `SP_691_THE_USER_IS_CURRENTLY_AT_ANOTHER_LOCATION`, `SP_718_CASH_SHOP`
  - `_DWORD *__thiscall CTabParty::GetToolTipString(CTabParty *this, _DWORD *a2, int rx, int ry)`
  - `v5 = *(this + 9);`
  - `v16 = 33 * *(*(v5 + 1472) + 56) + ry - 53;`
  - `if ( dword_BF0484(a1: v5 + 2028, a2: rx - 6, a3: v16) != 0 )`
  - `LOBYTE(v26) = 0;`
  - `if ( dword_BF0484(a1: *(this + 9) + 2044, a2: v23, a3: v24) != 0 )`
  - `MemberIDFromPoint = CTabParty::GetMemberIDFromPoint(this, a2: rx, a3: ry, a4: *(*(*(this + 9) + 1472) + 56));`
  - `if ( MemberIDFromPoint != 0 )`
  - `PartyItem = CTabParty::GetPartyItem(this, a2: MemberIDFromPoint, a3: v17);`
  - `if ( v22[4] != 0 && dword_BF0484(a1: v22, a2: v23, a3: v24) != 0 )`
  - `ZXString<char>::operator=(a1: v19);`
  - `if ( PartyItem == 2 && dword_BF0484(a1: v21, a2: v23, a3: v24) != 0 )`
  - `if ( v20 == *(*dword_BE7918 + 0x2058) )`
  - `CTabParty::PARTYITEM::~PARTYITEM(this: v17);`
  - `if ( v20 < 0 )`
  - `LOBYTE(v26) = 4;`
  - …（截断 16 行）

#### CTabParty::OnChangeBoss  (2298 B)

- SP_ 节点：`SP_4045_DO_YOU_WISH_TO_PASS_THE_LEADER_POSITION_TO__R_NS`
  - `void __thiscall CTabParty::OnChangeBoss(_DWORD *this)`
  - `v1 = *(this + 9);`
  - `v2 = *(v1 + 2172);`
  - `if ( v2 != 0 && v2 != *(*dword_BE7918 + 8352) )`
  - `v9 = *(v1 + 2172);`
  - `CTabParty::GetPartyItem(this, a2: v9, a3: v11);`
  - `LOBYTE(v16) = 1;`
  - `LOBYTE(v16) = 2;`
  - `ZXString<char>::Format(a1: &v15, a2: v9, a3: v12[0]);`
  - `sub_428211(a1: &v15);`
  - `if ( CUtilDlg::YesNo(a1: v6, a2: v7, a3: v8, a4: v9, a5: v10) == 6 )`
  - `get_field();`
  - `CField::SendChangePartyBossMsg(a1: v10);`
  - `LOBYTE(v16) = 0;`

#### CTabParty::OnChat  (1187 B)

  - `void __thiscall CTabParty::OnChat(CTabParty *this)`
  - `v1 = *(this + 9);`
  - `v2 = *(v1 + 2172);`
  - `if ( v2 != 0 && v2 != *(*dword_BE7918 + 8352) )`
  - `v4 = *(v1 + 2172);`
  - `CTabParty::GetPartyItem(this, a2: v4, a3: v6);`
  - `if ( dword_BF0F54 == 0 && CUIMessenger::TryNew(a1: 0) != 0 )`
  - `ZXString<char>::operator=(a1: v7);`
  - `LOBYTE(v9) = 0;`
  - `v3 = sub_851F49();`
  - `CUIMessenger::SendInviteMsg(this: v3, a2: v5);`

#### CTabParty::OnInvite  (1937 B)

- SP_ 节点：`SP_712_PLEASE_ENTER_THE_NAME_OF_THE_CHARACTER_R_NYOULL_INVITE_TO_YOUR_PARTY`
  - `int CTabParty::OnInvite()`
  - `v0 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v0 != 0 )`
  - `v1 = CUtilDlgEx::CUtilDlgEx(this: v0);`
  - `v11 = v2 != 0 ? (v2 - 8) : 0;`
  - `sub_4289CD(a1: v10);`
  - `LOBYTE(v13) = 1;`
  - `v5 = sub_428688(a1: v10);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v5, a2: 3, a3: 0, a4: v7, a5: v8, a6: InputStr_Result);`
  - `CUtilDlgEx::SetUtilDlgEx_INPUT_STR(this: v11, a2: &sStrDefault, a3: 4, a4: 12, a5: 0, a6: 0);`
  - `CUtilDlgEx::CreateUtilDlgEx(this: v11);`
  - `if ( CDialog::DoModal(a1: v11) == 1 )`
  - `InputStr_Result = CUtilDlgEx::GetInputStr_Result(this: v11, a2: &v12);`
  - `LOBYTE(v13) = 3;`
  - `get_field();`
  - `CField::SendJoinPartyMsg(a1: InputStr_Result);`
  - …（截断 16 行）

#### CTabParty::OnKick  (902 B)

  - `void __thiscall CTabParty::OnKick(_DWORD *this)`
  - `v2 = *(this + 9);`
  - `v3 = *(v2 + 2172);`
  - `if ( v3 != 0 && v3 != *(*dword_BE7918 + 8352) )`
  - `if ( CTabParty::GetPartyItem(this, a2: *(v2 + 2172), a3: v4) == -1 )`
  - `get_field();`
  - `CField::SendKickPartyMsg(a1: v5);`
  - `*(*(this + 9) + 2172) = 0;`

#### CTabParty::OnWhisper  (1424 B)

  - `void __thiscall CTabParty::OnWhisper(_DWORD *this)`
  - `v1 = *(this + 9);`
  - `v2 = *(v1 + 2172);`
  - `if ( v2 != 0 && v2 != *(*dword_BE7918 + 8352) )`
  - `v5 = *(v1 + 2172);`
  - `CTabParty::GetPartyItem(this, a2: v5, a3: v9);`
  - `if ( TSingleton<CUIStatusBar>::ms_pInstance != 0 )`
  - `sub_428211(a1: v10);`
  - `LOBYTE(v12) = 0;`
  - `v4 = sub_45BAE8(a1: v5, a2: v6, a3: v7, a4: v8);`
  - `CUIStatusBar::SetWhisperTarget(this: v4, a2: v5, a3: v6);`
  - `CUIStatusBar::StartChat(this: TSingleton<CUIStatusBar>::ms_pInstance, a2: 0);`

### CTabGuild（7 函数）


#### CTabGuild::1GUILDITEM  (246 B)

  - `void __thiscall CTabGuild::GUILDITEM::~GUILDITEM(CTabGuild::GUILDITEM *this)`

#### CTabGuild::Draw  (55826 B)

- SP_ 节点：`SP_2082__D_D_`, `SP_3348__GUILD_NOTICE`, `SP_350_YOU_ARE_NOT_IN_THE_GUILD`, `SP_5641_NONE`
  - `tagVARIANT::$::$2E851DBA257FA738680F86C475CAC6EB::$E3ADF3533BFFE4E09553D0C58E41D45E *__thiscall CTabGuild::Draw(`
  - `v292 = *(*(this + 10) + 2208);`
  - `sub_428902(a1: &v292);`
  - `v291 = *(*(this + 10) + 2204);`
  - `sub_428902(a1: &v291);`
  - `v4 = *(this + 10);`
  - `v5 = *(v4 + 2016) <= 0;`
  - `LOBYTE(v301) = 8;`
  - `if ( v5 )`
  - `sub_402F85(pvargDest: &v288, pvargSrc: v243.cyVal.Hi);`
  - `LOBYTE(v301) = 17;`
  - `LOBYTE(v301) = 18;`
  - `v21 = sub_428226(a1: String);`
  - `_bstr_t::_bstr_t(a1: v21);`
  - `sub_4284A8(a1: &v292);`
  - `v23 = sub_42782E(a1: v243.lVal, a2: v243.cyVal.Hi);`
  - …（截断 16 行）

#### CTabGuild::GetGuildItem  (997 B)

  - `int __thiscall CTabGuild::GetGuildItem(_DWORD *this, int a2, _DWORD *a3)`
  - `memset(a1: a3, a2: 0, a3: 0x40u);`
  - `for ( i = 0; ; i += 64 )`
  - `v6 = *(this + 53);`
  - `if ( v6 == 0 || v4 >= *(v6 - 4) )`
  - `if ( *(i + v6) == a2 )`
  - `sub_91192F(this: a3, a2: (*(this + 53) + (v4 << 6)));`
  - `for ( j = 0; ; j += 64 )`
  - `v10 = *(this + 84);`
  - `if ( v10 == 0 || v8 >= *(v10 - 4) )`
  - `if ( *(j + v10) == a2 )`
  - `sub_91192F(this: a3, a2: (*(this + 84) + (v8 << 6)));`

#### CTabGuild::GetToolTipString  (4459 B)

- SP_ 节点：`SP_3855_UNKNOWN`, `SP_684_ONLINE_GUILD_MEMBERS___TOTAL_REGISTERED_GUILD_MEMBERS`, `SP_685_OFFLINE_GUILD_MEMBERS___TOTAL_REGISTERED_GUILD_MEMBERS`, `SP_686_SORT_THRU_NAME`, `SP_687_SORT_THRU_JOB`, `SP_688_SORT_THRU_LEVEL`, `SP_689_SORT_THRU_GRADE`
  - `_DWORD *__thiscall CTabGuild::GetToolTipString(_DWORD *this, _DWORD *a2, signed int a3, signed int a4)`
  - `dword_BF040C(a1: v19, a2: 5, a3: 83, a4: 280, a5: 99);`
  - `if ( dword_BF0484(a1: v19, a2: a3, a3: a4) == 0 || (v5 = *(this + 22), v6 = this + 22, v5 == 0) || *v5 == 0 )`
  - `if ( a4 > 113 && a4 < 305 )`
  - `v8 = *(this + 10);`
  - `v9 = 33 * *(*(v8 + 1472) + 56) + a4 - 113;`
  - `if ( dword_BF0484(a1: v8 + 2028, a2: a3 - 6, a3: v9) != 0 )`
  - `if ( dword_BF0484(a1: *(this + 10) + 2044, a2: v10, a3: v9) != 0 )`
  - `if ( dword_BF0484(a1: this + 37, a2: v10, a3: v9) != 0 || dword_BF0484(a1: this + 68, a2: v10, a3: v9) != 0 )`
  - `if ( dword_BF0484(a1: this + 41, a2: v10, a3: v9) != 0 || dword_BF0484(a1: this + 72, a2: v10, a3: v9) != 0 )`
  - `if ( dword_BF0484(a1: this + 45, a2: v10, a3: v9) != 0 || dword_BF0484(a1: this + 76, a2: v10, a3: v9) != 0 )`
  - `if ( dword_BF0484(a1: this + 49, a2: v10, a3: v9) != 0 || dword_BF0484(a1: this + 80, a2: v10, a3: v9) != 0 )`
  - `v21 = sub_9117E9(a1: a3, a2: a4, a3: *(*(*(this + 10) + 1472) + 56));`
  - `CTabGuild::GetGuildItem(this, a2: v21, a3: v16);`
  - `if ( v21 == 0 || v16[0] == 0 )`
  - `ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF);`
  - …（截断 16 行）

#### CTabGuild::OnChat  (1166 B)

  - `void __thiscall CTabGuild::OnChat(CTabGuild *this)`
  - `v1 = *(this + 10);`
  - `v2 = *(v1 + 2172);`
  - `if ( v2 != 0 && v2 != *(*dword_BE7918 + 8352) )`
  - `v5 = *(v1 + 2172);`
  - `GuildItem = CTabGuild::GetGuildItem(this, a2: v5, a3: &v7);`
  - `if ( v7 != 0 && GuildItem == 0 && dword_BF0F54 == 0 && CUIMessenger::TryNew(a1: 0) != 0 )`
  - `ZXString<char>::operator=(a1: v8);`
  - `LOBYTE(v9) = 0;`
  - `v4 = sub_851F49();`
  - `CUIMessenger::SendInviteMsg(this: v4, a2: v6);`
  - `CTabGuild::GUILDITEM::~GUILDITEM(this: &v7);`

#### CTabGuild::OnKick  (1004 B)

  - `void __thiscall CTabGuild::OnKick(CTabGuild *this)`
  - `v1 = *(this + 10);`
  - `v2 = *(v1 + 2172);`
  - `if ( v2 != 0 && v2 != *(*dword_BE7918 + 8352) )`
  - `v4 = *(v1 + 2172);`
  - `CTabGuild::GetGuildItem(this, a2: v4, a3: v6);`
  - `ZXString<char>::operator=(a1: v7);`
  - `LOBYTE(v8) = 0;`
  - `field = get_field();`
  - `CField::SendKickGuildMsg(a1: field, a2: v5);`
  - `CTabGuild::GUILDITEM::~GUILDITEM(this: v6);`

#### CTabGuild::OnPartyInvite  (710 B)

  - `void __thiscall CTabGuild::OnPartyInvite(CTabGuild *this)`
  - `v1 = *(this + 10);`
  - `v2 = *(v1 + 2172);`
  - `if ( v2 != 0 && v2 != *(*dword_BE7918 + 8352) )`
  - `if ( CTabGuild::GetGuildItem(this, a2: *(v1 + 2172), a3: v3) != -1 && get_field() != 0 )`
  - `CField::SendJoinPartyMsg(a1: v4);`
  - `CTabGuild::GUILDITEM::~GUILDITEM(this: v3);`

### CTabGuildAlliance（7 函数）


#### CTabGuildAlliance::1GUILDITEM  (262 B)

  - `void __thiscall CTabGuildAlliance::GUILDITEM::~GUILDITEM(CTabGuildAlliance::GUILDITEM *this)`

#### CTabGuildAlliance::4GUILDITEM  (787 B)

  - `_DWORD *__thiscall CTabGuildAlliance::GUILDITEM::operator=(_DWORD *this, _DWORD *a2)`
  - `ZXString<char>::operator=(a1: a2 + 1);`
  - `ZXString<char>::operator=(a1: a2 + 2);`
  - `*(this + 3) = a2[3];`
  - `*(this + 4) = a2[4];`
  - `*(this + 5) = a2[5];`
  - `*(this + 6) = a2[6];`
  - `*(this + 7) = a2[7];`
  - `*(this + 8) = a2[8];`
  - `*(this + 9) = a2[9];`
  - `ZXString<char>::operator=(a1: a2 + 10);`
  - `*(this + 11) = a2[11];`
  - `*(this + 12) = a2[12];`
  - `*(this + 13) = a2[13];`
  - `*(this + 14) = a2[14];`
  - `*(this + 15) = a2[15];`
  - …（截断 16 行）

#### CTabGuildAlliance::GetGuildItem  (864 B)

  - `int __thiscall CTabGuildAlliance::GetGuildItem(int *this, int a2, void *a3)`
  - `memset(a1: a3, a2: 0, a3: 0x44u);`
  - `while ( 2 )`
  - `for ( i = 0; ; i += 68 )`
  - `if ( *v5 == 0 || v6 >= *(v8 - 4) )`
  - `if ( *(v8 + i) == a2 )`
  - `CTabGuildAlliance::GUILDITEM::operator=(a1: 68 * v6 + *(this + 32 * v3 + 52));`
  - `if ( v3 < 5 )`

#### CTabGuildAlliance::GetToolTipString  (4326 B)

- SP_ 节点：`SP_3855_UNKNOWN`, `SP_684_ONLINE_GUILD_MEMBERS___TOTAL_REGISTERED_GUILD_MEMBERS`, `SP_686_SORT_THRU_NAME`, `SP_687_SORT_THRU_JOB`, `SP_688_SORT_THRU_LEVEL`, `SP_689_SORT_THRU_GRADE`
  - `_DWORD *__thiscall CTabGuildAlliance::GetToolTipString(int *this, _DWORD *a2, int a3, int a4)`
  - `dword_BF040C(a1: v19, a2: 5, a3: 83, a4: 280, a5: 99);`
  - `if ( dword_BF0484(a1: v19, a2: a3, a3: a4) == 0 || (v5 = *(this + 21), v6 = this + 21, v5 == 0) || *v5 == 0 )`
  - `if ( a4 > 113 && a4 < 305 )`
  - `v8 = *(this + 9);`
  - `v9 = 33 * *(*(v8 + 1472) + 56) + a4 - 113;`
  - `if ( dword_BF0484(a1: v8 + 2076, a2: a3 - 6, a3: v9) != 0`
  - `|| dword_BF0484(a1: *(this + 9) + 2092, a2: v10, a3: v9) != 0 )`
  - `if ( dword_BF0484(a1: this + 36, a2: v10, a3: v9) != 0 || dword_BF0484(a1: this + 68, a2: v10, a3: v9) != 0 )`
  - `if ( dword_BF0484(a1: this + 40, a2: v10, a3: v9) != 0 || dword_BF0484(a1: this + 72, a2: v10, a3: v9) != 0 )`
  - `if ( dword_BF0484(a1: this + 44, a2: v10, a3: v9) != 0 || dword_BF0484(a1: this + 76, a2: v10, a3: v9) != 0 )`
  - `if ( dword_BF0484(a1: this + 48, a2: v10, a3: v9) != 0 || dword_BF0484(a1: this + 80, a2: v10, a3: v9) != 0 )`
  - `v21 = sub_915534(a1: a3, a2: a4, a3: *(*(*(this + 9) + 1472) + 56));`
  - `CTabGuildAlliance::GetGuildItem(this, a2: v21, a3: v16);`
  - `if ( v21 == 0 || v16[0] == 0 )`
  - `ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF);`
  - …（截断 16 行）

#### CTabGuildAlliance::LoadData  (7516 B)

  - `int __thiscall CTabGuildAlliance::LoadData(CTabGuildAlliance *this)`
  - `v2 = (this + 124);`
  - `for ( i = 5; i != 0; --i )`
  - `*(v2 - 1) = 1;`
  - `sub_9216DB(a1: v2 + 21);`
  - `v4 = *(*dword_BE7918 + 12398) == 0;`
  - `if ( v4 )`
  - `*(*(this + 9) + 2016) = 0;`
  - `*(*(this + 9) + 2020) = 0;`
  - `v5 = sub_912ABC(a1: &i);`
  - `ZXString<char>::operator=(a1: v5);`
  - `v6 = sub_912AE0(a1: &i);`
  - `ZXString<char>::operator=(a1: v6);`
  - `CTabGuildAlliance::MakeNoticeLayer(this);`
  - `AllianceGradeName = CWvsContext::GetAllianceGradeName(a1: &i, a2: v7 + 1);`
  - `ZXString<char>::operator=(a1: AllianceGradeName);`
  - …（截断 16 行）

#### CTabGuildAlliance::MakeNoticeLayer  (4320 B)

  - `CTabGuildAlliance *__thiscall CTabGuildAlliance::MakeNoticeLayer(CTabGuildAlliance *this)`
  - `v1 = (this + 88);`
  - `sub_428712(a1: 0);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `LOBYTE(v27) = 1;`
  - `if ( dword_BF14EC == 0 )`
  - `v2 = IWzGr2D::CreateLayer(`
  - `sub_428712(a1: *v2);`
  - `sub_4156C5(a1: &v25);`
  - `LOBYTE(v27) = 0;`
  - `if ( v3 < 0 )`
  - `if ( v4 < 0 )`
  - `if ( *v1 == 0 )`
  - `v6 = (*(**v1 + 224))(a1: *v1, a2: -1);`
  - `if ( v6 < 0 )`
  - …（截断 16 行）

#### CTabGuildAlliance::OnPartyInvite  (735 B)

  - `void __thiscall CTabGuildAlliance::OnPartyInvite(CTabGuildAlliance *this)`
  - `v1 = *(this + 9);`
  - `v2 = *(v1 + 2172);`
  - `if ( v2 != 0 && v2 != *(*dword_BE7918 + 8352) )`
  - `if ( CTabGuildAlliance::GetGuildItem(a1: *(v1 + 2172), a2: v3) != -1 && get_field() != 0 )`
  - `CField::SendJoinPartyMsg(a1: v4);`
  - `CTabGuildAlliance::GUILDITEM::~GUILDITEM(this: v3);`

### CTabBlackList（3 函数）


#### CTabBlackList::1BLACKLISTITEM  (148 B)

  - `void __thiscall CTabBlackList::BLACKLISTITEM::~BLACKLISTITEM(void *this)`

#### CTabBlackList::Draw  (15846 B)

- SP_ 节点：`SP_2081_D_D`, `SP_5641_NONE`
  - `VARIANTARG **__thiscall CTabBlackList::Draw(`
  - `v3 = *(*(this + 4) + 2208);`
  - `sub_428902(a1: &v82);`
  - `v83 = *(*(this + 4) + 2204);`
  - `sub_428902(a1: &v83);`
  - `LOBYTE(v89) = 3;`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `a2 = *(this + 1);`
  - `LOBYTE(v89) = 4;`
  - `if ( arg0 == 0 )`
  - `v5 = (*&lVal[8].vt)(`
  - `if ( v5 < 0 )`
  - `if ( v6 < 0 )`
  - `v71.lVal = *(v9 + 2016);`
  - `LOBYTE(v89) = 5;`
  - `ZXString<char>::Format(a1: &v81, a2: v10, a3: v71.cVal);`
  - …（截断 16 行）

#### CTabBlackList::OnAdd  (6566 B)

- SP_ 节点：`SP_725_PLEASE_ENTER_THE_NAME_OF_THE_CHARACTER_YOUD_LIKE_TO_ADD_TO_THE_BLACKLIST`, `SP_726_YOU_MAY_NOT_ENTER_YOURSELF_IN_THE_BLACKLIST`, `SP_727_THIS_CHARACTER_NAME_DOES_NOT_EXIST_R_NPLEASE_CHECK_AGAIN`, `SP_728_THIS_NAME_HAS_ALREADY_BEEN_REGISTERED_R_NPLEASE_CHECK_AGAIN`
  - `int __thiscall CTabBlackList::OnAdd(char **this)`
  - `v2 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v2 != 0 )`
  - `v3 = CUtilDlgEx::CUtilDlgEx(this: v2);`
  - `v37 = v4 != 0 ? (v4 - 8) : 0;`
  - `sub_4289CD(a1: v36);`
  - `LOBYTE(v41) = 1;`
  - `v7 = sub_428688(a1: v36);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v7, a2: 3, a3: 0, a4: v32, a5: v33, a6: v34[0]);`
  - `CUtilDlgEx::SetUtilDlgEx_INPUT_STR(this: v37, a2: &sStrDefault, a3: 4, a4: 12, a5: 0, a6: 0);`
  - `CUtilDlgEx::CreateUtilDlgEx(this: v37);`
  - `if ( CDialog::DoModal(a1: v37) != 1 )`
  - `LOBYTE(v41) = 3;`
  - `CUtilDlgEx::GetInputStr_Result(this: v37, a2: &v39);`
  - `LOBYTE(v41) = 4;`
  - `ZXString<char>::TrimRight(a1: 0);`
  - …（截断 16 行）

### CFriend（6 函数）


#### CFriend::FindIndex  (628 B)

  - `signed int __thiscall CWvsContext::CFriend::FindIndex(CWvsContext::CFriend *this, unsigned int a2)`
  - `if ( CWvsContext::CFriend::IsDataTwisted(a1: this) == 0 )`
  - `for ( i = 0; ; i += 39 )`
  - `CWvsContext::CFriend::IsDataTwisted(a1: this);`
  - `v6 = *this != 0 ? *(v5 - 4) : 0;`
  - `if ( v3 >= v6 )`
  - `if ( *(i + v5) == a2 )`

#### CFriend::Insert  (938 B)

  - `int __thiscall CWvsContext::CFriend::Insert(CWvsContext::CFriend *this, struct CInPacket *a2)`
  - `result = CWvsContext::CFriend::IsDataTwisted(a1: this);`
  - `if ( result == 0 )`
  - `v3 = ZArray<GW_Friend>::InsertBefore(a1: -1);`
  - `v4 = ZArray<int>::InsertBefore(a1: -1);`
  - `v6 = ZArray<int>::InsertBefore(a1: -1);`
  - `v5 = ZArray<int>::InsertBefore(a1: -1);`
  - `GW_Friend::Decode(a1: v3, a2);`
  - `*v4 = CInPacket::Decode1(this: a2);`
  - `*v6 = CConfig::IsBlockedFriend(this: TSingleton<CConfig>::ms_pInstance, a2: *v3, a3: 0);`
  - `result = CConfig::IsBlockedFriend(this: TSingleton<CConfig>::ms_pInstance, a2: *v3, a3: 1);`

#### CFriend::IsDataTwisted  (1091 B)

  - `int __thiscall CWvsContext::CFriend::IsDataTwisted(CWvsContext::CFriend *this)`
  - `if ( *this != 0 )`
  - `v1 = *(*this - 4);`
  - `v2 = *(this + 1);`
  - `if ( v2 != 0 )`
  - `v2 = *(v2 - 4);`
  - `if ( v1 == v2 )`
  - `v3 = *(this + 1);`
  - `if ( v3 != 0 )`
  - `v4 = *(v3 - 4);`
  - `v5 = *(this + 1);`
  - `if ( v5 != 0 )`
  - `v5 = *(v5 - 4);`
  - `if ( v4 == v5 )`
  - `v6 = *(this + 1);`
  - `if ( v6 != 0 )`
  - …（截断 16 行）

#### CFriend::Reset  (1073 B)

  - `_DWORD *__thiscall CWvsContext::CFriend::Reset(void **this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `ZArray<GW_Friend>::_Realloc(a1: v3, a2: 1, a3: &v6);`
  - `ZArray<int>::_Realloc(a1: v3, a2: 1, a3: &v6);`
  - `result = ZArray<int>::_Realloc(a1: v3, a2: 1, a3: &v6);`
  - `if ( v3 > 0 )`
  - `CInPacket::DecodeBuffer(a1: *this, a2: 39 * v3);`
  - `CInPacket::DecodeBuffer(a1: *(this + 1), a2: 4 * v3);`
  - `for ( i = 0; ; ++i )`
  - `CWvsContext::CFriend::IsDataTwisted(a1: this);`
  - `if ( *this != 0 )`
  - `result = *(result - 1);`
  - `if ( i >= result )`
  - `CWvsContext::CFriend::UpdateBlockedFriend(this, a2: i);`

#### CFriend::UpdateBlockedFriend  (548 B)

  - `int __thiscall CWvsContext::CFriend::UpdateBlockedFriend(CWvsContext::CFriend *this, int a2)`
  - `v3 = (4 * a2 + *(this + 2));`
  - `*v3 = CConfig::IsBlockedFriend(this: TSingleton<CConfig>::ms_pInstance, a2: *(39 * a2 + *this), a3: 0);`
  - `v4 = (4 * a2 + *(this + 3));`
  - `result = CConfig::IsBlockedFriend(this: TSingleton<CConfig>::ms_pInstance, a2: *(39 * a2 + *this), a3: 1);`

#### CFriend::UpdateFriend  (627 B)

  - `void __thiscall CWvsContext::CFriend::UpdateFriend(CWvsContext::CFriend *this, struct CInPacket *a2, int a3)`
  - `v4 = CInPacket::Decode4(this: a2);`
  - `Index = CWvsContext::CFriend::FindIndex(this, a2: v4);`
  - `if ( Index >= 0 )`
  - `v6 = (*(this + 1) + 4 * Index);`
  - `GW_Friend::Decode(a1: (*this + 39 * Index), a2);`
  - `*v6 = CInPacket::Decode1(this: a2);`
  - `if ( a3 != 0 )`
  - `CWvsContext::CFriend::UpdateBlockedFriend(this, a2: Index);`

### GW_Friend（1 函数）


#### GW_Friend::Decode  (124 B)

  - `int __thiscall GW_Friend::Decode(void *this, int a2)`
  - `return CInPacket::DecodeBuffer(a1: this, a2: 0x27u);`

### CBlackListDlg（3 函数）


#### CBlackListDlg::AddBlackList  (6123 B)

- SP_ 节点：`SP_725_PLEASE_ENTER_THE_NAME_OF_THE_CHARACTER_YOUD_LIKE_TO_ADD_TO_THE_BLACKLIST`, `SP_726_YOU_MAY_NOT_ENTER_YOURSELF_IN_THE_BLACKLIST`, `SP_727_THIS_CHARACTER_NAME_DOES_NOT_EXIST_R_NPLEASE_CHECK_AGAIN`, `SP_728_THIS_NAME_HAS_ALREADY_BEEN_REGISTERED_R_NPLEASE_CHECK_AGAIN`
  - `unsigned int __thiscall CEntrustedShopDlg::CBlackListDlg::AddBlackList(CEntrustedShopDlg::CBlackListDlg *this)`
  - `result = *(this + 121);`
  - `if ( result < 0x14 )`
  - `v3 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v3 != 0 )`
  - `v4 = CUtilDlgEx::CUtilDlgEx(this: v3);`
  - `v33 = v5 != 0 ? (v5 - 8) : 0;`
  - `sub_4289CD(a1: v32);`
  - `LOBYTE(v38) = 1;`
  - `v8 = sub_428688(a1: v32);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v8, a2: 3, a3: 0, a4: v29, a5: v30, a6: v31);`
  - `CUtilDlgEx::SetUtilDlgEx_INPUT_STR(this: v33, a2: &sStrDefault, a3: 4, a4: 12, a5: 0, a6: 0);`
  - `CUtilDlgEx::CreateUtilDlgEx(this: v33);`
  - `if ( CDialog::DoModal(a1: v33) != 1 )`
  - `CUtilDlgEx::GetInputStr_Result(this: v33, a2: &v35);`
  - `LOBYTE(v38) = 3;`
  - …（截断 16 行）

#### CBlackListDlg::DeleteBlackList  (1109 B)

  - `void __thiscall CEntrustedShopDlg::CBlackListDlg::DeleteBlackList(CEntrustedShopDlg::CBlackListDlg *this)`
  - `v2 = *(this + 111);`
  - `if ( v2 >= 0 && v2 < *(this + 121) )`
  - `v3 = sub_51AF3B(a1: *(this + 111));`
  - `ZXString<char>::operator=(a1: v3);`
  - `sub_519695(a1: v8[0]);`
  - `v4 = sub_51AF8E(a1: v3, a2: 0);`
  - `if ( v4 != 0 )`
  - `sub_51AE29(a1: v4);`
  - `v5 = *(this + 121);`
  - `(*(*(*(this + 114) + 4) + 28))(a1: *(this + 114) + 4, a2: v5 < 20);`
  - `if ( v5 > 0 )`
  - `v6 = *(this + 111);`
  - `if ( v6 < v5 && v6 >= 0 )`
  - `(*(*(*(this + 116) + 4) + 28))(a1: *(this + 116) + 4, a2: v7);`
  - `CWnd::InvalidateRect(this, a2: 0);`
  - …（截断 16 行）

#### CBlackListDlg::OnButtonClicked  (652 B)

  - `void __thiscall CEntrustedShopDlg::CBlackListDlg::OnButtonClicked(`
  - `switch ( a2 )`
  - `CEntrustedShopDlg::CBlackListDlg::AddBlackList(this);`
  - `CEntrustedShopDlg::CBlackListDlg::DeleteBlackList(this);`
  - `(*(*this + 52))(a1: this, a2: 1);`
  - `(*(*this + 52))(a1: this, a2: 2);`

### CUIGuildBBS（17 函数）


#### CUIGuildBBS::DestroyPreviousControls  (2125 B)

  - `int __thiscall CUIGuildBBS::DestroyPreviousControls(_DWORD *this)`
  - `v2 = *(this + 404);`
  - `if ( v2 != 0 )`
  - `result = (*(*v2 + 12))(a1: v2);`
  - `v4 = *(this + 406);`
  - `if ( v4 != 0 )`
  - `result = (*(*v4 + 12))(a1: v4);`
  - `v5 = *(this + 408);`
  - `if ( v5 != 0 )`
  - `result = (*(*v5 + 12))(a1: v5);`
  - `v6 = *(this + 376);`
  - `if ( v6 != 0 )`
  - `result = (*(*v6 + 12))(a1: v6);`
  - `v7 = *(this + 378);`
  - `if ( v7 != 0 )`
  - `result = (*(*v7 + 12))(a1: v7);`
  - …（截断 16 行）

#### CUIGuildBBS::FormatDate  (305 B)

  - `int __stdcall CUIGuildBBS::FormatDate(FILETIME *lpFileTime, int a2)`
  - `FileTimeToSystemTime(lpFileTime, lpSystemTime: &SystemTime);`
  - `return ZXString<char>::Format(a1: a2, a2: aD02d02d02d02d, a3: SystemTime.wYear);`

#### CUIGuildBBS::FormatShortDate  (587 B)

  - `int __stdcall CUIGuildBBS::FormatShortDate(FILETIME *lpFileTime, int a2)`
  - `dword_BF03B0(a1: &v3);`
  - `FileTimeToSystemTime(lpFileTime, lpSystemTime: &SystemTime);`
  - `if ( v3 == *&SystemTime.wYear && v4 == SystemTime.wDay )`
  - `return ZXString<char>::Format(a1: a2, a2: aD02d, a3: SystemTime.wHour);`
  - `return ZXString<char>::Format(a1: a2, a2: a02d02d, a3: SystemTime.wMonth);`

#### CUIGuildBBS::LoadViewTextBox  (16005 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_3729_UI_GUILDBBSIMG_GUILDBBS_BTNOTICE`, `SP_3730_UI_GUILDBBSIMG_GUILDBBS_BTWRITE`, `SP_3731_UI_GUILDBBSIMG_GUILDBBS_BTRETOUCH`, `SP_3732_UI_GUILDBBSIMG_GUILDBBS_BTDELETE`, `SP_3733_UI_GUILDBBSIMG_GUILDBBS_BTREPLY`, `SP_3734_UI_GUILDBBSIMG_GUILDBBS_BTREPLYDELETE`, `SP_3735_UI_GUILDBBSIMG_GUILDBBS_BACKGRND2`, `SP_5527_ARIAL`
  - `void __thiscall CUIGuildBBS::LoadViewTextBox(CWnd *this)`
  - `*(this + 372) = 0;`
  - `CUIGuildBBS::DestroyPreviousControls(this);`
  - `memset(v71, 0, sizeof(v71));`
  - `LOBYTE(v75) = 1;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `LOBYTE(v75) = 0;`
  - `v4 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v75) = 2;`
  - `if ( v4 != 0 )`
  - `v5 = CCtrlButton::CCtrlButton(this: v4);`
  - `sub_4284FF(a1: v5);`
  - `(*(**(this + 378) + 32))(a1: *(this + 378), a2: this, a3: 2001, a4: 404, a5: 275, a6: 1, a7: v71);`
  - `(*(*(*(this + 378) + 4) + 36))(a1: *(this + 378) + 4, a2: 1);`
  - `if ( !sub_813A70() )`
  - `(*(*(*(this + 378) + 4) + 28))(a1: *(this + 378) + 4, a2: 0);`
  - …（截断 16 行）

#### CUIGuildBBS::LoadWriteTextBox  (11628 B)

- SP_ 节点：`SP_3736_UI_GUILDBBSIMG_GUILDBBS_BTREGISTER`, `SP_3737_UI_GUILDBBSIMG_GUILDBBS_BTCANCEL`, `SP_3738_UI_GUILDBBSIMG_GUILDBBS_MOVEEMOTICON_BTLEFT`, `SP_3739_UI_GUILDBBSIMG_GUILDBBS_MOVEEMOTICON_BTRIGHT`, `SP_3740_UI_GUILDBBSIMG_GUILDBBS_BACKGRND3`, `SP_5527_ARIAL`
  - `void __thiscall CUIGuildBBS::LoadWriteTextBox(CWnd *this, signed int *Unknown, int *BSTR)`
  - `*(this + 372) = 1;`
  - `CUIGuildBBS::DestroyPreviousControls(this);`
  - `memset(v59, 0, sizeof(v59));`
  - `LOBYTE(v62) = 1;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `LOBYTE(v62) = 0;`
  - `if ( BSTR != 0 )`
  - `v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - `LOBYTE(v62) = 2;`
  - `if ( v6 != 0 )`
  - `v7 = CCtrlButton::CCtrlButton(this: v6);`
  - `sub_4284FF(a1: v7);`
  - `v8 = **(this + 384);`
  - `if ( Unknown != 0 )`
  - `v9 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: v51);`
  - …（截断 16 行）

#### CUIGuildBBS::OnButtonClicked  (2130 B)

- 分支 case：`2000`, `2001`, `2002`, `2003`, `2004`, `2005`, `2006`, `2007`, `2008`, `2009`, `2010`, `2011`, `2012`, `2013`, `2014`, `2015`
  - `int __thiscall CUIGuildBBS::OnButtonClicked(int *this, int a2)`
  - `switch ( a2 )`
  - `CUIGuildBBS::OnWrite(this, a2: 0, a3: v3);`
  - `return CUIWnd::OnButtonClicked(a1: a2);`
  - `CUIGuildBBS::OnDelete(this);`
  - `CUIGuildBBS::OnWrite(this, a2: 1, a3: 0);`
  - `sub_8166F6(this, a2: 1, a3: 0);`
  - `sub_8166F6(this, a2: 0, a3: v4);`
  - `CUIGuildBBS::OnCancel(this);`
  - `sub_816994(this);`
  - `sub_816AA8(this, a2: 0);`
  - `sub_816AA8(this, a2: 1);`
  - `sub_816AA8(this, a2: 2);`
  - `sub_816AA8(this, a2: 3);`
  - `CWvsContext::UI_Close(this: *dword_BE7918, a2: *(this + 354));`
  - `sub_81649B(this, a2: 2014);`
  - …（截断 16 行）

#### CUIGuildBBS::OnCancel  (166 B)

  - `int __thiscall CUIGuildBBS::OnCancel(CWnd *this)`
  - `CUIGuildBBS::LoadViewTextBox(this);`
  - `return CUIGuildBBS::SendViewEntryRequest(this);`

#### CUIGuildBBS::OnCreate  (13764 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_104_MAPLESTORY_ERROR__SPRITE_INIT_FAILEDUNKNOWN`, `SP_3724_UI_GUILDBBSIMG_GUILDBBS_BTQUIT`, `SP_3725_UI_GUILDBBSIMG_GUILDBBS_EMOTICON_BASIC_0`, `SP_3726_UI_GUILDBBSIMG_GUILDBBS_EMOTICON_BASIC_1`, `SP_3727_UI_GUILDBBSIMG_GUILDBBS_EMOTICON_BASIC_2`, `SP_3728_UI_GUILDBBSIMG_GUILDBBS_EMOTICON_SELECT`, `SP_3744_UI_GUILDBBSIMG_GUILDBBS_EMOTICON_CASH_D`
  - `void __thiscall CUIGuildBBS::OnCreate(CUIGuildBBS *this, int Unknown)`
  - `sub_41C06C(a1: &word_AF0E08, a2: -1);`
  - `CUIWnd::OnCreate(a1: Unknown, a2: v64);`
  - `CUIGuildBBS::LoadViewTextBox(this);`
  - `v3 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 116);`
  - `if ( v3 != 0 )`
  - `v4 = CCtrlScrollBar::CCtrlScrollBar(this: v3);`
  - `sub_428597(a1: v4);`
  - `(*(**(this + 410) + 32))(a1: *(this + 410), a2: this, a3: 2019, a4: 1, a5: 3, a6: 710, a7: 326, a8: 125, a9: 0);`
  - `v5 = *(this + 410);`
  - `*(v5 + 52) = 300;`
  - `v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: v64);`
  - `if ( v6 != 0 )`
  - `v7 = CCtrlSelector::CCtrlSelector(this: v6);`
  - `sub_4BCE07(a1: v7);`
  - `(*(**(this + 414) + 32))(a1: *(this + 414), a2: this, a3: 2021, a4: 40, a5: 450, a6: 320, a7: 0);`
  - …（截断 16 行）

#### CUIGuildBBS::OnDelete  (1519 B)

- SP_ 节点：`SP_3722_ARE_YOU_SURE_YOU_WANT_TO_DELETE_THE_POST`
  - `int __thiscall CUIGuildBBS::OnDelete(CWnd *this)`
  - `result = CUtilDlg::YesNo(a1: v4, a2: v5, a3: v6, a4: v7, a5: v8);`
  - `if ( result == 6 )`
  - `COutPacket::COutPacket(a1: 155);`
  - `COutPacket::Encode1(this: &v9, a2: 1);`
  - `COutPacket::Encode4(this: &v9, a2: *(*(this + 367) + 12));`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v9);`
  - `if ( *(this + 367) != 0 )`
  - `sub_8175EB(a1: 0);`
  - `*(this + 367) = 0;`
  - `CUIGuildBBS::SendLoadListRequest(this);`
  - `CUIGuildBBS::LoadViewTextBox(this);`
  - `return ZArray<unsigned char>::RemoveAll(a1: v10);`

#### CUIGuildBBS::OnEntryNotFound  (958 B)

- SP_ 节点：`SP_3743_THIS_MESSAGE_HAD_BEEN_DELETED_BY_ANOTHER_USER_R_NPLEASE_CHECK_AGAIN`
  - `void __thiscall CUIGuildBBS::OnEntryNotFound(CUIGuildBBS *this)`
  - `CUtilDlg::Notice(sMsg: v3, sSndName: v4, ppDialog: v5, bAutoSeparated: v6, bIsTightLine: v7);`
  - `if ( *(this + 367) != 0 )`
  - `sub_8175EB(a1: 0);`
  - `*(this + 367) = 0;`
  - `CUIGuildBBS::LoadViewTextBox(this);`

#### CUIGuildBBS::OnGuildBBSPacket  (534 B)

  - `void __thiscall CUIGuildBBS::OnGuildBBSPacket(CUIGuildBBS *this, volatile LONG *a2)`
  - `v3 = CInPacket::Decode1(this: a2) - 6;`
  - `if ( v3 != 0 )`
  - `if ( v4 != 0 )`
  - `if ( v4 == 1 )`
  - `CUIGuildBBS::OnEntryNotFound(this);`
  - `CUIGuildBBS::OnViewEntryResult(this, a2);`
  - `CUIGuildBBS::OnLoadListResult(this, a2);`

#### CUIGuildBBS::OnLoadListResult  (4773 B)

  - `void __thiscall CUIGuildBBS::OnLoadListResult(CUIGuildBBS *this, struct CInPacket *i)`
  - `sub_817741(a1: this + 1476);`
  - `sub_817574(a1: this + 1480);`
  - `if ( CInPacket::Decode1(this: i) != 0 )`
  - `v4 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 40);`
  - `if ( v4 != 0 )`
  - `v5 = sub_816F43(this: v4);`
  - `sub_81747F(a1: v5);`
  - `v6 = *(this + 371);`
  - `*(v6 + 12) = CInPacket::Decode4(this: v3);`
  - `v7 = *(this + 371);`
  - `*(v7 + 20) = CInPacket::Decode4(this: v3);`
  - `v8 = CInPacket::DecodeStr(a1: &v28);`
  - `ZXString<char>::operator=(a1: v8);`
  - `if ( v28 != 0 )`
  - `CInPacket::DecodeBuffer(a1: (*(this + 371) + 24), a2: 8u);`
  - …（截断 16 行）

#### CUIGuildBBS::OnMouseButton  (2595 B)

  - `void __thiscall CUIGuildBBS::OnMouseButton(_DWORD *this, int a2, int a3, int a4, int a5)`
  - `if ( a2 == 513 )`
  - `if ( *(this + 370) != 0 )`
  - `if ( dword_BF0484(a1: &v15, a2: a4, a3: a5) != 0 )`
  - `*(this + 364) = 0;`
  - `CUIGuildBBS::SendViewEntryRequest(this: this - 1);`
  - `v6 = *(this + 368);`
  - `if ( v6 != 0 )`
  - `v6 = *(v6 - 4);`
  - `if ( dword_BF0484(a1: v14, a2: a4, a3: a5) != 0 )`
  - `v7 = (a5 - 134) / 31;`
  - `if ( v7 >= 0 )`
  - `v8 = *(this + 368);`
  - `if ( v8 != 0 && v7 < *(v8 - 4) )`
  - `*(this + 364) = *(*(v8 + 8 * v7 + 4) + 12);`
  - `for ( i = 446; i < 515; i += 23 )`
  - …（截断 16 行）

#### CUIGuildBBS::OnViewEntryResult  (3090 B)

  - `void __thiscall CUIGuildBBS::OnViewEntryResult(CUIGuildBBS *this, volatile LONG *i)`
  - `if ( *(this + 367) != 0 )`
  - `sub_817513(a1: this + 1464);`
  - `v3 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 44);`
  - `if ( v3 != 0 )`
  - `v4 = sub_8171CF(this: v3);`
  - `sub_81740C(a1: v4);`
  - `v6 = *(this + 367);`
  - `*(v6 + 12) = CInPacket::Decode4(this: i);`
  - `*(this + 365) = *(*(this + 367) + 12);`
  - `v7 = *(this + 367);`
  - `*(v7 + 16) = CInPacket::Decode4(this: v5);`
  - `CInPacket::DecodeBuffer(a1: (*(this + 367) + 20), a2: 8u);`
  - `v8 = CInPacket::DecodeStr(a1: &i);`
  - `ZXString<char>::operator=(a1: v8);`
  - `if ( i != 0 )`
  - …（截断 16 行）

#### CUIGuildBBS::OnWrite  (2317 B)

- SP_ 节点：`SP_3723_PLEASE_DELETE_THE_CURRENT_NOTICE_R_NAND_THEN_TRY_AGAIN`
  - `void __thiscall CUIGuildBBS::OnWrite(CWnd *this, signed int *a2, int *a3)`
  - `if ( *(this + 371) != 0 && a3 != 0 )`
  - `CUtilDlg::Notice(sMsg: v9, sSndName: v10, ppDialog: v11, bAutoSeparated: v12, bIsTightLine: v13);`
  - `v5 = (this + 1708);`
  - `ZArray<long>::RemoveAll(a1: this + 1708);`
  - `if ( CWvsContext::IsExist(a1: v15) != 0 )`
  - `*ZArray<long>::InsertBefore(a1: -1) = v15 - 1322475;`
  - `v15 = (v15 + 1);`
  - `while ( (v15 - 1322500) < 7 );`
  - `if ( a2 != 0 )`
  - `v6 = *(*(this + 367) + 36);`
  - `if ( v6 >= 3 )`
  - `*(this + 373) = 0;`
  - `*(this + 374) = 0;`
  - `for ( i = 0; ; ++i )`
  - `if ( *v5 == 0 || i >= *(v8 - 4) )`
  - …（截断 16 行）

#### CUIGuildBBS::SendLoadListRequest  (527 B)

  - `int __thiscall CUIGuildBBS::SendLoadListRequest(int *this)`
  - `COutPacket::COutPacket(a1: 155);`
  - `COutPacket::Encode1(this: &v3, a2: 2);`
  - `COutPacket::Encode4(this: &v3, a2: *(this + 368));`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v3);`
  - `return ZArray<unsigned char>::RemoveAll(a1: v4);`

#### CUIGuildBBS::SendViewEntryRequest  (583 B)

  - `int __thiscall CUIGuildBBS::SendViewEntryRequest(int *this)`
  - `COutPacket::COutPacket(a1: 155);`
  - `COutPacket::Encode1(this: &v3, a2: 3);`
  - `COutPacket::Encode4(this: &v3, a2: *(this + 365));`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v3);`
  - `CUIGuildBBS::SendLoadListRequest(this);`
  - `return ZArray<unsigned char>::RemoveAll(a1: v4);`

### CWndGuildGrade（3 函数）


#### CWndGuildGrade::OnButtonClicked  (291 B)

  - `void __thiscall CWndGuildGrade::OnButtonClicked(CWndGuildGrade *this, unsigned int a2)`
  - `if ( a2 == 2140 )`
  - `CWndGuildGrade::OnChangeGradeName(this);`
  - `else if ( a2 == 2141 )`
  - `CWndGuildGrade::OnSaveGradeName(this);`

#### CWndGuildGrade::OnChangeGradeName  (7046 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_3265_MEMBER`, `SP_3299_PLEASE_PUT_A_POSITION_R_N_WHICH_YOU_WANT_TO_CHANGE_TO`, `SP_3300_YOU_CANNOT_DELETE_THIS_POSITION_BECAUSE_R_NTHERE_IS_A_LOWER_RANK_THAN_THIS_POSIT`, `SP_3301_YOU_CANNOT_DELETE_THIS_POSITION_R_NBECAUSE_THERE_ARE_STILL_MEMBERS_R_NWHO_ARE_IN`, `SP_824_IT_CANNOT_BE_USED_PLEASE_CHECK_IT_AGAIN`
  - `void *__thiscall CWndGuildGrade::OnChangeGradeName(CWndGuildGrade *this)`
  - `v2 = *(*dword_BE7918 + 12360) == 0;`
  - `if ( !v2 )`
  - `result = *(*dword_BE7918 + 8352);`
  - `if ( **(*dword_BE7918 + 12372) == result )`
  - `v3 = *(this + 31);`
  - `if ( v3 != 0 )`
  - `v4 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v4 != 0 )`
  - `v5 = CUtilDlgEx::CUtilDlgEx(this: v4);`
  - `v31 = v6 != 0 ? (v6 - 8) : 0;`
  - `sub_4289CD(a1: v30);`
  - `LOBYTE(v36) = 1;`
  - `v9 = sub_428688(a1: v30);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v9, a2: 3, a3: 0, a4: v27, a5: v28, a6: v29);`
  - `if ( v3 == 4 || v3 == 5 )`
  - …（截断 16 行）

#### CWndGuildGrade::OnSaveGradeName  (3813 B)

- SP_ 节点：`SP_3296_IT_IS_SAVED`, `SP_3302_WOULD_LIKE_TO_SAVE_THE_CHANGE`
  - `int __thiscall CWndGuildGrade::OnSaveGradeName(CWndGuildGrade *this)`
  - `result = *(*dword_BE7918 + 12360);`
  - `if ( result != 0 )`
  - `result = *(*dword_BE7918 + 8352);`
  - `v3 = *(*dword_BE7918 + 12372);`
  - `if ( *v3 == result )`
  - `result = CUtilDlg::YesNo(a1: v16, a2: v17, a3: v18, a4: v19, a5: v20[0]);`
  - `if ( result == 6 )`
  - `v5 = (this + 152);`
  - `while ( 1 )`
  - `sub_A39340(a1: &v23, a2: v6 + 1);`
  - `if ( *v5 == 0 )`
  - `if ( v23 != 0 )`
  - `if ( _strcmpi(a1: v9, a2: v8) != 0 )`
  - `if ( v7 >= 5 )`
  - `return CUtilDlg::Notice(sMsg: v16, sSndName: v17, ppDialog: v18, bAutoSeparated: v19, bIsTightLine: v20[0]);`
  - …（截断 16 行）

### CWndAllianceGrade（3 函数）


#### CWndAllianceGrade::OnButtonClicked  (303 B)

  - `void __thiscall CWndAllianceGrade::OnButtonClicked(CWndAllianceGrade *this, unsigned int a2)`
  - `if ( a2 == 2140 )`
  - `CWndAllianceGrade::OnChangeGradeName(this);`
  - `else if ( a2 == 2141 )`
  - `CWndAllianceGrade::OnSaveGradeName(this);`

#### CWndAllianceGrade::OnChangeGradeName  (6638 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_3265_MEMBER`, `SP_3299_PLEASE_PUT_A_POSITION_R_N_WHICH_YOU_WANT_TO_CHANGE_TO`, `SP_3300_YOU_CANNOT_DELETE_THIS_POSITION_BECAUSE_R_NTHERE_IS_A_LOWER_RANK_THAN_THIS_POSIT`, `SP_4459_UNABLE_TO_DELETE_DUE_TO_A_MEMBER_R_NIN_THE_UNION_CURRENTLY_BEING_ON_THE_POSITION`, `SP_824_IT_CANNOT_BE_USED_PLEASE_CHECK_IT_AGAIN`
  - `int __thiscall CWndAllianceGrade::OnChangeGradeName(CWndAllianceGrade *this)`
  - `result = sub_A3AC6F(a1: *dword_BE7918);`
  - `if ( result != 0 )`
  - `v3 = *(this + 31);`
  - `if ( v3 != 0 )`
  - `v4 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v4 != 0 )`
  - `v5 = CUtilDlgEx::CUtilDlgEx(this: v4);`
  - `v31 = v6 != 0 ? (v6 - 8) : 0;`
  - `sub_4289CD(a1: v30);`
  - `LOBYTE(v36) = 1;`
  - `v9 = sub_428688(a1: v30);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v9, a2: 3, a3: 0, a4: v27, a5: v28, a6: v29);`
  - `if ( v3 == 4 || v3 == 5 )`
  - `CUtilDlgEx::SetUtilDlgEx_INPUT_STR(this: v31, a2: &sStrDefault, a3: v10, a4: 10, a5: 0, a6: 0);`
  - `CUtilDlgEx::CreateUtilDlgEx(this: v31);`
  - …（截断 16 行）

#### CWndAllianceGrade::OnSaveGradeName  (3419 B)

- SP_ 节点：`SP_3296_IT_IS_SAVED`, `SP_3302_WOULD_LIKE_TO_SAVE_THE_CHANGE`
  - `int __thiscall CWndAllianceGrade::OnSaveGradeName(CWndAllianceGrade *this)`
  - `result = *(*dword_BE7918 + 12398);`
  - `if ( result != 0 )`
  - `result = sub_A3AC6F(a1: *dword_BE7918);`
  - `result = CUtilDlg::YesNo(a1: v14, a2: v15, a3: v16, a4: v17, a5: v18);`
  - `if ( result == 6 )`
  - `v21 = (this + 152);`
  - `for ( i = (this + 152); ; ++i )`
  - `CWvsContext::GetAllianceGradeName(a1: &v22, a2: v5 + 1);`
  - `if ( *i == 0 )`
  - `if ( v22 != 0 )`
  - `if ( _strcmpi(a1: v9, a2: v8) != 0 )`
  - `if ( v7 >= 5 )`
  - `return CUtilDlg::Notice(sMsg: v14, sSndName: v15, ppDialog: v16, bAutoSeparated: v17, bIsTightLine: v18);`
  - `COutPacket::COutPacket(a1: 143);`
  - `COutPacket::Encode1(this: &v19, a2: 8);`
  - …（截断 16 行）

### UFamilyItem（1 函数）


#### UFamilyItem::1?$ZRef  (400 B)

  - `LONG __thiscall ZRef<CUIFamilyChart::FamilyItem>::~ZRef<CUIFamilyChart::FamilyItem>(_DWORD *this, int a2)`
  - `result = InterlockedDecrement(lpAddend: (*(this + 1) + 4));`
  - `if ( result == 0 )`
  - `InterlockedIncrement(lpAddend: (*(this + 1) + 4));`
  - `return sub_402D2E(a1: *(this + 1));`

### PARTYDATA（1 函数）


#### PARTYDATA::Decode  (125 B)

  - `int __thiscall PARTYDATA::Decode(void *this, int a2)`
  - `return CInPacket::DecodeBuffer(a1: this, a2: 0x12Au);`

### PARTYSERACH_SETTING（1 函数）


#### PARTYSERACH_SETTING::Clear  (251 B)

  - `int __thiscall PARTYSERACH_SETTING::Clear(PARTYSERACH_SETTING *this)`
  - `*(this + 1) = 0;`
  - `*(this + 2) = 0;`
  - `*(this + 3) = 0;`
  - `*(this + 4) = 0;`

### TabPartySearch（7 函数）


#### TabPartySearch::HoldSearch  (394 B)

  - `void __thiscall TabPartySearch::HoldSearch(CWnd *this)`
  - `v2 = *(*dword_BE7918 + 12328);`
  - `if ( v2 != 0 && v2 != 2 )`
  - `*(*dword_BE7918 + 12328) = 2;`
  - `sub_A11DB7(this: *dword_BE7918);`
  - `CWvsContext::SendCancelPartyWanted(this: *dword_BE7918);`
  - `CWnd::InvalidateRect(this, a2: 0);`

#### TabPartySearch::IsValidSetting  (4153 B)

- SP_ 节点：`SP_730_YOU_MAY_ENTER_WITH_2_6_PARTY_MEMBERS`, `SP_731_YOUR_PARTY_ALREADY_HAS_D_OR_MORE_MEMBERS`, `SP_732_YOU_CAN_ONLY_SEARCH_FOR_PARTY_R_NMEMBERS_WITHIN_A_RANGE_OF_30_LEVELS`, `SP_733_CURRENTLY_THE_HIGHEST_LEVEL_IN_MAPLESTORY_R_NIS_AT_200`, `SP_734_THE_RANGE_OF_LEVEL_FOR_SEARCH_HAS_TO_INCLUDE_YOUR_OWN_LEVEL`, `SP_735_THE_MIN_VALUE_IS_HIGHER_THAN_THE_MAX`, `SP_736_PLEASE_ENTER_THE_JOB_CLASS_YOUD_LIKE_TO_PARTY_WITH`
  - `signed int __thiscall TabPartySearch::IsValidSetting(`
  - `if ( a4 > 200 )`
  - `if ( a4 - a3 > 30 )`
  - `if ( a3 > a4 )`
  - `LOBYTE(v26) = 1;`
  - `ZXString<char>::Format(a1: &v25, a2: v9, a3);`
  - `LOBYTE(v26) = 0;`
  - `sub_428211(a1: &v25);`
  - `CUtilDlg::Notice(sMsg: v20, sSndName: v21, ppDialog: v22, bAutoSeparated: v23, bIsTightLine: v24);`
  - `if ( a7 < a3 || a7 > a4 )`
  - `if ( a5 < 2 || a5 > 6 )`
  - `if ( a5 <= sub_A3F955(a1: *dword_BE7918) )`
  - `LOBYTE(v26) = 2;`
  - `ZXString<char>::Format(a1: &v25, a2: v13, a3: a5);`
  - `if ( a6 == 0 )`
  - `for ( i = 0; i < 25; ++i )`
  - …（截断 16 行）

#### TabPartySearch::OnButtonClicked  (490 B)

- 分支 case：`6000`, `6001`, `6002`
  - `int __thiscall TabPartySearch::OnButtonClicked(TabPartySearch *this, int a2)`
  - `switch ( a2 )`
  - `TabPartySearch::StartSearch(this);`
  - `TabPartySearch::HoldSearch();`
  - `TabPartySearch::StopSearch(this);`
  - `return CUIWnd::OnButtonClicked(a1: a2);`

#### TabPartySearch::OnCreate  (30120 B)

  - `_DWORD *__thiscall TabPartySearch::OnCreate(CWnd *this, int a2)`
  - `sub_41C06C(a1: &word_AF0E08, a2: -1);`
  - `CUIWnd::OnCreate(a1: a2, a2: v151);`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v157, pvargSrc: &pvargSrc);`
  - `LOBYTE(v167) = 1;`
  - `_bstr_t::_bstr_t(a1: str_Ui_12);`
  - `v4 = sub_406472();`
  - `v5 = sub_402680(a1: v4);`
  - `IWzResMan::GetObjectA(this: v5, pvargDest: &pvarg, sUOL: v147, vParam: v148, vAux: p_pvargDest);`
  - `LOBYTE(v167) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v150, a2: v151);`
  - `if ( Unknown != 0 )`
  - `v6 = _com_ptr_t<_com_IIID<IWzProperty,&__s_GUID const _GUID_986515d9_0a0b_4929_8b4f_718682177b92>>::_QueryInterface<IUnknown *>(a1: &Unknown);`
  - `if ( v6 < 0 && v6 != -2147467262 )`
  - `LOBYTE(v167) = 7;`
  - …（截断 16 行）

#### TabPartySearch::SetControl_As_Default  (1824 B)

  - `void __thiscall TabPartySearch::SetControl_As_Default(TabPartySearch *this)`
  - `v2 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v19) + 4);`
  - `if ( i != 0 )`
  - `sub_428C44(this: &v19, a2: 0);`
  - `v3 = _ZtlSecureFuse<unsigned char>(a1: v2 + 51, a2: *(v2 + 53));`
  - `v4 = (this + 1504);`
  - `for ( i = 25; i != 0; --i )`
  - `(*(*(*v4 + 1) + 28))(a1: *v4 + 4, a2: 1);`
  - `CCtrlCheckBox::SetChecked(this: *v4, a2: 1);`
  - `(*(*(*v4 + 1) + 28))(a1: *v4 + 4, a2: 0);`
  - `v5 = alloca(32);`
  - `if ( v3 - 15 <= 0 )`
  - `v7 = *(this + 426);`
  - `v8 = _itoa(a1: v6, a2: &v18, a3: 10);`
  - `CCtrlEdit::SetText(this: v7, a2: v8);`
  - `v9 = alloca(32);`
  - …（截断 16 行）

#### TabPartySearch::StartSearch  (2611 B)

- SP_ 节点：`SP_323_YOU_ARE_NOT_THE_MASTER_OF_THE_PARTY`
  - `void __thiscall TabPartySearch::StartSearch(TabPartySearch *this)`
  - `if ( *(*dword_BE7918 + 11988) != 0 && *(*dword_BE7918 + 12166) == *(*dword_BE7918 + 8352) )`
  - `if ( *(*dword_BE7918 + 12328) != 1 )`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v12) + 4);`
  - `sub_4289B7(this: &v12);`
  - `v4 = _ZtlSecureFuse<unsigned char>(a1: v3 + 51, a2: *(v3 + 53));`
  - `sub_A402CB(a1: v11);`
  - `v5 = *(this + 426);`
  - `Text = CCtrlEdit::GetText(this: v5, a2: &v13);`
  - `v11[1] = atol(a1: *Text);`
  - `if ( v13 != 0 )`
  - `v7 = CCtrlEdit::GetText(this: *(this + 428), a2: &v13);`
  - `v11[2] = atol(a1: *v7);`
  - `v8 = CCtrlEdit::GetText(this: *(this + 430), a2: &v13);`
  - `v11[3] = atol(a1: *v8);`
  - `v11[4] = sub_879415(a1: this);`
  - …（截断 16 行）

#### TabPartySearch::StopSearch  (464 B)

  - `void __thiscall TabPartySearch::StopSearch(TabPartySearch *this)`
  - `if ( *(*dword_BE7918 + 12328) != 0 )`
  - `TabPartySearch::SetControl_As_Default(this);`
  - `PARTYSERACH_SETTING::Clear(this: (*dword_BE7918 + 12308));`
  - `*(*dword_BE7918 + 12328) = 0;`
  - `CWvsContext::ClosePartySearch_Remocon(this: *dword_BE7918);`
  - `CWvsContext::SendCancelPartyWanted(this: *dword_BE7918);`

### CUIPartyHP（1 函数）


#### CUIPartyHP::Create  (3326 B)

  - `_DWORD *__thiscall CUIPartyHP::Create(int *this)`
  - `get_basic_font(a1: &v25, fType: 1);`
  - `v23 = (v1 + 11992);`
  - `if ( *v23 != 0 )`
  - `LOBYTE(v28) = 2;`
  - `sub_A3F836(a1: v26, a2: v13, a3: &v24, a4: v14, a5: &v21, a6: v15, a7: v16, a8: v17);`
  - `VariantInit(&pvarg);`
  - `v3 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v3 < 0 )`
  - `LOBYTE(v28) = 3;`
  - `v4 = sub_428226(a1: &v24);`
  - `_bstr_t::_bstr_t(a1: v4);`
  - `sub_4284A8(a1: &v25);`
  - `v20 = sub_42782E(a1: v10, a2: p_pvarg);`
  - `if ( v6 < 0 )`
  - `if ( v20 > v27 )`
  - …（截断 16 行）

### CUIPartySearch（1 函数）


#### CUIPartySearch::StartSearch  (371 B)

  - `void __thiscall CUIPartySearch::StartSearch(_DWORD *this)`
  - `if ( dword_BF0FA4 != 0 )`
  - `TabPartySearch::StartSearch(this: dword_BF0FA4);`
  - `*(this + 3082) = 1;`
  - `sub_A11D26(this);`
  - `sub_A3E264(a1: *(this + 3078), a2: *(this + 3079), a3: *(this + 3080), a4: *(this + 3081));`

### CTrunkDlg（22 函数）


#### CTrunkDlg::Draw  (2025 B)

  - `tagVARIANT::$::$2E851DBA257FA738680F86C475CAC6EB::$E3ADF3533BFFE4E09553D0C58E41D45E *__thiscall CTrunkDlg::Draw(`
  - `CWnd::Draw(this, a2);`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v10) + 4);`
  - `if ( v11 != 0 )`
  - `sub_428C44(this: &v10, a2: 0);`
  - `CWnd::GetCanvas(a1: &a2);`
  - `if ( a2 != 0 )`
  - `(*(a2->lVal + 4))(a1: a2);`
  - `CTrunkDlg::DrawGetItem(this, a2: v9[0]);`
  - `CTrunkDlg::DrawPutItem(this, a2: v9[0]);`
  - `v8 = *(this + 63);`
  - `sub_4281DD(a1: &a2);`
  - `CTrunkDlg::DrawMoney(a1: v7, a2: v8, a3: v9[0]);`
  - `v8 = sub_7C910D(this: v3);`
  - `return (*(a2->lVal + 8))(a1: a2);`

#### CTrunkDlg::DrawGetItem  (10496 B)

  - `tagVARIANT::$::$2E851DBA257FA738680F86C475CAC6EB::$E3ADF3533BFFE4E09553D0C58E41D45E *__thiscall CTrunkDlg::DrawGetItem(`
  - `v53 = *(*(this + 47) + 56);`
  - `if ( v2[60] == v53 )`
  - `sub_402FAB(a1: 255, a2: 3);`
  - `LOBYTE(v57) = 1;`
  - `if ( a2 == 0 )`
  - `v4 = (*(lVal + 128))(`
  - `if ( v4 < 0 )`
  - `LOBYTE(v57) = 0;`
  - `if ( v53 >= v2[62] )`
  - `LOBYTE(v57) = 2;`
  - `v7 = (*(v6 + 128))(`
  - `if ( v7 < 0 )`
  - `if ( v8 < 0 )`
  - `if ( v9 != 0 && v53 < *(v9 - 4) )`
  - `HIDWORD(v41[0]) = v53;`
  - …（截断 16 行）

#### CTrunkDlg::DrawMoney  (3480 B)

  - `VARIANTARG *__stdcall CTrunkDlg::DrawMoney(VARIANTARG *a1, VARIANTARG *a2, int a3)`
  - `format_integer(a1: &v26, a2, a3: 1);`
  - `VariantInit(&pvarg);`
  - `v3 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v3 < 0 )`
  - `LOBYTE(v27) = 2;`
  - `v4 = sub_428226(a1: &v26);`
  - `_bstr_t::_bstr_t(a1: v4);`
  - `LOBYTE(v27) = 3;`
  - `basic_font = get_basic_font(a1: &a2, fType: 24);`
  - `LOBYTE(v27) = 5;`
  - `sub_4284A8(a1: basic_font);`
  - `v7 = sub_42782E(a1: v21, a2: p_pvarg);`
  - `if ( a2 != 0 )`
  - `(*(*&a2->vt + 8))(a1: a2);`
  - `LOBYTE(v27) = 1;`
  - …（截断 16 行）

#### CTrunkDlg::DrawPutItem  (10014 B)

  - `tagVARIANT::$::$2E851DBA257FA738680F86C475CAC6EB::$E3ADF3533BFFE4E09553D0C58E41D45E *__thiscall CTrunkDlg::DrawPutItem(`
  - `v54 = *(*(this + 49) + 56);`
  - `if ( v3 != 0 )`
  - `if ( v54 < *(v3 - 4) )`
  - `v5 = (v3 + v52);`
  - `LOBYTE(v57) = 1;`
  - `if ( v6 )`
  - `sub_402FAB(a1: 255, a2: 3);`
  - `LOBYTE(v57) = 2;`
  - `if ( a2 == 0 )`
  - `v8 = (*(lVal + 128))(`
  - `if ( v8 < 0 )`
  - `v4 = HIDWORD(v41[0]);`
  - `if ( v9 < 0 )`
  - `v41[0] = __PAIR64__(v10, v4);`
  - `if ( v10 != 0 )`
  - …（截断 16 行）

#### CTrunkDlg::GetItemIndexFromPoint  (2345 B)

  - `signed int __thiscall CTrunkDlg::GetItemIndexFromPoint(_DWORD *this, int a2, int a3, _DWORD *a4, unsigned int *a5)`
  - `v22 = *(*(this + 47) + 56);`
  - `v6 = *(this + 53);`
  - `if ( v6 == 0 || v22 >= *(v6 - 4) )`
  - `if ( *(this + 58) == 0 )`
  - `v9 = sub_40B947(a1: *v7);`
  - `v10 = sub_40B920(a1: v8);`
  - `dword_BF040C(a1: v20, a2: 7, a3: v23, a4: v10 + 7, a5: v18);`
  - `if ( dword_BF0484(a1: v20, a2, a3) != 0 )`
  - `while ( v23 < 287 );`
  - `v22 = *(*(this + 49) + 56);`
  - `v13 = *(this + 54);`
  - `if ( v13 == 0 || v22 >= *(v13 - 4) )`
  - `if ( v21 == 0 )`
  - `v16 = sub_40B947(a1: v21);`
  - `v17 = sub_40B920(a1: v15);`
  - …（截断 16 行）

#### CTrunkDlg::OnButtonClicked  (794 B)

- 分支 case：`1000`, `1001`, `1002`, `1003`, `1004`, `1`, `2`
  - `void __thiscall CTrunkDlg::OnButtonClicked(_DWORD *this, int a2)`
  - `switch ( a2 )`
  - `CTrunkDlg::SendGetItemRequest(this);`
  - `CTrunkDlg::SendPutItemRequest(this);`
  - `CTrunkDlg::SendSortItemRequest(this);`
  - `CTrunkDlg::SendGetMoneyRequest(this);`
  - `CTrunkDlg::SendPutMoneyRequest(this);`
  - `(*(*this + 52))(a1: 1);`
  - `(*(*this + 52))(a1: 2);`

#### CTrunkDlg::OnCreate  (21041 B)

- SP_ 节点：`SP_1253_UI_BASICIMG_ITEMNO`, `SP_1822_UI_UIWINDOWIMG_TRUNK_EN`, `SP_1823_UI_UIWINDOWIMG_TRUNK_BTGET`, `SP_1824_UI_UIWINDOWIMG_TRUNK_BTPUT`, `SP_1825_UI_UIWINDOWIMG_TRUNK_BTSORT`, `SP_2854_UI_UIWINDOWIMG_TRUNK_SELECT`, `SP_2856_UI_UIWINDOW_TRUNK_BTOUTCOIN`, `SP_2857_UI_UIWINDOW_TRUNK_BTINCOIN`, `SP_2859_UI_UIWINDOWIMG_TRUNK_TAB_ENABLED`, `SP_2860_UI_UIWINDOWIMG_TRUNK_TAB_DISABLED`
  - `void __thiscall CTrunkDlg::OnCreate(char *this, int a2)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v135) = 1;`
  - `v5 = sub_406472();`
  - `v6 = sub_402680(a1: v5);`
  - `IWzResMan::GetObjectA(this: v6, pvargDest: &v121, sUOL: v110, vParam: p_pvarg, vAux: p_pvargDest);`
  - `LOBYTE(v135) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: p_Unknown, a2: v114);`
  - `v7 = sub_41E527(a1: &Unknown);`
  - `if ( v7 < 0 && v7 != -2147467262 )`
  - `if ( v8 < 0 )`
  - `LOBYTE(v135) = 0;`
  - `if ( v9 < 0 )`
  - `if ( v10 < 0 )`
  - `LOBYTE(v135) = 5;`
  - …（截断 16 行）

#### CTrunkDlg::OnMouseButton  (1024 B)

  - `void __thiscall CTrunkDlg::OnMouseButton(_DWORD *this, int a2, int a3, int a4, int a5)`
  - `if ( a2 == 513 )`
  - `if ( CTrunkDlg::GetItemIndexFromPoint(this: this - 1, a2: a4, a3: a5, a4: &a5, a5: &a2) != 0 )`
  - `if ( a5 != 0 )`
  - `*(this + 59) = a2;`
  - `*(this + 60) = -1;`
  - `*(this + 59) = -1;`
  - `*(this + 60) = v6;`
  - `CWnd::InvalidateRect(this: (this - 1), a2: 0);`
  - `else if ( a2 == 515 && CTrunkDlg::GetItemIndexFromPoint(this: this - 1, a2: a4, a3: a5, a4: &a5, a5: &a2) != 0 )`
  - `if ( a2 == *(this + 59) )`
  - `CTrunkDlg::SendGetItemRequest(this: this - 1);`
  - `else if ( a2 == *(this + 60) )`
  - `CTrunkDlg::SendPutItemRequest(this: this - 1);`

#### CTrunkDlg::OnMouseMove  (908 B)

  - `signed int __thiscall CTrunkDlg::OnMouseMove(char **this, int a2, int a3)`
  - `if ( CTrunkDlg::GetItemIndexFromPoint(this: this - 1, a2, a3, a4: &v10, a5: &a3) != 0 )`
  - `if ( v10 != 0 )`
  - `v6 = *(this + 52);`
  - `v6 = *(this + 53);`
  - `v9 = *(v6 + 28 * a3 + 24);`
  - `v8 = ((*(*this + 12))(a1: this) + v3 + 20);`
  - `v7 = (*(*this + 11))(a1: this);`
  - `CUIToolTip::ShowItemToolTip(this: this + 68, a2: (a2 + v7), a3: v8, a4: v9, a5: 0, a6: 0, a7: 0, a8: 0, a9: 0);`
  - `CUIToolTip::ClearToolTip(this: (this + 68));`

#### CTrunkDlg::OnPacket  (6127 B)

- 分支 case：`16`, `17`, `19`
- SP_ 节点：`SP_5599_YOU_DO_NOT_HAVE_ENOUGH_MESOS`, `SP_853_PLEASE_CHECK_IF_YOUR_INVENTORY_IS_FULL_OR_NOT`, `SP_855_DUE_TO_AN_ERROR_THE_TRADE_DID_NOT_HAPPEN`, `SP_865_THE_STORAGE_IS_FULL`, `SP_866_ITEM_COULD_NOT_BE_RETRIEVED_R_NBECAUSE_THERE_WAS_AN_ITEM_THAT_R_NCOULD_ONLY_BE_A`
  - `void __cdecl CTrunkDlg::OnPacket(int a1)`
  - `v1 = CInPacket::Decode1(this: a1);`
  - `if ( dword_BE7910 != 0 )`
  - `v4 = (*(*(dword_BE7910 + 4) + 72))(a1: dword_BE7910 + 4, a2: &dword_BF0DB4) != 0 ? dword_BE7910 : 0;`
  - `if ( v3 != 21 && v3 != 22 )`
  - `if ( v4 == 0 )`
  - `if ( *(v4 + 260) == 0 )`
  - `_CxxThrowException(&a1, a2: &_TI3_AVCDisconnectException__);`
  - `*(v4 + 260) = 0;`
  - `if ( v3 > 15 )`
  - `switch ( v3 )`
  - `if ( v3 != 22 )`
  - `if ( v3 == 23 )`
  - `if ( CInPacket::Decode1(this: a1) != 0 )`
  - `CInPacket::DecodeStr(&a1);`
  - `sub_428211(&a1);`
  - …（截断 16 行）

#### CTrunkDlg::ResetInfo  (259 B)

  - `void __thiscall CTrunkDlg::ResetInfo(CWnd *this)`
  - `CTrunkDlg::SetPutItems(this, a2: 0, a3: this + 216);`
  - `CTrunkDlg::SetScrollBar(this);`
  - `CTrunkDlg::SetAvatar(this);`
  - `CWnd::InvalidateRect(this, a2: 0);`

#### CTrunkDlg::SendGetItemRequest  (4544 B)

- SP_ 节点：`SP_4661_ONCE_FOUND_THE_ITEM_CANNOT_BE_TRADED_R_NWOULD_YOU_STILL_LIKE_TO_FIND_IT`, `SP_858_ARE_YOU_SURE_YOU_WANT_TO__R_NRECOVER_THE_ITEMS_FROM_STORAGE`, `SP_859_RECOVERING_THE_ITEMS_WILL_COST_D_MESOS_R_NARE_YOU_SURE_YOU_WANT_TO_RECOVER_THIS`
  - `void __thiscall CTrunkDlg::SendGetItemRequest(int *this)`
  - `v2 = *(this + 60);`
  - `if ( v2 >= 0 )`
  - `v3 = *(this + 53);`
  - `v4 = v3 != 0 ? *(v3 - 4) : 0;`
  - `if ( v2 < v4 && *(this + 65) == 0 )`
  - `v5 = *(this + 53) + 28 * v2;`
  - `v25 = *(v5 + 24);`
  - `if ( v25 != 0 )`
  - `InterlockedIncrement(lpAddend: (v25 + 4));`
  - `if ( (*(v8 + 20))(a1: v7) == 0 )`
  - `if ( CUtilDlg::YesNo(a1: v18, a2: v19, a3: v20, a4: v21, a5: v22) == 6 )`
  - `v11 = *(this + 51) <= 0;`
  - `LOBYTE(v30) = 1;`
  - `if ( v11 )`
  - `LOBYTE(v30) = 3;`
  - …（截断 16 行）

#### CTrunkDlg::SendGetMoneyRequest  (1220 B)

- SP_ 节点：`SP_863_HOW_MUCH_WILL_YOU_TAKE_OUT`
  - `void __thiscall CTrunkDlg::SendGetMoneyRequest(int *this)`
  - `if ( *(this + 65) == 0 )`
  - `v4 = *(this + 63);`
  - `v5 = sub_7C8FA5(a1: v3, a2: v4, a3: v4);`
  - `if ( v6 > 0 && v6 <= *(this + 63) )`
  - `COutPacket::COutPacket(a1: 62);`
  - `COutPacket::Encode1(this: &v7, a2: 7);`
  - `COutPacket::Encode4(this: &v7, a2: v6);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v7);`
  - `*(this + 65) = 1;`
  - `ZArray<unsigned char>::RemoveAll(a1: v8);`

#### CTrunkDlg::SendPutItemRequest  (6136 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_4660_ONCE_STORED_IN_THE_INVENTORY_R_NTHE_ITEM_CANNOT_BE_TRADED_R_NBY_THE_CHARACTER_WH`, `SP_860_ARE_YOU_SURE_YOU_WANT_TO_STORE_THE_ITEMS`, `SP_861_STORING_THIS_WILL_COST_D_MESOS_R_NARE_YOU_SURE_YOU_WANT_TO_STORE_THIS`, `SP_862_HOW_MANY_WILL_YOU_STORE`
  - `void __thiscall CTrunkDlg::SendPutItemRequest(_DWORD *this)`
  - `v2 = *(this + 61);`
  - `if ( v2 >= 0 )`
  - `v3 = *(this + 54);`
  - `v4 = v3 != 0 ? *(v3 - 4) : 0;`
  - `if ( v2 < v4 && *(this + 65) == 0 )`
  - `v5 = (*(this + 54) + 28 * v2);`
  - `sub_4288AF(a1: v26);`
  - `if ( (*(v27->dwLowDateTime + 20))(a1: v27, a2: v28 % 1000000) != 0 )`
  - `if ( CUtilDlg::YesNo(a1: v19, a2: v20, a3: v21, a4: v22, a5: v23) != 6 )`
  - `if ( sub_4F3144(ebx0: 0, a2: v27) )`
  - `LOBYTE(v37) = 1;`
  - `v34 = sub_7C8FA5(a1: v10, a2: v6, a3: v6);`
  - `LOBYTE(v37) = 0;`
  - `if ( v32 != 0 )`
  - `if ( v34 > 0 && v34 <= v6 )`
  - …（截断 16 行）

#### CTrunkDlg::SendPutMoneyRequest  (1832 B)

- SP_ 节点：`SP_864_HOW_MUCH_WILL_YOU_SAVE`
  - `void __thiscall CTrunkDlg::SendPutMoneyRequest(_DWORD *this)`
  - `v1 = *(this + 65) == 0;`
  - `if ( v1 )`
  - `v2 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v10) + 4);`
  - `if ( v11 != 0 )`
  - `sub_428C44(this: &v10, a2: 0);`
  - `v3 = _ZtlSecureFuse<long>(a1: v2 + 165, a2: *(v2 + 173));`
  - `v13 = _ZtlSecureFuse<long>(a1: v2 + 165, a2: *(v2 + 173));`
  - `v6 = sub_7C8FA5(a1: v5, a2: v13, a3: v3);`
  - `if ( v7 > 0 && v7 <= _ZtlSecureFuse<long>(a1: v2 + 165, a2: *(v2 + 173)) )`
  - `COutPacket::COutPacket(a1: 62);`
  - `COutPacket::Encode1(this: &v8, a2: 7);`
  - `COutPacket::Encode4(this: &v8, a2: -v7);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v8);`
  - `ZArray<unsigned char>::RemoveAll(a1: v9);`

#### CTrunkDlg::SendSortItemRequest  (648 B)

  - `void *__thiscall CTrunkDlg::SendSortItemRequest(_DWORD *this)`
  - `if ( *(this + 65) == 0 )`
  - `COutPacket::COutPacket(a1: 62);`
  - `COutPacket::Encode1(this: &v3, a2: 6);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v3);`
  - `*(this + 65) = 1;`
  - `return ZArray<unsigned char>::RemoveAll(a1: v4);`

#### CTrunkDlg::SetAvatar  (2307 B)

  - `_DWORD *__thiscall CTrunkDlg::SetAvatar(CTrunkDlg *this)`
  - `ZRef<CAvatar>::_Alloc(a1: this + 264);`
  - `CWnd::GetLayer(a1: &v13);`
  - `Layer = CWnd::GetLayer(a1: &v22);`
  - `LOBYTE(v25) = 1;`
  - `sub_428EC4(a1: Layer);`
  - `LOBYTE(v25) = 2;`
  - `v5 = sub_42861D();`
  - `CharacterData = CWvsContext::GetCharacterData(this: v5, a2: &v23);`
  - `LOBYTE(v25) = 3;`
  - `v7 = ZRef<CharacterData>::~ZRef<CharacterData>(a1: CharacterData);`
  - `v8 = AvatarLook::AvatarLook(a1: v7);`
  - `LOBYTE(v25) = 7;`
  - `v11 = sub_426626(a1: 2, a2: 1);`
  - `v9 = sub_4285F2(a1: v1);`
  - `CAvatar::Init(`
  - …（截断 16 行）

#### CTrunkDlg::SetGetItems  (5057 B)

- 分支 case：`1`, `2`, `3`, `4`, `5`
  - `int __thiscall CTrunkDlg::SetGetItems(CTrunkDlg *this, struct CInPacket *a2)`
  - `*(this + 62) = CInPacket::Decode1(this: a2);`
  - `CInPacket::DecodeBuffer(a1: &v23, a2: 8u);`
  - `if ( (v23 & 2) != 0 )`
  - `v2[63] = CInPacket::Decode4(this: a2);`
  - `sub_7C91A2(this: &v31);`
  - `for ( i = 1; i <= 5; ++i )`
  - `switch ( i )`
  - `if ( (v23 & v4) != 0 )`
  - `v6 = CInPacket::Decode1(this: a2);`
  - `if ( v6 != 0 )`
  - `sub_7C609D(this: v18);`
  - `LOBYTE(v33) = 1;`
  - `v8 = GW_ItemSlotBase::Decode(a1: v22, a2);`
  - `LOBYTE(v33) = 2;`
  - `sub_4288AF(a1: v8);`
  - …（截断 16 行）

#### CTrunkDlg::SetNPC  (10041 B)

  - `int __thiscall CTrunkDlg::SetNPC(CTrunkDlg *this)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `LOBYTE(v57) = 1;`
  - `if ( dword_BF14EC == 0 )`
  - `v2 = IWzGr2D::CreateLayer(`
  - `v3 = (this + 272);`
  - `sub_428712(a1: *v2);`
  - `sub_4156C5(a1: &v56);`
  - `LOBYTE(v57) = 0;`
  - `if ( v4 < 0 )`
  - `if ( v5 < 0 )`
  - `v6 = *CWnd::GetLayer(a1: &v55);`
  - `sub_410FDF(this: &pvargDest, a2: v6, a3: 1);`
  - `LOBYTE(v57) = 3;`
  - `if ( v7 )`
  - …（截断 16 行）

#### CTrunkDlg::SetPutItems  (3863 B)

  - `int __thiscall CTrunkDlg::SetPutItems(_DWORD *this, signed int a2, int *a3)`
  - `if ( a2 == 0 )`
  - `a2 = *(*(this + 45) + 60) + 1;`
  - `v4 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v19) + 4);`
  - `sub_4289B7(this: v19);`
  - `sub_7C91A2(this: a3);`
  - `v26 = (v4 + 4 * a2 + 1095);`
  - `while ( 1 )`
  - `v5 = *v26 != 0 ? *(*v26 - 4) : 0;`
  - `if ( v28 > result )`
  - `v7 = *(CharacterData::GetItem(this: v4, a2: v18, a3: a2, a4: v28) + 4);`
  - `sub_4288C1(a1: v18);`
  - `if ( v7 != 0 )`
  - `Data = TSecType<long>::GetData(a1: v7 + 12, a2: 0);`
  - `if ( CItemInfo::IsCashItem(a1: Data) == 0 && *(v7 + 3) == 0 )`
  - `v9 = TSecType<long>::GetData(a1: v7 + 12, a2: 0);`
  - …（截断 16 行）

#### CTrunkDlg::SetScrollBar  (556 B)

  - `void __thiscall CTrunkDlg::SetScrollBar(CTrunkDlg *this)`
  - `v2 = *(this + 62);`
  - `if ( v2 <= 5 )`
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 47), a2: v3);`
  - `v4 = *(this + 54);`
  - `if ( v4 != 0 && *(v4 - 4) > 4u )`
  - `v5 = *(v4 - 4) - 3;`
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 49), a2: v5);`

#### CTrunkDlg::SetTrunkDlg  (436 B)

  - `void __thiscall CTrunkDlg::SetTrunkDlg(CWnd *this, struct CInPacket *a2)`
  - `*(this + 50) = CInPacket::Decode4(this: a2);`
  - `CTrunkDlg::SetGetItems(this, a2);`
  - `CTrunkDlg::SetPutItems(this, a2: 0, a3: this + 216);`
  - `CTrunkDlg::SetScrollBar(this);`
  - `CTrunkDlg::SetAvatar(this);`
  - `CTrunkDlg::SetNPC(a1: this);`
  - `CWnd::InvalidateRect(this, a2: 0);`

### CTradingRoomDlg（17 函数）


#### CTradingRoomDlg::Draw  (14061 B)

  - `int __thiscall CTradingRoomDlg::Draw(CTradingRoomDlg *this, const struct tagRECT *a2)`
  - `CMiniRoomBaseDlg::Draw(this, a2);`
  - `CWnd::GetCanvas(a1: &v75);`
  - `v3 = *(*dword_BE7918 + 8376);`
  - `LOBYTE(v76) = 1;`
  - `if ( v3 != 0 )`
  - `v4 = (v3 + 4);`
  - `ZXString<char>::GetBuffer(a1: v4, a2: 0xFFFFFFFF);`
  - `LOBYTE(v76) = 2;`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `LOBYTE(v76) = 3;`
  - `v5 = alloca(2 * dword_BF03D0(a1: 0, a2: 0, a3: v73, a4: -1, a5: 0, a6: 0));`
  - `v6 = sub_428226(a1: &v73);`
  - `v7 = sub_46ABB2(a1: v6, a2: v68, a3: -1);`
  - `_bstr_t::_bstr_t(a1: v7);`
  - `sub_4284A8(a1: this + 432);`
  - …（截断 16 行）

#### CTradingRoomDlg::DrawItems  (3780 B)

  - `int *__thiscall CTradingRoomDlg::DrawItems(char *this, int *a2)`
  - `v2 = 39 * (v31 % 3) + 171;`
  - `v3 = 37 * (v31 / 3) + 161;`
  - `LOBYTE(v32) = 1;`
  - `for ( i = 2; i != 0; --i )`
  - `sub_4051E5(a1: *(v30 + *v4 + 8));`
  - `if ( v29 != 0 )`
  - `v5 = *(sub_7C3CCB(a1: v31) + 4);`
  - `if ( v5 != 0 )`
  - `InterlockedIncrement(lpAddend: (v5 + 4));`
  - `sub_4283F8();`
  - `v7 = sub_5CA1B8(a1: v24, a2: v25);`
  - `v8 = sub_7C3CCB(a1: v31);`
  - `v9 = sub_428244(a1: v8);`
  - `v20 = (*(*v9 + 4))(a1: v9);`
  - `v10 = sub_7C3CCB(a1: v31);`
  - …（截断 16 行）

#### CTradingRoomDlg::GetActionPosition  (466 B)

  - `struct tagPOINT *__thiscall CTradingRoomDlg::GetActionPosition(`
  - `if ( a2 == *(this + 50) )`

#### CTradingRoomDlg::GetItemIndexFromPoint  (943 B)

  - `int __thiscall CTradingRoomDlg::GetItemIndexFromPoint(CTradingRoomDlg *this, int a2, int a3)`
  - `while ( 1 )`
  - `v3 = 39 * (v9 % 3);`
  - `v4 = 37 * (v9 / 3);`
  - `dword_BF040C(a1: v8, a2: v3 + 172, a3: v4 + 131, a4: v3 + 204, a5: v4 + 163);`
  - `if ( dword_BF0484(a1: v8, a2, a3) != 0 )`
  - `dword_BF040C(a1: v8, a2: v3 + 10, a3: v6, a4: v3 + 42, a5: v5);`
  - `result = dword_BF0484(a1: v8, a2, a3);`
  - `if ( result != 0 )`
  - `if ( ++v9 >= 9 )`

#### CTradingRoomDlg::HitTest  (652 B)

  - `int __stdcall CTradingRoomDlg::HitTest(signed int rx, signed int ry, _DWORD *ppCtrl)`
  - `result = CWnd::HitTest(a1: rx, a2: ry, a3: ppCtrl);`
  - `if ( result == 2 )`
  - `if ( rx >= 288 && ry >= 275 )`
  - `else if ( result == 0 )`
  - `if ( ppCtrl == 0 || *ppCtrl == 0 )`
  - `dword_BF040C(a1: v4, a2: 0, a3: 128, a4: 288, a5: 148);`
  - `return (dword_BF0484(a1: v4, a2: rx, a3: ry) != 0) + 1;`

#### CTradingRoomDlg::OnButtonClicked  (1714 B)

  - `void __thiscall CTradingRoomDlg::OnButtonClicked(char **this, unsigned int a2)`
  - `switch ( a2 )`
  - `CTradingRoomDlg::Trade(this);`
  - `CTradingRoomDlg::PutMoney(this);`
  - `v4 = *(this + 107);`
  - `if ( v4 != 0 )`
  - `v5 = *CCtrlEdit::GetText(this: v4, a2: &v9);`
  - `if ( !v6 )`
  - `CCtrlEdit::GetText(this: *(this + 107), &a2);`
  - `v7 = *(this + 107);`
  - `CCtrlEdit::SetText(this: v7, a2: 0);`
  - `ZXString<char>::operator=(a1: &a2);`
  - `CMiniRoomBaseDlg::CheckAndSendChat(a1: v8[0]);`
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: *(this + 107) != 0 ? (*(this + 107) + 4) : 0);`
  - `(*(v3 + 13))(a1: this, a2: v8[0]);`

#### CTradingRoomDlg::OnCreate  (10340 B)

- SP_ 节点：`SP_1253_UI_BASICIMG_ITEMNO`, `SP_1260_UI_BASICIMG_BTCLAIM`, `SP_1818_UI_UIWINDOWIMG_TRADINGROOM_BTTRADE`, `SP_1819_UI_UIWINDOWIMG_TRADINGROOM_BTCOIN`, `SP_1820_UI_UIWINDOWIMG_TRADINGROOM_BTRESET`, `SP_2722_UI_UIWINDOWIMG_MESSENGER_BTENTER`, `SP_5527_ARIAL`
  - `void __thiscall CTradingRoomDlg::OnCreate(CTradingRoomDlg *this, void *a2)`
  - `CCtrlEdit::CREATEPARAM::CREATEPARAM(this: v51);`
  - `LOBYTE(v66) = 1;`
  - `sub_402C9B(a1: BSTR);`
  - `sub_402D9A(this: &Unknown);`
  - `v5 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 184);`
  - `LOBYTE(v66) = 2;`
  - `if ( v5 != 0 )`
  - `v6 = CCtrlEdit::CCtrlEdit(this: v5);`
  - `sub_473956(a1: v6);`
  - `v7 = *(this + 107);`
  - `(*(*v7 + 8))(a1: v7, a2: this, a3: 1006, a4: 296, a5: 245, a6: 192, a7: 15, a8: v51);`
  - `CWnd::SetFocusChild(this, a2: *(this + 107));`
  - `LOBYTE(v66) = 3;`
  - `ZXString<unsigned short>::operator=(a1: StringW);`
  - `v10 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1444);`
  - …（截断 16 行）

#### CTradingRoomDlg::OnKey  (1139 B)

  - `void __thiscall CTradingRoomDlg::OnKey(CTradingRoomDlg *this, unsigned int a2, signed int a3)`
  - `if ( a3 >= 0 && a2 == 13 )`
  - `v4 = *(this + 106);`
  - `if ( v4 != 0 )`
  - `v5 = *CCtrlEdit::GetText(this: v4, &a2);`
  - `if ( !v6 )`
  - `CCtrlEdit::GetText(this: *(this + 106), a2: &a3);`
  - `v7 = *(this + 106);`
  - `CCtrlEdit::SetText(this: v7, a2: 0);`
  - `ZXString<char>::operator=(a1: &a3);`
  - `CMiniRoomBaseDlg::CheckAndSendChat(a1: v8[0]);`
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: *(this + 106) != 0 ? (*(this + 106) + 4) : 0);`

#### CTradingRoomDlg::OnLeave  (4689 B)

- SP_ 节点：`SP_406_TRADE_CANCELLED_R_NBY_THE_OTHER_CHARACTER`, `SP_407_TRADE_SUCCESSFUL_R_NPLEASE_CHECK_THE_RESULTS`, `SP_408_TRADE_SUCCESSFUL_R_NRECEIVED_D_MESOS_AFTER_FEES_R_NPLEASE_CHECK_THE_RESULTS`, `SP_409_TRADE_UNSUCCESSFUL`, `SP_410_YOU_CANNOT_MAKE_THE_TRADE_BECAUSE_R_NTHERE_ARE_SOME_ITEMS_WHICH_R_NYOU_CANNOT_CA`, `SP_411_YOU_CANNOT_MAKE_THE_TRADE_BECAUSE_R_NTHE_OTHER_PERSONS_ON_A_DIFFERENT_MAP`, `SP_5566_THE_GAME_FILE_HAS_BEEN_DAMAGED_R_NSO_YOU_CANNOT_PARTICIPATE_IN_AN_ITEM_TRADE_PLE`
  - `void __thiscall CTradingRoomDlg::OnLeave(CTradingRoomDlg *this, int a2, struct CInPacket *a3)`
  - `v4 = CInPacket::Decode1(this: a3);`
  - `v5 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v24) + 4);`
  - `v23 = *(v5 + 173);`
  - `v6 = _ZtlSecureFuse<long>(a1: v5 + 165, a2: v23);`
  - `if ( v24[1] != 0 )`
  - `sub_428C44(this: v24, a2: 0);`
  - `if ( v4 == 7 )`
  - `v7 = *(this + 117);`
  - `if ( v26 > v7 && v26 - v7 < *(this + 116) - *(this + 115) )`
  - `(*(*this + 52))(a1: this, a2: 8);`
  - `if ( v9 != 0 )`
  - `if ( v10 != 0 )`
  - `if ( v11 != 0 )`
  - `if ( v12 != 0 )`
  - `if ( v13 != 0 )`
  - …（截断 16 行）

#### CTradingRoomDlg::OnMouseMove  (960 B)

  - `int __thiscall CTradingRoomDlg::OnMouseMove(char **this, int a2, int a3)`
  - `ItemIndexFromPoint = CTradingRoomDlg::GetItemIndexFromPoint(this: (this - 1), a2, a3);`
  - `if ( ItemIndexFromPoint != 0`
  - `&& (ItemIndexFromPoint <= 0`
  - `? (v5 = &(*(this + 113))[-16 * ItemIndexFromPoint - 16])`
  - `: (v5 = &(*(this + 112))[16 * ItemIndexFromPoint - 16]),`
  - `v5 != 0 && *(v5 + 12) >= 0) )`
  - `v9 = *(v5 + 4);`
  - `v8 = ((*(*this + 12))(a1: this) + a3 + 20);`
  - `v6 = (*(*this + 11))(a1: this);`
  - `CUIToolTip::ShowItemToolTip(this: this + 117, a2: (a2 + v6), a3: v8, a4: v9, a5: 0, a6: 0, a7: 0, a8: 0, a9: 0);`
  - `CUIToolTip::ClearToolTip(this: (this + 117));`

#### CTradingRoomDlg::OnPacket  (437 B)

  - `int __thiscall CTradingRoomDlg::OnPacket(_DWORD *this, int a2, int a3)`
  - `if ( a2 == 15 )`
  - `return CTradingRoomDlg::OnPutItem(this, a2: a3);`
  - `if ( a2 == 16 )`
  - `return CTradingRoomDlg::OnPutMoney(a1: a3);`
  - `if ( a2 == 17 )`
  - `return CTradingRoomDlg::OnTrade(a1: a3);`

#### CTradingRoomDlg::OnPutItem  (1125 B)

  - `void __thiscall CTradingRoomDlg::OnPutItem(CWnd *this, int a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `v4 = CInPacket::Decode1(this: a2);`
  - `GW_ItemSlotBase::Decode(a1: v8, a2);`
  - `v5 = *(this + v3 + 117);`
  - `*(v5 + 16 * v4 - 16 + 12) = (*(*v9 + 32))(a1: v9);`
  - `sub_428285(a1: v8);`
  - `Data = TSecType<long>::GetData(a1: v9 + 12, a2: dword_BE78D8);`
  - `ItemIcon = CItemInfo::GetItemIcon(a1: &a2, a2: Data, bPetDead: 1, bWithShadow: 0);`
  - `sub_4051E5(a1: *ItemIcon);`
  - `if ( a2 != 0 )`
  - `(*(*a2 + 8))(a1: a2);`
  - `CWnd::InvalidateRect(this: v10, a2: 0);`
  - `if ( v9 != 0 )`
  - `ZRef<GW_ItemSlotBase>::~ZRef<GW_ItemSlotBase>(a1: 0);`

#### CTradingRoomDlg::OnPutMoney  (276 B)

  - `void __thiscall CTradingRoomDlg::OnPutMoney(CTradingRoomDlg *this, struct CInPacket *a2)`
  - `v3 = CInPacket::Decode1(this: a2);`
  - `*(this + v3 + 119) = CInPacket::Decode4(this: a2);`
  - `CWnd::InvalidateRect(this, a2: 0);`

#### CTradingRoomDlg::OnTrade  (1855 B)

  - `int __thiscall CTradingRoomDlg::OnTrade(CTradingRoomDlg *this, struct CInPacket *a2)`
  - `v3 = *(v2 + *(v13 + 118) + 4);`
  - `if ( v3 != 0 )`
  - `Data = TSecType<long>::GetData(a1: (v3 + 12), a2: v2);`
  - `ItemCRC = CItemInfo::GetItemCRC(this: dword_BE78D8, a2: Data);`
  - `v6 = ZArray<ZPair<long,unsigned long>>::InsertBefore(a1: -1);`
  - `while ( v2 < 80 );`
  - `COutPacket::COutPacket(a1: 123);`
  - `LOBYTE(v15) = 1;`
  - `COutPacket::Encode1(this: &v11, a2: 20);`
  - `if ( v14 != 0 )`
  - `v7 = *(v14 - 4);`
  - `COutPacket::Encode1(this: &v11, a2: v7);`
  - `for ( i = 0; i < v7; ++i )`
  - `COutPacket::Encode4(this: &v11, a2: *(v14 + 8 * i));`
  - `COutPacket::Encode4(this: &v11, a2: *(v14 + 8 * i + 4));`
  - …（截断 16 行）

#### CTradingRoomDlg::PutItem  (3666 B)

- SP_ 节点：`SP_887_HOW_MANY_WILL_YOU_TRADE`
  - `int __thiscall CTradingRoomDlg::PutItem(int *this, char a2, const FILETIME *a3, char a4, __int16 a5, int a6, int a7)`
  - `v8 = *(this + 111) == 0;`
  - `if ( !v8`
  - `|| *(this + 52) <= 1`
  - `|| (ItemIndexFromPoint = CTradingRoomDlg::GetItemIndexFromPoint(this, a2: a6, a3: a7),`
  - `|| *(*(this + 113) + 16 * ItemIndexFromPoint - 4) >= 0`
  - `|| (v10 = *dword_BE7918, CWvsContext::CanSendExclRequest(a1: 500, a2: 0) == 0) )`
  - `sub_4288C1(a1: &a2);`
  - `if ( !sub_4F3144(ebx0: 0, a2: a3) )`
  - `a7 = (*(a3->dwLowDateTime + 32))(a1: a3);`
  - `if ( a7 > 1 )`
  - `v11 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `LOBYTE(v26) = 1;`
  - `if ( v11 != 0 )`
  - `v12 = CUtilDlgEx::CUtilDlgEx(this: v11);`
  - `v24 = v13 != 0 ? (v13 - 8) : 0;`
  - …（截断 16 行）

#### CTradingRoomDlg::PutMoney  (4033 B)

- SP_ 节点：`SP_3977_PLAYERS_THAT_ARE_LEVEL_15_AND_BELOW__R_NMAY_ONLY_TRADE_1_MILLION_MESOS_PER_DAY__`, `SP_412_HOW_MUCH_MONEY_WILL_YOU_PUT_UP`
  - `void *__thiscall CTradingRoomDlg::PutMoney(CTradingRoomDlg *this)`
  - `if ( *(this + 111) == 0 && *(this + 52) > 1 )`
  - `result = CWvsContext::CanSendExclRequest(a1: 500, a2: 0);`
  - `if ( result != 0 )`
  - `v3 = *(CWvsContext::GetCharacterData(this: v2, a2: v22) + 4);`
  - `sub_4289B7(this: v22);`
  - `v4 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v4 != 0 )`
  - `v5 = CUtilDlgEx::CUtilDlgEx(this: v4);`
  - `v24 = v6 != 0 ? (v6 - 8) : 0;`
  - `sub_4289CD(a1: v23);`
  - `LOBYTE(v27) = 1;`
  - `v9 = sub_428688(a1: v23);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v9, a2: 2, a3: 0, a4: v17, a5: v18, a6: v19);`
  - `v10 = _ZtlSecureFuse<long>(a1: v3 + 165, a2: *(v3 + 173));`
  - `CUtilDlgEx::SetUtilDlgEx_INPUT_NO(this: v24, a2: 1, a3: 1, a4: v10, a5: 0, a6: 10, a7: 0);`
  - …（截断 16 行）

#### CTradingRoomDlg::Trade  (3579 B)

- SP_ 节点：`SP_413_ARE_YOU_SURE_YOU_WANT_TO_TRADE`, `SP_4662_SOME_ITEMS_YOU_ARE_TRYING_TO_BARTER_R_NCANNOT_BE_TRADED_ONCE_RECEIVED_R_NWOULD_Y`
  - `int __fastcall CTradingRoomDlg::Trade(CTradingRoomDlg *this)`
  - `for ( i = 0; ; i += 4 )`
  - `v3 = *(v1 + 114);`
  - `if ( v3 == 0 )`
  - `if ( j >= *(v3 - 4) )`
  - `this = *(i + v3 + 4);`
  - `if ( this != 0 && (*(*this + 20))(a1: this) != 0 )`
  - `nIdx: (v2 != 0`
  - `result = CUtilDlg::YesNo(a1: v12, a2: v13, a3: v14, a4: v15, a5: v16);`
  - `if ( result == 6 )`
  - `*(v1 + 111) = 1;`
  - `(*(*(*(v1 + 99) + 4) + 28))(a1: *(v1 + 99) + 4, a2: 0);`
  - `(*(*(*(v1 + 101) + 4) + 28))(a1: *(v1 + 101) + 4, a2: 0);`
  - `CWnd::InvalidateRect(this: v1, a2: 0);`
  - `for ( j = 0; j < 144; j += 16 )`
  - `i = (v1 + 452);`
  - …（截断 16 行）

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

### CPersonalShopDlg（10 函数）


#### CPersonalShopDlg::0CSoldItemDlg  (762 B)

- SP_ 节点：`SP_2620_UI_UIWINDOWIMG_PERSONALSHOP_BACKGRND3`
  - `_DWORD *__thiscall CPersonalShopDlg::CSoldItemDlg::CSoldItemDlg(_DWORD *this, int a2)`
  - `CDialog::CDialog(this);`
  - `*(this + 31) = a2;`
  - `*(this + 33) = 0;`
  - `*(this + 35) = 0;`
  - `*(this + 36) = 0;`
  - `*(this + 1) = &off_AFD530;`
  - `*(this + 2) = &off_AFD52C;`
  - `CDialog::CreateDlg(a1: *StringW, a2: 1, a3: 0);`

#### CPersonalShopDlg::BuyItem  (17776 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_100_MAPLESTORY_ERROR__LOADING_REACTOR_FAILED`, `SP_3931_IT_HAS_EXCEEDED_THE_LIMIT_THAT_SYSTEM_ALLOWS`, `SP_3977_PLAYERS_THAT_ARE_LEVEL_15_AND_BELOW__R_NMAY_ONLY_TRADE_1_MILLION_MESOS_PER_DAY__`, `SP_419_THE_NUMBER_OF_ITEMS_SHOULD_BE_BASED_R_NON_THE_NUMBER_OF_ITEMS_IN_A_BUNDLE`, `SP_420_THE_TOTAL_PRICE_IS_D_MESOS_R_NWILL_YOU_MAKE_THE_PURCHASE`, `SP_427_THE_ITEM_HAS_BEEN_SOLD_ALREADY`, `SP_4659_ONCE_PURCHASED_THE_ITEM_CANNOT_BE_TRADED_R_NWOULD_YOU_STILL_LIKE_TO_PURCHASE_IT`, `SP_5214_TIME_LIMIT_EXPIRED`, `SP_5215_HAVE_YOU_CHECKED_THE_R_NTIME_LIMIT_ON_THE_ITEM`, `SP_5216_DDAYS_DHOURS_AND_DMINUTES_LEFT`, `SP_5270_CASH_ITEMS_BECOME_UNTRADABLE_R_NONCE_THEY_HAVE_BEEN_TRADED_AND__R_NMAPLE_POINTS_`, `SP_5570_THE_TIME_LIMIT_HAS_EXPIRED`, `SP_848_ARE_YOU_SURE_YOU_WANT_TO_BUY_IT`, `SP_849_HOW_MANY_ARE_YOU_WILLING_TO_BUY`
  - `void __thiscall CPersonalShopDlg::BuyItem(CPersonalShopDlg *this, int a2)`
  - `v3 = *(this + 50);`
  - `if ( v3 == 0 )`
  - `if ( a2 < 0 )`
  - `v4 = *(this + 101);`
  - `if ( v4 == 0 )`
  - `if ( a2 >= *(v4 - 4) )`
  - `if ( CWvsContext::CanSendExclRequest(a1: 500, a2: 0) == 0 )`
  - `v5 = *(v2 + 101);`
  - `if ( *v6 <= 0 )`
  - `CUtilDlg::Notice(`
  - `if ( v6[4] == 0 )`
  - `v70 = sub_7B8F35();`
  - `if ( (*(v8 + 24))(a1: v7) == 3 )`
  - `if ( *(v9 + 24) != 0 && (CompareFileTime(lpFileTime1: (v9 + 32), lpFileTime2: &stru_AFD3A8) < 0 || v78 != 0) )`
  - `ZXString<char>::GetBuffer(a1: &sStrDefault, a2: 0xFFFFFFFF);`
  - …（截断 16 行）

#### CPersonalShopDlg::DeliverBlackList  (1314 B)

  - `void *__thiscall CPersonalShopDlg::DeliverBlackList(CPersonalShopDlg *this)`
  - `if ( *(this + 100) != 0 )`
  - `COutPacket::COutPacket(a1: 123);`
  - `COutPacket::Encode1(this: &v7, a2: 30);`
  - `v2 = *(TSingleton<CConfig>::ms_pInstance + 46);`
  - `if ( v2 != 0 )`
  - `v3 = *(v2 - 4);`
  - `LOWORD(v3) = 0;`
  - `COutPacket::Encode2(a1: v3);`
  - `for ( i = 0; i < v3; ++i )`
  - `sub_538C98();`
  - `v5 = sub_490E87(a1: v6);`
  - `ZXString<char>::operator=(a1: v5);`
  - `COutPacket::EncodeStr(a1: v6);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v7);`
  - `return ZArray<unsigned char>::RemoveAll(a1: v8);`
  - …（截断 16 行）

#### CPersonalShopDlg::GetItemIndexFromPoint  (718 B)

  - `int __thiscall CPersonalShopDlg::GetItemIndexFromPoint(CPersonalShopDlg *this, int a2, int a3)`
  - `v8 = *(*(this + 148) + 56);`
  - `while ( 1 )`
  - `if ( v8 >= 0 && v8 < *(this + 109) )`
  - `if ( dword_BF0484(a1: v7, a2, a3) != 0 )`
  - `if ( v5 >= 396 )`

#### CPersonalShopDlg::MoveItemToInventory  (1998 B)

- SP_ 节点：`SP_3484_YOU_MAY_ONLY_POSSESS_ONE_OF_THIS_ITEM`
  - `signed int __thiscall CPersonalShopDlg::MoveItemToInventory(_DWORD *this, int a2)`
  - `if ( CWvsContext::CanSendExclRequest(a1: 500, a2: 0) == 0 || a2 < 0 || *(*(this + 101) + 20 * a2) <= 0 )`
  - `if ( (*(*this + 92))(a1: this) != 0 )`
  - `Data = TSecType<long>::GetData(a1: (*(*(this + 101) + 20 * a2 + 16) + 12), a2: v2);`
  - `if ( CItemInfo::IsOnlyItem(a1: Data) != 0 && CWvsContext::IsExist(a1: Data) != 0 )`
  - `CUtilDlg::Notice(sMsg: v11, sSndName: v12, ppDialog: v13, bAutoSeparated: v14, bIsTightLine: v15);`
  - `COutPacket::COutPacket(a1: 123);`
  - `v9 = (*(v8 + 92))(a1: this);`
  - `COutPacket::Encode1(this: &v16, a2: v9 != 0 ? 38 : 27);`
  - `COutPacket::Encode2(a1: a2);`
  - `CClientSocket::SendPacket(this: dword_BE7914, a2: &v16);`
  - `*(v2 + 8356) = 1;`
  - `update_time = get_update_time();`
  - `*(v2 + 8360) = update_time;`
  - `ZArray<unsigned char>::RemoveAll(a1: v17);`

#### CPersonalShopDlg::OnButtonClicked  (3947 B)

  - `int __thiscall CPersonalShopDlg::OnButtonClicked(char **this, _DWORD *a2)`
  - `switch ( a2 )`
  - `CPersonalShopDlg::DeliverBlackList(this);`
  - `(*(*this + 27))(a1: this);`
  - `v4 = (*(*this + 23))(a1: this);`
  - `(*(v3 + 13))(a1: this, a2: v4 != 0 ? 8 : 2);`
  - `CPersonalShopDlg::BuyItem(this, a2: *(this + 108));`
  - `v14 = *CMiniRoomBaseDlg::GetUserID(this, a2: &v19, a3: 0);`
  - `LOBYTE(v20) = 1;`
  - `RemoteUserByName = CUserPool::FindRemoteUserByName(this: dword_BEBFA8, sName: v14);`
  - `LOBYTE(v20) = 0;`
  - `if ( v19 != 0 )`
  - `if ( RemoteUserByName != 0 )`
  - `CWvsContext::SendCharacterInfoRequest(this: *dword_BE7918, a2: *(RemoteUserByName + 4520), a3: 0, a4: 0);`
  - `if ( *(this + 104) == 0 )`
  - `v6 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 148);`
  - …（截断 16 行）

#### CPersonalShopDlg::OnKey  (1219 B)

  - `void __thiscall CPersonalShopDlg::OnKey(CPersonalShopDlg *this, unsigned int a2, signed int a3)`
  - `if ( a3 >= 0 && a2 == 13 )`
  - `v4 = *(this + 145);`
  - `if ( v4 != 0 )`
  - `v5 = *CCtrlEdit::GetText(this: v4, &a2);`
  - `if ( !v6 )`
  - `CCtrlEdit::GetText(this: *(this + 145), a2: &a3);`
  - `v7 = *(this + 145);`
  - `CCtrlEdit::SetText(this: v7, a2: 0);`
  - `ZXString<char>::operator=(a1: &a3);`
  - `CMiniRoomBaseDlg::CheckAndSendChat(this: this - 1, a2: v8[0]);`
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: *(this + 145) != 0 ? (*(this + 145) + 4) : 0);`
  - `CDialog::OnKey(this, a2, a3);`

#### CPersonalShopDlg::OnMouseButton  (10402 B)

- SP_ 节点：`SP_1847_DRAGSTART`
  - `void __thiscall CPersonalShopDlg::OnMouseButton(`
  - `v51 = (this - 4);`
  - `ItemIndexFromPoint = CPersonalShopDlg::GetItemIndexFromPoint(this: (this - 4), a2: a4, a3: a5);`
  - `if ( ItemIndexFromPoint >= 0 )`
  - `v7 = *(20 * ItemIndexFromPoint + *(v5 + 100) + 16) == 0;`
  - `if ( !v7 )`
  - `if ( a2 == 513 )`
  - `if ( *(v5 + 98) != 0 && ((*(*v51 + 92))(a1: v51) == 0 || *(v5 + 49) != 0) )`
  - `*(v5 + 107) = ItemIndexFromPoint;`
  - `CWnd::InvalidateRect(this: v8, a2: 0);`
  - `else if ( CWvsContext::CanSendExclRequest(a1: 500, a2: 0) != 0 )`
  - `v9 = *(v5 + 100) + 20 * ItemIndexFromPoint;`
  - `if ( *(v9 + 16) != 0 )`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `LOBYTE(v52) = 1;`
  - …（截断 16 行）

#### CPersonalShopDlg::OnMouseMove  (887 B)

  - `int __thiscall CPersonalShopDlg::OnMouseMove(char **this, int a2, int a3)`
  - `ItemIndexFromPoint = CPersonalShopDlg::GetItemIndexFromPoint(this: (this - 1), a2, a3);`
  - `if ( ItemIndexFromPoint >= 0`
  - `&& (v5 = &(*(this + 100))[20 * ItemIndexFromPoint]) != 0`
  - `&& (*v5 != 0 || v5[4] != 0) )`
  - `v8 = ((*(*this + 12))(a1: this) + a3 + 20);`
  - `v6 = (*(*this + 11))(a1: this);`
  - `CUIToolTip::ShowItemToolTip(this: this + 148, a2: (a2 + v6), a3: v8, a4: v9, a5: 0, a6: 0, a7: 0, a8: 0, a9: 0);`
  - `CUIToolTip::ClearToolTip(this: (this + 148));`

#### CPersonalShopDlg::PutItem  (6641 B)

- SP_ 节点：`SP_3483_YOU_MAY_ONLY_SELL_ONE_OF_THIS_ITEM`, `SP_421_YOU_CANT_SELL_IT_ANYMORE`, `SP_4363_THE_TOTAL_PRICE_FOR_THE_ITEMS_EXCEEDED_R_NTHE_MAXIMUM_AMOUNT_OF_MESOS_THAT_CAN_B`
  - `int __fastcall CPersonalShopDlg::PutItem(_DWORD *a1, int a2, char a3, int a4, int *a5, __int16 a6)`
  - `if ( !v8 )`
  - `if ( a1[100] == a1[109] || (*(*a1 + 92))(a1) == 0 && a1[99] != 0 )`
  - `HIDWORD(v27) = a1 + 29;`
  - `LODWORD(v27) = 0;`
  - `CUtilDlg::Notice(`
  - `ppDialog: SHIDWORD(v27),`
  - `bIsTightLine: SHIDWORD(v28));`
  - `if ( CWvsContext::CanSendExclRequest(a1: 500, a2: 0) == 0 )`
  - `sub_4288C1(a1: &a3);`
  - `if ( (*(*a1 + 92))(a1) != 0 )`
  - `Data = TSecType<long>::GetData(a1: (a4 + 12), a2: 0);`
  - `if ( CItemInfo::IsOnlyItem(a1: Data) != 0 )`
  - `if ( !v10 )`
  - `while ( 1 )`
  - `v11 = *(v7 + a1[101] + 16);`
  - …（截断 16 行）

### CShopDlg（9 函数）


#### CShopDlg::Draw  (1541 B)

  - `const struct tagRECT *__thiscall CShopDlg::Draw(CShopDlg *this, struct tagRECT *a2)`
  - `CWnd::Draw(this, a2);`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: &v9) + 4);`
  - `if ( v10 != 0 )`
  - `sub_428C44(this: &v9, a2: 0);`
  - `CWnd::GetCanvas(a1: &a2);`
  - `if ( a2 != 0 )`
  - `(*(a2->left + 4))(a1: a2);`
  - `CShopDlg::DrawBuyItem(this, a2: v8[0]);`
  - `CShopDlg::DrawSellItem(this, arg0: v8[0]);`
  - `v4 = _ZtlSecureFuse<long>(a1: v3 + 165, a2: *(v3 + 173));`
  - `sub_4281DD(a1: &a2);`
  - `CShopDlg::DrawMoney(a1: v7, a2: v8[0]);`
  - `return (*(a2->left + 8))(a1: a2);`

#### CShopDlg::DrawBuyItem  (20531 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_5639_MESO`
  - `IWzCanvas *__thiscall CShopDlg::DrawBuyItem(_DWORD *this, IWzCanvas *a2)`
  - `v3 = *(*(this + 39) + 60);`
  - `if ( v3 == 0 )`
  - `CCtrlScrollBar::SetCurPos(this: v6, a2: v5);`
  - `if ( *v105 == 0 )`
  - `if ( v108 >= *(v8 - 4) )`
  - `v9 = *(v7 + v8 + 24) == 0;`
  - `v10 = (v7 + v8);`
  - `v110 = (v7 + v8);`
  - `if ( !v9 && *(v10 + 7) != 0 )`
  - `if ( v2[60] == v108 )`
  - `sub_402FAB(a1: 255, a2: 3);`
  - `LOBYTE(v117) = 1;`
  - `if ( a2 == 0 )`
  - `v12 = (*(v11 + 128))(`
  - `if ( v12 < 0 )`
  - …（截断 16 行）

#### CShopDlg::DrawMoney  (3387 B)

  - `VARIANTARG *__stdcall CShopDlg::DrawMoney(VARIANTARG *a1, VARIANTARG *a2)`
  - `format_integer(a1: &v24, a2, a3: 1);`
  - `VariantInit(&pvarg);`
  - `v2 = ZtlTaskMemFreeImp(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `if ( v2 < 0 )`
  - `LOBYTE(v25) = 2;`
  - `v3 = sub_428226(a1: &v24);`
  - `_bstr_t::_bstr_t(a1: v3);`
  - `LOBYTE(v25) = 3;`
  - `basic_font = get_basic_font(a1: &a2, fType: 24);`
  - `LOBYTE(v25) = 5;`
  - `sub_4284A8(a1: basic_font);`
  - `v6 = 447 - sub_42782E(a1: v19, a2: p_pvarg);`
  - `if ( a2 != 0 )`
  - `(*(*&a2->vt + 8))(a1: a2);`
  - `LOBYTE(v25) = 1;`
  - …（截断 16 行）

#### CShopDlg::DrawSellItem  (14370 B)

  - `_DWORD *__thiscall CShopDlg::DrawSellItem(_DWORD *this, _DWORD *arg0)`
  - `v3 = *(*(this + 46) + 56);`
  - `if ( v5 == 0 || (v6 = v3 + v80 - 2000) >= *(v5 - 4) )`
  - `v7 = *(v2[42] + v82 + 4);`
  - `if ( v7 != 0 )`
  - `(*(*v7 + 12))(a1: v7);`
  - `LOBYTE(v85) = 1;`
  - `if ( v10 )`
  - `sub_402FAB(a1: 255, a2: 3);`
  - `LOBYTE(v85) = 2;`
  - `if ( arg0 == 0 )`
  - `v12 = (*(v11 + 128))(`
  - `if ( v12 < 0 )`
  - `v5 = HIDWORD(v61[0]);`
  - `if ( v13 < 0 )`
  - `*&v61[0] = __PAIR64__(*(v9 + 64), v5);`
  - …（截断 16 行）

#### CShopDlg::OnCreate  (27405 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1253_UI_BASICIMG_ITEMNO`, `SP_1542_UI_UIWINDOWIMG_SHOP_SELECT`, `SP_1543_UI_UIWINDOWIMG_SHOP_MESO`, `SP_1544_UI_UIWINDOWIMG_SHOP_TOKEN`, `SP_1547_UI_UIWINDOWIMG_SHOP_BTBUY`, `SP_1548_UI_UIWINDOWIMG_SHOP_BTSELL`, `SP_2615_UI_UIWINDOWIMG_PERSONALSHOP_BTEXIT`, `SP_2676_UI_UIWINDOWIMG_SHOP_TABSELL_ENABLED`, `SP_2677_UI_UIWINDOWIMG_SHOP_TABSELL_DISABLED`, `SP_2678_UI_UIWINDOWIMG_SHOP_TABBUY_ENABLED`, `SP_2679_UI_UIWINDOWIMG_SHOP_TABBUY_DISABLED`
  - `void __thiscall CShopDlg::OnCreate(CShopDlg *this, void *a2)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &pvarg, pvargSrc: &pvargSrc);`
  - `LOBYTE(v174) = 1;`
  - `v5 = sub_406472();`
  - `v6 = sub_402680(a1: v5);`
  - `IWzResMan::GetObjectA(this: v6, pvargDest: &v161, sUOL: v148, vParam: p_pvarg, vAux: p_pvargDest);`
  - `LOBYTE(v174) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: p_Unknown, a2: v152);`
  - `v7 = sub_41E527(a1: &Unknown);`
  - `if ( v7 < 0 && v7 != -2147467262 )`
  - `if ( v8 < 0 )`
  - `LOBYTE(v174) = 0;`
  - `if ( v9 < 0 )`
  - `if ( v10 < 0 )`
  - `LOBYTE(v174) = 5;`
  - …（截断 16 行）

#### CShopDlg::OnPacket  (8573 B)

- 分支 case：`0`, `1`, `5`, `9`, `2`, `10`, `3`, `4`, `8`, `13`, `14`, `15`, `16`, `17`
- SP_ 节点：`SP_3977_PLAYERS_THAT_ARE_LEVEL_15_AND_BELOW__R_NMAY_ONLY_TRADE_1_MILLION_MESOS_PER_DAY__`, `SP_5434_YOU_NEED_MORE_ITEMS`, `SP_5449_YOU_MUST_BE_OVER_LVD_TO_PURCHASE_THIS_ITEM`, `SP_5450_YOU_MUST_BE_UNDER_LVD_TO_PURCHASE_THIS_ITEM`, `SP_5599_YOU_DO_NOT_HAVE_ENOUGH_MESOS`, `SP_852_YOU_DONT_HAVE_ENOUGH_IN_STOCK`, `SP_853_PLEASE_CHECK_IF_YOUR_INVENTORY_IS_FULL_OR_NOT`, `SP_855_DUE_TO_AN_ERROR_THE_TRADE_DID_NOT_HAPPEN`
  - `void __cdecl CShopDlg::OnPacket(signed int nType, int a2)`
  - `if ( nType == 305 )`
  - `if ( dword_BE7910 != 0 )`
  - `_CxxThrowException(a1: &v37, a2: &_TI3_AVCDisconnectException__);`
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1584) != 0 )`
  - `v29 = CShopDlg::CShopDlg();`
  - `CShopDlg::SetShopDlg(this: v29, a2);`
  - `else if ( nType == 306 )`
  - `? ((*(*(dword_BE7910 + 4) + 72))(a1: dword_BE7910 + 4, a2: &dword_BEDE00) != 0`
  - `if ( v2 != 0 )`
  - `if ( *(v2 + 252) == 0 )`
  - `_CxxThrowException(a1: &nType, a2: &_TI3_AVCDisconnectException__);`
  - `*(v2 + 252) = 0;`
  - `switch ( CInPacket::Decode1(this: v4) )`
  - `v6 = *(v2 + 248);`
  - `if ( v6 > 0 && v6 <= 5 )`
  - …（截断 16 行）

#### CShopDlg::ResetInfo  (243 B)

  - `void __thiscall CShopDlg::ResetInfo(CWnd *this)`
  - `CShopDlg::SetSellItems(this, a2: 0, a3: this + 212);`
  - `sub_7546F8(a1: this);`
  - `sub_754779(a1: this);`
  - `CWnd::InvalidateRect(this, a2: 0);`

#### CShopDlg::SetSellItems  (5890 B)

  - `int __thiscall CShopDlg::SetSellItems(_DWORD *this, signed int a2, int a3)`
  - `if ( a2 == 0 )`
  - `a2 = *(*(this + 41) + 60) + 1;`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v30) + 4);`
  - `sub_4289B7(this: v30);`
  - `sub_757716(a1: a3);`
  - `v37 = (v3 + 4 * a2 + 1095);`
  - `while ( 1 )`
  - `if ( *v37 != 0 )`
  - `v4 = *(v4 - 4);`
  - `if ( v42 > result )`
  - `v6 = *(CharacterData::GetItem(this: v3, a2: v29, a3: a2, a4: v42) + 4);`
  - `sub_4288C1(a1: v29);`
  - `if ( v6 != 0 )`
  - `Data = TSecType<long>::GetData(a1: (v6 + 12), a2: v6);`
  - `if ( CItemInfo::IsQuestItem(this: v7, a2: Data) == 0 )`
  - …（截断 16 行）

#### CShopDlg::SetShopDlg  (15358 B)

- SP_ 节点：`SP_2678_UI_UIWINDOWIMG_SHOP_TABBUY_ENABLED`, `SP_2679_UI_UIWINDOWIMG_SHOP_TABBUY_DISABLED`
  - `void __thiscall CShopDlg::SetShopDlg(CWnd *this, int a2)`
  - `*(this + 47) = CInPacket::Decode4(this: a2);`
  - `v4 = CInPacket::Decode2(this: v2);`
  - `if ( v4 != 0 )`
  - `while ( 1 )`
  - `sub_753146(a1: v88);`
  - `v5 = CInPacket::Decode4(this: v2);`
  - `TSecType<long>::SetData(a1: v5);`
  - `v89 = CInPacket::Decode4(this: v2);`
  - `v90 = CInPacket::Decode4(this: v2);`
  - `v91 = CInPacket::Decode4(this: v2);`
  - `v6 = CInPacket::Decode4(this: v2);`
  - `v7 = TSecType<long>::GetData(a1: v88, a2: v2) / 10000;`
  - `if ( v7 == 207 || v7 == 233 )`
  - `CInPacket::DecodeBuffer(a1: &v93, a2: 8u);`
  - `v96 = CInPacket::Decode2(this: v2);`
  - …（截断 16 行）

### CAdminShopDlg（14 函数）


#### CAdminShopDlg::GetCurNPCItem  (239 B)

  - `_DWORD *__thiscall CAdminShopDlg::GetCurNPCItem(_DWORD *this)`
  - `if ( *(*(this + 39) + 60) == 0 )`

#### CAdminShopDlg::GetItemIndexFromPoint  (2373 B)

  - `signed int __thiscall CAdminShopDlg::GetItemIndexFromPoint(_DWORD *this, int a2, int a3, _DWORD *a4, unsigned int *a5)`
  - `v22 = *(*(this + 43) + 56);`
  - `v6 = *CAdminShopDlg::GetCurNPCItem(this);`
  - `if ( v6 == 0 || v22 >= *(v6 - 4) )`
  - `if ( *(this + 57) == 0 )`
  - `v9 = sub_40B947(a1: *v7);`
  - `v10 = sub_40B920(a1: v8);`
  - `dword_BF040C(a1: v20, a2: 6, a3: v23, a4: v10 + 6, a5: v18);`
  - `if ( dword_BF0484(a1: v20, a2, a3) != 0 )`
  - `while ( v23 < 327 );`
  - `v22 = *(*(this + 45) + 56);`
  - `v13 = *(this + 52);`
  - `if ( v13 == 0 || v22 >= *(v13 - 4) )`
  - `if ( v21 == 0 )`
  - `v16 = sub_40B947(a1: v21);`
  - `v17 = sub_40B920(a1: v15);`
  - …（截断 16 行）

#### CAdminShopDlg::OnButtonClicked  (2408 B)

- SP_ 节点：`SP_4646_ISNT_THERE_ANYTHING_ELSE_YOU_WANT_ILL_MAKE_A_NOTE_OF_IT`
  - `void *__thiscall CAdminShopDlg::OnButtonClicked(int *this, unsigned int a2)`
  - `if ( a2 == 2 )`
  - `if ( *(this + 390) == 0 )`
  - `return (*(v10 + 52))(a1: this, a2: v14);`
  - `v4 = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1768);`
  - `if ( v4 != 0 )`
  - `v5 = CUtilDlgEx::CUtilDlgEx(this: v4);`
  - `v16 = v6 != 0 ? (v6 - 8) : 0;`
  - `sub_4289CD(a1: v15);`
  - `v11 = *(this + 46);`
  - `LOBYTE(v18) = 1;`
  - `v9 = sub_428688(a1: v15);`
  - `CUtilDlgEx::SetUtilDlgEx(this: v9, a2: 1, a3: v11, a4: v12, a5: v13, a6: v14);`
  - `CUtilDlgEx::SetUtilDlgEx_YESNO(a1: v16);`
  - `CUtilDlgEx::CreateUtilDlgEx(this: v16);`
  - `if ( CDialog::DoModal(a1: v16) == 6 )`
  - …（截断 16 行）

#### CAdminShopDlg::OnCreate  (26127 B)

- SP_ 节点：`SP_1253_UI_BASICIMG_ITEMNO`, `SP_1542_UI_UIWINDOWIMG_SHOP_SELECT`, `SP_1547_UI_UIWINDOWIMG_SHOP_BTBUY`, `SP_1548_UI_UIWINDOWIMG_SHOP_BTSELL`, `SP_2676_UI_UIWINDOWIMG_SHOP_TABSELL_ENABLED`, `SP_2677_UI_UIWINDOWIMG_SHOP_TABSELL_DISABLED`, `SP_2678_UI_UIWINDOWIMG_SHOP_TABBUY_ENABLED`, `SP_2679_UI_UIWINDOWIMG_SHOP_TABBUY_DISABLED`
  - `void __thiscall CAdminShopDlg::OnCreate(CAdminShopDlg *this, void *a2)`
  - `sub_402F85(pvargDest: &v163, pvargSrc: &pvargSrc);`
  - `sub_402F85(pvargDest: &v162, pvargSrc: &pvargSrc);`
  - `LOBYTE(v177) = 1;`
  - `v5 = sub_406472();`
  - `v6 = sub_402680(a1: v5);`
  - `IWzResMan::GetObjectA(this: v6, pvargDest: &v161, sUOL: v153, vParam: v154, vAux: v155);`
  - `LOBYTE(v177) = 3;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: p_Unknown, a2: v157);`
  - `v7 = sub_41E527(a1: &Unknown);`
  - `if ( v7 < 0 && v7 != -2147467262 )`
  - `if ( v8 < 0 )`
  - `LOBYTE(v177) = 0;`
  - `if ( v9 < 0 )`
  - `if ( v10 < 0 )`
  - `LOBYTE(v177) = 5;`
  - …（截断 16 行）

#### CAdminShopDlg::OnMouseButton  (1339 B)

  - `void __thiscall CAdminShopDlg::OnMouseButton(_DWORD *this, int a2, int a3, int a4, int a5)`
  - `if ( a2 != 513 )`
  - `if ( a2 != 515 )`
  - `if ( CAdminShopDlg::GetItemIndexFromPoint(this: this - 1, a2: a4, a3: a5, a4: &a5, a5: &a2) == 0 )`
  - `if ( a5 != 0 )`
  - `if ( *(this + 384) != 0 || a2 != *(this + 385) )`
  - `else if ( *(this + 384) != 1 || a2 != *(this + 386) )`
  - `CAdminShopDlg::SendTradeRequest(a1: this - 1);`
  - `if ( CAdminShopDlg::GetItemIndexFromPoint(this: this - 1, a2: a4, a3: a5, a4: &a2, &a5) == 0 )`
  - `*(this + 384) = -1;`
  - `*(this + 384) = a2 == 0;`
  - `if ( v6 )`
  - `*(this + 385) = v7;`
  - `*(this + 386) = v8;`
  - `CWnd::InvalidateRect(this: (this - 1), a2: 0);`

#### CAdminShopDlg::OnMouseMove  (3269 B)

  - `signed int __thiscall CAdminShopDlg::OnMouseMove(char **this, int a2, int a3)`
  - `if ( CAdminShopDlg::GetItemIndexFromPoint(this: this - 1, a2, a3, a4: &v25, a5: &v24) == 0 )`
  - `CUIToolTip::ClearToolTip(this: (this + 57));`
  - `if ( v25 != 0 )`
  - `if ( *(v4[39] + 15) == 0 )`
  - `v22 = *(*v6 + 8 * v24 + 4);`
  - `sub_428A28(a1: v21);`
  - `if ( TSecType<long>::GetData(a1: (v22 + 24), a2: 0) / 1000000 == 1 )`
  - `v7 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v18) + 4);`
  - `sub_4289B7(this: v18);`
  - `Data = TSecType<long>::GetData(a1: (v22 + 24), a2: 0);`
  - `get_bodypart_from_item(a1: Data, a2: *(v7 + 17), a3: &v26, a4: 0);`
  - `CharacterData::GetItem(this: v7, a2: v19, a3: 1, a4: -v26);`
  - `LOBYTE(v27) = 1;`
  - `if ( v20 != 0 )`
  - `if ( v26 != 5`
  - …（截断 16 行）

#### CAdminShopDlg::OnPacket  (12049 B)

- 分支 case：`1`, `2`, `3`, `4`, `5`, `6`, `7`, `8`, `9`, `10`
- SP_ 节点：`SP_4647_IM_NOT_DOING_BUSINESS_AT_THIS_TIME_DONT_I_DESERVE_SOME_REST_TOO`, `SP_4648_THATS_NOT_SOMETHING_I_BARTER_ILL_SHOW_YOU_THE_LIST_AGAIN_FOR_YOU_TO_CHOOSE_FROM`, `SP_4649_ARE_YOU_BEGGING`, `SP_4650_WHY_DO_YOU_CARRY_AROUND_SO_MUCH_MESOS_COME_BACK_AFTER_EMPTYING_YOUR_POCKETS`, `SP_4651_TAKING_YOUR_PRECIOUS_TIME_HUH_I_DONT_BARTER_THIS_ITEM_ANYMORE`, `SP_4652_IM_DONE_BARTERING_THIS_ITEM_FOR_THE_DAY_COME_BACK_TOMORROW`, `SP_4653_WHY_ARE_YOU_OBSESSING_OVER_THIS_ITEM_I_WONT_BARTER_ANYMORE`, `SP_4654_YOURE_GREEDY_HOW_ABOUT_YOU_LOWER_THE_QUANTITY`, `SP_4655_IM_A_LITTLE_PREOCCUPIED_RIGHT_NOW_SO_COME_BACK_LATER`
  - `void __cdecl CAdminShopDlg::OnPacket(int a1, int arg4)`
  - `if ( a1 == 307 )`
  - `if ( dword_BE7910 != 0 )`
  - `v12 = (*(*(dword_BE7910 + 4) + 72))(a1: dword_BE7910 + 4, a2: &dword_BE790C) != 0 ? dword_BE7910 : 0;`
  - `if ( v12 != 0 )`
  - `if ( *(v12 + 1556) == 0 )`
  - `_CxxThrowException(a1: &v61, a2: &_TI3_AVCDisconnectException__);`
  - `*(v12 + 1556) = 0;`
  - `if ( CInPacket::Decode1(this: v13) == 4 )`
  - `v14 = CInPacket::Decode1(this: arg4);`
  - `if ( v14 == 0 )`
  - `v16 = *(v12 + 1552);`
  - `if ( v16 > 0 && v16 <= 5 )`
  - `v64 = *(*(v12 + 180) + 56);`
  - `v17 = sub_428068(a1: v12);`
  - `if ( v17 < v64 )`
  - …（截断 16 行）

#### CAdminShopDlg::ResetInfo  (285 B)

  - `void __thiscall CAdminShopDlg::ResetInfo(CAdminShopDlg *this)`
  - `CAdminShopDlg::SetUserItems(this, a2: 0, a3: this + 208);`
  - `CAdminShopDlg::SetScrollBar(this);`
  - `CAdminShopDlg::SetAvatar(this);`
  - `CWnd::InvalidateRect(this, a2: 0);`

#### CAdminShopDlg::SendTradeRequest  (12225 B)

- SP_ 节点：`SP_3416_SEALED_ITEMS_CANNOT_BE_R_NSOLD_TRADED_OR_DROPPED`, `SP_4656_YOU_DO_NOT_HAVE_THE_ITEM`, `SP_5599_YOU_DO_NOT_HAVE_ENOUGH_MESOS`, `SP_848_ARE_YOU_SURE_YOU_WANT_TO_BUY_IT`, `SP_849_HOW_MANY_ARE_YOU_WILLING_TO_BUY`, `SP_850_ARE_YOU_SURE_YOU_WANT_TO_SELL_IT`, `SP_851_HOW_MANY_ARE_YOU_WILLING_TO_SELL`
  - `void *__thiscall CAdminShopDlg::SendTradeRequest(CAdminShopDlg *this)`
  - `if ( *(this + 389) == 0 )`
  - `v3 = *(this + 385);`
  - `if ( v3 != 0 )`
  - `if ( v3 != 1 )`
  - `v4 = *(this + 387);`
  - `if ( v4 < 0 )`
  - `v10 = *(this + 52);`
  - `v11 = v10 != 0 ? *(v10 - 4) : 0;`
  - `if ( v4 >= v11 )`
  - `v4 = *(this + 386);`
  - `v5 = *(this + 39);`
  - `if ( *(v5 + 60) == 0 )`
  - `v8 = v7 != 0 ? *(v7 - 4) : 0;`
  - `if ( v4 >= v8 )`
  - `sub_428A28(a1: v12);`
  - …（截断 16 行）

#### CAdminShopDlg::SetAdminShopDlg  (7731 B)

- SP_ 节点：`SP_956_CANVAS`
  - `void __thiscall CAdminShopDlg::SetAdminShopDlg(CAdminShopDlg *this, struct CInPacket *a2, unsigned int a3, int a4)`
  - `*(this + 385) = -1;`
  - `*(this + 387) = -1;`
  - `*(this + 386) = -1;`
  - `*(this + 46) = a3;`
  - `*(this + 389) = 0;`
  - `sub_428DF8(a1: this + 200);`
  - `sub_428DF8(a1: v4 + 204);`
  - `sub_428DF8(a1: v4 + 208);`
  - `sub_428DF8(a1: v4 + 212);`
  - `if ( a4 > 0 )`
  - `sub_428A00(a1: v41);`
  - `v5 = CInPacket::Decode4(this: a2);`
  - `TSecType<long>::SetData(a1: v5);`
  - `v6 = CInPacket::Decode4(this: a2);`
  - `TSecType<long>::SetData(a1: v6);`
  - …（截断 16 行）

#### CAdminShopDlg::SetAvatar  (2315 B)

  - `_DWORD *__thiscall CAdminShopDlg::SetAvatar(CAdminShopDlg *this)`
  - `ZRef<CAvatar>::_Alloc(a1: this + 188);`
  - `CWnd::GetLayer(a1: &v13);`
  - `Layer = CWnd::GetLayer(a1: &v22);`
  - `LOBYTE(v25) = 1;`
  - `sub_428EC4(a1: Layer);`
  - `LOBYTE(v25) = 2;`
  - `v5 = sub_42861D();`
  - `CharacterData = CWvsContext::GetCharacterData(this: v5, a2: &v23);`
  - `LOBYTE(v25) = 3;`
  - `v7 = ZRef<CharacterData>::~ZRef<CharacterData>(a1: CharacterData);`
  - `v8 = AvatarLook::AvatarLook(a1: v7);`
  - `LOBYTE(v25) = 7;`
  - `v11 = sub_426626(a1: 2, a2: 1);`
  - `v9 = sub_4285F2(a1: v1);`
  - `CAvatar::Init(`
  - …（截断 16 行）

#### CAdminShopDlg::SetNPC  (9851 B)

  - `int __thiscall CAdminShopDlg::SetNPC(char *this)`
  - `sub_402F85(&pvargDest, pvargSrc: &pvargSrc);`
  - `sub_402FAB(a1: 0, a2: 3);`
  - `LOBYTE(v57) = 1;`
  - `if ( dword_BF14EC == 0 )`
  - `v2 = IWzGr2D::CreateLayer(`
  - `v3 = (this + 196);`
  - `sub_428712(a1: *v2);`
  - `sub_4156C5(a1: &v55);`
  - `LOBYTE(v57) = 0;`
  - `if ( v4 < 0 )`
  - `if ( v5 < 0 )`
  - `v6 = *CWnd::GetLayer(a1: &v54);`
  - `sub_410FDF(this: &pvargDest, a2: v6, a3: 1);`
  - `LOBYTE(v57) = 3;`
  - `if ( v7 )`
  - …（截断 16 行）

#### CAdminShopDlg::SetScrollBar  (757 B)

  - `void __thiscall CAdminShopDlg::SetScrollBar(_DWORD *this)`
  - `v2 = *CAdminShopDlg::GetCurNPCItem(this);`
  - `if ( v2 != 0 && *(v2 - 4) > 5u )`
  - `v3 = *CAdminShopDlg::GetCurNPCItem(this);`
  - `if ( v3 != 0 )`
  - `v3 = *(v3 - 4);`
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 43), a2: v4);`
  - `v5 = *(this + 52);`
  - `if ( v5 != 0 && *(v5 - 4) > 5u )`
  - `v6 = *(v5 - 4) - 4;`
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 45), a2: v6);`

#### CAdminShopDlg::SetUserItems  (3942 B)

  - `int __thiscall CAdminShopDlg::SetUserItems(_DWORD *this, signed int a2, int a3)`
  - `if ( a2 == 0 )`
  - `a2 = *(*(this + 41) + 60) + 1;`
  - `v3 = *(CWvsContext::GetCharacterData(this: *dword_BE7918, a2: v18) + 4);`
  - `sub_4289B7(this: v18);`
  - `sub_428DF8(a1: a3);`
  - `v24 = (v3 + 4 * a2 + 1095);`
  - `while ( 1 )`
  - `if ( *v24 != 0 )`
  - `v4 = *(v4 - 4);`
  - `if ( v26 > result )`
  - `v6 = *(CharacterData::GetItem(this: v3, a2: v17, a3: a2, a4: v26) + 4);`
  - `sub_4288C1(a1: v17);`
  - `if ( v6 != 0 )`
  - `Data = TSecType<long>::GetData(a1: (v6 + 12), a2: dword_BE78D8);`
  - `if ( CItemInfo::IsQuestItem(this: v7, a2: Data) == 0 )`
  - …（截断 16 行）

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

### CEmployeePool（1 函数）


#### CEmployeePool::FindBalloon  (747 B)

  - `int __thiscall CEmployeePool::FindBalloon(CEmployeePool *this, const struct tagPOINT *a2)`
  - `v2 = *(this + 10);`
  - `if ( v2 == 0 )`
  - `while ( 1 )`
  - `v4 = *(v2 + 16);`
  - `v2 = *(v2 + 4);`
  - `if ( *(v4 + 144) != 0 )`
  - `CChatBalloon::GetMiniRoomBalloonRect(this: (v4 + 20), a2: &v6);`
  - `if ( dword_BF0484(a1: &v6, a2: a2->x, a3: a2->y) != 0 )`
  - `return *(v3 + 16);`

### CEmployee（1 函数）


#### CEmployee::GetNameTag  (176 B)

  - `_DWORD *__thiscall CEmployee::GetNameTag(char *this, _DWORD *a2)`
  - `ZXString<char>::operator=(a1: this + 152);`

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

### CUIMapleTV（1 函数）


#### CUIMapleTV::GetText  (3739 B)

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
  - `if ( v11 == a2 - 1 )`
  - `v18[v12] = *(*(v9 + 68) + v12 * 4);`
  - …（截断 16 行）

### CMapleTVMan（3 函数）


#### CMapleTVMan::GetReceivername  (183 B)

  - `_DWORD *__thiscall CMapleTVMan::GetReceivername(char *this, _DWORD *a2)`
  - `ZXString<char>::operator=(a1: this + 928);`

#### CMapleTVMan::GetSenderName  (181 B)

  - `_DWORD *__thiscall CMapleTVMan::GetSenderName(char *this, _DWORD *a2)`
  - `ZXString<char>::operator=(a1: this + 924);`

#### CMapleTVMan::IsLoadFlash  (189 B)

  - `int __thiscall CMapleTVMan::IsLoadFlash(CMapleTVMan *this)`
  - `if ( *(this + 246) == 0 )`
  - `*(this + 246) = 0;`

### CField_Wedding（4 函数）


#### CField_Wedding::OnPacket  (356 B)

  - `void __thiscall CField_Wedding::OnPacket(char *this, signed int a2, wchar_t *a3)`
  - `if ( a2 == 320 )`
  - `CField_Wedding::OnWeddingProgress(a1: a3);`
  - `else if ( a2 == 321 )`
  - `CField_Wedding::OnWeddingCeremonyEnd(a1: a3);`
  - `CField::OnPacket(this, a2, a3);`

#### CField_Wedding::OnWeddingCeremonyEnd  (366 B)

  - `void __thiscall CField_Wedding::OnWeddingCeremonyEnd(CField_Wedding *this, struct CInPacket *a2)`
  - `v3 = *(this + 510);`
  - `if ( v3 != 0 )`
  - `(*(*v3 + 32))(a1: v3, a2: 8193);`
  - `CField_Wedding::SetBlessEffect(this, a2: 1);`

#### CField_Wedding::OnWeddingProgress  (12227 B)

- SP_ 节点：`SP_1666_STRING_NPCIMG`, `SP_4202_SOUND_BGMGLIMG_CHAPEL`, `SP_4203_SOUND_BGMGLIMG_CATHEDRAL`, `SP_4204_DO_YOU_WANT_TO_BLESS_THIS_COUPLE`
  - `int __thiscall CField_Wedding::OnWeddingProgress(CField_Wedding *this, VARIANTARG **a2)`
  - `v62 = *(*dword_BE7918 + 8352);`
  - `if ( TSecType<unsigned long>::GetData(a1: this + 312) == 680000110 )`
  - `result = TSecType<unsigned long>::GetData(a1: v3);`
  - `if ( result != 680000210 )`
  - `v5 = CInPacket::Decode1(this: a2);`
  - `v6 = CInPacket::Decode4(this: a2);`
  - `*(this + 511) = v6;`
  - `*(this + 512) = CInPacket::Decode4(this: v7);`
  - `if ( *(this + 510) != 0 )`
  - `if ( TSecType<unsigned long>::GetData(a1: this + 312) != 680000210`
  - `|| *(this + 508) != 2`
  - `|| v62 == *(this + 511)`
  - `|| v62 == *(this + 512) )`
  - `v8 = **(this + 510);`
  - `(*(v8 + 32))(a1: v52);`
  - …（截断 16 行）

#### CField_Wedding::SetBlessEffect  (7753 B)

- SP_ 节点：`SP_4205_EFFECT_BASICEFFIMG_WEDDING`, `SP_958_SHAPE2DVECTOR2D`
  - `int __thiscall CField_Wedding::SetBlessEffect(CField_Wedding *this, int a2)`
  - `if ( a2 == 0 )`
  - `return sub_581FAA(a1: 0);`
  - `User = CUserPool::GetUser(this: dword_BEBFA8, a2: *(this + 511));`
  - `v4 = CUserPool::GetUser(this: dword_BEBFA8, a2: *(this + 512));`
  - `if ( User == 0 || v4 == 0 )`
  - `return sub_428712(a1: 0);`
  - `v5 = (*(User[1] + 16))(a1: User + 1);`
  - `pvargDest.cyVal.int64 = *(*(v50[1] + 16))(a1: v50 + 1);`
  - `LOBYTE(v53) = 1;`
  - `PcCreateObject::IWzVector2D(a1: v8, &a2, a3: 0);`
  - `LOBYTE(v53) = 0;`
  - `if ( v49 != 0 )`
  - `LOBYTE(v53) = 2;`
  - `LOBYTE(v53) = 3;`
  - `PcCreateObject::IWzVector2D(a1: v10, a2: &v51, a3: 0);`
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

### CUIQuestDelivery（1 函数）


#### CUIQuestDelivery::OnChildNotify  (375 B)

  - `void __thiscall CUIQuestDelivery::OnChildNotify(`
  - `if ( a2 == 2000 && a3 >= 0x12C && a3 <= 0x130 )`
  - `CWnd::InvalidateRect(this, a2: 0);`
  - `else if ( a3 == 100 )`
  - `(*(*this + 32))(a1: this, a2);`

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

### CCashShop（10 函数）


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

## 3. 坐标 / 尺寸 / 按钮 id / 封包字段候选行（核心窗口全函数）


### CAdminShopDlg::OnCreate
  - `memset(a1: this + 1532, a2: 0, a3: v134);`

### CAdminShopDlg::OnMouseButton
  - `if ( CAdminShopDlg::GetItemIndexFromPoint(this: this - 1, a2: a4, a3: a5, a4: &a5, a5: &a2) == 0 )`
  - `if ( CAdminShopDlg::GetItemIndexFromPoint(this: this - 1, a2: a4, a3: a5, a4: &a2, &a5) == 0 )`
  - `CWnd::InvalidateRect(this: (this - 1), a2: 0);`

### CAdminShopDlg::OnMouseMove
  - `if ( CAdminShopDlg::GetItemIndexFromPoint(this: this - 1, a2, a3, a4: &v25, a5: &v24) == 0 )`
  - `if ( TSecType<long>::GetData(a1: (v22 + 24), a2: 0) / 1000000 == 1 )`
  - `Data = TSecType<long>::GetData(a1: (v22 + 24), a2: 0);`
  - `get_bodypart_from_item(a1: Data, a2: *(v7 + 17), a3: &v26, a4: 0);`
  - `|| (v23 = TSecType<long>::GetData(a1: (v22 + 24), a2: 0),`
  - `((v10 = TSecType<long>::GetData(a1: v9 + 12, a2: 0)) / 10000 == 105) == (v23 / 10000 == 105)) )`
  - `CUIToolTip::SetToolTip_Equip2(this: (this + 57), a2: (a2 + v11), a3: v15, a4: v16, a5: v17, a6: 0, a7: 0);`

### CAdminShopDlg::OnPacket
  - `CCtrlScrollBar::SetCurPos(this: *(v12 + 180), a2: v18 - 4);`
  - `CUtilDlgEx::SetUtilDlgEx_TEXT(this: v50, a2: 0, a3: 0);`

### CAdminShopDlg::ResetInfo
  - `CAdminShopDlg::SetUserItems(this, a2: 0, a3: this + 208);`

### CAdminShopDlg::SendTradeRequest
  - `Data = TSecType<long>::GetData(a1: (v61 + 24), a2: 0);`
  - `v28 = TSecType<long>::GetData(a1: (v61 + 24), a2: 0);`

### CAdminShopDlg::SetAdminShopDlg
  - `Data = TSecType<long>::GetData(a1: (v42 + 24), a2: 0);`
  - `v13 = TSecType<long>::GetData(a1: (v42 + 24), a2: 0);`
  - `ItemIcon = CItemInfo::GetItemIcon(a1: &v45, a2: v13, bPetDead: 1, bWithShadow: 0);`
  - `PcCreateObject::IWzCanvas(a1: v17, a2: (v15 + 68), a3: 0);`
  - `CAdminShopDlg::SetUserItems(this: v4, a2: 0, a3: v4 + 208);`

### CBlackListDlg::AddBlackList
  - `CUtilDlgEx::SetUtilDlgEx(this: v8, a2: 3, a3: 0, a4: v29, a5: v30, a6: v31);`
  - `CUtilDlgEx::SetUtilDlgEx_INPUT_STR(this: v33, a2: &sStrDefault, a3: 4, a4: 12, a5: 0, a6: 0);`

### CCashShop::IsUsableItemGift
  - `if ( CharacterData::GetActiveItemSlotPet(this: a4, a2: 0) == 0 )`
  - `&& CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: a4, a3: 1004, a4: 0) <= 0`
  - `&& CSkillInfo::GetSkillLevel(this: dword_BE78DC, a2: a4, a3: &loc_989A68 + 4, a4: 0) <= 0 )`

### CCashShop::OnBuyPackage
  - `if ( TSecType<long>::GetData(a1: (v40 + 16), a2: 0) / 10000 != 910 )`
  - `TSecType<long>::GetData(a1: (v40 + 16), a2: 0);`
  - `Data = TSecType<long>::GetData(a1: v42 + 16, a2: 0);`
  - `v12 = TSecType<long>::GetData(a1: v42 + 16, a2: 0);`

### CCashShop::OnGiftMateInfoResult
  - `Data = TSecType<long>::GetData(a1: (v63 + 16), a2: 0);`

### CCashShop::OnQueryCashResult
  - `CWnd::InvalidateRect(this: (this + 7648), a2: 0);`

### CCashShop::OnRemoveWish
  - `if ( *(this + 6) == 0 && a2 != 0 && CCashShop::GetUsableCommodityIndexBySN(this, &a2) >= 0 )`

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

### CChatBalloon::MakeMiniRoomBalloon
  - `a9 = Ztl_variant_t::GetUnknown(a1: v285[8], a2: v285[12]);`

### CChatHelper::TryChat
  - `Buffer = ZXString<char>::GetBuffer(a1: 1024, a2: 1);`

### CField_PartyRaidResult::Init
  - `v32[0] = ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 72);`

### CField_Wedding::OnWeddingProgress
  - `if ( TSecType<unsigned long>::GetData(a1: this + 312) == 680000210 && *(this + 508) == 2 )`
  - `CUtilDlgEx::SetUtilDlgEx_TEXT(this: v69, a2: 0, a3: 1);`
  - `&& (TSecType<unsigned long>::GetData(a1: this + 312) != 680000210 || *(this + 508) != 2) )`

### CFriend::Reset
  - `CInPacket::DecodeBuffer(a1: *(this + 1), a2: 4 * v3);`

### CMemoryGameDlg::OnKey
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: *(this + 114) != 0 ? (*(this + 114) + 4) : 0);`

### CMiniRoomBaseDlg::OnInviteStatic
  - `if ( (TSecType<unsigned char>::GetData(this: v1 + 2060) & 1) != 0 && sub_4F38B4(this: v1) )`

### CPersonalShopDlg::0CSoldItemDlg
  - `CDialog::CreateDlg(a1: *StringW, a2: 1, a3: 0);`

### CPersonalShopDlg::BuyItem
  - `Data = TSecType<long>::GetData(a1: (v17 + 12), a2: 0);`
  - `COutPacket::Encode1(this: &v68, a2: v53 != 0 ? 34 : 23);`
  - `v55 = TSecType<long>::GetData(a1: (v6[4] + 12), a2: dword_BE78D8);`

### CPersonalShopDlg::MoveItemToInventory
  - `Data = TSecType<long>::GetData(a1: (*(*(this + 101) + 20 * a2 + 16) + 12), a2: v2);`
  - `COutPacket::Encode1(this: &v16, a2: v9 != 0 ? 38 : 27);`

### CPersonalShopDlg::OnButtonClicked
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: *(this + 146) != 0 ? (*(this + 146) + 4) : 0);`

### CPersonalShopDlg::OnKey
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: *(this + 145) != 0 ? (*(this + 145) + 4) : 0);`

### CPersonalShopDlg::OnMouseButton
  - `Data = TSecType<long>::GetData(a1: (*(v9 + 16) + 12), a2: &pvargSrc);`
  - `CItemInfo::GetItemIcon(a1: &v50, a2: Data, bPetDead: 0, bWithShadow: 0);`
  - `a5 = TSecType<long>::GetData(a1: (*(v46 + *(v49 + 100) + 16) + 12), a2) / 1000000;`
  - `CWndMan::BeginDragDrop(a1: v33 != 4 ? v33 : 0, a2: v32);`

### CPersonalShopDlg::PutItem
  - `Data = TSecType<long>::GetData(a1: (a4 + 12), a2: 0);`
  - `v13 = TSecType<long>::GetData(a1: (a4 + 12), a2: 0);`
  - `v14 = TSecType<long>::GetData(a1: (a4 + 12), a2: 0) / 10000;`
  - `COutPacket::Encode1(this: &v29, a2: v21 != 0 ? 33 : 22);`

### CShopDlg::DrawBuyItem
  - `v33 = *StringPool::GetString(this: Instance, result: v86[0], nIdx: SHIDWORD(v86[0]));`
  - `v48 = *StringPool::GetString(this: v47, result: v86[0], nIdx: SHIDWORD(v86[0]));`

### CShopDlg::DrawSellItem
  - `StringW = StringPool::GetStringW(this: Instance, a2: SLODWORD(v61[0]), a3: SHIDWORD(v61[0]));`
  - `v32 = *StringPool::GetString(this: v31, result: LODWORD(v61[0]), nIdx: SHIDWORD(v61[0]));`

### CShopDlg::OnPacket
  - `if ( ZAllocEx<ZAllocAnonSelector>::Alloc(a1: 1584) != 0 )`
  - `CCtrlScrollBar::SetCurPos(this: *(v2 + 184), a2: v11 - 4);`

### CShopDlg::ResetInfo
  - `CShopDlg::SetSellItems(this, a2: 0, a3: this + 212);`

### CShopDlg::SetSellItems
  - `ItemIcon = CItemInfo::GetItemIcon(a1: &v32, a2: v24, bPetDead: 1, bWithShadow: 0);`

### CShopDlg::SetShopDlg
  - `ItemIcon = CItemInfo::GetItemIcon(a1: &v103, a2: v17, bPetDead: 1, bWithShadow: 0);`
  - `CCtrlTab::SetTab(this: *(v3 + 39), a2: 1);`
  - `CShopDlg::SetSellItems(this: v3, a2: 0, a3: v3 + 212);`

### CTabBlackList::OnAdd
  - `CUtilDlgEx::SetUtilDlgEx(this: v7, a2: 3, a3: 0, a4: v32, a5: v33, a6: v34[0]);`
  - `CUtilDlgEx::SetUtilDlgEx_INPUT_STR(this: v37, a2: &sStrDefault, a3: 4, a4: 12, a5: 0, a6: 0);`
  - `ZXString<char>::_Release(lpAddend: (v35[0] - 12));`
  - `CWnd::InvalidateRect(this: v27[4], a2: 0);`

### CTabFriend::OnBlockFriend
  - `CWnd::InvalidateRect(this: *(this + 18), a2: 0);`

### CTabFriend::OnDblClick
  - `CTabFriend::GetFriendItem(this, a2: *(*(this + 18) + 2172), a3: v10);`

### CTabFriend::OnWhisper
  - `CDialog::CreateDlg(this: v22, a2: 0, a3: 0, a4: 1, a5: 0);`

### CTabGuild::OnPartyInvite
  - `if ( CTabGuild::GetGuildItem(this, a2: *(v1 + 2172), a3: v3) != -1 && get_field() != 0 )`

### CTabGuildAlliance::OnPartyInvite
  - `if ( CTabGuildAlliance::GetGuildItem(a1: *(v1 + 2172), a2: v3) != -1 && get_field() != 0 )`

### CTabParty::Draw
  - `v35 = *StringPool::GetString(this: v34, result: v279[0], nIdx: SHIDWORD(v279[0]));`
  - `v148 = StringPool::GetString(this: v147, result: v279[0], nIdx: SHIDWORD(v279[0]));`
  - `v310 = (*(get_field() + 324) & 0x800) != 0;`

### CTabParty::GetToolTipString
  - `MemberIDFromPoint = CTabParty::GetMemberIDFromPoint(this, a2: rx, a3: ry, a4: *(*(*(this + 9) + 1472) + 56));`

### CTabParty::OnInvite
  - `CUtilDlgEx::SetUtilDlgEx(this: v5, a2: 3, a3: 0, a4: v7, a5: v8, a6: InputStr_Result);`
  - `CUtilDlgEx::SetUtilDlgEx_INPUT_STR(this: v11, a2: &sStrDefault, a3: 4, a4: 12, a5: 0, a6: 0);`

### CTabParty::OnKick
  - `if ( CTabParty::GetPartyItem(this, a2: *(v2 + 2172), a3: v4) == -1 )`

### CTradingRoomDlg::Draw
  - `CMiniRoomBaseDlg::GetUserID(a1: &v72, a2: 1 - *(this + 50));`

### CTradingRoomDlg::DrawItems
  - `v13 = TSecType<long>::GetData(a1: (*(v30 + *v4 + 4) + 12), a2: v4) / 1000000;`

### CTradingRoomDlg::HitTest
  - `if ( ppCtrl == 0 || *ppCtrl == 0 )`

### CTradingRoomDlg::OnButtonClicked
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: *(this + 107) != 0 ? (*(this + 107) + 4) : 0);`

### CTradingRoomDlg::OnKey
  - `CWndMan::SetFocus(this: TSingleton<CWndMan>::ms_pInstance, a2: *(this + 106) != 0 ? (*(this + 106) + 4) : 0);`

### CTradingRoomDlg::OnPutItem
  - `ItemIcon = CItemInfo::GetItemIcon(a1: &a2, a2: Data, bPetDead: 1, bWithShadow: 0);`

### CTradingRoomDlg::OnTrade
  - `COutPacket::Encode4(this: &v11, a2: *(v14 + 8 * i + 4));`

### CTradingRoomDlg::PutItem
  - `CUtilDlgEx::SetUtilDlgEx(this: v16, a2: 2, a3: 0, a4: v18, a5: v19, a6: v20);`
  - `CUtilDlgEx::SetUtilDlgEx_INPUT_NO(this: v24, a2: a7, a3: 1, a4: a7, a5: 0, a6: 10, a7: 0);`

### CTradingRoomDlg::PutMoney
  - `CUtilDlgEx::SetUtilDlgEx(this: v9, a2: 2, a3: 0, a4: v17, a5: v18, a6: v19);`
  - `CUtilDlgEx::SetUtilDlgEx_INPUT_NO(this: v24, a2: 1, a3: 1, a4: v10, a5: 0, a6: 10, a7: 0);`

### CTradingRoomDlg::Trade
  - `Data = TSecType<long>::GetData(a1: (*(v6 + 1) + 12), a2: 0);`
  - `COutPacket::Encode4(this: &v17, a2: *(v23 + 8 * m + 4));`

### CTrunkDlg::OnMouseButton
  - `if ( CTrunkDlg::GetItemIndexFromPoint(this: this - 1, a2: a4, a3: a5, a4: &a5, a5: &a2) != 0 )`
  - `CWnd::InvalidateRect(this: (this - 1), a2: 0);`
  - `else if ( a2 == 515 && CTrunkDlg::GetItemIndexFromPoint(this: this - 1, a2: a4, a3: a5, a4: &a5, a5: &a2) != 0 )`

### CTrunkDlg::OnMouseMove
  - `if ( CTrunkDlg::GetItemIndexFromPoint(this: this - 1, a2, a3, a4: &v10, a5: &a3) != 0 )`

### CTrunkDlg::OnPacket
  - `CCtrlScrollBar::SetCurPos(this: *(v4 + 196), a2: v9 - 4);`

### CTrunkDlg::ResetInfo
  - `CTrunkDlg::SetPutItems(this, a2: 0, a3: this + 216);`

### CTrunkDlg::SetGetItems
  - `v18[0] = TSecType<long>::GetData(a1: v21 + 12, a2: 0);`
  - `ItemIcon = CItemInfo::GetItemIcon(a1: &v24, a2: v18[0], bPetDead: 1, bWithShadow: 0);`

### CTrunkDlg::SetPutItems
  - `Data = TSecType<long>::GetData(a1: v7 + 12, a2: 0);`
  - `v9 = TSecType<long>::GetData(a1: v7 + 12, a2: 0);`
  - `*v10 = TSecType<long>::GetData(a1: v7 + 12, a2: 0);`
  - `ItemIcon = CItemInfo::GetItemIcon(a1: &v22, a2: *v10, bPetDead: 1, bWithShadow: 0);`

### CTrunkDlg::SetTrunkDlg
  - `CTrunkDlg::SetPutItems(this, a2: 0, a3: this + 216);`

### CUIAdminShopWishList::OnButtonClicked
  - `CUIAdminShopWishList::ToggleAddOn(this, a2: *(this + 44) == 0);`

### CUIAdminShopWishList::SearchItemName
  - `CWnd::InvalidateRect(this: *(this + 184), a2: 0);`

### CUIAdminShopWishListSearchResult::OnButtonClicked
  - `if ( CUIAdminShopWishList::CanAddList(this: *v33, a2: *(v7 + 4), a3: v28[0]) != 0 )`

### CUIFamily::OnButtonClicked
  - `CUtilDlgEx::SetUtilDlgEx(this: v12, a2: 3, a3: 0, a4: v21, a5: v22, a6: v23[0]);`
  - `CUtilDlgEx::SetUtilDlgEx_INPUT_STR(this: v25, a2: *(*(this + 703) + 28), a3: 1, a4: 200, a5: 0, a6: 0);`

### CUIFamilyChart::OnMouseMove
  - `&& (InterlockedIncrement(lpAddend: (v5 + 4)), v5 = v16, v16 != 0)`

### CUIFamilyChart::_UnregisterCharacter
  - `CUtilDlgEx::SetUtilDlgEx(this: v8, a2: 3, a3: 0, a4: v29, a5: v30, a6: v31);`
  - `CUtilDlgEx::SetUtilDlgEx_INPUT_STR(this: v40, a2: &sStrDefault, a3: 4, a4: 12, a5: 0, a6: 0);`

### CUIGuildBBS::OnCreate
  - `CCtrlSelector::SetSelectorAttr(this: *(this + 414), a2: 0, a3: -10177296, a4: -16777216);`

### CUIGuildBBS::OnDelete
  - `COutPacket::Encode4(this: &v9, a2: *(*(this + 367) + 12));`

### CUIGuildBBS::OnLoadListResult
  - `CInPacket::DecodeBuffer(a1: (*(this + 371) + 24), a2: 8u);`
  - `CCtrlSelector::SetSelectorStart(this: *(this + 414), a2: v19, a3: 10, a4: 1, a5: 1);`

### CUIGuildBBS::OnMouseButton
  - `CWnd::InvalidateRect(this: (this - 1), a2: 0);`

### CUIGuildBBS::OnViewEntryResult
  - `CInPacket::DecodeBuffer(a1: (*(this + 367) + 20), a2: 8u);`

### CUIMessenger::AddChatText
  - `CCtrlScrollBar::SetScrollRange(this: v23, a2: v25 - *(this + 28) + 1);`
  - `CCtrlScrollBar::SetCurPos(this: v26, a2: *(v26 + 15) - 1);`

### CUIMessenger::OnChat
  - `Buffer = ZXString<char>::GetBuffer(a1: 1024, a2: 1);`

### CUIMessenger::OnCreate
  - `PcCreateObject::IWzFont(sUOL: v5, pObj: this + 34, pUnkOuter: 0);`

### CUIMessenger::OnKey
  - `v5 = *(TSingleton<CWndMan>::ms_pInstance + 136) == (v4 != 0 ? v4 + 4 : 0);`
  - `CWnd::SetFocusChild(this: this - 1, a2: *(this + 46));`
  - `CWndMan::SetFocus(a1: *(this + 46) != 0 ? *(this + 46) + 4 : 0);`

### CUIMessenger::ProcessChat
  - `CWndMan::SetFocus(a1: *(this + 47) != 0 ? v8 + 4 : 0);`

### CUIMessenger::SetCtrl
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 49), a2: v42 - *(this + 28) + 1);`

### CUIPartyHP::Create
  - `CWnd::CreateWnd(this, nLeft: v18, nTop: v19, nWidth: v27, nHeight: v8, z: 10, bScreenCoord: 1, pData: 0, bSetFocus: 0);`

### CUIShopScanResult::OnMouseMove
  - `CUIToolTip::SetToolTip_String(this: (this + 61), a2: v25 + a2 + 20, a3: v59, a4: ChannelName);`

### CUIUserList::CreateAllianceButton
  - `v20 = v19 != 0 ? CCtrlButton::CCtrlButton(this: v19) : 0;`

### CUIUserList::DestroyAllianceButton
  - `return ZRef<CCtrlButton>::_ReleaseRaw(a1: this + 8 * a2 + 1880);`

### CUIUserList::DestroyGuildButton
  - `return ZRef<CCtrlButton>::_ReleaseRaw(a1: this + 8 * a2 + 1736);`

### CUIUserList::Draw
  - `CTabBlackList::Draw(this: *(v129 + 550), arg0: HIDWORD(v103[0]));`
  - `CTabGuild::Draw(this: *(v129 + 547), a2: v103[0], a3: HIDWORD(v103[0]));`
  - `CTabParty::Draw(this: *(v129 + 546), a2: HIDWORD(v103[0]));`
  - `CTabFriend::Draw(a1: v103[0], a2: HIDWORD(v103[0]));`
  - `HIDWORD(v103[0]) = *StringPool::GetString(this: v54, result: v103[0], nIdx: SHIDWORD(v103[0]));`
  - `String = StringPool::GetString(this: v57, result: v103[0], nIdx: SHIDWORD(v103[0]));`

### CUIUserList::OnCreate
  - `PcCreateObject::IWzFont(sUOL: v245, pObj: this + 551, pUnkOuter: 0);`

### CUIUserList::OnMouseButton
  - `CUIUserList::SetScrollBar(this: (this - 4), a2: 0);`
  - `CWnd::InvalidateRect(this: (this - 4), a2: 0);`

### CUIUserList::OnMouseMove
  - `CUIToolTip::SetToolTip_String(this: (this + 28), a2: v14 + a2 + 20, a3: v16, a4: v17);`

### CUIUserList::ResetInfo
  - `CWnd::InvalidateRect(this: *(this + 549), a2: 0);`

### CUIUserList::SetScrollBar
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 368), a2: v9 / 33 + 2);`
  - `CCtrlScrollBar::SetScrollRange(this: *(this + 368), a2: 0);`

### CWndAllianceGrade::OnChangeGradeName
  - `CUtilDlgEx::SetUtilDlgEx(this: v9, a2: 3, a3: 0, a4: v27, a5: v28, a6: v29);`
  - `CUtilDlgEx::SetUtilDlgEx_INPUT_STR(this: v31, a2: &sStrDefault, a3: v10, a4: 10, a5: 0, a6: 0);`

### CWndGuildGrade::OnChangeGradeName
  - `CUtilDlgEx::SetUtilDlgEx(this: v9, a2: 3, a3: 0, a4: v27, a5: v28, a6: v29);`
  - `CUtilDlgEx::SetUtilDlgEx_INPUT_STR(this: v31, a2: &sStrDefault, a3: v10, a4: 10, a5: 0, a6: 0);`

### TabPartySearch::StartSearch
  - `if ( TabPartySearch::IsValidSetting(this, a2: v11[0], a3: v11[1], a4: v11[2], a5: v11[3], a6: v11[4], a7: v4) != 0 )`

### UFamilyItem::1?$ZRef
  - `result = InterlockedDecrement(lpAddend: (*(this + 1) + 4));`
  - `InterlockedIncrement(lpAddend: (*(this + 1) + 4));`
