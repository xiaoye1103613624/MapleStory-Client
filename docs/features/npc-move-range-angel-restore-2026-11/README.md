# NPC 走下台阶 / 超出点位 —— Angel 活动范围还原（2026-11-20）

## 现象

NPC 左右乱走，走下台阶，超出原生点位很远。

## 用户两问的结论

1. **原生 NPC 有左右移动范围限制，本端此前完全没还原。**
2. **对照 Angel 做过，但"物理/VecCtrl"那维当时明确标"保留简化（D6）"未还原**——范围限制正落在被跳过的部分。`docs/features/npc-angel-calibration-2026-11/README.md:91` D6；`notes/r01-angel-native-npc-facts.md:172` 物理维 ⚠️ 完整 VecCtrl 预留（D6）。不是没对照，是对照后判定暂不做，现暴露为 bug。

## 证据（行号级）

### 原生有范围约束
- Angel `CNpc::Init@0x6d089a`（`evidence/angel-npc-facts.md:250-252`）：两次 `Decode2` 读 rx0/rx1，存 `this+86/87`（`this+84=this+86`）。
- `CNpc::GetDCRange@0x6d3fde`（同文件 `:193-200`）：用 rx 算出活动矩形 `this+88/89`，交 `CVecCtrlNpc` 约束；`CVecCtrlNpc::WorkUpdateActive@0x9c211e` / `UpdateActiveInterrupted@0x9c1daf`（`:846-876`）执行。
- 服务端 s9 `PacketCreator.java:1433-1445`：`writeShort(getRx0()) + writeShort(getRx1())`；`AbstractLoadedLife.java:33-34,87-100` 定义；`PlayerNPC.java:338-339` / `NpcCommand.java:50-51` 实锤 `rx0=x+50(右界) / rx1=x-50(左界)` —— **绝对坐标，方向相反，需归一化**；`MapFactory.java:71-72,345-346` 从 wz life 节点原样读入。

### 本端把边界丢了
- `Net/Handlers/MapObjectHandlers.cpp:762-780` `SpawnNpcHandler`、`:782-820` `SpawnNpcControllerHandler`：读出 rx0/rx1 两 short 后直接丢弃（原注释误标 'rx'/'ry'）。
- `Spawn.h`/`Npc.h` 原本无范围成员；`Npc.cpp:41` 构造不接收 rx。

### 为什么会走下台阶
- `Npc.cpp:210-229`（改前）：授控每帧 `hforce=±0.08`（`:213`）→ `move_object`（`:217`）；`next_control_move`（`:288-326`）每次随机翻向，全程无边界判断。
- `FootholdTree.cpp:186-197`：站立时 `x>curfh.r()` 就把 fhid 切到 `next()`，相邻下行平台就跟着走下去；`:230` `onground=y==ground` 坐实落点。
- `FootholdTree.cpp:123-145` 的平台边缘转向只在 `TURNATEDGES` 置位时生效（`:134-144`），而 Npc 从未设该标志（全仓仅 `Mob.cpp:175` 设）——怪物有边缘保护、NPC 没有的不对称根因。

## 修复（一次性，5 文件）

1. `Net/Handlers/MapObjectHandlers.cpp`：两处 handler 解析 `rx0/rx1` 并透传给 `NpcSpawn`。
2. `Gameplay/Spawn.h/.cpp`：`NpcSpawn` 加 `rx_min/rx_max` 并在构造内归一化 min/max；**兜底**：若归一化范围不包含 spawn 的 x（说明 rx 是相对偏移/脏数据），则关闭范围（0/0），不把 NPC 钉错坐标；`instantiate` 把 rx 传给 `Npc`。
3. `Gameplay/MapleMap/Npc.h/.cpp`：
   - 构造接收 rx_min/rx_max 并存成员；
   - `movable` NPC 无条件 `set_flag(TURNATEDGES)`（对齐 `Mob.cpp:175`），无范围时也靠平台边缘转向兜住"走下台阶"；
   - 新增 `clamp_to_move_range()`（对齐 `GetDCRange@0x6d3fde`）：x 越界则 `limitx` 钉回边界 + 清 hforce + 翻向 + 切回 stand，置 `just_bounced` 立即上行新坐标（避免服务端按旧方向插值）；授控/非授控两分支都在 `move_object` 后调用。

## 验证

- 编译：`MSBuild MapleStory.sln /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v142`，`BUILD_EXITCODE=0`，`error C` 计数 0。
- 产物：`x64/out-npcrange/MapleStory.exe`（07:55:28，4941312B），SHA256 `c8bd88cc9fdf3e72985ac7d10602a6cf34aaaa008f00d79e5f70dcf27bce6a0c`；改动 obj（Npc/Spawn/MapObjectHandlers/MapNpcs）时间戳均新于源码。
- 部署：live `E:\MXD\BeiDou-Client-xy\MapleStory.exe` 覆盖后即时 / +8s / +20s 三次 SHA 均为 `c8bd88cc…`，无回滚；4 个僵尸 PID（26292/36840/42860/10856）未动，无 live 客户端锁 exe。

## 边界（如实）

- **真机未验收**：编译过 ≠ 功能对。需用户重启客户端，确认 NPC 不再走下台阶、不超出点位，并提供**本端**截图（USER.md 红线：参考端截图不作验收证据）。
- **语义口径声明**：rx0/rx1 按 s9 服务端源码判定为绝对坐标；已加"范围须包含 spawn x"的兜底防相对值误伤。若真机出现 NPC 被钉到地图边缘，优先怀疑 rx 口径，届时用 setfield_trace / 临时垫片打印 rx/spawn x 复核。
- 若登录器后续回滚 exe，本"已部署"声明立即失效，须实时重核 SHA。
