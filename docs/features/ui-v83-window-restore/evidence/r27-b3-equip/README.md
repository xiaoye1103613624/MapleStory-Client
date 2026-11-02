# R27-B3 取证：装备窗槽位表与灰显前提（img 实测，2026-11-02）

工具：`E:/project/MapleStory-Client/tools/nx_inspect.py`（直读 `.nxcache/UI.nx` 的 PKG4 结构，
不走任何解析猜测）＋ [measure_cell_tint.py](measure_cell_tint.py)。
数据源：`E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx` → `UIWindow.img/Equip`。

## 1. `UIWindow.img/Equip` 直接子节点（实测）

```
BtCashshop  BtDetail  BtPet1  BtPet2  BtPet3  BtPetEquipHide  BtPetEquipShow
FullBackgrnd[208x291] origin(0,0)   backgrnd[175x304] origin(0,0)   pet[177x181] origin(0,0)
```

- **没有 `Disabled` 子树，也没有 `disabled` 位图。** 每个 `Bt*` 下有 per-state 的
  `disabled{0}`，那是**按钮**的灰态，不是槽位灰底。
- 对照 `UIWindow.img/Item`：Item 有 `disabled[32x32]`（空格子衬底，B2 的 oracle 就是它），
  Equip **没有**对应节点。⇒ 装备窗的「不可用槽」在这包里**没有专用美术**。
- `BtPet1/2/3` 四态位图均 `30x27`，**origin = (15, 13)**（不是代码注释里的 (15,0)）。
  `BtDetail` 四态 `46x19`，origin (0,0)。

**⇒ 撤回 PLAN-R27 §B3「按 `Equip/Disabled/*` 实装灰显」这条**：该节点不存在，
照它写代码只会写出一条永远取不到的路径 + 一个静默 fallback。

**同节补正（同日）**：灰/粉底确实存在，但不是素材——实测两图所有格 `R-G≈-18`（冷灰，无一格偏暖）。
参考帧里的粉**既不是平涂、也不是另一张图**，是同一张底图被机械调色。详见 §3 与 §6。

> §3 第一版曾把粉判成「平涂矩形 ≈RGB(213,156,169)」，**该结论已于同日作废**，
> 作废理由与正确判据写在 §3 末尾，勿再引用那个色值。

## 2. 槽位坐标表：**代码里的表是对的**，偏移不在这张表

把 `backgrnd` 抠出来（[Equip_backgrnd.png](Equip_backgrnd.png)）后，格子是**烘在底图里**的，
且每格带中文标签（帽子/勋章/额饰/戒指×2/眼饰/耳饰/披风/上衣/项链/武器/盾牌/手套/裤·裙/腰带/
指环×2/鞋子/骑马/鞍子）。按代码表 `x∈{5,38,71,104,137}`、`y∈{32,65,…,230}`（步长 33）
逐格量内部亮度（[backgrnd-cell-tint.txt](backgrnd-cell-tint.txt)）：

| y | 5 | 38 | 71 | 104 | 137 | 底图里该行的标签 |
| --- | --- | --- | --- | --- | --- | --- |
| 32 | 白 | **格** | 白 | 白 | 白 | 帽子（第 2 列） |
| 65 | **格** | **格** | 亮 | **格** | **格** | 勋章 额饰 · 戒指 戒指 |
| 98 | 白 | 亮 | **格** | **格** | 白 | 眼饰 耳饰（第 3、4 列） |
| 131 | **格** | **格** | **格** | **格** | **格** | 披风 上衣 项链 武器 盾牌 |
| 164 | **格** | **格** | **格** | **格** | **格** | 手套 裤/裙 腰带 指环 指环 |
| 197 | 白 | 亮 | **格** | 白 | 白 | 鞋子（第 3 列） |
| 230 | **格** | **格** | 亮 | 白 | 白 | 骑马 鞍子 |

「格」= 内部亮度 ≈ 195~205（凹槽灰面），「白」= 255（底图上那里根本没有格子），
「亮」= 235~245（被 `pet` 宠物轮廓线压过，但仍无格）。

**7 行 × 5 列全部对上**：列相位 5、行相位 32、节距 33 —— 与 `UIEquipInventory.cpp:103-123`
的硬编码表逐格一致。⇒ 用户报的「装备 ui 位置偏移」**不是这张表错**，而是：

1. 绘制侧多加了一个不存在的偏移（`:338-343` 的 `(4,4)` 死偏移）；
2. 绘制与命中不同源（`:711-731` 命中区固定 32×32，另算）；
3. 两套开关（坐标表用 `is_classic_hud()`，美术分支用 `classic_flat`）不同步 ⇒ 整窗标签版式与坐标错位。

这三条才是 B3 的实体；**槽位表本身不需要改数值**。

## 3. 「骑兽/鞋子区有灰/粉底」——**这条是真的，但不是素材**

> 本节于同日**自我推翻过一次**：第一版用「内部亮度」判，得到「与正常格无差 ⇒ 是观感」。
> 错在判据：**粉底和灰底亮度可以完全相同**（≈200），只有**色度**能分开。改用 `R-G` 重测。

| 测法 | 结论 | 可信度 |
| --- | --- | --- |
| 内部亮度（`backgrnd-cell-tint.txt`） | 所有格 ≈195~205，「无差」 | **判据选错，作废** |
| 内部色度 `R-G`（[cell-chroma.txt](cell-chroma.txt)） | 每张真格 `R-G ≈ -17~-19`（**冷灰**），**没有任何一格偏暖** | 采信 |

⇒ 素材侧定案：`backgrnd` 与 `FullBackgrnd` **都不含粉/灰态**，两图所有格子色度一致。

再量参考帧 `角色装备.png`（348x305）里的粉（[disabled_measure.py](disabled_measure.py) →
[disabled_measure.txt](disabled_measure.txt)）：

- 粉格**不是单色**：宽格 64x31 里帧色有 31 种（主色 (204,153,170) 只占 47%），方格 31x31 里 13 种。
  素材同一区域分别有 34 / 18 种。**格内的网点、描边、暗角全部保留** ⇒ 不是「盖一层色」。
- 逐色对照：素材 34 色 → 帧 31 色，**每个素材色只映射到一个帧色**（一对一机械映射），
  实测 15 组映射对见 [disabled_measure.txt](disabled_measure.txt)。
