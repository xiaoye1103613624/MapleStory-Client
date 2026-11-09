# round36 · Wave1 收尾：1-3 死注册 / 1-5 IME 光标逐字核准 + UTF-8 光标边界修复

> 性质：开发/修复阶段（用户 m04292 授权「可自测自己测 / 功能测试完成就提交 /
> 遇 MapleStory.exe 占用直接关闭」）。本轮改 `IO/Components/Textfield.cpp`；
> 协议/WZ/IMG 一律只读，083/v83 唯一基准禁 095，校准≠改回原生，s9 优先，
> 数值重合≠语义重合。味道：🟠 阿里味。失败计数：已确认失败 0 次 → L0
> （harness 里一行硬编码 `0` 占位输出与下一行 `5` 自相矛盾，属自测脚本口径
> 毛边、非任务失败，已按事实删掉重跑，不隐藏）。

## 0. 结论

1. **1-3「事实死注册」是有意裁决，不是缺口**：`Net/PacketSwitch.cpp:58-62`
   与 `:433-437` 已逐字写明「GENDER_DONE/CONFIRM_EULA_RESULT 在 s9 下不发
   （性别/EULA 已由 LOGIN_RESULT(auth success) 闭环）……注册仅作协议完整性」；
   `Net/Handlers/LoginHandlers.cpp:454-486` 两个 handler 不但非空，还带
   2026-11-03 实机修复（拆 `UIGender` 欢迎框 / `UILoginWait` / `TOS` 残留窗），
   `LoginResultHandler::handle():50-57` 也在 LOGIN_RESULT 到达即无条件清三项。
   照 11-04 旧账「标注/收敛死注册」硬改会把这条兼容链删掉 → 销案、零改动。
2. **1-5「中文 IME 光标」主体已实现**：`Textfield.h:55-58` 有
   `set_accept_unicode(bool)` / `accepts_unicode()`；`IO/UI.cpp:496-527` 有
   `send_text()`（`focusedtextfield->add_string(text)` UTF-8 提交）与
   `focus_textfield()`（按 `accepts_unicode()` 决定 `begin_ascii_text_input`
   / `end_ascii_text_input`）；`IO/UITypes/UIMapSearchPanel.cpp:188` 明确启用
   中文 IME；`Textfield.cpp:192-202`(BACK) / `:230-238`(DELETE) 已按 UTF-8
   码点回退/跳续字节。**但 LEFT/RIGHT 漏了同一套守卫**。
3. **同构排查揪出真缺陷并修复**：`markerpos` 是 UTF-8 **字节索引**，裸
   `markerpos--/++` 让光标落进多字节汉字的续字节，`add_string:294` 的
   `text.insert(markerpos, 1, c)` 当场把字符劈成非法序列。本轮补 LEFT/RIGHT
   码点边界移动（仅 `IO/Components/Textfield.cpp`）。这是 round34
   `UIWorldSelect` / round35 `UICharSelect` 之后第三处「入口守卫不对称」病。
4. 自测 `tools/test_textfield_utf8_caret.py` = 6 守卫静态核验 + 10 边界
   case 修复前/修复后对照 PASS；构建 `GATE OK` + `EXITCODE=0`；原子提交。

## 1. 1-3 死注册 ↔ live 源码逐字（2026-11-09 23:0x 现场）

| 旧账口径（`feature-gap` §9.5.3 / 计划 §4 1-3） | live 源码逐字 | 判定 |
| --- | --- | --- |
| `GENDER_DONE(4)`/`CONFIRM_EULA_RESULT(5)` s9 不下发，「事实死注册」需标注/收敛 | `PacketSwitch.cpp:58-60` 注释「在 s9 下不发送（性别/EULA 已由 LOGIN_RESULT(auth success) 闭环，见 LoginResultHandler），仅作协议占位」；`:434-437` 再写「注册仅作协议完整性」并 `emplace<GENDER_DONE, GenderDoneHandler>()` / `emplace<CONFIRM_EULA_RESULT, ConfirmEulaResultHandler>()`；`:61-62` 枚举仍在（协议命名参考） | ✅ 有意裁决销案，零改动 |
| handler 是空实现 | `LoginHandlers.cpp:454-471 GenderDoneHandler::handle` 读 gender + drain + `remove(GENDER)` + `remove(LOGINWAIT)` + `ServerRequestPacket().dispatch()`，注释「修复（2026-11-03）：……旧代码只发 ServerRequestPacket，从不拆 UIGender 欢迎框与 UILoginWait，导致性别欢迎框跨到频道/创建页残留叠加（用户实机问题 4 的第一条泄漏路径）」；`:473-486 ConfirmEulaResultHandler::handle` `ok==1` 时 `remove(TOS)` + `ServerRequestPacket().dispatch()` | ✅ 非空且带实机修复，销案 |
| — | `LoginResultHandler::handle():50-57` 注释「这三项清理原只在 loginwait 活跃分支内执行；Wait 被门闩看护提前回收……GENDER/TOS/LOGINNOTICE 残留 active……LOGIN_RESULT 到达即代表上一轮登录/性别/EULA 对话终结——无论 Wait 是否还在，都必须拆干净」+ 无条件 `remove(LOGINNOTICE/TOS/GENDER)` | ✅ 主链已闭环 |

