# W1 — 宠物族原生还原（c2s）

## 对账（s9 RecvOpcode）

| 包 | opcode | Handler | 消费点 |
| --- | --- | --- | --- |
| SPAWN_PET | 0x62 / 98 | SpawnPetHandler | 背包 CASH 500* 双击 |
| PET_FOOD | 0x4C / 76 | PetFoodHandler | USE 212* → UseItemPacket 路由 |
| USE_MOUNT_FOOD | 0x4D / 77 | UseMountFoodHandler | USE 226* → UseItemPacket 路由 |
| MOVE_PET | 0xA7 / 167 | MovePetHandler | Player::update 跟随位移 |
| PET_CHAT | 0xA8 / 168 | PetChatHandler | UIUtilDlgExPet SoftKey `T` |
| PET_COMMAND | 0xA9 / 169 | PetCommandHandler | UIUtilDlgExPet SoftKey `C` |
| PET_LOOT | 0xAA / 170 | PetLootHandler | Stage::check_drops（有宠优先） |
| PET_AUTO_POT | 0xAB / 171 | PetAutoPotHandler | SoftKey `P` slot,itemId |
| PET_EXCLUDE_ITEMS | 0xAC / 172 | PetExcludeItemsHandler | SoftKey `E` 逗号 id 列表 |

## 文件
- `Net/Packets/PetPackets.h`
- `Net/OutPacket.h` 枚举
- `Character/Look/PetLook.*` uniqueid/position 暴露
- `Character/Player.cpp` MOVE_PET
- `IO/UITypes/UIItemInventory.cpp` / `UIMiscShells.cpp`
- `Gameplay/Stage.cpp` PET_LOOT

## 验收
召唤可见且服务端在场；喂食 212*；指令/对话 SoftKey；靠近掉落可拾。
