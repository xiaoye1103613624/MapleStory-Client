# 第九面 NPC — 既有文档四态审计

> 取证日期 2026-11-09 · 四态 = 销案 / 推翻 / 新增 / 遗留
> 范围：`docs/features/beidou-v17-parity/notes/r*-npc*.md`（19 个）、`docs/features/s9-protocol-align/notes/s8-npc.md`、`docs/features/change-quickslot-persist/notes/npc-action-move-pending.md`、`beidou-v17-parity/notes/evidence/r17/rea_search_*.json`、`ui-v83-window-restore/`、`tools/level999/full_classes/org/gms/**/*NPC*.class`。
> 性质：只读取证归组，不改任何既有笔记、不改源码。

## §0 归组总览

| 态 | 数量 | 含义 |
| --- | --- | --- |
| 销案 | 8 组 | 既有结论被 083 一手 + 当前端源码双向闭合 |
| 推翻 | 1 组 | 既有结论被本轮证据证伪（095 线冒充 083） |
| 新增 | 6 组 | 083 一手新发现，既有笔记无对应 |
| 遗留 | 5 组 | 真机/IMG/VecCtrl 缺口，本面不关闭 |

---

## §1 销案（既有结论 → 本轮闭合）

| # | 既有笔记 | 既有结论 | 本轮 083 一手 + 当前端 | 判定 |
| --- | --- | --- | --- | --- |
| S1 | `r143-npc-talk-shop-softkey.md` | GameMenu `!` oid→`TalkToNPCPacket`；`%` slot,itemid,qty→`NpcShopActionPacket` BUY | `CUserLocal::TalkToNpc @ 0x95fe9e` `COutPacket(58)`+`Encode4(oid)`；当前端 `Net/OutPacket.h:99 TALK_TO_NPC=58` / `:101 NPC_SHOP_ACTION=61` 全在 | 销案（数值一致，包体差异记 D1） |
| S2 | `r172-npctalk-shop-softkey.md` | NpcTalk GetText/GetNumber/Simple + `NpcTalkMorePacket(amount,as_number)` | `Net/Packets/NpcInteractionPackets.h` `NpcTalkMorePacket` 全族（`(lastmsg,response)` / `(string)` / `(int32)` / `GetNumber` / `AskAvatar` / `AskBoxText` / `AskQuiz` / `AskSpeedQuiz` / `AskPet` / `DimensionalMirror`）+ `UINpcTalk.cpp:1699-1778 dispatch_input_field` 闭合 | 销案 |
| S3 | `r787-nearest-npc-talk.md` | ShortCut/GameMenu/StatusBar SoftKey `N` 对最近 oid `TalkToNPC` | `CNpcPool::FindNpc @ 0x6d9390`（`GetDCRange` + `GetActionFrameList`）+ `MapNpcs.cpp:85-109 send_cursor` `npc->inrange()` → `TalkToNPCPacket(oid)` | 销案 |
| S4 | `r656-warn-merchant-npc-storage-rem.md` | `NpcDialogue / OpenNpcShop rem`（early-return 前 drain） | `Net/Handlers/NpcInteractionHandlers.cpp:41-401 NpcDialogueHandler` 收尾 `drain_rest()` + `:403-459 OpenNpcShopHandler` 逐字段读；`ConfirmShopTransactionHandler` `read_byte(mode)+drain` | 销案 |
| S5 | `s8-npc.md:1-49` | c2s 5 包 `TALK_TO_NPC 58 / NPC_TALK_MORE 60 / QUEST_ACTION 107 / DAMAGE_REACTOR 205 / NPC_SHOP_ACTION 61` 中 4 个逐项对齐；唯一疑点 `QUEST_ACTION action==0(restoreLostItem)` s9 读 2 int 客户端不发 itemid，低严重度不改 | 本面 083 `OnPacket` 不覆盖 QUEST_ACTION（独立链），当前端 `Net/OutPacket.h` 58/60/61 全在；疑点保持低严重度、不改客户端 | 销案（承前） |
| S6 | `s8-npc.md` | v83 portal 无独立 c2s（走 CHANGEMAP） | Angel 71 函数无 portal 发包，`CNpcPool::OnPacket` 八条分发无 portal | 销案 |
| S7 | `npc-action-move-pending.md:5-16` | s9 `NPCAnimationHandler` `length==6` 对话 / `>6` 移动（丢尾 9）；本端 `NpcActionTalkPacket(oid,0xFF,0x00)` / `NpcActionMovePacket` / `MovePathEncode.h` / 授控 ≥1000ms+fh>0 | Angel `CNpc::GenerateMovePath @ 0x6d3273` `COutPacket(197)` `Encode4(oid)+Encode1(action)+Encode1(chatIdx)+Flush`；`Npc.cpp:220 path_accum_ms>=1000 && fhid>0`；`NpcActionTalkPacket` `write_int(oid)+write_byte(-1)+write_byte(0)` | 销案（083 一手替换 095 依据，见 §2 推翻） |
| S8 | `r15 tab_plate` 误判（第八面 r05 已记，本面交叉） | 曾误判 borrow `recommend/0` 桃色横幅 | 当前端 `UISkillBook.cpp` 正确 `UiAssetResolve::window("Item")["New"]`；本面 `UINpcTalk.h` 布局常量同样引用 IDA `sub_9A64FC`，083 侧具名 `CUtilDlgEx::SetNPC @ 0x9a64fc` 闭合 | 销案（交叉面） |

