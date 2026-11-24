# P0 取证摘要

## Angel 负槽（既有 facts）
- `CPet::UpdatePetAbility`：页0 `21–29|46`，页1 `31–37|47`，页2 `39–45|48`
- `CUIPetEquip::OnMouseMove`：`GetItem(a3:1, a4:-v5)` / `a4:-100-v6`（现金镜像）

## IMG（orange-wz，BeiDou-Client_S9 UIWindow.img）
- `UserInfo`：backgrnd..backgrnd7 + BtPetShow/BtTamingShow/BtWish/BtExceptionShow 等齐全
- `Equip/pet`：canvas + origin only，**无 Slots 子节点** → UI 格网用 D7 像素表

## Inventory
- `ItemParser`：负 slot → EQUIPPED + 取反绝对值；`slotmaxima[EQUIPPED]=LENGTH(65)` 可容纳 21–48
- `EquipSlot::from_wire_bodypart` 对 21–48 返回 NONE（刻意）→ 宠物装走 `PetBodyPart` 旁路

## AskPet gage
- a11m：UtilDlgEx 无专用 gage canvas → ColorBox 合法降级

## IDA 本会话
- MCP 当时加载 `MapleStoryT.i64`（无 CUIPetEquip）→ 负槽表以既有 Angel facts 为准
