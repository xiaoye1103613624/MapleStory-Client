# 第九功能面 · NPC / NPC 对话 / NPC 商店 — Angel 083 逐功能校准

> 取证日期 2026-11-09 · 权威源 `Angel.exe (pid=29620, port=13339)` 083/v83 带符号 IDB
> 对照源 `BeiDou.exe (pid=31332, port=13337)` 同二进制 stripped
> 基线客户端 `MapleStory-Client`（branch `feature/dev_0.0.1`，cwd `E:\project\MapleStory-Client`）
> 性质：**只读取证 + 对照清单 + 决策点 + 一次性 Plan**，非实现提交；全程未改游戏源码/协议、未建提交。

## §0 五条结论

| # | 结论 |
| --- | --- |
| 0.1 | **九面 EA 字节级一致钉死同二进制**：本面 71 funcs / 15 classes，crosscheck `70/71`，唯一 missing `0x6d065b ::_GCNpc` 经两端各读 32 字节逐字相同洗白 → `71/71 by bytes`。九面表 login `327/328`、statusbar_hud `547/548`、corewindow `439/441`、social_economy `752/752`、pet_summon_dragon `968/968`、activity_minigame `844/844`、cashshop `938/938`、skill `106/106`、npc `71/71`（by bytes）。Angel 只补符号/语义层，BeiDou 侧 70 EA 全 `sub_XXXXXX`、`beidou_real_named=0`。 |
| 0.2 | **本面 c2s 仅 2 个 opcode 字面量**：`COutPacket(58)`（`CUserLocal::TalkToNpc` / `CNpc::ShowQuestList`）、`COutPacket(197)`（`CNpc::GenerateMovePath @ 0x6d3273`）。`CNpc::GenerateMovePath` 字段序 `Encode4(oid)+Encode1(action)+Encode1(chatIdx)+CMovePath::Flush+SendPacket` 与当前端 `NpcActionMovePacket` 逐字对齐。 |
| 0.3 | **数值重合 ≠ 语义重合**：083 `TalkToNpc` 的 58 带 `Encode2(x)+Encode2(y)`，当前端 `TalkToNPCPacket` 只 `write_int(oid)` + 顺带派 197 对话姿态；c2s `NPC_ACTION=197` 与 s2c `NPC_ACTION=260` 同名不同号；既有 095 线 `v95 op 241` 与 s9 `197` 又是第三套命名空间。083 与 s9 分列，禁止仅凭数值判等。 |
| 0.4 | **095 依据被推翻（证据来源，非实现）**：`npc-action-move-pending.md` 引用的 `Maplestory95.i64`（`0x671590` / `0x666e20` / `0x668160` / `v95 op 241`）降级为历史实现注记；本面 083 权威 `0x6d3273` / `op 197`。当前端字段序与 083 对齐，实现保留，依据换源。 |
| 0.5 | **遗留收敛、IMG 阻塞**：F 样式维 12 项因 orange-wz 未连接全部 ⏳；真机 c2s 197 `length>6` / 双开观感 / 完整 VecCtrl / 083 `ShowQuestList` 独立任务窗归 P2/P3。判定分布 **✅72 / ⚠️12 / ⏳14 = 98**（脚本逐条重数，见 §9.3）。 |

## §1 取证基线

### §1.1 IDA 端点表

| 端点 | pid / port | 状态 | 产出 |
| --- | --- | --- | --- |
| Angel 083 带符号 | `Angel.exe` / 13339 | ✅ REACHABLE | 71 具名符号、71/71 反编译、2 c2s opcode |
| BeiDou stripped | `BeiDou.exe` / 13337 | ✅ REACHABLE | 70/71 EA、1-EA 32B 字节审计 |
| orange-wz IMG | — | ⚠️ 未连接（仅 `json-photoshop`） | 降级链见 `notes/r03-img-blocked-fallback.md` |

### §1.2 三联门禁（REA → IDA → IMG）

- REA/IDA：端点探测双 REACHABLE；`tools/ida_angel_npc_{ea,decompile,crosscheck,byteaudit,facts}.py` 全跑通（`DONE 71/71 ok` / `70/71 EAs identical` / `VERDICT: 71/71 by bytes` / `922 lines facts`）。脚本由第八面已验证 SOP 派生，`byteaudit` 改为通用版（无硬编码 EA，读 `missing_eas` 列表逐个洗）。
- IMG：阻塞，按 §1.1 降级。不改门禁标尺：F 维不伪造 ✅。
- 门禁 `python tools/gen_opcode_matrix.py --check` → `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)`（见 §9.2）。

### §1.3 脚本 + evidence + notes

