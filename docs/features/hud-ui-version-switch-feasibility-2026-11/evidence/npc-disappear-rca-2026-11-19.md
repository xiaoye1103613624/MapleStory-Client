# NPC 消失 Bug 根因分析（RCA）— 2026-11-19

> 分析对象：冒险岛开源客户端（JourneyMS 衍生，工作区 `E:/project/MapleStory-Client`）
> 现象：进图后地图场景里的 NPC 本体看不到（非小地图标记）。
> 已核对未提交改动：`Gameplay/MapleMap/Npc.cpp` 把「非授控 / 不可动」NPC 的 `phobj.type` 设为 `FIXATED`，且 `update()` 中 FIXATED 不再调用 `physics.move_object`；注释声称修 "MapleTV 大屏模板从天而降"。
> 纪律：证据先行 / 禁止想象修改 / 三层链路（服务端发包 → 客户端解析 → 渲染），全程只读取证，未改任何代码。参考端用 IDA MCP（`MapleStoryT.exe` 已命名 IDB）。

---

## 0. 结论先行

| 判定项 | 结论 |
| --- | --- |
| FIXATED 改动是否导致「NPC 消失」 | **是，强相关，且是最可能根因**（帮倒忙，不是无关）。 |
| 直接机制 | 改动让**进图生成的全部普通 NPC**（`control=false`）走 FIXATED，而 `Npc::update()` 对 FIXATED 跳过 `physics.move_object()` → `FootholdTree::update_fh()` 不再被调用 → **`phobj.fhlayer` 永远停在默认值 0** → 所有 NPC 被登记到 `Layer::ZERO`，在最底层绘制，被上层地形/物件遮挡而"消失"。 |
| 附带风险 | `phobj.onground=true` + 跳过 `move_object` 使 NPC 失去 foothold 对齐（fh 校验、坡面 ground 对齐、fhforce 履带全部停摆），多层平台地图上位置会停在包里给的 y（可能悬空/陷地），进一步加重"看不到"。 |
| 参考端对照 | IDA 有符号：`CNpc::Update` **无条件**调 `CVecCtrl::UpdatePassive`；`CNpc::Init` 静态 NPC（`bMove==0`）只是把移动能力置 0，**仍然照常设置 foothold 且进入统一物理/分层**。参考端不存在"静态 NPC 跳过整个物理更新"的实现。本端改动的"FIXATED=修大屏从天而降"缺乏依据。 |

**修复一句话**：撤销这处 `FIXATED` 改动，恢复为「NPC 始终每帧 `physics.move_object(phobj)`，由 `FIXATED`（现成的类型）在 `Physics::move_object` 内部实现静止」，即对齐 `Drop.cpp:98` 的正确范式。见 §5 精确行号。

---

## 1. 三层链路取证（spawn → set_position → 物理 → draw）

### 1.1 服务端/包解析层：进图 NPC 全部 `control=false`（关键前提）

`Net/Handlers/MapObjectHandlers.cpp`：

```
762  void SpawnNpcHandler::handle(InPacket& recv) const
763  {
764      int32_t oid = recv.read_int();
765      int32_t id = recv.read_int();
766      Point<int16_t> position = recv.read_point();
767      bool flip = recv.read_bool();
768      uint16_t fh = recv.read_short();
769
770      recv.read_short(); // 'rx'
771      recv.read_short(); // 'ry'
772      while (recv.available())
773          recv.skip_byte();
774
775      Stage::get().get_npcs().spawn(
776          { oid, id, position, flip, fh }
777      );
```

- 第 775-777 行构造 `NpcSpawn` 时**只传 5 个参数，未传第 6 个 `control`**。
- `Gameplay/Spawn.h:32`：`NpcSpawn(..., bool control = false);` → **control 默认 false**。
- 另一个入口 `SpawnNpcControllerHandler`（同文件 782-820 行）：仅 mode!=0（本端成为控制器）时才 `{oid, id, position, flip, fh, true}`（816-818 行），这是少数动态授控 NPC；mode==0 分支（787-801 行）走 `set_control(false)`/`remove`，不产生控制。

**结论**：进图时**绝大多数普通 NPC 的 `control=false`**。配合 `movable`（`Npc.cpp:97` 由 `info["move"]` / move 动画决定，静态 NPC 通常为 false），改动里的 `phobj.type = (control && movable) ? NORMAL : FIXATED`（`Npc.cpp:151-153`）会把**几乎所有地图 NPC 都切成 FIXATED**。

