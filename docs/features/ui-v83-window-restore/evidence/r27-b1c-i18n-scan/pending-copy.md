# NO-NATIVE 文案待取证台账（R27-B1c）

**这张表是什么**：B1c 全量接入按用户拍板走「保持英文原值，只建键」，于是有一批键的**值**是本包自造的、
没有 v83 原生依据。门禁禁止凭记忆编词条（`String.img` / IDA 才算依据），所以这些键**只登记不翻译**：
日后逐条取证后**只改 ini 的值，代码不用动**（站点已在 `locale/*.ini`，绑定已在窗体）。

| 键 | 现值（zh / en） | 站点 | 建议取证来源 | 状态 |
| --- | --- | --- | --- | --- |
| `version.prefix` | `Ver. {}` / `Ver. {}` | `UILogin.cpp:76`、`UICharSelect.cpp:136`、`UIRaceSelect.cpp:156`、`UICommonCreation.cpp:78 / :360` | 原生版本号那行是 `Login.img/Common/version/pos` 定位的**位图数字**（`UIWindow.img/.../Number`）还是自绘文本，决定要不要这个前缀 —— 用 orange-wz 只读查 `pos` + 原生截帧对照 | 待取证（当前值 = 原英文字面量，行为不变） |
| `charselect.rank_unavailable` | `排行不可用` / `排行不可用` | `UICharSelect.cpp:387`（ctor 初值，刻意不上表）、`:1308`（update 无排行分支） | 角色选择窗「排行」行在原生 v83 是 `Login.img/CharSelect` 的烘焙位图还是自绘 `Text`；若自绘，真文案在 `String.img/CharSelect` | **两份同值**：en 不编英文（编一个和编一个是同一种错）。待取证 |
| `charselect.rank_up` | `{} up` / `{} up` | `UICharSelect.cpp:1300` | 原生排行升降是**箭头位图**（`Login.img/CharSelect/.../arrow`）而非文字后缀的可能性大 —— 若是，正确做法不是换文案，是改成贴图 | 待取证 |
| `charselect.rank_down` | `{} down` / `{} down` | `UICharSelect.cpp:1302` | 同上 | 待取证 |
| `mappanel.select_monster` | `Select a monster` / 同值 | `UIMapDropPanel.cpp:39`（ctor 绑定） | 这三块世界地图侧栏是本包自造 UI，原生 v83 无对应窗体 ⇒ 无依据可取，只能算「自拟文案」。要中文化须用户给措辞 | 待用户定措辞（当前值 = 站点原英文字面量） |
| `mappanel.no_monsters` | `No monsters on this map` / 同值 | `UIMapMobPanel.cpp:67`、`UIMapInfoPanel.cpp:174` | 同上 | 同上 |
| `mappanel.no_drop_data` | `No drop data` / 同值 | `UIMapDropPanel.cpp:41`、`UIMapInfoPanel.cpp:176` | 同上 | 同上 |
| `mappanel.mesos` | `mesos` / 同值 | `UIMapInfoPanel.cpp:178` | 「金币」二字在原生是 `String.img/Mobs/...` 还是 UI 自绘？本包侧栏是贴图 + 自绘混排，需先确认这行是不是该整体改成贴图 | 同上 |
| `mapdrop.unknown_item` | `Unknown` / 同值 | `UIMapDropPanel.cpp:278`（ItemData 无效分支，活文案） | 同上（无原生窗体） | 同上 |
| `userlist.none` | `none` / 同值 | `UIUserList.cpp:106 / :181 / :240`（三处绑定） | 好友/组员/黑名单窗的空态占位在原生 v83 是 `UIWindow.img/UserList` 烘焙位图还是自绘 `Text` —— 用 orange-wz 只读查该根下有无文字节点 | 待取证 |
| `userlist.cur_location` | `My Location - {}` / 同值 | `UIUserList.cpp:179`（带值绑定） | 前半截文案在 `String.img/UIWindow` 一类节点里可能有原生对应（如 `UserList/MyLocation`），需查 | 待取证 |
| `userlist.default_group` | `Default Group (0/0)` / 同值 | `UIUserList.cpp:183` | 同 `userlist.none`。注：这条整串都是文案（计数恒 0/0，无人改写），所以能进绑定表 | 待取证 |
| `viewallchar.world` | `World {}` / 同值 | `UIViewAllChar.cpp:50 / :52`（活文案） | 查看全部角色窗在原生是 `Login.img/SelectAllChar` + `String.img` 的组合，世界名前缀多半有现成文案 | 待取证 |
| `joypad.none` | `None` / 同值 | `UIJoypad.cpp:51`（绑定）、`:128`（恢复默认，活文案） | 手柄设置窗本包自造，原生 v83 无该窗 ⇒ 只能自拟 | 待用户定措辞 |
| `region.warn_na` / `region.warn_eu` | 两句完整警告 / 同值 | `UIRegion.cpp:62 / :65`（hover 现取） | 原生 v83 的区域选择提示是否存在（`String.img/Region`？）；CN 构建根本走不到这条路，接机制只为「站点归零」口径一致 | 待取证（低优先） |
| `buddylist.title` | `好友 ({})` / 同值 | `UIBuddyList.cpp:58`（draw 每帧现取） | 好友窗标题在原生多半是 `UIWindow.img/UserList` 的烘焙位图（B1c-3 已记 `userlist.none` 同问），若确认烘焙则该整行归位图、不留 ini 键 | 待取证（值 = 站点原中文，本批不翻译） |
| `common.channel` | `频道{}` / 同值 | `UIBuddyList.cpp:74`、`UIParty.cpp:84`（两窗共用一个键） | 原生角色名后的频道标注在 `String.img/Etc` 一类节点里可能有现成词（游戏内名牌上的「/频道」是贴图还是文本要分开查） | 待取证 |
| `common.offline` | `离线` / 同值 | `UIBuddyList.cpp:75`、`UIParty.cpp:86` | 同 `common.channel` | 待取证 |
| `party.title` | `队伍 #{}` / 同值 | `UIParty.cpp:68`（draw 每帧现取） | 原生队伍窗标题可能直接是 `Party.img` 烘焙字；若是，改贴图不是改文案 | 待取证 |
| `storage.meso` | `金币: {}` / 同值 | `UIStorage.cpp:64`（ctor）、`:170`、`:183`（两条包回调） | 「金币」在背包/仓库窗原生是 `String.img` 一类还是 `UIWarehouse.img` 烘焙位图 —— 与本包 `mappanel.mesos` 同一问，可一次查清 | 待取证 |
| `trade.title` | `交易` / 同值 | `UITrade.cpp:66`（draw 每帧现取） | 原生交易窗标题在 `Trade.img/backgrnd` 里是烘焙字的可能性大 | 待取证 |
| `trade.waiting` | `（等待对方）` / 同值 | `UITrade.cpp:67` | 原生「等待对方」提示是否在 `String.img/Trade` 有现成整句 | 待取证 |
| `trade.meso` | `金币: {}` / 同值 | `UITrade.cpp:72`、`:94` | 同 `storage.meso` | 待取证 |
| `trade.locked` | `[锁定]` / 同值 | `UITrade.cpp:73` | 原生锁定态多半是按钮态位图（BtLock 两态）而非文字后缀 | 待取证 |
| `mapsearch.list_header` | `地图列表` / 同值 | `UIMapSearchPanel.cpp:171`（面板自带绑定） | 世界地图侧栏是本包自造 UI，原生无对应窗体 ⇒ 只能自拟 | 待用户定措辞 |
| `mapsearch.no_data` | `未选择地图` / 同值 | `UIMapSearchPanel.cpp:173`（同上） | 同上 | 同上 |
| `mapsearch.cost` | `费用: {}` / 同值 | `UIMapSearchPanel.cpp:175`（带值绑定，值恒 `"0"`） | 同上；且本包还没接传送费用数据，接真值时这行要改成事件驱动现取 | 同上（附带数据未接） |
| `mapsearch.street` | `街: {}` / 同值 | `UIMapSearchPanel.cpp:290`（draw 现取） | 原生世界地图详情面板的街道前缀（`WorldMap.img` + `String.img/WorldMap`）可能有现成词 | 待取证 |
| `mapsearch.tab_unloaded` | `（该分类数据未加载）` / 同值 | `UIMapSearchPanel.cpp:330`（draw 局部 Text 现取） | 本包自造状态提示，原生无该分类页 ⇒ 自拟 | 待用户定措辞 |
| `mapinfo.type_boss` | `BOSS` / 同值 | `UIMapInfoPanel.cpp:499`（活文案，与「普通」同一表达式） | 原生怪物列表的 BOSS 标记多半是贴图（`Mob.img` 或 `UIWindow.img` 的徽章） | 待取证 |
| `mapinfo.type_normal` | `普通` / 同值 | `UIMapInfoPanel.cpp:499` | 同上；本包侧栏自造，与 `mapinfo.type_boss` 成对 | 待用户定措辞 |
| `mapinfo.attr` | `属性: {}` / 同值 | `UIMapInfoPanel.cpp:529`（活文案，值暂为占位 `-`） | 怪物属性/抗性数据本包未接 ⇒ 键先立，值等数据与原生文案两头都齐再定 | 待取证（数据也未接） |
| `mapinfo.resist` | `抗性: {}` / 同值 | `UIMapInfoPanel.cpp:529` | 同上 | 同上 |
| `dailygift.banner_sub` | `丰厚礼物，天天相送！` / 同值 | `UIDailyGift.cpp:125`（绑定） | 每日礼物窗本包自造（无协议、假数据），原生 v83 无该窗 ⇒ 自拟 | 待用户定措辞 |
| `dailygift.banner_main` | `冒险岛每日礼物` / 同值 | `UIDailyGift.cpp:127`（绑定） | 同上 | 同上 |
| `dailygift.rule_1`…`rule_6` | 六条规则整句 / 同值 | `UIDailyGift.cpp:132`（循环绑定，键名 `rule_1`…`rule_6`） | 同上，且这六句是**本版实现说明**（写明了假数据/无协议），措辞应与功能状态一起维护，不宜照抄商业客户端文案 | 待用户定措辞 |
| `dailygift.progress` | `等级范围怪物` / 同值 | `UIDailyGift.cpp:136`（绑定） | 同 `dailygift.banner_sub` | 同上 |
| `dailygift.claim` | `领取礼物` / 同值 | `UIDailyGift.cpp:138`（绑定） | 同上 | 同上 |
| `dailygift.day_prefix` / `day_suffix` | `第` / `天`（同值） | `UIDailyGift.cpp:155`、`:157`（绑定） | 同上；两条是「第 N 天」的左右单位词，英文语序会变（`Day N`），所以必须留在 ini 而不是拼在代码里 | 同上 |
| `dailygift.date_month` / `date_day` | `月` / `日`（同值） | `UIDailyGift.cpp:159`、`:161`（绑定） | 同上；日期单位词在两种语言里的位置不同，属同一条理由 | 同上 |


## 为什么模板里带占位符而不是「数字 + 独立后缀」

`Util/Locale.cpp` 的加载器会 `trim` 掉值两端空白，所以单独存 `" up"` 只会得到 `"up"`；
后缀连空格一起待在模板里（`{} up`），占位符两侧的空格由每种语言自己决定 —— 这是
`Locale::format` 的既定约定，不是这次的临时发明。升降档拆成「名次」+「上/下」两条键反而更糟：
中文会拿到「42 上」这种断不成句的东西，且没法表达「上 42」这种语序。

## 判据：哪些键**不进**这张表

只有 `# NO-NATIVE` 整行注释标出来的键才登记在此。已有原生依据的（`equip.pot.*`、
`sysopt.combo.*` 取自截帧等）不列 —— 那张表一旦混进有依据的条目，就没人再信「表里的都值得去取证」。

## 与「站点归零」的关系

`tools/scan_display_text.py` 的清点（`inventory.txt`，402 处）里，**壳窗 333 处**是英文自造提示，
按决策 B 同样「保持英文原值只建键」。那一批键落地时按同一格式追加到这里：一行一键，
值照抄现英文，状态写「待取证」。**所以这张表会随 B1c-5/B1c-6 变长，不是收尾时才写的总结。**
