# 进图/切图时 NPC 从原点掉到下方平台 —— 修复记录（2026-11-20）

## 现象

进入游戏或切换地图时，NPC 出现在其声明坐标位（原点位），随后掉到下方平台。

## 根因（行号级 RCA，基于本轮工作树实测）

### 1. NPC 落点完全由 spawn 包 fh 决定
- `Net/Handlers/MapObjectHandlers.cpp:762-776` `SpawnNpcHandler`：读 `fh = recv_short()`，入队 `NpcSpawn{oid,id,position,flip,fh}`。
- `Gameplay/Spawn.cpp:38-45`：`NpcSpawn::instantiate` 走 `physics.get_spawn_point(position, fh)`，构造 `Npc(..., fh, ...)`。
- `Gameplay/Physics/FootholdTree.cpp:390-399`（修复前）：fh 不存在/竖墙时回退 `get_y_below(position)`（按坐标反查）。
- `Gameplay/MapleMap/Npc.cpp:144-145`：`phobj.fhid = f; set_position(...)`；`PhysicsObject.h:201,207` 默认 `fhid=0, onground=true`。

### 2. 反查必然选到下方平台（帮凶）
- `FootholdTree.cpp:353-374` `get_fhid_below` 判据 `comp >= ycomp && ycomp >= fy`，要求平台 `ground_y >= 包 y`。
- NPC 包的 y 槽是 wz 的 `cy`，实测比平台线低 5–17px（`FootholdTree.h:45-47` 明写）。NPC 真正站的平台 `ground_y < packet_y`，被判"不够低"跳过 → 选到**下一层平台** → 实例化即在下方平台。

### 3. fh 与当前图不匹配 —— 时序竞态，且清理刚被回退（主因）
- `SetFieldHandlers.cpp:229-230` 入口 `lock()+Stage::clear()` → `Stage.cpp:129-137` → `MapNpcs.cpp:69-73 clear()+clear_spawns()`，只在入口清一次。
- fadeout 异步：回调里 `:145 poll_events()` 会 dispatch 包，`physics` 到 `:155 Stage::load → Stage.cpp:82 load_map → :181 physics=Physics(foothold)` 才重建。窗口内入队的包带**旧图 fh**。
- `SetFieldHandlers.cpp:158-163` 的 load 后二次 `clear_spawns()` 在本轮开始时已被删除（`git blame` 为 `Not Committed Yet 2026-11-20 00:52:42`），`E:\MXD\BeiDou-Client-xy\setfield_trace.log` 实锤 `[SetField] skip post-load clear_spawns (keep new-map spawns)`。
- 结果：残留包没有二次清理，带旧 fh 在新图实例化 → 命中第 2 步回退 → 掉到下方平台。

### 4. 同一竞态的两难
- 2026-11-17 triage / commit `712cd0af` 加 load 后二次 clear 修"切图 NPC 掉落"；2026-11-20 00:52 又删掉，因为误杀 fadeout 期间提前到达的**新图**包 → "进图后 NPC 全消失"。
- 用"清/不清队列"二选一区分不了旧图残留与新图提前包。

## 修复（A+B 一次性）

核心：`get_spawn_point` 升级为 `resolve_spawn_point`，**返回坐标 + 实际 foothold id**，实例化与 load 后重锚共用，幂等双保险。

### B —— 落点解析修正（治帮凶）
- `Gameplay/Physics/FootholdTree.h`：新增 `struct SpawnPoint { Point<int16_t> position; uint16_t fhid; }`；`get_spawn_point` 改名为 `resolve_spawn_point`；新增私有 `get_fhid_at_x(double fx)`、公开 `has_foothold`/`covers_x`。
- `Gameplay/Physics/FootholdTree.cpp`：
  - `get_fhid_at_x`：按 x 取最靠上的水平平台，**完全不依赖 cy**，消除 `get_fhid_below` 的 `ground>=y` 偏差。
  - `resolve_spawn_point`：fh 有效且覆盖 x → 保留原 fh；否则回退按 x 就近选平台并**回写实际 fhid**；x 处完全无平台 → 返回 `fhid=0`。
