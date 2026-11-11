# -*- coding: utf-8 -*-
"""round97 · P3 A 批余 6 条（map/D1、map/D8、cashshop/D2、map/D4、statusbar-hud/D1、social-economy/D3）

一次性幂等生成器。零源码行为改动：只写文档 + evidence + notes。
EOL 严格按原文件保留（WLE/dev/PLT = CRLF，P3 Plan/DECISIONS/新文档 = LF）。
绝不写 commit hash / git log（round88 B9b 教训）。
"""
import hashlib
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)

WLE = "docs/features/angel-calibration-d-matrix-2026-11/WLE-TRIAGE-PLAN.md"
P3 = "docs/features/angel-calibration-d-matrix-2026-11/P3-ANGEL-REBUILD-PLAN.md"
MAPS = "docs/features/angel-calibration-d-matrix-2026-11/P3-A-BATCH-REST-MAPS.md"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
EV = "docs/features/angel-calibration-d-matrix-2026-11/evidence/p3-batch-a-rest-round97.json"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave3-p3-batch-a-rest-20261111.md"


# ---------------------------------------------------------------- EOL helpers
def eol_of(path):
    b = open(path, "rb").read()
    t = b.decode("utf-8")
    crlf = t.count("\r\n")
    lone = t.count("\n") - crlf
    return {
        "bytes": len(b),
        "crlf": crlf,
        "lonelf": lone,
        "crcrlf": t.count("\r\r\n"),
        "bom": b[:3] == b"\xef\xbb\xbf",
    }


def read_text(path):
    """二进制读，避免通用换行把 CRLF 掰成 LF（round92 教训）。"""
    return open(path, "rb").read().decode("utf-8")


def write_text(path, text, crlf):
    data = text.replace("\n", "\r\n") if crlf else text
    with open(path, "wb") as f:
        f.write(data.encode("utf-8"))


def patch(path, old, new, label, expect=1):
    """幂等补丁：先判 new 已在（防 NEW 包含 OLD 时二次套娃，round96 教训），再断言 old 计数。"""
    raw = read_text(path)
    crlf = raw.count("\r\n") > (raw.count("\n") - raw.count("\r\n"))
    text = raw.replace("\r\n", "\n") if crlf else raw
    if text.count(new) >= 1:
        print("  [skip] %-52s already present" % label)
        return
    got = text.count(old)
    if got != expect:
        raise AssertionError("%s: expected %d occurrences of <%s>, got %d" % (label, expect, old[:40], got))
    text = text.replace(old, new, expect)
    write_text(path, text, crlf)
    print("  [ok]   %-52s x%d" % (label, expect))


def sha12(path):
    return hashlib.sha256(open(path, "rb").read()).hexdigest()[:12]


