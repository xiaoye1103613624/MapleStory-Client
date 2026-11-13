# r01 — Angel 083 原生状态栏 HUD 一手事实（观察层）

> 性质：只读取证，逐字 Hex-Rays 伪代码。来源 = `evidence/angel-hud-decompile-full.json`（247 函数，其中 10 个超大函数经 IDA MCP `/output/<uuid>.json` 端点补全、237 个嵌套 JSON 字符串已归一化）。
> 采集脚本：`tools/ida_angel_hud_ea.py`（EA 索引）、`tools/ida_angel_hud_decompile.py`（批量反编译）、`tools/ida_angel_hud_facts.py`（语义抽取 → `evidence/angel-hud-facts.md`）。
> 参照基准 083/v83；禁止引 095 线证据（MEMORY.md critical）。EA 交叉验证已在登录面钉死「Angel 与 BeiDou 是同一份 v83 二进制的不同符号状态」（327/328 同 EA），故本文即 v83 原生行为。
> 标注：`[观察]` = 伪代码逐字；`[推断]` = 由符号/常量推得，非逐字；`[未知]` = 证据不足。

## 0. 结论速览（7 条）

1. `[观察]` v83 原生状态栏是**单体窗 `CUIStatusBar`，41 个方法**，把「聊天输入框 / 聊天日志 + 滚动条 / 快捷栏 8 格 / HP·MP·EXP 条 / 主菜单按钮组 / 密语目标 / 浮公告 / 按钮闪烁提示 / 工具提示 / 频道·组队消息」全部塞进一个 `CWnd` 派生类。当前端把这些拆成约 10 个独立 UI 类（`UIStatusBar/UIChatBar/UIBuffList/UITemporaryStatView/UIEnergyBar/UIComboCounter/UIBossHP/UIClock/UIMiniMap/UIStatusMessenger`），这是**结构一致性差异的核心**（不是文件缺失）。
2. `[观察]` 原生状态栏画布固定 **800×578**（`SetStatusValue: (*(*v36+140))(0,0,800,578,0xFFFFFF)`；`CSlideNotice::OnCreate` 画布 **800×600**），坐标系是相对整屏 800 宽的左下角，不是「屏幕右下角浮动小窗」。
3. `[观察]` 原生有**完整按钮 id 表**（`OnButtonClicked` 逐字 switch）：`1000~1015` 主功能 + `2000~2003` 四个「装备/背包/属性/技能」大窗切换键，且每个都联动 `CWvsContext::UI_Toggle(...)` + `SetButtonBlink(...)`。
4. `[观察]` 原生快捷栏是**坐标命中表**：`GetShortCutIndexByPos` 用 `x-=647`、`y-=427`，逐格 **32×32** 矩形（8 格，基址表 `dword_BE2DB4..BE2DF4`）。上一轮 r15 记「右侧快捷 8 格仅绘制无 hit→send_key」[待当前端子代理复核]。
5. `[观察]` 原生聊天窗有**三态尺寸状态机**（`SetChatType` nType 1/2/3 + `ChangeChatWndSize` 拖拽改高），高度区间 **26..507 px、13 px 步进**，`聊天区高 = 509 - h`，滚动条子控件 id **1010**，拖动/滚轮都会重建 `CCtrlScrollBar` 与 `CCtrlEdit`。
6. `[观察]` 原生 `CCtrlMLEdit` 有完整 **IME 组合输入链路**（`OnIMEComp / CreateIMECandWnd / DestroyIMECandWnd`）；`CChatHelper::TryChat` 有**脏词过滤 `CCurseProcess::ProcessString` + 2000ms 刷屏限流 + 连续 4 条同句检测 + 禁言态 CHATLOG_ADD(nType=8)**。当前端聊天是否具备这些行为规则须复核。
7. `[观察]` 资源节点全部走 `StringPool` 编译期常量（IDA 符号化为 `SP_*`），HUD 面共 **316 个不同 `SP_` 符号**，其中 `UI_STATUSBARIMG_*` 20 个、`STRING_TOOLTIPHELPIMG_GAME_*` 提示文案若干。明文字符串极少（Angel 全库字符串缓存仅 1281）。IMG 层 orange-wz MCP 未接入，节点真值须以仓库 `.nx` 引用 + 正确端截图为准。

