# opcode-v95 证据包索引

> 证据源：`E:\MXD\Maplestory95.i64`（**未加壳 GMS v95 客户端**，22,428 函数 / 21,255 命名函数）。
> 工具：IDA Pro 9.4 + `ida-pro-mcp`，headless 端点 `http://127.0.0.1:13338/mcp`，数据库会话 `3c1162a2`（首轮为 `0dce3e26`，两者指向同一 `E:\MXD\Maplestory95.i64`；调用时以 `func_query` 返回的 session 为准）。
> 结论成品：[notes/opcode-v95.md](../../notes/opcode-v95.md)（934 行）+ [notes/opcode-map-v95-vs-s9.md](../../notes/opcode-map-v95-vs-s9.md)（833 行，v95↔s9 逐条映射）——本目录只放**原始证据**与**可复现脚本**。
>
> 免责：本目录仅用于个人学习与技术研究的本地单机环境取证，不含任何可直接用于联网运营的内容。

---

## 1. 为什么用 GMS v95 而不是 095 中文端

两个国产 095 客户端均已确认加壳，静态不可反编译：

| 目标 | 状态 |
|---|---|
| `开启游戏.vmp.exe`（浩浩岛 095 9.1 测试端） | `.vmps0/.vmps1/.vmps2` + `.mackt` + 随机名 RWX 段；230 个函数全部落在熵 7.78–7.97 的加密区 |
| `095PLUS2_10.1国庆版` 客户端 | `.rsrc` 熵 7.99，无有效代码段 |

因此本包导出的是 **GMS v95 的标准协议语义**（opcode 归属、二级分发、字段顺序），用于**语义参照**；
中文 095 端的自定义 opcode 差异仍须抓包校准（`REA + Frida` 挂 `WS2_32!send/recv`，见 [port-095 README §9](../../README.md) 阶段 B2/E）。

---

## 2. 文件清单

### 2.1 顶层分发器（反编译原文）

| 文件 | 内容 |
|---|---|
| `dec_processpacket.c` | `CClientSocket::ProcessPacket` @ `0x4b00f0` — uint16 读 opcode；16/17/18/19/20/23 连接层特例；`(v4-28) <= 0x70` 交 `CWvsContext::OnPacket`，其余交 `g_pStage` |
| `dec_cwvscontext_onpacket.c` | `CWvsContext::OnPacket` @ `0x9e5840` — 28–140 的 switch（106 个 case） |
| `dec_cfield_onpacket.c` | `CField::OnPacket` @ `0x546d59` — 141–432 的两段结构（先 147–381 直连 case，再 179–432 的嵌套区间链） |
| `dec_clock.json` | `CField::OnClock` 反编译 JSON —— 用于确认 **opcode 163** 的 vtable 虚调用指向（子 opcode 0/1/2/3，`CClock::SetClock` / `CClock::SetTimer`） |

### 2.2 二级分发器（33 份，`dec/`）

`dec/<mangled_name>.c`，命名即函数签名。分四组：

**A. 池（Pool）类 —— 只收一个包类型，内部再按子类型分发**

| 文件 | 承接的 opcode |
|---|---|
| `_OnPacket_CSummonedPool__QAEXJAAVCInPacket___Z.c` | 278–283 |
| `_OnPacket_CMobPool__QAEXJAAVCInPacket___Z.c` | 284–310 |
| `_OnPacket_CNpcPool__QAEXJAAVCInPacket___Z.c` | 311–318 |
| `_OnNpcTemplatePacket_CNpcPool__IAEXJAAVCInPacket___Z.c` | 311–318 中的 template 分支 |
| `_OnPacket_CEmployeePool__QAEXJAAVCInPacket___Z.c` | 319–321 |
| `_OnPacket_CDropPool__QAEXJAAVCInPacket___Z.c` | 322–324 |
| `_OnPacket_CMessageBoxPool__QAEXJAAVCInPacket___Z.c` | 325–327 |
| `_OnPacket_CAffectedAreaPool__QAEXJAAVCInPacket___Z.c` | 328–329 |
| `_OnPacket_CTownPortalPool__QAEXJAAVCInPacket___Z.c` | 330–331 |
| `_OnPacket_COpenGatePool__QAEXJAAVCInPacket___Z.c` | 332–333 |
| `_OnPacket_CReactorPool__QAEXJAAVCInPacket___Z.c` | 334–337 |

**B. 用户类 —— CUserPool 三级分发（remote / local / common）**