```
tools/ida_angel_npc_ea.py            → evidence/angel-npc-symbol-inventory.json        (16649 B)
tools/ida_angel_npc_decompile.py     → evidence/angel-npc-decompile.json               (314388 B, 71/71 ok)
tools/ida_angel_npc_crosscheck.py    → evidence/angel-beidou-npc-ea-crosscheck.json   (12698 B, 70/71)
tools/ida_angel_npc_byteaudit.py     → evidence/angel-beidou-npc-ea-bytelevel-audit.json (1887 B, 71/71 by bytes)
tools/ida_angel_npc_facts.py         → evidence/angel-npc-facts.md                     (34201 B / 922 行 / 2 opcode)
notes/r01-angel-native-npc-facts.md   ← 083 一手事实基线
notes/r03-img-blocked-fallback.md     ← IMG 阻塞降级 + F 维 12 项
notes/r05-npc-four-state-audit.md     ← 既有笔记四态（销案8/推翻1/新增8/遗留5）
```

### §1.4 本面独有技术点

1. `byteaudit` 通用化：不再硬编码单一 missing EA，直接消费 crosscheck 的 `missing_eas`；九面一致性表随面数自动追加 `npc`。
2. `functions` 是 dict（70 键，`FindNpc` 双重载合并），逻辑 71 / 物理键 70；facts 按 71 计数，读取器按 dict 遍历（一次 KeyError 已修，非任务失败）。
3. `decompile` MCP 返回多态（纯文本 / `{"addr","code","refs"}` JSON 串），`code_of` 归一沿用第八面修复，facts 无 `{"addr":...` 残留。
4. 095 线在既有实现注释里密集出现（`Npc.cpp:253`、port-095 dec、change-quickslot evidence），本面逐处标「历史注记」而非静默沿用。

## §2 口径

### §2.1 七维 × 五判定

| 维 | 含义 | 条数 |
| --- | --- | --- |
| A | 功能 | 14 |
| B | 业务 / 协议 | 14 |
| C | 逻辑 | 14 |
| D | UI | 14 |
| E | 排版 | 16 |
| F | 样式（IMG） | 12 |
| G | 交互 | 14 |
| **合计** | | **98** |

判定：`✅` 双端一致/已闭合 · `⚠️` 差异需决策 · `⏳` 待 IMG/真机 · `❌` 缺失 · `⊘` 不适用。

### §2.2 证据优先级

运行时尚未实机 → 已服务资产（s9 服务端 Java / opcode-matrix）→ 当前进程配置（`Net/OutPacket.h` / `PacketSwitch.cpp`）→ 083 IDB 反编译 → 既有文档 → 注释/死代码。`angel-npc-decompile.json` 是本面最高优先一手；`Maplestory95.i64` 系 095 样本，优先级最低且禁引为 083。

### §2.3 083 vs s9 红线

- 083 是校准标尺，s9 是联机协议基准；**校准 ≠ 改回原生**。
- 三套命名空间（083 c2s / s2c、s9 c2s / s2c、095 v95）分列，同号异义不得判等。
- 冲突一律交用户裁决 D1–D14 后 P1 才动源码；P0 纯文档闭环。
- 095 禁引：`docs/features/port-095/**`、`Maplestory95.i64`、`CubeRerollPacket.h:182` 类注释污染。

### §2.4 冲突取值表

| 冲突 | 083 一手 | 当前端 / s9 | 取值 |
| --- | --- | --- | --- |
| c2s 58 包体 | `Encode4(oid)+Encode2(x)+Encode2(y)` | `write_int(oid)` + 197 对话姿态 | 保持 s9（D1） |
| c2s 58 任务分支 | 有任务列表 → `ShowQuestList` 本地开窗，不发 58 | 直发 58 | 保持 s9（D3） |
| NPC_ACTION 号 | c2s `197` / s2c `260`（同名） | 同 | 分列，不判等（D4） |
| GenerateMovePath 地址 | `0x6d3273` / op 197 / `Flush(v4+428)` | `NpcActionMovePacket` 字段序 | 083 依据换源（D5） |
| 095 线 | `0x671590` / `0x666e20` / `0x668160` / op 241 | 历史实现注记 | 不作为 083 一手（D5） |
| 完整 VecCtrl | `for(i<55)` + `CVecCtrl::UpdateActive` 攒多节 | 单节 attr=0 + 尾 9 | 保留简化（D6） |

## §3 七维 98 条

> 表头：`| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |`
> 判定列单标记，供 §9.3 脚本逐条重数。

