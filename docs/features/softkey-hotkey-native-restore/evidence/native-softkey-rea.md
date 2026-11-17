# 原生软键盘对照证据（REA / IDA / IMG）

> 用途：回答"为什么很多按键都会打开软键盘、软键盘是否本该只在密码输入时出现"。
> 结论一句话：**原生 v83 北斗客户端不启用软键盘；软键盘的唯一原生场景是选角 PIC/二次密码（SPW）。本端"按键/按钮弹软键盘"是历史波次显式接线，非原生行为。**

## 0. 目标二进制身份

| 项 | 值 |
|---|---|
| 路径 | `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe` |
| 格式 / 架构 | PE / x86（32 位） |
| sha256 | `1198fa57ca5a7c489bae43ec13c69681d9cabe0f96762f3dc0357facf2e7d4df` |
| 版本判定 | v83 本体（与 s9 服务端 v83 协议一致） |
| REA 打开方式 | `open_binary(provider_id=ida)`，会话已 `close_binary` 收口 |
| 备注 | 本会话 `search_strings` 因上游 IDA MCP 返回 sha256 格式异常（非 64 位小写 hex）无法取回结果；改用已落盘的一手反编译产物作为证据，未重试相同请求。 |

## 1. 原生 v83 不引用软键盘字符串（负向证据）

一手产物：`docs/features/softkey-text-mode/evidence/ida_softkey_dump.txt`

```
##### BeiDou-ClientV17.7\_audit_copy.i64 segs=[...] #####
pat b'S\x00o\x00f\x00t\x00K\x00e\x00y\x00' hits=0 []
pat b'SoftKey' hits=0 []
pat b'B\x00t\x00N\x00u\x00m\x00' hits=0 []
pat b'K\x00e\x00y\x00W\x00i\x00n\x00d\x00o\x00w\x00' hits=0 []
code funcs referencing patterns: 0

##### BeiDou-ClientV17.7\suipre.dll.i64 segs=[...] #####
（同样 4 种编码 hits=0，引用函数 0）
```

- 三种编码（ASCII / UTF-16LE / 函数名片段）在 v83 主 exe 与 suipre.dll 中**均为 0 命中**。
- 交叉印证：`docs/features/ui-v83-window-restore/evidence/native-verdicts-2026-11-03/copy-literals.txt:110-126` 记载 v95 参照 exe 才加载 `n/SoftKey/BtNum/%d`（地址 `0xB5563C` / `0xB5568C` / `0xB5585C` / `0xB55904`），而 v83 exe 自身三编码 0 命中 ⇒ **v83 原生未启用软键盘，资产来自更高版本数据**。

## 2. 原生唯一软键盘场景：选角 PIC / 二次密码（正向证据）

一手产物：`E:\MXD\BeiDou-Client-xy\REA反编译\095REA\evidence\_ms_probe\dec_send\5da2a0.json`

函数：`void __thiscall CLogin::SendSelectCharPacket(CLogin *this)` @ **0x5da2a0**

关键分支（`m_bLoginOpt` = 登录选项，决定选角发包形态）：

| case | 行为 | 是否弹软键盘 |
|---|---|---|
| 0 | `CLoginUtilDlg::Notice(95)` → `CSoftKeyboardDlg::InitializeSecondaryPassword(&sSPW, ...)`，成功后发 opcode 28（含 SPW 字符串 + 双 MAC 字段） | **是**（PIC 二次密码） |
| 1 | `ZAllocEx::Alloc(0xC74)` → `CSoftKeyboardDlg::CSoftKeyboardDlg(nMin:6, nMax:16)` → `DoModal` → `GetResult(&sSPW)` → 发 opcode 29 | **是**（PIC 输入，最短 6 / 最长 16） |
| 2 / 3 | `COutPacket(nType:19) = CHAR_SELECT`，仅写 cid + MAC 串 | **否**（直接进角色，不弹） |

相关引用（同一 json 的 refs）：
- `?InitializeSecondaryPassword@CSoftKeyboardDlg@@SAH...` @ `0x715ed0`
- `??0CSoftKeyboardDlg@@QAE@JJ@Z` @ `0x715c10`（构造带 min/max）
- `?GetResult@CSoftKeyboardDlg@@QAE?AV?$ZXString@D@@XZ` @ `0x5d5370`
- `?DoModal@CDialog@@QAEJXZ` @ `0x4fe7a0`

