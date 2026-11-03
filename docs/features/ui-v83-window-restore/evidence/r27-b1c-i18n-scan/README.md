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
**本段的逐文件数字已被 §10 的新基线顶掉**（那条尺当时还看不见软键盘档）。

## 10. 扫描器补第四形状 + 全站基线重算（2026-11-03，用户拍板「扩扫描器 + 全站扫一遍软键盘位」）

§9.3 留的那个洞按用户抉择补上：`scan_display_text.py` 的 `PATTERNS` 从「三形状」改成「带名字的四形状」，
新增 `SoftKey hint` 一档，收法 `\}\s*,\s*"…"\s*[,)]`（即 `emplace<UISoftKey>(ok, cancel, "<提示语>")`
的第三实参；不认调用名，因为闭包体一长，调用头与实参就不在同一行）。报告同时改为**按形状分档计数**，
并把形状名打进每一行站点，方便下一批直接按档取舍。

| 读数 | 旧（三形状） | 新（四形状） |
| --- | --- | --- |
| 全站站点 | 304 | **730** |
| 分形状 | `change_text` 283 / `Text ctor` 21 / `Tooltip` 0 | 同上 + **`SoftKey hint` 426** |
| 分桶 | 壳窗 / 在用窗 | **壳窗 654（`UIMiscShells` 独占 388）/ 在用窗 76** |
| 中文档 | 0 | 0（不变，中文由 `scan_cjk_literals.py` 管，见 §8.4） |

**新档不是泛匹配**（这是本次唯一需要自证的点：`}, ".."` 在 C++ 里到处都有）：对 426 条逐条回查语句头，
424 条在 40 行内出现 `UISoftKey`；剩下 2 条（`UI/UITypes/UIGuild.cpp:357`、`UI/UITypes/UIOptionMenu.cpp:342`）
是闭包体超过 40 行的同一形状 ⇒ 全是真软键盘提示，**零污染**。反向也核过：5a 已接的 5 个文件
（`UIRpsGame`/`UIWedding`/`UIViciousHammer`/`UIPlant`/`UIVegaSpell`）在新档下余站 0，
说明 `already_localized` 对 `Locale::text("…")` 形态的第三实参照样跳过，没有把已改的站点算成漏网。

**口径变更的后果**（后面几批都按这条走）：
1. `402` / `304` 从此只是历史读数，**PLAN §3 的批次体量按新基线重写**：5b 余 266、6 是 388、
   另有一批在用窗软键盘位 76 处（`UIUserList` 11 / `UIChat` 10 / `UIMinigame` 9 / `UIDailyGift` 5 /
   `UIMegaphone` 5 / `UILoginNotice` 5 …）——这些在 B1c-3 时代根本数不出来。
2. 730 **依旧是下界**：实参折到下一行的调用仍看不见（与 `Text` ctor 档同一条已知盲区）。
3. 软键盘档里两类性质（真提示语 vs 被代码按 token 比较的输入语法）继续按用户拍板②处置：**都建键、
   后者在 ini 标 `# 禁译`**（§9.3 表），机制归零优先，禁译靠注释 + `pending-copy.md` 台账兜。
4. 原生侧对软键盘的取证结果见 [../native-verdicts-2026-11-03/copy-literals.txt](../native-verdicts-2026-11-03/copy-literals.txt)：
   素材存在（`UI.nx` 有 `Login.img/Common/SoftKey`、`UIWindow.img/SoftKeyboard`），但 **v83 北斗 exe 不引用它**
   （v95 端才加载）⇒ 这批提示语没有原生文案可对齐，键值只能保持现值等取证，**优先级低于在用窗**。

门禁：新尺只改文档与工具，未动业务代码 ⇒ 无需重编译；`check_locale_keys.py` 与 `layout.py check` 的读数
沿用 §9.5（本批无键位变化）。

### 10.1 §4.1 白名单落地（原生判据驱动，730 ⇒ **713**）

用户新规矩「待确认方案先查原生」的第一批结果直接决定了这条：白名单候选在原生 v83 全是
**格式串或初值**，不是可翻译文案 —— `"%d ~ %d"`（`0x6F2928`）、`" %d%%"`（`0x73BCC0`）、
`"EXP : %d / %d"`（`0x73BB5C`，原生 EXP 连「两位小数百分比」这个形态都没有）；`-` `!` `V` 与纯数字初值
在 ASCII / UTF-16LE / GBK 三编码下**全 0 命中**、`String.nx` 无对应节点（逐条证据见
[../native-verdicts-2026-11-03/copy-literals.txt](../native-verdicts-2026-11-03/copy-literals.txt)）。

