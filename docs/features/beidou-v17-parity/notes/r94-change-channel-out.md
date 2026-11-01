# r94 — UIChannel 换频出站

- `OutPacket::CHANGE_CHANNEL = 39`（s9 `0x27`）
- `ChangeChannelPacket(byte channel)`
- `UIChannel::change_channel` → dispatch；同频道则仅关闭
- GameMenu `I` → `UIMinigame`
- `MovementPacket.h` 补 `#include Movement.h`（UIChannel 拉入 GameplayPackets 时暴露）

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
