# REA 能否对 192 脱壳？（2026-11-12）

## 结论

**不能指望 REA「一键脱壳」192 `MapleStory.exe`。**

依据：

1. 静态面：当前 IDB `total_functions=3` / `total_strings=10`，入口在壳段 → 分析的是 stub，不是业务映像。  
2. REA 文档（skill `native-and-artifacts.md`）：*For packed targets, retain the original and separately derived artifact identities; decompiling an unpacking stub does not recover the unpacked program.*  
3. 运行时面：REA 提供 Frida / GDB / Rizin 等，但  
   - **GDB/Rizin 持久调试：Linux-only**  
   - Frida：需本机绑定；官方 verifier 偏 Linux；在 Windows 上挂 Maple + 壳保护（Xigncode 等）成功率/合规风险另论  
4. REA **没有** VMProtect/Themida/自定义壳的专用自动脱壳 provider。

## 若仍要逻辑级反编译

须先得到**另一份已脱壳映像**（手工 dump / Scylla / 专用工具），再：

`原包 IDB` 与 `dump 映像` 分开建库 → REA/IDA 分析 dump → 证据注明两份 artifact 身份。

在 dump 完成前：**技能/职业行为以 WZ/XML + Angel(s9) 协议对照为准**，不以 192 主程序伪代码为准。
