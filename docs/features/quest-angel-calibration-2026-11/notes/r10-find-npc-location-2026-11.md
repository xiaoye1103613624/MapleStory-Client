# 第十一面 · 任务窗「寻找NPC位置」功能实现 (r10)

> 时间：2026-11-23 23:5x CST。性质：**实现 + 构建验收**。
> 触发：用户问「原生和 minms 是什么逻辑？没有实现？」，确认按 MinMS 方案移植（"可以"）。
> 结论先行：原生有该按钮（ida `m_pBtMyLocation`），MinMS 有**纯本地查表**完整实现；
> 当前端此前是**空预留**（按钮显隐 r09 已修，点击无实现）。本轮接上点击功能。

## 0. 三方逻辑对照

| 端 | 「寻找NPC位置」逻辑 | 证据 |
| --- | --- | --- |
| 原生 083 | 有 `m_pBtMyLocation @0xc6c` / `m_pBtAllLocation @0xc74` 按钮，详情态出现；跳转动作字节级未取证（facts 里 OnButtonClicked 相关 case 被截断），不编造 | r07 §2.2；angel-quest-facts.md:1394 |
| MinMS | `npc_location_maps()` 读 `Etc/NpcLocation.img/<npcid>` → 打开/复用 UIWorldMap → `highlight_npc_locations()` 切到首图分区 → 弹 UIOk 列地图名。**纯本地查表、零封包** | MinMS UIQuestLog.cpp:45-69/678-736；UIWorldMap.cpp:458-474 |
| 当前端（本轮前） | `MARKNPC` case 空预留，注释写"两者都没有、不发明协议"；但零件其实齐（NpcLocation.img 存在、jump_to_map_region 已有） | UIQuestLog.cpp:1124（r08 版） |

纠正 r08/r09 里"需封包证据不发明协议"的过度保守说法：**跳转是客户端本地查表行为，不需要新协议**；
只有"付费传送"（`request_teleport`, UIWorldMap.h:138）才需服务端，两者不要混淆。

## 1. 本轮实现（行号级）

- `UIQuestLog.h`：`npc_location_maps(int32_t)` / `show_npc_locations(int32_t)` 私有方法声明。
- `UIQuestLog.cpp`：
  - include `UIWorldMap.h` / `UINotice.h`（UIOk 在 UINotice.h:156）。
  - `:959` `npc_location_maps()`：`nl::nx::Etc["NpcLocation.img"][<npcid>]` 子节点即 mapid（活数据：9000034 有 8 处）。
  - `:983` `show_npc_locations()`：反查地图 → 拼可读列表（≤6 处 + 总数）→ 打开/复用 UIWorldMap 并 `show_map_region(maps.front())` → 弹 UIOk。
  - `:1221` MARKNPC case：从 `selected_id` 取 `QuestData::get(id).get_npc()` 调 `show_npc_locations()`。
- `UIWorldMap.h/.cpp`：`show_map_region(map_id)` public 转发（内部 `makeactive` + `jump_to_map_region`），
  `jump_to_map_region` 保持 private 不动。
- i18n：`quest.npc_locations.{header,none,map_fallback,more,unknown_npc}` 中英双语。

## 2. 并行会话冲突处理（如实记录）

本轮编辑期间检测到**另一个会话同时改同批文件**（UITrade.cpp / Player.cpp / UIWorldMap.cpp / UIQuestLog.cpp）：
- 它加了 `show_map_region`（UIWorldMap.h:94/cpp:1362），我加了同义的 `reveal_map_location` → 重复接口。
  处理：**删掉我加的 `reveal_map_location`，统一采用它的 `show_map_region`**（更完整，含 makeactive）。
- 它并行改的 UITrade.cpp / Player.cpp 出现未收敛的编译错误（Stage 未 include、`nl::nx::Effect` 未声明），
  **均非本轮 quest 改动**。我没有改这些别人的 WIP。

## 3. 验证（如实）

- **全量构建通过（2026-11-24 00:17:10）**：`build_quest_npcloc_20261123.bat` → `EXITCODE=0`，日志解析 **error 行 = 0**；产物 `build-quest-npcloc-20261123/MapleStory.exe`（5,624,832 B，SHA256 前32 `9fe8fbd978501a01b6dab3603151851a`）。产物时间 00:17:10 晚于全部源码（最晚 UIQuestLog.cpp 00:01:29），含全部改动。
- 过程留痕：本轮编辑期间另一活跃会话曾先后引入 UITrade.cpp / Player.cpp / UIStatusBar.cpp 的未收敛中间态错误（非本轮 quest 改动，我没动其 WIP），导致前几次 `EXITCODE=1`；待其收敛后 00:17 重跑全量干净通过。
- **已部署到 live（2026-11-24 00:28:54，用户确认后）**：
  - live `E:/MXD/BeiDou-Client-xy/MapleStory.exe` 原子替换为构建产物；实时 SHA256 前32 `9fe8fbd978501a01b6dab3603151851a`，与产物逐字一致。
  - 替换前备份 `MapleStory.exe.bak_quest_npcloc_20261124`（旧 SHA `ea5328f7…`）。
  - locale 新增键 `quest.npc_locations.*` / `quest.detail.npc_unknown_inline` 已与源逐字一致。
  - 替换时发现 5 个 MapleStory.exe 僵尸进程（0 内存、无 Path，11/17~11/22 残留），确认不持锁后未杀进程、直接原子替换成功。
- **真机仍未验收**：已部署但**未启动游戏点按钮验证**。结论基于 MinMS 源码对照 + live NpcLocation.img + 全量构建 + SHA 核对。登录器会回滚 exe，若后续声称"已部署"须再次实时核对 live SHA。

## 4. 遗留

1. NpcLocation.img 对部分 NPC（如 9310088）无数据 → 弹"未找到该 NPC 的地图信息"，不编造。
2. 本端 `UIWorldMap` 无 MinMS 那种 `highlight_npc_locations` 持久高亮（多图同时标注）——
   本轮定位到首图即可，与原生"我的位置"单图定位语义一致；多图高亮是 P2 增强。
