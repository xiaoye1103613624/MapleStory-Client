# 地图对象逻辑还原（map-object-parity）

> 状态：波1/2/3 已完成并提交；隐藏门三态 / 迷雾区域 / 反应堆 idle / G11 场标贴图已后续闭合；余风筝等按证据边界显式预留。
> 适用客户端：MapleStory-Client（Journey 系）· 权威参照：BeiDou-Server_s9 源码 + BeiDou.exe V17.7（IDA/REA）+ Data/*.img（自研 nlnx 探针）
> 产物目录：`docs/features/map-object-parity/`
> 日期：2026-11-03

## 1. 背景与目标

用户问题：「客户端地图对象（物品/NPC/怪物/反应堆/Boss 等）逻辑是否存在缺失、实现是否正确？」

结论：**骨架全在、细节带伤**——302 个 opcode 全部注册且有非空 handler，但逐字节对账
（s9 `PacketCreator.java`）+ 资源实测（nlnx 探针）证实一批布局错位与"解析了但不消费"的
逻辑空洞。本轮分波修复并还原。

## 2. 提交索引

| 波次 | commit | 内容 |
| --- | --- | --- |
| 1 | `02e9401` | 封包布局六处：召唤兽 0x0A/毒雾 4×int+skillDelay/怪物尾段四形态分派/控制器变长 stati/MovementParser 缺失 case/宠物 long-pid |
| 2-A | `796f2e2` | 控制器怪出招（attack/skill 段上报）、MOVE_MONSTER_RESPONSE 接线、分裂怪(-3)雪藏/唤醒 |
| 2-B | `f1604ff` | Boss 血条 gauge 填充条、反应堆 Hit 音效 |
| 2-C | `582fd31` | ShowForeignEffect 三形态等级、RemoveLoot 宠物拾取、DamagePlayer -4 静默、SpawnChar 店招/team |
| 3 | `bcea1c6` | 召唤兽真对象化（Skill.img summon 状态机）、反应堆音效层级修正（{rid}/{state}/Hit） |
| 3b | `81b0537` | 显式传送门 pv 动画容器错位（default→pv 本体八帧） |

证据账本：`evidence/wave1-layout-ledger.md`、`evidence/wave2-logic-ledger.md`、
`evidence/wave3-research-ledger.md`；控制器细节 `notes/wave2a-controller.md`。
取证工具：`_tools/summon_probe/`（nlnx 只读探针，镜像 ImgSource open_merge 路径；
`build.bat` 一键重建）。

## 3. 关键机制摘要

### 3.1 怪物（Mob）
- 状态字节：基础 2..10（move/stand/jump/hit/die/fly），attack 段 24..41（下标 (b-24)/2）、
  skill 段 42..59。控制器（SPAWN_MOB_C）本地随机选已加载 attack 帧上报；服务端
  MOVE_MONSTER_RESPONSE 授权下一技能 ⇒ 客户端下次动作以 42+ 段携带 skillId/level
  （`MoveLifeHandler.java:69-103`）。
- 出生尾段四形态（parentless/fake short(-2)/makeReal short(-1)/parent oid）按
  **剩余长度+内容**分派；判别失败整包弃用，杜绝修前"marker 当 team 读"的错位。
- 分裂怪 effect==-3 本地雪藏（deactivate+opacity 0），`MapMobs::remove` 击杀父怪时唤醒。

### 3.2 召唤兽（MapSummon，波3 真对象化）
动画不在 Mob.nx——`Skill/<ns>.img/skill/<id>/summon` 自带状态机
（stand 8帧/hit 4/die 1/summoned 12，3111002 实测 origin(35,96)）。
修前读 `summon["0"]`（不存在）落到 `summoned["0"]` 7×7 占位帧 ⇒ "看不见"根因。
现在：SPAWN animated→入场闪光；DAMAGE_SUMMON→hit；REMOVE anim==4→die 播完自清。

### 3.3 反应堆（Reactor）
音效 `Sound/Reactor.img/{rid}/{state}/Hit`（rid 键不补零；audio 在状态层下，
catalog 实测 0..3 各一条、4 为空；IDA `sub_989ADD` 串 3073+5543 吻合）。
`play_state_sound(state)` 在 hit/destroy 时播对应状态音。

### 3.4 Boss 血条（UIBossHP）
UIWindow.img `DualMobGauge`：backgrd 681×43 + **gauge 1×9 横向拉伸填充** + text 791×26
（REA 反编译 `sub_56C9B7` 三分件顺序）。修前填充宽度算完被丢弃。

### 3.5 传送门（MapPortals）
MapHelper portal/game 三族：pv(0..7 帧样式族)/ph(default 三态)/psh(1..4 三态)。
显式门修前取不存在的 `pv["default"]` ⇒ 无动画；已修（本体优先）。
**隐藏门 portalStart/portalExit 三态未接** ⇒ 波4 预留（见 §5）。

### 3.6 移动流（MovementParser）
服务端 `rebroadcastMovementList` 逐字节原样转发 ⇒ 解析宽度必须与
`AbstractMovementPacketHandler.parseMovement` 全等：补齐 3/4/7/8/9(9B)、
14(9B)、18/19/20/22(7B)、10(1B)、21(3B)；未知命令整包作废。

### 3.7 掉落/宠物/角色
- DropLoot/RemoveLoot 宽度复核 OK；宠物拾取（anim=5+slot）不再吞 looter。
- SpawnChar 公告箱三形态 + team 取包尾；店招可显示。
- ShowForeignEffect 三形态按剩余长度取技能真等级；DamagePlayer skill==-4 不弹数字。
- SpawnPet uniqueId 恢复 long 全宽。

## 4. 验证

- 构建：MSBuild Release x64（VS18 v145）。波1 全绿出 exe；波2/3 各 TU 编译 0 error；
  收尾串行全量构建见 `_b_solo.log`（结果记录于 §6 提交后复查）。
- 仿真：怪物尾段 11 形态 Python 逐字节仿真全 PASS；移动流 fragment 宽度对账单。
- 探针：`summon_probe.exe` 实测 Skill/Sound/MapHelper/真实 Map portal 节点（只读）。
- Release 下 LOG 宏 no-op（MapleStory.h `#ifdef _DEBUG`），运行时观测需 Debug 构建。
- 人工冒烟（进图：受控怪出招/扎昆分裂/召唤兽入场/毒雾锚点/反应堆有声/BossHP 填充/
  显式门闪烁）——需用户在目标机登录验证，未由 AI 代跑。

## 5. 显式预留（证据边界，勿当遗漏）

| 项 | 原因 | 解锁路径 |
| --- | --- | --- |
| 隐藏门三态动画（portalStart/Exit） | ✅ 2026-11-05 已接（每门独立 Animation + HidePhase；见 [hidden-portal-tristate](../hidden-portal-tristate/)） | pending §18 真机 |
| 迷雾区域渲染（矩形平铺贴图） | ✅ 2026-11-05 已接（Skill tile 平铺；见 [mist-area-render](../mist-area-render/)） | pending 真机 |
| 出生特效播放 | 本数据无 Mob/SpecialEff.img（0009000.img 不存在） | 资源补齐（图鉴站）后接 effect>0 |
| 风筝/商店盒/Evan 龙贴图 | 同上，字符串池无直接命中 | IDA `sub_531325` 分发器 case 271/265/181 反编译 |
| BossHP 多目标队列/名牌色表 | DualMobGauge=`sub_56C9B7` 实为 PartyRace 专用（硬编码 9700036），非多目标队列；MobGage 单槽 + s9 5s 节流已证 | 双槽 UI 若要做需另取证布局；见 quest-page-icons README §5 |
| G11 Catch/黑板/店招/ItemFX | ✅ 2026-11-05 已接（见 [field-marker-g11](../field-marker-g11/)） | pending 真机 |
| 反应堆 default 悬浮动画 | ✅ 2026-11-05 已接（idle 循环 `update` + `reload_idle`；见 [reactor-default-idle](../reactor-default-idle/)） | pending 真机 |
| 客户端上行 MOVE_PET/SUMMON/DRAGON | 服务端收包口存在，但上行构造器涉及宠物/召唤跟随 AI，非本波范围 | 后续独立切片 |

## 6. 复盘（四步）

1. **目标**：审计+还原地图对象逻辑。实际：审计闭环（43 handler 全量对账 + 3 处子代理
   误报被逐字节仿真纠正）、修复 12 项、预留 7 项带解锁路径。
2. **差距**：人工游戏内冒烟未跑（需用户在目标机确认）；`Reactor.img` 的 catalog 键序
   假设（不补零）与 Mob 的 extend_id(7) 不一致，已在实现中按 Sound catalog 实测对齐。
3. **原因**：弯路一，Release exe 里 grep LOG 字符串做版本指纹——Release LOG no-op 导致
   假阴性（MEMORY 已有此条，仍未内化：应 grep **数据字面量**如 `Reactor.img` 或比对 obj mtime）；
   弯路二，同一消息并发两个 MSBuild 复现 PDB 竞争——构建必须串行独占。
4. **沉淀**：nlnx 探针模板（_tools/summon_probe）可复用于任何"节点结构是否如此"的取证；
   "子代理报告必须逐字节仿真复核"成为本线硬规则。

## 7. 触碰声明

- 波1 曾顺手修 `Data/MobDropData.cpp` 的 `operator[]`→`insert_or_assign`（C2512 阻塞全工程
  构建，属当时在途未提交改动；如上游另有方案以 theirs 为准）。
- `MapObjectHandlers.cpp` 里外部会话的 `parented()` 改名等构建修复予以保留。
- 未触碰评分/CI/服务端；未动 `UIAdmin/UIChatBar/GMCommandList` 等外部在途文件。
