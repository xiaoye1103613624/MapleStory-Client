# Messenger 关闭钮坐标（BtClose3 / IDA）

> 状态：代码已闭环；真机 → pending-live-tests  
> 日期：2026-11-06  
> 产物：`docs/features/messenger-btclose-coords/`

## 1. 选型

本轮先清 atlas_dbg 工程债后，闭合 open-issues B12-D / Messenger 关闭钮。  
跳过：婚礼/嘉年华、Title、DualMobGauge、step4、B11。C9/G8/G9 证据不足未动。

## 2. 证据

| 源 | 结论 |
| --- | --- |
| IDA Maplestory95 `CUIMessenger::SetCtrl` `0x7F34C0` | `m_nState!=2`：`CCtrlButton::Create(..., x=276, y=6)`；资源串 `UI/Basic.img/BtClose3`（SP_6498） |
| 同函数 `m_nState==2` | 最小化态 `(191, 3)` —— 本端无 Min 态，不接 |
| 本包 `Basic.img` | **无可用 BtClose3**（r15-closebtn）；`close_btn()`→`BtClose` 12×12 同族实心蓝叉 |
| 旧实现 | `dim.x()-21`(=274) + 后备 `BtUIClose` 32×15 ⇒ 越窗；且 **BtUIClose 非原生 Messenger 件** |

**【已证实】** 展开态关闭钮立即数 `(276,6)`，件为 `BtClose3`。  
**【差异】** 本端曾用 `dim-21`/`BtUIClose`；本包缺 BtClose3。  
**【推荐】** 坐标按 IDA；资源优先 BtClose3，否则 `close_btn()`。无待决项。

## 3. 实现

- `UIMessenger.h`：`BT_CLOSE_X/Y = 276/6`
- `UIMessenger.cpp`：探测 `Basic.img/BtClose3` → `close_btn()`；去掉 `BtUIClose` 后备

## 4. 待测

开 Messenger → 标题栏右上关闭钮应完整落在窗内、可点关闭。
