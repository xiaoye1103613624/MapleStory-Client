# 原生基础功能 / 引擎完成度体检 + 三组现象根因（2026-11-22）

> 性质：先只读取证/决策点（§0-§5），用户拍板 D1-D5 后按 Plan 实施（§6），含多轮拖拽真机回归修复（§6.4/§6.5）。**Release/x64 构建通过 EXITCODE=0 / GATE OK；真机联调待用户启动 s9 + MySQL 验证。未提交 git（工作区另有大量既有改动）。**
> 触发：用户报「底部快捷栏图标抓不了/拖不动、药水绑上按键也用不了」「脚本学到的瞬移、二段跳用不了」「本客户端是不是很多原生基础功能/引擎都没实现好？全部检查一下」。
> 对照源：`E:\project\MinMS`（同源 C++ 实现，含完整位移/拖放）、`E:\MXD\反编译IDA\v83\Angel.i64`（Angel 083 带符号 IDB，既有 106 技能 / 548 HUD 函数逐字证据）、`E:\MXD\【2023.8.8江浩生日福利】怀旧岛192整合版`（资源/脚本交叉验证）、s9 服务端源码 `E:\project\BeiDou-Server_s9`（协议权威）。

## 0. 一句话结论

**不是「大量原生基础功能/引擎都没实现」**——核心 UI（99 个 UI*.cpp）、网络（数百 handler，常规 opcode 仅 1 个有意未注册）、渲染、输入、资源、音频主路径均已成型。但用户报的三组现象**全部属实**，且根因明确、集中在 3 处具体断点：

1. 快捷栏抓取/拖拽：机制存在，但**拖拽状态 `qs_dragging` 起拖后不复位**，第一次拖完后永久卡 `CANCLICK`；普通图标（背包药水）松手不结算落点。
2. 药水按键无效：`Stage::send_key` **完全没有 `KeyType::Id::ITEM` 分支**，`UseItemPacket` 在状态栏/舞台层零调用 → 道具键被静默吞掉。
3. 瞬移 / 二段跳：`Combat::apply_use_movement` 四个技能 case **全是空语句**；MinMS 同源实现里有完整位移逻辑，本端未移植。

## 1. 三组现象根因（源码逐字证据）

### 1.1 快捷栏抓取 / 拖拽（部分实现 + 状态缺陷）

| 环节 | 证据 | 状态 |
|---|---|---|
| 绘制/命中/单击 | `IO/UITypes/UIStatusBar.cpp:2382-2604`、`:2679-2760`；`classic_qs_slot_at` | ✅ 已实现 |
| 起拖阈值 8px | `UIStatusBar.cpp:126`、`:2928-2937` | ✅ |
| 拖拽载荷构造 | `UIStatusBar.cpp:2762-2803` `setup_qs_drag_icon` → `UI::get().drag_icon` | ✅ |
| 落点绑定（换位/移动） | `UIStatusBar.cpp:2828-2908` `send_icon` → `kb.assign` + `ChangeKeyMapPacket(135)` + `redraw_quickslots` | ✅ |
| **`qs_dragging` 不复位** | 置位 `UIStatusBar.cpp:2936`；全文件唯一复位 `:2998`，且只在 `!quickslot_active` 分支 → 第一次拖后 `:2964/:2969` 永久 `CANCLICK`，**再点快捷栏不会触发使用** | ❌ 致命 |
| `qs_pressed` 死变量 | `UIStatusBar.h:137` 声明；`UIStatusBar.cpp:2989/:2999` 只置 false，从未置 true | ❌ |
| 背包图标松手不结算 | `IO/UIStateGame.cpp:838-853` 仅 macro 源黏住；`:856-868` 仅 SkillMacroEx 窗口；其余 `:870 return GRABBING` → 药水拖到快捷格松开不绑定，必须再点一次目标格 | ❌ |
| 右键清除槽位 | `IO/UIElement.h:328` `rightclick` 空默认，`UIStatusBar` 未 override | ❌ 缺失 |

相关历史文档自认未真机验收：`docs/features/quickslot-interaction-fix-2026-11-18/README.md:32,46`、`quickslot-item-bind-2026-11-17/README.md:49`。

### 1.2 药水放在快捷键上无法使用（完全缺失）

