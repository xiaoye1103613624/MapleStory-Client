# r07 · 核查：NpcTalk / 反应堆·脚本·广播·喇叭 / Tooltip（2026-11-12）

> 只读取证。Angel 13339 + orange-wz 128B + 本端源码 + s9 只读。不改业务代码。

## 1. 总判

**脚本 NPC 对话主路径与反应堆触碰/打击主链可闭环；喇叭收包与 Avatar 解析、道具喇叭 type8/多段 type10、装备 tip 的 cash 徽标与交易限制字段仍有明确缺口。**

---

## 2. 问题 1：NPC 对话框（脚本 + IMG）

### 结论表

| 维 | 判定 | 证据 |
| --- | --- | --- |
| 入站公共头 | ✅ | Angel `CScriptMan::OnScriptMessage @0x74660A`：Decode1 speakerType → Decode4 npc → Decode1 msgtype → Decode1 speaker；本端 `NpcInteractionHandlers.cpp:45-49` 同序 |
| msgtype 映射 0–10/12–14 | ✅ | Angel switch cases 0–10,12–14（无 11）；本端 `NpcInteractionHandlers.cpp:91-365` 全覆盖；缺壳 `end_talk` 防挂死 `:369-375` |
| AskYesNo vs AcceptDecline | ✅ | Angel case1 a7=0 / case12 a7=1；本端 case 1/12 同读 string |
| GetNumber 尾 pad | ✅ s9 偏置已吞 | 本端 `:142-143` 读 s9 `getNPCTalkNum` 尾 int；标注「IDA 不读」 |
| 出站 MoreTalk | ✅ | `NpcTalkMorePacket`（`NpcInteractionPackets.h:83+`）按 lastMsg 变长；s9 `NPCMoreTalkHandler` |
| 起手脚本 | ✅ | `MapNpcs.cpp:96-97` → `TalkToNPCPacket`（TALK_TO_NPC 58 + 顺带 NPC_ACTION）；s9 NPC 脚本入口 |
| IMG UtilDlgEx | ✅ | orange-wz：`UtilDlgEx/{t,c,s,bar,BtOK/Yes/No/Prev/Next,…}` 齐全；本端 `apply_dlg_skin` |
| 形态合并 | 🟡 | Angel 单体 `CUtilDlgEx`；本端 `UINpcTalk` + Quiz/Pet/Mirror 壳 — 功能齐、类非 1:1 |
| KEY_ANI 指引 | ✅ | Angel `#E/#I/#S/#K`→AnalyzeText→Draw→SetButtonBlink；本端 `apply_guide_blinks_from_text` `UINpcTalk.cpp:1233-1258` / `:1586-1595`；剥标签防字面 |
| 手指精灵画布 | 🟡 功能已收口 | 无独立 CT 手指对象接 NPC/传送点；不影响脚本主路径 |
| NpcTalk→107 | ⊘ 红线 | 保持 MoreTalk；107 走 QuestLog/Alarm |

**主路径影响**：Say/YesNo/Menu/Text/Number/Accept/Avatar/Box/Quiz/Pet/Mirror **不影响脚本对话主路径**。深度 UI 像素/AskPet gage 等位图预留不挡流程。

### Angel msgtype 现场（节选）

见 `evidence/verify-onScriptMessage.txt`：`OnScriptMessage` case 0→OnSay … 14→OnAskSlideMenu。

---

## 3. 问题 2：四子链

### A. 反应堆 Reactor — 🟡→主链 ✅ / 细节 🟡

| 环 | 状态 | 证据 |
| --- | --- | --- |
| c2s 触碰 | ✅ | `Stage::check_touch_reactors` `Stage.cpp:501-532` → `TouchingReactorPacket` oid+bool；s9 `TouchReactorHandler.java:36-47` → `ReactorScriptManager.touch/untouch` |
| c2s 打击 | ✅ 布局对齐 | `DamageReactorPacket` `GameplayPackets.h:278-285`：oid + point(2×short=charPos) + stance short + skip4 + skillid；s9 `ReactorHitHandler.java:38-46` 同序 |
| 打击调用 | 🟡 | `Combat.cpp:207` stance/skill 恒 0 — 一般可打，技能/姿态敏感脚本可能偏 |
| s2c hit/spawn/remove | ✅ | `HitReactorHandler`/`SpawnReactorHandler`/`RemoveReactorHandler` `MapObjectHandlers.cpp:903-950`；`PacketSwitch` 277/279/280 |
| 脚本 | ✅ | 触碰→服务端 JS reactor；打击→`reactor.hitReactor`→状态包回客户端 `trigger` |

