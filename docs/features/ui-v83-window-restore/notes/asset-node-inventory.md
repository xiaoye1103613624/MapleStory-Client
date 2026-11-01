# UI.nx 权威节点清单（v83 部署包实测）

> 取证方式：`python tools/nx_inspect.py list "E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx" <depth> <path>`（只读）
> 原始转储：[raw-nx-dumps/](raw-nx-dumps/)；日期：2026-10-31
> 原生路径字符串锚点：[native-ui-asset-strings.txt](../notes/native-ui-asset-strings.txt)（BeiDou.exe 内 38 条 `UI/*.img/*` 字面量）

## 0. 三条全局事实（先记，后面别再猜）

1. **本包只有 20 个 UI 根**（2026-11-01 用 `Nx.children(0)` 重数，原文写「14 个」是漏数）：
   `Basic BuffIcon CashShop CashShopPreview ChatBalloon DialogImage GuildBBS GuildMark ITC ITCPreview Login Logo MapLogin MapleEzorsiaV2wzfiles MapleTV NameTag Npt_GuideLine StatusBar UIWindow tutorial`。
   **不存在**：`UIToolTip.img`、`UIWindow2/4/6/GL.img`、`StatusBar3.img`、`BuddyList/GuildUI/Party/Storage/Trade/Gateway.img`。
   → 凡代码引用这些根，一律是"走错资产代次"（见 PLAN RC1/RC3/RC5）。原生 exe 内 `UIToolTip` **0 命中**，可反证。
   ⚠ **但其中 3 个只是"根名换了代次"，v83 真身在 `UIWindow.img/` 下**（实测件表见 §1.5）：
   `Storage.img`→`UIWindow.img/Trunk`、`Trade.img`→`UIWindow.img/TradingRoom`、
   `BuddyList.img`→`UIWindow.img/UserInfo`。`GuildUI.img`/`Party.img`/`Gateway.img` 本包**确实没有**。
2. **`nx_inspect` 的 `origin [vector]` 行不可信**（打印成指针样乱码），只有 **WxH 与子件名**是权威。需要 origin 时用 `image` 子命令导出 PNG 量边。
   → 2026-11-01 已修：`tools/nx_inspect.py` 的 `value()` 现按 type-4 双 int32 解析，origin 可信（见该函数注释里的 world-map BaseImg (320,235) 案例）。
3. **`UIWindow.img` 有 112 个顶层窗体根**（原文写 283，系把深层节点混计），含共享件 `BtUIClose`、`BtUIClose2`、`IconBase`、`Macro`、`ToolTip`。

## 1. 各窗体可用件（✓=包内有，✗=包内没有）

### `UIWindow.img/ToolTip`
`Equip{Can{0-9,beginner,bowman,magician,max,none,percent,pirate,reqDEX,reqINT,reqLEV,reqLUK,reqPOP,reqSTR,thief,warrior}, Cannot{…}, Dot{0,1,2}, GrowthDisabled{…}, GrowthEnabled{…}, ItemCategory{1-10,12,21}, Property{0-18}, Speed{0-6}, WeaponCategory{30-47子集}}`、`cash[23x5]`、`mesos[30x5]`
- ✗ **无九切片**（`c/e/ne/n/w/sw/s/se` 全无）→ PLAN §6-Q1
- ✓ 原生 exe 字面量证实 `ToolTip/Equip/GrowthDisabled|GrowthEnabled` 是真实取用路径

### `UIWindow.img/Item`
`backgrnd[175x289]`、`FullBackgrnd[603x289]`、`Tab{disabled{0-4}[26-27x14], enabled{0-4}[27x15-16]}`、`New{Tab0{0-4}[34x18], Tab1{0-4}[34x19], inventory{0-3}[36x36]}`、`BtCashshop[93x18]{4态}`、`BtCoin{d,mo{0-3},n,p}[14x14]`、`BtFull[12x12]`、`BtGather[12x12]`、`BtSmall[12x12]`、`BtSort[12x12]`、`Quality{0-3}[5x5]`、`activeIcon[36x36]`、`disabled[32x32]`、`shadow[26x6]`、`bossPetIcon[34x38]`
- ✗ `BtSetup`、✗ 滚动条、✗ 关闭钮（用 `BtUIClose`/`Basic.img/BtClose2`）
- ⚠ `Tab` 只有 0–4（5 个分类），代码若取 `"5"` 会静默失败

