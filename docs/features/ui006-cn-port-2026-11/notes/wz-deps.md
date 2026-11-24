# UIWindowCN.img 依赖调研：CN_Limitbreakthrough / CN_50100

- 调研日期：2026-11-21
- 性质：**只读**。仅读取导出的 node tree JSON、192 客户端 `Data/` 目录、通过 orange-wz MCP 做 `load_files` / `search_node` / `get_node_detail` / `get_node_tree_json` 查询；所有会话以 `unload_all` 收口。未修改任何既有文件，未对 `.wz/.img` 做任何写入/`save_as`。
- 客户端根：`E:\MXD\【2023.8.8江浩生日福利】怀旧岛192整合版\怀旧岛192客户端\冒险岛online192`
- 源包：`Data\UI\UI_006.wz` → `UIWindowCN.img`
- 树证据：`docs/features/ui006-cn-port-2026-11/evidence/uicn_full_tree.json`（`name=UIWindowCN.img`, `type=IMAGE`, 8 个顶层节点；`CN_Limitbreakthrough` 7 子节点、`CN_50100` 28 子节点）
- 调研脚本：授权创建 `tools/_recon_192deps.py`（只读分析导出树）；MCP 实解析/字符串检索通过复用 `tools/_orange_mcp.py` 的一次性只读脚本完成（已用完删除，未改动任何既有文件）。

---

## 0. 验证口径 / key 规格

按仓库规范 AGENTS.md，优先走 orange-wz MCP（`http://127.0.0.1:10012/mcp`，server `orange-wz-http-mcp v1.157.49`，29 工具），直连脚本复用 `tools/_orange_mcp.py`。

```
key = {"name":"gms128",
       "ivBase64":"AAAAAA==",
       "userKeyBase64": open("E:/project/MapleStory-Client/tools/_gms_key_128.b64").read().strip()}
```

- 该 128B userKey（B64 长 172，HEAD `EwAAAFIAAAAqAAAAWwAAAAgA`）为 `WzAESConstant.DEFAULT_KEY` 规格，供 orange-wz 路径使用；iv 为 192 零 IV `AAAAAA==`。
- `load_files` 全程 `exclusive:false`；全部加载成功，`rootCount/loadedCount` 正常，未出现 `IMG 解析失败` / `Index N out of bounds` / GBK 报错文本等解析异常。**因此本轮无需 `.nx` 回退**。
- 注意：MCP 回显的 rootPath 中文为 GBK 字节按 UTF-8 replace 解码的乱码（如 `MXD\...`），属显示层问题，不影响按 `rootPath`(实际路径字符串) + `nodePath` 定位节点；所有目标节点均实际 `RESOLVED`。

---

## 1. 两个节点的完整子节点清单 + 链接引用

图例：尺寸 `W×H`；`origin=(x,y)`；`z` 为层级；`_outlink`=外部 img 引用，`_inlink`=同 img 内引用。所有 CANVAS 均为 `ARGB4444`，除特别标注。

### 1.1 CN_Limitbreakthrough（突破石）

直接子节点 **7** 个；后代合计 231 节点（LIST 15 / CANVAS 48 / VECTOR 48 / INT 40 / STRING 80）。

| # | 子节点 | 类型 | 尺寸 | origin | z | state/帧 | 备注 |
|---|--------|------|------|--------|---|----------|------|
| 1 | backgrnd | CANVAS | 196×285 | (0, 1) | -5 | — | 主底板 |
| 2 | backgrnd2 | CANVAS | 186×256 | (-5, -21) | -4 | — | 内底板 |
| 3 | backgrnd3 | CANVAS | 176×34 | (-10, -26) | -3 | — | 标题/顶条 |
| 4 | backgrnd4 | CANVAS | **1×1 占位** | (-10, -151) | -3 | — | `_outlink = UI/UIWindow2.img/Cube_InGame1/backgrnd4` |
| 5 | Effect | LIST | — | — | — | 3 组 ×12 帧 | Normal / GradeUp / Use |
| 6 | BtOk | LIST | — | — | — | 4 state ×1 帧 | normal/pressed/disabled/mouseOver |
| 7 | BtOnemore | LIST | — | — | — | 4 state ×1 帧 | normal/pressed/disabled/mouseOver |