# ---------------------------------------------------------------- 主文档内容
MAPS_DOC = """# P3 A 批余 6 条 · 083 ↔ s9 ↔ 当前端 三向映射表

> 触发：用户 m12009「P3（16）按 Angel 全部重做」；round96 已交 A 批前 3 条，
> 本文件承接 A 批余 6 条（`map/D1`、`map/D8`、`cashshop/D2`、`map/D4`、
> `statusbar-hud/D1`、`social-economy/D3`）的 083 取证产物。
> 证据一律来自活 IDB `Angel.exe`(13339) 及 orange-wz 128B 门禁；s9 一律取
> `E:/project/BeiDou-Server_s9/gms-server` 唯一副本。

## 1. `map/D1` — 083 `SaveFuncKeyMap` 89 键差分 vs s9 `ChangeKeyMapPacket`

### 1.1 083 逐字（`CFuncKeyMappedMan::SaveFuncKeyMap @0x58df2f`，49 行）

| 步骤 | 反编译 | 语义 |
| --- | --- | --- |
| 建包 | `COutPacket::COutPacket(a1: 135);` | c2s opcode **135** |
| 模式 | `Encode4(&v7, a2: 0);` | mode **0**（逐条差分） |
| 差分 | `do { if ( *v3 != v3[445] \\|\\| *(v3 + 1) != *(v3 + 446) ) *ZArray<long>::InsertBefore(a1: -1) = v2; ++v2; v3 += 5; } while ( v2 < 89 );` | **89 槽 × 5 字节/槽**，与 `this+449` 的 **445 字节**（`0x1BD`）影子表逐槽比对，只收集变化槽下标 |
| 条目 | `Encode4(&v7, a2: *(v9 + 4 * i)); sub_4E43FA(a1: this + 4 * *(v9 + 4 * i) + *(v9 + 4 * i) + 4, a2: &v7);` | 键下标 4B + 条目 |
| 条目体 | `sub_4E43FA @0x4e43fa`: `return COutPacket::EncodeBuffer(this: a2, a2: this, a3: 5u);` | **5 字节** raw |
| 收尾 | `Encode4(&v7, a2: v4);`（先写条数）+ `CClientSocket::SendPacket` | count 4B + 条目循环 |
| 影子 | `memcpy(this + 449, this + 4, 445u);` | 发送成功后同步影子表 |

`CFuncKeyMappedMan::OnInit @0x58ddb4`：`Decode1(a2)!=0` → 读本地配置否则 `DefaultFuncKeyMap`；
`Decode1==0` → `for ( i = 89; i != 0; --i ) { FUNCKEY_MAPPED::Decode(v4, a2); v4 += 5; }`（89 × 5 = 445）；
末尾统一 `AdaptVirtualKey` + `memcpy(this+449, ...)` + `SaveFuncKeyMap`。
`FUNCKEY_MAPPED::Decode @0x4e4409` = `CInPacket::DecodeBuffer(this: a2, a2: this, a3: 5u);`（**5 字节**）。
`DefaultFuncKeyMap @0x58e114` = `memcpy(a1: this + 4, a2: dword_BD8BCC, a3: 0x1BDu);`（445B 默认表）。
`FuncKeyMapped @0x58e12b`：8 个虚拟键位别名槽（`this+214/414/359/369/149/419/399/409`），
其余落主表 `this+4`。

### 1.2 s9 逐字（`KeymapChangeHandler.java`）

```java
if (p.available() >= 8) {
    int mode = p.readInt();
    if (mode == 0) {
        int numChanges = p.readInt();
        for (...) { int key = p.readInt(); int type = p.readByte(); int action = p.readInt();
                    c.getPlayer().changeKeybinding(key, new KeyBinding(type, action)); }
    } else if (mode == 1) { int itemID = p.readInt(); ... changeKeybinding(91, new KeyBinding(7, itemID)); }
      else if (mode == 2) { ... changeKeybinding(92, ...); }
}
```

`Character.java:391` `private final Map<Integer, KeyBinding> keymap = new LinkedHashMap<>();` —— s9 是**稀疏 map**，不是 89 槽定长数组。

### 1.3 三向判定

| 维度 | 083 | s9 | 当前端 | 结论 |
| --- | --- | --- | --- | --- |
| opcode | 135 | 135 | `Net/OutPacket.h:169 CHANGE_KEYMAP = 135` | 一致 |
| mode | 0（差分） | 0 接受 | `Net/Packets/PlayerPackets.h:107 write_int(0)` | 一致 |
| 计数 | 4B count | `readInt()` | `write_int(updated_actions.size())` | 一致 |
| 条目 | 4B key + 5B raw | 4B key + 1B type + 4B action | `write_int(key) + write_byte(type) + write_int(action)` | **同宽 5B**，字段切分不同但总长一致 |
| 存储 | 89 槽 × 5B + 445B 影子 | 稀疏 map（DB 行 `keymap(characterid,key,type,action)`） | `Keyboard::staged_mappings` 差分 | 语义等价 |
| 触发 | `SaveFuncKeyMap`（含 OnInit 初始化路径） | 客户端上行 | `save_staged_mappings()` → `ChangeKeyMapPacket` | 一致 |

**判定：`DOC_ONLY`**。083 与 s9 在「135 + mode0 + 差分条目」三层同形，本端已按 s9 拆法实现且出站回译保证 v8v3 语义；
89 槽 vs 稀疏 map 是存储形态差异，不构成 wire 分歧。零代码改动。

## 2. `map/D8` — `CHANGE_QUICKSLOT` 32 字节

### 2.1 083 逐字

- `CQuickslotKeyMappedMan::SaveQuickslotKeyMap @0x72b87e`：
  `COutPacket::COutPacket(a1: 183); EncodeBuffer(this + 4, 0x20u); CClientSocket::SendPacket(...); memcpy(this + 36, this + 4, 0x20u);`
  → **opcode 183 + 32 字节 raw 缓冲**，发后同步影子。
- `OnInit @0x72b82a`：`Decode1!=0` → `DecodeBuffer(this+4, 0x20u)` 否则 `DefaultQuickslotKeyMap`；
  随后 `CUIStatusBar::ReDrawQuickslot` + 影子同步 + `SaveQuickslotKeyMap`。
- `DefaultQuickslotKeyMap @0x72b8e6` = `memcpy(a1: this + 4, a2: dword_BDAFAC, a3: 0x20u);`（32B = 8 × int）。

### 2.2 s9 逐字

```java
if (p.available() != QuickslotBinding.QUICKSLOT_SIZE * Integer.BYTES || c.getPlayer() == null) return;
byte[] aQuickslotKeyMapped = new byte[QUICKSLOT_SIZE];
for (...) aQuickslotKeyMapped[i] = (byte) p.readInt();
```
`QUICKSLOT_SIZE = 8`；`DEFAULT_QUICKSLOTS = {0x2A, 0x52, 0x47, 0x49, 0x1D, 0x53, 0x4F, 0x51}`。

### 2.3 三向判定

| 维度 | 083 | s9 | 当前端 | 结论 |
| --- | --- | --- | --- | --- |
| opcode | 183 | 183 (`0xB7`) | `Net/OutPacket.h:204 CHANGE_QUICKSLOT = 183` | 一致 |
| 长度 | `0x20` = 32B | 严格 `8 * 4` = 32B（多 1 字节丢整包） | `8 × write_int` = 32B | 一致 |
| 元素 | raw 8 × 4B | `(byte) readInt` 逐槽截断 | `write_int(uint32_t)` | v83 触发键全部 `< 0x100`，截断不发生 |
| 默认值 | `dword_BDAFAC` 32B | `{0x2A,0x52,0x47,0x49,0x1D,0x53,0x4F,0x51}` | `IO/Keyboard.cpp:215` 同 8 值 | 一致 |

**判定：`DOC_ONLY`**。(byte) 截断只在键值 ≥ 0x100 时才有损，v83 键值域不含此情况；本端 32 字节整型写出与 083 raw 缓冲字节级等价。零代码改动。

## 3. `cashshop/D2` — 083 七子窗 vs 本端单类

### 3.1 083 实测（`*CCSWnd*` 70 符号 / **7 个类**）

| 类 | 方法数 | 代表方法 |
| --- | --- | --- |
| `CCSWnd_Best` | 5 | `ChangeCategory / Draw / GetBestRect / OnCreate / OnMouseButton` |
| `CCSWnd_Char` | 18 | `BlowWeather / CmpAvatar / CmpPet / CreateFloatNotice / Draw / EndChat …` |
| `CCSWnd_Inventory` | 9 | `Draw / EnableExTrunkButton / GetItemSlotRect / GetSlotPositionFromPoint / OnChildNotify / OnCreate` |
| `CCSWnd_List` | 18 | `ChangeCategorySub / ChangePage / Draw / GetCommodityGender / GetPlateRect / OnButtonClicked …` |
| `CCSWnd_Locker` | 8 | `Draw / GetLockerIndex / GetLockerRect / OnCreate / OnMouseButton / OnMouseMove …` |
| `CCSWnd_Status` | 4 | `Draw / OnButtonClicked / OnCreate / OnKeyRet` |
| `CCSWnd_Tab` | 7 | `ChangeCategory / Draw / GetTabIndex / OnChildNotify / OnCreate / OnKeyRet …` |

`CCashShop` 本体 **67 方法**（`Init/Close/OnKey/OnBuyAvatar/SendBuyAvatarPacket/OnPacket/…`）。

### 3.2 当前端

`IO/UITypes/UICashShop.h:31 class UICashShop : public UIElement`，单类承载；
`IO/UITypes/UICashShop.h:56 static constexpr uint8_t MAX_ITEMS = 7u * 2u + 1u;`（= 15，与 `DECISIONS.md:171` 的 `MAX_ITEMS=15` 一致）。

### 3.3 判定

**架构差异，不拆窗（`DOC_ONLY`）**。083 的 7 子窗是 MFC `CWnd` 组合，本端 `UIElement` 单类的 draw/button/send_cursor 已经承载同等职责；
拆成 7 个新类属「重写全部商城 UI」级别，违反 P3 Plan §0 硬规则 2（一片一原子 commit、W 桶零改动）与 `DECISIONS.md:169 BATCH_1_KEEP`。
本文件提供职责映射，供后续若真机观感出现缺口时定位。

## 4. `map/D4` — 083 `CWorldMapDlg` MFC 大窗 vs 本端侧栏

### 4.1 083（21 符号）

`SetWorldMap @0x9ea93b`、`CreateWorldMapDlg @0x9eb366`（`CreateDlg(67, 0, 666, 524, 10, 1, 0)`）、
`LoadMapList @0x9ed38b`、`CheckSpotInfo @0x9edd67`、`CheckLinkInfo @0x9ee00e`、
**`SetWorldMapDeeper @0x9ee30e`**、**`SetWorldMapShallower @0x9ee507`**、`OnMouseButton @0x9ee6e5`、`OnKey @0x9ee717`。

### 4.2 IMG 实测（orange-wz 128B）

`UIWindow.img/WorldMap` 直接子节点**只有 2 个**：`title`（CANVAS）+ `Border`（LIST）；
`WorldMap/WorldMapSearch` **不存在**。`Border` 8 片实测 `0=60×33 / 1=1×33 / 2=7×32 / 3=7×1 / 4=7×1 / 5=7×18 / 6=1×18 / 7=7×18`。

### 4.3 判定

**资源缺失 + 架构差异（`DOC_ONLY`）**。083 的 `Deeper/Shallower` 是「进入/返回子地图」的层级导航，靠 `LoadMapList` 的数据结构驱动；
本端 `UIWorldMap` 走侧栏状态机 + 付费传送，且 `WorldMapSearch` 引用恒空（`IO/UITypes/UIWorldMap.cpp:171` 已注「本 v83 包缺素材 → 默认隐藏」）。
两者 Border 件实测一致，几何差异已在 round96 `corewindow/D10` 结案（654×521 vs 666×524，不改）。

## 5. `statusbar-hud/D1` — 083 单体 45 方法 → 本端 10+2 类

### 5.1 083 `CUIStatusBar` 45 方法按功能簇实测

| 簇 | 数量 | 方法 |
| --- | --- | --- |
| lifecycle | 8 | `0CUIStatusBar / 1CUIStatusBar / _ECUIStatusBar / OnCreate / Draw / HitTest / Update / CreateInstance` |
| input | 7 | `OnSetFocus / OnChildNotify / OnMouseEnter / OnButtonClicked / OnKey / OnMouseButton / OnMouseMove` |
| status | 1 | `SetStatusValue` |
| quickslot | 8 | `ClearToolTip / ProcessToolTip / ReDrawQuickslot / GetShortCutIndexByPos / ResetButtonToolTip / ToggleQuickSlot / SetButtonBlink / SetShortcutTooltip` |
| chat | 14 | `GetWhisperTarget / SetWhisperTarget / ChangeWhisperTarget / TryBeginWhisper / ConvertWhisperToNormal / StartChat / EndChat / SetChatType / ChangeChatWndSize / MakeCtrlEdit / GetChatLog / ChatLogAdd / ChatLogDraw / 0CChatLog` |
| misc | 7 | `GetRTTI / IsKindOf / SendClaim / GetEmotionKey / FloatNotice / SetProblem / SendGroupMessage` |

合计 `8 + 7 + 1 + 8 + 14 + 7 = 45`（现场减算，非抄表）。

### 5.2 当前端 HUD 面类（`IO/UITypes/*.h` 实测 18 个）

`UIStatusBar` / `UIChatBar` / `UIChat` / `UIBossHP` / `UIBuffList` / `UITemporaryStatView` /
`UIQuickslotConfig` / `UIKeyConfig` / `UIParty` / `UIPartySearch` / `UIPartyRace` /
`UIClock` / `UIComboCounter` / `UIEnergyBar` / `UIStatusMessenger` / `UIFloatNotice` /
`UISpecial` / `UIWhisperTargetDlg`。

### 5.3 簇 → 类映射

| 083 簇 | 当前端承载 |
| --- | --- |
| lifecycle / input / status | `UIStatusBar`（`draw`/`update`/`send_cursor`/`send_key`/`button_pressed`） |
| quickslot | `UIStatusBar`（`toggle_qs`/`activate_classic_qs_slot`/`start_button_blink`）+ `UIQuickslotConfig` + `UIKeyConfig` |
| chat | `UIChatBar` + `UIChat` + `UIWhisperTargetDlg` + `UIStatusMessenger` |
| misc | `UIFloatNotice` + `UISpecial` + `UIBossHP` + `UIBuffList` + `UITemporaryStatView` + `UIClock` + `UIComboCounter` + `UIEnergyBar` + `UIParty*` |

### 5.4 判定

**架构差异，不合并（`DOC_ONLY`）**。合并回单体等于重写全部 HUD 类，`DECISIONS.md:73 BATCH_1_KEEP` 已裁定且本轮 083 取证支持该结论（45 方法全部有簇归属、无孤儿方法）。

## 6. `social-economy/D3` — 联盟双入口

### 6.1 083 实测

- `*CUIAlliance*` 符号命中 **0** —— 083 **无独立联盟浮窗类**。
- `CTabGuildAlliance` **9 符号**：`LoadData @0x9126f4`（228 行，5 行循环 × 32 字节重置 + `GetAllianceGradeName` 逐级名 + `MakeNoticeLayer`）、
  `MakeNoticeLayer @0x9146e4`、`OnPartyInvite @0x9153dc`、`GetGuildItem @0x9155e9`、`GetToolTipString @0x9156c5`、
  `GUILDITEM` 构造/析构、`ZArray<GUILDITEM>::InsertBefore`。
- `CreateAllianceButton @CUIUserList 0x91eba3` / `DestroyAllianceButton @0x91ee49` —— 联盟入口是 `CUIUserList` 上的**按钮**，不是独立窗。

### 6.2 当前端

`IO/UITypes/UIAlliance.h:33` 独立联盟窗（由 `UIGuild` 的 `BtAlliance` 打开）；
`UIGuild` 的 4 处用户入口已在 round85 收敛到 `UIUserList::Tab::GUILD/UNION`（commit `4b2452c4`）。

### 6.3 判定

**保持现状（`DOC_ONLY`）**。083 侧确无独立窗、且入口是共用窗按钮 —— 本端 `UIUserList` 的 UNION 页签已在 round85 对齐；
保留 `UIAlliance` 浮窗是因为 s9 已用双入口（`DECISIONS.md:113 BATCH_1_KEEP`「合并风险极高」）。
本轮不删、不合并、不改入口。

## 7. 小结

| 条 | 083 取证形态 | s9 对照 | 判定 | 代码改动 |
| --- | --- | --- | --- | --- |
| `map/D1` | `SaveFuncKeyMap @0x58df2f` 89×5B 差分 + `sub_4E43FA` 5B | mode0 差分，稀疏 map | `DOC_ONLY` | 0 |
| `map/D8` | `SaveQuickslotKeyMap @0x72b87e` 183 + `0x20` raw | 严格 32B，`(byte)` 截断 | `DOC_ONLY` | 0 |
| `cashshop/D2` | `CCSWnd_*` 7 类 / 70 符号 + `CCashShop` 67 | 单窗 | 架构差异 | 0 |
| `map/D4` | `CWorldMapDlg` 21 符号 + `Deeper/Shallower` | 侧栏 + 付费传送 | 资源缺失 + 架构差异 | 0 |
| `statusbar-hud/D1` | `CUIStatusBar` 45 方法 = 8+7+1+8+14+7 | 10+2 类拆分 | 架构差异 | 0 |
| `social-economy/D3` | `CUIAlliance` 0 / `CTabGuildAlliance` 9 / 入口按钮 | 双入口 | 保持现状 | 0 |

六条均**零 wire / 零协议 / 零 WZ / 零源码行为改动**；A 批 9 条至此全部完成，B/C 批 7 条待续。
"""

