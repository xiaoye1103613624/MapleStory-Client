# 选角色界面真正根因：`Login.img/CharSelect` 节点 schema 不匹配

> 本文修正 `ui-positioning-scheme.md` 中「控件混用 `position`」的判断：
> 那只是**表象**，根因是**节点名对不上**，导致所有布局查询取到 `<absent>`。

## 一、实测节点转储（北斗 gms083 真实数据）

在 `UICharSelect` 构造函数 `[DBG-UI]` 临时块中打印（已抓取，见 `evidence/charselect_node_dump.txt`）：

```
CharSelect children:
  BtDelete BtNew BtSelect adventure aran charInfo charInfo1 charInfo2 charInfo3
  character effect icon knight nameTag pageL pageR scroll
```

而 `UICharSelect` 正在查询的路径**全部不存在**：

| 客户端查询路径 | 实测 |
|---|---|
| `CharSelect["pos"]`（及其 `avatarPos` / `detailPos`） | **children 为空** |
| `CharSelect["charSlot"]["pos"]` / `worldInfoLT` / `worldInfoRB` | `<absent>` |
| `CharSelect["pageNew"]["pos"]` / `numberpos` | `<absent>` |
| `CharSelect["pos"]["avatarPos"]["avatarLT"]` / `avatarRB` / `avatarSpace` | `<absent>` |
| `CharSelect["pos"]["detailPos"]["jobPos"]` / `levelPos` / `namePos` | `<absent>` |
| `CharSelect["EditCharList"]["BtCharacter"]` | children 为空 |
| `BtSelect/pos`、`BtNew/pos`、`BtDelete/pos`、`pageL/pos`、`pageR/pos` | 全 `<absent>` |
| `Common["BtChangePIC"]["pos"]`、`Common["BtPreview"]["pos"]` | `<absent>` |

**存在**的节点：`BtDelete`/`BtNew`/`BtSelect`（只有 `normal/pressed/mouseOver/disabled/keyFocused`）、
`pageL`/`pageR`、`scroll`、`character`、`effect`、`icon`、`nameTag`、
`charInfo`/`charInfo1`/`charInfo2`/`charInfo3`、`adventure`/`aran`/`knight`。

## 二、结论（两条，均可作为开发依据）

1. **必须做 schema 重映射**：把 `UICharSelect` 中所有 `CharSelect[...]` 路径改到上表真实节点名。
   在此之前，「读节点」这条路是**取不到值**的——这也是选角色界面控件全堆在左上角/错位的真正原因
   （所有 `pos` 查询返回空，退化为 `(0,0)` 或空纹理）。
2. **按钮位置不在节点里**：`BtDelete/pos` 等全部 `<absent>`，与 IDA 一致——
   原版 `CUICharSelect` 的按钮位置是**代码里给的**（`CWnd::CreateWnd(..., UIOrigin)` +
   代码内字面量），所以这部分只能从 **IDA 反编译**取，不能靠节点。

## 三、对「减少测试量」的回答

**成立，但前提是先做完 schema 重映射**。映射完成后：

| 坐标来源 | 覆盖范围 | 是否需截图校准 |
|---|---|---|
| NX 节点值（`pos`/`LT`/`RB`/`origin`） | 大部分面板、背景、图标、页码 | 否 |
| IDA 代码字面量 + `UIOrigin` 锚点 | 按钮、槽位网格、子窗口偏移 | 否 |
| **登录 UI 坐标系 → 屏幕原点的单一偏移** | 全局仅 1 个 | 是（1 次，2 个分辨率各 1 张） |

即测试量从「逐项截图逼近」降为「**1 次整体验证**」。

## 四、下一步（按优先级）

1. **补全节点转储**：打印 `scroll` / `character` / `effect` / `icon` / `nameTag` /
   `charInfo1..3` / `adventure` / `aran` / `knight` 的 children 与其中的 `pos`/`origin` 值
   ——这是重映射的输入。
2. **从 IDA 取代码侧布局**：`CUICharSelect` 的 `OnCreate` / `Draw`
   （`list_funcs "*CUICharSelect*"` 只返回 4 个函数：ctor / OnButtonClicked / SetKeyFocus /
   IsRequestValid，说明 `OnCreate`/`Draw` 未被命名或内联，需要按 xref / 虚表定位）。
3. **执行 schema 重映射**，再按上表做 1 次整体验证。

> 注意：`UICharSelect.cpp` 中的 `[DBG-UI]` 转储块与 `LoginHandlers.cpp` 的 `[WS]` 日志
> 均为临时调试代码，**功能完成后必须删除**（已记入 cleanup 待办）。

---

