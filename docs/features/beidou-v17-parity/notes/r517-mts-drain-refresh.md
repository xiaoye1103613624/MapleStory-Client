# r517 — MTS 余字节 drain + 状态不叠 tip

- `MtsOperation`：列表循环后 / 各 mode 后 drain；已开 UIMTS 只 `set_status`，未开才 SM（list 除外）
- `MtsOperation2`：已开窗刷 cash，不叠 float_tip
- `NpcAction`：去 tip

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