WLE_SEC14 = """## 14. round97：P3 A 批余 6 条（D1 / D8 / D2 / D4 / D1 / D3）

> 触发：用户 m12009「P3（16）按 Angel 全部重做」。承接 round96（A 批前 3 条），
> 本轮做 A 批**余 6 条**，逐条 083 活 IDA 取证 + s9 源码 + 当前端字面量 三向对拍。
> 映射表全文见 `P3-A-BATCH-REST-MAPS.md`。

### 14.1 六条逐条结论

- `map/D1`：083 `SaveFuncKeyMap @0x58df2f` = `COutPacket(135)` + `Encode4(mode=0)` +
  `89` 槽 × `5` 字节与 `this+449` 的 **445B**（`0x1BD`）影子表逐槽差分，条目体
  `sub_4E43FA @0x4e43fa` = `EncodeBuffer(5u)`。s9 `KeymapChangeHandler` mode0 同样是
  「count + (key:int, type:byte, action:int)」，`Character.keymap` 是**稀疏 map**。
  本端 `ChangeKeyMapPacket` 已 mode0 + 差分 + 5B 条目 → **`DOC_ONLY`**，零改动。
- `map/D8`：083 `SaveQuickslotKeyMap @0x72b87e` = `COutPacket(183)` + `EncodeBuffer(0x20)`（32B raw），
  默认表 `dword_BDAFAC`；s9 严格 `QUICKSLOT_SIZE * Integer.BYTES` = 32B，逐槽 `(byte)readInt`。
  本端 8 × `write_int` = 32B，默认值 `{0x2A,0x52,0x47,0x49,0x1D,0x53,0x4F,0x51}` 与 s9 逐项一致
  → **`DOC_ONLY`**，零改动（截断只在键值 ≥ 0x100 发生，v83 键值域不含）。
- `cashshop/D2`：083 `*CCSWnd*` 实测 **70 符号 / 7 个类**（Best 5 / Char 18 / Inventory 9 /
  List 18 / Locker 8 / Status 4 / Tab 7），`CCashShop` 本体 **67 方法**；本端 `UICashShop` 单类
  + `MAX_ITEMS = 7*2+1 = 15` → **架构差异**，不拆窗。
- `map/D4`：083 `CWorldMapDlg` 21 符号（`SetWorldMapDeeper @0x9ee30e` /
  `Shallower @0x9ee507` / `LoadMapList @0x9ed38b`），IMG `UIWindow.img/WorldMap` 只有
  `title` + `Border` 两节点、`WorldMapSearch` **不存在** → **资源缺失 + 架构差异**，不搬。
- `statusbar-hud/D1`：083 `CUIStatusBar` **45 方法实测**，按簇 `lifecycle 8 / input 7 /
  status 1 / quickslot 8 / chat 14 / misc 7`（`8+7+1+8+14+7 = 45` 现场减算）；本端 18 个 HUD 面类
  逐簇有归属、无孤儿方法 → **架构差异**，不合并。
- `social-economy/D3`：083 `*CUIAlliance*` 符号 **0**，`CTabGuildAlliance` **9 符号**，
  联盟入口是 `CUIUserList::CreateAllianceButton @0x91eba3` 的**按钮**而非独立窗；
  本端 `UIAlliance` 浮窗 + `UIUserList` UNION 页签（round85 `4b2452c4` 已收敛）
  → **保持现状**，不删不合并。

### 14.2 本轮口径

- 六条**全部 `DOC_ONLY`**：零 wire / 零协议 / 零 WZ / 零源码行为改动。
- A 批 9 条（3 + 6）至此完成；B 批 5 条（`skill/D9`、`social-economy/D12`、`pet-summon/D2`、
  `login-flow/D13`、`social-economy/D9`）与 C 批 2 条（`corewindow/D2`、`quest/D5`）待续。
- 算术不动：`168 = 76+38+14+40` / `reopen 27 = 已注销 10 + 残余 17` /
  `残余 17 = P1 0 + P2 1 + P3 16` 三条守恒式本轮全部未触及。

"""

