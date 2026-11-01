# r805 · J6 战神 / buff 掩码：64 位掩码解析改造 + 连击由攻击命中驱动

对应任务 #73（J6 战神：连击驱动 + 64 位 buff 掩码 + PoleArm 姿态动画）。
证据来源：服务端 s9 源码 `E:\project\BeiDou-Server_s9`（`gms-server`，`org.gms.*`）+
部署客户端 `E:\MXD\BeiDou-Client-xy\Data\.nxcache\*.nx`（`tools/nx_inspect.py`）。
本轮 IDA MCP / orange-wz MCP 不在会话工具目录内，所以所有结论只用「服务端源码 + nx 节点实测」两条路。

## ① GIVE_BUFF / CANCEL_BUFF：从「枚举撞值」改成「按掩码位消费」

### 问题

旧解析写的是 `switch (secondmask) case Buffstat::BATTLESHIP: return;`。
`BATTLESHIP` 是**枚举序号 21**，不是掩码位；服务端写的是 `1L << 21` 这一位对应的
数值，二者永远不可能相等——但如果真相等（其他位组合命中），整包剩下的 buff 数据块会被
直接 drain 掉。更根本的问题是：客户端的 `first_codes/second_codes` 表和服务端
`BuffStat.java` 的位号有多处不一致（见下），缺位/错位会让 10 字节数据块少读或多读，
一个 buff 读错，**之后所有 buff 全体错位**。

### 修

`Net/Handlers/PlayerHandlers.cpp` `BuffHandler::handle`：

- 先读 `firstmask`、`secondmask` 两个 long；
- 对每个掩码逐 bit 遍历，命中位就 `handle_buff(recv, Buffstat::by_code(code, first))`；
- `by_code` 认不出的位**照样消费一块**（`ApplyBuffHandler` 读 short+int+int 共 10 字节，
  `CancelBuffHandler` 不读；`Id::NONE` 时只丢状态不写 player），所以「表不全」不再等于
  「流错位」；
- 收尾 `while (recv.available()) recv.skip_byte();` 兜住未知尾部，再 `recalc_stats(false)`。

`Character/Buff.h` 新增 `extern const std::unordered_map<Id, uint64_t> first_codes / second_codes;`
和 `Id by_code(uint64_t code, bool first);`，`Buff.cpp` 两张表**逐位对齐**
`org.gms.consts.BuffStat.java:24-149`。修正 4 类脏数据：

1. `PUPPET` 服务端是 `0x4000`，客户端原先挂在 `0x800000000000000`（那是 PICKPOCKET 位），
   并与 PICKPOCKET 同值——同位匹配两个枚举就会把一块读两遍。同理删掉与 HANDS 同值的
   `SHOWDASH`、与 COMBO 同值的 `SUMMON`（服务端 COMBO=SUMMON=`0x20000000000000` 本就撞位，
   表里只留一个）。
2. `BATTLESHIP` 的 `0xA00000040` 是多位合成值，服务端从不这样写；留着会让任何带
   `MANA_REFLECTION/WDEF/MDEF` 的包被误判成多一块。
3. `SPEED_INFUSION` 服务端为 `0x40000000000000`，客户端多左移了两位。
4. 补上服务端有、客户端缺位的 `MAGIC_SHIELD`（`0x400000000`）/ `MAGIC_RESISTANCE`
   （`0x800000000`）——同时把两个枚举追加到 `Buff::Id` 末尾（`LENGTH` 之前），
   不动既有枚举序号，避免其他模块的硬编码。

### 关键协议事实（务必记住）

`PacketCreator.writeLongMask`（`PacketCreator.java:3050-3076`）**按 `isFirst()` 分两半写**，
但两半用的是**同一套 bit 位号**：`ARAN_COMBO` 在 first 是 `0x1000000000`，`ACC` 在 second
也是 `0x1000000000`。所以：

- 块的**个数 = popcount(first) + popcount(second)**，不能用 `popcount(first | second)`
  （两者同位时会少数一块 → 又一次全体错位）。
- 顺序是「先所有 isFirst 的位、再另一半」，且 javadoc 要求 `statups` 列表顺序 ==
  `BuffStat` 声明顺序（bit 升序）。

`Net/Handlers/MapObjectHandlers.cpp` `GiveForeignBuffHandler::handle` 按此重写：改用
`popcount(first) + popcount(second)` 计数；队友同时带多个可见 buff 时**逐个**
`combat.show_buff`（原实现命中第一个就 `break`，只显示一个）。

## ② 战神连击：本地猜数 → 攻击命中驱动 + 服务端权威显示

- `Net/Packets/AranPackets.h`：`AranComboPacket` = `OutPacket::Opcode::ARAN_COMBO_COUNTER`
  （163 / `0xA3`，**空包**），注释记录服务端契约。
- `Gameplay/Combat/Combat.cpp`：`AttackPacket` 之后，命中 `result.mobcount > 0` 且职业是战神
  且不是终结技时，**每个被命中的 mob 发一包**（服务端每收一包 +1）。
