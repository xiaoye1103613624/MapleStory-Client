# 世界地图 tip 详细信息增强 · Phase0 证据与方案（2026-11-20）

## 用户诉求
截图（本端，用户当面确认）显示 `金银岛:东岩石山II / 野猪(1) 25只`，质疑"详细信息没做"。用户拍板四项全要：怪物卡片图标 / 怪物数量统计 / NPC 名字 / 在线玩家列表。

## 权威证据（逐行号，点到面）
### 本端已实现（不要重造）
| 段 | 位置 | 现状 |
|---|---|---|
| 地图名 | `UIWorldMap.cpp:686` → `MapTooltip.cpp:85-107 set_title` | ✅ 有 |
| 怪物名 | `Misc.cpp:271-281 get_life_on_map` 产 `野猪(Lv. X)` → `MapTooltip.cpp:439-445` | ✅ 有名字+Lv，❌ 无数量头、无图标 |
| NPC 名 | `Misc.cpp:282-287` → `MapTooltip.cpp:446-456` | ✅ 有名字，❌ 无图标/数量头 |
| 在线玩家 | `UIWorldMap.cpp:683` → `MapTooltip.cpp:465-489 set_players` 产 `[Lv.X] 名 (Ch.Y)` | ✅ 有 |
| 玩家收发包 | s9 `WORLD_MAP_PLAYERS(0x115/0x178)` handler 已注册（`PacketProcessor.java:321`），客户端 `WorldMapHandlers.cpp:40` | ✅ 有 |

### 缺失（本次要补）
| 项 | 参考包依据 | 本端落点 |
|---|---|---|
| **数量统计头** | `worldmapinfo.cpp:923-925 BuildCountHeader` → `Monstros (N):` / `NPCs (N):`，`:938/:947` 在怪物/NPC 段前插 header | `MapTooltip.cpp:435-457` 循环前按 m/n 计数插一行 header |
| **怪物/NPC 卡片图标** | `:40 kEntityIconTargetSize=20`、`:941/:950 entry.id` 带 id 供图标、`getwzinfo.cpp` g_cardIcons 从 `Mob.img/card` 取 | `MapTooltip.h:51 LIFE_ICON_ADJ` 已有位置常量；需新增按 mob/npc id 取 canvas 的 Texture 数组 |
| NPC 名字 | 本端已有（`Misc.cpp:286`），**保留** | — |
| 在线玩家 | 本端已有（`MapTooltip.cpp:465-489`），**保留** | — |

### 数据口径说明
本端 `Misc.cpp:257-292 get_life_on_map` 返回 `unordered_map<int64_t, pair<type, "name(Lv.X)"/"name">>` —— key 是 life id，可拿来取卡片图标；type "m"/"n" 区分怪/NPC。数量 = 同 type 的条目数（即地图上该种类数量，对应参考包 count）。

## 方案（一次成型）
1. `MapTooltip.h`：新增 `Texture mob_icons[MAX_LIFE]`/`npc_icons[MAX_LIFE]` + `Text mob_count_header`/`npc_count_header`。
2. `MapTooltip.cpp set_worldmap_mapid`：循环按 m/n 分别计数 → 填 `Monstros (m):`/`NPCs (n):` 头（用 Locale 词条，国际化标准）；循环内按 `life_id` 从 `nl::nx::String/Mob.img/{id}/card` 或 `nl::nx::Mob/{id}.img/info/card` 取 canvas 填 icon（缺失留空、不崩）。
3. `draw_worldmap`：header 行 + 图标行（在 LIFE_ICON_ADJ 位置画）随 m/n 索引绘制，同步 fillheight。
4. i18n：新增 `worldmap.monsters.header` / `worldmap.npcs.header` 等词条（locale 文件），面向人文案不硬编码。
5. 构建验证 + 真机待用户。

## 风险
- 卡片 canvas 节点路径需 orange-wz 实测（Mob.img/{id}/card vs Character 卡册），实现期先 probe。
- live exe 14:25:13 被并行会话换成 SHA `0EC62B43…`，真机截图对应的 exe 版本需在实现后重新核对（USER.md 红线）。

## 图标路径探测（2026-11-20，orange-wz 实测，未找到可用 canvas）
按参考包路径逐一实测，均未在本端 live 找到卡片 canvas：
- `Mob/0100100.img/info/card`：❌ 无（info 只有 level/HP/MP 等数值属性）
- `String/Mob.img/0100100`（含补零 7 位）：❌ 子节点空
- `String/MonsterBook.img/100100`：⚠️ 有 map/reward/episode 文本，**无 canvas**（reward[0]=1002067，非 238xxxx）
- `Item/Consume.img/0238`：❌ 子节点 0（本端卡片资源不在参考包的 v83 原版路径）

参考包 `GetCardIconByCardId:245` 用 `Item/Consume.img/0238/{cardId:08d}/info/iconRaw`（巴西服 v83 私服 WZ 结构），本端 BeiDou GMS083 的 Item 结构不同——**图标源在本端没有确证路径**，无法照搬。

## 待用户抉择（图标项的实质分叉）
1. **卡片图标用替代源**：本端怪物手册 `String/MonsterBook.img/icon/0..7`（UIMonsterBook.cpp:156 现成占位图标，22x27）——非每怪专属，但一定有图、不崩；
2. **用 ItemData 道具图标代替**：按 mobId 关联掉落道具图标（语义偏，但资源确凿）；
3. **本轮不做图标**，只补数量统计头（用户四项中唯一能确证落地的）——图标列为需先确认资源路径的后续项。

其余三项：怪物数量头（可落地）、NPC 名字（已实现，保留）、在线玩家（已实现，保留）。

## 本轮动作
Phase0 证据已落。图标源需用户拍板；数量头可直接实现。
