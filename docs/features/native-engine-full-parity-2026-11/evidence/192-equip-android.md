# 192 Equip/Android 取证（2026-11-22）

## 源

- 包：`怀旧岛192` `Data/UI/UI_005.wz`
- 钥：IV `AAAAAA==` + 128B `WzAESConstant.DEFAULT_KEY` base64（`tools/_gms_key_128.b64`）
- 节点：`UIWindow4.img/Equip`

## Equip 直接子节点

`backgrnd`, `backgrnd2`, `tabbar`, `Equip`, `Cash`, `Pet`, `Android`, `Dragon`, `Mechanic`, `Coordinate`, `Tab`, `Zero_Cash`, `Angel_Cash`, `Haku`, `totem`

## Android

| 节点 | 说明 |
| --- | --- |
| `Android/backgrnd` | 162×209 canvas |
| `Android/Slots/1200`…`1206` | 槽位底板（名=BodyPart） |
| `Android/BtShop` | 商店钮 |

## Angel

- `CAndroidTab::InitEquipItem` @`0x1202630`：`nItemTI` / wire 从 **−1200** 递减至 **> −1207**
- 字符串：`UI/UIWindow4.img/Equip/Android/Slots/%d`、`.../backgrnd`、`.../BtShop`

## 转换产物

- `batch_convert_key` verify ok；fingerprint `ba82e943c131572d02b2b315bbf9760c9ae5572465b61ab8bca386edece03780`
- SHA256 `77EDF3E654CFB131B165083A771357BFCE4F8A66CD4A4237F08BD579769720D1`
- live：`BeiDou-Client-xy\Data\UI\UIWindow4.img`（220436 B）；bak `UIWindow4.img.bak_pre_equip192_20261122`
