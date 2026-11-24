# 联盟 Kick / 职级升降 / ChangeMaster 出站证据（2026-11-20）

## 工具链

- Angel IDA：`127.0.0.1:13337` → `E:\MXD\反编译IDA\v83\`Angel.i64`（module ``Angel.exe``）
- 对照：v95 `13338` → Maplestory95.i64（命名符号 `OnKick`/`OnGradeChange`/`OnChangeMaster`）
- s9：`BeiDou-Server_s9/.../AllianceOperationHandler.java`

## Angel 083 出站（COutPacket type=143 = ALLIANCE_OPERATION）

| 语义 | Angel 函数 | Encode | s9 case |
|------|------------|--------|---------|
| Invite 公会 | `sub_914CFE` @0x914cfe | `1(3)+Str(guildName)` | 0x03 |
| Leave | `sub_914E39` @0x914e39 | `1(2)` | 0x02 |
| Kick 驱逐公会 | `sub_914EB4` @0x914eb4 | `1(6)+4(guildid)+4(allianceid)` | 0x06 |
| Grade Up/Down | `sub_91505B` @0x91505b | `1(9)+4(cid)+1(bUp)` | 0x09 |
| Notice | `sub_915214` @0x915214 | `1(10)+Str` | 0x0A |
| ChangeMaster | `sub_915983` @0x915983 | `1(7)+4(cid)` | 0x07 |
| 职级名保存 | `CWndAllianceGrade::OnSaveGradeName` @0x91848d | `1(8)+5×Str` | 0x08（独立窗，非 UNION 底栏） |

Angel `CTabGuildAlliance` **无** 独立 `OnKick`/`OnGradeChange`/`OnChangeMaster` 符号；出站落在上述匿名 stub，与 v95 命名方法字段同构（v95 opcode 167，action 字节一致）。

## s9 字段（AllianceOperationHandler）

```text
0x06: readInt(guildid) + readInt(allianceid)  // Expel Guild；须 alliance_rank==1
0x07: readInt(victimid)                       // Change Leader；victim rank==2
0x09: readInt(cid) + readByte(raise)          // raise>0 升职；新职级仅 3..5
```

## 接线结论

有字段级证据 → UNION `BtKick`/`BtChange`/`BtUp`/`BtDown` 入盟+选中后 NORMAL，发包：

- `AllianceKickPacket(guild_id, alliance_id)`
- `AllianceChangeMasterPacket(cid)`
- `AllianceGradeChangePacket(cid, raise)`

禁止臆造；0x08 职级名编辑仍走 `CWndAllianceGrade` 浮窗另案。
