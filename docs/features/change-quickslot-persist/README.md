# CHANGE_QUICKSLOT 持久化（quickslotConfig → 183）

> 日期：2026-11-06 ｜ 构建：`tools/build_smoke.bat` EXITCODE=0 → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`

## 目标

快捷栏 **8 个触发键**改键能上行 `CHANGE_QUICKSLOT(183)` 并由 s9 `QuickslotKeyMappedModifiedHandler` 落库 `quickslotkeymapped`；重登经 `QUICKSLOT_INIT(0x9F)` 回灌。

## 证据

| 源 | 结论 |
| --- | --- |
| s9 `QuickslotBinding` / Handler | 上行 8×int；默认 `{0x2A,0x52,0x47,0x49,0x1D,0x53,0x4F,0x51}` |
| 095REA `SaveQuickslotKeyMap` | EncodeBuffer 0x20（8×int） |
| IMG `KeyConfig/quickslotConfig` | backgrnd 266×238（文案烘焙）；8 槽 TL `(67/102/137/172, 114/148)` 约 28×25；交互文案=「点槽再按键」 |

## 实现

1. `Keyboard::set_quickslot` 静默写 / `commit_quickslots` 上行；`QuickslotInitHandler` 改用静默写（修进图回声 183）。
2. 新窗 `UIQuickslotConfig`：`KeyConfig` 的 KEYSETTING（BtQuickSlot）打开；点槽 → 按键 → 互换撞车 → `commit_quickslots`。
3. StatusBar 8 槽显示/激活跟 `quickslots()` 触发键。

## 真机验收（pending / 可复测）

1. 键设置 →「变更快捷键」→ 点第 1 槽 → 按 `Q` → 槽显示 Q。
2. 关窗重登：该槽仍为 Q；DB `quickslotkeymapped` 有账号行。
3. 进图日志不应在 `QSI` 后立刻刷 8 次 CHANGE_QUICKSLOT。

## 非本项

- 槽内技能/道具绑定仍走 `KEYMAP(135)`（拖技能到快捷格 ≠ 183）。
- NPC_ACTION 移动分支、KeyDown cancel、公会滑轮上行：无充分证据，不发明。
