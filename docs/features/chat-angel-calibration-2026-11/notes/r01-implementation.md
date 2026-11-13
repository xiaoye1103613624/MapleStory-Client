# r01 · 实施笔记（2026-11-12）

## 改动文件

- `IO/UITypes/UIChatBar.h` / `.cpp` — `set_chat_type` / `end_chat_if_typing` / 删 COMPACT / 拖高 mod13
- `Gameplay/Stage.cpp` — `load` 调 `end_chat_if_typing`
- `Configuration.h` — `ChatType` 注释
- `IO/UITypes/UIMessenger.h` — 输入槽 9/318/207/15
- `IO/UITypes/UIChat.h` / `.cpp` — socialChatEnter 缺资源预留说明
- `tools/test_chat_angel_setchattype.py`
- `C:\Users\11036\.cursor\mcp.json` — ida-pro-mcp → `13339`（Angel）

## 旧存档

`chat.type=2` 启动映射为 `1`，避免幽灵紧凑态。

## 构建

- `python tools/test_chat_angel_setchattype.py` → PASS
- `python tools/test_chattarget_keybind.py` → PASS
- Live `OutDir=E:\MXD\BeiDou-Client-xy\` 被占用（MapleStory.exe Access Denied），改出到 `build-rebuild\MapleStory.exe` **EXIT=0**。
- 需用户关端后覆盖 live 或自行拷贝 `build-rebuild\MapleStory.exe`。
