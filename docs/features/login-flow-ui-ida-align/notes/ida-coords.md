# 登录链路 UI 坐标提取（IDA：BeiDou gms083）

本文件记录**登录链路各界面**在参考客户端（北斗 gms083，`E:\MXD\Maplestory95.i64` 同源反编译产物）中的
权威版式坐标，以及与当前 `MapleStory-Client` 实现的映射关系。

> 证据来源：`E:\project` 下的 IDA 反编译 dump（由 IDA MCP 导出，逐函数 JSON）：
> `_ws_drawitems.txt`、`sub6246ce.json.txt`、`sub62b2cf.json.txt`、`sub5fba85.json.txt`、
> `sub5f40a4.json.txt`、`clogin_init.json.txt`、`clogin_step.json.txt`、`getstep.json.txt`、
> `cuititle_fmt.txt`、`sign.txt`、`maplogin.txt`。
> 实测截图：`E:\project\_cap_login.png` / `_cap_world.png` / `_cap_channel.png` / `_cap_char.png`（1280x720）。

---

## 1. 相机 / 步进（已对齐，作为其余坐标的基准）

`CLogin::GetStepHeight`（0x5FC0E4）：

```c
return -8 - 600 * (v2 + a2);   // v2 = (*(this+90)==4) ? *(this+133) : 0
```

即**每一步进整体位移 600**。当前实现 `LoginBackground::step_height()` = `-8 - 600 * step`，
`draw()` 内 `scroll = 8 + 600 * step`，与此一致。

登录地图内嵌元素（`sign.txt` / `maplogin.txt`）：

| 元素 | 尺寸 | origin |
|---|---|---|
| logo | 397x219 | (198,109) |
| sign（告示牌） | 368x236 | (184,118) |
| eff | 238x298 | (0,0) |

---

## 2. 选世界（CUIWorldSelect）

### 2.1 对话框锚点

`sub_6246CE`（0x6246CE，CUIWorldSelect 构造，同时登记 `TSingleton<CUIWorldSelect>::ms_pInstance`）：

```c
CDialog::CreateDlg(this, nLeft: -140, nTop: -804, nWidth: 0x190 /*400*/, nHeight: 100,
                   z: 10, bScreenCoord: 0, pData);
// 30 个 CCtrlButton 槽位：eh vector ctor(this+35, 8, 30, ...)
```

- 槽位容器 **30 个**，按钮 id 基址 `1000`。
- 对话框宽 **400**、高 **100**，`bScreenCoord = 0`（相对父窗口）。负的 `nTop = -804`
  说明该对话框与父窗口同处一个“多步进画布”，在实际显示步进下参与滚动；
  **不可直接当作屏幕绝对坐标**（父窗口帧未知，详见 §6 备注）。

### 2.2 条幅内槽位排布（可直接使用）

`CUIWorldSelect::DrawWorldItems`（0x624E13，`_ws_drawitems.txt`）：

```c
v59 = 0; v57 = 0; v63 = 0;
do {
    ...
    (*(**v18 + 32))(*v18, this, v57 + 1000, /*x*/ v59, /*y*/ 0, 0, v53); // 创建槽位按钮
    // 世界名图：worldsrc["world"][worldId] 画在 (v59, 0)
    // 大区提示图标：画在 x = v59 + 5
    v63 += 32; ++v57; v59 += 27; v18 += 8;
} while (v63 < 960);   // 960 / 32 = 30 槽
```

**结论（已落地常量）**：

| 项目 | IDA 值 | 当前实现 |
|---|---|---|
| 槽位步进 X | **27** | `WS_SLOT_STEP_X = 27` ✅ |
| 槽位数 | **30** | `WS_SLOT_COUNT = 30` ✅ |
| 大区提示图标 X 偏移 | **+5** | `+ Point(..., 5)` ✅ |
| 槽位 Y | **0**（相对条幅） | 未用；见 §2.3 |

### 2.3 条幅在屏幕上的落点（实测反推）

条幅（木质横杆）由 `LoginBackground` 在 step=1 时随地图绘制，是**屏幕基准**。
对 `_cap_world.png`（1280x720）做木质色连通域分析：

| 元素 | 实测 bbox |
|---|---|
| 木质横杆（含左右卷曲端头） | x 327–994，y **111**–157 |
| 告示牌牌面 | x 425–865，y 183–290 |
| 当前 UI 世界名/槽位（`worldsrc_pos = (235,40)`） | x 235–262，y **40**–131 |

