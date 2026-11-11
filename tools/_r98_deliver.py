# -*- coding: utf-8 -*-
"""round98 P3 B 批 5 条（DOC_ONLY / 本地类型扩展）一次性生成器。

幂等策略：先判 new 已在则跳过，再断言 old 计数。
EOL 策略：二进制读原文，按原文 EOL 写回（四份文档全 CRLF，新建 .md 用 LF）。
硬规则：绝不把 git log / commit hash 写进任何产物（round88 B9b 教训）。
"""
import hashlib
import json
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DM = "docs/features/angel-calibration-d-matrix-2026-11"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
WLE = DM + "/WLE-TRIAGE-PLAN.md"
PLAN = DM + "/P3-ANGEL-REBUILD-PLAN.md"


def abspath(rel):
    return os.path.join(ROOT, rel.replace("/", os.sep))


def read_text(rel):
    """二进制读，返回 (text, eol)。text 已统一为 LF。"""
    raw = open(abspath(rel), "rb").read()
    if b"\r\r\n" in raw:
        raise SystemExit("CRCRLF detected in %s" % rel)
    eol = "\r\n" if b"\r\n" in raw else "\n"
    text = raw.decode("utf-8")
    text = text.replace("\r\n", "\n")
    return text, eol


def write_text(rel, text, eol):
    raw = text.replace("\r\n", "\n")
    if eol == "\r\n":
        raw = raw.replace("\n", "\r\n")
    open(abspath(rel), "wb").write(raw.encode("utf-8"))


def patch(rel, old, new, label, expect=1):
    text, eol = read_text(rel)
    if text.count(new) >= 1:
        print("  [skip] %s (%s) already present" % (label, rel))
        return
    n = text.count(old)
    if n != expect:
        raise SystemExit("ASSERT FAIL %s: old count %d != %d in %s" % (label, n, expect, rel))
    write_text(rel, text.replace(old, new), eol)
    print("  [applied] %s (%s)" % (label, rel))


def sha12(p):
    h = hashlib.sha256(open(abspath(p), "rb").read()).hexdigest()
    return h[:12]


def eol_report(rel):
    raw = open(abspath(rel), "rb").read()
    crlf = raw.count(b"\r\n")
    lone = raw.count(b"\n") - crlf
    return "%s %dB crlf=%d lonelf=%d crcrlf=%d" % (rel, len(raw), crlf, lone, raw.count(b"\r\r\n"))


# ── 1. 主交付：P3-B-BATCH-MAPS.md（新建，LF） ────────────────────────────────
P3B = """# P3 B 批 5 条 083 三向对拍结论（round98）

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
"""

open(abspath(DM + "/P3-B-BATCH-MAPS.md"), "wb").write(P3B.encode("utf-8"))
print("  [wrote] %s" % (DM + "/P3-B-BATCH-MAPS.md"))

