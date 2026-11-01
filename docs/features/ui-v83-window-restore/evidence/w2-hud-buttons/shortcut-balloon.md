# 「界面」气球（UIShortCut）· 条目与坐标（R16）

> 参考端：`C:\Users\11036\Desktop\正确客户端截图\”界面“按钮点击后的列表.png`（94×278，1:1 裁图）
> 素材：`UI.nx` 根 `UIWindow.img/ShortCut`（`backgrnd` 93×245 + 11 张 `Bt*` 81×25）
> 取证脚本：`E:\project\_r17_sc.py` → `E:\project\_r17_sc.txt`；
> 单件导出 `python tools/nx_inspect.py image <UI.nx> UIWindow.img/ShortCut/<Bt>/normal/0 <out.png>`
> （注意：`image` 子命令**没有 depth 参数**，`list` 才有）

## 1. 逐件 SAD 配准（整图 alpha>200 掩膜，搜索 x 0..13 / y 0..253）

| 节点 | 最佳 SAD | 落点(裁图) | 判定 |
| --- | --- | --- | --- |
| `BtEquip/normal/0` | **0.00** | (8, 48) | 在场 |
| `BtStat` | **0.00** | (8, 74) | 在场 |
| `BtSkill` | **0.00** | (8, 100) | 在场 |
| `BtComm` | **0.00** | (8, 126) | 在场 |
| `BtQuest` | **0.00** | (8, 152) | 在场 |
| `BtMobbook` | **0.00** | (8, 178) | 在场 |
| `BtMessenger` | **0.00** | (8, 204) | 在场 |
| `BtItem` | 8.82 | — | **在场但被标题条下沿压住**，取 0.00 系列的等差外推 → (8, 22) |
| `BtGuild` | 8.74 | — | 不在场（无精确命中） |
| `BtParty` | 8.41 | — | 不在场 |
| `BtRanking` | 38.25 | — | 不在场 |

七件 SAD 恰好 0.00 且等差 ⇒ 这是硬证据，不是拟合：

- **列 x = 8**（不是几何居中 `(93-81)/2 = 6`）；
- **首颗顶 y = 22、间距 26**（`22 + i*26`，i=0..7；末颗底边 229，底板 245 ⇒ 留 16px 尾巴）；
- **只有 8 颗**，顺序 `道具 / 装备窗 / 能力值 / 技能 / 社区 / 任务 / 怪物手册 / 聊天窗`。

底板 `backgrnd` 自身无法配准（SAD 116 —— 它被 8 颗钮盖住大半，掩膜区全是错配），
所以 y 的绝对基准取「裁图 y0 == 底板 y0」，由 7 件 0.00 命中互相印证。

## 2. 本批改动

`IO/UITypes/UIMiscShells.cpp` `UIShortCut::UIShortCut()`：原先把 **11** 颗全摆上、
`y = 20 + i*20`、`x = 6` ⇒ 间距比原生小 6px、多出的公会/组队/排行三颗把气球挤成一坨，
且顺序是 能力值/装备/道具…（与原生 道具/装备/能力值… 不一致）。
现按 §1 改为 8 颗 + `(8, 22+26i)`。

`UIMiscShells.h`：`Buttons` 枚举删 `BT_GUILD / BT_PARTY / BT_RANKING`，
`button_pressed` 删对应三个 case（原生这层气球不摆 → 留 case 就是死代码）。
三者的真入口不受影响：公会走 `UIGuild` 自身按钮、组队走 `UIPartySearch`、
排行走 `UIStatusBar` 的 `BtRanking` 通道。

## 3. 「界面展开后关闭不了」

代码层已修（早于本批）：`IO/UITypes/UIStatusBar.cpp:802` `BT_CLASSIC_SHORT` 分支
现在是 get→emplace / !is_active→makeactive / else→deactivate 三态，
旧实现只 emplace 不收起。注意 `UIShortCut::TOGGLED = false`，
所以**不能**指望 `UI::toggle` 帮忙关，必须走这条按钮分支 —— 验收时要点那颗按钮本身，
而不是再按一次快捷键。

## 4. 未决

- `BtItem` 的 y=22 是等差外推（它自己 SAD 8.82）。若真机截图显示首颗与标题条重叠，
  优先怀疑「裁图 y0 ≠ 底板 y0」，需要一张带完整上沿的参考图重定基准。
- 气球相对 `BtShort` 按钮的锚定（原生气球尾巴指到按钮上方）本批未动，
  沿用 `update_screen()` 的现有算式。
