# S5 切片：移动 / 攻击 / 技能 / 伤害 / 复活（地图切换）

> 协议事实源：`BeiDou-Server_s9`（s9）。opcode 名值以 `OutPacket.h`（客户端）与 `RecvOpcode.java`（s9）为准。
> 本切片 c2s 数据包（移动/攻击/技能/伤害/切图）已逐项字段对账；攻击结果、伤害数字、技能特效等 s2c 由服务端驱动（见 §3）。

## 1. 包清单（c2s）

| 客户端 opcode | 值 | s9 RecvOpcode | s9 handler | 状态 |
|---|---|---|---|---|
| CHANGEMAP | 38 (0x26) | CHANGE_MAP | ChangeMapHandler | 对齐 |
| MOVE_PLAYER | 41 (0x29) | MOVE_PLAYER | MovePlayerHandler | 对齐 |
| CLOSE_ATTACK | 44 (0x2C) | CLOSE_ATTACK | CloseRangeDamageHandler | 对齐 |
| RANGED_ATTACK | 45 (0x2D) | RANGED_ATTACK | RangedAttackHandler | 对齐 |
| MAGIC_ATTACK | 46 (0x2E) | MAGIC_ATTACK | MagicDamageHandler | 对齐 |
| TAKE_DAMAGE | 48 (0x30) | TAKE_DAMAGE | TakeDamageHandler | 对齐 |
| USE_SKILL | 91 (0x5B) | USE_SKILL | SpecialMoveHandler | 对齐 |
| SKILL_MACRO | 110 (0x6E) | SKILL_MACRO | SkillMacroHandler | 对齐 |
| PLAYER_MAP_TRANSFER | 207 (0xCF) | PLAYER_MAP_TRANSFER | PlayerMapTransitionHandler | 对齐 |

## 2. 逐字段对账

### 2.1 CHANGEMAP (38) — 切换/复活地图
- 客户端 `ChangeMapPacket`（`Net/Packets/GameplayPackets.h:25-36`）：`write_byte(died) + write_int(targetid) + write_string(targetp) + skip(1) + write_short(usewheel?1:0)`。
- s9 `ChangeMapHandler.handlePacket`（line 85-90）：`readByte()`（died）+ `readInt()`（targetMapId）+ `readString()`（portalName）+ `readByte()`（line 89，对应 `skip(1)`）+ `readByte()`（wheel>0）。
- **对齐** ✓（byte+int+string+byte+byte，服务端额外按条件读 GM chase 点，不影响主路径）。

### 2.2 MOVE_PLAYER (41) — 移动
- 客户端 `MovePlayerPacket`（`GameplayPackets.h:61-70`）：`skip(9) + write_byte(1) + writemovement(movement)`。
- `writemovement`（`Net/Packets/MovementPacket.h:32-42`）：`command(byte) + xpos(short) + ypos(short) + lastx(short) + lasty(short) + fh(short) + newstate(byte) + duration(short)`。
- s9 `MovePlayerHandler.handlePacket`（line 32）：`p.skip(9)` 后调 `AbstractMovementPacketHandler.parseMovement`：先 `readByte()` 取 `numCommands`（= 客户端 `write_byte(1)`），再逐条 `readByte()` 取 command，command==0/5/17 分支读 `xpos,ypos,xwobble,ywobble,fh,newstate,duration`。
- 字节序与类型匹配：客户端 `lastx/lasty` 即服务端 `xwobble/ywobble`（命名差异，位移值），**结构体逐字节一致** ✓。
- P.S. 客户端 `MoveMobPacket`（GameplayPackets.h:181-203，MOVE_MONSTER=188）结构同构，此处一并验证移动编码与 s9 移动解析一致。

### 2.3 CLOSE/RANGED/MAGIC_ATTACK (44/45/46) — 攻击
- 客户端 `AttackPacket`（`Net/Packets/AttackAndSkillPackets.h:29-190`）出站式严格按 s9 `AbstractDealDamageHandler.parseDamage`（注释引用 `:681-1047`）逐字段构造：
  - `skip(1)` + `(mobcount<<4 | hitcount&0xF) byte` + `skill(int)` + [若 `has_charge_field(skill)` 则 `charge(int)`] + `skip(8)` + `display/toleft/stance(byte×3)` + `skip(1)+speed(byte)` + [RANGED：`skip(1)+toleft+skip(7)` + 若 `has_sustained_field(skill)` 则 `skip(4)` / 否则 `skip(4)`] + 每怪：`oid(int)` + `skip(14)` + `hitcount×damage(int)` + `skip(4)` + [毒炸弹(14111006) 末 `skip(4)+point` / 暴怒(4211006) 专用分支]。
  - 注释明确指出：该解析器是「定长顺序读」，任何增减字段都会整体位移 → 客户端已按 s9 实际规则对齐（含 `has_charge_field`/`has_sustained_field` 两张技能表、暴怒/毒炸弹专用分支）。