## 1. `CUIStatusBar` 41 方法（EA 全表，`evidence/angel-hud-ea.json`）

`[观察]` 类清单（EA → 方法名）。加 ★ = 本轮取到完整/关键伪代码。

| EA | 方法 | 作用（[推断] 除注明外） |
|---|---|---|
| 0x4c6db2 | GetWhisperTarget | 取密语目标名 |
| 0x8cfd76 | OnSetFocus | 焦点 |
| 0x8cfd7b | ClearToolTip | 清提示 |
| 0x8cfd83 / 0x8cfd89 | GetRTTI / IsKindOf | RTTI |
| **0x8d01b2** | **OnCreate ★** | 构造全部子控件：聊天字体（多色 IWzFont）、CCtrlButton×N、CCtrlScrollBar（滚动条尺寸由 `CCtrlScrollBar::GetScrollBarSize` 决定）、chat edit；含 `(*(...+32))(…, 1000, 573, 543, 6, …)` 一处控件创建参数；`*(this+2980) = IsAdminAccount ? 256 : 70`（管理号聊天宽 256 / 普通 70，[推断]）|
| 0x8d419c | OnChildNotify ★ | 子控件回调路由（见 §3）|
| 0x8d4225 | OnMouseEnter | 悬停 |
| **0x8d423d** | **OnButtonClicked ★** | 按钮 id 分派（见 §2）|
| 0x8d4348 | ConvertWhisperToNormal | 密语转普通 |
| 0x8d468d | GetChatLog | 取聊天记录 |
| 0x8d4a22 | SendClaim | 举报/领取（[推断]）|
| 0x8d4a6d | SetChatType ★ | 聊天窗三态（见 §4）|
| 0x8d4dbf | ChangeWhisperTarget | 切密语目标 |
| 0x8d536c | OnKey ★ | 键盘（含快捷键/表情键 `GetEmotionKey`）|
| 0x8d57ca | GetEmotionKey | 表情键映射 |
| 0x8d5a4d / 0x8d5aaf | StartChat / EndChat | 开/关输入 |
| 0x8d5acd | FloatNotice | 浮公告 |
| 0x8d5b2b | SetProblem | 问题提示 |
| 0x8d5b5d | OnMouseButton | 鼠标按下 |
| 0x8d5bfb | TryBeginWhisper | 起密语 |
| **0x8d616c** | **ChangeChatWndSize ★** | 拖拽改聊天窗高（见 §4）|
| 0x8d76d7 | ProcessToolTip | 工具提示 |
| 0x8d795a | OnMouseMove | 悬停移动 |
| **0x8d80a4** | **Draw ★** | 主绘制：`GetCharacterData` → `_ZtlSecureGet_nJob` → `SetStatusValue` → `sub_8D850B(HP/MP/EXP/等级)` → `sub_8DD860/sub_8DDB30` 快捷栏重绘；`(*(*field+28))(field)==22` 特殊地图分支 |
| 0x8d82a7 | ReDrawQuickslot ★ | `sub_8DD860(this); return sub_8DDB30(this);` |
| 0x8d82b8 | SetStatusValue ★ | 画等级数字 + 职业名 + 第三行（见 §5）|
| 0x8db01c | SetWhisperTarget | 设密语目标 |
| 0x8db070 | ChatLogAdd | 追加日志 |
| 0x8db485 | CChatLog::CChatLog | 日志项 ctor |
| 0x8dbb93 | ChatLogDraw ★ | 聊天日志绘制（59 KB 大函数）|
| 0x8de7da | HitTest | 命中测试 |
| **0x8de8d5** | **GetShortCutIndexByPos ★** | 快捷栏坐标命中（见 §6）|
| 0x8de964 | Update | 帧更新 |
| 0x8defae | ResetButtonToolTip | 重置按钮提示 |
| 0x8df155 | SendGroupMessage | 组队/家族消息 |
| 0x8df4b1 | ToggleQuickSlot ★ | 快捷栏双层切换（改 `this+836` + 写 `CConfig+22`，动 IWzGr2DLayer LT/Origin）|
| 0x8dfb36 | MakeCtrlEdit | 建输入框 |
| 0x8dfe21 | SetButtonBlink ★ | 按钮闪烁：`a2<4`，按 `a2=0/1/2/3` 取 `SP_1986..1989_..._KEY_ANI` 动画节点；`this+4*a2+3292` |
| 0x8e03f1 | SetShortcutTooltip | 快捷格提示 |

