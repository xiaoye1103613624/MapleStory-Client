# 13 键窗口 · IMG 节点 / 本地按钮引用差集矩阵（口径修正版）

基准：083/v83；IMG 源 `E:/MXD/BeiDou-Client_S9/Data/UI/UIWindow.img`（orange-wz MCP，128B key + `TSPHKw==`，`isError:false`）。
本地源：各窗 `IO/UITypes/UI*.cpp` 中出现的任意 `"Bt*"` 字符串字面量。

> `img_only` = IMG 有按钮节点、本地没引用；`code_only` = 本地引用了、IMG 无该节点。线索不是判词，需再对 IDA/正确客户端截图。

| 窗口 | IMG 子节点 | IMG Bt* | 代码 Bt* | 已对上 | 通用关闭钮 | IMG 独有 | 代码独有 |
| --- | ---: | ---: | ---: | ---: | :---: | --- | --- |
| KeyConfig | 12 | 7 | 6 | 6 | 是 | BtHelp | — |
| Equip | 11 | 7 | 7 | 7 | 是 | — | — |
| Item | 15 | 6 | 6 | 6 | 是 | — | — |
| Stat | 11 | 5 | 12 | 3 | 是 | BtAuto1, BtAuto2 | BtAutoAP, BtDetailOpen, BtDexUp, BtHide, BtHpUp, BtIntUp, BtLukUp, BtMpUp, BtStrUp |
| Skill | 11 | 1 | 10 | 1 | 是 | — | BtCancle, BtGuildSkill, BtHyper, BtMacro, BtOK, BtOkay, BtRide, BtSpDown, BtSpMax |
| UserList | 9 | 0 | 39 | 0 | 是 | — | BtAdd, BtAddFriend, BtAddGroup, BtBlock, BtBoard, BtChange, BtChangeBoss, BtChat, BtCreate, BtDelete, BtDown, BtEnter, BtGroupWhisper, BtGuildBBS, BtHP, BtInfo, BtInvite, BtKick, BtMate, BtMessage, BtMessenger, BtMod, BtParty, BtPartyInvite, BtPartyLevel, BtPartyMake, BtPartyOut, BtPartySettings, BtPlusFriend, BtSearch, BtSheetIClose, BtShowAll, BtShowOnline, BtUnBlock, BtUp, BtWhere, BtWhisper, BtWithdraw, Btnotice |
| Family | 10 | 8 | 8 | 8 | — | — | — |
| Maker | 10 | 3 | 0 | 0 | 是 | BtCancel, BtDown, BtStart | — |
| MiniMap | 5 | 1 | 3 | 1 | — | — | BtMax, BtMin |
| Messenger | 7 | 1 | 2 | 1 | 是 | — | BtClose3 |
| WorldMap | 2 | 0 | 5 | 0 | 是 | — | BtAllsearch, BtAutoFly_1, BtHide, BtNaviRegister, BtSearch |
| MonsterBook | 15 | 2 | 0 | 0 | 是 | BtClose, BtSearch | — |
| PartySearch | 13 | 5 | 5 | 5 | 是 | — | — |
| QuestLog | — | — | — | — | — | IMG 中无此根 | — |
| Whisper | — | — | — | — | — | IMG 中无此根 | — |
| Medal | — | — | — | — | — | IMG 中无此根 | — |
| GuildBBS | — | — | — | — | — | IMG 中无此根 | — |

## 逐窗明细

### KeyConfig
- IMG 全部子节点：`backgrnd`, `icon`, `key`, `notice`, `BtClose`, `BtHelp`, `BtOK`, `BtCancel`, `BtDefault`, `BtDelete`, `BtQuickSlot`, `quickslotConfig`
- IMG Bt*：`BtCancel`, `BtClose`, `BtDefault`, `BtDelete`, `BtHelp`, `BtOK`, `BtQuickSlot`
- 代码引用 Bt*：`BtCancel`, `BtClose`, `BtDefault`, `BtDelete`, `BtOK`, `BtQuickSlot`
- 代码引用非 Bt* 资源：`KeyConfig`, `backgrnd`, `backgrnd2`, `backgrnd3`, `icon`, `key`

### Equip
- IMG 全部子节点：`backgrnd`, `pet`, `FullBackgrnd`, `BtDetail`, `BtPet1`, `BtPet2`, `BtPet3`, `BtPetEquipShow`, `BtPetEquipHide`, `BtCashshop`, `Addon`
- IMG Bt*：`BtCashshop`, `BtDetail`, `BtPet1`, `BtPet2`, `BtPet3`, `BtPetEquipHide`, `BtPetEquipShow`
- 代码引用 Bt*：`BtCashshop`, `BtDetail`, `BtPet1`, `BtPet2`, `BtPet3`, `BtPetEquipHide`, `BtPetEquipShow`
- 代码引用非 Bt* 资源：`backgrnd`, `disabled`, `normal`, `pet`

