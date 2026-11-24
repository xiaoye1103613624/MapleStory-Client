# Addon → 192 原生 Equip/Android 迁移与 BodyPart 冲突

> 2026-11-22｜裁决：自定义 `UIAddonEquip` **不是** 192 原生；长期改接 `UIWindow4.img/Equip` 多页签（含 Android）。

## 原生证据（REA / orange-wz）

| 项 | 证据 |
| --- | --- |
| 资源源 | 192 `Data/UI/UI_005.wz` → `UIWindow4.img/Equip`（IV=`AAAAAA==`，128B GMS userKey） |
| Android 叠层 | `Equip/Android/{backgrnd,Slots,BtShop}`；`backgrnd` **162×209** ARGB4444，**低 α 人影**（opaque=0，与 192 像素同构） |
| 页签色板 | `Equip/backgrnd2` **214×256**（侧栏须叠在此上；单独画 Android/backgrnd 会呈「只有轮廓」） |
| 槽位画布名 | `Equip/Android/Slots/1200`…`1206`（多为 1×1 原点锚点，非槽框美术） |
| Angel | `CAndroidTab::InitEquipItem` @`0x1202630`：循环 wire **−1200…−1206**；UOL `UI/UIWindow4.img/Equip/Android/Slots/%d`；`BtShop` |
| 同级页签 | `Equip/{Equip,Cash,Pet,Android,Dragon,Mechanic,Coordinate,Tab,totem,…}`；`Tab/enabled/0..7` |
| 辅助面板 | 192 **无** `AddonEquip` /「辅助」节点；仅有 `Equip/totem` 页签 ≠ 本端自绘 Addon |

## 本端现状（过渡）

| 组件 | 角色 | BodyPart |
| --- | --- | --- |
| `UIEquipInventory` | 角色装备主窗（classic `UIWindow.img/Equip`） | 常规 1–… |
| `UIAndroidEquip` | 侧栏机器人：`backgrnd2`+`Android/backgrnd` | **ANDROID1..7 = 1200..1206** |
| `UIAddonEquip` | **自绘**侧栏「辅助装备」（非 192；保留） | TOTEM 55–58 / EMBLEM 59 / HEART 61 / BADGE（机器人已迁走） |
| `UIMysticBadge` | 扩展神秘栏（可保留） | **70–75** `Mb` |

解析门：`UiAssetResolve::equip_panel_root()` 在检测到 `UIWindow4/Equip` 为 192 多页签壳时，主窗仍吃 classic；`equip_native_root()` 暴露原生壳给后续 Tab/Android 接线。

## BodyPart 冲突风险（硬）

```text
自定义 Addon ANDROID_SLOT = 60
原生 192 Android UI / Angel CAndroidTab = 1200–1206
```

- 若只把原生 Android **UI** 接上、仍发/收 BP60，则与官方槽语义错位，穿戴包会对不上。
- 若客户端改 1200–1206 而 s9 仍是 60，服务端会拒/错槽。
- 图腾：Addon 用 55–58；192 另有 `Equip/totem` 子壳——接线时按原生 Tab 语义复核，勿与主窗空格混用。
- 神秘 70–75 为扩展空洞，**不**在 192 Android 号段。
- 布局（2026-11-22 截图）：顶对齐横排 `[神秘][机器人][角色装备][辅助]`；辅助贴 `base_dimension` 右缘（宠物栏上方）。`UIElement::anchor_right_of_box`。

## 入库流水线（已执行）

```text
load UI_005 (零 IV) → create_img(同钥) → copy_paste Equip
→ save_as UIWindow4_zero.img → batch_convert_key(零IV→GMS, verify:true)
→ fingerprint ba82e943… / SHA256 77EDF3E6…
→ 原子替换 live BeiDou-Client-xy\Data\UI\UIWindow4.img
  bak: UIWindow4.img.bak_pre_equip192_20261122
  staging copy: docs/.../staging_uiwindow4/UIWindow4_Equip_192_GMS.img
```

禁止裸 `Copy-Item` 跨钥源包；本轮源为已 `batch_convert_key` 产物。

## 迁移阶段

1. **资源**：live `UIWindow4.img/Equip` 已含 Android/Tab/totem。
2. **布局**：顶对齐 `mystic | android | main | addon@base_dimension.x`（不再垂直叠左侧）。
3. **接线（本轮）**：
   - 客户端 `UIAndroidEquip` ← `backgrnd2` + `Android/backgrnd`；`EquipSlot::ANDROID1..7 = 1200..1206`
   - `islot=Dr` / 号段 166 → `ANDROID1`；`find_equipslot` 在 1200–1206 找空位
   - 旧 `ANDROID_SLOT=60` 仍可解析下行，**新穿戴不发 -60**
   - s9 `EquipSlot.ANDROID` 允许 `-1200..-1206`（+ cash `-1300..`）并兼容 `-60/-160`
   - Addon 去掉机器人格，仅图腾/纹章/心脏/徽章；**明确非 192 原生**
4. **验收**：开装备 → 机器人有色板+人影；四栏顶齐；穿卸走 1200 段；classic pet 仍可用。
5. **截图反馈（2026-11-22 20:28）**：live/192 `Android/backgrnd` 均非缺图，是页签叠层；侧栏补画 `backgrnd2` 后观感恢复，**无需**再从 192 `batch_convert_key` 覆盖（像素已同构）。

## 回归与 UI 答疑（2026-11-22 21:47）

### 1. 双击穿/卸失效（P0）

- **根因**：`UIStateGame::doubleclick` 在 `draggedicon` 非空时跳过转发；本端 `send_cursor` 于 **mousedown 即 `drag_icon`**，双击第二次按下先置拖拽 → 双击永远被门闩挡掉。MinMS **无**此门闩。
- **修复**：命中 UI 时若有拖拽则 `remove_icon()`（不 drop）再 `front->doubleclick`；仍必 `return` 防开个人情报。

### 2. 神秘徽章

- 绘制对齐 `assets/ui/mystic_badge/panel_base.png`（187×340，6 空槽，深蓝主区无解锁文案）。
- `lock.png` 仅可选资源；README 明确默认不画锁。无官方 Mb 解锁协议证据 → **不**实现锁提示。

### 3. Android 槽框

- live/192：`Slots/1200..1205` = **1×1 原点锚**；`1206` = **40×39** 真画布（含「手套」水印）。
- 客户端：仅绘制 `dim > 1×1` 的 `Slots/%d`；人影仍靠 `Android/backgrnd` 叠层。

### 4. 辅助装备

- **仍是自绘** `UIAddonEquip`（`UIWindow.img/AddonEquip`）；192 **无** AddonEquip，仅有 `Equip/totem` 页签（不同产品）。
- `SLOTS`：图腾×4 / 纹章 / 心脏 / 徽章；**(44,65) 机器人格不绑**。截图「口袋」为误读——实测末格水印为徽章、上排四格均为图腾。
- 位图仍烤有「机器人」水印：staging 已 blank 该格 → `UIWindow.img.AddonEquip_blank_robot.img`（live 占用时未原子替换；关客户端后覆盖 `UIWindow.img` 并清 UI.nx）。
