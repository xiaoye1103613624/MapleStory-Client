# round54 · statusbar-hud/D3 聊天输入框启用系统 IME（中文可输入）

> 提交：`#devfix-chat-unicode-ime`　分支：`feature/dev_0.0.1`

## 0. 性质与授权

- 性质：**L-A 功能缺口修复**（文本输入 allowlist 关掉了全部非 ASCII 字节），不是崩溃修复、不是协议改动。
- 授权：`m04292` 一次性 Plan（"可以自测的自己测试……不能自测的记录下来后面我一起测试"）＋跨面决策矩阵 `statusbar-hud/D3` 的 recommend 原文：**"先零风险开 `UIChatBar.cpp` 的 `set_accept_unicode(true)` + 复用系统 IME，不重建 `CCtrlMLEdit` 候选窗状态机"**。
- 未改协议 / opcode / 包体 / WZ / IMG / XML；未改任何 `limit` 数值；未重建 IME 候选窗。

## 1. 缺陷逐字

### 1.1 客户端现状（修前）

`IO/UITypes/UIChatBar.cpp` 两处 `input_text` 构造，**都没有** `set_accept_unicode(true)`：

- classic 分支 `:139-146`

  ```cpp
  input_text = Textfield(
      Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK,
      Rectangle<int16_t>(...),
      70, 11
  );
  ```

- modern 分支 `:287-291`

  ```cpp
  int16_t input_text_limit = 70;
  int16_t input_text_marker_height = 11;
  input_text = Textfield(
      Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
      Rectangle<int16_t>(...),
      input_text_limit, input_text_marker_height
  );
  ```

默认值是 false：`IO/Components/Textfield.h:84 bool accept_unicode = false;`。

### 1.2 后果链（逐跳）

1. `IO/Components/Textfield.cpp:291-318 add_string()`：

   ```cpp
   if (accept_unicode) { if (c < 32) continue; }
   else { if (c < 32 || c >= 128) continue; }   // ← 修前走这条
   ```

   → **中文（UTF-8 每字节 ≥ 0xE4）被逐字节丢弃，一个字都进不来。**

2. `IO/Components/Textfield.cpp:320-339 modifytext()`：同口径二次过滤，即使绕过 `add_string` 也会被清掉。

3. `IO/UI.cpp:507-527 UI::focus_textfield()`：

   ```cpp
   if (focusedtextfield->accepts_unicode())
       Window::get().begin_ascii_text_input();
   else
       Window::get().end_ascii_text_input();
   ```

   → accept_unicode=false 时**根本不开 SDL 文本输入**，系统 IME 不会激活。

4. `IO/UI.cpp:294-297`：`text_input_active()` 为真时 `KeyType::Id::TEXT` 的 KEY_DOWN 被丢弃；为假时由 `Textfield::send_key` 的 TEXT 分支处理，而该分支只认 `action >= 32 && action < 128`（`Textfield.cpp:269-288`，单字节 ASCII）。→ **两条通路都进不了中文**。

5. `IO/Window.cpp:865-872`：`SDL_EVENT_TEXT_INPUT` → `UI::get().send_text(event.text.text)` → `IO/UI.cpp:496-505` → `focusedtextfield->add_string(text)`。整条 IME 提交链本身是通的，**唯一断点就是 allowlist + 不开 text input**。

### 1.3 渲染侧早已支持 CJK（不是缺字形）

- `Graphics/GraphicsGL.cpp:283 FT_Select_Charmap(face, FT_ENCODING_UNICODE)`；
- `:445-480 get_wide_glyph(Font&, uint32_t codepoint)`：按码点取宽字形，`FT_Get_Char_Index == 0` 缺字保护、`WIDE_CELL` 溢出保护；
- `:1270-1293` 布局逐**码点**累加 wordwidth（注释明记"中文一个字符 3 字节，旧实现只认 32..127，整段中文对 wordwidth 贡献恒为 0"）；
- 字体 `Configuration.h:556` → `fonts/SourceHanSerifSC/SourceHanSerifSC-Regular.otf`（思源宋体，含 CJK）。

→ 所以本轮是**打开闸门**，不是补渲染能力。

### 1.4 全仓已有 4 处先例（同一 API、同一注释口径）

