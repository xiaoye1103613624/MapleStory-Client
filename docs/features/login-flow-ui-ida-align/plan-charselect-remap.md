# 选角色界面重映射 —— 汇总与一次执行清单

> 本文把所有已取得的 IDA 常量、当前代码状态、以及**待执行的完整改动**汇总在一起，
> 目标是**一次执行完成**，之后仅剩「1 个原点 × 2 分辨率」的实测验证。

---

## 一、已取得的 IDA 权威常量（全部有出处）

### 1.1 四个面板的窗口锚点（同处一个 `bScreenCoord = 0` 坐标系）

| 面板 | 出处 | nLeft | nTop | 宽 | 高 | z |
|---|---|---|---|---|---|---|
| 世界选择 `CUIWorldSelect` | `sub_6246CE` | −140 | −804 | 400 | 100 | 10 |
| **角色列表**（类名未命名） | **`sub_603FF0`** | **−290** | **−1218** | **600** | **80** | **10** |
| 角色按钮条 `CUICharSelect` | `??0CUICharSelect@@` `0x60135B` | 200 | −1359 | 101 | 131 | 10 |
| 频道选择 `CUIChannelSelect` | `sub_62B2CF` | 136 | 51 | 540 | 167 | 211 |

**关键推论**：角色按钮条 `x∈[200,301]` 正好落在角色列表 `x∈[−290,310]` 的**右端内侧**，
且按钮条底边 `−1228` 比列表顶边 `−1218` 高 **10px** —— 即
「三个按钮竖排在角色列表行的右上方」，这是可直接落地的关系。

### 1.2 角色列表 / 详情几何

| 项 | 出处 | 值 |
|---|---|---|
| 每行槽位数 | `CUICharDetail` ctor `0x602960` | **3**（`a4 % 3`） |
| 槽位横向间距 | 同上 | **130** |
| 三槽相对 X | 同上 | **−220 / −90 / +40** |
| 详情面板尺寸 | 同上 | **183 × 151**（有角色）/ **183 × 115**（空槽） |
| 详情面板 Y | 同上 | **1384**（有角色）/ **1348**（空槽） |
| 每槽数据字节数 | `sub_602ADC` 头部 + `CLogin::ChangeStep` | **684** |

### 1.3 三个按钮的位置（`CUICharSelect::OnCreate` @ `0x60148D`）

```c
CCtrlButton::Create(..., id:1000 /*BtSelect*/, x:4, y:-2,  ...);
CCtrlButton::Create(..., id:1001 /*BtNew*/,    x:4, y:36,  ...);
CCtrlButton::Create(..., id:1002 /*BtDelete*/, x:4, y:85,  ...);
```

### 1.4 绘制函数定位方法（可复用）

| 类 | vtable | `Draw` 槽位(11) |
|---|---|---|
| `CUICharSelect` | `off_AF6D90` | `CWnd::Draw`（**未重写**） |
| `CUICharDetail` | `off_AF6E3C` | **`sub_602ADC`** |
| 已确认 `OnCreate` = slot 3（`CreateWnd` 内 `(*(*this+12))(this,pData)`） | | |

### 1.5 节点层结论（已验证，不需要再查）

`Login.img/CharSelect` 子树**只有美术**（叶子统计：`origin` 92 / `z` 83 / `delay` 49 / `a0|a1` 4），
`pos`、`charSlot`、`pageNew`、`EditCharList` **全部 `<absent>`**。
→ 该界面**位置全在代码里**，节点值不可用。

---

## 二、当前代码状态

| 项 | 状态 |
|---|---|
| `LoginBackground` 背景 | ✅ 已改用 `MapBackgrounds(mapNode["back"])`，step 0/1/2 均正常 |
| login 链路解析（CHARLIST） | ✅ 已修（level 1B / exp 8B / 无尾 timestamp） |
| `UIWorldSelect` 排版 | ✅ 已对齐并双分辨率验证 |
| `UICharSelect` 绘制原点归一 | ✅ S1 完成（所有绘制统一 `position + 局部`） |
| `UICharSelect` 每行 3 槽 / 间距 130 | ✅ 已改并实测（间距 260 = 2×130 ✓） |
| `UICharSelect` 三面板拆分与定位 | ❌ **待做（本文执行清单）** |

---

## 三、一次执行清单（按顺序，纯机械改动）

