# 收尾与加固（第二轮）技术笔记

> 承接 [`ida-coords.md`](ida-coords.md) 与 `plan-charselect-remap.md`。
> 本轮聚焦：越界防御、容量对齐、两处经验常量的收尾、仓库卫生。
> **原则不变：每处改动都必须有 IDA 字面量或实测反推作依据；无依据的一律保留原值并标注待定。**

---

## 1. 世界 id / 频道越界缺陷（已修复）

### 1.1 缺陷定性（先证伪「数组越界」的直觉，再定性真实后果）

`IO/UIElement.h`：

```cpp
std::map<uint16_t, std::unique_ptr<Button>> buttons;   // 是 map，不是数组
```

⇒ `buttons[BtWorld0 + worldid]` 越界时**不会**数组越界，而是 `std::map::operator[]`
**静默插入一个空 unique_ptr**；后果分两种：

| 越界落点 | 后果 |
|---|---|
| 命中频道/进入/退出按钮的 id 段（5..34、35、36） | 静默误改那些按钮的状态（界面错乱） |
| 完全不存在（如 `worldid >= 37`） | 插入空项后紧接 `->set_state()` → **空指针解引用崩溃** |

另一种是**真数组越界**：`Gauge channel_gauge[BtGoWorld - BtChannel0]`（定长 30），
`change_world` 里 `for (i < selectedWorld.channel_count)` 未加限幅，`channel_count > 30`
时是**真实的内存破坏**；同处 `channel_capacities[i]`（变长 vector）也可能越界读。

### 1.2 修复方式

- 新增 `get_world_button(index)` / `get_channel_button(index)`：越界返回 `nullptr` 并按
  「同类 + 同 id 只告警一次」限频打印 WARN（`std::cerr`，ASCII 文案）；
  内部用 `map::find` 而非 `operator[]`，从根本上消除静默插入。
- `navigable_world_count()` = `min(worldcount, WS_MAX_WORLDS)`，供键盘导航与回车使用。
- `change_world` 的频道循环按 `min(channel_count, CHANNEL_BUTTON_COUNT, channel_capacities.size())` 收敛。
- `add_world` 超容量时告警并**不入列**，以维持 `worldcount == worlds.size()` 不变量。
- 构造函数里给按钮 map 的赋值仍用 `operator[]`（那是**创建**语义，且循环边界本身就受容量约束）。

> 日志文案为何用 ASCII：本仓库未启用 `/utf-8`，中文宽字节序列在错误代码页下会被误解析
> （尾字节 `0x5C` 转义引号 → `C2001: 常量中有换行符`）。中文说明一律写在注释里。

### 1.3 容量对齐参考版（15 世界 / 30 频道）

`Buttons` 枚举改为**表达式推导**，杜绝历史上「`BtChannel0` 隐式=5、`BtGoWorld` 硬编码=35」
这种与枚举顺序彼此独立写死的耦合：

```cpp
static constexpr uint16_t WORLD_BUTTON_COUNT = 15U;    // 参考版容量
static constexpr uint16_t CHANNEL_BUTTON_COUNT = 30U;

enum Buttons : uint16_t
{
    BtWorld0, ... , BtWorld14,
    BtChannel0 = BtWorld0 + WORLD_BUTTON_COUNT,
    BtGoWorld  = BtChannel0 + CHANNEL_BUTTON_COUNT,
    BtRegion, BtExit
};
```

`Gauge channel_gauge[BtGoWorld - BtChannel0]` 是差值表达式，自动跟随，无需改动。
`WS_SLOT_COUNT = 30`（版式槽位）与 `WS_SLOT_STEP_X = 27` 未动 —— 参考版
`DrawWorldItems` 同样预留 30 槽、15 世界。

**复验**：`evidence/worldselect_expanded_capacity.png`、`evidence/chain_verify_after_world_guard.png`
（世界→频道→选角色全链路点击通过；正常世界数下行为与日志输出零变化）。

---

## 2. 槽位横向偏移：`+140` 固化为具名常量

### 2.1 已确证的前提

`CUICharDetail::CUICharDetail`(0x60292F)：

```c
v8.y = v6 ? 1384 : 1348;       // 占用 / 空
v8.x = 130 * (a4 % 3) - 220;   // a4 = 角色绝对索引
CFadeWnd::SetOption(this, 0, 255, 0, v8, v8, v8, 80, 0, 80);
CFadeWnd::CreateFadeWnd(this, 183, v6 ? 151 : 115, 0, 20, 0 /*bScreenCoord*/, 0);
```

