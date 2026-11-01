# r209 — stub/button → OutPacket 批量

- WorldSelect `BtViewAll` → `ViewAllCharPacket`（`BtViewChoice` 登录态仍提示）
- Alliance `BtInvite`/`BtNotice` → SoftKey → `AllianceInvitePacket` / `AllianceNoticePacket`
- Wedding `BtOK` → SoftKey guest+msg → `WhisperPacket`（对齐 SoftKey I/W）
- CashShop `BtCoupon` → `UICoupon`；`BtHelp` → `UIHelp`
- ContextMenu 增 Messenger 行 → `MessengerInvitePacket`
- PartySearch SoftKey `J` → `jobs_mask`（`strtol` 支持 0x）
- Region NA/EU → `UIWorldSelect::set_region`

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