P3_SEC7 = """## 7. round97 回执（A 批余 6 条）

| 条 | 结果 | 产出 |
| --- | --- | --- |
| `map/D1` | `DOC_ONLY`：083 89 槽 × 5B 差分 vs s9 mode0 稀疏 map，本端已同形 | `P3-A-BATCH-REST-MAPS.md` §1 |
| `map/D8` | `DOC_ONLY`：083 `183 + 0x20` raw vs s9 严格 32B，本端 8×`write_int` 字节级等价 | 同上 §2 |
| `cashshop/D2` | 架构差异：083 `CCSWnd_*` 7 类 / 70 符号 vs 本端单类，不拆窗 | 同上 §3 |
| `map/D4` | 资源缺失 + 架构差异：`WorldMap` 仅 `title`+`Border`，`WorldMapSearch` 不存在 | 同上 §4 |
| `statusbar-hud/D1` | 架构差异：45 方法簇 `8/7/1/8/14/7` 全部有本端归属，不合并 | 同上 §5 |
| `social-economy/D3` | 保持现状：083 `CUIAlliance` 0 符号，入口是共用窗按钮 | 同上 §6 |

六条均**零 wire / 零行为改动**；A 批 9 条（前 3 + 余 6）完成，B/C 批 7 条待续。
"""