### Item
- IMG 全部子节点：`backgrnd`, `activeIcon`, `BtCoin`, `Tab`, `bossPetIcon`, `shadow`, `BtGather`, `BtSort`, `Quality`, `FullBackgrnd`, `disabled`, `BtFull`, `BtSmall`, `BtCashshop`, `New`
- IMG Bt*：`BtCashshop`, `BtCoin`, `BtFull`, `BtGather`, `BtSmall`, `BtSort`
- 代码引用 Bt*：`BtCashshop`, `BtCoin`, `BtFull`, `BtGather`, `BtSmall`, `BtSort`
- 代码引用非 Bt* 资源：`AutoBuild`, `FullAutoBuild`, `FullBackgrnd`, `FullBackgrnd2`, `FullBackgrnd3`, `New`, `Tab`, `Tab0`, `Tab1`, `activeIcon`, `backgrnd`, `backgrnd2`, `backgrnd3`, `disabled`, `enabled`, `inventory`, `pos`, `productionBackgrnd`, `productionBackgrnd2`, `productionBackgrnd3`, `slot_col`, `slot_pos`, `slot_row`, `slot_space_x`, `slot_space_y`

### Stat
- IMG 全部子节点：`basicStat`, `BtApUp`, `BtDetail`, `Job`, `BtAuto`, `BtAuto1`, `BtAuto2`, `Disabled`, `backgrnd2`, `backgrnd`, `backgrnd4`
- IMG Bt*：`BtApUp`, `BtAuto`, `BtAuto1`, `BtAuto2`, `BtDetail`
- 代码引用 Bt*：`BtApUp`, `BtAuto`, `BtAutoAP`, `BtDetail`, `BtDetailOpen`, `BtDexUp`, `BtHide`, `BtHpUp`, `BtIntUp`, `BtLukUp`, `BtMpUp`, `BtStrUp`
- 代码引用非 Bt* 资源：`DEX`, `Disabled`, `INT`, `LUK`, `STR`, `abilityTitle`, `activated`, `backgrnd`, `backgrnd2`, `backgrnd3`, `backgrnd4`, `basicStat`, `cover0`, `detail`, `disabled`, `epic`, `legendary`, `main`, `metierLine`, `normal`, `rare`, `unique`

### Skill
- IMG 全部子节点：`backgrnd`, `skill0`, `skill1`, `line`, `BtSpUp`, `Tab`, `tip0`, `tip1`, `tip2`, `CoolTime`, `recommend`
- IMG Bt*：`BtSpUp`
- 代码引用 Bt*：`BtCancle`, `BtGuildSkill`, `BtHyper`, `BtMacro`, `BtOK`, `BtOkay`, `BtRide`, `BtSpDown`, `BtSpMax`, `BtSpUp`
- 代码引用非 Bt* 资源：`New`, `Tab`, `Tab0`, `Tab1`, `backgrnd`, `backgrnd2`, `backgrnd3`, `disabled`, `enabled`, `line`, `macro`, `main`, `num`, `skill0`, `skill1`, `skillBlank`, `skillPoint`

### UserList
- IMG 全部子节点：`backgrnd`, `backgrnd2`, `backgrnd3`, `Friend`, `Party`, `Guild`, `Tab`, `BlackList`, `GuildUnion`
- IMG Bt*：`—`
- 代码引用 Bt*：`BtAdd`, `BtAddFriend`, `BtAddGroup`, `BtBlock`, `BtBoard`, `BtChange`, `BtChangeBoss`, `BtChat`, `BtCreate`, `BtDelete`, `BtDown`, `BtEnter`, `BtGroupWhisper`, `BtGuildBBS`, `BtHP`, `BtInfo`, `BtInvite`, `BtKick`, `BtMate`, `BtMessage`, `BtMessenger`, `BtMod`, `BtParty`, `BtPartyInvite`, `BtPartyLevel`, `BtPartyMake`, `BtPartyOut`, `BtPartySettings`, `BtPlusFriend`, `BtSearch`, `BtSheetIClose`, `BtShowAll`, `BtShowOnline`, `BtUnBlock`, `BtUp`, `BtWhere`, `BtWhisper`, `BtWithdraw`, `Btnotice`
- 代码引用非 Bt* 资源：`BlackList`, `CbCondition`, `Friend`, `Guild`, `GuildInfo`, `GuildUnion`, `Main`, `Party`, `PartySearch`, `Sheet1`, `Sheet2`, `Sheet6`, `Tab`, `TapShowAll`, `TapShowGuild`, `TapShowIndividual`, `TapShowOnline`, `backgrnd`, `backgrnd2`, `backgrnd3`, `base`, `disabled`, `enabled`, `guildinfo0`, `guildinfo1`, `guildinfo6`, `guildinfo7`, `line`, `partyName`, `request`, `table`, `text`, `title`

