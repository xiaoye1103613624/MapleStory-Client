# A11f GetText Avatar 变体 + 包内 min/max — IDA 取证

> 日期：2026-11-05 ｜ IDB：`BeiDou.exe.i64`（`E:\MXD\BeiDou-ClientV17.7`）

## 1. Avatar 布局 `sub_9A8716`（0x9A8716）

分发：`sub_9A6370` case dlgType 2/3：

```text
if (a1[41] != 0)  // this+0xA4
  sub_9A8716;     // Avatar 变体
else
  sub_9A83F6;     // 非 Avatar（A11e）
```

`sub_9A8716` ASM/伪代码定案：

| 元素 | 几何 |
| --- | --- |
| Edit(1000) | `(19, H-73)`，`226×15` |
| BtOK(1) | `(158, H-30)` |
| BtClose(2) | `(208, H-30)` |
| 黑/白框 | **无**（函数内无 DrawRect） |

对照：非 Avatar `sub_9A83F6` 有黑框 h18 / 白底 h16，BtOK=`(W-54,H-26)`，BtClose=`(9,H-26)`。

### speaker 与 inset

`sub_9ACC20`：`content_left = ([this+0x6E4] != 0) ? 25 : 157`。  
`+0x6E4` 由脚本包装写入：`(speaker & 6) != 0`（见 `sub_746A8B` / `sub_746DFF`）。  
与 `+0xA4`（SetUtilDlgEx 第 5 参，触发 9A8716）**不是同一字段**。

本仓约定：`speaker==1`（UtilDlgEx_Avatar 皮肤）走 **9A8716** 固定槽；非 Avatar 保持 A11e。

## 2. min/max/maxlen 字段与校验收口

### 写入

| 函数 | 地址 | 字段 |
| --- | --- | --- |
| `sub_9AC732`（AskNumber 参数） | `0x9AC732` | `+0xD0=min`，`+0xD4=max`，`+0xDC=maxlen`，`+0xCC=edit 上限(常 10)`，`+0xC8` 默认串 |
| `sub_9AC7CB`（AskText 参数） | `0x9AC7CB` | `+0xDC←a3`（最小长度），`+0xCC←a4` |

本地 AskNumber 包装 `sub_746DFF`：`SetUtilDlgEx(dlgType=2)` + `9AC732(def,min,max,…)`。  
本地 AskText 包装 `sub_746A8B`：`SetUtilDlgEx(dlgType=3)` + `9AC7CB`。

> **编号注意**：原生**内部** dlgType `2=Number / 3=Text`；BeiDou **线协议** msgtype `2=GetText / 3=GetNumber`（`PacketCreator.getNPCTalkText/Num`）。本仓 `TalkType` 跟线协议；校验按 TalkType 分支，不套内部编号。

### 校验收口 `sub_9AB50A`（0x9AB50A）

ASM：`cmp [esi+7Ch], 2` → 数字路径；`cmp …, 3` → 文本最小长度。

数字路径（内部 type2 / 本仓 SENDGETNUMBER）：

1. 逐字符须为 `'0'..'9'`，否则 tip 817  
2. `_atoi64`；越界/负 → tip 819（带 max）  
3. `n < [esi+0D0h]` → tip 818（min）  
4. `n > [esi+0D4h]` → tip 819（max）  
5. 另有对 `+0xDC` 的长度门槛（助手常传 0，空转）

文本路径（内部 type3）：`len < [esi+0DCh]` → tip 821。

本仓：校验失败**不发包、不关窗**（对齐「弹 tip 不收口」；tip UI 未接，仅拒收）。

## 3. 线包（BeiDou-Server_s9）

```text
GetText  (msgtype=2): string talk, string def, int tail(=0)
GetNumber(msgtype=3): string talk, int def, int min, int max, int pad(=0)
```

## 4. 本仓接线

- `NpcDialogueHandler`：按 msgtype 解析 def/min/max/tail → `TalkInputParams`
- `speaker==1` + GetText/Number → Avatar 固定 Edit/OK/Close；不画黑白框
- GetNumber：全数字 + min/max（有 bounds 时）；Edit limit=10
- GetText：`text_min_len` 仅当包尾 int>0；def 填入输入框
- 宋体路线未改

## 5. 局限 / pending

- ~~原生 tip 文案（string 817–821）与 `sub_9929DD`~~ → **A11g 已接**（见 a11g）
- ScriptMessage 完整 OnPacket 未逐行对齐；线参以服务端 PacketCreator + 9AC732/9AB50A 交叉为准
- Avatar 窗总高与 `H-30` 映射用 `BTN_DY-4` 近似，真机 ±1～2px
- ~~Pet（speaker=2）inset=25~~ → **A11g 已接**（`(speaker&6)!=0` → 25）