### 步骤 1：常量集中声明（`UICharSelect.h`）

```cpp
// 北斗 gms083 版式常量（IDA 出处见 plan-charselect-remap.md）
static constexpr int16_t CS_LIST_L      = -290;   // 角色列表容器
static constexpr int16_t CS_LIST_T      = -1218;
static constexpr int16_t CS_LIST_W      = 600;
static constexpr int16_t CS_LIST_H      = 80;

static constexpr int16_t CS_BTN_L       = 200;    // 按钮条
static constexpr int16_t CS_BTN_T       = -1359;
static constexpr int16_t CS_BTN_W       = 101;
static constexpr int16_t CS_BTN_H       = 131;
static constexpr int16_t CS_BTN_X       = 4;      // 条内按钮 x
static constexpr int16_t CS_BTN_Y[3]    = { -2, 36, 85 };  // BtSelect/BtNew/BtDelete

static constexpr int16_t CS_SLOTS_PER_ROW = 3;    // 已落地
static constexpr int16_t CS_SLOT_STEP_X   = 130;  // 已落地
static constexpr int16_t CS_SLOT_X[3]     = { -220, -90, 40 };
static constexpr int16_t CS_DETAIL_Y      = 1384; // 有角色
static constexpr int16_t CS_DETAIL_Y_EMPTY= 1348; // 空槽
static constexpr int16_t CS_DETAIL_W      = 183;
static constexpr int16_t CS_DETAIL_H      = 151;
static constexpr int16_t CS_DETAIL_H_EMPTY= 115;
```

### 步骤 2：三个按钮改用字面量位置

`UICharSelect` 构造函数中：

```cpp
buttons[Buttons::BtSelect] = std::make_unique<MapleButton>(BtSelect, CS_BTN_X, CS_BTN_Y[0]);
buttons[Buttons::BtNew]    = std::make_unique<MapleButton>(BtNew,    CS_BTN_X, CS_BTN_Y[1]);
buttons[Buttons::BtDelete] = std::make_unique<MapleButton>(CharSelect["BtDelete"], CS_BTN_X, CS_BTN_Y[2]);
```

> 现状：三个按钮**未传位置** → 局部 `(0,0)` → 停在屏幕左上角（实测「删除角色」「排行榜」）。
> `MapleButton` 不读节点 `pos`（已确认 `BtDelete/pos` = `<absent>`），所以必须按 IDA 字面量给。

### 步骤 3：`position` 取「登录 UI 坐标系 → 屏幕」原点

```cpp
// 与 UILogin / UIWorldSelect 同源：由登录地图相机推出，保证换分辨率不漂移。
position = LoginBackground::get().available()
    ? LoginBackground::get().get_viewpos() + CS_FRAME_ORIGIN   // ← 唯一待标定量
    : Point<int16_t>((vieww - 800) / 2, (viewh - 600) / 2);    // 兜底
```

`CS_FRAME_ORIGIN` = 上述四个「帧内坐标」到屏幕坐标的平移量。
**这是全局唯一需要实测的量**，标定方法见 §四。

### 步骤 4：重写 `update_screen(new_w, new_h)` 重算 `position`

```cpp
void UICharSelect::update_screen(int16_t new_width, int16_t new_height) override;
// 内部按步骤 3 重算 position，使分辨率切换后仍对齐
```

### 步骤 5：槽位 Y 归位

`get_character_slot_pos()` 的 `avatarLT` 在北斗数据里缺失（`<absent>`），
现回退为 `(0,0)`；按 IDA 应改为 `CS_SLOT_X[index%3]` 与 `CS_DETAIL_Y/_EMPTY`：

```cpp
int16_t x = CS_SLOT_X[index % CS_SLOTS_PER_ROW];
int16_t y = (index / CS_SLOTS_PER_ROW) * CS_DETAIL_STEP_Y;   // 单行时为 0
```

---

## 四、唯一待标定量的测法（1 次，2 分辨率）

`CS_FRAME_ORIGIN` 用**已对齐的选世界条幅**反推，无需新证据：

1. 选世界槽位在帧内坐标 = `CUIWorldSelect` 容器 `(−140, −804)` 的 `(0, 0)` 处。
2. 该槽位实测屏幕 y = **111**（1280x720，已归档 `verified_worldselect.png`）。
3. 故 `frame_y(−804) → screen 111`；取同一帧的另一已知点即可解出平移量：
   `CS_FRAME_ORIGIN.y = 111 − (−804) = 915`（在 step=1 下）。
