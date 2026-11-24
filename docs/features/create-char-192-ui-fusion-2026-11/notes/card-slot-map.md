# 槽位契约对照（T1）

> 2026-11-18 · 决策：`A + 仅五团` · 目标节点名 `BtEvan` / `BtDual`

## 本端 classic 消费（`UIRaceSelect::init_classic`）

每个职业卡根（如 `RaceSelect/aran`）：

| 子路径 | 用途 |
| --- | --- |
| `Bt*/normal/0` | 未选中卡面 |
| `Bt*/mouseOver/0` | 悬停卡面 |
| `OnAnimation`（帧序列） | 选中态动画/高亮 |
| `text/0` | 底部说明板 |
| `OffAnimation`（可选） | classic 有 1×1 占位 |

按钮名数组现状：`BtKnight, BtNormal, BtAran, BtAran, BtNormal` → 将改为 `…, BtEvan, BtDual`。

实测 xy：`aran/OnAnimation/0` ≈ 181×222；`text/0` ≈ 579×163。

## 192 源（`UI_001.wz` / LATEST）

| 用途 | 龙神 (EVAN=4) | 双刀 (DUALBLADE=8) | 尺寸 |
| --- | --- | --- | --- |
| 卡按钮五态 | `RaceSelect_new/button/4/{normal,mouseOver,pressed,keyFocused,disabled}` | `…/button/8/…` | **121×136** |
| 说明详情 | `Back2/4/0` | `Back2/8/0` | 282×287（龙神已测） |
| 标题小图 | `Back3/4/0` | `Back3/8/0` | 74×53（可选，本波不用） |
| 大背景 | `Back0/4`（含 spine） | `Back0/8` | 本波不用（spine 本端不渲染） |

## 目标写入树（xy `Login.img` / GMS）

```
RaceSelect/evan/
  BtEvan/{normal,mouseOver,pressed,disabled}[/0]  ← 自 button/4 对应态
  OnAnimation/0  ← 自 button/4/mouseOver/0（单帧选中高亮）
  OffAnimation/0 ← 自 classic aran/OffAnimation/0（1×1 占位，同钥复制）
  text/0         ← 自 Back2/4/0

RaceSelect/dual/
  BtDual/{…}     ← 自 button/8
  OnAnimation/0  ← button/8/mouseOver/0
  OffAnimation/0 ← aran/OffAnimation/0
  text/0         ← Back2/8/0
```

## 尺寸风险

192 book 按钮 121×136 小于 classic ~181×222；五卡横排坐标已压缩，较小卡面可接受。验收时确认可点区域（`AreaButton` 用位图尺寸）足够。

## 代码同步（T3）

`btn_names[]` → `{ "BtKnight", "BtNormal", "BtAran", "BtEvan", "BtDual" }`；回落扫描补 `BtEvan`/`BtDual`。

## 交付补记（T2 阻塞绕行）

orange-wz 对本端既有 `Login.img` 报 `IMG 解析失败`（`_wz.py` 仍可读）。已转钥 staging + 导出 PNG，由 `UIRaceSelect` `LocalPngTexture` 运行时加载：

| 文件 | 用途 |
| --- | --- |
| `evan_normal.png` / `evan_over.png` / `evan_on.png` / `evan_text.png` | 龙神 |
| `dual_normal.png` / `dual_over.png` / `dual_on.png` / `dual_text.png` | 双刀 |

路径候选：`UI/Creation/RaceSelect/`、`docs/features/create-char-192-ui-fusion-2026-11/resources/cards/`。