## 五、递归转储后的决定性结论：此界面的节点里**没有布局数据**

对 `scroll`/`character`/`effect`/`icon`/`nameTag`/`charInfo1..3`/`adventure`/`aran`/`knight`/
`BtSelect`/`BtNew`/`BtDelete`/`pageL`/`pageR` 做递归转储（234 行，见
`evidence/charselect_node_dump_full.txt`），叶子名统计：

| 叶子名 | 出现次数 | 含义 |
|---|---|---|
| `origin` | 92 | 精灵原点（点） |
| `z` | 83 | 图层深度 |
| `delay` | 49 | 动画帧延时 |
| `a0` / `a1` | 3 / 1 | 透明度 |

**没有任何 `pos` / `LT` / `RB` / `space` 之类的布局节点。**

即：北斗 `Login.img/CharSelect` 在数据层**只有美术资源**（帧动画 + 原点 + 层级），
`pos` / `charSlot` / `pageNew` / `EditCharList` 这些布局容器**根本不存在**。

### 对「用节点属性减少测试」的回答（修正）

- `UIWorldSelect`（选世界/频道）：**成立**——布局基本由 IDA 字面量 + 少量节点决定，已用实测闭环验证。
- `UICharSelect`（选角色）：**不成立**——该界面节点里没有布局，**所有位置都在原版代码里**
  （`CUICharSelect` 的 `CreateWnd(..., UIOrigin)` + 绘制代码内字面量）。
  这与会话早期从 IDA 得到的 `CUICharSelect` ctor（`CreateWnd(200, -1359, 0x65, 131, z:10, 0)`）
  互相印证：位置是**代码给定**的。

### 因此下一步必须转向 IDA（唯一权威来源）

1. 在 IDB 中定位 `CUICharSelect` 的 **`OnCreate` / `Draw`**（`list_funcs "*CUICharSelect*"` 只给出
   4 个函数：ctor / `OnButtonClicked` / `SetKeyFocus` / `IsRequestValid`，说明这两个未被命名或内联），
   方法：从 `off_AF6D90`（ctor 里写的 vtable）沿虚表找 `Draw@PBUtagRECT@@` 槽位，
   或用 `xref` 从 `CUICharSelect::OnButtonClicked` 反向定位同族函数。
2. 从 `Draw` 中提取每个精灵的绘制坐标字面量（`charSlots`、`charInfo`、`pageL/R`、三个按钮…），
   以及 `CLogin::ChangeStep` step=2 分支里对窗口的定位。
3. 这些字面量 + 唯一的一个「UI 坐标系→屏幕」原点是**全部**待定值；节点侧无需再查。

---

## 六、IDA 已定位：选角色屏的**窗口构成**与按钮坐标

### 6.1 vtable 解引用（`off_AF6D90` = `CUICharSelect` vtable）

| slot | 值 | 说明 |
|---|---|---|
| 0 | `loc_6017F0` | CUICharSelect 私有 |
| 1/2 | `CWnd::OnDragDrop` / `CWnd::PreCreateWnd` | **未重写** |
| 3 | **`sub_60148D`** | **`OnCreate`**（`CreateWnd` 内 `(*(*this+12))(this,pData)` 调的就是它） |
| 8 | `CUICharSelect::OnButtonClicked` | |
| 11 | `CWnd::Draw` | **未重写** → 本类不自己画 `CharSelect/*` 美术 |

### 6.2 按钮坐标（`CUICharSelect::OnCreate` @0x60148D 实测字面量）

```c
CCtrlButton::Create(..., id:1000 /*BtSelect*/, x:4, y:-2,  ...);
CCtrlButton::Create(..., id:1001 /*BtNew*/,    x:4, y:36,  ...);
CCtrlButton::Create(..., id:1002 /*BtDelete*/, x:4, y:85,  ...);
```

与本类 ctor 的 `CreateWnd(nLeft:200, nTop:-1359, nWidth:0x65 /*101*/, nHeight:131, z:10, 0)`
**完全自洽**：101×131 的窄条，三个按钮贴左沿（x=4）竖直排列。

### 6.3 选角色屏 = 多个窗口拼合（由全局对象 xref 定位）

| 全局 | 类 | ctor | 承担内容（推断） |
|---|---|---|---|
| `dword_BEDA4C` | 待定（写点在 `0x601790` 所在函数 / `sub_603FF0` / `sub_60418B`） | — | 角色列表/槽位面板（`scroll`/`character`/`effect`/`pageL`/`pageR`） |
| `dword_BEDA50` | **`CUICharDetail`** | `0x602960` | 角色详情面板（`charInfo1/2/3`） |
| `dword_BEDA54` | **`CUICharSelect`** | `0x60135B` | 101×131 竖直按钮条（`BtSelect`/`BtNew`/`BtDelete`） |
| `dword_BEDA48` | `CUINewCharNameSelectCygnus` | `0x61775B` | 新建角色取名（后续步骤） |