### A 功能（14）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| A1 | NPC 模板加载 | `CNpcTemplate::GetNpcTemplate @ 0x6dcb16` + `SP_1666 String/Npc.img` / `SP_955 NPC/%07d.img` | `Npc.cpp:41-149` `nl::nx::Npc[extend_id(id,7)+".img"]` + `info.link` | ✅ | 销案 S-模板 |
| A2 | NPC 构造/析构生命周期 | `CreateNpc` / `0CNpc` / `1CNpc` / `_GCNpc` scalar deleting destructor | `Npc(int32_t npcid,int32_t oid,...)` `Gameplay/MapleMap/Npc.h:30-106` | ✅ | 4 个 free 函数闭合 |
| A3 | spawn / remove / clear | `OnNpcEnterField @ 0x6d9993` / `OnNpcLeaveField` `Decode4` | `MapNpcs.h:30-60` `spawn/remove/clear` | ✅ | 销案 |
| A4 | SPAWN_NPC_C 授控 | `OnNpcChangeController @ 0x6d9a83` `SetLocalNpc/SetRemoteNpc` | `Npc::set_control(bool)` `Npc.cpp:343-351` | ✅ | ✅ 双分支对齐 |
| A5 | NPC 移动上报 | `GenerateMovePath @ 0x6d3273` `COutPacket(197)` | `NpcActionMovePacket` `NpcInteractionPackets.h` | ✅ | §0.2 逐字对齐 |
| A6 | NPC 对话入口 | `CUserLocal::TalkToNpc @ 0x95fe9e` `COutPacket(58)` | `TalkToNPCPacket` `TALK_TO_NPC=58` | ⚠️ | D1 包体差异，s9 优先 |
| A7 | 对话 14 类 msgtype | `CUtilDlgEx::SetNPC @ 0x9a64fc` + `ShowQuestList` SP_ 族 | `NpcDialogueHandler` case 0..14 `NpcInteractionHandlers.cpp:41-401` | ✅ | 逐 case 闭合 |
| A8 | NPC 商店 | `CAdminShopDlg::SetNPC` / `CTrunkDlg::SetNPC` / `CUtilDlgEx::SetNPC` | `OpenNpcShopHandler` + `NpcShopActionPacket` 四模式 | ✅ | 销案 S1/S2 |
| A9 | 交易确认 | v83 `CONFIRM_SHOP_TRANSACTION` 消费 result | `ConfirmShopTransactionHandler` `read_byte+drain` | ✅ | inventory 仍走 MODIFY_INVENTORY |
| A10 | MapleTV 双屏 + 广播 | `SetMapleTVMessage @ 0x6d417e` / `DrawMapleTVMessage` / `TVON|OFF` | `Npc.h` 双屏 + `MapNpcs.cpp:74-83 broadcast_tv_message` | ✅ | 机制闭合，贴图 ⏳F1 |
| A11 | NPC 任务列表 | `CNpc::ShowQuestList @ 0x6d3392 (27614 B)` + `CQuestMan::GetQuestByNpc` | 当前端无独立窗，直发 58 | ⚠️ | D3，P3 ⑪ 任务面 |
| A12 | 模仿 NPC 数据 | `OnNpcImitateData` / `CNpc::SetImitatedLook` | `IMITATED_NPC_DATA=81` `PacketSwitch.cpp:295` | ✅ | s2c 分发闭合 |
| A13 | LimitedDisableInfo | `OnUpdateLimitedDisableInfo` / `CNpc::OnUpdateLimitedInfo` | `LIMITED_NPC_DISABLE_INFO=82` `:339` | ✅ | s2c 分发闭合 |
| A14 | 模板脚本 | `OnNpcTemplatePacket` / `CNpcTemplate::UpdateScript/OnSetNpcScript/RegisterNpc` | `SET_NPC_SCRIPTABLE=263` `:310` | ✅ | 083 4 方法具名 |

### B 业务 / 协议（14）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| B1 | c2s `TALK_TO_NPC` 数值 | `COutPacket(58)` | `TALK_TO_NPC=58` `OutPacket.h:99` | ✅ | 数值一致 |
| B2 | c2s 58 包体 | `Encode4(oid)+Encode2(x)+Encode2(y)` | `write_int(oid)` + 197 对话姿态 | ⚠️ | D1，不改 |
| B3 | c2s `NPC_ACTION` 字段序 | `197` `Encode4+Encode1+Encode1+Flush` | `NpcActionMovePacket` `write_int+2×write_byte+encode_movepath` | ✅ | §0.2 |
| B4 | c2s `NPC_TALK_MORE` 全族 | 本面未抽到 `COutPacket(60)`（走 UtilDlgEx 另一条链） | `NPC_TALK_MORE=60` + 10 类构造器 | ✅ | 覆盖差异非缺失，D2 |
| B5 | c2s `NPC_SHOP_ACTION` 四模式 | 本面未抽到字面量 | `NPC_SHOP_ACTION=61` `BUY/SELL/RECHARGE/LEAVE` | ✅ | 销案 S2 |
| B6 | c2s `CLICK_GUIDE` 空包 | 本面未抽到字面量 | `CLICK_GUIDE=162` 空包（s9 按职业开 MIMO/LILIN） | ✅ | 低严重度，D2 |
| B7 | s2c 八条分发 | `OnPacket` `81/82/257/258/259/260..262/263` | `PacketSwitch.cpp` 逐号注册 | ✅ | §0.3 双向一致 |
| B8 | s2c 304/305/306 | `CNpcPool::OnPacket` 不覆盖（独立链） | `NPC_DIALOGUE=304` / `OPEN_NPC_SHOP=305` / `CONFIRM=306` | ⚠️ | D4 分列，不判缺 |
| B9 | c2s 197 ↔ s2c 260 同名 | `GenerateMovePath` 发 197 / `OnNpcPacket` 收 260 | `OutPacket.h:190` / `PacketSwitch.cpp:319` | ✅ | 分列纪律 |
| B10 | 095 v95 op 241 vs s9 197 | `Maplestory95.i64` 第三套命名空间 | 既有 `npc-action-move-pending.md:26` | ⚠️ | D5 历史注记 |
| B11 | QUEST_ACTION 107 疑点 | 083 `OnPacket` 不覆盖 | `s8-npc.md` restoreLostItem s9 读 2 int 客户端不发 itemid | ✅ | 低严重度，不改客户端 |
| B12 | v83 portal 无独立 c2s | 71 函数无 portal 发包 | `s8-npc.md` 走 CHANGEMAP | ✅ | 销案 S6 |
| B13 | NPCAnimationHandler 契约 | 083 197 短包 = 对话 / 长包 = Flush 移动 | s9 `length==6` 对话 / `>6` 丢尾 9 | ✅ | 销案 S7 |
| B14 | opcode-matrix 门禁 | — | `gen_opcode_matrix.py --check` 双源一致 | ✅ | §9.2 GATE OK |

