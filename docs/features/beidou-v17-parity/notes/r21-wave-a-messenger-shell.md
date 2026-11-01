# r21 · Wave A · Messenger UI 壳（第 1 切片）

> 范围：开窗 / 关窗 / 画底板与按钮 / 入口接线。**不做** `MESSENGER`(313) 收发包与 `PacketSwitch` emplace。  
> 部署：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`（Release|x64，2026-10-22 21:48，2,731,008 B；`tools/build_smoke.bat` EXITCODE=0）  
> 切片状态：**已完成**（wz-probe / UIMessenger / 入口 / 构建笔记）

## WZ 节点（`UIWindow.img/Messenger`）

证据：[`evidence/r21/messenger_wz_nodes.txt`](evidence/r21/messenger_wz_nodes.txt)、[`evidence/r21/messenger_backgrnd.png`](evidence/r21/messenger_backgrnd.png)

| 节点 | 说明 |
|------|------|
| `backgrnd` | 295×364 底板 |
| `backgrnd2` | 295×240 上层板 |
| `backgrnd3` | 184×22 标题条 |
| `NameBar/0..3` | 四槽名牌（89×19） |
| `BtEnter` | 进入/邀请钮（壳阶段仅日志） |
| `blink` / `chatBalloon` | 本切片未接动画 |

Origin 向量在 nx 探针侧有脏值；`Texture` 构造已 sanitize，布局按原点 (0,0) + 默认槽位。

## 实现

| 项 | 路径 |
|----|------|
| UI 类 | `IO/UITypes/UIMessenger.h/.cpp` |
| 类型 | `UIElement::Type::MESSENGER`（≠ `STATUSMESSENGER`） |
| 窗位 | `PosMESSENGER` → `ui.position.messenger` 默认 `(500, 80)` |
| 工程 | `MapleStory.vcxproj` + filters |

行为：拖拽 / `BtClose` 关窗 / Esc 关窗；`BtEnter` → `LOG_DEBUG`，不发包。

## 开窗入口

| 入口 | 行为 |
|------|------|
| **主**：`UIUserList` Friend 页 `BT_FRIEND_MESSENGER` | Friend 无 `BtMessenger`/`BtChat` 时复用 `Messenger/BtEnter`，位 `(208, 85)`；`emplace<UIMessenger>()`（TOGGLED） |
| **次**：`UIStatusBar` 社区 `BT_COMMUNITY_MAPLECHAT` | 有 Messenger 资源 → toggle UIMessenger；否则回退 `UIChat` |
| 快捷键 `KeyAction::MAPLECHAT` | 同上优先 Messenger |

## 手工验收

1. Release|x64 → 部署 `BeiDou-Client-xy`
2. 进图 → 开 UserList 好友 → 点 Messenger 入口 → 底板/`BtEnter`/`BtClose` 可见
3. 社区菜单 mapleChat / 快捷键 → 同窗 toggle
4. 关钮 / Esc 有效；控制台无崩溃；无 313 发包

## 显式预留（后续切片）

- **313 包序**：`MessengerPackets` / `MessengerHandlers` / `PacketSwitch` emplace（当前仅调试名）
- **REA 升 C**：CreateCtrl 坐标校准 NameBar / BtEnter（现为 WZ origin + 默认位）
- **邀请聊天闭环**：选好友邀请、进房、聊天气泡 `chatBalloon`/`blink` 同步

## 本切片不做

PartySearch 及 Wave A 其余项；选角 charInfo / 底栏像素抛光；包往返。
