# #15 装备悬停已装备对比 · Phase0 研判（2026-11-18）

## 结论：已原生闭环，无需移植参考包（本轮不改代码）

参考包 `部分没截全的代码.txt`（311 行）是 Kaentake Detours 注入实现，且**残缺**——所有关键硬编码地址/常量（kAddrContextInstance、kAddrCharacterDataGetItem、kAddrGetBodyPartFromItem、kAddrCUIToolTipCtor、DecodeItemId、kCompareTooltipGap 等）均缺失，按门禁 5 禁止臆造。

但当前 `MapleStory-Client` **早已有更完整的原生实现**，`EquipTooltip` 组件即该功能目标形态。移植 = 重复造轮子 + 破坏已验功能，违反"最小改动"红线。

## 一层层证据（逐行号）

### 1. 功能主干已实现（EquipTooltip.cpp:321-390）
- `:375` `if (show_compare && invtype == EQUIP && oequip)` —— 仅背包装备触发对比
- `:377-379` 悬停 itemid → `EquipData::get(item_id).get_eqslot()` 得部位（比参考包硬编码 getBodyPart 更稳）
- `:381` `inventory.has_equipped(eqslot)` → `:383` `get_equip(EQUIPPED, eqslot)` 取已穿戴 → 构建对比窗
- 与参考包 `FindEquippedCompareItem`（txt:188-215）语义一致且无硬编码地址。

### 2. 对比渲染比参考包更强（EquipTooltip.h）
- `:115` `show_compare` 开关；`:104-105` `invpos`/`invpos_preview` 主窗/对比槽
- `:187` `statdeltas_preview` 每条属性差值（悬停−已装备，正绿负红）—— 参考包只是并排显示，无差值
- `:62` `PREVIEW_OFFSET = FRAME_WIDTH+12+6` 两窗留 6px 场景缝隙（修复过交叉重叠）
- `:157` `starforce_preview` 对比窗星力、`:198` 套装三段布局 `[悬停][套装][已穿比较]`

### 3. 配置开关默认开启（Configuration.h:563-567）
`EquipCompare : BoolEntry("ui.equip_compare", "true")`，`EquipTooltip.cpp:297` 读取。用户默认就能看到对比。

### 4. 悬停入口全覆盖
- `UIItemInventory.cpp:1569-1577` 背包 EQUIP tab 走 `show_equip`
- `UIAddonEquip.cpp:258-260`、`:183` 附加装备槽也接 show_equip
- `UIStateGame.cpp:1023` show_equip → `eqtooltip.set_equip(parent, slot)`（:923 同帧重建无闪烁）

### 5. 已有真机校准文档
`docs/features/ui-v83-window-restore/evidence/w5-tooltip/equip-native-compare.md` —— 说明本功能经历过真机对比校准。

## 待用户确认（唯一有价值的问题）
- 若用户实际使用时**没看到对比**：优先查 `ui.equip_compare` 是否被设为 false，而非重新移植。可让我核对配置 + 真机截图定位（截图须确认是本端，不能用参考端）。
- 若用户想要的是**更老的对比样式**（参考包并排无差值），那是观感变更，需另立需求，不应覆盖现有增强版。

## 本轮动作
无代码改动。本任务标记"已闭环（原生实现）"，不进入实现阶段。
