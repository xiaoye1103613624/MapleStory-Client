# C12 foothold `forbidFallDown`（禁止下跳）

> 状态：已闭环  
> 适用：MapleStory-Client（Journey 系）+ BeiDou V17.7 地图数据  
> 日期：2026-11-05  
> 相关：open-issues C12 / R23-A 地图键审计副产物  
> 产物目录：`docs/features/forbid-fall-down/`

## 1. 背景与目标

- **问题**：地图 `foothold/*/forbidFallDown` 数据大量存在（400 图抽样 322 条），客户端全仓库 0 引用，站在禁下跳平台上仍可 ↓+跳穿板。
- **目标**：按原生加载语义接入，把门控接到既有 `enablejd` 下跳路径。
- **不做**：step4 外观面板；真机点测（本轮用户指定不真机）；服务端改动（s9 无该字段逻辑）。

## 2. 影响范围

| 层级 | 路径/模块 | 变更类型 |
| --- | --- | --- |
| 客户端物理 | `Gameplay/Physics/Foothold.{h,cpp}` | 解析 + 访问器 |
| 客户端物理 | `Gameplay/Physics/FootholdTree.cpp` | `enablejd` 门控 |
| 服务端 | — | 无 |
| WZ/IMG | 既有键，只读 | 无写资源 |

## 3. 最终实现逻辑

### 3.1 主流程

1. 进图装 foothold 时读叶子 `forbidFallDown`（`get_bool()`，缺键=false）。
2. `FootholdTree::update_fh` 在重算 `enablejd` 时：若当前 `nextfh.forbidFallDown()` → `enablejd = false`；否则保持原「下方 600px 内有板」规则。
3. `PlayerWalkState` / `PlayerProneState` 既有 `enablejd` 判断自动拒绝 ↓+跳，无需改输入层。

### 3.2 数据路径

```
Map/MapX/<mapid>.img/foothold/<layer>/<chunk>/<fhid>/forbidFallDown = 1
```

### 3.3 证据要点

见 [`evidence/ida-forbidFallDown.md`](evidence/ida-forbidFallDown.md)：`sub_A43E7B` @ `0xA43E7B`，`push 1141h`（string id 4417）→ `!= 0` 布尔写入。

## 4. 资源与同步

- 无新增资源；OutDir 即 `E:\MXD\BeiDou-Client-xy\`，构建直出。
- 占用策略：**B**（结束 `MapleStory.exe` 后覆盖）。本轮不真机启动。

## 5. 编码与 i18n

- 无 UI 文案；注释中文，对齐仓库物理模块近期风格。

## 6. 产物清单

| 类型 | 路径 | 说明 |
| --- | --- | --- |
| 文档 | README.md | 本文件 |
| 证据 | evidence/ida-forbidFallDown.md | IDA + 对照 |
| 路径记忆 | notes/client-path.md | 部署目录 |
| 构建 | builds/index.md | 构建索引 |

## 7. 验证记录

- 构建：`tools/build_smoke.bat`（见 builds/index.md）
- 真机：本轮跳过（用户指定）
- 待测清单见 §8

## 8. 预留、风险与回滚

**待测（真机）：**

1. 带 `forbidFallDown=1` 的平台（如部分 99000xxxx 家族图）↓+跳不应穿板。
2. 同图普通平台仍可下跳（下方 600px 内有板时）。
3. 履带 `force` 平台行为不被回归（R23-A）。

**回滚：** 还原 `Foothold`/`FootholdTree` 三文件即可。
