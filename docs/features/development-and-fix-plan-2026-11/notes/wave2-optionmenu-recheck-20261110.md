# Wave2 · 2-11 UIOptionMenu 分辨率下拉框 sButtonUOL 空串守卫销案（round43 / 2026-11-10）

> 结论：**修复并销案**。本项是纯本地 L-A（构造期字符串/异常守卫），可离屏自测，不解析 WZ/IMG/XML 节点内容，
> 不改协议/包体/资源。第 10 例同族病 = 「主体 happy-path 正常、旁路子节点漏守卫」在
> 空串解引用 + 裸 `std::stoi` 上的表现：round34 UIWorldSelect → 35 UICharSelect → 36 Textfield →
> 37 UINpcTalk → 38 UIRaceSelect → 39 UIGuildBBS → 40 MOVE_SUMMON → 41 入口遮蔽 →
> 42 无符号下溢 → **43 空串 UB / 抛异常**。

## 0. 性质与授权

- 用户 m04292 授权：改游戏源码 / 自测 / 测完提交 / 遇 `MapleStory.exe` 占用直接关闭 / 不能自测的记录 backlog。
- 硬约束：083/v83 only 禁 095；校准≠改回原生；s9 协议优先；数值重合≠语义重合；十一面校准 README 只读。
- AGENTS.md ACTIVE（WZ/IMG/XML 先 orange-wz，仅 `IMG 解析失败` 等解析异常才 `.nx` 回退留原文 + 65536B key 规格）。
  本项只改一处 C++ 构造期分支，不读取 `.wz/.img/.xml` 内容，沿用 round40 `evidence/orange-wz-resource-structure.json`
  与 `evidence/nx-resource-content-probe.json` 已闭合的资源层门禁结论，不重复触发。

## 1. 缺陷（live 逐字，修复前）

`IO/UITypes/UIOptionMenu.cpp:96-98`（`UIOptionMenu::UIOptionMenu()` 构造期，`:49`）：

```cpp
std::string sButtonUOL = graphic["combo:resolution"]["sButtonUOL"].get_string();
std::string ctype = std::string(1, sButtonUOL.back());
MapleComboBox::Type type = static_cast<MapleComboBox::Type>(std::stoi(ctype));
```

`type` 流向 `IO/UITypes/UIOptionMenu.cpp:150`
`buttons[Buttons::SELECT_RES] = std::make_unique<MapleComboBox>(type, resolutions, default_option, position, lt, combobox_width);`，
`IO/Components/MapleComboBox.cpp:30-37` 里 `if (type != Type::DEFAULT) combobox += std::to_string(type);` 决定取哪张
`nl::nx::UI["Basic.img"][combobox]` 贴图。

关键事实链：

| 事实 | 锚点 | 含义 |
| --- | --- | --- |
| `nl::node::get_string()` 缺失/非 string 返回默认空串 | `includes/NoLifeNx/nlnx/node.hpp:106 std::string get_string(std::string = "") const;`；`includes/NoLifeNx/nlnx/node.cpp:199-217`（`if (!m_data) return def;` + `case type::none/vector/bitmap/audio: return def;`） | `graphic["combo:resolution"]["sButtonUOL"]` 被裁剪或类型不符时 `sButtonUOL == ""` |
| 空串 `.back()` 是 UB | C++ `std::string::back()` 前置条件非空 | `std::string(1, sButtonUOL.back())` 未定义行为 |
| `std::stoi` 空串/非数字抛异常 | `std::stoi("")` / `std::stoi("x")` → `std::invalid_argument` | 构造期未捕获 → 崩 |
| 根 fallback 护不住子节点 | `IO/UITypes/UIOptionMenu.cpp:51 nl::node OptionMenu = nl::nx::UI["StatusBar3.img"]["OptionMenu"];`；`:53 if (!UiAssetResolve::usable(OptionMenu))`；`:77 nl::node graphic = OptionMenu["graphic"];` | `usable(OptionMenu)` 只判整个 `OptionMenu` 根，`graphic["combo:resolution"]["sButtonUOL"]` 单层节点缺失仍落到裸 `.back()` |
| 同文件另两处 `stoi` 已有守卫 | `IO/UITypes/UIOptionMenu.cpp:335-345` `try/catch (...)` | 反证 `:98` 是漏守卫，不是全文件统一风格 |
| 同类先例 | `IO/UITypes/UIGender.cpp:56` 直接用 `MapleComboBox::Type::DEFAULT` | `DEFAULT` 回退与既有口径一致 |
| 枚举定义 | `IO/Components/MapleComboBox.h:32-44 enum Type : uint8_t { DEFAULT = 1, BROWN = 3, BLUENEG, ... };` | 合法后缀是单数字，`ComboBox0/2` 等非枚举值原式也照透传 |

