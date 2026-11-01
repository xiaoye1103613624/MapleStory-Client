# r172 — NpcTalk GetText/GetNumber + Shop SoftKey

- NpcTalk：`T/S` GetText → SoftKey string；`N/S` GetNumber → SoftKey int（msgtype=3）；`S` Simple → SoftKey selection
- NpcTalkMorePacket 增 `(amount, as_number)` 构造
- Shop：`B` SoftKey `slot,qty` 买；`S` SoftKey `idx,qty` 卖
- Minigame：`T/C` SoftKey chat
- MobGage / FloatNotice：`S/T` SoftKey tip→GeneralChat

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
