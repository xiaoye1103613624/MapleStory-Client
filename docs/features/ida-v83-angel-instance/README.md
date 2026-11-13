# IDA 实例 13339（`Angel.exe`）版本判定与反编译详细度对比

> 记录时间：2026-11-08（Asia/Shanghai）
> 取证方式：IDA Pro 9.4 GUI MCP 实例（Streamable-HTTP）只读查询：`server_health` /
> `survey_binary` / `py_eval`（IDAPython） / Windows 进程命令行 / 目录与文件元数据。
> 结论分级：**观察**（工具直接返回）/ **推断**（由多项观察交叉推出）/ **未知**（未取证）。

---

## 0. 一句话结论

**13339 是 083 / v83（GMS 系）客户端，不是 095。** 它不是「更高版本」，而是
**同一代 v83 主程序的另一份带符号 IDB**：IDA 里能看到 3122 个 MSVC 修饰名、
361 个业务类、2612 个类方法名，而 13337 的 `BeiDou.exe` 只有 50 个修饰名、
10 个类（全是 CRT/Concurrency 运行时），业务函数一律 `sub_*`。
所以**反编译可读性显著更详细**，但**不是功能更多、也不是版本更新**。

⚠️ 同时纠正一个会直接误导后续工作的既有记录：
**`docs/features/port-095/README.md:265-272` 中「13339 = 095 客户端 / `开启游戏.vmp.exe`」已经过时。**
端口被复用了，实例已换成 v83 的 `Angel.i64`。

---

## 1. 实例身份（`[MCP] Registered instance` 日志的来源）

日志行：

```
[MCP] Registered instance: `Angel.exe (pid=29620, port=13339)
```

`C:\Users\11036\.rea\ida-mcp-env\Lib\site-packages\ida_pro_mcp\ida_mcp.py:317-327`：

```python
binary = ida_nalt.get_root_filename() or ""
idb_path = idc.get_idb_path() or ""
register_instance(host, port, pid=os.getpid(), binary=binary, idb_path=idb_path)
print(f"[MCP] Registered instance: {binary} (pid={os.getpid()}, port={port})")
```

→ **观察**：日志里的 `Angel.exe` 是 IDB 的 `root filename`（IDA 记录的原始输入文件名），
**不是磁盘上真实存在的 exe 文件名**。全盘 `find /e -iname "Angel*.exe"` **0 命中**，
磁盘上只有 IDB，没有 `Angel.exe` 本体。文件名前缀的反引号 `` ` `` 是 IDA
对旧版/特殊输入名的转义残留。

### 端口 ↔ 进程 ↔ IDB（2026-11-08 实测）

| 端口 | PID | 进程命令行 | IDB / 输入 |
|---|---|---|---|
| 13337 | 31332 | `"D:\software\IDA Professional 9.4\ida.exe"`（无参数） | `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64` |
| 13338 | 50432 | `idalib-mcp.exe --host 127.0.0.1 --port 13338 --verbose E:\MXD\Maplestory95.i64` | headless 直挂 `Maplestory95.i64`（095 替代证据源） |
| **13339** | **29620** | `"D:\software\IDA Professional 9.4\ida.exe" "E:\MXD\反编译IDA\v83\`Angel.idb"` | **`E:\MXD\反编译IDA\v83\`Angel.i64`** |

- **观察**：`13339` 的 `server_health` 返回 `idb_path = E:\MXD\反编译IDA\v83\`Angel.i64`、
  `module = \`Angel.exe`、`input_path = C:\Users\Admin\Desktop\v83 Server\`Angel.exe`。
- **观察**：`E:\MXD\反编译IDA\v83\` 目录里 `` `Angel.idb `` 为 **33,037,843 B，日期 2020-09-02**，
  同目录的 `.i64/.id0/.id1/.id2/.nam/.til` 都是 2026-11-08 00:33–01:01 生成 →
  **一份 2020 年遗留的 32 位 IDB，今天升级成 i64 并重新挂到 GUI**。
- **观察**：`idb_path` 与 `input_path` 都带 `v83`。

