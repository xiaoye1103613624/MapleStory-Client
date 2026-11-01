# r40 — UtilDlgEx 分支 + 剩余 UIWindow 壳

用户指令：不停歇继续。

## 本切片

### UtilDlgEx 族
- `UIUtilDlgExAvatar` / `UIUtilDlgExPet` / `UIUtilDlgExMultiPet` 壳
- `UINpcTalk::apply_dlg_skin`：speaker 0/1/2 → UtilDlgEx / `_Avatar` / `_Pet`

### 剩余顶层壳
ExceptionItemSearch · MobGage · SkillUp · MacroSkill · QuestIcon · MinigameTable · FloatNotice  
（`load_window_bg` 兼容 `t` / `Cover`）

### 接线
- `UpdateSkillHandler` → `UISkillUp` tip
- `ShowMobHpHandler` → `UIMobGage` tip
- Minigame ROOM / START → 同步开 `UIMinigameTable`

GameOpt / SysOpt 仍由既有 `UIOptionMenu` 覆盖（不重复壳）。

## 构建
`tools/build_smoke.bat` → OutDir `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
