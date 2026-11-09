# Wave2 · 2-10 UICommonCreation avatarSel 无符号下溢销案（round42 / 2026-11-10）

> 结论：**修复并销案**。本项是纯本地 L-A（构造期循环边界），可离屏自测，不解析 WZ/IMG/XML 节点内容，
> 不改协议/包体/资源。第 9 例同族病 = 「主体 happy-path 正常、旁路漏守卫」在 `size_t` 减法上的表现：
> round34 UIWorldSelect → 35 UICharSelect → 36 Textfield → 37 UINpcTalk → 38 UIRaceSelect →
> 39 UIGuildBBS → 40 MOVE_SUMMON → 41 入口遮蔽 → **42 无符号下溢**。

## 0. 性质与授权

- 用户 m04292 授权：改游戏源码 / 自测 / 测完提交 / 遇 `MapleStory.exe` 占用直接关闭 / 不能自测的记录 backlog。
- 硬约束：083/v83 only 禁 095；校准≠改回原生；s9 协议优先；数值重合≠语义重合；十一面校准 README 只读。
- AGENTS.md ACTIVE（WZ/IMG/XML 先 orange-wz，仅 `IMG 解析失败` 等解析异常才 `.nx` 回退留原文 + 65536B key 规格）。
  本项只改一处 C++ 循环边界，不读取 `.wz/.img/.xml` 内容，沿用 round40 `evidence/orange-wz-resource-structure.json`
  与 `evidence/nx-resource-content-probe.json` 已闭合的资源层门禁结论，不重复触发。

## 1. 缺陷（live 逐字，修复前）

`IO/UITypes/UICommonCreation.cpp:163`（`UICommonCreation::init_modern`）：

```cpp
nl::node avatarSel = CustomizeChar["avatarSel"];
```

`IO/UITypes/UICommonCreation.cpp:181`（修复前）：

```cpp
for (size_t i = 0; i < avatarSel.size() - 1; i++)
```

循环体 `IO/UITypes/UICommonCreation.cpp:186-202` 用 `size_t t = i;` 与 `avatarSel[i]["normal"]` 取子节点，
按 `className` 决定偏移（adventurer `i>=2` 时 `t++`；cygnusKnight `i==2 / i>2` 三种纵坐标；其余 `18*(t+1)`）。

关键事实链：

| 事实 | 锚点 | 含义 |
| --- | --- | --- |
| `nl::node::size()` 是无符号 `size_t` | `includes/NoLifeNx/nlnx/node.hpp:118 size_t size() const;` | `size()-1` 在 `size()==0` 时不是 `-1`，是 `SIZE_MAX`（64 位 `(1<<64)-1`） |
| `avatarSel` 可为空 | `IO/UITypes/UICommonCreation.cpp:163` = `CustomizeChar["avatarSel"]` | 资源裁剪 / 节点缺失时返回一个空 `nl::node`，`operator[]` 不抛异常 |
| `init_modern` 无空门控 | `IO/UITypes/UICommonCreation.cpp:163-204` | `:177/:179` 先 `emplace_back(avatarSel["WZ2_skin"], ...)`（空节点只是空贴图，不挂），随后循环用 `SIZE_MAX` 作上界 → 构造期死循环 |
| 对照 `init_classic` 有门控 | `IO/UITypes/UICommonCreation.cpp:383-387` `if (!NewChar) { LoginUiAssets::log_error("[Creation] missing " + root_name); return; }` | 同族的不对称：classic 有早退，modern 漏了 |

后果：`CustomizeChar["avatarSel"]` 缺失（WZ 裁剪版 / 节点改名 / 资源包不完整）会让新建角色 modern 分支
在现代版资源下**构造期挂死**，比崩溃更难定位。不属于「数值重合」问题，是纯 C++ 无符号语义问题，与 083/s9
协议命名空间无关。

## 2. 扫描范围与假阳性排除（`size()-N` 族，43 命中 / 32 NO_GUARD）

对 `IO/ Gameplay/ Character/ Data/` 全 `*.cpp` 正则扫 `.size() - N`，逐字核准后只认这一处确定性下溢；
其余写台账、不放宽标尺改安全代码：

