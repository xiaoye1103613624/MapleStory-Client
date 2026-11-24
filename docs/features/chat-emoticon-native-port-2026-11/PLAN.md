# #2 聊天表情包（Kaentake 动态表情）· Phase0 全链路方案（2026-11-18）

## 结论：可闭环，双端中等功能；资源已入 live，无需导入

客户端 `BtChatEmoticon` 按钮是**占位空壳**（`UIChatBar.cpp:780-783` case 为空），服务端无 opcode。这是真缺失。但资源（ChatEmoticon.img 9.4MB + UIWindow.img ChatExpression 节点）已在 live，服务端参考 handler 干净（66 行），头顶有现成动画机制——可在原生架构一次性闭环。

## 全链路证据（逐行号）

### 服务端（参考包，可直接适配 s9）
- `ChatEmoticonHandler.java:17-38`：收包读 int emoticonId → 白名单校验 `isSupported` → 1.5s 冷却（ConcurrentMap）→ `map.broadcastMessage(player, chatEmoticon(charId, emoticonId), true)` 排除自己
- `:40-60` 白名单：四组 Dynamic ID（100xxx/200xxx/300xxx 等共 21 段）
- 发包 `PacketCreator.chatEmoticon(charId, emoticonId)`：收 `0x11C` / 发 `0x17F`（**s9 冲突，须重排**，见决策点）

### 客户端现状
- `UIChatBar.h:189` `BtChatEmoticon` 枚举存在；`UIChatBar.cpp:390` 建钮（`input["button:chatEmoticon"]`）；`:780-783` 点击 case **空实现**
- 头顶机制现成：`IO/Components/ChatBalloon.h/.cpp`（头顶气泡）、`Graphics/EffectLayer.h/.cpp`（特效层）、`Character/Char.h/.cpp`（角色挂特效）
- 发包标准模式：`Net/Packets/*.h`（OutPacket 封装）

### 资源（已入 live，确认可用）
- `E:\MXD\BeiDou-Client-xy\Data\Effect\ChatEmoticon.img`（9.4MB，Dynamic0-3 动画帧）
- `Data\UI\UIWindow.img`（ChatExpression/open/{normal,mouseOver,pressed}/0、backgrnd）
- 参考包 `resources/wz/Effect.wz/ChatEmoticon.img.xml` 可核对 ID/帧结构

## 待拍板决策点（会实质改变交付）
1. **opcode 分配**：参考 `0x11C/0x17F` 在 s9 已占用（0x11C=LEFT_KNOCK_BACK）。需分配空闲对（我按 s9 空闲段选，双端对齐）。→ 授权重排？
2. **入口方式**：参考是聊天栏表情按钮（现成占位）点开网格窗。是否就用这个入口？（符合你"部分功能依赖左侧竖栏"要求吗？还是也要加侧栏图标？）
3. **表情范围**：参考包四组约 270 个 ID（Dynamic0 100 / Dynamic1 47 / Dynamic2 120 / Dynamic3 3）。是否全量，还是先做一组验证？
4. **头顶动画呈现**：参考是气泡+动态帧约 3 秒。用现成 ChatBalloon 挂载还是新建层？建议复用现成机制。

## 实现清单（拍板后一次性成型）
- 服务端：`ChatEmoticonHandler.java` + `PacketCreator.chatEmoticon` + opcode 枚举 + PacketProcessor 注册（4 处）
- 客户端：`Net/Packets/` 新 packet + `PacketSwitch` 注册 + 表情选择窗 `UIChatEmoticon` + `UIChatBar` 按钮接线 + 头顶特效挂载 + config 开关（~6 处）
- 验证：双端构建（mvn + VS 三证）+ opcode 对齐 + 真机发包待用户复核

## 风险
- 头顶动画需按资源帧结构调，可能要真机校准（用户执行）
- 服务端 `LAST_USE` 静态 map 无清理（参考包原样，可接受）

## 本轮动作
无代码改动；待用户拍板上述 4 点后一次性实现。