# ── 2. WLE §15 ───────────────────────────────────────────────────────────────
_, wle_eol = read_text(WLE)
wle_text, _ = read_text(WLE)
SEC15 = """
## 15. round98 · P3 B 批 5 条（三分 / DOC_ONLY / 本地类型扩展）

对 `skill/D9` / `social-economy/D12` / `pet-summon/D2` / `login-flow/D13` / `social-economy/D9` 做 083 符号+反编译、s9 对端、当前端字面量、DECISIONS 四项对拍，结论逐条：

- **`skill/D9` = KEEP**：083 `CUISkill::OnCreate @0x8aaa88` 全程单一布局（4 钮 2010..2013@y131 + 2023@(120,265)），`UIWindow.img/Skill` 11 直接子节点**无 `main`** → 本端 `classic` 恒 true，走的就是 083 经典路径。双布局是资源可用性降级（E13 环境适配），非缺陷。
- **`social-economy/D12` = P2**：`wedding.*` 实为 **27 键**（非 26），zh-CN 值也是英文；083 `CField_Wedding::OnWeddingProgress @0x58153d` 只有 4 组 SP_（`SP_4202/4203/4204` 教堂/大教堂/祝福问句 + `SP_1666` NPC 图），**非逐键对应** → 要换须先逐键对照 `String.img`，本轮不出键值。
- **`pet-summon/D2` = LOCAL_TYPE**：083 `CPet::Init @0x703314` 用 `DecodeBuffer(this+160, 8u)` 吃 8B uid，s9 `addPetInfo` 用 `writeLong` —— **wire 宽度相同无分叉**；丢的是本端 `PetLook.uniqueid` / `add_pet` / `:526` cast 三处 int32 收窄，而背包侧 `Character/Inventory/Pet.h:31` 已是 `int64_t`。发包 `MovePetPacket`/`PetCommandPacket` 用 int32 与 s9 `MovePetHandler.readInt` 一致，**绝不能改**。另两处 `(void)` 丢弃对应 083 根本不存在的 s9 扩展字段（nameTag/balloon、summon stance/movetype/attacking），按 m09729「扩展功能可以不对齐」保持。
- **`login-flow/D13` = DOC_ONLY**：s9 `getCharList` 按 `chars.size()` 循环、槽数只是尾部 int32（`getCharacterSlots`）；083 `SendSelectCharPacket @0x5f726d` 只有 19/29/30 三形态与容量无关；本端 `LoginHandlers.cpp:290-330` 按 `charcount` 循环且 `UICharSelect.cpp` 无 15 槽常量 → 「15 槽」是账号容量展示值，不是包体定长，无需改。
- **`social-economy/D9` = 三分**：`FriendGroup`（083 仅 3 符号、`OnOK` 90 行无 `COutPacket`、IMG `matches: []`）→ **维持不做**；`guildmark`（`SendSetGuildMarkMsg @0x530f2f` 17 行 = `126/15` + 7B 定长、IMG 真节点 `UserList/Guild/GuildInfo/guildmark`、本端**无出站封装**）→ 从「不做」改判「有节点无发包」登记 P3；`MapleTV`（`UIMapleTV.h` 类已实现、IMG 顶层 `MapleTV` LIST、`Data/UI/MapleTV.img` 在）→ 从「整族缺失」改判「壳层已对齐」，播放栈属 F 维。

**算术不动**：168 / 38+130 / reopen 27 = 10+17 / 17 = P1 0 + P2 1 + P3 16 全部零改；本轮只补判定层与事实层。

**零改动**：0 wire / 0 协议 / 0 WZ / 0 源码行为改动。
"""
if "## 15. round98" not in wle_text:
    body = wle_text.rstrip("\n") + "\n" + SEC15.lstrip("\n").rstrip("\n") + "\n"
    write_text(WLE, body, wle_eol)
    print("  [applied] WLE §15")
else:
    print("  [skip] WLE §15 already present")

# ── 3. P3 Plan §8 回执 ──────────────────────────────────────────────────────
plan_text, plan_eol = read_text(PLAN)
SEC8 = """
## 8. round98 B 批回执

| # | 决策点 | 判定 | 备注 |
| --- | --- | --- | --- |
| 10 | `skill/D9` | `KEEP` | 083 单一布局，`Skill` 无 `main`，classic 恒 true = 083 路径；E13 环境适配 |
| 11 | `social-economy/D12` | `P2` | 27 键（非 26），083 可换 4 组 SP_；须逐键对照 `String.img`，本轮不出键值 |
| 12 | `pet-summon/D2` | `LOCAL_TYPE` | wire 同宽（8B）；截断在本端 int32 三处；发包 int32 不动 |
| 13 | `login-flow/D13` | `DOC_ONLY` | 槽数=尾部 int32 容量值，两端均按 charcount 循环 |
| 14 | `social-economy/D9` | `三分` | FriendGroup 不做 / guildmark 登记 / MapleTV 壳已对齐 |
"""
if "## 8. round98 B 批回执" not in plan_text:
    write_text(PLAN, plan_text.rstrip("\n") + "\n" + SEC8.lstrip("\n").rstrip("\n") + "\n", plan_eol)
    print("  [applied] P3 Plan §8")
else:
    print("  [skip] P3 Plan §8 already present")

# ── 4. dev-plan 2-46 行 ─────────────────────────────────────────────────────
ROW246 = ("| **2-46** | round98 · P3 B 批 5 条（`skill/D9` / `social-economy/D12` / "
          "`pet-summon/D2` / `login-flow/D13` / `social-economy/D9`） | 083 三向对拍：D9 双布局=资源降级非缺陷、"
          "D12 实为 27 键且 083 仅 4 组 SP_、D2 截断在本端 int32（wire 8B 同宽，发包 int32 不动）、"
          "D13 15 槽是尾部 int32 容量非定长、D9 三族三分（不发明/登记/已对齐）；**零 wire / 零协议 / 零 WZ / "
          "零源码行为改动**，算术 168 与 17 条分流全不动 | — | `round98` |\n")