实现照 §4.1 的「不许偷偷放行」：**命中不删，单列一档** —— 报告末尾「白名单」段把 17 处逐条带
`文件:行` + 理由打出来，`WHITELIST` 常量里每个键自带一句原生依据，新增键必须同时补依据。
分档结果：`0%`×5（全在 `UIStatsInfo.cpp:564-574`）/ ` ~ `×3 / `-`×2 / `0.00%`×2 / `!` `V` `0` `1` `2` 各 1。

**两条刻意留在待办里**：`UIClock.cpp:74` `"0s"` 带英文单位 `s`（中文要写「秒」）⇒ 算文案，归 B1c-5b；
`UIChatBar.cpp:1781` `"/w "` 是**打字命令 token**，原生根本没有该 token（悄悄话在原生是 `BtWhisper`
按钮 / 双击流，`BtWhisper` UTF-16LE `0x73C4A8`、帮助串 `"Double-click to send a whisper."` ASCII `0x73C138`）
⇒ 问题不是「要不要翻译」而是「要不要这条自造入口」，**交回用户**（见「原生裁决台账」）。

## 11. 原生裁决台账（用户新规矩：待确认先查原生，原生有就照原生，查无才交回）

这一节是**跨批次**的，不只 i18n。每条都标「原生有 → 已照原生落地」或「原生查无 → 交回用户」，
证据落 [../native-verdicts-2026-11-03/](../native-verdicts-2026-11-03/)。

| # | 待确认项 | 原生取证结果 | 处置 |
| --- | --- | --- | --- |
| 1 | §4.1 白名单候选 `-` ` ~ ` `!` `V` `0` `1` `2` `0%` `0.00%` | **原生有答案**：`"%d ~ %d"`(0x6F2928)、`" %d%%"`(0x73BCC0)、`"EXP : %d / %d"`(0x73BB5C) 是格式串；其余三编码 0 命中、String.nx 无节点 ⇒ 排版/初值，不是文案 | **已照原生落地**（§10.1，`WHITELIST` 进工具，站点 730⇒713） |
| 2 | 装备 tooltip 背板 `0.9α` 还是 `1.0α`（CHECKLIST §I.3 第 2 条） | **原生有答案 = 实心**：同一背板压米色窗 (79.2,79.2,119.6) vs 压深色画面 (78.6,78.7,124.2) 只差 ΔR=0.6（0.9α 应差 ≈21）；主会话独立复核：外缘亮度 0..255 起伏、内部 R=G 无绿染（[native-tooltip-alpha-raw.txt](../native-verdicts-2026-11-03/native-tooltip-alpha-raw.txt)） | **已照原生改 `TextTooltip.h` 0.9f ⇒ 1.0f**；4 TU `cl /c` CL_EXIT=0。附带事实：原生背板是**深蓝**（众数 (17,17,51)/(23,23,57)/(62,68,119)），我方仍是纯黑 ⇒ **颜色另开一条待量**，本批只动 α |
| 3 | tooltip 有无外描边/投影 | 原生**无**独立描边与投影，靠背板自带的 1px 亮边 (85,85,130) + 底边 1px (17,18,47)；左/上缘那 1px「透出缝」是原生拼贴伪影 | 我方 1px 灰边**保留**（近似件），不复刻伪影 |
| 4 | `"/w "` 悄悄话打字命令 | **原生查无**：`/w` 三编码 0 命中，原生只有按钮 / 双击入口 | 交回用户：保留这条自造入口，还是删到只剩原生路径 |
| 5 | `"Wedding invite"`（5 处默认悄悄话正文，出站封包） | **原生查无先例**：原生婚礼邀请是**物品驱动**（`Etc.img/Etc/4031377` 邀请函-教堂，desc「双击邀请函输入您要邀请的角色名」），正文不由客户端硬编码 | 交回用户：建键（翻译会改对端收到的内容）/ 保持英文 / 改成走物品流程 |
| 6 | 软键盘提示位要不要接 i18n（**新基线里的 426 / 713**） | **原生 v83 不启用软键盘**：素材在（`UI.nx` 有 `Login.img/Common/SoftKey` 9 子节点、`UIWindow.img/SoftKeyboard`），但北斗 v83 exe **零引用**，v95 端才加载（`n/SoftKey/BtNum/%d`）。提示语文案（`"Buddy name"`、`"Account"/"Password"`、`rock\|paper\|scissor`）**两侧二进制都查无** | 交回用户：这 426 处（≈58% 的余量）要不要接 —— 它服务的 UI 在 v83 是死的。已按原生把**优先级**记下来：先用在产文案，软键盘排后 |

