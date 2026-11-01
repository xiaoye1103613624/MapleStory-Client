# 095 客户端 IDA/REA 首轮分析记录

> 执行时间：2026-11-01 02:12–02:20
> 执行方式：IDA MCP（ida-pro-mcp）JSON-RPC 直连
> - `13339`：IDA GUI 已打开的 `开启游戏.vmp.exe.i64`（浩浩岛095 9.1 测试端）
> - `13338`：idalib headless，用于打开 PLUS2 `MapleStory.exe.i64` 与 `E:\MXD\Maplestory95.i64`
> 产物：`docs/features/port-095/evidence/`（原始 JSON、段表、反编译片段）
> 性质：**只读分析**，未修改任何 IDB / exe / 代码

---

## 1. 结论摘要

| 目标 | 是否加壳 | IDA 静态可读性 | 结论 |
|---|---|---|---|
| 浩浩岛095 `开启游戏.vmp.exe`（11,745,280 B） | **是**（VMProtect 特征） | 230 函数 / 406 字符串，全部落在加密段内 | **静态反编译不可行** |
| 095PLUS2 `MapleStory.exe`（5,660,912 B） | **是**（未知壳/VMProtect 变体） | 3 函数 / 14 字符串 | **静态反编译不可行** |
| `E:\MXD\Maplestory95.i64`（2023 遗留 IDB，未加壳 GMS v95） | 否 | **22,428 函数 / 21,255 具名 / 2,583 字符串** | **可用作 095 客户端逻辑的替代证据源** |

**一句话**：两个国产 095 客户端都被壳保护，静态拿不到任何业务逻辑；但本机存在一份**已完整分析的 GMS v95 客户端 IDB**，其版本谱系与服务端一致（OdinMS 系 v95），可直接作为 opcode / 结构体 / 公式的取证来源，代价是需要甄别 GMS 与国产 095 的自定义差异。

---

## 2. 目标 A：浩浩岛095 `开启游戏.vmp.exe`

