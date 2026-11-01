# r18 · Wave0 小步 REA 升层（增量）

> 日期：2026-10-22  
> 目标 exe：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe`  
> IDA MCP：`http://127.0.0.1:13338`（`C:\Users\11036\.rea\ida-mcp.json`）  
> 证据目录：[evidence/r18/](evidence/r18/)  
> 本轮：**只做小步 REA / 文档**，不改业务代码。

---

## 0. 结论：小步 REA 可行

**可以，而且应该这样做。** 先前失败来自整包 `rea analyze` / 长会话导致的 PING timeout，不是 REA 或 IDA 不能用。

| 模式 | 结果（本轮实测） |
| --- | --- |
| 单次 `rea search … --kind strings` | **~2s** 完成，稳定 |
| 单次中等 `decompile`（如 `0x624E13`、`0x5FBA85`、`0x8D3B2F`） | **数秒**；REA 偶发 `unreadable_output` 时，用 MCP `/output/<uuid>.json` 或直调 `decompile` 可取回 |
| 超大函数（`sub_8D01B2` size `0x392d`） | REA 必截断 → **curl 取回** 即得全文（本轮 `ida_mcp_output_StatusBar_8D01B2.json` ≈112KB） |
| `rea analyze` 整包 | **禁止**（历史 PING timeout） |

环境：本轮 **13338 可用**（`initialize` / `tools/list` / `decompile` / `find_regex` / `xrefs_to` 均通）。Cursor 侧 `user-ida-pro-mcp` namespace 发现失败不影响 REA/HTTP。

---

## 1. 本轮逐步结果

证据均落在 `evidence/r18/`。状态：成功 / 跳过 / 待重试。

### 1.1 StatusBar（底栏 / 快捷格）

| 步骤 | 命令/动作 | 状态 | 产物 |
| --- | --- | --- | --- |
| search `BtShop` | `rea search … BtShop` | 成功（n=0 明文） | `rea_search_BtShop.json` |
| decompile `0x8D01B2` | REA → 截断 → curl `/output/…` | **成功（升层）** | `rea_decompile_StatusBar_8D01B2.txt`、`ida_mcp_output_StatusBar_8D01B2.json`、`statusbar_createctrl_coords.txt` |
| decompile 邻域 `0x8D3B2F` | MCP decompile | 成功（非 OnButton） | `rea_decompile_StatusBarHandler_8D3B2F.txt` |
| decompile `0x8DF4B1` / `0x8D76D7` | MCP | 成功（仍非 id switch） | `rea_decompile_StatusBar_8DF4B1.txt` 等 |
| OnButton / QS 点击分发 | 邻域无 `case 1000..` | **待重试** | 候选更大函数 `sub_8D6409` / `sub_8D850B` / `sub_8DBB93` |

**证据层：CreateCtrl 坐标 / 控件 id → C；点击分发行为 → 仍 C−（待 OnButton EA）。**

控件表（CreateCtrl 实测，与 r15 一致）：

| id | x | y | 语义（对照 `UIStatusBar.cpp` 注释 + `BtNPT` 串） |
| --- | ---: | ---: | --- |
| 1000 | 573 | 543 | BtShop |
| 1001 | 629 | 543 | BtNPT（明文 `UI/StatusBar.img/BtNPT` @`0xb3bab4` → xref 仅 CreateCtrl） |
| 1002 | 685 | 543 | BtMenu |
| 1007 | 741 | 543 | BtShort |
| 2000..2003 | 618..708 | 515 | Equip/Inven/Stat/Skill 键图标 |
| 1005 | 738 | 515 | KeySet（**不是**旧注释 2004） |
| 1006 | 768 | 515 | QuickSlot 相关（**不是** 2005） |
| 1009 | 573 | 515 | QuickSlotD / 聊天侧（**不是** 2006） |

> 旧注释把 KeySet/QS 写成 2004–2006；**编号以 CreateCtrl 的 1005/1006/1009 为准**，坐标一致。

### 1.2 World / Channel（`DrawWorldItems`）

