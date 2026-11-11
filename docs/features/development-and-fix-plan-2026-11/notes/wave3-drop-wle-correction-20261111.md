# round90 · drop 面 W/L/E 误分类纠偏（只读取证 + 零行为注释）

> 触发：m10601「还有未完成的或者你可以自测的任务吗？」
> 授权：m09211「全部继续，如果能自测的可以自己启动测试」+ m09729/m09737「按最完全反编译还原基础功能、其余功能同方向开发修复完善」
> 权威源：Angel(13339) 083/v83 带符号 IDB；s9 服务端源码仅约束 W 桶字段宽度。

## 1. 为什么先复核而不是直接按 §5 开工

round89 `WLE-TRIAGE-PLAN.md §5` 把「下一批（drop 面）」写成
「`CDropPool::OnDropEnterField` 的 animation 分支按 083 还原 → 纯逻辑补丁」，
§2.3 把 `drop/D2`（animation=0 消亡动画）+ `drop/D12`（animation=4 延时爆炸帧）
登记为 L 桶 P1。自测推进前先按门禁 18 拉活 IDA 对账，发现**函数锚点、桶归类、可行性三处都错**：

- 两条决策点都在 **269 离场** `CDropPool::OnDropLeaveField @0x506590`，
  `CDropPool::OnPacket @0x5058db` 逐字 `a1==268 → OnDropEnterField` / `a1==269 → OnDropLeaveField`；
  268 进场 `OnDropEnterField @0x505900` 管的是掉落物**生成**（`v108=Decode1(mode)` 等），不是离场动画。
- `drop/D12` 改回 083 会改 wire 读取宽度 → 属于 W 桶硬约束，不是 L 桶纯逻辑。
- `drop/D2` 缺的是 COM 渲染栈 → 属于 F 维 P2 资源层，不是逻辑分支。

## 2. 三方逐字证据

### 2.1 Angel 083 活反编译 `tools/_ida_r90/OnDropLeaveField.code.txt:83-99`

```
  v2 = CInPacket::Decode1(this: a2); /*0x5065b0*/
  v71 = CInPacket::Decode4(this: a2); /*0x5065bc*/
  if ( v2 == 2 || v2 == 3 || v2 == 5 ) /*0x5065e4*/
    *(v7 + 88) = CInPacket::Decode4(this: a2); /*0x506627*/
  else if ( v2 == 4 ) /*0x5065eb*/
  {
    v5 = CInPacket::Decode2(this: a2); /*0x5065f8*/
    *(v4 + 68) = get_update_time() + v5; /*0x506602*/
    *(v79 + 72) = 4; /*0x506608*/
  }
  ...
  if ( v2 == 0 ) /*0x506647*/  → IWzGr2DLayer::GetCanvas / RemoveCanvas / Animate
```

### 2.2 s9 `PacketCreator.java:2624-2642`

```
public static Packet removeItemFromMap(int objId, int animation, int chrId, boolean pet, int slot) {
    OutPacket p = OutPacket.create(SendOpcode.REMOVE_ITEM_FROM_MAP);
    p.writeByte(animation);
    p.writeInt(objId);
    if (animation >= 2) {
        p.writeInt(chrId);
        if (pet) { p.writeByte(slot); }
```

`animation>=2` **恒写 4B cid**；083 仅 `v2==4` 读 **2B delay** 并记账到 `+68/+72`。
数值同为 4、语义完全不同 → 数值重合≠语义重合。

### 2.3 当前端 `Gameplay/MapleMap/Drop.cpp:119-144`

`case 2 / 4 / 5` 同组 `PICKEDUP + vspeed=-4.5f`，`case 0` 仅 `PICKEDUP`（48 帧淡出）；
`Net/Handlers/MapObjectHandlers.cpp` 的 `RemoveLootHandler` 在 `mode > 1` 读 `int cid`。
与 s9 逐字段对齐，**按 s9 读 int 是正确解**。

## 3. 判定与闭合

| 项 | round89 | round90 | 依据 |
| --- | --- | --- | --- |
| `drop/D12` | L 桶 P1 | **W 桶**（保持 s9，零改动） | 083 2B delay vs s9 4B cid；改回读 short 平移后续字段、复现 `LoginParser.cpp:150-174` 白屏类错位 |
| `drop/D2` | L 桶 P1 | **F 维 P2**（不补） | `IWzGr2DLayer` COM 画布栈当前端没有；业务结果（消失）一致 |
| 锚点 | `OnDropEnterField @0x505900` | `OnDropLeaveField @0x506590` | `OnPacket @0x5058db` 268/269 分派 |

**reopen 27 闭合**：`27 = 25 P1 + 1 W + 1 F-P2`。
`DECISIONS.md` 168 条历史裁定与 `WLE-TRIAGE-PLAN.md §4` 的 `38+130=168` **不动**——
round88 四分类是「推荐默认文本」归类，round90 是「物理 wire 事实」判定，两层不互相覆盖，
只在 P1 登记层注销 2 条，不做语义再分类造假。

## 4. 同族审计（一类问题出去）

§2.3 其余候选 `quest/D8`（`sub_721008` 条件分支）、`skill/D4`（`CWndSkillGuide` 子窗）、
`corewindow/D8`（`CUIStat` 6 掩码 / `CUISkill` 500ms 门闩 / `CUIKeyConfig` 5 分支）
均为状态机/条件分支、不碰 wire，属**合法 L 桶候选**，无同族误分类。
下一批改走这三条，每批先出 083 符号 + 反编译锚点，证据不足登记 P2/P3，门禁 5 禁想象修改。

## 5. 改动清单与限制

- `WLE-TRIAGE-PLAN.md` §2.3 / §5 措辞纠偏 + 追加 §7 纠偏段。
- `evidence/wle-triage-round89.json` 顶层追加 `round90_correction`（历史 `p1_registered` 不删，标记 superseded）。
- 新增 `evidence/drop-wle-correction-round90.json`、本 notes、dev-plan §4 `2-38`、PLT §48-4 标记 + §49。
- `Gameplay/MapleMap/Drop.cpp::expire` 前补三方对账**纯注释**（CRLF 字节级保持，0 行为字面量）。
- 零 wire / 零协议 / 零 WZ / 零新封包；25 个外部程序在途 WIP 原样留工作树、精确 add 隔离。
- 真机五屏截图 60 节/142 处仍由用户认领。
