# r198 — SoftKey tip→out 饱和状态

## 结论

`send_key` SoftKey 重开路径已近饱和；有明确 OutPacket 的 tip→out 已接到（Quiz/Event/Claim/MultiLine/TV/Merchant/Enchant/Delivery/Search/ItemMove 等）。

## 仍浅层（无独立 opcode / 故意保留）

| UI | SoftKey 行为 | 原因 |
| --- | --- | --- |
| Radio / Minigame `T` | GeneralChat | 无 radio/minigame-chat 出站包 |
| CreateCygnus | GeneralChat 确认名 | 创角包仅登录态 |
| Carnival / Ariant / PartyRace tip | GeneralChat | 无赛事 chat 包；仅 tip 亦可 |
| MobGage / FloatNotice tip | GeneralChat | HUD tip 调试 |
| Radio / Minigame `T` | GeneralChat | 无专用 chat 出站包 |

## 已解（r200）

PersonalShop / Member / Entrusted：SoftKey 店名 → `HiredMerchantCreatePacket`（CREATE）；空/`O` → OPEN_STORE。

## 下一优先（非 SoftKey）

1. 登录链实机叠图（对照 `evidence/r20/correct_shots/`；偏移 knobs 已在 r15/r20 标定）
2. Messenger 立绘（搁置）
3. PersonalShop CREATE 自由市场实机验收

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
