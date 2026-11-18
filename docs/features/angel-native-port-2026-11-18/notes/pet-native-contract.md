# 宠物发包原生契约（REA 反编译取证，2026-11-18 续作轮）

> 来源：`E:/MXD/BeiDou-Client-xy/REA反编译/095REA/`（不需 MCP，纯文本）。
> 权威表：`notes/port-095/opcode-map-v95-vs-s9.md` + `opcode-v95.md`；反编译正文：`evidence/_ms_probe/dec_send/*.json`。

## 一、核心纠正：宠物输入走聊天栏文本，不是工具窗/软键盘

用户当面纠正 + 反编译坐实：
- `CPet::ParseCommand(ZXString<char> sChat)` @0x6a3cc0 —— 输入是聊天字符串。
- `CPet::DoAction(int nType, int nAction, Ztl_bstr_t sChat, ...)` @0x6a2340 —— 同样带聊天文本。
- `UISoftKey` 本端只剩 PIC 二级密码（UICharSelect.cpp:1264/1338/1729/1732）；UI.cpp:317 注明裸字母→SoftKey 是已移除的历史过度实现。
- 结论：`UIUtilDlgExPet` 的 C/T/E/P 空分支（UIMiscShells.cpp:6496-6507）是错误设计残留，不应往里填；正确入口是**聊天栏命令派发**。

## 二、原生函数 → opcode 映射（095 原生 vs s9）

| 原生函数 | 095 opcode | s9 opcode | 本端 packet 类 |
|---|---|---|---|
| `CPet::ParseCommand` | 201 | PET_CHAT 168 | PetPackets.h:73-85 |
| `CPet::DoAction` | 200 | PET_COMMAND 169 | PetPackets.h:89-100 |
| `CWvsContext::SendStatChangeItemUseRequestByPetQ` | 203 | PET_AUTO_POT 171 | PetPackets.h:119-131 |
| `CPet::SendUpdateExceptionListRequest` | 204 | PET_EXCLUDE_ITEMS 172 | PetPackets.h:135-148 |
| `CVecCtrlPet::EndUpdateActive` | 199 | MOVE_PET 167 | 已接 |

## 三、wire 正文（s9 口径已核对，本端 packet 类正确，勿按 095 改）

- PET_CHAT(168)：`int petId + int(0) + byte(0) + byte act + string text`
  - 原生 095: `COutPacket(168)`；s9 `PetChatHandler.java:38-42 readInt/readInt/readByte/readByte/readString` 一致。
- PET_COMMAND(169)：`int petId + int(0) + byte(0) + byte command`；s9 `PetCommandHandler.java:39-49` 一致（command 经 `PetDataFactory.getPetCommand(itemid, command)` 校验）。
- PET_AUTO_POT(171)：`byte(0) + long(0) + int(0) + short slot + int itemid`；s9 `PetAutoPotHandler.java:36-40` 完全一致。
  - 原生 095 `SendStatChangeItemUseRequestByPetQ@0x9de400` wire 是 8B SN+byte+int time+short+int（版本差），**s9 用上面的，不要照 095**。
- PET_EXCLUDE_ITEMS(172)：本端 `int uid + int(0) + byte count + N×int itemid`；原生 095 `SendUpdateExceptionListRequest@0x6a0dd0` 是 8B SN + byte count + N×int。
  - s9 服务端无 `PetExceptionListHandler.java`（find 0 命中，待复核是否叫别的名字）——**接线前必须先确认 s9 收包方，否则死包**。

## 四、本端可用数据源

- pet uid：`Character/Player.cpp:349-356` 遍历 `get_pet(i)`，`PetLook::get_uniqueid()` 已用于 MovePetPacket；data source = `SpawnPetHandler`（MapObjectHandlers.cpp:514 add_pet）。
- 聊天栏输入派发：`IO/UITypes/UIChatBar.cpp:2687 GeneralChatPacket(message,true).dispatch()` 附近，宠物命令应在此分流（参考 `CField::SendChatMsgSlash` 斜杠命令族）。
- **缺口**：本端无 `PetDataFactory`/宠物命令交互表（grep `PetCommand` 无数据表）；原生 `CPet::ParseCommand` 依赖 `CPetTemplate::INTERACTION` 表。无此表则无法把聊天文本翻译成 command/act。

