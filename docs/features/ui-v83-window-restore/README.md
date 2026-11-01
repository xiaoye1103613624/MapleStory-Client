# v83 客户端 UI 版式还原 · 最终实现逻辑

> 范围：`docs/features/ui-v83-window-restore/PLAN.md` 的 W0–W13（根因 RC1–RC15 + 待决 Q1–Q4）。
> 日期：2026-10-31。验收清单：[CHECKLIST.md](CHECKLIST.md)。逐窗取证：[evidence/](evidence/)。
> **验证轮之后**撞见的隐藏问题（含一条影响 14 个窗体的全局不透明度缺陷）：[notes/open-issues.md](notes/open-issues.md)。
> 边界：**纯客户端显示层**。未改服务端、未改 opcode、未写库、未动 `.img`/`.nxcache`（全程 `tools/nx_inspect.py` 只读）。

## 1. 结论：不是缺资源，是三类实现缺陷

> 验证轮补充：这条结论对**你报的 14 组缺陷**成立（版式/坐标/公共件），但**不覆盖全部**——
> 小地图 −/＋ 缩放钮、`UIOptionMenu` 面板内容属于**本包真的没有素材**（A 类之外的素材代次缺口），
> 已单列在 [notes/open-issues.md §C](notes/open-issues.md)。另有一条**全局缺陷**（UI 层把帧插值系数
> 当不透明度）此前不在任何一类的射程内，见 §A1。

两端 `Data/UI/*.img` SHA256 全同；原生 `BeiDou.exe` 内字符串 `UIToolTip` **0 命中**。所以问题不在包，在码：

| 类 | 表现 | 本质 |
| --- | --- | --- |
| 走错资产代次 | 滚动条 / tooltip / `UIWindow2/4/6` / `StatusBar3` / `chat/ingame` 全无图 | HeavenClient 偏新路径，本包（v83 部署包）只有 14 个 UI 根 + `UIWindow.img` 283 个窗体根 |
| 公共件静默失效 | 「有框无底」「`usable()` 形同放行」 | present-but-empty 节点判不出来；缺件时回退到不存在的根也不报警 |
| 默认坐标 `(0,0)` | 按钮叠在窗口原点、跑到左上角 | `try_maple_btn` / `TwoSpriteButton` 的 `pos` 默认 `{}`，大量调用点不传 |

## 2. 三层机制（W0 公共件，其余批次全部受益于此）

### 2.1 资产存在性判据
`Util/UiAssetResolve.cpp`：

```cpp
bool usable(nl::node n)   // 旧实现只是 bool(n)，空壳节点判不出来
{
    if (!n) return false;
    if (n.size() > 0) return true;
    return n.data_type() == nl::node::type::bitmap;
}
```

`window(name)` = `UIWindow2.img[name]` → 回退 `UIWindow.img[name]`；`window4(name)` 再多试 `UIWindow4.img`。回退不再静默：`diag_once(tag, msg)` 往运行目录 `_ui_asset_diag.log` **同一条只写一行**（构造期探针安全，无刷屏，非临时日志）。

### 2.2 坐标契约（最容易踩的一条）
`DrawArgument::get_rectangle()` 做 `rlt = pos - center - origin` ⇒ `ms::MapleButton(src, p)` 的 `p` 是**锚点**。而批次里想要的是**贴图左上角**。统一由新公共件承担换算：

```cpp
bool try_maple_btn(map<uint16_t, unique_ptr<Button>>& buttons, uint16_t id, nl::node src,
    Point<int16_t> position = maple_btn_nopos(), const std::string& site = {});
// usable(src)==false 或 position==哨兵(-32768,-32768) → 不创建 + diag_once
// 否则 buttons[id] = MapleButton(src, position + Texture(src["normal"]["0"]).get_origin())
```

同时**删除了散落在 5 个文件里的匿名 namespace 复制版**（UIEquipInventory / UIKeyConfig / UISkillBook / UIStatsInfo / UIItemInventory）——原来那批副本各自解释坐标语义，是「同一个钮在不同窗错位」的结构性原因。
副作用（有意为之）：不传坐标就不创建。所以 W6–W10 必须与 W0.3 同批发布，否则现有「恰好能用」的叠位钮会消失。