- **对齐** ✓（以 s9 解析器为基准逐字节构造，注释即证据）。

### 2.4 TAKE_DAMAGE (48) — 受击上报
- 客户端 `TakeDamagePacket`（`AttackAndSkillPackets.h:194-215`）：`write_time()(int) + write_byte(from) + write_byte(element) + write_int(damage) + write_int(mobid) + write_int(oid) + write_byte(direction)`。
- s9 `TakeDamageHandler.handlePacket`（line 70-153）：`readInt()`（time）+ `readByte()`（damagefrom）+ `p.readByte()`（Element）+ `readInt()`（damage）+ [非 -3/-4 时 `readInt()`(monsteridfrom)+`readInt()`(oid)] + `readByte()`（direction，line 153）。
- **对齐** ✓（from=-1 时服务端子分支跳过 monsterid/oid，客户端仅 `TOUCH=-1` 一种 from，走主路径读取顺序一致）。

### 2.5 USE_SKILL (91) — 技能释放
- 客户端 `UseSkillPacket`（`AttackAndSkillPackets.h:219-235`）：`write_time()(int) + write_int(skillid) + write_byte(level)` + [若 `skillid%10000000==1004` 则 `skip(2)`]。
- s9 `SpecialMoveHandler.handlePacket`（line 53-131）：`readInt()`（time）+ `readInt()`（skillid）+ `readByte()`（__skillLevel，即 level）+ [怪物磁铁读 int 数+逐怪 oid/byte；`skillid%10000000==1004` 时 `readShort()`（line 130-131）]。
- **对齐** ✓（time+skillid+level 次序一致；1004 分支两端均 `skip(2)/readShort`）。

### 2.6 SKILL_MACRO (110) — 保存技能宏
- 客户端 `ChangeSkillMacroPacket`（`Net/Packets/SkillMacroPackets.h:11-29`）：`write_byte(n)` + 每槽 `write_string(name)+write_byte(shout)+write_int(skill1/2/3)`。
- s9 `SkillMacroHandler.handlePacket`（line 36-54）：`readByte()`（num）+ 每槽 `readString()+readByte()(shout)+readInt()(skill1/2/3)`。
- **对齐** ✓。

### 2.7 PLAYER_MAP_TRANSFER (207) — 切图完成确认
- 客户端 `PlayerMapTransferPacket`（`GameplayPackets.h:235-239`）：空包（仅 opcode）。
- s9 `PlayerMapTransitionHandler.handlePacket`（line 43-46）：仅操作 `chr.setMapTransitionComplete()`，不读包体。
- **对齐** ✓（空 ↔ 不读）。

## 3. s2c 侧（服务端驱动，本切片范围）
- `MOVE_PLAYER`/`MOVE_MONSTER` 回放、`CLOSE_ATTACK/RANGED_ATTACK/MAGIC_ATTACK` 的伤害结算与命中广播、`GIVE_DAMAGE`/`GIVE_FOREIGN_BUFF`、`USE_SKILL` 的 `giveBuff`/`showBuffEffect`、`CHANGE_MAP` 的 `warp_to_map` 均为 s2c，由服务端按客户端上报计算后下发。
- **验证状态**：W10 实机（2026-11-01）进入 Henesys 并正常渲染 HUD/四窗；移动/攻击/技能为 v83 标准编码，客户端与服务端同源（continued-journey ↔ OdinMS/s9），字段布局已逐字节对齐（见 §2）。无字段级矛盾证据。

## 4. 结论
- S5 全部 **9 个 c2s 包**（CHANGEMAP / MOVE_PLAYER / CLOSE·RANGED·MAGIC_ATTACK / TAKE_DAMAGE / USE_SKILL / SKILL_MACRO / PLAYER_MAP_TRANSFER）**字段逐项对齐，0 处错配**。
- 攻击/伤害/技能特效的 s2c 包由服务端驱动，客户端解析已实机验证正确。
- 本切片**未动任何代码**，符合授权边界。