| 位置 | 调用 |
| --- | --- |
| `IO/UITypes/UIMapSearchPanel.cpp:189` | `search_text.set_accept_unicode(true);`（注释：搜索框接收中文：启用系统 IME，UTF-8 直接进字图缓存） |
| `IO/UITypes/UIMiscShells.cpp:1251` | `input_field.set_accept_unicode(true);`（limit 64） |
| `IO/UITypes/UIMiscShells.cpp:1549` | `input_field.set_accept_unicode(true);`（limit = `max_len > 0 ? max_len : 64`） |
| `IO/UITypes/UINpcTalk.cpp:1690` | `input_field.set_accept_unicode(type == TalkType::SENDGETTEXT \|\| type == TalkType::SENDBOXTEXT);` |

聊天框是这一类里**唯一漏掉的**。

## 2. 最小修复

`IO/UITypes/UIChatBar.cpp` 两处各加 `input_text.set_accept_unicode(true);` + 依据注释：

- classic（`:150-156`）：5 行注释 + `input_text.set_accept_unicode(true);`，位置在 `set_text_shift` 之后、`set_enter_callback` 之前。
- modern（`:299-300`）：1 行注释 + `input_text.set_accept_unicode(true);`，位置在 `TextField` 构造之后、`set_enter_callback` 之前。

注释里写清三件事，供后来人别把它当"多此一举"删掉：

1. 不启用时 `Textfield::add_string` 丢弃全部 ≥128 字节，中文一个字都进不来；
2. `limit = 70` 是**字节**上限（`belowlimit()` 用 `text.size()`），中文 3 字节/字 → 实际约 23 字；
3. s9 `GeneralChatHandler.java:46` 按**字符数**限 127，放宽上限属行为变更 → 本轮**不动** 70，记台账。

`git diff --stat`：`IO/UITypes/UIChatBar.cpp | 9 +++++++++`，`1 file changed, 9 insertions(+)`；`git diff --check` 干净。

## 3. 设计取舍

| 方案 | 结论 | 理由 |
| --- | --- | --- |
| **只开 `set_accept_unicode(true)`**（本轮） | ✅ 采用 | 零风险、复用系统 IME、与 4 处先例一致、不动 layout/limit/协议 |
| 重建 `CCtrlMLEdit` 候选窗状态机 | ❌ 不做 | 083 原生确有该状态机（`?OnIMEComp@CCtrlMLEdit@@...@Z 0x4d33c2`、`?OnKey@CCtrlMLEdit@@MAEXII@Z 0x4d3159`、`?InsertString@...@Z 0x4d3b02` 等 9 个符号，含 `ZArray<unsigned long>` 候选列表），但重建等于自绘候选窗 + 接管 IME 组合态，量与风险远超 L-A |
| 顺手把 `limit` 从 70 改大 | ❌ 不做 | 属"改 happy-path"，且 s9 服务端按字符数限 127 与本端按字节限 70 的换算关系需单独裁决 |
| 改 `add_string` 的全局 allowlist 默认值 | ❌ 不做 | 账号/密码框依赖 ASCII-only 默认值，全局放开会引入不可见字符 |

副作用如实记录：中文 3 字节/字，`limit=70` 下实际约 23 个汉字（`tools/test_d3_chat_unicode.py` 的 `CJK_LIMIT` 场景已钉死 23）。修前这 23 个字一个都打不出来，修后能打 23 个 —— 比 0 好，但不到 70 字，属已知限制。

## 4. 自测与门禁

### 4.1 harness `tools/test_d3_chat_unicode.py`

17 条静态守卫 + 10 组行为复刻：

- 静态：classic/modern 两处构造都调了 `set_accept_unicode(true)`（用 `[\s\S]{0,700}` 窗口限定在各自构造块内，避免命中别处）；`limit` 仍是 70；`Textfield.h` 默认 false + setter + getter 三件齐；`add_string` 的 false 分支仍有 `c >= 128` 过滤（证明修复点没被误删）；`add_string` 的 true 分支只挡 `c < 32`；`UI::focus_textfield` 里 `accepts_unicode()` → `begin_ascii_text_input`；`send_text` → `add_string` 通路在；注释里写清"3 字节"与"127"。
- 行为：复刻 `add_string` 的逐字节 allowlist，对 `ascii_hello / ascii_space / cjk_nihao / cjk_mixed / cjk_long / ctrl_only / emoji4b` 七组输入各跑修前(false)/修后(true)：修前 CJK 全空、ASCII 原样；修后 CJK 完整、ASCII 不变、控制字符仍被挡。另加三组：超限 100 个 ASCII 修前修后都停在 70；30 个汉字修后恰好 23 字（`70/3`）；`68 ASCII + 1 汉字` 的混合边界输出长度 70、尾部为半个汉字字节序列 —— **逐字节插入语义可能截断汉字**，这是既有行为（`limit` 作用在字节上），发送侧 `packet_from_internal` 转 GBK 时会把非法序列原样发出、s9 侧按 Java 字符串接收。本轮不修（要修得改成按码点计数，属行为变更）。

