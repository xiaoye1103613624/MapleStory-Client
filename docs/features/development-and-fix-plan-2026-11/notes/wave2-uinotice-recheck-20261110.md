# Wave2 · 2-12 UIEnterNumber 数字框空串/溢出守卫销案（round44 / 2026-11-10）

> 结论：**修复并销案**。本项是纯本地 L-A（交互态字符串→整数转换守卫），可离屏自测，不解析 WZ/IMG/XML 节点内容，
> 不改协议/包体/资源。第 11 例同族病 = 「主体 happy-path 有守卫、旁路入口漏守卫」在裸 `std::stoi` 上的表现：
> round34 UIWorldSelect → 35 UICharSelect → 36 Textfield → 37 UINpcTalk → 38 UIRaceSelect →
> 39 UIGuildBBS → 40 MOVE_SUMMON → 41 入口遮蔽 → 42 无符号下溢 → 43 空串 UB →
> **44 空串 vacuous-true / 10 位数字 int32 溢出**。

## 0. 性质与授权

- 用户 m04292 授权：改游戏源码 / 自测 / 测完提交 / 遇 `MapleStory.exe` 占用直接关闭 / 不能自测的记录 backlog。
- 沿用 m04292 一次性 Plan（`docs/features/development-and-fix-plan-2026-11/README.md`），不重开 Plan。
- 硬约束：083/v83 only 禁 095；校准≠改回原生；s9 协议优先；数值重合≠语义重合；十一面校准 README 只读。
- AGENTS.md ACTIVE（WZ/IMG/XML 先 orange-wz，仅 `IMG 解析失败` 等解析异常才 `.nx` 回退留原文 + 65536B key 规格）。
  本项只改一处 C++ 交互态分支，不读取 `.wz/.img/.xml` 内容，沿用 round40
  `evidence/orange-wz-resource-structure.json` 与 `evidence/nx-resource-content-probe.json` 已闭合的资源层门禁结论，
  不重复触发。

## 1. 缺陷（live 逐字，修复前）

`IO/UITypes/UINotice.cpp:339-359`（`UIEnterNumber::handlestring(std::string numstr)`）：

```cpp
int num = -1;
bool has_only_digits = (numstr.find_first_not_of("0123456789") == std::string::npos);
...
if (!has_only_digits)
{
    numfield.set_state(Textfield::State::DISABLED);
    UI::get().emplace<UIOk>("Only numbers are allowed.", okhandler);
    return;
}
else
{
    num = std::stoi(numstr);
}
```

关键事实链：

| 事实 | 锚点 | 含义 |
| --- | --- | --- |
| 空串 `find_first_not_of` 返 `npos` → vacuous true | `IO/UITypes/UINotice.cpp:342` `bool has_only_digits = (numstr.find_first_not_of("0123456789") == std::string::npos);`；C++ `std::string::find_first_not_of` 空串无字符可比较 | `numstr == ""` 时 `has_only_digits == true`，跳过 `:350 if (!has_only_digits)` 提示分支 |
| `std::stoi("")` 抛 `std::invalid_argument` | `IO/UITypes/UINotice.cpp:358 num = std::stoi(numstr);` 无 `try/catch` | 构造期/交互态未捕获 → 进程崩 |
| `numfield_limit = 10` 是字符数上限，不是数值上限 | `IO/UITypes/UINotice.cpp:244 int16_t numfield_limit = 10;` / `:246 Textfield(..., numfield_limit)` | `9999999999`（10 位纯数字，全过 `find_first_not_of`）→ `std::stoi` 抛 `std::out_of_range`；SoftKey 旁路还能送更长串 |
| 构造期初值非空但可退格清空 | `IO/UITypes/UINotice.cpp:247 numfield.change_text(std::to_string(quantity));`；`IO/Components/Textfield.cpp:215 text.erase(start, static_cast<size_t>(markerpos) - start);` / `:253 text.erase(static_cast<size_t>(markerpos), end - static_cast<size_t>(markerpos));` | 用户 BACKSPACE/DELETE 清框后按 RETURN 即崩，非空初值不构成守卫 |
| 四条入口三条裸奔，唯 SoftKey 有守卫 | `:252 handlestring(numstr);`（Textfield enter_callback） / `:300 handlestring(numfield.get_text());`（send_key RETURN，`:298`） / `:329 handlestring(numfield.get_text());`（button_pressed OK，`:328`） / `:312 handlestring(s);`（SoftKey lambda，`:310 if (s.empty()) return;`） | 同族不对称：主体 happy-path 三条无门控，旁路 SoftKey 反而有 `empty()` |
| 业务调用点纯交互态 | `IO/UITypes/UIItemInventory.cpp:1957,:1977`（丢弃数量，`UI::get().emplace<UIEnterNumber>(dropmessage, onenter, count, count);`）；`IO/UITypes/UIShop.cpp:822,:938`（买/卖数量，`UI::get().emplace<UIEnterNumber>(question, onenter, buyable, 1);`） | 真实可达，不是 dead code |
| 上游原生、近期未修 | `git blame -L 339,359` → `:341/:342/:350/:358` 同为 `1ca11fdd (Ryan Payton 2019-05-01 11:11:31 -0400)`；`git log --oneline -8 -- IO/UITypes/UINotice.cpp` → `2956985d`（UIOk tip）/ `b4874e42`（多分辨率）/ `0cd83033`（HUD）/ `a6ec68b3` | 最近改动只动 tip/重锚，未触 `handlestring` 转换逻辑 |