### 与 095 记录的关系（重要）

`docs/features/port-095/README.md:265-272`（2026-11-01）记的是：
13339 → pid 32644 → `浩浩岛095(9.1测试端)\开启游戏.vmp.exe.i64`（加壳 230 函数）；
13338 → pid 32604 adopt 两个 GUI。

**推断**：13339 端口被复用，095 GUI 实例已关闭，换成 v83 IDB；13338 也从
「adopt GUI」变成 headless 直挂 `Maplestory95.i64`。
**这些旧行号只作历史，不再作为当前实例的依据。**
`docs/features/port-095/notes/ida-095-initial.md:14-20` 关于「国产 095 客户端均加壳、
静态不可反编译」的结论仍然成立，但**不适用于当前的 13339**。

---

## 2. 版本判定：083 / v83（GMS 系）

| 证据 | 观察值 | 判定 |
|---|---|---|
| IDB 目录 | `E:\MXD\反编译IDA\v83\` | 观察 → v83 |
| 原始输入路径 | `C:\Users\Admin\Desktop\v83 Server\`Angel.exe` | 观察 → v83 整合端 |
| 符号体系 | `COutPacket` / `CInPacket` / `CWvsContext` / `CField` / `CStage` / `IWz*` COM / `ZXString<>` / `StringPool` / `CActionMan` / `CAESCipher` | 观察 → 经典 v83 客户端类体系 |
| 版本串（两端**几乎完全一致**） | `MapleStory_Global:%s`、`patch.mapleglobal.com`、`http://maplestory.nexon.net`、`ver.%d`、`The MapleStory Trading System is not available for the Guest ID Users.`、`Nexon.MapleStory.WebBrowserUI`、`CNMGetVersionFunc` | 观察 → **同一 GMS 分支** |
| `CLogin`/`CWorldSelect`/`CUICharSelect`/`CUILoginStart`/`CUIChannelSelect`/`CUIChannelShift` | Angel 有具名类 | 观察 → v83 登录链路 |
| `CPinCodeDlg`、`CUICharacterSaleDlg`、`CUIRecommendWorld`、`CUIAntiMacroCharacterName` | Angel 有具名类 | 观察 → v83 后期（GMS v0.83 附近）特征 |
| 无 `ExpeditionIntermediary`、无 095 线 Business-class | Angel 类索引里 0 命中 | 观察 → 排除 095 |

**结论（推断，置信高）**：13339 = **GMS/KMS 083 / v83 系客户端**，与 13337
的 `BeiDou.exe` **同代、同分支、同源主程序**（版本串逐字相同）。
它来自 `v83 Server` 整合包（俗称「Angel / 天使端」），是**另一份 v83 exe**，
不是 BeiDou 那个打包版。

> 运行时记忆约束仍然生效：**原版功能判定只以 083 系证据为准，禁止引
> `Maplestory95.i64` / 095 线符号作依据**。Angel 属 083 线，可作证据源，
> 但跨端差异须甄别（见 §5）。

---

## 3. 两端元数据硬对比

| 指标 | 13339 `Angel.exe` (v83) | 13337 `BeiDou.exe` (v83) | 差值 |
|---|---|---|---|
| `imagebase` | `0x400000` | `0x400000` | 同 |
| `image_size` | `0xa95000` | `0xa94000` | 差 `0x1000`（4 KB） |
| PE 架构 | 32-bit | 32-bit | 同 |
| 函数总数 | **55,016** | **55,655** | 差 639（1.2%） |
| `survey_binary.named_functions` | **3,338** | **510** | **6.5×** |
| `py_eval.real_named`（去掉 `sub_*`/`loc_*`） | **3,625** | **754** | **4.8×** |
| MSVC 修饰名（`?` 开头） | **3,122** | **50** | **62×** |
| 不同业务类（自解析修饰名） | **361** | **10**（全 CRT/Concurrency） | — |
| 具名类方法总数 | **2,612** | **0 个业务方法** | — |
| 其中 `CUI*` 前缀命中 | **446** | **0** | — |
| 结构体 | 123 | 134 | 接近 |
| 字符串缓存 | **1,281** | **3,889** | **Angel 反而少 3×** |
| `input_md5` | `cbe5ccba04cbe2760d8eabc537926bd1` | `44c1030ecd8484e849c83892eb8fa025` | 不同打包 |
| 段名 | `___` / `.idata` / `___` / `.idata__` / `________` / `.mackt` / `seg006` | `.text` / `.idata` / `.rdata` / `.data` / `.rsrc` / `.export` / `.import` | Angel 有 `.mackt` + 乱码段名 |

