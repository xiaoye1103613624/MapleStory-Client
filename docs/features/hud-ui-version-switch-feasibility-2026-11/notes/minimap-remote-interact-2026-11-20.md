# 小地图远距离交互（自定义 QoL）— 2026-11-20

## 定位

自定义 QoL，**非**原生 MiniMap `OnMouseButton` opcode 407 还原。入口对齐本端场景已有交互，不发明协议、不改 BeiDou 服务端（`NPCTalkHandler` 无距离校验）。

## 入口对齐表

| 小地图操作 | 行为 | 对齐场景入口 |
|---|---|---|
| 左键 NPC 图标 | `TalkToNPCPacket(oid)`（含商店，不按 shop 过滤） | `MapNpcs::send_cursor` |
| ListNpc 行左键 | 同上 + `select_npclist` | 同上 |
| 双击他角标记 | `CharInfoRequestPacket(cid)` | `UIStateGame::doubleclick` |
| 双击自己标记 | `UI::emplace<UICharInfo>(oid)` | 同上 |
| 右击玩家标记（他角/自己） | `UIContextMenu::open_at` | `UIStateGame::rightclick` |
| 视口外标记 | 不可点（`canvas_pos_in_view`） | 与绘制裁剪一致 |

原生 opcode **407 不实现**。

## 代码

- `IO/UITypes/UIMiniMap.cpp` / `.h`：`hit_npc_marker` / `hit_player_marker`；`send_cursor` 左键 TalkToNPC；`doubleclick` / `rightclick` 覆盖。

## 素材迁移（AGENTS.md §3.1）

| 项 | 值 |
|---|---|
| 源 | 192 `UI_004.wz` / `UIWindow2.img/MiniMap/{BtNpc,ListNpc,iconNpc}`，零 IV `AAAAAA==` + 128B DEFAULT_KEY |
| 目标 live | `E:/MXD/BeiDou-Client-xy/Data/UI/UIWindow.img` / `MiniMap/` |
| 流水线 | staging 复制 live → `load_files` → `copy_paste_nodes` → `save_as` 异路径 → `batch_convert_key(GMS, verify:true)` → `unload_all` → 原子替换 |
| staging | `E:/MXD/_minimap_listnpc_staging/`（`work` / `pasted` / `out_gms`） |
| 备份 | `UIWindow.img.bak-before-listnpc-20261120` |
| verify fingerprint | `c83f7af4a137c74f0a13238fe5f4e7b8265d5b06a26b6adead27f2dffa8aa149` |
| live SHA256（替换后） | `096275339A6BD91D95155E3C231E5A2C33ECC81E3F0E7D22E7F897A8EA0B6634` |
| orange-wz 核验 | `MiniMap` 含 `title/MinMap/MaxMap/Min/BtMap` + **`BtNpc`/`ListNpc`/`iconNpc`** |

### Probe / 运行时

- orange-wz `list_children` 已确认三节点在 `out_gms` **与替换后 live** 均存在。
- 客户端若 `resource.nxcache=true`，须删 `Data/.nxcache/UI.nx`（当前 nx 时间戳早于新 img，属脏缓存）后再启；默认 `resource.nxcache=false` 时直接读 `.img`，**重启客户端**即可。
- `--resource-probe` 本轮因已有 MapleStory 多实例 + 全量解析过久未收口；素材侧以 orange-wz live 核验为准，堆损坏/短启验收请用户重启后点验。

## 验收清单

- [ ] 远距左键 NPC 图标 → 对话/商店脚本弹出
- [ ] ListNpc 打开后点行名 → 同上
- [ ] 双击他角标记 → CharInfo；双击自己 → UICharInfo
- [ ] 右击任意玩家标记 → ContextMenu
- [ ] 视口外标记不可点；拖窗/列表滚动不误触
- [ ] 素材：`MiniMap` 可见 `BtNpc`/`ListNpc`/`iconNpc`；短启无 `0xc0000374`