| 文件 | 内容 |
|---|---|
| `_OnPacket_CUserPool__QAEXJAAVCInPacket___Z.c` | `CUserPool::OnPacket`：179 进图 / 180 离图；**181–209** → common，**210–230** → remote，**231–276** → local |
| `_OnUserRemotePacket_CUserPool__IAEXJAAVCInPacket___Z.c` | 远程角色（移动、动作、伤害、效果等） |
| `_OnUserLocalPacket_CUserPool__IAEXJAAVCInPacket___Z.c` | 本地角色（受击、死亡、复活、buff 等） |
| `_OnUserCommonPacket_CUserPool__IAEXJAAVCInPacket___Z.c` | 181/182 聊天（区分地图内外）、183–195、197；198–205 `OnPetPacket`、206–208 `OnDragonPacket`；196 / 209 无处理 |
| `_OnPacket_CUserLocal__QAEXJAAVCInPacket___Z.c` | 单角色实例级 |

**C. 对话框 / 单例 —— 自带完整子 switch**

| 文件 | opcode |
|---|---|
| `_OnPacket_CShopDlg__SAXJAAVCInPacket___Z.c` | 364–365 |
| `_OnPacket_CAdminShopDlg__SAXJAAVCInPacket___Z.c` | 366–367 |
| `_OnPacket_CStoreBankDlg__SAXJAAVCInPacket___Z.c` | 369–370 |
| `_OnPacket_CTrunkDlg__*` | 368 |
| `_OnPacket_CParcelDlg__*` | 381 |
| `_OnPacket_CRPSGameDlg__*` | 371 |
| `_OnPacket_CUIMessenger__*` | 372 |
| `_OnPacketBase_CMiniRoomBaseDlg__SAXAAVCInPacket___Z.c` | 373 |
| `_OnPacket_CScriptMan__QAEXJAAVCInPacket___Z.c` | 363 |
| `_OnPacket_CFuncKeyMappedMan__QAEXJAAVCInPacket___Z.c` | 398–400 |
| `_OnPacket_CBattleRecordMan__QAEXJAAVCInPacket___Z.c` | 420–423（420、423 无处理） |
| `_OnPacket_CMapleTVMan__QAEXJAAVCInPacket___Z.c` | 404–409 |

**D. 阶段 / 商店 / 登录**

| 文件 | 内容 |
|---|---|
| `_OnPacket_CCashShop__UAEXJAAVCInPacket___Z.c` | 382–396（**仅现金商店 stage**；游戏中不走这里）；392/393 共用 `OnCashItemGachaponResult`，389/394 无处理 |
| `_OnPacket_CMapLoadable__UAEXJAAVCInPacket___Z.c` | 144–146 |
| `_OnPacket_CLogin__UAEXJAAVCInPacket___Z.c` | 0–15、21–22、24–27 |

> **注**：`dec/` 里的 `CTrunkDlg` / `CParcelDlg` / `CRPSGameDlg` 子 switch 会被 `dec_maps.json` 的自动解析误读（例如把 `CCtrlScrollBar::SetCurPos` 当成 handler）。
> `notes/opcode-v95.md` §4 的表是**逐个读源后手工校订**的结果，以文档为准，不要用 `dec_maps.json` 的原始解析。

### 2.3 结构化映射（机器可读）