Effect 三组的帧数组：

| 组 | 帧数 | 帧 origin | 帧本体尺寸 | 链接 |
|----|------|-----------|------------|------|
| Effect/Normal | 12（0..11） | 全部 (-10,-62) | 176×86（本地实体帧） | 无链接（内嵌实体 12 帧） |
| Effect/GradeUp | 12（0..11） | 0..8=(-10,-26)，9..11=(-10,-62) | 1×1 占位 | 0..10 → `UI/UIWindow2.img/Cube_Red/Effect/GradeUp/{0..10}`；11 → `.../Effect/Normal/11` |
| Effect/Use | 12（0..11） | 全部 (-10,-62) | 1×1 占位 | 0..10 → `UI/UIWindow2.img/Cube_Red/Effect/Use/{0..10}`；11 → `.../Effect/Normal/11` |

**CN_Limitbreakthrough 的 `_outlink` 全量（共 32 条）**：backgrnd4 ×1 + GradeUp ×12 + Use ×12 + BtOk ×4 + BtOnemore ×4 = 33 引用（去重路径数：GradeUp 11 + Normal/11 + Use 11 + BtOk 4 + BtOnemore 4 + backgrnd4 1 = **32 条唯一目标**）。无 `_inlink`。目标分布：
- `UIWindow2.img/Cube_InGame1`：backgrnd4、BtOk×4、BtOnemore×4
- `UIWindow2.img/Cube_Red`：Effect/GradeUp/{0..10}、Effect/Use/{0..10}、Effect/Normal/11

### 1.2 CN_50100（魔女森林）

直接子节点 **28** 个；后代合计 1074 节点（LIST 75 / CANVAS 263 / VECTOR 250 / INT 219 / STRING 267）。