- 「平涂 ≈RGB(213,156,169)」这条**作废**。它的来源是把整行（含白边框、含装备图标）一起做
  逐通道最小二乘，得到的 a=0.63~0.87、b=−38~+59 本来就不该被当成一个填充色来读。
  正确读法是**只在格内**做映射表 + 逐通道反解：任何「一个 α + 一个纯色 P」的假设
  （`out = α·P + (1−α)·art`）都会在同一格内反解出互相矛盾的 α（如 art(187,204,221)→(204,153,170)
  在 R 通道要求 α=0.25、在 G 通道要求 α=1.0），并且需要 P=(255,0,0) 才能解释白色格 →
  而 (238,238,238)→R'=255 又把这个 α 卡死在 ≥0.25。**平涂假设被逐像素否证。**

⇒ 定案见 §6：**机制 = 代码 + 逐像素调色**，不是 UI 素材，也不是平涂填充。

## 3b. 参考帧到位情况（本轮新确认）

桌面 `角色装备.png` 348x305 = **收起态装备窗 + 展开态右栏**同框，
`角色装备栏显示物品信息.png` 359x404 = **装备 tooltip**。B3 / B3b 的 oracle 原料齐了，
不再等用户补拍。

## 4. 方法学结论（回应用户「能不能不看截图」）

| 事实种类 | 谁能裁决 | 说明 |
| --- | --- | --- |
| 位图尺寸 / origin / 有哪些状态 | **img**（本工具） | 确定性、可复跑，本目录全部属于这类 |
| 节距、列数、哪些位置有格 | **img**（底图自相关/逐格亮度） | 上面第 2 节 |
| 窗口在屏幕上的落位、标题栏、按钮摆放 | **只能原生帧** | 素材里没有「窗口左上角」这个信息 |
| 槽位→图标的落点相位 | img 给相位（5,32），帧给校验 | 两者一致才闭环 |

⇒ 「还原逻辑」能吃掉**大部分**截图依赖，但吃不掉**窗口级绝对定位**；
而窗口级定位在关闭源码的原生引擎里是**代码里的常数**，不是素材字段。

## 5. 装备窗的 `disabled` / `disabled2` 为什么永远不画（实测路径链）

代码 `UIEquipInventory.cpp:281-282` 取的是：

```cpp
disabled  = Equip[tab_source[BT_TAB0]]["disabled"];   //  = UIWindow.img/Equip["Equip"]["disabled"]
disabled2 = Equip[tab_source[BT_TAB0]]["disabled2"];
```

而 `tab_source[BT_TAB0] == "Equip"`（`:106` 附近赋值）。逐环实测：

| 环节 | 实测 | 依据 |
| --- | --- | --- |
| `window4("Equip")` 落到哪 | `UIWindow.img/Equip` | 本包 `.nxcache/UI.nx` 里 **没有** `UIWindow4.img`、**没有** `UIWindow2.img`（`nx_inspect list … 0 <root>` 两者 NOT FOUND），回退链只剩第三档 |
| `UIWindow.img/Equip` 有没有 `Equip` 子节点 | **没有** | §1 的直接子节点清单只有 7 个 `Bt*` + `backgrnd`/`FullBackgrnd`/`pet` |
| ⇒ `classic_flat` | **恒为 true** | `:106` `classic_flat = !usable(Equip["Equip"])`，参数是空节点 |
| ⇒ `disabled` / `disabled2` | **恒为无效** | `:330/:333/:356-358` 三处绘制全被 `.is_valid()` 短路 |

**⇒ 修正 PLAN-R27 的原表述**：那两处读取不是「在 flat 分支 `return` 之后不可达」，
而是**路径本身在本包不存在** —— 它假设的是另一套包结构（`Equip/Equip/{disabled,disabled2}` 的六列页签壳）。
后果比"不可达"更严重：**整条非 flat 分支（`:143-177` 六列坐标表 + `:300-301` 页签钮）在本包是死代码**，
且 `classic_flat` 这个开关**永远只有一个取值**，等于没有开关。

灰显因此**不可能靠补 `disabled` 美术实现**（§3 已证素材里也没有粉）。

## 6. 裁决：原生「槽位不可用」= **代码 + 逐像素调色**（用户门禁，2026-11-02 闭环）

用户问的是「用 ui+代码，还是绘制颜色+代码」。三条**有效**证据（原先算作第四条的二进制侧已**自降为非证据**，见 6.2），方向一致：

### 6.1 素材侧：整包里没有粉色装备底图，而且两端用的是同一个文件

| 证据 | 实测 |
| --- | --- |
| 两端素材是否同一份 | `E:/MXD/BeiDou-Client-xy/Data/UI/UIWindow.img` 与 `E:/MXD/BeiDou-ClientV17.7/Data/UI/UIWindow.img` **md5 相同** = `c19d34996a71f2be8cfce81de01d70a8`（11,806,696 B，2025-06-22）；`Data/UI/` 22 个 `.img` 逐名逐尺寸逐 mtime 一致 ⇒ **参考端没有我们缺的图** |
| 粉色是否以「另一张图」存在 | 对 `.nxcache/UI.nx`（= 上面那批 `.img` 的转换产物）**全部 15,134 张位图**按精确色扫（tol=8，≥60 px），目标色取 oracle 实测的 (255,187,187) 与 (204,153,170)：`pink1` 命中 31 张、`pink2` 命中 4 张，**没有一张在 `UIWindow.img/Equip` 下**（最高的是 `MapleTV.img/TVchat2/*`）。对照组同判据扫冷色 (187,204,221)：命中 **546** 张，含 `Equip/backgrnd` 10,163 px、`Equip/FullBackgrnd` 15,788 px、`Equip/pet` 9,688 px ⇒ 检测器有效，否证可信（[scan_pink.py](scan_pink.py) → [pink_scan.txt](pink_scan.txt)） |
| 之前那次「平坦暖色」扫描的洞 | 上一轮判据是「平坦」，而粉格 n=795/961 本身带网点，会被漏掉。本轮改成**逐像素精确色、不限尺寸、不限平坦度**，洞已补 |

### 6.2 二进制侧：**自降为非证据**（2026-11-02 专项复核后改写）

曾把「`BeiDou.exe` 里没有装备窗的 `disabled` 字面量」当作一条独立证据。复核后**它不成立**，理由是
**参照系本身是空的**——该二进制对装备窗**一个素材字面量都没有**（`UIWindow.img/Equip`、`Equip/`、
`FullBackgrnd`、连 `backgrnd` 整路径全为 0 命中），所以「没有 `disabled`」无法区分
「原生不走这条路」与「这扇窗的素材名根本不住在这个模块里」。全镜像搜的实测：

