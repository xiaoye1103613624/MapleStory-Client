# 好友 UI（UIBuddyList）校准 · 2026-11-10

## 目标
将好友窗 `UIBuddyList` 对齐 **BeiDou 运行时数据**与 **s9 服务端协议**，修正原代码读不到资源、缺「加好友」按钮的真实缺口（R6）。

## 取证
- **服务端协议（s9，`BeiDou-Server_s9`）**：
  - `BuddylistModifyHandler`：c2s `mode 1=add(name+group)` / `2=accept(cid)` / `3=delete(cid)`。
  - `PacketCreator`：s2c `updateBuddylist(mode7)`、`requestBuddylistAdd(mode9)`、`updateBuddyChannel(0x14)`、`updateBuddyCapacity(0x15)`，`SendOpcode.BUDDYLIST=0x3F=63`。
- **客户端（MapleStory-Client）**：
  - `Net/Packets/BuddyPackets.h`：c2s `BuddyAddPacket/BuddyAcceptPacket/BuddyDeletePacket` 字段序与 s9 逐项对齐；`OutPacket::Opcode::BUDDYLIST=130=0x82`（s9 `RecvOpcode.BUDDYLIST_MODIFY=0x82`）。
  - `Net/Handlers/BuddyHandlers.cpp`：s2c 解析 mode 7/9/0x14/0x15 与 s9 发送格式逐项吻合（含 `channel-1` 编码与客户端 `-2/-1` 在线判定配对）。
  - `PacketSwitch.cpp`：`emplace<BUDDYLIST, BuddyHandler>()`，recv opcode 63=0x3F 一致。
- **结论：好友协议 c2s + s2c 与 s9 端到端逐项对齐，无需改协议。**
- **资源**：运行时部署数据 `Data/UI/UIWindow.img` 的好友窗位于 `UIWindow.img/Recommendations/FriendsList`（`back` 182×310 背景 + `BtClose` 13×13 + `BtAddBuddy` 13×13）。原代码读 `UI["BuddyList.img"]`（精简数据中不存在）→ 窗口无背景、且无「加好友」按钮（枚举有 `BtAdd` 但构造从未实例化）。
- **本地化**：`buddylist.title`=好友({})、`common.channel`=频道{}、`common.offline`=离线，zh-CN/en-US.ini 均存在（`common.online` 代码未用，MISSING 无碍）。

## 决策点
| # | 决策 | 结论 |
| --- | --- | --- |
| D1 | 资源路径 | 改为 `nl::nx::UI["UIWindow.img"]["Recommendations"]["FriendsList"]`（R6：以运行时数据为准） |
| D2 | 加好友按钮 | 构造内实例化 `BtAdd`（`buddy["BtAddBuddy"]`），点击即 `prompt_add()` |
| D3 | 窗口尺寸 | 由 200×320 对齐资源 `back` 182×310 |
| D4 | 协议 | 已核验 c2s/s2c 与 s9 对齐，不改 |
| D5 | 本地化 | 键齐全，不改 |

## 修复（`IO/UITypes/UIBuddyList.cpp`）
- 资源路径修正 + 渲染 `back` 背景。
- 实例化 `BtClose`（顶右 165,4）与 `BtAdd`（顶右邻关闭钮左侧 146,4）。
- 窗口尺寸 `(182, 310)`。
- 全部 `if(node)` 守卫，节点缺失即降级（R6 合规，不崩）。

## 门禁
- `tools/build_smoke.bat` → `EXITCODE=0`，`MapleStory.exe` 已写入部署目录。

## 残留（真机核验）
- 按钮坐标（BtClose 165,4 / BtAdd 146,4）按标准 v83 布局放置，需进 `pending-live-tests` 截好友窗与参照端比对（背景/关闭/加好友按钮落位）。
- 需用户启 s9 + 建好友关系后截屏验证功能闭环。
