# 玩法家族轮 J1–J8（三家族创建 / 怪物加载 / 技能数据 / 出站封包 / buff 掩码 / 战神连击）

> 状态：**代码全部落地并构建通过、可自证项已真机自证；6 项需真人角色/队友的验收待用户**
> 分支：`feature/dev_0.0.1`，拆分提交 `169ea09..97954a2`（28 个），已推 `origin`
> 构建身份：md5 `9636f77b4030ab4d9a85379cb0694911`、`4,419,584 B`、`2026-11-01 14:16:01`（全量 `-t:Rebuild`，见 r807）
> 协议基准：**BeiDou-Server_s9**（`org.gms`，opcode 矩阵基线 `44d0d5d9ca`）。本轮**未修改服务端**。

## 1. 需求与范围（用户原话拆出来的三件事）

1. 各职业**转职、技能释放、特效、伤害**逻辑是否正确；
2. **怪物 / boss 的加载、刷新、攻击**是否正确；反应堆、事件是否正确；
3. **骑士团、冒险家、战神**三家族的**创建、任务、转职、技能**是否有问题。

已拍板的范围（三项都取了最宽档）：

| 决策 | 结论 |
| --- | --- |
| 家族范围 | 三家族全开（含服务端两个闸门开关的评估） |
| 技能修复面 | 命名空间 + 攻击位 + 职业名表 |
| 战神连击 | 含动画与姿态一起补 |

**非目标**：不引入外部 `Skill.wz` 资产、不做掉落物归属（暴怒炸金币）、不补 `UIComboCounter` 美术。

## 2. 最终实现逻辑

### 2.1 J1 怪物根本不出现（`54acd5b`）

- 根因在**收包状态块长度**：`Net/Handlers/MapObjectHandlers.cpp` 的 `SpawnMobHandler` 原样继承了
  HeavenMS/v83 的 `recv.skip(22)`，而 s9 的 `spawnMonsterInternal` 走 `writeLongEncodeTemporaryMask`
  （4 个 int = **16 字节**）之后才写 pos。多跳 6 字节 ⇒ 后续字段整体错位，handler 尾部的 `spawn()` 不执行。
  `SPAWN_MOB_C`（`requestController` 分支）同理。两处均改为 `recv.skip(16)`。
- 动画帧兜底（`Gameplay/MapleMap/Mob.cpp`）：挨打/死亡帧并不总叫 `hit1`/`die1`。抽样 2053 个 img，
  `die1` 只有 1608 个，另有 `die`(4)、`die2`(11)、`dieF`(9)；`hit1` 缺 476 个，其中有 `hit`(7)。
  ⇒ 补 `hit1→hit`、`die1→die/die2/dieF` 回退。
  **准确说法**：空节点会构造出「只有一张空白帧」的 `Animation`，`update()` 当场返回结束，
  表现是挨打无反馈、死亡瞬间消失——**不是崩溃**（拆提交时 `54acd5b` 的信息写成「首帧为空即崩」，以本文为准）。
- 新增 `FieldMarker` / `MapFieldMarkers` / `MapMist` / `MapSummon`：门口店标、黑板、公会名与雾、召唤物的绘制载体。

### 2.2 J2 技能数据（`06ee330`）

- **命名空间位数**：`Util/Misc.h::skill_namespace()` —— Skill 的目录名 = 技能 id 去掉末 4 位（不足 3 位补零）。
  7 位 id 得 3 位（冒险家 `110.img`、`211.img`），8 位 id 得 4 位（骑士团 `1000.img`、战神 `2112.img`），
  4 位新手 id 得 `000.img`。依据：s9 `client/Skill.java:37` `this.job = id / 10000` + `Skill.wz` 目录清单。
