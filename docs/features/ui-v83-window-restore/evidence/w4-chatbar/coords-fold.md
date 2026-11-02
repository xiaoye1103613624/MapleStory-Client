# W4 聊天栏 · 收起态（fold）逐格定版 + 输入态正文可见性

> 原生 oracle：`C:\Users\11036\Desktop\正确客户端截图\聊天框收起的时候.png`（**566×40 局部截帧**）
> 本端截帧：`E:\project\_r27_f7_{folded,typed,sent}.png`（PrintWindow 整窗，1280×720 视图 + 边框）
> 取证脚本：`E:\project\_ms_probe\_fold_verify.py`（断言）/ `_fold_compare.py`（首轮逐行比对）
> 驱动脚本：`E:\project\_r27_fold7.py`（登录进图 → ESC → Enter → 打字 → Enter 发送 → 抓帧）
> 原始读数：本目录 `fold-verify.txt`（断言输出）`fold-rows-native-top.txt`（原生顶缘逐行）
> `fold-rows-both.txt`（首轮原生 vs 本端逐行）`fold-compare-first-pass.txt`
> 图：`fold-native-3x.png`（原生 3×）`fold-band-sent.png`（本端收起态）`fold-typed-zoom.png`（本端输入态）
> 脚本副本：`fold-verify.py` `fold-live-driver.py`
> 结论落码：`IO/UITypes/UIChatBar.h` 的 `FOLD_PANEL_TOP / FOLD_PANEL_H / FOLD_EDGE_TOP /
> FOLD_TEXT_INK_TOP / FOLD_TEXT_X` + `classic_fold_panel` / `classic_fold_edge`

## 1. 用户报的缺陷

「聊天区域：聊天收起不是应该在白色框区域嘛？」——本端进图后聊天栏一直顶着一条**白色输入带**
（`view_input` 构造时留 `true`），收起态那条灰带根本没出现过。

## 2. 两张不同源的图怎么对齐

原生是 566×40 的局部截帧、本端是整窗 PrintWindow，不能直接叠。锚点取**白输入带顶缘那条
`(255,255,255)` 高光行**（原生 y=8；本端 capture y=684 = 带 `position.y()` 653 + 31）：

| | 原生截帧 | 本端 capture | 本端 view |
| --- | --- | --- | --- |
| 锚点（白带顶缘高光） | y=8 `(255,255,255)` | y=684 `(245,254,255)` | 653 |
| 换算 | — | `view = capture - (8, 31)` | PrintWindow 边框偏移 |

**x 轴同锚**已由右端蓝钮验证：原生蓝区左沿 x=537，本端蓝区左沿 view x=**537**（逐像素一致）
⇒ 原生截帧的 x0 就是 view x0，灰带左沿/文字左沿可跨图直接比。

## 3. 收起态灰带（逐行 @x300，锚点相对值）

| rel | 原生 | 本端（定版后） | 判定 |
| --- | --- | --- | --- |
| +0 | (255,255,255) | (245,254,255) | 高光行，来自 `base/backgrnd` 自身 |
| +1 | (204,221,221) | (201,212,216) | 同上 |
| +2 | **(87,87,95)** | (87,87,95) | 原生这条是**灰带面板自带的 1px 内沿暗线**：改前本端这行透出的是 (182,195,201) 亮带 ⇒ 不是 backgrnd 的像素，补 `classic_fold_edge` |
| +3..+25 | 平涂灰 (111→119) | **(119,119,119)** 平涂 | 灰带顶 = +3、**高 23** |
| +26 / +27 | (153,170,170) / (102,119,119) | (147,157,167) / (106,119,128) | 带下沿双线，来自 backgrnd |

> 上一版常量是 `TOP=2 / H=24`（灰带落在 view 655..678），比原生**高一行、多一行**；
> 文字墨迹顶 +9 与墨迹左沿当时都对不上。现改为 `TOP=3 / H=23`。

文字：原生墨迹顶 y=17（rel **+9**）、墨迹左沿 **x=6**。本端 `Text(Font::A11M)` 实测
`draw_x=2` 时墨迹落在 view 4 ⇒ 该字体左 bearing ≈ +2，所以绘制点取 **`FOLD_TEXT_X=4`**
让墨迹正好落在 6（**不能把 oracle 的 6 直接抄成绘制点**，那是 +2 的系统误差）。
`FOLD_TEXT_INK_TOP=9` 由 `Text::y_for_ink_top` 反解，改前改后都命中，保持 9。

蓝 `+`（`Basic.img/BtMax`）左沿 view **537**，与原生一致，不动。

## 4. 灰带为什么是 ColorBox 而不是位图（负结果，保留）

本包 `UI.nx` / `StatusBar.img` 里没有任何一张能拼出这条灰带：

