# r302 — Damage / Pet / Summon / Mist / Door / Catch

- DAMAGE_PLAYER(192) → Char::show_damage（前缀解析）
- ENERGY_ATTACK(189) → CloseAttack 同布局
- GIVE/CANCEL_FOREIGN_BUFF(199/200) cid 落日志（mask 深接后续）
- CATCH_MONSTER(251) / WITH_ITEM(252)
- PET_CHAT(171) → speak；PET_NAMECHANGE(172)；PET_COMMAND(174)
- REMOVE_NPC(258)
- SPAWN/REMOVE_MIST(273/274)、SPAWN/REMOVE_DOOR(275/276) 解析落日志
- SPAWN/REMOVE_SPECIAL_MAPOBJECT(175/176) summon 解析落日志（地图对象深接后续）

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
