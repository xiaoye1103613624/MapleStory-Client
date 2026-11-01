# r26 · Fredrick 补洞 + Hammer/Vega/Family 出站

## Fredrick

- 补 `PacketSwitch`：`emplace<FREDRICK_MESSAGE, FredrickMessageHandler>()`（310）
- 出站已有：`FREDRICK_ACTION` GetAll `0x1A` / Exit `0x1C`
- 入站：`FREDRICK` `0x23` 列表 → `UIStoreBank::load`

## ViciousHammer

| 方向 | Opcode | 说明 |
|------|--------|------|
| C→S | **260** `USE_HAMMER` | 空包（s9 UseHammerHandler → sendHammerMessage） |
| S→C | **354** | `0x39` data(used) / `0x3D` message |

`UIViciousHammer::BtStart` → `UseHammerPacket`；handler 填状态文案。

## VegaSpell

| S→C | **358** `VEGA_SCROLL` | op `0x40` 进行中 / `0x41` 成功 / `0x43` 失败（UseCashItem 触发） |
| C→S | （无独立） | 卷轴走 UseCashItem |

## Family 出站起步

- `OPEN_FAMILY` **146** — UIFamily 构造时派发
- `OPEN_FAMILY_PEDIGREE` **145** — BtTree

## Memo

- `NOTE_ACTION` **131** 已进 OutPacket；丢弃包类已备，列表解析仍预留