| 候选 | 尺寸 | 逐行取样 | 结论 |
| --- | --- | --- | --- |
| `base/chat` | 566×5 | 黑白条纹细条 | 是带下方那条细条（`UNDERLINE_Y`），不是灰带 |
| `base/backgrnd2` | 570×71 | 半透明白渐变 | 与 backgrnd 同原点叠加，无 (119,119,119) 平涂段 |

⇒ 按实测色自铺不透明矩形：`ColorBox(w, 23, Color(0x777777FF), 1.0f)` +
`ColorBox(w, 1, Color(0x57575FFF), 1.0f)`。`Color` 的四个 4 参构造对整数字面量是**歧义**的
（`C2440`），本包约定用 `Color(0xRRGGBBAA)`（见 `UIQuestLog.cpp:87-94`）。

## 5. 状态机：三条都要动

| 迁移 | 收起判据 | 落码 |
| --- | --- | --- |
| 进图默认 | `folded = !classic_log_open && !view_input` | ctor 里 `view_input = false`（原生进图就是收起态，白带要按 Enter 才出现） |
| Enter 开输入 | `toggle_input(true)` | classic 分支早退前 `sync_classic_buttons()` |
| 发送 / ESC 收起 | `toggle_input(false)` / `set_classic_log()` | 同一处 `sync_classic_buttons()` |

`sync_classic_buttons()` 是必须的：▶ 箭头（`BtWhisper`）由 `UIElement::draw` 画，构造时默认
active ⇒ 收起态箭头压在灰带正文左沿上。三个贴图按钮的可见性只有这一个判据，两条状态迁移
都要重算，所以抽成一处。`buttons` 是 `std::map`，classic 模式下必须用 `find()` 而不是
`operator[]`（否则插入空 `unique_ptr` 后解引用崩，见 `UIChatBar.cpp` 里既有注释）。

## 6. 输入态正文不可见（本轮新发现的独立缺陷）

真机抓帧 `typed` 帧实测：白带内除那条 1px 暗线外**零墨迹**，但按 Enter 后服务器回显
`Admin : hi` —— 说明字进了 `Textfield`、只是**画不出来**。根因是 classic 的
`input_text` 跟着历史消息用了 `Color::Name::WHITE`，打在白色输入带上等于白底白字。
本包所有浅底 `Textfield`（`UIMessenger` / `UISoftKey` / `UIMapSearchPanel` / `UIWorldMap`）
一律 `Color::Name::BLACK`，这里对齐；只改 classic 那一处（`UIChatBar.cpp:99`），
非 classic 的 `input_text`（第 235 行）背景不同，未动。

改后 `typed` 帧带内 view x 84.. 起有 'hi' 墨迹（22 px），视觉确认见 `fold-typed-zoom.png`。

## 7. 驱动侧教训（不是客户端缺陷）

**SendInput 打 scancode=28 的 Enter 进不了这个 SDL 客户端**（字母键正常）——表现是「按 Enter
没反应」，容易误判成聊天栏没接键。改用 `E:\project\_postkey.py return|esc`
（`PostMessage VK_RETURN/VK_ESCAPE`，lparam 带 scancode）后立刻生效。另外
`TOGGLECHAT` 的默认键位是**引号键**而不是 Enter（`UIKeyConfig.h:165/217`），Enter 走的是
`UI.cpp:381` 的聊天分支。

## 8. 复验判据（全绿）

```
python E:/project/_r27_fold7.py          # 登录进图 → ESC → Enter → 打 hi → Enter → 抓三帧
python E:/project/_ms_probe/_fold_verify.py
```

```
PASS  grey top rel == +3
PASS  grey h == 23
PASS  ink top rel == +9
PASS  ink x == 6
PASS  blue left == 537
PASS  edge @+2 dark (native 87,87,95) got (87, 87, 95)
PASS  typed text visible in white band (ink px=22)
```

端到端行为（同一轮真机）：进图灰带（无箭头、有蓝 +）→ Enter 出白带 + 目标框 → 打 `hi`
可见 → Enter 发送并回到灰带、灰带正文就是刚发的 `Admin : hi` → ESC 仍停在收起态。

## 9. 仍未闭环（不在本文件判据内）

- **展开态历史框背景**：原生 `聊天框展开时候.png` 里日志文字**直接压在场景上、没有深色底板**，
  而本端按先前 Q1 决策画了 `TooltipBackdrop`。要不要撤掉底板属**用户决策**，未擅自改。
- 聊天目标点击 → 悄悄话/目标选择弹层（oracle `悄悄话框.png` 261×117 已在手，现仅 `cycle_chat_target()` 循环标签）。
- UIMessenger「聊天窗」版式（oracle `好友列表框.png` / `组员列表框.png` / `拒绝列表框.png` / `家族联盟.png` / `好友、组员、家族、家族联盟、拒绝列表.png`）。
