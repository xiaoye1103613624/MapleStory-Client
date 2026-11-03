# PLAN-R27-B1c —— 非位图文案全量接入 i18n（一次性完整计划）

> 已拍板的三条（2026-11-03）：
> **A 范围 = 全量 402 处都接机制**；**B 文案 = 无原生依据的保持英文原值、只建键**；
> **C 刷新 = reload 广播、窗自取**（不做「重开窗才生效」，也不做逐窗 `change_text`）。
> 站点台账：[evidence/r27-b1c-i18n-scan/inventory.txt](evidence/r27-b1c-i18n-scan/inventory.txt)。

## 0. 目标 / 非目标

| | |
| --- | --- |
| **目标** | ① 402 处展示文案全部经 `Locale` 取值，代码里不再出现英文字面量/中文字面量<br>② 切语言后**已开着的窗**立刻换文案（机制唯一，不动 50 个窗的 update）<br>③ 反向门禁：代码引用的键 ⇒ 两份 ini 必须都有；清点工具站点 ⇒ 必须归零<br>④「等原生取证替换」的键有台账，不会烂尾成永久英文 |
| **非目标** | ① **不翻译**（决策 B：新键 zh 值 = 英文原值）<br>② 位图烘焙文字不做 i18n（能力窗标签行、装备槽名、UserList 标题、GameOpt 底板粉钮）<br>③ 不做语言切换 UI（`ui.language` 仍靠 xy-config 改；切换 UI 留下一期，PLAN §B1c-2 原文已声明）<br>④ 不动封包编码路径（GBK 那条线跟本批无关） |

## 1. 机制层设计（先做，其他批次都依赖它）

### 1.1 为什么不用 registry，也不用 `Text::draw` 里查代次

取证过的三个事实决定了形状：

| 事实 | 后果 |
| --- | --- |
| `Text::draw()` 是 **const**（[Graphics/Text.h:106-107](../../Graphics/Text.h)） | 想在绘制时自取文案，得把 `text` 变 mutable 或 `const_cast` —— 绘制改状态，否决 |
| `Text` 有隐式拷贝与赋值（现仓真实写法：`potflag_preview = Text(Text::Font::A11M, …)`，[IO/Components/EquipTooltip.cpp:196](../../IO/Components/EquipTooltip.cpp)） | 让 `Text` 自注册进全局表，就得自己写拷贝构造/赋值/析构三件套来摘挂指针，退出时序还有静态对象析构顺序风险 —— 否决 |
| 每帧遍历活动元素的派发点只有 **3 个**：`IO/UIStateGame.cpp:151`、`IO/UIStateLogin.cpp:127`、`IO/UIStateCashShop.cpp:60`（都是 `element->update()`） | 在这里插一次刷新 = 一处机制、全站生效、零逐窗代码 ⇒ **选定** |

### 1.2 落地形态

**(a) `Util/Locale`**：加代次计数，`reload()` 末尾 `++generation`（`Locale.cpp:78-93`）。
只加一个 `uint32_t` 和一个 getter，不改现有 `tr/has/text/format/enum_*` 语义。

**(b) `IO/UIElement`**：加一张极小的绑定表

```cpp
// 静态文案绑定：窗 ctor 里 bind，语言切换时由派发点统一重取
void bind_text(Text& target, const std::string& key);
void refresh_localized();          // generation 没变 -> 立即 return；空表 -> O(1)
std::vector<std::pair<Text*, std::string>> locale_binds;
```

规则写进头文件注释：
- **只给「值在会话内恒定」的标签上表**。判据不是「这行看起来是常量」，而是
  **全文搜该对象有没有别的写入点**：有 ⇒ 它就不恒定，绑上来会在语言重载时把数据刷回初值。
- **随数据变的**（金币数额、`Default Group (0/0)`、tooltip 行）继续用
  `Locale::text(key)` / `Locale::format(key, value)` **就地现算** —— 那条路本来就是活文案，
  上表反而会把旧值钉住。这一条是本批最容易做错的地方，所以要在门禁里管（§3.3）。