## §2 推翻（既有结论证伪）

| # | 既有笔记 | 错误结论 | 本轮证据 | 处置 |
| --- | --- | --- | --- | --- |
| **O1** | `docs/features/change-quickslot-persist/notes/npc-action-move-pending.md:18-29` | 「IDA（`Maplestory95.i64`）：`CNpc::GenerateMovePath 0x671590` / `CMovePath::Encode 0x666e20` / `Flush 0x668160` / `IsTimeForFlush ≥1000ms` / **v95 send op 241**」被当作 NPC_ACTION 上行权威；`Gameplay/MapleMap/Npc.cpp:253` 注释「字段序仅来自 IDA `CMovePath::Encode @ 0x666e20`」 | **095 线**：`Maplestory95.i64` + `docs/features/port-095/evidence/opcode-v95/dec/_OnNpcPacket_CNpcPool__*.c` 是 095/v95 样本，op `241 ≠ s9 197`，地址 `0x671590 ≠ 0x6d3273`。083 权威 = Angel `CNpc::GenerateMovePath @ 0x6d3273` `COutPacket(197)`，`CMovePath::Flush(v4+428,&v11,0,0)`，`IsTimeForFlush(this:(v4+428),a2:0)`。当前端字段序仍与 083 对齐（因协议同源），但**依据必须换** | 推翻依据来源，不推翻实现。校准文档以 083 `0x6d3273` 为唯一权威；095 地址/op 降级为「历史实现注记」，禁引为 083 一手事实。记 D5。 |

> 说明：本面「推翻」仅 1 组且是**证据来源**被推翻，不是功能实现被推翻。当前端 `NpcActionMovePacket` 字段序与 Angel 083 逐字对齐，实现保留，s9 优先不变。

## §3 新增（083 一手，既有笔记无对应）

| # | 新发现 | 证据 | 归维 |
| --- | --- | --- | --- |
| N1 | `CNpcPool::OnPacket @ 0x6d9734` 八条 s2c 分发逐字：`81/82/257/258/259/260..262/263` | `angel-npc-facts.md:647-663` + `r01 §3` | C / D |
| N2 | `CUserLocal::TalkToNpc` 的 `COutPacket(58)` 带 `Encode2(x)+Encode2(y)`，且有任务列表时走 `CNpc::ShowQuestList` 本地开窗**不发 58** | `r01 §3` 完整伪代码 `:95fedf/:95ff0b/:95ff1f` | B / D1 |
| N3 | `CNpc::GenerateMovePath` 的 `for(i=0;i<55;++i)` + `CVecCtrl::UpdateActive` + `IsTimeForFlush` 攒 path 循环 | `r01 §3` `:6d32e3/:6d32f8/:6d32fc` | B / D6 |
| N4 | `CNpcPool::OnNpcChangeController` `Decode1(bool)+Decode4(oid)` → `SetLocalNpc` / `SetRemoteNpc` 双分支 | `angel-npc-facts.md:571-579` + `r01 §3` | C |
| N5 | 083 SP_ 节点族（MapleTV TVON/OFF/TVCHAT1/2、QuestIcon/D、UtilDlgEx List0-3、ScriptInfo、QuestInfo、NPCSummon/NPCReturn、DCMark、String/Npc.img、NPC/%07d.img） | `angel-npc-facts.md` :92/:134/:361/:441/:471/:504/:726 | E / F |
| N6 | `CNpcTemplate::GetNpcTemplate @ 0x6dcb16 (6414 B)` / `RegisterNpc @ 0x6dce02 (2000 B)` / `CUtilDlgEx::SetNPC @ 0x9a64fc (1915 B)` 三个大函数一手具名（当前端 `UINpcTalk.h` 注释引用的 `sub_9A64FC` 在 083 侧闭合） | `angel-npc-facts.md:724/:758/:819` | A / E |
| N7 | 71/71 字节级一致（1-EA `0x6d065b ::_GCNpc` 32B 双端同）+ `nine_face_consistency` 九面表 | `angel-beidou-npc-ea-bytelevel-audit.json:91-103` | A / §0 |
| N8 | 095 历史注记与 083 一手地址对照表（0x671590/v95 241 vs 0x6d3273/197） | `r01 §0.5` / 本文件 §2 | 决策纪律 |

