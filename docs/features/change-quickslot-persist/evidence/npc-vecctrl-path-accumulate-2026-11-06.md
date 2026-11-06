# NPC 攒 path — VecCtrl / CMovePath 调用链（2026-11-06 续）

IDB：`E:\MXD\Maplestory95.i64`（idalib session `2f07cae6`）

## 触发（谁调 GenerateMovePath）

`CNpc::Update` @ `0x677b50`：

| 条件 | 含义 |
| --- | --- |
| `m_pvcActive` 非空 | 有活动 VecCtrl |
| `m_bMovePathSent == 0` | 本段尚未发过 path |
| `m_pTemplate->bMove != 0` | 可移动 NPC |
| `CVecCtrl::UpdatePassive` 返回 **`-1`** | 被动更新宣告「该发 path」 |

然后：`DoActionOrChat`（可填 nAction/nChatIdx）→ **`GenerateMovePath`**。

非移动 NPC（`bMove==0`）走另一支：仅 `DoActionOrChat` 成功才 `GenerateMovePath`（无 path body）。

## GenerateMovePath 内攒 path（已钉死）

源：`CNpc::GenerateMovePath` @ `0x671590`

1. `m_pvcActive` → `CVecCtrlNpc*`
2. **可选**（`nAction != -1`）：`GetActionFrameList` 累加 delay → `CVecCtrlNpc::UpdateActiveInterrupted(tElapse, v=0)` @ `0x99e020`
3. 循环 ≤ **55** 次：
   - `CMovePath::IsTimeForFlush(bFly=0, bDynamicFoothold=0)` @ `0x666870` → 真则 **break**
   - 否则 `CVecCtrl::UpdateActive` @ `0x990980`（虚表：Begin → Work → End）
4. 组包 op **241** / s9 **197**：`oid + nAction + nChatIdx`
5. `bMove`：`CMovePath::Flush` @ `0x668160` → `Encode` @ `0x666e20`
6. `SendPacket`；`m_bMovePathSent = 1`

## IsTimeForFlush 条件（钉死）

`CMovePath::IsTimeForFlush` @ `0x666870`：

```
m_lElem 空 → false
阈值 = m_bShortUpdate ? (bDynamicFoothold ? 200 : 500) : 1000
m_tGatherDuration < 阈值 → false
若 !m_bShortUpdate && !bFly：
  从 tail 向前找 fh > 0 的 ELEM；找不到 → false；找到 → true
否则 → true
```

NPC 路径：`bFly=0`、通常非 ShortUpdate → **需攒满 ≥1000ms 且至少一节 fh>0**。

## ELEM 如何进列表（钉死）

```
CVecCtrl::SetMovePathAttribute(nAttr) @ 0x52b000
  → 读 AbsPos x/y/vx/vy、m_pfh、ladder、m_nMoveAction
  → CMovePath::MakeMovePath @ 0x667c90
       m_tGatherDuration += tElapse
       fh = pfh->SN 或 -ladder.SN 或 0
       合并/AddTail ELEM（同 attr/fh/action/速度方向可合并 tElapse）
       写 nAttr,x,y,vx,vy,fh,fhFallStart,bMoveAction,offsets,rand
```

`UpdateActiveInterrupted` 在有 foothold 时先 `SetMovePathAttribute(2)`，再按 30ms 切片做相对坐标物理并虚调（vfptr[5]/[6]）继续推进。

`MakeMovePath` 其它调用方：`MakeContinuousMovePath`（User/基类）、Pet `WorkUpdateActive`——同族编码。

## Flush 何时真 Encode

`CMovePath::Flush`：清 gather；可选把尾部未完成节挪到临时表；若 forced / attr≠0 / 与 `m_elemLast` 坐标速度 fh action 有差 → 标记需 Encode；再 `Encode`；写回 secure x/y/vx/vy；清空 keyPad；还原未发完节。

## 伪代码落盘

| 文件 | 地址 |
| --- | --- |
| `vecctrl-0x99e020.decompiled.txt` | UpdateActiveInterrupted |
| `vecctrl-0x990980.decompiled.txt` | UpdateActive |
| `vecctrl-0x666870.decompiled.txt` | IsTimeForFlush |
| `vecctrl-SetMovePathAttribute-0x52b000.decompiled.txt` | SetMovePathAttribute |
| `cmovepath-makemovepath-0x667c90.decompiled.txt` | MakeMovePath |
| `cnpc-update-0x677b50.decompiled.txt` | CNpc::Update 触发 |

## 接线门禁（仍缺）

| # | 项 | 状态 |
| --- | --- | --- |
| A–B | 包形 / 尾 9 | ✅ 上轮 |
| C1 | 攒 path **条件**（本轮） | ✅ 已钉死 |
| C2 | 本端 `CVecCtrlNpc` 物理 + MakeMovePath 等价实现 | ❌ 无 |
| D | 真机 c2s `197` `length>6` 十六进制样例 | ❌ 未抓（本轮仅见 talk：`c5 00 … ff 00`） |
| E | 双开观感 | ❌ |

**结论**：条件证据已够写解码器/对照表；**仍禁止**发明上行 move body。下一刀：抓走动 NPC 的 `197 length>6`，或评估只做下行 `Encode` 解析位移（不仿 VecCtrl 上行）。