| 项 | 结果 |
| --- | --- |
| `disabled`（ASCII / UTF-16） | 0 / **1** @`0xafd17e` = `UIWindow.img/Delivery/Tab/disabled`（送货窗，与装备无关） |
| `Disabled`（ASCII / UTF-16） | **1** @`0xb3bc80` = `UIWindow.img/ToolTip/Equip/GrowthDisabled`（tooltip 节点）/ 0 |
| `Equip`（UTF-16）@`0xb377e8` | **不是**镜像子节点：xref 只来自 `sub_7D1FC5`@`0x7D23E5`，且位于 `Item, Equip, StartMap, InitSPDesc, …` 这串 **String 模块键名块**里 |
| 粉色 `D5 9C A9` / `A9 9C D5` 及其 `00/FF` 前后缀、RGB565 `D4F5`、RGB555 `6A75` | **全 0** |
| `.text` 内唯一真·粉色立即数 | `sub_66E9F7`@`0x66F27A` 的 `mov …, 0FFA0A0h` = **RGB(255,160,160)**。它 G≡B，实测粉 G=156/B=169，任何灰度混合都变不过去 ⇒ **排除**，不是我们的色 |
| 搜索机制自身的正对照 | UTF-16 `backgrnd` 8 命中（含已知的 `0xb405ba`）、8-bit 4 命中；2 字节 `9C A9` 54 命中；立即数搜 `5201002` 6 命中 ⇒ 3 字节的否定是**真否定**，不是工具没覆盖 |

**为什么整个模块里找不到 UI 代码**：该 exe **不导入任何 D3D**（只依赖 17 个系统 DLL；gdi32 的
`CreateDIBSection`/`BitBlt` 仅 3+3 个 xref，且都在截图/存 PNG 的路径上），也从未点名
`Gr2D_DX8 / Canvas / NameSpace / ResMan / Shape2D`，WZ 走 COM/`VARIANTARG`（`oleaut32`，
"IDispatch error #%d"），七个段里没有内嵌 PE。⇒ **绘制与调色逻辑在本模块之外**（最可能在
`Gr2D_DX8.dll` / `Canvas.dll`）。要拿二进制侧的正证据，得换见证人，不是继续搜这个 exe。

另：先前当作线索的 `backgrnd2` @`0xb405ba` 已核实是 `UI/UIWindow.img/NewYearsCard/backgrnd2`（贺年卡窗），与装备无关——**假线索，撤回**。

### 6.3 调色公式：15/15 组实测映射对 100% 命中

```
R' = snap17(0.75·R + 68.25)      # 上限截到 255
G' = snap17(0.75·G)
B' = snap17(0.75·B)
snap17 = 吸附到 ×17 网格（即素材自身的 4bit 色阶），tie 向下
```

- 参数唯一性要如实说：`round + scale=0.749 + K=68.25` 与 `half-down + scale=0.750 + K=68.25`
  都能 100% 解释全部样本（只有 16 个网格档位，样本不足以区分 tie 规则）。**tie 规则未定**，
  但影响面只有恰好压在 8.5 半格上的那几个值。
- 形状 = 通道缩放 + 常数红偏置，正是 DX8 纹理阶段「MODULATE 后 ADD」的形态；
  **纯乘法调制做不到**（要把 R 从 187 抬到 204，系数 >1）。

### 6.4 被染的是哪些矩形：逐像素按 pred 等式反查整窗

判据：素材不透明的像素里，帧色 == `pred(素材色)` 才算「调色区」。装备图标等叠绘内容
不满足这个逐像素等式，因此不会被误计（[tint_rects.py](tint_rects.py) → [tint_rects.txt](tint_rects.txt)）：

| 区域 | 帧坐标矩形 | pred 命中率 |
| --- | --- | --- |
| `backgrnd`（左栏装备格） | x=5..102 y=233..264（98x32） | **98%**（3059/3136） |
| `pet`（右栏宠物格） | x=218..248 y=134..165 | 71% |
| `pet` | x=285..314 y=134..165 | 77% |
| `pet` | x=252..281 y=200..264 | 35% |
| `pet` | x=285..314 y=200..264 | 71% |
| `FullBackgrnd` | 与 `backgrnd` 同带，59% | 该节点本帧未被绘制，忽略 |

左栏那个 98 宽 = **三格并排**（x=5/38/71，节距 33，格 31x32；两条 2px 分隔列正好是
未命中的那 77 px），对应我们槽表的 `TAMEDMOB(5,230)`、`SADDLE(38,230)`，**以及第 3 格
(71,230)——这张表里没有给它任何槽位**。宠物区同理是 30~31 见方的单格。
命中率不到 100% 的几格是格内有宠物图标/文字压住，属预期。

### 6.4b 三个新事实（同日逐像素复量，dy 扫描定出来的）

先定对齐：把 `backgrnd` 按 `dy∈[-4,+4]` 逐像素对帧，**dy=0 时 72.5% 的不透明像素与帧完全相等**
（其余 dy 只有 34~38%）⇒ 底图与帧是 **1:1 同点**，没有 3px 整体偏移。在此对齐下重量调色区：

| 新事实 | 实测 | 后果 |
| --- | --- | --- |
| **调色矩形 = 槽位坐标 + (0, +3)，尺寸 32x32** | 三格染色带都是 `y=233..264`；而槽表 `TAMEDMOB(5,230)`/`SADDLE(38,230)`；底图该两格凹槽内部量到 `y=234..264` ⇒ 染色矩形贴的是**格子凹槽**，不是图标绘制点 | 实装时染色原点要 `slot_pos + (0,3)`，不能直接拿 `iconpositions` |
| **调色对「白底」同样生效 ⇒ 它是矩形域的逐像素变换，不是「只染有美术的像素」** | 第三格 `x=71..102` 底图内部 784 px 里 **558 是纯白 (255,255,255)**（无凹槽），帧里同区主色 (255,187,187) 占 610/784；而 `pred(255,255,255) = (255,187,187)` 精确相等（`snap17(191.25)=187`，R 侧 259.5 截到 255） | 变换是「对背景在该矩形内逐像素重上色」，实现上等价于**矩形裁剪 + 调色**，与「素材有没有画格子」无关 |
| **第三格是一个我们表里没有的槽** | 底图第 7 行只有 `骑马(5)`/`鞍子(38)` 两格有凹槽与标签，`x=71` 处**无凹槽、无标签**（§2 早已量作「亮」），但原生仍把它染了 | 说明「染=不可用槽」时，**原生在这一行有第三槽**；本包 `EquipSlot::Id` 里没有对应项（`TAMEDMOB`/`SADDLE` 之外只剩 `POCKET/BOOK/PENDANT2/SHOULDER/EMBLEM/BADGE/HEART/ANDROID_SLOT` 全未落位）⇒ 不能靠猜填表 |

