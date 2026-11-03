# R27-B1c 非位图文案接入 i18n —— 刷新通道 + 全量清点

> 用户定调（2026-11-01）：**不是位图画出来的文案一律走国际化**，默认中文、配置驱动、
> 后期可运行时切换。位图烘焙文字（原生 `ReqLevel` 那类精灵字、能力窗标签、装备槽名）
> 不在此列。本目录是 B1c 的**起点台账**，不是完工证明。

## 1. 本轮落地的两件（都已编译取证）

### 1.1 运行时刷新通道真的接通了

三截管道此前只通了两截半：

| 截 | 位置 | 状态（本轮前） |
| --- | --- | --- |
| 配置键 `ui.language` | [Configuration.cpp:185](../../../../../Configuration.cpp) `settings.emplace<UiLanguage>()`，分组标题「界面」见 `Configuration.cpp:114` | 已有 |
| `Locale` 按语言载 ini | [Util/Locale.cpp:80-91](../../../../../Util/Locale.cpp) `reload()` 读 `get_ui_language()`，载 `locale/<lang>.ini` + `zh-CN` 兜底 | 已有 |
| **谁去触发 reload** | `Configuration::reload_if_changed()`（`Configuration.cpp:493`，内含 `Configuration.cpp:507` 的 `Locale::get().reload()`） | **全仓零调用点** ⇒ 头文件注释「运行时按 mtime 变化自动热重载」是空话 |

本轮在主循环首行接上：[MapleStory.cpp:240](../../../../../MapleStory.cpp)

```cpp
while (running())
{
    Configuration::get().reload_if_changed();   // xy-config mtime 变 -> 重解析 -> 语言变则 Locale::reload()
```

代价核过：`reload_if_changed()` 未命中 mtime 变化时只做一次 `stat`（`Configuration.cpp:493-501`），
命中才重解析 yaml。所以每帧调用不引入可测开销，也不需要额外的定时器或事件源。

**仍然没通的那一截（不在本轮，需拍板，见 §4 决策 C）**：窗口不重建。
`UI::emplace`（[IO/UI.h:79-93](../../../../../IO/UI.h)）走 `pre_add`，元素已存在时只 `toggle_active`
而**不重新构造**（[IO/UIStateGame.cpp:765](../../../../../IO/UIStateGame.cpp) `pre_add` 命中已存在元素时
只 `element->toggle_active()`），只有 `remove` 才 `release()`。
所以语言切换后，**构造期缓存的 `Text` 成员保持旧文案**，直到该窗被关掉重开。
现仓里能立刻生效的只有「每次刷新都重取」的活文案——惯用法是在 update / 事件路径里
`change_text(Locale::text(...))`（装备 tooltip 就是这么写的）。

### 1.2 装备 tooltip 预览侧潜能行改用既有键

`IO/Components/EquipTooltip.cpp:195-213` 五颗 potential（`POT_HIDDEN/RARE/EPIC/UNIQUE/LEGENDARY`）
此前还是英文字面量，而**真实侧**同一套早就走 `equip.pot.*`（同文件 440-456 行）。
也就是说 B3b 只改了主支，预览支（鼠标还在拖动/未落定时那条）漏掉了。
本轮补齐，**没有新增任何词条**——`locale/zh-CN.ini:19-23`、`locale/en-US.ini:19-23` 键已存在。

这类「同一逻辑两支分叉、只改了一支」的漏法，靠真机悬停很难抓到（要先造出带潜能的装备），
所以留了 §2 的门禁。

## 2. 门禁读数

```
python docs/features/ui-v83-window-restore/tools/check_locale_keys.py   # GATE OK, exit 0
```

zh 93 键 / en 93 键（键集严格对齐），`equip.type.` 32 条、`equip.stat.` 15 条、
`equip.speed.` 9 条枚举全部命中，缺 0。原始输出见 [gate-check-locale-keys.txt](gate-check-locale-keys.txt)。

## 3. 全量清点：还剩多少硬编码（`[inventory.txt](inventory.txt)`）

