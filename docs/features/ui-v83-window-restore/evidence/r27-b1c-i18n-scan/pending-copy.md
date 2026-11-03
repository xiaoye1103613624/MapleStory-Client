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

### B1c-5a（壳窗第一批 · 49 键，值 = 站点原英文字面量）

取证入口对五行是同一个动作：**先用 orange-wz 只读枚举 `String.img` 的顶层根名**，再按窗体定位整句。
节点名不在这里预设（预设就是编造），所以下表「建议取证来源」只写方向。这五个窗在本包都是壳窗
（能开、能发封包、状态行自拟），原生 v83 里 `UIWindow.img` 有同名根 ⇒ 大概率有现成文案可换值。

| 键 | 现值（zh / en） | 站点 | 建议取证来源 | 状态 |
| --- | --- | --- | --- | --- |
| `vegaspell.hint_init` / `hint_drop` | `Drop equip + scroll` / `Drop equip / scroll / vega` | `UIVegaSpell.cpp:32`（ctor 初值，不上表）、`:93`（drop 失败分支） | 原生 Vega 窗拖放区的引导语（`String.img` 里 Vega 相关根）；两条语义近乎重复，取证后可能合并成一条 | 待取证 |
| `vegaspell.status_scrolling` / `status_success` / `status_failed` | `Scrolling…` / `Success!` / `Failed.` | `UIVegaSpell.cpp:68 / :69 / :70`（收包回调现取） | 这三条对应回执 op 码 0x40/0x41/0x43，原生整句多半在 `String.img` 的强化/卷轴类节点下 | 待取证 |
| `vegaspell.status_result_op` | `Result op=0x{}` | `UIVegaSpell.cpp:71`（default 分支） | **多半没有原生依据**：这是本版调试兜底（未知 op 回显）。要中文化须先决定未知 op 到底该显示什么 | 待用户定策（可考虑改成不显示或走 LOG） |
| `vegaspell.slot_equip_at` / `slot_scroll_at` / `slot_vega_at` | `Equip @{}` / `Scroll @{}` / `Vega @{}` | `UIVegaSpell.cpp:82 / :87 / :91`（drop 现取） | 三条同源：拖放落位的「哪一格」回显。原生若用图标高亮而不是文字，正确做法是删文案不是换文案 | 待取证 |
| `vegaspell.slot_equip_num` / `slot_scroll_num` | `Equip #{}` / `Scroll #{}` | `UIVegaSpell.cpp:128, :138` / `:154, :164` | 与上三条同一问（`#N` 是本包自拟的槽位写法） | 待取证 |
| `vegaspell.prompt_equip_slot` / `prompt_scroll_slot` | `Equip slot` / `Scroll slot` | `UIVegaSpell.cpp:141`、`:167`（**UISoftKey 第三参**，扫描器盲区） | 属软键盘的**输入格式说明**（期望用户输入槽位号）：可译，但译文必须保留「输入槽位号」这层意思 | 待取证（格式说明类） |
| `vegaspell.need_equip_scroll` / `no_vega_in_cash` / `sent` | `Need equip + scroll` / `No Vega Spell in Cash` / `Vega sent…` | `UIVegaSpell.cpp:187 / :202 / :208` | 前两条是本包自造的失败提示，原生对应行为是**按钮置灰**而非弹文案的可能性大；`sent` 同理 | 待取证 |
| `rps.title` | `Rock Paper Scissors` | `UIRpsGame.cpp:22`（ctor 初值） | 原生 RPS 窗标题大概率是 `UIWindow.img/RpsGame` 烘焙位图 ⇒ 若是，这行归位图、不留 ini 键 | 待取证 |
| `rps.round_result` | `You:{} NPC:{}` | `UIRpsGame.cpp:67`（`format_all` 双占位） | 原生结算行是手势图标 + 胜负位图，文字整句需查 `String.img` RPS 根 | 待取证 |
| `rps.pick_rock` / `pick_paper` / `pick_scissor` | `Rock` / `Paper` / `Scissor`（各 ×2） | `UIRpsGame.cpp:92/:118`、`:95/:121`、`:98/:124`（键路径与软键盘路径各一处） | 同一状态行的两个入口，取证后可合并措辞；原生是图标则整行删文案 | 待取证 |
| `rps.started` / `continued` / `retry` | `Started` / `Continue` / `Retry` | `UIRpsGame.cpp:101/:127/:143`、`:104`、`:107/:147` | 原生对应 `BtStart/BtContinue/BtRetry` **按钮位图**上的烘焙字（按钮态本身已有文案，状态行重复） | 待取证（可能是冗余文案） |
| `rps.softkey_choices` | `rock\|paper\|scissor` | `UIRpsGame.cpp:132`（UISoftKey 第三参） | **禁译，无取证需要**：值是软键盘解析器的输入 token，`t == "rock"` 那套比较只认英文；建键只为站点归零，ini 注释已写死理由 | 不翻译（硬契约） |
| `plant.title` / `raise.title` | `Plant` / `Raise` | `UIPlant.cpp:70, :83` / `:171, :184`（ctor 初值 + 空输入回落） | 种植/收获壳窗的标题行；原生对应 `FruitGarden` 一类节点，整句需在 `String.img` 定位 | 待取证 |
| `plant.use_slot` / `use_item` | `Use #{}` / `Use {}` | `UIPlant.cpp:33`（两窗共用的静态 helper）、`:92/:130/:193/:231` | 「使用」动词在原生有现成词（`String.img/UIWindow` 类），但这两条本质是**调试回显**（告诉你刚发了哪格） | 待取证（或按调试件处置） |
| `plant.open_item_ui` / `info_number` | `OpenItemUI {}` / `infoNumber` | `UIPlant.cpp:152/:253`、`:154/:255` | 两条都是封包参数回显，原生界面不会出现 ⇒ 属调试文案，不该走文案取证，该决定是否移出 UI | 待用户定策（调试件） |
| `plant.water_of_life` | `WaterOfLife` | `UIPlant.cpp:159`、`:260` | 同上：这是封包名，不是给人读的文案 | 待用户定策（调试件） |
| `plant.softkey_hint` / `raise.softkey_hint` | `Plant itemid\|tip` / `Raise itemid\|tip` | `UIPlant.cpp:101/:137`、`:202/:238`（UISoftKey 第三参） | 输入格式说明，可译但须保留「itemid 或提示串」二选一这层意思；`\|` 是分隔符不是文案 | 待取证（格式说明类） |
| `hammer.hint_drop` / `drop_first` / `need_equip_tab` | `Drop an equip here` / `Drop an equip first` / `Need an equip from Equip tab` | `UIViciousHammer.cpp:32`（ctor 初值）、`:160`、`:88` | 原生铁锤窗拖放区引导语（`String.img` 的 ViciousHammer 相关根）；`hint_drop` 与 `drop_first` 语义重叠，取证后可能合并 | 待取证 |
| `hammer.used` / `equip_slot` / `equip_num` | `Hammer used: {}` / `Equip slot {}` / `Equip #{}` | `UIViciousHammer.cpp:78`、`:95`、`:127/:137` | 计数与槽位回显；原生用位图数字还是自绘 `Text` 决定这条留不留 | 待取证 |
| `hammer.no_hammer_cash` / `sent` | `No Vicious Hammer in Cash` / `Hammer sent…` | `UIViciousHammer.cpp:176`、`:182` | 失败/成功回执；原生多半是**不响应**或弹通用提示（`String.img` 公共失败句） | 待取证 |
| `hammer.softkey_equip_slot` | `Equip inv slot` | `UIViciousHammer.cpp:141`（UISoftKey 第三参） | 输入格式说明类，同 `vegaspell.prompt_*` | 待取证（格式说明类） |
| `wedding.hint_softkey` / `softkey_guest` / `softkey_invite_msg` | `Wedding — SoftKey guest` / `Wedding guest` / `Invite msg` | `UIWedding.cpp:49`、`:72/:140/:187`、`:62/:71/:129/:139/:179` | 前两条是本包自造的「该输入什么」说明（原生婚礼窗无此软键盘流程）⇒ 无依据可取，要中文化须用户给措辞 | 待用户定措辞 |
| `wedding.guest` / `invited` | `Guest: {}` / `Invited {}` | `UIWedding.cpp:57/:66/:123/:133/:172`、`:127/:137/:176` | 「受邀者名字」行的前缀与回执；原生婚礼请柬是烘焙位图 + 名字贴图，需先确认这行在原生是否以文本存在 | 待取证 |
| `wedding.progress` | `Step {}  G:{} B:{}` | `UIWedding.cpp:96`（`format_all` 三占位） | 婚礼流程步进条：原生是 `Wedding` 根的**位图步骤条**，`G:`/`B:` 这种缩写几乎肯定不是原生写法 | 待取证（低优先） |

**判据补充（本批立）**：`"Wedding invite"`（`UIWedding.cpp` 五处悄悄话默认正文）**没建键** ——
它是出站封包正文、给对端看的，翻译会改变对方收到的内容，属协议层不属本端展示文案；同理
`"m|f"` `"y|n"` `"save|close"` 这类被代码按 token 比较的输入语法，一律按 `rps.softkey_choices`
的「禁译」档处理。


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
