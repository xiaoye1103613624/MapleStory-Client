# 资源加载层抽象设计方案（img / wz / nx 多后端，零转换 + 自适应路径 + 密钥治理）

> 状态：**历史设计稿（已实现）** —— 代码已落地为 `Util/Resource/*`。  
> **请改读**：[`../resource-runtime-img-first/README.md`](../resource-runtime-img-first/README.md)  
> 与 [`../client-platform-masterplan/README.md`](../client-platform-masterplan/README.md)。  
> 下文保留作设计溯源，勿再按「尚未改动业务代码」执行。
>
> 原现状依据含已删除的 `Util/DataFiles.cpp` 等；现行入口为 `ResourceService`。

---

## 0. 目的与非目标

**目标**

| # | 目标 |
|---|---|
| G1 | 建立统一的资源加载**接口抽象**（策略接口），img / wz / nx 三种物理格式都作为可插拔后端 |
| G2 | **不转换**：消除 IMG 需要先落地 `.nxcache` 才能加载的中间环节，任何格式都可以原样读 |
| G3 | **自适应路径**：加载模块能在候选路径链中自动找到可用资源，而不是依赖"必须放在工作目录" |
| G4 | **密钥治理**：抽出独立的密钥层（内置表 / 运行时推导 / 可选外部密钥），并按数据源缓存命中结果 |
| G5 | **最小侵入**：上层 100+ 处 `nl::nx::Map[...]`、`nl::nx::UI[...]` 调用点**一行不改** |

**非目标（本版不做）**

- 不重写现有 parser（NoLifeNx、NoLifeWzToNx 的解析内核保持原样，只做包装）
- 不引入 WZ 写回 / 打包 / 转档能力（转换链路是**只读缓存**，见 §5 的 I3 决策点）
- 不改 `Error::Code` 的既有语义与 retry 行为（必要时**只增不删**）
- 不做资源热重载（仅保留接口位，见 §7）

---

## 1. 现状与痛点（证据驱动）

| 位置 | 现状 | 痛点 |
|---|---|---|
| `Util/DataFiles.cpp` | 按 `Configuration::get_data_format()` 做 `img/nx/wz` 三值字符串 `if` 分发 | **不是接口抽象**：后端是三个平级 namespace 自由函数，无统一契约、不可注册、不可替换，新增格式必须改分发器 |
| `Util/ImgFiles.cpp` | 扫描 `DataDir` 下 16 根目录 → `nl::convert_img_files_to_nx()` **转成 .nx 落盘** → 切 CWD → `nl::nx::load_all()` | **违反 G2**：首次启动昂贵、产生 ~20MB `.nxcache`、脏数据/版本不一致风险、`Map` 还要做 Map001/Map/Map002 三方重组 |
| `Util/WzFiles.cpp` / `Util/WzRuntime.cpp` | 已经是**运行时直读**（不落盘），`add_file("Base.wz")` | 好样板：可作为"零转换后端"的参考实现；但路径写死相对文件名 |
| `Util/NxFiles.cpp` / `includes/NoLifeNx/nlnx/nx.cpp` | `load_all()` 用相对文件名（`std::ifstream("Base.nx")`） | **违反 G3**：永远只看当前工作目录 |
| `Util/WzRuntime.cpp::decode_bitmap` | 解码时对每幅 Canvas **依次试** `archive_key / key_bms / key_gms / key_kms` | **违反 G4**：逐 bitmap 试密钥，热点丢失缓存；密钥与解码逻辑耦合，无法配置/审计 |
| `tools/vendor/.../Keys.cpp` | `key_bms / key_gms / key_kms` 三张 64KB 表编译进主目标 | 只能靠"穷举试"，没有专门的密钥层与命中统计 |
| `tools/vendor/.../NoLifeWzToNx.cpp` | 已提供 `deduce_key()`： standalone `.img` 可**运行时推导**密钥 | 现网 IMG 路径没走"直读"，把已有能力浪费在"先转 nx"上 |
| `Configuration.h` | `DataFormat`（默认 `wz`）、`DataDir`（默认 `Data`） | 二者语义耦合：只有 `img` 用 `DataDir`，`wz/nx` 只能放 CWD；无 `auto` |

> 一句话：**现在有"分发"，没有"抽象"；有"转换"，没有"适配"；有"试密钥"，没有"密钥层"。**

---

## 2. 总体架构（分层 + 模式映射）

