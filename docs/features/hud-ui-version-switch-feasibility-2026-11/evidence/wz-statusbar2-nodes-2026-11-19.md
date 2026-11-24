# WZ/IMG 节点取证：StatusBar2 / UIWindow2 / UIWindow4（192 参考端）

- 取证日期：2026-11-19
- 取证方式：**orange-wz MCP 直连**（`127.0.0.1:10012/mcp`，streamable-http，29 工具），经 `tools/_mcp_call.py` 连通性确认 + 自建单会话脚本 `tools/_wz_forensics.py`（同一会话 `load_files → get_node_tree_json/get_node_detail → unload_all` 收口）。
- 性质：**只读取证**，全程未对任何 `.wz`/`.img` 执行写操作（无 `mutate`/`save_as`/`change_key`）；所有查询后均 `unload_all`。
- 规格：128 字节 `WzAESConstant.DEFAULT_KEY` 的 base64（`tools/_gms_key_128.b64`，`B64_LEN=172`，HEAD `EwAAAFIAAAAqAAAAWwAAAAgA`）。该文件**已存在**，无需从 `_gms_key.b64` 截取生成。IV 按路径分两套：192 新版本分卷用 `AAAAAA==`（WZ_LATEST_IV，4×00），xy live 用 GMS `TSPHKw==`。`exclusive:false`。

---

## 0. 关键结论：哪个目录是"正确的 192 参考端资源"