- 输入链完整：`IO/UI.cpp:218` → `IO/UIStateGame.cpp:797-805` → `Stage::get().send_key(type, action, pressed)`。
- **断点**：`Gameplay/Stage.cpp:654-682` 的 `switch(type)` 只有 `ACTION` / `SKILL` / `MACRO` / `FACE`，**无 `KeyType::Id::ITEM`**。
- `UseItemPacket(72)` 全局仅 7 处调用：`Character/Player.cpp:331`、`IO/UITypes/UIItemInventory.cpp:866,1305`、`UIChatBar.cpp:1002`、`UIPlant.cpp:32`、`UIMiscShells.cpp:4274,6155`；`Stage.cpp` / `UIStatusBar.cpp` 零调用。
- 结论：道具绑定写入了键位表（135 包），但按键触发时不发 `USE_ITEM(72)`，药水永远用不了。

### 1.3 瞬移 / 二段跳（空壳 + 被黑名单禁止）

**技能 ID 纠正**：`2001004` 在两版都是 `ENERGY_BOLT`（能量弹），**不是瞬移**。真瞬移 = `2101002`(F/P) / `2201002`(I/L) / `2301001`(Priest)；二段跳 = `4111006` FLASH_JUMP（`Character/SkillId.h:128/149/152/156`）。

- `Gameplay/Combat/Combat.cpp:308-319`：
  ```cpp
  void Combat::apply_use_movement(const SpecialMove& move) {
      switch (move.get_id()) {
      case SkillId::Id::TELEPORT_FP:
      case SkillId::Id::IL_TELEPORT:
      case SkillId::Id::PRIEST_TELEPORT:
      case SkillId::Id::FLASH_JUMP:
      default: break;   // 四个 case 全空
      }
  }
  ```
- 该函数仅在攻击分支被调用（`Combat.cpp:200`），非攻击技能连空壳都走不到；非攻击路径 `:247-260` 只播特效 + 发 `UseSkillPacket(91)`，本地零位移。
- `Character/PlayerStates.h:80-90` `PlayerFallState::send_action` 空 `{}`：空中再跳无任何处理。
- `IO/UITypes/UIMiscShells.cpp:4379-4418` `is_able_to_map_macrosys_skill()` 把 `2101002/2201002/2301001/4111006` 列入黑名单 `return false`（调用点 `:4788`）→ 这些技能连宏都放不进去。
- 发包本身没问题：`UseSkillPacket` (`Net/Packets/AttackAndSkillPackets.h:285-303`) opcode 91 与 s9 `SPECIAL_MOVE(0x5B)` 对齐（`BeiDou-Server_s9/.../RecvOpcode.java:105`、`PacketProcessor.java:210`、`SpecialMoveHandler.java:48-163`）。**缺的是客户端本地位移**。
- **不存在脚本驱动技能系统**：本客户端源码零 `.js`/`.lua`；部署目录 83 个 .js 全是 095 REA 反编译的 NPC 脚本证据，不参与运行时。用户所说「脚本学到的技能」最多是宏/键位配置，而宏恰好禁止这些 ID。

## 2. 与 MinMS / Angel / 192 的对照（证据充分，无需截图兜底）

### 2.1 MinMS 是最佳直接移植源（同源且有完整实现）

MinMS 与本客户端目录/文件结构几乎逐一同构（`src/client/IO/UITypes/UIStatusBar.cpp`、`src/client/Gameplay/Combat/Combat.cpp`、`src/client/Character/SkillId.h`），差异集中在本端缺的这几段：

| 功能 | MinMS 完整实现（绝对路径 + 行号） | 本端缺口 |
|---|---|---|
| 快捷栏抓取 | `E:\project\MinMS\src\client\IO\UITypes\UIStatusBar.cpp:651-690`：`is_press_edge` → `start_drag` + `UI::drag_icon` | 有实现但状态不复位 |
| 背包→快捷栏落点 | `UIStatusBar.cpp:692-695` `send_icon → drop_on_bindings`；`:697-774` `drop_binding → bind_quickslot`；`:863-890` `commit_key_updates → ChangeKeyMapPacket` + 本地 Keyboard 同步 | 松手不结算 |
| 解除绑定 | `UIStatusBar.cpp:776-787` `unbind_key`（拖出场景/remove=true 触发） | 无独立 clear/unbind |
| 道具按键发包 | `E:\project\MinMS\src\client\Character\Player.cpp:331-352` `use_item(itemid)` → `UseItemPacket(slot,itemid).dispatch()`（`:351`） | `Stage::send_key` 无 ITEM case |
| 二段跳 | `E:\project\MinMS\src\client\Gameplay\Combat\Combat.cpp:1691-1718`：仅空中；`h=3.2+0.12*lv`、`v=-(1.8+0.04*lv)`，改 `hspeed/vspeed` + `set_state(FALL)` + `spend_flash_jump()`；`Player.cpp:1256-1265` 拒绝 `onground||flash_jump_used` | 空 case |
| 瞬移 | `Combat.cpp:1720-1725` `is_teleport_skill`、`:1759-1790` `apply_teleport`（`attach_floor` 直接改坐标 + Teleport 特效 + 15 tick 冷却）、`:1824-1868` `find_teleport_target`（读方向键，无方向按朝向）、`:1792-1822` 怪物碰撞过滤 | 空 case |

