# 伤害计算 / 伤害上限：三层核查 + long 化 + 双端联动

> 执行日期：2026-11-04。范围：当前 C++ 客户端 `e:/project/MapleStory-Client` 与实际运行的 s9 服务端 `e:/project/BeiDou-Server_s9`。
> 对照基准：北斗 V17.7 原生端（`BeiDou.exe` + `ijl15`）→ s9 服务端 → Fyms085 开源基线。
> 用户决议：s9 双端联动；12 字节/行锁步协议；上限 2^53；**不含**高版本伤害属性体系（damR/bdR/fdR、COMBAT_STAT_SYNC、Web/DB）。

## 1. 结论

**改造前没有完整还原**：出站封包版式已对齐（r806 13/13），但公式、动态上限、暴击链路、长伤害、伤害皮肤均不完整；s9 直接采信客户端上报伤害扣血，只做上限校验，因此公式错误会直接反映到玩法数值。

本轮已完成的闭环：

| 项 | 改造前 | 改造后 |
| --- | --- | --- |
| 原生单段上限 | `BeiDou.exe` `dbl_AFE8A0 = 199999.0`（IDA 取证） | 移除；客户端唯一 clamp 点 `Mob::next_damage` 改 2^53 |
| 客户端硬编码封顶 | `DAMAGECAP = 999999.0` | 删除；改为 `min(配置 cap, 0x3730 破功值, 2^53)` |
| 伤害行协议 | 4 字节 int32 | 可切换 12 字节 `int32 vanillaLine + int64 真实值` |
| 服务端伤害累计 | `Map<Integer,List<Integer>>` + int 累加 | `Map<Integer,List<Long>>` + long 累加 |
| 怪物 HP | `AtomicInteger`（>21 亿回绕） | `AtomicLong` |
| 反外挂幅度校验 | `maxWithCrit*1.5/*5` DAMAGE_HACK | 12 字节模式下移除；legacy 模式保留 |
| 破功值 | 仅客户端窗口标题、s9 无下发 | s9 上线时下发 `0x3730`，客户端接入 clamp |
| 暴击编码 | 服务端 `-Integer.MAX_VALUE + d - 1` 反写 | 暴击位只放 `vanillaLine` 的 `0x80000000` |
| 超长数字显示 | `int32_t` + 固定字宽 | `int64_t`，按最大宽度等比缩放、不截断 |
| 伤害公式 | Fyms085 简化式、多项恒零 | v2 开关补魔法基础值、无视防御、暴击倍率区间、BOSS 增伤、玩家受伤保护 |

**仍为「部分/预留」（不假装完成）**：

- 原生 `CalcDamage::PDamage/MDamage` 是约 10KB/2.5KB 的复杂函数，Hex-Rays 重建文本未逐字段还原；v2 公式是**证据驱动的近似补齐**，不是逐字节等价。玩家受伤公式仍沿用 Fyms085 近似式（只补了限幅与非负保护），原生独立函数未定位。
- 伤害皮肤：`UI.nx/UIWindow.img` 无 `DamageSkin` 节点、`Effect.nx/BasicEff.img` 无 `damageSkin` 节点，运行时资源缺失 → 显式预留；默认数字图集 `NoRed*/NoCri*/NoViolet*` 已守卫。
- `DamageRank` / `DPT_TRACKER(0x3714)`：客户端无 handler、包结构未取证 → 预留。
- `UIStatsInfo` 的 `DAMAGE_BONUS` / `FINAL_DAMAGE` 属高版本属性体系，用户明确不纳入；`CRITICAL_DAMAGE` 已改为真实 `mincrit`。
- 破功值**不做 DB 持久化**（无授权 DDL/DML），内存态默认 2^53；突破石成长未迁移。
- `summonAttack`、`PacketCreator.damageMonster` 的 HP 弹字、梅索爆炸 C2S 专用分支属插件文档 §15 leftovers，按原版式保留。

## 2. 取证与差异矩阵

- `notes/r001-native-damage-evidence.md`：IDA 地址、199999.0 常量字节、xrefs、反编译片段、运行时资源巡检、口径局限。
- `notes/r002-three-layer-matrix.md`：三层逐项差异矩阵与改动面/风险清单。
- `notes/client-path.md`：客户端与 s9 的改动路径索引。
- `evidence/ida_decompile_*`：原生 `sub_78DF87`(物理) / `sub_791617`(魔法) 反编译原文。
- `evidence/damage_formula_check.txt`、`attack_layout_*.txt`、`s9_maven_compile.txt`、`client_build_tail.txt`。
- `patches/client.patch`、`patches/s9.patch`：关键 diff（新文件不进 git diff，见第 3 节路径）。