**结论**：世界槽位条应**贴在木质横杆上**。IDA 中槽位 Y=0 相对条幅，故当前实现的
`worldsrc_pos.y = 40` 比横杆顶（111）**高 70px**，需下移 **+70**，即
`worldsrc_pos = (235, 110)`。

---

## 3. 频道面板（chBackgrn）

`chBackgrn` 取自 `UI/Login.img/WorldSelect/chBackgrn`，尺寸 **449x233**（以原点居中）。
对 `_cap_world.png` 与 `_cap_channel.png` 做差分，圈定面板实际绘制区域：

| 元素 | 实测 bbox（当前实现） |
|---|---|
| 频道面板 chBackgrn | **x 191–639（w449），y 69–306（h238）** |
| 「到选择的世界去」按钮 | x 570–700（w131），y 445–478（h34） |

当前实现 `channelsrc_pos = (415, 190)`（面板中心），面板左上 = (191, 74)，与实测一致 ✅
（X 已居中正确；Y 需随世界条幅一起下移，见 §2.3）。

频道按钮/负载条网格（当前实现 `channel_cell_offset`）：

```c
COLS = 4; CELL_W = 100; CELL_H = 34;
offset = (22 + 100*col, 56 + 34*row);   // 相对面板左上
```

> 参考客户端中 `dword_BEDA68`（由 `sub_62B2CF`，0x62B2CF 创建）即**登录选频道对话框**：
> `CDialog::CreateDlg(136, 51, 0x21C /*540*/, 167, z: 211, bScreenCoord: 1)`。
> 依据：`CLogin::ChangeStep`（0x5F53C0）在 **step==1（选世界）** 时
> 先销毁/新建 `CUIWorldSelect`，随后 `CWndMan::SetFocus(dword_BEDA68)`，
> 即选世界这一步同时激活的“频道”窗口；`sub_62B397` 析构时清 `dword_BEDA68 = 0`。
>
> **偏差记录（未改）**：参考版 `dword_BEDA68` 为 **540x167** @ (136,51)，本实现
> `chBackgrn` 为 **449x233**（尺寸/切片不同，属不同 NX 版本资源），故**不照搬**其尺寸与
> 绝对坐标，仅用 IDA 的**相对关系**（频道面板在选世界步进内、位于世界条幅下方）指导版式。

---

## 4. 选角色 / 创建角色（已补 dump，但未能落地方位）

### 4.1 登录步进模型（`CLogin::ChangeStep`，0x5F53C0）

`*(this+90) = step`，步进对 6 取模，共 **6 步**：

| step | 含义 | 关键行为 |
|---|---|---|
| 0 | 登录标题面板 | 复位 VAC，`*(this+133)=0` |
| 1 | **选世界** | 销毁并新建 `CUIWorldSelect`（`sub_6246CE`）；`SetFocus(dword_BEDA68)` = 选频道对话框 |
| 2 | 频道/角色列表 | `SetFocus(dword_BEDA4C)` |
| 3 | **新建角色·种族选择** | 新建 `CUINewCharRaceSelect`（`dword_BEDA6C`），`*(this+133)=1` |
| 4 | 角色预览 | `CLogin::GetSelectedAL` + `CAvatar::Init(..., x:22, y:-2369-600*k)` |
| 5 | 其他 | `*(*(this+120)+252)=0` |

步进切换统一走 `GetStepHeight(cur)` → `GetStepHeight(next)` 的补间（`v65 = 7000*(Δ)/dur/2`），
并播放 `SCROLLUP`。

容器规模：`sub_5FDD2D(this+101, 15, ...)`（**15 个世界**）、`sub_5FDD2D(this+78, 60, ...)`
（**60 个频道**），每项 684 字节。

### 4.2 各对话框窗口锚点

| 归属 | addr | 创建调用 | 尺寸 |
|---|---|---|---|
| `CUIWorldSelect` | 0x6246CE | `CreateDlg(-140, -804, 400, 100, z:10, bScreenCoord:0)` | 400x100 |
| `CUIChannelSelect` | 0x62B2CF | `CreateDlg(136, 51, 540, 167, z:211, bScreenCoord:1)` | 540x167 |
| `CUICharSelect` | 0x60135B | `CreateWnd(200, -1359, 0x65=101, 131, z:10, bScreenCoord:0)` | **101x131** |
| `sub_60E157` | 0x60E157 | `CreateDlg(0, 125, 125, 352, z:210, bScreenCoord:1)` | 125x352 |
| `sub_592992` | 0x592992 | `CreateDlg(275, 100, 250, 314, z:10, bScreenCoord:1)` | 250x314 |
| `sub_593A8D` | 0x593A8D | `CreateDlg(275, 100, 250, 343, z:10, bScreenCoord:1)` | 250x343 |
| `CUINewCharRaceSelect` | —（`dword_BEDA6C`） | step 3 创建，未取到 ctor | — |