| 文件 | 结构 | 说明 |
|---|---|---|
| `cfield_map.json` | `[{lo, hi, handler}]` × 69 | 从 `dec_cfield_onpacket.c` 的区间链还原出的「区间 → 二级分发器」表 |
| `dec_maps.json` | `{文件名: 解析结果}` × 33 | 每个二级分发器的 case 抽取（**未经人工校订**，仅供复查） |
| `send_ops.json` | `{opcode: [调用函数地址]}` × 238 | 发送 opcode → 构造 `COutPacket(nType)` 的函数地址（582 处构造点全归因） |
| `send_sites_all.json` | `[{fn, name, op, how, src}]` × 582 | 逐构造点明细：`how` 记录 opcode 的取得方式（`literal` / `assign` / `lobyte` / `shared with <addr>`），可直接复核每个数字 |
| `send_callers.json` | `{地址: mangled 名}` × 462 | 上述地址 → 函数名 |
| `ctor_xrefs_all.json` | 462 个函数的 xref 汇总 | `??0COutPacket@@QAE@J@Z` @0x68d090 的全量 xref（分页合并，替代早期 `xref_p0/200/400.json`） |
| `trunc_ctor_resolved.json` | 8 个函数 × 构造点 | 首轮反编译被截断、二次取证得到的构造点（含 opcode） |
| `trunc2_ctor_resolved.json` | 3 个函数 × 12 个构造点 | 第二轮补采（`0x5dbef0` / `0x627e20` / `0x9a6dd0`） |
| `map_candidates2.json` | `{recv: 375, send: 238}` | v95↔s9 语义候选打分（token 匹配），**仅作线索**，见 §4 第 7 条 |
| `v95_send_list.txt` | 238 行 `opcode<TAB>Class::Method` | 人工校订后的发送侧语义表（映射文档 §3 的输入） |
| `s9_list.txt` | recv 181 / send 311 | 从 s9 `RecvOpcode` / `SendOpcode` 抽出的 `value NAME handler` 清单 |
| `s9_opcodes.json` | `{recv, send}` | 同上，机器可读版 |
| `onconnect_manual.json` | 2 条 | `CClientSocket::OnConnect` 的 20/32 两个手工取证点（`insn_query` 直读 `push 20h @0x4af3e3` / `push 14h @0x4af458`），`merge_send.py` 直接并入 |

### 2.4 发送侧原始反编译

| 文件 | 内容 |
|---|---|
| `dec_send.zip` | 462 份 JSON 反编译（原 6 MB → 1.04 MB，`dec_send/<addr>.json`）。解压后配合 `send_ops.json` 查具体写包逻辑 |

### 2.5 脚本（`scripts/`）

| 脚本 | 用途 |
|---|---|
| `mcp_call.py` | 极简 MCP streamable-HTTP 客户端：`python mcp_call.py <port> <tool> <args.json\|->`；UTF-8 安全（Windows 终端 cp936 时不炸） |
| `extract_cases.py` | 从 Hex-Rays 文本里还原 case 标签与处理函数，处理 `if ((unsigned)(nType - X) > Y)` 形式的跳转表与 virtual call |
| `batch_send.py` | 批量反编译 `??0COutPacket@@QAE@J@Z` 的全部 xref 调用者 → `dec_send/` + `send_ops.json`；`python batch_send.py resume` 断点续跑 |
| `resolve_dyn.py` | 解析 `COutPacket::COutPacket(this, nType: <expr>)` 中的非常量表达式：常量 / 就近赋值 / `LOBYTE` / 右侧强转 → `ctor_sites.json` |
| `extract_trunc_send.py` | 首轮反编译被截断的 8 个函数（`0x5408e0` 等），按 `call ??0COutPacket` 地址回挖前序 `push` → `trunc_ctor_sites.json` |
| `extract_trunc2.py` | 第二轮补采 3 个函数（`0x5dbef0` / `0x627e20` / `0x9a6dd0`）→ `trunc2_ctor_sites.json` |
| `merge_send.py` | 合并四路构造点 + 同函数内共享 `push` 回溯 → `send_sites_all.json` / `send_ops.json`（582 处 / 238 opcode，重复运行结果字节一致） |
| `gen_send_section.py` | 依据上述产物重写 `notes/opcode-v95.md` 的 §5（原地替换，保留 §1–§4/§6–§8） |
| `extract_s9.py` | 解析 s9 `RecvOpcode` / `SendOpcode` 枚举 → `s9_opcodes.json` / `s9_list.txt` |
| `build_map2.py` | v95 与 s9 的语义候选打分（名称 token 匹配 + 手工别名）→ `map_candidates2.json` |
| `gen_opcode_map.py` | 生成 `notes/opcode-map-v95-vs-s9.md`（§3 人工校订表 + §4 接收表 + §5 新 opcode 证据 + §7 自动附加表） |
| `gen_opcode_doc.py` | 重写 `notes/opcode-v95.md` 的文件头与 §1–§4（就地拼接，不覆盖 §5 之后；也不再写 `send_ops.json`） |

---

## 3. 复现步骤

