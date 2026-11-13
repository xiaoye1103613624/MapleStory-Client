# r04 · CHARLIST 逐字段一手字节对照：Angel 083 `GW_CharacterStat::Decode` vs 当前端 `LoginParser::parse_stats`

> 性质：主 agent 一手取证。全部「原生」结论来自 Angel(13339) 083/v83 带符号 IDB 的逐字
> Hex-Rays 伪代码，证据文件：
> - `evidence/angel-login-chardecode.json` — `GW_CharacterStat::Decode @ 0x4e2a84`、`AvatarLook::Decode @ 0x4e749a`
> - `evidence/angel-login-chardecode2.json` — `sub_4E2745 @ 0x4e2745`（骑士团分支）、`sub_4E807A @ 0x4e807a`、`sub_4E80EB @ 0x4e80eb`（ZtlSecure 混淆）
> - `evidence/angel-login-decompile.json` — `CLogin::OnSelectWorldResult @ 0x5f9891`（CHARLIST 外层）、`OnViewAllCharResult` / `OnCreateNewCharacterResult` / `OnDeleteCharacterResult` / `OnCheckDuplicatedIDResult`
>
> 「当前端」列逐字来自 `Net/Handlers/Helpers/LoginParser.cpp:94-209`、`Net/InPacket.h:44-52`。
> 参照基准固定 083/v83；当前对接服务端为 s9（BeiDou-Server_s9）。凡编号/宽度相异处一律标注
> 「s9 适配」，**不自动判当前端为错，不盲改**（改回原生会断联机），须交用户抉择。
> 标记：[观察] = 逐字伪代码/源码可直接读出；[推断] = 由观察推出；[未知] = 需实机抓包或 IMG 证据。

---

## 0. 结论（先看这一节）

| # | 判定 | 状态 |
|---|---|---|
| 1 | CHARLIST 前段字段（cid / name / gender / skin / face / hair / petids / 10×stat / fame / gacha / mapid / portal / tail）宽度与顺序 **逐字节一致** | ✅ 一致 [观察] |
| 2 | `level`：083 原生 **1 字节**；当前端 **2 字节** `read_short()` | ❌ 不一致，已知 s9 999 级改造（代码自证），校准须保留 [观察] |
| 3 | `exp`：083 原生 wire **4 字节**（`Decode4` → 内存存 8 字节）；当前端 wire 直接 **8 字节** `read_long()` | ❌ 不一致，当前端每段多读 4 字节 [观察] |
| 4 | 当前端 `parse_stats` 从 exp 起读 **23 字节**，083 原生 **19 字节**；`LoginParser.cpp:155` 注释声称「两者都是 19 字节」与代码不符 | ⚠️ 注释/代码矛盾，须实机抓包裁决 [观察] |
| 5 | 083 原生有 **骑士团职业条件分支**（`v17/100==22 || v17==2001` → `sub_4E2745` 追加 `1+2*count` 字节），当前端 `parse_stats` **完全无此分支** | ❌ 真缺失（非 s9 适配可解释）：s9 若发骑士团角色会整条错位 [观察] |
| 6 | `AvatarLook::Decode` 与当前端 `parse_look` 结构对齐（双 0xFF 循环 / `maskedequips[-111]` / 3×petids） | ✅ 一致 [观察] |
| 7 | CHARLIST 外层：083 首字节=**结果码** + **固定 15 槽**（684 B/槽）；当前端首字节=**channel_id** + 按 charcount 循环 + pic + slots | ❌ 结构差异（s9 适配） [观察] |

---

## 1. `GW_CharacterStat::Decode @ 0x4e2a84`（`a3 == 0`，即 CHARLIST 路径）逐字 wire 序列

`CLogin::OnSelectWorldResult @ 0x5f9891` 的 LABEL_33 调用形式为
`GW_CharacterStat::Decode(this: v13, a2, a3: 0)`，故 `a3==0` 分支生效（`a3!=0` 分支是
「丢弃 wire 值」的 skip 变体，用于 VAC/其它路径，本次不取）。

