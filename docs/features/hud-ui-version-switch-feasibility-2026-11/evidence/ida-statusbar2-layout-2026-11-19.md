# IDA 取证：MapleStoryT.exe（"192客户端"）HUD / StatusBar2 布局与小地图裁剪

- **取证日期**：2026-11-19
- **取证方式**：只读取证（未改任何代码/资源；decompile 不修改 IDB）
- **IDB**：`E:\MXD\反编译IDA\MapleStoryT.i64`，module `MapleStoryT.exe`，imagebase `0x400000`
- **服务状态**：`server_health` → `auto_analysis_ready=true`、`hexrays_ready=true`、`strings_cache_ready=true`
- **资源契约**：确认本端 HUD 走 **`UI/StatusBar2.img`** + **`UI/UIWindow2.img`** + `UI/UIWindow4.img`（TMS 风格）。OnCreate 中 39 处 UI 路径**全部**为 `UI/StatusBar2.img/...`；小地图层路径为 `UI/UIWindow2.img/MiniMap/...`。**未出现 `StatusBar3.img`**，与背景约束一致。
- **原始反编译落盘**：本目录 `_raw_deComp_oncreate.c`（1931 行）、`_raw_deComp_setlayer.c`（3252 行）、`_raw_<小函数>.c`（见附录）；均由服务 `/output/<uuid>.json` 完整 spill 取回，非截断片段。
- **取证工具链**：`tools/_mcp_call.py`（IDA MCP 13339）、新增 `tools/_ida_decomp_batch.py`（批量取 spill 完整伪代码）。

> ⚠️ 口径说明（避免断章取义）：Hex-Rays 对本二进制的 COM 返回值存在误建模（把 `IWzVector2D::Release` 返回值指针当成 `m_nInitX` 等 int）。本文对每条结论标注「可信度」；凡涉及 COM 返回值强转的，一律不采信为布局数值，只采信明确的参数常量与内存表值。

---

## 1. 函数清单（任务1）

用 `py_eval` 在 IDA 上下文遍历 `idautils.Functions()` + `idc.get_func_name()`，对关键字
`CUIStatusBar / CUIMiniMap / QuickSlot / ShortCut / ChatBar / Gauge / ExpBar` 做大小写不敏感筛选：**命中 1431**（含 `PowerTransferGauge`、`SubGauge` 等噪声）。下表是取证直接相关的核心命中（完整清单可由 `_mcp_call.py` 脚本复跑；噪声函数与本次布局无关，不逐条列）。

| 函数 | 地址 | 用途 |
| --- | --- | --- |
| `CUIStatusBar::OnCreate` | `0x16BE4E0` | HUD 创建（size 0x1f59） |
| `CUIStatusBar::Draw` | `0x16C0440` | HUD 绘制 |
| `CUIStatusBar::ChangeScreenResolution` | `0x16A1FE0` | 分辨率切换 |
| `CUIStatusBar::_Init_Buttons_MapleWorld` | `0x16AC7D0` | 普通风格底部按钮 |
| `CUIStatusBar::_Init_Buttons_StarPlanet` | `0x16ABD80` | StarPlanet 风格底部按钮 |
| `CUIStatusBar::_InitLayerChat` | `0x169BF60` | 聊天层 |
| `CUIStatusBar::_ResetChatBarPos` | `0x169FC70` | 聊天栏位置 |
| `CUIStatusBar::GetChatTarget` / `SetChatTarget` | `0x761950` / `0x16B7CE0` | 聊天目标 |
| `CUIStatusBar::CQuickSlot::Init` | `0x16A80F0` | 快捷键位初始化 |
| `CUIStatusBar::CQuickSlot::GetInitX` | `0x1696580` | 返回 `m_nInitX` |
| `CUIStatusBar::CQuickSlot::GetPosByIndex` | `0x16968F0` | 取第 n 格坐标 |
| `CUIStatusBar::CQuickSlot::ChangeScreenResolution` | `0x169B710` | 快捷键位重定位 |
| `CUIStatusBar::CQuickSlot::Draw` | `0x16BBE20` | 快捷键位绘制 |
| `CUIStatusBar::CQuickSlot::Toggle` | `0x16972A0` | 收起/展开 |
| `CUIMiniMap::SetLayer` | `0x14393D0` | 小地图 canvas 装载（size 0x3f67） |
| `CUIMiniMap::OnCreate` | `0x1433380` | 小地图层创建 |
| `CUIMiniMap::SetCtrl` | `0x1435890` | 小地图按钮/尺寸控制 |
| `CUIMiniMap::CalculateScr` | `0x14316B0` | 视口滚动原点计算 |
| `CUIMiniMap::IsPtInViewRange` | `0x1431DD0` | 视口裁剪判定 |
| `CUIMiniMap::TransformPoint` | `0x1431670` | 世界点→小地图点 |
| `CUIMiniMap::DrawIcon(tagPOINT*)` | `0x1432C30` | 单点图标绘制 |
| `CUIMiniMap::DrawIcon(ZList<tagPOINT>*)` | `0x1432E30` | 批量图标绘制 |
| `CUIMiniMap::LoadMiniMap` | `0x1446FA0` | 从 IMG 读尺寸 |
| `CUIMiniMap::Update` | `0x1443B40` | 每帧刷新（图标绘制入口） |
| `CUIMiniMap::ToggleMiniMapState` | `0x1443A60` | 三态切换 |
| `CUIMiniMap::AutoExpandUI` | `0x1443AA0` | 自动展开 |
| `CUIShortCutMenu::OnCreate` | `0x140E720` | 右侧快捷菜单 |

