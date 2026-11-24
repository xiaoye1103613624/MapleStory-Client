# #2 聊天表情包（Kaentake 动态表情）原生移植 · 实现记录（2026-11-18）

## 决策（用户已拍板）
- 范围：全量实现（双端）；opcode C `0x3715` / S `0x3716`；入口：聊天栏表情按钮（现成占位接线）

## 服务端改动（BeiDou-Server_s9，mvn compile=0）
| 文件 | 动作 |
|---|---|
| `net/server/handlers/ChatEmoticonHandler.java` | 新建：白名单校验（与参考包 21 段 ID 一致）+ 1.5s 冷却 + `map.broadcastMessage(player, packet, true)` 排除自己；实现 `validateState(Client)` 适配 s9 `PacketHandler` 基类 |
| `util/PacketCreator.java` | 加 `chatEmoticon(charId, emoticonId)`：`writeInt(charId)+writeInt(emoticonId)` |
| `net/opcodes/RecvOpcode.java` | 加 `CHAT_EMOTICON(0x3715)` |
| `net/opcodes/SendOpcode.java` | 加 `CHAT_EMOTICON(0x3716)` |
| `net/PacketProcessor.java` | 注册 handler |

## 客户端改动
| 文件 | 动作 |
|---|---|
| `Net/OutPacket.h` | `Opcode::CHAT_EMOTICON = 14101 (0x3715)` |
| `Net/Packets/ChatEmoticonPackets.h` | 新建 `ChatEmoticonPacket(id)` |
| `Net/PacketSwitch.cpp` | 加 enum `CHAT_EMOTICON=0x3716` + `emplace<CHAT_EMOTICON, ChatEmoticonHandler>()` |
| `Net/Handlers/WaveAShellHandlers.h/.cpp` | 新建 `ChatEmoticonHandler::handle`（readInt charId+emoticonId → `UIChatEmoticon::show_overhead`） |
| `IO/UITypes/UIChatEmoticon.h/.cpp` | 新建：表情选择窗（4 分页网格，从 `Effect/ChatEmoticon.img/Dynamic{n}/{id}/effect` 读缩略图，点击发 `ChatEmoticonPacket`）+ 静态头顶动画管理器 |
| `IO/UITypes/UIChatBar.cpp` | `BtChatEmoticon` 点击 case 从空壳改为 `emplace<UIChatEmoticon>()` |
| `IO/UIElement.h` | enum Type 加 `CHATEMOTICON`（须整工程 Rebuild） |
| `Configuration.h` | 加 `PosCHATEMOTICON` |
| `Gameplay/Stage.cpp` | draw 链加 `UIChatEmoticon::draw_overhead(viewpos, alpha)`；update 链加 `update_overhead()` |
| `MapleStory.vcxproj` | 登记新文件 |

## 资源（已入 live，未改动）
`Data/Effect/ChatEmoticon.img`(9.4MB) + `Data/UI/UIWindow.img` 的 ChatExpression 节点（若窗背景缺失则纯控件可点，不崩）。

## 验证
- 服务端：`mvn -q -o compile -DskipTests` → MVN_EXIT=0（已确认）。
- 客户端：首次增量构建 LNK2001 `__std_find_*`/`__std_search_*`（STL 符号跨 obj）；第二次 `/t:Rebuild /m:2` 仍 LNK2001。RCA=脱离 #4 已验证基线（自加 `/m:2` + `/t:Rebuild`）；回到基线（纯增量 `/m:1` + `DebugType=none`）即 `BUILD_EXITCODE=0`。**教训：已验证的构建配置不要自作聪明改参数。**
- 最终三证齐备：链接行 `MapleStory.vcxproj -> E:\MXD\BeiDou-Client-xy\MapleStory.exe`；error=0；UIChatEmoticon.obj 20:24 更新（含 Texture frame0 修正）。live exe 20:28 更新 SHA256=`50FDCAB7B85FE1C80FE4F8E205021B448273CA8324D924C0CC1597669D45318D`。
- 蓝军自检缩略图 P1 已修：`Texture(nl::node)` 需 canvas 节点，初版传 `effect`(imgdir) 会空纹理 → 改传 `effect["0"]`（frame0）；`Animation(effect)` 保持（吃 imgdir 帧容器）；头顶坐标 `get_position()+viewpos` 对齐 MapChars.cpp:42。
- 真机发包/观感：**未验收**，待用户点聊天栏表情按钮复核（确认是本端截图）。

## 风险与取舍
- 白名单 ID 用服务端 21 段前缀生成，缩略图按资源存在性加载；参考包注释"部分 ID 有空洞"，空 ID 缩略图为空但仍可点击发包（服务端白名单会放行段内 ID，头顶动画若资源缺失则不显示）。
- 头顶动画 `get_position()+viewpos` 口径对齐 MapChars.cpp:42；3 秒倒计时按 ~60fps 粗略计时。
- 未改 live 资源、未碰数据库。