关于 fh 解析与长度守卫：本 handler 无 `available()` 早退式守卫（772-773 行是读完后跳过尾部），`fh = recv.read_short()`（768 行）正常落地到 `NpcSpawn.fh`（`Spawn.cpp:30-31`）→ 再进 `physics.get_spawn_point(position, fh)`（`Spawn.cpp:43`）。**没有发现"因 fh 解析/包长守卫导致 NPC 不被创建"的路径**——NPC 对象是创建了并 add 进容器的（见下），问题出在 update/draw 阶段。

### 1.2 spawn 落地与 set_position

`Gameplay/MapleMap/MapNpcs.cpp:31-48`（`update`）：
```
33      for (; !spawns.empty(); spawns.pop())
...
41          if (npcs.contains(oid))
42              npcs.remove(oid);
44          npcs.add(spawn.instantiate(physics));
47      npcs.update(physics);
```
NPC 被 `instantiate` 并 `add`。

`Gameplay/Spawn.cpp:38-45`：
```
43      auto spawnposition = physics.get_spawn_point(position, fh);
44      return std::make_unique<Npc>(id, oid, flip, fh, control, spawnposition);
```
`Npc` 构造（`Npc.cpp:41-157`）：
- 144 行 `phobj.fhid = f;`
- 145 行 `set_position(position)` → `MapObject::set_position`（`MapObject.cpp:35-46`）只设 `phobj.x/y`，**不设 `fhlayer`**。
- 146-153 行（改动新增）：`phobj.onground=true; phobj.type = (control&&movable)?NORMAL:FIXATED;`

`PhysicsObject.h:199-207`：`Type type = Type::NORMAL;`、`int8_t fhlayer = 0;` —— **fhlayer 默认 0，set_position 不碰它**。

### 1.3 物理层：fhlayer 的**唯一**赋值点，以及 FIXATED 早退

全仓库 `fhlayer` 赋值点（grep 全 `Gameplay/`，12 处命中）中，唯一写入发生在：

`Gameplay/Physics/FootholdTree.cpp:175-178, 261-262`：
```
175  void FootholdTree::update_fh(PhysicsObject& phobj) const
176  {
177      if (phobj.type == PhysicsObject::Type::FIXATED && phobj.fhid > 0)
178          return;                       // ← FIXATED 且 fh 有效：直接早退
...
261      if (phobj.fhlayer == 0 || phobj.onground)
262          phobj.fhlayer = nextfh.layer();   // ← fhlayer 唯一赋值处
```

`Gameplay/Physics/Physics.cpp:42-72`（`move_object`）：
```
42  void Physics::move_object(PhysicsObject& phobj) const
43  {
45      fht.update_fh(phobj);            // ← 唯一调用 update_fh 的地方
...
51      switch (phobj.type)
53      case NORMAL:  move_normal(phobj); fht.limit_movement(phobj); break;
...
65      case FIXATED:
66      default: break;                  // FIXATED 在物理引擎内部本就是"只 update_fh，不加力"
71      phobj.move();
72  }
```

**要点**：`FIXATED` 类型在 `Physics::move_object` 中**本来就被正确处理**（45 行照常 `update_fh` 定层，65 行 case 空转不加重力）。即"让静态 NPC 不坠落"根本不需要跳过 `move_object`——调用它即可，内部已 FIXATED 分支兜住。改动重复造轮子，而且造错了：

`Gameplay/MapleMap/Npc.cpp:208-258`（改动后的 update）：
```
218      if (control && movable)
...
225          physics.move_object(phobj);            // 授控可动：调
...
238      else
239      {
240          // FIXATED：只播动画，不进重力/落板；授控可动才走 move_object。
241          if (phobj.type != PhysicsObject::Type::FIXATED)
242              physics.move_object(phobj);        // ← 非授控/静态：跳过！
244          if (animations.count(stance)) ...
257      return phobj.fhlayer;
```

→ 静态 NPC 永不执行 `Physics::move_object` → 永不执行 `FootholdTree::update_fh` → **`fhlayer` 永远 = 0**（初始化默认值，`PhysicsObject.h:205`）。

### 1.4 渲染层：分层 draw 只画本层对象

