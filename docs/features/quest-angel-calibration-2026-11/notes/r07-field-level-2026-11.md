# 第十一面 · Angel 083 任务窗字段级还原 (r07)

> 取证时间：2026-11-21。性质：**只读**逆向取证（IDA MCP 13339 + orange-wz 10012）+ 客户端源码字段级落地。
> 未改协议 / 服务端 / WZ / IMG；不构建、不部署、不 commit。
> 适用范围：D4「新做 Angel IDA 字段级还原」；只改客户端 `UIQuestLog` / `QuestData` / i18n。

## 0. 结论（真 IDA 一手 vs 合理还原）

| 维 | 结论 |
| --- | --- |
| **字段级坐标/成员** | ✅ **真 IDA 一手**：Angel 带符号 IDB（`E:/MXD/反编译IDA/v83/`Angel.idb`，IDA MCP 13339 仍在线）`type_inspect` 直接读出 `QuestInfo`(16 字段) / `CUIQuestInfo`(42 字段) / `CUIQuestInfoDetail`(6588B) / `CT_INFO`(17 字段) / `MedalQuestInfo`(19 字段) 的**成员名+偏移+类型**；`decompile` 读出构造函数与 `OnTabChanged` 逐字伪代码。 |
| **IMG 布局定位** | ✅ **真 orange-wz 一手**：128B key + `autoParse:true` 成功解析 `UIWindow.img/Quest`（backgrnd 245×396 origin(0,0)、backgrnd2 305×396 origin(0,0)、Tab enabled 50/50/27×15、disabled 50/49/27×14、Gauge/frame 163×14 origin(81,7)、gauge/spot 1×11 origin(0,5)、底带钮 17px 高）。 |
| **合理还原（非一手坐标）** | 客户端是 Journey 系 `UIQuestLog`（无 CWnd/CLayoutMan 的逐像素坐标），不能照搬 IDA 的 `this+off`；只把**语义**（详见 §2）映射进现有实现。绝对像素仍以 IMG origin + `Text::y_for_ink_top` 为唯一真源，未编造原生字段坐标。 |
| **未取到** | `CUIQuestInfoDetail` 内部 3208B 匿名结构（`___u1`）IDA 未命名成员，不强解；勋章窗族 `CUIMedalQuestInfo` 的独立版式无真机截图，仍 ⏳（r05 L4）。 |

## 1. 取证走廊与 key 规格（避免历史越界重演）

- **IDA MCP**：`http://127.0.0.1:13339/mcp`，`initialize` + `tools/call`，66 tools；本面用到 `search_structs` / `type_inspect` / `decompile`。IDB = Angel 083 带符号（r01 已证 153 funcs/34 classes）。
- **orange-wz MCP**：`http://127.0.0.1:10012/mcp`，29 tools，`load_files(exclusive:false)` → `get_node_tree_json(autoParse:true)` → `unload_all`。
- **key 实测（本面新增证据，覆盖 r03 旧口径）**：
  - `tools/_gms_key_128.b64`（172 char b64，head `EwAAAFIAAAAqAAAAWwAAAAgA`，128B `WzAESConstant.DEFAULT_KEY`）= `load_files` 成功、`get_node_tree_json(autoParse:true)` 返回真实节点树。
  - `tools/_gms_key.b64`（65536B `key_gms` 全表，r03 所用）= `get_node_tree_json` 返回 `IMG 解析失败: UIWindow.img`（GBK 原文乱码），与 `AGENTS.md` §3「规格错配才是真因」一致。
  - IV 两条路径同为 `TSPHKw==`（GMS 4B）。
  - **autoParse 是必需项**：128B key 下 `autoParse:false`/省略时 `list_children`/`search_node` 返回「找不到节点」，只有 `autoParse:true` 才解析出 `Quest/*` 子树。

## 2. 真 IDA 一手字段清单（逐字，来自 `type_inspect`）

### 2.1 `QuestInfo`（size 64，16 字段）— 单条任务的原生语义
```
nQuestID @0x0  int            // 任务 id
nIconType @0x4  int           // 行首图标类型（对应 Quest/icon0..icon9）
nCategory @0x8  int           // 一级分类
nContentsCategory @0xc int    // 内容分类（故事/一般/…）
sName @0x10 ZXString<char>    // 任务名（原生确有独立名字段）
sLevelLimit @0x14 ZXString<char> // 等级限制文案（非裸数字）
asDesc @0x18 ZArray<ZXString<char>> // 分状态描述页
aCTHeight @0x1c / aaCT @0x20 // 富文本行（CT_INFO 二维）
bTimeLimitQuest @0x24 int
nCompletedCount_OfSeries @0x28 int // 系列完成数（连锁进度）
nSortKey @0x2c int
sPrefix @0x30 ZXString<char>  // 前缀串（如 [职业]/[系列]）
asSummary @0x34 / aCTSummary_Height @0x38 / aaCTSummary @0x3c // 摘要富文本页
```
→ 关键：**原生描述/摘要是 `ZXString` 富文本数组，等级限制是**字符串** `sLevelLimit`，不是裸 int；名字是 `sName`。**这正是本批「去掉自造裸数字字段、改可读名/提示」的一手依据。**