```bash
# 0. 前置：IDA 9.4 + ida-pro-mcp，idalib headless 端点 13338 已 adopt E:\MXD\Maplestory95.i64
#    会话 id 用 tools/call get_metadata 或 func_query 返回的 session 字段确认（本轮为 3c1162a2）

# 1. 反编译三个顶层分发器
python scripts/mcp_call.py 13338 decompile '{"addr":"0x4b00f0","database":"3c1162a2"}' > dec_processpacket.json
python scripts/mcp_call.py 13338 decompile '{"addr":"0x9e5840","database":"3c1162a2"}' > dec_cwvscontext_onpacket.json
python scripts/mcp_call.py 13338 decompile '{"addr":"0x546d59","database":"3c1162a2"}' > dec_cfield_onpacket.json

# 2. 取 COutPacket 构造函数 xref（分页，单次上限 10 行 → offset 0/200/400）
#    函数地址 ??0COutPacket@@QAE@J@Z @ 0x68d090
python scripts/mcp_call.py 13338 xref_query '{"database":"3c1162a2","queries":[{"filter":"??0COutPacket@@QAE@J@Z","count":200,"offset":0}]}' > xref_p0.json

# 3. 批量反编译 462 个调用者（约 8 分钟，可中断续跑）
python scripts/batch_send.py

# 4. 解析变量 opcode / 补采被截断函数
python scripts/resolve_dyn.py          # -> ctor_sites.json
python scripts/extract_trunc_send.py   # -> trunc_ctor_sites.json / trunc_ctor_resolved.json
python scripts/extract_trunc2.py       # -> trunc2_ctor_sites.json / trunc2_ctor_resolved.json

# 5. 合并 + 重写发送表
python scripts/merge_send.py           # -> send_sites_all.json / send_ops.json（582 / 238）
python scripts/gen_send_section.py     # 就地刷新 notes/opcode-v95.md §5

# 6. 重写文件头与 §1–§4（就地拼接，不动 §5 之后；只需 dec/ 与 cfield_map.json / dec_maps.json）
python scripts/gen_opcode_doc.py

# 7. v95 ↔ s9 映射
python scripts/extract_s9.py           # 读 s9 RecvOpcode/SendOpcode -> s9_opcodes.json
python scripts/build_map2.py           # -> map_candidates2.json
python scripts/gen_opcode_map.py       # -> notes/opcode-map-v95-vs-s9.md
```

校验：

```bash
cd evidence/opcode-v95 && sha256sum -c hashes.txt     # Linux/Git Bash
certutil -hashfile dec_processpacket.c SHA256          # Windows
```

---

## 4. 已知口径与坑

1. **case 标签会「串」到下一个 case**：Hex-Rays 在跳转表分支里把多个 case 标签归到同一条 `if` 链；
   若该 case 体是纯 vtable 虚调用（如 163），朴素正则会把下一个 case 的处理函数错配给它。`extract_cases.py` 用 `+ 44))(` / `vfptr[` 判定虚调用。
2. **`HELPERS` 前缀匹配**：`TSingleton` / `ZAllocEx` / `CWnd::` 等出现在整行不代表 handler，必须只对**捕获到的被调名**做前缀匹配
   （早期版本 `ZAllocEx<ZAllocAnonSelector>::Alloc` 被误当 handler）。
3. **区间链 vs switch**：`CField::OnPacket` 只有 37 个直接 case，其余全部是 `(unsigned)(nType - X) > Y` 区间，交给 23 个二级分发器。
   只看 switch 会漏掉 380+ 个 opcode。
4. **`CCashShop` 的 382–396 与 `CField` 的区间重叠**：`g_pStage` 的虚表在游戏中是 `CField`、在商城是 `CCashShop`、在登录是 `CLogin`；
   同一 opcode 在不同 stage 落到不同处理器，做移植映射时**必须带上 stage 前提**。
5. **发送侧只有「构造点」，没有「线上序号」**：`COutPacket(nType)` 的 `nType` 就是 v95 的发送 opcode，
   但 462 个函数里含**死代码/旧版本残留**（同一 opcode 多个构造点属正常），需按调用链再筛。
   582 处构造点已全部归因到 238 个 opcode：502 处来自完整反编译，80 处来自 11 个截断/漏采函数的
   `insn_query` 二次取证（`extract_trunc_send.py` / `extract_trunc2.py` / `onconnect_manual.json`），
   其中 1 处（`0x628304`）与前序跳转表分支共享 `push 90h`。
6. **`dec_maps.json` 不可直接引用**，见 §2.2 注。
7. **自动 token 匹配不可靠**：`build_map2.py` 只做线索（得分 1.0 的精确命中仅 send 7/238、recv 90/375），
   且有可证伪的错配——v95 recv 48 `OnQuestClear`→150（实为 49）、282 `CSummonedPool::OnSkill`→37（实为 180）、
   101 `FamilyJoinRequest`→98（实为 97）、114 `AvatarMegaphoneRes`→111（实为 110）。映射表以人工语义裁定为准。
