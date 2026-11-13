# 第十面 地图(Map) — 既有文档四态审计

> 口径承第七/八/九面：**销案（既有结论被 083 一手证实/已实现）/ 推翻（既有依据来源被证伪）/ 新增（083 一手新发现）/ 遗留（真机或资源缺口仍 open）**。
> 素材：`docs/features/beidou-v17-parity/notes/r*-*map*.md` 等 53 项 + `docs/features/s9-protocol-align/notes/s8-npc.md` 交叉面 + REA r17 evidence + 第九面 `r05-npc-four-state-audit.md`。
> 性质：只读取证 + 归组审计；非实现提交。

## 1. 既有笔记索引（ops_glob 53 项，scanComplete=true）

地图/传送/小地图/世界地图/键位/MapleTV 相关既有笔记：
`r01-module-map.md`、`r134-changemap-pickup-softkey.md`、`r142-delivery-maptransfer.md`、`r169-mapletv-softkey.md`、`r173-worldmap-chat-help-softkey.md`、`r185-inventory-chalk-kite-mapfx-softkey.md`、`r207-worldmap-teleport-softkey.md`、`r208-mapsearch-btteleport.md`、`r211-minimap-event-tip-out.md`、`r217-mapsearch-back-hud-softkey.md`、`r236-maplepoints-label-softkey.md`、`r247-maplepoints-local-setting.md`、`r268-cashshop-buttons-maplepoints.md`、`r276-inventory-maplepoints-server.md`、`r282-maple-tv-send-remove.md`、`r286-maplelife-result.md`、`r287-maplelife-out-softkey.md`、`r310-guest-hour-minimap-trock-clear.md`、`r315-minimap-onoff-ui.md`、`r335-map-field-markers.md`、`r337-chat-guildbbs-portal.md`、`r359-keymap-tip.md`、`r388-mapobj-obstacle.md`、`r401-mapmist.md`、`r404-openui-portal-rock.md`、`r416-map-tip-helper.md`、`r417-map-tip-consolidate.md`、`r426-keymap-macro-combo-sm.md`、`r432-rps-ariant-memo-mapobj-tips.md`、`r456-changemap-sm.md`、`r492-map-tip-noise-cut.md`、`r497-cd-keymap-knock-silent.md`、`r502-engage-minimap-forcedstat-dedupe.md`、`r508-mapchange-cs-move-silent.md`、`r510-float-map-field-shop-tip-sm-only.md`、`r511-servermsg-map-book-silence.md`、`r537-weather-portal-rock-family.md`、`r540-cs-map-player-silence.md`、`r569-enter-map-skillbook.md`、`r615-blockedmap-cs-refresh.md`、`r621-portal-dragon-fx-set-label.md`、`r638-cs-keymap-weather-rem-drain.md`、`r654-family-portal-mts-viewall-rem.md`、`r668-session-witch-keymap.md`、`r704-quest-map-buddy-inv-silence.md`、`r71-mapletv-adminclaim-event.md`、`r72-map-effect-avatar-mega.md`、`r766-worldmap-minimap-selected.md`、`r776-teleport-softkey-charinfo-wmap.md`、`r791-shortcut-skill-map-useitem.md`、`r794-worldmap-family-upgrade-merchant.md`、`r804-mts-remotestore-worldmap-plant.md`、`docs/features/ui-v83-window-restore/notes/regress-20261107-mapletv.md`。

## 2. 四态归组

### 2.1 销案（既有结论被 083 一手证实 / 已实现）— 8 组