> 口径提醒：本台账只记「原生能裁的」。存档迁移、是否加配置开关、部署方式这类**我方内部工程选择**
> 原生不会回答，仍按原样挂着，不冒充「已按原生定案」。

## 12. B1c-5b（壳窗第二批 · 24 文件 98 键）落地（2026-11-03）

范围（24 个 `.cpp`，与 `cl /c` 编译单元一一对应）：`UIAdmin` 7、`UIAlliance` 4、`UIAriantMatch` 5、
`UICashGachapon` 2、`UICashShop` 2（+2 个 `bind_text`）、`UICharInfo` 5、`UIClock` 4、
`UIDailyGift`（10 个 `bind_text`）、`UIDelivery` 13、`UIFamily` 8、`UIFamilyTree` 9、`UIMTS` 7、
`UIMaker` 5、`UIMapleTV` 1、`UIMegaphone`（含 `UIAvatarMegaphone`）3、`UIMemo` 9、`UIMinigame` 6、
`UIMonsterBook` 9、`UIMonsterCarnival` 5、`UINewYearsCard` 4、`UIPartyRace` 4、`UIPartySearch` 5、
`UIRemoteGachapon` 1、`UIStoreBank` 6 —— 数字是**直接 `Locale::` 调用点数**。键 98 个，
zh / en 同值（决策 B，键表 [keys-5b.txt](keys-5b.txt)，台账见 §B1c-5b 段），站点零措辞改动。

### 12.1 本批立的两个新洞与补上的两把尺

**洞一：反向门禁形同虚设之外，「建完键」本身也没有尺。** `Locale::format` 只填第一个 `{}`、
`format_all` 按序填 —— 占位符比实参多，界面上留裸 `{}`；比实参少，值直接拼在句尾。**两种都不报错、
不崩**，肉眼之外无人知晓。98 条值一旦写进 ini，这个风险就从「一条」变成「全站」。

⇒ 新增 [../../tools/check_placeholder_arity.py](../../tools/check_placeholder_arity.py)：拿 ini 当判据，
逐个调用点对齐「实参数 / `{}` 数」；`format` 额外要求**恰好 1 个**（多了会留裸 `{}`，少了根本没法填）；
`Locale::enum_key("equip.stat.", …)` 这种运行时拼键的，改查**该前缀下全部键**是否符合契约。
`--both` 让 zh / en 各按各的值查。

**洞二：忠实度无人复核。** 建键是把字面量从代码搬进 ini，搬的时候**改动值**（丢单位、丢空格、
把两个插值合成一个占位）不会有任何编译或运行期信号。本批人工比对 `git diff` 抓到两条真缺陷：
- `delivery.row_meso`：丢了金币行的尾缀 `m`，还把「发送者 + 金额」两个插值塌成一个占位
  ⇒ 改回 `{}  {}m` + `format_all({p.sender, std::to_string(p.mesos)})`；
- `delivery.row_item`：原文两个空格只剩一个。

⇒ 新增 [../../tools/check_value_fidelity.py](../../tools/check_value_fidelity.py)（判据：ini 值里每一段
**非占位符**片段必须在**该键引用处的 HEAD 旧文件**里逐字出现；`--ini` 让真源 ini 参与校验，键表只当
本批键清单，防「表与 ini 漂了没人查」）：98/98 通过，见 [gate-r27-b1c5b-fidelity.txt](gate-r27-b1c5b-fidelity.txt)。
**这条尺必须在提交前跑** —— 它拿 `HEAD` 当原文，commit 之后 `HEAD` 就是改后代码，会变假阳性
（本批 commit 后实测 98 键里 75 条报查无）。
提交后复查要传 `--rev <该批的父提交>`，而**父提交不能默认写成 `HEAD~1`**：批后一旦再追工具/文档提交，
`HEAD~1` 就落在批自己身上（本批实测：批 = `HEAD~1`，传它得 75 条假「查无」；真正的父是 `7f4e75b`，
传 `--rev HEAD~2` 才 98/98）。这条现在由工具自己兜住 —— 键字面量是本批新造的，**它已经出现在 rev 里
就说明该 rev 含改动后源码**，直接判 `WRONG-REV` 并 `rc=2`，不再让人去猜一片假报警：