- **职业名表**：`Character/Job.cpp` 补齐（含骑士团「魂骑士/炎术士/风灵使者/夜行者」，
  对齐 s9 `i18n/message_zh_CN.properties` 的 `job.name.<id>`），修掉 `case 1100/1110/1111/1112` 落空；
  新手判定 `id == 0 || 1000 || 2000`（依据 `client/Character.java:5920 isBeginnerJob()`），
  `Level::BEGINNER` 的转职基线 `(id / 1000) * 1000`。
- **主动收窄**：`critiDamage` 一类节点在本包 `Skill.nx` 中实测不存在 ⇒ 删掉对应读取。这不是漏做（见 CHECKLIST §K.3-5）。

### 2.3 J6 buff 掩码与战神连击（`06ee330`）

- `Character/Buff.h:105-111`：`first_codes` / `second_codes` 为 `unordered_map<Id, uint64_t>`，
  `by_code(uint64_t code, bool first)` —— 掩码从 32 位扩到 **64 位**，修「队友 buff 只显示第一个」。
  位表逐位对齐服务端 `StatUpgrade`。
- 战神（Aran）：连击驱动 + 命中才 +1 / 停手 3s 归零 / 每满 10 层给一次 buff / `21100004`·`21110004`·`21120006` 终结技后清零；
  `PoleArm` 姿态动画补入。`UIComboCounter`（`:12/:26/:29`）承担计数与显示，但**本包无原生美术**（`UIWindow.img` 查无 `Combo`/`Number`），显示层是空壳。
  机制取证：[r805](../beidou-v17-parity/notes/r805-buff-mask-aran-combo.md)。

### 2.4 J7 出站攻击封包（`06ee330`）

- `Net/Packets/AttackAndSkillPackets.h` + `Net/Packets/AranPackets.h`(NEW)：charge 段、sustained skip、每目标计数 byte 三处按 s9 校准。
- 可回归的自证工具：`python tools/attack_layout_check.py` ⇒ **0/13 异常**；`--legacy` ⇒ **9/13 异常**（反向对照，证明检查有效）。
  取证：[r806](../beidou-v17-parity/notes/r806-out-attack-packet-layout.md)。

### 2.5 J5 任务与转职（`d5ecd8c` + `3071de6`）

- `Net/Handlers/QuestHandlers.cpp`：`updateQuest` 解析顺序按 s9 对齐，修进度位错位。
- `Data/QuestData.{h,cpp}`：任务名取 `info/name`（此前误用 `strsrc`），新增 `area` 字段与 `get_area()`。
- `Net/Handlers/PlayerHandlers.cpp`（在 `3071de6`）：`Stat.SKIN` 宽度改 `writeByte`。
- **骑士团四转是资源缺口，不是客户端 bug**：本包 `Skill.nx` 里 `1112.img` 等 5 个节点的 `skill` 子树是
  **278 字节的空 imgdir**，且 s9 `client/Character.java:5011` 对骑士团写死 `return isCygnus() ? 120 : 200;`
  ⇒ 客户端只能显示到 3 转；要么接受上限 120，要么外部导入 `Skill.wz`。**未伪造技能数据**。
  取证：[r809](../beidou-v17-parity/notes/r809-j5-quest-cygnus4th.md)。

### 2.6 J3 三家族创建 + J4 服务端闸门（`ea11fba` / 服务端只证不改）

- `IO/UITypes/UIRaceSelect.cpp::enter_creation()` 按 `selected_class` 路由 `UIExplorerCreation` /
  `UICygnusCreation` / `UIAranCreation`；`classic_classes[0..2] = CYGNUSKNIGHTS / EXPLORERS / ARAN`，
  与服务端 `CreateCharHandler.java:100-116` 的家族号（0=骑士团 `NoblesseCreator`、1=冒险家 `BeginnerCreator`、
  2=战神 `LegendCreator`）一致。
