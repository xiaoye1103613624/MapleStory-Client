# r728 — Clock / DestroyShop / Buddy / SkillLearn sync

## Ensure-open / sync
- `ClockHandler` / `StopClockHandler`：emplace 后 `makeactive`
- `DestroyShopResult`：存在即 `deactivate`（不限 is_active）；失败 tip 也看 PersonalShop
- Buddy channel/capacity：`ensure_buddy` + makeactive 再刷
- SkillLearn：成功时 `change_skill` 写模型；ensure SkillBook 再 `update_skills`

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