证据优先级 §2.3：协议矩阵 > 源码 .dispatch()/emplace > 现场重算 > 真机 > 旧账。
删掉这些注册/枚举会破坏 v83 参考端兼容（未来 s9 启用时生效）与 2026-11-03
实机修复的残留窗清理链，**不动**。

## 2. 1-5 IME 主体已实现 + 真缺陷立项

### 2.1 主体链路逐字（不重复造轮子）

- `IO/Components/Textfield.h:55-58`：`void set_accept_unicode(bool accept)` /
  `bool accepts_unicode() const`，注释「允许接收 UTF-8（IME/剪贴板中文）。默认
  false（账号/密码等仍限 ASCII）」「UI::focus_textfield 据此决定开/关系统 IME」；
  `:84 bool accept_unicode = false;`。
- `IO/Components/Textfield.cpp:274-300 add_string()`：`accept_unicode` 时只滤
  `< 32`，非 unicode 字段滤 `<32 || >=128`；`:303-336 modifytext()` 同口径，
  `:324-329 crypt>0` 时按字节数铺密码掩码。
- `IO/UI.cpp:496-505 send_text()`：`focusedtextfield->add_string(text)`，注释
  「Composed text (IME / clipboard) may be UTF-8 (CJK)」；`:507-527
  focus_textfield()`：ASCII-only 字段 `end_ascii_text_input()` 走 KEY_DOWN
  ASCII 通路（注释解释 SDL3 部分宿主不发 `SDL_EVENT_TEXT_INPUT` 会让 ASCII
  输入框永远收不到字符），unicode 字段 `begin_ascii_text_input()` 由
  `send_text()` 送 UTF-8；`:529-534 remove_textfield()` 先清指针再关输入。
- `IO/UITypes/UIMapSearchPanel.cpp:188` 注释「搜索框接收中文：启用系统 IME，
  UTF-8 直接进字图缓存（思源宋体含 CJK）」。
- `Graphics/GraphicsGL.cpp:1349-1358` 注释「advances 按**字节**索引（Textfield
  的光标用 advance(byte) 定位），所以一个多字节字符要占 consumed 个槽位，且
  它们都记同一个笔位起点」+ `for(k<consumed) advances.push_back(ax)`；
  `Graphics/Text.cpp:206-209 Layout::advance(size_t index)` 返回
  `advances[index]`。`Textfield.cpp:78` `textlabel.advance(markerpos)` 用它定
  光标 x → **markerpos 是字节索引，不是码点索引**。

### 2.2 真缺陷：LEFT/RIGHT 与 BACK/DELETE 守卫不对称

`Textfield.cpp:174-187`（修复前）：

```cpp
case KeyAction::Id::LEFT:  { if (markerpos > 0) markerpos--; break; }
case KeyAction::Id::RIGHT: { if (markerpos < text.size()) markerpos++; break; }
```

但 `:192-202`(BACK) 已 `while (start > 0 && (text[start]&0xC0)==0x80) start--;`
回到码点首字节；`:230-238`(DELETE) 已 `while (end < text.size() && ...)
end++;` 跳过续字节。**LEFT/RIGHT 没跟上**：

- 光标落在 3 字节汉字（如 `你 = E4 BD A0`）的第 2/3 个续字节上；
- 此时 `add_string:294 text.insert(markerpos, 1, c)` 在续字节中间插一个
  ASCII → 该汉字被劈成 `E4 X BD A0` 非法 UTF-8，字图缓存画出乱码/白线；
