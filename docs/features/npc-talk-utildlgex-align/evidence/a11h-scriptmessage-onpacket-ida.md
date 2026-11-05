# A11h ScriptMessage OnPacket 全量对齐 + tip 820 — IDA 取证

> IDB：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64`  
> 日期：2026-11-05

## 1. 入口

| 符号 | 地址 | 说明 |
| --- | --- | --- |
| opcode 分发 | `sub_7465F4` | s2c **304** `NPC_TALK` |
| OnScriptMessage | `sub_74660A` | 公共头后 `switch(msgtype)` |

公共头（与 s9 `PacketCreator.getNPCTalk*` 一致）：

1. `byte speakerType`（常 4）
2. `int templateId`（npc）
3. `byte msgtype`
4. `byte speaker`

## 2. `switch (msgtype)` 尾字段（头之后）

| msgtype | 助手 | 尾字段 | UI 本轮 |
| --- | --- | --- | --- |
| 0 Say | `7467AB` | `(speaker&4)? int overrideNpc` + `string` + `byte prev` + `byte next` | 展示 |
| 1 YesNo | `74692C(...,a7=0)` | `string` | 展示 |
| 2 GetText | `746A8B` → `9AC7CB` | `string talk` + `string def` + **`short minLen`** + **`short editParam`** | 展示 |
| 3 GetNumber | `746DFF` → `9AC732` | `string` + `int def/min/max`；助手 **`a5=0,a6=10,a7=0`** | 展示 |
| 4 Simple | `746FAD` | `string` | 展示 |
| 5 Quiz | `747A22` → `A26B09` | `byte res`；`res==0` → 3×string + **3×int** | 吞包 `-1` |
| 6 SpeedQuiz | `747A34` → `A26C66` | `byte res`；`res==0` → **5×int** | 吞包 `-1` |
| 7 AskAvatar | `74713D` | `string` + `byte n` + `n×int` | 吞包 `-1` |
| 8 MembershopAvatar | `74730B` | 同 7 | 吞包 `-1` |
| 9 AskPet | `7474A2` | `string` + `byte n` + `n×byte`（本机 pet 反查） | 吞包 `-1` |
| 10 AskPetAll | `74775C` | `string` + `byte n` + `byte flag` + `n×byte` | 吞包 `-1` |
| 12 AcceptDecline | `74692C(...,a7=1)` | `string` | 展示 |
| 13 AskBoxText | `746C46` | 同 GetText（dlgType=8） | 吞包 `-1` |
| 14 Mirror | `747A46` → ctor `76B3F0`/`76B5C8` | `int` + `string` | 吞包 `-1` |

### 2.1 Say `speaker&4`

`7467AB`：`(a5 & 4) != 0` 时用 `sub_406629` 再读一个 int 覆盖 templateId，再 `sub_46F30C` 读正文，再两字节 prev/next。

### 2.2 GetText shorts

`746A8B`：两串后 `sub_42470C`×2（signed short）。  
`9AC7CB(Src=def, a3=minLen→this+0xDC, a4=editParam→this+0xCC, a5=0, a6=0)`。  
s9 `getNPCTalkText` 尾 `writeInt(0)` ≡ 两 short `0,0`。

### 2.3 GetNumber + tip 820

`746DFF` 只读 `def/min/max` 三 int（客户端无读 pad）；s9 多写 `writeInt(0)`，本仓 drain。  
`9AC732(..., a5=0, a6=10, a7=0)` ⇒ **`number_min_digits=0`**、`edit_maxlen=10`。

`sub_9AB50A` 数字路径：min/max 通过后，若 `len(this+0xE8) < this+0xDC` → StringPool **820**  
`Please enter more than %d digits.`  
线路径 `0xDC=0` 时正常 AskNumber **不会弹 820**；本仓仍接线，OnPacket 置 `number_min_digits=0`。

## 3. 与 s9 PacketCreator 交叉（本轮不改服务端）

| API | msgtype 字节 | 备注 |
| --- | --- | --- |
| `getNPCTalkText` | 2 | 尾 `int(0)` ≡ short×2 零 |
| `getNPCTalkNum` | 3 | 尾多 `int(0)` pad |
| `getNPCTalkStyle` | 7 | Avatar；本轮吞包 `-1` |
| `getDimensionalMirror` | `0x0E` | `int(0)+string` |
| `OnAskQuiz` | **`0x6`** | 与 IDA case **5** 不一致；字段写 short×2+int，IDA 读 3×int |
| `OnAskSpeedQuiz` | **`0x7`** | 与 IDA case **6** 不一致，且与 Avatar=7 冲突 |

客户端按 **IDA** 解析；未接 UI 的类型读完后 `NpcTalkMore(msgtype,-1)`，避免脚本挂死。

## 4. 本仓接线结论

- Handler：禁止先盲读 string；按 msgtype 分支。
- tip 820：`TalkInputParams::number_min_digits` + `dispatch_input_field`；样式沿用 A11g。
- 不新建 Quiz/Avatar/Pet/Mirror UI；不改服务端；不 push。
