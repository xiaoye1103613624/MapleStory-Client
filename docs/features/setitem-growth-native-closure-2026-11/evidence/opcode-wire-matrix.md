# Opcode / 线格式矩阵（2026-11-20）

| 方向 | 常量 | 码 | 线格式摘要 |
| --- | --- | --- | --- |
| S→C | SET_ITEM_FINAL_DAMAGE | 0x175 | short fd% \| int skin \| short damR/bdR/ndR/ignPDR/ignMDR/critR/critD/padR/madR \| short fdSrcN + short×N \| short drop/meso/dmgReduce \| optional asr/buff/stance |
| S→C | SET_ITEM_SKILL_BONUS | 0x176 | short count + { int setId, byte enabled, string text, short itemN + int×N, short tierN + {short need, string key, int val}×N } |
| C→S | SET_ITEM_BONUS_REQUEST | 0x177 | int setId |
| S→C | WORLD_MAP_DROP_RESULT | 0x179 | （保留世界地图；原 SET_SKILL_BONUS 已迁走） |
| S→C | SET_SKILL_BONUS | 0x17A | short count + { int skillId, int addLevel }×N |
| S→C | EQUIP_GROWTH_TIP | 0x17B | int itemId \| byte has \| string text \| byte hasBonus + short[15]? \| byte hasFlame + short[15]? |
| S→C | DAILY_CHECKIN | 0x17C | （已占用，勿动） |
| C→S | EQUIP_GROWTH_TIP_REQUEST | 0x17D | int itemId |

证据：IDA `CItemInfo::RegisterSetItemInfo` @ `0xaf1540`（MapleStoryT.i64）；参考 ijl15 SetItemBridge / EquipGrowthBridge。
