# 改动前链路取证摘要（mob-level-nametag）

日期：2026-11-21

## 解析要点（改动前只读）

- 怪物名牌绘制：`Gameplay/MapleMap/Mob.cpp:654-675` `Mob::draw()`，`namelabel.draw(absp)`（`absp` = 脚底），
  血条 `hpbar.draw(headpos, ...)`（头顶）。
- 名称内容：`Mob.cpp:148` `name = nl::nx::String["Mob.img"][id]["name"]`；名牌 `Text(A13M, CENTER, WHITE, NAMETAG, name)`（:190）。
- 真实等级：`Mob.cpp:42` `level = info["level"]`（`info = nl::nx::Mob["<id>.img"]["info"]`）；成员 `Mob.h:247 uint16_t level`；
  spawn 包无等级字段（`Gameplay/Spawn.h` `MobSpawn` 无 level；`MapObjectHandlers.cpp` 236/238 handler 不解析 level）。
- 显隐规则：`MOB_INFO_TIER[4]`（`Mob.cpp:639-651`，配置项 `ui.sysopt.mob_info`）控制名称/血条；
  血条受击后 `showhp.set_for(2000)`（`Mob.cpp:837`）显示 2 秒，`hppercent=0` 时不画。对齐 Angel 083 `CreateHPIndicator`。
- 等级差染色：`Mob::show_hp()` `playerlevel - level`，>9 黄、<-9 红（:819-829）。
- NAMETAG 黑底框：`GraphicsGL.cpp:1777-1820` `case Text::Background::NAMETAG`，程序矩形，左右各外扩约 1~3px。
- 独立名牌类 `IO/Components/NameTag.*`（`NameTag.img` 三切片 w/e/c）已实现但**未接线到 Mob**，本次按用户决策未使用（选纯代码动态绘制）。

## 本次改动（3 处 + 1 成员）

| 文件 | 位置 | 改动 |
| --- | --- | --- |
| `Mob.h` | `namelabel` 旁 | 新增 `Text levellabel` 成员 |
| `Mob.cpp` | 构造（:190 旁） | 初始化 `levellabel = Text(A13M,CENTER,WHITE,NAMETAG,"Lv:"+to_string(level))` |
| `Mob.cpp` | `draw()` `tier.name` 门禁 | 名称原样绘制；等级牌按 `name.width()+GAP(4)+level.width()` 挂左侧，同 y 并排 |
| `Mob.cpp` | `show_hp()` 染色分支 | 等级差黄/红染色同步给 `levellabel` |

## 校验

- 构建：`tools/build_smoke.bat` EXITCODE=0（GATE OK）。
- 部署 exe：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` 5124096 B，LastWriteTime 2026/11/21 8:31:27。
- lint：无新增诊断。
