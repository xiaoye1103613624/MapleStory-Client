# Wave2 · 聊天目标快捷键接线复核（round53 · statusbar-hud/D4 + social-economy/D1）

## 0. 性质与授权

- 轮次：goal round 53（`goal-11388c42` rev24 / active）。
- 授权：m04292 一次性 Plan（`docs/features/development-and-fix-plan-2026-11/README.md`）已授权改源码 / 自测 / 自测通过即提交 / 遇客户端占用直接关 / 不能自测者记 backlog。本轮不重开 Plan。
- 性质：**K1 零风险实施**（不新增协议、不改 opcode、不改包体、不改 WZ/IMG/XML），来自跨面决策矩阵 §5 的 K1 分类，同时销 `statusbar-hud/D4` 与 `social-economy/D1` 两条决策。
- 只读口径：十二面 Angel 083 校准文档（`docs/features/*-angel-calibration-2026-11/`）全程只读，本轮未改一字。

## 1. 缺陷与决策点

### 1.1 矩阵侧原文

- `statusbar-hud/D4`（`docs/features/statusbar-hud-angel-calibration-2026-11/README.md` §6）：`change_target(int32_t action) {}` 空 stub 是否接线 → 接线到 chatTarget 下拉选择（零风险，六个频道快捷键复活）。
- `social-economy/D1`：同一 `change_target()` 空 stub + v95 注释污染，与 statusbar-hud/D4 是同一处代码，合并处理。

### 1.2 修前逐字事实链

| 事实 | 位置 | 证据 |
| --- | --- | --- |
| 空 stub（函数体为空、带 TODO 占位注释） | `IO/UITypes/UIChatBar.h:122-123` | `// TODO: Change these?` + `void change_target(int32_t action) {}` |
| 唯一调用点：六个聊天键全部落到这个空函数 | `IO/UIStateGame.cpp:471-482` | `case KeyAction::Id::SAY / PARTYCHAT / FRIENDSCHAT / GUILDCHAT / ALLIANCECHAT / TOSPOUSE:` → `chatbar->change_target(action); break;` |
| 既有注释自承未实现 | `IO/UITypes/UIUserList.cpp:916` | 「v83 家族聊天由聊天框目标/斜杠别名路由；change_target 仍是预留空实现」 |
| 目标枚举与斜杠别名同序 | `IO/UITypes/UIChatBar.h:365-376` | `enum ChatTargetIndex : uint8_t { TARGET_ALL, TARGET_WHISPER, TARGET_PARTY, TARGET_GUILD, TARGET_BUDDY, TARGET_ALLIANCE, TARGET_SPOUSE, TARGET_MEGAPHONE, TARGET_COUNT }`，注释「顺序与斜杠命令别名一致：/w /p /g /a /b /s」 |
| 已有设置目标的 API | `IO/UITypes/UIChatBar.cpp:1633-1640` | `void UIChatBar::set_classic_target(uint8_t target_id) { if (target_id >= TARGET_COUNT) return; classic_chat_target_id = target_id; classic_target_text.change_text(chat_target_name(target_id)); }` |
| 目标名文案表 | `IO/UITypes/UIChatBar.cpp:1613-1631` | `chat_target_name()` 八键 `chat.target.all / whisper / party / guild / buddy / alliance / spouse / megaphone` |
| 已有开输入行的范式 | `IO/UIStateGame.cpp:497-512` | `case KeyAction::Id::TOGGLECHAT:` → `else if (chatbar->is_classic()) chatbar->toggle_input(true);` |
| 发送链路已按目标分派 | `IO/UITypes/UIChatBar.cpp:2313+` | 非斜杠 + `classic_mode && classic_chat_target_id != TARGET_ALL` → `send_to_target_channel(message)`（`CHANNEL_SENT` / `CHANNEL_HELD` / `CHANNEL_FALLBACK`）；否则 `GeneralChatPacket(message, true)` |