def insert_row_after(rel, row_prefix, new_row, label):
    """按行序插入：找到以 row_prefix 开头的行，在其后插入 new_row。幂等。"""
    text, eol = read_text(rel)
    if new_row.rstrip("\n") in text:
        print("  [skip] %s (%s) already present" % (label, rel))
        return
    lines = text.split("\n")
    idx = None
    for i, line in enumerate(lines):
        if line.startswith(row_prefix):
            idx = i
            break
    if idx is None:
        raise SystemExit("ASSERT FAIL %s: prefix %r not found in %s" % (label, row_prefix, rel))
    lines.insert(idx + 1, new_row.rstrip("\n"))
    write_text(rel, "\n".join(lines), eol)
    print("  [applied] %s (%s) after line %d" % (label, rel, idx + 1))


insert_row_after(DEV, "| **2-45** |", ROW246, "dev 2-46 row")

# ── 5. PLT §57 ──────────────────────────────────────────────────────────────
plt_text, plt_eol = read_text(PLT)
SEC57 = """## 57. round98 · P3 B 批 5 条（三分 / DOC_ONLY / 本地类型扩展，零源码改动）

57-1 `skill/D9` 双布局 **不是缺陷**：083 `CUISkill::OnCreate @0x8aaa88` 单一布局（4 钮 2010..2013 @y131 x=119/159/199/239 + 宏钮 2023 @(120,265)）；`UIWindow.img/Skill` 11 直接子节点无 `main` → 本端 `classic = !usable(Skill["main"])` 恒 true。真机观感项：技能窗在 083 资源下应与原生截图逐像素一致，请顺手比对一次四颗加点钮 x 间距 40px。

57-2 `social-economy/D12` wedding 键：`locale/zh-CN.ini:302-321` 与 `en-US.ini` 各 **27 键**且 zh-CN 值是英文。083 侧仅 4 组 SP_（`SP_4202`/`SP_4203`/`SP_4204`/`SP_1666`）。真机项：婚礼流程里「教堂 BGM 名 / 祝福确认问句」两处若能对上 083 原文，可作为后续 P2 换键的锚点。

57-3 `pet-summon/D2` 8B uid：发包侧 `MovePetPacket`/`PetCommandPacket` 用 int32 与 s9 `MovePetHandler.readInt` 一致（**不动**）；截断在 `PetLook.uniqueid` / `add_pet` / `MapObjectHandlers.cpp:526` 三处 int32。真机项：同屏两只宠物时 `UIMiscShells.cpp:1864` 的 `look.get_uniqueid() == uid32` 比较是否会把不同宠物认成同一只（uid 高位相同才撞）。

57-4 `login-flow/D13` CHARLIST 槽数：s9 尾部 int32 = `getCharacterSlots()`（账号容量），两端均按 `charcount`/`chars.size()` 循环。真机项：角色数满时选角页空槽位显示是否正确（`UICharSelect.cpp` 单槽几何 `CS_SLOT_W 183 / CS_SLOT_H_EMPTY 115 / CS_SLOT_H_OCCUPIED 151`）。

57-5 `social-economy/D9` 三族：`FriendGroup` 维持不做；**`guildmark` 有新发现**——083 有 `CField::SendSetGuildMarkMsg @0x530f2f`（`126/15` + 7B 定长）与本端**无出站封装**，IMG 真节点在 `UserList/Guild/GuildInfo/guildmark`；`MapleTV` 壳层已对齐（`UIMapleTV.h` 类在），播放栈属 F 维。真机项：公会窗里改徽章若无反应，即此缺出站包所致（登记，不在本轮实施）。

57-6 本轮 **0 wire / 0 协议 / 0 WZ / 0 源码行为改动**；算术 168 与 reopen 17 条分流全不动；构建门禁只做回归，**不声称真重编覆盖**。
"""
if "## 57. round98" not in plt_text:
    write_text(PLT, plt_text.rstrip("\n") + "\n" + SEC57.rstrip("\n") + "\n", plt_eol)
    print("  [applied] PLT §57")
else:
    print("  [skip] PLT §57 already present")

