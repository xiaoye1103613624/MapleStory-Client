# 快捷栏交互 + 丢弃弹窗修复（2026-11-18）

> 分支 feature/dev_0.0.1 ｜ 构建 `tools\build_smoke.bat` EXITCODE=0、GATE OK，UIStatusBar.cpp / UINotice.cpp / UIItemInventory.cpp 重新编译，无 error。
> 途中一次 LNK1104（exe 被运行中的客户端 PID 46872 占用），taskkill /F /PID 后重建通过。
> live exe SHA256 `0eadb428567099257f004afd406717878f144286bcc161936218923d6738c182`（00:25:55），归档副本一致。登录器会回滚 exe，交付前重核。

## 用户确认的两项前提

- 两张截图来源端 = **本端**（刚构建的 `E:\MXD\BeiDou-Client-xy`）→ #6/#7/#8 按本端缺陷处理。
- 快捷栏图标拖拽换位 = **要新增** → 本次实现。

## 9 项逐条结论（行号级）

| # | 问题 | 根因 | 处置 |
|---|---|---|---|
| 1 | 单击逻辑 | `send_cursor` 原按下即 `activate`（keydown）无松开配对 | 重写为按下记起点→松开判单击/拖拽 |
| 2 | 点攻击无限连击 | `Stage.cpp:324-325` 每帧读 `is_key_down(ATTACK)` 连发；快捷栏只 keydown，`keysdown` 永 true | ✅ 单击=keydown+keyup 一次（不再残留） |
| 3 | 图标无法抓取/换位 | 快捷栏图标是查表裸 Texture，无 Icon 对象、无拖拽入口 | ✅ 新增拖拽换位（详见下） |
| 4 | 同物品多份 | `Keyboard::assign` 只覆盖不清旧位 | ✅ `send_icon` 绑定前扫其它快捷槽去重（移动语义） |
| 5 | 弹窗没国际化 | 三句硬编码英文 | ✅ `Locale::text("inventory.drop_*")` + 双语 key |
| 6 | 按钮错位 | 弹窗没居中导致的相对观感 | ✅ 随居中修复一并归位（按钮本就按弹窗局部坐标，不再单独偏移） |
| 7 | 数量没显示 | numfield bounds 用绝对 `position+...` 算死，center 重算后失配；且初值在 FOCUSED 前设 | ✅ bounds 改局部坐标 + draw/命中统一加 position；change_text 移到 FOCUSED 后；digits_only + 白色字 |
| 8 | 弹窗没居中 | 仅非 1280×720 才 center，1280×720 落到偏移位置 | ✅ 统一 `center_on_screen()`（UINotice.cpp:108） |
| 9 | Angel 没实现吗 | 本端是 Journey 自研简化版，非 Angel 原生移植；无 Angel 拖拽函数证据 | 说明项 |

## 关键实现

### 快捷栏拖拽换位（#3）
- `UIStatusBar.h` 新增 `QsIconType`（Icon::Type 子类，填 get_item_id/get_skill_id/get_macro_index 让 `send_icon` 识别；`is_macro_source()=true` 走黏住态落点）。
- `setup_qs_drag_icon`（UIStatusBar.cpp:2349）：从起点格绑定构造可拖 Icon 并 `UI::drag_icon`。
- `send_cursor`（:2486-2544）状态机：按下记起点（不立即使用）→ 移动超 8px 且离开起点格 → 起拖；松开未移动且仍在原格 → 单击使用（keydown+keyup）；拖到目标格由 `UIStateGame::drop_icon → StatusBar::send_icon` 结算，配合去重实现换位。
- 走黏住态原因：UIStateGame 对普通 Icon「松手在快捷栏不结算」（:853-867），黏住态改成下一次点击目标格落点（:846），可靠且不改全局拖拽逻辑。

### 攻击连发（#2）
单击走 keydown→keyup 一次，`keysdown[ATTACK]` 不残留；按住仍由 Stage 每帧连击（原生语义）。

### 丢弃弹窗（#5/#7/#8）
- i18n：zh-CN.ini:199 / en-US.ini 新增 `inventory.drop_quantity/untradable/cash`，UIItemInventory.cpp:1967 改用 Locale。
- 居中：UINotice.cpp:108 统一 center_on_screen（删除仅非 1280×720 条件与手动偏移叠加）。
- 数量显示：numfield bounds 改相对弹窗局部坐标（:251 区），draw（:288）与 send_cursor（:303）统一加 position；初值在 FOCUSED 后设；digits_only + WHITE 字。

## 验收状态

- 编译/链接：✅ EXITCODE=0、GATE OK、无 error C/LNK/fatal。
- SHA 闭环：✅ live 与归档均 `0eadb428…`。
- **真机验收：⏳ 未做**。攻击停连、拖拽换位、弹窗居中/按钮/数量、i18n 中文——均未登录实测。

## 复测步骤（本端真机）

1. 单击快捷栏「攻击」→ 挥一次即停；按住 → 持续连击。
2. 拖动快捷格图标到另一格 → 两格绑定交换（原格清空，无重复）。
3. 背包拖出道具丢弃 → 弹窗中文「要丢弃多少个？」、屏幕正中、按钮贴底对齐、输入框预填当前数量。
