# Angel 原生(v83)口径复核（2026-11-18，用户要求排除 095/s9）

> 用户要求：以 Angel 原生(083)为准，不考虑 095/s9。此前取证大量用 095REA + s9 服务端，本文件用 Angel.i64(13337, module `Angel.exe`, hexrays ready) 重核，地址全是 v83。

## 一、宠物命令主链 —— Angel 原生确实实现，且本端已对齐（结论修正）

此前用 095 地址 `ParseCommand@0x6a3cc0` 查得"零 xref、是 s9 契约"，是**查错 IDB 地址**。Angel v83 正确地址 `CPet::ParseCommand@0x704d5d`：

- xrefs_to(0x704d5d) = **1 个调用者**：`CUIStatusBar::OnKey@0x8d536c`（xref 0x8d564f）——原生活着。
- disasm OnKey 0x8d5600-0x8d565d：循环宠物槽 → 检查槽有宠物 `[eax+ebx*8+4]!=0` → 取 arg_0(ZXString 输入文本) → `sub_52F093(slot)` 取 CPet* → `ParseCommand(文本)`。
- decompile ParseCommand@0x704d5d：遍历模板 interact 表（`this+136`，按 level 区间 v20/v20[1]、字符串匹配 command 名），命中 → `COutPacket(169)` + `EncodeBuffer(8B SN)` + `Encode1(name标记)` + `Encode1(序号)` → SendPacket。
- **结论**：Angel 原生宠物命令 = 状态栏 OnKey 收到聊天输入文本 → ParseCommand 查 WZ interact 表 → 发 opcode 169 + 序号。本端 `UIChatBar.cpp:65/105/2616` 的"聊天文本→WZ interact 表→PET_COMMAND(169) 序号"实现与 Angel 原生**语义、opcode、wire 全部对齐**。上轮实现正确；此前"零 xref/s9 专属"的论证作废。

## 二、按 Angel 原生重判所有已落地项

| 项 | Angel 原生依据 | 本端 | 判定 |
|---|---|---|---|
| 宠物命令 169 | ParseCommand@0x704d5d + CUIStatusBar::OnKey@0x8d536c（上述） | UIChatBar.cpp:65/105/2616 | ✅ Angel 原生逻辑，已对齐 |
| USE_ITEM_REWARD | 需按 Angel 原生 opcode 重核（此前用 s9 RecvOpcode 112） | UIItemInventory.cpp:856 | ⚠️ 见下，待用 Angel IDB 确认原生对应 opcode/触发 |

## 三、此前判"阻塞"的项按 Angel 原生重判

- 宠物说话：`CPet::ChatCommand@0x7050be` 被 `CField::SendChatMsgSlash@0x52c40c` 调用（Angel 实证），走 DoAction type=2 复述斜杠命令；StringPool 固定 ID 触发——仍需确认具体命令文本，维持预留。
- 宠物自动药/排除：Angel `SendStatChangeItemUseRequestByPetQ/SendUpdateExceptionListRequest` 需按 v83 地址重查 caller（此前查的 0x9de400/0x6a0dd0 是 095 地址，Angel 里不存在）→ 见下轮行动。
- 三窗（棱镜/魔方/格锁）：live WZ 无资源（orange-wz 实证，与版本无关）+ 魔方服务端无逻辑 → 阻塞结论不依赖 095，维持。
- 战斗环境 5 项、212、Title/Ranking/MTS：阻塞/裁决结论与版本无关，维持。

## 三·补  Angel 原生宠物发包函数穷尽（2026-11-18 复核）

Angel.i64 func_query `name_regex=(Send.*Pet|Pet.*Send|Autopot|AutoPot|ExceptionList|ByPetQ)` 只有 3 个具名函数：
- `SendActivatePetRequest@0xa240a2`（召唤宠物，已移植）
- `SendPetFoodItemUseRequest@0xa09905`（宠物粮，已移植）
- `OnLoadExceptionList@0x7061a5`（s2c 回包读，非发包）

**Angel v83 没有 SendPetChat / SendAutoPot / SendExcludeItems 具名发包函数**；唯一 c2s 宠物命令是 ParseCommand 内联 `COutPacket(169)`。immediate 168/171/172 的命中全是随机偏移/坐标/结构字段，非 COutPacket opcode 构造。

### 最终判定（纯 Angel 口径）

