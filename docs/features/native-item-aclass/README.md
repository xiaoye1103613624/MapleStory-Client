# W3 — A 类道具（发对包即活）

| 项 | opcode | 接线 |
| --- | --- | --- |
| 脚本道具 | SCRIPTED_ITEM 78 | `UseItemPacket` ← `ItemData::is_scripted_item` |
| 捕捉球 | USE_CATCH_ITEM 81 | USE 227* 双击 + 最近怪 |
| 死亡效果 | USE_DEATHITEM 53 | HP==0 时现金道具双击 |
| 取消道具效果 | CANCEL_ITEM_EFFECT 73 | UIBuffList 右键（itemid≥1e6） |
| 远程扭蛋 | USE_REMOTE 116 | UIRemoteGachapon（5451000） |
| 快捷栏改键 | CHANGE_QUICKSLOT 183 | Keyboard::assign_quickslot + KeyConfig 保存 |