| 步骤 | 状态 | 产物 |
| --- | --- | --- |
| decompile `0x624E13` | **成功**（REA 报错后 MCP 取回） | `rea_decompile_DrawWorldItems_624E13.txt`、`DrawWorldItems_624E13_behavior.txt` |
| search `Channel` | 成功但命中为 Messenger 事件名 | `rea_search_Channel.json`、`ida_get_string_Channel.json` |
| search `BtGoWorld` | 成功 n=0（exe 无此明文） | `rea_search_BtGoWorld.json` |

**证据层：频道槽创建逻辑 → C（本函数内）；世界过滤 / BtGoWorld 回调 → 仍 C−（串不在 ASCII，行为在其它 `sub_*`）。**

`sub_624E13` 对照要点：

1. `world_count = *(*(this+32)+512)` → 循环画世界名牌。  
2. `channel_count` 来自 `*(this+32)+396` 列表长度 → `*(this+33)`。  
3. 频道控件 id = **`v54 + 1000`**（从 1000 起）。  
4. `v54 >= channel_count`：仍 CreateCtrl，再 vtable+28 **禁用**（空槽）。  
5. 有频道时：string table 1355 格式化标签；`v17[2]!=0` 时画负载条。  
6. **本函数没有 `channel_count < 2` 跳过世界** → 当前端「少于 2 频道不显示世界」是**实现侧过滤**，不在 DrawWorldItems。

### 1.3 Login / Loading

| 步骤 | 状态 | 产物 |
| --- | --- | --- |
| REA search `Notice` / `signboard` / `frame` | 成功；有效 UI 路径命中弱 | `rea_search_*.json` |
| IDA `find_regex` `Notice/Loading`、`signboard`、`Common/frame` | **n=0** | `ida_find_regex_*.json` |
| 命中 `UI/Login.img/ViewAllChar/Job/0` @`0xaf6fd0` | **成功** | xref → `sub_6075DD` |
| decompile `0x6075DD` | **成功** | `rea_decompile_ViewAllChar_6075DD.txt` |

**证据层：ViewAllChar 构造相关 → C−→偏 C；Notice/Loading / signboard 木牌 → 仍 B/A（资源在 WZ，`Map/Obj/login.img/...`，exe 几乎无明文路径）。**

### 1.4 CharSelect / charInfo / Ranking

| 步骤 | 状态 | 产物 |
| --- | --- | --- |
| REA search `charInfo` / `CharSelect` / `Ranking` | 成功；REA regex 多空 | `rea_search_*.json` |
| IDA `charInfo` | 命中 `Etc/MakeCharInfo.img/OrientChar*`（创角方向，非选角羊皮） | `ida_find_regex_charInfo.json` |
| decompile `0x5FBA85` | **成功** | `rea_decompile_MakeCharInfo_5FBA85.txt` |
| `CUICharSelect` / `Ranking` / 选角 `charInfo` 面板 | **未找到明文 EA** | 待重试（UTF-16 / 动态拼接 / 更大 CUI*） |

**证据层：MakeCharInfo（创角）→ C−；选角 charInfo 羊皮 + Ranking → 仍待升。**

---

## 2. 升层总表（相对 r17）

| Wave0 项 | r17 前 | 本轮后 | 说明 |
| --- | --- | --- | --- |
| StatusBar CreateCtrl | C− | **C** | 全文伪代码 + 坐标表落盘 |
| StatusBar 按钮/QS 点击 | C− | **C−** → **r18b: C** | OnButton=`sub_8D423D`；见 §7 |
| World DrawWorldItems | C−/样板 | **C** | 频道 id 基址 1000、空槽禁用、无 `<2` 过滤 |
| BtGoWorld / 进世界 | B | B → **r18b: C−** | 无 BtGoWorld 串；频道 OnButton→`5F8078` 发包 |
| Login Notice/Loading/signboard | B | B | 无 ASCII；ViewAllChar 有锚点 |
| ViewAllChar | — | **C−** | `sub_6075DD` 已反编译 |
| CharSelect charInfo/Ranking | B | B → **r18b: C** | `sub_602ADC` 坐标 + 排行 2976 |
| MakeCharInfo | — | **C−** | `sub_5FBA85` |

---

## 3. 对 W0 实现的具体对照要点

### StatusBar