**结论**：修前六键（`SAY` / `PARTYCHAT` / `FRIENDSCHAT` / `GUILDCHAT` / `ALLIANCECHAT` / `TOSPOUSE`）按下去只进空函数，**目标不切换、输入行不打开**，六个频道快捷键形同不存在；而发送侧（`send_to_target_channel`）与目标设置侧（`set_classic_target`）都已就绪 —— 缺的只有中间那一段映射。

## 2. 最小修复

### 2.1 改动

- `IO/UITypes/UIChatBar.h`：删占位注释与空函数体，改为声明 + 12 行依据注释（13 / -1）。
- `IO/UITypes/UIChatBar.cpp`：新增实现（46 行含注释）。

```cpp
void UIChatBar::change_target(int32_t action)
{
    uint8_t target = TARGET_ALL;
    switch (action)
    {
        case KeyAction::Id::SAY:          target = TARGET_ALL;      break;
        case KeyAction::Id::PARTYCHAT:    target = TARGET_PARTY;    break;
        case KeyAction::Id::FRIENDSCHAT:  target = TARGET_BUDDY;    break;
        case KeyAction::Id::GUILDCHAT:    target = TARGET_GUILD;    break;
        case KeyAction::Id::ALLIANCECHAT: target = TARGET_ALLIANCE; break;
        case KeyAction::Id::TOSPOUSE:     target = TARGET_SPOUSE;   break;
        default: return;
    }
    set_classic_target(target);
    if (!view_input) toggle_input(true);
    LOG(LOG_DEBUG, "UIChatBar: change_target target=" << static_cast<int>(target));
}
```

diff：`2 files changed, 56 insertions(+), 2 deletions(-)`。

### 2.2 为什么这样改

- **不新造 API**：`set_classic_target` / `toggle_input` / `send_to_target_channel` 全已在盘，本轮只做「键 → 目标号」映射，一处函数销两条决策点。
- **默认收框不加分支**：`if (!view_input) toggle_input(true)` 与 `IO/UIStateGame.cpp:506` 的 classic 开框范式逐字同构，避免二次 `toggle_input` 把已打开的输入行关掉（harness 有专门场景覆盖）。
- **`default: return` 而不是 `target = TARGET_ALL`**：未命中键保持原状，不让无关键（`MENU` / `QUICKSLOTS` / `TOGGLECHAT`）顺带改目标。
- **`SAY → TARGET_ALL`**：与 `classic_chat_target_id != TARGET_ALL` 才走频道分派的分支配套 —— 单聊回到公共频道是既有默认语义。

### 2.3 083 依据（注释引用，不改行为）

Angel 083 `CWvsContext::UseFuncKeyMapped` @ `0xa0773d`：normal 键 case 4 → `sub_8D4D43(0)` → `CUIStatusBar::StartChat(0)`，case 17 → `2`、case 19 → `1` 同族；功能键 `v4-10` 链只产 `0/1/2/3/4/5/7`。`ALLIANCE` / `SPOUSE` 在 083 走另一条 `SetWhisperTarget` 分支，与 s9 服务的 `/a` `/s` 语义对齐，故映射为 `TARGET_ALLIANCE` / `TARGET_SPOUSE`。**权威依据是 s9（联机优先），083 仅作语义旁证** —— 数值重合 ≠ 语义重合。

## 3. 自测与门禁

| 门禁 | 命令 | 结果 |
| --- | --- | --- |
| 键位 harness | `python tools/test_chattarget_keybind.py` | `PASS: chat target keybind round53 (statusbar-hud/D4 + social-economy/D1)` / `TEST_RC=0` |
| 协议矩阵 | `python tools/gen_opcode_matrix.py --check` | `GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)` |
| 空白/冲突 | `git diff --check HEAD` | `DIFFCHECK_CLEAN` |
| 构建 | `MSYS2_ARG_CONV_EXCL='*' cmd.exe /d /c "cd /d E:\\project\\MapleStory-Client && tools\build_smoke.bat"` | `GATE OK` ×2 + `EXITCODE=0`（job `bash-61b52d7114e04250`） |

