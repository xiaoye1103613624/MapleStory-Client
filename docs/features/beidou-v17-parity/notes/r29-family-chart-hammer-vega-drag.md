# r29 — Family chart 解析 + Hammer/Vega 拖放

## Family chart
- `FAMILY_CHART_RESULT`(94) 按 s9 `showPedigree` / `addPedigreeEntry` 解析
- `UIFamilyTree::set_chart` 列表展示成员名/等级/在线/rep；`add_enabled` 控制 BtJunior

## Hammer / Vega
- `Icon` 增加 inventory slot/id/tab 查询；`ItemIcon` 实现
- `USE_CASH_ITEM`(79)：
  - Hammer 557：`UseViciousHammerPacket(cashSlot, id, equipSlot)`
  - Vega 561：`UseVegaSpellPacket(cashSlot, id, equipSlot, useSlot)`
- UI 接受拖入装备（Vega 另收卷轴/现金符）后 BtStart 发包

## 部署
- OutDir → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