```text
                      业务层 (Character/ IO/ Gameplay/ Data…)
                                 │  依然使用 nl::nx::<Root>[...]
                                 ▼
        ┌─────────────────────────────────────────────────────┐
        │  ResourceService   【外观 Facade + 单例】            │  ← 唯一收口
        │  init() / root(RootId) / diagnostics() / reload()    │
        └─────────────────────────────────────────────────────┘
             │ 选择             │ 定位                │ 解密/查 key
             ▼                 ▼                     ▼
   ┌───────────────┐  ┌──────────────────┐  ┌────────────────┐
   │ DataSource    │  │ ResourceLocator  │  │ Keyring        │
   │ Strategy 注册 │  │ 责任链 + 探测    │  │ KeyResolver    │
   │ 表/工厂       │  │                  │  │                │
   └───────┬───────┘  └──────────────────┘  └────────────────┘
           │ IDataSource（策略接口）
     ┌─────┴──────┬──────────────┬──────────────┐
     ▼            ▼              ▼              ▼
 ImgBackend    WzBackend     NxBackend      (XmlBackend 预留/可选)
 【适配器】    【适配器】     【适配器】      【预留插槽】
 img 直读      nl::wz 适配    nl::nx 适配
 (虚拟目录树   (复用现有      (加 base-dir
  + 惰性解析)   WzRuntime)    重载)
```

**设计模式与落点**

| 模式 | 在本方案中的角色 | 落点 |
|---|---|---|
| **Strategy（策略）** | 每个物理格式一个可选数据源，`ResourceService` 只依赖接口 | `IDataSource` + `ImgBackend/WzBackend/NxBackend` |
| **Adapter（适配器）** | 把第三方 parser（`nl::nx`、`nl::wz`、vendor `imgdirtonx` 树解析）适配到统一 API | 各 Backend 内部类成员包装 |
| **Facade（外观）** | 对业务层只暴露 `ResourceService`，隔离"选格式/找路径/算密钥"的复杂度 | `ResourceService` |
| **Chain of Responsibility（责任链）** | 候选路径/候选格式按优先级依次探测，首个可用者胜出 | `ResourceLocator`、`DataSourceStrategy` 的 `probe()` 链 |
| **Factory + Registry（工厂 + 注册表）** | 格式自省、按分数选后端，新增格式零改分发逻辑 | `DataSourceFactory`（静态注册表） |
| **Template Method（模板方法）** | 统一后端生命周期：`probe → open → bind_roots → verify → close` | `AbstractDataSource`（非虚基类，仅提供流程骨架） |
| **Proxy / Lazy Proxy（延迟代理）** | `.img` 子节点首次访问才真正解析（虚拟节点代理真实节点） | `ImgBackend` 的 vnode → real node |
| **Bridge（桥接）** | 抽象层的"28 个根"与具体 parser 解耦：后端负责把 root 绑定到 `nl::nx::*` | `RootTable` + 现有 `nl::nx::*` 全局变量 |
| **Null Object（空对象）** | 缺失根返回合法空节点而非失败（现网 BeiDou 数据无 `/Base` 必须容忍） | 沿用 NoLifeNx 语义，后端保证返回空 node |

---

## 3. 目录与文件结构

```text
Util/Resource/
  ResourceService.h/.cpp        # Facade：唯一的对外入口（替代 DataFiles::init 的分发角色）
  IDataSource.h                 # 【策略接口】+ 枚举 + DTO（无实现依赖）
  AbstractDataSource.h          # 【模板方法】通用流程骨架（可选基类，非强制）
  DataSourceFactory.h/.cpp      # 【工厂+注册表】按 probe 分数挑选后端
  RootTable.h                   # 28 个根的标识枚举与绑定/回绑（桥接到 nl::nx::*）
  ResourceLocator.h/.cpp        # 【责任链】候选路径生成 + 布局探测
  ResourceLayout.h              # 探测结果 DTO：{ format, base, hits }
  Keyring.h/.cpp                # 密钥供给（内置表 + 外部密钥 + 推导）+ 命中统计
  KeyResolver.h/.cpp            # 按 archive/file 维度缓存命中 key（消除逐 bitmap 试错）
  Diagnostics.h/.cpp            # 可选：启动自检报告（路径/格式/缺根/key 来源）
  Backends/
    ImgBackend.h/.cpp           # 直读 .img（虚拟目录树 + 惰性解析）★核心新增
    WzBackend.h/.cpp            # 包装现有 nl::wz（迁移 WzFiles.cpp）
    NxBackend.h/.cpp            # 包装 nl::nx（迁移 NxFiles.cpp，支持 base dir）
```

> 迁移完成后 `Util/{DataFiles,ImgFiles,WzFiles,NxFiles}.{h,cpp}` 由 `ResourceService` 收编（见 §7 阶段 4）。

---

## 4. 核心接口草案