`Gameplay/MapleMap/MapObjects.cpp:33-57`（`draw`）：
```
38      for (auto& oid : layers[layer])        // ← 只遍历指定 layer 的集合
42          if (!mmo || !mmo->is_active()) continue;
47          const Point<int16_t> pos = mmo->get_position();
51          if (sx < -CULL_MARGIN ... ) continue;   // 视口剔除
55          mmo->draw(viewx, viewy, alpha);
```
`MapObjects.cpp:111-117`（`add`）：
```
114      int8_t layer = toadd->get_layer();      // get_layer() = phobj.fhlayer
115      objects[oid] = std::move(toadd);
116      layers[layer].insert(oid);              // ← 按 fhlayer 登记
```
`MapObject.cpp:63-66`：`get_layer() { return phobj.fhlayer; }`
`MapObjects.cpp:59-96`（`update`）：每帧比较 `oldlayer/newlayer`，`newlayer` 来自 `mmo->update(physics)` 的返回值（即 `Npc::update` 的 `return phobj.fhlayer`，`Npc.cpp:257`），不同则迁移 layer 集合。

`Gameplay/Stage.cpp:208-220`：
```
210      for (auto id : Layer::IDs)
212          tilesobjs.draw(id, viewpos, alpha);   // 地形对象先画
213          reactors.draw(id, ...);
214          npcs.draw(id, viewx, viewy, alpha);   // NPC 画在同层地形之后
215          mobs.draw(id, ...);
...
```
Layer ID 0..7（`Layer.h:26-37`），数字越大越靠前（后画）。`fhlayer=0` 的 NPC 在**最底层**画，上层 tile/obj（layer 1..7）随后覆盖它 → 视觉上 NPC 被地形/前景遮挡，表现为"消失"。特别是多层平台地图（如树城），NPC 所在 foothold 的 layer 通常 >0，改动后全部掉到 0，与地形层错位，几乎必然被遮。

> 注：`MapObjects.cpp:51-53` 的视口剔除按 `get_position()`，FIXATED 不改 x/y，所以**不是**被剔除到屏幕外；"消失"来自分层遮挡 + fh 未对齐，而非坐标 (0,0)。NP C 的 `draw`（`Npc.cpp:159-206`）用 `phobj.get_absolute()`（`PhysicsObject.h:173-176`），只依赖 x/y，不受 `type` 影响——**FIXATED 本身不改变绘制位置**，但通过 `fhlayer` 间接决定了"画在哪一层/是否被覆盖"。

### 1.5 对照：Drop 的正确 FIXATED 范式（反证）

`Gameplay/MapleMap/Drop.cpp`：
```
96   int8_t Drop::update(const Physics& physics)
98       physics.move_object(phobj);          // ← 即使 type=FIXATED 也照常调用！
...
108          phobj.type = PhysicsObject::Type::FIXATED;   // 落地后切 FIXATED 静止
148      return phobj.fhlayer;
```
Drop 落地后也切成 FIXATED（`Drop.cpp:76, 108`），但**每帧依旧 `physics.move_object`**（98 行），靠 `Physics::move_object` 内部的 FIXATED case 实现"静止但仍 update_fh 定层"。NPC 改动跳过调用，与项目内既有正确范式直接矛盾。

---

## 2. 参考端 IDA 取证（MapleStoryT.exe）

- IDA MCP 连通正常：`server_health` → `idb_path=E:\MXD\反编译IDA\MapleStoryT.i64`、`module=MapleStoryT.exe`、`auto_analysis_ready=true`、`hexrays_ready=true`、`named_functions=153858`（**有符号，非裸机**）。
- `find_regex "CNpc"` 返回 0（该工具搜的是字符串字面量，不是函数名）；改用 `py_eval` 遍历 `idautils.Functions()`，得到 931 个含 `Npc` 的命名函数，筛出生命周期关键函数如下：