DEV_ROW = "| **2-45** | round97 · P3 A 批余 6 条：`map/D1` 89 键差分 + `map/D8` 32B 快捷栏 + `cashshop/D2` 七子窗 + `map/D4` 侧栏 + `statusbar-hud/D1` 45 方法簇 + `social-economy/D3` 联盟双入口 —— **六条全 DOC_ONLY**（零源码改动） | — | `round97` |\n"

PLT_SEC56 = """## 56. round97 · P3 A 批余 6 条（六条全 DOC_ONLY / 零源码改动）

- **56-1** `map/D1`：083 `SaveFuncKeyMap @0x58df2f` 89 槽 × 5B 差分 + `sub_4E43FA` 5B 条目，
  s9 mode0 同形；本端 `ChangeKeyMapPacket` 已对齐。真机待验：改 2~3 个键 → 退出重登，
  确认键位保留且未被重置（服务端 `keymap` 表应只增改对应行）。
- **56-2** `map/D8`：083 `SaveQuickslotKeyMap @0x72b87e` = `183 + 0x20` raw；s9 严格 32B。
  真机待验：改快捷栏触发键 → 重登确认 8 槽保留；若整包被服务端丢弃（键位回到默认），
  即为长度不匹配信号。
- **56-3** `cashshop/D2`：083 `CCSWnd_*` 7 类 / 70 符号 vs 本端单类，**不拆窗**。
  真机待验：商城开窗 → 切分类/翻页/预览角色/看 locker/wish，确认无空白页与错位。
- **56-4** `map/D4`：083 `CWorldMapDlg` 21 符号含 `Deeper/Shallower` 层级导航；
  IMG `WorldMap` 仅 `title`+`Border`、`WorldMapSearch` 不存在。真机待验：世界地图开窗 →
  点击地图节点跳转，确认侧栏与传送正常（搜索框预期不显示）。
- **56-5** `statusbar-hud/D1`：083 `CUIStatusBar` 45 方法簇 `8/7/1/8/14/7` 全部有本端归属。
  真机待验：HUD 全功能走一遍（快捷栏折叠/展开、聊天三态、Boss 血条、Buff 列表、时钟、
  组队、低血闪烁），确认无缺失交互。
- **56-6** `social-economy/D3`：083 无独立联盟窗（`CUIAlliance` 0 符号），入口是共用窗按钮；
  本端保留 `UIAlliance` 浮窗 + UNION 页签。真机待验：G 键 → 家族页签 → 联盟按钮，
  确认浮窗可开可关、页签不串位。
- **56-7** 限制：A 批 9 条完成；B 批 5 条、C 批 2 条未做。本轮**零源码改动**，
  故不声称真重编覆盖。构建门禁回归结果如实记录：`GATE OK`（协议门禁通过），
  但 MSBuild 被**工作树里他人外部在途 WIP** 阻断 —— `UIChatBar.cpp(2581/2595) error C2664`、
  `UIGender.cpp(189) fatal error C1070`、`UIMedal.cpp(38/67/161)`、`UISpecial.cpp(22/96)`、
  `UITemporaryStatView.cpp(193)`，共 9 error / 3 fatal，全部落在本轮从未触碰的文件
  （`git status` 均为 `M`/`??` 的外部改动，且这些文件里不含本轮任何标记串）。
  `EXITCODE=1`。round96 真重编（19:22）时这些文件尚未坏，坏点来自此后外部推进，
  与本轮交付无关；待外部 WIP 修好后需重跑一次门禁回归确认基线回绿。
"""

