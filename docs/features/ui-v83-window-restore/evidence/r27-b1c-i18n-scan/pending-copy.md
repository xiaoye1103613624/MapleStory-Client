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


### B1c-5b（壳窗第二批 · 98 键 / 24 个 .cpp，值 = 站点原英文字面量）

与 5a 的差别只在**广度**：剩余 24 个壳窗（admin / alliance / ariants / …）里的
`change_text`、`Text` 构造、SoftKey 提示一次建完，站点侧零措辞改动（决策 B）。

「建议取证来源」这一列本批**统一填「本包自造，无原生依据」** —— 这些文案在原生 v83 里
既没有对应的 `String.img` 节点，本包也没有那条流程的原生截图可对；逐条编一个看起来合理的
节点名会污染这张表（它的价值恰恰在于「表里的都值得去取证」）。要中文化：给措辞，或在真机
里指出原生确有该文案，再把该行状态从「待用户定措辞」改回「待取证」。

| 键 | 现值（zh / en） | 站点 | 建议取证来源 | 状态 |
| --- | --- | --- | --- | --- |
| `admin.title` | `Admin — SoftKey announce` | UIAdmin.cpp:22 | 本包自造，无原生依据 | 待用户定措辞 |
| `admin.show_users` | `ShowUsers` | UIAdmin.cpp:50、UIAdmin.cpp:93、UIAdmin.cpp:108 | 本包自造，无原生依据 | 待用户定措辞 |
| `admin.alert_admins` | `AlertAdmins` | UIAdmin.cpp:56、UIAdmin.cpp:98、UIAdmin.cpp:113 | 本包自造，无原生依据 | 待用户定措辞 |
| `alliance.name_line` | `Alliance: {}` | UIAlliance.cpp:93 | 本包自造，无原生依据 | 待用户定措辞 |
| `alliance.notice_line` | `Notice: {}` | UIAlliance.cpp:94、UIAlliance.cpp:101 | 本包自造，无原生依据 | 待用户定措辞 |
| `alliance.guilds_header` | `Guilds ({}):` | UIAlliance.cpp:106 | 本包自造，无原生依据 | 待用户定措辞 |
| `ariants.title` | `Ariant` | UIAriantMatch.cpp:34 | 本包自造，无原生依据 | 待用户定措辞 |
| `ariants.result` | `Result {}` | UIAriantMatch.cpp:41、UIAriantMatch.cpp:96、UIAriantMatch.cpp:106…+1 处 | 本包自造，无原生依据 | 待用户定措辞 |
| `gachapon.jackpot` | `JACKPOT #{} x{} left={}` | UICashGachapon.cpp:47 | 本包自造，无原生依据 | 待用户定措辞 |
| `gachapon.reward` | `Gacha #{} x{} left={}` | UICashGachapon.cpp:47 | 本包自造，无原生依据 | 待用户定措辞 |
| `gachapon.failed` | `Gacha failed` | UICashGachapon.cpp:52 | 本包自造，无原生依据 | 待用户定措辞 |
| `cashshop.stub_job` | `Illium` | UICashShop.cpp:100 | 本包自造，无原生依据 | 待用户定措辞 |
| `cashshop.stub_name` | `ShomeiZekkou` | UICashShop.cpp:102 | 本包自造，无原生依据 | 待用户定措辞 |
| `cashshop.meta` | `CS inv {} gift {} wish {}` | UICashShop.cpp:875 | 本包自造，无原生依据 | 待用户定措辞 |
| `cashshop.meta_stor` | `stor{}` | UICashShop.cpp:879 | 本包自造，无原生依据 | 待用户定措辞 |
| `charinfo.medal_stub` | `Junior Adventurer` | UICharInfo.cpp:170 | 本包自造，无原生依据 | 待用户定措辞 |
| `charinfo.pet_wish` | `Pet {} Wish {}` | UICharInfo.cpp:695 | 本包自造，无原生依据 | 待用户定措辞 |
| `charinfo.medal_id` | `Medal#{}` | UICharInfo.cpp:696 | 本包自造，无原生依据 | 待用户定措辞 |
| `charinfo.married` | `Married` | UICharInfo.cpp:697 | 本包自造，无原生依据 | 待用户定措辞 |
| `charinfo.book_count` | `Book {}` | UICharInfo.cpp:699 | 本包自造，无原生依据 | 待用户定措辞 |
| `clock.seconds_left` | `{}s` | UIClock.cpp:34、UIClock.cpp:72 | 本包自造，无原生依据 | 待用户定措辞 |
| `clock.title` | `Clock` | UIClock.cpp:39 | 本包自造，无原生依据 | 待用户定措辞 |
| `clock.zero` | `0s` | UIClock.cpp:75 | 本包自造，无原生依据 | 待用户定措辞 |
| `dailygift.title` | `DAILY GIFT` | UIDailyGift.cpp:125 | 本包自造，无原生依据 | 待用户定措辞 |
| `delivery.row_meso` | `{}  {}m` | UIDelivery.cpp:73 | 本包自造，无原生依据 | 待用户定措辞 |
| `delivery.row_item` | `item:{}` | UIDelivery.cpp:76 | 本包自造，无原生依据 | 待用户定措辞 |
| `delivery.no_packages` | `No packages.` | UIDelivery.cpp:80 | 本包自造，无原生依据 | 待用户定措辞 |
| `delivery.count` | `{} package(s)` | UIDelivery.cpp:81 | 本包自造，无原生依据 | 待用户定措辞 |
| `delivery.updated` | `Package updated.` | UIDelivery.cpp:96 | 本包自造，无原生依据 | 待用户定措辞 |
| `delivery.sending_to` | `Sending to {}…` | UIDelivery.cpp:151 | 本包自造，无原生依据 | 待用户定措辞 |
| `delivery.sent` | `Send {} m={}` | UIDelivery.cpp:244、UIDelivery.cpp:294 | 本包自造，无原生依据 | 待用户定措辞 |
| `delivery.discard_id` | `Discard id={}` | UIDelivery.cpp:323 | 本包自造，无原生依据 | 待用户定措辞 |
| `delivery.claim_id` | `Claim id={}` | UIDelivery.cpp:338 | 本包自造，无原生依据 | 待用户定措辞 |
| `delivery.fee_help` | `Fee = TradeFee(mesos) + 5000 (normal)` | UIDelivery.cpp:372、UIDelivery.cpp:382 | 本包自造，无原生依据 | 待用户定措辞 |
| `delivery.fee_calc` | `TradeFee({})+5000 = {}` | UIDelivery.cpp:379 | 本包自造，无原生依据 | 待用户定措辞 |
| `family.none` | `(no family)` | UIFamily.cpp:54、UIFamily.cpp:63 | 本包自造，无原生依据 | 待用户定措辞 |
| `family.rep_initial` | `Rep: 0 / 0` | UIFamily.cpp:55 | 本包自造，无原生依据 | 待用户定措辞 |
| `family.rep_line` | `Rep {}/{} today {} jr {}/{}` | UIFamily.cpp:66 | 本包自造，无原生依据 | 待用户定措辞 |
| `family.priv_count` | `Priv x{}` | UIFamily.cpp:75 | 本包自造，无原生依据 | 待用户定措辞 |
| `family.ask_separate` | `Separate junior {}?` | UIFamily.cpp:208、UIFamilyTree.cpp:143 | 本包自造，无原生依据 | 待用户定措辞 |
| `family.ask_separate_cid` | `Separate junior cid {}?` | UIFamily.cpp:226、UIFamilyTree.cpp:153 | 本包自造，无原生依据 | 待用户定措辞 |
| `family.ask_leave` | `Leave your senior and fork the family?` | UIFamily.cpp:216、UIFamilyTree.cpp:162 | 本包自造，无原生依据 | 待用户定措辞 |
| `familytree.header` | `Family Tree` | UIFamilyTree.cpp:54 | 本包自造，无原生依据 | 待用户定措辞 |
| `familytree.level` | `Lv.{}` | UIFamilyTree.cpp:69 | 本包自造，无原生依据 | 待用户定措辞 |
| `familytree.online` | `[ON]` | UIFamilyTree.cpp:70 | 本包自造，无原生依据 | 待用户定措辞 |
| `familytree.offline` | `[OFF]` | UIFamilyTree.cpp:70 | 本包自造，无原生依据 | 待用户定措辞 |
| `familytree.rep` | `rep {}` | UIFamilyTree.cpp:71 | 本包自造，无原生依据 | 待用户定措辞 |
| `familytree.pedigree` | `Pedigree ({} members)` | UIFamilyTree.cpp:75 | 本包自造，无原生依据 | 待用户定措辞 |
| `familytree.can_add` | `— can add junior` | UIFamilyTree.cpp:76 | 本包自造，无原生依据 | 待用户定措辞 |
| `maker.bad_item_id` | `Bad item id` | UIMaker.cpp:66 | 本包自造，无原生依据 | 待用户定措辞 |
| `maker.craft` | `Craft {}` | UIMaker.cpp:71 | 本包自造，无原生依据 | 待用户定措辞 |
| `maker.no_recipe` | `No recipe {}` | UIMaker.cpp:76 | 本包自造，无原生依据 | 待用户定措辞 |
| `maker.cost_meso` | `{} meso` | UIMaker.cpp:128 | 本包自造，无原生依据 | 待用户定措辞 |
| `maker.cost_req` | `(Lv{})` | UIMaker.cpp:130 | 本包自造，无原生依据 | 待用户定措辞 |
| `mapletv.no_512` | `No 512* map effect` | UIMapleTV.cpp:76 | 本包自造，无原生依据 | 待用户定措辞 |
| `megaphone.empty` | `Empty` | UIMegaphone.cpp:123 | 本包自造，无原生依据 | 待用户定措辞 |
| `megaphone.no_539` | `No 539*` | UIMegaphone.cpp:126 | 本包自造，无原生依据 | 待用户定措辞 |
| `megaphone.sent` | `Sent…` | UIMegaphone.cpp:130 | 本包自造，无原生依据 | 待用户定措辞 |
| `minigame.ready` | `Ready` | UIMinigame.cpp:83、UIMinigame.cpp:149 | 本包自造，无原生依据 | 待用户定措辞 |
| `minigame.start` | `Start` | UIMinigame.cpp:86、UIMinigame.cpp:153 | 本包自造，无原生依据 | 待用户定措辞 |
| `minigame.omok` | `Omok {}` | UIMinigame.cpp:115 | 本包自造，无原生依据 | 待用户定措辞 |
| `minigame.card` | `Card {}` | UIMinigame.cpp:127 | 本包自造，无原生依据 | 待用户定措辞 |
| `monsterbook.cover_id` | `Cover id={}` | UIMonsterBook.cpp:161 | 本包自造，无原生依据 | 待用户定措辞 |
| `monsterbook.not_found` | `Not found` | UIMonsterBook.cpp:190 | 本包自造，无原生依据 | 待用户定措辞 |
| `carnival.cp_initial` | `CP 0/0` | UIMonsterCarnival.cpp:42 | 本包自造，无原生依据 | 待用户定措辞 |
| `carnival.enemy_initial` | `EN 0/0` | UIMonsterCarnival.cpp:43 | 本包自造，无原生依据 | 待用户定措辞 |
| `carnival.title` | `Carnival` | UIMonsterCarnival.cpp:44 | 本包自造，无原生依据 | 待用户定措辞 |
| `carnival.cp` | `CP {}/{}` | UIMonsterCarnival.cpp:70 | 本包自造，无原生依据 | 待用户定措辞 |
| `carnival.enemy` | `EN {}/{}` | UIMonsterCarnival.cpp:77 | 本包自造，无原生依据 | 待用户定措辞 |
| `mts.title` | `MTS / ITC` | UIMTS.cpp:29 | 本包自造，无原生依据 | 待用户定措辞 |
| `mts.list_line` | `tab{} type{} p{} n={} slots={}` | UIMTS.cpp:44 | 本包自造，无原生依据 | 待用户定措辞 |
| `mts.cash_line` | `NX={} MP={}` | UIMTS.cpp:57 | 本包自造，无原生依据 | 待用户定措辞 |
| `mts.status_default` | `MTS` | UIMTS.cpp:65 | 本包自造，无原生依据 | 待用户定措辞 |
| `mts.item_slot` | `#{}@{}` | UIMTS.cpp:77 | 本包自造，无原生依据 | 待用户定措辞 |
| `mts.more` | `…+{}` | UIMTS.cpp:80 | 本包自造，无原生依据 | 待用户定措辞 |
| `mts.no_items` | `(no items)` | UIMTS.cpp:81 | 本包自造，无原生依据 | 待用户定措辞 |
| `nycard.need_receiver` | `Need receiver` | UINewYearsCard.cpp:74 | 本包自造，无原生依据 | 待用户定措辞 |
| `nycard.to` | `To {} — msg SoftKey` | UINewYearsCard.cpp:76 | 本包自造，无原生依据 | 待用户定措辞 |
| `nycard.no_card` | `No New Year card` | UINewYearsCard.cpp:80 | 本包自造，无原生依据 | 待用户定措辞 |
| `nycard.sent_to` | `Sent to {}` | UINewYearsCard.cpp:84 | 本包自造，无原生依据 | 待用户定措辞 |
| `partyrace.stage_initial` | `Stage 0` | UIPartyRace.cpp:49 | 本包自造，无原生依据 | 待用户定措辞 |
| `partyrace.score_initial` | `R 0 : 0 B` | UIPartyRace.cpp:50 | 本包自造，无原生依据 | 待用户定措辞 |
| `partyrace.stage` | `Stage {}` | UIPartyRace.cpp:80 | 本包自造，无原生依据 | 待用户定措辞 |
| `partyrace.score` | `R {} : {} B` | UIPartyRace.cpp:87 | 本包自造，无原生依据 | 待用户定措辞 |
| `partysearch.range` | `Range {}` | UIPartySearch.cpp:115、UIPartySearch.cpp:205 | 本包自造，无原生依据 | 待用户定措辞 |
| `partysearch.jobs_mask` | `Jobs mask={}` | UIPartySearch.cpp:217 | 本包自造，无原生依据 | 待用户定措辞 |
| `partysearch.searching` | `Searching Lv.{} …` | UIPartySearch.cpp:125 | 本包自造，无原生依据 | 待用户定措辞 |
| `partysearch.idle` | `Idle — press Start to search` | UIPartySearch.cpp:126 | 本包自造，无原生依据 | 待用户定措辞 |
| `remotegacha.box_mode` | `Gacha box mode={}` | UIRemoteGachapon.cpp:53 | 本包自造，无原生依据 | 待用户定措辞 |
| `memo.count` | `{} note(s)` | UIMemo.cpp:54 | 本包自造，无原生依据 | 待用户定措辞 |
| `memo.sel_from` | `Sel {}` | UIMemo.cpp:99 | 本包自造，无原生依据 | 待用户定措辞 |
| `memo.sent_to` | `Sent→{}` | UIMemo.cpp:124、UIMemo.cpp:132 | 本包自造，无原生依据 | 待用户定措辞 |
| `memo.discard_id` | `Discard id={}` | UIMemo.cpp:140、UIMemo.cpp:149 | 本包自造，无原生依据 | 待用户定措辞 |
| `memo.notes_count` | `Notes: {}` | UIMemo.cpp:156 | 本包自造，无原生依据 | 待用户定措辞 |
| `memo.none_selected` | `No note selected` | UIMemo.cpp:177 | 本包自造，无原生依据 | 待用户定措辞 |
| `memo.no_note` | `No note` | UIMemo.cpp:183 | 本包自造，无原生依据 | 待用户定措辞 |
| `storebank.mesos` | `Mesos: {}` | UIStoreBank.cpp:59、UIStoreBank.cpp:86、UIStoreBank.cpp:100 | 本包自造，无原生依据 | 待用户定措辞 |
| `storebank.getall_sent` | `GetAll sent` | UIStoreBank.cpp:65、UIStoreBank.cpp:155、UIStoreBank.cpp:163 | 本包自造，无原生依据 | 待用户定措辞 |