清点工具 `tools/scan_display_text.py`（只读）。三个模式命中展示文案落点：
`change_text("…")` / `Text(Text::Font::…, "…")` / `show_text(Tooltip::Parent::…, "…")`。
**已经走 i18n 的行不算站点**（引号紧挨 `Locale::*(` 之后，或字面量本身就是 ini 键名）——
第一版没做这个排除，把 `Locale::text("equip.label_type")` 本身也当成了硬编码，虚报 12 处。

| 桶 | 处数 | 说明 |
| --- | --- | --- |
| 英文原文 · **Wave-A 壳窗** | **333** | 壳窗判据：类名出现在 `Net/Handlers/WaveAShellHandlers.cpp`（56 个类）+ `UIMiscShells.cpp` 自身 |
| 英文原文 · 其余窗 | **49** | 见下 |
| 中文硬编码 | **20** | 8 个文件，见 inventory 末节 |
| 合计 | 402 | |

⚠ 分桶只按「Wave-A 壳窗」这一个判据，**偏乐观**：`UIPlant`(11)、`UIMinigame`(6)、
`UIDailyGift`(4+8)、`UIMegaphone`(3)、`UIJoypad`(2) 没进那个 handler 文件，但同样是本包
未接入玩法的壳，实际「在用」比 49 更少。

49 处里真正是玩家天天看的，只有零星几处，其余是初值或符号：

| 文件 | 处数 | 性质 |
| --- | --- | --- |
| `UIPlant.cpp` / `UIMinigame.cpp` / `UIMegaphone.cpp` / `UIDailyGift.cpp` / `UIJoypad.cpp` | 26 | 壳窗（分桶判据漏网） |
| `UIMapInfoPanel.cpp` `UIMapMobPanel.cpp` `UIMapDropPanel.cpp` | 8 | 「No monsters on this map / No drop data / mesos」——本包自造的地图信息面板，**原生无对应文案** |
| `UIUserList.cpp` | 5 | `none` ×3、`My Location - `、`Default Group (0/0)`——**原生有对应**（`String.img/User`） |
| `UICharSelect.cpp` `UICommonCreation.cpp` `UILogin.cpp` `UIRaceSelect.cpp` | 6 | `Ver. ` ×4 + `1`——版本号前缀 |
| `UIRegion.cpp` | 2 | NA/EU 延迟警告整句——**国服不会走到** |
| `UIViewAllChar.cpp` | 2 | `World ` ×2 |

20 处中文硬编码（`金币: `×3、`地图列表`、`好友 (`、`排行不可用`×2、`队伍 #`、
`属性: - 抗性: -`、`第/天/月/日`…）性质不同：**中文已经写好了，只是没进 ini**，
搬 key 零风险，不存在「编造原生文案」的问题。

## 4. 当时的三个决策点（已由用户 2026-11-03 拍板，结果见 §6）

门禁「禁止凭记忆编词条（换中文需要 v83 原生文案依据 `String.img`/IDA）」与
「非位图文字一律走 i18n、默认中文」这两条，在**壳窗**这一大类上是冲突的：
壳窗的原生文案本来就没人看过（原生 v83 里这些窗是另一套 UI），要么去取证，要么承认是自造文案。

- **决策 A（范围）**：402 处里只做「在用窗 + 有原生依据」的（个位数~十几处），
  还是连壳窗一起接机制？
- **决策 B（无依据文案的中文）**：自造提示（`No drop data`、`Sent…`、`排行不可用` 这类）
  的原生对应不存在 ⇒ 允许按语义自拟中文并在 ini 里标「无原生依据」，还是保持英文原值？
- **决策 C（刷新语义）**：切语言后已开着的窗要不要即时改文案？三选一——
  ①要求重开窗（现状，改动为零）；②在 update 里逐帧 `change_text`（活文案，改动散）；
  ③给 `Locale::reload()` 挂一个广播，让 `UIElement` 自选订阅（机制统一，动公共件）。

## 5. 编译取证方式与一个假警报（记下来免得再踩）

本轮**没跑整包 MSBuild**（OutDir 即运行目录、当时有两个 `MapleStory.exe` 在跑；
且两个 MSBuild 并发会争 `x64/obj-Release/vc145.pdb` 成片 C1041）。改用 `cl /c` 只编受影响 TU：