补充一手产物：`docs/features/statusbar-hud-angel-calibration-2026-11/evidence/angel-hud-decompile.json:392-402` — `CUISoftKeyboard::OnCreate @ 0x832979`、`CSoftKeyboardDlg::GetResult @ 0x5f7691`、`CSoftKeyboardDlg::OnCreate`，均为控件创建/取结果，**无"按键处理→打开软键盘"逻辑**。

**结论**：原生软键盘由服务端 `m_bLoginOpt` 状态驱动，只在选角 PIC/SPW 场景出现；不存在"按 S/E/C/W 等热键弹软键盘"的原生路径。

## 3. 两套软键盘资产（IMG 层）

取证：`docs/features/softkey-text-mode/evidence/softkey-layout-audit-2026-11-06.md:16-57`、同目录 `softkey-img-dump.txt`

| 资产 | 用途 | 几何 / 结构 |
|---|---|---|
| `UI/Login.img/Common/SoftKey/*` | PIC 二级密码**竖版随机键盘** | backgrnd 140×280；Tab 3 页签；`BtNum/BtHighCase/BtLowCase` 0..9 |
| `UI/UIWindow.img/SoftKeyboard/*` | 窗口式 **37 键 QWERTY** 虚拟键盘 | `backgrnd/1/back` 315×127；37 键 21×22；`KEY_CHARS="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-"` |

- 这是**两套不同资产**，本端把窗口式 QWERTY 键盘复用到了所有输入场景；原生只在 PIC 场景用竖版随机键盘（且 v83 不加载任何软键盘资产）。
- `Please re-enter your PIC.` 字符串仅与 PIC 路径关联（`docs/features/native-map-object-re/evidence/beidou_v177_decrypted_strings.tsv:4115-4126`）。

## 4. 本端过度实现的流水记录（非原生）

| 文档 | 内容 |
|---|---|
| `docs/features/beidou-v17-parity/notes/r161-userlist-softkey-hotkeys.md:1-7` | `A/G/C/I/L/E/M/P/W` 好友/组队/密语热键 → SoftKey |
| `docs/features/beidou-v17-parity/notes/r254-softkey-saturate-status.md:1-11` | GameMenu：字母+Tab+F1–F12+符号键已满；ShortCut：1–0+F/M/T/R/$/B/N/A/Y/H/G/U/P/V/W/D |
| `docs/features/beidou-v17-parity/notes/r285-keyconfig-autopot-softkey.md:1-7` | `H/M` 自动药 ID → SoftKey + CHANGE_KEYMAP 91/92 |
| `docs/features/ui-batch-fix-2026-11/README.md:21-47` | "465 处 `emplace<UISoftKey>`，三参数重载默认 `Mode::Pic`"；已删部分 A/P/Y 枝 |
| `docs/features/softkey-text-mode/README.md:5-32` | 本端设计：Pic 掩码 `/最短6/check_pic`；Text 明文/最短1；Mode 默认 Pic |

以上均为**本端实现清单/状态记录**，无一标注"原生还原"；结合 §1/§2/§3 可确证：这是历史波次把 PIC 控件当通用输入框的过度实现。

## 5. 本端关键代码锚点（改造禁区与总闸）

- **禁改（合法 PIC）**：`IO/UITypes/UICharSelect.cpp:1250-1268`（`require_pic` → `UISoftKey` / `SelectCharPicPacket`）、`IO/UITypes/UIViewAllChar.cpp:194-203`（`sel_use_pic` → `PickAllCharPicPacket`）。
- **总闸**：`IO/UI.cpp:317-327` 未绑定键的 ASCII 兜底（`map_action = keycode`）是裸字母捷径下传源头；方向键/回车兜底（`:319-324` 注释）须保留。
- **默认模式**：`IO/UITypes/UISoftKey.cpp:165-166` 三参构造默认 `Mode::Pic`。
- **批量分支示例**：`IO/UITypes/UIStatusBar.cpp:1367-1564`、`UIMiscShells.cpp:6568-6807(UIGameMenu) / :4111-4320(UIShortCut)`、`UIUserList.cpp:726-747`、`UIKeyConfig.cpp:990-1056`、`UIWorldSelect.cpp:1100-1181`、`UIAdmin.cpp:102-104`、`UIViewAllChar.cpp:156-169`。
- **可复用的原生输入通路**：`IO/Window.cpp:872-879`（`SDL_EVENT_TEXT_INPUT` → `UI::send_text`）、`IO/UI.cpp:525-552`（写 `focusedtextfield`）、`Window.cpp:908-949`（`begin/end_ascii_text_input`）。