定性：第 11 例同族病「主体 happy-path 有守卫、旁路入口漏守卫」。证据优先级：
`opcode-matrix > live 源码构造点/调用点 > Textfield 库语义 > git blame > 旧账`。

## 2. round44 全族扫描与假阳性排除（避免按 grep 数字立项）

换族动机：round41 入口遮蔽、round42 `size()-1` 下溢、round43 空串 `.back()` UB 已闭环；
本轮扫「裸数值转换 `stoi/stoul/stoll/stof/stod/atoi`」族（外部数据驱动构造期/交互态漏守卫）。

```
grep -rnE 'std::(stoi|stoul|stoll|stof|stod)|[^_]atoi\(' --include=*.cpp IO Gameplay Character Data Net
→ 344 命中；自写 python 扫描器（try/catch 跨行近似配对）→ 23 文件 unprotected 清单
```

逐字精读排除（扫描器误报率高，前几轮教训：**必须读控制流不按 grep 数字立项**）：

| 候选 | 逐字结论 | 判定 |
| --- | --- | --- |
| `Graphics/GraphicsGL.cpp:1511,1546,1671` | 注释里 IDA 字样 `atoi(token+2)`；实际 `try_parse_item_icon` / `try_parse_skill_icon` 是手写 digit 循环（`for (size_t p=first+2;p<last;++p)` + `ch<'0'||ch>'9' return false` + 溢出 `next<item_id` 守卫） | 假阳性 |
| `Util/Misc.cpp:177` | 已在 `try{...}catch(...){id=0;}` 内 | 假阳性（跨行 try 误判） |
| `Data/QuestData.cpp:373 std::atoi(name.c_str())` | `:370 !std::isdigit(name[0])` 前置；`atoi` 不抛 | 假阳性 |
| `Util/Patch/HttpClient.cpp:68,:248` | `:247 headers.size()>=12 && rfind("HTTP/",0)==0` 守卫；`atoi` 返 0 不抛 | 假阳性 |
| `IO/UITypes/UIMiscShells.cpp:954,1040 std::stoi(input)` | `:945 if(input.empty()) return;` + `:949-952 all_digit` 字符循环守卫 | 假阳性 |
| `IO/UITypes/UIQuestLog.cpp:761,762 atoi(...)` | `:755 isdigit` 循环 + `:758 if(i==tail) return -1`；`atoi` 不抛 | 假阳性 |
| `IO/UITypes/UICharSelect.cpp:913` / `IO/UIChannel.cpp:241` | `try{ stoi }catch(...){}` | 假阳性（跨行 try 误判） |
| `IO/UITypes/UITermsOfService.cpp:193 int32_t i = std::stoi(name);`（`name = text.name().substr(4, 6)`，`:190-191`） | 资源驱动（EULA 节点名裁剪/改名），非交互态；独立资源门控面 | 次要活跃候选，本轮不立项，留后续轮次 |
| `IO/UITypes/UIRaceSelect.cpp:270 class_map[node_index++] = std::stoi(name);` | 只 skip `intervalX`/`pos`/PINKBEAN/YETI（`:264`），其余节点名未校验数字；资源驱动 | 次要活跃候选，本轮不立项，留后续轮次 |
| **`IO/UITypes/UINotice.cpp:358`** | **纯交互态、确定性最高、四条入口三条裸奔** | **立项（本项）** |

## 3. 最小修复（live，17 insertions / 2 deletions）

删 `IO/UITypes/UINotice.cpp:342` 裸 vacuous 断言与 `:358` 裸 `stoi`：

```cpp
// 空串的 find_first_not_of 返回 npos（vacuous true），会让下面的 stoi("") 抛 invalid_argument：
// 退格清空数字框后按 RETURN / 点 OK / Textfield enter_callback 都会走到这里，必须显式挡住。
// 加 !empty() 后空串复用既有 "Only numbers are allowed." 提示，合法数字路径逐字不变。
bool has_only_digits = !numstr.empty()
    && (numstr.find_first_not_of("0123456789") == std::string::npos);
...
else
{
    // numfield 允许 10 字符（构造期 numfield_limit = 10），纯数字也可能溢出 int32
    // （例如 9999999999 -> std::stoi 抛 out_of_range）。统一收口到同一提示，不崩交互。
    try
    {
        num = std::stoi(numstr);
    }
    catch (...)
    {
        numfield.set_state(Textfield::State::DISABLED);
        UI::get().emplace<UIOk>("Only numbers are allowed.", okhandler);
        return;
    }
}
```

