# 聊天输入空格双写修复（2026-11-18 00:45）

> 用户报：`!item` 命令输入时空格总是变成两个空格，导致命令格式解析失败。
> 味道：🔴 华为 · 以点到面，全局汇聚点一次修完。

## 一、现象与口径

- 截图 `XiaoYe_2026-11-18_00-22-17.png`（本端实机，时间戳命名，可作线索）：
  `[Notice] 輸入："item <品名> <數量>"`。用户实测空格变双空格。
- 截图只作线索，根因以代码证据为准。

## 二、根因（行号级，双路径插入）

空格键按下时有**两条插入路径同时命中** unicode 文本输入框：

1. **SDL_TEXT_INPUT 路径**：聊天框 `UIChatBar.cpp:184/351 set_accept_unicode(true)`
   → `UI::focus_textfield`(`UI.cpp:561-562`) `begin_ascii_text_input()`
   → `SDL_EVENT_TEXT_INPUT`(`Window.cpp:872-877`) → `UI::send_text(" ")` → `add_string(" ")`。
2. **KEY_DOWN ACTION 路径**：空格键 `GLFW_KEY_SPACE` 在 `Keyboard.cpp:95` 注册进
   `textactions[GLFW_KEY_SPACE]=KeyAction::Id::SPACE`
   → `Keyboard::get_text_mapping`(`Keyboard.cpp:227-230`) 返回 `Mapping(ACTION, SPACE)`
   → `UI::send_key`(`UI.cpp:298-303`) 的跳过条件**只覆盖 `KeyType::Id::TEXT`、不覆盖 ACTION**
   → `Textfield::send_key(ACTION, SPACE)`(`Textfield.cpp:230-233`) 又 `add_string(" ")`。

→ 一个物理空格 = 两个字符 → `!item <ID> <数量>` 被解析成 `!item  <ID>  <数量>`（双空格）→ 服务端命令格式校验失败，回提示文案。

## 三、以点到面：影响面不止 !item

`set_accept_unicode(true)` 的输入框是一大批（全都会双空格）：
`UIChatBar`(聊天)、`UICommonCreation`(创角名)、`UILogin`(账号/密码)、`UIMessenger`(悄悄话)、
`UIMapSearchPanel`/`UIWorldMap`(地图搜索)、`UISoftKey`(软键盘文本)、`UIMiscShells`(多处)。
→ 必须修在**全局汇聚点**，不能只补聊天框一处。

## 四、修复（单点，全局生效）

`IO/UI.cpp::UI::send_text`（:540-551）：过滤"纯 ASCII 空格"的 TEXT_INPUT，
保留 ACTION/SPACE 路径的单次可靠插入。
- 为什么过滤这条而非那条：ACTION/SPACE 无条件可靠（`text_input_active` 与否都会走），
  TEXT_INPUT 才是不稳定重复源；宿主不发 TEXT_INPUT 时空格靠 ACTION PATH 仍正常。
- 中文 IME 选字/上屏是汉字不是纯空格；全角空格 U+3000 不命中（only `text==" "`）；
  ctrl+v 粘贴走 `UI.cpp:289` clipboard 分支不经 send_text → 均不受影响。

## 五、验证与边界（诚实声明）

- 构建 `BUILD_EXITCODE=0`，`error C`=0；但首次构建被**并行会话**正在编辑的
  `UICharInfo.h:230 operator bool()` C2549 打断（与他改动无关，等对方修完后重试通过）。
- 产物 vs live SHA 一度不一致（并行会话在 00:38 也部署了自己的构建 `b76e5082`），
  本轮最终部署我的产物 `1456d043b8edc55c34012303f6400e61f1717a75a9b216129aa5bc12a01580f0`，
  三处实时一致。
- **真机未验收**：需在客户端打 `!item <ID> <数量>` 确认单空格、命令成功，
  并确认中文输入法/全角空格/粘贴未受影响。编译通过 ≠ 功能验收。
- 工作区含并行会话的 UICharInfo 等改动，本轮全量构建一并编入（当前真实状态），未覆盖他人工作。
