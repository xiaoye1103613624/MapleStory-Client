# IDA：healer / pulley（公会 PQ 物件态机）

取样：`BeiDou.exe` IDB（`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64`）

## 地图装载

| 项 | 地址 / 结论 |
| --- | --- |
| 字符串 | `healer` @ `0xAF4A68`、`pulley` @ `0xAF4A58`、`yMin` @ `0xAF4A60` |
| 唯一 xref | → `sub_5587B3`（`CField_GuildBoss` 构造，vtable `@0xAF3F3C`） |
| 属性 | StringPool **977=`x`**、**978=`y`**（`beidou_v177_decrypted_strings.tsv`） |
| healer 初始化 | `sub_558455(x, yMin, path)` → 层挂在 field+508 |
| pulley 初始化 | `sub_558612(x, y, path)` → 层挂在 field+513 |
| 初始 | `field[506]=yMin`（当前 healer Y）；`field[507]=0`（pulley state） |

## 封包分发（与 s9 opcode 同号）

`sub_558B0F`（vtable OnPacket 覆写，`@0xAF3EE8`）：

```text
opcode == 287 → sub_558C0C(packet)   // HealerMove
opcode == 288 → sub_558C3B(packet)   // PulleyStateChange
else          → sub_531325(...)      // 通用字段包
```

### 287 HealerMove — `sub_558C0C`

1. `short newY = read_short(packet)`
2. `sub_55852B(healer_obj, newY - field[506])` — IWz 层 RelMove(ΔY)
3. `field[506] = newY`

### 288 PulleyStateChange — `sub_558C3B`

1. `byte state = read_byte(packet)`
2. `field[507] = state`（**不改精灵帧**；`sub_558B45` 交互时若 state≠0 则跳过发包）

## WZ 样板（orange-wz 私服 img 内部节点不可靠 → NX 字符串表）

`Map002.nx` → `Map/Map9/990000900.img`：

| 节点 | 值 |
| --- | --- |
| `healer/x` | 295 |
| `healer/yMin` | -564 |
| `healer/yMax` | -350 |
| `healer/rise` | 1 |
| `healer/fall` | 10 |
| `healer/healer` | `Map/Obj/guild.img/syarenian/boss/0` |
| `pulley/x` | -59 |
| `pulley/y` | -394 |
| `pulley/pulley` | `Map/Obj/guild.img/syarenian/boss/1` |

Obj 资源在 `Map.nx`（非 Map002）。

## s9

`GuildPackets.GuildBoss_HealerMove(short)` / `GuildBoss_PulleyStateChange(byte)`；脚本侧当前**无调用点**（仅封包工厂），真机需 GM/脚本主动发或补 PQ 脚本。