```
cl /c /utf-8 /bigobj /std:c++17 /EHsc /W3 /DNDEBUG /D_CONSOLE /DUSE_NX … MapleStory.cpp IO/Components/EquipTooltip.cpp
=> CL_EXIT=0
```

**假警报**：第一次漏了 `/utf-8`，编译报
`IO/Components/ChatBalloon.h(73): error C2447: “{”: 缺少函数标题`，看起来像并行会话写坏了公共头。
实际上 73 行是一条**中文注释**——没有 `/utf-8` 时 cl 按 cp936 解 UTF-8，某个中文字节的尾部
把紧随其后的换行吞进双字节字符，注释就"续"到了下一行的 `class` 上。
`MapleStory.vcxproj:102` 等 4 处 `AdditionalOptions` 里明确带 `/utf-8`，
**凡是手工 `cl` 复现构建，必须把项目附加选项一起抄上**，否则报的是编码错，不是代码错。

## 6. 三个决策点已定（2026-11-03 用户拍板）与 B1c-2 落地

| 决策 | 拍板结果 | 对本批的含义 |
| --- | --- | --- |
| A 范围 | **全量 402 处都接机制** | 壳窗不因「无原生依据」被排除；依据缺失只影响**值**，不影响**键位** |
| B 文案 | **保持英文原值，只建键** | 不编中文、不编英文；逐条记入 [pending-copy.md](pending-copy.md)，日后取证只改 ini 的值 |
| C 刷新 | **reload 广播，窗自取** | 已开着的窗切语言立刻换文案，不要求重开 |

B1c-2（机制层）已按 C 落地：`Locale::generation()` 代次 + `UIElement::bind_text()` 绑定表 +
三个 `UIState` 派发点各一行 `refresh_localized()`，6 个静态标签上表，4 个新键（`version.prefix`、
`charselect.rank_*`）。两个原决策点问题在实现时这样收：

- **为什么不用全局注册表 / 绘制期自取**：`Text::draw()` 是 const（绘制期取文案要改状态）；
  `Text` 有隐式拷贝赋值（`EquipTooltip.cpp:196` 就是 `x = Text(...)`），全局表存指针会在赋值后失效。
- **为什么只给静态标签上表**：绑定等于「把登记那一刻的值钉死」。金币数额、`Default Group (0/0)`
  这类随数据变的行继续 `Locale::text/format` 就地现算 —— 那条路本来就是活文案。

**门禁读数**：
`check_locale_keys.py` GATE OK（zh/en 各 97 键，键集对齐）；
`layout.py check` 十张 spec 对自身 oracle 帧 **全 rc=0 / 逐格 Δ=0**
（[gate-layout-regression.txt](gate-layout-regression.txt)）—— 本批只加机制与键，
zh 的 4 个新值与站点原英文字面量逐字相同，**不改变渲染串**，所以版式零风险是可证的而不是假设的。

**又一个假警报（工具姿势，已修）**：第一次跑这十张时循环取的是 spec 的 `native_frame` 字段 ——
那字段是「这帧从哪截的」人读说明（常带 `(348x305)` 尺寸后缀，甚至是整段中文叙述），
**不是路径**；能传的是 `oracle_frame`。传错后 `FileNotFoundError` 的 traceback 走 stderr，
而循环只留 stdout，于是看到「10/10 rc=1 且一个字没输出」，极易误读成版式全崩。
已在 `layout.py cmd_check` 加帧路径存在性拦截（rc=2，并把该传哪个字段打出来）。