- **坑的形状（B1c-2 真踩过，2026-11-03 自纠）**：`UICharSelect` 的 `ranklabel` 初值是
  「排行不可用」，看起来是静态占位 ⇒ 绑了表。但改写它的 `update_selected_character()`
  是**事件驱动**的（选中变化 / Charlist 到达），不是每帧。后果：已显示真排行时切语言，
  `refresh_localized()` 把它刷回「排行不可用」，且**没有下一次改写**，界面就停在那儿。
  ⇒ 占位初值一律用 `Locale::text()` 就地取，不进表。

**(c) 3 个派发点**：`element->update()` 前加一行 `element->refresh_localized();`。

**(d) 不做 template-method 重构**（把 `update()` 拆成 non-virtual + `do_update()`）：
那要给 ~50 个窗改名，改动面爆炸，且 `IO/UIElement.h` 正被并行会话占用（§5）。

## 2. 键名与 ini 约定

| 项 | 规定 |
| --- | --- |
| 命名 | `<窗/模块>.<语义>`，全小写点分：`userlist.group.none`、`mapinfo.no_monsters`、`charselect.version_prefix` |
| 占位 | 单占位沿用现有 `{}`（`Locale::format`，见 [Util/Locale.h](../../Util/Locale.h) 的注释：值两端空白会被加载器 trim，所以空格写在占位符两侧） |
| zh 值 | **英文原值**（决策 B）。原值是中文的（20 处）就写中文原值 |
| en 值 | 同 zh（保持两份键集严格对齐，门禁会查漂移） |
| 取证标记 | 键**上方一行** `# NO-NATIVE <窗> <文件:行>` —— 解析器只认**整行** `#`/`;` 注释（`Util/Locale.cpp:62-64`），**行尾追加注释会被算进值里**，这条必须照做否则值带脏尾巴 |
| 待办台账 | `evidence/r27-b1c-i18n-scan/pending-copy.md`：每个 `NO-NATIVE` 键一行，列「键 / 现值 / 建议取证来源（String.img 节点或 IDA 函数）/ 状态」 |

## 3. 批次分解（每批一个原子提交，批内自证后才进下一批）

| 批 | 内容 | 站点 | 自证判据 |
| --- | --- | --- | --- |
| **B1c-2 机制层** | `Locale::generation` + `UIElement::bind_text/refresh_localized` + 3 个派发点 + `EquipTooltip`/`UIMonsterBook` 的静态标签上表 | 6 | `cl /c /utf-8` 过；**负对照**：临时把 `ui.language` 从 zh-CN 改 en-US，已开着的窗的绑定文案必须变，没绑的 `Locale::text` 活文案也必须变（这一步同时验 §1.2b 的两条路都通） |
| **B1c-3 在用窗** | `UIUserList`(5) `UIMapInfoPanel`(4) `UIMapMobPanel`(1) `UIMapDropPanel`(3) `UIViewAllChar`(2) `UIRegion`(2) `Ver. `×4 `UIJoypad`(2) `UICharSelect`(2) | 25 | 门禁 §4 + 版式回归（换文案后宽度会变，必须重跑 `layout.py check` 十张 spec） |
| **B1c-4 中文硬编码** | `UIBuddyList`(3) `UIParty`(3) `UIStorage`(3) `UITrade`(5) `UIMapSearchPanel`(5) `UIMapInfoPanel`(2) `UIDailyGift`(14) | ~~20~~ **35**（清点器漏的 15 处见「落地情况」） | 同上；这批零翻译风险，最先能全绿 |
| **B1c-5 壳窗·小** | 壳窗里除 `UIMiscShells` 外的 ~117 处，按文件切 2~3 个提交：**5a** = `UIVegaSpell`/`UIRpsGame`/`UIPlant`+`UIRaise`/`UIViciousHammer`/`UIWedding`（~~已完，见「落地情况」~~）；**5b** = 其余壳窗 latin；**5c** = 中文盲区 `UIQuestLog`/`UIGuild`/`UIGender`/`UIMerchant` | ~~117~~ **80**（5a 实测，含 16 处扫描器盲区）+ 5b/5c 待清 | 同上 |
| **B1c-6 壳窗·`UIMiscShells`** | 216 处，单文件独占一批（体量最大，且与并行会话冲突最高） | 216 | 同上 |
| **B1c-7 收尾** | 站点归零、`pending-copy.md` 成册、CHECKLIST + PLAN §B1c + evidence README 定版、调试日志清理、全量 MSBuild + 抽窗真机复验 | — | 见 §6 Definition of Done |