```
WRONG-REV：98/98 个键的字面量已存在于 HEAD~1 ⇒ HEAD~1 含**改动后**源码，上面「片段查无」不成立。
```

复现读数：[gate-r27-b1c5b-fidelity-wrongrev.txt](gate-r27-b1c5b-fidelity-wrongrev.txt)（错 rev，rc=2）
与 [gate-r27-b1c5b-fidelity-final.txt](gate-r27-b1c5b-fidelity-final.txt)（`--rev HEAD~2`，98/98，rc=0）。
教训记一句：**「搬过去」不等于「搬对了」**，凡是机械搬运，验收判据必须回到原文逐字对；
而**判据选错版本时的报错，也必须工具自己认得出来** —— 否则假阳性会冒充真缺陷再耗一轮。

### 12.2 负对照差点自己骗了自己（两次）

新尺必须先证明**有牙**再拿去放行，本批这里踩了两层：

1. 工具从临时目录提进 `tools/` 时 `ROOT` 少上了一层，落到 `docs/`，扫不到任何源码 ——
   输出 `调用点 = 0 | 不合格 = 0`、**rc=0**。这就是 §9.2 同一类事故的复发：**空跑看着像全过**。
   ⇒ 现在两道闸都在工具里：判据/源码目录缺失直接 `rc=2` 拒出报告；`checked == 0` 时报告追加
   `FALSE-GREEN` 行。
2. 第一次负对照**没验到 arity**：脚本用 `str.replace` 改坏 3 条值，其中一条目标写成了英文原文，
   而 zh 值当时另有措辞 ⇒ replace 静默 no-op，报告显示只抓到 2 条。若不去查为什么少一条，
   就会把「尺子没牙」读成「值没问题」。⇒ 破坏脚本现在**自证三条都落地**（缺一条 `rc=3`）。
   重跑后 3/3 命中、zh 备份原样还回（md5 一致）。

### 12.3 门禁读数（本批）

| 尺 | 读数 |
| --- | --- |
| `check_locale_keys.py` | **提交树 286 / 286** 键齐（zh + en）⇒ GATE OK。当时 stdout 报 288/288、引用 215，那是**工作树**快照（混入并行会话未提交的 2 条 `sysopt.combo.*`）；口径更正见 §13.5 |
| `check_placeholder_arity.py --both` | 调用点 **318**，不合格 **0**；负对照 3/3 命中 [gate-r27-b1c5b-arity.txt](gate-r27-b1c5b-arity.txt) |
| 忠实度尺 | 98/98 片段逐字命中 [gate-r27-b1c5b-fidelity.txt](gate-r27-b1c5b-fidelity.txt) |
| `scan_display_text.py` | 站点 713 ⇒ **644**（壳窗 583 / 在用 61；形状 change_text 207 / `Text` 构造 11 / SoftKey 提示 426） |
| `scan_copy_blindspots.py` | 候选 469 ⇒ **387 / 34 文件**；24 个本批文件内残留 = 0（`UIAlliance.cpp:178` 是 SoftKey 三元提示，属 §9.3 第四形状，不在本批范围） |
| `cl /c`（24 TU） | `CL_EXIT=0`，**0 error** / 43 warning，全是既有类 C4005 C4101 C4244 C4267 |
| 版式自回归 `layout.py` | 10 条 spec 全 rc=0，未因加 `#include "../../Util/Locale.h"` 影响任何几何 |

### 12.4 欠账（本批明确不闭环）

- **文案还是英文**：决策 B 通的是**机制与键位**。98 条值照抄原英文，界面看不出变化，这是预期而不是漏做；
  中文措辞的输入就是 [pending-copy.md](pending-copy.md) 的 §B1c-5b 段（98 行，「建议取证来源」统一记
  「本包自造，无原生依据」—— 逐条编一个节点名会让这张表失去可信度）。
