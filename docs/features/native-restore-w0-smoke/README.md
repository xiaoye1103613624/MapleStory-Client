# W0 — 冒烟解锁（原生还原前置）

## 交付
1. **A′** `UIAdmin` SoftKey：`summon <mobid> [n]` / `exp <n>`（已有包，补全 tip）
2. **B′** s9 `CommandsExecutor` 恢复 `registerLv0–Lv6`；`CommandService.loadCommands` 空库时**不再 clear**内置表
3. **C′** 测试角换图：本机无 `mysql` CLI；优先服务端重启后用 `!warp`（B′）换有怪图，免 DML。

## 服务端生效
改的是 Java 源码，需重新编译/启动 `BeiDou-Server_s9` 后 `!warp` 等命令才进内存。

## 客户端
`tools/build_smoke.bat` 验证 UIAdmin tip 变更。
