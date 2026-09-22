# 三格式数据源加载矩阵（img / nx / wz）

> 本文件描述客户端三种数据源后端的**选择方式、数据放置规格、错误语义**，以及
> **静态核验结论**与**Windows 实测清单**。
>
> 运行时分发逻辑见 `Util/DataFiles.cpp`：`DataFiles::init()` 依据
> `Configuration::get().get_data_format()` 三值匹配 `img` / `nx` / `wz`，
> 未知值返回 `Error::Code::DATA_FORMAT`（可重试）。

---

## 1. 选择方式

`DataFormat` 取自 `Settings` 文件或部署配置 `xy-config.yaml`（优先）。

| 取值 | 后端 | 默认 |
|---|---|---|
| `img` | `ImgFiles::init()` | 否 |
| `nx`  | `NxFiles::init()`  | 否 |
| `wz`  | `WzFiles::init()`  | **是**（保留既有直读 .wz 行为） |

未知取值（如拼写错误）不会静默回退到 wz，而是明确报错并允许「改 Settings 后 retry」。

---

## 2. 各格式数据规格与放置位置

### 2.1 img（`DataFormat=img`）

- 配置项 `DataDir`（默认 `Data`）：指向**经典独立 .img 目录**，其下应有 16 个标准根子目录
  `Base/ Character/ Effect/ Etc/ Item/ Map/ Mob/ Morph/ Npc/ Quest/ Reactor/ Skill/ Sound/ String/ TamingMob/ UI`。
- 启动行为（`ImgFiles::init()`）：
  1. 若 `Data/` 不存在 → `MISSING_FILE`（可重试）。
  2. 在 `Data/.nxcache/` 下把各根目录的 `.img` 转换为 `.nx`（首次昂贵，后续命中缓存跳过）。
  3. `Map` 根被重组成 `Map001.nx`（helper）/ `Map.nx`（同 helper 副本）/ `Map002.nx`（按 `Map/Map<prefix>/<id>.img` 重组的逐图档案）。
  4. 顶层散落 `.img`（smap/StandardPDD/zmap）也各自成为 nl::nx 根。
  5. `nl::nx::load_all()` 加载生成的 `.nx`，缺失根作为空节点（不致命）。
- **实测确认点**：启动后 `Data/.nxcache/` 生成对应的 `*.nx`（含 `Map001/Map/Map002`）。

### 2.2 nx（`DataFormat=nx`）

- 将 NoLifeNx 的 **.nx 全集**放在**客户端工作目录**（即 exe 所在目录）。
- `NxFiles::init()`：
  1. 若「一个根都没有」→ `MISSING_FILE`（可重试）。
  2. `nl::nx::load_all()`；异常 → `NLNX`（不可重试）。
  3. UI 版本校验：`UI.nx` 中 `Login.img/WorldSelect/BtChannel/layer:bg` 须为 bitmap，否则 `WRONG_UI_FILE`（可重试）。
- **实测确认点**：用 2.1 产出的 `.nxcache/*.nx` 拷到客户端根、设 `DataFormat=nx` 启动，登录链路（标题/世界选择/频道）正常。

### 2.3 wz（`DataFormat=wz`，默认）

- 将 **16 个标准 .wz** 放在**客户端工作目录**：
  `Base.wz Character.wz Effect.wz Etc.wz Item.wz Map.wz Mob.wz Morph.wz Npc.wz Quest.wz Reactor.wz Skill.wz Sound.wz String.wz TamingMob.wz UI.wz`。
- `WzFiles::init()`：
  1. 若「一个 .wz 都没有」→ `MISSING_FILE`（可重试）——**负向验证核心**。
  2. `nl::wz::load_all()`；异常 → `WZ`（不可重试）。
  3. 同样的 UI 版本校验 → `WRONG_UI_FILE`（可重试）。
- **负向验证确认点**：工作目录无 .wz 时启动应**明确报错、不崩溃、可 retry**，而非静默或崩溃。

---

## 3. 错误码矩阵（retry 语义）

| 错误码 | 触发 | 可重试 |
|---|---|---|
| `DATA_FORMAT` | DataFormat 取值不在 {img,nx,wz} | 是（改 Settings 后 retry） |
| `MISSING_FILE` | img 目录缺失 / nx 无根 / wz 无根 | 是 |
| `NLNX` | img 转换或 nx load_all 抛异常 | 否 |
| `WZ`   | wz load_all 抛异常 | 否 |
| `WRONG_UI_FILE` | UI 版本不匹配（缺 Post-Chaos bitmap） | 是 |

---

## 4. 静态核验结论（本仓库已完成代码层核验）

- `DataFiles::init()` 三值显式匹配 + 未知值 `DATA_FORMAT` 明确报错 ✓
- `Error.h` 已含 `DATA_FORMAT` 枚举、`messages` 文案、`can_retry` 列入 ✓
- `WzFiles` / `NxFiles` / `ImgFiles` 均始终编译（移除 `USE_NX` 对 NxFiles 的死门控；WzFiles 已去 `#ifndef USE_NX`）✓
- `WzNxBootstrap` 死代码已**移除**（无调用者、门控宏未定义、引用不存在枚举）✓
- 调试残留（39 处 `dbg_stage` / `[DBG-IMG]` / `TEMP DEBUG` / 未用 `ofstream`）已清理 ✓

---

## 5. Windows 实测清单（需在 Windows + 游戏服环境执行）

1. **wz 负向**：清空工作目录 .wz → 启动 → 确认报 `MISSING_FILE` 且可 retry，无崩溃。
2. **img 正向**：放好 `Data/` → `DataFormat=img` 启动 → 确认 `Data/.nxcache/*.nx` 生成（含 Map001/Map/Map002）→ 登录链路截图。
3. **nx 正向**：把 2 产出的 `.nx` 拷到客户端根 → `DataFormat=nx` 启动 → 登录链路截图。
4. **wz 正向**：放齐 16 个 .wz → `DataFormat=wz`（默认）启动 → 登录链路截图。
5. **DATA_FORMAT 负向**：设 `DataFormat=bad` → 启动 → 确认 `DATA_FORMAT` 报错且可 retry。
6. 每次变更后跑构建：`tools/build_smoke.bat` 须 `EXITCODE=0`，`git status` 干净。

> 注：客户端目录运行时产物（`Data/.nxcache/`、旧 `_start_debug.log`、生成的 `.nx`）属本地缓存/日志，
> 已被 `.gitignore` 忽略；如需回收磁盘，手动删除 `Data/.nxcache/` 即可（~20MB 量级）。
