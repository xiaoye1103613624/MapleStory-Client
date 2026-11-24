# 套装双端权威闭环 + 装备成长 Companion Tip（1A / 2A）

> 状态：已实现（待真机悬停验收）  
> 日期：2026-11-20  
> 决策：套装 1A + 成长 2A；源码内建，不走 ijl15 Detour

## 1. 目标

以 083 原生 `SetItemInfo` / 主 tip 成长两行为底座，把 xy 双端扩展收敛为：

- 套装：服务端权威战斗 + companion tip（件列表/档位）+ `0x175/176/177` + 技能(+N) `0x17A`
- 成长：companion tip + 本地 WZ `info/level` + `0x17B/17D`
- 停靠链：`[悬停][套装?][成长?][对比]`

## 2. 权威规则

| 场景 | 权威 |
| --- | --- |
| 战斗 %（fdR / ignore / damR 面板） | `0x175` → `CombatPanelStats`（`synced`） |
| 本地 `DAMAGE/BOSS/IGNOREDEF` 注入 | 仅 `!synced` 兜底 |
| 套装 tip 文案 | 有 `0x176` 用服务端；否则本地 WZ 构图 |
| 成长 tip | 本地 WZ 先画；最多一次 `0x17D`  enrichment |

## 3. Opcode

见 [evidence/opcode-wire-matrix.md](evidence/opcode-wire-matrix.md)。  
**注意**：`SET_SKILL_BONUS` 从 `0x179` 迁到 `0x17A`（避 `WORLD_MAP_DROP_RESULT`），须双端同发。

## 4. 关键改动

### 客户端

- `Data/SetItemData.*`：WZ 键补 bdR/ignore*；`0x177` pending
- `IO/Components/SetTooltip.*`：件列表 + 部位排序/分色 + 档位
- `Data/EquipGrowthData.*` + `IO/Components/GrowthTooltip.*`：成长 companion
- `IO/Components/EquipTooltip.*`：停靠链
- `Net/OutPacket.h` / `PacketSwitch.cpp` / `CommonHandlers.*` / `SetItemPackets.h`
- `Character/Player.cpp`：synced 防双计
- `Character/SkillBonusCache.*`：`0x17A`
- `SetFieldHandlers`：进图清缓存

### 服务端（`E:\project\BeiDou-Server`）

- `SendOpcode` / `RecvOpcode`：`0x17A/17B/17D`
- `PacketCreator`：0x176 带 itemIds/tiers；`equipGrowthTip`
- `EquipGrowthTipManager` + `EquipGrowthTipRequestHandler`
- `ItemInformationProvider.getEquipLevelInfo` 公开

## 5. 构建证据

| 端 | 结果 |
| --- | --- |
| 客户端 | `setitem_growth_out\MapleStory.exe`（Release x64，MSBuild exit 0） |
| 服务端 | `mvn compile` gms-server exit 0 |

## 6. 手测清单（须用户本端截图）

1. 穿/脱套装：属性与伤害随服务端变化；悬停有件列表+档位；`0x177` 仅缺文案时发  
2. 世界地图掉落 `0x179` 与技能(+N) `0x17A` 不串包  
3. 可成长装备：主 tip 两行 + 右侧成长 companion；对比不重叠  
4. 非套装/无成长：无残留 companion  

## 7. 非目标

ijl15 hook、095 逐像素、套装 Web 管理台、DB DDL、主 tip 成长两行语义改动