### 2.2 `CUIQuestInfo`（size 3492，42 字段）— 任务窗状态机
```
baseclass_0 @0x0 CUIWnd(2916B)
m_pSBList @0xb6c ZRef<CCtrlScrollBar>
m_nTab @0xb74 int            // 当前分类页
m_nSelect @0xb78 int         // 当前选中 (-1 = 无)
m_nToggleTime @0xb7c int
m_bDetailOpen @0xb80 int     // 详情展开
m_bDetailShown @0xb84 int    // 详情可见
m_nScrollPos @0xb88 int
m_pProp @0xb8c IWzProperty*
m_pCanvasIcon[4] @0xb90      // 4 图标层
m_pCanvasPerform / SearchBg / CompHeader / PartyHeader / PartyBg / PartySelect / PartyTip @0xba0..0xbb8
m_pFontBack[2][2] @0xbbc / m_apFont @0xbcc / m_apFontCount[2] @0xbd0
m_apLayerIcon @0xbd8
m_aaQuestInfo[4] @0xbdc      // 4 分类页各自的 QuestInfo 数组
m_aaQuestInfoForSearch[4] @0xbec
m_pQuestInfoDetail @0xbfc ZRef<CUIQuestInfoDetail>
m_apBtMin[4] @0xc04 / m_apBtMax[4] @0xc14 // 分组折叠钮
m_pCBRegisterAlarm @0xc24 / m_pEditSearch @0xc28 / m_pBtSearch @0xc30
m_pCmbContentsCategory @0xc38 / m_apBtContentNavi[2] @0xc40
m_pBtStoryQuest @0xc50 / m_nContentPage @0xc58
m_pBtShowOnlyWorthyQuests @0xc5c / m_pBtShowAllQuests @0xc64
m_pBtMyLocation @0xc6c / m_pBtAllLocation @0xc74  // 我的位置/全部位置（对应当前端 MARKNPC 等）
m_mCountInCategory @0xc7c / m_nTotalCount @0xc94
m_bShowSubQuestForZero @0xc98 / m_lm @0xc9c CLayoutMan
```

### 2.3 `CUIQuestInfoDetail`（size 6588） / `CT_INFO`（size 68，17 字段）
```
CUIQuestInfoDetail: baseclass_0 CWnd @0x0；___u1(3208B 未命名) @0xa8；gapa7 @0xd30
CT_INFO: nType @0x0 / nItemNo @0x4 / nForceNpc @0x8 / nLine @0xc / pFont @0x10 /
         sText @0x14 / pIcon @0x18 / nLeft @0x1c / nTop @0x20 / nWidth @0x24 / nHeight @0x28 /
         nSelect @0x2c / nUnderLine @0x30 / bLineChange @0x34 / nFuncCode @0x38 /
         bReward @0x3c / bItemUnrelease @0x40
```
→ 原生详情正文是**带位置 `(nLeft,nTop,nWidth,nHeight)` 的可点富文本块**（`nFuncCode`/`bReward`/`nForceNpc`），当前端用 `Text` 自动折行等价承载，不编造逐块坐标。

### 2.4 `MedalQuestInfo`（size 80，19 字段）
`nQuestID` / `nMedalCategory` / `nMedalState` / `sName` / `sLevelLimit` / `asDesc` / `nViewMedalID` / `sMedalName` / `nTop` / `ftCompleteTime` / `aResignedTogetherQuest` / `asSummary` … → 勋章窗族**另有 category/state/medal 字段**，当前端合并窗无此数据，按 r05 L4 挂起不伪造。

