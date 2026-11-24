# One-shot death / HP alert / teleport & flash jump — 2026-11-22/23

> 状态：代码修复 + Release/x64 构建 `EXITCODE=0`；已部署 live（见 §Build）。真机联调待用户起 s9。

## 1. Lethal hit: death anim only (no knockback slide)

**Symptom:** Skill one-shot → die clip + backward slide.

**Evidence:**
- Angel `CMob::ShowHitEffect@0xc671e0`（IDA）：只 `SetOneTimeAction`，不叠客户端持续 `hforce`。
- Journey HIT 分支旧 `KBFORCE 0.2/0.1` 每帧推怪；kill 包晚到时表现为「die + 滑」。
- MinMS：冲量衰减；致死清 `hspeed`。

**Fix (`Gameplay/MapleMap/Mob.cpp`):**
- HIT 姿态不再施加持续 `hforce`。
- `apply_damage(..., lethal)`：`hp_est`/调用方判定致死时跳过 HIT 滑行，只播 CoolHit。
- `apply_death()` / `kill()` 清 `hforce`/`hspeed`。

## 2. HP/MP low alert keeps flashing

**Symptom:** Flashed once then never again.

**Evidence:** MinMS `UIStatusBar` 低血期间每帧 `hpanimation.update()`；Angel 无「闪一次永久静音」。

**Fix (`IO/UITypes/UIStatusBar.cpp`):**
- 阈值内（HP>0 / MP>0 且 ≤ warn%）持续 `sprite.update()` + draw。
- 去掉「下降穿越只启动一次」的静默门。

## 3. Teleport / Flash Jump usable from skill key & quickslot

**Symptom:** Script/help-learned 瞬间移动 / 二段跳按了不动。

**Evidence:** MinMS `Combat::use_move` 非攻击技能不拦 `can_attack`；`Stage::send_key(SKILL)` 仅按下沿；`FLASH_JUMP_NW=14101004`。

**Fix:**
- `Combat::use_move`：普攻/`is_attack` 才门禁；位移技走 `can_use` + `apply_use_movement` + `UseSkillPacket(91)`。
- `Stage::send_key(SKILL)`：仅 `down` 触发（防松开二次发包）。
- `SkillId::FLASH_JUMP_NW` + `is_flash_jump`；`Player::can_use` 地上/已用过静默拒。
- 宏黑名单保留（Angel 原生禁传送技入宏）；技能键/快捷栏可用。

## Build

```text
MSBuild MapleStory.vcxproj /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v142
  /p:MultiProcessorCompilation=false /m:1
  /p:OutDir=x64\out-bugfix-20261123\ /p:IntDir=x64\obj-bugfix-20261123\
```

- 产物：`x64\out-bugfix-20261123\MapleStory.exe`（≈5.48MB，2026-11-23）
- Live：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`（若占用则 `MapleStory.oneshotfix.exe`）