| 组 | 既有笔记 | 083 一手证据 | 当前端 | 结论 |
| --- | --- | --- | --- | --- |
| S1 MINIMAP_ON_OFF → UIMiniMap | `r315-minimap-onoff-ui.md`（on→emplace/activate UIMiniMap / off→deactivate） | `CWvsContext::OnMiniMapOnOff @ 0xa129ed` = `Decode1` 写 flag + `CUIMiniMap::ReloadMiniMap` | `IO/UITypes/UIMiniMap.h` `TOGGLED=true` | ✅ 销案：083 的「开关→重载小地图」与当前端一致 |
| S2 MapObjectVisible / Obstacle | `r388-mapobj-obstacle.md` | `CMapLoadable::OnPacket case 129` → `OnSetMapObjectVisible @ 0x6449d2`；`UpdateObstacleInfo @ 0x642264` | `Net/PacketSwitch.cpp:344 SET_MAP_OBJECT_VISIBLE=129` + handler | ✅ 销案 |
| S3 世界地图 SoftKey 传送 | `r207-worldmap-teleport-softkey.md`（纯数字 mapid → select_map_spot + ChangeMapPacket；T/Enter 传送）、`r766`、`r776`、`r208` | `CWorldMapDlg::OnKey @ 0x9ee717`（ESC 返回上一级 `SetWorldMapShallower`）+ `SetWorldMapDeeper/Shallower` | `IO/UITypes/UIWorldMap.h:123 request_teleport(int32_t map_id)` + `:63 send_key` + `select_map_spot` | ✅ 销案：083 有上下钻，当前端有分区跳转 |
| S4 ChangeMap 链路 | `r456-changemap-sm.md`（SetField change_map SM 显示 mapid+portal）、`r134`、`r508`、`r540` | `CField::LoadMap @ 0x529bb4` = `CMapLoadable::LoadMap` + 8 个 `Restore*` + `PlayBGMFromMapInfo` | `Net/Packets/GameplayPackets.h:25-40 ChangeMapPacket`（CHANGEMAP=38）+ `Gameplay/MapleMap/MapInfo.h` | ✅ 销案 |
| S5 Keymap 上行 | `r359-keymap-tip.md`、`r497`、`r426`、`r638` | `CFuncKeyMappedMan::SaveFuncKeyMap @ 0x58df2f` `COutPacket(135)` | `Net/OutPacket.h:169 CHANGE_KEYMAP=135` + `PlayerPackets.h:103 ChangeKeyMapPacket` | ✅ 销案（数值语义双对齐，body 微差记 D1） |
| S6 Quickslot 上行 | `SkillMacroPackets.h:34-49` 既有注释（8×int 触发键，走 CHANGE_QUICKSLOT=183；槽内绑定走 KEYMAP=135） | `CQuickslotKeyMappedMan::SaveQuickslotKeyMap @ 0x72b87e` `COutPacket(183)` + `EncodeBuffer(0x20)` | 同文件 `ChangeQuickslotPacket` 8×`write_int` | ✅ 销案（数值+body+语义三对齐） |
| S7 MapleTV 双屏（第九面交叉） | `r169-mapletv-softkey.md`、`r282`、`r71`、`regress-20261107-mapletv.md` | `CNpc::SetMapleTVMessage @ 0x6d417e` / `DrawMapleTVMessage @ 0x6d4ad4`（本面命中 2 函数）+ 第九面 `Npc.h` TV 双屏 | `Gameplay/MapleMap/Npc.cpp` MapleTV 上屏 240×90 / 下屏 240×180 | ✅ 销案 |
| S8 s9 协议对齐切片 | `docs/features/s9-protocol-align/notes/s8-npc.md`（portal 无独立 c2s、走 CHANGEMAP；QUEST_ACTION 107 restoreLostItem 低严重度不改） | 083 `CField::LoadMap` 无独立 portal 发包字面量 | `MapInteractPackets.h` CHANGE_MAP_SPECIAL=100 / USE_INNER_PORTAL=101 为 s9 通道 | ✅ 销案：083↔s9 命名空间分列，不混 |

### 2.2 推翻（既有依据来源被证伪）— 1 组

| 组 | 既有依据 | 证伪证据 | 处置 |
| --- | --- | --- | --- |
| O1 历史「orange-wz 未连接 / 阻塞 / 以 .nx dump 为准」口径（第七/八/九面 `r03` + D10/D13/D14，共 64 处） | 前九面 IMG 环直接写「MCP 未注册」跳过 | **round32 实证**：`custom-orange_wz` 已连接（global / 29 工具 / health_check 2/2），目录结构层 `Data/Map children=8` 真实节点 WORKS；仅 internal `.img` 内容层 `IMG 解析失败` → AGENTS.md（YG 2026-11-09 ACTIVE）写死「先 orange-wz，仅解析异常才回退 .nx」 | **推翻的是「未连接/绕过」口径，不是实现**：第十面起 IMG 环必须 `orange-wz MCP → 原始报错留证 → .nx 合规回退`。此前九面 README 的 r03 结论降级为「2026-11-09 前历史取证留存」，不再约束执行顺序。见 `r03-img-gate-fallback.md` |

> 与第九面 O1（095 线 `Maplestory95.i64 0x671590` 依据被 083 `0x6d3273` 推翻）同构：**推翻依据来源，不动已实现功能**。

### 2.3 新增（083 一手新发现）— 8 组

| 组 | 发现 | 证据 |
| --- | --- | --- |
| N1 `CMapLoadable` 36 方法资源管线全具名 | `Init/LoadMap/Close/MakeBack/MakeCloud/MakeObj/MakeTile/MakeObstacles/MakeVectorAnimate/Restore*/TransientLayer_*/OnSetBackEffect/OnSetMapObjectVisible/OnClearBackEffect/UpdateObstacleInfo/ProcessReflection` | `angel-map-facts.md` §1 |
| N2 `OnPacket` s2c 三 case 精确值 | `case 128/129/130` → `OnSetBackEffect / OnSetMapObjectVisible / OnClearBackEffect` @ `0x644446` | 逐字伪代码 r01 §3.3 |
| N3 `SaveFuncKeyMap` 89 键快照差分 | `do{ if(*v3!=v3[445]||*(v3+1)!=*(v3+446)) InsertBefore(v2); v3+=5; }while(v2<89)` → 仅变更键才发 135 | r01 §3.1 / D1 |
| N4 `SaveQuickslotKeyMap` 32B 原缓冲 | `EncodeBuffer(this+4, 0x20u)` + `memcpy(this+36, this+4, 0x20u)` 保存快照 | r01 §3.2 |
| N5 `CWorldMapDlg` 18 方法 / `CUIMiniMap` 10 方法全具名 | `CreateWorldMapDlg/SetWorldMap/LoadMapList/MakeBorder/CheckLinkInfo/CheckSpotInfo/SetWorldMapDeeper/Shallower` / `LoadMiniMap/SetCtrl/SetLayer/InsertStalkee/OnMouseMove` | `angel-map-symbol-inventory.json` |
| N6 `CItemInfo` 地图名查表链 | `FindMapByName @ 0x5cfaf3` / `GetMapString @ 0x5cf792` / `IterateMapString @ 0x5d0467` + `?$ZMap` 20 方法 | facts §1 |
| N7 `133/133 by bytes` 十面同二进制 | 2 个 missing EA（`0x9e98c2 ::_GCWorldMapDlg` / `0x58e12b CFuncKeyMappedMan::FuncKeyMapped`）32B 双端逐字一致 | `angel-beidou-map-ea-bytelevel-audit.json` |
| N8 orange-wz 结构层真实节点 + 合规回退链 | `Data/Map children=8` + 4 条 P1–P4 probe + `raw_error_texts_verbatim` + `no_fake_green` 自我批判 | `evidence/orange-wz-gate-verdict.json` / `r03` |