harness 覆盖：头文件空 stub 消失 / 声明存在 / `change_target` 旁无 TODO 占位；六键映射逐条正则；`default: return;`；`set_classic_target(target);`；`if (!view_input)` + `toggle_input(true);`；083 依据 `0xa0773d` 双处（h + cpp）；`IO/UIStateGame.cpp` 六 case 与 `chatbar->change_target(action);` 原样未动；行为复刻 `KEYMAP` 六键 + 四个未命中键（`MENU` / `QUICKSLOTS` / `TOGGLECHAT` / `SAY2`）状态不变，含「已开输入行时不收框」与「目标号 ∈ [0, 8)」断言。

### 3.1 harness 自身两处误判（就地修，不计任务失败）

1. `h_no_todo_placeholder` 误命中 `IO/UITypes/UIChatBar.h:31` 文件头 include 区的 `// TODO: Change these?`（与本修无关的无关注释）→ 改正则为「`TODO: Change these?` 紧跟 `void change_target` 才判失败」。
2. 裁剪实现区时 `body.find('\n\t}\n')` 从截取起点开始找，撞到**上一个函数**的右花括号，把函数体截空 → 改为 `body.find('\n\t}\n', sig - start)`，只在签名之后再收尾。

## 4. 范围边界

- 不新增协议、不改 opcode、不改包体（`MultiChatPacket` / `SpouseChatPacket` / `GeneralChatPacket` 全部复用）。
- 不解析 WZ/IMG/XML（纯 C++ 键位映射，不触 orange-wz 门禁）。
- 不改 `input_text_enter_callback` 的斜杠别名与频道分派逻辑（`/w` `/find` `/p` `/g` `/a` `/b` /`/s` 行为零变化）。
- `TARGET_WHISPER` / `TARGET_MEGAPHONE` 不在六键映射内：密语走 `focus_whisper()` → `UIWhisperTargetDlg`，喇叭走独立入口，本轮不动。

## 5. 真机 backlog（不能自测）

| # | 项 | 观测信号 |
| --- | --- | --- |
| 53-1 | 六键在 classic HUD 下是否真的切目标并开输入行 | 按 `SAY`/`PARTYCHAT`/`FRIENDSCHAT`/`GUILDCHAT`/`ALLIANCECHAT`/`TOSPOUSE`，聊天框目标名应逐一切换 |
| 53-2 | modern HUD 下的观感 | `classic_mode == false` 时 `toggle_input(true)` 走 `toggle_view(true, true)`，需确认输入行与 5 个 Bt 按钮 active 翻转正常 |
| 53-3 | 输入行已打开时再按频道键 | 不应收框，只切目标（harness 已断言状态，真机确认观感） |
| 53-4 | 切到 `PARTY`/`GUILD`/`ALLIANCE` 但无对应频道时 | 走 `send_to_target_channel` 的 `CHANNEL_HELD` / `CHANNEL_FALLBACK`，确认提示文案与不误发 |

## 6. 提交 scope

精确 `git add`：`IO/UITypes/UIChatBar.cpp` / `IO/UITypes/UIChatBar.h` / `tools/test_chattarget_keybind.py` / 本 notes / `docs/features/development-and-fix-plan-2026-11/README.md`（§4 追加 2-18）/ `docs/features/pending-live-tests/README.md`（追加 round53 节）。零署名（`git-commit-standard`：`<Emoji> <Type>(<Scope>): <标题> (#<IssueID>)`，不带 Powered by / Co-authored-by / Generated by）。校准文档与 evidence 不随源码提交。

## 7. 证据优先级

`opcode-matrix` > live 源码构造点/调用点 > s9 服务端实现 > Angel 083 反编译 > 旧台账。本轮无协议面，故 s9/Angel 只用于「语义旁证」，不构成行为裁决。
