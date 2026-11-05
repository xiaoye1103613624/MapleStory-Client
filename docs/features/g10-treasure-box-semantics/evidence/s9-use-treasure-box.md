# s9 UseTreasureBox 证据摘要

## UseTreasureChestHandler

- 读 `short slot` + `int itemid`
- 仅处理 `4280000`（金）/ `4280001`（银），需现金栏钥匙 `5490000/5490001`
- 成功：`getShowItemGain` + `PacketCreator.UseTreasureBox(toUse.getItemId())`

## PacketCreator.UseTreasureBox

```java
OutPacket p = OutPacket.create(SendOpcode.SUCCESS_IN_USE_GACHAPON_BOX);
p.writeInt(type); // itemId
```

## 与客户端对照

| 层 | 正确 | 修前错误 |
| --- | --- | --- |
| 出站 | `UseTreasureChestPacket`（背包已接） | — |
| 入站 | `GachaponBoxHandler` 读 int → `UITreasureBox` | 读 byte → `UIRemoteGachapon` |
| HitReactor | 仅 Reactor 动画 | 任意 state>0 弹 TreasureBox + 英文场标 |