**关于重点函数 `MiniMap::CalculateScr / SetCtrl / ToggleSize / OnDraw`：**
- `CalculateScr` ✅ `0x14316B0`、`SetCtrl` ✅ `0x1435890`。
- `ToggleSize` ❌ **未找到同名函数**。证据：函数表中无 `*ToggleSize*@CUIMiniMap`；但 `SetCtrl@0x1435CEB` 中存在成员 `this->m_pBtToggleSize`（"调整大小"按钮），其启用态由阈值控制（见 §3.1）。**局限**：无法确认开源 end 所说的 ToggleSize 是否指该按钮逻辑或另一未命名函数。
- `OnDraw` ❌ **CUIMiniMap 无独立 `OnDraw`**。绘制逻辑集中在 `CUIMiniMap::Update@0x1443B40`（2761 行伪代码）内。
- 快捷键位另有 `CUIKeyConfig::GetShortCutPos@0x1396510`、`CUIKeyConfig::GetShortCutIndexByPos@0x1396460`（键位→坐标映射），与 HUD 显示坐标不同层，列为延伸项。

---

## 2. CUIStatusBar::OnCreate @ `0x16BE4E0` 布局取证

### 2.1 基准原点（可信度：高）

```c
CLayoutMan::Init(this: &this->m_lm, pWnd: &this->baseclass_0,
                 nOffsetX: 512, nOffsetY: 577);   /*0x16be531*/
this->m_nScrWidth = CCtrlScrollBar::GetScrollBarSize(hv:1, type:8); /*0x16be544*/
CUIStatusBar::_CreateFont_ChatLog(this);          /*0x16be54a*/
```

**结论**：HUD 布局基准原点 **(512, 577)**（典型 1024×768 客户区的水平中线与底部）。`nOffsetY=577` 是 768 高客户端的状态栏起始带。

### 2.2 底部功能按钮创建顺序（可信度：高）

OnCreate 本体只创建「聊天/系统类」按钮；商城/角色/装备/技能等底部功能按钮由两个子函数创建。按**代码调用顺序**：

**A. OnCreate 本体内按钮（`0x16BF745`–`0x16BFDxx`，nOffsetX/Y 全 0，坐标取自 IMG）：**

| 序 | UOL | 成员 | nID | 地址 |
| --- | --- | --- | --- | --- |
| 1 | `UI/StatusBar2.img/mainBar/chatOpen` | `m_pBtMax` | — | `0x16BF745` |
| 2 | `UI/StatusBar2.img/mainBar/chatClose` | `m_pBtMin` | — | `0x16BF7D4` |
| 3 | `UI/StatusBar2.img/mainBar/BtChat` | `m_pBtWhisper` | — | `0x16BF863` |
| 4 | `UI/StatusBar2.img/mainBar/BtClaim` | `m_pBtClaim` | — | `0x16BF93A` |
| 5 | `UI/StatusBar2.img/mainBar/BtChannel` | `m_pBtChannel` | — | `0x16BFA11` |
| 6 | `UI/StatusBar2.img/chat/Tap/all` | `m_apBtChatFilter[0]` | `0x3FF` | `0x16BFAFF` |
| 7 | `.../chat/Tap/friend` | `[1]` | `0x3FF` | `0x16BFB7D` |
| 8 | `.../chat/Tap/party` | `[2]` | `0x3FF` | `0x16BFBFB` |
| 9 | `.../chat/Tap/guild` | `[3]` | `0x3FF` | `0x16BFC79` |
| 10 | `.../chat/Tap/association` | `[4]` | `0x3FF` | `0x16BCFF7` |
| 11 | `.../chat/Tap/expedition` | `[5]` | `0x3FF` | `0x16BFD75` |
| 12 | `.../chat/Tap/afreecaTV` | `[6]` | `0x3FF` | `0x16BFDF5` |