EV_JSON = """{
 "round": 97,
 "trigger": "m12009 P3(16) 按 Angel 083 全部重做 / 承接 round96 A 批前 3 条",
 "scope": "P3 A 批余 6 条",
 "items": {
  "map/D1": {
   "verdict": "DOC_ONLY",
   "ida": "CFuncKeyMappedMan::SaveFuncKeyMap @0x58df2f (49 lines) + sub_4E43FA @0x4e43fa + FUNCKEY_MAPPED::Decode @0x4e4409 + DefaultFuncKeyMap @0x58e114 + FuncKeyMapped @0x58e12b",
   "ida_facts": {
    "opcode": 135,
    "mode": 0,
    "slots": 89,
    "slot_bytes": 5,
    "shadow_bytes": 445,
    "shadow_hex": "0x1BD",
    "entry_bytes": 5
   },
   "s9": "KeymapChangeHandler mode0: count + (key:int, type:byte, action:int); Character.keymap 为稀疏 LinkedHashMap",
   "client": "Net/Packets/PlayerPackets.h ChangeKeyMapPacket: write_int(0) + write_int(size) + per-entry int/byte/int",
   "conclusion": "083 与 s9 在 opcode/mode/差分条目三层同形，本端已对齐；89 槽 vs 稀疏 map 属存储形态差异"
  },
  "map/D8": {
   "verdict": "DOC_ONLY",
   "ida": "CQuickslotKeyMappedMan::SaveQuickslotKeyMap @0x72b87e + OnInit @0x72b82a + DefaultQuickslotKeyMap @0x72b8e6",
   "ida_facts": {"opcode": 183, "body_bytes": 32, "body_hex": "0x20", "shadow_bytes": 32},
   "s9": "QuickslotKeyMappedModifiedHandler: p.available() != QUICKSLOT_SIZE(8) * Integer.BYTES(4) -> return; per-slot (byte) readInt",
   "client": "Net/Packets/SkillMacroPackets.h ChangeQuickslotPacket: 8 x write_int = 32B; IO/Keyboard.cpp:215 默认值 {0x2A,0x52,0x47,0x49,0x1D,0x53,0x4F,0x51}",
   "conclusion": "(byte) 截断仅在键值 >= 0x100 发生，v83 键值域不含；32B raw 与 8x int 字节级等价"
  },
  "cashshop/D2": {
   "verdict": "ARCH_DIFF_NO_SPLIT",
   "ida": "*CCSWnd* 70 symbols / 7 classes; CCashShop 67 symbols",
   "ida_facts": {
    "classes": {"CCSWnd_Best": 5, "CCSWnd_Char": 18, "CCSWnd_Inventory": 9, "CCSWnd_List": 18, "CCSWnd_Locker": 8, "CCSWnd_Status": 4, "CCSWnd_Tab": 7},
    "class_sum": 70,
    "cashshop_methods": 67
   },
   "client": "IO/UITypes/UICashShop.h:31 单类; MAX_ITEMS = 7*2+1 = 15",
   "conclusion": "MFC 七子窗 vs UIElement 单类职责等价；拆窗属重写全部商城 UI，不做"
  },
  "map/D4": {
   "verdict": "RESOURCE_MISSING_ARCH_DIFF",
   "ida": "CWorldMapDlg 21 symbols incl. SetWorldMapDeeper @0x9ee30e / SetWorldMapShallower @0x9ee507 / LoadMapList @0x9ed38b",
   "img_gate": {"WorldMap_children": ["title", "Border"], "WorldMapSearch": "NOT_FOUND", "Border_sizes": {"0": "60x33", "1": "1x33", "2": "7x32", "3": "7x1", "4": "7x1", "5": "7x18", "6": "1x18", "7": "7x18"}},
   "client": "IO/UITypes/UIWorldMap.cpp:53 引用 WorldMapSearch 恒空 -> :171 已注默认隐藏; 侧栏状态机 + 付费传送",
   "conclusion": "083 层级导航靠 LoadMapList 数据结构；本端侧栏等价；搜索轨因资源缺失不发明"
  },
  "statusbar-hud/D1": {
   "verdict": "ARCH_DIFF_NO_MERGE",
   "ida": "CUIStatusBar 45 symbols (0x8cfad8..0xa14ab7)",
   "ida_facts": {
    "clusters": {"lifecycle": 8, "input": 7, "status": 1, "quickslot": 8, "chat": 14, "misc": 7},
    "cluster_sum": 45,
    "arithmetic": "8+7+1+8+14+7=45"
   },
   "client": "IO/UITypes/*.h 实测 18 个 HUD 面类，逐簇有归属、无孤儿方法",
   "conclusion": "合并回单体等同重写全部 HUD 类；083 45 方法全部有簇归属，支持保持拆分"
  },
  "social-economy/D3": {
   "verdict": "KEEP_AS_IS",
   "ida": "*CUIAlliance* 0 symbols; CTabGuildAlliance 9 symbols; CUIUserList::CreateAllianceButton @0x91eba3",
   "ida_facts": {"cui_alliance_symbols": 0, "tab_guild_alliance_symbols": 9, "entry_is_button": true},
   "client": "IO/UITypes/UIAlliance.h:33 独立浮窗 + UIUserList UNION 页签（round85 4b2452c4 已收敛 4 处入口）",
   "conclusion": "083 无独立窗但 s9 已用双入口；删浮窗或合并入口均属行为级改动，保持现状"
  }
 },
 "batch_a": {"total": 9, "done_round96": 3, "done_round97": 6, "remaining": 0},
 "batch_remaining": {"B": 5, "C": 2, "total": 7},
 "zero_wire": true,
 "zero_behavior_change": true,
 "zero_source_change": true,
 "build_gate_regression": {
  "gate": "GATE OK: opcode matrix matches both sources (s9 a78ba681a4)",
  "msbuild_exit": 1,
  "cause": "external in-flight WIP in worktree (not touched by round97, no round97 markers)",
  "error_files": {
   "IO/UITypes/UIChatBar.cpp": "2581,2595 error C2664 (string -> const char*)",
   "IO/UITypes/UIGender.cpp": "189 fatal error C1070 (#if/#endif mismatch)",
   "IO/UITypes/UIMedal.cpp": "38,67 error C2440 + 161 fatal error C1070",
   "IO/UITypes/UISpecial.cpp": "22 error C2440 + 96 fatal error C1070",
   "IO/UITypes/UITemporaryStatView.cpp": "193 fatal error C1070"
  },
  "error_count": 9,
  "fatal_count": 3,
  "real_rebuild_claimed": false,
  "note": "round96 19:22 real rebuild was green; these breakages arrived afterwards from external WIP. Rerun gate regression once external WIP is fixed."
 },
 "arithmetic_unchanged": {
  "classification_168": "76+38+14+40=168",
  "reopen": "27 = closed 10 + remaining 17",
  "remaining_17": "P1 0 + P2 1 + P3 16"
 },
 "orange_wz_gate": "GATE_OK_128B_SKILL_NO_MAIN_WORLDMAP_TWO_NODES（round96 落盘，本轮复用 WorldMap 2 节点与 Border 8 片实测）",
 "constraints": [
  "083/v83 only，禁引 095",
  "s9 协议优先，W 桶 6 字段宽度/类型与 27 条 wire KEEP 零改动",
  "数值重合 != 语义重合",
  "门禁 5 禁想象修改",
  "门禁 18 REA -> IDA -> orange-wz -> .nx 只读取证"
 ]
}
"""