### B. 脚本执行 — ✅ 主链 / 🟡 形态

| 入口 | 客户端触发 | 服务端 | 回包渲染 |
| --- | --- | --- | --- |
| NPC | `TalkToNPCPacket` `MapNpcs.cpp:97` | NPCScriptManager | `NPC_TALK`→`NpcDialogueHandler`→UINpcTalk/壳 |
| 反应堆 | Touch/Damage 上列 | ReactorScriptManager | 常走 NPC_TALK 或地图效果 |
| 传送脚本 | `CHANGE_MAP` / `CHANGE_MAP_SPECIAL` + portal type SCRIPTED* `MapPortals.cpp` | PortalScript | 换图 / 偶发对话 |
| 任务脚本 | `QuestActionPacket` action 1/2/4/5 `QuestPackets.h:15-20` | QuestScript | ScriptMessage / QuestInfo |
| 脚本道具 | `SCRIPTED_ITEM` `InventoryPackets.h:141` | ItemScript | 对话或效果 |

与问题 1 交叉：**渲染侧主链已闭环**。

### C. 频道广播 — ✅

| 环 | 状态 | 证据 |
| --- | --- | --- |
| c2s 换频 | ✅ | `ChangeChannelPacket` OutPacket 39；s9 `ChangeChannelHandler` |
| s2c | ✅ | `ChangeChannelHandler` `PlayerHandlers.cpp:47-67` → `LoginParser::parse_login`；可选 tip |
| 频道内公告 | ✅ | `SERVER_MESSAGE(68)` → `ServerMessageHandler` `MessagingHandlers.cpp:382-483` type0/1/4/5/6/7 |
| 消费 | ✅ | ChatBar SYSTEM/RED/YELLOW + StatusMessenger + ScrollNotice |

### D. 喇叭 Megaphone — 🟡（发 ✅ / 收部分 ❌）

| 子类 | 发 (c2s) | 收 (s2c) | 判定 |
| --- | --- | --- | --- |
| 普通 5070000 type2 | `UseMegaphonePacket` / `UIMegaphoneBasic`；s9 `UseCashItemHandler` case1 → `serverNotice(2,…)` | `ServerMessageHandler` else + `type==2`→FloatNotice `:476-477` | ✅ |
| 超级 type3 | s9 case2 → `serverNotice(3, channel, …)` | type==3 Chat+FloatNotice `:414-435` | ✅ |
| 道具 type8 | `UseItemMegaphonePacket` / `UIItemMegaphone`；s9 case6 → `itemMegaphone` **type=8** + item blob `PacketCreator.java:6267-6278` | **无 type==8 分支**；落入 else，item 信息 `drain` 丢弃 `:464-482` | ❌ 收端断链 |
| 三段 type0x0A | s9 case7 → `getMultiMegaphone` type=10 `PacketCreator.java:6506+` | 无专用解析，多行被 drain | ❌ |
| Avatar | `UseAvatarMegaphonePacket`；s9 → `SET_AVATAR_MEGAPHONE` int itemId+name+lines+look | `AvatarMegaphoneHandler` `WaveAShellHandlers.cpp:1018-1031` **先 read_string**，与 s9 先 `writeInt(itemId)` 错位 | ❌ 解析断链 |
| Art 5073/5077 | 开 `UIArtMegaphone` 发多行 | 收依赖 type3/多段 — 与上叠加 | 🟡 |

---

## 4. 问题 3：装备 / 道具 Tooltip

### 结论表

