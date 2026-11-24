# 战斗 / HUD / 位移技三修（2026-11-22）

> 状态：代码已改；Release/x64 构建 + live 同步见 builds/

## 范围

1. **一击必杀只播死亡**：致死命中跳过 HIT 击退滑行  
2. **HP/MP 低量警报持续闪**：低于阈值期间循环播 hpFlash/mpFlash  
3. **脚本/help 学来的瞬移·二段跳可用**：走 `Combat::use_move` + USE_SKILL(91)

## 证据摘要

| 项 | 证据 | 本端差异 | 改法 |
| --- | --- | --- | --- |
| 致死无击退 | Angel `OnHit`：HP≤0 不 `GenerateMovePath`；MinMS `start_knockback` 仅非致死 | `apply_damage` 一律进 HIT+`hforce`；`lethal` 形参未接线 | WZ `maxHP`+`show_hp` 估 `hp_est`；`damage>=hp_est` 跳过 HIT；`kill()` 清 `hspeed` |
| 低血闪 | MinMS `UIStatusBar`：low 期间持续 `update/draw` | 曾用下降穿越只武装一次 | 低量且 >0 时每帧 `update`+`draw` |
| 位移技 | MinMS `use_move` 非攻击技不走 `can_attack`；SoftKey 应走 use_move | SoftKey/GameMenu/SkillEx 只 `UseSkillPacket`；`job`/`CLAW` 子弹门禁误拒 | SoftKey→`use_move`；持有 level>0 跳过 job；非攻击跳过子弹/武器 |

## 改动文件

- `Gameplay/MapleMap/Mob.{h,cpp}` — `maxhp`/`hp_est`、致死跳过击退、`kill` 清冲量  
- `IO/UITypes/UIStatusBar.cpp` — 低量持续闪光  
- `Gameplay/Combat/{Combat,Skill}.cpp`、`Character/{Player,SkillId}.*` — 位移技门禁与本地位移  
- `IO/UITypes/{UIMiscShells,UISkillBook}.cpp` — SoftKey/help → `use_move`（仍发 91）

## 同步修复

- SoftKey 宏黑名单仍排除传送/闪现（不可塞宏），与「键位/技能窗可用」分离  
- Cygnus/NW/Evan 瞬移·二段跳 id 补进 `apply_use_movement`  
- `UISideIconBar.cpp`：`Rectangle` 误用 `.r()/.t()` → `right()/top()`（否则 Release 编不过）  
- `tools/build_smoke.bat` 仍强制 v142，与工程 `PlatformToolset=v145` 冲突会 `__std_*` LNK2001；本轮用 VS18 MSBuild + staging OutDir 过门

## 验收

1. 一击秒小怪：只播 die，不后退滑行  
2. HP/MP 压到警报阈值下：闪光循环，回血后停  
3. 脚本学瞬移/二段跳：键位与技能窗/GameMenu F 均有本地位移，且上行 USE_SKILL(91)

## 产物

- 文档：本 README  
- Staging exe：`build-out-Release-x64/MapleStory.exe`（已覆盖 live `E:\MXD\BeiDou-Client-xy\MapleStory.exe`）  
- 戳记：[builds/exe-stamps.md](builds/exe-stamps.md)
