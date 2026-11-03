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
