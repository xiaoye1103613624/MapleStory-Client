# 本端 Login.img orange-wz GMS 钥复核（2026-11-18）

## 上一轮实际传钥（会话记录）

加载 `E:\MXD\BeiDou-Client-xy\Data\UI\Login.img`（及 `Login.live.copy.img`）时**已用 GMS 规格**，不是 LATEST：

| 次序 | key.name | ivBase64 | userKeyBase64 | 结果 |
| --- | --- | --- | --- | --- |
| 合入尝试 | `GMS` | `TSPHKw==` | 128B DEFAULT_KEY（HEAD `EwAAAFIAAAAqAAAAWwAAAAgA`） | `IMG 解析失败: Login.img` |
| 单独重载 | `GMS128` | `TSPHKw==` | 同上 | 同失败 |
| 副本 + LATEST（对照） | `新版本客户端` | `AAAAAA==` | 同上 | 同失败 |
| 同会话新建/转钥 staging | `GMS` | `TSPHKw==` | 同上 | **可解析**（`evan` 子树可读） |

192 用的 `AAAAAA==` 只作用于 `UI_001.wz`，**未**作为本端 Login 主路径钥。

## 本轮重试（MCP 实测）

- `load_files(Login.img, exclusive:false|true, key={name:GMS, iv:TSPHKw==, userKey:128B})` → `loadedCount:1`
- `list_children(.../RaceSelect, autoParse:true)` → **原文** `IMG 解析失败: Login.img`
- `search_node(keyword=RaceSelect)` → 同上
- 文件：size **7212655**；SHA256 **E545A0AE3FE4AC842DFF47DAF2AA88850D884220E0023032289899ABA030F21C**；Open Read 可打开（未写锁）
- `tools/_wz.py`（65536 `key_gms`）对本文件 **exit 0 可读**

## 结论（已被同日后续对照翻案）

当时会话下 GMS128 报 `IMG 解析失败`。同日 Orz 对照（`orz-vs-mcp-login-img-2026-11-18.md`）证明：Orz「国际服务器(低版本)」≡ MCP GMS128，**同 SHA256 文件同钥现可解析并 list RaceSelect**。本文件结论不再作为定论；以 Orz 对照证据为准。