伪代码样例（`0x16BF745` 起，全部 nOffset 为 0）：

```c
v102 = CLayoutMan::AddButton(p_m_lm, &v191,
    sButtonUOL: L"UI/StatusBar2.img/mainBar/chatOpen",
    nID: ..., nOffsetX: 0, nOffsetY: 0, pParam: 0, nAlpha: 255,
    bToggle: 0, bOnMoveIVRect: 0, bSkipIDCheck: 0);
this->m_pBtMax.p = v102->p;
```

**B. MapleWorld 风格底部按钮（`_Init_Buttons_MapleWorld@0x16AC7D0`，代码顺序）：**

| 序 | UOL (`UI/StatusBar2.img/mainBar/`) | nID |
| --- | --- | --- |
| 1 | `BtNotice` | `0x3F2` |
| 2 | `BtCharacter`（角色） | `0x7D5` |
| 3 | `BtFarm` | — |
| 4 | `BtCashShop`（商城） | — |
| 5 | `BtStarPlanet` | `0x401` |
| 6 | `BtSystem`（设置） | — |
| 7 | `BtStat`（能力） | `0x7D3` |
| 8 | `BtQuest`（任务） | `0x7D6` |
| 9 | `BtInven`（装备/道具） | `0x7D2` |
| 10 | `BtEquip` | `0x7D1` |
| 11 | `BtSkill`（技能） | `0x7D4` |
| 12 | `BtKeysetting`（键位） | `0x3ED` |
| 13 | `BtMenu`（菜单） | `0x3E9` |

```c
CLayoutMan::AddButton(&this->m_lm, &result,
    L"UI/StatusBar2.img/mainBar/BtCharacter", 0x7D5u, 0,0,0, 255, 0,0,0); /*0x16AC7D0+行110*/
```

**C. StarPlanet 风格底部按钮（`_Init_Buttons_StarPlanet@0x16ABD80`）：**
`BtCharacter → BtCashShop(0x3E8) → BtMaple(0x400) → BtSystem(0x3EF) → BtStatic(0x7D8) → BtFriend(0x7D7) → BtQuest(0x7D6) → BtInventory → BtEquip(0x7D1) → BtMoveSkill → BtKeysetting → BtMenu(0x3E9)`，路径前缀 `UI/StatusBar2.img/starPlanet/`。

**⚠️ 坐标局限**：`AddButton` 的第 5/6 参数 `nOffsetX/nOffsetY` 全为 0；按钮的像素坐标不在代码里，而在 `StatusBar2.img/mainBar/<BtXxx>` 节点的 `origin`/位置属性中。**代码层只能确认创建顺序与资源 UOL，不能给出像素坐标**——需后续用 orange-wz MCP 查 IMG 节点补齐（本次任务范围为 IDA，未越界）。禁止据此外推坐标数值。

### 2.3 血条 / 经验条 / HP / MP 布局（可信度：中高，位置数值来自 IMG）

OnCreate 按如下顺序创建 CGauge（`CGauge::Create`）：

