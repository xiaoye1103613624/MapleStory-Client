# A11j AskPet / AskPetAll / Mirror — 预留说明

> 日期：2026-11-05 ｜ 本轮优先 AskBoxText；此项证据不足完整可交互 UI，继续吞包 `-1`。

## AskPet (9) / AskPetAll (10)

| | AskPet `7474A2` | AskPetAll `74775C` |
| --- | --- | --- |
| 入站 | string + byte n + n×byte（槽位索引） | string + byte n + byte flag + n×byte |
| UI | `9AC552(..., a2=6)` UtilDlgEx dlgType=6 | 同族 |
| 出站 OK | lastMsg=9，action=1，再写 pet 相关 8 字节（`46C00C`） | 待完整反编译确认 |
| 空列表 | action=1 或 2 早退 | — |

阻塞：需本机宠物栏反查（`46D301` 等）与 dlgType=6 列表布局（`9ACB27`）IMG/IDA 对照；无宠物运行态时无法验收。本轮不臆造选宠 UI。

## Mirror (14)

| | |
| --- | --- |
| 入站 | `76B5C8`：`int` + `string`（s9 `getDimensionalMirror`：int(0)+string） |
| UI | **非** UtilDlgEx；独立 ctor `76B3F0` / 列表填充 `76C781` 等 |
| 出站 | 未在本轮钉死（需跟选择/关闭 xref） |

阻塞：维度镜独立窗体资源与选择出站未取全。继续 `-1` 防挂死。