### 2.3 滚动条根名钳制
`IO/Components/Slider.cpp`：`VSliderType` 枚举逐项算值，四处调用点传 `DEFAULT_SILVER`＝9 → `Basic.img/VScr9`，而本包只有 `VScr`…`VScr8`。现非 CHATBAR 分支不存在则钳到 `Basic.img/VScr` 并记一条 diag（`Slider.cpp:61-75` 的子件命名本来就已对齐本包，越代的只是根名）。

### 2.4 九切片 / 三切片的 origin 补偿
`NameTag`（W12）确立写法，`UIMiniMap`/`UIWorldMap`（W11）沿用：切片件 `origin` 常非零（如 `w.origin.x == width`），必须逐件补偿后再定位；`e`（中段）按目标宽平铺；框宽 = 左帽 + 文本量宽 + 右帽。

> 取证工具修复：NX PKG4 向量节点的 `(x,y)` 是内联在 4 字节 `data` 里的两个 int16，`tools/nx_inspect.py` 旧版把它打印成指针样乱码。
> 现按 `struct.unpack("<hh", struct.pack("<I", data))` 解出 ⇒ **origin 已可信**（[notes/asset-node-inventory.md](notes/asset-node-inventory.md) §0.2 的「origin 不可信」结论作废）。

## 3. 逐窗最终逻辑

### 底栏 HUD（W1/W2/W3/W4）
- **数值字库**：HP/MP/EXP 由 `Text(A11B, YELLOW)` 改为 `StatusBar.img/number` 位图 `Charset`（数字 5×7、`[ ]` 2×9、`/` `%` 7×7）。`number` **无 `.` 字形** → `draw_hud_number()` 自定推进模型：`[ ]` = `getw+2`、其余 = `getw+1`、`.` = 4px 且用 1×1 白点贴基线；7px 字形相对 9px 括号行下移 1px 居中。
- **LV 徽章**：`Basic.img/LevelNo`（11×13）位图，`ShowLevel`+`Bracket` 仅作缺失回退。
- **量规**：`gauge/bar`、`gauge/graduation` 是 **340×31 同行三联图集**（不是上下叠放），段偏移直接取打包间距 `0/109/222`；`HP/MP/EXP` 三个字标签**已烘焙在图集里** ⇒ 不再建 `Text`。
- **行位**：职业名 rel `(87,40)`、角色名 `(87,56)`（旧 `(21,39)` 压在聊天行＝「职业没显示」的真因）；`backgrnd` 817×71 不再拉伸到 VWIDTH。
- **快捷格**：按格数选 `base/quickSlot08|26|32`（151/466/571 宽，几何由图集反推：格 28、列距 35、行距 34、首格 (9,9)），两行；键帽层 = `key/0..6` 贴纸（Shift/Ins/Hm/Pup/Ctrl/Del/End）+ `FontMemo` 位图数字（1–5）+ 描边文字回退（本包 `Basic.img` 71 根全查无字母位图字体；`key/7` 是 28×20 红箭头不是 Pdn）。`QuickSlot`/`QuickSlotD` 收成**同位单钮**按状态换图标。废弃旧的「800 设计网格 + right_adj」居中（IDA 簇 x 末端 795/796 反证原生就是左贴边）。
- **四钮行为**：`帮助`→ `Npt_GuideLine.img/NptGuide/Text00..48` 文本窗 + `VScr` 滚动（不再用 `Login.img/Notice`）；`界面`→ 只开 `UIShortCut`，11 颗真 `Bt*`（`x=6, y=20+20i`，底板 93×245）并接到既有窗口；`商城`→ 明确「暂未支持」提示，**opcode 不动**；`键盘设定`窗 5 颗钮补坐标（此前因哨兵被整批跳过＝「窗里没按钮」）。
- **聊天栏**：`base/chatTarget` 81×20 目标条 + `BtWhisper` 12×19，展开/缩放用真件 `Basic.img/BtMax`/`BtMin`（互斥），删掉手搓 12×12 命中区；日志框换共享 `TooltipBackdrop`。
  **另发现功能级根因**：classic 模式 `is_in_range` 把命中区整体上移 100px ⇒ 光标事件**从不**派发给聊天栏，这才是「点了没反应」。

