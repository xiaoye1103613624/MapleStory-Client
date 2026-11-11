# P3 B 批 5 条 083 三向对拍结论（round98）

> 口径：`083/v83 only 禁 095`；`s9 协议优先`（W 桶 6 字段宽度/类型 + 27 wire KEEP 零改动）；
> `数值重合 ≠ 语义重合`；`校准 ≠ 改回原生`（L 桶同字段原始逻辑按 083 还原）。
> 本轮 5 条 **全部零 wire / 零协议 / 零 WZ / 零源码行为改动**。
> 每条结论必附四项：083 符号或反编译、s9 对端、当前端字面量、DECISIONS 原裁定。

## 0. 结论总表

| # | 决策点 | 判定 | 依据强度 | 本轮动作 |
| --- | --- | --- | --- | --- |
| 10 | `skill/D9` classic 双布局 | `KEEP`（环境适配增量 E13，非缺陷） | 083 反编译 + IMG 门禁 | 只出对照，零改动 |
| 11 | `social-economy/D12` wedding 键 | `P2`（需逐键对照 083 String.img） | 083 反编译（仅 4 组 SP_）+ Locale 实测 | 登记 P2，不出键值 |
| 12 | `pet-summon/D2` 两处字节丢弃 | `LOCAL_TYPE`（本地 int32→int64，wire 不动） | 083 反编译 + s9 源码 + 本端全链路 | 登记补齐清单，本轮零改 |
| 13 | `login-flow/D13` CHARLIST 15 槽 | `DOC_ONLY`（槽数是尾部 int32，非定长） | s9 源码 + 本端 handler | 只出对照，零改动 |
| 14 | `social-economy/D9` 三整族 | `三分`（不发明 / 登记 / 已对齐） | 083 符号族 + IMG 节点 + 本端类 | 只出族级对照 |

## 1. `skill/D9` — classic 双布局不是缺陷，是环境适配增量

**083 侧（反编译 + 符号）**

- `CUISkill::OnCreate @0x8aaa88`（402 行）：`SPUP` 滚动条 `a3: 2001, a4: 1, a5: 3, a6: 153, a7: 99`；4 颗加点钮 `a3: v83 + 2010, a4: 131, a5: Unknown` with `Unknown = 119; ... while (Unknown < 279)`（x = 119/159/199/239，y = 131）；宏钮 `a3: 2023, a4: 120, a5: 265`。**全程单一布局，无任何 `classic`/`modern` 分支**。
- `*CUISkill*` 全族 10 方法，无 `main` 相关、无 `SetLayout`/`ChangeLayout` 类方法。

**IMG 侧（orange-wz 128B 门禁）**

- `UIWindow.img/Skill` 直接子节点实测 11 个：`backgrnd / skill0 / skill1 / line / BtSpUp / Tab / tip0 / tip1 / tip2 / CoolTime / recommend` —— **无 `main`**（round96 已坐实，本轮复核仍一致）。

**当前端侧**

- `IO/UITypes/UISkillBook.cpp:154` `classic = !UiAssetResolve::usable(Skill["main"]);` —— 因为 083 无 `main`，本端在 083 资源下 `classic` **恒为 true**，走的就是 083 那套经典布局（y131 四钮 + 2023 宏钮路径）。
- `:94` `textcolor = classic ? BLACK : EMPEROR`、`:259` `tab_plate = classic ? window("Item")["New"] : nl::node()`、`:303` `if (classic && spupid >= CLASSIC_ROWS)` 等 11 处分支，全部是「有 `main` 走新版、无 `main` 走 083 经典」的**资源可用性降级**，不是双业务布局。

**s9 侧**

- `DistributeSPHandler.java:32-33` `p.readInt(); int skillid = p.readInt();`、`SkillMacroHandler` 上限 5 槽 —— 与布局无关，纯数值/槽位上界，两套布局共用同一协议。

**DECISIONS 原裁定**：`:195` `skill/D9`「双布局分支 classic 是否收敛为原生单布局 | 保持双布局 | `BATCH_1_KEEP`」。

**判定**：`KEEP`。双布局是本端为「未来可能带 `main` 的资源包」预留的降级路径（`skill-angel-calibration-2026-11/README.md:164` E13 已记为「环境适配增量」）；在当前 083 资源下与 083 行为一致，**不构成缺陷，不收敛**。

