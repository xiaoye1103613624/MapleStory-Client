# 聊天面 Angel/IMG 校准（A + ②）· 2026-11-12

## 目标

按 Angel 083 `CUIStatusBar::SetChatType` 重写信息栏聊天三态，并对 `UIChat` / `UIMessenger` 聊天行做 IMG+IDA 校准。

## 决策

- **A**：对齐 Angel 三态（删紧凑 120 / 去 BtMin·BtMax 逐级）。
- **②**：信息栏 + UIChat + Messenger 聊天行。

## 实施摘要

| 面 | 结论 |
| --- | --- |
| 状态机 | `set_chat_type(1/2/3)`：1 收起、2 输入瞬态不持久、3 完整可拖；BtMax→3、BtMin→1；`StartChat`/`EndChat`/`Stage::load` 切图清 type2 |
| 拖高 | clamp 26..507 后 `h%13==0 → +2`；仅 type3 |
| IMG | `StatusBar.img` 无 `chat/ingame`；`base/chat`/`chatTarget`/`BtWhisper` + `Basic BtMax/BtMin` 正确 |
| UIChat | 本包无 `socialChatEnter`（Angel 亦无此窗）→ inactive 预留，优先 Messenger |
| Messenger | 输入槽对齐 SetCtrl `@0x8505e2`：`(9,318,207×15)` |

## 证据

- `evidence/img-query-summary.json`
- `evidence/angel-Messenger-SetCtrl.json` / `angel-AddChatText.json`
- Angel live MCP `127.0.0.1:13339`（`` `Angel.i64 ``）
- 既有 `tools/_ida_chattype/*.dec.txt`

## 验证

```text
python tools/test_chat_angel_setchattype.py
python tools/test_chattarget_keybind.py
tools\build_smoke.bat
```

## 销案

- statusbar-hud **D7-A**：已按 Angel SetChatType 改回（本功能）。
- 旧 round69 harness（守卫「仍保留 COMPACT」）作废，改用本目录 harness。