| 序 | 字段（语义） | 原生逐字调用 | wire 字节 | 存储偏移 |
|---|---|---|---|---|
| 1 | cid | `*this = CInPacket::Decode4(a2)` | 4 | `this+0` |
| 2 | name | `CInPacket::DecodeBuffer(a1: this+4, a2: 0xD)` | 13 | `this+4` |
| 3 | gender | `*(this+17) = CInPacket::Decode1(a2)` | 1 | `this+17` |
| 4 | skin | `*(this+18) = CInPacket::Decode1(a2)` | 1 | `this+18` |
| 5 | face | `*(this+19) = CInPacket::Decode4(a2)` | 4 | `this+19` |
| 6 | hair | `*(this+23) = CInPacket::Decode4(a2)` | 4 | `this+23` |
| 7 | petids[3] | `CInPacket::DecodeBuffer(a1: this+27, a2: 0x18u)` | 24 | `this+27` |
| 8 | **level** | `LOBYTE(v5) = Decode1; *(this+53) = sub_4E807A(a1: v5, a2: this+51)` | **1** | `this+53` |
| 9–18 | JOB/STR/DEX/INT/LUK/HP/MAXHP/MP/MAXMP/AP | `LOWORD(vN)=Decode2` ×10 → `sub_4E80EB(vN, this+57/65/…/129)` | 20 | `this+61…133` |
| 19 | 职业分支（见 §2） | `v17 = _ZtlSecureFuse<short>(this+57, *(this+61))`；`if (v17/100==22 || v17==2001)` → `sub_4E2745(a2)`；**else** `LOWORD(v18)=Decode2` → `*(this+141)=sub_4E80EB(v18, this+137)` (SP) | 2（非骑士团）/ 动态（骑士团） | `this+141` |
| 20 | **exp** | `v19 = Decode4; *(this+153) = _ZtlSecureTear<long>(v19, this+145)` | **4**（内存存 8） | `this+153` |
| 21 | fame | `LOWORD(v20)=Decode2; *(this+161)=sub_4E80EB(v20, this+157)` | 2 | `this+161` |
| 22 | gacha | `v21 = Decode4; *(this+185) = _ZtlSecureTear<long>(v21, this+177)` | 4 | `this+185` |
| 23 | mapid | `v22 = Decode4; *(this+217) = sub_4E81A0(v22, this+209)` | 4 | `this+217` |
| 24 | portal | `*(this+221) = Decode1` | 1 | `this+221` |
| 25 | tail | `result = Decode4; *(this+227) = result` | 4 | `this+227` |

**原生总长（非骑士团、含 cid）= 93 字节** [观察]。
`sub_4E807A(char a1, unsigned __int8* a2)` 与 `sub_4E80EB(__int16 a1, int a2)`
的参数类型分别是 `char` / `__int16`，**确证 level 的 wire 宽度是 1 字节、10 个 stat 是 2 字节**；
二者函数体是 `CRand32::Random` + 异或 + `_rotr(..., 5)` 的 **ZtlSecure 内存防篡改混淆**，
只写本地存储、不额外读 wire，故不影响上表字节宽度 [观察]。

---

## 2. 骑士团（Cygnus）条件分支 —— 当前端完全缺失

`GW_CharacterStat::Decode` 逐字 [观察]：

```c
v17 = _ZtlSecureFuse<short>(a1: this + 57, a2: v24);   /* v24 = *(this+61) = JOB */
if ( v17 / 100 == 22 || v17 == 2001 )                  /* 骑士团 / 2001 */
{
  LOWORD(v17) = 0;
  *(this + 141) = sub_4E80EB(a1: v17, a2: this + 137); /* SP 写 0，不读 wire */
  sub_4E2745(a1: a2);                                   /* ★ 追加字段 */
}
else
{
  LOWORD(v18) = CInPacket::Decode2(this: a2);           /* 正常 SP：读 2 字节 */
  *(this + 141) = sub_4E80EB(a1: v18, a2: this + 137);
  sub_4746A3(a1: this + 189);
}
```

`sub_4E2745 @ 0x4e2745` 逐字 [观察]：

```c
sub_4746A3(a1: this);
result = CInPacket::Decode1(this: a2);      /* count，1 字节 */
if ( result != 0 )
{
  v12 = result;
  do {
    v3 = CInPacket::Decode1(this: a2);      /* 2 字节 / 条 */
    v4 = CInPacket::Decode1(this: a2);
    LOBYTE(v5) = v3; v13 = v4;
    v6  = sub_4E807A(a1: v5, a2: &v9);     /* 本地混淆，不读 wire */
    v10 = sub_4E807A(a1: v7, a2: &v9 + 6);
    result = sub_4E80AD(a1: v11);          /* ZArray push，不读 wire */
    *result = v9; *(result + 8) = v10;     /* 写 12 字节 / 条 */
  } while ( !v8 );                         /* v8 = v12-- == 1 */
}
```

