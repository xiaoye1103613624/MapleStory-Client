# 最优修复决策：改定义 vs 改调用点（2026-11-17）

> 方法论：🔶 Amazon Working Backwards（从最终正确状态倒推）+ Dive Deep。
> 本文件修正前报告盲区：`available()` 的受害形态不止 `>=N` 329 处。

## 1. 决定性新发现：InPacket.available() 有 6 种形态，全是同一个根因

全库 `available()` 共 1209 处，其中大部分是 `LoginBackground::get().available()` / `stbi__sse2_available()` 等**其它类**的同名方法，不受影响。真正属于 `InPacket`（recv/packet 等变量）的形态：

| 形态 | 处数 | bool 下行为 | 举例（文件:行号） |
| --- | --- | --- | --- |
| `available() >= N`（N≥2） | 329 | 恒假 → 字段吞成 default | AllianceHandlers.cpp:166、MapObjectHandlers.cpp:1058 |
| `available() < N`（N≥2） | ~7 | **恒真** → early-return 守卫永不触发 | PlayerInteractionHandlers.cpp:33 `if(avail<14) return`、WaveAShell:122/:134、Testing:130 |
| `available() == 21` | 1 | 恒假 → 21字节 pad 特判永不命中 | WaveAShellHandlers.cpp:1218 |
| `available() == 1` | 1 | 恒等于"有字节"（依赖二值） | MapObjectHandlers.cpp:865 |
| `skip(available()-5)` 算术 | 1 | bool 下 -4→巨值，被外层 `>=5` 恒假掩盖未爆 | MapObjectHandlers.cpp:1736 |
| 纯 bool 上下文（if/while/?:/!） | ~833 | 正常 | 遍布 40 文件 |

**结论**：改调用点（只改 `>=N`）会漏掉 `<N`、`==21`、算术三类 —— 前报告 329 处清单不完整。

## 2. 两方案对比（带行号证据）

### 方案 A · 改调用点（MinMS 风格：`available()>=N`→`length()>=N`）
- 需改 ≈338 处（329 + 7 + 1 + 算术1），且必须逐个覆盖 6 种形态。
- 优点：不动 `available()` 公开契约；只重编受影响的 ~12 个 .cpp（重编面较小）。
- 缺点：改动面巨大、易漏（本次扫描就差点漏 `<N`/`==21`）；源码继续 available/length 混用；**未来新增代码仍会再犯**；治标不治本。

### 方案 B · 改定义（`bool available()` → `size_t available()`，`return length()`）
- 只改 2 处：`Net/InPacket.h:37` 声明 + `Net/InPacket.cpp:30-33` 定义。
- 338 处调用点**零改动**自动全部修好；消除根因，未来不再复发。
- 逐形态语义验证（可证）：
  - 纯 bool 上下文（833 处）：size_t→bool 隐式转换非0即真，**语义不变**。
  - `>0`（3 处）/ `<N`（~7）/ `==21`（1）：**变正确**。
  - `>=N`（329）：**变正确**。
  - `==1`（MapObject:865）：位于 `else if`，前件 `>=2` 不成立 ⇒ size_t 下该处 available∈{0,1}，`==1` ⟺ length()==1，**语义等价**。
  - 算术 `skip(available()-5)`（:1736）：size_t 下值正确，且被外层 `>=5` 保护，**变正确**。
- 无 virtual/override、无函数指针/模板推导依赖（已扫 0 命中）；x64 调用方随 .h 全量重编，无 ABI 不匹配。
- 缺点：一次**全局语义变更**；改 .h 触发几乎所有 .cpp 重编（重编面大）；本仓库有 heisenbug 前科（代码布局敏感），需全量重编 + 真机验收兜底。

## 3. 推荐：方案 B 为主 + 2 处防御性显式化

1. 把 `InPacket::available()` 返回类型改为 `size_t`，实现改 `return length();`（2 处改动根治 338 处）。
2. 防御性：把 2 处依赖隐式语义的点显式改用 `length()`，不赌转换：
   - `MapObjectHandlers.cpp:865` `else if (recv.available() == 1)` → `else if (recv.length() == 1)`
   - `MapObjectHandlers.cpp:1736` `recv.skip(recv.available() - 5)` → `recv.skip(recv.length() - 5)`
3. 保留 `length()` 不动；`available()` 与 `length()` 同义，纯 bool 上下文仍写 `if(recv.available())` 保持可读性。

**理由**：这是契约设计错误（名为 available 却返回 bool，诱导调用方比大小），改定义是修正契约本身；改动面 2 处 vs 338 处，对这个增量构建敏感、代码布局敏感的仓库，**更小的源码改动面 = 更小的引入风险**。

## 4. 诚实的风险与验收边界（红线）

- 方案 B 是全局语义变更，静态分析证明等价，但**不等于真机通过**。
- 833 处纯 bool 上下文未逐一人工复核（语义等价由 size_t→bool 标准转换保证，可证）。
- 本仓库硬门禁：① 全量重编（改 .h 影响所有 include .cpp），② touch 源文件 + rm 对应 obj 破增量缓存，③ 构建 error C=0，④ 部署 exe SHA 相对基线变化，⑤ **真机发包**关键链路（进图/伤害/buff/商城/任务）确认无回归。
- 服务端侧结论不变：`QuestHandlers.cpp:96-99` 双分支、死 opcode 等 C/B 类问题与本次返回类型改造**正交**，仍需按各自分级单独处理。
- 本轮仅出决策建议，**未改任何代码**；实施需用户明确授权。