### Family
- IMG 全部子节点：`backgrnd`, `BtClose`, `BtFamilyPrecept`, `BtJuniorEntry`, `BtTree`, `BtLeft`, `BtRight`, `BtSpecial`, `BtOK`, `RightIcon`
- IMG Bt*：`BtClose`, `BtFamilyPrecept`, `BtJuniorEntry`, `BtLeft`, `BtOK`, `BtRight`, `BtSpecial`, `BtTree`
- 代码引用 Bt*：`BtClose`, `BtFamilyPrecept`, `BtJuniorEntry`, `BtLeft`, `BtOK`, `BtRight`, `BtSpecial`, `BtTree`
- 代码引用非 Bt* 资源：`backgrnd`

### Maker
- IMG 全部子节点：`backgrnd`, `notUse`, `randomMake`, `randomRecipe`, `BtStart`, `BtCancel`, `GaugeBar`, `BtDown`, `ComboBox1`, `ComboBox2`
- IMG Bt*：`BtCancel`, `BtDown`, `BtStart`
- 代码引用 Bt*：`—`
- 代码引用非 Bt* 资源：`Maker`, `backgrnd`, `cost`, `recipe`, `reqLevel`, `targetItem`

### MiniMap
- IMG 全部子节点：`title`, `MinMap`, `MaxMap`, `Min`, `BtMap`
- IMG Bt*：`BtMap`
- 代码引用 Bt*：`BtMap`, `BtMax`, `BtMin`
- 代码引用非 Bt* 资源：`DefaultHelper`, `ListNpc`, `MaxMap`, `Min`, `MinMap`, `MiniMapSimpleMode`, `another`, `c`, `canvas`, `centerX`, `centerY`, `e`, `height`, `iconNpc`, `info`, `mag`, `mapMark`, `mapName`, `mark`, `miniMap`, `minimap`, `n`, `ne`, `npc`, `nw`, `portal`, `pt`, `s`, `se`, `sw`, `tm`, `user`, `w`, `width`, `x`, `y`

### Messenger
- IMG 全部子节点：`backgrnd3`, `blink`, `chatBalloon`, `BtEnter`, `NameBar`, `backgrnd`, `backgrnd2`
- IMG Bt*：`BtEnter`
- 代码引用 Bt*：`BtClose3`, `BtEnter`
- 代码引用非 Bt* 资源：`NameBar`, `backgrnd`, `backgrnd2`, `backgrnd3`, `blink`, `chatBalloon`, `x`, `y`

### WorldMap
- IMG 全部子节点：`title`, `Border`
- IMG Bt*：`—`
- 代码引用 Bt*：`BtAllsearch`, `BtAutoFly_1`, `BtHide`, `BtNaviRegister`, `BtSearch`
- 代码引用非 Bt* 资源：`BaseImg`, `Border`, `MapLink`, `MapList`, `WorldMap`, `WorldMapSearch`, `backgrnd`, `curPos`, `desc`, `info`, `link`, `linkImg`, `linkMap`, `mapImage`, `mapNo`, `normal`, `notice`, `parentMap`, `path`, `spot`, `title`, `type`, `worldMap`

### MonsterBook
- IMG 全部子节点：`backgrnd`, `cardSlot`, `infoPage`, `select`, `cover`, `fullMark`, `arrowLeft`, `arrowRight`, `icon`, `BtClose`, `BtSearch`, `LeftTab`, `LeftTabInfo`, `RightTab`, `ContextMenu`
- IMG Bt*：`BtClose`, `BtSearch`
- 代码引用 Bt*：`—`
- 代码引用非 Bt* 资源：`MonsterBook`, `backgrnd`, `name`

### PartySearch
- IMG 全部子节点：`backgrnd`, `check0`, `check1`, `icon0`, `icon1`, `Party`, `PartyMember`, `BtReg`, `BtCancel`, `BtStart`, `BtPause`, `BtStop`, `Tab`
- IMG Bt*：`BtCancel`, `BtPause`, `BtReg`, `BtStart`, `BtStop`
- 代码引用 Bt*：`BtCancel`, `BtPause`, `BtReg`, `BtStart`, `BtStop`
- 代码引用非 Bt* 资源：`backgrnd`

### QuestLog
- UIWindow.img 无此根节点（原生复用别的窗/资源在别根）。

### Whisper
- UIWindow.img 无此根节点（原生复用别的窗/资源在别根）。

### Medal
- UIWindow.img 无此根节点（原生复用别的窗/资源在别根）。

### GuildBBS
- UIWindow.img 无此根节点（原生复用别的窗/资源在别根）。
