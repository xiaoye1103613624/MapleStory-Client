# r516 — CashGacha UI 接线 + takeFrom 吃满包

- `UICashGachapon::set_result/set_failed`；成功/失败结果刷窗内 status
- `UIRemoteGachapon::set_result`；`GachaponBox` 写入 mode 并 drain 余字节
- CashShop `0x68` takeFrom：`skip_cash_item_info` 吃满 itemInfo
- 未识别 CS mode：drain 余字节

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