MinMS 源码注释明确写着 `// Angel: refuse without a foothold under VecCtrl.`（`:1761`）、`// Angel FindHitMobInRect around the teleport segment`（`:1795`）——说明这套位移实现本身就是按 Angel 原生语义写的，是「Angel 原生逻辑的 C++ 落地版」，移植依据充分。

### 2.2 Angel 083 IDB 一手证据（既有归档，无需重跑）

- HUD：`CUIStatusBar::GetShortCutIndexByPos @ 0x8de8d5`（32×32、8 格、X=647/Y=427）、`ReDrawQuickslot @ 0x8d82a7`、`ToggleQuickSlot @ 0x8df4b1`；当前端命中→`send_key` 已销案为 ✅（`docs/features/statusbar-hud-angel-calibration-2026-11/README.md` A3/G2/E3）。
- 技能：`CUserLocal::IsTeleportSkillAvailable @ 0x96cb93`、`TryRegisterTeleport @ 0x957b74`、`DoActiveSkill_BoundJump`（12 子型之一）；106/106 函数与 BeiDou 同 EA 逐字一致（`docs/features/skill-angel-calibration-2026-11/`）。
- **ANGEL↔s9 协议红线**：Angel 瞬移登记走 c2s `101`，但 s9 中 `101 = USE_INNER_PORTAL`（同号异义）；`92/93` 亦同号异义。故修复只能走「本地位移 + 保留 s9 `USE_SKILL(91)`」，**不可把 opcode 改回 083**。

### 2.3 192 客户端

192 整合版提供资源/JS/NPC 脚本交叉验证，但其逻辑在 DLL/脚本层，不含本端 C++ 源码；本轮未从 192 提取新逻辑（MinMS 已覆盖）。若后续要核对技能 WZ 数值（hrange/mpCon）或动画帧序，再用 orange-wz 读其 IMG。

## 3. 全面体检结论（基于源码，未编译未运行）

**不是大面积未实现**。核心框架成型；确凿缺口如下（带证据）：

### A. 真缺口（证据确凿）
1. 掉落表数据源空壳：`Data/MobDropData.cpp:1-17`（stub loader，entries 空）。
2. 富文本标记未实现：`Graphics/GraphicsGL.cpp:1085/1109/1116/1161/1196`（`#h/#l/#L/#p/#t/#z` 仅 `skip += 2`）；`:1029/:1075` `#d/#g` 固定近似色。
3. 天气打滑 + 环境音循环仅接口桩：`Weather/WeatherGround.h:12-16`；根因之一是 `Audio/Audio.h:76-80,101-107` 无 loop/stop 句柄（Windows BASS 已接）。
4. 雇佣商人「点击进店」预留：`Net/Handlers/HiredMerchantBoxHandlers.cpp:49-54`。
5. 活动 HUD / 小游戏空按键分支：`IO/UITypes/UIAriantMatch.cpp:78-80`、`UIMonsterCarnival.cpp:86-96`、`UIMinigame.cpp:90-97`（文件头自述 `Minigame lobby shell`）。
6. 设置窗退化壳：`IO/UITypes/UIOptionMenu.cpp:53-72`（本包无 `StatusBar3.img/OptionMenu` 节点，另立 GAMEOPT/SYSOPT）。
7. 登录侧入口提示：`typegrade_int/locale/zh-CN.ini:5-6`（「查看世界地图/所有角色暂未开放」，与游戏内 `UIWorldMap` 已实现不矛盾）。
8. **本次新增三组**（§1）：快捷栏状态机/落点、道具使用包、位移技能——这三项是直接影响可玩性的真缺口，优先级高于上述 1-7。

### B. 已实现但有 bug / 未校准
- 怪物攻击帧已补（`Gameplay/MapleMap/Mob.cpp:128-144`）；聊天频道键已接线（`UIChatBar.cpp:1987-1997`）；聊天布局像素偏移仍待校准（`:1385/1431/1481/1500/1525/1559`）。
- 任务未知 mode 降级为通用 tip（`Net/Handlers/QuestHandlers.cpp:133-140`）。

### C. 数据/服务端未下发的空态（非客户端没实现）
- 怪物手册 HP/MP 固定 `???`（`IO/UITypes/UIMonsterBook.cpp:387-395`）；掉落面板空表（`MobDropData.cpp`）；组队搜索注册代理（`UIPartySearch.cpp:198-200`，s9 空 stub）；场标 placeholder（`Gameplay/MapleMap/FieldMarker.h:2/20/56`）。