⇒ 结合用户判断（**不可用槽由角色/已装备逻辑推导，不是写死**）：这张帧是「该号既无骑兽、也未召唤宠物」的
**判定快照**，不是常量表。所以实装必须是 `谓词(槽位, 角色, 已装备) → 不可用集合 → 逐格染色`，
第三格的归属只能靠**交互取证**定（原生端把骑兽装上，看哪一格转正常；或悬浮该格看 tooltip 槽名）。

### 6.5 结论与仍未闭合的那一半

**机制定案：绘制颜色 + 代码。** 支撑只剩**三条**，但每条都是硬否证/硬正证：
① 两端 `UIWindow.img` md5 相同 ⇒ 「参考端有粉素材、我们缺」不可能；
② 全 15,134 张位图逐像素精确色扫描，`UIWindow.img/Equip` 下**一张粉色都没有**（同判据冷色对照 546 张命中，检测器有效）⇒ 「另有一张粉图」不可能；
③ 粉格与同一张冷灰底图之间是**一一对应的确定性映射**，`R'=snap17(0.75R+68.25)`、`G'=B'=snap17(0.75V)` 15/15 全中，且平涂 α 假设被逐像素代数否证（(187,204,221)→(204,153,170) 在 R 需 α=0.25、在 G 需 α=1.0，自相矛盾）⇒ 「程序平涂一层色」不可能。

三条各杀掉一种替代解释，剩下的只有「**运行期对同一张底图按槽矩形重新上色**」。引擎按「不可用槽」列表，
在这些槽的矩形范围内以 `¾ 缩放 + 红偏置` 重上色后叠绘（保留网点结构）。不是 UI 素材，不是平涂填充，
也不是我们代码假设的 `disabled` 位图。

> ⚠ **6.2 不再是支撑**：二进制侧那条已自降为非证据（参照系全空），别把它算进证据链。
> 换句话说：**这个结论目前没有任何正证来自引擎代码**，全部来自素材与像素。若要拿正证，
> 得去 `Gr2D_DX8.dll` / `Canvas.dll` 找纹理阶段（MODULATE 后 ADD 的形态与 6.3 吻合）。

**未闭合（诚实标注，别当已知用）**：
1. **哪些槽算不可用 = 运行期判定，不是常量**（用户 2026-11-02 明确：由角色/已装备等逻辑推导）。
   本帧只是「该号无骑兽 + 未召唤宠物」的判定快照 ⇒ 实装按 `谓词 → 不可用集合 → 逐格染色` 做，
   **不得把 §6.4 的矩形抄成硬编码格子表**。
   仍缺的是谓词本身：左栏第 3 格 (71,233) 归属哪个槽、宠物区 4 格随什么状态变化。
   ⇒ 单靠帧推不出谓词，只能靠**交互取证**：原生端装上骑兽/召唤宠物各拍一张（看哪几格转正常），
   或悬浮粉格读 tooltip 槽名。
2. tie 规则（见 6.3）。
3. 我们这条栈**当前无法原样复现**该变换：`nl::bitmap` 构造函数是 private（只 `friend node`），
   不能从内存像素自建纹理（`includes/NoLifeNx/nlnx/bitmap.hpp:47-48`）；`DrawArgument` 的
   `Color` 在合成时是 `color * o.color` 纯乘法（`Graphics/DrawArgument.h:104`），抬不起 R。
   已存在的原始像素通道只有 `Graphics/LocalPngTexture`（从磁盘 PNG 进 GL atlas，走 `ensure_raw`）。
   ⇒ 实现路线是**决策点**，见 PLAN-R27 §B3。


## 7. 谓词取证（B3-2，2026-11-02，全部只读 `nx_inspect`）

「不可用槽由逻辑推导」要能落地，得先确认**判定输入在本包/本仓取得到**。实测：

| 判定输入 | 取证结果 | 依据 |
| --- | --- | --- |
| 骑兽技能 id | **`1004`「骑兽技能」**，满级 1、`invisible = 1`（通用技能块 `000.img/skill/0001004`） | `String.nx/Skill.img/0001004/name` + `Skill.nx/000.img/skill/0001004/{level,invisible}` |
| 特定坐骑的专属技能 | `1017`/`1018`「白雪人骑宠」带 **`disable = 1`、`timeLimited = 1`**，同样是满级 1 的隐形技能 | `Skill.nx/000.img/skill/0001017` |
| 装备槽类别是否有「骑兽 / 宠物装备」 | **有**：`String.nx/Eqp.img/Eqp` 16 类中含 `Taming{1902002}`、`PetEquip{1802002}`、`Dragon{1942002}` ⇒ 原生把它们当独立装备类别，不是装饰 | `nx_inspect` 实测（[../notes/asset-node-inventory.md](../../notes/asset-node-inventory.md) 同源） |
| 坐骑→技能 的绑定是否写在 `TamingMob.nx` | **没有**：本包每个坐骑只有 `info{fatigue, fs, jump, speed, swim}`，无 `skill`/`saddle` 字段 | `TamingMob.nx/000X.img/info` |
| 装备类物品数据 | `Item.nx` 只有 `Cash/Consume/Etc/Install/Pet/Special` 六个根，**19xxxxx 装备类不在此包**（散在 `Data/Item/`） ⇒ 「这匹坐骑要求哪个技能」本包拿不到 | `nx_inspect list Item.nx` |

本仓可用的判定入口（都不需要改公共 API）：

