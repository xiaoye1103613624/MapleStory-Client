# r780 — SkillEx/SkillUp 用技能书选中；ItemMove SoftKey M 用背包源

## UI
- `UISkillBook::get_selected_skill_id()` 公开
- SkillEx 打开 / SoftKey `U`/`S`：有选中技能则直接 UseSkill
- SkillUp 打开 / SoftKey `S`：有选中则直接 SpendSp
- ItemMove SoftKey `M`：无源时从背包选中取源格

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