**运行时负对照仍未做**（唯一欠账）：需要一次能覆盖 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` 的窗口
+ 整工程 Rebuild（`UIElement` 加了两个成员，实例尺寸变了，增量编译会留旧尺寸的目标文件）。
判据已备好：改 `xy-config.yaml` 的 `ui.language` 后看 `Locale refresh gen=` 日志 ——
不靠给某个键临时编一条两种语言不同的假文案来验机制。

## 7. B1c-3（在用窗 25 处）落地 + 机制层被迫改形（2026-11-03）

按 §6 的三条拍板接完 `UIUserList`(5) `UIMapInfoPanel`(4) `UIMapMobPanel`(1) `UIMapDropPanel`(3)
`UIViewAllChar`(2) `UIRegion`(2) `UIJoypad`(2) 共 19 处（另 6 处 `Ver. `/`charselect.rank_*` 属 B1c-2，
本批不重复），新增 12 键进两份 ini（zh/en 各 109，键集对齐）。清点器读数随之下移：
`sites=377 latin=359 shell=333 live=26 cjk=18`（原 402）。

### 7.1 机制层改形：绑定表从 UIElement 里搬出来，成了 `IO/LocaleBindings.h`

这不是重构洁癖，是**实现时被证伪的假设**。B1c-2 的判据「静态标签 ⇒ 进 UIElement 绑定表」默认了
「所有窗体都是 UIElement」。世界地图那三块侧栏（`UIMapMobPanel` / `UIMapDropPanel` /
`UIMapInfoPanel`）按设计**不是** UIElement —— 头文件里原话「Adsorb to the left of the world map.
Not a UIElement — owned by UIWorldMap」，它们由 `UIWorldMap` 按值持有、`UIWorldMap::update()`
逐帧调各自空着的 `update()`（`UIWorldMap.cpp:283-286`）。`cl` 的直接报错就是证据：

```
IO\UITypes\UIMapDropPanel.cpp(39): error C3861: “bind_text”: 找不到标识符   （Mob/Info 同）
```

三条出路里选了「搬出来」：① 让面板改继承 UIElement —— 它们是吸附绘制件，不是可焦点/可点击窗体，
套 `get_type()`/`send_cursor` 契约是假继承；② 在每块面板里再写一份代次比较 —— 机制出现第二套实现，
日后必然走样；③ **把绑定表抽成 `LocaleBindings`，UIElement 持有一份并转发，面板也持有一份、
在自己的 `update()` 里 `refresh()`**。C 决策「reload 广播、窗自取」的形态没变，只是「自取」的
派发点从「UIState 一处」扩成「UIElement 走派发点 + 非 UIElement 走宿主 update」。

顺手把两条不变量写进类型里：`LocaleBindings` **禁拷贝**（表里存的是指向宿主自己按值持有的 `Text`
的裸指针，宿主被拷贝后指针指到原件，改动落在看不见的地方 —— 让这种拷贝变成编译期错误，而不是
留给 review）；`refresh()` 返回 bool，日志口径保持与 B1c-2 完全一致（`Locale refresh gen= … rebound N labels (…)`），
面板那条带宿主名，UIElement 那条带 `get_type()`。

### 7.2 绑定 vs 活文案：本批按「有无第二个写入点」逐条判

- 进表：三块面板的空态标签、`meso_label`、`UIUserList` 五条标签（grep 全文只有 ctor 赋值 + `draw`，
  没有第三条写入点）、`UIJoypad::key_text` 的 ctor 那一处（另一处「恢复默认」写的也是同一个键）。
- 不进表：`meso_amount`（`UIMapInfoPanel.cpp:381` 每帧按数据 `change_text`，绑上去会把数额钉成 `"0"`）、
  `mapdrop.unknown_item` / `viewallchar.world` / `region.warn_*`（都是数据或 hover 现取，本来就是活文案）。
- `userlist.cur_location` 用**带值**那一档：前半截是文案、后半截是本窗生命周期内恒定的位置名，
  拆成两条键会让中文拿到半句。

### 7.3 门禁读数（本批）

见 [gate-r27-b1c3.txt](gate-r27-b1c3.txt)：`cl /c /utf-8 /bigobj` 9 TU 全过（含 `UIElement.cpp`
与 `UIWorldMap.cpp`，因为公共件改了形）；`check_locale_keys.py` GATE OK 109/109；
`layout.py check` 十张 spec 对自身 oracle 帧仍全 rc=0 / Δ=0。

**运行时负对照的欠账变大了**：`UIElement` 私有段从「vector + uint32」换成「一个持有同样
vector + uint32 + 宿主名的 `LocaleBindings`」，实例尺寸**又变了一次**（多一个 `std::string`），
所以整工程 Rebuild 依旧不可省。判据不变：改 `xy-config.yaml` 的 `ui.language`，
看 `Locale refresh gen=` 日志有没有把绑了表的窗逐条报出来（面板那三条现在也在这个口径里）。
## 8. B1c-4（中文硬编码批次）落地（2026-11-03）

按 §6 的三条拍板接完 `UIBuddyList`(3) `UIParty`(3) `UIStorage`(3) `UITrade`(5)
`UIMapSearchPanel`(5) `UIMapInfoPanel`(2) `UIDailyGift`(14) —— **35 处中文字面量，不是清点器
说的 18 处**。差的那 17 处不是漏搜，是**旧扫描器的形状盲区**：它按行匹配
`change_text("..")` / `Text(Text::Font::.. , "..")` / `show_text(..)` 三种形状，于是
① 实参折到了下一行（`UIDailyGift.cpp:131` 的 `footer_rules[i] = Text(字体…, 颜色…,` 后面才是那句中文）、
② 三元的另一支 `mob.boss ? "BOSS" : "普通"`、③ 拼接片段 `s += " 频道" + std::to_string(n)`
全都不在它的眼里。这批补了第二把尺 [`scan_cjk_literals.py`](../../tools/scan_cjk_literals.py)
（不问形状，只问「代码行里有没有带汉字的字符串字面量」，注释剔除），产物
[cjk-scan-full.txt](cjk-scan-full.txt)：开工前 65 → 本批 35 → 现余 30，剩余 30 处全在后续批次的
文件里（`IO/Window.cpp` 11 是标题栏拼接与 LOG、壳窗 `UIQuestLog` 10 / `UIGuild` 5 /
`UIGender` 2 / `UIMerchant` 1 / `UIMiscShells` 1）。**B1c-5/6 开工以这把尺为准**，
`sites=` 只当英文（latin）口径继续用。

### 8.1 分档：本批把「宿主档位」这条硬约定用实了

PLAN §B1c-3 写下的硬约定是「动手前先分宿主档位」。本批的分布：

| 档 | 站点 | 刷新通道 |
| --- | --- | --- |
| UIElement 子类 | `UIDailyGift`(14，经 `UIDragElement<PosDAILYGIFT>` 继承) | ctor `bind_text` → UIState 三个派发点 `refresh_localized()` |
| 非 UIElement 吸附面板 | `UIMapSearchPanel`(3 进表) | 自带 `LocaleBindings{ "UIMapSearchPanel" }`，在自身 `update()` 里 `refresh()` |
| 数据驱动活文案 | 其余 18 处 | 赋值点 `Locale::text/format` 现取 |

`UIMapSearchPanel::update()` 的刷新点放在 `if (!visible) return;` **之前**：隐藏期间语言换了也要
让绑定表跟上，否则面板重新打开的那一帧先闪一次旧语言文案；代次没变只有一次整数比较，不成热点。

### 8.2 绑定 vs 活文案：三条新判到的形状

- **行对象禁上表**（`UIMapInfoPanel` 的 `row.type_label` / `row.attr_label`）：`mob_rows` 每次重建都
  `clear()` + `push_back(std::move(row))`（`:473`/`:531`），绑定表存的是指向 `Text` 的裸指针 ——
  上表等于把悬垂指针留在表里。所以只能在赋值点现取。这条是 `LocaleBindings` 禁拷贝之外
  同一个不变量的另一种破法，值得单独记。
- **draw 里的局部 `Text`**（`UIMapSearchPanel` 的 `nodata2`）：对象一帧就没了，绑定无从谈起，
  也只能现取；好处是每帧重建 => 切语言下一帧就换字。
- **事件驱动活文案的切换延迟**（`UIStorage::mesolabel` 三个写入点全在 ctor 与包回调里）：
  切语言后到下一次 `set_storage` / `set_meso` 之前，它仍是旧语言。这是「reload 广播、窗自取」
  在**没进表的标签**上的已知边界；`UIBuddyList` / `UIParty` / `UITrade` 的 title 因为每帧在
  `draw()` 里重建（成员是 `mutable OutlinedText`），切换是即时的。要消除这层延迟就得把
  `mesolabel` 改成每帧现取或给窗体加一个「数据版 binding」，本批按最小改动**没有**动它，
  留在此处明说。

### 8.3 键位形态：模板留 `{}`、空格留在代码

新增 32 键（`buddylist.title` `common.channel` `common.offline` `party.title` `storage.meso`
`trade.title` `trade.waiting` `trade.meso` `trade.locked` `mapsearch.list_header`
`mapsearch.no_data` `mapsearch.cost` `mapsearch.street` `mapsearch.tab_unloaded`
`mapinfo.type_boss` `mapinfo.type_normal` `mapinfo.attr` `mapinfo.resist` `dailygift.*`×14）。
两条口径：① 「文案 + 数据」整句存成一个带 `{}` 的模板（`好友 ({})`、`金币: {}`、`队伍 #{}`），
拆成前后两截会让某个语言拿到半句；② **空格一律留在代码里拼** —— `Locale.cpp:70` 的加载器
`trim` 掉值两端空白，把 `Day ` 这种尾随空格写进 ini 会直接丢。
`common.channel` / `common.offline` 是好友窗与队伍窗共用的状态词，只有一个真源。

这批原值本来就是中文，zh 侧逐字照抄、零编造风险；en 侧同值占位（决策 B：本批通的是键位与
机制，不是翻译），32 键全部进 [pending-copy.md](pending-copy.md) 台账等取证。

### 8.4 门禁读数

见 [gate-r27-b1c4.txt](gate-r27-b1c4.txt)：`cl /c /utf-8 /bigobj` 8 TU 全过（CL_EXIT=0，error 0 行）；
`check_locale_keys.py` GATE OK（两份键集相等、无重复）；`layout.py check` 十张 spec 对自身
oracle 帧仍全 rc=0 / Δ=0；`scan_display_text.py` `sites=377→359`、`cjk=18→0`。

### 8.5 欠账（与 B1c-3 同一条，未变）

`MapleStory.exe` 仍占着 `OutDir=E:\MXD\BeiDou-Client-xy\` ⇒ 整工程 Rebuild 与**运行时负对照**
（改 `xy-config.yaml` 的 `ui.language`，看 `Locale refresh gen=` 把绑了表的窗逐条报出来）做不了。
按用户既有选择等他自行关闭客户端，不得代杀进程。本批没改 `UIElement`/`LocaleBindings` 的实例
尺寸（只加站点），所以 Rebuild 的理由仍是 B1c-2/B1c-3 那两次改形，不是本批。

## 9. B1c-5a（壳窗第一批 · 5 文件 80 处 / 49 键）落地（2026-11-03）

范围：`UIVegaSpell` 18、`UIRpsGame` 15、`UIPlant`（含 `UIRaise`）19、`UIViciousHammer` 10、
`UIWedding` 18 —— 数字是 **`Locale::` 调用点数**，一个调用点吃掉一处字面量。键 49 个，
zh / en 同值（决策 B：建键不翻译），值全部逐字来自改动前的英文字面量，没有编造。

### 9.1 机制层被迫补的第二件事：`Locale::format_all`

`rps.round_result`（`You:{} NPC:{}`）和 `wedding.progress`（`Step {}  G:{} B:{}`）都是**一条整句里
两个以上数据位**。原有 `Locale::format` 只认第一个 `{}`，要接住这类句子只有两条路：

1. 拆成若干条键在 C++ 里拼 —— 等于把**语序**钉进代码，ini 不再是真源（「A: 1 B: 2」在别的语言里
   可能整体反过来），与用户「机制必须通用」的定调冲突；
2. 一个按 `{}` 出现顺序依次填值的入口 —— 语序留在 ini，代码只交值。

选了 ②，`Util/Locale.h` 加 `format_all(key, std::initializer_list<std::string>)`，占位符与值数量
不匹配时沿用 `format` 的脾气（多出来的值拼在末尾）——宁可在界面上看出错位，也不在取值时抛异常。
只有这一个实现，没建第二套。

### 9.2 门禁层的第二个洞：反向门禁形同虚设（已修）

PLAN §4.2 要求「代码引用的键 ⇒ 两份 ini 都得有」，缺键不报错、只在界面上回显键名，所以它必须
是门禁。`check_locale_keys.py` 补上后**首版一条都没抓住**：正则要求键后紧跟 `+` / `,`，于是全站
最常见的单参 `Locale::text("k")` 全部漏网（只报 51 键，缺键清单只剩 `format` 族）。补 `\s*\)` 档
并按组数取尾字符后，读数回到 `代码引用键 121 个 / 动态前缀 5 个`、缺 0。
教训：**新门禁要先用已知缺陷做负对照**，只看它「跑通了」等于没跑。
它补上后第一次跑就抓到一个存量缺键 —— `63080c0`（R27-B7-4 系统设置窗）在
`UIOptionMenu.cpp:788/:789` 引用 `sysopt.combo.screenshot_folder` / `sysopt.combo.mob_name_hp`
却没建档；那两键的修复躺在另一个会话的在途 hunk 里，本批不代它落（口径细节见
[gate-r27-b1c5a.txt](gate-r27-b1c5a.txt) §2 末）。

### 9.3 站点计数是下界：`UISoftKey` 的第三实参扫描器看不见

`UISoftKey(Ok, Cancel, "<提示语>")` 第三参是要显示在软键盘上的文案，`scan_display_text.py` 的三种
形状（`change_text` / `Text(Text::Font..)` / `show_text`）都不覆盖它 ⇒ 本轮人工清点接入 16 处
（清单见 [gate-r27-b1c5a.txt](gate-r27-b1c5a.txt) §5）。全站还有多少这类站点**未知**，`402` / `304`
都是下界。该形状里同时存在两种性质完全不同的串：

| 性质 | 例 | 处理 |
| --- | --- | --- |
| 真提示语 | `UIBuddyList.cpp:200` `"Buddy name"`、`UILogin.cpp:586/592` `"Account"/"Password"`、`UICharSelect.cpp:1452/1455` PIC 提示 | 该建键 |
| 输入语法 | `rps.softkey_choices`=`rock\|paper\|scissor`、`"m\|f"`、`"y\|n"`、`"save\|close"` | 值**不可翻译**：解析侧按英文 token 比较，翻译等于改输入契约 |

本批的处置是「两类都建键、第二类在 ini 里写 `# 禁译` 注释」——机制归零优先，禁译靠注释挡。
是否改成「输入语法直接进白名单不建键」、以及要不要把该形状扩进扫描器（会重算全站总数），
**待用户抉择**，不擅自扩量。