**本批的门禁**（不是自查，是可重跑的尺）：`tools/check_placeholder_arity.py --both` 把
`format`/`format_all` 调用点实参数与 ini 值里的 `{}` 数对齐（318 个调用点，含 `enum_key`
动态前缀家族），键值忠实度尺 98/98 要求每条非占位符片段在原站点逐字出现。两份输出与
负对照记录见同目录 `gate-r27-b1c5b*.txt`。

### B1c-5c（中文盲区四窗 · 15 键 / 3 个 .cpp，zh 值 = 改动前原汉字）

**这批与上面两批的差别只在「等谁定」**：5a/5b 的原字面量是英文，zh 现值本身就是要用户改的措辞；
5c 的原字面量是汉字，**zh 值 = 改动前界面就在显示的那句**（决策 B 不翻译 ⇒ 零风险），
需要定稿的是 **en 那一份**（下表「英文草稿」列全部本包自造，无原生依据）。

| 键 | zh 值（原字面量） | en 草稿（自造） | 站点 | 状态 |
| --- | --- | --- | --- | --- |
| `quest.area.20` | `彩虹岛` | `Rainbow Island` | UIQuestLog.cpp:392 | 待用户定 en 措辞 |
| `quest.area.30` | `金银岛` | `Kernese` | UIQuestLog.cpp:393、UIQuestLog.cpp:394 | 待用户定 en 措辞 |
| `quest.detail.id` | `任务编号: {}` | `Quest ID: {}` | UIQuestLog.cpp:559 | 待用户定 en 措辞 |
| `quest.detail.level_range` | `等级范围: {} ~ {}` | `Level range: {} ~ {}` | UIQuestLog.cpp:562 | 待用户定 en 措辞 |
| `quest.detail.min_level` | `需要等级: {}` | `Required level: {}` | UIQuestLog.cpp:565 | 待用户定 en 措辞 |
| `quest.detail.area` | `所属区域: {}` | `Area: {}` | UIQuestLog.cpp:567 | 待用户定 en 措辞 |
| `quest.detail.npc` | `相关NPC编号: {}` | `Related NPC: {}` | UIQuestLog.cpp:570 | 待用户定 en 措辞 |
| `quest.detail.pre_none` | `前置任务: 无` | `Prereq: none` | UIQuestLog.cpp:573 | 待用户定 en 措辞 |
| `quest.detail.pre` | `前置任务: {}` | `Prereq: {}` | UIQuestLog.cpp:581 | 待用户定 en 措辞 |
| `guild.title_plain` | `公会` | `Guild` | UIGuild.cpp:60 | 待用户定 en 措辞 |
| `guild.title_named` | `公会: {}` | `Guild: {}` | UIGuild.cpp:61 | 待用户定 en 措辞 |
| `guild.member_rank` | `职位{}` | `Rank {}` | UIGuild.cpp:75 | 待用户定 en 措辞 |
| `guild.member_online` | `[在线]` | `[online]` | UIGuild.cpp:76 | 待用户定 en 措辞 |
| `guild.member_offline` | `[离线]` | `[offline]` | UIGuild.cpp:76 | 待用户定 en 措辞 |
| `merchant.owner_shop` | `{} 的商店` | `{}'s Shop` | UIMerchant.cpp:267 | 待用户定 en 措辞 |

