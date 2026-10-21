# v83 HUD / Quest 映射证据

来源：
- `BeiDou-Server/gms-server/wz/UI.wz/StatusBar.img.xml`
- `BeiDou-Server/gms-server/wz/UI.wz/UIWindow.img.xml`（一级节点见 `UIWindow.img.top-level.txt`）
- BeiDou `RecvOpcode.QUEST_ACTION = 0x6B`，`SendOpcode.UPDATE_QUEST_INFO = 0xD3`（211）
- IDA 字符串：`UI/StatusBar.img/base/chatTarget`、`UI/StatusBar.img/BtNPT`

## StatusBar3 → StatusBar.img

| Journey (StatusBar3) | v83 (StatusBar.img) | 备注 |
|---|---|---|
| mainBar/status*/backgrnd | base/backgrnd (+ backgrnd2) | 底部整条，高约 71 |
| mainBar/.../gauge/hp\|mp | gauge/bar + gauge/graduation + hpFlash/mpFlash | 裁剪绘制 |
| mainBar/EXPBar | gauge/gray + gauge/tempExp | 1px 条拉伸 |
| mainBar/menu buttons | BtShop / BtMenu / BtNPT / BtShort … | 直接按钮 |
| mainBar quickSlot | base/quickSlot + QuickSlot / QuickSlotD | |
| StatusBar3/chat | base/chat + chatTarget | 聊天嵌在状态栏 |
| StatusBar3/KeyConfig | UIWindow.img/KeyConfig | |
| StatusBar3/OptionMenu | UIWindow.img/GameOpt + SysOpt | |

## UIWindow2/4 → UIWindow.img

| 窗口 | modern | v83 |
|---|---|---|
| Item | UIWindow2/Item | UIWindow/Item |
| Equip | UIWindow4/Equip | UIWindow/Equip |
| Stat | UIWindow4/Stat | UIWindow/Stat |
| Skill | UIWindow2/Skill | UIWindow/Skill |
| Quest | UIWindow2/Quest | UIWindow/Quest |
| UserList | UIWindow2/UserList | UIWindow/UserList |
| WorldMap | UIWindow2/WorldMap | UIWindow/WorldMap |
| UserInfo | UIWindow2/UserInfo | UIWindow/UserInfo |
| Channel | UIWindow2/Channel | UIWindow/Channel |
| UtilDlgEx | UIWindow2/UtilDlgEx | UIWindow/UtilDlgEx |
| KeyConfig | StatusBar3/KeyConfig | UIWindow/KeyConfig |
| Shop | UIWindow2/Shop2 | UIWindow/Shop（已有） |
| MiniMap | UIWindow2/MiniMap | UIWindow/MiniMap（已有） |

## Quest 协议

- C2S `QUEST_ACTION(0x6B)`：`byte action, short questid, …`（1 start / 2 complete / 3 forfeit / 4 scripted start / 5 scripted end）
- S2C `UPDATE_QUEST_INFO(0xD3)`：mode byte（6/7 时限、8 finish、0x0A error、0x0F expire…）
- S2C `SHOW_STATUS_INFO` mode `1`：任务进度（questId, status, progress string）