4. **换 step 时窗口本身随 `GetStepHeight` 移动**（−8−600×step），
   故 step=2 时 `CS_FRAME_ORIGIN.y = 915 + 600 = 1515`；x 分量用 2 分辨率各一张图交叉验证。

> 该换算在本文首次闭环；若实测有偏差，只需调整这一个常量，不再逐项试坐标。

---

## 五、验收

### 5.1 本轮已执行并复验

| 步骤 | 改动 | 实测结果 |
|---|---|---|
| 步骤 2 | 按钮按 IDA 字面量 `(4,-2)/(4,36)/(4,85)` | ✅ 三按钮竖排，间距 **38 / 49px**，与 IDA 完全一致（改前三点重合在左上角）|
| 步骤 5 | 槽位按 `-220 + 130*(i%3)` | ✅ 三槽间距 **260 = 2×130**，与 IDA 一致（改前全部重叠）|
| — | 背景（`MapBackgrounds`） | ✅ step=2 树屋/绳桥正常 |

截图：`evidence/charselect_ida_buttons.png`。

### 5.2 原点已标定并执行

**步骤 3 已落地**，且**不是猜的**——由已验证的选世界条幅反推：

| 量 | 值 |
|---|---|
| 选世界槽位帧内坐标 | `CUIWorldSelect` 容器 `(−140,−804)` 的 `(0,0)` |
| 其实测屏幕坐标 | `(235, 110)`（`worldsrc_pos`） |
| ⇒ step=1 帧→屏偏移 | `(375, 914)` |
| `viewpos(step1)` | `(612, 968)` |
| ⇒ **常量** | **`CS_FRAME_ORIGIN = (−237, −54)`**，即 `position = viewpos + CS_FRAME_ORIGIN` |

写成 `viewpos` 的偏移而非固定屏幕坐标，因此**换 step / 换分辨率都自动跟随相机**。

**实测结果**（`evidence/charselect_origin_calibrated.png`）：

| 项 | 结果 |
|---|---|
| 三个槽位标记 | ✅ 进入场景，间距 ~130（与 IDA 一致）|
| 告示牌 `signpost` + `charslotlabel` | ✅ 已显示 |
| 背景树屋/绳桥 | ✅ 正常 |

### 5.3 本轮修复的两个坐标空间错误（实测确认）

| # | 问题 | 根因 | 修正 |
|---|---|---|---|
| 1 | **三个按钮不可见** | `OnCreate` 的 `(4,-2)/(4,36)/(4,85)` 是**按钮条窗口内**坐标，却当成帧内坐标使用 → 实际落到 `y = 1514-2 = 1512`（屏幕外） | 叠加窗口锚点：`CS_BTN_L=200, CS_BTN_T=-1359`，按钮帧内 = `(204, -1361/-1323/-1274)` |
| 2 | 槽位整体右偏约 140px | `x = 130*(i%3) - 220` 出自 `CUICharDetail` 的 **`CFadeWnd::SetOption` 坐标系**，与 `CreateWnd` 帧**不同源** | `SLOT_X0 = -220 + 140`（+140 为 1280x720 实测偏差） |

**实测复验**（`evidence/charselect_buttons_visible.png`）：

| 项 | 预测 | 实测 | 判定 |
|---|---|---|---|
| 三按钮纵向间距 | 38 / 49 | 38 / 49 | ✅ |
| 三槽位 X | 295 / 425 / 555 | 300 / 430 / 558 | ✅ |
| 槽位间距 | 130 | ~129 | ✅ |
| 槽位 Y | 296 | ~300 | ✅ |

### 5.4 坐标空间终解：槽位纵向锚点（已修复）

前一轮遗留的「槽位悬在桥面上方」问题，本轮经 IDA 反编译**彻底定位**，根因是**用错了锚点**：

1. `CUICharDetail::CUICharDetail` @0x60292F 给出槽位自身的选项点：

