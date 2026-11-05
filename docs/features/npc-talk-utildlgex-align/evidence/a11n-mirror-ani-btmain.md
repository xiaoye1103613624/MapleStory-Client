# A11n Mirror 动画底 + BtMain 图标 — IDA/IMG 取证与接线

> IDB：`BeiDou.exe.i64` ｜ IMG：`UIWindow.img/SlideMenu` ｜ 日期：2026-11-05  
> 承接 [a11l-askpet-mirror-ida.md](a11l-askpet-mirror-ida.md)

## 1. 动画底（`sub_76B9FC`）

| 项 | 证据 |
| --- | --- |
| 属性 | `SlideMenu/<param>/IsAniBackgrd`；本仓 IMG **0/1 均为 1** |
| 分支 | `==1` → `sub_9E0750`（动画底）；`==0` → `sub_9E0AB2`（静帧）；其它值早退 |
| 帧 | `backgrd/0..11`，每帧 **delay=120**；skin0 画布 284×217（origin 5,5）；skin1 235×165 |
| 窗尺寸 | ctor/`4EDAB7` 与箭头重建/`4EDA94` 均强制 **235×165** |

## 2. BtMain（纠正 A11l 预留表述）

| 项 | 证据 |
| --- | --- |
| 资源 | `BtMain/<id>`：38×38，含 `normal/pressed/disabled/mouseOver`（多数另有 `keyFocused`） |
| 节点 id | skin0：`0..24` + `88`/`99`；脚本 `9010022` 用 `0..3` 均有图 |
| 布局 | `Choice` 40×40 @ 绘制点 (90,50)；BtMain 居中内嵌 (91,51) |
| 交互 | IDA 点击分发 `76C562` 仅 **3000/3001/3002/3003/3005**；**无** BtMain 独立点击 → 图标非交互 |
| 前往钮 | **恒为** `BtMove` @ (128,134)；不得用 BtMain 替换（尺寸 38×38 ≠ 39×18） |

> A11l「选中后 BtMain 替 BtMove」为误判；本轮改为 Choice 内图标切换，BtMove 保持前往。

## 3. Cover

`Cover` 194×90 origin(-40,-53) 随皮肤绘制（叠在动画底之上）。F6 `UISlideMenu` 另用同目录 Cover，互不干扰。

## 4. AskPet 亲密/饱食原生位图条

全仓 `UIWindow.img` 扫描：`UtilDlgEx_Pet` **仅** `backgrnd`；无 pet gage/canvas。  
`UtilDlgEx/bar` 为对话皮装饰，非亲密条。**维持 ColorBox**（max 30000 / 100）。

## 5. 本仓接线

- `UIDimensionalMirror`：`Animation(backgrd)` + `update`；`Cover`；`rebuild_main_icon` 切 `BtMain/<id>/normal/0`
- 皮肤下标 = 入站 `mirror_param`（s9 `getDimensionalMirror` 写 0）
- 出站不变：`14 + action + int selection`
