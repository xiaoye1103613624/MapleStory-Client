# round72 · NPC_ACTION 移动包形 / CANCEL_DEBUFF 依据链按 Angel 083 复核纠偏

## 0. 授权与范围
- 目标 (m11386 Round 72/90)：以 Angel(13339) 083/v83 带符号 IDB 为权威校准源，逐功能只读取证 → 对照清单 → 决策点 + 一次性 Plan → 实现。
- User said (m09859)：「若碰到ui布局、动画一定要确认是否修复了，如果不是按照Angel 布局修复过的，可以重新修复；之前有很多参照BeiDou或者自行修复的ui、动画都有些问题；」
- User said (m11385)：「记录一次有可见 PUA 输出的交付，不记录评分。」
- 本轮性质：**纯注释依据链纠偏，0 行为字面量**。台账 PLT §28-A 挂账二十轮的「5 处 095 地址污染」+ 全仓扫出 4 处同族 = 9 块 11 行。
- 硬约束：083/v83 only 禁 095 线符号/IDB 作依据；s9 协议优先；校准≠改回原生；数值重合≠语义重合。

## 1. 缺陷逐字（旧注释）
- `Net/Packets/MovePathEncode.h:2-3` `CMovePath::Encode 移植（Maplestory95.i64 @ 0x666e20）` + `证据：.../cmovepath-encode-0x666e20.decompiled.txt`
- `Net/Packets/NpcInteractionPackets.h:42,44` `包形对齐 Maplestory95 CNpc::GenerateMovePath @ 0x671590` / `Encode 权威：0x666e20`
- `Net/Handlers/WaveAShellHandlers.cpp:3585` `解析头 x/y 对齐 Encode（IDA 0x666e20）`
- `Gameplay/MapleMap/Npc.cpp:253` `字段序仅来自 IDA CMovePath::Encode @ 0x666e20`
- `Character/Player.cpp:211` `对照 port-095 CWvsContext::CheckTemporaryStatDuration`
- 全仓补 4 处同族：`MovePathEncode.h:16`（`跟伪代码`）、`:120`（`default→LABEL_15`）、`OutPacket.h:152`（`CheckTemporaryStatDuration 语义`）、`CommonPackets.h:45`（`原生 CheckTemporaryStatDuration`）。
- 11 行全部在注释里，**0 行为字面量**。

## 2. 活 IDA 083 复核（门禁 5：不拿旧战报当权威）
- `python tools/_round72_ida.py` → `ROUND72_IDA_ALL_OK`；`tools/_ida_round72/*.txt`。
- 真符号：`Encode @0x68a563`(0x269) / `Decode @0x68a33c` / `CNpc::GenerateMovePath @0x6d3273` / `CMob::GenerateMovePath @0x66b6fc` / `OnTemporaryStatSet @0xa202be` / `OnTemporaryStatReset @0xa2071f`。
- 证伪：`py_eval ida_funcs.get_func(0x666E20)` → `0x666362 sub_666362`（匿名函数内）；`0x671590` → `0x6712e0 ?TrySpeaking@CMob@@IAEXJJ@Z`。`tools/_round72_ida2.py` 复跑确认。**两个 095 地址在 083 均非注释所指函数**。
- `*CheckTemporaryStatDuration*` / `*CancelDebuff*` 全 0 命中 → CANCEL_DEBUFF 的「原生」措辞是 095 线污染。

## 3. 双重造假翻案（决定性）
- 083 `CMovePath::Encode @0x68a563`：`Encode2(x); Encode2(y); Encode1(count);` → 头 **只有 x/y 两个 short + 1 count byte，没有 vx/vy**，attr switch 只到 0x16。
- 当前端 `Net/Packets/MovePathEncode.h:42-56`：`write_short(x); write_short(y); write_short(vx); write_short(vy); ...` → 头 4 short + count，attr 表含 0x23/0x24。
- 旧 095 证据文件 `cmovepath-encode-0x666e20.decompiled.txt`（16317B）头逐字 = `Encode2(m_x/m_y/m_vx/m_vy)+count` + `0/5/0xC/0xE/0x23/0x24` + `CClientOptMan::GetOpt(2) usRandCnt` + `LABEL_15` + keyPad + rect。
- ⇒ **当前端实现逐字来自 095 `0x666e20`，与 083 `0x68a563` 是不同版本**。机械把注释 `0x666e20` 换成 `0x68a563` 会造出「用 083 真地址给非 083 包形背书」的新谎言。
- `CNpc::GenerateMovePath @0x6d3273`：`COutPacket(197); Encode4(oid); Encode1(action); Encode1(chatIdx); CMovePath::Flush(...) → Encode; SendPacket` → opcode 197 一致，但包体走 Flush、字段序不同。
- s9 `NPCAnimationHandler.java:37-46`：`length==6` → oid/action/chat；`length>6` → `readBytes(length-9)`（**尾 9 丢弃属实**）。当前端下行解析是 s9 契约客户端侧，非 083 原生。