`bScreenCoord = 0` ⇒ 该点处于**登录画布坐标系**（非屏幕绝对）。

### 2.2 横向原点无法从现有 dump 反推（穷尽的可达路径）

| 尝试 | 结果 |
|---|---|
| `CLogin::Init`(0x5F42CD) | **不直接创建任何窗口**（dump 内无 `CreateWnd`/`CreateDlg`） |
| `CWndMan::UpdateWindowPosition`(0x9E03A6) | 仅 `RemoveWindow` + `InsertWindow` **重排 z 序**，无坐标换算 |
| `CWndMan::GetOrgWindow`(0x48BBA5, `UIOrigin`) | 登录链路中**未见调用点** |
| `CWnd::CreateWnd`(0x9DE4D2) / `CWnd::PreCreateWnd`(0x9DE7FB) | 均把坐标直接写入图层 LT，**无 `UIOrigin` 解析** |

### 2.3 结论：按实测反推固化为刚性平移

i = 0/1/2 三个槽位「按帧内 −220 系列预测」与实测落点差值**恒为 +140**
（预测 155/285/415 → 实测 295/425/555），说明两坐标系之间是**刚性平移**而非缩放/非线性，
故可安全固化为常量：

```cpp
static constexpr int16_t CS_CANVAS_TO_FRAME_X = 140;   // 帧内 x = 画布 x + 140
static constexpr int16_t SLOT_X0 = -220 + CS_CANVAS_TO_FRAME_X;
```

> 与纵向对照：纵向原点可由 `GetStepHeight(2) = -1208` 推出，横向不可 ——
> 这一不对称性已如实记录，未强行拟合成一个「看起来统一」的公式。

---

## 3. 槽位行距被证伪 → 校准为「固定 3 槽 + 翻页换角色」

### 3.1 证伪过程

| 证据 | 结论 |
|---|---|
| `CUICharDetail` ctor 的 `y` **只与占用状态有关、与索引无关** | 参考版**无行距概念** |
| `x = 130 * (a4 % 3) - 220`，用**绝对**索引取模 | 位置每 3 个循环 ⇒ 同时在屏上只有 **3** 个槽位 |
| `CUICharSelect::OnButtonClicked`(0x601667) **只处理 1000/1001/1002**（选/新/删），无翻页按钮 | 参考版 `CUICharSelect` 无分页 UI |
| `CUICharDetailVAC::CUICharDetailVAC`(0x60745A) | 独立的槽位类：`SetOption` 点 = `(148, -710)`，`CreateFadeWnd(170, 210, 0, z=11)`，读 `CLogin+316` 的**另一**角色数组 ⇒ 不是「第 2 行」，属另一类槽位 |

⇒ 原实现 `PAGESIZE = 12` + `index / 3 * 行距` 的两项均**无依据**，
且多行时第 3 行起（屏幕 y 784+）在 720 高下**必然落到屏幕外**，>6 个角色时版式已不可用。

### 3.2 校准

| 项 | 改前 | 改后 | 依据 |
|---|---|---|---|
| `PAGESIZE` | 12 | **3** | 上述 3.1 |
| 槽位纵向 | `CS_LIST_T + inset + (index/3)*151` | `CS_LIST_T + inset` | ctor 的 y 与索引无关 |
| 键盘导航 `COLUMNS` | 4 | **3** | 一行 3 槽 |
| 角色朝向 `fliplist` | 按**页内**索引 `i` 判断 | 按**绝对**索引 `index` 判断 | 保持 3/4/5/9/10/11 号角色既有朝向，不因每页槽位数变化而改变 |

**复验**：`evidence/charselect_pagesize3_verify.png` —— 与改前**逐像素一致**
（北斗每页 ≤3 个角色，故正常路径无可见变化），符合「正常路径行为零变化」的门禁。

---

## 4. 创建角色界面：锚点已取，但**不可照搬**（结论：待定）

### 4.1 参考版锚点

`CUINewCharRaceSelect::CUINewCharRaceSelect`(0x615596)：

```c
CWnd::CreateWnd(this, nLeft: -372, nTop: -2128, nWidth: 0x2BC /*700*/, nHeight: 460,
                z: 10, bScreenCoord: 0, pData: 0, bSetFocus: 1);
```

即 **700 x 460 @ canvas(−372, −2128)**，`bScreenCoord = 0`（与选角色槽位同一画布坐标系）。

### 4.2 为什么不能直接搬