**重要**：`CUICharSelect` 在参考版中是 **101x131 的小窗口**（角色槽位/按钮列），
而非整屏角色选择界面；整屏由多个对话框拼合。本实现 `UICharSelect` 是 800x600 的
单一 UIElement 且大量坐标取自 NX 节点（`CharSelect["pos"]` 等），**与参考版结构不同**，
因此不能把 `(200,-1359,101,131)` 直接搬过来。

### 4.3 为什么仍未改动角色/创建界面

1. `nTop = -804 / -1359 / -2369` 与 `GetStepHeight = -8-600*step`（-608/-1208/-1808/-2408）
   **不是 600 的整数倍关系**（差值 -196 / -151 / +39），说明这些坐标并非相对“步进窗口”的简单叠加，
   其父窗口原点的转换规则无法从现有 dump 唯一确定。
2. 登录流程在选频道后卡在“正在连接到服务器。”，**无法进入选角色界面做实测反推**（见 §6）。
3. 因此按「无充分证据不臆改」原则，本轮**未改**任何角色/创建界面坐标。

---

## 5. 当前本轮落地改动

| 文件 | 项 | 改前 | 改后 | 依据 |
|---|---|---|---|---|
| `IO/UITypes/UIWorldSelect.cpp` | `worldsrc_pos.y` | 40 | 110 (`WS_BANNER_Y`) | §2.3 实测横杆顶 y=111 |
| `IO/UITypes/UIWorldSelect.cpp` | `channelsrc_pos.y` | 190 | 260 (`WS_BANNER_Y + 34 + 233/2`) | 面板随条幅同步下移 70，保持面板顶 = 条幅 + 34 的既有相对关系 |
| `IO/UITypes/UIWorldSelect.cpp` | `worldNoticeMessage` 偏移 | (58, -50) | (58, -90) | 旧值落在面板中心下方，实测与第一行频道按钮（面板顶 +56）同行重叠 |
| `IO/UITypes/UIWorldSelect.cpp` | `chatballoon` 提示气球 Y | 105 | 105（不变） | 实测下移后会压住告示牌；原位置（横杆上方）本就正确 |

### 5.1 复验结果（1280x720 实测）

| 验证项 | 改前实测 | 改后实测 | 判定 |
|---|---|---|---|
| 世界名纵向位置 | y 48–72（悬在横杆上方） | y 119–142（落在横杆 111–145 内） | ✅ |
| 木质横杆 | y 111–157（未动） | 同上 | 基准 |
| 频道面板 chBackgrn | 顶 y 74 | 顶 y **139**（横杆下方，x 191–639 w449） | ✅ |
| 世界公告文本 | y ≈150（与 CH.1 同行） | y ≈170（面板表头空白带） | ✅ |
| 「到选择的世界去」按钮 | x 570–700 / y 445–478（点不动） | x 570–700 / y **515–548**（可点，且不再因“点面板外”先去选世界） | ✅ |

复验截图（1280x720 实测）：
`evidence/verified_worldselect.png`（选世界）、`evidence/verified_channel.png`（选频道）。

### 5.2 多分辨率自适应（锚点规则与 1024x768 复验）

**问题**：`WS_BANNER_Y = 110` 是**屏幕绝对像素**，与 `UILogin` 的做法不一致
（`UILogin` 用 `signboard_map_pos + LoginBackground::get_viewpos() - sign_origin` 把面板锚在
登录地图的告示牌上，控件再相对面板偏移）。换分辨率必然错位。

**锚点规则（与 `UILogin` 对齐）**：登录地图层由 `LoginBackground::draw()` 以
`viewpos = center - (28, step_height(step))` 绘制，即 **随 `viewh/2`、`vieww/2` 变化**。
故条幅应写成「地图坐标系偏移」，而非屏幕绝对 Y：