- `BACK:194-199` 从这样的 `markerpos` 往回找首字节也会删错范围。

`GraphicsGL.cpp:1349` 的槽位设计本意是「指针落在半个字上也不跳位」，但它只
保证**绘制**不跳；`markerpos` 作为**插入点**仍必须停在码点边界。

### 2.3 最小修复（LEFT/RIGHT 补码点边界，仅 1 文件）

```cpp
case KeyAction::Id::LEFT:
{
    if (markerpos > 0)
    {
        // 与 BACK/DELETE 的码点对齐同口径：markerpos 是 UTF-8 字节索引……
        size_t start = markerpos - 1;
        while (start > 0 && (static_cast<unsigned char>(text[start]) & 0xC0) == 0x80)
            start--;
        markerpos = start;
    }
    break;
}
case KeyAction::Id::RIGHT:
{
    if (markerpos < text.size())
    {
        // 右移须跳过当前码点的全部续字节，落到下一个码点首字节。
        size_t end = markerpos + 1;
        while (end < text.size() && (static_cast<unsigned char>(text[end]) & 0xC0) == 0x80)
            end++;
        markerpos = end;
    }
    break;
}
```

`git diff --stat` 见 §7 提交记录；不改协议、不改包体字节、不碰 WZ/IMG/XML、
不动 `add_string`/`modifytext`/`GraphicsGL`（绘制侧本就正确）。ASCII 文本
全是 1 字节码点，`start` 循环 0 次、`end` 循环 0 次 → 与旧 `±1` **逐字节等价**。

## 3. 自测证据

`tools/test_textfield_utf8_caret.py`（本轮新建）：
- §1 静态核验 6 守卫逐字在盘（LEFT 回退起笔 / LEFT 跳续字节 / RIGHT 跳续
  字节 / RIGHT 落到下一首字节 / 既有 BACK 守卫 / 既有 DELETE 守卫）→ 6×`[OK]`。
- §2 纯 Python 复刻 `move(bytes, markerpos, LEFT|RIGHT, guarded)` 与
  `insert_split_demo()`（`text.insert(markerpos,1,'X')` 后 `utf8_valid` 判
  合法），10 case 分 REG（纯 ASCII 3）/ CJK（2 汉字 6B 2）/ MIX（`a你b`
  5B 2）/ EMO（`中😀文` 3+4+3=10B 2），每组都跑修复前 `guarded=False`
  与修复后 `guarded=True`。
- §3 三条独立闭环，不合并计数：
  - 守卫 LEFT/RIGHT：修复后落在续字节的光标 `0`（期望 0）。
  - 修复前证据：落在续字节 `28` 次；CJK/MIX/EMO 首步 `insert()` 劈字
    `5` 例（期望 >0）。
  - 回归：REG（ASCII）`guarded == unguarded` 逐字节一致 `True`。

实跑输出（`PYTHONIOENCODING=utf-8`）：

```
[OK] C03 CJK LEFT 6  before(land=[5,4,3,2],bad=4,split=True) -> after(land=[3,0,0,0],bad=0,split=False)
[OK] C04 CJK RIGHT 6 before(land=[1,2,3,4],bad=4,split=True) -> after(land=[3,6,6,6],bad=0,split=False)
[OK] C08 EMO LEFT 10 before(land=[9,8,7,6],bad=7,split=True) -> after(land=[7,3,0,0],bad=0,split=False)
[guard LEFT/RIGHT] after-fix carets on continuation bytes: 0 (expected 0)
[before-fix evidence] before-fix carets on continuation bytes: 28
[before-fix evidence] CJK/MIX/EMO first-move insert() splits: 5 (expected >0)
[regression] ASCII guarded == unguarded: True
PASS: 6 static guards + 10 boundary cases (before bad=28/split=5 -> after bad=0; ASCII regression OK)
TEST_RC=0
```

### 自我批判（口径毛边，不硬撑）

首版 harness §3 有一行 `print("... splits a codepoint in %d cases" % 0)`
把数写死成 0，紧接下一行按 case 重算输出 `5` —— 同一份输出自相矛盾，
是「标尺不严」的假绿毛边。按事实删掉占位行、只留重算值，重跑仍 `PASS`
且 `TEST_RC=0`。不因为「大结论没变」就留着矛盾输出。

