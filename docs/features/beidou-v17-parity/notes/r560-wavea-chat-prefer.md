# r560 — WaveA tip→chat 批量优先

- `WaveAShellHandlers`：剩余 `if (!shown) float_tip` / 裸 `else float_tip` → chat 优先，无 ChatBar 再 float
- 修正字符串拼接 `.c_str()` 优先级；Pendant / ExtraEquip 外层 else 一并 chat

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
