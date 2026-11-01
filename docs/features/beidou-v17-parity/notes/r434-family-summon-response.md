# r434 — FamilySummon 出站 + BossHP tip

- s9 `FAMILY_SUMMON_RESPONSE = 0x99`（153）：`FamilySummonResponsePacket(family, accept)`
- FAMILY_SUMMON_REQUEST YesNo → 派发出站
- OutPacket 补 `USE_FAMILY=151`
- FieldEffect BossHP mode5 → field_tip 百分比

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