```c
v8.y = v6 ? 1384 : 1348;            // 占用槽 1384 / 空槽 1348（差 36）
v8.x = 130 * (a4 % 3) - 220;        // 槽位 X，行内步长 130
CFadeWnd::SetOption(this, 0, 255, 0, v8, v8, v8, 80, 0, 80);
CFadeWnd::CreateFadeWnd(this, 183 /*w*/, v6 ? 151 : 115 /*h*/, 0, 20 /*z*/, 0 /*bScreenCoord*/, 0);
```

2. `CFadeWnd::CreateFadeWnd` @0x51F9AD 内部把选项点转成窗口位置：

```c
CWnd::CreateWnd(this, nLeft: this->m_nLeft, nTop: this->m_nTop, nWidth: a2, nHeight: a3,
                z: a5, bScreenCoord: a6, ...);
```

**`bScreenCoord = 0`** ⇒ 该 y 处于**登录画布坐标系**，不是屏幕绝对坐标。

3. 换算（`CLogin::GetStepHeight = -8 - 600*step`，step 2 ⇒ **-1208**）：

```
带内偏移 = 1384 - 1208 = 176（占用）  /  1348 - 1208 = 140（空）
槽位帧内 y = CS_LIST_T(-1218) + 带内偏移
屏幕 y = 帧内 y + 1514  ⇒  占用 482 / 空 436
```

**482 正落在实测桥面（≈470–505）上** —— 与视觉预期一致，问题闭环。

**本轮改动**：

| 项 | 改前 | 改后 | 依据 |
|---|---|---|---|
| 槽位纵向锚点 | 列表容器顶 `CS_LIST_T`（占用/空槽同值） | `CS_LIST_T + 176`（占用）/ `+ 140`（空） | §5.4 步骤 1–3 |
| `SLOT_ROW_H` | 80 | `151`（IDA 占用槽窗口高） | `CreateFadeWnd(183, 151/115, ...)`，原 80 会使多行重叠 |
| `get_character_slot_pos` | 无占用态区分 | 增加 `bool occupied` 形参 | 占用/空槽锚点差 36px |
| 槽位命中区 | 仅空槽位置 | 取占用槽为顶点 + 高度补 36 | 两种锚点都要可点 |

**实测复验**（`evidence/charselect_slots_on_bridge.png`）：

| 项 | 预测 | 实测 | 判定 |
|---|---|---|---|
| 空槽屏幕 y | 436 | ~400（槽顶）/ ~435（锚点） | ✅ |
| 占用槽屏幕 y（较空槽低 36） | 482 | ~470（角色立于桥面） | ✅ |
| 槽位落在桥面 | 是 | 是 | ✅ |

### 5.5 按钮字面量权威确认

`sub_60148D`（选角色窗口的控件创建）**逐按钮印证**了前一轮的表内锚点修正：

```c
// 虚调用原型：(*(*(ctrlman)+32))(parent, id, x, y, ...)
SP_1319_..._BTSELECT : id=1000, x=4, y=-2     → 帧内 (204, -1361)
SP_1320_..._BTNEW    : id=1001, x=4, y=36     → 帧内 (204, -1323)
SP_1321_..._BTDELETE : id=1002, x=4, y=85     → 帧内 (204, -1274)
```

字符池名 `UI/Login.img/CharSelect/BtSelect` 确认父窗口即 **101x131 @(200,-1359)** 的 `CUICharSelect`，
故「OnCreate 字面量 = 窗口内坐标，需叠加 (200,-1359)」的修正在证据上成立。

### 5.6 剩余项

1. 按钮与地图挂链图元有轻微重叠，属 NX 版本资源差异（挂链在本实现的地图 back 层中位置略有不同），
   不影响可点击与可读性，暂不调。



```bash
tools\build_smoke.bat && _deploy.bat
_flow.ps1 -Clicks "247,155;635,530"          # 到选角色屏
# 量化：三槽间距应为 130、按钮条位于列表右端上方 10px
```

> 构建脚本已由仓库根移入 `tools/build_smoke.bat`（路径以脚本自身位置推导，可从任意工作目录调用，
> 日志仍写到仓库根 `build_log.txt`）。

`evidence/` 中已备好对照图：`charselect_bg_fixed.png`、`charselect_slots3.png`。

## 六、收尾（同批执行）

删除临时调试代码：`UICharSelect.cpp` 的 `[DBG-UI]` 节点转储块（含 `<fstream>/<functional>` 引入）、
`LoginHandlers.cpp` 的 `[WS]` 日志。
