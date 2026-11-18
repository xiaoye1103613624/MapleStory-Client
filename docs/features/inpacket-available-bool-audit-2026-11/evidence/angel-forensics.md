# Angel 侧取证报告（angel-forensics 子任务回报，纯只读）

## 结论先行
Angel 侧现有产物对 available()>=N bug **既不能佐证也不能反证**，无相关原生证据。不牵强附会。

## 关键认知：两套代码库，不能混用证据
- Angel 是 v83 原生 C++ 客户端（mangled `?Decode1@CInPacket@@QAEEXZ`）。
- `Net/InPacket.cpp:30 available() 返回 bool` 是 emulator 侧移植代码。
- 即便日后拿到 CInPacket 反编译，最多做"原生客户端如何做字节门控"的旁证，不能直接当作 available() bool 误用的证据。

## 已核实证据（带出处）
| # | 事实 | 出处 |
| --- | --- | --- |
| 1 | 取证产物自标来源 "Angel 13339 IDB"，非模拟器代码 | evidence/angel-drop-facts.md:3；angel-drop-symbol-inventory.json:2 |
| 2 | 全量反编译仅 18 个 drop 相关函数 | angel-drop-decompile.json:2-5 total:18,ok:18 |
| 3 | CInPacket 仅作函数参数出现 4 处 | angel-drop-decompile.json:30/:35/:40/:85 |
| 4 | 出现的 CInPacket 方法仅 4 个（Decode1/2/4/Buffer），无剩余字节判断 | angel-drop-decompile.json refs：?Decode1@CInPacket@@QAEEXZ@0x4065f3、?Decode4@@QAEKXZ@0x406629、?Decode2@@QAEGXZ@0x42470c、?DecodeBuffer@@QAEXPAXI@Z@0x432257 |
| 5 | OnPacket 按 opcode 268/269 分派，解码顺序硬编码，无长度门控分支 | angel-drop-decompile.json:30 |
| 6 | 唯一 "available" 字样是 StringPool 文案常量，与 packet 门控无关 | angel-drop-facts.md:166/:270 |
| 7 | Angel 与 BeiDou 18/18 EA 完全一致，差异只是 IDA 分析覆盖度 | angel-beidou-drop-ea-crosscheck.json:9 |
| 8 | tools/_ida_r92/ 26 个反编译文本全为 CUI* 类，零命中 | glob 全量 + ops_grep 24 文件 count=0 |
| 9 | 全盘无 Angel 二进制/IDB；仅 BeiDou.exe.i64 及无关 idb | find E:/project 结果 |
| 10 | IDA/REA 类 MCP 本轮不可用 | ops_discover_tools 无匹配 |

## available/length 相关结论
- 佐证"available() 返回 bool"的 Angel 原生证据：没有（产物无 available 函数/调用）。
- 反证：也没有（18 函数样本太小，不足以推出"Angel 不用 length 语义"）。
- 类似 available() 语义的函数（IsEmpty/GetLength/剩余字节判断）：未发现；唯一近似物 `?GetData@?$TSecType@J@@QBEJXZ@0x42873d` 是加密数值解封装，非剩余字节判断。

## 能力边界与补强条件
- 缺 CInPacket::Decode1/2/4/Buffer 函数体反编译（地址已知 0x4065f3/0x406629/0x42470c/0x432257，无 IDB 无法展开）。
- 补强：① Angel.exe + IDB（或用 BeiDou.exe.i64 次优替代，已证同 EA）；② IDA/REA MCP 恢复；③ 最低成本——用现有 BeiDou.exe.i64 查 4 地址交叉引用与函数体。
- 本轮零副作用：两份临时探针脚本已删除，仓库恢复纯只读。