```cpp
// Util/Resource/IDataSource.h
#pragma once
#include "../../Error.h"
#include "ResourceLayout.h"

#include <nlnx/node.hpp>

#include <filesystem>
#include <memory>
#include <vector>

namespace ms::res
{
    namespace fs = std::filesystem;

    // 物理格式标识（未来可加 Xml / PackedArchive 等）
    enum class Format { Unknown = 0, Img = 1, Nx = 2, Wz = 3, Xml = 4 };

    // 28 个标准根，顺序与 nl::nx::* 现有全局变量保持一致
    // Base, Character, Effect, Etc, Item, Map, Map001, Map002, Map2, Mob,
    // Mob001, Mob002, Mob2, Morph, Npc, Quest, Reactor, Skill, Skill001,
    // Skill002, Skill003, Sound, Sound001, Sound002, Sound2, String, TamingMob, UI
    enum class RootId : uint8_t { Base = 0, /* … */ UI = 27, Count = 28 };

    // 数据源能力：决定 ResourceService 可做的优化（延迟加载/异步预取等）
    struct Capabilities
    {
        bool lazy_nodes  = false;   // 子树是否按需解析
        bool lazy_bitmap = false;   // 位图是否按需解码
        bool audio_stream= false;   // 音频是否指向归档/文件无需整包读入
        bool needs_write = false;   // 是否会产生中间文件（转换缓存）
    };

    // 探测分数：让"自动选择"有明确优先级依据，而不是拍脑袋的顺序
    struct ProbeResult
    {
        bool   usable   = false;
        int    score    = 0;                // 同格式多候选时取高者
        fs::path base;
        std::vector<fs::path> roots_hit;    // 命中根文件清单（供诊断）
        std::vector<RootId>  roots_missed;  // 缺失根（允许缺失，仅诊断用）
    };

    // 【策略接口】任一数据源必须实现的契约
    class IDataSource
    {
    public:
        virtual ~IDataSource() = default;

        virtual Format        format() const noexcept = 0;
        virtual const char*   name()   const noexcept = 0;   // "img" / "wz" / "nx"

        // 1) 自省：给定候选根目录，判定是否是本后端可处理的数据
        virtual ProbeResult   probe(fs::path const& candidate) const = 0;

        // 2) 装载：绑定 28 个根到 RootTable（内部同时刷新 nl::nx::* 全局变量）
        virtual Error         open(OpenParams const& params) = 0;

        // 3) 访问：返回根节点（缺失根返回空 node，沿用 NoLifeNx 空对象语义）
        virtual nl::node      root(RootId id) const = 0;

        virtual Capabilities  caps()   const noexcept = 0;
        virtual void          close() = 0;
    };
}
```

```cpp
// Util/Resource/ResourceService.h  【外观 Facade】
#pragma once
#include "IDataSource.h"

namespace ms::res
{
    class ResourceService
    {
    public:
        static ResourceService& get();

        // 替代现有 ms::init() 中的 DataFiles::init() 调用点（唯一收口）
        Error init();

        // 统一根访问接口（未来替换 nl::nx::* 的迁移目标，当前仅内部使用）
        nl::node root(RootId id) const { return active_ ? active_->root(id) : nl::node{}; }

        // 诊断：格式、命中路径、命中/缺失根、密钥来源统计（INFO 级一行，DEBUG 级明细）
        std::string diagnostics(bool verbose) const;

        // 预留：reload-if-changed / 数据源热切换（本版只预留接口）
        // Error reload();

    private:
        std::unique_ptr<IDataSource> active_;   // 当前生效后端
        ResourceLayout               layout_;   // 本次命中的布局
    };
}
```

**关键点：为什么读写仍是 `nl::nx::*`？**
现有调用点（`Character/Look/*.cpp`、`IO/UITypes/*.cpp`、`Gameplay/**`）数以千计直接使用 `nl::nx::Map` 等全局 `node`。
本方案通过 **`RootTable` 桥接**：后端 `open()` 成功后再把这些根**一次性赋值**到 `nl::nx::*` 全局变量
（`nl::wz::load_all()` 已经在这么做），业务层零改动；`ResourceService::root()` 只是给未来渐进迁移准备的正规通道。

---

## 5. 零转换：IMG 后端怎么做（`ImgBackend`）

### 5.0 先明确基线：现有三后端是"惰性"还是"一次性"？

三者并**不一致**，这是本次设计的基准事实（证据见下表）：

| 后端 | 元数据（节点树 / 字符串表） | payload（bitmap 像素 / audio） | 代码证据 |
|---|---|---|---|
| **NX** | **惰性**（文件映射 + 随机读） | 惰性解码 | `includes/NoLifeNx/nlnx/file.cpp:65-101`：`CreateFileMappingW` + `MapViewOfFile`（非 Win 为 `mmap`）；`nx::load_all()` 只建立 28 个映射 |
| **WZ** | **一次性全量**（但不落盘） | 惰性解码 | `Util/WzRuntime.cpp:105-106`：`wztonx(file, true, false)` 构造不解析 → `parse_file()` 顺序全读建表；**不调用** `open_output()/write_nodes()/write_bitmaps()`，故无中间文件；`decode_bitmap()` 在真正取图时才跑 |
| **IMG** | **一次性全量（最贵）** | 惰性（下游是 NX） | `Util/ImgFiles.cpp:106-228`：递归扫描 16 根全部 `.img` → `convert_img_files_to_nx` 全量转换 → 落 `.nxcache` → 再 `nl::nx::load_all()` |

由此确定本方案的**改动边界**：

