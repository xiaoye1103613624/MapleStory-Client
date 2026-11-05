# A11m AskPet dlgType=6 完整 UI — IDA/IMG 取证

> IDB：`BeiDou.exe.i64` ｜ IMG：`UIWindow.img` ｜ 日期：2026-11-05  
> 承接 [a11l-askpet-mirror-ida.md](a11l-askpet-mirror-ida.md)

## 1. 布局（`sub_9A9B25`，dlgType=6）

| 项 | 证据 |
| --- | --- |
| 窗宽 | `sub_9A3CAE` dlgType5/6 → **419** |
| 窗高 | `sub_9A3CED` dlgType6 → **297** |
| 底图 | `UIWindow.img/UtilDlgEx_Pet/backgrnd` **419×296**（与 297 对齐） |
| BtPrev | StringPool 2268 `UtilDlgEx_Avatar/BtPrev` @ **(15,191)** id=0x2000 |
| BtNext | 2269 @ **(341,191)** id=0x2001 |
| Yes | 1257 `Basic.img/BtOK2` @ **(314,272)** id=1 |
| No | 1258 `Basic.img/BtCancel2` @ **(363,272)** id=2 |
| Exit | 2724 `UtilDlgEx_Avatar/BtExit` @ **(9,272)** id=2 |
| nameTag | 2270 `UtilDlgEx_Avatar/nameTag` 173×19；字号 12、色 `#000000`（-16777216） |
| 正文区 | `sub_9ACC20` a2≠0 → left=**110**，area_h=**100**（`sub_9A3D71` case6） |

## 2. 数据与交互

| 项 | 证据 |
| --- | --- |
| 装填 | `sub_9ACB27`：icon / name / level(`47465D`) / closeness(`4746DD`) / expired(`4E4003`) |
| 选中 | `this+80`；Prev/Next 环绕后 `sub_9AA9F0` |
| 立绘 | `9AA9F0`：未过期 stand 脚点 **(209,218)**；过期 icon **(226,218)** |
| 出站 | 不变：OK=`lastMsg+1+int64`；Cancel=`action=0` |

## 3. 本仓接线

- `UIAskPet`：Pet 底图 + Avatar 同族钮 + stand0 动画 + nameTag + Lv/亲密/饱食条
- `ItemParser::add_pet`：现金宠读入 64 位 uniqueId；`Inventory::find_pet_by_unique_id`
- 亲密/饱食条：UtilDlgEx **无**专用 gage 节点（`is/ic/it` 为 529 宽对话皮），条用 ColorBox 对齐 nameTag 宽 173；亲密 max=30000、饱食 max=100

## 4. 仍预留

| 项 | 说明 |
| --- | --- |
| 亲密/饱食原生位图条 | 无 UtilDlgEx 专用 canvas；ColorBox 几何条 |
| Mirror 动画底 / BtMain 全态 | 见 a11l |
| 未入库 uniqueId 的旧号 | 仅召唤宠可显示名字；等级/条需现金栏命中 |