| # | 子节点 | 类型 | 尺寸 | origin | z | state/帧 | 备注 |
|---|--------|------|------|--------|---|----------|------|
| 1 | backgrnd | CANVAS | 803×713 | (0,0) | -4 | — | 主底板 |
| 2 | button:Help | LIST(5) | — | — | — | 4 state×1 帧 | id=1000 |
| 3 | button:Close | LIST(5) | — | — | — | 4 state×1 帧 | id=1 |
| 4 | Number | LIST | — | (0,0) 起 | — | 10 帧(0..9) | 数字图集 10×12 左右 |
| 5 | Timer | LIST | — | 各异 | — | 11 帧(0..9 + ':') | 计时数字 |
| 6 | layer:reward50Bg0 | CANVAS | 151×116 | (-81,-401) | 0 | — | 实体 |
| 7 | layer:reward50Bg1 | CANVAS | 151×116 | (-237,-401) | 0 | — | 实体 |
| 8 | layer:reward50Bg2 | CANVAS | 101×111 | (-81,-520) | 0 | — | 实体 |
| 9 | layer:reward50Bg3 | CANVAS | 1×1 | (-184,-520) | 0 | — | `_inlink → CN_50100/layer:reward50Bg2` |
| 10 | layer:reward50Bg4 | CANVAS | 1×1 | (-287,-520) | 0 | — | `_inlink → CN_50100/layer:reward50Bg2` |
| 11 | layer:reward100Bg0 | CANVAS | 1×1 | (-406,-401) | 0 | — | `_inlink → CN_50100/layer:reward50Bg0` |
| 12 | layer:reward100Bg1 | CANVAS | 1×1 | (-562,-401) | 0 | — | `_inlink → CN_50100/layer:reward50Bg1` |
| 13 | layer:reward100Bg2 | CANVAS | 101×111 | (-406,-520) | 0 | — | 实体 |
| 14 | layer:reward100Bg3 | CANVAS | 1×1 | (-509,-520) | 0 | — | `_inlink → CN_50100/layer:reward100Bg2` |
| 15 | layer:reward100Bg4 | CANVAS | 1×1 | (-612,-520) | 0 | — | `_inlink → CN_50100/layer:reward100Bg2` |
| 16 | layer:miracleTime | CANVAS | 200×130 | (-58,-231) | 1 | — | 实体 |
| 17 | button:50 | LIST(5) | — | (-108,-636) | 1 | 4 state×1 帧 | 126×31，id=1001 |
| 18 | button:50X10 | LIST(5) | — | (-239,-635) | 1 | 4 state×1 帧 | 128×33，id=1002 |
| 19 | button:100 | LIST(5) | — | (-431,-636) | 1 | 4 state×1 帧 | id=1003；pressed/disabled/mouseOver 为 `_inlink → button:50/*` |
| 20 | button:100X10 | LIST(5) | — | (-566,-635) | 1 | 4 state×1 帧 | id=1004；pressed/disabled/mouseOver 为 `_inlink → button:50X10/*` |
| 21 | Grade | LIST | — | 无 origin 字段 | — | 5 帧(0..4) | 尺寸 23×13/32×13/33×13/32×22/42×22 |
| 22 | Type | LIST | — | 无 origin 字段 | — | 5 帧(0..4) | 尺寸 20×20/20×20/26×26/38×38/41×41 |
| 23 | specialRewardCover0 | CANVAS | 159×123 | (0,0) | 0 | — | 实体 |
| 24 | specialRewardCover1 | CANVAS | 109×119 | (0,0) | 0 | — | 实体 |
| 25 | popUp | LIST | — | — | — | 4 子节点 | backgrnd 394×148(0,0,z1) + 3 组按钮 |
| 26 | Result | LIST | — | — | — | 20 子节点 | 奖励结果弹层（见下） |
| 27 | button:Notice | LIST(5) | — | (-683,-51) | — | 4 state×1 帧 | 14×15，id=1005 |
| 28 | Notice | LIST | — | — | — | 4 子节点 | backgrnd 410×641(0,0,z-4) + button:Close + $scroll + $slot_contents |

**CN_50100 链接引用全量：**

`_outlink`（**11 条唯一外部目标**，全部 `UI/StatusBar3.img/chat/common/scroll/...`）：
- `.../enabled/prev0`, `prev1`, `prev2`
- `.../enabled/next0`, `next1`, `next2`
- `.../enabled/thumb0`（thumb1、thumb2 也各自 `_outlink → .../thumb0`，即 thumb1/thumb2 复用 thumb0）
- `.../disabled/prev`, `.../disabled/next`

`_inlink`（同 img 内部，共 30 条引用 / 22 条唯一目标）：
- reward slot 复用：`layer:reward50Bg3/4 → layer:reward50Bg2`；`layer:reward100Bg0/1 → reward50Bg0/1`；`layer:reward100Bg3/4 → reward100Bg2`
- 按钮复用：`button:100/{pressed,disabled,mouseOver}/0 → button:50/*`；`button:100X10/{pressed,disabled,mouseOver}/0 → button:50X10/*`
- 弹窗复用：`popUp/button:confirm2/{normal,pressed,mouseOver,disabled}/0 → popUp/button:confirm/*`
- 结果层复用：`Result/layer:backgrnd → Result/backgrnd`；`Result/EffectSS/{52,62,63,64} → Result/Effect/*`；`Result/layer:rewardBg{0,1,8,9,10} → Result/rewardSlot/0`；`Result/layer:rewardBg{3,4,5,6,7} → Result/layer:rewardBg2`

**整个 UIWindowCN.img 的外部 `_outlink` 目标 img 汇总**（含其它 6 节点，供移植依赖评估）：`UIWindow2.img`（Cube_InGame1 / Cube_Red / Incubator / SportsDay）、`UIWindow4.img`（blackCirculator）、`UIWindow6.img`（intro/animation:loading）、`StatusBar3.img`（chat/common/scroll）、`CashShop.img`（PicturePlate2）。

---