本实现 `UIRaceSelect` 是 `UIElement((0,0), (800,600))`，且**全部坐标取自 NX 节点**
`Login.img/RaceSelect_new`（`button["pos"]`、`button["intervalX"]`、`make["pos"]` …），
与参考版（gms083 的 `CUINewCharRaceSelect`，硬编码在二进制里）**node 集与结构都不同**，
与 `CUICharSelect` 101x131 的情形同类。故仅记录锚点备查，**不照搬**。

### 4.3 附带发现：`UIRaceSelect` 未锚定地图相机（潜在缺陷，待实测）

- `UIStateLogin::draw` 只负责画背景 + 逐个 `element->draw(inter)`，**不施加任何全局相机偏移**；
  `UIWorldSelect`/`UICharSelect`/`UILogin` 都是各自把 `position` 锚到 `LoginBackground::get_viewpos()`。
- 但 `UIRaceSelect` **从不设置 `position`**（保持 `(0,0)`，屏幕绝对）⇒ 进入 step 3 时，
  背景随相机滚动而 UI 不动，两者会错开。
- **未修**：修正量需要 step 3 的实测落点才能定（参考版 `nTop = -2128` 与
  `GetStepHeight(3) = -1808` 之差为 −320，但换算到本实现需实测锚定基准）；
  而进入创建角色界面属于本轮计划明确排除的范围（避免触及写库流程）。
  ⇒ 按「无充分证据不臆改」保留，并在此登记为待办。

---

## 5. 仓库卫生（含一处前提被证伪）

### 5.1 「行尾符噪声」前提被证伪

上一轮记录「`includes/NoLifeNx/**` 存在纯 LF/CRLF 行尾噪声」，本轮实测**不成立**：

- `git status` 里这些文件的 `M` 中，`audio.cpp` 等**没有 `git diff` 输出**
  ⇒ 只是 **stat 脏（mtime）**，内容未变（`git` 刷新索引时的 LF→CRLF 提示是*未来检出*的预告，不是当前差异）。
- 真正有差异的 14 个文件是**真实功能开发**（新增 `wz_file` 支持）：
  `node.hpp` 增 `friend wz_file`、`bitmap.hpp` 增 `bitmap(wz_file*, ...)` 与 `m_wz`/`m_wz_index` 等，
  与未入库的 `Util/WzNx*` 是同一套工作。
- ⇒ **不做** `git add --renormalize` 归一化提交（那会产生一个无意义的全仓提交）。

### 5.2 实际做的

| 项 | 内容 |
|---|---|
| 删除构建垃圾 | `build_log.txt`、`_build_status.txt`、`_build_status2.txt`、`_err.txt` |
| 删除重复证据 | `evidence/CUICharSelect_sub_0x60148d.txt`（与 `CUICharSelect_OnCreate_0x60148d.txt` 完全一致） |
| `.gitignore` | 增 `/.idea`、`/.codebuddy`、`/build_log.txt`、`/includes/NoLifeNx/nlnx/*/obj-*`；**`.codebuddy` 只忽略、不删除不提交** |
| `.gitattributes` | 新增：`* text=auto` + 二进制扩展名显式标注 + `*.bat eol=crlf` / `*.sh eol=lf`。实测**零副作用**（改动文件数 35→35，NoLifeNx 仍 14） |
| 构建脚本 | `build_smoke.bat` → `tools/build_smoke.bat`，路径改为由脚本自身位置推导（可从任意工作目录调用），移后实跑 `EXITCODE=0` |

---

## 6. 本轮新增证据索引

| 文件 | 函数 | addr |
|---|---|---|
| `CWndMan_UpdateWindowPosition_0x9e03a6.txt` | `CWndMan::UpdateWindowPosition` | 0x9E03A6 |
| `CUICharDetailVAC_ctor_0x60745a.txt` | `CUICharDetailVAC::CUICharDetailVAC` | 0x60745A |
| `CUICharSelect_OnButtonClicked_0x601667.txt` | `CUICharSelect::OnButtonClicked` | 0x601667 |
| `CUINewCharRaceSelect_ctor_0x615596.txt` | `CUINewCharRaceSelect::CUINewCharRaceSelect` | 0x615596 |
| `worldselect_expanded_capacity.png` | 选世界（容量 15/30 + 越界防御后） | — |
| `chain_verify_after_world_guard.png` | 世界→频道→选角色 全链路点击复验 | — |
| `charselect_pagesize3_verify.png` | 选角色（每页 3 槽） | — |