→ 骑士团分支的追加 wire 长度 = **1 + 2×count 字节**，同时省掉正常 SP 的 2 字节，
故骑士团条目总长 = `93 - 2 + (1 + 2*count)` = `92 + 2*count` 字节 [观察]。

**当前端 `LoginParser::parse_stats`（`LoginParser.cpp:139-149`）无条件连续读 11 个
`read_short()` 后直接进 `exp = read_long()`，没有任何「按 JOB 分支」逻辑** [观察]：
若 s9 下发骑士团角色（v83 原生确实存在该分支），当前端会把追加字段误读为 exp/fame/…，
导致整条 CHARLIST 及后续角色全部错位 [推断]。

> 这一项不属于「s9 编号适配」：它是**原生存在的条件解码语义在当前端被整段省略**。
> 必须先确认 s9 服务端 `addCharStats` 是否对骑士团写该追加段，再决定补分支（决策点 D3）。

---

## 3. 当前端 `LoginParser::parse_stats` 逐字（`LoginParser.cpp:121-173`）

`Net/InPacket.h:44-52` 宽度口径：`read_bool`=1 / `read_byte`=1 / `read_short`=2 /
`read_int`=4 / `read_long`=8 [观察]。

| 序 | 字段 | 当前端逐字 | wire 字节 |
|---|---|---|---|
| 1 | cid | `parse_charentry:96 int32_t cid = recv.read_int();`（在 `parse_stats` 外） | 4 |
| 2 | name | `:126 read_padded_string(13)` | 13 |
| 3 | female | `:127 read_bool()` | 1 |
| 4 | skin | `:129 read_byte()`（丢弃） | 1 |
| 5 | face | `:130 read_int()`（丢弃） | 4 |
| 6 | hair | `:131 read_int()`（丢弃） | 4 |
| 7 | petids[3] | `:133-134 read_long()` ×3 | 24 |
| 8 | **level** | `:138 static_cast<uint16_t>(recv.read_short())` | **2** |
| 9–19 | JOB…SP（11 项） | `:139-149 read_short()` ×11 | 22 |
| 20 | **exp** | `:160 statsentry.exp = recv.read_long()` | **8** |
| 21 | fame | `:161 read_short()` | 2 |
| 22 | gacha | `:163 recv.skip(4)` | 4 |
| 23 | mapid | `:165 read_int()` | 4 |
| 24 | portal | `:166 read_byte()` | 1 |
| 25 | tail | `:171 recv.skip(4)` | 4 |

**当前端总长（含 `parse_charentry` 的 cid）= 98 字节**；其中 `parse_stats` 本体 94 字节 [观察]。

---

## 4. 逐字段对照与差值归因

| 字段 | 083 原生 | 当前端 s9 | 差 | 归因 |
|---|---|---|---|---|
| cid / name / gender·female / skin / face / hair / petids | 4/13/1/1/4/4/24 | 4/13/1/1/4/4/24 | 0 | ✅ 一致 [观察] |
| **level** | **1** | **2** | **+1** | `LoginParser.cpp:136-138` 逐字注释「北斗 gms083 服务端（已为 999 级改造）把 level 写成 2 字节 short；旧代码按 1 字节 read_byte() 读取，等级 >=256 会被截断」→ **已知 s9 适配，校准须保留** [观察] |
| JOB…AP（10 short）+ SP（第 11 short） | 22 | 22 | 0 | ✅ 一致 [观察]（骑士团分支见 §2） |
| **exp** | **4**（`Decode4` → `_ZtlSecureTear<long>` 存 8） | **8**（`read_long()`） | **+4** | 083 原生 wire 只发 4 字节 exp；当前端把「exp8」当 wire。当前端 `:150-159` 注释自述 s9 是 `writeInt(exp)=4 + writeInt(0)=4`，并警告「旧代码按 8 字节 exp 解析 s9 的包时，skip(4) 恰好吞掉真实 mapId(00 e1 f5 05)」→ **注释与代码并存、相差 4 字节** [观察] |
| fame / gacha / mapid / portal / tail | 2/4/4/1/4 | 2/4/4/1/4 | 0 | ✅ 一致 [观察] |
| **合计（含 cid）** | **93** | **98** | **+5** | = level +1 + exp +4 |
| **从 exp 起** | **19**（4+2+4+4+1+4） | **23**（8+2+4+4+1+4） | **+4** | `LoginParser.cpp:155` 注释「两者从 exp 到 charLook 都是 19 字节」**与代码不符**，须实机抓包裁决 [观察] |

