# A11j AskBoxText（msgtype 13）— IDA 取证

> IDB：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64`  
> 日期：2026-11-05

## 1. 入站

| 符号 | 地址 | 说明 |
| --- | --- | --- |
| OnScriptMessage case 13 | `sub_746C46` | 读包 + 开 UtilDlgEx |
| 布局分发 dlgType=8 | `sub_9A6370` → `sub_9A8910` | 多行框 |
| 参数写入 | `sub_9AC811` | `this+224=cols`、`this+228=rows` → `sub_9ACDF3` |
| OK 校验 | `sub_9AB50A` dlgType==8 | 仅拷贝 Edit 文本，**无** minLen tip |

包尾（公共头之后）：

1. `string` talk  
2. `string` def  
3. `short` cols  
4. `short` rows  

与 GetText（`746A8B`）字段同形；差异：`9AC552(..., a2=8)`（GetText 为 `a2=3`），随后 `9AC811` 而非 `9AC7CB`。

## 2. 布局（非 Avatar，`sub_9A8910`）

| 元素 | 公式 |
| --- | --- |
| 外宽 `v3` | `6*cols + 4` |
| 高 `v43` | `12*rows + 2` |
| Edit(1003) | `(left+2, top+text_h+5)`，`w=v3-2`，`h=v43` |
| 黑框 | `(left-1, top+text_h+2)`，`w=v3+3`，`h=v43+4`，色 `0xFF000000` |
| 白底 | `(left, top+text_h+3)`，`w=v3+1`，`h=v43+2`，色 `0xFFFFFFFF` |
| BtOK | `(W-54, H-26)` |
| BtClose | `(9, H-26)`（`speaker&1==0`） |
| 容量 | `cols * rows`（CCtrlEdit 参数） |

Avatar 分支（`this+164!=0`）：Edit `(19, H-73)` 尺寸 `(v3,v43)`；BtOK `(176,H-30)`；BtClose `(218,H-30)`。

高度预留 `sub_9ACDF3`：`rows>1` 时正文垂直居中扣除 `12*rows`。

## 3. 出站（`746C46`）

| 结果 | 包体 |
| --- | --- |
| OK (`result==1`) | `byte 13` + `byte 1` + `string` |
| 非 OK 且非 Close | `byte 13` + `byte 0` |
| Close (`result==3`) | **静默**（与 GetText IDA 同） |

本仓 Close/Esc 仍发 `(13,0)`，与 GetText 客户端路径一致，避免 s9 脚本挂死（对照 A11i Avatar Cancel）。

## 4. 与 s9 交叉

- `PacketCreator` **无** AskBoxText API（本轮补 `getNPCTalkBoxText` + `sendAskBoxText`）。  
- `NPCMoreTalkHandler` 原仅 `lastMsg==2` 读 `returnText`；须含 **`13`**，否则 `(13,1,string)` 会被当成 selection。  
- Quiz/SpeedQuiz 另文；本条不改 Quiz。

## 5. 局限

- 本仓 `Textfield` 单行：框高按 `rows` 放大，多行换行/↩ 插行未接（↩ 仍提交）。  
- `cols/rows<=0` 时钳到 `1`（防不可用零尺寸；原生按原值公式）。
