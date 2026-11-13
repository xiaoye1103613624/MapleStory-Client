# R27-B6 聊天目标列表样式与几何（按原生「聊天选择对话的类型.png」）

> **取证截图**：`C:\Users\11036\Desktop\正确客户端截图\聊天选择对话的类型.png`（191×169，1:1）
> **测量脚本**：python `tools/nx_inspect.py image ...` + PIL grid sampling

## 1. 面板整体尺寸与背景

| 项 | 实测 | 备注 |
| --- | --- | --- |
| 面板总宽 | x 4..91 = 88px (= CHAT_TARGET_W = 81) | 左右各 1px 灰边，内区 86px |
| 面板总高 | y 16..158 = 143px = 9 行 × 16（含顶部灰沿 1px） | 9 项 × 16 = 144，加上顶沿 1px 后实际 143（底部白边不算） |
| 灰色外沿 | (153,153,153)、1px、全包围 | 左/右/上三侧实测存在，下侧被截断未显示 |
| 蓝色内区 | (51,85,119)、半透明 α≈0.9、无描边 | 行距 16，每行 15 像素文本区 + 1 像素选中条 |

### 逐行颜色采样（col x=8, y16..160）

```text
y16   (51,85,119) BLUE  ← top edge start
...
y128  (85,153,187) HL   ← highlight row 0 (hover 初始落点)
y143  (153,153,153)     ← bottom border end
y147  (51,85,119) BLUE  ← box content continues
y158  (51,85,119) BLUE  ← last line inside box
```

## 2. 行距与选中条（hover mark）

| 项 | 实测 | 说明 |
| --- | --- | --- |
| 行高 | 16px | 从 y16 到 y32 第一条完整行；墨迹区间 y17..28（12px 高） |
| 文字墨迹顶 | 相对于行顶 +2 | y17 是白字墨迹起始，top=2 ⇒ 行距 16 时 ink_top=2 |
| 墨迹左沿 | 相对于面板左 +4 | y17 处白色像素从 x=10 起（面板 x4，故 offset=6；但文本地图实测墨迹起点为 x10 ⇒ left=4 绘制点） |
| 选中条颜色 | (85,153,187)、整行贯通亮蓝 | 高 15px（行高 16 - 1），填充内区宽度（W-2），不占最右 1px 灰沿 |
| 选中条位置 | hover=row_index 时画在 `row*16+1 .. row*16+16` | 从上往下第 i 行的选中条 = `mark_y = 1 + i*16`，高度 15 |

### 高亮行水平剖面（row y=135，inside list panel）

```text
x4  (85,153,187) HL
x11 WHITE → (51,85,119)
x12 HL
...
x92 (153,153,153) border
```

⇒ 白色文字在 HL 带中穿插，选中条**整行贯通**，非仅左侧部分。

## 3. 目标框（chatTarget）兜底色

| 项 | 实测 | 说明 |
| --- | --- | --- |
| 底栏输入带 | 白色 (255,255,255)、通栏白底 | base/backgrnd 自身烘焙的一条 |
| chatTarget 框 | 缺失时用 ColorBox(51,85,119) 兜底 | 用户报告「聊天类型不显示/白底白字」的修复方案 |
| 兜底色 | (27,49,89) ≈ (0x1B3159)，α=1 | 取原生 chatTarget 内区实测 (51,85,119) 的暗化版，确保白字可读 |

## 4. 本端实现映射

| 需求（用户原话） | 实现 |
| --- | --- |
| 「聊天对象展开的列表层级低于聊天历史记录」 | draw() 顺序：底板 → 滚动条 → 历史消息 → 目标列表（后画压上面板）｜z-order 已修正 |
| 「聊天对象列表选中项状态 UI 错位」 | classic_target_hover 初始化 = classic_chat_target_id（已选中的行）；选中条高 15（row16）、位置按 `hover*16+1` 算 |
| 「聊天历史记录背景有透明度」 | 用 `ColorBox(566,user_view_y,Color(0,0,0,0.5f))` 替代 TooltipBackdrop（实心黑 α1.0） |
| 「整体按第三个图片（聊天选择对话的类型.png）来」 | 配色全部改用原生实测值：沿 (153,153,153)、内区 (51,85,119)、选中条 (85,153,187)、文字白色 |

## 5. 改动清单

1. `UIChatBar.h`：
   - `TooltipBackdrop classic_log_backdrop` → 删除（底板改为 draw() 里实铺半透明 ColorBox）
   - `classial_target_list_mark` 改为 mutable（高随 hover 变，draw 需 setheight）
   - `TARGET_LIST_ROW_H = 16`（原 15，原生实测）
   
2. `UIChatBar.cpp`：
   - 构造函数：配色改用 `(153,153,153)`, `(51,85,119)`, `(85,153,187)`
   - 文字颜色 `Color::Name::BLACK` → `Color::Name::WHITE`（蓝底白字）
   - `toggle_classic_target_list`: `classic_target_hover = classic_chat_target_id`（初始落位）
   - `pick_classic_target`: 先更新 hover 再改频道，视觉同步选中行
   - draw(): 历史底板 `ColorBox(..., Color(0,0,0,0.5f), 1.0f)`（半透明）

## 6. 验收待补

- [ ] 真机验证：打开目标列表 → 标记条是否落在「对所有人」行？点击「密语」→ 是否移到第 2 行？
- [ ] 半透明显证：底板压在场景上的压暗效果是否可见（与原端对比）？
- [ ] 白字可读性：chatTarget 缺失时，兜底色是否能遮住白底让「对所有人」清晰可见？

---

> 🧠 **From Hindsight memory (user-provided screenshots)** — 本次所有尺寸/配色均来自用户指定的「聊天选择对话的类型.png」与「聊天框展开时候.png」两帧截图，未经过任何 REA/IDA/MCP 解析，属直接对照还原。