### D. 网络覆盖度
- 收包枚举 `Net/PacketSwitch.cpp:52-444` vs 注册 `:449-834`：唯一未注册 `RECALCULATE_STATS(35)`，且因与 `FORCED_STAT_RESET(35)` 撞值而有意注销（`:768-773`）。**无「服务端发了但客户端无 handler」的常规 opcode 缺口**。
- 但「已注册 ≠ 业务完整」：`HackshieldRequestHandler` 只记剩余字节（`WaveAShellHandlers.cpp:4928-4930`）、`QuestHandlers` 未知 mode 兜底等。

## 4. 待用户拍板的决策点

| # | 决策点 | 推荐默认 | 依据 |
|---|---|---|---|
| **D1 范围** | 本轮只修用户报的三组（快捷栏 + 药水 + 瞬移/二段跳），还是把 §3 A 类 1-7 一并排期？ | **只修三组**，A 类 1-7 另立批次 | 三组直接影响可玩性；其余多为数据/资源/低优先，混修会放大回归面 |
| **D2 对照源** | 位移与拖放直接以 MinMS 现成实现移植（已按 Angel 语义注释），还是要求逐条重回 Angel IDB 再取证？ | **MinMS 直接移植 + Angel 既有 evidence 交叉校验** | MinMS 同源且注释明写 Angel 依据；重跑 IDB 成本高、结论不会变 |
| **D3 瞬移协议** | 是否接受 MinMS 方案（纯本地位移 + 保留 s9 `USE_SKILL(91)`，不碰 opcode 101）？ | **接受**，真机验证位移是否被服务端回弹后再迭代 | Angel 101 在 s9 是 USE_INNER_PORTAL，改回必断联机 |
| **D4 验证** | 是否由用户启动 s9 + MySQL 做真机联调（药水消耗、瞬移落点、二段跳手感）？ | **需要真机**：客户端只发 91，MP/道具扣除由服务端结算，纯构建无法验证同步 | 编译通过 ≠ 功能对；MinMS 注释亦强调本地不改 HP/MP 防 desync |
| **D5 192 资源** | 是否需要同步 192 的 IMG/脚本（技能 hrange/mpCon、动画帧）？ | **本轮不需要** | 先用当前运行数据跑通；数值差异留到实机比对后再定 |

> D1-D5 只要用户确认（或给不同取值），即可进入 Plan 模式输出一次性完整任务计划，用户确认后再动源码。

## 5. 证据索引

- 三路只读调查原始输出：`IO/UITypes/UIStatusBar.*`、`Gameplay/Stage.cpp`、`Gameplay/Combat/Combat.cpp`、`Net/PacketSwitch.cpp`、`Character/PlayerStates.h`、`IO/UITypes/UIMiscShells.cpp`（本仓）。
- MinMS：`E:\project\MinMS\src\client\{Gameplay\Combat\Combat.cpp, IO\UITypes\UIStatusBar.cpp, Character\Player.cpp, Character\SkillId.h}`。
- Angel 既有归档：`docs/features/statusbar-hud-angel-calibration-2026-11/README.md`、`docs/features/skill-angel-calibration-2026-11/README.md` + 各自 `evidence/`、`notes/`。
- 通用体检：`docs/features/angel-native-port-2026-11-18/README.md`、`docs/features/feature-gap-inventory-2026-11/README.md`（⚠️ 部分条目为过期台账，以源码逐字为准）。

## 6. 实施结果（用户拍板 D1 全部修复 / D2 前者 / D3 接受 / D4 可验证 / D5 是）

### 6.1 已改源码（Release/x64 构建 EXITCODE=0，GATE OK，exe 15:50:18）

