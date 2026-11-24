# W1.5/W2 四项预留收口终验（2026-11-20）

## 自动守卫

| 项 | 结果 |
|----|------|
| `python tools/test_r3_userlist_083.py` | **PASS** 46/46（I3/I7/I8 更新） |
| `python tools/gen_opcode_matrix.py` + `--check` | **GATE OK** |
| `tools/build_smoke.bat` | **EXITCODE=0**（GATE OK + MSBuild Release\|x64） |
| 产物 | `E:\MXD\BeiDou-Client-xy\MapleStory.exe` mtime 16:21 |

## 四项收口

| # | 项 | 结果 |
|---|----|------|
| 1 | 联盟 Kick/职级/ChangeMaster | 有 Angel+s9 字段 → 已接线（见 `alliance-kick-grade-changemaster-2026-11-20.md`） |
| 2 | Invite/Notice/箴言 Textfield | `UIEnterString` 物理键对话框；禁 SoftKey |
| 3 | 真机对拍正确端截图 | **阻塞**（见 `live-oracle-compare-block-2026-11-20.md`） |
| 4 | opcode matrix GATE | 重生成后 GATE OK |

## SoftKey

全路径无 `emplace<UISoftKey>`；输名走 `UIEnterString` / ChatBar。