目录：`E:\MXD\【2023.8.8江浩生日福利】怀旧岛192整合版\浩浩岛095(9.1测试端)\`
（该目录**没有** `MapleStory.exe`，此 11.7 MB exe 即客户端主体）

### 2.1 基线

| 项 | 值 |
|---|---|
| 模块 | `开启游戏.vmp.exe` |
| 架构 / ImageBase | x86-32 / `0x400000` |
| ImageSize | `0x2051000` |
| MD5 | `1e1004876949a592c9849d9a9ec2b5cb` |
| SHA256 | `6d8016f3d37c83987378aef2aa740e679109dc409dd048df892f7f78ba14dd4b` |
| 函数总数 | 230（具名 192，未命名 38，库函数 0） |
| 字符串总数 | 406 |
| 调用图 | 仅 1 条边，根节点全是 `nullsub_*` |

### 2.2 PE 段表与熵（`evidence/pe-sections.txt`）

| 段名 | VA | VSize | RawSize | 熵 | 权限 |
|---|---|---|---|---|---|
| `<unnamed>` | 0x1000 | 0xc1d000 | **0x0** | 0.00 | RWX |
| `.vmps0` | 0xc1e000 | 0x20000 | **0x0** | 0.00 | R |
| `.pdata` | 0xc3e000 | 0x319000 | **0x0** | 0.00 | RW |
| `vsqwcbse` | 0xf57000 | 0x2a2000 | **0x0** | 0.00 | RWX |
| `pwyuzxvp` | 0x11f9000 | 0x1000 | **0x0** | 0.00 | RWX |
| `ednhbazl` | 0x11fa000 | 0x1000 | **0x0** | 0.00 | RWX |
| `.mackt` | 0x11fb000 | 0x2000 | **0x0** | 0.00 | RWX |
| `.mackt` | 0x11fd000 | 0x2000 | **0x0** | 0.00 | RWX |
| `.vmps1` | 0x11ff000 | 0x31d7e8 | **0x0** | 0.00 | RX |
| `.vmps2` | 0x151d000 | 0xb310d0 | **0xb31200** | **7.99** | RX |
| `.rsrc` | 0x204f000 | 0x14bb | 0x1600 | 3.20 | R |

判读：

- 前 9 段磁盘 RawSize 全为 0 → 运行时才由 loader 填充（VMProtect 的典型特征）。
- 全文件 11.7 MB 中，只有 `.vmps2` 一段有磁盘数据（0xb31200 字节，熵 7.99），即**全部机读代码被加密/压缩后放在 `.vmps2`**。
- `.vmps0` / `.vmps1` / `.vmps2` 三段命名 + `.mackt` + 随机名段（`vsqwcbse` / `pwyuzxvp` / `ednhbazl`）+ RWX → VMProtect。

### 2.3 IDAPython 复核（`py_eval` 实测）

| 段 | 采样字节 | 熵 | 段内函数数 |
|---|---|---|---|
| `___` … `.vmps1`（前 9 段） | 全 `0xFF`（未初始化） | 0.00 | 0 |
| `.vmps2` (0x191d000) | `f40ad8a5…` | 7.78 | 88 |
| `.vmps2` (0x19b0658) | `0000…` | 7.97 | 142 |

230 个函数**全部**落在 `.vmps2` 的加密数据里，反汇编结果是壳的 VM 字节码，不是 MFC/游戏逻辑。

### 2.4 导入表（383 项，`evidence/imports-vmp.txt`）

| 模块 | 数量 | 说明 |
|---|---|---|
| kernel32 | 256 | |
| user32 | 43 | |
| advapi32 | 15 | 注册表（保护壳常见） |
| ws2_32 | 13 | 网络 |
| mss32 | 12 | Miles 音频 |
| oleaut32 / ole32 | 12 | COM |
| gdi32 | 7 | |
| wininet | 6 | 自动更新 |
| **ijl15** | 4 | 冒险岛渲染 DLL |
| **nmcogame** | 4 | 反外挂（nProtect） |
| version / dinput8 / iphlpapi / netapi32 / psapi / winmm / WTSAPI32 | 各 1–3 | |
| **nexonguard** | 2 | 反外挂 |

导入表本身是**壳的 IAT**（注册表/线程/内存 API 占多数），游戏真实导入被运行时重建。

---

## 3. 目标 B：095PLUS2 `MapleStory.exe`

目录：`E:\MXD\【2021最新】095PLUS2_10.1国庆版\V095冒险岛PLUS2客户端\`

| 项 | 值 |
|---|---|
| 大小 | 5,660,912 B |
| MD5 / SHA256 | `54b835fdc9770b4c84f68110191b8bf8` / `36c9b98e717f1ba74663f3f1bd3279b0517d333b54e6b6d0273f2f9b1fec7a7c` |
| ImageSize | `0xd3c000` |
| 函数总数 | **3**（`start` + 2 个 sub） |
| 字符串总数 | **14** |
| 导入 | 仅 `kernel32!CreateDirectoryA` + `ZtlTaskMemAllocImp/FreeImp/ReallocImp` |

段表（`evidence/pe-sections.txt`）：

| 段名 | VSize | RawSize | 熵 | 权限 |
|---|---|---|---|---|
| `<unnamed>` | 0xa38000 | 0x3e0600 | **7.99** | RWX |
| `.rsrc` | 0x2d9d0 | 0xe600 | **7.98** | RW |
| `.idata` | 0x1000 | 0x200 | 0.77 | RW |
| `zlgmmwkb` | 0x13f000 | 0x13e200 | 7.93 | RWX |
| `kjnnsnqi` / `ntlpglpj` | 0x1000 | 0x200 / 0x1000 | 4.14 / 2.45 | RWX |

判读：入口只有一段跳板，主体（约 4 MB 代码）与**资源段 .rsrc 都被加密**（资源熵 7.98 说明连图标/清单都被压缩加密），随机名 RWX 段为壳的运行期区。同样是加壳客户端。

> 值得注意：`ZtlTaskMemAllocImp` 是冒险岛客户端的堆分配导出前缀，说明壳保留了少量原始导入符号做伪装；真正的 MFC/游戏导入全部运行时重建。

---

## 4. 目标 C：`E:\MXD\Maplestory95.i64`（关键发现）

### 4.1 身份

| 项 | 值 |
|---|---|
| 文件 | `E:\MXD\Maplestory95.i64`（42,343,783 B，2026-09-17，由 2023 年的 `.idb` 升级而来） |
| 原始输入路径（IDB 内记录） | `C:\Users\Admin\Desktop\Random Stuff\Other\v95\Maplestory95.exe` |
| 模块 | `Maplestory95.exe` |
| 架构 / ImageBase / ImageSize | x86-32 / `0x400000` / `0xcee000` |
| 函数 | **22,428**（具名 21,255，库函数 647，未命名 526） |
| 字符串 | **2,583** |
| 符号形态 | **完整 MSVC 修饰名**（含类/命名空间/模板/参数类型） |

这是**未加壳的 GMS v95 客户端**，且当年被完整分析过。相较两个国产 095 客户端的 230 / 3 个函数，信息量差 3 个数量级。

### 4.2 抽样确认它是冒险岛 v95 客户端

字符串（`evidence/ida-survey-v95gms.json`）：

```
UI/UIWindow2.img/Macro/num1/%d            (xref 8)
UI/UIWindow2.img/Skill/macro/Macroicon/%d/icon   (xref 9)
UI/UIWindow2.img/Maker/randomRecipe       (xref 4)
```

函数（按类分布，节选）：

```
?GetInstance@TSingleton<CWvsContext>       CWvsContext 单例，506 个相关符号
?GetBasicStat@CWvsContext                  BasicStat 访问器
?Decode@ALLIANCEDATA(ALLIANCEDATA, CInPacket&)   联盟结构解码（140 个 Alliance 符号）
?Decode@MedalAchievementInfo(…, CInPacket&)      成就（8 个）
CItemMakerInfo / CUIItemMaker                    制造（96 个）
MonsterBookCard / CMobPool                       怪物图鉴（161 个）
```

### 4.3 直接回答四个数值改造问题（**v95 客户端实测**）

反编译 `GW_CharacterStat::Decode`（完整源码见 `evidence/decompile-v95-GW_CharacterStat_Decode.c`）：

| 字段 | 线上宽度 | 客户端存储类型 | 依据 |
|---|---|---|---|
| `dwCharacterID` | 4 | DWORD | `Decode4` |
| `sCharacterName` | 13 | char[13] | `DecodeBuffer(13)` |
| `nGender` / `nSkin` | 1 / 1 | byte | `Decode1` |
| `nFace` / `nHair` | 4 / 4 | int | `Decode4` |
| **`nLevel`** | **1** | **`unsigned char`** | `Decode1` + `_ZtlSecureGet_nLevel@…QBIE`（E = unsigned char） |
| `nJob` | 2 | short | `Decode2` + 返回 `F` |
| `nSTR/DEX/INT/LUK` | 2×4 | short | `Decode2` |
| **`nHP` / `nMHP` / `nMP` / `nMMP`** | **4×4** | **`long`(int32)** | `Decode4` + `_ZtlSecureGet_nHP…QBIJ`（J = long） |
| `nAP` | 2 | short | `Decode2` |
| `nSP` | 2（或 ExtendSP） | short | `Decode2` / `ExtendSP::Decode` |
| **`nEXP`** | **4** | **`long`(int32)** | `Decode4` |
| `nPOP` | 2 | short | `Decode2` |
| **`nTempEXP`** | **4** | **`long`(int32)** | `Decode4` |
| `dwPosMap` | 4 | unsigned long | `Decode4` |
| `nPortal` | 1 | byte | `Decode1` |
| `nPlaytime` | 4 | int | `Decode4` |
| `nSubJob` | 2 | short | `Decode2` |

`GW_CharacterStat::DecodeMoney`（`evidence/decompile-v95-GW_CharacterStat_DecodeMoney.c`）：

```c
int v3 = CInPacket::Decode4(iPacket);
this->_ZtlSecureTear_nMoney_CS = _ZtlSecureTear<long>(v3, this->_ZtlSecureTear_nMoney);
```

→ **v95 金币 = 4 字节 int32，客户端存储 `long`(32 位)**，与当前 s9 一致，不存在"v95 已经是 64 位"的情况。

怪物血量：`CField::ShowMobHPTag(uint id, int v1, int v2, int v3, int v4)`（4 个 32 位参数）+ `MobStat::SetFrom` 全用 `_ZtlSecureFuse<long>` → **v95 怪物 HP 也是 int32**。

对改造方案的含义：

1. **金币 long 化、怪物 HP long 化、经验 long 化、等级 999** 在 v95 原生客户端里**都不支持**（分别是 32 位 / 32 位 / 32 位 / 8 位字段）。想要 999 级，客户端 `nLevel` 的 `unsigned char` 与 `Decode1` 必须一起改，不能只改服务端。
2. 这反向印证了既定决策"一次协议升级（服务端 + 当前 v83 客户端同步改），不兼容旧客户端"是对的——**没有任何现成 095 客户端能直接对接扩宽后的协议**。
3. 等级若保持 1 字节，上限 255；要 999 必须把 `nLevel` 改 2 字节，客户端所有 `Decode1`/`Encode1` 相关点、`char[13]` 名字之外的排序/比较/UI 全部要跟着走。

---

## 5. 差距与风险

| # | 风险 | 说明 | 缓解 |
|---|---|---|---|
| 1 | **GMS v95 ≠ 国产 095** | 国产端在 GMS v95 上加了自定义 UI（签到/兑换/排行）、自定义 opcode、String.wz 改动 | 结构体/公式/opcode 以 IDB 为基线，**自定义系统以 095 服务端脚本 + XML-WZ 为准**（已归档） |
| 2 | 两个国产 095 客户端仍无法静态分析 | 若必须拿到国产端的自定义协议，只能动态脱壳 | REA 有 Frida 动态插桩工具（`start_frida_session` / `instrument_with_frida` / `trace_feature`），需运行客户端且要过 nProtect/HShield/nexonguard |
| 3 | 国产客户端含三重反外挂 | `nexonguard.aes`、`nmcogame.dll`、`HShield/`、`Sddyn03.dll`、`v3hunt.dll` | 动态分析前需先确认可离线运行（单机端一般不联网校验） |
| 4 | 遗留 IDB 无校验和 | `Maplestory95.i64` 的 md5/sha256 显示 `unavailable`（IDB 未记录原 exe 哈希，原 exe 已不在本机） | 以版本串与符号集交叉验证；结论只用于**结构/公式**，不用于字节级比对 |
| 5 | opcode 表尚未提取 | 本轮只做了侦察与结构体取证 | 下一步在 IDB 上定位 `CClientSocket::ProcessPacket` / `CWvsContext::OnPacket` 的 switch 表，导出 0x00–0xFF 映射 |

---

## 6. 下一步（建议顺序）

1. **提取 v95 opcode 表**：在 `Maplestory95.i64` 上找 `CClientSocket::ProcessPacket`、`CWvsContext::OnPacket`、`CField::OnPacket` 的跳转表，产出 `notes/opcode-v95.md`（与当前 s9 的 0x00–0xFF 逐条对照）。
2. **导出可移植系统的结构与解码函数**：`ALLIANCEDATA::Decode`、`MedalAchievementInfo::Decode`、`MonsterBookCard`、`CItemMakerInfo`、`CWvsContext` 的 potential/character card/hyper rock 相关符号，产出 `notes/v95-decoders.md`。
3. **与 095 服务端 XML-WZ 对齐**：用服务端 `wz/` XML 树（String/Mob/Item/Skill/Map）比对字段，标识国产端自加项。
4. **可选：动态验证**（仅当需要国产端自定义协议时）：REA + Frida 附加 `开启游戏.vmp.exe`，在 `WS2_32!send/recv` 上挂钩抓明文，直接得到 opcode + 包体；不依赖脱壳。
5. 全部结论汇总进 `README.md`，再回到协议 v2 执行方案的最终定稿。

---

## 7. 复现方式（本机）

```bash
# 侦察（13339 = 095 vmp 客户端）
python E:/project/_ms_probe/mcp_call.py 13339 survey_binary -

# 段熵/函数分布（需要 py_eval，仅 13339 GUI 实例支持）
python E:/project/_ms_probe/mcp_call.py 13339 py_eval args_pyeval_seg.json

# 打开未加壳 v95 IDB（13338 headless）
#   args: {"input_path":"E:\\MXD\\Maplestory95.i64","mode":"force_headless"}
python E:/project/_ms_probe/mcp_call.py 13338 idb_open args_open_m95.json

# 反编译角色属性解码
#   args: {"addr":"?Decode@GW_CharacterStat@@QAEXAAVCInPacket@@H@Z","database":"<session>"}
python E:/project/_ms_probe/mcp_call.py 13338 decompile args_dec.json
```

> 说明：Qoder 当前会话的 MCP 工具目录在会话启动时固定，新注册的 `ida-095` / `ida-headless` / `rea-095` 需重载后才出现在工具列表；本记录全部通过 JSON-RPC 直连完成。