依赖：B1c-2 → 其余全部；B1c-3/4/5/6 之间互不依赖，可并行推进；B1c-7 等前五批。

**B1c-2 落地情况（2026-11-03）**：机制层已按 §2 实现（`Locale::generation` 代次、
`UIElement::bind_text` 四重载含 `OutlinedText` 一档、`refresh_localized`、3 个 `UIState` 派发点各一行）。
上表的「站点」一栏**写错了对象**，这里改正：本批实际进绑定表的是 `Ver. ` 前缀 5 处
（`UILogin.cpp:76`、`UICharSelect.cpp:136`、`UIRaceSelect.cpp:156`、`UICommonCreation.cpp:78 / :360`，
已核这 5 处的 `version` 对象除 ctor 外无其它写入点）。角色选择「排行」行 3 处
（`UICharSelect.cpp:384` 初值 + `:1297/:1299/:1305` 改写）**不进表**，走 `Locale::text` 就地取
—— 它被事件驱动的 `update_selected_character()` 改写，绑上会在切语言时把真排行刷回初值且无人写回
（见 §1.2b「坑的形状」；这一处是 B1c-2 提交后自纠的，不是原样）。
**不是** `EquipTooltip`/`UIMonsterBook` —— 那两处上一批（B1c-1 / B3b）已经在就地调 `Locale::text`，
是活文案，按 §1.2b 的规则**不该**进绑定表（进了反而把登记那一刻的值钉死）。
门禁：`cl /c /utf-8` 9 TU 全过；`check_locale_keys.py` GATE OK（97/97）；
`layout.py check` 十张 spec 对自身 oracle 帧全 rc=0 / Δ=0。
**唯一欠账 = 运行时负对照**（要能覆盖 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` 的窗口 + 整工程 Rebuild，
`UIElement` 实例尺寸变了不能增量），判据改用 `Locale refresh gen=` 日志，见 evidence README §6。

**B1c-3 落地情况（2026-11-03）**：在用窗 19 处接完（`Ver. `×4 与 `UICharSelect` 的排行行已归 B1c-2，
本批不重复计），新增 12 键，清点器 402 ⇒ 377。
**机制层在本批改了形**，因为 B1c-2 的假设「要接的窗都是 UIElement」被世界地图那三块侧栏证伪
（`UIMapMobPanel` / `UIMapDropPanel` / `UIMapInfoPanel` 头文件原话「Not a UIElement — owned by
UIWorldMap」，`cl` 直接报 `bind_text 找不到标识符`）。绑定表因此从 `UIElement` 私有段抽成
[IO/LocaleBindings.h](../../IO/LocaleBindings.h)：`UIElement` 持有一份并转发（对外 API
`bind_text` / `refresh_localized` 一字未改），非 UIElement 的宿主也持有一份、在自己的 `update()`
里 `refresh()`。**这条改形对后面三批是硬约定**：B1c-4/5/6 动手前要先分宿主档位 —— UIElement 走
UIState 派发点，非 UIElement（吸附面板、绘制件）必须自带一份表并在其 update 里刷，
否则「切语言立刻换文案」只对一半的窗成立。
门禁：`cl /c /utf-8 /bigobj` 9 TU 全过（含公共件与宿主 `UIWorldMap.cpp`）；
`check_locale_keys.py` GATE OK（109/109）；`layout.py check` 十张 spec 对自身 oracle 帧仍全 rc=0 / Δ=0。
读数见 [gate-r27-b1c3.txt](evidence/r27-b1c-i18n-scan/gate-r27-b1c3.txt)，取舍理由见 evidence README §7。
**运行时负对照依旧欠着，且欠账变了一次**：`UIElement` 私有段现在是「vector + 代次 + 宿主名」，
实例尺寸与 B1c-2 又不同 ⇒ 整工程 Rebuild 仍不可省（见 [[msbuild-header-layout-stale-objs]]）。

**B1c-4 落地情况（2026-11-03）**：中文硬编码批次接完，**实际 35 处中文字面量，不是上表原先写的 20 处**
（原表的 `UICharSelect`(2) 属 B1c-2 的排行行，本批不重复计；真补回来的是下面这批）。差额不是漏搜，是
`scan_display_text.py` 的**形状盲区**：它按行匹配 `change_text("..")` / `Text(Text::Font::.. , "..")` /
`show_text(..)` 三种形状，看不见 ① 实参折到下一行（`footer_rules[i] = Text(...,\n    "· 击杀等级范围内怪物…")`）、
② 三元的另一支（`mob.boss ? "BOSS" : "普通"`）、③ 拼接片段（`s += " 频道" + std::to_string(n)`）。
为此加了第二把尺 [tools/scan_cjk_literals.py](tools/scan_cjk_literals.py)
（不问形状，只问「代码行里有没有带汉字的字符串字面量」，注释剔除），产物
[cjk-scan-full.txt](evidence/r27-b1c-i18n-scan/cjk-scan-full.txt)：开工前 65 ⇒ 本批 35 ⇒ 现余 30。
**所以 B1c-5/6 开工以这把尺为准**，`scan_display_text.py` 的 `sites=` 继续只当英文（latin）口径用；
剩余 30 处分布在 `IO/Window.cpp` 11（标题栏拼接与 LOG，B1c-7 单独定档）、`UIQuestLog` 10、`UIGuild` 5、
`UIGender` 2、`UIMerchant` 1、`UIMiscShells` 1 —— 除最后一处外全落在 B1c-5 的壳窗范围里。

本批按 §3 尾的硬约定分了三档（**没有新增第二套机制**）：UIElement 档 = `UIDailyGift` 14 处经
`UIDragElement<PosDAILYGIFT>` 继承，走 ctor `bind_text` ⇒ UIState 三个派发点；非 UIElement 档 =
`UIMapSearchPanel` 3 处自带 `LocaleBindings{ "UIMapSearchPanel" }`，刷新点放在其 `update()` 的
`if (!visible) return;` **之前**（隐藏期间换语言也要跟上，否则重开的那一帧先闪旧语言）；
数据驱动档 = 其余 18 处 `Locale::text/format` 现取。新判到的两种形状写进 evidence README §8.2：
行对象（`mob_rows` 每次 `clear()` + `push_back(std::move(row))`）禁上表 —— 绑定表存 `Text` 裸指针会悬垂；
`draw()` 内局部 `Text` 只能现取。另有一条**如实记为延迟、不当闭环**：`UIStorage::mesolabel`
三个写入点全带数据，只能走活文案 ⇒ 切语言到下一次仓库包到达之前仍是旧语言。

门禁：`cl /c /utf-8 /bigobj` 8 TU 全过（含宿主 `UIWorldMap.cpp`）；`check_locale_keys.py` GATE OK
（141/141，本批净 +32 键）；`layout.py check` 十张 spec 对自身 oracle 帧仍全 rc=0 / Δ=0；
`scan_display_text.py` 402 ⇒ 377 ⇒ 359；`scan_cjk_literals.py` 35 ⇒ 0（本批范围内）。
读数见 [gate-r27-b1c4.txt](evidence/r27-b1c-i18n-scan/gate-r27-b1c4.txt)，取舍理由见 evidence README §8。
**运行时负对照仍欠**：`MapleStory.exe` 占着 `OutDir` ⇒ 整工程 Rebuild 与「改 `xy-config.yaml` 的
`ui.language` 看 `Locale refresh gen=`」都做不了，按用户既有选择等他自行关闭客户端。

**B1c-5a 落地情况（2026-11-03）**：壳窗第一批 5 文件接完 —— `UIVegaSpell`(18) `UIRpsGame`(15)
`UIPlant`+`UIRaise`(19) `UIViciousHammer`(10) `UIWedding`(18)，**80 个调用点 / 49 新键**，
`scan_display_text.py` 359 ⇒ **304**（这 5 文件已无余站），`scan_cjk_literals.py` 仍 30（本批无中文站）。
本批把三件事摊开了，都算机制级信息，后两批按此走：

1. **`Locale` 补了多占位入口 `format_all(key, {v1, v2, …})`**（按 `{}` 出现顺序填）。触发点是
   `rps.round_result`=`You:{} NPC:{}` 与 `wedding.progress`=`Step {}  G:{} B:{}`。为什么不拆键在 C++ 里拼：
   语序是语言属性，拆完就把语序钉死在代码里、ini 失去真源地位；只此一份实现。
2. **反向门禁（§4.2）之前形同虚设**：`check_locale_keys.py` 首版正则要求键后紧跟 `+`/`,`，全站最常见的
   单参 `Locale::text("k")` 一条都不匹配 —— 只报 51 键、缺键清单只剩 `format` 族。补 `\s*\)` 档后
   121 键 / 缺 0。**新门禁必须先用已知缺陷做负对照**，「跑通了」不等于「在拦」。
3. **站点计数确认是下界**：`UISoftKey(Ok, Cancel, "<提示语>")` 第三实参是展示文案，`scan_display_text.py`
   的三种形状全看不见；本批人工补了 16 处（清单见 [gate-r27-b1c5a.txt](evidence/r27-b1c-i18n-scan/gate-r27-b1c5a.txt) §5），
   全站余量未清点。该形状混着两类东西 —— 真提示语（`UILogin.cpp:586/:592` `"Account"/"Password"`、
   `UICharSelect.cpp:1452/:1455` PIC 整句）与被代码按 token 比较的**输入语法**（`rock|paper|scissor`、
   `UIGender.cpp:135` `"m|f"`、`UIKeyConfig.cpp:916` `"save|close"`、`UIQuit.cpp:182` `"y|n"`）。
   本批处置：两类都建键，输入语法在 ini 标 `# 禁译`。**是否把该形状扩进扫描器（会重算全站总数）、
   或改为白名单不建键 ⇒ 待用户抉择，不擅自扩量**（见 evidence README §9.3）。

