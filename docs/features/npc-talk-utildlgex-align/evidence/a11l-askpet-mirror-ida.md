# A11l AskPet(9/10) + Mirror(14) UI — IDA/IMG 取证与接线

> IDB：`BeiDou.exe.i64` ｜ IMG：`UIWindow.img` ｜ 日期：2026-11-05  
> 承接 [a11j-askpet-mirror-pending.md](a11j-askpet-mirror-pending.md)

## 1. AskPet / AskPetAll

| 项 | 证据 |
| --- | --- |
| 入站 AskPet `7474A2` | `string` + `byte n` + **n×(long uniqueId + byte pad)**（`432257` 读 8 字节后 `4065F3` 再读 1 字节丢弃） |
| 入站 AskPetAll `74775C` | 同上，且在 n 后多 `byte flag` |
| 本机反查 | `46D301(invType=5, uniqueId)`；UI `9AC552(..., dlgType=6)` + `9ACB27` |
| 出站 OK | `lastMsg=9\|10` + `action=1` + **8 字节 uniqueId**（`46C00C` from item+24） |
| 出站 Cancel | `action=0` |
| 空列表 | AskPet `n<=0` → `action=2`；AskPetAll `n<=(flag==0)` → `action=2` |
| 空正文 | `action=1` + 8×0 |

### 本仓接线

- Handler：按 long+byte 解析；早退/空正文直接出站；否则 `UIAskPet::open`
- UI：UtilDlgEx 壳 + 列表选宠（宋体 A12M / DARKGREY）；完整立绘/亲密条 **预留**
- 出站：`NpcTalkMorePacket(AskPet{}, lastMsg, action, uid)`
- s9：`NPCMoreTalkHandler` lastMsg 9/10 读 `long`（selection 取低 32 位）

## 2. Mirror（DimensionalMirror）

| 项 | 证据 |
| --- | --- |
| 入站 `747A46` → ctor `76B3F0` / fill `76B5C8` | `int` + `string`；窗 **235×165** |
| 解析 `76B6D2` | `#id#label#` 循环（s9 `9010022.js` 同形） |
| 布局 `76BE7A` | 左箭头 `(11,67)` 右 `(200,67)` 取消 `(69,134)` 前往 `(128,134)`；文案 prev/cur/next y=58 |
| 资源 | `UIWindow.img/SlideMenu/0`：`backgrd/N`、`BtArrow/left\|right`、`BtCancle`、`BtMove`、`BtMain/<id>`、`Choice` |
| 出站 `747A46` | OK：`lastMsg=14` + `action=1` + `int selection`；Cancel：`action=0` |

### 本仓接线

- Handler case 14 → `UIDimensionalMirror::open`
- 出站：`NpcTalkMorePacket(DimensionalMirror{}, action, selection)`
- s9 既有 `getDimensionalMirror` / `NPCMoreTalkHandler` int selection 已对齐

## 3. 显式预留

| 项 | 说明 |
| --- | --- |
| AskPet 完整 dlgType=6 | 宠图标/等级/亲密/饱食条（`9ACB27`）未接；标签用召唤宠 itemId 或 `Pet [uid]` |
| 现金栏 64 位 uniqueId 入库 | Inventory 仍用本地 running_uid；出站用包内 uniqueId |
| Mirror 动画底图 | `IsAniBackgrd` / `backgrd` 多帧未播；固定 `backgrd/0` |
| Mirror BtMain 全态切换 | 选中后尝试 `BtMain/<id>`，缺则保留 `BtMove` |