NOTES_DOC = """# round97 · P3 A 批余 6 条（六条全 DOC_ONLY）

## 1. 本轮做了什么

承接 round96（A 批前 3 条：`login-flow/D12`、`skill/D11`、`corewindow/D10`），
本轮把 A 批**余 6 条**逐条走完 083 活 IDA 取证：`map/D1`、`map/D8`、`cashshop/D2`、
`map/D4`、`statusbar-hud/D1`、`social-economy/D3`。

方法固定为三向对拍：**083 反编译（活 IDB 13339）→ s9 服务端源码（唯一副本）→ 当前端字面量**，
再叠 `DECISIONS.md` 原裁定。任一项缺证即只登记不实施（P3 Plan §0 硬规则 1）。

## 2. 关键证据（决定判定的那几行）

- `map/D1`：`SaveFuncKeyMap @0x58df2f` 的 `while ( v2 < 89 )` + `v3 += 5` + `v3[445]` 三段
  把「89 槽 × 5B + 445B 影子」钉死；`sub_4E43FA` 全文只有一行 `EncodeBuffer(5u)`。
  s9 `KeymapChangeHandler` mode0 的 `readInt/readByte/readInt` 合起来同样是 5B —— 总长一致，
  字段切分不同但语义等价，故不动协议。
- `map/D8`：083 `EncodeBuffer(this + 4, 0x20u)` 与 s9 `QUICKSLOT_SIZE * Integer.BYTES`
  是同一个 32；s9 的 `(byte) readInt` 截断只在键值 ≥ 0x100 才有损，而 v83 触发键域不含此值。
- `cashshop/D2`：`*CCSWnd*` 用正则按类聚合后得到 **7 类**，方法数 `5+18+9+18+8+4+7 = 69`
  再加 `ZArray` 模板实例 1 条 = 70 符号 —— 与 `DECISIONS.md:170`「66 方法面」不是同一计数口径
  （66 指 `CCashShop` 本体的早期估计，实测 67），本轮以实测为准。
- `statusbar-hud/D1`：45 方法按簇求和 `8+7+1+8+14+7 = 45` 现场减算闭合，无孤儿方法。
- `social-economy/D3`：`*CUIAlliance*` **0 命中**是本轮最硬的一条 —— 083 确实没有独立联盟窗，
  但入口按钮 `CreateAllianceButton @CUIUserList 0x91eba3` 说明它挂在共用窗上，与本端
  round85 的收敛方向一致。

## 3. 为什么不改代码

六条全部落在两类：**(a) 本端已等价**（`map/D1` 差分、`map/D8` 32B）、
**(b) 架构差异且改动等于重写**（`cashshop/D2` 七子窗、`map/D4` 侧栏、`statusbar-hud/D1` 单体重构、
`social-economy/D3` 双入口）。P3 Plan §0 硬规则 2 要求「一片一原子 commit、W 桶零改动」，
把 (b) 类硬改成 083 形态会同时破坏 s9 联机与已对齐排版 —— 这正是 round90/91/92 三轮
同族病的反面。故本轮只交证据 + 映射表，零源码行为改动。

## 4. 与历史结论的关系

`DECISIONS.md` 这 6 条的裁定原文（`BATCH_0_BASELINE` / `BATCH_1_KEEP`）**不改**：
本轮不是翻案，而是把 round94 里「按当时证据判定保持」升级成「按 083 逐条取证后判定保持」，
证据强度不同、结论相同。历史证据链保留，新口径写在 WLE §14 与 P3 Plan §7。

## 5. 验证

- harness：静态守卫覆盖六条 083 锚点、s9 关键行、当前端字面量、四文档落位、EOL、WIP 隔离；
  场景对算术做真值表（`89*5=445`、`8*4=32`、`7` 子窗、`8+7+1+8+14+7=45`）。
- GATE：`python tools/gen_opcode_matrix.py --check`。
- 构建：**零源码改动，不声称真重编覆盖**。门禁回归如实记录：协议门禁 `GATE OK`，
  但 MSBuild `EXITCODE=1` —— 被工作树里**他人外部在途 WIP** 阻断（`UIChatBar.cpp(2581/2595) C2664`、
  `UIGender.cpp(189) C1070`、`UIMedal.cpp(38/67/161)`、`UISpecial.cpp(22/96)`、
  `UITemporaryStatView.cpp(193)`，9 error / 3 fatal）。这些文件本轮从未触碰、
  `git status` 全为外部 `M`/`??`、且不含本轮任何标记串；round96（19:22）真重编时尚未坏。
  外部 WIP 修好后须重跑门禁回归确认基线回绿 —— 本轮不把外部坏点记成自己的失败，也不假装绿。
- 提交：零署名，精确 `git add`，不夹带工作树里的他人 WIP。
"""


