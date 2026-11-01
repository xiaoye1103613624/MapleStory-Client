# r515 — Party refresh-if-open / 雇佣商人标签 / 障碍列表落地

- Party `0x07`：仅已开 `UIParty` 才 `update_party`；`0x0F` JOIN 仍开窗
- HiredMerchant update：无坐标时只 `set_label`，不在玩家脚下伪造 spawn（`MapFieldMarkers::set_label/contains`）
- FieldObstacleOnOffList：逐条 apply 与 OnOff 同逻辑；OnOff 去 tip

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