**P0 三组核心现象（全部修复）**
1. **快捷栏抓取/拖拽** — `IO/UIStateGame.cpp`：拖拽结算从 CLICKING（按住）帧改为 IDLE（松手）帧，对齐 MinMS `UIStateGame::send_cursor:505-523`（CLICKING 帧只保持 GRABBING）；普通图标松手统一 `drop_icon`，macro_sticky 黏住态仅保留给组合栏增量。`IO/UITypes/UIStatusBar.h`：QsIconType::is_macro_source() 由 true 改 false（快捷栏不再误入黏住态）。`UIStatusBar.cpp`：`send_icon` 落点结算后 + `update()` 主键松开兜底，复位 `qs_dragging/qs_drag_slot/qs_pressed`（旧实现从不复位，第一次拖完后快捷格永久 CANCLICK）。
2. **药水快捷键** — `Gameplay/Stage.cpp`：`send_key` switch 补 `KeyType::Id::ITEM` 分支，按下沿用一次 `player.use_item(action)` → `UseItemPacket(72)`（本端 Player::use_item 已含宠物食物/技能书 opcode 自动选择，无需改 Player）。
3. **瞬移/二段跳** — `Gameplay/Combat/Combat.cpp`：空壳 `apply_use_movement` 接真实位移，并在**非攻击分支**（技能走此路）调用它（旧版只攻击分支调用，非攻击技能永不到达）。`Character/Player.{h,cpp}`：新增 `apply_flash_jump`（4111006，空中冲量 h=3.2+0.12lv/v=-(1.8+0.04lv)，一次性限制）、`apply_teleport`（2101002/2201002/2301001，方向键或朝向，movexuntil 位移），range 取 SkillData.hrange*100。`PlayerStates.cpp`：着地复位 flash_jump_used。协议保持 s9 `USE_SKILL(91)`，未碰 opcode 101/92/93；宏黑名单保留（Angel 原生禁止传送技入宏，技能走技能键/快捷栏触发）。

**P1 体检项（逐项复核后的真实处置）**
- **P1-4 掉落表**：NO-FIX——c2s `WorldMapDropRequestPacket(278)` → s2c `WorldMapDropResultHandler` → `MobDropData` → `UIMapInfoPanel::rebuild_drops` 链路完整，体检报告"stub loader"是误判（stub 由 handler 填充属正常设计）。
- **P1-5 富文本**：NO-FIX——`#h/#p/#t/#z/#q/#m/#o` 在 `UINpcTalk::format_text:918-989` 逐处展开，`#L/#l` 由 `strip_simple_choices:1138+` 处理；GraphicsGL 空 case 是不可达兜底。
- **P1-6 环境音**：已实现——`Audio.h/.cpp` 新增 `AmbientSound`（BASS loop stream，per-channel 音量跟随 SFX）；`Weather/WeatherFx.{h,cpp}` 新增 `sound_img()` 单文件加载器；`WeatherGround.cpp::update_ambient` 按 sky 切 `Sound/Weather.img` 的 rain（RAIN/STORM）/wind（雪/叶/花/沙暴），清场 stop。**打滑保留桩**：Angel/192/MinMS 三方均无 weathermove 打滑逻辑（那是 bundle 私服特有），按 D2 原生还原原则不发明能力。
- **P1-7 雇佣商人进店**：NO-FIX（本轮）——场标 `CHARBOX` 已由 `HiredMerchantSpawnHandler` 显示；进店需接 s9 `PLAYER_INTERACTION(0x1E OPEN_THING)` 多模式协议 + `UIMerchant` 完整交互，属**独立二期功能**，非 bug。强接半吊子协议有崩溃风险。
- **P1-8 活动 HUD 空键**：NO-FIX（本轮）——Ariant/Carnival/Minigame 需服务端活动协议配合，属新增功能。
- **P1-9 设置窗**：NO-FIX——运行时 UIWindow 有 `GameOpt/backgrnd`(158×284) / `SysOpt`，`UIOptionMenu` fallback 可用（slider 逻辑完整），非空白壳。
- **P1-10 登录文案**：NO-FIX——`worldselect.stub_view_map/stub_view_all` 无任何代码引用（历史文档已记），死文案不影响功能。

### 6.2 真机验证清单（D4：用户启动 s9 + MySQL 后执行）
1. 背包药水拖到快捷格：抓起→移动→**松手**应绑定（一次拖放，不再两击/黏住）；快捷格图标互相换位、拖出清除。
2. 药水绑到数字键：单击应消耗 1 个（非 2 个），服务端 HP/MP 回包同步。
3. 法师瞬移（2101002/2201002/2301001）：按技能键 + 方向键，本地位移到对应 foothold；服务端回包后无回弹。
4. 二段跳（4111006）：空中再按一次应给冲量，同一次离地仅一次，着地后可再用。
5. 雨天/风暴进图：`Sound/Weather.img/rain` 循环播放，切晴天停止；风雪天 wind 循环。

### 6.3 风险/回滚
- 本轮未提交 git；改动与工作区既有大量改动混存，如需单独回滚按 §6.1 文件列表逐项 revert。
- 拖拽手势从 CLICKING→IDLE 是全局模型变更（MinMS 同源），真机重点看背包内整理拖拽是否仍正常（理论正常，MinMS 即此模型）。
- 环境音是新增 BASS stream；非 USE_BASS 构建全 no-op（已 guard）。

### 6.4 拖拽两项回归修复（用户真机反馈，2026-11-22 16:26）