`quest.area.*` 取「区域号当键名」：这套号来自任务数据，本包没有原生英文地名依据，
把 `Ellin Forest` 这类名字写进**键名**等于把没核对过的语义钉死；换措辞时只动 ini 值。

**本批门禁读数**：键**提交树 301/301** 对齐（工具当时报 303，是工作树，混了并行会话未提交的 2 条）、
忠实度 15/15（提交前 `HEAD` 判据）、arity 336 站点 0 不合格、
中文站点尺 30→14 且**余下 14 处全部带档号**（待接线 0）。见同目录 `gate-r27-b1c5c*.txt`、
`cjk-scan-full.txt`。中文站点的两处**不接线**（`UIGender.cpp` 的 `t == "女"/"男"`）理由已写进
`tools/scan_cjk_literals.py` 的 `EXEMPT`，那是软键盘输入的比对串，接了 i18n 会在切语言后失配。

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


### B1c-6（壳窗·UIMiscShells 非软键盘 · 138 键，值 = 站点原英文字面量逐字重建）

同 5b 的口径：「建议取证来源」统一「本包自造，无原生依据」（Wave-A 壳窗，原生 v83 无对应流程）。
`{}` = 变量插点位。`common.*` = 同值跨窗共享。

| 键 | 现值（zh / en） | 站点 | 建议取证来源 | 状态 |
| --- | --- | --- | --- | --- |
| `common.open_store` | `Open store…` | UIMiscShells.cpp:200 + UIMiscShells.cpp:201 + UIMiscShells.cpp:1556 …×5 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `shop.no_514_503_permit` | `No 514/503 permit` | UIMiscShells.cpp:206 + UIMiscShells.cpp:207 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `shop.create` | `Create: {}` | UIMiscShells.cpp:213 + UIMiscShells.cpp:214 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `report.report_softkey_name` | `Report — SoftKey name` | UIMiscShells.cpp:222 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `report.reported` | `Reported {}` | UIMiscShells.cpp:237 + UIMiscShells.cpp:244 + UIMiscShells.cpp:267 …×5 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.empty_name` | `Empty name` | UIMiscShells.cpp:242 + UIMiscShells.cpp:282 + UIMiscShells.cpp:1169 …×4 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `report.target_softkey_enter` | `Target: {} — SoftKey/Enter` | UIMiscShells.cpp:254 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `coupon.coupon_enter_for_code` | `Coupon — Enter for code` | UIMiscShells.cpp:293 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `coupon.empty_code` | `Empty code` | UIMiscShells.cpp:302 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.sent` | `Sent…` | UIMiscShells.cpp:306 + UIMiscShells.cpp:324 + UIMiscShells.cpp:499 …×7 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `claim.claim_softkey_to_msg` | `Claim — SoftKey to/msg` | UIMiscShells.cpp:332 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `claim.need_name` | `Need name` | UIMiscShells.cpp:341 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.note` | `Note → {}` | UIMiscShells.cpp:344 + UIMiscShells.cpp:374 + UIMiscShells.cpp:381 …×4 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `incubator.drop_egg_softkey_slot` | `Drop egg / SoftKey slot` | UIMiscShells.cpp:391 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `incubator.egg_enter` | `Egg #{} — Enter` | UIMiscShells.cpp:408 + UIMiscShells.cpp:417 + UIMiscShells.cpp:445 …×5 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `incubator.drop_an_egg_first` | `Drop an egg first` | UIMiscShells.cpp:490 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `incubator.no_incubator` | `No Incubator` | UIMiscShells.cpp:495 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.drop_equip_softkey_slot` | `Drop equip / SoftKey slot` | UIMiscShells.cpp:594 + UIMiscShells.cpp:808 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.equip_enter` | `Equip #{} — Enter` | UIMiscShells.cpp:609 + UIMiscShells.cpp:616 + UIMiscShells.cpp:653 …×8 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.need_equip` | `Need equip` | UIMiscShells.cpp:628 + UIMiscShells.cpp:714 + UIMiscShells.cpp:842 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.equip_enter_to_use` | `Equip #{} — Enter to use` | UIMiscShells.cpp:632 + UIMiscShells.cpp:846 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.drop_an_equip_first` | `Drop an equip first` | UIMiscShells.cpp:674 + UIMiscShells.cpp:888 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `karmascissors.no_karma_scissors` | `No Karma Scissors` | UIMiscShells.cpp:685 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `enchantskill.e_equip_s_scroll_enter` | `E=equip S=scroll Enter` | UIMiscShells.cpp:694 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `enchantskill.enchant_slot` | `Enchant slot #{}` | UIMiscShells.cpp:718 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `enchantskill.scroll` | `Scroll {}→{}` | UIMiscShells.cpp:733 + UIMiscShells.cpp:783 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `enchantskill.need_scroll_equip` | `Need scroll+equip` | UIMiscShells.cpp:736 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `enchantskill.equip` | `Equip #{}` | UIMiscShells.cpp:748 + UIMiscShells.cpp:755 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `enchantskill.scroll_2` | `Scroll #{}` | UIMiscShells.cpp:768 + UIMiscShells.cpp:775 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `enchantskill.scroll_3` | `Scroll {}` | UIMiscShells.cpp:795 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `itemprotector.no_item_protector` | `No Item Protector` | UIMiscShells.cpp:899 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `teleport.teleport_rock` | `Teleport Rock` | UIMiscShells.cpp:908 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `teleport.no_teleport_rock` | `No Teleport Rock` | UIMiscShells.cpp:923 + UIMiscShells.cpp:999 + UIMiscShells.cpp:1042 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `teleport.need_map_name` | `Need map/name` | UIMiscShells.cpp:927 + UIMiscShells.cpp:1011 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `teleport.warp_map` | `Warp map…` | UIMiscShells.cpp:936 + UIMiscShells.cpp:1007 + UIMiscShells.cpp:1020 …×4 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `teleport.warp_to` | `Warp to {}` | UIMiscShells.cpp:939 + UIMiscShells.cpp:1023 + UIMiscShells.cpp:1064 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `teleport.need_vip_rock_for_name` | `Need VIP rock for name` | UIMiscShells.cpp:941 + UIMiscShells.cpp:1025 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `teleport.saved_map` | `Saved map…` | UIMiscShells.cpp:1047 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `teleport.no_saved_map_softkey` | `No saved map — SoftKey` | UIMiscShells.cpp:1072 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `teleport.warp` | `Warp #{}` | UIMiscShells.cpp:1076 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `speedquiz.quiz_softkey_answer` | `Quiz — SoftKey answer` | UIMiscShells.cpp:1081 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `speedquiz.empty_answer` | `Empty answer` | UIMiscShells.cpp:1089 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `speedquiz.ans` | `Ans: {}` | UIMiscShells.cpp:1090 + UIMiscShells.cpp:1108 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `ranking.ranking_softkey_name` | `Ranking — SoftKey name` | UIMiscShells.cpp:1152 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `ranking.find` | `Find: {}` | UIMiscShells.cpp:1165 + UIMiscShells.cpp:1171 + UIMiscShells.cpp:1192 …×4 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `itemmegaphone.drop_item_softkey_msg` | `Drop item + SoftKey msg` | UIMiscShells.cpp:1208 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `itemmegaphone.item_tab` | `Item tab={} #{}` | UIMiscShells.cpp:1221 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `itemmegaphone.no_item` | `No item` | UIMiscShells.cpp:1248 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.empty_msg` | `Empty msg` | UIMiscShells.cpp:1254 + UIMiscShells.cpp:1484 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `itemmegaphone.no_item_megaphone` | `No Item Megaphone` | UIMiscShells.cpp:1257 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `counsel.counsel_softkey_to_msg` | `Counsel — SoftKey to/msg` | UIMiscShells.cpp:1282 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `counsel.whisper` | `Whisper → {}` | UIMiscShells.cpp:1298 + UIMiscShells.cpp:1329 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `muruengraid.raid_softkey_note` | `Raid — SoftKey note` | UIMiscShells.cpp:1368 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.empty` | `Empty` | UIMiscShells.cpp:1376 + UIMiscShells.cpp:1659 + UIMiscShells.cpp:1752 …×13 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `muruengraid.note` | `Note: {}` | UIMiscShells.cpp:1377 + UIMiscShells.cpp:1394 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `createcygnus.cygnus_softkey_name` | `Cygnus — SoftKey name` | UIMiscShells.cpp:1405 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `createcygnus.name` | `Name: {}` | UIMiscShells.cpp:1414 + UIMiscShells.cpp:1435 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `megaphonebasic.megaphone_softkey_msg` | `Megaphone — SoftKey msg` | UIMiscShells.cpp:1456 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `megaphonebasic.no_megaphone` | `No Megaphone` | UIMiscShells.cpp:1489 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `personalshop.shop_softkey_c_title_o_a_b_x` | `Shop — SoftKey/C title \| O A B X` | UIMiscShells.cpp:1515 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.exit` | `Exit…` | UIMiscShells.cpp:1543 + UIMiscShells.cpp:1614 + UIMiscShells.cpp:1770 …×5 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.buy` | `Buy {}` | UIMiscShells.cpp:1573 + UIMiscShells.cpp:1642 + UIMiscShells.cpp:1833 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `membershop.membershop_softkey_title` | `MemberShop — SoftKey title` | UIMiscShells.cpp:1586 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `tradingroom.trade_softkey_meso_chat_c_x` | `Trade — SoftKey meso/chat \| C/X` | UIMiscShells.cpp:1650 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.meso` | `Meso {}` | UIMiscShells.cpp:1667 + UIMiscShells.cpp:1760 + UIMiscShells.cpp:1862 …×4 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.chat_sent` | `Chat sent…` | UIMiscShells.cpp:1674 + UIMiscShells.cpp:1766 + UIMiscShells.cpp:1869 …×4 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.confirm` | `Confirm…` | UIMiscShells.cpp:1693 + UIMiscShells.cpp:1888 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.transaction` | `Transaction…` | UIMiscShells.cpp:1697 + UIMiscShells.cpp:1892 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.visit` | `Visit…` | UIMiscShells.cpp:1701 + UIMiscShells.cpp:1896 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.additem` | `AddItem #{}` | UIMiscShells.cpp:1727 + UIMiscShells.cpp:1922 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `cashtradingroom.additem` | `AddItem {}` | UIMiscShells.cpp:1746 + UIMiscShells.cpp:1940 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `entrustedshop.entrusted_softkey_o_open` | `Entrusted — SoftKey / O=open` | UIMiscShells.cpp:1782 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `cashtradingroom.cashtrade_softkey_meso_chat_c_x` | `CashTrade — SoftKey meso/chat \| C/X` | UIMiscShells.cpp:1846 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `itemsearch.search_softkey_query` | `Search — SoftKey query` | UIMiscShells.cpp:1972 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.q` | `Q: {}` | UIMiscShells.cpp:1988 + UIMiscShells.cpp:1994 + UIMiscShells.cpp:2015 …×8 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.empty_query` | `Empty query` | UIMiscShells.cpp:1993 + UIMiscShells.cpp:5624 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `aranskillguide.aran_guide_softkey_tip` | `Aran guide — SoftKey tip` | UIMiscShells.cpp:2032 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `aranskillguide.skill` | `Skill {}` | UIMiscShells.cpp:2051 + UIMiscShells.cpp:2090 + UIMiscShells.cpp:2103 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `monsterkilling.mobkill_softkey_tip` | `MobKill — SoftKey tip` | UIMiscShells.cpp:2173 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `skillex.skillex_softkey_skillid` | `SkillEx — SoftKey skillid` | UIMiscShells.cpp:2239 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `skillex.bad_id` | `Bad id` | UIMiscShells.cpp:2252 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `skillex.use_lv` | `Use {} lv{}` | UIMiscShells.cpp:2259 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `skillex.use` | `Use {}` | UIMiscShells.cpp:2287 + UIMiscShells.cpp:2299 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `book.book_softkey_page` | `Book — SoftKey page` | UIMiscShells.cpp:2311 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `book.cover` | `Cover: {}` | UIMiscShells.cpp:2332 + UIMiscShells.cpp:2341 + UIMiscShells.cpp:2375 …×4 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `book.page` | `Page: {}` | UIMiscShells.cpp:2346 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `questalarm.start` | `Start {}` | UIMiscShells.cpp:2418 + UIMiscShells.cpp:2427 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `gmboard.gmboard_softkey_announce` | `GMBoard — SoftKey announce` | UIMiscShells.cpp:2506 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `gmboard.showusers` | `ShowUsers` | UIMiscShells.cpp:2519 + UIMiscShells.cpp:2548 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `gmboard.alertadmins` | `AlertAdmins` | UIMiscShells.cpp:2524 + UIMiscShells.cpp:2553 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `shortcut.1_0_a_z_symbols_gamemenu` | `1-0 A-Z $+symbols(~GameMenu)` | UIMiscShells.cpp:2567 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `cancelrequests.enter_cancel_esc_close` | `Enter=cancel  Esc=close` | UIMiscShells.cpp:3728 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `cancelrequests.cancelled` | `Cancelled…` | UIMiscShells.cpp:3747 + UIMiscShells.cpp:3757 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `itemmove.drop_item_softkey_dst` | `Drop item / SoftKey dst` | UIMiscShells.cpp:3853 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `itemmove.marked_tab_softkey_dst` | `Marked tab={} #{} — SoftKey dst` | UIMiscShells.cpp:3867 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `itemmove.moved` | `Moved #{}→{}` | UIMiscShells.cpp:3873 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `itemmove.src_softkey_dst` | `Src #{} — SoftKey dst` | UIMiscShells.cpp:3895 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `itemmove.moved_2` | `Moved →{}` | UIMiscShells.cpp:3904 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `itemmove.move` | `Move {}` | UIMiscShells.cpp:3929 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `fadeyesno.y_enter_yes_n_esc_no` | `Y/Enter=Yes  N/Esc=No` | UIMiscShells.cpp:3940 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `contextmenu.info` | `Info` | UIMiscShells.cpp:4113 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `contextmenu.whisper` | `Whisper` | UIMiscShells.cpp:4114 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `contextmenu.party` | `Party` | UIMiscShells.cpp:4115 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `contextmenu.report` | `Report` | UIMiscShells.cpp:4116 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `contextmenu.trade` | `Trade` | UIMiscShells.cpp:4117 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `contextmenu.buddy` | `Buddy` | UIMiscShells.cpp:4118 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `contextmenu.messenger` | `Messenger` | UIMiscShells.cpp:4119 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `contextmenu.guild` | `Guild` | UIMiscShells.cpp:4120 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `contextmenu.family` | `Family` | UIMiscShells.cpp:4121 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `contextmenu.fame` | `Fame` | UIMiscShells.cpp:4122 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `eventwindow.event_softkey_note_e_uievent` | `Event — SoftKey note / E=UIEvent` | UIMiscShells.cpp:4182 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `multiline.multiline_softkey_text` | `MultiLine — SoftKey text` | UIMiscShells.cpp:4380 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `multiline.no_chalk_kite` | `No chalk/kite` | UIMiscShells.cpp:4411 + UIMiscShells.cpp:4448 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `macro.macro_softkey_name` | `Macro — SoftKey name` | UIMiscShells.cpp:5374 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.saved` | `Saved: {}` | UIMiscShells.cpp:5401 + UIMiscShells.cpp:5791 + UIMiscShells.cpp:5812 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.assigned` | `Assigned {}` | UIMiscShells.cpp:5410 + UIMiscShells.cpp:5422 + UIMiscShells.cpp:5821 …×4 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `common.macro_full` | `Macro full` | UIMiscShells.cpp:5413 + UIMiscShells.cpp:5424 + UIMiscShells.cpp:5823 …×4 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `utildlgexavatar.avatar_softkey_emote` | `Avatar — SoftKey emote` | UIMiscShells.cpp:5445 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `utildlgexavatar.emote` | `Emote {}` | UIMiscShells.cpp:5458 + UIMiscShells.cpp:5484 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `utildlgexmultipet.multipet_softkey_name` | `MultiPet — SoftKey name` | UIMiscShells.cpp:5553 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `utildlgexmultipet.no_517_tag` | `No 517* tag` | UIMiscShells.cpp:5569 + UIMiscShells.cpp:5595 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `utildlgexmultipet.rename` | `Rename: {}` | UIMiscShells.cpp:5573 + UIMiscShells.cpp:5597 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `exceptionitemsearch.exsearch_softkey_query` | `ExSearch — SoftKey query` | UIMiscShells.cpp:5607 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `mobgage.mob` | `Mob` | UIMiscShells.cpp:5662 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `mobgage.txt` | `{}%` | UIMiscShells.cpp:5695 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `skillup.skill_up` | `Skill Up` | UIMiscShells.cpp:5713 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `skillup.sp` | `SP {}` | UIMiscShells.cpp:5725 + UIMiscShells.cpp:5733 + UIMiscShells.cpp:5756 …×4 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `macroskill.macroskill_softkey_name` | `MacroSkill — SoftKey name` | UIMiscShells.cpp:5776 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `questicon.q` | `Q{}` | UIMiscShells.cpp:5859 + UIMiscShells.cpp:5868 + UIMiscShells.cpp:5895 …×4 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `questicon.done` | `Done {}` | UIMiscShells.cpp:5917 + UIMiscShells.cpp:5928 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `questicon.forfeit` | `Forfeit {}` | UIMiscShells.cpp:5939 + UIMiscShells.cpp:5946 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `minigametable.table` | `Table` | UIMiscShells.cpp:5959 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `minigametable.place_p` | `Place ({},{}) p={}` | UIMiscShells.cpp:5996 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `minigametable.card` | `Card #{}` | UIMiscShells.cpp:6047 + UIMiscShells.cpp:6084 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `minigametable.omok` | `Omok {}` | UIMiscShells.cpp:6061 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `teleport.tip_first_map` | `Enter=1st map #{} \| SoftKey map/name \| S=save` | UIMiscShells.cpp:955 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `teleport.tip_vip_map` | `Enter=1st map VIP#{} \| SoftKey map/name \| S=save` | UIMiscShells.cpp:957 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
| `teleport.tip_empty` | `Enter=1st map (empty) \| SoftKey map/name \| S=save` | UIMiscShells.cpp:958 | 本包自造，无原生依据（Wave-A 壳窗） | 待用户定措辞 |