### C 逻辑（14）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| C1 | `OnPacket` switch 八 case | `case 81/82/257/258/259` + `260..262` + `263` 逐字 | `PacketSwitch.cpp:566-763 emplace` | ✅ | N1 |
| C2 | 授控双分支 | `Decode1(v3)+Decode4(v4)` → `v3!=0 ? SetLocalNpc : SetRemoteNpc` | `MapNpcs.cpp:31-48` remove-then-add | ✅ | N4 |
| C3 | 对话解析顺序 | `CUtilDlgEx::SetNPC` 先公共头再 msgtype case | `NpcInteractionHandlers.cpp:41` 注释「禁止先盲读 string」 | ✅ | 销案 S4 |
| C4 | `end_talk` 契约 | `ShowQuestList` 本地开窗不发包 | 2/13→`(x,0)`、5/6→空串、其它→`(x,-1)` | ✅ | s9 NPCMoreTalkHandler |
| C5 | drain + 缺资源降级 | `UtilDlgEx` 资源链 | `drain_rest()` + 缺资源 `end_talk` 不 emplace 隐形窗 | ✅ | 增量兼容，不推翻 |
| C6 | 攒 path 循环 | `for(i=0;i<55;++i)` + `IsTimeForFlush(v4+428)` + `CVecCtrl::UpdateActive` | `kNpcPathFlushMs=1000` `Npc.cpp:35` | ✅ | N3 |
| C7 | 单节 movepath | `CMovePath::Flush(v4+428,&v11,0,0)` 攒多节 | 单节 `attr=0` + 尾 9 空 keyPad+rect | ⚠️ | D6 完整 VecCtrl 预留 |
| C8 | Flush 触发条件 | `IsTimeForFlush` + `a2=0` | `path_accum_ms>=1000 && phobj.fhid>0` `Npc.cpp:220` | ✅ | 条件对齐 |
| C9 | 同 oid 重下发 | `OnNpcEnterField` `Decode4` | 先 `npcs.remove(oid)` 再 add，否则卡原地 | ✅ | 销案 |
| C10 | MapleTV short 溢出 | `MapleTVmsgX/Y` short | `9250025 msgX=65304` 按 int16 = -232 | ✅ | 已修 |
| C11 | 8 位 id 下溢 | `NPC/%07d.img` 模板串 | `extend_id(id,7)+".img"` 修 `length_error` | ✅ | 已修 |
| C12 | Evan 特判（交叉面） | `CSkillInfo::GetSkill` `22xx/2001`（第八面） | `CashShopParser.h hasSPTable` | ⚠️ | 跨面，不作本面依据 |
| C13 | skillPoint 降级（交叉面） | — | `UISkillBook.cpp:1098-1152` 直接 `SpendSpPacket` | ✅ | 增量兼容 |
| C14 | 数值重合≠语义重合 | 三套命名空间 | §2.4 冲突取值表 | ✅ | 决策纪律 |

