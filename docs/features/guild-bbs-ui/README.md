# Guild BBS 完整 UI（GuildBBS.img）

## 状态（2026-11-05）

| 项 | 状态 |
| --- | --- |
| 原生窗 `UIGuildBBS` | ✅ 列表 / 读帖 / 发帖三页 |
| s2c 解析对齐 s9 | ✅ mode 0x06 / 0x07 |
| SoftKey `B` 入口 | ✅ 公会窗 `B` → 开窗 + List |
| SoftKey 摘要栏 | ✅ 仍同步 `UIGuild::set_bbs_summary` |
| 真机点测 | ⏳ pending-live-tests |

## 证据

| 源 | 结论 |
| --- | --- |
| REA `wz_dump/GuildBBS.img_runtime.json` | 控件：`backgrnd` 734×526、`backgrnd2/3` 327×288；BtWrite/Retouch/Delete/Register/Cancel/Notice/Quit/Reply/ReplyDelete；Emoticon；MoveEmoticon |
| orange-wz load GuildBBS.img | 根可加载；内部节点仍空（私服 img 已知缺陷）→ 以 runtime dump 为准 |
| IDA BeiDou.exe.i64 | 无 `CUIGuildBBS` 符号（私服剥符） |
| port-095 v95 | `CUIGuildBBS::SendLoadList/View/OnRegister/OnComment/OnDelete` → c2s 179；本端 s9=`BBS_OPERATION` 155 |
| s9 `GuildPackets` / `BBSOperationHandler` | 列表 0x06 / 读帖 0x07 字段序；出站 mode 0..5 |

## 实现要点

- 新 UI 型 `UIElement::Type::GUILDBBS` + `PosGUILDBBS`
- 出站补 `BbsEditPacket` / `BbsDeleteReplyPacket`
- 按钮落位无 WZ layout 节点：按底板量测常量（见 `notes/layout.md`），真机可微调

## 验证

- `tools/build_smoke.bat` → GATE OK / EXITCODE=0
- 真机步骤：`docs/features/pending-live-tests/README.md` § GuildBBS