## 2. `social-economy/D12` — wedding 键不是 26 个，且 083 侧只有 4 组 SP_

**当前端侧**

- `locale/zh-CN.ini:302-321` 与 `locale/en-US.ini` 各有 **27 个 `wedding.*` 键**（不是 Plan/DECISIONS 写的 26）；**zh-CN 的值也是英文原文**（决策 B「保持英文原值、只建键」的落地结果）。

**083 侧（反编译）**

- `CField_Wedding::OnWeddingProgress @0x58153d`（316 行）只出现 4 组字符串：
  - `SP_4202_SOUND_BGMGLIMG_CHAPEL` @`0x58166d`
  - `SP_4203_SOUND_BGMGLIMG_CATHEDRAL` @`0x5816af`
  - `SP_4204_DO_YOU_WANT_TO_BLESS_THIS_COUPLE` @`0x581785`
  - `SP_1666_STRING_NPCIMG` @`0x581882`
- **不是 26/27 键一一对应**：083 这 4 组是「教堂/大教堂 BGM 名 + 祝福确认问句 + NPC 图占位」，其余本端 23 键属本端婚礼流程的**自有提示文案**（`wedding-flow/README.md` 11 类封包驱动的本地回显）。

**s9 侧**

- `WeddingPackets.java` + s2c 72/73/74/75/320/321/43 —— 协议面已闭环（`social-economy-angel-calibration-2026-11/README.md:121` A11 ✅），文案不在协议里。

**DECISIONS 原裁定**：`:122` `social-economy/D12`「保持决策 B『保持英文原值、只建键』，等 083 String.img / IDA 依据再换 | `BATCH_1_KEEP`」。

**判定**：`P2`。083 侧仅 4 组 SP_ 可换；要动必须先逐键对照 `String.img`（本轮未做，不发明键值）。**本轮零改动**，登记 P2（原 `BATCH_1_KEEP` 保留，仅补「键数 27 非 26、083 可换 4 组」这一层事实）。

## 3. `pet-summon/D2` — 丢弃点在本地结构，不在 wire

**083 侧（反编译）**

- `CPet::Init @0x703314`（170 行）逐序：`Decode4` itemid → `CPetTemplate::GetPetTemplate` → `DecodeStr` name → **`CInPacket::DecodeBuffer(a1: this + 160, a2: 8u)`（8 字节 uid 整块写入 `this+160`）** → `CPet::GetItemSlot` → `CPet::SetShoeAttr` → `*(this+110)=Decode2` / `*(this+111)=Decode2` / `*(this+60)=Decode1`(stance) / `Decode2`(foothold) → `CVecCtrlPet::Init`。
- **083 侧完全没有 `nameTag`/`chatBalloon` 两个 bool** —— 这两个字段是 s9 自己加的。

**s9 侧**

- `PacketCreator.java:4570-4584 addPetInfo`：`p.writeLong(pet.getUniqueId()); p.writePos(...); p.writeByte(stance); p.writeShort(fh); p.writeBool(hasNameTag); p.writeBool(hasChatBalloon);`（8B uid 与 083 一致；两 bool 是 s9 扩展）。
- `MovePetHandler.java:36-37`：`int petId = p.readInt(); p.readLong();` —— **c2s 的 pid 是 int32**。

**当前端侧（全链路）**

| 位置 | 类型 | 说明 |
| --- | --- | --- |
| `Net/Handlers/MapObjectHandlers.cpp:521` | `read_long()` | 8B uid 读对了（与 083 `DecodeBuffer(8)` 一致） |
| `Net/Handlers/MapObjectHandlers.cpp:526` | `static_cast<int32_t>(uniqueid)` | **截断点 1**（传参收窄） |
| `Character/Char.h:116` / `Char.cpp:316` | `add_pet(..., int32_t uniqueid, ...)` | **截断点 2**（接口签名） |
| `Character/Look/PetLook.h:72` | `int32_t uniqueid;` | **截断点 3**（存储字段） |
| `Character/Inventory/Pet.h:31` | `int64_t get_uniqueid()` | **背包侧已是 int64**（说明扩宽可与背包对齐） |
| `Net/Packets/PetPackets.h:58-69` | `MovePetPacket(int32_t pet_unique_id, ...)` → `write_int` | 出站 int32，**与 s9 `readInt` 一致，绝不能改** |
| `Net/Packets/PetPackets.h:89-100` | `PetCommandPacket(int32_t pet_unique_id, ...)` → `write_int` | 同上 |