关键证据：`0x00AFE8A0` 的 8 字节 `00 00 00 00 F8 69 08 41` = double **199999.0**；物理 `0x79058F` / 魔法 `0x791F94` 的 `jb +8` 后在写槽前 `fld 199999.0` 替换 ST(0) 再 `__ftol`。这正是用户说的「19万9」。

## 3. 改动路径

**客户端（26 改 + 2 新增）**

- 新增 `Gameplay/Combat/Damage.h`：`EXACT_INT_CAP=2^53`、`DamageLine/DamageLines`、`clamp_damage`、`vanilla_line`、`legacy_line`、`server_limit_break/effective_cap`。
- 新增 `tools/damage_formula_check.py`：公式/上限/暴击位/legacy 边界对拍。
- `Configuration.{h,cpp}`、`xy-config.example.yaml`：`damage.formula_v2` / `damage.long_lines` / `damage.cap` 三个开关（默认 legacy/false、cap 2^53）。
- 公式链：`Character/CharStats.cpp`、`Character/Player.cpp`、`Gameplay/Combat/Skill.cpp`、`Data/SkillData.{h,cpp}`、`Gameplay/MapleMap/Mob.{h,cpp}`、`Util/Randomizer.h`。
- 封包链：`Net/Packets/AttackAndSkillPackets.h`、`Net/Handlers/AttackHandlers.cpp`、`Net/Handlers/CommonHandlers.cpp`、`Net/OutPacket.cpp`（仅注释）、`Gameplay/Combat/Attack.h`、`Combat.{h,cpp}`。
- 显示链：`Gameplay/Combat/DamageNumber.{h,cpp}`、`Character/Char.{h,cpp}`、`IO/UITypes/UIStatsInfo.cpp`、`IO/UITypes/UICharInfo.cpp`。
- 工具：`tools/attack_layout_check.py`（双模式）、`docs/features/s9-protocol-align/notes/opcode-diff.md`（复核记录）。

**s9 服务端（12 改 + 1 新增）**

- 新增 `org/gms/config/DamageLinesConfig.java`：双端开关、2^53 cap、`clampToInt`、`vanillaLine`、`legacyBroadcastLine`。
- `application.yml`：`gms.service.damage.long-lines` / `cap`。
- `AbstractDealDamageHandler.java`：`AttackInfo.allDamage` → `List<Long>`、`readDamageLine()`、long 累计、去幅度 DAMAGE_HACK（仅 12B 模式）、保留 Too many lines。
- `PacketCreator.java`：三个 attack 方法 + `addAttackBody` 改 Long 并写 int+long；新增 `damageMonster(int,long)` / `healMonster(...,long,..)` / `limitBreakSync(long)`。
- `Monster.java`：`AtomicLong hp`、long 版 `applyAndGetHpDamage/damage/applyDamage/aggroMonsterDamage`、Boss 血条收窄。
- `MapleMap.java` / `MonsterAggroCoordinator.java` / `EventInstanceManager.java` / `CloseRangeDamageHandler.java` / `MobDamageMobFriendlyHandler.java`：连带 long 化或收窄。
- `Character.java`（`limitBreak` 内存态）、`SendOpcode.java`（`LIMIT_BREAK_SYNC(0x3730)`）、`PlayerLoggedinHandler.java`（上线时下发）。

## 4. 12 字节/行契约（双端逐字节一致）

| 字段 | 宽度 | 语义 |
| --- | --- | --- |
| `vanillaLine` | int32 | 未溢出写真值，溢出写 `INT_MAX` 哨兵；暴击置 `0x80000000` 位 |
| `real` | int64（小端） | 非负真实伤害，`0..2^53` |

- C2S：`AttackPacket` 写 → s9 `readDamageLine()` 读；梅索爆炸 4211006 专用版式同样逐行 12B，其余 `skip` 规则保持 r806 不变。
- S2C：`PacketCreator.addAttackBody` 写 → 客户端 `AttackHandlers` 读；旁观者看到相同数值与暴击样式。
- `allDamage` 用**符号编码暴击**（负值 = 暴击，绝对值即真实伤害），统一后 `applyAttack` 不再做 `+Integer.MAX_VALUE` 补偿（旧实现广播少 1 点）。
- legacy 模式仍 4 字节/行，暴击沿用服务端旧负数编码；写包前 clamp 到 `INT_MAX` 避免溢出成负数被误判。