| 序 | 资源 UOL | 成员 | 关键代码地址 |
| --- | --- | --- | --- |
| 1 | `starPlanet/gaugeBackgrnd` + `starPlanet/gauge/red|blue` | `m_GaugeStarPoint` / `m_GaugeStarPointAchive` | `0x16BEAF7` / `0x16BEB37` |
| 2 | `mainBar/gaugeBackgrd` (职业变体 `...Demon/AB/Zero/Kinesis`) | 背景层 | `0x16BED13`–`0x16BED3F` |
| 3 | `mainBar/gauge/hp` | `m_GaugeHP` | `0x16BED8C` / `0x16BEDB0` |
| 4 | `mainBar/gauge/mp` | `m_GaugeMP` | `0x16BF1B5` / `0x16BF1D1` |
| 5 | `mainBar/gauge/tf` | (复用 m_GaugeMP 临时对象) | `0x16BF5CC` / `0x16BF5F4` |
| 6 | `mainBar/gauge/exp` | `m_GaugeEXP` | `0x16BF4C3` / `0x16BF4EB` |
| 7 | `mainBar/gauge/relaxExp` | `m_GaugeEXP_Relax` | `0x16BF50E` / `0x16BF52A` |
| 8 | `mainBar/gauge/df/%d` | `m_mGaugeDF` (map) | `0x16BF3xx` |
| 9 | `mainBar/gaugeCover` (职业变体 AB/Kinesis) | 覆盖层 | `0x16BF59E`–`0x16BF654` |

```c
v192.m_Data = L"UI/StatusBar2.img/mainBar/gauge/hp";  /*0x16bed8c*/
CGauge::Create(this: &this->m_GaugeHP, ...);          /*0x16bedb0*/
v186 = L"UI/StatusBar2.img/mainBar/gauge/mp";         /*0x16bf1b5*/
CGauge::Create(this: &this->m_GaugeMP, ...);          /*0x16bf1d1*/
v186 = L"UI/StatusBar2.img/mainBar/gauge/exp";        /*0x16bf4c3*/
CGauge::Create(this: &this->m_GaugeEXP, ...);         /*0x16bf4eb*/
```

等级数字/背景：`mainBar/lvNumber`(`0x16BE5C4`)、`mainBar/lvBacktrnd`(`0x16BE370`)、`mainBar/lvCover`(`0x16BE383`)。
**⚠️ 局限**：CGauge 的具体条宽/条高/HP:MP 相对偏移由 IMG 节点与 `CGauge::Create` 内部决定，本文只钉住"创建顺序 + 资源键"，不臆造像素值。

### 2.4 QuickSlot 快捷键位原点与 pitch（可信度：高，来自内存常量表）

`GetPosByIndex@0x16968F0`：

```c
void CQuickSlot::GetPosByIndex(int nIdx, int *x, int *y) {
  if ( nIdx < this->m_nQuickSlotDisplayNo ) {       /*0x16968fa*/
    *x = s_ptShortKeyPos[nIdx].x;                   /*0x1696907*/
    *y = s_ptShortKeyPos[nIdx].y;                   /*0x1696914*/
  }
}
```

`s_ptShortKeyPos` 全局数组 @ **`0x279C390`**（IDA `get_wide_dword` 实读，每项 8 字节 tagPOINT）：

| idx | (x,y) | idx | (x,y) |
| --- | --- | --- | --- |
| 0 | (7, 15) | 6 | (73, 48) |
| 1 | (40, 15) | 7 | (106, 48) |
| 2 | (73, 15) | 8 | (150, 15) |
| 3 | (106, 15) | 9 | (183, 15) |
| 4 | (7, 48) | 10 | (216, 15) |
| 5 | (40, 48) | 11 | (249, 15) |

**结论**：快捷键位原点 **(7,15)**；第一排 idx0–3 的 **pitch = 33px**（x: 7→40→73→106），第二排 idx4–7 同 pitch、**y = 48**（比第一排 +33）；idx8+ 为右侧扩展位，x 从 150 起 pitch=33、y=15。表中 `s_ptShortKeyPos_0@0x27C5978` 全为 `-1`（哨兵/未初始化），**不是**有效坐标，勿误用。

原点寄存器 `m_nInitX/m_nInitY` 来自 `Init@0x16A80F0` 的 slide 背景层 RelMove：

```c
IWzVector2D::RelMove(this: &v28->baseclass_0,
    nX: this->m_nInitX,
    nY: this->m_nInitY + (this->m_bShowSlide != 0 ? 0 : 67), /*0x16a85xx*/
    ...);   /*0x16a8462 / 0x16a84a0 / 0x16a852f / 0x16a86xx*/
```