### D UI（14）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| D1 | UINpcTalk 元素类型 | `CUtilDlgEx::SetNPC @ 0x9a64fc` | `TYPE=NPCTALK/FOCUSED=true/TOGGLED=false` `UINpcTalk.h:35` | ✅ | 独立真窗 |
| D2 | TalkType 枚举 | 083 msgtype 0..14 | `NONE=-1..SENDPREV=102` 15 值 | ✅ | Wire msgtype 对齐 |
| D3 | UIShop 元素类型 | `CAdminShopDlg/CTrunkDlg/CUtilDlgEx::SetNPC` | `UIDragElement<PosSHOP>` TYPE=SHOP | ✅ | — |
| D4 | `sub_9A64FC` 具名闭合 | `CUtilDlgEx::SetNPC @ 0x9a64fc (1915 B)` | `UINpcTalk.h` 注释引用 `sub_9A64FC` | ✅ | N6 |
| D5 | LayoutControls | `CUtilDlgEx::SetNPC` 布局 | `sub_9A83F6/9ACC20/9A8716/9A92C9` 注释族 | ✅ | 交叉印证 |
| D6 | 旧解析死代码 | — | `IO/Components/NpcText.cpp:1-166` 全注释 | ✅ | 非活动逻辑，不引用 |
| D7 | AskAvatar 预览 | `UtilDlgEx_Avatar` 资源链 | `CharLook avatar_base/preview` + `avatar_category` | ⚠️ | D8，资源疑缺 |
| D8 | Quiz/Pet/Mirror 窗体 | 083 `ShowQuestList` SP_ List0-3 | `UIInitialQuiz/UISpeedQuiz/UIAskPet/UIDimensionalMirror` | ⚠️ | IMG 待恢复 F8 |
| D9 | 缺资源不建隐形窗 | `UtilDlgEx` 缺失 | `end_talk` 降级 | ✅ | 增量兼容 |
| D10 | SimpleChoice 命中框 | `UtilDlgEx List` 族 | `#L<n>#label#l` `struct SimpleChoice` | ✅ | 行内排版 |
| D11 | reanchor_talk | — | 1280×720 保留原生 (400,240)，其它档居中 | ✅ | — |
| D12 | 交叉面 UIState | `CUIWnd` 族 | `UIStateCashShop.emplace<UICashShop>()` | ⚠️ | 本面无对应，分列 |
| D13 | UIShop 按钮族 | `CAdminShopDlg::GetCurNPCItem` | `BUY0..8/SELL0..8/EXIT/CHECKBOX` 等 | ✅ | — |
| D14 | skillPoint 面板缺失（交叉） | — | `BT_OKAY` 从未创建 `set_skillpoint(true)` 恒 false | ✅ | 降级是增量 |

### E 排版（16）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| E1 | MAX_HEIGHT | `CUtilDlgEx::LayoutControls` | `MAX_HEIGHT=248` `UINpcTalk.h` | ✅ | 常量对齐 |
| E2 | TEXT_X / TEXT_WIDTH | 同上 | `166 / 310` | ✅ | — |
| E3 | TEXT 三 padding | 同上 | `SLACK=18 / BOTTOM_PAD=34 / Y_PAD=7` | ✅ | — |
| E4 | BTN_DY 组 | 同上 | `BTN_DY=49 / BTN_DY_NEXT=-10` | ✅ | — |
| E5 | BTN_FROM_S 组 | 同上 | `-25 / 26` | ✅ | — |
| E6 | BTN 三 right inset | 同上 | `OK=54 / NEXT=68 / PREV=122` | ✅ | — |
| E7 | BTN edge inset | 同上 | `10 / AVATAR=140 / X_CLOSE=9` | ✅ | — |
| E8 | AVATAR 几何 | 同上 | `419×306 / LOOK(207,245)` | ✅ | — |
| E9 | SIMPLE 常量 | 同上 | `UL_EXT=8 / DASH #919191` | ✅ | — |
| E10 | TAB_PLATE 修正（交叉） | — | `TOPLEFT(4,23)/PITCH=34`（r15 误判已改） | ✅ | 继承第八面 |
| E11 | Q 帧坐标口径 | — | 帧坐标+(1,0) | ✅ | 逐像素口径 |
| E12 | MapleTV 双屏尺寸 | `SetMapleTVMessage` | 上屏 240×90 / 下屏 240×180 | ⚠️ | F1 待 IMG 像素 |
| E13 | TV 5 行文字 | `DrawMapleTVMessage` | A11M WHITE 行距 14 首行 y=-2 | ⚠️ | F3 待字体 bitmap |
| E14 | 名牌 / func 标签 | — | `namelabel(0,-4) / funclabel(0,18)` | ✅ | — |
| E15 | 经典技能窗坐标（交叉） | — | `CLASSIC_SKILL_OFFSET(10,87) / SLIDER_X=158` | ✅ | 继承第八面 |
| E16 | UtilDlgEx 九宫格切片 | `SP_3191..3194 List0..3` | `window4("UtilDlgEx")` | ⏳ | F5 IMG 阻塞 |