- 控件 id / 坐标以上表为准；实现里若仍写 2004–2006，应改为 **1005 / 1006 / 1009**。  
- 四钮：1000 Shop / 1001 NPT / 1002 Menu / 1007 Short。  
- QS 点击：REA **尚未**给出 hit→keymap 分发函数；实现侧仍按 r15 方案（槽 hit → `keyboard` 映射 → 与 `UIStateGame` 同套分发）推进，但 **OnButton EA 待下一小步**。  
- 勿再整函数死等 `rea decompile 0x8D01B2`；固定流程：REA 失败 → curl MCP output。

### WorldSelect / Channel

- 频道按钮 id 从 **1000** 递增；空频道槽仍创建后禁用。  
- **不要**在 DrawWorldItems 语义里加 `channel_count < 2` 隐藏世界；若端上有该过滤，应对照**填世界列表的上层**，而不是本函数。  
- `BtGoWorld` 字符串不在 exe → 用 WZ 节点名 + 现有 `UIWorldSelect` 枚举对照，另开小步找点击 handler。

### Login / CharSelect

- 木牌 / Loading 框路径在 **Map Obj / Login.img WZ**，不要指望 exe ASCII。  
- 已有可跟的 Login 链锚点：`sub_6075DD`（ViewAllChar）、`sub_5FBA85`（MakeCharInfo 创角）。  
- 选角头顶 charInfo + Ranking：本轮无串；续跑优先 xref `UI/Login.img` 其它子路径或 UTF-16 全量扫。

---

## 4. IDA 不可用时的续跑清单

若 13338 不通：保持 IDA 打开 `BeiDou.exe`（或对应 `.i64`），MCP 监听 **13338**，然后按序执行（**禁止** `rea analyze`；单步 >90s 则记待重试并跳过）：

```powershell
$env:REA_ANALYSIS_PROVIDER='ida'
$env:REA_IDA_MCP_CONFIG='C:\Users\11036\.rea\ida-mcp.json'
$exe='E:\MXD\BeiDou-ClientV17.7\BeiDou.exe'
$rea='D:\software\nodejs-24\rea.cmd'

# StatusBar OnButton 候选（待重试）
& $rea decompile $exe 0x8D6409 --provider ida --json
& $rea decompile $exe 0x8D850B --provider ida --json
& $rea decompile $exe 0x8DBB93 --provider ida --json

# World：BtGo / 进世界（短关键字）
& $rea search $exe 'WorldSelect' --kind strings --provider ida --json
& $rea search $exe 'BtWorld' --kind strings --provider ida --json

# Login / CharSelect
& $rea search $exe 'ViewAllChar' --kind strings --provider ida --json
& $rea decompile $exe 0x6075DD --provider ida --json   # 若需重取
& $rea search $exe 'Login.img' --kind strings --provider ida --json

# 大函数截断恢复示例
# curl -o evidence/r18/ida_mcp_output_<tag>.json http://127.0.0.1:13338/output/<uuid>.json
```

---

## 5. 进度一行账

| 时间 | 项 | 结果 |
| --- | --- | --- |
| r18 | 探活 13338 | OK |
| r18 | StatusBar CreateCtrl 全文 | OK → **C** |
| r18 | StatusBar OnButton | 待重试 → **r18b 升 C** |
| r18 | DrawWorldItems | OK → **C** |
| r18 | Channel/BtGoWorld search | OK（无 UI 明文）→ **r18b 升 C−（频道点击即发包）** |
| r18 | Login Notice/signboard | 无明文；ViewAllChar 升层 |
| r18 | charInfo/Ranking/CharSelect | MakeCharInfo 升；选角面板待重试 → **r18b charInfo/Ranking 升 C** |
| r18b | 四项待重试小步 REA | 见 §7 |

---

## 6. 与用户问题的直接回答

> 「超时就一点点 rea 补充也不行？」

**可以。** 本轮证明：单 search / 单 decompile + 短暂停 + 大函数 curl 恢复，足以把 Wave0 若干项从 C− 推到 C，而无需整包 analyze。

---

## 7. 续跑 r18b（用户选 A · 四项待重试）

> 日期：2026-10-22（同日续跑）  
> 证据：[evidence/r18/r18b/](evidence/r18/r18b/)  
> 方法：单步 `rea`（遇 `unreadable_output` 从 partial / MCP `/output/<uuid>` 取回）+ curl IDA MCP `decompile` / `find_bytes` / `xrefs_to` / `get_int`。**禁止**整包 analyze。

### 7.1 逐步结果