## 2. `OnButtonClicked @ 0x8d423d` 按钮 id 全表 `[观察逐字]`

入口先 `if (CUserLocal::ms_pInstance && *(…+12596)) CUserLocal::OnKeyDownSkillEnd(...)`（点按钮先结束蓄力技能）。

| id (hex) | dec | 行为 |
|---|---|---|
| 0x7D1 | 2001 | `CWvsContext::UI_Toggle(0,-1)` + `SetButtonBlink(1,0)` → 装备窗 `[推断]` |
| 0x7D2 | 2002 | `UI_Toggle(2,-1)` + `SetButtonBlink(2,0)` → 属性窗 `[推断]` |
| 0x7D3 | 2003 | `UI_Toggle(3,-1)` + `SetButtonBlink(3,0)` → 技能窗 `[推断]` |
| 0x7D0 | 2000 | `UI_Toggle(1,-1)` + `SetButtonBlink(0,0)` → 背包窗 `[推断]` |
| 0x3E8 | 1000 | `CWvsContext::SendMigrateToShopRequest(2000, 0)` → 现金商城迁移 |
| 0x3E9 | 1001 | `sub_A12522(*dword_BE7918)` → 快捷菜单/设置 `[推断]` |
| 0x3EA | 1002 | `CWvsContext::UI_Menu()` → 主菜单 |
| 0x3EB | 1003 | `SetChatType(1)` → 聊天窗「最小态」 |
| 0x3EC | 1004 | `SetChatType(3)` → 聊天窗「普通/展开态」 |
| 0x3ED | 1005 | `UI_Toggle(5,-1)` → 第 5 号窗 `[推断]` |
| 0x3EE | 1006 | `ToggleQuickSlot()` → 快捷栏折叠/展开 |
| 0x3EF | 1007 | `sub_A06CBC(*dword_BE7918)` → 频道/组队 `[推断]` |
| 0x3F1 | 1009 | `SendClaim()` → 举报 |

`[观察]` 无 0x3F0(1008) 分支。`[推断]` 2000..2003 与 `SetButtonBlink(0..3)` 一一对应四个主窗（装备/背包/属性/技能），且 `SP_1979..1982` 节点名 `EQUIPKEY/INVENKEY/STATKEY/SKILLKEY` 佐证。

## 3. `OnChildNotify @ 0x8d419c` 子控件回调 `[观察逐字]`

- `a2==1010`（滚动条）：`a3` 在 `0x12C..0x130`(300..304) 之外 → 跳过；之内 → `ChatLogDraw()` + `*(this+788) = time()`。
- `a2==1012`：`a3==600` → `sub_8D4D43(this, a4)`（聊天输入提交 `[推断]`）。
- `a2 ∈ [0x7D0,0x7D4)`（2000..2003）：`a3==100` → `SetButtonBlink(a2-2000, 0)`（主窗关闭时灭闪烁）。
- 尾：`if (a3==100) (*(*this+32))(this, a2)` → 转发父 `OnButtonClicked`。