- **保留不动**：NX 的映射式惰性；WZ 与 IMG 的 **payload 惰性解码**（bitmap/audio 按需）。这层逻辑原样继承。
- **改掉的**：只有 IMG 那条"整包目录 → 写 `.nx` → 再 load"的转换链。
- **改成的**：**目录索引一次性**（只扫文件名，不读内容）+ **每个 `.img` 元数据按需解析**（走到 `UI/Login.img` 才解析该文件，密钥由 vendor 现有 `deduce_key()` 推导）+ payload 惰性。
- **净收益**：行为向 WZ 这个"零转换样板"对齐并再懒一层（WZ 元数据仍是一次性全量，IMG 变为 per-file 惰性）；`Map / Map001 / Map002` 从 `fs::copy_file()` 的物理复制降级为**视图别名**，少一份中间文件。

> 结论（D1）：采用 **I2 惰性解析 + 虚拟目录树**，payload 惰性语义保持不变。

### 5.1 备选粒度

现状 `ImgFiles::init()` 的路径是"转 .nx → 再 load"。零转换有两种实现粒度：

### 方案 I1：一次性内存化（简单，内存高）
启动时把 `Data/<Root>/**/*.img` 全部解析进内存的 node/string/bitmap/audio 数组，直接构造
`node::data` 兼容结构；**不落任何中间文件**。

- 优点：实现最接近现有 `convert_img_files_to_nx` 流水线（把"写盘"换成"写内存"），迁移成本最低
- 缺点：常驻内存 ≈ `.nx` 体积量级（数十 MB 起），启动耗时 ≈ 首次转换

### 方案 I2：惰性解析 + 虚拟目录树（推荐，内存可控）
仿照 `Util/WzRuntime.cpp` 对 `wz_file` 的做法：为整个 IMG 目录做一个 `img_archive` provider。

1. **index 阶段（廉价）**：遍历目录，只为"目录/文件"建立虚拟节点（vnode），不读内容
2. **materialize 阶段（按需）**：当业务首次访问某个 `*.img` 子节点时才真正解析
   —— 复用 vendor 的 property 解析 + **`deduce_key()` 运行时推导密钥** + 文件映射读内容
3. **缓存**：已解析子树常驻；bitmap 仅在真正绘制时解码；audio 指向文件映射偏移量
4. **Map 特殊处理保留**：现有 `Map001/Map/Map002` 三方语义在 vnode 层做"视图映射"
   （同一批 helper 文件在 Map 与 Map001 两个根下各暴露一份，**不复制数据**，只做别名），
   同时去掉现在 `fs::copy_file(map001nx → mapnx)` 的物理复制

- 优点：按需付费，内存随"实际用到的资源"增长；Map 三方重组从"文件复制"降级为"视图别名"
- 缺点：需要为惰性节点设计稳健的生命周期（扁平 vector + 稳定索引，禁止 vector 重分配导致悬垂）

### 方案 I3：转换缓存降级为**可选加速器**（兜底）
保留 `.nxcache` 但默认关闭；只有 `ImgCache: on` 时才生成，并由**内容指纹**（文件数 + mtime + 大小）
决定是否复用，指纹不符则自动重建，杜绝"脏缓存导致画面异常"。

> **推荐组合**：`I2`（默认） + `I3`（可选开关，便于老机器/冷启动较慢的环境）。

**冷启动开销抑制**：vnode 索引阶段建议只做目录级的惰性 index（必要时先命中必要的少数根，如 UI/String），避免启动即递归 `stat` 上万个小文件成为瓶颈。

---

## 6. 自适应路径（`ResourceLocator`）

### 6.1 候选路径责任链（从高到低）

| 优先级 | 来源 | 说明 |
|---|---|---|
| 1 | 命令行 / 环境变量：`--data-dir=`、`%XY_DATA_DIR%` | 运维/联调临时指定，最高优先 |
| 2 | 部署配置 `xy-config.yaml`：`data.dir`、`data.format` | 复用现有 xy-config 热更机制 |
| 3 | `Settings`：`DataDir` / `DataFormat` | 用户级配置 |
| 4 | **可执行文件所在目录**（`GetModuleFileNameW`，Windows） | 摆脱对 CWD 的依赖 ★ |
| 5 | exe 目录的一级子目录白名单：`Data/`、`data/`、`wz/`、`Wz/` | 常见分发布局 |
| 6 | exe 目录的**父目录**（用于 `MXD/BeiDou-Client-xy/bin/*.exe` 这类布局） |  |
| 7 | 当前工作目录（CWD） | 兼容现有行为 |

> 每个候选根内部还做**布局归一化**：如果候选根直接躺平 `.wz/.nx` 文件 → `flat28` 布局；
> 如果存在 16 个标准根子目录 → `imgdir16` 布局；两者并存时按 `DataFormat` 显式值或分数决定。

### 6.2 自动识别（新增 `DataFormat = auto`）

```cpp
// ResourceLocator::resolve() 伪码
for (candidate in candidates)                   // 责任链
    for (backend in factory().all())            // 策略链，按 probe() 分数降序
        if (auto r = backend.probe(candidate); r.usable)
            best = max(best, r);                // 评分：根命中数优先，其次布局完整性
open(best.backend, best.base);
```