> **结论：真正的 192 参考端资源 = 怀旧岛 192 整合客户端的 ResMan 分卷 `UI_00N.wz`，路径 `E:\MXD\【2023.8.8江浩生日福利】怀旧岛192整合版\怀旧岛192客户端\冒险岛online192\Data\UI\`。StatusBar2.img/UIWindow2.img/UIWindow4.img 不是散装 .img 文件，而是分别打包在 UI_002.wz / UI_004.wz / UI_005.wz 内的 IMG 节点。**

甄别过程（搜索先于结论，简单可依赖）：

| 候选路径 | 形态 | orange-wz 实测 | 判定 |
|---|---|---|---|
| `怀旧岛192整合版\...\Data\UI\UI_00N.wz` | ResMan 分卷 `.wz` | 零 IV `AAAAAA==` + 128B key **load 成功**，枚举出 `StatusBar2.img`、`StatusBar3.img`（UI_002）、`UIWindow2.img`（UI_004）、`UIWindow4.img`（UI_005）节点，解析全程 `isError:false` | ✅ **正确 192 参考端** |
| `BeiDou-Client-xy\Data\UI\UIWindow2.img`（26102 B） | 散装 `.img` | GMS key 可读，但全树仅 `WorldMap` 一个子节点（WorldMapSearch/BtSearch/BtNaviRegister/BtAutoFly_1/2）——被裁剪，无 MiniMap | ❌ xy live 裁剪包 |
| `BeiDou-Client-xy\Data\UI\UIWindow4.img`（18428 B，= features/resources 同名文件 SHA256 `bd5a4da9…`） | 散装 `.img` | 有 ShortCutUI（top/middle/bottom + btMain + contentsIcon），系 192 迁移产物（见 side-icon-binding.md），非原生 192 全量 | ⚠️ 192 迁移副本（迁移对象，非权威源） |
| `中变修仙端放了\079修仙端\Data\UI\StatusBar2.img`（14937 B） | 散装 `.img` | GMS IV `TSPHKw==` 与 零 IV `AAAAAA==` **均解析失败**：`IMG 解析失败: StatusBar2.img` | ❌ 079 端异 key/异版本，不可用 |
| `BeiDou-Client_S9\Data\UI\UIWindow2.img` | 散装 `.img` | 可读，但根节点为 `Item/ItemProtector/MiracleCube/…`（背包类），与任务目标（MiniMap）无关 | ❌ 非目标资源 |

补充证据：`docs/features/.../notes/r01-parse-summary.md` 明确记载——MapleStoryT（x86 PE32）才是含 `UI/StatusBar2.img/mainBar`、`UIWindow2.img/*`、`UIWindow4.img/*` 路径契约的客户端；但本机 `E:\MXD\反编译IDA\Korean_MapleStoryT` 只有 exe/i64、无 live Data 目录。**在当前机器上，唯一可取证到 task 所列完整节点树的，就是 192 整合端的分卷 UI.wz。** 192 服务端 `MapleServer192/wz/UI.wz/{StatusBar2,UIWindow2}.img.xml` dump 亦交叉印证节点名（StatusBar2.xml 命中 mainBar×48 / quickSlot×5 / chatTarget×2 / gaugeBackgrd×6 / gaugeCover×4 / BtOption / BtKeySetting / BtChannel 等；UIWindow2.xml 命中 MiniMap×54 / BtMin×31 / BtMax×30 / BtBig×26 / BtSmall×12 / BtMap×2 / BtNpc×1 / iconPortal×4 / iconNpc×4 / iconDirection×25 / ListNpc×5），与 orange-wz 实测一致。

分卷定位（search_node 实测）：

| IMG | 所在分卷 | key |
|---|---|---|
| `StatusBar2.img` / `StatusBar3.img` | `UI_002.wz` | IV `AAAAAA==` + 128B DEFAULT_KEY |
| `UIWindow2.img` | `UI_004.wz` | 同上 |
| `UIWindow4.img` | `UI_005.wz` | 同上 |

完整根路径前缀（下表所有节点路径均以此为根）：
`E:\MXD\【2023.8.8江浩生日福利】怀旧岛192整合版\怀旧岛192客户端\冒险岛online192\Data\UI\UI_002.wz`（StatusBar2）
`/UI_004.wz`（UIWindow2）、`/UI_005.wz`（UIWindow4）。节点路径写成 `StatusBar2.img/mainBar/...` 等（相对分卷根）。

---

## 1. StatusBar2.img 节点树（192 / UI_002.wz）

### 1.1 顶层与 mainBar 一级

```
StatusBar2.img
├── starPlanet              [LIST]
├── mainBar                 [LIST]
│   ├── backgrnd            [CANVAS] 1366×85  ARGB4444
│   ├── lvBacktrnd          [CANVAS] 219×32   ARGB4444
│   ├── lvCover             [CANVAS] 215×14   ARGB4444
│   ├── gaugeBackgrd        [CANVAS] 339×31   ARGB4444
│   ├── gaugeCover          [CANVAS] 308×28   ARGB4444
│   ├── chatSpace           [CANVAS] 505×18   ARGB4444
│   ├── chatSpace2          [CANVAS] 505×25   ARGB4444
│   ├── chatEnter           [CANVAS] 457×21   ARGB4444
│   ├── chatCover           [CANVAS] 82×18    ARGB4444
│   ├── chatTarget          [LIST]   （9 个频道标签，见 1.5）
│   ├── chatOpen / chatClose / scrollUp / scrollDown [LIST]
│   ├── BtChat / BtClaim / BtNotice [LIST]
│   ├── notice              [CANVAS] 1×1
│   ├── lvNumber / gauge    [LIST]
│   ├── BtCharacter / BtStat / BtQuest / BtInven / BtEquip / BtSkill / BtKeysetting
│   ├── BtCashShop / BtStarPlanet / BtMenu / BtSystem / BtChannel [LIST]
│   ├── quickSlot / System / Menu [LIST]   （见 1.2/1.3/1.4）
│   ├── aniHPGauge / aniMPGauge / ApNotify / SpNotify / noncombatNotify [LIST]
│   ├── gaugeBackgrdDemon/AB/Zero/Kinesis/Kanna [CANVAS] 339×31（Kanna 339×32）
│   ├── gaugeCoverAB/Kinesis [CANVAS] 308×21
│   ├── aniHPGaugeAB / BtExitDungeon / readyZero / coolTimeReturn
│   ├── BtMapleTalk / Kinesis / BtNPT / BtFarm [LIST]
│   └── （共 55 个一级子节点）
├── chat                    [LIST]
└── mapleTalk               [LIST]
```

### 1.2 Menu 子树（`mainBar/menu`，注意实际大写节点名 `Menu`）

- `Menu/backgrnd/{0,1,2}`：1×1 ARGB4444，origin (0,0)，**均为 `_inlink` → `mainBar/System/backgrnd/{0,1,2}`**（UOL 复用，无独立位图）。
- 按钮清单（每个含 normal/pressed/disabled/mouseOver 四态，每态 1 帧 canvas `0`）：

| 按钮 | 态帧尺寸 | origin | z | 备注 |
|---|---|---|---|---|
| BtItem | 63×25 | (0,0) | 2 | 道具 |
| BtEquip | 63×25 | (0,0) | 2 | 装备 |
| BtStat | 63×25 | (0,0) | 2 | 能力 |
| BtSkill | 63×25 | (0,0) | 2 | 技能 |
| BtQuest | 63×25 | (0,0) | 2 | 任务 |
| BtCommunity | 63×25 | (0,0) | 2 | 社交/社区 |
| BtMSN | 63×25 | (0,0) | 2 | 好友 |
| BtRank | 63×25 | (0,0) | 2 | 排名 |
| BtEvent | 63×25 | (0,0) | 2 | 活动 |
| BtMonsterBattle | 63×25 | (0,0) | 2 | 怪物战斗 |
| BtMonsterLife | 63×25 | (0,0) | 2 | 怪物生活 |
| BtAfreecaTV | 63×25 | (0,0) | 2 | 直播 |
| BtBattleStats | 63×25 | (0,0) | 2 | 战斗统计 |
| BtEpisodBook | 63×25 | (0,0) | 2 | 冒险书 |

（共 14 个 Bt 按钮；每态含 `_hash` 32 字节校验串，四态像素各自独立。）

### 1.3 System 子树（`mainBar/System`）

- `backgrnd`：3 帧 canvas，尺寸分别 `79×1`、`79×34`、`79×41`（ARGB4444）——下拉菜单背景三段（顶/中/底）。
- 按钮（均四态、每态 1 帧、63×25、z=2、origin (0,0)）：
  `BtChannel`、`BtKeySetting`、`BtGameOption`、`BtSystemOption`、`BtOption`、`BtGameQuit`、`BtMonsterLife`、`BtJoyPad`、`BtRoomChange`、`BtCharacterSelect`。

### 1.4 quickSlot 子树（`mainBar/quickSlot`）

| 节点 | 尺寸 | 说明 |
|---|---|---|
| `backgrnd` | **145×93** ARGB4444 | 快捷栏底板 canvas |
| `BtOpen` | 47×13 / 47×15 / 48×15（四态多尺寸） | 展开钮 |
| `BtClose` | 48×15（+1×1 占位），四态 | 收起钮；含 `_inlink` → `mainBar/quickSlot/BtClose/normal/0` |
| 键位槽 | QWER... 数字键由客户端逻辑绑定，节点本体为 145×93 底板 + 按钮 | 原生无独立"键位标签" canvas 节点，键位在图内绘制 |

### 1.5 chatTarget 子树（`mainBar/chatTarget`）

| 节点 | 尺寸 | 节点 | 尺寸 |
|---|---|---|---|
| `base` | 1×1 + 68×21（底） | `guild` | 25×13 |
| `all` | 37×13 | `association` | 25×13 |
| `friend` | 25×13 | `expedition` | 37×13 |
| `party` | 25×13 | `afctv` | 51×12 |
| `mapleTalk` | 31×12 | | |

（9 个标签，各含 3 个子节点：canvas + origin + z。）

### 1.6 gauge 子树（`mainBar/gauge`）

- `number`：15 帧数字图（3×3 / 4×11 / 4×9 / 6×10 / 7×9 / 9×9 ARGB4444）。
- 条类（每类 3 帧，尺寸 1×12 / 1×13 / 2×13 ARGB4444，即条首/中/尾）：
  `hp`、`mp`、`exp`、`relaxExp`、`tf`、`barrier`、`barrier2`、`barrier3`、`tempExp`、`tw_subHp`；
  `df` 为 12 帧（含多状态）。

### 1.7 mainBar 顶层按钮 origin 实测（关键定位数据）

> origin 为相对底栏锚点的**负坐标**（按钮向左/上偏移），`z=2`，四态同 origin；canvas 尺寸为单态帧。

| 按钮 | canvas 尺寸 | origin (x,y) | 对应功能 |
|---|---|---|---|
| BtCharacter | 34×21 | (-82, 58) | 角色（ani 3 帧 30×18，origin -84,57，delay=210） |
| BtStat | 34×21 | (-116, 58) | 能力 |
| BtInven | 34×21 | (-184, 58) | 道具/背包 |
| BtEquip | 34×21 | (见 menu/System 同名按钮) | 装备 |
| BtSkill | 34×21 | (同上) | 技能 |
| BtCashShop | 56×35 | (-57, 36) | 商城（ Cashshop，ani 4 帧含 UOL `2→0`/`3→1`） |
| BtMenu | 56×35 | (-173, 36) | 菜单 |
| BtSystem | 56×35 | (-231, 36) | 系统/设置 |
| BtChannel | 75×35 | (-293, 36) | 频道切换 |
| BtStarPlanet | 56×35 | (同级，未单独展开) | StarPlanet |

各按钮均含 `normal/pressed/disabled/mouseOver` 四态；`BtCharacter/BtStat/BtInven` 另带 `ani` 动画帧（部分 ani 帧以 `_inlink` 指向 `mainBar/BtCharacter/ani/{0,1,2}` 复用）。`BtSystem/BtChannel`（mainBar 顶层）与 `System/BtChannel/BtOption/BtKeySetting`（下拉菜单内）为同名不同层节点，注意区分。

---

## 2. UIWindow2.img/MiniMap 子树（192 / UI_004.wz）

UIWindow2.img 根节点有 **315 个一级子节点**（192 原生完整 UI 集）。`MiniMap` 子树 17 个一级节点、444 行，关键如下（格式：节点 → canvas 尺寸集合，ARGB4444；均含 origin/z 子节点）：

| 节点 | 尺寸 | 说明 |
|---|---|---|
| `MinMap` | 64×27（+1×21 / 1×9 / 9×1 / 1×1 边框） | 小地图（收起底图） |
| `MaxMap` | 64×67（+1×61 / 1×9 / 9×1） | 大地图 |
| `Min` | 64×20 / 1×20 | 小地图底条 |
| `BtMin` | 1×1（四态占位，帧实际透明/1px） | 最小化钮 |
| `BtMax` | 1×1（四态占位） | 最大化钮 |
| `BtBig` | 13×12（四态） | 放大钮 |
| `BtSmall` | 13×12（四态） | 缩小钮 |
| `BtMap` | 39×12（四态） | 地图钮 |
| `BtNpc` | 39×12（四态） | NPC 钮 |
| `iconPortal` | 13×15（+1×1 占位，四态） | 传送点图标 |
| `iconNpc` | 13×15（+1×1，四态） | NPC 图标 |
| `iconDirection` | 17×17 / 18×20 / 20×18（+1×1，32 canvas / 8 子节点，四态×方向帧） | 方向指示箭头（8 方向） |
| `ListNpc` | 64×30 / 64×36（+1×30 / 1×36 / 64×1 / 1×1，9 canvas） | NPC 列表框 |
| `BtDungeonMap` | 55×12（四态） | 副本地图钮 |
| `MinMapMirror` / `MaxMapMirror` / `MinMirror` | 镜像/反向版（65×27 / 68×67 等） | 镜像地图（左右翻转场景用） |

节点路径示例：`UIWindow2.img/MiniMap/BtMin/normal/0`、`UIWindow2.img/MiniMap/iconDirection/{state}/{frame}`、`UIWindow2.img/MiniMap/ListNpc/...`。origin 均为 canvas 子节点 `origin`（VECTOR_PROPERTY），尺寸取 canvas 节点 width/height。

> 说明：`BtMin/BtMax` canvas 为 1×1，属占位/隐藏态（可见像素挂在其 `ani`/链接子帧或由逻辑控制），非取数错误。

---

## 3. UIWindow4.img/ShortCutUI 子树（192 / UI_005.wz）

UIWindow4.img 根节点 124 个一级子节点。`ShortCutUI` 子树 9 个一级节点、92 行：

```
UIWindow4.img/ShortCutUI
├── top       [CANVAS] 353×33  ARGB4444  origin(0,0) z=0
├── middle    [CANVAS] 353×92  ARGB4444  origin(0,0) z=0
├── bottom    [CANVAS] 353×33  ARGB4444  origin(0,0) z=0
├── textBox   [CANVAS] 53×14   ARGB4444  origin(0,0) z=0
├── newIcon   [LIST] 0/1/2/3 → 各 17×17 ARGB4444（新内容角标）
├── default   [CANVAS] 37×37   ARGB4444  origin(0,0) z=0
├── btClose   [LIST] normal/pressed/disabled/mouseOver（四态，1×1 占位）
├── btMain    [LIST] normal/pressed/disabled/mouseOver + ani
│             （四态各 1×1 占位；ani 含 100×20 canvas 帧，origin 见下）
└── contentsIcon [LIST] default / hekaton / bossArena / crossHunter
              （4 图标；实测尺寸 34×37、39×38、43×38 及 1×1 占位）
```

### 3.1 关于"竖栏灰底"——原生是否有底板节点

**有。** `ShortCutUI/top`（353×33）、`middle`（353×92）、`bottom`（353×33）三张 canvas 就是原生竖栏的**背景底板九宫片**（上/中/下），另有 `default`（37×37）为单图标槽底、`textBox`（53×14）说明文字底。均带 `origin(0,0)`/`z=0`/`_hash`。

> 结论供决策：用户要求"去掉竖栏灰底"——原生 192 ShortCutUI **确实自带灰底板**（top/middle/bottom + default）。去掉灰底不是恢复原生，而是对原生资源的**改造**（需明确是隐藏 `top/middle/bottom/default` 还是替换为透明贴图）。若目标是"对齐原生 192 观感"，灰底应保留；若目标是"透明化竖栏"，需在 C++ 侧跳过这些 canvas 或出透明替代图（走 Photoshop MCP，见 skill 门禁 16）。

`btMain` 折叠钮四态 + `ani`（含 100×20 帧）——与 notes/side-icon-binding.md「折叠钮 `ShortCutUI/btMain`（四态）」一致。`contentsIcon` 下 4 个图标槽（default/hekaton/bossArena/crossHunter）——与 notes 中 `UIWindow4.img/ShortCutUI/contentsIcon/%s` 路径契约一致。

---

## 4. 报错留证（原始文本）

遵守"解析失败记录原始报错文本"，逐条保留原文（服务端错误文本为 GBK，工具以 UTF-8 replace 解码后呈现；括号内为语义）：

| # | 场景 | 原始报错文本（原文） | key/IV |
|---|---|---|---|
| 1 | load 079 `StatusBar2.img` 后取树 | `IMG 解析失败: StatusBar2.img` | GMS `TSPHKw==` + 128B key |
| 2 | 同上换零 IV 复测 | `IMG 解析失败: StatusBar2.img` | `AAAAAA==` + 128B key |
| 3 | xy UIWindow2 取根子节点 `nodePath="."` | `节点路径未找到: rootPath=E:/MXD/BeiDou-Client-xy/Data/UI/UIWindow2.img, nodePath=.` | GMS |
| 4 | xy UIWindow2 取 `nodePath="MiniMap"` | `节点路径未找到: rootPath=E:/MXD/BeiDou-Client-xy/Data/UI/UIWindow2.img, nodePath=MiniMap`（证实 xy 包无 MiniMap，被裁剪） | GMS |

- 工具描述列表（`python tools/_mcp_call.py 10012 list`）在 GBK 控制台呈乱码（如 `load_files` 描述显示 `WZ/IMG/XML 文件...` 的 GBK 字节被 UTF-8 解码后的乱码），属显示编码问题，不影响调用；所有成功查询均 `isError:false`。
- 192 分卷 `load_files`（零 IV）与后续所有 `get_node_tree_json`/`get_node_detail` 均 `isError:false`，无解析异常。

---

## 5. 取证脚本与产物索引

| 文件 | 说明 |
|---|---|
| `tools/_mcp_call.py` | 现成 streamable-http MCP 客户端（用户指定） |
| `tools/_wz_forensics.py` | 自建单会话取数脚本（load→query→unload_all 同会话，GBK 输出容错） |
| `tools/_parse_tree.py` / `_summarize.py` | 树 JSON → 节点表/汇总 |
| `tools/_probe_out.json` / `_tree_out.json` / `_sb2_tree_out.json` / `_sb2_detail_out.json` / `_uiw_out.json` / `_final_out.json` | 各轮原始 MCP 响应（含全部 origin/尺寸/帧/_hash/_inlink） |
| `tools/_sb2_summary.json` / `_uiw_summary.json` | 汇总表（按钮/尺寸/origin） |

> 注：以上 `tools/_*.json` 为本轮取证中间产物（含完整节点明细），供复核；如需长期保留建议迁入本 `evidence/` 目录。报告本身只摘关键 origin/尺寸与结论。

## 6. 复盘（四步法）

1. **目标**：定位 192 参考端 StatusBar2/UIWindow2/UIWindow4 并记 origin/尺寸/帧。
2. **结果**：确认权威源是 192 整合端分卷 `UI_00N.wz`（非散装 .img），完成三棵子树取数，origin/尺寸/帧/底板结论齐备；079/xy 候选均证伪或标为迁移副本。
3. **弯路根因**：最初凭散装 .img 文件定位，被 079 解析失败与 xy 裁剪包误导；根因是"192 端用 ResMan 分卷 .wz 而非散装 .img"这一形态差异未被 notes 显式标注——搜索候选目录 + orange-wz 实测 load 后才翻案。
4. **沉淀规律**：跨版本取证先判资源**形态**（散装 .img vs 分卷 .wz）与 **IV 规格**（新版本 `AAAAAA==` vs GMS `TSPHKw==`），再用 `search_node(keyword=...)` 在分卷内定位 IMG；GBK 控制台需 `PYTHONIOENCODING=utf-8` 或对输出做 encode-replace，否则 Unicode 韩文节点名会中断落盘。
