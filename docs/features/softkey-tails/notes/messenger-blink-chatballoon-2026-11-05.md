# Messenger blink / chatBalloon（2026-11-05）

## 解析摘要

已用 **IDA（Maplestory95.i64）+ NX（UIWindow.img/Messenger）** 对照：

| 层 | 结论 |
| --- | --- |
| IDA | `CUIMessenger::StartBlink` / `ToggleBlink` / `Update` / `OnChat` / `SetAvatar`（`pBalloon` ← `UI/UIWindow2.img/Messenger/chatBalloon`） |
| IMG | 本包 `UIWindow.img/Messenger/blink` 12×10 橙标；`chatBalloon/0..4` 46×36 delay=120（白泡 + 递增省略号） |
| BeiDou.exe IDB | **无** Messenger UI 符号（仅 CNM* Windows Live）；逻辑 oracle 用 v95 |

证据目录：`docs/features/softkey-tails/evidence/messenger-blink/`  
（PNG 导出 + `ida-v95-blink-chatballoon.txt`）

## 已证实行为

1. **blink**：窗口级 `m_pLayerBlink`。`OnChat`（真消息）/`OnEnter`/`OnLeave` → `StartBlink(1)` 置 `m_nBlink=30`。`Update` 仅在 **`m_nState==2`（最小化）** 时半周期显隐；非 Min 态每帧 `StartBlink(0)` 清掉。
2. **chatBalloon**：每座位 `CHARACTER::pBalloon`，`SetAvatar` `LoadLayer(…/chatBalloon)`，`RelMove(-45,-100)` 相对 Avatar 原点。`OnChat` 无冒号且尾字符 `0|1` → `Animate(GA_STOP|GA_REPEAT)` 打字指示；有 `" : "` → `AddChatText` + `StartBlink`。
3. **打字出站**：`Update` 在输入空↔非空切换时 `Encode1(6)+EncodeStr(name+"0"|"1")`，节流约 3000。

## 本端实现（适配）

| 项 | 做法 |
| --- | --- |
| 资源 | 构造加载 `blink` Texture、`chatBalloon` Animation 样板 |
| chatBalloon | 座位 `typing` 时在脚点 + `(-31,-70)`（0.70×原生 RelMove）循环绘制 |
| blink | **无 Min 态**：收真聊天后标题条右上闪一周期（30 tick，半显半隐） |
| 协议 | `handle_messenger_chat` 分流；本机输入框空/非空发 typing 包 |

## 局限 / 待真机

- 原生 blink 只在最小化标题条闪；本端开窗也闪，属适配。
- Balloon 偏移按 0.70 缩放近似，双人房截图可再调 `BALLOON_DX/DY`。
- 服务端若透传 typing 串进聊天历史，需 s9 侧忽略无冒号的 0/1 包（本端已不入历史）。

## 会话占用策略

本会话冲突选 **B**：结束 MapleStory 相关进程后覆盖同步；后续同类不再询问。  
（PID 34476 无路径幽灵进程 Access Denied，未锁 OutDir，构建已直写部署目录。）