- 职业门照抄 `GameConstants.isAran`（`GameConstants.java:513-515`）：本 fork 战神 id 是
  **2000 与 2100–2112**（`Aran.java` 技能是 8 位 `21xxxxxx`，**不是** v83 的 `22xxxxxx`）。
  若按 22xx 判定，服务端 `AranComboHandler` 会整包忽略 → 连击永远不涨。
- 终结技排除 `21100004` Combo Smash / `21110004` Fenrir / `21120006` Combo Tempest
  （`RangedAttackHandler.java:86-105` 命中时服务端直接 `setCombo(0)`），否则刚清空又 +1。
- `IO/UITypes/UIComboCounter.*`：
  - 只依赖入站 `SHOW_COMBO`（`0xE1` = 225，`PacketCreator.showCombo` 只有一个
    `writeInt(count)`；`Character.setCombo` 上限 30000，且**只在 count>0 时下发**），
    客户端不再自行累加。
  - 视觉隐藏超时从 4000ms 改成 **3000ms**，与服务端 `AranComboHandler.java:41`
    「距上次超过 3000ms 且连击>0 就归零」一致；服务端不会主动下发 0，所以归零只能靠本地
    超时表现。
  - 删掉 `send_key` 调试分支（软键盘手填连击、手动 `AranComboPacket`），临时日志一并清理。
    `UIElement::send_key` 是空 virtual 默认（`UIElement.h:202`），类仍是具体类。

## ③ 封包宽度复核（无需改动）

- `SHOW_COMBO` 225/`0xE1`：单个 `writeInt` ✓（`PacketCreator.java:7309`）。
- `ARAN_COMBO_COUNTER` 163/`0xA3`：空载荷 ✓（`PacketProcessor.java:303` → `AranComboHandler`）。
- GIVE_BUFF 每块 `writeShort(value) + writeInt(buffid) + writeInt(bufflength)` ✓
  （`PacketCreator.giveBuff`），与客户端 short+int+int 消费一致。

## ④ PoleArm 姿态 / 动画键：**查证后判定「无缺陷」，不做想象修改**

原怀疑点是「战神动作取不到 PoleArm 的 stance/动画键」。实测结论相反：

1. `Skill.nx` 顶层是各职业的 `<ns>.img`（如 `211.img`），技能路径
   `Skill.nx/211.img/skill/<8位id>`；逐个 dump 战神技能 `action/0` 后，字符串全部是
   `swingP1 / swingP2 / stabT1 / stabT2 / alert ...` 这类**标准 stance 名**。
2. `Skill.cpp` 的动作选择：只有 `action/0` 时走 `SingleAction`（`SkillAction.cpp`），
   它调 `target.attack(action)`；`Character/Look/BodyDrawInfo::init()` 会遍历
   `Character.nx/00002000.img` 全部子帧，把带 string `action` 的帧建成
   `body_actions[name][frame]`（`BodyAction` = 重定向到 `Stance::by_string(action)` + frame +
   move + delay）。也就是说「长柄武器专属动作」是**通过 body_actions 重定向到标准姿态**的，
   不需要新增 Stance 枚举。
3. 武器描画在 `Character.nx/Weapon/0<itemid>.img`（不是 `PoleArm.img`；
   `0144xxxx` 前缀 80 条即 PoleArm）。`Clothing` 只加载 `Stance::names` 里的动画，
   而 `Weapon/0144xxxx.img` 里这些标准 stance 都在；`info/attack = 2` →
   `CharLook::getattackstance` 映射 `SPEAR = 2`，`twohanded` 对前缀 140–144 为 true。

所以：**没有证据支持「缺 PoleArm 姿态/动画键」**，本轮不写任何伪造的补丁。
留两个真实的小尾巴（都不是崩溃级）：
- `UI.nx/UIWindow.img` 在本客户端**没有** `Combo` / `Number` 节点（`nx_inspect` 验证），
  连击 HUD 只能保持纯文字；要原生外观需要补 UI 资源（另一条线）。
- `first_codes` 里 `PYRAMID_PQ = 0x20000000000` 这一位服务端实际是 `BERSERK` 家族位，
  属**纯命名的 cosmetic 标签错误**，位号正确、行为正确，不改（改了要动枚举，收益为零）。

## 已知遗留（转入 #72 J5 / #74 J7）

- 客户端职业名表是否覆盖 id 2100–2112（J2 跟进项）。若名表缺项，新的 `is_arleg` 门有真实
  职业之后仍需实测建号选战神。
- Cygnus 四转在本版本是空壳：`1112/1212/1312/1412/1512` 的 `img.xml` 都是 278 字节 stub，
  等级上限硬编码 120（J5 ⑥ 文档项）。

## 构建

`cmd //c "tools\\build_smoke.bat"` → `GATE OK: opcode matrix matches both sources (s9 44d0d5d9ca)`
/ `GATE OK` / `EXITCODE=0`。
因 `ARAN_COMBO_COUNTER/163` 新增了真实发送点，先用
`python tools/gen_opcode_matrix.py` 重新生成矩阵，否则门禁报 `GATE STALE`。