### F 样式（12，IMG 阻塞全部 ⏳）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| F1 | MapleTV 上下屏贴图 | `SP_3942/3943 TVON/OFF` | `tv_basic/tv_chat` | ⏳ | P2 orange-wz |
| F2 | TV chat 覆盖层 | `SP_3955/3956 TVCHAT1/2` | 覆盖层 Texture | ⏳ | P2 |
| F3 | 5 行文字 bitmap | `DrawMapleTVMessage` | A11M 字体 | ⏳ | P2 |
| F4 | 名牌描边 | — | `Text(A13B,CENTER,YELLOW,NAMETAG)` | ⏳ | P2 |
| F5 | UINpcTalk 底图切片 | `SP_3191..3194` | `MAX_HEIGHT/TEXT_*` 常量 | ⏳ | P2 |
| F6 | BtNext / BtClose 三态 | `UtilDlgEx["BtNext"]` | `BtClose2` 描边 X | ⏳ | P2 |
| F7 | 输入框 / Simple 命中框 | `UtilDlgEx List` | `SIMPLE_DASH #919191` | ⏳ | P2 |
| F8 | AskAvatar 预览窗贴图 | `UtilDlgEx_Avatar` | `AVATAR_W/H` 常量 | ⏳ | P2（本包疑缺） |
| F9 | 任务图标 / DCMark | `SP_3190 / SP_1665` | `QUESTICON_D / DCMARK` | ⏳ | P2 |
| F10 | NPC 召唤/回归特效 | `SP_4419/4420` | 当前端未接 | ⏳ | P2 + D7 |
| F11 | skillPoint 面板贴图 | `SP_4346`（第八面） | `window4("Skill")["skillPoint"]` NOT FOUND | ⏳ | P2 恢复后复核 |
| F12 | 宏按钮 / 组合栏贴图 | `SP_4346_..._BTMACROOUT` | `UISkillMacroEx` | ⏳ | P2 |

### G 交互（14）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置/决策 |
| --- | --- | --- | --- | --- | --- |
| G1 | 对话姿态顺带派发 | `TalkToNpc` 前 `OnKeyDownSkillEnd` | `TalkToNPCPacket` 构造内 `NpcActionTalkPacket(oid,-1,0)` | ✅ | 销案 S3 |
| G2 | 鼠标命中 NPC | `CNpcPool::FindNpc @ 0x6d9390` `GetDCRange` | `MapNpcs.cpp:85-109 send_cursor` `inrange()` | ✅ | 销案 S3 |
| G3 | 按钮应答 | `ShowQuestList` 按钮族 | `button_pressed` `NpcTalkMorePacket(lastmsg,-1|0|1)` :454-566 | ✅ | 销案 S2 |
| G4 | 键盘应答 | `CWndSkillGuide::OnKey a2==27`（交叉） | `send_key` escape/TAB/S/U/F/T/P :679 | ✅ | — |
| G5 | Simple 选择 | `UtilDlgEx case dlgType==4` | `dispatch_simple_selection` :1037/:1047 | ✅ | — |
| G6 | 输入框应答 | `CUtilDlgEx::SetNPC` | `dispatch_input_field` AskBoxText/string/GetNumber :1699-1778 | ✅ | 销案 S2 |
| G7 | AskAvatar 确认/取消 | `UtilDlgEx_Avatar` | `dispatch_avatar_ok/cancel` :1901/:1909 | ✅ | — |
| G8 | 商店买/卖/充值/离开 | `CAdminShopDlg::SetNPC` | `NpcShopActionPacket` 四模式 | ✅ | 销案 S1 |
| G9 | 空按钮初始 DISABLED | — | `BT_HYPER/BT_GUILDSKILL/BT_RIDE` 空 | ✅ | 不臆造功能 |
| G10 | 宏收起显式 deactivate（交叉） | — | `set_macro` `mex->deactivate()` | ✅ | 继承第八面 |
| G11 | ESC 关指引（交叉） | `CWndSkillGuide::OnKey @ 0x8aa4e4` | 第八面 UISkillBook | ✅ | 交叉印证 |
| G12 | 拖技能到宏（交叉） | `CDraggableSkill::MapMacro` | 第八面 | ✅ | 交叉印证 |
| G13 | SoftKey 快捷 | — | `r143/r172/r787` `!`/`%`/`N` | ✅ | 销案 S1/S2/S3 |
| G14 | 真机 c2s 197 length>6 / 双开观感 | `CMovePath::Flush` 字节待核 | `npc-action-move-pending.md:46-47` | ⏳ | P2 遗留 L1/L2 |

## §4 十条硬发现

