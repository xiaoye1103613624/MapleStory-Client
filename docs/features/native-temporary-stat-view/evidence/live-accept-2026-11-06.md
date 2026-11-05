# TemporaryStatView 真机 — 2026-11-06（续）

## 本轮开发（已部署 `BeiDou-Client-xy`）

| 改动 | 目的 |
| --- | --- |
| `UI.cpp` SoftKey ASCII 回退 | 未绑定键 `get_mapping→NONE/0` 时，把 GLFW 键码（=ASCII）当下传 `action`，否则 SoftKey `'A'/'S'/'D'` 永不命中 |
| `UIShortCut::FOCUSED=true` | 原 `FOCUSED=false` 导致「界面」气球打开后按 `A` 进不了 `UIShortCut::send_key`，Admin SoftKey 造 buff 死路径 |
| `UI.cpp` 未绑定 `D` 兜底开/关 TemporaryStatView | `UIBuffList::FOCUSED=false`，BuffList SoftKey `D` 不可达；与 BuffList SoftKey D 同语义 |
| `UIAdmin` quick 加 `buffme` | 点击 `!buffme` 发 `GeneralChatPacket`，绕开聊天框吞 `!`/空格 |

## 真机结果

| 项 | 状态 | 证据 |
| --- | --- | --- |
| SoftKey `D` 开明细板 | ✅ 面板半透明黑底出现（左上） | `live-2026-11-06-tsv-softkey-d.webp` |
| 明细行（名称/层数/倒计时） | ❌ | 无 `GIVE_BUFF(0x20)`；Admin lv1 仍无服务端 buff |
| SoftKey `界面→A→!buffme` 自动 | ❌ | `packet_dump` SEND `31 00`=0；自动化抢焦点/SoftKey 文本注入仍不稳 |
| 手点 Admin `!buffme` / SoftKey 确认 | ⏳ | 留给手点；路径已接 |

## 根因备忘（SoftKey 字母）

1. KEYMAP 未绑字母 → `Mapping(NONE,0)` → SoftKey 比较 `'A'` 失败 → ASCII 回退已修  
2. ShortCut `FOCUSED=false` → 即使回退有 action，焦点也不在 ShortCut → 已改 `true`  
3. 聊天框 / SendInput Unicode 仍常吞 `!`；优先 **Admin 面板点 `!buffme`** 或 SoftKey 文本框手输  

## 下轮（手点 1 分钟可闭环）

1. 进图 →「界面」→ `A` → SoftKey 手输 `!buffme` 回车，**或** 关 SoftKey 后点 Admin 列表 `!buffme`  
2. 确认右上 BuffList 图标 + SoftKey `D` 明细行有中文名/倒计时  
3. 截图覆盖本目录  

## 2026-11-06 续轮（本会话）

| 项 | 状态 | 备注 |
| --- | --- | --- |
| SoftKey `D` 开板 | ✅（上轮） | 已提交 `ec97ee9` |
| 自动化 `!buffme`（SendKeys / SoftKey A / 聊天框） | ❌ | `packet_dump` 无 SEND `31 00`、无 RECV `20 00`；仅心跳 `18 00`/`11 00` |
| 手点 Admin 列表 `!buffme` | ⏳ | Admin 面板路径已接；自动化抢焦点未点到蓝字列表 |
| 明细行验收 | ⏳ | 依赖造 buff |

建议手点：进图 →「界面」→ `A` → 点 `!buffme`（或 SoftKey 手输）→ `D` 看明细。
