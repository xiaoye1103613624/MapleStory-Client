# 隐藏问题清单（开发期遇到、但不在 W0–W13 交付面上的东西）

口径：本文件只收「**做 W 批次过程中撞见、当时没顺手改或改不动**」的问题，按**谁能收尾**分三类。
逐窗验收判据在 [../CHECKLIST.md](../CHECKLIST.md)，本文件不重复；只在需要引用时给 §号。

分类：
- **A 已自行修掉**（本轮代码已进 `build_check`，并已部署到客户端目录）
- **B 我能自证、但需要线上客户端配合**（客户端目前没在跑 → 需要你把号登进来，或授权我自己起）
- **C 需外部证据**（IDA / 缺素材 / 服务端），客户端侧无法自证
- **D 已核实为「不是问题」**（记下以免下次重新怀疑）

---

## A. 已自行修掉

### A1 UI 层把「帧插值系数」当不透明度用（本清单里影响面最大的一条）

主循环 `MapleStory.cpp` 每帧算 `float alpha = accumulator / timestep`（本帧还差多少时间才走到下一
逻辑帧，取值 [0,1)，逐帧锯齿抖动），然后原样传进 `UI::get().draw(alpha)`。UI 侧把它当**不透明度**
转发：

| 转发点 | 数量 |
| --- | --- |
| 显式 `sprite.draw(pos, inter)` 等写法 | **59 处 / 14 个文件** |
| 基类 `UIElement::draw_sprites()`（所有未重写它的窗体） | 1 处，但覆盖面是「全部用 `sprites` 集合的窗体」 |

受影响文件（`grep -c ", inter)"` 实测）：`UICommonCreation` 12、`UIMiniMap` 9、`UICharInfo` 8、
`UICashShop` 8、`UIRaceSelect` 7、`UICharSelect` 6、`UIMaker` 2，`UIStatusMessenger`/`UIQuit`/
`UIMonsterBook`/`UIMerchant`/`UILogo`/`UIChannel`/`UIAlliance` 各 1。

**证据（不是推测）**：抓帧 `_now2.png` 里小地图收缩条区域 (104,8)–(142,22) 的颜色直方图是
`(103,71,8)×210`、`(119,111,103)×130`、`(0,0,0)×88`；而 `UIWindow.img/MiniMap/BtMap/normal/0`
（36×12）自身的颜色计数是 `(221,153,17)×210`、`(255,238,221)×130`、`(0,0,0)×88`。
**像素个数逐个对上，每个非零通道都被同乘 0.466**。`Min/c` 的 (221,221,221) 同样被画成 (103,103,103)。

> ⚠ **这段证据当时不足以定因，本条的「小地图发暗」归因已被 A4 推翻。**
> 0.466 这个数字同时吻合两个互不相干的机制：`accumulator/timestep` 的某一帧取值，
> 以及公告条 `ColorBox(…, 0.535f)` 黑底的 `1 − 0.535 = 0.465`。
> 直方图对得上只能证明「有一层 ×0.465 的压暗」，不能证明压暗来自不透明度参数。
> 判据应是**逐帧是否抖动**与**是否与 y 区间相关**，见 A4。

修法（根因单点，不动 59 处调用）：
- `MapleStory.cpp:212` → `UI::get().draw(1.0f);`（`Stage::get().draw(alpha)` 保留，那边是位置插值，用法正确）
- `Net/Handlers/LoginHandlers.cpp:133` → 选角资源预热泵帧循环里原本是 `UI::get().draw(0.0f)`，
  按上述语义等于**每泵帧把 Wait 弹窗画成全透明**，「不冻屏」的收益被这一行抵消掉了。改成 `1.0f`。

**顺带的连带影响（重要）**：`UIElement::draw_buttons()` 忽略形参、`MapleFrame::draw(DrawArgument)`
默认不透明度 1.0 —— 所以 W1–W4/W6–W10 那批底栏与角色窗（走 `MapleFrame`）**没被拖暗**，
这解释了为什么你报的是版式问题而不是「全屏闪烁」。但 A1 覆盖的 14 个窗体（含小地图、频道、
联盟、图鉴、商人、制造、退出、选角/建角全流程）此前**所有抓帧的颜色判据都可能偏低**，
其中我依据抓帧做过的颜色/可见性结论需要按 B1 重看一遍。
**偏低幅度不再当作已知量**：小地图那一处已改由 A4 解释，其余窗体需逐窗重测，不能套用 ×0.465。

### A2 「目录」= UIGameMenu 从占位壳改成真窗体

原状：只有占位文本 + `send_key` 调试壳，5 颗钮一个都没建 → 你报的「点了没反应」。
现状（`IO/UITypes/UIMiscShells.cpp`）：
- 底板 `UIWindow.img/GameMenu/backgrnd`（93×140），5 颗 81×25 真按钮压在 x=6、y=23+i×20；
  末颗底边 23+4×20+25=128 收进正文区（标题烤在 row 2..21、暗色正文 row 22..127、下摆 row 132..139）。
- 频道变更→`UIChannel`；游戏设置/系统设置→`UIOptionMenu`（见 C2 的素材边界）；
  游戏结束→`UIQuit(stats)`；**场景变更→聊天栏黄字「场景变更暂未支持。」**（本客户端无任何皮肤
  UI/协议线，按门禁「不发明客户端能力」显式预留）。
- 英文调试提示只在**缺素材回退壳**上可见；`send_key` 的 1-9/0/A-Z 调试开窗入口全部保留，没删能力。

### A3 小地图顶带文字（对 W11 §A.6 结论的更正）

`evidence/w11-map/coords.md` §A.6 当时判定「NORMAL 顶带放地图名」，实测是错的：
- NORMAL 的 29px 顶带里「小地图」是 `MinMap/nw`（60×29）**烤死的艺术字**，再叠 `Text` 就是白字压白底、
  两行 A12B 只有 11px 间距直接糊 → NORMAL 分支不再画地图名。
- MAX 的 72px 顶带才有文字，且**第一行是街道名、第二行才是地图名**（参考端 `进入游戏页.png`
  彩虹岛 / 小蘑菇），原实现两行顺序反了 → 已换。
- `combined_text`（MIN 收缩条，浅灰底 (221,221,221)）改黑字。
- **但 MAX 两行不是黑字，是白字 + 1px 黑描边**：参考帧逐行量化（`evidence/refcrops/minimap.png`）
  得 MAX 顶带底色 = **(153,187,204)**，两行字 `min(rgb)>240` 的像素集中在 y=31..42 与 y=47..58
  （行距 16px，与代码里的 20/36 一致），字外圈是暗色描边。
  ⇒ `region_text` / `town_text` 由 `Text` 改 **`OutlinedText(A12B, LEFT, WHITE, BLACK)`**
  （`IO/UITypes/UIMiniMap.h:117-122`、`IO/UITypes/UIMiniMap.cpp:88-90`）。
  我最初把三处一律改 BLACK 属**过度修正** —— 浅蓝底上黑字可读，但不是原生观感。

§A.6 原文待按 B4 一并订正。

### A4 小地图收缩条发暗的真因：公告条压在全屏顶部（推翻 A1 的归因）

A1 修完（`UI::get().draw(1.0f)`）重新构建、部署、真机进图后复测，**收缩条仍然是 (103,103,103)**，
且连取 5 帧**完全不抖** —— 与 A1 的「逐帧锯齿」预期相反，归因作废。改判过程：

| 排除项 | 证据 |
| --- | --- |
| 素材自身带 alpha | `nx_inspect`：`MiniMap/Min/c` = 1×20，**全像素 alpha=255**，颜色 (221,221,221)×16 + (136,136,136)×2 + (255,255,255)×1 |
| `Animation` 的 `a0/a1` 帧淡入 | `Graphics/Animation.cpp:28-53` `read_frame_timing()`：无 `a0`/`a1` 时 `opacities = {255,255}`；`Min/c` 无这两键 |
| `Linear` 插值取错端 | `Template/Interpolated.h:105-127` `Linear::get(alpha) = lerp(before, now, alpha)`，`alpha=1.0` 即当前真值，`UI::draw(1.0f)` 传参正确 |
| 按钮代码 | `MapleButton`/`draw_buttons()` 丢弃形参、`DrawArgument(Point)` 固定不透明度 1.0（`Graphics/DrawArgument.h:28-49`） |
| 抓帧链路 | 同帧底部聊天/状态栏像素正常，且**只有 y≤22 的这一条**暗 |

**决定性一步**：压暗系数 `103/221 = 0.466`，而 `IO/Components/ScrollingNotice.cpp:25` 正是
`ColorBox(width, 23, Color::Name::BLACK, 0.535f)` → `1 − 0.535 = 0.465`；`ScrollingNotice::update()`
把 `width` 抬到 `get_viewwidth()`（整屏宽），`draw()` 固定在 `Point(0, 0)`，且 `UI::draw` 里它画在
`state->draw()` **之后** → 一条全宽、23px 高、压在一切 UI 之上的半透明黑带。
抓帧是**窗口相对**的（客户区 (0,0) = 图像 (8,31)），换算后收缩条正落在客户区 y=5..24，
其中 y=5..22 被黑带盖住、y=23..24 露出真色 —— 实测逐行颜色正好在客户区 y=23 处从 (103,103,103)
跳回 (221,221,221)。**边界与黑带下沿对齐到 1px。**

参考端 `进入游戏页.png` 量得小地图九切片顶边在 **y=24**（y≤23 为天空），即原生布局本来就把
顶部 0..22 让给公告条。所以这是**默认坐标错**，不是渲染错。

修法（最小面）：`Configuration.h:688` `PosMINIMAP` 默认值 `"(5, 5)"` → `"(5, 24)"`。
~~**未动 `ScrollingNotice`**：公告条压 UI 是原生行为，改它会牵连全部窗体。~~
← 这句已被 A6 推翻：公告条**不该常驻**，常驻才是缺陷。

> 部署侧注意：`ui.position.minimap` 是**持久化项**，`UIDragElement` 的 `position` 取
> `Setting<T>::load()`，只改代码默认值对已有 `Settings` 无效，必须同步改存档（见 B7）。

---

### A5 【本轮最大的一条】`Settings` 里所有三级键从未生效：解析器把层级拼反了

A4 改完默认值 `(5, 24)` 部署后，真机仍然画在 y=5，且客户端把 `Settings` 又写回 `(5, 5)`。
为终结「到底是默认值没生效还是渲染没听配置」，插了一处临时诊断（`[DBG-UICFG]`，验完已删）
同时打印 `rawsettings` / `xysettings` / `Entry::value` / `load()` / `UIMiniMap::position`，一次启动即定案：

```text
load: raw=(absent) xy=(absent) entry=(5, 24) parsed=(5,24) raw_keys=70 inv_entry=(24,24)
UIMiniMap ctor: position=(5,24) setting_load=(5,24) type=2
```

`raw=(absent)` —— 文件里明明写着 `minimap: (5, 60)`（为本次实验特意改的醒目值），解析结果里却**没有这个键**。
定位到 `Configuration::parse_config_file()`：缩进栈 `stack` 存的是「由外向内」的父键，
而拼层级键时写成了

```cpp
std::string composite = key;
for (const auto& p : stack)          // 正向遍历却做「前插」
    composite = p.second + "." + composite;
```