def main():
    print("== round97 deliver ==")

    # 1) 新建映射表文档（LF）
    if os.path.isfile(MAPS):
        old = read_text(MAPS)
        print("  [skip] %-52s exists (%d bytes)" % ("P3-A-BATCH-REST-MAPS.md", len(old)))
    else:
        write_text(MAPS, MAPS_DOC, crlf=False)
        print("  [ok]   %-52s created" % "P3-A-BATCH-REST-MAPS.md")

    # 2) WLE §14（原文 CRLF；patch() 内部统一 LF 匹配 + 写回还原 CRLF）
    if "## 14. round97" in read_text(WLE):
        print("  [skip] %-52s already present" % "WLE \u00a714")
    else:
        patch(WLE, "  \u771f\u673a\u5bf9\u9f50\u8bb0 PLT \u00a755-3\u3002\n",
              "  \u771f\u673a\u5bf9\u9f50\u8bb0 PLT \u00a755-3\u3002\n\n" + WLE_SEC14.rstrip("\n") + "\n",
              "WLE \u00a714 round97")

    # 3) P3 Plan §7 回执（LF）
    patch(P3, "三条均零 wire / 零行为改动；A 批余 6 条待续。\n",
          "三条均零 wire / 零行为改动；A 批余 6 条待续。\n\n" + P3_SEC7, "P3 Plan §7 回执")

    # 4) P3 Plan §3 执行顺序：步骤 2 标完成（LF）
    patch(P3, "2. 批 A 余 6 条 —— 每条补一个 IDA/IMG 探针后落。",
          "2. ~~批 A 余 6 条 —— 每条补一个 IDA/IMG 探针后落。~~ **round97 已完成**（见 §7，六条全 DOC_ONLY）。",
          "P3 Plan §3 step2 done")

    # 5) dev-plan §4 2-45 行（CRLF）
    # 锚定 2-44 行尾（唯一串），把 2-45 插在其后，保证行序单调
    patch(DEV, "\u7f16\u7801 666\u00d7524 \u5750\u5b9e\uff08\u7eaf\u6ce8\u91ca\uff0c\u96f6\u884c\u4e3a\uff09 | \u2014 | `round96` |\n",
          "\u7f16\u7801 666\u00d7524 \u5750\u5b9e\uff08\u7eaf\u6ce8\u91ca\uff0c\u96f6\u884c\u4e3a\uff09 | \u2014 | `round96` |\n" + DEV_ROW,
          "dev-plan 2-45 row", expect=1)

    # 6) PLT §56（CRLF）
    plt_raw = read_text(PLT)
    if "## 56. round97" in plt_raw:
        print("  [skip] %-52s already present" % "PLT §56")
    else:
        body = plt_raw.rstrip("\r\n") + "\r\n\r\n" + PLT_SEC56.replace("\n", "\r\n").rstrip("\r\n") + "\r\n"
        with open(PLT, "wb") as f:
            f.write(body.encode("utf-8"))
        print("  [ok]   %-52s appended" % "PLT §56")

    # 7) evidence（LF）
    if os.path.isfile(EV):
        print("  [skip] %-52s exists" % "evidence json")
    else:
        write_text(EV, EV_JSON, crlf=False)
        print("  [ok]   %-52s created" % "evidence json")

    # 8) notes（LF）
    if os.path.isfile(NOTES):
        print("  [skip] %-52s exists" % "notes md")
    else:
        write_text(NOTES, NOTES_DOC, crlf=False)
        print("  [ok]   %-52s created" % "notes md")

    print("\n== SHA / EOL ==")
    for p in [MAPS, WLE, P3, DEV, PLT, EV, NOTES]:
        e = eol_of(p)
        print("  %-14s %-88s %s" % (sha12(p), p, e))
    print("\nROUND97_DELIVER_ALL_OK")


if __name__ == "__main__":
    main()
