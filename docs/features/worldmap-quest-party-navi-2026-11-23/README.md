# 世界地图：队友点位 + 任务标记快照包 + 装饰级指路 + 不支持弹窗（2026-11-23）

> 状态：4 路只读取证（IDA 095/2016 + 服务端 + path 数据）→ 用户拍板 → 全量构建 → 部署 live + 短启无闪退；**真机双端联调未验收**（服务端需重启，2B 端到端未验）。
> 客户端部署：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` SHA256 `ce01f0c7c56ca474ee628a3f788934f8c54785c088976dcddba8fd1e06ff1af4`（5554688 B，19:35:35）
> 服务端：`E:\project\BeiDou-Server_s9`，`mvn compile` 通过（WorldMapQuestMarksHandler.class 已生成），**需重启服务端**生效。

## 1. 取证结论（4 路，行号/地址级）

- **192 本体**：加壳 x64，IDB 仅 3 个壳 stub，exe 内 WorldMap/QuestIcon/partyPos 字符串 0 命中，用户反馈 192 运行不了 → **无法静态/动态取证，闭环不可达**（不编造）。
- **095（同系列，IDA 权威）**：`CWorldMapDlg::SetWorldMap@0x9bd820` 用 `QuestGuide/QuestMark` 动画层画任务灯泡、`MapHelper/partyPos` 画队友；`CreateWorldMapDlg@0x9be670` 从 `GetPartyMemberData` 取队员 mapId；**明确无 auto fly/A→B 寻路**；不支持场景走 `CUtilDlg::Notice`（`MarkMapLocation_InWorldMap@0x823480`）。
- **2016 同族**：`CWorldMapDlg::Draw@0x1A73A70` 纯 IMG 合成，无任务/队友分支；通知是地图节点动画角标（精英怪/传送门），无 toast。
- **服务端（s9）**：无地图级任务标记（Quest.java 无 mapId、无 QUEST_MARK opcode）；但 `PartyHandlers.cpp:227-248` 已解析队友 mapId 入 `PartyModel`（零协议可用）；`Quest.canStart:288/canComplete:302/getNpcRequirement:648`、`MapFactory.getMapIdByLifeId:433` 可复用。
- **path 数据**：穷举 28 个 WorldMap*.img 的 27 个 path，100% 是装饰位图（无端点 id/邻接/权重）→ **无法真寻路**。

## 2. 实现（一次成型，5 阶段）

**A 队友点位（零协议）**
- `UIWorldMap.cpp` 构造加载 `MapHelper["partyPos"]`；draw 遍历 `PartyModel::get().data().members`，跳过 offline/mapid<=0，按 spot.map_ids 匹配画 partyPos（地图粒度，无地图内 x/y）。
- 修复分区名 bug：`:440-446` 由 `"WorldMap0"+prefix` 改为 `"WorldMap"+3位补零`（旧代码 prefix=0/2/5/55 拼错静默回落 WorldMap.img，副本/跨大陆显示不对根因之一）。

**B 不支持场景弹窗**
- `jump_to_map_region` 分区不存在（副本/事件/跨大陆）→ `UIOk` Notice（复刻 095 `CUtilDlg::Notice`）；i18n `uiworldmap.navi_unsupported`。

**C 装饰级指路（明确非真寻路）**
- 选中 spot 的 path 位图常亮 + 双层黄色高亮（0.28+0.55）替代原单层 0.35；`autofly_to_selected` 提示 `uiworldmap.autofly_route`（"路线示意，非自动寻路；自动飞行=直接传送"）。传送仍是 `!mapwarp`。

**D 任务快照包 2B（跨端）**
- 服务端：opcode `Send.WORLD_MAP_QUEST_MARKS(0x17C)` / `Recv.WORLD_MAP_QUEST_MARKS_REQUEST(0x11E)`；新 `WorldMapQuestMarksHandler`（TTL 750ms + npc→map 缓存）；遍历 `Quest.getAllQuests()`（新增）逐任务 `canStart(chr,getNpcRequirement(false))`/`canComplete(chr,getNpcRequirement(true))`（传真实 npcId，避免 null 陷阱），npc→map 用 `MapFactory.getMapIdByLifeId` 按 prefix 裁剪，位标志回包；`PacketProcessor` 注册。
- 客户端：`OutPacket.h` 请求 opcode 0x11E + `WorldMapQuestMarksRequestPacket`；`PacketSwitch.cpp` 回包 0x17C+注册+opcode_msg；`WorldMapHandlers` 新 handler 解析 `{mapid,flags}`；`UIWorldMap` 加 `set_quest_marks`（prefix 防乱序）+ 打开/切分区发请求（去重）；draw 用 QuestIcon 灯泡/Quest.notice1 书图标常驻显示。

**E 闭环**
- opcode 矩阵重生（`gen_opcode_matrix.py`，GATE OK）；客户端隔离全量 Rebuild（`MSBUILD_RC=0`，IntDir 隔离避 PDB 锁）；服务端 `mvn -pl gms-server -am compile` 通过；live exe 覆盖+实时 SHA 核对+短启 ALIVE。

## 3. 风险 / 未验收（如实声明）
- **2B 端到端真机未验**：服务端需重启 + 客户端重新打开世界地图拉快照；灯泡/书是否显示必须真机截图，且截图须先确认来源端+核对 live SHA `ce01f0c7…`。
- 已知限制：`getMapIdByLifeId` 只取 NPC 首次出现的地图；多地图 NPC 只标首图；跨频道队友 mapId 被服务端写 0（跳过）。
- path 是装饰级，非寻路——已用文案明确，不误导。
- 真·寻路若要补，需服务端导出 warp 拓扑建图（本次未做，用户选 1A）。

## 4. 回滚 / 关联
- 回退点：`build-wmquest-verify/MapleStory.exe`。
- 关联：`docs/features/worldmap-192-search/README.md`、`docs/features/worldmap-search-ui-fix-2026-11-22/README.md`。
- 取证临时脚本/IDA scratch 已清理；192 IDB 原件 mtime/MD5 未变。
