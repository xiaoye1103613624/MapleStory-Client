# s9 协议对接 · 最终实现逻辑（收尾文档）

> 范围：`E:\project\MapleStory-Client`（C++ 客户端）/ 规范源 `E:\project\BeiDou-Server_s9`（s9，v83，opcode 权威源 R3）。
> 完成日期：2026-11-02 ｜ 状态：**c2s + s2c 全量对齐，唯一真实 opcode 缺陷已修复并构建验证通过。**

## 1. 总体结论

| 维度 | 结果 |
|---|---|
| c2s opcode（客户端→服务端） | 103/103 全部与 s9 `RecvOpcode` 值对齐 |
| s2c opcode（服务端→客户端） | 299/299 已发 opcode 均有客户端 handler 对齐 |
| 字段级（代表性） | S10 现金商城 9 子命令、S1 登录链路、S9 社交包、RPS/新年卡/Hammer 等逐项验证对齐 |
| opcode 撞车缺陷 | **1 处已修复**（152 号 `WARN_MESSAGE`×`ARIANT_RESULT`） |
| 构建门禁 | `build_smoke.bat` → `GATE OK / EXITCODE=0`，opcode 矩阵与两侧源码持续一致 |
| REA/IDA 原生验证 | 原生 `BeiDou.exe` 为启动器，游戏逻辑在 `Gr2D_DX8.dll`（未加载于 REA）；opcode 真值以 s9 为准，原生引用 `CashShop.img` 已确认含商城功能 |

## 2. 修复记录（累计 5 处 opcode 级修正）

| # | opcode | 修正 | 性质 | 来源 |
|---|---|---|---|---|
| 1 | `STORAGE` | 309 → 62 | 发包误抄 s2c 值，改回 s9 `RecvOpcode` | F5（T6） |
| 2 | `PLAYER_INTERACTION` | 314 → 123 | 同上 | F5（T6） |
| 3 | `CASHSHOP_OPERATION` | 325 → 229 | 同上 | F5（T6） |
| 4 | `BUDDYLIST`(c2s) | 63 → 130 | c2s 对齐 s9 `RecvOpcode.BUDDYLIST_MODIFY=0x82=130`；s2c 收包侧保留 63 对齐 s9 `SendOpcode.BUDDYLIST=0x3F=63`（v83 收发双枚举非对称） | 本次（2026-11-02） |
| 5 | `WARN_MESSAGE`(s2c 152) | 删除枚举+注册 | s9 无 `WARN_MESSAGE`，与 `ARIANT_RESULT=152` 撞车致 Arian 结算被错路由；删废弃注册，152 归 `ARIANT_RESULT` | 本次（T14b，2026-11-02） |

> 注：好友 `BUDDYLIST` 的 c2s=130 / s2c=63 非对称是 v83 标准形态，已核对 s9 两端吻合，仅改发包侧一处（见 `s9-social.md` §2.8）。

## 3. 切片交付物（notes/）

- `s1-login.md` / `s2-charselect.md` / `s3-enterfield.md` / `s4-stat.md` / `s5-movement.md` / `s6-item.md` / `s7-skill.md` / `s8-npc.md` / `s9-social.md` / `s10-cash.md` / `s11-misc.md` —— c2s 11 个切片，opcode 与字段级对账。
- `t14b-s2c-scan.md` —— s2c 全量扫描：矩阵状态 `HANDLED=299 / UNHANDLED_EMITTED=3 / DEFINED_NOT_EMITTED=9`，撞车缺陷已修复。
- `opcode-diff.md` / `transport-vectors.md` / `log-pairing.md` / `probe.md` / `charset.md` / `client-path.md` / `real-session-1.md` / `crash-chatbar.md` / `gap-inventory.md` —— 工具/取证/归因文档。
- `enterfield-exp-misalign.md` —— **进图白屏 / 小地图失效根因**（2026-11-05 实机负对照定案）：s9 `addCharStats` 用 `writeInt(exp)=4B`+尾部 `writeInt(0)`，客户端按 `read_long(exp)=8B`（S10 口径）致字段平移 4B、mapid 读成 16 → `map data missing` → 白屏；小地图另叠「世界坐标被 2048 门禁误杀」。6 文件修复 + 实机验证闭环，附防复发判据。**修正了 `s3-enterfield.md` §2.2/§4 的「SET_FIELD 无字段级错配」旧结论。**