**结论**：slide 收起时（`m_bShowSlide==0`）快捷栏整体 **下移 67px**。
**⚠️ 局限**：`Init` 中 `m_nInitX = (int)sUOL._m_pStr`（`0x16A8462`）与 `m_nInitY = (int)sUOL._m_pStr`（`0x16A84A0`）是 Hex-Rays 把 COM 方法返回值错当指针的误建模，**不可信**，故真实初始 (m_nInitX, m_nInitY) 数值未定；可信的是相对位移 67 与 `s_ptShortKeyPos` 表。

`CQuickSlot::ChangeScreenResolution@0x169B710` 的重定位片段：

```c
IWzVector2D::RelMove(..., nX: 881, nY: 2, ...);       /*0x169ba24, 0x169bbf2*/
this->m_nQuickSlotDisplayNo = v21;                    /*0x169bcde*/
```

**结论**：分辨率变化时右侧快捷位重定位到 **(881, 2)**（相对父层），并重算显示格数 `m_nQuickSlotDisplayNo`。

### 2.5 聊天栏 chatTarget 布局（可信度：中）

```c
int CUIStatusBar::GetChatTarget() { return this->m_nChatTarget; }  /*0x761956*/
```

`_ResetChatBarPos@0x169FC70`：

```c
if ( this->m_pLayerChatBar.m_pInterface != 0 ) {        /*0x169fca0*/
  ...
  IWzVector2D::RelMove(this: m_pLayerChatBar,
      nX: 0, nY: this->m_ptChatWnd.y - 2, ...);          /*0x169fd56*/
}
```

聊天分页按钮 `m_apBtChatFilter[0..6]` = `chat/Tap/{all,friend,party,guild,association,expedition,afreecaTV}`，统一 `nID=0x3FF`、nOffset 全 0（见 §2.2A）；`_SetFilterButton@0x1697020` 控制可见性；创建后有 `LOBYTE(__that.m_nY)=60/45` 的辅助坐标写入（`0x16BFDF8`/`0x16BFE2C`，**可信度低**：同为 variant 字段强转，疑似工具误报，不采信为最终像素）。

**⚠️ 局限**：`chatTarget` 在 `GetChatTarget@0x761950` 仅返回 `m_nChatTarget` 枚举值；聊天窗口矩形 `m_ptChatWnd` 的具体数值由 `ChangeChatWndSize@0x16B2E20` / IMG 决定，本次未取全值。**未找到**独立的"chatTarget 布局函数"——只有 `GetChatTarget/SetChatTarget` 存取器。

### 2.6 右侧快捷键（键帽）布局（可信度：中）

右侧快捷菜单 `CUIShortCutMenu::OnCreate@0x140E720`（StarPlanet 变体 `0x140EF60`）是键帽按钮容器；`CUIShortCutMenu::OnKey@0x140D8F0`、`OnButtonClicked@0x140D5C0` 处理按键。键位→坐标由 `CUIKeyConfig::GetShortCutPos@0x1396510` / `GetShortCutIndexByPos@0x1396460` 映射。

**⚠️ 局限**：OnCreate 中**未发现**单独的"右侧键帽逐个 AddButton + 坐标"调用；键帽像素坐标来自 IMG（`UIWindow2.img` 侧）与 `CUIKeyConfig` 表，本次 IDA 取证只能确认类名/入口与映射函数，不能给出每个键帽的 x/y。

---

## 3. CUIMiniMap::SetLayer @ `0x14393D0` 与小地图裁剪取证

### 3.1 SetLayer 做了什么 / 视口尺寸阈值（可信度：高）

`SetLayer` 主体（3252 行）几乎全是 `IWzCanvas` 的 COM 装载：`pCanvas / pCanvasE / pCanvasSW / pCanvasW / pCanvasNW / pCanvasNE / pCanvasSE / pCanvasMapMark / pCanvasC`，对应小地图 8 方向边缘与地图标记贴图；字符串通过 `StringPool::GetBSTR` 取得。**`SetLayer` 内没有绘制逻辑，也没有 portal/NPC 图标绘制**。

`m_nMini_W / m_nMini_H` 的**阈值**不在 SetLayer，而在 `SetCtrl@0x1435890`（`0x1435D4F`）：

```c
// 控制“调整大小”按钮 m_pBtToggleSize 的启用态
if ( this->m_nMini_W <= 280 && this->m_nMini_H <= 150
  || this->m_nMini_W > get_screen_width()  - 50
  || this->m_nMini_H > get_screen_height() - 150 )
{
  this->m_pBtToggleSize.p->...->SetEnable(..., 0);   /*0x1435d60*/
}
```