**关键观察**：函数总数、imagebase、image size、版本串四项逐字对齐 →
**同一份 v83 主程序的不同打包/符号状态**，不是「两个不同版本的客户端」。

---

## 4. 「反编译内容是不是更详细」—— 是，但是**符号详细**，不是**功能详细**

### 4.1 质的差别（决定性）

13337 `BeiDou.exe` 的 754 个具名几乎全是 CRT / 库 / 导入：

```
_DllMain@12, ?seekpos@?$fpos@H@std@@QBE_JXZ,
?GetNumExternalThreads@SchedulerProxy@details@Concurrency@@QAEIXZ,
DialogFunc, TopLevelExceptionFilter, _WinMain@16, ZtlTaskMemAllocImp,
InterlockedDecrement, CreateFileA, LoadLibraryExA, GetTempFileNameA,
MultiByteToWideChar, IsDBCSLeadByte, GetVolumeInformationA, FindFirstFileA
```

项目文档多处印证：`docs/features/beidou-v17-parity/notes/r05-ui-layout.md:99`
「**无 CUI\* 符号，函数均 sub_\***」。

13339 `Angel` 的 3,122 个修饰名 demangle 后是**业务类方法**：

```
CActionFrame::LoadMappers / ExtractMap / MergeGroup / UpdateMBR / UpdateVisiblity / Draw
COutPacket::Encode1 / Encode2 / Encode4 / EncodeBuffer / EncodeStr
CInPacket::Decode1 / Decode2 / Decode4 / DecodeBuffer / AppendBuffer
CActionMan::Init / LoadCharacterAction / LoadFaceLook / LoadMobAction / LoadNpcAction
           / LoadPetAction / LoadSummonedAction / LoadTamingMobAction
           / GetSummonedProp / MergeCharacterSprite / CreateAfterimageLayer
CWvsContext::GetCharacterData / GetGuildName / GetOnlineFriendID / ClearFieldUI
           / ClosePartySearch_Remocon / CloseShopScanner / LoadCommodity
CCashShop::Init / OnBuyAvatar / OnBuyPackage / OnGiftMateInfoResult
           / OnChangedCategory / OnChangedPage / IsUsableItemGift
CUIStatusBar::ChatLogDraw / ChangeChatWndSize / ChangeWhisperTarget（41 个方法）
CUIMessenger::AddChatText / Draw / DrawStatusBar（26 个方法）
CUIToolTip::DrawItemIcon / AddInfoEx / ClearToolTip（26 个方法）
CQuestMan::CheckStartDemand / CheckCompleteDemand / GetQuestByNpc
CPersonalShopDlg::BuyItem / DeliverBlackList / MoveItemToInventory
CScriptMan::OnAskAvatar / OnAskBoxText / OnAskMembershopAvatar / OnAskMenu
CField_Wedding::OnWeddingProgress / OnWeddingCeremonyEnd / SetBlessEffect
CUIMonsterCarnival::DrawCPTooltip / FuncKeyProcess / OnButtonClicked
```

完整 361 类 / 2612 方法索引：
[`evidence/angel-idb-msvc-class-index.json`](evidence/angel-idb-msvc-class-index.json)。

