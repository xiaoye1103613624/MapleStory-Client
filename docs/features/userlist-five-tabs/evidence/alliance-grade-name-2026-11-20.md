# 联盟职衔名编辑窗证据（2026-11-20）

## 工具链

- Angel IDA：`127.0.0.1:13337` → `` `Angel.i64 ``
- 对照 v95：`13338` → Maplestory95.i64（命名更全）
- IMG：orange-wz `UIWindow.img`，GMS IV `TSPHKw==` + 128B key
- s9：`AllianceOperationHandler.java` case `0x08`

## Angel 函数 / EA

| 符号 | EA | 要点 |
|------|-----|------|
| `CWndAllianceGrade::OnButtonClicked` | `0x9181e7` | `2140`→OnChangeGradeName；`2141`→OnSaveGradeName |
| `CWndAllianceGrade::OnChangeGradeName` | `0x918204` | `CUtilDlgEx INPUT_STR` maxLen=10 |
| `CWndAllianceGrade::OnSaveGradeName` | `0x91848d` | YesNo → `COutPacket(143)` + `Encode1(8)` + 5×`EncodeStr` |
| OnCreate（匿名 `sub_917546`） | `0x917546` | bg=`UI/UIWindow.img/UserList/GuildUnion/backgrnd`；行条=`.../Guild/GuildGrade`；`BtModf`(25,268)/`BtSave`(125,268) |
| UserList 入口 | `0x91d29d` case `0x809` | `(*vtbl+60)(this, !flag)` = ToggleAllianceGrade |
| ChangeMaster | case `0x80C` → `sub_915983` | **不是**本窗；位图位 = GuildUnion `BtChange` @≈(159,316) |

落盘反编译：`angel-cwndalliancegrade-*.dec.txt`、`angel-sub_917546-oncreate.dec.txt`、`v95-togglealliancegrade.dec.txt`。

## IMG 节点（orange-wz）

- `UserList/GuildUnion/backgrnd` 233×304
- `UserList/Guild/GuildGrade/guildgrade0..4`
- `UserList/Guild/GuildGrade/BtModf` / `BtSave`（四态 normal/pressed/disabled/mouseOver）

## s9 字段

```text
case 0x08:
  for i in 0..4: ranks[i] = readString()
  alliance.updateRankTitles(ranks)
  → changeAllianceRankTitle 广播（客户端 0x1A 写 AllianceModel）
```

## 本端接线

- `AllianceGradeNamePacket`：`write_byte(0x08)` + 5×string
- `UIAllianceGrade`：UNION `BT_UNION_INFO` Toggle；禁 SoftKey
- `0x1A` handler：刷新打开中的 `UIAllianceGrade::reload_from_model`
