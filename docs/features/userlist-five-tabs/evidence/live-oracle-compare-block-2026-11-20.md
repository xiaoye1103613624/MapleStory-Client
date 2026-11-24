# 真机对拍阻塞（2026-11-20 收口波）

## 尝试

- 部署产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`（本波 `build_smoke` EXITCODE=0，mtime 16:21）。
- 进程：本机已有多个 `MapleStory.exe`，`MainWindowHandle=0` / 无标题（无可见游戏窗）。
- `--resource-probe` 短启：可起进程但无 HWND（探针非 UI）。
- Computer Use 抓屏：桌面为 IDE/浏览器，未见可对拍的五页签游戏窗；**勿假装已对拍**。

## Oracle 参照（已落盘，仅作终验基准）

`evidence/frames/native_{friend,party,guild,union,blacklist}_tab.png`

历史 live：`evidence/live-r3-*.png`（r3 波；本波未重截）。

## 手工验收清单（有可见游戏窗后执行）

1. 五页签空态切页 vs `native_*_tab.png`（底栏无叠绘；Δ≤2px）。
2. 联盟入盟：公告+成员行；选中后 Kick/Up/Down/Change 亮并可发包。
3. 联盟未入盟：仅空提示 + 笔形常亮。
4. 勋章 / 学院空态；输名走 `UIEnterString`（无 SoftKeyboard.img）。
5. SoftKey 口径全程无软键盘弹窗。
