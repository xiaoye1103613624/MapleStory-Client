# 交易行 D6A — UI 出图完整 Plan

> **状态**：D6A UI 出图 **已完成**（`ui/*/v1/` 零件 + `assembly/v1` 整窗拼装）· **本 Plan 不含业务代码**（不写客户端/服务端逻辑、不 DDL、不入库 live `.img` 除非另开任务）  
> **规范**：`xiaoye-MapleStory-dev` + `ui-photoshop.md`  
> **设计依据**：`DESIGN.md`（含 R1–R3 已确认）· `notes/ui-wireframes.md`  
> **确认后动作**：开 Photoshop MCP 按本计划出图；确认前 **禁止**调用 Photoshop

---

## 0. 目标与非目标

### 目标

按 **原生 v083 风格 · 1x · 组件化拼装**，为交易行交付可入库的 UI 位图全集：

- 通用小组件（边框九宫格、页签二态、MapleButton 四态、槽位、表头/行、滚动条零件等）
- 拼装整窗预览 PSD（浏览直售 / 浏览拍卖 / 上架双模式 / 已拍 / 记录）+ 出价弹层
- 每组件版本目录：`docs/features/trade-house-083/ui/<组件>/vN/`（PNG + `.psd` + `manifest.md`）
- 多状态按场景自动推断（§9），先 `normal` 再派生

### 非目标（本 Plan 明确不做）

| 不做 | 说明 |
| --- | --- |
| 客户端 / 服务端业务代码 | `UITradeHouse`、Handler、协议数值等另开实现 Plan |
| DDL / DML | 表仅设计稿 |
| Photoshop 出图执行 | **须用户确认本 Plan 后**才开工 |
| orange-wz 写入 live 客户端 | 本里程碑可选「仅归档 PNG」；入库另开或用户明确授权后做 |
| 改面交 / FM / NPC 店 UI | D8 |

---

## 1. 已确认决策摘要（与出图相关）

| ID | 要点 | 对出图影响 |
| --- | --- | --- |
| D5 / D6 | 原生自制窗；先出图不写代码 | 样式默认 **原生**；无参考跟图争议 |
| D9 / R3 | 购买与拍卖成交 **统一入已拍 50 格** | 「已拍」页为一级主路径；Tooltip/空态文案按「领取仓」设计，非溢出提示 |
| R1 | 已拍永不删，仅警告角标 | 须出 `BadgeStashOld`（久存角标） |
| R2 | 防狙击 +30s | UI 无需单独控件；倒计时可能被服务端延长（展示层用 endAt） |
| D10 | NPC + 状态栏入口 | 另出状态栏入口小按钮（四态） |
| D11 | 公开卖家名 | 列表列「卖家」需列宽与字色 |
| D13 | 分类 + 装备筛选 + 列排序 | 分类 Tab、筛选面板底板、可排序表头箭头三态 |
| 主窗基线 | 640×420（筛选展开 640×480） | 出图锁定尺寸写入 manifest |

---

## 2. 开工前索取 / 阻塞项

| # | 项 | 现状 | 动作 |
| --- | --- | --- | --- |
| P1 | **客户端 / Data 绝对路径** | **缺失**（无 `notes/client-path.md`，无 `_native-samples`） | 用户确认 Plan 时一并提供；落盘 `notes/client-path.md` |
| P2 | 原生样板 | 未采集 | 有路径后 orange-wz 采 `shop/` `inventory/` `statusbar/` `dialog/` → `_native-samples/` |
| P3 | Photoshop MCP | 出图时再 ping | 确认 Plan 后会话首次 `photoshop_ping` |
| P4 | 样式 | 默认原生 | 若用户另附参考图 → 先问跟图 or 原生（未答默认原生） |

**无路径时**：可按 ui-photoshop §6 v083 经验基线开工，但 `manifest.md` 必须标「未实测采样」；**优先**索取路径后再采样。

---

## 3. 有序任务（依赖）

