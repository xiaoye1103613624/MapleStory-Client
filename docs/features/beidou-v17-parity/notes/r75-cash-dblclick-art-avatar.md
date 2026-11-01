# r75 — 现金双击校正 + Art/Avatar Mega 出站

## 双击映射
| 前缀 | 行为 |
| --- | --- |
| 545* | EntrustedShop + `HiredMerchantOpenPacket`（原误开 Incubator） |
| 506* | Incubator（5060002 等） |
| 5073/5077 | ArtMegaphone |
| 539* | AvatarMegaphone 壳 + SoftKey `UseAvatarMegaphonePacket` |

## 出站加深
- ArtMegaphone BtPreView SoftKey → `UseMegaphonePacket`
- AvatarMegaphone BtOk SoftKey → `UseAvatarMegaphonePacket`
- PersonalShop SoftKey 空 → `HiredMerchantOpenPacket`

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
