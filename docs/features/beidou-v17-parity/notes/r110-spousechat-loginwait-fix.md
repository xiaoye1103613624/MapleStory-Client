# r110 — SpouseChat SoftKey + LoginWait 编译修复

- ChatBar `BtChat` SoftKey → `SpouseChatPacket`；`/s` `/spouse` `/mate`
- `UILoginWait`：`static constexpr Point` → `static const`（MSVC C2737）；去掉 `BarFrame::nl::node src`

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