## 4. 聊天窗三态尺寸（`SetChatType @ 0x8d4a6d` + `ChangeChatWndSize @ 0x8d616c`）`[观察逐字]`

`SetChatType(nType)` 写 `*(this+832)=nType` 与 `*(CConfig+21)=nType`：

| nType | `this+831`(高 h) | `this+833`(行数) | `this+835`(=509-h) | MakeCtrlEdit | 滚动条创建参数 `a7` |
|---|---|---|---|---|---|
| 1（最小） | 24 | 1 | 513 | 0 | 513 |
| 2 | 24 | 1 | 486 | 1 | 510-h |
| 3（普通/展开） | `CConfig+23`（26..507，越界回 70）| `h/13`（h%13==0 则 h+=2）| `509-h` | 1 | `510-h` |

`ChangeChatWndSize(a2,a3,a4)`：
- `a4>=0`：`a4==1` 时 h≥507 拒绝，否则 `h+=13`；其余 h<26 拒绝，否则 `h-=13`。
- `a4<0`：按拖拽 `a3` 与 `this+748` 差值，`h += 13*((start-a3)/13)`。
- clamp：`h<26→26`，`h>507→507`；`this+833 = h/13`；`h%13==0 → h+=2`；`this+835=this+748=509-h`。
- 随后 `MakeCtrlEdit(1)`、销毁并重建 `CCtrlScrollBar`（`ZAllocEx::Alloc(116)`），`CreateCtrl(this, 1010, 1, 4, 566-scrollbarSize, 510-h, h-2, 0)`，`SetScrollRange(m_aChatLog - 行数 + 1)`、`ChatLogDraw()`，并把 h 写回 `CConfig+23`（持久化）。

`[观察]` 滚动条 id 固定 1010；聊天区高 = `509 - h`；行高 **13 px**；范围 **26..507**。`[推断]` 这是 v83 经典「聊天栏可上下拖高」的完整实现，当前端 `UIChatBar` 是否有等价三态须复核。

## 5. `SetStatusValue @ 0x8d82b8` 绘制坐标 `[观察逐字]`

- 画布 `CWnd::GetCanvas` → `(*(+140))(0,0,800,578,0xFFFFFF)` 铺底。
- `draw_number_by_image(pCanvas, nLeft = 50 - v8, nTop = 554, nNo = a2(等级), …)`：`v8` 按等级位数取 `0 (<=9) / 6 (<=99) / 12 (>99)` → 等级数字左对齐于 x≈38..50、y=554。
- `get_job_name(a3)` → `IWzCanvas::DrawTextA(nLeft=87, nTop=545, sText=职业名, pFont, …)`。
- 第三行 `IWzCanvas::DrawTextA(nLeft=87, nTop=560, sText=a4, …)`。
- `[观察]` `Draw @ 0x8d80a4` 里 `sub_8D850B(this, v14=MP?, v13, v22, v23, v10=exp, v24, v25)` 画 HP/MP/EXP 条；`_ZtlSecureFuse<unsigned char>(v5+51, *(v5+53)) <= 0x32`（等级<=50）分支。

## 6. `GetShortCutIndexByPos @ 0x8de8d5` 快捷栏命中 `[观察逐字]`

```c
v3 = a1 - 647;                       // x 减去 647
v4 = dword_BE2DB4;                   // 每格 [left, top] 表
while (1) {
  v5 = *v4;                          // top
  v7[0]=*(v4-1); v7[2]=v7[0]+32;    // left .. left+32
  v7[1]=v5;      v7[3]=v5+32;       // top .. top+32
  if (PtInRect(v7, v3, a2 - 427))    // y 减去 427
    break;
  ++v8; v4 += 2;
  if (v4 >= &dword_BE2DF4) return -1; // 表尾
}
return v8;                           // 0..N 格索引
```