## 2. Outlink 目标在 192 客户端的落点 + MCP 实解析验证

### 2.1 各目标 img → 192 WZ 落点（search_node 实测）

| 被引用的 img | 所在 192 WZ | MCP `search_node` 证据 |
|---|---|---|
| UIWindow2.img | `Data\UI\UI_004.wz` | 返回 `nodePath=UIWindow2.img, type=IMAGE`（同时 `Cube_InGame1`/`Cube_Red` 均为 `LIST_PROPERTY`） |
| StatusBar3.img | `Data\UI\UI_002.wz` | 返回 `nodePath=StatusBar3.img, type=IMAGE` |
| UIWindow4.img | `Data\UI\UI_005.wz` | `nodePath=UIWindow4.img, type=IMAGE` |
| UIWindow6.img | `Data\UI\UI_005.wz` | `nodePath=UIWindow6.img, type=IMAGE` |
| CashShop.img | `Data\UI\UI_000.wz` | `nodePath=CashShop.img, type=IMAGE` |
| UIWindowCN.img（源） | `Data\UI\UI_006.wz` | 由导出树确认 |

### 2.2 逐目标 `get_node_detail` 实解析（key 同 §0，全部 `exclusive:false`）

以下 24 个抽样/全量代表点**全部 RESOLVED**（返回 `type=CANVAS_PROPERTY`），**无任何解析报错**：

| 来源 | nodePath（去掉前缀 `UI/`） | 宿主 WZ | 结果 |
|---|---|---|---|
| LB/backgrnd4 | `UIWindow2.img/Cube_InGame1/backgrnd4` | UI_004.wz | RESOLVED |
| LB/BtOk/normal/0 | `UIWindow2.img/Cube_InGame1/BtOk/normal/0` | UI_004.wz | RESOLVED |
| LB/BtOk/pressed/0 | `UIWindow2.img/Cube_InGame1/BtOk/pressed/0` | UI_004.wz | RESOLVED |
| LB/BtOnemore/normal/0 | `UIWindow2.img/Cube_InGame1/BtOnemore/normal/0` | UI_004.wz | RESOLVED |
| LB/Effect/GradeUp/0 | `UIWindow2.img/Cube_Red/Effect/GradeUp/0` | UI_004.wz | RESOLVED |
| LB/Effect/GradeUp/9 | `UIWindow2.img/Cube_Red/Effect/GradeUp/9` | UI_004.wz | RESOLVED |
| LB/Effect/GradeUp/11 | `UIWindow2.img/Cube_Red/Effect/Normal/11` | UI_004.wz | RESOLVED |
| LB/Effect/Use/0 | `UIWindow2.img/Cube_Red/Effect/Use/0` | UI_004.wz | RESOLVED |
| LB/Effect/Use/10 | `UIWindow2.img/Cube_Red/Effect/Use/10` | UI_004.wz | RESOLVED |
| Jiaozi/TeamIcon/blue/0 | `UIWindow2.img/SportsDay/TeamIcon/blue/0` | UI_004.wz | RESOLVED |
| Incubator 系列 | `UIWindow2.img/Incubator/5060029/{backgrnd2,backgrnd4,Effect/11}`、`5060046/backgrnd`、`5060047/backgrnd` | UI_004.wz | RESOLVED（5/5） |
| 50100/Notice scroll | `StatusBar3.img/chat/common/scroll/enabled/{prev0,thumb0}`、`disabled/{prev,next}` | UI_002.wz | RESOLVED（4/4） |
| CN_Cash_Web | `CashShop.img/PicturePlate2/BtOK/{normal,disabled}/0` | UI_000.wz | RESOLVED（2/2） |
| CN_Web1/intro | `UIWindow6.img/intro/animation:loading/{0,15}` | UI_005.wz | RESOLVED（2/2） |
| CN blackCirculator | `UIWindow4.img/blackCirculator/BtX/normal/0` | UI_005.wz | RESOLVED |

**重点：`CN_Limitbreakthrough/backgrnd4` 的 `_outlink = UI/UIWindow2.img/Cube_InGame1/backgrnd4`**

