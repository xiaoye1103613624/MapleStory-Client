# 待扩展功能记录（deferred features）

> 决策（2026-11-02，用户）：s9-protocol-align 协议层已 100% 对齐、0 缺陷。剩余 4 项为**功能缺口/冗余发包**，非协议错配。
> 处理原则：
> 1. 原生（BeiDou）**有**的功能 → 后续对照 REA 原生完善开发；
> 2. 原生**没有**的 → 忽略；
> 3. 全部**先记录**，等基础功能完善后再做扩展功能开发。
> 本次**不改动任何代码**。

## 1. LOGIN_EMAIL @ opcode 3（c2s：客户端发、s9 无 handler）
- **功能**：账号邮箱包。v83 某些版本在登录链路里客户端上报账号注册邮箱（用于账号找回 / 邮件验证 / PQ 邮箱登记）。当前 s9 `RecvOpcode` 的 opcode 3 **未注册任何 handler**，服务端静默丢弃未知 opcode。
- **原生证据**：REA 搜 `BeiDou.exe` 字符串 `email`（不区分大小写）= **0 命中** → 原生无 email 功能痕迹。
- **决策**：原生没有 → **忽略**（保留客户端发包不动，无害；不实现、不删除）。

## 2. WORLD_MAP_PLAYERS @ 376（s2c：s9 发、客户端无 handler）
- **功能**：世界地图上显示其他玩家 / 角色位置标记。
- **原生证据**：REA 搜 `BeiDou.exe` `WorldMap` = **3 命中**（`p/WorldMap/WorldMap%d.img`、`Map/WorldMap/`、`Map/MapHelper.img/worldMap/npcPos`）→ 原生**有世界地图功能**。
- **决策**：原生有 → 扩展阶段**对照 REA 开发**（需实现世界地图 UI + 玩家标记渲染）；当前仅记录、不实现。

## 3. GUILD_BOSS_HEALER_MOVE @ 287（s2c：s9 发、客户端无 handler）
- **功能**：家族 Boss（扎昆 / 暗黑龙王）PQ 里治疗师 NPC 的位置移动同步。
- **原生证据**：REA 搜 `BeiDou.exe` `GuildBoss` = **0 命中**。注意游戏逻辑在 `Gr2D_DX8.dll`（未加载于 REA），EXE 无痕迹不能直接定论 DLL 内部，但倾向原生未强调该子功能。
- **决策**：记录、延后；扩展阶段如需实现，先以 REA 加载游戏 DLL 还原治疗师移动逻辑再开发。

## 4. GUILD_BOSS_PULLEY_STATE_CHANGE @ 288（s2c：s9 发、客户端无 handler）
- **功能**：家族 Boss PQ 里滑轮（pulley）状态变化同步。
- **原生证据**：同 #3（`GuildBoss` 无 EXE 痕迹）。
- **决策**：同 #3，记录、延后、扩展阶段 REA 还原后开发。

## 汇总
- **协议层结论**：已全量对齐（c2s 102/103 + 1 冗余发包；s2c 299/311 + 3 缺口 + 9 死 opcode），**无需更改任何 opcode 真值**。
- **本次动作**：仅记录，零代码改动，构建 `GATE OK` 状态不受影响。
- **扩展功能开发（#2/#3/#4）**：待基础功能完善后，按"对照 REA 原生"原则立项，届时先用 REA 加载游戏 DLL 还原原生逻辑再着手。
- **9 个 `DEFINED_NOT_EMITTED` 死 opcode**（ZAKUM_SHRINE/ADMIN_SHOP/TOURNAMENT/各 CASHSHOP_*_RESULT 等）：s9 定义但从不发，客户端亦无 handler，完全无害，无需处理。