### 2.4 遗留（真机 / 资源缺口仍 open）— 5 组

| 组 | 缺口 | 归属 |
| --- | --- | --- |
| L1 F 维 12 项逐像素贴图对齐 | orange-wz 内容层 `IMG 解析失败`，须等服务端恢复或换可解析 `.wz` 原包 | P2 + `r03` §6 |
| L2 `SetWorldMapDeeper/Shallower` 上下钻真机观感 | 083 伪代码具名但真机分区层级动画缺双开比对 | P2 |
| L3 `TransientLayer_{FireCracker,NewYear,Weather}` / `ProcessReflection` 特效层实机 | 083 具名（`@ 0x6419e7 / 0x641b13 / 0x63fe57 / 0x643f7a`）但当前端无对应实现录屏 | P2 |
| L4 `OnMapTransferResult` 083 多分支（case 2..11 + Decode4 列表）vs 当前端 2 字段日志 | 不强行对齐，记 D6；真机多分支回包 hex 待采 | P2 / D6 |
| L5 地图名 `String.nx` / `SP_267` 提示串逐字比对 | `.nx` 内容层可读但 083 `SP_` 与当前端 StringPool 未逐条对账 | P3 ⑪任务面联动 |

## 3. 交叉面纪律（承第八/九面）

- **数值重合 ≠ 语义重合**：`135` 在 c2s = `CHANGE_KEYMAP`，在 `PacketSwitch.cpp:218` s2c = `WHISPER`；`183` 在 c2s = `CHANGE_QUICKSLOT`，在 `:262` s2c = `REMOVE_DRAGON`。禁止仅凭数值判等。
- **083 / s9 两套命名空间**：083 `COutPacket(135/183)` 与当前端同名 opcode 数值对齐属**正面闭环**；`CHANGEMAP=38 / CHANGE_MAP_SPECIAL=100 / USE_INNER_PORTAL=101 / PLAYER_MAP_TRANSFER=207 / WORLD_MAP_PLAYERS=376 / WORLD_MAP_DROP_RESULT=377` 属 s9 通道，分列不混。
- **校准 ≠ 改回原生**：083 `CWorldMapDlg`/`CUIMiniMap` 是 MFC `CDialog` 系；当前端 `UIWorldMap`/`UIMiniMap` 是 `UIDragElement` + 多分辨率重锚 + 侧栏状态机。功能覆盖，实现不同源。

## 4. REA subject 单列（不冒充 13339 一手）

`docs/features/beidou-v17-parity/notes/evidence/r17/rea_search_*.json` 的 subject 是 **`BeiDou-ClientV17.7/BeiDou.exe`**，sha256 `1198fa57ca5a7c489bae43ec13c69681d9cabe0f96762f3dc0357facf2e7d4df`，base `0x400000`，size `0xa94000`，md5 `44c1030ecd8484e849c83892eb8fa025`；`rea_search_SkillEx/SkillMacro/SkillUp` 等 `n:0` 全空反证 stripped 端符号层几乎全空。**这些是当前端既有 REA 素材，不是 Angel(13339) IDB 一手事实**，仅用于四态归组，不作 083 权威源。

## 5. 既有 095 线处置

- `docs/features/port-095/evidence/opcode-v95/dec/**` 与 `Maplestory95.i64` 相关 EA / 字段序 = 095 线，**083 唯一基准禁引**，仅作历史实现注记（如第九面 `npc-action-move-pending.md`）。
- `docs/features/change-quickslot-persist/notes/npc-action-move-pending.md` 的 v95 `op 241` 与 s9 `197` 对照已在第九面 O1 推翻换源，本面不重复计入。

## 6. 性质标注

本文件 = **校准 / 对照 / 四态审计 / 决策点素材**，非实现提交；全程只读未改游戏源码 / 协议 / WZ / IMG / XML，未建提交。D1–D14 须用户裁决后 P1 方可动源码（见主 README §6）。
