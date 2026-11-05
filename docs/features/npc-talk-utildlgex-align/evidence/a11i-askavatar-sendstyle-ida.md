# A11i AskAvatar / sendStyle（msgtype 7/8）— IDA 取证

> IDB：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64`  
> 日期：2026-11-05

## 1. OnPacket

| 符号 | 地址 | 说明 |
| --- | --- | --- |
| AskAvatar | `sub_74713D` | msgtype **7**；读 `string` + `byte n` + `n×int` |
| MembershopAvatar | `sub_74730B` | msgtype **8**；字段同 7 |
| SetUtilDlgEx | `sub_9AC552(..., a2=5)` | 内部 **dlgType=5** |
| SetAvatarStyles | `sub_9AC92C` | 写入 style 向量；`id/10000`→类别 |
| Layout | `sub_9A92C9` | 固定钮槽 |
| OnButton | `sub_9AB92A` case 5 | Prev/Next/Off/On/OK |
| 窗高 | `sub_9A3CED` | dlgType5 → **306** |
| 窗宽 | `sub_9A3CAE` | dlgType5/6 → **419** |

## 2. 类别（`sub_9AC92C`）

| `style_id / 10000` | this+65 | `sub_9AA69E` 写入 |
| --- | --- | --- |
| 2 | 0 | face |
| 3 | 1 | hair |
| else | 2 | skin（nameTag 跳过） |

默认 `this+67 = 1`（显示新样式预览）。

## 3. 布局（`sub_9A92C9`，窗原点）

| 控件 | StringPool / 节点 | 位置 |
| --- | --- | --- |
| BtPrev | 2268 `UtilDlgEx_Avatar/BtPrev` | (15, 195) id=0x2000 |
| BtNext | 2269 `…/BtNext` | (341, 195) id=8193 |
| Yes | 1257 `Basic.img/BtOK2` | (314, 281) id=1 |
| No | 1258 `Basic.img/BtCancel2` | (363, 281) id=2 |
| Exit | 2724 `…/BtExit` | (9, 281) id=2 |
| BtOff | 2271 `…/BtOff` | (172, 281) id=8194 |
| BtOn | 2272 `…/BtOn` | (172, 281) id=8195 |
| nameTag | 2270 | (123, 246) 173×19 |
| 预览脚点 | — | (207, 245) |
| 正文 | — | left=110（`sub_9ACC20` a2≠0） |

底图：`UIWindow.img/UtilDlgEx_Avatar/backgrnd` **419×306**（非 t/c/s 三段）。

互斥：`show_new==1` → 仅 BtOff；`==0` → 仅 BtOn。

## 4. 出站 NPC_TALK_MORE(60)

**AskAvatar OK**（result==1）：`byte 7, byte 1, byte selection`  
**AskAvatar Close**（result==3）：IDA **静默不发包**  
**AskAvatar 其它非 OK**：`byte 7, byte 0`  
**MembershopAvatar**：恒发包；OK=`8,1,sel`；否则 `8,0`

### 本仓适配

统一 Cancel/Exit/ESC → `(msgtype, 0)`，证据对齐 MembershopAvatar + s9 `NPCMoreTalkHandler`，避免脚本挂死。OK 严格按 IDA。

## 5. 服务端交叉

`PacketCreator.getNPCTalkStyle` → msgtype 7 + styles[]。  
`NPCMoreTalkHandler`：`available()>0` 时读 unsigned byte 为 selection。
