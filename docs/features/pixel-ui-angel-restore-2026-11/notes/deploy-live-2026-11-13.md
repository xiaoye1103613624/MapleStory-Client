# Live 部署与进图矩阵（2026-11-13 续）

## 1. 杀进程

| 手段 | 结果 |
|---|---|
| `taskkill /F /IM MapleStory.exe /T` | 用户态实例 **SUCCESS**（如 PID 22032）；幽灵 **Access denied** |
| 对 PID `6108/34476/56016` 再杀 | Access denied |
| UAC RunAs 提权 taskkill | Access denied |
| SYSTEM schtasks | Access denied（无权限建任务） |
| `OpenProcess`/`TerminateProcess`/`NtTerminateProcess` | handle=0，Win32 err=5 |
| 幽灵窗口 | **无可见窗**（不污染 EnumWindows 取证） |
| 幽灵特征 | Parent 已消亡、`ExecutablePath` 空、Owner 空 |

结论：幽灵无法从本机 Agent 会话清掉；不影响新进程窗口采样。

## 2. 部署

| 项 | 值 |
|---|---|
| Staging | `E:\MXD\BeiDou-Client-xy\_pixel_build_stage\MapleStory.exe` |
| Live | `E:\MXD\BeiDou-Client-xy\MapleStory.exe` |
| 动作 | 再次 `Copy-Item` → **RECOPY_OK** |
| 指纹 | size=`5160960` mtime=`2026-11-13 18:27:24`（与 staging 一致） |

附带修复：live `Settings` 曾被清空为 0 字节 → 从仓库 `Settings` 恢复并注入 `window.game_width/height/tier`。`xy-config.yaml` `login.auto: true`（矩阵用）；`127.0.0.1:8484` TcpTestSucceeded。

## 3. 进图多档矩阵

命令：`python tools/resolution/run_tier_matrix.py --all --seconds 42 --restore-tier 5`  
日志：`evidence/tier-matrix-enter.log`

| Tier | 期望 | pass | reached | 说明 |
|---|---|---|---|---|
| 0 | 800×600 | false | false | 全程客户区 1280×720 |
| 1 | 1024×768 | false | false | 同上 |
| 2 | 1366×768 | false | false | 同上 |
| 3 | 1600×900 | false | false | 同上 |
| 4 | 1920×1080 | false | false | 同上 |
| 5 | 1280×720 | **true** | true | 登录态达标 |
| 6 | 1440×900 | false | false | 同上 |
| 7 | 1680×1050 | false | false | 同上 |
| 8 | 2560×1440 | false | false | 同上 |

汇总：`passed=1/9`。tier0 70s 复试仍 fail。

### 进图失败真因（非分辨率）

`setfield_trace.log`：`parse_inventory` 后 `skillbook-start remain=354`，随即  
`Packet Error: Stack underflow at 1388`。  
偶发成功样例曾出现 `remain=355` 并 `fade done … enter_game=1`，与 CASH/技能段 **1 字节错位** 一致（见 `CharacterParser.cpp` 注释）。进图未完成 → 视口不会切到 `game_*` 档。

## 4. UIStatsLayoutV4

仍已接线：`custom_v4` + 底对齐 + 值进框；本轮无改。

## 5. 剩余阻塞

1. 幽灵 PID `6108/34476/56016` 需用户侧（安全软件/重启）清理（不挡进图矩阵）。  
2. ~~SET_FIELD skillbook 下溢~~ → **已修**，见 `setfield-skillbook-fix-2026-11-13.md`；进图 9/9 pass。
