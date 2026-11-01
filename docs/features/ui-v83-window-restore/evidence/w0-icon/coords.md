# W0-ICON — 图标锚点公共件：`Icon` 的 (0,32) 下移是错的

**结论**：本包（北斗 `.nxcache`）所有 `icon` 位图的 `origin` 都是小值（≈0），
上游 `Icon::Icon` / `StatefulIcon::StatefulIcon` 里无条件 `texture.shift(Point(0,32))`
会把**每一个**图标整体下移 32px。已删除该 shift。

影响面（全部走 `Icon::draw` / `StatefulIcon`）：背包格、装备格、快捷格、
商店/商城格、技能窗行图标、tooltip 拖拽预览。

---

## 依据 1：贴图 origin 实测（nx_inspect，可信）

| 节点 | 位图 | origin |
|---|---|---|
| `Character/Coat/01040000.img/info/icon` | 29×30 | (-2, -1) |
| `Character/Cap/01000000.img/info/icon` | 29×30 | (-2, -1) |
| `Character/Glove/01080000.img/info/icon` | 27×28 | (-4, -1) |
| `Character/Shoes/01070000.img/info/icon` | 28×22 | (-2, -1) |
| `Character/Weapon/01302000.img/info/icon` | 31×30 | (0, 0) |
| `Item/Consume/0200.img/02000000/info/icon` | 27×30 | (-3, -1) |
| `Skill/000.img/skill/*/icon`（新手全部 25 个技能） | 32×32 | (0, 0) |

⇒ 没有任何一个 icon 的 origin 接近 (0,32)，`shift(0,32)` 无的放矢。
（`Graphics/Texture.cpp:107` `shift` 做 `origin -= amount`；
`Graphics/DrawArgument.h:134` `rlt = pos - center - origin` ⇒ 左上角 = 绘制点 − origin。
origin.y 被减 32 ⇒ 绘制点下方 32px。）

## 依据 2：背包格线 vs 图标墨迹（参考截图 `e75687e9-…png`「现有道具+tooltip」）

逐像素扫描（ASCII 亮度图）得格线：

- 竖线（列分隔）中心 x = 32 / 68 / 104 / 140 → **步进 36**
- 横线（行分隔）y = 46 / 80 / 114 → **步进 34**

与 `UIItemInventory.cpp:138-147` 的回退常量完全对齐：
`slot_pos=(8,51)`、`slot_space_x=4`、`slot_space_y=2`、格宽 32
（窗口左上角绝对坐标 = (24, -5) ⇒ 格 0 的窗口相对左上 = (8, 51) ✓）。

白棉T恤（第一行第 1 格）图标墨迹左上 ≈ 格左上 + (4, 1)。
- **无 shift**：左上 = 绘制点 − origin = slot_pos + (2, 1) → 误差 ≤2px ✓
- **有 shift**：左上 = slot_pos + (2, 33) → 低 32px，图标掉进下一行格子里 ✗

## 依据 3：技能窗行图标（参考截图 `3fe4ade2-…png`「技能窗+组合栏」）

numpy alpha-masked SAD 模板匹配（`E:/project/tmpl_all.py`）：

| 目标 | 匹配点（截图绝对坐标） | SAD |
|---|---|---|
| 行底 `UIWindow.img/Skill/skill1`（141×35） | (315, 101)（行距 40 → 141/181/221） | 21 |
| `000.img/skill/0001000/icon` 蜗牛投掷术 | **(317, 143)** | **0** |
| `000.img/skill/0001001/icon` 团队治疗 | **(317, 183)** | **0** |
| `000.img/skill/0001002/icon` 疾风步 | **(317, 223)** | **0** |

⇒ 原生行图标左上 = 行底左上 + **(2, 2)**，正是 `SKILL_META_OFFSET`。
删 shift 前我们算得 + (2, 34)，低 32px：
每行的灰色图标框看起来是空的，图标压在**下一行**的框里 ⇒ 用户报「技能框主 UI 有技能缺图标」。

## 依据 4：文字基线复核（确认 `SKILL_META_OFFSET` 不用动）

行1（行底左上 (315,141)）：

| 元素 | 原生墨迹 | 行相对 | 我们的基线 |
|---|---|---|---|
| 技能名 | y 146..155、x 356..408 | +5..+14 / +41..+93 | `draw_y+16` = 行y+13（A11M，`char_y = y + linespace - bt`） |
| 等级数字 | y 165..172、x 355..359 | +24..+31 / +40..+44 | 行y+15+16 = 行y+31 ✓ |
| SP「+」钮 | y 121..132、x 440..451（行0） | +20..+31 / +125..+136 | `CLASSIC_SPUP_OFFSET=(125,20)` ✓ |

⇒ 名字/等级/加号三项与原生差 ≤2px，**不改**；本轮只改公共件 shift。

---

## 未做 / 风险

- 原生把 `invisible=1` 且 `disable=1` 的技能（如 `0000012 精灵的祝福`、`0000100 椅子大师`）
  的图标格画成**空白灰框**（`3fe4ade2` 行 0 实测无图标墨迹），但贴图本身是完整的
  （`_r15_s12_big.png` 可见翅膀精灵）。我们目前照常画图标。
  属「多画」而非「缺画」，与用户报的症状方向相反，**本轮不动**，留待逐窗核对。
- `NpcText.cpp:122` 自己 `shift(0,32)` 的是任务 NPC 立绘 `Texture`，不走 `Icon`，未受影响。
- `UIMapMobPanel.cpp:35-45 normalize_icon_origin()` 处理的是怪物图标裸 `Texture`，未受影响。