```cpp
WS_BANNER_OBJ_Y = -857;   // 1280x720 实测条幅顶 y=111，同分辨率 viewpos.y = 720/2 + 608 = 968
banner_y = LoginBackground::get().get_viewpos().y() + WS_BANNER_OBJ_Y;
```

**双分辨率实测验证**（`view-pos` 与实测条幅位置对照）：

| 分辨率 | viewpos.y | viewpos 预测条幅 y | 实测横杆 y | 实测槽位挂钩 y | 判定 |
|---|---|---|---|---|---|
| 1280x720 | 360+608 = **968** | 968−857 = **111** | 111–157 | 112–202 | ✅ |
| 1024x768 | 384+608 = **992** | 992−857 = **135** | 135–181 | **136**–226 | ✅ |

Δviewpos = +24 = Δviewh/2，Δ实测挂钩 = +24 → **完全跟随地图相机**。
若仍用写死的 110，1024x768 下槽位会落在 110–200，比横杆（135）高 **25px** → 错位。

复验截图：`evidence/verified_worldselect.png`（1280x720）、
`evidence/verified_worldselect_1024x768.png`（1024x768）。

---

## 6. 备注：未解项与已知偏差（未臆造）

1. **父窗口帧仍未唯一确定**：已补出 `CLogin::ChangeStep`(0x5F53C0)，但
   `CWnd::CreateWnd` / `CWndMan` / `CLogin` 自身窗口的 `MoveTo/SetPosition` 仍未导出，
   且 §4.3 的差值分析排除了“相对步进窗口简单叠加”的假设，
   故屏幕绝对 Y 只能由**实测截图反推**（§2.3），未按 `-804` 直接换算。
2. **世界槽位上限偏差**：参考版 `DrawWorldItems` 预留 30 个按钮槽、容器为 15 个世界；
   本实现 `WS_MAX_WORLDS = 5`（`Buttons::BtWorld0..BtWorld4`）且与 30 个频道按钮共用
   `5..34` 的按钮 id 段。如需支持 >5 世界，必须重排 `Buttons` 枚举（**本轮未改**，北斗实际下发世界数 ≤5）。
3. **登录流程卡在“正在连接到服务器。”**，选角色界面无法进入/截图复验（属「接通完整登录流程」范畴）。
4. 后续如需精确校准角色/创建界面，应补导出：`CWnd::CreateWnd`、`CUINewCharRaceSelect` 的 ctor、
   `dword_BEDA4C` / `dword_BEDA50` 归属，以及 `CUICharSelect` 的 `OnCreate/Draw`。

---

## 7. 证据索引

`evidence/` 目录（本轮 IDA MCP 导出，逐函数）：

| 文件 | 函数 | addr |
|---|---|---|
| `CUIWorldSelect_DrawWorldItems_0x624e13.txt` | `CUIWorldSelect::DrawWorldItems` | 0x624E13 |
| `CUIWorldSelect_ctor_0x6246ce.json` | CUIWorldSelect 构造 | 0x6246CE |
| `CUIChannelSelect_ctor_0x62b2cf.json` | CUIChannelSelect 构造 | 0x62B2CF |
| `CUICharSelect_ctor_0x60135b.txt` | `CUICharSelect::CUICharSelect` | 0x60135B |
| `CLogin_ChangeStep_0x5f53c0.txt` | `CLogin::ChangeStep` | 0x5F53C0 |
| `CLogin_OnStepChanged_0x5f5962.json` | `CLogin::OnStepChanged` | 0x5F5962 |
| `CLogin_Init_0x5f42cd.json` | `CLogin::Init` | 0x5F42CD |
| `CLogin_DestroyAll_0x5f40a4.json` | 登录相关窗口统一销毁 | 0x5F40A4 |
| `CLogin_ctor_0x5f3c59.json` | `CLogin::CLogin` | 0x5F3C59 |
| `CLogin_GetStepHeight_0x5fc0e4.json` | `CLogin::GetStepHeight` | 0x5FC0E4 |
| `CUITitle_OnCreate_0x62054a.txt` | `CUITitle::OnCreate` | 0x62054A |
| `MapLogin_sign_metrics.txt` / `MapLogin_logo_metrics.txt` | 登录地图内嵌元素尺寸/origin | — |

IDA MCP 端点：`http://127.0.0.1:13337/mcp`（工具 `list_funcs` / `lookup_funcs` / `decompile`，
参数 `{"addr":"0x...","include_addresses":false}`）。