### 角色类窗口（W5–W9）
- **tooltip**：根 `UI["UIToolTip.img"]["Item"]`（不存在）全部清零；内容图标改 `UIWindow.img/ToolTip/Equip/{Can,Cannot,Property,Growth*,Speed,WeaponCategory,ItemCategory}` + `cash`/`mesos`；**框体真身定案 = 程序绘制** `TooltipBackdrop`（黑 0.9α + 1px 灰边 (0.5,0.5,0.5)，托管在 `TextTooltip.h`），依据是本包 5 族九切片被逐族像素判否（都是浅色/透明/条状风格）。装备框固定宽 264。
- **能力值**：`backgrnd` 208×347 基准重排；`basicStat` 总是画 (8,195)；13 行数值重测（DEX/INT/LUK 曾低 10/18/28px）；`BtApUp`×6 → (186,115…299)；`BtDetail` 角锚 kx=9/ky=4 → (153,324)；**收缩钮真身 = `Basic.img/BtHide`**（13×12，SAD 模板精确命中）在 (405,326) 且仅展开态可点 —— 旧代码取不存在的 `main/BtDetailClose` 使 `set_detail(false)` 永不可达。数值色定案 BLACK+A11M（原生是比例文字，无位图字库可用，已写明理由）。
- **装备**：「左上两钮弹二级密码」定案为**误读烤图**——那是 `Equip/BtDetail`「详细信息」46×19 与 `Equip/BtCashshop`「Shop」32×11（全仓/全客户端/exe 内「二级密码」0 字节命中），原生**默认隐藏**两颗（匹配度 mae 55.8/61.1）；宠物区 `pet[177x181]` + `BtPet1/2/3` + `BtPetEquipShow/Hide` 按右下锚算位，展开只改 `dimension`。
- **背包**：tab 底衬 = `Item/New/Tab0`(灰)+`New/Tab1`(粉=选中) 叠 `Item/Tab/enabled|disabled` 文字，`x=5+34i, y=24` 且**只有 0–4**（旧代码取到 5 → 静默失败，表现为 SETUP/ETC 错位）；`BtFull`/`BtSmall` 合并到同槽 `(W−51,4)`（曾两颗叠在 (109,6)），`BtGather`/`BtSort` `(W−31,4)`；`set_full` 按 `dimension.x()` 重排全部工具位，金币标签 (141,275) 不再溢出 289 框高。
- **技能**：5 tab 槽0 中心 21、间距 33（由新手圆点实测 (16,29) 反推）；行底 `skill0/skill1[141x35]` + `line[141x1]`；`CLASSIC_SKILL_OFFSET` (10,87)→**(7,99)**（旧值＝贴图与文字 12px 错位的来源）；`+SP`→(132,119)；宏指令钮 `Basic.img/BtMacro` (121,265)（**不在** `Skill` 根下）；`recommend[0]`（105×35，origin 16,0）作顶部职业横幅接在 (5,20)。

### 任务 / 地图（W10/W11）
- **任务**：**先量后建**（`measure` → `relayout_slots` → `try_maple_btn`）解决 `close_pos/marknpc_pos` 在按钮构造之后才赋值 ⇒ 双双落 (0,0)；关闭钮定案 = `Basic.img/BtClose`（12×12 蓝底白叉），`BtUIClose`（32×15「X CLOSE」是对话框件）仅作后备；分类用真 `Quest/Tab{enabled,disabled}`+`icon2..icon9` 并**纠正了 Tab 贴图状态倒置**；补 `send_cursor` + `row_index_at` 实现**单击选中**（双击保留）；进度条 `Gauge{frame[163x14],gauge[1x11],spot[1x11]}`；`BtMarkNpc` 回到底带 (458,370)。
- **小地图**：`MinMap`/`MaxMap` 九切片拼框（角不拉伸、边单轴拉伸、`c` 铺内容区、逐件 origin 补偿），`Min` 三切片做收缩条，顶带补地图名；越代件 `BtMin/BtMax/BtSmall/BtBig/BtNpc` 停止实例化（本包只有 `BtMap[36x12]`），`BtMap` 从 y=−6 归位进框；`MAX_ADJ` 40→43 对齐 MaxMap 72 顶带；simpleMode 的死取用（`MiniMap/Window`）改为回落 + diag。**2048px canvas 守卫未放宽**（它挡的是 5944×1311 的教学图）。
- **大地图**：不再拿 `Border["0"]`（60×33 的一条边）当整窗底板，改用 `Border{0..7}` 拼九切片窗框；底图（带 origin 320,0）落在内容区 `base_position=(7,33)`；默认位置 `PosMAP (100,35)`→**(313,64)**（654×521 于 1280×720 居中偏上）、`PosMINIMAP (0,0)`→`(5,5)`。

