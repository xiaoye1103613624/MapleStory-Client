# 怪物手册全链路核查（2026-11-17）

> 需求：手册功能不完善、UI 错位、左侧白色标签可点击切换、是否对接服务端数据；结合 angel + IMG 检查全链路。
> 味道：🔴 华为 · RCA 根因分析 + 蓝军自攻击。本轮为**只读取证 + 结论**，未改业务代码。

## 一、结论速览（逐条，带行号）

| 疑问 | 结论 | 证据（行号级） |
| --- | --- | --- |
| 是否对接服务端数据？ | **对接了，且是双通道**：登录/进图全量 + 拾取增量 | 见 §2 |
| "卡片:0 / 统计空"是不是没接数据？ | 不能据此判定。本端手册统计**从不读服务端统计值**，全部本地现算；截图文案与本端 locale 不符，**高度疑似参考端（非本端）截图**，禁止当本端验收 | 见 §3 |
| 左白色标签能否点击切换？ | **能**。LeftTab 0..8 已绑按钮，`button_pressed` 有筛选分支；但 `BtSearch` 是空实现 | 见 §4 |
| UI 错位？ | IMG 资源与代码坐标已三方对齐（D2 修复），真实错位需本端实机复现才能定性；截图来源未确认 | 见 §5 |

## 二、数据链路（服务端 → 客户端，已闭环）

### 2.1 登录/进图全量下发
- 服务端 `PacketCreator.addMonsterBookInfo()`（`BeiDou-Server_s9/gms-server/src/main/java/org/gms/util/PacketCreator.java:607-617`）：
  `writeInt(cover)` + `writeByte(0)` + `writeShort(size)` + 循环 `writeShort(cardid%10000) + writeByte(level)`；
  由 `getCharStats` 类封包在 `:249` 追加。
- 客户端 `CharacterParser::parse_monsterbook()`（`Net/Handlers/Helpers/CharacterParser.cpp:316-332`）字节序逐字段对齐；
  调用点：`Net/Handlers/SetFieldHandlers.cpp:374`（进图）+ `Net/Handlers/Helpers/CashShopParser.cpp:54`。
  **注：先前 grep 误报"无调用点"已自我纠正——是检索过滤把 .cpp 排除了，链路实际是通的。**

### 2.2 拾取增量下发
- 服务端 `PacketCreator.addCard(bool full,int cardid,int level)`（`:6255-6261`）= `SendOpcode.MONSTER_BOOK_SET_CARD(0x53)`；
  cover 变更 `changeCover()`（`:6276-6280`）= `MONSTER_BOOK_SET_COVER(0x54)`。
- 客户端 opcode 注册：`Net/PacketSwitch.cpp:393-394`(83/84 枚举) + `:550-551`(emplace handler)；
  `MonsterBookHandlers.cpp:47-75` 解析并 `add_card(cardid%10000, level)` / `set_cover()`。
- 刷新：`refresh_book_if_open()`（`MonsterBookHandlers.cpp:32-43`）在实例存在时 `reload()`，解决"关窗期间拾取的卡不灌入单例缓存"历史问题。

### 2.3 模型
`Character/MonsterBook.{h,cpp}`：`std::map<int16_t,int8_t> cards` + `int32_t cover`，key = `cardid%10000`。与服务端一致。

## 三、统计显示：本端是"本地现算"，截图来源高度存疑

- 本端手册统计在 `UIMonsterBook.cpp::refresh_info()`（`:304-320`）：
  `total=all_cards.size()`、`special=count(level>=2)`、`info_line[..]` 走 locale `monsterbook.label_cards/info_id/cover_id`。
  **完全没有读取**服务端 `charInfo()` 里的 `bookLevel/normalCard/specialCard/totalCards`
  （服务端 `PacketCreator.java:2863,2917-2920`，那套数值是给"角色信息窗"用的）。
- 本端 `locale/zh-CN.ini` 手册键：`卡片：{}` / `卡片编号：{}` / `卡片等级：{}` / `Cover id={}` / `怪物编号 {}`。
- 用户截图文案：`怪物手册等级 / 总登记卡片数量 / 特殊卡片数量 / 一般卡片数量 / 手册书签 / Covered ID`（含"下月：1""卡片编号：1"错位值）。
  **这些文案在本端 locale 中不存在**。按用户 2026-11-17 当面纠错口径：截图很可能是**参考端/原生端**，不是本端修复后画面。
  → 不得用该截图的"卡片:0/错位"作为本端缺陷或验收证据。需用户确认截图来源端后再定性。