## 4. 最小修复（0 行为字面量）
- 10 块注释替换、7 文件、11 行 095 字面量清零。
- 新口径统一：① 包形/字段序**以 s9 服务端契约为准**（NPCAnimationHandler length-9 / RecvOpcode.CANCEL_DEBUFF(99)）；② 083 真符号@EA **仅作对照**（`0x68a563` / `0x68a33c` / `0x6d3273` / `0xa202be` / `0xa2071f`）；③ 明写「字段序不同，不得据 083 改行为」。
- CRLF 精确替换（`tools/_round72_deliver.py` 按行号倒序 + guard 子串断言，保留 4 个文件无尾换行 / 3 个有尾换行差异）。

## 5. 决策点 + 一次性 Plan
- **D-C（交用户裁决）**：是否把 NPC_ACTION 移动包形整体重写成 083 `CMovePath::Encode` 字段序（头 x/y+count）。这是**协议级**改动，会破坏 s9 联机，按「校准≠改回原生 / s9 优先」本轮不动。
- **D-C2（交用户裁决）**：是否在 s9 服务端解禁 `CancelDebuffHandler`。客户端已协议对齐，本轮不动。
- Plan（本轮已执行）：只读取证 → 对照清单（§1-§3）→ 决策点（本节）→ 零风险注释纠偏（§4）→ harness/门禁/真重编 → 四文档 + 原子提交 → 真机 backlog。

## 6. 真机 / 待办 backlog
- 38-1：NPC_ACTION 移动段（length>6）真机联机观感 / 服务端 log 核对 s9 尾 9 丢弃。
- 38-2：CANCEL_DEBUFF(99) 空包真机服务端是否收到（s9 handler 空实现）。
- 38-3：决策点 D-C / D-C2 待用户裁决后另立 wave。
- 38-4：`PLT §28-A` 台账已标闭环（9 处），如后续再发现 095 依据按同法纠偏。
- 38-5：五屏截图核对 NPC 移动动画与 083 观感差异（若 D-C 裁决通过）。

## 7. 提交 scope
精确 `git add` 13 文件：7 源码 + `tools/test_round72_npcaction_083.py` + `evidence/npcaction-083-round72.json` + 本 notes + dev-plan README（2-27）+ PLT README（§38 + §28-A 闭环标）+ `docs/features/s9-protocol-align/protocol/opcode-matrix.json`。
一次性脚本 / IDA 反编译 / 旧 095 证据文件仅本地证据链留存，不进版本库。

### 7.1 opcode-matrix.json 重生成（主动闭环）
本轮改了 `Net/OutPacket.h` 注释 → `gen_opcode_matrix.py --check` 首次报 `GATE STALE (not blocking): hashed sources changed: Net/OutPacket.h`。拿 STALE 当门禁交付是自嗨，故跑 `python tools/gen_opcode_matrix.py` 重生成后复检到 `GATE OK: opcode matrix matches both sources (s9 a78ba681a4)`。3 行变化：
- `client_source_hashes["Net/OutPacket.h"]`：`4a4b0eed…` → `be70cbca…`（注释变化触发哈希，派生事实同步）；
- `generated_at`：`2026-11-10T17:52:54` → `2026-11-11T04:01:59`；
- `s9_git_head`：`3741089c0f` → `a78ba681a4` —— 现场 `git -C E:/project/BeiDou-Server_s9 log -1` 核实该提交真实存在（`2026-11-10 18:07:34 ✨ feat(character): 新增 per-character damage_skin 存储与下发 (#L-B A+B)`），是 round55 外部程序的真实服务端提交，本轮只做事实同步，未改任何服务端代码。
- 业务数据（recv/send consts、collisions、c2s/s2c 计数）逐字不变：`s9 recv=182 send=314 handlers=174` / `client out=148 in=307` / `c2s MATCHED=146` / `s2c HANDLED=305` / `stale client s9-assertion comments: 0`。

## 8. 证据优先级与教训
- 优先级：活 IDB 反编译 > 095 旧证据文件 > 源码注释 > 台账。
- 教训 1：**换地址不等于换依据** —— 注释里的 095 地址不能机械替换成 083 真 EA，否则用真地址背书假包形。
- 教训 2：全仓扫同族（`ops_grep` 关键词集）比只修台账 5 处更闭环（补出 4 处 `伪代码`/`LABEL_15`/`原生 CheckTemporaryStatDuration`）。
- 教训 3：`s9 优先` 与 `083 权威` 不冲突 —— 083 是布局/符号权威，s9 是协议契约权威，注释须分列、不互相背书。
- 教训 4：CRLF 文件按行号倒序替换 + guard 断言，避免 tab/中文 old_string 失配与行号偏移。