`root hit count` 决定目的地选择的主要原因：部署包经常**只带部分根**（如无 `Base`），
以"命中数量+"而不是"全齐"为准则，符合 `NxFiles::init()` 目前采用的宽松校验（缺根 = 空节点不致命）。

`auto` 与显式配置取值的关系：
- `DataFormat = auto`（默认建议）：走上面的自动选择
- `DataFormat = img|nx|wz`：**绑定**到指定后端，但仍让 `Locator` 在候选路径链中找它的位置
- `DataFormat` 其它值：保持现有 `Error::Code::DATA_FORMAT`（可重试）

### 6.3 为什么必须改很少的第三方代码

`nl::nx::load_all()`（`includes/NoLifeNx/nlnx/nx.cpp`）与 `nl::wz::load_all()`
（`Util/WzRuntime.cpp`）都把文件名写死为相对路径。两种可选改法：

- **A. 加 `base_dir` 重载**：给 `nx::load_all(fs::path const&)` / `wz::load_all(fs::path const&)`
  加一个可选参数并在内部拼路径（**推荐**，改动面 ≈ 十几行，`nx.cpp` 已参与编译）
- **B. 装载期临时切 CWD**：现有 `ImgFiles::init()` 已经在做，但有全局副作用（多后端并发装载不安全）

推荐 A；只有在不允许动第三方代码时才退回 B（**决策点 D3**）。

---

## 7. 密钥层设计（`Keyring` + `KeyResolver`）

### 7.1 三层供给

| 层 | 来源 | 说明 |
|---|---|---|
| L1 内置表 | `key_bms` / `key_gms` / `key_kms`（`tools/vendor/.../Keys.cpp`） | 现状已有，收编进 `Keyring` 统一管理与计数 |
| L2 运行时推导 | vendor `deduce_key()`（独立 `.img` / 部分 WZ 变体） | 推导结果**按文件缓存**，避免重复计算 |
| L3 外部密钥 | 配置指定路径的密钥文件（**默认关闭**） | 便于私服新版本/改版密钥，无需重新打包客户端 |

### 7.2 缓存消除"逐 bitmap 试密钥"

现状 `WzRuntime::decode_bitmap()` 每幅 Canvas 试 4 个候选 key，而 bitmap 解码正是热点路径。
改为 `KeyResolver`：

```cpp
struct KeyResolution { uint8_t const* key; char const* origin; };  // origin: "builtin:gms" / "deduced" / "external"
class KeyResolver {
public:
    // per-archive / per-file 粒度缓存；命中一次后本数据源内不再试错
    std::optional<KeyResolution> resolve(std::string const& archive_or_file_id);
    void note_success(id, key_ptr, origin);
    void note_failure(id, key_ptr);              // 黑名单，避免重复失败
    size_t builtin_hits() const; size_t deduced_hits() const;  // 供 diagnostics()
};
```

预期收益：把"每幅图 ≤4 次解密试探"降为"每个归档 ≤1 次 + 极少数例外回退"。

### 7.3 合规与配置

- **明文密钥不入库**：外部密钥只在**运行时从本地文件路径**读取，配置项里只放路径，不写入 key 内容
- 新增配置：`KeyMode = auto|builtin|external`、`KeyFile = <本地路径>`（两者都可选，缺省行为与现状等价）
- 日志只输出**密钥来源与命中次数**（如 `key=bms(1201)/gms(7)/deduced(33)`），不输出任何密钥字节
- 新增 `Error::Code`（**仅增不删**）：`Error::Code::KEY_FILE`（外部密钥文件不可读/格式错误，**可重试**）

---

## 8. 错误语义与可观测

| 场景 | 现有 | 本方案 |
|---|---|---|
| 格式值非法 | `DATA_FORMAT`（可重试） | 保持 |
| 所有候选路径都没找到可用数据 | `MISSING_FILE`（可重试） | 保持；**错误信息里带出已尝试的完整候选路径链**（排障关键） |
| 根缺失（如无 Base） | 容忍，空节点 | 保持；计入 `diagnostics()` 的 `roots_missed` |
| 后端装载抛异常 | `NLNX` / `WZ`（不可重试） | 保持 |
| UI 版本校验失败 | `WRONG_UI_FILE`（可重试） | 保持，并上移到 `ResourceService` 作为**统一校验步骤**（不再在三个后端里各写一遍） |
| 外部密钥文件异常 | — | 新增 `KEY_FILE`（可重试） |

**日志（贴合仓库现状：Release 下 `LOG()` 是 no-op，且 Windows 子系统无控制台）**

- INFO（上线保留）：一行摘要 —— `format / base_path / roots(hit/miss) / key hits / elapsed_ms`
- DEBUG：候选路径逐个尝试结果、缺根清单、`deduce_key` 推导记录
- **禁止**在最终代码中保留 `[DBG-*]` 临时日志；临时调试日志在功能闭环时清完并做一次全局检索

---

## 9. 决策表（已确认项 + 剩余待定）