`[观察]` 每格 **32×32**，X 偏移 **647**、Y 偏移 **427**；`(BE2DF4-BE2DB4)/8 = 8` 个坐标对 → **8 格**（[推断]，与 r15「右侧快捷 8 格」吻合）。表是绝对坐标数组，非公式计算。

## 7. 其它 HUD 类关键语义 `[观察]`

- `CQuickslotKeyMappedMan::DefaultQuickslotKeyMap @ 0x72b8e6` = `memcpy(this+4, dword_BDAFAC, 0x20)` → **默认快捷键映射表 32 字节**；`OnInit(CInPacket&)` 从服务端包初始化；`SaveQuickslotKeyMap` 落盘。
- `CUIEnergyBar::SetEnergy @ 0x7f9bad` 用 `SP_956_CANVAS` 画能量条（32 KB 大函数）；`CUIEnergyBar::OnMouseMove @ 0x7faa49` 可拖动位置（对齐 `SP_1886_DISPLAYS_THE_ENERGY_LEVEL_THATS_BEEN_CHARGED_NUSE_THE_MOUSE_TO_CHANGE_POSITION` 提示）。
- `CUIPartyHP::Create @ 0x91f934`：循环 `v26<6`（**6 人组队**）测名宽，`宽 = max(150, maxwidth+100)`，`高 = v22*(fontH+5)+5`，`CWnd::CreateWnd(v18,v19,v27,v8,z=10,bScreenCoord=1)`；位置由 `CUIWnd::CreateUIWndPosSaved` 读持久化。
- `CClock::SetTimer @ 0x49ae6a`：`if (*(this+30)==0){ *(this+32)=a2; InvalidateRect(0);} ` + `Start()`。
- `CFloatNotice::CreateFloatNotice @ 0x58c53b` 用 `SP_1300_UI_UIWINDOWIMG_FLOATNOTICE_D`；`CSlideNotice::OnCreate @ 0x7e1686` 画布 **800×600**、`SetMsg(ZXString<char>)`。
- `CUIShortCutMenu::OnCreate @ 0x84a6bc`： alloc **1444 字节/按钮** `CCtrlButton`，id 1000..（x=6, y=24/50/…），节点 `SP_2228..2237_UI_UIWINDOWIMG_SHORTCUT_BT{ITEM,EQUIP,STAT,SKILL,QUEST,COMM,MOBBOOK,MESSENGER}` 共 8 项。
- `CChatHelper(5)`：`TryChat @ 0x4904be` 详见 §0.6（脏词 `CCurseProcess::ProcessString`、刷屏 2000ms、重复 4 句、禁言 `SP_893`）；`HistoryUp/HistoryDown/HistoryAdd/HistoryAddforCommand` 命令历史。
- `CTemporaryStatView(8)`：`SetTemporary(long,long,long,UINT128,ZXString)` / `ResetTemporary` / `AdjustPosition` / `ShowToolTip(CUIToolTip&, tagPOINT)` / `FindIcon` / `UpdatePassively` → Buff 图标视图（当前端 `UIBuffList/UITemporaryStatView` 对应）。
- `CCtrlMLEdit(12)`：**`OnIMEComp @ 0x4d33c2` / `CreateIMECandWnd @ 0x4d56b4` / `DestroyIMECandWnd @ 0x4d5be0`** 完整 IME 候选窗链路；`CCtrlEdit(9)` / `CCtrlButton(9)` / `CCtrlScrollBar(14)` / `CCtrlComboBox(5)` 为通用控件基类。

## 8. 资源节点 SymbolPool 全集（HUD 面 316 个 `SP_`，节选 20 个 `UI_STATUSBARIMG_*`）`[观察]`