### `UIWindow.img/Stat`
`backgrnd[208x347]`、`backgrnd2[218x203]`、`basicStat[159x121]`（＝蓝"基本属性点"头）、`BtDetail[46x19]`、`BtApUp{4态}[12x12]`、`BtAuto[100x35]`、`BtAuto1[73x17]`、`BtAuto2[73x17]`、`Disabled{DEX,INT,LUK,STR}[48x16]`、`Job{main{0-5,premium}, sub{0-5,premium}}`
- ✗ `BtApDown`、✗ `BtDetailClose` → PLAN §6-Q2
- ⚠ 代码还取了 `cover0/cover1`，包内不存在

### `UIWindow.img/Skill`
`backgrnd[175x289]`、`Tab{disabled{0-4}, enabled{0-4}, AranButton{Bt1..Bt4}}`、`skill0[141x35]`、`skill1[141x35]`、`line[141x1]`、`recommend{0}[105x35]`（＝顶部职业横幅）、`tip0/tip1/tip2[193x102]`、`BtSpUp{4态}[12x12]`、`CoolTime{0-15}[32x32]`
- ✗ `main` 节点（代码先取 `main[...]` 再回退 `Skill[...]`，回退是对的）
- ✗ 宏指令钮在本根 —— 实际是独立根 `UIWindow.img/Macro`
- ✗ 滚动条（用 `Basic.img/VScr*`）

### `UIWindow.img/Equip`
`backgrnd[175x304]`、`FullBackgrnd[208x291]`、`pet[177x181]`、`BtPet1[30x27]`、`BtPet2[30x27]`、`BtPet3[30x27]`、`BtPetEquipShow[67x18]`、`BtPetEquipHide[17x16]`、`BtCashshop[32x11]{4态; mo/n 有 0-5}`、`BtDetail[46x19]`
- ✗ `backgrnd2`（代码取了它）、✗ 绳索、✗ 关闭钮

### `UIWindow.img/Quest`
`backgrnd[245x396]`、`backgrnd2[305x396]`、`backgrnd3[296x289]`、`backgrnd4[305x396]`、`backgrnd5[266x117]`、`Tab{disabled,enabled}`、`icon0[13x14]`、`icon1`、`icon2{0-7}`、`icon3{0-6}`、`icon4`、`icon5{0-4}`、`icon6{0-7}`、`icon7{0-11}`、`icon8{0-7}`、`icon9{0-7}`（＝8 组分类图）、`BtAlert/BtDetail/BtGiveup/BtMarkNpc/BtNo/BtOK`（各 5 态）、`Gauge{frame[163x14],gauge[1x11],spot[1x11]}`、`Gauge2{frame[79x8],gauge[1x4]}`、`notice{0-2}[73x24]`、`notice0[124x35]`、`notice1[124x34]`、`notice2[102x33]`、`basic[71x14]`、`prob[60x14]`、`reward[72x20]`、`select[60x14]`、`summary[74x22]`、`TimeQuest{AlarmClock,TimeBar}`
- ✗ 关闭钮（用 `BtUIClose`）、✗ 滚动条
- ✓ 原生 exe 字面量证实 `Quest/Gauge/{frame,gauge,spot}` 真实使用