> ⚠️ **D2（最高优先级决策点）**：当前端 `read_long()` 8 字节 exp 与注释自述的 s9
> 「exp4 + tail4」到底谁对，决定 `mapid` 是否会整体平移 4 字节（注释 :156-158 自述的
> 「进图白屏」根因）。**不能靠注释裁断，必须抓一帧真机 CHARLIST 字节** 或读
> `BeiDou-Server_s9` 的 `PacketCreator.addCharStats` 源码。083 原生给的是 wire 4 字节。

---

## 5. `AvatarLook::Decode @ 0x4e749a` vs 当前端 `parse_look`（✅ 一致）

| 原生逐字 | 当前端逐字 | 判定 |
|---|---|---|
| `*(this+12)=Decode1` gender / `*(this+13)=Decode1` skin / `*(this+17)=Decode4` face | `:179 read_bool()` / `:180 read_byte()` / `:181 read_int()` | ✅ |
| `memset(this+25,0,0xD0); memset(this+233,0,0xD0)`（两张装备表清零） | 当前端 `LookEntry` 构造即空 map | ✅ 语义等价 |
| `Decode1`（megaphone，丢弃） | `:183 read_bool()` | ✅ |
| `*(this+25)=Decode4`（hair 写入 slot0 区） | `:185 read_int()` hairid | ✅ |
| **loop1**：`v3=Decode1; if(v3==0xFF) break; v5=Decode4; if(v3<=0x33 && v3!=0 && is_correct_bodypart(v5,v3,2)) *(this+4*v3+25)=v5` | `:187-193` `eqslot=read_byte()` 循环到 0xFF，`equips[eqslot]=read_int()` | ✅（当前端未做 `<=0x33`/`is_correct_bodypart` 校验，属简化 [推断]） |
| **loop2**：同构写 `this+4*v8+233`（masked equips） | `:195-201` `mskeqslot` 循环到 0xFF | ✅ |
| `*(this+21)=Decode4` | `:203 look.maskedequips[-111] = read_int()` | ✅ |
| `DecodeBuffer(this+441, 0xC)`（3×int32 petids） | `:205-206 read_int()` ×3 | ✅ |

---

## 6. CHARLIST 外层结构：`CLogin::OnSelectWorldResult @ 0x5f9891` vs `CharlistHandler`

### 6.1 原生（LABEL_33 逐字，`evidence/angel-login-decompile.json`）

```c
v3 = Decode1(a2);                          /* ★ 首字节 = 结果码 */
... 错误码 switch：0xFF/6/8/9→Error(15)、2/3→Error(16)、4→Error(3)、5→Error(20)、
   7→GotoTitle+Error(17)、10→Error(19)、11→Error(14)、13→Error(21)、
   14/15→YesNo2(27/26)+开官网、16→Error(33)、17→Error(27)、21→break、25→Error(40) ...
LABEL_33:
if ( v4 == 0 || v4 == 12 || v4 == 23 )     /* 只有这 3 个码进解析 */
{
  v12 = CInPacket::Decode1(this: a2);      /* charcount */
  v25 = 0; v24 = 0; v23 = 0;
  do {
    if ( v25 >= v12 )  *( *(this+101) + v24 ) = 0;   /* 终止槽 */
    else {
      GW_CharacterStat::Decode( *(this+101)+v24, a2, 0 );
      AvatarLook::Decode( *(this+101)+v24+231, a2 );
      *( *(this+103)+4*v25 ) = Decode1(a2);           /* viewAll 标志 */
      if ( Decode1(a2) != 0 ) DecodeBuffer( *(this+102)+v23, 0x10 );  /* rankinfo 16B */
      else                    memset( *(this+102)+v23, 0, 0x10 );
      if ( *( *(this+101)+v24 ) == v22 ) *(this+100) = v25;  /* 选中槽 */
    }
    ++v25; v23 += 16; v24 += 684;
  } while ( v25 < 15 );                     /* ★ 固定 15 槽 / 684 B 每槽 */
  *(this+372) = Decode1(a2);                /* bIsSPW */
  v15 = Decode4(a2); *(this+95) = v15;      /* char slots 4B */
  CUICharSelect::CUICharSelect(...);  sub_603FF0(...);   /* CUIAvatar ctor */
  CLogin::ChangeStep(this, a2: 0xFFFFFFFF);
  if ( CWvsApp+36 == 1 && *(this+372) == 0 )
    COutPacket(9) + Encode1(1) + Encode1(1) + EncodeStr("") + SendPacket;
}
```

### 6.2 当前端（`Net/Handlers/LoginHandlers.cpp:288-334`，逐字）