## 4. 构建门禁

后台 job `bash-f46c87d586e64418` exit 0，真实执行（MSYS2_ARG_CONV_EXCL 规避
MSYS 假执行）：

```
BEFORE_MTIME 2026-11-09 22:58:42   BEFORE_SIZE 5644
=== BUILD ===
GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)
GATE OK
EXITCODE=0
=== AFTER ===
AFTER_MTIME 2026-11-09 23:14:41    AFTER_SIZE 2867
ERROR_LINES 0
TAIL ... 已完成代码的生成 / MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe
     已复制 1 个文件 ×3 / 复制了 0 个文件
=== GATE_LINES ===
GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e) / GATE OK / EXITCODE=0
```

`build_log.txt` mtime `22:58:42 → 23:14:41`（真重编译，size 5644→2867），
零 `error|fatal|失败` 行；tail 仅既有 `LNK4099 vc120.pdb` warning。
占用按用户策略 `taskkill /F /IM MapleStory.exe /T` 先行：3 个 32K 空壳
（34476 / 6108 / 56016）`Access is denied`（不占资源、不阻断构建）。

## 5. 不在本轮范围（如实披露）

- IME **composition 预编辑串**（候选窗/下划线组合态）本端未做：当前是
  `send_text()` 直接收「已提交」UTF-8，不做 `WM_IME_COMPOSITION` 级预编辑
  渲染。这是产品层功能，需真机 + 输入法矩阵取证后另立 Plan，本轮只收
  「已提交串的光标/插入点正确性」。
- 1-3 的 v83 参考端兼容链已保留，未做「收敛死注册」（见 §1，强改破坏实机
  残留窗修复）。Wave1 1-1~1-5 至此全部逐字核准完毕。
- Wave2（IMG 内容层）/ Wave3（L-B 涉包体结构、需用户裁决 D1–D14）挂起。

## 6. 真机联测 backlog（交用户一起测，写入 `pending-live-tests` §15）

| # | 操作 | 期望 | 为何自动测不了 |
| --- | --- | --- | --- |
| 15-1 CJK 左右键 | 地图搜索框输入「你好世界」，HOME 后连按 RIGHT 4 次再插「X」 | 光标每次跳过整个汉字，插入得「X你好世界」/「你X好世界」等，无乱码无半字 | harness 只复刻字节算法，真机需 IME + 字图缓存渲染 |
| 15-2 BACK/DELETE 组合 | 上一步后连按 LEFT + BACK / RIGHT + DELETE | 整字删除，不残留半字节、不误删相邻字 | 需真机输入法提交 + 回删链路 |
| 15-3 ASCII 回归 | 账号/密码/聊天框纯英文，左右键 + 插入 + 回删 | 与修复前逐字节一致，无回归 | 需登录服 + 冷启动手工按键（harness 已证算法等价） |
| 15-4 IME 预编辑 | 中文输入法候选态（未提交）看输入框 | 当前只收已提交串；预编辑态观感须另行取证（§5） | 需多输入法矩阵 + 真机截图 |

## 7. 提交 scope（精确 add，零署名）

- M `IO/Components/Textfield.cpp`（LEFT/RIGHT 码点边界守卫）
- A `tools/test_textfield_utf8_caret.py`（自测 harness）
- A `docs/features/development-and-fix-plan-2026-11/notes/wave1-login-ime-recheck-20261109.md`（本记录）
- M `docs/features/development-and-fix-plan-2026-11/README.md`（§4 Wave1 1-3/1-5 销案 + 新增 1-7）
- M `docs/features/pending-live-tests/README.md`（新增 §15）

`.gitignore` 混合编码沿用 round34/35 结论：不整文件重写，改精确 `git add`。
提交模板按 `git-commit-standard` skill：`<Emoji> <Type>(<Scope>): <标题>
(#<IssueID>)` + 结构化变更列表 + 零署名。

## 8. 性质标注

本记录是「开发/修复阶段现场复核 + 最小修复 + 自测 + 提交」证据，非校准文档。
校准十一面（login→quest）仍只读闭环；本轮按用户 m04292 授权进入当前客户端
源码修复。全程不建 095 引用、不改协议、不碰 WZ/IMG/XML、不建提交前不验。
