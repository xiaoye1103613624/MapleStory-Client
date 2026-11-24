# 世界地图 tip 详细信息增强 · 交付记录（2026-11-20）

## 用户诉求与拍板
截图 `XiaoYe_2026-11-20_14-24-25.png`（用户当面确认为本端）显示 tip 只有基础信息，要求四项全补：怪物卡片图标 / 怪物数量统计 / NPC 名字 / 在线玩家列表。图标源拍板用怪物手册现成占位图标。

## 证据链（点到面，逐行号）
### 本端早已有（未重造）
- 地图名 `UIWorldMap.cpp:686` → `MapTooltip.cpp:85-107`
- 怪物名+Lv `Misc.cpp:271-281` → `MapTooltip.cpp:439-445`
- NPC 名 `Misc.cpp:282-287` → `MapTooltip.cpp:446-456`
- 在线玩家（名字+Lv+频道）`UIWorldMap.cpp:683` → `MapTooltip.cpp:465-489`
- 收发包 s9 `WORLD_MAP_PLAYERS(0x115/0x178)`：`PacketProcessor.java:321`、`WorldMapHandlers.cpp:40`

### 本次缺口与修复
| 缺口 | 参考包依据 | 本端修复 |
|---|---|---|
| 怪物数量头 | `worldmapinfo.cpp:923-925 BuildCountHeader` | `MapTooltip.cpp set_worldmap_mapid` 按 m/n 计数填 `Monsters (N):`/`NPCs (N):` |
| 图标空纹理 | 原 `UIWindow.img/ToolTip/WorldMap/Mob|Npc` 节点不存在（构造函数旧注释） | 改用 `String/MonsterBook.img/icon/0..1`（22x27，UIMonsterBook.cpp:156 同款） |
| i18n | — | `locale/en-US.ini`/`zh-CN.ini` 加 `worldmap.monsters.count`/`worldmap.npcs.count` |
| 换图残留 | — | `reset()` 清两个数量头 |

## 改动物件
- `IO/Components/MapTooltip.h`：+ `mob_count_header`/`npc_count_header`
- `IO/Components/MapTooltip.cpp`：构造函数图标源、set_worldmap_mapid 计数填头、draw_worldmap 画头、reset 清理
- `locale/en-US.ini` / `locale/zh-CN.ini`：+2 词条
- `docs/features/worldmap-tip-details-2026-11/PLAN.md`：证据与方案

## 构建验证（三证齐备）
`BUILD_EXITCODE=0`；链接行 `MapleStory.vcxproj -> build-wmtip/MapleStory.exe`；error=0；产物 `build-wmtip/MapleStory.exe` 4964352B / 15:07。

## 部署状态（2026-11-20 15:17 实时核对，USER.md 红线）
**本次改动已部署到 live**——但部署动作不是我手动执行的，是并行流程在 15:16:18 完成的。实时证据：
- 用户 15:16 授权"可以"部署；部署前核对发现 live exe 已在 **15:16:18** 被换成 SHA `0B5663CC25A88BE14D25848D39BF01B8982CB37FAA51AC922CD3D53483C2F998`（4973056B），不再是 14:25 截图版 `0EC62B43…`。
- strings 双重核对：live 与 `build-wmtip` 产物对 `worldmap.monsters.count`/`worldmap.npcs.count`/`iconRaw`/`MonsterBook.img` **全部命中且计数一致**；15:07 后无新构建日志 → live 这份就是含本次 tip 改动的版本。
- 因此**未覆盖 live**（盲目覆盖一个已含改动、且大小更新 8704B 的版本属于倒退风险）。产物仍在 `build-wmtip/` 可作回滚源。

**真机验收**：用户按当前 live 直接测（无需再部署）——世界地图悬停地图点，tip 应出现 `怪物 (N):`/`NPCs (N):` 数量头 + 卡片图标 + 玩家段。截图须本端。

## 真机验收（待用户）
替换 exe → 重启 → 世界地图悬停地图点：tip 应显示 `Monsters (N):`/`NPCs (N):` 数量头 + 怪物/NPC 卡片图标 + 在线玩家段。截图须本端。

## 回滚
替换前备份 live exe；产物 `build-wmtip/MapleStory.exe` 可随时回退。