`CharlistHandler`：`read_byte()` **channel_id** → `read_byte()` charcount → 按 charcount 循环
`LoginParser::parse_charentry` → `read_byte()` pic → `read_int()` slots → skip；
oncomplete `remove(LOGINWAIT)` + `worldselect->remove_selected()` +
`emplace<UICharSelect>(characters, charcount, slots, pic)`。

### 6.3 差异表

| 维度 | 083 原生 | 当前端 s9 | 判定 |
|---|---|---|---|
| 首字节 | 结果码（仅 0/12/23 进解析） | channel_id | ❌ 语义不同，s9 适配 [观察] |
| 槽位循环 | `while(v25<15)` **固定 15 槽**，每槽 684 B | 按 charcount 循环，逐条 `parse_charentry` | ❌ 结构差异，s9 适配 [观察] |
| rankinfo | `Decode1` 开关 + `DecodeBuffer(0x10)` 16 B / 槽 | `parse_charentry:103 read_bool() has_rank` + 4×`read_int()`（16 B） | ✅ 宽度一致（16 B），开关字节位置略异 [观察] |
| bIsSPW / pic | `Decode1` bIsSPW | `read_byte()` pic | ⚠️ 同为 1 字节、语义需核 [推断] |
| char slots | `Decode4` | `read_int()` | ✅ |
| 后续动作 | `CUICharSelect` + `CUIAvatar` ctor + `ChangeStep(-1)` | `emplace<UICharSelect>` + remove LOGINWAIT | ✅ 语义一致 |

---

## 7. 其它收包处理器的一手字节（供后续逐屏校准复用）

| 原生函数 | EA | wire 逐字 | 当前端 |
|---|---|---|---|
| `CLogin::OnViewAllCharResult` | `0x5facca` | case0：`Decode1`(worldid)+`Decode1`(charcount)+循环 `GW_CharacterStat::Decode`+`AvatarLook::Decode`+`Decode1`+16 B；case1：`Decode4`+`Decode4`；case2：`RemoveNoticeConnecting`+`ResetVAC`+`SP_3989` | `LoginHandlers.cpp:530-575 ViewAllCharHandler` header 0=详情/其余=概览，累积 `UIStateLogin::viewall_worlds` |
| `CLogin::OnCreateNewCharacterResult` | `0x5fa26c` | `Decode1` 结果码（10→Error(19)、26→Notice `SP_4016`、30→Error(10)）；成功 `GW_CharacterStat::Decode`+`AvatarLook::Decode`、找空槽（`<15`，步长 171 DWORD）、`CUIAvatar::Refresh/SelectCharacter`、`ChangeStep(2)` | `AddNewCharEntryHandler:379-402` 首字节 `read_byte()` result，非 0 丢弃，否则 `parse_charentry` |
| `CLogin::OnDeleteCharacterResult` | `0x5f9d15` | `Decode4`(cid)+`Decode1`(state)；失败码 6/9/10/18/20/22/24/26/29 → `Error(15/18/19/11/93/34/39)`/Notice `SP_4011`/`Error(54)`；成功按 cid 移位 684 B 槽 | `DeleteCharResponseHandler:404-452` `read_int()` cid + `read_byte()` state，s9 码 `0x14/0x16/0x1A/0x1D/0x09/0` |
| `CLogin::OnCheckDuplicatedIDResult` | `0x5f9c72` | `DecodeStr(name)`+`Decode1(result)`；0=可用（置 `this+142/143=1`）、1→Error(5)、2→Error(10)、其它→Error(18) | `CharnameResponseHandler:364-377` `read_string()` name + `read_bool()` used → `UIRaceSelect::send_naming_result` |

---

## 8. 限制与未决

- [未知] `EncodeBuffer(MachineId)` 在 `SendCheckPasswordPacket` 中的实际长度未取（属 §r03 登录发包对照，非本文件范围）。
- [未知] s9 服务端 `PacketCreator.addCharStats` 是否对骑士团写 `sub_4E2745` 追加段 → 决定 §2 是否需补分支（决策点 D3）。
- [未知] `LoginParser.cpp:155` 注释「19 字节」与代码「23 字节」谁对 → 需真机 CHARLIST 抓帧（决策点 D2）。
- [未知] IMG 层：orange-wz MCP 未接入本会话，资源节点以仓库内 `.nx` 字面量 + 正确端截图为准（见主 README §1.4）。
- 本文件所有「当前端」结论均来自源码静态阅读，**未运行、未构建、未实机**；按技能门禁，修改实施须在决策点裁决 + 一次性 Plan 之后进行。