**结论**：小地图最小尺寸 **280×150**；超过 `屏幕宽-50` / `屏幕高-150` 时禁用"调整大小"按钮。按钮创建：`BtToggleSize`(id 1003)、`BtMaximize`(1001)、`BtMinimize`(1000)，UOL `UI/UIWindow2.img/MiniMap/BtMax|BtMin`（`0x1435D79`/`0x1435E01`）。尺寸初值在 `LoadMiniMap@0x1446FA0` 从 IMG 属性读入：`v4->m_nMini_W = i;`、`v4->m_nMini_H = i;`（IDA 端滤波反编译输出第 399 / 413 行；**局限**：该大函数完整伪代码因 MCP 20000 字符传输上限且无 spill 标记未能取回全量，行号来自 IDA 端滤波结果，未逐字核对上下文）。

> 说明：任务措辞里的"视口尺寸上限 m_nMini_W/H 阈值"实为上面这个**按钮启用阈值**；它是尺寸约束，不是绘制裁剪。真正视口裁剪字段是 `m_nPane_W/m_nPane_H`（绘制视口），二者不要混。

### 3.2 CalculateScr 滚动逻辑 @ `0x14316B0`（可信度：高，纯整数运算）

```c
void CUIMiniMap::CalculateScr() {
  int mag = this->m_nMag;
  int paneW = this->m_nPane_W << mag;
  this->m_nScrOrig_X = this->m_nLocal_X - paneW / 2;                  /*0x14316e0*/
  if ( this->m_nScrOrig_X < -this->m_nReal_CX )                       /*0x14316e6*/
      this->m_nScrOrig_X = -this->m_nReal_CX;
  int limX = this->m_nReal_W - this->m_nReal_CX - paneW;              /*0x14316f6*/
  if ( this->m_nScrOrig_X > limX ) this->m_nScrOrig_X = limX;          /*0x1431700*/
  this->m_nScrOrig_X = (this->m_nReal_CX + this->m_nScrOrig_X) >> mag; /*0x143171e*/
  // Y 轴同理：paneH = m_nPane_H << mag；夹到 [-m_nReal_CY, m_nReal_H-m_nReal_CY-paneH]
  this->m_nScrOrig_Y = (this->m_nReal_CY + this->m_nScrOrig_Y) >> mag; /*0x143176c*/
}
```

**结论**：滚动原点 = 角色位置 − 视口半宽/半高，再夹在地图边界内，最后按 `m_nMag` 右移（缩放级别归一化）。这是标准 clamp 滚动，无溢出。

### 3.3 视口裁剪判定 IsPtInViewRange @ `0x1431DD0`（可信度：高）

```c
int CUIMiniMap::IsPtInViewRange(tagPOINT *pt) {
  int cx = ((pt->x + this->m_nReal_CX) >> this->m_nMag) - this->m_nScrOrig_X; /*0x1431df3*/
  int cy = ((pt->y + this->m_nReal_CY) >> this->m_nMag) - this->m_nScrOrig_Y; /*0x1431df9*/
  return cx >= 0 && cx <= this->m_nPane_W
      && cy >= 0 && cy <= this->m_nPane_H;                                    /*0x1431e17*/
}
```

**结论**：参考端存在正确的视口裁剪函数，边界为 `[0, m_nPane_W] × [0, m_nPane_H]`（含等号，边界像素算可见）。世界点先经 `Real_CX/CY` 平移与 `m_nMag` 缩放，再与滚动原点比较。

### 3.4 图标绘制是否裁剪 —— 本次 bug 的核心结论（可信度：高）

**`DrawIcon(tagPOINT*)@0x1432C30` 内部无任何裁剪**：

```c
void CUIMiniMap::DrawIcon(tagPOINT *pt, IWzCanvas *pIcon) {
  if ( this->m_pLayerBack.m_pInterface != 0 ) {                       /*0x1432c6f*/
    IWzCanvas *dst = IWzGr2DLayer::Getcanvas(this->m_pLayerBack, ...); /*0x1432cba*/
    int h = ...Release(&v11)...; int w = ...QueryInterface(&v12)...;
    IWzCanvas::Copy(this: dst,
        nDstLeft: pt->x + this->m_nPane_X - (w >> 1),                  /*0x1432d72*/
        nDstTop:  pt->y + this->m_nPane_Y - h,
        pSource: pIcon, &nAlpha);
    // 后续仅 variant 清理，无 IsPtInViewRange 调用
  }
}
```

