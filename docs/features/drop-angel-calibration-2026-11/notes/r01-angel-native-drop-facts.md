# 第十二面 · Angel 083/v83 原生掉落(Drop)一手事实基线 (r01)

> 取证源：`Angel.exe (pid=29620, port=13339)` 带符号 IDB（`Angel.i64`, v83 GMS, imagebase 0x400000）。
> 对照源：`BeiDou.exe (port=13337)` stripped IDB。采集时间 2026-11-10。
> 性质：纯只读取证 + 伪代码逐字摘录，未改游戏源码/协议/WZ/IMG/XML，未建提交。

## 0. 七条硬结论

1. **十二面 EA 字节级同二进制闭环**：Drop 面 `18/18 EAs identical`（`angel-beidou-drop-ea-crosscheck.json`），`beidou_sub_named=18 / real_named=0 / missing=0` → Angel 与 BeiDou 是同一份 v83 GMS 二进制，Angel 只多符号层。
2. **Angel 只补语义层**：BeiDou 侧 18 个 EA 全部退化为 `sub_XXXXXX`，Angel 侧 18 funcs / 9 classes 全具名（`CDropPool` 7、`CWvsContext` 3、`CWndMan` 2、`CDraggableItem/Menu/Skill` 各 1、`CUIItem` 1、`CUserLocal` 1、`CWnd` 1）。
3. **本面唯一 c2s 出站 = `94` (0x5E, MESO_DROP)**：`CWvsContext::SendDropMoneyRequest @0xa23de5` 内 `COutPacket(94)` + `Encode4(update_time)` + `Encode4(amount)`，前置 `CanSendExclRequest(200)` 节流 + `(*(get_field()+324) & 0x400000) != 0` 禁丢位（`SP_4558_YOU_CANNOT_DROP_THE_ITEM_HERE`）。
4. **`269` 的 `animation=4` 与 s9 分叉**：`CDropPool::OnDropLeaveField @0x506590` 对 `v2==4` 读 `Decode2` 并写 `*(v4+68)=update_time+delay`、`*(v79+72)=4`（原端爆炸延时记账）；s9 `PacketCreator.removeItemFromMap` 在 `animation>=2` 恒写 4 字节 `chrId`。数值同 4、语义不同 → **不得按 083 改 short**。
5. **金币四档阈值在未具名函数里**：`sub_506BFE @0x506bfe` = `a1<50 → 0`；`a1>=100 → (a1>=1000)+2`；否则 `1`（即 `<50 / 50..99 / 100..999 / >=1000`），由 `CDropPool::GetMoneyIcon @0x506c26` 的 `sub_506BFE(a1: a3)` 调用点反查得到；`facts` 自动抽取抽不到数值分支。
6. **083 收包 `OnDropEnterField` 与 s9 同序**：`Decode1 mode` → `Decode4 oid` → `Decode1 meso` → `Decode4 itemid` → `Decode4 owner` → `Decode1 pickuptype` → `Decode2×2 dropto` → `Decode4 dropper` → `Decode2×2 dropfrom`(mode≠2) → `Decode2 Fh` → `DecodeBuffer`(meso==0 时 8B) → `Decode1 playerdrop` 反相。
7. **校准 ≠ 改回原生**：s9 协议优先；095（`Maplestory95.i64` / `port-095/**`）全程禁引；D1–D14 交用户裁决后才动源码。

## 1. 三联门禁

| 环 | 端点 | 结果 |
| --- | --- | --- |
| REA | 既有 `evidence/r17/rea_search_*` | ⚠️ subject=`BeiDou-ClientV17.7/BeiDou.exe`，**非 13339**，仅作 stripped 端字符串锚点，单列不冒充一手 |
| IDA Angel | 13339 | ✅ 18 funcs / 9 classes，反编译 `18/18 ok (physical_keys=18)` |
| IDA BeiDou | 13337 | ✅ crosscheck `18/18 EAs identical`，`missing=0` |
| IMG (orange-wz MCP) | 10012 | ✅ 结构层 WORKS / ⚠️ 内容层 `IMG 解析失败` → 合规 `.nx` 回退（见 r03） |

采集脚本（均由第十一面已验证 SOP 派生 + 三形态归一 + `label@EA` 防重名覆盖）：

| 脚本 | 产出 |
| --- | --- |
| `tools/ida_angel_drop_ea.py` | `evidence/angel-drop-symbol-inventory.json` |
| `tools/ida_angel_drop_crosscheck.py` | `evidence/angel-beidou-drop-ea-crosscheck.json` |
| `tools/ida_angel_drop_byteaudit.py` | 0 missing → `byte audit not required` |
| `tools/ida_angel_drop_decompile.py` | `evidence/angel-drop-decompile.json` |
| `tools/ida_angel_drop_facts.py` | `evidence/angel-drop-facts.md` |

