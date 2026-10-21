# 装备悬停对比（EquipCompare）

> 状态：已闭环（客户端层）
> 适用客户端/协议版本：HeavenClient 系 v83（部署目录 `E:\MXD\BeiDou-Client-xy`）
> 项目名称：MapleStory-Client（XiaoYE MS v0.0.1）
> 作者/日期：萧曳 / 2026-10-21
> 相关提交：见 `git log --grep="装备悬停对比"`
> 产物目录：`docs/features/equip-compare/`

## 1. 背景与目标

- **问题**：在物品栏或商店悬停一件装备时，当前端只会显示该物品自身的 tooltip，无法直接看出「换上它比身上的强多少」。
- **目标**：对齐参考端 `ijl15.dll!EquipCompare` 的核心行为——右侧同时显示同部位已装备物品，并在每条属性后标注两件装备的差值：
  - 正差值（悬停物品更好）绿色 `(+N)`；
  - 负差值（换装会变差）红色 `(N)`。
- **明确不做**：不穷举多部位/双槽饰品与现金装备的 `bodyPart` 组合；不与套装（SetItem）、装备成长（EquipGrowth）伴生 tooltip 做停靠联动；不改装备属性计算与服务端。

## 2. 影响范围

| 层级 | 路径/模块 | 变更类型 |
| --- | --- | --- |
| 客户端 UI | `IO/Components/EquipTooltip.h/.cpp` | 补齐跨物品差值注释、修正预览开关与空槽保护 |
| 配置 | `Configuration.h/.cpp` | 新增 `ui.equip_compare` 开关（默认 `true`） |
| 服务端 / 协议 | — | 无（全部使用已下发的装备属性） |
| WZ/资源 | — | 无（复用 `UIToolTip.img/Item/Frame`） |

## 3. 最终实现逻辑

### 3.1 主流程

1. `EquipTooltip` 构造时读取 `ui.equip_compare`，存入 `show_compare`。
2. `set_equip(parent, invpos)` 在物品栏/商店（`InventoryType::Id::EQUIP`）悬停装备时：
   - 取悬停装备的 `EquipSlot::Id`；
   - 若同部位已有装备，构造右侧对比 tooltip 的全部缓存字段（`*_preview`）；
   - 逐条属性生成主行（已装备值 + 自身卷轴加成）与差值行（悬停值 − 已装备值）。
3. `draw()` 先画右侧对比 tooltip（`draw_preview`），再画悬停物品自身 tooltip，并做屏幕边界回退。
4. 差值文本在 `draw_preview` 中紧跟属性文本绘制，颜色在构造时确定，避免每帧切换字体颜色表。

### 3.2 数据 / 语义

- 对比方向：`diff = 悬停物品属性 − 已装备物品属性`。
  - `diff > 0` → `" (+N)"`，`Color::Name::GREEN`；
  - `diff < 0` → `" (N)"`（含负号），`Color::Name::RED`；
  - `diff == 0` → 不生成差值文本。
- 属性行输出条件：**任一侧非 0**。这样「已装备没有、悬停装备有」的属性也能显示为 `0 (+N)`，与参考端逐属性对比的意图一致。
- 已装备物品自身的卷轴加成仍按 `equip.get_stat(es) − equipdata.get_defstat(es)` 显示在括号内，与普通 tooltip 保持一致；差值文本追加在其后。
- 覆盖属性：`EquipStat::Id::STR … JUMP`（力量、敏捷、智力、运气、HP、MP、物攻、魔攻、物防、魔防、命中、回避、手技、移速、跳跃）。

### 3.3 与参考端的差异

| 参考端 | 当前端 | 原因 |
| --- | --- | --- |
| `FindEquippedCompareItem` 通过 `TryGetBodyPartsForItem` 穷举 bodyPart，并处理现金装备槽偏移 | 按 `EquipData::get_eqslot()` 单槽查找 | 当前端装备模型是单槽位；多部位饰品与双槽现金装备属边缘场景，留作后续 |
| 对比 tooltip 由 `CUIToolTip` 原生实例承载，走 `PlaceCompareTipAt` 与伴生停靠 | 复用 `EquipTooltip::draw_preview` 的既有缓存与右侧绘制 | 当前端没有可复用的原生 `CUIToolTip` 实例，避免引入第二套 tooltip 生命周期 |
| 正差值通过字体颜色表 remap 变绿，负差值用另一套 subtype 颜色 | `Text` 构造时直接指定 `GREEN` / `RED` | 当前端 `Text` 颜色不可变，构造期指定更简单且无每帧开销 |
| 只有装备（itemId 1000000–1999999）触发 | 仅在 `parent == ITEMINVENTORY/SHOP` 且库存类型为 EQUIP 时触发 | 与参考端入口条件等价 |

## 4. 资源与同步

- 无新增 WZ/IMG 节点，无需资源同步。
- 字体沿用 `Text::Font::A11M`，与属性行一致。
- 颜色使用 `Graphics/Color.h` 既有 `GREEN` / `RED` 调色板，无新增色值。

## 5. 编码与 i18n

- 新增文本仅为 `" (+N)"` / `" (N)"`，纯 ASCII 数字与符号，不涉及 GBK/UTF-8 转换。
- 属性名沿用 `EquipStat::names`，随客户端既有语言资源。

## 6. 产物清单

| 类型 | 路径 | 说明 |
| --- | --- | --- |
| 文档 | `docs/features/equip-compare/README.md` | 本文件 |
| 证据 | `docs/features/beidou-v17-parity/notes/r02-hook-table.md` §3 | 参考端函数 EA 与行为 |
| 配置 | `Settings` / `xy-config.yaml` 的 `ui.equip_compare` | 默认 `true` |
| 代码 | `IO/Components/EquipTooltip.h/.cpp` | 差值注释与开关 |

## 7. 验证记录

- **构建**：MSBuild Release / x64 / v142 → `EXITCODE=0`，无 error；产物 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2,548,224 B，2026-10-21 21:04）。
  - 首次链接时客户端正在运行，PostBuildEvent 复制 3 个三方 DLL 失败；已核对 `freetype.dll`、`bass.dll`、`liblz4.dll` 与部署目录哈希一致，改用 `PostBuildEventUseInBuild=false` 复跑得到 `EXITCODE=0`。
- **回归（待执行）**：客户端由用户以管理员权限运行，无法由构建端结束；重启后按以下路径验收：
  1. 登录 → 选角 → 进图 → 打开物品栏；
  2. 悬停同部位另一件装备，确认右侧出现已装备 tooltip，且属性行后出现绿色 `(+N)` / 红色 `(N)`；
  3. 悬停同部位无装备的槽位/物品，确认不出现对比；
  4. `ui.equip_compare=false` 后重启，确认只显示单件 tooltip。

## 8. 预留、风险与回滚

- **预留**：多部位饰品（戒指/吊坠双槽）、现金装备 bodyPart 映射、套装/成长伴生 tooltip 停靠。
- **风险**：右侧对比 tooltip 可能超出屏幕，已有 `adj_x/adj_y` 回退；若同时存在套装/成长面板，可能重叠（当前端未移植伴生面板，暂不触发）。
- **性能**：差值 `Text` 仅在 `set_equip` 时重建，绘制为每行一次额外文本，装备 tooltip 生命期内无逐帧分配。
- **回滚**：`ui.equip_compare=false` 即可关闭；代码层回滚还原 `EquipTooltip.*` 与 `Configuration.*` 的新增。
