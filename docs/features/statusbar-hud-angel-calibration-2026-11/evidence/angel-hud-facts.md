# Angel 083 状态栏 HUD 反编译语义抽取（观察层）

来源：`evidence/angel-hud-decompile-full.json`（247 函数，逐字 Hex-Rays 伪代码，含 10 个超大函数从 /output/ 端点补全）。
本文件由 `tools/ida_angel_hud_facts.py` 自动生成，只做字符串抽取，不含推断。

## 1. StringPool 资源节点全集（316 个）

- `SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK` ← CChatBalloon, CSoftKeyboardDlg, CUIKeyConfig, CUIMiniMap, CUIQuestAlarm, CUIStat…
- `SP_101_MAPLESTORY_ERROR__LOADING_TAMINGMOB_FAILED` ← CUIUserInfo
- `SP_10_EQP` ← CUIQuestAlarm
- `SP_1240_WORLDMAPOPEN` ← CWorldMapDlg
- `SP_1253_UI_BASICIMG_ITEMNO` ← CUISoftKeyboard, CUIStatusBar
- `SP_1254_UI_BASICIMG_LEVELNO` ← CUIStatusBar
- `SP_1257_UI_BASICIMG_BTOK2` ← CUIGameOpt, CUISysOpt
- `SP_1258_UI_BASICIMG_BTCANCEL2` ← CUIGameOpt, CUISysOpt
- `SP_1260_UI_BASICIMG_BTCLAIM` ← CUIMessenger
- `SP_1261_UI_BASICIMG_BTMIN` ← CUIMessenger, CUIMiniMap
- `SP_1262_UI_BASICIMG_BTMAX` ← CUIMessenger, CUIMiniMap
- `SP_1263_UI_UIWINDOWIMG_MINIMAP_BTMAP` ← CUIMiniMap
- `SP_1264_UI_BASICIMG_HSCR` ← CCtrlScrollBar
- `SP_1265_UI_BASICIMG_VSCR` ← CCtrlScrollBar
- `SP_1300_UI_UIWINDOWIMG_FLOATNOTICE_D` ← CFloatNotice
- `SP_1388_UI_CHATBALLOONIMG_` ← CChatBalloon
- `SP_1389_NPC` ← CChatBalloon
- `SP_1390_ADBOARD` ← CChatBalloon
- `SP_1394_NW` ← CChatBalloon, CUIMiniMap
- `SP_1395_N` ← CChatBalloon, CUIMiniMap
- `SP_1396_NE` ← CChatBalloon, CUIMiniMap
- `SP_1397_E` ← CChatBalloon, CUIMiniMap
- `SP_1398_W` ← CChatBalloon, CUIMiniMap
- `SP_1399_C` ← CChatBalloon, CUIMiniMap
- `SP_1400_SW` ← CChatBalloon, CUIMiniMap
- `SP_1401_S` ← CChatBalloon, CUIMiniMap
- `SP_1402_SE` ← CChatBalloon, CUIMiniMap
- `SP_1403_ARROW` ← CChatBalloon
- `SP_1409_CLR` ← CChatBalloon
- `SP_1410_CANVASFONT` ← CChatBalloon, CCtrlEdit, CCtrlMLEdit, CUIMessenger, CUIMiniMap, CUIStatusBar
- `SP_1411_BA` ← CChatBalloon, CUIMiniMap
- `SP_1414_UI_CHATBALLOONIMG_MINIROOM` ← CChatBalloon
- `SP_1416_OMOK` ← CChatBalloon
- `SP_1421_CNUM` ← CChatBalloon
- `SP_1422_MNUM` ← CChatBalloon
- `SP_1425_PROGRESS` ← CChatBalloon
- `SP_1434__NORMAL_0` ← CCtrlComboBox
- `SP_1436__PRESSED_0` ← CCtrlComboBox
- `SP_1438__DISABLED_0` ← CCtrlComboBox
- `SP_1440__MOUSEOVER_0` ← CCtrlComboBox
- `SP_1443_BTMOUSECLICK` ← CCtrlButton
- `SP_1444_BTMOUSEOVER` ← CCtrlButton
- `SP_1445_TITLE` ← CUIMiniMap, CWorldMapDlg
- `SP_1447_DESC` ← CWorldMapDlg
- `SP_1448_MIN` ← CUIMiniMap
- `SP_1451__ENABLED_PREV0` ← CCtrlScrollBar
- `SP_1452__ENABLED_PREV1` ← CCtrlScrollBar
- `SP_1453__ENABLED_NEXT0` ← CCtrlScrollBar
- `SP_1454__ENABLED_NEXT1` ← CCtrlScrollBar
- `SP_1455__ENABLED_BASE` ← CCtrlScrollBar
- `SP_1456__ENABLED_THUMB0` ← CCtrlScrollBar
- `SP_1457__ENABLED_THUMB1` ← CCtrlScrollBar
- `SP_1458__DISABLED_PREV` ← CCtrlScrollBar
- `SP_1459__DISABLED_NEXT` ← CCtrlScrollBar
- `SP_145__INVITE` ← CUIMessenger
- `SP_1460__DISABLED_BASE` ← CCtrlScrollBar
- `SP_147_PLEASE_TRY_AGAIN` ← CUIUserInfo
- `SP_1515_TYPE` ← CUIMiniMap, CWorldMapDlg
- `SP_153_S_IS_AT_S` ← CUIUserInfoWishList
- `SP_158_EITHER_THE_PARTY_DOESNT_EXIST_OR_NO_MEMBER_OF_YOUR_PARTY_IS_LOGGED_ON` ← CUIStatusBar
- `SP_1593_MOB` ← CChatBalloon
- `SP_1604_NAME` ← CUIQuestAlarm
- `SP_160_YOU_HAVE_NO_BUDDY_AVAILABLE_FOR_CHAT` ← CUIStatusBar, CUIUserInfo
- `SP_161_YOU_ARE_NOT_IN_A_GUILD_YET_OR_NONE_OF_YOUR_GUILD_MEMBER_IS_ONLINE` ← CUIStatusBar
- `SP_168_YOU_HAVE_ENTERED_AN_INVALID_CHARACTER_NAME` ← CUIUserInfo
- `SP_170_YOU_HAVE_EITHER_ENTERED_A_WRONG_NPC_NAME_OR` ← CUIQuestAlarm
- `SP_1729_STREETNAME` ← CUIMiniMap, CWorldMapDlg
- `SP_1730_MAPNAME` ← CUIMiniMap, CWorldMapDlg
- `SP_1731_MAPMARK` ← CUIMiniMap
- `SP_1732_MAP_MAPHELPERIMG_MARK_` ← CUIMiniMap
- `SP_1733_NONE` ← CUIMiniMap
- `SP_1751_WIDTH` ← CUIMiniMap
- `SP_1752_HEIGHT` ← CUIMiniMap
- `SP_1796_S__S` ← CUIStatusBar
- `SP_1803_PATH` ← CWorldMapDlg
- `SP_182__START` ← CUIUserInfo
- `SP_183__RESET` ← CUIUserInfo
- `SP_1847_DRAGSTART` ← CUIKeyConfig
- `SP_1858_UI_UIWINDOWIMG_MESSENGER_BLINK` ← CUIMessenger
- `SP_1859__________________________` ← CUIMessenger
- `SP_1868__Q` ← CUIMessenger
- `SP_1869__Q` ← CUIMessenger
- `SP_1870_MINIMAP` ← CUIMiniMap
- `SP_1871_HIDEMINIMAP` ← CUIMiniMap
- `SP_1872_MINIMAPONOFF` ← CUIMiniMap
- `SP_1873_CANVAS` ← CUIMiniMap
- `SP_1874_CENTERX` ← CUIMiniMap
- `SP_1875_CENTERY` ← CUIMiniMap
- `SP_1876_MAG` ← CUIMiniMap
- `SP_1877_LIFE` ← CUIMiniMap
- `SP_1882_UI_UIWINDOWIMG_MINIMAP` ← CUIMiniMap
- `SP_1883_UI_UIWINDOWIMG_ENERGYBAR` ← CUIEnergyBar
- `SP_1886_DISPLAYS_THE_ENERGY_LEVEL_THATS_BEEN_CHARGED_NUSE_THE_MOUSE_TO_CHANGE_POSITION` ← CUIEnergyBar
- `SP_1887_REQLEVEL` ← CUIUserInfo, CUIUserInfoDetail
- `SP_189__GIVEPRIZE` ← CUIUserInfo
- `SP_1923_UI_UIWINDOWIMG_SKILL_BTSPUP` ← CUISkill
- `SP_1924_UI_UIWINDOWIMG_SKILL_SKILL0` ← CUISkill
- `SP_1925_UI_UIWINDOWIMG_SKILL_SKILL1` ← CUISkill
- `SP_1926_UI_UIWINDOWIMG_SKILL_RECOMMEND_0` ← CUISkill
- `SP_1939_D_DD` ← CUIStat
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
- `SP_1957_UI_STATUSBARIMG_GAUGE_BAR` ← CUIStatusBar
- `SP_1958_UI_STATUSBARIMG_GAUGE_GRAY` ← CUIStatusBar
- `SP_1959_UI_STATUSBARIMG_GAUGE_TEMPEXP` ← CUIStatusBar
- `SP_1973_UI_STATUSBARIMG_BTSHOP` ← CUIStatusBar
- `SP_1974_UI_STATUSBARIMG_BTSHORT` ← CUIStatusBar
- `SP_1975_UI_STATUSBARIMG_BTMENU` ← CUIStatusBar
- `SP_1976_UI_STATUSBARIMG_BTCLAIM` ← CUIStatusBar
- `SP_1979_UI_STATUSBARIMG_EQUIPKEY` ← CUIStatusBar
- `SP_1980_UI_STATUSBARIMG_INVENKEY` ← CUIStatusBar
- `SP_1981_UI_STATUSBARIMG_STATKEY` ← CUIStatusBar
- `SP_1982_UI_STATUSBARIMG_SKILLKEY` ← CUIStatusBar
- `SP_1983_UI_STATUSBARIMG_KEYSET` ← CUIStatusBar
- `SP_1984_UI_STATUSBARIMG_QUICKSLOT` ← CUIStatusBar
- `SP_1985_UI_STATUSBARIMG_QUICKSLOTD` ← CUIStatusBar
- `SP_1986_UI_STATUSBARIMG_EQUIPKEY_ANI` ← CUIStatusBar
- `SP_1987_UI_STATUSBARIMG_INVENKEY_ANI` ← CUIStatusBar
- `SP_1988_UI_STATUSBARIMG_STATKEY_ANI` ← CUIStatusBar
- `SP_1989_UI_STATUSBARIMG_SKILLKEY_ANI` ← CUIStatusBar
- `SP_1990_STRING_TOOLTIPHELPIMG_GAME_BUTTON_SHOP` ← CUIStatusBar
- `SP_1993_STRING_TOOLTIPHELPIMG_GAME_BUTTON_CLAIM` ← CUIStatusBar
- `SP_1997_STRING_TOOLTIPHELPIMG_GAME_BUTTON_MENU` ← CUIStatusBar
- `SP_1998_STRING_TOOLTIPHELPIMG_GAME_BUTTON_KEYSET` ← CUIStatusBar
- `SP_1999_STRING_TOOLTIPHELPIMG_GAME_BUTTON_QUICKSLOT` ← CUIStatusBar
- `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID` ← CCtrlEdit, CCtrlMLEdit, CSoftKeyboardDlg, CUIMiniMap, CUIQuestAlarm, CUIStat…
- `SP_2000_STRING_TOOLTIPHELPIMG_GAME_BUTTON_QUICKSLOTD` ← CUIStatusBar
- `SP_2003_S___S___S` ← CUIStatusBar
- `SP_2051_UI_UIWINDOWIMG_USERINFO_BACKGRND` ← CUIUserInfo
- `SP_2052_UI_UIWINDOWIMG_USERINFO_BACKGRND2` ← CUIUserInfo
- `SP_2055_UI_UIWINDOWIMG_USERINFO_BACKGRND5` ← CUIUserInfo
- `SP_2057_UI_UIWINDOWIMG_USERINFO_BACKGRND7` ← CUIUserInfo
- `SP_2064_UI_UIWINDOWIMG_STAT_BTDETAIL` ← CUIStat
- `SP_2212_UI_UIWINDOWIMG_CHANNEL_BTCHANGE` ← CUIChannelShift
- `SP_2213_UI_UIWINDOWIMG_CHANNEL_BTCANCEL` ← CUIChannelShift
- `SP_2214_UI_UIWINDOWIMG_CHANNEL` ← CUIChannelShift
- `SP_2228_UI_UIWINDOWIMG_SHORTCUT_BTITEM` ← CUIShortCutMenu
- `SP_2229_UI_UIWINDOWIMG_SHORTCUT_BTEQUIP` ← CUIShortCutMenu
- `SP_2230_UI_UIWINDOWIMG_SHORTCUT_BTSTAT` ← CUIShortCutMenu
- `SP_2231_UI_UIWINDOWIMG_SHORTCUT_BTSKILL` ← CUIShortCutMenu
- `SP_2234_UI_UIWINDOWIMG_SHORTCUT_BTQUEST` ← CUIShortCutMenu
- `SP_2235_UI_UIWINDOWIMG_SHORTCUT_BTCOMM` ← CUIShortCutMenu
- `SP_2236_UI_UIWINDOWIMG_SHORTCUT_BTMOBBOOK` ← CUIShortCutMenu
- `SP_2237_UI_UIWINDOWIMG_SHORTCUT_BTMESSENGER` ← CUIShortCutMenu
- `SP_2240_STRING_MOBIMG` ← CUIQuestAlarm
- `SP_2289_UI_UIWINDOW_WORLDMAP_TITLE` ← CWorldMapDlg
- `SP_2290_BASEIMG_0` ← CWorldMapDlg
- `SP_2291_MAPLIST` ← CWorldMapDlg
- `SP_2292_MAPNO` ← CWorldMapDlg
- `SP_2293_MAPDESC` ← CWorldMapDlg
- `SP_236__POINTER03` ← CUIUserInfo
- `SP_2402_EQUIPS` ← CUIStatusBar
- `SP_2403_ITEMS` ← CUIStatusBar
- `SP_2404_ABILITYS` ← CUIStatusBar
- `SP_2405_SKILLS` ← CUIStatusBar
- `SP_2413_ICON_D` ← CUIKeyConfig
- `SP_2503_UI_UIWINDOWIMG_KEYCONFIG_NOTICE_D` ← CUIKeyConfig
- `SP_2552_UI_UIWINDOWIMG_CHANNEL_WORLD_D` ← CUIChannelShift
- `SP_2564_UI_UIWINDOWIMG_USERINFO_REQLV` ← CUIUserInfo, CUIUserInfoDetail
- `SP_2577_UI_UIWINDOWIMG_USERINFO_MARRY_MARRIED_0` ← CUIUserInfo
- `SP_2578_UI_UIWINDOWIMG_USERINFO_MARRY_UNMARRIED_0` ← CUIUserInfo
- `SP_25_HERO` ← CUIQuestAlarm
- `SP_2601_UI_UIWINDOWIMG_CONTEXTMENU_BTINFO` ← CUIContextMenu
- `SP_2602_UI_UIWINDOWIMG_CONTEXTMENU_BTSAVE` ← CUIContextMenu
- `SP_2603_UI_UIWINDOWIMG_CONTEXTMENU_BTWHISPER` ← CUIContextMenu
- `SP_2604_UI_UIWINDOWIMG_CONTEXTMENU_BTCLAIM` ← CUIContextMenu
- `SP_2605_UI_UIWINDOWIMG_CONTEXTMENU_T` ← CUIContextMenu
- `SP_2606_UI_UIWINDOWIMG_CONTEXTMENU_C` ← CUIContextMenu
- `SP_2607_UI_UIWINDOWIMG_CONTEXTMENU_S` ← CUIContextMenu
- `SP_2608_UI_UIWINDOWIMG_CONTEXTMENU_BTPARTY` ← CUIContextMenu
- `SP_2609_UI_UIWINDOWIMG_CONTEXTMENU_BTGUILD` ← CUIContextMenu
- `SP_2610_UI_UIWINDOWIMG_CONTEXTMENU_BTTRADE` ← CUIContextMenu
- `SP_2611_UI_UIWINDOWIMG_CONTEXTMENU_BTFAMILY` ← CUIContextMenu
- `SP_2623_MAXMAP` ← CUIMiniMap
- `SP_2624_MINMAP` ← CUIMiniMap
- `SP_2643_UI_STATUSBARIMG_FONTMEMO` ← CUIStatusBar
- `SP_2660_UI_UIWINDOWIMG_SKILL_LINE` ← CUISkill
- `SP_267_YOU_ARE_CURRENTLY_AT_A_PLACE_WHERE_R_NTHE_WORLD_MAP_IS_NOT_AVAILABLE` ← CUIMiniMap
- `SP_269_THE_WORLD_MAP_IS_UNAVAILABLE_TO_GUEST_ID_PLAYERS_R_NPLEASE_DOWNLOAD_THE_FULL_CLI` ← CUIMiniMap
- `SP_2722_UI_UIWINDOWIMG_MESSENGER_BTENTER` ← CUIMessenger
- `SP_272_CURSING_IS_NOT_ALLOWED` ← CChatHelper
- `SP_2756__SELECTED_0` ← CCtrlComboBox
- `SP_2757__NORMAL_1` ← CCtrlComboBox
- `SP_2758__PRESSED_1` ← CCtrlComboBox
- `SP_2759__DISABLED_1` ← CCtrlComboBox
- `SP_2760__MOUSEOVER_1` ← CCtrlComboBox
- `SP_2761__SELECTED_1` ← CCtrlComboBox
- `SP_2762__NORMAL_2` ← CCtrlComboBox
- `SP_2763__PRESSED_2` ← CCtrlComboBox
- `SP_2764__DISABLED_2` ← CCtrlComboBox
- `SP_2765__MOUSEOVER_2` ← CCtrlComboBox
- `SP_2766__SELECTED_2` ← CCtrlComboBox
- `SP_2769_THERE_IS_NO_ONE_TO_WHISPER_TO` ← CUIStatusBar
- `SP_2783_MAP_OBJ_ETCIMG_CLOCK_FONTTIME` ← CClock
- `SP_27_WHITE_KNIGHT` ← CUIMiniMap
- `SP_2803_UI_BASICIMG_COMBOBOX` ← CCtrlComboBox
- `SP_2867_MAP_OBJ_ETCIMG_TIMER_BACKGRND` ← CClock
- `SP_2892_UI_STATUSBARIMG_BASE_CHAT` ← CUIStatusBar
- `SP_28_PALADIN` ← CUIMiniMap
- `SP_2921_UI_UIWINDOWIMG_MEGAPHONE_CHD` ← CUIStatusBar
- `SP_2922_UI_UIWINDOWIMG_MEGAPHONE_NUM_D` ← CUIStatusBar
- `SP_29_SPEARMAN` ← CUIMiniMap
- `SP_2_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTPASSWORD` ← CUIStatusBar
- `SP_3095_HIDE` ← CUIMiniMap
- `SP_3401_HAHA` ← CUIStatusBar
- `SP_3402_HOHO` ← CUIStatusBar
- `SP_3403_HEHE` ← CUIStatusBar
- `SP_3404_HARHAR` ← CUIStatusBar
- `SP_3405_WAHAHA` ← CUIStatusBar
- `SP_3406_SOB` ← CUIStatusBar
- `SP_3407_TEARS` ← CUIStatusBar
- `SP_3408_SNIFF` ← CUIStatusBar
- `SP_3452_ITEM_CASH_0503IMG` ← CChatBalloon
- `SP_3458_SIGNBOARD` ← CChatBalloon
- `SP_3509_YOURE_LACKING_LEVEL_1_OR_2_SKILLS` ← CUISkill
- `SP_3510_YOURE_LACKING_LEVEL_1_SKILLS` ← CUISkill
- `SP_3511_YOU_ARE_LACKING_IN_EITHER_1ST_2ND_OR_3RD_LEVEL_SKILLS` ← CUISkill
- `SP_3550_GUILD_QUEST` ← CTemporaryStatView
- `SP_3580_WILL_YOU_EXIT_THIS_CHAT_ROOM` ← CUIMessenger
- `SP_3594_UI_UIWINDOWIMG_SKILL_COOLTIME_D` ← CUIStatusBar
- `SP_3620_QUEST_HELPER_D_5` ← CUIQuestAlarm
- `SP_3621_UI_BASICIMG_BTCLOSE2` ← CUIQuestAlarm
- `SP_3771_STRING_TOOLTIPHELPIMG_PORTALTOOLTIP_D` ← CUIMiniMap
- `SP_3_HTTP__MAPLESTORYNEXONNET_WZASPXPART_DOWNLOADS_GAMEPATCHES` ← CUIMiniMap, CUIStatusBar
- `SP_4038_SPOT` ← CWorldMapDlg
- `SP_4116_UI_LOGINIMG_COMMON_SOFTKEY_TAB_SELECTED` ← CSoftKeyboardDlg
- `SP_4117_UI_LOGINIMG_COMMON_SOFTKEY_TAB_NORMAL` ← CSoftKeyboardDlg
- `SP_4118_UI_LOGINIMG_COMMON_SOFTKEY_BTDEL` ← CSoftKeyboardDlg
- `SP_4119_UI_LOGINIMG_COMMON_SOFTKEY_BTNEXT` ← CSoftKeyboardDlg
- `SP_4120_UI_LOGINIMG_COMMON_SOFTKEY_BTOK` ← CSoftKeyboardDlg
- `SP_4121_UI_LOGINIMG_COMMON_SOFTKEY_BTCANCEL` ← CSoftKeyboardDlg
- `SP_4125_PLEASE_RE_ENTER_YOUR_PIC` ← CSoftKeyboardDlg
- `SP_4161_UI_UIWINDOWIMG_SKILLMACRO_MACROICON_D_ICON` ← CUIKeyConfig
- `SP_4188_DONT_MARK` ← CUISysOpt
- `SP_4189_MARK_NAME_ONLY` ← CUISysOpt
- `SP_4190_MARK_HP_ONLY` ← CUISysOpt
- `SP_4191_MARK_NAME_AND_HP` ← CUISysOpt
- `SP_4200_YOU_ARE_MARRIED_WITH__S` ← CUIToolTip
- `SP_4341_YOU_MAY_OPEN_CLOSE_YOUR_OWN_SKILL_WINDOW` ← CUISkill
- `SP_4344_UI_BASICIMG_BTMACRO` ← CUISkill
- `SP_434_YOU_CANT_START_THE_GAME_HERE` ← CUIStatusBar
- `SP_43_BISHOP` ← CUIMiniMap
- `SP_4430__S_S` ← CUIStatusBar
- `SP_4433_SHOWPARTYHP` ← CConfig
- `SP_44_ARCHER` ← CUIMiniMap
- `SP_45_HUNTER` ← CUIMiniMap
- `SP_4665_CYOU_CAN_ENABLE_1_TIME_OF_TRADING_WHEN_YOU_USE_THE_SCISSORS_OF_KARMA` ← CUIToolTip
- `SP_4667__R_N_CITEMS_THAT_CAN_BE_USED_` ← CUIToolTip
- `SP_4682_UI_UIWINDOWIMG_USERINFO_BOSSPETCROWN` ← CUIUserInfo
- `SP_4_HTTP__MAPLESTORYNEXONNET_PART_NEWS_NOTICEBBSIDX3255` ← CUIStatusBar
- `SP_5153_UI_UIWINDOWIMG_STAT_BASICSTAT` ← CUIStat
- `SP_5197_MOVEMENT_IS_POSSIBLE_ONLY_WITHIN_THE_ACCOUNT` ← CUIToolTip
- `SP_5274_TIME_LIMIT_EXTENSION_IS_NOT_POSSIBLE` ← CUIToolTip
- `SP_5275_UNIQUE_EQUIPPED_ITEM` ← CUIToolTip
- `SP_52_ASSASSIN` ← CUIUserInfo
- `SP_5515_D` ← CChatBalloon, CCtrlMLEdit, CUIStat, CUIUserInfo, CUIUserInfoDetail
- `SP_5516_D___D` ← CUIStat
- `SP_5527_ARIAL` ← CChatBalloon, CCtrlEdit, CSoftKeyboardDlg, CUIMessenger, CUIMiniMap, CUIStatusBar
- `SP_5537_EXP__D___D` ← CUIStatusBar
- `SP_5550_MESO` ← CUIQuestAlarm
- `SP_5577_UI_BASICIMG_BTCLOSE` ← CUIMessenger, CWorldMapDlg
- `SP_5597_WHISPER` ← CCtrlComboBox, CUIStatusBar
- `SP_5619_BACKGRND` ← CChatBalloon
- `SP_5622_CRY` ← CUIStatusBar
- `SP_5623_DEAD` ← CChatBalloon
- `SP_5624_DELAY` ← CChatBalloon, CCtrlButton
- `SP_5628_HEAD` ← CChatBalloon
- `SP_5641_NONE` ← CUIToolTip
- `SP_5643_PET` ← CChatBalloon
- `SP_572_D` ← CUIUserInfoWishList
- `SP_58_BRAWLER` ← CUIUserInfo
- `SP_61_GUNSLINGER` ← CUIStat
- `SP_651_ITS_A_GIFT_FROM_S` ← CUIToolTip
- `SP_671_A_GIFT_FROM_S` ← CUIToolTip
- `SP_672_A_COUPLE_RING_WITH__S` ← CUIToolTip
- `SP_673_THIS_IS_YOUR_FRIENDSHIP_RING_WITH_S` ← CUIToolTip
- `SP_680_S___S__S___PREMIUM` ← CUIStatusBar
- `SP_692_ONE_OF_A_KIND_ITEM` ← CUIToolTip
- `SP_693_QUEST_ITEM` ← CUIToolTip
- `SP_694_PARTY_QUEST_ITEM` ← CUIToolTip
- `SP_695_UNTRADEABLE` ← CUIToolTip
- `SP_696_CAN_TRADE_1_TIME_TRADE_DISABLED_AFTER_TRANSACTION` ← CUIToolTip
- `SP_699_TRADE_DISABLED_WHEN_EQUIPPED` ← CUIToolTip
- `SP_782_PLEASE_ENTER_THE_PERSON_YOU_WANT_TO_WHISPER_TO` ← CUIStatusBar
- `SP_783_INCORRECT_ID` ← CUIStatusBar
- `SP_784_YOU_CANT_WHISPER_TO_YOURSELF` ← CUIStatusBar
- `SP_786_TO_ALL` ← CCtrlComboBox, CUIStatusBar
- `SP_787_TO_THE_PARTY` ← CCtrlComboBox, CUIStatusBar
- `SP_788_TO_A_BUDDY` ← CCtrlComboBox, CUIStatusBar
- `SP_789_TO_SPOUSE` ← CCtrlComboBox, CUIStatusBar
- `SP_790_TO_THE_GUILD` ← CCtrlComboBox, CUIStatusBar
- `SP_791_TO_GROUP` ← CCtrlComboBox, CUIStatusBar
- `SP_800___MAPLE_MESSENGER_HELP__` ← CUIMessenger
- `SP_801_INVITE___INVITE_CHARACTER_NAME` ← CUIMessenger
- `SP_802_END___Q` ← CUIMessenger
- `SP_815_MAPLESTORY_FOLDER` ← CUISysOpt
- `SP_816_DESKTOP` ← CUISysOpt
- `SP_891_REPEATING_THE_SAME_LINE_OVER_AND_OVER_R_NCAN_NEGATIVELY_AFFECT_OTHER_USERS` ← CChatHelper
- `SP_892_TOO_MUCH_CHATTING_CAN_DISRUPT_R_NOTHER_PLAYERS_ABILITY_TO_PLAY_THE_GAME` ← CChatHelper
- `SP_893_YOU_ARE_CURRENTLY_BLOCKED_FROM_CHATTING` ← CChatHelper
- `SP_8_TAHOMA` ← CUIStatusBar
- `SP_956_CANVAS` ← CChatBalloon, CSlideNotice, CUIContextMenu, CUIEnergyBar, CUIMiniMap, CUIUserInfo
- `SP_959_ORIGIN` ← CWorldMapDlg
- `SP_977_X` ← CUIMiniMap
- `SP_978_Y` ← CFloatNotice, CUIMiniMap

