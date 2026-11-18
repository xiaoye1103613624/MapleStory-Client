# 服务端发包侧裁决（server-sender 子任务回报 + lead 抽验，纯只读）

## 0. 陷阱先行：客户端注释里的行号已全部漂移
客户端 handler 注释内嵌的 `PacketCreator.java:行号` 是历史快照，服务端源码迭代后**已失效**。抽验实录：

| 客户端注释声称 | 真实方法体行号（方法名定位） | 注释所指行号实际内容 |
| --- | --- | --- |
| giveForeignBuff :3040-3048 | `giveForeignBuff:3148-3157` | 3040 实为 `OutPacket.create(UPDATE_QUEST_INFO)` |
| quest skip5 :2947-2962 | `updateQuest:3071-3085` | 2947-2955 是 giveBuff 的 javadoc + hex dump |
| spawnMist :4074-4088 | `spawnMist:4227-4240` | 4074 实为 party operation |
| applyMonsterStatus :4009-4036 | `applyMonsterStatus:4162-4188` | — |
| cancelMonsterStatus :4038-4045 | `cancelMonsterStatus:4191-4197` | — |
| spawnSummon :1167-1181 | `spawnSummon:1204-1217` | — |
| spawnMonsterInternal :1514-1569 | `spawnMonsterInternal:1551-1606` | — |

**结论**：只能按方法名搜索（如 `public static Packet <方法名>`）定位真实发包函数，以方法体 writeInt/writeLong/writeShort 序列为准，注释行号只能当地图用。server-sender 全程用方法名定位，未踩坑。

服务端源：`E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/util/PacketCreator.java`（7758 行，revision 0f62572c…）。

## 1. 高危点裁决（lead 已逐条抽验，行号一字不差）
| 客户端门控 | 服务端发包方法体 | 裁决 |
| --- | --- | --- |
| GiveForeignBuff :1086(>=16)/:1099(>=2循环)/:1101(>=4)/:1103(>=2) | `giveForeignBuff:3148-3157` = writeInt(cid)4B + writeLongMask 两段 long 16B + 每 statup writeShort 2B + writeInt(0)4B + writeShort(0)2B | ✅ 严格下发且对齐，改 length() 全正确无错位；statups 空时 bits=0 跳过循环读尾仍对齐 |
| DamagePlayer :1060 `skill==-3 && avail>=4` | `damagePlayer:2751-2782`，:2755 `if(skill==-3) p.writeInt(0)`，:2758 writeInt(damage) | ✅ 条件严格一致，改 length() 安全；skill==-4 走 :2777-2779 第二个 int，客户端不弹伤害、while 吞尾，安全 |
| DamageMonster :1453-1459 五 int 含 skin | `damageMonster(...skinId):4285-4293` 写 int(oid)+byte0+int(damage)+int(curhp)+int(maxhp)+**int(skinId)**；所有入口 :4260/:4265/:4297 都走它 | ✅ skin 真下发，改 length() 读到真皮肤 id，正确 |
| SpawnChar level | `:2071 p.writeShort(chr.getLevel())` 注释"等级上限999需2字节，原writeByte在>255时溢出" | ✅ 确实 short(2B)，客户端按 short 读正确 |
| quest progress skip5 Messaging :212 | `updateQuest:3071-3085`，:3084 `p.skip(5)` | ✅ 5 字节真写，改 length() 正确 |
| SpawnMist :1302(>=16 拿box) | `spawnMist:4227-4240` oid/type/owner/skill int + level byte + short delay + 4×int box | ✅ 对齐，改 length() 正确 |

## 2. 唯一必须保留门控的命门（版本差实锤）
`UPDATE_QUEST_INFO` case8 存在**两个服务端写方法、尾部宽度不同**：
- `updateQuestInfo:3039-3045` = byte8 + short quest + int npc + **int 0**(:3044，4B)
- `updateQuestFinish:6489-6495` = byte8 + short quest + int npc + **short nextquest**(:6494，2B)

客户端 `QuestHandlers.cpp:96-99` `if(available>=4) read_int else if(available>=2) read_short` 正是兼容这两版。**绝不能把 >=4 简单替换后统一当 int 读——short 版会错位；>=4/>=2 双分支语义必须原样保留。** 这是全场唯一"服务端确实版本分支导致字段宽度不同"的实锤。抽验：`QuestHandlers.cpp:88-105` 原文与结论一致。

## 3. 死 opcode（非错位风险，是死代码）
`SET_ITEM_FINAL_DAMAGE(0x175)`：全库 1133 文件 grep `SET_ITEM_FINAL_DAMAGE|OutPacket.create(SendOpcode.SET_ITEM` 仅 `SendOpcode.java:367` 定义+注释，**0 处发包**。同理 0x176/0x17A(:368-369) 也 0 引用。→ 客户端 `SetItemFinalDamageHandler`(CommonHandlers.cpp:87 avail>=6) 是死 opcode，包永不到达、handler 永不触发。盲改 length() 无运行时错位风险，但改了也不生效。

## 4. 三分类总表
### ✅ 确实下发（改 length() 正确）
GiveForeignBuff 全段、DamagePlayer skill==-3、DamageMonster 5×int 含 skin、SpawnChar level short、quest skip5、SpawnMist、SpawnSummon、SkillEffect/skillCancel、ApplyMonsterStatus/CancelMonsterStatus、moveMonsterResponse、damageSummon、showMonsterHP(仅 int oid+byte pct)、CharInfo 全套(宠物循环/mount/wishlist/图鉴/勋章)、changeAllianceRankTitle(GuildPackets.java:457-464)、disbandAlliance(:498-502)、InventoryGrow、SkillLearnItemResult、getStorage、FameResponse newfame、levelUpMessage level、jobMessage job、damageSkin、Fredrick0x23 skip3。

### ⚠️ 有条件（保留门控 / 值条件 && length()）
UPDATE_QUEST_INFO case8 int/short 双版（见 §2，**必须保留双分支**）；getFindResult 2×int 仅 type==RT_SAME_CHANNEL(:5972)；damagePlayer skill==-3/-4；spawnMonsterInternal 尾部 parentMob vs parentless 分支(:1598-1605)；CharInfo mount 仅等级够(:2903)、pet 0-3 只。

### ❌ 根本不发（死代码，改了也不生效）
SET_ITEM_FINAL_DAMAGE(0x175)、0x176、0x17A。

## 5. 覆盖边界（如实标注）
已核 ~40 个发包方法 / 约 25 个 opcode；未覆盖 WaveAShell 200 处逐条（cash-shop item parser 连续 skip 序列，服务端 CASHSHOP_OPERATION 分支极多）、部分 MapObject 88 处中纯 skip 尾巴、Testing/Fishing/Wedding 边角。同构先例 `ItemParser.cpp:120-155` 已修复可作参照。未覆盖项未下结论。