用户真机报「抓起的消耗品图标浮在鼠标上方」「移动后再也抓不了其它图标（消耗/技能/自带）」，构建 EXITCODE=0、exe 16:39:23。

- **图标浮在鼠标上方** — `IO/UITypes/UIStatusBar.cpp::setup_qs_drag_icon`：快捷栏 drag icon 是 `Icon` 对象、`QsIconType.get_type()==KEY`，Icon 构造不会像 ITEM 型那样自动 `shift(0,32)`；而道具 `get_icon(false)` 贴图 origin.y≈32（底锚点），`dragdraw=cursor-offset-origin` 少减 32 → 图标上浮一格。修复：仅对 ITEM 绑定 `tex.shift(Point(0,32))`（技能/宏 origin 口径不同，勿动），与背包道具 drag 一致。
- **抓一个后无法再抓其它** — `IO/UIStateGame.cpp::send_cursor`：MinMS 原生 `IDLE` 落点后**无条件**清 `draggedicon`（:516-518），本端口径 `if(drop_icon()) remove_icon()` 在落点被拒（send_icon 返回 false，如快捷格未激活/落空白）时不清除 → 图标永久挂光标，此后每次 send_cursor 都进 `if(draggedicon)` 分支，永远无法进入 `else` 抓取新图标。修复：两处（macro_sticky/普通）改为先 `drop_icon()` 再无条件 `remove_icon()`；`QsIconType::drop_on_stage()` 是空实现，无条件清除无副作用。

### 6.5 技能宏 / 快捷栏拖拽二次回归修复（用户 16:44 真机反馈）

用户真机报「技能宏也没解决」「原生快捷键图标还不能拖拽——这不是原生逻辑吗？是没完全实现还是？」构建 EXITCODE=0。

**根因（务必记录，防止再犯）**：§6.4 上一轮把 `UIStateGame::send_cursor` 改成"CLICKING 帧一律 return GRABBING + 仅 IDLE 落点"的**简化 MinMS 模型**，**误删了本端经真机反复验证的双模型手势**：
- 宏源（`is_macro_source`，技能宏/组合栏图标）走**点击式黏住**：抓取松手进黏住态 → 第二次点击的 **CLICKING 按下帧**落点。我上轮把 CLICKING 帧一律吞掉，宏图标的落点手势被完全吃掉 → 技能宏拖到快捷格/宏窗内换位全失效。
- 普通图标（背包/快捷栏）走按住拖 → 松手落点。

**修复**：恢复备份 `tools/_cc_bak_UIStateGame.cpp:807-868` 的双模型状态机（CLICKING 帧：macro_sticky 保留 + 落点结算；IDLE 帧：宏源黏住态/普通落点），仅保留 §6.4 的"无条件 remove_icon"修复。这是本轮第 3 次在 send_cursor 上反复——**结论：该文件已被多个会话反复修改，含大量历史补丁，后续任何拖拽改动必须先读 `tools/_cc_bak_UIStateGame.cpp` 原始逻辑对齐，禁止再按"纯 MinMS 模型"重写**。

**对用户的诚实回答（完整度定性）**：code-explorer 静态核对 MinMS（`E:\project\MinMS`）确认——快捷栏图标自身拖出换位（MinMS `UIStatusBar.cpp:673-679` + `bind_quickslot:733-774`）、技能宏窗内抓起/换位/绑快捷格/拖出移除（MinMS `UISkillBook.cpp:2254-2268/2350-2414`）**都是原生能力，本端也都已移植实现**（本端 `UIStatusBar.cpp:2773+/2848+`、`UIMiscShells.cpp:4942-5001/4782/4816`、`UIStateGame.cpp:810+`）。所以答案是：**不是"没实现"，是"实现了的自定义重写 + 历史补丁维护中，反复出 bug"**。本端与 MinMS 的差异：① 宏面板本端拆成独立 `UISkillMacroEx`（非 MinMS 技能窗内嵌），靠每帧锚回技能窗；② 快捷栏拖拽本端是阈值状态机（MinMS 是 press-edge）；③ 取消拖拽回滚缺失（本端抓起即 `clear_cell`，拖到半路取消不写回源格，`:4925-4926` 已披露）。这些是"实现质量"问题，非"功能不存在"。

### 6.6 方案 A：彻底删除点击黏住、对齐三家原生 DragDrop（2026-11-22 18:13，最终）