## 2. CUIStatusBar 逐函数关键行


### CUIStatusBar::0CChatLog  (1450 B)


### CUIStatusBar::ChangeChatWndSize  (3856 B)

  - `{"addr":"0x8d616c","code":"int __thiscall CUIStatusBar::ChangeChatWndSize(CUIStatusBar *this, int a2, int a3, int a4)\n{\n  int result; // eax\n  int v6; // edx\n  int v7; // ecx\n  int v9; // eax\n  `

### CUIStatusBar::ChangeWhisperTarget  (6532 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_782_PLEASE_ENTER_THE_PERSON_YOU_WANT_TO_WHISPER_TO`, `SP_783_INCORRECT_ID`, `SP_784_YOU_CANT_WHISPER_TO_YOURSELF`

### CUIStatusBar::ChatLogAdd  (4218 B)


### CUIStatusBar::ChatLogDraw  (59158 B)

- SP_ 节点：`SP_2921_UI_UIWINDOWIMG_MEGAPHONE_CHD`, `SP_2922_UI_UIWINDOWIMG_MEGAPHONE_NUM_D`, `SP_4430__S_S`
  - `signed __int64 *__thiscall CUIStatusBar::ChatLogDraw(CUIStatusBar *this)`
  - `if ( CUIStatusBar::m_aChatLog != 0 )`
  - `v281 = *(CUIStatusBar::m_aChatLog - 1);`
  - `v1 = *(this + 708);`
  - `if ( *(v1 + 60) != 0 )`
  - `v281 += *(v1 + 56) - *(v1 + 60);`
  - `v2 = (this + 3260);`
  - `if ( *(this + 815) != 0 )`
  - `sub_428712(a1: 0);`
  - `LOBYTE(v295) = 1;`
  - `if ( dword_BF14EC == 0 )`
  - `v3 = IWzGr2D::CreateLayer(`
  - `uHeight: *(v285 + 831),`
  - `sub_428712(a1: *v3);`
  - `sub_4156C5(a1: &v286);`
  - `LOBYTE(v295) = 0;`
  - `if ( v4 < 0 )`
  - `if ( v5 < 0 )`
  - `if ( *v2 == 0 )`
  - `v7 = (*(**v2 + 224))(a1: *v2, a2: -1);`
  - `if ( v7 < 0 )`
  - `v8 = *CWnd::GetLayer(a1: v279);`
  - `sub_410FDF(this: &v272, a2: v8, a3: 1);`
  - `LOBYTE(v295) = 3;`
  - `if ( v9 )`
  - `v10 = *(v285 + 815);`
  - …（截断 26 行）