## 四、左侧白色标签（LeftTab）交互

- 绑定：`UIMonsterBook.cpp:176-184`，`LeftTab 0..8` → `BtNote0..8`，位置 `(-7, 131+20i)`。
- 状态灯：`sync_note_indicators()`（`:232-252`）按持有分类点亮，Angel `UpdateCheckList @0x867be6` 对齐。
- 点击：`button_pressed()`（`:554-568`）→ `set_note(i, turn_on)` → `apply_note_filter()` 重筛卡片视图。
  **结论：可点击、会切换筛选页**（不是纯装饰）。
- 缺口：`BtSearch` 分支（`:533-537`）为空实现——搜索按钮点了没反应，这是真实"功能不完善"。

## 五、UI 资源核对（orange-wz MCP 实测，全绿）

- 用 128B `WzAESConstant.DEFAULT_KEY`（`tools/_gms_key_128.b64`）经 `http://127.0.0.1:10012/mcp`
  `initialize → load_files(exclusive:false) → list_children → unload_all`，证据：`evidence/orange_wz_verify.json`。
- `Data/UI/UIWindow.img/MonsterBook` 解析成功；节点齐全：
  `backgrnd/infoPage/cardSlot/select/cover/fullMark/arrowLeft/arrowRight/BtClose/BtSearch`（CANVAS/LIST），
  `LeftTab` = 0..8 九个、`RightTab` = 0..3 四个、`icon` = 0..7 八个。
- 与代码绑定（`UIMonsterBook.cpp:140-191`）数量一致 → **资源非错位根因**；此前 D2 修复的坐标是
  orange-wz 尺寸 + Angel IDA 立即数 + 原生截图三方对齐结果（见 `UIMonsterBook.h:4-18` 头注）。
- Angel 对照证据：`docs/features/keyconfig-13key-ui-audit-2026-11/evidence/ida_monsterbook_ctor.json`（ctor/建钮立即数）、
  `docs/features/ida-v83-083-v83-i64-inventory-2026-11/notes/r17-monsterbook-facts.md`（两端 exe MD5 相同、类索引一致）。

## 六、蓝军自攻击（未被证据推翻的残余风险）

1. **统计口径不一致**：本端手册把"卡片等级≥2"当特殊卡（`:309-311`），服务端 `specialCard` 的判据是 `cardid/1000>=2388`（`MonsterBook.java:64`）。两侧"特殊卡"定义不同——若将来把服务端统计接进手册，数值会与本端现算冲突。属**潜在不一致**，当前未暴露（手册不读服务端统计）。
2. **R 键 reload 可用但无 UI 反馈**（`:613-616`），S 键搜索空实现（`:617-619`）。
3. **截图来源未确认前，任何"错位/空壳"结论都不成立**——这是本节最重要的一条。

## 七、待用户拍板（阻塞项，未擅自改码）

1. **截图来源端**：这张图是参考端（原生/Angel）还是本端修复后的画面？——决定"错位/0"是否算本端缺陷。
2. **范围**：本轮只做核查，若确认要修，建议作为独立功能走 Plan：
   - (a) 手册统计改为读服务端 `charInfo` 的 bookLevel/normal/special/total（需新增协议解析，改公共协议 → 必须用户确认）；
   - (b) 补全 `BtSearch` 搜索实现（编号/名称检索）；
   - (c) 若本端实机确有错位，按原生截图 + IMG 重新定位（需用户提供本端截图）。
3. **是否授权改公共协议 opcode / 存档结构**：若选 (a)，涉及协议字段，按 skill 门禁须先确认。

---

# 八、修复实施记录（2026-11-17 · 用户确认截图为本端后）

> 用户拍板「图示本端」，§3 的"来源存疑"作废，截图错位/空壳按真缺陷修。
> 味道：🔴 华为 · RCA + 蓝军。以下逐条给代码行号级结论。

## 8.1 根因（三条，RCA）