### 9.1 已确认

| # | 决策点 | 结论 | 落地要求 |
|---|---|---|---|
| **D1** | IMG 零转换粒度 | **I2 惰性解析 + 虚拟目录树**；payload（bitmap/audio）惰性解码逻辑**原样保留**（详见 §5.0） | 只去掉"整包目录 → 写 `.nx` → 再 load"的转换链；NX 的 mmap 惰性不动；I3 作为**可选**加速器保留（默认关闭） |
| **D2** | `DataFormat` | **新增 `auto` 并设为默认** | 显式 `img/nx/wz` 仍可用（绑定到指定后端，但仍在候选路径链里找位置）；非法值保持 `DATA_FORMAT`（可重试） |
| **D4** | 密钥治理 | **内置三表 + 运行时推导 + per-archive 缓存 + 外部密钥文件** | `KeyMode = auto\|builtin\|external`、`KeyFile = <本地路径>`，默认关闭；明文不入库；日志只报来源与命中次数 |
| **D5** | 验证方式 | **引入轻量测试 target** | 为 `locator` / `probe` / `keyring` 三层加单测；同时新增离线探针工具（见 §9.2）承担"不依赖服务端"的启动验证 |

### 9.2 由 D5 引出的必要补充：离线探针工具 `resource_probe`

**问题（当前排障阻塞）**：`ms::init()` 的第一步是 `Session::get().init()`（`MapleStory.cpp:36`），
必须有 8484 端口的真实握手，否则进程在 `DataFiles::init()` **之前**就退出。
因此**现有环境下资源加载链路无法单独验证**。

**解法**：新增 `tools/resource_probe` 目标（exe），只做：

```text
Configuration::get()  →  ResourceService::get().init()  →  打印 diagnostics(verbose)  →  抽样解析若干节点
```

它**不碰网络、不建窗口**，因此：

- 三种数据布局（img / nx / wz）的正向加载可以在**没有服务端**的情况下完成验证
- 负向用例（目录清空、`DataFormat=bad`、外部密钥文件损坏、资源放到非 CWD 目录）全部可自动化
- 与 D5 的单测 target 共用一个测试/工具构建面，改动一次性付清

> 建议保留到交付物里（不放完后删除），它对后续任何"资源加载类"排障都有长期价值。

### 9.3 已确认（第二轮）

| # | 决策点 | 结论 |
|---|---|---|
| **D3** | 第三方源码改动 | **允许，仅加重载**：`nx::load_all(base_dir)` / `wz::load_all(base_dir)` 可选参数，原 `load_all()` 语义与行为不变 |
| **D6** | 旧四件套处理 | **P4 阶段直接删除** `Util/{DataFiles,ImgFiles,WzFiles,NxFiles}.{h,cpp}`，只保留 `Util/Resource/*` 单一入口 |
| **D7** | 验证分工 | **用户全权负责画面类验证**（本机 `E:\MXD\BeiDou-Client-xy`，需 8484 服务端）；AI 负责交付构建产物 + 本地可跑的单元 / `resource_probe` / 构建三类验证与脚本 |
| **—** | 开工确认 | **确认，执行 P0 + P1**（P0 探针工具与测试骨架 → P1 行为等价的接口抽象重构） |

---

## 10. 分阶段落地计划

> 已按 §9.1 的 D1 / D2 / D4 / D5 结论编排；D3 / D6 / D7 见 §9.3。

| 阶段 | 内容 | 性质 | 闭环标志（可验收） |
|---|---|---|---|
| **P0** | 新增 `tools/resource_probe` 目标 + 轻量测试 target 骨架（CMake + vcxproj 同步） | 基建 | probe 能打印现状三布局的诊断；测试 target 能编能跑 0 fail |
| **P1** | 抽 `IDataSource` / `AbstractDataSource` / `DataSourceFactory` / `RootTable` / `ResourceService`；WZ、NX 后端迁移为适配器；`DataFiles::init()` 转发 Facade | 纯重构，**行为等价** | `resource_probe` 对 img（仍转 cache）/ nx / wz 三布局输出与改造前一致的挂载结果 |
| **P2** | `ResourceLocator` + 布局探测 + `auto` 模式 + `base_dir` 重载（D3-A）+ 错误信息带候选路径链 | 功能新增 | 资源放到非 CWD 目录也能找到（probe 验证）；故意挪走数据 → 报错含完整尝试过的路径链 |
| **P3** | `ImgBackend`（I2：虚拟目录树 + per-`.img` 惰性解析 + `deduce_key()`）+ `Keyring` / `KeyResolver` + per-archive 缓存 + `KeyMode/KeyFile` | 核心目标 | **删除 `.nxcache` 后仍能以 `img` 挂载成功**（probe 验证）；Map/Map001/Map002 走视图别名不再产生复制件；key 诊断显示每归档命中 ≤1 次试探 |
| **P4** | `diagnostics()` 完善 + 文档归档 + 按 D6 处理旧四件套 + 清理临时调试日志 | 收尾 | 构建通过；全局检索无 `[DBG-*]` 残留；`tools/build_smoke.bat` → `EXITCODE=0` |

