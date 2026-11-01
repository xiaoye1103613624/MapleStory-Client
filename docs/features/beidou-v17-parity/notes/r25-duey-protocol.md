# r25 · Duey / Delivery 包往返

## Opcode

| 方向 | 值 | 名 |
|------|-----|-----|
| C→S | **65** | `DUEY_ACTION`（修正：原 `GUILD_OPERATION` 出站误用 65 → 现改为 **126**） |
| S→C | **322** | `PARCEL` |

## 出站（`DueyPackets.h`）

| Mode | 类 |
|------|-----|
| `0x00` | DueyOpenRecvPacket |
| `0x02` | DueySendPacket（invent/slot/qty/mesos/name/quick[/msg]） |
| `0x04` | DueyClaimPacket |
| `0x05` | DueyDiscardPacket |
| `0x07` | DueyClosePacket |

## 入站（`DueyHandlers.cpp`）

`0x08` 列表解析 → `UIDelivery::set_packages`；`0x17` 移除；`0x09–0x16` 提示；`0x19/0x1B` StatusMessenger。

## UI

收件列表双击领取；Send 页 SoftKey 名+金币寄出；开窗自动 `0x00` 刷新。

## 附带修正

`OutPacket::GUILD_OPERATION`：**65 → 126**（对齐 s9 RecvOpcode.GUILD_OPERATION=0x7E）。