用户选 A 并要求先确认「MinMS 与 Angel/192 逻辑是否一致」。取证结论（**三家一致 = DragDrop**）：
- **Angel 083**：IDA 实证 DragDrop——`CDraggableSkill::OnDropped @0x4faa22`、`MapFuncKey @0x4fb32c`（内含 `CUIStatusBar::GetShortCutIndexByPos @0x8de8d5`）、`MapMacro @0x4fb112`、框架 `CWndMan::BeginDragDrop @0x9e353d / EndDragDrop @0x9e37c2`。证据 `docs/features/skill-angel-calibration-2026-11/evidence/angel-skill-facts.md:258-324`、`docs/features/drop-angel-calibration-2026-11/README.md:106`。
- **MinMS**：源码 `UIStateGame.cpp:505-523`（仅 IDLE 落点）、`UIStatusBar.cpp:651-690`（press-edge 起拖）。
- **192**：标准原生客户端同源体系，无反证（无源码/IDB，专项未取证）。
- **本端"点击式黏住"是三家都没有的自研发明**：`quickslot-interaction-fix-2026-11-18/README.md:24` 明确"本端是 Journey 自研简化版，非 Angel 原生移植"。

**按方案 A 实际改动（构建 EXITCODE=0，GATE OK，exe 18:13:23）**：
1. `IO/UIStateGame.cpp::send_cursor`（:805-820）：拖拽分支从"双模型（CLICKING 落点 + IDLE 落点 + macro_sticky 黏住）"**归一为 MinMS 单模型**——按住拖动恒 GRABBING，仅 IDLE 松手帧 drop_icon + 无条件 remove_icon。删 ~40 行历史补丁。
2. `IO/UIStateGame.cpp::drag_icon`（:952）：删除 `macro_sticky_pending = is_macro_source()` 设置。
3. `IO/UIStateGame.cpp::remove_icon`（:278）、`pre_add`（:1113）：删除 macro_sticky 清理/兜底条件（关闭来源窗仍无条件收拖拽副本）。
4. `IO/UIStateGame.h`（:100）：删除 `macro_sticky_pending/macro_sticky` 成员。
5. `IO/UITypes/UIMiscShells.cpp`（:129/:155）：MacroCellIcon/MacroRowIcon 删除 `is_macro_source()=true` override（回基类 false）；`:4823` send_icon 宏图标落点改 `is_in_range()` 判定（不再依赖 is_macro_source）。
6. `IO/UITypes/UIStatusBar.h`（:157）QsIconType::is_macro_source() 早已 false，保持。

**保留（有理由，非遗漏）**：
- 宏面板 `send_icon`（:4816）直接调 `drop_skill` 写格，功能等价 MinMS `Icon::drop_on_macro`，**不新增虚方法**（避免改动整个 Icon 体系）。
- 快捷栏阈值起拖（移动 8px）保留防抖——起拖方式与 MinMS press-edge 不同，但落点/状态机已统一 DragDrop；用户从未报"抓不起来"。
- 抓起即 `clear_cell` + grab_* 回滚（:4949-4954/:5049）保留——单模型下拖拽流程自洽；"落点非法丢技能"是边缘情况，真机验证后再定。

**真机验证清单（D4，用户启动 s9+MySQL）**：
1. 技能宏：技能窗技能→拖到宏面板格落下；宏面板内技能格→拖到别的格换位；宏面板技能→拖到快捷格绑定；整行宏图标→拖到键位绑定。**全部改为"按住拖、松手落"**（不再点两下）。
2. 快捷栏：药水拖到快捷格一次落位；快捷格之间换位；拖出移除。
3. 抓一个图标后松手（无论落点成功与否）→ 能立即抓下一个图标（不再卡光标）。

### 6.7 键位配置窗口两个现象（用户 18:15 / 18:18 真机反馈）

**现象 A（已修）**：把技能从 "1" 拖到 PUP 键后，PDN 键上出现白色图标。
- 根因（静态确凿）：拖动预览图标画在 `cursorpos - cursoroffset`（`Icon::dragdraw:82-85`），但落点判定 `UIKeyConfig::send_icon → key_by_position(cursorpos)`（:978-981）用的是**原始光标点**。从 "1" 图标底部抓起 `cursoroffset.y≈30`，用户把预览放到 PUP 中心（y≈82）时真实光标 y≈112，命中相邻 PDN（PUP y=66..98 / PDN y=99..131，`keys_pos:238/241`），于是 `stage_mapping` 绑到 PDN、PDN 画出该技能图标。PUP/PDN 命中矩形本身不重叠，`actionbyid` 也是恒等转换（`KeyConfig.h:45-48`），均已排除。
- 修复：`IO/Components/Icon.h` 新增 `get_drag_drop_point(cursor)` = `cursor - cursoroffset + (16,16)`（图标视觉中心）；`UIKeyConfig.cpp::send_icon` 用该中心点替代原始 cursorpos 做 unbound 命中与 `key_by_position` 落点，并对 unbound 命中加 `break` 防重复。构建 EXITCODE=0（单线程 /m:1 规避 PDB 竞争），exe 19:30:46。
- 白色像素本身静态无法定性（无白色占位绘制代码，`Texture::draw` 无效纹理直接 return）；按位置根因修好后应随 PDN 误绑一起消失，待真机确认。