**验证门禁**

| 层级 | 项目 | 执行方式 |
|---|---|---|
| 单元 | locator 候选链优先级、`probe()` 分数、`KeyResolver` 命中/黑名单缓存、惰性节点生命周期 | 测试 target，本地可跑 |
| 工具 | 三布局挂载、负向路径、缺根容忍、`KeyFile` 损坏、`DataFormat=bad` | `resource_probe`，**本地可跑（不需服务端）** |
| 构建 | `tools/build_smoke.bat`（MSBuild Release/x64）+ CMake 配置 | 本地可跑 |
| 画面 | img / nx / wz 三模式启动到登录界面；login + 选角色 + 进图截图 | **需服务端 8484，由你本机执行** |
| 性能 | img 惰性后端 vs 旧转换链路：冷启动耗时、常驻内存 | `resource_probe --perf` + 你的本机实跑对比 |

---

---

## 11. 风险与回滚

| 风险 | 影响 | 缓解 |
|---|---|---|
| 惰性节点的生命周期（悬垂指针） | 渲染随机崩溃 | 节点存储用**稳定分区 + 索引**，禁止向已有 vector 追加（`deque`/分块），并对 vector 重分配做静态断言/单测 |
| `base_dir` 改动第三方 `nx.cpp` | nx 后端回归 | 改动限制为新增重载；原 `load_all()` 语义不变，老路径代码仍可工作 |
| IMG 目录结构差异（部署包缺子目录、大小写混写） | 找不到资源 | probe 采用"分数制 + 大小写不敏感匹配"（现有 `iequals` 逻辑可复用） |
| 内存增长 | 低配机器 | 默认惰性（I2）；提供资源 preload 黑名单/上限保护 |
| 多个 `.img` 资源版本混杂 | 资源错乱 | `.nxcache`（I3）带内容指纹；诊断输出 base_path，便于快速发现 |

**回滚**：P1–P4 均可世代向前推进；任一阶段异常时，保留旧四件套作为回退路径（若选 D6 过渡封装），或直接回滚上月 commit。

---

## 12. 实施记录（本轮）

### 12.1 已落地

| 项 | 内容 | 状态 |
|---|---|---|
| P0 | `Util/Resource/ResourceProbe.h/.cpp` 离线探针与自检；`MapleStory.cpp` 增加 `--resource-probe [--verbose]` / `--resource-selftest`；`Error::get_code()` 只读访问器 | 已验证 |
| 配置层级化 | `Configuration.h/.cpp`：42 个配置项改为小写点分层级键名；Settings 与 xy-config.yaml 共用单一缩进解析器（`parse_config_file`）；Save 按 `SETTINGS_ORDER` 输出带分组注释的缩进结构 | 已验证 |
| 两个 bug 修复 | `ui.position.optionmenu` 不再误用 `ui.position.userlist`（此前导致 Settings 中该键重复并互相覆盖）；空值不再被丢弃（`login.account:` 可正确读回/保存） | 已验证 |
| 部署配置重写 | `E:\MXD\BeiDou-Client-xy\Settings`（用户级）与 `xy-config.yaml`（部署级）按新格式直接切换，保留原生效值；两者均 UTF-8 无 BOM + LF | 已验证 |
| UTF-8 全量 | 5 个源码文件去除 BOM（自有源码 BOM 清零）；CMake 为 GCC/Clang/MinGW 增加 `-finput-charset=UTF-8 -fexec-charset=UTF-8`；配置读取跳过 BOM、写出不带 BOM；新增 `.editorconfig` | 已验证 |
| 注释增强 | 配置项逐个补充含义/取值/默认值注释；Settings 模板自带头注释（生成时机、优先级、编码）；`Configuration` 类、`get_url` 三级回退、解析器等关键处补充中文注释 | 已完成 |
| P1 接口抽象 | 新增 `Util/Resource/{IDataSource,RootTable,Backends,ResourceService}`；`Img/Wz/Nx` 三个适配器实现统一策略接口；`MapleStory.cpp` 改走 `ResourceService` 门面；旧分发器 `Util/DataFiles.{h,cpp}` 删除（避免双入口） | 已验证 |
| P2 自适应路径 | 新增 `Util/Resource/ResourceLocator.{h,cpp}`：候选链（环境变量 `XY_DATA_DIR` → 配置 `data.dir` → exe 同目录 → `Data/` → 父目录 → `父目录/Data` → CWD → `CWD/Data`，去重）+ 在候选链上按 `probe()` 分数评分选后端；`DataFormat=auto` 与显式 `img/nx/wz` 均经此定位 | 已验证 |
| P2 base_dir 重载 | 第三方 `nl::nx::load_all(base_dir)`、`nl::wz::load_all(base_dir)` 加可选重载（前缀机制，原 `load_all()` 语义不变，D3 授权）；`WzFiles/NxFiles/ImgFiles::init(base_dir)` 同步加可选参数，空值回退既有行为 | 已验证 |
| P2 门面接入 | `ResourceService::init()` 改为先 `locate()` 再装载；未知 `data.format` 报 `DATA_FORMAT`；找不到可用源报 `MISSING_FILE` 并附带尝试过的完整路径链；新增 `base()` 暴露命中目录；`Configuration` 的 `data.format` 默认值由 `wz` 改为 `auto`（D2） | 已验证 |
| P2 探针增强 | `--resource-probe [--verbose]` 增加 `-- locator --` 段：打印候选链与 `locator picks`（格式/分数/命中 base），以及 `base` 行；`ResourceLocator.cpp` 已注册到 vcxproj/filters | 已验证 |