| 维 | 判定 | 证据 / 偏差 |
| --- | --- | --- |
| 底板样式 | ✅ / 🟡α | `TextTooltip.h:44-68` fill(79,79,119) α0.88 + 亮边(85,85,130)；Angel 程序描边口径一致；α 为实机观感偏离「实心」旧判 |
| IMG ToolTip/Equip | ✅ 资源在 | orange-wz：`Can/Cannot/Dot/Growth*/cash/mesos/WeaponCategory/…` |
| 需求块 Can/Cannot | ✅ | `EquipTooltip.cpp:197-228` / `:831-850` reqLEV/STR/DEX/INT/LUK/POP + reqok 双色 |
| 图标区 | ✅ | 程序灰底板 `iconbase_box`（弃坏 IconBase 白框）；`EquipTooltip.h:105-109` |
| 分隔线 | ✅ | 全宽白 1px `EquipTooltip.cpp:173` |
| 正文色 TOOLTIPBODY | ✅ | 奶油色；属性对比绿/红 `:530` |
| 稀有度名色 | ✅ | ORANGE/BLUE/VIOLET/YELLOW/WHITE/GREY `:642-662` |
| 对比装备 | ✅ | `show_compare` + `draw_preview`；配置 `ui.equip_compare` |
| 冒号中线排版 | ✅ / 偏离史 | `BodyLine` 中线冒号 `EquipTooltip.h:60-67`（用户实拍要求；曾偏离原生左对齐后又校正） |
| Dot 项目符 | ✅ 已接成员 | `Dot/0`；R28 后续已加 `dot` 成员 |
| **cash / mesos 徽标** | ❌ | IMG 有 `ToolTip/Equip/cash`、`mesos`；**EquipTooltip.cpp 无引用** |
| **装备不可交易等限制行** | ❌ / 🟡 | `ItemTooltip` 有 untradable/unique `:63-79`；**EquipTooltip 无交易限制行** |
| 潜能/火花 | 🟡 | 本端有 `potflag`；083 `SetToolTip_Equip @0x8e8252` 本轮未证潜能行是否 083 标准字段 — 不作「已对齐」 |
| 道具 ItemTooltip | 🟡 | 名/描述/品质橙字/灰底图标 ✅；无需求条（非装备合理）；颜色偏 WHITE 非 TOOLTIPBODY |
| 弹出逻辑 | ✅ | 背包悬停 `UIItemInventory.cpp:1594-1605` → `UI::show_equip/show_item` |
| Angel 类族 | 取证 | `SetToolTip_Equip @0x8e8252`、`ShowItemToolTip @0x8f5b20`、`ClearToolTip @0x8e6e23` — `evidence/verify-tooltip-ida.txt` |

**总判 Tooltip**：**装备主展示 🟡（核心字段/需求/对比齐，缺 cash 徽标与交易限制）；道具 tip 🟡（简版可用）。**

---

## 5. 非闭环项优先级

> **r08 已销案**：type8 / 0x0A / Avatar 包头、Equip cash+交易+inc 居中白字、ItemTip 色、DamageReactor stance/skill → 见 `r08-megaphone-tooltip-fix.md`。

| P | 项 | 说明 |
| --- | --- | --- |
| ~~P0~~ | ~~SERVER_MESSAGE type8~~ | ✅ r08 文案+Ch；道具图标浮层仍预留 |
| ~~P0~~ | ~~Avatar 包头~~ | ✅ r08 |
| ~~P1~~ | ~~type 0x0A~~ | ✅ r08 |
| ~~P1~~ | ~~Equip cash/交易~~ | ✅ r08 |
| ~~P2~~ | ~~DamageReactor stance/skill~~ | ✅ r08 |
| ~~P2~~ | ~~ItemTooltip TOOLTIPBODY~~ | ✅ r08 |
| P3 | 潜能行是否 083 | 证据不足先不扩 |
| P3 | type8 道具图标 / Avatar look 立绘 | 收包已稳，展示深度预留 |

红线保持：107 x/y、NpcTalk→107、远征面板、095。

---

## 6. 证据索引

| 路径 | 内容 |
| --- | --- |
| `evidence/verify-onScriptMessage.txt` | Angel OnScriptMessage 反编译 |
| `evidence/verify-tooltip-ida.txt` | CUIToolTip 符号表 |
| `evidence/p3-ida-tagmap.txt` | KEY_ANI `#E/#I/#S/#K` |
| `NpcInteractionHandlers.cpp:41-401` | 入站分支 |
| `UINpcTalk.cpp:1233+ / 1586+` | KEY_ANI |
| `Stage.cpp:501-532` / `MapObjectHandlers.cpp:903-950` | Reactor |
| `MessagingHandlers.cpp:382-483` | ServerMessage |
| `WaveAShellHandlers.cpp:1018-1031` | Avatar 收包 |
| `PacketCreator.java:6267+ / 6506+ / 1351+` | s9 喇叭发包 |
| `EquipTooltip.*` / `ItemTooltip.*` / `TextTooltip.h` | Tip 实现 |
| `npc-talk-utildlgex-align/README.md` | 既有 msgtype 闭环（本轮源码复核） |
| `ui-v83-window-restore/notes/r28-equip-tooltip-align.md` | Tip 几何史 |
