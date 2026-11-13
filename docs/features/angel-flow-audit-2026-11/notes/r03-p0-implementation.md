# r03 · P0 实现笔记（2026-11-12）

## 改动要点

- `BuddyListModel` / `PartyModel`：仿 `GuildModel` 会话级单例。
- `BuddyHandlers` mode7/0x14/0x15：写模型；浮窗仅已存在时同步（取消强开）。
- `PartyHandlers` 0x07/0x0F/0x0C/0x1B：写模型；DISBAND 清模型；HP 不强开浮窗。
- `UIUserList` 好友/组员页：`draw_*` + `update` 读模型。
- `UIChatBar::collect_online_buddy_cids` + 密语候选：读模型。
- `UIKeyConfig` classic/modern：`BOSSPARTY` → `MEDAL`；`KeyMapTranslate` 27↔28 不变。

## 未做（预留）

T4 blink/Progress；Quest 107 包体；NpcTalk 直发 107；远征面板；行绘像素精修。
