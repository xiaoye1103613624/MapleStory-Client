# TemporaryStatView `!buffme` 真机重试 — 2026-11-06（续）

## 目标

关 NPC 对话 →「界面」→ SoftKey `A` → 点蓝字 `!buffme` → SoftKey `D` 验明细行。

## 本轮结果

| 步骤 | 结果 | 说明 |
| --- | --- | --- |
| 强杀占用真客户端 | ✅ | `BeiDou-Client-xy\MapleStory.exe`；幽灵 PID 无 Path 忽略 |
| 重启客户端 | ✅ | 标题 `XiaoYE MS v0.0.1` |
| 登录 `admin/admin` | ❌ | computer-use 键注入/剪贴板对登录框**均未写入**（字段始终空） |
| 关 NPC / Admin / buffme / D | ⏭ | 未进图，阻塞于登录 |

## 根因备忘

GLFW/OpenGL 登录框不吃 OS `SendInput`/剪贴板（与上轮 SoftKey/聊天框吞键同类）。  
建议手点：账号密码 `admin`/`admin` → 连接 → 关对话 →「界面」→`A`→点 `!buffme`→`D`。

Admin 列表 `!buffme` 与 SoftKey `D` 开板路径代码侧已接（`ec97ee9`）。