档位照 §3 尾硬约定：这 5 窗的 `status`/`tip_label` 每处都有第二个写入点 ⇒ **一个绑定点都没加**，
全部 ctor 现取 + 事件现取；代价（切语言后已开着的窗要等下一次数据事件）已写进 README §9.4。
`UIWedding` 的 `"Wedding invite"`（5 处悄悄话默认正文）**刻意不建键** —— 出站封包正文给对端看，
翻译会改变对方收到的内容，属协议层，需用户单独定策。
门禁：6 TU `cl /c /utf-8 /bigobj` CL_EXIT=0 / error 0 行；`check_locale_keys.py` **190/190** + 反向门禁缺 0；
`layout.py check` 十张 spec 61 项断言不合格 0。余下 B1c-5b（其余壳窗 latin）/ 5c（`UIQuestLog` 10、
`UIGuild` 5、`UIGender` 2、`UIMerchant` 1 的中文盲区）。

## 4. 门禁（本批的验收尺，不是「看起来改完了」）

1. **站点归零**：`tools/scan_display_text.py` 读数英文 0 / 中文 0，
   白名单只有「纯数字与符号初值」（`0` `1` `-` ` ~ ` `0%` `0.00%` `!` `V`）——
   白名单要写进工具源码并在文档里逐条给理由，**不允许**用白名单偷偷放行真文案。