| 现象 | 根因 | 证据 |
| --- | --- | --- |
| 统计区"乱码式错位" | `refresh_info()` 用 `info_line[]` 画了带 label 的重复文字（`卡片等级：N` / `卡片编号：N` / `Cover id=N`），而 `infoPage.png` 背景已印死 5 个原生标签（怪物手册等级/总登记/特殊/一般/书签），两层文字同区叠加；且 info_line 是 LEFT 对齐，数值压在标签上 | 裁图 crop（左页文字重叠）+ `evidence/img/infoPage.png` |
| "手册等级"恒 0/1 | 旧逻辑 `mb.get_cover()>0?1:0` 把"有没有封面"当等级，语义错 | 旧 `UIMonsterBook.cpp:316-317` |
| 左标签点了没反应 | `apply_note_filter()` 中 `selected_index=-1`（统计页）时筛选后不自动选中，视图停在统计页 | 旧 `UIMonsterBook.cpp:274-277` |
| BtSearch 空实现 | `:533-537` case 体为空 | 旧源码 |

## 8.2 修复（逐条，行号以修复后为准）

1. **统计数值改纯数值 + RIGHT 对齐**：`UIMonsterBook.h` 新增 `Text stats_value[5]`（RIGHT）；
   `UIMonsterBook.cpp` 构造函数初始化；`refresh_info()` 统计分支只写纯数字到 stats_value，
   不再重复画 label；`draw()` 统计分支用 `org+(198, 113/166/197/218/254)` 画在背景标签右侧同一行。
   → 消除叠字。
2. **手册等级本地现算**：复刻服务端 `MonsterBook.calculateLevel` 的 collectionExp 公式
   （`exp_to_next += level*10`），不再用 cover 有无代替。未改公共协议（手册包不含 bookLevel）。
3. **左标签切换可见**：`apply_note_filter()` 增加 `selected_index<0 && !cards.empty() && active_note>=0 → selected_index=0`，
   从统计页点有卡的签会自动进卡片页。`active_note>=0` 限定用户主动筛选，避免无卡初始态误选中。
4. **补全 BtSearch**：软键盘输入 → 纯数字按 card id%10000 匹配、否则按 `mob_name_of(mob_id_for_card())` 模糊匹配 → 清筛选、选中命中卡、状态栏反馈。新增 locale 键 `monsterbook.search_hint/found`，并把 `not_found` 本地化为中文。
5. **编码**：真源 `locale/{zh-CN,en-US}.ini` + live 副本四处统一 UTF-8 无 BOM 写入，strict decode 通过、fffd=0。

## 8.3 验证闭环（证据）

- 隔离构建：MSBuild Release/x64，`BUILD_EXITCODE=0`，`error C`=0；`UIMonsterBook.obj` mtime 19:52 > cpp 19:49（入链判据）；
  产物 `monsterbook_out_v2/MapleStory.exe` SHA `47bd997e992589cff7b96939b28fb433c2bc6c3d8dc25aeb800dcb92ffdc0769`。
- 部署：kill 客户端 → 备份 live exe → 覆盖 `MapleStory.exe` + `MapleStory_pua24.exe` → **实时 SHA 三处一致**；交付前二次复核 SHA 未回滚。
- 日志：改动文件无残留 DBG 调试标记。

## 8.4 未验收 / 诚实声明（红线）

- **真机点验未做**：统计数值是否精确落在背景标签右侧、搜索软键盘交互、左标签实机切换手感——需在客户端实机打开手册目视确认。
  本轮是编译级 + 静态证据闭环，**不等于真机功能全链路验收**。
- 服务端"特殊卡"判据是 `cardid/1000>=2388`，本端手册仍按 `level>=2` 现算；两侧口径未统一（本轮未改协议，保守）。
- 若要让手册直接用服务端 bookLevel/normal/special/total（与角色信息窗一致），需新增协议解析，属公共协议改动，另行授权。

---

# 九、真机对照复核（2026-11-17 20:31 用户追查）

> 用户问：① 统计位置对照 IDA 修了没？② 红框放大镜还是弹软键盘？
> 提供 3 张正确端 + 4 张当前端实机图。live SHA `47bd997e...`（反回滚复核未变）。

## 9.1 已证实（逐条，行号级）

| 项 | 结论 | 证据 |
| --- | --- | --- |
| 统计数值位置 | **修对了**：正确端图1 与当前端图2 数值均在标签右侧同一行，视觉一致 | 对照截图 + `UIMonsterBook.cpp:494-498` |
| 红框放大镜弹软键盘 | **是**，当前端点它弹 SOFT KEYBOARD，提示"输入卡片编号或怪物名" | 当前端图4 + `UIMonsterBook.cpp:565` BtSearch 分支 + locale 键 |
| live SHA 未回滚 | 是，`47bd997e...` | 实时 sha256 |

