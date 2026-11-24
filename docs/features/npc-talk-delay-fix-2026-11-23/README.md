# NPC 对话"半天才打开/打不开"修复（2026-11-23 21:45 反馈）

> 状态：截图来源确认（本端）+ 4 路只读取证（IDA 095 NPC 发包）+ 一次性 4 项修复 + 全量构建 + 部署 live + 短启无闪退；**真机发包未验收**。
> 部署：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` SHA256 `bbeda08d78bdca7fc520695a6488937adbfc21ea7bc2edc23c216419cd846f42`（5561856 B，22:24:50）

## 1. 反馈与证据
用户 21:45 截图 `XiaoYe_2026-11-23_21-45-36.png`：小地图双击 NPC / 右侧 NPC 列表双击 / 场景点 NPC 都"半天才打开或打不开对话框"。
来源：live 当时 `5736a67f…`（20:24）早于截图 → 本端证据。

## 2. 逐条根因（行号/地址级）
1. **多余 NPC_ACTION 伴随包**：`NpcInteractionPackets.h:66-70` `TalkToNPCPacket` 构造内先派发 `NpcActionTalkPacket`(NPC_ACTION/197) 再发 TALK_TO_NPC(58) 双包。IDA 095 实证原生 `CUserLocal::TalkToNpc@0x9321f0` 是**单包**（opcode 63 版本差异，无 197 伴随包）；s9 `NPCTalkHandler` 不依赖 197（`NPCAnimationHandler` 只原样广播动画）。多余包 + 连点 → 服务端 `c.getCM()!=null` 分支静默 `enableActions()` 拒绝（NPCTalkHandler:69-71）→ 表现为"半天/打不开"。
2. **mouse-up 判定脆弱**：`UIMiniMap.cpp:521` 原 `mouse_up = !clicked && !is_primary_down()`；窗口失焦时 `UI.cpp` 强制清 primary_down、且鼠标移动帧也可能满足 → pending 误判/漏结算 → "有时点不开"。IDA 095 `HandleLButtonClk@0x933920` 证实原生也是 mouse-up 命中即发、无此门控。
3. **双击不处理 NPC**：`UIMiniMap::doubleclick:1547-1561` 只处理玩家 marker（CharInfo），NPC 双击直接 return —— 双击事件被吃掉但无动作。
4. **stale 裸指针**：`UIMiniMap.h:182` `listNpc_list` 存 `MapObject*`；`MapNpcs::remove/clear` 会 delete Npc，而 `npclist_signature` 只按 oid 集合刷新 → 同 oid 对象替换时裸指针悬垂 → `get_oid()` 读垃圾 → 服务端找不到 NPC 静默拒绝。

## 3. 修复（一次成型 4 项）
1. `NpcInteractionPackets.h`：`TalkToNPCPacket` 去 NPC_ACTION 伴随包，只发单包 TALK_TO_NPC(58)（保留 `NpcActionTalkPacket` 类本身，场景动画等其它用途不受影响）。
2. `UIMiniMap.cpp:521`：`mouse_up = !clicked`，去掉 is_primary_down 门控；pending 仍在 down arm、在 up/拖拽/滚动清零，hover 不误发。
3. `UIMiniMap::doubleclick`：新增 NPC 命中分支（右侧列表行 + 场景绿点），命中即发单包对话，与单击同语义。
4. `UIMiniMap.h/cpp`：`listNpc_list` 由 `vector<MapObject*>` 改为 `vector<NpcListEntry{oid, position}>` 值快照（`:489`/`:1569` 取 oid、`:1455` 取 position），彻底消除悬垂指针。

服务端未改（opcode 58/0x3A 是 s9 口径，IDA 095 的 63 是版本差异，不照搬）。

## 4. 构建/部署/验证
- opcode 矩阵重生 GATE OK；IntDir 隔离全量 Rebuild `MSBUILD_RC=0`；live exe 覆盖 + 实时 SHA 核对；短启 `_ab_launch.ps1 -Wait 12` → `ALIVE`。
- **真机发包未验收**：NPC 对话框是否稳定/快速打开需真机点击验证（场景/小地图/列表三处），且截图须先确认来源端 + 核对 live SHA `bbeda08d…`。
- 本轮多次撞并行会话构建写同 PDB，IntDir 隔离手法规避；未碰并行文件。

## 5. 回滚 / 关联
- 回退点：`build-npctalk-verify/MapleStory.exe`。
- 关联：`docs/features/worldmap-quest-party-navi-2026-11-23/README.md`。
- IDA 取证临时文件已清理，Maplestory95.i64 原件未改（save:false）。