## 五、s9 wire 已逐包核对（2026-11-18 续作轮，本端 packet 类全部正确，勿改）

- PET_CHAT(168)：s9 `PetChatHandler.java:38-42` = readInt(petId)+readInt(0)+readByte(0)+readByte(act)+readString → 本端 PetPackets.h:73-85 一致。
- PET_COMMAND(169)：s9 `PetCommandHandler.java:39-49` = readInt+readInt+readByte+readByte(command)，服务端也会用 `PetDataFactory.getPetCommand(itemid,command)` 二次校验 → 本端 :89-100 一致。
- PET_AUTO_POT(171)：s9 `PetAutoPotHandler.java:36-40` = readByte+readLong+readInt+readShort(slot)+readInt(itemid) → 本端 :119-131 一致。
- PET_EXCLUDE_ITEMS(172)：s9 `PetExcludeItemsHandler.java:42-59` = readInt(petId)+skip(4 timestamp)+readByte(amount)+N×readInt(itemid) → 本端 :135-148 一致（第二个 write_int(0) 即 timestamp）。
- 结论：4 包 s9 handler 全存在且注册，wire 全对；本端缺的只是"何时发什么"的交互逻辑，不是包。

## 六、WZ interact 命令表已用 orange-wz 实测取得（关键突破）

- 数据位置：`E:/MXD/BeiDou-Client-xy/Data/Item/Pet/<petid>.img/interact`。用 orange-wz(10012) HTTP API（128B key `tools/_gms_key_128.b64` / iv `TSPHKw==` / exclusive:false）实测 5000000.img 成功。
- 结构（= 原生 `CPetTemplate::INTERACTION`，即 `CPet::ParseCommand` 的数据源）：
  - `interact/N/command` STRING = 命令文本（如 c1/c2/c3）
  - `interact/N/inc` INT = 亲密度增量；`prob` INT = 概率；`l0`/`l1` INT = 亲密度等级区间
  - `interact/N/success/M/act` STRING = 成功动作（rest0 等）+ 子键 0/1/... = 成功对话 key
  - `interact/N/fail/M/act` STRING = 失败动作（stand1 等）+ 子键 = 失败对话 key
- 取证脚本（可复用）：`notes/_owz_pet_query.py`（load→list/search→unload_all）、`notes/_owz_pet_tree.py`（get_node_tree_json）。
- 结论：**A 层证据链齐了**——命令表(WZ)+pet uid(PetLook::get_uniqueid, Player.cpp:350)+wire(s9 handler 已核)+输入路径(聊天栏 UIChatBar.cpp:2668-2687 命令派发点，GeneralChat 前)。

## 六·补  opcode 语义澄清（蓝军复核，勿被函数名误导）

095 反编译正文（dec_send 实测）：
- `CPet::ParseCommand@0x6a3cc0` → `COutPacket(nType:201)`，`EncodeBuffer(8B SN)+Encode1(bCommandWithName)+Encode1(command序号)`。
- `CPet::DoAction@0x6a2340` → `COutPacket(nType:200)`，`8B SN+Encode4(time)+Encode1(type)+Encode1(act)`。
- 即 **095 原生：200=DoAction(act 动作)，201=ParseCommand(命令序号)**。

但 s9 服务端语义与 095 不同（版本差，以 s9 为准）：
- s9 `PetCommandHandler(169)` = readInt(petId)+int(0)+byte(0)+byte(command)，用 `PetDataFactory.getPetCommand(petid, command)` 读 `interact/{N}/prob|inc` 判概率 → **这就是"聊天命令文本→WZ interact 序号 N→发包"那条**。上轮 UIChatBar 发 PET_COMMAND(169) 正确，勿改。
- s9 `PetChatHandler(168)` = readInt+int+byte+byte(act)+readString(text)，比 095 ParseCommand **多一个 text 字段**（s9 扩展），用于"宠物说话+气泡"。
- 结论：函数名↔opcode 在 095/s9 间对调，判定一律以 **s9 handler wire** 为准，不按 095 函数名猜。

## 六·补2  IDA MCP 打通后的原生调用链实证（2026-11-18，第4轮）