## 9.2 IDA 核对的硬事实（`http://127.0.0.1:13337/mcp`，input MD5 cbe5ccba 同 Angel）

- `CUIMonsterBook` / `CMonsterBookMan` 全部具名方法（13 个，见 `func_query`）：
  `LoadBook/LoadCard/GetCard/LoadStringA/LoadBookIcon/GetString/GetAt/Clear/UpdateUI/LoadMobInfo/LoadMobAction/GetCard/UpdateCheckList`
  —— **没有独立的 Search handler、没有软键盘调用**；`find_regex` 全库只有 `NewYearsCard/BtSearch`，MonsterBook 无搜索字符串。
  → 结论：上一轮"补 BtSearch 软键盘搜索"**无 IDA 证据支撑，是自创实现**，与原生口径不符。
- `LoadBookIcon @0x685857` 只读 `UIWindow.img/MonsterBook/icon`（SP_2584 + SP_2356_ICON），逐帧加载 icon/0..7 — 与本端 `cell_icon`（`UIMonsterBook.cpp:154-157`）同源；orange-wz 实测 icon/0..7 均为有效 22×27 PNG。

## 9.3 未解决 / 待决策（诚实声明）

1. **当前端卡片页（图3 XiaoYe_20-30-08）5×5 全空、右页无怪名/HP/MP，但底部显示 1/1、统计页显示有 1 张卡** —— 这是真 bug，尚未定位到根因（已排除：资源缺失 icon/0..7 有效、cell_icon 加载代码存在、Texture 构造对 CANVAS 有效）。`vision` 曾推断"正确端显示真实怪物脸/未拥有也显示"，但那是纯视觉模型猜测，无 IDA/IMG 支撑，**不予采信**。
2. 需在不猜的前提下继续定位：建议在 `UIMonsterBook::draw` 卡片页分支加临时日志（cell_icon.size / cards.size / i），或真机设断点，确认是 `cards` 空还是 `cell_icon` 纹理空。
3. **软键盘搜索是否保留**：IDA 无支撑，需用户拍板——(a) 保留本端自创的软键盘搜索；(b) 按原生移除/禁用（原生 BtSearch 行为未从 IDA 定位到）。

---

# 十、空格子真因定位与正式修复（2026-11-17 21:49 收口）

## 10.1 调试垫片证据（已删）

按用户 b 方案加 `mb_trace.log` 垫片（打印 this/all/note/cards/cell_icons/selected/reload#）。增强版日志关键行：

```
this=...D580 stats=0 cards=1 all=1 note=-1 selected=0     ← 全量：有卡，正常
this=...D580 stats=1 cards=0 all=1 note=2 selected=-1      ← 点签2：筛掉
this=...D580 stats=1 cards=0 all=1 note=5/4/1/7 ...        ← 依次点签，全筛掉
```

分析（RCA）：
- this 全程同一指针 → **排除多实例**；all=1 恒定 → **排除 reload 风暴/模型丢数据**；cell_icons=8 恒定 → **排除资源/纹理缺失**。
- 唯一卡 `cardid=2380006`（DBX 只读查 beidou.monsterbook），客户端 key=`2380006%10000=6`，`note_category_of(6)=6/1000=0`。
  IDA `LoadCard @0x6848a6` 实证分类公式 `v11=(完整id-2380000)/1000=(2380006-2380000)/1000=0` → **本端分类与原生一致，算法没错**。
- 用户点的 note=2/5/4/1/7 都不是该卡的 0 → 卡片被**正确筛掉**。这不是分类 bug。

## 10.2 真 bug 与修复

真 bug：`book_stats_mode` 旧判定 `selected_index<0 && active_infotab==0` 没排除便签筛选态。
点一个"没有卡"的便签 → cards=0 → selected_index=-1 → 判定成立 → 跳到基本情报统计页，
用户观感是"切到便签后卡片全丢"。原生行为应留在卡片页显示空 5×5 格。

修复：`UIMonsterBook.cpp:487`
```cpp
const bool book_stats_mode = (selected_index < 0 && active_note < 0 && active_infotab == 0);
```
加 `active_note<0` 条件：便签筛选态（含筛出 0 张）保持卡片页，不跳统计页。

## 10.3 收尾