历史：`git blame -L 96,98` 三行同为 `0c294c6c (Ryan Payton 2019-08-23 03:45:28 -0400)` 上游原生；
本文件最近提交 `b54b506c (2026-11-06)` 加的根 fallback 未下沉到子节点。

后果：`StatusBar3.img/OptionMenu/graphic/combo:resolution/sButtonUOL` 缺失（WZ 裁剪版 / 节点改名 / 资源包不完整）
会让设置窗在**构造期** UB 或抛异常崩掉。不属于「数值重合」问题，是纯 C++ 字符串前置条件与异常路径问题，
与 083/s9 协议命名空间无关。

## 2. 扫描范围与假阳性排除（两族）

本轮换两条 round41/42 未扫的 C++ UB 族，先只读取证、用数据决定立不立项，避免原地打转。

### 族一：空容器 `.front()/.back()/.top()` 解引用（`IO/**/*.cpp` 全扫 15 命中）

逐字核准 14 处已有守卫，只留 `UIOptionMenu.cpp:96` 一处确定性缺口：

| 候选 | 判定 | 依据 |
| --- | --- | --- |
| `IO/UIStateGame.cpp:910 elementorder.back()` | 假阳性 | `elementorder.empty() \|\| elementorder.back() == type` 短路守卫 |
| `IO/UITypes/UIChatBar.cpp:357-359 message_history.back().text` | 假阳性 | `if (!message_history.empty())` |
| `IO/UITypes/UILoginWait.cpp:161-164 bar_frames.front()` | 假阳性 | `if (!bar_frames.empty())` |
| `IO/UITypes/UIMessenger.cpp:304-306 text.back()` | 假阳性 | `if (text.size() >= 2)` |
| `IO/UITypes/UINpcTalk.cpp:1858-1865 avatar_styles.front()` | 假阳性 | `if (avatar_styles.empty()) { LOG_WARN; return; }` |
| `IO/UITypes/UIMiscShells.cpp:2220/2222/7668/7670`、`UINpcTalk.cpp:857/859`、`IO/Window.cpp:289` | 假阳性 | 均 `while (!x.empty() && ...)` |
| `IO/UITypes/UIWorldSelect.cpp:633 ch_select_frames.back()` | 假阳性 | 上游 `:627 if (channelid != 0xFF && !ch_select_frames.empty())` 门控 |
| `.top()/.pop()` 16 命中（`UIMapInfoPanel.cpp` / `UIMapMobPanel.cpp` / `UIMapSearchPanel.cpp` / `UIQuestLog.cpp`） | 假阳性 | 全是 `area.top()` / `list_area().top()` 矩形方法，非容器 top |
| **`IO/UITypes/UIOptionMenu.cpp:96-98`** | **真缺陷** | 无任何空串/非数字守卫，且后果落在构造期 |

### 族二：range-for 遍历中改同容器（自写变异扫描器，`IO/ Gameplay/ Character/` 3 命中）