**`DrawIcon(ZList<tagPOINT>*)@0x1432E30` 也无裁剪**，只是循环：

```c
while ( m_pHead != 0 ) {
  ptMinimap = *m_pHead;
  CUIMiniMap::DrawIcon(this, pt: &ptMinimap, pIcon);                   /*0x1432ebc*/
  m_pHead = next;
}
```

**调用侧（`CUIMiniMap::Update@0x1443B40`）核对——逐个 DrawIcon 调用点是否有裁剪：**

| 绘制对象 | 调用地址 | 调用前有 `IsPtInViewRange`? |
| --- | --- | --- |
| NPC/怪物图标（批量 list） | `0x144513C`（`DrawIcon(list)`） | ❌ 无；list 内元素由 `TransformPoint` 变换后直接插入，未过滤（见 `0x14441CD`/`0x14442BF` 等） |
| 普通传送门 portal | `0x144520E`（`DrawIcon(pt)`） | ❌ 无；仅判 `nType==2||7||bShownAtMinimap`(`0x1445195`)，`TransformPoint` 后直接画 |
| 随机传送门 | `0x144556C` | ❌ 无 |
| 其余多个标记 | `0x1445599`/`0x14455C3`/`0x14455ED`/`0x1445617`/`0x1445641`/`0x144566B`/`0x14456E8`/`0x1445712` | ❌ 无 |
| **当前/目标传送门指针** (current/target portal pointer) | 分支 `0x1445256`–`0x1445269` | ✅ **有**：`IsPtInViewRange(p_ptPos)!=0 && IsPtInViewRange(v118->ptPos)!=0` (`0x1445256`/`0x1445269`) |

关键伪代码（portal 分支对比）：

```c
// 普通 portal 图标 —— 无裁剪
if ( nType==2 || nType==7 || portal->bShownAtMinimap ) {  /*0x1445195*/
  TransformPoint(this, &ptIn);                            /*0x14451a6*/
  ...
  CUIMiniMap::DrawIcon(this, &ptIn, icon);                /*0x144520e*/   // ← 无 IsPtInViewRange
}
// current/target portal 指针 —— 有裁剪
if ( PortalByName != 0
  && PORTAL::IsShownAtMinimap(PortalByName) != 0
  && CUIMiniMap::IsPtInViewRange(this, p_ptPos) != 0 ) {  /*0x1445256*/
    if ( CUIMiniMap::IsPtInViewRange(this, &v118->ptPos) != 0 ) /*0x1445269*/ { ... }
}
```

**⚠️ 关于 NPC 的补充核对（避免误判）**：Update 中 `0x14448F0`/`0x1444AC0`/`0x1444EB3` 处确有 `pTemplate.x <= m_nPane_H`、`ptIn.y > m_nPane_W` 一类比较，但精读上下文（`0x14447EB`–`0x1444AB0`）表明它们是 **stalker（被追踪角色）名字标签** `m_mStalkeeName` 的边界判断，与 NPC 图标无关；NPC 图标本体仍由 `0x144513C` 的批量 `DrawIcon` 无裁剪绘制。

### 3.5 与"当前端 bug"的对照结论

- 参考端（MapleStoryT.exe / "192客户端"）的 `IsPtInViewRange` 裁剪**只覆盖 current/target portal 指针这一种标记**；**普通传送门、随机传送门、NPC/怪物图标、其它标记均不裁剪**（`DrawIcon` 内无裁剪，调用侧绝大多数也无）。
- 因此，"未显示区域的传送门/NPC 图标溢出小地图外"这一当前端 bug，**在参考端也以同样方式存在**——参考端并未实现全量视口裁剪。若目标是"对齐参考端"，不应额外加裁剪；若目标是"修复视觉溢出"，那是**超出参考端行为的增强**，需产品决策（本报告只陈述证据，不改代码）。
- **局限**：`Update` 伪代码 2761 行，本取证逐字核对了所有 `DrawIcon`/`IsPtInViewRange` 调用点（grep 共 228 处命中，已全覆盖到 `0x1445712` 最后一个 DrawIcon 调用）；但 Hex-Rays 对部分 `tagPOINT` 变量命名混乱（如 `pTemplate.x` 实为某 pt 的 x），逐个变量的语义归属以地址为准，未做运行时验证。

