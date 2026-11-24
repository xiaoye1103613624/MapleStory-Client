# 按 Angel 083 原生还原：受击闪烁 / 血条报警 / 打击感（2026-11-21）

> 状态：代码改动完成 + 代码级编译验证（本端改动 3 个 cpp 零错误）；**完整 Rebuild 与 live 部署被其他会话在途文件阻塞**，见 §4。真机未验收。

## 1. 三方证据

- Angel 083 IDA 一手：`tools/_ida_native_restore/FINDINGS.md` + 7 组 `.txt/.json`
  - `CUserLocal::SetDamaged@0x9581a9`：受击 = `SetEmotion(1,1500)` + `5000ms` 无敌窗 + `TSecType(220)`；无 2000/300/红闪/灰度公式。
  - `CMob::ShowHitEffect@0x66fb4d`：`rand()%N+6` 设怪物 action + `PrepareActionLayer` + 10s cooldown；无技能 hit 的 WZ 节点。
  - `CUIStatusBar::sub_8D850B`：HP 报警阈值 `5*CConfig[32]`（档位×5%），`100*cur/max < 阈值` 且由高到低下降穿越触发；WZ hpFlash/mpFlash；**无"闪N帧歇M帧"硬编码**。
- MinMS 源码（`E:\project\MinMS\src`）：
  - `Char.cpp:338-358` 灰度 `rgb=0.9-0.5|sin(alpha*30)|`；`:1257-1277` `invincible.set_for(1500)` + `set_alerted(5000)` + Miss(damage<=0) 只飘字不闪。
  - `UIStatusBar.cpp:74-83/340-367` 低血 `cur>0 && pct<threshold` 循环播放、无静默。
  - `Mob.cpp:7331-7347` `show_hit_spark`：`Effect/BasicEff.img/CoolHit`，Y=-15。
- orange-wz 实测：`StatusBar.img/gauge/hpFlash` = 2 位图 + 3 UOL 别名 = 5 逻辑帧 delay=130；`Effect/BasicEff.img/CoolHit` 单帧无 delay 107×39 origin(49,37)。

## 2. 本次改动（逐项行号）

### A. 人物受击闪烁（删红闪 / 灰闪 1500ms / Miss 门控）
- `Character/Char.cpp:62-67` 删除 hitflash 红闪 tint（改为注释说明）。
- `Character/Char.cpp:131` 删除 `hitflash.update()`。
- `Character/Char.cpp:239-249` `show_damage`：`damage<=0` 提前 return（Miss 只飘字）；`invincible.set_for(2000)` → `set_for(1500)`；删 `hitflash.set_for(300)`；保留 `set_alerted(5000)`。
- `Character/Char.h:192-193` 删除 `TimedBool hitflash` 成员。
- 灰闪公式 `Char.cpp:52-55` 与 Angel/MinMS 一致，保留。

### B. 血条/蓝量报警闪烁（去静默期 / 下降穿越 / hp>0）
- `IO/UITypes/UIStatusBar.cpp:3393-3432`（原 11-17 的 60 帧静默块）替换为：
  - `hp_enter = hp_low && (prev<0 || prev>=阈值)` 下降穿越触发；血恢复或 hp=0 时 `flash=0` 停。
  - `classic_last_hp/mp` 复用为"上一次百分比"（公共头布局冻结，不加成员）。
  - 低血期间 WZ 5 帧动画持续循环（`Animation::update` 末帧回 0 已确认），无静默。
  - draw/update 仍以 `flash>0` 为开关（flash=5 保持直到血恢复），语义成立。

### C. 技能命中打击感（CoolHit 火花）
- `Gameplay/MapleMap/Mob.cpp:1090` `apply_damage` 命中时调 `show_hit_spark()`。
- `Gameplay/MapleMap/Mob.cpp:890-908` 新增 `show_hit_spark()`：`nl::nx::Effect["BasicEff.img"]["CoolHit"]`，原点上方 15px，受 `SysOptSkillEffects` 开关守卫。
- `Gameplay/MapleMap/Mob.h:167-168` 新增声明。
- 技能 hit 节点 pos/z 播放（`SkillHitEffect.h:38-48`）本端已与 MinMS 同构，保留。
- 伤害数字 s9 皮肤按用户决策 A 保留。

### D. 决策点处理
- 用户 "1B 2A"：①hit 严格按 Angel；②伤害皮肤保留 s9。
- ①的严格 Angel 部分（怪物 action `rand()%N+6` + `PrepareActionLayer`）在本端 **Journey Stance 架构下无基础设施**（`Mob.h:51-71` 仅 Stance 枚举；`Mob.cpp:97` 注释自承降级），且缺 `PrepareActionLayer` 完整函数体，按 skill 门禁5「禁止想象修改」**不可行**；已落地等价的 MinMS 火花语义 + 保留 Angel OnHit 的 `set_stance(HIT)`（不发移动包）。此取舍已在交付时向用户说明。

## 3. 验证

- 全量 Rebuild（隔离 IntDir `x64\obj-nativeverify\`，因并发构建污染默认 obj）：
  - `nativeverify_build2.log`：整体 EXITCODE=1，唯一错误为 `MapBackgrounds.cpp:297/300` C2248（他人会话在途改动）。
  - 本端改动文件 `Char.cpp`/`Mob.cpp`/`UIStatusBar.cpp` 均被实际编译且**零错误**（grep 确认）。
  - `nativeverify_build.log`：卡在 `UICashShop.cpp:1577/1579` C2248（他人会话在途，后续已修复）。

## 4. 阻塞与待办

- 完整 exe 链接/live 部署未做：`git blame` 证实 `Weather/WeatherBackLayers.cpp:252`（10:47 仍 "Not Committed Yet"）、`MapBackgrounds.cpp/.h`（10:25）、`UICashShop.cpp`（10:23）正被**其他会话实时编辑**，其半成品编译错误（C2248 refresh_viewport / C2440 DrawArgument 构造 / C2248 Item private）阻塞全量链接。按门禁4「最小改动、不碰无关功能 + 不强改二进制猜测」，未擅自修改这些他人会话的在途文件（曾短暂修 MapBackgrounds.h 后已撤销复原）。
- 本端改动 3 个 cpp（Char/Mob/UIStatusBar）在两次全量 Rebuild 中均实际编译且零错误，代码级验证通过。
- 待并发会话收尾、工作区编译干净后：`tools/build_smoke.bat` 或全量 Rebuild → 按铁律覆盖 live `E:\MXD\BeiDou-Client-xy\MapleStory.exe` → 实时核对 SHA → 真机验收（截图须本端）。
- 真机未验收：受击闪观感、血条报警节拍、CoolHit 火花位置均需用户实机确认。