```text
T0  用户确认本 Plan + 提供客户端路径（或同意 v083 基线）
      ↓
T1  落盘 notes/client-path.md；orange-wz 导出样板 → docs/features/_native-samples/{shop,inventory,statusbar,dialog}/
      ↓
T2  锁定主窗逻辑尺寸 / 已拍 10×5 / 格 32×32（对照样板微调）→ 写入本 Plan 附录或 wireframes 修订一行
      ↓
T3  小组件生成（按 §4 优先级）· 各组件 ui/<名>/v1/ · manifest
      ↓
T4  状态派生（四态/二态/行态/角标）· preview 验收
      ↓
T5  PSD 组件化拼装整窗 + BidDialog · 终稿区/素材区分层
      ↓
T6  §13 验收清单全过 · 版本目录归档完整
      ↓
T7  （可选）orange-wz 入库预演到旁路路径 — 须用户另授；默认本里程碑只归档
```

依赖说明：T3 依赖 T2 尺寸；T5 依赖 T3–T4 零件齐；T6 阻塞交付。

---

## 4. 组件出图优先级

对齐 `notes/ui-wireframes.md` §6，细化如下：

| 序 | 目录建议 `ui/<组件>/` | 内容 | 依赖 |
| --- | --- | --- | --- |
| 1 | `chrome` | frame 九宫格 nw…se、titleBar、panel/bg、listBg、searchBg、divider、BtClose 四态 | 样板 shop/inventory |
| 2 | `tabs` | MainTab / CategoryTab / ModeTab / RecordSubTab：normal + selected（+可选 mouseOver） | chrome |
| 3 | `list` | ColHeader（unsorted/asc/desc 箭头）、RowNormal/MouseOver/Selected、Scrollbar 上中下块 | chrome |
| 4 | `buttons` | BtSearch / BtBuy / BtBid / BtList / BtCancelListing / BtClaim / BtClaimAll / BtQty± / BtPrevNext / BtFilterToggle / 状态栏入口钮 — **MapleButton 四态** | chrome |
| 5 | `grid` | WonSlotEmpty / WonSlotFrame / WonSlotSelected；`BadgeStashOld`（R1） | inventory 格样板 |
| 6 | `inputs` | Textfield 底板 idle/focus/disabled；Combo 下拉头；数量框 | — |
| 7 | `overlay` | BidDialogFrame + BidDialogBg | chrome |
| 8 | `feedback` | BadgeAuction / BadgeFixed / Countdown 字色参考条 / EmptyState | — |
| 9 | `assembly`（或各整窗 v1） | 拼装：BrowseFixed / BrowseAuction / ListForm / WonStash / Records / BidDialog | 1–8 |

每项交付：`v1/*.png` + 同版 `.psd` + `manifest.md`（含状态列表、组件列表、样板来源、目标节点「待定」可接受）。

---

## 5. 多状态清单（场景推断 · 必出）

| 控件角色 | 契约 | 状态清单 |
| --- | --- | --- |
| 主操作 / 关闭 / 数量 / 翻页 / 状态栏入口 | MapleButton | `normal` → 派生 `mouseOver` / `pressed` / `disabled` |
| 主导航 / 分类 / 模式 / 记录子 Tab | TwoSpriteButton | `normal`（未选）+ `selected`（激活）；有样板悬停则加 `mouseOver` |
| 表头排序 | 热区+箭头图 | `unsorted` / `asc` / `desc` |
| 列表行 | 底图 | `normal` / `mouseOver` / `selected` |
| 已拍格 | 槽 | `empty` / `filled`（框）/ `selected`；久存另叠 `BadgeStashOld` |
| Textfield / Combo 头 | | `idle` / `focus` / `disabled` |
| 倒计时 | 字色参考 | `normal` / `warn(<5min)` / `critical(<1min)` |
| 九宫格 / 纯底板 / 分隔 | 静态 | `State: static`（manifest 标明） |
| 拍卖「下架」 | — | **不出可用态**（不渲染或仅 disabled 示意一张） |

