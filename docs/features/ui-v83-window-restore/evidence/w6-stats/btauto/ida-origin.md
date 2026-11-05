# Stat/BtAuto 落点取证（2026-11-05）

## IDA（Maplestory95.i64 · session `3b554971` · headless 13338）

- `CUIStat::OnCreate` = `0x867b90`（size `0x948`）
- Auto 族：`AddButton` id `0x7D7/0x7D8/0x7D9` → `m_pBtAutoApUp[0..2]`
- 反汇编实证（`0x867ebb`）：`push 0 ; nOffsetY` / `push 0 ; nOffsetX` / `push 7D7h` / `push offset aUi_221`
- UOL 宽串：`UI/UIWindow2.img/Stat/main/BtAuto`（及 BtAuto1/2）
- `CLayoutMan::AddButton` `0x5cf210`：把 `(nOffsetX+m_nOffsetX, nOffsetY+m_nOffsetY)` 交给 `CCtrlOriginButton::CreateCtrl`
- `CCtrlOriginButton::GetRect`：`OffsetRect(-get_cx, -get_cy)` ⇒ Create(0,0) 时控件可视原点 = **−canvas.origin**

## IMG（`BeiDou-Client-xy/REA反编译/wz_dump/UIWindow2_img.json`）

| 节点 | WxH | origin | topleft(=−origin) |
|---|---|---|---|
| `Stat/main/BtAuto` | 67×34 | (−94, −198) | **(94, 198)** |
| `Stat/main/BtAuto1` | 68×16 | (−93, −198) | (93, 198) |
| `Stat/main/BtHpUp` | 12×12 | (−147, −121) | (147, 121) |
| `Stat/main/BtStrUp` | 12×12 | (−147, −244) | (147, 244) |

## 经典窗（本包 UIWindow.img/Stat · 208×347）映射

- ApUp y 几乎同位（现代 STR 244 ↔ 经典 245）⇒ **y=198** 直取
- 本包 `BtAuto` = **100×35**（nx 导出 opaque 满幅）→ 现代 (94,198) 按 ApUp Δx=+39 得 (133,198) 右缘越界
- 定案 **(106, 198)**：`x = 208 − 100 − 2`（窗右缘留 2px，与 BtApUp 贴绿格右缘同量级）

## 截图负证

`btauto/sad_match.txt`：在 s1/native_stat 等新手帧上 SAD≥53，假峰（`basicStat` 遮挡）。

## SPAWN_PET 核对（本轮顺带）

`Net/Packets/PetPackets.h` + `UIItemInventory.cpp` 已发 `SPAWN_PET(98)` —— W1 真接，非缺口。