### 3.6 ToggleSize / OnDraw 的替代证据

- `ToggleMiniMapState@0x1443A60`：
  ```c
  if ( this->m_bShowMiniMap != 0 ) {
    v1 = (this->...m_nOption + 2) % 3;        /*0x1443a79*/  // 三态循环
    this->m_nSavedOption = -1;
    this->...m_nOption = v1;                  /*0x1443a86*/
    CUIMiniMap::ToggleMap(this);              /*0x1443a8c*/
  }
  ```
- `AutoExpandUI@0x1443AA0`（36 行）处理自动展开。
- **无 `OnDraw`**：绘制在 `Update@0x1443B40` 内完成。

---

## 4. 未找到 / 失败项声明（禁止猜测填充）

| 项 | 状态 | 原因 |
| --- | --- | --- |
| `CUIMiniMap::ToggleSize` | **未找到** | 函数表无同名函数；仅有 `m_pBtToggleSize` 成员（SetCtrl 中） |
| `CUIMiniMap::OnDraw` | **未找到** | 无独立 OnDraw；绘制集中在 `Update@0x1443B40` |
| `CUIMiniMap::SetCtrl` 中的窗口坐标像素值 | **未取证** | 本文只取到尺寸阈值与按钮 id；像素坐标来自 `UIWindow2.img/MiniMap` |
| 底部按钮 / gauge / chatTab 的具体像素坐标 | **未取证** | `AddButton` 的 nOffsetX/Y 全 0，坐标在 IMG `origin` 属性；需 orange-wz MCP（超出本次 IDA 范围） |
| `LoadMiniMap` 读尺寸的 IMG 键名 | **未取证** | 只确认 `m_nMini_W/H = i` 赋值；取值键名所在的 py_eval 内联脚本因嵌套引号报错未取回；不影响阈值/裁剪结论 |
| `m_nInitX/m_nInitY` 初始数值 | **不可信** | Hex-Rays 把 COM 返回值误建模为指针（`0x16A8462`/`0x16A84A0`），未采信 |
| 运行时验证 | **未做** | 本次为纯静态只读取证；裁剪缺失结论基于 Hex-Rays 伪代码调用链，未做动态 hook/截图验证 |

---

## 附录：证据文件索引（本目录）

```
_raw_deComp_oncreate.c                 CUIStatusBar::OnCreate       1931 行
_raw_deComp_setlayer.c                 CUIMiniMap::SetLayer         3252 行
_raw_minimap_calculatescr.c            CalculateScr                   34 行
_raw_minimap_isptinviewrange.c         IsPtInViewRange                13 行
_raw_minimap_transformpoint.c          TransformPoint                 15 行
_raw_minimap_drawicon_pt.c             DrawIcon(tagPOINT*)            91 行
_raw_minimap_drawicon_list.c           DrawIcon(list)                 35 行
_raw_minimap_setctrl.c                 SetCtrl                       365 行
_raw_minimap_update.c                  Update                      2761 行
_raw_minimap_oncreate.c                OnCreate                    1255 行
_raw_minimap_togglestate.c             ToggleMiniMapState             12 行
_raw_minimap_autoexpand.c              AutoExpandUI                   36 行
_raw_quickslot_init.c                  CQuickSlot::Init             1153 行
_raw_quickslot_getinitx.c              GetInitX                        4 行
_raw_quickslot_getposbyindex.c         GetPosByIndex                   8 行
_raw_quickslot_changescreenresolution.c ChangeScreenResolution       256 行
_raw_statusbar_changescreenresolution.c CUIStatusBar::ChangeScreenResolution 73 行
_raw_statusbar_resetchatbarpos.c       _ResetChatBarPos              149 行
_raw_statusbar_getchattarget.c         GetChatTarget                   4 行
_raw_statusbar_initlayerchat.c         _InitLayerChat                123 行
tools/_ida_decomp_batch.py             （仓库 tools/）批量 spill 取回脚本
```

> 方法论：先函数表定位 → 完整伪代码落盘 → grep 建证据索引 → 逐段精读调用点 → 只写有地址与代码片段支撑的结论。凡工具截断、符号缺失、COM 误建模处均显式标"局限"，不以臆测补齐。