## §4 遗留（本面不关闭，挂 P2/P3）

| # | 缺口 | 来源 | 归属 |
| --- | --- | --- | --- |
| L1 | 真机 c2s 197 `length>6` 十六进制样例（对照 `CMovePath::Flush` 字节） | `npc-action-move-pending.md:46` | P2 真机 |
| L2 | 双开观感（第二端看 NPC move） | `npc-action-move-pending.md:47` | P2 真机 |
| L3 | 完整 `CVecCtrlNpc` 攒多节 / Interrupted 动作帧 / 下行完整 elem 插值（本端只吃头坐标） | `npc-action-move-pending.md:48-49` + `Npc.cpp:250-286` 单节 attr=0 | P3 / D6 |
| L4 | IMG 门禁阻塞 → F 维 12 项贴图/像素证据全部 ⏳ | `r03` | P2 orange-wz |
| L5 | 083 `CNpc::ShowQuestList @ 0x6d3392 (27614 B)` 独立任务列表窗，当前端无对应 UI（直发 58） | `r01 §5` 任务交叉 | P3 ⑪ 任务面 / D3 |

## §5 REA subject 单列（不冒充 13339 一手）

`beidou-v17-parity/notes/evidence/r17/rea_search_{AranSkillGuide,EnchantSkill,MacroSkill,Skill,SkillEx,SkillMacro,SkillUp}.json` 的 subject = `BeiDou-ClientV17.7/BeiDou.exe`，sha256 `1198fa57ca5a7c489bae43ec13c69681d9cabe0f96762f3dc0357facf2e7d4df`，base `0x400000`，size `0xa94000`，md5 `44c1030ecd8484e849c83892eb8fa025`；`rea_search_SkillEx/SkillMacro/SkillUp` 均 `n:0` 全空 → stripped 端符号层几乎全空，反证 Angel 带符号 IDB 只补语义层。**这些是既有离线 REA 样本，不是 13339 实时端点，不与本面 `angel-npc-decompile.json` 混列。**

同理 `tools/level999/full_classes/org/gms/**/*NPC*.class` 是 s9 服务端 Java 反编译，属服务端协议基准，不作为客户端 083 一手。

## §6 既有笔记索引（ops_glob 命中 105 项，scan-budget 截断，信号已取）

- `beidou-v17-parity/notes/` 19 个 `r*-npc*.md`：`r143 / r172 / r656 / r787`（本文件逐字读）、其余 `r28/r30/r51/r57/r117/r118/r122/r159/r165/r251/r301/r306/r318/r330/r336/r342/r358/r365/r377/r387/r446/r472/r496/r506/r569/r585/r612/r627/r632/r636/r677/r703/r728/r736/r752/r780/r785/r791/r796` 系列中 NPC 相关条目。
- `beidou-v17-parity/notes/ui_layout_dump/`：`skill_backgrnd.png` / `skill_backgrnd_grid.png` / `_skill_nodes.txt` / `_skillbands.{py,txt}`。
- `ui-v83-window-restore/`：npctalk 截图几何参考帧。
- `change-quickslot-persist/evidence/`：`npc-action-generatemovepath-2026-11-06.md` / `npc-vecctrl-path-accumulate-2026-11-06.md` / `cmovepath-encode-0x666e20.decompiled.txt`（**均 095 线，见 §2 推翻 O1**）。
- `docs/features/port-095/evidence/opcode-v95/dec/_OnNpcPacket_CNpcPool__*.c`：**095 禁引**。

## §7 性质标注

本文件是校准/对照/决策点/Plan 的输入材料，非实现提交。四态归组只改本面 README 的判定列，不改任何既有笔记、不动游戏源码、不建提交。所有冲突（D1–D14）交用户裁决后 P1 才进实现。