→ **观察 + 推断**：在 Angel 上反编译 `0x40xxxx` 起的游戏函数，Hex-Rays 伪代码里
直接出现 `CWvsContext::GetCharacterData()`、`CCashShop::OnBuyPackage()` 这种
带类名的方法调用，以及 `CUIFamilyChart::_DrawChartItem` 这类字段级语义；
在 BeiDou 上同一位置是 `sub_XXXXXX` + 无名全局变量。**可读性差距是量级性的。**

### 4.2 但不是「版本更新 / 功能更多」

- 函数总数 55,016 vs 55,655（仅差 1.2%）、image size 差 4 KB、版本串逐字相同。
- 两者都是 v83；Angel 没有 095 的 `ExpeditionIntermediary` 之类新系统。
- **Angel 字符串缓存只有 1,281 条，BeiDou 3,889 条** → Angel 的字符串分析
  **不完整**（`strings_cache_ready` 初始 false，后补跑才 ready）。做字符串锚点
  取证时**仍以 13337 为准**，Angel 只补「符号/类名/调用关系」。
- Angel 段名含 `.mackt` 且乱码 → 有加壳/脱壳残留；但 55k 函数已完整分析，
  `auto_analysis_ready = true` / `hexrays_ready = true`，可正常 decompile。

### 4.3 对当前还原工作的实际价值（最重要的部分）

项目此前所有 parity 审计都受限于「BeiDou 无 CUI\* 符号」，只能靠字符串 +
xref + 控件表反推（见 `docs/features/beidou-v17-parity/notes/r16-rea-coverage-gap.md`
的三层证据 A/B/C）。Angel IDB **恰好补上了最缺的那一层**：

| 此前在 BeiDou 上取证困难的功能 | Angel IDB 里可直接定位的类 | 方法数 |
|---|---|---|
| Messenger | `CUIMessenger` | 26 |
| 家族 / 学院谱系 | `CUIFamily` + `CUIFamilyChart` | 3 + 8 |
| 组队搜索 | `CUIPartySearch`（`StartSearch`）+ `CWvsContext::ClosePartySearch_Remocon` | 1 + 1 |
| 现金商店礼物/礼包 | `CCashShop` | 66 |
| 怪物嘉年华 | `CUIMonsterCarnival` + `CField_MonsterCarnival` | 16 + 9 |
| 婚礼流程 | `CField_Wedding` | 4 |
| 个人商店 / 雇佣商店 | `CPersonalShopDlg` / `CAdminShopDlg` | 10 / 14 |
| 商店扫描 | `CUIShopScanResult` + `CWvsContext::CloseShopScanner` | 2 + 1 |
| 好友分组 / 屏蔽 | `CTabFriend` / `CFriend` / `CUIBlockUser` | 12 / 6 / 3 |
| 公会 BBS | `CUIGuildBBS` | 17 |
| 宠物 | `CPet` / `CPetTemplate` / `CVecCtrlPet` / `CUIPetEquip` | 28 / 2 / 6 / 2 |
| 召唤兽 / 龙 | `CSummoned` / `CDragon` / `CVecCtrlSummoned` / `CVecCtrlDragon` | 16 / 10 / 6 / 2 |
| 任务交付 / 任务栏 | `CUIQuestDelivery` / `CUIQuestAlarm` / `CUIMedalQuestInfo` | 1 / 6 / 10 |
| 新年卡 / MapleTV | `CUINewYearCardSenderDlg` / `CMapleTVMan` / `CUIMapleTV` | 10 / 3 / 1 |
| 快捷栏 / 键盘设置 | `CUIKeyConfig` / `CUIShortCutMenu` / `CUIStatusBar` | 7 / 2 / 41 |
| 宏 / 技能扩展 | `CUIMacroSys` / `CUIMacroSysEx` / `CUISkillEx` | 8 / 8 / 3 |
| Tooltip | `CUIToolTip`（`DrawItemIcon` / `AddInfoEx`） | 26 |
| UtilDlgEx 分支 | `CUtilDlgEx` | 43 |

→ **推断**：这些正是 `r16` / `r40` 里大量标 ❌ / 停 A 层证据的功能。
有 Angel 之后，可以直接 `lookup_funcs` → `decompile` 拿控件表、包序、
按钮回调，不必再靠字符串猜。**但必须遵守 §5 的甄别规则。**