| 候选 | 判定 | 依据 |
| --- | --- | --- |
| `UICashShop.cpp:333/344 promotion_sprites.size() - 1` | 假阳性 | `UICashShop.cpp:109 promotion_sprites.emplace_back(CSPromotionBanner["basic"]);` 恒 ≥1 |
| `UICharSelect.cpp:502/647 charinfo_scroll_frames.size() - 1` | 假阳性 | `:252 charinfo_has_scroll = !charinfo_scroll_frames.empty();` 门控；`:631 if (charinfo_scroll_playing)` 只在有帧时置位 |
| `UIWorldSelect.cpp:506 ch_select_frames.size() - 1` | 假阳性 | `while (ch_select_playing)` + `:1302 if (!ch_select_frames.empty())` 门控；`:627 if (channelid != 0xFF && !ch_select_frames.empty())` |
| `UIWorldSelect.cpp:630 ch_select_frames.size() - 1` | 假阳性 | 同上 `:627` `&& !ch_select_frames.empty()` |
| `UICharSelect.cpp:841 characters_count - 1` / `:899/:951/:1169` | 假阳性 | round35 已加 `characters_count <= 0 return;`；其余在 `characters_count>1` / `selected_page>0` 语境 |
| `UICommonCreation.cpp:1466-1637` 外观环绕 `vec.size()-1` | 假阳性 | `size()>1` 才激活按钮（`:1904-1950`）；`:567 skinColors.empty()` 早退；`randomize_look()` 用 `next_or_zero(size)` 三元守卫 |
| `UIMiniMap.cpp:959 m_stretch = LISTNPC_ITEM_HEIGHT * listNpc_names.size() - 34;` | 假阳性 | 结果是 `int16_t` 算术，不是 `size_t` 上界；`set_npclist` 每图清后重填 |
| `UIChatBar.cpp:498 message_history.size() - 1` | 假阳性 | `int i` 有符号回退，`i>=0` 自带终止 |
| `UIRaceSelect.cpp:816 order.size() - 1` | 假阳性 | `selected_index` 由已注 size 的列表派生 |
| `UIMessenger.cpp:309 text.substr(0, text.size() - 1)` | 假阳性 | `:308 if (text.size() >= 2)` 门控 |
| `UIMTS.cpp:80` / `UIMerchant.cpp:194` / `UIMiscShells.cpp:135` | 假阳性 | `if (items.size() > 4)` / `int vals[4]` 固定数组 + `while (vi<4 ...)` 计数守卫 |
| `UIContextMenu` / `UIMiscShells.cpp:6877` 分词 | 假阳性 | `switch` 有 `default: deactivate()`；`for(size_t i=0;i<=s.size();++i)` 逐字符安全 |

**定性**：只有 `avatarSel` 同时满足「`size()` 返回 `size_t`」+「容器可空且无空门控」+「结果直接作循环上界」三条，
才构成确定性 `SIZE_MAX` 挂死。其余要么恒非空、要么有门控、要么结果不进循环上界。

## 3. 最小修复（1 文件，4 insertions / 1 deletion）

`IO/UITypes/UICommonCreation.cpp:181-184`：

```cpp
// avatarSel 可能为空（资源裁剪 / 节点缺失）：size()-1 是无符号下溢，
// 会变成 SIZE_MAX 让构造期挂在死循环里。i + 1 < size() 在
// size>=1 时与原式逐字等价（同样跳过最后一个子节点），size==0 时 0 次迭代。
for (size_t i = 0; i + 1 < avatarSel.size(); i++)
```

为什么选 `i + 1 < size()` 而不是 `!avatarSel.empty() && i < avatarSel.size() - 1`：

- 上界表达式里**不出现减法**，从根上消掉下溢，不依赖读者记得先判空；
- `size()>=1` 时 `i + 1 < size()` ⟺ `i < size()-1`，迭代序列 `[0, size-1)` 与原式逐字一致
  （包括「跳过最后一个子节点」的语义）；
