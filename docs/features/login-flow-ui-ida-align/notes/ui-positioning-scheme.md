# 登录链路 UI 定位规范（正确方案）

> 本文回答「`UICharSelect` 控件混用 `position` 与 NX 绝对坐标，正确应该是什么」。

## 一、问题本质

`UIElement` 提供了唯一参照原点 `position`，但登录链路的各界面只部分使用它：

| 界面 | 使用 `position` | 漏用 `position`（直接用 NX 绝对坐标上屏） |
|---|---|---|
| `UIWorldSelect` | 槽位/面板/按钮（已统一） | —（本轮已修） |
| `UICharSelect` | `charinfo`、`pagebase`、`tab`、`burning_*`、全部 `buttons` | `get_character_slot_pos()` 派生的一切（`charlooks`/`signpost`/`nametags`/`emptyslot`/选中特效）、`levelPos`、`namePos`、`get_infolabel_pos()` |

后果：**原点无法整体平移**——改 `position` 只会让一半控件动、另一半不动，所以校准无从下手。

## 二、正确方案：单一参照原点（Single Reference Origin）

**三条规则**：

1. **每个 UI 元素只声明一个原点**。所有子控件、文本、按钮一律以「原点 + 局部偏移」绘制；
   禁止任何 NX 坐标绕过原点直接上屏。
2. **原点必须与「它所依附的场景」同源**。登录场景是「固定尺寸地图 + 固定 600 步进平移」，
   地图以 `viewpos = center − (28, step_height(step))` 居中绘制。
   因此登录链路 UI 的原点必须是 `viewpos` 的函数，否则分辨率一变，UI 与地图就会相对漂移
   （`UILogin` 已按此实现：面板锚在告示牌 `signboard_map_pos + viewpos − sign_origin`；
   `UIWorldSelect` 本轮已改为 `viewpos.y + WS_BANNER_OBJ_Y`）。
3. **响应分辨率变化**：重写 `UIElement::update_screen(new_w, new_h)` 重新求解原点
   （本工程其他 UI 也是这个套路）。注意 `bounds()` 会自行叠加 `position`，
   因此**按钮的局部坐标处不能重复加**。

**与原版的对应关系**：原版 `CWnd::CreateWnd / CDialog::CreateDlg / CFadeWnd::CreateFadeWnd`
末参都是 `W4UIOrigin@CWnd`（窗口原点枚举），`CWndMan::GetOrgWindow(UIOrigin)` 按原点返回参照窗口
——即「窗口声明锚点，由窗口管理器换算成屏幕坐标」，与上面的规则 1+2 完全同构。

## 三、落地步骤

| 步骤 | 内容 | 状态 |
|---|---|---|
| S1 | **结构归一**：把所有漏用 `position` 的绘制补上 `position`，使原点成为唯一参照 | ✅ 已完成（`UICharSelect`） |
| S2 | **原点求解**：令 `position = viewpos + 局部标定偏移`，并 `update_screen` 重算 | ⏳ 待标定 |
| S3 | **局部偏移标定**：以原版截图/IDA 逐项对齐（与选世界/频道同一闭环） | ⏳ 待做 |

**S1 是零视觉变化的纯重构**：`position` 当前为 `(0,0)`，`position + X ≡ X`，故不改画面，
但从此**标定只需改 `position` 一个点**，S2/S3 才能进行。

## 四、S1 具体改动（`IO/UITypes/UICharSelect.cpp`）

```cpp
// 槽位：局部坐标统一加 position（按钮的 bounds() 由 UIElement 负责叠加，故那里不加）
Point<int16_t> charpos = position + get_character_slot_pos(i);
DrawArgument chararg = DrawArgument(charpos, flip_character);

// 选中特效/名牌/等级/属性标签
selectedslot_effect[1].draw(charpos + Point<int16_t>(-5, 16), inter);
int16_t lvx = levelset.draw("201", position + levelPos + Point<int16_t>(-1, lvy));
levelset.draw('l', position + levelPos + Point<int16_t>(-lvx - 3, lvy - 1));
namelabel.draw(position + namePos);
Point<int16_t> labelpos = position + get_infolabel_pos(i);

// 空槽
Point<int16_t> emptyslotpos = position + get_character_slot_pos(i);
```

`chararg` 覆盖了 `signpost[j]` / `charlooks[index]`；`selectedslot_effect[0]` 同理随 `charpos` 一起归位。

## 五、S2/S3 待标定的量

`position` = 登录地图坐标系原点到「`Login.img/CharSelect` 界面坐标系原点」的偏移，
**不能凭现有 IDA dump 解析**（参考版 `CUICharSelect` 只是 101x131 的子窗口，
且 `nTop = −1359` 的父窗口帧无法唯一确定，见 `ida-coords.md` §4.3），
必须靠**原版截图实测反推**——与选世界条幅（`WS_BANNER_OBJ_Y`）同一方法。

期间的独立问题（与原点无关，需分别处理）：

1. 「删除角色」/「排行榜」等按钮创建时**未传位置**（`MapleButton(CharSelect["BtDelete"])`），
   局部坐标默认 `(0,0)` → 停在屏幕左上角。需要按 NX 节点补齐各自偏移。
2. `namelabel`/`infolabels`/`charlooks` 未显示：需确认是数据（角色 look/名字）还是坐标问题。

## 六、验收方式

沿用本项目既有闭环：**编译 → 部署 → `_flow.ps1` 自动登录并点击到目标屏 → 截图 → 量化比对**。
`UICharSelect` 现已可稳定复现（`evidence/charselect_bg_fixed.png`），可直接用于 S2/S3 迭代。