---

## 5. 使用约束（务必遵守）

1. **083 基准不变**：Angel 属 083 线，可作证据；`Maplestory95.i64`（13338）
   与 095 线符号/IDB 仍**禁止**用于原版功能判定。
2. **它是另一份 v83 整合端**：`input_path = C:\Users\Admin\Desktop\v83 Server\`Angel.exe`，
   有 `.mackt` 段。与 `BeiDou-ClientV17.7` 存在打包差异，跨端差异要甄别：
   - 控件 id / 坐标 / 包序：先在 Angel 定位，再回 13337 用字符串 xref 交叉验证；
   - 若两者冲突，以 **13337（当前还原基准）+ 实机 WZ/IMG** 为准，Angel 只作语义参考。
3. **字符串锚点仍用 13337**（3,889 条 vs Angel 1,281 条）。
4. **不要把「还有 5 万函数未命名」当进度指标**
   （`docs/features/beidou-v17-parity/notes/r16-rea-coverage-gap.md:235` 明确禁止）。
   同理，也不要把「Angel 有 361 个类」当成「还有 361 个功能要做」。
5. **端口会变**：`13339` 今天是 v83 Angel，明天可能被复用。每次取证先跑
   `server_health` 读 `idb_path` 确认，别信 README 里的旧端口表。

---

## 6. 复现命令

探针脚本（只读，全部落在 `tools/`）：

| 脚本 | 作用 |
|---|---|
| `tools/ida_instance_probe.py` | `server_health` + `survey_binary` + 函数/段/字符串统计 → `tools/_ida_probe_out2.json` |
| `tools/ida_symbol_compare.py` | 两端修饰名数量 / demangle 诊断 / 版本串 |
| `tools/ida_class_scan.py` | 自解析 MSVC 修饰名 → 类名分布 + 业务类命中 |
| `tools/ida_angel_class_index.py` | 13339 全量 361 类 / 2612 方法，IDA 侧直接写文件绕开 MCP 输出截断 |

```bash
cd /e/project/MapleStory-Client
PY="C:/Users/11036/.dsh/dsh-runtimes/dsh-primary-runtime/dependencies/python/python.exe"
PYTHONIOENCODING=utf-8 "$PY" tools/ida_angel_class_index.py
```

`py_eval` 注意（`C:\Users\11036\.rea\ida-mcp-env\Lib\site-packages\ida_pro_mcp\ida_mcp\api_python.py:116-198`）：
locals 跨调用持久化、AST 会把「多行 + 末尾表达式」切成 `exec` / `eval` 两个
不同 globals 作用域，循环里定义的变量会 `NameError`（本次踩了 `low` / `lt` 两次）。
**规避：把全部逻辑包进一个 `def _scan(): ...` 再 `\_scan()`，并传 `new_locals=True`。**

原始证据：

- [`evidence/ida-probe-health-survey.json`](evidence/ida-probe-health-survey.json)
- [`evidence/ida-class-scan.json`](evidence/ida-class-scan.json)
- [`evidence/ida-symbol-compare.json`](evidence/ida-symbol-compare.json)
- [`evidence/angel-idb-msvc-class-index.json`](evidence/angel-idb-msvc-class-index.json)

---

## 7. 未知 / 未取证

- `Angel.exe` 本体磁盘上不存在，**没有做 PE 头/节表/熵值的直接比对**
  （`.mackt` 只能从 IDB 段名观察）。
- 未把 Angel 的 55,016 个函数与 BeiDou 的 55,655 个做**地址级 1:1 映射**
  （判断「同一主程序」依据的是函数总数 + image size + imagebase + 版本串四项，
  置信高但非逐函数证明）。
- 未验证 Angel IDB 里 `CWvsContext` 的 196 个方法是否与当前 s9 协议
  opcode 一一对应；这需要后续逐功能 decompile 时做。
- 13338 的 `Maplestory95.i64` 本次只确认进程命令行，未查询其内部状态。