| # | 目标 | 动作 | 状态 | 关键产物 |
| --- | --- | --- | --- | --- |
| 1 | StatusBar 候选 `0x8D6409` | REA decompile | 成功（排除） | `rea_decompile_8D6409.txt` — tooltip，非 OnButton |
| 2 | StatusBar 候选 `0x8D850B` | MCP + curl output | 成功（排除） | `ida_mcp_output_StatusBar_8D850B.json` — 聊天绘制 |
| 3 | StatusBar 候选 `0x8DBB93` | MCP + curl | 成功（排除） | `rea_decompile_StatusBar_8DBB93.txt` — 无 1000.. 分发 |
| 4 | 关键字 `QuickSlot`/`BtGoWorld`/`Ranking` | REA/MCP find_regex | n=0 明文 | 改走 vtable / GBK / cmp |
| 5 | `push 1005` / `cmp eax,1000` | `find_bytes` | 成功 | `ida_find_bytes_push_1005.json` 等 |
| 6 | StatusBar vtable @`0xb3ba8c` | `get_int` + xrefs | **升层** | +0x10=`sub_8D419C` OnChildNotify；+0x14=`sub_8D423D` **OnButton** |
| 7 | OnButton / QS | decompile `0x8D423D` / `0x8D419C` / `0xA04F7A` / `0x8DFE21` | **成功 → C** | `rea_decompile_cand_8D423D.txt` 等 |
| 8 | WorldSelect vtable @`0xaf7a9c` | `get_int` + decompile | **成功 → C−** | +0x14=`sub_624AE8`；进世界=`sub_5F8078` |
| 9 | charInfo Draw | decompile `0x602ADC`（客户端注释已锚） | **成功 → C** | `rea_decompile_CharDetail_602ADC.txt` |
| 10 | Ranking「排行不可用」 | 同函数 else 支 | **成功 → C** | 字串表 **2976** @(36,99)；有排行时 2977/2979 |

### 7.2 升层总表（相对 r18）

| Wave0 项 | r18 后 | r18b 后 | 说明 |
| --- | ---: | ---: | --- |
| StatusBar OnButton / QS | C− | **C** | EA 与 id→行为表齐 |
| BtGoWorld / 进世界 | B | **C−** | 无独立 BtGoWorld；频道点击即发包 |
| 选角 charInfo 属性板 | B | **C** | `sub_602ADC` 尺寸/坐标 |
| Ranking / 排行不可用 | B | **C** | `this[57]` 条件 + 字串 2976 |

### 7.3 对 W0 实现的可执行对照点

#### StatusBar · 快捷格 → S/E/I/K（及底栏钮）

**分发链：**

1. `sub_8D419C`（OnChildNotify）：`a3==100`（点击）时调 vtable OnButton；对 **2000..2003** 另直调 `sub_8DFE21(slot, 0)`（`a3==0` 时该函数早退，真正开窗在 OnButton）。
2. `sub_8D423D`（OnButton）开关：

| 控件 id | 十六进制 | 行为 |
| ---: | ---: | --- |
| 1000 | 0x3E8 | `sub_A04DCA(0)` — Shop/CashShop |
| 1001 | 0x3E9 | `sub_A12522` — BtNPT |
| 1002 | 0x3EA | `sub_A06735` — BtMenu |
| 1003 | 0x3EB | `sub_8D4A6D(1)` |
| 1004 | 0x3EC | `sub_8D4A6D(3)` |
| 1005 | 0x3ED | `sub_A04F7A(5, -1)` — **KeyConfig** |
| 1006 | 0x3EE | `sub_8DF4B1` — QuickSlot 折叠/展开 |
| 1007 | 0x3EF | `sub_A06CBC` — BtShort |
| 1009 | 0x3F1 | `sub_8D4A22` — QuickSlotD/聊天侧 |
| **2000** | | `sub_A04F7A(1,-1)` + `sub_8DFE21(0,0)` → **Equip** |
| **2001** | | `sub_A04F7A(0,-1)` + `sub_8DFE21(1,0)` → **Inven** |
| **2002** | | `sub_A04F7A(2,-1)` + `sub_8DFE21(2,0)` → **Stat** |
| **2003** | | `sub_A04F7A(3,-1)` + `sub_8DFE21(3,0)` → **Skill** |