1. `71/71 by bytes`：1-EA `0x6d065b ::_GCNpc` 双端 32B 逐字相同 `[86,139,241,232,28,0,0,0,246,68,36,8,1,116,14,141,70,248,80,185,0,11,191,0,232,117,43,211,255,141,70,248]`，九面同二进制。
2. `CNpc::GenerateMovePath @ 0x6d3273` 是 083 唯一 c2s 197 一手，字段序 `Encode4(oid)+Encode1(action)+Encode1(chatIdx)+Flush+SendPacket`。
3. `CUserLocal::TalkToNpc @ 0x95fe9e` 的 58 带 `Encode2(x)+Encode2(y)`，且「有任务列表 → `ShowQuestList` 本地开窗、不发 58」是当前端没有的分支。
4. `CNpcPool::OnPacket @ 0x6d9734` 八条 s2c 分发与 `PacketSwitch.cpp` 逐号双向对齐（81/82/257/258/259/260..262/263）。
5. `CNpcPool::OnNpcChangeController` `Decode1(bool)+Decode4(oid)` → `SetLocalNpc/SetRemoteNpc` 双分支，与 `MapNpcs.cpp` remove-then-add 对齐。
6. 083 SP_ 节点族（MapleTV/QuestIcon/UtilDlgEx List0-3/ScriptInfo/QuestInfo/NPCSummon|DCMark/Npc.img）给 F 维 12 项提供节点名，但像素仍待 IMG。
7. `CUtilDlgEx::SetNPC @ 0x9a64fc` 让当前端 `UINpcTalk.h` 注释里的裸 `sub_9A64FC` 在 083 侧具名闭合。
8. 095 线 `0x671590/0x666e20/0x668160/op241` 被逐处标为历史注记；本面 083 权威 `0x6d3273/op197`。
9. `angel-npc-decompile.json` 的 `functions` 是 dict（70 键 / 逻辑 71，`FindNpc` 双重载合并）；`code_of` 多态归一后 facts 无残留 JSON 串。
10. 判定 ✅72/⚠️12/⏳14 由脚本逐条重数；无 ❌ / ⊘；F 维 12 项全 ⏳ 是门禁诚实，不是漏做。

## §5 四态

| 态 | 数量 | 摘要 |
| --- | --- | --- |
| 销案 | 8 组 | S1 Talk/Shop SoftKey、S2 NpcTalk/Shop、S3 最近 NPC、S4 drain/rem、S5 s8-npc 4 包、S6 portal、S7 NPCAnimationHandler、S8 r15 tab_plate 交叉 |
| 推翻 | 1 组 | O1 095 线 `Maplestory95.i64` 依据（证据来源，非实现）；换 083 `0x6d3273` |
| 新增 | 8 组 | N1 OnPacket 八 case、N2 58 包体/ShowQuestList 分支、N3 攒 path 循环、N4 授控双分支、N5 SP_ 族、N6 三大函数具名、N7 71/71 by bytes、N8 095↔083 对照表 |
| 遗留 | 5 组 | L1 真机 197 hex、L2 双开观感、L3 完整 VecCtrl、L4 IMG/F 维 12、L5 ShowQuestList 独立任务窗（P3 ⑪） |

详见 `notes/r05-npc-four-state-audit.md`。

## §6 决策点 D1–D14（推荐默认 + 依据）

| # | 决策点 | 推荐默认 | 依据 |
| --- | --- | --- | --- |
| D1 | c2s 58 是否补 `Encode2(x)+Encode2(y)` | **不补**，保持 s9 `write_int(oid)` + 197 对话姿态 | 改回 083 包体会断 s9 `NPCAnimationHandler length==6` 契约 |
| D2 | 60/61/162 本面未抽到 `COutPacket` 是否判缺 | **不判缺**，记 Angel 覆盖差异 | 这些走 `CUtilDlgEx`/`CUIShop` 独立链，不在 71 函数字面量内 |
| D3 | 是否补 083 `ShowQuestList` 本地任务列表窗 | **不补**，P3 ⑪ 任务面统一裁决 | 当前端直发 58 与 s9 一致 |
| D4 | s2c 304/305/306 与 c2s/s2c NPC_ACTION 撞名 | **分列不判等** | 三套命名空间纪律 |
| D5 | 095 依据是否沿用 | **不沿用为 083 一手**，仅历史注记 | `0x671590 ≠ 0x6d3273`、`v95 241 ≠ s9 197` |
| D6 | 是否补完整 `CVecCtrlNpc` 攒多节 | **保留单节 attr=0 简化** | s9 `length-9` 契约已满足，完整物理属 P3 |
| D7 | 是否接 `SP_4419/4420` NPC 召唤/回归特效 | **暂不接** | 需 IMG 帧序列 + 真机验证 |
| D8 | AskAvatar / Quiz / Pet 缺窗体资源 | **保留 `end_talk` 降级** | 不 emplace 隐形窗，否则脚本一直等回复 |
| D9 | 是否新增当前端缺的 083 发包 | **不新增** | s9 优先，083 只作标尺 |
| D10 | 是否补互斥门禁 / 改 `OutPacket.h` | **不改** | opcode-matrix GATE OK，双源一致 |
| D11 | IMG 恢复前 F 维 12 项 | **保持 ⏳** | 不伪造 ✅ |
| D12 | 71/71 by bytes 是否推翻既有「同二进制」结论 | **不推翻，正向闭合** | 字节级证据更强 |
| D13 | REA r17 subject 是否并入 13339 一手 | **不并入**，单列 | subject = `BeiDou-ClientV17.7/BeiDou.exe` |
| D14 | 是否在本面建提交 / 改源码 | **不建、不改** | P0 纯文档；原子提交须用户授权 |