8. **攻击类发送 opcode 静态取不到**：普攻/近战/远程/体术四个构造内联在 `0x9237f8`（无 `COutPacket` 构造调用、
   无 `push` 立即数），`0x91e780` / `0x925a00` / `0x9123c0` / `0x930710` 只调 `Encode*` / `SendPacket`。
   静态手段到此为止，必须抓包。
9. **生成脚本按章节分工，互不覆盖**：`gen_opcode_doc.py` 只重写文件头 + §1–§4，`gen_send_section.py` 只重写 §5，
   `gen_opcode_map.py` 整体生成映射文档；三者都以 `_ms_probe/` 下的中间产物为输入，因此须在 `E:/project/_ms_probe/` 运行。
   两轮重跑已验证输出字节一致（`opcode-v95.md` 935 行、`opcode-map-v95-vs-s9.md` 835 行）。

---

## 5. 与 s9（v83）的对照结论（摘要）

完整逐条表见 **[notes/opcode-map-v95-vs-s9.md](../../notes/opcode-map-v95-vs-s9.md)**（238 个发送 + 274 个接收 opcode，含证据等级标注）。
核心：**opcode 编号不可照抄，只能按语义对齐**。

| 系统 | v95 | s9 (v83) |
|---|---|---|
| 怪物图鉴 设卡 / 设封面 | recv 86 / 87 | send 83 / 84（方向也相反） |
| 制造 请求 / 结果 | 125 / 248 | 113 / 217 |
| 联盟 操作 | 167 | 143 |
| 信使 | 143 | 122 |
| 现金商店 | 382–396（CCashShop stage） | 走 CField 区间 |

覆盖度（映射文档 §6）：发送 238 = 证据 133 / 待核 90 / 无对应 15；
接收 274 = 证据 188 / 待核 84 / 无对应 2。
另有 12 个 v95 新增/改名 opcode 已逐条定位到构造点（映射文档 §5.1），
以及 5 处 v95 与 s9 语义归并不一致的修正（§5.2：44/45/46、103、144、214、219）。

---

## 6. 校验和

见 `hashes.txt`（14 项，`sha256sum` 生成）：

```
d7d8c5b100b20c5976ca492b5abd4696f154336c7924800848982989229ba768 *dec_processpacket.c
d150593bd4ad1192e8fbcf646a5eaedb2b5807443ea08e345cbb04c5df2acc04 *dec_cwvscontext_onpacket.c
2fced22ab659c9e2ac22410feb63e53b32a3c442aec807f1098e8d16531863b1 *dec_cfield_onpacket.c
469b8e87bbca9b4fb35376c06390f2c1b25c1ffca2a8573cf800ac88f6129ef2 *send_ops.json
2d2c3059999f72fc8dac930782cf99464c00a976c67f83595a4fa156282b3146 *send_sites_all.json
22ac5b5fca2c6b90462eab721cec51e12a5f81148c7858b3130d6d0d6249c669 *ctor_xrefs_all.json
10cff9538036a792bc4129fc3c6c74741669ded92532fbdeee72ad11525e0953 *trunc_ctor_resolved.json
93a2f6f8aef3f13a68d49f03863b929f905a18b7476e36b331403bb9808158d5 *trunc2_ctor_resolved.json
92fd786fc16b7b94efc0e2ce9a1f51929a105160abd8c792522c0b1ed92ce7df *map_candidates2.json
79ddea8f387d4259bb8ed04a534070e6fc77b87c80cd837440f15e3b264b22f9 *s9_opcodes.json
ca057f53c0f0b9257c5333e22d0a840ddab2051459b2f8770014cbf05e9155d5 *v95_send_list.txt
769b308977a0f4b4a0bec315d8163099c349bbd3d41c17b14174a894a8ee5ff3 *s9_list.txt
d9166ed4a20050275362768df175230a956fd3fd1ed6450b3478c52b45781243 *dec_send.zip
b04601507430043dd17bcb8e0a4953d033c3d82af9055ff5c59f3da784fcab7c *onconnect_manual.json
```

> 早期 `send_ops.json`（217 opcode 版本，sha256 `2ae9ec02…`）已被 238 opcode 版本取代，旧值仅作历史对照。

`dec/` 33 份共 106,493 B；`dec_send.zip` 1,039,088 B（解压 462 份）。