## 5. 开关、锁步与回滚

> **不兼容窗口**：客户端与 s9 必须同版本发布。旧端连新端 / 新端连旧端都会让攻击包整体读错位。

默认都是 `false`（legacy），所以**刚构建完不会立刻改变玩法数值**。启用步骤：

1. 客户端 `E:/MXD/BeiDou-Client-xy/xy-config.yaml` 增加/修改：
   ```yaml
   damage:
     formula_v2: true
     long_lines: true
     cap: 9007199254740992
   ```
2. s9 `gms-server/src/main/resources/application.yml` 把 `gms.service.damage.long-lines` 改为 `true`（或启动时加 `-Dgms.damage.longLines=true`）。
3. **重新打包并重启 s9**：本轮只做了 `mvn -o -pl gms-server -am compile`（刷新 `target/classes`），未重新生成 `BeiDou.jar`；需要用户用 Java 21 + MySQL 重新 `package` 并启动服务。
4. 重启客户端，确认 `packet_dump.log` 的伤害行是 12 字节且 `0x3730` 已下发。

回滚：双端同时把 `long_lines` 改回 `false`，其余数值链仍安全（long 累计/AtomicLong 在 legacy 下也生效）。

`damage.formula_v2` 单独控制公式补齐，关闭即回到改造前伤害结果；`damage.cap` 与服务端 `cap` 可设更小值，破功值 `0x3730` 会取 `min(cap, 破功值, 2^53)`。

## 6. 验证结果

| 项 | 结果 |
| --- | --- |
| 客户端 `tools/build_smoke.bat` | `GATE OK` / `EXITCODE=0`；`E:/MXD/BeiDou-Client-xy/MapleStory.exe` 2026-11-04 20:05，4,679,168 字节 |
| opcode 门禁 | 新增 `LIMIT_BREAK_SYNC` 后重跑 `gen_opcode_matrix.py` + `--check` → `GATE OK` |
| s9 编译 | `JAVA_HOME=C:/Users/11036/.jdks/ms-21.0.12.1`，Maven 3.9.16 离线，强制全量 `Compiling 1128 source files [debug target 21]`，`BUILD SUCCESS` |
| 字节对拍 legacy | 13/13 `ALL LAYOUTS ALIGN` |
| 字节对拍 12B/行 | 9/9 `ALL LAYOUTS ALIGN`（含超 INT_MAX、2^53、暴击位、梅索爆炸、15 怪钳制） |
| 修复前反向对照 | `--legacy` 9/13 FAIL（预期，保持原语义） |
| 公式/上限对拍 | `ALL DAMAGE CHECKS ALIGN：0 项异常` |
| IDE lint | 客户端 6 个关键改动文件 0 诊断 |
| **真机联调** | **阻塞**：检查时 8484 端口未监听，s9 未启动；需要用户用 Java 21 + MySQL 启动并按第 5 节启用双端开关后再抓包/截图 |

## 7. 数据库与日志

- 本轮**未做任何 DDL/DML**，破功值为内存态字段；持久化需先按规范取得数据库通道与 DDL 授权。
- 未新增 `[DBG-DMG]` 等临时调试日志；Release 下 `LOG()` 为 no-op 的约束未被突破。
- 已清理本轮产生的临时 Maven 日志；保留 `evidence/` 下的归档摘要。

## 8. 借鉴来源（只读，未安装插件）

- `E:/MXD/扩展改动/伤害上限/IMPLEMENTATION.md`：Kaentake 注入 DLL + Cosmic 服务端的 int→long 协议、服务端改造、pitfalls、测试计划。
- `E:/MXD/自定义功能实现/长伤害显示/最终实现方案.md` + `resources/damagelong.cpp`：北斗 ijl15 同构机制，但**完整 64 位封包链路休眠**（`AttachDamageLongMod()` 未接线）。
- 当前端是 C++ 源码重写，不需要 VA/hook/`__ftol` 拦截/哨兵槽位映射；协议与 pitfalls 借鉴，汇编不移植。