### 名牌（W12）
`NameTag.img/<id>{c,e,w,clr}` 正确用法：三切片 + 逐片 origin 补偿；`clr` **必须按 `data_type()` 分派**（id 178 存的是字符串 `-4096`，而 `nl::node::get_integer()` 对非数字串会抛 `includes/NoLifeNx/nlnx/node.cpp:169`）；`clr` 语义是**文字色**（深底白字/浅底黑字），`Text` 只吃 `Color::Name` ⇒ 白字模 × `DrawArgument` 乘法着色。资产事实：id 3…254 中只有 **92 个**存在；`medal`(130，独立 id 空间) 与 `pet`(289) 各自独立；同一 id 三片**高度可不同**（id 40 = 32/34/19）⇒ 逐片垂直居中而非统一高。

## 4. 待决项结案

| # | 结论 |
| --- | --- |
| Q1 tooltip 框体真身 | **程序绘制** `TooltipBackdrop`（§3 W5）。包内 5 族九切片全部被像素判否；原生 exe `UIToolTip` 0 命中 |
| Q2 能力值收缩/减点钮 | 收缩 = `Basic.img/BtHide`；`BtApDown` 本包不存在 → 未接（原生 v83 无减点钮） |
| Q3 装备两钮「二级密码」 | **误读烤图**，两颗默认隐藏。文案在仓库/客户端/exe 内 0 字节命中 |
| Q4 任务「任务整理」 | **本包无此控件**：导出 131 张 `Quest/*` 位图逐张看图识字，最接近的是 `Quest/BtAlert`＝「任务讯息」→ `set_active(false)` 预留 |
| RC7 商城 | 出站 40 vs `CASHSHOP_OPERATION=229`、HUD 换态靠 recv `SET_CASH_SHOP=127` —— 属跨 s9 协议线，**本线不换**，挂 #24 T14；本线只把「没反应」变「明确提示」 |
| NPC 对话乱码 | **不属本线**，转 T13。取证已转记：`UINpcTalk.cpp:467-519` 残留收尾 `#`、`:157-165` 逐字节打字机 |

## 5. 预留清单（显式，非静默）

`BtAuto`（能力值自动加点）｜宠物槽**穿戴逻辑**｜modern 装备分支 6 颗（`btEffect/btSalon/BtConsumeSetting/BtException/BtShop/BtSlot`，UIEquipInventory.cpp:288-293）｜`Skill/recommend` 显示条件（新手参考帧里看不到，疑高转职/选中态）｜任务 `BtOK/BtNo/BtAlert/BtMarkNpc` 动作（无现成协议，不发明）｜`UIGameMenu` 的**「场景变更」一颗**（本客户端无任何皮肤 UI/协议线 → 点它出黄字「场景变更暂未支持。」；其余 4 颗已实装，见 [notes/open-issues.md §A2](notes/open-issues.md)）｜大地图 `ListNpc`/`WorldMapSearch`/`BtSearch`/`BtAutoFly_1`/`BtNaviRegister`/`MiniMapSimpleMode`｜游戏内名牌接真 id（需改 `Npc/Mob/Char/UIStateGame`，未授权）｜任务窗 Tab 粉底/灰底、黑边框、选中行横条（本包无位图，**保留 ColorBox** 并附实测色值）｜小地图 −/＋ 缩放钮（本包无 `BtMin/BtMax/BtSmall/BtBig/BtNpc` 与 `Window/Max` 子树，素材代次差异）。

## 6. 构建与产物