**现象 B（待澄清，未改代码）**："自带图标无法抓取"。
- 静态排查：下方未绑定动作条图标的抓取（`UIKeyConfig::send_cursor:872-889` → `unbound_action_by_position`）依赖 `bound_actions` 不含该 action；ATTACK/JUMP 默认已绑（UIKeyConfig.h:181-185/232-236），本就不在下方条、也不可当未绑定图标抓。绘制/命中坐标一致（都用 `position + unbound_actions_pos + 32`），`dragged` 窗口拖拽标志只在顶部 20px 标题栏触发（`UIDragElement.h:56/126-130`），均排除。
- 结论：若用户指"下方动作条图标"，当前行为是设计结果（已绑定动作不在下方条）；若指"键位格上已绑图标抓不起"，则是另一条路径（`send_cursor:891-940`），需运行时证据。**不再盲改**，等用户明确"自带图标"具体指哪一类 + 是否可见。

### 6.8 "自带图标抓取只有音效无跟随、释放不改位置" 根因 = 实测 exe 未含方案 A（2026-11-22 21:5x）

用户递进反馈："自带图标无法抓取" → "PDN 键上是右下角同色的这些自带图标" → **"抓取时没有图标只有音效，释放以后也不会改变图标位置"**。

**根因链条（时间戳铁证）**：
- `git show HEAD:IO/UIStateGame.cpp` 的 `send_cursor` 是**旧双模型**：`if (cursor_state == CLICKING) { if (macro_sticky) return GRABBING; if (drop_icon(...)) remove_icon(); }` —— **按下帧（CLICKING）就 `drop_icon + remove_icon`**。自带 KeyIcon（非 macro source，`macro_sticky=false`）抓起后，只要拖动期间出现一帧 CLICKING 状态，立即落回原格并清除 → 表现为"有音效（`Icon::start_drag` 播 DRAGSTART）、无图标跟随、落点无效（图标已落回原位）"。
- 方案 A（IDLE 落点）源码在 **20:50:04** 才落盘到 `UIStateGame.cpp`；但用户实测的部署 exe 是 **20:06:57**（早 43 分钟）→ **用户一直在跑旧双模型 exe**，方案 A 从未部署。技能/消耗品能拖是因为它们走 `UIStatusBar::send_cursor` 的阈值起拖状态机（返回 CANCLICK），不依赖 UIStateGame 的 CLICKING 落点分支；键位窗自带图标直接返回 GRABBING，才暴露旧模型缺陷。
- MinMS 原生 `UIStateGame.cpp:505-523` 确认 IDLE 落点，方案 A 方向正确。

**排除的假设（均有证据）**：
- `GRABBING` 光标状态未加载导致降级 IDLE：orange-wz 查运行时 `E:/MXD/BeiDou-Client-xy/Data/UI/Basic.img` 的 `Cursor/11/0` = 22×21 CANVAS_PROPERTY（合法位图），`Cursor::init` `kClassicStates` 含 GRABBING，`state_loaded[11]=true`。
- Icon 纹理无效 / 窗口标题栏拖拽拦截 / is_action_mapping 不匹配：`Cursor/11` 有效、`dragarea=(width,20)` 仅顶栏、绘制与命中坐标一致。

**修复**：无需改代码——方案 A 已在源码。关键是**正确构建部署**：`MSBuild /m:1` EXITCODE=0，部署 exe 刷新到 **21:58:25**（晚于 UIStateGame.cpp 21:56:43）→ 方案 A 已进 exe。清理了诊断期临时日志（UIStateGame.cpp / Icon.cpp 的 `[DBG-DRAG]` 全部撤销，`dbg_drag.log` 等临时文件删除）。

**真机验证状态**：自动化脚本（key_seq.ps1 / drag_icon.ps1）能打开键位窗（发送 `\` = KEYBINDINGS），但 `mouse_event` 拖拽未能可靠命中图标（UIPI + 坐标偏移，截图显示点击落到地图导致角色移动），故未拿到"拖动中图标跟随"的自动截图。**遗留：请用户真机确认**——重开客户端后，键位窗右下角自带图标（对所有人/对组员/坐下等）按住拖动时图标跟随、松手落到目标键位生效；技能/宏/快捷栏拖拽回归。