- `Gameplay/Physics/Physics.h/.cpp`：`get_spawn_point` → `resolve_spawn_point`。
- `Gameplay/Spawn.cpp` `NpcSpawn::instantiate`：消费 `resolved.fhid` 构造 Npc（`phobj.fhid` 与落点平台一致，首帧不再坠落）；`resolved.fhid==0` 时返回 `nullptr`。
- `Gameplay/Spawn.h`：NpcSpawn 加 `get_fh()/set_fh()/get_position()`。
- `Gameplay/MapleMap/MapNpcs.cpp:update`：`instantiate` 返回 nullptr 时丢弃该包。

### A —— load 后按当前图重锚（治主因，替代一刀切 clear）
- `Gameplay/MapleMap/MapNpcs.h/.cpp`：新增 `reanchor_spawns(const Physics&)`：遍历队列，`resolve_spawn_point` 重解析——fh 落在新图则保留（新图提前包不误杀），fh 失效则按 x 重锚并回写，当前图无平台则丢弃（旧图残留）。
- `Gameplay/Stage.h/.cpp`：新增 public `const Physics& get_physics() const`。
- `Net/Handlers/SetFieldHandlers.cpp:158-163`：恢复 load 后处理，但由 `clear_spawns()` 改为 `get_npcs().reanchor_spawns(get_physics())`，日志 `[SetField] reanchor_spawns after load done`。

## 验证

- 编译：`MSBuild MapleStory.sln /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v142`，`BUILD_EXITCODE=0`，`error C` 计数 0。
  - 首次构建因天气文件 `Weather/WeatherBackLayers.cpp` 在我构建期间（01:12:32）被并发中间态修改而失败（C2511/C2597 等 5 处，均在我未触碰的文件）；源稳定后重编通过。
- 产物：`x64/out-fhfix/MapleStory.exe`（01:17:40，4940800B），SHA256 `01a9dcf5908b9611a84d83dca2f0fcb907a958b4ee016641fed7bae41d33fdd4`。
  - 改动 obj（FootholdTree/MapNpcs/Spawn/Stage/SetFieldHandlers/Npc/Physics）时间戳均新于各自源码。
- 首次部署遇到启动器回滚（如实留档）：01:17 覆盖后即时 SHA = staged；**10s 后 live 变回 `e775f9fd…`**。追查为新客户端 PID 10916 于 01:18:23（回滚时刻）由 `"E:\MXD\BeiDou-Client-xy\MapleStory.exe"` 启动，启动器把 exe 恢复为自身基线；随后 exe 被该进程锁定，`cp` 报 Permission denied。
- 用户 2026-11-20 确认「自动结束客户端进程再覆盖」后，结束 PID 10916 → 覆盖成功：
  - live SHA = `01a9dcf5908b9611a84d83dca2f0fcb907a958b4ee016641fed7bae41d33fdd4`，4940800B，mtime 01:21:49。
  - **延迟复核 +5s / +10s / +20s 三次 SHA 均为 `01a9dcf5…`，无回滚；4 个僵尸 PID（26292/36840/42860/10856，Path 空、不锁 exe）保持原样未动。**

## 边界（如实）

- **live 已部署**（SHA `01a9dcf5…`，三次延迟复核稳定）。但**真机未验收**：编译通过 ≠ 功能对。需用户重启客户端，确认 NPC 不再掉到下方平台、也不再"全消失"，并提供**本端**截图（USER.md 红线：参考端截图不得作本端验收证据）。
- 若登录器后续再次回滚 exe，本"已部署"声明立即失效，须实时重核 SHA。
- 天气文件（Weather/*）为他人并发在改，本轮未触碰；其最终状态由该任务自行闭环。
