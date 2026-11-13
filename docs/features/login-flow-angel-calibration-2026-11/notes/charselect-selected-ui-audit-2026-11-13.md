# 选中角色后 UI / 动画 / 字体对照（Angel ↔ 本端）· 2026-11-13

**状态：已按 Angel 字面落地**（2026-11-13：坐标 1/19/37/55/99 + 字色 `#000000`；权威见 `authority-switch-2026-11-12.md`）。

权威：Angel IDA（`Angel.i64`）+ orange-wz `Login.img`（BeiDou-Client-xy）+ `UICharSelect.cpp`。

## 选中后触发链

| 层 | Angel | 本端 |
| --- | --- | --- |
| 入口 | `CUIAvatar::SelectCharacter @0x60599B` → `CLogin::OnSelectedCharChanged @0x5F6482` | `button_pressed(CHARACTER_SLOT*)` → `update_selected_character` |
| 音效 | `play_ui_sound(SP_1237_CHARSELECT)` | `Sound::CHARSELECT` |
| 选中光 | 生成 `CharSelect/effect/0` 与 `effect/1` 层；x=`125*(i%3)-140`，y=`-300`（相对 Gr2D center） | `selectedslot_effect[1]` 脚底 + `effect[0]` 身光；锚 `CS_FX_FOOT(-5,-310)` / `CS_FX_BODY(-5,-298)` |
| 名牌 | `DrawNameTag`：未选灰 / 选中白；字 `nTop=2` 居中 | `NameTag::set_selected`：`EAGLE`→`WHITE`；脚点 `+(2,25)` |
| 人偶 | 层动画状态切换（选中槽） | `stance = WALK1` |
| 详情 | 独立 `CUICharDetail` FadeWnd（`CreateFadeWnd` α 进出） | 同窗内 `scroll/2` 展开定格末帧 + `charInfo1` 叠绘 |

结论：触发项齐全；详情「Fade vs 卷轴」是资源/架构差（BeiDou 有 `scroll/2`，Angel 走 FadeWnd），本端卷轴路径与既有视频取证一致，**不当作漏实现**。

## 详情板文字位置（相对 charInfo 画布）

Angel `CUICharDetail::Draw @0x602ADC`（`DrawTextA`）：

| 字段 | Angel (nLeft,nTop) | 本端 `CS_DETAIL_*` | ΔY |
| --- | --- | --- | --- |
| 职业 | (46, **1**) | (46, **3**) | +2 |
| 等级 | (46, 19) | (46, 21) | +2 |
| 人气 | (136, 19) | (136, 21) | +2 |
| 力量 | (46, 37) | (46, 39) | +2 |
| 智力 | (136, 37) | (136, 39) | +2 |
| 敏捷 | (46, 55) | (46, 57) | +2 |
| 运气 | (136, 55) | (136, 57) | +2 |
| 排行不可用 | (36, **99**) | (36, **93**) | −6 |
| 排行数字 | (115−w, **99**) | (115, **93**) | −6 |

X 与 Angel 一致。Y：属性行整体 +2（注释称对齐 BeiDou 烘焙标签行顶 3/21/39/57，且 `OutlinedText` 基线可能与 `DrawTextA` 不同）；排行 −6。

## 字体颜色

- Angel：`get_basic_font(fType:1)` → CreateFont 色 **`#000000`**（`push 0xFF000000` @`0x98A83A`，case1 @`0x98A7DF`）。
- 本端：`Color::TOBACCOBROWN` ≈ `#786645`（深棕，为米色面板可读性；注释称「对齐参考端深棕」）。
- 字面 Angel = 纯黑；观感上羊皮纸+抗锯齿常显深棕。属「字面黑 vs 观感棕」分叉。
- 名牌选中：Angel 白 / 未选灰；本端 `WHITE` / `EAGLE` — 语义一致。

## IMG（本端 Data）

| 节点 | 规格 |
| --- | --- |
| `charInfo1` | 183×151，origin (45,57) |
| `scroll/2` | 0:217×30 d250 → … → 3:217×193（末帧常驻） |
| `effect/0/0` | 49×132，origin (24,−205)，delay 300 |
| `effect/1/0` | 首帧 1×274，origin (0,0)（后续帧增高） |

与本端绘制路径（末帧常驻卷轴 + charInfo 叠绘、双层选中光）一致。

## 差异分级

| ID | 项 | 判定 | 说明 |
| --- | --- | --- | --- |
| OK1 | 选中逻辑/音效/名牌态/走路 | 一致 | — |
| OK2 | 卷轴展开 + charInfo | 资源适配 OK | Angel Fade ≠ BeiDou scroll，本端正确跟资源 |
| DRIFT1 | 属性值 Y +2 | 低 | 可能基线补偿；纯 Angel 字面量会偏烘焙标签 |
| DRIFT2 | 排行 Y 93 vs 99 | 中 | 差 6px，建议实机/截图像素裁决 |
| DRIFT3 | 字色 `#786645` vs Angel `#000000` | 中 | 字面应对齐黑；若跟 BeiDou 截图观感可保留深棕 |
| NOTE | 选中光锚点坐标系 | 观测 | Angel center 相对坐标 vs 本端脚点相对；已按贴图实测，非字面照搬 |

## 落地（2026-11-13）

用户裁定「全按 Angel」后已改 `UICharSelect.cpp`：

1. **字色**：详情 `infolabels` / `ranklabel` / `lvltext` → `Color::Name::BLACK`。
2. **坐标**：`CS_DETAIL_*` → Angel 1/19/37/55；排行 Y → **99**。
3. **未改**：卷轴 `scroll/2`（BeiDou 资源）、选中光脚点锚、名牌 WHITE/EAGLE。