## §7 一次性 Plan（P0–P3）

| 阶段 | 内容 | 状态 |
| --- | --- | --- |
| **P0** | 三联只读取证（符号/反编译/crosscheck/byteaudit/facts）+ 四件终稿（r01/r03/r05/README）+ `gen_opcode_matrix.py --check` + 98 条脚本重数 + present | ✅ 本轮闭环 |
| **P1** | 用户裁决 D1–D14 后方动源码；默认推荐全「保持 s9、不补 083 发包、不改协议」 | ⏸ 待裁决 |
| **P2** | orange-wz 恢复 → F 维 12 项像素回填 + `vision_pixel_diff` + `build_smoke.bat`；真机 c2s 197 length>6 hex + 双开观感（L1/L2/L4） | ⏸ 待 IMG / 真机 |
| **P3** | ⑩ 地图 / ⑪ 任务（含 `ShowQuestList` L5）/ ⑫ 掉落 续作；完整 VecCtrl（L3、D6） | ⏸ 下一面 |

## §8 限制与未知

- IMG 门禁阻塞（橙色 wz 未连接）→ F 维 12 项 ⏳，见 `r03`。
- 真机 c2s 197 `length>6` 十六进制样例、双开观感、完整 VecCtrl 多节、下行完整 elem 插值未关闭（L1–L3）。
- 083 `ShowQuestList @ 0x6d3392 (27614 B)` 的独立任务列表 UI 当前端无对应，挂 P3 ⑪。
- `CNpcPool::FindNpc` 双重载在反编译 JSON 合并为 1 键（物理 70 / 逻辑 71）。
- `angel-npc-decompile.json` 中部分大函数（`ShowQuestList` 27614 B / `SetMapleTVMessage` 17313 B）伪代码被 facts 截断为关键信号行，全量在 evidence JSON 内。
- 095 禁引；REA r17 subject 单列；全程只读未改源码/协议、未建提交。

## §9 证据索引与复现

### §9.1 文件清单

```
docs/features/npc-angel-calibration-2026-11/
├── README.md                                  本文件
├── notes/r01-angel-native-npc-facts.md        083 一手事实基线
├── notes/r03-img-blocked-fallback.md          IMG 阻塞 + F 维降级
├── notes/r05-npc-four-state-audit.md          既有笔记四态
└── evidence/
    ├── angel-npc-symbol-inventory.json        71 funcs / 15 classes
    ├── angel-npc-decompile.json               71/71 全量 Hex-Rays
    ├── angel-beidou-npc-ea-crosscheck.json    70/71 EA 一致
    ├── angel-beidou-npc-ea-bytelevel-audit.json  71/71 by bytes + 九面表
    └── angel-npc-facts.md                     922 行 / 2 opcode 字面量
```

### §9.2 复现命令与验收信号

```bash
PYTHONIOENCODING=utf-8 python tools/ida_angel_npc_ea.py           # 71 funcs / 15 classes
PYTHONIOENCODING=utf-8 python tools/ida_angel_npc_decompile.py    # DONE 71/71 ok
PYTHONIOENCODING=utf-8 python tools/ida_angel_npc_crosscheck.py   # 70/71 EAs identical, missing_eas=["0x6d065b"]
PYTHONIOENCODING=utf-8 python tools/ida_angel_npc_byteaudit.py    # VERDICT: 71/71 by bytes
PYTHONIOENCODING=utf-8 python tools/ida_angel_npc_facts.py        # WROTE ... (922 lines, 34201 B, 2 opcode literals)
python tools/gen_opcode_matrix.py --check                          # GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)
```

### §9.3 98 条判定重数（脚本，不手写）

```bash
PYTHONIOENCODING=utf-8 python -c "
import re,io
t=io.open('docs/features/npc-angel-calibration-2026-11/README.md',encoding='utf-8').read()
rows=re.findall(r'^\| ([A-G]\d+) \|.*\|\s*(✅|⚠️|⏳|❌|⊘)\s*\|', t, re.M)
from collections import Counter
c=Counter(v for _,v in rows)
print('TOTAL_ROWS',len(rows)); print('OVERALL',dict(c))
for d in 'ABCDEFG':
    print(d, dict(Counter(v for k,v in rows if k.startswith(d))))
"
# 期望: TOTAL_ROWS 98 / OVERALL {'✅':72,'⚠️':12,'⏳':14}
```

## §10 性质标注

本目录是**校准 / 对照 / 决策点 / Plan 文档**，不是实现提交。P0 纯文档闭环；P1 需用户裁决 D1–D14；P2 等 orange-wz + 真机；P3 下一功能面。全程只读，未改游戏源码或协议，未建 git 提交。
