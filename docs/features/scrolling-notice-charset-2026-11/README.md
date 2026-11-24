# 滚动公告中文乱码（2026-11-18）

## 结论

| 项 | 内容 |
| --- | --- |
| 来源 | 服务端 `SERVERMESSAGE` type=4（`PacketCreator.serverMessage` ← `game_config.server_message` / `Channel.setServerMessage`），非 WZ |
| 客户端路径 | `ServerMessageHandler` → `UI::set_scrollnotice` → `ScrollingNotice`（`Text` A12M + SourceHanSerif） |
| 乱码形态 | 本部署自动登录 `admin` 且 `accounts.language=2` → 线上按 **US-ASCII** 发，中文变 `????`（用户口语「乱码」）；另一类「娆㈣繋」= **UTF-8 被当 GBK** |
| 库内原文 | `server_message` = `欢迎来到 XiaoYe冒险岛！！`（UTF-8 正确，`HEX E6ACA2…`） |

## 已修

1. **服务端** `PacketCreator.serverMessage`：`isZhCN()` 时强制 `getCharset(3)`（GBK）写正文，不再跟账号 language（与 `setNPCScriptable` 同口径）。
2. **客户端** `packet_to_internal`：若线上字节已是严格 UTF-8（含多字节），跳过 CP936，防「UTF-8 当 GBK」。

## 可选（未做 DML）

```sql
UPDATE accounts SET language = 3 WHERE name = 'admin';
```

或其它中文号登录。`language=2` 仍会导致**其它**跟账号语言走的中文字段变 `?`。

## 验收

1. 重启 s9（加载新 `PacketCreator`）→ 进图看顶栏滚动字为「欢迎来到 XiaoYe冒险岛！！」。
2. 客户端含本次 `PacketCharset.cpp` 后同步到 `E:\MXD\BeiDou-Client-xy`（若 exe 占用按惯例处理）。
3. `tools/probe/charset_check`：原有向量 + `utf8 wire to_internal identity` 全 PASS。
