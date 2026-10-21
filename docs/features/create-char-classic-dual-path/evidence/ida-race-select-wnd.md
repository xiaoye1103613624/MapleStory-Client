# IDA：CUINewCharRaceSelect 窗口与按钮

## 窗口锚点

来源：`docs/features/login-flow-ui-ida-align/evidence/CUINewCharRaceSelect_ctor_0x615596.txt`

- `CWnd::CreateWnd(nLeft=-372, nTop=-2128, nWidth=700, nHeight=460, z=10, bScreenCoord=0)`
- 经典 `UIRaceSelect`：`LoginBackground` step=3 时 `position = viewpos + (-372, -2128)`；否则视口居中 700×460

## OnCreate 控件坐标（已校准）

来源：IDA `sub_6157C9`（`CUINewCharRaceSelect` vtable OnCreate），`CCtrlButton::Create(parent, id, x, y, …)`

| 控件 | 字符串池路径 | id | (x, y) |
| --- | --- | --- | --- |
| BtKnight | `UI/Login.img/RaceSelect/knight/BtKnight` | 1000 | **(104, 79)** |
| BtNormal | `UI/Login.img/RaceSelect/normal/BtNormal` | 1001 | **(293, 79)** |
| BtAran | `UI/Login.img/RaceSelect/aran/BtAran` | 1002 | **(511, 80)** |
| BtSelect | `UI/Login.img/RaceSelect/BtSelect` | 1003 | **(530, 422)** |

左→右职业顺序：**骑士团 → 冒险家 → 战神**。

点击处理（`sub_61747F`）：

- id 1000/1001/1002 → `sub_616283(this, 0/1/2)` 切换选中
- id 1003 → `CLogin::ChangeStep(login, -1)`（即当前 step+1，进入创建外观）

本实现对应：`classic_card_pos` / `classic_select_pos` / `classic_classes[]`；职业卡 `AreaButton` 使用 `pos - texture.origin` 与 `Texture::draw` 对齐。
