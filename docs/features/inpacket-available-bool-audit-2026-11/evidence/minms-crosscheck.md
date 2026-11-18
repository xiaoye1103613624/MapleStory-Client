# MinMS 收包侧对照证据（minms-receiver 子任务回报，纯只读）

## 0. 根因跨端确认（两端 InPacket 原文）
| 项 | 本端 | MinMS | 出处 |
| --- | --- | --- | --- |
| available() 返回类型 | bool | bool | MapleStory-Client/Net/InPacket.h:37、MinMS/.../InPacket.h:37 |
| available() 实现 | return length()>0 | return length()>0 | 本端 InPacket.cpp:30-33、MinMS InPacket.cpp:32-35 |
| length() 返回 | size_t (top-pos) | size_t (top-pos) | 本端 InPacket.cpp:35-38 / InPacket.h:39 |

结论：available()>=N(N≥2) 恒假；MinMS 因此全面改 length()>=N。本端已具备 length()，机械替换在编译/语义上成立。

## 1. 迁移状态：本端处于"混用未完成"
- CashShopHandlers.cpp：76 处门控**全部已是 length()>=N**（:236-247、:268-307）——已迁移完成。
- MapObjectHandlers.cpp：已有 7 处 length() 判定（:57/:75/:88/:410/:583/:589/:671），但仍有 88 处 available>=N。
- 结论：这是未完成的人工迁移，不是新需求。

## 2. 字段布局对照（三类）
### 一致（改 length() 后对齐）
- SpawnMist：本端 :1287-1297 vs MinMS :1503-1516，序列/宽度相同（oid/type/owner/skill int×4 + level byte + delay short + 矩形4int）。
- DamagePlayer：本端 :1058-1062 vs MinMS :953-964，skill 均 int8、-3 多读 int。本端 `skill==-3 && available()>=4`(:1060) 双重恒假致 damage 错位。
- SkillEffect/CancelSkillEffect：本端 :1016-1042 vs MinMS :732-757，字段一致。
- quest progress skip5：本端 MessagingHandlers :205-213 vs MinMS :679-687，本端注释已诊断 pad(5) 但门控写成 available>=5 恒假。

### 版本差（门控可改，字段序列勿照搬）
- DamageMonster：本端 6 字段（多 skin int，:1459），MinMS 5 字段（:1267-1271）。
- SpawnChar：level 本端 short(2B :314) vs MinMS byte(1B :219)。
- GiveForeignBuff：本端 :1078/:1086-1104 用 popcount(first)+popcount(second)，MinMS :810-872 用具体 mask 常量；本端 `if(available>=16)` 恒假 → mask 恒 0 → 所有 buff 失效。
- 根因：本端基于 s9/北斗 PacketCreator.java，MinMS 基于 Cosmic PacketCreator，协议分支不同。

### 无法对应（MinMS 无此 handler）
- Chalkboard、FacialExpression、Alliance（MinMS grep 0 处）、Inventory(:268/:271/:272)、Trade(:179/181/192/240/242/251/411/413)、WhisperResult（handler 拆分不同）。

## 3. MinMS 值得借鉴的 19 处组合门控写法（本端仅 DamagePlayer:1060 有 1 处且写错）
- `effect == -3 && length() >= 4`（MinMS MapObject:162，配 :166 effect>0&&>=3、:169 effect==15&&>=1）
- `length()==7 && inspect_short()==-2`（MinMS MapObject:152，值探测 fake mob，本端无此逻辑）
- `(firstmask & MASK)!=0 && length()>=10`（MinMS MapObject:824，配 :857 morph、:861 combo）
- `entry.stat==POISON && length()>=2`（MinMS MapObject:783）
- `msgtype==14 && length()>=4` 等 4 处（MinMS NpcInteraction:178/190/222/235）
- `for(... && length()>=N)`（MinMS MapObject:1612、Messaging:1251、PlayerHandlers:939/950、Setfield:795）
- `length()==sizeof(int64_t)`（MinMS Messaging:661，time vs string 分支，本端缺失）
- 核心模式：`length() >= 该分支实际需要的字节数`；可选字段由前字段决定时用 `值条件 && length()>=N`。

## 4. 未覆盖（如实标注）
- 未逐字段核对 WaveAShellHandlers.cpp（本端 200 处，MinMS 无此文件，无对照价值）。
- CashShopParser/Inventory/Trade 逐字节序列未逐字节比对（仅确认门控写法与有无对照）。
- opcode 数值未按数字逐一对齐（以 handler 函数名为锚点；如需精确 opcode 映射可续做）。
