# r22 · Messenger 协议闭环（出站 122 / 入站 313）

> 承接 [`r21-wave-a-messenger-shell.md`](r21-wave-a-messenger-shell.md) UI 壳；本轮补包往返与邀请/聊天/离房。  
> 对照：s9 `MessengerHandler` + `PacketCreator.messenger*`  
> 部署：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`

## Opcode

| 方向 | 值 | 说明 |
|------|-----|------|
| C→S | **122** (`0x7A`) | `OutPacket::MESSENGER` |
| S→C | **313** (`0x139`) | `PacketSwitch::MESSENGER` + `MessengerHandler` |

## 出站 mode（`MessengerPackets.h`）

| Mode | 类 | 载荷 |
|------|-----|------|
| `0x00` | `MessengerOpenPacket` | `int messengerid`（0=建房） |
| `0x02` | `MessengerLeavePacket` | — |
| `0x03` | `MessengerInvitePacket` | `string name` |
| `0x05` | `MessengerDeclinePacket` | `string sender` |
| `0x06` | `MessengerChatPacket` | `string text` |

## 入站 mode（`MessengerHandlers.cpp`）

| Mode | 行为 |
|------|------|
| `0x00`/`0x07` | `parse_look` + name/channel → `UIMessenger::set_member` |
| `0x01` | `set_local_pos` |
| `0x02` | `clear_member` |
| `0x03` | `UIYesNo` → 接受 `Open(id)` / 拒绝 `Decline(from)` |
| `0x04`/`0x05` | note → 聊天区或 StatusMessenger |
| `0x06` | `append_chat` |

## UI 行为

- 用户入口（UserList / StatusBar mapleChat / MAPLECHAT）→ `emplace` + `try_create_room()`（`Open(0)`）
- 邀请进房：仅 `Open(id)`，handler `ensure_ui` **不**再建房
- `BtEnter` → SoftKey 名 → Invite；输入框 Enter → Chat；关窗/Esc/`toggle_active` → Leave
- 槽位：最多 3；NameBar 画名（立绘本轮不强制）

## 验收

1. A 开 Messenger → 建房（有 join/add 回包）
2. BtEnter 邀请 B → B YesNo 接受 → 双方槽位更新
3. 聊天互见 → 关窗离房

## 仍预留

- REA CreateCtrl 坐标 / 立绘傀儡 / Friend 入口像素
- `blink` / `chatBalloon` 动画