- `size()==0` 时 `0 + 1 < 0` 为 false，0 次迭代，构造继续走 `:177/:179` 空贴图路径，不挂、不崩。

未改动：`avatarSel` 来源、循环体 `t` 偏移、`avatarSel[i]["normal"]` 取节点、协议、opcode、WZ/IMG/XML、资源键。

## 4. 自测与门禁（证据）

`tools/test_commoncreation_avatar_guard.py` = 9 静态守卫 + 8 场景（修复前/后对照）：

```
[OK] new_loop / old_loop_removed / comment_underflow / comment_equiv /
     avatarSel_source / loop_body_node_access / node_size_unsigned /
     classic_guard_present / no_avatar_empty_gate
[OK] empty_resource_node  n=0  before bound=SIZE_MAX(HANG) -> after iters=0
[OK] single_child..thirty_two_children  before == after（7/7 序列逐字相同）
[underflow] empty-node before HANG=1 -> after zero-iteration=1
[regression] happy-path identical sequences: 7/7
PASS: 9 static guards + 8 scenarios (before HANG 1 -> after 0; regression 7/7)
TEST_RC=0
```

- `python tools/gen_opcode_matrix.py --check` → `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)`
- `git diff --check HEAD -- IO/UITypes/UICommonCreation.cpp` → 空（`DIFFCHECK_CLEAN`）
- `tools\build_smoke.bat`（`MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c`）→
  `GATE OK` + `EXITCODE=0`；`build_log.txt` `2026-11-10 01:50:44 / 9426B` →
  `2026-11-10 02:22:08 / 16010B`（mtime 变化，真重编译），`ERROR_COUNT 0`；
  tail `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe` + 3 行「已复制 1 个文件」。
- `taskkill /F /IM MapleStory.exe /T` 对 PID 34476 / 6108 / 56016 三个 32K 空壳 `Access is denied`
  （历史同款，不占资源、不阻断构建），按 m04292 已如实关闭尝试并记录。

## 5. 范围边界（明确未做）

- 未给 `avatarSel` 加 `if (!avatarSel) return;` 早退：那会改变「空节点也走 `:177/:179` 空贴图」的现有行为，
  超出最小修复；本轮只保证不挂死。
- 未把 `init_classic():383` 的门控复制到 modern：classic 判的是 `NewChar` 根，modern 缺的是 `avatarSel` 子节点，
  两者资源层级不同，复制等于改资源语义，归入 Wave3 L-B 候选。
- 未动 2-1/2-2/2-4（orange-wz 内容层 `IMG 解析失败` / Photoshop 位图 / 真机动画），状态不变。
- 未解析任何 `.wz/.img/.xml` 内容，未写资源文件，未提交构建日志与校准证据。

## 6. 真机 backlog（自动测不了，交用户一起测）

见 `docs/features/pending-live-tests/README.md` §21，4 项：
modern 新建角色正常态 / 资源裁剪态不挂 / classic 回归 / 各职业 `avatarSel` 子节点数差异观感。

## 7. 提交 scope（`git-commit-standard`，零署名）

精确 `git add` 5 文件：

- `IO/UITypes/UICommonCreation.cpp`
- `tools/test_commoncreation_avatar_guard.py`
- `docs/features/development-and-fix-plan-2026-11/notes/wave2-commoncreation-recheck-20261110.md`
- `docs/features/development-and-fix-plan-2026-11/README.md`（§4 新增 2-10）
- `docs/features/pending-live-tests/README.md`（新增 §21）

提交标题：`🐛 fix(ui): 创角 avatarSel 空节点循环下溢守卫（size()-1 → i+1<size）(#devfix-commoncreation-avatar)`。
不 add：`build_log.txt` / `*.obj` / 校准 evidence / 截图 / `_*.log`。

## 8. 证据优先级（本项）

`C++ 源码逐字（loop 上界） > nl::node::size() 声明 > 修复前/后 harness 对照 > 构建门禁 > 真机 backlog`。
本项不依赖 IDA / IMG / 095 线任何结论；Angel 083 校准十一面文档只读、未改动。