正向遍历 + 前插 = 顺序倒过来。两级键只有一个父，恰好看不出来（`server.ip` 正常），
**三级及以上全部拼反**：`ui.position.minimap` → `position.ui.minimap`。
把同一套算法按两种顺序在 Python 里重放真实 `Settings`，差异被精确复现：

| 拼接顺序 | 解析出的键数 | `ui.position.minimap` | `position.ui.minimap` |
| --- | --- | --- | --- |
| 正确（自内向外前插 / 等价地正向 join） | 63 | **在** | 不在 |
| 代码原样（正向遍历 + 前插） | 63 | 不在 | **在** |

**影响面（按 `Configuration.h` 里名字含 ≥2 个点的 Entry 统计）= 39 项，全部一直是死的**：
31 个 `ui.position.*` 窗口坐标、`audio.bgm.volume`、`audio.sfx.volume`、`font.path.normal`、
`font.path.bold`、`ui.autopot.hp_item`、`ui.autopot.mp_item`、`ui.maplepoints.local`、
`boot.logo.frame_ms`。也就是说：**这些项在 `Settings` 里改什么都没用，永远退回代码默认值**。

这条同时解释了 B7 记录的两个怪现象：
1. 「我改了 `Settings`，客户端退出时又写回旧值」——不是写回旧值，是**读的时候就没读进去**，
   退出时把内存里的代码默认值整档写回，把外部编辑覆盖掉了；
2. 「拖动过的窗口位置会被重置」——拖动值确实存进了文件，但下次启动读不出来，于是回到默认。

修法（`Configuration.cpp:415-426`）：改成显式正向 `join`（先父后子再本键），语义一眼可读。

**回归核查（改完必须过的一关）**：解析一旦生效，历史上那些「从没被应用过」的文件值会突然开始生效，
可能把窗口挪到屏外、把音量清零。逐键比对了线上 `E:\MXD\BeiDou-Client-xy\Settings`：
39 项里 **32 项文件值 == 代码默认值**（无变化）、**7 项文件里根本没有该键**（继续吃默认）
⇒ 线上零位移、零副作用。
但**仓库根目录 `Settings` 是脏的**：`ui.position.map=(100,35)`、`ui.position.minimap=(0,0)`
是早年拖动留下的死数据，修复后若有人从仓库目录启动就会被它们接管 —— 已在 CHECKLIST 记为待清理项。

**自证闭环（真机）**：换回带诊断的构建、把文件设成 `(5, 60)` 启动，诊断行变成
`load: raw=(5, 60) ... parsed=(5,60)` + `UIMiniMap ctor: position=(5,60)`，
抓帧量顶部：收缩条**真的画到客户区 y=60**（y=60 起 136/221/255）。
文件 → 解析 → Entry → `load()` → 控件 `position` → 渲染，五段一次打通。

### A6 公告条常驻：一条公告滚完后 23px 全宽黑带永远留在屏幕上

A4 认定「公告条压 UI 是原生行为」，本轮否掉。`IO/Components/ScrollingNotice.cpp` 原文：

```cpp
if (xpos.last() < xmin)
    xpos.set(static_cast<double>(width));   // 滚出左边界就绕回右边界
```

`active` 只在 `setnotice()` 里被置 `true`，**没有任何路径把它置回 `false`** ⇒ 登录时一条
「Welcome to Scania!」让 `ColorBox(viewwidth, 23, BLACK, 0.535f)` 从此每帧都画，
屏幕顶部 0..22 永久变成半透明黑带，把落进该区间的一切 UI 一起压暗。
参考端 `进入游戏页.png` 同一位置是天空本色（x=28 处 y≤23 为 (71,87,103)，y=24 起才是小地图白边
(255,255,255)），**没有这条黑带** ⇒ 原生公告滚完即消失。

修法：文字完整滚出左边界后 `active = false`（不再绕回），黑带随公告一起消失。
触发链只有一条：`Net/Handlers/MessagingHandlers.cpp:438` → `UI::set_scrollnotice()`。

修完真机复测（`6AE5FE0D / 67145c64a74c67b3`，文件 `minimap: (5, 24)`）：

```text
client_y=23 mode=(0, 0, 0) x50      ← 顶部不再是全宽 137px 黑带
client_y=24 mode=(136,136,136) x137 ← 收缩条顶边，真色
client_y=25 mode=(221,221,221) x137
client_y=26 mode=(255,255,255) x136
```

顶边 136/221/255 与素材 `MiniMap/Min/c` 的实测色一一对上（A4 表格里那份 alpha=255 的取样），
不再出现 ×0.465 的 (103,103,103)。
**仍待补拍**：公告**正在滚动时**黑带应可见（下一次启动在进图后 ~45s 内抓帧，滚动周期
`(1280+文字宽)/0.5 ≈ 2860` 帧 ≈ 48s），确认「有公告时有带、滚完就没」。


---

### A7 classic HUD 下 `buttons[id]` 解引用空指针 → 客户端「自己退出」（**已修 + 已线上复验**）
**根因**：`IO/UIElement.h:214` 的 `buttons` 是 `std::map<uint16_t, std::unique_ptr<Button>>`，
`UIStatusBar` 里多处用 `buttons[id]->set_active(...)` 只读访问。`operator[]` 对不存在的 key 会
**插入一个空 unique_ptr 再解引用**。而 classic HUD 模式下
（`classic_mode = UiAssetResolve::is_classic_hud()`，构造函数 `if (classic_mode) { init_classic(); return; }`）
只创建 `BT_CLASSIC_*` 那一组，modern 的 27 个 submenu 按钮**根本不存在** ⇒ 按 `[`（MENU）/`]`（QUICKSLOTS）
进 `toggle_menu()` 就是空指针解引用。

**这条同时解释了 B 段开头挂了很久的那个悬案**：「上一轮结尾那次自己退出仍无 `.dmp`、无崩溃日志」——
那不是自己退出，是这个空指针；Release 下没有崩溃日志、没有 dmp，现象就是进程消失。
复现路径也吻合：当时正是先点了商城（HUD 换态）再按 `[`。

**修法**（`IO/UITypes/UIStatusBar.h` + `.cpp:1612-1740`）：加两个只读安全件
```cpp
Button* button_at(uint16_t id) const;                                  // find() 而非 operator[]
void set_buttons_active(std::initializer_list<uint16_t> ids, bool);    // 内部逐个判空
```
`toggle_menu/setting/community/character/event` 各自收敛成一次 `set_buttons_active({...}, flag)`，
状态切换改成 `if (Button* b = button_at(X)) b->set_state(...)`，`remove_active_menu` 的
`for i = BT_MENU_QUEST..BT_EVENT_DAILY` 循环同样加守卫。