### 9.4 活文案为主：本批一个绑定点都没加

这 5 个窗的 `status` / `tip_label` 每处都在事件回调里被改写（有第二个写入点）⇒ 按 §7.2 判据一律
**不上绑定表**，ctor 初值改成 `Locale::text(...)` 现取。代价说清：切语言后**已开着的窗**要等下一次
数据事件才换新文案，本批不引入「refresh 时把事件文案重置回默认」这种更糟的行为。
`UIWedding` 的 `"Wedding invite"` 保持英文字面量未建键：它是**出站悄悄话正文**（进封包给对端看），
不是本端展示文案，翻译会改变对方收到的内容 —— 属协议层，需用户单独定策。

### 9.5 门禁读数

见 [gate-r27-b1c5a.txt](gate-r27-b1c5a.txt)：6 TU `cl /c /utf-8 /bigobj` CL_EXIT=0、error 0 行；
`check_locale_keys.py` `190/190` 键 + 反向门禁缺 0；`layout.py check` 十张 spec 61 项断言不合格 0；
`scan_display_text.py` `sites=359→304`，本批 5 文件余站 0；`scan_cjk_literals.py` 仍 30（本批无中文站）。

### 9.6 欠账

运行时负对照同 §8.5，未变。B1c-5 还剩 `5b`（其余壳窗 latin，UIMemo 7 / UIAdmin 6 / UIMinigame 6 /
UIAriantMatch 5 / UICharInfo 4 / UIMonsterBook 4 / UINewYearsCard 4 / UIPartyRace 4 / 一批 ≤3 处）与
`5c`（4 文件 18 处中文盲区：UIQuestLog 10 / UIGuild 5 / UIGender 2 / UIMerchant 1）。
