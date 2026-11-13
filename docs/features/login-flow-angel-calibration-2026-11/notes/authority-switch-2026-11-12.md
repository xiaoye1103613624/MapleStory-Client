# 权威口径切换 · 2026-11-12（2026-11-13 扩围）

## 用户指令

| 日期 | 指令 |
|------|------|
| 2026-11-12 | 登录链后续 **全部以 Angel 反编译为准，不以 BeiDou 客户端 / 运营截图为准**。 |
| 2026-11-13 | **凡属原始 UI / 客户端代码 / 逻辑 / 动画问题，一律按 Angel 来**（不限登录链；含选角详情、HUD、对话等凡原生对照项）。 |

## 准绳

| 优先级 | 源 | 用途 |
|--------|-----|------|
| 1 | Angel IDA / REA（登录：`CUITitle` / `CUIWorldSelect` / `CUIChannelSelect` / `CUICharSelect` / `CUICharDetail` / Creation / `CLogin::*`；其它原生窗体同理） | 定位、动画、按钮、交互、状态机、字色字号 |
| 2 | IMG / `.nx`（orange-wz；失败则 nx dump） | origin、帧序、节点树 |
| 降级 | BeiDou「正确截图」、运营截图 Δ、经验平移 | 仅 Angel 对齐后的视觉抽检，**不得**再写进锚点常数 |

## 纠偏记录

### 2026-11-12 · 频道窗

| 项 | 错误历史 | Angel 真值 |
|----|----------|------------|
| 频道窗 | 误标 `0x62B2CF` RecommendWorld / 截图锚 signboard | `CUIChannelSelect` ctor `0x628501`：`(-176,-688)` 448×253 |
| 卷轴展开 | `WS_SCROLL_ANIM_OFF(-302,-274)` 截图 | `0x629348` 局部 `(-38,-141)` → 画布 TL `(-214,-829)` |
| 世界条 | 截图改 `WS_POLE_MAP_POS` | `CUIWorldSelect` ctor `0x6246CE`：`CreateDlg(-140,-804,400,100)` |

### 2026-11-13 · 选角详情板

| 项 | 错误历史 | Angel 真值 |
|----|----------|------------|
| 属性值 Y | 烘焙标签 +2 → 3/21/39/57 | `CUICharDetail::Draw @0x602ADC`：1/19/37/55 |
| 排行 Y | 93（截图折中） | nTop **99**（不可用 36；有排行 115−w） |
| 字色 | `TOBACCOBROWN` 观感棕 | `get_basic_font(1)` → **`#000000`**（`0xFF000000` @`0x98A83A`） |

实现：`IO/UITypes/UICharSelect.cpp`；审计：`notes/charselect-selected-ui-audit-2026-11-13.md`。

## 产物

对照矩阵：`notes/login-chain-matrix-2026-11-12.md`  
实现：`IO/UITypes/UILogin.cpp` / `UIWorldSelect.cpp` / `UICharSelect.cpp` / `UIRaceSelect.cpp` / `UICommonCreation.cpp`（及 Explorer/Cygnus/Aran）
