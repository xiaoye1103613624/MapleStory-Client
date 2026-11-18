# W-M 三联证据门禁（2026-11-16）

## 约束（已锁定）

- P1 SDL 多端；P2 Angel+IMG+MinMS；P3 禁止回退已闭环；P4 扩展以本端矩阵为主。
- 正确截图库：`C:\Users\11036\Desktop\正确客户端截图` —— 有商城/NPC 对话；**无匠人之魂窗截图** → Enchant 以 Angel+IMG+MinMS 为准。

## W-M1 Enchant

| 源 | 结论 |
|---|---|
| IMG orange-wz `UIWindow.img/EnchantSkill` | `backgrnd` 178×206；`say0/say1/say2`；`Effect/0..25` |
| Angel `CUIEnchantDlg::RefreshToolTip @0x7f675d` | 成功/失败文案+音效；白卷分支；MakeClone 刷新克隆图标；销毁则 Destroy 窗 |
| MinMS `UIEnchant.*` | 布局常量 PANEL 178×206；CHECK/SLOT/SAY；背包正槽 `ScrollEquipPacket`；白卷 2340000 |
| 本端不可触碰 | 装备栏 `ScrollEquipPacket(EquipSlot::Id)` 负 bodypart 路径；`UIItemInventory` 双击开窗入口 |

真缺（已补）：背包正槽 `ScrollEquipPacket` + Enchant 壳加深（Effect/say/RefreshToolTip 语义）。

## W-M2 CashShop

- 正确截图 `商城.png`：热卖格有 购买/礼物/购物车；底栏点券/充值/Exit。
- **已补**：`BtBuy`/`BtGift` → `UIYesNo` 确认；无效 SN/价目禁用；s9 `CashShopBuyPacket`/`Gift` 保留。真机已开商城对照截图（见 `01-live-verify.md`）。

## W-M3 CooldownList

- **已补**：`UICooldownList` + cooltime 秒→ms；挂 `UIStateGame` / PlayerHandlers。

## W-M4 PartyHP

- **已补**：UserList 内嵌 `GAUGE_FILL_W=63`（不独立窗）。

## W-M5 NpcTalk

- 正确截图 `NPC对话.png`：左立绘+名牌、右文本、底栏结束/接受/拒绝。
- **已补**：AcceptDecline 对齐 Angel `OnCreate_YESNO` BtYes/BtNo 槽；不改 MoreTalk/`end_talk`。