- 空串与非数字**复用同一既有提示**（不新增 UI 文案、不改消息表/资源）。
- 溢出同样复用该提示，不裁决「是否该改成 too large 专文」——那是产品文案决策，归真机 backlog。
- `1..max` 合法路径、`< 1` / `> max` 三条既有提示逐字等价，零 happy-path 改动。
- 不改协议/包体/WZ/IMG/XML，不触碰 `numhandler` 回调语义。
- `OLD1_FOUND 1 OLD2_FOUND 1` / `WROTE_OK crlf=True bytes 12934 -> 13699` / `git diff --check` = `DIFFCHECK_CLEAN`。
- 选择「`!empty()` + `try/catch`」而非把 `stoi` 换成手写 digit 循环：前者最小、不重写既有合法语义；
  `find_first_not_of` 已保证 `catch` 只兜 `out_of_range`（和防御性兜底），正常路径不进 catch。

## 4. 自测与门禁（实证）

- `tools/test_uinotice_handlestring_guard.py`：**22 静态守卫**（`!numstr.empty()` / `find_first_not_of` 保留 / 三条注释 /
  `try` / `catch (...)` / `stoi` 保留 / `numfield_limit = 10` / `Textfield.cpp text.erase` / 四条入口逐字 /
  三条既有提示 / Inventory+Shop 调用点）+ **13 输入场景**复刻 before/after 转换算法
  （5 组崩溃输入：空串 RETURN、空串 OK、`9999999999`、`2147483648`、SoftKey `999999999999`；
  8 组回归：非数字 `abc`/`12a`/`x`、`0`、`1`、`5@max=5`、`6@max=5`、INT32MAX 合法）。
  - 实跑：`PYCOMPILE_OK` / `STATIC GUARDS: 22 checked, 0 failed` /
    `PASS: 22 static guards + 13 scenarios (before THROW 5 -> after 0; regression 8/8)` / `TEST_RC=0`。
  - harness 期望值全部由 `SCENARIOS` 标注推导（`expected_before_throw=5` / `expected_regressions=8`），
    不手写死数，避免 round43 那种「脚本自身数错元组/死值」假红。
- `python tools/gen_opcode_matrix.py --check` → `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)`。
- `git diff --check HEAD -- IO/UITypes/UINotice.cpp` → `DIFFCHECK_CLEAN`。
- `tools\build_smoke.bat`（后台 job `bash-d284eac9fabc4438`，`MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c`）：
  `GATE OK` x2 + `EXITCODE=0` + `BUILD_RC=0` + `ERROR_LINES 0`；
  `build_log.txt` `BEFORE_MTIME 2026-11-10 02:43:19 / 4962B` → `AFTER_MTIME 2026-11-10 03:00:47 / 3521B`
  （真重编译）；tail `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe` + 3 行「已复制 1 个文件」
  + warning `LNK4098 LIBCMT` / `LNK4099 vc120.pdb` / `/LTCG`。
  `taskkill /F /IM MapleStory.exe /T` 对 34476/6108/56016 三个 32K 空壳 `Access is denied`
  （历史同款，不占资源、不阻断构建）。

## 5. 范围边界

- 只改 `IO/UITypes/UINotice.cpp` 一个文件的 `UIEnterNumber::handlestring` 两处（空串断言 + `stoi` 包裹）。
- 不改：协议/opcode/包体、WZ/IMG/XML、`MapleComboBox` 枚举合法性、`numfield_limit` 数值、
  `UIEnterNumber` 构造初值、其它 22 文件裸转换候选（逐字排除或留后续轮次）。
- 溢出提示文案是否与「只允许数字」语义一致属产品观感，不在 L-A 自测范围，进 §6 真机联测。
- 本轮不触发 orange-wz（纯 C++ 交互链）；AGENTS.md 的 WZ/IMG/XML 门禁结论沿用 round40 已闭合证据。

## 6. 真机 backlog（不能自测，交用户一起测）

逐字写入 `docs/features/pending-live-tests/README.md` §23（round44）4 项：
23-1 丢弃/买卖数字框正常态、23-2 退格清空后 RETURN/OK 不崩且弹提示、
23-3 超长/溢出数字提示观感、23-4 SoftKey 数字入口回归。

## 7. 提交 scope（精确 add，零署名）

```
IO/UITypes/UINotice.cpp
docs/features/development-and-fix-plan-2026-11/notes/wave2-uinotice-recheck-20261110.md
docs/features/development-and-fix-plan-2026-11/README.md
docs/features/pending-live-tests/README.md
tools/test_uinotice_handlestring_guard.py
```

`git-commit-standard`：`<Emoji> <Type>(<Scope>): <标题> (#<IssueID>)`，结构化 body，零署名
（无 Powered by / Co-authored-by / Generated by / Signed-off-by）；一个缺陷一次提交；
不 add 校准证据/截图/`*.obj`/`_*.log`/`build_log.txt`；`.gitignore` 混合编码不编辑。

## 8. 证据优先级

`opcode-matrix.json > live 源码构造点/调用点 > 库语义（Textfield::erase / std::stoi / find_first_not_of）
> 离线 harness before/after 复刻 > 构建门禁 > git blame > 旧账`。
不按 grep 344 命中立项，逐字读控制流后只认确定性交互态崩点。