- 调试垫片（mb_trace 函数/两处调用/reload 计数/`<fstream>`）全部删除，`grep DBG-MB` 零残留。
- 正式构建 `error C`=0，obj 21:46 > cpp 21:43；部署实时 SHA `91f4451c...` 三处一致（MapleStory.exe + MapleStory_pua24.exe + 产物）。
- 按用户拍板 b：BtSearch 软键盘搜索已移除（IDA 无支撑），改回空实现。
- 未验收：真机再点验（便签切空分类应显示空格子而非统计页；放大镜不再弹软键盘）——如实声明。

## 10.4 部署踩坑（记档）

MSYS 下 `taskkill //F //IM MapleStory.exe` 不生效（双斜杠转义），exe 被占用导致 cp 报 Permission denied、
SHA 未变（假部署）。正确做法：powershell 按 Path 过滤取 PID → `taskkill /F /PID <pid>` → 再覆盖 → 实时 SHA 复核。

---

# 十一、手册 5 项功能一次性补齐（2026-11-17 22:34 · 用户"一次改完"指令）

> 用户当面批评前几轮挤牙膏，要求以点到面、一次性把手册所有逻辑改完。
> 本轮通读全链路 + IDA 核对后，一次性补齐以下 5 点。

## 11.1 根因（框架级，之前一直没碰）

`UIElement::send_cursor`（:304/:326）只处理 NORMAL/MOUSEOVER 两态，点击后 `set_state(button_pressed(返回值))`；
`remove_cursor`（:290）与移出（:342）**只重置 MOUSEOVER，不碰 PRESSED/DISABLED**。
→ 常驻激活态必须用 `button_pressed` 返回 `PRESSED` + 同步函数（照 `UISkillBook::change_tab:998-1004` 范式）。
之前 InfoTab/Note 分支返回 NORMAL，激活态一帧都留不住——这才是"右标签/便签没激活效果"的真因，不是没实现。

## 11.2 五项修复（行号级）

| # | 功能 | 修改 | 行号 |
| --- | --- | --- | --- |
| 1 | 打开有首页 | 点"基本情报"(infotab 0) 清筛选+清选中→统计首页 | `button_pressed` InfoTab 分支 |
| 2 | 便签样式变化+内容变化 | `sync_note_indicators` 三态：当前签 PRESSED、有卡 NORMAL、无卡 DISABLED；`button_pressed` 返 PRESSED | :237-266 / :628-636 |
| 3 | 右标签逻辑+激活 | 新增 `sync_infotab_indicators()`（旧签 NORMAL+当前 PRESSED，对齐 Angel `UpdateUI@0x862527` 设 3 状态）；ctor 初始同步；返 PRESSED | :268-281 / :203 / :645-653 |
| 4 | 分页激活/未激活态 | `refresh_info` 开头设 4 个箭头态：首页禁上一页、末页禁下一页、单页全禁 | :325-339 |
| 5 | 满级卡高亮+右侧变化 | level>=5（服务端 addCard 上限 5）在格子右上角画 fullMark 角标；点击右页变化原有 `cell_at→select_card_by_index→refresh_info` 保留 | :570 |

## 11.3 Angel 对照结论（IDA 13337，input MD5 cbe5ccba 同 Angel）

- `UpdateCheckList@0x867be6`：逐 i<9 设勾选态 → 便签 2 有原生支撑 ✓
- `UpdateUI@0x862527`：开头 `for(i=3){*(this+2836+2i)=1}` 设 3 个 RightTab 状态 → 右标签激活态原生确有 ✓
- `LoadMobInfo@0x866c2c`(0xb1d) + `GetCard@0x867a2f` + `LoadBookIcon@0x685857` → 卡片详情/图标原生确有 ✓
- 分页态：ctor 建 6 箭头（IDA 立即数已取证），本端绑同帧；激活态以 NORMAL/DISABLED 表达 ✓
- 首页：infoPage 画布 + UpdateUI 无选中卡统计 → 原生确有 ✓
→ 用户列的 5 项 Angel 全有；本端本轮全部补齐。

## 11.4 验证与边界

- 构建 `error C`=0，obj 22:31 > cpp 22:28；部署实时 SHA 三处一致 `66102e84...`（按 PID 杀进程后覆盖）。
- 未验收：真机点验 5 项（尤其便签/右标签 PRESSED 常驻态、满级角标位置）——如实声明，不拿编译通过冒充功能验收。
- 交互说明：便签/右标签为 PRESSED 常驻态后，点当前签不再直接取消（与 UISkillBook tab 一致）；"基本情报"tab 负责清筛选回全量，等价提供取消路径。
