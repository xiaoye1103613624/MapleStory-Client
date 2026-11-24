# 怪物脚底等级名牌（Lv:N）

> 状态：已闭环（构建通过；实机截图待用户进图确认）
> 适用客户端/协议版本：BeiDou v16/v17（v83 协议，Gameplay/MapleMap/Mob）
> 项目名称：MapleStory-Client
> 作者/日期：2026-11-21
> 相关提交：未提交（工作区改动）
> 产物目录：docs/features/mob-level-nametag/

## 1. 背景与目标

用户诉求：普通怪物脚底名称和血条中，血条受击 2 秒后消失、名称常显是原生正确行为（保持不动），
本次仅新增一个**独立的等级名牌**，在怪物脚底与名称**水平并排、等级在前（左侧）**，
内容为该怪的真实等级 `Lv:N`，纯代码动态绘制，不改 WZ/IMG 资源。

明确不做的事：

- 不改显隐规则（名称常显 / 血条受击 2 秒消失，`Mob.cpp` 的 `MOB_INFO_TIER`、`showhp.set_for(2000)` 原样保留）。
- 不改协议（等级来自本地 WZ，spawn 包无等级字段，无需改 opcode/handler）。
- 不改 WZ/IMG 资源，不烘焙静态文字。

## 2. 影响范围

| 层级 | 路径/模块 | 变更类型 |
| --- | --- | --- |
| 服务端 | — | 无 |
| 脚本 | — | 无 |
| 管理端/前端 | — | 无 |
| WZ/XML | — | 无 |
| 客户端 IMG/资源目录 | — | 无 |
| 插件 | — | 无 |
| DB/配置 | — | 无 |
| 客户端代码 | `Gameplay/MapleMap/Mob.h`、`Mob.cpp` | 新增 `Text levellabel` 成员 + 构造/绘制/染色接线 |

## 3. 最终实现逻辑

### 3.1 数据 / 节点路径与语义

- 真实等级：`Mob.cpp` 构造开头 `level = info["level"]`，其中 `info = nl::nx::Mob["<7位id>.img"]["info"]`。
  spawn 包（`SpawnMobHandler` / `SpawnMobControllerHandler`）不含 level，故不依赖网络。
- 等级 0（精简/WZ 缺节点）：按原样显示 `Lv:0`，不臆造隐藏规则（R6 精简 img 兼容）。

### 3.2 主流程

1. **构造**（`Mob.cpp`，`namelabel` 初始化旁）：
   `levellabel = Text(A13M, CENTER, WHITE, NAMETAG, "Lv:" + std::to_string(level))`，
   与名称同字体、同 `Text::Background::NAMETAG` 黑底框，保证观感同构。
2. **绘制**（`Mob::draw()`，与名称同一 `tier.name` 门禁内）：
   名称 `namelabel.draw(absp)` 定位不动；等级牌挂其左侧，中心
   `level_x = absp.x - namelabel.width()/2 - LEVEL_TAG_GAP(4) - levellabel.width()/2`，
   y 与名称相同（`absp.y()`），实现脚底同一水平线并排、等级在前。
   名称档位为 0 时两块名牌一并隐藏（等级属名称信息）。
3. **等级差染色**（`Mob::show_hp()`）：首次受击按 `playerlevel - level` 给名称染黄/红时，
   同步给 `levellabel.change_color(...)`，保持"等级差提示"语义一致；等级牌颜色随名称持久，
   与既有 namelabel 行为一致。

### 3.3 模块与扩展点

- `Mob.h`：`Text namelabel` 旁新增 `Text levellabel`（独立成员，便于后续单独显隐/染色）。
- 名牌间隙常量 `LEVEL_TAG_GAP = 4`（`Mob.cpp` `draw()` 内 `constexpr`），可按实机观感微调。

## 4. 资源与同步

- 无新增/修改资源，无多语言/多端目录同步需求；`Lv:` 为代码字面量（与 `show_hp` 等级差色一样属通用符号，无 i18n 词条）。
- 无密钥/编码/版本兼容问题。

## 5. 编码与 i18n

- 源码 UTF-8（工程已开 `/utf-8`）；等级文本为 ASCII 数字拼接，无 GBK 风险。

## 6. 产物清单

| 类型 | 路径 | 说明 |
| --- | --- | --- |
| 文档 | `README.md` | 本文件 |
| 证据 | `evidence/change-summary.md` | 改动前链路取证摘要 + 构建/部署校验 |

## 7. 验证记录

- 构建：`tools/build_smoke.bat` → `EXITCODE=0`（2026-11-21，GATE OK）。
- 部署：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`，Size 5124096，LastWriteTime 2026/11/21 8:31:27（随构建刷新）。
- lint：`Mob.h`/`Mob.cpp` 无新增诊断。
- 实机：待用户启动客户端进图（如魔法密林 101000000）确认两块名牌底部并排、间距与底框观感。

## 8. 预留、风险与回滚

- 风险：名牌间隙 4px 为估值，NAMETAG 底框左右外扩约 1~3px，极端长名可能使左侧等级牌偏出怪物中心较多；
  仅视觉参数，改 `LEVEL_TAG_GAP` 即可。
- 回滚：删除 `Mob.h` 的 `levellabel` 成员与 `Mob.cpp` 三处接线（构造/draw/show_hp）即恢复原状，无协议/资源副作用。