### `UIWindow.img/MiniMap`
`BtMap{d,mo,n,p}[36x12]`、`MinMap{c[1x1],e[6x1],n[1x29],ne[6x29],nw[60x29],s[1x14],se[6x14],sw[6x14],w[6x1]}`（**完整九切片**）、`MaxMap{…[x72]}`（**完整九切片**）、`Min{c[1x20],e[4x20],w[8x20]}`（三切片）、`title[1x1]`
- ✗ `BtMin/BtMax/BtSmall/BtBig/BtNpc`（代码全都在取，只有 `BtMap` 存在）

### `UIWindow.img/WorldMap`
只有 `Border{0[60x33],1[1x33],2[7x32],3,4[7x1],5,7[7x18],6[1x18]}` + `title[1x1]` → 大地图必须靠 `Border` 拼边 + `Map.nx/WorldMap/*.img` 区域素材

### `UIWindow.img/{GameMenu, ShortCut, SlideMenu}`
- `GameMenu`：`BtChannel/BtGameOpt/BtQuit/BtSkin/BtSysOpt`（各 4 态）+ `backgrnd[93x140]`
- `ShortCut`：`BtComm/BtEquip/BtGuild/BtItem/BtMessenger/BtMobbook/BtParty/BtQuest/BtRanking/BtSkill/BtStat`（各 4 态）+ `backgrnd[93x245]` ← **当前代码一个都没实例化**
- `SlideMenu`：`{0,1}` 各含 `BtArrow/BtCancle/BtMain/BtMove/Choice[40x40]/Cover[142x38]/backgrd/info[string]/IsAniBackgrd[int]`

### `StatusBar.img`
- `base`：`backgrnd[817x71]`、`backgrnd2[570x71]`、`box[42x19]`、`chat[566x5]`（**细线，不是输入框**）、`chatTarget[81x20]`、`iconBlue[12x12]`、`iconMemo[14x11]`、`iconRed[12x12]`、`quickSlot[466x80]`、`quickSlot08[151x80]`、`quickSlot26[466x80]`、`quickSlot32[571x80]`
  - ✗ `gauge` 子树（gauge 是 `base` 的兄弟根）、✗ 展开/缩放钮（`base` 下没有，得用 `Basic.img/BtMax/BtMin`）
  - ✓ 原生 exe 字面量 `UI/StatusBar.img/base/chatTarget`、`UI/StatusBar.img/BtNPT`
- 兄弟根：`BtClaim BtMenu BtNPT BtShop BtShort BtWhisper EquipKey FontMemo InvenKey KeySet QuickSlot QuickSlotD SkillKey StatKey gauge key number`
- `gauge`：`bar[340x31]`、`graduation[340x31]`、`gray[1x16]`、`hpFlash{0-4}[109x18]`、`mpFlash{0-4}[109x18]`、`tempExp[1x16]`
- `number`：`0-9`、`Lbracket[2x9]`、`Rbracket[2x9]`、`percent[7x7]`、`slash[7x7]`，数字本体 **5×7** ← HP/MP/EXP 数值的正确字库

### `Basic.img`（71 根）
- 按钮：`BtCancel BtCancel2 BtCancel3 BtClaim BtClose BtClose2 BtCoin BtComboBox BtDecide BtDel BtDown BtHide BtHide2 BtHide3 BtMacro BtMax BtMax2 BtMin BtMin2 BtNo BtNo2 BtOK BtOK2 BtOK3 BtQGiveup BtReceive BtSend BtUP BtYes BtYes2`
  - ⚠ **`BtClose3` 不存在** → `basic_close_btn()` 的回退链实际永远落在 `BtClose2`
- 控件：`CheckBox ComboBox..ComboBox5 Cursor HScr..HScr6 KeyDownBar KeyDownBar1 Notice..Notice5 PQuestRank Slider Tab..Tab7 VScr..VScr8 YesNo YesNo2 YesNo3 dcMark icon{arrow,check,delivery}`
  - **滚动条根名 = `VScr`…`VScr8`（无 VScr0/VScr9）**；子件 `disabled->{base,next,prev}`、`enabled->{base,next0,next1[,next2],prev0,prev1[,prev2],thumb0,thumb1[,thumb2]}`；宽 12–15，thumb 高 25
  - `Slider.cpp:61-75` 的子件命名**已与本包对齐**，问题只在根名越代（PLAN RC1）
