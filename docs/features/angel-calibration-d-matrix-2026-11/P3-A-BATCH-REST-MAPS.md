# P3 A 批余 6 条 · 083 ↔ s9 ↔ 当前端 三向映射表

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
| 差分 | `do { if ( *v3 != v3[445] \|\| *(v3 + 1) != *(v3 + 446) ) *ZArray<long>::InsertBefore(a1: -1) = v2; ++v2; v3 += 5; } while ( v2 < 89 );` | **89 槽 × 5 字节/槽**，与 `this+449` 的 **445 字节**（`0x1BD`）影子表逐槽比对，只收集变化槽下标 |
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