### 2.5 伪代码一手（逐字摘录）
- `CUIQuestInfo::CUIQuestInfo @0x87e8f3`：`CUIWnd::CUIWnd(this,6,3,227,6,1,0,0)`；`*(this+369)=-1`（m_nSelect 初值 -1）；`*(this+398)=31`、`*(this+400)=100`、`*(this+401)=24`；`CWnd::CreateWnd(..., nWidth:245, nHeight:396, z:10, bScreenCoord:1)`。
- `CUIQuestInfo::OnTabChanged @0x87fd4f`：`*(this+369)=-1; *(this+368)=a2; LoadData(); SetScrollBar();` — 切页先清选中，与当前端 `change_tab()` 的 `selected_id=-1` 一致（注释已引）。
- `CUIMedalQuestInfoDetail::CUIMedalQuestInfoDetail @0x83d255`（来自 evidence decompile）：`CreateWnd(...,305,373,z:10,bScreenCoord:1)`。
- `CUIMedalQuestInfo::OnCreate @0x84056b`：加载 `SP_3232_QUEST_QUESTINFOIMG`、`SP_5168/5169 UIWindow.img/Title/Icon1/Icon2`、`SP_5162/5164 Title/Backgrnd2/4`、`SP_5170/5171 Title/Tab_Disabled/Enabled`；5 页签循环（`for j<5`）；滚动条 `CCtrlScrollBar` at (239,48)；`SetTab` 按当前页。→ 083 原生问询窗是**5 页签 + 搜索框 + 富文本**，当前端 3 页签合并窗属结构简化，非缺字段。

## 3. orange-wz 一手 IMG 定位（`UIWindow.img/Quest`，128B key + autoParse:true）

| 节点 | 尺寸 | origin | 当前端用法 / 校正 |
| --- | --- | --- | --- |
| `backgrnd` | 245×396 | (0,0) | 左板；`bg_left` 尺寸与窗口尺寸与之吻合 ✅ |
| `backgrnd2` | 305×396 | (0,0) | 右板；`right_width=305` ✅ |
| `backgrnd5` | 266×117 | (0,0) | 蓝横幅贴图；`detail_area.top=124` 在其下 ✅ |
| `Tab/enabled` 0/1/2 | 50×15 / 50×15 / 27×15 | (0,0) | 字图水平居中在分类格；**不是** 64/65/65 格宽；格宽来自 `tab_w`（自绘底） |
| `Tab/disabled` 0/1/2 | 50×14 / 49×14 / 27×14 | (0,0) | 同上 |
| `Gauge/frame` | 163×14 | **(81,7)** | 代码用 `gauge_frame.get_origin()` 动态加回 ✅ |
| `Gauge/gauge`,`spot` | 1×11 | (0,5) | 拉伸/收尾；垂直居中用 frame_h ✅ |
| `BtDetail`/`BtGiveup`/`BtOK`/`BtNo` 帧0 | 57×17 | — | 底带 17px 高 → `BAND_Y=370` 与 `BAND_TOP=367` 留 3px 居中 ✅ |
| `BtMarkNpc` 帧0 | 81×17 | — | 同上 |
| `BtAlert` 帧0 | 56×16 | — | 16px（略矮，仍同带） |

> `Quest/list` 节点在 live `UIWindow.img` 中不存在（`Quest` 直接子节点列表已核）；当前端 `quest["list"]` 兼容分支保留但无效，不影响。

## 4. 本批实现（字段语义 → 客户端落地）

1. **布局居中（按 `Text::y_for_ink_top` + IMG origin 校正）**：分类字图/行文字/底带按钮统一从「截图实测硬编码」改为「按 IMG 尺寸与墨迹顶反解」。详见 `UIQuestLog.cpp` 改动注释。
2. **去自造裸数字字段**：删 `quest.detail.id` / `quest.detail.npc` / `quest.detail.pre` 的 `std::to_string(裸id)`；改为 NPC 可读名（`String/Npc.img/<id>/name`，本包已证存在）、任务名（`QuestData::get_name()`）、前置任务用 `QuestData` 已有 name 解析；摘要空给可读占位。
3. **字段级语义落地**：详情态字段顺序与原生 `QuestInfo` 对齐（名字→等级限制串→区域→NPC 名→前置），等级范围仍从 `Check.img` 的 min/max 读，但文案用 `quest.detail.level_range`（非裸字段自造）。

## 5. 限制与未闭环

- **未一手取到**：`CUIQuestInfoDetail` 内部 3208B 匿名结构成员名；勋章窗独立版式；原生每像素坐标（当前端架构不同，不能 1:1 抄 `this+off`）。
- **不构建不部署**：本批只改源码 + 文档 + i18n，未跑编译/启动，需 parent 后续构建验收。
- **095 禁引**：全程只用 083 Angel IDB（13339）与 live xy 客户端 IMG，未引 095。
- **r03 口径刷新**：本面 orange-wz 内容层**已恢复**（128B key + autoParse:true），r03 的「内容层 12 次 IMG 解析失败 → .nx 回退」是 65536B key 错配导致的历史结论，按 `AGENTS.md` §3 由本面新证据覆盖；r07 不改 r03 原文，仅在此声明覆盖关系。