## 2. 18 函数地址表

| 函数 | EA | 大小 | 归属 |
| --- | --- | --- | --- |
| `CDraggableItem::OnDropped` | `0x4ef140` | 0xbe5 | 拖拽族 |
| `CDraggableMenu::OnDropped` | `0x4f91e4` | 0x19f | 拖拽族 |
| `CDraggableSkill::OnDropped` | `0x4faa22` | 0x5df | 拖拽族 |
| `CDropPool::TryPickUpDrop` | `0x50463c` | 0x1fe | 主链 |
| `CDropPool::OnPacket` | `0x5058db` | 0x25 | 主链（268/269 分派） |
| `CDropPool::OnDropEnterField` | `0x505900` | 0xb15 | 主链 |
| `CDropPool::OnDropLeaveField` | `0x506590` | 0x610 | 主链 |
| `CDropPool::GetMoneyIcon` | `0x506c26` | 0x23c | 主链（四档阈值） |
| `CDropPool::MakeMoneyAnimation` | `0x506e62` | 0x1313 | 主链（金币动画） |
| `CDropPool::BoobyTrapCheckPickupItem` | `0x5081b0` | 0xb8 | booby trap 族 |
| `CUIItem::OnDropMoney` | `0x81d996` | 0x18c | 主链（丢金币询问框） |
| `CUserLocal::CheckBoobyTrapPickUpRequest` | `0x95d7b7` | 0xd4 | booby trap 族 |
| `CWnd::OnDragDrop` | `0x4243fe` | 0x67 | 拖拽族 |
| `CWndMan::BeginDragDrop` | `0x9e353d` | 0x1104 | 拖拽族 |
| `CWndMan::EndDragDrop` | `0x9e37c2` | 0x16f9 | 拖拽族 |
| `CWvsContext::OnDropPickUpMessage` | `0xa20ad9` | 0x2189 | 主链（提示消息） |
| `CWvsContext::SendDropMoneyRequest` | `0xa23de5` | 0xd0 | 主链（唯一 c2s） |
| `CWvsContext::CheckBoobyTrapPickUpRequest` | `0xa32914` | 0xbc | booby trap 族 |

> 9 个拖拽族函数来自 `kws=(Drop/PickUp/Loot)` 宽口径命中，属子串假阳性，不进掉落主链结论。

## 3. 三段关键伪代码逐字

### 3.1 `CDropPool::OnPacket @0x5058db`（分派）

```c
int __stdcall CDropPool::OnPacket(int a1, struct CInPacket *a2)
{
  if ( a1 == 268 )
    return CDropPool::OnDropEnterField(a1: a2);
  return CDropPool::OnDropLeaveField(a1: a2);
}
```

### 3.2 `CDropPool::OnDropLeaveField @0x506590`（`269` 头部读序）

```c
v2  = CInPacket::Decode1(this: a2);            // animation / mode
v71 = CInPacket::Decode4(this: a2);            // oid
if ( sub_508AE3(a1: &v71, a2: v78) == 0 ) goto LABEL_7;   // 池内查不到 → 早退
if ( v2 == 2 || v2 == 3 || v2 == 5 )           // 拾取/宠物拾取 → 4B cid
    *(v7 + 88) = CInPacket::Decode4(this: a2);
else if ( v2 == 4 )                            // 爆炸 → 2B delay（083 专属记账）
{
    v5 = CInPacket::Decode2(this: a2);
    *(v4  + 68) = get_update_time() + v5;
    *(v79 + 72) = 4;
}
// 之后：v2==0 走 IWzGr2DLayer 消亡动画；v2==5 再 Decode1(pet slot)
```

### 3.3 `sub_506BFE @0x506bfe`（金币四档阈值）

```c
int __stdcall sub_506BFE(int a1)
{
  if ( a1 < 50 )                       // 0 = BRONZE
    return 0;
  if ( a1 >= 100 )
    return (a1 >= 1000) + 2;           // 100..999 → 2 = BUNDLE；>=1000 → 3 = BAG
  return 1;                            // 50..99 → 1 = GOLD
}
```

## 4. 三条证据优先级

`angel-drop-decompile.json` 内层伪代码 > 当前端源码 > s9 `PacketCreator` 权威写法 > 既有 `beidou-v17-parity` 笔记。

- `angel-drop-decompile.json` 的 `functions` 值是**双编码 JSON 字符串**（`json.loads(v)['code']`），直接 `v['code']` 会拿到整段 JSON 一行。
- 未具名 `sub_506BFE` 必须先按调用点反查地址再单独 `decompile`，否则阈值分支整个漏掉。