- **SoftKey 提示 426 处**：按 §11 第 6 条，原生 v83 不启用软键盘，本批**未建键**，等用户裁决。
- **zh / en 当前同值**：两份 ini 内容一致，运行时切换看不出差别；真正差异化翻译属文案批次。

## 13. B1c-5c（中文盲区三窗接线 + 一站按裁决不接）落地（2026-11-03）

范围：`UIQuestLog`（区域名 + 详细态 7 行）/ `UIGuild`（标题 + 成员行）/ `UIMerchant`（店主名）**接线**，
`UIGender`（软键盘输入比对串）**不接线并记理由**。新建 15 键，两份 ini 同键（键表 [keys-5c.txt](keys-5c.txt)）。
门禁读数打包在 [gate-r27-b1c5c.txt](gate-r27-b1c5c.txt)。

### 13.1 中文站点尺第一次「自己解释自己」

这条尺以前只报一个总数，本批给它加了档号（`EXEMPT` 表，键 = 文件、值 = 字面量 + 档号 + 理由）：

```
总计 14 处
分档：B1c-5c=2 | B1c-6=1 | 待裁决=11
```

30 → 14，**「待接线」归零** —— 全仓 `IO/` 树下已经没有「该接而没接」的中文展示站点。余下三档各是什么，
[cjk-scan-full.txt](cjk-scan-full.txt) 逐行带档号。**为什么值得改这条尺**：以前读数是「还剩 14 处」，
下一批必须重开源码才知道哪几处该动；现在读数直接说「2 处按裁决不接、1 处是日志、11 处等人拍板」，
欠账和责任边界都在数里。

`UIGender.cpp:126/128` 的 `t == "女"` / `t == "男"` 是**软键盘输入的比对串**：把 `"女"` 换成
`Locale::text(...)` 之后，切到 en 时值变了、比对就失配 —— 用户按同一颗键会得到「什么都不发生」。
这类站点不属于「展示文案可翻译」，属于「逻辑常量」，所以按裁决留原字面量并写进尺子里，而不是留在台账里等人重新发现。

### 13.2 本批补的形状缺口：三元选键（顺带追回 5b 漏掉的 2 个键）