**DECISIONS 原裁定**：`:131` `pet-summon/D2`「保持 s9 联机优先，改回原生会断联机，交裁决 | `BATCH_0_BASELINE`」。

**判定**：`LOCAL_TYPE`。083 `DecodeBuffer(8)` 与 s9 `writeLong` **宽度相同**，wire 无分叉；丢的是本端把 8B 塞进 `int32_t` 的**本地展示/寻址结构**。补齐动作 = 把 `PetLook.uniqueid` / `add_pet` 参数 / `:526` cast 三处扩到 `int64_t`，**发包侧保持 int32 不动**（发包 pid 是从 8B 里取的 pid 段，s9 也只收 int32）。→ 本轮**零改动**（须评估 `UIMiscShells.cpp:1864 look.get_uniqueid() == uid32` 等比较点），登记为「可自测的 L 桶本地类型扩展」。

另两处 `(void)` 丢弃（`MapObjectHandlers.cpp:520-523` nameTag/balloon、`:1414-1417` summon stance/movetype/attacking）：083 侧对应字段**根本不存在**（`CPet::Init` 无两 bool，`*CDragon*` 10 符号全无 Packet/Decode），属 s9 扩展字段 → 按 m09729「扩展功能可以不对齐」**保持丢弃**，只补注释。

## 4. `login-flow/D13` — 15 槽是展示层容量，不是 wire 定长

**s9 侧**

- `PacketCreator.java:935 getCharList`：`p.writeByte(status); p.writeByte((byte) chars.size()); for (Character chr : chars) addCharEntry(p, chr, false); p.writeByte(pic); p.writeInt(collective_chr_slot ? chars.size() + c.getAvailableCharacterSlots() : c.getCharacterSlots());`
- → **按 `chars.size()` 循环，不是固定 15 次**；槽数只是**尾部一个 int32**（`getCharacterSlots` 是账号容量，展示用）。

**083 侧**

- `CLogin::SendSelectCharPacket @0x5f726d`（177 行）：`COutPacket(30)`+`EncodeStr(name)`+`Encode4(v19)`；`COutPacket(19)`+`Encode4(v19)`；`COutPacket(29)`+`Encode1(1)`+`Encode4(v19)` —— 只有选角请求三形态，**与槽容量无关**。

**当前端侧**

- `Net/Handlers/LoginHandlers.cpp:290-330`：`int8_t charcount = recv.read_byte();` → `for (uint8_t i = 0; i < charcount; ++i) characters.emplace_back(LoginParser::parse_charentry(recv));` → `int8_t pic = recv.read_byte();` → `int32_t slots = recv.read_int();` → `while (recv.available()) recv.skip_byte();`
- `IO/UITypes/UICharSelect.cpp` **无 15 槽常量**（只有 `CS_SLOT_W = 183` / `CS_SLOT_H_EMPTY = 115` / `CS_SLOT_H_OCCUPIED = 151` 等单槽几何）。

**DECISIONS 原裁定**：`:66` `login-flow/D13`「CHARLIST 外层固定 15 槽 vs 按 charcount | 保持 s9 | `BATCH_0_BASELINE`」。

**判定**：`DOC_ONLY`。「15 槽」是 `getCharacterSlots()` 的账号容量值（服务端配置），**不是包体定长**；两端都按 `charcount`/`chars.size()` 循环。Plan 原写「改则只改本地槽数组」的前提不成立 → 无需改动。

## 5. `social-economy/D9` — 三整族三分，不是一刀切「不做」

