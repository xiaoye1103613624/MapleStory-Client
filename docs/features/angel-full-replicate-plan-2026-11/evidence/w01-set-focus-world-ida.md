# W01 evidence · SetFocusWorld（摘要，不重复全量 IDA dump）

- MCP `server_health`：`idb_path=E:\MXD\反编译IDA\v83\`Angel.i64`，module=``Angel.exe``
- `CLogin::OnLatestConnectedWorld @0x5f82f4`：step==1 时 Decode4；≠253 写入；若 id∉[254,255] 且存在 CUIWorldSelect → SetFocusWorld
- `CUIWorldSelect::SetFocusWorld @0x6272d0`：按世界 id 扫列表得槽位 → SetKeyFocus
- `SetKeyFocus @0x6253bc`：清全部按钮 focus，仅目标槽 SetKeyFocus(1)

客户端落地：`UIWorldSelect::set_focus_world` + `LastConnectedWorldHandler` 调用。