**关键含义**：本项目的 `UICharSelect` 把上述「列表面板 + 详情面板 + 按钮条」**三合一塞进了一个
800×600 的 UIElement**，并沿用 GMS 的 `CharSelect/pos`、`charSlot`、`pageNew` 等**不存在的节点名**
—— 所以它既取不到节点值，又没有对应原版的窗口划分，这才是错位的根因。

**正确的重映射方向**：按上表把这一个 UIElement **拆回三个逻辑面板**，各自按 IDA 字面量定位：

- 按钮条：101×131，按钮 (4,−2) / (4,36) / (4,85) ← **已拿到确切值**
- 详情面板 `CUICharDetail`：待取 ctor `0x602960` 的 `CreateWnd` 锚点 + 其 `OnCreate` 字面量
- 列表面板 `dword_BEDA4C`：待定位类名与 ctor

### 6.4 证据

- `evidence/CUICharSelect_OnCreate_0x60148d.txt`（含上述按钮坐标字面量）
- `evidence/CUICharSelect_ctor_0x60135b.txt`（`CreateWnd(200,-1359,101,131)`）
- `evidence/CUICharDetail_Draw_0x602adc.txt`（详情面板绘制函数全文）
- 全局对象 xref 表（见本文 §6.3，可用 `py_eval` + `idautils.XrefsTo` 复现）

---

## 七、`CUICharDetail` 几何（已取到）

ctor `0x602960`（`dword_BEDA50`）：

```c
a1[53] = a3;                                   // 世界/角色数据源
a1[54] = a4;                                   // 槽位序号
v6 = (*v5 != 0 && v5[2] != 0);                 // 该槽位是否有角色
a1[57] = v6;
v8.y = v6 ? 1384 : 1348;                       // 有角色 / 空槽位 两种 Y
v8.x = 130 * (a4 % 3) - 220;                   // ← 每行 3 个，间距 130
CFadeWnd::SetOption(a1, 0, 255, 0, v8, v8, v8, 80, 0, 80);
CFadeWnd::CreateFadeWnd(a1, 183, v6 ? 151 : 115, 0, 20, 0, 0);
```

**可直接使用的常量**：

| 项 | 值 |
|---|---|
| 每行槽位数 | **3**（`a4 % 3`）——**本实现用的是 6，需改** |
| 槽位横向间距 | **130** |
| 三个槽位 X（相对） | **−220 / −90 / +40** |
| 详情面板尺寸 | **183 × 151**（有角色）/ **183 × 115**（空槽） |
| Y（相对） | **1384**（有角色）/ **1348**（空槽） |

绘制函数：slot 11（`Draw` 槽）= **`sub_602ADC`**（与 `CUICharSelect` 的槽位规律一致，
其 slot 11 为 `CWnd::Draw` 表示未重写）。

> 待取：`sub_602ADC` 函数体内各精灵的坐标字面量（全文已归档，含大量强转声明，
> 下次可直接在其中检索 `DrawCanvas` / `DrawText` 调用处的常数）。

### 7.1 追加进展

- **`sub_602ADC` 函数体开头**（已取到）：
  ```c
  CWnd::GetCanvas(&v239);
  v3 = *(*(this + 53) + 404) + 684 * *(*(this + 53) + 400);
  v230 = v3;                      // 当前槽位的角色数据指针
  ```
  **`684` 字节/槽位** —— 与 `CLogin::ChangeStep` 里
  `sub_5FDD2D(this+101, 15, ...)` / `sub_5FDD2D(this+78, 60, ...)`
  的「每项 684 字节」（见 `ida-coords.md` §4.1）**完全一致**，互相印证：
  角色数据以 684 字节为单位，`+400` 是槽位索引、`+404` 是数组基址。

- **`dword_BEDA4C` 的写入点**：
  | 地址 | 所属函数 |
  |---|---|
  | `0x601790` | **IDA 未识别所属函数**（`get_func` 返回空，疑为未定义代码/数据段） |
  | `0x60401A` | `sub_603FF0` |
  | `0x60B04B` | `sub_603FF0` |
  | `0x6042BC` | `sub_60418B` |

  即列表面板的类名尚未确定，下一步应在 `sub_603FF0` / `sub_60418B` 中定位
  「`dword_BEDA4C = a1` 且紧邻 `CWnd::CreateWnd`」的构造函数。