- 在本地树里是 1×1 占位（origin -10,-151, z -3），`_hash=40f7f646...feb5db9`。
- MCP `get_node_tree_json`（UI_004.wz）解析该目标：真实节点 `origin=(-10,-151)`, `z=-3`, `size=176×97`, `pngFormat=ARGB4444`, `_hash=40f7f646...feb5db9`，与本地占位节点的 origin/z/hash 完全一致 → 目标**确实存在且可解析**，占位解析后应还原为 176×97 实体图。
- 顺带验证 `Cube_InGame1/BtOk/normal/0` 为 40×16 CANVAS（对应本地 1×1 占位 outlink），`Cube_Red/Effect/Normal/11` 为 CANVAS。

**解析错误记录：无。** 所有 load 与 find/get 均以正常 JSON 返回；未出现 AGENTS.md §2 中列出的任何解析异常文本，故本轮不触发 `.nx` dump 回退。

---

## 3. 192 客户端 Data 目录 String/Quest/UI 相关 WZ 清单 + 中文字符串节点

### 3.1 .wz 清单（实际枚举 `Data/` 下）

```
Data/UI/    UI.wz(63B 索引) + UI_000.wz … UI_008.wz   (9 个分片, 30~98 MB)
Data/String/String.wz(63B 索引) + String_000.wz (13.9 MB, 唯一分片)
Data/Quest/ Quest.wz(63B 索引)  + Quest_000.wz  (21.6 MB, 唯一分片)
```

（其余 `Base/Character/Effect/Etc/Item/Map/Mob/Morph/Npc/Reactor/Skill/Sound/TamingMob` 等目录本轮不展开。）

### 3.2 中文字符串节点（MCP `search_node(searchIn=value)` 实测，中文关键词以 `\u` 转义传入避免编码歧义）

`String_000.wz`（`rootPath=Data\String\String_000.wz`）内可用 img：`Etc.img / Npc.img / Map.img / Skill.img / UI.img / Consume.img / Cash.img / AdventureLogbook.img` 等。

| 关键词 | 命中数 | img/nodePath 代表 | 解码后的真实中文值（节选） |
|---|---|---|---|
| 魔女森林 | 3 | `Cash.img/5220034/desc` | 「在布满珍贵宝物的神秘**魔女森林**中帮助探险的向导妖精。在**魔女森林**中探险时，可以获得#c特别的道具#。」 |
| 魔女森林 | 同上 | `Consume.img/2630806/name` | **魔女森林地图** |
| 魔女森林 | 同上 | `Consume.img/2630806/desc` | 「布满珍贵宝物的神秘**魔女森林**的地图。…必须有的向导妖精才能进行探险。」 |
| 突破石 | 123 | `Cash.img/5532894/name` | 「100万**突破石** 30%」 |
| 突破石 | 同上 | `Cash.img/5532904/name` | 「50万**突破石** 100% 交换卷」 |
| 突破石 | 同上 | `Cash.img/5538014/name` | 「500万**突破石**100% 交易券」 |
| 魔女 | 331 | `Cash.img/5121006/name` 等 | 小魔女、向导妖精相关大量道具/任务文案 |
| 森林 | 1559 | `AdventureLogbook.img/text/...` 等 | 泛「森林」文本（噪声多） |

`Quest_000.wz`（`rootPath=Data\Quest\Quest_000.wz`）：顶层 img 有 `Quest.img`、`QuestInfo.img`、`Check.img`、`PQuest.img` 等。