> 说明：D6 约定的"删除旧四件套"提前完成了 `DataFiles` 一项（其路由职责已被门面完整取代，保留会形成双入口）；
> `ImgFiles / WzFiles / NxFiles` 仍作为适配器背后的实现保留，按计划在 P3/P4 处理。

### 12.2 验证结论（本机实跑）

```
构建：tools/build_smoke.bat → EXITCODE=0（MSBuild Release/x64）

自检：MapleStory.exe --resource-selftest → 10/10 PASS，0 failure

探针：MapleStory.exe --resource-probe（写 _resource_probe.log）
  DataFormat         : img            ← 来自新的层级配置 data.format
  img subdirectories : 16
  result             : NONE           ← 装载成功
  ui root children   : 20 (root bound)
  service            : format=img backend=img roots=17/28
                       caps=[lazy-bitmap audio-stream writes-cache] missing=11

配置文件写回：客户端退出后按新格式重写 Settings，原值全部保留；
  E:\MXD\BeiDou-Client-xy\Settings      → UTF-8 无 BOM，LF，2780 字节
  E:\MXD\BeiDou-Client-xy\xy-config.yaml → UTF-8 无 BOM，LF，1899 字节
```

### 12.3 关键发现（影响后续阶段）

1. **img 数据的 UI 根缺少 `Login.img/WorldSelect/BtChannel/layer:bg`**（探针实跑确认 `ui root children: 20` 但版本校验节点为 none）。
   因此 **P3 不能把 UI 版本校验统一上移到门面**，否则 img 模式会直接 `WRONG_UI_FILE` 而无法启动；
   校验必须继续由各后端自行决定（当前仅 wz / nx 校验，img 不校验）。
2. **转换缓存实际占用 6.5GB**（`Data/.nxcache` 22 个 .nx）—— 佐证"零转换 IMG 后端（P3）"的现实收益，不只是架构洁癖。
3. **项目默认标准为 C++14**：资源层头文件不得使用 C++17 特性（嵌套命名空间定义、`<filesystem>`），
   否则被 `MapleStory.cpp` 等 C++14 翻译单元包含时会编译失败。`Util/Resource/IDataSource.h` 已刻意只依赖 C++14，
   路径统一用 `std::string`，由各适配器实现（C++17）内部转换。
4. **`nl::node::size()` 对未绑定（空）节点不安全**：探针曾因此崩溃，需先判 `if (node)` 再取 size。

### 12.4 下一阶段待办（P3 / P4）

- **P3**：`ImgBackend` 改为 I2 零转换（虚拟目录树 + per-`.img` 惰性解析 + `deduce_key()`，`Map/Map001/Map002` 改视图别名）；`Keyring` / `KeyResolver` + `KeyMode` / `KeyFile`
- **P4**：删除 `Util/{ImgFiles,WzFiles,NxFiles}`；`diagnostics()` 完善；若需要再评估独立测试/探针 exe target

### 12.5 本轮（P2）验证结论（实跑）

```
构建：tools/build_smoke.bat → EXITCODE=0（MSBuild Release/x64，重编 ResourceLocator/ResourceService/ResourceProbe 等）

探针（data.format=img 显式时）：
  DataFormat         : img
  img subdirectories : 16
  -- locator --
  candidates         : 4
    [0] E:\MXD\BeiDou-Client-xy\Data
    [1] E:\MXD\BeiDou-Client-xy
    [2] E:\MXD
    [3] E:\MXD\Data
  locator picks      : img (score=15, base=E:\MXD\BeiDou-Client-xy\Data)
  -- load --
  result             : NONE
  ui root children   : 20 (root bound)
  service            : format=img backend=img roots=17/28
                       caps=[lazy-bitmap audio-stream writes-cache] missing=11
  base               : E:\MXD\BeiDou-Client-xy\Data

等价性说明：base 命中结果与旧实现一致（img 仍从 Data 目录装载），
但新增价值在于——今后把数据放到 exe 父目录、或设置 XY_DATA_DIR、或放到非 CWD 目录，
都能被自动定位，不再依赖"工作目录恰好是部署目录"这一隐含假设。
```

> 画面类验证（登录界面、选角色、进图截图）需 8484 端口服务端，由用户在 `E:\MXD\BeiDou-Client-xy` 执行。
