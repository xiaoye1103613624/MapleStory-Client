# round55 · social-economy/D1 后半 + corewindow/D14：v95 注释换 083 依据 & 三处确认文案 i18n

> 性质：**L-A（纯本地，不碰协议 / 不改行为 / 不改坐标 / 不改包体字节）**。前者是注释依据替换，后者是把三处硬编码英文提示改成 `Locale::text_or` 并补 locale 键。

## 0. 授权与目标

- 用户 m04292 一次性授权：可改客户端源码 / 自测 / 测试通过即提交 / 遇客户端占用直接关 / 不能自测的记录到 backlog。本计划（`docs/features/development-and-fix-plan-2026-11/README.md`）为该轮一次性 Plan，不重开。
- round55 `<goal_round>`（m08039）目标原文：以 Angel(13339) 083/v83 带符号 IDB 为权威校准源，逐功能 REA→IDA→IMG 只读取证 → 决策点 + 一次性 Plan → 实现。
- 本轮选取跨面决策矩阵里两条 K1（零风险可直接做）：
  - `social-economy/D1` 后半 —— 「只换注释依据引 Angel `CUIMessenger`/`CUIFamily`，不改行为/协议/坐标」。
  - `corewindow/D14` —— 「P0 零风险先做：`UIKeyConfig.cpp:731` 英文、locale 英文原值批次」。
  - 依据：`docs/features/angel-calibration-d-matrix-2026-11/README.md` §5 批量裁决表 + §6 P0 代表项。

## 1. 缺陷逐字与事实链

### 1.1 social-economy/D1 后半：14 处 `v95` 注释依据（095 线污染）

改前实测（`io.open(..., newline='')` 计数）：

| 文件 | `v95` 出现 | `0x7F34C0` |
| --- | --- | --- |
| `IO/UITypes/UIMessenger.cpp` | 10 | 0 |
| `IO/UITypes/UIMessenger.h` | 6（含 1 处地址） | 1 |
| `docs/features/messenger-btclose-coords/README.md:16` | 0 | 1 |

**硬约束依据**：MEMORY.md critical —— 参照基准固定 083/v83，`Maplestory95.i64` / port-095 及其地址仅登记存在，不作 083 判定依据。这些注释正在把 095 线函数名/地址当权威依据写给后来人看，属「证据链污染」，不改代码行为但会把未来的判断带偏。

### 1.2 `0x7F34C0` 在 083 IDB 里不是 `CUIMessenger::SetCtrl`

`py_eval` 直查（Angel 13339）：

```text
ida_funcs.get_func(0x7F34C0) -> FUNC 0x7f3483 sub_7F3483      # 未命名函数
```

`entity_query {"kind":"functions","filter":"*CUIMessenger*","count":80}` 得 27 个符号，其中：

```text
?SetCtrl@CUIMessenger@@QAEXXZ  0x8505e2  0x760
?OnCreate@CUIMessenger@@UAEXPAX@Z  0x84ddb0  0x56e
?SetAvatar@CUIMessenger@@QAEXJABUAvatarLook@@H@Z  0x84f44b  0xbad
?Update@CUIMessenger@@UAEXXZ  0x84e930  0x238
?OnChat@CUIMessenger@@QAEXAAVCInPacket@@@Z  0x8515d7  0x378
?ProcessChat@CUIMessenger@@IAEXXZ  0x851d3b  0x1cd
```

⇒ `UIMessenger.h:106` 的 `v95 CUIMessenger::SetCtrl（0x7F34C0）` 是既有污染；**坐标结论（276,6）经 083 立即数独立复核为真**（见 1.3），所以只换依据、不动常量。

### 1.3 083 侧四条独立复核（证明「换依据不改结论」）

- `CUIMessenger::SetCtrl @ 0x8505e2` 关闭钮坐标：反编译 `:103` 为 `a4: *(this + 27) != 2 ? 276 : 166` —— **展开态 x=276、最小化态 x=166**；本端无 Min 态，常量 276 沿用正确。
- `CUIMessenger::OnCreate @ 0x84ddb0` blink 资源：`:108-109` `StringPool::GetBSTR(a2: &v52, a3: SP_1858_UI_UIWINDOWIMG_MESSENGER_BLINK)` —— 与注释里的「标题条注意标」一致。
- `CUIMessenger::SetAvatar @ 0x84f44b` 气球位移：两处 `IWzGr2DLayer` 调用参数分别为 `a2: -45, a3: 4`（`:468-471`）与 `a2: -28, a3: -100`（`:615-618`）。**083 里不存在 (-45,-100) 这一对**；本端注释把「y -100」当主依据、x 用 -45，属近似取值，已在注释里如实写明并指向本节。
- `CUIMessenger::Update @ 0x84e930` 节流与 blink：`*(this+30)` 每 tick 递减 30ms、归零后置 `3000`；`*(this+31)` blink 计数上限 30、到 0 重置 30；与 `TYPING_THROTTLE_MS=3000` / `BLINK_TICKS=30` / `BLINK_HALF=15` 逐字对应。
- `CUIMessenger::Update` 打字指示包体：`COutPacket(122)` + `Encode1(6)` + `EncodeStr(GetCharacterName + a0)`，与 `MessengerChatPacket(self + (typing ? "1" : "0"))` 语义一致。
- `CUIMessenger::ProcessChat @ 0x851d3b`：`'/'` 开头走 `HandleSlashCommand`，否则拼 `"name : text"` 发聊天；与 `handle_messenger_chat` 的 `" : "` 分流一致。

