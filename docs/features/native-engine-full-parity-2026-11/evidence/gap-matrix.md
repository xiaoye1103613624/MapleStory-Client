# Wave0 缺口矩阵（2026-11-22）

对照：MinMS `E:\project\MinMS\src\client`、MapleStoryT IDA（高版本 Equip UI 串）、既有 Angel 083 归档、live `BeiDou-Client-xy`、s9 `EquipSlot.java`。

| ID | 项 | 证据 | 判定 | Wave |
| --- | --- | --- | --- | --- |
| G1 | 双击背包/装备开个人情报 | 命中 UI 必 return（对齐 MinMS） | **已修** | W1 |
| G2 | 背包/装备栏双击穿卸 | `draggedicon` 门闩 + mousedown 即拖拽 → 双击被吞；MinMS 无门闩 | **2026-11-22 21:47 已修**（cancel drag 再转发） | W1 |
| G3 | 快捷栏拖拽/道具键 | `native-baseline-audit` 已修；`Stage::send_key` 有 `ITEM` | 复验 | W1 |
| G4 | HP 下限 | s9 自损拒自杀；受击可到 0→`PlayerHandlers` die；无「clamp 到 1」权威 | **文档终裁，NO-FIX 玩法** | W1 |
| G5 | 龙咆哮 effect≈hit 观感 | WZ effect/hit 分节点；链完整；紫圈残留已修 | **观感 NO-FIX**（非错挂） | W1 |
| G6 | Addon（图腾/纹章/心脏/徽章） | 自绘过渡；**机器人已迁出** | 机器人 → G8 | 过渡 |
| G7 | 神秘徽章栏 | `UIMysticBadge` BP70–75；`anchor_left_of_below` | 扩展可留；勿盖主窗 | W2 |
| G8 | 192 Equip/Android | live 资源 + `UIAndroidEquip`（`backgrnd2`+叠层）+ `ANDROID1..7=1200..1206` + s9 | **侧栏色板已补**；Dragon/Mechanic/… Tab 仍待 | 迁移中 |
| G8b | 辅助装备 | 192 无 AddonEquip；保留自绘 `UIAddonEquip` | **确认自定义**，勿冒充 192 | 过渡 |
| G9 | POCKET(-33)/六戒(-52/-53) | 2026-11-23 D-RING/D-POCKET：六戒 52/53 + 口袋 BP33 出宠装表（−133 仍宠） | **已接** | layout-plan |
| G10 | AUX_WEAPON(-62) | 与 BOOK 占位/原武器号段冲突历史 | **NO-FIX 本轮** | W3 记 |

## 神秘徽章 BodyPart 定案

官方 192 无独立「神秘徽章」BodyPart 证据；本线扩展沿用 sidecar 空洞，取 **70–75**（`MYSTIC1..6`），避开宠物 21–48、Addon 54–61、非穿戴 BOOK 62–64。islot 约定 `Mb`；号段穿戴由 `EquipData`/`ItemInformationProvider` 在有道具时再绑（窗体先闭环）。
