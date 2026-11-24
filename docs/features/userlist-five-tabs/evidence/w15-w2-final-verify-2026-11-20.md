# W1.5 / W2 终验（2026-11-20）

## 自动守卫

| 项 | 结果 |
|----|------|
| `python tools/test_r3_userlist_083.py` | **PASS** 44/44（含 I1–I6 W1.5/W2） |
| `tools/build_smoke.bat` | 当时 **GATE FAIL EXITCODE=2**（opcode matrix 漂移）；**已在收口波** `gen_opcode_matrix.py` 修到 GATE OK（见 `w15-w2-reserve-clear-verify-2026-11-20.md`） |
| MSBuild Release\|x64 直编 | **OK** → `E:\MXD\BeiDou-Client-xy\MapleStory.exe` |

## 截图对拍

**阻塞**：本波未能启动客户端抓屏对拍（未确认 live 客户端进程可启 / Cursor assets 正确端截图路径未在本会话复用）。

勿假装已对拍。手工验收清单：

1. **玩家列表五页**：好友 / 组员 / 家族 / 家族联盟 / 拒绝列表切页；底栏无叠绘；空态按钮 DISABLED 贴图可见。
2. **联盟入盟**：有联盟号时 UNION 页显示公告 + 成员行列；选中行后 Whisper/Where/Chat/Info/PartyInvite 亮；Kick/Up/Down/Change 灰。
3. **联盟未入盟**：仅「还未加入联盟。」+ 笔形钮常亮。
4. **勋章**：分号键开窗；Title 真材框；5 页签；行再点开 Detail；无 114 图标时文本态。
5. **学院**：F/菜单开窗；BtLeft/BtRight 翻特权；BtSpecial 发包；输名走 ChatBar 提示（无 SoftKeyboard 弹窗）。
6. SoftKey 口径：全程无 SoftKeyboard.img 弹窗。

正确端截图（用户曾提供于 Cursor assets）仅作终验参照，布局以 Angel+IMG 为准。