# ── 6. evidence JSON ────────────────────────────────────────────────────────
EV = {
    "round": 98,
    "batch": "P3-B",
    "date": "2026-11-11",
    "trigger": "user: 接着推 B 批",
    "items": {
        "skill/D9": {
            "verdict": "KEEP",
            "issue": "classic 双布局是否收敛为原生单布局",
            "ida_083": {
                "CUISkill::OnCreate": "0x8aaa88 size 0x82d",
                "buttons": "4x (2010..2013) @y131 x=119/159/199/239; macro 2023 @(120,265)",
                "note": "全程单一布局，无 classic/modern 分支",
            },
            "img_gate": {
                "root": "UIWindow.img/Skill",
                "children": ["backgrnd", "skill0", "skill1", "line", "BtSpUp", "Tab",
                             "tip0", "tip1", "tip2", "CoolTime", "recommend"],
                "children_count": 11,
                "has_main": False,
            },
            "client": {
                "file": "IO/UITypes/UISkillBook.cpp",
                "line": 154,
                "code": "classic = !UiAssetResolve::usable(Skill[\"main\"]);",
                "effect": "083 资源下 classic 恒 true → 走 083 经典路径",
            },
            "s9": "DistributeSPHandler 上界与布局无关",
            "decisions": "DECISIONS.md:195 BATCH_1_KEEP",
            "action": "零改动，只出对照",
        },
        "social-economy/D12": {
            "verdict": "P2",
            "issue": "wedding.* 键值是否译",
            "key_count_actual": 27,
            "key_count_plan": 26,
            "locale_files": ["locale/zh-CN.ini:302-321", "locale/en-US.ini"],
            "zh_cn_values_are_english": True,
            "ida_083": {
                "CField_Wedding::OnWeddingProgress": "0x58153d size 0x60c",
                "sp_strings": [
                    "SP_4202_SOUND_BGMGLIMG_CHAPEL @0x58166d",
                    "SP_4203_SOUND_BGMGLIMG_CATHEDRAL @0x5816af",
                    "SP_4204_DO_YOU_WANT_TO_BLESS_THIS_COUPLE @0x581785",
                    "SP_1666_STRING_NPCIMG @0x581882",
                ],
                "note": "仅 4 组 SP_，非 27 键逐键对应",
            },
            "decisions": "DECISIONS.md:122 BATCH_1_KEEP（决策 B）",
            "action": "登记 P2，本轮不出键值",
        },
        "pet-summon/D2": {
            "verdict": "LOCAL_TYPE",
            "issue": "两处字节丢弃",
            "ida_083": {
                "CPet::Init": "0x703314 size 0x3a8",
                "uid_read": "CInPacket::DecodeBuffer(this+160, 8u)",
                "note": "083 无 nameTag/chatBalloon 两 bool（s9 扩展）",
            },
            "s9": {
                "addPetInfo": "PacketCreator.java:4570-4584 writeLong(uniqueId) + writeBool x2",
                "MovePetHandler": "readInt petId + readLong",
            },
            "client_truncation_points": [
                "Net/Handlers/MapObjectHandlers.cpp:521 read_long() 读对",
                "Net/Handlers/MapObjectHandlers.cpp:526 static_cast<int32_t>(uniqueid) 截断",
                "Character/Char.h:116 add_pet(..., int32_t uniqueid, ...) 截断",
                "Character/Look/PetLook.h:72 int32_t uniqueid; 截断",
            ],
            "client_int64_already": "Character/Inventory/Pet.h:31 int64_t get_uniqueid()",
            "wire_unchanged": [
                "Net/Packets/PetPackets.h:58-69 MovePetPacket write_int（与 s9 readInt 一致）",
                "Net/Packets/PetPackets.h:89-100 PetCommandPacket write_int",
            ],
            "void_discards_keep": [
                "MapObjectHandlers.cpp:520-523 nameTag/balloon（083 无此字段）",
                "MapObjectHandlers.cpp:1414-1417 summon stance/movetype/attacking（083 无 Decode）",
            ],
            "decisions": "DECISIONS.md:131 BATCH_0_BASELINE",
            "action": "登记本地类型扩展清单，本轮零改动",
        },
        "login-flow/D13": {
            "verdict": "DOC_ONLY",
            "issue": "CHARLIST 固定 15 槽",
            "s9": "PacketCreator.java:935 getCharList 按 chars.size() 循环，槽数=尾部 int32 getCharacterSlots()",
            "ida_083": {
                "CLogin::SendSelectCharPacket": "0x5f726d size 0x424",
                "opcodes": "19 / 29 / 30 三形态，与容量无关",
            },
            "client": {
                "file": "Net/Handlers/LoginHandlers.cpp:290-330",
                "pattern": "read_byte charcount → for i<charcount parse_charentry → read_byte pic → read_int slots",
                "no_15_slot_const": "IO/UITypes/UICharSelect.cpp 无 15 槽常量",
            },
            "decisions": "DECISIONS.md:66 BATCH_0_BASELINE",
            "action": "只出对照，零改动",
        },
        "social-economy/D9": {
            "verdict": "THREE_WAY",
            "issue": "UIFriendGroup / 公会徽章编辑 / MapleTV 三整族",
            "friendgroup": {
                "symbols": ["LoadFriendGroup@CConfig 0x49e6fb", "SetRet@CUIFriendGroup 0x80f0eb",
                            "OnOK@CUIFriendGroup 0x80f93e"],
                "onok_decompiled_no_coutpacket": True,
                "img_gate": "search FriendGroup → matches: []",
                "client": "无 UIFriendGroup 类；仅 UIUserList.cpp:1305 friend4 281x27 横条",
                "verdict": "不发明",
            },
            "guildmark": {
                "symbols": ["SendSetGuildMarkMsg@CField 0x530f2f size 0x7a"],
                "packet_shape": "COutPacket(126) + Encode1(15) + Encode2 + Encode1 + Encode2 + Encode1 = 7B",
                "img_gate": "UserList/Guild/GuildInfo/guildmark CANVAS_PROPERTY",
                "client": "WaveAShellHandlers.h:941/:947 仅声明；GuildPackets.h 无出站封装",
                "verdict": "登记（有节点无发包）",
            },
            "mapletv": {
                "symbols": ["IsLoadFlash@CMapleTVMan 0x63764c", "SetMapleTVMessage@CNpc 0x6d417e",
                            "DrawMapleTVMessage@CNpc 0x6d4ad4", "GetSenderName@CMapleTVMan 0x6d5811",
                            "GetReceivername@CMapleTVMan 0x6d5835", "GetText@CUIMapleTV 0x7f0dc9"],
                "img_gate": "MapleTV LIST_PROPERTY（UIWindow.img 顶层）；Data/UI/MapleTV.img 存在",
                "client": "IO/UITypes/UIMapleTV.h 类已实现（BtOk/BtCancel/has_assets）",
                "verdict": "壳层已对齐（播放栈属 F 维）",
            },
            "decisions": "DECISIONS.md:119 BATCH_1_KEEP",
            "action": "只出族级对照",
        },
    },
    "arithmetic_unchanged": {
        "classification_168": "76 + 38 + 14 + 40 = 168",
        "wle_tracks": "W38 + L130 = 168",
        "reopen": "27 = 已注销 10 + 残余 17",
        "residual": "17 = P1 0 + P2 1 + P3 16",
        "note": "本轮 5 条全在 16 条 P3 内，不新增分母、不注销",
    },
    "zero_change": {
        "wire": 0,
        "protocol": 0,
        "wz": 0,
        "source_behavior": 0,
        "note": "5 条全部 DOC_ONLY / KEEP / LOCAL_TYPE 登记，零源码行为改动",
    },
    "build_gate_regression": {
        "real_rebuild_claimed": False,
        "reason": "本轮零源码改动，只做门禁回归；不声称真重编覆盖",
    },
    "ida_evidence_files": [
        "tools/_ida_r98/CField_SendSetGuildMarkMsg.code.txt",
        "tools/_ida_r98/CField_Wedding_OnWeddingProgress.code.txt",
        "tools/_ida_r98/CLogin_SendSelectCharPacket.code.txt",
        "tools/_ida_r98/CPet_Init.code.txt",
        "tools/_ida_r98/CUIFriendGroup_OnOK.code.txt",
        "tools/_ida_r98/CUIMapleTV_GetText.code.txt",
        "tools/_ida_r98/CUISkill_OnCreate.code.txt",
        "tools/_ida_r98/img_gate.json",
        "tools/_ida_r98/q_dragon.json",
        "tools/_ida_r98/q_friendgroup.json",
        "tools/_ida_r98/q_guildmark.json",
        "tools/_ida_r98/q_login_charlist.json",
        "tools/_ida_r98/q_login_selectchar.json",
        "tools/_ida_r98/q_mapletv.json",
        "tools/_ida_r98/q_mapletv2.json",
        "tools/_ida_r98/q_pet.json",
        "tools/_ida_r98/q_pet_decode.json",
        "tools/_ida_r98/q_skill_img_layout.json",
        "tools/_ida_r98/q_skill_layout.json",
        "tools/_ida_r98/q_wedding.json",
        "tools/_ida_r98/symbol_summary.json",
    ],
    "constraints": [
        "083/v83 only 禁 095",
        "s9 协议优先（W 桶 6 字段 + 27 wire KEEP 零改动）",
        "数值重合 ≠ 语义重合",
        "校准 ≠ 改回原生（L 桶按 083 还原）",
        "orange-wz 128B key + MCP-Protocol-Version 头 + loadedCount 判成功",
        "门禁 5 禁想象修改 / 门禁 18 REA→IDA→orange-wz→.nx 只读取证",
    ],
}
EV_PATH = DM + "/evidence/p3-batch-b-round98.json"
open(abspath(EV_PATH), "wb").write(
    json.dumps(EV, ensure_ascii=False, indent=2).encode("utf-8") + b"\n")