实跑：`STATIC GUARDS checked=17 failed=0` / `SCENARIOS 10, bad=0` / `PASS` / `TEST_RC=0`。

harness 自身一处 bug（不计任务失败）：首跑 `focus_switches_ime` FAIL —— 正则 `focus_textfield[\s\S]{0,400}?begin_ascii_text_input` 窗口 400 字符不够（`UI::focus_textfield` 到 `begin_ascii_text_input` 之间夹着 10 行注释），改成 `void\s+UI::focus_textfield\([^)]*\)[\s\S]{0,1200}?accepts_unicode\(\)[\s\S]{0,120}?begin_ascii_text_input` 后通过。教训：正则窗口要按实际注释密度留足。

### 4.2 门禁

- `python tools/gen_opcode_matrix.py --check` → `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)`
- `git diff --check HEAD` → 干净
- `tools\build_smoke.bat` → `GATE OK` ×2 + `EXITCODE=0` + `BUILD_RC=0`
  - `build_log.txt`：`2026-11-10 11:35:33 / 175347 B` → `2026-11-10 11:49:39 / 6572 B`，首行即 `UIChatBar.cpp`（真重编译）
  - `E:\MXD\BeiDou-Client-xy\MapleStory.exe` mtime `11:49:38`、`4929024 B`（较前次 4928512 增 512 B）
  - `ERROR_COUNT 0`；`taskkill` 对 PID 56016 报 `Access is denied`（32K 空壳，历史同款，不影响构建）

## 5. 范围边界

- 改：仅 `IO/UITypes/UIChatBar.cpp` 两处构造后各加一行调用 + 注释。
- 未改：`Textfield` 实现、`UI::focus_textfield`、`add_string` 的 allowlist 口径、`limit = 70`、任何网络包体/opcode、任何 WZ/IMG/XML、任何布局坐标。
- 未做：自绘 IME 候选窗（详见 §3 取舍表）。

## 6. 真机 backlog（需用户一起测）

| ID | 场景 | 期望 |
| --- | --- | --- |
| 30-1 | 经典 HUD 聊天框输中文 | 输入法弹出、候选窗可用、上字正常、字图无竖线伪影 |
| 30-2 | 现代 HUD 聊天框（`view_max` 展开态）输中文 | 同上；白字在深底可读 |
| 30-3 | 中文发送后对端接收 | s9 收到 GBK 原文，不出现乱码或截断（23 字上限内） |
| 30-4 | 账号/密码框回归 | 仍只收 ASCII，中文被丢弃（ASC-II-only 默认值未被误改） |
| 30-5 | 地图搜索 / SoftKey 文本 / NPC GetText 三处回归 | 行为与修前一致（这三处本就开着，本轮不应影响） |

## 7. 提交 scope

- `IO/UITypes/UIChatBar.cpp`（修复）
- `tools/test_d3_chat_unicode.py`（harness）
- `docs/features/development-and-fix-plan-2026-11/notes/wave2-chat-unicode-ime-recheck-20261110.md`（本文）
- `docs/features/development-and-fix-plan-2026-11/README.md`（§4 Wave2 追加 2-19）
- `docs/features/pending-live-tests/README.md`（追加 §30）

## 8. 证据优先级

`live 源码逐字` > `Textfield/UI 通路语义` > `s9 服务端包体与长度限制` > `083 IDB 符号表` > `跨面决策矩阵 recommend` > `旧台账`。

补充教训：

1. 改 CRLF 文件时锚点必须带 `\r\n`（本轮 `s=` 首试用 `\n` 结尾 → `OLD_FOUND 0`，改用 `nl` 变量后一次命中）。
2. 插入注释后要复核语句顺序（首版把 5 行注释插到 `set_text_shift` 之前，导致"注释紧跟 set_text_shift"读起来像注释在说 shift，已重排为 shift → 注释 → accept_unicode）。
3. 静态守卫的正则窗口要按注释密度留足，400 字符在注释密集处不够。