| 族 | 083 符号 | IMG 节点（128B 门禁） | 当前端 | 判定 |
| --- | --- | --- | --- | --- |
| `UIFriendGroup` 好友分组独立窗 | `*FriendGroup*` 3：`LoadFriendGroup@CConfig 0x49e6fb` / `SetRet@CUIFriendGroup 0x80f0eb` / `OnOK@CUIFriendGroup 0x80f93e`（`OnOK` 90 行反编译无 `COutPacket`） | `search FriendGroup` → **`matches: []`（0 命中）** | 无 `UIFriendGroup` 类；只有 `UIUserList.cpp:1305 friend_group_bar = bitmap(Friend, "friend4")` 一条 281×27 横条 + `:1417 Locale::text("userlist.friend_group") + "(0/0)"` | **不发明**（083 无独立资源根、无发包；s9 无后端） |
| 公会徽章编辑 | `*GuildMark*` 1：`SendSetGuildMarkMsg@CField 0x530f2f`（17 行全文 = `COutPacket(126)`/`Encode1(15)`/`Encode2`/`Encode1`/`Encode2`/`Encode1`/`SendPacket`，**7B 定长**） | `search GuildMark` → `UserList/Guild/GuildInfo/guildmark` `CANVAS_PROPERTY`（真节点在），`GuildMark.img` 亦存在于 `Data/UI/` | `WaveAShellHandlers.h:941/:947` 只有 `GuildNameChangedHandler`/`GuildMarkChangedHandler` **声明**；`Net/Packets/GuildPackets.h` **无 guildmark 发包封装**（grep 空） | **登记**（有节点、有 083 包形、无出站封装）→ PLT 真机项 + P3 候选 |
| MapleTV 完整播 | `*MapleTV*` 6：`IsLoadFlash@CMapleTVMan 0x63764c` / `SetMapleTVMessage@CNpc 0x6d417e`(0x8e4) / `DrawMapleTVMessage@CNpc 0x6d4ad4`(0xd3d) / `GetSenderName 0x6d5811` / `GetReceivername 0x6d5835` / `GetText@CUIMapleTV 0x7f0dc9`(0x273) | `search MapleTV` → `MapleTV` `LIST_PROPERTY`（UIWindow.img 顶层）；`Data/UI/MapleTV.img` 存在 | `IO/UITypes/UIMapleTV.h` 类已实现（`UIDragElement<PosNOTICE>`、`BtOk/BtCancel`、`has_assets/backgrnd/tip_label`） | **已对齐壳层**（播放主体依赖 CNpc 绘制栈，属 F 维，非 D9） |

**DECISIONS 原裁定**：`:119`「保持明确不做/搁置：083 顶层清单无独立窗、s9 无后端 | `BATCH_1_KEEP`」。

**判定**：三分。**`FriendGroup` 维持不做**（083 无窗无包）；**`guildmark` 从「不做」改判「有节点无发包」**（083 有 7B 包形 `126/15`，本端缺出站封装，可立项 P3）；**`MapleTV` 从「整族缺失」改判「壳层已对齐」**（类已存在，播放栈属 F 维）。

## 6. 算术不动

- `168 = PURE76 + WIRE38 + MIXED14 + OTHER40`（round88 三分类，本轮零改）
- `168 = W轨38 + L轨130`（WLE §4，本轮零改）
- `reopen 27 = 已注销 10 + 残余 17`；`残余 17 = P1 0 + P2 1 + P3 16`（round94，本轮零改）
- 本轮 5 条**全部已在 16 条 P3 内**，不新增分母、不注销、只补判定层与事实层（27 键非 26 / 083 可换 4 组 / uniqueid 截断点 / 15 槽非定长 / 三族三分）。

## 7. 验证

- 静态守卫 + 场景：`tools/test_round98_p3_batch_b.py`
- `python tools/gen_opcode_matrix.py --check` → `GATE OK`
- 本轮零源码改动 → 只做构建门禁回归，**不声称真重编**
- 提交零署名（`git log -1 --format=%B` 签名行 0）

## 8. 约束

- 083/v83 only，禁引 095
- s9 协议优先：W 桶 6 字段宽度/类型 + 27 条 wire KEEP 全程零改动
- 数值重合 ≠ 语义重合（`map/D8` 32B / `pet` 8B uid 均按此判）
- 校准 ≠ 改回原生（L 桶同字段原始逻辑按 083 还原）
- WZ/IMG/XML 一律先 orange-wz MCP（128B `WzAESConstant.DEFAULT_KEY`，带 `MCP-Protocol-Version: 2025-03-26` 头，成功判定看 `loadedCount`）
- 门禁 5 禁想象修改 / 门禁 18 REA→IDA→orange-wz→.nx 只读取证（截图仅备选）