`SP_1957 GAUGE_BAR` / `SP_1958 GAUGE_GRAY` / `SP_1959 GAUGE_TEMPEXP`（HP/MP/EXP 条）；
`SP_1973 BTSHOP` / `SP_1974 BTSHORT` / `SP_1975 BTMENU` / `SP_1976 BTCLAIM`（按钮）；
`SP_1979 EQUIPKEY` / `SP_1980 INVENKEY` / `SP_1981 STATKEY` / `SP_1982 SKILLKEY` / `SP_1983 KEYSET` / `SP_1984 QUICKSLOT` / `SP_1985 QUICKSLOTD`（四主窗键 + 快捷栏折叠层）；
`SP_1986..1989 EQUIPKEY_ANI/INVENKEY_ANI/STATKEY_ANI/SKILLKEY_ANI`（按钮闪烁动画）；
`SP_1990/1993/1997/1998/1999/2000 STRING_TOOLTIPHELPIMG_GAME_BUTTON_{SHOP,CLAIM,MENU,KEYSET,QUICKSLOT,QUICKSLOTD}`（工具提示文案）；
`SP_2643 FONTMEMO` / `SP_2892 BASE_CHAT` / `SP_1253 BASICIMG_ITEMNO` / `SP_1254 BASICIMG_LEVELNO`（数字字库/聊天底板）。
另有 `SP_1883_UI_UIWINDOWIMG_ENERGYBAR`、`SP_1300_UI_UIWINDOWIMG_FLOATNOTICE_D`、`SP_2228..2237_UI_UIWINDOWIMG_SHORTCUT_*`、`SP_1263_UI_UIWINDOWIMG_MINIMAP_BTMAP`、`SP_2783_MAP_OBJ_ETCIMG_CLOCK_FONTTIME`、`SP_5537_EXP__D___D`（EXP 显示格式串）、`SP_4433_SHOWPARTYHP`、`SP_4190_MARK_HP_ONLY` / `SP_4191_MARK_NAME_AND_HP` 等。完整 316 个见 `evidence/angel-hud-facts.md` §1。

`[观察]` 登录面已证 Angel 明文 WZ 路径极少；HUD 面同样走 StringPool 索引，故 **IMG 节点树真值仍需仓库 `.nx` 引用 / `[DBG-UI]` 转储 / 正确端截图**，orange-wz MCP 未接入（第三路子代理复核中）。

## 9. 与「上一轮 BeiDou 无符号校准」的关系 `[推断]`

- 上一轮 `docs/features/beidou-v17-parity/notes/r16-rea-coverage-gap.md` 记 `StatusBar sub_8D01B2 CreateCtrl 坐标表（点击/QS/聊天行为未闭环）`，仅到 C− 层。Angel 给出 `sub_8D01B2 = CUIStatusBar::OnCreate` 的**符号归属 + 41 方法全貌 + 按钮/尺寸/命中表逐字伪代码**，把 C− 补到接近 C。
- 登录面 EA 327/328 同 EA 已证两端同一二进制；`0x8D01B2` 亦在 BeiDou 端同 EA（[推断]，待 `tools/ida_ea_crosscheck.py` 对 HUD 面跑一次确认）。
- r15 P0「右侧快捷 8 格仅绘制」、r16「聊天/HP·MP·EXP 条 🟡」现在有了可对照的原生数值（32×32 / x-647 / y-427 / h 26..507 / 行高 13 / 画布 800×578）。

## 10. 未决与下一步

- `[未知]` 当前端 HUD 源码是否具备本文 §2/§4/§6 的行为 → 第一路子代理 `e0827232`。
- `[未知]` 既有文档对状态栏的已校准/未闭环四态 → 第二路子代理 `1605f5df`。
- `[未知]` IMG 资源与样式证据覆盖度、orange-wz 可用性 → 第三路子代理 `c202a760`。
- 三路收齐后 → `notes/r02-...`（如需要）、主交付物 `README.md`（六维对照表 + 决策点 + 一次性 Plan），按技能门禁先交决策点再进入实现。
- 全程只读取证；本文未改任何产品源码、未构建、未实机、未提交。