| 用途 | 位置 |
| --- | --- |
| 取本地角色 | `Stage::get().get_player()`（[Gameplay/Stage.h:79](../../../../Gameplay/Stage.h#L79)），装备窗构造点已在用它（`UIStateGame.cpp:257`） |
| 技能等级 | `Player::get_skilllevel(int32_t)`（[Character/Player.h:107](../../../../Character/Player.h#L107)） |
| 已装备项 | `Inventory::get_equip(InventoryType::Id, int16_t slot)`（[Character/Inventory/Inventory.h:110](../../../../Character/Inventory/Inventory.h#L110)） |
| 双手武器 | `Weapon::Type` 含 `SWORD_2H/AXE_2H/MACE_2H/SPEAR/POLEARM/BOW/CROSSBOW/CLAW`（[Character/Inventory/Weapon.h:26-46](../../../../Character/Inventory/Weapon.h#L26-L46)） |

**结论（能闭环的 / 必须预留的）**：

- 可闭环：`TAMEDMOB` ← `get_skilllevel(1004) > 0`；`SADDLE` ← `TAMEDMOB` 槽已装备；
  `SHIELD` ← 已装备武器为双手类。
- **必须显式预留**：宠物面板 4 格与 `POCKET` —— 本仓**没有宠物模型**（无 `Pet`/`PetEquip` 类，
  `EquipSlot::Id` 里也没有任何 `PET_*` 槽位），`TamingMob.nx` 又不编码坐骑↔技能绑定，
  所以这两类只能按「默认不可用」画（与参考帧一致），并在代码里写明是缺子系统而不是缺证据。


## 8. 实装记录（B3-1 + B3-3，2026-11-02）

用户裁决：**甲（改渲染层做真调色）**，并限定「不可用槽位应该是根据角色、角色装备等逻辑判断而来的，**不是写死的**」。落地分三层：

| 层 | 位置 | 做了什么 |
| --- | --- | --- |
| 调色核 | [Graphics/MemoryTexture.cpp](../../../../Graphics/MemoryTexture.cpp) | §6.3 的浮点模型改写成**精确整数**式：`G'/B' = 17·((3C+33)/68)`、`R' = 17·((3R+306)/68)`，`k≤15` 故 255 截断天然成立；平手向下由 `ceil` 形式固定 |
| 像素通道 | [Graphics/Texture.h](../../../../Graphics/Texture.h) `copy_pixels()/get_bitmap_id()` | 只读取。`nl::bitmap::data()` 是 **BGRA、每次调用都解压、且共用一块全局缓冲** ⇒ 必须当场拷走，不能存指针 |
| 纹理 | `MemoryTexture` + `GraphicsGL::ensure_raw/draw_raw` | 绕开 §6.5-3 说的「不能从内存自建纹理」：走 `LocalPngTexture` 已有的 raw 通道，但像素来自内存。atlas id = FNV-1a(`源位图 id`, 裁剪矩形) 打上 `0x54494E…` 标记，与 nlnx 堆指针 id、fourcc id 都不撞 |

**按格裁剪，不整窗改色**：原生改色本来就是限定矩形的（§6.4），所以在同一位置叠一块改过色的 32×32 裁剪图，与画一整张改色底图像素一致，而 atlas 只为这几格付代价；id 由几何决定 ⇒ 穿脱装备不会长出新纹理，谓词只决定「这一格画哪个版本」。

**几何唯一真源**（§2 + §6.4）：调色矩形 = `iconpositions[槽] + (0, 3)`、32×32（底图凹槽相对格顶实测下移 3px：行 7 格顶 y=230，改色带实测 y=233..264）。
绘制落点 = `position − bg_origin + 矩形.xy`（backgrnd 的 origin 实测 (0,0)，此项为恒等，保留是为了换美术不失效）。
`bg_origin` 来自 `backgrnd_tex.get_origin()`，与 `draw_sprites` 那条同源，不再另抄常数。

**谓词**（`UIEquipInventory::slot_available()`，只有拿得出判据的三格参与改色）：

| 槽 | 判据 | 取证 |
| --- | --- | --- |
| `TAMEDMOB` | `Stage::get().get_player().get_skilllevel(1004) > 0` | §7「骑兽技能」 |
| `SADDLE` | `TAMEDMOB` 槽当前已装备 | §7 鞍子挂骑兽 |
| `SHIELD` | `!Stage::get().get_player().is_twohanded()`（`Char::is_twohanded()`，[Character/Char.h:134](../../../../Character/Char.h#L134)） | 双手武器占手 |
| 其余 | 恒 `true`（不改色） | 无判据 ⇒ 宁可不画，不抄格子表 |

**调色核门禁**（[verify_tint_kernel.py](verify_tint_kernel.py) → `E:/project/_b3/tint_kernel_verify.txt`）两道，任一不过即失败：
① 0..255 × 3 通道穷举，整数式与浮点式**逐值相等**（实测 768/768，不一致 0 组）；
② 逐矩形回算原生帧命中率不得劣于浮点式：三格 98.83% / 99.90% / **100.00%**，合计 6569/8924 = 73.61%，与浮点式**完全相等**。
> 第一版整数核用「先四舍五入再吸附」写成 `(3C+2)/4`，合计命中率 61.47%，比浮点式低 12 个点——是这道穷举门禁把它抓出来的，不是人眼看出来的。

**同批删除的死分支**（§5 的结论付诸实施：开关恒真 ⇒ 等于没有开关）：
`classic_flat`、六列 `iconpositions`、`tab_source`/`tab`/`tabbar`/`background[]`/`Slots[]`/`disabled`/`disabled2`、
`EquipGL` 的 `Totem/btEffect/btSalon`、`BT_TAB0..3` 钮与 TAB 键循环、`BT_EFFECT/BT_SALON/BT_CONSUMESETTING/BT_EXCEPTION` 处理器、
`hasPendantSlot/hasPocketSlot`、`is_in_range()` 里的 totem 命中区。
⇒ 底图、坐标、命中、调色现在共用同一张 `iconpositions`，「底图 v83 单张 + 坐标另一套六列」这类整窗错位从结构上不可能再发生。
（`CHECKLIST.md` D7 那行「modern 分支 6 颗钮无坐标」随之作废。）

**未随本批实现（诚实标注）**：
1. **宠物面板 4 格**：`Equip/pet` 上改色带的预测命中率只有 **71.27 / 77.19 / 34.67 / 71.08%**（对照装备三格的 98.8~100%），
   且本仓无 `PET_*` 槽位、无宠物子系统 ⇒ 拿这套几何去铺粉色就是瞎猜偏移。**待用户提供「宠物面板展开」的原生帧**再定，见 §3b。
2. **行 7 第 3 格 (71,233)**：`iconpositions` 里根本没有这个相位的槽 ⇒ 归属未定，需交互取证（装上骑兽后哪几格转正常）。
3. 真机逐格验收（`tools/layout.py check Equip`）：需要部署，等客户端退出。


## 9. 真机逐格验收（B3-6 首批，2026-11-02 17:00，部署包 `4d809b2c…`）

部署 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（md5 与 `build_check` 一致），换构建前的运行目录包
已快照到 `E:\MXD\_build_snapshots\20261102-1650_preplace-current\`。
自动登录进图（`tools/live/login.py`，口令走 `R27_USER`/`R27_PASS`，不落盘）→ `open_window.py e` 开装备窗
→ PrintWindow 抓帧 [live-20261102-equip-open.png](live-20261102-equip-open.png)
→ [live_verify.py](live_verify.py) 出 [live_verify.txt](live_verify.txt)。

**判据不是「有没有粉色」，而是「这一格的像素 == 调色核作用在底图上的结果」**：
以 `Equip_backgrnd.png` 为模板锚出窗口左上（agree **0.9124** > 阈值 0.8），再对每格 32×32 的
全部不透明像素同时算两个命中率——与 `tint(art)` 相等（调色核命中）、与 `art` 本身相等（原样命中）。
按颜色计数会误判：改色保留原生网点/渐变，一格里粉主色只占 ~46%。

| 格 | 调色核命中 | 原样命中 | 期望 | 结果 |
| --- | --- | --- | --- | --- |
| `TAMEDMOB` 骑兽 (5,230) | **100.00%** (1024/1024) | 0.00% | 染 | OK |
| `SADDLE` 鞍子 (38,230) | **100.00%** (1024/1024) | 0.00% | 染 | OK |
| `SHIELD` 盾牌 (137,131) | 0.00% | **100.00%** | 不染 | OK |
| `WEAPON` 武器 (104,131) | 0.00% | 100.00% | 不染 | OK |
| `HAT` 帽子 (71,32) | 0.00% | 100.00% | 不染 | OK |
| `SHOES` 鞋子 (71,197) | 0.00% | 100.00% | 不染 | OK |
| 行 7 第 3 格 (71,230) | 0.00% | 100.00% | 归属未定 | 不断言 |

⇒ 该号 Lv.1 无骑兽技能 ⇒ 骑兽+鞍子两格逐像素等于原生改色模型；盾牌格保持原美术（未装双手武器，
谓词判可用）。6 格 × 1024 像素**逐格全等**同时反证锚点正确、绘制/命中/调色三源合一没有相位差。

**这一轮还没证到的**（别当已知用）：
1. **谓词翻转**：装双手武器 ⇒ 盾牌格转粉；学骑兽技能 1004 ⇒ 骑兽/鞍子格恢复正常。
   需要能凑出这两态的号或 GM 命令，属交互取证。
2. 行 7 第 3 格在原生帧里是粉的、我们这里不是 —— 槽位归属仍未定（§6.5-1 遗留）。
3. 宠物面板 4 格的改色（§8 预留，等展开态原生帧）。

## 10. R27-B3-4a：Icon 拿回几何所有权（绘制 / 命中 / 抓取三处同源）

§9 那 6 格逐像素全等只证明了**装备窗**一处「绘制 = 命中 = 调色」。用户报的第二轮缺陷里
「键盘设定按键图偏上」「背包点了是别格」「技能窗鼠标压在图标上却不出 tooltip」是同一个
结构病的不同症状，所以这一批把病根一次治掉，而不是逐窗再量一遍。

### 10.1 契约

上游的 `ms::Icon` **不带任何几何**：只有一个 `draw(Point)` 和一个 `start_drag(Point)`，
「这一格在哪、多大、点哪里算这格」全部由持有者各自现场算。改完后的唯一真源在
[IO/Components/Icon.h:91-112](../../../../IO/Components/Icon.h#L91-L112)：

| API | 语义 |
| --- | --- |
| `cell_width / cell_height = 32` | 格大小。四窗共用一个常数，不再各写各的 `Point(32, 32)` |
| `set_cell(格左上角，窗口相对)` | **单一写入者 = 该窗口的绘制循环**，画之前写一次 |
| `draw_at(parent)` | 绘制点 = `parent + slot` |
| `rect(parent)` / `contains(parent, cursor)` | 命中矩形 = 本格，与 `draw_at` 同一个 `slot` |
| `start_drag_at(parent, cursor)` | 抓取偏移 = `cursor − parent − slot`（拖起来跟手，按下瞬间不跳格） |
| `start_drag_at(parent, slot, cursor)` | 一个图标画在两格时用（键盘设定的 Ctrl/Alt/Shift），显式指定「命中的那一格」 |

`draw(Point)` / `start_drag(Point)` **保留为原语**（`set_cell` 之后持有者仍可直接调），
这样这次改造只碰「几何从哪来」，不改「图标怎么画」。

**命名为什么是 `cell` 而不是 `slot`**（第一版写的 `set_slot` 已同日改名，不是风格问题）：
持有者侧的 `slot` 一律指**槽号**（`int16_t slot`、`EquipSlot::Id slot`、`slot_by_position`），
而这里存的是**格左上角**（`Point`）。`Icon::drop_on_items(..., int16_t slot, ...)` 里两者同名不同义，
实测触发 C4458「声明隐藏了类成员」；`UIKeyConfig.cpp:222` 更早就有个局部 `slot_width = 33`
（键帽步进），与 `Icon::slot_width = 32`（格子大小）在同一文件里并排会读错。⇒ 几何叫 `cell`，槽号留给 `slot`。

一处需要留档的几何细节：`Rectangle<int16_t>::contains` 两端闭区间
（[Template/Rectangle.h:66-72](../../../../Template/Rectangle.h#L66-L72)，`v.x() >= left() && v.x() <= right()` 两端都闭），所以 32×32 的格实际
覆盖 33px；背包列距 36、行距 34 ⇒ 相邻格不重叠，格间只剩 **3px（列）/ 1px（行）** 的死区。这不是缺陷，
但它意味着「命中范围」不能拿除法边界去近似 —— §10.2 那条背包旧式子犯的正是这个。

### 10.2 四个持有者的旧分叉（读码 + diff，非推测）

`ms::Icon` 的持有者全集只有 4 个（grep `std::unique_ptr<Icon>` / `Icon>` 在 IO/ 下枚举）：

| 窗口 | 旧绘制点 | 旧命中 | 旧抓取偏移 | 表观后果 |
| --- | --- | --- | --- | --- |
| 装备 `UIEquipInventory` | `position + ip + (4,4)`（§2 的 `:338-343` 死偏移） | `position + ip`，`Point(32,32)` | `cursorpos − position − iconpositions[slot]` | 每件装备压向右下 4px，盖住下一格中文标签（用户报「装备 ui 位置偏移」，§2 已量过底图凹槽相位本就是 (5,32)） |
| 背包 `UIItemInventory` | `position + get_slotpos(i)` | **另一套除法**：`col = x/(32+4)`、`row = y/(32+2) − 1`、`+ div.quot*28` 页补偿 | `cursor_relative − get_slotpos(slot)`（重算一次） | 除法边界与格子边界不同相（[hit_formula_probe.py](hit_formula_probe.py) → [hit_formula_probe.txt](hit_formula_probe.txt)，逐像素数出来的，不是推的）：**收起态画在一格上的 32×32 里有 50.8% 的像素解成别的槽**（右侧 4px 串到邻列、下侧 14px 串到下行）；**展开态 87.7%** —— 页距 149 根本不在这套除法里，第 1/2/3 页的格心分别落到 +8 / +16 / +25 槽。空格是拖放目标却被 `xmax/ymax` 粗筛直接排除 |
| 键盘设定 `UIKeyConfig` | `position + keys_pos[key] − adjust`（`adjust` = 道具/技能图标减 (2,3)、SPACE 减 (0,3)、键帽减 (0,0)） | `position + iter.second` + `Point(32,32)`，**没有 adjust** | `cursorpos − position − keys_pos[key]`，**没有 adjust** | 画在上、点在下，差整整 2×3px；用户报的「键盘设定按键图片全都偏上」（R27-B7 / 任务 #87）就是这个方向 |
| 技能 `UISkillBook` | 行底 + `SKILL_META_OFFSET (2,2)` | `Rectangle(0,32,0,32)` 贴**行底**（少 (2,2)）；`skill_by_position` 又是 `(y−base)/ROW_HEIGHT` 除法 | `cursorpos − skill_position`（行底，非图标格） | 三处三个点。双击/悬浮判定把 40px 行距整段认成一格（格高只有 32）⇒ 行下方 8px 空档也会命中；现代双列还用 `skill_index(row) + col`，与 (row%2, row/2) 的实际排布不符 ⇒ 取错技能 |

改造后四处只剩两个来源：**版式表**（`iconpositions` / `get_slotpos` / `keys_pos` / `skill_cell()`）
决定格左上角，**Icon** 决定格大小与三处读法。`UISkillBook` 顺带把 draw / `send_cursor` /
`doubleclick` 里重复了四遍的行推进式子收成一处
[`UISkillBook::skill_cell()`](../../../../IO/UITypes/UISkillBook.cpp#L994-L1008)，
并把 `skill_by_position()` 改成 `skill_cell()` 的逆（逐行问 `Icon::contains`，参数也从
「窗口相对」改成和调用点一致的绝对坐标，消除 `doubleclick` 里那个 `cursorpos − position`）。

### 10.3 删掉 `adjust` 的证据（这条不是「顺手清理」）

删除绘制侧的 `-(2,3)` 会让道具/技能图标整体上移 2×3px，属视觉改动，必须先有判据。
判据在同批读到的 [r27-oracle/origin-rule.md §4](../r27-oracle/origin-rule.md)：

> **窗口出了 oracle → 该窗口按规则对齐，子层补 `shift(get_origin())`，并把手写的 `-(2,3)`
> 这类「补偿型 nudge」删掉；nudge 只有在表与语义对上之后才允许作为实测残差存在。**

KeyConfig 有 oracle（[r27-oracle/KeyConfig.json](../r27-oracle/KeyConfig.json)），
且 §3 的机器量尺已证「键帽位图左上角 == `keys_pos` 表项，误差 0」⇒ `keys_pos` 与绘制语义
已经对上，`adjust` 属于「表对上了但只有绘制侧还看得见」的遗留补偿，按裁定必须删。
该篇 §5 留下的残差是**另外 2px**（截图比 `backgrnd` 矮 4px 造成的锚点歧义），明确写了
「等 B8 真机抓帧」——那是表本身的相位问题，改的话应改 `keys_pos` 一次，
而不是再叠一个三处读法只有一处看得到的补偿。

### 10.4 同根因但本轮**不改**：底栏快捷格

`origin-rule.md §6` 记录了同一处「一格两种语义」在
[IO/UITypes/UIStatusBar.cpp:102](../../../../IO/UITypes/UIStatusBar.cpp#L102)
的 `QS_ICON_SHIFT_Y = 32`：只对键帽 origin (0,32) 成立，绑技能 (0,0) / 道具 (-3,-1) 时
图标整个掉到格外。本轮**没动**，理由与 §6 一致 —— 两张原生底栏帧里快捷格全是**未绑定**
动作键帽，画面中没有一颗技能/道具图标，「图标在格内的正确落位」量不出来；按 §4「量不出就不改」。
跟踪任务 **#89 / R27-B1d**，等 B8 真机抓一张已绑定技能（或道具）的底栏帧建 `StatusBar.json`。

（注：这一处恰好是 §10.1 契约要解决的形态——若那时底栏已有 oracle，`set_cell` 之后
`QS_ICON_SHIFT_Y` 这类按贴图 origin 手工分档的常数就没有存在的位置。）

### 10.5 本批改动清单

- [IO/Components/Icon.h](../../../../IO/Components/Icon.h) / [Icon.cpp](../../../../IO/Components/Icon.cpp)：新几何 API + `cell` 成员。
  ⇒ **公共类成员变化（布局变化），必须 `-t:Rebuild`**（增量会复用旧 obj，登录阶段 AV）。
- `UIEquipInventory.cpp`：draw `:254`、抓取 `:365`、命中 `:518-522`（空格回退到同一把尺）。
- `UIItemInventory.cpp`：draw `:372`、抓取 `:921`、`slot_by_position()` 整段重写为 `get_slotpos` 的逆 `:1508`。
- `UIKeyConfig.cpp`：删 `adjust`、三组双格逐格 `set_cell + draw_at` `:648-665`、未绑定托盘 `:691`、两处抓取 `:802/:846`、两处命中 `Point(32,32)` → `Icon::cell_width/cell_height`。
- `UISkillBook.h/.cpp`：`SkillDisplayMeta::draw(parent, cell)`、新增 `skill_cell(row)`、`send_cursor` 用 `Icon::contains` + `start_drag_at`、`skill_by_position` 改逆运算并改为绝对坐标。

`CHECKLIST.md` 的 **D7c** 行即本节。

### 10.6 未证事项（别当已完成验收）

> 2026-11-03 更新：本批 4 项里第 1 项的技能窗部分已在真机验证（见 §11），其余仍待补，
> 逐条最新状态在 **§11.4**。

1. **真机逐格**：背包 hover/拖放、键盘设定拖拽不跳格、技能窗 tooltip 与双击、装备窗回归，
   都要重跑 `tools/layout.py check`（可跑的 oracle：`Item.json` / `ItemFull.json` / `KeyConfig.json` / `Equip.json`）
   并部署抓帧。**技能窗没有 oracle**（`r27-oracle/` 里无 `Skill.json`）⇒ 那扇窗本轮只能做「hover/双击落在压住的那一格」的手感验收，量不出逐格 Δ。
2. **`skill_cell()` 现代双列等价性**：由「draw 循环 `i%2` 分列 + `skill_index()` 取号」的旧写法
   等价改写推来。`ROW_WIDTH=143 / ROW_HEIGHT=40` 是**上游现代窗常数**
   （[IO/UITypes/UISkillBook.h:167-168](../../../../IO/UITypes/UISkillBook.h#L167-L168)，出处见
   `docs/features/beidou-v17-parity/notes/r05-ui-layout.md`），本包**没有对它建过 oracle**；
   有实测尺的只有经典单列那档（CHECKLIST D11，`CLASSIC_SKILL_OFFSET` 由 `ikse.png` 新手圆点反推）。
   ⇒ 双列逐格落位没抓帧，现代窗若偏，改 `skill_cell()` 一处即可。
3. `keys_pos` 那 2px 残差（§10.3 末）仍在，等 B8。
4. `Rectangle::contains` 闭区间带来的 33px 覆盖（§10.1）在**列距=32** 的窗口会变成相邻格重叠。
   四窗实测列距 33/34/36 都 >32 故无重叠；日后加窄距窗口时要改用 `rect` 的显式右开语义，
   不要再叠 per-window 补偿。

## 11. B3-4a 真机逐格验收结论（2026-11-03，技能窗）

验收档用的是用户定的那一档：**「悬停：tooltip 弹出的就是鼠标压住的那颗图标」**，
加**「拖起瞬间：图标中心跟住光标、不跳一格」**。不是「元素在位」。

实例 hwnd=12256942、帧 1296×759、落位取自运行目录 `Settings` 的 `ui.position.skill=(560,99)`
（⇒ 帧上本格行起点 x=577、y=231/271/311/351）；全部坐标换算按 `live/README.md`
「帧 = 客户区 + 边框 (8,31)」这条实测，不猜。

### 11.1 已证（PASS）

| 判据 | 内容 | 实测 |
| --- | --- | --- |
| **J1 / J2** | 格内 3 点必出 tooltip、行距缝与列距缝必不出 | 格内面积 **41082~45418** px，空隙 **272~322** px，面积闸 1200 落在中间，**余量 >100×** |
| **J3a** | 同格 3 点是同一个框，只是跟着光标走 | 4 行 tooltip 尺寸 266×196 / 194 / 194 / 196，同格 3 点**逐像素相等**；框位移 == 光标位移（±12,±12 精确） |
| **J3c** | 弹出的就是压住的那颗（内容级） | 4 行标题 = 精灵的祝福 / 蜗牛投掷术 / 团队治疗 / 疾风步，框内图标与 `[Master Level]` 20/3/3/3 **逐行对上**，无一行报成邻格 |

J1/J2 那条 >100× 的分离度就是 #94 的命题本身：**命中区收敛到 `ms::Icon` 自己的 32×32 本格**，
旧版「40px 整段行距都算一格」的误认在真机帧上消失了。

### 11.2 J3b 定案为「度量本身的极限」，降级成诊断

一开始把「同格像素签名比跨行更近」（1-NN）当内容闸，八项读数**全部不过**：
同格 0.0707~0.0949、跨行最近 0.0775~0.1252，leave-one-out 最差余量 **−0.0003**。
排查后确认这**不是代码缺陷**，三条独立尝试都没能救回来：

| 尝试 | 结果 |
| --- | --- |
| 调高差集强度阈值（120/200/250/300/380） | 更糟——先把真 tooltip 的边缘切掉 |
| 投影/行直方图度量 | rows 1↔2 直接失败（两行同为三行式、长度相近） |
| 「volatile 像素标定」（停放帧自差 + dilate 掩掉底噪） | 标定掉 23391 px，读数几乎不变 |

根因：**tooltip 是半透明的**，差集里混进了「底下压的是什么」，所以同格/跨行读数天然搭界。
⇒ 像素级判据在这扇窗**不能当内容闸**；内容判据改走 J3c 读图（证件两张裁片
[live-20261103-skill-tips.png](live-20261103-skill-tips.png) /
[live-20261103-skill-rownames.png](live-20261103-skill-rownames.png)）。
这条写死在探针的 docstring 里，J3b 只报数、不过闸。

### 11.3 本批归档件

| 件 | 作用 |
| --- | --- |
| [live_verify_b34a.py](live_verify_b34a.py) | 探针本体（真机打键 + PrintWindow 抓帧 + J1/J2/J3a/J3b/J3c 判定） |
| [live_verify_b34a_offline.py](live_verify_b34a_offline.py) | 对**同一批已落盘帧**复算 J3b/J3c。`importlib` 载入上面的模块，用的是**同一套** `largest_blob / mask_disagree / sheet` —— 存在理由是客户端已退回登录界面且本 shell 无口令，但判据公式必须与归档件同源，不能另写一份 |
| [live_verify_b34a.json](live_verify_b34a.json) / [live_verify_b34a_j3b.json](live_verify_b34a_j3b.json) | 逐探针原始读数（bbox / 面积 / tipsize / 余量），全部 `int()` 化，可机器复核 |
| [live_verify_b34a.txt](live_verify_b34a.txt) | 人读表：J1/J2 分离度、J3a 八项、J3b 八项、J3c 四行，末尾列「本轮未跑」4 条与「已知非缺陷」1 条 |

**已知非缺陷**（省下轮重复排查）：技能窗行底位图的 origin 使「可见白框」比命中格偏 `(-6,-7)`——
图标美术本体落在命中格内，6px 装饰边不进命中区，原生同样如此，**不改**。

### 11.4 仍未证（§10.6 的那四条，状态更新）

1. `Item.json / ItemFull.json / KeyConfig.json / Equip.json` 的 `tools/layout.py check` 逐格对账 —— **待用户给一次 `R27_USER`/`R27_PASS`**（口令不入库，本 shell 未带；095 那套参考库另有处置待拍板）。
2. **拖起瞬间不跳格** —— 同上。新手号背包三页全空，判据改在**键盘设定窗 / 技能窗**上做，已写进 `live/README.md`。
3. **装备窗 6 格调色等值回归**（§9 过一次，本批改了抓取与命中，须复跑）—— 同上。
4. 本探针**整脚本 live 一次跑通**的读数落盘 —— J3b/J3c 已用同一批帧离线复算，J1/J2 是 live 原始读数。
5. §10.6-2「`skill_cell()` 现代双列等价性」**本轮没碰**：真机跑的是经典单列，双列仍无 oracle。

### 11.5 同批修正的两件工具（都不是可选清理）

- [`tools/live/login.py`](../../tools/live/login.py)：密码点击 `(582,330)` → `(642,354)`（旧值压在账号行）、连接 `(715,300)` → `(774,317)`（旧值在钮外）。2026-11-03 真机量出，docstring 已写「别再改回去」。
- [`tools/live/open_window.py`](../../tools/live/open_window.py)：输出帧名带序号（`_r27_win_01_i.png`），同一个键打两次（开→关）不再把开态帧覆盖掉；并记录「进世界后要先点一下窗口拿激活，否则热键不生效」。