**线上复验（构建 `6AE608CB` / md5 `4231856851ccfa29`）**：`` ` ``（商城）→ `[`（界面）→ 两次都
`alive=True`，之后整轮键位扫描（g/f/o/5/l/;/]/[/\ + e/i/k/escape×多次）客户端全程存活。
**这条现在是关闭状态。**

### A8 任务窗「点了没反应」的三条症状是同一个笔误（R23-C，本轮）

`UIQuestLog` 的命中区写成了 `left_width + right_width - bg_right_pos.x()` = **307**，
而展开态真实绘制宽度是 `bg_right_pos.x() + right_width` = **548**（`backgrnd` 245 + `backgrnd2` 305，
见 [../evidence/r24-ui-geometry/](../evidence/r24-ui-geometry/)）。
`UIElement::is_in_range` 用 `dimension` 判命中 ⇒ x307 以右那 241px 的点击**根本传不到这个窗口**，
而那块区域正好盖着底带的「详细」「寻找NPC位置」两颗钮和右板滚动条。

一条改动同时解释用户报的三条：「详情没反应」「右侧任务详情不能收缩」「右板滚动条没用」。
构造期和 `set_collapsed()` 两处都收进 `update_dimension()`，不再各写一份。

同轮另修：正文按 `detail_area.width()` 折行 + 上下缘裁剪（原来只按 `\n` 分段，长句直接铺出窗口）、
行底/分组底宽度收进 `list_area.right()=226`（原来 204@27 / 224@9 都从滚动条底下透出来）、
三个分类页统一按区域分组且**分组行可点展开/收起**（原来只有"可开始"分组，且 `select_row` 吞掉标题行点击）、
蓝横幅右格补上任务 NPC 立绘。

### A9 「NPC 跑到路下面 / NPC 不见了」是一件事，且波及全服（R23-B，本轮）

服务端 `PacketCreator.spawnNPC` 往 y 槽写的是 **`getCy()`**（`spawnMonsterInternal` 写的是真 y）。
实测每条 `life` 的 `cy − y` 都是 **+5 ~ +17**（[../evidence/r23-map-motion/_r23_life.txt](../evidence/r23-map-motion/_r23_life.txt)），
即平台线在 `cy` **上方** 5~17px。而客户端 `NpcSpawn::instantiate` 又拿 `get_y_below` 反查一遍，
`FootholdTree::get_fhid_below` 要求 `ycomp >= fy` ⇒ 自家那块平台被判成"不够低"，
选中**下一层**平台（"站到路下面去了"）；下面没有平台时返回 `borders.second()`（"NPC 不见了"）。

修在客户端侧：改走包里声明的 `fh`（新增 `FootholdTree::get_spawn_point(pos, fhid)`，
`fh` 不存在或指到竖墙时退回旧反查）。**波及面 = 全部 3097 张有 `life` 的图**，不是番茄田专属。
同轮附带修两条：`MapNpcs` 对同 oid 重下发时只 `makeactive()` 复用旧对象、新坐标整包丢掉；
`Npc`/`UINpcTalk` 的 `strid.insert(0, 7 - strid.size(), '0')` 遇 8 位 id 无符号下溢抛 `length_error`。

**「有的怪物没显示」不是缺素材**：地图上摆的 827 个怪物 id 在 `Mob.nx` 里**一个不缺**，
1280 个 NPC id 只有 1 个缺（见 [../evidence/r24-ui-geometry/README.md](../evidence/r24-ui-geometry/README.md) §4）。

### A10 履带（foothold `force`）此前完全没接（R23-A，本轮）

纯客户端物理属性，不是反应堆事件。51 张图 / 855 条落脚点受益。
机制、单位标定和"这一类还有谁"的完整审计见 [../evidence/r23-map-motion/README.md](../evidence/r23-map-motion/README.md)。

### A11 NPC 对话框：正文压住「确定」+ 底带按钮无槽位（R23-B/R25，本轮）

两条独立成因，都要修：

1. **正文压按钮**（R23-B）：正文起笔 x162 + 折行宽 320 ⇒ 右缘铺到 x482，和右下角「确定」(471..517) 叠住。
   另修了翻页步长写死 400px（可见区只有 230px，每页跳过约 170px 正文）和立绘恒 `flip=true` 把面向正文的 NPC 翻成背对。
2. **底带按钮全靠代码给坐标**（R25 新增证据）：`.nxcache/UI.nx` 里 `UtilDlgEx.img` 的
   **9 个 `Bt*` 节点 origin 全是 (0,0)**，`t/c/s` 也全是 (0,0) —— 素材根本不提供槽位。
   代码里不给 `set_position` 就会全部叠在窗口左上角。原来只有 `CLOSE` 和几个分支显式给了坐标，
   `SENDOK`/`SENDNEXT` 等分支的 `activate()` 走的是默认 (0,0)。

R25 用四张原生帧（528×261 / 526×349 / 523×203 + 二级弹窗 527×351）逐列量出正文块与底带，
落进 `IO/UITypes/UINpcTalk.h` 的常量：

| 常量 | 值 | 依据 |
| --- | --- | --- |
| `TEXT_X` / `TEXT_WIDTH` | 166 / 310 | 正文暗字左缘 163..170、右缘 474..476 取中位；折行宽 = 右缘−起笔 |
| `TEXT_Y` | 48 | 三帧墨迹顶 39 / 58 / 72，**原生疑似按正文高度在正文区居中**，未定案（见下） |
| `BTN_DY` | `height + 49` | 底片 `s` 顶 = `t.height+height`，`s` 内分隔条 +15..+20，按钮压 +21（实测 224 / 312） |
| `BTN_X_RIGHT` | 474 | 46 宽 ⇒ 474..520，右对齐留边 9 |
| `BTN_X_RIGHT_WIDE` / `BTN_PAIR_PITCH` | 466 / 65 | 57 宽的 `BtQYes/BtQNo`；成对间距实测 65（莎丽 接受/拒绝 401 / 466） |
| `BTN_X_CLOSE` | 12 | 「结束对话」在左下 |

> **推翻了一条我自己上一轮写进注释的"实测"**：`TEXT_X=122 / TEXT_Y=35 / TEXT_WIDTH=387`
> 来自一次把 2× 网格叠画看走眼的读数，真量下来该帧正文左缘是 **170**。
> 教训：注释里写「实测」的数字必须能被脚本复现，否则就是在编。复现脚本已归档
> [../evidence/r25-npctalk-geometry/](../evidence/r25-npctalk-geometry/README.md)。

两个**留给真机对帧**的开放点：
- `TEXT_Y`：三帧墨迹顶 39/58/72 不是同一个偏移，先取 cj 原值 48（落在区间中间）。
- 「下项」槽位冲突：`NPC对话2.png` 量到 450 而参考帧量到 474，纵向一个 `height+11` 一个 `height+49`，
  差值来自两张图的裁剪边界不同，**没有编码这个孤例**，等真机同场景拍一张再定。

### A11b `SENDSIMPLE`（NPC 可选项列表）没有动作按钮（预留，未修）

原生 `NPC对话2.png` 的 4 项列表右下角有「下项」，但本仓库 `TalkType::SENDSIMPLE` 分支
在 `switch (type)` 里是 `break`（不激活任何按钮），且 `button_pressed` 的 `NEXT` 路径对 simple 列表
只会 `deactivate()` 而**不发封包**。硬接一个 `Buttons::NEXT` 就是「看得见、点了没反应」的半成品，
违反门禁 9，所以显式预留：需要先定「选中项 → `NpcScriptRequestPacket` 的 selection 语义」再接。
列表本身（渲染 + 点击发 `selection`）是通的。

### A12 退出路径 `STATUS_HEAP_CORRUPTION`（既有，非本轮引入，未修）

04:42 那份**能正常在线一小时**的产物（md5 `51d3fe17…`，PID 49660），在我用 `_quit.py`
（`WM_CLOSE` → 点「确定」）退出时留下了 `C0000374 STATUS_HEAP_CORRUPTION` 的 dump；
上一份 04:42 之前的实例（PID 45244）同样。⇒ 关闭/析构顺序上有一个早就存在的堆损坏，
之前被当成「正常退出」放过了。与 R23/R25 无关，**未擅自扩范围修**，先登记。
取证方法见 [../evidence/r26-login-crash/README.md](../evidence/r26-login-crash/README.md) §5。

### A13 `_boot.py` 的「已进图」判据不可信（本轮踩到）

`hud_frame()` 只看客户区**底部 60 行亮像素 > 500**，NEXON splash 的白底直接满足，
于是把开屏误报成 `HUD bright px=66560 after 11s -> in game`，害我把崩溃当成「部署后没等到」。
判「真进图」要换判据（底栏血蓝条的固定色块 / 小地图顶带），不能只用亮度。


## B. 我能自证，需要线上客户端

> 2026-10-31 22:00 更新：**门禁「构建通过 ≠ 能跑」已过** —— 我用 `xy-config.yaml` 里的
> `login.auto`（admin/admin）自己启动客户端并**成功进图**（彩虹岛/奥比斯，角色 Admin 可见）。
> 驱动方式：`E:/project/_drive.py`（cap/key/hold/move/click/where）+ `_cap2.py`/`_mmprobe.py` 抓帧。
> 关闭走 `WM_CLOSE` → 弹出「确定要退出游戏吗?」→ 点 `确定`，**没有杀过进程**。
>
> 仍然要留意的两件事：
> 1. **进图后有约 3 分钟全黑**（地图资源加载），期间抓帧是黑帧，别误判成渲染坏了；
> 2. ~~上一轮结尾那次「自己退出」仍无 `.dmp`、无崩溃日志，原因未定（见 B6）。~~
>    **已定案**：不是自己退出，是 A7 的 `buttons[id]` 空指针；Release 下无日志无 dmp，现象就是进程消失。

### B1 A1/A4 修复后的复验 —— **已自证完成（22:47 部署 `6AE5FE0D`）**
A1 那半边**已跑完并被推翻**（见 A4）：修完 `UI::draw(1.0f)` 后收缩条仍暗且不抖。
A4 的判据也跑完了，但**结论比原判据更宽**：真正该修的是两处，不是坐标一处 ——
`A5`（三级键解析倒序，导致存档永久无效）+ `A6`（公告条常驻）。实测（文件 `minimap: (5, 24)`）：
收缩条顶边落在客户区 **y=24**，逐行 `(136,136,136) → (221,221,221) → (255,255,255) → (221,221,221)`，
与 `MiniMap/Min/c` 素材真色一致，**不再出现 ×0.465 的 (103,103,103)**；y≤23 全宽黑带消失。
> 仍欠一张「公告正在滚动时」的抓帧（见 A6 末），下次启动顺手补。

A1 段落点名的其余 13 个窗体仍逐个补拍一张，把此前基于暗抓帧得出的颜色判据过一遍。
**这条现在有了新含义**：A5 修好后，若某个窗体在 `Settings` 里有历史拖动值，它会突然开始听存档
（以前一直听代码默认值）⇒ 逐窗拍之前先确认该键在线上 `Settings` 里的值 == 代码默认值。


### B2 小地图三态版式
MIN / NORMAL / MAX 各拍一张对 `正确客户端截图/进入游戏页.png`。注意本图 `has_map=false`
（见 C1），NORMAL/MAX 的**图体是空的**，只能验顶带。

### B3 界面（UIShortCut）双开回归
W2 改过四按钮行为，需确认「界面」键不再出现同窗双开。

### B4 文档订正（不依赖客户端，但排在验证轮之后）
- `evidence/w11-map/coords.md` §A.6 → 改成 A3 的结论；
- `CHECKLIST.md` §G「『目录』GameMenu 5 钮 未实装（B16）」→ 已实装，只剩「场景变更」预留；
- 项目记忆 `live-client-verification-gotchas.md` 里「PrintWindow 抓帧按 winrect+frame 换算，旧 (8,30) 结论已废」
  这条要按**工具**拆开，别一句话带过：
  | 抓帧工具 | 图像原点 | 与 `_drive.py click` 的关系 |
  | --- | --- | --- |
  | `_cap2.py` / `_mmprobe.py`（PrintWindow 到**客户区尺寸**的 DC） | 图像 = **窗口**相对，客户区 (0,0) 在图像 **(8,31)** | `_drive.py click` 用 `GetWindowRect` → **click 坐标 == 图像坐标，不用换算** |
  | `_cap_screen.py`（截**客户区**） | 图像 = 客户区相对 | click 坐标 = 图像坐标 **+ (8,31)** |
  混用两套抓帧时，所有几何结论差 (8,31)；A4 那条「边界对齐到 1px」就是先踩了这个坑才对不上。

### B5 抓帧工具自身的坑（方法论，必须记下来）
`_cap_screen.py` 的 PrintWindow 抓帧**可以整帧被压暗**：`_cap_hud.png` 全帧最大通道值只有 197
（=255×0.7725），而 `_now2.png` 是 255。所以**下任何「这个颜色不对」的结论之前，先量整帧 max**；
max 明显低于 255 的帧只能用于版式/几何，不能用于颜色。

**本轮（23:00–23:50 验证轮）又踩到三条，全部记成硬规则：**

| 坑 | 症状 | 硬规则 |
| --- | --- | --- |
| **拼图（montage）读数不可信** | 我把多张缩略图并排看，据此断言「`g` 打开了装备窗」，而像素统计证明画面毫无变化 | 一次只看**一张整窗图**；要判「有没有变」先看区域 `bright(>150)` 计数差，再决定看不看 |
| **缩略图会把 12px 字糊成错字** | 背包 tab 被我读成「装备/消耗/**装备**/其他/特殊」，1:1 放大后其实是「装备/消耗/**设置**/其它/特殊」，与参考逐字一致 | 任何**文案/图标**级判据必须 `crop` 后 ≥3x 放大看，禁止在全窗缩略图上下结论 |
| **`escape` 不是干净的复位键** | 用「按键 → escape → 再按键」的亮度差分扫描时，escape 顺带收掉了底部展开的聊天面板，全帧 `bright` 从 280k 掉到 160k，之后每个键的 Δ 都被这个台阶污染，`o` 看起来「-120805」其实什么都没发生 | 复位动作**不能和被测键共用同一个亮度oracle**；要么每键之间重新拍基线并只用**窗口所在的小区域**，要么放弃抓帧、改走**封包+代码**证据 |
| **驱动没抢到前台，键全打在别的窗口上**（本轮最坑的一条） | 复测 B9 时 `i`/`e` 毫无反应，我一度判成「Esc 不生效」；`GetForegroundWindow()` 实测 = `3082184`，类名 `MapleStoryClass`、标题「北斗」——**是原生参考客户端**（用户正在里面截图，屏上还弹着「保存图像」对话框），而我的 `SDL_app` 已被最小化到 `(-32000,-32000)`。旧 `foreground()` 不看 `SetForegroundWindow` 的返回值，照样打印 `tapped i x1` | `SendInput` 只投递给**前台线程**，所以「按了没反应」**先怀疑前台**。`_drive.py` 已改成：AttachThreadInput + 合成一次裸 ALT（解除前台锁）+ `BringWindowToTop/SetForegroundWindow/SetActiveWindow`，**并校验 `fg == hwnd`**；拿不到前台就 `ABORT: client is not foreground, no input sent` + 退出码 3，不再假装按过。`_postkey.py`（`PostMessage WM_KEYDOWN/UP` 直投 SDL 队列）是后台兜底手段，可在不抢前台时驱动客户端 |

> 因此本轮**撤回**一条基于「按键没反应」的结论：F 段里 `g` 是死键的**线上探针**证据作废
> （键位错位的结论本身不受影响，它靠的是 `4f 01` 封包解码 + `grep case KeyAction::Id::`，
> 与抓帧无关）。重测必须在 `fg_ok=True` 的前提下做。

最后一条的教训值得单独写：**当「代码 + 封包」已经能给出确定答案时，不要再花抓帧预算去凑旁证**。
本轮键位绑定那张表（见 F）就是靠 `_keymap_decode.py` 解 `4f 01` + `grep case KeyAction::Id::` 两下拿到的，
比之前一整晚的按键扫描都准。

### B6 驱动边界违规（报备）
上一轮在小地图状态变化后没重新量按钮包围盒，第二次点击落在框外 (130,46) → **对游戏世界点了一次左键**。
你定的边界是「不得在世界左键 / 不用快捷槽 / 完事复位鼠标和窗口」，这一条我破了。后续规则补进 B4 的记忆：
**每次目标窗体状态变化后，必须重新量一次按钮 bbox 再点**。
本轮又破了一次：`WM_CLOSE` 后固定 `sleep 1.5` 就点 (595,420)，弹窗还没画出来 → 左键落在世界。
**改法**：发完 `WM_CLOSE` 必须先抓一帧确认弹窗在位、量出按钮 bbox 再点，不能靠固定延时赌。

22:41~22:44 退出弹窗时又**打偏两次**（第三次破边界的嫌疑），逐条核过、结论是**没有接触世界**：
- 第 1 次：`_quit.py` 改成「网格扫描取 hover 最大者」后，忘了扫描结束时光标停在**最后一个探测点**，
  左键打在 img(790,470) 附近；
- 第 2 次：同一次修复前，`red_count>60` 太松，选中了 hover 仅 87 的假目标 img(550,380)。
两次都发生在「确定要退出游戏吗?」弹窗在屏期间 —— 前后各抓一帧，羊皮纸像素数 48909 → 48887 基本不变
（没弹时只有 ~80），说明弹窗未被点掉、点击被弹窗消费，角色没被下令移动、也没踩到世界。
仍算违规风险，`_quit.py` 已加三道闸：`SetForegroundWindow` → 把光标**移回**选中点并**复测 hover**
→ `WindowFromPoint` 断言该屏幕点属于本窗口，任一不满足就**放弃点击**（不点就是最安全的）。
顺带定案：客户端窗口类是 **`SDL_app`**，SDL 只把合成输入交给前台窗口 —— 之前「点了没反应」不是
UI 坏了，是没置前。键鼠同理（本条已并入项目记忆 `live-client-verification-gotchas` 第 6 点）。


### B7 `Settings` 是「退出/登录时整档回写」，外部改存档有两个坑（本轮实测）

`Configuration` 在 `~Configuration()`（`Configuration.cpp:242-247`，仅 `#ifndef _DEBUG`）
和 `UILogin.cpp:415` / `:521` / `LoginHandlers.cpp:155` 三处调 `save()` 全量回写。后果：

1. **改存档必须在客户端真正退干净之后**。本轮我按 `tasklist` 判「已退出」就改了
   `ui.position.minimap`，结果**旧进程约 1 分钟后又回写了一次 `Settings`**（`Configuration`
   静态析构），把改动冲掉，新进程读到的是旧值 → 白跑一轮重启。
   规矩：改存档前 `tasklist` 判空后**再等 ≥60s**，改完 `stat` 一次 mtime，启动后再 `stat` 比对。
   本轮三次实测都符合：窗口消失后 mtime 仍会再动一次（22:19:05 / 22:34:43 / 22:45:06）。
2. ~~**只改代码默认值没用**：存档里有值就覆盖默认。~~
   ← 这条**当时就是错的**，错因是 A5：三级键根本读不进来，所以那阵子**只有代码默认值说话**，
   存档里的位置一律被无视。A5 修完之后，这条反过来成立：存档里有值就覆盖默认（原生语义）。
   ⇒ 「默认坐标」类修复（W0/A4 那一批）的交付要求随之改变：
   - **新键**（存档里没有该项）：改默认值即生效，无需迁移；
   - **已有键**：默认值只对新存档生效，老存档要迁移或让用户拖一次。
   线上 `Settings` 本轮实测与代码默认值逐键相等（A5 回归核查），所以不迁移也不会错位。
3. 早先记的「`inv/skill/notice` 拖动过的坐标被全量回写重置成默认值」——**真因是 A5**：
   拖动值确实写进了文件，只是下次启动读不出来，于是显示回默认。A5 修复后拖动才真正可持久。


---

### B8 悬浮提示（tooltip）四条实测缺陷 —— **本轮新发现，W5 判为未完成**
证据：`_v_TIP1.png` → 裁剪放大 `_v_ztip.png`（背包第 1 格「蓝牛仔短裤 (M)」悬停）。
背包里**确实有 2 件装备**（蓝/红牛仔短裤），所以此前「空背包拍不出 tooltip」的限制已解除，
`atlas_dbg.log` 的 19 条 `no glyph` 也在这张图里**没有复现** ⇒ 那 19 条应重新归因（多半是早期构建）。

| # | 现象 | 判据 | 处置 |
| --- | --- | --- | --- |
| B8.1 | **框体下半截透出背包的「402,653,184 金币」条** | `_v_ztip.png` 里金币条文字压在 tooltip 内部 | ✅ **原生参考帧定案，无需改**：`e75687e9` 逐像素反算 ⇒ 底 = 纯黑 **α=0.90**（覆盖 238 得 24、覆盖 255 得 26），边框 (128,128,128) 1px，分隔线白 α0.4（覆盖黑底 102 / 覆盖 24 底 116）。三个值与 `TextTooltip.h:37` 现有实现完全吻合 ⇒ **透出背景正是原生行为**，改成 1.0 反而偏离原生。证据 [equip-native-compare.md §2](../evidence/w5-tooltip/equip-native-compare.md) |
| B8.2 | **需求两列互相重叠**：`要求力量/要求敏捷` 叠成糊块，`要求运气/要求智力` 同样叠行 | `reqstatpositions` 行距 47→62→**71**，第三行只差 9px，而 A11M 行高约 15px | ✅ **已修**：`IO/Components/EquipTooltip.cpp:57-64` DEX/INT 的 y `71 → 77`，三行统一 15px 行距 |
| B8.3 | ~~数值渲染成 `000`~~ | `EquipTooltip.cpp:379-384` `reqstr.insert(0, 3 - size, '0')` —— **有意补零到 3 位** | ✅ **原生定案：不是缺陷，且分支也对**。`e75687e9` 实测：STR/DEX/INT/LUK 四行都是**三位**「000」（墨点 x152,158,164 / x232,238,244，pitch 6），而**要求等级是单字符**「0」⇒ 与 `ms != MapleStat::Id::LEVEL` 才补零的写法逐位吻合 |
| B8.3b | **补零写法有越界风险**（顺手抓到的） | `3 - reqstr.size()` 是 `size_t`；需求值一旦 ≥1000（4 位）就下溢成天文数字 ⇒ `insert` 抛 `length_error`，tooltip 直接崩 | ✅ **已修**：加 `reqstr.size() < 3` 守卫。当前版本数值到不了 1000，属**潜在**崩溃，不是线上现象 |
| B8.4 | **分隔线上方一行渲染成纯黑块**（职业名行） | ~~缺素材~~、~~没推进 x 是缺陷~~ **两条都已证伪** | ⛔ **本条判定反了，已回退**。`e75687e9` 里原生职业名**本来就是一团 36×11 的白块**：把 `Can` 下 6 张职业名按**同原点叠绘**合成后与原生对拍 **SAD=0.12（仅取原生墨点 SAD=0.00）**，改用任意单张则 SAD 95~148 ⇒ 原生画法就是上游那个「不推进 x」的循环（`git show HEAD` 第 702-703 行）。当年线上看到「纯黑块」的真因是 `jobs[]` 取自不存在的 `UIToolTip.img/Item/Equip/Job/*` ⇒ **空纹理**，与叠绘无关；W5 把路径改对后叠绘即还原原生。<br>**处置**：`EquipTooltip.cpp:644-651`、`:751-753` 两处循环**回退为同原点叠绘**（原生保真）。副作用＝全职业装备职业名处显示白糊块，与原生一致。**若用户更看重可读性，改回 `job_x += job.width() + 2` 横排即可**（已列入 CHECKLIST 待拍板） |
| B8.5 | **英文串泄漏到正式 UI**：`Type: BOTTOM` / `Remaining Enhancements: 7` / `Hammers Applied: 0`（`EquipTooltip.cpp:519/537/544`，另有 `_preview` 一套在 `:265/294/301`） | 分隔线以下整段 ASCII | ✅ **证伪：原生就是英文**。`e75687e9` 里北斗原生 tooltip 显示的正是 `ATT INCREASE` / `Type: TOP` / `Defense: 3` / `Remaining Enhancements: 7` / `Hammers Applied: 0`（只有 `要求*` 与职业名是位图中文）。⇒ 这些串**不是泄漏**，与原生一致，不改 |

> 定位入口：`IO/UITypes/UIItemInventory.cpp` 的 hover 分支 → `ItemTooltip`（`Util/` 或 `IO/` 下），
> B8.5 那几串是 ASCII 字面量，直接 grep `Remaining Enhancements` 就能命中。

### B9 背包窗「第一次 Esc 不关、要按第二下」—— 根因与 I-5 是同一条，已修待复测
原始观察（旧验证轮）：`i` 开「现有道具」→ `escape` ×1 窗口原样在 → 再按 `i` 才关；
而 `e` 开「角色装备」→ `escape` ×1 就关了。

**代码侧证据（本轮补齐）**：
- `UIItemInventory::send_key` **本来就处理 Esc**（`IO/UITypes/UIItemInventory.cpp:939-946`：
  `if (pressed) { if (escape) toggle_active(); }`），`UIEquipInventory` 同款在 `:586-593`；
- 路由也是通的：`IO/UI.cpp:325-344` 在 escape 时把 `ITEMINVENTORY`/`EQUIPINVENTORY` 等都塞进
  types，`UIStateGame::get_front(types)`（`:793`，按 `elementorder` 从最前往后找 active 的那个）
  命中后 `element->send_key(...)`；键盘 `emplace` 打开窗口时 `pre_add` 会把该 Type
  `remove+push_back` 到 `elementorder` 末尾（`UIStateGame.cpp:720-723`），所以键盘开的窗也在序里。
- ⇒ 「背包没写 Esc」这个假设**不成立**。

**最可能的真因就是 I-5**：`UIStateGame::send_key` 旧实现在「`focused` 指向的窗体还在表里但已
`!is_active()`」时，清掉 `focused` 后**直接 `return`** —— 这一下按键既没交给窗体也没交给全局开关，
被静默吞掉。表现正好是「第一下 Esc 没反应（被吞），第二下才生效」。装备窗那次能一下关掉，
只是当时鼠标底下的焦点状态不同。
✅ **已修**：`IO/UIStateGame.cpp:211-226` 清脏焦点后**继续**往下 dispatch，不再丢弃按键。

**复测配方（必须在 `fg_ok=True` 下做，见 B5 第四条坑）**：
1. `python _drive.py focus` 确认输出 `focus ok=True`；
2. 鼠标停在**一个已关闭窗体原来占的位置**（制造脏焦点），`key i` 开背包 → `key escape` ×1
   → 抓帧：修复前需要两下，修复后应当**一下**就关；
3. 同法各测 `e` / `k` / `s`，确认 Esc 一律一下关闭，且没有「按一次开两次」的回归。

- 顺带证伪一条我怀疑过的缺陷：**`i`/`e` 不重复堆窗** —— `UIStateGame.cpp:245` 虽然是无条件
  `emplace<UIItemInventory>`，但 `UI::emplace` 会复用已有实例并 toggle，所以「双开」只在
  那些**没走复用路径**的窗体上成立（B3 的 UIShortCut 仍要单独验）。
- **更正我先前的一条错判**：我曾写下「v83 原生背包右上角本来就没有 ✕，最右蓝的是展开钮」，
  那是**错的**。用 `tools/nx_inspect.py` 把 `Basic.img/BtClose/normal/0` 导出放大比对后确认：
  背包右上三颗是 红`|+`(BtFull/BtSmall 展开-收起) / 红`+`(BtGather 整理) / **蓝`✕`(BtClose)**,
  白色 X 在 12×12 尺寸下看着像四个点，所以我误认成「展开钮」。像素定位：蓝钮在 frame
  x 878–892、y 93–112；我第一次点在 (885,102) 判定「关闭钮失效」**也不是缺陷**，而是我自己
  点偏了。**B9 真正的缺陷只剩一条：背包窗不响应 Esc（点击 BtClose 是有效的）。**

### B10 小地图地图名跨场景不刷新 —— ❌ **撤回，是我误判**
原先的依据：`_v_A0.png`（画面里有 Telephone 亭和 Perion/Ellinia 路牌）小地图顶带仍写
「彩虹岛：番茄田」，我据此认定「换图后不重算」。

**代码侧证伪**：`UIMiniMap::update()` 每帧比较 `Stage::get().get_mapid()` 与缓存 `mapid`
（`IO/UITypes/UIMiniMap.cpp:161-165`），一旦变化就在同一个块里重算 `Map` 节点、`scale`、
`center_offset`，并在收尾处依次调 `update_text(); update_buttons(); update_canvas();
update_static_markers(); toggle_buttons(); update_npclist();`（`:242-247`）。
`update_text()` 里 `combined/region/town` 三段文字全部由
`NxHelper::Map::get_map_info_by_id(mapid)` 重新取（`:605-611`）。
⇒ 换图时地图名**确实会重算**，路径是通的。

**我错在哪**：整轮验证里角色**从没真正换过图**（没有传送/没有走门），出生图本身在 v83 中文表里
就叫「番茄田」，画面里那组路牌/电话亭是**出生图的美术**，不是 Henesys。我把「美术看起来像别的图」
当成了「已经在别的图上」。

**留一个真正需要线上验的点**（不改代码）：换图后地图名重算依赖
`get_map_info_by_id()` 能在新 mapid 上取到 `info/name`、`streetName`；若某张图在 `Map.nx` 里
缺这些键，`change_text("")` 会让顶带**变空**而不是「留着旧名」。这条要一张**真的换过图**的抓帧
（走传送门或 GM 传送）才能验，归到联合验证轮。

### B11 `_ui_asset_diag.log` 每次启动都在写 —— 待决：留还是删
当前内容仍是缺件清单（`Combo`/`Number`/`UIWindow6` 缺、`VScr9→VScr5` 钳制、`Stat/BtAuto` 跳过）。
按门禁「临时调试日志闭环后删除」应当收掉，但它同时是唯一能一眼看出**本包缺哪些素材**的探针。
建议：改成「只在 `xy-config.yaml` 开了诊断开关时写」，默认静默 ⇒ 既守门禁又保住工具。**这条要用户点头。**

### B12 功能窗关闭钮「怎么是白色的」（R14-4）—— 已按四类定案，33 窗走共享**蓝底白 X**
> **2026-11-01 更正（原结论反了）**：本条早先写的是「`BtClose` 是实心方块不是 X，原生描的是
> `BtClose2` 描边 X」。四张原生标题栏逐像素对照推翻了这个判断 —— 原生画的就是
> **`Basic.img/BtClose`（12×12 实心蓝块 + 白 X）**，而 `BtClose2` 内部**全透明**，
> 贴在白色标题栏上只剩一圈黑线，**这才是用户说的"怎么是白色的"**。
> `close_btn()` 探测顺序已改成 `{ BtClose, BtClose2, BtClose3 }`，注释同步重写。
> 证据与四扇窗（技能 / 背包 / 好友 / 能力值）的裁片：
> [evidence/r15-closebtn/README.md](../evidence/r15-closebtn/README.md)。
> 遗留：33 扇窗的关闭钮坐标当初是按 14×14 写的，换 12×12 后右/下沿各内收 2px，属逐窗校准批次。

用户原话还带一句「其他功能窗口也一样」。逐颗导出本包所有候选关闭钮点阵（脚本与输出已归档
[evidence/r14-closebtn/](../evidence/r14-closebtn/)），关闭钮素材其实有**四类**，不能一刀切：

| 类 | 件 | 实测 | 处置 |
| --- | --- | --- | --- |
| A 窗根**没有**关闭钮 | `Item`/`Stat`/`Skill`/`Quest`/`Trunk`/`TradingRoom`/`UserInfo` … | 节点不存在 ⇒ 旧写法造出的是**空贴图**，`Texture::draw` 在 `is_valid()` 直接 return（[Graphics/Texture.cpp:94](../../../Graphics/Texture.cpp#L94)），所以既看不见也点不动 | 一律 `UiAssetResolve::close_btn()` → `Basic.img/BtClose`（12×12 实心蓝块 + 白 X，不透明比 0.97）。现共 **33 个窗**走这条 |
| B 实心亮块 | `Basic.img/BtClose`、`UIWindow.img/CashGachapon/BtClose`、`UIWindow.img/Family/BtClose` | 12×12，不透明比 **0.97**，三颗点阵同形（均值 BGR 168,135,65 / 167,133,63 / 164,131,66，**探针按 RGBA 求均值所以看着像暖色，实际是蓝**）| 就是原生本尊；后两颗是各自窗体的原生件，**不换** |
| C 文字宽钮 | `UtilDlgEx/BtClose` 85×19（与 `BtYes/BtNo` 同尺寸）、`NewYearsCard/BtClose` 48×22、`FamilyTree/BtClose` 11×11（暗底 X 镂空）| 不透明比 0.93 / 0.99 / 0.73 | **保真**。`UINpcTalk` 在 draw 里 `set_position` 重排（[IO/UITypes/UINpcTalk.cpp:649](../../../IO/UITypes/UINpcTalk.cpp#L649)），不受共享件影响 |
| D 原生英文 CLOSE 圆顶钮 | `UIWindow.img/BtUIClose` / `BtUIClose2` | **32×15**，origin (0,0)，蓝底圆顶 + 白 X + 「CLOSE」字样（导出图已存 `E:/project/_r21_btuiclose_8x.png`）| `UIMessenger` 用它是原生正解；但位置写的是 `dimension.x() - 21`（[IO/UITypes/UIMessenger.cpp:132](../../../IO/UITypes/UIMessenger.cpp#L132)）—— 那颗钮宽 32 ⇒ **右沿越窗 11px**。缺原生悄悄话房截图，暂不改，挂验证轮 |

顺带修掉三处**凭空写出来的**旧逻辑（`bn ? MapleButton(bn) : MapleButton(bn, pos)` 两分支都拿不到贴图，
真值分支还会把坐标丢成窗口原点）：`UIBuddyList`、`UIGuild`、`UIMerchant`、`UIStorage`、`UIParty`、`UITrade`
全部改走 `try_maple_btn` / `close_btn()`。其中 `UIParty` 原循环拿**下标当按钮 id**，而枚举里
`BtClose=2` 正夹在 `BtLeave` 与 `BtLeader` 之间 ⇒ 一旦 `Party.img` 存在就会把「队长」钮注册成关闭钮，已改成显式 id 表。

### B13 滚动条「没有滑块」的另一半：命中宽度只有真实宽度的一半（R14-5）
`Slider` 的 `thumbwidth` 上游写死 **8**，而本包兜底根 `Basic.img/VScr5/enabled/thumb0` 实测 **15×25**
（`prev*/next*/base` 都是 15×13）⇒ 右半 7px 按不住，表现就是"滑块点不动/拖不动"。
已改成从位图取真实宽度（[IO/Components/Slider.cpp:91](../../../IO/Components/Slider.cpp#L91)）。
两个坑记下来：
1. `nl::node` **没有** `get_dimensions()`（那是 `Texture` 的接口，`buttonheight = dnext.get_dimensions().y()`
   能过编译只因为 `dnext` 是 `Texture` 成员）⇒ 取位图尺寸要走 `.get_bitmap().width()`。首轮构建就是这条编译失败。
2. 「内容不足一屏时不显示滑块」与原生一致，**不是缺陷**；只有"该有滑块却没有"才算。

### B14 「点宏指令糊满屏幕」的真身：组合栏是 `SkillMacro` 不是 `SkillMacroEx`（R15-1）

用户原话：「点『宏指令』一下子弹出很多窗口还有二级密码窗口，**应该只显示技能组合栏而且在技能栏右侧**；组合栏内部 UI 缺失」。
三层原因，逐层已修：

1. **一次弹四窗**。旧 `UISkillBook::Buttons::BT_MACRO` 同时 `emplace` `UIMacro` / `UISkillMacroEx` /
   `UISkillEx`（法师再加 `UIAranSkillGuide`）。原生只开**一扇**技能组合栏 ⇒ 已改为单开
   `UISkillMacroEx` 一类（[IO/UITypes/UISkillBook.cpp:481](../../../IO/UITypes/UISkillBook.cpp#L481)）。
2. **「二级密码窗口」是副作用，不是有人点了它**。`UISoftKey`（148×278 安全键盘底板）在那几扇窗的**构造函数**里被
   拉起 ⇒ 窗一多就刷屏。新 `UISkillMacroEx` 构造不再自动开软键盘，改名（S/R）时才按需创建。
3. **「内部 UI 缺失」= 挂错了壳**。逐组件模板匹配（原生帧 `技能展开宏指令.png` 379×291，窗口原点 = 截图 (172,0)）：
   `UIWindow.img/SkillMacro` 的 `backgrnd` **207×289** 全部命中，而原先用的 `SkillMacroEx` 是
   **181×303** —— 303 高在这张 291 高的帧里**放不下**，且 181 宽对不上 207。所以原生组合栏的真身是 `SkillMacro`。
   另外 `Macroicon/0..2` 不是"结果技能图标"，它是 **TwoSpriteButton 契约的宏选择钮**（icon / iconMouseOver 两态，
   32×32，行距 45，SAD 0）；`macroslot` 124×38 一行三格（格中心 x 18/55/92）。全部几何与 SAD 表见
   [evidence/r15-skillmacro/README.md](../evidence/r15-skillmacro/README.md)。

位置：`mex->anchor_right_of(*this, -3)` —— 原生帧里技能窗 175 宽起于 x=0、组合栏起于 x=172（重叠 3px），
两窗同高 289 ⇒ 顶边天然对齐。
交互：三格按行读写 `SkillMacros::get_slot/set_slot`，**只写进"当前选中那一行"的第一个空格**；
旧实现走全局 `assign_skill`，会把技能塞进别的宏。滚动条 `Slider(DEFAULT_SILVER, 67..165, x=186, 3 行可见)`，
`setrows` 在 `draw()` 里随封包宏条数同步（`slider` 因此是 `mutable`，同 `name_label` 先例）。

**四项显式预留**（都是"素材里没有"，不是没做）：确定钮（1441 张位图全扫，无「确定」底板，最优 SAD=63 是
`Incubator/BtOk/disabled`，那顆烤的是「孵化」字样）、`显示个性技能名` 勾选框、`macroname` 里的白输入框
（该位图 >248 的白像素只有 **1 个** ⇒ 原生白框是运行期描的，非资源）、`macroslot2` 168×42 选中行底
（SAD=228，参考帧里没有选中行可对照）。

**真机自验后追加的第二条根因（2026-11-01，构建 `51d3fe17…`）**：换壳上线后首帧抓出来仍是**白板**。
素材树核对齐全 ⇒ 不是缺资源，是 `draw()` 里 `if (idx >= total) break;` 在 `SkillMacros::size()==0`
时把整段编辑区跳过；而 `count` 只由 recv `SKILL_MACROS` 的 `SkillMacrosHandler` 填，本号**登录时没收到**、
换图后服务端重发才有 ⇒ 已改成 `rows = max(total, 1)`（空宏表也必须画一行空槽，否则既看不出版式也没处点选择钮），
并把 `send_cursor()` 的命中界与 `draw()` 对齐。抓帧与逐步断言见
[evidence/r15-skillmacro/README.md §7](../evidence/r15-skillmacro/README.md)。
遗留：登录是否必然下发宏表，属协议侧，需一份真实包体，不在本 UI 线内动。

### B15 门禁自己的 1px：紧裁原生帧的锚点被尺寸约束强制（2026-11-01，**已修**）

**症状**：能力窗 / 背包 / 技能窗三处真机对账**同时**报 Δ=1（`slot_rows` 49 vs 50、
`meso_digit` 268 vs 269、`sp_digit` 268 vs 269）。三个互不相干的窗口同向偏 1px，
第一反应是「公共文本件低了一格」，但那正是最贵的误判 —— 会驱动 B3–B7 去「修」七个本来没错的窗口常数。

**真因**：oracle 全是相对锚点的坐标，而 `anchor_in` 硬性要求模板整张落在帧内。
用户给的原生截图是紧裁的，`背包栏（折叠）.png` 高 289 == `Item/backgrnd` 图高 289
⇒ dy 只有 0 这一个合法解，**竖直锚点是被裁出来的帧的尺寸约束强制的，不是内容对上的**。
这张截图实际上裁掉了美术的第 0 行，所以帧的 y0 = 美术 y1，整张 spec 齐偏 1px。

**怎么证的**（两把独立尺子，不靠单点结论）：
1. 新子命令 `layout.py bias <spec>`：允许模板出界、只比重叠实心像素，在记录锚点周围扫 ±2 的
   meanSAD 网格 → Item 的 dy=-1 是 9.77 / agree 0.913，dy=0 只有 23.37 / 0.792，分得很开。
2. 拿美术 PNG 自己当尺：金币白框上边框行在 `Item_backgrnd.png` 里 = 252、真机帧窗口相对 = 252、
   原生帧 = 251 ⇒ 原生帧的 art 左上角在 y=-1。（真机锚点同时被反向校验为正确：dy=0 最优、meanSAD 7.18。）

**处置**：`mark --anchor 3 -1` 重写 oracle（值仍然是量出来的，只是参照点换成量出来的那一格）
⇒ `slot_rows` 50/84/118/152/186/220、`meso_digit` (131,269)，**真机帧三格 Δ=0**。
⚠ 更正（同日修 `cmp_probe` 后）：这句对 `slot_rows` / `meso_digit` 的 **y** 成立，但当时 `meso_digit` 的
`x+w` 是**空转出的假 0**（派生键没走 `keyval()`）。修好后 `x+w` 在原生帧真 Δ=0、两张改前真机帧 −1 ⇒
金币**水平**落位退回待复验，见 [../evidence/r27-b2-bagfull/README.md](../evidence/r27-b2-bagfull/README.md)。
其余 6 张 spec 全部 `bias` 通过（记录锚点即最优格；KeyConfig / EquipTooltip 记录格 meanSAD=0.00），
**只有 Item 是紧裁等高的受害者** —— 但这检查必须每轮做，因为下次拿到更紧的裁图就会再中。

**顺带修掉两个由负锚点暴露的工具缺陷**（都在 `layout.py`，细节见
[../evidence/r27-oracle/README.md](../evidence/r27-oracle/README.md) 的锚点一节）：
`check`/`selftest` 原先在 oracle 帧上**重新自动锚定**，把 `mark` 的校正抹掉（改锚后反而报 Δ=1）；
以及负锚点让 `selftest` 的注入框变成 `src[-1:288]` 空切片 → `fill_far` 收 NaN 直接崩。

**给后续批次的规矩**：
- **Δ=1 且多窗同向 ⇒ 先跑 `bias` 审锚点，再怀疑客户端常数。** tol=2 会让这种伪影一直「ok」，
  所以它不是靠 FAIL 暴露的，只能主动审。
- `sp_digit` 除 y 外还有**墨迹右缘** 1px 缺口（原生 103+6=109、真机 102+5=107）。
  本轮不追：A12M 是 RIGHT 对齐，「x 是墨迹右缘还是 advance 右缘」是 B1a/origin 语义那一类问题，
  且原生那格的墨宽本身就比我们多 1px（同一字号的字形光栅差）。**归 #84 B4 技能窗**，
  复测配方：`check SkillMacro.json <技能窗真机帧>`，看 `sp_digit` 的 `w` 与 x+w 两栏，
  若右缘恒短 1~2px 则是 RIGHT 对齐语义，改公共件而不是改 109 这个常数。

## C. 需外部证据（IDA / 缺素材 / 服务端），客户端侧无法自证

### C1 小地图本图 `has_map=false`
`Map["miniMap"]["canvas"]` 缺失 → NORMAL/MAX 图体空白；且 `update()` 里 `kMaxCanvasEdge=2048`
越界保护会把 `has_map` 置 false 并退回 MIN。需要换一张**有 miniMap canvas 的图**再看，或从
`Map.nx` 侧确认该图是否真的没烤小地图。

### C2 小地图 −/＋ 缩放钮无法还原
参考端顶带有蓝色「−」和灰色「＋」，但本包 `UIWindow.img/MiniMap` 只有 `BtMap`（36×12）；
`BtMin/BtMax/BtSmall/BtBig/BtNpc` 与 `Window/Max` 子树**都不存在**。→ 素材代次差异，不是代码问题。
要还原得换 UI 素材代次（同 C3）。

### C3 `UIOptionMenu` 只能是空壳
本包 `GameOpt`/`SysOpt` 下**只有 `backgrnd`**（158×284 / 299×365），没有任何控件；且无
`StatusBar3.img`、无 `UIWindow2/4/6.img` 根。所以「游戏设置 / 系统设置」无论怎么接都拍不出原生面板。
A2 里两颗钮暂时都路由到 `UIOptionMenu`，等素材到位再拆成子页。

### C4 背包中间那颗钮：`BtSort` 还是 `BtGather`
`UIItemInventory.cpp` 里两个名字都出现过，原生到底哪颗在中间位、点击语义是什么，**需要 IDA**
（`UIInventory` 的按钮回调表）。现在按抓帧版式放了，动作未定。

**本轮把「该有几颗」钉死了**（参考 `背包栏展开.png` 右上角裁切放大 `_v_zrefclose.png`）：
红 `|+` / 红 `+` / 蓝 `✛` **三颗**，最右那颗蓝色是展开/收缩（不是关闭，v83 背包根本没有 ✕）。
而线上 `_v_zclose.png` 只数出 **红 `|+` + 蓝 `✛` 两颗** ⇒ 中间那颗红色（`+`）缺失。
代码侧本来给它留了位：`UIItemInventory.cpp:1641` `place(Buttons::BT_SORT, Point(W-31, 4))`
和 `:1640` `place(BT_GATHER, …同坐标)` —— 两颗**叠在同一个点**，且 `:1242-1269` 的
`set_act` 逻辑在任一时刻只 `true` 其中一颗。所以「缺一颗」的直接原因大概率是
`Item["BtSort"]` / `Item["BtGather"]` 里**有一颗在本包不存在**（`try_maple_btn` 静默跳过），
而不是坐标错。下一步：用 `tools/nx_inspect.py` 列 `UIWindow.img/Item` 下 `Bt*` 子节点，
看这两颗到底有有几颗 —— 这是**素材清点**，不需要 IDA 就能定性。

#### 定案过程（全部本地取证，没靠猜）
1. **素材清点**：`nx_inspect list … UIWindow.img/Item 1` ⇒ `BtCashshop / BtCoin / BtFull / BtGather / BtSmall / BtSort`
   **六颗全在**，且 `BtSort`、`BtGather` 各有 `normal|mouseOver|pressed|disabled` 四态 12×12。⇒ **不是缺素材**。
2. **认脸**：导出 `normal/0` 逐个看（`_a_BtSort_big.png` 等）——
   `BtSort` = 红底**三条横杠**；`BtGather` = 红底**白色 `+`**；`BtFull`/`BtSmall` = 红底 `|+`（箭头方向相反）；
   `Basic.img/BtClose` = 蓝底白叉（12×12 下看着像 4 个白点 ⇒ 我一度误认成「展开钮」）。
   ⇒ 两张参考里中间那颗红色 `+` **就是 `BtGather`**；最右蓝色**就是 `BtClose`**
   （所以「v83 背包没有关闭钮」我先前说反了：**它有**，只是长得不像 ✕）。
3. **量位置**：`_v_Z4.png` 沿 y=100 扫色 ⇒ 窗口左边界 x=285、右边界 x=881（W≈596 ≈ `FullBackgrnd` 603），
   红心 local x≈556 = `W−51` 槽 ✅、蓝心 ≈591 = `W−15` 槽 ✅、**`W−31` 槽是空的**。
4. **找根因**：`set_sort()` 的 `if (full_enabled)` 分支**只点亮 `BT_SORT_SM` / `BT_GATHER_SM`**，
   而这两颗只在 `if (FullAutoBuild)`（`:250`）里创建 —— 本包走的是 classic 分支（`:225`），`FullAutoBuild` 不存在
   ⇒ `_SM` 两颗**根本没进 `buttons`**，`find_btn` 返回 null、`set_act` 静默空转 ⇒ 中间槽整颗消失。
   触发点：`set_full()` 结尾 `:1671` 会调 `set_sort(sort_enabled)`，所以**一展开就空**。
5. **顺带第二条**：构造初值 `sort_enabled(true)`（`:112`）会让**未展开**时显示 `BtSort`（三横杠），
   与两张参考（都是 `+`）以及切 tab 的复位 `:666 set_sort(false)` 都不一致。

#### 修了什么
- `IO/UITypes/UIItemInventory.cpp:1238` 起：展开分支加 `sm_pair` 守卫 —— `if (full_enabled && sm_pair)`，
  `_SM` 对不存在时**回退到与非展开态共用的同一对**（classic 包因此重新有中间那颗）。
- `:112` 初值 `sort_enabled(true)` → **`false`**，使开窗即显示 `BtGather`，与参考和 `:666` 复位一致。
- 两处都写了「为什么」的注释，并注明：**若 IDA 后续证明 `_SM` 才是原生展开态的真身，去掉守卫即可**，回退成本一行。

> **仍未定案**：`BtSort`/`BtGather` 的**点击语义**（按钮表示「当前模式」还是「下一步动作」）
> 只有包络证据（`BT_GATHER → GatherItemsPacket`、`BT_SORT → SortItemsPacket`），
> 没有原生端验证过「点一下会不会换成另一颗」。这条**留在 IDA 待办**，不算闭环。

### C5 商城
出站 40 vs `CASHSHOP_OPERATION=229`、HUD 换态靠 recv `SET_CASH_SHOP=127` —— 属 s9 协议线，
挂 #24 T14，本线只做到「明确提示未支持」（已线上验证过黄字出现）。

### C6 NPC 对话乱码（T13）
`UINpcTalk.cpp:467-519` 残留收尾 `#`、`:157-165` 逐字节打字机。属 IME/文本处理线，不并到 UI 版式线做。

### C7 技能窗三处疑点，本轮逐条定性（结论和最初记录不同）

参考帧已裁进 [../evidence/refcrops/skillwin.png](../evidence/refcrops/skillwin.png)。

1. **新手技能行序（真问题，但本包无解 → C）**
   参考端自上而下 = `团队治疗 / 疾风步 / 蜗牛投掷术 / 精灵的祝福`
   = skill id `0001001, 0001002, 0001000, 0000012`（名字↔id 由 `String.img/Skill.img` 反查得）。
   我们的行序来自 `JobData::JobData()` 遍历 `Skill["000.img"]["skill"]` 的子节点
   （`Data/JobData.cpp:38-46`），而 `nx_inspect list … Skill.nx 2 000.img/skill` 实测子节点是
   **按 id 升序**存的（`0000008, 0000012, 0000100, …`）⇒ 原生 WZ 的**插入顺序在 .nxcache 里已经丢了**。
   要复刻只有两条路：拿原始 `.wz`（插入序）或给每本书写死一张顺序表。**不是取用写错**。
2. **`0000012` 光板（改判为 B 类，需线上复验）**
   原记「图标节点缺失」——**本轮判否**：`Skill.nx/000.img/skill/0000012/icon` 是 32×32 有效位图
   （实测色计数 262 白 / 168 (85,102,119) / 112 黑，见 `_ic_000.img_skill_0000012_icon.png`）。
   `SkillDisplayMeta` 也确实取 `data.get_icon(NORMAL)`（`IO/UITypes/UISkillBook.cpp:86`）。
   ⇒ 若线上仍是光板，那是**遮挡/裁剪/绘制顺序**问题，不是缺素材。B1 复验时优先看这条。
3. **BookName「冒险的技术」vs 参考端「冒险家技能」（改判为 D 类，不是 bug）**
   `JobData::JobData()` 读的就是 `String["Skill.img"]["000"]["bookName"]`（`Data/JobData.cpp:36`），
   而本包该节点的值**确实是**「冒险的技术」（`PYTHONIOENCODING=utf-8 nx_inspect list String.nx 2 Skill.img`）。
   全量 grep 本包 `Skill.img` 的 12103 行字符串，**没有**「冒险家技能」这个值。
   ⇒ 参考端那台客户端的 `String.nx` 是**另一代翻译**（其它书如 `100/bookName=战士技能`、
   `110/bookName=剑客技能` 都符合「<X>技能」式命名，参考端把 000 也归一成了「冒险家技能」）。
   **禁止为了对齐截图去硬编码书名。**
4. 职业行第二行 `[新手]`：参考端 `角色能力` 窗「职业」行就是 `新手[新手]` ⇒ 我们那行不是 bug（D 类）。

### C8 任务详情缺页码图标（NPC 头像部分本轮已修，见 A8）
`UIQuest` 详情面板的「第 N / M 页」那类小图标在参考端有、我们没画。
头像那一半已按 `QuestData::get_npc()` + `Npc.nx/<id>/stand/0` 落进蓝横幅右格（A8）；
页码图标仍缺 —— 卡点是**不知道它由什么驱动**（`Quest/backgrnd2` 里没有对应位图，
`Quest` 根下也没有 page 相关节点），需要 IDA 侧 `CQuestWindow` 的绘制函数才能定性，本会话无 IDA MCP ⇒ 预留。

### C9 HUD `EXP [0.00%]` 的数字位
经验百分比数字与参考端不一致，需 `StatusBar` 位图字库那套的度量依据（W1 遗留）。

### C10 键帽字母：位图字库本包确实没有，但 W3 已按回退方案做完
参考帧裁片：[../evidence/refcrops/keycaps.png](../evidence/refcrops/keycaps.png)
（两行：`Shift/Ins/Hm/Pup` + `Ctrl/Del/End/Pdn` 特殊键，加数字/字母键与粉色功能标签）。
W3 的落地状态见 [../evidence/w1w3-hud/coords.md](../evidence/w1w3-hud/coords.md) §2.3：
- `key/0..6` = Shift/Ins/Hm/Pup/Ctrl/Del/End 位图贴纸；**`key/7` 在本包是红箭头（28×20）不是 Pdn**，已排除；
- 数字走 `number` 位图字库；
- **字母位图字库在 `Basic.img` 全 71 根里不存在**（E0）⇒ 字母/`Pdn` 用 `OutlinedText`
  （#AABBCC 字身 + 1px 白描边，按参考帧逐像素取证）统一回退。

⇒ 残留只到「字母观感是描边文字而非原生位图」这一层。要彻底对齐只能补素材（换 UI 代次）
或按门禁 16 用 Photoshop MCP 重绘一套字母贴纸 —— **需要你先裁决走哪条**，本版不动。

### C11 服务端侧仍开着的开关
- Admin 监控（后台）仍在跑，本线未动；
- `game_config` 的双向日志位**已只读核实为 false**，见 D1。

### C12 `foothold/forbidFallDown` 数据有、客户端零实现（R23-A 同类审计副产物）
400 张抽样里有 322 条落脚点带这个键（全量比例见
[../evidence/r23-map-motion/_r23_mapfeat.txt](../evidence/r23-map-motion/_r23_mapfeat.txt)），
全仓库 `forbidFallDown` **0 引用**。语义是"这段平台上不许往下跳/掉落"，
要接就得动 `Foothold` + `FootholdTree` 的边界判定，且**原生到底怎么表现我没有证据**
（无 IDA、无参考帧）⇒ 按门禁 5「禁止想象修改」不接，只在此显式预留。

### C13 任务窗「寻找NPC位置」按了仍然没反应 —— 显式预留，不是漏实现
原生行为是"把该任务 NPC 所在地图在世界地图上高亮点出"。要落地需要两样本包都没有的东西：
1. **npc → map 反查表**（`QuestData` 只有 npc id；地图 `life` 客户端根本不读，见 A9）；
2. `UIWorldMap` 的"标记某个 NPC"入口（现有代码只支持标记地图）。
且没有任何封包/IDA 证据可对照 ⇒ 不发明协议。代码里的预留注释见
`IO/UITypes/UIQuestLog.cpp` 的 `button_pressed()` `case Buttons::MARKNPC`。
注意：这颗钮**过去连点都点不到**（A8 的 `dimension` 笔误），现在能点、只是行为预留。

### C14 `Mob::set_stance` 对 stance 字节 ≥12 会 `std::out_of_range`（审计到、未证实发生）
`animations` 只插了 STAND/MOVE/JUMP/HIT/DIE 五个偶数键，
`set_stance(uint8_t)` 只钳下界（`< MOVE → MOVE`），上界不钳；
`nameof()` 的 `index = (stance-1)/2` 对 12 会取到 `"fly"`，而 `fly` 从不进 map ⇒ `.at()` 抛。
**如果**服务端下发 12/13（飞行怪），表现就是这只怪静默消失，正好落在用户报的"有的怪物没显示"里。
但我**没有任何一次实测抓到**这个 stance 字节（Release 无日志，见 [[client-build-deploy-and-observability]]），
按「不为不会发生的场景加处理」的约定先不改；线上若再出现"单只怪不显示、其余正常"，
第一条该查的就是这里。

---

## D. 已核实为「不是问题」

### D1 `use_debug_show_packet` 从未开启
上一轮我记着「档 B 还开着、用完要改回 false」。本轮 `SELECT` 只读核实：
`id=222 use_debug_show_rcvd_packet = false`、`id=224 use_debug_show_packet = false`，
`update_time` 均为 `NULL`。→ 那条已授权的单行 UPDATE **至今没执行**，无需回滚，也不用重启报备。

### D2 抓帧「小地图发暗」不是 PrintWindow 的锅
最初怀疑是 PrintWindow 合成了带 alpha 的后缓冲。`_now2.png` 全帧 max=255、底部聊天/状态栏行
（y≥695）有大量 >200 的像素 → 抓帧本身没整体压暗。
（但 PrintWindow **确实可能**整体压暗，见 B5，两件事别混。）
**本条结论仍成立，但当时顺手把暗因归给 A1 是错的，真因见 A4（公告条压顶）。**

### D3 `UIElement::draw_buttons()` 不转发不透明度
`IO/UIElement.cpp:52-57` 形参直接丢弃 → **按钮本体确实不受 A1 影响**，这半句成立。
但当时用它反推「抓帧里 `BtMap` 之所以也暗，是 `Min/c` 被半透明画过后合成出来的」是**错的**：
`BtMap` 暗与 `Min/c` 暗是同一个外因 —— 公告条黑带盖住了 y≤22 这一整条，
按钮和切片一起被压暗（A4）。所以既不用改 A1 之外的渲染代码，也不用改按钮代码，
**要改的是小地图默认 y**。

### D4 商城「点了没反应」
线上已复验：聊天栏出现「商城暂未支持。」→ W2 交付面正常，剩下的属 C5。

---

### D5 背包空格的粉色「✕」底纹 —— **不是缺素材**
`_v_D1.png` 里背包右侧一大片粉色斜叉格子，第一眼看像 `SlotBackground` 没解析出来。
和参考 `背包栏展开.png` 并排一比：**参考完全一样** —— 左侧 5 列是当前页真实格子（米色），
右侧整片就是 v83 原生的占位 ✕ 底纹。W8 的「分类底衬」判为正确，别再当缺陷跟。

### D6 背包 tab 文案与「扩充道具栏」按钮文案 —— **逐字一致**
缩略图里我读成「装备/消耗/**装备**/其他/特殊」和「展开物品栏」，怀疑文案错。
`crop` + 3x 放大（`_v_ztabs.png` / `_v_zbtn.png`）后是
「装备 / 消耗 / **设置** / 其它 / 特殊」和「**扩充道具栏**」，与参考 `_ref_inv.png` 逐字相同。
⇒ 无缺陷，但**结论本身是 B5 第二条坑的产物**，留着当反面教材。

### D7 `i`/`e` 重复按键堆窗 —— **证伪**
`UIStateGame.cpp:245` 是无条件 `emplace<UIItemInventory>`，看着像会堆窗。
实测：背包已开时再按 `i` → 窗口**关闭**（`_v_Z2` 开 → `_v_Z3` 关），说明 `UI::emplace`
复用已有实例并 toggle。⇒ 无缺陷。B3 的 UIShortCut 双开仍要单独验（那条走的是 W2 改过的四按钮路径，
不是同一个 emplace 入口）。

## E. 跨线遗留（不属本 UI 线，但要防遗忘）

| 挂处 | 内容 |
| --- | --- |
| #18 T7 | 真机 handshake 与首批包取证仍 `in_progress` |
| #23 P2 | 字段级 body 对账 S1..S11 未开 |
| #24 T14 | 缺失系统归因清单（商城/皮肤/设置面板等协议线都指到这里） |
| C6 | NPC 对话乱码 T13 |
| #29 | T12b 字宽度量剩余受益面（长中文换行 / 气泡拉伸 / 名牌背景）抓帧验证未完 |

---

## F. 【需用户抉择】键位 action id 表错位（协议级，本线最严重的一条）

**证据链（不靠抓帧，两条硬证据）**
1. 封包：`packet_dump.log` 里 recv `4f 01`（=335/`0x14F` `KEYMAP`），body 451 字节
   = 1 跳过 + 90×(uint8 type + int32 action)，`skip=1` 时 leftover 恰好 0 ⇒ 布局已证。
   解码器：`E:/project/_keymap_decode.py`（下表就是它打印的）。
2. 代码：`IO/UIStateGame.cpp` 里 `grep 'case KeyAction::Id::'` 得到全部 28 个可达分支，
   与 `IO/KeyAction.h:29-56` 的枚举值逐一对齐。

**分界点：id ≤ 20 两端完全一致，从 21 开始整体错位。**
`KeyAction.h` 用的是较新版本编号（21=MAPLENEWS, 22=CASHSHOP, 23=ALLIANCECHAT, 24=注释掉的空位,
25=MANAGELEGION, 26=MEDALS, 27=BOSSPARTY），服务端发的是 v83
（21=TOSPOUSE, 22=MONSTERBOOK, 23=CASHSHOP, 24=TOALLIANCE, 25=PARTYSEARCH, 26=FAMILY, 27=MEDAL）。
另外 `TOSPOUSE` 在本枚举里被排到 **1001**（紧跟 `FAMILIAR=1000`，`KeyAction.h:91-92`），
所以 `UIStateGame.cpp:385` 的 `case TOSPOUSE` 是一条**永远进不去**的死分支。

| 键 | slot | type | 服务端 action | 客户端把它当成 | 有 case? | 线上实际效果 | v83 本该是 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `1`/`2`/`3`/`4` | 2–5 | MENU | 10/12/13/18 | SAY/PARTYCHAT/FRIENDSCHAT/GUILDCHAT | ✓ | 改聊天目标，正常 | 同 |
| **`5`** | 6 | MENU | 24 | （枚举里是注释掉的空位） | ✗ | **死键** | TOALLIANCE 联盟窗 |
| **`6`** | 7 | MENU | 21 | MAPLENEWS | ✗ | **死键** | TOSPOUSE |
| `Q`/`W`/`R`/`P` | 16/17/19/25 | MENU | 8/5/4/19 | QUESTLOG/WORLDMAP/FRIENDS/PARTY | ✓ | 正常 | 同 |
| `E` | 18 | MENU | 0 | EQUIPMENT | ✓ | 装备窗（已实拍 `_v_A1`） | 同 |
| `I` | 23 | MENU | 1 | ITEMS | ✓ | 背包窗（已实拍 `_v_D1`） | 同 |
| `K` | 37 | MENU | 3 | SKILLS | ✓ | 技能窗（已实拍） | 同 |
| `\` | 43 | MENU | 9 | KEYBINDINGS | ✓ | 键位设置窗 | 同 |
| `[` / `]` | 26/27 | MENU | 14/15 | MENU/QUICKSLOTS | ✓ | `toggle_menu()` / `toggle_qs()` | 同 |
| `'` | 40 | MENU | 16 | TOGGLECHAT | ✓ | 正常 | 同 |
| `C`/`M` | 46/50 | MENU | 6/7 | MAPLECHAT/MINIMAP | ✓ | 正常 | 同 |
| **`H`** | 35 | MENU | 11 | WHISPER | ✗ | **死键**（密语没有入口） | 同义，只是没实现 |
| **`G`** | 34 | MENU | 17 | GUILD | ✗ | **死键**（连按两次，区域 bright 40385→39721，与基线 39598 同） | 公会窗 |
| **`L`** | 38 | MENU | 20 | NOTIFIER | ✗ | **死键** | 同 |
| **`O`** | 24 | MENU | 25 | MANAGELEGION | ✗ | **死键** | PARTYSEARCH 组队搜索 |
| **`F`** | 33 | MENU | 26 | MEDALS | ✗ | **死键** | FAMILY 家族 |
| **`;`** | 39 | MENU | 27 | BOSSPARTY | ✓ | **开错窗**：弹出 Boss 队成员列表 | MEDAL 勋章窗 |
| **`` ` ``** | 41 | MENU | 23 | ALLIANCECHAT | ✓ | **走错分支**：只把聊天目标改成联盟频道，**不开商城** | CASHSHOP 商城 |
| **`B`** | 48 | MENU | 22 | CASHSHOP | ✓ | **发错包**：`EnterCashShopPacket` → 服务端换图 | MONSTERBOOK 怪物手册 |
| Ctrl/S/Z/X/Alt/Space | 29/31/44/45/56/57 | FACE | 50–54 | PICKUP/SIT/ATTACK/JUMP/INTERACT | — | **对齐正确**，移动/攻击实测正常 | 同 |
| F1–F7 | 59–65 | FACESLOT | 100–106 | FACE1–7 | — | 表情，正常 | 同 |

净结果：**7 个死键 + 3 个语义错位**，其中 `b`（本该开怪物手册却把角色传送进商城）和
`` ` ``（本该开商城却只改聊天目标，所以「点了没反应」）正是用户最初报的两条。
`case MONSTERCOLLECTION`（`UIStateGame.cpp:427`，W 线里 UIMonsterBook 的入口）**永远不可达**。

### 三种修法（动的是协议枚举 ⇒ 按门禁必须先由用户拍板）

| 方案 | 做法 | 利 | 弊 |
| --- | --- | --- | --- |
| **甲（推荐）** | 只在边界做翻译：`KeymapHandler::handle` 里把服务端 action 过一张 v83→本枚举的映射表再 `add_keymapping`；`KeyAction.h` 与 28 个 `case` 一字不动 | 改动面最小、可回滚、不碰协议定义 | `UIKeyConfig`（键位设置窗）**必须同时**走这张表，否则设置窗里显示的名字和实际绑定不一致 —— 不做就是半成品 |
| 乙 | `KeyAction.h` 整体改成 v83 编号，逐个改 `UIStateGame.cpp` 的 `case` | 一次到位，与服务端语义天然一致 | 改动面大；`KeyAction::Id` 还被 `UIKeyConfig`/`UIStatusBar` 和**存档 `Settings` 里的键位**引用，历史存档 action 值会集体错位 ⇒ 要写迁移 |
| 丙 | 只给 7 个死键补 `case` | 不碰协议 | **治不了** `b` 发错包 / `` ` `` 不开商城 / `;` 开错窗这三条玩家能看见的错，等于只留最严重的症状。不推荐 |

> 顺带一条独立缺陷（**不**属于 id 错位，是逻辑 bug）：
> `UIStateGame::send_key` 在 `focused` 指向「仍在 UI 列表里但 `is_active()==false`」的窗体时，
> 清掉 `focused` 后**直接 `return`**（旧 `IO/UIStateGame.cpp:219-224`），这一次按键既没交给焦点窗、
> 也没落到全局 switch ⇒ 关窗后紧接着按的第一个键被吞，玩家表现为「要按两下」。
> ✅ **本轮已修**（`IO/UIStateGame.cpp:211-226`：清脏焦点后继续往下 dispatch），
> 并且它同时是 **B9**「背包第一次 Esc 不关」的最可能真因 —— 见 B9 的复测配方。
> 残留风险（复测要专门看）：聊天框 / NPC 对话这类窗体在**失焦那一瞬间**也许**本该**吃掉这个键；
> 现在改成继续下发，若出现「关掉 NPC 对话后紧接着的一下聊天快捷键把字打进去了」之类现象，
> 就按窗体加白名单（只对 `is_active()` 的窗吞键，其余一律下发）。

---

## 取证与复现工具索引

| 用途 | 位置 |
| --- | --- |
| 驱动线上客户端（cap/key/hold/move/click/where/**focus**） | `E:/project/_drive.py`、`E:/project/_key.py`（临时，未归档）。**2026-11-01 起 `foreground()` 会校验 `fg == hwnd`**：`focus` 子命令单独试抢前台；抢不到时 `key/click/hold` 直接 `ABORT` + 退出码 3，不再「假装按过」（B5 第四条坑） |
| **后台投递按键（不抢前台）** | `E:/project/_postkey.py`：`PostMessageW(WM_KEYDOWN/WM_KEYUP)` 直接进 SDL 消息队列，用于别的程序占着前台时驱动客户端；`python _postkey.py i` / `esc` |
| **启动 + 自动登录 + 逐键探活（绝不杀进程）** | `E:/project/_boot.py`（PowerShell `Start-Process` → 轮询 `_drive.py where` → 等 HUD 亮度 → 逐键 `alive=`） |
| **解 KEYMAP 封包（F 段那张表的来源）** | `E:/project/_keymap_decode.py`（取 `packet_dump.log` 首条 `4f 01`，试 skip 1/2/3，打印 slot→键→type→action→枚举名） |
| 批量按键扫描（**慎用**，见 B5 第三条：escape 污染 oracle） | `E:/project/_keysweep.py` |
| 抓帧 | `python E:/project/_cap_screen.py <out.png>`（客户区 1280×720；click 坐标 = 抓帧坐标 + (8,31)） |
| 抓帧亮度自检（整帧 max） | [../tools/bright.py](../tools/bright.py) |
| 区域颜色直方图 / 目标色定位 / 条带几何 | [../tools/probe.py](../tools/probe.py)、[../tools/runs.py](../tools/runs.py) |
| 裁切放大 / 上下对照拼装 | [../tools/cmp.py](../tools/cmp.py)（`crop` / `stack`，`REF`/`CUR` 已指向桌面两组对照图） |
| 以上用法与两条硬规矩 | [../tools/README.md](../tools/README.md) |
| IMG 节点只读取证 | `python tools/nx_inspect.py list\|image E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx …`（中文须加 `PYTHONIOENCODING=utf-8`） |
| 参考帧逐窗裁片 | [../evidence/refcrops/](../evidence/refcrops/)（skillwin / statswin / minimap / bottombar / keycaps / inventory / equipslot / chatbar / questnotice） |
| 本轮小地图几何/颜色证据 | [../evidence/w11-map/pixels-alpha.md](../evidence/w11-map/pixels-alpha.md)、`w11-map/pngs/` |