`check_locale_keys.py` 的两条引用正则都要求**引号紧跟左括号**，于是
`Locale::text(cond ? "a.x" : "a.y")` 整形漏在门禁外。补尺后引用键 228 → **232**：本批 2 个
（`guild.member_online/offline`）+ **5b 漏了 2 个**（`familytree.online/offline`，
[IO/UITypes/UIFamilyTree.cpp:70](../../../../../IO/UITypes/UIFamilyTree.cpp#L70)）。
5b 那两条当时恰好 ini 有档，所以没在真机上炸出键名回显 —— **是运气不是门禁放行**，这条要记在案。

负对照（改错一个字母）证明新尺有牙，且自证破坏与还原真的落地：

```
rc=1   guild.member_onlie <- IO/UITypes/UIGuild.cpp     （还原后 rc=0）
```

### 13.3 两条「搬对形」的判据（都源于读机制，不是靠看起来对）

1. **加载器 trim 掉值两端空白**（[Util/Locale.cpp:15-23](../../../../../Util/Locale.cpp#L15)）。
   `前置任务: `、` 职位{}`、` 的商店` 若按「值 = 原字面量」原样进 ini，**首格/尾格会静默消失**，
   而值保真尺（比的是片段是否在原文件出现）和 arity 尺（比的是占位符数）都**不会报**。
   ⇒ 本批口径：空格写在**占位符两侧**（`{} 的商店`、`职位{}`），或留在代码拼接（`" " + format(...)`）。
2. **中文后置词在英文里是所有格**：`owner + " 的商店"` 若拆成「名字 + 后缀键」，en 会渲染成 `Bob 's Shop`。
   ⇒ `merchant.owner_shop` 整句进键（zh `{} 的商店` / en `{}'s Shop`）。这不是偏好，是
   `Locale::format` 存在的理由 —— 语序是语言属性，拆开后就把语序钉死在 C++ 里。

### 13.4 门禁与欠账

编译 3 TU `CL_EXIT=0`；键门禁**提交树 301/301**（工具当时报 303，见 §13.5）`GATE OK`；arity `336 站点 / 0 不合格`；
值保真（提交前，判据 `HEAD`）`15/15`；版式 `specs=10 非零返回=0`。

- **英文措辞全部本包自造**：15 条 en 值登记在 [pending-copy.md](pending-copy.md) §B1c-5c，等用户定稿。
  zh 侧零风险 —— 值就是改动前界面在显示的那句。
- **`IO/Window.cpp` 11 处开发者浮层**（破功值 / 当前时间 / 在线时长）：**待用户裁决**算不算展示文案；
  该文件此刻被并行会话占着，本批不动（最小改动 + 冲突面）。
- **真机运行时负对照仍欠**：`MapleStory.exe` 占 OutDir，须用户关闭客户端后整包 `-t:Rebuild`，
  然后切语言看这三窗（任务详细态 / 公会标题与成员行 / 店主名）是否随 ini 变。

### 13.5 归档口径事故：把「工作树读数」当成「本批键数」写进了档案

复核 5c 时顺手对了一遍提交树，发现**本批归档的键数一直偏大 2**：5b 报 288（提交树 286）、
5c 报 303（提交树 301）。数字**没记错**，是**口径**记错 —— `check_locale_keys.py` 读的是工作树，
而共享工作树里躺着并行会话未提交的 `sysopt.combo.*`（5b/5c 归档时各 2 条，复核时 6 条）。

**为什么这条值得单记**：档案里的键数不是「当时屏幕上显示过什么」，而是「**这批建了多少键**」——
后者要和键表行数（98 / 15）逐条对得上，一旦混进别人的键，下批复查时会先怀疑自己的键表漏行，
白花一轮时间去查一个不存在的缺陷。这跟 §12.1「搬过去不等于搬对了」、§9.2「空跑看着像全过」
是同一类：**读数必须和它要证明的命题同口径**。

处置（三件，都已落地）：

1. 尺子自己打两行 —— 工作树 + `HEAD` 提交树，且**提交树那一行带 rev**，旁边写出可复核的 `git show` 命令
   （[tools/check_locale_keys.py](../../tools/check_locale_keys.py) 的 `committed_key_counts()`；
   拿不到 git 时那一行直接不打，不影响门禁）。
2. 逐 rev 复核并落盘 [gate-recheck-committed-keys.txt](gate-recheck-committed-keys.txt)：
   提交树 `188 (5a) → 286 (5b) → 301 (5c)`，净增 `+98 / +15`，与两份键表行数一致 —— 本批**不变量**只有这一条。
   复核读数 [gate-recheck-5c-run.txt](gate-recheck-5c-run.txt)。
3. 旧档案**不删原始 stdout**（那是历史事实），只在其后补「口径更正」段：
   [gate-r27-b1c5b.txt](gate-r27-b1c5b.txt) §2、[gate-r27-b1c5c.txt](gate-r27-b1c5c.txt) §2、
   [gate-r27-b1c5c-keys.txt](gate-r27-b1c5c-keys.txt) 头部、[pending-copy.md](pending-copy.md) §B1c-5c，
   以及 §12.3 / §13.4 两张读数表。

顺带一条同源结论：**「代码引用键 N 个」也是工作树源码读数**，会随别人的未提交引用点漂移
（5b 归档 215 → 5c 归档 232 → 2026-11-03 重跑 230；差的 2 处不属本批）。所以批间比较一律用提交树键数，
不用引用数 —— 这条已写进 [PLAN-R27-B1c.md](../../PLAN-R27-B1c.md) 的常设门禁。

**复核当场又撞上第二层：`HEAD` 自己会动。** 我写下「工作树 307 / 提交树 301」之后几分钟，
并行会话提交了 `aa0d4e2`（R28-B4），把当初那 6 条未提交的 `sysopt.combo.*` 提进了提交树 ——
同一句 `git show HEAD:locale/zh-CN.ini | grep -c …` 从 301 变 307，两份档案读数当场「互相矛盾」。
数字都没错，错在**用 `HEAD` 当坐标**：它是个会过期的数。⇒ 从此「提交树」读数必须带 rev
（尺子现在打 `HEAD(aa0d4e2) 提交树 keys = …`），复核档案的 §1 表也逐 rev 写死，链条见
[gate-recheck-committed-keys.txt](gate-recheck-committed-keys.txt) §5。