IDA MCP 在 127.0.0.1:13337（.rea/ida-mcp.json attached 模式）一直在线，上轮只扫 10012-10030 漏掉=失职。本轮连上确认 IDB=`E:\MXD\反编译IDA\v83\`Angel.i64`（module `Angel.exe`，hexrays ready），用 xrefs_to/decompile/disasm 取得原生宠物命令调用链：

- `CField::SendChatMsgSlash@0x52c40c`（斜杠命令总入口）→ 0x52ed54 循环 3 只宠物 → `sub_52F093` 取 CPet* → `CPet::ChatCommand@0x7050be`（xref 0x52ed75，传入 var_10=StringPool GetString(0x110) 的固定命令文本，经 CCurseProcess 脏词过滤）。
- `CPet::ChatCommand@0x7050be` 伪代码：`if (GetLevel(pet)>=15) DoAction(nType=2, nAction=0, sChat=文本, bSend=1, bChatBalloon, time)` —— **是"宠物等级≥15 时复述斜杠命令文本说话"，不是 interact 表匹配**。
- `CPet::ParseCommand@0x6a3cc0`（interact 表匹配那条）在 Angel IDB **零 xref**（xrefs_to 返回空）——可能是废弃/动态调用函数。
- `CPet::DoAction@0x7055e2` 的调用者仅 4 个：OnAction/OnActionCommand/sub_704A92（全是 s2c 回包）+ ChatCommand。**没有任何"本地 UI 选 interact 命令→DoAction"入口**。

与 s9 对照的结论：
- 083 原生活着的是"斜杠命令→宠物复述说话"（PET_CHAT/DoAction type=2），由 StringPool 固定命令(0x110)触发；interact 表匹配(ParseCommand)在原生未接玩家输入。
- s9 服务端则有完整 `PetCommandHandler(169)` 读 `interact/{N}/prob|inc` 判概率——这是 s9 扩展通道，上轮 UIChatBar 的"聊天文本→interact 表→PET_COMMAND(169)"在 s9 语义下成立，保留。
- 宠物说话通道：s9 `PetChatHandler(168)` 广播时 act 直接透传、本端 s2c `speak(text)` 只画气泡不用 act，故 act 值无表现影响；但原生触发命令文本是 StringPool 0x110 固定值，本端无法从现有证据确定是哪个斜杠命令，且与本端已有 /g /b /shop 等命令冲突风险高 → **列明确预留，不硬做**。
- PET_AUTO_POT(171)/PET_EXCLUDE(172)：IDA 具名函数仅有发包层（SendStatChangeItemUseRequestByPetQ@0x9de400 / SendUpdateExceptionListRequest@0x6a0dd0），无 UI 点击层具名入口（CUtilDlgEx 有 GetInputStr_Result@0x519e3a 暗示工具窗有物理键盘输入框，但具体触发 UI 未具名）→ 仍缺原生 UI 入口语义，不硬接。

## 七、结论与下一步（goal 下轮输入）

- 宠物聊天/指令不是"接个 dispatch"，是**完整 feature**：新建 PetData(WZ interact 加载器) + UIChatBar 命令分流 + 亲密度/概率本地判定 + PetLook 动作播放(rest0/stand1 字符串→stance) + 4 opcode 发包。
- 原生双端都判定：客户端即时动画，服务端权威同步（s9 PetCommandHandler/PetChatHandler 均本地再判）。
- 未决细节：PetLook 当前只有 set_stance(uint8_t/Stance)，act 字符串（rest0/stand1）→ stance 映射需查 BodyDrawInfo/Stance.cpp 确认 pet 侧支持到哪一步；宠物对话 key（c1_s1 等）→ 本地 String 文本需核。
- 建议下轮：先建 PetData 加载器 + 命令分流最小闭环（PET_COMMAND），构建验证后再补 PET_CHAT/AUTO_POT/EXCLUDE；不碰 UIUtilDlgExPet 空分支（错误设计残留）。
- 部署注意：live exe SHA 会漂移（登录器回滚），2026-11-18 00:42 归档 b76e5082… 已被并发覆盖为 eeaf53ac…，源码改动已坐实、下次构建重新生成。