2. **反向键门禁**（扩 `tools/check_locale_keys.py`）：扫全仓 `Locale::text/text_or/format/enum_text`
   与 `bind_text` 的键字面量，要求在 **zh-CN 和 en-US 里都存在**；两份键集必须一致；值不得为空。
   现有正向门禁（数据表枚举 ⇒ 键必须建档）保留不动。
3. **绑定误用检查**：`bind_text` 的第二个参数若是拼接出来的非常量（`bind_text(t, key + val)`）⇒ 报警，
   因为上表的键必须是字面量；随数据变的内容应走 `Locale::format` 就地算。
4. **版式回归**：每批改完跑 `layout.py check` 十张 spec（69 探针）必须 rc=0。
   文案换语言会改字宽/行宽，这一条是防「i18n 把 B1b 的墨顶常数顶歪」。
5. **编码**：ini 一律 UTF-8（加载器已有 BOM 剥离，`Util/Locale.cpp:26-33`）；
   渲染走已修的矢量路径；**禁止**把中文塞进 GBK 封包路径。
6. 每批 `cl /c /utf-8 /bigobj` 范围编译受影响 TU；B1c-7 做一次整包 MSBuild。

## 5. 风险与阻塞（先说清，不许假装能闭环）

| 风险 | 处置 |
| --- | --- |
| **并行会话正在改本批要动的文件**：`IO/UIElement.h`、`IO/UIStateGame.cpp`、`UIMiscShells.cpp`、`UIMapInfoPanel.cpp`、`Configuration.cpp/.h`、`locale/*.ini`、`CHECKLIST.md` 目前全是脏的 | ① 机制层（B1c-2）动的是**公共件**，必须先确认并行会话在 `UIElement.h` 上改的是什么、有没有收敛；未收敛则**这一批暂停**，先把 B1c-3/4 这些没争议的文件推掉。<br>② `locale/*.ini` 两边都要追加键 ⇒ 用「追加到文件末尾 + 只 stage 我这段」的 blob 级精确暂存（本仓已有先例：CHECKLIST 的 D7b 行）。<br>③ **禁止 `git add -A`**，禁止 `git stash`。 |
| 换文案导致行宽/高度变，把已定版的版式顶歪（尤其 tooltip 高度按行数算） | §4.4 十张 spec 回归 + 关键窗出图对比；`equip.pot.*` 这类中英等长值风险低，`Ver. `→中文风险高 |
| 402 处里混着「其实不该 i18n」的（数字初值、单位符号） | 白名单显式化（§4.1），不留隐性豁免 |
| 决策 B 之后界面仍是英文，用户可能误以为「i18n 做完了 = 看到中文」 | 交付说明里明写：**本批通的是机制与键位，文案替换是另一批活**，`pending-copy.md` 是那一批的输入 |
| 真机复验要登录（`R27_USER`/`R27_PASS`） | 标记为**只能用户给凭证/亲自动手**的项；不给凭证则 B1c-2 的负对照改用「不登录也能开的窗」（登录窗/角色选择窗本身在 `UIStateLogin` 派发路径上，够用） |

## 6. Definition of Done

- [ ] 机制层三件（generation / bind_text / 3 派发点）落地且有负对照证据
- [ ] 402 站点：真文案 0 残留，白名单逐条有理由
- [ ] 反向键门禁接入并跑绿；两份 ini 键集对齐、无空值
- [ ] `pending-copy.md` 成册（每个 NO-NATIVE 键一行）
- [ ] 十张 spec 69 探针回归 rc=0；范围编译 + 一次整包 MSBuild
- [ ] 切语言真机自证：已开着的窗立刻换文案（或按 §5 末行降级到登录窗自证）
- [ ] 未擅自改并行会话的文件；未 push
- [ ] 原子提交（每批一提交）+ CHECKLIST/PLAN/evidence 三处文档定版
- [ ] 临时调试日志已删，上线级节点日志与注释齐全