| 函数 | 地址 | 反编译要点 |
| --- | --- | --- |
| `CNpcPool::DecodeInitPacket` | `0xdd4ea0` | `Decode4(oid)`→查重→`AddTail`→`Decode4(templateID)`→`GetNpcTemplate`→`CreateNpc`→`CNpc::Init(oid, packet)`。`0xdd4edf` 对已存在 oid 置标志返回 0（不重复创建），与客户端 `MapNpcs::update` 的 remove+add 策略方向一致。 |
| `CNpc::Init` | `0xdca290` | `0xdca31a/0xdca320` 解包 x/y；`0xdca34e` 解 `m_bMove`；`0xdca36c-0xdca37c` 解 `fh` 并 `CWvsPhysicalSpace2D::GetFoothold(dwSN)` 取得 `CStaticFoothold*`；`0xdca3da` 静态 NPC（`bMove==0`）仅把 `nMoveAbility` 置 0；`0xdca420` 把 x/y/moveAction/**Foothold 指针**一起传给物理控制器初始化。**静态 NPC 仍绑定 foothold、仍进统一物理控制器**。 |
| `CNpc::Update` | `0xdc85a0` | `0xdc88c3` **无条件** `CVecCtrl::UpdatePassive(v20)` → `CNpc::SetMoveAction(...)`；无任何"静态 NPC 跳过物理更新"的分支。 |
| `CNpc::SetActionLayerPos` | `0xdcd060` | 仅把 rx/ry/tTime 压入 `m_qActionLayerRelmove` 队列（相对位移层），与"绝对位置/分层"无关。 |

**判读**：参考端静态 NPC 的处理是"**关移动能力但照常纳入物理与分层系统**"（foothold 指针照传、Update 照跑），与本端改动"整段跳过 physics.move_object"的做法不一致。改动注释"MapleTV 无独立放置逻辑，与普通 NPC 同 spawn/物理路径"反而印证：**用 FIXATED 去修 MapleTV 从天而降，本来就不该靠跳过物理实现**——正确做法是把大屏模板纳入正常的 fh 对齐（或复用现成 FIXATED-in-`move_object` 语义）。

（取证方法：`python tools/_mcp_call.py 13339 call <tool> '<json>'`；工具 `server_health`/`survey_binary`/`py_eval`/`decompile`。）

---

## 3. 假设排序（按概率）

### 假设 A（最高概率）：FIXATED 跳过 `move_object` → `fhlayer` 锁死 0 → 全场 NPC 沉到最底层被遮挡

- **概率**：最高。改动直接把"进图 100% 普通 NPC"（`MapObjectHandlers.cpp:775-777` + `Spawn.h:32` control 默认 false）变成 FIXATED，`Npc.cpp:241-242` 跳过 `move_object`，`FootholdTree.cpp:177-178` 与 `Physics.cpp:45` 断链，`FootholdTree.cpp:261-262` 的 fhlayer 赋值永不执行，`PhysicsObject.h:205` 默认 0，`MapObjects.cpp:114-116` 登记到 Layer 0，`Stage.cpp:210-218` 层 0 先画且被上层 tile 覆盖。
- **特征吻合**：用户报"角色所在地图上 npc 都消失了"——是**全场**而非个别，符合"全部 control=false → 全部 FIXATED → 全部 layer 0"。
- **可证伪实验**：把 `Npc.cpp:241-242` 改回无条件 `physics.move_object(phobj)`（保留 type 赋值亦可），重进图观察 NPC 是否恢复。若恢复即坐实。

### 假设 B（中等概率，与 A 叠加）：fh 未对齐导致位置悬空/陷地，被地形吞没

- **概率**：中。FIXATED 且 `onground=true`（`Npc.cpp:150`）时，`FootholdTree::update_fh` 即便被调用也会在 177-178 行早退，不做 slope/ground 对齐；再叠加 `move_object` 被跳过，`phobj.y` 停在 `get_spawn_point` 给的值。`Spawn.cpp:40-43` 注释已记载"NPC 包 y 槽是 wz cy，比平台线低 5~17px"，本靠 fh 对齐补偿；现在补偿消失，NPC 可能停在平台线下或墙里。
- **证据**：`Spawn.cpp:40-43`、`FootholdTree.cpp:390-398`（`get_spawn_point` 用 fh 求 ground_below）、`Npc.cpp:150-153`。
- **注意**：此假设与 A 同根同源（都因跳过物理），修复 A 即同时修复 B。

### 假设 C（低概率，基本可排除）：spawn 包解析/fh 守卫导致 NPC 未被创建

- **概率**：低。`SpawnNpcHandler`（`MapObjectHandlers.cpp:762-780`）逐字段读 oid/id/position/flip/fh/rx/ry 后跳过尾部，无 `available()` 早退，`while(recv.available()) skip_byte()` 不会吞掉已读字段；`NpcSpawn::instantiate`（`Spawn.cpp:38-45`）必定返回 `Npc` 对象；`MapNpcs::update`（`MapNpcs.cpp:41-45`）必定 add。
- **排除依据**：若 NPC 未创建，应表现为"完全无对象"，但 A 的机制下对象是存在且 active 的、只是画在 layer 0——与"进图看不到"一致但与"包没解析"无关。除非服务端根本没发 spawn 包（本任务无服务端日志，无法证实），否则此路径不成立。

---

## 4. FIXATED 改动定性

**结论：帮倒忙（净负面），不是无关。**

| 改动点 | 行号（当前工作区） | 问题 |
| --- | --- | --- |
| `phobj.onground=true; phobj.type=(control&&movable)?NORMAL:FIXATED` | `Npc.cpp:150-153` | 使全图 control=false 的普通 NPC 变 FIXATED；与"从天而降"无因果关系（掉落根因是 fh 未命中/onground 失配，应修 fh 对齐而非改类型）。 |
| `if (phobj.type != FIXATED) physics.move_object(phobj);` | `Npc.cpp:240-242` | **致命**：跳过 `move_object` → 跳过 `update_fh` → fhlayer 锁 0 → NPC 沉底遮挡；同时丢掉 fh/坡面/履带对齐。 |
| `set_control` 内 `phobj.type=NORMAL/FIXATED` 切换 | `Npc.cpp:359-371` | 撤控时改 FIXATED 并清速度，同样会因后续 update 跳过 move_object 而锁死 fhlayer；且 type 反复横跳无必要。 |

与项目既有正确范式冲突：`Drop.cpp:98` 的 FIXATED 对象仍每帧 `physics.move_object`。参考端 `CNpc::Update @0xdc88c3` 也无条件更新物理。改动的注释"MapleTV 无独立放置逻辑"本身说明不该给它开特例。

---

## 5. 精确修复建议（改哪个文件哪几行、改成什么）

> 目标：撤销「跳过物理」这一错误做法，保留/恢复"NPC 始终走统一物理与分层"的原生语义。两档方案，按风险从低到高：

### 方案 1（推荐，最小改动，先验证根因 A）：恢复无条件 `move_object`

文件 `Gameplay/MapleMap/Npc.cpp`：

1. **第 240-242 行**，把
   ```cpp
   		// FIXATED：只播动画，不进重力/落板；授控可动才走 move_object。
   		if (phobj.type != PhysicsObject::Type::FIXATED)
   			physics.move_object(phobj);
   ```
   改为（恢复改动前）：
   ```cpp
   		physics.move_object(phobj);
   ```
   —— 仅此一处即可让 `update_fh` 恢复调用、fhlayer 正常迁移。FIXATED 在 `Physics::move_object` 的 `case FIXATED` 里本就"不加重力"，不会重新引入"从天而降"。

2. **第 146-153 行**：可保留 `movable` 判定，但鉴于 FIXATED 在 move_object 内部已安全，`phobj.type` 的赋值不再是必需；若要彻底还原基线，删除 146-153 行新增块（含 `phobj.onground=true` 与 `phobj.type=...`），让 type 保持 `PhysicsObject.h:199` 的默认 `NORMAL`（与改动前一致）。**若选择仅做第 1 步留着 type 赋值，也必须先确认 `FootholdTree.cpp:177-178` 的早退不会挡住 fhlayer 更新**——但第 1 步后 `update_fh` 对 `fhid>0` 的 FIXATED 仍会 177-178 行早退，fhlayer **依然锁死 0**！所以：
   - **必须**同时保证 FIXATED 对象能更新 fhlayer。最干净做法：**撤销 146-153 与 359-371 的 type 赋值**，回到全 NORMAL 基线；或
   - 修改 `FootholdTree.cpp:177-178`，让 FIXATED 早退前仍执行 261-262 的 fhlayer 赋值（例如把早退条件改为仅跳过重力相关计算、保留 `phobj.fhlayer = get_fh(phobj.fhid).layer()`）。

3. **第 359-371 行**（`set_control`）：删除新增的 `phobj.type = NORMAL/FIXATED` 与 `hspeed/vspeed=0; onground=true` 块，恢复为仅 `control=value; path_accum_ms=0; path_start_*` + 原 LOG。

### 方案 2（保留 FIXATED 语义的"正确实现"，供 MapleTV 专项修复复用）

若确需静态 NPC 用 FIXATED 表达"不移动"，则必须补齐 fhlayer 链路，二选一：

- **2a**：`Npc::update`（`Npc.cpp:218-255`）统一无条件 `physics.move_object(phobj)`（同 Drop.cpp:98），并删掉 `Npc.cpp:241-242` 的跳过判断；type 赋值可留，因为 `Physics::move_object` 的 `case FIXATED`（`Physics.cpp:65-67`）本就不加力，只是**必须让 `FootholdTree::update_fh` 不被 177-178 早退挡住 fhlayer**——故仍需处理下一条。
- **2b**：`Gameplay/Physics/FootholdTree.cpp:177-178`，把
  ```cpp
  if (phobj.type == PhysicsObject::Type::FIXATED && phobj.fhid > 0)
      return;
  ```
  改为不早退（或早退前先 `phobj.fhlayer = get_fh(phobj.fhid).layer();`），保证 FIXATED 对象在 spawn/manager 更新时能拿到正确 layer。这是让"FIXATED 静态 NPC"既不坠落、又能正确分层的**唯一自洽修复**。

**最小验证门禁**（改后必做，禁止空口完成）：
1. 回退改动后重新 `git diff -- Gameplay/MapleMap/Npc.cpp` 应只剩原始基线（或仅保留 2b 的 FootholdTree 修正）。
2. 客户端构建通过（VS 工程 `MapleStory.vcxproj`，最小改动仅 C++ 逻辑，不依赖资源）。
3. 真机进图观察：自由市场 9250025 / 树城多层平台 / 任意普通村，NPC 全部可见且站在正确平台层（layer 与 foothold 一致）；MapleTV 大屏不再"从天而降"。
4. 若真机仍异常，按假设 B 复查 `Spawn.cpp:43` 的 `get_spawn_point` 与服务端 spawn 包 y/fh 字段（需要服务端发包日志，本子任务无权限/无日志，未验证）。

---

## 6. 证据索引（行号级）

| 结论 | 文件:行 |
| --- | --- |
| 进图 NPC control 默认 false | `Net/Handlers/MapObjectHandlers.cpp:762-777` + `Gameplay/Spawn.h:32` |
| NPC spawn 包字段落地（无长度守卫丢弃） | `Net/Handlers/MapObjectHandlers.cpp:764-776` |
| fh → spawn_point → Npc 构造 | `Gameplay/Spawn.cpp:38-45`、`Gameplay/MapleMap/MapNpcs.cpp:31-48` |
| set_position 不设 fhlayer | `Gameplay/MapleMap/MapObject.cpp:35-46` |
| fhlayer 默认 0 | `Gameplay/Physics/PhysicsObject.h:205` |
| FIXATED 改动三处 | `Gameplay/MapleMap/Npc.cpp:150-153, 240-242, 359-371`（git diff 新增） |
| FIXATED 跳过 move_object | `Gameplay/MapleMap/Npc.cpp:241-242` |
| move_object 是 update_fh 唯一调用者 | `Gameplay/Physics/Physics.cpp:42-45` |
| update_fh 对 FIXATED 早退、fhlayer 唯一写入在末尾 | `Gameplay/Physics/FootholdTree.cpp:175-178, 261-262` |
| FIXATED 在物理引擎内部本就"静止但定层" | `Gameplay/Physics/Physics.cpp:51-72`（case FIXATED 空转） |
| Drop 正确范式（FIXATED 仍每帧 move_object） | `Gameplay/MapleMap/Drop.cpp:98, 108` |
| 分层 draw 只画本层、按 fhlayer 登记 | `Gameplay/MapleMap/MapObjects.cpp:33-57, 111-117` |
| Stage 逐层绘制，layer 0 最先被覆盖 | `Gameplay/Stage.cpp:208-220`、`Gameplay/MapleMap/Layer.h:26-37` |
| Npc draw 用 get_absolute（不受 type 直接影响） | `Gameplay/MapleMap/Npc.cpp:159-206`、`Gameplay/Physics/PhysicsObject.h:173-176` |
| 参考端 CNpc 有符号、Update 无条件物理 | IDA `0xdc85a0 @0xdc88c3`（`CVecCtrl::UpdatePassive`） |
| 参考端静态 NPC 仍绑 foothold | IDA `0xdca290 @0xdca36c-0xdca420`（`GetFoothold` + 传入物理控制器） |
| 参考端 NPC spawn 包解析与去重 | IDA `0xdd4ea0`（`DecodeInitPacket`） |

---

## 7. 局限与交接

- 本子任务**只读取证，未改任何代码，未构建、未真机验证**。所有结论基于源码静态链路 + IDA 反编译；假设 A/B 的真机确认需在修复后由 lead/用户完成。
- 无服务端发包日志，无法直接证实"包中 fh/坐标是否正确"，故假设 C 只能基于客户端解析代码排除客户端侧守卫，服务端侧保留开放。
- IDA 端 `CNpc::Init` 完整函数体因超长仅取到前段（足以证明 foothold 绑定与 nMoveAbility 置 0）；如需 MapleTV 专项对照，可继续反编译 `0xdca290` 后半段及 `SetMapleTVMessage` 相关函数。
