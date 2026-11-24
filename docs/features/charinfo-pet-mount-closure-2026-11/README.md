# 个人情报 / 宠物装备 / 坐骑最小预览闭环（2026-11）

## 决策
- 范围 D：UICharInfo 全子窗 + 装备栏宠物装备穿戴 + AskPet 条 + 坐骑最小预览
- 对照 Angel / GMS 083；**本轮未改 s9**（基础能力走既有协议）
- 验收：`tools/build_smoke.bat` + 用户实机

## 实现摘要

### P1 UICharInfo
- 宠物空态统一 `"-"`；亲密/饱清去掉英文前缀
- 坐骑子窗：有 id 时画道具图标；数值坐标微调
- `medal_stub` 初值改为 `"-"`

### P2 装备栏宠物装备
- 新增 [`Character/Look/PetBodyPart.h`](../../Character/Look/PetBodyPart.h)：Angel 页区间 21–29+46 / 31–37+47 / 39–45+48 + D7 格网
- [`UIEquipInventory`](../../IO/UITypes/UIEquipInventory.cpp)：三页图标加载、拖放穿/卸、`EquipPetItemPacket` / `UnequipPetItemPacket`
- 现金镜像位 `bp+100` 读取兼容

### P3 AskPet
- ColorBox 条加 1px 黑描边、间距微调；仍无几何条（UtilDlgEx 无 gage canvas）

### P4 坐骑
- [`TamingMobLook`](../../Character/Look/TamingMobLook.h)：从 `TamingMob.wz` 加载 stand/walk
- [`Player`](../../Character/Player.cpp)：`MONSTER_RIDING` + `TAMEDMOB` 时脚下预览（非完整 CActionMan）

### 道具子窗装备悬停 tip（续）
- 根因：① `UIStateGame` 每帧先清 tip，CharInfo 却只在槽位变化时 `show_*` → 一闪即没；② `show_item` 一律走 `ItemTooltip`，装备看不到属性/对比
- [`EquipTooltip::set_equip_itemid`](../../IO/Components/EquipTooltip.cpp)：仅有 itemid 时用 `EquipData` 默认属性拼装
- [`UIStateGame::show_item`](../../IO/UIStateGame.cpp)：装备类 itemid 改挂 `EquipTooltip`
- [`UICharInfo`](../../IO/UITypes/UICharInfo.cpp)：子窗命中区按 `active_panel`；悬停每帧重挂 tip

## 真机清单（用户）
1. 情报主窗字段 / 人气箭头
2. 六子窗互斥（有数据号 + 空态）
3. 装备窗宠物面板：三页切换、拖放穿/卸
4. NPC AskPet 亲密/饱食条
5. 装备坐骑 + 骑兽技能 → 脚下帧（缺 WZ 帧则跳过）
6. 情报「道具」子窗悬浮装备 → 完整装备 tip（可对比本机同部位）

## 改动文件
- `Character/Look/PetBodyPart.h`（新）
- `Character/Look/TamingMobLook.h/.cpp`（新）
- `IO/UITypes/UIEquipInventory.h/.cpp`
- `IO/UITypes/UICharInfo.cpp`
- `IO/UITypes/UIMiscShells.cpp`
- `IO/Components/EquipTooltip.h/.cpp`
- `IO/UIStateGame.cpp`
- `Data/EquipData.h/.cpp`
- `Net/Packets/InventoryPackets.h`
- `Character/Player.h/.cpp`
- `MapleStory.vcxproj` / `.filters`