`sub_A04F7A(type, -1)`：`type` 0/1/2/3/5 分别 toggle Inven/Equip/Stat/Skill/KeyConfig（开着则 `A0631B` 关，否则 `A05AB8` 开）。

**实现对照：** 底栏四键图标 id **2000–2003** 对应 **E→I→S→K**（不是按字母 S-E-I-K 顺序）；快捷格槽点击与键图标共用 OnButton/`A04F7A` 体系；勿再猜 2004–2006。

#### WorldSelect · GoWorld 包

- exe **无** `BtGoWorld` ASCII；独立 Go 钮 **未**在 WorldSelect OnButton 出现。
- 频道钮 id = `1000 + channelIndex`（与 DrawWorldItems 一致）。
- OnButton `sub_624AE8`：校验 `sub_62540A` → 选中频道 `this+34` → `sub_62536C` 高亮 → **`sub_5F8078(channelIndex)`**。
- **`sub_5F8078`（进世界/要角色列表）**：
  1. 若 `this+92!=0`（已在请求）则直接返回；
  2. `sub_6EC9CE(6)` 开出发包（`sub_6ECAA9(6)` 写 opcode）；
  3. `sub_427F74(*(WORD*)(channelIndex*32 + *(this+99)))` 写入频道相关 WORD；
  4. `this+92 = 1`；`sub_428CF1` 发送。

**实现对照：** 当前端 `BtGoWorld` → `CharlistRequestPacket(world, channel)`（opcode **5**）是两段确认；BeiDou 证据是 **点频道即发包**，opcode 形参为 **6**。W0 对齐时需二选一或做版本分支：跟端（双击/Go 钮 + opcode 5）还是跟 exe（频道点击 + opcode 6）。世界/频道 WORD 布局以 `this+99` 表项为准（待需要时可再拆字段）。

#### 选角 charInfo 坐标

`sub_602ADC`（CUICharDetail::Draw）：

| 项 | 值 |
| --- | --- |
| 面板宽 | **183** |
| 面板高 | `*(this+57)!=0` → **148**，否则 **112**（无排行区更矮） |
| Job | `(46, v188)`，有标题串时 y 来自字串 1322 |
| Level | **(46, 19)** |
| Fame | **(136, 19)** |
| STR / INT | **(46, 37)** / **(136, 37)** |
| DEX / LUK | **(46, 55)** / **(136, 55)** |

与 `UICharSelect.cpp` 中 `CS_DETAIL_*` 常量一致 → 实现可保持现坐标。

#### Ranking /「排行不可用」

同在 `sub_602ADC`：

- **有排行**（`this[57] != 0`）：字串表 **2977** 格式化名次；涨跌文案 **2979** + `L"up"` / `L"same"` /（负向走 `off_AF6EA0`）；绘制约 `(115-w, 99)` 与 `(165-w, 99)`。
- **无排行**（else）：字串表 **2976** 画在 **(36, 99)** → 即「排行不可用」类文案（exe 无该 GBK 明文，靠表 id）。

**实现对照：** 用角色是否带 rank 数据门闩（对齐 `this+57`）决定画排行条还是 2976 占位；坐标 y=99 落在 148 高面板的排行带。

### 7.4 仍待 / 缺口

| 缺口 | 说明 |
| --- | --- |
| opcode 5 vs 6 | 客户端 `CHARLIST_REQUEST=5`；exe `6EC9CE(6)`。需对照服端/实包再定 |
| 独立 BtGoWorld UI | 本轮未找到；若 WZ 有 Go 钮，可能在其它 CUI 或仅资源未绑 |
| 字串 2976 原文 | 未从字串表抽出 GBK 正文；语义由分支位置推定 |
| `A04F7A` 与键位槽绑定 | 开窗 type 已清；快捷栏「键位→技能/消耗」若另有 keymap 表，可再 xref `A04F7A` 调用方 |

### 7.5 r18b 续跑命令备忘

```powershell
$env:REA_ANALYSIS_PROVIDER='ida'
$env:REA_IDA_MCP_CONFIG='C:\Users\11036\.rea\ida-mcp.json'
# 大函数：REA unreadable → curl http://127.0.0.1:13338/output/<uuid>.json
# 直调 MCP 见 evidence/r18/r18b/_mcp_helper.py
```
