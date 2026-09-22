# WZ/NX 数据源支持的依赖闭包与提交边界

> 本文件记录「WZ/NX 数据源支持」入库时的**依赖核对结论**、**vendor 产物的判定依据**，
> 以及核对中发现的**两处待办**。

---

## 1. 提交顺序由依赖闭包决定（不是主观排序）

核对 `MapleStory.vcxproj` 与源码 `#include` 后，发现两个**高危漏依赖**，
直接决定 vendor 与 SDL3 必须先于「数据源支持」提交：

| 漏依赖 | 证据 | 若漏掉的后果 |
|---|---|---|
| `tools/vendor/NoLifeWzToNx-master/Keys.cpp` | `MapleStory.vcxproj` 中有 `<ClCompile Include="tools\vendor\NoLifeWzToNx-master\Keys.cpp" />` | 链接期缺 `key_bms/key_gms/key_kms`（内嵌的 `NoLifeWzToNx.cpp` 对其有 `extern` 引用）→ **链接失败** |
| `tools/vendor/NoLifeWzToNx-master/NoLifeWzToNx.cpp` | `Util/WzRuntime.cpp` 直接 `#include` 它（`NL_WZ_PARSER_EMBEDDED` 方式内嵌） | **编译失败** |

另：`MapleStory.vcxproj` 的 SDL3 引用在改动前（`HEAD` 版本）为 **0 处**，
本次新增 12 处（4 个配置 × 3 组路径/库设置）⇒ SDL3 属本次新接入，必须先于 vcxproj 落地。

**最终提交顺序**（满足「任一提交单独检出不破坏其余部分」）：

```
1. chore(tools) — tools/vendor/NoLifeWzToNx-master/**（源码 + 随包预编译库）
2. chore(build) — includes/SDL3/**
3. feat(data)   — vcxproj 接线 + Util/{WzRuntime,DataFiles,ImgFiles,WzFiles} + NoLifeNx wz_file + Configuration 等
```

**反向引用核对**：`IO/KeyCodes.h`、`includes/NoLifeNx/nlnx/wz.hpp`、`Util/DataFiles.h`、`Util/ImgFiles.h`
的被引用方全部落在同一提交内，无遗漏。
（`Template/Point.h` / `Audio/Audio.h` / `Gameplay/Physics/Foothold.h` 虽 `#include` 了 `Util/WzFiles.h`，
但均位于 `#ifdef USE_NX` 的**假分支**——`MapleStory.h` 无条件 `#define USE_NX`，故这些 include 不生效，无需纳入。）

---

## 2. vendor 的「产物 vs 随包依赖」边界（**不可凭扩展名一刀切**）

初判曾想把 `tools/vendor/**` 的 `*.obj/*.pdb/*.lib` 一并忽略，实测**证伪**：

| 证据 | 结论 |
|---|---|
| 上游自带 `cleanup.bat`（作者的「清理构建产物」脚本）只删 `.vs`、`.history`、`x64`、`x86`、`*.vcxproj.user`、`*.log` | 上游认定的产物范围**仅**这些 |
| 上游自带 `.gitignore` 内容同为 `/.history`、`/.vs`、`/x64`、`/x86`、`/files`、`*.log` 等 | 与上一致 |
| `git status --short -uall -- tools/vendor` 中 `x64`/`.vs` 命中 **0 条** | 嵌套 `.gitignore` **已生效**，产物本就进不了待提交 |
| `git ls-files --others --exclude-standard -- tools/vendor` = 119 个，约 16.84 MB | 待提交内容 = 源码 + `includes/{zlib,libsquish,lz4_v1_8_2_win*}` 的**随包预编译库** |
| `MapleStory.vcxproj` 把 `tools/vendor/.../includes/zlib/lib`、`includes/libsquish/win*/$(Configuration)`、`NoLifeNx/nlnx/includes/lz4_v1_8_2_win*` 加入**库目录** | 这些「随包 lib」是**游戏链接依赖**，忽略它们会直接导致链接失败 |

⇒ **未新增任何全局 `*.obj`/`*.pdb` 忽略规则**（那样会误伤随包库）。
体积构成：`x64/` 6.73 MB（产物，已忽略）+ `includes/` 16.09 MB（随包依赖，入库）。

---

## 3. 核对中发现的待办（已处置）

1. **`Util/WzNxBootstrap::prepare()` 无任何调用者**（全仓 `grep WzNxBootstrap` 只命中其自身 `.h/.cpp` 与 vcxproj）
   ⇒ 当前为**死代码**：能编译但不产生任何效果。
2. **`Util/WzNxBootstrap.cpp` 引用了 `Error.h` 中不存在的枚举**
   `WZ_CONVERTER_MISSING` / `WZ_CONVERSION_FAILED`；因门控 `#if defined(USE_NX) && defined(AUTO_CONVERT_WZ)`
   未定义而**当前不会编译到**，一旦启用 `AUTO_CONVERT_WZ` 即**编译失败**。

**结论：移除 `WzNxBootstrap`（`.cpp` / `.h` / vcxproj 条目），依据如下：**
- 全仓无任何调用点（`MapleStory.cpp` 启动链路从未调用 `prepare()`），留之仅为占用编译单元、且是潜在「一旦开宏即编译失败」的雷。
- 其本职「首次运行调用 `NoLifeWzToNx.exe` 把 `.wz` 转 `.nx`」已被运行时分发架构取代：
  - WZ 后端 `WzFiles::init()` 直接经由 `nl::wz::load_all()` 加载 `.wz`，**不需要** `.nx` 中间产物；
  - IMG 后端 `ImgFiles::init()` 自带 `.nxcache` 自转换逻辑，亦不消费 `WzNxBootstrap` 的输出。
- 故移除不影响任一数据源路径，反而消除死门控与 `Error.h` 缺失枚举的隐患。

> 备注：`Error.h` 中无需再补 `WZ_CONVERTER_MISSING` / `WZ_CONVERSION_FAILED`，随 `WzNxBootstrap` 一并作废。

3. **历史提交的一个小瑕疵（不修历史）**：`IO/LoginBackground.cpp` 在提交 `7d5ad8d` 中入库，
   但其 `vcxproj` 条目随 `c7d8bef` 才落地 ⇒ 单独检出 `7d5ad8d` 时该文件不参与编译。
   各提交的**最终状态**可编译且已实测通过，故不改写历史。

---

## 4. 验证结果

| 项 | 结果 |
|---|---|
| `tools/build_smoke.bat` | `EXITCODE=0` |
| `git status --short` | **空**（工作区完全干净） |
| 忽略规则 | `.codebuddy` / `build_log.txt` / `*.log` / `includes/NoLifeNx/nlnx/*/obj-*` 均命中 |
| 调试残留检索（`DBG-UI`/`DBG-PKT`/`CS-DUMP`/`_ws_debug`） | **0 命中** |