| 项 | 值 |
| --- | --- |
| 工具链 | **VS 18 Community**（`D:\software\Microsoft Visual Studio\18\Community`）的 MSBuild；工程 `PlatformToolset=v145` ⇒ 2019 BuildTools 会报 `MSB8020 找不到 v145` |
| 命令 | `MSBuild MapleStory.sln -p:Configuration=Release -p:Platform=x64 -p:OutDir="...\build_check\\" -m` |
| 结果 | 0 error；警告均为既有噪声（C4804/C4806 属另一条未提交工作线的 `WaveAShellHandlers.cpp`，C4005 `USE_NX` 重定义、C4828 nlnx 头、LNK4098/4099 既有） |
| 修的两处编译断点 | `Graphics/SpecialText.h`：`OutlinedText` 只有 `width()`，W12 用到 `height()` → 补 `inner.height()` 转发；`UIMiniMap.cpp`：`std::max` 混型 ⇒ 显式 `<int16_t>` |
| 并行禁忌 | 两个 MSBuild 同时跑会争用同一个 `x64/obj-Release/vc145.pdb` ⇒ 成片 `C1041`。串行构建（也印证批次约定里「不许各自构建」） |
| 部署身份 | **当前线上 = `6AE608CB` / SHA256 前 16 `ff305567d5a23546` / md5 前 16 `4231856851ccfa29` / 4,357,632 B**（2026-10-31 23:27 本地构建，含 A7 `UIStatusBar` 空指针修复；上一件 `6AE5FE0D / 67145c64a74c67b3` 已备份为 `MapleStory.exe.bak_pre-statusbar-fix-20261031`）。**这一件已被真机跑过并完成整轮键位扫描，全程未再崩溃**（Release 无客户端日志，只能靠这个认版本）。⚠ **读戳偏移只能是 `e_lfanew + 8`**：`+4` 读出的是 Machine+NumberOfSections，会得到 `427620`（1970-01-05）这种看着像「戳坏了」的假值——本轮误判过一次，实际戳一直正常。历史：W0–W13 交付件（4,356,096 B）→ 20:06 件 4,357,632 B → A1/A2/A3 件 4,358,656 B → OutlinedText 件 `6AE5EC18 / 547d2d37e956756e` → `6AE5ED29 / 0756a986a9f39d93` → `6AE5FE0D / 67145c64a74c67b3` → **`6AE608CB`（当前）**。 **待部署**：`14A40006` / md5 前 16 `13eba5ac7782ece2` / 4,357,632 B（00:17:41，EXITCODE=0），含「关窗后吞键」「tooltip 需求行距」「补零下溢」「职业名叠画」「背包中间那颗 `+`」「`sort_enabled` 初值」六项；`cp` 到客户端被 `Device or resource busy` 挡住（客户端在跑），备份 `MapleStory.exe.bak_r13-20261031` 已落好 |
| 回退点 | `E:\MXD\BeiDou-Client-xy\MapleStory.exe.bak_pre-uialpha-20261031`（= 20:06 件）；另有 `bak_pre-ui-v83-restore-*`、`bak_pre-t12b-*`、`bak_pre-t11-*` |

## 7. 提交建议

本轮改动横跨 **约 22 个源文件**，但工作树里**还叠着另一条未提交的工作线**（缺失系统外壳：`UIEnergyBar/UIMono*`、`FieldMarker`、`MapSummon`、`MapMist`、`WaveAShellHandlers` 等，以及 `IO/UIElement.h`、`MapleStory.vcxproj` 被两条线同时改）。
⇒ 想做「W0 一批 + 逐窗一批」的原子提交，需要先 `git stash`/`git add -p` 分离，**不建议整树一次提交**。等你指示再动 git。

## 8. 方法论（可复用）

1. **先做全局事实表**再动代码：`nx_inspect list` 出权威根/件清单（WxH + origin），逐窗标 ✓/✗ —— 一次投入，省掉后面每次「这按钮该不该有图」的争论。
2. **三类缺陷分开治**：公共件（W0）先行，逐窗只改「取用 + 坐标」，不各自发明判定逻辑。
3. **坐标推导优先级**：① 烤进底板的不重画 ② 角锚算术（`x = W_bg − w − k`）③ 截图实测（三步算式进 evidence）。
4. **判否要留像素证据**：候选素材导出 PNG、算 mae/SAD，再下「本包没有 / 原生不用它」的结论（Q1、Q2、Q3 都靠这条）。
5. **区分版式与功能**：同一条「点了没反应」可能是命中区偏移（W4）而不是画错。验收清单里分开列。
6. **跨代次件一律钳制 + 记一次日志**，禁止静默透明 —— 否则下个窗口又把同一颗越代件接回来。
7. **判颜色前先量整帧 max**：PrintWindow 抓帧可能整帧被压暗（实测出现过 max=197=255×0.7725 的帧），
   这种帧只能用于版式/几何。
8. **怀疑「素材颜色不对」时，用像素计数比对而不是目测**：把抓帧区域的颜色直方图和 `nx_inspect`
   导出的素材直方图并排 —— 若各色**像素个数逐个对上、只有通道值同乘一个常数**，那就不是素材差异、
   也不是渲染错色，而是**不透明度被污染**（A1 就是靠这一招定性的，见
   [evidence/w11-map/pixels-alpha.md](evidence/w11-map/pixels-alpha.md)）。
