# 构建 / 部署 / 稳定性索引（换频道修复）

> 记录构建产物与迭代过程，供复查与回滚。SHA 为 sha256 前 12 位。

## 构建环境

- MSBuild：`C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\MSBuild\Current\Bin\MSBuild.exe`
- 参数：`/p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v142 /m:1`
- 构建脚本范式：`tools/_cc_minbuild.bat`（OutDir=cc_min_out / IntDir=cc_min_int）
- 终态：`BUILD_EXITCODE=0`，error C/LNK = 0

## 关键产物

| 用途 | 路径 | SHA(12) | 稳定性 |
| --- | --- | --- | --- |
| 回滚基线（HEAD，无修复） | `cc_bisect_out/MapleStory.exe` | 3963858f | SURVIVED FPS224 |
| 全修复（重构版 FIN，崩） | `cc_bisect4_out/MapleStory.exe` | 1429e7a4 | CRASH 4/4 |
| 只回滚 Socket（排除元凶） | `MapleStory_bs_nosocket.exe` | da20283a | SURVIVED FPS228 |
| 最小 diff FIN（验证稳定版） | `MapleStory_final_fixed_3b178559.exe` | 3b178559 | SURVIVED 4/4 |
| 带 CCTRIGGER 钩子（验证用，已弃） | `cc_hook_out/MapleStory.exe` | 798a41f7 | 启动崩（布局敏感）|
| 当前 live（并行 agent 重编） | `E:/MXD/BeiDou-Client-xy/MapleStory.exe` | 8b03165b | SURVIVED FPS142 |

## 二分定位记录（FIN 崩溃 heisenbug）

崩溃表现：标题画面播放 BGM 后 7–8s，native 退出 0xC0000374（堆损坏）/ 0xC0000005（访问违规）；WER 故障模块 MapleStory.exe 自身，异常偏移 RVA 0x61b6（`mov rbx,[r14+8]`，std::unordered_map 查找，静态初始化期）；与换线代码（收包才执行）、服务端、autologin 均无因果关系。

| 实验 | 回滚文件 | SHA(12) | 结果 | 结论 |
| --- | --- | --- | --- | --- |
| 回滚全 5 文件 | 全部 | 3963858f | SURVIVED | 崩溃与换线改动相关 |
| 只保留 Stage | Socket/SetField/Player | 969e9928 | SURVIVED | 排除 Stage 布局 |
| 单文件 noplayer | PlayerHandlers | a55cba02 | CRASH | 排除 Player |
| 单文件 nosetfield | SetFieldHandlers | 68e2620e | CRASH | 排除 SetField |
| 单文件 nosocket | SocketWinsock | da20283a | SURVIVED | **元凶=SocketWinsock FIN 补丁** |

根因：`const int ready=select(...)` + `const int received=recv(...)` 的变量拆分/控制流重构改变栈与代码布局，触发布局敏感堆损坏。修复：最小 diff 补丁（`Net/SocketWinsock.cpp:132-156`，保留原 `int result` 变量与原控制流，仅在 `if(result>0)` 内 recv 后加 `if(result==0){*success=false;return 0;}`），4/4 稳定。

## live exe 覆盖坑

交付核查期间主 exe SHA 多次被并行构建/登录器覆盖：3b178559 → 6178eaf4(07:47) → 8b03165b(13:36)。判定标准改为"含全部修复标记（reusing/fail_utf8）+ CCTRIGGER=0 + 实测稳定存活"，不追固定 SHA。终态 8b03165b 三项全过。

## 诊断工具脚本（tools/_cc_*.py）

- 启动/截图：`_cc_boot.py`（PrintWindow 对 OpenGL 黑屏，弃）、`_cc_drive.py`（fshot 置前+点击+截图）、`_cc_ingame.py`
- 稳定性：`_cc_stab_one.py`（单 exe auto:false 测存活）、`_cc_batch_stability.py`
- 二分：`_cc_bisect_rollback.py` / `_cc_restore_fix.py` / `_cc_bisect_stage.py` / `_cc_bisect_keepstage.py` / `_cc_fullbuild_test.py` / `_cc_bisect_single.py`
- 崩溃分析：`_cc_disasm_crash.py`（capstone 反汇编 RVA）、`_cc_pe_info.py`、`_cc_sym_resolve.py`（dbghelp 符号解析失败）、`_cc_read_wer.ps1`（WER 报告）
- 真机触发：`_cc_hook_channel_trigger.py` / `_cc_unhook_channel_trigger.py`（临时钩子，已还原）、`_cc_hook_verify.py`
- 修复补丁：`_cc_socket_minimal_patch.py`（最小 diff FIN）