- 位图数字字库：`ItemNo{0-9}[8x11]`、`LevelNo{0-9,blank}[11x13]`、`ShowLevel{0,1,2,Bracket}` ← LV 徽章的正确素材

### `NameTag.img`
按 id（3,4,…,254）分组的**三切片** `{c, e, w, clr}`（`clr` 是 ARGB int，偶见字符串如 `-4096`）；另有 `medal{~130 id}`、`pet{~289 id}` 同构分组。✗ 无 `Frame`。
→ 名牌/气泡框的正确做法（PLAN W12，也是 #29 未拍到的 NAMETAG 面）

### 其它
- `Npt_GuideLine.img` **纯文本零位图**：`NptGuide{Text00..Text48 中文}` + `Version{Number[int 2]}` → "帮助"钮的正确内容（PLAN RC9）
- `CashShop.img` 23 根（`Base CS3thEvent CSChangeName CSChar CSDiscount CSGift CSGift CSIcon CSInventory CSItemSearch CSList CLocker CSMemberShop CSNotice CSStatus CSTab CSTransferWorld CSWebMsg CashItem GuideWords NewItem PicturePlate PrepaidCashItem）→ 商城素材齐，"点了没反应"是接线/协议问题（PLAN RC7）
- 包内**全部**完整九切片只有 5 族：`ChatBalloon.img/pet/*`、`ChatBalloon.img/dead`、`Login.img/WorldNotice/Balloon`（9×9 角 + `clr`）、`Login.img/WorldNotice/BalloonForLoginProcess`、`UIWindow.img/MiniMap/{MinMap,MaxMap}`、`UIWindow.img/UserList/Party/PartyHP`

## 1.5 「根名换代次」三窗实测件表（2026-11-01，`nx_inspect list … 3`）

> 用途：`Storage/Trade/BuddyList` 三窗的 v83 真身。换根本身有依据，**但窗内按钮坐标仍缺**
> （无 IDA、原生参考帧刚补齐）⇒ 坐标落定前不改根，只按此表登记。
> `try_maple_btn` 的 `position` = 贴图**左上角**。

### `UIWindow.img/Trunk`（＝代码里的 `Storage.img`）底衬 `backgrnd[463x318]`
| 件 | 尺寸 | 说明 |
| --- | --- | --- |
| `BtSort` | 71x18 | 四态齐全（＝旧代码要的 `BtArrange` 整理钮真身）|
| `BtPut` / `BtGet` / `BtGetAll` / `BtExit` | 各 70x19 | 存 / 取 / 全部取出 / 退出（`BtExit` 即本窗关闭钮）|
| `BtCoin` | 36x18 | 仓库金币区钮 |
| `BtInCoin` / `BtOutCoin` | 各 36x18 | 金币存入 / 取出（＝旧代码要的 `BtMesoIn/BtMesoOut`）|
| `Tab{disabled 26-27x14, enabled 27x15-16}` | 0–4 | 与 `UIWindow.img/Item/Tab` 同尺寸 |
| `en[199x35]`、`select[162x35]` | — | 标题/选中条 |
- 代码现取 `BtArrange/BtMesoIn/BtMesoOut/BtClose` 四个名字，**本根一个都没有** ⇒ 名字映射为
  `BtArrange→BtSort`、`BtMesoIn→BtInCoin`、`BtMesoOut→BtOutCoin`、`BtClose→BtExit`（语义按件名，未见原生文案）。

### `UIWindow.img/TradingRoom`（＝代码里的 `Trade.img`）底衬 `backgrnd[565x474]`
| 件 | 尺寸 |
| --- | --- |
| `BtTrade` 60x19、`BtReset` 60x19、`BtCancel` 60x18 | 四态齐全 |
| `BtCoin` 14x14 | `mouseOver` 下是 **4 帧动画**（0–3），其余态单帧 |
| `icon1[12x10]` | — |
- 代码现取 `BtAddMeso/BtLock/BtTrade/BtCancel/BtChat`：只有 `BtTrade/BtCancel` 命中，
  `BtAddMeso/BtLock/BtChat` 本根**没有**（原生交易窗的金币/锁定走 `BtCoin` 与右键菜单）。

### `UIWindow.img/UserInfo`（＝代码里的 `BuddyList.img`）底衬 `backgrnd[275x199]`，另有 `backgrnd2[275x399] / backgrnd5[275x361] / backgrnd7[275x404]`（展开态系列）
| 钮 | 尺寸 | 钮 | 尺寸 |
| --- | --- | --- | --- |
| `BtRegister` | 31x17 | `BtDelete` | 31x17 |
| `BtMeso` | 31x17 | `BtSearch` | 38x17 |
| `BtTrade` | 59x17 | `BtParty` | 59x17 |
| `BtItem` | 60x19 | `BtWish` | 60x19 |
| `BtPresent` | 45x13 | `BtFamily` | 40x37 |
| `BtBookShow/Hide` | 48x17 / 48x17 | `BtPetShow` 49x17 / `BtPetHide` 48x17 |
| `BtCollectionShow/Hide` | 各 49x17 | `BtTamingShow/Hide` | 各 47x17 |
| `BtExceptionShow` | 71x17 | `BtExceptionHide` | 12x12 |
| `BtPet1/2/3` | 各 30x27 | `Marry{married,unmarried}` | 20x17 |
| `ReqLv[31x10]`、`bossPetCrown[18x12 origin(9,6)]` | — | — | — |
- 代码现取 `BtAdd/BtClose`：`BtAdd` 本根**没有**（原生「加好友」= `BtRegister` 31x17）；
  本根也**没有** `BtClose` ⇒ 关闭钮仍须走共享件 `Basic.img/BtClose2`。

### 关闭钮素材分三类（`#51` 判据，2026-11-01 逐颗导出点阵实测）
| 类 | 件 | 实测 | 处置 |
| --- | --- | --- | --- |
| A 窗根无关闭钮 | `Item/Stat/Skill/Quest/Trunk/TradingRoom/UserInfo/…` | 节点不存在 | 必须用 `UiAssetResolve::close_btn()`→`Basic.img/BtClose2`（14x14 描边 X，不透明比 0.24）|
| B 实心亮块 | `Basic.img/BtClose`、`UIWindow.img/CashGachapon/BtClose`、`UIWindow.img/Family/BtClose` | 12x12，不透明比 **0.97**，三颗点阵同形（均值 BGR 168,135,65 / 167,133,63 / 164,131,66）| 本包没有 `BtClose3` ⇒ `close_btn()` 恒落到 `BtClose2`。这三颗**是原生本窗体真素材**，形状"实心"是原生样式，不做替换 |
| C 宽条钮 | `UtilDlgEx/BtClose` 85x19（与 `BtYes/BtNo` 同尺寸）、`NewYearsCard/BtClose` 48x22、`FamilyTree/BtClose` 11x11（暗底 X 镂空）| 不透明比 0.93 / 0.99 / 0.73 | **保真**：NPC 对话/贺年卡/家族树的关闭钮原生就是文字宽钮，`UINpcTalk` 在 draw 里 `set_position` 重排，勿改成共享 X |
| D 英文 CLOSE 圆顶钮 | `UIWindow.img/BtUIClose`、`BtUIClose2` | **32x15**，origin (0,0)，蓝底圆顶 + 白 X + 「CLOSE」字样 | 原生件（不是白块）。`UIMessenger` 用它是正解，但按 `dim.x()-21` 摆放会**越窗 11px**（宽 32）⇒ 待原生帧定坐标 |