### 1.4 corewindow/D14：三处硬编码英文提示

```cpp
// IO/UITypes/UIKeyConfig.cpp（改前）
:731  static const std::string& message = "Would you like to revert to default settings?";   // Buttons::DEFAULT
:759  static const std::string& message = "Would you like to clear all key bindings?";        // Buttons::DELETE
:1006 static const std::string& message = "Do you want to save your changes?";                // safe_close()
```

D14 原文只点了 `:731`（P0 提取时口径），实测同文件同形态共三处。文件 include 区（`:18-42`）无 `Util/Locale.h`。

**Locale 语义（决定用哪个 API）**：

```cpp
// Util/Locale.h:47 / :55
static std::string text(const std::string& key) { return Locale::get().tr(key); }
static std::string text_or(const std::string& key, const std::string& fallback) {
    return Locale::get().has(key) ? Locale::get().tr(key) : fallback;
}
// Util/Locale.cpp:97-113
std::string Locale::tr(key) const { primary.find -> fallback_zh.find -> return key; }
bool Locale::has(key) const { primary.find != end || fallback_zh.find != end; }
```

⇒ `tr()` 缺键会**返回 key 本身**（把 `keyconfig.confirm_default` 原文弹给玩家），因此不能用 `text()`；必须 `text_or(key, 原英文)`。

**用户 2026-11-02 决策 B**：`en-US.ini` 中被判定为「原生英文原值」的条目保持英文，不翻译。⇒ en-US 侧三段新键的值**逐字等于改前英文原值**，zh-CN 侧给出中文。

## 2. 最小修复

### 2.1 social-economy/D1 后半（3 文件、纯注释）

`IO/UITypes/UIMessenger.cpp` 10 处、`IO/UITypes/UIMessenger.h` 6 处、`docs/features/messenger-btclose-coords/README.md` 1 处，统一把 `v95 xxx` / `Maplestory95 ... 0x7F34C0` 换成 Angel 083 的符号名 + 真实地址。全部常量与逻辑行**逐字未动**（见 §3.2 常量守卫）。

### 2.2 corewindow/D14（3 文件）

```cpp
// 新增 include
#include "../../Util/Locale.h"

// 三处改为
const std::string message = Locale::text_or("keyconfig.confirm_default", "Would you like to revert to default settings?");
const std::string message = Locale::text_or("keyconfig.confirm_clear", "Would you like to clear all key bindings?");
const std::string message = Locale::text_or("keyconfig.confirm_save", "Do you want to save your changes?");
```

`locale/zh-CN.ini`（LF）与 `locale/en-US.ini`（LF）各追加一段 3 键（zh 带 `#` 说明行）：

```ini
# UIKeyConfig 三处确认框（Angel 083 CNoticeDlg @ CUIKeyConfig::CNoticeDlg 0x83665b）。
keyconfig.confirm_default=确定要恢复默认设置吗？
keyconfig.confirm_clear=确定要清除所有键位绑定吗？
keyconfig.confirm_save=要保存本次修改吗？
```

```ini
# UIKeyConfig confirm dialogs. Values are the pre-i18n English literals (decision B: keep as-is).
keyconfig.confirm_default=Would you like to revert to default settings?
keyconfig.confirm_clear=Would you like to clear all key bindings?
keyconfig.confirm_save=Do you want to save your changes?
```

### 2.3 设计取舍

| 方案 | 结论 | 理由 |
| --- | --- | --- |
| A. `Locale::text(key)` | ✗ | 缺键返回 key 原文（`Locale::tr` 尾部 `return key;`），会把 `keyconfig.confirm_default` 弹给玩家 |
| B. **`Locale::text_or(key, 原英文)`** | ✓ 采用 | 命中给文案、缺键逐字回退旧行为，零回归面 |
| C. 保留 `static const` | ✗ | `text_or` 是运行时查表（语言可在运行期切换），必须去掉 `static` |
| D. 只改 `:731` 一处 | ✗ | 同文件三处同形态、同属一个确认对话框族（083 `CNoticeDlg @ 0x83665b`），只改一处会留两处英文 |

**为什么不顺手把 `limit=70` / 其它 locale 英文原值也改**：属行为变更（字符上限）或已按决策 B 明示保留，不在 D14 零风险范围，记录在 §5 台账。

## 3. 自测与门禁

### 3.1 harness