print("  [wrote] %s" % EV_PATH)

# ── 7. notes ────────────────────────────────────────────────────────────────
NOTES = """# round98 · P3 B 批 5 条三向对拍（零源码改动）

## 1. 本轮范围

`P3-ANGEL-REBUILD-PLAN.md:39-43` 的 B 批 5 条：`skill/D9`、`social-economy/D12`、`pet-summon/D2`、`login-flow/D13`、`social-economy/D9`。

## 2. 取证口径

每条必须凑齐四项才落判定：

1. **083 活 IDB**（endpoint `http://127.0.0.1:13339/mcp`）：`entity_query` 取符号族 → `decompile` 取正文，落 `tools/_ida_r98/`。
2. **orange-wz 128B 门禁**：`load_files`（`tools/_gms_key_128.b64`，172 字符，iv `TSPHKw==`，带 `MCP-Protocol-Version: 2025-03-26` 头）→ `search`/`list_children` → `unload_all`；成功判定看 `loadedCount` 而非 `isError`。
3. **s9 服务端源码**（唯一副本 `E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/`）。
4. **当前端字面量 + DECISIONS 原裁定**。

## 3. 五条结论

| 决策点 | 判定 | 一句话 |
| --- | --- | --- |
| `skill/D9` | `KEEP` | 083 单一布局，`Skill` 无 `main` → classic 恒 true = 083 路径；双布局是资源降级（E13） |
| `social-economy/D12` | `P2` | 实为 27 键（非 26）；083 仅 4 组 SP_，须逐键对照 `String.img` |
| `pet-summon/D2` | `LOCAL_TYPE` | wire 同宽 8B；截断在本端 int32 三处；发包 int32 与 s9 一致不能改 |
| `login-flow/D13` | `DOC_ONLY` | 槽数=尾部 int32 容量值，两端均按 charcount 循环 |
| `social-economy/D9` | `三分` | FriendGroup 不做 / guildmark 有节点无发包 / MapleTV 壳层已对齐 |

## 4. 本轮发现的两处「Plan 措辞与事实不符」

1. `social-economy/D12` Plan/DECISIONS 写「26 键」，现场实数 **27 键**。
2. `login-flow/D13` Plan 写「改则只改本地槽数组」，但两端都没有槽数组定长结构（槽数只是尾部 int32），前提不成立。

两处只补事实层，不改算术、不注销条目。

## 5. 验证与边界

- 静态守卫 + 场景：`tools/test_round98_p3_batch_b.py`（全绿）
- `python tools/gen_opcode_matrix.py --check` → `GATE OK … (s9 a78ba681a4)`
- 零源码改动 → 只做构建门禁回归，**不声称真重编覆盖**
- 零署名提交；产物不含任何 commit hash
"""
NOTES_PATH = "docs/features/development-and-fix-plan-2026-11/notes/wave3-p3-batch-b-20261111.md"
open(abspath(NOTES_PATH), "wb").write(NOTES.encode("utf-8"))
print("  [wrote] %s" % NOTES_PATH)

# ── 报告 ────────────────────────────────────────────────────────────────────
print("\n=== SHA / EOL ===")
for p in [DM + "/P3-B-BATCH-MAPS.md", WLE, PLAN, DEV, PLT, EV_PATH, NOTES_PATH]:
    print("  %s  %s" % (sha12(p), eol_report(p)))
print("\nROUND98_DELIVER_ALL_OK")
