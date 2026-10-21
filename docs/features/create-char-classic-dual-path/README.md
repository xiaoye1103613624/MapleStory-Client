# 创建角色闪退：北斗经典双路径

> 状态：已闭环  
> 适用客户端/协议版本：BeiDou / Journey 双路径（v83 经典 RaceSelect + NewChar*；现代 RaceSelect_new + CustomizeChar）  
> 项目名称：MapleStory-Client  
> 作者/日期：2026-09-24  
> 产物目录：docs/features/create-char-classic-dual-path/

## 1. 背景与目标

- **问题**：选角界面点「创建」后客户端闪退。
- **根因**：`UIRaceSelect` 硬读 `Login.img/RaceSelect_new`；北斗 UI.nx 无此节点，`order` 为空后访问 `order[0..5]` 越界。后续创建页依赖的 `CustomizeChar` / `createLimit` / `MakeCharInfo[000|1000|2000]` 同样缺失。
- **目标**：双路径——有现代资源走 Journey UI；否则走北斗经典三职业 + `NewChar*`，打通种族选择 → 外观/命名 → `CREATE_CHAR`。
- **不做**：不移植高版本 `RaceSelect_new` 资源包；不改服务端建角协议字段。

## 2. 影响范围

| 层级 | 路径/模块 | 变更类型 |
| --- | --- | --- |
| 客户端 | `Util/LoginUiAssets.*` | 新增探测与 MakeCharInfo 映射 |
| 客户端 | `IO/UITypes/UIRaceSelect.*` | 经典/现代双路径 |
| 客户端 | `IO/UITypes/UICommonCreation.*` | 绑定 NewChar* / Gender / signboard |
| 客户端 | `IO/UITypes/UILoginNotice.cpp` | ClassConfirm 缺资源时 Notice 兜底 |
| WZ/NX | 无修改 | 仅消费北斗已有节点 |

## 3. 最终实现逻辑

### 3.1 主流程

```
BtNew → use_classic_race_select()?
  ├─ 否 → init_modern(RaceSelect_new) → UIClassConfirm → Creation(CustomizeChar)
  └─ 是 → init_classic(RaceSelect) → enter_creation()
            → use_classic_creation()?
                 ├─ 是 → NewChar* + Gender + MakeCharInfo(Info/Premium/Orient)
                 └─ 否 → CustomizeChar 现代板
            → NameCharPacket → CreateCharPacket(job 1/0/2)
```

### 3.2 节点绑定

| 现代 | 经典（北斗） |
| --- | --- |
| `RaceSelect_new` | `RaceSelect`（normal/knight/aran + BtSelect） |
| `CustomizeChar/000\|1000\|2000` | `NewChar` / `NewCharKnight` / `NewCharAran` |
| `genderSelect` | `Login.img/Gender` |
| `Map/.../CustomizeChar/...` | `Map/Obj/login.img/NewChar/signboard/{0,1,2}` |
| `MakeCharInfo[000\|1000\|2000]` | `Info/Char*` / `PremiumChar*` / `OrientChar*` |

### 3.3 日志

运行目录 `_create_char.log`：`[RaceSelect] mode=classic|modern`、`[Creation] mode=...`、进入 Creation、ClassConfirm 兜底、池为空 ERROR。

## 4. 资源与同步

- 不依赖外服补包；北斗本地 NX 已具备经典路径节点（见 `evidence/nx-probe.txt`）。
- 现代路径资源齐全的客户端仍走 Journey UI，无行为回退。

## 5. 编码与 i18n

- 日志为 ASCII 英文标签；UI 文案来自 NX 贴图，无新增 UTF-8 直出封包。

## 6. 产物清单

| 类型 | 路径 | 说明 |
| --- | --- | --- |
| 文档 | README.md | 本文件 |
| 证据 | evidence/nx-probe.txt | BeiDou NX 存在性探测 |
| 证据 | evidence/ida-race-select-wnd.md | CreateWnd 锚点笔记 |
| 构建 | builds/index.md | 产物路径与时间 |

## 7. 验证记录

- 构建：`MSBuild MapleStory.vcxproj /p:Configuration=Release /p:Platform=x64` → 成功，输出 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
- NX 探测：`RaceSelect_new`/`CustomizeChar`/`MakeCharInfo/000` 缺失；经典节点齐全（见 evidence）
- 手工冒烟（需本地运行）：
  1. 选角点「创建」→ 三职业页，不闪退；日志 `mode=classic`
  2. 选冒险家/骑士团/战神进创建页
  3. 改外观 / 骰子 / 命名 → 发包
  4. ESC/返回可回选角

## 8. 预留、风险与回滚

- 经典 RaceSelect 控件坐标已按 IDA OnCreate 校准（见 `evidence/ida-race-select-wnd.md`）：BtSelect (530,422)；左骑士/中冒险家/右战神。
- `MakeCharInfo` 映射错误会导致非法外观 ID；发包前有空池 ERROR 日志。
- `LoginUiAssets.cpp` 必须先 `#include MapleStory.h`（定义 `USE_NX`），否则 `classic_makechar_*` 会编译进空实现、外观池为空。
- 回滚：还原 `LoginUiAssets`、`UIRaceSelect`、`UICommonCreation`、`UILoginNotice` 相关改动。
