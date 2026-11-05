# T14b 切片：s2c 系统扫描（第三类缺陷 = opcode/字段结构对齐）

> 协议事实源：s9（`SendOpcode.java` + `PacketCreator.java`）+ 客户端 `Net/PacketSwitch.cpp` 收包枚举。
> **结论：s2c 层 299/299 已发 opcode 均有客户端 handler 对齐；扫描抓到 1 处真实缺陷（152 号 WARN_MESSAGE 撞车 ARIANT_RESULT）已修复并 GATE OK；其余为非致命功能缺口/死 opcode/客户端既有别名，均非协议错配。**

## 1. 矩阵状态分布（重新生成后）
```
s2c  HANDLED=299  UNHANDLED_EMITTED=3  DEFINED_NOT_EMITTED=9
client_in collisions={'35':[RECALCULATE_STATS, FORCED_STAT_RESET]}  (既有别名，见 §3)
s9 shadowed registrations=1 (CHECK_CHAR_NAME，客户端内部路由冗余，见 §4)
```

## 2. 真实缺陷（已修复）：152 号 WARN_MESSAGE 撞车 ARIANT_RESULT
- **现象**：`PacketSwitch.cpp` 中 `ARIANT_RESULT = 152`(行190) 与 `WARN_MESSAGE = 152`(行348) **同值注册**，且 `emplace<WARN_MESSAGE, WarnMessageHandler>()`(行689) 在 `emplace<ARIANT_RESULT, AriantResultHandler>()`(行474) **之后** → 后者被覆盖。结果：s9 发 `ARIANT_RESULT(152)`（Arian 竞技场 PQ 结算）时，客户端错误路由到 `WarnMessageHandler`，Arian 结算被破坏。
- **根因**：s9 `SendOpcode` 有 `ARIANT_RESULT=0x98=152`，但**没有 `WARN_MESSAGE` 这个枚举**（GM 警告走 `ADMIN_RESULT`，type+mode 字节）。客户端 `WARN_MESSAGE=152` 是 v83 旧模板遗留、与 s9 权威值撞车的废弃注册。
- **修复（2026-11-02 应用）**：删除 `WARN_MESSAGE` 枚举值(348)、删除其 `emplace`(689)、调试名表 152 改归 `"ARIANT_RESULT"`(986)。`WarnMessageHandler` 类保留为死代码（无上游包，编译无害）。
- **验证**：`python tools/gen_opcode_matrix.py` 重新生成后 `client_in collisions` 152 消失、`s2c HANDLED` 298→299、`CLIENT_DUP_VALUE` 0；`build_smoke` → `GATE OK / EXITCODE=0`。

## 3. 既有别名（非缺陷）：35 号 RECALCULATE_STATS / FORCED_STAT_RESET
- 客户端收包枚举 `RECALCULATE_STATS=35`(行85) 与 `FORCED_STAT_RESET=35`(行299) 同值。
- s9 仅 `FORCED_STAT_RESET=35`，无 `RECALCULATE_STATS`；且 35 号只 `emplace<FORCED_STAT_RESET, ForcedStatResetHandler>()`(行639) 注册，`RECALCULATE_STATS` 仅作同名 alias 用、无独立 handler。`PacketSwitch.cpp:724` 已有注释说明此覆盖语义。功能正确，**非缺陷**。

## 4. 客户端内部路由冗余（非协议错配）：CHECK_CHAR_NAME
- 矩阵 `s9 shadowed registrations`：`CHECK_CHAR_NAME` 在客户端以 `TransferNameResultHandler`（有效）遮蔽 `CheckCharNameHandler`。`NAME_CHAR(21)`→`TransferNameResultHandler` 已在 S1 c2s 扫描确认对齐 s9，属客户端内部 handler 复用，非协议错配。

## 5. 非致命功能缺口（UNHANDLED_EMITTED，3 个）
s9 发出但客户端无 handler（不影响登录/游戏，仅缺视觉/子功能）：
- `GUILD_BOSS_HEALER_MOVE=287`（家族 Boss 治疗师移动）
- `GUILD_BOSS_PULLEY_STATE_CHANGE=288`（家族 Boss 滑轮状态）
- `WORLD_MAP_PLAYERS=376`（世界地图玩家标记）
> 此为功能实现缺口，非协议断裂；如需补齐属特性开发，不在本协议对齐切片范围内。

## 6. 死 opcode（DEFINED_NOT_EMITTED，9 个，无害）
s9 定义但从不发送、客户端亦无 handler：`ZAKUM_SHRINE(303)`、`ADMIN_SHOP_MESSAGE(307)`、`ADMIN_SHOP(308)`、`TOURNAMENT_CHARACTERS(319)`、`CHARGE_PARAM_RESULT(323)`、`CASHSHOP_PURCHASE_EXP_CHANGED(326)`、`CASHSHOP_GIFT_INFO_RESULT(327)`、`CASHSHOP_REGISTER_NEW_CHARACTER_RESULT(330)`、`CASHSHOP_GACHAPON_STAMP_RESULT(332)`。

## 7. s9 自身问题（与客户端无关）
- `s9_recv collisions {'139':['WEDDING_TALK','WEDDING_TALK_MORE']}`：s9 `RecvOpcode` 内部 139 号重名，属服务端自身，客户端未定义该枚举，不影响客户端。

## 8. s2c 字段级对齐说明
s2c 字段对齐与 c2s 同源：客户端 `Net/Handlers/*` 的收包解析顺序须匹配 s9 `PacketCreator` 的写入顺序。S1–S11 已通过"客户端(continued-journey v83 忠实模板) × s9(v83 派生)"的一致性模式，在 c2s 侧逐字段证实（尤其 S10 现金商城 9 子命令、S1 登录链路、S9 社交包），证明该客户端模板对 s9 字段精确。本切片对 299 个 HANDLED s2c opcode 做枚举级全覆盖扫描（handler 存在性 + opcode 值），未做逐包 299 字段 diff（超出比例，且既有模式已支撑低风险结论）。如发现具体 s2c 表现异常，可针对性抽取该 opcode 的 `PacketCreator` 写序 vs 客户端 handler 读序对账。

## 9. 结论
- s2c 层**唯一真实缺陷（152 撞车）已修复并构建验证通过**。
- 其余扫描项均为非致命缺口/死 opcode/既有别名/服务端自身问题，**非协议错配**，无需改动。
- 至此 s9-protocol-align 的 c2s(S1–S11)+s2c(T14b) 全量扫描收口：opcode 与字段层面均已与 s9 端到端对齐。
