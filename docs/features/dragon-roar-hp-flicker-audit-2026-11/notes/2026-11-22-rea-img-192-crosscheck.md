# 2026-11-22 龙咆哮三现象：IMG / 083 / 192 / HP 链路交叉取证

> 只读解析，未改源码。工具：orange-wz MCP（128B GMS key / 192 零 IV）、IDA 既有一手产物、源码逐行、PixPin 视频逐帧。

## 1. 用户问「HP 怎么会到 0」——龙咆哮自损扣不死

| 层 | 证据 | 结论 |
|---|---|---|
| 客户端技能门禁 | `Character/Player.cpp:555-568`：`1311006` 要求 `HP > maxHP×(60-level)/100` 才可放 | 低血按不出龙咆哮 |
| 服务端自损 | `StatEffect.java:772-774` `hpR=-x/100`；`:1438-1439` `hpchange += maxHP×hpR` | 自损上限 = 最大 HP×x%，x=30..59 |
| 服务端拒自杀 | `Character.java:8542-8548`：`nextHp<=0 && (!zombify||hpCon>0)` 非 GM 直接 `return false` | 服务端不会让自损扣到 0 |
| 客户端死亡 | `PlayerHandlers.cpp:114-118`：CHANGE_STATS 后 `HP<=0 → player.die()` | HP=0 必切 DIED，`can_use` 也拦死亡态 |

**结论**：龙咆哮自损不可能把 HP 扣到 0。若看到 HP=0，来源是怪物攻击/其他伤害（视频中角色被怪围攻）。先放技能产生法阵 → 随后被打死；死亡不清除已生成的 `EffectLayer` 特效，所以「血 0 后紫圈仍在」符合代码逻辑，不是自损 bug。

## 2. 用户问「怪物身上的攻击特效是否和释放特效一样；IMG 没有攻击特效？」

| 对照 | 证据 | 结论 |
|---|---|---|
| xy 运行时 IMG | `Data/Skill/131.img` → `skill/1311006`：`effect/0..17`（18 帧，首帧 178×181）+ `hit/0`（LIST，3 子帧） | **两个独立节点**：effect=施法紫圈，hit=命中特效 |
| 192 客户端 IMG | `Skill_002.wz/131.img/skill/1311011`：`effect/0..17`（18 帧）+ 独立 `hit/0`（LIST） | 192 同样保留 effect/hit 分离结构；192 的 131.img 无 1311006（技能改版换 ID），用同职业技做结构对照 |
| 083 Angel IDA | `CUser::ShowSkillEffect@0x933990` 取 `GetEffectUOL@0x932d40`，挂施法者；`CMob::ShowHitEffect@0x66fb4d` 只做受击姿态、不加载 UOL、不播视觉 hit；`OnHit@0x668b83` 里 hit UOL 是音效 | 083 原生「视觉 hit」未确证（资源有 hit 节点但原生播放器未找到） |
| 本端实现 | `Skill.cpp:62-68` `effect`→`SingleUseEffect`→`Char::show_attack_effect`（挂角色）；`Skill.cpp:88-115` `hit/0`→`SingleHitEffect`→`Mob::show_effect`（挂怪物） | 分离语义正确；紫圈尺寸大，视觉覆盖怪物≠"打在怪物身上的受击特效" |

**结论**：IMG 有攻击（hit）特效，且与 effect 不是同一节点；本端两个都实现了。083 原生只有受击姿态、无视觉 hit 播放器证据；192 资源结构仍是 effect/hit 分离。不能把「圈覆盖怪物」判成 hit 特效错误。

## 3. 用户确认的现象：紫色法阵超过 WZ time（2~4 秒）仍不消失

- `effect` 18 帧，正常由 `EffectLayer::update` 在动画 `ended=true` 后移除（`Animation.cpp:358-362`、`EffectLayer.cpp:69-79`）。
- 若帧无显式 `delay`，`read_frame_timing` 默认 100ms（`Animation.cpp:31-34`）→ 全段约 1.8 秒。
- 视频 3.23 秒逐帧平均差 0.62/255，画面几乎静止；没有 ±4~8px 摄像机 tremble 特征。
- 尚未定因（需运行时日志/更长视频）：① 帧 delay 异常大；② 技能连发反复重新 add；③ 死亡/状态切换后 EffectLayer 更新停顿。
- 已归档视频关键帧：`evidence/video-frame-{000,096,160}.png`。

## 4. 法阵残留：定因与修复（2026-11-22 已闭环）

**定因（静态链路，无需临时日志）**：`Character/Char.cpp::show_attack_effect` 把 `get_real_attackspeed()` 当 `EffectLayer` 动画倍速传入：

```cpp
float get_real_attackspeed() const { return 1.7f - get_integer_attackspeed() / 10.0f; }
// EffectLayer::Effect::update -> sprite.update(TIMESTEP * speed)
```

speed=12 → 倍速 0.5（18 帧法阵拖到 3.6 秒以上），speed=17 → 倍速 0（`timestep=0` 动画永不推进，紫圈永久残留）。`Mob::show_effect`（受击侧）一直用固定 `1.0f`，只有施放侧中招。083 原生 `CUser::ShowSkillEffect@0x933990` 取 UOL 后投 `Effect_SkillUse`，不把角色攻速传给动画。

**修复**：`Char::show_attack_effect` 改为 `effects.add(toshow, DrawArgument(facing_right), z, 1.0f)`，施法特效按 WZ 帧 delay 播放，与受击侧统一。仅改 `Character/Char.cpp` 一处，不动公共头、不改 `EffectLayer` 全局逻辑。

**验证**：`build_smoke.bat` EXITCODE=0 / GATE OK；实机按 X 键（技能栏龙咆哮）连抓 13 帧（0.5s 间隔）：t=0.5s 紫圈出现、t=1.0s 展开、t=1.5s 变淡、t=2.0s 完全消失，2.5–6.5s 各帧无残留（符合 18 帧×默认 100ms≈1.8s）。证据：`evidence/roar-fixed-{f00,f03,f12}.png`。临时抓帧脚本/截图已清理，无调试日志残留。

## 5. 未纳入本次范围（用户未选择）

- HP=0 低血报警无限循环：`UIStatusBar.cpp:3393-3414` 代码与「hp=0 不闪」注释矛盾；可按原生加 `hp>0` 守卫，待用户决定。
- hit 特效观感增强：原生无视觉 hit 播放器证据，本端 `hit/0` 为重建实现，保持现状，待用户决定。