## 4. 已知非致命缺口（非协议错配，不需改协议层）

- **历史功能缺口（现多已接）**：`GUILD_BOSS_HEALER_MOVE(287)` / `PULLEY(288)` 已由 `MapGuildBoss`+`UIGuildBoss` 承接（见 `guild-boss-hud/`）；仍待特性闭环的主要是 `WORLD_MAP_PLAYERS(376)`。属特性开发，不在本协议对齐切片范围。
- **9 个 s9 定义但从不发送的 s2c**（死 opcode，无害）：`ZAKUM_SHRINE` 等（见 `t14b-s2c-scan.md` §6）。
- **s9 自身 `RecvOpcode` 撞名** `WEDDING_TALK`/`WEDDING_TALK_MORE=139`：服务端内部问题，与客户端无关，不改。
- **客户端 `client_in` 35 号别名** `RECALCULATE_STATS`/`FORCED_STAT_RESET`：v83 命名冗余，35 仅注册 `ForcedStatResetHandler`，功能正确（非缺陷，见 `t14b-s2c-scan.md` §3）。

## 5. 方法学（已固化）

1. **opcode/协议以 s9 为准（R3）**：原生 `BeiDou.exe` 连的就是 s9，故"匹配 s9" = "匹配原生"，无需用 REA 反推原生 opcode。
2. **功能/UI 逻辑以原生优先**：涉及商城 UI、好友渲染等实现，先以原生 `BeiDou.exe`（REA）+ 运行时 `wz_dump`/img 数据（R1/R2/R5）为参照，其次扩展。
3. **门禁防漂移**：`opcode-matrix.json` 由 `tools/gen_opcode_matrix.py` 从两侧源码生成，`build_smoke.bat` 在 MSBuild 前校验矩阵与源码一致（`GATE OK` 才放行）。

## 6. 调试日志清理（WRAP）

- 已移除 `Net/SocketPosix.cpp` 的 `[DBG-SOCK]`：原每次建连无条件写 `_socket_debug.log`，已按注释"联调通过后整段移除"清理。
- `Util/Resource/ResourceProbe.cpp` 的 `[DBG-*]` 走 `--resource-probe` 诊断工具自身的 `Report` 输出（仅该 dev 模式触发，非常规游戏常开），保留。
- 其余 `[DBG-*]`（MapleStory.cpp/Char.cpp/BodyDrawInfo.cpp/GraphicsGL.cpp 等）注释已标注"闭环后不再落文件（no-op）"，无运行时写文件。
- 保留可配置上线级输出 `debug.packet_dump`（`Session.cpp`，由 `xy-config.yaml` 开关）。

## 7. 回归与遗留

- 所有协议层改动仅触及 `Net/OutPacket.h`（枚举值）、`Net/PacketSwitch.cpp`（收包注册）、`Net/Packets/*`（字段对齐），未触碰服务端、未改 `Data/*.img`。
- 本次新增实际代码改动：`BUDDYLIST 63→130` + `BuddyPackets.h` 注释；`WARN_MESSAGE` 撞车移除。构建 `GATE OK`，无回归。
- 未做：3 个 UNHANDLED_EMITTED 的功能实现（属特性开发，需补 UI/逻辑）、input 侧中文输入（T13，按决策"先显示后输入"预留）。
- **2026-11-05 回归修正**：`SET_FIELD` 存在字段级错配（s9 `addCharStats` 的 exp 宽度，非 opcode 名值），导致进图白屏/小地图失效；`LoginParser.cpp` 等 6 文件已修复并实机验证闭环，详见 `notes/enterfield-exp-misalign.md`。opcode 矩阵门禁（§5.3）持续 GATE OK，不受影响。