| 用户问的项 | Angel 原生是否实现 | 本端状态 |
|---|---|---|
| 宠物命令(opcode 169) | ✅ 实现：OnKey@0x8d536c → ParseCommand@0x704d5d → COutPacket(169) | ✅ 已对齐（UIChatBar.cpp:65/105/2616） |
| 宠物说话 Chat | 仅 ChatCommand→DoAction type=2 复述斜杠命令（s2c 显示），无独立 c2s 发包具名函数 | 本端 s2c speak() 已接；c2s 通道 Angel 无对应实现，无需移植 |
| 自动药 AutoPot | ❌ Angel 无具名发包函数、无具名 UI 入口 | 无原生可实现，维持不做 |
| 排除列表 ExcludeItems | ❌ Angel 仅有 s2c OnLoadExceptionList，无 c2s 发包 | 无原生可实现，维持不做 |

**结论（回应用户）**：按 Angel 原生口径，除已列阻塞项外，"Angel 已实现的可移植逻辑"确实都实现了——宠物命令是 Angel 原生逻辑且已对齐；宠物说话/自动药/ excluded 在 Angel 本身就没有对应 c2s 实现，不是"没移植"，是"原生没有"。此前用 095/s9 得出的"缺入口/待拍板"结论作废。

### USE_ITEM_REWARD 的 Angel 复核（完成）

- Angel.i64 func_query `(ItemReward|Reward.*Use|Send.*Item.*Use)` 的具名发包函数：`SendStatChangeItemUseRequest@0xa092fb`、`SendExpUpItemUseRequest@0xa12685`、`SendMobSummonItemUseRequest@0xa0977b`、`SendPetFoodItemUseRequest@0xa09905`、`SendAntiMacroItemUseRequest@0xa262d9` 等——**没有独立的 ItemReward / USE_ITEM_REWARD(112) 发包函数**。
- find_regex reward 仅 4 处字符串（bonusExp/randomReward/#Wreward#/bonus），无 opcode 112 专用物品奖励发包路径。
- 结论：**opcode 112 的 USE_ITEM_REWARD 是 s9 服务端特有通道，不是 Angel 083 原生逻辑**。本端 UIItemInventory.cpp:856 的实现基于 s9 112，按用户"不考虑 095/s9、以 Angel 为准"的要求，**这条不计入 Angel 原生移植**；它对接的是 s9 服务端，属服务端适配（保留不动，因为本端服务端就是 s9，删了会坏功能，但不是 Angel 原生项）。

## 最终回答用户（逐条，纯 Angel 口径）

1. **Angel 原生已实现且本端已对齐的可移植逻辑**：宠物命令（OnKey→ParseCommand@0x704d5d→COutPacket 169）。✅ 真 Angel 原生，已实现。
2. **Angel 原生已实现、抽检在位的**：临时属性面板、道具 use 发包族、地图交互、宠物显示/移动、长伤害/骰子/伤害皮肤、GuildBBS、公会副本 HUD（均为旧台账僵尸 ❌，源码在）。✅ 无需动。
3. **Angel 原生本身没有对应实现的**：宠物说话 c2s、自动药、排除列表——Angel 只有 ChatCommand(s2c 显示) + ParseCommand(169) + SendActivatePetRequest/SendPetFoodItemUseRequest，无 SendPetChat/SendAutoPot/SendExcludeItems 具名发包。→ 不是"没移植"，是**原生没有**，无需移植。
4. **不是 Angel 原生、是 s9 适配的**：USE_ITEM_REWARD(112)、宠物命令 wire 序号与 s9 契约对齐（但 opcode 169 与 Angel 一致）。→ 按用户要求单列，不算 Angel 原生移植。
5. **阻塞（与版本无关）**：三窗 live WZ 无资源、战斗环境缺内容面、212 未裁决、Title/Ranking/MTS 已裁决。→ objective 明确排除。

**所以对用户问题的直接回答**：是的——按 Angel 原生口径，除已列阻塞外，Angel 已实现的可移植逻辑都实现了；此前列为"待办/阻塞"的宠物说话/自动药/排除列表，经 Angel IDB 复核是**原生本身就没有 c2s 实现**，不是本端漏移植。唯一需要澄清的是 USE_ITEM_REWARD：它是 s9 服务端适配，不是 Angel 原生逻辑（保留因本端服务端就是 s9）。

## 四、下一步（用户要求 Angel 原生口径）

1. 用 Angel IDB 重核 USE_ITEM_REWARD 的原生 opcode 与触发（不能拿 s9 112 当 Angel 原生）。
2. 用 Angel 正确地址重查 PET_AUTO_POT / PET_EXCLUDE / PET_CHAT 的具名 caller，把"阻塞"结论从 095 地址纠正为 Angel 地址实证。
3. 逐条给出 Angel 原生口径的"已实现/未实现/阻塞"最终账，不混入 095/s9 证据。