### CUIStatusBar::ClearToolTip  (227 B)

  - `{"addr":"0x8cfd7b","code":"_DWORD *__thiscall CUIStatusBar::ClearToolTip(CUIStatusBar *this)\n{\n  return CUIToolTip::ClearToolTip(this: (this + 104));\n}","refs":[{"addr":"0x8e6e23","name":"?ClearToo`

### CUIStatusBar::ConvertWhisperToNormal  (6891 B)


### CUIStatusBar::Draw  (4341 B)

  - `{"addr":"0x8d80a4","code":"VARIANTARG *__thiscall CUIStatusBar::Draw(CUIStatusBar *this, const struct tagRECT *pRect)\n{\n  _DWORD *v2; // edi\n  BOOL v3; // esi\n  VARIANTARG *result; // eax\n  int v`

### CUIStatusBar::EndChat  (389 B)

  - `{"addr":"0x8d5aaf","code":"int __thiscall CUIStatusBar::EndChat(CUIStatusBar *this)\n{\n  if ( *(this + 832) == 2 )\n    CUIStatusBar::SetChatType(this, nType: 1);\n  return CWndMan::SetFocus(a1: 0);\`

### CUIStatusBar::FloatNotice  (749 B)

  - `{"addr":"0x8d5acd","code":"void __thiscall CUIStatusBar::FloatNotice(CUIStatusBar *this, const char *a2, char a3, unsigned int a4)\n{\n  if ( a2 != 0 )\n  {\n    if ( a4 != 0 )\n      *(this + 813) = `

### CUIStatusBar::GetChatLog  (7309 B)


### CUIStatusBar::GetEmotionKey  (5415 B)

- SP_ 节点：`SP_3401_HAHA`, `SP_3402_HOHO`, `SP_3403_HEHE`, `SP_3404_HARHAR`, `SP_3405_WAHAHA`, `SP_3406_SOB`, `SP_3407_TEARS`, `SP_3408_SNIFF`, `SP_5622_CRY`

### CUIStatusBar::GetRTTI  (181 B)

  - `{"addr":"0x8cfd83","code":"int *__thiscall CUIStatusBar::GetRTTI(CUIStatusBar *this)\n{\n  return &dword_BF10FC;\n}","refs":[{"addr":"0xbf10fc","name":"dword_BF10FC"}]}`

### CUIStatusBar::GetShortCutIndexByPos  (927 B)

  - `{"addr":"0x8de8d5","code":"signed int __stdcall CUIStatusBar::GetShortCutIndexByPos(int a1, int a2)\n{\n  int v3; // edi\n  int *v4; // esi\n  int v5; // ecx\n  _DWORD v7[4]; // [esp+Ch] [ebp-10h] BYR`

### CUIStatusBar::GetWhisperTarget  (291 B)

  - `{"addr":"0x4c6db2","code":"_DWORD *__thiscall CUIStatusBar::GetWhisperTarget(char *this, _DWORD *a2)\n{\n  *a2 = 0;\n  ZXString<char>::operator=(a1: this + 3156);\n  return a2;\n}","refs":[{"addr":"0x`

### CUIStatusBar::HitTest  (1610 B)

  - `{"addr":"0x8de7da","code":"int __thiscall CUIStatusBar::HitTest(CUIStatusBar *this, signed int a2, signed int a3, struct CCtrlWnd **a4)\n{\n  int v5; // eax\n  int v6; // ecx\n  int v7; // edx\n  int `

### CUIStatusBar::IsKindOf  (471 B)

  - `{"addr":"0x8cfd89","code":"int __thiscall CUIStatusBar::IsKindOf(CUIStatusBar *this, const struct CRTTI *a2)\n{\n  const struct CRTTI *v2; // eax\n\n  v2 = &dword_BF10FC;\n  if ( &dword_BF10FC == 0 )\`

### CUIStatusBar::MakeCtrlEdit  (6019 B)

- SP_ 节点：`SP_5597_WHISPER`, `SP_786_TO_ALL`, `SP_787_TO_THE_PARTY`, `SP_788_TO_A_BUDDY`, `SP_789_TO_SPOUSE`, `SP_790_TO_THE_GUILD`, `SP_791_TO_GROUP`

### CUIStatusBar::OnButtonClicked  (3262 B)

  - `{"addr":"0x8d423d","code":"void __thiscall CUIStatusBar::OnButtonClicked(CUIStatusBar *this, unsigned int a2)\n{\n  if ( TSingleton<CUserLocal>::ms_pInstance != 0 && *(TSingleton<CUserLocal>::ms_pInst`

### CUIStatusBar::OnChildNotify  (1050 B)

  - `{"addr":"0x8d419c","code":"void __thiscall CUIStatusBar::OnChildNotify(CUIStatusBar *this, unsigned int a2, unsigned int a3, signed int a4)\n{\n  if ( a2 == 1010 )\n  {\n    if ( a3 < 0x12C || a3 > 0x`

### CUIStatusBar::OnCreate  (126188 B)

- SP_ 节点：`SP_0_HTTP__PASSPORTNEXONNET_PART_REGISTRATION_AGECHECK`, `SP_1253_UI_BASICIMG_ITEMNO`, `SP_1254_UI_BASICIMG_LEVELNO`, `SP_1410_CANVASFONT`, `SP_1957_UI_STATUSBARIMG_GAUGE_BAR`, `SP_1958_UI_STATUSBARIMG_GAUGE_GRAY`, `SP_1959_UI_STATUSBARIMG_GAUGE_TEMPEXP`, `SP_1973_UI_STATUSBARIMG_BTSHOP`, `SP_1974_UI_STATUSBARIMG_BTSHORT`, `SP_1975_UI_STATUSBARIMG_BTMENU`, `SP_1976_UI_STATUSBARIMG_BTCLAIM`, `SP_1979_UI_STATUSBARIMG_EQUIPKEY`, `SP_1980_UI_STATUSBARIMG_INVENKEY`, `SP_1981_UI_STATUSBARIMG_STATKEY`, `SP_1982_UI_STATUSBARIMG_SKILLKEY`, `SP_1983_UI_STATUSBARIMG_KEYSET`, `SP_1984_UI_STATUSBARIMG_QUICKSLOT`, `SP_1985_UI_STATUSBARIMG_QUICKSLOTD`, `SP_1990_STRING_TOOLTIPHELPIMG_GAME_BUTTON_SHOP`, `SP_1993_STRING_TOOLTIPHELPIMG_GAME_BUTTON_CLAIM`, `SP_1997_STRING_TOOLTIPHELPIMG_GAME_BUTTON_MENU`, `SP_1998_STRING_TOOLTIPHELPIMG_GAME_BUTTON_KEYSET`, `SP_1999_STRING_TOOLTIPHELPIMG_GAME_BUTTON_QUICKSLOT`, `SP_1_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTID`, `SP_2000_STRING_TOOLTIPHELPIMG_GAME_BUTTON_QUICKSLOTD`, `SP_2643_UI_STATUSBARIMG_FONTMEMO`, `SP_2_HTTP__PASSPORTNEXONNET_WZASPXPART_ACCOUNTS_FORGOTPASSWORD`, `SP_3594_UI_UIWINDOWIMG_SKILL_COOLTIME_D`, `SP_3_HTTP__MAPLESTORYNEXONNET_WZASPXPART_DOWNLOADS_GAMEPATCHES`, `SP_434_YOU_CANT_START_THE_GAME_HERE`, `SP_4_HTTP__MAPLESTORYNEXONNET_PART_NEWS_NOTICEBBSIDX3255`, `SP_5527_ARIAL`, `SP_8_TAHOMA`
  - `void __userpurge CUIStatusBar::OnCreate(_DWORD *a1@<ecx>, int ebp0@<ebp>, int a3)`
  - `*(ebp0 - 24) = a1;`
  - `a1[787] = CCtrlScrollBar::GetScrollBarSize(hv: v458, type: StringW);`
  - `basic_font = get_basic_font(a1: (ebp0 - 16), fType: 1);`
  - `v6 = get_basic_font(a1: (ebp0 - 16), fType: 0);`
  - `v458 = (a1 + 758);`
  - `*(ebp0 - 4) = 0;`
  - `v9 = *(ebp0 - 16);`
  - `*(ebp0 - 4) = -1;`
  - `if ( v9 != 0 )`
  - `*(ebp0 - 44) = &v456;`
  - `*(ebp0 - 4) = 1;`
  - `if ( v12 < 0 )`
  - `v458 = (a1 + 757);`
  - `*(ebp0 - 4) = 3;`
  - `v15 = *(ebp0 - 16);`
  - `if ( v15 != 0 )`
  - `*(ebp0 - 4) = 4;`
  - `if ( v18 < 0 )`
  - `v458 = (a1 + 760);`
  - `*(ebp0 - 4) = 6;`
  - `v21 = *(ebp0 - 16);`
  - `if ( v21 != 0 )`
  - `*(ebp0 - 4) = 7;`
  - `if ( v24 < 0 )`
  - `v458 = (a1 + 761);`
  - …（截断 26 行）

### CUIStatusBar::OnKey  (11651 B)

- 分支 case：`0`, `1`, `2`, `3`, `4`, `5`, `6`, `7`
- SP_ 节点：`SP_2769_THERE_IS_NO_ONE_TO_WHISPER_TO`

### CUIStatusBar::OnMouseButton  (1559 B)

  - `{"addr":"0x8d5b5d","code":"void __fastcall CUIStatusBar::OnMouseButton(\n        CUIStatusBar *this,\n        int a2,\n        unsigned int a3,\n        unsigned int a4,\n        int a5,\n        int `

### CUIStatusBar::OnMouseEnter  (349 B)

  - `{"addr":"0x8d4225","code":"_DWORD *__thiscall CUIStatusBar::OnMouseEnter(CUIStatusBar *this, int a2)\n{\n  CWnd::OnMouseEnter(a1: a2);\n  return CUIToolTip::ClearToolTip(this: (this + 104));\n}","refs`

### CUIStatusBar::OnMouseMove  (18117 B)

- SP_ 节点：`SP_2892_UI_STATUSBARIMG_BASE_CHAT`

### CUIStatusBar::OnSetFocus  (128 B)

  - `{"addr":"0x8cfd76","code":"int __thiscall CUIStatusBar::OnSetFocus(CUIStatusBar *this, int a2)\n{\n  return 0;\n}"}`

### CUIStatusBar::ProcessToolTip  (4660 B)

- SP_ 节点：`SP_2003_S___S___S`, `SP_5537_EXP__D___D`, `SP_680_S___S__S___PREMIUM`

### CUIStatusBar::ReDrawQuickslot  (271 B)

  - `{"addr":"0x8d82a7","code":"VARIANTARG *__thiscall CUIStatusBar::ReDrawQuickslot(CUIStatusBar *this)\n{\n  sub_8DD860(this);\n  return sub_8DDB30(this);\n}","refs":[{"addr":"0x8dd860","name":"sub_8DD86`

### CUIStatusBar::ResetButtonToolTip  (838 B)

  - `{"addr":"0x8defae","code":"BOOL __thiscall CUIStatusBar::ResetButtonToolTip(CUIStatusBar *this)\n{\n  int v2; // esi\n  BOOL result; // eax\n  int v4; // [esp+8h] [ebp-8h] BYREF\n  int v5; // [esp+Ch]`

### CUIStatusBar::SendClaim  (945 B)

  - `{"addr":"0x8d4a22","code":"int __thiscall CUIStatusBar::SendClaim(CUIStatusBar *this)\n{\n  int *ChatLog; // [esp-8h] [ebp-1Ch]\n  _DWORD v3[2]; // [esp-4h] [ebp-18h] BYREF\n  int v4; // [esp+4h] [ebp`

### CUIStatusBar::SendGroupMessage  (8747 B)

- 分支 case：`0`, `1`, `2`, `3`
- SP_ 节点：`SP_158_EITHER_THE_PARTY_DOESNT_EXIST_OR_NO_MEMBER_OF_YOUR_PARTY_IS_LOGGED_ON`, `SP_160_YOU_HAVE_NO_BUDDY_AVAILABLE_FOR_CHAT`, `SP_161_YOU_ARE_NOT_IN_A_GUILD_YET_OR_NONE_OF_YOUR_GUILD_MEMBER_IS_ONLINE`, `SP_1796_S__S`

### CUIStatusBar::SetButtonBlink  (16726 B)

- SP_ 节点：`SP_1986_UI_STATUSBARIMG_EQUIPKEY_ANI`, `SP_1987_UI_STATUSBARIMG_INVENKEY_ANI`, `SP_1988_UI_STATUSBARIMG_STATKEY_ANI`, `SP_1989_UI_STATUSBARIMG_SKILLKEY_ANI`

### CUIStatusBar::SetChatType  (5017 B)

  - `{"addr":"0x8d4a6d","code":"signed __int64 *__thiscall CUIStatusBar::SetChatType(CUIStatusBar *this, signed __int64 *nType)\n{\n  signed __int64 *result; // eax\n  _DWORD *v4; // edx\n  Concurrency::de`

### CUIStatusBar::SetProblem  (485 B)

  - `{"addr":"0x8d5b2b","code":"void __thiscall CUIStatusBar::SetProblem(CUIStatusBar *this, const char *a2)\n{\n  if ( a2 != 0 )\n    CFloatNotice::CreateFloatNotice(this: (this + 3240), a2, a3: 0, a4: 0x`

### CUIStatusBar::SetShortcutTooltip  (5778 B)

- 分支 case：`35`, `17`, `18`, `33`, `34`, `1`, `2`, `3`
- SP_ 节点：`SP_2402_EQUIPS`, `SP_2403_ITEMS`, `SP_2404_ABILITYS`, `SP_2405_SKILLS`

### CUIStatusBar::SetStatusValue  (6389 B)


### CUIStatusBar::SetWhisperTarget  (550 B)


### CUIStatusBar::StartChat  (812 B)

  - `{"addr":"0x8d5a4d","code":"int __thiscall CUIStatusBar::StartChat(int *this, char *a2)\n{\n  int result; // eax\n\n  result = *(this + 832);\n  if ( result == 1 )\n  {\n    CUIStatusBar::SetChatType(t`

### CUIStatusBar::ToggleQuickSlot  (10652 B)

- SP_ 节点：`SP_1984_UI_STATUSBARIMG_QUICKSLOT`, `SP_1985_UI_STATUSBARIMG_QUICKSLOTD`, `SP_1999_STRING_TOOLTIPHELPIMG_GAME_BUTTON_QUICKSLOT`, `SP_2000_STRING_TOOLTIPHELPIMG_GAME_BUTTON_QUICKSLOTD`

### CUIStatusBar::TryBeginWhisper  (7060 B)


### CUIStatusBar::Update  (2357 B)

  - `{"addr":"0x8de964","code":"void __thiscall CUIStatusBar::Update(CUIStatusBar *this)\n{\n  signed int v2; // ebp\n  int v3; // eax\n  int v4; // eax\n  _DWORD *v5; // edi\n  int v6; // eax\n  int *v7; `

## 3. 坐标 / 尺寸 / 按钮 id 候选行（HUD 全函数）


### CChatBalloon::ADBoardMouseDown
  - `{"addr":"0x48e51c","code":"void *__thiscall CChatBalloon::ADBoardMouseDown(CChatBalloon *this, struct tagPOINT a2)\n{\n  void *result; // eax\n  bool v4; // zf\n  int v5; // eax\n  int v6; // ebx\n  int v7; // eax\n  int`

### CChatBalloon::ADBoardMouseUp
  - `{"addr":"0x48e631","code":"int __thiscall CChatBalloon::ADBoardMouseUp(CChatBalloon *this, struct tagPOINT a2)\n{\n  if ( *(this + 2) == 0 || *(this + 15) == 0 )\n    return 0;\n  *(this + 15) = 0;\n  return sub_48E654(t`

### CChatBalloon::AdjustCoordY
  - `{"addr":"0x48e85a","code":"void *__thiscall CChatBalloon::AdjustCoordY(CChatBalloon *this, int a2)\n{\n  void *result; // eax\n  bool v4; // zf\n  int v5; // eax\n  _DWORD *v6; // ecx\n  int v7; // edx\n  int v8; // eax\`

### CChatBalloon::CheckTimeOut
  - `{"addr":"0x48e6d4","code":"void *__thiscall CChatBalloon::CheckTimeOut(CChatBalloon *this, int a2)\n{\n  void *result; // eax\n  _DWORD *v4; // ebx\n  int v5; // esi\n  int v6; // ecx\n  int v7; // eax\n  int v8; // eax\`

### CChatBalloon::GetMiniRoomBalloonRect
  - `{"addr":"0x48e136","code":"int __thiscall CChatBalloon::GetMiniRoomBalloonRect(CChatBalloon *this, struct tagRECT *a2)\n{\n  bool v4; // zf\n  int *Canvas; // edi\n  int *v6; // eax\n  int v7; // edi\n  int v8; // eax\n `

### CChatBalloon::MakeADBoardBalloon
  - `{"addr":"0x48e298","code":"_DWORD *__thiscall CChatBalloon::MakeADBoardBalloon(CChatBalloon *this, int a2, _DWORD *a3, _DWORD *a4, int a5)\n{\n  int v6; // ecx\n  int v7; // ecx\n  _DWORD *result; // eax\n  int v9; // [e`

### CChatBalloon::MakeBalloon
  - `{"addr":"0x48a935","code":"int __thiscall CChatBalloon::MakeBalloon(\n        CChatBalloon *this,\n        int a2,\n        int a3,\n        int a4,\n        int a5,\n        char *a6,\n        int *a7,\n        int **in`

### CChatBalloon::MakeMiniRoomBalloon
  - `a9 = Ztl_variant_t::GetUnknown(a1: v285[8], a2: v285[12]);`

### CChatBalloon::MakeMobBalloon
  - `{"addr":"0x48b4ff","code":"int __thiscall CChatBalloon::MakeMobBalloon(\n        CChatBalloon *this,\n        int a2,\n        int a3,\n        int a4,\n        int *a5,\n        int *p_p_pvargDest,\n        int a7)\n{\n`

### CChatBalloon::MakeScreenBalloon
  - `{"addr":"0x48b86a","code":"int __fastcall CChatBalloon::MakeScreenBalloon(_DWORD *a1, int a2, int a3, int a4, int a5, int a6)\n{\n  _DWORD *Instance; // eax\n  VARIANTARG *Item; // eax\n  int int32; // esi\n  int v10; //`

### CClock::0INITPARAM
  - `{"addr":"0x52bf42","code":"_DWORD *__thiscall CClock::INITPARAM::INITPARAM(_DWORD *this)\n{\n  _DWORD *Instance; // eax\n  _DWORD *String; // eax\n  _DWORD *v4; // eax\n  _DWORD *v5; // eax\n  int v7; // [esp+10h] [ebp-1`

### CClock::SetTimer
  - `{"addr":"0x49ae6a","code":"void __thiscall CClock::SetTimer(CClock *this, int a2)\n{\n  if ( *(this + 30) == 0 )\n  {\n    *(this + 32) = a2;\n    CWnd::InvalidateRect(this, a2: 0);\n  }\n}","refs":[{"addr":"0x9e04c9","n`

### CClock::Start
  - `{"addr":"0x49aeee","code":"void __thiscall CClock::Start(CClock *this)\n{\n  if ( *(this + 27) != 0 && *(this + 30) == 0 )\n  {\n    *(this + 30) = 1;\n    *(this + 31) = 1000 * *(this + 32) + (dword_BF060C)();\n  }\n}",`

### CConfig::GetShowPartyHP
  - `{"addr":"0x49f1ac","code":"int __thiscall CConfig::GetShowPartyHP(CConfig *this)\n{\n  _DWORD *Instance; // eax\n  const char *v3; // eax\n  int v4; // eax\n  int v5; // esi\n  int v7; // [esp+4h] [ebp-10h] BYREF\n  int `

### CCtrlButton::ChangeDisplayState
  - `{"addr":"0x4c0bed","code":"int __thiscall CCtrlButton::ChangeDisplayState(CCtrlButton *this, int a2)\n{\n  int v2; // edi\n  _DWORD *v4; // edi\n  int v5; // ecx\n  _DWORD *Instance; // eax\n  void *v7; // eax\n  _DWORD `

### CCtrlButton::Destroy
  - `{"addr":"0x4c0554","code":"void __thiscall CCtrlButton::Destroy(CCtrlButton *this)\n{\n  if ( this == dword_BED5D8 )\n    dword_BED5D8 = 0;\n  sub_428712(a1: 0);\n  CCtrlWnd::Destroy(this);\n}","refs":[{"addr":"0xbed5d8"`

### CCtrlButton::MouseDown
  - `{"addr":"0x4c1059","code":"int __thiscall CCtrlButton::MouseDown(CCtrlButton *this)\n{\n  int result; // eax\n\n  result = (*(*(this + 1) + 32))(a1: this + 4);\n  if ( result != 0 )\n  {\n    *(this + 21) = 0;\n    *(thi`

### CCtrlButton::MouseEnter
  - `{"addr":"0x4c0ad9","code":"void __fastcall CCtrlButton::MouseEnter(CCtrlButton *this, int a2, int a3, int a4)\n{\n  int v5; // ebx\n  int v6; // eax\n  int v7; // eax\n  _DWORD *Instance; // eax\n  wchar_t **StringW; // `

### CCtrlButton::MouseUp
  - `{"addr":"0x4c107d","code":"int __thiscall CCtrlButton::MouseUp(CCtrlButton *this)\n{\n  int result; // eax\n  _DWORD *Instance; // eax\n  wchar_t *v4; // [esp-4h] [ebp-1Ch]\n  _BYTE v5[4]; // [esp+8h] [ebp-10h] BYREF\n  `

### CCtrlButton::OnMouseButton
  - `{"addr":"0x4c09e3","code":"void __thiscall CCtrlButton::OnMouseButton(CCtrlButton *this, unsigned int a2, unsigned int a3, int a4, int a5)\n{\n  int v6; // eax\n  int v7; // eax\n  int v8; // eax\n\n  v6 = *(this + 18);\`

### CCtrlButton::OnMouseEnter
  - `{"addr":"0x4c0ac1","code":"void __thiscall CCtrlButton::OnMouseEnter(CCtrlButton *this, int a2)\n{\n  if ( a2 == 0 )\n  {\n    *(this + 17) = 0;\n    CCtrlButton::MouseEnter(this: (this - 4), a2: 0, a3: 0);\n  }\n}","ref`

### CCtrlButton::OnMouseMove
  - `{"addr":"0x4c0a72","code":"int __thiscall CCtrlButton::OnMouseMove(CCtrlButton *this, int a2, int a3)\n{\n  int v3; // eax\n\n  v3 = *(this + 18);\n  if ( v3 > a2 || a2 >= *(this + 6) - v3 || v3 > a3 || a3 >= *(this + 7)`

### CCtrlButton::SetKeyFocus
  - `{"addr":"0x4c0e3f","code":"int __thiscall CCtrlButton::SetKeyFocus(CCtrlButton *this, _DWORD *a2)\n{\n  int result; // eax\n  int Layer; // eax\n  void *v5; // eax\n  int v6; // ecx\n  int v7; // eax\n  int v8; // eax\n `

### CCtrlComboBox::AddItem
  - `{"addr":"0x4c6dd6","code":"int __thiscall CCtrlComboBox::AddItem(char *this, char *a2, int a3)\n{\n  int v4; // edi\n  int result; // eax\n\n  v4 = sub_4C7755(a1: this + 52);\n  ZXString<char>::GetBuffer(a1: a2, a2: 0xFF`

### CCtrlComboBox::Draw
  - `{"addr":"0x4c5fb9","code":"VARIANTARG **__thiscall CCtrlComboBox::Draw(int *this, int rx, int ry, int pRect)\n{\n  int v5; // eax\n  bool v6; // zf\n  _DWORD *v7; // eax\n  VARIANTARG *v8; // ecx\n  int v9; // eax\n  int`

### CCtrlComboBox::GetComboBoxUOL
  - `{"addr":"0x4c4552","code":"int __stdcall CCtrlComboBox::GetComboBoxUOL(int a1, int a2)\n{\n  _DWORD *Instance; // eax\n  _DWORD *BSTR; // eax\n  int result; // eax\n  int v5; // [esp+4h] [ebp-4h] BYREF\n\n  Instance = St`

### CCtrlComboBox::SetSelect
  - `{"addr":"0x4c3a0d","code":"StringPoolStrings *__thiscall CCtrlComboBox::SetSelect(int *this, int a2, int a3, int a4)\n{\n  bool v5; // zf\n  int v6; // eax\n  StringPoolStrings *v7; // ebx\n  int v8; // eax\n  StringPool`

### CCtrlEdit::0CREATEPARAM
  - `{"addr":"0x4c8d5f","code":"CCtrlEdit::CREATEPARAM *__thiscall CCtrlEdit::CREATEPARAM::CREATEPARAM(CCtrlEdit::CREATEPARAM *this)\n{\n  _DWORD *Instance; // eax\n  _DWORD *BSTR; // eax\n  int v5; // [esp+10h] [ebp-10h] BYR`

### CCtrlEdit::1CREATEPARAM
  - `{"addr":"0x471b56","code":"void __thiscall CCtrlEdit::CREATEPARAM::~CREATEPARAM(CCtrlEdit::CREATEPARAM *this)\n{\n  int v2; // ecx\n\n  v2 = *(this + 5);\n  if ( v2 != 0 )\n  {\n    _bstr_t::Data_t::Release(a1: v2);\n   `

### CCtrlEdit::AdjustHorz
  - `{"addr":"0x4cb829","code":"void __thiscall CCtrlEdit::AdjustHorz(CCtrlEdit *this, int a2, VARIANTARG **a3, int a4)\n{\n  bool v5; // zf\n  int v6; // eax\n  size_t v7; // ecx\n  int v8; // eax\n  int v9; // eax\n  int v1`

### CCtrlEdit::GetText
  - `{"addr":"0x471353","code":"_DWORD *__thiscall CCtrlEdit::GetText(char *this, _DWORD *a2)\n{\n  *a2 = 0;\n  ZXString<char>::operator=(a1: this + 52);\n  return a2;\n}","refs":[{"addr":"0x4181c9","name":"??4?$ZXString@D@@Q`

### CCtrlEdit::InsertString
  - `{"addr":"0x4ca89e","code":"int __thiscall CCtrlEdit::InsertString(signed int *this, char *a2, int a3, int a4)\n{\n  int v5; // ebx\n  bool v6; // zf\n  int v8; // edi\n  signed int v9; // eax\n  int v10; // ecx\n  signed`

### CCtrlEdit::OnCreate
  - `{"addr":"0x4ca25d","code":"int __thiscall CCtrlEdit::OnCreate(CCtrlEdit *this, int *a2)\n{\n  int *v2; // edi\n  int v4; // eax\n  int v5; // ecx\n  int v6; // eax\n  int v7; // ecx\n  VARIANTARG *p_pvarg; // eax\n  int `

### CCtrlEdit::OnSetFocus
  - `{"addr":"0x4ca043","code":"signed int __thiscall CCtrlEdit::OnSetFocus(CCtrlEdit *this, int a2)\n{\n  int v3; // eax\n\n  if ( a2 != 0 )\n    v3 = *(this + 9);\n  else\n    v3 = 1;\n  if ( v3 != 0 )\n  {\n    if ( a2 != `

### CCtrlEdit::SetClipboard
  - `{"addr":"0x4caef1","code":"void __thiscall CCtrlEdit::SetClipboard(CCtrlEdit *this)\n{\n  int v1; // edx\n  int v2; // edi\n  int v3; // eax\n  _DWORD *v4; // esi\n  int v5; // eax\n  int v6; // eax\n  int v7; // [esp+8h`

### CCtrlEdit::SetText
  - `{"addr":"0x4cc512","code":"void __thiscall CCtrlEdit::SetText(int *this, char *a2)\n{\n  char *v2; // eax\n  int *v4; // esi\n  int v5; // esi\n  int v6; // esi\n\n  v2 = a2;\n  if ( a2 == 0 )\n    v2 = &sStrDefault;\n  `

### CCtrlMLEdit::CreateIMECandWnd
  - `{"addr":"0x4d56b4","code":"void *__thiscall CCtrlMLEdit::CreateIMECandWnd(_DWORD *this, int a2, int a3, int a4, int a5, unsigned int a6)\n{\n  void *result; // eax\n  int v8; // eax\n  int v9; // edi\n  int v10; // ecx\n`

### CCtrlMLEdit::DestroyIMECandWnd
  - `{"addr":"0x4d5be0","code":"int __thiscall CCtrlMLEdit::DestroyIMECandWnd(CCtrlMLEdit *this)\n{\n  int v2; // ecx\n  int result; // eax\n\n  v2 = *(this + 48);\n  if ( v2 != 0 )\n  {\n    CWnd::Destroy(a1: v2);\n    retur`

### CCtrlMLEdit::ExtractSelection
  - `{"addr":"0x4d3f2e","code":"int __fastcall CCtrlMLEdit::ExtractSelection(CCtrlMLEdit *this, int a2, int a3)\n{\n  int v3; // edx\n  int *v4; // esi\n  int *v5; // edi\n  int v6; // eax\n  _DWORD *v7; // ebx\n  int v8; // `

### CCtrlMLEdit::GetText
  - `{"addr":"0x6f2084","code":"_DWORD *__thiscall CCtrlMLEdit::GetText(char *this, _DWORD *a2)\n{\n  *a2 = 0;\n  ZXString<char>::operator=(a1: this + 52);\n  return a2;\n}","refs":[{"addr":"0x4181c9","name":"??4?$ZXString@D@`

### CCtrlMLEdit::InsertString
  - `{"addr":"0x4d3b02","code":"int __thiscall CCtrlMLEdit::InsertString(signed int *this, char *a2, int a3, int a4)\n{\n  int v5; // ebx\n  bool v6; // zf\n  int v8; // edi\n  signed int v9; // eax\n  int v10; // ecx\n  sign`

### CCtrlMLEdit::MoveCaret
  - `{"addr":"0x4d4aa8","code":"void __thiscall CCtrlMLEdit::MoveCaret(CCtrlMLEdit *this, int a2)\n{\n  int v3; // eax\n  int v4; // eax\n\n  switch ( a2 )\n  {\n    case 0:\n      *(this + 16) -= sub_4D4B18(a1: this);\n     `

### CCtrlMLEdit::OnCreate
  - `{"addr":"0x4d349e","code":"int __thiscall CCtrlMLEdit::OnCreate(CCtrlMLEdit *this, int *a2)\n{\n  int *v2; // edi\n  int v4; // eax\n  int v5; // ecx\n  int v6; // eax\n  int v7; // ecx\n  VARIANTARG *p_pvarg; // eax\n  `

### CCtrlMLEdit::OnIMEComp
  - `{"addr":"0x4d33c2","code":"void __thiscall CCtrlMLEdit::OnIMEComp(_DWORD *this, char *a2, int *a3, int a4, int a5, int a6, int a7, int a8, int a9)\n{\n  unsigned int i; // ebx\n  int v11; // eax\n  void *v12; // ecx\n  v`

### CCtrlMLEdit::OnKey
  - `{"addr":"0x4d3159","code":"void __thiscall CCtrlMLEdit::OnKey(CCtrlMLEdit *this, unsigned int a2, int a3)\n{\n  void (__stdcall **v4)(unsigned int, int); // eax\n  CCtrlWnd *v5; // esi\n  void *v6; // ecx\n  void *v7; //`

### CCtrlMLEdit::OnSetFocus
  - `{"addr":"0x4d32ab","code":"signed int __thiscall CCtrlMLEdit::OnSetFocus(CCtrlMLEdit *this, int a2)\n{\n  int v3; // eax\n\n  if ( a2 != 0 )\n    v3 = *(this + 9);\n  else\n    v3 = 1;\n  if ( v3 != 0 )\n  {\n    if ( a2`

### CCtrlMLEdit::SetClipboard
  - `{"addr":"0x4d406b","code":"void __thiscall CCtrlMLEdit::SetClipboard(CCtrlMLEdit *this)\n{\n  int v1; // edx\n  int v2; // edi\n  int v3; // eax\n  _DWORD *v4; // esi\n  int v5; // eax\n  int v6; // eax\n  int v7; // [es`

### CCtrlScrollBar::CreateCtrl
  - `{"addr":"0x4d7037","code":"int __thiscall CCtrlScrollBar::CreateCtrl(char *this, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9)\n{\n  int v9; // eax\n  int v11; // ecx\n  int v12; // eax\n  int v13; // e`

### CCtrlScrollBar::DoPrevNext
  - `{"addr":"0x4d8145","code":"void __thiscall CCtrlScrollBar::DoPrevNext(CCtrlScrollBar *this, int a2)\n{\n  *(this + 14) += a2;\n  CCtrlScrollBar::ValidatePosAndRange(this);\n  CCtrlWnd::InvalidateRect(this);\n  if ( a2 >=`

### CCtrlScrollBar::DoPrevNextBar
  - `{"addr":"0x4d8189","code":"void __thiscall CCtrlScrollBar::DoPrevNextBar(CCtrlScrollBar *this, int a2)\n{\n  int v3; // eax\n  int v4; // edi\n\n  v3 = *(this + 15) / 16;\n  if ( v3 < 16 )\n    v3 = 16;\n  v4 = a2 * v3;\`

### CCtrlScrollBar::DoThumbEnd
  - `{"addr":"0x4d827f","code":"int __thiscall CCtrlScrollBar::DoThumbEnd(CCtrlScrollBar *this, int a2, int a3)\n{\n  int result; // eax\n  int v5; // eax\n  int v6; // edi\n\n  result = *(this + 15);\n  if ( result > 1 )\n  `

### CCtrlScrollBar::DoThumbMove
  - `{"addr":"0x4d8203","code":"int __thiscall CCtrlScrollBar::DoThumbMove(CCtrlScrollBar *this, int a2, int a3)\n{\n  int result; // eax\n  int v5; // eax\n  int v6; // edi\n\n  result = *(this + 15);\n  if ( result > 1 )\n `

### CCtrlScrollBar::Draw
  - `{"addr":"0x4d74e4","code":"int __thiscall CCtrlScrollBar::Draw(char *this, int a2, int a3, int a4)\n{\n  int v5; // eax\n  int v6; // eax\n  int v7; // eax\n  int v8; // eax\n  int v9; // eax\n  int v10; // edi\n  int v1`

### CCtrlScrollBar::GetScrollBarBaseUOL
  - `{"addr":"0x4d6d96","code":"int __cdecl CCtrlScrollBar::GetScrollBarBaseUOL(int hv, int type, int sUOL)\n{\n  _DWORD *Instance; // eax\n  int StringW; // eax\n  int *p_sUOL; // ecx\n  _DWORD *v6; // eax\n  int v7; // eax\`

### CCtrlScrollBar::GetScrollBarCanvas
  - `{"addr":"0x4d7145","code":"int __thiscall CCtrlScrollBar::GetScrollBarCanvas(char *this, int a2, int Unknown)\n{\n  _DWORD *Instance; // eax\n  _DWORD *v4; // ecx\n  int v5; // eax\n  int v6; // eax\n  _DWORD *v7; // eax`

### CCtrlScrollBar::GetScrollBarSize
  - `{"addr":"0x4d6e7a","code":"int __cdecl CCtrlScrollBar::GetScrollBarSize(int hv, int type)\n{\n  _DWORD *Instance; // eax\n  _DWORD *v3; // ecx\n  int v4; // eax\n  wchar_t *v5; // eax\n  int v6; // ecx\n  int v7; // eax\`

### CCtrlScrollBar::MouseUp
  - `{"addr":"0x4d7ed8","code":"void __thiscall CCtrlScrollBar::MouseUp(CCtrlScrollBar *this, int a2, int a3)\n{\n  int v4; // eax\n\n  if ( *(this + 15) > 1 && *(this + 18) != 0 )\n  {\n    *(this + 18) = 0;\n    CWndMan::Re`

### CCtrlScrollBar::OnMouseMove
  - `{"addr":"0x4d7de7","code":"int __thiscall CCtrlScrollBar::OnMouseMove(CCtrlScrollBar *this, int a2, int a3)\n{\n  if ( *(this + 14) <= 1 || *(this + 17) == 0 || *(this + 15) != 105 )\n    return 0;\n  CCtrlScrollBar::DoT`

### CCtrlScrollBar::ValidatePosAndRange
  - `{"addr":"0x4d7dc6","code":"signed int __thiscall CCtrlScrollBar::ValidatePosAndRange(CCtrlScrollBar *this)\n{\n  signed int result; // eax\n  int v2; // edx\n\n  result = *(this + 15);\n  if ( result <= 1 )\n  {\n    *(t`

### CFloatNotice::CreateFloatNotice
  - `{"addr":"0x58c53b","code":"void __thiscall CFloatNotice::CreateFloatNotice(CFloatNotice *this, const char *a2, int *a3, IWzGr2DLayer **a4, int *i)\n{\n  int v5; // esi\n  _DWORD *Instance; // eax\n  wchar_t *v7; // eax\n`

### CQuickslotKeyMappedMan::OnInit
  - `{"addr":"0x72b82a","code":"int __thiscall CQuickslotKeyMappedMan::OnInit(char *this, int a2)\n{\n  if ( CInPacket::Decode1(this: a2) != 0 )\n    CInPacket::DecodeBuffer(a1: this + 4, a2: 0x20u);\n  else\n    CQuickslotKe`

### CQuickslotKeyMappedMan::SaveQuickslotKeyMap
  - `{"addr":"0x72b87e","code":"int __thiscall CQuickslotKeyMappedMan::SaveQuickslotKeyMap(CQuickslotKeyMappedMan *this)\n{\n  _BYTE v3[4]; // [esp+8h] [ebp-1Ch] BYREF\n  _BYTE v4[12]; // [esp+Ch] [ebp-18h] BYREF\n  int v5; /`

### CSlideNotice::OnCreate
  - `{"addr":"0x7e1686","code":"int __thiscall CSlideNotice::OnCreate(CSlideNotice *this, void *a2)\n{\n  _DWORD *Canvas; // eax\n  int v3; // esi\n  int v4; // eax\n  int result; // eax\n  int v6; // [esp+4h] [ebp-10h] BYREF`

### CSlideNotice::SetMsg
  - `{"addr":"0x7e16fe","code":"int __thiscall CSlideNotice::SetMsg(_DWORD **this, int a2)\n{\n  int **v2; // esi\n  int **v3; // eax\n  int v4; // eax\n  int v5; // eax\n  int *v6; // edi\n  int v7; // eax\n  int *Layer; // `

### CUIEnergyBar::OnMouseMove
  - `{"addr":"0x7faa49","code":"int __thiscall CUIEnergyBar::OnMouseMove(CUIEnergyBar *this, int a2, int a3)\n{\n  _DWORD *Instance; // eax\n  int v5; // eax\n  int v6; // eax\n  int v8; // [esp-10h] [ebp-24h]\n  CUIEnergyBar`

### CUIEnergyBar::SetEnergy
  - `{"addr":"0x7f9bad","code":"int __thiscall CUIEnergyBar::SetEnergy(ULONG **this, ULONG *arg0, int a2)\n{\n  int result; // eax\n  bool v4; // zf\n  int v5; // edx\n  _DWORD *Instance; // eax\n  int v7; // eax\n  int v8; /`

### CUIGameOpt::GetSysOptFromCtrl
  - `{"addr":"0x9940bd","code":"int __thiscall CUIGameOpt::GetSysOptFromCtrl(CUIGameOpt *this)\n{\n  int result; // eax\n\n  *(this + 78) = *(*(this + 34) + 72);\n  *(this + 79) = *(*(this + 36) + 72);\n  *(this + 80) = *(*(t`

### CUIKeyConfig::GetShortCutIndexByPos
  - `{"addr":"0x833a65","code":"int __thiscall CUIKeyConfig::GetShortCutIndexByPos(CUIKeyConfig *this, int a2, int a3)\n{\n  int *v4; // esi\n  int v5; // ecx\n  _DWORD v7[4]; // [esp+Ch] [ebp-10h] BYREF\n  int v8; // [esp+24`

### CUIMessenger::DrawStatusBar
  - `{"addr":"0x850ec9","code":"_DWORD *__thiscall CUIMessenger::DrawStatusBar(wchar_t ***this, _DWORD *a2)\n{\n  wchar_t *v2; // ebx\n  wchar_t ***v3; // esi\n  size_t v4; // eax\n  _DWORD *result; // eax\n  int v6; // [esp+`

### CUIMessenger::SetCtrl
  - `{"addr":"0x8505e2","code":"void __thiscall CUIMessenger::SetCtrl(CUIMessenger *this)\n{\n  BOOL v2; // eax\n  int v3; // ecx\n  _DWORD *Instance; // eax\n  int StringW; // eax\n  _DWORD *v6; // ecx\n  _DWORD *v7; // eax\`

### CUIMiniMap::LoadMiniMap
  - `{"addr":"0x858603","code":"signed int __thiscall CUIMiniMap::LoadMiniMap(int *this, _DWORD *a2, _DWORD *a3, VARIANTARG **p_p_pvarg)\n{\n  bool v5; // zf\n  VARIANTARG *v6; // ecx\n  _DWORD *Instance; // eax\n  VARIANTARG`

### CUIMiniMap::OnButtonClicked
  - `{"addr":"0x85ace7","code":"void __userpurge CUIMiniMap::OnButtonClicked(CUIMiniMap *this@<ecx>, int a2@<ebx>, unsigned int a3)\n{\n  int field; // edi\n  int Data; // eax\n  int v5; // ecx\n  bool v6; // zf\n  _DWORD *In`

### CUIMiniMap::OnMouseMove
  - `{"addr":"0x85ade2","code":"signed int __thiscall CUIMiniMap::OnMouseMove(wchar_t **this, int arg0, int a3)\n{\n  int v4; // eax\n  int v5; // eax\n  wchar_t **v6; // edi\n  wchar_t *v7; // edi\n  int v8; // eax\n  int v9`

### CUIMiniMap::SetCtrl
  - `{"addr":"0x858344","code":"void __usercall CUIMiniMap::SetCtrl(_DWORD *a1@<ecx>, int a2@<ebx>)\n{\n  int v3; // ecx\n  int v4; // ecx\n  int v5; // ecx\n  _DWORD *Instance; // eax\n  int StringW; // eax\n  _DWORD *v8; //`

### CUIMiniMap::SetLayer
  - `HIDWORD(v326[0]) = *CItemInfo::GetMapString(this: dword_BE78D8, a2: &v356, a3: v326[0], a4: SBYTE4(v326[0]));`
  - `v338 = *IWzProperty::GetItem(pvargDest: &v333, a2: SHIDWORD(v326[0])) != 0;`
  - `Unknown = Ztl_variant_t::GetUnknown(a1: v326[0], a2: SBYTE4(v326[0]));`
  - `v30 = *StringPool::GetStringW(this: v29, a2: v326[0], a3: SHIDWORD(v326[0]));`
  - `PcCreateObject::IWzFont(sUOL: v30, pObj: this + 408, pUnkOuter: 0);`
  - `BSTR = StringPool::GetBSTR(this: v31, a2: v326[0], a3: SHIDWORD(v326[0]));`
  - `v39 = StringPool::GetBSTR(this: v38, a2: v326[0], a3: SHIDWORD(v326[0]));`
  - `v356 = Ztl_variant_t::GetUnknown(a1: v326[0], a2: SBYTE4(v326[0]));`

### CUIPartyHP::Create
  - `{"addr":"0x91f934","code":"_DWORD *__thiscall CUIPartyHP::Create(int *this)\n{\n  int v1; // esi\n  int v3; // eax\n  int v4; // eax\n  int *v5; // ecx\n  int v6; // eax\n  int v7; // eax\n  int v8; // esi\n  _DWORD *res`

### CUIShortCutMenu::OnCreate
  - `{"addr":"0x84a6bc","code":"int __thiscall CUIShortCutMenu::OnCreate(CUIShortCutMenu *this, void *a2)\n{\n  _DWORD *Instance; // eax\n  int StringW; // eax\n  _DWORD *v5; // eax\n  _DWORD *v6; // eax\n  _DWORD *v7; // eax`

### CUIShortCutMenu::ResetButtonState
  - `{"addr":"0x84af14","code":"void __thiscall CUIShortCutMenu::ResetButtonState(CUIShortCutMenu *this)\n{\n  int v2; // ebx\n  CCtrlButton **v3; // esi\n  BOOL v4; // eax\n\n  v2 = 0;\n  v3 = (this + 216);\n  do\n  {\n    v`

### CUIStatusBar::0CChatLog
  - `{"addr":"0x8db485","code":"_DWORD *__thiscall CUIStatusBar::CChatLog::CChatLog(\n        _DWORD *this,\n        char sChat,\n        int nType,\n        int nBack,\n        int nChannelID,\n        int bWhisperIcon,\n   `

### CUIStatusBar::ChangeChatWndSize
  - `{"addr":"0x8d616c","code":"int __thiscall CUIStatusBar::ChangeChatWndSize(CUIStatusBar *this, int a2, int a3, int a4)\n{\n  int result; // eax\n  int v6; // edx\n  int v7; // ecx\n  int v9; // eax\n  int v10; // edi\n  B`

### CUIStatusBar::ChangeWhisperTarget
  - `{"addr":"0x8d4dbf","code":"signed int __thiscall CUIStatusBar::ChangeWhisperTarget(CUIStatusBar *this)\n{\n  CUtilDlgEx *v2; // ecx\n  int v3; // ebx\n  CUtilDlgEx *v4; // eax\n  int v5; // ecx\n  StringPoolStrings v6; /`

### CUIStatusBar::ChatLogAdd
  - `{"addr":"0x8db070","code":"void __thiscall CUIStatusBar::ChatLogAdd(\n        char *this,\n        int sChat,\n        int lType,\n        int nChannelID,\n        int bWhisperIcon,\n        int a6,\n        VARIANTARG *`

### CUIStatusBar::ChatLogDraw
  - `Data = TSecType<long>::GetData(a1: (lVal + 12), a2: 0);`
  - `v76 = TSecType<long>::GetData(a1: (v75 + 12), a2: 0);`

### CUIStatusBar::ConvertWhisperToNormal
  - `{"addr":"0x8d4348","code":"_DWORD *__stdcall CUIStatusBar::ConvertWhisperToNormal(_DWORD *a1, _WORD *a2)\n{\n  _WORD *v2; // eax\n  bool v3; // zf\n  int i; // esi\n  int v5; // eax\n  __int16 v6; // cx\n  int v7; // eax`

### CUIStatusBar::Draw
  - `{"addr":"0x8d80a4","code":"VARIANTARG *__thiscall CUIStatusBar::Draw(CUIStatusBar *this, const struct tagRECT *pRect)\n{\n  _DWORD *v2; // edi\n  BOOL v3; // esi\n  VARIANTARG *result; // eax\n  int v5; // esi\n  int fie`

### CUIStatusBar::EndChat
  - `{"addr":"0x8d5aaf","code":"int __thiscall CUIStatusBar::EndChat(CUIStatusBar *this)\n{\n  if ( *(this + 832) == 2 )\n    CUIStatusBar::SetChatType(this, nType: 1);\n  return CWndMan::SetFocus(a1: 0);\n}","refs":[{"addr":`

### CUIStatusBar::FloatNotice
  - `{"addr":"0x8d5acd","code":"void __thiscall CUIStatusBar::FloatNotice(CUIStatusBar *this, const char *a2, char a3, unsigned int a4)\n{\n  if ( a2 != 0 )\n  {\n    if ( a4 != 0 )\n      *(this + 813) = a4 + (dword_BF060C)(`

### CUIStatusBar::GetChatLog
  - `{"addr":"0x8d468d","code":"int *__thiscall CUIStatusBar::GetChatLog(void *this, int *a2)\n{\n  unsigned int v2; // ecx\n  int v3; // esi\n  int v4; // eax\n  int v5; // eax\n  _WORD *v6; // ecx\n  _DWORD *v7; // eax\n  i`

### CUIStatusBar::GetEmotionKey
  - `{"addr":"0x8d57ca","code":"int __stdcall CUIStatusBar::GetEmotionKey(int a1)\n{\n  _DWORD *Instance; // eax\n  char v2; // bl\n  _DWORD *v3; // eax\n  _DWORD *v4; // eax\n  _DWORD *v5; // eax\n  _DWORD *v6; // eax\n  boo`

### CUIStatusBar::GetShortCutIndexByPos
  - `{"addr":"0x8de8d5","code":"signed int __stdcall CUIStatusBar::GetShortCutIndexByPos(int a1, int a2)\n{\n  int v3; // edi\n  int *v4; // esi\n  int v5; // ecx\n  _DWORD v7[4]; // [esp+Ch] [ebp-10h] BYREF\n  int v8; // [es`

### CUIStatusBar::GetWhisperTarget
  - `{"addr":"0x4c6db2","code":"_DWORD *__thiscall CUIStatusBar::GetWhisperTarget(char *this, _DWORD *a2)\n{\n  *a2 = 0;\n  ZXString<char>::operator=(a1: this + 3156);\n  return a2;\n}","refs":[{"addr":"0x4181c9","name":"??4?`

### CUIStatusBar::HitTest
  - `{"addr":"0x8de7da","code":"int __thiscall CUIStatusBar::HitTest(CUIStatusBar *this, signed int a2, signed int a3, struct CCtrlWnd **a4)\n{\n  int v5; // eax\n  int v6; // ecx\n  int v7; // edx\n  int v8; // ecx\n  int v1`

### CUIStatusBar::IsKindOf
  - `{"addr":"0x8cfd89","code":"int __thiscall CUIStatusBar::IsKindOf(CUIStatusBar *this, const struct CRTTI *a2)\n{\n  const struct CRTTI *v2; // eax\n\n  v2 = &dword_BF10FC;\n  if ( &dword_BF10FC == 0 )\n    return 0;\n  wh`

### CUIStatusBar::MakeCtrlEdit
  - `{"addr":"0x8dfb36","code":"void *__thiscall CUIStatusBar::MakeCtrlEdit(char **this, volatile LONG *a2)\n{\n  void *result; // eax\n  CCtrlEdit *v4; // eax\n  int v5; // eax\n  CCtrlEdit *v6; // eax\n  int v7; // eax\n  _`

### CUIStatusBar::OnButtonClicked
  - `{"addr":"0x8d423d","code":"void __thiscall CUIStatusBar::OnButtonClicked(CUIStatusBar *this, unsigned int a2)\n{\n  if ( TSingleton<CUserLocal>::ms_pInstance != 0 && *(TSingleton<CUserLocal>::ms_pInstance + 12596) != 0 )`

### CUIStatusBar::OnChildNotify
  - `{"addr":"0x8d419c","code":"void __thiscall CUIStatusBar::OnChildNotify(CUIStatusBar *this, unsigned int a2, unsigned int a3, signed int a4)\n{\n  if ( a2 == 1010 )\n  {\n    if ( a3 < 0x12C || a3 > 0x130 )\n      goto LA`

### CUIStatusBar::OnCreate
  - `basic_font = get_basic_font(a1: (ebp0 - 16), fType: 1);`
  - `v6 = get_basic_font(a1: (ebp0 - 16), fType: 0);`

### CUIStatusBar::OnKey
  - `{"addr":"0x8d536c","code":"void __thiscall CUIStatusBar::OnKey(CUIStatusBar *this, char *a2, char *a3)\n{\n  char *v4; // ebx\n  _DWORD *v5; // ecx\n  _BYTE *v6; // eax\n  BOOL v7; // ebx\n  int v8; // ecx\n  char *v9; /`

### CUIStatusBar::OnMouseButton
  - `{"addr":"0x8d5b5d","code":"void __fastcall CUIStatusBar::OnMouseButton(\n        CUIStatusBar *this,\n        int a2,\n        unsigned int a3,\n        unsigned int a4,\n        int a5,\n        int a6)\n{\n  _DWORD *v7`

### CUIStatusBar::OnMouseMove
  - `{"addr":"0x8d795a","code":"int __thiscall CUIStatusBar::OnMouseMove(CUIStatusBar *this, CY arg0)\n{\n  _DWORD *v2; // esi\n  bool v4; // zf\n  int v5; // eax\n  _DWORD *v6; // esi\n  int **v7; // eax\n  int v8; // eax\n `

### CUIStatusBar::ProcessToolTip
  - `{"addr":"0x8d76d7","code":"void __thiscall CUIStatusBar::ProcessToolTip(CUIStatusBar *this, int a2, int a3)\n{\n  int v4; // eax\n  int v5; // eax\n  int v6; // edi\n  int v7; // esi\n  unsigned __int8 v8; // al\n  bool `

### CUIStatusBar::ResetButtonToolTip
  - `{"addr":"0x8defae","code":"BOOL __thiscall CUIStatusBar::ResetButtonToolTip(CUIStatusBar *this)\n{\n  int v2; // esi\n  BOOL result; // eax\n  int v4; // [esp+8h] [ebp-8h] BYREF\n  int v5; // [esp+Ch] [ebp-4h]\n\n  v2 = `

### CUIStatusBar::SendClaim
  - `{"addr":"0x8d4a22","code":"int __thiscall CUIStatusBar::SendClaim(CUIStatusBar *this)\n{\n  int *ChatLog; // [esp-8h] [ebp-1Ch]\n  _DWORD v3[2]; // [esp-4h] [ebp-18h] BYREF\n  int v4; // [esp+4h] [ebp-10h] BYREF\n  int v`

### CUIStatusBar::SendGroupMessage
  - `{"addr":"0x8df155","code":"int __thiscall CUIStatusBar::SendGroupMessage(char *this, signed int a2, VARIANTARG **a3)\n{\n  int v3; // edi\n  int v4; // ecx\n  signed int v5; // esi\n  int v6; // ecx\n  _DWORD *v7; // eax`

### CUIStatusBar::SetButtonBlink
  - `{"addr":"0x8dfe21","code":"int *__thiscall CUIStatusBar::SetButtonBlink(CUIStatusBar *this, unsigned int a2, int a3)\n{\n  int *result; // eax\n  unsigned int v4; // edi\n  int v5; // eax\n  int v6; // eax\n  int v7; // `

### CUIStatusBar::SetChatType
  - `{"addr":"0x8d4a6d","code":"signed __int64 *__thiscall CUIStatusBar::SetChatType(CUIStatusBar *this, signed __int64 *nType)\n{\n  signed __int64 *result; // eax\n  _DWORD *v4; // edx\n  Concurrency::details::SchedulerProx`

### CUIStatusBar::SetProblem
  - `{"addr":"0x8d5b2b","code":"void __thiscall CUIStatusBar::SetProblem(CUIStatusBar *this, const char *a2)\n{\n  if ( a2 != 0 )\n    CFloatNotice::CreateFloatNotice(this: (this + 3240), a2, a3: 0, a4: 0xA0, a5: -1073342723)`

### CUIStatusBar::SetShortcutTooltip
  - `{"addr":"0x8e03f1","code":"void __thiscall CUIStatusBar::SetShortcutTooltip(CUIStatusBar *this, int a2, int a3)\n{\n  _DWORD *v3; // eax\n  char *v4; // eax\n  char **v5; // eax\n  int *v6; // ecx\n  _DWORD *v7; // eax\n`

### CUIStatusBar::SetStatusValue
  - `{"addr":"0x8d82b8","code":"void __thiscall CUIStatusBar::SetStatusValue(char *this, signed int a2, char a3, char a4)\n{\n  int v5; // esi\n  int v6; // eax\n  int *v7; // ecx\n  int v8; // esi\n  int *v9; // ecx\n  char `

### CUIStatusBar::SetWhisperTarget
  - `{"addr":"0x8db01c","code":"void __thiscall CUIStatusBar::SetWhisperTarget(_DWORD *this, char a2, int a3)\n{\n  _BYTE **v4; // esi\n  _BYTE *v5; // esi\n\n  v4 = (this + 789);\n  ZXString<char>::operator=(a1: &a2);\n  v5 `

### CUIStatusBar::StartChat
  - `{"addr":"0x8d5a4d","code":"int __thiscall CUIStatusBar::StartChat(int *this, char *a2)\n{\n  int result; // eax\n\n  result = *(this + 832);\n  if ( result == 1 )\n  {\n    CUIStatusBar::SetChatType(this, nType: 2);\n  }`

### CUIStatusBar::ToggleQuickSlot
  - `{"addr":"0x8df4b1","code":"int __thiscall CUIStatusBar::ToggleQuickSlot(CUIStatusBar *this)\n{\n  BOOL v2; // eax\n  _DWORD *LT; // eax\n  bool v4; // zf\n  int v5; // eax\n  _DWORD *v6; // eax\n  int v7; // eax\n  _DWOR`

### CUIStatusBar::TryBeginWhisper
  - `{"addr":"0x8d5bfb","code":"signed int __thiscall CUIStatusBar::TryBeginWhisper(CUIStatusBar *this, int a2, int a3)\n{\n  bool v4; // cc\n  int v5; // eax\n  int v6; // ecx\n  int v7; // eax\n  int v8; // ecx\n  int v9; /`

### CUIStatusBar::Update
  - `{"addr":"0x8de964","code":"void __thiscall CUIStatusBar::Update(CUIStatusBar *this)\n{\n  signed int v2; // ebp\n  int v3; // eax\n  int v4; // eax\n  _DWORD *v5; // edi\n  int v6; // eax\n  int *v7; // edi\n  int v8; //`

### CUISysOpt::GetSysOptFromCtrl
  - `{"addr":"0x994b1a","code":"int __thiscall CUISysOpt::GetSysOptFromCtrl(CUISysOpt *this)\n{\n  int result; // eax\n\n  *(this + 73) = *(*(this + 32) + 52);\n  *(this + 74) = *(*(this + 34) + 52);\n  *(this + 75) = *(*(thi`