新增 `tools/test_round55_d1_d14.py`（A–E 五组）：

- **A**：三文件 `v95` / `0x7F34C0` 归零；6 个 083 地址就位（cpp 6 / hdr 4 / doc 1）。
- **B**：`UIMessenger.h` 14 个布局/动画常量逐字不变（`BT_CLOSE_X=276` / `BT_CLOSE_Y=6` / `LOOK_SCALE=0.70f` / `LOOK_FOOT_DX=44` / `LOOK_FOOT_DY=-10` / `LOOK_NAME_DX=44` / `LOOK_NAME_DY=2` / `BALLOON_DX=-31` / `BALLOON_DY=-70` / `BLINK_TICKS=30` / `BLINK_HALF=15` / `BLINK_DRAW_X=260` / `BLINK_DRAW_Y=4` / `TYPING_THROTTLE_MS=3000`）。
- **C**：`UIKeyConfig.cpp` 有 Locale include、无 `static const std::string& message`、三条 `Locale::text_or("key", "原英文")` 正则命中、`emplace<UIOk>(message, onok)` 恰 3 处。
- **D**：两 locale 文件各含 3 键；en-US 值逐字等于改前英文；zh-CN 值含 CJK。
- **E**：`text_or` 行为复刻 —— 缺键返回 fallback（=改前行为）、命中返回文案。

实跑：

```text
[A] 17 OK / [B] 14 OK / [C] 6 OK / [D] 9 OK / [E] 6 OK
PASS: round55 (social-economy/D1 comments + corewindow/D14 i18n)
TEST_RC=0
```

### 3.2 门禁与构建

```text
python tools/gen_opcode_matrix.py --check
-> GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)

git diff --check HEAD
-> (empty)

tools\build_smoke.bat
-> GATE OK / EXITCODE=0 / 首个编译单元为 UIKeyConfig.cpp（真重编译）
```

## 4. 范围边界

- 未改：任何 `.cpp/.h` 的行为行、任何常量值、任何 opcode / 包体 / 协议字段。
- 未改：`docs/features/guild-bbs-ui/README.md:20`（该行是 `port-095 v95 CUIGuildBBS::...` 的**对照记录**，不是把 095 当权威依据，本轮不动，登记在 §5）。
- 未改：`locale/*.ini` 里既有的 44 条 en-US 中文残留（决策 B 范围内，需用户裁决是否翻译）。

## 5. 真机联测与后续台账

- 55-1 家族聊天（Messenger）：确认关闭钮位置与改前一致（应仍为 276,6），blink 起始计数与打字气泡位置不变。
- 55-2 聊天窗打字指示：空/非空输入框切换时对端仍看到「正在输入」，无重复发包。
- 55-3 快捷键设置窗三处确认框在 zh-CN 下显示中文，`en-US` 下显示改前英文原值。
- 55-4 语言切换（不重启）后确认框文案随之更新（`text_or` 为运行时查表）。
- 台账 L1：`docs/features/guild-bbs-ui/README.md:20` 的 port-095 对照行，需用户裁决是否也标注为「仅对照、非依据」。
- 台账 L2：`locale/en-US.ini` 44 条中文残留（`trade.*` 19 / `dailygift.*` 14 / `mapsearch.*` 5 / `mapinfo.*` 3 / 其它 3），属决策 B 待裁决批次。

## 6. 提交 scope

```text
IO/UITypes/UIMessenger.cpp
IO/UITypes/UIMessenger.h
IO/UITypes/UIKeyConfig.cpp
locale/zh-CN.ini
locale/en-US.ini
docs/features/messenger-btclose-coords/README.md
tools/test_round55_d1_d14.py
docs/features/development-and-fix-plan-2026-11/notes/wave2-d1-d14-recheck-20261110.md
docs/features/development-and-fix-plan-2026-11/README.md
docs/features/pending-live-tests/README.md
```

## 7. 证据优先级与教训

证据优先级（沿用 `docs/features/development-and-fix-plan-2026-11/README.md` §2.3）：`opcode-matrix.json` > live 源码 > Angel 083 IDB 反编译 > git blame/历史台账 > 旧注释。

教训：

1. **注释里的「地址」也是证据**：`0x7F34C0` 在 083 指向 `sub_7F3483`，说明「注释写了个真地址」并不等于「这地址在本版本对」。改注释前必须 `ida_funcs.get_func` 复核。
2. **`Locale::tr` 缺键返回 key 本身**：任何把硬编码文案换 i18n 的地方都必须用 `text_or`，否则语言表没跟上时会退化成显示键名。
3. **决策 B 是「值不动」不是「不接线」**：en-US 侧新增键仍写英文原值，接线 + 保留原值两者不冲突。
4. CRLF 文件（`UIMessenger.*` / `UIKeyConfig.cpp` / `messenger-btclose-coords/README.md`）用**不含换行的锚点**最稳；LF 文件（`locale/*.ini`）追加后要确认末尾仍是单个 `\n`。