| 关键词 | 命中数 | img/nodePath 代表 | 解码后的真实中文值（节选） |
|---|---|---|---|
| 魔女森林 | 0 | — | 全词精确命中为 0（仅「魔女」「森林」分散命中） |
| 突破石 | 16 | `QuestInfo.img/16235/{0,1,2}` | 「…只要登录就能获得可获得的液和结晶…还能获得**突破石**奖励…」 |
| 突破石 | 同上 | `QuestInfo.img/65101/name` | 「[活动] 首个武器突破石」；`QuestInfo.img/65102/name` 「[活动] …增加**突破石**…」 |
| 魔女 | 177 | `QuestInfo.img/2086/2`、`2870/1`、`3075/name` 等 | 「…药草、魔女的叶…」「**魔女**森林相关」… |
| 森林 | 1310 | `Check.img/.../conditionContent`、`PQuest.img/1324/info/desc` 等 | 泛「森林」文本 |

**结论**：中文字符串**可以正常解析**（MCP 返回 UTF-16/GBK 解码后的正确 Unicode，仅终端 codepage 显示为乱码；以 JSON unicode-escape 验证值正确）。「魔女森林」精确词只出现在 `String_000.wz` 的 `Cash.img/5220034/desc`、`Consume.img/2630806/{name,desc}`；「突破石」主要在 `String_000.wz` 的 `Cash.img/*`（123 处）与 `Quest_000.wz` 的 `QuestInfo.img/*`（16 处）。候选 img 文件路径：`Data\String\String_000.wz`、`Data\Quest\Quest_000.wz`。

---

## 4. 布局摘要表（origin / 数量）

说明：坐标均取自 `uicn_full_tree.json` 各 CANVAS 的 `origin` 子节点。按钮 4 个 state 的帧 origin 基本一致（mouseOver 略放大并微调 2~3px）。魔女森林窗口底板 803×713，突破石底板 196×285；按钮 origin 多为相对窗口中心的负坐标。

### 4.1 CN_50100（魔女森林）

| 节点 | state/帧 | 数量 | normal origin | 尺寸(W×H) | z | 备注 |
|---|---|---|---|---|---|---|
| button:Help | normal/pressed/disabled/mouseOver | 4×1帧 | (-698, -49) | 12×15（mouseOver 18×20 @ -695,-47） | — | id=1000 |
| button:Close | 4 state | 4×1帧 | (-712, -50) | 15×15（mouseOver 19×19 @ -710,-48） | — | id=1 |
| button:50 | 4 state | 4×1帧 | (-108, -636) | 126×31 | 1 | id=1001，4 帧同 origin |
| button:50X10 | 4 state | 4×1帧 | (-239, -635) | 128×33 | 1 | id=1002 |
| button:100 | 4 state | 4×1帧 | (-431, -636) | normal 126×31；其余 1×1 | 1 | id=1003；pressed/disabled/mouseOver `_inlink→button:50/*` |
| button:100X10 | 4 state | 4×1帧 | (-566, -635) | normal 128×33；其余 1×1 | 1 | id=1004；pressed/disabled/mouseOver `_inlink→button:50X10/*` |
| button:Notice | 4 state | 4×1帧 | (-683, -51) | 14×15 | — | id=1005 |
| popUp/backgrnd | — | 1 | (0, 0) | 394×148 | 1 | 弹窗底板 |
| popUp/button:confirm | 4 state | 4×1帧 | (-92, -90) | 87×30 | 2 | — |
| popUp/button:cancel | 4 state | 4×1帧 | (-211, -90) | 87×30 | 2 | — |
| popUp/button:confirm2 | 4 state | 4×1帧 | (-154, -90) | 1×1（全 `_inlink→confirm/*`） | 2 | 内部复用 confirm |
| Timer | 帧 0..9 + ':' | **11** | 帧0 (1,0)、帧1..9 ':' (0,0) | 13~18×19~20 | — | 计时数字 |
| Number | 帧 0..9 | **10** | 全部 (0,0) | 8~10×12 | — | 数字图集 |
| Grade | 帧 0..4 | **5** | 无 origin 字段 | 23×13 / 32×13 / 33×13 / 32×22 / 42×22 | — | 等级图标 |
| Type | 帧 0..4 | **5** | 无 origin 字段 | 20×20 / 20×20 / 26×26 / 38×38 / 41×41 | — | 类型图标 |
| Result/rewardSlot | 帧 0..4 | 5 | — | — | — | 奖励槽；layer:rewardBg0/1/8/9/10 复用 slot/0 |
| Result/Effect | 帧 0..64 | **65** | — | — | — | 结果特效；EffectSS 中 52/62/63/64 复用 |
| Result/EffectSS | 帧 0..64 | **65** | — | — | — | 4 帧 `_inlink→Effect/*` |
| Notice/backgrnd | — | 1 | (0, 0) | 410×641 | -4 | 公告底板 |
| Notice/$scroll | enabled prev0-2 / next0-2 / thumb0-2；disabled prev/next | 11 槽 | (0,0)（1×1 占位） | 1×1（占位，目标在 StatusBar3） | 0 | 全部 `_outlink→UIWindow2? 否→StatusBar3` |

