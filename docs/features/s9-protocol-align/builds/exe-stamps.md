# exe 构建指纹登记

**为什么需要这张表**：崩溃取证靠的是 `WER P3/P6 签名（= PE TimeDateStamp）→ 同代 PDB → 源码行`。
构建直接写进 `E:\MXD\BeiDou-Client-xy\`，**exe 与 PDB 每次都被原地覆盖**，所以一旦重新构建，
旧崩溃偏移就再也解析不了了。表里的 `TimeDateStamp` 是唯一能把 WER 记录和构建对上的钥匙。

| 构建时间 | TimeDateStamp | exe sha256 前缀 | pdb sha256 前缀 | 门禁 | 变更内容 | PDB 是否还在 |
| --- | --- | --- | --- | --- | --- | --- |
| 2026-10-30 20:08:01 | `0x6ae488a0` | `f735afdf09b473c9` | 未登记 | `GATE OK` / EXITCODE=0 | 会话 #1/#2 所用构建（T5/T6 改值之后） | ❌ 已被 21:12 覆盖 |
| 2026-10-30 21:12:39 | `0x6ae497c6` | `3cbf1ec6c0cfa4bb` | `e94edab063dc492d` | `GATE OK` / EXITCODE=0 | **classic HUD 聊天栏空指针修复**（`notes/crash-chatbar.md` §4） | ❌ 已被 23:23 覆盖；**但它解出的 `0x191880` 已落盘**，见 [`evidence/crash/session4-uievent-oob.txt`](../evidence/crash/session4-uievent-oob.txt) §1 |
| 2026-10-30 23:23:01 | `0x6ae4b655` | `9de6ba1b7182b0e5` | `2c3ee42a9acf6fd6` | `GATE OK` / EXITCODE=0 | UIEvent 越界修复 + 0x004D 改按 yellow tip 路由的第一版 | ❌ 被 23:34 覆盖（**仅注释差异**，代码相同；见下一行） |
| 2026-10-30 23:34:09 | `0x6ae4b8f0` | `db0570832a0cec04` | `709204e48f3759fd` | `GATE OK` / EXITCODE=0 | 同上 + 注释订正（`notes/crash-chatbar.md` §8）⇒ 会话 #5 所用构建 | ✅ 已快照 `E:\MXD\_build_snapshots\20261030-2334_uievent-oob-fix-final\`（exe+pdb，仓库外） |
| 2026-10-31 00:09:25 | `0x6ae4c134` | `1ef806775e8c4639` | `714309b476a42736` | `GATE OK` / EXITCODE=0 | 聊天栏回车收框 + classic 行文字错位修复（`notes/crash-chatbar.md` §9）⇒ **仅编译链接验证，未部署**：客户端在线，`OutDir` 就是运行目录，覆盖不了 exe | ❌ 同一份源码 3 分钟后正式部署为 `0x6ae4c1fd`，暂存目录 `build_validate/` 已删 |
| 2026-10-31 00:12:45 | `0x6ae4c1fd` | `8d3f221c83658b46` | `15c96673fb5b3cd7` | `GATE OK` / EXITCODE=0 | 同上（退出客户端后正式部署）⇒（当时）当前部署中的构建；已被 2026-11-01 14:16 的全量 `-t:Rebuild` 取代；真机验证见 `notes/crash-chatbar.md` §9.5 | ✅ 已快照 `E:\MXD\_build_snapshots\20261031-0012_chatbar-classic-layout\`（exe+pdb，仓库外） |
| 2026-11-01 06:04 | 未解出（exe 已被 14:16 覆盖） | md5 `942cd202ff87af200fb47b28e2863870`（4,340,224 B） | 未登记 | `GATE OK` / EXITCODE=0 | J 段中途构建（增量），CHECKLIST §J.5 曾以此登记 | ❌ 已覆盖，PDB 亦不可解偏移 |
| 2026-11-01 14:16:01 | `0x6ae6d920` | `d109e1ef979281a1` | `053fcf044d537ae1` | `GATE OK` / EXITCODE=0，0 error | **全量 `-t:Rebuild`**（md5 `9636f77b4030ab4d9a85379cb0694911`，4,419,584 B）⇒ R26 脏 obj 修复后的当前部署构建；J1–J8 全部玩法改动都在这一份里，门禁与脏 obj 复核见 `../../beidou-v17-parity/notes/r807-j8-rebuild-gate.md` | ❌ 未快照；如需保留 PDB，按下方规则复制到 `E:\MXD\_build_snapshots\` |
| 2026-11-01 18:53:30 | 未解出 | `cfa90de97913b871` | `64d833056ad87d12` | `GATE OK` / EXITCODE=0 | R27-B1a/B1b/B2（md5 `6f0a8bc10be012bdb0297350b06be128`，4,422,144 B）⇒ **R27-B2 首轮真机复验就是这一版**（`ItemFull` 9/9、`Item` 4/4、`Stat` 8/8） | ❌ 已被 11-02 的构建覆盖（`build_check/` 里仍是这一版，线上目录已换） |
| 2026-11-02 07:46:19 | 未登记 | 未登记 | 未登记 | 未知（**非本会话构建**） | md5 `d728fc99dddfde4feeaaffcff3ec0518`，4,462,080 B ⇒ 并行会话的地图搜索线，源码当时未提交 | ❌ 本会话未取证，不替它背书 |
| 2026-11-02 10:36:27 | 未登记 | 未登记 | 未登记 | 未知（**非本会话构建**） | md5 `d780ce771b055b7b6d981ff4862f585c`，4,462,592 B ⇒ 同上，本会话只在它上面复跑过 R27-B2 三张 spec（全绿，见 `../../ui-v83-window-restore/evidence/r27-b2-bagfull/rerun-d780ce77.txt`） | ❌ 同上 |

- `TimeDateStamp` 读法：`e_lfanew = u32@0x3C`，然后 `u32@(e_lfanew + 8)`（COFF FileHeader 字段）。
  ⚠ 别从 OptionalHeader 里找它——`tools/symbolize_offset.py` 早期版本就是这么错的。
- 要保留某次构建的符号，把 `MapleStory.exe` + `MapleStory.pdb` 一起复制到构建目录外的带日期目录，再登记本表。
  PDB 约 45MB，不进仓库。自 23:23 起本轮采用的固定位置：`E:\MXD\_build_snapshots\<yyyymmdd-hhmm>_<slug>\`。
- 覆盖前先跑一次 `python tools/wer_event_raw.ps1` + `tools/symbolize_offset.py`，把输出抄进 evidence 文件——
  23:23 那次就没赶上：会话 #4 的 `0x191880` 是 23:08 解的，解完 15 分钟 PDB 就换代了。