流水线（ui-photoshop §9.2）：AI/绘制只出高质量 `normal`（及静态件）→ PS 规则派生 → `photoshop_get_preview` → 不像原生再单态重生成。

---

## 6. 拼装页面清单

| 预览 PSD 场景 | 须可见要素 |
| --- | --- |
| 浏览 · 直售 | 主 Tab、模式/分类、搜索条、可排序表、购买底栏、金币 |
| 浏览 · 拍卖 | 拍卖列、倒计时 warn、出价钮 |
| 浏览 · 装备筛选展开 | 高级筛选面板（职业/部位/等级/属性） |
| 上架 · 直售 | 模式切换、背包格、时长三档、费用预估、确认上架 |
| 上架 · 拍卖 | 起拍价、时长只读「1 小时」、无下架 |
| 已拍 | 10×5 格、占用 n/50、领取/全部领取、含 1 格久存角标示例（R1） |
| 记录 | 购买/销售子 Tab + 只读行 |
| BidDialog | 当前价、最低加价后、出价输入、冻结提示、取消/确认 |

PSD：**终稿区 + 旁侧素材区**；保留小组件图层组与可编辑文字层（ui-photoshop §10）。

---

## 7. 验收（交付前必过）

对照 ui-photoshop **§13**：

- [ ] scale 均为 **1x**；尺寸与 PNG 一致  
- [ ] 可点/可选控件状态齐全（四态 / 二态等按上表）  
- [ ] 无紫靛渐变 / 玻璃拟态 / 大柔光 / 胶囊 Material（§6.3 禁区）  
- [ ] 九宫格接缝无糊；按下/禁用可辨  
- [ ] 完整图由小组件拼装，非一张烤死；PSD 分层可改  
- [ ] 每组件 `ui/<名>/vN/` 含 PNG + `.psd` + `manifest.md`  
- [ ] 客户端路径已落盘或 manifest 标「未实测」  
- [ ] R1 角标、R3「已拍领取」信息架构在已拍页预览中体现  

---

## 8. 产物路径

```text
docs/features/trade-house-083/
  PLAN-ui.md                 # 本文
  DESIGN.md                  # 玩法/UI IA 依据
  notes/
    client-path.md           # T0/T1 落盘（开工前索取）
    ui-wireframes.md
  ui/
    chrome/v1/
    tabs/v1/
    list/v1/
    buttons/v1/
    grid/v1/
    inputs/v1/
    overlay/v1/
    feedback/v1/
    assembly/v1/             # 整窗拼装预览 + 源 PSD
docs/features/_native-samples/
  shop/ inventory/ statusbar/ dialog/ …
```

禁止覆盖旧 `vN`；改版递增。

---

## 9. 风险与回滚

| 风险 | 缓解 |
| --- | --- |
| 无客户端路径导致观感偏差 | 优先索取路径；否则基线 + manifest 标注；有路径后 v2 校准 |
| 整窗一次 AI 烤死难改 | 强制小组件优先（§10）；禁止默认真烤整窗 |
| 四态外形漂移 | 只出 normal 派生；单态重生成带差分 prompt |
| 主窗 640 与样板冲突 | T2 对照 shop 微调并文档锁定 |
| 范围膨胀（业务代码） | 本 Plan 门禁：只出图；实现另开 Plan |
| Photoshop / 占用冲突 | 出图不写 live `.img`；入库另议 |

回滚：保留 `v1` 不动；不合格出 `v2`，不删旧版。

---

## 10. 用户确认清单（回复即可开工）

请确认或修正后回复，例如「**确认 PLAN-ui，客户端路径：E:\…\Data**」：

1. 同意本 Plan 范围（仅 UI 出图，不含业务代码）  
2. 样式：原生 1x（默认）  
3. 客户端 Data 路径（或明确同意先用 v083 经验基线）  
4. （可选）是否本里程碑结束后做 orange-wz 入库预演  

**确认前不调用 Photoshop。**