| 候选 | 判定 | 依据 |
| --- | --- | --- |
| `Character/GuildModel.cpp:16` | 假阳性 | 粗 brace 匹配器把 `:27` 下一个索引循环 `guild.members.erase(...)` 串进 range-for 体，纯扫描器误报 |
| `IO/UITypes/UIKeyConfig.cpp:1070` / `:1161 staged_mappings.erase(it.first)` | 假阳性 | `:1096`/`:1200` erase 后立刻 `break`，`IO/UITypes/UIKeyConfig.h:132 std::map<int32_t, Keyboard::Mapping> staged_mappings;` 是 node-based 容器，被删元素迭代器不再自增 → 定义良好 |

教训：静态扫描器命中必须逐字读控制流，不按 grep 数字立项；「文件内重复/变异」不等于缺陷。

**定性**：只有 `UIOptionMenu.cpp:96-98` 同时满足「`get_string()` 可返空串」+「无空串守卫」+「无异常捕获」+
「落在构造期」四条，其余要么有门控、要么不是容器操作、要么扫描器误报。

## 3. 最小修复（1 文件，13 insertions / 2 deletions）

`IO/UITypes/UIOptionMenu.cpp`（live 逐字，`git diff --stat HEAD` = `13 insertions(+), 2 deletions(-)`）：

```cpp
// sButtonUOL 末位是 MapleComboBox::Type 的数字后缀（如 ".../ComboBox1"）。
// get_string() 在节点缺失 / 非 string 类型时返回空串（nlnx/node.cpp:199-217）；
// 空串 .back() 是 UB，std::stoi("") / 非数字字符会抛 invalid_argument，构造期崩。
// 空串或末位非数字 → 回退 DEFAULT（同 UIGender.cpp:56 口径），合法数字路径逐字等价。
MapleComboBox::Type type = MapleComboBox::Type::DEFAULT;
std::string sButtonUOL = graphic["combo:resolution"]["sButtonUOL"].get_string();

if (!sButtonUOL.empty())
{
    char ctype = sButtonUOL.back();

    if (ctype >= '0' && ctype <= '9')
        type = static_cast<MapleComboBox::Type>(std::stoi(std::string(1, ctype)));
}
```

删除修复前 `std::string ctype = std::string(1, sButtonUOL.back());` 与
`MapleComboBox::Type type = static_cast<MapleComboBox::Type>(std::stoi(ctype));` 两行裸操作。

为什么选 `DEFAULT` 兜底 + 双守卫，而不是抛异常/早退/整窗 return：

- 构造期不能因单个 UOL 子节点缺失让整个设置窗废掉；`UIGender.cpp:56` 已用 `DEFAULT`，口径一致；
- `sButtonUOL` 非空且末位是数字时，转换结果与原式**逐字等价**（包括 `ComboBox0`/`ComboBox2` 等非枚举数字照透传，
  本轮 L-A 不裁决枚举合法性，属 D1–D14 类）；
- 空串 / 末位非数字（`""` / `"x"` / `"abc/"` / `"ComboBox"`）不再 UB、不再抛异常，回退 `DEFAULT`(=1)。

未改动：`OptionMenu`/`graphic` 来源、`:53` 根 fallback、`:150` ComboBox 构造、`:335-345` 已有 try/catch、
协议、opcode、WZ/IMG/XML、资源键。

## 4. 自测与门禁（证据）

`tools/test_optionmenu_combo_guard.py` = 12 静态守卫 + 13 输入场景（修复前/后转换算法复刻）：

```
PYCOMPILE_OK
STATIC GUARDS: 12 checked, 0 failed
SCENARIOS:
  ''                               before=UB    after=1
  'x'                              before=THROW after=1
  'abc/'                           before=THROW after=1
  'ComboBox'                       before=THROW after=1
  'ComboBox1'                      before=1     after=1
  'ComboBox3'                      before=3     after=3
  'ComboBox9'                      before=9     after=9
  'ComboBox0'                      before=0     after=0
  '/UI/Basic.img/ComboBox1'        before=1     after=1
  '1'                              before=1     after=1
  '3'                              before=3     after=3
  '7'                              before=7     after=7
  'ComboBox2'                      before=2     after=2
PASS: 12 static guards + 13 scenarios (before UB/THROW 4 -> after 0; regression 9/9)
TEST_RC=0
```

