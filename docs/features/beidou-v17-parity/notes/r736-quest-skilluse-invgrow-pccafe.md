# r736 — Ensure crumbs（Handler wave 收尾）

## Ensure / tip sync
- ShowStatusInfo mode1 quest tip → QuestAlarm ensure
- QuestInfo `0x0A`/`0x0F`/default → 走 `quest_alarm()`（不再裸 notify）
- SkillUseResult fail → SkillUp ensure + tip
- InventoryGrow → ItemInventory ensure
- KoreanInternetCafe → Event ensure（对齐 WeekEvent）

## 状态
Handler ensure-open 波次接近饱和。阻塞项仍待用户：登录叠图、PersonalShop CREATE FM、Messenger 立绘。

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