### 4.2 CN_Limitbreakthrough（突破石）

| 节点 | state/帧 | 数量 | origin | 尺寸(W×H) | z | 备注 |
|---|---|---|---|---|---|---|
| backgrnd | — | 1 | (0, 1) | 196×285 | -5 | 主底板 |
| backgrnd2 | — | 1 | (-5, -21) | 186×256 | -4 | 内底板 |
| backgrnd3 | — | 1 | (-10, -26) | 176×34 | -3 | 顶条 |
| backgrnd4 | — | 1 | (-10, -151) | 1×1 占位 | -3 | `_outlink→Cube_InGame1/backgrnd4`（真实 176×97） |
| Effect/Normal | 帧 0..11 | **12** | (-10, -62)（全部） | 176×86 | 0 | 实体帧（无链接） |
| Effect/GradeUp | 帧 0..11 | **12** | 0..8 (-10,-26)，9..11 (-10,-62) | 1×1 占位 | 0 | 全 `_outlink→Cube_Red/Effect/*` |
| Effect/Use | 帧 0..11 | **12** | (-10, -62)（全部） | 1×1 占位 | 0 | 全 `_outlink→Cube_Red/Effect/*` |
| BtOk | normal/pressed/disabled/mouseOver | 4×1帧 | (-146, -256)（4 帧同） | 1×1 占位 | — | 全 `_outlink→Cube_InGame1/BtOk/*`（真实 40×16） |
| BtOnemore | normal/pressed/disabled/mouseOver | 4×1帧 | (-10, -256)（4 帧同） | 1×1 占位 | — | 全 `_outlink→Cube_InGame1/BtOnemore/*` |

关键坐标关系：BtOk 在 (-146,-256)、BtOnemore 在 (-10,-256)，水平间距约 **136px**，同处底部 y=-256；三组 Effect 共用原点区域 (-10,-62)/(−10,−26)；backgrnd4 占位于标题下方 (-10,-151)。

---

## 5. 移植提示（基于本轮证据，不含实际改写）

1. `CN_Limitbreakthrough` 与 `CN_50100` 的实体资源分布在 **5 个外部 img**：`UIWindow2.img`(UI_004)、`StatusBar3.img`(UI_002)、`CashShop.img`(UI_000，来自其它 CN 节点)、`UIWindow4/6.img`(UI_005)。其中 50100 的 `$scroll` 11 槽依赖 `StatusBar3.img/chat/common/scroll`，突破石的按钮/特效依赖 `Cube_InGame1`/`Cube_Red`。
2. 1×1 占位节点（`backgrnd4`、BtOk/BtOnemore、GradeUp/Use 帧、Notice $scroll 等）靠 `_outlink/_inlink` 解析；移植时若只复制 UIWindowCN.img 本体而不带上外部 img，需在本端重建这些实体或保留等效 link 解析能力。
3. 跨端迁移必须按 AGENTS.md §3.1 走显式 `batch_convert_key(目标key, verify:true)` + 本端 probe/短启，禁止裸拷 `.img`；192 零 IV `AAAAAA==` 与目标端 GMS IV `TSPHKw==` 不通用。
4. 中文字符串在 `String_000.wz` / `Quest_000.wz`，若移植需 UI 文案，按 §3.2 的 img/nodePath 抽取。