- harness 首跑 `TEST_RC=1` 系脚本自身解包 bug（`STATIC_GUARDS` 每项 5 元组、解包写成 4，
  `ValueError: too many values to unpack (expected 4, got 5)`）→ 就地改 `for ... in ...: *_` 重跑即 PASS；
  属测试脚本自身缺陷，非修复方案失败，不计任务失败。
- `python tools/gen_opcode_matrix.py --check` → `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)`
- `git diff --check HEAD -- IO/UITypes/UIOptionMenu.cpp` → 空（`DIFFCHECK_CLEAN`）
- `tools\build_smoke.bat`（`MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c`）→
  `BUILD_RC=0` + `GATE OK` x2 + `EXITCODE=0`；`build_log.txt` `2026-11-10 02:22:08 / 16010B` →
  `2026-11-10 02:43:19 / 4962B`（mtime 变化，真重编译），`ERROR_LINES 0`；
  tail `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe`。
- 后台 job 外层 `[exit code: 1]` 来自收尾 `grep -ciE "error|错误|失败|fatal"` 无匹配时自身退出码 1
  （打印 `0`），构建本体 `EXITCODE=0` 已单独钉死，不误读为构建失败。
- `taskkill /F /IM MapleStory.exe /T` 对 PID 34476 / 6108 / 56016 三个 32K 空壳 `Access is denied`
  （历史同款，不占资源、不阻断构建），按 m04292 已如实关闭尝试并记录。

## 5. 范围边界（明确未做）

- 未裁决 `ComboBox0` / `ComboBox2` 等非枚举数字的合法性：原式透传，本轮保持一致，归入 Wave3 L-B / D1–D14 候选。
- 未给整窗加早退或改 `:53` 根 fallback 语义：那是资源层级裁决，超出最小修复。
- 未动 2-1/2-2/2-4（orange-wz 内容层 `IMG 解析失败` / Photoshop 位图 / 真机动画），状态不变。
- 未解析任何 `.wz/.img/.xml` 内容，未写资源文件，未提交构建日志与校准证据。

## 6. 真机 backlog（自动测不了，交用户一起测）

见 `docs/features/pending-live-tests/README.md` §22，4 项：
设置窗正常态下拉框外观 / 资源裁剪态不崩且回退 DEFAULT / 分辨率切换回归 / 各类型 ComboBox 数字后缀观感。

## 7. 提交 scope（`git-commit-standard`，零署名）

精确 `git add` 5 文件：

- `IO/UITypes/UIOptionMenu.cpp`
- `tools/test_optionmenu_combo_guard.py`
- `docs/features/development-and-fix-plan-2026-11/notes/wave2-optionmenu-recheck-20261110.md`
- `docs/features/development-and-fix-plan-2026-11/README.md`（§4 新增 2-11）
- `docs/features/pending-live-tests/README.md`（新增 §22）

提交标题：`🐛 fix(ui): 设置窗分辨率下拉框 sButtonUOL 空串守卫（.back() UB / stoi 抛异常）(#devfix-optionmenu-combo)`。
不 add：`build_log.txt` / `*.obj` / 校准 evidence / 截图 / `_*.log`。

## 8. 证据优先级（本项）

`live 源码构造点 > nl::node::get_string() 声明与实现 > 同文件 try/catch 反证 > 同类 UIGender 先例 > git blame > 修复前/后 harness 对照 > 构建门禁 > 真机 backlog`。
本项不依赖 IDA / IMG / 095 线任何结论；Angel 083 校准十一面文档只读、未改动。