- 闸门配置在 **MySQL `game_config` 表**（不是 yaml），`GameConfig` 只在 bean 构造时读一次。
  真机取证：三键均已为 `true`，且 `game_config` 最后写入 `09:33:16` 早于 `Started ServerApplication` `09:36:47`
  ⇒ **已在运行进程内生效**。**本轮未执行任何 UPDATE，也不需要重启**（依据与「为什么不做」见
  [r808](../beidou-v17-parity/notes/r808-j4-server-job-switches.md)）。
  注意：今后若用裸 SQL 改 `game_config`，必须重启进程或走 `ConfigService.updateConfig()` 才生效。

### 2.7 J8 构建门禁（`r807`）

- 凡改动**公共类头文件的成员布局**（`Attack.h` 新增 `Point<int16_t> origin`、`PhysicsObject.h`、`Foothold.h` 等），
  增量构建不会重编依赖它的 TU ⇒ 登录阶段 100% AV 且构建 **0 error**。
  本轮脏 obj 计数 `200/274` ⇒ 强制 `-t:Rebuild` 后 `0/274`，0 error，门禁 `GATE OK` + `EXITCODE=0`。
- **MSYS 参数陷阱**：Git Bash 里 `/t:Rebuild` 会被路径转换 ⇒ `MSB1008: 只能指定一个项目`，构建根本没跑。必须写 `-t:Rebuild`。
- 后台任务尾部的 `echo "EXITCODE=$?"` 会掩盖失败，**以 `build_log.txt` 为准**，不要信包装器的状态。

## 3. 验证状态

| 类别 | 内容 |
| --- | --- |
| 已自证（不用再确认） | 登录进图不崩；怪物出现且带 `100/100` 血条会动；`attack_layout_check` 0/13（legacy 9/13）；构建门禁 GATE OK + 0 error |
| 只剩你能验（6 项） | K2-1 战神连击、K2-2 组队 buff 全显示、K2-3 魔法盾/抵抗不串号、K2-4 Hyper Body HP%/MP%、K2-5 四条 J7 伤害路径、K2-6 三家族创建点穿 |
| 明确边界（不是 bug） | 暴怒不炸金币堆（缺掉落归属）；一次攻击最多报 15 怪（格式上限）；连击窗无美术；`PYRAMID_PQ` 符号名误导但位号对；J2-③ 主动收窄 |

细则与判据在 [CHECKLIST §K](../ui-v83-window-restore/CHECKLIST.md)。

## 4. 产物索引

| 类型 | 位置 |
| --- | --- |
| 逐轮取证笔记 | `docs/features/beidou-v17-parity/notes/` 的 r805–r810（buff 掩码/连击、出站封包布局、Rebuild 门禁、服务端闸门、任务与四转、提交台账） |
| 统一验证清单 | `docs/features/ui-v83-window-restore/CHECKLIST.md` §K |
| 构建身份 | `docs/features/s9-protocol-align/builds/exe-stamps.md` + r807 表 |
| 客户端路径记忆 | `docs/features/beidou-v17-parity/notes/client-path.md` |
| 协议门禁数据 | `docs/features/s9-protocol-align/protocol/opcode-matrix.json`（与 `tools/gen_opcode_matrix.py` 同源） |
| 提交台账 | [r810](../beidou-v17-parity/notes/r810-commit-split-ledger.md) |

> 说明：J 段的机制取证**并入既有的 `beidou-v17-parity` 轮次笔记**（保持时间序列可查），本目录只做「最终实现逻辑 + 索引」，
> 不复制笔记正文。原计划把归档独立放 `docs/features/job-families/`，落地时按轮次连续性选择了前者。

## 5. 风险与回滚

- 全部改动为**纯客户端**（除 J4 只读取服务端库、未写入）。回滚单个功能可按上表的 sha `git revert`：
  `54acd5b`(J1) / `06ee330`(J2·J6·J7) / `d5ecd8c`(J5) / `ea11fba`(J3) / `3071de6`(注册与 SKIN)。
- 回滚后需 `-t:Rebuild` 重新构建（头文件成员布局已被改动）。
- 未验证项在真人验收前不应视为「已上线」：J7 四条伤害路径、J6 连击、J2 转职名表都还没跑过对应用职业的真机。
