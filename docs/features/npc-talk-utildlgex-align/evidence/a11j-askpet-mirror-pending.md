# A11j AskPet / AskPetAll / Mirror — 预留说明（已由 A11l 接续）

> 日期：2026-11-05 ｜ **已由 [a11l-askpet-mirror-ida.md](a11l-askpet-mirror-ida.md) 接续接线**。

## AskPet (9) / AskPetAll (10)

| | AskPet `7474A2` | AskPetAll `74775C` |
| --- | --- | --- |
| 入站 | string + byte n + **n×(long+byte)** | string + byte n + byte flag + n×(long+byte) |
| UI | `UIAskPet`（UtilDlgEx 壳，可证子集） | 同 |
| 出站 OK | lastMsg + action=1 + int64 | 同 |
| 仍预留 | dlgType=6 完整宠立绘/亲密条；现金栏 64 位 uniqueId 入库 | 同 |

## Mirror (14)

| | |
| --- | --- |
| 入站 | `76B5C8`：`int` + `string` |
| UI | `UIDimensionalMirror`（`SlideMenu/0`，235×165） |
| 出站 | lastMsg=14 + action + int selection |
| 仍预留 | `IsAniBackgrd` 动画底；BtMain 全态打磨 |

详见 a11l。
